#!/usr/bin/env python

import ROOT as r
r.gROOT.ProcessLine('.x ~/Scratch/lhcb/lhcbStyle.C')

cls_canvs = []
exp_sigma = [1,2,3]

def getCLs(tree):

  p = len(cls_canvs)
  cls_canvs.append(r.TCanvas('c%d'%p,'c%d'%p,(p+1)*10,(p+1)*10,800,600))
  hypothBF = -999.
  dataTestStat = -999.

  # get test stat value in data
  entry = 0
  while ( tree.toyn != -1 ):
    tree.GetEntry(entry)
    hypothBF = tree.hypothBF
    dataTestStat = tree.sbtoy_teststat
    entry += 1

  bkg_vals_list = []
  sb_vals_list = []

  # get test stat value in toys
  for entry in range(tree.GetEntries()):
    tree.GetEntry(entry)
    hypothBF = tree.hypothBF
    # data
    if tree.toyn<0: continue
    # bkg
    bkg_test_stat = tree.btoy_teststat if tree.btoy_fitBF <= tree.hypothBF else 0.
    #bkg_test_stat = tree.btoy_teststat
    bkg_vals_list.append(bkg_test_stat)
    # sb
    sb_test_stat  = tree.sbtoy_teststat if tree.sbtoy_fitBF <= tree.hypothBF else 0.
    #sb_test_stat = tree.sbtoy_teststat
    sb_vals_list.append(sb_test_stat)

  import numpy as np

  bkg_vals_list.sort()
  sb_vals_list.sort()
  bkg_vals = np.array(bkg_vals_list)
  sb_vals = np.array(sb_vals_list)

  quantiles = []
  test_stat_evals = []
  btoy_counters = []
  sbtoy_counters = []

  # low expec err
  for exp_s in reversed(exp_sigma):
    quantile = 100.*r.TMath.Prob(exp_s**2,1)
    quantiles.append(quantile)
    test_stat_evals.append(np.percentile(bkg_vals,quantile))
    btoy_counters.append(0)
    sbtoy_counters.append(0)
  # median
  quantile = 50.
  quantiles.append(quantile)
  test_stat_evals.append(np.percentile(bkg_vals,quantile))
  btoy_counters.append(0)
  sbtoy_counters.append(0)
  # high expec err
  for exp_s in exp_sigma:
    quantile = 100.*(1.-r.TMath.Prob(exp_s**2,1))
    quantiles.append(quantile)
    test_stat_evals.append(np.percentile(bkg_vals,quantile))
    btoy_counters.append(0)
    sbtoy_counters.append(0)
  # observation
  quantile = -1.
  quantiles.append(quantile)
  test_stat_evals.append(dataTestStat)
  btoy_counters.append(0)
  sbtoy_counters.append(0)

  hmax = max(np.ceil(np.amax(bkg_vals)),np.ceil(np.amax(sb_vals)))

  btoys = r.TH1F('btoys_p%d'%p,'btoys',100,0,hmax)
  sbtoys = r.TH1F('sbtoys_p%d'%p,'sbtoys',100,0,hmax)
  data = r.TH1F('data_p%d'%p,'data',10000,0,50)
  data.SetBinContent(data.FindBin(dataTestStat),tree.GetEntries()/10)

  # get cls values and fill hists
  for bkg_val in bkg_vals:
    btoys.Fill(bkg_val)
    for i, test_stat_point in enumerate(test_stat_evals):
      if bkg_val > test_stat_point:
        btoy_counters[i] += 1
  for sb_val in sb_vals:
    sbtoys.Fill(sb_val)
    for i, test_stat_point in enumerate(test_stat_evals):
      if sb_val > test_stat_point:
        sbtoy_counters[i] += 1

  btoy_fracs = [ float(btoy_count)/float(len(bkg_vals)) for btoy_count in btoy_counters ]
  sbtoy_fracs = [ float(sbtoy_count)/float(len(sb_vals)) for sbtoy_count in sbtoy_counters ]
  cls_vals = [ sbtoy_fracs[i]/btoy_fracs[i] for i in range(len(btoy_fracs)) ]

  print '%6s  %10s  %5s  %5s  %4s  %4s  %4s'%('quant','qval','nBkg','nSB','fBkg','fSB','CLs')
  for i, test_stat_point in enumerate(test_stat_evals):
    print '%6.2f  %10.7f  %5d  %5d  %4.2f  %4.2f  %4.2f'%(quantiles[i],test_stat_point,btoy_counters[i],sbtoy_counters[i],btoy_fracs[i],sbtoy_fracs[i],cls_vals[i])

  obs_cls = cls_vals[-1]
  exp_cls = cls_vals[len(exp_sigma)]

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

  sbtoys.GetXaxis().SetTitle('-2LLR')
  sbtoys.GetXaxis().SetTitleOffset(0.8)
  sbtoys.GetYaxis().SetTitle('Number of Toys')
  sbtoys.GetYaxis().SetTitleOffset(0.7)

  pave = r.TPaveText(0.15,0.7,0.4,0.9,"ndc")
  pave.SetFillColor(0)
  pave.SetShadowColor(0)
  pave.SetLineColor(0)
  pave.SetTextAlign(11)
  pave.SetTextSize(0.05)
  pave.AddText('BF_{H} = %g'%hypothBF)
  pave.AddText("CL_{S} (obs) = %4.2f"%obs_cls)
  pave.AddText("CL_{S} (exp) = %4.2f"%exp_cls)

  sbtoys.Draw("HIST")
  btoys.Draw("HISTsame")
  data.Draw("HISTsame")
  leg.Draw("same")
  pave.Draw("same")
  cls_canvs[-1].SetLogy()
  cls_canvs[-1].Update()
  cls_canvs[-1].Modified()
  cls_canvs[-1].Print("plots/stats/pdf/cls_p%d.pdf"%p)
  cls_canvs[-1].Print("plots/stats/png/cls_p%d.png"%p)
  cls_canvs[-1].Print("plots/stats/C/cls_p%d.C"%p)

  return (hypothBF, cls_vals)

