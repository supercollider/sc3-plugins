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
