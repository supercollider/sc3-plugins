HOAEncoder3 : MultiOutUGen
{
  *ar { | in1, gain_0(0.0), radius_0(2.0), azimuth_0(0.0), elevation_0(0.0), yes(0.0), speaker_radius_0(1.07) |
      ^this.multiNew('audio', in1, gain_0, radius_0, azimuth_0, elevation_0, yes, speaker_radius_0)
  }

  *kr { | in1, gain_0(0.0), radius_0(2.0), azimuth_0(0.0), elevation_0(0.0), yes(0.0), speaker_radius_0(1.07) |
      ^this.multiNew('control', in1, gain_0, radius_0, azimuth_0, elevation_0, yes, speaker_radius_0)
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
      ^this.initOutputs(16, rate)
  }

  name { ^"HOAEncoder3" }


  info { ^"Generated with Faust" }
}

