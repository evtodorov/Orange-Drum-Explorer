#include <vector>
#include <chrono>
#include <memory>
#include <random>
#include <algorithm>
#include <numeric>

#include "Solver.h"

// some parameters for the computationally intesive function
const double package_length = 3.0;
const size_t N_rollers = 10000;
std::vector<double> roller_locations;
std::vector<double> roller_forces;
std::vector<bool> mask(N_rollers, false);
std::vector<double> accumulator(N_rollers, 0.);
// computationally intensive function
// roughly models movement of a package on powered rollers with random speeds
// computation comes from detection of rollers to be taken into account
// sorting the locations is purposefully ignored as an option to optimize.
double compute(double t, OrangeDrumExplorer::vec y){
    //start with trivial implementation to have something to optimize
    double acceleration = 0;

    for (auto i=0; i<N_rollers; i++){
        mask[i] =  abs(roller_locations[i] - y[0] ) < package_length;
    }
    for (auto i=0; i<N_rollers; i++){
        accumulator[i] = roller_forces[i]*mask[i];
    }
    acceleration = std::accumulate(accumulator.begin(), accumulator.end(), 0.);
    return acceleration;
}

int main(int, char**) {
    //Setup a generic solver
    std::unique_ptr<OrangeDrumExplorer::EulerExplicit> solver;
    
    //Initialize an Explicit Euler solver with domain between 0 and 4, default time step
    solver = std::make_unique<OrangeDrumExplorer::EulerExplicit>(0., 10.);
    // Explicitly set time step
    solver->set_time_step(10./(1024*64));
    // Create initial conditions
    OrangeDrumExplorer::vec y0 = {0., 10., -1.};
    // Setup for the computation
    std::mt19937 eng(42);

    std::uniform_real_distribution<double> dist_force(-5, 5);
    auto gen_force = std::bind(dist_force, eng);
    roller_forces.reserve(N_rollers);
    std::generate_n(roller_forces.begin(), N_rollers, gen_force);

    std::uniform_real_distribution<double> dist_loc(-1000.,1000.);
    auto gen_loc = std::bind(dist_loc, eng);
    roller_locations.reserve(N_rollers);
    std::generate_n(roller_locations.begin(), N_rollers, gen_loc);

  
    auto t0 = std::chrono::steady_clock::now();
    // Solve the equation for the given initial conditions
    OrangeDrumExplorer::vec y1 = solver->solve(compute, y0);
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count()/1000.;
    std::cout << "Hello, Solution! in " << time << " seconds." << std::endl;
}