#include <vector>

#include "gtest/gtest.h"

#include "lib/weight_balanced_tree.hh"

TEST(weight_balanced_tree, construction) {
    enum v {
        a = 0,
        b,
        c,
        d,
        e,
        f,
        g,
        h,
        i,
        j,
        k,
        no_child,
    };

    std::vector<std::size_t> subtree_sizes{
        11, // a.
        1,  // b.
        1,  // c.
        3,  // d.
        2,  // e.
        1,  // f.
        1,  // g.
        2,  // h.
        3,  // i.
        1,  // j.
        5,  // k.
        1,  // no_child
    };

    std::vector<std::size_t> p1_map{
        b,
        c,
        d,
        j,
        no_child,
        no_child,
        no_child,
    };
    std::vector<std::size_t> p1_weights{
        subtree_sizes[p1_map[0]],
        subtree_sizes[p1_map[1]],
        subtree_sizes[p1_map[2]],
        subtree_sizes[p1_map[3]],
        subtree_sizes[p1_map[4]],
        subtree_sizes[p1_map[5]],
        subtree_sizes[p1_map[6]],
    };

    using WBT = AutocraticWeightBalancedTree<std::size_t>;
    WBT wbt_p1(p1_weights, p1_map);
    EXPECT_EQ(wbt_p1.total_weight, 9);
    tree_type p1_tree{
        {1, 2},
        {9, 10},
        {3, 4},
        {7, 8},
        {5, 6},
        {},
        {},
        {},
        {},
        {11, 12},
        {},
        {},
        {},
    };
    EXPECT_EQ(wbt_p1.tree, p1_tree);
    std::vector<std::pair<int, int>> p1_intervals{
        std::make_pair(0, 7),
        std::make_pair(0, 3),
        std::make_pair(3, 7),
        std::make_pair(3, 5),
        std::make_pair(5, 7),
        std::make_pair(5, 6),
        std::make_pair(6, 7),
        std::make_pair(3, 4),
        std::make_pair(4, 5),
        std::make_pair(0, 2),
        std::make_pair(2, 3),
        std::make_pair(0, 1),
        std::make_pair(1, 2),
    };
    EXPECT_EQ(wbt_p1.interval_nodes, p1_intervals);
    std::vector<int> p1_depths{
        0,
        1,
        1,
        2,
        2,
        3,
        3,
        3,
        3,
        2,
        2,
        3,
        3,
    };
    EXPECT_EQ(wbt_p1.depths, p1_depths);

    std::vector<std::size_t> p2_map{
        no_child, no_child, no_child,
    };
    std::vector<std::size_t> p2_weights{
        subtree_sizes[p2_map[0]],
        subtree_sizes[p2_map[1]],
        subtree_sizes[p2_map[2]],
    };

    WBT wbt_p2(p2_weights, p2_map);
    EXPECT_EQ(wbt_p2.total_weight, 3);
    tree_type p2_tree{
        {1, 2},
        {3, 4},
        {},
        {},
        {},
    };
    EXPECT_EQ(wbt_p2.tree, p2_tree);
    std::vector<std::pair<int, int>> p2_intervals{
        std::make_pair(0, 3),
        std::make_pair(0, 2),
        std::make_pair(2, 3),
        std::make_pair(0, 1),
        std::make_pair(1, 2),
    };
    EXPECT_EQ(wbt_p2.interval_nodes, p2_intervals);
    std::vector<int> p2_depths{
        0,
        1,
        1,
        2,
        2,
    };
    EXPECT_EQ(wbt_p2.depths, p2_depths);
}
