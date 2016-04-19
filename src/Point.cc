#include<cmath>
#include "Point.h"
using namespace std;


float measure(Point &p1,Point &p2){
	float dist=0;
	static int nComp = p1.x.size();
	for(int i=0;i<nComp;++i)
		dist+=(p1.x[i]-p2.x[i])*(p1.x[i]-p2.x[i]);
	return sqrt(dist);	
}
