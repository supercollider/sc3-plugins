//BatUGens, Batuhan Bozkurt

Coyote : UGen {
	*kr { arg in = 0.0, trackFall = 0.2, slowLag = 0.2, fastLag = 0.01, fastMul = 0.5, thresh = 0.05, minDur = 0.1;
		^this.multiNew('control', in, trackFall, slowLag, fastLag, fastMul, thresh, minDur)
	}
}

TrigAvg : UGen {
	*kr { arg in = 0.0, trig = 0;
		^this.multiNew('control', in, trig)
	}
}

WAmp : UGen {
	*kr { arg in = 0.0, winSize = 0.1;
		^this.multiNew('control', in, winSize)
		}
}

MarkovSynth : UGen {
	*ar { arg in = 0.0, isRecording = 1, waitTime = 2, tableSize = 10;
		^this.multiNew('audio', in, isRecording, waitTime, tableSize)
		}
}

FrameCompare : UGen {
	*kr { arg buffer1, buffer2, wAmount = 0.5;
		^this.multiNew('control', buffer1, buffer2, wAmount);
	}
}

NeedleRect : UGen {
	*ar { arg rate=1.0, imgWidth=100, imgHeight=100, rectX=0, rectY=0, rectW=100, rectH=100;
		^this.multiNew('audio', rate, imgWidth, imgHeight, rectX, rectY, rectW, rectH);
	}
}

SkipNeedle : UGen {
	*ar { arg range = 44100, rate = 10, offset = 0;
		^this.multiNew('audio', range, rate, offset);
	}
}