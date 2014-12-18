/////////////////////////////////////
//                                 //
// VariablesPlotter.h               //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will run the analysis chain     //
//                                 //
/////////////////////////////////////

#ifndef VariablesPlotter_h
#define VariablesPlotter_h

#include <iostream>
#include <vector>
#include <map>

#include "TString.h"
#include "TH1F.h"

#include "../interface/BaseAnalyser.h"
#include "../interface/Looper.h"
#include "../interface/PlotterUtils.h"

// utils
class VariablesPlotter : public BaseAnalyser {

	public:

		VariablesPlotter(TString _name);
		~VariablesPlotter();

		virtual void Init(Looper *l);
		virtual void Term(Looper *l);
		virtual bool AnalyseEvent(Looper *l);

  private:

    void addHist(TString name, int nbins, float xlow, float xhigh, TString legPos="L");
    void plottingOpts();
    void defineHists();
    void fillHistograms(Looper *l);
    void saveHistograms(TString outFileName);
    void deleteHistograms();
    void drawHistograms();
    std::vector<HistContainer> histList;
    std::map<int,std::map<TString,TH1F*> > histMap;
    std::vector<DrawingConfig> drawOpts;
    std::vector<std::pair<int,int> > residOpts;

    std::vector<int> lcolors;
    std::vector<int> fcolors;
    std::vector<int> lstyles;
    std::vector<int> fstyles;
    std::vector<int> mcolors;
    std::vector<int> mstyles;

    bool normalise;
    double normalisation;

};

#endif
