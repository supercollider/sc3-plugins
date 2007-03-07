GuidoScore {
	// score will be an Dictionary of GuidoVoice objects
	var <>score, file;
	*new {
		^super.new.init;
		}
		
	init {
		score = []; // Dictionary.new;
		}
	
	// add a voice to the score
	add {arg aVoice;
		aVoice.isKindOf(GuidoVoice).if({
			//score = score.add(aVoice.id -> aVoice)
			score = score.add(aVoice);
			}, {
			"GuidoScore can only add GuidoVoices".warn;
			})
		}
		
	remove {arg anID;
		(score.at(anID).notNil).if({
			score.removeAt(anID)}, {
			"No voice with that ID found".warn;
			})
		}
		
	output {arg pathname, mode = "w";
		var string, eventstring;
		file = File.new(pathname, mode);
		file.write("%% SuperCollider output from " ++ Date.localtime ++ "\n");
		file.write("{\n");
		score.do{arg me, i; 
			file.write("%%Voice" ++ i ++ "\n");
			me.output(file); 
			(i != (score.size - 1)).if({file.write(",")});
			};
		file.write("}");
		file.close;
		}
	}

/* 

each GuidoVoice must have a unique id. More then one GuidoVoice can exist on a staff (in other words, two GuidoVoices with different id can share a staffid. To differentiate two lines on one staff, you may want to specify one \stemsUp and the other \stemsDown

clef is \g, \f or \c 
optionally, you can specify a line to place the note indication of the clef on (1 bottom, 5 top)
\g2 is standard treble
\c3 is standard alto
\c4 is tenor

double clefs are allowed
\gg

and these are also valid: \treble, \bass, \alto and \tenor

key can be an integer. 0 is no sharps or flats, -2 is 2 flats, 3 is 3 sharps OR:
	\ef - E Flat minor - where lowercase indicates minot
	\Fs - F sharp major - where upper case indicates major
*/


