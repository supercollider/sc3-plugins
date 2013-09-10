
(
Ndef(\tiickle, {
	var src;
	src =	DiodeRingMod.ar(
		SinOsc.ar(3000 * ([1, 1.1, 1.2] * SinOsc.ar(100).range(1, 2))),
		SinOsc.ar(100 * [0.75, 1, 0.5])
	) * LFPulse.ar(10.3, 0, 0.04);
	
	SplayAz.ar(8, 
		src, 
		//LFNoise0.kr(0.5).range(0.1, 1), 
		center: LFSaw.ar(\round.kr(10))
	)
})

)



(
(
Ndef('tiickle', {
	var src;
	src =	DiodeRingMod.ar(
		2*SinOsc.ar(0.5*333 * ([0.5,1, 1.333, 1.5,1.7,2.5*0.5] )),
		SinOsc.ar(0.1 * [0.25, 2, 0.5])
	) * (LFPulse.ar(1.3, {Rand().round(0.2)}!8, 0.3)+LFPulse.ar(1.3*0.49, {Rand().round(0.2)}!8, 0.3));
	
	SplayAz.ar(8, 
		src, 
		//LFNoise0.kr(0.5).range(0.1, 1), 
		center: LFSaw.ar(\round.kr(10))
	)
});
);


Ndef('tiickle').set('round', 0.53185759458923);
Ndef('tiickle').play(
	vol: 0.0049917296786389

);
)

Ndef(\tiickle).edit



///////////////

s.reboot

Ndef(\tiickle, {
}).play






Ndef(\tiickle, {
	var index = 0;
	Bend({ |original|
		[original, original.isNumber, index].postln;
		if(original.isNumber) {
			original = NamedControl.kr("bend_"++index, original);
			index = index + 1;	
		};
		original;
	}, {
		DiodeRingMod.ar(SinOsc.ar(3000 * ([1, 1.1, 1.2] * 
			SinOsc.ar(100).range(1, 2))), SinOsc.ar(100 * [0.75, 1, 0.5])).sum * 0.2 * LFPulse.ar(10.3, 0, 0.04) * 0.1
	})
})


Ndef(\tiickle).edit





y = {DiodeRingMod.ar(SinOsc.ar(3700 * ([1, 1.1, 1.2] * SinOsc.ar(200).range(1, 2))), SinOsc.ar(100 * [0.75, 1, 0.5])).sum * 0.2 * LFPulse.ar(10.3 * 0.5, 0, 0.04) * 0.1}.play


z = {DiodeRingMod.ar(SinOsc.ar(18000 * ([1, 1.1, 1.2] * SinOsc.ar(200).range(1, 2))), SinOsc.ar(100 * [0.75, 1, 0.5])).sum * 0.2 * LFPulse.ar(10.3 * 1/6, 0, 0.02) * 0.1}.play

a = {DiodeRingMod.ar(SinOsc.ar(1800 * ([1, 1.1, 1.2] * SinOsc.ar(200).range(1, 2))), SinOsc.ar(100 * [0.75, 1, 0.5])).sum * 0.2 * LFPulse.ar(10.3 * LFPulse.ar(10.3).range(1/6, 1/2), 0, 0.02) * 0.1}.play

a = {DiodeRingMod.ar(SinOsc.ar(800 * ([1, 1.1, 1.2] * SinOsc.ar(200).range(1, 2))), SinOsc.ar(100 * [0.75, 1, 0.5])).sum * 0.2 * LFPulse.ar(10.3 * 1/32, 0, 0.2) * 0.1}.play

a = {DiodeRingMod.ar(SinOsc.ar(400 * ([1, 1.1, 1.2] * SinOsc.ar(200).range(1, 2))), SinOsc.ar(100 * [0.75, 1, 0.5])).sum * 0.2 * LFPulse.ar(10.3 * 1/32, 0, 0.2) * 0.1}.play

a = {DiodeRingMod.ar(SinOsc.ar(200 * ([1, 1.1, 1.2] * SinOsc.ar(200).range(1, 2))), SinOsc.ar(100 * [0.75, 1, 0.5])).sum * 0.2 * LFPulse.ar(10.3 * 1/32, 0, 0.2) * 0.1}.play

a = {DiodeRingMod.ar(SinOsc.ar(100 * ([1, 1.1, 1.2] * SinOsc.ar(200).range(1, 2))), SinOsc.ar(100 * [0.75, 1, 0.5])).sum * 0.2 * LFPulse.ar(10.3 * 1/32, 0, 0.2) * 0.1}.play

a = {DiodeRingMod.ar(SinOsc.ar(40 * ([1, 1.1, 1.2] * SinOsc.ar(50).range(1, 2))), SinOsc.ar(100 * [0.75, 1, 0.5])).sum * 0.2 * LFPulse.ar(10.3 * 1/32, 0, 0.4) * 0.5}.play



a

z.free

y.free
s.scope