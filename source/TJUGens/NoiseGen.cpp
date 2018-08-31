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
#include <ctime>
#include <iostream>
#include "NoiseGen.h"

/* Look-up tables */

const int32_t NoiseGen::kn[] = {

  1991057938, 0,          1611602771, 1826899878, 1918584482, 1969227037, 2001281515, 2023368125, 
  2039498179, 2051788381, 2061460127, 2069267110, 2075699398, 2081089314, 2085670119, 2089610331, 
  2093034710, 2096037586, 2098691595, 2101053571, 2103168620, 2105072996, 2106796166, 2108362327, 
  2109791536, 2111100552, 2112303493, 2113412330, 2114437283, 2115387130, 2116269447, 2117090813, 
  2117856962, 2118572919, 2119243101, 2119871411, 2120461303, 2121015852, 2121537798, 2122029592, 
  2122493434, 2122931299, 2123344971, 2123736059, 2124106020, 2124456175, 2124787725, 2125101763, 
  2125399283, 2125681194, 2125948325, 2126201433, 2126441213, 2126668298, 2126883268, 2127086657, 
  2127278949, 2127460589, 2127631985, 2127793506, 2127945490, 2128088244, 2128222044, 2128347141, 
  2128463758, 2128572095, 2128672327, 2128764606, 2128849065, 2128925811, 2128994934, 2129056501, 
  2129110560, 2129157136, 2129196237, 2129227847, 2129251929, 2129268426, 2129277255, 2129278312, 
  2129271467, 2129256561, 2129233410, 2129201800, 2129161480, 2129112170, 2129053545, 2128985244, 
  2128906855, 2128817916, 2128717911, 2128606255, 2128482298, 2128345305, 2128194452, 2128028813, 
  2127847342, 2127648860, 2127432031, 2127195339, 2126937058, 2126655214, 2126347546, 2126011445, 
  2125643893, 2125241376, 2124799783, 2124314271, 2123779094, 2123187386, 2122530867, 2121799464, 
  2120980787, 2120059418, 2119015917, 2117825402, 2116455471, 2114863093, 2112989789, 2110753906, 
  2108037662, 2104664315, 2100355223, 2094642347, 2086670106, 2074676188, 2054300022, 2010539237

};

