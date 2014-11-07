#!/usr/bin/env python

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-i","--infile",default="AnalysisOut.root",help="Name of input root file. Default=%default")
parser.add_option("-t","--treename",default="AnalysisTree",help="Name of input tree. Default=%default")
parser.add_option("-o","--outfile",default="FitterOut.root",help="Name of output file. Default=%default")
parser.add_option("-I","--interactive",default=False,action="store_true")
(opts,args) = parser.parse_args()

import ROOT as r
r.gSystem.Load("lib/libAnalysis")
if not opts.interactive: r.gROOT.SetBatch()

sw = r.TStopwatch()
sw.Start()

print 'Starting Fitter'

fitter = r.Fitter()
fitter.setBDTCut(0.2)
fitter.makeObsVars()
fitter.makeDatasets()
fitter.fillDatasets(opts.infile,opts.treename)
fitter.doKstarSplot()
fitter.save(opts.outfile)

sw.Stop()

if opts.interactive: raw_input('Done')
