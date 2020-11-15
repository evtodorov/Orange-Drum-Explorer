#include <stdexcept>
#include <iostream>

#include "solver.h"

namespace OrangeDrumExplorer{
    Solver::Solver(){
        set_limits(0., 1.);
        set_time_step(0.01);
    }

    Solver::Solver(double limit_low, double limit_high){
        if (limit_high <= limit_low){
            throw std::invalid_argument("limit_high must be larger than limit_low");
        }
        else{
            set_limits(limit_low, limit_high);
            set_time_step( (limit_high - limit_low)/100);
        }
    }

    void Solver::set_limits(double limit_low, double limit_high){
        if (limit_high <= limit_low){
            throw std::invalid_argument("limit_high must be larger than limit_low");
        }
        else{
            this->limit_low = limit_low;
            this->limit_high = limit_high;
        }
    }

    void Solver::set_time_step(double time_step){
        if (time_step < 0.){
            throw std::invalid_argument("time_step must be larger than 0");
        }
        else{
            this->time_step = time_step;
        }
    }

    vec EulerExplicit::solve(func dnf_dtn, const vec& y0){
        double a = this->limit_low;
        double b = this->limit_high;
        double dt = this->time_step;
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
            std::cout << "WARNING: the timestep is too small for the boundaries!" << std::endl;
        }

        return result;
    }

}