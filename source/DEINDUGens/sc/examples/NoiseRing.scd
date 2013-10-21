s.reboot;
s.unmute;
s.mute;


// plot
{
	Demand.ar(Impulse.ar(1000), 0, DNoiseRing(change: 0.1, numBits: 32)) * (2**32).reciprocal
}.plot(1)

// plain 32bit value scaled to 0..1 and out of the speaker
{Demand.ar(Impulse.ar(10000), 0, DNoiseRing(change: MouseX.kr(0, 1), chance: 0.51, numBits: 32)) * (2**32).reciprocal}.play

// as a sequencer
{
	var freq = (Demand.ar(Impulse.ar(10), 0, DNoiseRing(change: MouseX.kr(0, 1), chance: MouseY.kr(0, 1), numBits: 32))).linlin(0, 2**32, 40, 40+48).midicps.poll;
	
	SinOsc.ar(freq)!2
}.play