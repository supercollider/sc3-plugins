HOABeamHCardio2HOA2 : MultiOutUGen
{
  *ar { | in1, in2, in3, in4, in5, in6, in7, in8, in9, azimuth(0.0), elevation(0.0), int_float(0.0), order(0.0) |
      ^this.multiNew('audio', in1, in2, in3, in4, in5, in6, in7, in8, in9, azimuth, elevation, int_float, order)
  }

  *kr { | in1, in2, in3, in4, in5, in6, in7, in8, in9, azimuth(0.0), elevation(0.0), int_float(0.0), order(0.0) |
      ^this.multiNew('control', in1, in2, in3, in4, in5, in6, in7, in8, in9, azimuth, elevation, int_float, order)
  } 

  checkInputs {
    if (rate == 'audio', {
      9.do({|i|
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
      ^this.initOutputs(25, rate)
  }

  name { ^"HOABeamHCardio2HOA2" }


  info { ^"Generated with Faust" }
}