const double NoiseGen::wn[] = {
  
  1.72904052154279800e-09, 1.26809284470029200e-10, 1.68975177731846410e-10, 1.98626884424791160e-10, 
  2.22324317925000040e-10, 2.42449361254489800e-10, 2.60161319006320950e-10, 2.76119887117039770e-10, 
  2.90739628177159950e-10, 3.04299704143766170e-10, 3.16997952139542940e-10, 3.28980205271130800e-10, 
  3.40357381218340840e-10, 3.51216022136647230e-10, 3.61625099505651850e-10, 3.71640576349598000e-10, 
  3.81308564311059900e-10, 3.90667568099488270e-10, 3.99750118699769100e-10, 4.08583986159844030e-10, 
  4.17193096401606540e-10, 4.25598235345926260e-10, 4.33817597392551050e-10, 4.41867218125288600e-10, 
  4.49761319626658200e-10, 4.57512588945882870e-10, 4.65132404814001000e-10, 4.72631023848117600e-10, 
  4.80017734723256700e-10, 4.87300986779874800e-10, 4.94488498053897300e-10, 5.01587346611961600e-10, 
  5.08604048242456000e-10, 5.15544622919539000e-10, 5.22414651970631600e-10, 5.29219327500630500e-10, 
  5.35963495331289000e-10, 5.42651692482061900e-10, 5.49288180034602100e-10, 5.55876972076077300e-10, 
  5.62421861298358800e-10, 5.68926441734655000e-10, 5.75394129037560300e-10, 5.81828178639089800e-10, 
  5.88231702081217000e-10, 5.94607681762499600e-10, 6.00958984310830300e-10, 6.07288372762788500e-10, 
  6.13598517705413500e-10, 6.19892007515592100e-10, 6.26171357814942800e-10, 6.32439020243540100e-10, 
  6.38697390643573500e-10, 6.44948816733738200e-10, 6.51195605346469700e-10, 6.57440029292859800e-10, 
  6.63684333913987400e-10, 6.69930743372330100e-10, 6.76181466732744400e-10, 6.82438703879113700e-10, 
  6.88704651310073300e-10, 6.94981507855166700e-10, 7.01271480351315500e-10, 7.07576789318556000e-10, 
  7.13899674673584900e-10, 7.20242401519748600e-10, 7.26607266052704700e-10, 7.32996601622086400e-10, 
  7.39412784991122800e-10, 7.45858242838353900e-10, 7.52335458548348800e-10, 7.58846979341765200e-10, 
  7.65395423799226300e-10, 7.71983489838440000e-10, 7.78613963209838100e-10, 7.85289726582899700e-10, 
  7.92013769303409800e-10, 7.98789197911353600e-10, 8.05619247520217000e-10, 8.12507294171396800e-10, 
  8.19456868292574500e-10, 8.26471669406662500e-10, 8.33555582258784500e-10, 8.40712694553299100e-10, 
  8.47947316521837200e-10, 8.55264002577609400e-10, 8.62667575351936300e-10, 8.70163152457442400e-10, 
  8.77756176380328400e-10, 8.85452447973727800e-10, 8.93258164108036900e-10, 9.01179960135660500e-10, 
  9.09224957951138100e-10, 9.17400820578600500e-10, 9.25715814404012600e-10, 9.34178880398847200e-10, 
  9.42799715966631400e-10, 9.51588869399888300e-10, 9.60557849383125300e-10, 9.69719252545394400e-10, 
  9.79086912790890000e-10, 9.88676077068772400e-10, 9.98503613453542500e-10, 1.00858825899144730e-09, 
  1.01895091686213820e-09, 1.02961501520066680e-09, 1.04060694369998740e-09, 1.05195658927280390e-09, 
  1.06369799919308710e-09, 1.07587021016458190e-09, 1.08851829606072830e-09, 1.10169470781350440e-09, 
  1.11546100955971630e-09, 1.12989016134932160e-09, 1.14506957000672370e-09, 1.16110524260223480e-09, 
  1.17812756094561300e-09, 1.19629950538507560e-09, 1.21582869832955640e-09, 1.23698562908049660e-09, 
  1.26013233006085250e-09, 1.28576968442051530e-09, 1.31462018496771830e-09, 1.34778395622108550e-09, 
  1.38706353150670430e-09, 1.43574031918163800e-09, 1.50086590302229930e-09, 1.60309479380911230e-09

};

