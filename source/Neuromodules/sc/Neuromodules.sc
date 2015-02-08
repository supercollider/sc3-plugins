MultiOutDemandUGen : DUGen {
	// a class for UGens with multiple outputs
	var <channels;

	*newFromDesc { arg rate, numOutputs, inputs;
		^super.new.rate_(rate).inputs_(inputs).initOutputs(numOutputs, rate)
	}

	initOutputs { arg numChannels, rate;
		channels = Array.fill(numChannels, { arg i;
			OutputProxy(rate, this, i);
		});
		if (numChannels == 1, {
			^channels.at(0)
		});
		^channels
	}

	numOutputs { ^channels.size }
	writeOutputSpecs { arg file;
		channels.do({ arg output; output.writeOutputSpec(file); });
	}
	synthIndex_ { arg index;
		synthIndex = index;
		channels.do({ arg output; output.synthIndex_(index); });
	}
}


DNeuromodule2 : MultiOutDemandUGen {

	*new { |in1 = 0, in2 = 0, x1 = 0, x2 = 0, w11 = 0, w12 = 0, w21 = 0, w22 = 0|
		^super.multiNew(\demand, in1, in2, x1 = 0, x2 = 0, w11, w12, w21, w22)
	}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1)
		];
		^channels
	}
	checkInputs { ^this.checkNInputs(1) } // TODO: check.

}


