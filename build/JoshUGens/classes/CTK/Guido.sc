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
				(me.isKindOf(GuidoNote) || me.isKindOf(GuidoChord)).if({
					notearray = [];
					notearray = notearray.add(me.class.new(me.note, curbeat, firstdur));
					curbeat = curbeat + firstdur;
					numFullMeasNotes = ((me.duration - firstdur) / beatsPerMeasure).floor;
					numFullMeasNotes.do({arg i;
						notearray = notearray.add(me.class.new(me.note, curbeat, 
							beatsPerMeasure));
						curbeat = curbeat + beatsPerMeasure;
						});
					lastdur = (me.duration - firstdur) - (numFullMeasNotes * beatsPerMeasure);
					(lastdur > 0).if({
						notearray = notearray.add(me.class.new(me.note, curbeat, lastdur))
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
	var <>note, <>beat, <>duration, <>marks;
	
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
		numnotes = note.size;
		notestring = "{ ";
		note.do({arg me, i;
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

