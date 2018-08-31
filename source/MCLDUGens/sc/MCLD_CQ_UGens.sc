CQ_Diff : MultiOutUGen { // Based on Qitch

	*kr { arg in1 = 0.0, in2 = 0.0, databufnum;
		^this.multiNew('control', in1, in2, databufnum)
	}


	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(1, rate);
	}
}
