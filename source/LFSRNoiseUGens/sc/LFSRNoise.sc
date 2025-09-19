LFSRNoise : UGen {
    *ar { |freq = 500.0, fbIndex = 14, reset = 0.0, iState = -1, mul = 1.0, add = 0.0|
        ^this.multiNew('audio', freq, fbIndex, reset, iState).madd(mul, add);
    }
    *kr { |freq = 500.0, fbIndex = 14, reset = 0.0, iState = -1, mul = 1.0, add = 0.0|
        ^this.multiNew('control', freq, fbIndex, reset, iState).madd(mul, add);
    }
}
