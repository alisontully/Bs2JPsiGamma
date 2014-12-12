#!/usr/bin/env python

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-T","--type",default="KstGammaFit",help="Which fitter to run. Default=%default")
parser.add_option("-i","--infile",default="AnalysisOut.root",help="Name of input root file. Default=%default")
parser.add_option("-t","--treename",default="AnalysisTree",help="Name of input tree. Default=%default")
parser.add_option("-o","--outfile",default="DefaultFitterOut.root",help="Name of output file. Default=%default")
parser.add_option("-I","--interactive",default=False,action="store_true",help="Run in interactive mode")
parser.add_option("-v","--verbose",default=False,action="store_true",help="Run with verbose output")
parser.add_option("-d","--debug",default=False,action="store_true",help="Run in debug mode")
(opts,args) = parser.parse_args()

if opts.outfile == "DefaultFitterOut.root":
  opts.outfile = "%sOut.root"%opts.type

import ROOT as r
r.gSystem.Load("lib/libAnalysis")
if not opts.interactive: r.gROOT.SetBatch()

sw = r.TStopwatch()
sw.Start()

fitter = getattr(r,opts.type)("w",opts.verbose,opts.debug)
fitter.addObsVars()
fitter.addCuts()
fitter.addDatasets()
fitter.makeDatasets()
fitter.fillDatasets(opts.infile, opts.treename)
fitter.constructPdfs()
fitter.run()
fitter.save(opts.outfile)

sw.Stop()
print 'Took:', sw.Print()

if opts.interactive: raw_input("Done\n")
