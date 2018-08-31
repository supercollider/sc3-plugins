/* a miscellaneous set of probabilistic noise ugens by bhob rainey.  http://www.bhobrainey.net
Gendy4 is a version of Nick Collins's Gendy1 ugen with cubic
interpolation.  Gendy5 is the same, but with no interpolation.

SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct LFBrownNoise0 : public Unit
{
	int32 mCounter;
	float mLevel;
};

struct LFBrownNoise1 : public Unit
{
	int32 mCounter;
	float mSlope, mLevel;
};

struct LFBrownNoise2 : public Unit
{
	int32 mCounter;
	float mSlope, mLevel, mCurve, m_nextvalue;
};

struct Dbrown2 : public Unit
{
	int32 m_repeats, m_repeatCount;
	int m_dist;
	float m_lo, m_hi, m_step, m_val;
};

/* struct Dbeta : public Unit
{
	int32 m_repeats, m_repeatCount;
	float m_lo, m_hi, m_prob1, m_prob2;
}; */

struct Dgauss : public Unit
{
	int32 m_repeats, m_repeatCount;
	float m_lo, m_hi;
};

struct GaussTrig : public Unit
{
	int mCount;
};

struct TBrownRand : public Unit
{
	float m_trig, m_value;
};

struct TGaussRand : public Unit
{
	float m_trig, m_value;
};

struct TBetaRand : public Unit
{
	float m_trig, m_value;
};

struct Gendy4 : public Unit
{
	int32 mPhase;
	float mAmp, mNextAmp, mDur, mSlope, mMidpnt, mCurve;
	int mMemorySize, mIndex;
	float* mMemoryAmp;
	float* mMemoryDur;
};

struct Gendy5 : public Unit
{
	double mPhase;
	float mAmp, mDur, mSpeed;
	int mMemorySize, mIndex;
	float* mMemoryAmp;
	float* mMemoryDur;
};

// declare unit generator functions
extern "C"
{
	void TBrownRand_next_a(TBrownRand *unit, int inNumSamples);
	void TBrownRand_next_k(TBrownRand *unit, int inNumSamples);;
	void TBrownRand_Ctor(TBrownRand* unit);

	void Dbrown2_Ctor(Dbrown2 *unit);
	void Dbrown2_next(Dbrown2 *unit, int inNumSamples);

	/* void Dbeta_Ctor(Dbeta *unit);
	void Dbeta_next(Dbeta *unit, int inNumSamples); */

	void Dgauss_Ctor(Dgauss *unit);
	void Dgauss_next(Dgauss *unit, int inNumSamples);

	void TGaussRand_next_a(TGaussRand *unit, int inNumSamples);
	void TGaussRand_next_k(TGaussRand *unit, int inNumSamples);;
	void TGaussRand_Ctor(TGaussRand* unit);

	void TBetaRand_next_a(TBetaRand *unit, int inNumSamples);
	void TBetaRand_next_k(TBetaRand *unit, int inNumSamples);;
	void TBetaRand_Ctor(TBetaRand* unit);

	void GaussTrig_next(GaussTrig *unit, int inNumSamples);
	void GaussTrig_Ctor(GaussTrig* unit);

	void LFBrownNoise0_next(LFBrownNoise0 *unit, int inNumSamples);
	void LFBrownNoise0_Ctor(LFBrownNoise0* unit);

	void LFBrownNoise1_next(LFBrownNoise1 *unit, int inNumSamples);
	void LFBrownNoise1_Ctor(LFBrownNoise1* unit);

	void LFBrownNoise2_next(LFBrownNoise2 *unit, int inNumSamples);
	void LFBrownNoise2_Ctor(LFBrownNoise2* unit);

	void Gendy4_next_k(Gendy4 *unit, int inNumSamples);
	void Gendy4_Ctor(Gendy4* unit);
	void Gendy4_Dtor(Gendy4* unit);

	void Gendy5_next_k(Gendy5 *unit, int inNumSamples);
	void Gendy5_Ctor(Gendy5* unit);
	void Gendy5_Dtor(Gendy5* unit);
};

