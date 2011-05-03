DbufTag : DUGen {
	var <>axiom, <>rules;
	*new { arg bufnum, v, axiom, rules, recycle=0, mode=0;
		^this.multiNewList(['demand', bufnum, v, recycle, mode]
			++ this.convertRules(axiom, rules)).axiom_(axiom).rules_(rules)
	}

	*convertRules { arg axiom, rules;
		^[axiom.size, rules.size] ++ axiom ++ rules.collect(_.size) ++ rules.flatten(1)
	}
	allSymbols {
		^Dseq([
				Dseq(axiom),
				Dswitch(rules.collect { |x| Dseq(x) }, this)
			])
	}

}

Dtag : DbufTag {
	*new { arg bufsize, v, axiom, rules, recycle=0, mode=0;
		^this.multiNewList(['demand', bufsize, v, recycle, mode]
			++ this.convertRules(axiom, rules)).axiom_(axiom).rules_(rules)
	}
}
