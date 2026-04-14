#include "logical.hpp"
#include <iostream>

bool check_logical_z_error(const Lattice& lat) {
    int z_count = 0;
    
    for (int x = 0; x < lat.d; x++) {
        int q = qubit_index(lat.d, x, 0);
        if (lat.errors[q] == Pauli::Z || lat.errors[q] == Pauli::Y) {
            z_count++;
        }
    }
    return (z_count % 2 != 0);
}

bool check_logical_x_error(const Lattice& lat) {
    int x_count = 0;
    
    for (int y = 0; y < lat.d; y++) {
        int q = qubit_index(lat.d, 0, y);
        if (lat.errors[q] == Pauli::X || lat.errors[q] == Pauli::Y) {
            x_count++;
        }
    }

    return (x_count % 2 != 0);
}

LogicalResult evaluate_logical_errors(const Lattice& lat) {
    LogicalResult res;
    res.logical_z_error = check_logical_z_error(lat);
    res.logical_x_error = check_logical_x_error(lat);
    res.success = !(res.logical_z_error || res.logical_x_error);
    
    return res;
}

void print_logical_results(const LogicalResult& result) {
    std::cout << "\n=== Logical Verification ===\n";
    std::cout << "Logical Z Error: " << (result.logical_z_error ? "FAIL (Detected)" : "SUCCESS (None)") << "\n";
    std::cout << "Logical X Error: " << (result.logical_x_error ? "FAIL (Detected)" : "SUCCESS (None)") << "\n";
    std::cout << "Overall Status : " << (!result.success ? "LOGICAL FAILURE" : "DECODING SUCCESS") << "\n";
    std::cout << "============================\n";
}
