/*
	Copyright the ATK Community and Joseph Anderson, 2011-2016
		J Anderson	j.anderson[at]ambisonictoolkit.net


	This file is part of SuperCollider3 version of the Ambisonic Toolkit (ATK).

	The SuperCollider3 version of the Ambisonic Toolkit (ATK) is free software:
	you can redistribute it and/or modify it under the terms of the GNU General
	Public License as published by the Free Software Foundation, either version 3
	of the License, or (at your option) any later version.

	The SuperCollider3 version of the Ambisonic Toolkit (ATK) is distributed in
	the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
	the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with the
	SuperCollider3 version of the Ambisonic Toolkit (ATK). If not, see
	<http://www.gnu.org/licenses/>.
*/


//---------------------------------------------------------------------
//	The Ambisonic Toolkit (ATK) is a soundfield kernel support library.
//
// 	Class: FoaSpeakerMatrix
// 	Class: FoaDecoderMatrix
// 	Class: FoaEncoderMatrix
// 	Class: FoaDecoderKernel
// 	Class: FoaEncoderKernel
//
//	The Ambisonic Toolkit (ATK) is intended to bring together a number of tools and
//	methods for working with Ambisonic surround sound. The intention is for the toolset
//	to be both ergonomic and comprehensive, providing both classic and novel algorithms
//	to creatively manipulate and synthesise complex Ambisonic soundfields.
//
//	The tools are framed for the user to think in terms of the soundfield kernel. By
//	this, it is meant the ATK addresses the holistic problem of creatively controlling a
//	complete soundfield, allowing and encouraging the composer to think beyond the placement
//	of sounds in a sound-space and instead attend to the impression and image of a soundfield.
//	This approach takes advantage of the model the Ambisonic technology presents, and is
//	viewed to be the idiomatic mode for working with the Ambisonic technique.
//
//
//	We hope you enjoy the ATK!
//
//	For more information visit http://ambisonictoolkit.net/ or
//	email info[at]ambisonictoolkit.net
//
//---------------------------------------------------------------------


//------------------------------------------------------------------------
// Third Party Notices
//------------------------------------------------------------------------
//
//-----------------------------------------------------------------------
// Support for Gerzon's Diametric Decoder Theorem (DDT) decoding algorithm is derived
// from Aaron Heller's Octave code available at: http://www.ai.sri.com/ajh/ambisonics/
//
// Benjamin, et al., "Localization in Horizontal-Only Ambisonic Systems"
// Preprint from AES-121, 10/2006, San Francisco
//
// Implementation in the SuperCollider3 version of the ATK is by
// Joseph Anderson <j.anderson[at]ambisonictoolkit.net>
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
// Irregular array decoding coefficients (5.0, 7.0) are kindly provided by
// Bruce Wiggins: http://www.brucewiggins.co.uk/
//
// B. Wiggins, "An Investigation into the Real-time Manipulation and Control of
// Three-dimensional Sound Fields," PhD Thesis, University of Derby, Derby, 2004.
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
// matrix decoders

//   Heller's DDT (helper function)
FoaSpeakerMatrix {
	var <positions, <k, m, n;

	*new { arg directions, k;
		var positions;

		switch (directions.rank,					// 2D or 3D?
			1, { positions = Matrix.with(			// 2D
					directions.collect({ arg item;
						Polar.new(1, item).asPoint.asArray
					})
				)
			},
			2, { positions = Matrix.with(			// 3D
					directions.collect({ arg item;
						Spherical.new(1, item.at(0), item.at(1)).asCartesian.asArray
					})
				)
			}
		);

		^super.newCopyArgs(positions, k).initDiametric;
	}

	*newPositions { arg positions, k;
		^super.newCopyArgs(positions, k).initDiametric;
	}

	initDiametric {

    		// n = number of output channel (speaker) pairs
    		// m = number of dimensions,
    		//        2=horizontal, 3=periphonic
		m = this.positions.cols;
		n = this.positions.rows;
	}

	dim { ^m }

	numChannels { ^n * 2 }

	matrix {
		var s, directions, pos, dir;

	    	// scatter matrix accumulator
	    	s = Matrix.newClear(m, m);

		// output channel (speaker) directions matrix
        	// NOTE: this isn't the user supplied directions arg
	    	directions = Matrix.newClear(m, n);

		n.do({ arg i;

			// allow entry of positions as
	    		// transpose for convenience
	    		// e.g., output channel (speaker) positions are now in columns
	    		// rather than rows, then
	        	// get the i'th output channel (speaker) position
	        	// e.g., select the i'th column
        		pos = positions.flop.getCol(i);

        		// normalize to get direction cosines
        		dir = pos /  pos.squared.sum.sqrt;

        		// form scatter matrix and accumulate
        		s = s + Matrix.with(dir * dir.flop);

        		// form matrix of output channel (speaker) directions
        		directions.putCol(i, dir)

			});

		// return resulting matrix
	 	^sqrt(1/2) * n * k * ( s.inverse * directions);
	}

	printOn { arg stream;
		stream << this.class.name << "(" <<* [this.dim, this.numChannels] <<")";
	}
}

