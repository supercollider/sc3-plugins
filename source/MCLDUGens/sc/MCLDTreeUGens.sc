/*
hyperplane-based tree classifiers, (c) 2009-2011 Dan Stowell
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


} // end PlaneTree class


/**
* See the KDTree quark to actually construct a kd-tree data structure.
*  This UGen class converts the resulting data structure to a Buffer,
*  and then the UGen does nearest-neighbour search on the server side.
*  The ugen returns a sorted array of the 'num' closest matches,
*  in the format [index0, distsq0, index1, distsq1, index2, distsq2, ...].
*  If you want the co-ordinates back, you can easily read them back out of the treebuf.
*/
NearestN : MultiOutUGen {

*kr { |treebuf, in, gate=1, num=1|
	^this.multiNew('control', treebuf, gate, num, *in)
}
init { |... theInputs|
	inputs = theInputs;
	^this.initOutputs(theInputs[2] * 3 /* <-- 'num' times three */, rate);
}
*categories {	^ #["UGens>Analysis"]	}

// Give it a KDTree object, you get an array suitable for loading to a Buffer.
// Note that you have to do .flat on it before .sending to the server!
*makeBufferData { |tree|
	// We use the format of uniqueid to know where each item should be slotted.
	// Since root's uniqueid is 1, the zero'th slot is always wasted, but that's the price of efficiency and understandability.
	// The root's kids are [1 << 1, 1 << 1 | 1] == [2, 3], and so on.
	/* format of each entry:
	 * 0 or 1: leftIsLeaf
	 * 0 or 1: rightIsLeaf
	 * k floats: location
	 * 1 float: label
	*/
	var numChannels = tree.location.size + 3;
	var array = {{0.0}.dup(numChannels)}.dup(tree.highestUniqueId+1);
	tree.do{|node|
		array[node.uniqueid] = [
			if(node.leftChild .isNil){1}{0},
			if(node.rightChild.isNil){1}{0}
			] ++ node.location ++ (node.label ? 0.0).asFloat;
	};
	^array
}

} // end NearestN class

