CombLP : UGen {
	*ar { arg in = 0.0, gate = 1.0, maxdelaytime = 0.2, delaytime = 0.2, decaytime = 1.0,
			coef = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, gate, maxdelaytime, delaytime, decaytime, coef).madd(mul, add)}
	}


/*

a = {
	Out.ar(1, CombLP.ar(WhiteNoise.ar(0.5), MouseX.kr(0, 1), 0.1, 200.reciprocal, 5, 0.02, 0.2))}.play(s)
a.free;
*/