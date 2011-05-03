
InterPoint {
	var <>x, <>y, <>curve;
	*new { arg x, y, curve=0;
		^super.new
			.x_(x)
			.y_(y)
			.curve_(curve)
	}

	postln { ("InterPoint(" + x ++","+ y ++","+ curve +")").postln}

	value { ^y }

	interpl { arg end, n, theoneafterthat;
		var dx, sign, xrange, k;
		var a, b, c, y0, ym, y1;
		sign = (end.y - y).sign;
		k = curve;
		((x == end.x) || (curve == \step)).if( { ^y });
		(k == \quadratic).if( {^this.quadinterp(end, n, theoneafterthat)} );
		xrange = (end.x - x);
		dx = (n - x)/xrange;
		((k == \exponential) || (k == \exp)).if(
			{
				k = 2.71828 * (end.y/y).log / xrange;
			});
		k.isNumber.if(
			{
				(k != 0).if( {dx = (1.0 - (dx*k).exp ) / (1.0 - k.exp)} )
			},{
				((k == \sine) || (k == \sin)).if({
					dx = ((pi * (dx - 0.5) ).sin + 1) * 0.5	// this version slightly more efficient
					//	dx = (0.5pi * dx).sin.squared
				});
				((k == \welch) || (k == \wel)).if({
					dx = ( sign.isPositive ).if(
						{ (1 - ( dx * 0.5 )) * dx * 2 },
						{ ( dx * 0.5 ) * dx * 2}
					)
				});
			}
		);
		^y + (dx * (end.y - y));
	}

	interclip { arg end, n, theoneafterthat;
		^this.interpl( end, n.clip( this.x, end.x ), theoneafterthat )
	}

	quadinterp { arg end, n, theoneafterthat;
		var dx, a, b, c, y0, ym, y1;

		dx = (n - x)/(end.x - x);
		y0 = (y + end.y) / 2;
		ym = end.y;
		y1 = ( end.y + theoneafterthat.y) / 2;
		c = y0;
		b = 2 * (ym -y0);
		a = y1 - (2*ym) + y0;
		^((a * dx.squared) + (b * dx) + c);
	}
}

