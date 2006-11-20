FreeVerb1 { *ar 
{  arg  in, revTime = 3, hfDamping = 0.995,
   numTaps = 10, numCombs = 8, numAllpasses = 4,
   tapScale = 1, combScale = 1, allpassScale = 1;


 var buffer,  // a buffer for the delay line
  tapData,  // Early reflection tap data (times, levels)
  times, levels,
  tapsOut,   // output from the delay taps
  combData,   // Table of comb data (times, levels)
  allpassData,  // delayTimes for allpasses (times left, times right)
  usedAllpassData,

  combs,  // Array of comb objects
  combsOut,  // their sound output
  allPassIO,  // allPass chain in/out variable
  inMono;  // input signal is converted to mono if necessary.

 var timeOneSample;  // used for comb average-filtering;

// timeOneSample = Synth.sampleRate.reciprocal;
timeOneSample = (SampleRate.ir).reciprocal;
// Initialize the tap table for early reflections

tapData =
 // delay     amplitude Moorer/Loy numbers as quoted in Pope, SC Tutorial...
 [ [0.0046, 0.841],
  [0.0218, 0.504],
  [0.0268, 0.379],
  [0.0298, 0.346],
  [0.0485, 0.272],
  [0.0572, 0.192],
  [0.0595, 0.217],
  [0.0708, 0.181],
  [0.0741, 0.142],
  [0.0797, 0.134]];

// Initialize comb table for longer reverberations

combData =
 // delay amplitude
 [ [0.0253061, 0.46],  // orig was 0.050, 0.056, etc;
  [0.0269388, 0.48],  // last digit added by AdC to avoid a
  [0.0289569, 0.50],  // large common multiple, 0.001, which
  [0.0307483, 0.52],  // a slightly ringing tone in the reverb.
  [0.0322449, 0.53],
  [0.0338095, 0.55],
  [0.0353061, 0.56],
  [0.0366667, 0.57]
  ];

// Initialize allpass delay times:
        // primeNumber based delayTimes generated with:
         // [ 146, 174, 220, 244].collect({ arg each; each.nthPrime/32380 })
        // [ 160, 183, 208, 229]...
allpassData =
 [ 0.0126077,
   0.01,
   0.00773243,
   0.00510204 ];


// mix input down to mono if necessary:

 inMono = if(in.size > 0, { Mix.new(in) }, { in });

 inMono = OnePole.ar(inMono, 0.6);  // round off reverb signal.

//// Create a multi-tap delay line:
 if (numTaps > 0,
  {  #times, levels = tapData.copyRange(0, (numTaps - 1).min(tapData.size)).flop;
   tapsOut = DelayL.ar(in, times, times, levels);
  },
  {  tapsOut = 0 }
 );

// Create an array of 6 combs:
 combsOut = in;    // initialize combs output first,
 numCombs.min(combData.size).do({arg index;
	  var time, level;
	  #time, level = combData.at(index);
	     // Get comb signal, scale by each comb's level and sum them up.
	 combsOut = CombL.ar(
	   	in + tapsOut.sum,
	   	0.1,
	   LFNoise2.ar( 4100, 0.0002, 1) * 
	 	  (time * combScale).round(timeOneSample) + (hfDamping * timeOneSample/2),
	   revTime,
	   level,
	   combsOut)
 });

// Put the output through two parallel chains of allpass delays:

 allPassIO = combsOut;       // initialize daisy-chain variable

 if (numAllpasses > 0,
  {
   usedAllpassData = allpassData.copyRange(0, (numAllpasses - 1).min(allpassData.size));
    usedAllpassData.do({ arg each;
     allPassIO = AllpassN.ar(
	     allPassIO,   // use same variable name for in- and output,
	     each,         // maxDelayTime
	     each,         // one allpass comb per channel,
	    1)
    });         // 1 second decay time is fixed;
            // simply because it sounds better to me.
  });

 ^allPassIO
 }
}
