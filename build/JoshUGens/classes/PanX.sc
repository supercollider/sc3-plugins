PanX : Panner {
	
	*ar { arg numChans, in, pos = 0.0, level = 1.0, width = 2.0;
		^this.multiNew('audio', numChans, in, pos, level, width )
	}
	*kr { arg numChans, in, pos = 0.0, level = 1.0, width = 2.0;
		^this.multiNew('control', numChans, in, pos, level, width )
	}
	init { arg numChans ... theInputs;
		inputs = theInputs;		
		channels = Array.fill(numChans, { arg i; OutputProxy(rate,this, i) });
		^channels
	}
}