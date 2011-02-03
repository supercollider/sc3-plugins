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

#include <inttypes.h>

/**
 * DFM-1 Noisegen state.
 */

struct NoiseGenState {

  uint32_t x;
  uint32_t y;
  uint32_t z;
  uint32_t w;

};

/**
 * Gaussian noise generator using the Ziggurat method.
 */

class NoiseGen {

  private:
  
    static const int32_t kn[];
    static const double wn[];
    static const double fn[];
    static int counter;
    static int32_t xor128(NoiseGenState* ng);
    static double uni(NoiseGenState* ng);
    static double nfix(int32_t hz, int32_t iz, NoiseGenState* ng);
  
  public:

    static double rnor(NoiseGenState* ng);
    static void initialize(NoiseGenState* ng);
    static void initialize(int seed, NoiseGenState* ng);
    static double correlationTest(int numberOfEvents);
    static bool quickTest();
    static bool longTest();

};
