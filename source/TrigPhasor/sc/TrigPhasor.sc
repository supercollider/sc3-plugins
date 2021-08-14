TrigPhasor : MultiOutUGen {
	*ar { arg trig=0.0, rate=1.0, start=0, end=0, resetPos=0;
		^this.multiNew('audio', trig, rate, start, end, resetPos);
	}

	*kr { arg trig=0.0, rate=1.0, start=0, end=0, resetPos=0;
		^this.multiNew('control', trig, rate, start, end, resetPos);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}
