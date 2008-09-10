//This file is part of MLfftwUGens. Copyright (C) 2006  Nicholas M.Collins distributed under the terms of the GNU General Public License full notice in file MachineListening.license

Concat : UGen
{
	*ar { arg control, source, storesize=1.0, seektime=1.0, seekdur=1.0, matchlength=0.05, freezestore=0, zcr=1.0, lms=1.0, sc=1.0, st=0.0, randscore=0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio',control, source, storesize, seektime, seekdur, matchlength, freezestore, zcr, lms, sc, st, randscore).madd(mul, add);
	}
}



