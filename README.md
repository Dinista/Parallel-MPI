# Parallel-MPI

## Introduction

C implementation of the __master-worker algorithm__ model of __Sum.c__ program, executed in __parallel__ using <a href= "https://pt.wikipedia.org/wiki/Message_Passing_Interface">__MPI__</a>, designed to analyze and measure its performance.

__In the <a href="https://github.com/Dinista/Parallel-MPI/tree/main/Docs">"Docs"</a> folder, there is an article in Portuguese evaluating the execution performance of the algorithm with different inputs, calculating a <a href="https://en.wikipedia.org/wiki/Speedup">speed up graph</a>.__

## How it works

The program reads a __list of "tasks" from a file__, where each task includes a character code indicating an __action and a corresponding number__. The character code can be either __"p"__ (for "process") or __"e"__ (for "wait"). The input file simulates different workloads entering a multiprocessing system.

In an actual system, the "p" actions (tasks) would typically represent __calls to computational routines__. In this scenario, "processing" a task with number __n__ simply involves __waiting for n seconds using the sleep function__ and __then updating specific global aggregate variables__ (such as sum, odd count, min, and max). The "e" action serves as a means to __simulate a pause in incoming tasks__.
