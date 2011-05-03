+ SequenceableCollection {

	// Returns an array of addends to the sum.
	// Useful for fitting the inter-offset times of an
	// arbitrary number of notes to the length of a measure.
	asAddendsOf{ arg sum;
		var result, values, val;
		result = this.class.new;
		values = this.copy;

		while({ sum > 0 },{
			val = values.choose;
			if( val <= sum, {
				result = result.add(val);
				sum = sum - val;
			}, {
				// value can't be used anymore
				values.remove(val);
				// if there are no more usable values, exit loop
				if( values.size == 0, {
					sum = -1;
				});
			});
		});
		^result
	}

	// same as asSummationOf, but with weights
	aswAddendsOf{ arg summation, weights;
		var result, values, index;
		result = this.class.new;
		values = this.copy;
		weights = weights.normalizeSum;

		while({ summation > 0 },{
			index = Array.series(values.size, 0, 1).wchoose(weights);
			if( values[index] <= summation, {
				result = result.add(values[index]);
				summation = summation - values[index];
			}, {
				// value can't be used anymore
				values.removeAt(index);
				weights.removeAt(index);
				weights = weights.normalizeSum;

				// if there are no more usable values, exit loop
				if( values.size == 0, {
					summation = -1;
				});
			});
		});
		^result
	}

}