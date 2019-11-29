cc_test(
	name = "heavy-path-decomposition",
	srcs = [
		"heavy_path_decomposition_tests.cc",
	],
	copts = ["-Iexternal/gtest/include"],
	deps = [
		"@gtest//:main",
		"//lib:dyadic-tree-metric-embedding",
	],
	size = "small",
)

cc_test(
	name = "weight-balanced-tree",
	srcs = [
		"weight_balanced_tree_tests.cc",
	],
	copts = ["-Iexternal/gtest/include"],
	deps = [
		"@gtest//:main",
		"//lib:dyadic-tree-metric-embedding",
	],
	size = "small",
)
