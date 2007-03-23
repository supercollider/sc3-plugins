// a wrapper for Score... takes SCEvents and calcs a pad time, sorts, etc.
SCScore {
	
	var <endtime = 0, score, buffers;
	
	*new {arg ... events;
		^super.new.init(events);
		}

	init {arg events;
		score = Score.new;
		buffers = Array.new;
		events.notNil.if({
			this.add(events);
			})

		}
				
	add {arg ... events;
		events.flat.do({arg event;
			case { // if the event is a note ...
				event.isKindOf(SCNote)
				} {
				event.bundle.do({arg me;
					score.add(me);
					});
				(event.endtime).notNil.if({
					(endtime < event.endtime).if({
						endtime = event.endtime
						})
					});
				} { // if the event is a buffer
				event.isKindOf(SCBuffer);
				} {
				buffers = buffers.add(event);
				}
			})
		}
		
	score {
		^score.score;
		}
		
	saveToFile {arg path;
		this.sortScore;
		score.saveToFile(path);
		}
	
	sortScore {
		score.add([endtime, [0]]);
		score.sort;
		}
		
	play {arg server, clock, quant = 0.0;
		server = server ? Server.default;
		server.boot;
		server.waitForBoot({
			this.loadBuffers(server, clock, quant);
			CmdPeriod.doOnce({
				buffers.do({arg me;
					me.free(nil);
					});
				"Buffers freed".postln;
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
				"Buffer loaded!".postln;
				});
			this.sortScore; // sort the score
			score.play(server, clock, quant); // finally... play the score!
			});
		}
		
	// SC2 it!
	write {arg path, duration, sampleRate = 44100, headerFormat = "AIFF", 
			sampleFormat = "int16", options;
		(buffers.size > 0).if({
			buffers.do({arg me;
				score.score.addFirst([0, me.bundle]);
				score.add([endtime, me.freeBundle]);
				(me.closeBundle.notNil).if({
					score.add([endtime, me.closeBundle]);
					})
				});
			"Buffer loaded!".postln;
			});		
		this.sortScore;
		score.recordNRT("trashme", path, sampleRate: sampleRate, headerFormat: headerFormat,
		 	options: options, duration: duration);
		}

	}

