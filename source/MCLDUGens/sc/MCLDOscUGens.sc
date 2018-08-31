SawDPW : UGen {
	*ar { arg freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, iphase).madd(mul, add)
	}

	*kr { arg freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, iphase).madd(mul, add)
	}
}


PulseDPW {
	*ar { arg freq=440.0, width = 0.5, mul = 1.0, add = 0.0;
		^this.new('ar', freq, width, mul, add);
	}

	*kr { arg freq=440.0, width = 0.5, mul = 1.0, add = 0.0;
		^this.new('kr', freq, width, mul, add);
	}

	// The difference between two saws can be a square
	*new { arg rate='ar', freq=440.0, width = 0.5, mul = 1.0, add = 0.0;
		^(
			SawDPW.perform(rate, freq, 0)
				-
			SawDPW.perform(rate, freq, (width+width).wrap(-1, 1))
			).madd(mul, add);
	}
}
