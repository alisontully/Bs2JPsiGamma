/////////////////////////////////////
//                                 //
// Trigger.h               //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will run the analysis chain     //
//                                 //
/////////////////////////////////////

#ifndef Trigger_h
#define Trigger_h

#include "TString.h"
#include "../interface/BaseAnalyser.h"
#include "../interface/Looper.h"

class Trigger : public BaseAnalyser {

	public:

		Trigger(TString _name);
		~Trigger();

		virtual void Init(Looper *l);
		virtual void Term(Looper *l);
		virtual bool AnalyseEvent(Looper *l);

};

#endif
