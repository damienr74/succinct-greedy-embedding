#include "gtest/gtest.h"

#include "lib/dyadic_tree_metric_embedding.hh"

TEST(dyadic_tree_metric_embedding, embed) {
    using Fp = long double;
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
    tree_type tree{
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
}
