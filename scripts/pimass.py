import ROOT as r

tf = r.TFile('nTuples/Bd2KstGamma_data_2012.root')
tree = tf.Get("JpsiGamma_Tuple/DecayTree")

h = r.TH1F("h","",200,600,1400)

for ev in range(tree.GetEntries()):

  tree.GetEntry(ev)
  p4_pi = r.TLorentzVector(tree.piminus_PX,tree.piminus_PY,tree.piminus_PZ,r.TMath.Sqrt((497.**2)+(tree.piminus_P**2)))
  p4_K = r.TLorentzVector(tree.Kplus_PX,tree.Kplus_PY,tree.Kplus_PZ,tree.Kplus_PE)

  inv_m = p4_pi + p4_K

  if (ev%10000==0):
    print ev, inv_m.M()
  h.Fill(inv_m.M())

canv = r.TCanvas()
h.Draw()
canv.Update()
canv.Modified()
raw_input()
