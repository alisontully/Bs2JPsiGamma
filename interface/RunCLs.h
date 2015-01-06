#ifndef RunCLs_h
#define RunCLs_h

#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "RooWorkspace.h"
#include "RooAbsData.h"

class RunCLs {

  public:

    RunCLs();
    ~RunCLs();

    void setup(TString infile, TString outfile);
    void save();
    void setSeed(int seed);
    void runData();
    void runToy();

    void plotCLs(TString fname);

    double getQValue(double hypothval, RooAbsData *dset);

  private:

    TFile *inFile;
    RooWorkspace *w;
    TFile *outFile;

    void setupTrees();

    std::vector<double> bf_hypoth_vals;

    std::vector<TTree*> resultTrees;

    // tree values
    int toyn;
    double hypothBF;
    double btoy_fitBF;
    double btoy_bestMinNll;
    double btoy_hypothMinNll;
    double btoy_teststat;
    double sbtoy_fitBF;
    double sbtoy_bestMinNll;
    double sbtoy_hypothMinNll;
    double sbtoy_teststat;

};

#endif
