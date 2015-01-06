#!/usr/bin/env python

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-i","--infile",default="model_ws.root",help="Name of input root file. Default=%default")
parser.add_option("-o","--outfile",default="CLsOut.root",help="Name of output file. Default=%default")
parser.add_option("-t","--ntoys",default=-1,type="int",help="Number of toys to run")
parser.add_option("-j","--njobs",default=-1,type="int",help="Number of different jobs to run")
parser.add_option("-p","--plot",default=False,action="store_true",help="Make plot")
parser.add_option("-q","--queue",help="Batch queue to submit to")
(opts,args) = parser.parse_args()
import ROOT as r
r.gSystem.Load("lib/libAnalysis")

if opts.njobs<0:

  runner = r.RunCLs()
  if opts.ntoys>0:
    runner.setup(opts.infile, opts.outfile)
    runner.runData()
    for i in range(opts.ntoys):
      runner.runToy()
    runner.save()

  if opts.plot:
    runner.plotCLs(opts.outfile)
    raw_input("Done\n")

else:
  import os
  os.system('mkdir -p stats')
  for j in range(opts.njobs):
    f = open('%s/stats/sub_j%d.sh'%(os.getcwd(),j),'w')
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
    f.write('source /afs/cern.ch/group/z5/group_login.sh\n')
    f.write('source setup_lxplus.sh\n')
    f.write('cd -\n')
    f.write('cp %s/lib/libAnalysis.so lib/\n'%os.getcwd())
    f.write('cp %s/runCLs.py .\n'%os.getcwd())
    f.write('cp %s/%s .\n'%(os.getcwd(),opts.infile))
    exec_line = './runCLs.py -i %s -o %s -t %d'%(opts.infile,opts.outfile,opts.ntoys)
    f.write('if ( %s ) then\n'%exec_line)
    f.write('\tmv %s %s/stats/%s\n'%(opts.outfile,os.getcwd(),opts.outfile.replace('.root','_j%d.root'%j)))
    f.write('\ttouch %s.done\n'%f.name)
    f.write('else\n')
    f.write('\ttouch %s.fail\n'%f.name)
    f.write('fi\n')
    f.write('rm -f %s.run\n'%f.name)
    f.close()
    os.system('chmod +x %s'%f.name)
    if opts.queue:
      os.system('bsub -q %s -o %s.log %s'%(opts.queue,f.name,f.name))
