#include "../interface/AssessBDTCutFit.h"

using namespace std;
using namespace RooFit;

AssessBDTCutFit::AssessBDTCutFit(TString wsname, TString name, bool verbose, bool debug):
  FitterBase(wsname,name,verbose,debug)
{}

AssessBDTCutFit::~AssessBDTCutFit(){}

void AssessBDTCutFit::addObsVars(){

  addObsVar("B0_MM",         "m(K^{#pm}#pi^{#mp}#gamma)",  "MeV",    4500,   6000);
}

void AssessBDTCutFit::addCuts(){
}

void AssessBDTCutFit::addDatasets(){

  addDataset("DataJpsiG",  "Data (J/#psi#gamma)",  71, 81);

  addDataset("DataJpsiGLL", "Data (J/#psi#gamma) LL", 71, 81);
  addRequirement("DataJpsiGLL","eplus_TRACK_Type",          3);
  addRequirement("DataJpsiGLL","eminus_TRACK_Type",         3);

  addDataset("DataJpsiGDD", "Data (J/#psi#gamma) DD", 71, 81);
  addRequirement("DataJpsiGDD","eplus_TRACK_Type",          5);
  addRequirement("DataJpsiGDD","eminus_TRACK_Type",         5);

  addDataset("MCJpsiG",    "MC (J/#psi#gamma)",   -81);

  addDataset("MCJpsiGLL",  "MC (J/#psi#gamma) LL",-81);
  addRequirement("MCJpsiGLL","eplus_TRACK_Type",          3);
  addRequirement("MCJpsiGLL","eminus_TRACK_Type",         3);

  addDataset("MCJpsiGDD",  "MC (J/#psi#gamma) DD",-81);
  addRequirement("MCJpsiGDD","eplus_TRACK_Type",          5);
  addRequirement("MCJpsiGDD","eminus_TRACK_Type",         5);

  addDataset("DataKstG",  "Data (K*#gamma)",  72, 82);

  addDataset("DataKstGLL", "Data (K*#gamma) LL", 72, 82);
  addRequirement("DataKstGLL","eplus_TRACK_Type",          3);
  addRequirement("DataKstGLL","eminus_TRACK_Type",         3);

  addDataset("DataKstGDD", "Data (K*#gamma) DD", 72, 82);
  addRequirement("DataKstGDD","eplus_TRACK_Type",          5);
  addRequirement("DataKstGDD","eminus_TRACK_Type",         5);

  addDataset("MCKstG",    "MC (K*#gamma)",   -88);

  addDataset("MCKstGLL",  "MC (K*#gamma) LL",-88);
  addRequirement("MCKstGLL","eplus_TRACK_Type",          3);
  addRequirement("MCKstGLL","eminus_TRACK_Type",         3);

  addDataset("MCKstGDD",  "MC (K*#gamma) DD",-88);
  addRequirement("MCKstGDD","eplus_TRACK_Type",          5);
  addRequirement("MCKstGDD","eminus_TRACK_Type",         5);

}

