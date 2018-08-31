NestedAllpassN : UGen {
	*ar { arg in, maxdelay1=0.036, delay1=0.036, gain1=0.08, maxdelay2=0.03, delay2=0.03, gain2=0.3, mul=1.0, add=0.0;
		^this.multiNew('audio', in, maxdelay1, delay1, gain1, maxdelay2, delay2, gain2).madd(mul, add)
	}
}

NestedAllpassL : NestedAllpassN { }
NestedAllpassC : NestedAllpassN { }

DoubleNestedAllpassN : UGen {
	*ar { arg in, maxdelay1=0.0047, delay1=0.0047, gain1=0.15, maxdelay2=0.022, delay2=0.022, gain2=0.25, maxdelay3=0.0083, delay3=0.0083, gain3=0.3, mul=1.0, add=0.0;
		^this.multiNew('audio', in, maxdelay1, delay1, gain1, maxdelay2, delay2, gain2, maxdelay3, delay3, gain3).madd(mul, add)
	}
}

DoubleNestedAllpassL : DoubleNestedAllpassN { }
DoubleNestedAllpassC : DoubleNestedAllpassN { }

MoogLadder : UGen {
	*ar { arg in, ffreq = 440.0, res = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, ffreq, res).madd(mul, add)
	}
	*kr { arg in, ffreq = 440.0, res = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, ffreq, res).madd(mul, add)
	}
}

RLPFD : UGen {
	*ar { arg in, ffreq = 440.0, res = 0.0, dist = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, ffreq, res, dist).madd(mul, add)
	}
	*kr { arg in, ffreq = 440.0, res = 0.0, dist = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, ffreq, res, dist).madd(mul, add)
	}
}

Streson : UGen {
	*ar { arg input, delayTime = 0.003, res=0.9, mul = 1.0, add = 0.0;
		^this.multiNew('audio', input, delayTime, res).madd(mul, add)
	}
	*kr { arg input, delayTime = 0.003, res=0.9, mul = 1.0, add = 0.0;		^this.multiNew('control', input, delayTime, res).madd(mul, add)	}
}

NLFiltN : UGen {
	*ar { arg input, a, b, d, c, l, mul = 1.0, add = 0.0;
		^this.multiNew('audio', input, a, b, d, c, l).madd(mul, add)
	}
	*kr { arg input, a, b, d, c, l, mul = 1.0, add = 0.0;		^this.multiNew('control', input, a, b, d, c, l).madd(mul, add)
	}
}

NLFiltL : NLFiltN {
}

NLFiltC : NLFiltN {
}

GaussTrig : UGen {
	*ar { arg freq = 440.0, dev = 0.3, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, dev).madd(mul, add)
	}
	*kr { arg freq = 440.0, dev = 0.3, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, dev).madd(mul, add)
	}
	signalRange { ^\unipolar }
}

LFBrownNoise0 : UGen {
	*ar { arg freq = 20, dev = 1.0, dist = 0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, dev, dist).madd(mul, add)
	}
	*kr { arg freq= 20, dev = 1.0, dist = 0, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, dev, dist).madd(mul, add)
	}
}

LFBrownNoise1 : LFBrownNoise0 { }

LFBrownNoise2 : LFBrownNoise0 { }

TBrownRand : UGen {
	*ar { arg lo = 0, hi = 1.0, dev = 1.0, dist = 0, trig=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', lo, hi, dev, dist, trig).madd(mul, add)
	}
	*kr { arg lo = 0, hi = 1.0, dev = 1.0, dist = 0, trig=0, mul = 1.0, add = 0.0;
		^this.multiNew('control', lo, hi, dev, dist, trig).madd(mul, add)	}
}

Dbrown2 : UGen {
	*new { arg lo, hi, step, dist, length=inf;
		^this.multiNew('demand', length, lo, hi, step, dist)
	}
}

/* Dbeta : UGen {
	*new { arg lo, hi, prob1, prob2, length=inf;
		^this.multiNew('demand', length, lo, hi, prob1, prob2)
	}
} */

Dgauss : UGen {
	*new { arg lo, hi, length=inf;
		^this.multiNew('demand', length, lo, hi)
	}
}

TGaussRand : UGen {
	*ar { arg lo = 0, hi = 1.0, trig=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', lo, hi, trig).madd(mul, add)
	}
	*kr { arg lo = 0, hi = 1.0, trig=0, mul = 1.0, add = 0.0;
		^this.multiNew('control', lo, hi, trig).madd(mul, add)	}
}

TBetaRand : UGen {
	*ar { arg lo = 0, hi = 1.0, prob1, prob2, trig=0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', lo, hi, prob1, prob2, trig).madd(mul, add)
	}
	*kr { arg lo = 0, hi = 1.0, prob1, prob2, trig=0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', lo, hi, prob1, prob2, trig).madd(mul, add)	}
}

Gendy4 : UGen {
	*ar { arg ampdist=1, durdist=1, adparam=1.0, ddparam=1.0, minfreq=440, maxfreq=660, ampscale= 0.5, durscale=0.5, initCPs= 12, knum, mul=1.0,add=0.0;
		^this.multiNew('audio', ampdist, durdist, adparam, ddparam, minfreq, maxfreq, ampscale, durscale, initCPs, knum ? initCPs).madd( mul, add )
	}

	*kr {arg ampdist=1, durdist=1, adparam=1.0, ddparam=1.0, minfreq=20, maxfreq=1000, ampscale= 0.5, durscale=0.5, initCPs= 12, knum,mul=1.0,add=0.0;
	^this.multiNew('control', ampdist, durdist, adparam, ddparam, minfreq, maxfreq, ampscale, durscale, initCPs, knum ? initCPs).madd( mul, add )
	}
}

Gendy5 : Gendy4 { }

TGrains2 : MultiOutUGen {
	*ar { arg numChannels, trigger=0, bufnum=0, rate=1, centerPos=0,
			dur=0.1, pan=0, amp=0.1, att=0.5, dec=0.5, interp=4;
		^this.multiNew('audio', numChannels, trigger, bufnum, rate, centerPos,
				dur, pan, amp, att, dec, interp)
	}
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	argNamesInputsOffset { ^2 }
}

TGrains3 : MultiOutUGen {
	*ar { arg numChannels, trigger=0, bufnum=0, rate=1, centerPos=0,
			dur=0.1, pan=0, amp=0.1, att=0.5, dec=0.5, window=1, interp=4;
		^this.multiNew('audio', numChannels, trigger, bufnum, rate, centerPos,
				dur, pan, amp, att, dec, window, interp)
	}
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	argNamesInputsOffset { ^2 }
}
