/////////////////////////////////////
//                                 //
// Trigger.cc              	 //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will execute analysis pre-sel   //
//                                 //
/////////////////////////////////////

#include "TMath.h"
#include "../interface/Trigger.h"

using namespace std;

Trigger::Trigger(TString _name):
	BaseAnalyser(_name)
{}

Trigger::~Trigger(){}

void Trigger::Init(Looper *l){
	cout << Form("%-30s","Trigger::Init()") << " " << "Initialising Analyser (" << name << ")." << endl;
}

void Trigger::Term(Looper *l){
	cout << Form("%-30s","Trigger::Term()") << " " << "Terminating Analyser (" << name << ")." << endl;
}

bool Trigger::AnalyseEvent(Looper *l){

	// do physics here e.g.:

	// only KstarGamma
	if ( l->itype == 72 || l->itype == 82 || l->itype == -88 || l->itype == -89 || l->itype==-90 ) {

		// L0
		if ( ! *l->B0_L0HadronDecision_TOS 								  &&
         ! *l->B0_L0ElectronDecision_TOS                &&
				 ! *l->B0_L0Global_TIS                            ) return false;

		// HLT1
		if ( ! *l->B0_Hlt1TrackAllL0Decision_TOS            &&
         ! *l->B0_Hlt1TrackPhotonDecision_TOS             ) return false;

		// HLT2
		if ( ! *l->B0_Hlt2Topo2BodyBBDTDecision_TOS         &&
				 ! *l->B0_Hlt2Topo3BodyBBDTDecision_TOS         &&
				 ! *l->B0_Hlt2Topo4BodyBBDTDecision_TOS         &&
         ! *l->B0_Hlt2TopoE2BodyBBDTDecision_TOS        &&
         ! *l->B0_Hlt2TopoE3BodyBBDTDecision_TOS        &&
         ! *l->B0_Hlt2TopoE4BodyBBDTDecision_TOS        &&
         ! *l->B0_Hlt2RadiativeTopoPhotonDecision_TOS     ) return false;
	}
	// all other JpsiGamma
	else {

		// L0
		if ( ! *l->B0_L0MuonDecision_TOS 									  &&
				 ! *l->B0_L0DiMuonDecision_TOS                   ) return false;

		// HLT1
		if ( ! *l->B0_Hlt1TrackMuonDecision_TOS              ) return false;

		// HLT2
		if ( ! *l->B0_Hlt2DiMuonDetachedDecision_TOS 			  &&
				 ! *l->B0_Hlt2DiMuonDetachedHeavyDecision_TOS 	&&
				 ! *l->B0_Hlt2DiMuonDetachedJPsiDecision_TOS 	  &&
				 ! *l->B0_Hlt2SingleMuonDecision_TOS     			  &&
				 ! *l->B0_Hlt2Topo2BodyBBDTDecision_TOS  			  &&
				 ! *l->B0_Hlt2Topo3BodyBBDTDecision_TOS 				&&
				 ! *l->B0_Hlt2TopoMu2BodyBBDTDecision_TOS 			&&
				 ! *l->B0_Hlt2TopoMu3BodyBBDTDecision_TOS 			 ) return false;
	}

	// MC only
	//if ( l->itype < 0 ) {
	//}
	// Data only
	//if ( l->itype > 0 ) {
	//}


	return true;
}
