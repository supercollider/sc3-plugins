//SLUGens released under the GNU GPL as extensions for SuperCollider 3, by Nick Collins http://composerprogrammer.com/index.html

SortBuf : UGen
{
	*ar { arg bufnum=0, sortrate=10, reset=0;
		^this.multiNew('audio', bufnum,sortrate,reset);
	}
}


GravityGrid : UGen
{
	*ar { arg reset=0, rate=0.1, newx=0.0, newy=0.0, bufnum, mul = 1.0, add = 0.0;
		^this.multiNew('audio', reset,rate,newx,newy,bufnum? (-1)).madd(mul, add);
	}
}

//must pass in buffer for this version
GravityGrid2 : UGen
{
	*ar { arg reset=0, rate=0.1, newx=0.0, newy=0.0, bufnum, mul = 1.0, add = 0.0;
		^this.multiNew('audio', reset,rate,newx,newy,bufnum).madd(mul, add);
	}
}

//bufnum, 0 capturein 1, capturetrigger 2, duration of repeat 3, ampdropout 4
Breakcore : UGen
{
	*ar { arg bufnum=0, capturein, capturetrigger, duration=0.1, ampdropout;
		^this.multiNew('audio', bufnum, capturein, capturetrigger, duration, ampdropout);
	}
}


//find max amplitude over time
Max : UGen
{
	*kr { arg in, numsamp=64;
		^this.multiNew('control', in, numsamp);
	}
}


//print value every x blocks; no longer needed, use Poll
PrintVal : UGen
{
	*kr { arg in, numblocks=100, id=0;
		^this.multiNew('control', in, numblocks, id);
	}
}

//envelope detection filter from musicdsp.org
EnvDetect : UGen
{
	*ar { arg in, attack=100, release=0;
		^this.multiNew('audio', in, attack, release);
	}
}

FitzHughNagumo : UGen
{
	*ar { arg reset=0, rateu=0.01, ratew=0.01, b0=1, b1=1, initu=0, initw=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', reset,rateu, ratew, b0, b1, initu, initw).madd(mul, add);
	}
}


DoubleWell : UGen
{
	*ar { arg reset=0, ratex=0.01, ratey=0.01, f=1,w=0.001,delta=1, initx=0,inity=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio',reset,ratex,ratey,f,w,delta,initx, inity).madd(mul, add);
	}
}

DoubleWell2 : UGen
{
	*ar { arg reset=0, ratex=0.01, ratey=0.01, f=1,w=0.001,delta=1, initx=0,inity=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio',reset,ratex,ratey,f,w,delta,initx, inity).madd(mul, add);
	}
}

DoubleWell3 : UGen
{
	*ar { arg reset=0, rate=0.01, f=0, delta=0.25, initx=0,inity=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio',reset,rate,f,delta,initx, inity).madd(mul, add);
	}
}


WeaklyNonlinear : UGen
{
	*ar { arg input, reset=0, ratex=1, ratey=1, freq=440,initx=0,inity=0, alpha=0, xexponent=0, beta=0, yexponent=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio',input,reset,ratex,ratey,freq,initx, inity, alpha, xexponent, beta, yexponent).madd(mul, add);
	}
}

WeaklyNonlinear2 : UGen
{
	*ar { arg input, reset=0, ratex=1, ratey=1, freq=440,initx=0,inity=0, alpha=0, xexponent=0, beta=0, yexponent=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio',input,reset,ratex,ratey,freq,initx, inity, alpha, xexponent, beta, yexponent).madd(mul, add);
	}
}


//Look into coupled oscillator networks?
TermanWang : UGen
{
	*ar { arg input=0, reset=0, ratex=0.01, ratey=0.01, alpha=1.0, beta=1.0, eta=1.0, initx=0, inity=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', input, reset, ratex, ratey, alpha, beta, eta, initx, inity).madd(mul, add);
	}
}

LTI : UGen
{
	*ar { arg input, bufnuma=0, bufnumb=1, mul = 1.0, add = 0.0;
		^this.multiNew('audio', input, bufnuma, bufnumb).madd(mul, add);
	}
}


NL : UGen
{
	*ar { arg input, bufnuma=0, bufnumb=1, guard1= 1000.0, guard2= 100.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', input, bufnuma, bufnumb, guard1, guard2).madd(mul, add);
	}
}


