HOADecBinaural1 : MultiOutUGen
{
  *ar { | in1, in2, in3, in4, inputs_gain(0.0), outputs_gain(0.0) |
      ^this.multiNew('audio', in1, in2, in3, in4, inputs_gain, outputs_gain)
  }

  *kr { | in1, in2, in3, in4, inputs_gain(0.0), outputs_gain(0.0) |
      ^this.multiNew('control', in1, in2, in3, in4, inputs_gain, outputs_gain)
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
      ^this.initOutputs(2, rate)
  }

  name { ^"HOADecBinaural1" }


  info { ^"Generated with Faust" }
}

