#include "../interface/DataSet.h"

using namespace std;
using namespace RooFit;

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
