#include <iostream>
#include <vector>
#include <map>

#include "TFile.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "TStopwatch.h"
#include "RooPlot.h"

pair<double,double> calcEffSigma(RooRealVar *mass, RooAbsPdf *pdf, double wmin=5000., double wmax=5500., double step=0.2, double epsilon=1.e-4){

  RooAbsReal *cdf = pdf->createCdf(RooArgList(*mass));
  cout << "Computing effSigma...." << endl;
  TStopwatch sw;
  sw.Start();
  double point=wmin;
  vector<pair<double,double> > points;
  
  while (point <= wmax){
    mass->setVal(point);
    if (pdf->getVal() > epsilon){
      points.push_back(pair<double,double>(point,cdf->getVal())); 
    }
    point+=step;
  }
  double low = wmin;
  double high = wmax;
  double width = wmax-wmin;
  for (unsigned int i=0; i<points.size(); i++){
    for (unsigned int j=i; j<points.size(); j++){
      double wy = points[j].second - points[i].second;
      if (TMath::Abs(wy-0.683) < epsilon){
        double wx = points[j].first - points[i].first;
        if (wx < width){
          low = points[i].first;
          high = points[j].first;
          width=wx;
        }
      }
    }
  }
  sw.Stop();
  cout << "effSigma: [" << low << "-" << high << "] = " << width/2. << endl;
  cout << "\tTook: "; sw.Print();
  pair<double,double> result(low,high);
  return result;
}


void getEffSigma(TString wsfile="model_ws_mfit.root") {
  
  TFile *tf = TFile::Open(wsfile);
  RooWorkspace *w = (RooWorkspace*)tf->Get("model_ws");

  RooRealVar *mass = (RooRealVar*)w->var("B0_M");
  RooAbsPdf  *ll_pdf = (RooAbsPdf*)w->pdf("sig0_LL");
  RooAbsPdf  *dd_pdf = (RooAbsPdf*)w->pdf("sig0_DD");
  
  RooPlot *plot = mass->frame();
  ll_pdf->plotOn(plot, RooFit::LineColor(kRed) );
  dd_pdf->plotOn(plot);
  plot->Draw();

  pair<double,double> ll_range = calcEffSigma( mass, ll_pdf , 5000, 5500, 0.01);
  pair<double,double> dd_range = calcEffSigma( mass, dd_pdf , 5000, 5500, 0.01);
  
  cout << "EffSig LL: " << (ll_range.second - ll_range.first)/2. << endl;
  cout << "EffSig DD: " << (dd_range.second - dd_range.first)/2. << endl;
}

