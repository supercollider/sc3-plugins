PV_CommonMag : UGen
{
	*new { arg bufferA, bufferB, tolerance=0.0, remove=0.0;
		^this.multiNew('control', bufferA, bufferB, tolerance, remove)
	}
}

PV_CommonMul : PV_CommonMag
{
}

PV_MagMinus : UGen
{
	*new { arg bufferA, bufferB, remove=1.0;
		^this.multiNew('control', bufferA, bufferB, remove)
	}
}

PV_MagGate : UGen
{
	*new { arg buffer, thresh=1.0, remove=0.0;
		^this.multiNew('control', buffer, thresh, remove)
	}
}

PV_Compander : UGen
{
	*new { arg buffer, thresh=50, slopeBelow=1, slopeAbove=1;
		^this.multiNew('control', buffer, thresh, slopeBelow, slopeAbove)
	}
}

PV_MagScale : UGen
{
	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}

PV_Morph : UGen
{
	*new { arg bufferA, bufferB, morph=0.0;
		^this.multiNew('control', bufferA, bufferB, morph)
	}
}

PV_XFade : UGen
{
	*new { arg bufferA, bufferB, fade=0.0;
		^this.multiNew('control', bufferA, bufferB, fade)
	}
}

PV_SoftWipe : UGen
{
	*new { arg bufferA, bufferB, wipe=0.0;
		^this.multiNew('control', bufferA, bufferB, wipe)
	}
}

PV_Cutoff : PV_SoftWipe { }