Pluck : UGen {
	
	*ar { arg in = 0.0, trig = 1.0, maxdelaytime = 0.2, delaytime = 0.2, decaytime = 1.0, 
			coef = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, trig, maxdelaytime, delaytime, decaytime, coef).madd(mul, add)}
	}

CombLP : UGen {
	
	*ar { arg in = 0.0, gate = 1.0, maxdelaytime = 0.2, delaytime = 0.2, decaytime = 1.0, 
			coef = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, gate, maxdelaytime, delaytime, decaytime, coef).madd(mul, add)}
	}
		
/*
s = Server.local.boot;

{Pluck.ar(WhiteNoise.ar(1), Impulse.kr(2), 1, 440.reciprocal, 1, coef: SinOsc.ar(20).range(0.0, 0.01))}.play(s)

(
	{var freq, numparts;
	numparts = 100;
	freq = SinOsc.kr(Array.fill(numparts, {Rand(0.05, 0.2)}), 
		Array.fill(numparts, {Rand(0, 1.0)})).range(1000, 3000);
	LeakDC.ar(
		Pluck.ar(
			WhiteNoise.ar(1).dup(numparts), 
			Impulse.ar(Array.fill(numparts, {Rand(20, 24)})), 
			1, freq.reciprocal, 51, mul: 0.01,
			coef: 0.2).sum);	
			}.play(s);
)
s.quit;
s=Server.internal.boot;
s.scope
{LeakDC.ar(
	a = CombLP.ar(Dust.ar(10), MouseX.kr(0,1).round, 1, 440.reciprocal, 50, 
	coef: 0.001);
	[a, DelayN.ar(a, 0.01, 0.01)]
	)
}.play(s)

*/