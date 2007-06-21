// a wrapper for Score... takes CtkEvents and calcs a pad time, sorts, etc.
CtkScore {
	
	var <endtime = 0, score, <buffers, <buffermsg;
	
	*new {arg ... events;
		^super.new.init(events);
		}

	init {arg events;
		score = Score.new;
		buffers = Array.new;
		buffermsg = Array.new;
		events.notNil.if({
			this.add(events);
			})
		}
				
	add {arg ... events;
		events.flat.do({arg event;
			case { // if the event is a note ...
				(event.isKindOf(CtkNote) || event.isKindOf(CtkGroup))
				} {
				event.prBundle.do({arg me;
					score.add(me);
					});
				(event.endtime).notNil.if({
					(endtime < event.endtime).if({
						endtime = event.endtime
						})
					});
				} { // if the event is a buffer
				event.isKindOf(CtkBuffer);
				} {
				buffers = buffers.add(event);
				(event.messages.size > 0).if({
					event.messages.do({arg thismsg;
						var msgtime;
						msgtime = thismsg[0];
						buffermsg = buffermsg.add(thismsg);
						(msgtime > endtime).if({
							endtime = msgtime
							})
						})
					});
				} {
				true;
				} {
				event.messages.do({arg me;
					score.add(me);
					});
				(event.endtime > endtime).if({
					endtime = event.endtime
					})
				}
					
			})
		}
		
	score {
		^score.score;
		}
		
	saveToFile {arg path;
		this.addBuffers;
		this.sortScore;
		this.mergeTimes;
		score.saveToFile(path);
		}
	
	sortScore {
		score.add([endtime, [0]]);
		score.sort({arg a, b; a[0] <= b[0]});
		}
	
	addBuffers {
		buffers.do({arg me;
			score.add([0.0, me.bundle]);
			score.add([endtime, me.freeBundle]);
			(me.closeBundle.notNil).if({
				score.add([endtime, me.closeBundle]);
				});
			});
		this.addBufferMsg;
		}
		
	addBufferMsg {
		buffermsg.do({arg me;
			score.add(me);
			});			
		}
	
	mergeTimes {
		var idx, ascore;
		idx = 0;
		ascore = score.score;
		while({
//			(ascore[idx][0].round(0.00001) == ascore[idx+1][0].round(0.00001)).if({
			(ascore[idx][0].fuzzyEqual(ascore[idx+1][0], 0.00001) > 0).if({				ascore[idx+1].removeAt(0);
				ascore[idx+1].do({arg me;
					ascore[idx] = ascore[idx].add(me);
					});
				ascore.removeAt(idx+1);
				}, {
				idx = idx + 1;
				});
			idx < (ascore.size-1);
			});		
		}
		
	play {arg server, clock, quant = 0.0;
		server = server ?? {Server.default};
		server.boot;
		server.waitForBoot({
			this.loadBuffers(server, clock, quant);
			CmdPeriod.doOnce({
				(buffers.size > 0).if({
					buffers.do({arg me;
						me.free(nil);
						});
					"Buffers freed".postln;
					})
				})
			})
		}
	
	loadBuffers {arg server, clock, quant;
		var cond;
		cond = Condition.new;
		Routine.run({
			(buffers.size > 0).if({
				server.sync(cond, 
					Array.fill(buffers.size, {arg i;
						buffers[i].bundle;
						})
					);
				buffers.do({arg me;
					score.add([endtime, me.freeBundle]);
					(me.closeBundle.notNil).if({
						score.add([endtime, me.closeBundle]);
						})
					});
				this.addBufferMsg;
				"Buffer loaded!".postln;
				});
			this.sortScore; // sort the score
			this.mergeTimes;
			score.play(server, clock, quant); // finally... play the score!
			});
		}
		
	// SC2 it!
	write {arg path, duration, sampleRate = 44100, headerFormat = "AIFF", 
			sampleFormat = "int16", options;
		var newscore;
		(buffers.size > 0).if({
			buffers.do({arg me;
				score.score.addFirst([0, me.bundle]);
				score.add([endtime, me.freeBundle]);
				(me.closeBundle.notNil).if({
					score.add([endtime, me.closeBundle]);
					})
				});
			this.addBufferMsg;
			"Buffer loaded!".postln;
			});		
		this.sortScore;
		this.mergeTimes;
		score.recordNRT("/tmp/trashme", path, sampleRate: sampleRate, 
			headerFormat: headerFormat,
		 	sampleFormat: sampleFormat, options: options, duration: duration);
		}
		
	// add a time to all times in a CtkScore
	offset {arg duration;
		score.score = this.prOffset(score.score, duration);
		}
	
	prOffset {arg array, duration;
		^array.deepCopy.do({arg me; me[0] = me[0] + duration});
		}
		
	merge {arg newScore, newScoreOffset = 0;
		var addScore;
		addScore = newScore.score.deepCopy;
		addScore = this.prOffset(addScore, newScoreOffset);
		score.score = score.score ++ addScore;
		}
	}

