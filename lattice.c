#include <stdio.h>
#include <stdlib.h>
#include "lattice.h"

void init_lattice(Lattice *lat, int d) {
    printf("[lattice] initializing d=%d...\n", d);

    lat->d = d;
    lat->num_qubits = d * d;
    lat->num_x_stabilizers = (d * d - 1) / 2;
    lat->num_z_stabilizers = (d * d - 1) / 2;

    lat->x_stabilizers = malloc(lat->num_x_stabilizers * sizeof(Stabilizer));
    lat->z_stabilizers = malloc(lat->num_z_stabilizers * sizeof(Stabilizer));
    lat->errors = (int*)calloc(lat->num_qubits, sizeof(int));
    lat->x_syndrome = (int*)calloc(lat->num_x_stabilizers, sizeof(int));
    lat->z_syndrome = (int*)calloc(lat->num_z_stabilizers, sizeof(int));

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
                lat->x_stabilizers[x_idx++] = stab;
            else
                lat->z_stabilizers[z_idx++] = stab;
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

            lat->z_stabilizers[z_idx++] = stab;
        }
        if (((i + (d-1)) % 2) == 1) {
            Stabilizer stab;
            stab.x = i;
            stab.y = d-1;
            stab.degree = 2;

            stab.neighbors[0] = qubit_index(d, i, d-1);
            stab.neighbors[1] = qubit_index(d, i+1, d-1);

            lat->z_stabilizers[z_idx++] = stab;
        }
        if (i % 2 == 1) {
            Stabilizer stab;
            stab.x = -1;
            stab.y = i;
            stab.degree = 2;

            stab.neighbors[0] = qubit_index(d, 0, i);
            stab.neighbors[1] = qubit_index(d, 0, i+1);

            lat->x_stabilizers[x_idx++] = stab;
        }
        if (((d-1 + i) % 2) == 0) {
            Stabilizer stab;
            stab.x = d-1;
            stab.y = i;
            stab.degree = 2;

            stab.neighbors[0] = qubit_index(d, d-1, i);
            stab.neighbors[1] = qubit_index(d, d-1, i+1);

            lat->x_stabilizers[x_idx++] = stab;
        }
    }
}

void free_lattice(Lattice *lat) {
    free(lat->x_stabilizers);
    free(lat->z_stabilizers);
    free(lat->errors);
    free(lat->x_syndrome);
    free(lat->z_syndrome);
}

int qubit_index(int d, int x, int y) {
    return d * y + x;
}

void qubit_coords(int d, int index, int *x, int *y) {
    *x = index % d;
    *y = index / d;
}

void print_stabilizer(Lattice *lat, Stabilizer *stab, const char *type, int idx) {
    printf("%s[%d] at (%d,%d), degree=%d: ", type, idx, stab->x, stab->y, stab->degree);

    for (int i = 0; i < stab->degree; i++) {
        int q = stab->neighbors[i];
        int x, y;
        qubit_coords(lat->d, q, &x, &y);

        printf("%d=(%d,%d)", q, x, y);
        if (i < stab->degree - 1) printf(", ");
    }

    printf("\n");
}

void print_lattice(Lattice *lat) {
    printf("=== Lattice d=%d ===\n", lat->d);
    printf("Qubits: %d\n", lat->num_qubits);
    printf("X stabilizers: %d\n", lat->num_x_stabilizers);
    printf("Z stabilizers: %d\n\n", lat->num_z_stabilizers);

    printf("---- X stabilizers ----\n");
    for (int i = 0; i < lat->num_x_stabilizers; i++) {
        print_stabilizer(lat, &lat->x_stabilizers[i], "X", i);
    }

    printf("\n---- Z stabilizers ----\n");
    for (int i = 0; i < lat->num_z_stabilizers; i++) {
        print_stabilizer(lat, &lat->z_stabilizers[i], "Z", i);
    }

    printf("=======================\n");
}
