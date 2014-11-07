/////////////////////////////////////
//                                 //
// VariablesPlotter.cc             //
// Author: Matthew Kenzie          //
// Will make plot                  //
//                                 //
/////////////////////////////////////
//
//
// EDIT defineHists() to add histogram options
// EDIT plottingOpts() to change style

#include "TROOT.h"
#include "TColor.h"
#include "TMath.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLorentzVector.h"
#include "TPaveStats.h"
#include "../interface/VariablesPlotter.h"

using namespace std;
using namespace TMath;

VariablesPlotter::VariablesPlotter(TString _name):
	BaseAnalyser(_name),
  normalise(true),
  normalisation(1.)
{
  TH1F::SetDefaultSumw2();
  gROOT->ProcessLine(".x /Users/matt/Scratch/lhcb/lhcbStyle.C");
}

VariablesPlotter::~VariablesPlotter(){}

void VariablesPlotter::Init(Looper *l){
	cout << Form("%-30s","VariablesPlotter::Init()") << " " << "Initialising Analyser (" << name << ")." << endl;

  defineHists();
}

void VariablesPlotter::Term(Looper *l){
  saveHistograms("PostReductionPlots.root");
  drawHistograms();
  deleteHistograms();
	cout << Form("%-30s","VariablesPlotter::Term()") << " " << "Terminating Analyser (" << name << ")." << endl;
}

bool VariablesPlotter::AnalyseEvent(Looper *l){

	// do physics here e.g.:
  fillHistograms(l);
  // MC only
	//if ( l->itype < 0 ) {
  //}
	// Data only
	//if ( l->itype > 0 ) {
	//}

	return true;
}

void VariablesPlotter::defineHists(){

  addHist("B0_MM",100,4500,6000,"L");
  addHist("B0_P",50,0,400e3,"R");
  addHist("B0_PT",50,0,40e3,"R");
  addHist("B0_DIRA_OWNPV",50,0.99999,1.,"L");
  addHist("B0_ENDVERTEX_CHI2",50,0,20,"L");
  addHist("B0_ISOLATION_BDT1_highq2",50,-1,1,"L");
  addHist("B0_TAU",50,0,0.02,"R");
  addHist("gamgams_PT",50,0,20e3,"R");
	addHist("bdtoutput",50,-1,1,"L");

  // draw options
  // add a name and a title with a string and then a vector of the itypes to be filled

  drawOpts.push_back(DrawingConfig("mc_jpsigam",    "MC (J/#psi#gamma)",   -81));
  drawOpts.push_back(DrawingConfig("mc_kstargam",   "MC (K*#gamma)",       -88));
  drawOpts.push_back(DrawingConfig("data_jpsigam",  "Data (J/#psi#gamma)", 71,81));
  drawOpts.push_back(DrawingConfig("data_kstargam", "Data (K*#gamma)",     72,82));

  // residual histogram options, use integer in drawOpts vector
  residOpts.push_back(make_pair(2,0));
  residOpts.push_back(make_pair(3,1));

  // call plottingOpts()
  plottingOpts();
}

void VariablesPlotter::plottingOpts(){

  // some color defs
  TColor* transpBlue = gROOT->GetColor(kBlue-7);
  transpBlue->SetAlpha(0.6);
  TColor *transpGreen = gROOT->GetColor(kGreen+1);
  transpGreen->SetAlpha(0.4);
  TColor *transpRed = gROOT->GetColor(kRed-7);
  transpRed->SetAlpha(0.4);

  // styles and colors below

  // 1st
  drawOpts[0].lcolor = transpBlue->GetNumber();
  drawOpts[0].fcolor = transpBlue->GetNumber();
  drawOpts[0].lwidth = 3;
  drawOpts[0].plotOpt = "HIST";
  drawOpts[0].legOpt = "LF";
  //drawOpts[0].print();

  // 2nd
  drawOpts[1].lcolor = transpRed->GetNumber();
  drawOpts[1].fcolor = transpRed->GetNumber();
  drawOpts[1].lwidth = 3;
  drawOpts[1].plotOpt = "HIST";
  drawOpts[1].legOpt = "LF";
  //drawOpts[1].print();

  // 3rd
  drawOpts[2].lcolor = kBlue+1;
  drawOpts[2].mcolor = kBlue+1;
  drawOpts[2].mstyle = kFullCircle;
  drawOpts[2].lwidth = 3;
  drawOpts[2].plotOpt = "LEP";
  drawOpts[2].legOpt = "LEP";
  //drawOpts[2].print();

  // 4th
  drawOpts[3].lcolor = kRed+1;
  drawOpts[3].mcolor = kRed+1;
  drawOpts[3].mstyle = kFullSquare;
  drawOpts[3].lwidth = 3;
  drawOpts[3].plotOpt = "LEP";
  drawOpts[3].legOpt = "LEP";
  //drawOpts[3].print();

}

