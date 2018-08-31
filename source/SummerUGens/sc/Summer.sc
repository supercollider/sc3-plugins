Summer : UGen {

	*ar { arg trig=0, step=1, reset=0, resetval=0;
		^this.multiNew('audio', trig, step, reset, resetval)
	}
	*kr { arg trig=0, step=1, reset=0, resetval=0;
		^this.multiNew('control', trig, step, reset, resetval)
	}
	checkInputs { ^this.checkSameRateAsFirstInput }
}

WrapSummer : UGen {

	*ar { arg trig=0, step=1, min=0, max=1, reset=0, resetval;
		^this.multiNew('audio', trig, step, min, max, reset, resetval ? min)
	}
	*kr { arg trig=0, step=1, min=0, max=1, reset=0, resetval;
		^this.multiNew('control', trig, step, min, max, reset, resetval ? min)
	}
	checkInputs { ^this.checkSameRateAsFirstInput }
}
