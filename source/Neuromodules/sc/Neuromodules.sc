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


Dneuromodule : MultiOutDemandUGen {

	*new { |numChannels, theta, x, weights|
		^super.multiNew(\demand, *[numChannels] ++ theta ++ x ++ weights )
	}

	*ar { |dt, numChannels, theta, x, weights|
		^Duty.ar(dt, 0, this.new(numChannels, theta, x, weights))
	}

	*kr { |dt, numChannels, theta, x, weights|
		^Duty.ar(dt, 0, this.new(numChannels, theta, x, weights))
	}

	init { arg numChannels ... theInputs;
		inputs = [numChannels] ++ theInputs;
		channels = numChannels.collect { |i|
			OutputProxy(rate, this, i)
		};
		^channels
	}
	checkInputs { ^this.checkNInputs(1) } // TODO: check.

}


