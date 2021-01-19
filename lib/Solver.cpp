#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cmath>

#include "Solver.h"

#ifndef ODEINCL_ADEPT_SORUCE_H
#include <adept_source.h>
#define ODEINCL_ADEPT_SORUCE_H
#endif /*ODEINCL_ADEPT_SORUCE_H*/

#include <Eigen/Core>
#include <Eigen/LU>

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
            yt.push_back(d);
        }
        advec ynext(n);
        result.push_back(y0[0]);
        result.resize(N+1);
        double t = a;
        //step through the domain
        for (auto i = 0; i < N; ++i){
            t = a+(i+1)*dt;

            //update lower derivatives based on previous loop
            for (auto j=0; j < n - 1; ++j){
                // y(t+dt) = y(t) + y'(t)*dt
                ynext[j] = yt[j] + yt[j+1]*dt;
            }
            // compute highest derivative for this loop
            // update second highest derivative based on highest
            ynext[n-1] = yt[n-1]+ dnf_dtn(t, yt)*dt;
            yt = ynext; //copy - more efficient way?
            result[i+1] = adept::value(ynext[0]);
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


    struct EulerImplicit::DivergentException : public std::exception{
        const char * what () const throw (){
    	    return "The solution doesn't converge.";
        }
    };

    vec EulerImplicit::NewtonSolve(func dnf_dtn, const double t, const vec& x0){
        adept::Stack ADstack;
        const size_t n = x0.size();
        const double dt = time_step;
        advec x;
        for (auto el : x0){
            x.push_back(el);
        }
        ADstack.new_recording();

        int iter=0;

        //define Jacobian Jf for ND Newton Method
        //   -1   1     0    0
        //   0    -1    1    0
        //   0    0     -1   1
        // df/dy df/dy df/dy df/y-1 (update every loop)
        Eigen::MatrixXd JF = Eigen::MatrixXd::Constant(n,n,0.);
        for(size_t i=0; i<n-1; i++){
            JF(i,i)=-1;
            JF(i,i+1)=dt;
        }

        Eigen::VectorXd F = Eigen::VectorXd::Constant(n, 0);
        Eigen::VectorXd delta = Eigen::VectorXd::Constant(n, 0);
        adouble eval_dnf_dtn;
        while (iter<max_iterations){

            eval_dnf_dtn = dnf_dtn(t, x);
            eval_dnf_dtn.set_gradient(1.0);
            ADstack.compute_adjoint();

            //Define F (RHS)
            // F = y(previous t) + dt*y'(this t, previous iter) + y(this t, previous iter)
            for (size_t j=0; j<n-1;j++){
                F(j) = adept::value(x0[j] + dt*x[j+1]-x[j]);
                JF(n-1,j) = dt*x[j].get_gradient(); //last row of JF thru automatic derivatives
            }
            // y'[n] = dnf_dtn(this t, previous iter)
            F(n-1) = adept::value(x0[n-1] + dt*eval_dnf_dtn - x[n-1]);
            
            JF(n-1, n-1) = dt*x[n-1].get_gradient() - 1.; //last element
    
            // use Eigen to solve x = solve(Jf, F) - x
            delta = JF.fullPivLu().solve(F);

            // Check if real solution
            if((JF*delta).isApprox(F)!=true){
                throw DivergentException();
            }
            // Check if valid solution
            if (delta.array().isNaN().any()){
                throw DivergentException();
            }
            // Check if converged
            if ((delta.array().abs() < threshold).all()){
                //last loop
                iter = max_iterations; 
            }
            // Update x
            for (size_t j=0; j<n; ++j){
                x[j] -= delta(j);
            }
            ++iter;
        }
        vec out;
        for (auto el: x){
            out.push_back(adept::value(el));
        }
        return out;
    }

    vec& EulerImplicit::solve(func dnf_dtn, const vec& y0){
        const double a = limit_low;
        const double b = limit_high;
        const double dt = time_step;
        const size_t N = (b-a)/dt;
        const size_t n = y0.size();
        
        vec yt = y0;

        vec ynext(n);
        result.push_back(y0[0]);
        result.resize(N+1);
        double t = a;
        //step through the domain
        for (auto i = 0; i < N; ++i){
            t = a+(i+1)*dt;
            try{
                 ynext = NewtonSolve(dnf_dtn, t, yt);
            }
            catch (DivergentException){
                for (auto j=i; j<N; ++j){
                    result[j+1] = std::nan("");
                }
                break;
            }
            result[i+1] = ynext[0];
            yt = ynext;
        }
        has_been_solved = true;
        return result;
    }

}