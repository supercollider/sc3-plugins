
SMS : MultiOutUGen
{
	*ar { arg input,maxpeaks=80, currentpeaks=80, tolerance=4, noisefloor= 0.2, freqmult=1.0,freqadd=0.0, formantpreserve=0, useifft=0, ampmult=1.0, graphicsbufnum, mul=1.0, add=0.0;
		^this.multiNew('audio',input,maxpeaks, currentpeaks, tolerance, noisefloor, freqmult,freqadd,formantpreserve, useifft, ampmult, graphicsbufnum ? -1).madd(mul, add);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}
