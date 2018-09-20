/*
Michael McCrea  mtm5@uw.edu
Jo Anderson     j.anderson@ambisonictoolkit.net
2018
*/

// A sum of samples over a variable rectangular window of time
MovingSum : UGen {

	*ar { arg in, numsamp=40, maxsamp=400;
		^this.multiNew('audio', in, numsamp, maxsamp);
	}

	*kr { arg in, numsamp=40, maxsamp=400;
		^this.multiNew('control', in, numsamp, maxsamp);
	}

}

// An average of samples over a variable rectangular window of time
MovingAverage : UGen {

	*ar { arg in, numsamp=40, maxsamp=400;
		^this.multiNew('audio', in, numsamp, maxsamp);
	}

	*kr { arg in, numsamp=40, maxsamp=400;
		^this.multiNew('control', in, numsamp, maxsamp);
	}

	*power { arg in, numsamp=40, maxsamp=400;
		^this.perform( UGen.methodSelectorForRate(in.rate),
			in.squared, numsamp, maxsamp
		)
	}

	*rms { arg in, numsamp=40, maxsamp=400;
		^this.power(in, numsamp, maxsamp).sqrt
	}

}
