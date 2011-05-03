ListTrig2	Emit a sequence of triggers at specified time intervals

	ListTrig2.kr(bufnum, reset, numframes)

The data stored in the [Buffer] at bufnum should be a (single-channel)
list of time intervals in seconds. ListTrig2 will then emit a
trigger signal (a single-sample value of 1, at control rate) at each
of those interval times, which are measured from the previous trigger.

reset starts the sequence again from the beginning.

numframes tells the UGen the size of the buffer. If not set, this will
automatically be filled with BufFrames.kr(bufnum), which is typically
what you want to use anyway.

See also ListTrig, which triggers for a sequence of time offsets.

Example

s.boot;
// Everyone likes Fibonacci numbers:
b = Buffer.loadCollection(s, [1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89] * 0.1);
// Or you could load some numbers from a file:
b = Buffer.loadCollection(s, FileReader.read("/Users/danstowell/svn/stored_docs/bbx annots/onsets_gt/vb5gt.txt", true, true).collect(_.at(0).asFloat));
(
// ListTrig2 used here to output some simple grains.
// I'm also using .poll and a ramp to output the calculated time value, to check the output.
// Note the accuracy, which is limited to the accuracy of the control rate.
x = { |t_reset=0|
	var trigs, env, son, ramp;
	trigs = ListTrig2.kr(b.bufnum, t_reset);
	env = EnvGen.ar(Env.perc(0.01, 0.1), trigs);
	son = SinOsc.ar(440, 0, env * 0.2);

	ramp = Phasor.kr(t_reset, ControlRate.ir.reciprocal, 0, inf);
	ramp.poll(trigs, "Trigger at time offset");

	son.dup;
}.play(s)
);
x.set(\t_reset, 1);