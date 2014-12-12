#!/usr/bin/env python
from math import *
import ROOT as r

bdt_cut_vals    =  [0.0, 0.1, 0.2,0.25,0.3,0.35,0.4]
mc_fit_sigmas   =  [ 28.6752, 25.7633, 27.6419, 24.0047, 20.9643, 24.3379, 21.9366 ]
mc_fit_errors  =  [ 3.3796, 2.95666,  2.70223, 3.71541, 4.3329, 6.20311, 6.96611 ]
data_fit_sigmas =  [ 72.5 , 41.9961, 45.9382, 38.0295, 31.5004, 28.8532, 30.6819 ]
data_fit_errors =  [ 5.27557, 3.07669, 3.81999, 3.51817, 3.0187, 3.52031, 5.68583 ]


gr = r.TGraphErrors()
#gr = r.TGraph()

for p, val in enumerate(bdt_cut_vals):
   if val>0.35: continue
   x = val
   y = data_fit_sigmas[p]/mc_fit_sigmas[p]
   yerr = y * sqrt((mc_fit_errors[p]/mc_fit_sigmas[p])**2 + (data_fit_errors[p]/data_fit_sigmas[p])**2)
   gr.SetPoint(p,x,y)
   gr.SetPointError(p,0.0,yerr)

r.gROOT.ProcessLine(".x /Users/matt/Scratch/lhcb/lhcbStyle.C")
canv = r.TCanvas()
gr.SetMarkerStyle(r.kFullCircle)
gr.SetMarkerSize(1.2)
gr.SetLineColor(r.kRed)
gr.SetFillColor(r.kRed)
gr.SetFillStyle(1003)
gr.SetLineWidth(3)
gr.GetXaxis().SetTitle("BDT cut value")
gr.GetYaxis().SetTitle("Resolution scale factor (data/MC)")
gr.GetXaxis().SetTitleOffset(0.8)
gr.GetYaxis().SetTitleOffset(0.8)
gr.GetYaxis().SetTitleSize(0.8*gr.GetYaxis().GetTitleSize())
gr.GetXaxis().SetTitleSize(0.8*gr.GetXaxis().GetTitleSize())
gr.Draw("AP")
gr.GetXaxis().SetRangeUser(-1.,0.4)
canv.Update()
canv.Modified()
canv.Print("plots/resolution.pdf")
raw_input()
