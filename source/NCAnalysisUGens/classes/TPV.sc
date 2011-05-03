TPV : UGen {

	*ar { arg chain, windowsize=1024, hopsize=512, maxpeaks=80, currentpeaks, freqmult=1.0, tolerance=4, noisefloor= 0.2, mul=1.0, add=0.0;

		^this.multiNew('audio',chain, windowsize, hopsize, maxpeaks, currentpeaks, freqmult, tolerance, noisefloor).madd(mul, add);
	}
}
