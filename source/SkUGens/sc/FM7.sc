FM7 : MultiOutUGen {
	*numOperators { ^6 }
	*numControls { ^3 }
	*controlArraySize { ^this.numControls * this.numOperators }
	*modArraySize { ^this.numOperators.squared }
	*numRequiredInputs { ^this.controlArraySize + this.modArraySize }

	*controlMatrix { | ... args |
		var matrix;
		matrix = Array.fill2D(this.numOperators, this.numControls, 0);
		args.do { | x |
			matrix[x[0]] = x.copyToEnd(1);
		};
		^matrix
	}
	*modMatrix { | ... args |
		var matrix;
		matrix = Array.fill2D(this.numOperators, this.numOperators, 0);
		args.do { | x |
			matrix[x[0]][x[1]] = x[2];
		};
		^matrix
	}

	*algoSpec { | algo, feedback=0.0 |
		var matrix;
		matrix = this.modMatrix;
		^[
			{[
				this.modMatrix(
					[0, 1, 1],
					[2, 3, 1],
					[3, 4, 1],
					[4, 5, 1],
					[5, 5, feedback]
				),  [0, 2]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[1, 1, feedback],
					[2, 3, 1],
					[3, 4, 1],
					[4, 5, 1]
				),  [0, 2]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[1, 2, 1],
					[3, 4, 1],
					[4, 5, 1],
					[5, 5, feedback]
				),  [0, 3]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[1, 2, 1],
					[3, 4, 1],
					[4, 5, 1],
					[5, 3, feedback]
				),  [0, 3]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[2, 3, 1],
					[5, 5, feedback],
				),  [0, 2, 4]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[2, 3, 1],
					[5, 4, feedback],
				),  [0, 2, 4]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[2, 3, 1],
					[2, 4, 1],
					[4, 5, 1],
					[5, 5, feedback],
				),  [0, 2]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[2, 3, 1],
					[2, 4, 1],
					[4, 5, 1],
					[3, 3, feedback],
				),  [0, 2]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[2, 3, 1],
					[2, 4, 1],
					[4, 5, 1],
					[1, 1, feedback],
				),  [0, 2]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[1, 2, 1],
					[3, 4, 1],
					[3, 5, 1],
					[2, 2, feedback],
				),  [0, 3]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[1, 2, 1],
					[3, 4, 1],
					[3, 5, 1],
					[5, 5, feedback],
				),  [0, 3]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[1, 2, 1],
					[2, 3, 1],
					[2, 4, 1],
					[2, 5, 1],
					[2, 2, feedback],
				),  [0, 2]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[1, 2, 1],
					[2, 3, 1],
					[2, 4, 1],
					[2, 5, 1],
					[5, 5, feedback],
				),  [0, 2]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[2, 3, 1],
					[2, 3, 1],
					[3, 4, 1],
					[3, 5, 1],
					[5, 5, feedback],
				),  [0, 2]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[2, 3, 1],
					[2, 3, 1],
					[3, 4, 1],
					[3, 5, 1],
					[1, 1, feedback],
				),  [0, 2]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[0, 2, 1],
					[0, 4, 1],
					[2, 3, 1],
					[4, 5, 1],
					[5, 5, feedback],
				),  [0]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[0, 2, 1],
					[0, 4, 1],
					[2, 3, 1],
					[4, 5, 1],
					[1, 1, feedback],
				),  [0]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[0, 2, 1],
					[0, 3, 1],
					[3, 4, 1],
					[4, 5, 1],
					[2, 2, feedback],
				),  [0]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[1, 2, 1],
					[3, 5, 1],
					[4, 5, 1],
					[5, 5, feedback],
				),  [0, 3, 4]
			]},
			{[
				this.modMatrix(
					[0, 2, 1],
					[1, 2, 1],
					[3, 4, 1],
					[3, 5, 1],
					[2, 2, feedback],
				),  [0, 1, 3]
			]},
			{[
				this.modMatrix(
					[0, 2, 1],
					[1, 2, 1],
					[3, 5, 1],
					[4, 5, 1],
					[2, 2, feedback],
				),  [0, 1, 3, 4]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[2, 5, 1],
					[3, 5, 1],
					[4, 5, 1],
					[5, 5, feedback],
				),  [0, 2, 3, 4]
			]},
			{[
				this.modMatrix(
					[1, 2, 1],
					[3, 5, 1],
					[4, 5, 1],
					[5, 5, feedback],
				),  [0, 1, 3, 4]
			]},
			{[
				this.modMatrix(
					[2, 5, 1],
					[3, 5, 1],
					[4, 5, 1],
					[5, 5, feedback],
				),  [0, 1, 2, 3, 4]
			]},
			{[
				this.modMatrix(
					[3, 5, 1],
					[4, 5, 1],
					[5, 5, feedback],
				),  [0, 1, 2, 3, 4]
			]},
			{[
				this.modMatrix(
					[1, 2, 1],
					[3, 4, 1],
					[3, 5, 1],
					[5, 5, feedback],
				),  [0, 1, 3]
			]},
			{[
				this.modMatrix(
					[1, 2, 1],
					[3, 4, 1],
					[3, 5, 1],
					[2, 2, feedback],
				),  [0, 1, 3]
			]},
			{[
				this.modMatrix(
					[0, 1, 1],
					[2, 3, 1],
					[3, 4, 1],
					[4, 4, feedback],
				),  [0, 2, 5]
			]},
			{[
				this.modMatrix(
					[2, 3, 1],
					[4, 5, 1],
					[5, 5, feedback],
				),  [0, 1, 2, 4]
			]},
			{[
				this.modMatrix(
					[2, 3, 1],
					[3, 4, 1],
					[4, 4, feedback],
				),  [0, 1, 2, 5]
			]},
			{[
				this.modMatrix(
					[4, 5, 1],
					[5, 5, feedback],
				),  [0, 1, 2, 3, 4]
			]},
			{[
				this.modMatrix(
					[5, 5, feedback],
				),  [0, 1, 2, 3, 4, 5]
			]},
		].at(algo).value
	}

	*ar { | ctlMatrix, modMatrix |
		^this.multiNewList(
			['audio']
			++ (ctlMatrix ?? { this.controlMatrix }).flatten(1)
			++ (modMatrix ?? { this.modMatrix }).flatten(1)
		)
	}
	*arAlgo { | algo=0, ctlMatrix, feedback=0.0 |
		var modMatrix, channels;
		#modMatrix, channels = this.algoSpec(algo, feedback).postln;
		^this.ar(ctlMatrix, modMatrix).slice(channels)
	}

	init { | ... args |
		inputs = args;
		^this.initOutputs(this.class.numOperators, rate)
	}
	checkInputs {
		^if (inputs.size != this.class.numRequiredInputs) {
			this.class.numRequiredInputs.asString + "inputs required (" ++ inputs.size ++ ")"
		}
	}
}

// EOF
