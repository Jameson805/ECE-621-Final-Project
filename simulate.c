#include <stdio.h>
#include "simulate.h"
#include "lattice.h"
#include "noise.h"
#include "syndrome.h"
#include "mwpm.h"

void run_simulation() {
    printf("[simulate] running...\n");

    Lattice lat;
    int d = 3; // example code distance
    init_lattice(&lat, d);
    print_lattice(&lat);

    double p = 0.2; // example error probability
    sample_noise(&lat, p);
    print_noise(&lat);

    compute_syndrome(&lat);
    print_syndrome(&lat);

    run_decoder(&lat);

    free_lattice(&lat);
    printf("[simulate] finished.\n");
}
