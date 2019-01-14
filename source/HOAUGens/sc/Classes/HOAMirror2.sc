HOAMirror2 : MultiOutUGen
{
  *ar { | in1, in2, in3, in4, in5, in6, in7, in8, in9, front_back(0.0), left_right(0.0), up_down(0.0) |
      ^this.multiNew('audio', in1, in2, in3, in4, in5, in6, in7, in8, in9, front_back, left_right, up_down)
  }

  *kr { | in1, in2, in3, in4, in5, in6, in7, in8, in9, front_back(0.0), left_right(0.0), up_down(0.0) |
      ^this.multiNew('control', in1, in2, in3, in4, in5, in6, in7, in8, in9, front_back, left_right, up_down)
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
      ^this.initOutputs(9, rate)
  }

  name { ^"HOAMirror2" }


  info { ^"Generated with Faust" }
}

