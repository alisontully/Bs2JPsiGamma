#!/usr/bin/env python

import sys
if len(sys.argv)==2:
  fname = sys.argv[1]
else:
  fname = "AnalysisOut.root"

import ROOT as r

tf = r.TFile.Open(fname)
tree = tf.Get('AnalysisTree')

nbins = 50

channels = ['jpsig', 'kstarg']
categories = ['ll','dd']
sources = ['mc','data']

hists = {}

def setStyle():
  for ch in channels:
    for so in sources:
      for ca in categories:
        hist = hists[ch][so][ca]
        if so=='data':
          hist.SetMarkerSize(0.04)
        if so=='mc':
          hist.SetLineWidth(2)
        if ch=='jpsig':
          hist.SetLineColor(r.kRed-7)
          hist.SetFillColor(r.kRed-7)
          hist.SetMarkerColor(r.kRed)
        if ch=='kstarg':
          hist.SetLineColor(r.kBlue-7)
          hist.SetFillColor(r.kBlue-7)
          hist.SetMarkerColor(r.kBlue)


for channel in channels:
  hists[channel] = {}
  for source in sources:
    hists[channel][source] = {}
    for category in categories:
      name = channel+'_'+source+'_'+category
      hists[channel][source][category] = r.TH1F(name,'',nbins,-1,1)

for ev in range(tree.GetEntries()):
  tree.GetEntry(ev)

  category = 'll' if tree.eminus_TRACK_Type==3 and tree.eplus_TRACK_Type==3 else ''
  category = 'dd' if tree.eminus_TRACK_Type==5 and tree.eplus_TRACK_Type==5 else ''

  channel = 'jpsig' if tree.itype==71 or tree.itype==72 or tree.itype==-81 else ''
  channel = 'kstarg' if tree.itype==72 or tree.itype==82 or tree.itype==-88 else ''

  source = 'data' if tree.itype>0 else ''
  source = 'mc' if tree.itype<0 else ''

  if category=='' or channel=='' or source=='': continue

  hists[channel][source][category].Fill(tree.bdtoutput)

for ch in channel:
  for so in source:
    for ca in category:
      hists[ch][so][ca].Scale(1./hists[ch][so][ca].Integral())

r.gROOT.ProcessLine('.x ~/Scratch/lhcb/lhcbStyle.C')

setStyle()

for category in categories:
  canv = r.TCanvas()
  hists['jpsig']['mc'][category].Draw("HIST")
  hists['kstarg']['mc'][category].Draw("HISTsame")
  hists['jpsig']['data'][category].Draw("LEPsame")
  hists['kstarg']['data'][category].Draw("LEPsame")
  canv.Update()
  canv.Modified()
  raw_input()
