#include <unordered_map>

#include "gtest/gtest.h"

#include "lib/heavy_path_decomposition.hh"

TEST(heavy_path_decomposition, check_decomposition) {
    using hpd = HeavyPathDecomposition;
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
    };
    tree_type tree_adj{
        {b, c, d, k}, // a root.
        {}, // b leaf.
        {}, // c leaf.
        {e}, // d.
        {f}, // e.
        {}, // f.
        {}, // g.
        {g}, // h.
        {h}, // i.
        {}, // j.
        {i, j}, // k.
    };

    hpd decomposition(tree_adj);
    const std::vector<std::size_t> parents{
        a, // a.
        a, // b.
        a, // c.
        a, // d.
        d, // e.
        e, // f.
        h, // g.
        i, // h.
        k, // i.
        k, // j.
        a, // k.
    };
    EXPECT_EQ(decomposition.parent, parents);

    const std::vector<std::size_t> depths{
        0, // a.
        1, // b.
        1, // c.
        1, // d.
        2, // e.
        3, // f.
        4, // g.
        3, // h.
        2, // i.
        2, // j.
        1, // k.
    };
    EXPECT_EQ(decomposition.depth, depths);

    const std::vector<std::size_t> heavies{
        k, // a
        hpd::no_child, // b
        hpd::no_child, // c
        e, // d
        f, // e
        hpd::no_child, // f
        hpd::no_child, // g
        g, // h
        h, // i
        hpd::no_child, // j
        i, // k
    };
    EXPECT_EQ(decomposition.heavy, heavies);

    const std::vector<std::size_t> heads{
        a, // a.
        b, // b.
        c, // c.
        d, // d.
        d, // e.
        d, // f.
        a, // g.
        a, // h.
        a, // i.
        j, // j.
        a, // k.
    };
    EXPECT_EQ(decomposition.head, heads);


    const std::vector<std::size_t> positions{
        0,  // a.
        6,  // b.
        7,  // c.
        8,  // d.
        9,  // e.
        10, // f.
        4,  // g.
        3,  // h.
        2,  // i.
        5,  // j.
        1,  // k.
    };
    EXPECT_EQ(decomposition.pos, positions);

    const std::vector<std::size_t> subtree_sizes{
        11, // a.
        1, // b.
        1, // c.
        3, // d.
        2, // e.
        1, // f.
        1, // g.
        2, // h.
        3, // i.
        1, // j.
        5, // k.
    };
    EXPECT_EQ(decomposition.subtree_size, subtree_sizes);
}
