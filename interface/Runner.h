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
#include "TTree.h"
#include "TMath.h"
#include "TStopwatch.h"

#include "../interface/Looper.h"
#include "../interface/BaseAnalyser.h"
#include "../interface/BranchDef.h"

class Runner {

	public:

		Runner(TTree *_outTree, BranchDef *_branchDef, TString _name="Runner");
		~Runner();

		void addLooperTree(TTree *tree, TString name, int itype, int sqrts);
		void addAnalyser(BaseAnalyser *analyser);
		void setEntryRange(Long64_t first, Long64_t last) { firstEntry = first; lastEntry = last; }
		void setFirstEntry(Long64_t ent) { firstEntry = ent; }
		void setLastEntry(Long64_t ent) { lastEntry = ent; }
		void printProgressBar(Long64_t jentry, bool isDone=false);
		void run();

	private:

		TString name;
		Looper* looper;
		std::vector<BaseAnalyser*> analysers;
		std::vector<std::vector<std::pair<int,int> > > nPassFail;
		Long64_t nentries;
		Long64_t firstEntry;
		Long64_t lastEntry;
		Long64_t naccepted;
		TStopwatch timer;

};

#endif