AtkMatrix {
	// copyArgs
	var <kind, <set = 'FOA', <type; // NOTE: 'set' default will change with addition of HOA

	var <matrix;
	var <filePath;		// matrices from files only
	var <fileParse;		// data parsed from YAML file
	var <op = 'matrix';

	// most typically called by subclass
	// *new { |mtxKind, set, mtxType|
	*new { |kind, set, type|
		^super.newCopyArgs(kind).init(set, type)
	}

	init { |argSet, argType|
		if (argSet.notNil) {
			set = argSet
		} { // detect from class
			if (this.class.asString.keep(3) == "Foa") {
				set = 'FOA';
			}
		};
		if (argType.notNil) {
			type = argType
		} { // detect from class
			type = switch( this.class,
				FoaEncoderMatrix, {'encoder'},
				FoaEncoderKernel, {'encoder'},
				FoaDecoderMatrix, {'decoder'},
				FoaDecoderKernel, {'decoder'},
				FoaXformerMatrix, {'xformer'}
			);
		};
	}

	// used when writing a Matrix to file:
	// need to convert to AtkMatrix first
	*newFromMatrix { |aMatrix, set, type|
		^super.newCopyArgs('fromMatrix').initFromMatrix(aMatrix, set, type)
	}

	initFromMatrix { |aMatrix, argSet, argType|
		this.init(argSet, argType);
		matrix = aMatrix;
	}

	initFromFile { arg filePathOrName, mtxType, searchExtensions=false;
		var resolvedPathName = Atk.resolveMtxPath(filePathOrName, mtxType, searchExtensions);

		// instance var
		filePath = resolvedPathName.fullPath;

		case
		{ resolvedPathName.extension == "txt"} {
			if (resolvedPathName.fileName.contains(".mosl")) {
				// .mosl.txt file: expected to be matrix only,
				// single values on each line, by rows
				matrix = Matrix.with( this.prParseMOSL(resolvedPathName) );
			} {
				// .txt file: expected to be matrix only, cols separated by spaces,
				// rows by newlines
				matrix = Matrix.with( FileReader.read(filePath).asFloat );
			};
			kind = resolvedPathName.fileName.asSymbol; // kind defaults to filename
		}
		{ resolvedPathName.extension == "yml"} {
			var dict = filePath.parseYAMLFile;
			fileParse = IdentityDictionary(know: true);

			// replace String keys with Symbol keys, make "knowable"
			dict.keysValuesDo{
				|k,v|
				fileParse.put( k.asSymbol,
					if (v=="nil", {nil},{v}) // so .info parsing doesn't see nil as array
				)
			};

			if (fileParse[\type].isNil) {
				"Matrix 'type' is undefined in the .yml file: cannot confirm the type matches the loaded object (encoder/decoder/xformer)".warn
			} {
				if (fileParse[\type].asSymbol != mtxType.asSymbol) {
					Error(
						format(
							"Matrix 'type' defined in the .yml file (%) doesn't match the type of matrix you're trying to load (%)",
							fileParse[\type], mtxType
						).throw
					)
				}
			};

			matrix = Matrix.with(fileParse.matrix.asFloat);
			kind = fileParse.kind ?? resolvedPathName.fileNameWithoutExtension.asSymbol;
		}
		{ Error("Unsupported file extension.").throw };
	}


	// post readable matrix information
	info {
		var attributes;
		// gather attributes in order of posting
		attributes = [ \kind, \dirInputs, \dirOutputs, \dim, \matrix ];
		if (this.isKindOf(FoaDecoderMatrix)) { attributes = attributes ++ [\shelfK, \shelfFreq] };
		filePath !? { attributes = attributes ++ [\fileName, \filePath] };

		// other non-standard metadata provided in yml file
		fileParse !? {
			fileParse.keys.do{|key|
				attributes.includes(key.asSymbol).not.if{
					attributes = attributes ++ key.asSymbol;
				}
			}
		};

		if (attributes.includes(\type)) {
			// bump 'type' to the top of the post
			attributes.remove(\type); attributes.addFirst(\type);
		};

		postf(":: % Info ::\n", this.class);

		attributes.do{ |attribute|
			var value;
			value = this.tryPerform(attribute);
			if (value.isNil and: fileParse.notNil) {
				value = fileParse[attribute] // this can still return nil
			};

			if (value.isKindOf(Array)) {
				value = value.asArray; // cast the Matrix to array for posting
				if (value.rank > 1) {
					postf("\n% : [\n", attribute);
					value.do{ |elem| postf("\t%\n", elem) };
					"  ]".postln;
				} {
					postf("\n% : \n\t%\n", attribute, value);
				}
			} {
				postf("\n% : %\n", attribute, value);
			};
		};
	}

	// For subclasses of AtkMatrix
	writeToFile { arg fileNameOrPath, note, attributeDictionary, overwrite=false;
		this.prWriteToFile(fileNameOrPath, this.set, this.type, note, attributeDictionary, overwrite);
	}

	// argSet: FOA, HOA1, HOA2, etc
	// argType: \encoder, \decoder, \xformer
	prWriteToFile { arg fileNameOrPath, argSet, argType, note, attributeDictionary, overwrite=false;
		var pn, writer, ext;

		pn = PathName(fileNameOrPath);

		if (PathName(pn.parentPath).isFolder.not) { // check for an enclosing folder
			// ... no enclosing folder found so assumed
			// to be relative to extensions/matrices/'type' directory

			Atk.checkSet(argSet);

			// This is only needed for relative file paths in user-matrices directory
			['encoder', 'decoder', 'xformer'].includes(argType).not.if{
				Error("'type' argument must be 'encoder', 'decoder', or 'xformer'").throw; ^this
			};

			case
			{ pn.colonIndices.size == 0} {
				// only filename provided, write to dir matching 'type'
				pn = Atk.getMatrixExtensionSubPath(argSet, argType) +/+ pn;

			} { pn.colonIndices.size > 0} {
				// relative path given, look for it
				var mtxPath, relPath;
				mtxPath = Atk.getMatrixExtensionSubPath(argSet, argType);
				relPath = (mtxPath +/+ PathName(pn.parentPath));
				if (relPath.isFolder) {
					// valid relative path confirmed
					pn = mtxPath +/+ pn;
				} {
					Error(
						format(
							"Specified relative folder path was not found in %\n",
							relPath.fullPath
						)
					).throw;
					^this
				}
			};
		}; // otherwise, provided path is absolute

		ext = pn.extension;
		if (ext == "") {pn = pn +/+ PathName(".yml")};

		overwrite.not.if{
			pn.isFile.if{
				Error(format(
					"File already exists:\n\t%\nChoose another name or location, or set overwrite:true",
					pn.fullPath
			)).throw; ^this}
		};

		case
		{ext == "txt"} {
			if (pn.fileName.contains(".mosl")) {
				this.prWriteMatrixToMOSL(pn)
			} {
				this.prWriteMatrixToTXT(pn)
			}
		}
		{ext == "yml"} {this.prWriteMatrixToYML(pn, argSet, argType, note, attributeDictionary)}
		{	// catch all
			Error( format( "%%",
				"Invalid file extension: provide '.txt' for writing matrix only, ",
				"or '.yml' or no extension to write matrix with metadata (as YAML)")
			).throw;
		};
	}


	prWriteMatrixToTXT { arg pn; // a PathName
		var writer;
		writer = FileWriter( pn.fullPath );
		// write the matrix into it by row, and close
		matrix.rows.do{ |i| writer.writeLine( matrix.getRow(i) ) };
		writer.close
	}

	prWriteMatrixToMOSL { arg pn; // a PathName
		var writer;
		writer = FileWriter( pn.fullPath );

		// write num rows and cols to first 2 lines
		writer.writeLine(["// Dimensions: rows, columns"]);
		writer.writeLine(matrix.rows.asArray);
		writer.writeLine(matrix.cols.asArray);
		// write the matrix into it by row, and close
		matrix.rows.do{ |i|
			var row;
			writer.writeLine([""]); // blank line
			writer.writeLine([format("// Row %", i)]);

			row = matrix.getRow(i);
			row.do{ |j| writer.writeLine( j.asArray ) };
		};
		writer.close;
	}

	prWriteMatrixToYML { arg pn, set, type, note, attributeDictionary;
		var writer, defAttributes;

		writer = FileWriter( pn.fullPath );

		// writer.writeLine(["matrix :"] ++ m.asArray.asString.split($ )); // all one line

		// overkill on formatting, but more readable...
		writer.writeLine(["matrix : ["]);
		matrix.rows.do{ |i|
			var line, row;
			row = matrix.getRow(i);
			line = row.asString.split($ );
			if ((i+1) != matrix.rows) {line[line.size-1] = line.last ++ ","};
			writer.writeLine(line);
		};
		writer.writeLine(["]"]);

		type !? {
			writer.writeLine([]);
			writer.writeLine( ["type", ":", type] )
		};

		// write default attributes
		defAttributes = [\kind, \dirOutputs, \dirInputs];
		if (type=='decoder') {
			defAttributes = defAttributes ++ [\shelfK,\shelfFreq]
		};

		if (attributeDictionary.notNil) {
			// make sure attribute dict doesn't explicitly set the attribute first
			defAttributes.do{ |attribute|
				attributeDictionary[attribute] ?? {
					writer.writeLine([]); //newline for readability
					writer.writeLine( [attribute, ":", this.tryPerform(attribute)] )
				}
			};
		} {
			defAttributes.do{ |attribute|
				writer.writeLine([]); //newline for readability
				writer.writeLine( [attribute, ":", this.tryPerform(attribute)] )
			};
		};

		note !? {
			writer.writeLine([]);
			writer.writeLine( ["note", ":", note] )
		};

		attributeDictionary !? {
			attributeDictionary.keysValuesDo{ |k,v|
				writer.writeLine([]);
				writer.writeLine( [k.asString, ":", v] )
			}
		};

		writer.close;
	}

	prParseMOSL { |pn|
		var file, numRows, numCols, mtx, row;
		file = FileReader.read(pn.fullPath);
		numRows = nil;
		numCols = nil;
		mtx = [];
		row = [];
		file.do{ |line|
			var val = line[0];
			switch( val,
				"//",	{}, // ignore comments
				"",		{},	// ignore blank line
				{	// found valid line
					case
					{numRows.isNil} { numRows = val.asInt }
					{numCols.isNil} { numCols = val.asInt }
					{
						row = row.add(val.asFloat);
						if (row.size==numCols) {
							mtx = mtx.add(row);
							row = [];
						}
					}
				}
			)
		};
		// test matrix dimensions
		(mtx.size==numRows).not.if{
			Error(
				format(
					"Mismatch in matrix dimensions: rows specified [%], rows parsed from file [%]",
					numRows, mtx.size
				)
			).throw
		};
		mtx.do{ |row, i|
			if (row.size!=numCols) {
				Error(
					format(
						"Mismatch in matrix dimensions: rows % has % columns, but file species %",
						i, row.size, numCols
					)
				).throw
			}
		};

		^mtx
	}

	fileName { ^try {PathName(filePath).fileName} }

	loadFromLib { |...args|
		var pathStr;
		pathStr = this.kind.asString ++ "/";

		args.do{ |argParam, i|
			pathStr = if (i>0) {
				format("%-%", pathStr, argParam.asString)
			} {
				format("%%", pathStr, argParam.asString)
			};
		};

		this.initFromFile(
			// format("%/%-%.yml", this.kind, *args),
			pathStr++".yml",
			this.type
		);

		switch( this.type,
			'\encoder', {this.initEncoderVarsForFiles}, // properly set dirInputs
			'\decoder', {this.initDecoderVarsForFiles}, // properly set dirOutputs
			'\xformer', {}
		)
	}

}