def compute_exclusion(graph,val):
  # flip graph
  points = []
  flipped = r.TGraph()
  x = r.Double()
  y = r.Double()
  for p in range(graph.GetN()):
    graph.GetPoint(p,x,y)
    flipped.SetPoint(p,y,x)

  print flipped.Eval(val)
  return flipped.Eval(val)

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

cls_info = []

for tree in list_of_trees:
  for fil in rel_files:
    tree.Add(fil)

  hypothBF, cls_vals = getCLs(tree)
  if hypothBF>2.e-5: continue
  cls_info.append([hypothBF, cls_vals])

cls_info.sort(key=lambda x: x[0])
err_graphs = [ r.TGraphAsymmErrors() for i in range(len(exp_sigma)) ]
mean_graph = r.TGraph()
obs_graph = r.TGraph()

for p, vals in enumerate(cls_info):

  x = vals[0]
  yobs = vals[1][-1]
  obs_graph.SetPoint(p,x,yobs)
  ymean = vals[1][len(exp_sigma)]
  mean_graph.SetPoint(p,x,ymean)
  for i in range(len(exp_sigma)):
    err_graphs[-1-i].SetPoint(p,x,ymean)
    err_graphs[-1-i].SetPointError(p,0.,0.,ymean-vals[1][i],vals[1][-2-i]-ymean)

if len(err_graphs)>0:
  err_graphs[0].SetLineColor(r.kYellow)
  err_graphs[0].SetFillColor(r.kYellow)
if len(err_graphs)>1:
  err_graphs[1].SetLineColor(r.kGreen)
  err_graphs[1].SetFillColor(r.kGreen)
if len(err_graphs)>2:
  err_graphs[2].SetLineColor(r.kBlue-7)
  err_graphs[2].SetFillColor(r.kBlue-7)

mean_graph.SetLineWidth(3)
mean_graph.SetLineColor(r.kRed)

obs_graph.SetLineWidth(3)
obs_graph.SetLineColor(r.kBlack)
obs_graph.SetMarkerColor(r.kBlack)

# draw
dummy = r.TH1F('d','d',1,cls_info[0][0]*0.5,cls_info[-1][0]*2.)
canv = r.TCanvas()
dummy.SetLineColor(0)
dummy.GetYaxis().SetRangeUser(1.e-4,1)
dummy.GetXaxis().SetTitle('B_{s} #rightarrow J/#psi#gamma BF')
dummy.GetXaxis().SetTitleOffset(0.9)
dummy.GetXaxis().SetTitleSize(0.08)
dummy.GetYaxis().SetTitle('CL_{S}')
dummy.GetYaxis().SetTitleOffset(0.7)

dummy.Draw()
for gr in reversed(err_graphs): gr.Draw("LE3same")
mean_graph.Draw("Lsame")

exp_excl_95cl = compute_exclusion(mean_graph,0.07)
obs_excl_95cl = compute_exclusion(obs_graph,0.05)

exp_line = r.TLine()
exp_line.SetLineColor(r.kRed)
exp_line.SetLineStyle(r.kDashed)
exp_line.SetLineWidth(2)
exp_line.DrawLine(dummy.GetBinLowEdge(1),0.05,exp_excl_95cl,0.05)
exp_line.DrawLine(exp_excl_95cl,1.e-4,exp_excl_95cl,0.05)

obs_line = r.TLine()
obs_line.SetLineColor(r.kBlack)
obs_line.SetLineStyle(r.kDashed)
obs_line.SetLineWidth(2)
obs_line.DrawLine(dummy.GetBinLowEdge(1),0.05,obs_excl_95cl,0.05)
obs_line.DrawLine(obs_excl_95cl,1.e-4,obs_excl_95cl,0.05)

obs_graph.Draw("LEPsame")

leg = r.TLegend(0.18,0.18,0.6,0.6)
leg.SetFillColor(0)
leg.SetLineColor(0)
leg.SetTextSize(0.05)
leg.AddEntry(obs_line,"Observed BF < %1.3g @ 95%% CL"%obs_excl_95cl,"L")
leg.AddEntry(exp_line,"Expected BF < %1.3g @ 95%% CL"%exp_excl_95cl,"L")
leg.AddEntry(obs_graph,'Observed','LEP')
leg.AddEntry(mean_graph,'Expected','L')
for i, exp_sig in enumerate(exp_sigma):
  leg.AddEntry(err_graphs[i],'#pm %d#sigma'%exp_sig,'LF')

leg.Draw("same")

canv.Update()
canv.Modified()
canv.SetLogx()
canv.SetLogy()
canv.RedrawAxis()
canv.Print('plots/stats/pdf/cls_bf.pdf')
canv.Print('plots/stats/png/cls_bf.png')
canv.Print('plots/stats/C/cls_bf.C')

raw_input('Done\n')

