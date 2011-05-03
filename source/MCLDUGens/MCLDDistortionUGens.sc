/*
MCLD Distortion UGens. (c) Dan Stowell.
Released under the GNU General Public License - see LICENSE.txt for details.
*/
InsideOut : Filter {

	*ar { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in).madd(mul, add)
	}
	*kr { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in).madd(mul, add)
	}
	*categories {	^ #["UGens>Filters>Nonlinear"]	}
}

WaveLoss : Filter {

	*ar { |in=0.0, drop=20, outof=40, mode=1, mul=1.0, add=0.0|
		^this.multiNew('audio', in, drop, outof, mode).madd(mul, add)
	}
	*kr { |in=0.0, drop=20, outof=40, mode=1, mul=1.0, add=0.0|
		^this.multiNew('control', in, drop, outof, mode).madd(mul, add)
	}
	*categories {	^ #["UGens>Filters>Nonlinear"]	}
}
Squiz : UGen {
	*ar {
		|in, pitchratio=2, zcperchunk=1, memlen=0.1, mul=1, add=0|
		^this.multiNew('audio', in, pitchratio, zcperchunk, memlen).madd(mul, add)
	}
	*kr {
		|in, pitchratio=2, zcperchunk=1, memlen=0.1, mul=1, add=0|
		^this.multiNew('control', in, pitchratio, zcperchunk, memlen).madd(mul, add)
	}
	*categories {	^ #["UGens>Filters>Nonlinear"]	}
}
