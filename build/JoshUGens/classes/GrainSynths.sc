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