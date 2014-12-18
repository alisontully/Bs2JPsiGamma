/////////////////////////////////////
//                                 //
// BDTInputPlotter.h               //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will run the analysis chain     //
//                                 //
/////////////////////////////////////

#ifndef BDTInputPlotter_h
#define BDTInputPlotter_h

#include <iostream>
#include <vector>
#include <map>

#include "TString.h"
#include "TH1F.h"

#include "../interface/BaseAnalyser.h"
#include "../interface/PlotterBase.h"
#include "../interface/Looper.h"

class BDTInputPlotter : public PlotterBase {

	public:

		BDTInputPlotter(TString _name);
		~BDTInputPlotter();

    void defineHistograms();
    void defineDrawingConfig();
    bool fillHistograms(Looper *l);

};

#endif
