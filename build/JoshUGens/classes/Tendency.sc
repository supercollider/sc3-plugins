Tendency {
	var <>high, <>low, <>parA, <>parB;
	
	*new {|high = 1.0, low = 0.0, parA = 0.1, parB = 0.1|
		^super.newCopyArgs(high, low, parA, parB);
		}
		
	at {|time = 0.0, dist = \uniform|
		^case
			{dist == \uniform} {this.uniformAt(time)}
			{dist == \lpRand} {this.lpRandAt(time)}
			{dist == \hpRand} {this.hpRandAt(time)}
			{dist == \meanRand} {this.meanRandAt(time)}
			{dist == \betaRand} {this.betaRandAt(time)}
			{dist == \cauchy} {this.cauchyRandAt(time)}
			{dist == \gauss} {this.gaussRandAt(time)}
			{dist == \poisson} {this.poissonRandAt(time)}
			{dist == \expRand} {this.expRandAt(time)}
			{true} {this.uniformAt(time)};		
	}
	
	uniformAt {|time|
		^this.highAt(time).rrand(this.lowAt(time))
		}
		
	highAt {|time|
		^this.valAt(high.value(time), time);
		}
		
	lowAt {|time|
		^this.valAt(low.value(time), time);
		}
	
	valAt {|obj, time|
		^(obj.isKindOf(Env)).if({obj[time]}, { obj.value(time) })
		}
		
	lpRandAt {|time|
		^min(this.at(time), this.at(time))
		}
		
	hpRandAt {|time|
		^max(this.at(time), this.at(time))
		}
		
	meanRandAt {|time|
		^(this.at(time) + this.at(time) * 0.5)
		}
		
	betaRandAt {|time|
		var sum, rprob1, rprob2, temp, curlow, curhigh, i;
		sum = 2;
		i = 0;
		rprob1 = this.valAt(parA, time).reciprocal;
		rprob2 = this.valAt(parB, time).reciprocal;
		curhigh = this.highAt(time);
		curlow = this.lowAt(time);
		while ({
			(sum > 1) && (i < 10) 
			}, {
			temp = 1.0.rand ** rprob1;
			sum = temp + (1.0.rand ** rprob2);
			i = i + 1;
			});
		^((temp / sum) * (curhigh - curlow) + curlow)
		}

	// high = mean, low = spread
	cauchyRandAt {|time|
		var tmp;
		while ({
			tmp = 1.0.rand;
			tmp == 0.5;
			});
		^(this.lowAt(time) * tan(tmp * pi)) + this.highAt(time);
		}
	
	// high = mean, low = dev
	gaussRandAt {|time|
		^(((-2 * log(1.0.rand)).sqrt * sin(2pi.rand)) * this.lowAt(time)) + this.highAt(time);
		}
	
	// high = mean	
	poissonRandAt {|time|
		var count = -1, ranVal, tmp;
		ranVal = 1.0.rand;
		tmp = exp(this.highAt(time).neg);
		while({
			count = count + 1;
			ranVal = ranVal * 1.0.rand;
			ranVal > tmp;
			});
		^count;
		}
	
	// don't cross 0! don't do it!
	expRandAt {|time|
		^exprand(this.lowAt(time), this.highAt(time));
		}
	
	// may be broken		
	embedInStream {| inval, dist |
		var startTime, thisVal, thisTime;
		startTime = thisThread.beats;
		loop {
			thisTime = thisThread.beats - startTime;
			inval = this.at(thisTime, dist, parA, parB);
			inval.yield;
			}
		^inval;
	}
		
	asStream { |dist|
		^Routine({ arg inval; this.embedInStream(inval, dist, parA, parB) });
	}
			
}

