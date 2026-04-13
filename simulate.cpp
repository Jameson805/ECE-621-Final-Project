#include <iostream>
#include "simulate.hpp"
#include "lattice.hpp"
#include "noise.hpp"
#include "syndrome.hpp"
#include "decoder.hpp"

void run_simulation() {
    std::cout << "[simulate] running...\n";

    int d = 5;
    double p = 0.2;

    Lattice lat(d);
    lat.print();
    DecoderGraph z_graph(lat, DecoderGraph::Z);
    z_graph.print();
    DecoderGraph x_graph(lat, DecoderGraph::X);
    x_graph.print();

    sample_noise(lat, p);
    print_noise(lat);

    compute_syndrome(lat);
    print_defects(lat);

    std::cout << "[simulate] finished.\n";
}
