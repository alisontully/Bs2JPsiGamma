#!/usr/bin/env python

import ROOT as r

tf = r.TFile('AnalysisOutAllProcsNoBDTCut.root')
tree = tf.Get('AnalysisTree')
hists = {}

def makeHist(name, title, bins, xlow, xhigh, var, cut):

  if name in hists.keys():
    print 'WARNING -- the hist with name', name, 'has already been made. Returning this hist'
    return hists[name]

  th1f = r.TH1F(name,title,bins,xlow,xhigh)
  tree.Draw('%s>>%s'%(var,name),cut,'goff')

  hists[name] = th1f
  return th1f

def getCutString(types, cut):

  cut_string = ''
  for i, typ in enumerate(types):
    this_cut = '(itype==%d)'%typ
    cut_string += this_cut
    if i<(len(types)-1):
      cut_string += ' && '

  if cut != '':
    cut_string += ' && (%s)'%cut

  return cut_string


config = {
    'Data_jpsig'       : [71,81] ,
    'Data_jpsig_2011'  : [71]    ,
    'Data_jpsig_2012'  : [81]    ,
    'Data_kstg'        : [72,82] ,
    'Data_kstg_2011'   : [72]    ,
    'Data_kstg_2012'   : [82]    ,
    'MC_jpsig'         : [-81]   ,
    'MC_kstg'          : [-88]   ,
    'MC_kstg_2011'     : [-89]   ,
    'MC_kstg_Sim06'    : [-90]
    }

subsets = {
    'NoCut':      '',
    'BDTCut':     '(pass_bdt)',
    'NoCut_LL':   '(eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)',
    'NoCut_DD':   '(eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)',
    'BDTCut_LL':  '(pass_bdt) && (eplus_TRACK_Type==3) && (eminus_TRACK_Type==3)',
    'BDTCut_DD':  '(pass_bdt) && (eplus_TRACK_Type==5) && (eminus_TRACK_Type==5)'
    }

settings = {
    'Jpsi_MM' :    { 'name':  'jpsi_MM',
                     'nbins': 100,
                     'xlow':  1650,
                     'xhigh': 1850
                   } ,
    'gamgams_MM' : { 'name':  'gamgams_MM',
                     'nbins': 100,
                     'xlow':  0,
                     'xhigh': 100
                   }
    }

outf = r.TFile('Plots.root','RECREATE')

for var, setting in settings.items():
  for ds, types in config.items():
    for subset, cut in subsets.items():
      cut_string = getCutString(types, cut)
      h_name = setting['name'] + '_' + ds + '_' + subset
      th1f = makeHist(h_name, setting['name'], setting['nbins'], setting['xlow'], setting['xhigh'], var, cut_string)
      outf.cd()
      th1f.Write()

outf.Close()
tf.Close()


