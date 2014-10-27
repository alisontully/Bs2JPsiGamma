/////////////////////////////////////
//                                 //
// TruthMatching.h               //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will run the analysis chain     //
//                                 //
/////////////////////////////////////

#ifndef TruthMatching_h
#define TruthMatching_h

#include "TString.h"
#include "../interface/BaseAnalyser.h"
#include "../interface/Looper.h"

class TruthMatching : public BaseAnalyser {

	public:

		TruthMatching(TString _name);
		~TruthMatching();

		virtual void Init(Looper *l);
		virtual void Term(Looper *l);
		virtual bool AnalyseEvent(Looper *l);

};

#endif
