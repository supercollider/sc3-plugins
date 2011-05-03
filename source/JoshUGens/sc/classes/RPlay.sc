RPlay {
	var <score, <task, <isPlaying = false, <isPaused = false, <>clock, now;

	*new {arg score, clock;
		clock = clock ? TempoClock.new;
		^super.new.init(score, clock);
		}

	init {arg argscore, argclock;
		score = argscore;
		clock = argclock;
		now = 0;
		}

	play {
		var nextfn, time, args;
		"Rplay is playing".postln;
		isPlaying = true;
		task = Task({
			score.do({arg me;
				#time, nextfn ... args = me;
				(time - now).wait;
				nextfn.value(args);
				now = time;
			})
		}).play;
	}

	pause {
		isPlaying.if({task.pause; isPaused = true}, {"RPlay not playing".warn});
		}

	resume {
		isPaused.if({task.resume; isPaused = false}, {"RPlay not paused".warn})
		}

	stop {
		isPlaying.if({task.stop; isPlaying = false; task.reset; now = 0;}, {"RPlay not playing".warn})
		}
	}

//args are passed to the funtion as a single array, which should be parsed inside the function

/*

s.boot;
//example code
SynthDef("sines", {arg freq1, freq2;
	var a;
	a = SinOsc.ar([freq1, freq2], 0, 0.3) * Line.kr(1, 0, 1, doneAction: 2);
	Out.ar(0, a);
	}).load(s);

SynthDef("sines2", {arg freq1, freq2, freq3;
	var a;
	a = Mix.ar(SinOsc.ar([freq1, freq2, freq3], 0, 0.3)) * Line.kr(1, 0, 1, doneAction: 2);
	Out.ar(0, a);
	}).load(s);

(
var event1, event2, score, r;

event1 = {arg args;
	var arg1, arg2;
	#arg1, arg2 = args;
	Synth("sines", [\freq1, arg1, \freq2, arg2]);
	};

event2 = {arg args;
	var arg1, arg2, arg3;
	#arg1, arg2, arg3 = args;
	Synth("sines2", [\freq1, arg1, \freq2, arg2, \freq3, arg3])
	};

score = [
	[1, event2, 400, 550],
	[3, event1, 440, 220],
	[5, event2, 440, 880, 1320]
	];

r = RPlay.new(score);

r.play;
)
*/