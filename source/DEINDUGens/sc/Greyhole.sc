Greyhole {
	*ar { | in, delayTime(2.0), damp(0.0), size(1.0), diff(0.707), feedback(0.9), modDepth(0.1), modFreq(2.0)|
		in = in.asArray;
		
		^GreyholeRaw.ar(in.first, in.last, 
			damp, delayTime, diff, feedback, modDepth, modFreq, size)
	}
}

GreyholeRaw : MultiOutUGen
{
  *ar { | in1, in2, damping(0.0), delaytime(2.0), diffusion(0.5), feedback(0.9), moddepth(0.1), modfreq(2.0), size(1.0) |
      ^this.multiNew('audio', in1, in2, damping, delaytime, diffusion, feedback, moddepth, modfreq, size)
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

  name { ^"GreyholeRaw" }
}

