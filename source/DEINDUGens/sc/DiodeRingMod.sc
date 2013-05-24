// without mul and add.
DiodeRingMod : UGen {
*ar { arg car = 0.0, mod = 0.0;
	^this.multiNew('audio', car, mod)
}

}