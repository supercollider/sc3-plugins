/*

These UGens were written by Dan Stowell, re-using code from Lance Putnam's chaos UGens.
Released under the GNU Public License.

*/

MCLDChaosGen : UGen {
	*categories {	^ #["UGens>Generators>Chaotic"]	}
}

// Rossler attractor
RosslerL : MultiOutUGen {
	const <equation="x' = -y-z\ny' = x + ay\nz' = b + z(x - c)";
	*ar { arg freq=22050, a=0.2, b=0.2, c=5.7, h=0.05, xi=0.1, yi=0, zi=0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, a, b, c, h, xi, yi, zi).madd(mul, add)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
	}
	*categories {	^ #["UGens>Generators>Chaotic"]	}
}
RosslerResL : MCLDChaosGen {
	const <equation="x' = -y-z\ny' = x + ay\nz' = b + z(x - c)";
	*ar { arg in, stiff=1.0, freq=22050, a=0.2, b=0.2, c=5.7, h=0.05, xi=0.1, yi=0, zi=0, mul=1.0, add=0.0;
		^this.multiNew('audio', in, stiff, freq, a, b, c, h, xi, yi, zi).madd(mul, add)
	}
}

// This is the attractor described as "the L system" on http://sprott.physics.wisc.edu/chaos/finco/abs.html
FincoSprottL : MultiOutUGen {
	const <equation="x' = y+z\ny' = a|x| - y\nz' = 1 - x";
	*ar { arg freq=22050, a=2.45,h=0.05, xi=0, yi=0, zi=0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, a, h, xi, yi, zi).madd(mul, add)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
	}
	*categories {	^ #["UGens>Generators>Chaotic"]	}
}
// Ditto "M" system - N.B. the parameter A is negative, compared to the online version
FincoSprottM : MultiOutUGen {
	//const <equation="x' = y+z\ny' = a|x| - y\nz' = 1 - x";
	*ar { arg freq=22050, a= -7, b=4, h=0.05, xi=0, yi=0, zi=0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, a, b, h, xi, yi, zi).madd(mul, add)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
	}
	*categories {	^ #["UGens>Generators>Chaotic"]	}
}
// Ditto "S" system
FincoSprottS : MultiOutUGen {
	//const <equation="x' = y+z\ny' = a|x| - y\nz' = 1 - x";
	*ar { arg freq=22050, a=8, b=2, h=0.05, xi=0, yi=0, zi=0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, a, b, h, xi, yi, zi).madd(mul, add)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
	}
	*categories {	^ #["UGens>Generators>Chaotic"]	}
}

/*
// Rosenzweig-MacArthur food chain - can't seem to get periodic behaviour
RMAFoodChainL : MCLDChaosGen {
	//const <equation="x' = -y-z\ny' = x + ay\nz' = b + z(x - c)";
	*ar { arg freq=22050, a1=5.0, b1=3.0, d1= 0.4, a2=0.1, b2=2.0, d2=0.01, k=1.0943, r=0.8904, h=0.05, xi=0.1, yi=0, zi=0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, a1, b1, d1, a2, b2, d2, k, r, h, xi, yi, zi).madd(mul, add)
	}
}
*/

RMAFoodChainL : MultiOutUGen {
	//const <equation="x' = -y-z\ny' = x + ay\nz' = b + z(x - c)";
	*ar { arg freq=22050, a1=5.0, b1=3.0, d1= 0.4, a2=0.1, b2=2.0, d2=0.01, k=1.0943, r=0.8904, h=0.05, xi=0.1, yi=0, zi=0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, a1, b1, d1, a2, b2, d2, k, r, h, xi, yi, zi).madd(mul, add)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
	}
	*categories {	^ #["UGens>Generators>Chaotic"]	}
}

Perlin3 : UGen {
	*ar { |x=0, y=0, z=0|
		^this.multiNew('audio', x, y, z);
	}
	*kr { |x=0, y=0, z=0|
		^this.multiNew('control', x, y, z);
	}
}
