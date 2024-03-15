#include "delegator.h"
#include "worker.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// defining global variables
#define MAX_LENGTH 4096
#define NUM_WORKERS 100
#define read_end 0
#define write_end 1

// for keeping the count of signals received
int usr1_received = 0;
int usr2_received = 0;

// signal handlers
void sigusr1_handler(int signum) { usr1_received++; }
void sigusr2_handler(int signum) { usr2_received++; }

// function declarations
void computeAndDisplayStats(int n);
int setNonBlocking(int fd);
void sortAndPrintNumbers(const char *input);

int main(int argc, char *argv[]) {
  srand(time(NULL)); // setting random seed to null
  pid_t rootpid = getpid();
  char distribution = 'e'; // default equal distribution
  // initialising variables to read into by user
  int lb = 0, ub = 0, n = 0;
  int totalRange = 0;
  int allocatedRange = 0;

  // assigning user defined signals to signal handlers
  signal(SIGUSR1, sigusr1_handler);
  signal(SIGUSR2, sigusr2_handler);

  // reading user input
  if (argc != 8) {
    fprintf(stderr,
            "Usage: %s -l LowerBound -u UpperBound -[e|r] -n NumOfNodes\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  // For receiving all the input conditions
  for (int i = 1; i < argc; i += 2) {
    if (strcmp(argv[i], "-l") == 0) {
      lb = atoi(argv[i + 1]);
    } else if (strcmp(argv[i], "-u") == 0) {
      ub = atoi(argv[i + 1]);
    } else if (strcmp(argv[i], "-n") == 0) {
      n = atoi(argv[i + 1]);
    } else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "-r") == 0) {
      distribution = argv[i][1];
      i--;
    }
  }
  pid_t pids[n]; // storing the child pids for later synchronization use

  if (lb >= ub || n <= 0) { // verifying for invalid cases
    fprintf(stderr, "Invalid input. Make sure lb < ub and n > 0.\n");
    exit(EXIT_FAILURE);
  }

  totalRange = ub - lb + 1;
  int ptc[n][2],
      ctp[2]; // pipes for parent-to-child and child-to-parent communication

  if (pipe(ctp) == -1) { // checking for proper initialization of the ctp pipe
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  // setting the ctp pipe to non-blocking mode
  setNonBlocking(ctp[read_end]);
  setNonBlocking(ctp[write_end]);

  for (int i = 0; i < n; ++i) {
    if (pipe(ptc[i]) ==
        -1) { // checking for proper initialization of the ptc pipes
      perror("pipe");
      exit(EXIT_FAILURE);
    }

    // making the chunkes of the range for prime numbers calculation
    int start, end, chunk_size, remaining;
    chunk_size = totalRange / n;
    remaining = totalRange % n;
    start = lb + i * chunk_size;
    end = start + chunk_size - 1;
    if (i == n - 1) {
      end += remaining;
    }

    // forking the child process
    if ((pids[i] = fork()) < 0) {
      perror("fork");
    } else if (pids[i] == 0) { // Child process
      close(ptc[i][write_end]);
      setNonBlocking(ptc[i][read_end]);

      close(ctp[read_end]);
      dup2(ctp[write_end], STDOUT_FILENO); // Redirect stdout to pipe
      close(ctp[write_end]);

      delegator(n, i, start, end, distribution);
      exit(EXIT_SUCCESS);
    } else { // Parent process
      close(ptc[i][read_end]);

      // writing the start and the end of ranges to the delegator processes
      write(ptc[i][write_end], &start, sizeof(start));
      write(ptc[i][write_end], &end, sizeof(end));

      close(ptc[i][write_end]);
    }
  }
  close(ctp[write_end]); // Close the write end after all children are forked

  // Buffer to store data read from the pipe
  char buffer[MAX_LENGTH + 1]; // +1 for null terminator
  ssize_t bytesRead;
  int nread = 0; // Total bytes read

  // Initialize the file descriptor set
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(ctp[read_end], &read_fds);

  
  struct timeval timeout; // Set up a timeout for select
  memset(buffer, 0, sizeof(buffer)); // Clear the buffer

  // Loop to handle non-blocking reads
  while (1) {
    fd_set fds_copy = read_fds; // Make a copy because select() modifies it

    int select_result = select(ctp[read_end] + 1, &fds_copy, NULL, NULL, &timeout);
    
    if (select_result == -1) {
      if (errno == EINTR) {continue;} 
      else {
        // An actual error occurred
        perror("select");
        exit(EXIT_FAILURE);
      }
    } 
    else if (select_result == 0) {
      printf("Timeout while waiting for data.\n");
      break;
    } 
    else {
      if (FD_ISSET(ctp[read_end], &fds_copy)) {
        bytesRead = read(ctp[read_end], buffer + nread, MAX_LENGTH - nread);
        if (bytesRead > 0) {
          nread += bytesRead;
          buffer[nread] = '\0'; // Ensure buffer is null-terminated
        } 
        else if (bytesRead == -1 &&
                   (errno == EAGAIN || errno == EWOULDBLOCK)) {
          break; // Or handle as needed
        } 
        else if (bytesRead == 0) {break;} 
        else {
          perror("read");
          break;
        }
      }
    }
  }
  sortAndPrintNumbers(buffer);
  computeAndDisplayStats(n);
  printf("The number of times sigusr1 was received: %d\n", usr1_received);
  printf("The number of times sigusr2 was received: %d\n", usr2_received);

  // Wait for all children to exit before reading
  for (int i = 0; i < n; ++i) {
    waitpid(pids[i], NULL, 0);
  }

  return 0;
}
void sortAndPrintNumbers(const char *input) {
  int *numbers;
  int numCount = 0;
  char *inputCopy = strdup(input);
  char *token = strtok(inputCopy, " ");

  while (token) {
    numCount++;
    token = strtok(NULL, " ");
  }
  // Allocate memory for numbers
  numbers = (int *)malloc(numCount * sizeof(int));
  if (!numbers) {
    perror("Failed to allocate memory");
    free(inputCopy);
    return;
  }
  // Second pass: parse and store numbers
  int i = 0;
  token = strtok(input, " ");
  while (token) {
    numbers[i++] = atoi(token);
    token = strtok(NULL, " ");
  }
  // Simple insertion sort
  for (int i = 1; i < numCount; i++) {
    int key = numbers[i];
    int j = i - 1;

    while (j >= 0 && numbers[j] > key) {
      numbers[j + 1] = numbers[j];
      j = j - 1;
    }
    numbers[j + 1] = key;
  }

  // Print sorted numbers
  for (int i = 0; i < numCount; i++) {
    printf("%d ", numbers[i]);
  }
  printf("\n");
  free(numbers);
  free(inputCopy);
}

// Function to extract the first integer from a string
int setNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1)
    return -1;
  flags |= O_NONBLOCK;
  return fcntl(fd, F_SETFL, flags);
}