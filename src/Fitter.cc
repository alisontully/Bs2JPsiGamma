#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TLine.h"

#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooCBShape.h"
#include "RooAddPdf.h"

#include "RooStats/SPlot.h"

#include "../interface/Fitter.h"

using namespace std;
using namespace RooFit;

Fitter::Fitter(TString wsname):
	bdtcut(0.2)
{
  gROOT->ProcessLine(".x /Users/matt/Scratch/lhcb/lhcbStyle.C");
	w = new RooWorkspace(wsname);
	RooArgSet *observables = new RooArgSet();
	w->defineSet("observables",*observables);
	delete observables;
  colors.push_back(kRed);
  colors.push_back(kBlue);
  colors.push_back(kGreen+1);
}

Fitter::~Fitter(){
	//delete w;
	//tf->Close();
	//delete tf;
}

TCanvas* Fitter::createCanvas(){

  int canv_w = 800;
  int canv_h = 600;
  int top_x = canvs.size()*20;
  int top_y = canvs.size()*20;
  TString canvName = Form("c%d",int(canvs.size()));

  TCanvas *c = new TCanvas(canvName,canvName,top_x,top_y,canv_w,canv_h);
  canvs.push_back(c);
  return c;
}

void Fitter::addObsVar(TString name, double min, double max){
  addObsVar(name,name,"",min,max);
}

void Fitter::addObsVar(TString name, TString title, TString unit, double min, double max){
	w->factory(Form("%s[%f,%f]",name.Data(),min,max));
	((RooArgSet*)w->set("observables"))->add(*w->var(name));
  w->var(name)->SetTitle(title);
  w->var(name)->setUnit(unit);
}

void Fitter::makeObsVars(){
  addObsVar("B0_MM",         "m(K^{#pm}#pi^{#mp}#gamma)",  "MeV",    4500,   6000);
  addObsVar("gamgams_PT",    "p_{T}(#gamma)",  "MeV",    0,      50e3);
  addObsVar("B0_DIRA_OWNPV", "DIRA(B_{d})",    "",       0.99999,1.);
  addObsVar("Jpsi_MM",       "m(K#pi)",        "MeV",    800,    1000);
}

void Fitter::setUnit(TString var, TString unit){
  if (!w->var(var)) cerr << "WARNING -- Fitter::setUnit() -- no variable named " << var << " in workspace" << endl;
  w->var(var)->setUnit(unit);
}

void Fitter::setBins(TString var, int bins){
  if (!w->var(var)) cerr << "WARNING -- Fitter::setBins() -- no variable named " << var << " in workspace" << endl;
  w->var(var)->setBins(bins);
}

void Fitter::makeDatasets(){

  dataSets.push_back(DataSet("Data",           "Data",                                w,  71,81));
  dataSets.push_back(DataSet("DataKstG",       "Data (K*#gamma)",                     w,  72,82));
  dataSets.push_back(DataSet("MCJpsiG",        "MC (J/#psi #gamma)",                  w, -81));
  dataSets.push_back(DataSet("MCJpsiPi",       "MC (J/#psi #pi^{0})",                 w, -82));
  dataSets.push_back(DataSet("MCBdJpsiX",      "MC (B_{d} #rightarrow J/#psi X)",     w, -83));
  dataSets.push_back(DataSet("MCBuJpsiX",      "MC (B_{u} #rightarrow J/#psi X)",     w, -84));
  dataSets.push_back(DataSet("MCBdJpsiKs",     "MC (B_{d} #rightarrow J/#psi K_{S})", w, -85));
  dataSets.push_back(DataSet("MCBdJpsiKstar",  "MC (B_{d} #rightarrow J/#psi K*)",    w, -86));
  dataSets.push_back(DataSet("MCKstG",         "MC (B_{d} #rightarrow K*#gamma)",     w, -88));
  dataSets.push_back(DataSet("MCBuKstPiG",     "MC (B_{u} #rightarrow K*#pi#gamma)",  w, -90));

  // add them to workspace here
  for (vector<DataSet>::iterator dataIt=dataSets.begin(); dataIt!=dataSets.end(); dataIt++){
    RooDataSet *data = new RooDataSet(dataIt->name,dataIt->title,*w->set("observables"));
    w->import(*data);
    delete data;
  }
}

