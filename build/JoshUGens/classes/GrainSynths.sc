MonoGrain : UGen {
	*ar { arg in, winsize = 0.1, grainrate = 10, winrandpct = 0, mul = 1, add = 0;
		^this.multiNew('audio', in, winsize, grainrate, winrandpct).madd(mul, add);
	}
}

MonoGrainBF : Panner {
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

SinGrain : UGen {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, freq).madd(mul, add);
		}
	}
		
SinGrainB : UGen {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, envbuf, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, freq, envbuf).madd(mul, add);
		}
	}		

SinGrainI : UGen {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, envbuf1, envbuf2, ifac = 0.5, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, freq, envbuf1, envbuf2, ifac).madd(mul, add);
		}
	}	
	
FMGrain : UGen {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index).madd(mul, add);
		}
	}

FMGrainB : UGen {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, envbuf, mul = 1, 
			add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index, envbuf).madd(mul, add);
		}
	}

FMGrainI : UGen {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, envbuf1, envbuf2, 
			ifac = 0.5, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index, envbuf1, envbuf2, ifac)
			.madd(mul, add);
		}
	}
	
BufGrain : UGen {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0, interp = 2, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp).madd(mul, add);
		}
	}
		
BufGrainB : UGen {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0, envbuf, interp = 2, mul = 1, 
			add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp, envbuf).madd(mul, add);
		}
	}
	
BufGrainI : UGen {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0,  envbuf1, envbuf2, 
			ifac = 0.5, interp = 2, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp, envbuf1, envbuf2,
			ifac).madd(mul, add);
		}
	}
	
InGrain : UGen {
	*ar	{ arg trigger = 0, dur = 1, in, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in).madd(mul, add);
		}
	}
		
InGrainB : UGen {
	*ar	{ arg trigger = 0, dur = 1, in, envbuf, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in, envbuf).madd(mul, add);
		}
	}		

InGrainI : UGen {
	*ar	{ arg trigger = 0, dur = 1, in, envbuf1, envbuf2, ifac = 0.5, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in, envbuf1, envbuf2, ifac).madd(mul, add);
		}
	}
	
/* Ambisonic Versions */

BFGrainPanner : MultiOutUGen {
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

SinGrainBF : MultiOutUGen {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, azimuth = 0, elevation = 0, rho = 1, mul = 1, 
			add = 0;
		^this.multiNew('audio', trigger, dur, freq, azimuth, elevation, rho).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}
		
SinGrainBBF : MultiOutUGen {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, envbuf, azimuth = 0, elevation = 0, rho = 1,
			mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, freq, envbuf, azimuth, elevation, rho)
			.madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

SinGrainIBF : MultiOutUGen {
	*ar	{ arg trigger = 0, dur = 1, freq = 440, envbuf1, envbuf2, ifac = 0.5, azimuth = 0, 
			elevation = 0, rho = 1,mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, freq, envbuf1, envbuf2, ifac, azimuth, 
			elevation, rho).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}
	
FMGrainBF : MultiOutUGen {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, azimuth = 0, 
			elevation = 0, rho = 1,mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index,
			azimuth, elevation, rho).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

FMGrainBBF : MultiOutUGen {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, envbuf, azimuth = 0, 
			elevation = 0, rho = 1, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index, envbuf, 
			azimuth, elevation, rho).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}

FMGrainIBF : MultiOutUGen {
	*ar	{ arg trigger = 0, dur = 1, carfreq = 440, modfreq = 200, index = 1, envbuf1, envbuf2, 
			ifac = 0.5, azimuth = 0, elevation = 0, rho = 1, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, carfreq, modfreq, index, envbuf1, envbuf2, ifac,
			azimuth, elevation, rho).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}
	
BufGrainBF : MultiOutUGen {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0,  
			azimuth = 0, elevation = 0, rho = 1, interp = 2, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp, azimuth, elevation, 
			rho).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}
		
BufGrainBBF : MultiOutUGen {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0, envbuf,  
			azimuth = 0, elevation = 0, rho = 1, interp = 2, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp, envbuf, azimuth,
			elevation, rho).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}
	
BufGrainIBF : MultiOutUGen {
	*ar { arg trigger = 0, dur = 1, sndbuf, rate = 1, pos = 0,  envbuf1, envbuf2, 
			ifac = 0.5, azimuth = 0, elevation = 0, rho = 1, interp = 2, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, sndbuf, rate, pos, interp, envbuf1, envbuf2,
			ifac, azimuth, elevation, rho).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}
	
InGrainBF : BFGrainPanner {
	*ar	{ arg trigger = 0, dur = 1, in, azimuth = 0, elevation = 0, rho = 1, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in, azimuth, elevation, rho).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}
		
InGrainBBF : BFGrainPanner {
	*ar	{ arg trigger = 0, dur = 1, in, envbuf, azimuth = 0, elevation = 0, rho = 1, 
			mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in, envbuf, azimuth, elevation, rho).madd(mul, add);
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
			azimuth = 0, elevation = 0, rho = 1, mul = 1, add = 0;
		^this.multiNew('audio', trigger, dur, in, envbuf1, envbuf2, ifac, azimuth, elevation,
			rho).madd(mul, add);
		}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
		}
	}
