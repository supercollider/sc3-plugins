Feedback {
//basic feedback class... buffer size needs to be the same as the delayTime
//e.g. for a 0.1 sec delay time, a buffer should be allocated as such:
//s.sendMsg(\b_alloc, 1, 44100*0.1, 1);
	*ar {arg in, feedbackpct = 1, delayTime = 0.1, bufnum = -1, mul = 1;
		var trig, delayedSignals;
		trig = Impulse.ar(delayTime.reciprocal);
		delayedSignals = (PlayBuf.ar(1, bufnum, 1, 1, 0, 1.0)
			* feedbackpct) + in;
		RecordBuf.ar(delayedSignals, bufnum, 0, 1.0, 0.0, 1.0, 1.0, 1);
		^(delayedSignals * mul);
	}
}