FoaDecoderMatrix : AtkMatrix {
	var <dirOutputs;
	var <>shelfFreq, <shelfK;

	*newDiametric { arg directions = [ pi/4, 3*pi/4 ], k = 'single';
		^super.new('diametric').initDiametric(directions, k);
	}

	*newPanto { arg numChans = 4, orientation = 'flat', k = 'single';
		^super.new('panto').initPanto(numChans, orientation, k);
	}

	*newPeri { arg numChanPairs = 4, elevation = 0.61547970867039,
				orientation = 'flat', k = 'single';
		^super.new('peri').initPeri(numChanPairs, elevation,
			orientation, k);
	}

	*newQuad { arg angle = pi/4, k = 'single';
		^super.new('quad').initQuad(angle, k);
	}

	*newStereo { arg angle = pi/2, pattern = 0.5;
		^super.new('stereo').initStereo(angle, pattern);
	}

	*newMono { arg theta = 0, phi = 0, pattern = 0;
		^super.new('mono').initMono(theta, phi, pattern);
	}

	*new5_0 { arg irregKind = 'focused';
		^super.new('5.0').init5_0(irregKind);
		// ^super.new('5_0').loadFromLib(irregKind);
	}

	*newBtoA { arg orientation = 'flu', weight = 'dec';
		^super.new('BtoA').initBtoA(orientation, weight);
		// ^super.new('BtoA').loadFromLib(orientation, weight);
	}

	*newHoa1 { arg ordering = 'acn', normalisation = 'n3d';
		^super.new('hoa1').initHoa1(ordering, normalisation);
		// ^super.new('hoa1').loadFromLib(ordering, normalisation);
	}

	*newAmbix1 {
		var ordering = 'acn', normalisation = 'sn3d';
		^super.new('hoa1').initHoa1(ordering, normalisation);
		// ^super.new('hoa1').loadFromLib(ordering, normalisation);
	}

	*newFromFile { arg filePathOrName;
		^super.new.initFromFile(filePathOrName, 'decoder', true).initDecoderVarsForFiles;
	}

	initK2D { arg k;

		if ( k.isNumber, {
				^k
			}, {
				switch ( k,
					'velocity', 	{ ^1 },
					'energy', 	{ ^2.reciprocal.sqrt },
					'controlled', { ^2.reciprocal },
					'single', 	{ ^2.reciprocal.sqrt },
					'dual', 		{
						shelfFreq = 400.0;
						shelfK = [(3/2).sqrt, 3.sqrt/2];
						^1;
					}
				)
			}
		)
	}

	initK3D { arg k;

		if ( k.isNumber, {
				^k
			}, {
				switch ( k,
					'velocity', 	{ ^1 },
					'energy', 	{ ^3.reciprocal.sqrt },
					'controlled', { ^3.reciprocal },
					'single', 	{ ^3.reciprocal.sqrt },
					'dual', 		{
						shelfFreq = 400.0;
						shelfK = [2.sqrt, (2/3).sqrt];
						^1;
					}
				)
			}
		)
	}

	initDiametric { arg directions, k;

		var positions, positions2;
		var speakerMatrix, n;

		switch (directions.rank,			// 2D or 3D?
			1, {									// 2D

				// find positions
				positions = Matrix.with(
					directions.collect({ arg item;
						Polar.new(1, item).asPoint.asArray
					})
				);

				// list all of the output channels (speakers)
				// i.e., expand to actual pairs
				positions2 = positions ++ (positions.neg);


			    // set output channel (speaker) directions for instance
				dirOutputs = positions2.asArray.collect({ arg item;
					item.asPoint.asPolar.angle
				});

				// initialise k
				k = this.initK2D(k);
			},
			2, {									// 3D

				// find positions
				positions = Matrix.with(
					directions.collect({ arg item;
						Spherical.new(1, item.at(0), item.at(1)).asCartesian.asArray
					})
				);

				// list all of the output channels (speakers)
				// i.e., expand to actual pairs
				positions2 = positions ++ (positions.neg);


			    // set output channel (speaker) directions for instance
				dirOutputs = positions2.asArray.collect({ arg item;
					item.asCartesian.asSpherical.angles
				});

				// initialise k
				k = this.initK3D(k);
			}
		);


	    	// get velocity gains
	    	// NOTE: this comment from Heller seems to be slightly
	    	//       misleading, in that the gains returned will be
	    	//       scaled by k, which may not request a velocity
	    	//       gain. I.e., k = 1 isn't necessarily true, as it
	    	//       is assigned as an argument to this function.
	    	speakerMatrix = FoaSpeakerMatrix.newPositions(positions2, k).matrix;

	    	// n = number of output channels (speakers)
		n = speakerMatrix.cols;

		// build decoder matrix
		// resulting rows (after flop) are W, X, Y, Z gains
		matrix = speakerMatrix.insertRow(0, Array.fill(n, {1}));

		// return resulting matrix
		// ALSO: the below code calls for the complex conjugate
		//       of decoder_matrix. As we are expecting real vaules,
		//       we may regard this call as redundant.
		// res = sqrt(2)/n * decoder_matrix.conj().transpose()
		matrix = 2.sqrt/n * matrix.flop;
	}

	initPanto { arg numChans, orientation, k;

		var g0, g1, theta;

	    	g0 = 1.0;								// decoder gains
	    	g1 = 2.sqrt;							// 0, 1st order


		// return theta from output channel (speaker) number
		theta = numChans.collect({ arg channel;
			switch (orientation,
				'flat',	{ ((1.0 + (2.0 * channel))/numChans) * pi },
				'point',	{ ((2.0 * channel)/numChans) * pi }
			)
		});
		theta = (theta + pi).mod(2pi) - pi;

	    // set output channel (speaker) directions for instance
		dirOutputs = theta;

		// initialise k
		k = this.initK2D(k);


		// build decoder matrix
		matrix = Matrix.newClear(numChans, 3); // start w/ empty matrix

		numChans.do({ arg i;
			matrix.putRow(i, [
				g0,
	              k * g1 * theta.at(i).cos,
	              k * g1 * theta.at(i).sin
			])
			});
		matrix = 2.sqrt/numChans * matrix
	}

	initPeri { arg numChanPairs, elevation, orientation, k;

		var theta, directions, upDirs, downDirs, upMatrix, downMatrix;

		// generate output channel (speaker) pair positions
		// start with polar positions. . .
		theta = [];
		numChanPairs.do({arg i;
			theta = theta ++ [2 * pi * i / numChanPairs]}
		);
		if ( orientation == 'flat',
			{ theta = theta + (pi / numChanPairs) });       // 'flat' case

		// collect directions [ [theta, phi], ... ]
		// upper ring only
		directions = [
			theta,
			Array.newClear(numChanPairs).fill(elevation)
		].flop;


	    // prepare output channel (speaker) directions for instance
		upDirs = (directions + pi).mod(2pi) - pi;

		downDirs = upDirs.collect({ arg angles;
			Spherical.new(1, angles.at(0), angles.at(1)).neg.angles
		});

		// initialise k
		k = this.initK3D(k);


		// build decoder matrix
		matrix = FoaDecoderMatrix.newDiametric(directions, k).matrix;

		// reorder the lower polygon
		upMatrix = matrix[..(numChanPairs-1)];
		downMatrix = matrix[(numChanPairs)..];

		if ( (orientation == 'flat') && (numChanPairs.mod(2) == 1),
			{									 // odd, 'flat'

				downDirs = downDirs.rotate((numChanPairs/2 + 1).asInteger);
				downMatrix = downMatrix.rotate((numChanPairs/2 + 1).asInteger)

			}, {     								// 'flat' case, default

				downDirs = downDirs.rotate((numChanPairs/2).asInteger);
				downMatrix = downMatrix.rotate((numChanPairs/2).asInteger)
			}
		);

		dirOutputs = upDirs ++ downDirs;		// set output channel (speaker) directions
		matrix = upMatrix ++ downMatrix;			// set matrix

	}

	initQuad { arg angle, k;

		var g0, g1, g2;

	    // set output channel (speaker) directions for instance
	    dirOutputs = [ angle, pi - angle, (pi - angle).neg, angle.neg ];


		// initialise k
		k = this.initK2D(k);

		// calculate g1, g2 (scaled by k)
		g0	= 1;
		g1	= k / (2.sqrt * angle.cos);
		g2	= k / (2.sqrt * angle.sin);

		// build decoder matrix
	    matrix = 2.sqrt/4 * Matrix.with([
	    		[ g0, g1, 	g2 		],
	        	[ g0, g1.neg, g2 		],
	        	[ g0, g1.neg, g2.neg	],
	        	[ g0, g1, 	g2.neg	]
	    ])
	}

	initStereo { arg angle, pattern;

		var g0, g1, g2;

	    // set output channel (speaker) directions for instance
	    dirOutputs = [ pi/6, pi.neg/6 ];

		// calculate g0, g1, g2 (scaled by pattern)
		g0	= (1.0 - pattern) * 2.sqrt;
		g1	= pattern * angle.cos;
		g2	= pattern * angle.sin;

		// build decoder matrix, and set for instance
	    matrix = Matrix.with([
	    		[ g0, g1, g2		],
	        	[ g0, g1, g2.neg	]
	    ])
	}

	initMono { arg theta, phi, pattern;

	    // set output channel (speaker) directions for instance
	    dirOutputs = [ 0 ];

		// build decoder matrix, and set for instance
	    matrix = Matrix.with([
	    		[
	    			(1.0 - pattern) * 2.sqrt,
	    			pattern * theta.cos * phi.cos,
	    			pattern * theta.sin * phi.cos,
	    			pattern * phi.sin
	    		]
	    ])
	}

	init5_0 { arg irregKind;

	    // set output channel (speaker) directions for instance
	    dirOutputs = [ 0, pi/6, 110/180 * pi, 110/180 * pi.neg, pi.neg/6 ];

		// build decoder matrix
		// Wigging's Matricies (credit contribution/copyright at top)
		matrix = switch (irregKind,
			'focused', {[
		    		[ 0.2000,  0.1600,  0.0000 ],
		        	[ 0.4250,  0.3600,  0.4050 ],
		        	[ 0.4700, -0.3300,  0.4150 ],
		        	[ 0.4700, -0.3300, -0.4150 ],
		        	[ 0.4250,  0.3600, -0.4050 ]
			]},
			'equal', {[
		    		[ 0.0000,  0.0850,  0.0000 ],
		        	[ 0.3650,  0.4350,  0.3400 ],
		        	[ 0.5550, -0.2850,  0.4050 ],
		        	[ 0.5550, -0.2850, -0.4050 ],
		        	[ 0.3650,  0.4350, -0.3400 ]
		    ]},
			'four', {[
		    		[ 0.0000,  0.0000,  0.0000 ],
		        	[ 0.4250,  0.3850,  0.3300 ],
		        	[ 0.6300, -0.2750,  0.2850 ],
		        	[ 0.6300, -0.2750, -0.2850 ],
		        	[ 0.4250,  0.3850, -0.3300 ]
		    ]}
		);
		matrix = Matrix.with(matrix)
	}

	initBtoA { arg orientation, weight;

		var recSqrt2 = 2.sqrt.reciprocal;
		var sqrt3Div2 = 3.sqrt/2;
		var sqrt3Div6 = 3.sqrt/6;
		var sqrt6Div3 = 6.sqrt/3;
		var recSqrt6 = 6.sqrt.reciprocal;
		var g0;


		// build decoder matrix, and set for instance
		g0 = switch ( weight,
			'dec', { (2/3).sqrt },	// decorrelated (on the sphere)
			'can', { 1 },			// canonical
			'uns', { 2.sqrt },		// unscaled, W_gain = 1
			'car', { 6.sqrt }		// cardioid
		);

		matrix = switch ( orientation,

			// 0 - orthogonal (front left up)
			// [ FLU, FRD, BLD, BRU ]
			'flu', {[
				[ 0.5, 0.5, 0.5, 0.5 ],
				[ 0.5, 0.5, -0.5, -0.5 ],
				[ 0.5, -0.5, 0.5, -0.5 ],
				[ 0.5, -0.5, -0.5, 0.5 ]
			]},
			// 1 - front left down
			// [ FLD, FRU, BLU, BRD ]
			'fld', {[
				[ 0.5, 0.5, 0.5, -0.5 ],
				[ 0.5, 0.5, -0.5, 0.5 ],
				[ 0.5, -0.5, 0.5, 0.5 ],
				[ 0.5, -0.5, -0.5, -0.5 ]
			]},
			// 2 - front left-right
			// [ FL, FR, BU, BD ]
			'flr', {[
				[ 0.5, 0.5, recSqrt2, 0 ],
				[ 0.5, 0.5, recSqrt2.neg, 0 ],
				[ 0.5, -0.5, 0, recSqrt2 ],
				[ 0.5, -0.5, 0, recSqrt2.neg ]
			]},
			// 3 - front up-down
			// [ FU, FD, BL, BR ]
			'fud', {[
				[ 0.5, 0.5, 0, recSqrt2 ],
				[ 0.5, 0.5, 0, recSqrt2.neg ],
				[ 0.5, -0.5, recSqrt2, 0 ],
				[ 0.5, -0.5, recSqrt2.neg, 0 ]
			]},
			// 4 - front & back down
			// [ F, BD, BLU, BRU ]
			'fbd', {[
				[ 0.5, sqrt3Div2, 0, 0 ],
				[ 0.5, sqrt3Div6.neg, 0, sqrt6Div3.neg ],
				[ 0.5, sqrt3Div6.neg, recSqrt2, recSqrt6 ],
				[ 0.5, sqrt3Div6.neg, recSqrt2.neg, recSqrt6 ]
			]},
			// 5 - front & back up
			// [ F, BU, BLD, BRD ]
			'fbu', {[
				[ 0.5, sqrt3Div2, 0, 0 ],
				[ 0.5, sqrt3Div6.neg, 0, sqrt6Div3 ],
				[ 0.5, sqrt3Div6.neg, recSqrt2, recSqrt6.neg ],
				[ 0.5, sqrt3Div6.neg, recSqrt2.neg, recSqrt6.neg ]
			]},
			// 6 - front left-right up
			// [ FLU, FRU, FD, B ]
			'flru', {[
				[ 0.5, sqrt3Div6, recSqrt2, recSqrt6 ],
				[ 0.5, sqrt3Div6, recSqrt2.neg, recSqrt6 ],
				[ 0.5, sqrt3Div6, 0, sqrt6Div3.neg ],
				[ 0.5, sqrt3Div2.neg, 0, 0 ]
			]},
			// 7 - front left-right down
			// [ FLD, FRD, FU, B ]
			'flrd', {[
				[ 0.5, sqrt3Div6, recSqrt2, recSqrt6.neg ],
				[ 0.5, sqrt3Div6, recSqrt2.neg, recSqrt6.neg ],
				[ 0.5, sqrt3Div6, 0, sqrt6Div3 ],
				[ 0.5, sqrt3Div2.neg, 0, 0 ]
			]}
		);
		matrix = Matrix.with(matrix);
		matrix = matrix.putCol(0, g0 * matrix.getCol(0));


	    // set output channel (speaker) directions for instance
	    dirOutputs = matrix.removeCol(0).asArray.collect({arg item;
			item.asCartesian.asSpherical.angles
		})
	}

	initHoa1 { arg ordering, normalisation;

		var sqrt2 = 2.sqrt;
		var sqrt3 = 3.sqrt;

		matrix = switch ( (ordering ++ normalisation).asSymbol,

			// 0 - acn-n3d (aka Hoa1)
			'acnn3d', {[
				[ sqrt2, 0.0, 0.0, 0.0 ],
				[ 0.0, 0.0, sqrt3, 0.0 ],
				[ 0.0, 0.0, 0.0, sqrt3 ],
				[ 0.0, sqrt3, 0.0, 0.0 ]
			]},
			// 1 - acn-sn3d (aka Ambix1)
			'acnsn3d', {[
				[ sqrt2, 0.0, 0.0, 0.0 ],
				[ 0.0, 0.0, 1.0, 0.0 ],
				[ 0.0, 0.0, 0.0, 1.0 ],
				[ 0.0, 1.0, 0.0, 0.0 ]
			]},
			// 2 - sid-n3d
			'sidn3d', {[
				[ sqrt2, 0.0, 0.0, 0.0 ],
				[ 0.0, sqrt3, 0.0, 0.0 ],
				[ 0.0, 0.0, sqrt3, 0.0 ],
				[ 0.0, 0.0, 0.0, sqrt3 ]
			]},
			// 3 - sid-sn3d
			'sidsn3d', {[
				[ sqrt2, 0.0, 0.0, 0.0 ],
				[ 0.0, 1.0, 0.0, 0.0 ],
				[ 0.0, 0.0, 1.0, 0.0 ],
				[ 0.0, 0.0, 0.0, 1.0 ]
			]},

		);
		matrix = Matrix.with(matrix);

		// set output channel (speaker) directions for instance
		// still in b-format in this case, so 'inf' directions
		dirOutputs = matrix.rows.collect({ inf });
	}

	initDecoderVarsForFiles {
		if (fileParse.notNil) {
			dirOutputs = if (fileParse.dirOutputs.notNil) {
				fileParse.dirOutputs.asFloat
			} { // output directions are unspecified in the provided matrix
				matrix.rows.collect({ 'unspecified' })
			};
			shelfK = fileParse.shelfK !? {fileParse.shelfK.asFloat};
			shelfFreq = fileParse.shelfFreq !? {fileParse.shelfFreq.asFloat};
		} { // txt file provided, no fileParse
			dirOutputs = matrix.rows.collect({ 'unspecified' });
		};
	}

	dirInputs { ^this.numInputs.collect({ inf }) }

	dirChannels { ^this.dirOutputs }

	numInputs { ^matrix.cols }

	numOutputs { ^matrix.rows }

	numChannels { ^this.numOutputs }

	dim { ^this.numInputs - 1}

	type { ^'decoder' }

	printOn { arg stream;
		stream << this.class.name << "(" <<* [this.kind, this.dim, this.numChannels] <<")";
	}
}


