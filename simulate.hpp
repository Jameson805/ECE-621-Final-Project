#ifndef SIMULATE_HPP
#define SIMULATE_HPP

#include "logical.hpp"

struct SimConfig {
    int d;
    double p; 
    bool use_measurement_errors;
    bool verbose = false;
};

void run_verbose_simulation(const SimConfig& config);
void run_monte_carlo(const SimConfig& config, int num_shots);
void run_threshold_sweep(const std::vector<int>& distances, const std::vector<double>& probabilities, int num_shots, const std::string& filename);

#endif
