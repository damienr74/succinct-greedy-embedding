load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
	name = "gtest",
	url = "https://github.com/google/googletest/archive/release-1.7.0.zip",
	sha256 = "b58cb7547a28b2c718d1e38aee18a3659c9e3ff52440297e965f5edffe34b6d0",
	build_file = "@//external:gtest.BUILD",
	strip_prefix = "googletest-release-1.7.0",
)

http_archive(
	name = "benchmark",
	url = "https://github.com/google/benchmark/archive/v1.5.0.tar.gz",
	strip_prefix = "benchmark-1.5.0",
	sha256 = "3c6a165b6ecc948967a1ead710d4a181d7b0fbcaa183ef7ea84604994966221a",
)
