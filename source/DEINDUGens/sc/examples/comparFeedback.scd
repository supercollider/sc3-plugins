// ComparFeedback implements an FM feedback matrix based on the ComplexRes UGen. The number of nodes can be set when defining the synthesis engine. ComparFeedback implements a superset of the Compar system (see Compar.scd in this folder).
// This code example accompanies the paper "Dynamic FM synthesis using a network of complex resonator filters" by Julian Parker and Till Bovermann. 
// see http://tai-studio.org/index.php/projects/complexres/ for details.

(
q = q ? ();
q.numOscs = 10;
q.numChans = 2;
)

(
// careful! the system uses AudioIn as input, feedback might cause serious trouble, possibly use headphones unless you know what you're doing.


Ndef(\comparMatrix, {
	var numChans = q.numChans;
	
	var in = LeakDC.ar(SoundIn.ar(\in.kr));
	var filterIn, feedbacks, filterOut, dst;
	var ctlLag = \ctlLag.kr(0.1);
	var numOscs = q.numOscs;
	

	var preAmp = \preAmp.kr(0.1, ctlLag);
	var postAmp = \postAmp.kr(0.1, ctlLag);
	var dryAmp = \dryAmp.kr(1, ctlLag);
	var filterWet = \filterWet.kr(0.9, ctlLag);
	var reverbWet = \reverbWet.kr(0.9, ctlLag);

	var freqs = \freqs.kr({|i| 500}!numOscs);
	var modParams = \modParams.kr({|i|   0}!(numOscs**2)).clump(numOscs).postln;
	var amps     = \amps    .kr(0!numOscs);
	var decays = \decays.kr(0.01!numOscs);

	var oscs;
	var tmpOsc;


	
	// FM network
	feedbacks = LocalIn.ar(numOscs);

	filterIn = preAmp * in;
	oscs = freqs.inject([], {|oscArray, freq, i|
		tmpOsc = ComplexRes.ar(filterIn, 
			freq
			+ oscArray.inject(0, {|sum, osc, j| 
				// modulators from already instantiated oscs
				sum + (feedbacks[j] * modParams[i][j]) })
			+ (numOscs - 1 - Array.iota(numOscs - (i))).inject(0, {|sum, g| 
				// modulators from to be instantiated oscs
				sum + (feedbacks[g] * modParams[i][g]) }),
			decays[i]
		);
		oscArray ++ tmpOsc;
	}); // end inject

	LocalOut.ar(oscs); // feedback is pre-"fader"
	filterOut = oscs * amps * postAmp;
	// end filter

	// dryWet
	dst = SelectX.ar(filterWet, [
		OnePole.ar(dryAmp * in, \lpCoeff.kr(0.3, 0.1)), 
		postAmp * LeakDC.ar(filterOut)
	]);

	// Compressor
	dst = Compander.ar(dst,dst,
		thresh: \compThresh.kr(0.5,0.1),
		slopeBelow: 1 ,
		slopeAbove: \compRatio.kr(0.3, 0.1),
		clampTime: 0.0001,
		relaxTime: 0.1
	);

	// reverb, channel spreading + gp limiting
	Limiter.ar(
		SelectX.ar(reverbWet, [
			SplayAz.ar(numChans, dst), 
			AdCVerb.ar(0.1 * dst, 10, nOuts: numChans)
		])
	);
})
)

(
// control range specifications
Spec.add(\freq0, [1, 20000, \exp]);
Spec.add(\freq1, \freq0);
Spec.add(\freq2, \freq0);
Spec.add(\in, [0, 2, \lin, 1, 1]);
Spec.add(\filterWet, [0, 1]);
Spec.add(\reverbWet, [0, 1]);
Spec.add(\preAmp, [0.5, 5, \exp]);
Spec.add(\dryAmp, [0.5, 5, \exp]);
Spec.add(\postAmp, [0.5, 50, \exp]);
Spec.add(\lpCoeff, [0, 1]);
)


