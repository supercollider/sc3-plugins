/**
* Lots of FFT-based ugens for SC
* (c) 2006-2008 Dan Stowell, all rights reserved.
* Released under the GPL, v2 or later.
*/
PV_DiffMags : PV_ChainUGen
{
	*new { |bufferA, bufferB|
		"PV_DiffMags has changed name to PV_MagSubtract. 'PV_DiffMags' will be removed in future".warn;
		^PV_MagSubtract(bufferA, bufferB)
	}
	*categories { ^ nil }
}

PV_MagSubtract : PV_ChainUGen
{
	*new { |bufferA, bufferB, zerolimit=0|
		^this.multiNew('control', bufferA, bufferB, zerolimit)
	}
}

FFTPower : UGen
{
	*kr { arg buffer, square=true;
		if(square.isKindOf(Boolean)){
			square = square.binaryValue
		};
		^this.multiNew('control', buffer, square)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

FFTDiffMags : UGen
{
	*kr { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}


FFTFlux : UGen
{
	*kr { arg buffer, normalise=1;
		^this.multiNew('control', buffer, normalise)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

FFTFluxPos : UGen
{
	*kr { arg buffer, normalise=1;
		^this.multiNew('control', buffer, normalise)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

FFTSubbandPower : MultiOutUGen {

	var <numbands;

	*kr { arg chain, cutfreqs, square=1, scalemode=1;
		cutfreqs = cutfreqs.copy.sort;
		// Note the extra arg inserted so the UGen knows how many freqs to expect
		^this.multiNew('control', chain, cutfreqs.size, square, scalemode, *cutfreqs)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		numbands = inputs[1] + 1;
		^this.initOutputs(numbands, rate);
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

FFTSubbandFlux : MultiOutUGen {

	var <numbands;

	*kr { arg chain, cutfreqs, posonly=0;
		cutfreqs.sort;
		// Note the extra arg inserted so the UGen knows how many freqs to expect
		^this.multiNew('control', chain, cutfreqs.size, posonly, *cutfreqs)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		numbands = inputs[1] + 1;
		^this.initOutputs(numbands, rate);
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

PV_MagLog : PV_ChainUGen
{
	*new { arg buffer;
		^this.multiNew('control', buffer)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

PV_MagExp : PV_ChainUGen
{
	*new { arg buffer;
		^this.multiNew('control', buffer)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

FFTPhaseDev : UGen
{
	*kr { |buffer, weight=0, powthresh=0.1|
		^this.multiNew('control', buffer, weight, powthresh)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}
FFTComplexDev : UGen
{
	*kr { |buffer, rectify=0, powthresh=0.1|
		^this.multiNew('control', buffer, rectify, powthresh)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}
FFTMKL : UGen
{
	*kr { |buffer, epsilon=1e-06|
		^this.multiNew('control', buffer, epsilon)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

PV_Whiten : PV_ChainUGen
{
	*new { | chain, trackbufnum, relaxtime=2, floor=0.1, smear=0, bindownsample=0 |
		^this.multiNew('control', chain, trackbufnum, relaxtime, floor, smear, bindownsample)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

FFTCentroid : UGen
{
	*kr { | buffer |
		this.deprecated(thisMethod, SpecCentroid.findMethod(\kr));
		^SpecCentroid.kr(buffer)
	}
}

FFTSubbandFlatness : MultiOutUGen {

	var <numbands;

	*kr { arg chain, cutfreqs;
		cutfreqs.sort;
		// Note the extra arg inserted so the UGen knows how many freqs to expect
		^this.multiNew('control', chain, cutfreqs.size, *cutfreqs)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		numbands = inputs[1] + 1;
		^this.initOutputs(numbands, rate);
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

FFTCrest : UGen
{
	*kr { | buffer, freqlo=0, freqhi=50000 |
		^this.multiNew('control', buffer, freqlo, freqhi)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

FFTSpread : UGen
{
	*kr { | buffer, centroid |
		^this.multiNew('control', buffer, centroid ?? { SpecCentroid.kr(buffer) } )
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

FFTSlope : UGen
{
	*kr { | buffer |
		^this.multiNew('control', buffer)
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

FFTPeak : MultiOutUGen
{
	*kr { | buffer, freqlo=0, freqhi=50000 |
		^this.multiNew('control', buffer, freqlo, freqhi)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
	*categories { ^ #["UGens>Analysis", "UGens>FFT"] }
}

PV_MagSmooth : PV_ChainUGen
{
	*new { arg buffer, factor=0.1;
		^this.multiNew('control', buffer, factor)
	}
	*categories { ^ #["UGens>FFT"] }
}

PV_MagMulAdd : PV_ChainUGen
{
	*new { | buffer, mul=1, add=0 |
		^this.multiNew('control', buffer, mul, add)
	}
	*categories { ^ #["UGens>FFT"] }
}

PV_ExtractRepeat : PV_ChainUGen
{
	*new { | buffer, loopbuf, loopdur, memorytime=30, which=0, ffthop=0.5, thresh=1 |
		^this.multiNew('control', buffer, loopbuf, loopdur, memorytime, which, ffthop, thresh)
	}
	*categories { ^ #["UGens>FFT"] }
}
