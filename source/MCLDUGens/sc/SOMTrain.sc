// SOM UGens for SuperCollider, (c) 2008 Dan Stowell. Published under the terms of the GPL (v2 or, at your option, any later version)

SOMTrain : MultiOutUGen {

	*kr { |bufnum, inputdata, netsize=10, numdims=2, traindur=5000, nhood=0.5, gate=1, initweight=1|

		inputdata = inputdata.asArray;
		^this.multiNew('control', bufnum, netsize, numdims, traindur, nhood, gate, initweight, *inputdata)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
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
		buf.loadCollection(this.makeGrid(buf.numChannels, netsize, numdims, spinmatrix, offsets, scales).flat);
	}
	*makeGrid {  |numchans, netsize, numdims, spinmatrix, offsets=0, scales=1|
		var step, array, row, norm, minval = -1, maxval = 1;

		step = (maxval - minval) / (netsize - 1);
		array = this.pr_makegrid(minval, step, maxval, numdims);

		if(offsets.isArray.not){ offsets = offsets.dup(numchans) };
		if( scales.isArray.not){  scales =  scales.dup(numchans) };

		// Apply the rotation
		array = array.collect{|point|
			spinmatrix.collect{|spincol, scindex|
				(point * spincol).sum * scales[scindex] + offsets[scindex]
			}
		};
		^array
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

	// Convenience to take a data file ("path", path to an AIFF/WAV/etc) and train a SOM from it.
	// The WAV/AIFF shouldn't (normally) be a standard audio file, rather a multi-channel file of your data.
	// "init" can be path to an AIFF file with initial SOM coords, or a symbol for \rand or \gridrand init
	// "numruns" is how many times to play the data file through. More than once will give smoother fit.
	*trainFromFile { | path, init=\gridrand, numdims=2, netsize=10, numruns=1, server, action, nhood=0.5, initweight=1 |
		var sombuf, databuf, sf, numFramesToDo, dur, numFeatures;
		if(server.isNil){ server = Server.default };
		if(File.exists(path).not){ Error("File not found: %".format(path)).throw};
		if(action.isNil){action = path.splitext[0] ++ "-SOMcoords.aiff"};

		server.waitForBoot {

			// Check data file
			sf = SoundFile(path);
			if(sf.openRead){
				numFramesToDo = numruns * sf.numFrames;
				dur = numFramesToDo * server.options.blockSize / server.sampleRate; // We DON'T use the soundfile's own sample rate
				"data file will play % time(s) though % data points, taking % seconds".format(numruns, sf.numFrames, dur).postln;
				numFeatures = sf.numChannels;
			}{
				Error("trainFromFile: Could not read header of data file %".format(path)).throw
			};
			sf.close;

			// Check initialisation file, if wanted
			if(init.isString){
				sf = SoundFile(init);
				if(sf.openRead){
					// Check that the init file has the expected numdims and netsize and numFeatures
					if(sf.numChannels != numFeatures){
						Error("trainFromFile: data file has % features, yet % channels in init file %"
						.format(numFeatures, sf.numChannels, init)).throw
					};
					if(sf.numFrames != (netsize ** numdims)){
						Error("trainFromFile: called with netsize=% and numdims=%, yet % frames in init file %"
							.format(netsize, numdims, sf.numFrames, init)).throw
					};
				}{
					Error("trainFromFile: Could not read header of init file %".format(init)).throw
				};
				sf.close;
			};

			// Load the audio buffer, init the SOM buffer
			databuf = Buffer.read(server, path);
			if(init.isString){
				sombuf = Buffer.read(server, init);
			}{
				sombuf = this.allocBuf(server, netsize, numdims, numFeatures, init)
			};
			0.6.wait;
			server.sync;
			"sombuf has % channels and % frames".format(sombuf.numChannels, sombuf.numFrames).postln;

			// Run the synth, playing the data back at one-frame-per-control-block
			{
				var data;
				data = PlayBuf.kr(numFeatures, databuf, loop: 1);
				SOMTrain.kr(sombuf, data, netsize, numdims, /* traindur: */ numFramesToDo, nhood, 1, initweight);
				Line.kr(dur, 0, dur, doneAction: 2).poll(0.1, "trainFromFile time remaining");
				0 // silence
			}.play(server);

			// Wait a decent amount of time
			(dur + 0.5).wait;

			// Then perform the action (giving it the lovely buffer). NB if action is string we write to disk.
			if(action.isString){
				sombuf.write(action, "AIFF", "float")
			}{
				action.value(sombuf)
			};

			// Then wait a bit and sync before tidying up
			0.6.wait;
			server.sync;
			"trainFromFile: done".postln;
			0.2.wait;
			[databuf, sombuf].do(_.free);
		}
	}

} // End SOMTrain class

////////////////////////////////////////////////////////////////////////////////////////////////////

SOMRd : MultiOutUGen {

	*kr { |bufnum, inputdata, netsize=10, numdims=2, gate=1|
		if((numdims < 1) or: {numdims > 4})
		{Error("numdims must be between 1 and 4").throw};
		inputdata = inputdata.asArray;
		^this.multiNew('control', bufnum, netsize, numdims, gate, *inputdata)
	}
	*ar { |bufnum, inputdata, netsize=10, numdims=2, gate=1|
		if((numdims < 1) or: {numdims > 4})
		{Error("numdims must be between 1 and 4").throw};
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
		// note: would like to use .asInteger but can't use that in a synthdef
		bufindex = bufindex.round;
		netsize  = netsize.round;
		// but numdims can't go into the synthdef, must be evaluated by lang:
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


SOMAreaWr : UGen {

	*kr { |bufnum, inputdata, coords, netsize=10, numdims=2, nhood=0.5, gate=1|
		inputdata = inputdata.asArray ++  coords.asArray;
		^this.multiNew('control', bufnum, netsize, numdims, nhood, gate, *inputdata)
	}
}

KMeansRT : UGen {

	*kr { |bufnum, inputdata, k=5, gate=1, reset=0, learn=1|
		inputdata = inputdata.asArray;
		^this.multiNew('control', bufnum, k, gate, reset, learn, *inputdata)
	}
	*getCentroid { |bufnum, classif, ndims|
		^BufRd.kr(ndims + 1, bufnum, classif, interpolation: 1)[..ndims-1];
	}
}

