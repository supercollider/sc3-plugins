PVFile : File {
	var <header, <nFrames, <nBins, <sndDur, <buffer, <>coefs, <server, <data;
	var <magicNum, <headerSize, <fileSize, <fileFormat, <sr, <numChannels, <windowSize, 
			<hopSize, <frameBSize, <pvpvoc, <minFreq, <maxFreq, <spacing;
	var <freqs, <mags, <len, <signal;	
	
	*new {arg path, buffer, server;
		server = server ? Server.default;
		buffer = buffer ? server.bufferAllocator.alloc(1);
		^super.new(path, "rb").init(buffer, server);
		}
	
	init {arg thisbuf, thisserver;
		buffer = thisbuf;
		server = thisserver;
		this.readHeader;
		}
		
	readHeader {
		len = this.length /  4;
//		headerSize = this.getInt32;
		magicNum = this.getInt32 == 517730;
		magicNum.if({
			magicNum = 517730;
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
			header = [magicNum, headerSize, fileSize, fileFormat, sr, numChannels, windowSize, 
				hopSize, frameBSize, pvpvoc, minFreq, maxFreq, spacing];
			nFrames = (((len - headerSize) * 4) / frameBSize).ceil;
			nBins = frameBSize / 8;
			sndDur = (nFrames * hopSize) / sr;
			this.read;
			
			}, {"This does not appear to be an PV file".warn})
			
		}
	
	readData {
		var datalen;
		datalen = (this.length - headerSize);
		this.pos_(headerSize);
		data = Array.fill(datalen, {this.getFloat});
		this.close;
		}
		
	read {
		var maxMag;
		this.readData;
		freqs = Array.fill(nBins, {Array.newClear(nFrames)});
		mags = Array.fill(nBins, {Array.newClear(nFrames)});
		nBins.do{arg bin; // i is the bin number
			var start;
			start = bin * 2;
			nFrames.do{arg frame;
				var offset;
				offset = (frame * (nBins * 2));
				freqs[bin][frame] = data[start + 1 + offset]; // j is the frame number
				mags[bin][frame] = data[start + offset]
				};
			};
		maxMag = mags.flat.maxItem;
		mags = mags / (maxMag * nBins);

		"File read and sorted!".postln;
			
		}

	saveToFile { arg path;
		var data, sndfile, collection, startFrame;
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
		var c, inc, numpartials, numframes, numcycles, size, bandarray, tmp;
		c = Condition.new;
		Routine.run{

			// since we are loading so much data, we need to sync the server

			server.sync(c);

			inc = 4; // the first 4 slots will be data from the header, so, adjust the
				// the incrementer here
			size = 2**((len).log2.ceil);
			signal = Signal.newClear(size);
			signal[0] = 1; //ats file type
			signal[1] = nBins; //numbins
			signal[2] = nFrames; //numframes
			signal[3] = windowSize; //window size in samples
	
			this.mags.size.do{arg i;
				this.mags[i].size.do{arg j;
					signal[inc]= this.mags[i][j];
					inc = inc + 1;
					}};
			this.freqs.size.do{arg i;
				this.freqs[i].size.do{arg j;
					signal[inc]= this.freqs[i][j];
					inc = inc + 1;
					}};	

			// this will cause the server to wait until the buffer is allocated
			// before anything is loaded into it
			
			tmp = Buffer.loadCollection(this.server, signal);
			buffer = tmp.bufnum;
	
		("PV data loaded to buffer " ++ buffer.asString).postln
		}
	}

	}