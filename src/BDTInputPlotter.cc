#include "../interface/BDTInputPlotter.h"

using namespace std;

BDTInputPlotter::BDTInputPlotter(TString _name):
  PlotterBase(_name)
{
  normalise = true;
  normalisation = 1.;
  outfilename = "BDTInputPlotterOut.root";
}

BDTInputPlotter::~BDTInputPlotter(){}

void BDTInputPlotter::defineHistograms(){

  // add these histograms
  addHist("B0_MM", "m(#mu^{+}#mu^{-}#gamma) (MeV)"    , 100,4500,6000,"L");
  addHist("B0_P",  "p(#mu^{+}#mu^{-}#gamma) (MeV)"    , 50,0,800e3,"R");
  addHist("B0_PT", "p_{T}(#mu^{+}#mu^{-}#gamma) (MeV)", 50,0,20e3,"R");
  addHist("B0_DIRA_OWNPV","B_{s}^{0} DIRA", 50,0.99999,1.,"L");
  addHist("B0_ENDVERTEX_CHI2","B_{s}^{0} vertex #chi^{2}", 50,0,20,"L");
  addHist("B0_ISOLATION_BDT1_highq2","B_{s}^{0} isolation", 50,-1,1,"L");
  addHist("B0_TAU","#tau(B_{s}^{0}) (ps)",50,0,0.015,"R");
  addHist("gamgams_PT","p_{T}(#gamma) (MeV)", 50,0,10e3,"R");

}

void BDTInputPlotter::defineDrawingConfig(){

  // add these drawing options
  addDrawOpt("mc_jpsigam",   "Signal", -81);
  setDrawOptDefaultFill(kBlue-7);

  addDrawOpt("data_jpsigam", "Background", 71, 81);
  setDrawOptDefaultLine(kRed);

  // add this as a residual plot option
  //addResidOpt(make_pair(0,1));
}

bool BDTInputPlotter::fillHistograms(Looper *l){

  // any cuts here
  if (l->itype > 0 ){
    if (*l->B0_MM < 5500 || *l->B0_MM > 6500) return false;
  }

  // fill hists now
  fillHist("B0_MM",*l->B0_MM,l);
  fillHist("B0_P",*l->B0_P,l);
  fillHist("B0_PT",*l->B0_PT,l);
  fillHist("B0_DIRA_OWNPV",*l->B0_DIRA_OWNPV,l);
  fillHist("B0_ENDVERTEX_CHI2",*l->B0_ENDVERTEX_CHI2,l);
  fillHist("B0_ISOLATION_BDT1_highq2",*l->B0_ISOLATION_BDT1_highq2,l);
  fillHist("B0_TAU",*l->B0_TAU,l);
  fillHist("gamgams_PT",*l->gamgams_PT,l);

  return true;
}
