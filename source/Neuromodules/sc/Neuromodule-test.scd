


{ Duty.ar(0.2, 0, DNeuromodule3().dpoll) }.play


a = (0, 0.01..1);
a.scurve.plot;

a = (0, 0.1..10);

(1/(1 + exp(neg(a)))).plot

s.options.blockSize = 4; s.reboot;

(
p = ProxySpace.push;
n = 3;
~theta = { 0.1.rand }.dup(n);
~w = { 10.0.rand }.dup(n * n);
~a = { 1.0.rand }.dup(n);


~a = {
	var a = ~a.kr(n), w = ~w.kr(n * n), theta = ~theta.kr(n);
	var z = { |x| 1 / (1 + exp(neg(x))) };
	a.collect { |each, i|
		theta[i] * a.sum { |an, j|
			w[n*j + i] * z.(an)
		}
	}

};

~zz = { ~a.kr(3).poll; Splay.ar(SinOsc.ar(~a.kr(3) * 2000 + 300)) * 0.2 };
~zz.play;
)