//-----------------------------------------------------------------------
// martrix encoders

FoaEncoderMatrix : AtkMatrix {
	var <dirInputs;

	*newAtoB { arg orientation = 'flu', weight = 'dec';
		^super.new('AtoB').initAtoB(orientation, weight);
		// ^super.new('AtoB').loadFromLib(orientation, weight)
	}

	*newHoa1 { arg ordering = 'acn', normalisation = 'n3d';
		^super.new('hoa1').initHoa1(ordering, normalisation);
		// ^super.new('hoa1').loadFromLib(ordering, normalisation);
	}

	*newAmbix1 {
		var ordering = 'acn', normalisation = 'sn3d';
		^super.new('hoa1').initHoa1(ordering, normalisation);
		// ^super.new('hoa1').loadFromLib(ordering, normalisation);
	}

	*newZoomH2n{
		var ordering = 'acn', normalisation = 'sn3d';
		^super.new('hoa1').initHoa1(ordering, normalisation);
		// ^super.new('hoa1').loadFromLib(ordering, normalisation);
	}

	*newOmni {
		^super.new('omni').initOmni;
	}

	*newDirection { arg theta = 0, phi = 0;
		^super.new('dir').initDirection(theta, phi);
	}

	*newStereo { arg angle = 0;
		^super.new('stereo').initStereo(angle);
	}

	*newQuad {
		^super.new('quad').initQuad;
	}

	*new5_0 {
		^super.new('5.0').init5_0;
	}

	*new7_0 {
		^super.new('7.0').init7_0;
	}

	*newDirections { arg directions, pattern = nil;
		^super.new('dirs').initDirections(directions, pattern);
	}

	*newPanto { arg numChans = 4, orientation = 'flat';
		^super.new('panto').initPanto(numChans, orientation);
	}

	*newPeri { arg numChanPairs = 4, elevation = 0.61547970867039,
				orientation = 'flat';
		^super.new('peri').initPeri(numChanPairs, elevation,
			orientation);
	}

	*newZoomH2 { arg angles = [pi/3, 3/4*pi], pattern = 0.5857, k = 1;
		^super.new('zoomH2').initZoomH2(angles, pattern, k);
	}

	*newFromFile { arg filePathOrName;
		^super.new.initFromFile(filePathOrName, 'encoder', true).initEncoderVarsForFiles
	}

	init2D {

		var g0 = 2.sqrt.reciprocal;

		// build encoder matrix, and set for instance
		matrix = Matrix.newClear(3, dirInputs.size); // start w/ empty matrix

		dirInputs.do({ arg theta, i;
			matrix.putCol(i, [
				g0,
	              theta.cos,
	              theta.sin
			])
		})
	}

	init3D {

		var g0 = 2.sqrt.reciprocal;

		// build encoder matrix, and set for instance
		matrix = Matrix.newClear(4, dirInputs.size); // start w/ empty matrix

		dirInputs.do({ arg thetaPhi, i;
			matrix.putCol(i, [
				g0,
	              thetaPhi.at(1).cos * thetaPhi.at(0).cos,
	              thetaPhi.at(1).cos * thetaPhi.at(0).sin,
	              thetaPhi.at(1).sin
			])
		})
	}

	initInv2D { arg pattern;

		var g0 = 2.sqrt.reciprocal;

		// build 'decoder' matrix, and set for instance
		matrix = Matrix.newClear(dirInputs.size, 3); 	// start w/ empty matrix

		if ( pattern.isArray,
			{
				dirInputs.do({ arg theta, i;			// mic positions, indivd patterns
					matrix.putRow(i, [
						(1.0 - pattern.at(i)),
			              pattern.at(i) * theta.cos,
			              pattern.at(i) * theta.sin
					])
				})
			}, {
				dirInputs.do({ arg theta, i;			// mic positions
					matrix.putRow(i, [
						(1.0 - pattern),
			              pattern * theta.cos,
			              pattern * theta.sin
					])
				})
			}
		);

		// invert to encoder matrix
		matrix = matrix.pseudoInverse;

		// normalise matrix
		matrix = matrix * matrix.getRow(0).sum.reciprocal;

		// scale W
		matrix = matrix.putRow(0, matrix.getRow(0) * g0);
	}

	initInv3D { arg pattern;

		var g0 = 2.sqrt.reciprocal;

		// build 'decoder' matrix, and set for instance
		matrix = Matrix.newClear(dirInputs.size, 4); 	// start w/ empty matrix

		if ( pattern.isArray,
			{
				dirInputs.do({ arg thetaPhi, i;		// mic positions, indivd patterns
					matrix.putRow(i, [
						(1.0 - pattern.at(i)),
			              pattern.at(i) * thetaPhi.at(1).cos * thetaPhi.at(0).cos,
			              pattern.at(i) * thetaPhi.at(1).cos * thetaPhi.at(0).sin,
			              pattern.at(i) * thetaPhi.at(1).sin
					])
				})
			}, {
				dirInputs.do({ arg thetaPhi, i;		// mic positions
					matrix.putRow(i, [
						(1.0 - pattern),
			              pattern * thetaPhi.at(1).cos * thetaPhi.at(0).cos,
			              pattern * thetaPhi.at(1).cos * thetaPhi.at(0).sin,
			              pattern * thetaPhi.at(1).sin
					])
				})
			}
		);

		// invert to encoder matrix
		matrix = matrix.pseudoInverse;

		// normalise matrix
		matrix = matrix * matrix.getRow(0).sum.reciprocal;

		// scale W
		matrix = matrix.putRow(0, matrix.getRow(0) * g0);
	}

	initAtoB { arg orientation, weight;
		var bToAMatrix;

		// retrieve corresponding A-format decoder
		bToAMatrix = FoaDecoderMatrix.newBtoA(orientation, weight);

	    // set input channel directions for instance
	    dirInputs = bToAMatrix.dirOutputs;

		// build encoder matrix, and set for instance
	    matrix = bToAMatrix.matrix.inverse
	}

	initHoa1 { arg ordering, normalisation;
		var bToHoa1Matrix;

		// retrieve corresponding Hoa1 decoder
		bToHoa1Matrix = FoaDecoderMatrix.newHoa1(ordering, normalisation);

	    // set input channel directions for instance
	    dirInputs = bToHoa1Matrix.dirInputs;

		// build encoder matrix, and set for instance
	    matrix = bToHoa1Matrix.matrix.inverse
	}

	initOmni {

	    // set input channel directions for instance
	    dirInputs = [ inf ];

		// build encoder matrix, and set for instance
	    matrix = Matrix.with([
		    	[ 2.sqrt.reciprocal ]
		])
	}

	initDirection { arg theta, phi;

	    // set input channel directions for instance
	    (phi == 0).if (
		    {
				dirInputs = [ theta ];
    				this.init2D
			}, {
	    			dirInputs = [ [theta, phi] ];
    				this.init3D
			}
		)
	}

	initStereo { arg angle;

	    // set input channel directions for instance
	    dirInputs = [ pi/2 - angle, (pi/2 - angle).neg ];

	    this.init2D
	}

	initQuad {

	    // set input channel directions for instance
	    dirInputs = [ pi/4, pi * 3/4, pi.neg * 3/4, pi.neg/4 ];

	    this.init2D
	}

	init5_0 {

	    // set input channel directions for instance
	    dirInputs = [ 0, pi/6, 110/180 * pi, 110/180 * pi.neg, pi.neg/6 ];

	    this.init2D
	}

	init7_0 {

	    // set input channel directions for instance
	    dirInputs = [ 0, pi/6, pi/2, 135/180 * pi, 135/180 * pi.neg, pi.neg/2, pi.neg/6 ];

	    this.init2D
	}

	initDirections { arg directions, pattern;

	    // set input channel directions for instance
	    dirInputs = directions;

		switch (directions.rank,					// 2D or 3D?
			1, {									// 2D
				if ( pattern == nil, {
					this.init2D					// plane wave
				}, {
					this.initInv2D(pattern)			// mic inversion
				})
			},
			2, {									// 3D
				if ( pattern == nil, {
					this.init3D					// plane wave
				}, {
					this.initInv3D(pattern)			// mic inversion
				})
			}
		)
	}

	initPanto { arg numChans, orientation;

		var theta;

		// return theta from output channel (speaker) number
		theta = numChans.collect({ arg channel;
			switch (orientation,
				'flat',	{ ((1.0 + (2.0 * channel))/numChans) * pi },
				'point',	{ ((2.0 * channel)/numChans) * pi }
			)
		});
		theta = (theta + pi).mod(2pi) - pi;

	    // set input channel directions for instance
		dirInputs = theta;

		this.init2D
	}

	initPeri { arg numChanPairs, elevation, orientation;

		var theta, directions, upDirs, downDirs, upMatrix, downMatrix;

		// generate input channel pair positions
		// start with polar positions. . .
		theta = [];
		numChanPairs.do({arg i;
			theta = theta ++ [2 * pi * i / numChanPairs]}
		);
		if ( orientation == 'flat',
			{ theta = theta + (pi / numChanPairs) });       // 'flat' case

		// collect directions [ [theta, phi], ... ]
		// upper ring only
		directions = [
			theta,
			Array.newClear(numChanPairs).fill(elevation)
		].flop;


	    // prepare output channel (speaker) directions for instance
		upDirs = (directions + pi).mod(2pi) - pi;

		downDirs = upDirs.collect({ arg angles;
			Spherical.new(1, angles.at(0), angles.at(1)).neg.angles
		});

		// reorder the lower polygon
		if ( (orientation == 'flat') && (numChanPairs.mod(2) == 1),
			{									 // odd, 'flat'
				downDirs = downDirs.rotate((numChanPairs/2 + 1).asInteger);
			}, {     								// 'flat' case, default
				downDirs = downDirs.rotate((numChanPairs/2).asInteger);
			}
		);

	    // set input channel directions for instance
		dirInputs = upDirs ++ downDirs;

		this.init3D
	}

	initZoomH2 { arg angles, pattern, k;

	    // set input channel directions for instance
	    dirInputs = [ angles.at(0), angles.at(0).neg, angles.at(1), angles.at(1).neg ];

		this.initInv2D(pattern);

		matrix = matrix.putRow(2, matrix.getRow(2) * k); // scale Y
	}

	initEncoderVarsForFiles {
		dirInputs = if (fileParse.notNil) {
			if (fileParse.dirInputs.notNil) {
				fileParse.dirInputs.asFloat
			} { // so input directions are unspecified in the provided matrix
				matrix.cols.collect({'unspecified'})
			};
		} { // txt file provided, no fileParse
			matrix.cols.collect({'unspecified'});
		};
	}


	dirOutputs { ^this.numOutputs.collect({ inf }) }

	dirChannels { ^this.dirInputs }

	numInputs { ^matrix.cols }

	numOutputs { ^matrix.rows }

	numChannels { ^this.numInputs }

	dim { ^this.numOutputs - 1}

	type { ^'encoder' }

	printOn { arg stream;
		stream << this.class.name << "(" <<* [kind, this.dim, this.numInputs] <<")";
	}
}


