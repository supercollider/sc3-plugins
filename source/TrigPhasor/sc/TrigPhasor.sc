TrigPhasor : MultiOutUGen {
	*ar { arg trig=0.0, resetPos=0, start=0, end=0, rate=1.0;
		^this.multiNew('audio', trig, resetPos, start, end, rate);
	}

	*kr { arg trig=0.0, resetPos=0, start=0, end=0, rate=1.0;
		^this.multiNew('control', trig, resetPos, start, end, rate);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}
