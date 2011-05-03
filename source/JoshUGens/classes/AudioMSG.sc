AudioMSG : UGen {	*ar { arg in, index=0pi, mul = 1.0, add=0.0;//		^SinOsc.ar( 0, ( in*0.5pi ) + SinOsc.ar( 0, in*2pi, index ), mul, add )
		^this.multiNew('audio', in, index).madd(mul, add);
	}}

//(in*0.5pi) + (in*2pi*index)
/*

s = Server.internal.boot;
s.scope(1);
{AudioMSG.ar(SinOsc.ar(440, 0, 0.3), MouseX.kr(0, 2pi))}.play(s);
s.quit;
*/