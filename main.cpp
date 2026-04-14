#include <iostream>
#include "simulate.hpp"

int main() {
    std::cout << "Starting simulation...\n";

    int d = 5; // code distance
    double p = 0.2; // error probability
    run_simulation(d, p);

    std::cout << "Done.\n";
    return 0;
}
