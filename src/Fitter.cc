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
	bdtcut(0.2),
	hasfit(false)
{
  gROOT->ProcessLine(".x /Users/matt/Scratch/lhcb/lhcbStyle.C");
	w = new RooWorkspace(wsname);
	RooArgSet *observables = new RooArgSet();
	w->defineSet("observables",*observables);
	delete observables;
}

Fitter::~Fitter(){
	//delete w;
	//tf->Close();
	//delete tf;
}

void Fitter::addObsVar(TString name, double min, double max){
	w->factory(Form("%s[%f,%f]",name.Data(),min,max));
	((RooArgSet*)w->set("observables"))->add(*w->var(name));
}

void Fitter::makeObsVars(){
  addObsVar("B0_MM",5050,6000);
  addObsVar("gamgams_PT",0,50e3);
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
  dataSets.push_back(DataSet("MCKstG",         "MC (K*#gamma)",                       w, -88));

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

/*
void Fitter::readTree(TString fname, TString tname){

	TFile *tf = TFile::Open(fname);
	TTree *tree = (TTree*)tf->Get(tname);

	RooDataSet *data        = new RooDataSet("data","data",RooArgSet(*(w->var("B0_MM"))));
	RooDataSet *mc          = new RooDataSet("mc","mc",RooArgSet(*(w->var("B0_MM"))));
	RooDataSet *mcPi0       = new RooDataSet("mcPi0","mcPi0",RooArgSet(*(w->var("B0_MM"))));
	RooDataSet *mcJpsiX     = new RooDataSet("mcJpsiX","mcJpsiX",RooArgSet(*(w->var("B0_MM"))));
	RooDataSet *mcBdJpsiX   = new RooDataSet("mcBdJpsiX","mcBdJpsiX",RooArgSet(*(w->var("B0_MM"))));
	RooDataSet *mcBuJpsiX   = new RooDataSet("mcBuJpsiX","mcBuJpsiX",RooArgSet(*(w->var("B0_MM"))));
  RooDataSet *mcJspiKs    = new RooDataSet("mcJpsiKs","mcJpsiKs",RooArgSet(*(w->var("B0_MM"))));
  RooDataSet *mcJpsiKstar = new RooDataSet("mcJpsiKstar","mcJpsiKstar",RooArgSet(*(w->var("B0_MM"))));
  RooDataSet *dataKst     = new RooDataSet("dataKst","dataKst",RooArgSet(*(w->var("B0_MM"))));
  RooDataSet *mcKst       = new RooDataSet("mcKst","mcKst",RooArgSet(*(w->var("B0_MM"))));

	double B0_M;
	float bdtoutput;
	int itype;
	int sqrts;

	tree->SetBranchAddress("B0_MM",&B0_M);
	tree->SetBranchAddress("bdtoutput",&bdtoutput);
	tree->SetBranchAddress("itype",&itype);
	tree->SetBranchAddress("sqrts",&sqrts);

	for (int e=0; e<tree->GetEntries(); e++){
		tree->GetEntry(e);

		if ( bdtoutput < bdtcut ) continue;
		if ( B0_M < w->var("B0_MM")->getMin() || B0_M > w->var("B0_MM")->getMax() ) continue;

		w->var("B0_MM")->setVal(B0_M);

		if ( itype == -81 ) {
			mc->add(RooArgSet(*(w->var("B0_MM"))));
		}
		if ( itype == -82 ) {
			mcPi0->add(RooArgSet(*(w->var("B0_MM"))));
		}
		if ( itype == -83 ){
			mcBdJpsiX->add(RooArgSet(*(w->var("B0_MM"))));
			mcJpsiX->add(RooArgSet(*(w->var("B0_MM"))));
		}
		if ( itype == -84 ){
			mcBuJpsiX->add(RooArgSet(*(w->var("B0_MM"))));
			mcJpsiX->add(RooArgSet(*(w->var("B0_MM"))));
		}
    if ( itype == -85 ){
      mcBd
    }
		if ( itype == 71 || itype == 81 ) {
			data->add(RooArgSet(*(w->var("B0_MM"))));
		}
		if ( itype == 72 || itype == 82 ) {
      dataKst->add(RooArgSet(*(w->var("B0_MM"))));
    }
	}

	w->import(*data);
	w->import(*mc);
	w->import(*mcPi0);
	w->import(*mcBd);
	w->import(*mcBu);
	w->import(*mcJpsiX);
  w->import(*dataKst);

	delete tree;
	tf->Close();
	delete tf;
	delete data;
	delete mc;
	delete mcPi0;
	delete mcBd;
	delete mcBu;
	delete mcJpsiX;
  delete dataKst;
}
*/

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
  w->factory("CBShape:kstg_cb2(B0_MM,kstg_mean,kstg_sigma_2[10,200],kstg_alpha_2[-1.,0.],kstg_n_2[0.,50.])");
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

  constructKstarGamMCPdf();
  constructKstarDataPdf();

  fit("kstg_sig","MCKstG");
  plot("B0_MM","MCKstG","kstg_sig");
  //freeze("kstg_sig");
  fit("kstg_pdf","DataKstG");
  plot("B0_MM","DataKstG","kstg_pdf");

  RooAbsPdf *pdf = w->pdf("kstg_pdf");
  RooDataSet *data = (RooDataSet*)w->data("DataKstG");

  pdf->fitTo(*data,Extended());
  freeze("kstg_sig");
  freeze("kstg_bkg");
  RooStats::SPlot *sData = new RooStats::SPlot("sData","SPlot Data",*data,pdf,RooArgList(*w->var("kstg_sig_y"),*w->var("kstg_bkg_y")));
  w->import(*sData,"DataKstG_SWeights");

  cout << "Check sWeights:" << endl;
  cout << "sYield: " << w->var("kstg_sig_y") << " from sWeights: " << sData->GetYieldFromSWeight("kstg_sig_y") << endl;
  cout << "bYield: " << w->var("kstg_bkg_y") << " from sWeights: " << sData->GetYieldFromSWeight("kstg_bkg_y") << endl;

  for (int i=0; i<10; i++){
    cout << "   " << "bW " << sData->GetSWeight(i,"kstg_bkg_y") << " sW " << sData->GetSWeight(i,"kstg_sig_y") << " total " << sData->GetSumOfEventSWeight(i) << endl;
  }

  TCanvas *canv1 = new TCanvas();
  RooPlot *mplot = w->var("B0_MM")->frame();
  data->plotOn(mplot);
  TObject *dLeg = mplot->getObject(mplot->numItems()-1);
  pdf->plotOn(mplot);
  TObject *pLeg = mplot->getObject(mplot->numItems()-1);
  pdf->plotOn(mplot,Components(*w->pdf("kstg_sig")),LineStyle(kDashed),LineColor(kRed));
  TObject *sLeg = mplot->getObject(mplot->numItems()-1);
  pdf->plotOn(mplot,Components(*w->pdf("kstg_bkg")),LineStyle(kDashed),LineColor(kGreen+1));
  TObject *bLeg = mplot->getObject(mplot->numItems()-1);
  mplot->SetTitle("Fit to B_{d} mass");
  mplot->GetXaxis()->SetTitleOffset(0.8);
  mplot->GetYaxis()->SetTitleOffset(0.7);
  TLegend *l1 = new TLegend(0.5,0.6,0.9,0.9);
  l1->AddEntry(dLeg,"Data","LEP");
  l1->AddEntry(pLeg,"Total PDF","L");
  l1->AddEntry(sLeg,"Signal (2CB)","L");
  l1->AddEntry(bLeg,"Background (EXP)","L");
  l1->SetFillColor(0);
  mplot->Draw();
  l1->Draw("same");
  canv1->Update();
  canv1->Modified();
  canv1->Print("plots/sweight1.pdf");
  canv1->Print("plots/sweight1.png");

  TCanvas *canv2 = new TCanvas();
  RooDataSet *data_swpt = new RooDataSet(data->GetName(),data->GetTitle(),data,*data->get(),0,"kstg_sig_y_sw");
  RooPlot *splot = w->var("gamgams_PT")->frame();
  data_swpt->plotOn(splot);
  RooHist *dh = (RooHist*)splot->getObject(splot->numItems()-1);
  w->data("MCKstG")->plotOn(splot,MarkerColor(kRed),LineColor(kRed));
  RooHist *mh = (RooHist*)splot->getObject(splot->numItems()-1);
  splot->SetTitle("#gamma p_{T} projection");
  splot->GetXaxis()->SetTitle("#gamma p_{T}");
  splot->GetXaxis()->SetTitleOffset(0.8);
  splot->GetYaxis()->SetTitleOffset(0.7);
  TLegend *l2 = new TLegend(0.5,0.6,0.9,0.9);
  l2->AddEntry(dh,"Data (sweighted)","LEP");
  l2->AddEntry(mh,"MC (expected)","LEP");
  l2->SetFillColor(0);
  splot->Draw();
  l2->Draw("same");
  canv2->Update();
  canv2->Modified();
  canv2->Print("plots/sweight2.pdf");
  canv2->Print("plots/sweight2.png");

  TCanvas *canv3 = new TCanvas();
  cout << "here" << endl;
  cout << dh->Integral() << " -- " << mh->Integral() << endl;
  RooHist *residHist = new RooHist(*dh,*mh,1.,-1.,RooAbsData::SumW2);
  cout << residHist->Integral() << endl;
  TLegend *l3 = new TLegend(0.5,0.6,0.9,0.9);
  l3->AddEntry(dh,"Data (sweighted) - MC","LEP");
  residHist->SetTitle("Residual");
  residHist->GetYaxis()->SetTitle("Data-MC");
  residHist->GetXaxis()->SetTitle("#gamma p_{T}");
  residHist->GetXaxis()->SetTitleOffset(0.8);
  residHist->GetYaxis()->SetTitleOffset(0.7);
  residHist->Draw();
  residHist->SetDrawOption("AP");
  TLine *l = new TLine();
  l->SetLineColor(kRed);
  l->SetLineStyle(kDashed);
  l->SetLineWidth(3);
  l->DrawLine(w->var("gamgams_PT")->getMin(),0.,w->var("gamgams_PT")->getMax(),0.);
  l3->Draw("same");
  canv3->Update();
  canv3->Modified();
  canv3->Print("plots/sweight3.pdf");
  canv3->Print("plots/sweight3.png");

}