#define RGET \
	RGen& rgen = *unit->mParent->mRGen; \
	uint32 s1 = rgen.s1; \
	uint32 s2 = rgen.s2; \
	uint32 s3 = rgen.s3;

#define RPUT \
	rgen.s1 = s1; \
	rgen.s2 = s2; \
	rgen.s3 = s3;

// this is Nick Collins's version of Gendyn distributions
static inline float Gendyn_distribution( int which, float a, float f) {
	float temp, c;

	if(a>1.0) a=1.0;
	if(a<0.0001) a=0.0001;

	switch (which)
	{
		case 0: //LINEAR
			break;
		case 1: //CAUCHY
			c= atan(10*a);
			temp= (1/a)*tan(c*(2*f - 1));

			return temp*0.1;

		case 2: //LOGIST (ic)

			c=0.5+(0.499*a);
			c= log((1-c)/c);
			f= ((f-0.5)*0.998*a)+0.5;
			temp= log((1-f)/f)/c;

			return temp;

		case 3: //HYPERBCOS
			c=tan(1.5692255*a);
			temp= tan(1.5692255*a*f)/c;
			temp= log(temp*0.999+0.001)*(-0.1447648);

			return 2*temp-1.0;

		case 4: //ARCSINE
			c= sin(1.5707963*a);
			temp= sin(pi*(f-0.5)*a)/c;

			return temp;

		case 5: //EXPON
			c= log(1.0-(0.999*a));
			temp= log(1.0-(f*0.999*a))/c;

			return 2*temp-1.0;

		case 6: //SINUS
			return 2*a-1.0;

		default:
			break;
	}

	return 2*f-1.0;
}

static inline float Mirroring (float lower, float upper, float in)
{
	if(in>upper || in<lower) {
		if(in<lower) {
			float diff=lower - in;
			in = sc_min(upper, lower + diff);
		} else {
			float diff=in - upper;
			in = sc_max(lower, upper - diff);
		}
	}
	return in;
}

void GaussTrig_Ctor(GaussTrig* unit)
{
	RGET
	int nextSamples=(int)(SAMPLERATE/sc_max(ZIN0(0), 0.001f));
	float nextcount=sqrt(-2*log(frand(s1, s2, s3))) * sin(frand(s1, s2, s3) * twopi);
	nextcount=Mirroring(-1.f, 1.f, nextcount);
	int count=(int)(nextSamples + nextcount * ZIN0(1) * nextSamples);
	SETCALC(GaussTrig_next);
	unit->mCount = count;
	ZOUT0(0)=1.f;
	RPUT
}

void GaussTrig_next(GaussTrig* unit, int inNumSamples)
{
	float *out=ZOUT(0);
	float freq=ZIN0(0);
	int count=unit->mCount;

	LOOP(inNumSamples,
		if (count <= 0) {
			RGET
			int nextSamples=(int)(SAMPLERATE/sc_max(freq, 0.001f));
			float nextcount=sqrt(-2*log(frand(s1, s2, s3))) * sin(frand(s1, s2, s3) * twopi);
			nextcount=Mirroring(-1.0, 1.0, nextcount);
			count=(int)(nextSamples + nextcount * ZIN0(1) * nextSamples);
			ZXP(out)=1.f;
			RPUT
		} else {
			ZXP(out)=0.f;
		}
		count -= 1;
	);

	unit->mCount=count;
}

void LFBrownNoise0_next(LFBrownNoise0 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freq = ZIN0(0);
	float level = unit->mLevel;
	int32 counter = unit->mCounter;
	RGen& rgen = *unit->mParent->mRGen;

	int remain = inNumSamples;
	do {
		if (counter<=0) {
			counter = (int32)(unit->mRate->mSampleRate / sc_max(freq, .001f));
			counter = sc_max(1, counter);
			level = level + (ZIN0(1) * Gendyn_distribution((int)ZIN0(2), 1.0, rgen.frand()));
			level = Mirroring(-1.0, 1.0, level);
		}
		int nsmps = sc_min(remain, counter);
		remain -= nsmps;
		counter -= nsmps;
		LOOP(nsmps, ZXP(out) = level;);
	} while (remain);
	unit->mLevel = level;
	unit->mCounter = counter;

}

