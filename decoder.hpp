#ifndef DECODER_HPP
#define DECODER_HPP

#include "lattice.hpp"
#include <lemon/list_graph.h>
#include <vector>

class DecoderGraph {
public:
    enum Type { X, Z };

    DecoderGraph(const Lattice &lat, Type t);

    void build(const Lattice &lat, Type t);
    void print() const;

    const lemon::ListGraph& graph() const { return g; }
    const lemon::ListGraph::EdgeMap<int>& weights() const { return weight; }
    lemon::ListGraph::Node get_boundary() const { return boundary; }

    std::vector<lemon::ListGraph::Node> stab_nodes;

private:
    lemon::ListGraph g;
    lemon::ListGraph::EdgeMap<int> weight;

    lemon::ListGraph::Node boundary;

    void init_maps(); 
};

struct CorrectionMatch {
    int defect_1;
    int defect_2; // -1 represents a match to the boundary
};

std::vector<CorrectionMatch> run_mwpm(const std::vector<int>& defects, const DecoderGraph& dec_graph);
void print_matches(const std::vector<CorrectionMatch>& matches);

#endif
