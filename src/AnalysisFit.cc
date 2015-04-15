#include "../interface/AnalysisFit.h"

using namespace std;
using namespace RooFit;

AnalysisFit::AnalysisFit(TString wsname, TString name, bool verbose, bool debug):
  FitterBase(wsname,name,verbose,debug)
{}

AnalysisFit::~AnalysisFit(){}

void AnalysisFit::addObsVars(){

  addObsVar("B0_MM",         "m(#mu^{+}#mu^{-}#gamma)",  "MeV",    4500,   7000);
}

void AnalysisFit::addCuts(){
  addCut("pass_bdt",true);
}

void AnalysisFit::addDatasets(){

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

  addDataset("MCJpsiPiz", "MC (J/#psi#pi^{0})",   -82);
  addDataset("MCJpsiEta", "MC (J/#psi#eta)",      -83);
  addDataset("MCJpsiKs",  "MC (J/#psiK_{S})",     -85);
  addDataset("MCJpsiKst", "MC (J/#psiK*^{0})",    -86);
  addDataset("MCJpsiRho", "MC (J/#psi#rho)",      -87);

}

void AnalysisFit::constructPdfs(){

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
  w->factory("CBShape::jpsig_dd_cb1(B0_MM,jpsig_dd_mean,jpsig_dd_sigma,jpsig_dd_alpha_1[0.,5.],jpsig_dd_n_1[0.1,50.])");
  w->factory("CBShape::jpsig_dd_cb2(B0_MM,jpsig_dd_mean,jpsig_dd_sigma,jpsig_dd_alpha_2[-5.,0.],jpsig_dd_n_2[0.1,50.])");
  w->factory("SUM::jpsig_sig_dd_pdf( jpsig_dd_f_1[0.5,0.,1.]*jpsig_dd_cb1, jpsig_dd_cb2 )");
  defineParamSet("jpsig_sig_dd_pdf");
  w->pdf("jpsig_sig_dd_pdf")->SetTitle("PDF (J/#psi#gamma) DD");

  // JpsiG MC
  w->factory("SUM::jpsig_sig_pdf( jpsig_sig_ll_f[0.5,0.,1.]*jpsig_sig_ll_pdf, jpsig_sig_dd_pdf )");
  defineParamSet("jpsig_sig_pdf");
  w->pdf("jpsig_sig_pdf")->SetTitle("PDF (J/#psi#gamma) Signal");

  // JpsiPiz MC
  w->factory("jpsipiz_mean[5100,5400]");
  w->factory("CBShape::jpsipiz_cb1(B0_MM,jpsipiz_mean,jpsipiz_sigma_1[20,0,100],jpsipiz_alpha_1[0.,5.],jpsipiz_n_1[0.1,50.])");
  w->factory("CBShape::jpsipiz_cb2(B0_MM,jpsipiz_mean,jpsipiz_sigma_2[20,0,100],jpsipiz_alpha_2[-5.,0.],jpsipiz_n_2[0.1,50.])");
  w->factory("SUM::jpsipiz_pdf( jpsipiz_f_1[0.5,0.,1.]*jpsipiz_cb1, jpsipiz_cb2 )");
  defineParamSet("jpsipiz_pdf");
  w->pdf("jpsipiz_pdf")->SetTitle("PDF (J/#psi#pi^{0})");

  // JpsiEta MC
  w->factory("jpsieta_mean[4800,5500]");
  w->factory("CBShape::jpsieta_cb1(B0_MM,jpsieta_mean,jpsieta_sigma_1[20,10,200],jpsieta_alpha_1[0.,20.],jpsieta_n_1[0.1,50.])");
  w->factory("CBShape::jpsieta_cb2(B0_MM,jpsieta_mean,jpsieta_sigma_2[20,10,200],jpsieta_alpha_2[-20.,0.],jpsieta_n_2[0.1,50.])");
  w->factory("SUM::jpsieta_pdf( jpsieta_f_1[0.5,0.,1.]*jpsieta_cb1, jpsieta_cb2 )");
  defineParamSet("jpsieta_pdf");
  w->pdf("jpsieta_pdf")->SetTitle("PDF (J/#psi#eta)");

  // JpsiKs
  w->factory("Gaussian::jpsiks_pdf( B0_MM, jpsiks_mean[4800,5800], jpsiks_sigma[100,80,500] )");
  defineParamSet("jpsiks_pdf");
  w->pdf("jpsiks_pdf")->SetTitle("PDF (J/#psiK_{S})");

  // JpsiKst
  w->factory("Gaussian::jpsikst_pdf( B0_MM, jpsikst_mean[4200,5000], jpsikst_sigma[100,80,500] )");
  defineParamSet("jpsikst_pdf");
  w->pdf("jpsikst_pdf")->SetTitle("PDF (J/#psiK*^{0})");

  // JpsiRho
  w->factory("Gaussian::jpsirho_pdf( B0_MM, jpsirho_mean[4800,5800], jpsirho_sigma[100,80,500] )");
  defineParamSet("jpsirho_pdf");
  w->pdf("jpsirho_pdf")->SetTitle("PDF (J/#psi#rho)");

  // JpsiG MC Bkg
  w->factory("Exponential::jpsig_bkg_pdf( B0_MM, jpsig_exp_p1[-0.02,0.] )");
  defineParamSet("jpsig_bkg_pdf");
  w->pdf("jpsig_bkg_pdf")->SetTitle("PDF (J/#psi#gamma) Background");

  // JpsiG TOTAL
  w->factory("SUM::jpsig_pdf( jpsig_sig_y[0,5000]*jpsig_sig_pdf, jpsig_bkg_y[0,1e5]*jpsig_bkg_pdf, jpsipiz_y[0,1e4]*jpsipiz_pdf, jpsieta_y[0,1e4]*jpsieta_pdf, jpsiks_y[0,5000]*jpsiks_pdf, jpsikst_y[0,5000]*jpsikst_pdf, jpsirho_y[0,5000]*jpsirho_pdf )");
  defineParamSet("jpsig_pdf");
  defineYieldSet("jpsig_pdf");
  w->pdf("jpsig_pdf")->SetTitle("PDF (J/#psi#gamma)");

}

