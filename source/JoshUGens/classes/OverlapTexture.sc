OverlapTexture {
	var <>synthdef, <isPlaying, task, server;

	*new {arg synth, inserver;
		^super.new.init(synth, inserver);
	}

	init {arg synth, inserver;
		synthdef = synth;
		server = inserver ? Server.default;
	}

	play {arg overlaps, susdur, transdur, maxRepeats = inf, addaction = 0, target = 0, args;
		var theseargs, slot, thissynth;
		task = Task({
			maxRepeats.do{arg index;
				synthdef.isArray.if({
					slot = 0.rrand(synthdef.size - 1);
					thissynth = synthdef[0.rrand(synthdef.size - 1)];
					theseargs = Array.fill(args[slot].size, {arg i; args[slot][i].value(index)});
					server.sendBundle(server.latency,
						[\s_new, thissynth, -1, addaction, target] ++ theseargs ++
							[\susdur, susdur, \transdur, transdur]);
					((susdur + (transdur * 2)) / overlaps).wait;
					}, {
					theseargs = Array.fill(args.size, {arg i; args[i].value(index)});
					server.sendBundle(server.latency,
						[\s_new, synthdef, -1, addaction, target] ++ theseargs ++
							[\susdur, susdur, \transdur, transdur]);
					((susdur + (transdur * 2)) / overlaps).wait;
					})
				}
			});
		task.play;
		isPlaying = true;
	}

	stop {
		(isPlaying).if({task.stop; isPlaying = false});
		}
}

/*
(
SynthDef(\sinetest, {arg freq, amp = 0.2, susdur = 4, transdur = 1;
	var env, sound;
	env = EnvGen.kr(
		Env([0, 1, 1, 0], [transdur, susdur, transdur], \sin),
		doneAction: 2);
	sound = SinOsc.ar(freq, 0, amp) * env;
	Out.ar(0, sound);
}).load(s);
)

s.boot;

a = OverlapTexture.new(\sinetest);

a.play(60, 1, 1, args: [\freq, {440.rrand(880)}, \amp, 60.reciprocal])

a.stop;

a.isPlaying
*/
