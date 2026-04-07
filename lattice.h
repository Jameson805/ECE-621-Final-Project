#ifndef LATTICE_H
#define LATTICE_H

#define MAX_NEIGHBORS 4

typedef struct {
    int x, y;                 
    int degree;               
    int neighbors[MAX_NEIGHBORS]; 
} Stabilizer;

typedef struct {
    int d;
    int num_qubits;

    int num_x_stabilizers;
    int num_z_stabilizers;

    Stabilizer *x_stabilizers;
    Stabilizer *z_stabilizers;

    int *errors;
    int *x_syndrome;
    int *z_syndrome;
} Lattice;

void init_lattice(Lattice *lat, int d);
void free_lattice(Lattice *lat);

int qubit_index(int d, int x, int y);
void qubit_coords(int d, int index, int *x, int *y);

void print_lattice(Lattice *lat);

#endif
