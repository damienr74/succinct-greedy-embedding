#include "lib/weight_balanced_tree.hh"
#include "lib/heavy_path_decomposition.hh"

int main() {
    // TODO(drobi) write acceptable tests.
    using Fp = long double;
    std::vector<Fp> weights1{1.5, 2.4, 3.5, 3.4};
    AutocraticWeightBalancedTree<Fp> tree1(weights1);

    std::vector<Fp> weights2{1, 1, 1, 1};
    AutocraticWeightBalancedTree<Fp> tree2(weights2);
    return 0;
}
