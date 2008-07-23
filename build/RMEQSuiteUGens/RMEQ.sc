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

/* Not yet */
/*
RMShelf2 : RMEQSuite {
	*ar {arg in, freq = 440.0, k = 0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, k).madd(mul, add);
	}
}
	
*/	
