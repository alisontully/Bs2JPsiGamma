#ifndef PlotterBase_h
#define PlotterBase_h

#include <iostream>

#include "../interface/BaseAnalyser.h"
#include "../interface/Looper.h"
#include "../interface/PlotterUtils.h"

class PlotterBase : public BaseAnalyser {

  public:

    PlotterBase(TString _name);
    ~PlotterBase();

    void Init(Looper *l);
    void Term(Looper *l);
    bool AnalyseEvent(Looper *l);

    virtual bool fillHistograms(Looper *l) = 0;
    virtual void defineDrawingConfig() = 0;
    virtual void defineHistograms() = 0;


  protected:

    void addHist(TString name, TString title, int bins, float xlow, float xhigh, TString legPos="L");
    void addDrawOpt(TString name, TString title, int itype);
    void addDrawOpt(TString name, TString title, int itype1, int itype2);
    void addDrawOpt(TString name, TString title, int itype1, int itype2, int itype3);
    void addDrawOpt(TString name, TString title, int itype1, int itype2, int itype3, int itype4);

    void setDrawOptDefaultFill(int color);
    void setDrawOptDefaultLine(int color);
    void setDrawOptDefaultPoint(int color);

    void setDrawOptFill(int color, int style);
    void setDrawOptLine(int color, int style);
    void setDrawOptPoint(int color, int style);

    void addResidOpt(std::pair<int,int> opt);

    void fillHist(TString name, double val, Looper *l);
    void fillHist(TString name, float val,  Looper *l);
    void fillHist(TString name, int val,    Looper *l);
    void fillHist(TString name, bool val,   Looper *l);

    bool normalise;
    double normalisation;
    TString outfilename;

  private:
    // functions
    void initHistograms();
    void drawHistograms();
    void saveHistograms();
    void deleteHistograms();

    // data memebers
    std::vector<HistContainer> histList;
    std::vector<std::map<TString,TH1F*> > histMap;
    std::vector<DrawingConfig> drawOpts;
    std::vector<std::pair<int,int> > residOpts;
};

#endif
