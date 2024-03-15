# Prime Number Generator

## Overview

This program leverages a distributed system to efficiently generate prime numbers within a designated range. By allocating tasks among multiple processes, it achieves parallel processing, significantly boosting performance. The architecture encompasses several core components: a central controller (main.c), a task delegator (`delegator.c` and `delegator.h`), individual workers (`worker.c` and `worker.h`), and a prime number computation unit (`prime.c`). It facilitates task management and coordination across various processes using pipes and signals for inter-process communication.

## Features

- **Distributed Computation:** Divides the prime number generation task across multiple worker processes.
- **Flexible Distribution:** Supports even and random distribution of workload among worker processes.
- **Inter-Process Communication:** Utilizes pipes for data transfer and signals for synchronization.
- **Performance Metrics:** Gathers and displays execution time statistics to evaluate performance.

## Compilation

The project includes a `Makefile` for easy compilation. Use the following command to compile all components:

 ```bash
 make
```

## Usage

After compilation, run the program with the following syntax:

 ```bash
 ./primes -l LowerBound -u UpperBound -[e|r] -n NumOfNodes
```


- `-l LowerBound`: The lower bound of the range within which to find prime numbers.
- `-u UpperBound`: The upper bound of the range.
- `-e` or `-r`: Distribution mode (`e` for even, `r` for random).
- `-n NumOfNodes`: The number of worker nodes (processes) to deploy.

## Example

 ```bash
 ./primes -l 1 -u 100 -e -n 4
```


This command will find prime numbers between 1 and 100, distributing the workload evenly across 4 worker nodes.

## Implementation Details

- **Main Controller (`main.c`):** Parses command-line arguments, sets up signal handlers and orchestrates the overall execution.
- **Delegator (`delegator.c`, `delegator.h`):** This process manages worker processes and distributes the workload based on the specified strategy.
- **Worker (`worker.c`, `worker.h`):** Executes the prime number finding task within the assigned range and reports results back to the delegator.
- **Prime Calculation (`prime.c`):** Contains the logic for determining if a number is prime.

## Dependencies

- This project uses standard C libraries only.


