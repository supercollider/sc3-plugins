Getenv : UGen {
	*new { |key, defaultval=0|
		^this.multiNewList(['scalar', key, defaultval]);
	}
	*new1 { | rate, key, defaultval |
		key = key ? \SHLVL; // SHLVL is numeric, at least...
		
		key = key.asString.collectAs(_.ascii, Array);
		^super.new.rate_(rate).addToSynth.init([key.size, defaultval] ++ key);
	}
		
 	init { arg theInputs;
 		// store the inputs as an array
 		inputs = theInputs;
 	}
 	*categories {	^ #["UGens>InfoUGens"]	}
}