void Fitter::setBDTCut(double val){
	bdtcut = val;
}

void Fitter::fillDatasets(TString fname, TString tname){

	TFile *tf = TFile::Open(fname);
	TTree *tree = (TTree*)tf->Get(tname);

  RooArgSet *observables = (RooArgSet*)w->set("observables");

  // default variables to read
	float bdtoutput;
	int itype;
	tree->SetBranchAddress("bdtoutput",&bdtoutput);
	tree->SetBranchAddress("itype",&itype);

  // container for observable values
  map<TString,double> values;

  // read from tree into container
  RooRealVar *parg;
	TIterator *iter = observables->createIterator();
	while ((parg = (RooRealVar*)iter->Next())){
		values[parg->GetName()] = -999.;
		tree->SetBranchAddress(parg->GetName(),&values[parg->GetName()]);
	}
  delete iter;

  // now loop entries
	for (int e=0; e<tree->GetEntries(); e++){
		tree->GetEntry(e);

    // bdt cut
		if ( bdtoutput < bdtcut ) continue;

    bool passes = true;

    // observable cuts
		iter = observables->createIterator();
		while ((parg = (RooRealVar*)iter->Next())) {
			if (values[parg->GetName()] < w->var(parg->GetName())->getMin() || values[parg->GetName()] > w->var(parg->GetName())->getMax()) {
				passes=false;
			}
			w->var(parg->GetName())->setVal(values[parg->GetName()]);
		}

		if (!passes) continue;

    // dataset filling
    for (vector<DataSet>::iterator dataIt=dataSets.begin(); dataIt!=dataSets.end(); dataIt++){
      // check itype
      for (vector<int>::iterator typIt=dataIt->itypes.begin(); typIt!=dataIt->itypes.end(); typIt++){
        if (itype==*typIt){
          w->data(dataIt->name)->add(*(w->set("observables")));
        }
      }
    }
  }
}

void Fitter::constructSignalPdf(){

	w->factory("mean[5200,5300]");
	w->factory("CBShape::sig_cb1(B0_MM,mean,sigma_1[10,0,200],alpha_1[0.5,0.,1.],n_1[0.01,0.,10.])");
	w->factory("CBShape::sig_cb2(B0_MM,mean,sigma_2[10,0,200],alpha_2[-0.5,-1.,0.],n_2[0.01,0.,10.])");
	w->factory("SUM::signal( f[0.5,0,1] * sig_cb1, sig_cb2 )");

	RooArgSet *argset = w->pdf("signal")->getParameters(*(w->var("B0_MM")));
	w->defineSet("signal_params",*argset);
	delete argset;
}

void Fitter::constructPiZeroPdf(){

	w->factory("pi0_mean[5100,5400]");
	w->factory("CBShape::pi0_cb1(B0_MM,pi0_mean,pi0_sigma_1[100,0,1000],pi0_alpha_1[0.,10.],pi0_n_1[0.,20.])");
	w->factory("CBShape::pi0_cb2(B0_MM,pi0_mean,pi0_sigma_2[100,0,1000],pi0_alpha_2[-10.,0.],pi0_n_2[0.,10.])");
	w->factory("SUM::pizero( pi0_f[0.5,0,1] * pi0_cb1, pi0_cb2 )");

	RooArgSet *argset = w->pdf("pizero")->getParameters(*(w->var("B0_MM")));
	w->defineSet("pizero_params",*argset);
	delete argset;
}

