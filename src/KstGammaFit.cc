#include "../interface/KstGammaFit.h"

using namespace std;
using namespace RooFit;

KstGammaFit::KstGammaFit(TString wsname, TString name, bool verbose, bool debug):
  FitterBase(wsname,name,verbose,debug)
{}

KstGammaFit::~KstGammaFit(){}

void KstGammaFit::addObsVars(){

  addObsVar("B0_MM",         "m(K^{#pm}#pi^{#mp}#gamma)",  "MeV",    4500,   6000);
  addObsVar("gamgams_PT",    "p_{T}(#gamma)",  "MeV",    0,      10e10);
  addObsVar("B0_DIRA_OWNPV", "DIRA(B_{d})",    "",       0.9999,1.);
  addObsVar("gamgams_eminus_StandaloneBremMultiplicityD","e^{-} Brem Multiplicity", "",0,4);
  addObsVar("gamgams_eplus_StandaloneBremMultiplicityD","e^{+} Brem Multiplicity","",0,4);
}

void KstGammaFit::addCuts(){
  //addCut("bdtoutput",float(0.2),float(1.0));
  addCut("pass_bdt",true);
  addCut("Kplus_PIDK", double(3.), double(999.));
  addCut("piminus_PIDK",double(-999.), double(-3.));
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

  addDataset("MCKstG2011",    "MC (K*#gamma)",   -89);

  addDataset("MCKstGLL2011",  "MC (K*#gamma) LL",-89);
  addRequirement("MCKstGLL2011","eplus_TRACK_Type",          3);
  addRequirement("MCKstGLL2011","eminus_TRACK_Type",         3);

  addDataset("MCKstGDD2011",  "MC (K*#gamma) DD",-89);
  addRequirement("MCKstGDD2011","eplus_TRACK_Type",          5);
  addRequirement("MCKstGDD2011","eminus_TRACK_Type",         5);

  addDataset("MCKstGSim06",    "MC (K*#gamma)",   -90);

  addDataset("MCKstGLLSim06",  "MC (K*#gamma) LL",-90);
  addRequirement("MCKstGLLSim06","eplus_TRACK_Type",          3);
  addRequirement("MCKstGLLSim06","eminus_TRACK_Type",         3);

  addDataset("MCKstGDDSim06",  "MC (K*#gamma) DD",-90);
  addRequirement("MCKstGDDSim06","eplus_TRACK_Type",          5);
  addRequirement("MCKstGDDSim06","eminus_TRACK_Type",         5);

}

void KstGammaFit::constructPdfs(){


  // Bd2KstGamma MC LL
  w->factory("kstg_ll_mean[5200,5300]");
  w->factory("kstg_ll_sigma[10,200]");
  w->factory("CBShape:kstg_ll_cb1(B0_MM,kstg_ll_mean,kstg_ll_sigma,kstg_ll_alpha_1[0.,5],kstg_ll_n_1[0.,5.])");
  w->factory("CBShape:kstg_ll_cb2(B0_MM,kstg_ll_mean,kstg_ll_sigma,kstg_ll_alpha_2[-5.,0],kstg_ll_n_2[0.,5.])");
  w->factory("SUM::kstg_sig_ll_pdf( kstg_ll_f_1[0.5,0.,1.]*kstg_ll_cb1, kstg_ll_cb2 )");
  defineParamSet("kstg_sig_ll_pdf");

  // Bd2KstGamma MC DD
  w->factory("kstg_dd_mean[5200,5300]");
  w->factory("kstg_dd_sigma[10,200]");
  w->factory("CBShape:kstg_dd_cb1(B0_MM,kstg_dd_mean,kstg_dd_sigma,kstg_dd_alpha_1[0.,5.],kstg_dd_n_1[0.,50.])");
  w->factory("CBShape:kstg_dd_cb2(B0_MM,kstg_dd_mean,kstg_dd_sigma,kstg_dd_alpha_2[-5.,0.],kstg_dd_n_2[0.,50.])");
  w->factory("SUM::kstg_sig_dd_pdf( kstg_dd_f_1[0.5,0.,1.]*kstg_dd_cb1, kstg_dd_cb2 )");
  defineParamSet("kstg_sig_dd_pdf");

  // Bd2KstGamma Bkg LL
  w->factory("Exponential::kstg_bkg_ll_pdf( B0_MM, kstg_ll_exp_p1[-0.02,0.] )");
  defineParamSet("kstg_bkg_ll_pdf");

  // Bd2KstGamma Bkg DD
  w->factory("Exponential::kstg_bkg_dd_pdf( B0_MM, kstg_dd_exp_p1[-0.02,0.] )");
  defineParamSet("kstg_bkg_dd_pdf");

  // Bd2KstGamma TOTAL LL
  w->factory("SUM::kstg_ll_pdf( kstg_sig_ll_y[0,10000]*kstg_sig_ll_pdf, kstg_bkg_ll_y[0,10000]*kstg_bkg_ll_pdf )");
  defineParamSet("kstg_ll_pdf");
  defineYieldSet("kstg_ll_pdf");

  // Bd2KstGamma TOTAL DD
  w->factory("SUM::kstg_dd_pdf( kstg_sig_dd_y[0,10000]*kstg_sig_dd_pdf, kstg_bkg_dd_y[0,10000]*kstg_bkg_dd_pdf )");
  defineParamSet("kstg_dd_pdf");
  defineYieldSet("kstg_dd_pdf");

}