const double NoiseGen::fn[] = {

  1.00000000000000000e+00, 9.63599693127085300e-01, 9.36282681685058900e-01, 9.13043647971739600e-01, 
  8.92281650784025700e-01, 8.73243048910069100e-01, 8.55500607869450300e-01, 8.38783605295989400e-01, 
  8.22907211381408800e-01, 8.07738294682960300e-01, 7.93177011771304800e-01, 7.79146085929687500e-01, 
  7.65584173897704300e-01, 7.52441559174611200e-01, 7.39677243672647100e-01, 7.27256918344184600e-01, 
  7.15151507410498400e-01, 7.03336099016158000e-01, 6.91789143436675000e-01, 6.80491840997334100e-01, 
  6.69427667348890400e-01, 6.58582000050088000e-01, 6.47941821110222500e-01, 6.37495477335042300e-01, 
  6.27232485249927300e-01, 6.17143370818880900e-01, 6.07219536625120300e-01, 5.97453150944516700e-01, 
  5.87837054434706600e-01, 5.78364681119763100e-01, 5.69029991067950900e-01, 5.59827412704086900e-01, 
  5.50751793114604500e-01, 5.41798355025425500e-01, 5.32962659383836100e-01, 5.24240572672984100e-01, 
  5.15628238244001800e-01, 5.07122051075569000e-01, 4.98718635470979500e-01, 4.90414825283844100e-01, 
  4.82207646329485200e-01, 4.74094300693016950e-01, 4.66072152689456100e-01, 4.58138716267872060e-01, 
  4.50291643682039200e-01, 4.42528715275468440e-01, 4.34847830249990850e-01, 4.27246998304996000e-01, 
  4.19724332049574400e-01, 4.12278040102661100e-01, 4.04906420807223100e-01, 3.97607856493873400e-01, 
  3.90380808237314700e-01, 3.83223811055901300e-01, 3.76135469510562700e-01, 3.69114453664472300e-01, 
  3.62159495369317700e-01, 3.55269384847917200e-01, 3.48442967546326640e-01, 3.41679141231550400e-01, 
  3.34976853313589200e-01, 3.28335098372850240e-01, 3.21752915875984900e-01, 3.15229388065010900e-01, 
  3.08763638006181100e-01, 3.02354827786483540e-01, 2.96002156846933000e-01, 2.89704860442959840e-01, 
  2.83462208223233000e-01, 2.77273502919188100e-01, 2.71138079138384600e-01, 2.65055302255589200e-01, 
  2.59024567396204830e-01, 2.53045298507325770e-01, 2.47116947512321400e-01, 2.41238993545439820e-01, 
  2.35410942263479080e-01, 2.29632325232116130e-01, 2.23902699385008420e-01, 2.18221646554305430e-01, 
  2.12588773071730300e-01, 2.07003709439926520e-01, 2.01466110074313670e-01, 1.95975653116277740e-01, 
  1.90532040319137170e-01, 1.85134997008992200e-01, 1.79784272123295450e-01, 1.74479638330789500e-01, 
  1.69220892237365000e-01, 1.64007854683420380e-01, 1.58840371139479300e-01, 1.53718312208181660e-01, 
  1.48641574242342260e-01, 1.43610080090627760e-01, 1.38623779984594600e-01, 1.33682652583439370e-01, 
  1.28786706195943200e-01, 1.23935980202867820e-01, 1.19130546707650830e-01, 1.14370512448866010e-01, 
  1.09656021014840270e-01, 1.04987255409421320e-01, 1.00364441028655870e-01, 9.57878491217314400e-02, 
  9.12578008268302400e-02, 8.67746718947801900e-02, 8.23388982422356600e-02, 7.79509825139734000e-02, 
  7.36115018841134000e-02, 6.93211173935779100e-02, 6.50805852130680700e-02, 6.08907703480404060e-02, 
  5.67526634810498500e-02, 5.26674019030510100e-02, 4.86362958598678050e-02, 4.46608622004914250e-02, 
  4.07428680744441750e-02, 3.68843887866562000e-02, 3.30878861462257500e-02, 2.93563174400068500e-02, 
  2.56932919359342700e-02, 2.21033046159270980e-02, 1.85921027370112880e-02, 1.51672980105465680e-02, 
  1.18394786578848620e-02, 8.62448441285988500e-03, 5.54899522077134500e-03, 2.66962908388092300e-03

};

/**
 * Generates gaussian noise using the Ziggurat method.
 *
 * An implementation of the Ziggurat algorithm by George Marsaglia and Wai
 * Wan Tsang. The code used is a minimally translated version of their C
 * program in the paper: "The Ziggurat Method for Generating Random
 * Variables", Journal of Statistical Software, vol. 5 (2000), No. 8. 
 * The only significant difference is that a random number generator
 * with a longer period has been used, and look-up tables are precalculated.
 *
 * @see <a href="http://www.jstatsoft.org/v05/i08">The Ziggurat Method</a>.
 * @see <a href="http://en.wikipedia.org/wiki/Ziggurat_algorithm">Wikipedia</a>.
 * @see <a href="http://seehuhn.de/pages/ziggurat">An implementation for GSL</a>
 * @see <a href="http://www.jstatsoft.org/v12/i07/paper">A comment by Leong et al</a>
 * @see <a href="http://www.doornik.com/research/ziggurat.pdf">Improved Ziggurat Method</a>
 *
 * @param ng noise generator state
 * @return next number (standard deviation = 1.0).
 */

