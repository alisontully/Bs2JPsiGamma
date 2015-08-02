#!/usr/bin/env python

import ROOT as r

import sys
tf = r.TFile(sys.argv[1])
w = tf.Get('model_ws')

import numpy

gr = r.TGraph()
p=0
for bf in numpy.arange(0., 2.e-5, 1.e-6,):
  print bf
  BF = w.var('BF')
  BF.setVal(bf)
  BF.setConstant(bf)
  fitres = w.pdf('simpdf').fitTo(w.data('data'),r.RooFit.Extended(),r.RooFit.Save())
  gr.SetPoint(p, bf, fitres.minNll())
  p += 1

canv = r.TCanvas()
gr.Draw("ALP")
canv.Update()
canv.Modified()
