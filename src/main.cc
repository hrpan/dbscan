#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "Point.h"
#include "dbscan.h"
#include<vector>
#include<iostream>
#include<iomanip>
#include<set>
#include<string>
#include<fstream>
using namespace std;

const string argFileName = "./input/input";


void setup(vector<float> &tmp,TFile **f,TTree **tr,TFile **fout,TTree **tr_out,int &nComp,float &eps,int &minPTS);
void ptsInit(vector<float> &tmp,TTree *tr,vector<Point> &pts,int nComp);
void ptsNorm(vector<float> &sum, vector<float> &sumsq, vector<Point> &pts,int nComp);


int main(){
	TFile *f,*fout;
	TTree *tr,*tr_out;

	vector<float> tmp;
	int nComp,minPTS;
	float eps;
	setup(tmp,&f,&tr,&fout,&tr_out,nComp,eps,minPTS);

	int cluster,nbhds;
	float mindist;
	tr_out->Branch("cluster",&cluster);
	tr_out->Branch("nbhd",&nbhds);
	tr_out->Branch("mindist",&mindist);


	vector<Point> pts;
	ptsInit(tmp,tr,pts,nComp);
	

	int curClust=1;

	int n = pts.size();
	cout << "TOTAL EVTS:" << n << endl;

	for(int i=0;i<n;++i){
		if(i%(n/10)==0)
			cout << "DBSCAN PROGRESS:"  << setprecision(4) << float(i)*100/n << "%" << endl;
		if(!pts[i].visited){
			pts[i].visited=1;
			set<int> nbhd = regionQuery(i,eps,pts);
			if(pts[i].nbhds>minPTS){
			//cout << "	NEIGHBORHOODS:" << nb << endl;
				expandCluster(i,curClust,nbhd,eps,minPTS,pts);
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

void setup(vector<float> &tmp,TFile **f,TTree **tr,TFile **fout,TTree **tr_out,int &nComp,float &eps,int &minPTS){
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
	for(int i=0;i<nComp;++i){
		string varStr;
		ifile >> varStr;
		(*tr)->SetBranchAddress(varStr.c_str(),&(*(tmp.begin()+i)));
		cout << "SETTING INPUT VARIABLE" << i+1 << ": " << varStr << endl; 
	}

	cout << "SETUP COMPLETE" << endl;

}

void ptsInit(vector<float> &tmp,TTree *tr,vector<Point> &pts,int nComp){

	int n = tr->GetEntries();

	vector<float> sum(nComp,0);
	vector<float> sumsq(nComp,0);

	for(int i=0;i<n;++i){
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
	cout << "POINT INITIALIZATION COMPLETE" << endl;

	ptsNorm(sum,sumsq,pts,nComp);	
}

void ptsNorm(vector<float> &sum, vector<float> &sumsq, vector<Point> &pts,int nComp){
	vector<float> sigma(nComp,0);

	int n = pts.size();

	for(int i=0;i<nComp;++i)
		sigma[i]=sqrt((sumsq[i]-(sum[i]*sum[i])/n)/(n-1));	
	
	for(int i=0;i<n;++i)
		for(int j=0;j<nComp;++j)
			pts[i].x[j]/=sigma[j];
	cout << "POINT NORMALIZATION COMPLETE" << endl;	
}
