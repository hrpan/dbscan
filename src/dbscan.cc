#include "Point.h"
#include<vector>
#include<iostream>
#include<iomanip>
#include<set>
#include<algorithm>
	
std::set<int> regionQuery(int p_idx, float eps, std::vector<Point> &pts){
	
	static int n = pts.size();

	std::set<int> nbhd;

	for(int i=0;i<n;++i){
		if(i==p_idx) 
			continue;
		float dist = measure(pts[p_idx],pts[i]);
		if(measure(pts[p_idx],pts[i])<eps)
			nbhd.insert(i);
		pts[p_idx].mindist=std::min(dist,pts[p_idx].mindist);
	}
	pts[p_idx].nbhds=nbhd.size();
	return nbhd;
}

void expandCluster(int p_idx,int curCluster,std::set<int> &nbhd, float eps, int minPTS, std::vector<Point> &pts){
	
	std::cout << "Expanding cluster at:" << p_idx << "... (this is going to take forever)" << std::endl;

	pts[p_idx].cluster=curCluster;

	while(!nbhd.empty()){
		std::set<int>::iterator it = nbhd.begin();
		int idx = *it;
		if(!pts[idx].visited){
			std::cout << "    Expanding inner cluster at:" << std::setw(10) << idx;
			std::cout << " Remaining:" << std::setw(10) << nbhd.size() << std::endl;
			pts[idx].visited=true;
			std::set<int> nbhd_tmp = regionQuery(idx,eps,pts);
			if(pts[idx].nbhds>minPTS)
				nbhd.insert(nbhd_tmp.begin(),nbhd_tmp.end());		
			
		}
		if(pts[idx].cluster==-1)
			pts[idx].cluster=curCluster;
		nbhd.erase(it);
	}
}
	
