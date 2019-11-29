#ifndef LIB_WEIGHT_BALANCED_TREE_HH
#define LIB_WEIGHT_BALANCED_TREE_HH

#include <iostream>
#include <iterator>
#include <numeric>
#include <tuple>
#include <vector>

// Creates a weight balanced tree in the form of a median split tree.
// This is practically the same O(n) construction as the paper.
// The only exception is that instead of doing a doubling search before
// doing a binary search on the range that was found by the doubling
// search, I do the binary search directly since they are asymptotically
// the same, and it is more maintaineable.
template <typename Weight>
struct AutocraticWeightBalancedTree {
    using idx_type = std::size_t;
    using tree_type = std::vector<std::vector<idx_type>>;

    // adjacency representation of tree.
    tree_type tree;
    // interval represented by that node.
    std::vector<std::pair<int, int>> interval_nodes;
    // depth and autocratic depth.
    std::vector<std::pair<int, int>> depths;
    // original index of the weight.
    std::vector<idx_type> original_index;
    // Total weight of the autocratic weight balanced tree.
    Weight total_weight;

    explicit AutocraticWeightBalancedTree(
        std::tuple<const std::vector<Weight>&, const std::vector<idx_type>&> t)
        : AutocraticWeightBalancedTree(std::get<0>(t), std::get<1>(t)) {}

    // TODO(drobi) clean up logging.
    explicit AutocraticWeightBalancedTree(const std::vector<Weight> &weights,
        const std::vector<idx_type> &original) {
        original_index = original;

        using std::make_pair;

        if (weights.size() == 1) {
            tree = {};
            total_weight = weights[0];
            interval_nodes = {make_pair(0, 0)};
            depths = {make_pair(0, 0)};
        }

        std::vector<Weight> prefix_sum(weights.size() + 1, Weight{0});
        for (idx_type i = 0; i < weights.size(); i++) {
            prefix_sum[i+1] = prefix_sum[i] + weights[i];
            std::cerr << "prefix_sum[" << (i+1) << "] = " <<
                prefix_sum[i+1] << '\n';
        }
        total_weight = prefix_sum.back();

        auto range_sum = [&] (int l, int r) -> Weight {
            return prefix_sum[r] - prefix_sum[l];
        };

        std::vector<idx_type> stack{0};
        tree.push_back(std::vector<idx_type>());
        interval_nodes.push_back(make_pair(0, weights.size()));
        depths.push_back(make_pair(0,0));

        while (!stack.empty()) {
            auto node = stack.back();
            auto [l, r] = interval_nodes[node];

            stack.pop_back();

            // If the node is a leaf, there are no children.
            if ((r - l) <= 1) {
                std::cerr << "node " << node << " is a leaf\n";
                auto &[depth, autocratic] = depths[node];
                autocratic += depth;

                continue;
            }

            auto total = range_sum(l, r);
            std::cerr << "total range sum " << total << '\n';

            // Algorithm does a doubling search before a binary search.. why?
            // Just do the binary search.
            auto it = std::lower_bound(
                prefix_sum.begin() + 1 + l,
                prefix_sum.begin() + 1 + r,
                total,
                [&, l = l](const Weight f1, const Weight f2) -> bool {
                return 2*(f1 - prefix_sum[l]) < f2;
            });
            auto dist = std::distance(prefix_sum.begin() + l, it);
            if ((r - l) == 2) {
                dist = 1;
            }

            const auto &pdepth = depths[node];
            if ((l + dist) >= r) {
                // only child of node.
                std::cerr << "sinle child at [" << l << ", " << r << ")\n";
                std::cerr << "single child at " << tree.size() << "\n";
                std::cerr << "  weight of " << range_sum(l, r) << "\n";
                tree[node].push_back(tree.size());
                tree.push_back(std::vector<idx_type>());
                interval_nodes.push_back(make_pair(l, r));
                depths.push_back(make_pair(pdepth.first + 1, pdepth.first + 1));
                continue;
            }

            // left child of node.
            std::cerr << "left child at [" << l << ", " << (l + dist) << ")\n";
            std::cerr << "left child at " << tree.size() << "\n";
            std::cerr << "  weight of " << range_sum(l, l + dist) << "\n";
            tree[node].push_back(tree.size());
            stack.push_back(tree.size());
            tree.push_back(std::vector<idx_type>());
            interval_nodes.push_back(make_pair(l, l + dist));
            depths.push_back(make_pair(pdepth.first + 1, pdepth.first + 1));

            // right child of node.
            std::cerr << "right child at [" << (l + dist) << ", " << r << ")\n";
            std::cerr << "right child at " << tree.size() << "\n";
            std::cerr << "  weight of " << range_sum(l + dist, r) << "\n";
            tree[node].push_back(tree.size());
            stack.push_back(tree.size());
            tree.push_back(std::vector<idx_type>());
            interval_nodes.push_back(make_pair(l + dist, r));
            depths.push_back(make_pair(pdepth.first + 1, pdepth.first + 1));
        }
    }
};

#endif // LIB_WEIGHT_BALANCED_TREE_HH