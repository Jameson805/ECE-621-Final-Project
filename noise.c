#include <stdio.h>
#include "noise.h"
#include <stdlib.h>
#include <time.h>

// Depolarizing noise model
// TODO: add support for different noise models and automatic selection
void sample_noise(Lattice *lat, double p) {
    printf("[noise] sampling noise for d=%d\n", lat->d);

    // initialize random seed (call once in main instead?)
    srand((unsigned int)time(NULL));

    for (int q = 0; q < lat->num_qubits; q++) {
        double r = (double)rand() / RAND_MAX;
        if (r < p) {
            int type = rand() % 3 + 1;
            lat->errors[q] = type;
        } else {
            lat->errors[q] = 0;
        }
    }
}

void print_noise(Lattice *lat) {
    printf("[noise] qubit errors:\n");
    for (int q = 0; q < lat->num_qubits; q++) {
        if (lat->errors[q] != 0) printf("q%d=%d ", q, lat->errors[q]);
    }
    printf("\n");
}
