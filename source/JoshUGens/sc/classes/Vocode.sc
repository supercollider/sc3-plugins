Vocode {
	*ar {arg src, harm, noise, anabwscale = 0.5, outbw = 0.01, freqmul = 1, mul = 1, add = 0;
		var freqArray, bwArray, ana, amps, freqs, pch, haspch, sigout;
		freqArray = [50, 150, 250, 350, 450, 570, 700, 840, 1000, 1170, 1370, 1600, 1850, 2150,
			2500, 2900, 3400, 4000, 4800];
		bwArray = [50, 50, 50, 50, 60, 70, 80, 80, 100, 110, 130, 150, 180, 200, 250, 400, 500,
			600, 700] * anabwscale;
		ana = BPF.ar(src, freqArray, bwArray / freqArray);
		amps = Amplitude.ar(ana); //should be ana
		freqs = Array.fill(freqArray.size,
			{arg i;
				Pitch.kr(ana[i], freqArray[i], freqArray[i] - bwArray[i],
					freqArray[i] + bwArray[i])[0]}
			);
		#pch, haspch = Pitch.kr(src, 100, 60, 440);
		haspch = Lag2.kr(haspch, 0.01);
		sigout = (harm * haspch) + (noise * (1 - haspch)) ;
		^(Resonz.ar(sigout, freqs * freqmul, outbw) * amps).sum.madd(mul, add);
	}
}

VocodeBand {
	*ar {arg src, band = 0, harm, noise, anabwscale = 0.5, outbw = 0.01, freqmul = 1, mul = 1,
			add = 0;
		var freqArray, bwArray, ana, amps, freqs, pch, haspch, sigout, freq, bw;
		freqArray = [50, 150, 250, 350, 450, 570, 700, 840, 1000, 1170, 1370, 1600, 1850, 2150,
			2500, 2900, 3400, 4000, 4800];
		bwArray = [50, 50, 50, 50, 60, 70, 80, 80, 100, 110, 130, 150, 180, 200, 250, 400, 500,
			600, 700] * anabwscale;
		freq = freqArray[band];
		bw = bwArray[band];
		ana = BPF.ar(src, freq, bw / freq);
		amps = Amplitude.ar(ana); //should be ana
		freqs = Pitch.kr(ana, freq, freq - bw, freq + bw)[0];
		#pch, haspch = Pitch.kr(src, 100, 60, 440);
		haspch = Lag2.kr(haspch, 0.01);
		sigout = (harm * haspch) + (noise * (1 - haspch)) ;
		^(Resonz.ar(sigout, freq * freqmul, outbw) * amps).madd(mul, add);
	}
}