import ROOT as r

r.gROOT.ProcessLine('.x ~/Scratch/lhcb/lhcbStyle.C')
r.TH1.SetDefaultSumw2()

tfs = {
    "2011": r.TFile("nTuples/WithBremInfo/Bd2KstGamma_2011.root") ,
    "2012": r.TFile("nTuples/WithBremInfo/Bd2KstGamma_2012.root") ,
    "MC":   r.TFile("nTuples/WithBremInfo/Bd2KstGamma_MC.root") ,
    }

colors = {
    "2011": r.kRed ,
    "2012": r.kBlue ,
    "MC": r.kBlack
    }

trees = {}
for name, fil in tfs.iteritems():
  trees[name] = fil.Get('JpsiGamma_Tuple/DecayTree')


l0_string   = "(B0_L0HadronDecision_TOS || B0_L0ElectronDecision_TOS || B0_L0Global_TIS)"
hlt1_string = "(B0_Hlt1TrackAllL0Decision_TOS || B0_Hlt1TrackPhotonDecision_TOS)"
hlt2_string = "(B0_Hlt2RadiativeTopoPhotonDecision_TOS || B0_Hlt2Topo2BodyBBDTDecision_TOS || B0_Hlt2Topo3BodyBBDTDecision_TOS || B0_Hlt2Topo4BodyBBDTDecision_TOS || B0_Hlt2TopoE2BodyBBDTDecision_TOS || B0_Hlt2TopoE3BodyBBDTDecision_TOS || B0_Hlt2TopoE4BodyBBDTDecision_TOS)"
pid_string = "(Kplus_ProbNNk>0.15 && (piminus_ProbNNpi*(1.-piminus_ProbNNk)*(1.-piminus_ProbNNp))>0.10)"
presel_string = '(B0_DIRA_OWNPV>0.9999 && B0_PT>2000 && gamgams_PT>1000 && B0_TAU>0)'

l0_cut     = l0_string
hlt1_cut   = l0_string + ' && ' + hlt1_string
hlt2_cut   = l0_string + ' && ' + hlt1_string + ' && ' + hlt2_string
pid_cut    = l0_string + ' && ' + hlt1_string + ' && ' + hlt2_string + ' && ' + pid_string
presel_cut = l0_string + ' && ' + hlt1_string + ' && ' + hlt2_string + ' && ' + pid_string + ' && ' + presel_string

hists = {}
for name in tfs.keys():
  hists[name] = r.TH1F('h_%s'%name,'',3,0,3)

for name, tree in trees.iteritems():
  tree.Draw("gamgams_eplus_StandaloneBremMultiplicity>>+h_%s"%name,presel_cut,"GOFF")

for name, hist in hists.iteritems():
  hist.Scale(1./hist.Integral())
  hist.SetLineWidth(2)
  hist.SetLineColor(colors[name])
  hist.SetMarkerColor(colors[name])

canv = r.TCanvas()
hists["2011"].GetYaxis().SetRangeUser(0.,0.7)
hists["2011"].GetXaxis().SetTitle("Brem Multiplicity")
hists["2011"].GetXaxis().SetTitleOffset(0.8)
hists["2011"].GetYaxis().SetTitle("Fraction")
hists["2011"].GetYaxis().SetTitleOffset(0.8)

hists["2011"].Draw("LEP")
hists["2012"].Draw("LEPsame")
hists["MC"].Draw("LEPsame")

leg = r.TLegend(0.7,0.6,0.92,0.92)
leg.SetFillColor(0)
leg.AddEntry(hists["2011"],"2011","LEP")
leg.AddEntry(hists["2012"],"2012","LEP")
leg.AddEntry(hists["MC"],"MC","LEP")
leg.Draw("same")
canv.Update()
canv.Modified()
canv.Print("plots/brem.pdf")
raw_input()
