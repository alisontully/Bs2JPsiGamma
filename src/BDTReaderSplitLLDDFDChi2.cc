/////////////////////////////////////
//                                 //
// BDTReaderSplitLLDDFDChi2.h            			   //
// Author: Matthew Kenzie          //
// Will train BDTs   					     //
//                                 //
/////////////////////////////////////

#include "TMath.h"
#include "TLorentzVector.h"

#include "../interface/BDTReaderSplitLLDDFDChi2.h"

using namespace std;
using namespace TMVA;

BDTReaderSplitLLDDFDChi2::BDTReaderSplitLLDDFDChi2(TString _name):
	BaseAnalyser(_name),
	evCount(0),
	numberOfBDTs(5)
{}

BDTReaderSplitLLDDFDChi2::~BDTReaderSplitLLDDFDChi2(){}

void BDTReaderSplitLLDDFDChi2::Init(Looper *l){
	cout << Form("%-30s","BDTReaderSplitLLDDFDChi2::Init()") << " " << "Initialising Analyser (" << name << ")." << endl;

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
		readerContainer["LL"][b]->AddVariable("B0_FD_CHI2",                    &B0_FD_CHI2);
		readerContainer["LL"][b]->AddVariable("gamgams_PT",                    &gamgams_PT);

		readerContainer["DD"][b]->AddVariable("B0_P", 												 &B0_P);
		readerContainer["DD"][b]->AddVariable("B0_PT",                         &B0_PT);
		readerContainer["DD"][b]->AddVariable("B0_ARCCOS_DIRA_OWNPV",        	 &B0_ARCCOS_DIRA_OWNPV);
		readerContainer["DD"][b]->AddVariable("B0_ENDVERTEX_CHI2",             &B0_ENDVERTEX_CHI2);
		readerContainer["DD"][b]->AddVariable("B0_ISOLATION_BDT1_highq2",      &B0_ISOLATION_BDT1_highq2);
		readerContainer["DD"][b]->AddVariable("B0_FD_CHI2",                    &B0_FD_CHI2);
		readerContainer["DD"][b]->AddVariable("gamgams_PT",                    &gamgams_PT);

    // Book MVA methods
		weightsFile = Form("weights/BDTTrainerSplitLLDDFactoryRegieVarsLL%d_LL_BDT_%d.weights.xml",b,b);
		methodName = Form("BDT%dmethod",b);
		readerContainer["LL"][b]->BookMVA( methodName, weightsFile );

    weightsFile = Form("weights/BDTTrainerSplitLLDDFactoryRegieVarsDD%d_DD_BDT_%d.weights.xml",b,b);
		methodName = Form("BDT%dmethod",b);
		readerContainer["DD"][b]->BookMVA( methodName, weightsFile );
	}

}

void BDTReaderSplitLLDDFDChi2::Term(Looper *l){

	cout << Form("%-30s","BDTReaderSplitLLDDFDChi2::Term()") << " " << "Terminating Analyser (" << name << ")." << endl;
}

bool BDTReaderSplitLLDDFDChi2::AnalyseEvent(Looper *l){

	// Have to set everything as float because of TMVA
  B0_P            						= float(*l->B0_P);
  B0_PT                       = float(*l->B0_PT);
  B0_ARCCOS_DIRA_OWNPV        = float(TMath::ACos(*l->B0_DIRA_OWNPV));
  B0_ENDVERTEX_CHI2           = float(*l->B0_ENDVERTEX_CHI2);
  B0_ISOLATION_BDT1_highq2    = float(*l->B0_ISOLATION_BDT1_highq2);
  B0_FD_CHI2                  = float(*l->B0_FD_CHI2);

  Jpsi_PT          						= float(*l->Jpsi_PT);
  Jpsi_MINIPCHI2              = float(*l->Jpsi_MINIPCHI2);

	gamgams_PT                  = float(*l->gamgams_PT);
  gamgams_DIRA_OWNPV          = float(*l->gamgams_DIRA_OWNPV);
  gamgams_MINIPCHI2           = float(*l->gamgams_MINIPCHI2);
  gamgams_ENDVERTEX_CHI2      = float(*l->gamgams_ENDVERTEX_CHI2);

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

	// bdt cut
  *l->pass_bdt = false;
  if ( ( trackType == "LL" && *l->bdtoutput > 0.40 ) || ( trackType == "DD" && *l->bdtoutput > 0.45  ) ) {
    *l->pass_bdt = true;
  }

  //if ( !*l->pass_bdt ) return false;

	// step up counter
	evCount++;

	return true;
}