void KstGammaFit::run(){

  if ( verbose || debug ) w->Print();

  // fit LL sig MC
  fit("kstg_sig_ll_pdf","MCKstGLL");
  plot("B0_MM","MCKstGLL","kstg_sig_ll_pdf","LL MC Fit");
  //w->var("kstg_ll_mean")->setConstant(true);
  //freeze("kstg_sig_ll_pdf");
  //w->var("kstg_ll_sigma")->setConstant(false);

  // fit DD sig MC
  fit("kstg_sig_dd_pdf","MCKstGDD");
  plot("B0_MM","MCKstGDD","kstg_sig_dd_pdf","DD MC Fit");
  //w->var("kstg_dd_mean")->setConstant(true);
  //freeze("kstg_sig_dd_pdf");
  //w->var("kstg_dd_sigma")->setConstant(false);

  // fit LL data
  fit("kstg_ll_pdf","DataKstGLL");
  plot("B0_MM","DataKstGLL","kstg_ll_pdf","LL Data Fit");

  // fit DD data
  fit("kstg_dd_pdf","DataKstGDD");
  plot("B0_MM","DataKstGDD","kstg_dd_pdf","DD Data Fit");

  // plot LL data
  makeDataPlotLL();
  // plot DD data
  makeDataPlotDD();

  cout << "-------- Dataset Entries -------" << endl;
  cout << "   KstG:   " << w->data("DataKstG")->numEntries() << endl;
  cout << "   KstGLL: " << w->data("DataKstGLL")->numEntries() << endl;
  cout << "   KstGDD: " << w->data("DataKstGDD")->numEntries() << endl;
  cout << "--------------------------------" << endl;

  // splot LL
  sfit("kstg_ll_pdf","DataKstGLL");
  sproject("DataKstGLL","kstg_sig_ll_y");

  vector<TString> compDsetsLL;
  compDsetsLL.push_back("MCKstGLL");

  // splot DD
  sfit("kstg_dd_pdf","DataKstGDD");
  sproject("DataKstGDD","kstg_sig_dd_y");

  vector<TString> compDsetsDD;
  compDsetsDD.push_back("MCKstGDD");

  w->var("gamgams_PT")->setRange(0,20e3);

  w->data("DataKstGLL_wsweights_proj_kstg_sig_ll_y")->SetTitle("Data (K*#gamma) DD sweighted");
  w->data("DataKstGDD_wsweights_proj_kstg_sig_dd_y")->SetTitle("Data (K*#gamma) DD sweighted");

  splot("gamgams_PT",     "DataKstGLL_wsweights_proj_kstg_sig_ll_y", compDsetsLL, "LL sWeights", 10, "_ll");
  splot("B0_DIRA_OWNPV",  "DataKstGLL_wsweights_proj_kstg_sig_ll_y", compDsetsLL, "LL sWeights", 10, "_ll");

  splot("gamgams_PT",     "DataKstGDD_wsweights_proj_kstg_sig_dd_y", compDsetsDD, "DD sWeights", 10, "_dd");
  splot("B0_DIRA_OWNPV",  "DataKstGDD_wsweights_proj_kstg_sig_dd_y", compDsetsDD, "DD sWeights", 10, "_dd");

  splot("gamgams_eplus_StandaloneBremMultiplicityD",     "DataKstGLL_wsweights_proj_kstg_sig_ll_y", compDsetsLL, "LL sWeights", 4, "_ll");
  splot("gamgams_eminus_StandaloneBremMultiplicityD",    "DataKstGLL_wsweights_proj_kstg_sig_ll_y", compDsetsLL, "LL sWeights", 4, "_ll");

  splot("gamgams_eplus_StandaloneBremMultiplicityD",     "DataKstGDD_wsweights_proj_kstg_sig_dd_y", compDsetsDD, "DD sWeights", 4, "_dd");
  splot("gamgams_eminus_StandaloneBremMultiplicityD",    "DataKstGDD_wsweights_proj_kstg_sig_dd_y", compDsetsDD, "DD sWeights", 4, "_dd");
}

