/*
Generic hyperplane-based tree classifier, (c) 2009-2010 Dan Stowell
*/
PlaneTree : UGen {

*kr { |treebuf, in, gate=1|
	^this.multiNew('control', treebuf, gate, *in)
}

*categories {	^ #["UGens>Analysis"]	}

// a LANGUAGE-SIDE equivalent of the classification that the server-side ugen does.
// returns pathInteger
// (Don't use the "pathInt" arg, it's used for recursion.)
*classify { |point, treedata, pathInt=1|
	var node = treedata[pathInt - 1]; // note the minus one, it's because the indexing has to start at 1 not 0
	var result = sumF((point - node[..point.size-1]) * node[point.size..point.size*2-1]);
	var isleaf;

	if(result > 0){ // "left" branch
		pathInt = (pathInt << 1);
		isleaf = node[node.size - 2] > 0.0;
	}{ // "right" branch
		pathInt = (pathInt << 1) | 1;
		isleaf = node[node.size - 1] > 0.0;
	};

	^if(isleaf){
		pathInt
	}{
		this.classify(point, treedata, pathInt) // recurse
	}
}

} // end class
