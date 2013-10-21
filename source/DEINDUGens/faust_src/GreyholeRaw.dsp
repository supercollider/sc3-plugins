declare name        "GreyholeRaw";
declare version     "1.0";
declare author      "Julian Parker";
declare license     "none";
declare copyright   "(c) Julian Parker 2013";


import("math.lib"); 
import("maxmsp.lib"); 
import("music.lib"); 
import("oscillator.lib"); 
import("reduce.lib"); 
import("filter.lib"); 
import("effect.lib"); 

fb = hslider("feedback",0.9,0.0,1.0,0.01):linear_interp;
depth = ((SR/44100)*50*hslider("modDepth",0.1,0.0,1.0,0.001)):linear_interp;
freq = hslider("modFreq",2.0,0.0,10.0,0.01):linear_interp;
diff = hslider("diffusion",0.5,0.0,0.99,0.0001):linear_interp;
dt = min(65533,SR*hslider("delayTime",0.2,0.001,1.45,0.0001));
size = hslider("size",1.0,0.5,3.0,0.0001);
damp = hslider("damping", 0.0, 0.0, 0.99,0.001):linear_interp;

linear_interp = _<:(mem,_):+:*(0.5):_;

smooth_init(s,default) = *(1.0 - s) : + ~ (+(default*init(1)):*(s))
with
{
init(value) = value - value';
};

prime_delays = ffunction(int primes (int),<jprev.h>,"primes");

diffuser_nested(1,angle,g,scale) = bus(2) <: ( (bus(2)	:par(i,2,*(c_norm))

													  	: (( bus(4) :> bus(2) :rotator(angle) : (
																fdelay1a(8192, prime_delays(size*scale):smooth_init(0.9999,prime_delays(size*scale)) -1  ),
																				fdelay1a(8192, prime_delays(size*scale +10):smooth_init(0.9999,prime_delays(size*scale + 10)) -1  ) 
																) )~par(i,2,*(-s_norm)))
														: par(i,2,mem:*(c_norm)) )
												, 
												par(i,2,*(s_norm))) 
											:> bus(2)
with
{
	rotator(angle) = bus(2) <: (*(c),*(-s),*(s),*(c)) :(+,+) : bus(2)
	with{
		c = cos(angle);
		s = sin(angle);
	};
	c_norm = cos(g);
	s_norm = sin(g);
};



diffuser_nested(N,angle,g,scale) = bus(2) <: ( (bus(2)	:par(i,2,*(c_norm))

													  	: (( bus(4) :> bus(2) : diffuser_nested(N-1,angle,g,scale+13) : rotator(angle) : (
																fdelay1a(8192, prime_delays(size*scale):smooth_init(0.999,prime_delays(size*scale)) -1  ),
																				fdelay1a(8192, prime_delays(size*scale +10):smooth_init(0.999,prime_delays(size*scale + 10)) -1  ) 
																) )~par(i,2,*(-s_norm)))
														: par(i,2,mem:*(c_norm)) )
												, 
												par(i,2,*(s_norm))) 
											:> bus(2)
with
{
	rotator(angle) = bus(2) <: (*(c),*(-s),*(s),*(c)) :(+,+) : bus(2)
	with{
		c = cos(angle);
		s = sin(angle);
	};
	c_norm = cos(g);
	s_norm = sin(g);
};


//Black Hole
process= 	( bus(4) :> seq(i,3,diffuser_nested(4,PI/2,(-1^i)*diff,10+19*i) ):par(i,2,smooth(damp)) ) 
			 ~( (fdelay4(512, 10+depth + depth*oscrc(freq)),fdelay4(512, 10+ depth + depth*oscrs(freq)) ) :
			 	(sdelay(65536,44100/2,floor(dt)),sdelay(65536,44100/2,floor(dt))) :
				par(i,2,*(fb)) );

