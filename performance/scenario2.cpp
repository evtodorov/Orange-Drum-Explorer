#include <vector>
#include <chrono>
#include <memory>

#include "Solver.h"


int main(int, char**) {
    //Setup a generic solver
    std::unique_ptr<OrangeDrumExplorer::Solver> solver;
    
    //Initialize an Implicit Euler solver with domain between 0 and 10, default time step
    solver = std::make_unique<OrangeDrumExplorer::EulerImplicit>(0., 10.);
    // Explicitly set time step
    solver->set_time_step(10./(1024*256));
    // Create initial conditions
    OrangeDrumExplorer::vec y0 = {1., -2.};
    // Create lightweight function to solve
    // y'' - y' + 3y = t -> y'' = t + y' - 3y
    OrangeDrumExplorer::func f = [](OrangeDrumExplorer::adouble t, OrangeDrumExplorer::advec y)
                                    {return OrangeDrumExplorer::adouble(t + y[1] - 3.0*y[0]);};

    auto t0 = std::chrono::steady_clock::now();
    // Solve the equation for the given initial conditions
    OrangeDrumExplorer::vec y1 = solver->solve(f, y0);
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count()/1000.;
    std::cout << "Hello, Solution! in " << time << " seconds." << std::endl;
}