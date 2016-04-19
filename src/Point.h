#ifndef POINT_H
#define POINT_H
#include<vector>

struct Point{
	std::vector<float> x;
	bool visited;
	int nbhds;
	int cluster;
	float mindist;
};

float measure(Point &p1,Point &p2);

#endif
