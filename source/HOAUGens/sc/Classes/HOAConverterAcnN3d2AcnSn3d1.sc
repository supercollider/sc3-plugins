HOAConverterAcnN3d2AcnSn3d1 : MultiOutUGen
{
  *ar { | in1, in2, in3, in4 |
      ^this.multiNew('audio', in1, in2, in3, in4)
  }

  *kr { | in1, in2, in3, in4 |
      ^this.multiNew('control', in1, in2, in3, in4)
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
      ^this.initOutputs(4, rate)
  }

  name { ^"HOAConverterAcnN3d2AcnSn3d1" }


  info { ^"Generated with Faust" }
}