GuidoVoice {
	var <>id, <instr, <>events, <>clef, <>key, <>timesig, <>stemdir, <>staffid;
	classvar keyConvert;
	// GuidoVoice Objects will be an array Array of GuidoEvents
	*new {arg id, instr, events, clef = "treble", key = 0, timesig, stemdir = \stemsAuto, staffid;
		staffid = staffid ? id;
		timesig = timesig ? GuidoTimeSig([[1, [4, 4]]]);
		events = events ? [];
		^super.newCopyArgs(id, instr, events, clef, key, timesig, stemdir, staffid);
		}
	
	// add anEvent or an array of events
	add {arg anEvent;
		anEvent.isKindOf(Array).if({
			anEvent.do({arg me; events = events.add(me)})
			}, {
			events = events.add(anEvent);
			})
		}
//		
//	sort {arg theseevents;
//		theseevents = theseevents.sort({arg a, b; a.beat < b.beat});
//		^theseevents;
//		}

	sort {
		events = events.sort({arg a, b; a.beat < b.beat});
		}
				
	fillWithRests {
		var curbeat, lastbeat, eventcopy;
		curbeat = 1.0;
		eventcopy = events.copy;
		// check for gaps... fill with rests
		eventcopy.do{arg me;
			(me.beat > curbeat).if({
//				eventcopy = eventcopy.add(GuidoRest(curbeat, me.beat - curbeat));
				this.add(GuidoRest(curbeat, me.beat - curbeat));
				}, {
				(me.beat < curbeat).if({
					"This voice appears to have overlapping events. Voices can't overlap, though two voices may have the same staffid.".warn
					})
				});
			curbeat = me.beat + me.duration;
			};
		this.sort;
		}
	/*	
	formatToMeasures {arg theseevents;
		var currentMeter, currentMeasure, currentBeat, newevents;
		newevents = Array.new;
		currentMeasure = currentBeat = 1;
		currentMeter = timesig.meterAt(currentMeasure);
		// iterate over a voice -
		//	- check if the meter of each measure is the same as the previous measure-
		//			if not, create a insert a new GuidoMeter into the event list
		//	- check the duration of each note, and compare its position in the measure
		//			if its duration is longer then the space in the measure, remove
		//			the note object, and replace it with new objects tied together across
		//			measures
		"not yet built".postln;
		^theseevents;
		}
	*/
	
	formatToMeasures {
		var beatsPerMeasure, check, firstdur, numFullMeasNotes, lastdur, curbeat, notearray;
		beatsPerMeasure = timesig.beatAt(1);
		events.copy.do({arg me;
			check = (((me.beat - 1) % beatsPerMeasure) + me.duration) > beatsPerMeasure;
			check.if({
				curbeat = me.beat;
				events.remove(me);
				firstdur = beatsPerMeasure - ((me.beat - 1) % beatsPerMeasure);
				me.isKindOf(GuidoNote).if({
					notearray = [];
					notearray = notearray.add(GuidoNote(me.note, curbeat, firstdur));
					curbeat = curbeat + firstdur;
					numFullMeasNotes = ((me.duration - firstdur) / beatsPerMeasure).floor;
					numFullMeasNotes.do({arg i;
						notearray = notearray.add(GuidoNote(me.note, curbeat, beatsPerMeasure));
						curbeat = curbeat + beatsPerMeasure;
						});
					lastdur = (me.duration - firstdur) - (numFullMeasNotes * beatsPerMeasure);
					(lastdur > 0).if({
						notearray = notearray.add(GuidoNote(me.note, curbeat, lastdur))
						});
					this.add(GuidoSpanner(notearray, me.beat, \tie));
					}, {
					this.add(GuidoRest(curbeat, firstdur));
					curbeat = curbeat + firstdur;
					numFullMeasNotes = ((me.duration - firstdur) / beatsPerMeasure).floor;
					numFullMeasNotes.do({arg i;
						this.add(GuidoRest(curbeat, beatsPerMeasure));
						curbeat = curbeat + beatsPerMeasure;
						});
					lastdur = (me.duration - firstdur) - (numFullMeasNotes * beatsPerMeasure);
					(lastdur > 0).if({
						this.add(GuidoRest(curbeat, lastdur))
						})
					})
				})
			});
		this.sort;
		}
	
	output {arg file;
		var string, eventstring, initMeter, currentMeter, currentMeasure, theseevents;
		file.write("[\n");
		file.write("\\staff<\""++staffid.asString++"\"> ");
		instr.notNil.if({file.write("\\instr<\""++instr.asString++"\"> ")});
//		file.write("\\" ++ stemdir.asString); // turn on auto beaming for now
		// clef
		file.write("\\clef<\"" ++ clef.asString ++ "\"> ");
		// key
		key.isKindOf(Integer).if({
			file.write("\\key<"++key++"> ");
			}, {
			file.write("\\key<\"" ++ keyConvert[key] ++ "\"> ");
			});
		// meter
		currentMeasure = 1;
		initMeter = currentMeter = timesig.meterAt(currentMeasure);
		file.write("\\meter<\""++initMeter[0]++"/"++initMeter[1]++"\"> ");
		file.write("\\"++stemdir.asString++" ");
		// fill in rests
		this.fillWithRests;
		this.formatToMeasures;
//		theseevents = this.formatToMeasures(theseevents);
		events.do{arg me; me.output(file)};
		file.write("]\n");
		}
		
	*initClass {
		keyConvert = Dictionary[
			\c -> "c",
			\C -> "C",
			\cs -> "c#",
			\Cs -> "C#",
			\df -> "d&",
			\Df -> "D&",
			\d -> "d",
			\D -> "D",
			\ds -> "d#",
			\Ds -> "D#",
			\e -> "e",
			\E -> "E",
			\f -> "f",
			\F -> "F",
			\fs -> "f#",
			\Fs -> "F#",
			\Gf -> "G&",
			\g -> "g",
			\G -> "G",
			\gs -> "g#",
			\Gs -> "G#",
			\af -> "a&",
			\Af -> "A&",
			\a -> "a",
			\A -> "A",
			\bf -> "b&",
			\Bf -> "B&",
			\b -> "b",
			\B -> "B",
			\Cf -> "C&" 
			]
		}
	
	}

GuidoEvent {
	output {arg file;
		var string;
		string = this.outputString;
		file.write(string);
		string.postln;
		}
	}

GuidoNote : GuidoEvent {
	var <>note, <>beat, <>duration, <>marks;
	*new {arg aPitchClass, beat, duration, marks;
		(beat >= 1).if({
			^super.newCopyArgs(aPitchClass, beat, duration, marks.asArray)
			}, {
			"Note events must start at beat 1 or higher".warn;
			^nil
			})
	}
	
	outputString {
		var string, markstring, articulation = 0;
		var rhythm = (duration / 4).asFraction;
		markstring = "";
		marks.do({arg me; me.isKindOf(GuidoArticulation).if({articulation = articulation + 1})});
		(marks.size > 0).if({
			marks.do({arg me; 
					markstring = markstring ++ me.outputString
				})
			});
		string = markstring ++ note.guidoString++"*"++rhythm[0]++"/"++rhythm[1];		articulation.do({string = string ++" )"});
		^"\t"++string ++" \t%% "++ beat ++ "\n";
		}
	}

// No need to actually create GuidoRests on your own.. they will be created for you when a 
// GuidoVoice is written out

GuidoRest : GuidoEvent {
	var <>beat, <>duration;
	*new {arg beat = 1, duration = 1;
		^super.newCopyArgs(beat, duration);
		}
		
	outputString {
		var rhythm;
		rhythm = (duration / 4).asFraction;
		^"\t_"++"*"++rhythm[0]++"/"++rhythm[1]++" \t%% "++beat++"\n"
		}
	}
	
