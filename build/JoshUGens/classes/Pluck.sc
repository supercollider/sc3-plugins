Pluck : UGen {
	
	*ar { arg in = 0.0, trig = 1.0, maxdelaytime = 0.2, delaytime = 0.2, decaytime = 1.0, 
			coef = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, trig, maxdelaytime, delaytime, decaytime, coef).madd(mul, add)}
	}

Pluck2 : UGen {
	
	*ar { arg in = 0.0, gate = 1.0, maxdelaytime = 0.2, delaytime = 0.2, decaytime = 1.0, 
			coef = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, gate, maxdelaytime, delaytime, decaytime, coef).madd(mul, add)}
	}
		
/*
s = Server.local.boot;

{Pluck.ar(WhiteNoise.ar(1), Impulse.kr(2), 1, 440.reciprocal, 1, coef: SinOsc.ar(20).range(0.9, 1.0))}.play(s)

(
	{var freq, numparts;
	numparts = 10;
	freq = SinOsc.kr(Array.fill(numparts, {Rand(0.05, 0.2)}), 
		Array.fill(numparts, {Rand(0, 1.0)})).range(1000, 3000);
	LeakDC.ar(
		Pluck.ar(
			WhiteNoise.ar(1).dup(numparts), 
			Impulse.ar(Array.fill(numparts, {Rand(20, 24)})), 
			1, freq.reciprocal, 51, mul: 1,
			coef: 0.5).sum);	
			}.play(s);
)
s.quit;
s=Server.internal.boot;
s.scope
{LeakDC.ar(
	Pluck2.ar(LFNoise0.ar(30, 0.1), Trig.kr(Dust.kr(4), 0.1), 1, 440.reciprocal, 5, 
	coef: 0.3)
	)
}.play(s)

*/