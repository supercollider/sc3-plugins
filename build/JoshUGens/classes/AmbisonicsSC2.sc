// June 19, 2001: Added rect method to BFDecode

// Mar 3, 2001: Added interior localization stuff after joe anderson
// 	BFEncode.ar takes a polar (or a point), where rho of 1 is the speaker radius.

BFEncode {
	classvar  <methodReqs=#['Array.stripz'];
//	var <>w, <>x, <>y, <>z;
//	W = sig * -3.dbamp
//	X = sig * cosA * cosB (front-back)
//	Y = sig * sinA * cosB (left-right)
//	Z = sig * sinB (up-down)
//
//	A=azimuth-angle
//	B=elevation-angle

	*ar { arg sig, polar; ^super.new.ar1( PointSource.new( sig, polar ) ) }
	*ar1 { arg sig, rho, theta; ^super.new.ar1( PointSource.newt( sig, rho, theta ) ) }
	*ar2 { arg sig, x, y;
		var point;
		point = Point( y, -1 * x );
		^super.new.ar1( PointSource.newt( sig, point.rho, point.theta ) )
	}

	ar1 { arg pointsource;
		var elevcos, sig, azim, encodeArray, sinInt, cosInt;
		// Mix down source to mono if needed
		sig = (pointsource.sig.size > 1).if({sig = Mix.new(pointsource.sig)}, {pointsource.sig} );
		// Cosine of the elevation angle, we need this for X and Y
		elevcos = pointsource.phi.cos;

		// If creating a Polar directly, rho can be negative: deal with it here
	//	azim = pointsource.theta + ((pointsource.rho < 0).binaryValue.neg*pi);
	//	azim = pointsource.theta + ((pointsource.rho < 0) * pi);
		azim = pointsource.theta + (BinaryOpUGen.new( '<', pointsource.rho, 0 )* pi);
		// Must add
		//	binaryValue { ^this }
		// method to BinaryOpUGen!!

		// dist = (pointsource.rho < 0);
		// dist = dist.respondsTo(\binaryValue).if({dist.binaryValue},{dist});
		// azim = pointsource.theta + dist.neg*pi);

		// Interior localization stuff
//		sinInt = (pointsource.rho > 1).if( 1, (2.sqrt * (0.25pi * pointsource.rho).sin) );
//		cosInt = (pointsource.rho > 1).if( 1, (2.sqrt * (0.25pi * pointsource.rho).cos) );
		// this should be a little more efficient:
		sinInt = (2.sqrt * (0.25pi * pointsource.rho.abs.min(1.0)).sin);
		cosInt = (2.sqrt * (0.25pi * pointsource.rho.abs.min(1.0)).cos);

		// Now create encoding matrix, we eliminate the Z coefficient in case point is bi-dimensional
		encodeArray = [
			-3.dbamp * cosInt, 			// W
			azim.cos * elevcos * sinInt, 	// X
			azim.sin * elevcos * sinInt,	// Y
			pointsource.phi.sin * sinInt	// Z
		]	//.select({arg chan, i; i <= pointsource.dims});
			// maybe more efficient: copyRange is a primitive, select uses a loop
			.copyRange( 0, pointsource.dims );

		// Scale signal by the encoding matrix and return the encoded B format signal
//		#w, x, y, z = (sig * encodeArray);
//		^z.isNil.if({ Array.with(w,x,y,z) }, { Array.with(w,x,y) } )
		^(sig * encodeArray);
	}

	*arDist { arg sig, point, distScale=1.0, revScale=0.1, distmax=60.0;
		var dry, wet;
		#dry, wet = DistAmp.ar( sig, point.rho * distScale, revScale, distmax );
		^this.ar( dry, point );
	}
	*woof { arg sig, point, distScale=1.0, revScale=0.1, revTime=3.0, hfDamp=0.9, distmax=60.0;
		var dry, wet;
		#dry, wet = DistAmp.ar( sig, point.rho * distScale, revScale, distmax );
		dry = this.ar( dry, point );
		wet = this.ar( wet, point );
		wet = BFRev.ar( wet, revTime, hfDamp );
		^dry+wet
	}
}

