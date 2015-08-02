#!/usr/bin/env python

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-d","--dir")
parser.add_option("-b","--Bd",default=False,action="store_true")
parser.add_option("-w","--wide",default=False,action="store_true")
(opts,args) = parser.parse_args()

ext = 'bs'
if opts.Bd: ext='bd'
if opts.wide: ext += '_wide'

import ROOT as r
r.gROOT.ProcessLine('.x ~/lhcbStyle.C')
r.gROOT.SetBatch()

cls_canvs = []
#exp_sigma = [1,2,3]
exp_sigma = [1]

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
    if bkg_test_stat >= 0.:
      bkg_vals_list.append(bkg_test_stat)
    # sb
    sb_test_stat  = tree.sbtoy_teststat if tree.sbtoy_fitBF <= tree.hypothBF else 0.
    #sb_test_stat = tree.sbtoy_teststat
    if sb_test_stat >= 0.:
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

  real_btoy_fracs = [ 1.-quantiles[i]/100. for i in range(2*len(exp_sigma)+1) ]
  real_btoy_fracs.append(btoy_fracs[-1])
  assert(len(real_btoy_fracs)==len(btoy_fracs))
 
  cls_vals = [ sbtoy_fracs[i]/real_btoy_fracs[i] for i in range(len(real_btoy_fracs)) ]
  asymptotic_cls_vals = [ min(1.,r.TMath.Prob(x,1)/real_btoy_fracs[i]) for i, x in enumerate(test_stat_evals) ]

  # adjust cls vals for lack of stats
  for i in range(len(exp_sigma)):
    if i == len(exp_sigma)-1: continue
    if cls_vals[i]<cls_vals[len(exp_sigma)] or cls_vals[i]<cls_vals[i+1]: cls_vals[i] = 1.

  print '%6s  %10s  %5s  %5s  %6s  %6s  %6s  %8s  %8s'%('quant','qval','nBkg','nSB','fBkg','fSB','rfBkg','CLs','asCLs')
  for i, test_stat_point in enumerate(test_stat_evals):
    print '%6.2f  %10.7f  %5d  %5d  %6.4f  %6.4f  %6.4f  %8.6f  %8.6f'%(quantiles[i],test_stat_point,btoy_counters[i],sbtoy_counters[i],btoy_fracs[i],sbtoy_fracs[i],real_btoy_fracs[i],cls_vals[i],asymptotic_cls_vals[i])

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

  pave = r.TPaveText(0.2,0.7,0.45,0.9,"ndc")
  pave.SetFillColor(0)
  pave.SetShadowColor(0)
  pave.SetLineColor(0)
  pave.SetTextAlign(11)
  pave.SetTextSize(0.05)
  pave.AddText('BF_{H} = %g'%hypothBF)
  pave.AddText("CL_{S} (obs) = %4.2f"%obs_cls)
  pave.AddText("CL_{S} (exp) = %4.2f"%exp_cls)

  #chi2_dist = r.TF1("chi2","%6.4g*(1./(TMath::Power(2,0.5)*TMath::Gamma(0.5)))*TMath::Power(x,-0.5)*TMath::Exp(-0.5*x)"%sbtoys.GetEntries(),0,hmax)
  #chi2_dist.SetLineColor(r.kRed)
  #chi2_dist.SetLineWidth(2)

  sbtoys.Draw("HIST")
  #chi2_dist.Draw("Lsame")
  btoys.Draw("HISTsame")
  data.Draw("HISTsame")
  leg.Draw("same")
  pave.Draw("same")
  cls_canvs[-1].SetLogy()
  cls_canvs[-1].Update()
  cls_canvs[-1].Modified()
  cls_canvs[-1].Print("plots/stats/pdf/%s_cls_p%d.pdf"%(ext,p))
  cls_canvs[-1].Print("plots/stats/png/%s_cls_p%d.png"%(ext,p))
  cls_canvs[-1].Print("plots/stats/C/%s_cls_p%d.C"%(ext,p))

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

  #print flipped.Eval(val)
  return flipped.Eval(val)

direc = opts.dir

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

trees_to_run = list_of_trees
#if opts.Bd:
#  trees_to_run = list_of_trees[:-2]
#else:
#  trees_to_run = list_of_trees[:-1]
#
#if opts.wide:
#  trees_to_run = list_of_trees

for i, tree in enumerate(trees_to_run):
  if i<4 or i==len(list_of_trees)-1: continue
  #if tree == list_of_trees[-1] or tree == list_of_trees[-3]: continue
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
    y_err_high = vals[1][i]-ymean
    y_err_low = ymean-vals[1][-2-i]
    if y_err_high<0 and y_err_low<0:
      y_err_high *= -1.
      y_err_high *= -1.
    if y_err_high<0 and y_err_low>=0:
      y_err_high = y_err_low
    if y_err_low<0 and y_err_high>=0:
      y_err_low = y_err_high
    err_graphs[-1-i].SetPointError(p,0.,0.,y_err_low,y_err_high)

  print '%-3d %2.0g %6.4f %6.4f %6.4f %6.4f'%(p,x,yobs,ymean,y_err_low,y_err_high)

if len(err_graphs)>0:
  err_graphs[0].SetLineColor(r.kBlue-9)
  err_graphs[0].SetFillColor(r.kBlue-9)
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
dummy = r.TH1F('d','d',1,cls_info[0][0]*0.8,cls_info[-1][0]*1.2)
canv = r.TCanvas()
dummy.SetLineColor(0)
dummy.GetYaxis().SetRangeUser(1.e-4,1)
dummy.GetXaxis().SetTitle('BF ( B^{0}_{s} #rightarrow J/#psi#gamma )')
if opts.Bd:
  dummy.GetXaxis().SetTitle('BF ( B^{0}_{d} #rightarrow J/#psi#gamma )')
