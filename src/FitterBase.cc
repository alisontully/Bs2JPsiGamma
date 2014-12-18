#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TLine.h"
#include "TGraphAsymmErrors.h"
#include "TMath.h"
#include "TPaveText.h"

#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooCBShape.h"
#include "RooAddPdf.h"

#include "RooMsgService.h"

#include "RooStats/SPlot.h"

#include "../interface/FitterBase.h"

using namespace std;
using namespace RooFit;
using namespace TMath;

FitterBase::FitterBase(TString wsname, TString name, bool _verbose, bool _debug):
  fitterName(name),
  verbose(_verbose),
  debug(_debug),
  pBoxX(0.22),
  pDrawLog(false),
  pTitle("")
{
  if ( !debug ) {
    RooMsgService::instance().setGlobalKillBelow(FATAL);
    RooMsgService::instance().setSilentMode(true);
  }
  gROOT->ProcessLine(".x /Users/matt/Scratch/lhcb/lhcbStyle.C");
	w = new RooWorkspace(wsname);
	RooArgSet *observables = new RooArgSet();
	w->defineSet("observables",*observables);
	delete observables;
  colors.push_back(kRed);
  colors.push_back(kBlue);
  colors.push_back(kGreen+1);
  system(Form("mkdir -p plots/%s/png",fitterName.Data()));
  system(Form("mkdir -p plots/%s/pdf",fitterName.Data()));
  system(Form("mkdir -p plots/%s/C",fitterName.Data()));
}

FitterBase::~FitterBase(){
  delete w;
}

void FitterBase::loadCachedWorkspace(TString fname){
  // get name of and then delete old workspace
  TString wsname = w->GetName();
  delete w;
  // now load the cached version in its place
  TFile *cacheFile = TFile::Open(fname);
  w = (RooWorkspace*)cacheFile->Get(wsname);
}

void FitterBase::addObsVar(TString name, double min, double max){
  addObsVar(name,name,"",min,max);
}

void FitterBase::addObsVar(TString name, TString title, TString unit, double min, double max){
	w->factory(Form("%s[%f,%f]",name.Data(),min,max));
	((RooArgSet*)w->set("observables"))->add(*w->var(name));
  w->var(name)->SetTitle(title);
  w->var(name)->setUnit(unit);
  if ( verbose || debug ) {
    print("Added observable "+name);
  }
}

void FitterBase::setUnit(TString var, TString unit){
  if (!w->var(var)) cerr << "WARNING -- FitterBase::setUnit() -- no variable named " << var << " in workspace" << endl;
  w->var(var)->setUnit(unit);
}

void FitterBase::setBins(TString var, int bins){
  if (!w->var(var)) cerr << "WARNING -- FitterBase::setBins() -- no variable named " << var << " in workspace" << endl;
  w->var(var)->setBins(bins);
}

void FitterBase::addDataset(TString name, TString title, int itype) {
  dataSets.push_back(DataSet(name,title,w,itype));
  if ( verbose || debug ) print("Added dataset "+name);
}

void FitterBase::addDataset(TString name, TString title, int itype1, int itype2) {
  dataSets.push_back(DataSet(name,title,w,itype1,itype2));
  if ( verbose || debug ) print("Added dataset "+name);
}

void FitterBase::addDataset(TString name, TString title, int itype1, int itype2, int itype3) {
  dataSets.push_back(DataSet(name,title,w,itype1,itype2,itype3));
  if ( verbose || debug ) print("Added dataset "+name);
}

void FitterBase::addDataset(TString name, TString title, int itype1, int itype2, int itype3, int itype4) {
  dataSets.push_back(DataSet(name,title,w,itype1,itype2,itype3,itype4));
  if ( verbose || debug ) print("Added dataset "+name);
}

void FitterBase::addCut(TString name, double low, double high){
  cut_value_d[name] = make_pair(low,high);
  values_d[name] = -999.;
  if ( verbose || debug ) print(Form("Added cut %s in range (%f,%f)",name.Data(),low,high));
}

