# Prime Number Generator

## Overview

This project implements a distributed approach to generate prime numbers within a specified range. It uses multiple processes to divide the workload, enhancing performance through parallel computation. The project consists of several key components, including a main controller (`main.c`), delegator (`delegator.c` and `delegator.h`), worker (`worker.c` and `worker.h`), and a prime number calculation module (`prime.c`). It employs inter-process communication through pipes and signals to manage and synchronize tasks across different processes.

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


