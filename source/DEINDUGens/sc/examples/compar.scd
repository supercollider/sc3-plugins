// Compar is a feed-forward resonator network featuring three ComplexRes nodes.
// This code example accompanies the paper "Dynamic FM synthesis using a network of complex resonator filters" by Julian Parker and Till Bovermann. 
// see http://tai-studio.org/index.php/projects/complexres/ for details.



(
Ndef(\compar, {|in = 0|
	var ctlLag = \ctlLag.kr(0.1);

	var src, dst, filterIn, mods, env, tmpIter;
	var ampEnvs;

	// number of FM neurons
	var numNodes = 3;

	// controls
	var preAmp = \preAmp.kr(0.1, ctlLag);
	var postAmp = \postAmp.kr(0.1, ctlLag);
	var dryAmp = \dryAmp.kr(1, ctlLag);
	var filterWet = \filterWet.kr(0.9, ctlLag);
	var reverbWet = \reverbWet.kr(0.9, ctlLag);

	var freqs = numNodes.collect{|i|
		("freq"++i).asSymbol.kr((500 + (i * 100)), ctlLag)
	}.flat;
	var amps = numNodes.collect{|i|
		("amp"++i).asSymbol.kr(1, ctlLag)
	}.flat;
	var fms = numNodes.collect{|i|
		("fm"++i).asSymbol.kr(1, ctlLag)
	}.flat;
	var decays = numNodes.collect{|i|
		("decay" ++i).asSymbol.kr(0.1, ctlLag)
	}.flat;
	
	
	// signal preparation
	src = LeakDC.ar(SoundIn.ar(in));
	filterIn = preAmp * src;
	
	// FM network
	dst = freqs.inject([filterIn, 0], {|in, freq, i|
		tmpIter = ComplexRes.ar( 
			filterIn,
			freq 
			+ (fms[i] * in[0]),
			decays[i]
		);
					
		[
			tmpIter * amps[i],
			in[1] + tmpIter
		]
	});
	dst = Mix.ar(dst * [1,  amps.sum * numNodes.reciprocal]);


	// dryWet
	dst = SelectX.ar(filterWet, [
		OnePole.ar(dryAmp * src, \lpCoeff.kr(0.3, 0.1)), 
		postAmp * LeakDC.ar(dst)
	]);

	// compressor
	dst = Compander.ar(dst,dst,
		thresh: \compThresh.kr(0.5,0.1),
		slopeBelow: 1 ,
		slopeAbove: \compRatio.kr(0.3, 0.1),
		clampTime: 0.0001,
		relaxTime: 0.1
	);

	// reverb + gp limiting
	Limiter.ar(
		SelectX.ar(reverbWet, [dst, AdCVerb.ar(0.1 * dst, 10)])
	);
});
)


(
// control range specifications
Spec.add(\freq0, [1, 20000, \exp]);
Spec.add(\freq1, \freq0);
Spec.add(\freq2, \freq0);
Spec.add(\in, [0, 2, \lin, 1, 1]);
Spec.add(\amp0, [0, 1]);
Spec.add(\amp1, \amp0);
Spec.add(\amp2, \amp0);
Spec.add(\fm0, [0, 10000]);
Spec.add(\fm1, \fm0);
Spec.add(\fm2, \fm0);
Spec.add(\decay0, [0.01, 5, \exp]);
Spec.add(\decay1, \decay0);
Spec.add(\decay2, \decay0);
Spec.add(\filterWet, [0, 1]);
Spec.add(\reverbWet, [0, 1]);
Spec.add(\preAmp, [0.5, 5, \exp]);
Spec.add(\dryAmp, [0.5, 5, \exp]);
Spec.add(\postAmp, [0.5, 50, \exp]);
Spec.add(\lpCoeff, [0, 1]);
)

(
// GUI
Ndef(\compar).gui;
)