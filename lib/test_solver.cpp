#include <iostream>
#include <vector>
#include "solver.h"
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

OrangeDrumExplorer::EulerExplicit test_solution(){
    OrangeDrumExplorer::EulerExplicit solver(0., 4.);
    solver.set_time_step(4./128);
    OrangeDrumExplorer::vec y0 = {1., -2.};
    OrangeDrumExplorer::func f = [](double t, OrangeDrumExplorer::vec y){return t + y[1] - 3*y[0];};
    OrangeDrumExplorer::vec y1 = solver.solve(f, y0);
    assert(((5.40506 < y1.back() && y1.back() < 5.40508) && "Solution accuracy"));
    return solver;
}

void _refresh_file(const std::string& fname){
    std::ofstream test_out(fname, std::ios::trunc);
    if (test_out.is_open()){
        test_out << "";
        test_out.close();
    }
    else{
        assert((1==0 && "Couldn't test file storage"));
    }
}

void _check_file(const std::string& fname, int steps, double last_min, double last_max){
    std::ifstream test_in(fname);
    if (test_in.is_open()){
        double val;
        int rows = 0;
        while (test_in >> val){
            ++rows;
        }
        assert((rows==steps && "Number of rows in file"));
        assert(((last_min < val && val < last_max) && "Solution accuracy from file"));
    }
    else{
        assert((1==0 && "Couldn't read stored file"));
    }
}

void test_save_to_file(OrangeDrumExplorer::EulerExplicit& solver){
    const std::string fname = "test_EE_solution.txt";
    OrangeDrumExplorer::EulerExplicit solver2(0., 4.);
    solver2.set_time_step(4./128);
    try{
        solver2.save_solution(std::ofstream());
    }
    catch (std::bad_function_call){}
    _refresh_file(fname);
    std::ofstream test_out(fname, std::ios::trunc);
    if (test_out.is_open()){
        solver.save_solution(test_out);
        test_out.close();
        _check_file(fname, 129, 5.40506, 5.40508);
    }
    else{
        assert((1==0 && "Couldn't test file storage"));
    }
}

int main(int, char**) {
    test_default();
    test_custom();
    test_limits();
    test_dt();
    test_reversed();
    test_large_dt();
    auto solver = test_solution();
    test_save_to_file(solver);
}