void FitterBase::addCut(TString name, float low, float high){
  cut_value_f[name] = make_pair(low,high);
  values_f[name] = -999.;
  if ( verbose || debug ) print(Form("Added cut %s in range (%f,%f)",name.Data(),low,high));
}

void FitterBase::addCut(TString name, int low, int high){
  cut_value_i[name] = make_pair(low,high);
  values_i[name] = -999;
  if ( verbose || debug ) print(Form("Added cut %s in range (%d,%d)",name.Data(),low,high));
}

void FitterBase::addCut(TString name, bool val){
  cut_value_b[name] = val;
  values_b[name] = false;
  if ( verbose || debug ) val ? print("Added cut "+name+" is true") : print("Added cut "+name+" is false");
}

void FitterBase::addRequirement(TString dset, TString name, double low, double high){
  dataSets[dataSets.size()-1].addRequirement(name, low, high);
  values_d[name] = -999.;
  if ( verbose || debug ) print(Form("Added requirement %s in range (%f,%f) for dataset %s",name.Data(),low,high,dset.Data()));
}

void FitterBase::addRequirement(TString dset, TString name, float low, float high){
  dataSets[dataSets.size()-1].addRequirement(name, low, high);
  values_f[name] = -999.;
  if ( verbose || debug ) print(Form("Added requirement %s in range (%f,%f) for dataset %s",name.Data(),low,high,dset.Data()));
}

void FitterBase::addRequirement(TString dset, TString name, int low, int high){
  dataSets[dataSets.size()-1].addRequirement(name, low, high);
  values_i[name] = -999;
  if ( verbose || debug ) print(Form("Added requirement %s in range (%d,%d) for dataset %s",name.Data(),low,high,dset.Data()));
}

void FitterBase::addRequirement(TString dset, TString name, int val){
  dataSets[dataSets.size()-1].addRequirement(name, val, val);
  values_i[name] = -999;
  if ( verbose || debug ) print(Form("Added requirement %s == %d for dataset %s",name.Data(),val,dset.Data()));
}

void FitterBase::addRequirement(TString dset, TString name, bool val){
  dataSets[dataSets.size()-1].addRequirement(name, val);
  values_i[name] = false;
  if ( verbose || debug ) val ? print("Added requirement "+name+" is true for dataset "+dset) : print("Added cut "+name+" is false for dataset "+dset);
}

void FitterBase::makeDatasets() {

  for (vector<DataSet>::iterator dataIt=dataSets.begin(); dataIt!=dataSets.end(); dataIt++){
    RooDataSet *data = new RooDataSet(dataIt->name,dataIt->title,*w->set("observables"));
    w->import(*data);
    delete data;
  }
}

void FitterBase::fillDatasets(TString fname, TString tname){

  print("Filling datasets....");

	TFile *tf = TFile::Open(fname);
	TTree *tree = (TTree*)tf->Get(tname);

  RooArgSet *observables = (RooArgSet*)w->set("observables");

  // default variables to read
	int itype;
	tree->SetBranchAddress("itype",&itype);

  // set branch addresses from tree into container for observables
  RooRealVar *parg;
	TIterator *iter = observables->createIterator();
	while ((parg = (RooRealVar*)iter->Next())){
		obs_values[parg->GetName()] = -999.;
		tree->SetBranchAddress(parg->GetName(),&obs_values[parg->GetName()]);
	}
  delete iter;

  // set branch address for cut values and requirements
  setBranchAddresses(tree);

  // now loop entries
	for (int e=0; e<tree->GetEntries(); e++){
		tree->GetEntry(e);

    // print occasional entries in debug mode
    if ( debug && e%10000==0 ) {
      printEntry(e);
    }

    bool passes = true;

    // the variable cuts
    if ( !passesCuts() ) {
      continue;
    }

    // the observable range cuts
		iter = observables->createIterator();
		while ((parg = (RooRealVar*)iter->Next())) {
			if (obs_values[parg->GetName()] < w->var(parg->GetName())->getMin() || obs_values[parg->GetName()] > w->var(parg->GetName())->getMax()) {
				passes=false;
			}
			w->var(parg->GetName())->setVal(obs_values[parg->GetName()]);
		}

		if (!passes) continue;

    // dataset filling
    for (vector<DataSet>::iterator dataIt=dataSets.begin(); dataIt!=dataSets.end(); dataIt++){
      // check requirements
      if ( !passesRequirements(*dataIt) ) continue;
      // check itype
      for (vector<int>::iterator typIt=dataIt->itypes.begin(); typIt!=dataIt->itypes.end(); typIt++){
        if (itype==*typIt){
          w->data(dataIt->name)->add(*(w->set("observables")));
        }
      }
    }
  }
  tf->Close();
}

