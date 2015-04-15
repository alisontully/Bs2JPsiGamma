#!/usr/bin/env python

import ROOT as r
r.gROOT.SetBatch()
r.gROOT.ProcessLine(".x ~/Scratch/lhcb/lhcbStyle.C")

with_bdt_cut = True

tf = r.TFile('AnalysisOutAllProcsNoBDTCut.root')
tree = tf.Get('AnalysisTree')

m_ee_kstg_LL     = r.TH1F('m_ee_kstg_LL','',50,0,100)
m_ee_jpsig_LL    = r.TH1F('m_ee_jpsig_LL','',50,0,100)
m_mumu_jpsig_LL  = r.TH1F('m_mumu_jpsig_LL','',50,3000,3200)
m_kpi_kstg_LL    = r.TH1F('m_kpikpi_kstg_LL','',50,800,1000)
m_ee_kstg_DD     = r.TH1F('m_ee_kstg_DD','',50,0,100)
m_ee_jpsig_DD    = r.TH1F('m_ee_jpsig_DD','',50,0,100)
m_mumu_jpsig_DD  = r.TH1F('m_mumu_jpsig_DD','',50,3000,3200)
m_kpi_kstg_DD    = r.TH1F('m_kpikpi_kstg_DD','',50,800,1000)

if with_bdt_cut:
  tree.Draw("Jpsi_MM>>m_mumu_jpsig_LL", "(itype==71 || itype==81) && (pass_bdt) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)","goff")
  tree.Draw("Jpsi_MM>>m_kpikpi_kstg_LL","(itype==72 || itype==82) && (pass_bdt) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)","goff")
  tree.Draw("gamgams_MM>>m_ee_jpsig_LL","(itype==71 || itype==81) && (pass_bdt) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)","goff")
  tree.Draw("gamgams_MM>>m_ee_kstg_LL", "(itype==72 || itype==82) && (pass_bdt) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)","goff")
  tree.Draw("Jpsi_MM>>m_mumu_jpsig_DD", "(itype==71 || itype==81) && (pass_bdt) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)","goff")
  tree.Draw("Jpsi_MM>>m_kpikpi_kstg_DD","(itype==72 || itype==82) && (pass_bdt) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)","goff")
  tree.Draw("gamgams_MM>>m_ee_jpsig_DD","(itype==71 || itype==81) && (pass_bdt) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)","goff")
  tree.Draw("gamgams_MM>>m_ee_kstg_DD", "(itype==72 || itype==82) && (pass_bdt) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)","goff")
else:
  tree.Draw("Jpsi_MM>>m_mumu_jpsig_LL", "(itype==71 || itype==81) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)","goff")
  tree.Draw("Jpsi_MM>>m_kpikpi_kstg_LL","(itype==72 || itype==82) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)","goff")
  tree.Draw("gamgams_MM>>m_ee_jpsig_LL","(itype==71 || itype==81) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)","goff")
  tree.Draw("gamgams_MM>>m_ee_kstg_LL", "(itype==72 || itype==82) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)","goff")
  tree.Draw("Jpsi_MM>>m_mumu_jpsig_DD", "(itype==71 || itype==81) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)","goff")
  tree.Draw("Jpsi_MM>>m_kpikpi_kstg_DD","(itype==72 || itype==82) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)","goff")
  tree.Draw("gamgams_MM>>m_ee_jpsig_DD","(itype==71 || itype==81) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)","goff")
  tree.Draw("gamgams_MM>>m_ee_kstg_DD", "(itype==72 || itype==82) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)","goff")

m_ee_jpsig_LL.SetFillColor(r.kRed)
m_ee_kstg_LL.SetFillColor(r.kRed)
m_mumu_jpsig_LL.SetFillColor(r.kRed)
m_kpi_kstg_LL.SetFillColor(r.kRed)
m_ee_jpsig_DD.SetFillColor(r.kBlue)
m_ee_kstg_DD.SetFillColor(r.kBlue)
m_mumu_jpsig_DD.SetFillColor(r.kBlue)
m_kpi_kstg_DD.SetFillColor(r.kBlue)

m_ee_jpsig = r.THStack("m_ee_jpsig","")
m_ee_jpsig.Add(m_ee_jpsig_LL)
m_ee_jpsig.Add(m_ee_jpsig_DD)

m_ee_kstg = r.THStack("m_ee_kstg","")
m_ee_kstg.Add(m_ee_kstg_LL)
m_ee_kstg.Add(m_ee_kstg_DD)

m_mumu_jpsig = r.THStack("m_mumu_jpsig","")
m_mumu_jpsig.Add(m_mumu_jpsig_LL)
m_mumu_jpsig.Add(m_mumu_jpsig_DD)

m_kpi_kstg = r.THStack("m_kpi_kstg","")
m_kpi_kstg.Add(m_kpi_kstg_LL)
m_kpi_kstg.Add(m_kpi_kstg_DD)

pave_jpsig = r.TPaveText(0.6,0.6,0.89,0.89,"ndc")
pave_jpsig.SetFillColor(0)
pave_jpsig.SetShadowColor(0)
pave_jpsig.SetLineColor(0)
pave_jpsig.AddText("B_{s} #rightarrow J/#psi#gamma")

pave_kstg = r.TPaveText(0.6,0.6,0.89,0.89,"ndc")
pave_kstg.SetFillColor(0)
pave_kstg.SetShadowColor(0)
pave_kstg.SetLineColor(0)
pave_kstg.AddText("B_{d} #rightarrow K^{*}#gamma")

leg = r.TLegend(0.3,0.5,0.8,0.8)
if not with_bdt_cut:
  leg = r.TLegend(0.45,0.5,0.8,0.8)
leg.SetFillColor(0)
leg.SetLineColor(0)
leg.AddEntry(m_ee_jpsig_LL,"LL","LF")
leg.AddEntry(m_ee_jpsig_DD,"DD","LF")

canv = r.TCanvas("c","c",1600,1200)
canv.Divide(2,2)
canv.cd(1)
m_ee_jpsig.Draw()
leg.Draw('same')
pave_jpsig.Draw('same')
m_ee_jpsig.GetXaxis().SetTitle('m(e^{+}e^{-}) MeV')
m_ee_jpsig.GetXaxis().SetTitleOffset(0.8)
canv.Update()
canv.Modified()
canv.cd(2)
m_ee_kstg.Draw()
pave_kstg.Draw('same')
m_ee_kstg.GetXaxis().SetTitle('m(e^{+}e^{-}) MeV')
m_ee_kstg.GetXaxis().SetTitleOffset(0.8)
canv.Update()
canv.Modified()
canv.cd(3)
m_mumu_jpsig.Draw()
pave_jpsig.Draw('same')
m_mumu_jpsig.GetXaxis().SetTitle('m(#mu^{+}#mu^{-}) MeV')
m_mumu_jpsig.GetXaxis().SetTitleOffset(0.8)
canv.Update()
canv.Modified()
canv.cd(4)
m_kpi_kstg.Draw()
pave_kstg.Draw('same')
m_kpi_kstg.GetXaxis().SetTitle('m(#pi^{+}K^{-}) MeV')
m_kpi_kstg.GetXaxis().SetTitleOffset(0.8)
canv.Update()
canv.Modified()

if with_bdt_cut:
  canv.Print("plots/inv_mass_daughters.pdf")
  canv.Print("plots/inv_mass_daughters.png")
else:
  canv.Print("plots/inv_mass_daughters_nobdt.pdf")
  canv.Print("plots/inv_mass_daughters_nobdt.png")

raw_input()
