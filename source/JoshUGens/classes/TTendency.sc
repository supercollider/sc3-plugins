TTendency : UGen {

	*ar {arg trigger, dist = 0, parX = 0, parY = 1, parA = 0, parB = 0;
		^this.multiNew('audio', trigger, dist, parX, parY, parA, parB);
	}

	*kr {arg trigger, dist = 0, parX = 0, parY = 1, parA = 0, parB = 0;
		^this.multiNew('control', trigger, dist, parX, parY, parA, parB);
	}
}

/*
dist types:
	0 = uniform
	1 = lowpass
	2 = highpass
	3 = mean
	4 = beta
	5 = cauchy
	6 = gauss
	7 = poisson
	8 = expRand
	9 = exponential
	10 = gamma
	11 = laplace
	12 = alaplace
	13 = hcos
	14 = logistic
	15 = arcsin

*/