void FitterBase::save(TString fname){
  TFile *outf = new TFile(fname,"RECREATE");
  outf->cd();
  w->Write();
  TDirectory *dir = outf->mkdir("residuals");
  dir->cd();
  for (vector<TObject*>::iterator obj=saveObjsStore.begin(); obj!=saveObjsStore.end(); obj++){
    (*obj)->Write();
  }
  outf->Close();
  cout << "Written output to file: " << fname << endl;
}

void FitterBase::print(TString line, bool blank){
  TString lead = TString(Form("%-30s ","FitterBase()"));
  if (blank) lead = TString(Form("%-30s "," "));
  line = lead + line;
  cout << line << endl;
}

void FitterBase::defineParamSet(TString pdf){
  if ( ! w->pdf(pdf) ) {
    cout << "ERROR -- No valid pdf named " << pdf << " in workspace" << endl;
    exit(1);
  }
  RooArgSet *argset = w->pdf(pdf)->getParameters(*w->set("observables"));
  w->defineSet(Form("%s_params",pdf.Data()),*argset);
  delete argset;
}

void FitterBase::defineYieldSet(TString pdf_name){

  RooAbsPdf* pdf = w->pdf(pdf_name);
  RooArgSet *comps = pdf->getComponents();
  RooArgSet *allNonYieldParams = new RooArgSet();
  RooArgSet *nonYieldParams = new RooArgSet();
  // loop components
  RooAbsPdf *parg;
  TIterator *iter = comps->createIterator();
  while ( (parg = (RooAbsPdf*)iter->Next()) ) {
    if ( parg == w->pdf(pdf_name) ) continue;
    RooArgSet *argset = parg->getParameters(*w->set("observables"));
    nonYieldParams->add(*argset);
    allNonYieldParams->add(*argset);
    delete argset;
  }
  delete iter;
  allNonYieldParams->add(*w->set("observables"));
  RooArgSet *yieldParams = w->pdf(pdf_name)->getParameters(*allNonYieldParams);
  w->defineSet(Form("%s_yield_params",pdf_name.Data()),*yieldParams);
  w->defineSet(Form("%s_nonyield_params",pdf_name.Data()),*nonYieldParams);
  delete nonYieldParams;
  delete allNonYieldParams;
  delete yieldParams;
}

void FitterBase::saveSnapshot(TString name, TString pdf){

  RooArgSet *observables = (RooArgSet*)w->set("observables");
  RooArgSet *parameters = w->pdf(pdf)->getParameters(observables);
  w->saveSnapshot(name,*parameters);
  if ( verbose || debug ){
    print("Saved snapshot of pdf: "+pdf+" fit parameters to "+name);
  }
}

void FitterBase::loadSnapshot(TString name){
  w->loadSnapshot(name);
}

TCanvas* FitterBase::createCanvas(int canv_w, int canv_h){

  int top_x = canvs.size()*20;
  int top_y = canvs.size()*20;
  TString canvName = Form("c%d",int(canvs.size()));

  TCanvas *c = new TCanvas(canvName,canvName,top_x,top_y,canv_w,canv_h);
  canvs.push_back(c);
  return c;
}

