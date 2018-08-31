Logger : UGen {

	*kr { arg inputArray, trig=0.0, bufnum=0, reset=0.0;
		// Note rearranged args - code expects buffer ref first, and input data last
		^this.multiNewList(['control', bufnum, trig, reset] ++ inputArray.asArray)
	}
	*categories { ^ #["UGens>Buffer"] }
}

ListTrig : UGen {

	*kr { arg bufnum=0, reset=0, offset=0, numframes;
		if(numframes.isNil, {numframes = BufFrames.kr(bufnum)});
		^this.multiNewList(['control', bufnum, reset, offset, numframes])
	}
	*categories { ^ #["UGens>Buffer"] }
}

ListTrig2 : UGen {

	*kr { arg bufnum=0, reset=0, numframes;
		if(numframes.isNil, {numframes = BufFrames.kr(bufnum)});
		^this.multiNewList(['control', bufnum, reset, numframes])
	}
	*categories { ^ #["UGens>Buffer"] }
}

GaussClass : UGen {
	*kr { | in, bufnum=0, gate=0 |
		^this.multiNewList(['control', bufnum, gate] ++ in.asArray)
	}

	*categories { ^ #["UGens>Analysis", "UGens>Buffer"] }

	// This method REQUIRES MATHLIB QUARK. Convenience for preparing data.
	// Input is an array of classes, each specified as [mean, covariance, weight].
	*classesToFloatArray { |arr|
		var numclasses, numdims, classsize, cov;
		numclasses = arr.size;
		numdims = arr[0][0].size; // size of first mean-array
		classsize = numdims + (numdims*numdims) + 2; // number of floats for each class

		^arr.collect{|cl|
			cov = Matrix.with(cl[1]);

			cl[0]		// mean
			++ cov.inverse // inverse of covariance matrix
			++ (cl[2] / cov.det.sqrt)	// weight / sqrt of determinant of covariance
		}.flat.as(FloatArray);


	}
}

BufMax : MultiOutUGen {

	*kr { arg bufnum=0, gate=1;
		^this.multiNew('control', bufnum, gate)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
	*categories { ^ #["UGens>Buffer"] }
}

BufMin : BufMax {}

/*
{ArrayMax.ar(SinOsc.ar(1000, [0, pi]))}.plot(1)
{ArrayMax.kr(SinOsc.kr(1000, [0, pi]))}.plot(1)
*/

ArrayMax : MultiOutUGen {
	*ar { | array |
		^this.multiNewList(['audio'] ++ array.asArray)
	}
	*kr { | array |
		^this.multiNewList(['control'] ++ array.asArray)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1)
		];
		^channels
	}
	*categories { ^ #["UGens>Multichannel", "UGens>Analysis"] }
}

ArrayMin : ArrayMax {}

/*
MIDelay : UGen {

	*kr { |in1, in2, maxdelay= 0.2, gate=1, mibuf= -1|
		^this.multiNewList(['control', in1, in2, maxdelay, gate, mibuf])
	}
	*categories { ^ #["UGens>Buffer"] }
}
*/
