#include "worker.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <signal.h>
#include <string.h>
#define NUM_WORKERS 100

WorkerTime workerTimes[NUM_WORKERS]; // Definition

void computeAndDisplayStats(int n) {
  double maxReal = 0, minReal = 0, avgReal = 0;
  double maxCpu = 0, minCpu = 0, avgCpu = 0;

  for (int i = 0; i < (n*n); i++) 
  {
      if (workerTimes[i].realTime > maxReal) maxReal = workerTimes[i].realTime;
      if (workerTimes[i].realTime < minReal) minReal = workerTimes[i].realTime;
      avgReal += workerTimes[i].realTime;

      if (workerTimes[i].cpuTime > maxCpu) maxCpu = workerTimes[i].cpuTime;
      if (workerTimes[i].cpuTime < minCpu) minCpu = workerTimes[i].cpuTime;
      avgCpu += workerTimes[i].cpuTime;
  }
  if (avgReal != 0){avgReal /= (n*n);}
  if (avgCpu != 0) {avgCpu /= (n*n);}

  printf("Real Time - Max: %lf, Min: %lf, Avg: %lf\n", maxReal, minReal, avgReal);
  printf("CPU Time - Max: %lf, Min: %lf, Avg: %lf\n", maxCpu, minCpu, avgCpu);
  printf("\n");
}

void worker(pid_t root, int sig, int lb, int ub) {
  int idx = 0; // Definition
  double t1, t2, cpu_time;
  struct tms tb1, tb2;
  double ticspersec = (double) sysconf(_SC_CLK_TCK);
  t1 = (double) times(&tb1);

  if (sig == 0) {
    kill(root, SIGUSR1);
  }
  else if(sig == 1){
    kill(root, SIGUSR2);
  }

  //main work being done
  char lb_str[20], ub_str[20];
  sprintf(lb_str, "%d", lb);
  sprintf(ub_str, "%d", ub);
  char *args[] = {"./prime", lb_str, ub_str, NULL};
  execvp(args[0], args);
  perror("execvp");
  exit(EXIT_FAILURE);

  t2 = (double) times(&tb2);
  cpu_time = (double)((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
  
  // Store the times in the workerTimes array
  workerTimes[idx].realTime = (t2 - t1) / ticspersec;
  workerTimes[idx].cpuTime = cpu_time / ticspersec;
  idx++;
}

