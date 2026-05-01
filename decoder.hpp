#ifndef DECODER_HPP
#define DECODER_HPP

#include "lattice.hpp"
#include <lemon/list_graph.h>
#include <vector>

class DecoderGraph {
public:
    enum Type { X, Z };

    DecoderGraph(const Lattice &lat, Type t, int num_rounds = 1);

    void build(const Lattice &lat);
    void print() const;

    const lemon::ListGraph& graph() const { return g; }
    const lemon::ListGraph::EdgeMap<int>& weights() const { return weight; }
    Type getType() const { return type; }
    int getNumRounds() const { return num_rounds; }

    std::vector<lemon::ListGraph::Node> stab_nodes;

private:
    Type type;
    int num_rounds;
    lemon::ListGraph g;
    lemon::ListGraph::EdgeMap<int> weight;
};

struct CorrectionMatch {
    int defect_1;
    int defect_2; // -1 represents a match to the boundary
};

std::vector<CorrectionMatch> run_mwpm(const std::vector<SpaceTimeDefect>& defects, const DecoderGraph& dec_graph, const Lattice& lat);
void print_matches(const std::vector<CorrectionMatch>& matches);

#endif
