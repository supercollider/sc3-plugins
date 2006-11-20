// A series of BFormat UGens for SC3. UGens by Josh Parmenter 4-3-2004
// UGen plug-ins are based on James McCartney's Pan2B and DecodeB2 UGens.
// added interior localization, z-signal manipulation, and rotate, tilt
// and tumble transformations. Push and Dominate equations by Joe Anderson.
// B2Ster equations from 
// http://www.cyber.rdg.ac.uk/P.Sharkey/WWW/icdvrat/WWW96/Papers/keating.htm


BFEncode1 : Panner {
	
	*ar { arg in, azimuth=0, elevation=0, rho = 1, gain=1;
		^this.multiNew('audio', in, azimuth, elevation, rho, gain )
	}
	
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
}


BFEncode2 : Panner {
	
	*ar { arg in, point_x = 1, point_y = 1, elevation=0, gain=1;
		^this.multiNew('audio', in, point_x, point_y, elevation, gain )
	}

	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
} 

BFEncodeSter : Panner {

	*ar { arg l, r, spread = 0.5pi, azimuth=0, elevation=0, rho = 1, gain=1;
		^this.multiNew('audio', l, r, spread, azimuth, elevation, rho, gain )
	}
	
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}

}

BFDecode1 : Panner {
	
	*ar { arg w, x, y, z, azimuth = 0, elevation = 0, mul = 1, add = 0;
		^this.multiNew('audio', w, x, y, z, azimuth, elevation ).madd(mul, add);
	}
	
	*ar1 {arg w, x, y, z, azimuth = 0, elevation = 0, maxDist = 10, distance = 10, mul = 1, 
			add = 0;
		var dist, scaler;
		dist = ((maxDist - distance) / 345);
		scaler = 1/((distance/maxDist)**1.5);
		^DelayC.ar(this.multiNew('audio', w, x, y, z, azimuth, elevation ), dist, dist, 			scaler.reciprocal).madd(mul, add);
	}
	
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [OutputProxy(\audio,this, 0)];
		^channels
	}
// 	checkInputs { ^this.checkNInputs(4) }
}

BFManipulate : Panner {
	
	*ar { arg w, x, y, z, rotate = 0, tilt = 0, tumble = 0;
		^this.multiNew('audio', w, x, y, z, rotate, tilt, tumble);
	}
		
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
	
 	checkInputs { ^this.checkNInputs(4) }

} 

// Rotate tilt and tumble classes, built from Rotate2.  Allows w, x, y and z to be passed in, and 
// returns the new w, x, y, and z
Rotate : Panner {
	*ar {arg w, x, y, z, rotate;
		var xout, yout;
		#xout, yout = Rotate2.ar(x, y, rotate / 2pi);
		^[w, xout, yout, z];
		}
		
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
	
 	checkInputs { ^this.checkNInputs(4) }
}	 

Tilt : Panner {
	*ar {arg w, x, y, z, tilt;
		var xout, zout;
		#xout, zout = Rotate2.ar(x, z, tilt / 2pi);
		^[w, xout, y, zout];
		}
		
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
	
 	checkInputs { ^this.checkNInputs(4) }
}	

Tumble : Panner {
	*ar {arg w, x, y, z, tilt;
		var yout, zout;
		#yout, zout = Rotate2.ar(y, z, tilt / 2pi);
		^[w, x, yout, zout];
		}
		
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
	
 	checkInputs { ^this.checkNInputs(4) }
}	

A2B : Panner {
	
	*ar { arg a, b, c, d;
		^this.multiNew('audio', a, b, c, d);
	}
		
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
	
 	checkInputs { ^this.checkNInputs(4) }

}

B2A : Panner {
	
	*ar { arg w, x, y, z;
		^this.multiNew('audio', w, x, y, z);
	}
		
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
	
 	checkInputs { ^this.checkNInputs(4) }

}

B2Ster : Panner {
	*ar {arg w, x, y, mul = 1, add = 0;
		^this.multiNew('audio', w, x, y).madd(mul, add);
		}
		
	init {arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio, this, 0), OutputProxy(\audio, this, 1)];		^channels;
		}

 	checkInputs { ^this.checkNInputs(3) }

	}

//Hilbert : MultiOutUGen {
//	*ar {arg in, mul = 1, add = 0;
//		^this.multiNew('audio', in).madd(mul, add);
//		}
//	
//	init {arg ... theInputs;
//		inputs = theInputs;
//		^this.initOutputs(2, rate);
//		}
//	}
		
// takes w, x, and y from a BF sig, returns a 2 channel UHJ file

B2UHJ : Panner {
	*ar {arg w, x, y;
		^this.multiNew('audio', w, x, y);
		}
		
	init {arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio, this, 0), OutputProxy(\audio, this, 1)];		^channels;
		}

 	checkInputs { ^this.checkNInputs(3) }

	}

// takes the left signal (ls) and right signal (rs) of a UHJ signal, 
// and returns w, x, and y of a BF signal

UHJ2B : Panner {
	*ar {arg ls, rs;
		^this.multiNew('audio', ls, rs);
		}
		
	init {arg ... theInputs;
		inputs = theInputs;
		^channels = [ OutputProxy(\audio, this, 0), OutputProxy(\audio, this, 1),
			OutputProxy(\audio, this, 2)]
		}

 	checkInputs { ^this.checkNInputs(2) }

	}
		
//////////////////////////////////////////////////////////////////////////////////////////////
// Classes from SuperCollider 2 are in the file AmbisonicsSC2
//////////////////////////////////////////////////////////////////////////////////////////////



