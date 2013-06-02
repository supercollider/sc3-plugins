/*
 *
 *    Copyright (C) 2013 Tim Blechmann
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "SC_PlugIn.hpp"

#include <cmath>
#include <limits>

#include "Interpolators.hpp"

static InterfaceTable *ft;

struct PulseDPW2:
	public SCUnit
{
public:
	PulseDPW2():
		mFreq(std::numeric_limits<float>::quiet_NaN()),
		mLastVal0(0),
		mLastVal1(0)
	{
		const float width = in0(1);
		const float phase = in0(2);

		mPhase0 = sc_wrap(phase, -1.0f, 1.0f);
		mPhase1 = sc_wrap(phase + width + width, -1.0f, 1.0f);

		switch (inRate(0)) {
		case calc_ScalarRate:
		{
			float freq = in0(0);
			float scale;
			double phaseIncrement;
			updateFrequency<true>(freq, scale, phaseIncrement);

			set_calc_function<PulseDPW2, &PulseDPW2::next_i>();
			break;
		}

		case calc_BufRate:
			set_calc_function<PulseDPW2, &PulseDPW2::next_k>();
			break;

		case calc_FullRate:
			set_calc_function<PulseDPW2, &PulseDPW2::next_a>();
		}
	}

private:
	void next_i(int inNumSamples)
	{
		float scale = mScale;
		double phaseIncrement = mPhaseIncrement;

		next(inNumSamples, makeScalar(scale), makeScalar(phaseIncrement));
	}

	void next_k(int inNumSamples)
	{
		float freq = in0(0);
		if (freq == mFreq) {
			next_i(inNumSamples);
		} else {
			float lastScale = mScale;
			double lastPhaseIncrement = mPhaseIncrement;

			float nextScale;
			double nextPhaseIncrement;
			updateFrequency<true>(freq, nextScale, nextPhaseIncrement);
			next(inNumSamples, makeSlope(nextScale, lastScale), makeSlope(nextPhaseIncrement, lastPhaseIncrement));
		}
	}

	void next_a(int inNumSamples)
	{
		float lastVal0 = mLastVal0;
		float lastVal1 = mLastVal1;
		double phase0 = mPhase0;
		double phase1 = mPhase1;

		float * outSig = zout(0);

		AudioSignal<float> freq(in(0));

		loop(inNumSamples, [&] {
			double phaseIncrement;
			float scale;

			updateFrequency<false>(freq.consume(), scale, phaseIncrement);

			ZXP(outSig) = tick(phase0, phase1, phaseIncrement, lastVal0, lastVal1, scale);
		});

		mLastVal0 = lastVal0;
		mLastVal1 = lastVal1;
		mPhase0 = phase0;
		mPhase1 = phase1;
	}

	template <typename ScaleType, typename PhaseType>
	inline void next(int inNumSamples, ScaleType scale, PhaseType phaseIncrement)
	{
		float lastVal0 = mLastVal0;
		float lastVal1 = mLastVal1;
		double phase0 = mPhase0;
		double phase1 = mPhase1;

		float * outSig = zout(0);
		LOOP(inNumSamples >> 1,
			float out0 = tick(phase0, phase1, phaseIncrement.consume(), lastVal0, lastVal1, scale.consume());
			float out1 = tick(phase0, phase1, phaseIncrement.consume(), lastVal0, lastVal1, scale.consume());

			ZXP(outSig) = out0;
			ZXP(outSig) = out1;
		);

		LOOP(inNumSamples & 1,
			ZXP(outSig) = tick(phase0, phase1, phaseIncrement.consume(), lastVal0, lastVal1, scale.consume());
		);

		mLastVal0 = lastVal0;
		mLastVal1 = lastVal1;
		mPhase0 = phase0;
		mPhase1 = phase1;
	}

	template <bool StoreUpdate = true>
	inline void updateFrequency(float freq, float & scale, double & phaseIncrement)
	{
		// we simulate negative frequencies by scaling positive frequencies by -1
		const float absScale = freq >= 0 ? 1 : -1;
		freq = std::abs(freq);

		const float sampleDuration = sampleDur();
		freq = freq * 2 * sampleDuration;

		scale = absScale / (4.f * freq * ( 1.f - freq * sampleDuration));
		phaseIncrement = freq;

		if (StoreUpdate) {
			mFreq = freq;
			mScale = scale;
			mPhaseIncrement = phaseIncrement;
		}
	}

	static inline double incrementPhase(double phase, double phaseIncrement)
	{
		phase += phaseIncrement;
		if (phase >= 1.) return phase - 2.;
		return phase;
	}

	static inline float tick(double & phase0, double & phase1, double phaseIncrement,
							 float & lastVal0, float & lastVal1, float scale)
	{
		phase0 = incrementPhase(phase0, phaseIncrement);
		phase1 = incrementPhase(phase1, phaseIncrement);

		// squared saw
		float val0 = (float)phase0 * (float)phase0;
		float val1 = (float)phase1 * (float)phase1;

		// differentiate parabolic wave
		float differentiatedPhase0 = val0 - lastVal0;
		float differentiatedPhase1 = val1 - lastVal1;

		// substracting saws to get a pulse
		float pulse = (differentiatedPhase0 - differentiatedPhase1) * scale;

		lastVal0 = val0;
		lastVal1 = val1;

		return pulse;
	}

	double mPhase0, mPhase1, mPhaseIncrement;
	float mLastVal0, mLastVal1;

	float mFreq, mScale;
};

DEFINE_XTORS(PulseDPW2)


enum interpolator {
	SampleHold,
	Linear,
	Cubic,
	Interp_4_5_2nd,
	Bspline_4p_3o,
	Bspline_6p_5o,
	Opt_2x_4p_4o,
	Opt_2x_6p_5o
};


struct TableLookupOscillatorBase:
	public SCUnit
{
	typedef float phaseType;

	template <int Interpolator>
	static const int numberOfPoints()
	{
		switch (Interpolator)
		{
		case SampleHold:
			return 1;

		case Linear:
			return 2;

		case Cubic:
		case Interp_4_5_2nd:
		case Bspline_4p_3o:
		case Opt_2x_4p_4o:
			return 4;

		case Bspline_6p_5o:
		case Opt_2x_6p_5o:
			return 6;

		default:
			assert(false);
		}
	}

	template <int Interpolator>
	static inline float calcSample(const float * table, int32 iphase, phaseType fracPhase, int32 mask)
	{
		const int numberOfInterpolationPoints = numberOfPoints<Interpolator>();

		switch (numberOfInterpolationPoints) {
		case 1:
			assert(Interpolator == SampleHold);
			return table[iphase];

		case 2:
		{
			int32 index0 = iphase & mask;
			int32 index1 = (iphase+1) & mask;
			float s0 = table[index0];
			float s1 = table[index1];

			assert(Interpolator == Linear);
			float x = lininterp(fracPhase, s0, s1);
			return x;
		}

		case 4:
		{
			int32 index0 = (iphase-1) & mask;
			int32 index1 = iphase & mask;
			int32 index2 = (iphase+1) & mask;
			int32 index3 = (iphase+2) & mask;
			phaseType s0 = table[index0];
			phaseType s1 = table[index1];
			phaseType s2 = table[index2];
			phaseType s3 = table[index3];

			switch (Interpolator) {
			case Cubic:
				return cubicinterp(fracPhase, s0, s1, s2, s3);

			case Interp_4_5_2nd:
				return interp_4_5_2nd(fracPhase, s0, s1, s2, s3);

			case Bspline_4p_3o:
				return bspline_4p_3o(fracPhase, s0, s1, s2, s3);

			case Opt_2x_4p_4o:
				return opt_2x_4p_4o(fracPhase, s0, s1, s2, s3);
			}
		}

		case 6:
		{
			int32 index0 = (iphase-2) & mask;
			int32 index1 = (iphase-1) & mask;
			int32 index2 = iphase & mask;
			int32 index3 = (iphase+1) & mask;
			int32 index4 = (iphase+2) & mask;
			int32 index5 = (iphase+3) & mask;
			phaseType s0 = table[index0];
			phaseType s1 = table[index1];
			phaseType s2 = table[index2];
			phaseType s3 = table[index3];
			phaseType s4 = table[index4];
			phaseType s5 = table[index5];

			switch (Interpolator) {
			case Bspline_6p_5o:
				return bspline_6p_5o(fracPhase, s0, s1, s2, s3, s4, s5);

			case Opt_2x_6p_5o:
				return opt_2x_6p_5o(fracPhase, s0, s1, s2, s3, s4, s5);
			}
		}
		}
		assert(false);
		return 0.f;
	}
};

using std::numeric_limits;

namespace detail {

template <typename T>
struct identity_functor
{
	identity_functor(T const & v): val(v) {}
	inline T operator()() const { return val; }

	const T val;
};

}

template <typename T>
inline detail::identity_functor<T> identity(T const & val) { return detail::identity_functor<T>(val); };


struct NovaOscBase:
	TableLookupOscillatorBase
{
	phaseType mPhase;
	float mPhasein;
	float mFreqIn;

	NovaOscBase():
		mPhase(0), mPhasein(numeric_limits<float>::quiet_NaN()), mFreqIn(numeric_limits<float>::quiet_NaN())
	{}

	template <int Interpolator, int FreqInputIndex, int PhaseInputIndex>
	force_inline void nextN( int inNumSamples, const float * wavetable, int32 bitmask, int32 wavetableSize )
	{
		int PhaseRate = inRate(PhaseInputIndex); // TODO: compiletime dispatching

		switch (PhaseRate) {
		case calc_ScalarRate: {
			nextN<Interpolator, FreqInputIndex>(inNumSamples, wavetable, bitmask, wavetableSize, identity(mPhasein * rtwopi * wavetableSize));
			return;
		}

		case calc_BufRate: {
			const float phaseOffset = in0(PhaseInputIndex);
			if (phaseOffset == mPhasein) {
				nextN<Interpolator, FreqInputIndex>(inNumSamples, wavetable, bitmask, wavetableSize, identity(phaseOffset * rtwopi * wavetableSize));
				return;
			} else {
				float currentPhase = mPhasein;
				const float phaseSlope = calcSlope(phaseOffset, mPhasein);

				auto phaseOffsetFN = [&]() {
					float ret = currentPhase;
					currentPhase += phaseSlope;
					return ret * rtwopi * wavetableSize;
				};

				nextN<Interpolator, FreqInputIndex>(inNumSamples, wavetable, bitmask, wavetableSize, phaseOffsetFN);
				mPhasein = currentPhase;
				return;
			}
		}

		case calc_FullRate: {
			const float * phaseIn = zin(PhaseInputIndex);
			auto phaseOffsetFN = [&]() {
				return ZXP(phaseIn) * rtwopi * wavetableSize;
			};

			nextN<Interpolator, FreqInputIndex>(inNumSamples, wavetable, bitmask, wavetableSize, phaseOffsetFN);
			return;
		}
		}
	}

	template <int Interpolator, int FreqInputIndex, typename PhaseInput>
	force_inline void nextN( int inNumSamples, const float * wavetable, int32 bitmask, int32 wavetableSize, PhaseInput const & phaseOffset )
	{
		const int FreqRate = inRate(FreqInputIndex);  // TODO: compiletime dispatching

		switch (FreqRate) {
		case calc_ScalarRate: {
			const float freqIn = mFreqIn;
			const float phaseIncrement = freqIn * sampleDur() * (double)wavetableSize;

			nextN<Interpolator>(inNumSamples, wavetable, bitmask, wavetableSize, identity(phaseIncrement), phaseOffset);
			return;
		}

		case calc_BufRate: {
			const float freqIn = in0(FreqInputIndex);
			const float phaseIncrement = freqIn * sampleDur() * (double)wavetableSize;
			if (freqIn == mFreqIn) {
				nextN<Interpolator>(inNumSamples, wavetable, bitmask, wavetableSize, identity(phaseIncrement), phaseOffset);
				return;
			} else {
				float currentPhaseIncrement = mFreqIn * sampleDur() * (double)wavetableSize;
				const float phaseIncrementSlope = calcSlope(currentPhaseIncrement, phaseIncrement);

				auto phaseIncrementFN = [&]() {
					float ret = currentPhaseIncrement;
					currentPhaseIncrement += phaseIncrementSlope;
					return ret;
				};

				nextN<Interpolator>(inNumSamples, wavetable, bitmask, wavetableSize, phaseIncrementFN, phaseOffset);

				mFreqIn = freqIn;
				return;
			}

			return;
		}

		case calc_FullRate: {
			const float * freqIn = zin(FreqInputIndex);
			const float factor = sampleDur() * (double)wavetableSize;

			auto phaseIncrementFN = [&freqIn, factor]() {
				float currentFreq = ZXP(freqIn);
				return currentFreq * factor;
			};

			nextN<Interpolator>(inNumSamples, wavetable, bitmask, wavetableSize, phaseIncrementFN, phaseOffset);
			return;
		}
		}
	}

	template <int Interpolator, typename PhaseIncrement, typename PhaseInput>
	force_inline void nextN( int inNumSamples, const float * wavetable, int32 bitmask, int32 wavetableSize,
					   PhaseIncrement const & phaseIncrement, PhaseInput const & phaseOffset )
	{
		float * out = zout(0);
		phaseType phase = mPhase;

		for (int i = 0; i != inNumSamples; ++i) {
			if (phase > wavetableSize)
				phase -= wavetableSize;

			phaseType phase1 = phase + phaseOffset();
			int32 iphase = (int32)phase1;
			phaseType fracphase = phase1 - (phaseType)iphase;

			float phaseInc = phaseIncrement();

			ZXP(out) = TableLookupOscillatorBase::calcSample<Interpolator>(wavetable, iphase, fracphase, bitmask);

			phase += phaseInc;
		}

		mPhase = phase;
	}
};

struct NovaSinOsc:
	NovaOscBase
{
	const float * mTable;
	int32 mSize;
	int32 mMask;

	NovaSinOsc():
		mTable(ft->mSine), mSize(ft->mSineSize), mMask(mSize - 1)
	{
		mFreqIn  = in0(0);
		mPhasein = in0(1);
		int typeOfInterpolation = in0(2);
		typeOfInterpolation = sc_clip(typeOfInterpolation, 0, Opt_2x_6p_5o);

		switch (typeOfInterpolation) {
		case 0: set_calc_function<NovaSinOsc, &NovaSinOsc::nextN<SampleHold> >(); break;
		case 1: set_calc_function<NovaSinOsc, &NovaSinOsc::nextN<Linear> >(); break;
		case 2: set_calc_function<NovaSinOsc, &NovaSinOsc::nextN<Cubic> >(); break;
		case 3: set_calc_function<NovaSinOsc, &NovaSinOsc::nextN<Interp_4_5_2nd> >(); break;
		case 4: set_calc_function<NovaSinOsc, &NovaSinOsc::nextN<Bspline_4p_3o> >(); break;
		case 5: set_calc_function<NovaSinOsc, &NovaSinOsc::nextN<Bspline_6p_5o> >(); break;
		case 6: set_calc_function<NovaSinOsc, &NovaSinOsc::nextN<Opt_2x_4p_4o> >(); break;
		case 7: set_calc_function<NovaSinOsc, &NovaSinOsc::nextN<Opt_2x_6p_5o> >(); break;
		}
	}

	template <int Interpolator>
	void nextN( int inNumSamples )
	{
		NovaOscBase::nextN<Interpolator, 0, 1>(inNumSamples, mTable, mMask, mSize);
	}
};

DEFINE_XTORS(NovaSinOsc)

struct NovaOsc:
	NovaOscBase
{
	SndBuf *m_buf;
	float m_fbufnum;

	NovaOsc()
	{
		m_fbufnum = numeric_limits<float>::quiet_NaN();
		NovaOsc * unit = this;
		SIMPLE_GET_BUF;

		mFreqIn  = in0(1);
		mPhasein = in0(2);
		int typeOfInterpolation = in0(3);
		typeOfInterpolation = sc_clip(typeOfInterpolation, 0, Opt_2x_6p_5o);

		switch (typeOfInterpolation) {
		case 0: set_calc_function<NovaOsc, &NovaOsc::nextN<SampleHold> >(); break;
		case 1: set_calc_function<NovaOsc, &NovaOsc::nextN<Linear> >(); break;
		case 2: set_calc_function<NovaOsc, &NovaOsc::nextN<Cubic> >(); break;
		case 3: set_calc_function<NovaOsc, &NovaOsc::nextN<Interp_4_5_2nd> >(); break;
		case 4: set_calc_function<NovaOsc, &NovaOsc::nextN<Bspline_4p_3o> >(); break;
		case 5: set_calc_function<NovaOsc, &NovaOsc::nextN<Bspline_6p_5o> >(); break;
		case 6: set_calc_function<NovaOsc, &NovaOsc::nextN<Opt_2x_4p_4o> >(); break;
		case 7: set_calc_function<NovaOsc, &NovaOsc::nextN<Opt_2x_6p_5o> >(); break;
		}
	}

	template <int Interpolator>
	void nextN( int inNumSamples )
	{
		const SndBuf * buf = m_buf;
		const float *table0 = buf->data;
		const int32 mask = buf->mask;

		NovaOscBase::nextN<Interpolator, 1, 2>(inNumSamples, table0, mask, buf->samples);
	}
};


DEFINE_XTORS(NovaOsc)

PluginLoad(NovaOsc)
{
	ft = inTable;
	DefineSimpleUnit(PulseDPW2);
	DefineSimpleUnit(NovaSinOsc);
	DefineSimpleUnit(NovaOsc);
}

