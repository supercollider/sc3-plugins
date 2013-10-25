JPverbLite {
	*ar { | in, t60(1.0), damp(0.0), size(1.0), earlyDiff(0.707)|
		in = in.asArray;

		^JPverbLiteRaw.ar(in.first, in.last, damp, earlyDiff, size, t60)
	}
}

JPverbLiteRaw : MultiOutUGen
{
	*ar { | in1, in2, damp(0.0), earlyDiff(0.707), size(1.0), t60(1.0) |
		^this.multiNew('audio', in1, in2,  damp, earlyDiff, size, t60)
	}

	*kr { | in1, in2, damp(0.0), earlyDiff(0.707), size(1.0), t60(1.0) |
		^this.multiNew('control', in1, in2, damp, earlyDiff, size, t60)
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
		^this.initOutputs(2, rate)
	}

	name { ^"JPverbLiteRaw" }
}
