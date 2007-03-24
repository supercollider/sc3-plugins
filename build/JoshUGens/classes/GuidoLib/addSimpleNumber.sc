// this can be handy for keynum being wrapped within a certain range around a target
// takes a number and will try to give you a value within +-range of target 
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

a = 76.3.mapIntoRange(2, 60);
a
*/