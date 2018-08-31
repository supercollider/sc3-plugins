
// blackrain at realizedsound do net
// ugens 11/2006

AmplitudeMod : UGen {
	*ar { arg in = 0.0, attackTime = 0.01, releaseTime = 0.01, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, attackTime, releaseTime).madd(mul, add)
	}
	*kr { arg in = 0.0, attackTime = 0.01, releaseTime = 0.01, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, attackTime, releaseTime).madd(mul, add)
	}
}

IIRFilter : UGen {
	*ar { arg in, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq).madd(mul, add)
	}
}

BMoog : UGen {
	*ar { arg in, freq = 440.0, q = 0.2, mode = 0.0, saturation = 0.95, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, q, mode).madd(mul, add)
	}
}

SVF : UGen {
	*ar {
		arg signal, cutoff=2200.0, res=0.1, lowpass=1.0, bandpass=0.0, highpass=0.0,
			notch=0.0, peak=0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', signal, cutoff, res, lowpass, bandpass, highpass, notch, peak)
			.madd(mul, add)
	}
	*kr {
		arg signal, cutoff=2200.0, res=0.1, lowpass=1.0, bandpass=0.0, highpass=0.0,
			notch=0.0, peak=0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', signal, cutoff, res, lowpass, bandpass, highpass, notch, peak)
			.madd(mul, add)
	}
}
