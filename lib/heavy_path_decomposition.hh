#ifndef LIB_HEAVY_LIGHT_DECOMPOSITION
#define LIB_HEAVY_LIGHT_DECOMPOSITION

#include <vector>

// Create heavy path decomposition data structure to allow for fast
// computation of arbitrary node properties via segment trees and or
// prefix sums/mins/operation.
struct HeavyPathDecomposition {
    using idx_type = std::size_t;
    using tree_type = std::vector<std::vector<idx_type>>;
    static constexpr idx_type no_child = ~idx_type{0};

    idx_type n;
    std::vector<idx_type> parent; // parent of a tree node.
    std::vector<idx_type> depth; // depth of a tree node.
    std::vector<idx_type> heavy; // which child is the heavy one.
    std::vector<idx_type> head; // topmost vertex of the current heavy path.
    std::vector<idx_type> pos; // contiguous positioning of nodes for queries.
    std::vector<idx_type> subtree_size;

    auto dfs(idx_type v, const tree_type &tree) -> int {
        int size = 1;
        int max_subtree_size = 0;

        for (idx_type child : tree[v]) {
            parent[child] = v;
            depth[child] = depth[v] + 1;
            int subtree_size = dfs(child, tree);
            size += subtree_size;
            if (subtree_size > max_subtree_size) {
                max_subtree_size = subtree_size;
                heavy[v] = child;
            }
        }

#if 0
        // Investigate whether this reduces or increases the number of bits
        // (most likely increases).
        if (max_subtree_size < size/2) {
            heavy[v] = no_child;
        }
#endif
        return subtree_size[v] = size;
    }

    void decompose(idx_type v, idx_type h, const tree_type &tree, idx_type &cur) {
        head[v] = h;
        pos[v] = cur++;
        if (heavy[v] != no_child) {
            decompose(heavy[v], h, tree, cur);
        }

        for (idx_type child : tree[v]) {
            if (child != parent[v] && child != idx_type(heavy[v])) {
                decompose(child, child, tree, cur);
            }
        }
    }

public:
    explicit HeavyPathDecomposition(const tree_type &tree)
        : n(tree.size())
        , parent(n)
        , depth(n)
        , heavy(n, no_child)
        , head(n)
        , pos(n)
        , subtree_size(n)
        {

        dfs(0, tree);
        idx_type current_position = 0;
        decompose(0, 0, tree, current_position);
    }
};

#endif // LIB_HEAVY_LIGHT_DECOMPOSITION
