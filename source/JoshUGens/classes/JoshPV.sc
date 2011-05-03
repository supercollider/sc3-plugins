// these will always operate on the current FFT buffer.
// HOWEVER, there will be numFrames of no transformations while the data is stored up

PV_NoiseSynthP : PV_ChainUGen
{
	*new {arg buffer, threshold = 0.1, numFrames = 2, initflag = 0;
		^this.multiNew('control', buffer, threshold, numFrames, initflag);
	}
}

PV_PartialSynthP : PV_NoiseSynthP {}

PV_NoiseSynthF : PV_NoiseSynthP {}

PV_PartialSynthF : PV_NoiseSynthP {}

PV_MagMap : PV_ChainUGen
{
	*new {arg buffer, mapbuf;
		^this.multiNew('control', buffer, mapbuf);
		}
}

PV_MaxMagN : PV_ChainUGen
{
	*new {arg buffer, numbins;
		^this.multiNew('control', buffer, numbins);
		}
}

PV_MinMagN : PV_MaxMagN { }

PV_MagBuffer : PV_ChainUGen
{
	*new {arg buffer, databuffer;
		^this.multiNew('control', buffer, databuffer)
		}

}

PV_FreqBuffer : PV_MagBuffer { }

PV_OddBin : PV_ChainUGen
{
	*new {arg buffer;
		^this.multiNew('control', buffer);
		}
}

PV_EvenBin : PV_OddBin {}

PV_BinFilter : PV_ChainUGen
{
	*new {arg buffer, start = 0, end = 0;
		^this.multiNew('control', buffer, start, end);
	}
}

PV_Invert : PV_ChainUGen
{
	*new {arg buffer;
		^this.multiNew('control', buffer);
		}
}

PV_BinDelay : PV_ChainUGen
{
	*new {arg buffer, maxdelay, delaybuf, fbbuf, hop = 0.5;
		^this.multiNew('control', buffer, maxdelay, delaybuf, fbbuf, hop);
		}
}

PV_Freeze : PV_ChainUGen
{
	*new { arg buffer, freeze = 0.0;
		^this.multiNew('control', buffer, freeze)
	}
}

PV_RecordBuf : PV_ChainUGen
{
	*new { arg buffer, recbuf, offset = 0.0, run = 0.0, loop = 0.0, hop = 0.5, wintype = 0;
		^this.multiNew('control', buffer, recbuf, offset, run, loop, hop, wintype);
	}
}

// rate relates to expansion or compression of speed. 1 is normal, -1 is backwards, 0.5 doubles
// the duration, 2.0, halves it.

PV_PlayBuf : PV_ChainUGen
{
	*new { arg buffer, playbuf, rate = 1.0, offset = 0.0, loop = 0.0;
		^this.multiNew('control', buffer, playbuf, rate, offset, loop);
	}
}

PV_BinPlayBuf : PV_ChainUGen
{
	*new { arg buffer, playbuf, rate = 1.0, offset = 0.0, binStart = 0, binSkip = 1,
			numBins = 1, loop = 0.0, clear = 0;
		^this.multiNew('control', buffer, playbuf, rate, offset, loop,
			binStart, binSkip, numBins, clear);
	}
}

// point is a value between 0.0 (beginning) and 1.0 (end)
PV_BufRd : PV_ChainUGen
{
	*new { arg buffer, playbuf, point = 1.0;
		^this.multiNew('control', buffer, playbuf, point);
	}
}

PV_BinBufRd : PV_ChainUGen
{
	*new { arg buffer, playbuf, point = 1.0, binStart = 0, binSkip = 1, numBins = 1, clear = 0;
		^this.multiNew('control', buffer, playbuf, point, binStart, binSkip, numBins, clear);
	}
}

PV_SpectralMap : PV_ChainUGen
{
	*new {arg buffer, specBuffer, floor = 0.0, freeze = 0.0, mode = 0.0, norm = 0.0, window = 0.0;
		^this.multiNew('control', buffer, specBuffer, floor, freeze, mode, norm, window)
		}
}

PV_SpectralEnhance : PV_ChainUGen
{
	*new {arg buffer, numPartials = 8, ratio = 2, strength = 0.1;
		^this.multiNew('control', buffer, numPartials, ratio, strength);
		}
}

BinData : MultiOutUGen
{
	*ar {arg buffer, bin, overlaps = 0.5;
		^this.multiNew('audio', buffer, bin, overlaps);
		}

	*kr {arg buffer, bin, overlaps = 0.5;
		^this.multiNew('control', buffer, bin, overlaps);
		}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}

// very poorly implemented for the time being - do not use
PV_PitchShift : PV_ChainUGen {
	*new {arg buffer, ratio;
		^this.multiNew('control', buffer, ratio);
		}
}

+ SimpleNumber {
	calcPVRecSize {arg frameSize, hop, sampleRate;
		var rawsize, tmp;
		sampleRate = sampleRate ?? {Server.default.sampleRate};
		rawsize = (((this * sampleRate) / (frameSize)).ceil * frameSize);
		^rawsize * hop.reciprocal + 3;
		}
	}


/* these aren't really working... hmmm ... maybe make some UGens that output Freq and Mag data
but doesn't sort it... */
/*
PV_MaxSortF : UGen
{
	*new {arg buffer, databuffer;
		^this.multiNew('control', buffer, databuffer);
		}

}

PV_MaxSortM : PV_MaxSortF {}

*/
