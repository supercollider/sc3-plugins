// outputs sin values based on the sin functiom (rather than table lookup). More CPU, no error

SinTone : UGen {
	*ar {arg freq = 440, phase = 0, mul = 1, add = 0;
		^this.multiNew('audio', freq, phase).madd(mul, add)
		}
	}