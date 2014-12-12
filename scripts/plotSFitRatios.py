#!/usr/bin/env python

import sys
import ROOT as r

r.gROOT.ProcessLine('.x ~/Scratch/lhcb/lhcbStyle.C')

fname = ''
if len(sys.argv)==1:
  fname = 'FitterOut.root'
else:
  fname = sys.argv[1]

canvs = []
line = r.TLine()
line.SetLineWidth(2)
line.SetLineColor(r.kBlue)
line.SetLineStyle(r.kDashed)

def draw(graph):
  canvs.append(r.TCanvas())
  name = graph.GetName().split('_v')[-1]
  graph.GetXaxis().SetTitle(name)
  graph.GetYaxis().SetTitle("Data (sweighted) / MC")
  graph.GetXaxis().SetTitleOffset(0.8)
  graph.GetYaxis().SetTitleOffset(0.75)
  graph.GetYaxis().SetRangeUser(0.,2.5)
  graph.Draw("APE")
  line.DrawLine(graph.GetXaxis().GetXmin(),1.,graph.GetXaxis().GetXmax(),1.)
  graph.Draw("PEsame")
  canvs[-1].Update()
  canvs[-1].Modified()
  canvs[-1].Print("plots/splot_ratio_%s.pdf"%name)

tf = r.TFile(fname)

for key in tf.Get('residuals').GetListOfKeys():
  if key.GetName().startswith('resid'):
    residGr = tf.Get('residuals/%s'%key.GetName())
    draw(residGr)

raw_input()
tf.Close()
