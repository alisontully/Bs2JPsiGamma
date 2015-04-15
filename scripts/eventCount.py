import ROOT as r

tf = r.TFile("AnalysisOutAllProcsNoBDTCut.root")
tree = tf.Get("AnalysisTree")

kst_mc     = tree.GetEntries("itype==-88")
jpsi_mc    = tree.GetEntries("itype==-81")
jpsipiz_mc = tree.GetEntries("itype==-82")
kst_data   = tree.GetEntries("itype==72 || itype==82")
jpsi_data  = tree.GetEntries("itype==71 || itype==81")

kst_mc_LL     = tree.GetEntries("(itype==-88) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)")
jpsi_mc_LL    = tree.GetEntries("(itype==-81) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)")
jpsipiz_mc_LL = tree.GetEntries("(itype==-82) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)")
kst_data_LL   = tree.GetEntries("(itype==72 || itype==82) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)")
jpsi_data_LL  = tree.GetEntries("(itype==71 || itype==81) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)")

kst_mc_DD     = tree.GetEntries("(itype==-88) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)")
jpsi_mc_DD    = tree.GetEntries("(itype==-81) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)")
jpsipiz_mc_DD = tree.GetEntries("(itype==-82) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)")
kst_data_DD   = tree.GetEntries("(itype==72 || itype==82) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)")
jpsi_data_DD  = tree.GetEntries("(itype==71 || itype==81) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)")

procs = { 'KstGamMC'  : [ kst_mc_DD, kst_mc_LL, kst_mc ] ,
          'JpsiGamMC' : [ jpsi_mc_DD, jpsi_mc_LL, jpsi_mc ],
          'JpsiPiZMC' : [ jpsipiz_mc_DD, jpsipiz_mc_LL, jpsipiz_mc ],
          'KstGamData': [ kst_data_DD, kst_data_LL, kst_data],
          'JpsiGamData': [ jpsi_data_DD, jpsi_data_LL, jpsi_data ]
        }

for proc, vals in procs.items():
  print '%15s %5d  %5d  %5d  %1.3f'%(proc,vals[0],vals[1],vals[2],float(vals[0])/float(vals[1]))