void AssessBDTCutFit::constructPdfs(){

  // KstG MC LL
  w->factory("kstg_ll_mean[5200,5300]");
  w->factory("kstg_ll_sigma[10,200]");
  w->factory("CBShape:kstg_ll_cb1(B0_MM,kstg_ll_mean,kstg_ll_sigma,kstg_ll_alpha_1[0.,5],kstg_ll_n_1[0.1,5.])");
  w->factory("CBShape:kstg_ll_cb2(B0_MM,kstg_ll_mean,kstg_ll_sigma,kstg_ll_alpha_2[-5.,0],kstg_ll_n_2[0.1,5.])");
  w->factory("SUM::kstg_sig_ll_pdf( kstg_ll_f_1[0.5,0.,1.]*kstg_ll_cb1, kstg_ll_cb2 )");
  defineParamSet("kstg_sig_ll_pdf");
  w->pdf("kstg_sig_ll_pdf")->SetTitle("PDF (K*#gamma) LL");

  // KstG MC DD
  w->factory("kstg_dd_mean[5200,5300]");
  w->factory("kstg_dd_sigma[10,200]");
  w->factory("CBShape:kstg_dd_cb1(B0_MM,kstg_dd_mean,kstg_dd_sigma,kstg_dd_alpha_1[0.,5.],kstg_dd_n_1[0.1,50.])");
  w->factory("CBShape:kstg_dd_cb2(B0_MM,kstg_dd_mean,kstg_dd_sigma,kstg_dd_alpha_2[-5.,0.],kstg_dd_n_2[0.1,50.])");
  w->factory("SUM::kstg_sig_dd_pdf( kstg_dd_f_1[0.5,0.,1.]*kstg_dd_cb1, kstg_dd_cb2 )");
  defineParamSet("kstg_sig_dd_pdf");
  w->pdf("kstg_sig_dd_pdf")->SetTitle("PDF (K*#gamma) DD");

  // KstG MC
  w->factory("SUM::kstg_sig_pdf( kstg_sig_ll_f[0.5,0.,1.]*kstg_sig_ll_pdf, kstg_sig_dd_pdf )");
  defineParamSet("kstg_sig_pdf");
  w->pdf("kstg_sig_pdf")->SetTitle("PDF (K*#gamma) Signal");

  // KstG MC Bkg
  w->factory("Exponential::kstg_bkg_pdf( B0_MM, kstg_exp_p1[-0.02,0.] )");
  defineParamSet("kstg_bkg_pdf");
  w->pdf("kstg_bkg_pdf")->SetTitle("PDF (K*#gamma) Background");

  // KstG TOTAL
  w->factory("SUM::kstg_pdf( kstg_sig_y[0,5000]*kstg_sig_pdf, kstg_bkg_y[0,10000]*kstg_bkg_pdf )");
  defineParamSet("kstg_pdf");
  defineYieldSet("kstg_pdf");
  w->pdf("kstg_pdf")->SetTitle("PDF (K*#gamma)");

  // JpsiG MC LL
  w->factory("jpsig_ll_mean[5300,5400]");
  w->factory("jpsig_ll_sigma[10,200]");
  w->factory("CBShape:jpsig_ll_cb1(B0_MM,jpsig_ll_mean,jpsig_ll_sigma,jpsig_ll_alpha_1[0.,5],jpsig_ll_n_1[0.1,5.])");
  w->factory("CBShape:jpsig_ll_cb2(B0_MM,jpsig_ll_mean,jpsig_ll_sigma,jpsig_ll_alpha_2[-5.,0],jpsig_ll_n_2[0.1,5.])");
  w->factory("SUM::jpsig_sig_ll_pdf( jpsig_ll_f_1[0.5,0.,1.]*jpsig_ll_cb1, jpsig_ll_cb2 )");
  defineParamSet("jpsig_sig_ll_pdf");
  w->pdf("jpsig_sig_ll_pdf")->SetTitle("PDF (J/#psi#gamma) LL");

  // JpsiG MC DD
  w->factory("jpsig_dd_mean[5300,5400]");
  w->factory("jpsig_dd_sigma[10,200]");
  w->factory("CBShape:jpsig_dd_cb1(B0_MM,jpsig_dd_mean,jpsig_dd_sigma,jpsig_dd_alpha_1[0.,5.],jpsig_dd_n_1[0.1,50.])");
  w->factory("CBShape:jpsig_dd_cb2(B0_MM,jpsig_dd_mean,jpsig_dd_sigma,jpsig_dd_alpha_2[-5.,0.],jpsig_dd_n_2[0.1,50.])");
  w->factory("SUM::jpsig_sig_dd_pdf( jpsig_dd_f_1[0.5,0.,1.]*jpsig_dd_cb1, jpsig_dd_cb2 )");
  defineParamSet("jpsig_sig_dd_pdf");
  w->pdf("jpsig_sig_dd_pdf")->SetTitle("PDF (J/#psi#gamma) DD");

  // JpsiG MC
  w->factory("SUM::jpsig_sig_pdf( jpsig_sig_ll_f[0.5,0.,1.]*jpsig_sig_ll_pdf, jpsig_sig_dd_pdf )");
  defineParamSet("jpsig_sig_pdf");
  w->pdf("jpsig_sig_pdf")->SetTitle("PDF (J/#psi#gamma) Signal");

  // JpsiG MC Bkg LL
  w->factory("Exponential::jpsig_bkg_ll_pdf( B0_MM, jpsig_ll_exp_p1[-0.05,0.] )");
  defineParamSet("jpsig_bkg_ll_pdf");
  w->pdf("jpsig_bkg_ll_pdf")->SetTitle("PDF (J/#psi#gamma) Background LL");

  // JpsiG MC Bkg
  w->factory("Exponential::jpsig_bkg_dd_pdf( B0_MM, jpsig_dd_exp_p1[-0.05,0.] )");
  defineParamSet("jpsig_bkg_dd_pdf");
  w->pdf("jpsig_bkg_dd_pdf")->SetTitle("PDF (J/#psi#gamma) Background DD");

  // JpsiG MC Bkg
  w->factory("Exponential::jpsig_bkg_pdf( B0_MM, jpsig_exp_p1[-0.02,0.] )");
  defineParamSet("jpsig_bkg_pdf");
  w->pdf("jpsig_bkg_pdf")->SetTitle("PDF (J/#psi#gamma) Background");

  // JpsiG TOTAL LL
  w->factory("SUM::jpsig_ll_pdf( jpsig_sig_ll_y[0,5000]*jpsig_sig_ll_pdf, jpsig_bkg_ll_y[0,1e5]*jpsig_bkg_ll_pdf )");
  defineParamSet("jpsig_ll_pdf");
  defineYieldSet("jpsig_ll_pdf");
  w->pdf("jpsig_ll_pdf")->SetTitle("PDF (J/#psi#gamma) LL");

  // JpsiG TOTAL DD
  w->factory("SUM::jpsig_dd_pdf( jpsig_sig_dd_y[0,5000]*jpsig_sig_dd_pdf, jpsig_bkg_dd_y[0,1e5]*jpsig_bkg_dd_pdf )");
  defineParamSet("jpsig_dd_pdf");
  defineYieldSet("jpsig_dd_pdf");
  w->pdf("jpsig_dd_pdf")->SetTitle("PDF (J/#psi#gamma) DD");

  // JpsiG TOTAL
  w->factory("SUM::jpsig_pdf( jpsig_sig_y[0,5000]*jpsig_sig_pdf, jpsig_bkg_y[0,1e5]*jpsig_bkg_pdf )");
  defineParamSet("jpsig_pdf");
  defineYieldSet("jpsig_pdf");
  w->pdf("jpsig_pdf")->SetTitle("PDF (J/#psi#gamma)");

}

