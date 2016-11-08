+ Array {

	// set: FOA, HOA1, HOA2, etc.
	// type: \encoder, \decoder, \xformer
	// NOTE: set and type aren't currently enforced, but it's a
	//       good idea to provide it for writing to file
	asAtkMatrix { arg set, type;
		var mtx;
		mtx = Matrix.with(this.asArray);
		^AtkMatrix.newFromMatrix(mtx, set, type);
	}

}
