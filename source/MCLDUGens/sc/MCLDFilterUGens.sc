
Friction : Filter {

	*ar { | in, friction = 0.5, spring = 0.414, damp = 0.313, mass = 0.1, beltmass = 1, mul=1, add=0 |
		^this.multiNew('audio', in, friction, spring, damp, mass, beltmass).madd(mul, add)
	}
	*kr { | in, friction = 0.5, spring = 0.414, damp = 0.313, mass = 0.1, beltmass = 1, mul=1, add=0 |
		^this.multiNew('control', in, friction, spring, damp, mass, beltmass).madd(mul, add)
	}
	*categories {	^ #["UGens>Filters>Chaotic"]	}
}

Crest : UGen {
	*kr { | in, numsamps=400, gate=1, mul=1, add=0 |
		^this.multiNew('control', in, numsamps, gate).madd(mul, add)
	}
	*categories {	^ #["UGens>Analysis"]	}
}

Goertzel : MultiOutUGen {

	*kr { arg in = 0.0, bufsize=1024, freq, hop=1;
		^this.multiNew('control', in, bufsize, freq, hop)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate); // outputs [real, imag]
	}
}
