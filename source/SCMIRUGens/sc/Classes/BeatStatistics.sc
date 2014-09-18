/*

//input
FFT

//outputs

[0] entropy
[1] ratio top value to second highest
//[2]

*/

BeatStatistics : MultiOutUGen
{
	//windowsize,hopsize
	*kr { arg fft;
		^this.multiNew('control',fft);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}

}