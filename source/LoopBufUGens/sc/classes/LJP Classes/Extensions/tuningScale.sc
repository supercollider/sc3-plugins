// © 2003 Lance Putnam

+ SequenceableCollection {

	// Makes a specified tuning scale
	// The last element of stepRatios is the octave ratio.  This is typically 2.

	*tuningScale { arg rootFreq = 440, stepRatios=#[1.5, 2], loFreqBound = 20, hiFreqBound = 20000;
		var index, octaveUp, octaveDown, divs, scaleFreqs;
		var current, octave;

		divs = stepRatios.size;
		if(stepRatios[divs-1] <= 1.001, { ^nil });

		index = -1;
		octaveUp = stepRatios[divs - 1];
		octaveDown = 1/octaveUp;
		octave = rootFreq;
		scaleFreqs = this.new;

		while({ octave > loFreqBound }, {	// below low bound?
			octave = octave * octaveDown;	// go down by "octave" from root
		});

		current = octave;

		while({ current < loFreqBound }, {	// below low bound?
			index = index + 1;
			if(index >= divs, {
				index = 0;
				octave = octave * octaveUp;
			});
			current = octave * stepRatios[index];	// go up one step
		});

		while({ current <= hiFreqBound }, {	// at or below high bound?

			scaleFreqs = scaleFreqs.add(current);
			index = index + 1;
			if(index >= divs, {
				index = 0;
				octave = octave * octaveUp;
			});
			current = octave * stepRatios[index];
		});

		^scaleFreqs
	}

	// does a binary search
	nearestTo { arg value;
		var lowerInd, upperInd, centerInd;

		lowerInd = 0;
		upperInd = this.size - 1;

		while({ (upperInd - lowerInd) > 1 },{

			centerInd = (((upperInd - lowerInd)/2.0)+lowerInd).round.asInt;

			if( value < this[centerInd], {
				upperInd = centerInd;
			},{
				lowerInd = centerInd;
			});
		});

		if( (value - this[lowerInd]) < (this[upperInd] - value), {
			^this[lowerInd]
		},{
			^this[upperInd]
		});
	}

	// same as nearestTo, but input/output is an array
	nearestToList { arg values;
		var matches;
		var lowerInd, upperInd, centerInd;

		values.do({ arg value, i;

			lowerInd = 0;
			upperInd = this.size - 1;

			while({ (upperInd - lowerInd) > 1 },{

				centerInd = (((upperInd - lowerInd)/2.0)+lowerInd).round.asInt;

				if( value < this[centerInd], {
					upperInd = centerInd;
				},{
					lowerInd = centerInd;
				});

				//("In range: [" ++ this[lowerInd] ++ ", " ++ this[upperInd] ++ "]").postln;

			});

			if( (value - this[lowerInd]) < (this[upperInd] - value), {
				matches = matches.add( this[lowerInd] );
			},{
				matches = matches.add( this[upperInd] );
			});
		});

		^matches
	}

}