void Fitter::constructJpsiXPdf(){

	w->factory("Exponential::jpsix( B0_MM, jpsix_p1[-0.05,0.] )");

	RooArgSet *argset = w->pdf("jpsix")->getParameters(*(w->var("B0_MM")));
	w->defineSet("jpsix_params",*argset);
	delete argset;
}

void Fitter::constructBkgPdf(){

	w->factory("Exponential::background( B0_MM, bkg_p1[-0.05,-0.1,0.] )");

	RooArgSet *argset = w->pdf("background")->getParameters(*(w->var("B0_MM")));
	w->defineSet("background_params",*argset);
	delete argset;
}

void Fitter::constructFullPdf(){

	w->factory("n_background[200,0,2000]");
	w->factory("n_pizero[100,0,500]");
	w->factory("mu_jpsix[2.,1,50]");
	w->factory("prod::n_jpsix( mu_jpsix, n_pizero )");
	w->factory("n_signal[0.01,0.,100]");
	w->factory("SUM::pdf( n_background*background, n_jpsix*jpsix, n_pizero*pizero, n_signal*signal )");

	RooArgSet *argset = w->pdf("pdf")->getParameters(*(w->var("B0_MM")));
	w->defineSet("pdf_params",*argset);
	delete argset;
}

void Fitter::defineParamSet(TString pdf){

  RooArgSet *argset = w->pdf(pdf)->getParameters(*w->set("observables"));
  w->defineSet(Form("%s_params",pdf.Data()),*argset);
  delete argset;
}

void Fitter::constructKstarGamMCPdf(){
  w->factory("kstg_mean[5200,5300]");
  w->factory("CBShape:kstg_cb1(B0_MM,kstg_mean,kstg_sigma_1[10,200],kstg_alpha_1[0.,1.],kstg_n_1[0.,50.])");
  w->factory("prod::kstg_sigma_2(kstg_f_sigma[0.1,0.,1],kstg_sigma_1)");
  w->factory("CBShape:kstg_cb2(B0_MM,kstg_mean,kstg_sigma_1,kstg_alpha_2[-1.,0.],kstg_n_2[0.,50.])");
  w->factory("SUM::kstg_sig( kstg_f[0.5,0,1]*kstg_cb1, kstg_cb2)");

  defineParamSet("kstg_sig");

  w->Print();
}

void Fitter::constructKstarDataPdf(){

  w->factory("Exponential:kstg_bkg( B0_MM, bktg_exp_p1[-0.02,0.] )");
  w->factory("SUM:kstg_pdf( kstg_sig_y[0,2000]*kstg_sig, kstg_bkg_y[0,600]*kstg_bkg )");

  defineParamSet("kstg_bkg");
  defineParamSet("kstg_pdf");

  w->Print();
}

void Fitter::constructPdfs(){

	constructSignalPdf();
	constructPiZeroPdf();
	constructJpsiXPdf();
	constructBkgPdf();
	constructFullPdf();

}

