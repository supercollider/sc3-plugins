WarpIn {
	*ar {arg in, buffer = 0, warpFactor = 1, freqScale=1, windowSize=0.2, overlaps=10, 			windowRandRatio = 0.1, preLevel = 0, mul=0.1; 
		var out, trig, grainrate, start, pointer, record;
		record = RecordBuf.ar(in, buffer, preLevel: preLevel, loop: 0);
		out = Warp0.ar(buffer, warpFactor, freqScale, windowSize, overlaps, windowRandRatio,
			mul);
		^out;
	}
}

Warp0 {
	*ar {arg buffer = 0, warpFactor = 1, freqScale=1.0, windowSize=0.2, overlaps=10, 			windowRandRatio = 0.1, mul=0.1; 
		var pointer;
		pointer = LFSaw.ar( warpFactor * BufDur.kr(buffer).reciprocal, 1, 0.5, 0.5 );
		^Warp1.ar(buffer, pointer, freqScale, windowSize, overlaps, windowRandRatio, mul);
		
	}
}