//-----------------------------------------------------------------------
// martrix transforms


FoaXformerMatrix : AtkMatrix {

	*newMirrorX {
		^super.new('mirrorX').initMirrorX;
	}

	*newMirrorY {
		^super.new('mirrorY').initMirrorY;
	}

	*newMirrorZ {
		^super.new('mirrorZ').initMirrorZ;
	}

	*newMirrorO {
		^super.new('mirrorO').initMirrorO;
	}

	*newRotate { arg angle = 0;
		^super.new('rotate').initRotate(angle);
	}

	*newTilt { arg angle = 0;
		^super.new('tilt').initTilt(angle);
	}

	*newTumble { arg angle = 0;
		^super.new('tumble').initTumble(angle);
	}

	*newDirectO { arg angle = 0;
		^super.new('directO').initDirectO(angle);
	}

	*newDirectX { arg angle = 0;
		^super.new('directX').initDirectX(angle);
	}

	*newDirectY { arg angle = 0;
		^super.new('directY').initDirectY(angle);
	}

	*newDirectZ { arg angle = 0;
		^super.new('directZ').initDirectZ(angle);
	}

	*newDominateX { arg gain = 0;
		^super.new('dominateX').initDominateX(gain);
	}

	*newDominateY { arg gain = 0;
		^super.new('dominateY').initDominateY(gain);
	}

	*newDominateZ { arg gain = 0;
		^super.new('dominateZ').initDominateZ(gain);
	}

	*newZoomX { arg angle = 0;
		^super.new('zoomX').initZoomX(angle);
	}

	*newZoomY { arg angle = 0;
		^super.new('zoomY').initZoomY(angle);
	}

	*newZoomZ { arg angle = 0;
		^super.new('zoomZ').initZoomZ(angle);
	}

	*newFocusX { arg angle = 0;
		^super.new('focusX').initFocusX(angle);
	}

	*newFocusY { arg angle = 0;
		^super.new('focusY').initFocusY(angle);
	}

	*newFocusZ { arg angle = 0;
		^super.new('focusZ').initFocusZ(angle);
	}

	*newPushX { arg angle = 0;
		^super.new('pushX').initPushX(angle);
	}

	*newPushY { arg angle = 0;
		^super.new('pushY').initPushY(angle);
	}

	*newPushZ { arg angle = 0;
		^super.new('pushZ').initPushZ(angle);
	}

	*newPressX { arg angle = 0;
		^super.new('pressX').initPressX(angle);
	}

	*newPressY { arg angle = 0;
		^super.new('pressY').initPressY(angle);
	}

	*newPressZ { arg angle = 0;
		^super.new('pressZ').initPressZ(angle);
	}

	*newAsymmetry { arg angle = 0;
		^super.new('asymmetry').initAsymmetry(angle);
	}

	*newBalance { arg angle = 0;
		^super.new('zoomY').initZoomY(angle);
	}

	*newRTT { arg rotAngle = 0, tilAngle = 0, tumAngle = 0;
		^super.new('rtt').initRTT(rotAngle, tilAngle, tumAngle);
	}

	*newMirror { arg theta = 0, phi = 0;
		^super.new('mirror').initMirror(theta, phi);
	}

	*newDirect { arg angle = 0, theta = 0, phi = 0;
		^super.new('direct').initDirect(angle, theta, phi);
	}

	*newDominate { arg gain = 0, theta = 0, phi = 0;
		^super.new('dominate').initDominate(gain, theta, phi);
	}

	*newZoom { arg angle = 0, theta = 0, phi = 0;
		^super.new('zoom').initZoom(angle, theta, phi);
	}

	*newFocus { arg angle = 0, theta = 0, phi = 0;
		^super.new('focus').initFocus(angle, theta, phi);
	}

	*newPush { arg angle = 0, theta = 0, phi = 0;
		^super.new('push').initPush(angle, theta, phi);
	}

	*newPress { arg angle = 0, theta = 0, phi = 0;
		^super.new('press').initPress(angle, theta, phi);
	}

	*newFromFile { arg filePathOrName;
		^super.new.initFromFile(filePathOrName, 'xformer', true);
	}

	initMirrorChan { arg chan;
		matrix = matrix.put(chan, chan, matrix.get(chan, chan).neg)
	}

	initMirrorX {
		var chan;

		// ambisonic channel index
		chan = 1;

		// build identity matrix
		matrix = Matrix.newIdentity(4);

		// mirror it
		this.initMirrorChan(chan)
	}

	initMirrorY {
		var chan;

		// ambisonic channel index
		chan = 2;

		// build identity matrix
		matrix = Matrix.newIdentity(4);

		// mirror it
		this.initMirrorChan(chan)
	}

	initMirrorZ {
		var chan;

		// ambisonic channel index
		chan = 3;

		// build identity matrix
		matrix = Matrix.newIdentity(4);

		// mirror it
		this.initMirrorChan(chan)
	}

	initMirrorO {
		var chans;

		// ambisonic channel index
		chans = [1, 2, 3];

		// build identity matrix
		matrix = Matrix.newIdentity(4);

		// mirror it
		chans.do({arg chan;
			this.initMirrorChan(chan)
		})
	}

	initRotate { arg angle;
		var cosAngle, sinAngle;

		// build transform matrix, and set for instance
		// calculate cos, sin
		cosAngle	= angle.cos;
		sinAngle	= angle.sin;

	    matrix = Matrix.with([
	    		[ 1, 0, 			0,			0 ],
	    		[ 0, cosAngle,	sinAngle.neg,	0 ],
	    		[ 0, sinAngle, 	cosAngle,		0 ],
	    		[ 0, 0, 			0,			1 ]
	    ])
	}

	initTilt { arg angle;
		var cosAngle, sinAngle;

		// build transform matrix, and set for instance
		// calculate cos, sin
		cosAngle	= angle.cos;
		sinAngle	= angle.sin;

	    matrix = Matrix.with([
	    		[ 1, 0, 0,		0 			],
	    		[ 0, 1, 0,		0 			],
	    		[ 0,	0, cosAngle,	sinAngle.neg 	],
	    		[ 0,	0, sinAngle, 	cosAngle 		]
	    ])
	}

	initTumble { arg angle;
		var cosAngle, sinAngle;

		// build transform matrix, and set for instance
		// calculate cos, sin
		cosAngle	= angle.cos;
		sinAngle	= angle.sin;

	    matrix = Matrix.with([
	    		[ 1, 0, 			0,	0 			],
	    		[ 0, cosAngle,	0,	sinAngle.neg	],
	    		[ 0, 0,			1, 	0 			],
	    		[ 0, sinAngle,	0, 	cosAngle 		]
	    ])
	}

	initDirectO { arg angle;
		var g0, g1;

		// build transform matrix, and set for instance
		g0 = (1 + angle.sin).sqrt;
		g1 = (1 - angle.sin).sqrt;

	    matrix = Matrix.with([
	    		[ g0,	0,	0,	0 	],
	    		[ 0, 	g1,	0,	0	],
	    		[ 0, 	0,	g1, 	0 	],
	    		[ 0, 	0,	0, 	g1 	]
	    ])
	}

	initDirectX { arg angle;
		var g0, g1;

		// build transform matrix, and set for instance
		g0 = (1 + angle.sin).sqrt;
		g1 = (1 - angle.sin).sqrt;

	    matrix = Matrix.with([
	    		[ g0,	0,	0,	0 	],
	    		[ 0, 	g1,	0,	0	],
	    		[ 0, 	0,	g0, 	0 	],
	    		[ 0, 	0,	0, 	g0 	]
	    ])
	}

	initDirectY { arg angle;
		var g0, g1;

		// build transform matrix, and set for instance
		g0 = (1 + angle.sin).sqrt;
		g1 = (1 - angle.sin).sqrt;

	    matrix = Matrix.with([
	    		[ g0,	0,	0,	0 	],
	    		[ 0, 	g0,	0,	0	],
	    		[ 0, 	0,	g1, 	0 	],
	    		[ 0, 	0,	0, 	g0 	]
	    ])
	}

	initDirectZ { arg angle;
		var g0, g1;

		// build transform matrix, and set for instance
		g0 = (1 + angle.sin).sqrt;
		g1 = (1 - angle.sin).sqrt;

	    matrix = Matrix.with([
	    		[ g0,	0,	0,	0 	],
	    		[ 0, 	g0,	0,	0	],
	    		[ 0, 	0,	g0, 	0 	],
	    		[ 0, 	0,	0, 	g1 	]
	    ])
	}

	initDominateX { arg gain;
		var g0, g1, k;

		// build transform matrix, and set for instance
		k = gain.dbamp;

		g0 = (k + k.reciprocal) / 2;
		g1 = (k - k.reciprocal) / 2.sqrt;

	    matrix = Matrix.with([
	    		[ g0,	g1/2,	0,	0 	],
	    		[ g1, 	g0,		0,	0	],
	    		[ 0, 	0,		1, 	0 	],
	    		[ 0, 	0,		0, 	1 	]
	    ])
	}

	initDominateY { arg gain;
		var g0, g1, k;

		// build transform matrix, and set for instance
		k = gain.dbamp;

		g0 = (k + k.reciprocal) / 2;
		g1 = (k - k.reciprocal) / 2.sqrt;

	    matrix = Matrix.with([
	    		[ g0,	0,	g1/2,	0 	],
	    		[ 0, 	1,	0, 		0 	],
	    		[ g1, 	0,	g0,		0	],
	    		[ 0, 	0,	0, 		1 	]
	    ])
	}

	initDominateZ { arg gain;
		var g0, g1, k;

		// build transform matrix, and set for instance
		k = gain.dbamp;

		g0 = (k + k.reciprocal) / 2;
		g1 = (k - k.reciprocal) / 2.sqrt;

	    matrix = Matrix.with([
	    		[ g0,	0,	0,	g1/2	],
	    		[ 0, 	1,	0, 	0 	],
	    		[ 0, 	0,	1, 	0 	],
	    		[ g1, 	0,	0,	g0	]
	    ])
	}

	initZoomX { arg angle;
		var g0, g1;

		// build transform matrix, and set for instance
		g0 = angle.sin;
		g1 = angle.cos;

	    matrix = Matrix.with([
	    		[ 1,			g0/2.sqrt,	0,	0 	],
	    		[ 2.sqrt*g0, 	1,			0,	0	],
	    		[ 0, 		0,			g1, 	0 	],
	    		[ 0, 		0,			0, 	g1 	]
	    ])
	}

	initZoomY { arg angle;
		var g0, g1;

		// build transform matrix, and set for instance
		g0 = angle.sin;
		g1 = angle.cos;

	    matrix = Matrix.with([
	    		[ 1,			0,	g0/2.sqrt,	0 	],
	    		[ 0, 		g1,	0, 			0 	],
	    		[ 2.sqrt*g0, 	0,	1,			0	],
	    		[ 0, 		0,	0, 			g1 	]
	    ])
	}

	initZoomZ { arg angle;
		var g0, g1;

		// build transform matrix, and set for instance
		g0 = angle.sin;
		g1 = angle.cos;

	    matrix = Matrix.with([
	    		[ 1,			0,	0,	g0/2.sqrt	],
	    		[ 0, 		g1,	0, 	0 		],
	    		[ 0, 		0, 	g1,	0 		],
	    		[ 2.sqrt*g0, 	0,	0,	1		]
	    ])
	}

	initFocusX { arg angle;
		var g0, g1, g2;

		// build transform matrix, and set for instance
		g0 = (1 + angle.abs.sin).reciprocal;
		g1 = 2.sqrt * angle.sin * g0;
		g2 = angle.cos * g0;

	    matrix = Matrix.with([
	    		[ g0,	g1/2,	0,	0	],
	    		[ g1,	g0,		0,	0	],
	    		[ 0,		0,		g2, 	0 	],
	    		[ 0,		0,		0, 	g2	]
	    ])
	}

	initFocusY { arg angle;
		var g0, g1, g2;

		// build transform matrix, and set for instance
		g0 = (1 + angle.abs.sin).reciprocal;
		g1 = 2.sqrt * angle.sin * g0;
		g2 = angle.cos * g0;

	    matrix = Matrix.with([
	    		[ g0,	0,	g1/2,	0	],
	    		[ 0,		g2,	0, 		0 	],
	    		[ g1,	0,	g0,		0	],
	    		[ 0,		0,	0, 		g2	]
	    ])
	}

	initFocusZ { arg angle;
		var g0, g1, g2;

		// build transform matrix, and set for instance
		g0 = (1 + angle.abs.sin).reciprocal;
		g1 = 2.sqrt * angle.sin * g0;
		g2 = angle.cos * g0;

	    matrix = Matrix.with([
	    		[ g0,	0,	0,	g1/2	],
	    		[ 0,		g2,	0, 	0 	],
	    		[ 0,		0, 	g2,	0	],
	    		[ g1,	0,	0,	g0	]
	    ])
	}

	initPushX { arg angle;
		var g0, g1;

		// build transform matrix, and set for instance
		g0 = 2.sqrt * angle.sin * angle.abs.sin;
		g1 = angle.cos.squared;

	    matrix = Matrix.with([
	    		[ 1,		0,	0,	0	],
	    		[ g0,	g1,	0,	0	],
	    		[ 0,		0,	g1, 	0 	],
	    		[ 0,		0,	0, 	g1	]
	    ])
	}

	initPushY { arg angle;
		var g0, g1;

		// build transform matrix, and set for instance
		g0 = 2.sqrt * angle.sin * angle.abs.sin;
		g1 = angle.cos.squared;

	    matrix = Matrix.with([
	    		[ 1,		0,	0,	0	],
	    		[ 0,		g1,	0, 	0 	],
	    		[ g0,	0,	g1,	0	],
	    		[ 0,		0,	0, 	g1	]
	    ])
	}

	initPushZ { arg angle;
		var g0, g1;

		// build transform matrix, and set for instance
		g0 = 2.sqrt * angle.sin * angle.abs.sin;
		g1 = angle.cos.squared;

	    matrix = Matrix.with([
	    		[ 1,		0,	0,	0	],
	    		[ 0,		g1,	0, 	0 	],
	    		[ 0,		0, 	g1,	0	],
	    		[ g0,	0,	0,	g1	]
	    ])
	}

	initPressX { arg angle;
		var g0, g1, g2;

		// build transform matrix, and set for instance
		g0 = 2.sqrt * angle.sin * angle.abs.sin;
		g1 = angle.cos.squared;
		g2 = angle.cos;

	    matrix = Matrix.with([
	    		[ 1,		0,	0,	0	],
	    		[ g0,	g1,	0,	0	],
	    		[ 0,		0,	g2, 	0 	],
	    		[ 0,		0,	0, 	g2	]
	    ])
	}

	initPressY { arg angle;
		var g0, g1, g2;

		// build transform matrix, and set for instance
		g0 = 2.sqrt * angle.sin * angle.abs.sin;
		g1 = angle.cos.squared;
		g2 = angle.cos;

	    matrix = Matrix.with([
	    		[ 1,		0,	0,	0	],
	    		[ 0,		g2,	0, 	0 	],
	    		[ g0,	0,	g1,	0	],
	    		[ 0,		0,	0, 	g2	]
	    ])
	}

	initPressZ { arg angle;
		var g0, g1, g2;

		// build transform matrix, and set for instance
		g0 = 2.sqrt * angle.sin * angle.abs.sin;
		g1 = angle.cos.squared;
		g2 = angle.cos;

	    matrix = Matrix.with([
	    		[ 1,		0,	0,	0	],
	    		[ 0,		g2,	0, 	0 	],
	    		[ 0,		0, 	g2,	0	],
	    		[ g0,	0,	0,	g1	]
	    ])
	}

	initAsymmetry { arg angle;
		var g0, g1, g2, g3, g4;

		// build transform matrix, and set for instance
		g0 = angle.sin.neg;
		g1 = angle.sin.squared;
		g2 = angle.cos.squared;
		g3 = angle.cos * angle.sin;
		g4 = angle.cos;

	    matrix = Matrix.with([
	    		[ 1,			    0, 2.sqrt.reciprocal*g0, 0 ],
	    		[ 2.sqrt*g1,	   g2, g0,				 0 ],
	    		[ 2.sqrt.neg*g3, g3, g4, 				 0 ],
	    		[ 0,			   0,  0, 				g4 ]
	    ])
	}

	initRTT { arg rotAngle, tilAngle, tumAngle;

		matrix = (
		    	FoaXformerMatrix.newTumble(tumAngle).matrix *
		    	FoaXformerMatrix.newTilt(tilAngle).matrix *
		    	FoaXformerMatrix.newRotate(rotAngle).matrix
	    	)
	}

	initMirror { arg theta, phi;

		matrix = (
		    	FoaXformerMatrix.newRotate(theta).matrix *
		    	FoaXformerMatrix.newTumble(phi).matrix *
		    	FoaXformerMatrix.newMirrorX.matrix *
		    	FoaXformerMatrix.newTumble(phi.neg).matrix *
		    	FoaXformerMatrix.newRotate(theta.neg).matrix
	    	)
	}

	initDirect { arg angle, theta, phi;

		matrix = (
		    	FoaXformerMatrix.newRotate(theta).matrix *
		    	FoaXformerMatrix.newTumble(phi).matrix *
		    	FoaXformerMatrix.newDirectX(angle).matrix *
		    	FoaXformerMatrix.newTumble(phi.neg).matrix *
		    	FoaXformerMatrix.newRotate(theta.neg).matrix
	    	)
	}

	initDominate { arg gain, theta, phi;

		matrix = (
		    	FoaXformerMatrix.newRotate(theta).matrix *
		    	FoaXformerMatrix.newTumble(phi).matrix *
		    	FoaXformerMatrix.newDominateX(gain).matrix *
		    	FoaXformerMatrix.newTumble(phi.neg).matrix *
		    	FoaXformerMatrix.newRotate(theta.neg).matrix
	    	)
	}

	initZoom { arg angle, theta, phi;

		matrix = (
		    	FoaXformerMatrix.newRotate(theta).matrix *
		    	FoaXformerMatrix.newTumble(phi).matrix *
		    	FoaXformerMatrix.newZoomX(angle).matrix *
		    	FoaXformerMatrix.newTumble(phi.neg).matrix *
		    	FoaXformerMatrix.newRotate(theta.neg).matrix
	    	)
	}

	initFocus { arg angle, theta, phi;

		matrix = (
		    	FoaXformerMatrix.newRotate(theta).matrix *
		    	FoaXformerMatrix.newTumble(phi).matrix *
		    	FoaXformerMatrix.newFocusX(angle).matrix *
		    	FoaXformerMatrix.newTumble(phi.neg).matrix *
		    	FoaXformerMatrix.newRotate(theta.neg).matrix
	    	)
	}

	initPush { arg angle, theta, phi;

		matrix = (
		    	FoaXformerMatrix.newRotate(theta).matrix *
		    	FoaXformerMatrix.newTumble(phi).matrix *
		    	FoaXformerMatrix.newPushX(angle).matrix *
		    	FoaXformerMatrix.newTumble(phi.neg).matrix *
		    	FoaXformerMatrix.newRotate(theta.neg).matrix
	    	)
	}

	initPress { arg angle, theta, phi;

		matrix = (
		    	FoaXformerMatrix.newRotate(theta).matrix *
		    	FoaXformerMatrix.newTumble(phi).matrix *
		    	FoaXformerMatrix.newPressX(angle).matrix *
		    	FoaXformerMatrix.newTumble(phi.neg).matrix *
		    	FoaXformerMatrix.newRotate(theta.neg).matrix
	    	)
	}

	dirInputs { ^this.numInputs.collect({ inf }) }

	dirOutputs { ^this.numOutputs.collect({ inf }) }

	dirChannels { ^this.dirOutputs }

	dim { ^3 }				// all transforms are 3D

	numInputs { ^matrix.cols }

	numOutputs { ^matrix.rows }

	numChannels { ^4 }			// all transforms are 3D

	type { ^'xformer' }

	printOn { arg stream;
		stream << this.class.name << "(" <<* [kind, this.dim, this.numChannels] <<")";
	}
}



