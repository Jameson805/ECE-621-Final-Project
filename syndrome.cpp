#include <iostream>
#include <vector>
#include <random>
#include "syndrome.hpp"
#include "noise.hpp"

static void apply_measurement_noise(std::vector<int>& syndrome, double p) {
    static std::mt19937 rng(54321);
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    for (size_t i = 0; i < syndrome.size(); i++) {
        if (prob(rng) < p) {
            syndrome[i] ^= 1; 
        }
    }
}

void compute_syndrome(Lattice& lat, const SimConfig& config) {
    lat.x_defects.clear();
    lat.z_defects.clear();

    int num_rounds = config.use_measurement_errors ? lat.d : 1;

    std::vector<int> prev_x_syndrome(lat.num_x_stabilizers, 0);
    std::vector<int> prev_z_syndrome(lat.num_z_stabilizers, 0);

    for (int t = 0; t < num_rounds; t++) {
        if (config.verbose) {
            std::cout << "[noise] sampling noise for p = " << config.p << " at t = " << t << ":\n";
            sample_noise(lat, config.p);
            print_noise(lat);
        } else {
            sample_noise(lat, config.p);
        }

        // Compute ideal syndromes based on current accumulated errors
        std::vector<int> curr_x_syndrome(lat.num_x_stabilizers, 0);
        std::vector<int> curr_z_syndrome(lat.num_z_stabilizers, 0);

        for (int i = 0; i < lat.num_x_stabilizers; i++) {
            const Stabilizer& stab = lat.x_stabilizers[i];
            for (int j = 0; j < stab.degree; j++) {
                Pauli error = lat.errors[stab.neighbors[j]];
                if (error == Z || error == Y) curr_x_syndrome[i] ^= 1;
            }
        }

        for (int i = 0; i < lat.num_z_stabilizers; i++) {
            const Stabilizer& stab = lat.z_stabilizers[i];
            for (int j = 0; j < stab.degree; j++) {
                Pauli error = lat.errors[stab.neighbors[j]];
                if (error == X || error == Y) curr_z_syndrome[i] ^= 1;
            }
        }

        // Skip measurement noise on code capacity case and final round of phenomenological case
        if (config.use_measurement_errors && t < num_rounds - 1) {
            apply_measurement_noise(curr_x_syndrome, config.p);
            apply_measurement_noise(curr_z_syndrome, config.p);
        }

        // A defect is a difference between round t and round t-1
        for (int i = 0; i < lat.num_x_stabilizers; i++) {
            if (curr_x_syndrome[i] != prev_x_syndrome[i]) {
                lat.x_defects.push_back({i, t});
            }
        }
        for (int i = 0; i < lat.num_z_stabilizers; i++) {
            if (curr_z_syndrome[i] != prev_z_syndrome[i]) {
                lat.z_defects.push_back({i, t});
            }
        }

        prev_x_syndrome = curr_x_syndrome;
        prev_z_syndrome = curr_z_syndrome;
    }
}

void print_defects(const Lattice &lat) {
    std::cout << "\n=== DEFECTS ===\n";

    std::cout << "X defects:\n";
    for (const auto& defect : lat.x_defects) {
        const auto &s = lat.x_stabilizers[defect.stab_idx];
        std::cout << "  X[" << defect.stab_idx << "] @("
                  << s.x << "," << s.y << ", t=" << defect.t << ")\n";
    }

    std::cout << "Z defects:\n";
    for (const auto& defect : lat.z_defects) {
        const auto &s = lat.z_stabilizers[defect.stab_idx];
        std::cout << "  Z[" << defect.stab_idx << "] @("
                  << s.x << "," << s.y << ", t=" << defect.t << ")\n";
    }

    std::cout << "================\n";
}

