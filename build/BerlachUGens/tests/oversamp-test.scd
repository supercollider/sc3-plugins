s.quit
s.boot

// bypass (upsamples the signal by 8 and downsamples by 8)
{ OS8.ar(SinOsc.ar(200)) * 0.5 }.play;

{ SinOsc.ar(200) * 0.5 }.play;