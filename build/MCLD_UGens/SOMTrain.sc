// SOM UGens for SuperCollider, (c) 2008 Dan Stowell. Published under the terms of the GPL (v2)

SOMTrain : MultiOutUGen {
	
	*kr { |bufnum, inputdata, netsize=10, numdims=2, traindur=5000, nhood=0.5, gate=1, initweight=1|
		if((numdims < 1) or: {numdims > 4})
			{"numdims must be between 1 and 4".error};
		
		inputdata = inputdata.asArray;
		^this.multiNew('control', bufnum, netsize, numdims, traindur, nhood, gate, initweight, *inputdata)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
	
	// Conveniences
	*allocBuf { |server, netsize, numdims, numinputchannels|
		^Buffer.alloc(server ? Server.default, netsize ** numdims, numinputchannels)
	}
	
	*initBufRand { |buf, minval, maxval|
		
		if(minval.isArray.not){
			minval = (minval ? -1).dup(buf.numChannels)
		};
		if(maxval.isArray.not){
			maxval = (maxval ?  1).dup(buf.numChannels)
		};
		
		
		buf.loadCollection({
			buf.numChannels.collect{|index| 
				1.0.rand.linlin(0, 1, minval[index], maxval[index])
			}
		}.dup(buf.numFrames).flat);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SOMRd : MultiOutUGen {
	
	*kr { |bufnum, inputdata, netsize=10, numdims=2, gate=1|
		if((numdims < 1) or: {numdims > 4})
			{^"numdims must be between 1 and 4".error};
		inputdata = inputdata.asArray;
		^this.multiNew('control', bufnum, netsize, numdims, gate, *inputdata)
	}
	*ar { |bufnum, inputdata, netsize=10, numdims=2, gate=1|
		if((numdims < 1) or: {numdims > 4})
			{^"numdims must be between 1 and 4".error};
		inputdata = inputdata.asArray;
		^this.multiNew('audio', bufnum, netsize, numdims, gate, *inputdata)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		// This unit outputs the co-ordinate of the best-matching node, i.e. its integer index in the grid. 
		// NOT the template vector - you can work that out yrself!
		^this.initOutputs(inputs[2], rate);
	}
	
	// SOMRd returns node index. If you want to retrieve the template vector at that node, this will convert it to the relevant frame index:
	*coordsToBufIndex { |coords, netsize=10|
		coords = coords.asArray;
		^coords.size.switch(
			1, { coords[0] },
			2, { coords[0] + (coords[1] * netsize)},
			3, { coords[0] + (coords[1] * netsize) + (coords[2] * (netsize * netsize))},
			4, { coords[0] + (coords[1] * netsize) + (coords[2] * (netsize * netsize)) 
					+ (coords[3] * (netsize * netsize * netsize))}
			);
	}
}