void LFBrownNoise0_Ctor(LFBrownNoise0* unit)
{
	SETCALC(LFBrownNoise0_next);

	unit->mCounter = 0;
	unit->mLevel = unit->mParent->mRGen->frand2();

	LFBrownNoise0_next(unit, 1);
}


void LFBrownNoise1_next(LFBrownNoise1 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freq = ZIN0(0);
	float level = unit->mLevel;
	float slope = unit->mSlope;
	int32 counter = unit->mCounter;
	RGen& rgen = *unit->mParent->mRGen;

	int remain = inNumSamples;

	do {
		if (counter<=0) {
			counter = (int32)(unit->mRate->mSampleRate / sc_max(freq, .001f));
			counter = sc_max(1, counter);
			float nextlevel = level + (ZIN0(1) * Gendyn_distribution((int)ZIN0(2), 1.0, rgen.frand()));
			nextlevel = Mirroring(-1.0, 1.0, nextlevel);
			slope = (nextlevel - level) / counter;
		}
		int nsmps = sc_min(remain, counter);
		remain -= nsmps;
		counter -= nsmps;
		LOOP(nsmps, ZXP(out) = level; level += slope;);
	} while (remain);
	unit->mLevel = level;
	unit->mSlope = slope;
	unit->mCounter = counter;

}

void LFBrownNoise1_Ctor(LFBrownNoise1* unit)
{
	SETCALC(LFBrownNoise1_next);

	unit->mCounter = 0;
	unit->mLevel = unit->mParent->mRGen->frand2();
	unit->mSlope = 0.f;

	LFBrownNoise1_next(unit, 1);
}

void LFBrownNoise2_next(LFBrownNoise2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freq = ZIN0(0);
	float level = unit->mLevel;
	float slope = unit->mSlope;
	float curve = unit->mCurve;
	int32 counter = unit->mCounter;
	RGen& rgen = *unit->mParent->mRGen;

	int remain = inNumSamples;
	do {
		if (counter<=0) {
			float value = unit->m_nextvalue;
			float nextlevel = value + (ZIN0(1) * Gendyn_distribution((int)ZIN0(2), 1.0, rgen.frand()));
			nextlevel = Mirroring(-1.0, 1.0, nextlevel);
			unit->m_nextvalue=nextlevel;
			float nextmidpt = (nextlevel + value) * 0.5;

			counter = (int32)(unit->mRate->mSampleRate / sc_max(freq, .001f));
			counter = sc_max(2, counter);
			float fseglen = (float)counter;
			curve = 2.f * (nextmidpt - level - fseglen * slope) / (fseglen * fseglen + fseglen);
		}
		int nsmps = sc_min(remain, counter);
		remain -= nsmps;
		counter -= nsmps;
		LOOP(nsmps,
			ZXP(out) = level;
			slope += curve;
			level += slope;
		);
	} while (remain);
	unit->mLevel = level;
	unit->mSlope = slope;
	unit->mCurve = curve;
	unit->mCounter = counter;

}

void LFBrownNoise2_Ctor(LFBrownNoise2* unit)
{
	SETCALC(LFBrownNoise2_next);

	unit->mCounter = 0;
	unit->mSlope = 0.f;
	unit->mLevel = unit->mParent->mRGen->frand2();
	float nextvalue = unit->mLevel + unit->mParent->mRGen->frand2() * ZIN0(2);
	nextvalue = Mirroring(-1.0, 1.0, nextvalue);
	unit->m_nextvalue = nextvalue;
	LFBrownNoise2_next(unit, 1);
}

void Dbrown2_next(Dbrown2 *unit, int inNumSamples)
{
	if (inNumSamples) {
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
			unit->m_lo = DEMANDINPUT(1);
			unit->m_hi = DEMANDINPUT(2);
			unit->m_step = DEMANDINPUT(3);
			unit->m_dist = (int)DEMANDINPUT(4);
			unit->m_val = unit->mParent->mRGen->frand() * (unit->m_hi - unit->m_lo) + unit->m_lo;
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		unit->m_repeatCount++;
		OUT0(0) = unit->m_val;
		float x = unit->m_val + Gendyn_distribution(unit->m_dist, 1.0, unit->mParent->mRGen->frand()) * unit->m_step;
		unit->m_val = sc_fold(x, unit->m_lo, unit->m_hi);
	} else {
		unit->m_repeats = -1;
		unit->m_repeatCount = 0;
	}
}

