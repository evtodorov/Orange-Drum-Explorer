#include <iostream>
#include <vector>
#include "Solver.h"

// Helper function to see the solution
template <typename T>
void printVec(const std::vector<T>& v){
    for (auto i : v){
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

int main(int, char**) {
    std::cout << "Hello, world!\n";
    //Initialize the solver between with domain between 0 and 4, default time step
    OrangeDrumExplorer::EulerExplicit solver(0., 4.);
    // Explicitly set time step
    solver.set_time_step(4./128);
    // Create initial conditions
    OrangeDrumExplorer::vec y0 = {1., -2.};
    // Create equation to solve
    // y'' - y' + 3y = t -> y'' = t + y' - 3y
    OrangeDrumExplorer::func f = [](double t, OrangeDrumExplorer::vec y){return t + y[1] - 3*y[0];};
    std::cout << "Hello, Solver!\n";
    // Solve the equation for the given initial conditions
    OrangeDrumExplorer::vec y1 = solver.solve(f, y0);
    std::cout << "Hello, Solution!\n";
    // Print the output
    printVec(y1);
}