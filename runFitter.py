#!/usr/bin/env python

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-i","--infile",default="AnalysisOut.root",help="Name of input root file. Default=%default")
parser.add_option("-t","--treename",default="AnalysisTree",help="Name of input tree. Default=%default")
parser.add_option("-o","--outfile",default="FitterOut.root",help="Name of output file. Default=%default")
(opts,args) = parser.parse_args()

import ROOT as r
r.gSystem.Load("lib/libAnalysis")
r.gROOT.SetBatch()

sw = r.TStopwatch()
sw.Start()

print 'Starting Fitter'

fitter = r.Fitter()
fitter.setup(opts.infile,opts.treename)
fitter.fit()
fitter.plot()
fitter.save(opts.outfile)