(
// the standard Ndef gui and an example preset. 
// to hear something, press play.

Ndef('comparMatrix').set('preAmp', 5.0, 'reverbWet', 0.036082474226804, 'postAmp', 7.7229139013412, 'filterWet', 1.0, 'compThresh', 0.11354325669618, 'compRatio', 1.0642438515127);
Ndef('comparMatrix').setn('amps', [ 0.0, 0.0, 0.0625, 0.8875, 0.0, 0.0, 0.2125, 0.275, 0.05, 0.0625 ], 'modParams', [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8697.0, 0, 0, 0, 2275.0, 728.0, 6045.0, 0, 0, 0.0, 169.0, 104.0, 0, 156.0, 104.0, 13.0, 39.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5863.0, 0, 0, 0, 0, 741.0, 0, 0, 0, 9997.0, 3900.0, 0, 0, 9997.0, 0.0, 9997.0, 0.0, 0, 0, 6097.0, 6097.0, 0, 0, 7163.0, 0.0, 0.0, 0, 0, 0, 9997.0, 9100.0, 0, 0, 117.0, 611.0, 13.0, 3887.0 ], 'decays', [ 0.01, 0.01, 0.01, 0.55, 0.65, 1.8, 0.72, 2.97, 0.18, 0.26 ], 'freqs', [ 500, 500, 1537.5, 58.5, 500, 500, 179.5, 500.6, 365.7, 305.8 ]);

Ndef(\comparMatrix).gui;
Ndef(\comparMatrix).setn(\modParams, 0!(q.numOscs**2));

// GUI
(
var specs = (
	freqs: [0, 10000, \lin, 0.1].asSpec,
	modParams: [0, 10000, \lin, 13].asSpec,
	in: [0, 3, \lin, 1].asSpec,
	decays: [0.01, 4, \lin, 0.01].asSpec,
);
var modParams     = Ndef(\comparMatrix).get(\modParams).clump(q.numOscs);
var freqState = Ndef(\comparMatrix).get(\freqs);
var ampState     = Ndef(\comparMatrix).get(\amps);
var decayState     = Ndef(\comparMatrix).get(\decays);


var colWidth = 40;
var knobHeight = 50;
var idxKnobColors   = [
	// upper right area
	[Color.gray(0.8), Color.blue, blend(Color.white, Color.blue, 0.5)],
	[Color.gray(0.8), Color.blue, blend(Color.white, Color.blue, 0.2)],
	// lower left area
	[Color.gray(0.8), Color.red, blend(Color.white, Color.red, 0.5)],
	[Color.gray(0.8), Color.red, blend(Color.white, Color.red, 0.2)]
];	
var bgColors = [
	Color.gray(0.8), Color.gray(1),
	Color.gray(0.6), Color.gray(0.8), 
];
q.win = Window.new("ComparFeedback", Rect(100, 100, (q.numOscs+5) * colWidth, 800)).decorate.front;

/////////// INDEXES

StaticText(q.win, Rect(10, 10, q.numOscs * (colWidth + 5) + 150, 20)).string_("-- modulation ");
q.win.view.decorator.nextLine;
q.higherAmpSliders = q.numOscs.collect{|i|
	var slider;
	//(i+1).do{|j|
	q.numOscs.do{|j|
		var ez;
		
		ez = EZKnob(q.win, Rect(25, 25, colWidth, knobHeight), 
			controlSpec: specs[\modParams],
			initAction: true,
			initVal: modParams[i][j].postln
		)
		.action_{|knob| 
			modParams[i][j] = knob.value;
			Ndef(\comparMatrix).setn(\modParams, modParams.flat);
		};
		ez.knobView.mode_(\vert);
		((j) > i).if({
			ez.setColors(knobColors: idxKnobColors[j%2]);
			ez.setColors(background: bgColors     [j%2]);
		}, {
			ez.setColors(knobColors: idxKnobColors[j%2 + 2]);
			ez.setColors(background: bgColors     [j%2 + 2]);
		});

		(i == j).if{
			ez.knobView.color_([Color.gray, Color.blue, Color.green]);
		};
	};
	slider = EZSlider(q.win, Rect(0, 0, 150, knobHeight * 0.5), 
		label: i, 
		layout: 'horz',
		numberWidth: 0,
		labelWidth: 10,
		initVal: ampState[i]
	)
	.action_{|slider|
		ampState[i] = slider.value;
		q.lowerAmpSliders[i].value = slider.value;
		Ndef(\comparMatrix).setn(\amps, ampState);
	};
	slider.setColors(background: bgColors[i%2 + 2]);

	q.win.view.decorator.nextLine;

	// return
	slider
};
q.win.view.decorator.nextLine;


/////////// FREQS
StaticText(q.win, Rect(10, 10, q.numOscs * (colWidth + 5), 20)).string_("-- freqs ----------");

q.win.view.decorator.nextLine;
q.numOscs.do{|i|
	var ez;
	ez = EZKnob(q.win, Rect(0, 0, colWidth, knobHeight), 
		controlSpec: specs[\freqs],
		initAction: true,
		initVal: freqState[i]

	)
	.action_{|knob|
		freqState[i] = knob.value;
		Ndef(\comparMatrix).setn(\freqs, freqState);
	};
	ez.knobView.mode_(\vert);
	ez.setColors(background: bgColors[i%2 + 2]);

};

/////////// DECAYS
q.win.view.decorator.nextLine;
StaticText(q.win, Rect(10, 10, q.numOscs * (colWidth + 5), 20)).string_("-- decays ----------");
q.win.view.decorator.nextLine;
q.decays = q.numOscs.collect{|i|
	var ez; 
	ez = EZKnob(q.win, Rect(25, 25, colWidth, knobHeight), 
		controlSpec: specs[\decays],
		initVal: decayState[i]
	)
	.action_{|knob|
		decayState[i] = knob.value;
		Ndef(\comparMatrix).setn(\decays, decayState);
	};
	ez.knobView.mode_(\vert);
	ez.setColors(background: bgColors[i%2 + 2]);
};


q.win.view.decorator.nextLine;


q.lowerAmpSliders = q.numOscs.collect{|i|
	var ez; 
	
	ez = EZSlider(q.win, Rect(0, 0, colWidth, 150), 
		label: i, 
		layout: 'vert',
		initVal: ampState[i]
	)
	.action_{|slider|
		ampState[i] = slider.value;
		Ndef(\comparMatrix).setn(\amps, ampState);
		q.higherAmpSliders[i].value = slider.value;
	};
	ez.setColors(background: bgColors[i%2 + 2]);
}
)


///////////// set random values ///////////
Ndef(\comparMatrix).setn(\decays, {1.0.rand}!10)
Ndef(\comparMatrix).setn(\freqs, {200.rand}!10)
Ndef(\comparMatrix).setn(\amps, {1.0.rand}!10)
Ndef(\comparMatrix).setn(\modParams, {10000.0.rand}!100)