// Use GuidoTimeSig to set up Time signatures. GuidoMeter is used internally for reading 
// GuidoTimeSigs

GuidoMeter : GuidoEvent {
	var <>beat, <>timesig;
	*new {arg beat = 1, timesig = [4, 4];
		^super.newCopyArgs(beat, timesig);
		}
		
	outputString {arg file;
		^"\t\\meter<\""++timesig[0]++"/"++timesig[1]++"\"> \n";
		}
	}



GuidoChord : GuidoEvent {
	var <>notes, <>beat, <>duration, <>marks;
	
	*new {arg aPitchClassArray, beat, duration, marks;
		(beat >= 1).if({
			^super.newCopyArgs(aPitchClassArray, beat, duration, marks.asArray)
			}, {
			"Note events must start at beat 1 or higher".warn;
			^nil
			})
		}
		
	outputString {
		var string, markstring, notestring, numnotes, articulation = 0;
		var rhythm = (duration / 4).asFraction;
		marks.do({arg me; me.isKindOf(GuidoArticulation).if({articulation = articulation + 1})});
		markstring = "";
		numnotes = notes.size;
		notestring = "{ ";
		notes.do({arg me, i;
			notestring = notestring ++ me.guidoString ++ "*"++rhythm[0]++"/"++rhythm[1];
			(i != (numnotes-1)).if({
				notestring = notestring ++ ", ";
				})
			});
		notestring = notestring ++ "} ";
		(marks.size > 0).if({
			marks.do{arg me; markstring = markstring ++ me.outputString}});
		string = markstring ++ notestring;
		articulation.do({string = string ++" )"});
		^"\t"++string ++ "\t%% "++beat++"\n";
		
		}
	}

// A Spanner is a collection of GuidoNotes that are linked through some aspect (for instance, all
// notes in a Spanner may be tied together). These act like chords, but in a horizontal sense
// Spanners will be used internally to create multiple notes out of durations that extend past
// a barline, and possibly for other markings. They will substitute for a GuidoNote call, and should
// be a collection of GuidoNotes (all of the same pitch class).

// arrGuidoNotes = array of GuidoNotes, already sorted in time
// the spanner symbol (e.g. \tie)

GuidoSpanner : GuidoEvent {
	var <>arrGuidoNotes, <>beat, <>spanner;
	
	*new {arg arrGuidoNotes, beat, spanner;
		^super.newCopyArgs(arrGuidoNotes, beat, spanner);
		}
		
	outputString {
		var string;
		string = "\\"++spanner.asString++"(\n";
		arrGuidoNotes.do({arg me;
			string = string ++ "\t"++ me.outputString;
			});
		string = string ++"\t)\n"
		^"\t"++string;
	}	
}
	
GuidoMark {
	*new {
		^super.new;
		}
	}

/*
GuidoDynamic marks are:
\intens or \i
\crescBegin
\crescEnd
\dimBegin
\dimEnd

GuidoDynamic dynamics are:
"fff" .. "f"
"mf" and "mp"
"p" .. "ppp"

*/


GuidoDynamic : GuidoMark {
	var tag, dynamic;
	*new {arg tag, dynamic;
		([\intens, \i, \crescBegin, \crescEnd, \dimBegin, \dimEnd].indexOf(tag).notNil).if({
			^super.newCopyArgs(tag, dynamic).init;
			}, {
			"Tag not recognized as a GuidoDynamic".warn;
			^nil;
			})
		}
	
	init {
		dynamic = dynamic.isNil.if({
			" "
			}, {
			"<\""++dynamic++"\"> "
			}); 
		}
		
	outputString {
		^"\\"++tag.asString++dynamic;
		}
	}

/*
Tempo tags are
\tempo
\accelBegin
\accelEnd
\ritBegin
\ritEnd

tempoString is a direction i.e. "Moderato"
absTempoString is in the form of x/y=n i.e 1/4=120
*/
	
GuidoTempo : GuidoMark {
	var tag, tempoString, absTempoString, string;
	*new {arg tag, tempoString, absTempoString;
		([\tempo, \accelBegin, \accelEnd, \ritBegin, \ritEnd].indexOf(tag).notNil).if({
			^super.newCopyArgs(tag, tempoString, absTempoString).init;
			}, {
			"Tag not recognized as a GuidoTempo".warn;
			^nil;
			})
		}
	init {
		string = (tempoString.isNil && absTempoString.isNil).if({
				"\\"++ tag++" ";
				}, {
				(tempoString.notNil && absTempoString.notNil).if({
					"\\"++ tag++"<\"" ++ tempoString ++ "\", \"" ++ absTempoString ++ "\"> \n"
					}, {
					"\\"++ tag++"<\"" ++ tempoString ++ "\"> \n";
					})
				})
		}

	outputString {
		^string;
		}
	}
	