void VariablesPlotter::addHist(TString name, int nbins, float xlow, float xhigh, TString legPos){

  HistContainer hist;
  hist.name = name;
  hist.nbins = nbins;
  hist.xlow = xlow;
  hist.xhigh = xhigh;
  hist.legPos = legPos;
  histList.push_back(hist);
}

void VariablesPlotter::fillHistograms(Looper *l){

  // if this type is not already in the map then make the histograms
  if (histMap.find(l->itype)==histMap.end()){
    for (vector<HistContainer>::iterator histIt=histList.begin(); histIt!=histList.end(); histIt++){
      histMap[l->itype][histIt->name] = new TH1F(Form("%s_t%d",histIt->name.Data(),l->itype),histIt->name.Data(),histIt->nbins,histIt->xlow,histIt->xhigh);
      histMap[l->itype][histIt->name]->SetDirectory(0);
    }
  }

  histMap[l->itype]["B0_MM"]->Fill(*l->B0_MM);
  histMap[l->itype]["B0_P"]->Fill(*l->B0_P);
  histMap[l->itype]["B0_PT"]->Fill(*l->B0_PT);
  histMap[l->itype]["B0_DIRA_OWNPV"]->Fill(*l->B0_DIRA_OWNPV);
  histMap[l->itype]["B0_ENDVERTEX_CHI2"]->Fill(*l->B0_ENDVERTEX_CHI2);
  histMap[l->itype]["B0_ISOLATION_BDT1_highq2"]->Fill(*l->B0_ISOLATION_BDT1_highq2);
  histMap[l->itype]["B0_TAU"]->Fill(*l->B0_TAU);
  histMap[l->itype]["gamgams_PT"]->Fill(*l->gamgams_PT);
	histMap[l->itype]["bdtoutput"]->Fill(*l->bdtoutput);
}

void VariablesPlotter::saveHistograms(TString outFileName){

  TFile *outFile = new TFile(outFileName,"RECREATE");
  outFile->cd();
  for (map<int,map<TString,TH1F*> >::iterator it1=histMap.begin(); it1!=histMap.end(); it1++){
    for (map<TString,TH1F*>::iterator it2=it1->second.begin(); it2!=it1->second.end(); it2++){
      it2->second->Write();
    }
  }
  outFile->Close();

}

void VariablesPlotter::deleteHistograms(){

  for (map<int,map<TString,TH1F*> >::iterator it1=histMap.begin(); it1!=histMap.end(); it1++){
    for (map<TString,TH1F*>::iterator it2=it1->second.begin(); it2!=it1->second.end(); it2++){
      delete it2->second;
    }
  }
}

