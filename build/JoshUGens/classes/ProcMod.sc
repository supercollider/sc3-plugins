ProcMod {
	var <amp, <>group, <addAction, <target, 
		<timeScale, <lag, <>id, <>function, 
		<>releaseFunc, <>onReleaseFunc, <responder, <envnode, <isRunning = false, <data,
		<starttime, <window, gui = false, <button, <process, retrig = false, isReleasing = false,
		oldgroups, <>clock, <env, <>server, <envbus, <releasetime, uniqueClock = false,
		<tempo = 1;

	*new {arg env, amp = 1, id, group, addAction = 0, target = 1, function, releaseFunc,
			onReleaseFunc, responder, timeScale = 1, lag = 0.01, clock, server;
		^super.newCopyArgs(amp, group, addAction, target, timeScale, lag, id, function, 
			releaseFunc, onReleaseFunc, responder).init(clock, server, env);
	}
	
	init {arg argClock, argServer, argEnv;
		server = argServer ?? {Server.default};
		env = argEnv;
		(env.notNil).if({
			env.isKindOf(Env).if({
				releasetime = env.releaseTime;
				}, {
				env.isKindOf(Number).if({releasetime = env})
				})
			});
		clock = argClock;// ?? {uniqueClock = true; TempoClock.new};
		oldgroups = [];
		}
		
	*play {arg env, amp = 1, id, group, addAction = 0, target = 1, function, 
			releaseFunc, onReleaseFunc, responder, timeScale = 1, lag = 0.01, clock, server;
		^this.new(env, amp, id, group, addAction, target, function, releaseFunc, onReleaseFunc,
			responder, timeScale, lag, clock, server).play;
		}

	play {
		var thisfun;
		clock = clock ?? {uniqueClock = true; TempoClock.new(tempo)};
		isRunning.not.if({	
			isRunning = true;
			// add the responder if it isn't nil
			responder.notNil.if({responder.add});
			// create this Proc's group, and if there is an env, start it
			// also, if there is no release node, schedule the Procs release
			group = group ?? {server.nextNodeID};
			env.isKindOf(Env).if({
				envbus = server.controlBusAllocator.alloc(1);
				server.sendBundle(nil,
					[\g_new, group, addAction, target], 
					[\s_new, \procmodenv_5216, envnode = server.nextNodeID, 0, group, 
						\outbus, envbus, \amp, amp, \timeScale, timeScale, \lag, lag],
					[\n_setn, envnode, \env, env.asArray.size] ++ env.asArray);
				env.releaseNode.isNil.if({
					clock.sched(env.times.sum, {
						this.release; 
						{gui.if({button.value_(0)})}.defer;
						});
					})
				}, {
				server.sendBundle(nil, [\g_new, group, addAction, target])
				});
			starttime = Main.elapsedTime;
			function.isKindOf(Function).if({
				retrig = true;
				thisfun = function.value(group, envbus, server);
				(thisfun.isKindOf(Routine) || thisfun.isKindOf(Task)).if({
					process = thisfun; this.processPlay;
					})
				}, {
				process = function; this.processPlay;
				})
			})
		}
	
	tempo_ {arg newTempo;
		tempo = newTempo;
		clock.notNil.if({
			clock.isKindOf(TempoClock).if({
				clock.tempo_(tempo)
				})
			})
		}
			
	processPlay {
		process.reset;
		process.play(clock);
		}
	
	now {
		^Main.elapsedTime - starttime
		}
	
	value {
		this.play;
	}
	
	amp_ {arg newamp;
		amp = newamp;
		envnode.notNil.if({server.sendBundle(nil, [\n_set, envnode, \amp, amp])});
	}
		
	lag_ {arg newlag;
		lag = newlag;
		envnode.notNil.if({server.sendBundle(nil, [\n_set, envnode, \lag, lag])});
	} 
	
	saveToData {arg anAssociation;
		data.isNil.if({data = Dictionary.new(1)});
		data.add(anAssociation);
		}
		
	env_ {arg newenv;
		isRunning.if({
			"You can not change an envelope while the ProcMod is running".warn;
			}, {
			env = newenv;
			})
	}

	addAction_ {arg newaddAction;
		isRunning.if({
				"You can not change an addAction while the ProcMod is running".warn;
				}, {
				addAction = newaddAction;
				})
		}
		
	target_ {arg newtarget;
		isRunning.if({
			"You can not change the target while the ProcMod is running".warn;
			}, {
			target = newtarget;
			})
		}
		
	timeScale_ {arg newtimeScale;
		isRunning.if({
			"You can not change the timescale while the ProcMod is running".warn;
			}, {
			timeScale = newtimeScale;
			})
		}
		
	release {arg reltime;
		var curproc, curresp, curgroup, currelfunc, newrelval, curclock;
		curproc = process;
		curresp = responder;
		responder = nil;
		curgroup = group;
		currelfunc = releaseFunc;
		uniqueClock.if({curclock = clock; clock = nil});
		isRunning.if({
			onReleaseFunc.value;
			env.notNil.if({
				newrelval = reltime.notNil.if({
					reltime.neg - 1;
					}, {
					0
					});
				server.sendMsg(\n_set, group, \gate, newrelval);
				});
			releasetime.notNil.if({
				newrelval = reltime.notNil.if({
					reltime
					}, {
					releasetime
					});
				curclock.sched(newrelval, {
					this.clear(curproc, curresp, curgroup, currelfunc, curclock)
					})
				}, {
				this.clear(curproc, curresp, curgroup, currelfunc, curclock)
				});
			});
		// if retriggerable... clear out group now
		retrig.if({
			oldgroups = oldgroups.add(group); 
			group = nil; 
			isRunning = false; 
			isReleasing = true;
			});
		}
	
	kill {
		var curproc, curresp, curgroup, currelfunc, oldclock;
		curproc = process;
		curresp = responder;
		curgroup = group;
		currelfunc = releaseFunc;
		isRunning.if({server.sendMsg(\n_free, curgroup)});
		isReleasing.if({oldgroups.do({arg me; server.sendMsg(\n_free, me)})});
		// if a tempo clock was created for this procMod, clear it
		uniqueClock.if({clock.clear; oldclock = clock; clock = nil});
		curproc.stop;
		this.clear(curproc, curresp, curgroup, currelfunc, oldclock);
		isRunning = false;

	}
	
	// stops the function that is running
	stopprocess {arg oldproc;
		oldproc.stop;
	}

// need to check for TempoClock - old clocks need to be saved until 
// they can be garbage collected
	clear {arg oldproc, oldresp, oldgroup, oldrelfunc, oldclock;
		server.sendMsg(\n_free, oldgroup);
		oldgroups.remove(oldgroup);
		oldproc.notNil.if({this.stopprocess(oldproc)});
		oldrelfunc.notNil.if({
			oldrelfunc.isKindOf(Function).if({
				oldrelfunc.value;
				}, {
				oldrelfunc.reset; oldrelfunc.play(clock)
				})
			});
		oldresp.notNil.if({oldresp.remove});
		retrig.not.if({isRunning = false});
		retrig.if({isReleasing = false});
		oldclock.notNil.if({ oldclock.stop });
//		gui.if({{button.value_(0)}.defer});//
		}

	responder_ {arg aResponder;
		responder = aResponder;
		isRunning.if({responder.add});
		}
			
	// ProcMod.gui should create a small GUI that will control the ProcMod - start / stop, amp
	gui {arg bounds = Rect(0, 0, 400, 70), upperLevel = 6, lowerLevel = -inf, parent;
		var slider, numbox, winw, winh, dbspec, xspace, yspace;
		gui = true;
		window = parent ?? {GUI.window.new(this.id, bounds)};
		winh = bounds.height;
		winw = bounds.width;
		window.view.decorator.isNil.if({
			window.view.decorator = FlowLayout( window.view.bounds, Point(10, 10), Point(10, 10));
			});
		xspace = window.view.decorator.gap.x * 1.5;
		yspace = window.view.decorator.gap.y * 1.5;
		window.front;
		window.onClose_({gui = false});
		button = GUI.button.new(window, Rect(0, 0, winw * 0.3 - xspace, winh * 0.8 - yspace))
			.states_([
				["start " ++ this.id, Color.black, Color(0.3, 0.7, 0.3, 0.3)],
				["stop " ++ this.id, Color.black, Color(0.7, 0.3, 0.3, 0.3)],
				]);
		isRunning.if({button.value_(1)});
		button.action_({arg me;
				var actions;
				actions = [
					{this.release},
					{this.play}
					];
				actions[me.value].value;
				});
		dbspec = [lowerLevel, upperLevel, \db].asSpec;
		slider = GUI.slider.new(window, Rect(winw * 0.3, 0, winw * 0.5 - xspace, winh * 0.8 - yspace))
			.value_(dbspec.unmap(amp.ampdb).quantize(0.01))
			.action_({arg me;
				var ampval;
				ampval = dbspec.map(me.value);
				this.amp_(ampval.dbamp);
				numbox.value_(ampval.quantize(0.01));
				});
		numbox = GUI.numberBox.new(window, Rect(winw * 0.8, 0, winw * 0.2 - xspace, winh * 0.8- yspace))
			.value_(amp.ampdb.quantize(0.01))
			.action_({arg me;
				this.amp_(me.value.dbamp);
				slider.value_(dbspec.unmap(me.value));
				});
		^this;
		}
	
	*initClass {
		StartUp.add {
		SynthDef(\procmodenv_5216, {arg gate = 1, outbus, amp = 1, timeScale = 1, lag = 0.01;
			var env;
			env = EnvGen.kr(
				Control.names(\env).kr(Env.newClear(30)), gate, 
					1, 0, timeScale, doneAction: 13) * Lag2.kr(amp, lag);
			Out.kr(outbus, env);
		}).writeDefFile;
		}
	}
}

