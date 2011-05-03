// make a portable Hard Disk Recorder with scope
HDR {
	var <>addAction, <>target, <sessionPath, <filename, <>headerFormat, <>sampleFormat,
		<reclevel, <channels, <bufnum, <isRecording = false, <server, <synthdef, <synthdefname,
		<>bufsize = 65536, cond, <node, <guiMode = false, <monitorMode = false, <monitorOut = 0,
		<monitorNode, <scopeBufnum, <scopeNode, <numChannels, <bfMonitorMode = false,
		<>bfMonitorOut = 0, <>bfw, <>bfx, <>bfy, <>bfz, <bfMonitorChans, <bfMonitor, cper,
		timestamp;

	*new { arg server, channels, addAction = 1, target = 0, sessionPath, filename,
			headerFormat = "aiff", sampleFormat = "int16", reclevel = 1, timestamp = true;
		^super.newCopyArgs(addAction, target, sessionPath, filename, headerFormat,
			sampleFormat, reclevel).init(channels, server, timestamp);
		}

	init {arg argChannels, argServer, argTimeStamp;
		server = argServer ? Server.default;
		server.serverRunning.not({
			server.boot;
			});
		timestamp = argTimeStamp;
		cond = Condition.new;
		channels = argChannels.isKindOf(Array).not.if({argChannels.asArray}, {argChannels});
		numChannels = channels.size;
		server.waitForBoot({
			synthdefname = this.hash.asString;
			SynthDef(synthdefname, {arg hdrgate = 1, buffer, reclevel = 1;
				var in;
				in = [];
				channels.do({arg me; in = in ++ In.ar(me, 1)});
				in = in * EnvGen.kr(
					Env([1, 1, 0], [0.1, 0.1], -10, 1),
					hdrgate, doneAction: 2) *
					Lag2.kr(reclevel, 0.1);
				DiskOut.ar(buffer, in);
				}).send(server);
			SynthDef(synthdefname++"monitor", {arg mongate = 1, outbus, reclevel = 1;
				var in;
				in = [];
				channels.do({arg me; in = in ++ In.ar(me, 1)});
				in = in * EnvGen.kr(
					Env([1, 1, 0], [0.1, 0.1], -10, 1),
					mongate, doneAction: 2) *
					Lag2.kr(reclevel, 0.1);
				Out.ar(outbus, in);
				}).send(server);
			SynthDef(synthdefname++"scope", {arg scopegate = 1, buffer, reclevel = 1;
				var in;
				in = [];
				channels.do({arg me; in = in ++ In.ar(me, 1)});
				in = in * EnvGen.kr(
					Env([1, 1, 0], [0.1, 0.1], -10, 1),
					scopegate, doneAction: 2) *
					Lag2.kr(reclevel, 0.1);
				ScopeOut.ar(in, buffer);
				}).send(server);
			});

		}

	bfMonitorChans_ {arg ... wxyz;
		wxyz = wxyz.flat;
		(wxyz.size != 4).if({
			"WXYZ array is not four channels - decoding may be incorrect".warn;
			});
		bfMonitorChans = wxyz;
		#bfw, bfx, bfy, bfz = wxyz;
		}

	startBfMonitor {
		bfMonitorMode = true;
		bfMonitor = BFMonitor(bfw, bfx, bfy, bfz, bfMonitorOut, \stereo, server);
		bfMonitor.level_(reclevel);
		bfMonitor.play;
		}

	stopBfMonitor {
		bfMonitorMode = false;
		bfMonitor.stop;
		}

	// should be run in a routine
	allocBuf {
		bufnum = server.bufferAllocator.alloc(1);
		server.sendMsgSync(cond, \b_alloc, bufnum, bufsize, channels.size);
		}

	// should be run in a routine
	openFile {
		server.sendMsgSync(cond, \b_write, bufnum, sessionPath ++
			filename ++
				timestamp.if({Date.getDate.stamp}, {""}) ++
				"." ++ headerFormat, headerFormat, sampleFormat,
				0, 0, 1);
		}

	// close and free buffer
	closeFile {
		server.sendMsg(\b_close, bufnum);
		server.sendMsg(\b_free, bufnum);
		server.bufferAllocator.free(bufnum);
		}

	record {
		isRecording.not.if({
			isRecording = true;
			cper = {this.stop; "Works".postln};
			CmdPeriod.add(cper);
			Routine.run({
				this.allocBuf;
				this.openFile;
				server.sendMsg(\s_new, synthdefname, node = server.nextNodeID, addAction, target,
					\buffer, bufnum, \reclevel, reclevel);
				})
			}, {
			"HDR is already recording".warn;
			});
		}

	stop {arg waittime = 0.0;
		isRecording.if({
			Routine.run({
				waittime.wait;
				server.sendMsg(\n_set, node, \hdrgate, 0);
				1.0.wait;
				this.closeFile;
				isRecording = false;
				CmdPeriod.remove(cper);
				})
			}, {
			"HDR is not recording".warn;
			});
		}

	reclevel_ {arg newlevel;
		reclevel = newlevel;
		isRecording.if({server.sendMsg(\n_set, node, \reclevel, newlevel)});
		monitorMode.if({server.sendMsg(\n_set, monitorNode, \reclevel, newlevel)});
		bfMonitorMode.if({bfMonitor.level_(newlevel)});
		guiMode.if({server.sendMsg(\n_set, scopeNode, \reclevel, newlevel)})
		}

	monitor {
		monitorMode = true;
		server.sendMsg(\s_new, synthdefname++"monitor",
			monitorNode = server.nextNodeID, addAction, target, \outbus, monitorOut,
			\reclevel, reclevel);
		}

	stopMonitor {
		monitorMode = false;
		server.sendMsg(\n_set, monitorNode, \mongate, 0);
		}

	monitorOut_ {arg newbus;
		monitorOut = newbus;
		(isRecording && monitorMode).if({
			server.sendMsg(\n_set, monitorNode, \outbus, monitorOut);
			})
		}



	gui {
		var window, guirout, bounds, dbspec;
		server.doWhenBooted({
			guiMode = true;
			bounds = GUI.window.screenBounds;
			scopeBufnum = server.bufferAllocator.alloc(1);
			dbspec = [-inf, 6, \db].asSpec;
			window = GUI.window.new(filename, Rect(bounds.width * 0.1, bounds.height * 0.1,
				bounds.width * 0.6, bounds.height * 0.6));

			GUI.button.new(window, Rect(window.bounds.width * 0.05, window.bounds.height * 0.05,
					window.bounds.width * 0.2, window.bounds.height * 0.07))
				.states_([
					["Start recording", Color.black, Color(0.3, 0.7, 0.3, 0.3)],
					["Stop recording", Color.black, Color(0.7, 0.3, 0.3, 0.3)]])
				.action_({arg me;
					var actions;
					actions = [
						{this.stop},
						{this.record}
						];
					actions[me.value].value
					});

			GUI.button.new(window, Rect(window.bounds.width * 0.3, window.bounds.height * 0.05,
					window.bounds.width * 0.2, window.bounds.height * 0.07))
				.states_([
					["Start monitoring", Color.black, Color(0.3, 0.7, 0.3, 0.3)],
					["Stop monitoring", Color.black,Color(0.7, 0.3, 0.3, 0.3)]])
				.action_({arg me;
					var actions;
					actions = [
						{this.stopMonitor},
						{this.monitor}
						];
					actions[me.value].value
					});

			GUI.button.new(window, Rect(window.bounds.width * 0.55, window.bounds.height * 0.05,
					window.bounds.width * 0.2, window.bounds.height * 0.07))
				.states_([
					["Start BFmonitoring", Color.black, Color(0.3, 0.7, 0.3, 0.3)],
					["Stop BFmonitoring", Color.black,Color(0.7, 0.3, 0.3, 0.3)]])
				.action_({arg me;
					var actions;
					actions = [
						{this.stopBfMonitor},
						{this.startBfMonitor}
						];
					actions[me.value].value
					});

			GUI.slider.new(window, Rect(window.bounds.width * 0.05, window.bounds.height * 0.15,
					window.bounds.width * 0.65, window.bounds.height * 0.07))
				.value_(dbspec.unmap(0))
				.action_({arg me;
					var db;
					db = dbspec.map(me.value);
					window.view.children[window.view.children.indexOf(me) + 1].value_(db);
					this.reclevel_(db.dbamp);
					});

			GUI.numberBox.new(window, Rect(window.bounds.width * 0.75,
					window.bounds.height * 0.15, window.bounds.width * 0.2,
					window.bounds.height * 0.07))
				.value_(0)
				.action_({arg me;
					var db;
					window.view.children[window.view.children.indexOf(me) + 1]
						.value_(dbspec.unmap(db));
					this.reclevel_(me.dbamp);
					});

			GUI.scopeView.new(window,
				Rect(window.bounds.width * 0.05, window.bounds.height * 0.25,
					window.bounds.width * 0.9, window.bounds.height * 0.7))
				.bufnum_(scopeBufnum)
				.waveColors_(
					Array.fill(numChannels, {Color.green})
					);

			window.onClose_({
				isRecording.if({
					this.stop
					});
				monitorMode.if({
					this.stopMonitor;
					});
				bfMonitorMode.if({
					this.stopBfMonitor;
					});
				server.sendMsg(\n_free, scopeNode);
				server.sendMsg(\b_free, scopeBufnum);
				server.bufferAllocator.free(scopeBufnum);
				});

			guirout = Routine({
				// allocate a buffer for the scope
				server.sendMsgSync(cond, \b_alloc, scopeBufnum, 4096, numChannels);
				{ window.front }.defer;
				server.sendMsg(\s_new, synthdefname++"scope", scopeNode = server.nextNodeID,
					addAction, target,	\buffer, scopeBufnum, \reclevel, reclevel);
				});

			guirout.play(AppClock);
			})

		}
	}

