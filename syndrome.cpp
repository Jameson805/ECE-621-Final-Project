#include <iostream>
#include "syndrome.hpp"

static inline bool anti_commutes_with_x(Pauli p) {
    return (p == Z || p == Y);
}

static inline bool anti_commutes_with_z(Pauli p) {
    return (p == X || p == Y);
}

void compute_syndrome(Lattice &lat) {
    std::cout << "[syndrome] computing defects...\n";

    lat.x_defects.clear();
    lat.z_defects.clear();

    for (int i = 0; i < lat.num_x_stabilizers; i++) {
        const Stabilizer &stab = lat.x_stabilizers[i];

        int parity = 0;

        for (int j = 0; j < stab.degree; j++) {
            int q = stab.neighbors[j];

            Pauli p = lat.errors[q];

            if (p == Z || p == Y) {
                parity ^= 1;
            }
        }

        if (parity == 1) {
            lat.x_defects.push_back(i);
        }
    }

    for (int i = 0; i < lat.num_z_stabilizers; i++) {
        const Stabilizer &stab = lat.z_stabilizers[i];

        int parity = 0;

        for (int j = 0; j < stab.degree; j++) {
            int q = stab.neighbors[j];

            Pauli p = lat.errors[q];

            if (p == X || p == Y) {
                parity ^= 1;
            }
        }

        if (parity == 1) {
            lat.z_defects.push_back(i);
        }
    }
}

void print_defects(const Lattice &lat) {
    std::cout << "\n=== DEFECTS ===\n";

    std::cout << "X defects:\n";
    for (int idx : lat.x_defects) {
        const auto &s = lat.x_stabilizers[idx];
        std::cout << "  X[" << idx << "] @("
                  << s.x << "," << s.y << ")\n";
    }

    std::cout << "Z defects:\n";
    for (int idx : lat.z_defects) {
        const auto &s = lat.z_stabilizers[idx];
        std::cout << "  Z[" << idx << "] @("
                  << s.x << "," << s.y << ")\n";
    }

    std::cout << "================\n";
}
