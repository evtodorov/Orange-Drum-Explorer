# Performance optimization of Orange-Drum-Explorer
This file outlines the process of profiling  the library and optimizing the performance, as well as showcasing typical performance optimization for scientific code as taught in the context of  IN1503 W20/21

## Reference envrionment

The optimization is carried out on the following system:

### OS
Linux Mint 20 Cinnamon  
Linux Kernel: 5.4.0-52-generic #57-Ubuntu SMP Thu Oct 15 10:57:00 UTC 2020

### Compiler
g++ 9.3.0

### Hardware
CPU: Intel(R) Core(TM) i5-3320M CPU @ 2.60GHz  (2(4) cores @ 2.60 GHz - 3.30 GHz)  
Cache: 128KiB L1/ 512KiB L2/ 3MiB L3  
Memory: 8GiB (2x4GiB SODIMM DDR3 Synchronous 1600 MHz)  
Instruction sets: x86_64 SSE/AVX


## Scenarios 
Three scenarios are used as reference for the perfromance optimization of the code:

1. Light-weight function integrated with the Explicit Euler method.  
    This scenario is used to optimize the Explict Euler solution method

1. Light-weight function integrated with the Implicit Euler method with Automated differentiaon.  
    This scenario is used to optimize the Implicit Euler solution method

1. Computationally intesive function integrated with the Explicit Euler method.  
    This scenario is used to showcase optimization techniques which are not suitable to application in either solution algorithm (e.g. vectorization, which cannot be applied due to the depdency of each computed value on the previously computed value. In most cases for both algortihms, the innermost loop is over the relatively short vector of 0-th, 1st, second, etc. derivatives.) 

## Baseline

### Scenario 1

### Scenario 2

### Scenario 3

## Implemented Optimizations