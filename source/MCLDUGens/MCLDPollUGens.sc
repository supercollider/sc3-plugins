TextVU : UGen {
	*ar { arg trig=2, in, label, width=21, reset=0, ana;
		this.multiNewList(['audio', trig, in, label, width, reset, ana]);
		^in;
	}
	*kr { arg trig=2, in, label, width=21, reset=0, ana;
		this.multiNewList(['control', trig, in, label, width, reset, ana]);
		^in;
	}
	*new { arg trig=2, in, label, width=21, reset=0, ana;
		var rate = in.asArray.collect(_.rate).unbubble;
		this.multiNewList([rate, trig, in, label, width, reset, ana]);
		^in;
	}
	*new1 { arg rate, trig, in, label, width=21, reset=0, ana;
		var releaseTime;
		label = label ?? {  "UGen(%)".format(in.class) };
		label = label.asString.collectAs(_.ascii, Array);
		if(ana.isNil){ana = {|sig|
			releaseTime = if(trig.isNumber){1/trig}{5};
			if(rate === \audio){
				A2K.kr(Amplitude.ar(sig, releaseTime: releaseTime))
			}{
				Amplitude.kr(sig, releaseTime: releaseTime)
			}
		}};
		if(trig.isNumber) { trig = Impulse.kr(trig, 0) };
		^super.new.rate_(\control).addToSynth.init([trig, ana.value(in), width, reset, label.size] ++ label);
	}

	init { arg theInputs;
		// store the inputs as an array
		inputs = theInputs;
	}
}
