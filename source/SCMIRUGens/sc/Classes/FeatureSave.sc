FeatureSave : UGen {

	*kr {arg features, trig; 
		 
		 
		^this.multiNewList(['control', features.size.max(1), trig] ++ features.asArray)
	}	
	
}