void AssessBDTCutFit::run(){

  if ( verbose || debug ) w->Print();

  fit("kstg_sig_ll_pdf","MCKstGLL");
  plot("B0_MM","MCKstGLL","kstg_sig_ll_pdf","LL MC Fit");
  freeze("kstg_sig_ll_pdf");

  fit("kstg_sig_dd_pdf","MCKstGDD");
  plot("B0_MM","MCKstGDD","kstg_sig_dd_pdf","DD MC Fit");
  freeze("kstg_sig_dd_pdf");

  fit("kstg_sig_pdf","MCKstG");
  plot("B0_MM","MCKstG","kstg_sig_pdf","MC Fit");
  freeze("kstg_sig_pdf");
  w->var("kstg_sig_ll_f")->setConstant(false);

  fit("kstg_pdf","DataKstG");
  plot("B0_MM","DataKstG","kstg_pdf","Data Fit");

  makeKstarPlot();

  fit("jpsig_sig_ll_pdf","MCJpsiGLL");
  plot("B0_MM","MCJpsiGLL","jpsig_sig_ll_pdf","LL MC Fit");
  freeze("jpsig_sig_ll_pdf");

  fit("jpsig_sig_dd_pdf","MCJpsiGDD");
  plot("B0_MM","MCJpsiGDD","jpsig_sig_dd_pdf","DD MC Fit");
  freeze("jpsig_sig_dd_pdf");

  // set frac to be the same as in kstg
  w->var("jpsig_sig_ll_f")->setVal(w->var("kstg_sig_ll_f")->getVal());
  w->var("jpsig_sig_ll_f")->setConstant(true);

  fit("jpsig_ll_pdf","DataJpsiGLL");
  plot("B0_MM","DataJpsiGLL","jpsig_ll_pdf","Data Fit LL");

  fit("jpsig_dd_pdf","DataJpsiGDD");
  plot("B0_MM","DataJpsiGDD","jpsig_dd_pdf","Data Fit DD");

  double tot_exp_sig = 26.; // this comes from theory prediction of 40 with 80% trig eff * 80% presel eff
  double frac_ll_sig = w->var("kstg_sig_ll_f")->getVal();
  double exp_sig_ll = tot_exp_sig*frac_ll_sig;
  double exp_sig_dd = tot_exp_sig*(1.-frac_ll_sig);

  double exp_bkg_ll = integral("jpsig_ll_pdf","B0_MM","jpsig_bkg_ll_y",5250,5400);
  double exp_bkg_dd = integral("jpsig_dd_pdf","B0_MM","jpsig_bkg_dd_y",5250,5400);
  double frac_ll_bkg = exp_bkg_ll/(exp_bkg_ll+exp_bkg_dd);

  cout << "========= SUMMARY ========" << endl;
  cout << "LL: " << endl;
  cout << "\t nSig = " << exp_sig_ll << endl;
  cout << "\t nBkg = " << exp_bkg_ll << endl;
  cout << "DD: " << endl;
  cout << "\t nSig = " << exp_sig_dd << endl;
  cout << "\t nBkg = " << exp_bkg_dd << endl;
  cout << "==========================" << endl;
  cout << "fLL Sig: " << frac_ll_sig << endl;
  cout << "fll Bkg: " << frac_ll_bkg << endl;
  cout << "==========================" << endl;

}