// creates a dictionary of Synthdefs, and CtkNoteObjects
CtkProtoNotes {
	var <synthdefs, <dict;
	*new {arg ... synthdefs;
		^super.newCopyArgs(synthdefs).init;
		}
		
	init {
		dict = Dictionary.new;
		this.addToDict(synthdefs);
		}
	
	// load and add to the dictionary
	addToDict {arg sds;
		sds.do({arg me;
			dict.add(me.name -> CtkNoteObject.new(me));
			});	
		}
	
	at {arg id;
		^dict[id.asString]
		}
		
	add {arg ... newsynthdefs;
		synthdefs = synthdefs ++ newsynthdefs;
		this.addToDict(newsynthdefs);
		}
}
	
		
CtkNoteObject {
	var <synthdef, <server, <synthdefname, args;
	*new {arg synthdef, server;
		^super.newCopyArgs(synthdef, server).init;
		}
		
	init {
		var sargs, sargsdefs;
		synthdef.load(server ?? {Server.default});
		args = Dictionary.new;
		synthdefname = synthdef.name;
		synthdef.allControlNames.do({arg ctl, i;
			var def, name = ctl.name;
			def = ctl.defaultValue ?? {
				(i == (synthdef.allControlNames.size - 1)).if({
					synthdef.controls[ctl.index..synthdef.controls.size-1];
					}, {
					synthdef.controls[ctl.index..synthdef.allControlNames[i+1].index-1];
					})
				};
			args.add(name -> def);
			})
		}
	
	// create an CtkNote instance
	new {arg starttime, duration, addAction = 0, target = 1, server;
		^CtkNote.new(starttime, duration, addAction, target, server, synthdefname)
			.args_(args.deepCopy);
		}
		
	args {
		("Arguments and defaults for SynthDef "++synthdefname.asString++":").postln;
		args.keysValuesDo({arg key, val;
			("\t"++key++" defaults to "++val).postln;
			});
		^args;
		}
	
}

CtkObj {
	
	addTo {arg aCtkScore;
		aCtkScore.add(this);
		^this;
		}
	}
	
CtkNode : CtkObj {
	classvar addActions, <nodes, <servers, <resps, cmd;

	var <addAction, <target, <server;
	var <node, <messages, <starttime, <>willFree = false;
	
	watch {
		var thisidx, idx;
		servers.includes(server).not.if({
			idx = servers.size;
			servers = servers.add(server); // add the server
			nodes = nodes.add([]); // add an array for these nodes to live in
			resps = resps.add(OSCresponderNode(server.addr, '/n_end', {arg time, resp, msg;
				nodes[idx].remove(msg[1])}).add); // add a responder to remove nodes
			cmd.if({cmd = false; CmdPeriod.doOnce({this.cmdPeriod})});
			});
		thisidx = servers.indexOf(server);
		nodes[thisidx] = nodes[thisidx].add(node);
		}
	
	isPlaying {
		var idx;
		(servers.size > 0).if({
			idx = servers.indexOf(server);
			nodes[idx].includes(node).if({^true}, {^false});
			}, {
			^false
			})
		}
		
	cmdPeriod {
		resps.do({arg me; me.remove});
		resps = [];
		servers = [];
		nodes = [];
		cmd = true;
		}
		
	set {arg time = 0.0, key, value;
		var bund;
		bund = [\n_set, node, key, value];
		this.isPlaying.if({ // if playing... send the set message now!
			SystemClock.sched(time, {
				server.sendBundle(nil, bund);
				});
			}, {
			messages = messages.add([starttime + time, bund]);
			})
		}
	
	setn {arg time = 0.0, key ... values;
		var bund;
		values = values.flat;
		bund = [\n_setn, node, key, values.size] ++ values;
		this.isPlaying.if({
			SystemClock.sched(time, {server.sendBundle(nil, bund)});
			}, {
			messages = messages.add([time+starttime, bund])
			})		
		}
	
	release {arg time = 0.0, key = \gate;
		this.set(time, key, 0);
		willFree = true;
		^this;
		}
	
	// immeditaely kill the node
	free {arg time = 0.0, addMsg = true;
		var bund;
		bund = [\n_free, node];
		willFree = true;
		this.isPlaying.if({
			SystemClock.sched(time, {server.sendBundle(nil, bund)});
			}, {
			addMsg.if({
				messages = messages.add([time+starttime, bund]);
				})
			})
		}
		
		
	*initClass {
		addActions = IdentityDictionary[
			\head -> 0,
			\tail -> 1,
			\before -> 2,
			\after -> 3,
			\replace -> 4,
			0 -> 0,
			1 -> 1,
			2 -> 2,
			3 -> 3,
			4 -> 4
			];
		nodes = [];
		servers = [];
		resps = [];
		cmd = true;
		}
	}	

