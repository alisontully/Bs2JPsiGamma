/////////////////////////////////////
//                                 //
// BDTTrainer.h            			   //
// Author: Matthew Kenzie          //
// Will train BDTs   					     //
//                                 //
/////////////////////////////////////

#include "TMath.h"
#include "TLorentzVector.h"

#include "../interface/BDTTrainer.h"

using namespace std;
using namespace TMVA;

BDTTrainer::BDTTrainer(TString _name):
	BaseAnalyser(_name),
	outfilename("MVATrainingOutput.root"),
	evCount(0),
	numberOfBDTs(5)
{}

BDTTrainer::~BDTTrainer(){}

void BDTTrainer::Init(Looper *l){
	cout << Form("%-30s","BDTTrainer::Init()") << " " << "Initialising Analyser (" << name << ")." << endl;

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
	for (int b=0; b<numberOfBDTs; b++){
		factoryContainer.push_back(new Factory(Form("%sFactory%d",name.Data(),b),outFile,factoryOptions));
	}

	// add variables
	varNames.push_back("B0_P");
	varNames.push_back("B0_PT");
	varNames.push_back("B0_ARCCOS_DIRA_OWNPV");
	varNames.push_back("B0_ENDVERTEX_CHI2");
	varNames.push_back("B0_ISOLATION_BDT1_highq2");
	varNames.push_back("B0_TAU");
	//varNames.push_back("B0_FD_CHI2");

	//varNames.push_back("Jpsi_PT");
	//varNames.push_back("Jpsi_MINIPCHI2");

	varNames.push_back("gamgams_PT");
	//varNames.push_back("gamgams_DIRA_OWNPV");
	//varNames.push_back("gamgams_MINIPCHI2");
	//varNames.push_back("gamgams_ENDVERTEX_CHI2");

	// add variables to factories and inialise map
	for (vector<TString>::iterator var=varNames.begin(); var!=varNames.end(); var++){
		for (int b=0; b<numberOfBDTs; b++){
			factoryContainer[b]->AddVariable(*var);
		}
		varMap.insert(make_pair(*var,-9999999.));
	}

}

void BDTTrainer::Term(Looper *l){

	for (int b=0; b<numberOfBDTs; b++) {

		factoryContainer[b]->PrepareTrainingAndTestTree("","!V");

		// TMVA methods here
		factoryContainer[b]->BookMethod( Types::kBDT, Form("BDT%d",b) , "!H:!V:NTrees=500:BoostType=AdaBoost:nCuts=-1:MaxDepth=3:NegWeightTreatment=IgnoreNegWeightsInTraining" );

		// Train, Test and Evaluate
		factoryContainer[b]->TrainAllMethods();
		factoryContainer[b]->TestAllMethods();
		factoryContainer[b]->EvaluateAllMethods();
	}

	outFile->Close();

	cout << Form("%-30s","BDTTrainer::Term()") << " " << "Training output written to: " << outFile->GetName() << endl;

	cout << Form("%-30s","BDTTrainer::Term()") << " " << "Terminating Analyser (" << name << ")." << endl;
}

bool BDTTrainer::AnalyseEvent(Looper *l){

	varMap["B0_P"] 												 = *l->B0_P;
	varMap["B0_PT"]                        = *l->B0_PT;
	varMap["B0_ARCCOS_DIRA_OWNPV"]         = TMath::ACos(*l->B0_DIRA_OWNPV);
	varMap["B0_ENDVERTEX_CHI2"]            = *l->B0_ENDVERTEX_CHI2;
	varMap["B0_ISOLATION_BDT1_highq2"]     = *l->B0_ISOLATION_BDT1_highq2;
	varMap["B0_TAU"]                       = *l->B0_TAU;
	//varMap["B0_FD_CHI2"]                   = *l->B0_FD_CHI2;

	//varMap["Jpsi_PT"]                      = *l->Jpsi_PT;
	//varMap["Jpsi_MINIPCHI2"]               = *l->Jpsi_MINIPCHI2;

	varMap["gamgams_PT"]                   = *l->gamgams_PT;
	//varMap["gamgams_DIRA_OWNPV"]           = *l->gamgams_DIRA_OWNPV;
	//varMap["gamgams_MINIPCHI2"]            = *l->gamgams_MINIPCHI2;
	//varMap["gamgams_ENDVERTEX_CHI2"]       = *l->gamgams_ENDVERTEX_CHI2;

	// now put the variable values in a nice vector (in the right order!!)
	assert(varNames.size()==varMap.size());
	vector<double> values;
	for (vector<TString>::iterator it=varNames.begin(); it!=varNames.end(); it++){
		values.push_back(varMap[*it]);
	}

	int lastDigit = *l->eventNumber%10;
	int relBDT = int(floor(lastDigit/2))%numberOfBDTs;

	// MC only
	if ( l->itype < 0 ) {

		// now put event in relevant BDT
		for (int b=0; b<numberOfBDTs; b++){
			if (b==relBDT){
				factoryContainer[b]->AddSignalTestEvent(values);
			}
			else {
				factoryContainer[b]->AddSignalTrainingEvent(values);
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
				factoryContainer[b]->AddBackgroundTestEvent(values);
			}
			else {
				factoryContainer[b]->AddBackgroundTrainingEvent(values);
			}
		}

		// step up counter
		evCount++;
	}

	// for reading BDT
	//float bdtvalue = bdtReader[evCount%numberOfBDTs]->EvaluateMVA();

	return true;
}
