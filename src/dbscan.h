#ifndef DBSCAN_H
#define DBSCAN_H
#include<vector>
#include<set>
#include "Point.h"

std::set<int> regionQuery(int p_idx, float eps, std::vector<Point> &pts);
void expandCluster(int p_idx,int curCluster, std::set<int> &nbhd, float eps, int minPTS, std::vector<Point> &pts);


#endif
	
