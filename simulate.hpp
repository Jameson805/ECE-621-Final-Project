#ifndef SIMULATE_HPP
#define SIMULATE_HPP

#include "logical.hpp"
#include <cmath>

enum class NoiseModel {
    INDEPENDENT,
    DEPOLARIZING,
    BIASED
};

struct SimConfig {
    int d;
    double p;
    NoiseModel noise_model;
    double p_meas_ratio; 
    double bias_eta; 
    bool verbose;

    // Pre-calculated exact probabilities
    double p_x;
    double p_y;
    double p_z;
    double p_meas;
    double p_ind; // Used only for independent Bernoulli trials

    // Constructor to automatically compute the exact Pauli probabilities
    SimConfig(int d_, double p_, NoiseModel model_, double meas_ratio_, double eta_ = 10.0, bool v_ = false)
        : d(d_), p(p_), noise_model(model_), p_meas_ratio(meas_ratio_), bias_eta(eta_), verbose(v_) {
        
        p_meas = p * p_meas_ratio;
        p_x = p_y = p_z = p_ind = 0.0;

        if (noise_model == NoiseModel::INDEPENDENT) {
            p_ind = 1.0 - std::sqrt(1.0 - p);
            p_x = p_ind;
            p_z = p_ind;
            p_y = p_ind * p_ind; 
        } 
        else if (noise_model == NoiseModel::DEPOLARIZING) {
            p_x = p / 3.0;
            p_z = p / 3.0;
            p_y = p / 3.0;
        } 
        else if (noise_model == NoiseModel::BIASED) {
            p_z = p * (bias_eta / (bias_eta + 1.0));
            double p_bit_flip = p * (1.0 / (bias_eta + 1.0));
            p_x = p_bit_flip / 2.0;
            p_y = p_bit_flip / 2.0;
        }
    }
};

void run_verbose_simulation(const SimConfig& config);
void run_monte_carlo(const SimConfig& config, int num_shots);
void run_threshold_sweep(const std::vector<int>& distances, const std::vector<double>& probabilities, int num_shots, NoiseModel noise_model, double p_meas_ratio, double bias_eta);

#endif
