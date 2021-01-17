#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "Solver.h"

#ifndef ODEINCL_ADEPT_SORUCE_H
#include <adept_source.h>
#define ODEINCL_ADEPT_SORUCE_H
#endif /*ODEINCL_ADEPT_SORUCE_H*/

namespace OrangeDrumExplorer{
    class bad_function_call : public std::bad_function_call
    {
        private:
            std::string msg;
        public:
            bad_function_call(const std::string& message):
            msg(message)
            {}
            const char* what() const noexcept {
                return msg.c_str();
            }
    };


    Solver::Solver()
        : Solver(0., 1.) //default limits and step
    {}

    Solver::Solver(double low, double high){
        //default step
        set_limits(low, high);
        set_time_step( (high - low)/100);
    }

    void Solver::set_limits(double low, double high){
        if (high <= low){
            throw std::invalid_argument("limit_high must be larger than limit_low");
        }
        else{
            limit_low = low;
            limit_high = high;
            init_result();
        }
    }

    void Solver::set_time_step(double dt){
        if (dt < 0.){
            throw std::invalid_argument("time_step must be larger than 0");
        }
        else if (dt> (limit_high - limit_low)/2.){
            throw std::invalid_argument("time_step must be smaller than half the domain");
        }
        else {
            time_step = dt;
            init_result();
        }
    }

    void Solver::init_result(){
        const size_t N = (limit_high-limit_low)/time_step;
        try{
            result.reserve(N+1);
        }
        catch (std::bad_alloc& e){
            std::cerr << "The solution doesn't fit into memory" << std::endl << 
                        e.what() << std::endl;
        }
        catch (std::length_error& e){
            std::cerr << "The solution is too longfor std::vector" << std::endl <<
                        e.what() << std::endl;
        }

       
    }
    bool Solver::check_solution_cache(){
        return has_been_solved;
    }
    void Solver::save_solution(std::ofstream& outfile){
        if (!has_been_solved){
            throw bad_function_call("No cached solution to save");
        }
        bool doitmyself = !outfile.is_open();
        if (doitmyself){
            outfile.open("OrangeDrumExplorer_solution.txt");
        }
        for(auto el : result){
            outfile << el << std::endl;
        }
        if (doitmyself){
            outfile.close();
        }
    }

    vec& EulerExplicit::solve(func dnf_dtn, const vec& y0){
        adept::Stack ADstack; //segfault if not initialized
        ADstack.pause_recording();

        const double a = limit_low;
        const double b = limit_high;
        const double dt = time_step;
        const size_t N = (b-a)/dt;
        const size_t n = y0.size();
        
        advec yt;
        for (auto d : y0){
            adouble ad = d;
            yt.push_back(ad);
        }
        advec ynext(n);
        result.push_back(y0[0]);
        result.resize(N+1);
        double t = a;
        adouble at;
        //step through the domain
        for (auto i = 0; i < N; ++i){

            //update lower derivatives based on previous loop
            for (auto j=0; j < n - 1; ++j){
                // y(t+dt) = y(t) + y'(t)*dt
                ynext[j] = yt[j] + yt[j+1]*dt;
            }
            // compute highest derivative for this loop
            // update second highest derivative based on highest
            at.set_value(t); 
            ynext[n-1] = yt[n-1]+ dnf_dtn(at, yt)*dt;
            yt = ynext; //copy - more efficient way?
            result[i+1] = adept::value(ynext[0]);
            t = a+i*dt;
        }
        has_been_solved = true;
        return result;
    }

    double EulerImplicit::get_threshold(){
        return threshold;
    }

    void EulerImplicit::set_threshold(double new_threshold){
        threshold = new_threshold;
    }


    advec EulerImplicit::NewtonSolve(func dnf_dtn, const double t, const advec& x0){
        adept::Stack ADstack; 
        ADstack.new_recording();
        const size_t n = x0.size();
        const double dt = time_step;
        advec F = x0;
        advec x = x0;
        int iter=0;
        while (iter<max_iterations){
            for (size_t j=0; j<n-1;j++){
                F[j] = x0[j] + dt*x[j+1]-x[j];
            }
            F[n-1] = x0[n-1] + dt*dnf_dtn(t, x) - x[n-1];

            //TODO: define Jacobian Jf
            //   -1   1     0    0
            //   0    -1    1    0
            //   0    0     -1   1
            // df/dy df/dy df/dy df/y-1
            //TODO: use Eigen to solve x = solve(Jf, F) - x
            //    Eigen::FullPivLU<Matrix<double, 3, 3>> lu(m2);
            //    invertible = lu.isInvertible();
            //    Eigen::Vector3d sol2 = lu.solve(v);
            ++iter;
        }

        return x;
    }

    vec& EulerImplicit::solve(func dnf_dtn, const vec& y0){


        const double a = limit_low;
        const double b = limit_high;
        const double dt = time_step;
        const size_t N = (b-a)/dt;
        const size_t n = y0.size();
        
        advec yt;
        for (auto d : y0){
            adouble ad = d;
            yt.push_back(ad);
        }
        advec ynext(n);
        result.push_back(y0[0]);
        result.resize(N+1);
        double t = a;
        adouble at;
        //step through the domain
        for (auto i = 0; i < N; ++i){
            if (isnan(adept::value(ynext[0]))){ 
                ynext = NewtonSolve(dnf_dtn, t, yt);
            }
            result[i+1] = adept::value(ynext[0]);
            t = a+i*dt;
        }
        has_been_solved = true;
        return result;
    }

}