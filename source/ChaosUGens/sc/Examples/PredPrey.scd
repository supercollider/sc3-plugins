s.reboot;

(
Ndef(\pp).addSpec(\freq, [0.1, 24000, \exp]);
Ndef(\pp).addSpec(\h, [0.01, 1, \exp]);
Ndef(\pp).addSpec(\which, [0, 2, \lin]);
Ndef(\pp).addSpec(\alpha, [1, 1.7, \lin]);

Ndef(\pp, {
	var which = \which.kr(0, 0.1);
	var x, y, z;
	# x, y, z = ArneodoCoulletTresser.ar(SampleRate.ir*0.5, alpha: \alpha.kr(1.5), h: \h.kr(0.01));
	# x, y, z = [x, y, z] - 0.5 * 0.1;
	[[x, y, z], [y, z, x]].collect{|chan|
		SelectX.ar(which, chan);
	}

});
)

Ndef(\pp).scope

Ndef(\pp).edit


Ndef(\pp).clear


(
Ndef(\lv).addSpec(\freq, [0.1, 24000, \exp]);
Ndef(\lv).addSpec(\h, [0.01, 1, \exp]);
Ndef(\lv).addSpec(\which, [0, 2, \lin]);
Ndef(\lv).addSpec(\a, [0.01, 5, \lin]);
Ndef(\lv).addSpec(\b, [0.01, 5, \lin]);
Ndef(\lv).addSpec(\c, [0.01, 5, \lin]);
Ndef(\lv).addSpec(\d, [0.01, 5, \lin]);

Ndef(\lv, {
	LeakDC.ar(
		LotkaVolterra.ar(SampleRate.ir*0.5, \a.kr(1.5, 0.1), \b.kr(1.5, 0.1), \c.kr(0.5, 0.1), \d.kr(1.5, 0.1), h: \h.kr(0.01), xi: 0.1 + \tr.tr(1)) * 0.2
	).tanh + 1 * LFPar.ar([100, 150]);
});
)


(
Ndef(\lv, {
	Mix(
		LeakDC.ar(
			LotkaVolterra.ar(SampleRate.ir*0.5,
				a: {LFNoise2.ar(Rand(0.000174, 0.00174)).range(0.49, 0.5)}!3,
				b: \b.kr(1.5, 0.1),
				c: {LFNoise2.ar(Rand(0.000174, 0.00174)).range(2, 4)}!4, // \c.kr(0.5, 0.1),
				a: {LFNoise2.ar(Rand(0.000174, 0.00174)).range(0.4, 0.41)}!4,
				h: \h.kr(0.01).lag2(1) * [0.5, 1, 2, 4],
				xi: 0.1 + \tr.tr(1)) * 0.2
		) * [5, 3, 3, 1].normalizeSum;
	).tanh
});

)

Ndef(\lv).set(\tr, 0.1)

Ndef(\lv).set(\a, 3.029850, \b, 4.094132, \c, 1.967217, \d, 2.295942);



Ndef(\lv).scope

Ndef(\lv).edit


Ndef(\lv).clear


