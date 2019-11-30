#ifndef LIB_DYADIC_TREE_EMBEDDING_HH
#define LIB_DYADIC_TREE_EMBEDDING_HH

#include <bitset>
#include <set>
#include <map>

#include "heavy_path_decomposition.hh"
#include "weight_balanced_tree.hh"

template <class Float>
class DyadicTreeMetricEmbedding {
public:
    constexpr static auto path_length = 128;
    using idx_type = std::size_t;
    using tree_type = std::vector<std::vector<idx_type>>;
    using weight_balanced_tree_type = AutocraticWeightBalancedTree<idx_type>;
    using bitpath = std::bitset<path_length>;
    using embedding_map = std::vector<std::pair<Float, Float>>;

    explicit DyadicTreeMetricEmbedding(const tree_type &t)
        : tree{t}
        , point_embedding(t.size()) 
        , tree_paths{}
    {

        tree = t;
        HeavyPathDecomposition hpd(tree);

        const auto pm = fix_heavy_path_children(hpd);

        for (const auto &[super, segment] : pm) {
            std::vector<idx_type> super_weights;
            std::vector<idx_type> super_children;

            for (const auto v : segment) {
                for (auto child : tree[v]) {
                    // Ignore the heavy child.
                    if (child == hpd.heavy[v]) { continue; }

                    super_children.push_back(child);
                    super_weights.push_back(hpd.subtree_size[child]);
                }
            }

            tree_embedding.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(idx_type(super)),
                std::forward_as_tuple(super_weights, super_children));
        }

        dfs_and_compute_point_embedding(hpd);
        compute_embedding(hpd);
    }

    auto embedding() -> embedding_map const { return point_embedding; }

private:
    tree_type tree;
    std::map<idx_type, weight_balanced_tree_type> tree_embedding;
    embedding_map point_embedding;
    std::map<idx_type, std::pair<bitpath, int>> tree_paths{};

    struct LUT {
        constexpr static auto start_fraction = 0.5;
        long double t[path_length]; // NOLINT.
        constexpr LUT() : t{start_fraction} {
            for (int i = 1; i < path_length; i++) {
                t[i] = t[i-1] / 2;
            }
        }

        constexpr auto operator()(const bitpath &bits, int depth) const -> Float {
            Float total = {start_fraction};
            for (int i = path_length-1; i > path_length - depth; i--) {
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

    using pathmap = std::map<idx_type, std::vector<idx_type>>;
    // add the dummy node the the vertices in the heavy paths.
    // also returns a path map i.e. a map that contains the vertices
    // of each heavy path in order of parent to child.
    auto fix_heavy_path_children(HeavyPathDecomposition &hpd) -> pathmap {
        constexpr auto no_child = HeavyPathDecomposition::no_child;
        pathmap pm;
        using pathset = std::set<idx_type>;

        for (auto path_root : pathset(hpd.head.begin(), hpd.head.end())) {
            auto &segments = pm[path_root];

            auto it = path_root;
            do {
                segments.push_back(it);
                // size <= 1
                if (tree[it].size() == 1 ||
                    (tree[it].size() == 0 && it != hpd.head[it])) {
                    const auto dummy = tree.size();
                    tree.push_back({});
                    tree[it].push_back(dummy);

                    // insert light_leaf into heavy path decomposition.
                    hpd.parent.push_back(it);
                    hpd.heavy.push_back(no_child);
                    hpd.head.push_back(dummy);
                    hpd.depth.push_back(hpd.depth[it] + 1);
                    hpd.pos.push_back(hpd.pos.size());
                    hpd.subtree_size.push_back(1);
                }
            } while ((it = hpd.heavy[it]) != no_child);
        }

        return pm;
    }

    // DFS and compute the point_embedding of each vertex in original tree.
    void dfs_and_compute_point_embedding(const HeavyPathDecomposition &hpd) {
        using node_descriptor = std::tuple<idx_type, bitpath, int>;
        using std::make_tuple;

        std::vector<node_descriptor> stack{make_tuple(0, 0, 0)};
        while (!stack.empty()) {
            auto [s_idx, s_path, s_depth] = stack.back();
            if (hpd.subtree_size[s_idx] == 1) {
                s_depth += s_depth;
            }

            tree_paths[s_idx] = std::make_pair(s_path, s_depth);
            const auto it = tree_embedding.find(s_idx);
            stack.pop_back();
            if (it == tree_embedding.end()) {
                continue;
            }
            const auto s = it->second;

            std::vector<node_descriptor> wbt_stack = {
                make_tuple(0, s_path, s_depth),
            };
            while (!wbt_stack.empty()) {
                auto [v_idx, v_path, v_depth] = wbt_stack.back();
                wbt_stack.pop_back();

                const auto [l, r] = s.interval_nodes[v_idx];
                if ((l + 1) == r) {
                    const auto v_original = s.original_index[l];
                    stack.push_back(make_tuple(
                        v_original,
                        v_path,
                        v_depth+1
                    ));
                }

                const auto &adj = s.tree[v_idx];
                if (adj.size() >= 1) {
                    wbt_stack.push_back(make_tuple(
                        adj[0], v_path, v_depth + 1
                    ));
                }
                if (adj.size() >= 2) {
                    const auto next_bit = ((path_length - 1) - (v_depth + 1));
                    wbt_stack.push_back(make_tuple(
                        adj[1],
                        v_path | (bitpath(1) << next_bit),
                        v_depth + 1
                    ));
                }
            }
        }
    }

    const void compute_embedding(const HeavyPathDecomposition &hpd) {
        for (int v = 0; v < hpd.n; v++) {
            const auto &[h_path, h_depth] = tree_paths[hpd.head[v]];
            const auto x = lut(h_path, h_depth);
            const auto y = [&, &children = tree[v]]() {
                if (children.size() == 0) {
                    return x;
                }

                std::vector<bitpath> lca_buffer;
                std::vector<int> lca_depth;
                for (const auto c : children) {
                    if (c == hpd.heavy[v]) { continue; }
                    const auto &[bp, d] = tree_paths[c];
                    lca_buffer.push_back(bp);
                    lca_depth.push_back(d);
                }
                if (lca_buffer.size() == 1) {
                    return lut(lca_buffer[0], lca_depth[0]);
                }

                return lut(lca_buffer[0], lca(lca_buffer));
            }();

            point_embedding[v] = std::make_pair(x, y);
        }
    }
};

#endif // LIB_DYADIC_TREE_EMBEDDING_HH
