#include <iostream>
#include <vector>
#include <experimental/filesystem>
#include "simulate.hpp"

void run_full_experiment(const std::vector<int>& distances, const std::vector<double>& probabilities, int shots) {
    /*
    std::string dir_path = "results";
    if (std::experimental::filesystem::exists(dir_path)) {
        std::cout << "Purging old results directory...\n";
        std::experimental::filesystem::remove_all(dir_path);
    }
    std::experimental::filesystem::create_directory(dir_path);
    */

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
        3, 5, 7, 9, 11, 15, 17
    };

    std::vector<double> probabilities = {
        0.0005, 0.0007, 0.0010, 0.0014, 0.0020, 
        0.0028, 0.0040, 0.0056, 0.0080, 0.0110, 
        0.0160, 0.0220, 0.0310, 0.0440, 0.0630, 
        0.0880, 0.1250, 0.1760, 0.2500, 0.3500, 
        0.5000
    };

    int shots = 5000;

    run_full_experiment(distances, probabilities, shots);

    return 0;
}

