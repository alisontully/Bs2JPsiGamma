#include "../interface/FitterUtils.h"

using namespace std;
using namespace RooFit;
using namespace TMath;

// constructors
DataSet::DataSet(TString _name, TString _title, RooWorkspace* _w):
  name(_name),
  title(_title),
  w(_w)
{}

DataSet::DataSet(TString _name, TString _title, RooWorkspace* _w, int typ1):
  name(_name),
  title(_title),
  w(_w)
{
  addType(typ1);
}

DataSet::DataSet(TString _name, TString _title, RooWorkspace* _w, int typ1, int typ2):
  name(_name),
  title(_title),
  w(_w)
{
  addType(typ1);
  addType(typ2);
}

DataSet::DataSet(TString _name, TString _title, RooWorkspace* _w, int typ1, int typ2, int typ3):
  name(_name),
  title(_title),
  w(_w)
{
  addType(typ1);
  addType(typ2);
  addType(typ3);
}

DataSet::DataSet(TString _name, TString _title, RooWorkspace* _w, int typ1, int typ2, int typ3, int typ4):
  name(_name),
  title(_title),
  w(_w)
{
  addType(typ1);
  addType(typ2);
  addType(typ3);
  addType(typ4);
}

PlotComponent::PlotComponent(TString _name, TString _title):
  name(_name),
  title(_title),
  lcolor(1),
  lstyle(1),
  lwidth(2),
  mcolor(1),
  mstyle(0),
  fcolor(0),
  fstyle(0),
  doption("L")
{}

PlotComponent::PlotComponent(TString _name, TString _title, int _lcolor):
  name(_name),
  title(_title),
  lcolor(_lcolor),
  lstyle(1),
  lwidth(2),
  mcolor(1),
  mstyle(0),
  fcolor(0),
  fstyle(0),
  doption("L")
{}

PlotComponent::PlotComponent(TString _name, TString _title, int _lcolor, int _lstyle):
  name(_name),
  title(_title),
  lcolor(_lcolor),
  lstyle(_lstyle),
  lwidth(2),
  mcolor(1),
  mstyle(0),
  fcolor(0),
  fstyle(0),
  doption("L")
{}

void PlotComponent::plotOn(RooWorkspace *w, RooPlot *plot, TLegend *leg){

  // check if component is requested (by passing of ":" in name)
  TString main = name;
  TString comp = name;

  if (name.Contains(":")) {
    main.Remove(main.First(":"),main.Length());
    comp.Remove(0,comp.First(":")+1);
  }

  // check if dset
  if (w->data(name)) {
    if (mstyle > 0) {
      w->data(name)->plotOn(plot,LineColor(lcolor),MarkerColor(mcolor),MarkerStyle(mstyle));
    }
    else {
      w->data(name)->plotOn(plot,LineColor(lcolor),MarkerColor(mcolor));
    }
  }

  // check if pdf
  else if (w->pdf(main)) {
    if (name.Contains(":")) {
      w->pdf(main)->plotOn(plot,Components(comp),LineColor(lcolor),LineStyle(lstyle),LineWidth(lwidth));
    }
    else {
      w->pdf(main)->plotOn(plot,LineColor(lcolor),LineStyle(lstyle),LineWidth(lwidth));
    }
  }

  // add to legend if necessary
  if (leg && (w->data(name) || w->pdf(main))) {
    TObject *legObj = (TObject*)plot->getObject(plot->numItems()-1);
    leg->AddEntry(legObj, title, doption);
  }
}

void PlotComponent::setDefaultDataStyle(){
  lcolor = kBlack;
  mcolor = kBlack;
  doption = "LEP";
  // the rest are already defaults
}

void PlotComponent::setDashedLine(int color){
  lcolor = color;
  lwidth = 3;
  lstyle = kDashed;
  doption = "L";
  // the rest are already defaults
}

void PlotComponent::setSolidLine(int color){
  lcolor = color;
  lwidth = 3;
  lstyle = 1;
  doption = "L";
  // the rest are already defaults
}

