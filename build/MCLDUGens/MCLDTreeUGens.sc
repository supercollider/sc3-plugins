/*
Generic hyperplane-based tree classifier, (c) 2009 Dan Stowell
*/
PlaneTree : UGen {

*kr { |treebuf, in, gate=1|
	^this.multiNew('control', treebuf, gate, *in)
}

*categories {	^ #["UGens>Analysis"]	}

// a LANGUAGE-SIDE equivalent of the classification that the server-side ugen does.
// returns two things: [index, pathInteger]
// (Don't use the "startAt"/"pathsofar" arg, it's used for recursion.)
*classify { |point, treedata, startAt=0, pathsofar=1|
	var kidIndex;
	var node = treedata[startAt];
	var result = sumF((point - node[..point.size-1]) * node[point.size..point.size*2-1]);
	^if(result > 0){ // "left" branch
		kidIndex = node[node.size - 3];
		pathsofar = (pathsofar << 1);
		if(node[node.size - 4] > 0){
			[kidIndex, pathsofar]
		}{
			this.classify(point, treedata, kidIndex, pathsofar)
		}
	}{ // "right" branch
		kidIndex = node[node.size - 1];
		pathsofar = (pathsofar << 1) | 1;
		if(node[node.size - 2] > 0){
			[kidIndex, pathsofar]
		}{
			this.classify(point, treedata, kidIndex, pathsofar)
		}
	}
}

} // end class
