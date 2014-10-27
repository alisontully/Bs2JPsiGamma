/////////////////////////////////////
//                                 //
// PreSelection.h               //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will run the analysis chain     //
//                                 //
/////////////////////////////////////

#ifndef PreSelection_h
#define PreSelection_h

#include "TString.h"
#include "../interface/BaseAnalyser.h"
#include "../interface/Looper.h"

class PreSelection : public BaseAnalyser {

	public:

		PreSelection(TString _name);
		~PreSelection();

		virtual void Init(Looper *l);
		virtual void Term(Looper *l);
		virtual bool AnalyseEvent(Looper *l);

};

#endif
