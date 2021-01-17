#include <iostream>
#include <vector>
#include "Solver.h"

// Helper function to see the solution
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v){
    for (auto i : v){
        os << i << " ";
    }
    os << std::endl;
    return os;
}

int main(int, char**) {
    std::cout << "Hello, world!\n";
    try{
        //Setup a generic solver
        std::unique_ptr<OrangeDrumExplorer::Solver> solver;
        //Initialize an Explicit Euler solver with domain between 0 and 4, default time step
        solver = std::make_unique<OrangeDrumExplorer::EulerExplicit>(0., 4.);
        // Explicitly set time step
        solver->set_time_step(4./128);
        // Create initial conditions
        OrangeDrumExplorer::vec y0 = {1., -2.};
        // Create equation to solve
        // y'' - y' + 3y = t -> y'' = t + y' - 3y
        OrangeDrumExplorer::func f = [](double t, OrangeDrumExplorer::vec y){return t + y[1] - 3*y[0];};
        std::cout << "Hello, Solver!\n";
        // Solve the equation for the given initial conditions
        OrangeDrumExplorer::vec y1 = solver->solve(f, y0);
        std::cout << "Hello, Solution!\n";
        // Store the output
        std::ofstream outfile("Example_solution.txt");
        solver->save_solution(outfile);
        outfile.close();
        // Print the last value to stdout
        std::cout << y1[y1.size()-1] << std::endl;
    }
    catch (std::invalid_argument& e){
        std::cerr << "One of the functions received an invalid argument!" << std::endl;
        return -1;
    }
    catch ( const std::exception& e ){
        std::cerr << "An unknown exception occured: " << e.what() << std::endl;
        return -2;
    }
    catch (...){
        std::cerr << "An unknown error. Aborting." << std::endl;
        return -3;
    }
    return 0;
}