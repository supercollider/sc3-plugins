s.reboot;
s.options.numAudioChannels = 10;
s.makeWindow

///Alien Wibble
(
{
	var in, out, mod1, mod2, mod3;
	in = SoundIn.ar(0);
	mod1 = SinOsc.ar(100);
	mod2 = SinOsc.ar(700);
	mod3 = SinOsc.ar(20);
	out = ComplexRes.ar(in, 5 + (500*in)+(200*mod1),0.5);
	out = ComplexRes.ar(out, 20.1 + (1*out) + (5000*mod2), 0.2);
	out = ComplexRes.ar(out, 10.1 + (1*out) + (100*mod3), 0.2);
	out = 0.001*LeakDC.ar(out);
	out = Compander.ar(out,out,0.5,1,0.3,0.0001,0.1);
	[1,1]*out;
}.play;
)



NdefMixer(s);

Ndef(\compar).asCode

(
Spec.add(\freq1, [0.001, 20000, \exp]);
Spec.add(\freq2, \freq1);
Spec.add(\freq3, \freq1);
Spec.add(\freq4, \freq1);
Spec.add(\freq5, \freq1);
Spec.add(\freq6, \freq1);

Ndef(\compar, {
	var in, out, d_out, mod1, mod2, mod3, env;
	in = SoundIn.ar(0);
	env = EnvFollow.ar(in);
	mod1 = SinOsc.ar(env*\freq1.kr(200));
	mod2 = SinOsc.ar(\freq2.kr(26));
	mod3 = SinOsc.ar(\freq3.kr(100));
	out = ComplexRes.ar(in, \freq4.kr(100) + (1*in)+(100*mod1),0.1);
	out = out+ ComplexRes.ar(out, (env*\freq5.kr(300.1)) + (1*out) + (100*mod2), 0.1);
	out = out+ ComplexRes.ar(out, \freq6.kr(100.1) + (1*out) + (100*mod3), 0.14);
	out = (5*OnePole.ar(in,0.3)) + (0.001*LeakDC.ar(out));
	out = Compander.ar(out,out,0.5,1,0.3,0.0001,0.1);
	d_out = AllpassC.ar(out, 1, 0.1, 0.4);
	[out,d_out];
});
)

Ndef(\compar).asCompileString

NodeProxy