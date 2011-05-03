/*
MdaUGens: UGens ported from Paul Kellett's VST plugins http://mda.smartelectronix.com/
MdaPiano ported to SC3 by Dan Stowell.

(c) Paul Kellett, Dan Stowell
The mda plug-ins are released under the MIT license or under the GPL
"either version 2 of the License, or (at your option) any later version".
*/
MdaPiano : MultiOutUGen {
	*ar {
		| freq=440.0, gate=1, vel=100, decay=0.8, release=0.8, hard=0.8, velhard=0.8, muffle=0.8, velmuff=0.8, velcurve=0.8, stereo=0.2, tune=0.5, random=0.1, stretch=0.1, sustain=0, mul=1, add=0 |
		^this.multiNew('audio', freq, gate, vel, decay, release, hard, velhard, muffle, velmuff, velcurve, stereo, tune, random, stretch, sustain).madd(mul, add)
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}
