SwitchDelay : UGen {
	*ar { arg in, drylevel=1.0, wetlevel=1.0, delaytime=1.0, delayfactor=0.7, maxdelaytime=20.0, mul=1.0, add=0.0;
		^this.multiNew('audio', in, drylevel, wetlevel, delaytime, delayfactor, maxdelaytime).madd(mul, add)
	}
}
