//UGens by Nick Collins, (c) 8 August 2010, following research work by Nam, Valimaki, Abel and Smith released under GNU GPL as SuperCollider server extension plugins

BlitB3 : UGen {

	*ar { arg freq= 440.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq).madd(mul, add)
	}
}

BlitB3Saw : UGen {

	*ar { arg freq= 440.0, leak= 0.99, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, leak).madd(mul, add)
	}
}

BlitB3Square : UGen {

	*ar { arg freq= 440.0, leak= 0.99, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, leak).madd(mul, add)
	}
}


//no better than LFTri really! 
BlitB3Tri : UGen {

	*ar { arg freq= 440.0, leak= 0.99, leak2= 0.99,  mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, leak, leak2).madd(mul, add)
	}
}

DPW4Saw : UGen {

	*ar { arg freq= 440.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq).madd(mul, add)
	}
}

DPW3Tri : UGen {

	*ar { arg freq= 440.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq).madd(mul, add)
	}
}






