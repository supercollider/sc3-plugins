//conversion of Tartini pitch tracking algorithm, from Philip MacLeod's Tartini project

Tartini : MultiOutUGen {

	*kr { arg in = 0.0, threshold=0.93, n=2048, k=0, overlap=1024, smallCutoff=0.5;
		^this.multiNew('control', in, threshold, n, k, overlap, smallCutoff)
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}
