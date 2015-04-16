#include "../interface/RunCLs.h"

#include "TCanvas.h"
#include "TH1F.h"

#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooAbsData.h"
#include "RooDataSet.h"
#include "RooAbsPdf.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "RooFitResult.h"
#include "RooRandom.h"

using namespace std;
using namespace RooFit;

RunCLs::RunCLs():
  toyn(0)
{
}

RunCLs::~RunCLs(){
}

void RunCLs::setupTrees() {

  for (unsigned int i=0; i<bf_hypoth_vals.size(); i++){
    resultTrees.push_back(new TTree(Form("result_tree_p%d",i),"CLs Result Tree"));
    resultTrees[i]->Branch("toyn",               &toyn);
    resultTrees[i]->Branch("hypothBF",           &hypothBF          );
    resultTrees[i]->Branch("btoy_fitBF",         &btoy_fitBF        );
    resultTrees[i]->Branch("btoy_bestMinNll",    &btoy_bestMinNll   );
    resultTrees[i]->Branch("btoy_hypothMinNll",  &btoy_hypothMinNll );
    resultTrees[i]->Branch("btoy_teststat",      &btoy_teststat     );
    resultTrees[i]->Branch("sbtoy_fitBF",        &sbtoy_fitBF       );
    resultTrees[i]->Branch("sbtoy_bestMinNll",   &sbtoy_bestMinNll  );
    resultTrees[i]->Branch("sbtoy_hypothMinNll", &sbtoy_hypothMinNll);
    resultTrees[i]->Branch("sbtoy_teststat",     &sbtoy_teststat    );
  }

}

void RunCLs::setup(TString infile, TString outfile){

  inFile = TFile::Open(infile);
  w = (RooWorkspace*)((RooWorkspace*)inFile->Get("model_ws"))->Clone("w");
  system("mkdir -p plots/stats/pdf");
  system("mkdir -p plots/stats/png");
  system("mkdir -p plots/stats/C");

  // for Bd ----
  //bf_hypoth_vals.push_back(1.e-7);
  //bf_hypoth_vals.push_back(2.e-7);
  //bf_hypoth_vals.push_back(3.e-7);
  //bf_hypoth_vals.push_back(4.e-7);
  // for both ----
  bf_hypoth_vals.push_back(5.e-7);
  bf_hypoth_vals.push_back(6.e-7);
  bf_hypoth_vals.push_back(7.e-7);
  bf_hypoth_vals.push_back(8.e-7);
  bf_hypoth_vals.push_back(9.e-7);
  bf_hypoth_vals.push_back(1.e-6);
  bf_hypoth_vals.push_back(1.25e-6);
  bf_hypoth_vals.push_back(1.50e-6);
  bf_hypoth_vals.push_back(1.75e-6);
  bf_hypoth_vals.push_back(2.e-6);
  bf_hypoth_vals.push_back(3.e-6);
  bf_hypoth_vals.push_back(4.e-6);
  bf_hypoth_vals.push_back(5.e-6);
  bf_hypoth_vals.push_back(6.e-6);
  bf_hypoth_vals.push_back(7.e-6);
  // for Bs ----
  bf_hypoth_vals.push_back(8.e-6);
  bf_hypoth_vals.push_back(9.e-6);
  bf_hypoth_vals.push_back(1.e-5);
  bf_hypoth_vals.push_back(2.e-5);

  RooRandom::randomGenerator()->SetSeed(0);

  outFile = TFile::Open(outfile,"RECREATE");

  cout << "Setting up" << endl;
  // setup out trees
  setupTrees();

  // kill messages
  RooMsgService::instance().setGlobalKillBelow(FATAL);
  RooMsgService::instance().setSilentMode(true);

  w->var("BF")->setMin(0.0);

  // get fit parameters
  RooArgSet *params = (RooArgSet*)w->pdf("simpdf")->getParameters(w->data("data"));
  w->defineSet("fit_parameters",*params);

  // fit bkg only and save snapshot
  w->var("BF")->setConstant(false);
  w->var("BF")->setVal(0.0);
  w->var("BF")->setConstant(true);
  w->pdf("simpdf")->fitTo(*w->data("data"),Extended());
  w->saveSnapshot("bkgonly_fit",*params);
  w->var("BF")->setConstant(false);

  // fit s+b and save snapshot
  for (unsigned int i=0; i<bf_hypoth_vals.size(); i++) {
    w->var("BF")->setConstant(false);
    w->var("BF")->setVal(bf_hypoth_vals[i]);
    w->var("BF")->setConstant(true);
    w->pdf("simpdf")->fitTo(*w->data("data"),Extended());
    w->saveSnapshot(Form("splusb_fit_p%d",i),*params);
    w->var("BF")->setConstant(false);
  }

  // do best fit and save snapshot
  w->var("BF")->setConstant(false);
  w->pdf("simpdf")->fitTo(*w->data("data"),Extended());
  w->saveSnapshot("splusb_fit_bestfit",*params);
  w->var("BF")->setConstant(false);
}

