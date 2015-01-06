/////////////////////////////////////
//                                 //
// Runner.h                        //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will run the analysis chain     //
//                                 //
/////////////////////////////////////

#ifndef Runner_h
#define Runner_h

#include <iostream>
#include <vector>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TH2F.h"
#include "TStopwatch.h"

#include "../interface/Looper.h"
#include "../interface/BaseAnalyser.h"
#include "../interface/BranchDef.h"

class Runner {

	public:

		Runner(TString outFileName, TString outTreeName, BranchDef *_branchDef, TString _name="Runner");
		~Runner();

		void addLooperTree(TTree *tree, TString name, int itype, int sqrts);
		void addAnalyser(BaseAnalyser *analyser);
		void setEntryRange(Long64_t first, Long64_t last) { firstEntry = first; lastEntry = last; }
		void setFirstEntry(Long64_t ent) { firstEntry = ent; }
		void setLastEntry(Long64_t ent) { lastEntry = ent; }
    void setBatchMode(bool val=true) { batchmode = val; }
		void printProgressBar(Long64_t jentry, bool isDone=false);
		void run();
    void save();

    TFile *outFile;
    TTree *outTree;

	private:

		TString name;
		Looper* looper;
		std::vector<BaseAnalyser*> analysers;
		std::vector<std::vector<std::pair<int,int> > > nPassFail;
    TH2F *hPass;
    TH2F *hFail;
    TH2F *hEff;
    TH1I *hType;
		Long64_t nentries;
		Long64_t firstEntry;
		Long64_t lastEntry;
		Long64_t naccepted;
		Long64_t nprocessed;
    bool batchmode;
		TStopwatch timer;

    void setYLabel(int bin, TString label);
    void setXLabel(int bin, TString label);
    void setHistogramValues(int xval, int yval, int nPass, int nFail);

};

#endif
