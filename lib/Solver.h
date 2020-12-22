#ifndef ORANGE_DRUM_EXPLORER_SOLVER_H
#define ORANGE_DRUM_EXPLORER_SOLVER_H

#include <functional>
#include <vector>

namespace OrangeDrumExplorer
{
    typedef std::vector<double> vec;
    typedef std::function<double(double, const vec&)> func ;

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
        public:
            // Use default domain and time step as per implementation
            Solver();
            // Use user-defined domain and default time step
            Solver(double limit_low, double limit_high);
            // Set domain limits for the solution
            void set_limits(double, double);
            // Set a custom time step
            void set_time_step(double);
            /**
             * Solve the function over the domain, given the initial value
             * 
             * @param dnf_dtn(t,y) - explicit definition of the ODE in terms of the highest order (n) derivative
             *      @param t - independet variable
             *      @param y - vector of lower derivatives y[0] = f; y[1] =f'; y[2] = f'' etc. up-to n-1
             * @param y0 - initial value of the function and n-1 lowest derivatives at the lower limit
             */
            virtual vec solve(func dnf_dtn, const vec& y0) = 0;
    };

    class EulerExplicit : public Solver {
        public:
            using Solver::Solver;
            vec solve(func dnf_dtn, const vec& y0);
    };
}

#endif /*ORANGE_DRUM_EXPLORER_SOLVER_H*/