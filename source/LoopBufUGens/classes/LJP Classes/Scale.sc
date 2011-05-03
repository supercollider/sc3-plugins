ScaleLP {

	var <>stepRatios;

	*new {
		^super.new
	}

	// n Cent Equal-Temperament
	nCET { arg cent;
		stepRatios = [2.pow(cent/1200)];
		^stepRatios
	}

	// n Equal Division Octave
	nEDO { arg divs=12, octaveRatio=2, intervals;
		if(intervals.isNil, {
			intervals = Array.series(divs, 1, 1);
		},{
			intervals = intervals.integrate;
		});

		stepRatios = octaveRatio.pow(intervals/divs);
		^stepRatios
	}

	// 12-tone equal-temperament
	et12 { arg intervals;
		^this.nEDO(12, 2, intervals)
	}

	// Equal-temperament chromatic scale
	chromatic { arg divs=12, octaveRatio=2;
		^this.nEDO(divs, octaveRatio)
	}

	// 12-et modes
	ionian 		{ ^this.et12([2,2,1,2,2,2,1]) }
	dorian 		{ ^this.et12([2,1,2,2,2,1,2]) }
	phrygian 		{ ^this.et12([1,2,2,2,1,2,2]) }
	lydian		{ ^this.et12([2,2,2,1,2,2,1]) }
	mixolydian	{ ^this.et12([2,2,1,2,2,1,2]) }
	aeolian		{ ^this.et12([2,1,2,2,1,2,2]) }
	locrian		{ ^this.et12([1,2,2,1,2,2,2]) }
	ionian5 		{ ^this.et12([2,2,3,2,3]) }
	dorian5 		{ ^this.et12([2,1,4,2,3]) }
	phrygian5 	{ ^this.et12([1,2,4,1,4]) }
	lydian5		{ ^this.ionian5 }
	mixolydian5	{ ^this.ionian5 }
	aeolian5		{ ^this.et12([2,1,4,1,4]) }
	locrian5		{ ^this.et12([1,2,3,2,4]) }

	// 12-et scales
	augmented		{ ^this.et12([3,1,2,1,3,1]) }
	balinese1		{ ^this.phrygian5 }
	blues		{ ^this.et12([3,1,1,2,2,1,2]) }
	chinese1		{ ^this.maj5 }
	chinese2		{ ^this.et12([4,2,1,4,1]) }
	diminished 	{ ^this.et12([2,1,2,1,2,1,2,1]) }
	enigmatic		{ ^this.et12([1,3,2,2,2,1,1]) }
	doubleHarmonic{ ^this.et12([1,3,1,2,1,3,1]) }
	ethiopian1	{ ^this.et12([2,2,1,2,1,3,1]) }
	flamenco		{ ^this.et12([1,2,1,1,2,1,2,2]) }
	egyptian		{ ^this.et12([2,3,2,3,2]) }
	hindu		{ ^this.et12([2,2,1,2,1,2,2]) }
	hirajoshi		{ ^this.aeolian5 }				// japanese
	hungarianGypsy{ ^this.et12([2,1,3,1,1,2,2]) }
	hungarianMaj	{ ^this.et12([3,1,2,1,2,1,2]) }
	hungarianMin	{ ^this.et12([2,1,3,1,1,3,1]) }
	indian		{ ^this.et12([1,2,1,3,2,2]) }
	iwato		{ ^this.et12([1,4,1,4,2]) }		// japanese
	japanese1		{ ^this.et12([1,4,2,1,4]) }
	javanese		{ ^this.et12([1,2,2,2,2,1,2]) }
	locrianMaj	{ ^this.et12([2,2,1,1,2,2,2]) }
	locrianNat2	{ ^this.et12([2,1,2,1,2,2,2]) }
	locrianSuper	{ ^this.et12([1,2,1,2,2,2,2]) }
	locrianUltra	{ ^this.et12([1,2,1,2,2,1,3]) }
	lydianMin		{ ^this.et12([2,2,2,1,1,2,2]) }
	lydianDom		{ ^this.et12([2,2,2,1,2,1,2]) }
	lydianAug		{ ^this.et12([2,2,2,2,1,1,2]) }
	maj 			{ ^this.ionian }
	maj5 		{ ^this.ionian5 }
	majHarmonic	{ ^this.et12([2,2,1,3,1,2,1]) }
	marva		{ ^this.et12([1,3,2,1,2,2,1]) }	// indian
	min 			{ ^this.aeolian } 				// natural
	min5 		{ ^this.et12([3,2,2,3,2]) }
	minHarmonic 	{ ^this.et12([2,1,2,2,1,3,1]) }
	minMelodic1 	{ ^this.et12([2,1,2,2,1,3,1]) } 	// ascending
	minMelodic2 	{ ^this.aeolian } 				// descending
	mixolydianAug	{ ^this.et12([2,2,1,3,1,1,2]) }
	neapolitanMaj	{ ^this.et12([1,2,2,2,2,2,1]) }
	neapolitanMin	{ ^this.et12([1,2,2,2,1,3,1]) }
	oriental		{ ^this.et12([1,3,1,1,3,1,2]) }
	pelog		{ ^this.et12([1,2,4,3,2]) }		// balinese
	persian		{ ^this.et12([1,3,1,1,2,3,1]) }
	phrygianMaj	{ ^this.et12([1,3,1,2,1,2,2]) }
	romanian		{ ^this.et12([2,1,3,1,2,1,2]) }
	semitone3 	{ ^this.et12([3,3,3,3]) }
	semitone4 	{ ^this.et12([4,4,4]) }
	spanish8		{ ^this.et12([1,2,1,1,1,2,2,2]) }
	symmetrical	{ ^this.et12([1,2,1,2,1,2,1,2]) }
	todi			{ ^this.et12([1,2,3,1,1,3,1]) }	// indian
	whole 		{ ^this.et12([2,2,2,2,2,2]) }
	wholeLeading	{ ^this.et12([2,2,2,2,2,1,1]) }

	// W. Carlos
	alpha 		{ ^this.nCET(78) }
	beta 		{ ^this.nCET(63.8) }
	gamma 		{ ^this.nCET(35.1) }


}