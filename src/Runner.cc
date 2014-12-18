/////////////////////////////////////
//                                 //
// Runner.cc                       //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will run the analysis chain     //
//                                 //
/////////////////////////////////////

#include <fstream>
#include "TTimeStamp.h"

#include "../interface/Runner.h"

using namespace std;

Runner::Runner(TString outFileName, TString outTreeName, BranchDef *_branchDef, TString _name):
	name(_name),
	nentries(0),
	firstEntry(-1),
	lastEntry(-1),
	naccepted(0),
	nprocessed(0),
  batchmode(false)
{
	outFile = new TFile(outFileName,"RECREATE");
  outTree = new TTree(outTreeName,"Analysis Output Tree");
	// Large autosave number to avoid duplicate trees in outfile (this is probably a bit dumb)
	outTree->SetAutoSave(3e9);
  looper = new Looper(outTree,_branchDef,_name);
}

Runner::~Runner(){}

void Runner::save(){
	cout << Form("%-30s","Runner::run()") << " " << "Saving tree (" << outTree->GetName() << ") to file (" << outFile->GetName() << ")." << endl;
  outFile->cd();
	outFile->Write();
  hPass->SetDirectory(outFile);
  hPass->Write();
  hFail->SetDirectory(outFile);
  hFail->Write();
  hEff->SetDirectory(outFile);
  hEff->Write();
  outFile->Close();
  //delete looper;
  //delete outTree;
  //delete outFile;
}

void Runner::addLooperTree(TTree *tree, TString name, int itype, int sqrts) {
	looper->addTreeContainer(tree,name,itype,sqrts);
	Long64_t ents = tree->GetEntries();
	nentries += ents;
	vector<pair<int,int> > passFailInit;
	nPassFail.push_back(passFailInit);
	cout << Form("%-30s","Runner::addLooperTree()") << " " << "Added LooperTree (" << name.Data() << ") with " << ents << " entries." << endl;
}

void Runner::addAnalyser(BaseAnalyser *analyser) {
	analysers.push_back(analyser);
	// set counters to zero
	for (unsigned int t=0; t<looper->treeContainers.size(); t++){
		nPassFail[t].push_back(make_pair(0,0));
	}
	cout << Form("%-30s","Runner::addAnalyser()") << " " << "Added Analyser (" << analyser->name.Data() << ")." << endl;
}

void Runner::printProgressBar(Long64_t jentry, bool isDone) {
	double percentage = 100.*double(jentry-firstEntry)/double(lastEntry-firstEntry);
	TString prog = "[";
	for (int i=0; i<=100; i+=2) {
		if (percentage>(double(i)-0.001)) prog += "-";
		else prog += " ";
	}
	prog += "]";

	double time = timer.RealTime();
	timer.Continue();
	double timeperevent = time/double(jentry-firstEntry);
	double esttimeleft = timeperevent*double(lastEntry-jentry);

	if (isDone) percentage = 100.;
	TString summary = Form("%5.1f%% -- %6d/%-6d -- %6.2f ms/ev -- %10.0f secs left",percentage,int(jentry-firstEntry),int(lastEntry-firstEntry),timeperevent*1000.,esttimeleft);
	cout << Form("%-30s","Runner::run()") << " " << prog << " " << summary << "\r" << flush;
}

