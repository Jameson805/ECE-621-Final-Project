#ifndef NOISE_HPP
#define NOISE_HPP

#include "lattice.hpp"

void sample_physical_noise(Lattice &lat, const SimConfig &config);
void sample_measurement_noise(std::vector<int>& syndrome, const SimConfig &config);
void print_noise(const Lattice &lat);

#endif
