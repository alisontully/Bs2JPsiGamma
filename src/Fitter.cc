#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLegend.h"

#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooCBShape.h"
#include "RooAddPdf.h"

#include "../interface/Fitter.h"

using namespace std;
using namespace RooFit;

Fitter::Fitter(TString wsname):
	massRange(4500,7000),
	bdtcut(0.2),
	hasfit(false)
{
	w = new RooWorkspace(wsname);
}

Fitter::~Fitter(){
	//delete w;
	//tf->Close();
	//delete tf;
}

void Fitter::setMassRange(double mlow, double mhigh){
	massRange.first = mlow;
	massRange.second = mhigh;
}

void Fitter::setBDTCut(double val){
	bdtcut = val;
}

void Fitter::readTree(TString fname, TString tname){

	TFile *tf = TFile::Open(fname);
	TTree *tree = (TTree*)tf->Get(tname);

	RooDataSet *data = new RooDataSet("data","data",RooArgSet(*(w->var("mass"))));
	RooDataSet *mc = new RooDataSet("mc","mc",RooArgSet(*(w->var("mass"))));
	RooDataSet *mcPi0 = new RooDataSet("mcPi0","mcPi0",RooArgSet(*(w->var("mass"))));
	RooDataSet *mcJpsiX = new RooDataSet("mcJpsiX","mcJpsiX",RooArgSet(*(w->var("mass"))));
	RooDataSet *mcBd = new RooDataSet("mcBd","mcBd",RooArgSet(*(w->var("mass"))));
	RooDataSet *mcBu = new RooDataSet("mcBu","mcBu",RooArgSet(*(w->var("mass"))));

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
		if ( B0_M < w->var("mass")->getMin() || B0_M > w->var("mass")->getMax() ) continue;

		w->var("mass")->setVal(B0_M);

		if ( itype == -81 ) {
			mc->add(RooArgSet(*(w->var("mass"))));
		}
		if ( itype == -82 ) {
			mcPi0->add(RooArgSet(*(w->var("mass"))));
		}
		if ( itype == -83 ){
			mcBd->add(RooArgSet(*(w->var("mass"))));
			mcJpsiX->add(RooArgSet(*(w->var("mass"))));
		}
		if ( itype == -84 ){
			mcBu->add(RooArgSet(*(w->var("mass"))));
			mcJpsiX->add(RooArgSet(*(w->var("mass"))));
		}
		//if ( itype == 82 ) {
		if ( itype == 71 || itype == 81 ) {
			data->add(RooArgSet(*(w->var("mass"))));
		}
	}

	w->import(*data);
	w->import(*mc);
	w->import(*mcPi0);
	w->import(*mcBd);
	w->import(*mcBu);
	w->import(*mcJpsiX);

	delete tree;
	tf->Close();
	delete tf;
	delete data;
	delete mc;
	delete mcPi0;
	delete mcBd;
	delete mcBu;
	delete mcJpsiX;
}

void Fitter::constructSignalPdf(){

	w->factory("mean[5300,5400]");
	w->factory("CBShape::sig_cb1(mass,mean,sigma_1[100,0,1000],alpha_1[0.,10.],n_1[0.,10.])");
	w->factory("CBShape::sig_cb2(mass,mean,sigma_2[100,0,1000],alpha_2[-10.,0.],n_2[0.,10.])");
	w->factory("SUM::signal( f[0.5,0,1] * sig_cb1, sig_cb2 )");

	RooArgSet *argset = w->pdf("signal")->getParameters(*(w->var("mass")));
	w->defineSet("signal_params",*argset);
	delete argset;
}

void Fitter::constructPiZeroPdf(){

	w->factory("pi0_mean[5100,5400]");
	w->factory("CBShape::pi0_cb1(mass,pi0_mean,pi0_sigma_1[100,0,1000],pi0_alpha_1[0.,10.],pi0_n_1[0.,20.])");
	w->factory("CBShape::pi0_cb2(mass,pi0_mean,pi0_sigma_2[100,0,1000],pi0_alpha_2[-10.,0.],pi0_n_2[0.,10.])");
	w->factory("SUM::pizero( pi0_f[0.5,0,1] * pi0_cb1, pi0_cb2 )");

	RooArgSet *argset = w->pdf("pizero")->getParameters(*(w->var("mass")));
	w->defineSet("pizero_params",*argset);
	delete argset;
}

void Fitter::constructJpsiXPdf(){

	w->factory("Exponential::jpsix( mass, jpsix_p1[-0.05,0.] )");

	RooArgSet *argset = w->pdf("jpsix")->getParameters(*(w->var("mass")));
	w->defineSet("jpsix_params",*argset);
	delete argset;
}

void Fitter::constructBkgPdf(){

	w->factory("Exponential::background( mass, bkg_p1[-0.05,-0.1,0.] )");

	RooArgSet *argset = w->pdf("background")->getParameters(*(w->var("mass")));
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

	RooArgSet *argset = w->pdf("pdf")->getParameters(*(w->var("mass")));
	w->defineSet("pdf_params",*argset);
	delete argset;
}

void Fitter::setup(TString fname, TString tname){

	w->factory(Form("mass[%f,%f]",massRange.first,massRange.second));

	readTree(fname,tname);

	constructSignalPdf();
	constructPiZeroPdf();
	constructJpsiXPdf();
	constructBkgPdf();
	constructFullPdf();

	w->Print();

	//w->var("mass")->Print();
	//w->data("data")->Print();
	//w->data("mc")->Print();

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

void Fitter::plot(){

	TCanvas *canv = new TCanvas("c","c",800,600);
	canv->Divide(2,2);

	// signal mc
	canv->cd(1);
	RooPlot *mcplot = w->var("mass")->frame(Title("MC: J/#psi#gamma"),Range(4750,6000));
	w->data("mc")->plotOn(mcplot);
	if (hasfit){
		w->pdf("signal")->plotOn(mcplot);
	}
	mcplot->Draw();

	// pizero mc
	canv->cd(2);
	RooPlot *pizeroplot = w->var("mass")->frame(Title("MC: J/#psi#pi^{0}"),Range(4750,6000));
	w->data("mcPi0")->plotOn(pizeroplot);
	if (hasfit){
		w->pdf("pizero")->plotOn(pizeroplot);
	}
	pizeroplot->Draw();

	// psiX mc
	canv->cd(3);
	RooPlot *jpsixplot = w->var("mass")->frame(Title("MC: J/#psiX"),Range(4750,6000));
	w->data("mcJpsiX")->plotOn(jpsixplot);
	if (hasfit) {
		w->pdf("jpsix")->plotOn(jpsixplot);
	}
	jpsixplot->Draw();

	// data
	canv->cd(4);
	RooPlot *dplot = w->var("mass")->frame(Title("Data"));
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

