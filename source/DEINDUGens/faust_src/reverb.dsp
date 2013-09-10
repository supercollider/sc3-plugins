declare name        "JP_dreamverb_proto";
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
 
aisti = (1, 1.166666666, 1.5333333333);

allpass_stretched(maxdel,N,a) 	= (+ <: fdelay1a(maxdel,N-1),*(a)) ~ *(-a) : mem, _ : + ;

prime_delays = ffunction(int primes (int),<jprev.h>,"primes");

gi = 1.618;
depth = hslider("mDepth",5.0,0.0,50.0,0.01):smooth(0.99);
freq = hslider("mFreq",2.0,0.0,10.0,0.01):smooth(0.99);
wet = hslider("wet",0.5,0.0,1.0,0.01):smooth(0.99);
low = hslider("lowX",1.0,0.0,1.0,0.01):smooth(0.99);
mid = hslider("midX",1.0,0.0,1.0,0.01):smooth(0.99);
high = hslider("highX",1.0,0.0,1.0,0.01):smooth(0.99);
early_diff = hslider("earlyDiff", 0.707, 0.0 ,0.99,0.001);
low_cutoff = hslider("lowBand",500,100.0,6000.0,0.1):smooth(0.99);
high_cutoff = hslider("highBand",2000,1000.0,10000.0,0.1):smooth(0.99);
size = hslider("size",1.0, 0.7,5.0,0.01);
T60 = hslider("t60",1.0,0.1,60.0,0.1):smooth(0.99);
damping = hslider("damp",0.0,0.0,0.999,0.0001):smooth(0.99);
calib = 1.5; // Calibration constant given by T60 in seconds when fb = 0.5
total_length = calib*0.1*(size*5/4 -1/4);
fb = 10^(-3/((T60)/(total_length)));


diffuser(angle,g,scale1,scale2) = bus(2) : 
				(bus(4) :> bus(2) <: 
				(rotator(angle) : (delay(65536, prime_delays(size*scale1) -1 ),delay(65536, prime_delays(size*scale2) -1  ) ) ) ,(*(g),*(g)) )
				 ~ (*(-g),*(-g))
			    :( (mem,mem), bus(2) ) :> bus(2) 
with
{
	rotator(angle) = bus(2) <: (*(c),*(-s),*(s),*(c)) :(+,+) : bus(2)
	with{
		c = cos(angle);
		s = sin(angle);
	};
};

reverb = (	( bus(4) :>  (fdelay4(65536, depth + depth*oscrs(freq) +1 ),fdelay4(65536, depth + depth*oscrc(freq)+ 1 )  ) : seq(i,3,diffuser(PI/4,early_diff,25+40*i,180-33*i) ) 
	
			) ~(
				seq(i,5,diffuser(PI/4,0.707,10+20*i,100 + 5*i) ): par(i,2,fdelay4(65536, depth + (-1^i)*depth*oscrc(freq)+prime_delays(size*(174-150*i)) -1 )) :
				seq(i,5,diffuser(PI/4,0.707,95+20*i, 23+7*i) ): par(i,2,fdelay4(65536, depth + (-1^i)*depth*oscrs(freq)+prime_delays(size*(124-100*i)) -1 )) :
				par(i,2, filterbank(5,(low_cutoff,high_cutoff) ):(_*(high),_*(mid),_*(low)) :> _ ):
				par(i,2,smooth(damping)):
				par(i,2,*(fb))
			));
reverb2 = (	( bus(4) :>   seq(i,1,diffuser(PI/4,0.0,25+40*i,150+10*i) ) 
	
			) ~(
				seq(i,3,diffuser(PI/4,early_diff,10+20*i,100+32*i) ): 
				(fdelay1a(65536, size*prime_delays(100) -1 ),fdelay1a(65536, size*prime_delays(200) -1 )) :
				par(i,2,*(0.7))
			));

//Alesis Style 2-channel
process = (_,_) <: ((reverb:par(i,2,_*(wet))), par(i,2,_*(1-wet))) :> (_,_);


