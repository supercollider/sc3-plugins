(
Ndef(\many, {
	var num = 60;
	var pos, src;
	var b = {BBlockerProgram().fillUpRandom_(true).makeCollection.as(LocalBuf)}!num;
	
	#pos, src = ({|i| BBlockerBuf.ar(s.sampleRate, b[i])}!num).flop;
	Pan2.ar(LeakDC.ar(src) * 0.02, pos).sum.tanh;
})
)

Ndef(\many).play

Ndef(\many).fadeTime = 30

(
Tdef(\manyPlay, {
	loop{
		(2*Ndef(\many).fadeTime).wait;
		Ndef(\many).rebuild;
	}
}).play
)

Tdef(\manyPlay).stop
Ndef(\many).free(10)