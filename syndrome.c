#include <stdio.h>
#include "syndrome.h"

void compute_syndrome(Lattice *lat) {
    printf("[syndrome] computing syndrome...\n");

    // X stabilizers: detect Z/Y errors
    for (int i = 0; i < lat->num_x_stabilizers; i++) {
        Stabilizer *stab = &lat->x_stabilizers[i];
        int parity = 0;
        for (int j = 0; j < stab->degree; j++) {
            int q = stab->neighbors[j];
            if (lat->errors[q] == 2 || lat->errors[q] == 3) parity ^= 1;
        }
        lat->x_syndrome[i] = parity;
    }

    // Z stabilizers: detect X/Y errors
    for (int i = 0; i < lat->num_z_stabilizers; i++) {
        Stabilizer *stab = &lat->z_stabilizers[i];
        int parity = 0;
        for (int j = 0; j < stab->degree; j++) {
            int q = stab->neighbors[j];
            if (lat->errors[q] == 1 || lat->errors[q] == 3) parity ^= 1;
        }
        lat->z_syndrome[i] = parity;
    }
}

void print_syndrome(Lattice *lat) {
    printf("X syndrome:\n");
    for (int i = 0; i < lat->num_x_stabilizers; i++) {
        printf("  X[%d] at (%d,%d): %d\n", i, lat->x_stabilizers[i].x, lat->x_stabilizers[i].y, lat->x_syndrome[i]);
    }

    printf("Z syndrome:\n");
    for (int i = 0; i < lat->num_z_stabilizers; i++) {
        printf("  Z[%d] at (%d,%d): %d\n", i, lat->z_stabilizers[i].x, lat->z_stabilizers[i].y, lat->z_syndrome[i]);
    }
}
