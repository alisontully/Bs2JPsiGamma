#!/usr/bin/env python

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-d","--datafile",default=[],action="append",help="File to dump branches from. Can parse multiple times. Use form filename:treename")
parser.add_option("-m","--mcfile",default=[],action="append",help="File to dump branches from. Can parse multiple times. Use form filename:treename")
(options,args) = parser.parse_args()

import ROOT as r

all_files = []
for f in options.datafile:
	all_files.append( [f.split(':')[0], f.split(':')[1], 1] )
for f in options.mcfile:
	all_files.append( [f.split(':')[0], f.split(':')[1], -1] )

varInfo = {}

def getInfoTree(tree,ityp):
	for leaf in tree.GetListOfLeaves():
		if leaf.GetName() in varInfo.keys():
			varInfo[leaf.GetName()] = [leaf.GetTypeName(),0]
		else:
			varInfo[leaf.GetName()] = [leaf.GetTypeName(),ityp]

def writeHeader(varKeys,varInfo):

	f = open('interface/Looper.h','w')
	f.write('/////////////////////////////////////                                           \n')
	f.write('//                                 //                                           \n')
	f.write('// Looper.h                        //                                           \n')
	f.write('// Author: Matthew Kenzie          //                                           \n')
	f.write('// Auto-generated                  //                                           \n')
	f.write('// Essentially a wrapper for TTree //                                           \n')
	f.write('//                                 //                                           \n')
	f.write('/////////////////////////////////////                                           \n')
	f.write('                                                                                \n')
	f.write('#ifndef Looper_h                                                                \n')
	f.write('#define Looper_h                                                                \n')
	f.write('                                                                                \n')
	f.write('#include <iostream>                                                             \n')
	f.write('#include <cassert>                                                              \n')
	f.write('#include "TChain.h"                                                             \n')
	f.write('#include "TTree.h"                                                              \n')
	f.write('#include "TBranch.h"                                                            \n')
	f.write('#include "TFile.h"                                                              \n')
	f.write('#include "TString.h"                                                            \n')
	f.write('                                                                                \n')
	f.write('#include "../interface/BranchDef.h"                                             \n')
	f.write('                                                                                \n')
	f.write('struct TreeContainer {                                                          \n')
	f.write('	TTree *tree;                                                                  \n')
	f.write('	TString name;                                                                 \n')
	f.write('	Long64_t nentries;                                                            \n')
	f.write('	int itype;                                                                    \n')
	f.write('	int sqrts;                                                                    \n')
	f.write('};                                                                              \n')
	f.write('                                                                                \n')
	f.write('class BranchDef; // just let Looper know that BranchDef exists                  \n')
	f.write('                                                                                \n')
	f.write('class Looper {                                                                  \n')
	f.write('                                                                                \n')
	f.write('	friend class BranchDef;                                                       \n')
	f.write('                                                                                \n')
	f.write('	public:                                                                       \n')
	f.write('                                                                                \n')
	f.write('		Looper(TTree *_outTree, BranchDef *_branchDefClass, TString _name="Looper");\n')
	f.write('		~Looper();                                                                  \n')
	f.write('                                                                                \n')
	f.write('		// functions                                                                \n')
	f.write('		void addTreeContainer(TTree *tree, TString _name, int _itype, int _sqrts);  \n')
	f.write('		void loadTree(int i);                                                       \n')
	f.write('		void setOutputBranches();                                                   \n')
	f.write('		inline void setFirstEntry(Long64_t ent) { firstEntry = ent; }               \n')
	f.write('		inline void setLastEntry(Long64_t ent) { lastEntry = ent; }                 \n')
	f.write('		Long64_t GetEntries() { return nentries; }                                  \n')
	f.write('		Int_t Fill() { return outTree->Fill(); }                                    \n')
	f.write('                                                                                \n')
	f.write('		// members                                                                  \n')
	f.write('		TTree *outTree;                                                             \n')
	f.write('		BranchDef *branchDefClass;                                                  \n')
	f.write('		TString name;                                                               \n')
	f.write('		int itype;                                                                  \n')
	f.write('		int sqrts;                                                                  \n')
	f.write('		Long64_t nentries;                                                          \n')
	f.write('		Long64_t nbytes;                                                            \n')
	f.write('		Long64_t firstEntry;                                                        \n')
	f.write('		Long64_t lastEntry;                                                         \n')
	f.write('		std::vector<TreeContainer> treeContainers;                                  \n')
	f.write('                                                                                \n')
	f.write('  	// branch variables\n')
	for key in varKeys:
		f.write('    %-15s %-40s\n'%(varInfo[key][0],'*'+key+';'))
	f.write('\n')
	f.write(' 	// branch definitions\n')
	for key in varKeys:
		f.write('    TBranch *b_%-40s\n'%(key+';'))
	f.write('\n')
	f.write('};\n')
	f.write('\n')
	f.write('#endif\n')
	f.close()

	print 'Written new file:', f.name

