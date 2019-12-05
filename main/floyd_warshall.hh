#ifndef MAIN_FLOYD_WARSHAL_HH
#define MAIN_FLOYD_WARSHAL_HH

#include <limits>
#include <vector>

#include "lib/types.hh"

using idx_type = std::size_t;

template <typename Weight>
using weighted_graph = std::vector<std::vector<std::pair<idx_type, Weight>>>;

template <typename Weight>
auto count_most_common_edges(const weighted_graph<Weight>& g) -> weighted_graph<int> {
    const auto n = g.size();

    const auto inf = std::numeric_limits<Weight>::infinity();
    std::vector<std::vector<Weight>> dist(n, std::vector<Weight>(n, inf));
    std::vector<std::vector<idx_type>> next(n, std::vector<idx_type>(n, -1));
    for (idx_type u = 0; u < n; u++) {
        for (const auto &[v, w] : g[u]) {
            dist[u][v] = w;
            next[u][v] = v;
        }

        dist[u][u] = 0;
        next[u][u] = u;
    }

    for (idx_type k = 0; k < n; k++) {
        for (idx_type i = 0; i < n; i++) {
            for (idx_type j = 0; j < n; j++) {
                if (dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }

    std::vector<std::vector<int>> counts(n, std::vector<int>(n));
    for (idx_type u = 0; u < n; u++) {
        for (idx_type v = 0; v < n; v++) {
            const auto from_u = next[v][u];
            if (from_u == -1) {
                continue;
            }
            counts[u][from_u]++;
        }
    }

    weighted_graph<int> new_weights(n, std::vector<int>());
    for (idx_type u = 0; u < n; u++) {
        for (const auto &[v, _] : g[u]) {
            new_weights[u].push_back(std::make_pair(v, counts[u][v]));
        }
    }

    return new_weights;
}

#endif