double NoiseGen::rnor(NoiseGenState* ng) {
  
  int32_t hz = xor128(ng);
  int32_t iz = hz & 127;

  return (std::abs((float)hz) < kn[iz]) ? hz * wn[iz] : nfix(hz, iz, ng);

}

/**
 * Generates a random integer.
 *
 * A random number generator with period 2^128-1.
 *
 * @param ng noise generator state, must be non-zero
 * @return random integer in range -2^31 to 2^31-1
 *
 * @see <a href="http://www.jstatsoft.org/v08/i14">Xorshift RNGs</a>
 */

int32_t NoiseGen::xor128(NoiseGenState* ng) {

  uint32_t t = (ng->x ^ (ng->x << 15));
  
  ng->x = ng->y;
  ng->y = ng->z;
  ng->z = ng->w;
  
  return (ng->w = (ng->w ^ (ng->w >> 21)) ^ (t ^ (t >> 4)));

}

/** 
 * Generates a unipolar random floating point number.
 *
 * @param ng noise generator state
 * @return random value between 0 and 1
 */

double NoiseGen::uni(NoiseGenState* ng) {

  const double k = 1.0 / 4294967296.0; // 1 / 2^32
  return 0.5 + xor128(ng) * k;

}

/**
 * Fixes normal distribution.
 *
 * If the first random number lies outside a rectangular central
 * region of the ziggurat, this method is called to populate the
 * curved region at the edge.
 *
 * @param hz random number -2^31..2^31
 * @param iz random number 0..127
 * @param ng noise generator state
 * @return Random number 
 */

double NoiseGen::nfix(int32_t hz, int32_t iz, NoiseGenState* ng) {

  const double r = 3.442619855899;

  double x, y;

  for (;;) {

    x = hz * wn[iz];

    if (iz == 0) {

      do {

        x = -log(uni(ng)) * 0.2904764516147;
        y = -log(uni(ng));

      } while (y + y < x * x);

      return (hz > 0) ? r + x : -r - x;

    }

    if (fn[iz] + uni(ng) * (fn[iz - 1] - fn[iz]) < exp(-0.5 * x * x)) {
      return x;
    }
    
    hz = xor128(ng);
    iz = hz & 127;

    if (std::abs((float)hz) < kn[iz]) {
      return hz * wn[iz];
    }

  }

}

/** 
 * Counter for randomizing state initialization.
 */

int NoiseGen::counter = 0;

/**
 * Initialize noise generator state.
 *
 * This initializes a noise generator state to a different set of 
 * values each time it is called.
 *
 * @param ng noise generator state
 */

void NoiseGen::initialize(NoiseGenState* ng) {
  
  /* Create state values which will be different for each initialization */

  ng->x = (uint32_t) NoiseGen::counter++;
  ng->y = (uint32_t) NoiseGen::counter++;
  ng->z = (uint32_t) NoiseGen::counter++;
  ng->w = (uint32_t) (NoiseGen::counter++ * (int)time(NULL));
  
  /* At least one of the state values must be non-zero */
  if ((ng->x | ng->y | ng->z) == 0) ng->x = 1;

}

/**
 * Initialize noise generator state.
 *
 * This initializes a noise generator state using a seed value so it
 * can be used to generate a repeatable pseudorandom sequence.
 *
 * @param seed seed value for noise generator
 * @param ng noise generator state
 */

void NoiseGen::initialize(int seed, NoiseGenState *ng) {

  ng->x = (uint32_t) seed++;
  ng->y = (uint32_t) seed++;
  ng->z = (uint32_t) seed++;
  ng->w = (uint32_t) seed++;
  
  /* At least one of the state values must be non-zero */
  if ((ng->x | ng->y | ng->z) == 0) ng->x = 1;

}
  
