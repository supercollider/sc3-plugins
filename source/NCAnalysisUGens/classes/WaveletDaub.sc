WaveletDaub : UGen {

	*ar { arg input, n=64, which=0,mul=1.0, add=0.0;

		^this.multiNew('audio',input,n, which).madd(mul, add);
	}
}
