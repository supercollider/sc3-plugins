//transient input not currently used but reserved for future use in downweighting frames which have high transient content

KeyMode : UGen {

	*kr { arg chain,keydecay=2.0,chromaleak= 0.5; //transient=0.0;

		^this.multiNew('control',chain,keydecay,chromaleak); //transient;
	}
}