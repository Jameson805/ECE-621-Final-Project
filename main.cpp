#include <iostream>
#include "simulate.hpp"

int main() {
    std::cout << "Starting surface code simulator...\n";

    std::vector<int> distances = {
        3, 5, 7, 9, 11, 13, 15, 17
    };
    
    std::vector<double> probabilities = {
        0.005, 0.008, 0.012, 0.018, 0.028, 
        0.042, 0.065, 0.090, 0.110, 0.130, 
        0.150, 0.170, 0.190, 0.220, 0.260, 
        0.300
    };
    
    int shots = 1000;

    // Run 2D Code Capacity
    // run_threshold_sweep(distances, probabilities, shots, false);

    // Run 3D Phenomenological
    run_threshold_sweep(distances, probabilities, shots, true);

    return 0;
    
    /*
    int d = 51;
    double p = 0.0005;
    SimConfig config{d, p, false, true};
    run_verbose_simulation(config);
    return 0;
    */
}