// proc events should
// - create a GUI for rehearsal purposes -
//		- should include a bank of sliders for each Proc Mod (and identify the event number)
//  		- should be able to save values of amps to a file
//		- MAYBE? allow you to set certain parameters of a ProcMod (like envelope release time, etc)

ProcEvents {
	var <eventDict, <ampDict, <eventArray, <releaseArray, <timeArray, <index,
		<id, gui = false, <window, <server, firstevent = false, initmod, killmod, <amp,
		<procampsynth, procevscope = false, <pracwindow, <pracmode = false, pracdict, 
		<eventButton, <killButton, <releaseButton, >starttime = nil, <pedal = false, 
		<pedalin, <triglevel, <pedrespsetup, <pedresp, <pedalnode, <pedalgui, bounds,
		<bigNum = false, bigNumWindow;
	
	*new {arg events, amp, initmod, killmod, id, server;
		server = server ?? {Server.default};
		^super.new.init(events, amp, initmod, killmod, id, server);
	}
	
	init {arg events, argamp, arginitmod, argkillmod, argid, argserver;
		var proc, release, newproc, evid;
		eventDict = Dictionary.new(events.flat.size);
		eventArray = Array.fill(events.size, {Array.new});
		releaseArray = Array.fill(events.size, {Array.new});
		timeArray = Dictionary.new(events.flat.size);
		id = argid;
		server = argserver;
		amp = argamp;
		index = 0;
		firstevent = true; 
		arginitmod.notNil.if({initmod = arginitmod;});
		argkillmod.notNil.if({killmod = argkillmod});
		events.do{arg me, i;
			#proc, release = me;
			proc.asArray.flat.do{arg thisev;
				thisev.isKindOf(ProcMod).if({
					thisev.id.isNil.if({ thisev.id = thisev.identityHash.abs.asString });
					eventDict.put(thisev.id, thisev);
					eventArray[i] = eventArray[i].add(thisev.id)
					}, {
					(thisev.isKindOf(Function)).if({
						evid = thisev.identityHash.abs.asString;
						newproc = ProcMod(id: evid).function_(thisev);
						eventDict.put(newproc.id, newproc);
						eventArray[i] = eventArray[i].add(newproc.id);
						}, {
						"ProcEvents needs a ProcMod or Function to work properly".warn;
						});
					});
				};
			release.asArray.do{arg thisrel;
				var id;
				thisrel.isKindOf(ProcMod).if({
					releaseArray[i] = releaseArray[i].add(thisrel.id)
					}, {
					releaseArray[i] = releaseArray[i].add(thisrel)
					})
				}
			};
		// for the GUI if needed
//		bounds = GUI.window.screenBounds;	//
	}
	
	index_ {arg nextIdx;
		index = nextIdx;
		gui.if({window.view.children[0].value_(nextIdx)});
		}
		
	next {
		this.play(index);
		index = index + 1;
	}
	
	play {arg event;
		firstevent.if({
			"Play".postln;
			// for some reason, asking for the first node ID sometimes throws a
			// 'duplicate node' error. This avoids it
			starttime.isNil.if({starttime = Main.elapsedTime});
			initmod.notNil.if({initmod.value}); 
			server.sendMsg(\s_new, \procevoutenv6253, procampsynth = server.nextNodeID, 1,
				0, \amp, amp);
			firstevent = false;
			});
		eventArray[event].do{arg me; 
			timeArray.put(eventDict[me].id, Main.elapsedTime);
			eventDict[me].value;
			pracmode.if({pracwindow.view.children[pracdict[me]].valueAction_(1);
			})};
		releaseArray[event].do{arg me; eventDict[me].release;
			pracmode.if({pracwindow.view.children[pracdict[me]].valueAction_(0);
			})};
	}
	
	releaseAll {arg rel = true;
		rel.if({eventDict.do{arg me; me.isRunning.if({me.release})}});
		gui.if({
			window.view.children[2].string_("No events currently running");
			window.view.children[1].focus(true);
			});
	}

	reset {
		this.releaseAll(false);
		eventDict.do{arg me; me.isRunning.if({me.kill})};
		server.freeAll;
		firstevent = true;
		index = 0;
		gui.if({
			window.view.children[0].value_(0);
			});
		starttime = nil;
		eventDict.do{arg me; me.group = server.nextNodeID};

	}

	scope {
		procevscope = true;
		^(server.name == 'internal').if({server.scope}, {"Must use internal server".warn});
	}
	
	killAll {
		eventDict.do{arg me; me.isRunning.if({me.kill})};
		killmod.notNil.if({killmod.value});
		initmod.notNil.if({initmod.kill});
		gui.if({window.close; gui = false; pracwindow.notNil.if({pracwindow.close})});
		pedal.if({pedrespsetup.remove; pedresp.remove; pedalgui.notNil.if({pedalgui.close})});
		this.reset;
	}
	
	now {arg id;
		^id.isNil.if({
			Main.elapsedTime - starttime;
			}, {
			Main.elapsedTime - (this.timeArray[id]);
			})
		}
	
	starttime {arg id;
		^id.isNil.if({
			starttime;
			}, {
			this.timeArray[id] - starttime;
			})
		}
		
	pedalTrig {arg pedalbus, headroom = 2, trigwindow = 0.5, testdur = 2, guiBounds,
				addAction = 1, target = 0;
			var pedlevel = 0, numlevels, testlevel = 0, 
				testinc = 0;
			var testnode, testid, pedalid, headspec;
			headspec = ControlSpec.new(0, headroom * 10, \lin);
			guiBounds = guiBounds ?? {gui.if({
				Rect(window.bounds.left + window.bounds.width + 10, 
					window.bounds.top,window.bounds.width * 0.3, 
					window.bounds.height * 0.7);
					}, {Rect(10, 10, 144, 288)})};
			server.serverRunning.if({
				pedal = true;
				// poll every 0.1 seconds
				numlevels = (testdur / 0.1).floor;
				testnode = server.nextNodeID;
				testid = server.nextNodeID;
				pedalnode = server.nextNodeID;
				pedalid = server.nextNodeID;
				// the actual responder that will cause pedal triggers to work
				
				pedresp = OSCresponderNode(server.addr, '/tr', {arg time, resp, msg;
					(msg[2] == pedalid).if({
						// if there is a GUI, advance it and run the next function,
						// else, just call .next
						gui.if({
							{eventButton.valueAction_(eventButton.value + 1)}.defer;
							}, {
							this.next;
							});
						});
					});
				
				pedrespsetup = OSCresponderNode(server.addr, '/tr', {arg time, resp, msg;
					(msg[2] == testid).if({
						(testinc < numlevels).if({
							("Pedal setup on bus: " ++ pedalbus ++ " turn: " ++
								 testinc).postln;
							testlevel = testlevel + msg[3];
							testinc = testinc + 1;
							}, {
							server.sendMsg(\n_free, testnode);
							// calc the trig level, and give it some headroom
							testlevel = ((testlevel + msg[3]) / numlevels);
							// start the pedal listening at the head of 0
							server.sendMsg(\s_new, \procevtrig2343, pedalnode, addAction,
							 	target, \pedalin, pedalbus, \id, pedalid, \level, testlevel,
								\trigwindow, trigwindow, \headroom, headroom);
							// add the pedresp responder
							"Pedal ready".postln;
							pedresp.add;
							{
								pedalgui = GUI.window.new("pedal", guiBounds).front;
								GUI.button.new(pedalgui, Rect(pedalgui.bounds.width * 0.1, 
										pedalgui.bounds.height * 0.02, 
										pedalgui.bounds.width * 0.8, 
										pedalgui.bounds.height * 0.2))
									.states_([
										["Mute Trig", Color.black, 
											Color(0.7, 0.3, 0.3, 0.3)],
										["Allow Trig", Color.black, 
											Color(0.3, 0.7, 0.3, 0.3)]
										])
									.action_({arg me;
										server.sendMsg(\n_set, pedalnode, 
											\mute, (me.value - 1).abs);
										});
								GUI.slider.new(pedalgui, Rect(pedalgui.bounds.width * 0.1, 
										pedalgui.bounds.height * 0.3,
										pedalgui.bounds.width * 0.3, 
										pedalgui.bounds.height * 0.6))
									.value_(headspec.unmap(headroom))
									.action_({arg me;
										var idx, mapval;
										mapval = headspec.map(me.value.round(0.01));
										idx = pedalgui.view.children.indexOf(me);
										pedalgui.view.children[idx + 1].value_(mapval);
										server.sendMsg(\n_set, pedalnode, \headroom, mapval); 
										});
								GUI.numberBox.new(pedalgui, Rect(pedalgui.bounds.width * 0.45, 
										pedalgui.bounds.height * 0.8, 
										pedalgui.bounds.width * 0.4,
										pedalgui.bounds.height * 0.1))
									.value_(headroom)
									.action_({arg me;
										var idx;
										idx = pedalgui.view.children.indexOf(me);
										pedalgui.view.children[idx - 1].value_(
											headspec.unmap(me.value));
										server.sendMsg(\n_set, pedalnode, 
											\headroom, me.value); 

										});
								gui.if({window.front});
							}.defer;
							// remove this repsonder
							pedrespsetup.remove;
							})
						})
					}).add;
				
				server.sendMsg(\s_new, \procevtesttrig76234, testnode, addAction, target,
					\pedalin, pedalbus, \id, testid);

			}, {"Server isn't booted, pedal trig can't be loaded".warn})
		}
			
	pracGUI {
		var guibounds, stripwidth, stripheight, level;
		this.perfGUI;
		pracmode = true;
		pracdict = Dictionary.new;
		//bounds = SCWindow.screenBounds;
		pracwindow = GUI.window.new(id.asString ++ " practice window", 
			guibounds = Rect(20, 20, bounds.width * 0.9, bounds.height * 0.9)).front;
		stripwidth = (guibounds.width - 10) * 0.066;
		stripheight = (guibounds.height - 40) * 0.25;
		eventArray.flat.do{arg me, i;
			var thisbutton;
			level = (i / 15).floor;
			thisbutton = GUI.button(pracwindow,
				Rect(5 + (stripwidth * (i % 15)),
					5 + ((level * stripheight) + (level * 5)),
					stripwidth * 0.9, stripheight * 0.1))
				.states_([
					[me.asString, Color.black, Color(0.3, 0.7, 0.3, 0.7)],
					[me.asString, Color.black, Color(0.7, 0.3, 0.3, 0.7)]
					])
				.action_({arg button;
					var actions;
					firstevent.if({initmod.value; 
						server.sendMsg(\s_new, \procevoutenv6253, 
							procampsynth = server.nextNodeID, 1, 0, \amp, amp);
						firstevent = false});
					actions = [{eventDict[me].release}, {eventDict[me].play}];
					actions[button.value].value;
					});
					
			pracdict.add(me -> pracwindow.view.children.indexOf(thisbutton));
			
			GUI.slider.new(pracwindow,
				Rect(5 + (stripwidth * (i % 15)),
					5 + ((level * stripheight) + (((stripheight * 0.2) + (level *  5)))),
					stripwidth * 0.3, stripheight * 0.6))
				.value_(0.7079)
				.action_({arg slider;
					var val;
					val = ControlSpec(-90, 6, \db).map(slider.value);
					pracwindow.view.children[pracwindow.view.children.indexOf(slider) + 1]
						.value_(val.round(0.01));
					eventDict[me].amp_(val.dbamp);
					});
			
			GUI.numberBox.new(pracwindow,
				Rect(5 + (stripwidth * (i % 15)),
					10 + ((level * stripheight) + (((stripheight * 0.8) + (level * 5)))),
					stripwidth * 0.75, stripheight * 0.125))
				.value_(0)
				.action_({arg number;
					var val;
					val = ControlSpec(-90, 6, \db).unmap(number.value);
					pracwindow.view.children[pracwindow.view.children.indexOf(number) - 1]
						.value_(val);
					eventDict[me].amp_(val);
					});
				

			};
		pracwindow.onClose_({pracmode = false});
		^pracwindow;
		}
		
	bigNumGUI {arg bigNumBounds, fontSize = 96;
		var numstring;
		bigNumBounds = bigNumBounds ?? {Rect(bounds.width * 0.5, bounds.height * 0.5,
			bounds.width * 0.3, bounds.height * 0.3)};
		bigNum = true;
		bigNumWindow = GUI.window.new(id.asString, bigNumBounds);
		bigNumWindow.front;
		bigNumWindow.onClose_({bigNum = false});
		GUI.staticText.new(bigNumWindow, Rect(0, 0, bigNumBounds.width, bigNumBounds.height))
			.string_("No event")
			.font_(Font("Arial", fontSize))
			.align_(\center);
		gui.if({window.front});
		^this;
		}
		
	perfGUI {arg guiBounds, buttonColor = Color(0.3, 0.7, 0.3, 0.7);
		var buttonheight, buttonwidth;
		bounds = GUI.window.screenBounds;	
		
		guiBounds = guiBounds ?? {Rect(10, bounds.height * 0.5, bounds.width * 0.3, 
				bounds.height * 0.3)};
				
		gui = true;
		
		window = GUI.window.new(id.asString, guiBounds);
		
		buttonheight = guiBounds.height * 0.15;
		buttonwidth = guiBounds.width * 0.5;
		
		GUI.numberBox.new(window, Rect(10 + buttonwidth, 10, buttonwidth * 0.9, 
				buttonheight * 0.9))
			.value_(index)
			.font_(Font("Arial", 24))
			.action_({arg me;
				window.view.children[window.view.children.indexOf(me) + 1].focus(true);
				});
			
		eventButton = GUI.button.new(window, Rect(10, 10, buttonwidth * 0.9, 
				buttonheight * 0.9))
			.states_([
				["Next Event:", Color.black, buttonColor]
				])
			.font_(Font("Arial", 24))
			.action_({arg me;
				var numbox, numboxval;
				numbox = window.view.children.indexOf(me)-1;
				numboxval = window.view.children[numbox].value;
				bigNum.if({
					bigNumWindow.view.children[0].string_(numboxval)
					});
				(numboxval < eventArray.size).if({
					this.play(numboxval);
					window.view.children[numbox].value_(numboxval + 1);
					window.view.children[window.view.children.indexOf(me)+1]
						.string_("Current Event: "++numboxval);
					}, {
					"No event at that index".warn
					})
				});
						
		GUI.staticText.new(window, Rect(10, 10 + buttonheight, buttonwidth * 1.8, 
				buttonheight * 0.9))
			.font_(Font("Arial", 24))
			.string_("No events currently running");

		GUI.button.new(window, Rect(10, 10 + (buttonheight * 2), buttonwidth * 0.9, 
				buttonheight * 0.9))
			.states_([
				["Reset", Color.black, buttonColor]
				])
			.font_(Font("Arial", 24))
			.action_({arg me;
				window.view.children[window.view.children.indexOf(me) - 2].focus(true);
				this.reset;
				});				 

		releaseButton = GUI.button.new(window, Rect(10, 10 + (buttonheight * 3), buttonwidth * 0.9, 
				buttonheight * 0.9))
			.states_([
				["Release All", Color.black, buttonColor]
				])
			.font_(Font("Arial", 24))
			.action_({arg me;
				window.view.children[window.view.children.indexOf(me) - 3].focus(true);
				this.releaseAll;
				});

		killButton = GUI.button.new(window, Rect(10, 10 + (buttonheight * 4), buttonwidth * 0.9, 
				buttonheight * 0.9))
			.states_([
				["Kill All", Color.black, buttonColor]
				])
			.font_(Font("Arial", 24))
			.action_({arg me;
				window.view.children[window.view.children.indexOf(me) - 4].focus(true);
				this.killAll;
				});
		
		GUI.slider.new(window, Rect(buttonwidth + 10, 5 + buttonheight * 2, buttonwidth * 0.2,
				buttonheight * 3))
			.value_(ControlSpec(-90, 6, \db).unmap(amp.ampdb))
			.action_({arg me;
				var val;
				val = ControlSpec(-90, 6, \db).map(me.value);
				amp = val;
				server.sendMsg(\n_set, procampsynth, \amp, val.dbamp.postln);
				window.view.children[window.view.children.indexOf(me) + 1].value_(
					val.round(0.01));
				});
				
		GUI.numberBox.new(window, Rect((buttonwidth * 1.3) + 10, 5 + buttonheight * 2, 
				buttonwidth * 0.4, buttonheight * 0.9))
			.value_(amp.ampdb)
			.font_(Font("Arial", 24))
			.action_({arg me;
				var val;
				val = ControlSpec(-90, 6, \db).unmap(me.value);
				window.view.children[window.view.children.indexOf(me) - 1].value_(val);
				server.sendMsg(\n_set, procampsynth, \amp, me.value.dbamp.postln);
				});
				
		window.view.children[1].focus(true);
		
		window.onClose_({this.killAll; gui = false; bigNum.if({bigNumWindow.close})});
		
		window.front;
		
		^this
		}
	
	*initClass {
		StartUp.add {	
			SynthDef(\procevoutenv6253, {arg amp = 1, lag = 0.2;
				ReplaceOut.ar(0, In.ar(0, 8) * Lag2.kr(amp, lag))
				}).writeDefFile;	
			
			SynthDef(\procevtesttrig76234, {arg pedalin, id, dur = 2;
					var ped;
					ped = RunningSum.rms(In.ar(pedalin), 0.1 * SampleRate.ir);
					SendTrig.ar(Impulse.ar(10), id, ped);
				}).writeDefFile;
				
			SynthDef(\procevtrig2343, {arg pedalin, id, level = 1, headroom = 1, trigwindow = 1,
					mute = 1;
				var ped;
				ped = In.ar(pedalin) * mute;
				SendTrig.ar(Trig1.ar(ped > (level * headroom), trigwindow), id, 1);
				}).writeDefFile;	
			
			}
	}

}

