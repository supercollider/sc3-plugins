Maxamp : UGen {
	*ar {arg in, numSamps = 1000;
		^this.multiNew('audio', in, numSamps);
		}
	}