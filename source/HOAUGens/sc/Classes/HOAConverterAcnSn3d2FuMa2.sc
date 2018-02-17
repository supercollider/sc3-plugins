HOAConverterAcnSn3d2FuMa2 : MultiOutUGen
{
  *ar { | in1, in2, in3, in4, in5, in6, in7, in8, in9 |
      ^this.multiNew('audio', in1, in2, in3, in4, in5, in6, in7, in8, in9)
  }

  *kr { | in1, in2, in3, in4, in5, in6, in7, in8, in9 |
      ^this.multiNew('control', in1, in2, in3, in4, in5, in6, in7, in8, in9)
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

  name { ^"HOAConverterAcnSn3d2FuMa2" }


  info { ^"Generated with Faust" }
}

