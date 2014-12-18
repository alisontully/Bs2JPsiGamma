#!/usr/bin/env python
# vim: ts=2 sw=2 expandtab

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-d","--datfile",default="dat/config.dat",help="Configuration datfile. Default=%default")
parser.add_option("-o","--outfile",default="AnalysisOut.root",help="Name of output root file. Default=%default")
parser.add_option("-t","--treename",default="AnalysisTree",help="Name of output tree. Default=%default")
parser.add_option("-r","--runAsReduction",default=False,action="store_true")
parser.add_option("-q","--submitToQueue",help="Submit jobs to batch queue")
parser.add_option("-j","--jobDir",default="sub",help="Create jobs in this directory")
parser.add_option("-s","--splitJobs",type="int",help="Split up jobs to run this number of events per job")
parser.add_option("--dryRun",default=False,action="store_true",help="Don't actually run anything")
parser.add_option("--runLocal",default=False,action="store_true",help="When splitting jobs run them locally")
parser.add_option("-f","--firstEntry",type="int",help="First entry in tree to run")
parser.add_option("-l","--lastEntry",type="int",help="Last entry in tree to run")
parser.add_option("-m","--memoryRequest",type="int",help="Request this much local memory (in MB) at batch node (for PID calib use 40000)")
parser.add_option("-v","--verbose",default=False,action="store_true")
parser.add_option("-b","--batchMode",default=False,action="store_true",help="Run in batch mode")
(opts,args) = parser.parse_args()

import ROOT as r
r.gSystem.Load("lib/libAnalysis")
r.gROOT.SetBatch()

import sys

def getListOfInputFiles(datname):

  relfiles=[]
  f = open(datname)
  for line in f.readlines():
    if not line.startswith('itype'): continue
    els = line.split()
    if len(els)<3: continue
    fel = els[2]
    if not 'eoslhcb' in fel:
      relfiles.append(fel.split('=')[1])
  f.close()
  for root, dirs, files in os.walk('input'):
    for fil in files:
      relfiles.append(root+'/'+fil)
  return relfiles

def writeSubScript(name,datname,jobN=0):

	os.system('mkdir -p %s'%opts.jobDir)
	fname = '%s/%s/%s_j%d.sh'%(os.getcwd(),opts.jobDir,name,jobN)
	f = open(fname,'w')
	f.write('#!/bin/bash\n')
	f.write('touch %s.run\n'%f.name)
	f.write('rm -f %s.fail\n'%f.name)
	f.write('rm -f %s.done\n'%f.name)
	f.write('rm -f %s.log\n'%f.name)
	f.write('mkdir -p scratch\n')
	f.write('cd scratch\n')
	f.write('mkdir -p lib\n')
	f.write('mkdir -p python\n')
	f.write('mkdir -p dat\n')
	f.write('cd %s\n'%os.getcwd())
	if not opts.runLocal: f.write('source /afs/cern.ch/group/z5/group_login.sh\n')
	if not opts.runLocal: f.write('source setup_lxplus.sh\n')
	f.write('cd -\n')
	f.write('cp %s/lib/libAnalysis.so lib/\n'%os.getcwd())
	f.write('cp %s/runAnalysis.py .\n'%os.getcwd())
	f.write('cp %s/python/configProducer.py python/\n'%os.getcwd())
	f.write('cp %s/python/__init__.py python/\n'%os.getcwd())
	f.write('cp %s/%s dat/\n'%(os.getcwd(),datname))
	for fil in getListOfInputFiles(datname):
		fil_base = os.path.basename(fil)
		fil_dir = os.path.dirname(fil)
		f.write('mkdir -p %s\n'%fil_dir)
		if opts.runLocal:
			f.write('ln -s %s/%s/%s %s/%s\n'%(os.getcwd(),fil_dir,fil_base,fil_dir,fil_base))
		else:
			f.write('cp %s/%s/%s %s/\n'%(os.getcwd(),fil_dir,fil_base,fil_dir))
	exec_line = './runAnalysis.py -d dat/%s -o %s_Reduced_j%d.root -t ReducedTree -b'%(os.path.basename(datname),name,jobN)
	if opts.firstEntry: exec_line += ' -f %d'%opts.firstEntry
	if opts.lastEntry: exec_line += ' -l %d'%opts.lastEntry
	f.write('if ( %s ) then\n'%exec_line)
	f.write('\tcp %s_Reduced_j%d.root %s/%s/\n'%(name,jobN,os.getcwd(),opts.jobDir))
	f.write('\ttouch %s.done\n'%f.name)
	f.write('else\n')
	f.write('\ttouch %s.fail\n'%f.name)
	f.write('fi\n')
	f.write('rm -f %s.run\n'%f.name)
	f.close()
	os.system('chmod +x %s'%f.name)
	return f.name

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

