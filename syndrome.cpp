#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include "syndrome.hpp"
#include "noise.hpp"
#include "simulate.hpp"

void compute_syndrome(Lattice& lat, const SimConfig& config) {
    lat.x_defects.clear();
    lat.z_defects.clear();

    // 0.0 ratio = Code Capacity (1 round), otherwise Phenomenological (d rounds)
    int num_rounds = (config.p_meas_ratio > 0.0) ? lat.d : 1;

    std::vector<int> prev_x_syndrome(lat.num_x_stabilizers, 0);
    std::vector<int> prev_z_syndrome(lat.num_z_stabilizers, 0);

    for (int t = 0; t < num_rounds; t++) {
        sample_physical_noise(lat, config);

        if (config.verbose) {
            std::cout << "[noise] round t = " << t << " noise state:";
            print_noise(lat);
        }

        std::vector<int> curr_x_syndrome(lat.num_x_stabilizers, 0);
        std::vector<int> curr_z_syndrome(lat.num_z_stabilizers, 0);

        // X-Stabilizers detect Z and Y errors
        for (int i = 0; i < lat.num_x_stabilizers; i++) {
            for (int j = 0; j < lat.x_stabilizers[i].degree; j++) {
                Pauli error = lat.errors[lat.x_stabilizers[i].neighbors[j]];
                if (error == Z || error == Y) curr_x_syndrome[i] ^= 1;
            }
        }

        // Z-Stabilizers detect X and Y errors
        for (int i = 0; i < lat.num_z_stabilizers; i++) {
            for (int j = 0; j < lat.z_stabilizers[i].degree; j++) {
                Pauli error = lat.errors[lat.z_stabilizers[i].neighbors[j]];
                if (error == X || error == Y) curr_z_syndrome[i] ^= 1;
            }
        }

        // Apply measurement noise to all but the final 'perfect' round
        if (config.p_meas_ratio > 0.0 && t < num_rounds - 1) {
            sample_measurement_noise(curr_x_syndrome, config);
            sample_measurement_noise(curr_z_syndrome, config);
        }

        // Compare current syndrome to previous round to find Defects
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
