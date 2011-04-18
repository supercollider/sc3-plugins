//redFrik
BitAnd : UGen {
	*ar {|a= 0, b= 1|
		^this.multiNew('audio', a, b)
	}
	*kr {|a= 0, b= 1|
		^this.multiNew('control', a, b)
	}
	*categories {^#["UGens>Maths"]}
}
BitOr : BitAnd {
}
BitXor : BitAnd {
}
BitNot : UGen {
	*ar {|a= 0|
		^this.multiNew('audio', a)
	}
	*kr {|a= 0|
		^this.multiNew('control', a)
	}
	*categories {^#["UGens>Maths"]}
}
BitLeftShift : UGen {
	*ar {|a= 0, shift= 0, bits= 8|
		^this.multiNew('audio', a, shift, bits)
	}
	*kr {|a= 0, shift= 0, bits= 8|
		^this.multiNew('control', a, shift, bits)
	}
	*categories {^#["UGens>Maths"]}
}
BitUnsignedRightShift : BitLeftShift {
}
BitRightShift : UGen {
	*ar {|a= 0, shift= 0|
		^this.multiNew('audio', a, shift)
	}
	*kr {|a= 0, shift= 0|
		^this.multiNew('control', a, shift)
	}
	*categories {^#["UGens>Maths"]}
}
