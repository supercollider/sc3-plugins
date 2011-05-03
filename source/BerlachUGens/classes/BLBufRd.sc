BLBufRd : UGen  {
	*ar { arg bufnum=0, phase=0, ratio=1;
		^this.multiNew('audio', bufnum, phase, ratio);
	}
	*kr { arg bufnum=0, phase=0, ratio=1;
		^this.multiNew('control', bufnum, phase, ratio);
	}
}
