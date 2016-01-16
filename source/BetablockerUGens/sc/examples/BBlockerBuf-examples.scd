s.reboot;
Ndef(\one).fadeTime = 10

Ndef(\one).clear
(
Ndef(\one, {	
	var pos, src;
	var b = 
		// BBlockerProgram().fillUpRandom_(true)
		BBlockerProgram([\ORG, \INC, \DUP, \INC, \JMP, 1])
		.makeCollection.as(LocalBuf);
	
	#pos ... src = BBlockerBuf.ar(10000, b);
	// #pos ... src = BBlockerBuf.ar(Line.ar(1000, 10000, 10), b);
	// #pos ... src = BBlockerBuf.ar(Line.kr(1000, 10000, 10), b);
	// #pos ... src = BBlockerBuf.ar(SinOsc.ar(0).exprange([4000, 5000, 2000], 20000), b).flop;
	
	src = LeakDC.ar(src);
	pos.postln;
	// pos.poll(label: pos);
	// pos.collect{|p, i| SplayAz.ar(8, src, spread: 1, center: p , orientation: LFSaw.ar(1 * ( -1 * (i%2))) + (i+1)/3 )}.postln.sum;
	
	src.postln
})
)

Ndef(\one).vol = 0.01
Ndef(\one).play()
Ndef(\one).rebuild
Ndef(\one).stop
Ndef(\one).clear

(
Ndef(\one, {	
	var pos, src;
	var b = BBlockerProgram()
		.fillUpRandom_(true)
		.makeCollection.as(LocalBuf);
	
	LeakDC.ar(Pan2.ar(*BBlockerBuf.ar(s.sampleRate, b)));
})
)



(
a = BBlockerProgram([0, 0, 0, \ORG, \NOP, \NOP, \NOP] /* <-- some space*/ ++ ([\PSHL, -3, \PSHL, -5, \POP]) ++ [\JMP, 1]);
Ndef(\one, {a.ar(24000)!2}).play
)

a.collection.postcs

(
Ndef(\one, {	
	var pos, src;
	var b = a
		.fillUpRandom_(true)
		.makeCollection.as(LocalBuf);
	
	#src, pos = BBlockerBuf.ar(s.sampleRate, b);
	
	Pan2.ar(src, (pos * Line.kr(0, 1, 20)).tanh);
}).play;
)

a.play(24000)

Ndef(\one).fadeTime = 0;

Ndef(\one).rebuild

a.fillUpRandom_(false)


{[\tanh, \softclip, \distort, \fold2].collect{|method|
	SinOsc.ar(3, 0, Line.kr(1, 10, 5)).perform(method)
}}.plot2(5)
