// For reading and using ATS files.  By Don Craig and Josh Parmenter\

AtsFile : File {

	var <header, <data, <numFrames, <numPartials, <sndDur, <filename, <filetype, <buffer, <server;
	var <freqs, <amps, <phases, <parNoi, <bandNoi, <critBands, <parTables, signal ;
	var bige = nil;
	
	*new { 	arg path, server; 
			var file, inserver;
			inserver = server ? Server.default;
			^file = super.new(path, "rb").init(path, inserver);
	
	}
	
	init { arg path, serv; 
		filename = path; server = serv;
			this.readHeader;
			this.read }
	
	
	loadToBuffer {
		var c, inc, numpartials, numframes, numcycles, size, bandarray, tmp;
		c = Condition.new;
		Routine.run{

			// since we are loading so much data, we need to sync the server

			server.sync(c);

			inc = 4; // the first 4 slots will be data from the header, so, adjust the
				// the incrementer here
//			size = 2**((this.data.size*0.125).log2.ceil);
			// if there was no phase data, compensate for that
			size = ((filetype == 1) || (filetype == 3)).if({
				2**(((this.data.size + freqs.size)*0.125).log2.ceil);
				}, {
				2**(((this.data.size + freqs.size)*0.125).log2.ceil);
				});
			signal = Signal.newClear(size);
			signal[0] = this.header[9]; //ats file type
			signal[1] = this.header[4]; //numpartials
			signal[2] = this.header[5]; //numframes
			signal[3] = this.header[3]; //window size in samples
	
			amps.do{arg me, i;
				me.do{arg thisval;
					signal[inc]= thisval;
					inc = inc + 1;
					}};
			freqs.do{arg me, i;
				me.do{arg thisval, j;
					signal[inc]= thisval;
					inc = inc + 1;
					}};	
			
			phases.do{arg me, i;
				me.do{arg thisval, j;
					signal[inc]= thisval;
					inc = inc + 1;
					}};
			
			//fill the bandNoi info in	
				
			bandNoi.do{arg me, i;									me.do{arg thisval, j;
					signal[inc]= thisval;
					inc = inc + 1;
					}};

			// this will cause the server to wait until the buffer is allocated
			// before anything is loaded into it
			
			tmp = Buffer.loadCollection(this.server, signal);
			buffer = tmp.bufnum;
	
		("ATS data loaded to buffer " ++ buffer.asString).postln
		}
	}


	freeBuffer {
		this.server.sendMsg(\b_free, this.buffer);
		("Buffer " ++ buffer.asString ++ " freed").postln;
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
	
	addToScore {arg newfile, file;
		var c, f, inc, numpartials, numframes, numcycles, signal, size, scorelist, newlist;

			scorelist = file.notNil.if({Score.newFromFile(file)}, {Score.new});
			inc = 3; // the first 3 slots will be data from the header, so, adust the
				// the incrementer here
			size = ((filetype == 1) || (filetype == 3)).if({
				2**(((this.data.size + freqs.size) * 0.125).log2.ceil);
				}, {
				2**(((this.data.size + freqs.size) * 0.125).log2.ceil)
				});
			numcycles = (size/512).asInteger;
			signal = Signal.newClear(size);
			signal[0] = this.header[9]; //ats file type
			signal[1] = this.header[4]; //numpartials
			signal[2] = this.header[5]; //numframes
	
			this.amps.size.do{arg i;
				this.amps[i].size.do{arg j;
					signal[inc]= this.amps[i][j];
					inc = inc + 1;
					}};
			this.freqs.size.do{arg i;
				this.freqs[i].size.do{arg j;
					signal[inc]= this.freqs[i][j];
					inc = inc + 1;
					}};	
			this.phases.size.do{arg i;
				this.phases[i].size.do{arg j;
					signal[inc]= this.phases[i][j];
					inc = inc + 1;
					}};

			numcycles.do{arg i;
				scorelist.score_(scorelist.score.add([0.0, [\b_setn, buffer, 512*i, 512] ++
						signal[(512*i)..((512*i)+511)]]));
				};
			scorelist.score_(scorelist.score.add([0.0, [\b_alloc, buffer, size]]));
			scorelist.sort;
			f = File.new(newfile, "w");
			f.putString((scorelist.score).asCompileString);
			f.close;
			
		("ATS data loaded to file " ++ newfile.asString).postln
	}
		
	readHeader {
	
		var magic, len, arr;
		
		this.pos = 0;		
		//header = Array.fill(10, { this.getDouble });

		
		(this.getDouble == 123.0).if(
			{bige = true; this.pos = 0;
				header = Array.fill(10, { this.getDouble })},
			{this.pos_(0); (this.getDoubleLE == 123.0).if(
				{bige = nil; this.pos = 0;
					header = Array.fill(10, {this.getDoubleLE})},
				{"This is not an ATSFile!".error; header = nil})});
				
//		header.postln;
		
		magic = header[0] == 123.0;
		
		magic.if({ 
		
		critBands = [ 0, 100, 200, 300, 400, 510, 630, 770, 920, 1080, 1270, 1480, 1720,
					2000, 2320, 2700, 3150, 3700, 4400, 5300, 6400, 7700, 9500, 12000,
					15500, 20000 ];
		numFrames = header[5];
		numPartials = header[4];
		sndDur = header[8];
		filetype = header[9];
		parNoi = Array.fill(numPartials, { Array.fill(numFrames, { 0.0 }) });
		bandNoi = Array.fill(25, { Array.fill(numFrames, { 0.0 }) });  
		}, { "This is not an ATSFile!".error; header=nil});
		
		^magic
		
	}
	
	readData {
		var arr, len;
		
		len = this.length - 80;
		this.pos_(80);
		arr = bige.notNil.if(
			{Array.fill(len, { this.getDouble })}, 
			{Array.fill(len, { this.getDoubleLE})});
		data = arr;
		
	}
	
	read {
	
		this.readHeader.if({
		this.readData;
		this.sortData;
		"File read and sorted!".inform })
	
	}
	
	sortData {
		case
			{filetype == 1} {
				freqs = Array.fill(numPartials, { arg i;
					this.getParFreq(i) });
		
				amps = Array.fill(numPartials, { arg i;
					this.getParAmp(i) });
				phases = Array.fill(numPartials, { arg i;
						0.0 });	
						
				}
			{filetype == 2} {
				freqs = Array.fill(numPartials, { arg i;
						this.getParFreq(i) });
				
				amps = Array.fill(numPartials, { arg i;
						this.getParAmp(i) });
						
				phases = Array.fill(numPartials, { arg i;
						this.getParPhase(i) });	
				
				}
			{filetype == 3} {
				freqs = Array.fill(numPartials, { arg i;
					this.getParFreq(i) });
		
				amps = Array.fill(numPartials, { arg i;
					this.getParAmp(i) });
					
				phases = Array.fill(numPartials, { arg i;
						0.0 });	
					
				bandNoi = Array.fill(25, {arg i;
						this.getBandNoi(i)});
				}
			{filetype == 4} {
				freqs = Array.fill(numPartials, { arg i;
					this.getParFreq(i) });
		
				amps = Array.fill(numPartials, { arg i;
					this.getParAmp(i) });
					
				phases = Array.fill(numPartials, { arg i;
						this.getParPhase(i) });			
				bandNoi = Array.fill(25, {arg i;
						this.getBandNoi(i)});
				}
				

		
	}
		
	info {
			header.notNil.if({
			
			("a ATSFile made from \"" ++ this.filename ++ "\"").inform;
			("Frames in analysis file:" + this.numFrames).inform;
			("Partials in analysis file:" + this.numPartials).inform;
			("Duration of original sound:" + this.sndDur).inform;
			this.parTables.notNil.if( { ("Number of partial tables created:" 
				+ parTables.size).inform },
				{ "Partial Tables have not been created".inform });
			("Total Memory used is about" + this.size.round(0.1) + "megabytes").inform
			}, { "This is not a ATSFile!".error })					
	}
	
	size { 	var tablesSize, filesize, outsize, datasize;
			
			^header.isNil.if( { "This file has not been read yet!".error; nil }, {  
			datasize = data.notNil.if( { data.size }, { 0 });
			filesize = (10 + datasize)*8;
			
			tablesSize = (parTables.size!=0).if({parTables.size * 2 * 
				parTables.at(0).at(0).size * 8 },{ 0 });
										
			outsize = filesize + tablesSize;
			
			(outsize/1000000) })
	
	}
	
	getParTime {	arg par;
				var increment, arr;
				
				arr = Array.with( 0, 	numPartials*2 + 1,  
								numPartials*3 + 1, 
								numPartials*2 + 26,
								numPartials*3 + 26 );
				
				increment = arr.at(filetype.asInteger);
				
				^Array.fill(numFrames, 
					{ arg i; data.at( ( i*increment ).asInteger ) })
	
	}
	 
	getParFreq {	arg par;
				var increment, arr, offset, arrayone;
				
				arr = Array.with( 0, 	numPartials*2 + 1,  
								numPartials*3 + 1, 
								numPartials*2 + 26,
								numPartials*3 + 26 );
				
				increment = arr.at(filetype.asInteger);
				offset = [ 0, 2, 3, 2, 3 ].at(filetype.asInteger);
				
				arrayone = Array.fill(numFrames, 
					{ arg i; data.at( ( i*increment+2+(par*offset) ).asInteger ) });
				numFrames.do
					{ arg i; (arrayone[i] == 0).if({arrayone[i] = arrayone[i-1]})};
				^arrayone;
	
	}
	
	getParAmp {	arg par;
				var increment, arr, offset;
				
				arr = Array.with( 0, 	numPartials*2 + 1,  
								numPartials*3 + 1, 
								numPartials*2 + 26,
								numPartials*3 + 26 );
				
				increment = arr.at(filetype.asInteger);
				offset = [ 0, 2, 3, 2, 3 ].at(filetype.asInteger);
				
				^Array.fill(numFrames, 
					{ arg i; data.at( ( i*increment+1+(par*offset) ).asInteger ) })
	
	}
	
	getParPhase {	arg par;
				var increment, arr;
				
				arr = Array.with( 0, numPartials*2 + 1,  
								numPartials*3 + 1, 
								numPartials*2 + 26,
								numPartials*3 + 26 );
				
				increment = arr.at(filetype.asInteger);
				
				^Array.fill(numFrames, 
					{ arg i; //0.0; // not using phase for now... fill with 0.0's 
						data.at( ( i*increment+3+(par*3) ).asInteger ) 
					})
	
	}
	
	getFrameFreq {	arg frame;
				var arr, increment, offset;
				
				arr = Array.with( 0, 	numPartials*2 + 1,  
								numPartials*3 + 1, 
								numPartials*2 + 26,
								numPartials*3 + 26 );
				
				increment = arr.at(filetype.asInteger);
				
				offset = [ 0, 2, 3, 2, 3 ].at(filetype.asInteger);
				
				^Array.fill(numPartials, 
					{ arg i; data.at( ( (frame*increment) + (i*offset) + 2 ).asInteger ) })
	
	}
	
	getFrameAmp {	arg frame;
				var arr, increment, offset;
				
				arr = Array.with( 0, 	numPartials*2 + 1,  
								numPartials*3 + 1, 
								numPartials*2 + 26,
								numPartials*3 + 26 );
				
				increment = arr.at(filetype.asInteger);
				
				offset = [ 0, 2, 3, 2, 3 ].at(filetype.asInteger);
				
				^Array.fill(numPartials, 
					{ arg i; data.at( ( (frame*increment) + (i*offset) + 1 ).asInteger ) })
	
	}
	
	transferNoi {	var arr, frame, noi, lo, hi;
	
				noi = Array.fill(numFrames, { arg i;
				
				frame = this.getFrameFreq(i);
				25.do({ arg j; 
					lo = critBands.at(j); 
					hi = critBands.at(j+1);
					arr = (frame.collect({ arg it, i; if((it >=lo)&&(it<=hi), i)})).reject(
						{ arg it; it.isNil });
					})
				
				})
	
	}
	
	getFrameNoi {	arg frame;
				var arr, increment, offset, off;
				
				arr = Array.with( 0, 	numPartials*2 + 1,  
								numPartials*3 + 1, 
								numPartials*2 + 26,
								numPartials*3 + 26 );
				
				increment = arr.at(filetype.asInteger);
				
				off = [ 0, 2, 3, 2, 3 ].at(filetype.asInteger);
				
				offset =  numPartials*off + 1;
				 
				^Array.fill(25, 
					{ arg i; data.at( ( (frame*increment) + (i*offset) + 1 ).asInteger ) })
	
	}
	
	getBandNoi {	arg band;
				var increment, arr, offset, off;
				
				arr = Array.with( 0, 	numPartials*2 + 1,  
								numPartials*3 + 1, 
								numPartials*2 + 26,
								numPartials*3 + 26 );
				
				increment = arr.at(filetype.asInteger);
				
				off = [ 0, 2, 3, 2, 3 ].at(filetype.asInteger);
				
				offset = numPartials*off + 1;
				
				^Array.fill(numFrames, 
					{ arg i; data.at( ( i*increment+offset+band ).asInteger ) })
	
	}
				
}