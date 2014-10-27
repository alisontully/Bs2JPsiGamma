/////////////////////////////////////
//                                 //
// BDTReader.h            			   //
// Author: Matthew Kenzie          //
// Will train BDTs   					     //
//                                 //
/////////////////////////////////////

#ifndef BDTReader_h
#define BDTReader_h

#include "TString.h"
#include "TMVA/Reader.h"

#include "../interface/BaseAnalyser.h"
#include "../interface/Looper.h"

class BDTReader : public BaseAnalyser {

	public:

		BDTReader(TString _name);
		~BDTReader();

		virtual void Init(Looper *l);
		virtual void Term(Looper *l);
		virtual bool AnalyseEvent(Looper *l); // no implementation here

	private:

		int evCount;
		int numberOfBDTs;
		std::vector<TMVA::Reader*> readerContainer;
		TString weightsFile;
		TString methodName;

		// read vars
		float B0_P;
		float B0_PT;
		float B0_ARCCOS_DIRA_OWNPV;
		float B0_ENDVERTEX_CHI2;
		float B0_ISOLATION_BDT1_highq2;
		float B0_TAU;
		float B0_FD_CHI2;

    float Jpsi_PT;
    float Jpsi_MINIPCHI2;

    float gamgams_PT;
    float gamgams_DIRA_OWNPV;
    float gamgams_MINIPCHI2;
  	float gamgams_ENDVERTEX_CHI2;
};
#endif

