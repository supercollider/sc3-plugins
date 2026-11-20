BFold : UGen {
    *ar { arg in, fold=0.0, offset=0.0;
        ^this.multiNew('audio', in, fold, offset)
    }
    checkInputs {
        [0, 1].do { |i|
            (inputs[i].rate != 'audio').if {
                ^(" input at index " + i + "(" + inputs.at(i) +
            ") is not audio rate");
            };
        };
        ^this.checkValidInputs;
    }
}
