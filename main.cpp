#include <iostream>
#include <vector>
#include <chrono>

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
    std::cout << "Demonstrating the use of the Orange-Drum-Explorer Ordinary diiferential equation solver..." << std::endl;
    std::cout << "The following solver options are currently available:" << std::endl << std::endl;
    std::cout << "1) Explicit Euler solver" << std::endl;
    std::cout << "2) Implicit Euler solver (using Newton method with automatic derivatives)" << std::endl;
    std::cout << std::endl << 
    "The test IVP y(t)'' - y(t)' + 3y(t) = t; y(0)=1; y'(0)=-2; will be solved between t=[0,4] in 128 steps." << std::endl;
    std::cout << "y(t=4) will be displayed. The full solution will be stored in 'Example_solution.txt' in your cwd." << std::endl;
    std::cout << "Please enter the number of the solver you want to use and press Enter:." << std::endl;
    int choice = 0;
    try{
        std::cin>>choice;
        while(std::cin.fail() || !(choice == 1 || choice == 2) ){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Your choice is not valid. Please try again. "<< std::endl;
            std::cin>>choice;
        }
    }
    catch (...){
        std::cerr << "An unknown error in your input popped the drums. Aborting." << std::endl;
        return -4;
    }
    try{
        auto t0 = std::chrono::steady_clock::now();
        //Setup a generic solver
        std::unique_ptr<OrangeDrumExplorer::Solver> solver;
        
        switch (choice) {
        case 1:
            //Initialize an Explicit Euler solver with domain between 0 and 4, default time step
            solver = std::make_unique<OrangeDrumExplorer::EulerExplicit>(0., 4.);
            break;
        case 2:
            //Or can be replaced by an Implicit Euler solver using the Bridge Pattern 
            solver = std::make_unique<OrangeDrumExplorer::EulerImplicit>(0., 4.);
            break;
        default:
            throw std::invalid_argument("Should never get here.");
        }

        // Explicitly set time step
        solver->set_time_step(4./1024);
        // Create initial conditions
        OrangeDrumExplorer::vec y0 = {1., -2.};
        // Create equation to solve
        // y'' - y' + 3y = t -> y'' = t + y' - 3y
        OrangeDrumExplorer::func f = [](OrangeDrumExplorer::adouble t, OrangeDrumExplorer::advec y)
                                     {return OrangeDrumExplorer::adouble(t + y[1] - 3.0*y[0]);};
        std::cout << "Hello, Solver!\n";
        // Solve the equation for the given initial conditions
        OrangeDrumExplorer::vec y1 = solver->solve(f, y0);
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count()/1000.;
        std::cout << "Hello, Solution! in " << time << " seconds." << std::endl;
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
        std::cerr << "An unknown error popped the drums. Aborting." << std::endl;
        return -3;
    }
    return 0;
}