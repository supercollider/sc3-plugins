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
#include "Dfm1.h"

/**
 * Filters audio samples.
 *
 * The arguments define the target values which will have been reached by the
 * filter parameters when the method returns. If a large number of samples
 * (more than a thousand) needs to be processed, this method should be called
 * multiple times with a smaller number of samples. The audio data has a
 * typical range from minus to plus one, but can be greater. The input samples
 * are multiplied by the input gain so that high distortion levels can be
 * readily achieved with ordinary signals. Output samples are always in the
 * range minus to plus one. Filter self-oscillation also covers this range.
 *
 * @param inputLevel Input level (0..n, typically 0.5..5.0)
 * @param frequency Resonant frequency in hertz.
 * @param resonance Resonance level (1 = self osc., and can be higher).
 * @param filterType Filter type (below 0.5 gives low pass, otherwise high).
 * @param noiseLevel Level of noise in the filter (0..n, typically 0.0003)
 * @param inBuffer Input audio samples
 * @param outBuffer Output audio samples, will contain filtered audio on exit
 * @param nSamples Number of audio samples to process.
 * @param sampleRate Sample rate in hertz.
 * @param df Dfm1 state variables
 */

void Dfm1::filter ( 
    
    float inputLevel,
    float frequency,
    float resonance,
    float filterType,
    float noiseLevel,
    float inBuffer [],
    float outBuffer [],
    int nSamples,
    float sampleRate,
    Dfm1State *df ) {

  if (nSamples < 1) return;

  if (frequency < 1.0f) frequency = 1.0f;
  if (resonance < 0.0f) resonance = 0.0f;
  if (resonance > 10.0f) resonance = 10.0f;

  float *input = inBuffer;
  float *output = outBuffer;
  
  /* Copy state into local variables for speed */

  double l = df->l; // Low pass filter input gain (derived from "type" and "frequency")
  double h = df->h; // High pass filter input gain (derived from "type")
  double a = df->a; // First stage filter coefficient (derived from "frequency")
  double b = df->b; // Second stage filter coefficient (derived from "frequency")
  double r = df->r; // Resonance (derived from "resonance" and "frequency")
  double s = df->s; // Noise level (derived from "noiseLevel")
  
  double za = df->za; // First stage integrator
  double zb = df->zb; // Second stage integrator
  double zh = df->zh; // High pass differentiator
  double zr = df->zr; // Resonance differentiator
  double zy = df->zy; // Filter output 
  
  NoiseGenState* ng = &(df->ng); // Noise generator state (pointer)
  
  /* Obtain target values ("rt" means "target value for parameter r") */

  double coefficients[3];
  
  Dfm1Lut::getCoefficients(frequency, sampleRate, coefficients);
  
  double at = coefficients[0]; // Feedback for first integrator
  double bt = coefficients[1]; // Feedback for second integrator
  double rt = coefficients[2] * resonance; // Apply resonance correction factor

  /* HPF/LPF selection */

  double lt = inputLevel * PRE_GAIN; // Low pass input gain
  double ht = inputLevel * PRE_GAIN; // High pass input gain
  
  if (filterType < 0.5) ht = 0.0; else lt = 0.0;
  
  lt *= (1.0 - at); // Low pass gain is conflated with first stage integrator gain
  
  /* Noise */

  double st = noiseLevel * NOISE_GAIN;
  if (noiseLevel < NOISE) st = NOISE; // Set minimum noise level

  /* Calculate parameter interpolation increments */
  
  double i = 1.0 / nSamples;
  
  double li = checkValue((lt - l) * i);
  double hi = checkValue((ht - h) * i);
  double ai = checkValue((at - a) * i);
  double bi = checkValue((bt - b) * i);
  double ri = checkValue((rt - r) * i);
  double si = checkValue((st - s) * i);
  
  /* Sample loop */
  
  for (int t = 0; t < nSamples; t++) {

    l += li; h += hi; a += ai; b += bi; r += ri; s += si; // Increment parameters
    
    double x = input[t];
    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
     * DFM-1 Filter 
     *
     *                    . - a - .                . - b - .
     *                    |       |                |       |
     *                    |       z' (za)          |       z' (zb)
     *                    |       |                |       |
     *      x[t]          v       |                v       |  q[t]               y[t]
     * lpf --->-- l -->-- + --------->-- 1-b -->-- + ---------->--- f(q[t]) -- + ---->
     *                  / ^                      / ^                         / |
     *                s   |                    s   |                       s   |
     *              /     r                  /    + -                    /     |
     *        noise       |            noise     /   \             noise       |
     *                   - +                    |     z' (zh)                  |
     *                  /   \                   |     |                        |
     *                 |     |    hpf -- h -->--------'                        |
     *           (zr)  z'    |                                                 |
     *                 |     |   r[t]              (zy)                        |
     *                 `-----------<--------------- z' ------------<-----------'
     *
     *
     * The filter's frequency is defined by a and b. These take different values,
     * simulating different capacitor values in each of the two stages. Resonance is
     * defined by r. The lpf input gain is provided by l (conflated with 1-a to prescale
     * the first stage). The hpf input gain is provided by h. Distortion is provided by
     * f(q[t]), in which the sample magnitude is limited to unity, and then the transfer
     * function f(x) = x - 2(nx)^3 is applied. The value of n is set so the maximum
     * input values coincide with the function's maxima, and the filter's output is
     * multiplied by a scaling factor to compensate for the resulting reduction in
     * amplitude of loud signals.
     *
     * The noise has two purposes: Disruption and dithering. In an analogue circuit,
     * a large number of noise sources is accumulated, resulting in rare high amplitude
     * events that can push the filter further into or out of distortion, knocking the
     * filter's resonant phase-locked loop out of or into phase with a signal harmonic.
     * Noise is applied so the magnitude of its low frequency components increases 
     * inversely with respect to filter cutoff frequency.
     *
     * Dithering ensures that amplitude quantization artefacts do not accumulate to
     * audible levels as a result of integration. In a floating point implementation, 
     * dither also ensures that denormalisation events are rare.
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
    za = x * l + r * (zy - zr) + a * za + s * NoiseGen::rnor(ng);
    zb = za * (1.0 - b) + (x - zh) * h + b * zb + s * NoiseGen::rnor(ng);
    zh = x;
    zr = zy;
    
    zb = zb < -1.0 ? -1.0 : zb > 1.0 ? 1.0 : zb;
    double nx = N * zb;
    zy = zb - 2.0 * nx * nx * nx;
    
    zy += s * NoiseGen::rnor(ng);
    
    output[t]  = (float)(zy * POST_GAIN);

  }

  /* After loop, store filter state */
  
  df->l = lt; df->h = ht; df->a = at; df->b = bt; df->r = rt; df->s = st;
  df->za = za; df->zb = zb; df->zh = zh; df->zr = zr; df->zy = zy;

}

/**
 * Checks for parameter denormalisation.
 *
 * This method anticipates any denormalisation errors which may occur if a
 * parameter increment is extremely small, returning zero if this is the
 * case.    
 *
 * @param pi Parameter increment
 * @return Corrected parameter increment
 */

double Dfm1::checkValue(double pi) {

  /* This check is very simple, and assumes that less than a thousand 
   * samples are processed each call, and that the audio sample rate is 
   * no more than a few hundred kilohertz. */

  const double DENORM_THRESHOLD = 0.000001;
  return std::abs(pi) < DENORM_THRESHOLD ? 0.0 : pi;

}

/**
 * Initializes the filter.
 *
 * Sets the filter state variables and current parameter values to zero.
 */

void Dfm1::initialize(Dfm1State* df) {

  df->l = df->h = df->a = df->b = df->r = df->s = 0;
  df->za = df->zb = df->zr = df->zy = 0;
  
  NoiseGen::initialize(&(df->ng));

}