void FitterBase::plot(TString var, TString data, TString pdf, TString title){

  w->pdf(pdf) ?
    print("Plotting data: "+data+" and pdf: "+pdf+" in variable: "+var) :
    print("Plotting data: "+data+" in variable: "+var) ;

  RooPlot *plot = w->var(var)->frame(Title(data));
  plot->GetXaxis()->SetTitleOffset(0.8);
  plot->GetYaxis()->SetTitleOffset(0.75);

  TPaveText *text = new TPaveText(0.15,0.8,0.4,0.9,"ndc");
  text->SetFillColor(0);
  text->SetShadowColor(0);
  text->SetLineColor(0);
  text->AddText(title);

  TLegend *leg = new TLegend(0.65,0.7,0.89,0.89);
  leg->SetFillColor(0);

  if (w->data(data)) {
    w->data(data)->plotOn(plot);
    TObject *obj = plot->getObject(plot->numItems()-1);
    leg->AddEntry(obj,w->data(data)->GetTitle(),"LEP");
  }
  if (w->pdf(pdf)) {
    w->pdf(pdf)->plotOn(plot);
    TObject *obj = plot->getObject(plot->numItems()-1);
    leg->AddEntry(obj,w->pdf(pdf)->GetTitle(),"L");
  }
  TCanvas *canv = createCanvas();
  canv->SetBottomMargin(0.2);
  plot->Draw();
  leg->Draw("same");
  if ( title != "" ) text->Draw("same");
  canv->Update();
  canv->Modified();
  canv->Print(Form("plots/%s/pdf/v%s_d%s_p%s.pdf",fitterName.Data(),var.Data(),data.Data(),pdf.Data()));
  canv->Print(Form("plots/%s/png/v%s_d%s_p%s.png",fitterName.Data(),var.Data(),data.Data(),pdf.Data()));
  canv->Print(Form("plots/%s/C/v%s_d%s_p%s.C",    fitterName.Data(),var.Data(),data.Data(),pdf.Data()));

  delete text;
  delete leg;
}

void FitterBase::plot(TString var, vector<PlotComponent> plotComps, TString fname, const RooArgSet *params) {

  print("Plotting following components in variable: "+var);
  for (unsigned int i=0; i<plotComps.size(); i++){
    print("\t "+plotComps[i].name,true);
  }

  TCanvas *canv = createCanvas();
  canv->SetLeftMargin(0.18);
  RooPlot *plot = w->var(var)->frame();
  TString xtitle = w->var(var)->GetTitle();
  if (TString(w->var(var)->getUnit())!=TString("")) xtitle = Form("%s (%s)",w->var(var)->GetTitle(),w->var(var)->getUnit());
  plot->GetXaxis()->SetTitle(xtitle);
  plot->GetXaxis()->SetTitleOffset(0.8);
  plot->GetYaxis()->SetTitleOffset(0.8);

  TLegend *leg;
  if ( plotComps.size()<7 ) {
    leg = new TLegend(0.6,0.6,0.9,0.9);
  }
  else {
    leg = new TLegend(0.6,0.5,0.9,0.9);
  }
  leg->SetFillColor(0);

  for (unsigned int i=0; i<plotComps.size(); i++){
    plotComps[i].plotOn(w,plot,leg);
  }

  plot->Draw();
  leg->Draw("same");

  // if requested plot title
  if ( pTitle != "" ) {
    TPaveText *text = new TPaveText(0.22,0.8,0.45,0.9,"ndc");
    text->SetFillColor(0);
    text->SetShadowColor(0);
    text->SetLineColor(0);
    text->AddText(pTitle);
    text->Draw("same");
  }

  // if requested also plot parameter values
  if (params) {
    double top = 0.9;
    if ( pTitle != "" ) {
      top = 0.8;
    }
    double bottom = top-(0.05*params->getSize());
    TPaveText *pNames = new TPaveText(pBoxX,bottom,pBoxX+0.08,top,"ndc");
    TPaveText *pVals = new TPaveText(pBoxX+0.08,bottom,pBoxX+0.2,top,"ndc");
    pNames->SetFillColor(0);
    pNames->SetShadowColor(0);
    pNames->SetLineColor(0);
    pNames->SetTextSize(0.03);
    pNames->SetTextAlign(11);
    pVals->SetFillColor(0);
    pVals->SetShadowColor(0);
    pVals->SetLineColor(0);
    pVals->SetTextSize(0.03);
    pVals->SetTextAlign(11);
    RooRealVar *parg;
    TIterator *iter = params->createIterator();
    while ((parg = (RooRealVar*)iter->Next())){
	    pNames->AddText(Form("%-10s",parg->GetTitle()));
      pVals->AddText(Form("= %4.2f #pm %4.2f",parg->getVal(),parg->getError()));
    }
    pNames->Draw("same");
    pVals->Draw("same");
  }

  canv->Update();
  canv->Modified();
  canv->Print(Form("plots/%s/pdf/%s.pdf",fitterName.Data(),fname.Data()));
  canv->Print(Form("plots/%s/png/%s.png",fitterName.Data(),fname.Data()));
  canv->Print(Form("plots/%s/C/%s.C",    fitterName.Data(),fname.Data()));

  if ( pDrawLog ) {
    TCanvas *canvLog = createCanvas();
    canvLog->SetLeftMargin(0.18);
    plot->GetYaxis()->SetRangeUser(1.,plot->GetMaximum()*2);
    plot->Draw();
    canvLog->SetLogy();
    canvLog->Update();
    canvLog->Modified();
    canvLog->Print(Form("plots/%s/pdf/%s_log.pdf",fitterName.Data(),fname.Data()));
    canvLog->Print(Form("plots/%s/png/%s_log.png",fitterName.Data(),fname.Data()));
    canvLog->Print(Form("plots/%s/C/%s_log.C",    fitterName.Data(),fname.Data()));
  }
}

