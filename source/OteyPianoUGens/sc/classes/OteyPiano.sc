OteyPiano : UGen
{	

	*ar { arg freq=440,vel=1,t_gate=0,rmin=0.35,rmax=2,rampl=4,rampr=8,rcore=1,lmin=0.07,lmax=1.4,lampl= -4,lampr=4,rho=1,e=1,zb=1,zh=0,mh=1,k=0.2,alpha=1,p=1,hpos=0.142,loss=1,detune=0.0003,hammer_type=1;
		^this.multiNew('audio',freq, vel, t_gate, rmin, rmax, rampl, rampr, rcore, lmin, lmax, lampl, lampr, rho, e, zb, zh, mh, k, alpha, p, hpos, loss,detune,hammer_type);
	}
}

OteyPianoStrings : UGen
{
	*ar { arg freq=440,vel=1,t_gate=0,rmin=0.35,rmax=2,rampl=4,rampr=8,rcore=1,lmin=0.07,lmax=1.4,lampl= -4,lampr=4,rho=1,e=1,zb=1,zh=0,mh=1,k=0.2,alpha=1,p=1,hpos=0.142,loss=1,detune=0.0003,hammer_type=1;
		^this.multiNew('audio',freq, vel, t_gate, rmin, rmax, rampl, rampr, rcore, lmin, lmax, lampl, lampr, rho, e, zb, zh, mh, k, alpha, p, hpos, loss,detune,hammer_type);
	}
}

OteySoundBoard : UGen
{
	*ar { arg inp=0,c1=20,c3=20,mix=0.8;
		^this.multiNew('audio',inp,c1,c3,mix);
	}
}