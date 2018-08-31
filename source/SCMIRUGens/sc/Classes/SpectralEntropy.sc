SpectralEntropy : MultiOutUGen {	

	*kr {
		arg fft, fftsize=2048, numbands=1;
		
		if(numbands>fftsize) {
		
			numbands = fftsize; 	
		};
		
		^this.multiNew('control', fft, fftsize, numbands);
	}
	
	init { arg ... theInputs;
		inputs = theInputs;

		^this.initOutputs(theInputs[2], rate);
	}
}
