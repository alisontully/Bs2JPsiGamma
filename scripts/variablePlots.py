#!/usr/bin/env python

import os
os.system('mkdir -p plots/vars/pdf')
import ROOT as r
tf = r.TFile("AnalysisOut.root")

r.gROOT.ProcessLine('.x ~/Scratch/lhcb/lhcbStyle.C')

tree = tf.Get('AnalysisTree')

c1 = r.TCanvas()
h1 = r.TH1F('h1','h1',100,3000,3200)
h1.SetStats(0)
tree.Draw('Jpsi_MM>>h1','(itype==71 || itype==81) && (pass_bdt)','goff')
h1.GetXaxis().SetTitle('m(#mu^{-}#mu^{+})')
h1.GetXaxis().SetTitleOffset(0.8)

pave = r.TPaveText(0.6,0.77,0.9,0.9,"ndc")
pave.SetFillColor(0)
pave.SetShadowColor(0)
pave.SetLineColor(0)
pave.SetTextSize(0.075)
pave.SetTextAlign(11)
pave.AddText("B_{s} #rightarrow J/#psi#gamma Data")
h1.Draw()
pave.Draw("same")
c1.Update()
c1.Modified()
c1.Print("plots/vars/pdf/jpsim.pdf")

c2 = r.TCanvas()
h2 = r.TH1F('h2','h2',50,750,1050)
h2.SetStats(0)
tree.Draw('Jpsi_MM>>h2','(itype==72 || itype==82) && (pass_bdt)','goff')
h2.GetXaxis().SetTitle('m(K^{-}#pi^{+})')
h2.GetXaxis().SetTitleOffset(0.8)

pave = r.TPaveText(0.6,0.77,0.9,0.9,"ndc")
pave.SetFillColor(0)
pave.SetShadowColor(0)
pave.SetLineColor(0)
pave.SetTextSize(0.075)
pave.SetTextAlign(11)
pave.AddText("B_{d} #rightarrow K^{*}#gamma Data")
h2.Draw()
pave.Draw("same")
c2.Update()
c2.Modified()
c2.Print("plots/vars/pdf/kstm.pdf")

c3 = r.TCanvas()
h3 = r.TH1F('h3','h3',50,0,100)
h3.SetStats(0)
tree.Draw('gamgams_MM>>h3','(itype==72 || itype==82) && (pass_bdt)','goff')
h3.GetXaxis().SetTitle('m(e^{-}e^{+})')
h3.GetXaxis().SetTitleOffset(0.8)

pave = r.TPaveText(0.6,0.77,0.9,0.9,"ndc")
pave.SetFillColor(0)
pave.SetShadowColor(0)
pave.SetLineColor(0)
pave.SetTextSize(0.075)
pave.SetTextAlign(11)
pave.AddText("B_{d} #rightarrow K^{*}#gamma Data")
h3.Draw()
pave.Draw("same")
c3.Update()
c3.Modified()
c3.Print("plots/vars/pdf/kst_mee.pdf")

c4 = r.TCanvas()
h4 = r.TH1F('h4','h4',50,0,100)
h4.SetStats(0)
tree.Draw('gamgams_MM>>h4','(itype==71 || itype==81) && (pass_bdt)','goff')
h4.GetXaxis().SetTitle('m(e^{-}e^{+})')
h4.GetXaxis().SetTitleOffset(0.8)

pave = r.TPaveText(0.6,0.77,0.9,0.9,"ndc")
pave.SetFillColor(0)
pave.SetShadowColor(0)
pave.SetLineColor(0)
pave.SetTextSize(0.075)
pave.SetTextAlign(11)
pave.AddText("B_{s} #rightarrow J/#psi#gamma Data")
h4.Draw()
pave.Draw("same")
c4.Update()
c4.Modified()
c4.Print("plots/vars/pdf/jpsi_mee.pdf")





raw_input()