void FitterBase::fit(TString pdf, TString data) {

  print("Fitting pdf: "+pdf+" to dataset: "+data);
  if (w->pdf(pdf) && w->data(data)) {
    w->pdf(pdf)->fitTo(*w->data(data));
  }
  saveSnapshot(Form("%s_fit",pdf.Data()),pdf);
  if ( verbose ) {
    print("Fitted values: ");
    w->set(Form("%s_params",pdf.Data()))->Print("v");
  }
}

void FitterBase::sfit(TString pdf_name, TString data_name) {

  // check pdf, data and yields exist
  RooAbsPdf *pdf    = w->pdf(pdf_name);
  if ( !pdf ) {
    cerr << "ERROR -- FitterBase::sfit() -- pdf name \'" << pdf_name << "\' is NULL" << endl;
  }
  RooDataSet *data  = (RooDataSet*)w->data(data_name);
  if ( !data ) {
    cerr << "ERROR -- FitterBase::sfit() -- data name \'" << data_name << "\' is NULL" << endl;
    exit(1);
  }
  RooArgSet *yields = (RooArgSet*)w->set(Form("%s_yield_params",pdf_name.Data()));
  if ( !yields ) {
    cerr << "ERROR -- FitterBase::sfit() -- yields name \'" << pdf_name << "_yield_params\' is NULL" << endl;
    exit(1);
  }
  RooArgSet *nonyields = (RooArgSet*)w->set(Form("%s_nonyield_params",pdf_name.Data()));
  if ( !yields ) {
    cerr << "ERROR -- FitterBase::sfit() -- nonyields name \'" << pdf_name << "_nonyield_params\' is NULL" << endl;
    exit(1);
  }

  print("Performing sfit of pdf: "+pdf_name+" to data: "+data_name);

  pdf->fitTo(*data, Extended());
  nonyields->setAttribAll("Constant");

  if ( verbose || debug ) {
    print("Will use these yields in splot:");
    debug ?
      yields->Print("v") :
      yields->Print()    ;
    print("Will not float these in splot:");
    debug ?
      nonyields->Print("v") :
      nonyields->Print()    ;
  }

  RooStats::SPlot *sData = new RooStats::SPlot(Form("%s_sfit",data->GetName()),Form("%s sfit",data->GetTitle()), *data, pdf, RooArgList(*yields));
  w->import(*sData);
  w->import(*data,Rename(Form("%s_wsweights",data->GetName())));
  print("Created SPlot with name "+TString(sData->GetName()));
  print("Renamed data with sweights to: "+data_name+"_wsweights");
  delete sData;
}

