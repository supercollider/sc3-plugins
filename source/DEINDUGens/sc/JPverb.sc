
JPverb {
	*ar { | in, t60(1.0), damp(0.0), size(1.0), earlyDiff(0.707), modDepth(0.1), modFreq(2.0), low(1.0), mid(1.0), high(1.0), lowcut(500.0), highcut(2000.0)|
		in = in.asArray;
		
		^JPverbRaw.ar(in.first, in.last, damp, earlyDiff, highcut, high, lowcut, low, modDepth, modFreq, mid, size, t60)
	}
}

JPverbRaw : MultiOutUGen
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

  name { ^"JPverbRaw" }
}

