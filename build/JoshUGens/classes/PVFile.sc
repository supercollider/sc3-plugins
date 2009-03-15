PVFile : File {
	var <header, <nFrames, <nBins, <sndDur, <buffer, <server, <data;
	var <magicNum, <headerSize, <fileSize, <fileFormat, <sr, <numChannels, <windowSize, 
			<hopSize, <frameBSize, <pvpvoc, <minFreq, <maxFreq, <spacing;
	var <len, <signal, <le, <path, filepath, <loaded, <magScale;	
	
	*new {arg path, server;
		^super.new(path, "rb").init(path, server);
		}

	*load {arg path, server;
		^this.new(path, server).load;
		}

	init {arg argpath, argserver;
		path = argpath;
		server = argserver ?? {Server.default};
		loaded = false;
		this.isOpen.if({
			this.readHeader;
			}, {
			("PVFile couldn't find a file at: "++path).warn;
			})
		}
		
	readHeader {
		len = this.length /  4;
		this.checkMN.if({
			magicNum = 517730;
			le.if({
				headerSize = this.getInt32LE;
				fileSize = this.getInt32LE;
				fileFormat = this.getInt32LE;
				sr = this.getFloatLE;
				numChannels = this.getInt32LE;
				windowSize = this.getInt32LE;
				hopSize = this.getInt32LE;
				frameBSize = this.getInt32LE;
				pvpvoc = this.getInt32LE;
				minFreq = this.getFloatLE;
				maxFreq = this.getFloatLE;
				spacing = this.getInt32LE;
				}, {
				headerSize = this.getInt32;
				fileSize = this.getInt32;
				fileFormat = this.getInt32;
				sr = this.getFloat;
				numChannels = this.getInt32;
				windowSize = this.getInt32;
				hopSize = this.getInt32;
				frameBSize = this.getInt32;
				pvpvoc = this.getInt32;
				minFreq = this.getFloat;
				maxFreq = this.getFloat;
				spacing = this.getInt32;			
				});
			header = [magicNum, headerSize, fileSize, fileFormat, sr, numChannels, windowSize, 
				hopSize, frameBSize, pvpvoc, minFreq, maxFreq, spacing];
			nFrames = (((len - headerSize) * 4) / frameBSize).ceil;
			nBins = frameBSize / 8;
			sndDur = (nFrames * hopSize) / sr;
			this.readData;
			this.close;
			this.calcMagScale;
			}, {"This does not appear to be an PV file".warn})
			
		}

	checkMN {
		case
			{this.getInt32 == 517730} {le = false; ^true}
			{this.pos_(0); this.getInt32LE == 517730} {le = true; ^true}
			{true} {"This does not appear to be an PV file".warn; this.close; ^false}
		}
			
	readData {
		var tmp;
		this.pos_(headerSize);
		tmp = le.if({
			FloatArray.fill(len - header.size, {this.getFloatLE})
			}, {
			FloatArray.fill(len - header.size, {this.getFloat})
			});
		data = header.as(FloatArray) ++ tmp;
		}
		
	// saves the PV file as an Aiff that can be loaded to the server
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

	loadToBuffer {arg buf;
		var size, numturns, numsamps, remainder, cond;
		server.serverRunning.if({
			buffer = buf ?? {server.bufferAllocator.alloc(1)};
			this.saveForSC;
			cond = Condition.new;
			Routine.run({
				server.sendMsgSync(cond, \b_allocRead, buffer, filepath);
				("PV file loaded to buffer "++buffer).postln;
				loaded = true;
				});
			}, {
			("The " ++ server.name ++ " server doesn't appear to be running. Boot the server before loading an PV file").warn;
			})
		}
		
	freeBuffer {
		loaded.if({
			server.sendMsg(\b_free, buffer);
			("Buffer "++buffer++" freed").postln;
			loaded = false;
			}, {
			"No data was loaded".warn;
			});
		}
		
	getBinFreqs {arg bin;
		var frameOffset, binOffset;
		binOffset = bin * 2 + 1;
		^Array.fill(nFrames, {arg frame; 
			frameOffset = frame * nBins * 2;
			data[header.size + frameOffset + binOffset]
			});
		}

	getBinMags {arg bin;
		var frameOffset, binOffset;
		binOffset = bin * 2;
		^Array.fill(nFrames, {arg frame; 
			frameOffset = frame * nBins * 2;
			data[header.size + frameOffset + binOffset]
			});
		}	
	
	getFrameFreqs {arg frame;
		var frameOffset, binOffset;
		frameOffset = frame * nBins * 2;
		^Array.fill(nBins, {arg bin; 
			binOffset = bin * 2 + 1;
			data[header.size + frameOffset + binOffset]
			});
		}

	getFrameMags {arg frame;
		var frameOffset, binOffset;
		frameOffset = frame * nBins * 2;
		^Array.fill(nBins, {arg bin; 
			binOffset = bin * 2;
			data[header.size + frameOffset + binOffset]
			});
		}

	freqs {
		^Array.fill(nBins, {arg i; this.getBinFreqs(i)});
		}

	mags {
		^Array.fill(nBins, {arg i; this.getBinMags(i)});
		}
		
	calcMagScale {
		magScale = (this.mags.flat.mean * nBins).reciprocal;
		}
		
	bufnum {^buffer}
	asControlInput { ^buffer }
	asUGenInput { ^buffer }
}