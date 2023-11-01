# Parallel-MPI

## Introduction

C implementation of the __master-worker algorithm__ model of __Sum.c__ program, executed in __parallel__ using <a href= "https://pt.wikipedia.org/wiki/Message_Passing_Interface">__MPI__</a>, designed to analyze and measure its performance.

__In the <a href="https://github.com/Dinista/Parallel-MPI/tree/main/Docs">"Docs"</a> folder, there is an article in Portuguese evaluating the execution performance of the algorithm with different inputs, calculating a <a href="https://en.wikipedia.org/wiki/Speedup">speed up graph</a>.__

## How it works

The program reads a __list of "tasks" from a file__, where each task includes a character code indicating an __action and a corresponding number__. The character code can be either __"p"__ (for "process") or __"e"__ (for "wait"). The input file simulates different workloads entering a multiprocessing system.

In an actual system, the "p" actions (tasks) would typically represent __calls to computational routines__. In this scenario, "processing" a task with number __n__ simply involves __waiting for n seconds using the sleep function__ and __then updating specific global aggregate variables__ (such as sum, odd count, min, and max). The "e" action serves as a means to __simulate a pause in incoming tasks__.

### Input

For instance, the given input represents an initial one-second task entering the system, followed by a two-second delay. After the delay, a two-second task is introduced, followed by a three-second task.

After the delay, a two-second task enters the system followed by a three-second task.

```
p 1
w 2
p 2
p 3
```
### Output

The final output should match the following (sum, # odd, min, max):

```
6 2 1 3
```

## How to use

__The test files must be in the same folder as the program.__

__Enter the file name when prompted (if it has an extension, enter that too).__

Compile with make:

```console
Make
```
or

```console
mpicc mpi_sum.c -o mpi_sum
```
Run:
```console
mpirun -n <num trabalhadores> ./mpi_sum
```
Example:

```console
mpirun -n 5 ./mpi_sum
```
### Dependecies

MPI version: 3.3.2 or higher
