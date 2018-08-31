//MoogVCF.ar(in, fco, res, mul, add); fco = filter cut-off, res = resonance (0 -> 1)

MoogVCF : UGen {
	*ar {arg in, fco, res, mul = 1, add = 0;
		^this.multiNew('audio', in, fco, res).madd(mul, add);
		}
	}

/*

s.boot;

{a = Blip.ar(20, 2000);
MoogVCF.ar(a, MouseY.kr(50, 1760), SinOsc.ar(0.1, 0, 0.4, 0.5), 1)}.play(s);

*/