void RunCLs::save(){
  outFile->cd();
  w->Write();
  for (unsigned int i=0; i<resultTrees.size(); i++){
    resultTrees[i]->Write();
  }
  inFile->Close();
  outFile->Close();
}

void RunCLs::setSeed(int seed){
  RooRandom::randomGenerator()->SetSeed(seed);
}

void RunCLs::runData() {

  cout << "Computing Test Stat in Data: " << endl;

  toyn = -1;

  // do best fit first
  w->var("BF")->setConstant(false);
  RooFitResult *dataBestFitRes = w->pdf("simpdf")->fitTo(*w->data("data"),Extended(),Save());
  btoy_fitBF = w->var("BF")->getVal();
  sbtoy_fitBF = w->var("BF")->getVal();
  btoy_bestMinNll = dataBestFitRes->minNll();
  sbtoy_bestMinNll = dataBestFitRes->minNll();

  // loop each signal hypo and fill test stat
  for (unsigned int i=0; i<bf_hypoth_vals.size(); i++){
    cout << "\t" << "Fit " << i+1 << "/" << bf_hypoth_vals.size() << endl;
    hypothBF = bf_hypoth_vals[i];
    w->var("BF")->setConstant(false);
    w->var("BF")->setVal(hypothBF);
    w->var("BF")->setConstant(true);
    RooFitResult *dataSBRes = w->pdf("simpdf")->fitTo(*w->data("data"),Extended(),Save());
    btoy_hypothMinNll = dataSBRes->minNll();
    sbtoy_hypothMinNll = dataSBRes->minNll();
    btoy_teststat = 2.*(btoy_hypothMinNll - btoy_bestMinNll);
    sbtoy_teststat = 2.*(sbtoy_hypothMinNll - sbtoy_bestMinNll);

    resultTrees[i]->Fill();

  }

  toyn++;

}

void RunCLs::runToy() {

  cout << "Running Toy " << toyn << endl;

  RooArgSet *observables = w->pdf("simpdf")->getObservables(*w->data("data"));
  int nevents = int(w->data("data")->numEntries());

  // bkg only toy
  w->loadSnapshot("bkgonly_fit");
  RooDataSet *btoydset = w->pdf("simpdf")->generate(*observables,NumEvents(nevents),Extended());
  // find best fit of bkg only toy
  w->var("BF")->setConstant(false);
  RooFitResult *bkgRes = w->pdf("simpdf")->fitTo(*btoydset,Extended(),Save());
  cout << "\t" << "Fit 1/" << (bf_hypoth_vals.size()*3)+1 << endl;
  btoy_fitBF = w->var("BF")->getVal();
  btoy_bestMinNll = bkgRes->minNll();


  // now loop signal hypos fitting the bkg only toy and throwing s+b toy and refitting
  for (unsigned int i=0; i<bf_hypoth_vals.size(); i++){

    // 1. fit bkg only toy with s+b hypoth pdf
    hypothBF = bf_hypoth_vals[i];
    w->var("BF")->setConstant(false);
    w->var("BF")->setVal(hypothBF);
    w->var("BF")->setConstant(true);
    RooFitResult *sbRes = w->pdf("simpdf")->fitTo(*btoydset,Extended(),Save());
    cout << "\t" << "Fit " << (i*3)+2 << "/" << (bf_hypoth_vals.size()*3)+1 << endl;
    btoy_hypothMinNll = sbRes->minNll();
    btoy_teststat = 2.*(btoy_hypothMinNll-btoy_bestMinNll);
    w->var("BF")->setConstant(false);

    // 2. throw s+b hypoth toy
    w->loadSnapshot(Form("splusb_fit_p%d",i));
    RooDataSet *sbtoydset = w->pdf("simpdf")->generate(*observables,NumEvents(nevents),Extended());

    // 3. find best fit of s+b toy
    w->var("BF")->setConstant(false);
    RooFitResult *bestRes = w->pdf("simpdf")->fitTo(*sbtoydset,Extended(),Save());
    cout << "\t" << "Fit " << (i*3)+3 << "/" << (bf_hypoth_vals.size()*3)+1 << endl;
    sbtoy_fitBF = w->var("BF")->getVal();
    sbtoy_bestMinNll = bestRes->minNll();

    // 4. find hypoth fit of s+b toy
    w->var("BF")->setConstant(false);
    w->var("BF")->setVal(hypothBF);
    w->var("BF")->setConstant(true);
    RooFitResult *hypothRes = w->pdf("simpdf")->fitTo(*sbtoydset,Extended(),Save());
    cout << "\t" << "Fit " << (i*3)+4 << "/" << (bf_hypoth_vals.size()*3)+1 << endl;
    sbtoy_hypothMinNll = hypothRes->minNll();
    sbtoy_teststat = 2.*(sbtoy_hypothMinNll-sbtoy_bestMinNll);
    w->var("BF")->setConstant(false);

    resultTrees[i]->Fill();

  }
  toyn++;

}

