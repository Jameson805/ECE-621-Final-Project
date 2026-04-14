#include <iostream>
#include "simulate.hpp"
#include "lattice.hpp"
#include "noise.hpp"
#include "syndrome.hpp"
#include "decoder.hpp"
#include "correction.hpp"
#include "logical.hpp"

void run_simulation(int d, double p) {
    std::cout << "[simulate] running...\n";

    std::cout << "[lattice] initializing lattice for d=" << d << "\n";
    Lattice lat(d);
    lat.print();
    std::cout << "[decoder] building X decoder graph...\n";
    DecoderGraph x_graph(lat, DecoderGraph::X);
    x_graph.print();
    std::cout << "[decoder] building Z decoder graph...\n";
    DecoderGraph z_graph(lat, DecoderGraph::Z);
    z_graph.print();

    std::cout << "[noise] sampling noise for p=" << p << "\n";
    sample_noise(lat, p);
    print_noise(lat);
    std::cout << "[syndrome] computing defects...\n";
    compute_syndrome(lat);
    print_defects(lat);

    std::cout << "[decoder] building X syndrome graph and running mwpm...\n";
    std::vector<CorrectionMatch> x_matches = run_mwpm(lat.x_defects, x_graph);
    print_matches(x_matches);
    std::cout << "[decoder] building Z syndrome graph and running mwpm...\n";
    std::vector<CorrectionMatch> z_matches = run_mwpm(lat.z_defects, z_graph);
    print_matches(z_matches);

    std::cout << "[correction] Applying corrections...\n";
    apply_correction(lat, x_matches, DecoderGraph::X);
    apply_correction(lat, z_matches, DecoderGraph::Z);
    print_noise(lat);

    std::cout << "[logical] evaluating logical errors...\n";
    LogicalResult result = evaluate_logical_errors(lat);
    print_logical_results(result);

    std::cout << "[simulate] finished.\n";
}
