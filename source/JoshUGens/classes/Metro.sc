Metro : UGen {
	*ar {arg bpm, numBeats, mul = 1, add = 0;
		^this.multiNew('audio', bpm, numBeats).madd(mul, add)
		}

	*kr {arg bpm, numBeats, mul = 1, add = 0;
		^this.multiNew('control', bpm, numBeats).madd(mul, add)
		}
	}