DelTapWr : UGen {
	*ar {arg buffer, in;
		^this.multiNew('audio', buffer, in)
		}
		
	*kr {arg buffer, in;
		^this.multiNew('control', buffer, in)
		}
	}

DelTapRd : UGen {
	*ar {arg buffer, phase, delTime, interp = 1, mul = 1, add = 0;
		^this.multiNew('audio', buffer, phase, delTime, interp).madd(mul, add)
		}
		
	*kr {arg buffer, phase, delTime, interp = 1, mul = 1, add = 0;
		^this.multiNew('control', buffer, phase, delTime, interp).madd(mul, add)
		}
	}