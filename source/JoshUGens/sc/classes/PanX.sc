PanX : MultiOutUGen {

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

PanX2D : MultiOutUGen {

	*ar { arg numChansX, numChansY, in, posX = 0.0, posY = 0.0, level = 1.0, widthX = 2.0, widthY=2.0;
		^PanX.ar( numChansX,
			PanX.ar( numChansY, in, posY, level, widthY )
			, posX, level, widthX
		);
	}

	*kr { arg numChansX, numChansY, in, posX = 0.0, posY = 0.0, level = 1.0, widthX = 2.0, widthY=2.0;
		^PanX.kr( numChansX,
			PanX.kr( numChansY, in, posY, level, widthY )
			, posX, level, widthX
		);
	}

}
