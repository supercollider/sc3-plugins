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

static InterfaceTable *ft;

template <typename FloatType>
struct ScalarSignal
{
	ScalarSignal(FloatType value):
		value(value)
	{}

	FloatType consume() const
	{
		return value;
	}

	FloatType value;
};

template <typename FloatType>
struct SlopeSignal
{
	SlopeSignal(FloatType value, FloatType slope):
		value(value), slope(slope)
	{}

	FloatType consume()
	{
		FloatType ret = value;
		value += slope;
		return ret;
	}

	FloatType value, slope;
};

template <typename FloatType>
struct AudioSignal
{
	AudioSignal(const FloatType * pointer):
		pointer(pointer)
	{}

	FloatType consume()
	{
		return *pointer++;
	}

	const FloatType * pointer;
};

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
	template <typename FloatType>
	inline SlopeSignal<FloatType> makeSlope(FloatType last, FloatType next)
	{
		return SlopeSignal<FloatType>(last, calcSlope(next, last));
	}

	template <typename FloatType>
	inline ScalarSignal<FloatType> makeScalar(FloatType value)
	{
		return ScalarSignal<FloatType>(value);
	}

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

			updateFrequency(freq.consume(), scale, phaseIncrement);

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

		mFreq = freq;

		const float sampleDuration = sampleDur();
		freq = freq * 2 * sampleDuration;

		scale = absScale / (4.f * freq * ( 1.f - freq * sampleDuration));
		phaseIncrement = freq;

		if (StoreUpdate) {
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

PluginLoad(NovaOsc)
{
	ft = inTable;
	DefineSimpleUnit(PulseDPW2);
}

