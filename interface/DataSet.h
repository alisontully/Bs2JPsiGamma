#ifndef DataSet_h
#define DataSet_h

#include <iostream>
#include <vector>
#include "TString.h"
#include "RooWorkspace.h"
#include "RooArgSet.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooAbsArg.h"

class DataSet {

  public:

    DataSet(TString _name, TString _title, RooWorkspace* _w);
    ~DataSet(){}

    // overloaded constructors
    DataSet(TString _name, TString _title, RooWorkspace* _w, int typ1);
    DataSet(TString _name, TString _title, RooWorkspace* _w, int typ1, int typ2);
    DataSet(TString _name, TString _title, RooWorkspace* _w, int typ1, int typ2, int typ3);
    DataSet(TString _name, TString _title, RooWorkspace* _w, int typ1, int typ2, int typ3, int typ4);

    TString name;
    TString title;
    std::vector<int> itypes;
    RooWorkspace *w;

    void addType(int type) { itypes.push_back(type); }
};

#endif