def writeSrc():

	f = open('src/Looper.cc','w')
	f.write('/////////////////////////////////////                                              \n')
	f.write('//                                 //                                              \n')
	f.write('// Looper.cc                       //                                              \n')
	f.write('// Author: Matthew Kenzie          //                                              \n')
	f.write('// Auto-generated                  //                                              \n')
	f.write('// Essentially a wrapper for TTree //                                              \n')
	f.write('//                                 //                                              \n')
	f.write('/////////////////////////////////////                                              \n')
	f.write('                                                                                   \n')
	f.write('#include "../interface/Looper.h"                                                   \n')
	f.write('                                                                                   \n')
	f.write('using namespace std;                                                               \n')
	f.write('                                                                                   \n')
	f.write('Looper::Looper(TTree *_outTree, BranchDef *_branchDefClass, TString _name):        \n')
	f.write('	outTree(_outTree),                                                               \n')
	f.write('	branchDefClass(_branchDefClass),                                                 \n')
	f.write('	name(_name),                                                                     \n')
	f.write('	nentries(0),                                                                     \n')
	f.write('	nbytes(0),                                                                       \n')
	f.write('	firstEntry(-1),                                                                  \n')
	f.write('	lastEntry(-1)                                                                    \n')
	f.write('{                                                                                  \n')
	f.write('	branchDefClass->initialiseVariables(this); 			                                 \n')
	f.write('	branchDefClass->setOutputBranches(this,outTree);                                 \n')
	f.write('}                                                                                  \n')
	f.write('                                                                                   \n')
	f.write('Looper::~Looper(){                                                                \n')
	f.write(' branchDefClass->cleanVariables(this);                                            \n')
	f.write('} 									                                                                \n')
	f.write('                                                                                   \n')
	f.write('void Looper::addTreeContainer(TTree *tree, TString _name, int _itype, int _sqrts) {\n')
	f.write('                                                                                   \n')
	f.write('	TreeContainer treeContainer;                                                     \n')
	f.write('	treeContainer.tree = tree;                                                       \n')
	f.write('	treeContainer.name = _name;                                                      \n')
	f.write('	treeContainer.nentries = tree->GetEntries();                                     \n')
	f.write('	treeContainer.itype = _itype;                                                    \n')
	f.write('	treeContainer.sqrts = _sqrts;                                                    \n')
	f.write('                                                                                   \n')
	f.write('	treeContainers.push_back(treeContainer);                                         \n')
	f.write('	nentries += treeContainer.nentries;                                              \n')
	f.write('}                                                                                  \n')
	f.write('                                                                                   \n')
	f.write('void Looper::loadTree(int i) {                \n')
	f.write('	assert(i>=0 && i<treeContainers.size());    \n')
	f.write('                                              \n')
	f.write('	TTree *tree = treeContainers[i].tree;       \n')
	f.write('	itype = treeContainers[i].itype;            \n')
	f.write('	sqrts = treeContainers[i].sqrts;            \n')
	f.write('                                              \n')
	f.write('	branchDefClass->setInputBranches(this,tree);\n')
	f.write('\n')
	f.write('}\n')
	f.write('\n')
	f.write('void Looper::setOutputBranches()                  \n')
	f.write('{                                                 \n')
	f.write('	branchDefClass->setOutputBranches(this,outTree);\n')
	f.write('}\n')
	f.close()

	print 'Written new file:', f.name

for f, t, typ in all_files:

	print 'Reading:', t, 'from', f, 'of type', typ
	assert( f.endswith('.root') )
	tf = r.TFile(f)
	tree = tf.Get(t)
	getInfoTree(tree,typ)

varKeys = varInfo.keys()
varKeys.sort()

writeHeader(varKeys,varInfo)
writeSrc()



