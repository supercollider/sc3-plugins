//Tendency {
//	var <>parX, <>parY, <>parA, <>parB;
//
//	*new {|parX = 1.0, parY = 0.0, parA = 0.1, parB = 0.1|
//		^super.newCopyArgs(parX, parY, parA, parB);
//		}
//
//	at {|time = 0.0, dist = \uniform|
//		^case
//			{dist == \uniform} {this.uniformAt(time)}
//			{dist == \lpRand} {this.lpRandAt(time)}
//			{dist == \hpRand} {this.hpRandAt(time)}
//			{dist == \meanRand} {this.meanRandAt(time)}
//			{dist == \betaRand} {this.betaRandAt(time)}
//			{dist == \cauchy} {this.cauchyRandAt(time)}
//			{dist == \gauss} {this.gaussRandAt(time)}
//			{dist == \poisson} {this.poissonRandAt(time)}
//			{dist == \expRand} {this.expRandAt(time)}
//			{dist == \exponential} {this.expAt(time)}
//			{dist == \gamma} {this.gammaAt(time)}
//			{dist == \laplace} {this.laplaceAt(time)}
//			{dist == \alaplace} {this.alaplaceAt(time)}
//			{dist == \hcos} {this.hcosAt(time)}
//			{dist == \logistic} {this.logisticAt(time)}
//			{dist == \arcsin} {this.arcsinAt(time)}
//			{true} {this.uniformAt(time)};
//	}
//
//	uniformAt {|time|
//		^this.parXAt(time).rrand(this.parYAt(time))
//		}
//
//	parXAt {|time|
//		^this.valAt(parX.value(time), time);
//		}
//
//	parYAt {|time|
//		^this.valAt(parY.value(time), time);
//		}
//
//	valAt {|obj, time|
//		^(obj.isKindOf(Env)).if({obj[time]}, { obj.value(time) })
//		}
//
//	lpRandAt {|time|
//		^min(this.at(time), this.at(time))
//		}
//
//	hpRandAt {|time|
//		^max(this.at(time), this.at(time))
//		}
//
//	meanRandAt {|time|
//		^(this.at(time) + this.at(time) * 0.5)
//		}
//
//	betaRandAt {|time|
//		var sum, rprob1, rprob2, temp, curparY, curparX, i;
//		sum = 2;
//		i = 0;
//		rprob1 = this.valAt(parA, time).reciprocal;
//		rprob2 = this.valAt(parB, time).reciprocal;
//		curparX = this.parXAt(time);
//		curparY = this.parYAt(time);
//		while ({
//			(sum > 1) && (i < 10)
//			}, {
//			temp = 1.0.rand ** rprob1;
//			sum = temp + (1.0.rand ** rprob2);
//			i = i + 1;
//			});
//		^((temp / sum) * (curparX - curparY) + curparY)
//		}
//
//	// parX = mean, parY = spread
////	cauchyRandAt {|time|
////		var tmp;
////		while ({
////			tmp = 1.0.rand;
////			tmp == 0.5;
////			});
////		^(this.parYAt(time) * tan(tmp * pi)) + this.parXAt(time);
////		}
//
//	// cauchy
//	// parX = spread, parA=1 => positive half only
//	cauchyRandAt {|time|
//		var u;
//		u = 1.0.rand;
//		(this.parA == 1).if{ u = u * 0.5 };
//		u = pi * u;
//		^( this.parXAt(time) * tan(u) + this.parYAt(time) );
//	}
//
//	// parX = mean, parY = dev
////	gaussRandAt {|time|
////		^(((-2 * log(1.0.rand)).sqrt * sin(2pi.rand)) * this.parYAt(time)) + this.parXAt(time);
////		}
//
//	// parX = dev
//	gaussRandAt {|time|
//		var a, b;
//		a = 1.0.rand;
//		b = 1.0.rand;
//		^((-2 * log(1 - a)).sqrt * cos(2pi * b) * this.parXAt(time) + this.parYAt(time));
//		}
//
//	// parX = mean
//	poissonRandAt {|time|
//		var count = -1, ranVal, tmp;
//		ranVal = 1.0.rand;
//		tmp = exp(this.parXAt(time).neg);
//		while({
//			count = count + 1;
//			ranVal = ranVal * 1.0.rand;
//			ranVal > tmp;
//			});
//		^count;
//		}
//
//	// don't cross 0! don't do it!
//	expRandAt {|time|
//		^exprand(this.parYAt(time), this.parXAt(time));
//		}
//
//	// exponential with parX = density
//	expAt {|time|
//		var xs, us;
//		xs = log(1.0.rand).neg;
//		^((xs/this.parXAt(time)) + this.parYAt(time))
//		}
//
//	// gamma distribution, parX = mean
//	gammaAt { |time|
//		var sum;
//		sum = 1.0;
//		for(1, this.parXAt(time), { sum = sum * (1 - 1.0.rand) });
//		^(log(sum).neg + this.parYAt(time));
//	}
//
//	// laplace
//	// parX = dispersion or spread
//	laplaceAt { |time|
//		var u, val;
//		u = 1.0.rand * 2.0;
//		(u > 1.0).if({
//			u =  2.0 - u;
//			val = this.parXAt(time).neg * log(u);
//			},{
//			val = this.parXAt(time) * log(u)  ;
//			});
//		^(val + this.parYAt(time))
//	}
//
//	// alaplace
//	// special case of laplace (returns exp(u) instead of log(u)
//	// parX = dispersion or spread
//	alaplaceAt { |time|
//		var u, val;
//		u = 1.0.rand * 2.0;
//		(u > 1.0).if({
//			u =  2.0 - u;
//			val = this.parXAt(time).neg * exp(u);
//			},{
//			val = this.parXAt(time) * exp(u)  ;
//			});
//		^(val + this.parYAt(time))
//	}
//
//	// hcos
//	// hyperbolic cosine distribution
//	hcosAt { |time|
//		var u, val;
//		u = 1.0.rand;
//		val = log(tan(0.5pi * u));
//		^(this.parXAt(time) * val + this.parYAt(time))
//	}
//
//
//	// logistic
//	// logistic distribution
//	// parX = dispersion (beta)
//	// we keep alpha as 1 for simplicity
//	logisticAt { |time|
//		var u, val;
//		u = 1.0.rand;
//		val = log(u.reciprocal - 1);
//		^(this.parXAt(time).neg * val + this.parYAt(time))
//	}
//
//	// arcsin
//	// arcsin distribution
//	arcsinAt { |time|
//		var u, val;
//		u = 1.0.rand;
//		val = (1 - sin(pi * (u - 0.5))) * 0.5;
//		^(this.parXAt(time) * val + this.parYAt(time))
//	}
//
//
//	// may be broken
//	embedInStream {| inval, dist |
//		var startTime, thisVal, thisTime;
//		startTime = thisThread.beats;
//		loop {
//			thisTime = thisThread.beats - startTime;
//			inval = this.at(thisTime, dist, parA, parB);
//			inval.yield;
//			}
//		^inval;
//	}
//
//	asStream { |dist|
//		^Routine({ arg inval; this.embedInStream(inval, dist, parA, parB) });
//	}
//
//}
