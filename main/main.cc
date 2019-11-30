#include "lib/dyadic_tree_metric_embedding.hh"

auto main() -> int {
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
    const auto embedding = dtme.embedding();
    for (unsigned v = 0; v < embedding.size(); v++) {
        const auto &[x, y] = embedding[v];
        std::printf("vertex = %u: (%La, %La)\n", v, x, y);
    }
    return 0;
}
