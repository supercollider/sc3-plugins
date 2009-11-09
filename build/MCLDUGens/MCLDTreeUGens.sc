/*
Generic hyperplane-based tree classifier, (c) 2009 Dan Stowell
*/
PlaneTree : UGen {

*kr { |treebuf, in, gate=1|
	^this.multiNew('control', treebuf, gate, *in)
}

*categories {	^ #["UGens>Analysis"]	}

// a LANGUAGE-SIDE equivalent of the classification that the server-side ugen does.
// (Don't use the "startAt" arg, it's used for recursion.)
*classify { |point, treedata, startAt=0|
	var node = treedata[startAt];
	var result = sumF((point - node[..point.size-1]) * node[point.size..point.size*2-1]);
	^if(result > 0){ // "left" branch
		if(node[node.size - 4] > 0){
			node[node.size - 3]
		}{
			this.classify(point, treedata, node[node.size - 3])
		}
	}{ // "right" branch
		if(node[node.size - 2] > 0){
			node[node.size - 1]
		}{
			this.classify(point, treedata, node[node.size - 1])
		}
	}
}

} // end class
