#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TThread.h"
#include<vector>
#include<deque>
#include<iostream>
#include<iomanip>
#include<set>
using namespace std;


const int nComp=4;

double mean[nComp],var[nComp];

//~0.2% N

const TString inputFileName = "../../data/reducedibd.root";
const TString outputFileName = "./output/prescan/prescan.root";

struct Point{
	float x[nComp];
	float mindist;
};

vector<Point> pts;

float measure(Point &p1, Point &p2);
void distScan(int p_idx);

int main(){
	TFile *f = new TFile(inputFileName,"READ");
	TTree *tr = (TTree*)f->Get("ibdred");

	TFile *fout = new TFile(outputFileName,"RECREATE");
	TTree *tr_out = new TTree("out","out");

	int nbhds;
	float mindist;

	TBranch *nbhd_br = tr_out->Branch("nbhd",&nbhds);
	TBranch *mindist_br = tr_out->Branch("mindist",&mindist);

	float tmp[nComp];
	tr->SetBranchAddress("dist",tmp);
	tr->SetBranchAddress("ep",tmp+1);
	tr->SetBranchAddress("ed",tmp+2);
	tr->SetBranchAddress("dt",tmp+3);

	int n = tr->GetEntries();
	cout << "TOTAL EVTS:" << n << endl;

	double sum[nComp]={};
	double sumsq[nComp]={};

	for(int i=0;i<n;++i){
		if(i%200000==0)
			cout << setprecision(3) << double(i)*100/n << "%" << endl;
		tr->GetEntry(i);
		Point p_tmp;
		for(int j=0;j<nComp;++j){
			p_tmp.x[j]=tmp[j];
			sum[j]+=tmp[j];
			sumsq[j]+=tmp[j]*tmp[j];
		}
		p_tmp.mindist=9999;	
		pts.push_back(p_tmp);
	}
	f->Close();
	cout << "SUMMATION/POINT INITIALIZATION COMPLETE" << endl;
	

	for(int i=0;i<nComp;++i){
		mean[i]=sum[i]/n;
		var[i]=(sumsq[i]-(sum[i]*sum[i])/n)/(n-1);
	}
	



	for(int i=0;i<n;++i){
		if(i%2000==0)
			cout << "PRESCAN DIST PROGRESS:"  << setprecision(4) << float(i)*100/n << "%" << endl;
		distScan(i);
	}

		

	for(int i=0;i<n;++i){
		mindist=pts[i].mindist;
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

void distScan(int p_idx){
	int n = pts.size();

	for(int i=0;i<n;++i){
		if(i==p_idx)
			continue;
		float dist = measure(pts[p_idx],pts[i]);
		pts[p_idx].mindist = min(dist,pts[p_idx].mindist);
	}
}



	
