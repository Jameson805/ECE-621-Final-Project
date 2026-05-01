#include <iostream>
#include <random>
#include <thread>
#include "simulate.hpp"
#include "noise.hpp"

thread_local std::mt19937 rng(std::random_device{}() + std::hash<std::thread::id>{}(std::this_thread::get_id()));

void sample_physical_noise(Lattice &lat, const SimConfig &config) {
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    if (config.noise_model == NoiseModel::INDEPENDENT) {
        // Independent X and Z trials
        for (int q = 0; q < lat.num_qubits; q++) {
            bool err_x = (prob(rng) < config.p_ind);
            bool err_z = (prob(rng) < config.p_ind);

            if (err_x && err_z) {
                lat.errors[q] = static_cast<Pauli>(lat.errors[q] ^ Pauli::Y);
            } else if (err_x) {
                lat.errors[q] = static_cast<Pauli>(lat.errors[q] ^ Pauli::X);
            } else if (err_z) {
                lat.errors[q] = static_cast<Pauli>(lat.errors[q] ^ Pauli::Z);
            }
        }
    } else {
        // Mutually Exclusive Models: Depolarizing and Biased
        for (int q = 0; q < lat.num_qubits; q++) {
            double r = prob(rng);
            Pauli error = Pauli::I;

            if (r < config.p_x) error = Pauli::X;
            else if (r < config.p_x + config.p_y) error = Pauli::Y;
            else if (r < config.p_x + config.p_y + config.p_z) error = Pauli::Z;

            if (error != Pauli::I) {
                lat.errors[q] = static_cast<Pauli>(lat.errors[q] ^ error); 
            }
        }
    }
}

void sample_measurement_noise(std::vector<int>& syndrome, const SimConfig &config) {
    if (config.p_meas <= 0.0) return;

    std::uniform_real_distribution<double> prob(0.0, 1.0);
    for (size_t i = 0; i < syndrome.size(); i++) {
        if (prob(rng) < config.p_meas) {
            syndrome[i] ^= 1; 
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
            char type = (lat.errors[q] == Pauli::X ? 'X' :
                         lat.errors[q] == Pauli::Z ? 'Z' : 'Y');
            std::cout << "q(" << x << "," << y << ")=" << type << " ";
        }
    }
    if (!any) std::cout << "none";
    std::cout << "\n";
}
