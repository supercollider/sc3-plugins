s.quit
s.boot

// Compare various simple nonlinear functions (clip, wrap..) and
// their oversampled versions.
// Sweep the input sinewave with the mouse and listen for the aliasing

{ Clipper4.ar(SinOsc.ar(MouseX.kr(200, 5000), 0, 2), -0.2, 0.2) }.play
{ Clipper8.ar(SinOsc.ar(MouseX.kr(200, 5000), 0, 2), -0.2, 0.2) }.play
{ SinOsc.ar(MouseX.kr(200, 5000), 0, 2).clip2(0.2) }.play

{ OSWrap4.ar(SinOsc.ar(MouseX.kr(200, 5000), 0, 2), -0.2, 0.2) }.play
{ OSWrap8.ar(SinOsc.ar(MouseX.kr(200, 5000), 0, 2), -0.2, 0.2) }.play
{ SinOsc.ar(MouseX.kr(200, 5000), 0, 2).wrap2(0.2) }.play

{ OSFold4.ar(SinOsc.ar(MouseX.kr(200, 5000), 0, 2), -0.2, 0.2) }.play
{ OSFold8.ar(SinOsc.ar(MouseX.kr(200, 5000), 0, 2), -0.2, 0.2) }.play
{ SinOsc.ar(MouseX.kr(200, 5000), 0, 2).fold2(0.2) }.play

{ OSTrunc4.ar(SinOsc.ar(MouseX.kr(200, 5000), 0, 2), 0.2) }.play
{ OSTrunc8.ar(SinOsc.ar(MouseX.kr(200, 5000), 0, 2), 0.2) }.play
{ SinOsc.ar(MouseX.kr(200, 5000)).trunc(0.2) }.play