import ROOT as r

r.gROOT.SetBatch()
r.gROOT.ProcessLine(".x ~/Scratch/lhcb/lhcbStyle.C")

def cut_flow_plot(is_kst):

  tf = r.TFile("nTuples/Bs2JpsiGamma_2012.root")
  if is_kst:
    tf = r.TFile("nTuples/Bd2KstGamma_2012.root")

  tree = tf.Get("JpsiGamma_Tuple/DecayTree")

  l0_string   = "(B0_L0MuonDecision_TOS || B0_L0DiMuonDecision_TOS)"
  hlt1_string = "(B0_Hlt1TrackMuonDecision_TOS)"
  hlt2_string = "(B0_Hlt2DiMuonDetachedDecision_TOS || B0_Hlt2DiMuonDetachedHeavyDecision_TOS || B0_Hlt2DiMuonDetachedJPsiDecision_TOS || B0_Hlt2SingleMuonDecision_TOS || B0_Hlt2Topo2BodyBBDTDecision_TOS || B0_Hlt2Topo3BodyBBDTDecision_TOS || B0_Hlt2TopoMu2BodyBBDTDecision_TOS || B0_Hlt2TopoMu3BodyBBDTDecision_TOS)"
  pid_string = ""
  presel_string = '(B0_DIRA_OWNPV>0.9999 && B0_PT>2000 && gamgams_PT>1000 && B0_TAU>0)'
  if is_kst:
    l0_string   = "(B0_L0HadronDecision_TOS || B0_L0ElectronDecision_TOS || B0_L0Global_TIS)"
    hlt1_string = "(B0_Hlt1TrackAllL0Decision_TOS || B0_Hlt1TrackPhotonDecision_TOS)"
    hlt2_string = "(B0_Hlt2RadiativeTopoPhotonDecision_TOS || B0_Hlt2Topo2BodyBBDTDecision_TOS || B0_Hlt2Topo3BodyBBDTDecision_TOS || B0_Hlt2Topo4BodyBBDTDecision_TOS || B0_Hlt2TopoE2BodyBBDTDecision_TOS || B0_Hlt2TopoE3BodyBBDTDecision_TOS || B0_Hlt2TopoE4BodyBBDTDecision_TOS)"
    pid_string = "(Kplus_ProbNNk>0.15 && (piminus_ProbNNpi*(1.-piminus_ProbNNk)*(1.-piminus_ProbNNp))>0.10)"
    presel_string = '(B0_DIRA_OWNPV>0.9999 && B0_PT>2000 && gamgams_PT>1000 && B0_TAU>0)'

  l0_cut     = l0_string
  hlt1_cut   = l0_string + ' && ' + hlt1_string
  hlt2_cut   = l0_string + ' && ' + hlt1_string + ' && ' + hlt2_string
  pid_cut = hlt2_cut
  presel_cut = l0_string + ' && ' + hlt1_string + ' && ' + hlt2_string + ' && ' + presel_string
  if is_kst:
    pid_cut    = l0_string + ' && ' + hlt1_string + ' && ' + hlt2_string + ' && ' + pid_string
    presel_cut = l0_string + ' && ' + hlt1_string + ' && ' + hlt2_string + ' && ' + pid_string + ' && ' + presel_string

  entries = tree.GetEntries()
  l0_entries = tree.GetEntries(l0_cut)
  hlt1_entries = tree.GetEntries(hlt1_cut)
  hlt2_entries = tree.GetEntries(hlt2_cut)
  pid_entries  = tree.GetEntries(pid_cut)
  presel_entries = tree.GetEntries(presel_cut)

  eff        = float(entries)/float(entries)
  l0_eff     = float(l0_entries)/float(entries)
  hlt1_eff   = float(hlt1_entries)/float(entries)
  hlt2_eff   = float(hlt2_entries)/float(entries)
  pid_eff    = float(pid_entries)/float(entries)
  presel_eff = float(presel_entries)/float(entries)

  canv = r.TCanvas()
  canv.SetLeftMargin(0.15)

  th1f_nocut   = r.TH1F('nocut','nocut',75,4500,6000)
  th1f_l0cut   = r.TH1F('l0cut','l0cut',75,4500,6000)
  th1f_hlt1cut = r.TH1F('hlt1cut','hlt1cut',75,4500,6000)
  th1f_hlt2cut = r.TH1F('hlt2cut','hlt2cut',75,4500,6000)
  th1f_pidcut  = r.TH1F('pidcut','pidcut',75,4500,6000)
  th1f_preselcut = r.TH1F('preselcut','preselcut',75,4500,6000)

  tree.Draw('B0_MM>>nocut','','goff')
  tree.Draw('B0_MM>>l0cut',l0_cut,'goff')
  tree.Draw('B0_MM>>hlt1cut',hlt1_cut,'goff')
  tree.Draw('B0_MM>>hlt2cut',hlt2_cut,'goff')
  tree.Draw('B0_MM>>pidcut',pid_cut,'goff')
  tree.Draw('B0_MM>>preselcut',presel_cut,'goff')

  th1f_nocut.SetLineColor(r.kRed)
  th1f_l0cut.SetLineColor(r.kBlue)
  th1f_hlt1cut.SetLineColor(r.kGreen+1)
  th1f_hlt2cut.SetLineColor(r.kOrange)
  th1f_pidcut.SetLineColor(r.kMagenta)
  th1f_preselcut.SetLineColor(r.kBlue-7)

  th1f_nocut.SetLineWidth(2)
  th1f_l0cut.SetLineWidth(2)
  th1f_hlt1cut.SetLineWidth(2)
  th1f_hlt2cut.SetLineWidth(2)
  th1f_pidcut.SetLineWidth(2)
  th1f_preselcut.SetLineWidth(2)

  th1f_nocut.GetYaxis().SetRangeUser(0,10000)
  if is_kst:
    th1f_nocut.GetYaxis().SetRangeUser(0,23000)
  th1f_nocut.GetXaxis().SetTitle("m(#mu^{+}#mu^{-}#gamma) MeV")
  if is_kst:
    th1f_nocut.GetXaxis().SetTitle("m(K^{#pm}#pi^{#mp}#gamma) MeV")
  th1f_nocut.GetXaxis().SetTitleOffset(0.8)
  th1f_nocut.GetYaxis().SetTitle("Entries")
  th1f_nocut.GetYaxis().SetTitleOffset(0.9)

  th1f_nocut.Draw("HIST")
  th1f_l0cut.Draw("same")
  th1f_hlt1cut.Draw("same")
  th1f_hlt2cut.Draw("same")
  th1f_pidcut.Draw("same")
  th1f_preselcut.Draw("same")

  leg = r.TLegend(0.5,0.6,0.9,0.9)
  leg.SetFillColor(0)
  leg.SetTextSize(0.05)
  leg.AddEntry(th1f_nocut,    "No cut: %d : %5.2f%%"%(entries,eff*100),"L")
  leg.AddEntry(th1f_l0cut,    "L0 cut: %d : %5.2f%%"%(l0_entries,l0_eff*100),"L")
  leg.AddEntry(th1f_hlt1cut,  "Hlt1 cut: %d : %5.2f%%"%(hlt1_entries,hlt1_eff*100),"L")
  leg.AddEntry(th1f_hlt2cut,  "Hlt2 cut: %d : %5.2f%%"%(hlt2_entries,hlt2_eff*100),"L")
  leg.AddEntry(th1f_pidcut,   "PID cut: %d : %5.2f%%"%(pid_entries,pid_eff*100),"L")
  leg.AddEntry(th1f_preselcut,"Presel cut: %d : %5.2f%%"%(presel_entries,presel_eff*100),"L")
  leg.Draw("same")

  pave = r.TPaveText(0.35,0.8,0.5,0.9,"ndc")
  pave.SetFillColor(0);
  pave.SetShadowColor(0);
  pave.SetLineColor(0);
  pave.SetTextSize(0.08)
  title = "B_{s} #rightarrow J/#psi#gamma"
  if is_kst:
    title = "B_{d} #rightarrow K*#gamma"
  pave.AddText(title);
  pave.Draw("same")

  canv.Update()
  canv.Modified()
  name = 'JpsiGamma'
  if is_kst: name = 'KstGamma'
  canv.Print("plots/KstGammaFit/pdf/%sTrig.pdf"%name)
  canv.Print("plots/KstGammaFit/png/%sTrig.png"%name)
  canv.Print("plots/KstGammaFit/C/%sTrig.C"%name)

  canv2 = r.TCanvas()
  canv2.SetLeftMargin(0.15)

  th1f_hlt2cut.GetYaxis().SetRangeUser(0,8000)
  if is_kst:
    th1f_hlt2cut.GetYaxis().SetRangeUser(0,1800)
  th1f_hlt2cut.GetXaxis().SetTitle("m(#mu^{+}#mu^{-}#gamma) MeV")
  if is_kst:
    th1f_hlt2cut.GetXaxis().SetTitle("m(K^{#pm}#pi^{#mp}#gamma) MeV")
  th1f_hlt2cut.GetXaxis().SetTitleOffset(0.8)
  th1f_hlt2cut.GetYaxis().SetTitle("Entries")
  th1f_hlt2cut.GetYaxis().SetTitleOffset(0.9)

  leg2 = r.TLegend(0.58,0.62,0.89,0.9)
  leg2.SetFillColor(0)
  leg2.SetTextSize(0.04)
  leg2.AddEntry(th1f_hlt2cut,  "Hlt2 cut: %d : %5.2f%%"%(hlt2_entries,hlt2_eff*100),"L")
  leg2.AddEntry(th1f_pidcut,   "PID cut: %d : %5.2f%%"%(pid_entries,pid_eff*100),"L")
  leg2.AddEntry(th1f_preselcut,"Presel cut: %d : %5.2f%%"%(presel_entries,presel_eff*100),"L")

  th1f_hlt2cut.Draw("HIST")
  th1f_pidcut.Draw("same")
  th1f_preselcut.Draw("same")
  leg2.Draw("same")
  pave.Draw("same")
  canv2.Update()
  canv2.Modified()
  canv2.Print("plots/KstGammaFit/pdf/%sPID.pdf"%name)
  canv2.Print("plots/KstGammaFit/png/%sPID.png"%name)
  canv2.Print("plots/KstGammaFit/C/%sPID.C"%name)

  print 'NoCut: ', entries,        eff
  print 'L0:    ', l0_entries,     l0_eff
  print 'Hlt1:  ', hlt1_entries,   hlt1_eff
  print 'Hlt2:  ', hlt2_entries,   hlt2_eff
  print 'PID:   ', pid_entries,    pid_eff
  print 'PreSel:', presel_entries, presel_eff

  tf.Close()

  if is_kst:
    #### next bit
    #### comparison of MCs
    tf_mc_2012    = r.TFile("nTuples/Bd2KstGamma_MC_2012.root")
    tree_mc_2012  = tf_mc_2012.Get("JpsiGamma_Tuple/DecayTree")
    tf_mc_2011    = r.TFile("nTuples/Bd2KstGamma_MC_2011.root")
    tree_mc_2011  = tf_mc_2011.Get("JpsiGamma_Tuple/DecayTree")
    tf_mc_Sim06   = r.TFile("nTuples/Bd2KstGamma_MC_2012_noptcut.root")
    tree_mc_Sim06 = tf_mc_Sim06.Get("JpsiGamma_Tuple/DecayTree")

    settings = {
        'B0_MM':   { 'name' : 'B0_MM' ,
                     'title': 'm(K^{#pm}#pi^{#mp}#gamma) MeV' ,
                     'bins' : 50 ,
                     'xlow' : 5000,
                     'xhigh': 5500
                   } ,
        'gamgams_PT': { 'name' :  'gamgams_PT' ,
                        'title' : 'p_{T}(#gamma) MeV',
                        'bins' : 40 ,
                        'xlow' : 0,
                        'xhigh': 20e3
                      } ,
        'B0_DIRA_OWNPV': { 'name': 'B0_DIRA',
                           'title': 'DIRA(B_{d})',
                           'bins': 50 ,
                           'xlow': 0.9999,
                           'xhigh': 1.
                          }
                  }

    trees = {
        '2012 MC Sim08' :  tree_mc_2012 ,
        '2012 MC Sim06' :  tree_mc_Sim06 ,
        '2011 MC Sim08' :  tree_mc_2011
            }

    colors = [r.kRed, r.kBlue, r.kGreen+1]

    canvs = []
    hists = []
    for var, setting in settings.items():

      canvs.append(r.TCanvas())
      c = 0
      leg = r.TLegend(0.65,0.6,0.89,0.89)
      leg.SetFillColor(0)
      leg.SetLineColor(0)

      mc_keys = trees.keys()
      mc_keys.sort()
      for mc in mc_keys:
        tree = trees[mc]

        h_name = setting['name']+'_'+ (mc.replace(' ','_'))

        hists.append(r.TH1F(h_name, setting['name'], setting['bins'], setting['xlow'], setting['xhigh']))

        tree.Draw('%s>>%s'%(var,h_name), presel_cut, 'goff')

        hists[-1].Scale(1./hists[-1].Integral())

        hists[-1].SetLineColor(colors[c])
        hists[-1].SetLineWidth(2)
        hists[-1].GetXaxis().SetTitle(setting['title'])
        hists[-1].GetXaxis().SetTitleOffset(0.8)

        leg.AddEntry(hists[-1],mc,'L')

        if c==0:
          hists[-1].Draw("hist")
        else:
          hists[-1].Draw("same")

        c+=1

      leg.Draw("same")
      canvs[-1].Update()
      canvs[-1].Modified()
      canvs[-1].Print('plots/%s.pdf'%var)

    tf_mc_2012.Close()
    tf_mc_2011.Close()
    tf_mc_Sim06.Close()



cut_flow_plot(True)
cut_flow_plot(False)

