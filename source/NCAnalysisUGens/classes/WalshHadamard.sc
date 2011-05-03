WalshHadamard : UGen {

	*ar { arg input, which=0,mul=1.0, add=0.0;

		^this.multiNew('audio',input, which).madd(mul, add);
	}
}
