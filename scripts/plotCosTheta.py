import ROOT as r
r.gROOT.SetBatch()
r.gROOT.ProcessLine(".x ~/Scratch/lhcb/lhcbStyle.C")

tf = r.TFile("AnalysisOutAllProcsNoBDTCut.root")

th1f_jpsipiz = r.TH1F("jpsipiz","",25,-1,1)
th1f_jpsigam = r.TH1F("jpsigam","",25,-1,1)

tree = tf.Get("AnalysisTree")

tree.Draw("muminus_CosTheta>>jpsigam","(itype==-81)","goff")
tree.Draw("muminus_CosTheta>>jpsipiz","(itype==-82)","goff")

th1f_jpsigam.Scale(1./th1f_jpsigam.Integral())
th1f_jpsipiz.Scale(1./th1f_jpsipiz.Integral())

th1f_jpsigam.SetLineColor(r.kRed)
th1f_jpsipiz.SetLineColor(r.kBlue)

th1f_jpsigam.SetLineWidth(2)
th1f_jpsipiz.SetLineWidth(2)

th1f_jpsigam.GetXaxis().SetTitle("#mu^{-} cos(#theta)")
th1f_jpsigam.GetXaxis().SetTitleOffset(0.8)

canv = r.TCanvas()

th1f_jpsigam.Draw("HIST")
th1f_jpsipiz.Draw("HISTsame")

leg = r.TLegend(0.4,0.4,0.69,0.7)
leg.SetFillColor(0)
leg.SetLineColor(0)
leg.AddEntry(th1f_jpsigam,"B_{s} #rightarrow J/#psi#gamma MC","L")
leg.AddEntry(th1f_jpsipiz,"B_{d} #rightarrow J/#psi#pi^{0} MC","L")
leg.Draw("same")

canv.Update()
canv.Modified()
canv.Print("plots/costheta.pdf")
canv.Print("plots/costheta.png")
raw_input()