void FitterBase::sproject(TString data_name, TString var_name) {

  data_name = data_name+"_wsweights";
  RooDataSet *data = (RooDataSet*)w->data(data_name);
  if ( !data ) {
    cerr << "ERROR -- FitterBase::sproject() -- data name \'" << data_name << "\' is NULL" << endl;
    exit(1);
  }

  print("Projecting sweighted data with weight: "+var_name);

  RooDataSet *swdata = new RooDataSet(Form("%s_proj_%s",data->GetName(),var_name.Data()),Form("%s sweight proj %s",data->GetTitle(),var_name.Data()), data, *data->get(),0,var_name+"_sw");
  w->import(*swdata);
  delete swdata;
}

void FitterBase::freeze(TString pdf){
	if ( ! w->set(Form("%s_params",pdf.Data())) ) {
    cout << "ERROR -- No valid set named " << pdf << "_params in workspace" << endl;
    exit(1);
  }
  ((RooArgSet*)w->set(Form("%s_params",pdf.Data())))->setAttribAll("Constant");
  if ( verbose || debug ) {
    print("Frozen parameter values of pdf: "+pdf);
    if ( debug ) {
      w->set(Form("%s_params",pdf.Data()))->Print("v");
    }
  }
}

double FitterBase::integral(TString pdf, TString var, TString scale, double low, double high){

  double scaleValue = 1.;
  if ( w->var(scale) ) {
    scaleValue = w->var(scale)->getVal();
  }
  else if ( w->data(scale) ) {
    scaleValue = w->data(scale)->sumEntries();
  }
  else if ( scale=="") {
    scaleValue = 1.;
  }
  else {
    cout << "ERROR -- FitterBase::integral() -- nothing called " << scaleValue << " in workspace" << endl;
    exit(1);
  }

  bool hasRange = (low<-998 && high<-998) ? false : true ;
  if (hasRange){
    w->var(var)->setRange("IntegralRange",low,high);
  }

  RooAbsReal *integral;
  if (hasRange) {
    integral = w->pdf(pdf)->createIntegral(RooArgSet(*w->var(var)),NormSet(RooArgSet(*w->var(var))),Range("IntegralRange"));
  }
  else {
    integral = w->pdf(pdf)->createIntegral(RooArgSet(*w->var(var)),NormSet(RooArgSet(*w->var(var))));
  }
  double value = scaleValue*integral->getVal();
  delete integral;
  return value;
}

void FitterBase::splot(TString var, TString data, TString title, int bins){
  vector<TString> temp;
  splot(var, data, temp, title, bins);
}

