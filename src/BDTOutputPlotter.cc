#include "../interface/BDTOutputPlotter.h"

using namespace std;

BDTOutputPlotter::BDTOutputPlotter(TString _name):
  PlotterBase(_name)
{
  normalise = true;
  normalisation = 1.;
  outfilename = "BDTOutputPlotterOut.root";
}

BDTOutputPlotter::~BDTOutputPlotter(){}

void BDTOutputPlotter::defineHistograms(){

  // add these histograms
  addHist("B0_MM",     "m(#mu^{+}#mu^{-}#gamma) (MeV)"    , 100,4500,6000,"L");
  addHist("bdtoutput", "BDT Score"                        , 50 , -1, 1, "L");
}

void BDTOutputPlotter::defineDrawingConfig(){

  // add these drawing options
  addDrawOpt("mc_jpsigam",   "J/#psi#gamma Signal", -81);
  setDrawOptDefaultFill(kBlue-7);

  addDrawOpt("data_jpsigam", "J/#psi#gamma Data", 71, 81);
  setDrawOptDefaultLine(kRed);

  addDrawOpt("mc_kstgam",   "K*#gamma Signal", -88);
  setDrawOptDefaultPoint(kBlue+1);

  addDrawOpt("data_kstgam", "K*#gamma Data", 72, 82);
  setDrawOptDefaultPoint(kRed+1);

  // add this as a residual plot option
  //addResidOpt(make_pair(0,1));
}

bool BDTOutputPlotter::fillHistograms(Looper *l){

  // fill hists now
  fillHist("B0_MM",*l->B0_MM,l);
  fillHist("bdtoutput",*l->bdtoutput,l);
  return true;
}
