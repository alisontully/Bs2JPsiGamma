import ROOT as r
r.gROOT.SetBatch()
r.gROOT.ProcessLine(".x ~/Scratch/lhcb/lhcbStyle.C")

tf_std = r.TFile('AnalysisOutAllProcsNoBDTCut.root')
tf_alt = r.TFile('AnalysisOutAltTraining.root')

tree_std = tf_std.Get('AnalysisTree')
tree_alt = tf_alt.Get('AnalysisTree')

th1f_std = r.TH1F('std','',100,4550,7000)
th1f_alt = r.TH1F('alt','',100,4550,7000)

tree_std.Draw("B0_MM>>std","(itype==71 || itype==81) && (pass_bdt)","goff")
tree_alt.Draw("B0_MM>>alt","(itype==71 || itype==81) && (pass_bdt)","goff")

th1f_std.SetMarkerColor(r.kBlue)
th1f_alt.SetMarkerColor(r.kRed)
th1f_std.SetMarkerStyle(r.kFullSquare)
th1f_alt.SetMarkerStyle(r.kOpenCircle)
th1f_std.SetMarkerSize(0.8)
th1f_alt.SetMarkerSize(0.8)
th1f_std.SetLineColor(r.kBlue-3)
th1f_alt.SetLineColor(r.kRed)
th1f_std.SetLineWidth(2)
th1f_alt.SetLineWidth(3)

th1f_std.GetXaxis().SetTitle('m(#mu^{+}#mu^{-}#gamma) MeV')
th1f_std.GetXaxis().SetTitleOffset(0.8)
th1f_std.GetYaxis().SetTitle("Events / 25 MeV")
th1f_std.GetYaxis().SetTitleOffset(0.8)

leg = r.TLegend(0.3,0.6,0.9,0.9)
leg.SetFillColor(0)
leg.SetLineColor(0)
leg.SetTextSize(0.04)
leg.AddEntry(th1f_std,"Standard BDT (%d evs.)"%th1f_std.GetEntries(),"LEP")
leg.AddEntry(th1f_alt,"BDT w/ FD Chi2 wo/ #mu cos(#theta) (%d evs.)"%th1f_alt.GetEntries(),"LEP")

canv = r.TCanvas()
th1f_std.Draw("LEP")
th1f_alt.Draw("LEPsame")
leg.Draw("same")
canv.Update()
canv.Modified()
canv.Print("plots/bdt_check.pdf")
canv.Print("plots/bdt_check.png")
raw_input()
