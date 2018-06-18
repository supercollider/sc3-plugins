HOAPanLebedev502 : MultiOutUGen
{
  *ar { | in1, gain_0(0.0), radius_0(2.0), azimuth_0(0.0), elevation_0(0.0), yes(0.0), checkbox_0(0.0), checkbox_1(0.0), checkbox_2(0.0), speaker_radius(1.07) |
      ^this.multiNew('audio', in1, gain_0, radius_0, azimuth_0, elevation_0, yes, checkbox_0, checkbox_1, checkbox_2, speaker_radius)
  }

  *kr { | in1, gain_0(0.0), radius_0(2.0), azimuth_0(0.0), elevation_0(0.0), yes(0.0), checkbox_0(0.0), checkbox_1(0.0), checkbox_2(0.0), speaker_radius(1.07) |
      ^this.multiNew('control', in1, gain_0, radius_0, azimuth_0, elevation_0, yes, checkbox_0, checkbox_1, checkbox_2, speaker_radius)
  } 

  checkInputs {
    if (rate == 'audio', {
      1.do({|i|
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
      ^this.initOutputs(50, rate)
  }

  name { ^"HOAPanLebedev502" }


  info { ^"Generated with Faust" }
}

