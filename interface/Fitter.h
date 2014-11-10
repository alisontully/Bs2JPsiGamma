#ifndef Fitter_h
#define Fitter_h

#include "TCanvas.h"
#include "TString.h"
#include "RooWorkspace.h"
#include "../interface/FitterUtils.h"

class Fitter {

	public:

		Fitter(TString wsname="w");
		~Fitter();

    void addObsVar(TString name, double min, double max);
    void addObsVar(TString name, TString title, TString unit, double min, double max);
    void setUnit(TString var, TString unit);
    void setBins(TString var, int bins);

    void makeObsVars();
    void makeDatasets();
		void fillDatasets(TString fname, TString tname);

    void constructPdfs();
    void doKstarSplot();

		void save(TString fname);

		void setBDTCut(double val);

		RooWorkspace *w;

	private:

		void constructSignalPdf();
		void constructPiZeroPdf();
		void constructJpsiXPdf();
		void constructBkgPdf();
		void constructFullPdf();
    void constructKstarGamMCPdf();
    void constructKstarDataPdf();

    void defineParamSet(TString pdf);

    void plot(TString var, TString data, TString pdf="");
    void plot(TString var, std::vector<PlotComponent> plotComps, TString fname);
		void fit(TString pdf, TString data);
    void splot(TString var, TString data);
    void splot(TString var, TString data, std::vector<TString> compDsets);
    void freeze(TString pdf);

		float bdtcut;

    std::vector<DataSet> dataSets;
    std::vector<int> colors;

    std::vector<TCanvas*> canvs;
    TCanvas* createCanvas();

};

#endif
