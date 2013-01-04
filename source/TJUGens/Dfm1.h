/* -------------------------------------------------------------------------------
 * DFM-1 Digital Filter Module
 * Copyright (C) 2010, 2011
 *
 * Tony Hardie-Bick <tony@entitysynth.net> - Java version
 * Jonny Stutters <jstutters@jeremah.co.uk> - SuperCollider port
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * ------------------------------------------------------------------------------ */

#include <cmath>
#include "NoiseGen.h"
#include "Dfm1Lut.h"

/**
 * DFM-1 state.
 */

struct Dfm1State {
  
  double l; // Low pass filter input gain (derived from "type" and "frequency")
  double h; // High pass filter input gain (derived from "type")
  double a; // First stage filter coefficient (derived from "frequency")
  double b; // Second stage filter coefficient (derived from "frequency")
  double r; // Resonance (derived from "resonance" and "frequency")
  double s; // Noise level (derived from "noiseLevel")

  double za; // First stage integrator
  double zb; // Second stage integrator
  double zh; // High pass differentiator
  double zr; // Resonance differentiator
  double zy; // Filter output 

  NoiseGenState ng; // Noise generator state

};

/** 
 * DFM-1 Digital Filter Module.
 *
 * A 12dB-per-octave low pass resonant filter, switchable to 6dB-per-octave high pass. 
 */

static const double NOISE = 0.00001; // Minimum component noise level
static const double HEADROOM = 0.05; // Headroom for output signals (some noise is added after)
static const double POST_GAIN = (1.0 - 0.05) * 3.0 / 2.0; // Compensates attenuation of loud signals
static const double PRE_GAIN = 1.0 / (1.0 - 0.05) * 3.0 / 2.0; // Sets the overall gain to unity
static const double NOISE_GAIN = 0.05; // Filter noisiness
static const double N = 0.55032120814910446; // = (1/6)^(1/3) for the distortion transfer function


class Dfm1 {

  private:
    
    static double checkValue(double pi); // Checks for parameter denormalisation errors
  
  public:

    static void filter ( 

      float inputLevel,
      float frequency,
      float resonance,
      float filterType,
      float noiseLevel,
      float inBuffer [],
      float outBuffer [],
      int nSamples,
      float sampleRate,
      Dfm1State *df );

    static void initialize(Dfm1State* df);

};




