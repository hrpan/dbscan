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

//const TString inputFileName = "../../data/reducedibd.root";
//const TString outputFileName = "./output/dbscan/out.root";
const string argFileName = "./input/input";

struct Point{
	vector<float> x;
	bool visited;
	int nbhds;
	int cluster;
	float mindist;
};

int nComp;
double eps;
int minPTS;

vector<double> mean,var;

vector<Point> pts;

float measure(Point &p1, Point &p2);
set<int> regionQuery(int p_idx);
void expandCluster(int p_idx,int curCluster,set<int> &nbhd);
void setup(vector<float> &tmp,TFile **f,TTree **tr,TFile **fout,TTree **tr_out);


int main(){
	TFile *f,*fout;
	TTree *tr,*tr_out;

	vector<float> tmp;
	setup(tmp,&f,&tr,&fout,&tr_out);
	cout << "SETUP COMPLETE" << endl;

	int cluster,nbhds;
	float mindist;
	tr_out->Branch("cluster",&cluster);
	tr_out->Branch("nbhd",&nbhds);
	tr_out->Branch("mindist",&mindist);

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
		p_tmp.mindist=999;
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
		mindist=pts[i].mindist;
		tr_out->Fill();
	}
 
	fout->Write();
	fout->Close();
}

void setup(vector<float> &tmp,TFile **f,TTree **tr,TFile **fout,TTree **tr_out){
	ifstream ifile(argFileName.c_str());

	TString inputFileName,inputTreeName;
	ifile >> inputFileName >> inputTreeName;
	*f = new TFile(inputFileName,"READ");
	*tr = (TTree*)((*f)->Get(inputTreeName));
	cout << "INPUT FILE: " << inputFileName << endl;
	cout << "INPUT TREE: " << inputTreeName << endl;

	TString outputFileName,outputTreeName;
	ifile >> outputFileName >> outputTreeName;
	*fout = new TFile(outputFileName,"RECREATE");
	*tr_out = new TTree(outputTreeName,outputTreeName);
	cout << "OUTPUT FILE: " << outputFileName << endl;
	cout << "OUTPUT TREE: " << outputTreeName << endl;

	ifile >> nComp >> eps >> minPTS;
	cout << "Number of components: " << nComp << endl;
	cout << "EPS: " << eps << "	minPTS: " << minPTS << endl;

	tmp.resize(nComp,0);
	mean.resize(nComp,0);
	var.resize(nComp,0);
	for(int i=0;i<nComp;++i){
		string varStr;
		ifile >> varStr;
		(*tr)->SetBranchAddress(varStr.c_str(),&(*(tmp.begin()+i)));
		cout << "SETTING INPUT VARIABLE" << i+1 << ": " << varStr << endl; 
	}


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
		pts[p_idx].mindist=min(dist,pts[p_idx].mindist);
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
	
