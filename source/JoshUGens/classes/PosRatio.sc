PosRatio : UGen {
	*ar {arg in, period = 100, thresh = 0.1;
		^this.multiNew('audio', in, period, thresh);
	}
}

/*
// period, in hertz to poll for. 100 will look for the number of positive values corresponding to
// 1/100 of a second

s = Server.internal.boot;
s.scope(2)
{a = SinOsc.ar(440, 0, 0.5, MouseX.kr(-0.5, 0.51));[a,PosRatio.ar(a, 10)]}.scope

s.quit;
x-axis (pins horizontal)
1g 0.67
0g 0.55
-1g 0.43
0.67+(0.12*2.75)
InterplPairs([[0, -3.584], [1.0, 2.75]])

y-axis (pins vertical)

1g 0.62
0g 0.5
-1g 0.38
*/