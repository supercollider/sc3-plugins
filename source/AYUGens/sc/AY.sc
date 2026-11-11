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

	*freqtotone { |freq, sampleRate|
		var chipTactsPerOutCount;
		sampleRate = sampleRate ?? { Server.default.sampleRate };
		// this mirrors ayemu's internal calculation of ChipTacts_per_outcount
		chipTactsPerOutCount = 1773400.div(sampleRate).div(8);
		^(sampleRate * chipTactsPerOutCount / (2 * freq)).round
	}
}
