AY : UGen {

	*ar { | tonea=1777, toneb=1666, tonec=1555,
			noise=1, control = 7,
			vola=15, volb=15, volc=15,
			envfreq=4, envstyle=1,
			chiptype = 0,
			mul = 1, add = 0 |

		^this.multiNew('audio', tonea, toneb, tonec,
				noise, control,
				vola, volb, volc,
				envfreq, envstyle,
				chiptype).madd(mul, add)
	}

	*categories { ^ #["UGens>Oscillators"] }

	*freqtotone { |freq|
		// Approximate empirical...
		//^(109300 / (freq - 3.70727))
		^(110300 / (freq - 0.5))
	}
}
