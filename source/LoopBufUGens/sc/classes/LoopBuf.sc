// LoopBuf by Lance Putnam 2004
// lance@uwalumni.com

LoopBuf : MultiOutUGen {
	*ar { arg numChannels, bufnum=0, rate=1.0, gate=1.0, startPos=0.0, startLoop, endLoop, interpolation=2;
		^this.multiNew('audio', numChannels, bufnum, rate, gate, startPos, startLoop, endLoop, interpolation)
	}

	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	argNamesInputsOffset { ^2 }
}

//FLoopBuf : LoopBuf {
//}