#include <stdio.h>
#include "lattice.h"

void init_lattice(Lattice *lat, int d) {
    lat->d = d;
    lat->num_qubits = d * d;
    lat->num_x_stabilizers = (d * d - 1) / 2;
    lat->num_z_stabilizers = (d * d - 1) / 2;

    printf("[lattice] d=%d\n", d);
    printf("  qubits = %d\n", lat->num_qubits);
    printf("  X = %d, Z = %d\n", lat->num_x_stabilizers, lat->num_z_stabilizers);
}
