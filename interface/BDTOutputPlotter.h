/////////////////////////////////////
//                                 //
// BDTOutputPlotter.h               //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will run the analysis chain     //
//                                 //
/////////////////////////////////////

#ifndef BDTOutputPlotter_h
#define BDTOutputPlotter_h

#include <iostream>
#include <vector>
#include <map>

#include "TString.h"
#include "TH1F.h"

#include "../interface/BaseAnalyser.h"
#include "../interface/PlotterBase.h"
#include "../interface/Looper.h"

class BDTOutputPlotter : public PlotterBase {

	public:

		BDTOutputPlotter(TString _name);
		~BDTOutputPlotter();

    void defineHistograms();
    void defineDrawingConfig();
    bool fillHistograms(Looper *l);

};

#endif
