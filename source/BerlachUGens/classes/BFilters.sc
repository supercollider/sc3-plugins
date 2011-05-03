LPF1 : UGen  {
	*ar { arg  in, freq=1000;
		^this.multiNew('audio', in, freq);
	}
	*kr { arg in, freq=1000;
		^this.multiNew('control', in, freq);
	}
}

LPFVS6 : UGen  {
	*ar { arg  in, freq=1000, slope=0.5;
		^this.multiNew('audio', in, freq, slope);
	}
	*kr { arg in, freq=1000, slope=0.5;
		^this.multiNew('control', in, freq, slope);
	}
}


LPF18 : UGen {
	*ar {
		arg in, freq=100, res=1, dist=0.4;
		^this.multiNew('audio', in, freq, res, dist);
	}
}
