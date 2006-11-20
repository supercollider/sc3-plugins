
AtsSynth : UGen {
	*ar { 
		arg atsbuffer, numPartials = 0, partialStart = 0, partialSkip = 1,
			filePointer = 0, freqMul = 1.0, freqAdd = 0.0, mul=1.0, add=0.0;
		var pointer;
		// limits the file pointer to values between 0 and 1;
		pointer = filePointer.max(0.0).min(1.0);
		^this.multiNew('audio', atsbuffer, numPartials, partialStart,
			partialSkip, pointer, freqMul, freqAdd).madd(mul, add)
	}
}
 
AtsNoiSynth : UGen {
	*ar { 
		arg atsbuffer, numPartials = 0, partialStart = 0, partialSkip = 1,
			filePointer = 0, sinePct = 1.0, noisePct = 1.0, freqMul = 1.0, freqAdd = 0.0, 
			numBands = 25, bandStart = 0, bandSkip = 1, mul=1.0, add=0.0;
		var pointer;
		// limits the file pointer to values between 0 and 1;
		pointer = filePointer.max(0.0).min(1.0);
		^this.multiNew('audio', atsbuffer, numPartials, partialStart,
			partialSkip, pointer, sinePct, noisePct, freqMul, freqAdd, numBands,
				bandStart, bandSkip).madd(mul, add)
	}
}
 
AtsFreq : UGen {
	*kr {
		arg atsbuffer, partialNum = 0, filePointer = 0, mul = 1, add = 0;
		var pointer;
		// limits the file pointer to values between 0 and 1;
		pointer = filePointer.max(0.0).min(1.0);
		^this.multiNew('control', atsbuffer, partialNum, pointer).madd(mul, add)
		}
}


AtsAmp : UGen {
	*ar {
		arg atsbuffer, partialNum = 0, filePointer = 0, mul = 1, add = 0;
		var pointer;
		// limits the file pointer to values between 0 and 1;
		pointer = filePointer.max(0.0).min(1.0);
		^this.multiNew('audio', atsbuffer, partialNum, pointer).madd(mul, add)
		}
	*kr {
		arg atsbuffer, partialNum = 0, filePointer = 0, mul = 1, add = 0;
		var pointer;
		// limits the file pointer to values between 0 and 1;
		pointer = filePointer.max(0.0).min(1.0);
		^this.multiNew('control', atsbuffer, partialNum, pointer).madd(mul, add)
		}
}

AtsNoise : UGen {
	*ar {
		arg atsbuffer, bandNum = 0, filePointer = 0, mul = 1, add = 0;
		var pointer;
		// limits the file pointer to values between 0 and 1;
		pointer = filePointer.max(0.0).min(1.0);
		^this.multiNew('audio', atsbuffer, bandNum, pointer).madd(mul, add)
		}
	*kr {
		arg atsbuffer, bandNum = 0, filePointer = 0, mul = 1, add = 0;
		var pointer;
		// limits the file pointer to values between 0 and 1;
		pointer = filePointer.max(0.0).min(1.0);
		^this.multiNew('control', atsbuffer, bandNum, pointer).madd(mul, add)
		}
}

AtsParInfo : MultiOutUGen {
	*kr {
		arg atsbuffer, partialNum = 0, filePointer = 0, mul = 1, add = 0;
		var pointer;
		// limits the file pointer to values between 0 and 1;
		pointer = filePointer.max(0.0).min(1.0);
		^this.multiNew('control', atsbuffer, partialNum, pointer).madd(mul, add)
		}
		
	init {arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
		}
	}
