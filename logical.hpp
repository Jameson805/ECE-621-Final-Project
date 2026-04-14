#ifndef LOGICAL_HPP
#define LOGICAL_HPP

#include "lattice.hpp"

struct LogicalResult {
    bool logical_z_error;
    bool logical_x_error;
    bool success; 
};

bool check_logical_z_error(const Lattice& lat);
bool check_logical_x_error(const Lattice& lat);
LogicalResult evaluate_logical_errors(const Lattice& lat);
void print_logical_results(const LogicalResult& result);

#endif
