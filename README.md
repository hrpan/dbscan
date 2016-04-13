# DBSCAN

##What is this?

This is an implementation of Density-based spatial clustering of applications with noise (DBSCAN) used to analyze ROOT data files.
For detail about DBSCAN, please check https://en.wikipedia.org/wiki/DBSCAN


##How to use this?

First you have to compile the sources (duh).

~~~`prescan` performs a preliminary scan on the distance and neighborhood distribution which is useful for determining `eps` 
and `minPTS`. An output file `prescan.root` with a `TTree` containing variables `mindist` (minimum distance to any point 
in the data) and `nbhd` (number of points in the mean `mindist` region). (Note that a `prescan` is necessary before `dbscan`.)~~~
`prescan` is deleted due to its inefficiency.

`dbscan` performs DBSCAN to the required ROOT file and outputs a ROOT file with `TTree` containing the `cluster` for each entry. It 
now requires an input file specifying the number of variables and their `TBranch` names in the analysis (`nComp`). Also the 
`eps` and `minPTS` are required for DBSCAN.

###Input format
The input file should be placed at ./input/input
With the ordering `nComp` `eps` `minPTS` and names of the `TBranch`

####Example input file
3
0.1
100
x
y
z

##Some details
1. Many things (`TTree`, `TBranch`...) are still hard coded, you will have to adapt it manually.
2. Variables are normalized by dividing their deviation.
3. Euclidean metric is used.
~~~4. By default, `eps` = mean of `mindist` and `minPTS` = mean of `nbhd` from `prescan.root`~~~
