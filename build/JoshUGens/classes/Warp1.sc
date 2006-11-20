//See Warp1.help for more details and examples
// SC3 implementation of Chad Kirby's Warp1 class from SC2, based on Richard Karpen's sndwarp
// for csound.  By Josh Parmenter, with great suggestions from Alo Allik

Warp1 : UGen {
	*ar { arg bufnum=0, pointer=0, freqScale = 1,
			windowSize = 0.2, overlaps = 8, windowRandRatio = 0.0, mul=0.1, interp=4; 
		^this.multiNew('audio', bufnum, pointer, freqScale, 
			windowSize, overlaps, windowRandRatio, mul, interp)
	}
//	init { arg ... theInputs;
//		inputs = theInputs;
//		^this.initOutputs(1, rate);
//	}
}
