import ROOT as r
r.gROOT.ProcessLine(".x ~/Scratch/lhcb/lhcbStyle.C")

with_bdt_cut = True

tf = r.TFile("AnalysisOutAllProcsNoBDTCut.root")
if with_bdt_cut:
  tf = r.TFile("AnalysisOutAllProcsWithBDTCut.root")

th1f_kstgmc         = r.TH1F("kstgmc","",50,0,100)
th1f_kstgdata_win   = r.TH1F("kstgdata_win","",50,0,100)
th1f_kstgdata_sideb = r.TH1F("kstgdata_sideb","",50,0,100)

tree = tf.Get("AnalysisTree")
tree.Draw("gamgams_MM>>kstgmc",        "(itype==-88)","goff")
tree.Draw("gamgams_MM>>kstgdata_win",  "(itype==72 || itype==82) && (B0_MM>5200) && (B0_MM<5350)","goff")
tree.Draw("gamgams_MM>>kstgdata_sideb","(itype==72 || itype==82) && (B0_MM>5350)","goff")

th1f_kstgmc.Scale(1./th1f_kstgmc.Integral())
th1f_kstgdata_win.Scale(1./th1f_kstgdata_win.Integral())
th1f_kstgdata_sideb.Scale(1./th1f_kstgdata_sideb.Integral())

th1f_kstgmc.SetLineColor(r.kRed)
th1f_kstgdata_win.SetLineColor(r.kBlue)
th1f_kstgdata_sideb.SetLineColor(r.kGreen+1)

th1f_kstgmc.SetLineWidth(2)
th1f_kstgdata_win.SetLineWidth(2)
th1f_kstgdata_sideb.SetLineWidth(2)

th1f_kstgmc.GetXaxis().SetTitle("m(e^{+}e^{-}) MeV")
th1f_kstgmc.GetXaxis().SetTitleOffset(0.8)

canv = r.TCanvas()

th1f_kstgmc.Draw("HIST")
th1f_kstgdata_win.Draw("HISTsame")
th1f_kstgdata_sideb.Draw("HISTsame")

leg = r.TLegend(0.6,0.6,0.89,0.9)
leg.SetFillColor(0)
leg.SetLineColor(0)
leg.AddEntry(th1f_kstgmc,"KstGam MC","L")
leg.AddEntry(th1f_kstgdata_win,"KstGam Data Wind","L")
leg.AddEntry(th1f_kstgdata_sideb,"KstGam Data Sideb","L")
leg.Draw("same")

canv.Update()
canv.Modified()
if with_bdt_cut:
  canv.Print("plots/mee_kstg_withbdt.pdf")
else:
  canv.Print("plots/mee_kstg_nobdt.pdf")
raw_input()
