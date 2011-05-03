+ Integer {

	asPaddedString { arg size = 2, padChar = "0", padSide = \left;
		var string, stringSize;

		string = this.asString;
		stringSize = string.size;

		if(padSide == \right, {
			(size - stringSize).do({
				string = string ++ padChar;
			});
		},{
			(size - stringSize).do({
				string = padChar ++ string;
			});
		});
		^string
	}
}

+SequenceableCollection {
	asPaddedString { ^this.performUnaryOp('asPaddedString') }
}