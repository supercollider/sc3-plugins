Tendency {
	var <>high, <>low;
	
	*new {|high, low|
		^super.newCopyArgs(high, low);
		}
		
	at {|time|
		^(this.highAt(time)).rrand(this.lowAt(time))
	}
	
	highAt {|time|
		^this.valAt(high, time);
		}
		
	lowAt {|time|
		^this.valAt(low, time);
		}
	
	valAt {|obj, time|
		^(obj.isKindOf(Env)).if({obj[time]}, { obj.value })
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
		
	betaRandAt {|time, prob1 = 0.5, prob2 = 0.5|
		var sum, rprob1, rprob2, temp, curlow, curhigh, i;
		sum = 2;
		i = 0;
		rprob1 = this.valAt(prob1, time).reciprocal;
		rprob2 = this.valAt(prob2, time).reciprocal;
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
		
	embedInStream {| inval, dist, prob1, prob2 |
		var startTime, thisVal, thisTime;
		startTime = thisThread.beats;
		loop {
			thisTime = thisThread.beats - startTime;
			inval = case 
				{dist == \uniform} {this.at(thisTime)}
				{dist == \lpRand} {this.lpRandAt(thisTime)}
				{dist == \hpRand} {this.hpRandAt(thisTime)}
				{dist == \meanRand} {this.meanRandAt(thisTime)}
				{dist == \betaRand} {this.betaRandAt(thisTime, prob1, prob2)}
				{true} {this.at(thisTime)};
			inval.yield;
			}
		^inval;
	}
		
	asStream { |dist, prob1, prob2|
		^Routine({ arg inval; this.embedInStream(inval, dist, prob1, prob2) });
	}
			
}

