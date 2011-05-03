Phenon : Pattern {
	var <>a, <>b, <>x, <>y, <>n;
	*new { |a=1.3, b=0.3, x=0.30501993062401, y=0.20938865431933, n=true|
		^super.newCopyArgs(a, b, x, y, n);
	}
	embedInStream {|inval|
		var locala, localb, localx, localy, localn, newx;
		locala = a.copy; localb = b.copy; localx = x.copy; localy = y.copy;
		localn = n.copy;
		loop {
			newx = localy + 1 - (locala * localx.squared);
			localy = localb * (localx);
			localx = newx;
			(localn).if(
				{ ([localx, localy] * [0.77850360953955, 2.5950120317984] + 1 * 0.5).yield },
				{ [localx, localy].yield }
			);
		};
		^inval
	}
}

Platoo : Pattern {
	var <>a, <>b, <>c, <>d, <>x, <>y, <>n;
	*new {|a=3.0, b= -2.0, c=0.7, d=0.9, x=0.34082301375036, y= -0.38270086971332, n=true|
		^super.newCopyArgs(a, b, c, d, x, y, n);
	}
	embedInStream {|inval|
		var locala, localb, localc, locald, localx, localy, localn, newx;
		locala=a.copy; localb=b.copy; localc=c.copy; locald=d.copy; localx=x.copy; localy=y.copy;
		localn=n.copy;
		loop {
			newx=sin(localb*localy)+(localc*sin(localb*localx));
			localy=sin(locala*localy)+(locald*sin(locala*localx));
			localx=newx;
			(localn).if(
				{ ([localx, localy] * [2.8213276124707, 2.4031871436393] + 1 * 0.5).yield },
				{ [localx, localy].yield }
			)
		};
		^inval
	}
}

Plorenz : Pattern {
	var <>s, <>r, <>b, <>h, <>x, <>y, <>z, <>h;
	*new {|s=10, r=28, b=2.66666666667, h=0.01, x=0.090879182417163, y=2.97077458055, z=24.282041054363|
		^super.newCopyArgs(s, r, b, h, x, y, z);
	}
	embedInStream {|inval|
		var localx, localy, localz, localh, sigma, rayleigh, ratio, newx, newy;
		localx=x.copy; localy=y.copy; localz=z.copy; localh=h.copy;
		sigma=s.copy; rayleigh=r.copy; ratio=b.copy;
		loop {
			newx=localh*sigma*(localy-localx)+localx;
			newy=localh*(localx.neg*localz+(rayleigh*localx)-localy)+localy;
			localz=localh*(localx*localy-(ratio*localz))+localz;
			localx=newx; localy=newy;
			([localx, localy, localz] * [0.048269545768799, 0.035757929840258, 0.019094390581019] + 1 * 0.5).yield
		}
		^inval
	}
}

Pquad : Pattern {
	var <>a, <>b, <>c, <>x, <>n;
	*new {|a= -3.741, b=3.741, c=0, x=0.1, n=true|
		^super.newCopyArgs(a, b, c, x, n);
	}
	embedInStream {|inval|
		var locala, localb, localc, localx, localn;
		locala=a.copy; localb=b.copy; localc=c.copy; localx=x.copy; localn=n.copy;
		loop {
			localx=(locala*localx.squared) + (localb*localx) + localc;
			(localn).if({ (localx * 1.0693715927735).yield },
				{localx.yield}
			)
		}
		^inval
	}
}

PlinCong : Pattern {
	var <>a, <>c, <>m, <>x, <>n;
	*new {|a=1.1, c=0.1, m=0.5, x=0.0, n=true|
		^super.newCopyArgs(a, c, m, x, n);
	}
	embedInStream {|inval|
		var locala, localc, localm, localx, localn;
		locala=a.copy; localc=c.copy; localm=m.copy; localx=x.copy; localn=n.copy;
		loop {
			localx=((locala * localx) + localc) % localm;
			(localn).if({ (localx * 2.0000515599933).yield },
				{localx.yield}
			)
		}
		^inval
	}
}

Pstandard : Pattern {
	var <>k, <>x, <>y, <>n;
	*new {|k=1.5, x=4.9789799812499, y=5.7473416156381, n=true|
		^super.newCopyArgs(k, x, y, n)
	}
	embedInStream {|inval|
		var localk, localx, localy, localn;
		localk=k.copy; localx=x.copy; localy=y.copy; localn=n.copy;
		loop {
			localy=(localk * sin(localx) + localy) % 2pi;
			localx=(localx + localy) % 2pi;
			(localn).if(
				{ ([localx, localy] * [0.1591583187703, 0.15915788974082]).yield },
				{ [localx, localy].yield }
			);
		}
		^inval
	}
}

Pgbman : Pattern {
	var <>x, <>y, <>n;
	*new {|x=1.2, y=2.1, n=true|
		^super.newCopyArgs(x, y, n)
	}
	embedInStream {|inval|
		var localx, localy, localn, last_x;
		localx=x.copy; localy=y.copy; localn=n.copy;
		loop {
			last_x=localx;
			(last_x < 0.0).if({ localx = 1.0 - localy - last_x }, { localx = 1.0 - localy + last_x });
			localy = last_x;
			(localn).if({ (localx * 0.12788595029832).yield },
				{localx.yield}
			)
		}
		^inval
	}
}

Pfhn : Pattern {
	var <>a, <>b, <>c, <>d, <>i, <>u, <>v, <>n;
	*new {|a=0.7, b=0.8, c=1.0, d=1.0, i, u= -0.1, v=0.1, n=true|
		^super.newCopyArgs(a, b, c, d, i, u, v, n)
	}
	embedInStream {|inval|
		var la, lb, lc, ld, li, lu, lv, ln, newu;
		la=a.copy; lb=b.copy; lc=c.copy; ld=d.copy; li=i.copy; lu=u.copy; lv=v.copy;
		ln=n.copy;
		li=li ? Pseq([0.0], inf).asStream;
		loop {
			newu=lc * (lu.cubed * -0.33333333 - lv + lu + li.next) + lu;
			lv=ld * (lb * lu + la - lv) + lv;
			lu=newu;
			if ((lu > 1.0) || (lu < -1.0)) {
				lu=((lu - 1)%4.0 - 2.0).abs - 1.0;
			};
			(ln).if({ [lu + 1 * 0.5, lv * 0.5 + 1 * 0.5].yield },
				{ [lu, lv].yield }
			);
		}
		^inval
	}
}