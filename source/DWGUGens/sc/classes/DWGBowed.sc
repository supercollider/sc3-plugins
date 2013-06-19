DWGBowedSimple : UGen
{
	*ar { arg freq=440, velb=0.5, force=1, gate=1, pos=0.14, release=0.1, c1=1, c3=30;
		^this.multiNew('audio',freq, velb, force, gate,pos,release,c1,c3);
	}
}
DWGBowed : UGen
{
	*ar { arg freq=440, velb=0.5, force=1, gate=1, pos=0.14, release=0.1, c1=1, c3=3,impZ=0.55, fB=2;
		^this.multiNew('audio',freq, velb, force, gate,pos,release,c1,c3,impZ,fB);
	}
}
DWGBowedTor : UGen
{
	*ar { arg freq=440, velb=0.5, force=1, gate=1, pos=0.14, release=0.1, c1=1, c3=3,impZ=0.55, fB=2, mistune=5.2, c1tor=1, c3tor=3000, iZtor=1.8;
		^this.multiNew('audio',freq, velb, force, gate,pos,release,c1,c3,impZ,fB,mistune,c1tor,c3tor,iZtor);
	}
}
DWGSoundBoard : UGen
{
	*ar { arg inp=0,c1=20,c3=20,mix=0.8,d1=199,d2=211,d3=223,d4=227,d5=229,d6=233,d7=239,d8=241;
		^this.multiNew('audio',inp,c1,c3,mix,d1,d2,d3,d4,d5,d6,d7,d8);
	 }
}

