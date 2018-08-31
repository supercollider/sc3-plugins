Henon2DN : UGen {
	*ar { arg minfreq=11025, maxfreq=22050, a=1.4, b=0.3, x0=0.30501993062401, y0=0.20938865431933, mul=1, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, a, b, x0, y0).madd( mul, add )
	}
	*kr { arg minfreq=40, maxfreq=100, a=1.4, b=0.3, x0=0.30501993062401, y0=0.20938865431933, mul=1, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, a, b, x0, y0).madd( mul, add )
	}
}

Henon2DL : Henon2DN { }

Henon2DC : Henon2DN { }

HenonTrig : UGen {
	*ar { arg minfreq=5, maxfreq=10, a=1.4, b=0.3, x0=0.30501993062401, y0=0.20938865431933, mul=1, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, a, b, x0, y0).madd( mul, add )
	}
	*kr { arg minfreq=5, maxfreq=10, a=1.4, b=0.3, x0=0.30501993062401, y0=0.20938865431933, mul=1, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, a, b, x0, y0).madd( mul, add )
	}
}

Gbman2DN : UGen {
	*ar { arg minfreq=11025, maxfreq=22050, x0=1.2, y0=2.1, mul=1, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, x0, y0).madd( mul, add )
	}
	*kr { arg minfreq=40, maxfreq=100, x0=1.2, y0=2.1, mul=1, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, x0, y0).madd( mul, add )
	}
}

Gbman2DL : Gbman2DN { }

Gbman2DC : Gbman2DN { }

GbmanTrig : UGen {
	*ar { arg minfreq=5, maxfreq=10, x0=1.2, y0=2.1, mul=1, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, x0, y0).madd( mul, add )
	}
	*kr { arg minfreq=5, maxfreq=10, x0=1.2, y0=2.1, mul=1, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, x0, y0).madd( mul, add )
	}
}

Standard2DN : UGen {
	*ar { arg minfreq=11025, maxfreq=22050, k=1.4, x0=4.9789799812499, y0=5.7473416156381, mul=1, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, k, x0, y0).madd( mul, add )
	}
	*kr { arg minfreq=40, maxfreq=100, k=1.4, x0=4.9789799812499, y0=5.7473416156381, mul=1, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, k, x0, y0).madd( mul, add )
	}
}

Standard2DL : Standard2DN { }

Standard2DC : Standard2DN { }

StandardTrig : UGen {
	*ar { arg minfreq=5, maxfreq=10, k=1.4, x0=4.9789799812499, y0=5.7473416156381, mul=1, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, k, x0, y0).madd( mul, add )
	}
	*kr { arg minfreq=5, maxfreq=10, k=1.4, x0=4.9789799812499, y0=5.7473416156381, mul=1, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, k, x0, y0).madd( mul, add )
	}
}

Latoocarfian2DN : UGen {
	*ar { arg minfreq=11025, maxfreq=22050, a=1, b=3, c=0.5, d=0.5, x0=0.34082301375036, y0= -0.38270086971332, mul=1, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, a, b, c, d, x0, y0).madd( mul, add )
	}
	*kr { arg minfreq=40, maxfreq=100, a=1, b=3, c=0.5, d=0.5, x0=0.34082301375036, y0= -0.38270086971332, mul=1, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, a, b, c, d, x0, y0).madd( mul, add )
	}
}

Latoocarfian2DL : Latoocarfian2DN { }

Latoocarfian2DC : Latoocarfian2DN { }

LatoocarfianTrig : UGen {
	*ar { arg minfreq=5, maxfreq=10, a=1, b=3, c=0.5, d=0.5, x0=0.34082301375036, y0= -0.38270086971332, mul=1, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, a, b, c, d, x0, y0).madd( mul, add )
	}
	*kr { arg minfreq=5, maxfreq=10, a=1, b=3, c=0.5, d=0.5, x0=0.34082301375036, y0= -0.38270086971332, mul=1, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, a, b, c, d, x0, y0).madd( mul, add )
	}
}

Lorenz2DN : UGen {
	*ar { arg minfreq=11025, maxfreq=22050, s=10, r=28, b=2.6666667, h=0.02, x0=0.090879182417163, y0=2.97077458055, z0=24.282041054363, mul=1.0, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, s, r, b, h, x0, y0, z0).madd(mul, add)
	}
	*kr { arg minfreq=40, maxfreq=100, s=10, r=28, b=2.6666667, h=0.02, x0=0.090879182417163, y0=2.97077458055, z0=24.282041054363, mul=1.0, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, s, r, b, h, x0, y0, z0).madd(mul, add)
	}
}

Lorenz2DL : Lorenz2DN { }

Lorenz2DC : Lorenz2DN { }

LorenzTrig : UGen {
	*ar { arg minfreq=11025, maxfreq=22050, s=10, r=28, b=2.6666667, h=0.02, x0=0.090879182417163, y0=2.97077458055, z0=24.282041054363, mul=1.0, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, s, r, b, h, x0, y0, z0).madd(mul, add)
	}
	*kr { arg minfreq=40, maxfreq=100, s=10, r=28, b=2.6666667, h=0.02, x0=0.090879182417163, y0=2.97077458055, z0=24.282041054363, mul=1.0, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, s, r, b, h, x0, y0, z0).madd(mul, add)
	}
}

Fhn2DN : UGen {
	*ar { arg minfreq=11025, maxfreq=22050, urate=0.1, wrate=0.1, b0=0.6, b1=0.8, i=0.0, u0=0.0, w0=0.0, mul=1.0, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, urate, wrate, b0, b1, i, u0, w0).madd(mul, add)
	}
	*kr { arg minfreq=40, maxfreq=100, urate=0.1, wrate=0.1, b0=0.6, b1=0.8, i=0.0, u0=0.0, w0=0.0, mul=1.0, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, urate, wrate, b0, b1, i, u0, w0).madd(mul, add)
	}
}

Fhn2DL : Fhn2DN { }

Fhn2DC : Fhn2DN { }

FhnTrig : UGen {
	*ar { arg minfreq=4, maxfreq=10, urate=0.1, wrate=0.1, b0=0.6, b1=0.8, i=0.0, u0=0.0, w0=0.0, mul=1.0, add=0.0;
		^this.multiNew('audio', minfreq, maxfreq, urate, wrate, b0, b1, i, u0, w0).madd(mul, add)
	}
	*kr { arg minfreq=4, maxfreq=10, urate=0.1, wrate=0.1, b0=0.6, b1=0.8, i=0.0, u0=0.0, w0=0.0, mul=1.0, add=0.0;
		^this.multiNew('control', minfreq, maxfreq, urate, wrate, b0, b1, i, u0, w0).madd(mul, add)
	}
}