void Dbrown2_Ctor(Dbrown2 *unit)
{
	SETCALC(Dbrown2_next);
	Dbrown2_next(unit, 0);
	OUT0(0) = 0.f;
}

/*
void Dbeta_next(Dbeta *unit, int inNumSamples)
{
	float sum=2.f;
	float value;
	if (inNumSamples) {
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
			unit->m_lo = DEMANDINPUT(1);
			unit->m_hi = DEMANDINPUT(2);
			unit->m_prob1 = DEMANDINPUT(3);
			unit->m_prob2 = DEMANDINPUT(4);
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		unit->m_repeatCount++;
		while (sum > 1.f) {
			value=pow(unit->mParent->mRGen->frand(), unit->m_prob1);
			sum=value + pow(unit->mParent->mRGen->frand(), unit->m_prob2);
		};
		OUT0(0) = (value/sum) * (unit->m_hi-unit->m_lo) + unit->m_lo;
	} else {
		unit->m_repeats = -1;
		unit->m_repeatCount = 0;
	}
}

void Dbeta_Ctor(Dbeta *unit)
{
	SETCALC(Dbeta_next);
	Dbeta_next(unit, 0);
	OUT0(0) = 0.f;
} */

void Dgauss_next(Dgauss *unit, int inNumSamples)
{
	if (inNumSamples) {
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
			unit->m_lo = DEMANDINPUT(1);
			unit->m_hi = DEMANDINPUT(2);
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		unit->m_repeatCount++;
		float range = (unit->m_hi - unit->m_lo);
		float value = ((sqrt(-2*log(unit->mParent->mRGen->frand())) * sin(unit->mParent->mRGen->frand() * twopi)) + 1) * 0.5;
		value=Mirroring(0.0, 1.0, value);
		OUT0(0) = value * range + unit->m_lo;
	} else {
		unit->m_repeats = -1;
		unit->m_repeatCount = 0;
	}
}

void Dgauss_Ctor(Dgauss *unit)
{
	SETCALC(Dgauss_next);
	Dgauss_next(unit, 0);
	OUT0(0) = 0.f;
}

void TBrownRand_next_k(TBrownRand* unit, int inNumSamples)
{
	float trig = ZIN0(4);
	float lo = ZIN0(0);
	float hi = ZIN0(1);
	float range = (hi - lo) * 0.5;
	RGen& rgen = *unit->mParent->mRGen;

	if (trig > 0.f && unit->m_trig <= 0.f) {
		float value = unit->m_value + (ZIN0(2)*Gendyn_distribution((int)ZIN0(3), 1.0, rgen.frand()));
		value = Mirroring(-1.0, 1.0, value);
		ZOUT0(0) = value * range + range + lo;
		unit->m_value=value;
	} else {
		ZOUT0(0) = unit->m_value * range + range + lo;
	}
	unit->m_trig = trig;
}

void TBrownRand_next_a(TBrownRand* unit, int inNumSamples)
{
	float *trig = ZIN(4);
	float prev = unit->m_trig;
	float *out = ZOUT(0);
	float oldval = unit->m_value;
	float next, range, value;
	RGen& rgen = *unit->mParent->mRGen;

	LOOP(inNumSamples,
		next = ZXP(trig);
		if (next > 0.f &&  prev <= 0.f) {
			float lo=ZIN0(0);
			float hi=ZIN0(1);
			range = (hi - lo) * 0.5;
			value = oldval + (ZIN0(2)*Gendyn_distribution((int)ZIN0(3), 1.0, rgen.frand()));
			value = Mirroring(-1.0, 1.0, value);
			ZXP(out) = value * range + range + lo;
			oldval=value;
			prev=next;
		} else {
			range = (ZIN0(1) - ZIN0(0)) * 0.5;
			ZXP(out) = oldval * range + range + ZIN0(0);
		};
		prev = next;
	)

	unit->m_trig = next;
	unit->m_value=oldval;
}