// creates a dictionary of Synthdefs, and SCNoteObjs
SCProtoNotes {
	var <synthdefs, <dict;
	*new {arg ... synthdefs;
		^super.newCopyArgs(synthdefs).init;
		}
		
	init {
		dict = Dictionary.new;
		this.addToDict(synthdefs);
		}
	
	addToDict {arg sds;
		sds.do({arg me;
			dict.add(me.name -> SCNoteObj.new(me));
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
	
		
SCNoteObj {
	var <synthdef, <synthdefname, args;
	*new {arg synthdef;
		^super.newCopyArgs(synthdef).init;
		}
		
	init {
		var sargs, sargsdefs;
		args = Dictionary.new;
		synthdefname = synthdef.name;
		synthdef.allControlNames.do({arg ctl;
			var def, name = ctl.name;
			def = ctl.defaultValue ?? {[]};
			args.add(name -> def);
			})
		}
	
	// create an SCNote instance
	new {arg starttime, duration, addAction = 0, target = 1, server;
		^SCNote.new(starttime, duration, addAction, target, server, synthdefname)
			.args_(args);
		}
		
	args {
		("Arguments and defaults for SynthDef "++synthdefname.asString++":").postln;
		args.keysValuesDo({arg key, val;
			("\t"++key++" defaults to "++val).postln;
			})
		}
	
}
// these objects are similar to the Node, Synth and Buffer objects, except they are used to 
// create Scores and don't directly send messages to the Server

SCNote  {
	var <starttime, <duration, <>addAction, <>target, server, <synthdefname,
		<endtime, <args, <>node, messages, <isPlaying = false, setnDict;
	
	*new {arg starttime, duration, addAction = 0, target = 1, server, synthdefname;
		server = server ? Server.default;
		^super.newCopyArgs(starttime, duration, addAction, target, server, synthdefname)
			.init;
		}

	init {
		node = server.nextNodeID;
		messages = Array.new;
		(starttime.notNil && duration.notNil).if({
			endtime = starttime + duration;
			});
		setnDict = Dictionary.new;
		}
	
	args_ {arg argdict;
		args = argdict;
		argdict.keysValuesDo({arg argname, val;
			this.addUniqueMethod(argname.asSymbol, {arg note; args.at(argname)});
			this.addUniqueMethod((argname.asString++"_").asSymbol, {arg note, newValue;
				args.put(argname.asSymbol, newValue);
				// real-time support
				isPlaying.if({
					(newValue.isArray || newValue.isKindOf(Env)).if({
						newValue.asArray;
						server.sendBundle(nil, [\n_setn, node, argname, newValue.size] ++
							newValue) 
						}, {
						server.sendBundle(nil, [\n_set, node, argname, newValue]);
						})
					});
				note;
				});
			});		
		}

	// every one of these has a tag and body... leaves room for addAction and 
	// target in SCEvent
	
	newBundle {
		var bundlearray;
		bundlearray =	this.buildBundle;
		^[starttime, bundlearray];	
		}
	
	buildBundle {
		var bundlearray;
		bundlearray =	[\s_new, synthdefname, node, addAction, target];
		args.keysValuesDo({arg key, val;
			((val.isArray || val.isKindOf(Env)).not).if({
				bundlearray = bundlearray ++ [key, val];
				}, {
				setnDict.add(key -> val.asArray);
				})
			});
		^bundlearray;		
		}
		
	bundle {
		messages = messages.add(this.newBundle);
		^messages;
		}
	
	// support playing and releaseing notes ... not for use with scores
	play {arg latency = nil;
		isPlaying.not.if({
			server.sendBundle(latency, this.buildBundle);
			setnDict.keysValuesDo({arg key, val;
				server.sendBundle(latency, [\n_setn, node, key, val.size] ++ val);
				});
			isPlaying = true;
			^this;
			}, {
			"This instance of SCNote is already playing".warn;
			})
		}
		
	checktime {arg time;
		(time > starttime).if({
			^true
			}, {
			"You can only set values after an event has occured".warn;
			^false;
			})		
		}
		
	set {arg time, key, value;
		var bund;
		bund = [\n_set, node, key, value];
		isPlaying.if({ // if playing... send the set message now!
			server.sendBundle(time, bund);
			}, {
			this.checktime(time).if({
				messages = messages.add([time, bund]);
				}, {
				"\"set\" failed".warn
				})
			})
		}
	
	setn {arg time, key ... values;
		var bund;
		values = values.flat;
		bund = [\n_setn, node, key, values.size] ++ values;
		isPlaying.if({
			server.sendBundle(time, bund);
			}, {
			this.checktime(time).if({
				messages = messages.add([time, bund])
				}, {
				"\"setn\" failed".warn
				})
			})		
		}
		
	release {arg time, key = \gate;
		this.set(time, key, 0);
		isPlaying = false;
		node = server.nextNodeID;
		}
	
	// immeditaely kill the node
	free {arg time;
		var bund;
		bund = [\n_free, node];
		isPlaying.if({
			server.sendBundle(time, bund);
			}, {
			this.checktime(time).if({
				messages = messages.add([time, bund]);
				}, {
				"\"free\" failed".warn
				})
			})
		}	
	}
	
SCBuffer {
	var <bufnum, <path, <size, <numChannels, <server, <bundle, <freeBundle, <closeBundle;
	var <duration;
	
	*new {arg path, size, numChannels, bufnum, server;
		^this.newCopyArgs(bufnum, path, size, numChannels, server).init;
		}
		
	init {
		var sf;
		server = server ? Server.default;
		bufnum = bufnum ? server.bufferAllocator.alloc(1);
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
			bundle = [\b_allocRead, bufnum, path];
			} {// path, size ( for DiskIn )
			path.notNil && size.notNil
			} {
			bundle = [\b_alloc, bufnum, size, numChannels, 
				[\b_read, bufnum, path, 0, size, 0, 1]];
			closeBundle = [\b_close, bufnum];
			} { /// just allocate memory (for delays, FFTs etc.)
			path.isNil && size.notNil
			} {
			bundle = [\b_alloc, bufnum, size, numChannels];
			};
		freeBundle = [\b_free, bufnum];			
		}
		
	load {arg time;
		server.sendBundle(nil, bundle);
		}
	
	free {arg time;
		closeBundle.notNil.if({
			server.sendBundle(time, closeBundle, freeBundle);
			}, {
			server.sendBundle(time, freeBundle)
			})
		}

	}
	

SCGroup {
	
	
	}
	
SCControl {

	}

/* this will be similar to ProcMod ... global envelope magic */
SCEvent {

	}