// articulations
/*

\stacc
\accent
\ten
\marcato
\trem
\grace

for \trem and \grace, a note value can be passed in indicating that kind of note to draw:

e.g. 32 = 32nd notes

*/

GuidoArticulation : GuidoMark {
	var tag, val;
	*new {arg tag, val;
		([\stacc, \accent, \ten, \marcato, \trem, \grace].indexOf(tag).notNil).if({
			^super.newCopyArgs(tag, val);
			}, {
			"Tag not recognized as a GuidoArticulation".warn;
			^nil;
			})
		}
		
	outputString {
		([\trem, \grace].indexOf(tag).notNil && val.notNil).if({
			^"\\"++tag.asString++"<"++val++">( ";		
			}, {
			^"\\"++tag.asString++"( ";
			})
		}
	}
	
GuidoArt {
	*new {arg tag;
		^GuidoArticulation.new(tag);
		}
	}

// maps out timesigs over measures: pass in an array of arrays:
// [ [measure, [beats, div]], [measure, [beats, div]] ]
// if a measure is left out, the previous measures timesig will be used
GuidoTimeSig {
	var <>timesigArray, <timesigDict;
	
	*new {arg timesigArray;
		^super.newCopyArgs(timesigArray).init;
		}
		
	init {
		var measure, meter;
		timesigDict = Dictionary.new;
		timesigArray.do{arg me;
			#measure, meter = me;
			timesigDict.add(measure -> meter);
			}
		}
		
	// create an array of meters for use in parsing voices... remember, measure one is at index 0!
	fillMeters {arg numMeasures;
		var curmeter, newmeter;
		newmeter = this.timesigDict.at(1);
		^Array.fill(numMeasures, {arg i;
			curmeter = newmeter;
			newmeter = this.timesigDict[i + 2] ? curmeter;
			curmeter;
			})
		}
	
	meterAt {arg measure = 1;
		var measures;
		measures = this.fillMeters(measure);
		^measures.last;
		}
		
	beatAt {arg measure = 1;
		^this.meterAt(measure)[0]
		}
		
	divAt {arg measure = 1;
		^this.meterAt(measure)[1]
		}
	
	// returns which measure a beat occurs in, and the beat in that measure.
	// beats assumed to be quarter for now
	measureFromBeat {arg beat;
		var measure = 1, thisbeat = 1, meter;
		meter = this.meterAt(measure);
		thisbeat = thisbeat + ((meter[0] * 4) / meter[1]);
		while({thisbeat <= beat}, {
			measure = measure + 1;
			meter = this.meterAt(measure);
			thisbeat = thisbeat + ((meter[0] * 4) / meter[1]);
			});
		^[measure, (beat - thisbeat) % meter[0] + 1];
		}	
	}

	
/* For GUIDO objects - no quartertones yet */

PitchClass {
	var <note, <acc, <octave, <pitch, <pitchclass, <keynum, <freq;
	classvar notenames, notenums, noteToScale, scaleToNote, accToSize, sizeToAcc, accToGuido;
	// deal with transposition, notenames and PC classes here
	// note and acc are symbols, octave is an integer, where middle c = c4
	
//	*new {arg note, acc = \n, octave = 4;
//		var pitch;
//		note.isKindOf(Number).if({
//			octave = (note.round/12).floor - 1;
//			#note, acc = notenums[note%12]});
//		pitch = (note ++ acc).asSymbol;
//		^super.newCopyArgs(note, acc, octave, pitch).init;
//		}

	*new {arg pitch, octave = 4;
		var note, acc, str;
		pitch.isKindOf(Number).if({
			octave = (pitch.round/12).floor - 1;
			pitch = notenums[pitch%12];
//			pitch = (note ++ acc).asSymbol
			});
		str = pitch.asString;
		note = str[0].asSymbol;
		if(str.size > 1, 
			{acc = str[1..str.size-1].asSymbol},
			{pitch = (str ++ "n").asSymbol; acc = \n}
			);
		^super.newCopyArgs(note, acc, octave, pitch).init;
		}

		
	init {
		pitch = pitch.asSymbol;
		pitchclass = notenames[pitch];
		keynum = pitchclass + (12 * (1 + octave));
		freq = keynum.midicps;
		}
	
	guidoString {
		var oct, gacc;
		oct = octave - 3;
		gacc = accToGuido[acc]
		^note.asString++gacc++oct;
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
	
	transpose {arg pitchArray, steps = 0, octaveSize = 12;
		pitchArray = pitchArray + steps;
		^this.new(pitchArray, nil, octaveSize);
		}
*/	

}

