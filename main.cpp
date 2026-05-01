#include <iostream>
#include <vector>
#include "simulate.hpp"

void run_full_experiment(const std::vector<int>& distances, const std::vector<double>& probabilities, int shots) {
    // Define the 4 measurement error ratios:
    // 0.0 = Code Capacity (2D baseline)
    // 0.1 = Good Readout (Meas errors 10x lower than gate errors)
    // 1.0 = Isotropic (Equal gate and measurement errors)
    // 10.0 = Bad Readout (Meas errors 10x higher than gate errors)
    std::vector<double> ratios = {0.0, 0.1, 1.0, 10.0};

    std::cout << "\nCommencing 12-condition surface code simulation suite...\n";

    for (double ratio : ratios) {
        std::cout << "\n======================================================\n";
        std::cout << " RUNNING EXPERIMENT BLOCK | MEASUREMENT RATIO: " << ratio;
        std::cout << "\n======================================================\n";

        run_threshold_sweep(distances, probabilities, shots, NoiseModel::INDEPENDENT, ratio, 10.0);
        run_threshold_sweep(distances, probabilities, shots, NoiseModel::DEPOLARIZING, ratio, 10.0);
        run_threshold_sweep(distances, probabilities, shots, NoiseModel::BIASED, ratio, 10.0);
    }
        std::cout << "\nSimulation suite finished successfully!\n";
}

int main() {
        std::vector<int> distances = {
        3, 5, 7, 9, 11, 13, 15, 17, 19, 21
    };
    
    std::vector<double> probabilities = {
        0.005, 0.008, 0.012, 0.018, 0.028, 
        0.042, 0.065, 0.090, 0.110, 0.130, 
        0.150, 0.170, 0.190, 0.220, 0.260, 
        0.300
    };
    
    int shots = 1000;

    // Run 2D Code Capacity
    run_threshold_sweep(distances, probabilities, shots, NoiseModel::DEPOLARIZING, 0.0, 10.0);

    // Run 3D Phenomenological
    // run_threshold_sweep(distances, probabilities, shots, NoiseModel::DEPOLARIZING, 1.0, 10.0);

    return 0;

    /*
    std::vector<int> distances = {
        3, 5, 7, 9, 11, 13, 15
    };
    std::vector<double> probabilities = {
        0.005, 0.008, 0.012, 0.018, 0.028, 
        0.042, 0.065, 0.090, 0.110, 0.130, 
        0.150, 0.170, 0.190, 0.220, 0.260, 
        0.300
    };
    int shots = 1000;

    run_full_experiment(distances, probabilities, shots);

    return 0;
    */
}