dummy.GetXaxis().SetTitleOffset(0.9)
dummy.GetXaxis().SetTitleSize(0.08)
dummy.GetYaxis().SetTitle('CL_{S}')
dummy.GetYaxis().SetTitleOffset(0.7)

dummy.Draw()
for gr in reversed(err_graphs): gr.Draw("LE3same")
mean_graph.Draw("Lsame")

exp_excl_90cl = compute_exclusion(mean_graph,0.1)
obs_excl_90cl = compute_exclusion(obs_graph,0.1)
exp_excl_95cl = compute_exclusion(mean_graph,0.05)
obs_excl_95cl = compute_exclusion(obs_graph,0.05)
exp_excl_99cl = compute_exclusion(mean_graph,0.01)
obs_excl_99cl = compute_exclusion(obs_graph,0.01)

exp_line = r.TLine()
exp_line.SetLineColor(r.kRed)
exp_line.SetLineStyle(r.kDashed)
exp_line.SetLineWidth(2)
exp_line.DrawLine(dummy.GetBinLowEdge(1),0.1,exp_excl_90cl,0.1)
exp_line.DrawLine(exp_excl_90cl,1.e-4,exp_excl_90cl,0.1)
exp_line.DrawLine(dummy.GetBinLowEdge(1),0.05,exp_excl_95cl,0.05)
exp_line.DrawLine(exp_excl_95cl,1.e-4,exp_excl_95cl,0.05)
#exp_line.DrawLine(dummy.GetBinLowEdge(1),0.01,exp_excl_99cl,0.01)
#exp_line.DrawLine(exp_excl_99cl,1.e-4,exp_excl_99cl,0.01)

obs_line = r.TLine()
obs_line.SetLineColor(r.kBlack)
obs_line.SetLineStyle(r.kDashed)
obs_line.SetLineWidth(2)
obs_line.DrawLine(dummy.GetBinLowEdge(1),0.1,obs_excl_90cl,0.1)
obs_line.DrawLine(obs_excl_90cl,1.e-4,obs_excl_90cl,0.1)
obs_line.DrawLine(dummy.GetBinLowEdge(1),0.05,obs_excl_95cl,0.05)
obs_line.DrawLine(obs_excl_95cl,1.e-4,obs_excl_95cl,0.05)
#obs_line.DrawLine(dummy.GetBinLowEdge(1),0.01,obs_excl_99cl,0.01)
#obs_line.DrawLine(obs_excl_99cl,1.e-4,obs_excl_99cl,0.01)

obs_graph.Draw("LEPsame")

leg = r.TLegend(0.17,0.42,0.55,0.65)
leg.SetFillColor(0)
leg.SetLineColor(0)
leg.SetTextSize(0.05)
leg.AddEntry(obs_graph,'Observed','LEP')
leg.AddEntry(mean_graph,'Expected','L')
for i, exp_sig in enumerate(exp_sigma):
  leg.AddEntry(err_graphs[i],'#pm %d#sigma'%exp_sig,'LF')
leg2 = r.TLegend(0.17,0.17,0.55,0.42)
leg2.SetFillColor(0)
leg2.SetLineColor(0)
leg2.SetTextSize(0.042)
leg2.AddEntry(obs_line,"Observed BF < %1.3g @ 90%% CL"%obs_excl_90cl,"L")
leg2.AddEntry(exp_line,"Expected BF < %1.3g @ 90%% CL"%exp_excl_90cl,"L")
leg2.AddEntry(obs_line,"Observed BF < %1.3g @ 95%% CL"%obs_excl_95cl,"L")
leg2.AddEntry(exp_line,"Expected BF < %1.3g @ 95%% CL"%exp_excl_95cl,"L")
#leg.AddEntry(obs_line,"Observed BF < %1.3g @ 99%% CL"%obs_excl_99cl,"L")
#leg.AddEntry(exp_line,"Expected BF < %1.3g @ 99%% CL"%exp_excl_99cl,"L")

pave = r.TPaveText(0.17,0.76,0.45,0.85,"ndc")
pave.SetFillColor(0)
pave.SetShadowColor(0)
pave.SetLineColor(0)
pave.SetTextSize(0.075)
pave.SetTextAlign(11)
pave.AddText("LHCb Preliminary")

leg.Draw("same")
leg2.Draw("same")
pave.Draw("same")

canv.SetLogx()
canv.SetLogy()
canv.RedrawAxis()
canv.Update()
canv.Modified()
canv.Print('plots/stats/pdf/%s_cls_bf.pdf'%ext)
canv.Print('plots/stats/png/%s_cls_bf.png'%ext)
canv.Print('plots/stats/C/%s_cls_bf.C'%ext)

canv.SetLogx(False)
canv.SetLogy(False)
leg.SetX1NDC(0.65)
leg.SetX2NDC(0.92)
leg.SetY1NDC(0.69)
leg.SetY2NDC(0.92)
leg2.SetX1NDC(0.4)
leg2.SetX2NDC(0.9)
leg2.SetY1NDC(0.44)
leg2.SetY2NDC(0.69)
pave.SetX1NDC(0.2)
pave.SetX2NDC(0.48)
canv.RedrawAxis()
canv.Update()
canv.Modified()
canv.Print('plots/stats/pdf/%s_cls_bf_nolog.pdf'%ext)
canv.Print('plots/stats/png/%s_cls_bf_nolog.png'%ext)
canv.Print('plots/stats/C/%s_cls_bf_nolog.C'%ext)

print '---------- RESULT: -----------'
print '   CLs Obs (90%%CL) = %5.3g'%obs_excl_90cl
print '   CLs Obs (95%%CL) = %5.3g'%obs_excl_95cl
print '------------------------------'

raw_input('Done\n')

