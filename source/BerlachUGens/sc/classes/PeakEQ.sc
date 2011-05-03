PeakEQ4 : UGen {
	*ar {arg in, freq = 1200.0, rs = 1.0, db = 0.0;
		^this.multiNew('audio', in, freq, rs, db);
	}
}

PeakEQ2 : UGen {
	*ar {arg in, freq = 1200.0, rs = 1.0, db = 0.0;
		^this.multiNew('audio', in, freq, rs, db);
	}
}
