#!/usr/bin/env python

import ROOT as r

r.TH1.SetDefaultSumw2()

tf = r.TFile("AnalysisOut.root")
tree = tf.Get("AnalysisTree")

varname = "bdtoutput"
cut = ""
bins = 50
xlow = -1.
xhigh = 1.

ll_mc = r.TH1F("%s_ll_mc"%varname,"",bins,xlow,xhigh)
dd_mc = r.TH1F("%s_dd_mc"%varname,"",bins,xlow,xhigh)

ll_data = r.TH1F("%s_ll_data"%varname,"",bins,xlow,xhigh)
dd_data = r.TH1F("%s_dd_data"%varname,"",bins,xlow,xhigh)

ll_mc_cut = r.TCut("( (itype==-88) && eplus_TRACK_Type==3 && eminus_TRACK_Type==3 )")
dd_mc_cut = r.TCut("( (itype==-88) && eplus_TRACK_Type==5 && eminus_TRACK_Type==5 )")

ll_data_cut = r.TCut("( (itype==72 || itype==82) && eplus_TRACK_Type==3 && eminus_TRACK_Type==3 )")
dd_data_cut = r.TCut("( (itype==72 || itype==82) && eplus_TRACK_Type==5 && eminus_TRACK_Type==5 )")

hists = [ ll_mc, dd_mc, ll_data, dd_data ]
cuts  = [ ll_mc_cut, dd_mc_cut, ll_data_cut, dd_data_cut ]

maxi=0.
for i, h in enumerate(hists):
  tcut = r.TCut(cut) + cuts[i]
  tree.Draw("%s>>%s"%(varname,h.GetName()), tcut, "GOFF")
  h.Scale(1./h.Integral())
  h.SetLineWidth(2)
  maxi = r.TMath.Max(maxi,h.GetMaximum())

ll_mc.SetLineColor(r.kRed-1)
ll_mc.SetLineWidth(1)
myRed = r.gROOT.GetColor(r.kRed)
myRed.SetAlpha(0.3)
ll_mc.SetFillColor(myRed.GetNumber())
#ll_mc.SetFillStyle(3003)

dd_mc.SetMarkerColor(r.kRed+1)
dd_mc.SetLineColor(r.kRed+1)
dd_mc.SetLineWidth(2)

ll_data.SetLineColor(r.kBlue-1)
ll_data.SetLineWidth(1)
myBlue = r.gROOT.GetColor(r.kBlue)
myBlue.SetAlpha(0.3)
ll_data.SetFillColor(myBlue.GetNumber())
#ll_data.SetFillStyle(3003)

dd_data.SetMarkerColor(r.kBlue+1)
dd_data.SetLineColor(r.kBlue+1)
dd_data.SetLineWidth(2)

r.gROOT.ProcessLine(".x ~/Scratch/lhcb/lhcbStyle.C")
canv = r.TCanvas()

leg = r.TLegend(0.2,0.6,0.5,0.89)
leg.AddEntry(ll_mc, "LL MC", "LF")
leg.AddEntry(dd_mc, "DD MC", "LEP")
leg.AddEntry(ll_data, "LL Data", "LF")
leg.AddEntry(dd_data, "DD Data", "LEP")

ll_mc.GetYaxis().SetRangeUser(0.,maxi*1.2)
ll_mc.GetXaxis().SetTitle("BDT Output")
ll_mc.GetXaxis().SetLabelSize(0.05)
ll_mc.GetYaxis().SetLabelSize(0.05)
ll_mc.GetXaxis().SetTitleSize(0.05)
ll_mc.Draw("HIST")
ll_data.Draw("HISTsame")
dd_mc.Draw("LEPsame")
dd_data.Draw("LEPsame")
leg.Draw("same")

canv.Update()
canv.Modified()
raw_input()
canv.Print("plots/bdtoutputLLDD.pdf")

tf.Close()

