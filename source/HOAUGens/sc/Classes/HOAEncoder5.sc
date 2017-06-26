FaustHOAEncoder5 : MultiOutUGen
{
  *ar { | in1, gain(0.0), yes(0.0), source_azimuth(0.0), source_elevation(0.0), source_radius(2.0), speakers_radius(1.07) |
      ^this.multiNew('audio', in1, gain, yes, source_azimuth, source_elevation, source_radius, speakers_radius)
  }

  *kr { | in1, gain(0.0), yes(0.0), source_azimuth(0.0), source_elevation(0.0), source_radius(2.0), speakers_radius(1.07) |
      ^this.multiNew('control', in1, gain, yes, source_azimuth, source_elevation, source_radius, speakers_radius)
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
      ^this.initOutputs(36, rate)
  }

  name { ^"FaustHOAEncoder5" }
}

