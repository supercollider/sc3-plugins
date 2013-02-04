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

#include "NovaUGensCommon.hpp"

static InterfaceTable *ft;

struct FBAM:
	public NovaUnit
{
public:
	FBAM():
		zm1_(0)
	{
		float feedback = in0(1);
		fb_ = sc_clip(feedback, 0.0, 1.9); // really depends on the frequency

		switch (inRate(1)) {
		case calc_ScalarRate:
			set_calc_function<FBAM, &FBAM::next_i>();
			break;

		case calc_BufRate:
			set_calc_function<FBAM, &FBAM::next_k>();
			break;

		case calc_FullRate:
			set_calc_function<FBAM, &FBAM::next_a>();
			break;
		}
	}

private:
	void next_i(int inNumSamples)
	{
		auto fb = makeScalar(fb_);
		next(inNumSamples, fb);
	}

	void next_k(int inNumSamples)
	{
		float newFeedback = in0(1);
		newFeedback = sc_clip(newFeedback, 0.0, 1.9); // really depends on the frequency

		if (newFeedback != fb_) {
			auto fb = makeSlope(newFeedback, fb_);
			fb_ = newFeedback;
			next(inNumSamples, fb);
		} else
			next_i(inNumSamples);
	}

	void next_a(int inNumSamples)
	{
		auto fb = makeSignal(1, [](float f) { return sc_clip(f, 0.f, 1.f);});
		next(inNumSamples, fb);
	}

	template <typename FeedBack>
	void next(int inNumSamples, FeedBack & fb)
	{
		const float * inSig = zin(0);
		float * outSig = zout(0);

		float zm1 = zm1_;

		loop (inNumSamples >> 2, [&] {
			const float x0 = ZXP(inSig);
			const float x1 = ZXP(inSig);
			const float x2 = ZXP(inSig);
			const float x3 = ZXP(inSig);

			float fb0 = fb.consume();
			float fb1 = fb.consume();
			float fb2 = fb.consume();
			float fb3 = fb.consume();

			float out0 = tick(x0, zm1, fb0);
			float out1 = tick(x1, zm1, fb1);
			float out2 = tick(x2, zm1, fb2);
			float out3 = tick(x3, zm1, fb3);

			ZXP(outSig) = out0;
			ZXP(outSig) = out1;
			ZXP(outSig) = out2;
			ZXP(outSig) = out3;
		});

		loop(inNumSamples & 3, [&] {
			const float x = ZXP(inSig);
			float feedback = fb.consume();
			ZXP(outSig) = tick(x, zm1, feedback);
		});

		zm1_ = zm1;
	}

	static float tick (float x, float & zm1, float feedback)
	{
		const float out = x * (1 + zm1 * feedback);
		zm1 = out;
		return out;
	}

	float zm1_;
	float fb_;
};

DEFINE_XTORS(FBAM)

PluginLoad(FBAM)
{
	ft = inTable;
	DefineSimpleUnit(FBAM);
}
