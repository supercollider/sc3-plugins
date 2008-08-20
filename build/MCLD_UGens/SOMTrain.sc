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
	*categories {	^ #["UGens>Analysis"]	}
	
	// Conveniences
	*allocBuf { |server, netsize, numdims, numinputchannels, init|
		var buf;
		buf = Buffer.alloc(server ? Server.default, netsize ** numdims, numinputchannels);
		if(init.notNil){
			Task{
				server.sync;
				init.switch(
					\rand,
						{ this.initBufRand(buf) },
					\gridrand, 
						{ this.initBufGridRand(buf, netsize, numdims) }
				)
			}.play(AppClock);
		};
		^buf;
	}
	
	*initBufRand { |buf, minval = -1, maxval = 1|
		
		if(minval.isArray.not){
			minval = minval.dup(buf.numChannels)
		};
		if(maxval.isArray.not){
			maxval = maxval.dup(buf.numChannels)
		};		
		
		buf.loadCollection({
			buf.numChannels.collect{|index| 
				1.0.rand.linlin(0, 1, minval[index], maxval[index])
			}
		}.dup(buf.numFrames).flat);
	}
	
	// spinmatrix is an array of buf.numChannels vectors, each of which has "numdims" entries
	*initBufGrid { |buf, netsize, numdims, spinmatrix, offsets = 0, scales = 1|
		var step, array, row, norm, minval = -1, maxval = 1;
		
		step = (maxval - minval) / (netsize - 1);
		array = this.pr_makegrid(minval, step, maxval, numdims);
		
		if(offsets.isArray.not){ offsets = offsets.dup(buf.numChannels) };
		if( scales.isArray.not){  scales =  scales.dup(buf.numChannels) };
		
		// Apply the rotation
		array = array.collect{|point|
			spinmatrix.collect{|spincol, scindex|
				(point * spincol).sum * scales[scindex] + offsets[scindex]
			}
		};
		
		array = array.flat;
		
		buf.loadCollection(array);
	}
	
	*initBufGridRand { |buf, netsize, numdims, offsets = 0, scales = 1|
		var step, array, spinmatrix, row, norm, minval = -1, maxval = 1;
		
		// Create a random orientation into the higher-dimensional feature-space
		spinmatrix = numdims.collect{
			row = buf.numChannels.collect{|chanindex| 3.0.sum3rand}; // 3.0.sum3rand creates quasi-gaussian with stdDev of about 1.0
			norm = row.sum{|val| val.squared}.sqrt;
			row / norm
		}.flop;
		// So spinmatrix is an array of buf.numChannels vectors, each of which has "numdims" entries
		
		this.initBufGrid(buf, netsize, numdims, spinmatrix, offsets, scales);
	}
	
	// Generates a an array, each one containing co-ordinates for a point in a grid
	*pr_makegrid { |from, step, to, dims|
		var less;
		^if(dims == 1){
			(from, from+step .. to).collect(_.asArray)
		}{
			less = this.pr_makegrid(from, step, to, dims-1);
			(from, from+step .. to).collect{|val| less.collect{|lval| lval ++ val} }.flatten
		}
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
	
	// And the reverse, in case you need it:
	*bufIndexToCoords { |bufindex, netsize=10, numdims=3|
		bufindex = bufindex.asInteger;
		netsize  = netsize.asInteger;
		numdims  = numdims.asInteger;
		^numdims.switch(
			1, { bufindex },
			2, { [bufindex.mod(netsize), (bufindex/netsize).floor] },
			3, { [bufindex.mod(netsize), (bufindex/netsize).floor.mod(netsize), (bufindex/(netsize*netsize)).floor] },
			4, { [bufindex.mod(netsize), (bufindex/netsize).floor.mod(netsize), 
					(bufindex/(netsize*netsize)).floor.mod(netsize), (bufindex/(netsize*netsize*netsize)).floor] }
		)
	}
	/*
	Tests:
	9.collect{|val| SOMRd.bufIndexToCoords(val, 3, 2) }
	
	27.collect{|val| SOMRd.bufIndexToCoords(val, 3, 3) }.do(_.postln);""
	27.collect{|val| SOMRd.coordsToBufIndex(SOMRd.bufIndexToCoords(val, 3, 3), 3) }
	27.collect{|val| SOMRd.coordsToBufIndex(SOMRd.bufIndexToCoords(val, 3, 3), 3) } == (0..26)
	
	81.collect{|val| SOMRd.bufIndexToCoords(val, 3, 4) }.do(_.postln);""
	81.collect{|val| SOMRd.coordsToBufIndex(SOMRd.bufIndexToCoords(val, 3, 4), 3) }
	81.collect{|val| SOMRd.coordsToBufIndex(SOMRd.bufIndexToCoords(val, 3, 4), 3) } == (0..80)
	*/
}