void FitterBase::splot(TString var, TString data, vector<TString> compDsets, TString title, int bins) {

  if (!w->data(data)) {
    cerr << "ERROR -- FitterBase::splot() -- no sweighted data exists" << endl;
    exit(1);
  }
  if ( bins==-1 ){
    bins = w->var(var)->getBins();
  }
  TCanvas *canv = createCanvas();

  TPaveText *text = new TPaveText(0.15,0.8,0.4,0.9,"ndc");
  text->SetFillColor(0);
  text->SetShadowColor(0);
  text->SetLineColor(0);
  text->AddText(title);

  TLegend *leg = new TLegend(0.6,0.6,0.9,0.9);
  leg->SetFillColor(0);
  RooPlot *splot = w->var(var)->frame();
  splot->SetTitle(Form("%s projection",w->var(var)->GetTitle()));
  splot->GetXaxis()->SetTitleOffset(0.8);
  splot->GetYaxis()->SetTitleOffset(0.7);
  TString xtitle = w->var(var)->GetTitle();
  if (TString(w->var(var)->getUnit())!=TString("")) xtitle = Form("%s (%s)",w->var(var)->GetTitle(),w->var(var)->getUnit());
  splot->GetXaxis()->SetTitle(w->var(var)->GetTitle());

  w->data(data)->plotOn(splot,Binning(bins));
  RooHist *dh = (RooHist*)splot->getObject(splot->numItems()-1);
  leg->AddEntry(dh,w->data(data)->GetTitle(),"LEP");
  storeSPlotProjection(dh,Form("%s_v%s",data.Data(),var.Data()));

  vector<RooHist*> compHists;
  for (unsigned int i=0; i<compDsets.size(); i++){
    TString dset = compDsets[i];
    w->data(dset)->plotOn(splot,Binning(bins),MarkerColor(colors[i]),LineColor(colors[i]),Rescale(w->data(data)->sumEntries()/w->data(dset)->sumEntries()));
    RooHist *sh = (RooHist*)splot->getObject(splot->numItems()-1);
    leg->AddEntry(sh,w->data(dset)->GetTitle(),"LEP");
    compHists.push_back(sh);
    storeSPlotProjection(sh,Form("%s_v%s",dset.Data(),var.Data()));
    storeSPlotRatio(dh,sh,Form("resid_%s_o_%s_v%s",data.Data(),dset.Data(),var.Data()));
  }

  splot->Draw();
  leg->Draw("same");
  text->Draw("same");
  canv->Update();
  canv->Modified();
  canv->Print(Form("plots/%s/pdf/splot_v%s.pdf",fitterName.Data(),var.Data()));
  canv->Print(Form("plots/%s/png/splot_v%s.png",fitterName.Data(),var.Data()));
  canv->Print(Form("plots/%s/C/splot_v%s.C",    fitterName.Data(),var.Data()));

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
  canvResid->Print(Form("plots/%s/pdf/splot_resid_v%s.pdf",fitterName.Data(),var.Data()));
  canvResid->Print(Form("plots/%s/png/splot_resid_v%s.png",fitterName.Data(),var.Data()));
  canvResid->Print(Form("plots/%s/C/splot_resid_v%s.C",    fitterName.Data(),var.Data()));

}

void FitterBase::storeSPlotProjection(RooHist *rh, TString name){

  TGraphAsymmErrors *gr = new TGraphAsymmErrors();
  gr->SetName(name);
  double x,y;
  for (int p=0; p<rh->GetN(); p++){
    rh->GetPoint(p,x,y);
    gr->SetPoint(p,x,y);
    gr->SetPointError(p,rh->GetErrorXlow(p),rh->GetErrorXhigh(p),rh->GetErrorYlow(p),rh->GetErrorYhigh(p));
  }
  saveObjsStore.push_back(gr);

}

void FitterBase::storeSPlotRatio(RooHist *h1, RooHist *h2, TString name){

  if (h1->GetN()!=h2->GetN()){
    cerr << "ERROR -- FitterBase::storeSPlotRatio() -- two histograms do not have same number of points" << endl;
    exit(1);
  }

  TGraphAsymmErrors *gr = new TGraphAsymmErrors();
  gr->SetName(name);
  double x1, y1, x2, y2;
  for (int p=0; p<h1->GetN(); p++){
    h1->GetPoint(p,x1,y1);
    h2->GetPoint(p,x2,y2);
    assert(Abs(x1-x2)<0.001);
    double val = y1/y2;
    double y1errL = h1->GetErrorYlow(p);
    double y1errH = h1->GetErrorYhigh(p);
    double y2errL = h2->GetErrorYlow(p);
    double y2errH = h2->GetErrorYhigh(p);
    double errL = val * Sqrt( (y1errL/y1)*(y1errL/y1) + (y2errL/y2)*(y2errL/y2)  );
    double errH = val * Sqrt( (y1errH/y1)*(y1errH/y1) + (y2errH/y2)*(y2errH/y2)  );
    if (Abs(y1)<1.e-3 || Abs(y2)<1.e-3) {
      val = 1.;
      errL = 10.;
      errH = 10.;
    }
    gr->SetPoint(p,x1,val);
    gr->SetPointError(p,h1->GetErrorXlow(p),h1->GetErrorXhigh(p),errL,errH);
  }
  saveObjsStore.push_back(gr);
}

