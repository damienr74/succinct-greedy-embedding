#ifndef MAIN_PRIMS_HH
#define MAIN_PRIMS_HH

#include "lib/types.hh"
#include "main/floyd_warshall.hh"

template <typename Weight>
auto best_mst_heuristic(weighted_graph<Weight> g) -> tree_type {
    return kruskals(count_most_common_edges(g));
}

#endif
