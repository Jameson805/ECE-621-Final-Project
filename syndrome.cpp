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

    int num_rounds = (config.p_meas_ratio > 0.0) ? lat.d : 1;

    std::fill(lat.prev_x_syndrome.begin(), lat.prev_x_syndrome.end(), 0);
    std::fill(lat.prev_z_syndrome.begin(), lat.prev_z_syndrome.end(), 0);

    for (int t = 0; t < num_rounds; t++) {
        sample_physical_noise(lat, config);

        if (config.verbose) {
            std::cout << "[noise] round t = " << t << " noise state:";
            print_noise(lat);
        }

        std::fill(lat.curr_x_syndrome.begin(), lat.curr_x_syndrome.end(), 0);
        std::fill(lat.curr_z_syndrome.begin(), lat.curr_z_syndrome.end(), 0);

        // Shoutout to Gemini for this branchless syndrome calculation using the bitwise Pauli representation
        // Don't fully get it but confirmed it works and it's very fast, so leaving it as is
        // X-Stabilizers detect Z and Y errors (the 2s bit)
        for (int i = 0; i < lat.num_x_stabilizers; i++) {
            int parity = 0;
            const auto& stab = lat.x_stabilizers[i];
            for (int j = 0; j < stab.degree; j++) {
                parity ^= lat.errors[stab.neighbors[j]];
            }
            lat.curr_x_syndrome[i] = (parity >> 1) & 1;
        }

        // Z-Stabilizers detect X and Y errors (the 1s bit)
        for (int i = 0; i < lat.num_z_stabilizers; i++) {
            int parity = 0;
            const auto& stab = lat.z_stabilizers[i];
            for (int j = 0; j < stab.degree; j++) {
                parity ^= lat.errors[stab.neighbors[j]];
            }
            lat.curr_z_syndrome[i] = parity & 1;
        }

        // Apply measurement noise
        if (config.p_meas_ratio > 0.0 && t < num_rounds - 1) {
            sample_measurement_noise(lat.curr_x_syndrome, config);
            sample_measurement_noise(lat.curr_z_syndrome, config);
        }

        // Compare current syndrome to previous round to find Defects
        for (int i = 0; i < lat.num_x_stabilizers; i++) {
            if (lat.curr_x_syndrome[i] != lat.prev_x_syndrome[i]) {
                lat.x_defects.push_back({i, t});
            }
        }
        for (int i = 0; i < lat.num_z_stabilizers; i++) {
            if (lat.curr_z_syndrome[i] != lat.prev_z_syndrome[i]) {
                lat.z_defects.push_back({i, t});
            }
        }

        std::swap(lat.prev_x_syndrome, lat.curr_x_syndrome);
        std::swap(lat.prev_z_syndrome, lat.curr_z_syndrome);
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
