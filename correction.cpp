#include "correction.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

// Bitwise XOR to apply Pauli multiplication
static void apply_pauli(Lattice& lat, int q, Pauli p) {
    if (p == Pauli::I) return;
    lat.errors[q] = static_cast<Pauli>(lat.errors[q] ^ p);
}

static void route_manhattan(Lattice& lat, const Stabilizer& s1, const Stabilizer& s2, Pauli p) {
    int curr_x = s1.x;
    int curr_y = s1.y;
    int target_x = s2.x;
    int target_y = s2.y;

    while (curr_x != target_x || curr_y != target_y) {
        int next_x = curr_x;
        int next_y = curr_y;

        if (curr_x != target_x && curr_y != target_y) {
            next_x += (target_x > curr_x) ? 1 : -1;
            next_y += (target_y > curr_y) ? 1 : -1;
        } else if (curr_x != target_x) {
            next_x += (target_x > curr_x) ? 1 : -1;
            next_y += (curr_y + 1 < lat.d) ? 1 : -1; 
        } else if (curr_y != target_y) {
            next_y += (target_y > curr_y) ? 1 : -1;
            next_x += (curr_x + 1 < lat.d) ? 1 : -1;
        }

        int q_x = std::max(curr_x, next_x);
        int q_y = std::max(curr_y, next_y);
        apply_pauli(lat, qubit_index(lat.d, q_x, q_y), p);

        curr_x = next_x;
        curr_y = next_y;
    }
}

static void route_to_boundary(Lattice& lat, const Stabilizer& start, DecoderGraph::Type type, Pauli p) {
    int curr_x = start.x;
    int curr_y = start.y;

    if (type == DecoderGraph::Z) {
        // Z defects route to top (y=-1) or bottom (y=d-1)
        int target_y = (std::abs(curr_y - (-1)) <= std::abs(curr_y - (lat.d - 1))) ? -1 : (lat.d - 1);
        
        while (curr_y != target_y) {
            int next_y = curr_y + ((target_y > curr_y) ? 1 : -1);
            int next_x = curr_x + ((curr_x + 1 < lat.d) ? 1 : -1); // zig-zag in X
            
            int q_x = std::max(curr_x, next_x);
            int q_y = std::max(curr_y, next_y);
            apply_pauli(lat, qubit_index(lat.d, q_x, q_y), p);
            
            curr_x = next_x;
            curr_y = next_y;
        }
    } else {
        // X defects route to left (x=-1) or right (x=d-1)
        int target_x = (std::abs(curr_x - (-1)) <= std::abs(curr_x - (lat.d - 1))) ? -1 : (lat.d - 1);
        
        while (curr_x != target_x) {
            int next_x = curr_x + ((target_x > curr_x) ? 1 : -1);
            int next_y = curr_y + ((curr_y + 1 < lat.d) ? 1 : -1); // zig-zag in Y
            
            int q_x = std::max(curr_x, next_x);
            int q_y = std::max(curr_y, next_y);
            apply_pauli(lat, qubit_index(lat.d, q_x, q_y), p);
            
            curr_x = next_x;
            curr_y = next_y;
        }
    }
}

void apply_correction(Lattice& lat, const std::vector<CorrectionMatch>& matches, DecoderGraph::Type type) {
    Pauli correction_pauli = (type == DecoderGraph::Z) ? Pauli::X : Pauli::Z;
    const std::vector<Stabilizer>& stabs = (type == DecoderGraph::X) ? lat.x_stabilizers : lat.z_stabilizers;

    for (const auto& match : matches) {
        if (match.defect_2 == -1) {
            route_to_boundary(lat, stabs[match.defect_1], type, correction_pauli);
        } else {
            route_manhattan(lat, stabs[match.defect_1], stabs[match.defect_2], correction_pauli);
        }
    }
}