void TBrownRand_Ctor(TBrownRand* unit)
{
	float lo = ZIN0(0);
	float hi = ZIN0(1);
	float range = (hi - lo) * 0.5;
	RGen& rgen = *unit->mParent->mRGen;
	unit->m_value = rgen.frand2();
	ZOUT0(0) = unit->m_value * range + range + lo;
	if(unit->mCalcRate == calc_FullRate){ SETCALC(TBrownRand_next_a); } else { SETCALC(TBrownRand_next_k); }
	unit->m_trig = ZIN0(3);
}

void TGaussRand_next_k(TGaussRand* unit, int inNumSamples)
{
	float trig = ZIN0(2);
	float lo = ZIN0(0);
	float hi = ZIN0(1);
	float range = (hi - lo);

	if (trig > 0.f && unit->m_trig <= 0.f) {
		RGET
		float value = ((sqrt(-2*log(frand(s1, s2, s3))) * sin(frand(s1, s2, s3) * twopi)) + 1) * 0.5;
		value=Mirroring(0.0, 1.0, value);
		ZOUT0(0) = value * range + lo;
		unit->m_value=value;
		RPUT
	} else {
		ZOUT0(0) = unit->m_value * range + lo;
	}
	unit->m_trig = trig;
}

void TGaussRand_next_a(TGaussRand* unit, int inNumSamples)
{
	float *trig = ZIN(2);
	float prev = unit->m_trig;
	float *out = ZOUT(0);
	float value = unit->m_value;
	float range, next;

	LOOP(inNumSamples,
		next = ZXP(trig);
		if (next > 0.f &&  prev <= 0.f) {
			RGET
			float lo=ZIN0(0);
			float hi=ZIN0(1);
			range = (hi - lo);
			value = ((sqrt(-2*log(frand(s1, s2, s3))) * sin(frand(s1, s2, s3) * twopi)) + 1) * 0.5;
			value=Mirroring(0.0, 1.0, value);
			ZXP(out) = value * range + lo;
			prev=next;
			RPUT
		} else {
			range = (ZIN0(1) - ZIN0(0));
			ZXP(out) = value * range + ZIN0(0);
		};
		prev = next;
	)

	unit->m_trig = next;
	unit->m_value=value;
}

void TGaussRand_Ctor(TGaussRand* unit)
{
	float lo = ZIN0(0);
	float hi = ZIN0(1);
	float range = (hi - lo);
	RGen& rgen = *unit->mParent->mRGen;
	unit->m_value = rgen.frand();
	ZOUT0(0) = unit->m_value * range + lo;
	if(unit->mCalcRate == calc_FullRate){ SETCALC(TGaussRand_next_a); } else { SETCALC(TGaussRand_next_k); }
	unit->m_trig = ZIN0(2);
}

void TBetaRand_next_k(TBetaRand* unit, int inNumSamples)
{
	float trig = ZIN0(4);
	float lo = ZIN0(0);
	float hi = ZIN0(1);
	float prob1 = 1/ZIN0(2);
	float prob2 = 1/ZIN0(3);
	float sum = 2.f;
	float value = unit->m_value;

	if (trig > 0.f && unit->m_trig <= 0.f) {
		RGET
		while (sum > 1.f) {
			value=pow(frand(s1, s2, s3), prob1);
			sum=value + pow(frand(s1, s2, s3), prob2);
		};
		if (sum == 0.f) {
			ZOUT0(0) = unit->m_value = 0.f;
		} else {
			ZOUT0(0) = unit->m_value = (value/sum) * (hi-lo) + lo;
		}
		RPUT
	} else {
		ZOUT0(0) = unit->m_value;
	}
	unit->m_trig = trig;
}

