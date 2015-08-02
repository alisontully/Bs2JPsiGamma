import ROOT as r

canvs = []
plots = []

bs_mass = 5366.77
bd_mass = 5279.58

def myIntegrateCurve( curve, low, high, npoints=1000 ):

  total = 0.
  step_size = float(high-low)/npoints
  for p in range(npoints):

    x1 = low + p*step_size
    x2 = low + (p+1)*step_size
    y1 = curve.Eval(x1)
    y2 = curve.Eval(x2)
    
    square = (x2-x1)* min(y1,y2)
    triangle = ((x2-x1)*abs(y1-y2))/2.

    sum = square+triangle
    print x1, x2, y1, y2, sum
    total += sum
  
  full_range_total = 0.
  x2 = -1.  
  p=0
  while x2 < 7000.:
    x1 = 4500 + p*step_size
    x2 = 4500 + (p+1)*step_size
    y1 = curve.Eval(x1)
    y2 = curve.Eval(x2)
    square = (x2-x1)* min(y1,y2)
    triangle = ((x2-x1)*abs(y1-y2))/2.
    sum = square+triangle
    full_range_total += sum

    p += 1
  
  return total/full_range_total


def integrateCurve( curve, low, high ):

  x = r.Double(0.)
  y = r.Double(0.)

  low_index=0
  high_index=-1

  for p in range( curve.GetN() ):

    curve.GetPoint(p,x,y)
    
    if x<low: 
      low_index = p

    if x<high:
      high_index = p

  low_eval = r.Double(0.)
  high_eval = r.Double(0.)

  curve.GetPoint(low_index,low_eval,y)
  curve.GetPoint(high_index,high_eval,y)

  sum = 0.
  for p in range(low_index, high_index):
    curve.GetPoint(p, x, y)
    sum += y

  integral = curve.Integral(low_index, high_index) / curve.Integral()
  print integral, ' from ', low_eval, 'to', high_eval
  return ( integral, low_eval, high_eval )

def printCLsNumbers( fname, hypoth_value, bin_center, bin_width ):

  tf = r.TFile( fname )
  ws = tf.Get('model_ws')

  mass = ws.var('B0_M')
  bf = ws.var('BF')
  data = ws.data('data')
  pdf = ws.pdf('simpdf')
  cat = ws.cat('tracktype')

  #pdf.fitTo(data)
  
  mass.Print('v')

  bf.setConstant(False)
  bf.setVal(0.)
  bf.setConstant(True)
  pdf.fitTo(data)

  bkg_plot = mass.frame()
  data.plotOn(bkg_plot)
  pdf.plotOn(bkg_plot, r.RooFit.ProjWData(r.RooArgSet(cat),data), r.RooFit.Precision(1.e-7))

  bkg_curve = bkg_plot.getObject( int(bkg_plot.numItems())-1 )
  #nbkg, bkg_low, bkg_high  = integrateCurve( bkg_curve, bin_center - (bin_width/2.), bin_center + (bin_width/2.) )
  nbkg  = myIntegrateCurve( bkg_curve, bin_center - (bin_width/2.), bin_center + (bin_width/2.) )

  bf.setConstant(False)
  bf.setVal(hypoth_value)
  bf.setConstant(True)
  pdf.fitTo(data)
  
  sb_plot = mass.frame()
  data.plotOn(sb_plot)
  pdf.plotOn(sb_plot, r.RooFit.ProjWData(r.RooArgSet(cat),data), r.RooFit.Precision(1.e-7))
  
  sb_curve = sb_plot.getObject( int(sb_plot.numItems())-1 )
  
  #print bkg_curve.Integral()
  #print sb_curve.Integral()
  #print data.sumEntries()

  #nsig, sig_low, sig_high  = integrateCurve( sb_curve,  bin_center - (bin_width/2.), bin_center + (bin_width/2.) )
  nsig  = myIntegrateCurve( sb_curve,  bin_center - (bin_width/2.), bin_center + (bin_width/2.) )
  
  #mass.setRange("bkg_range", bkg_low, bkg_high )
  #mass.setRange("sig_range", sig_low, sig_high )

  #brange_data = data.reduce( r.RooFit.CutRange( "bkg_range" ) )
  #sbrange_data = data.reduce( r.RooFit.CutRange( "sig_range" ) )
  mass.setRange("count_range", bin_center - (bin_width/2.), bin_center + (bin_width/2.) )
  reduced_data = data.reduce( r.RooFit.CutRange( "count_range") )

  print '------'
  print nbkg*data.sumEntries()
  print nsig*data.sumEntries()
  print reduced_data.sumEntries()
  print '------'

  tf.Close()

  return ( reduced_data.sumEntries(), nbkg*data.sumEntries(), nsig*data.sumEntries() )
  
  #bkg_full_integral = pdf.createIntegral(r.RooArgSet(mass,cat),r.RooFit.Range("full_range"), r.RooFit.NormSet(r.RooArgSet(mass,cat)))
  #bkg_integral = pdf.createIntegral(r.RooArgSet(mass,cat), r.RooFit.Range("count_range"), r.RooFit.NormSet(r.RooArgSet(mass,cat)))
  #nbkg = ( bkg_integral.getVal() / bkg_full_integral.getVal() ) * data.sumEntries()

  #print '--------'
  #print bkg_integral.getVal()
  #print bkg_full_integral.getVal()
  #print data.sumEntries()
  #bf.setConstant(False)
  #bf.setVal(hypoth_value)
  #bf.setConstant(True)
  #pdf.fitTo(data)
  #sb_full_integral = pdf.createIntegral(r.RooArgSet(mass))
  #sb_integral = pdf.createIntegral(r.RooArgSet(mass), r.RooFit.Range("count_range"))
  #nsb = ( sb_integral.getVal() / sb_full_integral.getVal() ) * data.sumEntries()

  #print '-------------'
  #print 'D:  ', ndata
  #print 'B:  ', nbkg
  #print 'SB: ', nsb
  #print '-------------'
  
  #plots.append(plot)
  #canvs.append( r.TCanvas() )
  #plots[-1].Draw()
 
  #canvs[-1].Update()
  #canvs[-1].Modified()
  #canvs[-1].Print("plot.pdf")



bs_data, bs_bkg, bs_sig = printCLsNumbers( 'model_ws_mfit.root',    5.e-6, bs_mass, 25. )

bd_data, bd_bkg, bd_sig = printCLsNumbers( 'model_ws_Bz_mfit.root', 5.e-6, bd_mass, 25. )

print '       Data  Sig    Bkg    '
print '==========================='
print 'Bs --  %5d   %7.3f   %7.3f'%(bs_data,bs_bkg,bs_sig)
print 'Bd --  %5d   %7.3f   %7.3f'%(bd_data,bd_bkg,bd_sig)

raw_input()

