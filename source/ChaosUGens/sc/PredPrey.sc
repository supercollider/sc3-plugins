/*

These UGens were written by Till Bovermann, re-using code from Dan Stowell's MCLDChaos UGens(reusing code from Lance Putnam's chaos UGens).
Released under v.2 GNU Public License.
*/


// 2-species PredatorPrey system (Arneodo, Coullet, Tresser, as described by G.W. Flake (1998): Computational Beauty of Nature, p. 187)
LotkaVolterra : MultiOutUGen {
	const <equation="x' = x * (a - b * y); y' = y * (c * x - d);";

	*categories {	^ #["UGens>Generators>Chaotic"]	}


	*ar { arg freq=22050, a=1.5, b=1.5, c=0.5, d=1.5, h=0.05, xi=1, yi=0.2, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, a, b, c, d, h, xi, yi).madd(mul, add)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}

}


// 3-species PredatorPrey system (Arneodo, Coullet, Tresser, as described by G.W. Flake (1998): Computational Beauty of Nature, p. 187)
ArneodoCoulletTresser : MultiOutUGen {
	const <equation="x' = x * (1.1 - x / 2 - y / 2 - z / 10);\ny' = y * (-0.5 + x / 2 + y / 10 - z / 10);\nz' = z * (alpha + 0.2 - alpha * x - y / 10 - z / 10);";

	*categories {	^ #["UGens>Generators>Chaotic"]	}


	*ar { arg freq=22050, alpha=1.5, h=0.05, xi=0.5, yi=0.5, zi=0.5, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, alpha, h, xi, yi, zi).madd(mul, add)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
	}

}
