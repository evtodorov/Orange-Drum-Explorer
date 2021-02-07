#ifndef ORANGE_DRUM_EXPLORER_SOLVER_H
#define ORANGE_DRUM_EXPLORER_SOLVER_H

#include <functional>
#include <vector>
#include <iostream>
#include <fstream>

#include <adept.h>

namespace OrangeDrumExplorer
{
    typedef adept::adouble adouble;
    typedef std::vector<double> vec;
    typedef std::vector<adouble> advec;
    typedef std::function<adouble(adouble, const advec&)> func ;

    /**
     * Base class for implementing solvers.\n
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
             *      @param t - independent variable
             *      @param y - vector of lower derivatives y[0] = f; y[1] =f'; y[2] = f'' etc. up-to n-1
             * @param y0 - initial value of the function and n-1 lowest derivatives at the lower limit
             */
            virtual vec& solve(func dnf_dtn, const vec& y0) = 0;
    };

    class EulerExplicit : public Solver {
        public:
            using Solver::Solver;
            vec& solve(std::function<double(double, const vec&)> dnf_dtn, const vec& y0);
            vec& solve(func dnf_dtn, const vec& y0) override;
    };

    class EulerImplicit : public Solver {
        protected:
            double threshold = 1e-4;
            const size_t max_iterations = 50;
            struct DivergentException;
            // Solve a non-linear equation using the Newton Method
            vec NewtonSolve(func dnf_dtn, const double t, const vec& x0);
        public:
            using Solver::Solver;
            // Check the current threshold for the Newton iterative solver
            double get_threshold();
            // Check the current threshold for the Newton iterative solver
            void set_threshold(double);
            vec& solve(func dnf_dtn, const vec& y0) override;
    };
}

#endif /*ORANGE_DRUM_EXPLORER_SOLVER_H*/