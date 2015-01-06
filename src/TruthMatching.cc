/////////////////////////////////////
//                                 //
// TruthMatching.cc              	 //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will execute analysis pre-sel   //
//                                 //
/////////////////////////////////////

#include "TMath.h"
#include "../interface/TruthMatching.h"

using namespace std;

TruthMatching::TruthMatching(TString _name):
	BaseAnalyser(_name)
{}

TruthMatching::~TruthMatching(){}

void TruthMatching::Init(Looper *l){
	cout << Form("%-30s","TruthMatching::Init()") << " " << "Initialising Analyser (" << name << ")." << endl;
}

void TruthMatching::Term(Looper *l){
	cout << Form("%-30s","TruthMatching::Term()") << " " << "Terminating Analyser (" << name << ")." << endl;
}

bool TruthMatching::AnalyseEvent(Looper *l){

	// do physics here e.g.:
	// MC only
	if ( l->itype < 0 ) {
		if ( l->itype==-81 && *l->B0_BKGCAT > 20 ) return false;  // Bs2JpsiGamma SIGNAL
		if ( l->itype==-82 && *l->B0_BKGCAT > 55 ) return false;  // Bd2JpsiPiZero
		if ( l->itype==-83 && *l->B0_BKGCAT > 55 ) return false;  // Bs2JpsiEta_gg
	  if ( l->itype==-84 && *l->B0_BKGCAT > 55 ) return false;  // Bs2JpsiEta_3pi
	  if ( l->itype==-85 && *l->B0_BKGCAT > 55 ) return false;  // Bd2JpsiKs_pizpiz
    if ( l->itype==-86 && *l->B0_BKGCAT > 55 ) return false;  // Bd2JpsiKst
    if ( l->itype==-87 && *l->B0_BKGCAT > 55 ) return false;  // Bd2JpsiRho
	  if ( l->itype==-88 && *l->B0_BKGCAT > 0  ) return false;  // Bd2KstGamma
	}
	// Data only
	//if ( l->itype > 0 ) {
	//}


	return true;
}
