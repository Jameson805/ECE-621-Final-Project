#ifndef DECODER_H
#define DECODER_H

#include "lattice.h"

typedef struct {
    int num_nodes;

    int num_edges;

    int *head;
    int *to;
    int *next;
    int *weight;

    int edge_count;
} DecoderGraph;

typedef struct {
    int count;
    int stabs[2];  // surface code: max 2
} QubitAdj;

DecoderGraph build_decoder_graph(Stabilizer *stabs, int num_stabs, int num_qubits);
void free_decoder_graph(DecoderGraph *g);
void print_decoder_graph(DecoderGraph *g);

void run_decoder(Lattice *lat);

#endif
