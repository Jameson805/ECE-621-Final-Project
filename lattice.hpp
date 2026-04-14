#ifndef LATTICE_HPP
#define LATTICE_HPP

#include <vector>

#define MAX_NEIGHBORS 4

struct Stabilizer {
    int x, y;
    int degree;
    int neighbors[MAX_NEIGHBORS];
};

enum Pauli {
    I = 0,
    X = 1,
    Z = 2,
    Y = 3
};

class Lattice {
public:
    int d;
    int num_qubits;

    int num_x_stabilizers;
    int num_z_stabilizers;

    std::vector<Stabilizer> x_stabilizers;
    std::vector<Stabilizer> z_stabilizers;

    std::vector<Pauli> errors;
    std::vector<int> x_defects;
    std::vector<int> z_defects;

    Lattice() = default;
    Lattice(int d);
    void build(int d);
    void print() const;
};

int qubit_index(int d, int x, int y);
void qubit_coords(int d, int index, int &x, int &y);

#endif
