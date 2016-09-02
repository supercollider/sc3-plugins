+ Matrix {

	// see also: AtkMatrix:writeToFile
	// set: FOA, HOA1, HOA2, etc
	// type: \encoder, \decoder, \xformer
	writeToFile { arg fileNameOrPath, set, type, note, attributeDictionary, overwrite=false;
		var atkMatrix;

		set ?? {Error("Unspecified set argument. Choose 'FOA', 'HOA1', 'HOA2', etc.").throw};

		atkMatrix = AtkMatrix.newFromMatrix(this);

		atkMatrix.prWriteToFile(fileNameOrPath, set, type, note, attributeDictionary, overwrite);
	}

}
