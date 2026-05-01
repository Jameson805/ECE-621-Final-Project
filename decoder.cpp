#include "decoder.hpp"
#include <lemon/dijkstra.h> 
#include <lemon/matching.h>
#include <iostream>
#include <algorithm>

using namespace lemon;

DecoderGraph::DecoderGraph(const Lattice &lat, Type t, int rounds) : type(t), num_rounds(rounds), weight(g) {
    build(lat);
}

void DecoderGraph::build(const Lattice &lat) {
    const std::vector<Stabilizer> &stabs = (type == X) ? lat.x_stabilizers : lat.z_stabilizers;

    const int num_stabs = stabs.size();
    const int num_qubits = lat.num_qubits;

    stab_nodes.clear();
    stab_nodes.resize(num_rounds * num_stabs);

    for (int t = 0; t < num_rounds; t++) {
        for (int i = 0; i < num_stabs; i++) {
            stab_nodes[t * num_stabs + i] = g.addNode();
        }
    }

    struct QubitAdj {
        int count = 0;
        int stabs[2];
    };

    std::vector<QubitAdj> adj(num_qubits);
    for (int s = 0; s < num_stabs; s++) {
        const Stabilizer &stab = stabs[s];

        for (int j = 0; j < stab.degree; j++) {
            int q = stab.neighbors[j];
            if (adj[q].count < 2) {
                adj[q].stabs[adj[q].count++] = s;
            }
        }
    }

    for (int t = 0; t < num_rounds; t++) {
        // 1. Spatial Edges (Data Errors)
        for (int q = 0; q < num_qubits; q++) {
            const QubitAdj &a = adj[q];
            if (a.count == 2) {
                auto u = stab_nodes[t * num_stabs + a.stabs[0]];
                auto v = stab_nodes[t * num_stabs + a.stabs[1]];
                auto e = g.addEdge(u, v);
                weight[e] = 1;
            }
        }

        // 2. Temporal Edges (Measurement Errors)
        if (t < num_rounds - 1) {
            for (int s = 0; s < num_stabs; s++) {
                auto u = stab_nodes[t * num_stabs + s];
                auto v = stab_nodes[(t + 1) * num_stabs + s];
                auto e = g.addEdge(u, v);
                weight[e] = 1;
            }
        }
    }
}

void DecoderGraph::print() const {
    std::cout << "\n=== LEMON Decoder Graph ===\n";

    for (ListGraph::EdgeIt e(g); e != INVALID; ++e) {
        auto u = g.u(e);
        auto v = g.v(e);

        std::cout << g.id(u) << " <--> " << g.id(v) << " (w=" << weight[e] << ")\n";
    }

    std::cout << "===========================\n";
}

std::vector<CorrectionMatch> run_mwpm(const std::vector<SpaceTimeDefect>& defects, const DecoderGraph& dec_graph, const Lattice& lat) {
    std::vector<CorrectionMatch> results;
    int N = defects.size();
    
    if (N == 0) return results;

    lemon::ListGraph match_graph;
    lemon::ListGraph::EdgeMap<int> match_weight(match_graph);
    
    // Create 2N nodes: 0 to N-1 are defects, N to 2N-1 are virtual boundaries
    std::vector<lemon::ListGraph::Node> nodes(2 * N);
    for(int i = 0; i < 2 * N; i++) {
        nodes[i] = match_graph.addNode();
    }

    const int MAX_WEIGHT = 100000; 

    // Connect all N virtual boundaries to each other with Cost = 0
    for(int i = 0; i < N; i++) {
        for(int j = i + 1; j < N; j++) {
            auto e = match_graph.addEdge(nodes[N + i], nodes[N + j]);
            match_weight[e] = MAX_WEIGHT; 
        }
    }

    DecoderGraph::Type type = dec_graph.getType();
    int num_stabs = (type == DecoderGraph::X) ? lat.num_x_stabilizers : lat.num_z_stabilizers;

    for (int i = 0; i < N; i++) {
        int stab_u = defects[i].stab_idx;
        int t_u = defects[i].t; 
        
        lemon::ListGraph::Node dec_u = dec_graph.stab_nodes[t_u * num_stabs + stab_u]; 
        lemon::Dijkstra<lemon::ListGraph, lemon::ListGraph::EdgeMap<int>> dijkstra(dec_graph.graph(), dec_graph.weights());
        dijkstra.run(dec_u);

        // Edges between defects (Cost = physical space-time Dijkstra distance)
        for (int j = i + 1; j < N; j++) {
            int stab_v = defects[j].stab_idx;
            int t_v = defects[j].t;
            
            lemon::ListGraph::Node dec_v = dec_graph.stab_nodes[t_v * num_stabs + stab_v];
            int dist = dijkstra.dist(dec_v);
            
            auto e = match_graph.addEdge(nodes[i], nodes[j]);
            match_weight[e] = MAX_WEIGHT - dist; 
        }

        // Edge from defect to its own virtual spatial boundary 
        const Stabilizer& s = (type == DecoderGraph::X) ? lat.x_stabilizers[stab_u] : lat.z_stabilizers[stab_u];
        
        int dist_to_bound = (type == DecoderGraph::X) 
                            ? std::min(s.y + 1, lat.d - 1 - s.y) 
                            : std::min(s.x + 1, lat.d - 1 - s.x);

        auto e = match_graph.addEdge(nodes[i], nodes[N + i]);
        match_weight[e] = MAX_WEIGHT - dist_to_bound;
    }

    lemon::MaxWeightedPerfectMatching<lemon::ListGraph, lemon::ListGraph::EdgeMap<int>> mwpm(match_graph, match_weight);
    mwpm.run();

    for(int i = 0; i < N; i++) {
        lemon::ListGraph::Node u = nodes[i];
        lemon::ListGraph::Node mate = mwpm.mate(u);
        int mate_idx = -1;
        for(int j = 0; j < 2 * N; j++) {
            if (nodes[j] == mate) {
                mate_idx = j;
                break;
            }
        }

        // Because we only push `stab_idx`, 3D temporal pairs are naturally projected to 2D
        if (mate_idx >= N) {
            results.push_back({defects[i].stab_idx, -1});
        } else if (i < mate_idx) {
            results.push_back({defects[i].stab_idx, defects[mate_idx].stab_idx});
        }
    }

    return results;
}

void print_matches(const std::vector<CorrectionMatch>& matches) {
    std::cout << "\n=== Correction Pairs ===\n";
    if (matches.empty()) {
        std::cout << "No defects to match. Trivial success.\n";
    }
    
    for (const auto& match : matches) {
        if (match.defect_2 == -1) {
            std::cout << "Defect " << match.defect_1 << " <--> Boundary\n";
        } else {
            std::cout << "Defect " << match.defect_1 << " <--> Defect " << match.defect_2 << "\n";
        }
    }
    std::cout << "========================\n";
}
