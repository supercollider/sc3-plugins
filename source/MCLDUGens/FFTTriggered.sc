FFTTriggered : UGen
{
	*new { arg buffer, in = 0.0, trig = 0.0, overlaplimit = 0.5;
		"FFTTriggered is deprecated and will be removed. Please use FFT, and use the 'active' and 'hop' arguments to control when it triggers.".warn;
		^this.multiNew('control', buffer, in, trig, overlaplimit)
	}
	*categories { ^ #["UGens>FFT"] }
}

FFTInterleave : UGen
{
	*new { arg chain1, chain2;
		^this.multiNew('control', chain1, chain2)
	}
	*categories { ^ #["UGens>FFT"] }
}