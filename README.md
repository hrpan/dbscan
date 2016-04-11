# DBSCAN

##What is this?

This is an implementation of Density-based spatial clustering of applications with noise (DBSCAN) used to analyze ROOT data files.
For detail about DBSCAN, please check https://en.wikipedia.org/wiki/DBSCAN


##How to use this?

First you have to compile the sources (duh).

`prescan` performs a preliminary scan on the distance and neighborhood distribution which is useful for determining `eps` 
and `minPTS`. An output file `prescan.root` with a `TTree` containing variables `mindist` (minimum distance to any point 
in the data) and `nbhd` (number of points in the mean `mindist` region). (Note that a `prescan` is necessary before `dbscan`.)

`dbscan` performs the DBSCAN and output a ROOT file with `TTree` containing the `cluster` for each entry.

##Some details
1. Many things (`TTree`, `TBranch`...) are still hard coded, you will have to adapt it manually.
2. Variables are normalized by dividing their deviation.
3. Euclidean metric is used.
4. By default, `eps` = mean of `mindist` and `minPTS` = mean of `nbhd` from `prescan.root`
