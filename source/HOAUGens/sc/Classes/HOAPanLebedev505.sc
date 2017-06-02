HOAPanLebedev505 : MultiOutUGen
{
  *ar { | in1, in2, gain_0(0.0), radius_0(2.0), azimuth_0(0.0), elevation_0(0.0), yes_0(0.0), gain_1_0(0.0), radius_1_0(2.0), azimuth_1_0(0.0), elevation_1_0(0.0), yes_1(0.0), checkbox_0_1(0.0), checkbox_1_1(0.0), checkbox_2_1(0.0), checkbox_3_1(0.0), checkbox_4_1(0.0), checkbox_5_1(0.0), speaker_radius_1(1.07) |
      ^this.multiNew('audio', in1, in2, gain_0, radius_0, azimuth_0, elevation_0, yes_0, gain_1_0, radius_1_0, azimuth_1_0, elevation_1_0, yes_1, checkbox_0_1, checkbox_1_1, checkbox_2_1, checkbox_3_1, checkbox_4_1, checkbox_5_1, speaker_radius_1)
  }

  *kr { | in1, in2, gain_0(0.0), radius_0(2.0), azimuth_0(0.0), elevation_0(0.0), yes_0(0.0), gain_1_0(0.0), radius_1_0(2.0), azimuth_1_0(0.0), elevation_1_0(0.0), yes_1(0.0), checkbox_0_1(0.0), checkbox_1_1(0.0), checkbox_2_1(0.0), checkbox_3_1(0.0), checkbox_4_1(0.0), checkbox_5_1(0.0), speaker_radius_1(1.07) |
      ^this.multiNew('control', in1, in2, gain_0, radius_0, azimuth_0, elevation_0, yes_0, gain_1_0, radius_1_0, azimuth_1_0, elevation_1_0, yes_1, checkbox_0_1, checkbox_1_1, checkbox_2_1, checkbox_3_1, checkbox_4_1, checkbox_5_1, speaker_radius_1)
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
      ^this.initOutputs(50, rate)
  }

  name { ^"HOAPanLebedev505" }
}

