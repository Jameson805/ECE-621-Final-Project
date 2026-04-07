#include <stdio.h>
#include "simulate.h"
#include "lattice.h"
#include "noise.h"
#include "syndrome.h"
#include "mwpm.h"

void run_simulation() {
    printf("[simulate] running...\n");

    Lattice lat;
    init_lattice(&lat, 3);

    sample_noise(&lat);
    compute_syndrome(&lat);
    run_decoder(&lat);

    printf("[simulate] finished.\n");
}
