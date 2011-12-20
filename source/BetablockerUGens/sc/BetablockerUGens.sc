DetaBlockerBuf : DUGen {
	*new { arg bufnum = 0, startpoint = 0;
		^this.multiNew('demand', bufnum, startpoint)
	}
}

BBlockerBuf : MultiOutUGen {
	*ar { arg  freq, bufnum = 0, startpoint = 0;
 		//arg freq = 440.0, iphase = 0.0;
		^this.multiNew('audio', bufnum, freq, startpoint)
    }

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(9, rate);
	}
}


// without mul and add.
/*
Betablocker : UGen {
        *ar {
	 		//arg freq = 440.0, iphase = 0.0;
            //^this.multiNew('audio', freq, iphase)
			^this.multiNew('audio')
        }
        *kr { 
			// arg freq = 440.0, iphase = 0.0;
            //^this.multiNew('control', freq, iphase)
			^this.multiNew('control')
        }
}
*/

/*
DetaBlocker01 : DUGen {
	*new { arg length = inf;
		^this.multiNew('demand', length)
	}
}
*/


