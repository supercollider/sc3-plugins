Balance : UGen {
	*ar {arg in, test, hp = 10, stor = 0, mul = 1, add = 0;
		^this.multiNew('audio', in, test, hp, stor).madd(mul, add);
		}
	}
