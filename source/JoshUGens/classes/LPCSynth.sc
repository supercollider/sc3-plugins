LPCVals : MultiOutUGen {
	*kr {arg buffer, pointer;
		^this.multiNew('control', buffer, pointer.min(1).max(0))
	}

	*ar {arg buffer, pointer;
		^this.multiNew('audio', buffer, pointer.min(1).max(0))
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
	}
}


LPCSynth : UGen {
	*ar {arg buffer, signal, pointer, mul = 1, add = 0;
		^this.multiNew('audio', buffer, signal, pointer.min(1).max(0)).madd(mul, add)
	}
}

// #errval, origrms = LP_Ana.ar(in, lpbuffer, numpoles, windowsize, check)
// check will check poles for stability. THIS DOES NOT RUN IN REAL_TIME! In NRT, use a value
// > 1.
/*
LP_Ana : MultiOutUGen {
	*ar {arg in, lpbuffer, numpoles, windowsize, check = 0;
		^this.multiNew('audio', in, lpbuffer, numpoles, windowsize, check);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}

LP_Synth : UGen {
	*ar {arg soundin, lpbuffer, numpoles, mul = 1, add = 0;
		^this.multiNew('audio', soundin, lpbuffer, numpoles).madd(mul, add);
	}
}

//#errval, origrms, pitch = LPC_Ana.ar(in, lpbuffer, numpoles, windowsize)

LPC_Ana : MultiOutUGen {
	*ar {arg in, lpbuffer, numpoles, windowsize;
		^this.multiNew('audio', in, lpbuffer, numpoles, windowsize);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
	}
}

LPC_Synth : UGen {
	*ar {arg soundin, lpbuffer, numpoles, mul = 1, add = 0;
		^this.multiNew('audio', soundin, lpbuffer, numpoles).madd(mul, add);
	}
}

// framincr is 1/2 windowsize... sampledur is size of lpbuffer in seconds
LPCAnal : UGen {
	*ar {arg in, lpbuffer, frameincr, numpoles, sampledur;
		this.multiNew('audio', in, lpbuffer, frameincr, numpoles, sampledur);
		^in;
	}

}

LPCSynth2 : UGen {
	*ar {arg lpbuffer, in, framein, numpoles;
		^this.multiNew('audio', lpbuffer, in, framein, numpoles);
	}
}

LPCVals2 : MultiOutUGen {
	*ar {arg lpbuffer, framein, numpoles;
		^this.multiNew('audio', lpbuffer, framein, numpoles);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
	}
}
*/
/*


SynthDef(\test, {var vocbuf = 1, lpbuf = 0;
	var errval, origrms, cps, noise, blip, pitch, hasPitch, in, out, rms2;
	in = LPF.ar(PlayBuf.ar(1, vocbuf, loop: 1), 500);
	#errval, origrms, pitch = LPC_Ana.ar(in, lpbuf, 19, 200);
	pitch = MouseX.kr(50, 200);
	noise = PinkNoise.ar(errval);
	blip = Blip.ar(pitch.max(50), 20, 0.95);
//	blip = PinkNoise.ar(1);
	out = LP_Synth.ar(blip + noise, lpbuf, 39, origrms).dup;
//	Maxamp.ar(pitch, 4410);
//	out = blip;
	Out.ar(0, out * EnvGen.kr(Env([0, 0, 0.0001], [1, 0.1])));
}).load(s);

s=Server.local.boot;
s.scope(2)
// allocate a buffer of NextPowerOf2(numpoles + 1)
s.sendMsg(\b_alloc, 0, 64);
s.sendMsg(\b_allocRead, 1, "/Users/josh/SND/fate.aiff");

s.sendMsg(\s_new, \test, 1001,0, 1)
s.sendMsg(\n_free, 1001);

s.sendMsg(\b_free, 0);
s.sendMsg(\b_free, 1);

s.quit;

a = Score.new

a = a.add([0, [\b_alloc, 0, 64], [\b_allocRead, 1, "/Users/josh/SND/fate.aiff"]])
a = a.add([1, [\s_new, \test, -1, 0, 0]]);
a = a.add([6, [0]]);
a.recordNRT("/tmp/trashme", "/Users/josh/Desktop/test.aif")

Player.new("/Users/josh/Desktop/Test.aif").gui
*/