#include "../interface/KstGammaFit.h"

using namespace std;
using namespace RooFit;

KstGammaFit::KstGammaFit(TString wsname, bool verbose, bool debug):
  FitterBase(wsname,verbose,debug)
{}

KstGammaFit::~KstGammaFit(){}

void KstGammaFit::addObsVars(){

  addObsVar("B0_MM",         "m(K^{#pm}#pi^{#mp}#gamma)",  "MeV",    4500,   6000);
  addObsVar("gamgams_PT",    "p_{T}(#gamma)",  "MeV",    0,      20e3);
  addObsVar("B0_DIRA_OWNPV", "DIRA(B_{d})",    "",       0.99999,1.);
}

void KstGammaFit::addCuts(){
  addCut("bdtoutput",float(0.2),float(1.0));
}

void KstGammaFit::addDatasets(){

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

void KstGammaFit::constructPdfs(){

  w->factory("kstg_mean[5200,5300]");

  // Bd2KstGamma MC LL
  w->factory("kstg_ll_sigma[10,200]");
  w->factory("CBShape:kstg_ll_cb1(B0_MM,kstg_mean,kstg_ll_sigma,kstg_ll_alpha_1[0.,5],kstg_ll_n_1[0.,5.])");
  w->factory("CBShape:kstg_ll_cb2(B0_MM,kstg_mean,kstg_ll_sigma,kstg_ll_alpha_2[-5.,0],kstg_ll_n_2[0.,5.])");
  w->factory("SUM::kstg_sig_ll_pdf( kstg_ll_f_1[0.5,0.,1.]*kstg_ll_cb1, kstg_ll_cb2 )");
  defineParamSet("kstg_sig_ll_pdf");

  // Bd2KstGamma MC DD
  w->factory("kstg_dd_sigma[10,200]");
  w->factory("CBShape:kstg_dd_cb1(B0_MM,kstg_mean,kstg_dd_sigma,kstg_dd_alpha_1[0.,5.],kstg_dd_n_1[0.,50.])");
  w->factory("CBShape:kstg_dd_cb2(B0_MM,kstg_mean,kstg_dd_sigma,kstg_dd_alpha_2[-5.,0.],kstg_dd_n_2[0.,50.])");
  w->factory("SUM::kstg_sig_dd_pdf( kstg_dd_f_1[0.5,0.,1.]*kstg_dd_cb1, kstg_dd_cb2 )");
  defineParamSet("kstg_sig_dd_pdf");

  // Bd2KstGamma MC
  w->factory("SUM::kstg_sig_pdf( kstg_f_ll[0.5,0.,1.]*kstg_sig_ll_pdf, kstg_sig_dd_pdf )");
  defineParamSet("kstg_sig_pdf");

  // Bd2KstGamma Bkg
  w->factory("Exponential::kstg_bkg_pdf( B0_MM, kstg_exp_p1[-0.02,0.] )");
  defineParamSet("kstg_bkg_pdf");

  // Bd2KstGamma TOTAL
  w->factory("SUM::kstg_pdf( kstg_sig_y[0,2000]*kstg_sig_pdf, kstg_bkg_y[0,1000]*kstg_bkg_pdf )");
  defineParamSet("kstg_pdf");
  defineYieldSet("kstg_pdf");

  // Bd2KstGamma TOTAL LL
  w->factory("SUM::kstg_ll_pdf( kstg_sig_ll_y[0,2000]*kstg_sig_ll_pdf, kstg_bkg_ll_y[0,1000]*kstg_bkg_pdf )");
  defineParamSet("kstg_ll_pdf");

  // Bd2KstGamma TOTAL DD
  w->factory("SUM::kstg_dd_pdf( kstg_sig_dd_y[0,2000]*kstg_sig_dd_pdf, kstg_bkg_dd_y[0,1000]*kstg_bkg_pdf )");
  defineParamSet("kstg_dd_pdf");

}

void KstGammaFit::run(){

  if ( verbose || debug ) w->Print();

  fit("kstg_sig_ll_pdf","MCKstGLL");
  plot("B0_MM","MCKstGLL","kstg_sig_ll_pdf","LL MC Fit");
  freeze("kstg_sig_ll_pdf");
  w->var("kstg_mean")->setConstant(false);

  fit("kstg_sig_dd_pdf","MCKstGDD");
  plot("B0_MM","MCKstGDD","kstg_sig_dd_pdf","DD MC Fit");
  freeze("kstg_sig_dd_pdf");
  w->var("kstg_mean")->setConstant(false);

  fit("kstg_sig_pdf","MCKstG");
  plot("B0_MM","MCKstG","kstg_sig_pdf","MC Fit");
  freeze("kstg_sig_pdf");

  //w->var("kstg_f_ll")->setConstant(false);

  fit("kstg_ll_pdf","DataKstGLL");
  plot("B0_MM","DataKstGLL","kstg_ll_pdf","LL Data Fit");

  fit("kstg_dd_pdf","DataKstGDD");
  plot("B0_MM","DataKstGDD","kstg_dd_pdf","DD Data Fit");

  fit("kstg_pdf","DataKstG");
  plot("B0_MM","DataKstG","kstg_pdf","Data Fit");

  makeDataPlot();

  sfit("kstg_pdf","DataKstG");
  sproject("DataKstG","kstg_sig_y");

  vector<TString> compDsets;
  compDsets.push_back("MCKstG");

  splot("gamgams_PT",     "DataKstG_wsweights_proj_kstg_sig_y", compDsets, 10);
  splot("B0_DIRA_OWNPV",  "DataKstG_wsweights_proj_kstg_sig_y", compDsets, 10);
}

void KstGammaFit::makeDataPlot(){

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

  plot("B0_MM", plotComps, "fullfit");

  return;
}
