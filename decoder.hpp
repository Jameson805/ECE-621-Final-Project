#ifndef DECODER_HPP
#define DECODER_HPP

#include "lattice.hpp"
#include <vector>
#include <iostream>

class DecoderGraph {
public:
    enum Type { X, Z };

    DecoderGraph(const Lattice &lat, Type t);

    void build(const Lattice &lat, Type t);
    void print() const;

private:
    int num_nodes = 0;
    int edge_count = 0;

    std::vector<int> head;
    std::vector<int> to;
    std::vector<int> next;
    std::vector<int> weight;
};

#endif
