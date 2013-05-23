// without mul and add.
DNoiseRing : DUGen {
	*new { arg change = 0.5, chance = 0.5, shift = 1, numBits = 8, resetval = 0;
		^this.multiNew('demand', change, chance, shift, numBits, resetval)
	}
}