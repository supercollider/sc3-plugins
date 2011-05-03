Getenv : UGen {
	*new { |key, defaultval=0|
		^this.multiNewList(['scalar', key, defaultval]);
	}
	*new1 { | rate, key, defaultval |
		key = key ? \SHLVL; // SHLVL is numeric, at least...

		key = key.asString.collectAs(_.ascii, Array);
		^super.new.rate_(rate).addToSynth.init([key.size, defaultval] ++ key);
	}

	init { arg theInputs;
		// store the inputs as an array
		inputs = theInputs;
	}
	*categories {	^ #["UGens>InfoUGens"]	}
}

/*
s.boot
{Clockmus.kr.poll(10)}.play
(
x = {
	var t1,s1,t2,s2,t3;
	t1 = Clockmus.kr;
	s1 = SinOsc.ar((300..400)).mean;
	t2 = Clockmus.kr;
	s2 = SinOsc.ar((300..320)).mean;
	t3 = Clockmus.kr;
	(t2-t1).poll(1, "101 sines");
	(t3-t2).poll(1, "21 sines");
	[s1, s2]
}.play
)
x.trace
*/
Clockmus : UGen {
	*kr {
		^this.multiNew('control')
	}
	*categories {	^ #["UGens>InfoUGens"]	}
}
