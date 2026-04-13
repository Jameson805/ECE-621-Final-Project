#include <iostream>
#include "lattice.hpp"

Lattice::Lattice(int d) {
    build(d);
}

void Lattice::build(int d) {
    std::cout << "[lattice] initializing d=" << d << "\n";

    this->d = d;
    num_qubits = d * d;

    num_x_stabilizers = (d * d - 1) / 2;
    num_z_stabilizers = (d * d - 1) / 2;

    x_stabilizers.resize(num_x_stabilizers);
    z_stabilizers.resize(num_z_stabilizers);

    errors.assign(num_qubits, Pauli::I);
    x_defects.clear();
    z_defects.clear();

    int x_idx = 0;
    int z_idx = 0;

    // Central stabilizers
    for (int x = 0; x < d - 1; x++) {
        for (int y = 0; y < d - 1; y++) {

            Stabilizer stab;
            stab.x = x;
            stab.y = y;
            stab.degree = 4;

            stab.neighbors[0] = qubit_index(d, x, y);
            stab.neighbors[1] = qubit_index(d, x+1, y);
            stab.neighbors[2] = qubit_index(d, x, y+1);
            stab.neighbors[3] = qubit_index(d, x+1, y+1);

            if ((x + y) % 2 == 0)
                x_stabilizers[x_idx++] = stab;
            else
                z_stabilizers[z_idx++] = stab;
        }
    }

    // Border stabilizers
    for (int i = 0; i < d - 1; i++) {

        if (i % 2 == 0) {
            Stabilizer stab;
            stab.x = i;
            stab.y = -1;
            stab.degree = 2;

            stab.neighbors[0] = qubit_index(d, i, 0);
            stab.neighbors[1] = qubit_index(d, i+1, 0);

            z_stabilizers[z_idx++] = stab;
        }

        if (((i + (d-1)) % 2) == 1) {
            Stabilizer stab;
            stab.x = i;
            stab.y = d-1;
            stab.degree = 2;

            stab.neighbors[0] = qubit_index(d, i, d-1);
            stab.neighbors[1] = qubit_index(d, i+1, d-1);

            z_stabilizers[z_idx++] = stab;
        }

        if (i % 2 == 1) {
            Stabilizer stab;
            stab.x = -1;
            stab.y = i;
            stab.degree = 2;

            stab.neighbors[0] = qubit_index(d, 0, i);
            stab.neighbors[1] = qubit_index(d, 0, i+1);

            x_stabilizers[x_idx++] = stab;
        }

        if (((d-1 + i) % 2) == 0) {
            Stabilizer stab;
            stab.x = d-1;
            stab.y = i;
            stab.degree = 2;

            stab.neighbors[0] = qubit_index(d, d-1, i);
            stab.neighbors[1] = qubit_index(d, d-1, i+1);

            x_stabilizers[x_idx++] = stab;
        }
    }
}

int qubit_index(int d, int x, int y) {
    return d * y + x;
}

void qubit_coords(int d, int index, int &x, int &y) {
    x = index % d;
    y = index / d;
}

static void print_stabilizer(
    const Lattice &lat,
    const Stabilizer &s,
    const char type,
    int idx
) {
    std::cout << type << "[" << idx << "] ("
              << s.x << "," << s.y << "), deg=" << s.degree
              << " : ";

    for (int j = 0; j < s.degree; j++) {
        int q = s.neighbors[j];
        int x, y;
        qubit_coords(lat.d, q, x, y);

        std::cout << q << "=(" << x << "," << y << ")";

        if (j + 1 < s.degree)
            std::cout << " | ";
    }

    std::cout << "\n";
}

void Lattice::print() const {
    std::cout << "\n========== LATTICE d=" << d << " ==========\n";
    std::cout << "Qubits: " << num_qubits << "\n";
    std::cout << "X stabilizers: " << num_x_stabilizers << "\n";
    std::cout << "Z stabilizers: " << num_z_stabilizers << "\n\n";

    std::cout << "---- X stabilizers ----\n";
    for (int i = 0; i < num_x_stabilizers; i++) {
        print_stabilizer(*this, x_stabilizers[i], 'X', i);
    }

    std::cout << "\n---- Z stabilizers ----\n";
    for (int i = 0; i < num_z_stabilizers; i++) {
        print_stabilizer(*this, z_stabilizers[i], 'Z', i);
    }

    std::cout << "=====================================\n";
}
