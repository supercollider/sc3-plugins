/*
AttackSlope : MultiOutUGen
{
	*kr { arg windowsize,leak,minenergy,minaverage,mingap;
		^this.multiNew('audio',windowsize,leak,minenergy,minaverage,mingap);
	}

//outputs
//[0] onset trigger
//[1] corrected to minima time of last onset
//[2] most recent slope measurement
//[3] average slope calculation
//[4] detection function
//[5] peakpick function

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(5, rate);
	}
}
*/

AttackSlope : MultiOutUGen
{
	*kr { arg input,windowsize=1024,peakpicksize=20,leak=0.999,energythreshold=0.01,sumthreshold=20,mingap=30, numslopesaveraged=10;
		^this.multiNew('audio',input,windowsize,peakpicksize,leak,energythreshold,sumthreshold,mingap,numslopesaveraged);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(6, rate);
	}

}