void AnalysisFit::run(){

  if ( verbose || debug ) w->Print();

  fit("jpsig_sig_ll_pdf","MCJpsiGLL");
  plot("B0_MM","MCJpsiGLL","jpsig_sig_ll_pdf","LL MC Fit");
  freeze("jpsig_sig_ll_pdf");

  fit("jpsig_sig_dd_pdf","MCJpsiGDD");
  plot("B0_MM","MCJpsiGDD","jpsig_sig_dd_pdf","DD MC Fit");
  freeze("jpsig_sig_dd_pdf");

  fit("jpsipiz_pdf","MCJpsiPiz");
  plot("B0_MM","MCJpsiPiz","jpsipiz_pdf","MC Fit");
  freeze("jpsipiz_pdf");

  fit("jpsieta_pdf","MCJpsiEta");
  plot("B0_MM","MCJpsiEta","jpsieta_pdf","MC Fit");
  freeze("jpsieta_pdf");

  fit("jpsiks_pdf","MCJpsiKs");
  plot("B0_MM","MCJpsiKs","jpsiks_pdf","MC Fit");
  freeze("jpsiks_pdf");

  fit("jpsikst_pdf","MCJpsiKst");
  plot("B0_MM","MCJpsiKst","jpsikst_pdf","MC Fit");
  freeze("jpsikst_pdf");

  fit("jpsirho_pdf","MCJpsiRho");
  plot("B0_MM","MCJpsiRho","jpsirho_pdf","MC Fit");
  freeze("jpsirho_pdf");

  fit("jpsig_pdf","DataJpsiG");
  plot("B0_MM","DataJpsiG","jpsig_pdf","Data Fit");

  makeDataPlot();

}

void AnalysisFit::makeDataPlot(){

  vector<PlotComponent> plotComps;

  PlotComponent pc_data( "DataJpsiG", "Data" );
  pc_data.setDefaultDataStyle();
  pc_data.binning = 50;

  PlotComponent pc_pdf( "jpsig_pdf", "Total PDF" );
  pc_pdf.setSolidLine(kBlue);

  PlotComponent pc_pdf_bkg( "jpsig_pdf:jpsig_bkg_pdf", "Combinatorial" );
  pc_pdf_bkg.setDashedLine(kGreen+1);

  PlotComponent pc_pdf_sig( "jpsig_pdf:jpsig_sig_pdf", "Signal" );
  pc_pdf_sig.setDashedLine(kRed);

  PlotComponent pc_pdf_jpsipiz( "jpsig_pdf:jpsipiz_pdf", "J/#psi#pi^{0}" );
  pc_pdf_jpsipiz.setDashedLine(kMagenta);

  PlotComponent pc_pdf_jpsieta( "jpsig_pdf:jpsieta_pdf", "J/#psi#eta" );
  pc_pdf_jpsieta.setDashedLine(kGreen-7);

  PlotComponent pc_pdf_jpsiks( "jpsig_pdf:jpsiks_pdf", "J/#psiK_{S}" );
  pc_pdf_jpsiks.setDashedLine(kOrange+1);

  PlotComponent pc_pdf_jpsikst( "jpsig_pdf:jpsikst_pdf", "J/#psiK*^{0}" );
  pc_pdf_jpsikst.setDashedLine(kBlue-7);

  PlotComponent pc_pdf_jpsirho( "jpsig_pdf:jpsirho_pdf", "J/#psi#rho" );
  pc_pdf_jpsirho.setDashedLine(kYellow+1);

  plotComps.push_back(pc_data);
  plotComps.push_back(pc_pdf_bkg);
  plotComps.push_back(pc_pdf_jpsipiz);
  plotComps.push_back(pc_pdf_jpsieta);
  plotComps.push_back(pc_pdf_jpsiks);
  plotComps.push_back(pc_pdf_jpsikst);
  plotComps.push_back(pc_pdf_jpsirho);
  plotComps.push_back(pc_pdf_sig);
  plotComps.push_back(pc_pdf);

  w->var("jpsig_bkg_y")->SetTitle("N_{bkg}");
  w->var("jpsig_sig_y")->SetTitle("N_{sig}");
  w->var("jpsipiz_y")->SetTitle("N_{J#psi#pi^{0}}");
  w->var("jpsieta_y")->SetTitle("N_{J#psi#eta}");
  w->var("jpsiks_y")->SetTitle("N_{J#psiK_{S}}");
  w->var("jpsikst_y")->SetTitle("N_{J#psiK*^{0}}");
  w->var("jpsirho_y")->SetTitle("N_{J#psi#rho}");

  setPBoxX(0.35);

  plot("B0_MM", plotComps, "preselfit_kstar", w->set("jpsig_pdf_yield_params"));

  return;
}

