/////////////////////////////////////
//                                 //
// PreSelection.cc              	 //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will execute analysis pre-sel   //
//                                 //
/////////////////////////////////////

#include "TMath.h"
#include "../interface/PreSelection.h"

using namespace std;

PreSelection::PreSelection(TString _name):
	BaseAnalyser(_name)
{}

PreSelection::~PreSelection(){}

void PreSelection::Init(Looper *l){
	cout << Form("%-30s","PreSelection::Init()") << " " << "Initialising Analyser (" << name << ")." << endl;
}

void PreSelection::Term(Looper *l){
	cout << Form("%-30s","PreSelection::Term()") << " " << "Terminating Analyser (" << name << ")." << endl;
}

bool PreSelection::AnalyseEvent(Looper *l){

	// do physics here e.g.:

	if ( *l->B0_DIRA_OWNPV < 0.99999 ) return false;
	if ( *l->B0_PT < 2000 ) return false;
	if ( *l->gamgams_PT < 1000 ) return false;
	if ( *l->B0_TAU < 0 ) return false;

	if ( *l->B0_MM < 4250 || *l->B0_MM > 7000 ) return false;

	// MC only
	//if ( l->itype < 0 ) {
	//}
	// Data only
	//if ( l->itype > 0 ) {
	//}


	return true;
}
