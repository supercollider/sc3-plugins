
PVSynth : UGen {
	*categories {^#["UGens>Analysis:Synthesis"]}

	*ar {
		arg pvbuffer, numBins = 0, binStart = 0, binSkip = 1,
			filePointer = 0, freqMul = 1.0, freqAdd = 0.0, mul=1.0, add=0.0;
		^this.multiNew('audio', pvbuffer, numBins, binStart,
			binSkip, filePointer, freqMul, freqAdd).madd(mul, add)
	}
}

PVInfo : MultiOutUGen {
	*categories {^#["UGens>Analysis:Synthesis"]}

	*ar {
		arg pvbuffer, binNum = 0, filePointer = 0, mul = 1, add = 0;
		^this.multiNew('audio', pvbuffer, binNum, filePointer).madd(mul, add)
		}

	*kr {
		arg pvbuffer, binNum = 0, filePointer = 0, mul = 1, add = 0;
		^this.multiNew('control', pvbuffer, binNum, filePointer).madd(mul, add)
		}

	init {arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
		}
	}