// these objects are similar to the Node, Synth and Buffer objects, except they are used to 
// create Scores and don't directly send messages to the Server

CtkNote : CtkNode {

	var <duration, <synthdefname,
		<endtime, <args, <setnDict, <mapDict;
			
	*new {arg starttime, duration, addAction = 0, target = 1, server, synthdefname;
		server = server ?? {Server.default};
		^super.newCopyArgs(addAction, target, server).init(starttime, duration, synthdefname);
		}
		

	init {arg argstarttime, argduration, argsynthdefname;
		starttime = argstarttime;
		duration = argduration;
		synthdefname = argsynthdefname;
		node = server.nextNodeID;
		messages = Array.new;
		(starttime.notNil && duration.notNil).if({
			endtime = starttime + duration;
			});
		setnDict = Dictionary.new;
		mapDict = Dictionary.new;
		}
	
	args_ {arg argdict;
		args = argdict;
		argdict.keysValuesDo({arg argname, val;
			this.addUniqueMethod(argname.asSymbol, {arg note; args.at(argname)});
			this.addUniqueMethod((argname.asString++"_").asSymbol, {arg note, newValue;
				args.put(argname.asSymbol, newValue);
				this.isPlaying.if({
					this.handleRealTimeUpdate(argname, newValue);
					});
				note;
				});
			});		
		}
		
	handleRealTimeUpdate {arg argname, newValue;
		// real-time support
		case {
			(newValue.isArray || newValue.isKindOf(Env))
			}{
			newValue = newValue.asArray;
			server.sendBundle(nil, [\n_setn, node, argname, newValue.size] ++
				newValue) 
			}{
			newValue.isKindOf(CtkControl)
			}{
			server.sendBundle(nil, [\n_map, node, argname, newValue.bus])
			}{
			newValue.isKindOf(CtkBuffer)
			}{
			server.sendBundle(nil, [\n_set, node, argname, newValue.bufnum])
			}{
			newValue.isKindOf(CtkAudio)
			}{
			server.sendBundle(nil, [\n_set, node, argname, newValue.bus])
			}{
			true
			}{
			server.sendBundle(nil, [\n_set, node, argname, newValue]);
			};
		}

	// every one of these has a tag and body... leaves room for addAction and 
	// target in CtkEvent
	
	newBundle {
		var bundlearray, initbundle;
		bundlearray =	this.buildBundle;
		initbundle = [starttime, bundlearray];
		setnDict.keysValuesDo({arg key, val;
			initbundle = initbundle.add([\n_setn, node, key, val.size] ++ val);
			});
		mapDict.keysValuesDo({arg key, val;
			initbundle = initbundle.add([\n_map, node, key, val])			});
		^initbundle;	
		}
		
	buildBundle {
		var bundlearray;
		(target.isKindOf(CtkNote) || target.isKindOf(CtkGroup)).if({
			target = target.node});
		bundlearray =	[\s_new, synthdefname, node, addActions[addAction], target];
		args.keysValuesDo({arg key, val;
			case {
				(val.isArray || val.isKindOf(Env))
				}{
				setnDict.add(key -> val.asArray)
				}{
				val.isKindOf(CtkControl)
				}{
				mapDict.add(key -> val.bus);
				}{
				val.isKindOf(CtkBuffer)
				}{
				bundlearray = bundlearray ++ [key, val.bufnum];
				}{
				val.isKindOf(CtkAudio)
				}{
				bundlearray = bundlearray ++ [key, val.bus];
				}{
				true
				}{
				bundlearray = bundlearray ++ [key, val];
				}
			});
		^bundlearray;		
		}
		
	prBundle {
		messages = messages.add(this.newBundle);
		(duration.notNil && willFree.not).if({
			messages = messages.add([(starttime + duration).asFloat, [\n_free, node]]);
			});
		^messages;
		}

	bundle {
		var thesemsgs;
		thesemsgs = [];
		thesemsgs = thesemsgs.add(this.newBundle);
		(duration.notNil && willFree.not).if({
			thesemsgs = thesemsgs.add([(starttime + duration).asFloat, [\n_free, node]]);
			});
		^thesemsgs;
		}
			
	// support playing and releasing notes ... not for use with scores
	play {arg latency;
		var bund;
		this.isPlaying.not.if({
			bund = this.buildBundle;
			server.sendBundle(latency, this.buildBundle);
			setnDict.keysValuesDo({arg key, val;
				server.sendBundle(latency, [\n_setn, node, key, val.size] ++ val);
				});
			mapDict.keysValuesDo({arg key, val;
				server.sendBundle(latency, [\n_map, node, key, val]);
				});
			this.watch;
			// if a duration is given... kill it
			duration.notNil.if({
				SystemClock.sched(duration, {this.free(addMsg: false)})
				});
			^this;
			}, {
			"This instance of CtkNote is already playing".warn;
			})
		}
		
	}

