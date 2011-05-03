/*
	Probability RNGs by Lance Putnam (lance@uwalumni.com)

	These classes were ported from BASIC from the book
	Automated Music Composition by Phil Winsor.

	The number receiving the PDF message is by default the mean of the
	distribution and is referred to as 'this'.
*/

+SimpleNumber {

	/* Eularian Beta Distribution
		Smaller prob values produce higher probabilities near respective
		boundaries.  The mean is 'this'/('this'+'val2'). Be careful when
		using prob values > 5 as this can use a lot of CPU.

		Special cases:
		prob1<1, prob2<1
			The probabilities are greatest near 'this' and 'val2'.
		prob1=prob2=1
			This is a uniform distribution.
		prob1>1, prob2>1
			This a bounded Gaussian-type distribution.  The mean is the
			midpoint of the range if prob1=prob2.  If prob1 != prob2,
			the mean will be skewed to the lower probabilities respective
			bound.
		prob1=1, prob2=2
			This is a linear distribution.

		val2: values will be in the range ['this', 'val2']
		prob1: determines the probability of values near 'this'
		prob2: determines the probability of values near 'val2'
	*/
	betarand { arg val2=1, prob1=1, prob2=1;
		var invProb1, invProb2, temp, sum;

		invProb1 = 1/prob1;
		invProb2 = 1/prob2;
		sum = 2;

		while({
			sum > 1;
		},{
			temp = 1.0.rand ** invProb1;
			sum = temp + (1.0.rand ** invProb2);
		});

		^( ((temp/sum) * (val2-this)) + this )
	}

	/* Cauchy Distribution
		A symmetric distribution centered around mean.  It is unbounded
		above and below the mean similar to the Gaussian dist. but with
		a higher occurance of remote values.

		spread: determines horizontal dispersion of values along curve
	*/
	cauchy { arg spread=1.0;
		var randNum;

		randNum = 0.5;

		while({
			randNum == 0.5; // to avoid discontinuity at tan(pi/2)
		},{
			randNum = 1.0.rand;
		});

		^( (spread * tan(randNum * pi)) + this )
	}

	/* Gaussian Distribution (aka Normal Distribution)
		Generates a bell-shaped curve centered around mean.  Good for
		simulating life-like behavior and mutations.

		dev: determines the spread of values above and below mean
	*/

	// fast Box-Muller algorithm
	gaussian { arg dev=1;
		^(((-2*log(1.0.rand)).sqrt * sin(2pi.rand)) * dev) + this
	}

	/* Knuth's Box-Muller algorithm (not as fast as above)
	gaussian2 { arg dev=1;
		var result, r, theta, x, y;

		//if(valBM.notNil, {
		//	result = valBM;
		//	valBM = nil;
		//	^((result * dev) + this)
		//},{
			result = 2;
			while({
				result >= 1;
			},{
				x = rrand(-1.0,1);
				y = rrand(-1.0,1);
				result = (x*x) + (y*y);
			});
			result = ((-2 * log(result))/result).sqrt;
			//valBM = result * x;
			y = result * y;
			^((y * dev) + this)
		//});
	}
	*/

	/* Linear Distribution
		This is similar to exprand, but with a looser distribution.
		Values closer to 'this' are more likely to occur.

		val2: Produces values in range ['this', 'val2'].
	*/
	linrrand { arg val2=1.0;
		var randNum, result;

		randNum = 1.0.rand;
		result = 1.0.rand;

		if( randNum < result, { result = randNum })

		^(result*(val2-this) + this)
	}

	logistic { arg spread=1;
		var u;
		u = 1.0.rand;
		^( (log(u/(1-u)))*spread + this )
	}

	pareto { arg shape=1;
		^( (1.0.rand ** (-1/shape))*this )
	}

	/* Poisson Distribution
		This function models the occurance rate of rare events.
		This is a discrete distribution that returns positive integer values.
		The distribution of values is unbounded at the upper end.  The mean
		and variance of the function are 'this'.
	*/
	poisson {
		var count, someNum, temp;

		count = 0;
		someNum = 1.0.rand;
		temp = exp(this.neg);

		while({
			someNum > temp;
		},{
			count = count + 1;
			someNum = someNum * 1.0.rand;
		});

		^count
	}

	/* Weibull Distribution
		This is a complex, yet powerful, distribution relying on 3 parameters.
		These are known as the location, spread, and shape parameters.
		The 'location' and 'spread' params are used interchangeably to set the
		mean.  The 'shape' param alters the curve of the distribution.

		Effects of 'shape'

		0 < 'shape' <= 1
			-occurance of values near 'location' increases as 'shape'->0.

		'shape' = 1
			-same as exponential distribution

		this: location, or offset, parameter
		spread: distribution range scaling factor
		shape: curve shaping parameter
	*/
	weibull { arg spread, shape=1;
		var randNum = 1.0;

		// this will avoid a div by 0
		while({
			randNum == 1.0
		},{
			randNum = 1.0.rand;
		});

		^( (spread* (log(randNum).neg ** (1.0/shape))) + this )
	}
}

+ SequenceableCollection {

	// in place gaussian operation on array
	gaussian { arg dev=1;
		var result, r, theta, pairs, inc;

		pairs = this.size.div(2);
		inc = 0;

		pairs.do({
			r = ((-2*log(1.0.rand)).sqrt) * dev;
			theta = 2pi.rand;
			this[inc] = this[inc] + (r * cos(theta));
			inc = inc + 1;
			this[inc] = this[inc] + (r * sin(theta));
			inc = inc + 1;
		});

		if(inc < this.size, {
			this[inc] = this[inc].gaussian(dev);
		});

		^this
	}

	cauchy { arg spread=1; ^this.performBinaryOp('cauchy', spread) }
	logistic { arg spread=1; ^this.performBinaryOp('logistic', spread) }
	pareto { arg shape=1; ^this.performBinaryOp('pareto', shape) }
	poisson { ^this.performUnaryOp('poisson') }
}
