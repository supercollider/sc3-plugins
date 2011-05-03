/* Begin CK Ambisonics additions */

+ Array  {
	w { ^this.at(0) }
	x { ^this.at(1) }
	y { ^this.at(2) }
	z { ^this.at(3) }

	w_ { arg w; ^this.put( 0, w) }
	x_ { arg x; ^this.put( 1, x) }
	y_ { arg y; ^this.put( 2, y) }
	z_ { arg z; ^this.zP.if({
			this.put( 3, z)
		},{
			this.add( z )
		})
	}

	stripz { ^this.copyRange( 0, 2 ) }

	zP { ^this.z.notNil}

	toRelative {
		// converts a list of absolute times to a list of relative times
		var x;
		x = [];
		this.doAdjacentPairs({arg a, b; x = x.add( b - a)});
		^x
	}

	toAbsolute {
		// converts a list of relative times to a list of absolute times
		var x;
		x = [0,this.at(0)];
		this.doAdjacentPairs({arg a, b; x = x.add( b + x.last)});
		^x
	}

/* End CK Ambisonics additions */
}
