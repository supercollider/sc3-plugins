// author: Julian Parker & Till Bovermann
// license: GPL2+
// year: 2013

// without mul and add.
ComplexRes : UGen {
*ar { arg in = 0.0, freq = 100.0, decay = 0.2;
	^this.multiNew('audio', in, freq, decay)
}

}