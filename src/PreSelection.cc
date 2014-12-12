/////////////////////////////////////
//                                 //
// PreSelection.cc              	 //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will execute analysis pre-sel   //
//                                 //
/////////////////////////////////////

#include "TMath.h"
#include "TLorentzVector.h"
#include "../interface/PreSelection.h"

using namespace std;
using namespace TMath;

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

  if ( l->itype==72 || l->itype==82 || l->itype==-88 || l->itype==-89 || l->itype==-90) {
    //if (*l->piminus_PIDK > 5.) return false;
    //if (*l->Kplus_PIDK < -2. ) return false;

    TLorentzVector pi_p4(*l->piminus_PX,*l->piminus_PY,*l->piminus_PZ, Sqrt((497.*497.) + ((*l->piminus_P)*(*l->piminus_P))));
    TLorentzVector K_p4(*l->Kplus_PX,*l->Kplus_PY,*l->Kplus_PZ,*l->Kplus_PE);
    TLorentzVector inv_m = pi_p4+K_p4;
    *l->phi_mass = inv_m.M();
  }

	// MC only
	//if ( l->itype < 0 ) {
	//}
	// Data only
	//if ( l->itype > 0 ) {
	//}


	return true;
}
