BLowPass : UGen {
	*ar { arg in, freq = 1200.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq).madd(mul, add);
	}
}

BHiPass : UGen {
	*ar { arg in, freq = 1200.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq).madd(mul, add);
	}
}

BAllPass : UGen {
	*ar { arg in, freq = 1200.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq).madd(mul, add);
	}
}

BBandPass : UGen {
	*ar {arg in, freq = 1200.0, bw = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, bw).madd(mul, add);
	}
}

BBandStop : UGen {
	*ar {arg in, freq = 1200.0, bw = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, bw).madd(mul, add);
	}
}

BPeakEQ : UGen {
	*ar {arg in, freq = 1200.0, rq = 1.0, db = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq, db).madd(mul, add);
	}
}

BLowShelf : UGen {
	*ar {arg in, freq = 1200.0, rs = 1.0, db = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rs, db).madd(mul, add);
	}
}

BHiShelf : UGen {
	*ar {arg in, freq = 1200.0, rs = 1.0, db = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rs, db).madd(mul, add);
	}	
}