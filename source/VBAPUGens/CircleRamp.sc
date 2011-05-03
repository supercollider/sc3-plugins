CircleRamp : Filter {

	*ar { arg in = 0.0, lagTime = 0.1, circmin = -180, circmax = 180, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, lagTime, circmin, circmax).madd(mul, add)
	}
	*kr { arg in = 0.0, lagTime = 0.1, circmin = -180, circmax = 180, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, lagTime, circmin, circmax).madd(mul, add)
	}
}

+ UGen {

	circleRamp { arg lagTime=0.1, circmin = -180, circmax = 180;
		^CircleRamp.multiNew(this.rate, this, lagTime, circmin, circmax)
	}

}