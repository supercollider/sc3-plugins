######
#
# Readme for the SuperCollider port of the DFM-1 Digital Filter Module
#
######

## USAGE ##

To check DFM1 is operating correctly, execute the following in SuperCollider:
   s = Server.local.boot;
   a = {DFM1.ar(PinkNoise.ar(mul:0.4), freq:MouseX.kr(50, 10000))}.play;

You should hear a filtered pink noise signal and be able to modulate the filter
frequency with the mouse.

The DFM1 UGen supports the following arguments:
  freq:       the cutoff frequency of the filter
  res:        the filter resonance (values > 1.0 may lead to self-oscillation)
  inputgain:  gain applied to the input signal which can be used to generate
              distortion
  type:       set to 0.0 for low-pass behaviour or 1.0 for high-pass
  noiselevel: the noisiness of the filter (defaults to 0.0003)


## SUPPORT ##

For questions related to the audio code in DFM1 please contact
Tony Hardie-Bick <tony@entitysynth.net>

For questions regarding the function of DFM1 within SuperCollider
contact Jonny Stutters <jstutters@jeremah.co.uk>
