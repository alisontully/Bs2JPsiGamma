#!/usr/bin/env python

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-i","--infile",help="File to run on")
parser.add_option("-s","--nsig",default=100,help="Number of expected signal events")
parser.add_option("-b","--nbkg",default=100,help="Number of expected background events")
parser.add_option("-I","--interactive",default=False,action="store_true",help="Run in interactive mode")
(opts,args) = parser.parse_args()

import ROOT as r
if not opts.interactive:
  r.gROOT.SetBatch()

import os

# make output directories
os.system('mkdir -p plots/MVATrainingFDChi2/pdf')
os.system('mkdir -p plots/MVATrainingFDChi2/png')
os.system('mkdir -p plots/MVATrainingFDChi2/C')

# get required files
if not os.path.exists('scripts/tmvaglob.C'):
  os.system('cp -r /Applications/root/tmva/test/tmvaglob.C scripts/')

# make output variable plots
if not os.path.exists('scripts/mvas.C'):
  os.system('cp -r /Applications/root/tmva/test/mvas.C scripts/')

r.gROOT.ProcessLine('.x scripts/mvas.C(\"%s\",CompareType)'%opts.infile)
os.system('mv plots/overtrain_*.pdf plots/MVATrainingFDChi2/pdf')
os.system('mv plots/overtrain_*.png plots/MVATrainingFDChi2/png')
os.system('mv plots/overtrain_*.C plots/MVATrainingFDChi2/C')

# make cut efficiency plots
if not os.path.exists('scripts/mvaeffs.C'):
  os.system('cp -r /Applications/root/tmva/test/mvaeffs.C scripts/')

r.gROOT.ProcessLine('.x scripts/mvaeffs.C(\"%s\",%f,%f)'%(opts.infile,float(opts.nsig),float(opts.nbkg)))
os.system('mv plots/mvaeffs_*.pdf plots/MVATrainingFDChi2/pdf')
os.system('mv plots/mvaeffs_*.png plots/MVATrainingFDChi2/png')
os.system('mv plots/mvaeffs_*.C plots/MVATrainingFDChi2/C')

