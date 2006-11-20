
PVSynth : UGen {
	*ar { 
		arg pvbuffer, numBins = 0, binStart = 0, binSkip = 1,
			filePointer = 0, freqMul = 1.0, freqAdd = 0.0, mul=1.0, add=0.0;
		var pointer;
		// limits the file pointer to values between 0 and 1;
		pointer = filePointer.max(0.0).min(1.0);
		^this.multiNew('audio', pvbuffer, numBins, binStart,
			binSkip, pointer, freqMul, freqAdd).madd(mul, add)
	}
}

PVInfo : MultiOutUGen {
	*kr {
		arg pvbuffer, binNum = 0, filePointer = 0, mul = 1, add = 0;
		var pointer;
		// limits the file pointer to values between 0 and 1;
		pointer = filePointer.max(0.0).min(1.0);
		^this.multiNew('control', pvbuffer, binNum, pointer).madd(mul, add)
		}
		
	init {arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
		}
	}