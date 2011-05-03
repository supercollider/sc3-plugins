/**
* Cepstrum ugens for SC
* (c) 2009, 2010 Dan Stowell, all rights reserved.
* Released under the GPL, v2 or later.
*/

Cepstrum : PV_ChainUGen
{
	*new { | cepbuf, fftchain |
		^this.multiNew('control', cepbuf, fftchain)
	}
}

ICepstrum : Cepstrum {
	*new { | cepchain, fftbuf |
		^this.multiNew('control', cepchain, fftbuf)
	}
}
