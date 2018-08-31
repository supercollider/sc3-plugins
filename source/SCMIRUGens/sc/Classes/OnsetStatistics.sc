/*

//input
control rate onset trigger signal

//outputs

[0] density (number of onsets in window)
[1] mean IOI
[2] variance IOI
//[3] staggered proximity to now? score more by close to now (kurtosis IOI?)

*/

OnsetStatistics : MultiOutUGen
{
	*kr { arg input, windowsize = 1.0, hopsize = 0.1;
		^this.multiNew('control',input,windowsize,hopsize);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
	}

}