#include <iostream>
#include "simulate.hpp"

int main() {
    std::cout << "Starting surface code simulator...\n";

    std::vector<int> distances = {
        3, 5, 7, 9, 13, 17, 21, 31, 41, 51
    };
    
    std::vector<double> probabilities = {
        0.0001, 0.0002, 0.0005, 
        0.001, 0.002, 0.005, 
        0.01, 0.02, 0.05, 
        0.1, 0.2, 0.5
    };
    
    int shots = 1000;
    std::string output_file = "threshold_data.csv";

    run_threshold_sweep(distances, probabilities, shots, output_file);

    return 0;
}