void Fitter::fit(TString pdf, TString data) {

  if (w->pdf(pdf) && w->data(data)) {
    w->pdf(pdf)->fitTo(*w->data(data));
  }
}

void Fitter::freeze(TString pdf){
	((RooArgSet*)w->set(Form("%s_params",pdf.Data())))->setAttribAll("Constant");
}

void Fitter::fit(){

	// mc first
	w->pdf("signal")->fitTo(*(w->data("mc")));
	w->pdf("pizero")->fitTo(*(w->data("mcPi0")));
	w->pdf("jpsix")->fitTo(*(w->data("mcJpsiX")));

	((RooArgSet*)w->set("signal_params"))->setAttribAll("Constant");
	((RooArgSet*)w->set("pizero_params"))->setAttribAll("Constant");
	((RooArgSet*)w->set("jpsix_params"))->setAttribAll("Constant");

	// now fit data
	w->pdf("pdf")->fitTo(*(w->data("data")));

	hasfit = true;
}

void Fitter::plot(TString var, TString data, TString pdf){

  RooPlot *plot = w->var(var)->frame(Title(data));
  plot->GetXaxis()->SetTitleOffset(0.8);
  if (w->data(data)) w->data(data)->plotOn(plot);
  if (w->pdf(pdf)) w->pdf(pdf)->plotOn(plot);
  TCanvas *canv = new TCanvas();
  canv->SetBottomMargin(0.2);
  plot->Draw();
  canv->Update();
  canv->Modified();
  canv->Print(Form("plots/v%s_d%s_p%s.pdf",var.Data(),data.Data(),pdf.Data()));
  canv->Print(Form("plots/v%s_d%s_p%s.png",var.Data(),data.Data(),pdf.Data()));
}

