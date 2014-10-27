/////////////////////////////////////
//                                 //
// Runner.cc                       //
// Author: Matthew Kenzie          //
// Auto-generated                  //
// Will run the analysis chain     //
//                                 //
/////////////////////////////////////

#include "../interface/Runner.h"

using namespace std;

Runner::Runner(TTree *_outTree, BranchDef *_branchDef, TString _name):
	name(_name),
	nentries(0),
	firstEntry(-1),
	lastEntry(-1),
	naccepted(0)
{
	looper = new Looper(_outTree,_branchDef,_name);
}

Runner::~Runner(){}

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

		for (Long64_t jentry=0; jentry<jentries; jentry++){

			if (jentry%int(TMath::Ceil(jentries/1000))==0) {
				printProgressBar(jentry);
			}
			looper->treeContainers[t].tree->GetEntry(jentry);
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
		printProgressBar(jentries,true);
		cout << endl;
		firstEntry = cachedFirstEntry;
		lastEntry = cachedLastEntry;
	}

	// Terminate Analysers
	for (unsigned int a=0; a<analysers.size(); a++){
		analysers[a]->Term(looper);
	}

	// Summarise results
	cout << Form("%-30s","Runner::run()") << " " << "Analysers cut flow summary:" << endl;
	for (unsigned int t=0; t<looper->treeContainers.size(); t++){
		cout << Form("%-30s","Runner::run()") << " " << "   " << looper->treeContainers[t].name << " : " << endl;
		for (unsigned int a=0; a<analysers.size(); a++){
			double eff = double(nPassFail[t][a].first)/double(nPassFail[t][a].first + nPassFail[t][a].second) * 100.;
			cout << Form("%-30s","Runner::run()") << " " << "      " << a+1 << ".) " << Form("%-15s",(analysers[a]->name+":").Data()) << "  " <<	nPassFail[t][a].first << "/" << nPassFail[t][a].first + nPassFail[t][a].second << " of events passed -- " << Form("%6.2f%%",eff) << " efficient" << endl;
		}
	}

	cout << Form("%-30s","Runner::run()") << " " << "Processing complete. Accepted " << naccepted << " / " << nentries << " events -- " << Form("%6.2f%%",100.*double(naccepted)/double(nentries)) << " efficient" << endl;
}

