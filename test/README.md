Most of the tests in this directory at the time of readme writing are based
off of one full example of running the algorithm manually. The example is as
follows.

```
(a)
 |\
 | \
 |\ \
 | \ \
 |\ \ \
 | \ \ \
 |  \ \ \
 |   | \ \
 |   |  \ \
 |   |   | \
 |   |   |  \
(b) (c) (d) (k)
        /   / \
      (e) (i) (j)
       |   |
      (f) (h)
           |
          (g)

for clarification, the adjacency is as follows
(a) -> {b, c, d, k}
(b) -> {}
(c) -> {}
(d) -> {e}
(e) -> {f}
(f) -> {}
(g) -> {}
(h) -> {g}
(i) -> {h}
(j) -> {}
(k) -> {i, j}
```

The heavy path decomposition will form the following heavy paths.
I modified the heavy path decomposition to make paths larger by
including the largest subtree regardless of whether or not that
is more than half of the subtree. All this does is compress the
tree further (the property that this weakens does not apear to
be used in the paper).
```
P1 = {a, k, i, h, g}
P2 = {d, e, f}
P3 = {b}
P4 = {c}
P5 = {j}
```

I will test the building of weight balanced trees on each of the
compressed heavy paths with additional nodes.
