/////////////////////////////////////
//                                 //
// BDTReader.h            			   //
// Author: Matthew Kenzie          //
// Will train BDTs   					     //
//                                 //
/////////////////////////////////////

#include "TMath.h"
#include "TLorentzVector.h"

#include "../interface/BDTReader.h"

using namespace std;
using namespace TMVA;

BDTReader::BDTReader(TString _name):
	BaseAnalyser(_name),
	evCount(0),
	numberOfBDTs(5)
{}

BDTReader::~BDTReader(){}

void BDTReader::Init(Looper *l){
	cout << Form("%-30s","BDTReader::Init()") << " " << "Initialising Analyser (" << name << ")." << endl;

	// MVA
	for (int b=0; b<numberOfBDTs; b++){
		readerContainer.push_back( new TMVA::Reader( "!Color:!Silent" ) );

		// add variables
		readerContainer[b]->AddVariable("B0_P", 												 &B0_P);
		readerContainer[b]->AddVariable("B0_PT",                         &B0_PT);
		readerContainer[b]->AddVariable("B0_ARCCOS_DIRA_OWNPV",        	 &B0_ARCCOS_DIRA_OWNPV);
		readerContainer[b]->AddVariable("B0_ENDVERTEX_CHI2",             &B0_ENDVERTEX_CHI2);
		readerContainer[b]->AddVariable("B0_ISOLATION_BDT1_highq2",      &B0_ISOLATION_BDT1_highq2);
		readerContainer[b]->AddVariable("B0_TAU",                        &B0_TAU);
		//readerContainer[b]->AddVariable("B0_FD_CHI2",                    &B0_FD_CHI2);

		//readerContainer[b]->AddVariable("Jpsi_PT",                       &Jpsi_PT);
		//readerContainer[b]->AddVariable("Jpsi_MINIPCHI2",                &Jpsi_MINIPCHI2);

		readerContainer[b]->AddVariable("gamgams_PT",                    &gamgams_PT);
		//readerContainer[b]->AddVariable("gamgams_DIRA_OWNPV",            &gamgams_DIRA_OWNPV);
		//readerContainer[b]->AddVariable("gamgams_MINIPCHI2",             &gamgams_MINIPCHI2);
		//readerContainer[b]->AddVariable("gamgams_ENDVERTEX_CHI2",        &gamgams_ENDVERTEX_CHI2);

		// Book MVA methods
		weightsFile = Form("weights/BDTTrainerFactory%d_BDT%d.weights.xml",b,b);
		methodName = Form("BDT%dmethod",b);
		readerContainer[b]->BookMVA( methodName, weightsFile );
	}

}

void BDTReader::Term(Looper *l){

	cout << Form("%-30s","BDTReader::Term()") << " " << "Terminating Analyser (" << name << ")." << endl;
}

bool BDTReader::AnalyseEvent(Looper *l){

	// Have to set everything as float because of TMVA
  B0_P            						= float(*l->B0_P);
  B0_PT                       = float(*l->B0_PT);
  B0_ARCCOS_DIRA_OWNPV        = float(TMath::ACos(*l->B0_DIRA_OWNPV));
  B0_ENDVERTEX_CHI2           = float(*l->B0_ENDVERTEX_CHI2);
  B0_ISOLATION_BDT1_highq2    = float(*l->B0_ISOLATION_BDT1_highq2);
  B0_TAU                      = float(*l->B0_TAU);
  B0_FD_CHI2                  = float(*l->B0_FD_CHI2);

  Jpsi_PT          						= float(*l->Jpsi_PT);
  Jpsi_MINIPCHI2              = float(*l->Jpsi_MINIPCHI2);

	gamgams_PT                  = float(*l->gamgams_PT);
  gamgams_DIRA_OWNPV          = float(*l->gamgams_DIRA_OWNPV);
  gamgams_MINIPCHI2           = float(*l->gamgams_MINIPCHI2);
  gamgams_ENDVERTEX_CHI2      = float(*l->gamgams_ENDVERTEX_CHI2);

	int lastDigit = *l->eventNumber%10;
	int relBDT = int(floor(lastDigit/2))%numberOfBDTs;
	assert(relBDT < numberOfBDTs);

	*l->bdtoutput = readerContainer[relBDT]->EvaluateMVA( Form("BDT%dmethod",relBDT) );

  // training mass window
  //if ((l->itype>0) && (*l->B0_MM < 5500 || *l->B0_MM > 6500)) return false;

	// bdt cut?
	if ( *l->bdtoutput < 0.2 ) return false;

	// step up counter
	evCount++;

	return true;
}


