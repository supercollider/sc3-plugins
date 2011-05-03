/*

OnsetsDS - a SuperCollider "pseudo-UGen" that encapsulates a good and convenient onset-detection setup.
(c) 2007 Dan Stowell.
Released under the GNU GPL. See LICENSE.txt for more details.

*/

OnsetsDS : UGen {

*kr { |in, fftbuf, trackbuf, thresh=0.5, type=\power,
   extchain=false, relaxtime=0.1, floor=0.1, smear=0, mingap=0.05, medianspan=11|

var chain, odf, onsets;

	"OnsetsDS is deprecated and will be removed. Use 'Onsets.kr' instead.".error;

^{
// Perform FFT, if needed
chain = if(extchain, in, {FFT(fftbuf, in)});

// Whiten the FFT signal. Note: for MKL we don't do this!
chain = if(type == \mkl, chain, {PV_Whiten(chain, trackbuf, relaxtime, floor, smear)});

// Convert to k-rate onset detection function
odf = type.switch(
  \complex,  { FFTComplexDev.kr(chain, 0) },
  \rcomplex, { FFTComplexDev.kr(chain, 1) },
  \phase,    { FFTPhaseDev.kr(chain, 0, 0.4) },
  \wphase,   { FFTPhaseDev.kr(chain, 1, 0.4) },
  \mkl,      { FFTMKL.kr(chain, 0.01) },
  // Power is default
             { FFTPower.kr(chain) }
);

// Median filter
odf = odf - MedianTriggered.kr(odf, chain>=0, medianspan);

// Now re-scale
odf = type.switch(
  \complex,  { odf * 0.02 },
  \rcomplex, { odf * 0.02 },
  \phase,    { odf * 0.01 },
  \wphase,   { odf * 0.01 },
  \mkl,      { odf * 0.015 },
  // Power is default
             { odf * 5 }
);

// Threshold and trigger.
onsets = if(odf > thresh, 1, 0);
onsets = Trig1.kr(onsets, mingap);
onsets = Trig1.kr(onsets, 0);

} // End of func to return

} // End .kr


} // End class
