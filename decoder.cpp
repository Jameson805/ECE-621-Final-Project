#include "decoder.hpp"

struct QubitAdj {
    int count = 0;
    int stabs[2];
};

DecoderGraph::DecoderGraph(const Lattice &lat, Type t) {
    build(lat, t);
}

void DecoderGraph::build(const Lattice &lat, Type t) {
    std::cout << "[decoder] building decoder graph...\n";

    const std::vector<Stabilizer> &stabs =
    (t == X) ? lat.x_stabilizers : lat.z_stabilizers;

    const int num_stabs = stabs.size();
    const int num_qubits = lat.num_qubits;

    int boundary = num_stabs;
    num_nodes = num_stabs + 1;

    int max_edges = 3 * num_qubits;

    head.assign(num_nodes, -1);
    to.resize(max_edges);
    next.resize(max_edges);
    weight.resize(max_edges);

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

    for (int q = 0; q < num_qubits; q++) {
        const QubitAdj &a = adj[q];

        if (a.count == 2) {
            int u = a.stabs[0];
            int v = a.stabs[1];

            to[edge_count] = v;
            weight[edge_count] = 1;
            next[edge_count] = head[u];
            head[u] = edge_count++;

        } else if (a.count == 1) {
            int u = a.stabs[0];

            to[edge_count] = boundary;
            weight[edge_count] = 1;
            next[edge_count] = head[u];
            head[u] = edge_count++;
        }
    }

    head[boundary] = -1;

    std::cout << "[decoder] built graph: nodes="
              << num_nodes
              << " edges="
              << edge_count << "\n";
}

void DecoderGraph::print() const {
    std::cout << "\n=== Decoder Graph ===\n";
    std::cout << "Nodes: " << num_nodes
              << " Edges: " << edge_count << "\n\n";

    for (int u = 0; u < num_nodes; u++) {
        for (int e = head[u]; e != -1; e = next[e]) {

            int v = to[e];

            if (v == num_nodes - 1) {
                std::cout << u << " <--> BOUNDARY (w="
                          << weight[e] << ")\n";
            } else {
                std::cout << u << " <--> " << v
                          << " (w=" << weight[e] << ")\n";
            }
        }
    }

    std::cout << "=====================\n";
}