void Runner::run(){

	timer.Start();

	cout << Form("%-30s","Runner::run()") << " " << "Processing Looper (" << looper->name.Data() << ")." << endl;

	// Initialise Analysers
	for (unsigned int a=0; a<analysers.size(); a++){
		analysers[a]->Init(looper);
	}

	cout << Form("%-30s","Runner::run()") << " " << "Will run Analysers in the following order:" << endl;
	for (unsigned int a=0; a<analysers.size(); a++){
		cout << Form("%-30s","Runner::run()") << " " << "   " << a+1 << ".) " << analysers[a]->name << endl;
	}

	for (unsigned int t=0; t<looper->treeContainers.size(); t++) {

		Long64_t jentries = looper->treeContainers[t].nentries;
		cout << Form("%-30s","Runner::run()") << " " << "Loading tree (" << looper->treeContainers[t].name << ") with entries " << jentries << endl;
		looper->loadTree(t);

		int cachedFirstEntry = firstEntry;
		int cachedLastEntry = lastEntry;

		if (firstEntry<0) firstEntry=0;
		if (lastEntry<0) lastEntry=jentries;

    Long64_t run_entries = lastEntry - firstEntry;

		for (Long64_t jentry=firstEntry; jentry<lastEntry; jentry++){

			if (batchmode) {
				// print every 1%
				if (jentry%int(TMath::Ceil(run_entries/100.))==0) {
          cout << "\t" << "Entry " << jentry-firstEntry << " / " << run_entries << endl;
				}
			}
			else {
				// do progress bar update every 0.1%
				if (jentry%int(TMath::Ceil(run_entries/1000.))==0) {
          printProgressBar(jentry);
        }
			}
			looper->treeContainers[t].tree->GetEntry(jentry);
			nprocessed++;
			bool passesAll = true;
			for (unsigned a=0; a<analysers.size(); a++){
				if ( ! analysers[a]->AnalyseEvent(looper) ) {
					nPassFail[t][a].second++;
					passesAll = false;
					break; // can skip on if the event fails one analysis in the chain
				}
				nPassFail[t][a].first++;
			}
			if (passesAll) {
				naccepted++;
				looper->Fill();
			}
		}
		printProgressBar(run_entries,true);
		cout << endl;
		firstEntry = cachedFirstEntry;
		lastEntry = cachedLastEntry;
	}

	// Terminate Analysers
	for (unsigned int a=0; a<analysers.size(); a++){
		analysers[a]->Term(looper);
	}

  // Print cutflow to logfile
  ofstream outf("cutflow.log");
  TTimeStamp s;
  // Configure efficiency historam summary
  hPass = new TH2F("hPass","nEvents pass",analysers.size()+1,0,analysers.size()+1,looper->treeContainers.size(),0,looper->treeContainers.size());
  hFail = new TH2F("hFail","nEvents fail",analysers.size()+1,0,analysers.size()+1,looper->treeContainers.size(),0,looper->treeContainers.size());
  hEff = new TH2F("hEff","Efficiencies",analysers.size()+1,0,analysers.size()+1,looper->treeContainers.size(),0,looper->treeContainers.size());
  setXLabel(1,"Start");

	// Summarise results
	cout << Form("%-30s","Runner::run()") << " " << "Analysers cut flow summary:" << endl;
  outf << "Timestamp = " << s.AsString() << endl;
  outf << "Analysers cut flow summary:" << endl;
	for (unsigned int t=0; t<looper->treeContainers.size(); t++){
		cout << Form("%-30s","Runner::run()") << " " << "   " << looper->treeContainers[t].name << " : " << endl;
    outf << "   " << looper->treeContainers[t].name << " : " << endl;
    // set histogram labels
    setYLabel(t+1,looper->treeContainers[t].name);
    // start counters
    int totalpass=0;
    int totaltried=0;
		for (unsigned int a=0; a<analysers.size(); a++){

      // set histogram labels
      setXLabel(a+2,analysers[a]->name);

			// total tried will be those from first analyser
			if (a==0) {
        totaltried = nPassFail[t][a].first + nPassFail[t][a].second;
        setHistogramValues(0,t,totaltried,0);
      }
			// total passes will be those which passes last analyser
			if (a==analysers.size()-1) totalpass = nPassFail[t][a].first;

			// print efficiency of each analyser on each looper
			double eff = double(nPassFail[t][a].first)/double(nPassFail[t][a].first + nPassFail[t][a].second) * 100.;
			cout << Form("%-30s","Runner::run()") << " " << "      " << a+1 << ".) " << Form("%-15s",(analysers[a]->name+":").Data()) << "  " <<	nPassFail[t][a].first << "/" << nPassFail[t][a].first + nPassFail[t][a].second << " of events passed -- " << Form("%6.2f%%",eff) << " efficient" << endl;
      outf << "      " << a+1 << ".) " << Form("%-15s",(analysers[a]->name+":").Data()) << "  " <<	nPassFail[t][a].first << "/" << nPassFail[t][a].first + nPassFail[t][a].second << " of events passed -- " << Form("%6.2f%%",eff) << " efficient" << endl;

      // set values in histograms
      setHistogramValues(a+1,t,nPassFail[t][a].first,nPassFail[t][a].second);
		}

		// print efficiency for each looper
    cout << Form("%-30s","Runner::run()") << " " << "      " << Form("%-19s","TOTAL:") << " " << totalpass << "/" << totaltried << " of events passed -- " << Form("%6.2f%%",double(totalpass)/double(totaltried)*100.) << " efficient" << endl;
	}

	cout << Form("%-30s","Runner::run()") << " " << "Processing complete. Accepted " << naccepted << " / " << nprocessed << " events -- " << Form("%6.2f%%",100.*double(naccepted)/double(nprocessed)) << " efficient" << endl;
  outf << "Processing complete. Accepted " << naccepted << " / " << nentries << " events -- " << Form("%6.2f%%",100.*double(naccepted)/double(nentries)) << " efficient" << endl;

  outf.close();
}

void Runner::setXLabel(int bin, TString label){
  if (hPass) hPass->GetXaxis()->SetBinLabel(bin,label);
  if (hFail) hFail->GetXaxis()->SetBinLabel(bin,label);
  if (hEff)  hEff->GetXaxis()->SetBinLabel(bin,label);
}

void Runner::setYLabel(int bin, TString label){
  if (hPass) hPass->GetYaxis()->SetBinLabel(bin,label);
  if (hFail) hFail->GetYaxis()->SetBinLabel(bin,label);
  if (hEff)  hEff->GetYaxis()->SetBinLabel(bin,label);
}

void Runner::setHistogramValues(int xval, int yval, int nPass, int nFail) {

  int bin;
  double eff = double(nPass)/double(nPass+nFail);

  bin = hPass->FindBin(xval,yval);
  hPass->SetBinContent(bin,nPass);

  bin = hFail->FindBin(xval,yval);
  hFail->SetBinContent(bin,nFail);

  bin = hEff->FindBin(xval,yval);
  hEff->SetBinContent(bin,eff);

}
