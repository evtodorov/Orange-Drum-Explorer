#include <iostream>
#include <vector>
#include "Solver.h"
#include <cassert>

const OrangeDrumExplorer::vec y0_const = {1.};
const OrangeDrumExplorer::func f_const = [](double t, OrangeDrumExplorer::vec y){return 1.;};

void test_default(){
    OrangeDrumExplorer::EulerExplicit solver;
    OrangeDrumExplorer::vec y1 = solver.solve(f_const, y0_const);
    assert((y1.size()==101 && "Default constructor limits and time step.")) ;
}

void test_custom(){
    OrangeDrumExplorer::EulerExplicit solver(0, 4.);
    OrangeDrumExplorer::vec y1 = solver.solve(f_const, y0_const);
    assert((y1.size()==101 && "Custom constructor limits and default time step.")) ;
}

void test_limits(){
    OrangeDrumExplorer::EulerExplicit solver;
    solver.set_limits(-2.,0);
    OrangeDrumExplorer::vec y1 = solver.solve(f_const, y0_const);
    assert((y1.size()==201 && "Custom limits and default time step.")) ;
}

void test_dt(){
    OrangeDrumExplorer::EulerExplicit solver;
    solver.set_time_step(0.05);
    OrangeDrumExplorer::vec y1 = solver.solve(f_const, y0_const);
    assert((y1.size()==21 && "Default constructor limits and custom time step.")) ;
}

void test_reversed(){
    try{
        OrangeDrumExplorer::EulerExplicit solver(4, 0);
    }
    catch (std::invalid_argument& e){
        return;
    }
    assert((1==0 && "Reversed limits"));
}

void test_large_dt(){
    OrangeDrumExplorer::EulerExplicit solver(0,1.);
    try{
        solver.set_time_step(0.6);
    }
        catch (std::invalid_argument& e){
        return;
    }
    assert((1==0 && "Too large time step"));
}

void test_solution(){
    OrangeDrumExplorer::EulerExplicit solver(0., 4.);
    solver.set_time_step(4./128);
    OrangeDrumExplorer::vec y0 = {1., -2.};
    OrangeDrumExplorer::func f = [](double t, OrangeDrumExplorer::vec y){return t + y[1] - 3*y[0];};
    OrangeDrumExplorer::vec y1 = solver.solve(f, y0);
    assert(((5.40506 < y1.back() && y1.back() < 5.40508) && "Solution accuracy"));
}

int main(int, char**) {
    test_default();
    test_custom();
    test_limits();
    test_dt();
    test_reversed();
    test_large_dt();
    test_solution();
}
