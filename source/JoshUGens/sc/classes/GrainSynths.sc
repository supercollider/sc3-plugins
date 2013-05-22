JoshGrain : UGen {
	*categories {^#["UGens>GranularSynthesis"]}

/*
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}

	argNamesInputsOffset { ^2 }
*/
	}

JoshMultiChannelGrain : MultiOutUGen {
	*categories {^#["UGens>GranularSynthesis"]}
	}

JoshMultiOutGrain : MultiOutUGen {
	*categories {^#["UGens>GranularSynthesis"]}
	}


MonoGrain : JoshGrain {
	*ar { arg in, winsize = 0.1, grainrate = 10, winrandpct = 0, mul = 1, add = 0;
		^this.multiNew('audio', in, winsize, grainrate, winrandpct).madd(mul, add);
	}
}

MonoGrainBF : JoshMultiChannelGrain {
	*ar { arg in, winsize = 0.1, grainrate = 10, winrandpct = 0, azimuth = 0, azrand = 0,
			elevation = 0, elrand = 0, rho = 1, mul = 1, add = 0;
		^this.multiNew('audio', in, winsize, grainrate, winrandpct, azimuth, azrand, elevation,
			elrand, rho).madd(mul, add);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
}

/* other granular synths */

SinGrain : JoshGrain {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, freq).madd(mul, add);
		}
	}

SinGrainB : JoshGrain {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, envbuf, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, freq, envbuf).madd(mul, add);
		}
	}

SinGrainI : JoshGrain {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, envbuf1, envbuf2, ifac = 0.5, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, freq, envbuf1, envbuf2, ifac).madd(mul, add);
		}
	}

FMGrain : JoshGrain {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index).madd(mul, add);
		}
	}

FMGrainB : JoshGrain {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, envbuf, mul = 1,
			add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index, envbuf).madd(mul, add);
		}
	}

FMGrainI : JoshGrain {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, envbuf1, envbuf2,
			ifac = 0.5, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index, envbuf1, envbuf2, ifac)
			.madd(mul, add);
		}
	}

BufGrain : JoshGrain {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0, interp = 2, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp).madd(mul, add);
		}
	}

BufGrainB : JoshGrain {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0, envbuf, interp = 2, mul = 1,
			add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp, envbuf).madd(mul, add);
		}
	}

BufGrainI : JoshGrain {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0,  envbuf1, envbuf2,
			ifac = 0.5, interp = 2, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp, envbuf1, envbuf2,
			ifac).madd(mul, add);
		}
	}

InGrain : JoshGrain {
	*ar	{ arg trigger = 0, dur = 1, in, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in).madd(mul, add);
		}
	}

InGrainB : JoshGrain {
	*ar	{ arg trigger = 0, dur = 1, in, envbuf, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in, envbuf).madd(mul, add);
		}
	}

InGrainI : JoshGrain {
	*ar	{ arg trigger = 0, dur = 1, in, envbuf1, envbuf2, ifac = 0.5, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in, envbuf1, envbuf2, ifac).madd(mul, add);
		}
	}

/*
// add panning

SinGrain : JoshGrain {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, freq = 440, pan = 0, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, freq, pan).madd(mul, add);
		}
	}

SinGrainB : JoshGrain {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, freq = 440, envbuf, pan = 0, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, freq, envbuf, pan).madd(mul, add);
		}
	}

SinGrainI : JoshGrain {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, freq = 440, envbuf1, envbuf2, ifac = 0.5, pan = 0, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, freq, envbuf1, envbuf2, ifac, pan).madd(mul, add);
		}
	}

FMGrain : JoshGrain {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, pan = 0, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, carfreq, modfreq, index, pan).madd(mul, add);
		}
	}

FMGrainB : JoshGrain {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, envbuf, pan = 0, mul = 1,
			add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, carfreq, modfreq, index, envbuf, pan).madd(mul, add);
		}
	}

FMGrainI : JoshGrain {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, envbuf1, envbuf2,
			ifac = 0.5, pan = 0, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, carfreq, modfreq, index, envbuf1, envbuf2, ifac, pan)
			.madd(mul, add);
		}
	}

BufGrain : JoshGrain {
	*ar { arg numChannels = 1, trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0, interp = 2, pan = 0, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, sndbuf, rate, pos, interp, pan).madd(mul, add);
		}
	}

BufGrainB : JoshGrain {
	*ar { arg numChannels = 1, trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0, envbuf, interp = 2, pan = 0, mul = 1,
			add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, sndbuf, rate, pos, interp, envbuf, pan).madd(mul, add);
		}
	}

BufGrainI : JoshGrain {
	*ar { arg numChannels = 1, trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0,  envbuf1, envbuf2,
			ifac = 0.5, interp = 2, pan = 0, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, sndbuf, rate, pos, interp, envbuf1, envbuf2,
			ifac, pan).madd(mul, add);
		}
	}

InGrain : JoshGrain {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, in, pan = 0, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, in, pan).madd(mul, add);
		}
	}

InGrainB : JoshGrain {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, in, envbuf, pan = 0, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, in, envbuf, pan).madd(mul, add);
		}
	}

InGrainI : JoshGrain {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, in, envbuf1, envbuf2, ifac = 0.5, pan = 0, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, in, envbuf1, envbuf2, ifac, pan).madd(mul, add);
		}
	}

*/
/* Ambisonic Versions */

