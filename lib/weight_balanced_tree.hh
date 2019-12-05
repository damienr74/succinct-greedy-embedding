#ifndef LIB_WEIGHT_BALANCED_TREE_HH
#define LIB_WEIGHT_BALANCED_TREE_HH

#include <iostream>
#include <iterator>
#include <numeric>
#include <tuple>
#include <vector>

#include "lib/types.hh"

// Creates a weight balanced tree in the form of a median split tree.
// This is practically the same O(n) construction as the paper.
// The only exception is that instead of doing a doubling search before
// doing a binary search on the range that was found by the doubling
// search, I do the binary search directly since they are asymptotically
// the same, and it is more maintaineable.
template <typename Weight>
struct AutocraticWeightBalancedTree {
    // adjacency representation of tree.
    tree_type tree;
    // interval represented by that node.
    std::vector<std::pair<int, int>> interval_nodes;
    // depth and autocratic depth.
    std::vector<int> depths;
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

        tree.push_back(std::vector<idx_type>());
        interval_nodes.push_back(make_pair(0, weights.size()));
        depths.push_back(0);

        std::vector<Weight> prefix_sum(weights.size() + 1, Weight{0});
        for (idx_type i = 0; i < weights.size(); i++) {
            prefix_sum[i+1] = prefix_sum[i] + weights[i];
        }
        total_weight = prefix_sum.back();

        if (weights.size() == 1) {
            return;
        }

        auto range_sum = [&] (int l, int r) -> Weight {
            return prefix_sum[r] - prefix_sum[l];
        };

        std::vector<idx_type> stack{0};
        while (!stack.empty()) {
            auto node = stack.back();
            auto [l, r] = interval_nodes[node];

            stack.pop_back();

            // If the node is a leaf, there are no children.
            if ((r - l) <= 1) {
                continue;
            }

            auto total = range_sum(l, r);

            // Algorithm does a doubling search before a binary search.. why?
            // Just do the binary search.
            const auto it = std::lower_bound(
                prefix_sum.cbegin() + 1 + l,
                prefix_sum.cbegin() + 1 + r,
                total,
                [&, l = l](const Weight f1, const Weight f2) -> bool {
                return 2*(f1-prefix_sum[l]) < f2;
            });
            const auto dist = [&](int l, int r) -> long {
                if ((r - l) == 2) {
                    return 1l;
                }
                if (it >= (prefix_sum.cbegin() + r)) {
                    return r - l - 1;
                }
                return std::distance(prefix_sum.cbegin() + l, it);
            }(l, r);

            // don't use a reference here... you're inserting... you doofus.
            const auto pdepth = depths[node];
            // left child of node.
            tree[node].push_back(tree.size());
            stack.push_back(tree.size());
            tree.push_back(std::vector<idx_type>());
            interval_nodes.push_back(make_pair(l, l + dist));
            depths.push_back(pdepth + 1);

            // right child of node.
            tree[node].push_back(tree.size());
            stack.push_back(tree.size());
            tree.push_back(std::vector<idx_type>());
            interval_nodes.push_back(make_pair(l + dist, r));
            depths.push_back(pdepth + 1);
        }
    }
};

#endif // LIB_WEIGHT_BALANCED_TREE_HH
