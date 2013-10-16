JPverb : MultiOutUGen
{
  *ar { | in1, in2, damp(0.0), earlydiff(0.707), highband(2000.0), highx(1.0), lowband(500.0), lowx(1.0), mdepth(0.1), mfreq(2.0), midx(1.0), size(1.0), t60(1.0) |
      ^this.multiNew('audio', in1, in2, damp, earlydiff, highband, highx, lowband, lowx, mdepth, mfreq, midx, size, t60)
  }

  *kr { | in1, in2, damp(0.0), earlydiff(0.707), highband(2000.0), highx(1.0), lowband(500.0), lowx(1.0), mdepth(0.1), mfreq(2.0), midx(1.0), size(1.0), t60(1.0) |
      ^this.multiNew('control', in1, in2, damp, earlydiff, highband, highx, lowband, lowx, mdepth, mfreq, midx, size, t60)
  } 

  checkInputs {
    if (rate == 'audio', {
      2.do({|i|
        if (inputs.at(i).rate != 'audio', {
          ^(" input at index " + i + "(" + inputs.at(i) + 
            ") is not audio rate");
        });
      });
    });
    ^this.checkValidInputs
  }

  init { | ... theInputs |
      inputs = theInputs
      ^this.initOutputs(2, rate)
  }

  name { ^"JPverb" }
}

