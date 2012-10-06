MatchingP : MultiOutUGen {

        *ar { arg dict=0, in=0, dictsize=1, ntofind=1, hop=1, method=0;
                ^this.multiNew('audio', dict, in, dictsize, ntofind, hop, method)
        }
        *kr { arg dict=0, in=0, dictsize=1, ntofind=1, hop=1, method=0;
                ^this.multiNew('control', dict, in, dictsize, ntofind, hop, method)
        }
        init { arg ... theInputs;
                inputs = theInputs;
                ^this.initOutputs(2 + (theInputs[3]*2), rate);
        }
        *categories { ^ #["UGens>Buffer", "UGens>Analysis"] }
}

MatchingPResynth : UGen {
        *ar { arg dict, method=0, trigger, residual=0 ... activs;
                ^this.multiNewList(['audio', dict, method, (activs.size / 2), trigger, residual] ++ activs)
        }
        *kr { arg dict, method=0, trigger, residual=0 ... activs;
                ^this.multiNewList(['control', dict, method, (activs.size / 2), trigger, residual] ++ activs)
        }
        *categories { ^ #["UGens>Buffer", "UGens>Analysis"] }
}

