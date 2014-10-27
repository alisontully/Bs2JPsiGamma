/////////////////////////////////////
//                                 //
// BDTTrainer.h            			   //
// Author: Matthew Kenzie          //
// Will train BDTs   					     //
//                                 //
/////////////////////////////////////

#ifndef BDTTrainer_h
#define BDTTrainer_h

#include "TString.h"
#include "TMVA/Factory.h"

#include "../interface/BaseAnalyser.h"
#include "../interface/Looper.h"

class BDTTrainer : public BaseAnalyser {

	public:

		BDTTrainer(TString _name);
		~BDTTrainer();

		virtual void Init(Looper *l);
		virtual void Term(Looper *l);
		virtual bool AnalyseEvent(Looper *l);

	private:

		TString outfilename;
		int evCount;
		int numberOfBDTs;
		std::vector<TMVA::Factory*> factoryContainer;
		TFile *outFile;

		std::vector<TString> varNames;
		std::map<TString,double> varMap;

};

#endif

