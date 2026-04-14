#include <iostream>
#include "simulate.hpp"
#include "lattice.hpp"
#include "noise.hpp"
#include "syndrome.hpp"
#include "decoder.hpp"
#include "correction.hpp"
#include "logical.hpp"

LogicalResult run_single_simulation(const SimConfig& config, Lattice& lat, const DecoderGraph& x_graph, const DecoderGraph& z_graph) {
    lat.reset();
    sample_noise(lat, config.p);
    compute_syndrome(lat);
    std::vector<CorrectionMatch> x_matches = run_mwpm(lat.x_defects, x_graph);
    std::vector<CorrectionMatch> z_matches = run_mwpm(lat.z_defects, z_graph);
    apply_correction(lat, x_matches, DecoderGraph::X);
    apply_correction(lat, z_matches, DecoderGraph::Z);
    return evaluate_logical_errors(lat);
}

void run_monte_carlo(const SimConfig& config, int num_shots) {
    std::cout << "Starting Monte Carlo...\n";
    std::cout << "d = " << config.d << ", p = " << config.p << ", shots = " << num_shots << "\n";

    Lattice lat(config.d); 
    DecoderGraph x_graph(lat, DecoderGraph::X);
    DecoderGraph z_graph(lat, DecoderGraph::Z);

    int logical_x_fails = 0;
    int logical_z_fails = 0;
    int total_fails = 0;

    for (int i = 0; i < num_shots; i++) {
        LogicalResult res = run_single_simulation(config, lat, x_graph, z_graph);

        if (res.logical_x_error) logical_x_fails++;
        if (res.logical_z_error) logical_z_fails++;
        if (!res.success) total_fails++;

        if ((i + 1) % (num_shots / 10) == 0) {
            std::cout << "  Progress: " << (i + 1) << " / " << num_shots << "\n";
        }
    }

    double p_L = static_cast<double>(total_fails) / num_shots;
    std::cout << "\n=== Monte Carlo Results ===\n";
    std::cout << "Total Fails: " << total_fails << " / " << num_shots << "\n";
    std::cout << "Logical Error Rate (p_L): " << p_L << "\n";
    std::cout << "===========================\n";
}

void run_verbose_simulation(const SimConfig& config) {
    std::cout << "[simulate] running...\n";

    std::cout << "[lattice] initializing lattice for d=" << config.d << "\n";
    Lattice lat(config.d);
    lat.print();
    std::cout << "[decoder] building X decoder graph...\n";
    DecoderGraph x_graph(lat, DecoderGraph::X);
    x_graph.print();
    std::cout << "[decoder] building Z decoder graph...\n";
    DecoderGraph z_graph(lat, DecoderGraph::Z);
    z_graph.print();

    std::cout << "[noise] sampling noise for p=" << config.p << "\n";
    sample_noise(lat, config.p);
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
