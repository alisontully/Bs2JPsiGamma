/////////////////////////////////////
//                                 //
// BDTReaderSplitLLDD.h            			   //
// Author: Matthew Kenzie          //
// Will train BDTs   					     //
//                                 //
/////////////////////////////////////

#include "TMath.h"
#include "TLorentzVector.h"

#include "../interface/BDTReaderSplitLLDD.h"

using namespace std;
using namespace TMVA;

BDTReaderSplitLLDD::BDTReaderSplitLLDD(TString _name):
	BaseAnalyser(_name),
	evCount(0),
	numberOfBDTs(5)
{}

BDTReaderSplitLLDD::~BDTReaderSplitLLDD(){}

void BDTReaderSplitLLDD::Init(Looper *l){
	cout << Form("%-30s","BDTReaderSplitLLDD::Init()") << " " << "Initialising Analyser (" << name << ")." << endl;

	// MVA
  readerContainer["LL"] = vector<TMVA::Reader*>();
  readerContainer["DD"] = vector<TMVA::Reader*>();

  for (int b=0; b<numberOfBDTs; b++){
		readerContainer["LL"].push_back( new TMVA::Reader( "!Color:!Silent" ) );
		readerContainer["DD"].push_back( new TMVA::Reader( "!Color:!Silent" ) );

		// add variables
		readerContainer["LL"][b]->AddVariable("B0_P", 												 &B0_P);
		readerContainer["LL"][b]->AddVariable("B0_PT",                         &B0_PT);
		readerContainer["LL"][b]->AddVariable("B0_ARCCOS_DIRA_OWNPV",        	 &B0_ARCCOS_DIRA_OWNPV);
		readerContainer["LL"][b]->AddVariable("B0_ENDVERTEX_CHI2",             &B0_ENDVERTEX_CHI2);
		readerContainer["LL"][b]->AddVariable("B0_ISOLATION_BDT1_highq2",      &B0_ISOLATION_BDT1_highq2);
		readerContainer["LL"][b]->AddVariable("B0_TAU",                        &B0_TAU);
		readerContainer["LL"][b]->AddVariable("gamgams_PT",                    &gamgams_PT);
    readerContainer["LL"][b]->AddVariable("muminus_CosTheta",              &muminus_CosTheta);

		readerContainer["DD"][b]->AddVariable("B0_P", 												 &B0_P);
		readerContainer["DD"][b]->AddVariable("B0_PT",                         &B0_PT);
		readerContainer["DD"][b]->AddVariable("B0_ARCCOS_DIRA_OWNPV",        	 &B0_ARCCOS_DIRA_OWNPV);
		readerContainer["DD"][b]->AddVariable("B0_ENDVERTEX_CHI2",             &B0_ENDVERTEX_CHI2);
		readerContainer["DD"][b]->AddVariable("B0_ISOLATION_BDT1_highq2",      &B0_ISOLATION_BDT1_highq2);
		readerContainer["DD"][b]->AddVariable("B0_TAU",                        &B0_TAU);
		readerContainer["DD"][b]->AddVariable("gamgams_PT",                    &gamgams_PT);
    readerContainer["DD"][b]->AddVariable("muminus_CosTheta",              &muminus_CosTheta);

    // Book MVA methods
		weightsFile = Form("weights/BDTTrainerSplitLLDDFactoryLL%d_LL_BDT_%d.weights.xml",b,b);
		methodName = Form("BDT%dmethod",b);
		readerContainer["LL"][b]->BookMVA( methodName, weightsFile );

    weightsFile = Form("weights/BDTTrainerSplitLLDDFactoryDD%d_DD_BDT_%d.weights.xml",b,b);
		methodName = Form("BDT%dmethod",b);
		readerContainer["DD"][b]->BookMVA( methodName, weightsFile );
	}

}

void BDTReaderSplitLLDD::Term(Looper *l){

	cout << Form("%-30s","BDTReaderSplitLLDD::Term()") << " " << "Terminating Analyser (" << name << ")." << endl;
}

bool BDTReaderSplitLLDD::AnalyseEvent(Looper *l){

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

  muminus_CosTheta            = float(*l->muminus_CosTheta);

  // get conversion track type
  TString trackType = "";
  if ( *l->eplus_TRACK_Type==3 && *l->eplus_TRACK_Type==3 ) {
    trackType = "LL";
  }
  else if ( *l->eplus_TRACK_Type==5 && *l->eplus_TRACK_Type==5 ){
    trackType = "DD";
  }
  else {
    // not a valid event type
    return false;
  }

	int lastDigit = *l->eventNumber%10;
	int relBDT = int(floor(lastDigit/2))%numberOfBDTs;
	assert(relBDT < numberOfBDTs);

	*l->bdtoutput = readerContainer[trackType][relBDT]->EvaluateMVA( Form("BDT%dmethod",relBDT) );

  // training mass window
  //if ((l->itype>0) && (*l->B0_MM < 5500 || *l->B0_MM > 6500)) return false;

	// bdt cut?
	//if ( *l->bdtoutput < 0.2 ) return false;
  if ( trackType == "LL" && *l->bdtoutput < 0.35 ) return false;
  if ( trackType == "DD" && *l->bdtoutput < 0.45  ) return false;

	// step up counter
	evCount++;

	return true;
}