//------------------------------------------------------------------------
// kernel decoders

FoaDecoderKernel {
	var <kind, <subjectID;
	var <kernel, kernelBundle, kernelInfo;
	var <dirChannels;
	var <op = 'kernel';
	var <set = 'FOA';


	// *newSpherical { arg subjectID = 0004, kernelSize = 512, server = Server.default;
	// 	^super.newCopyArgs('spherical', subjectID).initKernel(kernelSize, server);
	// }
	//
	// *newListen { arg subjectID = 1002, server = Server.default;
	// 	^super.newCopyArgs('listen', subjectID).initKernel(512, server);
	// }
	//
	// *newCIPIC { arg subjectID = 0021, server = Server.default;
	// 	^super.newCopyArgs('cipic', subjectID).initKernel(256, server);
	// }

	*newSpherical { arg subjectID = 0004, server = Server.default, sampleRate, score;
		^super.newCopyArgs('spherical', subjectID).initKernel(nil, server, sampleRate, score);
	}

	*newListen { arg subjectID = 1002, server = Server.default, sampleRate, score;
		^super.newCopyArgs('listen', subjectID).initKernel(nil, server, sampleRate, score);
	}

	*newCIPIC { arg subjectID = 0021, server = Server.default, sampleRate, score;
		^super.newCopyArgs('cipic', subjectID).initKernel(nil, server, sampleRate, score);
	}

	*newUHJ { arg kernelSize = 512, server = Server.default, sampleRate, score;
		^super.newCopyArgs('uhj', 0).initKernel(kernelSize, server, sampleRate, score);
	}

	initPath {

		var kernelLibPath;
		var decodersPath;
		kernelLibPath = PathName.new(
			Atk.userKernelDir
		);

		if ( kernelLibPath.isFolder.not, {	// is kernel lib installed for all users?
			PathName.new(					// no? set for single user
				Atk.systemKernelDir
			)
		});

		decodersPath	= PathName.new("/FOA/decoders");

		^kernelLibPath +/+ decodersPath +/+ PathName.new(kind.asString)
	}

	initKernel { arg kernelSize, server, sampleRate, score;

		var databasePath, subjectPath;
		var chans;
		var errorMsg;

		if((server.serverRunning.not) && (sampleRate.isNil) && (score.isNil), {
			Error(
				"Please boot server: %, or provide a CtkScore or Score.".format(
					server.name.asString
				)
			).throw
		});

		kernelBundle = [0.0];
		kernelInfo = [];

		// constants
		chans = 2;			// stereo kernel

		// init dirChannels (output channel (speaker) directions) and kernel sr
		if ( kind == 'uhj', {
			dirChannels = [ pi/6, pi.neg/6 ];
			sampleRate = "None";
		}, {
			dirChannels = [ 5/9 * pi, 5/9 * pi.neg ];
			if(sampleRate.isNil, {
				sampleRate = server.sampleRate.asString;
			}, {
				sampleRate = sampleRate.asString;
			});
		});

		// init kernelSize if need be (usually for HRIRs)
		if ( kernelSize == nil, {
			kernelSize = Dictionary.newFrom([
				'None', 512,
				'44100', 512,
				'48000', 512,
				'88200', 1024,
				'96000', 1024,
				'176400', 2048,
				'192000', 2048,
			]).at(sampleRate.asSymbol)
		});


		// init kernel root, generate subjectPath and kernelFiles
		databasePath = this.initPath;

		subjectPath = databasePath +/+ PathName.new(
			sampleRate ++ "/" ++
			kernelSize ++ "/" ++
			subjectID.asString.padLeft(4, "0")
		);


		// attempt to load kernel
		if ( subjectPath.isFolder.not, {	// does kernel path exist?

			case
			// --> missing kernel database
			{ databasePath.isFolder.not }
			{
				errorMsg = "ATK kernel database missing!" +
				"Please install % database.".format(kind)
			}

			// --> unsupported SR
			{ PathName.new(subjectPath.parentLevelPath(2)).isFolder.not }
			{
				"Supported samplerates:".warn;
				PathName.new(subjectPath.parentLevelPath(3)).folders.do({
					arg folder;
					("\t" + folder.folderName).postln;
				});

				errorMsg = "Samplerate = % is not available for".format(sampleRate)
				+
							"% kernel decoder.".format(kind)
			}

			// --> unsupported kernelSize
			{ PathName.new(subjectPath.parentLevelPath(1)).isFolder.not }
			{
				"Supported kernel sizes:".warn;
				PathName.new(subjectPath.parentLevelPath(2)).folders.do({
					arg folder;
					("\t" + folder.folderName).postln;
				});

				errorMsg = "Kernel size = % is not available for".format(kernelSize)
				+
						"% kernel decoder.".format(kind)
			}

			// --> unsupported subject
			{ subjectPath.isFolder.not }
			{
				"Supported subjects:".warn;
				PathName.new(subjectPath.parentLevelPath(1)).folders.do({
					arg folder;
					("\t" + folder.folderName).postln;
				});

				errorMsg = "Subject % is not available for".format(subjectID)
				+
						"% kernel decoder.".format(kind)
			};

			// throw error!
			"\n".post;
			Error(errorMsg).throw
		}, {
			score.isNil.if({
				if ( server.serverRunning.not, {		// is server running?

					// throw server error!
					Error(
						"Please boot server: %. Encoder kernel failed to load.".format(
							server.name.asString
						)
					).throw
				}, {
					// Else... everything is fine! Load kernel.
					kernel = subjectPath.files.collect({ arg kernelPath;
						chans.collect({ arg chan;
							Buffer.readChannel(server, kernelPath.fullPath, channels: [chan],
								action: { arg buf;
									(
										kernelBundle = kernelBundle.add(
							["/b_allocReadChannel", buf.bufnum, kernelPath.fullPath, 0, kernelSize, chan]
						);
										kernelInfo = kernelInfo.add([kernelPath.fullPath, buf.bufnum, [chan]]);
										"Kernel %, channel % loaded.".format(
											kernelPath.fileName, chan
										)
									).postln
								}
							)
						})
					})
				})
			});

			score.isKindOf(CtkScore).if({
				kernel = subjectPath.files.collect({ arg kernelPath;
					chans.collect({ arg chan;
						var buf = CtkBuffer(kernelPath.fullPath, channels: [chan]);
						kernelInfo = kernelInfo.add([kernelPath.fullPath, buf.bufnum, [chan]]);
						score.add(buf);
						buf;
					})
				})
			});

			score.isKindOf(Score).if({
				kernel = subjectPath.files.collect({ arg kernelPath;
					chans.collect({ arg chan;
						var buf;
						buf = Buffer(server, kernelSize);
						kernelBundle = kernelBundle.add(
							["/b_allocReadChannel", buf.bufnum, kernelPath.fullPath, 0, kernelSize, chan]
						);
						kernelInfo = kernelInfo.add([kernelPath.fullPath, buf.bufnum, [chan]]);
						buf;
					})
				});
				score.add(kernelBundle)
			});

			(score.isKindOf(CtkScore).not && score.isKindOf(Score).not && score.notNil).if( {
				Error(
					"Score is not a Score or a CtkScore. Score is a %.".format(
						score.class.asString
					)
				).throw

			})


		})
	}

	free {
		var path;
		kernel.shape.at(0).do({ arg i;
			kernel.shape.at(1).do({ arg j;
				path = kernel.at(i).at(j).path;
				kernel.at(i).at(j).free;
				(
					"Kernel %, channel % freed.".format(
						PathName.new(path).fileName, j
					)
				).postln
			})
		})
	}

	buffers { ^kernel.flat }

	kernelInfo { ^kernelInfo }

	kernelBundle { ^kernelBundle }

	dim { ^kernel.shape.at(0) - 1}

	numChannels { ^kernel.shape.at(1) }

	kernelSize { ^kernel.at(0).at(0).numFrames }

	numOutputs { ^kernel.shape.at(1) }

	dirOutputs { ^dirChannels }

	numInputs { ^kernel.shape.at(0) }

	dirInputs { ^this.numInputs.collect({ inf }) }

	type { ^'decoder' }

	printOn { arg stream;
		stream << this.class.name << "(" <<*
			[kind, this.dim, this.numChannels, subjectID, this.kernelSize] <<")";
	}
}


