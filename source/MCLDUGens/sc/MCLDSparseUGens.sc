MatchingP : MultiOutUGen {

        *ar { arg dict=0, in=0, dictsize=1, ntofind=1, hop=1, method=0;
                ^this.multiNew('audio', dict, in, dictsize, ntofind, hop, method)
        }
        *kr { arg dict=0, in=0, dictsize=1, ntofind=1, hop=1, method=0;
                ^this.multiNew('control', dict, in, dictsize, ntofind, hop, method)
        }
        init { arg ... theInputs;
                inputs = theInputs;
                ^this.initOutputs(2 + theInputs[2], rate);
        }
        *categories { ^ #["UGens>Buffer", "UGens>Analysis"] }
}

MatchingPResynth : UGen {
	// NB this is just for convenience and is not infallible - it doesn't handle overlapping activations correctly! fine if hop=1 though.
	*ar { |dict=0, trig=0 ... activations|
		^(PlayBuf.ar(activations.size, dict, trigger: trig, loop: 0) * activations).sum;
	}
}

