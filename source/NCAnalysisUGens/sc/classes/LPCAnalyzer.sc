LPCAnalyzer : UGen
{
	*ar { arg input=0, source=0.01, n=256, p=10,testE=0,delta=0.999, windowtype=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio',input, source, n, p, testE, delta, windowtype).madd(mul, add);
	}
}