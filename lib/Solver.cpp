#include <stdexcept>
#include <iostream>

#include "Solver.h"

namespace OrangeDrumExplorer{
    Solver::Solver(){
        //default limits and step
        set_limits(0., 1.);
        set_time_step(0.01);
    }

    Solver::Solver(double low, double high){
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
        }
    }

    vec EulerExplicit::solve(func dnf_dtn, const vec& y0){
        double a = limit_low;
        double b = limit_high;
        double dt = time_step;
        const size_t N = (b-a)/dt;
        const size_t n = y0.size();
        
        vec yt = y0; //copy
        vec ynext(n);
        vec result(N+1, 0.);
        result[0] = y0[0];

        double t = a;
        //step through the domain
        for (auto i = 0; i < N; ++i){

            //update lower derivatives based on previous loop
            for (auto j=0; j < n - 1; ++j){
                // y(t+dt) = y(t) + y'(t)*dt
                ynext[j] = yt[j] + yt[j+1]*dt;
            }
            // compute highest derivative for this loop
            // update second highest derivative based on highest
            ynext[n-1] = yt[n-1] + dnf_dtn(t, yt)*dt;
            yt = ynext; //copy - more efficient way?
            result[i+1] = ynext[0];
            t += dt;
        }
        if (t < b-dt){
            std::cout << "WARNING: the timestep is too small for the boundaries! Overflow might affect the solution" << std::endl;
        }

        return result;
    }

}