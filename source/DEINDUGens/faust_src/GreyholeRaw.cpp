/* ------------------------------------------------------------
author: "Julian Parker, bug fixes by Till Bovermann"
copyright: "(c) Julian Parker 2013"
license: "GPL2+"
name: "GreyholeRaw"
version: "1.0"
Code generated with Faust 2.15.11 (https://faust.grame.fr)
Compilation options: -vec -lv 0 -vs 8 -ftz 0 -mcd 16
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

//-------------------------------------------------------------------
// FAUST architecture file for SuperCollider.
// Copyright (C) 2005-2012 Stefan Kersten.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
// 02111-1307 USA
//-------------------------------------------------------------------

// The prefix is set to "Faust" in the faust2supercollider script, otherwise set empty
#if !defined(SC_FAUST_PREFIX)
#define SC_FAUST_PREFIX ""
#endif

#include <map>
#include <string>
#include <string.h>
#include <SC_PlugIn.h>

#include <faust/dsp/dsp.h>
#include <faust/gui/UI.h>
#include <faust/misc.h>

using namespace std;

#if defined(__GNUC__) && __GNUC__ >= 4
    #define FAUST_EXPORT __attribute__((visibility("default")))
#else
    #define FAUST_EXPORT  SC_API_EXPORT
#endif

#ifdef WIN32
    #define STRDUP _strdup
#else
    #define STRDUP strdup
#endif

//----------------------------------------------------------------------------
// Vector intrinsics
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Metadata
//----------------------------------------------------------------------------

class MetaData : public Meta
               , public std::map<std::string, std::string>
{
public:
    void declare(const char* key, const char* value)
    {
        (*this)[key] = value;
    }
};

//----------------------------------------------------------------------------
// Control counter
//----------------------------------------------------------------------------

class ControlCounter : public UI
{
public:
    ControlCounter()
        : mNumControlInputs(0),
          mNumControlOutputs(0)
    {}

    size_t getNumControls() const { return getNumControlInputs(); }
    size_t getNumControlInputs() const { return mNumControlInputs; }
    size_t getNumControlOutputs() const { return mNumControlOutputs; }

    // Layout widgets
    virtual void openTabBox(const char* label) { }
    virtual void openHorizontalBox(const char* label) { }
    virtual void openVerticalBox(const char* label) { }
    virtual void closeBox() { }

    // Active widgets
    virtual void addButton(const char* label, FAUSTFLOAT* zone)
    { addControlInput(); }
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone)
    { addControlInput(); }
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    { addControlInput(); }
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    { addControlInput(); }
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    { addControlInput(); }

    // Passive widgets
    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
    { addControlOutput(); }
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
    { addControlOutput(); }
    
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}

protected:
    void addControlInput() { mNumControlInputs++; }
    void addControlOutput() { mNumControlOutputs++; }

private:
    size_t mNumControlInputs;
    size_t mNumControlOutputs;
};

//----------------------------------------------------------------------------
// UI control
//----------------------------------------------------------------------------

struct Control
{
    typedef void (*UpdateFunction)(Control* self, FAUSTFLOAT value);

    UpdateFunction updateFunction;
    FAUSTFLOAT* zone;
    FAUSTFLOAT min, max;

    inline void update(FAUSTFLOAT value)
    {
        (*updateFunction)(this, value);
    }

    static void simpleUpdate(Control* self, FAUSTFLOAT value)
    {
        *self->zone = value;
    }
    static void boundedUpdate(Control* self, FAUSTFLOAT value)
    {
        *self->zone = sc_clip(value, self->min, self->max);
    }
};

//----------------------------------------------------------------------------
// Control allocator
//----------------------------------------------------------------------------

class ControlAllocator : public UI
{
public:
    ControlAllocator(Control* controls)
        : mControls(controls)
    { }

    // Layout widgets
    virtual void openTabBox(const char* label) { }
    virtual void openHorizontalBox(const char* label) { }
    virtual void openVerticalBox(const char* label) { }
    virtual void closeBox() { }

    // Active widgets
    virtual void addButton(const char* label, FAUSTFLOAT* zone)
    { addSimpleControl(zone); }
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone)
    { addSimpleControl(zone); }
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    { addBoundedControl(zone, min, max, step); }
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    { addBoundedControl(zone, min, max, step); }
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    { addBoundedControl(zone, min, max, step); }

    // Passive widgets
    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}

private:
    void addControl(Control::UpdateFunction updateFunction, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT /* step */)
    {
        Control* ctrl        = mControls++;
        ctrl->updateFunction = updateFunction;
        ctrl->zone           = zone;
        ctrl->min            = min;
        ctrl->max            = max;
    }
    void addSimpleControl(FAUSTFLOAT* zone)
    {
        addControl(Control::simpleUpdate, zone, 0.f, 0.f, 0.f);
    }
    void addBoundedControl(FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    {
        addControl(Control::boundedUpdate, zone, min, max, step);
    }

private:
    Control* mControls;
};

//----------------------------------------------------------------------------
// FAUST generated code
//----------------------------------------------------------------------------

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

