+SimpleNumber {

	midinote {
		var midi, notes;
		midi = (this + 0.5).asInteger;
		notes = ["c ", "c#", "d ", "d#", "e ", "f ", "f#", "g ", "g#", "a ", "a#", "b "];
		^(notes[midi%12] ++ midi.div(12))
	}
}

+String {
	notemidi {
		var twelves, ones, octaveIndex, midis;

		midis = Dictionary[($c->0),($d->2),($e->4),($f->5),($g->7),($a->9),($b->11)];
		ones = midis.at(this[0].toLower);

		if( (this[1].isDecDigit), {
			octaveIndex = 1;
		},{
			octaveIndex = 2;
			if( (this[1] == $#) || (this[1].toLower == $s) || (this[1] == $+), {
				ones = ones + 1;
			},{
				if( (this[1] == $b) || (this[1].toLower == $f) || (this[1] == $-), {
					ones = ones - 1;
				});
			});
		});
		twelves = (this.copyRange(octaveIndex, this.size).asInteger) * 12;
		^(twelves + ones)
	}
}

+SequenceableCollection {

	midinote { ^this.performUnaryOp('midinote') }
	notemidi { ^this.performUnaryOp('notemidi') }
}