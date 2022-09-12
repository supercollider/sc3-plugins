
StkPluck : UGen {
	*ar { arg  freq=440, decay=0.99,   mul = 1.0, add = 0.0;
		^this.multiNew('audio',freq,decay ).madd(mul, add)
	}
	*kr { arg  freq=440, decay=0.99,    mul = 1.0, add = 0.0;
		^this.multiNew('control',freq,decay) .madd(mul, add)
	}
}
StkFlute : UGen {
	*ar { arg  freq=440,jetDelay=49, noisegain=0.15, jetRatio = 0.32,  mul = 1.0, add = 0.0;
		^this.multiNew('audio' ,freq,jetDelay, noisegain, jetRatio).madd(mul, add)
	}
	*kr { arg     freq=220,jetDelay=49, noisegain=0.15, jetRatio = 0.32,  mul = 1.0, add = 0.0;
		^this.multiNew('control',freq,jetDelay, noisegain, jetRatio).madd(mul, add) .madd(mul, add)
	}
}

StkBowed : UGen {
	*ar { arg  freq=220, bowpressure = 64, bowposition = 64, vibfreq=64, vibgain=64, loudness=64,gate=1, attackrate=1, decayrate=1, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq,  bowpressure, bowposition, vibfreq, vibgain, loudness, gate, attackrate, decayrate).madd(mul, add)
	}
	*kr { arg  freq=220, bowpressure = 64, bowposition = 64, vibfreq=64, vibgain=64, loudness=64, trig = 1, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq,  bowpressure, bowposition, vibfreq, vibgain, loudness, trig).madd(mul, add)
	}
}

StkMandolin : UGen {
	*ar { arg  freq=520,   bodysize=64,  pickposition=64,  stringdamping=69,  stringdetune=10,  aftertouch=64, trig = 1,
 mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq,  bodysize,  pickposition,  stringdamping,  stringdetune,  aftertouch, trig).madd(mul, add)
	}
	*kr { arg  freq=220,   bodysize=64,  pickposition=64,  stringdamping=69,  stringdetune=10,  aftertouch=64,trig = 1,  mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, bodysize,  pickposition,  stringdamping,  stringdetune,  aftertouch, trig).madd(mul, add)
	}
}
StkSaxofony : UGen {

	*ar { arg  freq=220, reedstiffness=64,reedaperture=64,noisegain=20,blowposition=26,
			  vibratofrequency=20, vibratogain=20,breathpressure=128,trig = 1, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, reedstiffness,reedaperture,noisegain,blowposition,
					vibratofrequency, vibratogain, breathpressure, trig).madd(mul, add)
	}
	*kr { arg  freq=220, reedstiffness=64,reedaperture=64,noisegain=20,blowposition=26,
				vibratofrequency=20, vibratogain=20,breathpressure=128,trig = 1, mul = 1.0, add = 0.0;
		^this.multiNew('control',  freq, reedstiffness,reedaperture,noisegain,blowposition,
				vibratofrequency, vibratogain, breathpressure, trig).madd(mul, add)
	}
}

StkShakers : UGen {
	classvar shakerTypes;

	*initClass {
		shakerTypes = Dictionary[
			"Maraca" -> 0,
			"Cabasa" -> 1,
			"Sekere" -> 2,
			"Guiro" -> 3,
			"Water Drops" -> 4,
			"Bamboo Chimes" -> 5,
			"Tambourine" -> 6,
			"Sleigh Bells" -> 7,
			"Sticks" -> 8,
			"Crunch" -> 9,
			"Wrench" -> 10,
			"Sand Paper" -> 11,
			"Coke Can" -> 12,
			"Next Mug" -> 13,
			"Penny + Mug" -> 14,
			"Nickle + Mug" -> 15, // Mispelling of "Nickel" in original STK docs
			"Dime + Mug" -> 16,
			"Quarter + Mug" -> 17,
			"Franc + Mug" -> 18,
			"Peso + Mug" -> 19,
			"Big Rocks" -> 20,
			"Little Rocks" -> 21,
			"Tuned Bamboo Chimes" -> 21
		].freeze;
	}

	*ar { arg  instr=0, energy=64, decay=64, objects=64, resfreq=64, mul = 1.0, add = 0.0;
		^this.multiNew('audio',instr, energy, decay, objects, resfreq).madd(mul, add)
	}
	*kr { arg   instr=0, energy=64, decay=64, objects=64, resfreq=64, mul = 1.0, add = 0.0;
		^this.multiNew('control',  instr, energy, decay, objects, resfreq ).madd(mul, add)
	}

	*directory {
		^shakerTypes
	}
}



