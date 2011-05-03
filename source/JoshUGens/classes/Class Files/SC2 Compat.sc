//AudioIn {
//	*ar { arg channel = 1, mul=1.0, add=0.0;
//		var chanOffset, consecutiveOrSingleton;
//		channel = channel.asArray;
//		// check to see if channels array is consecutive
//		consecutiveOrSingleton = (channel.size == 1) or:
//			{channel.every({arg item, i; ((i==0) or: {item == (channel.at(i-1)+1)})})};
//		chanOffset = NumOutputBuses.ir - 1;
//		^consecutiveOrSingleton.if({
//			//"more efficient, yay".postln;
//			In.ar(chanOffset + channel.minItem, channel.size)
//		},{
//			//"yet compatible, yay".postln;
//			In.ar(chanOffset + channel)
//		}).madd(mul,add)
//	}
//}

TapN {
	*ar {arg bufnum, numChannels=1, delaytime = 0.2, mul = 1.0, add = 0.0;
		^PlayBuf.ar( numChannels, bufnum, 1.0, 1.0, delaytime*BufSampleRate.kr(bufnum) * -1, 		1.0).madd(mul,add)
	}
}

DelayWr {
	*ar { arg bufnum, in = 0.0;
		^RecordBuf.ar( in, bufnum )
	}
}
