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
	*kr { arg fft, leak = 0.995, numpreviousbeats=4;
		^this.multiNew('control',fft, leak, numpreviousbeats);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(4, rate);
	}

}