NL2 : UGen
{
	*ar { arg input, bufnum=0, maxsizea=10, maxsizeb=10, guard1= 1000.0, guard2= 100.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', input, bufnum, maxsizea, maxsizeb, guard1, guard2).madd(mul, add);
	}
}

LPCError : UGen
{
	*ar { arg input, p=10, mul = 1.0, add = 0.0;
		^this.multiNew('audio', input, p).madd(mul, add);
	}
}


//adding buffer to pass in nonrandom data point and mean start positions
KmeansToBPSet1 : UGen
{
	*ar { arg freq=440, numdatapoints=20, maxnummeans=4, nummeans=4, tnewdata=1, tnewmeans=1, soft=1.0, bufnum, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, numdatapoints, maxnummeans, nummeans, tnewdata, tnewmeans, soft, bufnum ? (-1)).madd(mul, add);
	}
}


Instruction : UGen
{
	*ar { arg bufnum=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio',bufnum).madd(mul, add);
	}
}


//optional to add later; wrapbehaviour, interpolation type
WaveTerrain : UGen
{
	*ar { arg bufnum=0, x, y, xsize=100, ysize=100, mul = 1.0, add = 0.0;
		^this.multiNew('audio',bufnum, x, y, xsize, ysize).madd(mul, add);
	}
}


VMScan2D : MultiOutUGen
{
	*ar { arg bufnum=0, mul = 1.0, add = 0.0;
		^this.multiNew('audio',bufnum).madd(mul, add);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}

}

SLOnset : UGen
{
	*kr { arg input, memorysize1=20, before=5,after=5, threshold=10, hysteresis=10, mul = 1.0, add = 0.0;
		^this.multiNew('control', input, memorysize1, before, after, threshold, hysteresis).madd(mul, add);
	}
}


//TwoTube.ar(input, scatteringcoefficient,lossfactor,d1length,d2length);
TwoTube : UGen
{
	*ar { arg input=0, k=0.01, loss=1.0, d1length=100,d2length=100,  mul = 1.0, add = 0.0;
		^this.multiNew('audio',input, k, loss, d1length, d2length).madd(mul, add);
	}
}


NTube : UGen
{
	*ar { arg input=0, lossarray=1.0, karray, delaylengtharray, mul = 1.0, add = 0.0;
	var allargs;
	var lossarrayfix;

	lossarrayfix= if(lossarray.value.isKindOf(Collection),{lossarray.value},{lossarray.value.dup(delaylengtharray.value.size+1)});

	allargs= ['audio', input]++(lossarrayfix)++(karray.value)++(delaylengtharray.value);

	//[allargs.size, allargs].postln;
	^this.multiNew(*allargs).madd(mul, add);
	}
}



EnvFollow : UGen {

	*ar { arg input, decaycoeff = 0.99, mul = 1.0, add = 0.0;
		^this.multiNew('audio', input, decaycoeff).madd(mul, add)
	}

	*kr { arg input, decaycoeff = 0.99, mul = 1.0, add = 0.0;
		^this.multiNew('control', input, decaycoeff).madd(mul, add)
	}
}



Sieve1 : UGen {

	*ar { arg bufnum, gap = 2, alternate=1, mul = 1.0, add = 0.0;
		^this.multiNew('audio', bufnum, gap, alternate).madd(mul, add)
	}

	*kr { arg bufnum, gap = 2, alternate=1, mul = 1.0, add = 0.0;
		^this.multiNew('control', bufnum, gap, alternate).madd(mul, add)
	}
}

Oregonator : MultiOutUGen
{
	*ar { arg reset=0, rate=0.01, epsilon=1.0, mu=1.0, q=1.0, initx=0.5,inity=0.5,initz=0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio',reset,rate, epsilon, mu, q, initx,inity, initz).madd(mul, add);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(3, rate);
	}
}


Brusselator : MultiOutUGen
{
	*ar { arg reset=0, rate=0.01, mu=1.0, gamma=1.0, initx=0.5,inity=0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio',reset,rate, mu, gamma, initx,inity).madd(mul, add);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}


SpruceBudworm : MultiOutUGen
{
	*ar { arg reset=0, rate=0.1, k1= 27.9, k2= 1.5, alpha = 0.1, beta = 10.1, mu=0.3, rho=10.1, initx=0.9,inity=0.1,mul = 1.0, add = 0.0;
		^this.multiNew('audio',reset,rate, k1, k2, alpha, beta, mu, rho,initx,inity).madd(mul, add);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}
