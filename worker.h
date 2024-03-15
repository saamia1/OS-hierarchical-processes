#ifndef WORKER_H
#define WORKER_H
#include <unistd.h>
#define NUM_WORKERS 100

typedef struct WorkerTime { //structure for storing the worker nodes time
    double realTime;
    double cpuTime;
} WorkerTime;

extern WorkerTime workerTimes[NUM_WORKERS]; // Declaration
extern int idx; // Declaration

void computeAndDisplayStats(int n);
void worker(pid_t root, int sig, int lb, int ub);

#endif // WORKER_H