void Fitter::doKstarSplot(){

  // construct pdfs
  constructKstarGamMCPdf();
  constructKstarDataPdf();

  // fit mc
  fit("kstg_sig","MCKstG");
  plot("B0_MM","MCKstG","kstg_sig");

  // freeze mc params but loosen off resolutions
  freeze("kstg_sig");
  w->var("kstg_sigma_1")->setConstant(false);
  //w->var("kstg_sigma_2")->setConstant(false);

  // fit the data
  fit("kstg_pdf","DataKstG");
  plot("B0_MM","DataKstG","kstg_pdf");

  // now create s-weights
  RooAbsPdf *pdf = w->pdf("kstg_pdf");
  RooDataSet *data = (RooDataSet*)w->data("DataKstG");
  pdf->fitTo(*data,Extended());
  freeze("kstg_sig");
  freeze("kstg_bkg");
  RooStats::SPlot *sData = new RooStats::SPlot("sData","SPlot Data",*data,pdf,RooArgList(*w->var("kstg_sig_y"),*w->var("kstg_bkg_y")));
  w->import(*sData,"DataKstG_SWeights");

  // create weighted dataset of other variables
  RooDataSet *swData = new RooDataSet(Form("%s_SWeightedProjection",data->GetName()),data->GetTitle(),data,*data->get(),0,"kstg_sig_y_sw");
  w->import(*swData);

  // make plots
  vector<PlotComponent> plotComps;
  PlotComponent pc_data( data->GetName(), "Data (K*#gamma)" );
  pc_data.lcolor    = kBlack;
  pc_data.mcolor    = kBlack;
  pc_data.doption   = "LEP";

  PlotComponent pc_pdf( pdf->GetName(), "Total PDF" );
  pc_pdf.lcolor     = kBlue;
  pc_pdf.lwidth     = 3;
  pc_pdf.lstyle     = 1;
  pc_pdf.doption    = "L";

  PlotComponent pc_pdf_bkg( Form("%s:kstg_bkg",pdf->GetName()), "Background (Exp)" );
  pc_pdf_bkg.lcolor  = kGreen+1;
  pc_pdf_bkg.lwidth  = 3;
  pc_pdf_bkg.lstyle  = kDashed;
  pc_pdf_bkg.doption = "L";

  PlotComponent pc_pdf_sig( Form("%s:kstg_sig",pdf->GetName()), "Signal (2CB)" );
  pc_pdf_sig.lcolor   = kRed;
  pc_pdf_sig.lwidth   = 3;
  pc_pdf_sig.lstyle   = kDashed;
  pc_pdf_sig.doption  = "L";

  plotComps.push_back(pc_data);
  plotComps.push_back(pc_pdf_bkg);
  plotComps.push_back(pc_pdf_sig);
  plotComps.push_back(pc_pdf);

  plot("B0_MM", plotComps, "sweight");
  // end of main plot

  // do splots
  vector<TString> compDsets;
  compDsets.push_back("MCKstG");

  splot("gamgams_PT",   swData->GetName(),compDsets);
  splot("B0_DIRA_OWNPV",swData->GetName(),compDsets);
  splot("Jpsi_MM",      swData->GetName(),compDsets);

}

void Fitter::fit(TString pdf, TString data) {

  if (w->pdf(pdf) && w->data(data)) {
    w->pdf(pdf)->fitTo(*w->data(data));
  }
}

void Fitter::freeze(TString pdf){
	((RooArgSet*)w->set(Form("%s_params",pdf.Data())))->setAttribAll("Constant");
}

void Fitter::plot(TString var, TString data, TString pdf){

  RooPlot *plot = w->var(var)->frame(Title(data));
  plot->GetXaxis()->SetTitleOffset(0.8);
  if (w->data(data)) w->data(data)->plotOn(plot);
  if (w->pdf(pdf)) w->pdf(pdf)->plotOn(plot);
  TCanvas *canv = createCanvas();
  canv->SetBottomMargin(0.2);
  plot->Draw();
  canv->Update();
  canv->Modified();
  canv->Print(Form("plots/v%s_d%s_p%s.pdf",var.Data(),data.Data(),pdf.Data()));
  canv->Print(Form("plots/v%s_d%s_p%s.png",var.Data(),data.Data(),pdf.Data()));
}

void Fitter::plot(TString var, vector<PlotComponent> plotComps, TString fname) {

  TCanvas *canv = createCanvas();
  RooPlot *plot = w->var(var)->frame();
  TString xtitle = w->var(var)->GetTitle();
  if (TString(w->var(var)->getUnit())!=TString("")) xtitle = Form("%s (%s)",w->var(var)->GetTitle(),w->var(var)->getUnit());
  plot->GetXaxis()->SetTitle(xtitle);
  plot->GetXaxis()->SetTitleOffset(0.8);
  plot->GetYaxis()->SetTitleOffset(0.7);

  TLegend *leg = new TLegend(0.6,0.6,0.9,0.9);
  leg->SetFillColor(0);

  for (unsigned int i=0; i<plotComps.size(); i++){
    plotComps[i].plotOn(w,plot,leg);
  }

  plot->Draw();
  leg->Draw("same");
  canv->Update();
  canv->Modified();
  canv->Print(Form("plots/%s.pdf",fname.Data()));
  canv->Print(Form("plots/%s.png",fname.Data()));
}

