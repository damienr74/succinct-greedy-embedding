cc_library(
	name = 'dyadic-tree-metric-embedding',
	hdrs = ['dyadic_tree_metric_embedding.hh'],
	deps = [
		':heavy-path-decomposition',
		':weight-balanced-tree',
		':types',
	],
	visibility = ['//visibility:public'],
)

cc_library(
	name = 'heavy-path-decomposition',
	hdrs = ['heavy_path_decomposition.hh'],
	deps = [':types'],
	visibility = ['//visibility:public'],
)

cc_library(
	name = 'weight-balanced-tree',
	hdrs = ['weight_balanced_tree.hh'],
	deps = [':types'],
	visibility = ['//visibility:public'],
)

cc_library(
	name = 'types',
	hdrs = ['types.hh'],
	visibility = ['//visibility:public'],
)
