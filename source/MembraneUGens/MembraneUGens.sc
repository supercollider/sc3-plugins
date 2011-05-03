MembraneCircle : UGen {
	*ar { arg excitation, tension=0.05, loss = 0.99999, mul = 1.0, add = 0.0;
		^this.multiNew('audio', excitation, tension, loss).madd(mul, add)
	}
	checkInputs { ^this.checkSameRateAsFirstInput }
}

MembraneHexagon : MembraneCircle {
	*ar { arg excitation, tension=0.05, loss = 0.99999, mul = 1.0, add = 0.0;
		^this.multiNew('audio', excitation, tension, loss).madd(mul, add)
	}
}
