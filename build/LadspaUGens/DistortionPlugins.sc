CrossoverDistortion : UGen {

	*ar { arg in, amp = 0.5, smooth=0.5, mul=1.0, add=0;

		^this.multiNew('audio', in, amp, smooth).madd(mul, add)
		
	}
}

SmoothDecimator : UGen {

	*ar { arg in, rate=44100.0, smoothing=0.5, mul=1.0, add=0;

		^this.multiNew('audio', in, rate, smoothing).madd(mul, add)
		
	}
}
