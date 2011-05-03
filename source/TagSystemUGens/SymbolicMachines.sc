

Dfsm : DUGen {
	*new { arg rules, n = 1, rgen;
		^this.multiNewList(['demand', n, rgen ? Dwhite.new]
			++ this.convertRules(rules))
	}

	*convertRules { arg rules;
		var states, nextStates, sizes, data, res;

		// if exit state not given, add one.
		if(rules.size.odd) { rules = rules ++ 0.0 };

		// reformat in a more uniform way.
		// exitstate is treated as initial state

		rules = rules.rotate(1);

		states = rules[0, 2..];
		nextStates = rules[1, 3..];

		sizes = nextStates.collect { |x| x.asArray.size };
		data = states ++ nextStates.flat;

		res = [states.size] ++ sizes ++ data;
		res.do { |x, i|
			postf("% : %\n", i + 3, x.asCompileString)
		};
		^res;
	}

}
