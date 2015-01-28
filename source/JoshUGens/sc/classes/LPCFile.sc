LPCFile : File {
	var <header, <sndDur, <buffer, <>coefs, <server, <path;
	var <headerSize, <magicNum, <numPoles, <nvals, <frameRate, <srate, <nframes, <data;
	var <>resrms, <>origrms, <>nrmerr, <>pchcps, <len, <signal, <restOfHeader, <le;

	*new {arg path, buffer, server;
		^super.new(path, "rb").init(buffer, server, path);
		}

	init {arg thisbuf, thisserver, thispath;
		thisserver = thisserver ? Server.default;
		server = thisserver;
		thisbuf = buffer.asUGenInput ? server.bufferAllocator.alloc(1);
		path = thispath;
		buffer = thisbuf;
		le = false;
		this.isOpen.if({
			this.readHeader;
			}, {
			("LPCFile couldn't find a file at: "++path).warn;
			})
		}

	readHeader {
		var rest;
		len = this.length /  4;
		headerSize = this.getInt32;
		magicNum = ((this.getInt32 == 999) or: {
			(this.pos_(4); this.getInt32LE == 999).if({
				le = true;
				this.pos_(0);
				headerSize = this.getInt32LE;
				this.pos_(8);
				true
				}, {
				false
				});
			});
		magicNum.if({
			magicNum = 999;
			le.if({
				numPoles = this.getInt32LE;
				nvals = this.getInt32LE;
				frameRate = this.getFloatLE;
				srate = this.getFloatLE;
				sndDur = this.getFloatLE;
				rest = (headerSize - 28) / 4;
				restOfHeader = Array.fill(rest, {this.getFloatLE});
				}, {
				numPoles = this.getInt32;
				nvals = this.getInt32;
				frameRate = this.getFloat;
				srate = this.getFloat;
				sndDur = this.getFloat;
				rest = (headerSize - 28) / 4;
				restOfHeader = Array.fill(rest, {this.getFloat});
				});
			header = [headerSize, magicNum, numPoles, nvals, frameRate, srate, sndDur] ++
				restOfHeader;
			nframes = (len - header.size) / nvals;
			this.read;
			}, {"This does not appear to be an LPC file".warn})

		}

	readData {
		var datalen;
		datalen = (this.length - headerSize);
		this.pos_(headerSize);
		le.if({
			data = Array.fill(datalen, {this.getFloatLE});
			}, {
			data = Array.fill(datalen, {this.getFloat});
			});
		this.close;
		}

	read {
		this.readData;
		resrms = Array.newClear(nframes);
		origrms = Array.newClear(nframes);
		nrmerr = Array.newClear(nframes);
		pchcps = Array.newClear(nframes);
		coefs = Array.fill(numPoles, {Array.newClear(nframes)});
		nframes.do{arg i;
			var start;
			start = nvals * i;
			resrms[i] = data[start];
			origrms[i] = data[start + 1];
			nrmerr[i] = data[start + 2];
			pchcps[i] = data[start + 3];
			numPoles.do{arg j; coefs[j][i] = data[start + 4 + j]};
			// coefs = [[pole1], [pole2], [pole3]]- each pole is nFrames long
			};
		//origrms = origrms * 32768.reciprocal;
		origrms = origrms * (origrms.flat.maxItem * numPoles).reciprocal;
		resrms = resrms * resrms.flat.maxItem.reciprocal;
		"File read and sorted!".postln;

		}

	// saves in a format that SC can use as a Buffer
	saveToFile { arg path;
		var data, sndfile, collection, startFrame;
		this.loadToSignal;
		collection = signal;
		startFrame = 0;
		server.isLocal.if({
			if(collection.isKindOf(RawArray).not,
				{data = collection.collectAs({|item| item}, FloatArray)}, {data = collection;}
			);
			sndfile = SoundFile.new;
			sndfile.sampleRate = server.sampleRate;
			sndfile.numChannels = 1;
			if(sndfile.openWrite(path),
				{
					sndfile.writeData(data);
					sndfile.close;
				}, {"Failed to write data".warn;}
			);
		}, {"cannot do saveToFile with a non-local Server".warn;});
	}

	loadToBuffer {
		var c, numcycles, tmp;
		c = Condition.new;
		Routine.run{
			this.server.sendMsg(\b_free, buffer);
			this.server.sync(c);
			this.loadToSignal;
			tmp = Buffer.alloc(this.server, signal.size, 1, bufnum: buffer);
			this.server.sync(c);
			tmp.loadCollection(signal, action: { c.unhang });
			c.hang;
			buffer = tmp.bufnum;
			("LPC data loaded to buffer "++ buffer.asString).postln;
			}
		}

	loadToSignal {
		var inc, size;
		inc = 3;
		size = 2**((len).log2.ceil);
		signal = Signal.newClear(size);
		signal[0] = this.numPoles; // the number of poles
		signal[1] = this.nframes; // the number of frames
		signal[2] = this.nvals; // number of values for each frame (for inc later in UGen)
		this.resrms.do{arg val;
			signal[inc] = val;
			inc = inc + 1;
			};
		this.origrms.do{arg val;
			signal[inc] = val;
			inc = inc + 1;
			};
		this.nrmerr.do{arg val;
			signal[inc] = val;
			inc = inc + 1;
			};
		this.pchcps.do{arg val;
			signal[inc] = val;
			inc = inc + 1;
			};
		this.numPoles.do{arg k;
			this.coefs[k].do{arg val;
				signal[inc] = val;
				inc = inc + 1;
				}
			};
		}

	saveLPCFile {arg path;
		var fil;
		fil = File.new(path, "wb");
		fil.putInt32(headerSize);
		fil.putInt32(magicNum);
		fil.putInt32(numPoles);
		fil.putInt32(nvals);
		fil.putFloat(frameRate);
		fil.putFloat(srate);
		fil.putFloat(sndDur);

		restOfHeader.do({arg me;
			fil.putFloat(me);
			});

		nframes.do({arg i;
			fil.putFloat(resrms[i]);
			fil.putFloat(origrms[i]);
			fil.putFloat(nrmerr[i]);
			fil.putFloat(pchcps[i]);
			coefs.do({arg me, j;
				fil.putFloat(me[i]);
				});
			});

		fil.close;
		}

	buffer_ {arg buf;
		buffer = buf.asUGenInput
		}

	asUGenInput {^buffer}
	free {
		this.server.sendMsg(\b_free, buffer);
		this.server.bufferAllocator.free(buffer);
		buffer = nil
	}
	bufnum {^buffer}
	asControlInput { ^buffer }
	}
