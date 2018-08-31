FaustGreyholeRaw : MultiOutUGen
{
  *ar { | in1, in2, damping(0.0), delaytime(0.2), diffusion(0.5), feedback(0.9), moddepth(0.1), modfreq(2.0), size(1.0) |
      ^this.multiNew('audio', in1, in2, damping, delaytime, diffusion, feedback, moddepth, modfreq, size)
  }

  *kr { | in1, in2, damping(0.0), delaytime(0.2), diffusion(0.5), feedback(0.9), moddepth(0.1), modfreq(2.0), size(1.0) |
      ^this.multiNew('control', in1, in2, damping, delaytime, diffusion, feedback, moddepth, modfreq, size)
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

  name { ^"FaustGreyholeRaw" }
}

