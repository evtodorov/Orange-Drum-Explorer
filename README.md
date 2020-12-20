# Orange-Drum-Explorer

Ordinary Differential Equiation Solver in the context of Advanced Programming IN1503 WS20/21

## How to use

The solver is packaged in a static library, defining class `Solver` and subclassed to implement different solvers. Currently, only an Explicit Euler method has been defined.

An example of how to use the library is provided in [main.cpp]() and is explained below:

1. The user creates a Solver object, assigns a solver type and (optionally) configures the domain and time step to use
    ```
    //Setup a generic solver
    std::unique_ptr<OrangeDrumExplorer::Solver> solver;
    //Initialize an Explicit Euler solver with domain between 0 and 4, default time step
    solver = std::make_unique<OrangeDrumExplorer::EulerExplicit>(0., 4.);
    // Explicitly set time step
    solver->set_time_step(4./128);
    ```

1. The user configures the initial conditions to use. The initial condition vector should contain initial value of the function and n-1 lowest derivatives at the lower limit, with `y0[0] = f(t0), y0[1] = f'(t0), ..., y0[n-1]`.
    ```
    // Create initial conditions
    OrangeDrumExplorer::vec y0 = {1., -2.};
    ```

1. The user creates a function `f(t,y)` for the ODE in terms of the highest order derivative, assuming y is a vector of the function and n-1 lower derivatives such that  `y[0] = f(t), y[1] = f'(t), ..., y[n-1]` and t is the independent variable. 
    ```
    // Create equation to solve
    // y'' - y' + 3y = t -> y'' = t + y' - 3y
    OrangeDrumExplorer::func f = [](double t, OrangeDrumExplorer::vec y){return t + y[1] - 3*y[0];};
    ```
1.  The user calls the `solve` function of the Solver with the equation function and initial conditions vector. The output is the numerical solutions of the ODE at each time step in the domain.
    ```
    // Solve the equation for the given initial conditions
    OrangeDrumExplorer::vec y1 = solver->solve(f, y0);
    ```