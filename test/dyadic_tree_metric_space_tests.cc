#include "gtest/gtest.h"

#include "lib/dyadic_tree_metric_embedding.hh"

TEST(dyadic_tree_metric_embedding, embed) {
    using Fp = long double;
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
    hpd::tree_type tree{
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

    DyadicTreeMetricEmbedding<Fp> dtme(tree);
    std::vector<std::pair<Fp, Fp>> coordinates{
        std::make_pair(0x8p-4, 0x8p-4),     // a.
        std::make_pair(0x8p-11, 0x8p-11),   // b.
        std::make_pair(0x8.4p-6, 0x8.4p-6), // c.
        std::make_pair(0xCp-5, 0x8.02p-5),  // d.
        std::make_pair(0xCp-5, 0x9.02p-5),  // e.
        std::make_pair(0xCp-5, 0xA.08p-5),  // f.
        std::make_pair(0x8p-4, 0xE.1p-4),   // g.
        std::make_pair(0x8p-4, 0xC.1p-4),   // h.
        std::make_pair(0x8p-4, 0xA.1p-4),   // i.
        std::make_pair(0x8.1p-4, 0x8.1p-4), // j.
        std::make_pair(0x8p-4, 0x8.1p-4),   // k.
    };
    EXPECT_EQ(dtme.embedding(), coordinates);
}
