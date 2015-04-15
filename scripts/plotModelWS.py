#!/usr/bin/env python

import ROOT as r
import sys

r.gROOT.ProcessLine('.x ~/Scratch/lhcb/lhcbStyle.C')

if len(sys.argv)<=1: fname = 'model_ws.root'
else: fname = sys.argv[1]

tf = r.TFile(fname)
w = tf.Get('model_ws')


w.var('B0_MM').SetTitle('m(#mu^{+}#mu^{-}#gamma)')
w.var('BF').setVal(1.e-7)

w.pdf('simpdf').fitTo(w.data('data'))

w.Print()

plot = w.var('B0_MM').frame(r.RooFit.Range(4500,6000))

leg = r.TLegend(0.6,0.3,0.92,0.92)
leg.SetEntrySeparation(0.25)

w.data('data').plotOn(plot,r.RooFit.Binning(50))
leg.AddEntry( plot.getObject(int(plot.numItems())-1),"Data","LEP")

w.pdf('simpdf').plotOn(plot,r.RooFit.ProjWData(r.RooArgSet(w.cat('tracktype')),w.data('data')),r.RooFit.Components("bkg0_DD,bkg0_LL"),r.RooFit.LineColor(r.kGreen+1),r.RooFit.LineStyle(r.kDashed))
leg.AddEntry( plot.getObject(int(plot.numItems())-1),"Combinatorial","L")

w.pdf('simpdf').plotOn(plot,r.RooFit.ProjWData(r.RooArgSet(w.cat('tracktype')),w.data('data')),r.RooFit.Components("sig0_DD,sig0_LL"),r.RooFit.LineColor(r.kRed),r.RooFit.LineStyle(r.kDashed))
leg.AddEntry( plot.getObject(int(plot.numItems())-1),"B_{s}#rightarrowJ/#psi#gamma","L")

w.pdf('simpdf').plotOn(plot,r.RooFit.ProjWData(r.RooArgSet(w.cat('tracktype')),w.data('data')),r.RooFit.Components("pr0_DD,pr0_LL"),r.RooFit.LineColor(r.kBlue-7),r.RooFit.LineStyle(r.kDashed))
leg.AddEntry( plot.getObject(int(plot.numItems())-1),"B_{d}#rightarrowJ/#psi#pi^{0}","L")

w.pdf('simpdf').plotOn(plot,r.RooFit.ProjWData(r.RooArgSet(w.cat('tracktype')),w.data('data')),r.RooFit.Components("pr1_DD,pr1_LL"),r.RooFit.LineColor(r.kOrange+1),r.RooFit.LineStyle(r.kDashed))
leg.AddEntry( plot.getObject(int(plot.numItems())-1),"B_{d}#rightarrowJ/#psiK_{S}","L")

w.pdf('simpdf').plotOn(plot,r.RooFit.ProjWData(r.RooArgSet(w.cat('tracktype')),w.data('data')),r.RooFit.Components("pr2_DD,pr2_LL"),r.RooFit.LineColor(r.kGreen+3),r.RooFit.LineStyle(r.kDashed))
leg.AddEntry( plot.getObject(int(plot.numItems())-1),"B_{u}^{+}#rightarrowJ/#psiK^{*+}","L")

w.pdf('simpdf').plotOn(plot,r.RooFit.ProjWData(r.RooArgSet(w.cat('tracktype')),w.data('data')),r.RooFit.Components("pr3_DD,pr3_LL"),r.RooFit.LineColor(r.kViolet),r.RooFit.LineStyle(r.kDashed))
leg.AddEntry( plot.getObject(int(plot.numItems())-1),"B_{s}#rightarrowJ/#psi#eta","L")

w.pdf('simpdf').plotOn(plot,r.RooFit.ProjWData(r.RooArgSet(w.cat('tracktype')),w.data('data')),r.RooFit.Components("pr5_DD,pr5_LL"),r.RooFit.LineColor(r.kYellow+1),r.RooFit.LineStyle(r.kDashed))
leg.AddEntry( plot.getObject(int(plot.numItems())-1),"B_{u}^{+}#rightarrowJ/#psi#rho^{+}","L")

w.pdf('simpdf').plotOn(plot,r.RooFit.ProjWData(r.RooArgSet(w.cat('tracktype')),w.data('data')),r.RooFit.Components("pr6_DD,pr6_LL"),r.RooFit.LineColor(r.kBlue+2),r.RooFit.LineStyle(r.kDashed))
leg.AddEntry( plot.getObject(int(plot.numItems())-1),"B_{d}#rightarrowJ/#psi#eta","L")

w.pdf('simpdf').plotOn(plot,r.RooFit.ProjWData(r.RooArgSet(w.cat('tracktype')),w.data('data')))
leg.AddEntry( plot.getObject(int(plot.numItems())-1),"Total","L")

plot.GetXaxis().SetTitleSize(0.8*plot.GetXaxis().GetTitleSize())
plot.GetYaxis().SetTitleSize(0.8*plot.GetYaxis().GetTitleSize())
plot.GetXaxis().SetTitleOffset(1.)
plot.GetYaxis().SetTitleOffset(1.)


pave = r.TPaveText(0.22,0.8,0.45,0.9,"ndc")
pave.SetFillColor(0)
pave.SetShadowColor(0)
pave.SetLineColor(0)
pave.SetTextSize(0.075)
pave.SetTextAlign(11)
pave.AddText("LHCb Preliminary")
pave2 = r.TPaveText(0.22,0.75,0.45,0.8,"ndc")
pave2.SetFillColor(0)
pave2.SetShadowColor(0)
pave2.SetLineColor(0)
pave2.SetTextAlign(11)
pave2.SetTextSize(0.05)
pave2.AddText("L = 3fb^{-1}")

canv = r.TCanvas("c","c",800,1200)
canv.Divide(1,2)
canv.cd(1)
plot.Draw()
leg.Draw("same")
pave.Draw("same")
#pave2.Draw("same")
canv.Update()
canv.Modified()
canv.cd(2)
log_plot = plot.Clone('log_plot')
log_plot.GetYaxis().SetRangeUser(0.1,200)
log_plot.Draw()
r.gPad.SetLogy()
canv.Update()
canv.Modified()
import os
os.system('mkdir -p plots/ModelWS/pdf')
canv.Print("plots/ModelWS/pdf/plot.pdf")

raw_input('Done\n')