/* methods common to CtkGroup and CtkNote need to be put into their own class (CtkNode???) */
CtkGroup : CtkNode {
	var <endtime = nil, <duration, <isGroupPlaying = false;
	
	*new {arg starttime = 0.0, duration, node, addAction = 0, target = 1, server;
		^super.newCopyArgs(addAction, target, server).init(starttime, duration);
		}
		
	*play {arg starttime = 0.0, duration, node, addAction = 0, target = 1, server;
		^this.new(starttime, duration, node, addAction, target, server).play;
		}
		
	init {arg argstarttime, argduration;
		starttime = argstarttime;
		duration = argduration;
		duration.notNil.if({
			endtime = starttime + duration
			});
		(target.isKindOf(CtkNote) || target.isKindOf(CtkGroup)).if({
			target = target.node
			});
		server = server ?? {Server.default};
		node = node ?? {server.nextNodeID};
		messages = Array.new;
		}
		
	newBundle {
		var start, bundlearray;
		bundlearray =	this.buildBundle;
		start = starttime ?? {0.0}
		^[starttime, bundlearray];	
		}
	
	buildBundle {
		var bundlearray;
		bundlearray =	[\g_new, node, addActions[addAction], target];
		^bundlearray;		
		}
		
	prBundle {
		messages = messages.add(this.newBundle);
		(duration.notNil).if({
			messages = messages.add([(starttime + duration).asFloat, 
				[\g_freeAll, node], [\n_free, node]]);
			});
		^messages;
		}

	bundle {
		var thesemsgs;
		thesemsgs = [];
		thesemsgs = thesemsgs.add(this.newBundle);
		(duration.notNil && willFree.not).if({
			thesemsgs = thesemsgs.add([(starttime + duration).asFloat, [\n_free, node]]);
			});
		^thesemsgs;
		}
		
	// create the group for RT uses
	play {arg time;
		var bundle = this.buildBundle;
		time.notNil.if({
			SystemClock.sched(time, {server.sendBundle(nil, bundle)});
			}, {
			server.sendBundle(nil, bundle);
			});
		duration.notNil.if({
			SystemClock.sched(duration, {this.freeAll})
			});
		this.watch;
		isGroupPlaying = true;
		^this;
		}
		
	freeAll {arg time = 0.0;
		var bund1, bund2;
		bund1 = [\g_freeAll, node];
		bund2 = [\n_free, node];
		isGroupPlaying.if({
			SystemClock.sched(time, {server.sendBundle(nil, bund1, bund2)});
			isGroupPlaying = false;
			}, {
			messages = messages.add([starttime + time, bund1, bund2]);
			})
		}
	
	deepFree {arg time = 0.0;
		this.freeAll(time);
		}
		
	}

