

Stk{

	classvar names;
	classvar namenumber;
	*initClass{
		names = Array.with("Clarinet", "BlowHole", "Saxofony", "Flute", "Brass",
		"BlowBotl", "Bowed", "Plucked", "StifKarp", "Sitar", "Mandolin",
		"Rhodey", "Wurley", "TubeBell", "HevyMetl", "PercFlut",
		"BeeThree", "FMVoices", "VoicForm", "Moog", "Simple", "Drummer",
		"BandedWG", "Shakers", "ModalBar", "Mesh2D", "Resonate", "Whistle");

		namenumber = Dictionary.new;
		names.do({arg item,i; namenumber[item] = i;});
	}

	*at { arg name;
		^namenumber[name];
	}
}

StkInst : UGen {
	*ar { arg instNumber=6,freq=220,gate=1,onamp=1,offamp=0.5,args;
		^this.multiNewList(['audio', freq,gate,onamp,offamp,instNumber]++ args.asArray);
	}
}

StkBowedI {
	*ar {arg freq=220,gate=1, onamp=1, offamp=1, bowpressure = 64, bowposition = 64, vibfreq=64, vibgain=64, loudness=64;
		^StkInst.ar(Stk.at("Bowed"),freq,gate,onamp,offamp,[2,bowpressure,4,bowposition,11,vibfreq,1,vibgain,128,loudness]);
	}
}
StkGlobals : UGen {
	*ar { arg showWarnings, printErrors, rawfilepath;
		var ascii = rawfilepath.ascii;
		^this.multiNewList(['audio', showWarnings, printErrors].addAll(ascii));
	}
}