//B2A {
////	Date: Mon, 12 Mar 2001 17:04:22 -0800 (PST)
////	From: J. Pampin <pampin@u.washington.edu>
////	To: Chad Kirby <ckirby@u.washington.edu>
////	Subject: Re: A/B format
////
////	I allways use the following matrix to remember A-B conversion
////
////	        lf      rf      lb      rb
////	W        1       1       1       1
////	X        1       1      -1      -1
////	Y        1      -1       1      -1
////	Z        1      -1      -1       1
///*
//	*ar { arg b, scale=1.0;
//		// This is not as efficient: don't use this method!
//		^[
//			Mix.ar( b ),
//			Mix.ar( b * #[1,1,-1,-1] ),
//			Mix.ar( b * #[1,-1,1,-1] ),
//			Mix.ar( b * #[1,-1,-1,1] )
//		] * scale
//	}
//*/
//	*help { arg method='short_desc', argument;
//		^Switch(method.asSymbol)
//			.case(\short_desc, {"B Format to A format transcoder"})
//			.case(\long_desc, {"Converts a B Format signal (Z is optional) to A format."})
//			.case(\ar,{
//				Switch(argument.asSymbol)
//					.case( 'b', {"input signal. [w,x,y,z] -or- [w,x,y]"} )
//					.case( 'scale', {"gain scaler"} )
//					.result
//			})
//			.case(true, {""})
//			.result
//	}
//
//	*ar { arg b, scale=0.5;
//		var w, x, y, z, yneg, xneg, zneg, out;
//		#w, x, y, z = b;
//		yneg = y.neg;
//		xneg = x.neg;
//		out = z.isNil.if({
//			[
//				Mix.ar( [ w, x, y ] ),
//				Mix.ar( [ w, x, y.neg ] ),
//				Mix.ar( [ w, x.neg, y ] ),
//				Mix.ar( [ w, x.neg, y.neg ] )
//			]
//		},{	zneg = z.neg;
//			[
//				Mix.ar( [ w, x, y, z ] ),
//				Mix.ar( [ w, x, y.neg, z.neg ] ),
//				Mix.ar( [ w, x.neg, y, z.neg ] ),
//				Mix.ar( [ w, x.neg, y.neg, z ] )
//			]
//		});
//		// save a ugen if unity gain
//		^(scale == 1).if({out},{out * scale})
//	}
//}
//
//A2B : B2A {}
//
//A2BnoZ : B2A {
//	*ar { arg in, scale = 0.5;
//		^super.ar( in, scale ).stripz;
//	}
//}

