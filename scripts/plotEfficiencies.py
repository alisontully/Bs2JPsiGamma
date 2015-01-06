#!/usr/bin/env python

import os
import sys
import ROOT as r

os.system('mkdir -p plots/Efficiencies')

r.gROOT.ProcessLine(".x ~/Scratch/lhcb/lhcbStyle.C")
r.gStyle.SetGridWidth(3)
r.gStyle.SetGridStyle(1)
r.gStyle.SetPalette(1)

infiles = sys.argv[1:]
canvs=[]

xlabels = []
ylabels = []

def fillDict(hName, isEff=False):
  theDict = {}
  for f in infiles:
    tf = r.TFile.Open(f)
    hist = tf.Get(hName)
    for xbin in range(1,hist.GetNbinsX()+1):
      analyser_label = hist.GetXaxis().GetBinLabel(xbin)
      if analyser_label not in theDict.keys():
        theDict[analyser_label] = {}
        if analyser_label not in xlabels: xlabels.append(analyser_label)
      for ybin in range(1,hist.GetNbinsY()+1):
        dataset_label = hist.GetYaxis().GetBinLabel(ybin)
        if dataset_label not in theDict[analyser_label].keys():
          bin2d = hist.FindBin(xbin-1,ybin-1)
          theDict[analyser_label][dataset_label] = hist.GetBinContent(bin2d)
          if dataset_label not in ylabels: ylabels.append(dataset_label)
        elif analyser_label == 'Start':
          bin2d = hist.FindBin(xbin-1,ybin-1)
          new_val = hist.GetBinContent(bin2d)
          old_val = theDict[analyser_label][dataset_label]
          theDict[analyser_label][dataset_label] = max(new_val,old_val)
        else:
          print 'WARNING - clash - entry already exists for [', analyser_label, ',', dataset_label, ']'
    tf.Close()

  theDict['Total'] = {}
  if 'Total' in xlabels: xlabels.remove('Total')
  for ylabel in ylabels:
    totalval = 0
    if isEff: totalval = 1.
    for xlabel in xlabels:
      val = theDict[xlabel][ylabel]
      if isEff:
        totalval *= val
      else:
        totalval = val
    theDict['Total'][ylabel] = totalval
  xlabels.append('Total')
  return theDict

def convertDictToTH2(theDict, name):

  xbins = len(theDict.keys())
  ybins = len(theDict[theDict.keys()[0]])

  for xlabel, yDict in theDict.items():
    assert( len(yDict.keys()) == ybins )

  assert( len(xlabels)==xbins )
  assert( len(ylabels)==ybins )

  ylabels.sort()

  th2f = r.TH2F(name,name,xbins,0,xbins,ybins,0,ybins)

  for xbin, xlabel in enumerate(xlabels):
    yDict = theDict[xlabel]
    th2f.GetXaxis().SetBinLabel(xbin+1,xlabel)
    for ybin, ylabel in enumerate(ylabels):
      th2f.GetYaxis().SetBinLabel(ybin+1,tidyLabel(ylabel))
      bin2d = th2f.FindBin(xbin,ybin)
      th2f.SetBinContent(bin2d, theDict[xlabel][ylabel])

  return th2f

def printDict(theDict):
  for xlabel, yDict in theDict.items():
    for ylabel, value in yDict.items():
      print '%20s  %20s'%(xlabel,ylabel), ' = ', value

def tidyLabel(label):
  new_label = label.replace('_',' ')
  new_label = new_label.replace('MagDown','MD')
  new_label = new_label.replace('MagUp','MU')
  new_label = new_label.replace('LowMass','(LM)')
  new_label = new_label.replace('HighMass','(HM)')
  return new_label

def draw(th2f, name, textformat='', col=False):

  canvs.append(r.TCanvas(name,name,1100,600))
  canvs[-1].SetLeftMargin(0.3)
  if col: canvs[-1].SetRightMargin(0.15)
  else: canvs[-1].SetRightMargin(0.05)
  canvs[-1].SetGridx()
  canvs[-1].SetGridy()
  canvs[-1].cd()
  r.gStyle.SetPaintTextFormat(textformat)
  th2f.SetMarkerSize(2.0)
  #if col: th2f.SetMarkerColor(r.kWhite)
  th2f.SetStats(0)
  if col: th2f.Draw("TEXTcolz")
  else: th2f.Draw("TEXT")
  canvs[-1].Update()
  canvs[-1].Modified()
  canvs[-1].Print("plots/Efficiencies/eff_%s.pdf"%name)

# __main__
passDict = fillDict('hPass')
failDict = fillDict('hFail')
effDict  = fillDict('hEff',True)

passHist = convertDictToTH2(passDict,'hPass')
failHist = convertDictToTH2(failDict,'hFail')
effHist  = convertDictToTH2(effDict,'hEff')
effHist.Scale(100.)

draw(passHist,'pass','g')
draw(failHist,'fail','g')
draw(effHist, 'eff','5.1f%%',True)

raw_input('Done\n')
