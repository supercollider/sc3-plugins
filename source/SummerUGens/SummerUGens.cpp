/*
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

// Summer, based on Stepper, but allowing floats as inputs, so that you can increment a sum with each step
// WrapSummer, based on Stepper, but allowing floats as inputs, so that you can increment a sum with each step


#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct Summer : public Unit
{
	float mLevel;
	float m_prevtrig, m_prevreset;
};

struct WrapSummer : public Unit
{
	float mLevel;
	float m_prevtrig, m_prevreset;
};

extern "C"
{
	void load(InterfaceTable *inTable);

	void Summer_Ctor(Summer *unit);
	void Summer_next_aa(Summer *unit, int inNumSamples);
	void Summer_next_ak(Summer *unit, int inNumSamples);
	void Summer_next_a0(Summer *unit, int inNumSamples);
	void Summer_next_aaa(Summer *unit, int inNumSamples);
	void Summer_next_aka(Summer *unit, int inNumSamples);
	void Summer_next_a0a(Summer *unit, int inNumSamples);

	void WrapSummer_Ctor(Summer *unit);
	void WrapSummer_next_aa(Summer *unit, int inNumSamples);
	void WrapSummer_next_ak(Summer *unit, int inNumSamples);
	void WrapSummer_next_a0(Summer *unit, int inNumSamples);
	void WrapSummer_next_aaa(Summer *unit, int inNumSamples);
	void WrapSummer_next_aka(Summer *unit, int inNumSamples);
	void WrapSummer_next_a0a(Summer *unit, int inNumSamples);

}

void Summer_Ctor(Summer *unit)
{
	if (unit->mCalcRate == calc_FullRate && INRATE(0) == calc_FullRate && INRATE(2) == calc_ScalarRate) {
	  if ( INRATE(1) == calc_FullRate ){
		SETCALC(Summer_next_a0a);
	  } else {
		SETCALC(Summer_next_a0);
	  }
	} else if (unit->mCalcRate == calc_FullRate && INRATE(0) == calc_FullRate && INRATE(2) != calc_FullRate) {
	  if ( INRATE(1) == calc_FullRate ){
		SETCALC(Summer_next_aka);
	  } else {
		SETCALC(Summer_next_ak);
	  }
	} else {
	  if ( INRATE(1) == calc_FullRate ){
		SETCALC(Summer_next_aaa);
	  } else {
		SETCALC(Summer_next_aa);
	  }
	}

	unit->m_prevtrig = 0.f;
	unit->m_prevreset = 0.f;
	unit->mLevel = (float)ZIN0(3);

	ZOUT0(0) = 0.f;
}

void Summer_next_aa(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float step = ZIN0(1);
	float *reset = ZIN(2);
	float resetval = ZIN0(3);
	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
		float curtrig = ZXP(trig);
		float curreset = ZXP(reset);
		if (prevreset <= 0.f && curreset > 0.f) {
			level = resetval;
		} else if (prevtrig <= 0.f && curtrig > 0.f) {
			level = level + step;
		}
		ZXP(out) = level;
		prevtrig = curtrig;
		prevreset = curreset;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
}

void Summer_next_ak(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float step = ZIN0(1);
	float curreset = ZIN0(2);
	float resetval = ZIN0(3);
	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
	      float curtrig = ZXP(trig);
	      if (prevreset <= 0.f && curreset > 0.f) {
		level = resetval;
	      } else if (prevtrig <= 0.f && curtrig > 0.f) {
		level = level + step;
	      }
	      ZXP(out) = level;
	      prevtrig = curtrig;
	      prevreset = curreset;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
}

void Summer_next_a0(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float step = ZIN0(1);
	float prevtrig = unit->m_prevtrig;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
		float curtrig = ZXP(trig);
		if (prevtrig <= 0.f && curtrig > 0.f) {
		  level = level + step;
		}
		ZXP(out) = level;
		prevtrig = curtrig;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
}


void Summer_next_aaa(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float *step = ZIN(1);
	float *reset = ZIN(2);
	float resetval = ZIN0(3);
	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
	      float curtrig = ZXP(trig);
	      float curreset = ZXP(reset);
	      float curstep = ZXP(step);
		if (prevreset <= 0.f && curreset > 0.f) {
			level = resetval;
		} else if (prevtrig <= 0.f && curtrig > 0.f) {
			level = level + curstep;
		}
		ZXP(out) = level;
		prevtrig = curtrig;
		prevreset = curreset;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
}

void Summer_next_aka(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float *step = ZIN(1);
	float curreset = ZIN0(2);
	float resetval = ZIN0(3);
	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
	      float curtrig = ZXP(trig);
	      float curstep = ZXP(step);
	      if (prevreset <= 0.f && curreset > 0.f) {
		level = resetval;
	      } else if (prevtrig <= 0.f && curtrig > 0.f) {
		level = level + curstep;
	      }
	      ZXP(out) = level;
	      prevtrig = curtrig;
	      prevreset = curreset;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
}

void Summer_next_a0a(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float *step = ZIN(1);
	float prevtrig = unit->m_prevtrig;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
		float curtrig = ZXP(trig);
	      float curstep = ZXP(step);
		if (prevtrig <= 0.f && curtrig > 0.f) {
		  level = level + curstep;
		}
		ZXP(out) = level;
		prevtrig = curtrig;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
}


void WrapSummer_Ctor(Summer *unit)
{
	if (unit->mCalcRate == calc_FullRate && INRATE(0) == calc_FullRate && INRATE(4) == calc_ScalarRate) {
	    if ( INRATE(1) == calc_FullRate ){
	      SETCALC(WrapSummer_next_a0a);
	    } else {
	      SETCALC(WrapSummer_next_a0);
	    }
	} else if (unit->mCalcRate == calc_FullRate && INRATE(0) == calc_FullRate && INRATE(4) != calc_FullRate) {
	    if ( INRATE(1) == calc_FullRate ){
	      SETCALC(WrapSummer_next_aka);
	    } else {
		SETCALC(WrapSummer_next_ak);
	    }
	} else {
	    if ( INRATE(1) == calc_FullRate ){
	      SETCALC(WrapSummer_next_aaa);
	    } else {
	      SETCALC(WrapSummer_next_aa);
	    }
	}

	unit->m_prevtrig = 0.f;
	unit->m_prevreset = 0.f;
	unit->mLevel = (float)ZIN0(5);

	ZOUT0(0) = 0.f;
}

void WrapSummer_next_aaa(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float *step = ZIN(1);
	float zmin = ZIN0(2);
	float zmax = ZIN0(3);
	float *reset = ZIN(4);
	float resetval = ZIN0(5);
	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
	      float curtrig = ZXP(trig);
	      float curreset = ZXP(reset);
	      float curstep = ZXP(step);
	      if (prevreset <= 0.f && curreset > 0.f) {
		level = sc_wrap(resetval, zmin, zmax);
	      } else if (prevtrig <= 0.f && curtrig > 0.f) {
		level = sc_wrap(level + curstep, zmin, zmax);
	      }
	      ZXP(out) = level;
	      prevtrig = curtrig;
	      prevreset = curreset;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
}

void WrapSummer_next_aa(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float step = ZIN0(1);
	float zmin = ZIN0(2);
	float zmax = ZIN0(3);
	float *reset = ZIN(4);
	float resetval = ZIN0(5);
	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
		float curtrig = ZXP(trig);
		float curreset = ZXP(reset);
		if (prevreset <= 0.f && curreset > 0.f) {
			level = sc_wrap(resetval, zmin, zmax);
		} else if (prevtrig <= 0.f && curtrig > 0.f) {
			level = sc_wrap(level + step, zmin, zmax);
		}
		ZXP(out) = level;
		prevtrig = curtrig;
		prevreset = curreset;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
}

void WrapSummer_next_ak(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float step = ZIN0(1);
	float zmin = ZIN0(2);
	float zmax = ZIN0(3);
	float curreset = ZIN0(4);
	float resetval = ZIN0(5);
	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
		float curtrig = ZXP(trig);
		if (prevreset <= 0.f && curreset > 0.f) {
			level = sc_wrap(resetval, zmin, zmax);
		} else if (prevtrig <= 0.f && curtrig > 0.f) {
			level = sc_wrap(level + step, zmin, zmax);
		}
		ZXP(out) = level;
		prevtrig = curtrig;
		prevreset = curreset;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
}

void WrapSummer_next_aka(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float *step = ZIN(1);
	float zmin = ZIN0(2);
	float zmax = ZIN0(3);
	float curreset = ZIN0(4);
	float resetval = ZIN0(5);
	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
	      float curtrig = ZXP(trig);
	      float curstep = ZXP( step );
		if (prevreset <= 0.f && curreset > 0.f) {
			level = sc_wrap(resetval, zmin, zmax);
		} else if (prevtrig <= 0.f && curtrig > 0.f) {
			level = sc_wrap(level + curstep, zmin, zmax);
		}
		ZXP(out) = level;
		prevtrig = curtrig;
		prevreset = curreset;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
}

void WrapSummer_next_a0a(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float *step = ZIN(1);
	float zmin = ZIN0(2);
	float zmax = ZIN0(3);
	float prevtrig = unit->m_prevtrig;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
	      float curtrig = ZXP(trig);
	      float curstep = ZXP(step);
		if (prevtrig <= 0.f && curtrig > 0.f) {
		  level = sc_wrap(level + curstep, zmin, zmax);
		}
		ZXP(out) = level;
		prevtrig = curtrig;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
}

void WrapSummer_next_a0(Summer *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *trig = ZIN(0);
	float step = ZIN0(1);
	float zmin = ZIN0(2);
	float zmax = ZIN0(3);
	float prevtrig = unit->m_prevtrig;
	float level = unit->mLevel;

	LOOP1(inNumSamples,
		float curtrig = ZXP(trig);
		if (prevtrig <= 0.f && curtrig > 0.f) {
		  level = sc_wrap(level + step, zmin, zmax);
		}
		ZXP(out) = level;
		prevtrig = curtrig;
	);
	unit->mLevel = level;
	unit->m_prevtrig = prevtrig;
}


PluginLoad(Summer)
{
	ft = inTable;
	DefineSimpleUnit(Summer);
	DefineSimpleUnit(WrapSummer);
}