BFDecode {
// 	Spn = directivity-factor*W +
// 	[cos Sp-angle]*X +
// 	[sin Sp-angle]*Y +
// 	[sin elevation-angle]*Z

	*ar { arg sig, directivity=1.0 ... spkrs;
		// spkrs should be array of Spkrs
		var spkrDeltas, maxDist;
		( spkrs.size == 0 ).if({ spkrs = ThisMac.speakers });
		spkrs = spkrs.flat;
		directivity = directivity ? 1.0;
		maxDist = spkrs.maxItem({ arg polar; polar.rho}).rho;
		spkrDeltas = spkrs.collect({arg zpolar; (maxDist - zpolar.rho)/345});
		^spkrs.collect({arg spkr, chan;
			var out, delta, decodeArray;
			delta = spkrDeltas.at(chan);
			decodeArray = [ directivity, spkr.theta.cos, spkr.theta.sin, spkr.phi.sin ];
			out = Mix.new( decodeArray * sig * spkr.amp );
			(delta > 0).if({DelayN.ar( out, delta, delta )},{out})
		})
	}

	*rect { arg sig, width=4, length=7, directivity=1.0;
		// By JP, paper by Malham, initial work in SC by JLA
		// classified by CK
		var a, b, alpha;
		var fL, fR, bL, bR;
		var ww, xx, yy, zz;
		alpha = atan(width/length);
		a = 0.5/sin(alpha);
		b = 0.5/cos(alpha);

		#ww, xx, yy, zz = sig;

		xx = xx * a;
		yy = yy * b;

		fL = ww + xx + yy;
		fR = ww + xx - yy;
		bL = ww - xx + yy;
		bR = ww - xx - yy;

		^[fL, fR, bL, bR];
	}

	*asQuad { arg sig;
		^this.ar( sig, 1.0, Speakers.quad )
	}

	*asStereo { arg sig;
		// http://www.cyber.rdg.ac.uk/P.Sharkey/WWW/icdvrat/WWW96/Papers/keating.htm
		// L = W + 0.35X + 0.61Y
		// R = W + 0.35X - 0.61Y
		var w, x, y, yneg;
		w = sig.at(0);
		x = 0.35 * sig.at(1);
		y = 0.61 * sig.at(2);
		yneg = -0.61 * sig.at(2);
		^[(w + x + y), (w + x + yneg)]

//		var wout, xout, yout, allout;
//		wout = sig * 0.707;
//		xout = (sig * cos(loc));
//		yout = (sig * sin(loc));
//		xout = (wout + xout) * 0.35 ;
//		yout = (wout + yout) * 0.61;
//		^allout = [xout, yout]

	}

	*asUHJ { arg sig;
		^BtoUHJ.ar(sig)
	}

	*asUHJ1 { arg sig, fftsize, numffts;
		// http://www.cyber.rdg.ac.uk/P.Sharkey/WWW/icdvrat/WWW96/Papers/keating.htm
		// http://www.york.ac.uk/inst/mustech/3d_audio/ambis2.htm
		// L = (0.4699 - 0.17j)W + (0.0928 + 0.255j)X + 0.3278Y	(33)
		// R = (0.4699 + 0.17j)W + (0.0928 - 0.255j)X - 0.3278Y	(34)
		var wj, xj, yj, w, x, y;
		w = sig.at(0);
		wj = Shift90.fft(w, fftsize, numffts );
		x = sig.at(1);
		xj = Shift90.fft(x, fftsize, numffts );
		y = sig.at(2) * 0.3278;
		^[	(0.4699 - (0.17 * wj) * w) + (0.0928 + (0.255 * xj) * x) + y,
			(0.4699 + (0.17 * wj) * w) + (0.0928 - (0.255 * xj) * x) - y]
	}

	*hexagon { arg sig;
	/*
		<http://www.muse.demon.co.uk/ref/speakers.html>
		Rig `Controlled Opposites' Decode Matrix (First Order)

		<0.8660,0.5000,0.0000>		0.2357	0.1443	0.0833	0.0000
		<0.0000,1.0000,0.0000>		0.2357	0.0000	0.1667	0.0000
		<-0.8660,0.5000,0.0000>	0.2357	-0.1443	0.0833	0.0000
		<-0.8660,-0.5000,0.0000>	0.2357	-0.1443	-0.0833	0.0000
		<0.0000,-1.0000,0.0000>	0.2357	0.0000	-0.1667	0.0000
		<0.8660,-0.5000,0.0000>	0.2357	0.1443	-0.0833	0.0000

	*/
		^[
			  Mix.new( #[0.2357,  0.1443,  0.0833] * sig )
			, Mix.new( #[0.2357,  0.0000,  0.1667] * sig )
			, Mix.new( #[0.2357,  -0.1443, 0.0833] * sig )
			, Mix.new( #[0.2357, -0.1443, -0.0833] * sig )
			, Mix.new( #[0.2357,  0.0000, -0.1667] * sig )
			, Mix.new( #[0.2357,  0.1443, -0.0833] * sig )
		]
	}

	*surround { arg sig;
	/*
		<http://www.muse.demon.co.uk/ref/speakers.html>
		Rig `Controlled Opposites' Decode Matrix (First Order)

		This rig configuration produces an `in-phase' response.
		Speaker					W		X		Y		Z
		<0.8660,0.5000,0.0000>		0.1690	0.0797	0.0891	0.0000
		<1.0000,0.0000,0.0000>		0.1635	0.0923	0.0000	0.0000
		<0.8660,-0.5000,0.0000>	0.1690	0.0797	-0.0891	0.0000
		<-0.5000,0.8660,0.0000>	0.4563	-0.1259	0.1543	0.0000
		<-0.5000,-0.8660,0.0000>	0.4563	-0.1259	-0.1543	0.0000
	*/
		^[
			  Mix.new( #[0.1690,  0.0797,  0.0891] * sig )	// L
			, Mix.new( #[0.1635,  0.0923,  0.0000] * sig )	// Center
			, Mix.new( #[0.1690,  0.0797, -0.0891] * sig )	// R
			, Mix.new( #[0.4563, -0.1259, -0.1543] * sig )	// LR
			, Mix.new( #[0.4563, -0.1259,  0.1543] * sig )	// LL
		]
	}

	*fiveDot1 { arg sig, freq=120;
		var w;
		#w = sig;
		^this.surround( sig ) ++ [ LPF.ar( w, freq )]
	}

}


BF {	// Ambisonics Codec

	*ar { arg sig, point, directivity ... speakers;
		^BFDecode.ar( BFEncode.ar(sig, point), directivity, speakers );
	}

	*ar1 { arg pointsource, directivity ... speakers;
		^BFDecode.ar( BFEncode.ar1(pointsource), directivity, speakers );
	}

	*asStereo { arg sig, point; ^BFDecode.asStereo( BFEncode.ar(sig, point) )}
	*asUHJ { arg sig, point; ^BFDecode.asUHJ( BFEncode.ar(sig, point) )}
	*asUHJ1 { arg sig, point, fftsize, numffts; ^BFDecode.asUHJ1( BFEncode.ar(sig, point), fftsize, numffts )}
}

Spkr {
	var <>point, <>amp;

	*newt { arg argpoint, argamp=1.0;
		^super.new
			.point_(argpoint)
			.amp_(argamp)
	}

	*new { arg x, y, argamp=1.0;
		var point;
		point = Point( y, -1 * x );
		^super.new
			.point_(Polar(point.rho, point.theta))
			.amp_(argamp)
	}

	rho { ^point.rho }
	theta { ^point.theta }
	phi { ^point.respondsTo(\phi).if({point.phi},{0.0}) }

}

Speakers {
	*quad { arg distance=1, clockwise=true;
		^this.n( 4, distance, clockwise )
	}
	*eight { arg distance=1, clockwise=true;
		^this.n( 8, distance, clockwise )
	}
	*n { arg n, distance=1, clockwise=true;
		var pi2;
		pi2 = clockwise.if(-2pi, 2pi);
		^Array.fill( n, { arg i;
			Spkr.newt( Polar( distance, (pi2 * i/n + (pi/n)).wrap(0,2pi) ) )
		})
	}
}

Reflections {
	*ar { arg pointsource, rooml, roomw, absorbtion=0.9, directivity ... spkrs;
		var sig, sigloc, spoint, rpoints, phi;
		sig = pointsource.sig;
		spoint = pointsource.asPoint;
		phi = pointsource.respondsTo( \zpolar ).if({pointsource.phi}, {0.0});
		rpoints = [ZPoint(spoint.x,rooml/2,phi),ZPoint(spoint.x,rooml/(-2),phi),ZPoint(roomw/2,spoint.y,phi),ZPoint(roomw/(-2),spoint.y)];
		^Mix.fill(rpoints.size,
		{ arg i;
			var point, pathdist;
			point = rpoints.at(i);
			pathdist = point.rho + point.dist(spoint);
			BF.ar( Distance.ar( pointsource.class.newt( sig, pathdist, point.theta, point.phi ),
						hypot(rooml, roomw) + max(rooml, roomw) ) * absorbtion,
				directivity, spkrs );
		})
	}
}

ZPolar {
	var <>rho=0.0, <>theta=0.0, <>phi=0.0;
	// rho is the radius, 0 <= rho <= inf
	// theta is azimuth on xy plane, 0 <= theta <= 2pi
	// standing at the origin, 0 or: 2pi is to the left, 0.5pi straight ahead,
	// pi to the right, 1.5pi to the rear
	// phi is the z-axis polar angle, -0.5pi <= phi <= 0.5pi
	// 0 is straight up, 0.5pi is horizontal, pi is straight down

	*new { arg argrho=0.0, argtheta=0.0, argphi=0.0;
		^super.new.rho_(argrho)
			.theta_(argtheta)
			.phi_(argphi);
	}

	x {^(rho*theta.cos*phi.cos)}
	y {^(rho*theta.sin*phi.cos)}
	z {^(rho*phi.sin)}

	asPoint {^Point.new(this.x, this.y)}
	asZPoint {^ZPoint.new(this.x, this.y, this.z)}
	asPolar {^Polar.new(rho, theta)}
	asZPolar {^this}

	dist {arg aPoint;
		var argx, argy, argz;
		argx = aPoint.x; argy = aPoint.y;
		argz = aPoint.respondsTo(\z).if({aPoint.z},{0});
		^((this.x-argx).squared + (this.y-argy).squared + (this.z-argz).squared).sqrt
		}
	angle {^theta}
	azim {^theta}
}

ZPoint {
	var <>x=0.0, <>y=0.0, <>z=0.0;
	*new { arg argx=0.0, argy=0.0, argz=0.0;
		^super.new.x_(argx)
			.y_(argy)
			.z_(argz);
	}

	rho {^(x.squared+y.squared+z.squared).sqrt}
	theta {^(atan2(y,x))}					// atan2(x.neg,y)
	phi {^(atan2( hypot(x,y), z.neg)-0.5pi)}	// atan2( hypot(x,y), z.neg)-0.5pi
	dist {arg aPoint;
		var argx, argy, argz;
		argx = aPoint.x; argy = aPoint.y;
		argz = aPoint.respondsTo(\z).if({aPoint.z},{0});
		^((x-argx).squared + (y-argy).squared + (z-argz).squared).sqrt
		}

	asZPoint {^this}
	asPoint {^Point.new(x,y)}
	asPolar {^Polar.new( hypot(x,y), this.theta )}
	asZPolar {^ZPolar.new( this.rho, this.theta, this.phi )}
}

PointSource {
	var <>sig, <>thepolar;

	*new { arg argsig, argthepolar;
		^super.new.sig_(argsig)
			.thepolar_(argthepolar);
	}

	*newt { arg argsig, rho=0.0, theta=0.0, phi;
		^super.new.sig_(argsig)
			.thepolar_(phi.isNil.if({Polar.new(rho, theta)},{ZPolar.new(rho, theta, phi)}));
	}

	x {^thepolar.x}
	y {^thepolar.y}
	z {^(this.dims == 3).if({thepolar.z},{0.0})}

	asPoint {^thepolar.asPoint}
	asPolar {^thepolar.isKindOf(Polar).if({thepolar},{thepolar.asPolar})}
	asZPolar {^thepolar.isKindOf(ZPolar).if({thepolar},{ZPolar.new(thepolar.rho,thepolar.theta)})}
	asZPoint {^thepolar.isKindOf(ZPolar).if({thepolar.asZPoint},{ZPoint.new(this.x,this.y)})}

	dist {^thepolar.rho}
	angle {^thepolar.angle}
	theta {^thepolar.theta}
	rho {^thepolar.rho}
	phi {^(this.dims == 3).if({thepolar.phi}, {0.0})}
	polar {^this.asPolar}
	point {^this.asPolar}

	ar {^sig}

	dims {^thepolar.respondsTo(\phi).if({3},{2})}
}
