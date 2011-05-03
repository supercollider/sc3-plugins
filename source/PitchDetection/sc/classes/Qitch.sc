//constant Q transform, cross correlation spectral template and instantaneous frequency pitch tracker

Qitch : MultiOutUGen {

	*kr { arg in = 0.0, databufnum, ampThreshold=0.01, algoflag=1, ampbufnum, minfreq=0, maxfreq=2500;
		^this.multiNew('control', in, databufnum, ampThreshold, algoflag, ampbufnum ? (-1), minfreq, maxfreq)
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}
