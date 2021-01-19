# Orange-Drum-Explorer

Ordinary Differential Equiation Solver in the context of Advanced Programming IN1503 WS20/21.

![logo](media/logo_white_small.png)

## Building
### Dependencies
Orange-Drum-Explorer has the following dependencies:

* [adept-v1.1](http://www.met.reading.ac.uk/clouds/adept/adept_documentation.pdf) is used for automatic differentiation
* (a subset of) [eigen-v3.3.9](https://eigen.tuxfamily.org/) is used for linear algebra 
* (TODO:) [catch2-v2.13.4](https://github.com/catchorg/Catch2) is used for testing

Header-only versions are distributed together with the package in [](lib/ext). If you have any of them already installed, you might consider editing [](lib/CMakeLists.txt) to use the installed packages.

### Using CMake

On Windows:
```
cmake . -Bbuild
cd build
cmake --build . --target Orange-Drum-Explorer --config Release
```

On Linux (to be tested):
```
cmake .
make all
```

## How to use

The solver is packaged in a static library, defining class `Solver` and subclassed to implement different solvers. Currently, the following solvers are implemented:

1. A basic Explicit Euler method2
2. An implicit Euler method using the adept library to implement automatic differentiation.

An example of how to use the library is provided in [main.cpp](./main.cpp) and is explained below:

1. The user creates a Solver object, assigns a solver type and (optionally) configures the domain and time step to use
    ```
    //Setup a generic solver
    std::unique_ptr<OrangeDrumExplorer::Solver> solver;
    //Initialize an Explicit Euler solver with domain between 0 and 4, default time step
    solver = std::make_unique<OrangeDrumExplorer::EulerExplicit>(0., 4.);
	//Or can be replaced by an Implicit Euler solver using the Bridge Pattern 
	solver = std::make_unique<OrangeDrumExplorer::EulerImplicit>(0., 4.);

    // Explicitly set time step
    solver->set_time_step(4./128);
    ```

1. The user configures the initial conditions to use. The initial condition vector should contain initial value of the function and n-1 lowest derivatives at the lower limit, with `y0[0] = f(t0), y0[1] = f'(t0), ..., y0[n-1]`.
    ```
    // Create initial conditions
    OrangeDrumExplorer::vec y0 = {1., -2.};
    ```

1. The user creates a function `f(t,y)` for the ODE in terms of the highest order derivative, assuming y is a vector of the function and n-1 lower derivatives such that  `y[0] = f(t), y[1] = f'(t), ..., y[n-1]` and t is the independent variable. The function needs to use the adept library overloaded type `adouble` as illustrated.
    ```
    // Create equation to solve
    // y'' - y' + 3y = t -> y'' = t + y' - 3y
    OrangeDrumExplorer::func f = [](OrangeDrumExplorer::adouble t, OrangeDrumExplorer::advec y)
                                 {return OrangeDrumExplorer::adouble(t + y[1] - 3*y[0]);};
    ```
	
1.  The user calls the `solve` function of the Solver with the equation function and initial conditions vector. The output is the numerical solutions of the ODE at each time step in the domain.
    ```
    // Solve the equation for the given initial conditions
    OrangeDrumExplorer::vec y1 = solver->solve(f, y0);
    ```
	
1. The user can store the solution to a file using the `save_solution` function of the Solver. Please note, the user is responsible for handling file operations
	```
	// Store the output
    std::ofstream outfile("Example_solution.txt");
    solver->save_solution(outfile);
    outfile.close();
	```
	
### How to extend
Implementing new Solvers is very easy - simply inherrit from the `Solver` class, override the virtual `solve` function and reuse or override other functions as you see fit!
```class YourNewSolver : public Solver {
        protected:
			//your private vars
        public:
			//reuse Constructors
            using Solver::Solver;
			//override the virtual solve function
            vec& solve(func dnf_dtn, const vec& y0) override; ```