Interpl {
/*****
	Break Point interpolator. Similar to an Envelope that can be indexed discreetly or continuously by any arbitrary
	value (while Envelopes can only be indexed by time, using EnvGen).
*****/
	var <points, <minX, <maxX, <minY, <maxY, <size;

	*new { arg ... xyc;
		/*
			Takes a list of triples: [x0, y0, curve0], [x1, y1, curve1], ... [xn, yn]
			Where each 'y' is the value that will be returned at index point 'x'.
			The curves may be any value that is a valid Env curve: \sine, \welch, \exponential,
			\step, or a float
			Interpolation curves are designed to behave similarly to their Env counterparts,
			but are not exact matches in most cases.
			x = Interpl.new( [0,40,5], [5,72,\welch], [10,80,-5], [20,0] );
			x.plot;
			21.do({arg i; x.at(i).postln})
		*/
		xyc = xyc.flat.clump(3);
		^super.new.init( xyc.size ).addxyc( xyc )
	}

	*newWithPairs { arg pairs, curve; ^this.newPairs( pairs, curve )}
	*newPairs { arg pairs, curve;
		/*
			Takes a list of x, y pairs and a curve value for all segments:
			Interpl.newPairs( [ 0,60, 5,72, 10,80, 20,0], \sin ).plot
		*/
		pairs = pairs.flat.clump(2);
		curve = [curve].flat;
		pairs = pairs.collect({ arg xy, i;
			xy.add( curve.clipAt( i ) )
		});
		^super.new.init( pairs.size ).addxyc( pairs )
	}

	*newFromEnv { arg env; ^this.newEnv }
	*newEnv { arg env;
		/*
			Creates a new Interpl instance using the times and levels of an Envelope
			a = Interpl.newEnv( Env.new([60,72,80,0], [0.5,0.4,0.10], \sine) ).plot;

		*/
		var x, y, c;
		y = env.levels;
		x = [0];
		env.times.do({arg time;
			x = x ++ [x.last + time];
		});
		( x.size != y.size ).if(
			{"Env levels array should have one more value than times array.".error; this.halt });
		c = env.curves;
		c.isKindOf( Collection ).if(
			{
				( x.size != (c.size+1) ).if(
					{"\tEnv levels array should have one more value than curves array.".error;
						this.halt },
					{ c = c ++ [0] }
				);
			},
			{ c = Array.fill( x.size, {c} )}
		);
		^super.new.init( x.size ).addxyc( [x,y,c].flop(3) )
	}

	setCurves { arg curve;
		points.do({arg p; p.curve_( curve )});
		this.validate
	}

	plot {
		Array.fill(1024, {|i| this[(i * this.maxX )/1024]}).plot;
	}

	scaleX { arg scale;
		points.do({ arg p; p.x_( p.x * scale )});
		this.setBounds
	}

	scaleY { arg scale;
		points.do({ arg p; p.y_( p.y * scale )});
		this.validate;
	}

	addX { arg scale;
		points.do({ arg p; p.x_( p.x + scale )});
		this.setBounds;
	}

	addY { arg scale;
		points.do({ arg p; p.y_( p.y + scale )});
		this.validate;
	}

	do { arg fn;
		points.do({arg point, i; fn.value( point, i )});
		this.setBounds;
		this.validate;
	}

	dumpPoints { points.do({arg p; p.postln }); this.dump; }

	@ {arg x; ^this.at(x)}

	at { arg x;
	/*
		at, wrapAt, clipAt, foldAt
			Index the Interpl with a discreet number
		a = Interpl.new( [0,40,5], [5,72,\welch], [10,80,-5], [20,0] );
		a.clipAt(50).postln;
		a.at( 3 ).postln;
	*/
		var startpoint, endpoint, index;

		x.isNumber.not.if({this.barf("Index" + x + "not a number")});

		^x.inclusivelyBetween( minX, maxX ).if(
			{
				endpoint = points.detect({ arg point, i;
					index = (i-1);
					point.x >= x
				});

				startpoint = points.clipAt( index );

				startpoint.interpl( endpoint, x )
			},
			{
				this.barf("Interpl index:"+x+" out of bounds");
			}
		)
	}

	wrapAt { arg x;
		x = x.wrap( minX, maxX )
		^this.at( x )
	}

	clipAt { arg x;
		x = x.clip( minX, maxX )
		^this.at( x )
	}

	foldAt { arg x;
		x = x.fold( minX, maxX )
		^this.at( x )
	}

	kr { arg x, lag=0;
	/*
		kr, wrapkr, clipkr, foldkr
			Index the Interpl with a k-rate ugen
			Returns a k-rate ugen
		play{
			x = Interpl.newPairs([0,60, 1,63, 2,65, 3,66, 4,67, 5,70, 6,72], 20);
			i = MouseX.kr(0, 12);
		//	i = LFTri.kr( 1, 3, 3);
			SinOsc.ar( x.wrapkr(i).midicps, 0, 0.5)
		}
	*/
		^{ this.at( x.poll ) }.kr(lag)
	}

	wrapkr { arg x, lag=0;
		^{ this.wrapAt( x.poll ) }.kr(lag)
	}

	clipkr { arg x, lag=0;
		^{ this.clipAt( x.poll ) }.kr(lag)
	}

	foldkr { arg x, lag=0;
		^{ this.foldAt( x.poll ) }.kr(lag)
	}

		init { arg size;
		points = SortedList.new( size, {arg a, b; a.x < b.x} );
	}


	// PRIVATE
	addxyc { arg xyc;
		points.addAll( xyc.collect({arg xy; InterPoint.new( xy@0, xy@1, xy@2 ? 0 ) }) );
		this.setBounds
	}

	setBounds {
		this.validate;
		minX = points.minItem({arg p; p.x}).x;
		maxX = points.maxItem({arg p; p.x}).x;
		minY = points.minItem({arg p; p.y}).y;
		maxY = points.maxItem({arg p; p.y}).y;
		size = points.size;
	}

	validate {
		points.doAdjacentPairs({ arg a, b;
			(a.curve == \exponential).if({
				(a.y==0).if(
					{
						this.barf( "Interpl exponential curve: start or end is zero." );
					});
				((a.y.sign != b.y.sign)).if(
					{
						this.barf( "Interpl exponential curve: start and end signs mismatched." );
					});
			})
		})
	}

	barf { arg string;
		string.error;
		this.dumpPoints;
		this.halt;
	}
}
