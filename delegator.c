#include "delegator.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "worker.h"

void delegator(int n, int sig, int lb, int ub, char distribution) {
  pid_t pids[n];
  int pipefds[n][2];
  pid_t root = getppid();

  int totalRange = ub - lb + 1;
  int allocatedRange = 0;
  for (int i = 0; i < n; ++i) {
    int start = 0, end = 0, chunk_size = 0, remaining = 0;
    if (distribution == 'e') { // Even distribution
      chunk_size = (ub - lb + 1) / n;
      remaining = (ub - lb + 1) % n;
      start = lb + i * chunk_size;
      end = start + chunk_size - 1;
      if (i == n - 1) { end += remaining;}

    } else if (distribution == 'r'){ // Random distribution 
      int maxPossibleSize = totalRange - allocatedRange;
      chunk_size = (rand() % maxPossibleSize) + 1;
      start = lb + allocatedRange;
      end = start + chunk_size - 1;
      if (i == n - 1) {
        end = ub;
      }
      allocatedRange += chunk_size;
    }

    if (pipe(pipefds[i]) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }

    if ((pids[i] = fork()) < 0) {
      perror("fork");
    } else if (pids[i] == 0) // child process
    {
      close(pipefds[i][0]);               // Close unused read end in child
      dup2(pipefds[i][1], STDOUT_FILENO); // Redirect stdout to pipe
      close(pipefds[i][1]);

      worker(root, sig%2, start, end);
      exit(EXIT_SUCCESS);
    } else if (pids[i] > 0) { // parent process
      close(pipefds[i][1]);   // close write end immidiately after forking
    }
  }

  // Wait for all children to exit before reading
  for (int i = 0; i < n; ++i) {
    waitpid(pids[i], NULL, 0);
  }

  // Read from pipes after all children have finished
  for (int i = 0; i < n; ++i) {
    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(pipefds[i][0], buffer, sizeof(buffer) - 1)) > 0) {
      buffer[bytesRead] = '\0';
    }
    close(pipefds[i][0]); // Close read end after finishing reading
    printf("%s", buffer);
  }
}
