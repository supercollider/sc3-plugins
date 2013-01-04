/* -------------------------------------------------------------------------------
 * DFM-1 Digital Filter Module
 * Copyright (C) 2010, 2011
 *
 * Tony Hardie-Bick <tony@entitysynth.net> (Original Java version)
 * Jonny Stutters <jstutters@jeremah.co.uk> (SuperCollider port)
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

/**
 * DFM-1 coefficient look-up.
 */

/**
 * Highest frequency of self-oscillation (as fraction of audio sample rate).
 *
 * The filter can still ring above this frequency, however, this is the
 * maximum frequency at which oscillations can be sustained.
 */

static const double MAX_OSC_FREQ = 0.1640625;

/**
 * Number of octaves.
 *
 * The top octave is always fs/2 down to fs/4, so the number of octaves
 * determines the lowest frequency for which a coefficient is stored. For
 * example, if the maximum expected sample rate is 192kHz, then the lowest
 * filter cutoff frequency is 192000 / 2^(n+1), where n is the number of
 * octaves.
 */

static const int OCTAVES = 16;

/**
 * Number of steps per octave.
 */

static const int STEPS = 32;

/**
 * Lowest frequency in look-up table (as fraction of audio sample rate).
 */

static const double MIN_LUT_FREQ = 1.0 / 131072.0;

/**
 * Highest frequency in look-up table (as fraction of audio sample rate).
 */

static const double MAX_LUT_FREQ = 0.5 - 1.0 / (STEPS * 4.0) - 0.001;


class Dfm1Lut {

  private:
    
    static const double lut[];
  
  public:
  
    static double getMaxOscFreq(double sampleRate);
    static void getCoefficients(double frequency, double sampleRate, double coefficients[]);
    static bool test();

};
