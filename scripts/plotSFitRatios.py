#!/usr/bin/env python

import os
import sys
import ROOT as r

os.system('mkdir -p plots/SFitResiduals/pdf')
os.system('mkdir -p plots/SFitResiduals/png')
os.system('mkdir -p plots/SFitResiduals/C')

r.gROOT.ProcessLine('.x ~/Scratch/lhcb/lhcbStyle.C')

titleMap = {  'B0_DIRA_OWNPV' : 'DIRA(B_{d})'
             ,'gamgams_PT'    : 'p_{T}(#gamma)'
           }

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
  splits = graph.GetName().split('_v')
  name = splits[-2]+'_'+splits[-1]

  title = splits[-1]
  if splits[-1] in titleMap.keys():
    title = titleMap[splits[-1]]
  graph.GetXaxis().SetTitle(title)
  graph.GetYaxis().SetTitle("Data (sweighted) / MC")
  graph.GetXaxis().SetTitleOffset(0.8)
  graph.GetYaxis().SetTitleOffset(0.75)
  graph.GetYaxis().SetRangeUser(-0.5,3.5)
  graph.Draw("APE")
  line.DrawLine(graph.GetXaxis().GetXmin(),1.,graph.GetXaxis().GetXmax(),1.)
  graph.Draw("PEsame")

  text = r.TPaveText(0.15,0.8,0.4,0.9,"ndc");
  text.SetFillColor(0);
  text.SetShadowColor(0);
  text.SetLineColor(0);
  text.AddText(graph.GetTitle());
  text.Draw("same")

  canvs[-1].Update()
  canvs[-1].Modified()
  canvs[-1].Print("plots/SFitResiduals/pdf/splot_ratio_%s.pdf"%name)
  canvs[-1].Print("plots/SFitResiduals/png/splot_ratio_%s.png"%name)
  canvs[-1].Print("plots/SFitResiduals/C/splot_ratio_%s.C"%name)

tf = r.TFile(fname)

for key in tf.Get('residuals').GetListOfKeys():
  if key.GetName().startswith('resid'):
    residGr = tf.Get('residuals/%s'%key.GetName())
    if 'DD' in residGr.GetName(): residGr.SetTitle('DD Candidates')
    if 'LL' in residGr.GetName(): residGr.SetTitle('LL Candidates')
    draw(residGr)

raw_input()
tf.Close()
