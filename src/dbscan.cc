#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include<vector>
#include<deque>
#include<iostream>
#include<iomanip>
#include<set>
#include<string>
#include<fstream>
using namespace std;

const TString inputFileName = "../../data/reducedibd.root";
const TString outputFileName = "./output/dbscan/out.root";
const TString prescanFileName = "./output/prescan/prescan.root";
const string argFileName = "./input/input";

struct Point{
	vector<float> x;
	bool visited;
	int nbhds;
	int cluster;
};

int nComp;
double eps;
int minPTS;

vector<double> mean,var;

vector<Point> pts;

float measure(Point &p1, Point &p2);
set<int> regionQuery(int p_idx);
void expandCluster(int p_idx,int curCluster,set<int> &nbhd);
void setTree(TTree *tr,float *tmp);

int main(){
	TFile *f = new TFile(inputFileName,"READ");
	TTree *tr = (TTree*)f->Get("ibdred");
	TFile *fout = new TFile(outputFileName,"RECREATE");
	TTree *tr_out = new TTree("out","out");

	ifstream ifile(argFileName.c_str());

	ifile >> nComp >> eps >> minPTS;
	cout << "Number of components: " << nComp << endl;
	cout << "EPS: " << eps << "	minPTS: " << minPTS << endl;
 	vector<float> tmp(nComp,0);
	for(int i=0;i<nComp;++i){
		mean.push_back(0);
		var.push_back(0);
		string varStr;
		ifile >> varStr;
		tr->SetBranchAddress(varStr.c_str(),&(*(tmp.begin()+i)));
		cout << "INPUT VARIABLE" << i+1 << ": " << varStr << endl; 
	}

	int cluster,nbhds;

	tr_out->Branch("cluster",&cluster);
	tr_out->Branch("nbhd",&nbhds);


	int n = tr->GetEntries();
	cout << "TOTAL EVTS:" << n << endl;

	vector<float> sum(nComp,0);
	vector<float> sumsq(nComp,0);

	for(int i=0;i<n;++i){
		if(i%200000==0)
			cout << setprecision(3) << double(i)*100/n << "%" << endl;
		tr->GetEntry(i);
		Point p_tmp;
		for(int j=0;j<nComp;++j){
			p_tmp.x.push_back(tmp[j]);
			sum[j]+=tmp[j];
			sumsq[j]+=tmp[j]*tmp[j];
		}
		p_tmp.visited=false;
		p_tmp.cluster=-1;
		pts.push_back(p_tmp);
	}
	f->Close();
	cout << "SUMMATION/POINT INITIALIZATION COMPLETE" << endl;
	

	for(int i=0;i<nComp;++i){
		mean[i]=sum[i]/n;
		var[i]=(sumsq[i]-(sum[i]*sum[i])/n)/(n-1);
	}
	

	int curClust=1;

	for(int i=0;i<n;++i){
		if(i%2000==0)
			cout << "DBSCAN PROGRESS:"  << setprecision(4) << float(i)*100/n << "%" << endl;
		if(!pts[i].visited){
			pts[i].visited=1;
			set<int> nbhd = regionQuery(i);
			if(pts[i].nbhds>minPTS){
			//cout << "	NEIGHBORHOODS:" << nb << endl;
				expandCluster(i,curClust,nbhd);
				curClust++;
			}
		}
	}	
	
	for(int i=0;i<n;++i){
		cluster=pts[i].cluster;
		nbhds=pts[i].nbhds;
		tr_out->Fill();
	}
 
	fout->Write();
	fout->Close();
}


float measure(Point &p1,Point &p2){
	float dist=0;
	for(int i=0;i<nComp;++i)
		dist+=(p1.x[i]-p2.x[i])*(p1.x[i]-p2.x[i])/var[i];
	return sqrt(dist);	
}

set<int> regionQuery(int p_idx){
	
	int n = pts.size();

	set<int> nbhd;

	for(int i=0;i<n;++i){
		if(i==p_idx) 
			continue;
		float dist = measure(pts[p_idx],pts[i]);
		if(measure(pts[p_idx],pts[i])<eps)
			nbhd.insert(i);
	}
	pts[p_idx].nbhds=nbhd.size();
	return nbhd;
}

void expandCluster(int p_idx,int curCluster,set<int> &nbhd){
	
	cout << "Expanding cluster at:" << p_idx << "... (this is going to take forever)" << endl;

	pts[p_idx].cluster=curCluster;

	while(!nbhd.empty()){
		set<int>::iterator it = nbhd.begin();
		int idx = *it;
		if(!pts[idx].visited){
			cout << "	Expanding inner cluster at:" << setw(6) << idx << " Remaining:" << setw(6) << nbhd.size() << endl;
			pts[idx].visited=true;
			set<int> nbhd_tmp = regionQuery(idx);
			if(pts[idx].nbhds>minPTS)
				nbhd.insert(nbhd_tmp.begin(),nbhd_tmp.end());		
			
		}
		if(pts[idx].cluster==-1)
			pts[idx].cluster=curCluster;
		nbhd.erase(it);
	}
}
	
