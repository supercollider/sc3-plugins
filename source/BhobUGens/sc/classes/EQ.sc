BhobLoShelf {
	*ar {|in, freq, amp|
		var wc, a0, allpass;
		wc=pi * freq * SampleDur.ir;
		a0=(1 - wc)/(1 + wc);
		allpass=FOS.ar(in, a0.neg, 1, a0, -1);
		^(0.5 * (in + allpass + (amp * (in-allpass))))
	}
}

BhobHiShelf {
	*ar {|in, freq, amp|
		var wc, a0, allpass;
		wc=pi * freq * SampleDur.ir;
		a0=(1 - wc)/(1 + wc);
		allpass=FOS.ar(in, a0.neg, 1, a0, 1);
		^(0.5 * (in + allpass + (amp * (in-allpass))))
	}
}

BhobTone {
	*ar {|in, tone|
		^Mix([HiShelf.ar(in, 10000, tone), LoShelf.ar(in, 100, tone.reciprocal)])
	}
}