void VariablesPlotter::drawHistograms(){

  for (vector<HistContainer>::iterator hist=histList.begin(); hist!=histList.end(); hist++){

    vector<TH1F*> histContainer;
    TLegend *legend;
    if (hist->legPos=="L") {
      legend = new TLegend(0.2,0.6,0.45,0.89);
    }
    else if (hist->legPos=="R") {
      legend = new TLegend(0.6,0.6,0.85,0.89);
    }
    else {
      cerr << "ERROR -- VariablesPlotter::drawHistograms() -- Unrecognised legPos option: " << hist->legPos << endl;
      exit(1);
    }
    legend->SetFillColor(0);
    double max = 0.;

    // loop over drawOpt types to make histograms for drawing
    for (vector<DrawingConfig>::iterator opt=drawOpts.begin(); opt!=drawOpts.end(); opt++){

      TH1F *th1f = new TH1F(Form("%s_%s",hist->name.Data(),opt->name.Data()),hist->name,hist->nbins,hist->xlow,hist->xhigh);

      for (vector<int>::iterator type=opt->itypes.begin(); type!=opt->itypes.end(); type++){
        th1f->Add(histMap[*type][hist->name]);
      }

      // normalise
      if (normalise) {
        th1f->Scale(normalisation/th1f->Integral());
      }

      th1f->GetXaxis()->SetTitle(hist->name);

      // set style options
      opt->SetStyle(th1f);

      // update max
      max = Max(max,th1f->GetMaximum());

      // add to legend
      legend->AddEntry(th1f,opt->title,opt->legOpt);

      // add to tempory container
      histContainer.push_back(th1f);
    }

    // should now make resid hists
    vector<TH1F*> residContainer;
    for (vector<pair<int,int> >::iterator opt=residOpts.begin(); opt!=residOpts.end(); opt++){

      // make residual histogram
      TH1F *numerator = histContainer[opt->first];
      TH1F *denominator = histContainer[opt->second];
      TString name = Form("resid_%s_over_%s",numerator->GetName(),denominator->GetName());
      TH1F *th1f = (TH1F*)numerator->Clone(name);
      th1f->Add(denominator,-1.);

      // set style to the same as numerator
      drawOpts[opt->first].SetStyle(th1f);

      // set max
      double residMax = Max(th1f->GetMaximum(),Abs(th1f->GetMinimum()));
      th1f->GetYaxis()->SetRangeUser(-1.4*residMax,1.4*residMax);

      // add to container
      residContainer.push_back(th1f);

    }

		gStyle->SetOptStat(0);
    histContainer[0]->GetYaxis()->SetRangeUser(0,max*1.3);
    histContainer[0]->GetYaxis()->SetTitle("Events");
    if (normalise) histContainer[0]->GetYaxis()->SetTitle("Probability");
    histContainer[0]->GetXaxis()->SetTitleOffset(0.8);
    histContainer[0]->GetYaxis()->SetTitleOffset(0.7);

    // now draw on canvas
    // figure out canvas size (default with no residuals is 800,600)
    int canvWidth = 800;
    int canvHeight = 600;
    if (residContainer.size()>0) canvHeight = 900;
    TCanvas *canv = new TCanvas("c","c",canvWidth,canvHeight);

    // nominal plot first
    double nomPadLow = 0.;
    if (residContainer.size()>0) nomPadLow = 0.4;
    TPad *pad = new TPad("p_nom","p_nom",0.,nomPadLow,1.,1.);
    pad->cd();
    histContainer[0]->Draw(drawOpts[0].plotOpt);
    for (unsigned int i=1; i<histContainer.size(); i++){
      histContainer[i]->Draw(drawOpts[i].plotOpt+"same");
    }
    legend->Draw("same");
    canv->cd();
    pad->Draw();
		canv->Update();
		canv->Modified();

    // resid plot second
    if (residContainer.size()>0) {
      TPad *resPad = new TPad("p_res","p_res",0.,0.0,1.,nomPadLow);
      resPad->SetTopMargin(0.01);
      resPad->SetBottomMargin(0.25);
      resPad->cd();
      // get relative height of resid pad to nominal pad
      double scale = pad->GetHNDC()/resPad->GetHNDC();
      double relscale = resPad->GetHNDC()/(pad->GetHNDC()+resPad->GetHNDC());
      // scale axis variables appropriately
      residContainer[0]->GetXaxis()->SetTitleSize(residContainer[0]->GetXaxis()->GetTitleSize()*scale);
      residContainer[0]->GetXaxis()->SetLabelSize(residContainer[0]->GetXaxis()->GetLabelSize()*scale);
      residContainer[0]->GetXaxis()->SetTitleOffset(residContainer[0]->GetXaxis()->GetTitleOffset()*relscale*1.5);
      residContainer[0]->GetXaxis()->SetLabelOffset(residContainer[0]->GetXaxis()->GetLabelOffset()*relscale);
      residContainer[0]->GetYaxis()->SetTitleSize(residContainer[0]->GetYaxis()->GetTitleSize()*scale);
      residContainer[0]->GetYaxis()->SetLabelSize(residContainer[0]->GetYaxis()->GetLabelSize()*scale);
      residContainer[0]->GetYaxis()->SetTitleOffset(residContainer[0]->GetYaxis()->GetTitleOffset()*relscale);
      residContainer[0]->GetYaxis()->SetLabelOffset(residContainer[0]->GetYaxis()->GetLabelOffset()*relscale);
      residContainer[0]->GetYaxis()->SetTitle("Residual");
      // draw them
      residContainer[0]->Draw(drawOpts[residOpts[0].first].plotOpt);
      // draw a line
      TLine line;
      line.SetLineWidth(3);
      line.SetLineColor(kBlack);
      line.SetLineStyle(kDashed);
      line.DrawLine(residContainer[0]->GetXaxis()->GetXmin(),0.,residContainer[0]->GetXaxis()->GetXmax(),0.);
      // redraw
      for (unsigned int i=0; i<residContainer.size(); i++){
        residContainer[i]->Draw(drawOpts[residOpts[i].first].plotOpt+"same");
      }

      // update the canvas
      canv->cd();
      resPad->Draw();
      pad->Draw();
      canv->Update();
      canv->Modified();
    }
    canv->Print(Form("plots/%s.pdf",hist->name.Data()));
    canv->Print(Form("plots/%s.png",hist->name.Data()));
    delete canv;
    delete legend;

    for (unsigned int i=0; i<histContainer.size(); i++){
      delete histContainer[i];
    }
    histContainer.clear();
    for (unsigned int i=0; i<residContainer.size(); i++){
      delete residContainer[i];
    }
    residContainer.clear();
  }
}


