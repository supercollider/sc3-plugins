s.quit
s.boot

// fill a buffer with an impulse-train
b = Buffer.alloc(Server.default, Server.default.sampleRate*2, 1);
{ RecordBuf.ar(Blip.ar(440, SampleRate.ir/440), b.bufnum) }.play

// play back with double speed - heavy aliasing occurs with BufRd and cubic interpolation
{ Pan2.ar(BufRd.ar(1, b.bufnum, Phasor.ar(1, 2, 0, BufFrames.ir(b.bufnum)))) }.play

// no aliasing with BLBufRd
{ Pan2.ar(BLBufRd.ar(b.bufnum, Phasor.ar(1, 2, 0, BufFrames.ir(b.bufnum)), 2)) }.play

// compare BLBufRD and BufRd with a variable playback rate
{ var rat = MouseX.kr(1,4.0); Pan2.ar(BLBufRd.ar(b.bufnum, Phasor.ar(1, rat, 0, BufFrames.ir(b.bufnum)), rat*1.25)) }.play

{ Pan2.ar(BufRd.ar(1, b.bufnum, Phasor.ar(1, MouseX.kr(1,4.0), 0, BufFrames.ir(b.bufnum)))) }.play
