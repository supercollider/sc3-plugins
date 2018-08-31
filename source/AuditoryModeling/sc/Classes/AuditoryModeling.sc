Meddis : UGen {	
	*ar { 
		arg input, mul=1.0, add=0.0;
		^this.multiNew('audio', input).madd(mul, add)
	}
	*kr {
		arg input, mul=1.0, add=0.0;
		^this.multiNew('control', input).madd(mul, add)
	}
}


HairCell : UGen {	
	*ar { 
		arg input, spontaneousrate=0.0, boostrate=200.0, restorerate=1000.0, loss=0.99, mul=1.0, add=0.0;
		^this.multiNew('audio', input,  spontaneousrate, boostrate, restorerate, loss).madd(mul, add)
	}
	*kr {
		arg input, spontaneousrate=0.0, boostrate=200.0, restorerate=1000.0, loss=0.99, mul=1.0, add=0.0;
		^this.multiNew('control', input,  spontaneousrate, boostrate, restorerate, loss).madd(mul, add)
	}
}

Gammatone : UGen {	
	*ar { 
		arg input, centrefrequency=440.0, bandwidth= 200.0, mul=1.0, add=0.0;
		^this.multiNew('audio', input, centrefrequency, bandwidth).madd(mul, add)
	}

}