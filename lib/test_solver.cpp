#include <iostream>
#include <vector>
#include "Solver.h"
#include <cassert>

const OrangeDrumExplorer::vec y0_const = {1.};
const OrangeDrumExplorer::func f_const = [](OrangeDrumExplorer::adouble t, OrangeDrumExplorer::advec y){return 1.;};

template<typename S>
void test_default(){
    S solver;
    OrangeDrumExplorer::vec y1 = solver.solve(f_const, y0_const);
    assert((y1.size()==101 && "Default constructor limits and time step.")) ;
}

template<typename S>
void test_custom(){
    S solver(0, 4.);
    OrangeDrumExplorer::vec y1 = solver.solve(f_const, y0_const);
    assert((y1.size()==101 && "Custom constructor limits and default time step.")) ;
}

template<typename S>
void test_limits(){
    S solver;
    solver.set_limits(-2.,0);
    OrangeDrumExplorer::vec y1 = solver.solve(f_const, y0_const);
    assert((y1.size()==201 && "Custom limits and default time step.")) ;
}

template<typename S>
void test_dt(){
    S solver;
    solver.set_time_step(0.05);
    OrangeDrumExplorer::vec y1 = solver.solve(f_const, y0_const);
    assert((y1.size()==21 && "Default constructor limits and custom time step.")) ;
}

template<typename S>
void test_reversed(){
    try{
        S solver(4, 0);
    }
    catch (std::invalid_argument& e){
        return;
    }
    assert((false && "Reversed limits"));
}

template<typename S>
void test_large_dt(){
    S solver(0,1.);
    try{
        solver.set_time_step(0.6);
    }
        catch (std::invalid_argument& e){
        return;
    }
    assert((false && "Too large time step"));
}


OrangeDrumExplorer::EulerExplicit test_solution(){
    OrangeDrumExplorer::EulerExplicit solver(0., 4.);
    solver.set_time_step(4./128);
    OrangeDrumExplorer::vec y0 = {1., -2.};
    OrangeDrumExplorer::func f = [](OrangeDrumExplorer::adouble t, OrangeDrumExplorer::advec y)
                                 {return OrangeDrumExplorer::adouble(t + y[1] - 3*y[0]);};
    OrangeDrumExplorer::vec y1 = solver.solve(f, y0);
    assert(((5.47601 < y1.back() && y1.back() < 5.47603) && "Solution accuracy"));
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
        _check_file(fname, 129, 5.47601, 5.47603);
    }
    else{
        assert((1==0 && "Couldn't test file storage"));
    }
}

int main(int, char**) {
    typedef OrangeDrumExplorer::EulerExplicit EE;
    test_default<EE>();
    test_custom<EE>();
    test_limits<EE>();
    test_dt<EE>();
    test_reversed<EE>();
    test_large_dt<EE>();
    EE solver = test_solution();
    test_save_to_file(solver);
}
