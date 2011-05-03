AtsUGen : UGen {
	*categories {^#["UGens>Analysis:Synthesis"]}
	}

AtsSynth : AtsUGen {
	*ar {
		arg atsbuffer, numPartials = 0, partialStart = 0, partialSkip = 1,
			filePointer = 0, freqMul = 1.0, freqAdd = 0.0, mul=1.0, add=0.0;
		^this.multiNew('audio', atsbuffer, numPartials, partialStart,
			partialSkip, filePointer, freqMul, freqAdd).madd(mul, add)
	}
}


AtsNoiSynth : AtsUGen {
	*ar {
		arg atsbuffer, numPartials = 0, partialStart = 0, partialSkip = 1,
			filePointer = 0, sinePct = 1.0, noisePct = 1.0, freqMul = 1.0, freqAdd = 0.0,
			numBands = 25, bandStart = 0, bandSkip = 1, mul=1.0, add=0.0;
		^this.multiNew('audio', atsbuffer, numPartials, partialStart,
			partialSkip, filePointer, sinePct, noisePct, freqMul, freqAdd, numBands,
				bandStart, bandSkip).madd(mul, add)
	}
}


AtsPartial : AtsUGen {
	*ar {
		arg atsbuffer, partial, filePointer = 0, freqMul = 1.0, freqAdd = 0.0, mul=1.0, add=0.0;
		^this.multiNew('audio', atsbuffer, partial, filePointer, freqMul, freqAdd).madd(mul, add)
	}
}

AtsBand : AtsUGen {
	*ar {
		arg atsbuffer, band, filePointer = 0, mul=1.0, add=0.0;
		^this.multiNew('audio', atsbuffer, band, filePointer).madd(mul, add)
	}
}


AtsFreq : AtsUGen {
	*ar {
		arg atsbuffer, partialNum = 0, filePointer = 0, mul = 1, add = 0;
		^this.multiNew('audio', atsbuffer, partialNum, filePointer).madd(mul, add)
		}

	*kr {
		arg atsbuffer, partialNum = 0, filePointer = 0, mul = 1, add = 0;
		^this.multiNew('control', atsbuffer, partialNum, filePointer).madd(mul, add)
		}
}


AtsAmp : AtsUGen {
	*ar {
		arg atsbuffer, partialNum = 0, filePointer = 0, mul = 1, add = 0;
		^this.multiNew('audio', atsbuffer, partialNum, filePointer).madd(mul, add)
		}
	*kr {
		arg atsbuffer, partialNum = 0, filePointer = 0, mul = 1, add = 0;
		^this.multiNew('control', atsbuffer, partialNum, filePointer).madd(mul, add)
		}
}

AtsNoise : AtsUGen {
	*ar {
		arg atsbuffer, bandNum = 0, filePointer = 0, mul = 1, add = 0;
		^this.multiNew('audio', atsbuffer, bandNum, filePointer).madd(mul, add)
		}
	*kr {
		arg atsbuffer, bandNum = 0, filePointer = 0, mul = 1, add = 0;
		^this.multiNew('control', atsbuffer, bandNum, filePointer).madd(mul, add)
		}
}

AtsParInfo : MultiOutUGen {
	*categories {^#["UGens>Analysis:Synthesis"]}

	*ar {
		arg atsbuffer, partialNum = 0, filePointer = 0, mul = 1, add = 0;
		^this.multiNew('audio', atsbuffer, partialNum, filePointer).madd(mul, add)
		}
	*kr {
		arg atsbuffer, partialNum = 0, filePointer = 0, mul = 1, add = 0;
		^this.multiNew('control', atsbuffer, partialNum, filePointer).madd(mul, add)
		}

	init {arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
		}
	}
