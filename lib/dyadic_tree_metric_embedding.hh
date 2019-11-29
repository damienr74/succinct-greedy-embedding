#ifndef LIB_DYADIC_TREE_EMBEDDING_HH
#define LIB_DYADIC_TREE_EMBEDDING_HH

#include <bitset>
#include <set>
#include <map>

#include "heavy_path_decomposition.hh"
#include "weight_balanced_tree.hh"

template <class Float>
struct DyadicTreeMetricEmbedding {
    using idx_type = std::size_t;
    using tree_type = std::vector<std::vector<idx_type>>;
    using weight_balanced_tree_type = AutocraticWeightBalancedTree<idx_type>;

    tree_type tree;
    std::map<idx_type, weight_balanced_tree_type> tree_embedding;
    std::map<idx_type, std::pair<Float, Float>> point_embedding;

    constexpr static auto path_length = 128;
    using bitpath = std::bitset<path_length>;

    struct LUT {
        constexpr static auto start_fraction = 0.5;
        long double t[path_length]; // NOLINT.
        constexpr LUT() : t{start_fraction} {
            for (int i = 1; i < path_length; i++) {
                t[i] = t[i-1] / 2;
            }
        }

        constexpr auto operator()(const bitpath &bits, int depth) const -> Float {
            Float total = {0};
            for (int i = path_length; i > path_length - depth; i++) {
                total += (-1 + 2*bits[i-1])*t[path_length - i];
            }
            return total;
        }
    };
    constexpr static LUT lut{};

    static auto lca(const std::vector<bitpath> &children) -> int {
        if (children.size() <= 1) {
            return children.empty() ? -1 : 0;
        }

        int curr_lca = path_length;
        bitpath u{0};
        for (const auto v : children) {
            int lca = ffs(u^v);
            if (lca < curr_lca) {
                curr_lca = lca;
                u = v;
            }
        }

        return curr_lca;
    }
    static auto ffs(bitpath bits) -> int {
        int w = path_length;
        int fs = 0;
        const int bw = 64;

        while(w > 0) {
            auto bottom = (bitpath(~0ULL) & bits).to_ullong();
            auto pfs = __builtin_ffsll(bottom);
            fs = std::max(w * bool(bottom) + pfs, fs);
            w -= bw;
            bits >>= bw;
        }

        return fs;
    }

    constexpr static long double exp = 0.5;
    constexpr static auto no_child = HeavyPathDecomposition::no_child;

    // TODO(drobi) refactor into multiple functions.
    explicit DyadicTreeMetricEmbedding(const tree_type &t) {
        tree = t;
        // For each heavy path p in hpd, build a weight balanced binary
        // tree on the light clidren of each vertex v of p. The weight
        // of the corresponding children are the weights of the subtree
        // rooted at each child.
        //
        // For each of these weight balanced binary trees for the given
        // path p, build a weight balanced binary tree on the trees.
        // The root of this tree now represents the path p in a new tree
        // Z.
        using pathset = std::set<idx_type>;
        using pathmap = std::map<idx_type, std::vector<idx_type>>;


        HeavyPathDecomposition hpd(tree);
        pathmap pm;

        for (auto path_root : pathset(hpd.head.begin(), hpd.head.end())) {
            auto &segments = pm[path_root];

            auto it = path_root;
            while ((it = hpd.heavy[it]) != no_child) {
                segments.push_back(it);
                if (tree[it].size() == 1) {
                    tree[it].push_back(no_child);
                }
            }
        }

        for (const auto [super, segments] : pm) {
            std::vector<idx_type> tree_weights;

            for (const auto v : segments) {
                std::vector<idx_type> mapping;
                std::vector<idx_type> weights;

                const auto degree = tree[v].size();
                mapping.reserve(degree);
                weights.reserve(degree);

                for (auto child : tree[v]) {
                    mapping.push_back(child);
                    weights.push_back(
                        child == no_child ? 1 : hpd.subtree_size[child]);
                }

                auto [t, _] = tree_embedding.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(v),
                    std::forward_as_tuple(weights, mapping));
                tree_weights.push_back(t->second.total_weight);
            }

            tree_embedding.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(idx_type(super)),
                std::forward_as_tuple(tree_weights, segments));
        }

        // DFS and compute the point_embedding of each vertex in original tree.
        using node_descriptor = std::tuple<idx_type, bitpath, int>;
        using std::make_tuple;


        std::vector<node_descriptor> stack{make_tuple(0, 0, 0)};
        std::map<idx_type, std::vector<bitpath>> lca_buffers;
        std::map<idx_type, std::pair<bitpath, int>> tree_paths{};
        while (!stack.empty()) {
            auto [s_idx, s_path, s_depth] = stack.back();
            tree_paths[s_idx] = std::make_pair(s_path, s_depth);
            const auto it = tree_embedding.find(s_idx);
            stack.pop_back();
            if (it == tree_embedding.end()) { continue; }
            const auto s = it->second;

            std::vector<node_descriptor> wbt_stack = {
                make_tuple(0, s_path, s_depth),
            };
            while (!wbt_stack.empty()) {
                auto [v_idx, v_path, v_depth] = wbt_stack.back();
                wbt_stack.pop_back();

                const auto [l, r] = s.interval_nodes[v_idx];
                if (l == r) {
                    const auto v_original = s.original_index[l];
                    stack.push_back(make_tuple(
                        v_original,
                        v_path,
                        v_depth+1
                    ));
                    lca_buffers[hpd.parent[v_original]].push_back(v_path);
                }

                const auto &adj = s.tree[v_idx];
                if (adj.size() >= 1) {
                    wbt_stack.push_back(make_tuple(
                        adj[0], v_path, v_depth + 1
                    ));
                }
                if (adj.size() >= 2) {
                    wbt_stack.push_back(make_tuple(
                        adj[1],
                        v_path | (bitpath(1) << (path_length - 1 - v_depth)),
                        v_depth + 1
                    ));
                }
            }
        }

        for (const auto &[v, _] : tree_paths) {
            const auto &[h_path, h_depth] = tree_paths[hpd.head[v]];
            const auto x = lut(h_path, h_depth);
            const auto y = [&, v=v]() {
                if (lca_buffers[v].size() == 0) {
                    return x;
                }

                return lut(lca_buffers[v][0], lca(lca_buffers[v]));
            }();

            point_embedding[v] = std::make_pair(x, y);
            // must use the path to the LCA to find the the y coord.
        }
    }
};


#endif // LIB_DYADIC_TREE_EMBEDDING_HH
