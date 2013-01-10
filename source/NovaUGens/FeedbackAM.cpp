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

static InterfaceTable *ft;

struct FBAM:
	public SCUnit
{
public:
	FBAM():
		zm1_(0)
	{
		set_calc_function<FBAM, &FBAM::next>();
	}

private:
	void next(int inNumSamples)
	{
		const float * inSig = zin(0);
		float feedback = in0(1);
		float * outSig = zout(0);

		float zm1 = zm1_;

		feedback = sc_clip(feedback, 0.0, 1.9); // really depends on the frequency

		for (int i = 0; i != inNumSamples; ++i) {
			const float x = ZXP(inSig);
			const float out = x * (1 + zm1 * feedback);

			zm1 = out;
			ZXP(outSig) = out;
		}

		zm1_ = zm1;
	}

	float zm1_;
};

DEFINE_XTORS(FBAM)

PluginLoad(FBAM)
{
	ft = inTable;
	DefineSimpleUnit(FBAM);
}
