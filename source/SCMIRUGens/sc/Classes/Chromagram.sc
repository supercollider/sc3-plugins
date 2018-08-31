Chromagram : MultiOutUGen {

	*kr {
		arg fft, fftsize=2048, n=12, tuningbase=32.703195662575, octaves=8, integrationflag=0, coeff=0.9, octaveratio=2, perframenormalize=0;
		^this.multiNew('control', fft, fftsize, n, tuningbase, octaves, integrationflag, coeff, octaveratio, perframenormalize);
	}

	init { arg ... theInputs;
		inputs = theInputs;

		^this.initOutputs(theInputs[2], rate);
	}
}
