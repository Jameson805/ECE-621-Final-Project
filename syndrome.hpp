#ifndef SYNDROME_HPP
#define SYNDROME_HPP

#include "lattice.hpp"
#include "simulate.hpp"

void compute_syndrome(Lattice &lat, const SimConfig& config);
void print_defects(const Lattice &lat);

#endif
