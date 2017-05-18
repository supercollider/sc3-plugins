/*
	Copyright the ATK Community, Joseph Anderson, and Josh Parmenter, 2011
		J Anderson	j.anderson[at]ambisonictoolkit.net
		J Parmenter	j.parmenter[at]ambisonictoolkit.net


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
// 	Class (UGen superclass): Foa
//
// 	Class (UGen): FoaPanB
//
// 	Class (UGen): FoaDirectO
// 	Class (UGen): FoaDirectX
// 	Class (UGen): FoaDirectZ
// 	Class (UGen): FoaRotate
// 	Class (UGen): FoaTilt
// 	Class (UGen): FoaTumble
// 	Class (UGen): FoaFocusX
// 	Class (UGen): FoaFocusY
// 	Class (UGen): FoaFocusZ
// 	Class (UGen): FoaPushX
// 	Class (UGen): FoaPushY
// 	Class (UGen): FoaPushZ
// 	Class (UGen): FoaPressX
// 	Class (UGen): FoaPressY
// 	Class (UGen): FoaPressZ
// 	Class (UGen): FoaZoomX
// 	Class (UGen): FoaZoomY
// 	Class (UGen): FoaZoomZ
// 	Class (UGen): FoaDominateX
// 	Class (UGen): FoaDominateY
// 	Class (UGen): FoaDominateZ
// 	Class (UGen): FoaAsymmetry
//
// 	Class: FoaRTT
// 	Class: FoaMirror
// 	Class: FoaDirect
// 	Class: FoaDominate
// 	Class: FoaZoom
// 	Class: FoaFocus
// 	Class: FoaPush
// 	Class: FoaPress
//
// 	Class (UGen): FoaProximity
// 	Class (UGen): FoaNFC
// 	Class (UGen): FoaPsychoShelf
//
// 	Class: AtkMatrixMix
// 	Class: AtkKernelConv
//
// 	Class: FoaDecode
// 	Class: FoaEncode
//
// 	Class: FoaXform
// 	Class: FoaTransform
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

Atk {
	classvar <userSupportDir, <userSoundsDir, <userKernelDir, <userMatrixDir, <userExtensionsDir;
	classvar <systemSupportDir, <systemSoundsDir, <systemKernelDir, <systemMatrixDir, <systemExtensionsDir;
	classvar <sets;

	*initClass {
		userSupportDir = Platform.userAppSupportDir.dirname ++ "/ATK";
		userSoundsDir = userSupportDir ++ "/sounds";
		userKernelDir = userSupportDir ++ "/kernels";
		userMatrixDir = userSupportDir ++ "/matrices";
		userExtensionsDir = userSupportDir ++ "/extensions";

		systemSupportDir = Platform.systemAppSupportDir.dirname ++ "/ATK";
		systemSoundsDir = systemSupportDir ++ "/sounds";
		systemKernelDir = systemSupportDir ++ "/kernels";
		systemMatrixDir = systemSupportDir ++ "/matrices";
		systemExtensionsDir = systemSupportDir ++ "/extensions";

		// supported sets
		sets = [ 'FOA', 'HOA1', 'HOA2', 'HOA3', 'HOA4', 'HOA5'];

		if (\Matrix.asClass.isNil) {
			"Please run Quarks.install(\"MathLib\") and recompile the class library."
			" It is a dependency for the Ambisonic Toolkit (ATK).".warn;
		};
	}

	*userSupportDir_ {arg userSupportDirIn;
		userSupportDir = userSupportDirIn;
		userSoundsDir = userSupportDir ++ "/sounds";
		userKernelDir = userSupportDir ++ "/kernels";
		userMatrixDir = userSupportDir ++ "/matrices";
		userExtensionsDir = userSupportDir ++ "/extensions";
	}

	*systemSupportDir_ {arg systemSupportDurIn;
		systemSupportDir = systemSupportDurIn;
		systemSoundsDir = systemSupportDir ++ "/sounds";
		systemKernelDir = systemSupportDir ++ "/kernels";
		systemMatrixDir = systemSupportDir ++ "/matrices";
		systemExtensionsDir = systemSupportDir ++ "/extensions";
	}

	*openUserSupportDir {
		File.exists(Atk.userSupportDir).if({
			Atk.userSupportDir.openOS;
		}, {
			"User Support Dir may not exist. Run \n\tAtk.createUserSupportDir\nto create it".warn
		})
	}

	*createUserSupportDir {
		File.mkdir(Atk.userSupportDir);
	}

	*createSystemSupportDir {
		File.mkdir(Atk.systemSupportDir);
	}

	*openSystemSupportDir {
		File.exists(Atk.systemSupportDir).if({
			Atk.systemSupportDir.openOS;
		}, {
			"System Support Dir may not exist.".warn
		})
	}

	*createExtensionsDir {
		var exists=false, dir, ops, mtxTypes, makeDirs;

		ops =	["kernels", "matrices"];  // i.e., ops
		mtxTypes =	["encoders", "decoders", "xformers"];  // i.e., types

		makeDirs = { |baseDir|
			Atk.sets.do{ |set|
				var path;
				ops.do{ |op|
					mtxTypes.do{ |mtxType|
						path = baseDir +/+ op +/+ set.asString +/+ mtxType;
						File.mkdir( path );
}
				}
			}
		};

		if( File.exists(userExtensionsDir) ) {
			exists = true;
			dir = userExtensionsDir;
		};

		if( File.exists(systemExtensionsDir) ) {
			exists = true;
			dir = userExtensionsDir;
		};

		if (exists) { ^format("ATK extensions directory already found at: %\n", dir).warn; };

		if (File.exists( userSupportDir )) { // try user dir first
			makeDirs.(Atk.userExtensionsDir);
		} {
			if (File.exists( systemSupportDir )) { // then system dir
				makeDirs.(systemExtensionsDir);
			} {
				format("No /ATK directory found in\n\t%\nor\n\t%\n",
					Platform.userAppSupportDir.dirname,
					Platform.systemAppSupportDir.dirname
				).throw
			};
		};
	}

	// op: 'matrices', 'kernels'
	*getAtkOpPath { arg op, isExtension=false;
		var str, subPath, kindPath, fullPath, tested;

		tested = List();

		str = switch (op.asSymbol,
			'matrices', {"/matrices"},
			'kernels',  {"/kernels"},
			// include singular
			'matrix',   {"/matrices"},
			'kernel',   {"/kernels"}
		);

		// assume user directory first
		subPath = PathName.new(
			if (isExtension) {
				Atk.userExtensionsDir ++ str
			} {
				Atk.userSupportDir ++ str
			}
		);

		tested.add(subPath);

		if ( subPath.isFolder.not, { // is  lib installed for user?
			subPath = PathName.new(  // no? check for system wide install
				if (isExtension) {
					Atk.systemExtensionsDir ++ str
				} {
					Atk.systemSupportDir ++ str
				}
			);
			tested.add(subPath);
		});

		if ( subPath.isFolder.not, {
			Error(
				format("\nNo folder found in\n\t%\nor\n\t%\n", *tested)
			).throw;
		});

		^subPath
	}

	//  set: 'FOA', 'HOA1', 'HOA2', etc
	//  type: 'decoder(s)', 'encoder(s)', 'xformer(s)'
	//  op: 'matrices', 'kernels'
	*getExtensionSubPath { arg set, type, op;
		var subPath, typePath, fullPath;

		Atk.checkSet(set);

		subPath = Atk.getAtkOpPath(op, isExtension:true);

		typePath = PathName.new(
			set.asString.toUpper ++ "/" ++ // folder structure is uppercase
			switch( type.asSymbol,
				'decoders', {"decoders"},
				'encoders', {"encoders"},
				'xformers', {"xformers"},
				// include singular
				'decoder', {"decoders"},
				'encoder', {"encoders"},
				'xformer', {"xformers"}
			);
		);

		fullPath = subPath +/+ typePath;
		Atk.folderExists(fullPath); // throws on fail
		^fullPath
	}

	//  set: 'FOA', 'HOA1', 'HOA2', etc
	//  type: 'decoder(s)', 'encoder(s)', 'xformer(s)'
	//  op: 'matrices', 'kernels'
	*getAtkOpSubPath { arg set, type, op;
		var subPath, typePath, fullPath;

		Atk.checkSet(set);

		subPath = Atk.getAtkOpPath(op, isExtension:false);

		typePath = PathName.new(
			set.asString.toUpper ++ "/" ++ // folder structure is uppercase
			switch( type.asSymbol,
				'decoders', {"decoders"},
				'encoders', {"encoders"},
				'xformers', {"xformers"},
				// include singular
				'decoder', {"decoders"},
				'encoder', {"encoders"},
				'xformer', {"xformers"}
			);
		);

		fullPath = subPath +/+ typePath;
		Atk.folderExists(fullPath); // throws on fail
		^fullPath
	}

	// shortcuts for matrices and kernels, aka 'ops'
	*getMatrixExtensionSubPath { arg set, type;
		type ?? {Error("Unspecified matrix type. Please specify 'encoder', 'decoder', or 'xformer'.").errorString.postln; ^nil};
		^Atk.getExtensionSubPath(set, type, 'matrices');
	}

	*getKernelExtensionSubPath { arg set, type;
		type ?? {Error("Unspecified kernel type. Please specify 'encoder', 'decoder', or 'xformer'.").errorString.postln; ^nil};
		^Atk.getExtensionSubPath(set, type, 'kernels');
	}

	*getAtkMatrixSubPath { arg set, type;
		type ?? {Error("Unspecified matrix type. Please specify 'encoder', 'decoder', or 'xformer'.").errorString.postln; ^nil};
		^Atk.getAtkOpSubPath(set, type, 'matrices');
	}

	*getAtkKernelSubPath { arg set, type;
		type ?? {Error("Unspecified matrix type. Please specify 'encoder', 'decoder', or 'xformer'.").errorString.postln; ^nil};
		^Atk.getAtkOpSubPath(set, type, 'kernels');
	}

	*folderExists { |folderPathName, throwOnFail=true|
		if (folderPathName.isFolder) {
			^true
		} {
			if (throwOnFail) {
				Error(
					format("No directory found at\n\t%\n", folderPathName.fullPath)
				).throw
			} { ^false }
		};
	}


	// NOTE: could be generalized for other user extensions, e.g. kernels, etc.
	*resolveMtxPath { arg filePathOrName, mtxType, searchExtensions=false;
		var usrPN, srcPath, relPath, mtxDirPath;
		var hasExtension, hasRelPath;
		var name, matches;
		var relWithoutLast;
		var foundCnt;
		var str;

		usrPN = PathName( filePathOrName ); // as PathName

		if (usrPN.isFile,
			{	// valid absolute path, easy!
				srcPath = usrPN;
			}, {
				hasExtension = usrPN.extension.size > 0;
				hasRelPath = usrPN.colonIndices.size > 0;

				mtxDirPath = if (searchExtensions) {
					Atk.getMatrixExtensionSubPath('FOA', mtxType);  // hard coded to 'FOA'..
				} {
					Atk.getAtkMatrixSubPath('FOA', mtxType);   // .. for now
				};

				relPath = mtxDirPath +/+ usrPN;

				if (hasRelPath,
					{	// search specific path within matrix directory
						if (hasExtension, {

							if( relPath.isFile, {
								srcPath = relPath;  // valid relative path, with file extension
							},{
								Error(format("No file found at\n\t%\n", relPath)).throw;
							});

						}, { // user gives a path, but no file extension

							relWithoutLast = PathName( relPath.fullPath.dirname );

							if (relWithoutLast.isFolder, // test enclosing folder
								{
									foundCnt = 0;
									name = usrPN.fileNameWithoutExtension;
									// NOTE: filesDo searches recursively in the parent folder,
									// so keep track of matches in case there are multiple
									relWithoutLast.filesDo{
										|file|
										if (file.fileNameWithoutExtension == name, {
											srcPath = file;
											foundCnt = foundCnt+1;
										});
									};

									if (foundCnt >1) {
										Error( format(
											"Found multiple matches in recursive search of\n\t%\nPlease provide a more specific path",
											relWithoutLast.fullPath
										) ).throw;
									};

								},{
									Error( format(
										"Parent directory isn't a folder:\n\t%\n",
										relWithoutLast.fullPath )
									).throw;
								}
							)
						}
						);
					}, {	// single filename, no other path
						matches = [];

						// name = usrPN.fileNameWithoutExtension;
						name = usrPN.fileName;

						// recursively search whole directory
						mtxDirPath.filesDo { |file|
							var test;
							test = if (hasExtension) {file.fileName} {file.fileNameWithoutExtension};
							if (test == name, { matches  = matches.add(file) });
						};

						case
						{ matches.size == 1 } { srcPath = matches[0] }
						{ matches.size == 0 } { Error( format("No file found for %", name) ).throw }
						{ matches.size   > 1 } {
							str = format("Multiple matches found for filename:\t%\n", usrPN.fileName);
							matches.do{|file| str = str ++ "\t" ++ file.asRelativePath( mtxDirPath ) ++ "\n" };
							str = str ++ format(
								"Provide either an absolute path to the matrix, or one relative to\n\t%\n",
								mtxDirPath);
							Error( str ).throw;
						};
				});
			}
		);

		if( srcPath.notNil,
			{
				// postf("Found matrix file: \n\t> %\n\t> %\n", srcPath.asRelativePath(mtxDirPath), srcPath);
				^srcPath
			},{ Error("No matrix file found!").throw }
		);
	}

	*checkSet { |set|
		Atk.sets.includes(set.asString.toUpper.asSymbol).not.if {^Error("Invalid set").throw};
	}


	// NOTE: could be generalized for other user extensions, e.g. kernels, etc.
	// type: 'decoders', 'encoders', 'xformers'
	*postMyMatrices { |set, type|
		var postContents;

		block { |break|

			if (set.isNil) {
				// no set provided, show all sets
				Atk.sets.do{ |thisSet|
					Atk.postMyMatrices(thisSet, type)
				};
				break.()
			} {
				Atk.checkSet(set);
			};

			postf("~ %%% ~\n", set.asString.toUpper, type.notNil.if({" "},{""}), type ?? "");

			postContents = { |folderPN, depth=1|
				var offset, f_offset;
				offset = ("\t"!depth).join;
				f_offset = ("\t"!(depth-1)).join;
				postf("%:: % ::\n", f_offset, folderPN.folderName);

				// folderPN.fileName.postln;
				folderPN.entries.do{ |entry|

					offset = ("\t"!depth).join;
					offset.post;
					entry.isFolder.if(
						{ postContents.(entry, depth+1) },
						{ postf("%%\n", offset, entry.fileName) }
					)
				};
			};

			postContents.(
				type.isNil.if(
					{  Atk.getAtkOpPath('matrices', isExtension:true) +/+ set.asString.toUpper },
					{
						if (
							[
								'decoders', 'encoders', 'xformers',
								'decoder', 'encoder', 'xformer'		// include singular
							].includes(type.asSymbol)
						)
						{ Atk.getMatrixExtensionSubPath(set, type) }
						{ Error("'type' must be 'decoder', 'encoder', 'xformer', or nil (to see all matrix directories)").throw; };
					}
				);

			);
		}
	}

}


FoaPanB : MultiOutUGen {

	*ar { arg in, azimuth=0, elevation=0, mul = 1, add = 0;
		^this.multiNew('audio', in, azimuth, elevation ).madd(mul, add);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}

	checkInputs { ^this.checkNInputs(1) }
}

Foa : MultiOutUGen {

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}

 	checkInputs { ^this.checkNInputs(4) }

	*checkChans {arg in;
		(in.size < 4).if({
			^([in] ++ (4 - in.size).collect({Silent.ar})).flat;
		}, {
			^in
		});
	}

 }

FoaDirectO : Foa {
	*ar { arg in, angle = pi/2, mul = 1, add = 0;
		var w, x, y, z;
		in = this.checkChans(in);
		#w, x, y, z = in;
		^this.multiNew('audio', w, x, y, z, angle).madd(mul, add);
	}
}


FoaDirectX : Foa {
	*ar { arg in, angle = pi/2, mul = 1, add = 0;
		var w, x, y, z;
		in = this.checkChans(in);
		#w, x, y, z = in;
		^this.multiNew('audio', w, x, y, z, angle).madd(mul, add);
	}
}

FoaDirectY : FoaDirectX { }
FoaDirectZ : FoaDirectX { }

FoaRotate : Foa {
	*ar { arg in, angle = 0, mul = 1, add = 0;
		var w, x, y, z;
		in = this.checkChans(in);
		#w, x, y, z = in;
		^this.multiNew('audio', w, x, y, z, angle).madd(mul, add);
	}
}
FoaTilt : FoaRotate { }
FoaTumble : FoaRotate { }

FoaFocusX : FoaRotate { }
FoaFocusY : FoaRotate { }
FoaFocusZ : FoaRotate { }

FoaPushX : FoaRotate { }
FoaPushY : FoaRotate { }
FoaPushZ : FoaRotate { }

FoaPressX : FoaRotate { }
FoaPressY : FoaRotate { }
FoaPressZ : FoaRotate { }

FoaZoomX : FoaRotate { }
FoaZoomY : FoaRotate { }
FoaZoomZ : FoaRotate { }

FoaBalance {
	*ar { arg in, angle = 0, mul = 1, add = 0;
		var w, x, y, z;
		in = this.checkChans(in);
		#w, x, y, z = in;
		^FoaZoomY.ar(w, x, y, z, angle, mul, add);
	}
}


FoaDominateX : Foa {
	*ar { arg in, gain = 0, mul = 1, add = 0;
		var w, x, y, z;
		in = this.checkChans(in);
		#w, x, y, z = in;
		^this.multiNew('audio', w, x, y, z, gain).madd(mul, add);
	}
}

FoaDominateY : FoaDominateX { }
FoaDominateZ : FoaDominateX { }

FoaAsymmetry : FoaRotate { }


FoaRTT {
	*ar { arg in, rotAngle = 0, tilAngle = 0, tumAngle = 0, mul = 1, add = 0;
		in = FoaRotate.ar(in, rotAngle);
		in = FoaTilt.ar(in, tilAngle);
		^FoaTumble.ar(in, tumAngle, mul, add);
	}
}

FoaMirror {
	*ar { arg in, theta = 0, phi = 0, mul = 1, add = 0;
		in = FoaRotate.ar(in, theta.neg);
		in = FoaTumble.ar(in, phi.neg);
		in = FoaXform.ar(in, FoaXformerMatrix.newMirrorX);
		in = FoaTumble.ar(in, phi);
		^FoaRotate.ar(in, theta, mul, add);
	}
}

FoaDirect {
	*ar { arg in, angle = 0, theta = 0, phi = 0, mul = 1, add = 0;

		in = FoaRotate.ar(in, theta.neg);
		in = FoaTumble.ar(in, phi.neg);
		in = FoaDirectX.ar(in, angle);
		in = FoaTumble.ar(in, phi);
		^FoaRotate.ar(in, theta, mul, add);
	}
}

FoaDominate {
	*ar { arg in, gain = 0, theta = 0, phi = 0, mul = 1, add = 0;

		in = FoaRotate.ar(in, theta.neg);
		in = FoaTumble.ar(in, phi.neg);
		in = FoaDominateX.ar(in, gain);
		in = FoaTumble.ar(in, phi);
		^FoaRotate.ar(in, theta, mul, add);
	}
}

FoaZoom {
	*ar { arg in, angle = 0, theta = 0, phi = 0, mul = 1, add = 0;

		in = FoaRotate.ar(in, theta.neg);
		in = FoaTumble.ar(in, phi.neg);
		in = FoaZoomX.ar(in, angle);
		in = FoaTumble.ar(in, phi);
		^FoaRotate.ar(in, theta, mul, add);
	}
}

FoaFocus {
	*ar { arg in, angle = 0, theta = 0, phi = 0, mul = 1, add = 0;

		in = FoaRotate.ar(in, theta.neg);
		in = FoaTumble.ar(in, phi.neg);
		in = FoaFocusX.ar(in, angle);
		in = FoaTumble.ar(in, phi);
		^FoaRotate.ar(in, theta, mul, add);
	}
}

FoaPush {
	*ar { arg in, angle = 0, theta = 0, phi = 0, mul = 1, add = 0;

		in = FoaRotate.ar(in, theta.neg);
		in = FoaTumble.ar(in, phi.neg);
		in = FoaPushX.ar(in, angle);
		in = FoaTumble.ar(in, phi);
		^FoaRotate.ar(in, theta, mul, add);
	}
}

FoaPress {
	*ar { arg in, angle = 0, theta = 0, phi = 0, mul = 1, add = 0;

		in = FoaRotate.ar(in, theta.neg);
		in = FoaTumble.ar(in, phi.neg);
		in = FoaPressX.ar(in, angle);
		in = FoaTumble.ar(in, phi);
		^FoaRotate.ar(in, theta, mul, add);
	}
}


//------------------------------------------------------------------------
// Filters

FoaProximity : Foa {
	*ar { arg in, distance = 1, mul = 1, add = 0;
		var w, x, y, z;
		in = this.checkChans(in);
		#w, x, y, z = in;
		^this.multiNew('audio', w, x, y, z, distance).madd(mul, add);
	}

}

FoaNFC : Foa {
	*ar { arg in, distance = 1, mul = 1, add = 0;
		var w, x, y, z;
		in = this.checkChans(in);
		#w, x, y, z = in;
		^this.multiNew('audio', w, x, y, z, distance).madd(mul, add);
	}

}

FoaPsychoShelf : Foa {
	*ar { arg in, freq = 400, k0 = (3/2).sqrt, k1 = 3.sqrt/2, mul = 1, add = 0;
		var w, x, y, z;
		in = this.checkChans(in);
		#w, x, y, z = in;
		^this.multiNew('audio', w, x, y, z, freq, k0, k1).madd(mul, add);
	}

}


//------------------------------------------------------------------------
// AtkMatrixMix & AtkKernelConv

AtkMatrixMix {
	*ar { arg in, matrix, mul = 1, add = 0;

		var out;

		// wrap input as array if needed, for mono inputs
		in.isArray.not.if({ in = [in] });

		out = Mix.fill( matrix.cols, { arg i; // fill input
			UGen.replaceZeroesWithSilence(
				matrix.flop.asArray.at(i) * in.at(i)
			)
		});

		^out.madd(mul, add)
	}
}

AtkKernelConv {
	*ar { arg in, kernel, mul = 1, add = 0;

		var out;

		// wrap input as array if needed, for mono inputs
		in.isArray.not.if({ in = [in] });

		out = Mix.new(
			kernel.shape.at(0).collect({ arg i;
				kernel.shape.at(1).collect({ arg j;
					Convolution2.ar(
						in.at(i),
						kernel.at(i).at(j),
						framesize: kernel.at(i).at(j).numFrames
					)
				})
			})
		);

		^out.madd(mul, add)
	}
}


//------------------------------------------------------------------------
// Decoder built using AtkMatrixMix & AtkKernelConv

FoaUGen {
	*checkChans {arg in;
		(in.size < 4).if({
			^([in] ++ (4 - in.size).collect({Silent.ar})).flat;
		}, {
			^in
		});
	}

	*argDict { arg ugen, args, argDefaults;
			var index, userDict;
			var ugenKeys;
			var ugenDict;
			// find index dividing ordered and named args
			index = args.detectIndex({arg item; item.isKindOf(Symbol)});

			// find ugen args, drop [ 'this', sig]
			ugenKeys = ugen.class.findRespondingMethodFor(\ar).argNames.drop(2);
			ugenDict = Dictionary.new;
			ugenKeys.do({arg key, i; ugenDict.put(key, argDefaults.at(i))});

			// build user dictionary
			userDict = Dictionary.new(ugenKeys.size);
			(index == nil).not.if({
				userDict = userDict.putAll(Dictionary.newFrom(args[index..]));
			}, {
				index = args.size;
			});
			userDict = userDict.putAll(Dictionary.newFrom((index).collect({arg i;
				[ugenKeys.at(i), args.at(i)]}).flat));

			// merge
			^ugenDict.merge(userDict, {
				arg ugenArg, userArg; (userArg != nil).if({userArg})
			})
		}
}

FoaDecode : FoaUGen {
	*ar { arg in, decoder, mul = 1, add = 0;
		in = this.checkChans(in);

		case
			{ decoder.isKindOf(FoaDecoderMatrix) } {

				if ( decoder.shelfFreq.isNumber, { // shelf filter?
					in = FoaPsychoShelf.ar(in,
						decoder.shelfFreq, decoder.shelfK.at(0), decoder.shelfK.at(1))
				});

				^AtkMatrixMix.ar(in, decoder.matrix, mul, add)
			}
			{ decoder.isKindOf(FoaDecoderKernel) } {
				^AtkKernelConv.ar(in, decoder.kernel, mul, add)
			};
	}
}


//------------------------------------------------------------------------
// Encoder built using AtkMatrixMix & AtkKernelConv

FoaEncode : FoaUGen {
	*ar { arg in, encoder, mul = 1, add = 0;
		var out;

		case
			{ encoder.isKindOf(FoaEncoderMatrix) } {
				out = AtkMatrixMix.ar(in, encoder.matrix, mul, add)
			}
			{ encoder.isKindOf(FoaEncoderKernel) } {
				out = AtkKernelConv.ar(in, encoder.kernel, mul, add)
			};

//		if ( out.size < 4, {			// 1st order, fill missing harms with zeros
//			out = out ++ Silent.ar(4 - out.size)
//		});
		out = this.checkChans(out);
		^out
	}
}


//------------------------------------------------------------------------
// Transformer built using AtkMatrixMix & AtkKernelConv

FoaXform : FoaUGen {
	*ar { arg in, xformer, mul = 1, add = 0;

		var out;
		in = this.checkChans(in);

//		switch ( xformer.class,
//
//			FoaXformerMatrix, {
//				out = AtkMatrixMix.ar(in, xformer.matrix, mul, add)
//			},
//
//			FoaXformerKernel, {
//				out = AtkKernelConv.ar(in, xformer.kernel, mul, add)
//			}
//		);
//
//		^out

		// for now...
		^AtkMatrixMix.ar(in, xformer.matrix, mul, add)
	}
}


//------------------------------------------------------------------------
// Transformer: UGen wrapper
/*
argument key - see helpfile for reasonable values
'rtt' - angle

*/

