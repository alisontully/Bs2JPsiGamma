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
		if ( l->itype==-81 && *l->B0_BKGCAT > 20 ) return false;  // JpsiGamma SIGNAL
		if ( l->itype==-82 && *l->B0_BKGCAT > 55 ) return false;  // JpsiPiZero
		if ( l->itype==-83 && *l->B0_BKGCAT > 50 ) return false; // BdJpsiX
	  if ( l->itype==-84 && *l->B0_BKGCAT > 50 ) return false; // BuJpsiX
	  if ( l->itype==-85 && *l->B0_BKGCAT > 50 ) return false; // BdJpsiKs_pizpiz
    if ( l->itype==-86 && *l->B0_BKGCAT > 50 ) return false; // BdJpsiKst
    if ( l->itype==-87 && *l->B0_BKGCAT > 50 ) return false; // BdJpsiRho
	  if ( l->itype==-88 && *l->B0_BKGCAT > 0  ) return false; // BdKstGamma
	}
	// Data only
	//if ( l->itype > 0 ) {
	//}


	return true;
}