/* this is predominantly a GUI class that stores ProcMods and displays them in a GUI. The GUI has 
* two parts. A window that shows ProcMods in GUI mode, and a DragSink that takes ProcMods, places
* them in the GUI, and can save them to a file as an archive
*/

ProcSink {
	var <name, initmod, killmod, server, <sinkWindow, <sinkBounds, <procWindow, <bounds, <numProcs,
		<screenWidth, <screenHeight, <procMods, gui = false, <columns, rows, baseheight,
		<>starttime, firstProc = true;

	*new {arg name, sinkBounds, bounds, columns = 3, initmod, killmod, server, parent;
		server = server ?? {Server.default};
		^super.newCopyArgs(name, initmod, killmod, server).init(sinkBounds, bounds, columns, parent);
	}
	
	*loadFromFile {arg path;
		^ProcSink.readArchive(path).initFromFile;
		}
	
	init {arg argSinkBounds, argBounds, argColumns, argParent;
		procMods = Array.new;
		numProcs = 0;
		columns = argColumns;
		rows = 1;
		screenWidth = GUI.window.screenBounds.width;
		screenHeight = GUI.window.screenBounds.height;
		sinkBounds = argSinkBounds ?? {Rect(10, screenHeight * 0.6, screenWidth * 0.1 - 20, 
			screenHeight * 0.2 - 20)};
		sinkWindow = GUI.window.new("ProcSink", sinkBounds).front;
		GUI.dragSink.new(sinkWindow, Rect(5, 5, sinkBounds.width - 10, sinkBounds.height * 0.6))
			.action_({arg me;
				this.add(me.object)
				});
		GUI.button.new(sinkWindow, Rect(5, sinkBounds.height * 0.7, sinkBounds.width - 10,
				sinkBounds.height * 0.3 - 10))
			.states_([
				["Start piece", Color.black, Color(0.7, 0.3, 0.3, 0.3)],
				["Stop piece", Color.black, Color(0.7, 0.3, 0.3, 0.3)],
				])
			.action_({arg me;
				var actions;
				actions = [
					{this.kill},
					{this.start}
					];
				actions[me.value].value;
				});
		sinkWindow.onClose_({this.kill});
		sinkWindow.view.children[1].focus(true);
		bounds = argBounds ?? {Rect(screenWidth * 0.1, screenHeight * 0.9, 
			screenWidth * 0.9, screenHeight * 0.1)};
		baseheight = bounds.height;
		this.gui(argParent);
		}
	
	gui {arg parent;
		procWindow = parent ?? {GUI.window.new(name, bounds).front};
		procWindow.view.decorator = 
			FlowLayout(procWindow.view.bounds, Point(10, 10), Point(10, 10));
		procWindow.onClose_({this.kill});
		sinkWindow.front;
		}
		
	start {
		procWindow.front;
		initmod.notNil.if({initmod.play});
		starttime = Main.elapsedTime;
		}
	
	now {
		^(Main.elapsedTime - starttime)
		}
		
	kill {
		{
			procWindow.isClosed.not.if({procWindow.close});
			sinkWindow.isClosed.not.if({sinkWindow.close});
		}.defer;
		procMods.do({arg me; (me.isRunning).if({ me.kill })});
		killmod.notNil.if({killmod.play});
		}
		
	add {arg proc, upperLevel = 6, lowerLevel = -inf; 
		numProcs = numProcs + 1;
		procMods = procMods.add(proc);
		rows = (numProcs / columns).ceil;
		{
		procWindow.bounds_(Rect(procWindow.bounds.left, procWindow.bounds.top, 
			procWindow.bounds.width, rows * baseheight - procWindow.view.decorator.gap.x));
		proc.gui(Rect(0, 0, procWindow.bounds.width / columns, baseheight), upperLevel, 
			lowerLevel, procWindow)}.defer;
		// why does this need to be contiually reset?
		procWindow.onClose_({this.kill});
	}
			

}