void Fitter::plot(){

	TCanvas *canv = new TCanvas("c","c",800,600);
	canv->Divide(2,2);

	// signal mc
	canv->cd(1);
	RooPlot *mcplot = w->var("B0_MM")->frame(Title("MC: J/#psi#gamma"),Range(4750,6000));
	w->data("mc")->plotOn(mcplot);
	if (hasfit){
		w->pdf("signal")->plotOn(mcplot);
	}
	mcplot->Draw();

	// pizero mc
	canv->cd(2);
	RooPlot *pizeroplot = w->var("B0_MM")->frame(Title("MC: J/#psi#pi^{0}"),Range(4750,6000));
	w->data("mcPi0")->plotOn(pizeroplot);
	if (hasfit){
		w->pdf("pizero")->plotOn(pizeroplot);
	}
	pizeroplot->Draw();

	// psiX mc
	canv->cd(3);
	RooPlot *jpsixplot = w->var("B0_MM")->frame(Title("MC: J/#psiX"),Range(4750,6000));
	w->data("mcJpsiX")->plotOn(jpsixplot);
	if (hasfit) {
		w->pdf("jpsix")->plotOn(jpsixplot);
	}
	jpsixplot->Draw();

	// data
	canv->cd(4);
	RooPlot *dplot = w->var("B0_MM")->frame(Title("Data"));
	//TLegend *leg = new TLegend(0.5,0.6,0.89,0.89);
	//leg->SetFillColor(0);
	w->data("data")->plotOn(dplot);
	if (hasfit) {
		/*
		w->pdf("pdf")->plotOn(dplot,Components("background,jpsix,pizero,signal"),FillColor(kGreen-3),LineColor(kGreen-3),DrawOption("F"));
		TObject *sigLeg = (TObject*)dplot->getObject(dplot->numItems()-1);
		w->pdf("pdf")->plotOn(dplot,Components("background,jpsix,pizero"),FillColor(kBlue-7),LineColor(kBlue-7),DrawOption("F"));
		TObject *pizeroLeg = (TObject*)dplot->getObject(dplot->numItems()-1);
		w->pdf("pdf")->plotOn(dplot,Components("backgorund,jpsix"),FillColor(kRed-7),LineColor(kRed-7),DrawOption("F"));
		TObject *jpsixLeg = (TObject*)dplot->getObject(dplot->numItems()-1);
		w->pdf("pdf")->plotOn(dplot,Components("background"),LineColor(kOrange-2),FillColor(kOrange-2),DrawOption("F"));
		TObject *backgroundLeg = (TObject*)dplot->getObject(dplot->numItems()-1);
		w->data("data")->plotOn(dplot);
		TObject *dataLeg = (TObject*)dplot->getObject(dplot->numItems()-1);
		w->pdf("pdf")->plotOn(dplot,DrawOption("L"));
		TObject *pdfLeg = (TObject*)dplot->getObject(dplot->numItems()-1);

		leg->AddEntry(dataLeg,"Data (3fb^{-1})","LEP");
		leg->AddEntry(sigLeg,"J/#psi#gamma","LF");
		leg->AddEntry(pizeroLeg,"J/#psi#pi^{0}","LF");
		leg->AddEntry(jpsixLeg,"J/#psiX","LF");
		leg->AddEntry(backgroundLeg,"Combinatorial","LF");
		leg->AddEntry(pdfLeg,"Total PDF","L");
		*/
	}
	//dplot->Draw();
	//leg->Draw("same");

	canv->Update();
	canv->Modified();
	canv->Print("plots/datasets.pdf");
	delete canv;

	TCanvas *canvAlone = new TCanvas();
	dplot->Draw();
	//leg->Draw("same");
	canvAlone->Update();
	canvAlone->Modified();
	canvAlone->Print("plots/datafit.pdf");
	delete canvAlone;

	TCanvas *canvLog = new TCanvas();
	dplot->GetYaxis()->SetRangeUser(0.1,500);
	dplot->Draw();
	//leg->Draw("same");
	canvLog->SetLogy();
	canvLog->Update();
	canvLog->Modified();
	canvLog->Print("plots/datafit_log.pdf");
	delete canvLog;

}

void Fitter::save(TString fname){
	w->writeToFile(fname);
}

