/*
 * File: distance_vector_routing.c
 * Assignment 7: Implementation of Distance Vector Routing Algorithm in C
 * Description: Simulates the Distributed Bellman-Ford Distance Vector Routing Protocol (like RIP).
 *              Each router maintains a routing table of shortest distances to all destinations
 *              and exchanges distance vectors with adjacent neighboring nodes until convergence.
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_NODES 10
#define INF 9999

typedef struct {
    int distance[MAX_NODES];
    int next_hop[MAX_NODES];
} RoutingTable;

int main() {
    int num_nodes;
    int cost_matrix[MAX_NODES][MAX_NODES];
    RoutingTable routers[MAX_NODES];

    printf("===================================================================\n");
    printf("  Assignment 7: Distance Vector Routing Algorithm (Bellman-Ford) in C\n");
    printf("===================================================================\n\n");

    // Default 4-node network topology if user wants automated simulation
    num_nodes = 4;
    printf("Simulating 4-Node Network Topology (Nodes 0, 1, 2, 3):\n");
    printf("Link Costs:\n");
    printf("  (0 <-> 1) = 2\n");
    printf("  (0 <-> 2) = 5\n");
    printf("  (1 <-> 2) = 1\n");
    printf("  (1 <-> 3) = 4\n");
    printf("  (2 <-> 3) = 2\n\n");

    int default_matrix[4][4] = {
        {0,   2,   5,   INF},
        {2,   0,   1,   4},
        {5,   1,   0,   2},
        {INF, 4,   2,   0}
    };

    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            cost_matrix[i][j] = default_matrix[i][j];
            routers[i].distance[j] = cost_matrix[i][j];
            if (cost_matrix[i][j] != INF && i != j) {
                routers[i].next_hop[j] = j;
            } else {
                routers[i].next_hop[j] = i;
            }
        }
    }

    // Bellman-Ford Iterative Convergence
    int updated;
    int iteration = 0;
    do {
        updated = 0;
        iteration++;
        printf("--- Convergence Round #%d ---\n", iteration);

        for (int i = 0; i < num_nodes; i++) {
            for (int j = 0; j < num_nodes; j++) {
                for (int k = 0; k < num_nodes; k++) {
                    // If distance through neighbor k is shorter than direct distance
                    if (routers[i].distance[j] > cost_matrix[i][k] + routers[k].distance[j]) {
                        routers[i].distance[j] = cost_matrix[i][k] + routers[k].distance[j];
                        routers[i].next_hop[j] = k;
                        updated = 1;
                    }
                }
            }
        }
    } while (updated);

    printf("\nRouting Tables Converged in %d iteration(s)!\n\n", iteration);

    // Display Final Converged Routing Tables
    for (int i = 0; i < num_nodes; i++) {
        printf("----------------------------------------\n");
        printf(" Routing Table for Node %d:\n", i);
        printf(" Destination Node | Shortest Distance | Next Hop\n");
        printf("----------------------------------------\n");
        for (int j = 0; j < num_nodes; j++) {
            if (routers[i].distance[j] >= INF) {
                printf("        %d         |     Unreachable   |    ---\n", j);
            } else {
                printf("        %d         |         %d         |     %d\n",
                       j, routers[i].distance[j], routers[i].next_hop[j]);
            }
        }
        printf("----------------------------------------\n\n");
    }

    return 0;
}
