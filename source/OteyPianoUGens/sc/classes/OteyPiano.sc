OteyPiano : UGen
{
	*ar { arg freq=440, amp=0.5, gate=1, release=0.1;
		^this.multiNew('audio',freq, amp, gate, release);
	}
}