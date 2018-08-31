RMEQSuite : UGen {
	*categories {^#["UGens>Filters"]}
	}

Allpass1 : RMEQSuite {
	*ar { arg in, freq = 1200.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq).madd(mul, add);
	}
}

Allpass2 : RMEQSuite {
	*ar { arg in, freq = 1200.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq).madd(mul, add);
	}
}


RMEQ : RMEQSuite {
	*ar {arg in, freq = 440, rq = 0.1, k = 0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq, k).madd(mul, add);
	}
}

RMShelf : RMEQSuite {
	*ar {arg in, freq = 440.0, k = 0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, k).madd(mul, add);
	}
}

RMShelf2 : RMEQSuite {
	*ar {arg in, freq = 440.0, k = 0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, k).madd(mul, add);
	}
}


Spreader : MultiOutUGen {
	*ar {arg in, theta = 1.5707963267949, filtsPerOctave = 8, mul = 1, add = 0;
		^this.multiNew('audio', in, theta, filtsPerOctave).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1)
		];
		^channels
	}
	}