StkBandedWG : UGen {
	*ar { arg  freq=440,instr=0,  bowpressure=0,bowmotion=0 , integration=0,
				modalresonance=64,bowvelocity=0,setstriking=0,trig=1, mul = 1.0, add = 0.0;
		^this.multiNew('audio',freq,instr,bowpressure,bowmotion,
			integration,modalresonance,bowvelocity,setstriking,trig).madd(mul, add)
	}
	*kr { arg  freq=440,instr=0,  bowpressure=0,bowmotion=0,   integration=0,
				modalresonance=64,bowvelocity=0,setstriking=0,trig=1, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq,instr,bowpressure,bowmotion,
			integration,modalresonance,bowvelocity,setstriking,trig).madd(mul, add)
	}
}

StkVoicForm : UGen {
	*ar { arg  freq=440, vuvmix=64,  vowelphon=64,vibfreq=64 , vibgain=20,loudness=64,trig=1,
					 mul = 1.0, add = 0.0;
		^this.multiNew('audio',freq, vuvmix,vowelphon,vibfreq , vibgain,loudness,trig).madd(mul, add)
	}
	*kr { arg  freq=440, vuvmix=64,  vowelphon=64,vibfreq=64 , vibgain=20,loudness=64,trig=1,
					 mul = 1.0, add = 0.0;
		^this.multiNew('control',freq, vuvmix,vowelphon,vibfreq , vibgain,loudness,trig).madd(mul, add)
	}
}
StkModalBar : UGen {
	*ar { arg  freq=440,  instrument=0, stickhardness=64, stickposition=64, vibratogain=20,
					vibratofreq=20, directstickmix=64,volume=64,trig = 1,
					 mul = 1.0, add = 0.0;
		^this.multiNew('audio',freq, instrument, stickhardness, stickposition, vibratogain,
					vibratofreq, directstickmix,volume,trig
					).madd(mul, add)
	}
	*kr { arg  freq=440,instrument=0, stickhardness=64, stickposition=64, vibratogain=20,
					vibratofreq=20, directstickmix=64,volume=64, trig = 1,
					 mul = 1.0, add = 0.0;
		^this.multiNew('control',freq, instrument, stickhardness, stickposition, vibratogain,
					vibratofreq, directstickmix,volume,trig
				).madd(mul, add)
	}
}
 StkClarinet : UGen {
	*ar { arg  freq=440,reedstiffness=64,noisegain=4,vibfreq=64,vibgain=11,breathpressure=64,trig=1, mul = 1.0, add = 0.0;
		^this.multiNew('audio',freq,reedstiffness,noisegain,vibfreq,vibgain,breathpressure,trig;
					).madd(mul, add)
	}
	*kr { arg  freq=440,reedstiffness=64,noisegain=4,vibfreq=64,vibgain=11,breathpressure=64, trig=1, mul = 1.0, add = 0.0;
		^this.multiNew('control',freq,reedstiffness,noisegain,vibfreq,vibgain,breathpressure,trig;
					).madd(mul, add)
	}
}
StkBlowHole : UGen {
	*ar { arg  freq=440,reedstiffness=64,noisegain=20,tonehole=64,register=11,breathpressure=64, mul = 1.0, add = 0.0;
		^this.multiNew('audio',freq,reedstiffness,noisegain,tonehole,register,breathpressure;
					).madd(mul, add)
	}
	*kr { arg  freq=440,reedstiffness=64,noisegain=4,tonehole=64,register=11,breathpressure=64, mul = 1.0, add = 0.0;
		^this.multiNew('control',freq,reedstiffness,noisegain,tonehole,register,breathpressure;
					).madd(mul, add)
	}
}

StkMoog : UGen {
	*ar { arg  freq=440,filterQ=10,sweeprate=20, vibfreq=64, vibgain=0, gain=64,trig=1,
					mul = 1.0, add = 0.0;
		^this.multiNew('audio',freq,filterQ,sweeprate, vibfreq, vibgain, gain, trig
					).madd(mul, add)
	}
	*kr { arg  freq=440,filterQ=10,sweeprate=20, vibfreq=64, vibgain=0, gain=64,trig=1,
					mul = 1.0, add = 0.0;
		^this.multiNew('control',freq,filterQ,sweeprate, vibfreq, vibgain, gain, trig
					).madd(mul, add)
	}
}
StkBeeThree : UGen {
	*ar { arg  freq=440,op4gain=10,op3gain=20, lfospeed=64, lfodepth=0, adsrtarget=64,trig=1,
					mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq,op4gain,op3gain,lfospeed,lfodepth,adsrtarget,trig
					).madd(mul, add)
	}
	*kr { arg   freq=440,op4gain=10,op3gain=20, lfospeed=64, lfodepth=0, adsrtarget=64,trig=1,
					mul = 1.0, add = 0.0;
		^this.multiNew('control',freq,op4gain,op3gain,lfospeed,lfodepth,adsrtarget,trig
					).madd(mul, add)
	}
}
