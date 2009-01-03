
Friction : Filter {
	
	*ar { | in, friction = 0.5, spring = 0.414, damp = 0.313, mass = 0.1, beltmass = 1, mul=1, add=0 |
		^this.multiNew('audio', in, friction, spring, damp, mass, beltmass).madd(mul, add)
	}
	*kr { | in, friction = 0.5, spring = 0.414, damp = 0.313, mass = 0.1, beltmass = 1, mul=1, add=0 |
		^this.multiNew('control', in, friction, spring, damp, mass, beltmass).madd(mul, add)
	}
	*categories {	^ #["UGens>Filters>Chaotic"]	}
}