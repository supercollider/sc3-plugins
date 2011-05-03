//glame lowpass iir and whitenoise
play{LADSPA.ar(1,1891,1000+LFNoise1.kr(10,800),2,LADSPA.ar(1,1050,1!2))}

x=play{|r=2,cf=2000|LADSPA.ar(1,1219,0.2,0.2,0.9,0.9,1,LADSPA.ar(1,1891,cf+LFNoise1.kr(r,cf*0.8),2,LADSPA.ar(1,1050,0.1!2)))}
x.set(\r,5)
x.set(\cf,5000)


//not found plugin, just silent
play{LADSPA.ar(1,500)}

play{LADSPA.ar(1,1050,0.1!2)}

play{Pan2.ar(WhiteNoise.ar(0.1),0)}
play{Pan2.ar(LADSPA.ar(1,1050,0.1),0)}

//plate reverb, mono input, stereo output.
play{LADSPA.ar(2,1779,PinkNoise.ar(LFNoise1.ar(10,0.5)),0.5,0.9,0.5,0.9)}

//plate reverb, stereo in/out.
play{LADSPA.ar(2,1795,PinkNoise.ar(LFNoise1.ar(2)),PinkNoise.ar(LFNoise1.ar(10)),0.5,0.7,0.5,0.7)}

play{Limiter.ar(({|i|LADSPA.ar(1,1412,SinOsc.ar(i*20+40,0,LFNoise1.ar(0.1)),SinOsc.ar(i*20+500+LFNoise1.kr(0.01,10),0,LFNoise1.ar(0.1)))*0.4}!7).sum!2)}

play{LADSPA.ar(1!2,1209,0.4,0.01,SinOsc.ar([50,200],0,LFNoise1.kr(0.1)).sum)}
