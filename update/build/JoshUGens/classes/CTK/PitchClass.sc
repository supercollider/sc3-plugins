/* For GUIDO objects - no quartertones yet */
	
PitchClass {
	var <note, <acc, <octave, <pitch, <pitchclass, <keynum, <freq;
	classvar notenames, notenums, noteToScale, scaleToNote, accToSize, sizeToAcc, accToGuido;
	// deal with transposition, notenames and PC classes here
	// note and acc are symbols, octave is an integer, where middle c = c4

	*new {arg pitch, octave = 4;
		^super.new.init(pitch, octave);
		}

		
	init {arg argpitch, argoctave;
		this.calcpitch(argpitch, argoctave);
		}
		
	calcpitch {arg thispitch, thisoctave;
		var str;
		octave = thisoctave ?? {octave};
		thispitch.isKindOf(Number).if({
			octave = (thispitch.round/12).floor - 1;
			pitch = notenums[thispitch%12];
			}, {
			pitch = thispitch;
			});	
		str = pitch.asString;
		note = str[0].asSymbol;
		if(str.size > 1, 
			{acc = str[1..str.size-1].asSymbol},
			{pitch = (str ++ "n").asSymbol; acc = \n}
			);
		pitch = pitch.asSymbol;
		pitchclass = notenames[pitch];
		keynum = pitchclass + (12 * (1 + octave));
		freq = keynum.midicps;
		}
	
	pitch_ {arg newpitch, newoctave;
		this.calcpitch(newpitch, newoctave);
		}
		
	guidoString {
		var oct, gacc;
		oct = octave - 3;
		gacc = accToGuido[acc]
		^note.asString++gacc++oct.asInteger;
		}
	// can be a PitchClass or float that represents a keynum (quartertones are okay this way)
	invert {arg center;
		var change;
		center = center.isKindOf(PitchClass).if({
			center.keynum
			}, {
			center ?? {60}
			});
		change = this.keynum - center * 2;
		^this.class.new(this.keynum - change)
		}
		
//	// direction should be \up or \down - aPitchInterval can be an instance of PitchInterval
	// OR an + or - integer (direction can be exculded in this case
	transpose {arg aPitchInterval, direction = \up;
		var startscale, endnote, numnotes, newscale, newoctave, newacc, size, sizeAlter;
		var intervalSize, modIntervalSize, intervalQuality, dir;
		dir = case
			{direction == \up}{1}
			{direction == \down}{-1}
			// if neither, set direction to up and return 1
			{true}{direction = \up; 1};
		aPitchInterval.isKindOf(PitchInterval).if({	
			intervalSize = aPitchInterval.size;
			modIntervalSize = aPitchInterval.mod;
			intervalQuality = aPitchInterval.quality;
			startscale = noteToScale[note];
			numnotes = intervalSize - 1 * dir;
			newscale = (startscale + numnotes);
			newoctave = (newscale / 7).floor + octave.asFloat;
			endnote = scaleToNote[newscale % 7];
			// distance from the 'natural' note
			size = accToSize[acc];

			// need to work in exceptions for scales!
			sizeAlter = 
				case
					{((modIntervalSize == 1) && (intervalQuality == \perf))} 
						{0 * dir}
					{((modIntervalSize == 1) && (intervalQuality == \dim))} 
						{-1 * dir}
					{((modIntervalSize == 1) && (intervalQuality == \aug))} 
						{1 * dir}
					{((modIntervalSize == 2) && (intervalQuality == \dim))}
						{-2 * dir}
					{((modIntervalSize == 2) && (intervalQuality == \minor))}
						{(direction == \up).if({
							case
								{((note == \b) || (note == \e))} {0}
								{true} {-1};
							}, {
							case
								{((note == \c) || (note == \f))} {0}
								{true} {1};
							}
						)}				
					{((modIntervalSize == 2) && (intervalQuality == \major))}
						{(direction == \up).if({
							case
								{((note == \b) || (note == \e))} {1}
								{true} {0};
							}, {
							case
								{((note == \c) || (note == \f))} {-1}
								{true} {0};
							}
						)}				
					{((modIntervalSize == 2) && (intervalQuality == \aug))}
						{(direction == \up).if({
							case
								{((note == \b) || (note == \e))} {2}
								{true} {1};
							}, {
							case
								{((note == \c) || (note == \f))} {-2}
								{true} {-1};
							}
						)}		
					{((modIntervalSize == 3) && (intervalQuality == \dim))}
						{(direction == \up).if({
							case
								{((note == \c) || (note == \f) || (note == \g))} {-2}
								{true} {-1};
							}, {
							case
								{((note == \a) || (note == \b) || (note == \e))} {2}
								{true} {1};
							}
						)}
					{((modIntervalSize == 3) && (intervalQuality == \minor))}
						{(direction == \up).if({
							case
								{((note == \c) || (note == \f) || (note == \g))} {-1}
								{true} {0};
							}, {
							case
								{((note == \a) || (note == \b) || (note == \e))} {1}
								{true} {0};
							}
						)}		
					{((modIntervalSize == 3) && (intervalQuality == \major))}
						{(direction == \up).if({
							case
								{((note == \c) || (note == \f) || (note == \g))} {0}
								{true} {1};
							}, {
							case
								{((note == \a) || (note == \b) || (note == \e))} {0}
								{true} {-1};
							}
						)}
					{((modIntervalSize == 3) && (intervalQuality == \aug))}
						{(direction == \up).if({
							case
								{((note == \c) || (note == \f) || (note == \g))} {1}
								{true} {2};
							}, {
							case
								{((note == \a) || (note == \b) || (note == \e))} {-1}
								{true} {-2};
							}
						)}
					{((modIntervalSize == 4) && (intervalQuality == \dim))}
						{(direction == \up).if({
							case
								{(note == \f)} {-2}
								{true} {-1};
							}, {
							case
								{(note == \b)} {2}
								{true} {1};
							}
						)}
					{((modIntervalSize == 4) && (intervalQuality == \perf))}
						{(direction == \up).if({
							case
								{(note == \f)} {-1}
								{true} {0};
							}, {
							case
								{(note == \b)} {1}
								{true} {0};
							}
						)}
					{((modIntervalSize == 4) && (intervalQuality == \aug))}
						{(direction == \up).if({
							case
								{(note == \f)} {-2}
								{true} {-1};
							}, {
							case
								{(note == \b)} {2}
								{true} {1};
							}
						)}
					{((modIntervalSize == 5) && (intervalQuality == \dim))}
						{(direction == \up).if({
							case
								{(note == \b)} {0}
								{true} {-1};
							}, {
							case
								{(note == \f)} {0}
								{true} {1};
							}
						)}
					{((modIntervalSize == 5) && (intervalQuality == \perf))}
						{(direction == \up).if({
							case
								{(note == \b)} {1}
								{true} {0};
							}, {
							case
								{(note == \f)} {-1}
								{true} {0};
							}
						)}
					{((modIntervalSize == 5) && (intervalQuality == \aug))}
						{(direction == \up).if({
							case
								{(note == \b)} {2}
								{true} {1};
							}, {
							case
								{(note == \f)} {-2}
								{true} {-1};
							}
						)}
					{((modIntervalSize == 6) && (intervalQuality == \dim))}
						{(direction == \up).if({
							case
								{((note == \e) || (note == \a) || (note == \b))} {-1}
								{true} {-2};
							}, {
							case
								{((note == \c) || (note == \f) || (note == \g))} {1}
								{true} {2};
							}
						)}
					{((modIntervalSize == 6) && (intervalQuality == \minor))}
						{(direction == \up).if({
							case
								{((note == \e) || (note == \a) || (note == \b))} {0}
								{true} {-1};
							}, {
							case
								{((note == \c) || (note == \f) || (note == \g))} {0}
								{true} {1};
							}
						)}			
					{((modIntervalSize == 6) && (intervalQuality == \major))}
						{(direction == \up).if({
							case
								{((note == \e) || (note == \a) || (note == \b))} {1}
								{true} {0};
							}, {
							case
								{((note == \c) || (note == \f) || (note == \g))} {-1}
								{true} {0};
							}
						)}		
					{((modIntervalSize == 6) && (intervalQuality == \aug))}
						{(direction == \up).if({
							case
								{((note == \e) || (note == \a) || (note == \b))} {2}
								{true} {1};
							}, {
							case
								{((note == \c) || (note == \f) || (note == \g))} {-2}
								{true} {-1};
							}
						)}	
					{((modIntervalSize == 7) && (intervalQuality == \dim))}
						{(direction == \up).if({
							case
								{((note == \c) || (note == \f))} {-2}
								{true} {-1};
							}, {
							case
								{((note == \b) || (note == \e))} {2}
								{true} {1};
							}
						)}		
					{((modIntervalSize == 7) && (intervalQuality == \minor))}
						{(direction == \up).if({
							case
								{((note == \c) || (note == \f))} {-1}
								{true} {0};
							}, {
							case
								{((note == \b) || (note == \e))} {1}
								{true} {0};
							}
						)}			
					{((modIntervalSize == 7) && (intervalQuality == \major))}
						{(direction == \up).if({
							case
								{((note == \c) || (note == \f))} {0}
								{true} {1};
							}, {
							case
								{((note == \b) || (note == \e))} {0}
								{true} {-1};
							}
						)}	
					{((modIntervalSize == 7) && (intervalQuality == \aug))}
						{(direction == \up).if({
							case
								{((note == \c) || (note == \f))} {1}
								{true} {2};
							}, {
							case
								{((note == \b) || (note == \e))} {-1}
								{true} {-2};
							}
						)}
					{true} {0};
			newacc = sizeToAcc[size + sizeAlter];
			^this.class.new((endnote ++ newacc).asSymbol, newoctave);
			}, {
			^this.class.new(this.keynum + (aPitchInterval * dir))
			})
		}
		
	
	// cn = c, cs = c-sharp, df = d-flat, dqf = d-quarter-flat, cqs = c-quarter-sharp
	// dtqf = d-three-quarter-flat, ctqs = c-three-quarter-sharp
	*initClass {	
		noteToScale = IdentityDictionary[
			\c -> 0,
			\d -> 1,
			\e -> 2,
			\f -> 3,
			\g -> 4,
			\a -> 5,
			\b -> 6
			];
		scaleToNote = IdentityDictionary[
			0 -> \c,
			1 -> \d,
			2 -> \e,
			3 -> \f,
			4 -> \g,
			5 -> \a,
			6 -> \b
			];
		accToSize = IdentityDictionary[
			\ffff -> -4,
			\fff -> -3,
			\ff -> -2,
//			\tqf -> -1.5,
			\f -> -1,
//			\qf -> -0.5,
			\n -> 0,
//			\qs -> 0.5,
			\s -> 1,
//			\tqs -> 1.5,
			\ss -> 2,
			\sss -> 3,
			\ssss -> 4
			];
		sizeToAcc = IdentityDictionary[
			-4 -> \ffff,
			-3 -> \fff,
			-2 -> \ff,
//			-1.5 -> \tqf,
			-1 -> \f,
//			-0.5 -> \qf,
			0 -> \n,
//			0.5 -> \qs,
			1 -> \s,
//			1.5 -> \tqs,
			2 -> \ss,
			3 -> \sss,
			4 -> \ssss
			];
		notenames = IdentityDictionary[
			\bs -> 0,
			\cn -> 0,
			\dff -> 0,
//			\cqs -> 0.5,
//			\dtqf -> 0.5,
			\bss -> 1,
			\cs -> 1,
			\df -> 1,
//			\ctqs -> 1.5,
//			\dqf -> 1.5,
			\css -> 2,
			\dn -> 2,
			\eff -> 2,
//			\dqs -> 2.5,
//			\etqf -> 2.5,
			\ds -> 3,
			\ef -> 3,
//			\dtqs -> 3.5,
//			\eqf -> 3.5,
			\dss -> 4,
			\en -> 4,
			\ff -> 4,
//			\eqs -> 4.5,
//			\fqf -> 4.5,
			\es -> 5,
			\fn -> 5,
			\gff -> 5,
//			\fqs -> 5.5,
//			\gtqf -> 5.5,
			\fs -> 6,
			\gf -> 6,
//			\ftqs -> 6.5,
//			\gqf -> 6.5,
			\fss -> 7,
			\gn -> 7,
			\aff -> 7,
//			\gqs -> 7.5,
//			\atqf -> 7.5,
			\gs -> 8,
			\af -> 8,
//			\gtqs -> 8.5,
//			\aqf -> 8.5,
			\gss -> 9,
			\an -> 9,
			\bff -> 9,
//			\aqs -> 9.5,
//			\btqf -> 9.5,
			\as -> 10,
			\bf -> 10,
//			\atqs -> 10.5,
//			\bqf -> 10.5,
			\ass -> 11,
			\bn -> 11,
			\cf -> 11 
//			\bqs -> 11.5,
//			\cqf -> 11.5
			];
		notenums = Dictionary[
			0 -> \cn,
//			0.5 -> [\c, \qs],
			1 -> \cs,
//			1.5 -> [\c, \tqs],
			2 -> \dn,
//			2.5 -> [\d, \qs],
			3 -> \ef,
//			3.5 -> [\d, \tqs],
			4 -> \en,
//			4.5 -> [\e, \qs],
			5 -> \fn,
//			5.5 -> [\f, \qs],
			6 -> \fs,
//			6.5 -> [\f, \tqs],
			7 -> \gn,
//			7.5 -> [\g, \qs],
			8 -> \af,
//			8.5 -> [\g, \tqs],
			9 -> \an,
//			9.5 -> [\a, \qs],
			10 -> \bf,
//			10.5 -> [\a, \tqs],
			11 -> \bn,
//			11.5 -> [\b, \qs]
			];
		accToGuido = IdentityDictionary[
			\ffff -> "&&&&",
			\fff -> "&&&",
			\ff -> "&&",
//			\tqf -> -1.5,
			\f -> "&",
//			\qf -> -0.5,
			\n -> "",
//			\qs -> 0.5,
			\s -> "#",
//			\tqs -> 1.5,
			\ss -> "##",
			\sss -> "###",
			\ssss -> "####"
			];
		}
}

