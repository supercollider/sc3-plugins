VOSIM : UGen {
	*ar { arg trig = 0.1, freq = 400.0, nCycles = 1,decay = 0.9, mul = 1.0, add = 0.0;
		^this.multiNew('audio', trig, freq, nCycles, decay).madd(mul, add)
	}
}