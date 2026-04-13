#include <stdio.h>
#include "simulate.h"
#include "lattice.h"
#include "noise.h"
#include "syndrome.h"
#include "decoder.h"

void run_simulation() {
    printf("[simulate] running...\n");

    Lattice lat;
    int d = 3; // example code distance
    init_lattice(&lat, d);
    print_lattice(&lat);

    DecoderGraph z_graph = build_decoder_graph(
        lat.x_stabilizers,
        lat.num_x_stabilizers,
        lat.num_qubits
    );
    DecoderGraph x_graph = build_decoder_graph(
        lat.z_stabilizers,
        lat.num_z_stabilizers,
        lat.num_qubits
    );
    print_decoder_graph(&z_graph);
    print_decoder_graph(&x_graph);

    double p = 0.2; // example error probability
    sample_noise(&lat, p);
    print_noise(&lat);

    compute_syndrome(&lat);
    print_syndrome(&lat);

    run_decoder(&lat);

    free_lattice(&lat);
    free_decoder_graph(&z_graph);
    free_decoder_graph(&x_graph);
    printf("[simulate] finished.\n");
}
