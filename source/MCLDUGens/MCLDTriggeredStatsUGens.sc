MeanTriggered : Filter {

	*ar { arg in = 0.0, trig=0.0, length = 10, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, trig, length).madd(mul, add)
	}
	*kr { arg in = 0.0, trig=0.0, length = 10, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, trig, length).madd(mul, add)
	}
	*categories {	^ #["UGens>Analysis", "UGens>Maths"]	}
}

MedianTriggered : Filter {

	*ar { arg in = 0.0, trig=0.0, length = 10, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, trig, length).madd(mul, add)
	}
	*kr { arg in = 0.0, trig=0.0, length = 10, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, trig, length).madd(mul, add)
	}
	*categories {	^ #["UGens>Analysis", "UGens>Maths"]	}
}
