#ifndef KstGammaFit_h
#define KstGammaFit_h

#include "../interface/FitterBase.h"

class KstGammaFit : public FitterBase {

  public:

    KstGammaFit(TString wsname="w", TString name="KstGammaFit",bool verbose=false, bool debug=false);
    ~KstGammaFit();

    void addObsVars();
    void addCuts();
    void addDatasets();
    void constructPdfs();

    void run();

  private:

    void makeDataPlotLL();
    void makeDataPlotDD();
    void makeDataPlot();



};

#endif
