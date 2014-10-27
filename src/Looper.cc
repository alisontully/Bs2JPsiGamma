/////////////////////////////////////                                              
//                                 //                                              
// Looper.cc                       //                                              
// Author: Matthew Kenzie          //                                              
// Auto-generated                  //                                              
// Essentially a wrapper for TTree //                                              
//                                 //                                              
/////////////////////////////////////                                              
                                                                                   
#include "../interface/Looper.h"                                                   
                                                                                   
using namespace std;                                                               
                                                                                   
Looper::Looper(TTree *_outTree, BranchDef *_branchDefClass, TString _name):        
	outTree(_outTree),                                                               
	branchDefClass(_branchDefClass),                                                 
	name(_name),                                                                     
	nentries(0),                                                                     
	nbytes(0),                                                                       
	firstEntry(-1),                                                                  
	lastEntry(-1)                                                                    
{                                                                                  
	branchDefClass->initialiseVariables(this); 			                                 
	branchDefClass->setOutputBranches(this,outTree);                                 
}                                                                                  
                                                                                   
Looper::~Looper(){                                                                
 branchDefClass->cleanVariables(this);                                            
} 									                                                                
                                                                                   
void Looper::addTreeContainer(TTree *tree, TString _name, int _itype, int _sqrts) {
                                                                                   
	TreeContainer treeContainer;                                                     
	treeContainer.tree = tree;                                                       
	treeContainer.name = _name;                                                      
	treeContainer.nentries = tree->GetEntries();                                     
	treeContainer.itype = _itype;                                                    
	treeContainer.sqrts = _sqrts;                                                    
                                                                                   
	treeContainers.push_back(treeContainer);                                         
	nentries += treeContainer.nentries;                                              
}                                                                                  
                                                                                   
void Looper::loadTree(int i) {                
	assert(i>=0 && i<treeContainers.size());    
                                              
	TTree *tree = treeContainers[i].tree;       
	itype = treeContainers[i].itype;            
	sqrts = treeContainers[i].sqrts;            
                                              
	branchDefClass->setInputBranches(this,tree);

}

void Looper::setOutputBranches()                  
{                                                 
	branchDefClass->setOutputBranches(this,outTree);
}
