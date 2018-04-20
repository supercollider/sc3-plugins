NHHall : MultiOutUGen {
    *ar {
        |
            in,
            rt60 = 1,
            stereo = 0.5,
            lowFreq = 200,
            lowRatio = 0.5,
            hiFreq = 4000,
            hiRatio = 0.5,
            earlyDiffusion = 0.5,
            lateDiffusion = 0.5,
            modRate = 0.2,
            modDepth = 0.3
        |
        in = in.asArray;
        if(in.size != 2) {
            Error(
                "NHHall expects a stereo signal, but received a size % array."
            ).throw;
        };
        ^this.multiNew(
            \audio,
            in[0],
            in[1],
            rt60,
            stereo,
            lowFreq,
            lowRatio,
            hiFreq,
            hiRatio,
            earlyDiffusion,
            lateDiffusion,
            modRate,
            modDepth
        );
    }

    init { |... theInputs|
        inputs = theInputs;
        ^this.initOutputs(2, rate);
    }

    checkInputs { ^this.checkNInputs(2) }
}
