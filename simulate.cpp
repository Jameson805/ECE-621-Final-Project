#include "simulate.hpp"
#include "syndrome.hpp"
#include "correction.hpp"
#include "noise.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <future>
#include <thread>

LogicalResult run_single_simulation(const SimConfig& config, Lattice& lat, const DecoderGraph& x_graph, const DecoderGraph& z_graph) {
    lat.reset();

    compute_syndrome(lat, config);
    
    std::vector<CorrectionMatch> x_matches = run_mwpm(lat.x_defects, x_graph, lat);
    std::vector<CorrectionMatch> z_matches = run_mwpm(lat.z_defects, z_graph, lat);
    
    apply_correction(lat, x_matches, DecoderGraph::X);
    apply_correction(lat, z_matches, DecoderGraph::Z);
    
    return evaluate_logical_errors(lat);
}

void run_threshold_sweep(const std::vector<int>& distances, const std::vector<double>& probabilities, int num_shots, bool use_measurement_errors) {
    std::ostringstream filename_stream;
    filename_stream << "results/sweep";
    filename_stream << "_meas" << (use_measurement_errors ? "1" : "0");
    filename_stream << "_shots" << num_shots;
    if (!distances.empty()) {
        filename_stream << "_d" << distances.front() << "to" << distances.back();
    }
    filename_stream << ".csv";
    
    std::string filename = filename_stream.str();

    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open " << filename << " for writing.\n";
        return;
    }
    outfile << "d,p,logical_error_rate,total_fails,num_shots\n";

    std::cout << "\n=== Starting Threshold Sweep ===\n";
    std::cout << "Model: " << (use_measurement_errors ? "Phenomenological (3D)" : "Code Capacity (2D)") << "\n";
    std::cout << "Output: " << filename << "\n";

    // Determine how many hardware threads your CPU supports
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4; // Fallback

    for (int d : distances) {
        std::cout << "\nInitializing graphs for d = " << d << "...\n";
        
        Lattice dummy_lat(d); 
        int num_rounds = use_measurement_errors ? d : 1; 
        
        DecoderGraph x_graph(dummy_lat, DecoderGraph::X, num_rounds);
        DecoderGraph z_graph(dummy_lat, DecoderGraph::Z, num_rounds);

        for (double p : probabilities) {
            SimConfig config{d, p, use_measurement_errors, false};
            
            std::vector<std::future<int>> futures;

            for (int t = 0; t < num_threads; t++) {
                int shots_for_thread = num_shots / num_threads + (t < (num_shots % num_threads) ? 1 : 0);

                futures.push_back(std::async(std::launch::async, [shots_for_thread, config, &x_graph, &z_graph, d]() {
                    int thread_fails = 0;
                    
                    Lattice thread_lat(d); 
                    for (int i = 0; i < shots_for_thread; i++) {
                        LogicalResult res = run_single_simulation(config, thread_lat, x_graph, z_graph);
                        if (!res.success) thread_fails++;
                    }
                    return thread_fails;
                }));
            }

            // Wait for all threads to finish and sum up the fails
            int total_fails = 0;
            for (auto& f : futures) {
                total_fails += f.get();
            }

            double p_L = static_cast<double>(total_fails) / num_shots;
            
            outfile << d << "," << p << "," << p_L << "," << total_fails << "," << num_shots << "\n";
            std::cout << "  p = " << std::left << std::setw(6) << p 
                      << " | p_L = " << p_L 
                      << " (" << total_fails << "/" << num_shots << ")\n";

            if (p_L >= 0.66) {
                std::cout << "  -> Saturation reached (p_L >= 0.66). Skipping remaining probabilities for d=" << d << ".\n";
                break;
            }
        }
    }
    
    outfile.close();
    std::cout << "=== Sweep Complete. Data saved to " << filename << " ===\n";
}

//Runs a single verbose monte carlo for a given configuration, for debugging
void run_monte_carlo(const SimConfig& config, int num_shots) {
    std::cout << "Starting Monte Carlo...\n";
    std::cout << "d = " << config.d << ", p = " << config.p << ", shots = " << num_shots << "\n";

    Lattice lat(config.d); 
    int num_rounds = config.use_measurement_errors ? config.d : 1;
    DecoderGraph x_graph(lat, DecoderGraph::X, num_rounds);
    DecoderGraph z_graph(lat, DecoderGraph::Z, num_rounds);

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

//Runs a single simulation with detailed printouts, for debugging
void run_verbose_simulation(const SimConfig& config) {
    std::cout << "[simulate] running...\n";

    std::cout << "[lattice] initializing lattice for d=" << config.d << "\n";
    Lattice lat(config.d);
    lat.print();
    int num_rounds = config.use_measurement_errors ? config.d : 1;
    std::cout << "[decoder] building X decoder graph...\n";
    DecoderGraph x_graph(lat, DecoderGraph::X, num_rounds);
    x_graph.print();
    std::cout << "[decoder] building Z decoder graph...\n";
    DecoderGraph z_graph(lat, DecoderGraph::Z, num_rounds);
    z_graph.print();

    std::cout << "[syndrome] sampling noise and computing defects...\n";
    compute_syndrome(lat, config);
    print_defects(lat);

    std::cout << "[decoder] building X syndrome graph and running mwpm...\n";
    std::vector<CorrectionMatch> x_matches = run_mwpm(lat.x_defects, x_graph, lat);
    print_matches(x_matches);
    std::cout << "[decoder] building Z syndrome graph and running mwpm...\n";
    std::vector<CorrectionMatch> z_matches = run_mwpm(lat.z_defects, z_graph, lat);
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
