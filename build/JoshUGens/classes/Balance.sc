Balance : UGen {
	*ar {arg in, test, numperiods = 4, mul = 1, add = 0;
		^this.multiNew('audio', in, test, numperiods).madd(mul, add);
		}
	}
	
