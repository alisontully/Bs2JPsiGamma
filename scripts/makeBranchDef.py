#!/usr/bin/env python

def printUsage():
	print '  -  This will make a new class which can be used to set which branches are read and written'
	print '     for your analysis.'
	print '  -  It will inherit from the BranchDef class'
	print '  -  All this will do is define the branches which should be read in and written out to file'
	print '  -  This saves time as you don\'t have to read every branch'

def readBranchesIn():

	f = open(opts.inputbranches)
	for line in f.readlines():
		if line.startswith('#'): continue
		if line=='' or line=='\n': continue
		els = line.split()
		if len(els)!=3: continue
		vtype = els[0]
		vname = els[1]
		itype = int(els[2])
		variableDict[vname] = [vtype, itype, -1] # -1 means read in

	f.close()

def readBranchesOut():

	f = open(opts.outputbranches)
	for line in f.readlines():
		if line.startswith('#'): continue
		if line=='' or line=='\n': continue
		els = line.split()
		if len(els)!=3: continue
		vtype = els[0]
		vname = els[1]
		itype = int(els[2])
		if vname in variableDict.keys():
			if vtype!=variableDict[vname][0]:
				sys.exit('ERROR -- Same branch ('+vname+') requested for in and out but with different types!')
			if itype!=variableDict[vname][1]:
				sys.exit('ERROR -- Same branch ('+vname+') requested for in and out but with different types!')
			variableDict[vname] = [vtype, itype, 0] # means read in AND write out
		else:
			variableDict[vname] = [vtype, itype, 1] # 1 means write out

	f.close()

def writeHeader():

	f = open(headername,'w')
	f.write('///////////////////////////////////////\n')
	f.write('// %-33s //\n'%os.path.basename(headername))
	f.write('// %-33s //\n'%'Author: Matthew Kenzie')
	f.write('// %-33s //\n'%'Auto-generated')
	f.write('///////////////////////////////////////\n')

	f.write('\n')
	f.write('#ifndef %s_h\n'%opts.classname)
	f.write('#define %s_h\n'%opts.classname)

	f.write('#include "../interface/BranchDef.h"\n')
	f.write('\n')
	f.write('class %s : public BranchDef {\n'%opts.classname)
	f.write('\n')
	f.write('	public:                                                                     \n')
	f.write('                                                                             \n')
	f.write('		%s(); 																                               			\n'%opts.classname)
	f.write('		~%s(); 		                                                							 	\n'%opts.classname)
	f.write('                                                                             \n')
	f.write('		virtual void initialiseVariables(Looper *l); 															\n')
	f.write('		virtual void cleanVariables(Looper *l); 							 										\n')
	f.write('		virtual void setInputBranches(Looper *l, TTree *tree); 										\n')
	f.write('		virtual void setOutputBranches(Looper *l, TTree *tree); 									\n')
	f.write('\n')
	f.write('\n')
	f.write('};\n')
	f.write('\n')
	f.write('#endif\n')

	f.close()

	print 'Written new file: ', f.name

def writeSrc():

	varKeys = variableDict.keys()
	varKeys.sort()

	f = open(srcname,'w')
	f.write('///////////////////////////////////////\n')
	f.write('// %-33s //\n'%os.path.basename(srcname))
	f.write('// %-33s //\n'%'Author: Matthew Kenzie')
	f.write('// %-33s //\n'%'Auto-generated')
	f.write('///////////////////////////////////////\n')

	f.write('\n')

	f.write('#include "../interface/%s.h"\n'%opts.classname)
	f.write('\n')

	# write contructor here
	f.write('%s::%s(){} 																	 \n'%(opts.classname,opts.classname))

	# write destructor here
	f.write('%s::~%s(){} 																	 \n'%(opts.classname,opts.classname))

	# write setInputBranches() here
	f.write('void %s::setInputBranches(Looper *l, TTree *tree){ \n'%opts.classname)
	for key in varKeys:
		if variableDict[key][2]>0: continue # write only
		line = 'tree->SetBranchAddress("%s", l->%s, &(l->b_%s));\n'%(key,key,key)
		if variableDict[key][1]<0: # this is MC only
			line = "if (l->itype<0) "+line
		if variableDict[key][1]>0: # this is data only
			line = "if (l->itype>0) "+line
		f.write("\t"+line)
	f.write('\n')
	f.write('}\n')
	f.write('\n')

	# write initialiseVariables() here
	f.write('void %s::initialiseVariables(Looper *l) {\n'%opts.classname)
	for key in varKeys:
		line = '%-40s = new %s(0);\n'%('l->'+key,variableDict[key][0])
		f.write('\t'+line)
	f.write('\n')
	f.write('}\n')
	f.write('\n')

	# write cleanVariables() here
	f.write('void %s::cleanVariables(Looper *l) {\n'%opts.classname)
	for key in varKeys:
		line = 'delete l->%s;\n'%key
		f.write('\t'+line)
	f.write('}\n')
	f.write('\n')

	# write setOutputBranches() here
	f.write('void %s::setOutputBranches(Looper *l, TTree *tree){ \n'%opts.classname)
	f.write('\ttree->Branch("itype",&(l->itype));\n')
	f.write('\ttree->Branch("sqrts",&(l->sqrts));\n')
	for key in varKeys:
		if variableDict[key][2]<0: continue # read only
		line = 'tree->Branch("%s",l->%s);\n'%(key,key)
		f.write('\t'+line)
	f.write('}\n')
	f.write('\n')

	f.close()

	print 'Written new file: ', f.name

def main():
	readBranchesIn()
	readBranchesOut()

	writeHeader()
	writeSrc()


if __name__ == "__main__":

	from optparse import OptionParser

	parser = OptionParser(usage=printUsage())
	parser.add_option("-i","--inputbranches")
	parser.add_option("-o","--outputbranches")
	parser.add_option("-c","--classname",default="NewLooper")
	(opts,args) = parser.parse_args()

	headername = 'interface/'+opts.classname+'.h'
	srcname = 'src/'+opts.classname+'.cc'

	import os
	import sys

	variableDict = {}

	main()
