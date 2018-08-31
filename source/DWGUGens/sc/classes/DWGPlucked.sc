DWGPlucked : UGen
{
	*ar { arg freq=440, amp=0.5, gate=1, pos=0.14, c1=1, c3=30, inp= 0, release=0.1;
		^this.multiNew('audio',freq, amp, gate,pos,c1,c3,inp,release);
	}
}
DWGPluckedStiff : UGen
{
	*ar { arg freq=440, amp=0.5, gate=1, pos=0.14, c1=1, c3=30, inp= 0, release=0.1, fB=2;
		^this.multiNew('audio',freq, amp, gate,pos,c1,c3,inp,release, fB);
	}
}
DWGPlucked2 : UGen
{
	*ar { arg freq=440, amp=0.5, gate=1, pos=0.14, c1=1, c3=30, inp= 0, release=0.1, mistune=1.008, mp = 0.55, gc = 0.01;
		^this.multiNew('audio',freq, amp, gate,pos,c1,c3,inp,release,mistune,mp,gc);
	}
}
