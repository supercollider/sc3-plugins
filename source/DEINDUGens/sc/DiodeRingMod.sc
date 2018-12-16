// author: Julian Parker & Till Bovermann
// license: GPL2+
// year: 2013

// without mul and add.
DiodeRingMod : UGen {
*ar { arg car = 0.0, mod = 0.0;
	^this.multiNew('audio', car, mod)
}

}