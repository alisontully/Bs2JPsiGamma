#!vim noexpandtab
import ROOT as r
import os
import sys

class infoObj:
	def __init__(self):
		self.name = ""
		self.fname = ""
		self.tname = ""

class configProducer:

	def __init__(self,runner,datfile,verbose=False):

		self.runner = runner
		self.datfile = datfile
		self.verbose = verbose
		self.cfgDict = {}
		self.branchdef = None
		self.analysers = []

		self.readDatfile()
		if self.verbose: self.printCfg()
		self.parseDatfile()

	def getSqrts(self,itype):
		if itype<0: return int(str(itype)[1])
		else: return int(str(itype)[0])

	def readDatfile(self):

		f = open(self.datfile)
		for line in f.readlines():
			if line.startswith('#'): continue
			if line=='\n': continue
			if line==' ': continue
			if len(line.strip())==0: continue
			if len(line.split())==0 and not (line.startswith('analysers') or line.startswith('branchdef')): continue
			itype = -999
			line = line.strip('\n')

			# pick up branch def
			if line.startswith('branchdef'):
				branchdef = line.split('=')[1]
				self.branchdef = getattr(r,branchdef)()
				continue

			# pick up analysers line second
			if line.startswith('analysers'):
				analysersLine = line.split('=')[1]
				for analyser in analysersLine.split(','):
					self.analysers.append(getattr(r,analyser)(analyser))
				continue

			els = line.split()
			# build cfg dictionary
			info = infoObj()
			for el in els:
				if el.startswith('itype'):
					itype = int(el.split('=')[1])
				else:
					varName = el.split('=')[0]
					varVal = el.split('=')[1]

					setattr(info,varName,varVal)

			if itype in self.cfgDict.keys():
				self.cfgDict[itype].append( info )
			else:
				self.cfgDict[itype] = [ info ]

		if len(self.analysers)==0:
			sys.exit('ERROR in datfile - Cannot run with no analysers!')
		if not self.branchdef:
			sys.exit('ERROR in datfile - Must specify a branchdef class!')
		f.close()

	def printCfg(self):

		print '%-30s'%'configProducer::printCfg()', '%-5s  %-5s  %-30s  %-70s  %-20s'%('itype','sqrts','name','fname','tname')
		for itype, flist in self.cfgDict.items():
			for f in flist:
				print '%-30s'%'', '%5d  %-5d  %-30s  %-70s  %-20s'%(itype,self.getSqrts(itype),f.name,f.fname, f.tname)

		print '%-30s'%'configProducer::printCfg()', 'Analysers:'
		for analyser in self.analysers:
			print '%-30s'%'', analyser.name

	def checkFile(self,fname,tname):
		if not os.path.isfile(fname) and not 'eoslhcb' in fname: # get out for eos files
			sys.exit('** ERROR ** -- The requested file (%s) does not exist'%fname)
		if not fname.endswith('.root'):
			sys.exit('** ERROR ** -- The requested file (%s) does not appear to be a root file'%fname)

		tf = r.TFile.Open(fname)
		if not tf.Get(tname):
			sys.exit('** ERROR *** -- The tree (%s) was not found in the file (%s)'%(tname,fname))
		tf.Close()

	def parseDatfile(self):

		for itype, flist in self.cfgDict.items():

			name = flist[0].name

			tree = r.TChain(name)

			for f in flist:
				self.checkFile(f.fname,f.tname)
				tree.AddFile(f.fname+'/'+f.tname)
				if self.verbose:
					print '%-30s'%'configProducer::parseDatfile()', 'Appended file', f.fname

				if name != f.name:
					sys.exit('ERROR -- If the itype is the same for two lines in the datfile, the name must be the same also')

			self.runner.addLooperTree(tree,name,itype,self.getSqrts(itype))

		for analyser in self.analysers:
			self.runner.addAnalyser(analyser)