void AssessBDTCutFit::makeKstarPlot(){

  vector<PlotComponent> plotComps;

  PlotComponent pc_data( "DataKstG", "Data" );
  pc_data.setDefaultDataStyle();

  PlotComponent pc_pdf( "kstg_pdf", "Total PDF" );
  pc_pdf.setSolidLine(kBlue);

  PlotComponent pc_pdf_bkg( "kstg_pdf:kstg_bkg_pdf", "Background" );
  pc_pdf_bkg.setDashedLine(kGreen+1);

  PlotComponent pc_pdf_sig( "kstg_pdf:kstg_sig_pdf", "Signal" );
  pc_pdf_sig.setDashedLine(kRed);

  PlotComponent pc_pdf_sig_ll( "kstg_pdf:kstg_sig_ll_pdf", "Signal LL" );
  pc_pdf_sig_ll.setDashedLine(kMagenta);

  PlotComponent pc_pdf_sig_dd( "kstg_pdf:kstg_sig_dd_pdf", "Signal DD" );
  pc_pdf_sig_dd.setDashedLine(kOrange+1);

  plotComps.push_back(pc_data);
  plotComps.push_back(pc_pdf_bkg);
  plotComps.push_back(pc_pdf_sig_ll);
  plotComps.push_back(pc_pdf_sig_dd);
  plotComps.push_back(pc_pdf_sig);
  plotComps.push_back(pc_pdf);

  w->var("kstg_bkg_y")->SetTitle("N_{bkg}");
  w->var("kstg_sig_y")->SetTitle("N_{sig}");
  w->var("kstg_sig_ll_f")->SetTitle("f_{LL}");
  RooArgSet *argset = new RooArgSet();
  argset->add(*w->var("kstg_bkg_y"));
  argset->add(*w->var("kstg_sig_y"));
  argset->add(*w->var("kstg_sig_ll_f"));

  plot("B0_MM", plotComps, "preselfit_kstar", argset);

  delete argset;
  return;
}

void AssessBDTCutFit::makeJpsiPlot(){
}