BFGrainPanner : JoshMultiOutGrain {
	checkNInputs { arg n;
		if (rate == 'audio') {
			n.do {| i |
				i = i + 2;
				if (inputs.at(i).rate != 'audio') {
					//"failed".postln;
					^("input " ++ i ++ " is not audio rate: " + inputs.at(i) + inputs.at(2).rate);
				};
			};
		 };
		^this.checkValidInputs
	}
	checkInputs { ^this.checkNInputs(1) }
}

/* other granular synths */

SinGrainBF : JoshMultiOutGrain {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, azimuth = 0, elevation = 0, rho = 1, wComp = 0,
			mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, freq, azimuth, elevation, rho, wComp).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

SinGrainBBF : JoshMultiOutGrain {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, envbuf, azimuth = 0, elevation = 0, rho = 1,
			wComp = 0, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, freq, envbuf, azimuth, elevation, rho, wComp)
			.madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

SinGrainIBF : JoshMultiOutGrain {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, envbuf1, envbuf2, ifac = 0.5, azimuth = 0,
			elevation = 0, rho = 1, wComp = 0, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, freq, envbuf1, envbuf2, ifac, azimuth,
			elevation, rho, wComp).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

FMGrainBF : JoshMultiOutGrain {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, azimuth = 0,
			elevation = 0, rho = 1, wComp = 0,mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index,
			azimuth, elevation, rho, wComp).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

FMGrainBBF : JoshMultiOutGrain {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, envbuf, azimuth = 0,
			elevation = 0, rho = 1, wComp = 0, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index, envbuf,
			azimuth, elevation, rho, wComp).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

FMGrainIBF : JoshMultiOutGrain {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, envbuf1, envbuf2,
			ifac = 0.5, azimuth = 0, elevation = 0, rho = 1, wComp = 0, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index, envbuf1, envbuf2, ifac,
			azimuth, elevation, rho, wComp).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

BufGrainBF : JoshMultiOutGrain {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0,
			azimuth = 0, elevation = 0, rho = 1, interp = 2, wComp = 0, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp, azimuth, elevation,
			rho, wComp).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

BufGrainBBF : JoshMultiOutGrain {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0, envbuf,
			azimuth = 0, elevation = 0, rho = 1, interp = 2, wComp = 0, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp, envbuf, azimuth,
			elevation, rho, wComp).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

BufGrainIBF : JoshMultiOutGrain {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0,  envbuf1, envbuf2,
			ifac = 0.5, azimuth = 0, elevation = 0, rho = 1, interp = 2, wComp = 0,
			mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp, envbuf1, envbuf2,
			ifac, azimuth, elevation, rho, wComp).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

InGrainBF : BFGrainPanner {
	*ar	{ arg trigger = 0, dur = 1, in, azimuth = 0, elevation = 0, rho = 1, wComp = 0,
			mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in, azimuth, elevation, rho, wComp).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

InGrainBBF : BFGrainPanner {
	*ar	{ arg trigger = 0, dur = 1, in, envbuf, azimuth = 0, elevation = 0, rho = 1, wComp = 0,
			mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in, envbuf, azimuth, elevation, rho, wComp).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

InGrainIBF : BFGrainPanner {
	*ar	{ arg trigger = 0, dur = 1, in, envbuf1, envbuf2, ifac = 0.5,
			azimuth = 0, elevation = 0, rho = 1, wComp = 0, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in, envbuf1, envbuf2, ifac, azimuth, elevation,
			rho, wComp).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

GrainSinJ : MultiOutUGen {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, freq = 440, grainAmp = 1, pan = 0, envbufnum = -1,
			maxGrains = 512, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, freq, pan, envbufnum, maxGrains, grainAmp)
			.madd(mul, add);
		}

	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	argNamesInputsOffset { ^2 }
	}

GrainFMJ : MultiOutUGen {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1,
			grainAmp = 1, pan = 0, envbufnum = -1, maxGrains = 512, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, carfreq, modfreq, index, pan, envbufnum,
				maxGrains, grainAmp).madd(mul, add);
		}

	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}

	argNamesInputsOffset { ^2 }
	}

GrainBufJ : MultiOutUGen {
	*ar { arg numChannels = 1, trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0, loop = 0, interp = 2,
			grainAmp = 1, pan = 0, envbufnum = -1, maxGrains = 512, mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, sndbuf, rate, pos, interp, pan,
			envbufnum, maxGrains, grainAmp, loop).madd(mul, add);
		}

	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	argNamesInputsOffset { ^2 }
	}

GrainInJ : MultiOutUGen {
	*ar	{ arg numChannels = 1, trigger = 0, dur = 1, in, grainAmp = 1, pan = 0, envbufnum = -1, maxGrains = 512,
			mul = 1, add = 0;
		^this.multiNew('audio', numChannels, trigger, dur, in, pan, envbufnum, maxGrains, grainAmp)
			.madd(mul, add);
		}

	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	argNamesInputsOffset { ^2 }
	}