double RunCLs::getQValue(double hypothval, RooAbsData *dset){

  if (!w->pdf("simpdf")) {
    cout << "ERROR -- pdf pointer is null" << endl;
    exit(1);
  }
  if (!dset) {
    cout << "ERROR -- dset pointer is null" << endl;
    exit(1);
  }

  // BEST-FIT
  w->var("BF")->setConstant(false);
  RooFitResult *bestFitRes = w->pdf("simpdf")->fitTo(*dset,Extended(),Save());
  double bestFitNll = bestFitRes->minNll();

  // HYPOTH FIT
  w->var("BF")->setConstant(false);
  w->var("BF")->setVal(hypothval);
  w->var("BF")->setConstant(true);
  RooFitResult *hypothFitRes = w->pdf("simpdf")->fitTo(*dset,Extended(),Save());
  double hypothFitNll = hypothFitRes->minNll();

  double testStatVal = 2.*(hypothFitNll-bestFitNll);

  return testStatVal;

}

void RunCLs::plotCLs(TString fname) {

  TFile *plotFile = TFile::Open(fname);

  int i=0;
  TTree *tree;
  while ( (tree = (TTree*)plotFile->Get(Form("result_tree_p%d",i))) ) {

    tree->SetBranchAddress("toyn",               &toyn);
    tree->SetBranchAddress("hypothBF",           &hypothBF          );
    tree->SetBranchAddress("btoy_fitBF",         &btoy_fitBF        );
    tree->SetBranchAddress("btoy_bestMinNll",    &btoy_bestMinNll   );
    tree->SetBranchAddress("btoy_hypothMinNll",  &btoy_hypothMinNll );
    tree->SetBranchAddress("btoy_teststat",      &btoy_teststat     );
    tree->SetBranchAddress("sbtoy_fitBF",        &sbtoy_fitBF       );
    tree->SetBranchAddress("sbtoy_bestMinNll",   &sbtoy_bestMinNll  );
    tree->SetBranchAddress("sbtoy_hypothMinNll", &sbtoy_hypothMinNll);
    tree->SetBranchAddress("sbtoy_teststat",     &sbtoy_teststat    );

    TH1F *btoys = new TH1F("btoys","btoys",100,0,25);
    TH1F *sbtoys = new TH1F("sbtoys","sbtoys",100,0,25);
    TH1F *data = new TH1F("data","data",10000,0,25);

    for (int entry=0; entry<tree->GetEntries(); entry++){
      tree->GetEntry(entry);

      if (toyn<0) { // data
        data->Fill(sbtoy_teststat);
        continue;
      }
      if ( btoy_fitBF > hypothBF ) { // one-sided
        btoys->Fill(0.);
      }
      else {
        btoys->Fill(btoy_teststat);
      }
      if ( sbtoy_fitBF > hypothBF ) { // one-sided
        sbtoys->Fill(0.);
      }
      else {
        sbtoys->Fill(sbtoy_teststat);
      }
    }

    btoys->SetLineColor(kBlue);
    btoys->SetLineWidth(2);
    sbtoys->SetLineColor(kRed);
    sbtoys->SetLineWidth(2);
    data->SetLineWidth(5);

    TCanvas *canv = new TCanvas();
    sbtoys->Draw("HIST");
    btoys->Draw("HISTsame");
    data->Draw("HISTsame");
    canv->Print("toys.pdf");

    i++;
  }

  plotFile->Close();
}
