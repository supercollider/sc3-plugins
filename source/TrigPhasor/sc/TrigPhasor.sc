TrigPhasor : MultiOutUGen {
	*ar { arg trig=0.0, resetPos=0, start=0, end=0, step;
		^this.multiNew('audio', trig, resetPos, start, end, step);
	}

	*kr { arg trig=0.0, resetPos=0, start=0, end=0, step;
		^this.multiNew('control', trig, resetPos, start, end, step);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}
