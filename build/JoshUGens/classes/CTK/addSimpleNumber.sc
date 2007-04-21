+ SimpleNumber {
	mapIntoRange {arg range, target, steps = 12, numturns = 0, maxturns = 10, origval;
		var newtestval;
		origval = origval ? this;
		(numturns < maxturns).if({
			((this - target).abs > range).if({
				newtestval = (this > target).if({
					this - steps
					}, {
					this + steps
					});
				^newtestval.mod(120).mapIntoRange(range, target, steps, 
					numturns + 1, maxturns, origval)
				}, {
				^this;
				})
			}, {
			"Your Number couldn't be wrapped into the desired range".warn;
			^origval;
			})
		}
	}
	
/*

a = 76.3.mapIntoRange(12, 60);
a
*/