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

for f, t, typ in all_files:

	#print 'Reading:', t, 'from', f, 'of type', typ
	assert( f.endswith('.root') )
	tf = r.TFile(f)
	tree = tf.Get(t)
	getInfoTree(tree,typ)

varKeys = varInfo.keys()
varKeys.sort()

print '# auto-generated'
print '%-15s %-40s %5s'%('# type',' name',' itype (1=data, -1=mc, 0=both)')
for key in varKeys:
	print '%-15s %-40s %5d'%(varInfo[key][0],key,varInfo[key][1])

"""
print '\n'
for key in varKeys:
	print '\t\t%-15s %s;'%('TBranch','*b_'+key)

print '\n'
for key in varKeys:
	line = 'tree->SetBranchAddress("%s", &%s, b_%s);'%(key,key,key)
	if varInfo[key][1]<0:
		line = "if (itype<0) "+line
	if varInfo[key][1]>0:
		line = "if (itype>0) "+line
	print "\t"+line
"""