void TBetaRand_next_a(TBetaRand* unit, int inNumSamples)
{
	float *trig = ZIN(4);
	float *prob1 = ZIN(2);
	float *prob2 = ZIN(3);
	float prev = unit->m_trig;
	float *out = ZOUT(0);
	float value = unit->m_value;
	float sum = 2.f;
	float next;

	LOOP(inNumSamples,
		float invProb1 = 1/ZXP(prob1);
		float invProb2 = 1/ZXP(prob2);
		next = ZXP(trig);
		if (next > 0.f &&  prev <= 0.f) {
			RGET
			while (sum > 1.f) {
				value=pow(frand(s1, s2, s3), invProb1);
				sum=value + pow(frand(s1, s2, s3), invProb2);
			};
			float lo=ZIN0(0);
			float hi=ZIN0(1);
			if (sum == 0.f) {
				ZXP(out) = value = 0.f;
			} else {
				ZXP(out) = value = (value/sum) * (hi - lo) + lo;
			}
			prev=next;
			RPUT
		} else {
			ZXP(out) = value;
			prev = next;
		};
	);

	unit->m_trig = next;
	unit->m_value=value;
}

void TBetaRand_Ctor(TBetaRand* unit)
{
	float lo = ZIN0(0);
	float hi = ZIN0(1);
	float prob1 = 1/ZIN0(2);
	float prob2 = 1/ZIN0(3);
	float sum = 2.f;
	float value;
	RGET
	while (sum > 1.f) {
		value=pow(frand(s1, s2, s3), prob1);
		sum=value + pow(frand(s1, s2, s3), prob2);
	};
	ZOUT0(0) = unit->m_value = (value/sum) * (hi-lo) + lo;
	RPUT
	if(unit->mCalcRate == calc_FullRate){ SETCALC(TBetaRand_next_a); } else { SETCALC(TBetaRand_next_k); }
	unit->m_trig = ZIN0(4);
}

void Gendy4_Ctor( Gendy4* unit ) {

	SETCALC(Gendy4_next_k);
	unit->mPhase = 0;
	unit->mAmp = 0.0;
	unit->mNextAmp = 0.0;
	unit->mSlope = 0.f;
	unit->mMidpnt = 0.f;

	unit->mMemorySize= (int) ZIN0(8);	//default is 12
	if(unit->mMemorySize<1) unit->mMemorySize=1;
	unit->mIndex=0;
	unit->mMemoryAmp= (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));
	unit->mMemoryDur= (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));


	RGen& rgen = *unit->mParent->mRGen;

	int i=0;
	for(i=0; i<unit->mMemorySize;++i) {
		unit->mMemoryAmp[i]=2*rgen.frand() - 1.0;
		unit->mMemoryDur[i]=rgen.frand();
	}
}

void Gendy4_Dtor(Gendy4 *unit)
{
	RTFree(unit->mWorld, unit->mMemoryAmp);
	RTFree(unit->mWorld, unit->mMemoryDur);
}

void Gendy4_next_k(Gendy4 *unit, int inNumSamples) {

	float *out = ZOUT(0);

	int whichamp= (int)ZIN0(0);
	int whichdur= (int)ZIN0(1);
	float aamp = ZIN0(2);
	float adur = ZIN0(3);
	float minfreq = ZIN0(4);
	float maxfreq = ZIN0(5);
	float scaleamp = ZIN0(6);
	float scaledur = ZIN0(7);

	float rate=unit->mDur;
	float slope=unit->mSlope;
	float midpnt=unit->mMidpnt;
	float curve=unit->mCurve;

	int32 phase = unit->mPhase;

	float amp=unit->mAmp;
	float nextamp=unit->mNextAmp;
	RGen& rgen = *unit->mParent->mRGen;

	int remain = inNumSamples;
	do {
		if (phase <= 0) {

			int index= unit->mIndex;
			int num= (int)(ZIN0(9));
			if((num>(unit->mMemorySize)) || (num<1)) num=unit->mMemorySize;
			index=(index+1)%num;
			amp=nextamp;
			unit->mIndex=index;
			nextamp=(unit->mMemoryAmp[index])+(scaleamp*Gendyn_distribution(whichamp, aamp, rgen.frand()));
			nextamp=Mirroring(-1.f, 1.f, nextamp);
			float next_midpnt = (amp + nextamp) * 0.5;
			unit->mMemoryAmp[index] = nextamp;

			rate=(unit->mMemoryDur[index])+(scaledur*Gendyn_distribution(whichdur, adur, rgen.frand()));
			rate=Mirroring(0.0, 1.0, rate);
			unit->mMemoryDur[index]=rate;
			float fphase=(minfreq+((maxfreq-minfreq)*rate)) * num;
			fphase=sc_max(fphase, 0.001f);
			phase=(int32)(unit->mRate->mSampleRate/fphase);
			phase=sc_max(2, phase);
			curve = 2.f * (next_midpnt - midpnt - phase * slope) / (phase * phase + phase);
		}

		int nsmps = sc_min(remain, phase);
		remain -= nsmps;
		phase -= nsmps;
		LOOP(nsmps,
			ZXP(out) = midpnt;
			slope += curve;
			midpnt += slope;
		);
	} while (remain);

	unit->mPhase = phase;
	unit->mAmp =  amp;
	unit->mNextAmp = nextamp;
	unit->mMidpnt = midpnt;
	unit->mDur = rate;
	unit->mSlope = slope;
	unit->mCurve = curve;
}

