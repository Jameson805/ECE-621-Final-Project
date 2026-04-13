#include <stdio.h>
#include "decoder.h"

#include <stdlib.h>

DecoderGraph build_decoder_graph(Stabilizer *stabs, int num_stabs, int num_qubits) {
    printf("[decoder] building decoder graph...\n");

    DecoderGraph g;
    g.num_nodes = num_stabs;
    g.num_edges = 0;
    g.edge_count = 0;

    int max_edges = 2 * num_qubits;

    g.head = malloc(num_stabs * sizeof(int));
    g.to = malloc(max_edges * sizeof(int));
    g.next = malloc(max_edges * sizeof(int));
    g.weight = malloc(max_edges * sizeof(int));

    for (int i = 0; i < num_stabs; i++) {
        g.head[i] = -1;
    }

    QubitAdj *adj = calloc(num_qubits, sizeof(QubitAdj));

    // build qubit -> stabilizer adjacency
    for (int s = 0; s < num_stabs; s++) {
        for (int j = 0; j < stabs[s].degree; j++) {
            int q = stabs[s].neighbors[j];

            QubitAdj *a = &adj[q];
            if (a->count < 2) {
                a->stabs[a->count++] = s;
            }
        }
    }

    // convert to graph edges
    for (int q = 0; q < num_qubits; q++) {
        QubitAdj *a = &adj[q];

        if (a->count == 2) {
            int u = a->stabs[0];
            int v = a->stabs[1];

            g.to[g.edge_count] = v;
            g.weight[g.edge_count] = 1;
            g.next[g.edge_count] = g.head[u];
            g.head[u] = g.edge_count++;
        }
        else if (a->count == 1) {
            int u = a->stabs[0];

            // boundary represented as -1 node (special handling later)
            g.to[g.edge_count] = -1;
            g.weight[g.edge_count] = 1;
            g.next[g.edge_count] = g.head[u];
            g.head[u] = g.edge_count++;
        }
    }
    free(adj);
    return g;
}

void free_decoder_graph(DecoderGraph *g) {
    if (!g) return;

    free(g->head);
    free(g->to);
    free(g->next);
    free(g->weight);

    g->head = NULL;
    g->to = NULL;
    g->next = NULL;
    g->weight = NULL;

    g->num_nodes = 0;
    g->num_edges = 0;
    g->edge_count = 0;
}

void print_decoder_graph(DecoderGraph *g) {
    printf("=== Decoder Graph ===\n");
    printf("Nodes: %d\n", g->num_nodes);
    printf("Edges: %d\n\n", g->edge_count);

    for (int u = 0; u < g->num_nodes; u++) {
        for (int e = g->head[u]; e != -1; e = g->next[e]) {
            int v = g->to[e];

            if (v == -1) {
                printf("%d <--> BOUNDARY (w=%d)\n", u, g->weight[e]);
            } else {
                printf("%d <--> %d (w=%d)\n", u, v, g->weight[e]);
            }
        }
    }

    printf("=====================\n");
}

void run_decoder(Lattice *lat) {
    printf("[decoder] running decoder for d=%d\n", lat->d);
}
