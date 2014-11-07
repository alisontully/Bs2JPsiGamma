#ifndef Fitter_h
#define Fitter_h

#include "TString.h"
#include "RooWorkspace.h"
#include "../interface/DataSet.h"

class Fitter {

	public:

		Fitter(TString wsname="w");
		~Fitter();

    void addObsVar(TString name, double min, double max);
    void setUnit(TString var, TString unit);
    void setBins(TString var, int bins);

    void makeObsVars();
    void makeDatasets();
		void fillDatasets(TString fname, TString tname);

    void constructPdfs();
    void doKstarSplot();

    void plot();
    void fit();
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
		void fit(TString pdf, TString data);
    void freeze(TString pdf);

		float bdtcut;
		bool hasfit;

    std::vector<DataSet> dataSets;



};

#endif
