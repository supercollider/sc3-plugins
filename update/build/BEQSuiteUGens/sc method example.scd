
s.boot

// ar method
(
	z = SynthDef('ar_test', { arg amp=0.25;
		var coefs, audio;

		audio = AudioIn.ar([1,2]);

		audio = BLowPass.ar(audio, MouseX.kr(80,12e3,1), MouseY.kr(0.1,1));
		
		Out.ar(0, audio * amp);
	}).play
)

z.set(\amp, 0.5)

z.trace

z.free

// sc method gets biquad coefs
(
	z = SynthDef('sc_test', { arg amp=0.25;
		var coefs, audio;
	
		audio = AudioIn.ar([1,2]);
		
		coefs = BLowPass.sc(nil, MouseX.kr(80,12e3,1), MouseY.kr(0.1,1));
		
		audio = SOS.ar(audio, *coefs);
		
		Out.ar(0, audio * amp);
	}).play
)

z.set(\amp, 0.5)

z.trace

z.free

s.quit
