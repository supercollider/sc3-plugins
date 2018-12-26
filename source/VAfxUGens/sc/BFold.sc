BFold : UGen {
    *ar { arg in, fold=0.0, offset=0.0;
        ^this.multiNew('audio', in, fold, offset)
    }
}
