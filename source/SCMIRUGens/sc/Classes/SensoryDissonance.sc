SensoryDissonance : UGen {	

	*kr {
		arg fft, maxpeaks=100, peakthreshold=0.1, norm, clamp=1.0;
		
		norm = norm ?? {0.01/maxpeaks};
		
		^this.multiNew('control', fft, maxpeaks, peakthreshold, norm, clamp);
	}
	
}
