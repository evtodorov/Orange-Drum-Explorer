#pragma once

#include <functional>
#include <vector>
#include <iostream>
#include <fstream>

namespace OrangeDrumExplorer
{
    typedef std::vector<double> vec;
    typedef std::function<double(double, const vec&)> func ;

    /**
     * Base class for implementing solvers.
     * 
     * Solve an ordinary differential equation within the limits of a Domain,
     * given an initial value at the lower limit
     *
     * @param limit_low - lower limit of the Domain
     * @param limit_high - upper limit of the Domain
     * @param time_step - time step to use to solve the equation
     */
    class Solver
    {   
        protected:
            double limit_low;
            double limit_high;
            double time_step;
            bool has_been_solved = false;
            vec result;
            void init_result();
        public:
            // Use default domain and time step as per implementation
            Solver();
            // Use user-defined domain and default time step
            Solver(double limit_low, double limit_high);
            // Set domain limits for the solution
            void set_limits(double, double);
            // Set a custom time step
            void set_time_step(double);
            // Check if a solution has been cached
            bool check_solution_cache();
            // Store the last solution to filestream. The user needs to handle open/close.
            void save_solution(std::ofstream&);
            /**
             * Solve the function over the domain, given the initial value
             * 
             * @param dnf_dtn(t,y) - explicit definition of the ODE in terms of the highest order (n) derivative
             *      @param t - independet variable
             *      @param y - vector of lower derivatives y[0] = f; y[1] =f'; y[2] = f'' etc. up-to n-1
             * @param y0 - initial value of the function and n-1 lowest derivatives at the lower limit
             */
            virtual vec& solve(func dnf_dtn, const vec& y0) = 0;
    };

    class EulerExplicit : public Solver {
        public:
            using Solver::Solver;
            vec& solve(func dnf_dtn, const vec& y0) override;
    };
}

