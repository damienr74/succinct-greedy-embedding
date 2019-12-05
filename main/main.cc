#include "lib/dyadic_tree_metric_embedding.hh"

#include "main/spanning_tree_heuristic.hh"
#include "main/kruskals.hh"

#include <chrono>
#include <set>
#include <random>

auto random_geometric_graph(int n, int rectangle_ratio) -> weighted_graph<float> {
    std::vector<std::pair<float, float>> points(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> uniform(1.0, rectangle_ratio*1.0);
    for (int i = 0; i < n; i++) {
        points[i] = std::make_pair(uniform(gen), uniform(gen));
    }

    std::vector<std::tuple<int, int, float>> edges;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            const auto &[xi, yi] = points[i];
            const auto &[xj, yj] = points[j];

            const auto dist = std::pow(yj - yi, 2) + std::pow(xj - xi, 2);
            edges.push_back(std::make_tuple(i, j, dist));
        }
    }

    const auto tuple_dist = [](const auto &t1, const auto &t2) {
        return std::get<2>(t1) < std::get<2>(t2);
    };

    std::sort(edges.begin(), edges.end(), tuple_dist);

    weighted_graph<float> output(n);
    DisjointSets ds(n);

    float maxw = 0;
    for (const auto &[u, v, w] : edges) {
        int fu = ds.find(u);
        int fv = ds.find(v);
        if (fu == fv) { continue; }
        output[u].push_back(std::make_pair(v, w));
        output[v].push_back(std::make_pair(u, w));
        maxw = std::max(maxw, w);
        ds.union_of(u, v);
    }

    for (const auto &[u, v, w] : edges) {
        if (w >= maxw) { break; }    
        bool already = false;
        for (const auto [vp, wp] : output[u]) {
            already |= vp == v;
        }
        if (already) {
            continue;
        }

        output[u].push_back(std::make_pair(v, w));
        output[v].push_back(std::make_pair(u, w));
    }

    return output;
}

auto root_tree(tree_type tree, idx_type root) -> tree_type {
    std::vector<idx_type> stack{root};
    tree_type ret(tree.size());

    std::vector<bool> visited(tree.size());
    while (!stack.empty()) {
        auto node = stack.back();
        visited[node] = 1;
        stack.pop_back();

        for (unsigned i = 0; i < tree[node].size(); i++) {
            const auto &v = tree[node][i];

            if (visited[v]) {
                continue;
            }
            stack.push_back(v);

            // swap nodes.
            if (v == 0) { 
                ret[node].push_back(root);
            } else if (v == root) {
                ret[node].push_back(0);
            } else {
                ret[node].push_back(v);
            }

        }
    }

    return ret;
}


auto main() -> int {
    using Fp = float;

    const int scale = 1024;
    int step = 2;
    int N = 50;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::cout << "n,m,bits,constant,time(ms), maxdeg, avgdeg,\n";
    for (int n = 1; n <= N; n += step) {
        int size = n * scale;
        weighted_graph<Fp> graph = random_geometric_graph(size, 1);

        weighted_edges<Fp> edges;
        for (idx_type u = 0; u < graph.size(); u++) {
            for (const auto [v, w] : graph[u]) {
                edges.push_back(std::make_tuple(u, v, w));
            }
        }

        for (int i = 0; i < 100; i++) {
            std::shuffle(edges.begin(), edges.end(), gen);
            tree_type spanning_tree = kruskals(edges, size);
            tree_type rooted_tree = root_tree(spanning_tree, 0);
            long degree_sum = 0;
            unsigned long degree_max = 0;
            for (idx_type u = 0; u < rooted_tree.size(); u++) {
                degree_sum += rooted_tree[u].size();
                degree_max = std::max(degree_max,
                    rooted_tree[u].size()); 
            }


            auto tik = std::chrono::steady_clock::now();
            DyadicTreeMetricEmbedding<Fp> dtme(rooted_tree);
            auto tok = std::chrono::steady_clock::now();
            const auto bitsize = dtme.coordinate_bitsize();

            std::cout << rooted_tree.size()
                << ", " << (rooted_tree.size() - 1)
                << ", " << bitsize
                << ", " << (bitsize / std::log2(size))
                << ", " << std::chrono::duration<double, std::milli>(tok-tik).count()
                << ", " << degree_max
                << ", " << (degree_sum / float(rooted_tree.size()))
            << '\n';
        }
        #if 0
        const auto embedding = dtme.embedding();
        for (unsigned i = 0; i < embedding.size(); i++) {
            const auto [p1, p2] =  embedding[i];
            std::cout << i << " " << p1.first  << ", " << p1.second << '\n';
            std::cout << i << " " << p2.first  << ", " << p2.second << '\n';
            std::cout << "distance(i, i) = " << dtme.distance(i, i) << '\n';
        }
        for (unsigned u = 0; u < embedding.size(); u++) {
            for (unsigned v = 0; v < embedding.size(); v++) {
                const auto dist = dtme.distance(u, v);
                std::cout << "distance(" << u << ", " << v << ") = " << dist << '\n';
            }
        }
        #endif
    }
    return 0;
}
