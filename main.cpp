#include <iostream>
#include "simulate.hpp"

int main() {
    std::cout << "Starting simulation...\n";

    int d = 11; // code distance
    double p = 0.05; // error probability
    SimConfig config{d, p};
    run_monte_carlo(config, 1000);

    std::cout << "Done.\n";
    return 0;
}
