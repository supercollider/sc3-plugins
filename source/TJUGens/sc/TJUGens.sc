DFM1 : UGen {
  *ar { arg in, freq = 1000.0, res = 0.1, inputgain = 1.0, type = 0.0, noiselevel= 0.0003, mul = 1.0, add = 0.0;
    ^this.multiNew('audio', in, freq, res, inputgain, type, noiselevel).madd(mul, add)
  }
}
