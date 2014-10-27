##############################
## Help for LHCb Analysis   ##
##############################

Should have started by running LHCbAnalysisScripts/make_new_package.py

This will create a new package which some important core files in it.

From your new package directory you can do the following:

1.) Run ./dumpTreeBranches.py to dump all the tree branches into a file
2.) Run ./makeLooper.py to make all the relevant variables.
3.) Run ./makeBranchDef.py with cut down branch lists to then read in and write out subsets of the branches
4.) Write classes which inherit from interface/BaseAnalyser.h to do the analysis
5.) Write a Fitter.h class which can execute the fit afterwards
