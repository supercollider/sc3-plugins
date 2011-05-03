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

Decimator : UGen {

	*ar { arg in, rate=44100.0, bits=24, mul=1.0, add=0;

		^this.multiNew('audio', in, rate, bits).madd(mul, add)

	}
}

SineShaper : UGen {

	*ar { arg in, limit=1.0, mul=1.0, add=0;

		^this.multiNew('audio', in, limit).madd(mul, add)

	}
}

Disintegrator : UGen {

	*ar { arg in, probability=0.5, multiplier=0.0, mul=1.0, add=0;

		^this.multiNew('audio', in, probability, multiplier).madd(mul, add)

	}
}
