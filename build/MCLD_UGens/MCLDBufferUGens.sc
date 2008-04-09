Logger : UGen {
	
	*kr { arg inputArray, trig=0.0, bufnum=0, reset=0.0;
		// Note rearranged args - code expects buffer ref first, and input data last
		^this.multiNewList(['control', bufnum, trig, reset] ++ inputArray.asArray)
	}
}

ListTrig : UGen {
	
	*kr { arg bufnum=0, reset=0, offset=0, numframes;
		if(numframes.isNil, {numframes = BufFrames.kr(bufnum)});
		^this.multiNewList(['control', bufnum, reset, offset, numframes])
	}
}

ListTrig2 : UGen {
	
	*kr { arg bufnum=0, reset=0, numframes;
		if(numframes.isNil, {numframes = BufFrames.kr(bufnum)});
		^this.multiNewList(['control', bufnum, reset, numframes])
	}
}
