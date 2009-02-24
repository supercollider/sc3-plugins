Clipper4 : UGen {
	*ar {
		arg in, lo=(-0.8), hi=0.8;
		^this.multiNew('audio', in, lo, hi);
	}
}

Clipper8 : UGen {
	*ar {
		arg in, lo=(-0.8), hi=0.8;
		^this.multiNew('audio', in, lo, hi);
	}
}

Clipper32 : UGen {
	*ar {
		arg in, lo=(-0.8), hi=0.8;
		^this.multiNew('audio', in, lo, hi);
	}
}

SoftClipper4 : UGen {
	*ar {
		arg in;
		^this.multiNew('audio', in);
	}
}

SoftClipper8 : UGen {
	*ar {
		arg in;
		^this.multiNew('audio', in);
	}
}

SoftClipAmp : UGen {
	*ar {
		arg in , pregain = 1, mul=1, add=0;
		^(in*pregain).softclip.madd(mul, add);
	}
}


SoftClipAmp4 : UGen {
	*ar {
		arg in , pregain = 1, mul=1, add=0;
		^this.multiNew('audio', in, pregain).madd(mul, add);
	}
}


SoftClipAmp8 : UGen {
	*ar {
		arg in, pregain = 1, mul=1, add=0;
		^this.multiNew('audio', in, pregain).madd(mul, add);
	}
}

OSWrap4 : UGen {
	*ar {
		arg in, lo, hi;
		^this.multiNew('audio', in, lo, hi);
	}
}

OSWrap8 : UGen {
	*ar {
		arg in, lo, hi;
		^this.multiNew('audio', in, lo, hi);
	}
}

OSFold4 : UGen {
	*ar {
		arg in, lo, hi;
		^this.multiNew('audio', in, lo, hi);
	}
}

OSFold8 : UGen {
	*ar {
		arg in, lo, hi;
		^this.multiNew('audio', in, lo, hi);
	}
}

OSTrunc4 : UGen {
	*ar {
		arg in, quant=0.5;
		^this.multiNew('audio', in, quant);
	}
}

OSTrunc8 : UGen {
	*ar {
		arg in, quant=0.5;
		^this.multiNew('audio', in, quant);
	}
}

DriveNoise : UGen {
	*ar {
		arg in, amount=1, multi=5;
		^this.multiNew('audio', in, amount ,multi);
	}
}