def configureJobSplitting(fnames,tnames):

  if not opts.splitJobs:
    return { 0: (None,None) }

  nentries = 0
  for f, fname in enumerate(fnames):
    tf = r.TFile.Open(fname)
    tree = tf.Get(tnames[f])
    if not tree:
      sys.exit('Tree %s does not exist in file %s'%(tname,fname))

    nentries += tree.GetEntries()
    tf.Close()

  splittingOpts = {}

  nJobs = int(r.TMath.Ceil(float(nentries)/opts.splitJobs))

  for j in range(nJobs):
    firstEntry = opts.splitJobs*j
    lastEntry = opts.splitJobs*(j+1)
    if lastEntry > nentries:
      lastEntry = nentries
    splittingOpts[j] = (firstEntry,lastEntry)

  print 'Splitting file:', fname, 'into', len(splittingOpts.keys()), 'jobs'
  for key, item in splittingOpts.items():
    print  '\t', '[',item[0],'-',item[1], ']'

  return splittingOpts


# run to reduce tree size (will create one output file for each input file)
if opts.runAsReduction:

  import os

  os.system('mkdir -p tmp')
  os.system('mkdir -p root')
  preambe = []
  # split up the datfile into individual jobs based on the itype
  jobs_cfg = {}
  file_lines = []

  f = open(opts.datfile)
  for line in f.readlines():
    if line.startswith('itype'):
      file_lines.append(line)
      itype = int(line.split()[0].split('=')[1])
      if itype in jobs_cfg.keys():
        jobs_cfg[itype].append(line)
      else:
        jobs_cfg[itype] = [line]
    else:
      preambe.append(line)

  f.close()

  print '%-30s'%'runAnalysis.py', 'Running reduction jobs'
  stripped_name = os.path.splitext(os.path.basename(opts.datfile))[0]

  njobs=0

  for i, itype in enumerate(jobs_cfg.keys()):
    job_lines = jobs_cfg[itype]
    newdat = open('tmp/%s_%d.dat'%(stripped_name,i),'w')
    for l in preambe:
      newdat.write(l)
    for l in job_lines:
      newdat.write(l)
    newdat.close()

  for i, itype in enumerate(jobs_cfg.keys()):

    job_lines = jobs_cfg[itype]

    ####################################
    ## configure job splitting
    jobname  = ''
    fnames = []
    tnames = []
    for l in job_lines:
      for el in l.split():
        if el.startswith('name'):
          if jobname == '': jobname = el.split('=')[1]
          assert(jobname == el.split('=')[1])
        if el.startswith('fname'):
          fname = el.split('=')[1]
          fnames.append(fname)
        if el.startswith('tname'):
          tname = el.split('=')[1]
          tnames.append(tname)

    subjobSplittingOpts = configureJobSplitting(fnames,tnames)
    ####################################

    datname = 'tmp/%s_%d.dat'%(stripped_name,i)

    # loop subjobs
    for subjobN, subjobRange in subjobSplittingOpts.items():

      opts.firstEntry = subjobRange[0]
      opts.lastEntry = subjobRange[1]

      # write sub script
      subscript = writeSubScript(jobname,datname,subjobN)

      njobs += 1

      # execute action on sub script (dryrun, localrun, batchrun)
      if opts.submitToQueue:
        if not opts.dryRun:
          bsub_line = 'bsub -q %s -o %s.log'%(opts.submitToQueue,subscript)
          if opts.memoryRequest:
            bsub_line += ' -R "rusage[mem=%d]"'%(opts.memoryRequest)
          bsub_line += ' %s'%subscript
          print 'Submitting job:', subscript
          os.system(bsub_line)
      else:
        print 'Written sub script', subscript
        if opts.runLocal and not opts.dryRun:
          os.system('./%s'%subscript)
        else:
          exec_line = './runAnalysis.py -d %s -o root/%s_Reduced.root -t ReducedTree'%(datname,jobname)
          if opts.firstEntry: exec_line += ' -f %d'%opts.firstEntry
          if opts.lastEntry: exec_line += ' -l %d'%opts.lastEntry
          exec_line += ' 2>&1 | tee root/%s_reduction_out.log'%(jobname)
          if not opts.dryRun:
            os.system(exec_line)

  print 'TOTAL --', njobs,' jobs'

  # normal analysis below here (will merge all input files into a single output)
else:

	print '%-30s'%'runAnalysis.py', 'Starting analysis'
	sw = r.TStopwatch()
	sw.Start()

	from python.configProducer import *

	cfg_file = opts.datfile

	branchdefclass = getBranchDef()
	runner = r.Runner(opts.outfile,opts.treename,branchdefclass,"Runner")
	cfg = configProducer(runner,cfg_file,opts.verbose)

	if opts.firstEntry:
		runner.setFirstEntry(opts.firstEntry)
	if opts.lastEntry:
		runner.setLastEntry(opts.lastEntry)
	if opts.batchMode:
		runner.setBatchMode()
	runner.run()

	print '%-30s'%'runAnalysis.py', 'Analysis complete!!!'

	runner.save()

	print '%-30s'%'runAnalysis.py', 'Success!!!'

	sw.Stop()
	print '%-30s'%'runAnalysis.py', 'Took: %4.2f secs (real) %4.2f secs (CPU)'%(sw.RealTime(),sw.CpuTime())

	del runner
	del branchdefclass
