/////////////////////////////////////
//                                 //
// BDTTrainerSplitLLDD.h            			   //
// Author: Matthew Kenzie          //
// Will train BDTs   					     //
//                                 //
/////////////////////////////////////

#include "TMath.h"
#include "TLorentzVector.h"

#include "../interface/BDTTrainerSplitLLDD.h"

using namespace std;
using namespace TMVA;

BDTTrainerSplitLLDD::BDTTrainerSplitLLDD(TString _name):
	BaseAnalyser(_name),
	outfilename("MVATrainingOutput.root"),
	evCount(0),
	numberOfBDTs(5)
{}

BDTTrainerSplitLLDD::~BDTTrainerSplitLLDD(){}

void BDTTrainerSplitLLDD::Init(Looper *l){
	cout << Form("%-30s","BDTTrainerSplitLLDD::Init()") << " " << "Initialising Analyser (" << name << ")." << endl;

  //outFiles["LL"] = TFile::Open(outfilename.ReplaceAll(".root","LL.root"),"RECREATE");
  //outFiles["DD"] = TFile::Open(outfilename.ReplaceAll(".root","DD.root"),"RECREATE");
  outFile = TFile::Open(outfilename,"RECREATE");

	// MVA factory options
	TString factoryOptions = "";
	factoryOptions += "!V:";
	factoryOptions += "!Silent:";
	factoryOptions += "Color:";
	factoryOptions += "DrawProgressBar:";
	factoryOptions += "Transformations=G,D:";
	factoryOptions += "AnalysisType=Classification";

	// make factories
  factoryContainer["LL"] = vector<TMVA::Factory*>();
  factoryContainer["DD"] = vector<TMVA::Factory*>();
	for (int b=0; b<numberOfBDTs; b++){
		factoryContainer["LL"].push_back(new Factory(Form("%sFactoryLL%d",name.Data(),b),outFile,factoryOptions));
		factoryContainer["DD"].push_back(new Factory(Form("%sFactoryDD%d",name.Data(),b),outFile,factoryOptions));
	}

	// add variables
	varNames.push_back("B0_P");
	varNames.push_back("B0_PT");
	varNames.push_back("B0_ARCCOS_DIRA_OWNPV");
	varNames.push_back("B0_ENDVERTEX_CHI2");
	varNames.push_back("B0_ISOLATION_BDT1_highq2");
	varNames.push_back("B0_TAU");
	varNames.push_back("gamgams_PT");
  //varNames.push_back("muplus_CosTheta");
  varNames.push_back("muminus_CosTheta");

	// add variables to factories and inialise map
	for (vector<TString>::iterator var=varNames.begin(); var!=varNames.end(); var++){
		for (int b=0; b<numberOfBDTs; b++){
			factoryContainer["LL"][b]->AddVariable(*var);
			factoryContainer["DD"][b]->AddVariable(*var);
		}
		varMap.insert(make_pair(*var,-9999999.));
	}

}

void BDTTrainerSplitLLDD::Term(Looper *l){

	for (int b=0; b<numberOfBDTs; b++) {

		factoryContainer["LL"][b]->PrepareTrainingAndTestTree("","!V");
		factoryContainer["DD"][b]->PrepareTrainingAndTestTree("","!V");

		// TMVA methods here
		factoryContainer["LL"][b]->BookMethod( Types::kBDT, Form("LL_BDT_%d",b) , "!H:!V:NTrees=100:BoostType=AdaBoost:nCuts=-1:MaxDepth=3:NegWeightTreatment=IgnoreNegWeightsInTraining" );
		factoryContainer["DD"][b]->BookMethod( Types::kBDT, Form("DD_BDT_%d",b) , "!H:!V:NTrees=100:BoostType=AdaBoost:nCuts=-1:MaxDepth=3:NegWeightTreatment=IgnoreNegWeightsInTraining" );

		// Train, Test and Evaluate
		factoryContainer["LL"][b]->TrainAllMethods();
		factoryContainer["LL"][b]->TestAllMethods();
		factoryContainer["LL"][b]->EvaluateAllMethods();
		factoryContainer["DD"][b]->TrainAllMethods();
		factoryContainer["DD"][b]->TestAllMethods();
		factoryContainer["DD"][b]->EvaluateAllMethods();
	}

  outFile->Close();
	//outFiles["LL"]->Close();
	//outFiles["DD"]->Close();

  cout << Form("%-30s","BDTTrainerSplitLLDD::Term()") << " " << "Training output written to: " << outFile->GetName() << endl;
	//cout << Form("%-30s","BDTTrainerSplitLLDD::Term()") << " " << "Training output written to: " << outFiles["LL"]->GetName() << endl;
	//cout << Form("%-30s","BDTTrainerSplitLLDD::Term()") << " " << "Training output written to: " << outFiles["DD"]->GetName() << endl;

	cout << Form("%-30s","BDTTrainerSplitLLDD::Term()") << " " << "Terminating Analyser (" << name << ")." << endl;
}

bool BDTTrainerSplitLLDD::AnalyseEvent(Looper *l){

	varMap["B0_P"] 												 = *l->B0_P;
	varMap["B0_PT"]                        = *l->B0_PT;
	varMap["B0_ARCCOS_DIRA_OWNPV"]         = TMath::ACos(*l->B0_DIRA_OWNPV);
	varMap["B0_ENDVERTEX_CHI2"]            = *l->B0_ENDVERTEX_CHI2;
	varMap["B0_ISOLATION_BDT1_highq2"]     = *l->B0_ISOLATION_BDT1_highq2;
	varMap["B0_TAU"]                       = *l->B0_TAU;
	varMap["gamgams_PT"]                   = *l->gamgams_PT;
  //varMap["muplus_CosTheta"]              = *l->muplus_CosTheta;
  varMap["muminus_CosTheta"]             = *l->muminus_CosTheta;

	// now put the variable values in a nice vector (in the right order!!)
	assert(varNames.size()==varMap.size());
	vector<double> values;
	for (vector<TString>::iterator it=varNames.begin(); it!=varNames.end(); it++){
		values.push_back(varMap[*it]);
	}

	int lastDigit = *l->eventNumber%10;
	int relBDT = int(floor(lastDigit/2))%numberOfBDTs;

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


	// MC only
	if ( l->itype < 0 ) {

		// now put event in relevant BDT
		for (int b=0; b<numberOfBDTs; b++){
			if (b==relBDT){
				factoryContainer[trackType][b]->AddSignalTestEvent(values);
			}
			else {
				factoryContainer[trackType][b]->AddSignalTrainingEvent(values);
			}
		}

		// step up counter
		evCount++;
	}

	// Data only
	if ( l->itype > 0 ) {

		// cut out signal region
		if (*l->B0_MM < 5500 || *l->B0_MM > 6500) return false;

		// now put event in relevant BDT
		for (int b=0; b<numberOfBDTs; b++){
			if (b==relBDT){
				factoryContainer[trackType][b]->AddBackgroundTestEvent(values);
			}
			else {
				factoryContainer[trackType][b]->AddBackgroundTrainingEvent(values);
			}
		}

		// step up counter
		evCount++;
	}

	// for reading BDT
	//float bdtvalue = bdtReader[evCount%numberOfBDTs]->EvaluateMVA();

	return true;
}