FoaTransform : FoaUGen {
	*ar { arg in, kind ... args;

		var argDict, argDefaults;
		var ugen;
		in = this.checkChans(in);

//		argDict = { arg ugen, args, argDefaults;
//			var index, userDict;
//			var ugenKeys;
//			var ugenDict;
//			[ugen, args, argDefaults].postln;
//			// find index dividing ordered and named args
//			index = args.detectIndex({arg item; item.isKindOf(Symbol)});
//
//			// find ugen args, drop [ 'this', w, x, y, z ]
//			ugenKeys = ugen.class.findRespondingMethodFor(\ar).argNames.drop(2);
//			ugenDict = Dictionary.new;
//			ugenKeys.do({arg key, i; ugenDict.put(key, argDefaults.at(i))});
//
//			// build user dictionary
//			userDict = Dictionary.new(ugenKeys.size);
//			(index == nil).not.if({
//				userDict = userDict.putAll(Dictionary.newFrom(args[index..]));
//			}, {
//				index = args.size;
//			});
//			userDict = userDict.putAll(Dictionary.newFrom((index).collect({arg i;
//				[ugenKeys.at(i), args.at(i)]}).flat));
//
//			// merge
//			ugenDict.merge(userDict, {
//				arg ugenArg, userArg; (userArg != nil).if({userArg})
//			})
//		};
//

		switch ( kind,

			'rotate', {

				ugen = FoaRotate;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'tilt', {

				ugen = FoaTilt;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'tumble', {

				ugen = FoaTumble;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'directO', {

				ugen = FoaDirectO;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'directX', {

				ugen = FoaDirectX;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'directY', {

				ugen = FoaDirectY;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'directZ', {

				ugen = FoaDirectZ;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'dominateX', {

				ugen = FoaDominateX;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\gain), argDict.at(\mul), argDict.at(\add)
				)
			},

			'dominateY', {

				ugen = FoaDominateY;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\gain), argDict.at(\mul), argDict.at(\add)
				)
			},

			'dominateZ', {

				ugen = FoaDominateZ;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\gain), argDict.at(\mul), argDict.at(\add)
				)
			},

			'zoomX', {

				ugen = FoaZoomX;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'zoomY', {

				ugen = FoaZoomY;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'zoomZ', {

				ugen = FoaZoomZ;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'focusX', {

				ugen = FoaFocusX;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'focusY', {

				ugen = FoaFocusY;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'focusZ', {

				ugen = FoaFocusZ;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'pushX', {

				ugen = FoaPushX;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'pushY', {

				ugen = FoaPushY;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'pushZ', {

				ugen = FoaPushZ;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'pressX', {

				ugen = FoaPressX;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'pressY', {

				ugen = FoaPressY;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'pressZ', {

				ugen = FoaPressZ;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'asymmetry', {

				ugen = FoaAsymmetry;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'balance', {

				ugen = FoaZoomY;
				argDefaults = [0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\mul), argDict.at(\add)
				)
			},

			'rtt', {

				ugen = FoaRTT;
				argDefaults = [0, 0, 0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\rotAngle), argDict.at(\tilAngle), argDict.at(\tumAngle),
					argDict.at(\mul), argDict.at(\add)
				)
			},

			'mirror', {

				ugen = FoaMirror;
				argDefaults = [0, 0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\theta), argDict.at(\phi),
					argDict.at(\mul), argDict.at(\add)
				)
			},

			'direct', {

				ugen = FoaDirect;
				argDefaults = [0, 0, 0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\theta), argDict.at(\phi),
					argDict.at(\mul), argDict.at(\add)
				)
			},

			'dominate', {

				ugen = FoaDominate;
				argDefaults = [0, 0, 0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\gain), argDict.at(\theta), argDict.at(\phi),
					argDict.at(\mul), argDict.at(\add)
				)
			},

			'zoom', {

				ugen = FoaZoom;
				argDefaults = [0, 0, 0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\theta), argDict.at(\phi),
					argDict.at(\mul), argDict.at(\add)
				)
			},

			'focus', {

				ugen = FoaFocus;
				argDefaults = [0, 0, 0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\theta), argDict.at(\phi),
					argDict.at(\mul), argDict.at(\add)
				)
			},

			'push', {

				ugen = FoaPush;
				argDefaults = [0, 0, 0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\theta), argDict.at(\phi),
					argDict.at(\mul), argDict.at(\add)
				)
			},

			'press', {

				ugen = FoaPress;
				argDefaults = [0, 0, 0, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\angle), argDict.at(\theta), argDict.at(\phi),
					argDict.at(\mul), argDict.at(\add)
				)
			},

			'nfc', {

				ugen = FoaNFC;
				argDefaults = [1, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\distance),
					argDict.at(\mul), argDict.at(\add)
				)
			},

			'proximity', {

				ugen = FoaProximity;
				argDefaults = [1, 1, 0];

				argDict = this.argDict(ugen, args, argDefaults);

				^ugen.ar(
					in,
					argDict.at(\distance),
					argDict.at(\mul), argDict.at(\add)
				)
			}
		)
	}
}
