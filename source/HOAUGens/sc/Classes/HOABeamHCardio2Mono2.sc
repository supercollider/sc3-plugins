HOABeamHCardio2Mono2 : UGen
{
  *ar { | in1, in2, in3, in4, in5, in6, in7, in8, in9, int_float(0.0), order(0.0), output_gain(0.0), azimuth(0.0), elevation(0.0) |
      ^this.multiNew('audio', in1, in2, in3, in4, in5, in6, in7, in8, in9, int_float, order, output_gain, azimuth, elevation)
  }

  *kr { | in1, in2, in3, in4, in5, in6, in7, in8, in9, int_float(0.0), order(0.0), output_gain(0.0), azimuth(0.0), elevation(0.0) |
      ^this.multiNew('control', in1, in2, in3, in4, in5, in6, in7, in8, in9, int_float, order, output_gain, azimuth, elevation)
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

  name { ^"HOABeamHCardio2Mono2" }


  info { ^"Generated with Faust" }
}