//------------------------------------------------------------------------
// kernel encoders

FoaEncoderKernel {
	var <kind, <subjectID;
	var <kernel, kernelBundle, kernelInfo;
	var <dirChannels;
	var <op = 'kernel';
	var <set = 'FOA';

	*newUHJ { arg kernelSize = nil, server = Server.default, sampleRate, score;
		^super.newCopyArgs('uhj', 0).initKernel(kernelSize, server, sampleRate, score);
	}

	*newSuper { arg kernelSize = nil, server = Server.default, sampleRate, score;
		^super.newCopyArgs('super', 0).initKernel(kernelSize, server, sampleRate, score);
	}

	*newSpread { arg subjectID = 0006, kernelSize = 2048, server = Server.default, sampleRate, score;
		^super.newCopyArgs('spread', subjectID).initKernel(kernelSize, server, sampleRate, score);
	}

	*newDiffuse { arg subjectID = 0003, kernelSize = 2048, server = Server.default, sampleRate, score;
		^super.newCopyArgs('diffuse', subjectID).initKernel(kernelSize, server, sampleRate, score);
	}

	// Encoding via Isophonics Room Impulse Response Data Set, not yet implemented.
	// (http://isophonics.net/content/room-impulse-response-data-set)
	//
	// NOTE: Convolution2 doesn't support large, arbitrary sized kernels.

//	*newGreatHall { arg subjectID = "x06y02", server = Server.default;
//		^super.newCopyArgs('greathall', subjectID).initKernel("None", server);
//	}
//
//	*newOctagon { arg subjectID = "x06y02", server = Server.default;
//		^super.newCopyArgs('octagon', subjectID).initKernel("None", server);
//	}
//
//	*newClassroom { arg subjectID = "x30y10", server = Server.default;
//		^super.newCopyArgs('classroom', subjectID).initKernel("None", server);
//	}

	initPath {

		var kernelLibPath;
		var encodersPath;

		kernelLibPath = PathName.new(Atk.userKernelDir);

		if ( kernelLibPath.isFolder.not, {	// is kernel lib installed for all users?
			PathName.new(					// no? set for single user
				Atk.systemKernelDir)
		});

		encodersPath	= PathName.new("/FOA/encoders");

		^kernelLibPath +/+ encodersPath +/+ PathName.new(kind.asString)
	}

	initKernel { arg kernelSize, server, sampleRate, score;

		var databasePath, subjectPath;
		var chans;
		var errorMsg;

		if((server.serverRunning.not) && (sampleRate.isNil) && (score.isNil), {
			Error(
				"Please boot server: %, or provide a CtkScore or Score.".format(
					server.name.asString
				)
			).throw
		});


		kernelBundle = [0.0];
		kernelInfo = [];

		// init dirChannels (output channel (speaker) directions) and kernel sr
		switch ( kind,
			'super', {
				dirChannels = [ pi/4, pi.neg/4 ];	 // approx, doesn't include phasiness
				sampleRate = "None";
				chans = 3;					// [w, x, y]
			},
			'uhj', {
				dirChannels = [ inf, inf ];
				if(sampleRate.isNil, {
					sampleRate = server.sampleRate.asString;
				}, {
					sampleRate = sampleRate.asString;
				});
				chans = 3;					// [w, x, y]
			},
			'spread', {
				dirChannels = [ inf ];
				if(sampleRate.isNil, {
					sampleRate = server.sampleRate.asString;
				}, {
					sampleRate = sampleRate.asString;
				});
				chans = 4;					// [w, x, y, z]
			},
			'diffuse', {
				dirChannels = [ inf ];
				sampleRate = "None";
				chans = 4;					// [w, x, y, z]
			}

			// Encoding via Isophonics Room Impulse Response Data Set, not yet implemented.
			// (http://isophonics.net/content/room-impulse-response-data-set)
			//
			// NOTE: Convolution2 doesn't support large, arbitrary sized kernels.

			//			},
			//			'greathall', {
			//				dirChannels = [ inf ];
			//				sampleRate = server.sampleRate.asString;
			//				chans = 4;					// [w, x, y, z]
			//			},
			//			'octagon', {
			//				dirChannels = [ inf ];
			//				sampleRate = server.sampleRate.asString;
			//				chans = 4;					// [w, x, y, z]
			//			},
			//			'classroom', {
			//				dirChannels = [ inf ];
			//				sampleRate = server.sampleRate.asString;
			//				chans = 4;					// [w, x, y, z]
			//			}
		);

		// init kernelSize if need be
		if ( kernelSize == nil, {
			kernelSize = Dictionary.newFrom([
				'None', 512,
				'44100', 512,
				'48000', 512,
				'88200', 1024,
				'96000', 1024,
				'176400', 2048,
				'192000', 2048,
			]).at(sampleRate.asSymbol)
		});


		// init kernel root, generate subjectPath and kernelFiles
		databasePath = this.initPath;

		subjectPath = databasePath +/+ PathName.new(
			sampleRate ++ "/" ++
			kernelSize ++ "/" ++
			subjectID.asString.padLeft(4, "0")
		);

		// attempt to load kernel

		if ( subjectPath.isFolder.not, {	// does kernel path exist?

			case
			// --> missing kernel database
			{ databasePath.isFolder.not }
			{
				errorMsg = "ATK kernel database missing!" +
				"Please install % database.".format(kind)
			}

			// --> unsupported SR
			{ PathName.new(subjectPath.parentLevelPath(2)).isFolder.not }
			{
				"Supported samplerates:".warn;
				PathName.new(subjectPath.parentLevelPath(3)).folders.do({
					arg folder;
					("\t" + folder.folderName).postln;
				});

				errorMsg = "Samplerate = % is not available for".format(sampleRate)
				+
				"% kernel encoder.".format(kind)
			}

			// --> unsupported kernelSize
			{ PathName.new(subjectPath.parentLevelPath(1)).isFolder.not }
			{
				"Supported kernel sizes:".warn;
				PathName.new(subjectPath.parentLevelPath(2)).folders.do({
					arg folder;
					("\t" + folder.folderName).postln;
				});

				errorMsg = "Kernel size = % is not available for".format(kernelSize)
				+
				"% kernel encoder.".format(kind)
			}

			// --> unsupported subject
			{ subjectPath.isFolder.not }
			{
				"Supported subjects:".warn;
				PathName.new(subjectPath.parentLevelPath(1)).folders.do({
					arg folder;
					("\t" + folder.folderName).postln;
				});

				errorMsg = "Subject % is not available for".format(subjectID)
				+
				"% kernel encoder.".format(kind)
			};

			// throw error!
			"\n".post;
			Error(errorMsg).throw
		}, {
			score.isNil.if( {
				if ( server.serverRunning.not, {		// is server running?

					// throw server error!
					Error(
						"Please boot server: %. Encoder kernel failed to load.".format(
							server.name.asString
						)
					).throw
				}, {
					// Else... everything is fine! Load kernel.
					kernel = subjectPath.files.collect({ arg kernelPath;
						chans.collect({ arg chan;
							Buffer.readChannel(server, kernelPath.fullPath, channels: [chan],
								action: { arg buf;
									(
										kernelBundle = kernelBundle.add(
							["/b_allocReadChannel", buf.bufnum, kernelPath.fullPath, 0, kernelSize, chan]
						);
										kernelInfo = kernelInfo.add([kernelPath.fullPath, buf.bufnum, [chan]]);
										"Kernel %, channel % loaded.".format(
											kernelPath.fileName, chan
										)
									).postln
								}
							)
						})
					})
				})
			});
			score.isKindOf(CtkScore).if({
				kernel = subjectPath.files.collect({ arg kernelPath;
					chans.collect({ arg chan;
						var buf = CtkBuffer(kernelPath.fullPath, channels: [chan]);
						kernelInfo = kernelInfo.add([kernelPath.fullPath, buf.bufnum, [chan]]);
						score.add(buf);
						buf;
					})
				})
			});

			score.isKindOf(Score).if({
				kernel = subjectPath.files.collect({ arg kernelPath;
					chans.collect({ arg chan;
						var buf;
						buf = Buffer(server, kernelSize);
						kernelBundle = kernelBundle.add(
							["/b_allocReadChannel", buf.bufnum, kernelPath.fullPath, 0, kernelSize, chan]
						);
						kernelInfo = kernelInfo.add([kernelPath.fullPath, buf.bufnum, [chan]]);
						buf;
					})
				});
				score.add(kernelBundle)
			});

			(score.isKindOf(CtkScore).not && score.isKindOf(Score).not && score.notNil).if({
				Error(
					"Score is not a Score or a CtkScore. Score is a %.".format(
						score.class.asString
					)
				).throw

			});


		})
	}

	free {
		var path;
		kernel.shape.at(0).do({ arg i;
			kernel.shape.at(1).do({ arg j;
				path = kernel.at(i).at(j).path;
				kernel.at(i).at(j).free;
				(
					"Kernel %, channel % freed.".format(
						PathName.new(path).fileName, j
					)
				).postln
			})
		})
	}

	buffers { ^kernel.flat }

	kernelInfo { ^kernelInfo }

	kernelBundle { ^kernelBundle }

	dim { ^kernel.shape.at(1) - 1}

	numChannels { ^kernel.shape.at(0) }

	kernelSize { ^kernel.at(0).at(0).numFrames }

	numOutputs { ^kernel.shape.at(1) }

	dirInputs { ^dirChannels }

	numInputs { ^kernel.shape.at(0) }

	dirOutputs { ^this.numOutputs.collect({ inf }) }

	type { ^'encoder' }

	printOn { arg stream;
		stream << this.class.name << "(" <<*
			[kind, this.dim, this.numChannels, subjectID, this.kernelSize] <<")";
	}
}
