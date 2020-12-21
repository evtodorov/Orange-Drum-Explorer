#include <stdexcept>
#include <iostream>

#include "solver.h"

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

    Solver::Solver(){
        //default limits and step
        set_limits(0., 1.);
        set_time_step(0.01);
    }

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
        double a = limit_low;
        double b = limit_high;
        double dt = time_step;
        const size_t N = (b-a)/dt;
        const size_t n = y0.size();
        
        vec yt = y0; //copy
        vec ynext(n);
        result.push_back(y0[0]);

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
            result.push_back(ynext[0]);
            t += dt;
        }
        if (t < b-dt || t > b+dt){
            std::cout << "WARNING: the timestep is too small for the boundaries! Overflow might affect the solution" << std::endl;
        }
        has_been_solved = true;
        return result;
    }

}