void Fitter::splot(TString var, TString data){
  vector<TString> temp;
  splot(var, data, temp);
}

void Fitter::splot(TString var, TString data, vector<TString> compDsets) {

  if (!w->data(data)) {
    cerr << "ERROR -- Fitter::splot() -- no sweighted data exists" << endl;
    exit(1);
  }
  TCanvas *canv = createCanvas();

  TLegend *leg = new TLegend(0.6,0.6,0.9,0.9);
  leg->SetFillColor(0);
  RooPlot *splot = w->var(var)->frame();
  splot->SetTitle(Form("%s projection",w->var(var)->GetTitle()));
  splot->GetXaxis()->SetTitleOffset(0.8);
  splot->GetYaxis()->SetTitleOffset(0.7);
  TString xtitle = w->var(var)->GetTitle();
  if (TString(w->var(var)->getUnit())!=TString("")) xtitle = Form("%s (%s)",w->var(var)->GetTitle(),w->var(var)->getUnit());
  splot->GetXaxis()->SetTitle(w->var(var)->GetTitle());

  w->data(data)->plotOn(splot);
  RooHist *dh = (RooHist*)splot->getObject(splot->numItems()-1);
  leg->AddEntry(dh,w->data(data)->GetTitle(),"LEP");

  vector<RooHist*> compHists;
  for (unsigned int i=0; i<compDsets.size(); i++){
    TString dset = compDsets[i];
    w->data(dset)->plotOn(splot,MarkerColor(colors[i]),LineColor(colors[i]),Rescale(w->data(data)->sumEntries()/w->data(dset)->sumEntries()));
    RooHist *sh = (RooHist*)splot->getObject(splot->numItems()-1);
    leg->AddEntry(sh,w->data(dset)->GetTitle(),"LEP");
    compHists.push_back(sh);
  }

  splot->Draw();
  leg->Draw("same");
  canv->Update();
  canv->Modified();
  canv->Print(Form("plots/splot_v%s.pdf",var.Data()));
  canv->Print(Form("plots/splot_v%s.png",var.Data()));

  TCanvas *canvResid = createCanvas();
  for (unsigned int i=0; i<compHists.size(); i++){
    RooHist *residHist = new RooHist(*dh,*compHists[i],1.,-1.,RooAbsData::SumW2);
    residHist->SetTitle("Residual");
    residHist->GetYaxis()->SetTitle("Data-MC");
    residHist->GetXaxis()->SetTitle(xtitle);
    residHist->GetXaxis()->SetTitleOffset(0.8);
    residHist->GetYaxis()->SetTitleOffset(0.7);
    residHist->SetLineColor(colors[i]);
    residHist->SetMarkerColor(colors[i]);
    if (i==0) {
      residHist->Draw();
    }
    else {
      residHist->Draw("same");
    }
    residHist->SetDrawOption("AP");
    canvResid->Update();
    canvResid->Modified();
  }
  TLine *l = new TLine();
  l->SetLineColor(kBlack);
  l->SetLineStyle(kDashed);
  l->SetLineWidth(3);
  l->DrawLine(w->var(var)->getMin(),0.,w->var(var)->getMax(),0.);
  canvResid->Update();
  canvResid->Modified();
  canvResid->Print(Form("plots/splot_resid_v%s.pdf",var.Data()));
  canvResid->Print(Form("plots/splot_resid_v%s.png",var.Data()));

}

void Fitter::save(TString fname){
	w->writeToFile(fname);
}

