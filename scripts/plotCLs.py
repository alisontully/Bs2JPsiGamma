#!/usr/bin/env python

import ROOT as r

cls_canvs = []

def getCLs(tree):

  p = len(cls_canvs)
  cls_canvs.append(r.TCanvas('c%d'%p,'c%d'%p,(p+1)*10,(p+1)*10,800,600))
  btoys = r.TH1F('btoys_p%d'%p,'btoys',100,0,10)
  sbtoys = r.TH1F('sbtoys_p%d'%p,'sbtoys',100,0,10)
  data = r.TH1F('data_p%d'%p,'data',10000,0,10)
  hypothBF = -999.
  dataTestStat = -999.

  # get test stat value in data
  entry = 0
  while ( tree.toyn != -1 ):
    tree.GetEntry(entry)
    hypothBF = tree.hypothBF
    dataTestStat = tree.sbtoy_teststat
    entry += 1

  data.SetBinContent(data.FindBin(dataTestStat),tree.GetEntries()/10)

  # get test stat value in toys
  for entry in range(tree.GetEntries()):
    tree.GetEntry(entry)
    hypothBF = tree.hypothBF
    if tree.toyn<0: # data
      continue
    if ( tree.btoy_fitBF > tree.hypothBF ): # one-sided
      btoys.Fill(0)
    else:
      btoys.Fill(tree.btoy_teststat)
    if ( tree.sbtoy_fitBF > tree.hypothBF ): # one-sided
      sbtoys.Fill(0)
    else:
      sbtoys.Fill(tree.sbtoy_teststat)

  sig_expec = sbtoys.GetMean()

  n_bkg_toys_above_data = 0
  n_bkg_toys_above_sig_expec = 0
  n_sb_toys_above_data = 0
  n_sb_toys_above_sig_expec = 0

  # get cls values
  for entry in range(tree.GetEntries()):
    tree.GetEntry(entry)
    if tree.toyn<0: continue
    bkg_test_stat = tree.btoy_teststat
    if ( tree.btoy_fitBF > tree.hypothBF ): bkg_test_stat = 0.
    sb_test_stat = tree.sbtoy_teststat
    if ( tree.sbtoy_fitBF > tree.hypothBF ): sb_test_stat = 0.
    # see if above criteria
    if bkg_test_stat > dataTestStat: n_bkg_toys_above_data += 1
    if bkg_test_stat > sig_expec: n_bkg_toys_above_sig_expec += 1
    if sb_test_stat > dataTestStat: n_sb_toys_above_data += 1
    if sb_test_stat > sig_expec: n_sb_toys_above_sig_expec += 1 # should hope this is 50%

  obs_cls = ( float(n_bkg_toys_above_data)/float(btoys.GetEntries()) ) / ( float(n_sb_toys_above_data)/float(sbtoys.GetEntries()) )
  exp_cls = ( float(n_bkg_toys_above_sig_expec)/float(btoys.GetEntries()) ) / ( float(n_sb_toys_above_sig_expec)/float(sbtoys.GetEntries()) )

  btoys.SetLineColor(r.kBlue)
  btoys.SetLineWidth(2)
  btoys.SetStats(0)
  sbtoys.SetLineColor(r.kRed)
  sbtoys.SetLineWidth(2)
  sbtoys.SetStats(0)
  data.SetLineWidth(5)

  leg = r.TLegend(0.6,0.6,0.89,0.89)
  leg.SetFillColor(0)
  leg.AddEntry(btoys,"Bkg Only Toys","L")
  leg.AddEntry(sbtoys,"S+B Toys","L")
  leg.AddEntry(data,"Observation","L")

  sbtoys.Draw("HIST")
  btoys.Draw("HISTsame")
  data.Draw("HISTsame")
  leg.Draw("same")
  cls_canvs[-1].Update()
  cls_canvs[-1].Modified()
  cls_canvs[-1].Print("plots/stats/pdf/cls_p%d.pdf"%p)
  cls_canvs[-1].Print("plots/stats/png/cls_p%d.png"%p)
  cls_canvs[-1].Print("plots/stats/C/cls_p%d.C"%p)

  return (hypothBF,0.05)

import sys
if len(sys.argv)!=2:
  sys.exit('usage plotCLs.py <dir_w_files>')

direc = sys.argv[1]

import os
os.system('mkdir -p plots/stats/pdf')
os.system('mkdir -p plots/stats/png')
os.system('mkdir -p plots/stats/C')
import fnmatch
for root, dirs, files in os.walk(direc):
  if root != direc: continue
  rel_files = fnmatch.filter(files,'*.root')
  if 'model_ws.root' in rel_files: rel_files.remove('model_ws.root')

rel_files = [ direc+'/'+x for x in rel_files ]
print rel_files
list_of_trees = []

testf = r.TFile(rel_files[0])

i=0
while ( testf.Get('result_tree_p%d'%i) != None ):
  list_of_trees.append( r.TChain('result_tree_p%d'%i) )
  i+=1

testf.Close()

for tree in list_of_trees:
  for fil in rel_files:
    tree.Add(fil)

  (BF,CLs) = getCLs(tree)

raw_input('Done\n')

