#include <vector>
#include <chrono>
#include <memory>

#include "Solver.h"

// Create lightweight function to solve
// y'' - y' + 3y = t -> y'' = t + y' - 3y
double f (double t , const OrangeDrumExplorer::vec& y){
    return t + y[1] - 3*y[0];
}

int main(int, char**) {
    //Setup a generic solver
    std::unique_ptr<OrangeDrumExplorer::EulerExplicit> solver;
    
    //Initialize an Explicit Euler solver with domain between 0 and 4, default time step
    solver = std::make_unique<OrangeDrumExplorer::EulerExplicit>(0., 10.);
    // Explicitly set time step
    solver->set_time_step(10./(1024*1024*2));
    // Create initial conditions
    OrangeDrumExplorer::vec y0 = {1., -2.};

    auto t0 = std::chrono::steady_clock::now();
    // Solve the equation for the given initial conditions
    OrangeDrumExplorer::vec y1 = solver->solve(f, y0);
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count()/1000.;
    std::cout << "Hello, Solution! in " << time << " seconds." << std::endl;
}