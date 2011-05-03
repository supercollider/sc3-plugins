AtsFile : File {
	var <path, server, le, <bufnum, filepath, numSamples;
	var <header, loaded, <data;
	// header data
	var magic, <sr, <frameSize, <winSize, <numPartials, <numFrames, <ampMax, <freqMax, <sndDur,
		<atsType;
	var offset, offset2, increment;

	*new {arg path, server;
		var file;
		^file = super.new(path, "rb").init(path, server);
		}

	*load {arg path, server;
		^this.new(path, server).load;
		}

	buffer {
		^bufnum
		}

	init {arg argpath, argserver;
		path = argpath;
		server = argserver ?? {Server.default};
		loaded = false;
		this.readHeader;
		}

	readHeader {
		this.checkMN.if({
			header = DoubleArray.fill(9, {le.if({this.getDoubleLE}, {this.getDouble})});
			header = header.addFirst(123);
			#magic, sr, frameSize, winSize, numPartials, numFrames, ampMax, freqMax, sndDur,
				atsType = header;
			numSamples = this.length * 0.125;
			offset = [ 0, 2, 3, 2, 3 ].at(atsType.asInteger);
			offset2 = [0, 1, 1, 26, 26].at(atsType.asInteger);
			increment = numPartials * offset + offset2;
			this.readData;
			this.close;
			})
		}

	removeZeroFreqs {
		numPartials.do({arg thisPar;
			var thisParData, lastVal;
			thisParData = this.getParFreq(thisPar);
			lastVal = thisParData[0];
			thisParData.do({arg thisFreq, inc;
				(thisFreq == 0).if({
					data[10 + (inc * increment + 2 + (thisPar * offset))] = lastVal;
				}, {
					lastVal = thisFreq;
				})
			})
		})
	}

	checkMN {
		case
			{this.getDouble == 123} {le = false; ^true}
			{this.pos_(0); this.getDoubleLE == 123} {le = true; ^true}
			{true} {"This does not appear to be an ATS file".warn; this.close; ^false}
		}

	readData {
		this.pos_(0);
		data = le.if({
			DoubleArray.fill(numSamples, {this.getDoubleLE})
			}, {
			DoubleArray.fill(numSamples, {this.getDouble})
			});
		this.removeZeroFreqs;
		}

	// saves the ATS file as an Aiff that can be loaded to the server
	saveForSC {arg path;
		var sndfile;
		sndfile = SoundFile.new;
		sndfile.sampleRate_(44100);
		sndfile.sampleFormat("float32");
		filepath = path ?? {PathName.tmp ++ sndfile.hash.asString};
		sndfile.openWrite(filepath);
		sndfile.writeData(data);
		sndfile.close;
		}

	save {arg path;
		var fil;
		path.notNil.if({
			fil = File.new(path, "wb");
			fil.write(data);
			fil.close;
			}, {
			"Please specify a path to save the file to".warn;
			})
		}

	load {arg buffer;
		var size, numturns, numsamps, remainder, cond;
		server.serverRunning.if({
			bufnum = buffer ?? {server.bufferAllocator.alloc(1)};
			this.saveForSC;
			cond = Condition.new;
			Routine.run({
				server.sendMsgSync(cond, \b_allocRead, bufnum, filepath);
				("Ats file loaded to buffer "++bufnum).postln;
				loaded = true;
				});
			}, {
			("The " ++ server.name ++ " server doesn't appear to be running. Boot the server before loading an ATS file").warn;
			})
		}

	loadToBuffer {arg buffer;
		this.load(buffer);
		}

	freeBuffer {
		loaded.if({
			server.sendMsg(\b_free, bufnum);
			("Buffer "++bufnum++" freed").postln;
			loaded = false;
			}, {
			"No data was loaded".warn;
			});
		}

	getParTime { arg par;
		^Array.fill(numFrames, {arg i; data[10 + (i * increment)]})
		}

	getParFreq {arg par;
		^Array.fill(numFrames, {arg i; data[10 + (i * increment + 2 + (par * offset))]});
		}

	getParAmp {arg par;
		^Array.fill(numFrames, {arg i; data[10 + (i * increment + 1 + (par * offset))]});
		}

	getParPhase {arg par;
		((atsType == 2) || (atsType == 4)).if({
			^Array.fill(numFrames, {arg i; data[10 + (i * increment + 3 + (par * 3))]});
			}, {
			("No phase information in Ats file type " ++ atsType).warn;
			});
		}

	getBandNoi {arg band;
		(band <= 24).if({
			(atsType >= 3).if({
				^Array.fill(numFrames, {arg i;
					data[10 + (i * increment) + (numPartials * offset) + 1 + band]
					})
				}, {
				"This ATS file doesn't appear to contain noise information".warn;
				^nil
				})
			}, {
			"Band number must be 24 or less".warn;
			^nil
			})
		}

	getFrameFreq {arg frame;
		^Array.fill(numPartials, {arg i; data[10 + (frame * increment) + (i * offset) + 2]});
		}

	getFrameAmp {arg frame;
		^Array.fill(numPartials, {arg i; data[10 + (frame * increment) + (i * offset) + 1]});
		}

	getFramePhase {arg frame;
		((atsType == 2) || (atsType == 4)).if({
			^Array.fill(numPartials, {arg i; data[10 + (frame * increment) + (i * offset) + 3]});
			}, {
			("No phase information in Ats file type " ++ atsType).warn;
			});
		}

	getFrameNoi {arg frame;
		(atsType > 2).if({
			^Array.fill(25, {arg i;
				data[10 + (frame * increment) + (numPartials * offset) + 1 + i]})
			}, {
			"This ATS file doesn't appear to contain noise information".warn;
			^nil
			});
		}

	asControlInput { ^bufnum }
	asUGenInput { ^bufnum }

}