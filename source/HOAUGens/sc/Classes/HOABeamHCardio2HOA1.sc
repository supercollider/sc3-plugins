HOABeamHCardio2HOA1 : MultiOutUGen
{
  *ar { | in1, in2, in3, in4, azimuth(0.0), elevation(0.0), int_float(0.0), order(0.0) |
      ^this.multiNew('audio', in1, in2, in3, in4, azimuth, elevation, int_float, order)
  }

  *kr { | in1, in2, in3, in4, azimuth(0.0), elevation(0.0), int_float(0.0), order(0.0) |
      ^this.multiNew('control', in1, in2, in3, in4, azimuth, elevation, int_float, order)
  } 

  checkInputs {
    if (rate == 'audio', {
      4.do({|i|
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
      ^this.initOutputs(16, rate)
  }

  name { ^"HOABeamHCardio2HOA1" }


  info { ^"Generated with Faust" }
}