PitchInterval {
	var <quality, <size, <mod;
	//quality is a symbol of \major, \minor, \perf, \dim or \aug
	// size is a 
	*new {arg quality, size;
		var mod;
		// make sure size is an int, grab its mod
		mod = (size > 7).if({(size.round % 8) + 1}, {size});
		((mod == 1) || (mod == 4) || (mod == 5)).if({
			((quality ==  \perf) || (quality == \aug) || (quality == \dim)).if({
				^super.newCopyArgs(quality, size, mod)
				}, {
				"Unisons, fourths, fifths or octaves need to be \\perf or \\aug".warn;
				^nil;
				})
			}, {
			((mod == 2) || (mod == 3) || (mod == 6) || (mod == 7)).if({
				((quality == \major) || (quality == \minor) || 
					(quality == \dim) || (quality == \aug)).if({
				^super.newCopyArgs(quality, size, mod)
				}, {
				"Seconds, thirds, sixths or sevents need to be \\major, \\minor, \\dim or \\aug".warn;
				^nil;
				})
			})
		})
	}
}

PitchCollection {
	var <pitchCollection, <tonic, <octaveSize, <pitchBase, <sortedBase;
	
	*new {arg pitchCollection, tonic, octaveSize = 12;
		pitchCollection = pitchCollection.asArray;
		tonic = tonic ? pitchCollection[0];
		^super.newCopyArgs(pitchCollection, tonic, octaveSize).init;
		}
		
	init {
		pitchBase = Array.fill(pitchCollection.size, {arg i; pitchCollection[i].pitchclass});
		sortedBase = pitchBase.copy.sort; 
		}
		
	// will make scales and deal with note-names, transposition, string fomratting
	// random filtering
	
	*major {arg tonic;
		var steps, scale, start;
		start = tonic = tonic.notNil.if({
			tonic.isKindOf(PitchClass).if({tonic}, {PitchClass(tonic)})
			}, {
			PitchClass.new(\c)
			});
		steps = [\major, \major, \minor, \major, \major, \major, \minor];
		steps.do{arg me, i;
			var note; 
			note = start; 
			start = start.transpose(PitchInterval(me, 2), \up); 
			steps[i] = note;
			};
		^this.new(steps, tonic, 12)
		}
	
	*minor {arg tonic;
		var steps, scale, start;
		start = tonic = tonic.notNil.if({
			tonic.isKindOf(PitchClass).if({tonic}, {PitchClass(tonic)})
			}, {
			PitchClass.new(\c)
			});
		steps = [\major, \minor, \major, \major, \minor, \major, \major];
		steps.do{arg me, i;
			var note; 
			note = start; 
			start = start.transpose(PitchInterval(me, 2), \up); 
			steps[i] = note;
			};
		^this.new(steps, tonic, 12)
		}
		
	*natMinor {arg tonic;
		^this.minor(tonic);
		}
		
	*harmMinor {arg tonic;
		var steps, scale, start;
		start = tonic = tonic.notNil.if({
			tonic.isKindOf(PitchClass).if({tonic}, {PitchClass(tonic)})
			}, {
			PitchClass.new(\c)
			});
		steps = [\major, \minor, \major, \major, \minor, \aug, \minor];
		steps.do{arg me, i;
			var note; 
			note = start; 
			start = start.transpose(PitchInterval(me, 2), \up); 
			steps[i] = note;
			};
		^this.new(steps, tonic, 12)
		}
		
	*ionian {arg tonic;
		^this.major(tonic);
		}
		
	*dorian {arg tonic;
		var steps, scale, start;
		start = tonic = tonic.notNil.if({
			tonic.isKindOf(PitchClass).if({tonic}, {PitchClass(tonic)})
			}, {
			PitchClass.new(\c)
			});
		steps = [\major, \minor, \major, \major, \major, \minor, \major];
		steps.do{arg me, i;
			var note; 
			note = start; 
			start = start.transpose(PitchInterval(me, 2), \up); 
			steps[i] = note;
			};
		^this.new(steps, tonic, 12)
		}
		
	*phrygian {arg tonic;
		var steps, scale, start;
		start = tonic = tonic.notNil.if({
			tonic.isKindOf(PitchClass).if({tonic}, {PitchClass(tonic)})
			}, {
			PitchClass.new(\c)
			});
		steps = [\minor, \major, \major, \major, \minor, \major, \major];
		steps.do{arg me, i;
			var note; 
			note = start; 
			start = start.transpose(PitchInterval(me, 2), \up); 
			steps[i] = note;
			};
		^this.new(steps, tonic, 12)
		}
		
	*lydian {arg tonic;
		var steps, scale, start;
		start = tonic = tonic.notNil.if({
			tonic.isKindOf(PitchClass).if({tonic}, {PitchClass(tonic)})
			}, {
			PitchClass.new(\c)
			});
		steps = [\major, \major, \major, \minor, \major, \major, \minor];
		steps.do{arg me, i;
			var note; 
			note = start; 
			start = start.transpose(PitchInterval(me, 2), \up); 
			steps[i] = note;
			};
		^this.new(steps, tonic, 12)
		}
	
	*mixolydian {arg tonic;
		var steps, scale, start;
		start = tonic = tonic.notNil.if({
			tonic.isKindOf(PitchClass).if({tonic}, {PitchClass(tonic)})
			}, {
			PitchClass.new(\c)
			});
		steps = [\major, \major, \minor, \major, \major, \minor, \major];
		steps.do{arg me, i;
			var note; 
			note = start; 
			start = start.transpose(PitchInterval(me, 2), \up); 
			steps[i] = note;
			};
		^this.new(steps, tonic, 12)
		}
		
	*aeolian {arg tonic;
		^this.minor(tonic);
		}
		
	*locrian {arg tonic;
		var steps, scale, start;
		start = tonic = tonic.notNil.if({
			tonic.isKindOf(PitchClass).if({tonic}, {PitchClass(tonic)})
			}, {
			PitchClass.new(\c)
			});
		steps = [\minor, \major, \major, \minor, \major, \major, \major];
		steps.do{arg me, i;
			var note; 
			note = start; 
			start = start.transpose(PitchInterval(me, 2), \up); 
			steps[i] = note;
			};
		^this.new(steps, tonic, 12)
		}
		
	*chromatic {arg tonic;
		var start, steps, step, newoctave; 
		tonic = tonic.notNil.if({
			tonic.isKindOf(PitchClass).if({tonic}, {PitchClass(tonic)})
			}, {
			PitchClass.new(\c)
			});
		start = tonic.pitchclass;
		steps = Array.fill(12, {arg i; 
			step = (i+start)%12;
			newoctave = tonic.octave + ((i+start) / 12).floor;
			PitchClass.new(step, octave: newoctave);
			});
		^this.new(steps, tonic, 12);
		}
	
	// takes a keynum, returns the PitchClass closest to that keynum (under octave equivalence)
	filterKeynum {arg keynum;
		var baseKeynum, closestKey, member, thisSortedBase, thisIndex;
		// round out the octaves
		thisSortedBase = sortedBase ++ (sortedBase[0] + octaveSize);
		// get the mod for searching purposes
		baseKeynum = keynum % 12;
		// find the closes keynum's index in the sorted collection
		closestKey = (thisSortedBase).indexIn(baseKeynum);
		// find that closest keynums position in order of the collection
		thisIndex = pitchBase.indexOf(thisSortedBase[closestKey]%12);
		// figure out which note it is
		member = pitchCollection[thisIndex];
		// return a new PitchClass with the appropriate octave... check for nums close
		// to the next octave at C with the round
		^PitchClass.new((member.note ++ member.acc), (keynum.round / 12).floor - 1);
		}
		
	choose {
		^pitchCollection.choose;
		}
		
	wchoose {arg weights;
		^pitchCollection.wchoose(weights);
		}
		
	chunk {arg start = 0, end;
		end = end ?? pitchCollection.size - 1
		^this.class.new(pitchCollection[start..end], tonic, 12);
		}
		
	add {arg aPitchClass;
		^this.class.new(pitchCollection ++ aPitchClass, tonic, 12)
		}
		
	insert {arg position = 0, aPitchClass;
		^this.class.new(pitchCollection.insert(position, aPitchClass).flat, tonic, 12)
		}
		
	transpose {arg aPitchInterval, direction = \up;
		^this.class.new(Array.fill(pitchCollection.size, 
				{arg i; pitchCollection[i].transpose(aPitchInterval, direction)}),
			tonic.transpose(aPitchInterval, direction), octaveSize)
		}
	
	/*	
	*quartertone {arg tonic;
		var start, steps, step, newoctave; 
		start = tonic.pitchclass;
		steps = Array.fill(24, {arg i;
			step = ((i * 0.5)+start)%12;
			newoctave = tonic.octave + (((i*0.5)+start) / 12).floor;
			PitchClass.new(step, octave: newoctave);
			});
		^this.new(steps, tonic, 12);
		}
	*/
		
/*	
	*octatonic {arg tonic = 0; ^this.new([0, 2, 3, 5, 6, 8, 9, 11], tonic)}
	*octatonic013 {arg tonic = 0; ^this.new([0, 1, 3, 4, 6, 7, 9, 10], tonic)}
	*octatonic023 {arg tonic = 0; ^this.octatonic(tonic)}

*/	

}

PC : PitchClass { }
PI : PitchInterval { }
PColl : PitchCollection { }