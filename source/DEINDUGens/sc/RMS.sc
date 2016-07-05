RMS : UGen  {
	*ar { arg  in, lpFreq=10;
		^this.multiNew('audio', in, lpFreq);
	}
	*kr { arg in, lpFreq=10;
		^this.multiNew('control', in, lpFreq);
	}
}