void Gendy5_Ctor( Gendy5* unit ) {

	SETCALC(Gendy5_next_k);
	unit->mPhase = 1.f;
	unit->mAmp = 0.0;

	unit->mMemorySize= (int) ZIN0(8);
	if(unit->mMemorySize<1) unit->mMemorySize=1;
	unit->mIndex=0;
	unit->mMemoryAmp= (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));
	unit->mMemoryDur= (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));


	RGen& rgen = *unit->mParent->mRGen;

	int i=0;
	for(i=0; i<unit->mMemorySize;++i) {
		unit->mMemoryAmp[i]=2*rgen.frand() - 1.0;
		unit->mMemoryDur[i]=rgen.frand();
	}
}

void Gendy5_Dtor(Gendy5 *unit)
{
	RTFree(unit->mWorld, unit->mMemoryAmp);
	RTFree(unit->mWorld, unit->mMemoryDur);
}

void Gendy5_next_k(Gendy5 *unit, int inNumSamples) {

	float *out = ZOUT(0);

	int whichamp = (int)ZIN0(0);
	int whichdur = (int)ZIN0(1);
	float aamp = ZIN0(2);
	float adur = ZIN0(3);
	float minfreq = ZIN0(4);
	float maxfreq = ZIN0(5);
	float scaleamp = ZIN0(6);
	float scaledur = ZIN0(7);

	float rate=unit->mDur;

	double phase = unit->mPhase;
	float speed = unit->mSpeed;

	float amp=unit->mAmp;
	RGen& rgen = *unit->mParent->mRGen;

	LOOP(inNumSamples,
		if (phase >= 1.f) {
			phase -= 1.f;

			int index=unit->mIndex;
			int num=(int)(ZIN0(9));
			if((num>(unit->mMemorySize)) || (num<1)) num=unit->mMemorySize;
			index=(index+1)%num;
			unit->mIndex=index;
			amp=(unit->mMemoryAmp[index])+(scaleamp*Gendyn_distribution(whichamp, aamp, rgen.frand()));
			amp=Mirroring(-1.f, 1.f, amp);
			unit->mMemoryAmp[index]= amp;

			rate=(unit->mMemoryDur[index])+(scaledur*Gendyn_distribution(whichdur, adur, rgen.frand()));
			rate=Mirroring(0.0, 1.f, rate);
			unit->mMemoryDur[index]= rate;
			speed=(minfreq+((maxfreq-minfreq)*rate))*SAMPLEDUR*num;
		}

		phase +=  speed;
		ZXP(out) = amp;
	);

	unit->mPhase = phase;
	unit->mSpeed = speed;
	unit->mAmp =  amp;
	unit->mDur = rate;
}

PluginLoad(BhobNoise)
{
	ft = inTable;

	DefineSimpleUnit(GaussTrig);
	DefineSimpleUnit(LFBrownNoise0);
	DefineSimpleUnit(LFBrownNoise1);
	DefineSimpleUnit(LFBrownNoise2);
	DefineSimpleUnit(TBrownRand);
	DefineSimpleUnit(Dbrown2);
	// DefineSimpleUnit(Dbeta);
	DefineSimpleUnit(Dgauss);
	DefineSimpleUnit(TGaussRand);
	DefineSimpleUnit(TBetaRand);
	DefineDtorUnit(Gendy4);
	DefineDtorUnit(Gendy5);
}

////////////////////////////////////////////////////////////////////
