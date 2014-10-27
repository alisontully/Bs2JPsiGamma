#ifndef BranchDef_h
#define BranchDef_h

#include "TTree.h"
#include "../interface/Looper.h"

class Looper; // just let BranchDef know that Looper exists

class BranchDef {

	friend class Looper;

	public:

		BranchDef();
		virtual ~BranchDef() = 0;

		virtual void initialiseVariables(Looper *l) = 0;
		virtual void cleanVariables(Looper *l) = 0;
		virtual void setInputBranches(Looper *l, TTree *tree) = 0;
		virtual void setOutputBranches(Looper *l, TTree *tree) = 0;

};

#endif
