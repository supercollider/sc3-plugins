GlitchRHPF : Filter {

	*ar { arg in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq).madd(mul, add)
	}
	*kr { arg in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, freq, rq).madd(mul, add)
	}
}

GlitchHPF : Filter {

	*ar { arg in = 0.0, freq = 440.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq).madd(mul, add)
	}
	*kr { arg in = 0.0, freq = 440.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, freq).madd(mul, add)
	}
}

GlitchBPF : Filter {

	*ar { arg in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq).madd(mul, add)
	}
	*kr { arg in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, freq, rq).madd(mul, add)
	}
}

GlitchBRF : GlitchBPF {}