// if a CtkBuffer is loaded to a server, its 'isPlaying' instance var will be set to true, and 
// the CtkBuffer will be considered live.

CtkBuffer : CtkObj {
	var <bufnum, <path, <size, <startFrame, <numFrames, <numChannels, <server, <bundle, 
		<freeBundle, <closeBundle, <messages, <isPlaying = false, <isOpen = false;
	var <duration;
	
	*new {arg path, size, startFrame = 0, numFrames, numChannels, bufnum, server;
		^this.newCopyArgs(bufnum, path, size, startFrame, numFrames, numChannels, server).init;
		}
	
	*diskin {arg path, size = 32768, startFrame = 0, server;
		^this.new(path, size, startFrame, server: server)
		}
		
	*playbuf {arg path, startFrame = 0, numFrames = 0, server;
		^this.new(path, startFrame: startFrame, numFrames: numFrames, server: server)
		}
		
	*buffer {arg size, numChannels, server;
		^this.new(size: size, numChannels: numChannels, server: server)
		}

	init {
		var sf, nFrames;
		server = server ?? {Server.default};
		bufnum = bufnum ?? {server.bufferAllocator.alloc(1)};
		messages = [];
		path.notNil.if({
			sf = SoundFile.new(path);
			sf.openRead;
			numChannels = sf.numChannels;
			duration = sf.duration;
			sf.close;
			});
		case { // path, not size - load file with b_allocRead
			path.notNil && size.isNil
			} {
			nFrames = numFrames ?? {0};
			bundle = [\b_allocRead, bufnum, path, startFrame, nFrames];
			} {// path, size ( for DiskIn )
			path.notNil && size.notNil
			} {
			nFrames = numFrames ?? {size};
			bundle = [\b_alloc, bufnum, size, numChannels, 
				[\b_read, bufnum, path, startFrame, nFrames, 0, 1]];
			closeBundle = [\b_close, bufnum];
			} { /// just allocate memory (for delays, FFTs etc.)
			path.isNil && size.notNil
			} {
			numChannels = numChannels ?? {1};
			duration = size / server.sampleRate;
			bundle = [\b_alloc, bufnum, size, numChannels];
			};
		freeBundle = [\b_free, bufnum];			
		}
		
	load {arg time = 0.0, sync = true;
		SystemClock.sched(time, {
			var cond;
			Routine.run({
				var msg;
				server.sendBundle(nil, bundle);
				// are there already messages to send? If yes... SYNC!, then send NOW
				(messages.size > 0).if({
					server.sync(cond);
					messages.do({arg me; 
						msg = me[1].postln;
						server.sendBundle(nil, msg);
						server.sync(cond);
						});
					});
				sync.if({
					server.sync(cond);
					("CtkBuffer with bufnum id "++bufnum++" loaded").postln;
					});
				isPlaying = true;
				})
			});
		}
	
	free {arg time = 0.0;
		closeBundle.notNil.if({
			SystemClock.sched(time, {server.sendBundle(nil, closeBundle, freeBundle)});
			}, {
			SystemClock.sched(time, {server.sendBundle(nil, freeBundle)});
			});
		isPlaying = false;
		}

	set {arg time = 0.0, startPos, values;
		var bund;
		values = values.asArray;
		// check for some common problems
		((values.size + startPos) > size).if({
			"Number of values and startPos exceeds CtkBuffer size. No values were set".warn;
			^this;
			}, {
			bund = [\b_setn, bufnum, startPos, values.size] ++ values;
			([0.0, bund].bundleSize >= 8192).if({
				"Bundle size exceeds UDP limit. Use .loadCollection. No values were set".warn;
				^this;
				}, {
				this.bufferFunc(time, bund);
				^this;
				})
			})
		}
		
	zero {arg time = 0;
		var bund;
		bund = [\b_zero, bufnum];
		this.bufferFunc(time, bund);
		}
		
	fill {arg time = 0.0, newValue, start = 0, numSamples = 1;
		var bund;
		bund = [\b_fill, bufnum, start, numSamples, newValue];
		this.bufferFunc(time, bund);
		}
	
	// write a buffer out to a file. For DiskOut usage in real-time, use openWrite and closeWrite
	write {arg time = 0.0, path, headerFormat = 'aiff', sampleFormat='int16', 
			numberOfFrames = -1, startingFrame = 0;	
		var bund;
		bund = [\b_write, bufnum, path, headerFormat, sampleFormat, numberOfFrames, 
			startingFrame, 0];
		this.bufferFunc(time, bund);
		}
	
	// prepare a buffer for use with DiskOut
	openWrite {arg time = 0.0, path, headerFormat = 'aiff', sampleFormat='int16', 
			numberOfFrames = -1, startingFrame = 0;	
		var bund;
		isOpen = true;
		bund = [\b_write, bufnum, path, headerFormat, sampleFormat, numberOfFrames, 
			startingFrame, 1];
		this.bufferFunc(time, bund);
		}
		
	closeWrite {arg time = 0.0;
		var bund;
		isOpen = false;
		bund = [\b_close, bufnum];
		this.bufferFunc(time, bund);
		}
		
	gen {arg time = 0.0, cmd, normalize = 0, wavetable = 0, clear = 1 ... args;
		var bund, flag;
		flag = (normalize * 1) + (wavetable * 2) + (clear * 4);
		bund = ([\b_gen, bufnum, cmd, flag] ++ args).flat;
		this.bufferFunc(time, bund);
		}
		
	sine1 {arg time, normalize = 0, wavetable = 0, clear = 1 ... args;
		this.gen(time, \sine1, normalize, wavetable, clear, args);
		}

	sine2 {arg time, normalize = 0, wavetable = 0, clear = 1 ... args;
		this.gen(time, \sine2, normalize, wavetable, clear, args);
		}
		
	sine3 {arg time, normalize = 0, wavetable = 0, clear = 1 ... args;
		this.gen(time, \sine3, normalize, wavetable, clear, args);
		}
		
	cheby {arg time, normalize = 0, wavetable = 0, clear = 1 ... args;
		this.gen(time, \cheby, normalize, wavetable, clear, args);
		}
		
	fillWithEnv {arg time = 0.0, env, wavetable = 0.0;
		env = (wavetable > 0.0).if({
			env.asSignal(size * 0.5).asWavetable;
			}, {
			env.asSignal(size)
			});
		this.set(time = 0.0, 0, env);
		}

	// checks if this is a live, active buffer for real-time use, or being used to build a CtkScore
	bufferFunc {arg time, bund;
		isPlaying.if({
			SystemClock.sched(time, {server.sendBundle(nil, bund)})
			}, {
			messages = messages.add([time, bund])
			});
		}
	}
		
