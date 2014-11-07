#!/usr/bin/env python

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-d","--datfile",default="dat/config.dat",help="Configuration datfile. Default=%default")
parser.add_option("-o","--outfile",default="AnalysisOut.root",help="Name of output root file. Default=%default")
parser.add_option("-t","--treename",default="AnalysisTree",help="Name of output tree. Default=%default")
parser.add_option("-r","--runAsReduction",default=False,action="store_true")
parser.add_option("--dryRun",default=False,action="store_true",help="Don't actually run anything")
parser.add_option("-v","--verbose",default=False,action="store_true")
parser.add_option("-f","--firstEvent",type="int")
parser.add_option("-l","--lastEvent",type="int")
(opts,args) = parser.parse_args()

import ROOT as r
r.gSystem.Load("lib/libAnalysis")
r.gROOT.SetBatch()

def getBranchDef():

	import sys
	f = open(opts.datfile)
	found = False
	for line in f.readlines():
		line = line.strip('\n')
		if line.startswith('branchdef'):
			branchdef = line.split('=')[1]
			found = True
	f.close()

	if found:
		return getattr(r,branchdef)()
	else:
		sys.exit('No valid branchdef found in datfile %s'%f.name)

# run to reduce tree size (will create one output file for each input file)
if opts.runAsReduction:

	import os

	os.system('mkdir -p tmp')
	os.system('mkdir -p root')
	preambe = []
	file_lines = []

	f = open(opts.datfile)
	for line in f.readlines():
		if line.startswith('itype'):
			file_lines.append(line)
		else:
			preambe.append(line)

	f.close()

	print '%-30s'%'runAnalysis.py', 'Running reduction jobs'
	stripped_name = os.path.splitext(os.path.basename(opts.datfile))[0]
	for i, line in enumerate(file_lines):
		newdat = open('tmp/%s_%d.dat'%(stripped_name,i),'w')
		for l in preambe:
			newdat.write(l)
		newdat.write(line)
		newdat.close

	for i, line in enumerate(file_lines):
		datname = 'tmp/%s_%d.dat'%(stripped_name,i)
		rootname = ''
		for el in line.split():
			if el.startswith('name'):
				rootname = el.split('=')[1]


		exec_line = './runAnalysis.py -d %s -o root/%s_Reduced.root -t ReducedTree 2>&1 | tee root/%s_reduction_out.log'%(datname,rootname,rootname)
		if not opts.dryRun:
			print '%-30s'%'runAnalysis.py', 'Running Reduction Job %d/%d'%(i+1,len(file_lines))
			os.system(exec_line)
		else:
			print exec_line

# normal analysis below here (will merge all input files into a single output)
else:

	print '%-30s'%'runAnalysis.py', 'Starting analysis'
	sw = r.TStopwatch()
	sw.Start()

	from python.configProducer import *

	cfg_file = opts.datfile

	outt = r.TTree(opts.treename,"Analysis Output Tree")

	branchdefclass = getBranchDef()
	runner = r.Runner(outt,branchdefclass,"Runner")
	cfg = configProducer(runner,cfg_file,opts.verbose)

	if opts.firstEvent: runner.setFirstEntry(opts.firstEvent)
	if opts.lastEvent: runner.setLastEntry(opts.lastEvent)

	runner.run()

	print '%-30s'%'runAnalysis.py', 'Analysis complete!!!'
	print '%-30s'%'runAnalysis.py', 'Writing tree', outt.GetName(), 'to output file:', opts.outfile
	outf = r.TFile(opts.outfile,"RECREATE")
	outf.cd()
	outt.SetDirectory(outf)
	#outf.Write()
	outt.Write()
	outf.Close()

	print '%-30s'%'runAnalysis.py', 'Success!!!'

	sw.Stop()
	print '%-30s'%'runAnalysis.py', 'Took: %4.2f secs (real) %4.2f secs (CPU)'%(sw.RealTime(),sw.CpuTime())
