//LPCBuffer {
//	var <header, <numFrames, <sndDur, <buffer, <>coefs, <server;
//	var <headerSize, <magicNum, <numPoles, <nvals, <frameRate, <srate, <nframes, <data;
//	var <resrms, <origrms, <nrmerr, <pchcps, <len, <signal, <windowsize, <duration, cond;
//
///*	*new {arg path, buffer, server;
//		server = server ? Server.default;
//		buffer = buffer ? server.bufferAllocator.alloc(1);
//		^super.new(path, "rb").init(buffer, server);
//		}
//*/
//	// creates a new buffer, but all other info is empty
//	// can use with saveToFile later?
//	*new {arg duration, windowsize, numPoles, buffer, server;
//		server = server ? Server.default;
//		buffer = buffer ? server.bufferAllocator.alloc(1);
//		^super.new.init(buffer, server, duration, windowsize, numPoles);
//		}
//
//	init {arg thisbuf, thisserver, thisduration, thiswindowsize, thisnumPoles;
//		buffer = thisbuf;
//		server = thisserver;
//		duration = thisduration;
//		windowsize = thiswindowsize;
//		numPoles = thisnumPoles;
//		this.allocateBuffer;
////		this.readHeader;
//		}
//
//	allocateBuffer {
//		// calculate buffer size
//		var buffersize;
//		cond = Condition.new;
//		nframes = ((duration * server.sampleRate) / (windowsize * 0.5)).ceil;
//		nvals = numPoles + 4;
//		buffersize = (nframes * nvals) + 3;
//		Routine.run({
//			server.sendMsg(\b_alloc, buffer, buffersize, 1);
//			("Buffer " ++ buffer ++ " created").postln;
//			server.sync(cond);
//			// load the first three values to the buffer
//			server.sendBundle(nil, [\b_setn, buffer, 0, 3, numPoles, nframes, nvals]);
//			});
//		}
///*
//	readHeader {
//		var restOfHeader, rest;
//		len = this.length /  4;
//		headerSize = this.getInt32;
//		magicNum = this.getInt32 == 999;
//		magicNum.if({
//			magicNum = 999;
//			numPoles = this.getInt32;
//			nvals = this.getInt32;
//			frameRate = this.getFloat;
//			srate = this.getFloat;
//			sndDur = this.getFloat;
//			rest = (headerSize - 28) / 4;
//			restOfHeader = Array.fill(rest, {this.getFloat});
//			header = [headerSize, magicNum, numPoles, nvals, frameRate, srate, sndDur] ++
//				restOfHeader;
//			nframes = (len - header.size) / nvals;
//			this.read;
//			}, {"This does not appear to be an LPC file".warn})
//
//		}
//
//	readData {
//		var datalen;
//		datalen = (this.length - headerSize);
//		this.pos_(headerSize);
//		data = Array.fill(datalen, {this.getFloat});
//		this.close;
//		}
//
//	read {
//		this.readData;
//		resrms = Array.newClear(nframes);
//		origrms = Array.newClear(nframes);
//		nrmerr = Array.newClear(nframes);
//		pchcps = Array.newClear(nframes);
//		coefs = Array.fill(numPoles, {Array.newClear(nframes)});
//		nframes.do{arg i;
//			var start;
//			start = nvals * i;
//			resrms[i] = data[start];
//			origrms[i] = data[start + 1];
//			nrmerr[i] = data[start + 2];
//			pchcps[i] = data[start + 3];
//			numPoles.do{arg j; coefs[j][i] = data[start + 4 + j]};
//			// coefs = [[pole1], [pole2], [pole3]]- each pole is nFrames long
//			};
//		//origrms = origrms * 32768.reciprocal;
//		origrms = origrms * (origrms.flat.maxItem * numPoles).reciprocal;
//		resrms = resrms * resrms.flat.maxItem.reciprocal;
//		"File read and sorted!".postln;
//
//		}
//*/
//
//	// write out buffer data for later use
//	saveToFile { arg path;
//		server.sendMsg(\b_write, buffer, path, "AIFF", "float");
//		}
//
///*	loadToBuffer {
//		var c, inc, size, numcycles, tmp;
//		c = Condition.new;
//		Routine.run{
//			this.server.sync(c);
//			inc = 3;
//			size = 2**((len).log2.ceil);
//			signal = Signal.newClear(size);
//			signal[0] = this.numPoles; // the number of poles
//			signal[1] = this.nframes; // the number of frames
//			signal[2] = this.nvals; // number of values for each frame (for inc later in UGen)
//			this.resrms.do{arg val;
//				signal[inc] = val;
//				inc = inc + 1;
//				};
//			this.origrms.do{arg val;
//				signal[inc] = val;
//				inc = inc + 1;
//				};
//			this.nrmerr.do{arg val;
//				signal[inc] = val;
//				inc = inc + 1;
//				};
//			this.pchcps.do{arg val;
//				signal[inc] = val;
//				inc = inc + 1;
//				};
//			this.numPoles.do{arg k;
//				this.coefs[k].do{arg val;
//					signal[inc] = val;
//					inc = inc + 1;
//					}
//				};
//
//			tmp = Buffer.loadCollection(this.server, signal);
//			buffer = tmp.bufnum;
//			//
////			this.server.sendMsgSync(c, \b_alloc, this.buffer, size);
////
////			numcycles.do{arg i;
////				this.server.listSendMsg([\b_setn, this.buffer, 512*i, 512] ++
////					signal[(512*i)..((512*i)+511)])};
////
//			("LPC data loaded to buffer "++ buffer.asString).postln;
//			}
//		} */
//	}
//