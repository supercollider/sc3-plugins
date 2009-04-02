ClockFace {
	var <starttime, <tempo, <>inc, <cursecs, isPlaying = false, clock, <window, timeString;
	var remFun;
	
	*new{ arg starttime = 0, tempo = 1, inc = 0.1;
		^super.newCopyArgs(starttime, tempo, inc).init;
		}
		
	init {
		cursecs = starttime;
		this.digitalGUI;
		}
		
	play {
		var start;
		clock = TempoClock.new(tempo);
		start = clock.elapsedBeats;
		remFun = {this.stop};
		CmdPeriod.add(remFun);
		clock.sched(inc, {
			this.cursecs_(clock.elapsedBeats - start + starttime, false);
			inc;
			})
		}
		
	cursecs_ {arg curtime, updateStart = true;
		var curdisp;
		cursecs = curtime;
		curdisp = curtime.asTimeString;
		curdisp = curdisp[0 .. (curdisp.size-3)];
		updateStart.if({starttime = cursecs});
		{timeString.string_(curdisp)}.defer;
		}
		
	stop {
		starttime = cursecs;
		clock.clear;
		CmdPeriod.remove(remFun);
		clock.stop;
		}
		
	digitalGUI {
		window = GUI.window.new("Digital Clock", Rect(10, 250, 450, 110)).front;
		timeString = GUI.staticText.new(window, Rect(0, 0, 430, 100))
			.string_(cursecs.asTimeString)
			.font_(Font("Arial", 40));
		window.onClose_({this.stop});
		}

}

