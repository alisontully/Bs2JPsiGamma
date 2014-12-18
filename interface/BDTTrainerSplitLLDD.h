/////////////////////////////////////
//                                 //
// BDTTrainerSplitLLDD.h           //
// Author: Matthew Kenzie          //
// Will train BDTs   					     //
//                                 //
/////////////////////////////////////

#ifndef BDTTrainerSplitLLDD_h
#define BDTTrainerSplitLLDD_h

#include "TString.h"
#include "TMVA/Factory.h"

#include "../interface/BaseAnalyser.h"
#include "../interface/Looper.h"

class BDTTrainerSplitLLDD : public BaseAnalyser {

	public:

		BDTTrainerSplitLLDD(TString _name);
		~BDTTrainerSplitLLDD();

		virtual void Init(Looper *l);
		virtual void Term(Looper *l);
		virtual bool AnalyseEvent(Looper *l);

	private:

		TString outfilename;
		int evCount;
		int numberOfBDTs;
    std::map<TString, std::vector<TMVA::Factory*> > factoryContainer;
    TFile *outFile;
    //std::map<TString,TFile*> outFiles;

		std::vector<TString> varNames;
		std::map<TString,double> varMap;

};

#endif

