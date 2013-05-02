

//only triggers from FFT input

//PV_ChainUGen
//2 outputs for separate FFTs
MedianSeparation : MultiOutUGen {	

	*new {
		//paper notes p=2 better quality 
		arg fft, fftharmonic, fftpercussive, fftsize=1024, mediansize=17, hardorsoft=0, p=2, medianormax=0;
		
		^this.multiNew('control', fft, fftharmonic, fftpercussive, fftsize, mediansize, hardorsoft, p, medianormax);
	}
	
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
	
}


