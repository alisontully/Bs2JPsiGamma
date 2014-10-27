/////////////////////////////////////
//                                 //
// BaseAnalyser.h                  //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will run the analysis chain     //
//                                 //
/////////////////////////////////////

#ifndef BaseAnalyser_h
#define BaseAnalyser_h

#include "TString.h"
#include "../interface/Looper.h"

class BaseAnalyser {

	public:

		BaseAnalyser(TString _name);
		virtual ~BaseAnalyser() = 0;

		virtual void Init(Looper *l) = 0; // no implementation here
		virtual void Term(Looper *l) = 0; // no implementation here
		virtual bool AnalyseEvent(Looper *l) = 0; // no implementation here

		TString name;

};

#endif
