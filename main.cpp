#include <iostream>
#include <vector>
#include "solver.h"

template <typename T>
void printVec(const std::vector<T>& v){
    for (auto i : v){
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

int main(int, char**) {
    std::cout << "Hello, world!\n";
    OrangeDrumExplorer::EulerExplicit solver(0., 4.);
    solver.set_time_step(4./128);
    OrangeDrumExplorer::vec y0 = {1., -2.};
    // y'' - y' + 3y = t -> y'' = t + y' - 3y
    OrangeDrumExplorer::func f = [](double t, OrangeDrumExplorer::vec y){return t + y[1] - 3*y[0];};
    std::cout << "Hello, Solver!\n";
    OrangeDrumExplorer::vec y1 = solver.solve(f, y0);
    std::cout << "Hello, Solution!\n";
    printVec(y1);
}