void FitterBase::setBranchAddresses(TTree *tree) {
  // doubles
  for (map<TString,double>::iterator it = values_d.begin(); it != values_d.end(); it++){
    //it->second = -999.;
    tree->SetBranchAddress(it->first, &it->second);
    if ( verbose || debug ) print("Added cut branch "+it->first);
  }
  // floats
  for (map<TString,float>::iterator it = values_f.begin(); it != values_f.end(); it++){
    //it->second = -999.;
    tree->SetBranchAddress(it->first, &it->second);
    if ( verbose || debug ) print("Added cut branch "+it->first);
  }
  // ints
  for (map<TString,int>::iterator it = values_i.begin(); it != values_i.end(); it++){
    //it->second = -999;
    tree->SetBranchAddress(it->first, &it->second);
    if ( verbose || debug ) print("Added cut branch "+it->first);
  }
  // bools
  for (map<TString,bool>::iterator it = values_b.begin(); it != values_b.end(); it++){
    //it->second = 0;
    tree->SetBranchAddress(it->first, &it->second);
    if ( verbose || debug ) print("Added cut branch "+it->first);
  }
}

bool FitterBase::passesCuts() {
  // doubles
  for (map<TString,pair<double,double> >::iterator it = cut_value_d.begin(); it != cut_value_d.end(); it++){
    if ( values_d[it->first] < it->second.first)  return false;
    if ( values_d[it->first] > it->second.second) return false;
  }
  // floats
  for (map<TString,pair<float,float> >::iterator it = cut_value_f.begin(); it != cut_value_f.end(); it++){
    if ( values_f[it->first] < it->second.first)  return false;
    if ( values_f[it->first] > it->second.second) return false;
  }
  // ints
  for (map<TString,pair<int,int> >::iterator it = cut_value_i.begin(); it != cut_value_i.end(); it++){
    if ( values_i[it->first] < it->second.first)  return false;
    if ( values_i[it->first] > it->second.second) return false;
  }
  // bools
  for (map<TString,bool>::iterator it = cut_value_b.begin(); it != cut_value_b.end(); it++){
    if ( values_b[it->first] != it->second)  return false;
  }
  return true;
}

bool FitterBase::passesRequirements(DataSet &dset){
  // doubles
  for (map<TString,pair<double,double> >::iterator it = dset.requirements_d.begin(); it != dset.requirements_d.end(); it++){
    if ( values_d[it->first] < it->second.first)  return false;
    if ( values_d[it->first] > it->second.second) return false;
  }
  // floats
  for (map<TString,pair<float,float> >::iterator it = dset.requirements_f.begin(); it != dset.requirements_f.end(); it++){
    if ( values_f[it->first] < it->second.first)  return false;
    if ( values_f[it->first] > it->second.second) return false;
  }
  // ints
  for (map<TString,pair<int,int> >::iterator it = dset.requirements_i.begin(); it != dset.requirements_i.end(); it++){
    if ( values_i[it->first] < it->second.first)  return false;
    if ( values_i[it->first] > it->second.second) return false;
  }
  // bools
  for (map<TString,bool>::iterator it = dset.requirements_b.begin(); it != dset.requirements_b.end(); it++){
    if ( values_b[it->first] != it->second)  return false;
  }
  return true;
}

void FitterBase::printEntry(int entry){

  print(Form("----- Printing contents of entry %-6d: -----",entry));
  print("   OBSERVABLES:",true);
  for (map<TString,double>::iterator val = obs_values.begin(); val != obs_values.end(); val++){
    print(Form(" \t %-20s %f",val->first.Data(),val->second),true);
  }
  print("   CUTS/REQUIREMENTS:",true);
  for (map<TString,double>::iterator val = values_d.begin(); val != values_d.end(); val++){
    print(Form(" \t %-20s %f",val->first.Data(),val->second),true);
  }
  for (map<TString,float>::iterator val = values_f.begin(); val != values_f.end(); val++){
    print(Form(" \t %-20s %f",val->first.Data(),val->second),true);
  }
  for (map<TString,int>::iterator val = values_i.begin(); val != values_i.end(); val++){
    print(Form(" \t %-20s %d",val->first.Data(),val->second),true);
  }
  for (map<TString,bool>::iterator val = values_b.begin(); val != values_b.end(); val++){
    print(Form(" \t %-20s %d",val->first.Data(),val->second),true);
  }
  print("----------------------------------------------",true);

}
