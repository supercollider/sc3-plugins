PermModT : UGen{
	*ar{
		arg in, outfreq =440, infreq=5000, mul= 1.0, add= 0.0;
		^this.multiNew('audio', in, outfreq, infreq).madd(mul,add)
	}
	checkInputs {
 		if (rate == 'audio' and: {inputs.at(0).rate != 'audio'}, {
 			^("first input is not audio rate: " + inputs.at(0) + inputs.at(0).rate);
 		});

 		^this.checkValidInputs
 	}

}

PermMod : UGen{
	*ar{
		arg in, freq=100, mul= 1.0, add= 0.0;
		^this.multiNew('audio', in, freq).madd(mul,add)
	}
	checkInputs {
 		if (rate == 'audio' and: {inputs.at(0).rate != 'audio'}, {
 			^("first input is not audio rate: " + inputs.at(0) + inputs.at(0).rate);
 		});

 		^this.checkValidInputs
 	}

}

PermModArray : UGen{
	*ar{
		arg in, freq=50, pattern=[1,0], mul= 1.0, add= 0.0;
		^this.multiNewList(['audio', in, freq]++pattern.asArray).madd(mul,add);
	}
	checkInputs {
 		if (rate == 'audio' and: {inputs.at(0).rate != 'audio'}, {
 			^("first input is not audio rate: " + inputs.at(0) + inputs.at(0).rate);
 		});

 		^this.checkValidInputs
 	}

}