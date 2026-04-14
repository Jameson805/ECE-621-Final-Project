#include "decoder.hpp"
#include <lemon/dijkstra.h> 
#include <lemon/matching.h>
#include <string>

using namespace lemon;

DecoderGraph::DecoderGraph(const Lattice &lat, Type t) : weight(g) {
    build(lat, t);
}

void DecoderGraph::build(const Lattice &lat, Type t) {
    const std::vector<Stabilizer> &stabs =
        (t == X) ? lat.x_stabilizers : lat.z_stabilizers;

    const int num_stabs = stabs.size();
    const int num_qubits = lat.num_qubits;

    boundary = g.addNode();

    stab_nodes.resize(num_stabs);
    for (int i = 0; i < num_stabs; i++) {
        stab_nodes[i] = g.addNode();
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

    int edge_count = 0;

    for (int q = 0; q < num_qubits; q++) {
        const QubitAdj &a = adj[q];

        if (a.count == 2) {
            auto u = stab_nodes[a.stabs[0]];
            auto v = stab_nodes[a.stabs[1]];

            auto e = g.addEdge(u, v);
            weight[e] = 1;
            edge_count++;

        } else if (a.count == 1) {
            auto u = stab_nodes[a.stabs[0]];

            auto e = g.addEdge(u, boundary);
            weight[e] = 1;
            edge_count++;
        }
    }
}

void DecoderGraph::print() const {
    std::cout << "\n=== LEMON Decoder Graph ===\n";

    for (ListGraph::EdgeIt e(g); e != INVALID; ++e) {
        auto u = g.u(e);
        auto v = g.v(e);

        std::cout << g.id(u)
                  << " <--> "
                  << g.id(v)
                  << " (w=" << weight[e] << ")\n";
    }

    std::cout << "===========================\n";
}

std::vector<CorrectionMatch> run_mwpm(const std::vector<int>& defects, const DecoderGraph& dec_graph) {
    std::vector<CorrectionMatch> results;
    int N = defects.size();
    
    if (N == 0) return results;

    lemon::ListGraph match_graph;
    lemon::ListGraph::EdgeMap<int> match_weight(match_graph);
    std::vector<lemon::ListGraph::Node> nodes(2 * N);
    for(int i = 0; i < 2 * N; i++) {
        nodes[i] = match_graph.addNode();
    }

    const int MAX_WEIGHT = 100000; 

    // Uses N dummy boundaries connected with weight 0, so all defects can connect to boundary if needed
    for(int i = 0; i < N; i++) {
        for(int j = i + 1; j < N; j++) {
            auto e = match_graph.addEdge(nodes[N + i], nodes[N + j]);
            match_weight[e] = MAX_WEIGHT; 
        }
    }

    for (int i = 0; i < N; i++) {
        int stab_u = defects[i];
        lemon::ListGraph::Node dec_u = dec_graph.stab_nodes[stab_u]; 

        lemon::Dijkstra<lemon::ListGraph, lemon::ListGraph::EdgeMap<int>> dijkstra(dec_graph.graph(), dec_graph.weights());
        dijkstra.run(dec_u);

        for (int j = i + 1; j < N; j++) {
            int stab_v = defects[j];
            lemon::ListGraph::Node dec_v = dec_graph.stab_nodes[stab_v];
            int dist = dijkstra.dist(dec_v);
            auto e = match_graph.addEdge(nodes[i], nodes[j]);
            match_weight[e] = MAX_WEIGHT - dist; 
        }

        int dist_to_bound = dijkstra.dist(dec_graph.get_boundary());
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

        if (mate_idx >= N) {
            results.push_back({defects[i], -1});
        } else if (i < mate_idx) {
            results.push_back({defects[i], defects[mate_idx]});
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
