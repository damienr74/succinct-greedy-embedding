#include "lib/dyadic_tree_metric_embedding.hh"

auto main() -> int {
    // TODO(drobi) write acceptable tests.
    using Fp = long double;

    std::vector<std::size_t> id{0, 1, 2, 3};
    std::vector<Fp> weights1{1.5, 2.4, 3.5, 3.4};
    AutocraticWeightBalancedTree<Fp> tree1(weights1, id);

    std::vector<Fp> weights2{1, 1, 1, 1};
    AutocraticWeightBalancedTree<Fp> tree2(weights2, id);

    std::vector<std::vector<std::size_t>> tree {
        {1, 2}, // 0
        {3, 4}, // 1
        {5, 6}, // 2
        {}, // 3
        {}, // 4
        {}, // 5
        {}, // 6
    };
/*
      (0)
      / \
     /   \
   (1)   (2)
   / \   / \
 (3)(4) (5)(6)
*/

    DyadicTreeMetricEmbedding<Fp> dtme(tree);
    for (const auto& [v, coord] : dtme.point_embedding) {
        std::cout << "vertex = " << v 
            << ": (" << coord.first << ", " << coord.second << ")\n";
    }
    return 0;
}
