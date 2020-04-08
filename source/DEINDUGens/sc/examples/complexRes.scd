s.reboot;

{[0.1,0.1]*ComplexRes.ar(Pulse.ar(0.1,0.001,0.1),50+ 5000*SinOsc.ar(50),0.5)}.play;