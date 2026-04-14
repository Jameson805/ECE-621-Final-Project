#ifndef SIMULATE_HPP
#define SIMULATE_HPP

#include "logical.hpp"

struct SimConfig {
    int d;
    double p; 
};

void run_monte_carlo(const SimConfig& config, int num_shots);
void run_verbose_simulation(const SimConfig& config);

#endif
