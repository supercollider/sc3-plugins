Player {

	var <soundfilepath, <sf, <isPlaying = false, <formatOut= \straight, <bufnum, <server, <numchan,
		<>outbus, <isScope = false, <isGui = false;
	var initsoundfilepath, initformatOut, initstarttime, initoutbus, initamp, initxywz,
		initendtime;
	var <>group, <amp, <starttime, <player, <>nodeID, <scopeID, <scopebuf, <outarray, <curtime,
		<clock, <xywz, <endtime, <tempoclock;
	var <mute_w = 1, <mute_x = 1, <mute_y = 1, <mute_z = 1;
	var <window, texttask, timestart, timestop, timeformat, cond;
	var <env;
	var renderdef, deffunc, envsig, envbuf;

	// these are vars that the BFMix can use

	var mixstart;

	classvar formats, routing, outputChoices, synths, outvals, numouts;

	*new {arg soundfilepath, formatOut = \straight, starttime = 0, outbus = 0, amp = 1,
			xywz = false, env;
		var me;
		^super.new.init(soundfilepath, formatOut, starttime, outbus, amp, xywz, env);
		}

	init {arg argsoundfilepath, argFormatOut, argstarttime, argoutbus, argamp, argxywz, argenv;
//		tempoclock = TempoClock.new;
		(GUI.id == \cocoa).if({
			(argsoundfilepath.isNil).if({
				GUI.dialog.getPaths({arg paths;
					argsoundfilepath = paths[0];
					this.soundfile_(argsoundfilepath, argFormatOut, argstarttime, argoutbus,
						argamp, argxywz, argenv)}, {"cancel".postln})
				}, {
				this.soundfile_(argsoundfilepath.standardizePath, argFormatOut,
					argstarttime, argoutbus, argamp, argxywz, argenv)})
			}, {
			(argsoundfilepath.isNil).if({
				"You must supply the path to a soundfile".warn;
				}, {
				this.soundfile_(argsoundfilepath.standardizePath, argFormatOut,
					argstarttime, argoutbus, argamp, argxywz, argenv)
				});
			});
		cond = Condition.new;
	}

	// reset any variables to the values at init time
	reset {
		this.soundfile_(initsoundfilepath, initstarttime);
		formatOut = initformatOut;
		outbus = initoutbus;
		amp = initamp;
		}

	soundfile_ {arg argsoundfilepath, argFormatOut, argstarttime, argoutbus, argamp, argxywz,
			argenv;
		var allocfun, endcalc;
		"soundfile".postln;
		sf = SoundFile.new;
		soundfilepath = argsoundfilepath;
		sf.openRead(soundfilepath);
		sf.close;
		numchan = sf.numChannels;
		(argFormatOut != \straight).if({formatOut_(argFormatOut)});
		initsoundfilepath = soundfilepath;
		this.initstarttime_(argstarttime);
		initformatOut = formatOut;
		this.initendtime_(sf.duration);
		outbus = argoutbus;
		initoutbus = outbus;
		this.env_(argenv ?
			Env([0.0001, 1, 1, 0.0001], [0.05, (sf.duration - 0.1 + 0.05).round(0.1), 0.05],
				\exp));
		amp = argamp;
		initamp = amp;
		// allow xywz bf files to be decoded correctly!
		xywz = argxywz;
		allocfun = {bufnum = server.bufferAllocator.alloc(numchan)};
		// I don't like this... should be optional?
		server = (thisProcess.platform.name == \osx).if({
			Server.internal
			}, {
			Server.local
			});
		(server.serverRunning).if({
			allocfun
			},{
			server.boot;
			server.waitForBoot{allocfun};
			isGui.if({this.gui})});
		}

	formatOut_ {arg format;
		((numchan == 3) || (numchan == 4)).if({
			formatOut = format},{
			formatOut = \straight;
			"You can only change the format of a BFormat Signal".warn})
		}

	// in mixer... to skip into a file, use initstarttime
	initstarttime_ {arg newstart;
		initstarttime = newstart;
		this.starttime_(newstart);
		}

	starttime_ {arg newstart;
		var check, timetwo, checker;
		check = false;
		isPlaying.if({
			check = true;
			this.pause;
			starttime = newstart;
			this.play;
			}, {starttime = newstart;
				isGui.if({
					AppClock.sched(0.01,
						{window.view.children[4].string_(starttime.asTimeString);
						window.view.children[5].value_(starttime / sf.duration);
					})
				})
			});
		}

	// in mixer, use initendtime to truncate
	initendtime_ {arg newend;
		initendtime = newend;
		this.endtime_(newend);
		}

	endtime_ {arg newend;
			endtime = newend;
			isGui.if({
				window.view.children[8].hi_(endtime / sf.duration)});
		}

	env_ {arg newenv;
		var envstring;
		envstring = newenv;
		newenv.isKindOf(Env).if({
			newenv.releaseNode.isNil.if({
				this.endtime_(starttime + (newenv.times.sum));
				(initstarttime == 0.0).if({
					env = InterplEnv.new(newenv.levels, newenv.times, newenv.curves)
					}, {
					env = InterplEnv.new([0] ++ newenv.levels, [initstarttime] ++ newenv.times,
						[\lin] ++ newenv.curves)});
				isGui.if({window.view.children[14].value_(envstring.asCompileString)});
				}, {
				"Players expect fixed duration envelopes. Use and Env without\n
				a releaseNode or one of the InterplEnv classes".warn;
				});
			}, {
			newenv.isKindOf(InterplEnv).if({
				this.endtime_(starttime + (newenv.times.sum));
				env = newenv;
				isGui.if({window.view.children[14].value_(envstring.asCompileString)});
				}, {
				"Players expect fixed duration envelopes. Use and Env without\n
				a releaseNode or one of the InterplEnv classes".warn;
				});
	})
}

	close {
		server.sendMsg(\b_close, bufnum);
		server.sendMsg(\b_free, bufnum);
	}

	play {
		var dur;
		dur = endtime - starttime;
		(isPlaying).if({
			this.stop;
			this.playfunc(starttime, dur)
			},{
			this.playfunc(starttime, dur)
			})
	}

	pausedplay {arg waitdur;
		isPlaying = true;
		tempoclock.sched(waitdur, {
			isPlaying.if({this.play})})
	}

	amp_ {arg newamp;
		amp = newamp;
		isPlaying.if({server.sendMsg(\n_set, nodeID, \amp, newamp)});
	}

	playfunc {arg start, dur;
		tempoclock = TempoClock.new;
		Routine.run({
			var players, defenv;
			isPlaying = true;
			0.1.wait;
			bufnum = server.bufferAllocator.alloc(numchan);
			players = [
				// play straight out, as is
				{arg gate = 1, outbus = 0, amp = 1;
						var out, envpoint, envstartpoint;
						envstartpoint = start - initstarttime;
						envpoint = Line.ar(envstartpoint, envstartpoint + dur, dur);
						out = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum));
						if(numchan == 1, {out = [out, out]});
						Out.ar(outbus, out * IEnvGen.ar(env, envpoint, amp) *
							EnvGen.kr(Env.asr(0.05, 1, 0.05), gate, doneAction: 2))
						},
				{arg gate = 1, outbus = 0, amp = 1, mute_w = 1, mute_x = 1, mute_y = 1,
					mute_z = 1;
					var out, w, x, y, z, decode, envpoint, envstartpoint;
					envstartpoint = start - initstarttime;
					envpoint = Line.ar(envstartpoint, envstartpoint + dur, dur);
					xywz.if({
						(numchan == 3).if({
							#x, y, w = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum)) *
								[mute_x, mute_y, mute_w];
							z = K2A.ar(0.0)},{
							#x, y, w, z = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum))
								* [mute_x, mute_y, mute_w, mute_z];
							})},{
						(numchan == 3).if({
							#w, x, y = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum)) *
								[mute_w, mute_x, mute_y];
							z = K2A.ar(0.0)},{
							#w, x, y, z = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum))
								* [mute_w, mute_x, mute_y, mute_z]
							})});
					decode = B2Ster.ar(w, x, y);
					Out.ar(outbus, decode * IEnvGen.kr(env, envpoint, amp) *
							EnvGen.kr(Env.asr(0.05, 1, 0.05), gate, doneAction: 2))					},
				{arg gate = 1, outbus = 0, amp = 1, mute_w = 1, mute_x = 1, mute_y = 1,
					mute_z = 1;
					var out, w, x, y, z, decode, envpoint, envstartpoint;
					envstartpoint = start - initstarttime;
					envpoint = Line.ar(envstartpoint, envstartpoint + dur, dur);
					xywz.if({
						(numchan == 3).if({
							#x, y, w = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum)) *
								[mute_x, mute_y, mute_w];
							z = K2A.ar(0.0)},{
							#x, y, w, z = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum))
								* [mute_x, mute_y, mute_w, mute_z];
							})},{
						(numchan == 3).if({
							#w, x, y = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum)) *
								[mute_w, mute_x, mute_y];
							z = K2A.ar(0.0)},{
							#w, x, y, z = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum))
								* [mute_w, mute_x, mute_y, mute_z]
							})});
					decode = B2UHJ.ar(w, x, y);
					Out.ar(outbus, decode * IEnvGen.kr(env, envpoint, amp) *
							EnvGen.kr(Env.asr(0.05, 1, 0.05), gate, doneAction: 2))					},
				{arg gate = 1, outbus = 0, amp = 1, mute_w = 1, mute_x = 1, mute_y = 1,
					mute_z = 1;
					var out, w, x, y, z, decode, envpoint, envstartpoint;
					envstartpoint = start - initstarttime;
					envpoint = Line.ar(envstartpoint, envstartpoint + dur, dur);					xywz.if({
						(numchan == 3).if({
							#x, y, w = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum)) *
								[mute_x, mute_y, mute_w];
							z = K2A.ar(0.0)},{
							#x, y, w, z = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum))
								* [mute_x, mute_y, mute_w, mute_z];
							})},{
						(numchan == 3).if({
							#w, x, y = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum)) *
								[mute_w, mute_x, mute_y];
							z = K2A.ar(0.0)},{
							#w, x, y, z = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum))
								* [mute_w, mute_x, mute_y, mute_z]
							})});
					decode = BFDecode1.ar(w, x, y, z, [0.25pi, -0.25pi, -0.75pi, -1.25pi]);
					Out.ar(outbus, decode * IEnvGen.kr(env, envpoint, amp) *
							EnvGen.kr(Env.asr(0.05, 1, 0.05), gate, doneAction: 2))					},
				{arg gate = 1, outbus = 0, amp = 1, mute_w = 1, mute_x = 1, mute_y = 1,
					mute_z = 1;
					var out, w, x, y, z, decode, envpoint, envstartpoint;
					envstartpoint = start - initstarttime;
					envpoint = Line.ar(envstartpoint, envstartpoint + dur, dur);					xywz.if({
						(numchan == 3).if({
							#x, y, w = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum)) *
								[mute_x, mute_y, mute_w];
							z = K2A.ar(0.0)},{
							#x, y, w, z = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum))
								* [mute_x, mute_y, mute_w, mute_z];
							})},{
						(numchan == 3).if({
							#w, x, y = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum)) *
								[mute_w, mute_x, mute_y];
							z = K2A.ar(0.0)},{
							#w, x, y, z = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum))
								* [mute_w, mute_x, mute_y, mute_z]
							})});
					decode = BFDecode1.ar(w, x, y, z, [0.1666pi, -0.16666pi, -0.5pi,
						-0.83333pi, 0.83333pi, 0.5pi ]);
					Out.ar(outbus, decode * IEnvGen.kr(env, envpoint, amp) *
							EnvGen.kr(Env.asr(0.05, 1, 0.05), gate, doneAction: 2))					},
				{arg gate = 1, outbus = 0, amp = 1, mute_w = 1, mute_x = 1, mute_y = 1,
					mute_z = 1;
					var out, w, x, y, z, decode, envpoint, envstartpoint;
					envstartpoint = start - initstarttime;
					envpoint = Line.ar(envstartpoint, envstartpoint + dur, dur);					xywz.if({
						(numchan == 3).if({
							#x, y, w = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum)) *
								[mute_x, mute_y, mute_w];
							z = K2A.ar(0.0)},{
							#x, y, w, z = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum))
								* [mute_x, mute_y, mute_w, mute_z];
							})},{
						(numchan == 3).if({
							#w, x, y = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum)) *
								[mute_w, mute_x, mute_y];
							z = K2A.ar(0.0)},{
							#w, x, y, z = VDiskIn.ar(numchan, bufnum, BufRateScale.kr(bufnum))
								* [mute_w, mute_x, mute_y, mute_z]
							})});
					decode = BFDecode1.ar(w, x, y, z, [0.125pi, -0.125pi, -0.375pi, -0.625pi,
						-0.875pi, 0.875pi, 0.625pi, 0.375 ]);
					Out.ar(outbus, decode * IEnvGen.kr(env, envpoint, amp) *
							EnvGen.kr(Env.asr(0.05, 1, 0.05), gate, doneAction: 2))					}
			];
			tempoclock.sched(0.05, {curtime = 0;
				Routine.run({
					var synthdef, newenv;
					clock = Main.elapsedTime;
					synthdef = players[formats[formatOut]].asSynthDef.send(server);
					server.sync(cond);
					nodeID = server.nodeAllocator.alloc(1);
					server.sendBundle(nil,
						[\b_alloc, bufnum, 32768, numchan],
						[\b_read, bufnum, soundfilepath, start * sf.sampleRate, 32768, 0,
							1, [\s_new, synthdef.name, nodeID, 0, 0, \gate, 1, \amp, amp,
							\outbus, outbus, \mute_w, mute_w, \mute_x, mute_x,
							\mute_y, mute_y, \mute_z, mute_z]]
						);
					isGui.if({AppClock.sched(0.01, {
						timestart.value(starttime);
						window.view.children[0].value_(1)})
					})
			}, clock: tempoclock)
		});

		tempoclock.sched(dur, {this.stop; nil});
		})
	}

	pause {
		isPlaying.if({
			// release the node after 0.01 seconds.. see EnvGen help for an explanation
			server.sendMsg(\n_set, nodeID, \gate, 0);
			isPlaying = false;
			curtime = Main.elapsedTime - clock + starttime;
			(curtime >= endtime).if({curtime = starttime});
			this.starttime_(curtime);
			tempoclock.clear;
			this.close;
			// if the GUI is open, stop it
			AppClock.sched(0.01, {isGui.if({
				window.view.children[0].value_(0);
				timestop.value})});
			})
	}

	stop {
		isPlaying.if({server.sendMsg(\n_set, nodeID, \gate, 0)});
		// release the node after 0.01 seconds.. see EnvGen help for an explanation
		isPlaying = false;
		// reset start time in case pause has been used
		this.starttime_(initstarttime);
		tempoclock.stop;
		this.close;
		// if the GUI is open, stop it
		AppClock.sched(0.01, {isGui.if({
			//this.starttime_(initstarttime);
			this.endtime_(endtime);
			window.view.children[0].value_(0);
			timestop.value})});
	}

	/*
	// this should render the file with the given envelope and output specs
	// starttime will place the file after time 0.0 (for mixer or padding)
	render {arg fileout, starttime = 0.0;
		var fil, score, synthdefname, options;
		options = ServerOptions.new. ...
		// build the SynthDef with a PlayBuf
		synthdefname = this.renderdef;
		score = Score.new([
			[0.0, [\b_allocRead...]]
			[starttime, [\s_new, renderdef.name, -1, 0 , 0 ... ]]
			[calcendtime [0]]
			]);
		// Write the file out... is there a Path?
		fileout.isNil.if({
			CocoaDialog.savePanel(
				{arg path;
					score.recordNRT("/tmp/trashme", path, options: options);
				}, {"cancelled".postln})}, {
				score.recordNRT("/tmp/trashme", path, options: options)});
		}

	// builds the SynthDef needed to render the file
	renderDef {
		renderdef = {}.asSynthDef.load(server);
		}

	*/

	// these are some of the guts... and the SynthDefs
	*initClass {
		formats = IdentityDictionary[
			\straight -> 0,
			\stereo -> 1,
			\uhj ->2,
			\quad -> 3,
			\hex -> 4,
			\eight -> 5
			];
		numouts = IdentityDictionary[
			\straight -> 0,
			\stereo -> 2,
			\uhj ->2,
			\quad -> 4,
			\hex -> 6,
			\eight -> 8
			];
		outvals = IdentityDictionary[
			14 -> 0,
			0 -> 1,
			4 -> 2,
			2 -> 3,
			];
		}

	filename {
		var len, delim='/', ext='.', ind1, ind2, sym, arr, ind, path;
		path = soundfilepath;
		len = path.size;
		arr = Array.fill(len, {arg i; path[i].asSymbol });
		ind1 = len - arr.reverse.indexOf(delim);
		ind = arr.reverse.indexOf(ext);
		ind2 = ind.notNil.if({ len - ind - 2}, { len - 1});
		^sym = path.copyRange( ind1, ind2 );
		}

