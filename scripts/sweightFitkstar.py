#!/usr/bin/env python

from ROOT import TFile, TCanvas
from ROOT import RooDataSet, RooWorkspace, RooArgSet

tf = TFile.Open('AnalysisOut.root')
tree = tf.Get('AnalysisTree')

ws = RooWorkspace("w","w")
observables = RooArgSet()
ws.defineSet("observables",observables)

ws.factory("mass[5050,6000]")
getattr(ws,'set')("observables").add(ws.var("mass"))
ws.factory("gamgams_pt[0,40e3]")
getattr(ws,'set')("observables").add(ws.var("gamgams_pt"))


mc = RooDataSet('mc','',getattr(ws,'set')('observables'))
data = RooDataSet('data','',getattr(ws,'set')('observables'))

for ev in range(tree.GetEntries()):
  tree.GetEntry(ev)
  if tree.itype != -88 and tree.itype!=72 and tree.itype!=82: continue
  if tree.bdtoutput<0.2: continue
  if tree.B0_MM < ws.var("mass").getMin() or tree.B0_MM > ws.var("mass").getMax(): continue

  ws.var("mass").setVal(tree.B0_MM)
  ws.var("gamgams_pt").setVal(tree.gamgams_PT)

  if tree.itype == -88:
    mc.add(getattr(ws,'set')('observables'))
  if tree.itype == 72 or tree.itype == 82:
    data.add(getattr(ws,'set')('observables'))

getattr(ws,'import')(mc)
getattr(ws,'import')(data)

ws.Print()

ws.factory("mean[5200,5300]")
ws.factory("CBShape:sig_cb1(mass,mean,sigma_1[10,200],alpha_1[0,1.],n_1[0.,50.])");
ws.factory("CBShape:sig_cb2(mass,mean,sigma_2[10,200],alpha_2[-1.,0.],n_2[0.,50.])");
ws.factory("SUM::mcsig( f[0.5,0,1]*sig_cb1, sig_cb2)");

ws.pdf("mcsig").fitTo(ws.data("mc"))

mcplot = ws.var("mass").frame()
ws.data("mc").plotOn(mcplot)
ws.pdf("mcsig").plotOn(mcplot)

# freeze obs
argset = ws.pdf("mcsig").getParameters(RooArgSet(ws.var("mass")))
ws.defineSet("signal_params",argset)
#RooArgSet(getattr(ws,'set')("signal_params")).setAttribAll("Constant")

ws.factory("Exponential:bkg(mass,exp_p1[-0.02,0.])")
ws.factory("SUM:pdf( sig_y[0,1000]*mcsig, bkg_y[0,2000]*bkg )")

ws.pdf("pdf").fitTo(ws.data("data"))

dataplot = ws.var("mass").frame()
ws.data("data").plotOn(dataplot)
ws.pdf("pdf").plotOn(dataplot)

canv = TCanvas("c","c",1200,600)
canv.Divide(2,1)
canv.cd(1)
mcplot.Draw()
canv.cd(2)
dataplot.Draw()
canv.Update()
canv.Modified()
canv.Print("plots/kstarfit.pdf")

print ws.data("mc").numEntries()
print ws.data("data").numEntries()
raw_input()