void KstGammaFit::makeDataPlotLL(){

  vector<PlotComponent> plotComps;

  PlotComponent pc_data( "DataKstGLL", "Data (LL)");
  pc_data.setDefaultDataStyle();

  PlotComponent pc_pdf( "kstg_ll_pdf", "Total PDF (LL)");
  pc_pdf.setSolidLine(kBlue);

  PlotComponent pc_pdf_bkg( "kstg_ll_pdf:kstg_bkg_ll_pdf", "Background (LL)");
  pc_pdf_bkg.setDashedLine(kGreen+1);

  PlotComponent pc_pdf_sig( "kstg_ll_pdf:kstg_sig_ll_pdf", "Signal (LL)");
  pc_pdf_sig.setDashedLine(kRed);

  plotComps.push_back(pc_data);
  plotComps.push_back(pc_pdf_bkg);
  plotComps.push_back(pc_pdf_sig);
  plotComps.push_back(pc_pdf);

  w->var("kstg_sig_ll_y")->SetTitle("N_{s}");
  w->var("kstg_bkg_ll_y")->SetTitle("N_{b}");

  RooArgSet *argset = new RooArgSet();
  argset->add(*w->var("kstg_sig_ll_y"));
  argset->add(*w->var("kstg_bkg_ll_y"));

  setDrawLog(false);
  setTitle("LL Data Fit");

  plot("B0_MM", plotComps, "kstg_ll_fit", argset);

  delete argset;
}

void KstGammaFit::makeDataPlotDD(){

  vector<PlotComponent> plotComps;

  PlotComponent pc_data( "DataKstGDD", "Data (DD)");
  pc_data.setDefaultDataStyle();

  PlotComponent pc_pdf( "kstg_dd_pdf", "Total PDF (DD)");
  pc_pdf.setSolidLine(kBlue);

  PlotComponent pc_pdf_bkg( "kstg_dd_pdf:kstg_bkg_dd_pdf", "Background (DD)");
  pc_pdf_bkg.setDashedLine(kGreen+1);

  PlotComponent pc_pdf_sig( "kstg_dd_pdf:kstg_sig_dd_pdf", "Signal (DD)");
  pc_pdf_sig.setDashedLine(kRed);

  plotComps.push_back(pc_data);
  plotComps.push_back(pc_pdf_bkg);
  plotComps.push_back(pc_pdf_sig);
  plotComps.push_back(pc_pdf);

  w->var("kstg_sig_dd_y")->SetTitle("N_{s}");
  w->var("kstg_bkg_dd_y")->SetTitle("N_{b}");

  RooArgSet *argset = new RooArgSet();
  argset->add(*w->var("kstg_sig_dd_y"));
  argset->add(*w->var("kstg_bkg_dd_y"));

  setDrawLog(false);
  setTitle("DD Data Fit");

  plot("B0_MM", plotComps, "kstg_dd_fit", argset);

  delete argset;
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
