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
Main memory bandwidth (STREAM measurement): ~ 13 GB/s
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

A baseline performance is established for each of the scenarios at several optimization levels, created by a combined [Makefile](Makefile) and storing the results in 4 different directories by running `make`. The code is executed to collect profiling information by `make runall`. The profiling information is converted to report for each separate scenario. Additionally, any optimization work done by the compiler is logged using the `-fopt-info` option.

The complexity of the function and the number of steps are chosen to be at roughly 1 s at baseline and are kept constant through the optimization.

### Compiler optimization levels
[debug/](debug/) is run with compiler option `-Og` as recommended in g++ documentation and is the most used in optimization.  
[not_optimized/](not_optimized/) uses option `-O0`, however due to the use of the adept and eigen libraries, which are designed to be run with optimizing options, the profiling information is heavily polluted and not usable to identify bottlenecks in the Solver library.  
[optimized/](optimized/) is run with `-O1` (and potentially custom single -fopt flags) to identify optimizing techniques which improve the performance of the Solver library  
[fully_optimized/](fully_optimized/) uses `-O3` to provide an upper bound on the perfromance improvement.

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

**Scenario 3** is, as constructed, more than 95% dominated by the computationally intensive toy function. The loop inside ("collision detection") has not been automaticaally vectorized due to the control flow, as confirmed by the `-fopt-info-all-vec-loop` output.

## Optimizations
A testing branch `optimization_tests` was setup in the configuration tracker to evaluate each change. The commit hashes are used as reference to the code changes.
### Exclude automatic differnetion from Euler Explicit - `4155c98`
In the profiling it was observed that even though the actual functionality for automatic differentiation is turned off in the Explicit Euler solver, the act of instrumentating with the augmented type `adept::adouble` is enough to result in a significant amount of time being spend in the library functions. Since the automatic differentiation is not relevant to the Explicit Euler solver, an initial test was conducted by providing an overloaded `vec& ExplicitEuler::solve(std::function<double(double, const vec&)>, const vec&)` function. 

| Compiler Optimization | debug | not_optimized | optimized | fully_optimized |
|-----------------------|------:|--------------:|----------:|----------------:|
| Scenario 1            | *0.3 s|   *3.4 s      |  *0.13 s  |     *0.13 s     |
| Scenario 2            | 1.3 s |   58.3 s      |   0.93 s  |      0.85 s     |
| Scenario 3            | 1.1 s |  *28.4 s      |   0.67 s  |      0.68 s     |

As seen in the measurement, signficant speedup (denoted with *) has been observed for Scenario 1 - explicit solution with a light-weight function - at all optimization leves. This makes this optimization worthwhile, despite the relatively significant impact on the User Interface required to implement it.

### Avoid vector copy inside Euler Explicit - `e6b7392`
The profiling results of Scenario 1 after the removal of the unneeded automatic differentiaon are dominated by a vector copy operation, which could be eliminated by reordering operations.

| Compiler Optimization | debug | not_optimized | optimized | fully_optimized |
|-----------------------|------:|--------------:|----------:|----------------:|
| Scenario 1            | *0.18s|   *2.5 s      |  *0.09 s  |     *0.09 s     |
| Scenario 2            | 1.3 s |   58.3 s      |   0.93 s  |      0.85 s     |
| Scenario 3            | 1.1 s |   28.4 s      |   0.64 s  |      0.67 s     |

### Pass input function directly and not as lambda - `24cbade`
Following the removal of the vector copy operation, the next dominant entry on profiling was the input lambda function. A test was carried out by transfering from lambda function as originally defined to an explicit function. As seen below, the lambda adds some overhead on the lower optimization levels, but is easily optimized away by the compiler. This optimization is thus not taken onboard.

| Compiler Optimization | debug | not_optimized | optimized | fully_optimized |
|-----------------------|------:|--------------:|----------:|----------------:|
| Scenario 1            | *0.11s|   *0.7 s      |   0.09 s  |      0.09 s     |

### Vectorizing masked accumulation - `c673258`
The `compute` function in scneario3 was consturcted specificaly to allow the investigation of vectorizing techniques. As inteded, the original function was not vectorized by the gcc compiler due to the if-statement.

```
double compute(double t, std::vector<double> y){
    double acceleration = 0;
    for (auto i=0; i<N_rollers; i++){
        if ( abs(roller_locations[i] - y[0] ) < package_length ){
            acceleration += roller_forces[i];
        }
    }
    return acceleration;
}
```

An investigation of possible modifications was carried out using Compiler Explorer [https://godbolt.org/z/9PYErK](https://godbolt.org/z/9PYErK). It turned out that the original definition of the function can be vectorized by ICC, which in general demonstrated much better level of vectorization. One configuration could also be vectorized with gcc. As shown below. 

```
std::vector<double> mask(N_rollers, false);
std::vector<double> accumulator(N_rollers, 0.);
double compute_vector(double t, OrangeDrumExplorer::vec y){
    double acceleration = 0;
    for (auto i=0; i<N_rollers; i++){
        mask[i] =  abs(roller_locations[i] - y[0] ) < package_length;
    }
    for (auto i=0; i<N_rollers; i++){
        accumulator[i] = roller_forces[i]*mask[i];
    }
    acceleration = std::accumulate(accumulator.begin(), accumulator.end(), 0.);
    return acceleration;
}
```

Despite the larger number of FLOP executed in `compute_vector` (to multiply by 0 and then sum ~ `5*N_rollers`, against the ~`3*N_rollers + N_rollers*package_length/2000` for `compute`, naively the usage of AVX set of instructions for 32 byte SIMD (increasing the compute bound 4 times) should have resulted in faster execution. In fact, about twice as slow execution was observed. One possible hypotheses is that the assumption that the memory bound will not be relevant since all the data (`4*64*10 000=2 560 000` fits into the L3 cache (3MiB) is not in fact correct.) Potentially, due to additional copies of the data needed for "versioned vectorization", the data does not fit in the cache.

This optimization is also not taken onboard,

## Optimized
As final conclusion, the optimizations taken into account all mostly impact the Explict Euler method. The profiling of the Implicit Euler suggests that the bottlenecks in the other methods are mostly due to the connections to the external libraries Adept and Eigen. Future optimizations of the Implicit Euler method would be thus most likely based on numrical algorithm choice (e.g. using itterative solutions rather than LU-decomposition of the  Jacobian). The final performance is shown below:

| Compiler Optimization | debug | not_optimized | optimized | fully_optimized |
|-----------------------|------:|--------------:|----------:|----------------:|
| Scenario 1            | 0.11 s|    0.6 s      |   0.09 s  |      0.09 s     |
| Scenario 2            | 1.2 s |   56.0 s      |   0.78 s  |      0.76 s     |
| Scenario 3            | 1.0 s |   28.8 s      |   0.65 s  |      0.68 s     |