BFMonitor {
	var <wbus, <xbus, <ybus, <zbus, <outbus, <formatOut, <server, <cond, synthdef, <nodeID;
	var <wmute, <xmute, <ymute, <zmute, <isPlaying, <level;

	classvar formats;

	*new {arg wbus, xbus, ybus, zbus, outbus, formatOut, server;
		^super.newCopyArgs(wbus, xbus, ybus, zbus, outbus, formatOut).init(server);
		}

	init { arg argServer;
		server = argServer ? Server.default;
		server.serverRunning.not.if({
			server.boot;
			});
		cond = Condition.new;
		(wbus.isNil || xbus.isNil || ybus.isNil || zbus.isNil).if({
			"WXYZ all need values assigned!".warn;
			});
		wmute = xmute = ymute = zmute = 1.0;
		isPlaying = false;
		}

	*play {arg wbus, xbus, ybus, zbus, outbus, formatOut, server;
		^this.new(wbus, xbus, ybus, zbus, outbus, formatOut, server).play;
		}

	play {
		var players, w, x, y, z;
		players = [
			{arg playgate = 1, out, wmute = 1, xmute = 1, ymute = 1, zmute = 1, level = 1;
				var env;
				env = EnvGen.kr(
					Env([0, 1, 0], [0.1, 0.1], \sin, 1),
					playgate, doneAction: 2);
				#w, x, y, z = In.ar([wbus, xbus, ybus, zbus], 1) *
					[wmute, xmute, ymute, zmute] * level;
				Out.ar(out, [w, x, y, z] * env);
			}, {arg playgate = 1, out, wmute = 1, xmute = 1, ymute = 1, zmute = 1, level = 1;
				var env;
				env = EnvGen.kr(
					Env([0, 1, 0], [0.1, 0.1], \sin, 1),
					playgate, doneAction: 2);
				#w, x, y, z = In.ar([wbus, xbus, ybus, zbus], 1) *
					[wmute, xmute, ymute, zmute] * level;
				Out.ar(out, B2Ster.ar(w, x, y) * env);
			}, {arg playgate = 1, out, wmute = 1, xmute = 1, ymute = 1, zmute = 1, level = 1;
				var env;
				env = EnvGen.kr(
					Env([0, 1, 0], [0.1, 0.1], \sin, 1),
					playgate, doneAction: 2);
				#w, x, y, z = In.ar([wbus, xbus, ybus, zbus], 1) *
					[wmute, xmute, ymute, zmute] * level;
				Out.ar(out, B2UHJ.ar(w, x, y) * env);
			}, {arg playgate = 1, out, wmute = 1, xmute = 1, ymute = 1, zmute = 1, level = 1;				var env;
				env = EnvGen.kr(
					Env([0, 1, 0], [0.1, 0.1], \sin, 1),
					playgate, doneAction: 2);
				#w, x, y, z = In.ar([wbus, xbus, ybus, zbus], 1) *
					[wmute, xmute, ymute, zmute] * level;
				Out.ar(out, BFDecode1.ar(w, x, y, z, [0.25, -0.25, -0.75, -1.25] * pi) * env);
			}, {arg playgate = 1, out, wmute = 1, xmute = 1, ymute = 1, zmute = 1, level = 1;				var env;
				env = EnvGen.kr(
					Env([0, 1, 0], [0.1, 0.1], \sin, 1),
					playgate, doneAction: 2);
				#w, x, y, z = In.ar([wbus, xbus, ybus, zbus], 1) *
					[wmute, xmute, ymute, zmute] * level;
				Out.ar(out, BFDecode1.ar(w, x, y, z, [0.1666, -0.1666, -0.5, -0.8333, 0.8333, 0.5]
					* pi) * env);
			}
			];
		Routine.run({
			isPlaying.not.if({
				isPlaying = true;
				"Routine".postln;
				formats[formatOut].postln;
				synthdef = players[formats[formatOut]].asSynthDef.send(server);
				nodeID = server.nextNodeID;
				server.sync(cond);
				server.sendMsg(\s_new, synthdef.name, nodeID, 1, 0, \wmute, wmute, \xmute, xmute,
					\ymute, ymute, \zmute, zmute);
				}, {
				"BFMonitor already playing".postln;
				})
			})
		}

	stop {
		isPlaying.if({
			isPlaying = false;
			server.sendMsg(\n_set, nodeID, \playgate, 0);
			})
		}

	level_ {arg newlevel;
		level = newlevel;
		isPlaying.if({
			server.sendMsg(\n_set, nodeID, \level, level)
			});
		}

	*initClass {
		formats = IdentityDictionary[
			\straight -> 0,
			\stereo -> 1,
			\uhj -> 2,
			\quad -> 3,
			\hex -> 4
			];
		}
	}
