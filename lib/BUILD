cc_library(
	name = 'heavy-path-decomposition',
	hdrs = ['heavy_path_decomposition.hh'],
	visibility = ['//visibility:public'],
)

cc_library(
	name ='weight-balanced-tree',
	hdrs = ['weight_balanced_tree.hh'],
	visibility = ['//visibility:public'],
)

cc_library(
	name = 'dyadic-tree-metric-embedding',
	hdrs = ['dyadic_tree_metric_embedding.hh'],
	deps = [
		':heavy-path-decomposition',
		':weight-balanced-tree',
	],
	visibility = ['//visibility:public'],
)
