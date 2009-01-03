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
		var numclasses, numdims, classsize, ret, cov;
		numclasses = arr.size;
		numdims = arr[0][0].size; // size of first mean-array
		classsize = numdims + (numdims*numdims) + 2; // number of floats for each class
		ret = FloatArray.newClear(classsize * numclasses);
		
		^arr.collect{|cl|
			cov = Matrix.with(cl[1]);
			
			cl[0]		// mean
			++ cov.inverse // inverse of covariance matrix
			++ (cl[2] / cov.det.sqrt)	// weight / sqrt of determinant of covariance
		}.flat.as(FloatArray);
		
		
	}
}

