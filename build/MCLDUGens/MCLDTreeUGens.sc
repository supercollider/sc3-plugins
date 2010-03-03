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

// Distance from one node to another, both represented in the path-integer format returned by *classify
*treedist { |path1, path2|
		var bitpos1=0, bitpos2=0, res;
		// First we find the MSB 1 representing the root node in each one
		while{(path1 >> bitpos1) != 0}{
			bitpos1 = bitpos1 + 1
		};
		while{(path2 >> bitpos2) != 0}{
			bitpos2 = bitpos2 + 1
		};
		
		// Then we descend until any of the bits are different
		while{(bitpos1 != -1) and: {bitpos2 != -1} and: 
					{((path1 >> bitpos1) & 1) == ((path2 >> bitpos2) & 1)}}{
			bitpos1 = bitpos1 - 1;
			bitpos2 = bitpos2 - 1;
		};

		// The actual descended positions we intend are one more than achieved by the iteration
		bitpos1 = bitpos1 + 1;
		bitpos2 = bitpos2 + 1;
		//postln("Descended positions: % %".format(bitpos1, bitpos2));
		
		// Then we sum the number of remaining bits, 
		// i.e. the number of steps needed to go up the tree and back down again
		res = bitpos1 + bitpos2;
		//"d(%,%)=%".format(path1.asBinaryDigits(12).join, path2.asBinaryDigits(12).join, res).postln;
		^res
}


} // end class