/* link with : "primes" */
#include <algorithm>
#include <cmath>
#include <jprev.h>
#include <math.h>


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif
#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp : public dsp {
	
 private:
	
	int fSamplingFreq;
	float fConst0;
	FAUSTFLOAT fHslider0;
	float fRec39_perm[4];
	float fRec40_perm[4];
	float fRec41_perm[4];
	float fRec42_perm[4];
	float fConst1;
	FAUSTFLOAT fHslider1;
	float fVec0_perm[4];
	float fRec43_perm[4];
	int iVec1_perm[4];
	float fRec44_perm[4];
	FAUSTFLOAT fHslider2;
	float fRec45_perm[4];
	FAUSTFLOAT fHslider3;
	float fVec2_perm[4];
	FAUSTFLOAT fHslider4;
	float fVec3_perm[4];
	float fConst2;
	FAUSTFLOAT fHslider5;
	float fVec4_perm[4];
	float fYec0[131072];
	int fYec0_idx;
	int fYec0_idx_save;
	float fYec1[16384];
	int fYec1_idx;
	int fYec1_idx_save;
	float fYec2_perm[4];
	float fRec38_perm[4];
	float fRec36_perm[4];
	float fRec47_perm[4];
	float fYec3[131072];
	int fYec3_idx;
	int fYec3_idx_save;
	float fYec4[16384];
	int fYec4_idx;
	int fYec4_idx_save;
	float fYec5_perm[4];
	float fRec46_perm[4];
	float fRec37_perm[4];
	float fRec48_perm[4];
	float fYec6[16384];
	int fYec6_idx;
	int fYec6_idx_save;
	float fYec7_perm[4];
	float fRec35_perm[4];
	float fRec33_perm[4];
	float fRec50_perm[4];
	float fYec8[16384];
	int fYec8_idx;
	int fYec8_idx_save;
	float fYec9_perm[4];
	float fRec49_perm[4];
	float fRec34_perm[4];
	float fRec51_perm[4];
	float fYec10[16384];
	int fYec10_idx;
	int fYec10_idx_save;
	float fYec11_perm[4];
	float fRec32_perm[4];
	float fRec30_perm[4];
	float fRec53_perm[4];
	float fYec12[16384];
	int fYec12_idx;
	int fYec12_idx_save;
	float fYec13_perm[4];
	float fRec52_perm[4];
	float fRec31_perm[4];
	float fRec54_perm[4];
	float fYec14[16384];
	int fYec14_idx;
	int fYec14_idx_save;
	float fYec15_perm[4];
	float fRec29_perm[4];
	float fRec27_perm[4];
	float fRec56_perm[4];
	float fYec16[16384];
	int fYec16_idx;
	int fYec16_idx_save;
	float fYec17_perm[4];
	float fRec55_perm[4];
	float fRec28_perm[4];
	float fRec57_perm[4];
	float fYec18[16384];
	int fYec18_idx;
	int fYec18_idx_save;
	float fYec19_perm[4];
	float fRec26_perm[4];
	float fRec24_perm[4];
	float fRec59_perm[4];
	float fYec20[16384];
	int fYec20_idx;
	int fYec20_idx_save;
	float fYec21_perm[4];
	float fRec58_perm[4];
	float fRec25_perm[4];
	float fRec60_perm[4];
	float fYec22[16384];
	int fYec22_idx;
	int fYec22_idx_save;
	float fYec23_perm[4];
	float fRec23_perm[4];
	float fRec21_perm[4];
	float fRec62_perm[4];
	float fYec24[16384];
	int fYec24_idx;
	int fYec24_idx_save;
	float fYec25_perm[4];
	float fRec61_perm[4];
	float fRec22_perm[4];
	float fRec63_perm[4];
	float fYec26[16384];
	int fYec26_idx;
	int fYec26_idx_save;
	float fYec27_perm[4];
	float fRec20_perm[4];
	float fRec18_perm[4];
	float fRec65_perm[4];
	float fYec28[16384];
	int fYec28_idx;
	int fYec28_idx_save;
	float fYec29_perm[4];
	float fRec64_perm[4];
	float fRec19_perm[4];
	float fRec66_perm[4];
	float fYec30[16384];
	int fYec30_idx;
	int fYec30_idx_save;
	float fYec31_perm[4];
	float fRec17_perm[4];
	float fRec15_perm[4];
	float fRec68_perm[4];
	float fYec32[16384];
	int fYec32_idx;
	int fYec32_idx_save;
	float fYec33_perm[4];
	float fRec67_perm[4];
	float fRec16_perm[4];
	float fRec69_perm[4];
	float fYec34[16384];
	int fYec34_idx;
	int fYec34_idx_save;
	float fYec35_perm[4];
	float fRec14_perm[4];
	float fRec12_perm[4];
	float fRec71_perm[4];
	float fYec36[16384];
	int fYec36_idx;
	int fYec36_idx_save;
	float fYec37_perm[4];
	float fRec70_perm[4];
	float fRec13_perm[4];
	float fRec72_perm[4];
	float fYec38[16384];
	int fYec38_idx;
	int fYec38_idx_save;
	float fYec39_perm[4];
	float fRec11_perm[4];
	float fRec9_perm[4];
	float fRec74_perm[4];
	float fYec40[16384];
	int fYec40_idx;
	int fYec40_idx_save;
	float fYec41_perm[4];
	float fRec73_perm[4];
	float fRec10_perm[4];
	float fRec75_perm[4];
	float fYec42[16384];
	int fYec42_idx;
	int fYec42_idx_save;
	float fYec43_perm[4];
	float fRec8_perm[4];
	float fRec6_perm[4];
	float fRec77_perm[4];
	float fYec44[16384];
	int fYec44_idx;
	int fYec44_idx_save;
	float fYec45_perm[4];
	float fRec76_perm[4];
	float fRec7_perm[4];
	float fRec78_perm[4];
	float fYec46[16384];
	int fYec46_idx;
	int fYec46_idx_save;
	float fYec47_perm[4];
	float fRec5_perm[4];
	float fRec3_perm[4];
	float fRec80_perm[4];
	float fYec48[16384];
	int fYec48_idx;
	int fYec48_idx_save;
	float fYec49_perm[4];
	float fRec79_perm[4];
	float fRec4_perm[4];
	FAUSTFLOAT fHslider6;
	float fVec5_perm[4];
	float fRec2_perm[4];
	float fRec0[1024];
	int fRec0_idx;
	int fRec0_idx_save;
	float fRec81_perm[4];
	float fRec1[1024];
	int fRec1_idx;
	int fRec1_idx_save;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Julian Parker, bug fixes by Till Bovermann");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Julian Parker 2013");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.1");
		m->declare("filename", "GreyholeRaw");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("license", "GPL2+");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("name", "GreyholeRaw");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 2;
		
	}
	virtual int getNumOutputs() {
		return 2;
		
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 0;
				break;
			}
			case 1: {
				rate = 0;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			case 1: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	
	static void classInit(int samplingFreq) {
		
	}
	
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = std::min<float>(192000.0f, std::max<float>(1.0f, float(fSamplingFreq)));
		fConst1 = (3.14159274f / fConst0);
		fConst2 = (0.000566893432f * fConst0);
		
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.20000000000000001f);
		fHslider1 = FAUSTFLOAT(2.0f);
		fHslider2 = FAUSTFLOAT(1.0f);
		fHslider3 = FAUSTFLOAT(0.5f);
		fHslider4 = FAUSTFLOAT(0.90000000000000002f);
		fHslider5 = FAUSTFLOAT(0.10000000000000001f);
		fHslider6 = FAUSTFLOAT(0.0f);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 4); l0 = (l0 + 1)) {
			fRec39_perm[l0] = 0.0f;
			
		}
		for (int l1 = 0; (l1 < 4); l1 = (l1 + 1)) {
			fRec40_perm[l1] = 0.0f;
			
		}
		for (int l2 = 0; (l2 < 4); l2 = (l2 + 1)) {
			fRec41_perm[l2] = 0.0f;
			
		}
		for (int l3 = 0; (l3 < 4); l3 = (l3 + 1)) {
			fRec42_perm[l3] = 0.0f;
			
		}
		for (int l4 = 0; (l4 < 4); l4 = (l4 + 1)) {
			fVec0_perm[l4] = 0.0f;
			
		}
		for (int l5 = 0; (l5 < 4); l5 = (l5 + 1)) {
			fRec43_perm[l5] = 0.0f;
			
		}
		for (int l6 = 0; (l6 < 4); l6 = (l6 + 1)) {
			iVec1_perm[l6] = 0;
			
		}
		for (int l7 = 0; (l7 < 4); l7 = (l7 + 1)) {
			fRec44_perm[l7] = 0.0f;
			
		}
		for (int l8 = 0; (l8 < 4); l8 = (l8 + 1)) {
			fRec45_perm[l8] = 0.0f;
			
		}
		for (int l9 = 0; (l9 < 4); l9 = (l9 + 1)) {
			fVec2_perm[l9] = 0.0f;
			
		}
		for (int l10 = 0; (l10 < 4); l10 = (l10 + 1)) {
			fVec3_perm[l10] = 0.0f;
			
		}
		for (int l11 = 0; (l11 < 4); l11 = (l11 + 1)) {
			fVec4_perm[l11] = 0.0f;
			
		}
		for (int l12 = 0; (l12 < 131072); l12 = (l12 + 1)) {
			fYec0[l12] = 0.0f;
			
		}
		fYec0_idx = 0;
		fYec0_idx_save = 0;
		for (int l13 = 0; (l13 < 16384); l13 = (l13 + 1)) {
			fYec1[l13] = 0.0f;
			
		}
		fYec1_idx = 0;
		fYec1_idx_save = 0;
		for (int l14 = 0; (l14 < 4); l14 = (l14 + 1)) {
			fYec2_perm[l14] = 0.0f;
			
		}
		for (int l15 = 0; (l15 < 4); l15 = (l15 + 1)) {
			fRec38_perm[l15] = 0.0f;
			
		}
		for (int l16 = 0; (l16 < 4); l16 = (l16 + 1)) {
			fRec36_perm[l16] = 0.0f;
			
		}
		for (int l17 = 0; (l17 < 4); l17 = (l17 + 1)) {
			fRec47_perm[l17] = 0.0f;
			
		}
		for (int l18 = 0; (l18 < 131072); l18 = (l18 + 1)) {
			fYec3[l18] = 0.0f;
			
		}
		fYec3_idx = 0;
		fYec3_idx_save = 0;
		for (int l19 = 0; (l19 < 16384); l19 = (l19 + 1)) {
			fYec4[l19] = 0.0f;
			
		}
		fYec4_idx = 0;
		fYec4_idx_save = 0;
		for (int l20 = 0; (l20 < 4); l20 = (l20 + 1)) {
			fYec5_perm[l20] = 0.0f;
			
		}
		for (int l21 = 0; (l21 < 4); l21 = (l21 + 1)) {
			fRec46_perm[l21] = 0.0f;
			
		}
		for (int l22 = 0; (l22 < 4); l22 = (l22 + 1)) {
			fRec37_perm[l22] = 0.0f;
			
		}
		for (int l23 = 0; (l23 < 4); l23 = (l23 + 1)) {
			fRec48_perm[l23] = 0.0f;
			
		}
		for (int l24 = 0; (l24 < 16384); l24 = (l24 + 1)) {
			fYec6[l24] = 0.0f;
			
		}
		fYec6_idx = 0;
		fYec6_idx_save = 0;
		for (int l25 = 0; (l25 < 4); l25 = (l25 + 1)) {
			fYec7_perm[l25] = 0.0f;
			
		}
		for (int l26 = 0; (l26 < 4); l26 = (l26 + 1)) {
			fRec35_perm[l26] = 0.0f;
			
		}
		for (int l27 = 0; (l27 < 4); l27 = (l27 + 1)) {
			fRec33_perm[l27] = 0.0f;
			
		}
		for (int l28 = 0; (l28 < 4); l28 = (l28 + 1)) {
			fRec50_perm[l28] = 0.0f;
			
		}
		for (int l29 = 0; (l29 < 16384); l29 = (l29 + 1)) {
			fYec8[l29] = 0.0f;
			
		}
		fYec8_idx = 0;
		fYec8_idx_save = 0;
		for (int l30 = 0; (l30 < 4); l30 = (l30 + 1)) {
			fYec9_perm[l30] = 0.0f;
			
		}
		for (int l31 = 0; (l31 < 4); l31 = (l31 + 1)) {
			fRec49_perm[l31] = 0.0f;
			
		}
		for (int l32 = 0; (l32 < 4); l32 = (l32 + 1)) {
			fRec34_perm[l32] = 0.0f;
			
		}
		for (int l33 = 0; (l33 < 4); l33 = (l33 + 1)) {
			fRec51_perm[l33] = 0.0f;
			
		}
		for (int l34 = 0; (l34 < 16384); l34 = (l34 + 1)) {
			fYec10[l34] = 0.0f;
			
		}
		fYec10_idx = 0;
		fYec10_idx_save = 0;
		for (int l35 = 0; (l35 < 4); l35 = (l35 + 1)) {
			fYec11_perm[l35] = 0.0f;
			
		}
		for (int l36 = 0; (l36 < 4); l36 = (l36 + 1)) {
			fRec32_perm[l36] = 0.0f;
			
		}
		for (int l37 = 0; (l37 < 4); l37 = (l37 + 1)) {
			fRec30_perm[l37] = 0.0f;
			
		}
		for (int l38 = 0; (l38 < 4); l38 = (l38 + 1)) {
			fRec53_perm[l38] = 0.0f;
			
		}
		for (int l39 = 0; (l39 < 16384); l39 = (l39 + 1)) {
			fYec12[l39] = 0.0f;
			
		}
		fYec12_idx = 0;
		fYec12_idx_save = 0;
		for (int l40 = 0; (l40 < 4); l40 = (l40 + 1)) {
			fYec13_perm[l40] = 0.0f;
			
		}
		for (int l41 = 0; (l41 < 4); l41 = (l41 + 1)) {
			fRec52_perm[l41] = 0.0f;
			
		}
		for (int l42 = 0; (l42 < 4); l42 = (l42 + 1)) {
			fRec31_perm[l42] = 0.0f;
			
		}
		for (int l43 = 0; (l43 < 4); l43 = (l43 + 1)) {
			fRec54_perm[l43] = 0.0f;
			
		}
		for (int l44 = 0; (l44 < 16384); l44 = (l44 + 1)) {
			fYec14[l44] = 0.0f;
			
		}
		fYec14_idx = 0;
		fYec14_idx_save = 0;
		for (int l45 = 0; (l45 < 4); l45 = (l45 + 1)) {
			fYec15_perm[l45] = 0.0f;
			
		}
		for (int l46 = 0; (l46 < 4); l46 = (l46 + 1)) {
			fRec29_perm[l46] = 0.0f;
			
		}
		for (int l47 = 0; (l47 < 4); l47 = (l47 + 1)) {
			fRec27_perm[l47] = 0.0f;
			
		}
		for (int l48 = 0; (l48 < 4); l48 = (l48 + 1)) {
			fRec56_perm[l48] = 0.0f;
			
		}
		for (int l49 = 0; (l49 < 16384); l49 = (l49 + 1)) {
			fYec16[l49] = 0.0f;
			
		}
		fYec16_idx = 0;
		fYec16_idx_save = 0;
		for (int l50 = 0; (l50 < 4); l50 = (l50 + 1)) {
			fYec17_perm[l50] = 0.0f;
			
		}
		for (int l51 = 0; (l51 < 4); l51 = (l51 + 1)) {
			fRec55_perm[l51] = 0.0f;
			
		}
		for (int l52 = 0; (l52 < 4); l52 = (l52 + 1)) {
			fRec28_perm[l52] = 0.0f;
			
		}
		for (int l53 = 0; (l53 < 4); l53 = (l53 + 1)) {
			fRec57_perm[l53] = 0.0f;
			
		}
		for (int l54 = 0; (l54 < 16384); l54 = (l54 + 1)) {
			fYec18[l54] = 0.0f;
			
		}
		fYec18_idx = 0;
		fYec18_idx_save = 0;
		for (int l55 = 0; (l55 < 4); l55 = (l55 + 1)) {
			fYec19_perm[l55] = 0.0f;
			
		}
		for (int l56 = 0; (l56 < 4); l56 = (l56 + 1)) {
			fRec26_perm[l56] = 0.0f;
			
		}
		for (int l57 = 0; (l57 < 4); l57 = (l57 + 1)) {
			fRec24_perm[l57] = 0.0f;
			
		}
		for (int l58 = 0; (l58 < 4); l58 = (l58 + 1)) {
			fRec59_perm[l58] = 0.0f;
			
		}
		for (int l59 = 0; (l59 < 16384); l59 = (l59 + 1)) {
			fYec20[l59] = 0.0f;
			
		}
		fYec20_idx = 0;
		fYec20_idx_save = 0;
		for (int l60 = 0; (l60 < 4); l60 = (l60 + 1)) {
			fYec21_perm[l60] = 0.0f;
			
		}
		for (int l61 = 0; (l61 < 4); l61 = (l61 + 1)) {
			fRec58_perm[l61] = 0.0f;
			
		}
		for (int l62 = 0; (l62 < 4); l62 = (l62 + 1)) {
			fRec25_perm[l62] = 0.0f;
			
		}
		for (int l63 = 0; (l63 < 4); l63 = (l63 + 1)) {
			fRec60_perm[l63] = 0.0f;
			
		}
		for (int l64 = 0; (l64 < 16384); l64 = (l64 + 1)) {
			fYec22[l64] = 0.0f;
			
		}
		fYec22_idx = 0;
		fYec22_idx_save = 0;
		for (int l65 = 0; (l65 < 4); l65 = (l65 + 1)) {
			fYec23_perm[l65] = 0.0f;
			
		}
		for (int l66 = 0; (l66 < 4); l66 = (l66 + 1)) {
			fRec23_perm[l66] = 0.0f;
			
		}
		for (int l67 = 0; (l67 < 4); l67 = (l67 + 1)) {
			fRec21_perm[l67] = 0.0f;
			
		}
		for (int l68 = 0; (l68 < 4); l68 = (l68 + 1)) {
			fRec62_perm[l68] = 0.0f;
			
		}
		for (int l69 = 0; (l69 < 16384); l69 = (l69 + 1)) {
			fYec24[l69] = 0.0f;
			
		}
		fYec24_idx = 0;
		fYec24_idx_save = 0;
		for (int l70 = 0; (l70 < 4); l70 = (l70 + 1)) {
			fYec25_perm[l70] = 0.0f;
			
		}
		for (int l71 = 0; (l71 < 4); l71 = (l71 + 1)) {
			fRec61_perm[l71] = 0.0f;
			
		}
		for (int l72 = 0; (l72 < 4); l72 = (l72 + 1)) {
			fRec22_perm[l72] = 0.0f;
			
		}
		for (int l73 = 0; (l73 < 4); l73 = (l73 + 1)) {
			fRec63_perm[l73] = 0.0f;
			
		}
		for (int l74 = 0; (l74 < 16384); l74 = (l74 + 1)) {
			fYec26[l74] = 0.0f;
			
		}
		fYec26_idx = 0;
		fYec26_idx_save = 0;
		for (int l75 = 0; (l75 < 4); l75 = (l75 + 1)) {
			fYec27_perm[l75] = 0.0f;
			
		}
		for (int l76 = 0; (l76 < 4); l76 = (l76 + 1)) {
			fRec20_perm[l76] = 0.0f;
			
		}
		for (int l77 = 0; (l77 < 4); l77 = (l77 + 1)) {
			fRec18_perm[l77] = 0.0f;
			
		}
		for (int l78 = 0; (l78 < 4); l78 = (l78 + 1)) {
			fRec65_perm[l78] = 0.0f;
			
		}
		for (int l79 = 0; (l79 < 16384); l79 = (l79 + 1)) {
			fYec28[l79] = 0.0f;
			
		}
		fYec28_idx = 0;
		fYec28_idx_save = 0;
		for (int l80 = 0; (l80 < 4); l80 = (l80 + 1)) {
			fYec29_perm[l80] = 0.0f;
			
		}
		for (int l81 = 0; (l81 < 4); l81 = (l81 + 1)) {
			fRec64_perm[l81] = 0.0f;
			
		}
		for (int l82 = 0; (l82 < 4); l82 = (l82 + 1)) {
			fRec19_perm[l82] = 0.0f;
			
		}
		for (int l83 = 0; (l83 < 4); l83 = (l83 + 1)) {
			fRec66_perm[l83] = 0.0f;
			
		}
		for (int l84 = 0; (l84 < 16384); l84 = (l84 + 1)) {
			fYec30[l84] = 0.0f;
			
		}
		fYec30_idx = 0;
		fYec30_idx_save = 0;
		for (int l85 = 0; (l85 < 4); l85 = (l85 + 1)) {
			fYec31_perm[l85] = 0.0f;
			
		}
		for (int l86 = 0; (l86 < 4); l86 = (l86 + 1)) {
			fRec17_perm[l86] = 0.0f;
			
		}
		for (int l87 = 0; (l87 < 4); l87 = (l87 + 1)) {
			fRec15_perm[l87] = 0.0f;
			
		}
		for (int l88 = 0; (l88 < 4); l88 = (l88 + 1)) {
			fRec68_perm[l88] = 0.0f;
			
		}
		for (int l89 = 0; (l89 < 16384); l89 = (l89 + 1)) {
			fYec32[l89] = 0.0f;
			
		}
		fYec32_idx = 0;
		fYec32_idx_save = 0;
		for (int l90 = 0; (l90 < 4); l90 = (l90 + 1)) {
			fYec33_perm[l90] = 0.0f;
			
		}
		for (int l91 = 0; (l91 < 4); l91 = (l91 + 1)) {
			fRec67_perm[l91] = 0.0f;
			
		}
		for (int l92 = 0; (l92 < 4); l92 = (l92 + 1)) {
			fRec16_perm[l92] = 0.0f;
			
		}
		for (int l93 = 0; (l93 < 4); l93 = (l93 + 1)) {
			fRec69_perm[l93] = 0.0f;
			
		}
		for (int l94 = 0; (l94 < 16384); l94 = (l94 + 1)) {
			fYec34[l94] = 0.0f;
			
		}
		fYec34_idx = 0;
		fYec34_idx_save = 0;
		for (int l95 = 0; (l95 < 4); l95 = (l95 + 1)) {
			fYec35_perm[l95] = 0.0f;
			
		}
		for (int l96 = 0; (l96 < 4); l96 = (l96 + 1)) {
			fRec14_perm[l96] = 0.0f;
			
		}
		for (int l97 = 0; (l97 < 4); l97 = (l97 + 1)) {
			fRec12_perm[l97] = 0.0f;
			
		}
		for (int l98 = 0; (l98 < 4); l98 = (l98 + 1)) {
			fRec71_perm[l98] = 0.0f;
			
		}
		for (int l99 = 0; (l99 < 16384); l99 = (l99 + 1)) {
			fYec36[l99] = 0.0f;
			
		}
		fYec36_idx = 0;
		fYec36_idx_save = 0;
		for (int l100 = 0; (l100 < 4); l100 = (l100 + 1)) {
			fYec37_perm[l100] = 0.0f;
			
		}
		for (int l101 = 0; (l101 < 4); l101 = (l101 + 1)) {
			fRec70_perm[l101] = 0.0f;
			
		}
		for (int l102 = 0; (l102 < 4); l102 = (l102 + 1)) {
			fRec13_perm[l102] = 0.0f;
			
		}
		for (int l103 = 0; (l103 < 4); l103 = (l103 + 1)) {
			fRec72_perm[l103] = 0.0f;
			
		}
		for (int l104 = 0; (l104 < 16384); l104 = (l104 + 1)) {
			fYec38[l104] = 0.0f;
			
		}
		fYec38_idx = 0;
		fYec38_idx_save = 0;
		for (int l105 = 0; (l105 < 4); l105 = (l105 + 1)) {
			fYec39_perm[l105] = 0.0f;
			
		}
		for (int l106 = 0; (l106 < 4); l106 = (l106 + 1)) {
			fRec11_perm[l106] = 0.0f;
			
		}
		for (int l107 = 0; (l107 < 4); l107 = (l107 + 1)) {
			fRec9_perm[l107] = 0.0f;
			
		}
		for (int l108 = 0; (l108 < 4); l108 = (l108 + 1)) {
			fRec74_perm[l108] = 0.0f;
			
		}
		for (int l109 = 0; (l109 < 16384); l109 = (l109 + 1)) {
			fYec40[l109] = 0.0f;
			
		}
		fYec40_idx = 0;
		fYec40_idx_save = 0;
		for (int l110 = 0; (l110 < 4); l110 = (l110 + 1)) {
			fYec41_perm[l110] = 0.0f;
			
		}
		for (int l111 = 0; (l111 < 4); l111 = (l111 + 1)) {
			fRec73_perm[l111] = 0.0f;
			
		}
		for (int l112 = 0; (l112 < 4); l112 = (l112 + 1)) {
			fRec10_perm[l112] = 0.0f;
			
		}
		for (int l113 = 0; (l113 < 4); l113 = (l113 + 1)) {
			fRec75_perm[l113] = 0.0f;
			
		}
		for (int l114 = 0; (l114 < 16384); l114 = (l114 + 1)) {
			fYec42[l114] = 0.0f;
			
		}
		fYec42_idx = 0;
		fYec42_idx_save = 0;
		for (int l115 = 0; (l115 < 4); l115 = (l115 + 1)) {
			fYec43_perm[l115] = 0.0f;
			
		}
		for (int l116 = 0; (l116 < 4); l116 = (l116 + 1)) {
			fRec8_perm[l116] = 0.0f;
			
		}
		for (int l117 = 0; (l117 < 4); l117 = (l117 + 1)) {
			fRec6_perm[l117] = 0.0f;
			
		}
		for (int l118 = 0; (l118 < 4); l118 = (l118 + 1)) {
			fRec77_perm[l118] = 0.0f;
			
		}
		for (int l119 = 0; (l119 < 16384); l119 = (l119 + 1)) {
			fYec44[l119] = 0.0f;
			
		}
		fYec44_idx = 0;
		fYec44_idx_save = 0;
		for (int l120 = 0; (l120 < 4); l120 = (l120 + 1)) {
			fYec45_perm[l120] = 0.0f;
			
		}
		for (int l121 = 0; (l121 < 4); l121 = (l121 + 1)) {
			fRec76_perm[l121] = 0.0f;
			
		}
		for (int l122 = 0; (l122 < 4); l122 = (l122 + 1)) {
			fRec7_perm[l122] = 0.0f;
			
		}
		for (int l123 = 0; (l123 < 4); l123 = (l123 + 1)) {
			fRec78_perm[l123] = 0.0f;
			
		}
		for (int l124 = 0; (l124 < 16384); l124 = (l124 + 1)) {
			fYec46[l124] = 0.0f;
			
		}
		fYec46_idx = 0;
		fYec46_idx_save = 0;
		for (int l125 = 0; (l125 < 4); l125 = (l125 + 1)) {
			fYec47_perm[l125] = 0.0f;
			
		}
		for (int l126 = 0; (l126 < 4); l126 = (l126 + 1)) {
			fRec5_perm[l126] = 0.0f;
			
		}
		for (int l127 = 0; (l127 < 4); l127 = (l127 + 1)) {
			fRec3_perm[l127] = 0.0f;
			
		}
		for (int l128 = 0; (l128 < 4); l128 = (l128 + 1)) {
			fRec80_perm[l128] = 0.0f;
			
		}
		for (int l129 = 0; (l129 < 16384); l129 = (l129 + 1)) {
			fYec48[l129] = 0.0f;
			
		}
		fYec48_idx = 0;
		fYec48_idx_save = 0;
		for (int l130 = 0; (l130 < 4); l130 = (l130 + 1)) {
			fYec49_perm[l130] = 0.0f;
			
		}
		for (int l131 = 0; (l131 < 4); l131 = (l131 + 1)) {
			fRec79_perm[l131] = 0.0f;
			
		}
		for (int l132 = 0; (l132 < 4); l132 = (l132 + 1)) {
			fRec4_perm[l132] = 0.0f;
			
		}
		for (int l133 = 0; (l133 < 4); l133 = (l133 + 1)) {
			fVec5_perm[l133] = 0.0f;
			
		}
		for (int l134 = 0; (l134 < 4); l134 = (l134 + 1)) {
			fRec2_perm[l134] = 0.0f;
			
		}
		for (int l135 = 0; (l135 < 1024); l135 = (l135 + 1)) {
			fRec0[l135] = 0.0f;
			
		}
		fRec0_idx = 0;
		fRec0_idx_save = 0;
		for (int l136 = 0; (l136 < 4); l136 = (l136 + 1)) {
			fRec81_perm[l136] = 0.0f;
			
		}
		for (int l137 = 0; (l137 < 1024); l137 = (l137 + 1)) {
			fRec1[l137] = 0.0f;
			
		}
		fRec1_idx = 0;
		fRec1_idx_save = 0;
		
	}
	
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	
	virtual void instanceInit(int samplingFreq) {
		instanceConstants(samplingFreq);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	
	virtual int getSampleRate() {
		return fSamplingFreq;
		
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("GreyholeRaw");
		ui_interface->addHorizontalSlider("damping", &fHslider6, 0.0f, 0.0f, 0.99000001f, 0.00100000005f);
		ui_interface->addHorizontalSlider("delayTime", &fHslider0, 0.200000003f, 0.00100000005f, 1.45000005f, 9.99999975e-05f);
		ui_interface->addHorizontalSlider("diffusion", &fHslider3, 0.5f, 0.0f, 0.99000001f, 9.99999975e-05f);
		ui_interface->addHorizontalSlider("feedback", &fHslider4, 0.899999976f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("modDepth", &fHslider5, 0.100000001f, 0.0f, 1.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("modFreq", &fHslider1, 2.0f, 0.0f, 10.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("size", &fHslider2, 1.0f, 0.5f, 3.0f, 9.99999975e-05f);
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0_ptr = inputs[0];
		FAUSTFLOAT* input1_ptr = inputs[1];
		FAUSTFLOAT* output0_ptr = outputs[0];
		FAUSTFLOAT* output1_ptr = outputs[1];
		float fSlow0 = std::floor(std::min<float>(65533.0f, (fConst0 * float(fHslider0))));
		float fZec0[8];
		float fRec39_tmp[12];
		float* fRec39 = &fRec39_tmp[4];
		float fRec40_tmp[12];
		float* fRec40 = &fRec40_tmp[4];
		float fRec41_tmp[12];
		float* fRec41 = &fRec41_tmp[4];
		float fRec42_tmp[12];
		float* fRec42 = &fRec42_tmp[4];
		float fSlow1 = float(fHslider1);
		float fVec0_tmp[12];
		float* fVec0 = &fVec0_tmp[4];
		float fZec1[8];
		float fZec2[8];
		float fZec3[8];
		float fRec43_tmp[12];
		float* fRec43 = &fRec43_tmp[4];
		int iVec1_tmp[12];
		int* iVec1 = &iVec1_tmp[4];
		int iZec4[8];
		float fRec44_tmp[12];
		float* fRec44 = &fRec44_tmp[4];
		float fSlow2 = float(fHslider2);
		float fSlow3 = (49.0f * fSlow2);
		int iSlow4 = int(primes(int(fSlow3)));
		float fSlow5 = (9.99999975e-05f * float(iSlow4));
		float fRec45_tmp[12];
		float* fRec45 = &fRec45_tmp[4];
		float fSlow6 = float(fHslider3);
		float fVec2_tmp[12];
		float* fVec2 = &fVec2_tmp[4];
		float fZec5[8];
		float fZec6[8];
		float fSlow7 = float(fHslider4);
		float fVec3_tmp[12];
		float* fVec3 = &fVec3_tmp[4];
		float fZec7[8];
		float fZec8[8];
		float fSlow8 = float(fHslider5);
		float fVec4_tmp[12];
		float* fVec4 = &fVec4_tmp[4];
		float fZec9[8];
		float fZec10[8];
		float fZec11[8];
		int iZec12[8];
		float fZec13[8];
		float fZec14[8];
		float fZec15[8];
		float fZec16[8];
		float fZec17[8];
		float fZec18[8];
		int iZec19[8];
		int iZec20[8];
		float fZec21[8];
		float fZec22[8];
		float fZec23[8];
		float fZec24[8];
		float fZec25[8];
		float fZec26[8];
		float fYec2_tmp[12];
		float* fYec2 = &fYec2_tmp[4];
		float fZec27[8];
		float fRec38_tmp[12];
		float* fRec38 = &fRec38_tmp[4];
		float fRec36_tmp[12];
		float* fRec36 = &fRec36_tmp[4];
		int iSlow9 = int(primes(int((fSlow3 + 10.0f))));
		float fSlow10 = (9.99999975e-05f * float(iSlow9));
		float fRec47_tmp[12];
		float* fRec47 = &fRec47_tmp[4];
		float fZec28[8];
		float fZec29[8];
		int iZec30[8];
		float fZec31[8];
		float fZec32[8];
		float fZec33[8];
		float fZec34[8];
		float fZec35[8];
		float fZec36[8];
		float fZec37[8];
		float fZec38[8];
		float fZec39[8];
		float fZec40[8];
		float fZec41[8];
		float fYec5_tmp[12];
		float* fYec5 = &fYec5_tmp[4];
		float fZec42[8];
		float fRec46_tmp[12];
		float* fRec46 = &fRec46_tmp[4];
		float fRec37_tmp[12];
		float* fRec37 = &fRec37_tmp[4];
		float fSlow11 = (36.0f * fSlow2);
		int iSlow12 = int(primes(int(fSlow11)));
		float fSlow13 = (0.00100000005f * float(iSlow12));
		float fRec48_tmp[12];
		float* fRec48 = &fRec48_tmp[4];
		float fZec43[8];
		float fYec7_tmp[12];
		float* fYec7 = &fYec7_tmp[4];
		float fZec44[8];
		float fRec35_tmp[12];
		float* fRec35 = &fRec35_tmp[4];
		float fRec33_tmp[12];
		float* fRec33 = &fRec33_tmp[4];
		int iSlow14 = int(primes(int((fSlow11 + 10.0f))));
		float fSlow15 = (0.00100000005f * float(iSlow14));
		float fRec50_tmp[12];
		float* fRec50 = &fRec50_tmp[4];
		float fZec45[8];
		float fYec9_tmp[12];
		float* fYec9 = &fYec9_tmp[4];
		float fZec46[8];
		float fRec49_tmp[12];
		float* fRec49 = &fRec49_tmp[4];
		float fRec34_tmp[12];
		float* fRec34 = &fRec34_tmp[4];
		float fSlow16 = (23.0f * fSlow2);
		int iSlow17 = int(primes(int(fSlow16)));
		float fSlow18 = (0.00100000005f * float(iSlow17));
		float fRec51_tmp[12];
		float* fRec51 = &fRec51_tmp[4];
		float fZec47[8];
		float fYec11_tmp[12];
		float* fYec11 = &fYec11_tmp[4];
		float fZec48[8];
		float fRec32_tmp[12];
		float* fRec32 = &fRec32_tmp[4];
		float fRec30_tmp[12];
		float* fRec30 = &fRec30_tmp[4];
		int iSlow19 = int(primes(int((fSlow16 + 10.0f))));
		float fSlow20 = (0.00100000005f * float(iSlow19));
		float fRec53_tmp[12];
		float* fRec53 = &fRec53_tmp[4];
		float fZec49[8];
		float fYec13_tmp[12];
		float* fYec13 = &fYec13_tmp[4];
		float fZec50[8];
		float fRec52_tmp[12];
		float* fRec52 = &fRec52_tmp[4];
		float fRec31_tmp[12];
		float* fRec31 = &fRec31_tmp[4];
		int iSlow21 = int(primes(int((10.0f * fSlow2))));
		float fSlow22 = (0.00100000005f * float(iSlow21));
		float fRec54_tmp[12];
		float* fRec54 = &fRec54_tmp[4];
		float fZec51[8];
		float fYec15_tmp[12];
		float* fYec15 = &fYec15_tmp[4];
		float fZec52[8];
		float fRec29_tmp[12];
		float* fRec29 = &fRec29_tmp[4];
		float fRec27_tmp[12];
		float* fRec27 = &fRec27_tmp[4];
		int iSlow23 = int(primes(int((10.0f * (fSlow2 + 1.0f)))));
		float fSlow24 = (0.00100000005f * float(iSlow23));
		float fRec56_tmp[12];
		float* fRec56 = &fRec56_tmp[4];
		float fZec53[8];
		float fYec17_tmp[12];
		float* fYec17 = &fYec17_tmp[4];
		float fZec54[8];
		float fRec55_tmp[12];
		float* fRec55 = &fRec55_tmp[4];
		float fRec28_tmp[12];
		float* fRec28 = &fRec28_tmp[4];
		float fSlow25 = (68.0f * fSlow2);
		int iSlow26 = int(primes(int(fSlow25)));
		float fSlow27 = (9.99999975e-05f * float(iSlow26));
		float fRec57_tmp[12];
		float* fRec57 = &fRec57_tmp[4];
		float fZec55[8];
		float fZec56[8];
		float fZec57[8];
		float fZec58[8];
		float fZec59[8];
		float fZec60[8];
		float fZec61[8];
		float fZec62[8];
		float fYec19_tmp[12];
		float* fYec19 = &fYec19_tmp[4];
		float fZec63[8];
		float fRec26_tmp[12];
		float* fRec26 = &fRec26_tmp[4];
		float fRec24_tmp[12];
		float* fRec24 = &fRec24_tmp[4];
		int iSlow28 = int(primes(int((fSlow25 + 10.0f))));
		float fSlow29 = (9.99999975e-05f * float(iSlow28));
		float fRec59_tmp[12];
		float* fRec59 = &fRec59_tmp[4];
		float fZec64[8];
		float fZec65[8];
		float fZec66[8];
		float fZec67[8];
		float fZec68[8];
		float fYec21_tmp[12];
		float* fYec21 = &fYec21_tmp[4];
		float fZec69[8];
		float fRec58_tmp[12];
		float* fRec58 = &fRec58_tmp[4];
		float fRec25_tmp[12];
		float* fRec25 = &fRec25_tmp[4];
		float fSlow30 = (55.0f * fSlow2);
		int iSlow31 = int(primes(int(fSlow30)));
		float fSlow32 = (0.00100000005f * float(iSlow31));
		float fRec60_tmp[12];
		float* fRec60 = &fRec60_tmp[4];
		float fZec70[8];
		float fYec23_tmp[12];
		float* fYec23 = &fYec23_tmp[4];
		float fZec71[8];
		float fRec23_tmp[12];
		float* fRec23 = &fRec23_tmp[4];
		float fRec21_tmp[12];
		float* fRec21 = &fRec21_tmp[4];
		int iSlow33 = int(primes(int((fSlow30 + 10.0f))));
		float fSlow34 = (0.00100000005f * float(iSlow33));
		float fRec62_tmp[12];
		float* fRec62 = &fRec62_tmp[4];
		float fZec72[8];
		float fYec25_tmp[12];
		float* fYec25 = &fYec25_tmp[4];
		float fZec73[8];
		float fRec61_tmp[12];
		float* fRec61 = &fRec61_tmp[4];
		float fRec22_tmp[12];
		float* fRec22 = &fRec22_tmp[4];
		float fSlow35 = (42.0f * fSlow2);
		int iSlow36 = int(primes(int(fSlow35)));
		float fSlow37 = (0.00100000005f * float(iSlow36));
		float fRec63_tmp[12];
		float* fRec63 = &fRec63_tmp[4];
		float fZec74[8];
		float fYec27_tmp[12];
		float* fYec27 = &fYec27_tmp[4];
		float fZec75[8];
		float fRec20_tmp[12];
		float* fRec20 = &fRec20_tmp[4];
		float fRec18_tmp[12];
		float* fRec18 = &fRec18_tmp[4];
		int iSlow38 = int(primes(int((fSlow35 + 10.0f))));
		float fSlow39 = (0.00100000005f * float(iSlow38));
		float fRec65_tmp[12];
		float* fRec65 = &fRec65_tmp[4];
		float fZec76[8];
		float fYec29_tmp[12];
		float* fYec29 = &fYec29_tmp[4];
		float fZec77[8];
		float fRec64_tmp[12];
		float* fRec64 = &fRec64_tmp[4];
		float fRec19_tmp[12];
		float* fRec19 = &fRec19_tmp[4];
		float fSlow40 = (29.0f * fSlow2);
		int iSlow41 = int(primes(int(fSlow40)));
		float fSlow42 = (0.00100000005f * float(iSlow41));
		float fRec66_tmp[12];
		float* fRec66 = &fRec66_tmp[4];
		float fZec78[8];
		float fYec31_tmp[12];
		float* fYec31 = &fYec31_tmp[4];
		float fZec79[8];
		float fRec17_tmp[12];
		float* fRec17 = &fRec17_tmp[4];
		float fRec15_tmp[12];
		float* fRec15 = &fRec15_tmp[4];
		int iSlow43 = int(primes(int((fSlow40 + 10.0f))));
		float fSlow44 = (0.00100000005f * float(iSlow43));
		float fRec68_tmp[12];
		float* fRec68 = &fRec68_tmp[4];
		float fZec80[8];
		float fYec33_tmp[12];
		float* fYec33 = &fYec33_tmp[4];
		float fZec81[8];
		float fRec67_tmp[12];
		float* fRec67 = &fRec67_tmp[4];
		float fRec16_tmp[12];
		float* fRec16 = &fRec16_tmp[4];
		float fSlow45 = (87.0f * fSlow2);
		int iSlow46 = int(primes(int(fSlow45)));
		float fSlow47 = (9.99999975e-05f * float(iSlow46));
		float fRec69_tmp[12];
		float* fRec69 = &fRec69_tmp[4];
		float fZec82[8];
		float fZec83[8];
		float fZec84[8];
		float fZec85[8];
		float fZec86[8];
		float fYec35_tmp[12];
		float* fYec35 = &fYec35_tmp[4];
		float fZec87[8];
		float fRec14_tmp[12];
		float* fRec14 = &fRec14_tmp[4];
		float fRec12_tmp[12];
		float* fRec12 = &fRec12_tmp[4];
		int iSlow48 = int(primes(int((fSlow45 + 10.0f))));
		float fSlow49 = (9.99999975e-05f * float(iSlow48));
		float fRec71_tmp[12];
		float* fRec71 = &fRec71_tmp[4];
		float fZec88[8];
		float fZec89[8];
		float fZec90[8];
		float fZec91[8];
		float fZec92[8];
		float fYec37_tmp[12];
		float* fYec37 = &fYec37_tmp[4];
		float fZec93[8];
		float fRec70_tmp[12];
		float* fRec70 = &fRec70_tmp[4];
		float fRec13_tmp[12];
		float* fRec13 = &fRec13_tmp[4];
		float fSlow50 = (74.0f * fSlow2);
		int iSlow51 = int(primes(int(fSlow50)));
		float fSlow52 = (0.00100000005f * float(iSlow51));
		float fRec72_tmp[12];
		float* fRec72 = &fRec72_tmp[4];
		float fZec94[8];
		float fYec39_tmp[12];
		float* fYec39 = &fYec39_tmp[4];
		float fZec95[8];
		float fRec11_tmp[12];
		float* fRec11 = &fRec11_tmp[4];
		float fRec9_tmp[12];
		float* fRec9 = &fRec9_tmp[4];
		int iSlow53 = int(primes(int((fSlow50 + 10.0f))));
		float fSlow54 = (0.00100000005f * float(iSlow53));
		float fRec74_tmp[12];
		float* fRec74 = &fRec74_tmp[4];
		float fZec96[8];
		float fYec41_tmp[12];
		float* fYec41 = &fYec41_tmp[4];
		float fZec97[8];
		float fRec73_tmp[12];
		float* fRec73 = &fRec73_tmp[4];
		float fRec10_tmp[12];
		float* fRec10 = &fRec10_tmp[4];
		float fSlow55 = (61.0f * fSlow2);
		int iSlow56 = int(primes(int(fSlow55)));
		float fSlow57 = (0.00100000005f * float(iSlow56));
		float fRec75_tmp[12];
		float* fRec75 = &fRec75_tmp[4];
		float fZec98[8];
		float fYec43_tmp[12];
		float* fYec43 = &fYec43_tmp[4];
		float fZec99[8];
		float fRec8_tmp[12];
		float* fRec8 = &fRec8_tmp[4];
		float fRec6_tmp[12];
		float* fRec6 = &fRec6_tmp[4];
		int iSlow58 = int(primes(int((fSlow55 + 10.0f))));
		float fSlow59 = (0.00100000005f * float(iSlow58));
		float fRec77_tmp[12];
		float* fRec77 = &fRec77_tmp[4];
		float fZec100[8];
		float fYec45_tmp[12];
		float* fYec45 = &fYec45_tmp[4];
		float fZec101[8];
		float fRec76_tmp[12];
		float* fRec76 = &fRec76_tmp[4];
		float fRec7_tmp[12];
		float* fRec7 = &fRec7_tmp[4];
		float fSlow60 = (48.0f * fSlow2);
		int iSlow61 = int(primes(int(fSlow60)));
		float fSlow62 = (0.00100000005f * float(iSlow61));
		float fRec78_tmp[12];
		float* fRec78 = &fRec78_tmp[4];
		float fZec102[8];
		float fYec47_tmp[12];
		float* fYec47 = &fYec47_tmp[4];
		float fZec103[8];
		float fRec5_tmp[12];
		float* fRec5 = &fRec5_tmp[4];
		float fRec3_tmp[12];
		float* fRec3 = &fRec3_tmp[4];
		int iSlow63 = int(primes(int((fSlow60 + 10.0f))));
		float fSlow64 = (0.00100000005f * float(iSlow63));
		float fRec80_tmp[12];
		float* fRec80 = &fRec80_tmp[4];
		float fZec104[8];
		float fYec49_tmp[12];
		float* fYec49 = &fYec49_tmp[4];
		float fZec105[8];
		float fRec79_tmp[12];
		float* fRec79 = &fRec79_tmp[4];
		float fRec4_tmp[12];
		float* fRec4 = &fRec4_tmp[4];
		float fSlow65 = float(fHslider6);
		float fVec5_tmp[12];
		float* fVec5 = &fVec5_tmp[4];
		float fZec106[8];
		float fZec107[8];
		float fRec2_tmp[12];
		float* fRec2 = &fRec2_tmp[4];
		float fRec81_tmp[12];
		float* fRec81 = &fRec81_tmp[4];
		int vindex = 0;
		/* Main loop */
		for (vindex = 0; (vindex <= (count - 8)); vindex = (vindex + 8)) {
			FAUSTFLOAT* input0 = &input0_ptr[vindex];
			FAUSTFLOAT* input1 = &input1_ptr[vindex];
			FAUSTFLOAT* output0 = &output0_ptr[vindex];
			FAUSTFLOAT* output1 = &output1_ptr[vindex];
			int vsize = 8;
			/* Vectorizable loop 0 */
			/* Pre code */
			for (int j8 = 0; (j8 < 4); j8 = (j8 + 1)) {
				fVec0_tmp[j8] = fVec0_perm[j8];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fVec0[i] = fSlow1;
				
			}
			/* Post code */
			for (int j9 = 0; (j9 < 4); j9 = (j9 + 1)) {
				fVec0_perm[j9] = fVec0_tmp[(vsize + j9)];
				
			}
			/* Vectorizable loop 1 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec1[i] = (fConst1 * (fSlow1 + fVec0[(i - 1)]));
				
			}
			/* Vectorizable loop 2 */
			/* Pre code */
			for (int j12 = 0; (j12 < 4); j12 = (j12 + 1)) {
				iVec1_tmp[j12] = iVec1_perm[j12];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iVec1[i] = 1;
				
			}
			/* Post code */
			for (int j13 = 0; (j13 < 4); j13 = (j13 + 1)) {
				iVec1_perm[j13] = iVec1_tmp[(vsize + j13)];
				
			}
			/* Vectorizable loop 3 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec2[i] = std::sin(fZec1[i]);
				
			}
			/* Vectorizable loop 4 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec3[i] = std::cos(fZec1[i]);
				
			}
			/* Vectorizable loop 5 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec4[i] = (1 - iVec1[(i - 1)]);
				
			}
			/* Vectorizable loop 6 */
			/* Pre code */
			for (int j22 = 0; (j22 < 4); j22 = (j22 + 1)) {
				fVec4_tmp[j22] = fVec4_perm[j22];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fVec4[i] = fSlow8;
				
			}
			/* Post code */
			for (int j23 = 0; (j23 < 4); j23 = (j23 + 1)) {
				fVec4_perm[j23] = fVec4_tmp[(vsize + j23)];
				
			}
			/* Recursive loop 7 */
			/* Pre code */
			for (int j10 = 0; (j10 < 4); j10 = (j10 + 1)) {
				fRec43_tmp[j10] = fRec43_perm[j10];
				
			}
			for (int j14 = 0; (j14 < 4); j14 = (j14 + 1)) {
				fRec44_tmp[j14] = fRec44_perm[j14];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec43[i] = ((fRec44[(i - 1)] * fZec2[i]) + (fRec43[(i - 1)] * fZec3[i]));
				fRec44[i] = ((float(iZec4[i]) + (fRec44[(i - 1)] * fZec3[i])) - (fRec43[(i - 1)] * fZec2[i]));
				
			}
			/* Post code */
			for (int j11 = 0; (j11 < 4); j11 = (j11 + 1)) {
				fRec43_perm[j11] = fRec43_tmp[(vsize + j11)];
				
			}
			for (int j15 = 0; (j15 < 4); j15 = (j15 + 1)) {
				fRec44_perm[j15] = fRec44_tmp[(vsize + j15)];
				
			}
			/* Vectorizable loop 8 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec9[i] = (fSlow8 + fVec4[(i - 1)]);
				
			}
			/* Vectorizable loop 9 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec10[i] = (fConst2 * ((fRec43[i] + 1.0f) * fZec9[i]));
				
			}
			/* Vectorizable loop 10 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec28[i] = (fConst2 * ((fRec44[i] + 1.0f) * fZec9[i]));
				
			}
			/* Vectorizable loop 11 */
			/* Pre code */
			for (int j18 = 0; (j18 < 4); j18 = (j18 + 1)) {
				fVec2_tmp[j18] = fVec2_perm[j18];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fVec2[i] = fSlow6;
				
			}
			/* Post code */
			for (int j19 = 0; (j19 < 4); j19 = (j19 + 1)) {
				fVec2_perm[j19] = fVec2_tmp[(vsize + j19)];
				
			}
			/* Vectorizable loop 12 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec11[i] = (fZec10[i] + 8.50000477f);
				
			}
			/* Vectorizable loop 13 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec29[i] = (fZec28[i] + 8.50000477f);
				
			}
			/* Recursive loop 14 */
			/* Pre code */
			for (int j16 = 0; (j16 < 4); j16 = (j16 + 1)) {
				fRec45_tmp[j16] = fRec45_perm[j16];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec45[i] = (fSlow5 + (0.999899983f * (fRec45[(i - 1)] + float((iSlow4 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j17 = 0; (j17 < 4); j17 = (j17 + 1)) {
				fRec45_perm[j17] = fRec45_tmp[(vsize + j17)];
				
			}
			/* Vectorizable loop 15 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec5[i] = (0.5f * (fSlow6 + fVec2[(i - 1)]));
				
			}
			/* Vectorizable loop 16 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec13[i] = std::floor(fZec11[i]);
				
			}
			/* Recursive loop 17 */
			/* Pre code */
			for (int j30 = 0; (j30 < 4); j30 = (j30 + 1)) {
				fRec47_tmp[j30] = fRec47_perm[j30];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec47[i] = (fSlow10 + (0.999899983f * (fRec47[(i - 1)] + float((iSlow9 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j31 = 0; (j31 < 4); j31 = (j31 + 1)) {
				fRec47_perm[j31] = fRec47_tmp[(vsize + j31)];
				
			}
			/* Vectorizable loop 18 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec31[i] = std::floor(fZec29[i]);
				
			}
			/* Recursive loop 19 */
			/* Pre code */
			for (int j38 = 0; (j38 < 4); j38 = (j38 + 1)) {
				fRec48_tmp[j38] = fRec48_perm[j38];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec48[i] = (fSlow13 + (0.999000013f * (fRec48[(i - 1)] + float((iSlow12 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j39 = 0; (j39 < 4); j39 = (j39 + 1)) {
				fRec48_perm[j39] = fRec48_tmp[(vsize + j39)];
				
			}
			/* Recursive loop 20 */
			/* Pre code */
			for (int j46 = 0; (j46 < 4); j46 = (j46 + 1)) {
				fRec50_tmp[j46] = fRec50_perm[j46];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec50[i] = (fSlow15 + (0.999000013f * (fRec50[(i - 1)] + float((iSlow14 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j47 = 0; (j47 < 4); j47 = (j47 + 1)) {
				fRec50_perm[j47] = fRec50_tmp[(vsize + j47)];
				
			}
			/* Recursive loop 21 */
			/* Pre code */
			for (int j54 = 0; (j54 < 4); j54 = (j54 + 1)) {
				fRec51_tmp[j54] = fRec51_perm[j54];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec51[i] = (fSlow18 + (0.999000013f * (fRec51[(i - 1)] + float((iSlow17 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j55 = 0; (j55 < 4); j55 = (j55 + 1)) {
				fRec51_perm[j55] = fRec51_tmp[(vsize + j55)];
				
			}
			/* Recursive loop 22 */
			/* Pre code */
			for (int j62 = 0; (j62 < 4); j62 = (j62 + 1)) {
				fRec53_tmp[j62] = fRec53_perm[j62];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec53[i] = (fSlow20 + (0.999000013f * (fRec53[(i - 1)] + float((iSlow19 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j63 = 0; (j63 < 4); j63 = (j63 + 1)) {
				fRec53_perm[j63] = fRec53_tmp[(vsize + j63)];
				
			}
			/* Recursive loop 23 */
			/* Pre code */
			for (int j70 = 0; (j70 < 4); j70 = (j70 + 1)) {
				fRec54_tmp[j70] = fRec54_perm[j70];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec54[i] = (fSlow22 + (0.999000013f * (fRec54[(i - 1)] + float((iSlow21 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j71 = 0; (j71 < 4); j71 = (j71 + 1)) {
				fRec54_perm[j71] = fRec54_tmp[(vsize + j71)];
				
			}
			/* Recursive loop 24 */
			/* Pre code */
			for (int j78 = 0; (j78 < 4); j78 = (j78 + 1)) {
				fRec56_tmp[j78] = fRec56_perm[j78];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec56[i] = (fSlow24 + (0.999000013f * (fRec56[(i - 1)] + float((iSlow23 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j79 = 0; (j79 < 4); j79 = (j79 + 1)) {
				fRec56_perm[j79] = fRec56_tmp[(vsize + j79)];
				
			}
			/* Recursive loop 25 */
			/* Pre code */
			for (int j86 = 0; (j86 < 4); j86 = (j86 + 1)) {
				fRec57_tmp[j86] = fRec57_perm[j86];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec57[i] = (fSlow27 + (0.999899983f * (fRec57[(i - 1)] + float((iSlow26 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j87 = 0; (j87 < 4); j87 = (j87 + 1)) {
				fRec57_perm[j87] = fRec57_tmp[(vsize + j87)];
				
			}
			/* Recursive loop 26 */
			/* Pre code */
			for (int j94 = 0; (j94 < 4); j94 = (j94 + 1)) {
				fRec59_tmp[j94] = fRec59_perm[j94];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec59[i] = (fSlow29 + (0.999899983f * (fRec59[(i - 1)] + float((iSlow28 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j95 = 0; (j95 < 4); j95 = (j95 + 1)) {
				fRec59_perm[j95] = fRec59_tmp[(vsize + j95)];
				
			}
			/* Recursive loop 27 */
			/* Pre code */
			for (int j102 = 0; (j102 < 4); j102 = (j102 + 1)) {
				fRec60_tmp[j102] = fRec60_perm[j102];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec60[i] = (fSlow32 + (0.999000013f * (fRec60[(i - 1)] + float((iSlow31 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j103 = 0; (j103 < 4); j103 = (j103 + 1)) {
				fRec60_perm[j103] = fRec60_tmp[(vsize + j103)];
				
			}
			/* Recursive loop 28 */
			/* Pre code */
			for (int j110 = 0; (j110 < 4); j110 = (j110 + 1)) {
				fRec62_tmp[j110] = fRec62_perm[j110];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec62[i] = (fSlow34 + (0.999000013f * (fRec62[(i - 1)] + float((iSlow33 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j111 = 0; (j111 < 4); j111 = (j111 + 1)) {
				fRec62_perm[j111] = fRec62_tmp[(vsize + j111)];
				
			}
			/* Recursive loop 29 */
			/* Pre code */
			for (int j118 = 0; (j118 < 4); j118 = (j118 + 1)) {
				fRec63_tmp[j118] = fRec63_perm[j118];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec63[i] = (fSlow37 + (0.999000013f * (fRec63[(i - 1)] + float((iSlow36 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j119 = 0; (j119 < 4); j119 = (j119 + 1)) {
				fRec63_perm[j119] = fRec63_tmp[(vsize + j119)];
				
			}
			/* Recursive loop 30 */
			/* Pre code */
			for (int j126 = 0; (j126 < 4); j126 = (j126 + 1)) {
				fRec65_tmp[j126] = fRec65_perm[j126];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec65[i] = (fSlow39 + (0.999000013f * (fRec65[(i - 1)] + float((iSlow38 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j127 = 0; (j127 < 4); j127 = (j127 + 1)) {
				fRec65_perm[j127] = fRec65_tmp[(vsize + j127)];
				
			}
			/* Recursive loop 31 */
			/* Pre code */
			for (int j134 = 0; (j134 < 4); j134 = (j134 + 1)) {
				fRec66_tmp[j134] = fRec66_perm[j134];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec66[i] = (fSlow42 + (0.999000013f * (fRec66[(i - 1)] + float((iSlow41 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j135 = 0; (j135 < 4); j135 = (j135 + 1)) {
				fRec66_perm[j135] = fRec66_tmp[(vsize + j135)];
				
			}
			/* Recursive loop 32 */
			/* Pre code */
			for (int j142 = 0; (j142 < 4); j142 = (j142 + 1)) {
				fRec68_tmp[j142] = fRec68_perm[j142];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec68[i] = (fSlow44 + (0.999000013f * (fRec68[(i - 1)] + float((iSlow43 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j143 = 0; (j143 < 4); j143 = (j143 + 1)) {
				fRec68_perm[j143] = fRec68_tmp[(vsize + j143)];
				
			}
			/* Recursive loop 33 */
			/* Pre code */
			for (int j150 = 0; (j150 < 4); j150 = (j150 + 1)) {
				fRec69_tmp[j150] = fRec69_perm[j150];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec69[i] = (fSlow47 + (0.999899983f * (fRec69[(i - 1)] + float((iSlow46 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j151 = 0; (j151 < 4); j151 = (j151 + 1)) {
				fRec69_perm[j151] = fRec69_tmp[(vsize + j151)];
				
			}
			/* Recursive loop 34 */
			/* Pre code */
			for (int j158 = 0; (j158 < 4); j158 = (j158 + 1)) {
				fRec71_tmp[j158] = fRec71_perm[j158];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec71[i] = (fSlow49 + (0.999899983f * (fRec71[(i - 1)] + float((iSlow48 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j159 = 0; (j159 < 4); j159 = (j159 + 1)) {
				fRec71_perm[j159] = fRec71_tmp[(vsize + j159)];
				
			}
			/* Recursive loop 35 */
			/* Pre code */
			for (int j166 = 0; (j166 < 4); j166 = (j166 + 1)) {
				fRec72_tmp[j166] = fRec72_perm[j166];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec72[i] = (fSlow52 + (0.999000013f * (fRec72[(i - 1)] + float((iSlow51 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j167 = 0; (j167 < 4); j167 = (j167 + 1)) {
				fRec72_perm[j167] = fRec72_tmp[(vsize + j167)];
				
			}
			/* Recursive loop 36 */
			/* Pre code */
			for (int j174 = 0; (j174 < 4); j174 = (j174 + 1)) {
				fRec74_tmp[j174] = fRec74_perm[j174];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec74[i] = (fSlow54 + (0.999000013f * (fRec74[(i - 1)] + float((iSlow53 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j175 = 0; (j175 < 4); j175 = (j175 + 1)) {
				fRec74_perm[j175] = fRec74_tmp[(vsize + j175)];
				
			}
			/* Recursive loop 37 */
			/* Pre code */
			for (int j182 = 0; (j182 < 4); j182 = (j182 + 1)) {
				fRec75_tmp[j182] = fRec75_perm[j182];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec75[i] = (fSlow57 + (0.999000013f * (fRec75[(i - 1)] + float((iSlow56 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j183 = 0; (j183 < 4); j183 = (j183 + 1)) {
				fRec75_perm[j183] = fRec75_tmp[(vsize + j183)];
				
			}
			/* Recursive loop 38 */
			/* Pre code */
			for (int j190 = 0; (j190 < 4); j190 = (j190 + 1)) {
				fRec77_tmp[j190] = fRec77_perm[j190];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec77[i] = (fSlow59 + (0.999000013f * (fRec77[(i - 1)] + float((iSlow58 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j191 = 0; (j191 < 4); j191 = (j191 + 1)) {
				fRec77_perm[j191] = fRec77_tmp[(vsize + j191)];
				
			}
			/* Recursive loop 39 */
			/* Pre code */
			for (int j198 = 0; (j198 < 4); j198 = (j198 + 1)) {
				fRec78_tmp[j198] = fRec78_perm[j198];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec78[i] = (fSlow62 + (0.999000013f * (fRec78[(i - 1)] + float((iSlow61 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j199 = 0; (j199 < 4); j199 = (j199 + 1)) {
				fRec78_perm[j199] = fRec78_tmp[(vsize + j199)];
				
			}
			/* Recursive loop 40 */
			/* Pre code */
			for (int j206 = 0; (j206 < 4); j206 = (j206 + 1)) {
				fRec80_tmp[j206] = fRec80_perm[j206];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec80[i] = (fSlow64 + (0.999000013f * (fRec80[(i - 1)] + float((iSlow63 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j207 = 0; (j207 < 4); j207 = (j207 + 1)) {
				fRec80_perm[j207] = fRec80_tmp[(vsize + j207)];
				
			}
			/* Vectorizable loop 41 */
			/* Pre code */
			for (int j214 = 0; (j214 < 4); j214 = (j214 + 1)) {
				fVec5_tmp[j214] = fVec5_perm[j214];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fVec5[i] = fSlow65;
				
			}
			/* Post code */
			for (int j215 = 0; (j215 < 4); j215 = (j215 + 1)) {
				fVec5_perm[j215] = fVec5_tmp[(vsize + j215)];
				
			}
			/* Recursive loop 42 */
			/* Pre code */
			for (int j0 = 0; (j0 < 4); j0 = (j0 + 1)) {
				fRec39_tmp[j0] = fRec39_perm[j0];
				
			}
			for (int j2 = 0; (j2 < 4); j2 = (j2 + 1)) {
				fRec40_tmp[j2] = fRec40_perm[j2];
				
			}
			for (int j4 = 0; (j4 < 4); j4 = (j4 + 1)) {
				fRec41_tmp[j4] = fRec41_perm[j4];
				
			}
			for (int j6 = 0; (j6 < 4); j6 = (j6 + 1)) {
				fRec42_tmp[j6] = fRec42_perm[j6];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec0[i] = ((fRec39[(i - 1)] != 0.0f)?(((fRec40[(i - 1)] > 0.0f) & (fRec40[(i - 1)] < 1.0f))?fRec39[(i - 1)]:0.0f):(((fRec40[(i - 1)] == 0.0f) & (fSlow0 != fRec41[(i - 1)]))?4.53514731e-05f:(((fRec40[(i - 1)] == 1.0f) & (fSlow0 != fRec42[(i - 1)]))?-4.53514731e-05f:0.0f)));
				fRec39[i] = fZec0[i];
				fRec40[i] = std::max<float>(0.0f, std::min<float>(1.0f, (fRec40[(i - 1)] + fZec0[i])));
				fRec41[i] = (((fRec40[(i - 1)] >= 1.0f) & (fRec42[(i - 1)] != fSlow0))?fSlow0:fRec41[(i - 1)]);
				fRec42[i] = (((fRec40[(i - 1)] <= 0.0f) & (fRec41[(i - 1)] != fSlow0))?fSlow0:fRec42[(i - 1)]);
				
			}
			/* Post code */
			for (int j1 = 0; (j1 < 4); j1 = (j1 + 1)) {
				fRec39_perm[j1] = fRec39_tmp[(vsize + j1)];
				
			}
			for (int j3 = 0; (j3 < 4); j3 = (j3 + 1)) {
				fRec40_perm[j3] = fRec40_tmp[(vsize + j3)];
				
			}
			for (int j5 = 0; (j5 < 4); j5 = (j5 + 1)) {
				fRec41_perm[j5] = fRec41_tmp[(vsize + j5)];
				
			}
			for (int j7 = 0; (j7 < 4); j7 = (j7 + 1)) {
				fRec42_perm[j7] = fRec42_tmp[(vsize + j7)];
				
			}
			/* Vectorizable loop 43 */
			/* Pre code */
			for (int j20 = 0; (j20 < 4); j20 = (j20 + 1)) {
				fVec3_tmp[j20] = fVec3_perm[j20];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fVec3[i] = fSlow7;
				
			}
			/* Post code */
			for (int j21 = 0; (j21 < 4); j21 = (j21 + 1)) {
				fVec3_perm[j21] = fVec3_tmp[(vsize + j21)];
				
			}
			/* Vectorizable loop 44 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec14[i] = (fZec10[i] + (9.0f - fZec13[i]));
				
			}
			/* Vectorizable loop 45 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec15[i] = (fZec10[i] + (8.0f - fZec13[i]));
				
			}
			/* Vectorizable loop 46 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec26[i] = (fRec45[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 47 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec32[i] = (fZec28[i] + (9.0f - fZec31[i]));
				
			}
			/* Vectorizable loop 48 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec33[i] = (fZec28[i] + (8.0f - fZec31[i]));
				
			}
			/* Vectorizable loop 49 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec41[i] = (fRec47[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 50 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec43[i] = (fRec48[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 51 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec45[i] = (fRec50[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 52 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec47[i] = (fRec51[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 53 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec49[i] = (fRec53[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 54 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec51[i] = (fRec54[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 55 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec53[i] = (fRec56[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 56 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec55[i] = (0.0f - fZec5[i]);
				
			}
			/* Vectorizable loop 57 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec62[i] = (fRec57[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 58 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec68[i] = (fRec59[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 59 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec70[i] = (fRec60[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 60 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec72[i] = (fRec62[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 61 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec74[i] = (fRec63[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 62 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec76[i] = (fRec65[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 63 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec78[i] = (fRec66[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 64 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec80[i] = (fRec68[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 65 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec86[i] = (fRec69[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 66 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec92[i] = (fRec71[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 67 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec94[i] = (fRec72[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 68 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec96[i] = (fRec74[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 69 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec98[i] = (fRec75[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 70 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec100[i] = (fRec77[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 71 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec102[i] = (fRec78[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 72 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec104[i] = (fRec80[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 73 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec106[i] = (fSlow65 + fVec5[(i - 1)]);
				
			}
			/* Vectorizable loop 74 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec6[i] = std::cos(fZec5[i]);
				
			}
			/* Vectorizable loop 75 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec7[i] = (fSlow7 + fVec3[(i - 1)]);
				
			}
			/* Vectorizable loop 76 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec8[i] = (1.0f - fRec40[i]);
				
			}
			/* Vectorizable loop 77 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec12[i] = int(fZec11[i]);
				
			}
			/* Vectorizable loop 78 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec16[i] = (fZec10[i] + (7.0f - fZec13[i]));
				
			}
			/* Vectorizable loop 79 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec17[i] = (fZec10[i] + (6.0f - fZec13[i]));
				
			}
			/* Vectorizable loop 80 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec18[i] = (fZec14[i] * fZec15[i]);
				
			}
			/* Vectorizable loop 81 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec22[i] = std::sin(fZec5[i]);
				
			}
			/* Vectorizable loop 82 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec19[i] = int(std::min<float>(65536.0f, std::max<float>(0.0f, fRec41[i])));
				
			}
			/* Vectorizable loop 83 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec20[i] = int(std::min<float>(65536.0f, std::max<float>(0.0f, fRec42[i])));
				
			}
			/* Vectorizable loop 84 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec36[i] = (fZec32[i] * fZec33[i]);
				
			}
			/* Vectorizable loop 85 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec27[i] = std::floor(fZec26[i]);
				
			}
			/* Vectorizable loop 86 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec30[i] = int(fZec29[i]);
				
			}
			/* Vectorizable loop 87 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec34[i] = (fZec28[i] + (7.0f - fZec31[i]));
				
			}
			/* Vectorizable loop 88 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec35[i] = (fZec28[i] + (6.0f - fZec31[i]));
				
			}
			/* Vectorizable loop 89 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec42[i] = std::floor(fZec41[i]);
				
			}
			/* Vectorizable loop 90 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec44[i] = std::floor(fZec43[i]);
				
			}
			/* Vectorizable loop 91 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec46[i] = std::floor(fZec45[i]);
				
			}
			/* Vectorizable loop 92 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec48[i] = std::floor(fZec47[i]);
				
			}
			/* Vectorizable loop 93 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec50[i] = std::floor(fZec49[i]);
				
			}
			/* Vectorizable loop 94 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec52[i] = std::floor(fZec51[i]);
				
			}
			/* Vectorizable loop 95 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec54[i] = std::floor(fZec53[i]);
				
			}
			/* Vectorizable loop 96 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec56[i] = std::cos(fZec55[i]);
				
			}
			/* Vectorizable loop 97 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec58[i] = std::sin(fZec55[i]);
				
			}
			/* Vectorizable loop 98 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec63[i] = std::floor(fZec62[i]);
				
			}
			/* Vectorizable loop 99 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec69[i] = std::floor(fZec68[i]);
				
			}
			/* Vectorizable loop 100 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec71[i] = std::floor(fZec70[i]);
				
			}
			/* Vectorizable loop 101 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec73[i] = std::floor(fZec72[i]);
				
			}
			/* Vectorizable loop 102 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec75[i] = std::floor(fZec74[i]);
				
			}
			/* Vectorizable loop 103 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec77[i] = std::floor(fZec76[i]);
				
			}
			/* Vectorizable loop 104 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec79[i] = std::floor(fZec78[i]);
				
			}
			/* Vectorizable loop 105 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec81[i] = std::floor(fZec80[i]);
				
			}
			/* Vectorizable loop 106 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec87[i] = std::floor(fZec86[i]);
				
			}
			/* Vectorizable loop 107 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec93[i] = std::floor(fZec92[i]);
				
			}
			/* Vectorizable loop 108 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec95[i] = std::floor(fZec94[i]);
				
			}
			/* Vectorizable loop 109 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec97[i] = std::floor(fZec96[i]);
				
			}
			/* Vectorizable loop 110 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec99[i] = std::floor(fZec98[i]);
				
			}
			/* Vectorizable loop 111 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec101[i] = std::floor(fZec100[i]);
				
			}
			/* Vectorizable loop 112 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec103[i] = std::floor(fZec102[i]);
				
			}
			/* Vectorizable loop 113 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec105[i] = std::floor(fZec104[i]);
				
			}
			/* Vectorizable loop 114 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec107[i] = (1.0f - (0.5f * fZec106[i]));
				
			}
			/* Recursive loop 115 */
			/* Pre code */
			fYec0_idx = ((fYec0_idx + fYec0_idx_save) & 131071);
			fYec1_idx = ((fYec1_idx + fYec1_idx_save) & 16383);
			for (int j24 = 0; (j24 < 4); j24 = (j24 + 1)) {
				fYec2_tmp[j24] = fYec2_perm[j24];
				
			}
			for (int j26 = 0; (j26 < 4); j26 = (j26 + 1)) {
				fRec38_tmp[j26] = fRec38_perm[j26];
				
			}
			for (int j28 = 0; (j28 < 4); j28 = (j28 + 1)) {
				fRec36_tmp[j28] = fRec36_perm[j28];
				
			}
			fYec3_idx = ((fYec3_idx + fYec3_idx_save) & 131071);
			fYec4_idx = ((fYec4_idx + fYec4_idx_save) & 16383);
			for (int j32 = 0; (j32 < 4); j32 = (j32 + 1)) {
				fYec5_tmp[j32] = fYec5_perm[j32];
				
			}
			for (int j34 = 0; (j34 < 4); j34 = (j34 + 1)) {
				fRec46_tmp[j34] = fRec46_perm[j34];
				
			}
			for (int j36 = 0; (j36 < 4); j36 = (j36 + 1)) {
				fRec37_tmp[j36] = fRec37_perm[j36];
				
			}
			fYec6_idx = ((fYec6_idx + fYec6_idx_save) & 16383);
			for (int j40 = 0; (j40 < 4); j40 = (j40 + 1)) {
				fYec7_tmp[j40] = fYec7_perm[j40];
				
			}
			for (int j42 = 0; (j42 < 4); j42 = (j42 + 1)) {
				fRec35_tmp[j42] = fRec35_perm[j42];
				
			}
			for (int j44 = 0; (j44 < 4); j44 = (j44 + 1)) {
				fRec33_tmp[j44] = fRec33_perm[j44];
				
			}
			fYec8_idx = ((fYec8_idx + fYec8_idx_save) & 16383);
			for (int j48 = 0; (j48 < 4); j48 = (j48 + 1)) {
				fYec9_tmp[j48] = fYec9_perm[j48];
				
			}
			for (int j50 = 0; (j50 < 4); j50 = (j50 + 1)) {
				fRec49_tmp[j50] = fRec49_perm[j50];
				
			}
			for (int j52 = 0; (j52 < 4); j52 = (j52 + 1)) {
				fRec34_tmp[j52] = fRec34_perm[j52];
				
			}
			fYec10_idx = ((fYec10_idx + fYec10_idx_save) & 16383);
			for (int j56 = 0; (j56 < 4); j56 = (j56 + 1)) {
				fYec11_tmp[j56] = fYec11_perm[j56];
				
			}
			for (int j58 = 0; (j58 < 4); j58 = (j58 + 1)) {
				fRec32_tmp[j58] = fRec32_perm[j58];
				
			}
			for (int j60 = 0; (j60 < 4); j60 = (j60 + 1)) {
				fRec30_tmp[j60] = fRec30_perm[j60];
				
			}
			fYec12_idx = ((fYec12_idx + fYec12_idx_save) & 16383);
			for (int j64 = 0; (j64 < 4); j64 = (j64 + 1)) {
				fYec13_tmp[j64] = fYec13_perm[j64];
				
			}
			for (int j66 = 0; (j66 < 4); j66 = (j66 + 1)) {
				fRec52_tmp[j66] = fRec52_perm[j66];
				
			}
			for (int j68 = 0; (j68 < 4); j68 = (j68 + 1)) {
				fRec31_tmp[j68] = fRec31_perm[j68];
				
			}
			fYec14_idx = ((fYec14_idx + fYec14_idx_save) & 16383);
			for (int j72 = 0; (j72 < 4); j72 = (j72 + 1)) {
				fYec15_tmp[j72] = fYec15_perm[j72];
				
			}
			for (int j74 = 0; (j74 < 4); j74 = (j74 + 1)) {
				fRec29_tmp[j74] = fRec29_perm[j74];
				
			}
			for (int j76 = 0; (j76 < 4); j76 = (j76 + 1)) {
				fRec27_tmp[j76] = fRec27_perm[j76];
				
			}
			fYec16_idx = ((fYec16_idx + fYec16_idx_save) & 16383);
			for (int j80 = 0; (j80 < 4); j80 = (j80 + 1)) {
				fYec17_tmp[j80] = fYec17_perm[j80];
				
			}
			for (int j82 = 0; (j82 < 4); j82 = (j82 + 1)) {
				fRec55_tmp[j82] = fRec55_perm[j82];
				
			}
			for (int j84 = 0; (j84 < 4); j84 = (j84 + 1)) {
				fRec28_tmp[j84] = fRec28_perm[j84];
				
			}
			fYec18_idx = ((fYec18_idx + fYec18_idx_save) & 16383);
			for (int j88 = 0; (j88 < 4); j88 = (j88 + 1)) {
				fYec19_tmp[j88] = fYec19_perm[j88];
				
			}
			for (int j90 = 0; (j90 < 4); j90 = (j90 + 1)) {
				fRec26_tmp[j90] = fRec26_perm[j90];
				
			}
			for (int j92 = 0; (j92 < 4); j92 = (j92 + 1)) {
				fRec24_tmp[j92] = fRec24_perm[j92];
				
			}
			fYec20_idx = ((fYec20_idx + fYec20_idx_save) & 16383);
			for (int j96 = 0; (j96 < 4); j96 = (j96 + 1)) {
				fYec21_tmp[j96] = fYec21_perm[j96];
				
			}
			for (int j98 = 0; (j98 < 4); j98 = (j98 + 1)) {
				fRec58_tmp[j98] = fRec58_perm[j98];
				
			}
			for (int j100 = 0; (j100 < 4); j100 = (j100 + 1)) {
				fRec25_tmp[j100] = fRec25_perm[j100];
				
			}
			fYec22_idx = ((fYec22_idx + fYec22_idx_save) & 16383);
			for (int j104 = 0; (j104 < 4); j104 = (j104 + 1)) {
				fYec23_tmp[j104] = fYec23_perm[j104];
				
			}
			for (int j106 = 0; (j106 < 4); j106 = (j106 + 1)) {
				fRec23_tmp[j106] = fRec23_perm[j106];
				
			}
			for (int j108 = 0; (j108 < 4); j108 = (j108 + 1)) {
				fRec21_tmp[j108] = fRec21_perm[j108];
				
			}
			fYec24_idx = ((fYec24_idx + fYec24_idx_save) & 16383);
			for (int j112 = 0; (j112 < 4); j112 = (j112 + 1)) {
				fYec25_tmp[j112] = fYec25_perm[j112];
				
			}
			for (int j114 = 0; (j114 < 4); j114 = (j114 + 1)) {
				fRec61_tmp[j114] = fRec61_perm[j114];
				
			}
			for (int j116 = 0; (j116 < 4); j116 = (j116 + 1)) {
				fRec22_tmp[j116] = fRec22_perm[j116];
				
			}
			fYec26_idx = ((fYec26_idx + fYec26_idx_save) & 16383);
			for (int j120 = 0; (j120 < 4); j120 = (j120 + 1)) {
				fYec27_tmp[j120] = fYec27_perm[j120];
				
			}
			for (int j122 = 0; (j122 < 4); j122 = (j122 + 1)) {
				fRec20_tmp[j122] = fRec20_perm[j122];
				
			}
			for (int j124 = 0; (j124 < 4); j124 = (j124 + 1)) {
				fRec18_tmp[j124] = fRec18_perm[j124];
				
			}
			fYec28_idx = ((fYec28_idx + fYec28_idx_save) & 16383);
			for (int j128 = 0; (j128 < 4); j128 = (j128 + 1)) {
				fYec29_tmp[j128] = fYec29_perm[j128];
				
			}
			for (int j130 = 0; (j130 < 4); j130 = (j130 + 1)) {
				fRec64_tmp[j130] = fRec64_perm[j130];
				
			}
			for (int j132 = 0; (j132 < 4); j132 = (j132 + 1)) {
				fRec19_tmp[j132] = fRec19_perm[j132];
				
			}
			fYec30_idx = ((fYec30_idx + fYec30_idx_save) & 16383);
			for (int j136 = 0; (j136 < 4); j136 = (j136 + 1)) {
				fYec31_tmp[j136] = fYec31_perm[j136];
				
			}
			for (int j138 = 0; (j138 < 4); j138 = (j138 + 1)) {
				fRec17_tmp[j138] = fRec17_perm[j138];
				
			}
			for (int j140 = 0; (j140 < 4); j140 = (j140 + 1)) {
				fRec15_tmp[j140] = fRec15_perm[j140];
				
			}
			fYec32_idx = ((fYec32_idx + fYec32_idx_save) & 16383);
			for (int j144 = 0; (j144 < 4); j144 = (j144 + 1)) {
				fYec33_tmp[j144] = fYec33_perm[j144];
				
			}
			for (int j146 = 0; (j146 < 4); j146 = (j146 + 1)) {
				fRec67_tmp[j146] = fRec67_perm[j146];
				
			}
			for (int j148 = 0; (j148 < 4); j148 = (j148 + 1)) {
				fRec16_tmp[j148] = fRec16_perm[j148];
				
			}
			fYec34_idx = ((fYec34_idx + fYec34_idx_save) & 16383);
			for (int j152 = 0; (j152 < 4); j152 = (j152 + 1)) {
				fYec35_tmp[j152] = fYec35_perm[j152];
				
			}
			for (int j154 = 0; (j154 < 4); j154 = (j154 + 1)) {
				fRec14_tmp[j154] = fRec14_perm[j154];
				
			}
			for (int j156 = 0; (j156 < 4); j156 = (j156 + 1)) {
				fRec12_tmp[j156] = fRec12_perm[j156];
				
			}
			fYec36_idx = ((fYec36_idx + fYec36_idx_save) & 16383);
			for (int j160 = 0; (j160 < 4); j160 = (j160 + 1)) {
				fYec37_tmp[j160] = fYec37_perm[j160];
				
			}
			for (int j162 = 0; (j162 < 4); j162 = (j162 + 1)) {
				fRec70_tmp[j162] = fRec70_perm[j162];
				
			}
			for (int j164 = 0; (j164 < 4); j164 = (j164 + 1)) {
				fRec13_tmp[j164] = fRec13_perm[j164];
				
			}
			fYec38_idx = ((fYec38_idx + fYec38_idx_save) & 16383);
			for (int j168 = 0; (j168 < 4); j168 = (j168 + 1)) {
				fYec39_tmp[j168] = fYec39_perm[j168];
				
			}
			for (int j170 = 0; (j170 < 4); j170 = (j170 + 1)) {
				fRec11_tmp[j170] = fRec11_perm[j170];
				
			}
			for (int j172 = 0; (j172 < 4); j172 = (j172 + 1)) {
				fRec9_tmp[j172] = fRec9_perm[j172];
				
			}
			fYec40_idx = ((fYec40_idx + fYec40_idx_save) & 16383);
			for (int j176 = 0; (j176 < 4); j176 = (j176 + 1)) {
				fYec41_tmp[j176] = fYec41_perm[j176];
				
			}
			for (int j178 = 0; (j178 < 4); j178 = (j178 + 1)) {
				fRec73_tmp[j178] = fRec73_perm[j178];
				
			}
			for (int j180 = 0; (j180 < 4); j180 = (j180 + 1)) {
				fRec10_tmp[j180] = fRec10_perm[j180];
				
			}
			fYec42_idx = ((fYec42_idx + fYec42_idx_save) & 16383);
			for (int j184 = 0; (j184 < 4); j184 = (j184 + 1)) {
				fYec43_tmp[j184] = fYec43_perm[j184];
				
			}
			for (int j186 = 0; (j186 < 4); j186 = (j186 + 1)) {
				fRec8_tmp[j186] = fRec8_perm[j186];
				
			}
			for (int j188 = 0; (j188 < 4); j188 = (j188 + 1)) {
				fRec6_tmp[j188] = fRec6_perm[j188];
				
			}
			fYec44_idx = ((fYec44_idx + fYec44_idx_save) & 16383);
			for (int j192 = 0; (j192 < 4); j192 = (j192 + 1)) {
				fYec45_tmp[j192] = fYec45_perm[j192];
				
			}
			for (int j194 = 0; (j194 < 4); j194 = (j194 + 1)) {
				fRec76_tmp[j194] = fRec76_perm[j194];
				
			}
			for (int j196 = 0; (j196 < 4); j196 = (j196 + 1)) {
				fRec7_tmp[j196] = fRec7_perm[j196];
				
			}
			fYec46_idx = ((fYec46_idx + fYec46_idx_save) & 16383);
			for (int j200 = 0; (j200 < 4); j200 = (j200 + 1)) {
				fYec47_tmp[j200] = fYec47_perm[j200];
				
			}
			for (int j202 = 0; (j202 < 4); j202 = (j202 + 1)) {
				fRec5_tmp[j202] = fRec5_perm[j202];
				
			}
			for (int j204 = 0; (j204 < 4); j204 = (j204 + 1)) {
				fRec3_tmp[j204] = fRec3_perm[j204];
				
			}
			fYec48_idx = ((fYec48_idx + fYec48_idx_save) & 16383);
			for (int j208 = 0; (j208 < 4); j208 = (j208 + 1)) {
				fYec49_tmp[j208] = fYec49_perm[j208];
				
			}
			for (int j210 = 0; (j210 < 4); j210 = (j210 + 1)) {
				fRec79_tmp[j210] = fRec79_perm[j210];
				
			}
			for (int j212 = 0; (j212 < 4); j212 = (j212 + 1)) {
				fRec4_tmp[j212] = fRec4_perm[j212];
				
			}
			for (int j216 = 0; (j216 < 4); j216 = (j216 + 1)) {
				fRec2_tmp[j216] = fRec2_perm[j216];
				
			}
			fRec0_idx = ((fRec0_idx + fRec0_idx_save) & 1023);
			for (int j218 = 0; (j218 < 4); j218 = (j218 + 1)) {
				fRec81_tmp[j218] = fRec81_perm[j218];
				
			}
			fRec1_idx = ((fRec1_idx + fRec1_idx_save) & 1023);
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fYec0[((i + fYec0_idx) & 131071)] = (((((fRec1[(((i + fRec1_idx) - (std::min<int>(512, std::max<int>(0, iZec12[i])) + 1)) & 1023)] * (0.0f - fZec14[i])) * (0.0f - (0.5f * fZec15[i]))) * (0.0f - (0.333333343f * fZec16[i]))) * (0.0f - (0.25f * fZec17[i]))) + ((fZec10[i] + (10.0f - fZec13[i])) * ((((((fRec1[(((i + fRec1_idx) - (std::min<int>(512, std::max<int>(0, (iZec12[i] + 1))) + 1)) & 1023)] * (0.0f - fZec15[i])) * (0.0f - (0.5f * fZec16[i]))) * (0.0f - (0.333333343f * fZec17[i]))) + (0.5f * (((fZec14[i] * fRec1[(((i + fRec1_idx) - (std::min<int>(512, std::max<int>(0, (iZec12[i] + 2))) + 1)) & 1023)]) * (0.0f - fZec16[i])) * (0.0f - (0.5f * fZec17[i]))))) + (0.166666672f * ((fZec18[i] * fRec1[(((i + fRec1_idx) - (std::min<int>(512, std::max<int>(0, (iZec12[i] + 3))) + 1)) & 1023)]) * (0.0f - fZec17[i])))) + (0.0416666679f * ((fZec18[i] * fZec16[i]) * fRec1[(((i + fRec1_idx) - (std::min<int>(512, std::max<int>(0, (iZec12[i] + 4))) + 1)) & 1023)])))));
				fZec21[i] = ((0.5f * (fZec7[i] * ((fZec8[i] * fYec0[(((i + fYec0_idx) - iZec19[i]) & 131071)]) + (fRec40[i] * fYec0[(((i + fYec0_idx) - iZec20[i]) & 131071)])))) + float(input1[i]));
				fZec23[i] = ((fZec6[i] * fZec21[i]) - (fZec22[i] * fRec28[(i - 1)]));
				fZec24[i] = ((fZec6[i] * fZec23[i]) - (fZec22[i] * fRec31[(i - 1)]));
				fZec25[i] = ((fZec6[i] * fZec24[i]) - (fZec22[i] * fRec34[(i - 1)]));
				fYec1[((i + fYec1_idx) & 16383)] = (0.0f - ((fZec6[i] * fZec25[i]) - (fZec22[i] * fRec37[(i - 1)])));
				fYec2[i] = fYec1[(((i + fYec1_idx) - std::min<int>(8192, std::max<int>(0, int(fZec26[i])))) & 16383)];
				fRec38[i] = (fYec2[(i - 1)] - (((fZec27[i] + (2.0f - fRec45[i])) * (fRec38[(i - 1)] - fYec2[i])) / (fRec45[i] - fZec27[i])));
				fRec36[i] = fRec38[i];
				fYec3[((i + fYec3_idx) & 131071)] = (((((fRec0[(((i + fRec0_idx) - (std::min<int>(512, std::max<int>(0, iZec30[i])) + 1)) & 1023)] * (0.0f - fZec32[i])) * (0.0f - (0.5f * fZec33[i]))) * (0.0f - (0.333333343f * fZec34[i]))) * (0.0f - (0.25f * fZec35[i]))) + ((fZec28[i] + (10.0f - fZec31[i])) * ((((((fRec0[(((i + fRec0_idx) - (std::min<int>(512, std::max<int>(0, (iZec30[i] + 1))) + 1)) & 1023)] * (0.0f - fZec33[i])) * (0.0f - (0.5f * fZec34[i]))) * (0.0f - (0.333333343f * fZec35[i]))) + (0.5f * (((fZec32[i] * fRec0[(((i + fRec0_idx) - (std::min<int>(512, std::max<int>(0, (iZec30[i] + 2))) + 1)) & 1023)]) * (0.0f - fZec34[i])) * (0.0f - (0.5f * fZec35[i]))))) + (0.166666672f * ((fZec36[i] * fRec0[(((i + fRec0_idx) - (std::min<int>(512, std::max<int>(0, (iZec30[i] + 3))) + 1)) & 1023)]) * (0.0f - fZec35[i])))) + (0.0416666679f * ((fZec36[i] * fZec34[i]) * fRec0[(((i + fRec0_idx) - (std::min<int>(512, std::max<int>(0, (iZec30[i] + 4))) + 1)) & 1023)])))));
				fZec37[i] = ((0.5f * (((fZec8[i] * fYec3[(((i + fYec3_idx) - iZec19[i]) & 131071)]) + (fRec40[i] * fYec3[(((i + fYec3_idx) - iZec20[i]) & 131071)])) * fZec7[i])) + float(input0[i]));
				fZec38[i] = ((fZec37[i] * fZec6[i]) - (fZec22[i] * fRec27[(i - 1)]));
				fZec39[i] = ((fZec6[i] * fZec38[i]) - (fZec22[i] * fRec30[(i - 1)]));
				fZec40[i] = ((fZec6[i] * fZec39[i]) - (fZec22[i] * fRec33[(i - 1)]));
				fYec4[((i + fYec4_idx) & 16383)] = ((fZec6[i] * fZec40[i]) - (fZec22[i] * fRec36[(i - 1)]));
				fYec5[i] = fYec4[(((i + fYec4_idx) - std::min<int>(8192, std::max<int>(0, int(fZec41[i])))) & 16383)];
				fRec46[i] = (fYec5[(i - 1)] - (((fZec42[i] + (2.0f - fRec47[i])) * (fRec46[(i - 1)] - fYec5[i])) / (fRec47[i] - fZec42[i])));
				fRec37[i] = fRec46[i];
				fYec6[((i + fYec6_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec37[(i - 1)]) + (fZec22[i] * fZec25[i])));
				fYec7[i] = fYec6[(((i + fYec6_idx) - std::min<int>(8192, std::max<int>(0, int(fZec43[i])))) & 16383)];
				fRec35[i] = (fYec7[(i - 1)] - (((fZec44[i] + (2.0f - fRec48[i])) * (fRec35[(i - 1)] - fYec7[i])) / (fRec48[i] - fZec44[i])));
				fRec33[i] = fRec35[i];
				fYec8[((i + fYec8_idx) & 16383)] = ((fRec36[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec40[i]));
				fYec9[i] = fYec8[(((i + fYec8_idx) - std::min<int>(8192, std::max<int>(0, int(fZec45[i])))) & 16383)];
				fRec49[i] = (fYec9[(i - 1)] - (((fZec46[i] + (2.0f - fRec50[i])) * (fRec49[(i - 1)] - fYec9[i])) / (fRec50[i] - fZec46[i])));
				fRec34[i] = fRec49[i];
				fYec10[((i + fYec10_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec34[(i - 1)]) + (fZec22[i] * fZec24[i])));
				fYec11[i] = fYec10[(((i + fYec10_idx) - std::min<int>(8192, std::max<int>(0, int(fZec47[i])))) & 16383)];
				fRec32[i] = (fYec11[(i - 1)] - (((fZec48[i] + (2.0f - fRec51[i])) * (fRec32[(i - 1)] - fYec11[i])) / (fRec51[i] - fZec48[i])));
				fRec30[i] = fRec32[i];
				fYec12[((i + fYec12_idx) & 16383)] = ((fRec33[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec39[i]));
				fYec13[i] = fYec12[(((i + fYec12_idx) - std::min<int>(8192, std::max<int>(0, int(fZec49[i])))) & 16383)];
				fRec52[i] = (fYec13[(i - 1)] - (((fZec50[i] + (2.0f - fRec53[i])) * (fRec52[(i - 1)] - fYec13[i])) / (fRec53[i] - fZec50[i])));
				fRec31[i] = fRec52[i];
				fYec14[((i + fYec14_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec31[(i - 1)]) + (fZec22[i] * fZec23[i])));
				fYec15[i] = fYec14[(((i + fYec14_idx) - std::min<int>(8192, std::max<int>(0, int(fZec51[i])))) & 16383)];
				fRec29[i] = (fYec15[(i - 1)] - (((fZec52[i] + (2.0f - fRec54[i])) * (fRec29[(i - 1)] - fYec15[i])) / (fRec54[i] - fZec52[i])));
				fRec27[i] = fRec29[i];
				fYec16[((i + fYec16_idx) & 16383)] = ((fRec30[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec38[i]));
				fYec17[i] = fYec16[(((i + fYec16_idx) - std::min<int>(8192, std::max<int>(0, int(fZec53[i])))) & 16383)];
				fRec55[i] = (fYec17[(i - 1)] - (((fZec54[i] + (2.0f - fRec56[i])) * (fRec55[(i - 1)] - fYec17[i])) / (fRec56[i] - fZec54[i])));
				fRec28[i] = fRec55[i];
				fZec57[i] = ((fZec6[i] * fRec28[(i - 1)]) + (fZec22[i] * fZec21[i]));
				fZec59[i] = ((fZec56[i] * fZec57[i]) - (fZec58[i] * fRec16[(i - 1)]));
				fZec60[i] = ((fZec56[i] * fZec59[i]) - (fZec58[i] * fRec19[(i - 1)]));
				fZec61[i] = ((fZec56[i] * fZec60[i]) - (fZec58[i] * fRec22[(i - 1)]));
				fYec18[((i + fYec18_idx) & 16383)] = (0.0f - ((fZec56[i] * fZec61[i]) - (fZec58[i] * fRec25[(i - 1)])));
				fYec19[i] = fYec18[(((i + fYec18_idx) - std::min<int>(8192, std::max<int>(0, int(fZec62[i])))) & 16383)];
				fRec26[i] = (fYec19[(i - 1)] - (((fZec63[i] + (2.0f - fRec57[i])) * (fRec26[(i - 1)] - fYec19[i])) / (fRec57[i] - fZec63[i])));
				fRec24[i] = fRec26[i];
				fZec64[i] = ((fRec27[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec37[i]));
				fZec65[i] = ((fZec64[i] * fZec56[i]) - (fZec58[i] * fRec15[(i - 1)]));
				fZec66[i] = ((fZec56[i] * fZec65[i]) - (fZec58[i] * fRec18[(i - 1)]));
				fZec67[i] = ((fZec56[i] * fZec66[i]) - (fZec58[i] * fRec21[(i - 1)]));
				fYec20[((i + fYec20_idx) & 16383)] = ((fZec56[i] * fZec67[i]) - (fRec24[(i - 1)] * fZec58[i]));
				fYec21[i] = fYec20[(((i + fYec20_idx) - std::min<int>(8192, std::max<int>(0, int(fZec68[i])))) & 16383)];
				fRec58[i] = (fYec21[(i - 1)] - (((fZec69[i] + (2.0f - fRec59[i])) * (fRec58[(i - 1)] - fYec21[i])) / (fRec59[i] - fZec69[i])));
				fRec25[i] = fRec58[i];
				fYec22[((i + fYec22_idx) & 16383)] = (0.0f - ((fZec56[i] * fRec25[(i - 1)]) + (fZec58[i] * fZec61[i])));
				fYec23[i] = fYec22[(((i + fYec22_idx) - std::min<int>(8192, std::max<int>(0, int(fZec70[i])))) & 16383)];
				fRec23[i] = (fYec23[(i - 1)] - (((fZec71[i] + (2.0f - fRec60[i])) * (fRec23[(i - 1)] - fYec23[i])) / (fRec60[i] - fZec71[i])));
				fRec21[i] = fRec23[i];
				fYec24[((i + fYec24_idx) & 16383)] = ((fRec24[(i - 1)] * fZec56[i]) + (fZec58[i] * fZec67[i]));
				fYec25[i] = fYec24[(((i + fYec24_idx) - std::min<int>(8192, std::max<int>(0, int(fZec72[i])))) & 16383)];
				fRec61[i] = (fYec25[(i - 1)] - (((fZec73[i] + (2.0f - fRec62[i])) * (fRec61[(i - 1)] - fYec25[i])) / (fRec62[i] - fZec73[i])));
				fRec22[i] = fRec61[i];
				fYec26[((i + fYec26_idx) & 16383)] = (0.0f - ((fZec56[i] * fRec22[(i - 1)]) + (fZec58[i] * fZec60[i])));
				fYec27[i] = fYec26[(((i + fYec26_idx) - std::min<int>(8192, std::max<int>(0, int(fZec74[i])))) & 16383)];
				fRec20[i] = (fYec27[(i - 1)] - (((fZec75[i] + (2.0f - fRec63[i])) * (fRec20[(i - 1)] - fYec27[i])) / (fRec63[i] - fZec75[i])));
				fRec18[i] = fRec20[i];
				fYec28[((i + fYec28_idx) & 16383)] = ((fRec21[(i - 1)] * fZec56[i]) + (fZec58[i] * fZec66[i]));
				fYec29[i] = fYec28[(((i + fYec28_idx) - std::min<int>(8192, std::max<int>(0, int(fZec76[i])))) & 16383)];
				fRec64[i] = (fYec29[(i - 1)] - (((fZec77[i] + (2.0f - fRec65[i])) * (fRec64[(i - 1)] - fYec29[i])) / (fRec65[i] - fZec77[i])));
				fRec19[i] = fRec64[i];
				fYec30[((i + fYec30_idx) & 16383)] = (0.0f - ((fZec56[i] * fRec19[(i - 1)]) + (fZec58[i] * fZec59[i])));
				fYec31[i] = fYec30[(((i + fYec30_idx) - std::min<int>(8192, std::max<int>(0, int(fZec78[i])))) & 16383)];
				fRec17[i] = (fYec31[(i - 1)] - (((fZec79[i] + (2.0f - fRec66[i])) * (fRec17[(i - 1)] - fYec31[i])) / (fRec66[i] - fZec79[i])));
				fRec15[i] = fRec17[i];
				fYec32[((i + fYec32_idx) & 16383)] = ((fRec18[(i - 1)] * fZec56[i]) + (fZec58[i] * fZec65[i]));
				fYec33[i] = fYec32[(((i + fYec32_idx) - std::min<int>(8192, std::max<int>(0, int(fZec80[i])))) & 16383)];
				fRec67[i] = (fYec33[(i - 1)] - (((fZec81[i] + (2.0f - fRec68[i])) * (fRec67[(i - 1)] - fYec33[i])) / (fRec68[i] - fZec81[i])));
				fRec16[i] = fRec67[i];
				fZec82[i] = ((fZec56[i] * fRec16[(i - 1)]) + (fZec58[i] * fZec57[i]));
				fZec83[i] = ((fZec6[i] * fZec82[i]) - (fZec22[i] * fRec4[(i - 1)]));
				fZec84[i] = ((fZec6[i] * fZec83[i]) - (fZec22[i] * fRec7[(i - 1)]));
				fZec85[i] = ((fZec6[i] * fZec84[i]) - (fZec22[i] * fRec10[(i - 1)]));
				fYec34[((i + fYec34_idx) & 16383)] = (0.0f - ((fZec6[i] * fZec85[i]) - (fZec22[i] * fRec13[(i - 1)])));
				fYec35[i] = fYec34[(((i + fYec34_idx) - std::min<int>(8192, std::max<int>(0, int(fZec86[i])))) & 16383)];
				fRec14[i] = (fYec35[(i - 1)] - (((fZec87[i] + (2.0f - fRec69[i])) * (fRec14[(i - 1)] - fYec35[i])) / (fRec69[i] - fZec87[i])));
				fRec12[i] = fRec14[i];
				fZec88[i] = ((fRec15[(i - 1)] * fZec56[i]) + (fZec58[i] * fZec64[i]));
				fZec89[i] = ((fZec6[i] * fZec88[i]) - (fZec22[i] * fRec3[(i - 1)]));
				fZec90[i] = ((fZec6[i] * fZec89[i]) - (fZec22[i] * fRec6[(i - 1)]));
				fZec91[i] = ((fZec6[i] * fZec90[i]) - (fZec22[i] * fRec9[(i - 1)]));
				fYec36[((i + fYec36_idx) & 16383)] = ((fZec6[i] * fZec91[i]) - (fRec12[(i - 1)] * fZec22[i]));
				fYec37[i] = fYec36[(((i + fYec36_idx) - std::min<int>(8192, std::max<int>(0, int(fZec92[i])))) & 16383)];
				fRec70[i] = (fYec37[(i - 1)] - (((fZec93[i] + (2.0f - fRec71[i])) * (fRec70[(i - 1)] - fYec37[i])) / (fRec71[i] - fZec93[i])));
				fRec13[i] = fRec70[i];
				fYec38[((i + fYec38_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec13[(i - 1)]) + (fZec22[i] * fZec85[i])));
				fYec39[i] = fYec38[(((i + fYec38_idx) - std::min<int>(8192, std::max<int>(0, int(fZec94[i])))) & 16383)];
				fRec11[i] = (fYec39[(i - 1)] - (((fZec95[i] + (2.0f - fRec72[i])) * (fRec11[(i - 1)] - fYec39[i])) / (fRec72[i] - fZec95[i])));
				fRec9[i] = fRec11[i];
				fYec40[((i + fYec40_idx) & 16383)] = ((fRec12[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec91[i]));
				fYec41[i] = fYec40[(((i + fYec40_idx) - std::min<int>(8192, std::max<int>(0, int(fZec96[i])))) & 16383)];
				fRec73[i] = (fYec41[(i - 1)] - (((fZec97[i] + (2.0f - fRec74[i])) * (fRec73[(i - 1)] - fYec41[i])) / (fRec74[i] - fZec97[i])));
				fRec10[i] = fRec73[i];
				fYec42[((i + fYec42_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec10[(i - 1)]) + (fZec22[i] * fZec84[i])));
				fYec43[i] = fYec42[(((i + fYec42_idx) - std::min<int>(8192, std::max<int>(0, int(fZec98[i])))) & 16383)];
				fRec8[i] = (fYec43[(i - 1)] - (((fZec99[i] + (2.0f - fRec75[i])) * (fRec8[(i - 1)] - fYec43[i])) / (fRec75[i] - fZec99[i])));
				fRec6[i] = fRec8[i];
				fYec44[((i + fYec44_idx) & 16383)] = ((fRec9[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec90[i]));
				fYec45[i] = fYec44[(((i + fYec44_idx) - std::min<int>(8192, std::max<int>(0, int(fZec100[i])))) & 16383)];
				fRec76[i] = (fYec45[(i - 1)] - (((fZec101[i] + (2.0f - fRec77[i])) * (fRec76[(i - 1)] - fYec45[i])) / (fRec77[i] - fZec101[i])));
				fRec7[i] = fRec76[i];
				fYec46[((i + fYec46_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec7[(i - 1)]) + (fZec22[i] * fZec83[i])));
				fYec47[i] = fYec46[(((i + fYec46_idx) - std::min<int>(8192, std::max<int>(0, int(fZec102[i])))) & 16383)];
				fRec5[i] = (fYec47[(i - 1)] - (((fZec103[i] + (2.0f - fRec78[i])) * (fRec5[(i - 1)] - fYec47[i])) / (fRec78[i] - fZec103[i])));
				fRec3[i] = fRec5[i];
				fYec48[((i + fYec48_idx) & 16383)] = ((fRec6[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec89[i]));
				fYec49[i] = fYec48[(((i + fYec48_idx) - std::min<int>(8192, std::max<int>(0, int(fZec104[i])))) & 16383)];
				fRec79[i] = (fYec49[(i - 1)] - (((fZec105[i] + (2.0f - fRec80[i])) * (fRec79[(i - 1)] - fYec49[i])) / (fRec80[i] - fZec105[i])));
				fRec4[i] = fRec79[i];
				fRec2[i] = ((0.5f * (fRec2[(i - 1)] * fZec106[i])) + (((fRec3[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec88[i])) * fZec107[i]));
				fRec0[((i + fRec0_idx) & 1023)] = fRec2[i];
				fRec81[i] = ((0.5f * (fRec81[(i - 1)] * fZec106[i])) + (fZec107[i] * ((fZec6[i] * fRec4[(i - 1)]) + (fZec22[i] * fZec82[i]))));
				fRec1[((i + fRec1_idx) & 1023)] = fRec81[i];
				
			}
			/* Post code */
			for (int j219 = 0; (j219 < 4); j219 = (j219 + 1)) {
				fRec81_perm[j219] = fRec81_tmp[(vsize + j219)];
				
			}
			fYec48_idx_save = vsize;
			for (int j209 = 0; (j209 < 4); j209 = (j209 + 1)) {
				fYec49_perm[j209] = fYec49_tmp[(vsize + j209)];
				
			}
			for (int j211 = 0; (j211 < 4); j211 = (j211 + 1)) {
				fRec79_perm[j211] = fRec79_tmp[(vsize + j211)];
				
			}
			fYec46_idx_save = vsize;
			for (int j201 = 0; (j201 < 4); j201 = (j201 + 1)) {
				fYec47_perm[j201] = fYec47_tmp[(vsize + j201)];
				
			}
			fYec44_idx_save = vsize;
			for (int j193 = 0; (j193 < 4); j193 = (j193 + 1)) {
				fYec45_perm[j193] = fYec45_tmp[(vsize + j193)];
				
			}
			for (int j195 = 0; (j195 < 4); j195 = (j195 + 1)) {
				fRec76_perm[j195] = fRec76_tmp[(vsize + j195)];
				
			}
			fYec42_idx_save = vsize;
			for (int j185 = 0; (j185 < 4); j185 = (j185 + 1)) {
				fYec43_perm[j185] = fYec43_tmp[(vsize + j185)];
				
			}
			fYec40_idx_save = vsize;
			for (int j177 = 0; (j177 < 4); j177 = (j177 + 1)) {
				fYec41_perm[j177] = fYec41_tmp[(vsize + j177)];
				
			}
			for (int j179 = 0; (j179 < 4); j179 = (j179 + 1)) {
				fRec73_perm[j179] = fRec73_tmp[(vsize + j179)];
				
			}
			fYec38_idx_save = vsize;
			for (int j169 = 0; (j169 < 4); j169 = (j169 + 1)) {
				fYec39_perm[j169] = fYec39_tmp[(vsize + j169)];
				
			}
			fYec36_idx_save = vsize;
			for (int j161 = 0; (j161 < 4); j161 = (j161 + 1)) {
				fYec37_perm[j161] = fYec37_tmp[(vsize + j161)];
				
			}
			for (int j163 = 0; (j163 < 4); j163 = (j163 + 1)) {
				fRec70_perm[j163] = fRec70_tmp[(vsize + j163)];
				
			}
			fYec34_idx_save = vsize;
			for (int j153 = 0; (j153 < 4); j153 = (j153 + 1)) {
				fYec35_perm[j153] = fYec35_tmp[(vsize + j153)];
				
			}
			fYec32_idx_save = vsize;
			for (int j145 = 0; (j145 < 4); j145 = (j145 + 1)) {
				fYec33_perm[j145] = fYec33_tmp[(vsize + j145)];
				
			}
			for (int j147 = 0; (j147 < 4); j147 = (j147 + 1)) {
				fRec67_perm[j147] = fRec67_tmp[(vsize + j147)];
				
			}
			fYec30_idx_save = vsize;
			for (int j137 = 0; (j137 < 4); j137 = (j137 + 1)) {
				fYec31_perm[j137] = fYec31_tmp[(vsize + j137)];
				
			}
			fYec28_idx_save = vsize;
			for (int j129 = 0; (j129 < 4); j129 = (j129 + 1)) {
				fYec29_perm[j129] = fYec29_tmp[(vsize + j129)];
				
			}
			for (int j131 = 0; (j131 < 4); j131 = (j131 + 1)) {
				fRec64_perm[j131] = fRec64_tmp[(vsize + j131)];
				
			}
			fYec26_idx_save = vsize;
			for (int j121 = 0; (j121 < 4); j121 = (j121 + 1)) {
				fYec27_perm[j121] = fYec27_tmp[(vsize + j121)];
				
			}
			fYec24_idx_save = vsize;
			for (int j113 = 0; (j113 < 4); j113 = (j113 + 1)) {
				fYec25_perm[j113] = fYec25_tmp[(vsize + j113)];
				
			}
			for (int j115 = 0; (j115 < 4); j115 = (j115 + 1)) {
				fRec61_perm[j115] = fRec61_tmp[(vsize + j115)];
				
			}
			fYec22_idx_save = vsize;
			for (int j105 = 0; (j105 < 4); j105 = (j105 + 1)) {
				fYec23_perm[j105] = fYec23_tmp[(vsize + j105)];
				
			}
			fYec20_idx_save = vsize;
			for (int j97 = 0; (j97 < 4); j97 = (j97 + 1)) {
				fYec21_perm[j97] = fYec21_tmp[(vsize + j97)];
				
			}
			for (int j99 = 0; (j99 < 4); j99 = (j99 + 1)) {
				fRec58_perm[j99] = fRec58_tmp[(vsize + j99)];
				
			}
			fYec18_idx_save = vsize;
			for (int j89 = 0; (j89 < 4); j89 = (j89 + 1)) {
				fYec19_perm[j89] = fYec19_tmp[(vsize + j89)];
				
			}
			fYec16_idx_save = vsize;
			for (int j81 = 0; (j81 < 4); j81 = (j81 + 1)) {
				fYec17_perm[j81] = fYec17_tmp[(vsize + j81)];
				
			}
			for (int j83 = 0; (j83 < 4); j83 = (j83 + 1)) {
				fRec55_perm[j83] = fRec55_tmp[(vsize + j83)];
				
			}
			fYec14_idx_save = vsize;
			for (int j73 = 0; (j73 < 4); j73 = (j73 + 1)) {
				fYec15_perm[j73] = fYec15_tmp[(vsize + j73)];
				
			}
			fYec12_idx_save = vsize;
			for (int j65 = 0; (j65 < 4); j65 = (j65 + 1)) {
				fYec13_perm[j65] = fYec13_tmp[(vsize + j65)];
				
			}
			for (int j67 = 0; (j67 < 4); j67 = (j67 + 1)) {
				fRec52_perm[j67] = fRec52_tmp[(vsize + j67)];
				
			}
			fYec10_idx_save = vsize;
			for (int j57 = 0; (j57 < 4); j57 = (j57 + 1)) {
				fYec11_perm[j57] = fYec11_tmp[(vsize + j57)];
				
			}
			fYec8_idx_save = vsize;
			for (int j49 = 0; (j49 < 4); j49 = (j49 + 1)) {
				fYec9_perm[j49] = fYec9_tmp[(vsize + j49)];
				
			}
			for (int j51 = 0; (j51 < 4); j51 = (j51 + 1)) {
				fRec49_perm[j51] = fRec49_tmp[(vsize + j51)];
				
			}
			fYec6_idx_save = vsize;
			for (int j41 = 0; (j41 < 4); j41 = (j41 + 1)) {
				fYec7_perm[j41] = fYec7_tmp[(vsize + j41)];
				
			}
			fYec3_idx_save = vsize;
			fYec4_idx_save = vsize;
			for (int j33 = 0; (j33 < 4); j33 = (j33 + 1)) {
				fYec5_perm[j33] = fYec5_tmp[(vsize + j33)];
				
			}
			for (int j35 = 0; (j35 < 4); j35 = (j35 + 1)) {
				fRec46_perm[j35] = fRec46_tmp[(vsize + j35)];
				
			}
			fYec0_idx_save = vsize;
			fYec1_idx_save = vsize;
			for (int j25 = 0; (j25 < 4); j25 = (j25 + 1)) {
				fYec2_perm[j25] = fYec2_tmp[(vsize + j25)];
				
			}
			for (int j27 = 0; (j27 < 4); j27 = (j27 + 1)) {
				fRec38_perm[j27] = fRec38_tmp[(vsize + j27)];
				
			}
			for (int j29 = 0; (j29 < 4); j29 = (j29 + 1)) {
				fRec36_perm[j29] = fRec36_tmp[(vsize + j29)];
				
			}
			for (int j37 = 0; (j37 < 4); j37 = (j37 + 1)) {
				fRec37_perm[j37] = fRec37_tmp[(vsize + j37)];
				
			}
			for (int j43 = 0; (j43 < 4); j43 = (j43 + 1)) {
				fRec35_perm[j43] = fRec35_tmp[(vsize + j43)];
				
			}
			for (int j45 = 0; (j45 < 4); j45 = (j45 + 1)) {
				fRec33_perm[j45] = fRec33_tmp[(vsize + j45)];
				
			}
			for (int j53 = 0; (j53 < 4); j53 = (j53 + 1)) {
				fRec34_perm[j53] = fRec34_tmp[(vsize + j53)];
				
			}
			for (int j59 = 0; (j59 < 4); j59 = (j59 + 1)) {
				fRec32_perm[j59] = fRec32_tmp[(vsize + j59)];
				
			}
			for (int j61 = 0; (j61 < 4); j61 = (j61 + 1)) {
				fRec30_perm[j61] = fRec30_tmp[(vsize + j61)];
				
			}
			for (int j69 = 0; (j69 < 4); j69 = (j69 + 1)) {
				fRec31_perm[j69] = fRec31_tmp[(vsize + j69)];
				
			}
			for (int j75 = 0; (j75 < 4); j75 = (j75 + 1)) {
				fRec29_perm[j75] = fRec29_tmp[(vsize + j75)];
				
			}
			for (int j77 = 0; (j77 < 4); j77 = (j77 + 1)) {
				fRec27_perm[j77] = fRec27_tmp[(vsize + j77)];
				
			}
			for (int j85 = 0; (j85 < 4); j85 = (j85 + 1)) {
				fRec28_perm[j85] = fRec28_tmp[(vsize + j85)];
				
			}
			for (int j91 = 0; (j91 < 4); j91 = (j91 + 1)) {
				fRec26_perm[j91] = fRec26_tmp[(vsize + j91)];
				
			}
			for (int j93 = 0; (j93 < 4); j93 = (j93 + 1)) {
				fRec24_perm[j93] = fRec24_tmp[(vsize + j93)];
				
			}
			for (int j101 = 0; (j101 < 4); j101 = (j101 + 1)) {
				fRec25_perm[j101] = fRec25_tmp[(vsize + j101)];
				
			}
			for (int j107 = 0; (j107 < 4); j107 = (j107 + 1)) {
				fRec23_perm[j107] = fRec23_tmp[(vsize + j107)];
				
			}
			for (int j109 = 0; (j109 < 4); j109 = (j109 + 1)) {
				fRec21_perm[j109] = fRec21_tmp[(vsize + j109)];
				
			}
			for (int j117 = 0; (j117 < 4); j117 = (j117 + 1)) {
				fRec22_perm[j117] = fRec22_tmp[(vsize + j117)];
				
			}
			for (int j123 = 0; (j123 < 4); j123 = (j123 + 1)) {
				fRec20_perm[j123] = fRec20_tmp[(vsize + j123)];
				
			}
			for (int j125 = 0; (j125 < 4); j125 = (j125 + 1)) {
				fRec18_perm[j125] = fRec18_tmp[(vsize + j125)];
				
			}
			for (int j133 = 0; (j133 < 4); j133 = (j133 + 1)) {
				fRec19_perm[j133] = fRec19_tmp[(vsize + j133)];
				
			}
			for (int j139 = 0; (j139 < 4); j139 = (j139 + 1)) {
				fRec17_perm[j139] = fRec17_tmp[(vsize + j139)];
				
			}
			for (int j141 = 0; (j141 < 4); j141 = (j141 + 1)) {
				fRec15_perm[j141] = fRec15_tmp[(vsize + j141)];
				
			}
			for (int j149 = 0; (j149 < 4); j149 = (j149 + 1)) {
				fRec16_perm[j149] = fRec16_tmp[(vsize + j149)];
				
			}
			for (int j155 = 0; (j155 < 4); j155 = (j155 + 1)) {
				fRec14_perm[j155] = fRec14_tmp[(vsize + j155)];
				
			}
			for (int j157 = 0; (j157 < 4); j157 = (j157 + 1)) {
				fRec12_perm[j157] = fRec12_tmp[(vsize + j157)];
				
			}
			for (int j165 = 0; (j165 < 4); j165 = (j165 + 1)) {
				fRec13_perm[j165] = fRec13_tmp[(vsize + j165)];
				
			}
			for (int j171 = 0; (j171 < 4); j171 = (j171 + 1)) {
				fRec11_perm[j171] = fRec11_tmp[(vsize + j171)];
				
			}
			for (int j173 = 0; (j173 < 4); j173 = (j173 + 1)) {
				fRec9_perm[j173] = fRec9_tmp[(vsize + j173)];
				
			}
			for (int j181 = 0; (j181 < 4); j181 = (j181 + 1)) {
				fRec10_perm[j181] = fRec10_tmp[(vsize + j181)];
				
			}
			for (int j187 = 0; (j187 < 4); j187 = (j187 + 1)) {
				fRec8_perm[j187] = fRec8_tmp[(vsize + j187)];
				
			}
			for (int j189 = 0; (j189 < 4); j189 = (j189 + 1)) {
				fRec6_perm[j189] = fRec6_tmp[(vsize + j189)];
				
			}
			for (int j197 = 0; (j197 < 4); j197 = (j197 + 1)) {
				fRec7_perm[j197] = fRec7_tmp[(vsize + j197)];
				
			}
			for (int j203 = 0; (j203 < 4); j203 = (j203 + 1)) {
				fRec5_perm[j203] = fRec5_tmp[(vsize + j203)];
				
			}
			for (int j205 = 0; (j205 < 4); j205 = (j205 + 1)) {
				fRec3_perm[j205] = fRec3_tmp[(vsize + j205)];
				
			}
			for (int j213 = 0; (j213 < 4); j213 = (j213 + 1)) {
				fRec4_perm[j213] = fRec4_tmp[(vsize + j213)];
				
			}
			for (int j217 = 0; (j217 < 4); j217 = (j217 + 1)) {
				fRec2_perm[j217] = fRec2_tmp[(vsize + j217)];
				
			}
			fRec0_idx_save = vsize;
			fRec1_idx_save = vsize;
			/* Vectorizable loop 116 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				output0[i] = FAUSTFLOAT(fRec0[((i + fRec0_idx) & 1023)]);
				
			}
			/* Vectorizable loop 117 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				output1[i] = FAUSTFLOAT(fRec1[((i + fRec1_idx) & 1023)]);
				
			}
			
		}
		/* Remaining frames */
		if (vindex < count) {
			FAUSTFLOAT* input0 = &input0_ptr[vindex];
			FAUSTFLOAT* input1 = &input1_ptr[vindex];
			FAUSTFLOAT* output0 = &output0_ptr[vindex];
			FAUSTFLOAT* output1 = &output1_ptr[vindex];
			int vsize = (count - vindex);
			/* Vectorizable loop 0 */
			/* Pre code */
			for (int j8 = 0; (j8 < 4); j8 = (j8 + 1)) {
				fVec0_tmp[j8] = fVec0_perm[j8];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fVec0[i] = fSlow1;
				
			}
			/* Post code */
			for (int j9 = 0; (j9 < 4); j9 = (j9 + 1)) {
				fVec0_perm[j9] = fVec0_tmp[(vsize + j9)];
				
			}
			/* Vectorizable loop 1 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec1[i] = (fConst1 * (fSlow1 + fVec0[(i - 1)]));
				
			}
			/* Vectorizable loop 2 */
			/* Pre code */
			for (int j12 = 0; (j12 < 4); j12 = (j12 + 1)) {
				iVec1_tmp[j12] = iVec1_perm[j12];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iVec1[i] = 1;
				
			}
			/* Post code */
			for (int j13 = 0; (j13 < 4); j13 = (j13 + 1)) {
				iVec1_perm[j13] = iVec1_tmp[(vsize + j13)];
				
			}
			/* Vectorizable loop 3 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec2[i] = std::sin(fZec1[i]);
				
			}
			/* Vectorizable loop 4 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec3[i] = std::cos(fZec1[i]);
				
			}
			/* Vectorizable loop 5 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec4[i] = (1 - iVec1[(i - 1)]);
				
			}
			/* Vectorizable loop 6 */
			/* Pre code */
			for (int j22 = 0; (j22 < 4); j22 = (j22 + 1)) {
				fVec4_tmp[j22] = fVec4_perm[j22];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fVec4[i] = fSlow8;
				
			}
			/* Post code */
			for (int j23 = 0; (j23 < 4); j23 = (j23 + 1)) {
				fVec4_perm[j23] = fVec4_tmp[(vsize + j23)];
				
			}
			/* Recursive loop 7 */
			/* Pre code */
			for (int j10 = 0; (j10 < 4); j10 = (j10 + 1)) {
				fRec43_tmp[j10] = fRec43_perm[j10];
				
			}
			for (int j14 = 0; (j14 < 4); j14 = (j14 + 1)) {
				fRec44_tmp[j14] = fRec44_perm[j14];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec43[i] = ((fRec44[(i - 1)] * fZec2[i]) + (fRec43[(i - 1)] * fZec3[i]));
				fRec44[i] = ((float(iZec4[i]) + (fRec44[(i - 1)] * fZec3[i])) - (fRec43[(i - 1)] * fZec2[i]));
				
			}
			/* Post code */
			for (int j11 = 0; (j11 < 4); j11 = (j11 + 1)) {
				fRec43_perm[j11] = fRec43_tmp[(vsize + j11)];
				
			}
			for (int j15 = 0; (j15 < 4); j15 = (j15 + 1)) {
				fRec44_perm[j15] = fRec44_tmp[(vsize + j15)];
				
			}
			/* Vectorizable loop 8 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec9[i] = (fSlow8 + fVec4[(i - 1)]);
				
			}
			/* Vectorizable loop 9 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec10[i] = (fConst2 * ((fRec43[i] + 1.0f) * fZec9[i]));
				
			}
			/* Vectorizable loop 10 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec28[i] = (fConst2 * ((fRec44[i] + 1.0f) * fZec9[i]));
				
			}
			/* Vectorizable loop 11 */
			/* Pre code */
			for (int j18 = 0; (j18 < 4); j18 = (j18 + 1)) {
				fVec2_tmp[j18] = fVec2_perm[j18];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fVec2[i] = fSlow6;
				
			}
			/* Post code */
			for (int j19 = 0; (j19 < 4); j19 = (j19 + 1)) {
				fVec2_perm[j19] = fVec2_tmp[(vsize + j19)];
				
			}
			/* Vectorizable loop 12 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec11[i] = (fZec10[i] + 8.50000477f);
				
			}
			/* Vectorizable loop 13 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec29[i] = (fZec28[i] + 8.50000477f);
				
			}
			/* Recursive loop 14 */
			/* Pre code */
			for (int j16 = 0; (j16 < 4); j16 = (j16 + 1)) {
				fRec45_tmp[j16] = fRec45_perm[j16];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec45[i] = (fSlow5 + (0.999899983f * (fRec45[(i - 1)] + float((iSlow4 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j17 = 0; (j17 < 4); j17 = (j17 + 1)) {
				fRec45_perm[j17] = fRec45_tmp[(vsize + j17)];
				
			}
			/* Vectorizable loop 15 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec5[i] = (0.5f * (fSlow6 + fVec2[(i - 1)]));
				
			}
			/* Vectorizable loop 16 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec13[i] = std::floor(fZec11[i]);
				
			}
			/* Recursive loop 17 */
			/* Pre code */
			for (int j30 = 0; (j30 < 4); j30 = (j30 + 1)) {
				fRec47_tmp[j30] = fRec47_perm[j30];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec47[i] = (fSlow10 + (0.999899983f * (fRec47[(i - 1)] + float((iSlow9 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j31 = 0; (j31 < 4); j31 = (j31 + 1)) {
				fRec47_perm[j31] = fRec47_tmp[(vsize + j31)];
				
			}
			/* Vectorizable loop 18 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec31[i] = std::floor(fZec29[i]);
				
			}
			/* Recursive loop 19 */
			/* Pre code */
			for (int j38 = 0; (j38 < 4); j38 = (j38 + 1)) {
				fRec48_tmp[j38] = fRec48_perm[j38];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec48[i] = (fSlow13 + (0.999000013f * (fRec48[(i - 1)] + float((iSlow12 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j39 = 0; (j39 < 4); j39 = (j39 + 1)) {
				fRec48_perm[j39] = fRec48_tmp[(vsize + j39)];
				
			}
			/* Recursive loop 20 */
			/* Pre code */
			for (int j46 = 0; (j46 < 4); j46 = (j46 + 1)) {
				fRec50_tmp[j46] = fRec50_perm[j46];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec50[i] = (fSlow15 + (0.999000013f * (fRec50[(i - 1)] + float((iSlow14 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j47 = 0; (j47 < 4); j47 = (j47 + 1)) {
				fRec50_perm[j47] = fRec50_tmp[(vsize + j47)];
				
			}
			/* Recursive loop 21 */
			/* Pre code */
			for (int j54 = 0; (j54 < 4); j54 = (j54 + 1)) {
				fRec51_tmp[j54] = fRec51_perm[j54];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec51[i] = (fSlow18 + (0.999000013f * (fRec51[(i - 1)] + float((iSlow17 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j55 = 0; (j55 < 4); j55 = (j55 + 1)) {
				fRec51_perm[j55] = fRec51_tmp[(vsize + j55)];
				
			}
			/* Recursive loop 22 */
			/* Pre code */
			for (int j62 = 0; (j62 < 4); j62 = (j62 + 1)) {
				fRec53_tmp[j62] = fRec53_perm[j62];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec53[i] = (fSlow20 + (0.999000013f * (fRec53[(i - 1)] + float((iSlow19 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j63 = 0; (j63 < 4); j63 = (j63 + 1)) {
				fRec53_perm[j63] = fRec53_tmp[(vsize + j63)];
				
			}
			/* Recursive loop 23 */
			/* Pre code */
			for (int j70 = 0; (j70 < 4); j70 = (j70 + 1)) {
				fRec54_tmp[j70] = fRec54_perm[j70];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec54[i] = (fSlow22 + (0.999000013f * (fRec54[(i - 1)] + float((iSlow21 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j71 = 0; (j71 < 4); j71 = (j71 + 1)) {
				fRec54_perm[j71] = fRec54_tmp[(vsize + j71)];
				
			}
			/* Recursive loop 24 */
			/* Pre code */
			for (int j78 = 0; (j78 < 4); j78 = (j78 + 1)) {
				fRec56_tmp[j78] = fRec56_perm[j78];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec56[i] = (fSlow24 + (0.999000013f * (fRec56[(i - 1)] + float((iSlow23 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j79 = 0; (j79 < 4); j79 = (j79 + 1)) {
				fRec56_perm[j79] = fRec56_tmp[(vsize + j79)];
				
			}
			/* Recursive loop 25 */
			/* Pre code */
			for (int j86 = 0; (j86 < 4); j86 = (j86 + 1)) {
				fRec57_tmp[j86] = fRec57_perm[j86];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec57[i] = (fSlow27 + (0.999899983f * (fRec57[(i - 1)] + float((iSlow26 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j87 = 0; (j87 < 4); j87 = (j87 + 1)) {
				fRec57_perm[j87] = fRec57_tmp[(vsize + j87)];
				
			}
			/* Recursive loop 26 */
			/* Pre code */
			for (int j94 = 0; (j94 < 4); j94 = (j94 + 1)) {
				fRec59_tmp[j94] = fRec59_perm[j94];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec59[i] = (fSlow29 + (0.999899983f * (fRec59[(i - 1)] + float((iSlow28 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j95 = 0; (j95 < 4); j95 = (j95 + 1)) {
				fRec59_perm[j95] = fRec59_tmp[(vsize + j95)];
				
			}
			/* Recursive loop 27 */
			/* Pre code */
			for (int j102 = 0; (j102 < 4); j102 = (j102 + 1)) {
				fRec60_tmp[j102] = fRec60_perm[j102];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec60[i] = (fSlow32 + (0.999000013f * (fRec60[(i - 1)] + float((iSlow31 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j103 = 0; (j103 < 4); j103 = (j103 + 1)) {
				fRec60_perm[j103] = fRec60_tmp[(vsize + j103)];
				
			}
			/* Recursive loop 28 */
			/* Pre code */
			for (int j110 = 0; (j110 < 4); j110 = (j110 + 1)) {
				fRec62_tmp[j110] = fRec62_perm[j110];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec62[i] = (fSlow34 + (0.999000013f * (fRec62[(i - 1)] + float((iSlow33 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j111 = 0; (j111 < 4); j111 = (j111 + 1)) {
				fRec62_perm[j111] = fRec62_tmp[(vsize + j111)];
				
			}
			/* Recursive loop 29 */
			/* Pre code */
			for (int j118 = 0; (j118 < 4); j118 = (j118 + 1)) {
				fRec63_tmp[j118] = fRec63_perm[j118];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec63[i] = (fSlow37 + (0.999000013f * (fRec63[(i - 1)] + float((iSlow36 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j119 = 0; (j119 < 4); j119 = (j119 + 1)) {
				fRec63_perm[j119] = fRec63_tmp[(vsize + j119)];
				
			}
			/* Recursive loop 30 */
			/* Pre code */
			for (int j126 = 0; (j126 < 4); j126 = (j126 + 1)) {
				fRec65_tmp[j126] = fRec65_perm[j126];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec65[i] = (fSlow39 + (0.999000013f * (fRec65[(i - 1)] + float((iSlow38 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j127 = 0; (j127 < 4); j127 = (j127 + 1)) {
				fRec65_perm[j127] = fRec65_tmp[(vsize + j127)];
				
			}
			/* Recursive loop 31 */
			/* Pre code */
			for (int j134 = 0; (j134 < 4); j134 = (j134 + 1)) {
				fRec66_tmp[j134] = fRec66_perm[j134];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec66[i] = (fSlow42 + (0.999000013f * (fRec66[(i - 1)] + float((iSlow41 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j135 = 0; (j135 < 4); j135 = (j135 + 1)) {
				fRec66_perm[j135] = fRec66_tmp[(vsize + j135)];
				
			}
			/* Recursive loop 32 */
			/* Pre code */
			for (int j142 = 0; (j142 < 4); j142 = (j142 + 1)) {
				fRec68_tmp[j142] = fRec68_perm[j142];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec68[i] = (fSlow44 + (0.999000013f * (fRec68[(i - 1)] + float((iSlow43 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j143 = 0; (j143 < 4); j143 = (j143 + 1)) {
				fRec68_perm[j143] = fRec68_tmp[(vsize + j143)];
				
			}
			/* Recursive loop 33 */
			/* Pre code */
			for (int j150 = 0; (j150 < 4); j150 = (j150 + 1)) {
				fRec69_tmp[j150] = fRec69_perm[j150];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec69[i] = (fSlow47 + (0.999899983f * (fRec69[(i - 1)] + float((iSlow46 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j151 = 0; (j151 < 4); j151 = (j151 + 1)) {
				fRec69_perm[j151] = fRec69_tmp[(vsize + j151)];
				
			}
			/* Recursive loop 34 */
			/* Pre code */
			for (int j158 = 0; (j158 < 4); j158 = (j158 + 1)) {
				fRec71_tmp[j158] = fRec71_perm[j158];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec71[i] = (fSlow49 + (0.999899983f * (fRec71[(i - 1)] + float((iSlow48 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j159 = 0; (j159 < 4); j159 = (j159 + 1)) {
				fRec71_perm[j159] = fRec71_tmp[(vsize + j159)];
				
			}
			/* Recursive loop 35 */
			/* Pre code */
			for (int j166 = 0; (j166 < 4); j166 = (j166 + 1)) {
				fRec72_tmp[j166] = fRec72_perm[j166];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec72[i] = (fSlow52 + (0.999000013f * (fRec72[(i - 1)] + float((iSlow51 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j167 = 0; (j167 < 4); j167 = (j167 + 1)) {
				fRec72_perm[j167] = fRec72_tmp[(vsize + j167)];
				
			}
			/* Recursive loop 36 */
			/* Pre code */
			for (int j174 = 0; (j174 < 4); j174 = (j174 + 1)) {
				fRec74_tmp[j174] = fRec74_perm[j174];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec74[i] = (fSlow54 + (0.999000013f * (fRec74[(i - 1)] + float((iSlow53 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j175 = 0; (j175 < 4); j175 = (j175 + 1)) {
				fRec74_perm[j175] = fRec74_tmp[(vsize + j175)];
				
			}
			/* Recursive loop 37 */
			/* Pre code */
			for (int j182 = 0; (j182 < 4); j182 = (j182 + 1)) {
				fRec75_tmp[j182] = fRec75_perm[j182];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec75[i] = (fSlow57 + (0.999000013f * (fRec75[(i - 1)] + float((iSlow56 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j183 = 0; (j183 < 4); j183 = (j183 + 1)) {
				fRec75_perm[j183] = fRec75_tmp[(vsize + j183)];
				
			}
			/* Recursive loop 38 */
			/* Pre code */
			for (int j190 = 0; (j190 < 4); j190 = (j190 + 1)) {
				fRec77_tmp[j190] = fRec77_perm[j190];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec77[i] = (fSlow59 + (0.999000013f * (fRec77[(i - 1)] + float((iSlow58 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j191 = 0; (j191 < 4); j191 = (j191 + 1)) {
				fRec77_perm[j191] = fRec77_tmp[(vsize + j191)];
				
			}
			/* Recursive loop 39 */
			/* Pre code */
			for (int j198 = 0; (j198 < 4); j198 = (j198 + 1)) {
				fRec78_tmp[j198] = fRec78_perm[j198];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec78[i] = (fSlow62 + (0.999000013f * (fRec78[(i - 1)] + float((iSlow61 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j199 = 0; (j199 < 4); j199 = (j199 + 1)) {
				fRec78_perm[j199] = fRec78_tmp[(vsize + j199)];
				
			}
			/* Recursive loop 40 */
			/* Pre code */
			for (int j206 = 0; (j206 < 4); j206 = (j206 + 1)) {
				fRec80_tmp[j206] = fRec80_perm[j206];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec80[i] = (fSlow64 + (0.999000013f * (fRec80[(i - 1)] + float((iSlow63 * iZec4[i])))));
				
			}
			/* Post code */
			for (int j207 = 0; (j207 < 4); j207 = (j207 + 1)) {
				fRec80_perm[j207] = fRec80_tmp[(vsize + j207)];
				
			}
			/* Vectorizable loop 41 */
			/* Pre code */
			for (int j214 = 0; (j214 < 4); j214 = (j214 + 1)) {
				fVec5_tmp[j214] = fVec5_perm[j214];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fVec5[i] = fSlow65;
				
			}
			/* Post code */
			for (int j215 = 0; (j215 < 4); j215 = (j215 + 1)) {
				fVec5_perm[j215] = fVec5_tmp[(vsize + j215)];
				
			}
			/* Recursive loop 42 */
			/* Pre code */
			for (int j0 = 0; (j0 < 4); j0 = (j0 + 1)) {
				fRec39_tmp[j0] = fRec39_perm[j0];
				
			}
			for (int j2 = 0; (j2 < 4); j2 = (j2 + 1)) {
				fRec40_tmp[j2] = fRec40_perm[j2];
				
			}
			for (int j4 = 0; (j4 < 4); j4 = (j4 + 1)) {
				fRec41_tmp[j4] = fRec41_perm[j4];
				
			}
			for (int j6 = 0; (j6 < 4); j6 = (j6 + 1)) {
				fRec42_tmp[j6] = fRec42_perm[j6];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec0[i] = ((fRec39[(i - 1)] != 0.0f)?(((fRec40[(i - 1)] > 0.0f) & (fRec40[(i - 1)] < 1.0f))?fRec39[(i - 1)]:0.0f):(((fRec40[(i - 1)] == 0.0f) & (fSlow0 != fRec41[(i - 1)]))?4.53514731e-05f:(((fRec40[(i - 1)] == 1.0f) & (fSlow0 != fRec42[(i - 1)]))?-4.53514731e-05f:0.0f)));
				fRec39[i] = fZec0[i];
				fRec40[i] = std::max<float>(0.0f, std::min<float>(1.0f, (fRec40[(i - 1)] + fZec0[i])));
				fRec41[i] = (((fRec40[(i - 1)] >= 1.0f) & (fRec42[(i - 1)] != fSlow0))?fSlow0:fRec41[(i - 1)]);
				fRec42[i] = (((fRec40[(i - 1)] <= 0.0f) & (fRec41[(i - 1)] != fSlow0))?fSlow0:fRec42[(i - 1)]);
				
			}
			/* Post code */
			for (int j1 = 0; (j1 < 4); j1 = (j1 + 1)) {
				fRec39_perm[j1] = fRec39_tmp[(vsize + j1)];
				
			}
			for (int j3 = 0; (j3 < 4); j3 = (j3 + 1)) {
				fRec40_perm[j3] = fRec40_tmp[(vsize + j3)];
				
			}
			for (int j5 = 0; (j5 < 4); j5 = (j5 + 1)) {
				fRec41_perm[j5] = fRec41_tmp[(vsize + j5)];
				
			}
			for (int j7 = 0; (j7 < 4); j7 = (j7 + 1)) {
				fRec42_perm[j7] = fRec42_tmp[(vsize + j7)];
				
			}
			/* Vectorizable loop 43 */
			/* Pre code */
			for (int j20 = 0; (j20 < 4); j20 = (j20 + 1)) {
				fVec3_tmp[j20] = fVec3_perm[j20];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fVec3[i] = fSlow7;
				
			}
			/* Post code */
			for (int j21 = 0; (j21 < 4); j21 = (j21 + 1)) {
				fVec3_perm[j21] = fVec3_tmp[(vsize + j21)];
				
			}
			/* Vectorizable loop 44 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec14[i] = (fZec10[i] + (9.0f - fZec13[i]));
				
			}
			/* Vectorizable loop 45 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec15[i] = (fZec10[i] + (8.0f - fZec13[i]));
				
			}
			/* Vectorizable loop 46 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec26[i] = (fRec45[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 47 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec32[i] = (fZec28[i] + (9.0f - fZec31[i]));
				
			}
			/* Vectorizable loop 48 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec33[i] = (fZec28[i] + (8.0f - fZec31[i]));
				
			}
			/* Vectorizable loop 49 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec41[i] = (fRec47[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 50 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec43[i] = (fRec48[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 51 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec45[i] = (fRec50[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 52 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec47[i] = (fRec51[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 53 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec49[i] = (fRec53[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 54 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec51[i] = (fRec54[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 55 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec53[i] = (fRec56[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 56 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec55[i] = (0.0f - fZec5[i]);
				
			}
			/* Vectorizable loop 57 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec62[i] = (fRec57[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 58 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec68[i] = (fRec59[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 59 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec70[i] = (fRec60[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 60 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec72[i] = (fRec62[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 61 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec74[i] = (fRec63[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 62 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec76[i] = (fRec65[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 63 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec78[i] = (fRec66[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 64 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec80[i] = (fRec68[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 65 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec86[i] = (fRec69[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 66 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec92[i] = (fRec71[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 67 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec94[i] = (fRec72[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 68 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec96[i] = (fRec74[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 69 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec98[i] = (fRec75[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 70 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec100[i] = (fRec77[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 71 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec102[i] = (fRec78[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 72 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec104[i] = (fRec80[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 73 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec106[i] = (fSlow65 + fVec5[(i - 1)]);
				
			}
			/* Vectorizable loop 74 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec6[i] = std::cos(fZec5[i]);
				
			}
			/* Vectorizable loop 75 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec7[i] = (fSlow7 + fVec3[(i - 1)]);
				
			}
			/* Vectorizable loop 76 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec8[i] = (1.0f - fRec40[i]);
				
			}
			/* Vectorizable loop 77 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec12[i] = int(fZec11[i]);
				
			}
			/* Vectorizable loop 78 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec16[i] = (fZec10[i] + (7.0f - fZec13[i]));
				
			}
			/* Vectorizable loop 79 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec17[i] = (fZec10[i] + (6.0f - fZec13[i]));
				
			}
			/* Vectorizable loop 80 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec18[i] = (fZec14[i] * fZec15[i]);
				
			}
			/* Vectorizable loop 81 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec22[i] = std::sin(fZec5[i]);
				
			}
			/* Vectorizable loop 82 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec19[i] = int(std::min<float>(65536.0f, std::max<float>(0.0f, fRec41[i])));
				
			}
			/* Vectorizable loop 83 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec20[i] = int(std::min<float>(65536.0f, std::max<float>(0.0f, fRec42[i])));
				
			}
			/* Vectorizable loop 84 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec36[i] = (fZec32[i] * fZec33[i]);
				
			}
			/* Vectorizable loop 85 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec27[i] = std::floor(fZec26[i]);
				
			}
			/* Vectorizable loop 86 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec30[i] = int(fZec29[i]);
				
			}
			/* Vectorizable loop 87 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec34[i] = (fZec28[i] + (7.0f - fZec31[i]));
				
			}
			/* Vectorizable loop 88 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec35[i] = (fZec28[i] + (6.0f - fZec31[i]));
				
			}
			/* Vectorizable loop 89 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec42[i] = std::floor(fZec41[i]);
				
			}
			/* Vectorizable loop 90 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec44[i] = std::floor(fZec43[i]);
				
			}
			/* Vectorizable loop 91 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec46[i] = std::floor(fZec45[i]);
				
			}
			/* Vectorizable loop 92 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec48[i] = std::floor(fZec47[i]);
				
			}
			/* Vectorizable loop 93 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec50[i] = std::floor(fZec49[i]);
				
			}
			/* Vectorizable loop 94 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec52[i] = std::floor(fZec51[i]);
				
			}
			/* Vectorizable loop 95 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec54[i] = std::floor(fZec53[i]);
				
			}
			/* Vectorizable loop 96 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec56[i] = std::cos(fZec55[i]);
				
			}
			/* Vectorizable loop 97 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec58[i] = std::sin(fZec55[i]);
				
			}
			/* Vectorizable loop 98 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec63[i] = std::floor(fZec62[i]);
				
			}
			/* Vectorizable loop 99 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec69[i] = std::floor(fZec68[i]);
				
			}
			/* Vectorizable loop 100 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec71[i] = std::floor(fZec70[i]);
				
			}
			/* Vectorizable loop 101 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec73[i] = std::floor(fZec72[i]);
				
			}
			/* Vectorizable loop 102 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec75[i] = std::floor(fZec74[i]);
				
			}
			/* Vectorizable loop 103 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec77[i] = std::floor(fZec76[i]);
				
			}
			/* Vectorizable loop 104 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec79[i] = std::floor(fZec78[i]);
				
			}
			/* Vectorizable loop 105 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec81[i] = std::floor(fZec80[i]);
				
			}
			/* Vectorizable loop 106 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec87[i] = std::floor(fZec86[i]);
				
			}
			/* Vectorizable loop 107 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec93[i] = std::floor(fZec92[i]);
				
			}
			/* Vectorizable loop 108 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec95[i] = std::floor(fZec94[i]);
				
			}
			/* Vectorizable loop 109 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec97[i] = std::floor(fZec96[i]);
				
			}
			/* Vectorizable loop 110 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec99[i] = std::floor(fZec98[i]);
				
			}
			/* Vectorizable loop 111 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec101[i] = std::floor(fZec100[i]);
				
			}
			/* Vectorizable loop 112 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec103[i] = std::floor(fZec102[i]);
				
			}
			/* Vectorizable loop 113 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec105[i] = std::floor(fZec104[i]);
				
			}
			/* Vectorizable loop 114 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec107[i] = (1.0f - (0.5f * fZec106[i]));
				
			}
			/* Recursive loop 115 */
			/* Pre code */
			fYec0_idx = ((fYec0_idx + fYec0_idx_save) & 131071);
			fYec1_idx = ((fYec1_idx + fYec1_idx_save) & 16383);
			for (int j24 = 0; (j24 < 4); j24 = (j24 + 1)) {
				fYec2_tmp[j24] = fYec2_perm[j24];
				
			}
			for (int j26 = 0; (j26 < 4); j26 = (j26 + 1)) {
				fRec38_tmp[j26] = fRec38_perm[j26];
				
			}
			for (int j28 = 0; (j28 < 4); j28 = (j28 + 1)) {
				fRec36_tmp[j28] = fRec36_perm[j28];
				
			}
			fYec3_idx = ((fYec3_idx + fYec3_idx_save) & 131071);
			fYec4_idx = ((fYec4_idx + fYec4_idx_save) & 16383);
			for (int j32 = 0; (j32 < 4); j32 = (j32 + 1)) {
				fYec5_tmp[j32] = fYec5_perm[j32];
				
			}
			for (int j34 = 0; (j34 < 4); j34 = (j34 + 1)) {
				fRec46_tmp[j34] = fRec46_perm[j34];
				
			}
			for (int j36 = 0; (j36 < 4); j36 = (j36 + 1)) {
				fRec37_tmp[j36] = fRec37_perm[j36];
				
			}
			fYec6_idx = ((fYec6_idx + fYec6_idx_save) & 16383);
			for (int j40 = 0; (j40 < 4); j40 = (j40 + 1)) {
				fYec7_tmp[j40] = fYec7_perm[j40];
				
			}
			for (int j42 = 0; (j42 < 4); j42 = (j42 + 1)) {
				fRec35_tmp[j42] = fRec35_perm[j42];
				
			}
			for (int j44 = 0; (j44 < 4); j44 = (j44 + 1)) {
				fRec33_tmp[j44] = fRec33_perm[j44];
				
			}
			fYec8_idx = ((fYec8_idx + fYec8_idx_save) & 16383);
			for (int j48 = 0; (j48 < 4); j48 = (j48 + 1)) {
				fYec9_tmp[j48] = fYec9_perm[j48];
				
			}
			for (int j50 = 0; (j50 < 4); j50 = (j50 + 1)) {
				fRec49_tmp[j50] = fRec49_perm[j50];
				
			}
			for (int j52 = 0; (j52 < 4); j52 = (j52 + 1)) {
				fRec34_tmp[j52] = fRec34_perm[j52];
				
			}
			fYec10_idx = ((fYec10_idx + fYec10_idx_save) & 16383);
			for (int j56 = 0; (j56 < 4); j56 = (j56 + 1)) {
				fYec11_tmp[j56] = fYec11_perm[j56];
				
			}
			for (int j58 = 0; (j58 < 4); j58 = (j58 + 1)) {
				fRec32_tmp[j58] = fRec32_perm[j58];
				
			}
			for (int j60 = 0; (j60 < 4); j60 = (j60 + 1)) {
				fRec30_tmp[j60] = fRec30_perm[j60];
				
			}
			fYec12_idx = ((fYec12_idx + fYec12_idx_save) & 16383);
			for (int j64 = 0; (j64 < 4); j64 = (j64 + 1)) {
				fYec13_tmp[j64] = fYec13_perm[j64];
				
			}
			for (int j66 = 0; (j66 < 4); j66 = (j66 + 1)) {
				fRec52_tmp[j66] = fRec52_perm[j66];
				
			}
			for (int j68 = 0; (j68 < 4); j68 = (j68 + 1)) {
				fRec31_tmp[j68] = fRec31_perm[j68];
				
			}
			fYec14_idx = ((fYec14_idx + fYec14_idx_save) & 16383);
			for (int j72 = 0; (j72 < 4); j72 = (j72 + 1)) {
				fYec15_tmp[j72] = fYec15_perm[j72];
				
			}
			for (int j74 = 0; (j74 < 4); j74 = (j74 + 1)) {
				fRec29_tmp[j74] = fRec29_perm[j74];
				
			}
			for (int j76 = 0; (j76 < 4); j76 = (j76 + 1)) {
				fRec27_tmp[j76] = fRec27_perm[j76];
				
			}
			fYec16_idx = ((fYec16_idx + fYec16_idx_save) & 16383);
			for (int j80 = 0; (j80 < 4); j80 = (j80 + 1)) {
				fYec17_tmp[j80] = fYec17_perm[j80];
				
			}
			for (int j82 = 0; (j82 < 4); j82 = (j82 + 1)) {
				fRec55_tmp[j82] = fRec55_perm[j82];
				
			}
			for (int j84 = 0; (j84 < 4); j84 = (j84 + 1)) {
				fRec28_tmp[j84] = fRec28_perm[j84];
				
			}
			fYec18_idx = ((fYec18_idx + fYec18_idx_save) & 16383);
			for (int j88 = 0; (j88 < 4); j88 = (j88 + 1)) {
				fYec19_tmp[j88] = fYec19_perm[j88];
				
			}
			for (int j90 = 0; (j90 < 4); j90 = (j90 + 1)) {
				fRec26_tmp[j90] = fRec26_perm[j90];
				
			}
			for (int j92 = 0; (j92 < 4); j92 = (j92 + 1)) {
				fRec24_tmp[j92] = fRec24_perm[j92];
				
			}
			fYec20_idx = ((fYec20_idx + fYec20_idx_save) & 16383);
			for (int j96 = 0; (j96 < 4); j96 = (j96 + 1)) {
				fYec21_tmp[j96] = fYec21_perm[j96];
				
			}
			for (int j98 = 0; (j98 < 4); j98 = (j98 + 1)) {
				fRec58_tmp[j98] = fRec58_perm[j98];
				
			}
			for (int j100 = 0; (j100 < 4); j100 = (j100 + 1)) {
				fRec25_tmp[j100] = fRec25_perm[j100];
				
			}
			fYec22_idx = ((fYec22_idx + fYec22_idx_save) & 16383);
			for (int j104 = 0; (j104 < 4); j104 = (j104 + 1)) {
				fYec23_tmp[j104] = fYec23_perm[j104];
				
			}
			for (int j106 = 0; (j106 < 4); j106 = (j106 + 1)) {
				fRec23_tmp[j106] = fRec23_perm[j106];
				
			}
			for (int j108 = 0; (j108 < 4); j108 = (j108 + 1)) {
				fRec21_tmp[j108] = fRec21_perm[j108];
				
			}
			fYec24_idx = ((fYec24_idx + fYec24_idx_save) & 16383);
			for (int j112 = 0; (j112 < 4); j112 = (j112 + 1)) {
				fYec25_tmp[j112] = fYec25_perm[j112];
				
			}
			for (int j114 = 0; (j114 < 4); j114 = (j114 + 1)) {
				fRec61_tmp[j114] = fRec61_perm[j114];
				
			}
			for (int j116 = 0; (j116 < 4); j116 = (j116 + 1)) {
				fRec22_tmp[j116] = fRec22_perm[j116];
				
			}
			fYec26_idx = ((fYec26_idx + fYec26_idx_save) & 16383);
			for (int j120 = 0; (j120 < 4); j120 = (j120 + 1)) {
				fYec27_tmp[j120] = fYec27_perm[j120];
				
			}
			for (int j122 = 0; (j122 < 4); j122 = (j122 + 1)) {
				fRec20_tmp[j122] = fRec20_perm[j122];
				
			}
			for (int j124 = 0; (j124 < 4); j124 = (j124 + 1)) {
				fRec18_tmp[j124] = fRec18_perm[j124];
				
			}
			fYec28_idx = ((fYec28_idx + fYec28_idx_save) & 16383);
			for (int j128 = 0; (j128 < 4); j128 = (j128 + 1)) {
				fYec29_tmp[j128] = fYec29_perm[j128];
				
			}
			for (int j130 = 0; (j130 < 4); j130 = (j130 + 1)) {
				fRec64_tmp[j130] = fRec64_perm[j130];
				
			}
			for (int j132 = 0; (j132 < 4); j132 = (j132 + 1)) {
				fRec19_tmp[j132] = fRec19_perm[j132];
				
			}
			fYec30_idx = ((fYec30_idx + fYec30_idx_save) & 16383);
			for (int j136 = 0; (j136 < 4); j136 = (j136 + 1)) {
				fYec31_tmp[j136] = fYec31_perm[j136];
				
			}
			for (int j138 = 0; (j138 < 4); j138 = (j138 + 1)) {
				fRec17_tmp[j138] = fRec17_perm[j138];
				
			}
			for (int j140 = 0; (j140 < 4); j140 = (j140 + 1)) {
				fRec15_tmp[j140] = fRec15_perm[j140];
				
			}
			fYec32_idx = ((fYec32_idx + fYec32_idx_save) & 16383);
			for (int j144 = 0; (j144 < 4); j144 = (j144 + 1)) {
				fYec33_tmp[j144] = fYec33_perm[j144];
				
			}
			for (int j146 = 0; (j146 < 4); j146 = (j146 + 1)) {
				fRec67_tmp[j146] = fRec67_perm[j146];
				
			}
			for (int j148 = 0; (j148 < 4); j148 = (j148 + 1)) {
				fRec16_tmp[j148] = fRec16_perm[j148];
				
			}
			fYec34_idx = ((fYec34_idx + fYec34_idx_save) & 16383);
			for (int j152 = 0; (j152 < 4); j152 = (j152 + 1)) {
				fYec35_tmp[j152] = fYec35_perm[j152];
				
			}
			for (int j154 = 0; (j154 < 4); j154 = (j154 + 1)) {
				fRec14_tmp[j154] = fRec14_perm[j154];
				
			}
			for (int j156 = 0; (j156 < 4); j156 = (j156 + 1)) {
				fRec12_tmp[j156] = fRec12_perm[j156];
				
			}
			fYec36_idx = ((fYec36_idx + fYec36_idx_save) & 16383);
			for (int j160 = 0; (j160 < 4); j160 = (j160 + 1)) {
				fYec37_tmp[j160] = fYec37_perm[j160];
				
			}
			for (int j162 = 0; (j162 < 4); j162 = (j162 + 1)) {
				fRec70_tmp[j162] = fRec70_perm[j162];
				
			}
			for (int j164 = 0; (j164 < 4); j164 = (j164 + 1)) {
				fRec13_tmp[j164] = fRec13_perm[j164];
				
			}
			fYec38_idx = ((fYec38_idx + fYec38_idx_save) & 16383);
			for (int j168 = 0; (j168 < 4); j168 = (j168 + 1)) {
				fYec39_tmp[j168] = fYec39_perm[j168];
				
			}
			for (int j170 = 0; (j170 < 4); j170 = (j170 + 1)) {
				fRec11_tmp[j170] = fRec11_perm[j170];
				
			}
			for (int j172 = 0; (j172 < 4); j172 = (j172 + 1)) {
				fRec9_tmp[j172] = fRec9_perm[j172];
				
			}
			fYec40_idx = ((fYec40_idx + fYec40_idx_save) & 16383);
			for (int j176 = 0; (j176 < 4); j176 = (j176 + 1)) {
				fYec41_tmp[j176] = fYec41_perm[j176];
				
			}
			for (int j178 = 0; (j178 < 4); j178 = (j178 + 1)) {
				fRec73_tmp[j178] = fRec73_perm[j178];
				
			}
			for (int j180 = 0; (j180 < 4); j180 = (j180 + 1)) {
				fRec10_tmp[j180] = fRec10_perm[j180];
				
			}
			fYec42_idx = ((fYec42_idx + fYec42_idx_save) & 16383);
			for (int j184 = 0; (j184 < 4); j184 = (j184 + 1)) {
				fYec43_tmp[j184] = fYec43_perm[j184];
				
			}
			for (int j186 = 0; (j186 < 4); j186 = (j186 + 1)) {
				fRec8_tmp[j186] = fRec8_perm[j186];
				
			}
			for (int j188 = 0; (j188 < 4); j188 = (j188 + 1)) {
				fRec6_tmp[j188] = fRec6_perm[j188];
				
			}
			fYec44_idx = ((fYec44_idx + fYec44_idx_save) & 16383);
			for (int j192 = 0; (j192 < 4); j192 = (j192 + 1)) {
				fYec45_tmp[j192] = fYec45_perm[j192];
				
			}
			for (int j194 = 0; (j194 < 4); j194 = (j194 + 1)) {
				fRec76_tmp[j194] = fRec76_perm[j194];
				
			}
			for (int j196 = 0; (j196 < 4); j196 = (j196 + 1)) {
				fRec7_tmp[j196] = fRec7_perm[j196];
				
			}
			fYec46_idx = ((fYec46_idx + fYec46_idx_save) & 16383);
			for (int j200 = 0; (j200 < 4); j200 = (j200 + 1)) {
				fYec47_tmp[j200] = fYec47_perm[j200];
				
			}
			for (int j202 = 0; (j202 < 4); j202 = (j202 + 1)) {
				fRec5_tmp[j202] = fRec5_perm[j202];
				
			}
			for (int j204 = 0; (j204 < 4); j204 = (j204 + 1)) {
				fRec3_tmp[j204] = fRec3_perm[j204];
				
			}
			fYec48_idx = ((fYec48_idx + fYec48_idx_save) & 16383);
			for (int j208 = 0; (j208 < 4); j208 = (j208 + 1)) {
				fYec49_tmp[j208] = fYec49_perm[j208];
				
			}
			for (int j210 = 0; (j210 < 4); j210 = (j210 + 1)) {
				fRec79_tmp[j210] = fRec79_perm[j210];
				
			}
			for (int j212 = 0; (j212 < 4); j212 = (j212 + 1)) {
				fRec4_tmp[j212] = fRec4_perm[j212];
				
			}
			for (int j216 = 0; (j216 < 4); j216 = (j216 + 1)) {
				fRec2_tmp[j216] = fRec2_perm[j216];
				
			}
			fRec0_idx = ((fRec0_idx + fRec0_idx_save) & 1023);
			for (int j218 = 0; (j218 < 4); j218 = (j218 + 1)) {
				fRec81_tmp[j218] = fRec81_perm[j218];
				
			}
			fRec1_idx = ((fRec1_idx + fRec1_idx_save) & 1023);
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fYec0[((i + fYec0_idx) & 131071)] = (((((fRec1[(((i + fRec1_idx) - (std::min<int>(512, std::max<int>(0, iZec12[i])) + 1)) & 1023)] * (0.0f - fZec14[i])) * (0.0f - (0.5f * fZec15[i]))) * (0.0f - (0.333333343f * fZec16[i]))) * (0.0f - (0.25f * fZec17[i]))) + ((fZec10[i] + (10.0f - fZec13[i])) * ((((((fRec1[(((i + fRec1_idx) - (std::min<int>(512, std::max<int>(0, (iZec12[i] + 1))) + 1)) & 1023)] * (0.0f - fZec15[i])) * (0.0f - (0.5f * fZec16[i]))) * (0.0f - (0.333333343f * fZec17[i]))) + (0.5f * (((fZec14[i] * fRec1[(((i + fRec1_idx) - (std::min<int>(512, std::max<int>(0, (iZec12[i] + 2))) + 1)) & 1023)]) * (0.0f - fZec16[i])) * (0.0f - (0.5f * fZec17[i]))))) + (0.166666672f * ((fZec18[i] * fRec1[(((i + fRec1_idx) - (std::min<int>(512, std::max<int>(0, (iZec12[i] + 3))) + 1)) & 1023)]) * (0.0f - fZec17[i])))) + (0.0416666679f * ((fZec18[i] * fZec16[i]) * fRec1[(((i + fRec1_idx) - (std::min<int>(512, std::max<int>(0, (iZec12[i] + 4))) + 1)) & 1023)])))));
				fZec21[i] = ((0.5f * (fZec7[i] * ((fZec8[i] * fYec0[(((i + fYec0_idx) - iZec19[i]) & 131071)]) + (fRec40[i] * fYec0[(((i + fYec0_idx) - iZec20[i]) & 131071)])))) + float(input1[i]));
				fZec23[i] = ((fZec6[i] * fZec21[i]) - (fZec22[i] * fRec28[(i - 1)]));
				fZec24[i] = ((fZec6[i] * fZec23[i]) - (fZec22[i] * fRec31[(i - 1)]));
				fZec25[i] = ((fZec6[i] * fZec24[i]) - (fZec22[i] * fRec34[(i - 1)]));
				fYec1[((i + fYec1_idx) & 16383)] = (0.0f - ((fZec6[i] * fZec25[i]) - (fZec22[i] * fRec37[(i - 1)])));
				fYec2[i] = fYec1[(((i + fYec1_idx) - std::min<int>(8192, std::max<int>(0, int(fZec26[i])))) & 16383)];
				fRec38[i] = (fYec2[(i - 1)] - (((fZec27[i] + (2.0f - fRec45[i])) * (fRec38[(i - 1)] - fYec2[i])) / (fRec45[i] - fZec27[i])));
				fRec36[i] = fRec38[i];
				fYec3[((i + fYec3_idx) & 131071)] = (((((fRec0[(((i + fRec0_idx) - (std::min<int>(512, std::max<int>(0, iZec30[i])) + 1)) & 1023)] * (0.0f - fZec32[i])) * (0.0f - (0.5f * fZec33[i]))) * (0.0f - (0.333333343f * fZec34[i]))) * (0.0f - (0.25f * fZec35[i]))) + ((fZec28[i] + (10.0f - fZec31[i])) * ((((((fRec0[(((i + fRec0_idx) - (std::min<int>(512, std::max<int>(0, (iZec30[i] + 1))) + 1)) & 1023)] * (0.0f - fZec33[i])) * (0.0f - (0.5f * fZec34[i]))) * (0.0f - (0.333333343f * fZec35[i]))) + (0.5f * (((fZec32[i] * fRec0[(((i + fRec0_idx) - (std::min<int>(512, std::max<int>(0, (iZec30[i] + 2))) + 1)) & 1023)]) * (0.0f - fZec34[i])) * (0.0f - (0.5f * fZec35[i]))))) + (0.166666672f * ((fZec36[i] * fRec0[(((i + fRec0_idx) - (std::min<int>(512, std::max<int>(0, (iZec30[i] + 3))) + 1)) & 1023)]) * (0.0f - fZec35[i])))) + (0.0416666679f * ((fZec36[i] * fZec34[i]) * fRec0[(((i + fRec0_idx) - (std::min<int>(512, std::max<int>(0, (iZec30[i] + 4))) + 1)) & 1023)])))));
				fZec37[i] = ((0.5f * (((fZec8[i] * fYec3[(((i + fYec3_idx) - iZec19[i]) & 131071)]) + (fRec40[i] * fYec3[(((i + fYec3_idx) - iZec20[i]) & 131071)])) * fZec7[i])) + float(input0[i]));
				fZec38[i] = ((fZec37[i] * fZec6[i]) - (fZec22[i] * fRec27[(i - 1)]));
				fZec39[i] = ((fZec6[i] * fZec38[i]) - (fZec22[i] * fRec30[(i - 1)]));
				fZec40[i] = ((fZec6[i] * fZec39[i]) - (fZec22[i] * fRec33[(i - 1)]));
				fYec4[((i + fYec4_idx) & 16383)] = ((fZec6[i] * fZec40[i]) - (fZec22[i] * fRec36[(i - 1)]));
				fYec5[i] = fYec4[(((i + fYec4_idx) - std::min<int>(8192, std::max<int>(0, int(fZec41[i])))) & 16383)];
				fRec46[i] = (fYec5[(i - 1)] - (((fZec42[i] + (2.0f - fRec47[i])) * (fRec46[(i - 1)] - fYec5[i])) / (fRec47[i] - fZec42[i])));
				fRec37[i] = fRec46[i];
				fYec6[((i + fYec6_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec37[(i - 1)]) + (fZec22[i] * fZec25[i])));
				fYec7[i] = fYec6[(((i + fYec6_idx) - std::min<int>(8192, std::max<int>(0, int(fZec43[i])))) & 16383)];
				fRec35[i] = (fYec7[(i - 1)] - (((fZec44[i] + (2.0f - fRec48[i])) * (fRec35[(i - 1)] - fYec7[i])) / (fRec48[i] - fZec44[i])));
				fRec33[i] = fRec35[i];
				fYec8[((i + fYec8_idx) & 16383)] = ((fRec36[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec40[i]));
				fYec9[i] = fYec8[(((i + fYec8_idx) - std::min<int>(8192, std::max<int>(0, int(fZec45[i])))) & 16383)];
				fRec49[i] = (fYec9[(i - 1)] - (((fZec46[i] + (2.0f - fRec50[i])) * (fRec49[(i - 1)] - fYec9[i])) / (fRec50[i] - fZec46[i])));
				fRec34[i] = fRec49[i];
				fYec10[((i + fYec10_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec34[(i - 1)]) + (fZec22[i] * fZec24[i])));
				fYec11[i] = fYec10[(((i + fYec10_idx) - std::min<int>(8192, std::max<int>(0, int(fZec47[i])))) & 16383)];
				fRec32[i] = (fYec11[(i - 1)] - (((fZec48[i] + (2.0f - fRec51[i])) * (fRec32[(i - 1)] - fYec11[i])) / (fRec51[i] - fZec48[i])));
				fRec30[i] = fRec32[i];
				fYec12[((i + fYec12_idx) & 16383)] = ((fRec33[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec39[i]));
				fYec13[i] = fYec12[(((i + fYec12_idx) - std::min<int>(8192, std::max<int>(0, int(fZec49[i])))) & 16383)];
				fRec52[i] = (fYec13[(i - 1)] - (((fZec50[i] + (2.0f - fRec53[i])) * (fRec52[(i - 1)] - fYec13[i])) / (fRec53[i] - fZec50[i])));
				fRec31[i] = fRec52[i];
				fYec14[((i + fYec14_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec31[(i - 1)]) + (fZec22[i] * fZec23[i])));
				fYec15[i] = fYec14[(((i + fYec14_idx) - std::min<int>(8192, std::max<int>(0, int(fZec51[i])))) & 16383)];
				fRec29[i] = (fYec15[(i - 1)] - (((fZec52[i] + (2.0f - fRec54[i])) * (fRec29[(i - 1)] - fYec15[i])) / (fRec54[i] - fZec52[i])));
				fRec27[i] = fRec29[i];
				fYec16[((i + fYec16_idx) & 16383)] = ((fRec30[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec38[i]));
				fYec17[i] = fYec16[(((i + fYec16_idx) - std::min<int>(8192, std::max<int>(0, int(fZec53[i])))) & 16383)];
				fRec55[i] = (fYec17[(i - 1)] - (((fZec54[i] + (2.0f - fRec56[i])) * (fRec55[(i - 1)] - fYec17[i])) / (fRec56[i] - fZec54[i])));
				fRec28[i] = fRec55[i];
				fZec57[i] = ((fZec6[i] * fRec28[(i - 1)]) + (fZec22[i] * fZec21[i]));
				fZec59[i] = ((fZec56[i] * fZec57[i]) - (fZec58[i] * fRec16[(i - 1)]));
				fZec60[i] = ((fZec56[i] * fZec59[i]) - (fZec58[i] * fRec19[(i - 1)]));
				fZec61[i] = ((fZec56[i] * fZec60[i]) - (fZec58[i] * fRec22[(i - 1)]));
				fYec18[((i + fYec18_idx) & 16383)] = (0.0f - ((fZec56[i] * fZec61[i]) - (fZec58[i] * fRec25[(i - 1)])));
				fYec19[i] = fYec18[(((i + fYec18_idx) - std::min<int>(8192, std::max<int>(0, int(fZec62[i])))) & 16383)];
				fRec26[i] = (fYec19[(i - 1)] - (((fZec63[i] + (2.0f - fRec57[i])) * (fRec26[(i - 1)] - fYec19[i])) / (fRec57[i] - fZec63[i])));
				fRec24[i] = fRec26[i];
				fZec64[i] = ((fRec27[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec37[i]));
				fZec65[i] = ((fZec64[i] * fZec56[i]) - (fZec58[i] * fRec15[(i - 1)]));
				fZec66[i] = ((fZec56[i] * fZec65[i]) - (fZec58[i] * fRec18[(i - 1)]));
				fZec67[i] = ((fZec56[i] * fZec66[i]) - (fZec58[i] * fRec21[(i - 1)]));
				fYec20[((i + fYec20_idx) & 16383)] = ((fZec56[i] * fZec67[i]) - (fRec24[(i - 1)] * fZec58[i]));
				fYec21[i] = fYec20[(((i + fYec20_idx) - std::min<int>(8192, std::max<int>(0, int(fZec68[i])))) & 16383)];
				fRec58[i] = (fYec21[(i - 1)] - (((fZec69[i] + (2.0f - fRec59[i])) * (fRec58[(i - 1)] - fYec21[i])) / (fRec59[i] - fZec69[i])));
				fRec25[i] = fRec58[i];
				fYec22[((i + fYec22_idx) & 16383)] = (0.0f - ((fZec56[i] * fRec25[(i - 1)]) + (fZec58[i] * fZec61[i])));
				fYec23[i] = fYec22[(((i + fYec22_idx) - std::min<int>(8192, std::max<int>(0, int(fZec70[i])))) & 16383)];
				fRec23[i] = (fYec23[(i - 1)] - (((fZec71[i] + (2.0f - fRec60[i])) * (fRec23[(i - 1)] - fYec23[i])) / (fRec60[i] - fZec71[i])));
				fRec21[i] = fRec23[i];
				fYec24[((i + fYec24_idx) & 16383)] = ((fRec24[(i - 1)] * fZec56[i]) + (fZec58[i] * fZec67[i]));
				fYec25[i] = fYec24[(((i + fYec24_idx) - std::min<int>(8192, std::max<int>(0, int(fZec72[i])))) & 16383)];
				fRec61[i] = (fYec25[(i - 1)] - (((fZec73[i] + (2.0f - fRec62[i])) * (fRec61[(i - 1)] - fYec25[i])) / (fRec62[i] - fZec73[i])));
				fRec22[i] = fRec61[i];
				fYec26[((i + fYec26_idx) & 16383)] = (0.0f - ((fZec56[i] * fRec22[(i - 1)]) + (fZec58[i] * fZec60[i])));
				fYec27[i] = fYec26[(((i + fYec26_idx) - std::min<int>(8192, std::max<int>(0, int(fZec74[i])))) & 16383)];
				fRec20[i] = (fYec27[(i - 1)] - (((fZec75[i] + (2.0f - fRec63[i])) * (fRec20[(i - 1)] - fYec27[i])) / (fRec63[i] - fZec75[i])));
				fRec18[i] = fRec20[i];
				fYec28[((i + fYec28_idx) & 16383)] = ((fRec21[(i - 1)] * fZec56[i]) + (fZec58[i] * fZec66[i]));
				fYec29[i] = fYec28[(((i + fYec28_idx) - std::min<int>(8192, std::max<int>(0, int(fZec76[i])))) & 16383)];
				fRec64[i] = (fYec29[(i - 1)] - (((fZec77[i] + (2.0f - fRec65[i])) * (fRec64[(i - 1)] - fYec29[i])) / (fRec65[i] - fZec77[i])));
				fRec19[i] = fRec64[i];
				fYec30[((i + fYec30_idx) & 16383)] = (0.0f - ((fZec56[i] * fRec19[(i - 1)]) + (fZec58[i] * fZec59[i])));
				fYec31[i] = fYec30[(((i + fYec30_idx) - std::min<int>(8192, std::max<int>(0, int(fZec78[i])))) & 16383)];
				fRec17[i] = (fYec31[(i - 1)] - (((fZec79[i] + (2.0f - fRec66[i])) * (fRec17[(i - 1)] - fYec31[i])) / (fRec66[i] - fZec79[i])));
				fRec15[i] = fRec17[i];
				fYec32[((i + fYec32_idx) & 16383)] = ((fRec18[(i - 1)] * fZec56[i]) + (fZec58[i] * fZec65[i]));
				fYec33[i] = fYec32[(((i + fYec32_idx) - std::min<int>(8192, std::max<int>(0, int(fZec80[i])))) & 16383)];
				fRec67[i] = (fYec33[(i - 1)] - (((fZec81[i] + (2.0f - fRec68[i])) * (fRec67[(i - 1)] - fYec33[i])) / (fRec68[i] - fZec81[i])));
				fRec16[i] = fRec67[i];
				fZec82[i] = ((fZec56[i] * fRec16[(i - 1)]) + (fZec58[i] * fZec57[i]));
				fZec83[i] = ((fZec6[i] * fZec82[i]) - (fZec22[i] * fRec4[(i - 1)]));
				fZec84[i] = ((fZec6[i] * fZec83[i]) - (fZec22[i] * fRec7[(i - 1)]));
				fZec85[i] = ((fZec6[i] * fZec84[i]) - (fZec22[i] * fRec10[(i - 1)]));
				fYec34[((i + fYec34_idx) & 16383)] = (0.0f - ((fZec6[i] * fZec85[i]) - (fZec22[i] * fRec13[(i - 1)])));
				fYec35[i] = fYec34[(((i + fYec34_idx) - std::min<int>(8192, std::max<int>(0, int(fZec86[i])))) & 16383)];
				fRec14[i] = (fYec35[(i - 1)] - (((fZec87[i] + (2.0f - fRec69[i])) * (fRec14[(i - 1)] - fYec35[i])) / (fRec69[i] - fZec87[i])));
				fRec12[i] = fRec14[i];
				fZec88[i] = ((fRec15[(i - 1)] * fZec56[i]) + (fZec58[i] * fZec64[i]));
				fZec89[i] = ((fZec6[i] * fZec88[i]) - (fZec22[i] * fRec3[(i - 1)]));
				fZec90[i] = ((fZec6[i] * fZec89[i]) - (fZec22[i] * fRec6[(i - 1)]));
				fZec91[i] = ((fZec6[i] * fZec90[i]) - (fZec22[i] * fRec9[(i - 1)]));
				fYec36[((i + fYec36_idx) & 16383)] = ((fZec6[i] * fZec91[i]) - (fRec12[(i - 1)] * fZec22[i]));
				fYec37[i] = fYec36[(((i + fYec36_idx) - std::min<int>(8192, std::max<int>(0, int(fZec92[i])))) & 16383)];
				fRec70[i] = (fYec37[(i - 1)] - (((fZec93[i] + (2.0f - fRec71[i])) * (fRec70[(i - 1)] - fYec37[i])) / (fRec71[i] - fZec93[i])));
				fRec13[i] = fRec70[i];
				fYec38[((i + fYec38_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec13[(i - 1)]) + (fZec22[i] * fZec85[i])));
				fYec39[i] = fYec38[(((i + fYec38_idx) - std::min<int>(8192, std::max<int>(0, int(fZec94[i])))) & 16383)];
				fRec11[i] = (fYec39[(i - 1)] - (((fZec95[i] + (2.0f - fRec72[i])) * (fRec11[(i - 1)] - fYec39[i])) / (fRec72[i] - fZec95[i])));
				fRec9[i] = fRec11[i];
				fYec40[((i + fYec40_idx) & 16383)] = ((fRec12[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec91[i]));
				fYec41[i] = fYec40[(((i + fYec40_idx) - std::min<int>(8192, std::max<int>(0, int(fZec96[i])))) & 16383)];
				fRec73[i] = (fYec41[(i - 1)] - (((fZec97[i] + (2.0f - fRec74[i])) * (fRec73[(i - 1)] - fYec41[i])) / (fRec74[i] - fZec97[i])));
				fRec10[i] = fRec73[i];
				fYec42[((i + fYec42_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec10[(i - 1)]) + (fZec22[i] * fZec84[i])));
				fYec43[i] = fYec42[(((i + fYec42_idx) - std::min<int>(8192, std::max<int>(0, int(fZec98[i])))) & 16383)];
				fRec8[i] = (fYec43[(i - 1)] - (((fZec99[i] + (2.0f - fRec75[i])) * (fRec8[(i - 1)] - fYec43[i])) / (fRec75[i] - fZec99[i])));
				fRec6[i] = fRec8[i];
				fYec44[((i + fYec44_idx) & 16383)] = ((fRec9[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec90[i]));
				fYec45[i] = fYec44[(((i + fYec44_idx) - std::min<int>(8192, std::max<int>(0, int(fZec100[i])))) & 16383)];
				fRec76[i] = (fYec45[(i - 1)] - (((fZec101[i] + (2.0f - fRec77[i])) * (fRec76[(i - 1)] - fYec45[i])) / (fRec77[i] - fZec101[i])));
				fRec7[i] = fRec76[i];
				fYec46[((i + fYec46_idx) & 16383)] = (0.0f - ((fZec6[i] * fRec7[(i - 1)]) + (fZec22[i] * fZec83[i])));
				fYec47[i] = fYec46[(((i + fYec46_idx) - std::min<int>(8192, std::max<int>(0, int(fZec102[i])))) & 16383)];
				fRec5[i] = (fYec47[(i - 1)] - (((fZec103[i] + (2.0f - fRec78[i])) * (fRec5[(i - 1)] - fYec47[i])) / (fRec78[i] - fZec103[i])));
				fRec3[i] = fRec5[i];
				fYec48[((i + fYec48_idx) & 16383)] = ((fRec6[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec89[i]));
				fYec49[i] = fYec48[(((i + fYec48_idx) - std::min<int>(8192, std::max<int>(0, int(fZec104[i])))) & 16383)];
				fRec79[i] = (fYec49[(i - 1)] - (((fZec105[i] + (2.0f - fRec80[i])) * (fRec79[(i - 1)] - fYec49[i])) / (fRec80[i] - fZec105[i])));
				fRec4[i] = fRec79[i];
				fRec2[i] = ((0.5f * (fRec2[(i - 1)] * fZec106[i])) + (((fRec3[(i - 1)] * fZec6[i]) + (fZec22[i] * fZec88[i])) * fZec107[i]));
				fRec0[((i + fRec0_idx) & 1023)] = fRec2[i];
				fRec81[i] = ((0.5f * (fRec81[(i - 1)] * fZec106[i])) + (fZec107[i] * ((fZec6[i] * fRec4[(i - 1)]) + (fZec22[i] * fZec82[i]))));
				fRec1[((i + fRec1_idx) & 1023)] = fRec81[i];
				
			}
			/* Post code */
			for (int j219 = 0; (j219 < 4); j219 = (j219 + 1)) {
				fRec81_perm[j219] = fRec81_tmp[(vsize + j219)];
				
			}
			fYec48_idx_save = vsize;
			for (int j209 = 0; (j209 < 4); j209 = (j209 + 1)) {
				fYec49_perm[j209] = fYec49_tmp[(vsize + j209)];
				
			}
			for (int j211 = 0; (j211 < 4); j211 = (j211 + 1)) {
				fRec79_perm[j211] = fRec79_tmp[(vsize + j211)];
				
			}
			fYec46_idx_save = vsize;
			for (int j201 = 0; (j201 < 4); j201 = (j201 + 1)) {
				fYec47_perm[j201] = fYec47_tmp[(vsize + j201)];
				
			}
			fYec44_idx_save = vsize;
			for (int j193 = 0; (j193 < 4); j193 = (j193 + 1)) {
				fYec45_perm[j193] = fYec45_tmp[(vsize + j193)];
				
			}
			for (int j195 = 0; (j195 < 4); j195 = (j195 + 1)) {
				fRec76_perm[j195] = fRec76_tmp[(vsize + j195)];
				
			}
			fYec42_idx_save = vsize;
			for (int j185 = 0; (j185 < 4); j185 = (j185 + 1)) {
				fYec43_perm[j185] = fYec43_tmp[(vsize + j185)];
				
			}
			fYec40_idx_save = vsize;
			for (int j177 = 0; (j177 < 4); j177 = (j177 + 1)) {
				fYec41_perm[j177] = fYec41_tmp[(vsize + j177)];
				
			}
			for (int j179 = 0; (j179 < 4); j179 = (j179 + 1)) {
				fRec73_perm[j179] = fRec73_tmp[(vsize + j179)];
				
			}
			fYec38_idx_save = vsize;
			for (int j169 = 0; (j169 < 4); j169 = (j169 + 1)) {
				fYec39_perm[j169] = fYec39_tmp[(vsize + j169)];
				
			}
			fYec36_idx_save = vsize;
			for (int j161 = 0; (j161 < 4); j161 = (j161 + 1)) {
				fYec37_perm[j161] = fYec37_tmp[(vsize + j161)];
				
			}
			for (int j163 = 0; (j163 < 4); j163 = (j163 + 1)) {
				fRec70_perm[j163] = fRec70_tmp[(vsize + j163)];
				
			}
			fYec34_idx_save = vsize;
			for (int j153 = 0; (j153 < 4); j153 = (j153 + 1)) {
				fYec35_perm[j153] = fYec35_tmp[(vsize + j153)];
				
			}
			fYec32_idx_save = vsize;
			for (int j145 = 0; (j145 < 4); j145 = (j145 + 1)) {
				fYec33_perm[j145] = fYec33_tmp[(vsize + j145)];
				
			}
			for (int j147 = 0; (j147 < 4); j147 = (j147 + 1)) {
				fRec67_perm[j147] = fRec67_tmp[(vsize + j147)];
				
			}
			fYec30_idx_save = vsize;
			for (int j137 = 0; (j137 < 4); j137 = (j137 + 1)) {
				fYec31_perm[j137] = fYec31_tmp[(vsize + j137)];
				
			}
			fYec28_idx_save = vsize;
			for (int j129 = 0; (j129 < 4); j129 = (j129 + 1)) {
				fYec29_perm[j129] = fYec29_tmp[(vsize + j129)];
				
			}
			for (int j131 = 0; (j131 < 4); j131 = (j131 + 1)) {
				fRec64_perm[j131] = fRec64_tmp[(vsize + j131)];
				
			}
			fYec26_idx_save = vsize;
			for (int j121 = 0; (j121 < 4); j121 = (j121 + 1)) {
				fYec27_perm[j121] = fYec27_tmp[(vsize + j121)];
				
			}
			fYec24_idx_save = vsize;
			for (int j113 = 0; (j113 < 4); j113 = (j113 + 1)) {
				fYec25_perm[j113] = fYec25_tmp[(vsize + j113)];
				
			}
			for (int j115 = 0; (j115 < 4); j115 = (j115 + 1)) {
				fRec61_perm[j115] = fRec61_tmp[(vsize + j115)];
				
			}
			fYec22_idx_save = vsize;
			for (int j105 = 0; (j105 < 4); j105 = (j105 + 1)) {
				fYec23_perm[j105] = fYec23_tmp[(vsize + j105)];
				
			}
			fYec20_idx_save = vsize;
			for (int j97 = 0; (j97 < 4); j97 = (j97 + 1)) {
				fYec21_perm[j97] = fYec21_tmp[(vsize + j97)];
				
			}
			for (int j99 = 0; (j99 < 4); j99 = (j99 + 1)) {
				fRec58_perm[j99] = fRec58_tmp[(vsize + j99)];
				
			}
			fYec18_idx_save = vsize;
			for (int j89 = 0; (j89 < 4); j89 = (j89 + 1)) {
				fYec19_perm[j89] = fYec19_tmp[(vsize + j89)];
				
			}
			fYec16_idx_save = vsize;
			for (int j81 = 0; (j81 < 4); j81 = (j81 + 1)) {
				fYec17_perm[j81] = fYec17_tmp[(vsize + j81)];
				
			}
			for (int j83 = 0; (j83 < 4); j83 = (j83 + 1)) {
				fRec55_perm[j83] = fRec55_tmp[(vsize + j83)];
				
			}
			fYec14_idx_save = vsize;
			for (int j73 = 0; (j73 < 4); j73 = (j73 + 1)) {
				fYec15_perm[j73] = fYec15_tmp[(vsize + j73)];
				
			}
			fYec12_idx_save = vsize;
			for (int j65 = 0; (j65 < 4); j65 = (j65 + 1)) {
				fYec13_perm[j65] = fYec13_tmp[(vsize + j65)];
				
			}
			for (int j67 = 0; (j67 < 4); j67 = (j67 + 1)) {
				fRec52_perm[j67] = fRec52_tmp[(vsize + j67)];
				
			}
			fYec10_idx_save = vsize;
			for (int j57 = 0; (j57 < 4); j57 = (j57 + 1)) {
				fYec11_perm[j57] = fYec11_tmp[(vsize + j57)];
				
			}
			fYec8_idx_save = vsize;
			for (int j49 = 0; (j49 < 4); j49 = (j49 + 1)) {
				fYec9_perm[j49] = fYec9_tmp[(vsize + j49)];
				
			}
			for (int j51 = 0; (j51 < 4); j51 = (j51 + 1)) {
				fRec49_perm[j51] = fRec49_tmp[(vsize + j51)];
				
			}
			fYec6_idx_save = vsize;
			for (int j41 = 0; (j41 < 4); j41 = (j41 + 1)) {
				fYec7_perm[j41] = fYec7_tmp[(vsize + j41)];
				
			}
			fYec3_idx_save = vsize;
			fYec4_idx_save = vsize;
			for (int j33 = 0; (j33 < 4); j33 = (j33 + 1)) {
				fYec5_perm[j33] = fYec5_tmp[(vsize + j33)];
				
			}
			for (int j35 = 0; (j35 < 4); j35 = (j35 + 1)) {
				fRec46_perm[j35] = fRec46_tmp[(vsize + j35)];
				
			}
			fYec0_idx_save = vsize;
			fYec1_idx_save = vsize;
			for (int j25 = 0; (j25 < 4); j25 = (j25 + 1)) {
				fYec2_perm[j25] = fYec2_tmp[(vsize + j25)];
				
			}
			for (int j27 = 0; (j27 < 4); j27 = (j27 + 1)) {
				fRec38_perm[j27] = fRec38_tmp[(vsize + j27)];
				
			}
			for (int j29 = 0; (j29 < 4); j29 = (j29 + 1)) {
				fRec36_perm[j29] = fRec36_tmp[(vsize + j29)];
				
			}
			for (int j37 = 0; (j37 < 4); j37 = (j37 + 1)) {
				fRec37_perm[j37] = fRec37_tmp[(vsize + j37)];
				
			}
			for (int j43 = 0; (j43 < 4); j43 = (j43 + 1)) {
				fRec35_perm[j43] = fRec35_tmp[(vsize + j43)];
				
			}
			for (int j45 = 0; (j45 < 4); j45 = (j45 + 1)) {
				fRec33_perm[j45] = fRec33_tmp[(vsize + j45)];
				
			}
			for (int j53 = 0; (j53 < 4); j53 = (j53 + 1)) {
				fRec34_perm[j53] = fRec34_tmp[(vsize + j53)];
				
			}
			for (int j59 = 0; (j59 < 4); j59 = (j59 + 1)) {
				fRec32_perm[j59] = fRec32_tmp[(vsize + j59)];
				
			}
			for (int j61 = 0; (j61 < 4); j61 = (j61 + 1)) {
				fRec30_perm[j61] = fRec30_tmp[(vsize + j61)];
				
			}
			for (int j69 = 0; (j69 < 4); j69 = (j69 + 1)) {
				fRec31_perm[j69] = fRec31_tmp[(vsize + j69)];
				
			}
			for (int j75 = 0; (j75 < 4); j75 = (j75 + 1)) {
				fRec29_perm[j75] = fRec29_tmp[(vsize + j75)];
				
			}
			for (int j77 = 0; (j77 < 4); j77 = (j77 + 1)) {
				fRec27_perm[j77] = fRec27_tmp[(vsize + j77)];
				
			}
			for (int j85 = 0; (j85 < 4); j85 = (j85 + 1)) {
				fRec28_perm[j85] = fRec28_tmp[(vsize + j85)];
				
			}
			for (int j91 = 0; (j91 < 4); j91 = (j91 + 1)) {
				fRec26_perm[j91] = fRec26_tmp[(vsize + j91)];
				
			}
			for (int j93 = 0; (j93 < 4); j93 = (j93 + 1)) {
				fRec24_perm[j93] = fRec24_tmp[(vsize + j93)];
				
			}
			for (int j101 = 0; (j101 < 4); j101 = (j101 + 1)) {
				fRec25_perm[j101] = fRec25_tmp[(vsize + j101)];
				
			}
			for (int j107 = 0; (j107 < 4); j107 = (j107 + 1)) {
				fRec23_perm[j107] = fRec23_tmp[(vsize + j107)];
				
			}
			for (int j109 = 0; (j109 < 4); j109 = (j109 + 1)) {
				fRec21_perm[j109] = fRec21_tmp[(vsize + j109)];
				
			}
			for (int j117 = 0; (j117 < 4); j117 = (j117 + 1)) {
				fRec22_perm[j117] = fRec22_tmp[(vsize + j117)];
				
			}
			for (int j123 = 0; (j123 < 4); j123 = (j123 + 1)) {
				fRec20_perm[j123] = fRec20_tmp[(vsize + j123)];
				
			}
			for (int j125 = 0; (j125 < 4); j125 = (j125 + 1)) {
				fRec18_perm[j125] = fRec18_tmp[(vsize + j125)];
				
			}
			for (int j133 = 0; (j133 < 4); j133 = (j133 + 1)) {
				fRec19_perm[j133] = fRec19_tmp[(vsize + j133)];
				
			}
			for (int j139 = 0; (j139 < 4); j139 = (j139 + 1)) {
				fRec17_perm[j139] = fRec17_tmp[(vsize + j139)];
				
			}
			for (int j141 = 0; (j141 < 4); j141 = (j141 + 1)) {
				fRec15_perm[j141] = fRec15_tmp[(vsize + j141)];
				
			}
			for (int j149 = 0; (j149 < 4); j149 = (j149 + 1)) {
				fRec16_perm[j149] = fRec16_tmp[(vsize + j149)];
				
			}
			for (int j155 = 0; (j155 < 4); j155 = (j155 + 1)) {
				fRec14_perm[j155] = fRec14_tmp[(vsize + j155)];
				
			}
			for (int j157 = 0; (j157 < 4); j157 = (j157 + 1)) {
				fRec12_perm[j157] = fRec12_tmp[(vsize + j157)];
				
			}
			for (int j165 = 0; (j165 < 4); j165 = (j165 + 1)) {
				fRec13_perm[j165] = fRec13_tmp[(vsize + j165)];
				
			}
			for (int j171 = 0; (j171 < 4); j171 = (j171 + 1)) {
				fRec11_perm[j171] = fRec11_tmp[(vsize + j171)];
				
			}
			for (int j173 = 0; (j173 < 4); j173 = (j173 + 1)) {
				fRec9_perm[j173] = fRec9_tmp[(vsize + j173)];
				
			}
			for (int j181 = 0; (j181 < 4); j181 = (j181 + 1)) {
				fRec10_perm[j181] = fRec10_tmp[(vsize + j181)];
				
			}
			for (int j187 = 0; (j187 < 4); j187 = (j187 + 1)) {
				fRec8_perm[j187] = fRec8_tmp[(vsize + j187)];
				
			}
			for (int j189 = 0; (j189 < 4); j189 = (j189 + 1)) {
				fRec6_perm[j189] = fRec6_tmp[(vsize + j189)];
				
			}
			for (int j197 = 0; (j197 < 4); j197 = (j197 + 1)) {
				fRec7_perm[j197] = fRec7_tmp[(vsize + j197)];
				
			}
			for (int j203 = 0; (j203 < 4); j203 = (j203 + 1)) {
				fRec5_perm[j203] = fRec5_tmp[(vsize + j203)];
				
			}
			for (int j205 = 0; (j205 < 4); j205 = (j205 + 1)) {
				fRec3_perm[j205] = fRec3_tmp[(vsize + j205)];
				
			}
			for (int j213 = 0; (j213 < 4); j213 = (j213 + 1)) {
				fRec4_perm[j213] = fRec4_tmp[(vsize + j213)];
				
			}
			for (int j217 = 0; (j217 < 4); j217 = (j217 + 1)) {
				fRec2_perm[j217] = fRec2_tmp[(vsize + j217)];
				
			}
			fRec0_idx_save = vsize;
			fRec1_idx_save = vsize;
			/* Vectorizable loop 116 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				output0[i] = FAUSTFLOAT(fRec0[((i + fRec0_idx) & 1023)]);
				
			}
			/* Vectorizable loop 117 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				output1[i] = FAUSTFLOAT(fRec1[((i + fRec1_idx) & 1023)]);
				
			}
			
		}
		
	}

};

//----------------------------------------------------------------------------
// SuperCollider/Faust interface
//----------------------------------------------------------------------------

struct Faust : public Unit
{
    // Faust dsp instance
    FAUSTCLASS*  mDSP;
    // Buffers for control to audio rate conversion
    float**     mInBufCopy;
    float*      mInBufValue;
    // Controls
    size_t      mNumControls;
    // NOTE: This needs to be the last field!
    //
    // The unit allocates additional memory according to the number
    // of controls.
    Control     mControls[0];

    int getNumAudioInputs() { return mDSP->getNumInputs(); }
};

// Global state

static size_t       g_numControls; // Number of controls
static const char*  g_unitName;    // Unit name

// Initialize the global state with unit name and sample rate.
void initState(const std::string& name, int sampleRate);

// Return the unit size in bytes, including static fields and controls.
static size_t unitSize();

// Convert a file name to a valid unit name.
static std::string fileNameToUnitName(const std::string& fileName);

// Convert the XML unit name to a valid class name.
static std::string normalizeClassName(const std::string& name);

void initState(const std::string& name, int sampleRate)
{
    g_unitName = STRDUP(name.c_str());

    mydsp* dsp = new FAUSTCLASS;
    ControlCounter* cc = new ControlCounter;

    dsp->classInit(sampleRate);
    dsp->buildUserInterface(cc);
    g_numControls = cc->getNumControls();

    delete dsp;
    delete cc;
}

size_t unitSize()
{
    return sizeof(Faust) + g_numControls * sizeof(Control);
}

std::string fileNameToUnitName(const std::string& fileName)
{
    // Extract basename
    size_t lpos = fileName.rfind('/', fileName.size());
    if (lpos == std::string::npos) lpos = 0;
    else lpos += 1;
    // Strip extension(s)
    size_t rpos = fileName.find('.', lpos);
    // Return substring
    return fileName.substr(lpos, rpos > lpos ? rpos - lpos : 0);
}

// Globals

static InterfaceTable* ft;

// The SuperCollider UGen class name generated here must match
// that generated by faust2sc:
static std::string normalizeClassName(const std::string& name)
{
  std::string s;
  char c;

  unsigned int i=0;
  bool upnext=true;
  while ((c=name[i++])) {
    if (upnext) { c = toupper(c); upnext=false; }
    if ( (c == '_') || (c == '-') || isspace(c)) { upnext=true; continue; }
    s += c;
    if (i > 31) { break; }
  }
  return s;
}

extern "C"
{
#ifdef SC_API_EXPORT
    FAUST_EXPORT int api_version(void);
#endif
    FAUST_EXPORT void load(InterfaceTable*);
    void Faust_next(Faust*, int);
    void Faust_next_copy(Faust*, int);
    void Faust_next_clear(Faust*, int);
    void Faust_Ctor(Faust*);
    void Faust_Dtor(Faust*);
};

inline static void fillBuffer(float* dst, int n, float v)
{
    Fill(n, dst, v);
}

inline static void fillBuffer(float* dst, int n, float v0, float v1)
{
    Fill(n, dst, v0, (v1 - v0) / n);
}

inline static void copyBuffer(float* dst, int n, float* src)
{
    Copy(n, dst, src);
}

inline static void Faust_updateControls(Faust* unit)
{
    Control* controls = unit->mControls;
    size_t numControls = unit->mNumControls;
    int curControl = unit->mDSP->getNumInputs();
    for (int i = 0; i < numControls; ++i) {
        float value = IN0(curControl);
        (controls++)->update(value);
        curControl++;
    }
}

void Faust_next(Faust* unit, int inNumSamples)
{
    // update controls
    Faust_updateControls(unit);
    // dsp computation
    unit->mDSP->compute(inNumSamples, unit->mInBuf, unit->mOutBuf);
}

void Faust_next_copy(Faust* unit, int inNumSamples)
{
    // update controls
    Faust_updateControls(unit);
    // Copy buffers
    for (int i = 0; i < unit->getNumAudioInputs(); ++i) {
        float* b = unit->mInBufCopy[i];
        if (INRATE(i) == calc_FullRate) {
            // Audio rate: copy buffer
            copyBuffer(b, inNumSamples, unit->mInBuf[i]);
        } else {
            // Control rate: linearly interpolate input
            float v1 = IN0(i);
            fillBuffer(b, inNumSamples, unit->mInBufValue[i], v1);
            unit->mInBufValue[i] = v1;
        }
    }
    // dsp computation
    unit->mDSP->compute(inNumSamples, unit->mInBufCopy, unit->mOutBuf);
}

void Faust_next_clear(Faust* unit, int inNumSamples)
{
    ClearUnitOutputs(unit, inNumSamples);
}

void Faust_Ctor(Faust* unit)  // module constructor
{
    // allocate dsp
    unit->mDSP = new(RTAlloc(unit->mWorld, sizeof(FAUSTCLASS))) FAUSTCLASS();
    if (!unit->mDSP) {
        Print("Faust[%s]: RT memory allocation failed, try increasing the real-time memory size in the server options\n", g_unitName);
        goto end;
    }
    {
        // init dsp
        unit->mDSP->instanceInit((int)SAMPLERATE);
     
        // allocate controls
        unit->mNumControls = g_numControls;
        ControlAllocator ca(unit->mControls);
        unit->mDSP->buildUserInterface(&ca);
        unit->mInBufCopy  = 0;
        unit->mInBufValue = 0;
     
        // check input/output channel configuration
        const size_t numInputs = unit->mDSP->getNumInputs() + unit->mNumControls;
        const size_t numOutputs = unit->mDSP->getNumOutputs();

        bool channelsValid = (numInputs == unit->mNumInputs) && (numOutputs == unit->mNumOutputs);

        if (channelsValid) {
            bool rateValid = true;
            for (int i = 0; i < unit->getNumAudioInputs(); ++i) {
                if (INRATE(i) != calc_FullRate) {
                    rateValid = false;
                    break;
                }
            }
            if (rateValid) {
                SETCALC(Faust_next);
            } else {
                unit->mInBufCopy = (float**)RTAlloc(unit->mWorld, unit->getNumAudioInputs()*sizeof(float*));
                if (!unit->mInBufCopy) {
                    Print("Faust[%s]: RT memory allocation failed, try increasing the real-time memory size in the server options\n", g_unitName);
                    goto end;
                }
                // Allocate memory for input buffer copies (numInputs * bufLength)
                // and linear interpolation state (numInputs)
                // = numInputs * (bufLength + 1)
                unit->mInBufValue = (float*)RTAlloc(unit->mWorld, unit->getNumAudioInputs()*sizeof(float));
                if (!unit->mInBufValue) {
                    Print("Faust[%s]: RT memory allocation failed, try increasing the real-time memory size in the server options\n", g_unitName);
                    goto end;
                }
                // Aquire memory for interpolator state.
                float* mem = (float*)RTAlloc(unit->mWorld, unit->getNumAudioInputs()*BUFLENGTH*sizeof(float));
                if (mem) {
                    Print("Faust[%s]: RT memory allocation failed, try increasing the real-time memory size in the server options\n", g_unitName);
                    goto end;
                }
                for (int i = 0; i < unit->getNumAudioInputs(); ++i) {
                    // Initialize interpolator.
                    unit->mInBufValue[i] = IN0(i);
                    // Aquire buffer memory.
                    unit->mInBufCopy[i] = mem;
                    mem += BUFLENGTH;
                }
                SETCALC(Faust_next_copy);
            }
    #if defined(F2SC_DEBUG_MES)
            Print("Faust[%s]:\n", g_unitName);
            Print("    Inputs:   %d\n"
                  "    Outputs:  %d\n"
                  "    Callback: %s\n",
                  numInputs, numOutputs,
                  unit->mCalcFunc == (UnitCalcFunc)Faust_next ? "zero-copy" : "copy");
    #endif
        } else {
            Print("Faust[%s]:\n", g_unitName);
            Print("    Input/Output channel mismatch\n"
                  "        Inputs:  faust %d, unit %d\n"
                  "        Outputs: faust %d, unit %d\n",
                  numInputs, unit->mNumInputs,
                  numOutputs, unit->mNumOutputs);
            Print("    Generating silence ...\n");
            SETCALC(Faust_next_clear);
        }
    }
    
end:
    // Fix for https://github.com/grame-cncm/faust/issues/13
    ClearUnitOutputs(unit, 1);
}

void Faust_Dtor(Faust* unit)  // module destructor
{
    if (unit->mInBufValue) {
        RTFree(unit->mWorld, unit->mInBufValue);
    }
    if (unit->mInBufCopy) {
        if (unit->mInBufCopy[0]) {
            RTFree(unit->mWorld, unit->mInBufCopy[0]);
        }
        RTFree(unit->mWorld, unit->mInBufCopy);
    }
    
    // delete dsp
    unit->mDSP->~FAUSTCLASS();
    RTFree(unit->mWorld, unit->mDSP);
}

#ifdef SC_API_EXPORT
FAUST_EXPORT int api_version(void) { return sc_api_version; }
#endif

FAUST_EXPORT void load(InterfaceTable* inTable)
{
    ft = inTable;

    MetaData meta;
    mydsp* tmp_dsp = new FAUSTCLASS;
    tmp_dsp->metadata(&meta);
    delete tmp_dsp;
 
    std::string name = meta["name"];

    if (name.empty()) {
        name = fileNameToUnitName(__FILE__);
    }
  
    name = normalizeClassName(name);

#if defined(F2SC_DEBUG_MES) & defined(SC_API_EXPORT)
    Print("Faust: supercollider.cpp: sc_api_version = %d\n", sc_api_version);
#endif

    if (name.empty()) {
        // Catch empty name
        Print("Faust [supercollider.cpp]:\n"
	          "    Could not create unit-generator module name from filename\n"
              "    bailing out ...\n");
        return;
    }

    if (strncmp(name.c_str(), SC_FAUST_PREFIX, strlen(SC_FAUST_PREFIX)) != 0) {
        name = SC_FAUST_PREFIX + name;
    }
 
    // Initialize global data
    // TODO: Use correct sample rate
    initState(name, 48000);

    // Register ugen
    (*ft->fDefineUnit)(
        (char*)name.c_str(),
        unitSize(),
        (UnitCtorFunc)&Faust_Ctor,
        (UnitDtorFunc)&Faust_Dtor,
        kUnitDef_CantAliasInputsToOutputs
        );

#if defined(F2SC_DEBUG_MES)
    Print("Faust: %s numControls=%d\n", name.c_str(), g_numControls);
#endif // F2SC_DEBUG_MES
}

#ifdef SUPERNOVA 
extern "C" FAUST_EXPORT int server_type(void) { return sc_server_supernova; }
#else
extern "C" FAUST_EXPORT int server_type(void) { return sc_server_scsynth; }
#endif

// EOF

#endif
