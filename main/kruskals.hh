#ifndef MAIN_KRUSALS_HH
#define MAIN_KRUSALS_HH

#include "main/spanning_tree_heuristic.hh"

#include <algorithm>
#include <vector>

struct DisjointSets {
    std::vector<int> id;
    std::vector<int> sizes;

    explicit DisjointSets(int n)
        : id(n)
        , sizes(n, 1) {
        std::iota(id.begin(), id.end(), 0);
    }

    auto find(int v) -> int {
        while (v != id[v]) {
            id[v] = id[id[v]];
            v = id[v];
        }
        return v;
    }

    void union_of(int u, int v) {
        int i = find(u);
        int j = find(v);
        if (i == j) { return; }

        if (sizes[i] < sizes[j]) {
            std::swap(i, j);
        } 

        id[j] = i;
        sizes[i] += sizes[j];
    }
};

template <typename Weight>
using weighted_edges = std::vector<std::tuple<idx_type, idx_type, Weight>>;

template <typename Weight>
auto maximum_spanning_tree(const weighted_graph<Weight>& g) -> tree_type {
    const auto n = g.size();
    weighted_edges<Weight> edges;
    for (idx_type u = 0; u < n; u++) {
        for (const auto &[v, w] : g) {
            edges.push_back(std::make_tuple(u, v, w));
        }
    }

    std::sort(edges.begin(), edges.end(), [](const auto &t1, const auto &t2) {
        return std::get<2>(t1) > std::get<2>(t2);
    });

    return kruskals(edges, n);
}

template <typename Weight>
auto kruskals(const weighted_edges<Weight> &edges, int n) -> tree_type {
    tree_type tree(n);
    DisjointSets ds(n);

    for (const auto &[u, v, w] : edges) {
        int fu = ds.find(u);
        int fv = ds.find(v);
        if (fu == fv) { continue; }
        tree[u].push_back(v);
        tree[v].push_back(u);
        ds.union_of(u, v);
    }

    return tree;
}

#endif // MAIN_KRUSALS_HH
