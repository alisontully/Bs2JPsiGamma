/////////////////////////////////////
//                                 //
// BDTTrainerSplitLLDDFDChi2.h           //
// Author: Matthew Kenzie          //
// Will train BDTs   					     //
//                                 //
/////////////////////////////////////

#ifndef BDTTrainerSplitLLDDFDChi2_h
#define BDTTrainerSplitLLDDFDChi2_h

#include "TString.h"
#include "TMVA/Factory.h"

#include "../interface/BaseAnalyser.h"
#include "../interface/Looper.h"

class BDTTrainerSplitLLDDFDChi2 : public BaseAnalyser {

	public:

		BDTTrainerSplitLLDDFDChi2(TString _name);
		~BDTTrainerSplitLLDDFDChi2();

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

