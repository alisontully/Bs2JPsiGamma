#ifndef AnalysisFit_h
#define AnalysisFit_h

#include "../interface/FitterBase.h"

class AnalysisFit : public FitterBase {

  public:

    AnalysisFit(TString wsname="w", TString name="AnalysisFit", bool verbose=false, bool debug=false);
    ~AnalysisFit();

    void addObsVars();
    void addCuts();
    void addDatasets();
    void constructPdfs();

    void run();

  private:

    void makeDataPlot();

};

#endif

