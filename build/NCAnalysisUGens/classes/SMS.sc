
SMS : MultiOutUGen
{
	*ar { arg input,maxpeaks=80, currentpeaks, tolerance=4, noisefloor= 0.2, freqmult=1.0, mul=1.0, add=0.0;
		^this.multiNew('audio',input,maxpeaks, currentpeaks, tolerance, noisefloor, freqmult).madd(mul, add);
	}
	
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}
