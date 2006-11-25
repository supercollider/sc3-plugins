PV_DiffMags : UGen
{
	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}

FFTPower : UGen
{
	*kr { arg buffer;
		^this.multiNew('control', buffer)
	}
}

FFTFlatness : UGen
{
	*kr { arg buffer;
		^this.multiNew('control', buffer)
	}
}

FFTPercentile : UGen
{
	*kr { arg buffer, fraction=0.5;
		^this.multiNew('control', buffer, fraction)
	}
}

FFTDiffMags : UGen
{
	*kr { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}


FFTFlatnessSplitPercentile : MultiOutUGen {
	
	*kr { arg buffer, fraction=0.5;
		^this.multiNew('control', buffer, fraction)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}

FFTFlux : UGen
{
	*kr { arg buffer;
		^this.multiNew('control', buffer)
	}
}

FFTSubbandPower : MultiOutUGen {
	
	var <numbands;
	
	*kr { arg chain, cutfreqs, incdc=1;
		cutfreqs.sort;
		// Note the extra arg inserted so the UGen knows how many freqs to expect
		^this.multiNew('control', chain, cutfreqs.size, incdc, *cutfreqs)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		numbands = inputs[1] + 1;
		^this.initOutputs(numbands, rate);
	}
}

