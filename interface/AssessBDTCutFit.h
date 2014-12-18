#ifndef AssessBDTCutFit_h
#define AssessBDTCutFit_h

#include "../interface/FitterBase.h"

class AssessBDTCutFit : public FitterBase {

  public:

    AssessBDTCutFit(TString wsname="w", TString name="AssessBDTCutFit", bool verbose=false, bool debug=false);
    ~AssessBDTCutFit();

    void addObsVars();
    void addCuts();
    void addDatasets();
    void constructPdfs();

    void run();

  private:

    void makeKstarPlot();
    void makeJpsiPlot();



};

#endif

