#include <iostream>
#include <random>
#include "noise.hpp"

// Depolarizing noise model
void sample_noise(Lattice &lat, double p) {
    // Note: Consider moving the rng initialization outside to avoid identical seeds
    static std::mt19937 rng(12345); 
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    std::uniform_int_distribution<int> pauli(1, 3);

    for (int q = 0; q < lat.num_qubits; q++) {
        if (prob(rng) < p) {
            Pauli new_error = static_cast<Pauli>(pauli(rng));
            lat.errors[q] = static_cast<Pauli>(lat.errors[q] ^ new_error); 
        }
    }
}

void print_noise(const Lattice &lat) {
    std::cout << "\n=== Noise ===\n";

    bool any = false;

    for (int q = 0; q < lat.num_qubits; q++) {
        if (lat.errors[q] != Pauli::I) {
            any = true;

            int x, y;
            qubit_coords(lat.d, q, x, y);

            char type =
                (lat.errors[q] == Pauli::X ? 'X' :
                 lat.errors[q] == Pauli::Z ? 'Z' : 'Y');

            std::cout << "q(" << x << "," << y << ")=" << type << " ";
        }
    }

    if (!any) std::cout << "none";
    std::cout << "\n";
}
