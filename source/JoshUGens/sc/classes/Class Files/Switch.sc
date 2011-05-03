Switch1 {
	// by Chad Kirby <ckirby@u.washington.edu>
	*new { arg test ... cases;
		var x;
		x = cases.detect({ arg case, i;
			var r;
			r = case.at(0).valueArray( test );
			(r == test) // if we're comparing values
				or: {r.isKindOf(Boolean).if({r},{false})}
		});
		^x.notNil.if({x.at(1).valueArray( test ); true},{false})
	}
}

Case {
	*new { arg ... args;
		^args
	}
}
/*
Emulates a switch statement
by sekhar, modified by CK
*/

Switch {
	var <onValue, hasExecuted, <result=nil;

	// Instance Creation
	*new {arg aValue;
		^super.new.init(aValue);
	}

	// return empty string instead of nil
	resultStr { ^result ? "" }

	// initialize-release
	init {arg aValue;
		onValue = aValue;
		hasExecuted = false;
	}

	// case stuff
	case {arg test, block;
		var result;
		hasExecuted.if({^this});
		result = test.valueArray(onValue);
		((result == onValue) or: {result.isKindOf(Boolean).if({result},{false})}).if({ this.execute(block) });
	}

	caseIncludes {arg test, block;
		var result;
		hasExecuted.if({^this});
		result = test.includes(onValue);
		result.if({ this.execute(block) });
	}

	caseGT { arg test, block;
		var result;
		hasExecuted.if({^this});
		result = (onValue > test);
		result.if({ this.execute(block) });
	}
	caseGTE { arg test, block;
		var result;
		hasExecuted.if({^this});
		result = (onValue >= test);
		result.if({ this.execute(block) });
	}
	caseLT { arg test, block;
		var result;
		hasExecuted.if({^this});
		result = (onValue < test);
		result.if({ this.execute(block) });
	}
	caseLTE { arg test, block;
		var result;
		hasExecuted.if({^this});
		result = (onValue <= test);
		result.if({ this.execute(block) });
	}

	execute {arg block;
		result = block.valueArray(onValue);
		hasExecuted = true;
	}

	fall {
		hasExecuted = false;
	}

	value { ^hasExecuted }
	valueArray { ^hasExecuted }
}
