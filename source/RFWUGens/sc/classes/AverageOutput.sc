AverageOutput : UGen {
  *ar {
    arg in, trig=0.0, mul=1.0, add=0.0;
    ^this.multiNew('audio', in, trig).madd(mul, add);
  }

  *kr {
    arg in, trig=0.0, mul=1.0, add=0.0;
    ^this.multiNew('control', in, trig).madd(mul, add);
  }
}