CtkControl : CtkObj {
	var server, <numChans, <bus, value, starttime, <messages, isPlaying = false, <endtime = 0.0;
	
	*new {arg numChans = 1, initVal = 0.0, starttime = 0.0, bus, server;
		^this.newCopyArgs(server, numChans, bus, initVal, starttime).init;
		}
	
	/* calling .play on an object tells the object it is being used in real-time
	and therefore will send messages to server */
	*play {arg numChans = 1, initVal = 0.0, bus, server;
		^this.new(numChans, initVal, 0.0, bus, server).play;
		}
	
	init {
		var bund;
		server = server ?? {Server.default};
		bus = bus ?? {server.controlBusAllocator.alloc(numChans)};
		messages = []; // an array to store sceduled bundles for this object
		bund = [\c_setn, bus, numChans, value];
		messages = messages.add([starttime, bund]);
		}
		
	play {
		isPlaying = true;
		server.sendBundle(nil, [\c_setn, bus, numChans, value]);
		}
		
	set {arg val, time = 0.0;
		var bund;
		bund = [\c_setn, bus, numChans, val];
		isPlaying.if({
			SystemClock.sched(time, {server.sendBundle(nil, bund)});
			}, {
			time = time ?? {0.0};
			messages = messages.add([starttime + time, bund]);
			});
		value = val;
		^this;
		}
	}

// not really needed... but it does most of the things that CtkControl does
CtkAudio : CtkObj {
	var server, <bus, <numChans;
	*new {arg bus, numChans = 1, server;
		^this.newCopyArgs(server, bus, numChans).init;
		}
	
	init {
		server = server ?? {Server.default};
		bus = bus ?? {server.audioBusAllocator.alloc(numChans)};
		}
	}
	
/* this will be similar to ProcMod ... global envelope magic */
CtkEvent : CtkObj {

	}

