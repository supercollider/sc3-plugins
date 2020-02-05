// carrier_wave is "pitch you hear" (like a saw wave, or sound), model_wave is voice, number_of_bands is number of bands
// to be spaced over the threshhold. distance controls how much the output sounds like the model voice. the higher the
// distance, the less it will sound like the model wave
Vocoder {
	*ar {
		arg carrier_wave, model_wave, number_of_bands,
		lowest_band = 20,
		highest_band = 20000,
		distance = 20;
		var octaves, fractional_bandwidth, high_pass_filter, output;
		octaves = log(highest_band / lowest_band) / log(2);
		fractional_bandwidth = sqrt(2) / (octaves / number_of_bands);
		^Mix.arFill(
			number_of_bands + 1,
			{
				arg index;
				var center_frequency;
				center_frequency = lowest_band * ((highest_band / lowest_band) ** (index / number_of_bands));
				BPF.ar(
					LPF.ar(BPF.ar(model_wave, center_frequency, 1 / fractional_bandwidth).abs, center_frequency / distance) *
					BPF.ar(carrier_wave, center_frequency, 1 / fractional_bandwidth),
					center_frequency, 1 / fractional_bandwidth
				)
			}
		)
	}
	*bark {arg signal, input, mull = 10;

		var sourceAmp, freqArray, bwArray;

		freqArray = [50, 150, 250, 350, 450, 570, 700, 840, 1000,
			1170, 1370, 1600, 1850,
			2150, 2500, 2900, 3400, 4000,
			4800, 5800, 7000, 8500, 10500, 13500];
		sourceAmp = Amplitude.kr(input);
		bwArray =
		[50,50,50,50,60,70,80,80,100,110,130,150,180,200,250,400,500,600,700,
			1000,1500,2000,3500,3500];
		^Mix.arFill ( freqArray.size, {arg i;
			var output, freq, bandWidth;
			freq = freqArray.at(i);
			bandWidth = bwArray.at(i);
			output = BPF.ar(signal, freqArray.at(i),bandWidth/freq, sourceAmp)*mull
		})
	}
}