////////////////////////////////Below is only GUI stuff///////////////////////////

	gui {
		var guiplay, guiplayfunc, guiformat, guioutbus, guiformats, scope, scopefun,
		changestart, changeend, sliderfun, guibfformats, numout;

		(soundfilepath.notNil).if({
			isGui = true;

			guiplayfunc = [{this.pause},{this.play}];
			guioutbus = [{outbus = 14}, {outbus = 0}, {outbus = 8}, {outbus = 2}];
			guiformats = [\straight, \stereo, \uhj, \quad, \hex, \eight];
			guibfformats = [{xywz = false}, {xywz = true}];
			scopefun = [
				{isScope = false; scope.window.close; window.front},
				{isScope = true;
				(formatOut == \straight).if({numout = numchan}, {numout = numouts[formatOut]});
					scope = server.scope(numout, outbus);
					scope.size_(400);
					scope.window.view.children[0].waveColors_(
						Array.fill(numout, {rgb(0, 255, 0)}));
					scope.window.userCanClose_(false);
					window.front
					}
				];

			timestart = {arg startvar;
				var epoch, start, minsdisp,time, secs;
				texttask = Task({
					epoch = Main.elapsedTime;
					start = starttime.value;
					loop{
						time = start + (Main.elapsedTime - epoch) ;
						window.view.children[4].string_(time.asTimeString );
						window.view.children[5].value_( time / sf.duration);
						0.1.wait;
					}
				}, AppClock);
				texttask.play
			};

			timestop = {
				var time, secs;
				texttask.stop;
				time = starttime;
				window.view.children[4].string_(time.asTimeString);
				window.view.children[5].value_(starttime / sf.duration)};

			window = GUI.window.new(this.filename.asString, Rect(200, 662, 520, 310));

			// window.view.children[0]

			GUI.button.new(window, Rect(10, 10, 100, 20))
				.states_([
					["play (space)", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
					["pause (space)", Color.black, Color(0.9, 0.5, 0.5, 0.5)]])
				.action_({arg me;
					soundfilepath.notNil.if({guiplayfunc[me.value].value},
						{"WARNING! No soundfile is loaded!".postln;
						me.value_(0)})});
			// 1
			GUI.button.new(window, Rect(120, 10, 150, 20))
				.states_([
					["828mkII Optical Out", Color.black, Color(0.9, 0.5, 0.9, 0.5)],
					["Analog Out", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
					["Optical Out", Color.black, Color(0.5, 0.5, 0.9, 0.5)],
					["828mkII Analog Out", Color.black, Color(0.9, 0.5, 0.5, 0.5)],
					])
				.action_({arg me;
					isPlaying.if({this.pause; guioutbus[me.value].value; this.play},
						{guioutbus[me.value].value});
					isScope.if({scope.index_(outbus)});
					window.view.children[0].focus(true)})
				.value_(outvals[outbus] ?? {1});
			// 2
			GUI.button.new(window, Rect(120, 35, 100, 18))
				.states_([
					["Straight Out", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
					["Stereo", Color.black, Color(0.9, 0.5, 0.5, 0.5)],
					["UHJ", Color.black, Color(0.5, 0.5, 0.9, 0.5)],
					["Quad", Color.black, Color(0.5, 0.5, 0.5, 0.9)],
					["Hexagon", Color.black, Color(0.7, 0.5, 0.7, 0.9)],
					["Eight", Color.black, Color.white],					])
				.action_({arg me;
					var thisnum;
					window.view.children[0].focus(true);
					((numchan == 3) || (numchan == 4)).if({
						isPlaying.if({
							this.pause; this.formatOut_(guiformats[me.value]); this.play},
							{this.formatOut_(guiformats[me.value])});
						isScope.if({
							(formatOut == \straight).if({thisnum = numchan},
								{thisnum = numouts[formatOut]});
							scope.numChannels_(thisnum);
						scope.window.view.children[0].waveColors_(
							Array.fill(thisnum, {rgb(0, 255, 0)}));
							})},{
						"You can only change the format of a BFormat Signal".warn;
						me.value_(0);
						})});

			window.view.children[2].value_(formats[formatOut]);

			// 3 //

			GUI.button.new(window, Rect(10, 60, 100, 18))
				.states_([
					["scope on", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
					["scope off", Color.black, Color(0.9, 0.5, 0.5, 0.5)]
					])
				.action_({arg me;
					scopefun[me.value].value;
					window.front;
					window.view.children[0].focus(true);
					});

			// The clock // 4 //
			GUI.staticText.new(window, Rect(390,4,120,30))
				.string_(starttime.asTimeString);
			(GUI.id == \cocoa).if({
				window.view.children[4].font = GUI.font.new(\Helvetica, 24);
				}, {
				window.view.children[4].font = GUI.font.new(\Arial, 20);
				});
			// The Range Slider and Slider for time control // 5 //

			GUI.slider.new(window, Rect(10, 100, 500, 10))
				.value_(starttime / sf.duration)
				.action_({arg me;
					window.view.children[0].focus(true);
					isPlaying.if({
						Routine.run({
							this.pause;
							this.starttime_(me.value * sf.duration);
							0.01.wait;
							this.play}) }, {this.starttime_(me.value * sf.duration)})
						})
				.beginDragAction_({
					Routine.run({0.1.wait;})});

			// 6 //

			GUI.button.new(window, Rect(10, 35, 100, 18))
				.states_([
					["stop", Color.black, Color(0.5, 0.9, 0.5, 0.5)]
					])
				.action_({this.stop;window.view.children[0].focus(true)});

			// 7 //

			GUI.button.new(window, Rect(120, 60, 100, 18))
				.states_([
					["wxyz", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
					["xywz", Color.black, Color(0.9, 0.5, 0.5, 0.5)]
					])
				.action_({arg me; isPlaying.if(
					{this.pause; guibfformats[me.value].value; this.play},
					{guibfformats[me.value].value});
					window.view.children[0].focus(true)});

			// 8 //

			GUI.rangeSlider.new(window, Rect(10, 85, 500, 10))
				.lo_(starttime * sf.duration)
				.hi_(endtime * sf.duration)
				.action_({arg me;
					this.initstarttime_(me.lo * sf.duration);
					this.initendtime_(me.hi * sf.duration);
					window.view.children[0].focus(true)});

			// 9 //

			GUI.slider.new(window, Rect(250, 60, 200, 18))
				.value_(amp * 0.5)
				.action_({arg me;
					this.amp_(me.value * 2);
					window.view.children[window.view.children.indexOf(me)+1]
						.value_((amp.ampdb.round(0.01)));
					window.view.children[0].focus(true)
					});

			// 10 //

			GUI.numberBox.new(window, Rect(470, 58, 40, 20))
				.value_(amp.ampdb)
				.action_({arg me; this.amp_(me.value.dbamp);
					window.view.children[window.view.children.indexOf(me)-1]
						.value_(amp * 0.5);
					window.view.children[0].focus(true)});

			// 11 //

			GUI.staticText.new(window, Rect(250, 40, 80, 20))
				.string_("Amp (in dB)");


			// 13 //

			GUI.staticText.new(window, Rect(10, 230, 30, 20))
				.string_("Env");

			// 14 //

			GUI.textField.new(window, Rect(40, 230, 400, 20))
				.value_(env.asCompileString)
				.action_({arg me;
					window.view.children[0].focus(true);
					this.env_(me.value.interpret);
				});


			// 15 //

			GUI.button.new(window, Rect(40, 252, 60, 18))
				.states_([
					["Plot Env", Color.black, Color(0.9, 0.5, 0.5, 0.5)]])
				.action_({env.plot;
					window.view.children[0].focus(true)});
			(numchan == 3).if({
				GUI.staticText.new(window, Rect(110, 272, 50, 20))
					.string_("W");
				GUI.button.new(window, Rect(110, 252, 50, 20))
					.states_([
						["play", Color.black, Color(0.9, 0.5, 0.5, 0.5)],
						["mute", Color.black, Color(0.5, 0.9, 0.5, 0.5)]
						])
					.action_({arg me;
						mute_w = me.value;
						window.view.children[0].focus(true);
						isPlaying.if({
							server.sendMsg(\n_set, nodeID, \mute_w, mute_w)})})					.value_(mute_w);
				GUI.staticText.new(window, Rect(170, 272, 50, 20))
					.string_("X");
				GUI.button.new(window, Rect(170, 252, 50, 20))
					.states_([
						["play", Color.black, Color(0.9, 0.5, 0.5, 0.5)],
						["mute", Color.black, Color(0.5, 0.9, 0.5, 0.5)]
						])
					.action_({arg me;
						mute_x = me.value;
						window.view.children[0].focus(true);
						isPlaying.if(
							{server.sendMsg(\n_set, nodeID, \mute_x, mute_x)})})
					.value_(mute_x);
				GUI.staticText.new(window, Rect(230, 272, 50, 20))
					.string_("Y");
				GUI.button.new(window, Rect(230, 252, 50, 20))
					.states_([
						["play", Color.black, Color(0.9, 0.5, 0.5, 0.5)],
						["mute", Color.black, Color(0.5, 0.9, 0.5, 0.5)]
						])
					.action_({arg me;
						mute_y = me.value;
						window.view.children[0].focus(true);
						isPlaying.if(
							{server.sendMsg(\n_set, nodeID, \mute_y, mute_y)})})
					.value_(mute_y);
				});

			(numchan == 4).if({
				GUI.staticText.new(window, Rect(110, 272, 50, 20))
					.string_("W");
				GUI.button.new(window, Rect(110, 252, 50, 20))
					.states_([
						["play", Color.black, Color(0.9, 0.5, 0.5, 0.5)],
						["mute", Color.black, Color(0.5, 0.9, 0.5, 0.5)]
						])
					.action_({arg me;
						mute_w = me.value;
						window.view.children[0].focus(true);
						isPlaying.if({
							server.sendMsg(\n_set, nodeID, \mute_w, mute_w)})})					.value_(mute_w);
				GUI.staticText.new(window, Rect(170, 272, 50, 20))
					.string_("X");
				GUI.button.new(window, Rect(170, 252, 50, 20))
					.states_([
						["play", Color.black, Color(0.9, 0.5, 0.5, 0.5)],
						["mute", Color.black, Color(0.5, 0.9, 0.5, 0.5)]
						])
					.action_({arg me;
						mute_x = me.value;
						window.view.children[0].focus(true);
						isPlaying.if(
							{server.sendMsg(\n_set, nodeID, \mute_x, mute_x)})})
					.value_(mute_x);
				GUI.staticText.new(window, Rect(230, 272, 50, 20))
					.string_("Y");
				GUI.button.new(window, Rect(230, 252, 50, 20))
					.states_([
						["play", Color.black, Color(0.9, 0.5, 0.5, 0.5)],
						["mute", Color.black, Color(0.5, 0.9, 0.5, 0.5)]
						])
					.action_({arg me;
						mute_y = me.value;
						window.view.children[0].focus(true);
						isPlaying.if(
							{server.sendMsg(\n_set, nodeID, \mute_y, mute_y)})})
					.value_(mute_y);
				GUI.staticText.new(window, Rect(290, 272, 50, 20))
					.string_("Z");
				GUI.button.new(window, Rect(290, 252, 50, 20))
					.states_([
						["play", Color.black, Color(0.9, 0.5, 0.5, 0.5)],
						["mute", Color.black, Color(0.5, 0.9, 0.5, 0.5)]
						])
					.action_({arg me;
						mute_z= me.value;
						window.view.children[0].focus(true);
						isPlaying.if(
							{server.sendMsg(\n_set, nodeID, \mute_z, mute_z)})})
					.value_(mute_z);
				});

			// 12 //

			GUI.soundFileView.new(window, Rect(10, 120, 500, 100))
				.soundfile_( sf )
				.readWithTask(0, sf.numFrames )
				.canFocus_(false)
				.waveColors_(Array.fill(sf.numChannels, rgb(0, 255, 0)))
				.gridColor_(Color.black);



			window.front;
			window.onClose_({isGui = false; isScope.if({scopefun[0].value});
				isPlaying.if({guiplayfunc[0].value})});
			},{"A soundfile must be loaded before a GUI can be made".warn});

			window.view.children[0].focus(true);

		// returns a new GUI object of this soundfile
	}

}
