ProximityB : Panner { 
	*ar { arg w, x, y, z, distance, mul = 1, add = 0;
		^this.multiNew('audio', w, x, y, z, distance).madd(mul, add);
	}
		
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
	
 	checkInputs { ^this.checkNInputs(4) }

}

DistanceB : Panner { 
	*ar { arg w, x, y, z, distance, mul = 1, add = 0;
		^this.multiNew('audio', w, x, y, z, distance).madd(mul, add);
	}
		
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
	
 	checkInputs { ^this.checkNInputs(4) }

}

// canonical dominance - Y, Z -> 0
// dom value should be between -0.5 and 1?

Dominate : Panner {	
	*ar { arg w, x, y, z, dom;
		^this.multiNew('audio', w, x, y, z, dom);
	}
		
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
	
 	checkInputs { ^this.checkNInputs(4) }

}


// canonical push / pull of a soundfield
// 0 is normal, 1 pushes entire soundfield to the front, -1 to the back
Push : Panner { 
	*ar { arg w, x, y, z, push;
		^this.multiNew('audio', w, x, y, z, push);
	}
		
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
	
 	checkInputs { ^this.checkNInputs(4) }

}
