#include <iostream>
#include <random>
#include <thread>
#include <cstdint>
#include "simulate.hpp"
#include "noise.hpp"

// Ultra-fast PRNG for Monte Carlo (satisfies C++ UniformRandomBitGenerator)
struct Xoshiro256StarStar {
    using result_type = uint64_t;
    uint64_t s[4];

    static constexpr uint64_t min() { return 0; }
    static constexpr uint64_t max() { return UINT64_MAX; }

    Xoshiro256StarStar(uint64_t seed) {
        // SplitMix64 seeder to initialize the state
        uint64_t z = seed;
        for (int i = 0; i < 4; i++) {
            z += 0x9e3779b97f4a7c15;
            uint64_t x = z;
            x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
            x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
            s[i] = x ^ (x >> 31);
        }
    }

    static inline uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    uint64_t operator()() {
        const uint64_t result = rotl(s[1] * 5, 7) * 9;
        const uint64_t t = s[1] << 17;
        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];
        s[2] ^= t;
        s[3] = rotl(s[3], 45);
        return result;
    }
};

thread_local Xoshiro256StarStar rng(std::random_device{}() + std::hash<std::thread::id>{}(std::this_thread::get_id()));
thread_local std::uniform_real_distribution<double> prob_dist(0.0, 1.0);

void sample_physical_noise(Lattice &lat, const SimConfig &config) {
    if (config.noise_model == NoiseModel::INDEPENDENT) {
        for (int q = 0; q < lat.num_qubits; q++) {
            bool err_x = (prob_dist(rng) < config.p_ind);
            bool err_z = (prob_dist(rng) < config.p_ind);

            if (err_x && err_z) {
                lat.errors[q] = static_cast<Pauli>(lat.errors[q] ^ Pauli::Y);
            } else if (err_x) {
                lat.errors[q] = static_cast<Pauli>(lat.errors[q] ^ Pauli::X);
            } else if (err_z) {
                lat.errors[q] = static_cast<Pauli>(lat.errors[q] ^ Pauli::Z);
            }
        }
    } else {
        for (int q = 0; q < lat.num_qubits; q++) {
            double r = prob_dist(rng);
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
    
    for (size_t i = 0; i < syndrome.size(); i++) {
        if (prob_dist(rng) < config.p_meas) {
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
