# Performance optimization of Orange-Drum-Explorer
This file outlines the process of profiling the Solver library and optimizing the performance, as well as showcasing typical performance optimization for scientific code as taught in the context of  IN1503 W20/21

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

The baseline measurements are conducted at commit `45babd1` (tagged as `baseline`).

### Instrumentation

A baseline performance is established for each of the scenarios at several optimization levels, created by a combined [Makefile]() and storing the results in 4 different directories by running `make`. The code is executed to collect profiling information by `make runall`. The profiling information is converted to report for each separate scenario. Additionally, any optimization work done by the compiler is logged using the `-fopt-info` option.

The complexity of the function and the number of steps are chosen to be at roughly 1s at baseline and are kept constant through the optimization.

### Compiler optimization levels
[debug/]() is run with compiler option `-Og` as recommended in g++ documentation and is the most used in optimization.  
[not_optimized/]() uses option `-O0`, however due to the use of the adept and eigen libraries, which are designed to be run with optimizing options, the profiling information is heavily polluted and not usable to identify bottlenecks in the Solver library.  
[optimized/]() is run with `-O1` (and potentially custom single -fopt flags) to identify optimizing techniques which improve the performance of the Solver library  
[fully_optimized/]() uses `-O3` to provide an upper bound on the perfromance improvement.

### Baseline Performance
| Compiler Optimization | debug | not_optimized | optimized | fully_optimized |
|-----------------------|------:|--------------:|----------:|----------------:|
| Scenario 1            | 1.2 s |   16.3 s      |   0.98 s  |      0.92 s     |
| Scenario 2            | 1.3 s |   60.3 s      |   0.89 s  |      0.85 s     |
| Scenario 3            | 1.1 s |   166.3 s     |   0.75 s  |      0.70 s     |

### Identified Bottlenecks

The output of the `gprof` tool is used to identify bottlenecks.

**Scenario 1** is profiled to spend about one-third of the time running the lambda function containing the differential equation, about another third is spent, as expected, in the `EulerExplicit::solve` function. The last third is split equally between a vector copy function, and a couple of the functions of the adept module, both called by `EulerExplicit::solve`.

**Scenario 2** is dominated by the `EulerImplicit::NewtonSolve` function, which is in turn dominated by the initialization of the automatic differentioan stack `adept::Stack::initialize` and the LU-decomposition `Eigen::FullPivLU` of the Jacobian matrix.

**Scenario 3** is, as constructed, more than 95% dominated by the computationally intensive toy function.

## Implemented Optimizations
### Templated adept adouble function
### Vectorized masked accumulation