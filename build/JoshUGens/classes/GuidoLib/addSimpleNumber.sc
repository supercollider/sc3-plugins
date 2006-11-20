//+ SimpleNumber {
//	filter {arg aPitchCollection;
//		var sortedCollection, octave, octaveSize, nearestPitch;
//		octaveSize = aPitchCollection.octaveSize;
//		// grab the base collection, sorted
//		sortedCollection = aPitchCollection.sortedBase ++ octaveSize;
//		// calculate how much we will need to re-multiply later
//		octave = (this / octaveSize).floor;
//		nearestPitch = sortedCollection[sortedCollection.indexIn(this%octaveSize)];
//		^(nearestPitch + (octave * octaveSize))
//		}
//	}
//	
/*

a = PitchCollection.new([0, 2, 3, 5, 7, 8, 11])
72.1.filter(a)

*/