/**
 * Correlates noise distribution with the normal distribution.
 *
 * This method is for checking class functionality. It correlates noise
 * generator output with the normal distribution. The higher the number 
 * of events tested, the closer the correlation should be to 1.0.
 *
 * @param numberOfEvents Number of noise generator events for test
 * @return Correlation with normal distribution
 */

double NoiseGen::correlationTest(int numberOfEvents) {
  
  const int numberOfBuckets = 1024;
  
  /* The width of the bell curve at the inflexion points as a proportion
   * of the test range. This is set to a value such as 0.2, so that some
   * of the tail of the distribution is included in the test (the width 
   * is also the standard deviation). */
  const double width = 0.2;
  
  NoiseGenState* ng = new NoiseGenState;
  NoiseGen::initialize(0, ng);
  
  double buckets[numberOfBuckets];
  double normal[numberOfBuckets];
  
  /* Clear buckets */
  for (int b = 0; b < numberOfBuckets; b++) {
    buckets[b] = 0;
  }
  
  const int halfNumberOfBuckets = numberOfBuckets / 2;
  
  /* Create normal distribution with unity height and variance */ 
  for (int b = 0; b < numberOfBuckets; b++) {
    double x = (double)(b - halfNumberOfBuckets) / (double)(halfNumberOfBuckets);
    normal[b] = exp(-(x * x) / (2.0 * width * width));
  }
  
  /* Run test */
  
  for (int e = 0; e < numberOfEvents; e++) {

    int32_t r = (int32_t)round(NoiseGen::rnor(ng) * halfNumberOfBuckets * width);
    r += halfNumberOfBuckets;
    if (r >= 0 && r < numberOfBuckets) {
      buckets[r]++;
    }
  }
  
  /* Calculate correlation (three pass method) */

  /* 1. Calculate averages */
  
  double sumBuckets = 0;
  double sumNormal = 0;
  
  for (int b = 0; b < numberOfBuckets; b++) {
    sumBuckets += buckets[b];
    sumNormal += normal[b];
  }
  
  double averageBuckets = sumBuckets / (double)numberOfBuckets;
  double averageNormal = sumNormal / (double)numberOfBuckets;
  
  /* 2. Calculate standard deviations and move averages to zero */
  
  double sdBuckets = 0;
  double sdNormal = 0;
  
  for (int b = 0; b < numberOfBuckets; b++) {
    buckets[b] -= averageBuckets;
    sdBuckets += buckets[b] * buckets[b];
    normal[b] -= averageNormal;
    sdNormal += normal[b] * normal[b];
  }
  
  sdBuckets = sqrt(sdBuckets / numberOfBuckets);
  sdNormal = sqrt(sdNormal / numberOfBuckets);
  
  /* 3. Accumulate products and calculate correlation */

  double sumProducts = 0;
  double rSdP = 1.0 / (sdBuckets * sdNormal);
  
  for (int b = 0; b < numberOfBuckets; b++) {
    sumProducts += buckets[b] * normal[b] * rSdP;
  }
  
  std::cout << "correlationTest result: " << sumProducts / (double)numberOfBuckets << std::endl;
  return sumProducts / (double)numberOfBuckets; // Pearson correlation coefficient

}

/**
 * Tests class functionality.
 *
 * This is a quick test, which takes less than a second to run. If
 * this test passes, the class is probably working correctly.
 */

bool NoiseGen::quickTest() {

  const int numberOfEvents = 1000000;
  const double correlationThreshold = 0.999;
  
  return (correlationTest(numberOfEvents) > correlationThreshold);

}

/**
 * Tests class implementation in great detail.
 *
 * This is a long test, taking several seconds. If successful, the
 * class can reasonably be assumed to be working in every detail.
 */

bool NoiseGen::longTest() {
  
    /* A subtle error in the calculation of the distribution's tails
     * can be introduced by changing the first log function in nfix()
     * to include the 0.2904 factor. This test should fail when this
     * is done, and pass otherwise. */

  const int numberOfEvents = 360000000;
  const double correlationThreshold = 0.9999985;
  
  return (correlationTest(numberOfEvents) > correlationThreshold);

}

