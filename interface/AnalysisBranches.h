///////////////////////////////////////
// AnalysisBranches.h                //
// Author: Matthew Kenzie            //
// Auto-generated                    //
///////////////////////////////////////

#ifndef AnalysisBranches_h
#define AnalysisBranches_h
#include "../interface/BranchDef.h"

class AnalysisBranches : public BranchDef {

	public:                                                                     
                                                                             
		AnalysisBranches(); 																                               			
		~AnalysisBranches(); 		                                                							 	
                                                                             
		virtual void initialiseVariables(Looper *l); 															
		virtual void cleanVariables(Looper *l); 							 										
		virtual void setInputBranches(Looper *l, TTree *tree); 										
		virtual void setOutputBranches(Looper *l, TTree *tree); 									


};

#endif
