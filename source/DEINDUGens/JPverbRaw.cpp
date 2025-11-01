/* ------------------------------------------------------------
author: "Julian Parker, bug fixes by Till Bovermann"
copyright: "(c) Julian Parker 2013"
license: "GPL2+"
name: "JPverbRaw"
version: "1.1"
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

static float mydsp_faustpower2_f(float value) {
	return (value * value);
	
}

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
	float fConst1;
	FAUSTFLOAT fHslider0;
	float fRec15_perm[4];
	int iVec0_perm[4];
	float fRec16_perm[4];
	FAUSTFLOAT fHslider1;
	float fRec53_perm[4];
	float fYec0[16384];
	int fYec0_idx;
	int fYec0_idx_save;
	float fYec1_perm[4];
	float fRec52_perm[4];
	float fRec50_perm[4];
	float fRec55_perm[4];
	float fYec2[16384];
	int fYec2_idx;
	int fYec2_idx_save;
	float fYec3_perm[4];
	float fRec54_perm[4];
	float fRec51_perm[4];
	float fRec56_perm[4];
	float fYec4[16384];
	int fYec4_idx;
	int fYec4_idx_save;
	float fYec5_perm[4];
	float fRec49_perm[4];
	float fRec47_perm[4];
	float fRec58_perm[4];
	float fYec6[16384];
	int fYec6_idx;
	int fYec6_idx_save;
	float fYec7_perm[4];
	float fRec57_perm[4];
	float fRec48_perm[4];
	float fRec59_perm[4];
	float fYec8[16384];
	int fYec8_idx;
	int fYec8_idx_save;
	float fYec9_perm[4];
	float fRec46_perm[4];
	float fRec44_perm[4];
	float fRec61_perm[4];
	float fYec10[16384];
	int fYec10_idx;
	int fYec10_idx_save;
	float fYec11_perm[4];
	float fRec60_perm[4];
	float fRec45_perm[4];
	float fRec62_perm[4];
	float fYec12[16384];
	int fYec12_idx;
	int fYec12_idx_save;
	float fYec13_perm[4];
	float fRec43_perm[4];
	float fRec41_perm[4];
	float fRec64_perm[4];
	float fYec14[16384];
	int fYec14_idx;
	int fYec14_idx_save;
	float fYec15_perm[4];
	float fRec63_perm[4];
	float fRec42_perm[4];
	float fRec65_perm[4];
	float fYec16[16384];
	int fYec16_idx;
	int fYec16_idx_save;
	float fYec17_perm[4];
	float fRec40_perm[4];
	float fRec38_perm[4];
	float fRec67_perm[4];
	float fYec18[16384];
	int fYec18_idx;
	int fYec18_idx_save;
	float fYec19_perm[4];
	float fRec66_perm[4];
	float fRec39_perm[4];
	float fRec68_perm[4];
	float fYec20[1024];
	int fYec20_idx;
	int fYec20_idx_save;
	FAUSTFLOAT fHslider2;
	float fYec21[16384];
	int fYec21_idx;
	int fYec21_idx_save;
	float fYec22_perm[4];
	float fRec37_perm[4];
	float fRec70_perm[4];
	float fYec23[1024];
	int fYec23_idx;
	int fYec23_idx_save;
	float fYec24[16384];
	int fYec24_idx;
	int fYec24_idx_save;
	float fYec25_perm[4];
	float fRec69_perm[4];
	float fRec71_perm[4];
	float fYec26[16384];
	int fYec26_idx;
	int fYec26_idx_save;
	float fYec27_perm[4];
	float fRec36_perm[4];
	float fRec34_perm[4];
	float fRec73_perm[4];
	float fYec28[16384];
	int fYec28_idx;
	int fYec28_idx_save;
	float fYec29_perm[4];
	float fRec72_perm[4];
	float fRec35_perm[4];
	float fRec74_perm[4];
	float fYec30[16384];
	int fYec30_idx;
	int fYec30_idx_save;
	float fYec31_perm[4];
	float fRec33_perm[4];
	float fRec31_perm[4];
	float fRec76_perm[4];
	float fYec32[16384];
	int fYec32_idx;
	int fYec32_idx_save;
	float fYec33_perm[4];
	float fRec75_perm[4];
	float fRec32_perm[4];
	float fRec77_perm[4];
	float fYec34[16384];
	int fYec34_idx;
	int fYec34_idx_save;
	float fYec35_perm[4];
	float fRec30_perm[4];
	float fRec28_perm[4];
	float fRec79_perm[4];
	float fYec36[16384];
	int fYec36_idx;
	int fYec36_idx_save;
	float fYec37_perm[4];
	float fRec78_perm[4];
	float fRec29_perm[4];
	float fRec80_perm[4];
	float fYec38[16384];
	int fYec38_idx;
	int fYec38_idx_save;
	float fYec39_perm[4];
	float fRec27_perm[4];
	float fRec25_perm[4];
	float fRec82_perm[4];
	float fYec40[16384];
	int fYec40_idx;
	int fYec40_idx_save;
	float fYec41_perm[4];
	float fRec81_perm[4];
	float fRec26_perm[4];
	float fRec83_perm[4];
	float fYec42[16384];
	int fYec42_idx;
	int fYec42_idx_save;
	float fYec43_perm[4];
	float fRec24_perm[4];
	float fRec22_perm[4];
	float fRec85_perm[4];
	float fYec44[16384];
	int fYec44_idx;
	int fYec44_idx_save;
	float fYec45_perm[4];
	float fRec84_perm[4];
	float fRec23_perm[4];
	float fRec86_perm[4];
	float fYec46[16384];
	int fYec46_idx;
	int fYec46_idx_save;
	float fYec47[16384];
	int fYec47_idx;
	int fYec47_idx_save;
	float fYec48_perm[4];
	float fRec21_perm[4];
	float fConst2;
	FAUSTFLOAT fHslider3;
	float fRec20_perm[4];
	float fRec19_perm[4];
	float fRec18_perm[4];
	FAUSTFLOAT fHslider4;
	float fRec17_perm[4];
	float fRec92_perm[4];
	float fRec91_perm[4];
	float fRec90_perm[4];
	float fYec49_perm[4];
	float fRec89_perm[4];
	float fRec88_perm[4];
	float fRec87_perm[4];
	float fRec95_perm[4];
	float fRec94_perm[4];
	float fRec93_perm[4];
	FAUSTFLOAT fHslider5;
	FAUSTFLOAT fHslider6;
	FAUSTFLOAT fHslider7;
	FAUSTFLOAT fHslider8;
	FAUSTFLOAT fHslider9;
	float fYec50[1024];
	int fYec50_idx;
	int fYec50_idx_save;
	float fRec14_perm[4];
	float fRec102_perm[4];
	float fYec51[16384];
	int fYec51_idx;
	int fYec51_idx_save;
	float fYec52[16384];
	int fYec52_idx;
	int fYec52_idx_save;
	float fYec53_perm[4];
	float fRec101_perm[4];
	float fRec100_perm[4];
	float fRec99_perm[4];
	float fRec98_perm[4];
	float fRec97_perm[4];
	float fRec108_perm[4];
	float fRec107_perm[4];
	float fRec106_perm[4];
	float fYec54_perm[4];
	float fRec105_perm[4];
	float fRec104_perm[4];
	float fRec103_perm[4];
	float fRec111_perm[4];
	float fRec110_perm[4];
	float fRec109_perm[4];
	float fYec55[1024];
	int fYec55_idx;
	int fYec55_idx_save;
	float fRec96_perm[4];
	FAUSTFLOAT fHslider10;
	float fYec56[16384];
	int fYec56_idx;
	int fYec56_idx_save;
	float fYec57_perm[4];
	float fRec13_perm[4];
	float fRec11_perm[4];
	float fRec113_perm[4];
	float fYec58[16384];
	int fYec58_idx;
	int fYec58_idx_save;
	float fYec59_perm[4];
	float fRec112_perm[4];
	float fRec12_perm[4];
	float fYec60[16384];
	int fYec60_idx;
	int fYec60_idx_save;
	float fYec61_perm[4];
	float fRec10_perm[4];
	float fRec8_perm[4];
	float fYec62[16384];
	int fYec62_idx;
	int fYec62_idx_save;
	float fYec63_perm[4];
	float fRec114_perm[4];
	float fRec9_perm[4];
	float fYec64[16384];
	int fYec64_idx;
	int fYec64_idx_save;
	float fYec65_perm[4];
	float fRec7_perm[4];
	float fRec5_perm[4];
	float fRec116_perm[4];
	float fYec66[16384];
	int fYec66_idx;
	int fYec66_idx_save;
	float fYec67_perm[4];
	float fRec115_perm[4];
	float fRec6_perm[4];
	float fRec117_perm[4];
	float fYec68[16384];
	int fYec68_idx;
	int fYec68_idx_save;
	float fYec69_perm[4];
	float fRec4_perm[4];
	float fRec2_perm[4];
	float fYec70[16384];
	int fYec70_idx;
	int fYec70_idx_save;
	float fYec71_perm[4];
	float fRec118_perm[4];
	float fRec3_perm[4];
	float fRec0_perm[4];
	float fRec1_perm[4];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("analyzers.lib/name", "Faust Analyzer Library");
		m->declare("analyzers.lib/version", "0.0");
		m->declare("author", "Julian Parker, bug fixes by Till Bovermann");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Julian Parker 2013");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.1");
		m->declare("filename", "JPverbRaw");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("license", "GPL2+");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("name", "JPverbRaw");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.1");
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
		fConst1 = (6.28318548f / fConst0);
		fConst2 = (3.14159274f / fConst0);
		
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(2.0f);
		fHslider1 = FAUSTFLOAT(1.0f);
		fHslider2 = FAUSTFLOAT(0.10000000000000001f);
		fHslider3 = FAUSTFLOAT(2000.0f);
		fHslider4 = FAUSTFLOAT(500.0f);
		fHslider5 = FAUSTFLOAT(0.0f);
		fHslider6 = FAUSTFLOAT(1.0f);
		fHslider7 = FAUSTFLOAT(1.0f);
		fHslider8 = FAUSTFLOAT(1.0f);
		fHslider9 = FAUSTFLOAT(1.0f);
		fHslider10 = FAUSTFLOAT(0.70699999999999996f);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 4); l0 = (l0 + 1)) {
			fRec15_perm[l0] = 0.0f;
			
		}
		for (int l1 = 0; (l1 < 4); l1 = (l1 + 1)) {
			iVec0_perm[l1] = 0;
			
		}
		for (int l2 = 0; (l2 < 4); l2 = (l2 + 1)) {
			fRec16_perm[l2] = 0.0f;
			
		}
		for (int l3 = 0; (l3 < 4); l3 = (l3 + 1)) {
			fRec53_perm[l3] = 0.0f;
			
		}
		for (int l4 = 0; (l4 < 16384); l4 = (l4 + 1)) {
			fYec0[l4] = 0.0f;
			
		}
		fYec0_idx = 0;
		fYec0_idx_save = 0;
		for (int l5 = 0; (l5 < 4); l5 = (l5 + 1)) {
			fYec1_perm[l5] = 0.0f;
			
		}
		for (int l6 = 0; (l6 < 4); l6 = (l6 + 1)) {
			fRec52_perm[l6] = 0.0f;
			
		}
		for (int l7 = 0; (l7 < 4); l7 = (l7 + 1)) {
			fRec50_perm[l7] = 0.0f;
			
		}
		for (int l8 = 0; (l8 < 4); l8 = (l8 + 1)) {
			fRec55_perm[l8] = 0.0f;
			
		}
		for (int l9 = 0; (l9 < 16384); l9 = (l9 + 1)) {
			fYec2[l9] = 0.0f;
			
		}
		fYec2_idx = 0;
		fYec2_idx_save = 0;
		for (int l10 = 0; (l10 < 4); l10 = (l10 + 1)) {
			fYec3_perm[l10] = 0.0f;
			
		}
		for (int l11 = 0; (l11 < 4); l11 = (l11 + 1)) {
			fRec54_perm[l11] = 0.0f;
			
		}
		for (int l12 = 0; (l12 < 4); l12 = (l12 + 1)) {
			fRec51_perm[l12] = 0.0f;
			
		}
		for (int l13 = 0; (l13 < 4); l13 = (l13 + 1)) {
			fRec56_perm[l13] = 0.0f;
			
		}
		for (int l14 = 0; (l14 < 16384); l14 = (l14 + 1)) {
			fYec4[l14] = 0.0f;
			
		}
		fYec4_idx = 0;
		fYec4_idx_save = 0;
		for (int l15 = 0; (l15 < 4); l15 = (l15 + 1)) {
			fYec5_perm[l15] = 0.0f;
			
		}
		for (int l16 = 0; (l16 < 4); l16 = (l16 + 1)) {
			fRec49_perm[l16] = 0.0f;
			
		}
		for (int l17 = 0; (l17 < 4); l17 = (l17 + 1)) {
			fRec47_perm[l17] = 0.0f;
			
		}
		for (int l18 = 0; (l18 < 4); l18 = (l18 + 1)) {
			fRec58_perm[l18] = 0.0f;
			
		}
		for (int l19 = 0; (l19 < 16384); l19 = (l19 + 1)) {
			fYec6[l19] = 0.0f;
			
		}
		fYec6_idx = 0;
		fYec6_idx_save = 0;
		for (int l20 = 0; (l20 < 4); l20 = (l20 + 1)) {
			fYec7_perm[l20] = 0.0f;
			
		}
		for (int l21 = 0; (l21 < 4); l21 = (l21 + 1)) {
			fRec57_perm[l21] = 0.0f;
			
		}
		for (int l22 = 0; (l22 < 4); l22 = (l22 + 1)) {
			fRec48_perm[l22] = 0.0f;
			
		}
		for (int l23 = 0; (l23 < 4); l23 = (l23 + 1)) {
			fRec59_perm[l23] = 0.0f;
			
		}
		for (int l24 = 0; (l24 < 16384); l24 = (l24 + 1)) {
			fYec8[l24] = 0.0f;
			
		}
		fYec8_idx = 0;
		fYec8_idx_save = 0;
		for (int l25 = 0; (l25 < 4); l25 = (l25 + 1)) {
			fYec9_perm[l25] = 0.0f;
			
		}
		for (int l26 = 0; (l26 < 4); l26 = (l26 + 1)) {
			fRec46_perm[l26] = 0.0f;
			
		}
		for (int l27 = 0; (l27 < 4); l27 = (l27 + 1)) {
			fRec44_perm[l27] = 0.0f;
			
		}
		for (int l28 = 0; (l28 < 4); l28 = (l28 + 1)) {
			fRec61_perm[l28] = 0.0f;
			
		}
		for (int l29 = 0; (l29 < 16384); l29 = (l29 + 1)) {
			fYec10[l29] = 0.0f;
			
		}
		fYec10_idx = 0;
		fYec10_idx_save = 0;
		for (int l30 = 0; (l30 < 4); l30 = (l30 + 1)) {
			fYec11_perm[l30] = 0.0f;
			
		}
		for (int l31 = 0; (l31 < 4); l31 = (l31 + 1)) {
			fRec60_perm[l31] = 0.0f;
			
		}
		for (int l32 = 0; (l32 < 4); l32 = (l32 + 1)) {
			fRec45_perm[l32] = 0.0f;
			
		}
		for (int l33 = 0; (l33 < 4); l33 = (l33 + 1)) {
			fRec62_perm[l33] = 0.0f;
			
		}
		for (int l34 = 0; (l34 < 16384); l34 = (l34 + 1)) {
			fYec12[l34] = 0.0f;
			
		}
		fYec12_idx = 0;
		fYec12_idx_save = 0;
		for (int l35 = 0; (l35 < 4); l35 = (l35 + 1)) {
			fYec13_perm[l35] = 0.0f;
			
		}
		for (int l36 = 0; (l36 < 4); l36 = (l36 + 1)) {
			fRec43_perm[l36] = 0.0f;
			
		}
		for (int l37 = 0; (l37 < 4); l37 = (l37 + 1)) {
			fRec41_perm[l37] = 0.0f;
			
		}
		for (int l38 = 0; (l38 < 4); l38 = (l38 + 1)) {
			fRec64_perm[l38] = 0.0f;
			
		}
		for (int l39 = 0; (l39 < 16384); l39 = (l39 + 1)) {
			fYec14[l39] = 0.0f;
			
		}
		fYec14_idx = 0;
		fYec14_idx_save = 0;
		for (int l40 = 0; (l40 < 4); l40 = (l40 + 1)) {
			fYec15_perm[l40] = 0.0f;
			
		}
		for (int l41 = 0; (l41 < 4); l41 = (l41 + 1)) {
			fRec63_perm[l41] = 0.0f;
			
		}
		for (int l42 = 0; (l42 < 4); l42 = (l42 + 1)) {
			fRec42_perm[l42] = 0.0f;
			
		}
		for (int l43 = 0; (l43 < 4); l43 = (l43 + 1)) {
			fRec65_perm[l43] = 0.0f;
			
		}
		for (int l44 = 0; (l44 < 16384); l44 = (l44 + 1)) {
			fYec16[l44] = 0.0f;
			
		}
		fYec16_idx = 0;
		fYec16_idx_save = 0;
		for (int l45 = 0; (l45 < 4); l45 = (l45 + 1)) {
			fYec17_perm[l45] = 0.0f;
			
		}
		for (int l46 = 0; (l46 < 4); l46 = (l46 + 1)) {
			fRec40_perm[l46] = 0.0f;
			
		}
		for (int l47 = 0; (l47 < 4); l47 = (l47 + 1)) {
			fRec38_perm[l47] = 0.0f;
			
		}
		for (int l48 = 0; (l48 < 4); l48 = (l48 + 1)) {
			fRec67_perm[l48] = 0.0f;
			
		}
		for (int l49 = 0; (l49 < 16384); l49 = (l49 + 1)) {
			fYec18[l49] = 0.0f;
			
		}
		fYec18_idx = 0;
		fYec18_idx_save = 0;
		for (int l50 = 0; (l50 < 4); l50 = (l50 + 1)) {
			fYec19_perm[l50] = 0.0f;
			
		}
		for (int l51 = 0; (l51 < 4); l51 = (l51 + 1)) {
			fRec66_perm[l51] = 0.0f;
			
		}
		for (int l52 = 0; (l52 < 4); l52 = (l52 + 1)) {
			fRec39_perm[l52] = 0.0f;
			
		}
		for (int l53 = 0; (l53 < 4); l53 = (l53 + 1)) {
			fRec68_perm[l53] = 0.0f;
			
		}
		for (int l54 = 0; (l54 < 1024); l54 = (l54 + 1)) {
			fYec20[l54] = 0.0f;
			
		}
		fYec20_idx = 0;
		fYec20_idx_save = 0;
		for (int l55 = 0; (l55 < 16384); l55 = (l55 + 1)) {
			fYec21[l55] = 0.0f;
			
		}
		fYec21_idx = 0;
		fYec21_idx_save = 0;
		for (int l56 = 0; (l56 < 4); l56 = (l56 + 1)) {
			fYec22_perm[l56] = 0.0f;
			
		}
		for (int l57 = 0; (l57 < 4); l57 = (l57 + 1)) {
			fRec37_perm[l57] = 0.0f;
			
		}
		for (int l58 = 0; (l58 < 4); l58 = (l58 + 1)) {
			fRec70_perm[l58] = 0.0f;
			
		}
		for (int l59 = 0; (l59 < 1024); l59 = (l59 + 1)) {
			fYec23[l59] = 0.0f;
			
		}
		fYec23_idx = 0;
		fYec23_idx_save = 0;
		for (int l60 = 0; (l60 < 16384); l60 = (l60 + 1)) {
			fYec24[l60] = 0.0f;
			
		}
		fYec24_idx = 0;
		fYec24_idx_save = 0;
		for (int l61 = 0; (l61 < 4); l61 = (l61 + 1)) {
			fYec25_perm[l61] = 0.0f;
			
		}
		for (int l62 = 0; (l62 < 4); l62 = (l62 + 1)) {
			fRec69_perm[l62] = 0.0f;
			
		}
		for (int l63 = 0; (l63 < 4); l63 = (l63 + 1)) {
			fRec71_perm[l63] = 0.0f;
			
		}
		for (int l64 = 0; (l64 < 16384); l64 = (l64 + 1)) {
			fYec26[l64] = 0.0f;
			
		}
		fYec26_idx = 0;
		fYec26_idx_save = 0;
		for (int l65 = 0; (l65 < 4); l65 = (l65 + 1)) {
			fYec27_perm[l65] = 0.0f;
			
		}
		for (int l66 = 0; (l66 < 4); l66 = (l66 + 1)) {
			fRec36_perm[l66] = 0.0f;
			
		}
		for (int l67 = 0; (l67 < 4); l67 = (l67 + 1)) {
			fRec34_perm[l67] = 0.0f;
			
		}
		for (int l68 = 0; (l68 < 4); l68 = (l68 + 1)) {
			fRec73_perm[l68] = 0.0f;
			
		}
		for (int l69 = 0; (l69 < 16384); l69 = (l69 + 1)) {
			fYec28[l69] = 0.0f;
			
		}
		fYec28_idx = 0;
		fYec28_idx_save = 0;
		for (int l70 = 0; (l70 < 4); l70 = (l70 + 1)) {
			fYec29_perm[l70] = 0.0f;
			
		}
		for (int l71 = 0; (l71 < 4); l71 = (l71 + 1)) {
			fRec72_perm[l71] = 0.0f;
			
		}
		for (int l72 = 0; (l72 < 4); l72 = (l72 + 1)) {
			fRec35_perm[l72] = 0.0f;
			
		}
		for (int l73 = 0; (l73 < 4); l73 = (l73 + 1)) {
			fRec74_perm[l73] = 0.0f;
			
		}
		for (int l74 = 0; (l74 < 16384); l74 = (l74 + 1)) {
			fYec30[l74] = 0.0f;
			
		}
		fYec30_idx = 0;
		fYec30_idx_save = 0;
		for (int l75 = 0; (l75 < 4); l75 = (l75 + 1)) {
			fYec31_perm[l75] = 0.0f;
			
		}
		for (int l76 = 0; (l76 < 4); l76 = (l76 + 1)) {
			fRec33_perm[l76] = 0.0f;
			
		}
		for (int l77 = 0; (l77 < 4); l77 = (l77 + 1)) {
			fRec31_perm[l77] = 0.0f;
			
		}
		for (int l78 = 0; (l78 < 4); l78 = (l78 + 1)) {
			fRec76_perm[l78] = 0.0f;
			
		}
		for (int l79 = 0; (l79 < 16384); l79 = (l79 + 1)) {
			fYec32[l79] = 0.0f;
			
		}
		fYec32_idx = 0;
		fYec32_idx_save = 0;
		for (int l80 = 0; (l80 < 4); l80 = (l80 + 1)) {
			fYec33_perm[l80] = 0.0f;
			
		}
		for (int l81 = 0; (l81 < 4); l81 = (l81 + 1)) {
			fRec75_perm[l81] = 0.0f;
			
		}
		for (int l82 = 0; (l82 < 4); l82 = (l82 + 1)) {
			fRec32_perm[l82] = 0.0f;
			
		}
		for (int l83 = 0; (l83 < 4); l83 = (l83 + 1)) {
			fRec77_perm[l83] = 0.0f;
			
		}
		for (int l84 = 0; (l84 < 16384); l84 = (l84 + 1)) {
			fYec34[l84] = 0.0f;
			
		}
		fYec34_idx = 0;
		fYec34_idx_save = 0;
		for (int l85 = 0; (l85 < 4); l85 = (l85 + 1)) {
			fYec35_perm[l85] = 0.0f;
			
		}
		for (int l86 = 0; (l86 < 4); l86 = (l86 + 1)) {
			fRec30_perm[l86] = 0.0f;
			
		}
		for (int l87 = 0; (l87 < 4); l87 = (l87 + 1)) {
			fRec28_perm[l87] = 0.0f;
			
		}
		for (int l88 = 0; (l88 < 4); l88 = (l88 + 1)) {
			fRec79_perm[l88] = 0.0f;
			
		}
		for (int l89 = 0; (l89 < 16384); l89 = (l89 + 1)) {
			fYec36[l89] = 0.0f;
			
		}
		fYec36_idx = 0;
		fYec36_idx_save = 0;
		for (int l90 = 0; (l90 < 4); l90 = (l90 + 1)) {
			fYec37_perm[l90] = 0.0f;
			
		}
		for (int l91 = 0; (l91 < 4); l91 = (l91 + 1)) {
			fRec78_perm[l91] = 0.0f;
			
		}
		for (int l92 = 0; (l92 < 4); l92 = (l92 + 1)) {
			fRec29_perm[l92] = 0.0f;
			
		}
		for (int l93 = 0; (l93 < 4); l93 = (l93 + 1)) {
			fRec80_perm[l93] = 0.0f;
			
		}
		for (int l94 = 0; (l94 < 16384); l94 = (l94 + 1)) {
			fYec38[l94] = 0.0f;
			
		}
		fYec38_idx = 0;
		fYec38_idx_save = 0;
		for (int l95 = 0; (l95 < 4); l95 = (l95 + 1)) {
			fYec39_perm[l95] = 0.0f;
			
		}
		for (int l96 = 0; (l96 < 4); l96 = (l96 + 1)) {
			fRec27_perm[l96] = 0.0f;
			
		}
		for (int l97 = 0; (l97 < 4); l97 = (l97 + 1)) {
			fRec25_perm[l97] = 0.0f;
			
		}
		for (int l98 = 0; (l98 < 4); l98 = (l98 + 1)) {
			fRec82_perm[l98] = 0.0f;
			
		}
		for (int l99 = 0; (l99 < 16384); l99 = (l99 + 1)) {
			fYec40[l99] = 0.0f;
			
		}
		fYec40_idx = 0;
		fYec40_idx_save = 0;
		for (int l100 = 0; (l100 < 4); l100 = (l100 + 1)) {
			fYec41_perm[l100] = 0.0f;
			
		}
		for (int l101 = 0; (l101 < 4); l101 = (l101 + 1)) {
			fRec81_perm[l101] = 0.0f;
			
		}
		for (int l102 = 0; (l102 < 4); l102 = (l102 + 1)) {
			fRec26_perm[l102] = 0.0f;
			
		}
		for (int l103 = 0; (l103 < 4); l103 = (l103 + 1)) {
			fRec83_perm[l103] = 0.0f;
			
		}
		for (int l104 = 0; (l104 < 16384); l104 = (l104 + 1)) {
			fYec42[l104] = 0.0f;
			
		}
		fYec42_idx = 0;
		fYec42_idx_save = 0;
		for (int l105 = 0; (l105 < 4); l105 = (l105 + 1)) {
			fYec43_perm[l105] = 0.0f;
			
		}
		for (int l106 = 0; (l106 < 4); l106 = (l106 + 1)) {
			fRec24_perm[l106] = 0.0f;
			
		}
		for (int l107 = 0; (l107 < 4); l107 = (l107 + 1)) {
			fRec22_perm[l107] = 0.0f;
			
		}
		for (int l108 = 0; (l108 < 4); l108 = (l108 + 1)) {
			fRec85_perm[l108] = 0.0f;
			
		}
		for (int l109 = 0; (l109 < 16384); l109 = (l109 + 1)) {
			fYec44[l109] = 0.0f;
			
		}
		fYec44_idx = 0;
		fYec44_idx_save = 0;
		for (int l110 = 0; (l110 < 4); l110 = (l110 + 1)) {
			fYec45_perm[l110] = 0.0f;
			
		}
		for (int l111 = 0; (l111 < 4); l111 = (l111 + 1)) {
			fRec84_perm[l111] = 0.0f;
			
		}
		for (int l112 = 0; (l112 < 4); l112 = (l112 + 1)) {
			fRec23_perm[l112] = 0.0f;
			
		}
		for (int l113 = 0; (l113 < 4); l113 = (l113 + 1)) {
			fRec86_perm[l113] = 0.0f;
			
		}
		for (int l114 = 0; (l114 < 16384); l114 = (l114 + 1)) {
			fYec46[l114] = 0.0f;
			
		}
		fYec46_idx = 0;
		fYec46_idx_save = 0;
		for (int l115 = 0; (l115 < 16384); l115 = (l115 + 1)) {
			fYec47[l115] = 0.0f;
			
		}
		fYec47_idx = 0;
		fYec47_idx_save = 0;
		for (int l116 = 0; (l116 < 4); l116 = (l116 + 1)) {
			fYec48_perm[l116] = 0.0f;
			
		}
		for (int l117 = 0; (l117 < 4); l117 = (l117 + 1)) {
			fRec21_perm[l117] = 0.0f;
			
		}
		for (int l118 = 0; (l118 < 4); l118 = (l118 + 1)) {
			fRec20_perm[l118] = 0.0f;
			
		}
		for (int l119 = 0; (l119 < 4); l119 = (l119 + 1)) {
			fRec19_perm[l119] = 0.0f;
			
		}
		for (int l120 = 0; (l120 < 4); l120 = (l120 + 1)) {
			fRec18_perm[l120] = 0.0f;
			
		}
		for (int l121 = 0; (l121 < 4); l121 = (l121 + 1)) {
			fRec17_perm[l121] = 0.0f;
			
		}
		for (int l122 = 0; (l122 < 4); l122 = (l122 + 1)) {
			fRec92_perm[l122] = 0.0f;
			
		}
		for (int l123 = 0; (l123 < 4); l123 = (l123 + 1)) {
			fRec91_perm[l123] = 0.0f;
			
		}
		for (int l124 = 0; (l124 < 4); l124 = (l124 + 1)) {
			fRec90_perm[l124] = 0.0f;
			
		}
		for (int l125 = 0; (l125 < 4); l125 = (l125 + 1)) {
			fYec49_perm[l125] = 0.0f;
			
		}
		for (int l126 = 0; (l126 < 4); l126 = (l126 + 1)) {
			fRec89_perm[l126] = 0.0f;
			
		}
		for (int l127 = 0; (l127 < 4); l127 = (l127 + 1)) {
			fRec88_perm[l127] = 0.0f;
			
		}
		for (int l128 = 0; (l128 < 4); l128 = (l128 + 1)) {
			fRec87_perm[l128] = 0.0f;
			
		}
		for (int l129 = 0; (l129 < 4); l129 = (l129 + 1)) {
			fRec95_perm[l129] = 0.0f;
			
		}
		for (int l130 = 0; (l130 < 4); l130 = (l130 + 1)) {
			fRec94_perm[l130] = 0.0f;
			
		}
		for (int l131 = 0; (l131 < 4); l131 = (l131 + 1)) {
			fRec93_perm[l131] = 0.0f;
			
		}
		for (int l132 = 0; (l132 < 1024); l132 = (l132 + 1)) {
			fYec50[l132] = 0.0f;
			
		}
		fYec50_idx = 0;
		fYec50_idx_save = 0;
		for (int l133 = 0; (l133 < 4); l133 = (l133 + 1)) {
			fRec14_perm[l133] = 0.0f;
			
		}
		for (int l134 = 0; (l134 < 4); l134 = (l134 + 1)) {
			fRec102_perm[l134] = 0.0f;
			
		}
		for (int l135 = 0; (l135 < 16384); l135 = (l135 + 1)) {
			fYec51[l135] = 0.0f;
			
		}
		fYec51_idx = 0;
		fYec51_idx_save = 0;
		for (int l136 = 0; (l136 < 16384); l136 = (l136 + 1)) {
			fYec52[l136] = 0.0f;
			
		}
		fYec52_idx = 0;
		fYec52_idx_save = 0;
		for (int l137 = 0; (l137 < 4); l137 = (l137 + 1)) {
			fYec53_perm[l137] = 0.0f;
			
		}
		for (int l138 = 0; (l138 < 4); l138 = (l138 + 1)) {
			fRec101_perm[l138] = 0.0f;
			
		}
		for (int l139 = 0; (l139 < 4); l139 = (l139 + 1)) {
			fRec100_perm[l139] = 0.0f;
			
		}
		for (int l140 = 0; (l140 < 4); l140 = (l140 + 1)) {
			fRec99_perm[l140] = 0.0f;
			
		}
		for (int l141 = 0; (l141 < 4); l141 = (l141 + 1)) {
			fRec98_perm[l141] = 0.0f;
			
		}
		for (int l142 = 0; (l142 < 4); l142 = (l142 + 1)) {
			fRec97_perm[l142] = 0.0f;
			
		}
		for (int l143 = 0; (l143 < 4); l143 = (l143 + 1)) {
			fRec108_perm[l143] = 0.0f;
			
		}
		for (int l144 = 0; (l144 < 4); l144 = (l144 + 1)) {
			fRec107_perm[l144] = 0.0f;
			
		}
		for (int l145 = 0; (l145 < 4); l145 = (l145 + 1)) {
			fRec106_perm[l145] = 0.0f;
			
		}
		for (int l146 = 0; (l146 < 4); l146 = (l146 + 1)) {
			fYec54_perm[l146] = 0.0f;
			
		}
		for (int l147 = 0; (l147 < 4); l147 = (l147 + 1)) {
			fRec105_perm[l147] = 0.0f;
			
		}
		for (int l148 = 0; (l148 < 4); l148 = (l148 + 1)) {
			fRec104_perm[l148] = 0.0f;
			
		}
		for (int l149 = 0; (l149 < 4); l149 = (l149 + 1)) {
			fRec103_perm[l149] = 0.0f;
			
		}
		for (int l150 = 0; (l150 < 4); l150 = (l150 + 1)) {
			fRec111_perm[l150] = 0.0f;
			
		}
		for (int l151 = 0; (l151 < 4); l151 = (l151 + 1)) {
			fRec110_perm[l151] = 0.0f;
			
		}
		for (int l152 = 0; (l152 < 4); l152 = (l152 + 1)) {
			fRec109_perm[l152] = 0.0f;
			
		}
		for (int l153 = 0; (l153 < 1024); l153 = (l153 + 1)) {
			fYec55[l153] = 0.0f;
			
		}
		fYec55_idx = 0;
		fYec55_idx_save = 0;
		for (int l154 = 0; (l154 < 4); l154 = (l154 + 1)) {
			fRec96_perm[l154] = 0.0f;
			
		}
		for (int l155 = 0; (l155 < 16384); l155 = (l155 + 1)) {
			fYec56[l155] = 0.0f;
			
		}
		fYec56_idx = 0;
		fYec56_idx_save = 0;
		for (int l156 = 0; (l156 < 4); l156 = (l156 + 1)) {
			fYec57_perm[l156] = 0.0f;
			
		}
		for (int l157 = 0; (l157 < 4); l157 = (l157 + 1)) {
			fRec13_perm[l157] = 0.0f;
			
		}
		for (int l158 = 0; (l158 < 4); l158 = (l158 + 1)) {
			fRec11_perm[l158] = 0.0f;
			
		}
		for (int l159 = 0; (l159 < 4); l159 = (l159 + 1)) {
			fRec113_perm[l159] = 0.0f;
			
		}
		for (int l160 = 0; (l160 < 16384); l160 = (l160 + 1)) {
			fYec58[l160] = 0.0f;
			
		}
		fYec58_idx = 0;
		fYec58_idx_save = 0;
		for (int l161 = 0; (l161 < 4); l161 = (l161 + 1)) {
			fYec59_perm[l161] = 0.0f;
			
		}
		for (int l162 = 0; (l162 < 4); l162 = (l162 + 1)) {
			fRec112_perm[l162] = 0.0f;
			
		}
		for (int l163 = 0; (l163 < 4); l163 = (l163 + 1)) {
			fRec12_perm[l163] = 0.0f;
			
		}
		for (int l164 = 0; (l164 < 16384); l164 = (l164 + 1)) {
			fYec60[l164] = 0.0f;
			
		}
		fYec60_idx = 0;
		fYec60_idx_save = 0;
		for (int l165 = 0; (l165 < 4); l165 = (l165 + 1)) {
			fYec61_perm[l165] = 0.0f;
			
		}
		for (int l166 = 0; (l166 < 4); l166 = (l166 + 1)) {
			fRec10_perm[l166] = 0.0f;
			
		}
		for (int l167 = 0; (l167 < 4); l167 = (l167 + 1)) {
			fRec8_perm[l167] = 0.0f;
			
		}
		for (int l168 = 0; (l168 < 16384); l168 = (l168 + 1)) {
			fYec62[l168] = 0.0f;
			
		}
		fYec62_idx = 0;
		fYec62_idx_save = 0;
		for (int l169 = 0; (l169 < 4); l169 = (l169 + 1)) {
			fYec63_perm[l169] = 0.0f;
			
		}
		for (int l170 = 0; (l170 < 4); l170 = (l170 + 1)) {
			fRec114_perm[l170] = 0.0f;
			
		}
		for (int l171 = 0; (l171 < 4); l171 = (l171 + 1)) {
			fRec9_perm[l171] = 0.0f;
			
		}
		for (int l172 = 0; (l172 < 16384); l172 = (l172 + 1)) {
			fYec64[l172] = 0.0f;
			
		}
		fYec64_idx = 0;
		fYec64_idx_save = 0;
		for (int l173 = 0; (l173 < 4); l173 = (l173 + 1)) {
			fYec65_perm[l173] = 0.0f;
			
		}
		for (int l174 = 0; (l174 < 4); l174 = (l174 + 1)) {
			fRec7_perm[l174] = 0.0f;
			
		}
		for (int l175 = 0; (l175 < 4); l175 = (l175 + 1)) {
			fRec5_perm[l175] = 0.0f;
			
		}
		for (int l176 = 0; (l176 < 4); l176 = (l176 + 1)) {
			fRec116_perm[l176] = 0.0f;
			
		}
		for (int l177 = 0; (l177 < 16384); l177 = (l177 + 1)) {
			fYec66[l177] = 0.0f;
			
		}
		fYec66_idx = 0;
		fYec66_idx_save = 0;
		for (int l178 = 0; (l178 < 4); l178 = (l178 + 1)) {
			fYec67_perm[l178] = 0.0f;
			
		}
		for (int l179 = 0; (l179 < 4); l179 = (l179 + 1)) {
			fRec115_perm[l179] = 0.0f;
			
		}
		for (int l180 = 0; (l180 < 4); l180 = (l180 + 1)) {
			fRec6_perm[l180] = 0.0f;
			
		}
		for (int l181 = 0; (l181 < 4); l181 = (l181 + 1)) {
			fRec117_perm[l181] = 0.0f;
			
		}
		for (int l182 = 0; (l182 < 16384); l182 = (l182 + 1)) {
			fYec68[l182] = 0.0f;
			
		}
		fYec68_idx = 0;
		fYec68_idx_save = 0;
		for (int l183 = 0; (l183 < 4); l183 = (l183 + 1)) {
			fYec69_perm[l183] = 0.0f;
			
		}
		for (int l184 = 0; (l184 < 4); l184 = (l184 + 1)) {
			fRec4_perm[l184] = 0.0f;
			
		}
		for (int l185 = 0; (l185 < 4); l185 = (l185 + 1)) {
			fRec2_perm[l185] = 0.0f;
			
		}
		for (int l186 = 0; (l186 < 16384); l186 = (l186 + 1)) {
			fYec70[l186] = 0.0f;
			
		}
		fYec70_idx = 0;
		fYec70_idx_save = 0;
		for (int l187 = 0; (l187 < 4); l187 = (l187 + 1)) {
			fYec71_perm[l187] = 0.0f;
			
		}
		for (int l188 = 0; (l188 < 4); l188 = (l188 + 1)) {
			fRec118_perm[l188] = 0.0f;
			
		}
		for (int l189 = 0; (l189 < 4); l189 = (l189 + 1)) {
			fRec3_perm[l189] = 0.0f;
			
		}
		for (int l190 = 0; (l190 < 4); l190 = (l190 + 1)) {
			fRec0_perm[l190] = 0.0f;
			
		}
		for (int l191 = 0; (l191 < 4); l191 = (l191 + 1)) {
			fRec1_perm[l191] = 0.0f;
			
		}
		
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
		ui_interface->openVerticalBox("JPverbRaw");
		ui_interface->addHorizontalSlider("damp", &fHslider5, 0.0f, 0.0f, 0.999000013f, 9.99999975e-05f);
		ui_interface->addHorizontalSlider("earlyDiff", &fHslider10, 0.707000017f, 0.0f, 0.99000001f, 0.00100000005f);
		ui_interface->addHorizontalSlider("highBand", &fHslider3, 2000.0f, 1000.0f, 10000.0f, 0.100000001f);
		ui_interface->addHorizontalSlider("highX", &fHslider7, 1.0f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("lowBand", &fHslider4, 500.0f, 100.0f, 6000.0f, 0.100000001f);
		ui_interface->addHorizontalSlider("lowX", &fHslider9, 1.0f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("mDepth", &fHslider2, 0.100000001f, 0.0f, 1.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("mFreq", &fHslider0, 2.0f, 0.0f, 10.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("midX", &fHslider8, 1.0f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("size", &fHslider1, 1.0f, 0.5f, 3.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("t60", &fHslider6, 1.0f, 0.100000001f, 60.0f, 0.100000001f);
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0_ptr = inputs[0];
		FAUSTFLOAT* input1_ptr = inputs[1];
		FAUSTFLOAT* output0_ptr = outputs[0];
		FAUSTFLOAT* output1_ptr = outputs[1];
		float fSlow0 = (fConst1 * float(fHslider0));
		float fSlow1 = std::sin(fSlow0);
		float fSlow2 = std::cos(fSlow0);
		float fRec15_tmp[12];
		float* fRec15 = &fRec15_tmp[4];
		int iVec0_tmp[12];
		int* iVec0 = &iVec0_tmp[4];
		int iZec0[8];
		float fRec16_tmp[12];
		float* fRec16 = &fRec16_tmp[4];
		float fSlow3 = float(fHslider1);
		int iSlow4 = int(primes(int((10.0f * fSlow3))));
		float fSlow5 = (9.99999975e-05f * float(iSlow4));
		float fRec53_tmp[12];
		float* fRec53 = &fRec53_tmp[4];
		float fZec1[8];
		float fZec2[8];
		float fZec3[8];
		float fYec1_tmp[12];
		float* fYec1 = &fYec1_tmp[4];
		float fZec4[8];
		float fRec52_tmp[12];
		float* fRec52 = &fRec52_tmp[4];
		float fRec50_tmp[12];
		float* fRec50 = &fRec50_tmp[4];
		int iSlow6 = int(primes(int((110.0f * fSlow3))));
		float fSlow7 = (9.99999975e-05f * float(iSlow6));
		float fRec55_tmp[12];
		float* fRec55 = &fRec55_tmp[4];
		float fZec5[8];
		float fYec3_tmp[12];
		float* fYec3 = &fYec3_tmp[4];
		float fZec6[8];
		float fRec54_tmp[12];
		float* fRec54 = &fRec54_tmp[4];
		float fRec51_tmp[12];
		float* fRec51 = &fRec51_tmp[4];
		int iSlow8 = int(primes(int((40.0f * fSlow3))));
		float fSlow9 = (9.99999975e-05f * float(iSlow8));
		float fRec56_tmp[12];
		float* fRec56 = &fRec56_tmp[4];
		float fZec7[8];
		float fZec8[8];
		float fZec9[8];
		float fZec10[8];
		float fZec11[8];
		float fYec5_tmp[12];
		float* fYec5 = &fYec5_tmp[4];
		float fZec12[8];
		float fRec49_tmp[12];
		float* fRec49 = &fRec49_tmp[4];
		float fRec47_tmp[12];
		float* fRec47 = &fRec47_tmp[4];
		int iSlow10 = int(primes(int((140.0f * fSlow3))));
		float fSlow11 = (9.99999975e-05f * float(iSlow10));
		float fRec58_tmp[12];
		float* fRec58 = &fRec58_tmp[4];
		float fZec13[8];
		float fYec7_tmp[12];
		float* fYec7 = &fYec7_tmp[4];
		float fZec14[8];
		float fRec57_tmp[12];
		float* fRec57 = &fRec57_tmp[4];
		float fRec48_tmp[12];
		float* fRec48 = &fRec48_tmp[4];
		int iSlow12 = int(primes(int((70.0f * fSlow3))));
		float fSlow13 = (9.99999975e-05f * float(iSlow12));
		float fRec59_tmp[12];
		float* fRec59 = &fRec59_tmp[4];
		float fZec15[8];
		float fZec16[8];
		float fZec17[8];
		float fZec18[8];
		float fZec19[8];
		float fYec9_tmp[12];
		float* fYec9 = &fYec9_tmp[4];
		float fZec20[8];
		float fRec46_tmp[12];
		float* fRec46 = &fRec46_tmp[4];
		float fRec44_tmp[12];
		float* fRec44 = &fRec44_tmp[4];
		int iSlow14 = int(primes(int((170.0f * fSlow3))));
		float fSlow15 = (9.99999975e-05f * float(iSlow14));
		float fRec61_tmp[12];
		float* fRec61 = &fRec61_tmp[4];
		float fZec21[8];
		float fYec11_tmp[12];
		float* fYec11 = &fYec11_tmp[4];
		float fZec22[8];
		float fRec60_tmp[12];
		float* fRec60 = &fRec60_tmp[4];
		float fRec45_tmp[12];
		float* fRec45 = &fRec45_tmp[4];
		int iSlow16 = int(primes(int((100.0f * fSlow3))));
		float fSlow17 = (9.99999975e-05f * float(iSlow16));
		float fRec62_tmp[12];
		float* fRec62 = &fRec62_tmp[4];
		float fZec23[8];
		float fZec24[8];
		float fZec25[8];
		float fZec26[8];
		float fZec27[8];
		float fYec13_tmp[12];
		float* fYec13 = &fYec13_tmp[4];
		float fZec28[8];
		float fRec43_tmp[12];
		float* fRec43 = &fRec43_tmp[4];
		float fRec41_tmp[12];
		float* fRec41 = &fRec41_tmp[4];
		int iSlow18 = int(primes(int((200.0f * fSlow3))));
		float fSlow19 = (9.99999975e-05f * float(iSlow18));
		float fRec64_tmp[12];
		float* fRec64 = &fRec64_tmp[4];
		float fZec29[8];
		float fYec15_tmp[12];
		float* fYec15 = &fYec15_tmp[4];
		float fZec30[8];
		float fRec63_tmp[12];
		float* fRec63 = &fRec63_tmp[4];
		float fRec42_tmp[12];
		float* fRec42 = &fRec42_tmp[4];
		int iSlow20 = int(primes(int((130.0f * fSlow3))));
		float fSlow21 = (9.99999975e-05f * float(iSlow20));
		float fRec65_tmp[12];
		float* fRec65 = &fRec65_tmp[4];
		float fZec31[8];
		float fZec32[8];
		float fZec33[8];
		float fZec34[8];
		float fZec35[8];
		float fYec17_tmp[12];
		float* fYec17 = &fYec17_tmp[4];
		float fZec36[8];
		float fRec40_tmp[12];
		float* fRec40 = &fRec40_tmp[4];
		float fRec38_tmp[12];
		float* fRec38 = &fRec38_tmp[4];
		int iSlow22 = int(primes(int((230.0f * fSlow3))));
		float fSlow23 = (9.99999975e-05f * float(iSlow22));
		float fRec67_tmp[12];
		float* fRec67 = &fRec67_tmp[4];
		float fZec37[8];
		float fYec19_tmp[12];
		float* fYec19 = &fYec19_tmp[4];
		float fZec38[8];
		float fRec66_tmp[12];
		float* fRec66 = &fRec66_tmp[4];
		float fRec39_tmp[12];
		float* fRec39 = &fRec39_tmp[4];
		int iSlow24 = int(primes(int((54.0f * fSlow3))));
		float fSlow25 = (0.00499999989f * float(iSlow24));
		float fRec68_tmp[12];
		float* fRec68 = &fRec68_tmp[4];
		float fSlow26 = (50.0f * float(fHslider2));
		float fZec39[8];
		float fZec40[8];
		int iZec41[8];
		int iZec42[8];
		float fZec43[8];
		float fZec44[8];
		float fZec45[8];
		float fZec46[8];
		float fZec47[8];
		float fZec48[8];
		float fZec49[8];
		float fZec50[8];
		float fZec51[8];
		float fZec52[8];
		int iZec53[8];
		float fZec54[8];
		float fZec55[8];
		float fZec56[8];
		int iZec57[8];
		float fZec58[8];
		float fZec59[8];
		float fZec60[8];
		int iZec61[8];
		float fZec62[8];
		float fZec63[8];
		int iZec64[8];
		float fZec65[8];
		float fYec22_tmp[12];
		float* fYec22 = &fYec22_tmp[4];
		float fZec66[8];
		float fRec37_tmp[12];
		float* fRec37 = &fRec37_tmp[4];
		int iSlow27 = int(primes(int((204.0f * fSlow3))));
		float fSlow28 = (0.00499999989f * float(iSlow27));
		float fRec70_tmp[12];
		float* fRec70 = &fRec70_tmp[4];
		float fSlow29 = (0.0f - fSlow26);
		float fZec67[8];
		float fZec68[8];
		int iZec69[8];
		float fZec70[8];
		float fZec71[8];
		float fZec72[8];
		float fZec73[8];
		float fZec74[8];
		float fZec75[8];
		float fZec76[8];
		float fYec25_tmp[12];
		float* fYec25 = &fYec25_tmp[4];
		float fZec77[8];
		float fRec69_tmp[12];
		float* fRec69 = &fRec69_tmp[4];
		int iSlow30 = int(primes(int((125.0f * fSlow3))));
		float fSlow31 = (9.99999975e-05f * float(iSlow30));
		float fRec71_tmp[12];
		float* fRec71 = &fRec71_tmp[4];
		float fZec78[8];
		float fZec79[8];
		float fZec80[8];
		float fYec27_tmp[12];
		float* fYec27 = &fYec27_tmp[4];
		float fZec81[8];
		float fRec36_tmp[12];
		float* fRec36 = &fRec36_tmp[4];
		float fRec34_tmp[12];
		float* fRec34 = &fRec34_tmp[4];
		int iSlow32 = int(primes(int((25.0f * fSlow3))));
		float fSlow33 = (9.99999975e-05f * float(iSlow32));
		float fRec73_tmp[12];
		float* fRec73 = &fRec73_tmp[4];
		float fZec82[8];
		float fYec29_tmp[12];
		float* fYec29 = &fYec29_tmp[4];
		float fZec83[8];
		float fRec72_tmp[12];
		float* fRec72 = &fRec72_tmp[4];
		float fRec35_tmp[12];
		float* fRec35 = &fRec35_tmp[4];
		int iSlow34 = int(primes(int((155.0f * fSlow3))));
		float fSlow35 = (9.99999975e-05f * float(iSlow34));
		float fRec74_tmp[12];
		float* fRec74 = &fRec74_tmp[4];
		float fZec84[8];
		float fZec85[8];
		float fZec86[8];
		float fZec87[8];
		float fZec88[8];
		float fYec31_tmp[12];
		float* fYec31 = &fYec31_tmp[4];
		float fZec89[8];
		float fRec33_tmp[12];
		float* fRec33 = &fRec33_tmp[4];
		float fRec31_tmp[12];
		float* fRec31 = &fRec31_tmp[4];
		int iSlow36 = int(primes(int((55.0f * fSlow3))));
		float fSlow37 = (9.99999975e-05f * float(iSlow36));
		float fRec76_tmp[12];
		float* fRec76 = &fRec76_tmp[4];
		float fZec90[8];
		int iZec91[8];
		float fYec33_tmp[12];
		float* fYec33 = &fYec33_tmp[4];
		float fZec92[8];
		float fZec93[8];
		float fZec94[8];
		float fRec75_tmp[12];
		float* fRec75 = &fRec75_tmp[4];
		float fRec32_tmp[12];
		float* fRec32 = &fRec32_tmp[4];
		int iSlow38 = int(primes(int((185.0f * fSlow3))));
		float fSlow39 = (9.99999975e-05f * float(iSlow38));
		float fRec77_tmp[12];
		float* fRec77 = &fRec77_tmp[4];
		float fZec95[8];
		float fZec96[8];
		float fZec97[8];
		float fZec98[8];
		float fZec99[8];
		float fYec35_tmp[12];
		float* fYec35 = &fYec35_tmp[4];
		float fZec100[8];
		float fRec30_tmp[12];
		float* fRec30 = &fRec30_tmp[4];
		float fRec28_tmp[12];
		float* fRec28 = &fRec28_tmp[4];
		int iSlow40 = int(primes(int((85.0f * fSlow3))));
		float fSlow41 = (9.99999975e-05f * float(iSlow40));
		float fRec79_tmp[12];
		float* fRec79 = &fRec79_tmp[4];
		float fZec101[8];
		int iZec102[8];
		float fYec37_tmp[12];
		float* fYec37 = &fYec37_tmp[4];
		float fZec103[8];
		float fZec104[8];
		float fZec105[8];
		float fRec78_tmp[12];
		float* fRec78 = &fRec78_tmp[4];
		float fRec29_tmp[12];
		float* fRec29 = &fRec29_tmp[4];
		int iSlow42 = int(primes(int((215.0f * fSlow3))));
		float fSlow43 = (9.99999975e-05f * float(iSlow42));
		float fRec80_tmp[12];
		float* fRec80 = &fRec80_tmp[4];
		float fZec106[8];
		float fZec107[8];
		float fZec108[8];
		float fZec109[8];
		float fZec110[8];
		int iZec111[8];
		float fYec39_tmp[12];
		float* fYec39 = &fYec39_tmp[4];
		float fZec112[8];
		float fZec113[8];
		float fZec114[8];
		float fRec27_tmp[12];
		float* fRec27 = &fRec27_tmp[4];
		float fRec25_tmp[12];
		float* fRec25 = &fRec25_tmp[4];
		int iSlow44 = int(primes(int((115.0f * fSlow3))));
		float fSlow45 = (9.99999975e-05f * float(iSlow44));
		float fRec82_tmp[12];
		float* fRec82 = &fRec82_tmp[4];
		float fZec115[8];
		int iZec116[8];
		float fYec41_tmp[12];
		float* fYec41 = &fYec41_tmp[4];
		float fZec117[8];
		float fZec118[8];
		float fZec119[8];
		float fRec81_tmp[12];
		float* fRec81 = &fRec81_tmp[4];
		float fRec26_tmp[12];
		float* fRec26 = &fRec26_tmp[4];
		int iSlow46 = int(primes(int((245.0f * fSlow3))));
		float fSlow47 = (9.99999975e-05f * float(iSlow46));
		float fRec83_tmp[12];
		float* fRec83 = &fRec83_tmp[4];
		float fZec120[8];
		float fZec121[8];
		float fZec122[8];
		float fZec123[8];
		float fZec124[8];
		float fYec43_tmp[12];
		float* fYec43 = &fYec43_tmp[4];
		float fZec125[8];
		float fRec24_tmp[12];
		float* fRec24 = &fRec24_tmp[4];
		float fRec22_tmp[12];
		float* fRec22 = &fRec22_tmp[4];
		int iSlow48 = int(primes(int((145.0f * fSlow3))));
		float fSlow49 = (9.99999975e-05f * float(iSlow48));
		float fRec85_tmp[12];
		float* fRec85 = &fRec85_tmp[4];
		float fZec126[8];
		int iZec127[8];
		float fYec45_tmp[12];
		float* fYec45 = &fYec45_tmp[4];
		float fZec128[8];
		float fZec129[8];
		float fZec130[8];
		float fRec84_tmp[12];
		float* fRec84 = &fRec84_tmp[4];
		float fRec23_tmp[12];
		float* fRec23 = &fRec23_tmp[4];
		int iSlow50 = int(primes(int((134.0f * fSlow3))));
		float fSlow51 = (0.00499999989f * float(iSlow50));
		float fRec86_tmp[12];
		float* fRec86 = &fRec86_tmp[4];
		float fZec131[8];
		float fZec132[8];
		int iZec133[8];
		int iZec134[8];
		float fZec135[8];
		float fZec136[8];
		float fZec137[8];
		float fZec138[8];
		float fZec139[8];
		float fZec140[8];
		float fZec141[8];
		float fZec142[8];
		float fZec143[8];
		float fZec144[8];
		int iZec145[8];
		float fZec146[8];
		float fZec147[8];
		float fZec148[8];
		int iZec149[8];
		float fZec150[8];
		float fZec151[8];
		float fZec152[8];
		int iZec153[8];
		float fZec154[8];
		float fZec155[8];
		int iZec156[8];
		float fZec157[8];
		float fYec48_tmp[12];
		float* fYec48 = &fYec48_tmp[4];
		float fZec158[8];
		float fRec21_tmp[12];
		float* fRec21 = &fRec21_tmp[4];
		float fSlow52 = std::tan((fConst2 * float(fHslider3)));
		float fSlow53 = (1.0f / fSlow52);
		float fSlow54 = (fSlow53 + 1.0f);
		float fSlow55 = (0.0f - (1.0f / (fSlow54 * fSlow52)));
		float fSlow56 = (1.0f / fSlow54);
		float fSlow57 = (1.0f - fSlow53);
		float fRec20_tmp[12];
		float* fRec20 = &fRec20_tmp[4];
		float fSlow58 = (1.0f / (((fSlow53 + 1.61803401f) / fSlow52) + 1.0f));
		float fSlow59 = (((fSlow53 + -1.61803401f) / fSlow52) + 1.0f);
		float fSlow60 = mydsp_faustpower2_f(fSlow52);
		float fSlow61 = (1.0f / fSlow60);
		float fSlow62 = (2.0f * (1.0f - fSlow61));
		float fRec19_tmp[12];
		float* fRec19 = &fRec19_tmp[4];
		float fSlow63 = (0.0f - (2.0f / fSlow60));
		float fSlow64 = (((fSlow53 + 0.618034005f) / fSlow52) + 1.0f);
		float fSlow65 = (1.0f / fSlow64);
		float fSlow66 = (((fSlow53 + -0.618034005f) / fSlow52) + 1.0f);
		float fRec18_tmp[12];
		float* fRec18 = &fRec18_tmp[4];
		float fSlow67 = std::tan((fConst2 * float(fHslider4)));
		float fSlow68 = (1.0f / fSlow67);
		float fSlow69 = (1.0f / (((fSlow68 + 1.61803401f) / fSlow67) + 1.0f));
		float fSlow70 = (((fSlow68 + -1.61803401f) / fSlow67) + 1.0f);
		float fSlow71 = mydsp_faustpower2_f(fSlow67);
		float fSlow72 = (1.0f / fSlow71);
		float fSlow73 = (2.0f * (1.0f - fSlow72));
		float fZec159[8];
		float fRec17_tmp[12];
		float* fRec17 = &fRec17_tmp[4];
		float fRec92_tmp[12];
		float* fRec92 = &fRec92_tmp[4];
		float fRec91_tmp[12];
		float* fRec91 = &fRec91_tmp[4];
		float fRec90_tmp[12];
		float* fRec90 = &fRec90_tmp[4];
		float fSlow74 = (fSlow68 + 1.0f);
		float fSlow75 = (0.0f - (1.0f / (fSlow74 * fSlow67)));
		float fZec160[8];
		float fYec49_tmp[12];
		float* fYec49 = &fYec49_tmp[4];
		float fSlow76 = (1.0f / fSlow74);
		float fSlow77 = (1.0f - fSlow68);
		float fSlow78 = (1.0f / (fSlow64 * fSlow67));
		float fRec89_tmp[12];
		float* fRec89 = &fRec89_tmp[4];
		float fSlow79 = (1.0f / (((fSlow68 + 1.61803401f) / fSlow67) + 1.0f));
		float fSlow80 = (((fSlow68 + -1.61803401f) / fSlow67) + 1.0f);
		float fRec88_tmp[12];
		float* fRec88 = &fRec88_tmp[4];
		float fSlow81 = (0.0f - (2.0f / fSlow71));
		float fSlow82 = (1.0f / (((fSlow68 + 0.618034005f) / fSlow67) + 1.0f));
		float fSlow83 = (((fSlow68 + -0.618034005f) / fSlow67) + 1.0f);
		float fRec87_tmp[12];
		float* fRec87 = &fRec87_tmp[4];
		float fRec95_tmp[12];
		float* fRec95 = &fRec95_tmp[4];
		float fRec94_tmp[12];
		float* fRec94 = &fRec94_tmp[4];
		float fRec93_tmp[12];
		float* fRec93 = &fRec93_tmp[4];
		float fSlow84 = float(fHslider5);
		float fSlow85 = (1.0f - fSlow84);
		float fSlow86 = std::pow(10.0f, (0.0f - (0.50999999f * (((1.25f * fSlow3) + -0.25f) / float(fHslider6)))));
		float fSlow87 = float(fHslider7);
		float fSlow88 = float(fHslider8);
		float fSlow89 = float(fHslider9);
		float fRec14_tmp[12];
		float* fRec14 = &fRec14_tmp[4];
		int iSlow90 = int(primes(int((34.0f * fSlow3))));
		float fSlow91 = (0.00499999989f * float(iSlow90));
		float fRec102_tmp[12];
		float* fRec102 = &fRec102_tmp[4];
		float fZec161[8];
		float fZec162[8];
		int iZec163[8];
		float fZec164[8];
		float fZec165[8];
		float fZec166[8];
		float fZec167[8];
		float fZec168[8];
		float fZec169[8];
		float fZec170[8];
		float fYec53_tmp[12];
		float* fYec53 = &fYec53_tmp[4];
		float fZec171[8];
		float fRec101_tmp[12];
		float* fRec101 = &fRec101_tmp[4];
		float fRec100_tmp[12];
		float* fRec100 = &fRec100_tmp[4];
		float fRec99_tmp[12];
		float* fRec99 = &fRec99_tmp[4];
		float fRec98_tmp[12];
		float* fRec98 = &fRec98_tmp[4];
		float fZec172[8];
		float fRec97_tmp[12];
		float* fRec97 = &fRec97_tmp[4];
		float fRec108_tmp[12];
		float* fRec108 = &fRec108_tmp[4];
		float fRec107_tmp[12];
		float* fRec107 = &fRec107_tmp[4];
		float fRec106_tmp[12];
		float* fRec106 = &fRec106_tmp[4];
		float fZec173[8];
		float fYec54_tmp[12];
		float* fYec54 = &fYec54_tmp[4];
		float fRec105_tmp[12];
		float* fRec105 = &fRec105_tmp[4];
		float fRec104_tmp[12];
		float* fRec104 = &fRec104_tmp[4];
		float fRec103_tmp[12];
		float* fRec103 = &fRec103_tmp[4];
		float fRec111_tmp[12];
		float* fRec111 = &fRec111_tmp[4];
		float fRec110_tmp[12];
		float* fRec110 = &fRec110_tmp[4];
		float fRec109_tmp[12];
		float* fRec109 = &fRec109_tmp[4];
		float fRec96_tmp[12];
		float* fRec96 = &fRec96_tmp[4];
		float fSlow92 = float(fHslider10);
		float fSlow93 = std::cos(fSlow92);
		float fSlow94 = std::sin(fSlow92);
		float fZec174[8];
		float fZec175[8];
		float fYec57_tmp[12];
		float* fYec57 = &fYec57_tmp[4];
		float fRec13_tmp[12];
		float* fRec13 = &fRec13_tmp[4];
		float fRec11_tmp[12];
		float* fRec11 = &fRec11_tmp[4];
		int iSlow95 = int(primes(int((240.0f * fSlow3))));
		float fSlow96 = (9.99999975e-05f * float(iSlow95));
		float fRec113_tmp[12];
		float* fRec113 = &fRec113_tmp[4];
		float fZec176[8];
		float fYec59_tmp[12];
		float* fYec59 = &fYec59_tmp[4];
		float fZec177[8];
		float fRec112_tmp[12];
		float* fRec112 = &fRec112_tmp[4];
		float fRec12_tmp[12];
		float* fRec12 = &fRec12_tmp[4];
		float fZec178[8];
		float fZec179[8];
		float fZec180[8];
		float fZec181[8];
		float fYec61_tmp[12];
		float* fYec61 = &fYec61_tmp[4];
		float fRec10_tmp[12];
		float* fRec10 = &fRec10_tmp[4];
		float fRec8_tmp[12];
		float* fRec8 = &fRec8_tmp[4];
		float fYec63_tmp[12];
		float* fYec63 = &fYec63_tmp[4];
		float fRec114_tmp[12];
		float* fRec114 = &fRec114_tmp[4];
		float fRec9_tmp[12];
		float* fRec9 = &fRec9_tmp[4];
		float fZec182[8];
		float fZec183[8];
		float fZec184[8];
		float fZec185[8];
		float fYec65_tmp[12];
		float* fYec65 = &fYec65_tmp[4];
		float fRec7_tmp[12];
		float* fRec7 = &fRec7_tmp[4];
		float fRec5_tmp[12];
		float* fRec5 = &fRec5_tmp[4];
		int iSlow97 = int(primes(int((190.0f * fSlow3))));
		float fSlow98 = (9.99999975e-05f * float(iSlow97));
		float fRec116_tmp[12];
		float* fRec116 = &fRec116_tmp[4];
		float fZec186[8];
		float fYec67_tmp[12];
		float* fYec67 = &fYec67_tmp[4];
		float fZec187[8];
		float fRec115_tmp[12];
		float* fRec115 = &fRec115_tmp[4];
		float fRec6_tmp[12];
		float* fRec6 = &fRec6_tmp[4];
		int iSlow99 = int(primes(int((175.0f * fSlow3))));
		float fSlow100 = (9.99999975e-05f * float(iSlow99));
		float fRec117_tmp[12];
		float* fRec117 = &fRec117_tmp[4];
		float fZec188[8];
		float fZec189[8];
		float fZec190[8];
		float fZec191[8];
		float fZec192[8];
		float fYec69_tmp[12];
		float* fYec69 = &fYec69_tmp[4];
		float fZec193[8];
		float fRec4_tmp[12];
		float* fRec4 = &fRec4_tmp[4];
		float fRec2_tmp[12];
		float* fRec2 = &fRec2_tmp[4];
		float fYec71_tmp[12];
		float* fYec71 = &fYec71_tmp[4];
		float fRec118_tmp[12];
		float* fRec118 = &fRec118_tmp[4];
		float fRec3_tmp[12];
		float* fRec3 = &fRec3_tmp[4];
		float fRec0_tmp[12];
		float* fRec0 = &fRec0_tmp[4];
		float fRec1_tmp[12];
		float* fRec1 = &fRec1_tmp[4];
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
			for (int j2 = 0; (j2 < 4); j2 = (j2 + 1)) {
				iVec0_tmp[j2] = iVec0_perm[j2];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iVec0[i] = 1;
				
			}
			/* Post code */
			for (int j3 = 0; (j3 < 4); j3 = (j3 + 1)) {
				iVec0_perm[j3] = iVec0_tmp[(vsize + j3)];
				
			}
			/* Vectorizable loop 1 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec0[i] = (1 - iVec0[(i - 1)]);
				
			}
			/* Recursive loop 2 */
			/* Pre code */
			for (int j0 = 0; (j0 < 4); j0 = (j0 + 1)) {
				fRec15_tmp[j0] = fRec15_perm[j0];
				
			}
			for (int j4 = 0; (j4 < 4); j4 = (j4 + 1)) {
				fRec16_tmp[j4] = fRec16_perm[j4];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec15[i] = ((fSlow1 * fRec16[(i - 1)]) + (fSlow2 * fRec15[(i - 1)]));
				fRec16[i] = ((float(iZec0[i]) + (fSlow2 * fRec16[(i - 1)])) - (fSlow1 * fRec15[(i - 1)]));
				
			}
			/* Post code */
			for (int j1 = 0; (j1 < 4); j1 = (j1 + 1)) {
				fRec15_perm[j1] = fRec15_tmp[(vsize + j1)];
				
			}
			for (int j5 = 0; (j5 < 4); j5 = (j5 + 1)) {
				fRec16_perm[j5] = fRec16_tmp[(vsize + j5)];
				
			}
			/* Vectorizable loop 3 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec39[i] = (fSlow26 * (fRec16[i] + 1.0f));
				
			}
			/* Vectorizable loop 4 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec131[i] = (fSlow26 * (fRec15[i] + 1.0f));
				
			}
			/* Vectorizable loop 5 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec40[i] = (fZec39[i] + 3.50000501f);
				
			}
			/* Vectorizable loop 6 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec67[i] = (fSlow29 * fRec16[i]);
				
			}
			/* Vectorizable loop 7 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec132[i] = (fZec131[i] + 3.50000501f);
				
			}
			/* Vectorizable loop 8 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec161[i] = (fSlow29 * fRec15[i]);
				
			}
			/* Vectorizable loop 9 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec43[i] = std::floor(fZec40[i]);
				
			}
			/* Vectorizable loop 10 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec68[i] = (fSlow26 + (fZec67[i] + 3.50000501f));
				
			}
			/* Recursive loop 11 */
			/* Pre code */
			for (int j122 = 0; (j122 < 4); j122 = (j122 + 1)) {
				fRec76_tmp[j122] = fRec76_perm[j122];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec76[i] = (fSlow37 + (0.999899983f * (fRec76[(i - 1)] + float((iSlow36 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j123 = 0; (j123 < 4); j123 = (j123 + 1)) {
				fRec76_perm[j123] = fRec76_tmp[(vsize + j123)];
				
			}
			/* Recursive loop 12 */
			/* Pre code */
			for (int j138 = 0; (j138 < 4); j138 = (j138 + 1)) {
				fRec79_tmp[j138] = fRec79_perm[j138];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec79[i] = (fSlow41 + (0.999899983f * (fRec79[(i - 1)] + float((iSlow40 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j139 = 0; (j139 < 4); j139 = (j139 + 1)) {
				fRec79_perm[j139] = fRec79_tmp[(vsize + j139)];
				
			}
			/* Recursive loop 13 */
			/* Pre code */
			for (int j146 = 0; (j146 < 4); j146 = (j146 + 1)) {
				fRec80_tmp[j146] = fRec80_perm[j146];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec80[i] = (fSlow43 + (0.999899983f * (fRec80[(i - 1)] + float((iSlow42 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j147 = 0; (j147 < 4); j147 = (j147 + 1)) {
				fRec80_perm[j147] = fRec80_tmp[(vsize + j147)];
				
			}
			/* Recursive loop 14 */
			/* Pre code */
			for (int j154 = 0; (j154 < 4); j154 = (j154 + 1)) {
				fRec82_tmp[j154] = fRec82_perm[j154];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec82[i] = (fSlow45 + (0.999899983f * (fRec82[(i - 1)] + float((iSlow44 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j155 = 0; (j155 < 4); j155 = (j155 + 1)) {
				fRec82_perm[j155] = fRec82_tmp[(vsize + j155)];
				
			}
			/* Recursive loop 15 */
			/* Pre code */
			for (int j170 = 0; (j170 < 4); j170 = (j170 + 1)) {
				fRec85_tmp[j170] = fRec85_perm[j170];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec85[i] = (fSlow49 + (0.999899983f * (fRec85[(i - 1)] + float((iSlow48 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j171 = 0; (j171 < 4); j171 = (j171 + 1)) {
				fRec85_perm[j171] = fRec85_tmp[(vsize + j171)];
				
			}
			/* Vectorizable loop 16 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec135[i] = std::floor(fZec132[i]);
				
			}
			/* Vectorizable loop 17 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec162[i] = (fSlow26 + (fZec161[i] + 3.50000501f));
				
			}
			/* Recursive loop 18 */
			/* Pre code */
			for (int j6 = 0; (j6 < 4); j6 = (j6 + 1)) {
				fRec53_tmp[j6] = fRec53_perm[j6];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec53[i] = (fSlow5 + (0.999899983f * (fRec53[(i - 1)] + float((iSlow4 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j7 = 0; (j7 < 4); j7 = (j7 + 1)) {
				fRec53_perm[j7] = fRec53_tmp[(vsize + j7)];
				
			}
			/* Recursive loop 19 */
			/* Pre code */
			for (int j22 = 0; (j22 < 4); j22 = (j22 + 1)) {
				fRec56_tmp[j22] = fRec56_perm[j22];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec56[i] = (fSlow9 + (0.999899983f * (fRec56[(i - 1)] + float((iSlow8 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j23 = 0; (j23 < 4); j23 = (j23 + 1)) {
				fRec56_perm[j23] = fRec56_tmp[(vsize + j23)];
				
			}
			/* Recursive loop 20 */
			/* Pre code */
			for (int j14 = 0; (j14 < 4); j14 = (j14 + 1)) {
				fRec55_tmp[j14] = fRec55_perm[j14];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec55[i] = (fSlow7 + (0.999899983f * (fRec55[(i - 1)] + float((iSlow6 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j15 = 0; (j15 < 4); j15 = (j15 + 1)) {
				fRec55_perm[j15] = fRec55_tmp[(vsize + j15)];
				
			}
			/* Recursive loop 21 */
			/* Pre code */
			for (int j38 = 0; (j38 < 4); j38 = (j38 + 1)) {
				fRec59_tmp[j38] = fRec59_perm[j38];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec59[i] = (fSlow13 + (0.999899983f * (fRec59[(i - 1)] + float((iSlow12 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j39 = 0; (j39 < 4); j39 = (j39 + 1)) {
				fRec59_perm[j39] = fRec59_tmp[(vsize + j39)];
				
			}
			/* Recursive loop 22 */
			/* Pre code */
			for (int j30 = 0; (j30 < 4); j30 = (j30 + 1)) {
				fRec58_tmp[j30] = fRec58_perm[j30];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec58[i] = (fSlow11 + (0.999899983f * (fRec58[(i - 1)] + float((iSlow10 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j31 = 0; (j31 < 4); j31 = (j31 + 1)) {
				fRec58_perm[j31] = fRec58_tmp[(vsize + j31)];
				
			}
			/* Recursive loop 23 */
			/* Pre code */
			for (int j46 = 0; (j46 < 4); j46 = (j46 + 1)) {
				fRec61_tmp[j46] = fRec61_perm[j46];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec61[i] = (fSlow15 + (0.999899983f * (fRec61[(i - 1)] + float((iSlow14 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j47 = 0; (j47 < 4); j47 = (j47 + 1)) {
				fRec61_perm[j47] = fRec61_tmp[(vsize + j47)];
				
			}
			/* Recursive loop 24 */
			/* Pre code */
			for (int j54 = 0; (j54 < 4); j54 = (j54 + 1)) {
				fRec62_tmp[j54] = fRec62_perm[j54];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec62[i] = (fSlow17 + (0.999899983f * (fRec62[(i - 1)] + float((iSlow16 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j55 = 0; (j55 < 4); j55 = (j55 + 1)) {
				fRec62_perm[j55] = fRec62_tmp[(vsize + j55)];
				
			}
			/* Recursive loop 25 */
			/* Pre code */
			for (int j62 = 0; (j62 < 4); j62 = (j62 + 1)) {
				fRec64_tmp[j62] = fRec64_perm[j62];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec64[i] = (fSlow19 + (0.999899983f * (fRec64[(i - 1)] + float((iSlow18 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j63 = 0; (j63 < 4); j63 = (j63 + 1)) {
				fRec64_perm[j63] = fRec64_tmp[(vsize + j63)];
				
			}
			/* Recursive loop 26 */
			/* Pre code */
			for (int j70 = 0; (j70 < 4); j70 = (j70 + 1)) {
				fRec65_tmp[j70] = fRec65_perm[j70];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec65[i] = (fSlow21 + (0.999899983f * (fRec65[(i - 1)] + float((iSlow20 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j71 = 0; (j71 < 4); j71 = (j71 + 1)) {
				fRec65_perm[j71] = fRec65_tmp[(vsize + j71)];
				
			}
			/* Recursive loop 27 */
			/* Pre code */
			for (int j78 = 0; (j78 < 4); j78 = (j78 + 1)) {
				fRec67_tmp[j78] = fRec67_perm[j78];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec67[i] = (fSlow23 + (0.999899983f * (fRec67[(i - 1)] + float((iSlow22 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j79 = 0; (j79 < 4); j79 = (j79 + 1)) {
				fRec67_perm[j79] = fRec67_tmp[(vsize + j79)];
				
			}
			/* Recursive loop 28 */
			/* Pre code */
			for (int j86 = 0; (j86 < 4); j86 = (j86 + 1)) {
				fRec68_tmp[j86] = fRec68_perm[j86];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec68[i] = (fSlow25 + (0.995000005f * (fRec68[(i - 1)] + float((iSlow24 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j87 = 0; (j87 < 4); j87 = (j87 + 1)) {
				fRec68_perm[j87] = fRec68_tmp[(vsize + j87)];
				
			}
			/* Vectorizable loop 29 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec44[i] = (fZec39[i] + (4.0f - fZec43[i]));
				
			}
			/* Vectorizable loop 30 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec46[i] = (fZec39[i] + (3.0f - fZec43[i]));
				
			}
			/* Recursive loop 31 */
			/* Pre code */
			for (int j92 = 0; (j92 < 4); j92 = (j92 + 1)) {
				fRec70_tmp[j92] = fRec70_perm[j92];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec70[i] = (fSlow28 + (0.995000005f * (fRec70[(i - 1)] + float((iSlow27 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j93 = 0; (j93 < 4); j93 = (j93 + 1)) {
				fRec70_perm[j93] = fRec70_tmp[(vsize + j93)];
				
			}
			/* Vectorizable loop 32 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec70[i] = std::floor(fZec68[i]);
				
			}
			/* Recursive loop 33 */
			/* Pre code */
			for (int j98 = 0; (j98 < 4); j98 = (j98 + 1)) {
				fRec71_tmp[j98] = fRec71_perm[j98];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec71[i] = (fSlow31 + (0.999899983f * (fRec71[(i - 1)] + float((iSlow30 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j99 = 0; (j99 < 4); j99 = (j99 + 1)) {
				fRec71_perm[j99] = fRec71_tmp[(vsize + j99)];
				
			}
			/* Recursive loop 34 */
			/* Pre code */
			for (int j106 = 0; (j106 < 4); j106 = (j106 + 1)) {
				fRec73_tmp[j106] = fRec73_perm[j106];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec73[i] = (fSlow33 + (0.999899983f * (fRec73[(i - 1)] + float((iSlow32 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j107 = 0; (j107 < 4); j107 = (j107 + 1)) {
				fRec73_perm[j107] = fRec73_tmp[(vsize + j107)];
				
			}
			/* Recursive loop 35 */
			/* Pre code */
			for (int j114 = 0; (j114 < 4); j114 = (j114 + 1)) {
				fRec74_tmp[j114] = fRec74_perm[j114];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec74[i] = (fSlow35 + (0.999899983f * (fRec74[(i - 1)] + float((iSlow34 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j115 = 0; (j115 < 4); j115 = (j115 + 1)) {
				fRec74_perm[j115] = fRec74_tmp[(vsize + j115)];
				
			}
			/* Vectorizable loop 36 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec90[i] = (fRec76[i] + -1.49998999f);
				
			}
			/* Recursive loop 37 */
			/* Pre code */
			for (int j130 = 0; (j130 < 4); j130 = (j130 + 1)) {
				fRec77_tmp[j130] = fRec77_perm[j130];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec77[i] = (fSlow39 + (0.999899983f * (fRec77[(i - 1)] + float((iSlow38 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j131 = 0; (j131 < 4); j131 = (j131 + 1)) {
				fRec77_perm[j131] = fRec77_tmp[(vsize + j131)];
				
			}
			/* Vectorizable loop 38 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec101[i] = (fRec79[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 39 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec110[i] = (fRec80[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 40 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec115[i] = (fRec82[i] + -1.49998999f);
				
			}
			/* Recursive loop 41 */
			/* Pre code */
			for (int j162 = 0; (j162 < 4); j162 = (j162 + 1)) {
				fRec83_tmp[j162] = fRec83_perm[j162];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec83[i] = (fSlow47 + (0.999899983f * (fRec83[(i - 1)] + float((iSlow46 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j163 = 0; (j163 < 4); j163 = (j163 + 1)) {
				fRec83_perm[j163] = fRec83_tmp[(vsize + j163)];
				
			}
			/* Vectorizable loop 42 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec126[i] = (fRec85[i] + -1.49998999f);
				
			}
			/* Recursive loop 43 */
			/* Pre code */
			for (int j178 = 0; (j178 < 4); j178 = (j178 + 1)) {
				fRec86_tmp[j178] = fRec86_perm[j178];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec86[i] = (fSlow51 + (0.995000005f * (fRec86[(i - 1)] + float((iSlow50 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j179 = 0; (j179 < 4); j179 = (j179 + 1)) {
				fRec86_perm[j179] = fRec86_tmp[(vsize + j179)];
				
			}
			/* Vectorizable loop 44 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec136[i] = (fZec131[i] + (4.0f - fZec135[i]));
				
			}
			/* Vectorizable loop 45 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec138[i] = (fZec131[i] + (3.0f - fZec135[i]));
				
			}
			/* Recursive loop 46 */
			/* Pre code */
			for (int j214 = 0; (j214 < 4); j214 = (j214 + 1)) {
				fRec102_tmp[j214] = fRec102_perm[j214];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec102[i] = (fSlow91 + (0.995000005f * (fRec102[(i - 1)] + float((iSlow90 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j215 = 0; (j215 < 4); j215 = (j215 + 1)) {
				fRec102_perm[j215] = fRec102_tmp[(vsize + j215)];
				
			}
			/* Vectorizable loop 47 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec164[i] = std::floor(fZec162[i]);
				
			}
			/* Recursive loop 48 */
			/* Pre code */
			for (int j256 = 0; (j256 < 4); j256 = (j256 + 1)) {
				fRec113_tmp[j256] = fRec113_perm[j256];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec113[i] = (fSlow96 + (0.999899983f * (fRec113[(i - 1)] + float((iSlow95 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j257 = 0; (j257 < 4); j257 = (j257 + 1)) {
				fRec113_perm[j257] = fRec113_tmp[(vsize + j257)];
				
			}
			/* Recursive loop 49 */
			/* Pre code */
			for (int j282 = 0; (j282 < 4); j282 = (j282 + 1)) {
				fRec116_tmp[j282] = fRec116_perm[j282];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec116[i] = (fSlow98 + (0.999899983f * (fRec116[(i - 1)] + float((iSlow97 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j283 = 0; (j283 < 4); j283 = (j283 + 1)) {
				fRec116_perm[j283] = fRec116_tmp[(vsize + j283)];
				
			}
			/* Recursive loop 50 */
			/* Pre code */
			for (int j290 = 0; (j290 < 4); j290 = (j290 + 1)) {
				fRec117_tmp[j290] = fRec117_perm[j290];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec117[i] = (fSlow100 + (0.999899983f * (fRec117[(i - 1)] + float((iSlow99 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j291 = 0; (j291 < 4); j291 = (j291 + 1)) {
				fRec117_perm[j291] = fRec117_tmp[(vsize + j291)];
				
			}
			/* Vectorizable loop 51 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec3[i] = (fRec53[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 52 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec5[i] = (fRec55[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 53 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec13[i] = (fRec58[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 54 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec11[i] = (fRec56[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 55 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec19[i] = (fRec59[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 56 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec21[i] = (fRec61[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 57 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec27[i] = (fRec62[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 58 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec29[i] = (fRec64[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 59 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec35[i] = (fRec65[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 60 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec37[i] = (fRec67[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 61 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec41[i] = int(fZec40[i]);
				
			}
			/* Vectorizable loop 62 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec48[i] = (fZec39[i] + (2.0f - fZec43[i]));
				
			}
			/* Vectorizable loop 63 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec50[i] = (fZec39[i] + (1.0f - fZec43[i]));
				
			}
			/* Vectorizable loop 64 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec60[i] = (fZec44[i] * fZec46[i]);
				
			}
			/* Vectorizable loop 65 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec65[i] = (fRec68[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 66 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec71[i] = (fSlow26 + (fZec67[i] + (4.0f - fZec70[i])));
				
			}
			/* Vectorizable loop 67 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec72[i] = (fSlow26 + (fZec67[i] + (3.0f - fZec70[i])));
				
			}
			/* Vectorizable loop 68 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec76[i] = (fRec70[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 69 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec80[i] = (fRec71[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 70 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec82[i] = (fRec73[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 71 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec88[i] = (fRec74[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 72 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec92[i] = std::floor(fZec90[i]);
				
			}
			/* Vectorizable loop 73 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec99[i] = (fRec77[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 74 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec103[i] = std::floor(fZec101[i]);
				
			}
			/* Vectorizable loop 75 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec112[i] = std::floor(fZec110[i]);
				
			}
			/* Vectorizable loop 76 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec117[i] = std::floor(fZec115[i]);
				
			}
			/* Vectorizable loop 77 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec124[i] = (fRec83[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 78 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec128[i] = std::floor(fZec126[i]);
				
			}
			/* Vectorizable loop 79 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec133[i] = int(fZec132[i]);
				
			}
			/* Vectorizable loop 80 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec140[i] = (fZec131[i] + (2.0f - fZec135[i]));
				
			}
			/* Vectorizable loop 81 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec142[i] = (fZec131[i] + (1.0f - fZec135[i]));
				
			}
			/* Vectorizable loop 82 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec152[i] = (fZec136[i] * fZec138[i]);
				
			}
			/* Vectorizable loop 83 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec157[i] = (fRec86[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 84 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec165[i] = (fSlow26 + (fZec161[i] + (4.0f - fZec164[i])));
				
			}
			/* Vectorizable loop 85 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec166[i] = (fSlow26 + (fZec161[i] + (3.0f - fZec164[i])));
				
			}
			/* Vectorizable loop 86 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec170[i] = (fRec102[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 87 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec176[i] = (fRec113[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 88 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec186[i] = (fRec116[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 89 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec192[i] = (fRec117[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 90 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec4[i] = std::floor(fZec3[i]);
				
			}
			/* Vectorizable loop 91 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec6[i] = std::floor(fZec5[i]);
				
			}
			/* Vectorizable loop 92 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec14[i] = std::floor(fZec13[i]);
				
			}
			/* Vectorizable loop 93 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec12[i] = std::floor(fZec11[i]);
				
			}
			/* Vectorizable loop 94 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec20[i] = std::floor(fZec19[i]);
				
			}
			/* Vectorizable loop 95 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec22[i] = std::floor(fZec21[i]);
				
			}
			/* Vectorizable loop 96 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec28[i] = std::floor(fZec27[i]);
				
			}
			/* Vectorizable loop 97 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec30[i] = std::floor(fZec29[i]);
				
			}
			/* Vectorizable loop 98 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec36[i] = std::floor(fZec35[i]);
				
			}
			/* Vectorizable loop 99 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec38[i] = std::floor(fZec37[i]);
				
			}
			/* Vectorizable loop 100 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec42[i] = std::min<int>(512, std::max<int>(0, iZec41[i]));
				
			}
			/* Vectorizable loop 101 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec45[i] = (0.0f - fZec44[i]);
				
			}
			/* Vectorizable loop 102 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec47[i] = (0.0f - (0.5f * fZec46[i]));
				
			}
			/* Vectorizable loop 103 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec49[i] = (0.0f - (0.333333343f * fZec48[i]));
				
			}
			/* Vectorizable loop 104 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec51[i] = (0.0f - (0.25f * fZec50[i]));
				
			}
			/* Vectorizable loop 105 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec52[i] = (fZec39[i] + (5.0f - fZec43[i]));
				
			}
			/* Vectorizable loop 106 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec53[i] = std::min<int>(512, std::max<int>(0, (iZec41[i] + 1)));
				
			}
			/* Vectorizable loop 107 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec54[i] = (0.0f - fZec46[i]);
				
			}
			/* Vectorizable loop 108 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec55[i] = (0.0f - (0.5f * fZec48[i]));
				
			}
			/* Vectorizable loop 109 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec56[i] = (0.0f - (0.333333343f * fZec50[i]));
				
			}
			/* Vectorizable loop 110 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec57[i] = std::min<int>(512, std::max<int>(0, (iZec41[i] + 2)));
				
			}
			/* Vectorizable loop 111 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec58[i] = (0.0f - fZec48[i]);
				
			}
			/* Vectorizable loop 112 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec59[i] = (0.0f - (0.5f * fZec50[i]));
				
			}
			/* Vectorizable loop 113 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec61[i] = std::min<int>(512, std::max<int>(0, (iZec41[i] + 3)));
				
			}
			/* Vectorizable loop 114 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec62[i] = (0.0f - fZec50[i]);
				
			}
			/* Vectorizable loop 115 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec63[i] = (fZec60[i] * fZec48[i]);
				
			}
			/* Vectorizable loop 116 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec64[i] = std::min<int>(512, std::max<int>(0, (iZec41[i] + 4)));
				
			}
			/* Vectorizable loop 117 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec66[i] = std::floor(fZec65[i]);
				
			}
			/* Vectorizable loop 118 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec69[i] = int(fZec68[i]);
				
			}
			/* Vectorizable loop 119 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec73[i] = (fSlow26 + (fZec67[i] + (2.0f - fZec70[i])));
				
			}
			/* Vectorizable loop 120 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec74[i] = (fSlow26 + (fZec67[i] + (1.0f - fZec70[i])));
				
			}
			/* Vectorizable loop 121 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec75[i] = (fZec71[i] * fZec72[i]);
				
			}
			/* Vectorizable loop 122 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec77[i] = std::floor(fZec76[i]);
				
			}
			/* Vectorizable loop 123 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec81[i] = std::floor(fZec80[i]);
				
			}
			/* Vectorizable loop 124 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec83[i] = std::floor(fZec82[i]);
				
			}
			/* Vectorizable loop 125 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec89[i] = std::floor(fZec88[i]);
				
			}
			/* Vectorizable loop 126 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec91[i] = std::min<int>(8192, std::max<int>(0, int(fZec90[i])));
				
			}
			/* Vectorizable loop 127 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec93[i] = (fZec92[i] + (2.0f - fRec76[i]));
				
			}
			/* Vectorizable loop 128 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec94[i] = (fRec76[i] - fZec92[i]);
				
			}
			/* Vectorizable loop 129 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec100[i] = std::floor(fZec99[i]);
				
			}
			/* Vectorizable loop 130 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec102[i] = std::min<int>(8192, std::max<int>(0, int(fZec101[i])));
				
			}
			/* Vectorizable loop 131 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec104[i] = (fZec103[i] + (2.0f - fRec79[i]));
				
			}
			/* Vectorizable loop 132 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec105[i] = (fRec79[i] - fZec103[i]);
				
			}
			/* Vectorizable loop 133 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec111[i] = std::min<int>(8192, std::max<int>(0, int(fZec110[i])));
				
			}
			/* Vectorizable loop 134 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec113[i] = (fZec112[i] + (2.0f - fRec80[i]));
				
			}
			/* Vectorizable loop 135 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec114[i] = (fRec80[i] - fZec112[i]);
				
			}
			/* Vectorizable loop 136 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec116[i] = std::min<int>(8192, std::max<int>(0, int(fZec115[i])));
				
			}
			/* Vectorizable loop 137 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec118[i] = (fZec117[i] + (2.0f - fRec82[i]));
				
			}
			/* Vectorizable loop 138 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec119[i] = (fRec82[i] - fZec117[i]);
				
			}
			/* Vectorizable loop 139 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec125[i] = std::floor(fZec124[i]);
				
			}
			/* Vectorizable loop 140 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec127[i] = std::min<int>(8192, std::max<int>(0, int(fZec126[i])));
				
			}
			/* Vectorizable loop 141 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec129[i] = (fZec128[i] + (2.0f - fRec85[i]));
				
			}
			/* Vectorizable loop 142 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec130[i] = (fRec85[i] - fZec128[i]);
				
			}
			/* Vectorizable loop 143 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec134[i] = std::max<int>(0, iZec133[i]);
				
			}
			/* Vectorizable loop 144 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec137[i] = (0.0f - fZec136[i]);
				
			}
			/* Vectorizable loop 145 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec139[i] = (0.0f - (0.5f * fZec138[i]));
				
			}
			/* Vectorizable loop 146 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec141[i] = (0.0f - (0.333333343f * fZec140[i]));
				
			}
			/* Vectorizable loop 147 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec143[i] = (0.0f - (0.25f * fZec142[i]));
				
			}
			/* Vectorizable loop 148 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec144[i] = (fZec131[i] + (5.0f - fZec135[i]));
				
			}
			/* Vectorizable loop 149 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec145[i] = std::max<int>(0, (iZec133[i] + 1));
				
			}
			/* Vectorizable loop 150 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec146[i] = (0.0f - fZec138[i]);
				
			}
			/* Vectorizable loop 151 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec147[i] = (0.0f - (0.5f * fZec140[i]));
				
			}
			/* Vectorizable loop 152 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec148[i] = (0.0f - (0.333333343f * fZec142[i]));
				
			}
			/* Vectorizable loop 153 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec149[i] = std::max<int>(0, (iZec133[i] + 2));
				
			}
			/* Vectorizable loop 154 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec150[i] = (0.0f - fZec140[i]);
				
			}
			/* Vectorizable loop 155 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec151[i] = (0.0f - (0.5f * fZec142[i]));
				
			}
			/* Vectorizable loop 156 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec153[i] = std::max<int>(0, (iZec133[i] + 3));
				
			}
			/* Vectorizable loop 157 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec154[i] = (0.0f - fZec142[i]);
				
			}
			/* Vectorizable loop 158 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec155[i] = (fZec152[i] * fZec140[i]);
				
			}
			/* Vectorizable loop 159 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec156[i] = std::max<int>(0, (iZec133[i] + 4));
				
			}
			/* Vectorizable loop 160 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec158[i] = std::floor(fZec157[i]);
				
			}
			/* Vectorizable loop 161 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec163[i] = int(fZec162[i]);
				
			}
			/* Vectorizable loop 162 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec167[i] = (fSlow26 + (fZec161[i] + (2.0f - fZec164[i])));
				
			}
			/* Vectorizable loop 163 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec168[i] = (fSlow26 + (fZec161[i] + (1.0f - fZec164[i])));
				
			}
			/* Vectorizable loop 164 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec169[i] = (fZec165[i] * fZec166[i]);
				
			}
			/* Vectorizable loop 165 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec171[i] = std::floor(fZec170[i]);
				
			}
			/* Vectorizable loop 166 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec177[i] = std::floor(fZec176[i]);
				
			}
			/* Vectorizable loop 167 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec187[i] = std::floor(fZec186[i]);
				
			}
			/* Vectorizable loop 168 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec193[i] = std::floor(fZec192[i]);
				
			}
			/* Recursive loop 169 */
			/* Pre code */
			fYec0_idx = ((fYec0_idx + fYec0_idx_save) & 16383);
			for (int j8 = 0; (j8 < 4); j8 = (j8 + 1)) {
				fYec1_tmp[j8] = fYec1_perm[j8];
				
			}
			for (int j10 = 0; (j10 < 4); j10 = (j10 + 1)) {
				fRec52_tmp[j10] = fRec52_perm[j10];
				
			}
			for (int j12 = 0; (j12 < 4); j12 = (j12 + 1)) {
				fRec50_tmp[j12] = fRec50_perm[j12];
				
			}
			fYec2_idx = ((fYec2_idx + fYec2_idx_save) & 16383);
			for (int j16 = 0; (j16 < 4); j16 = (j16 + 1)) {
				fYec3_tmp[j16] = fYec3_perm[j16];
				
			}
			for (int j18 = 0; (j18 < 4); j18 = (j18 + 1)) {
				fRec54_tmp[j18] = fRec54_perm[j18];
				
			}
			for (int j20 = 0; (j20 < 4); j20 = (j20 + 1)) {
				fRec51_tmp[j20] = fRec51_perm[j20];
				
			}
			fYec4_idx = ((fYec4_idx + fYec4_idx_save) & 16383);
			for (int j24 = 0; (j24 < 4); j24 = (j24 + 1)) {
				fYec5_tmp[j24] = fYec5_perm[j24];
				
			}
			for (int j26 = 0; (j26 < 4); j26 = (j26 + 1)) {
				fRec49_tmp[j26] = fRec49_perm[j26];
				
			}
			for (int j28 = 0; (j28 < 4); j28 = (j28 + 1)) {
				fRec47_tmp[j28] = fRec47_perm[j28];
				
			}
			fYec6_idx = ((fYec6_idx + fYec6_idx_save) & 16383);
			for (int j32 = 0; (j32 < 4); j32 = (j32 + 1)) {
				fYec7_tmp[j32] = fYec7_perm[j32];
				
			}
			for (int j34 = 0; (j34 < 4); j34 = (j34 + 1)) {
				fRec57_tmp[j34] = fRec57_perm[j34];
				
			}
			for (int j36 = 0; (j36 < 4); j36 = (j36 + 1)) {
				fRec48_tmp[j36] = fRec48_perm[j36];
				
			}
			fYec8_idx = ((fYec8_idx + fYec8_idx_save) & 16383);
			for (int j40 = 0; (j40 < 4); j40 = (j40 + 1)) {
				fYec9_tmp[j40] = fYec9_perm[j40];
				
			}
			for (int j42 = 0; (j42 < 4); j42 = (j42 + 1)) {
				fRec46_tmp[j42] = fRec46_perm[j42];
				
			}
			for (int j44 = 0; (j44 < 4); j44 = (j44 + 1)) {
				fRec44_tmp[j44] = fRec44_perm[j44];
				
			}
			fYec10_idx = ((fYec10_idx + fYec10_idx_save) & 16383);
			for (int j48 = 0; (j48 < 4); j48 = (j48 + 1)) {
				fYec11_tmp[j48] = fYec11_perm[j48];
				
			}
			for (int j50 = 0; (j50 < 4); j50 = (j50 + 1)) {
				fRec60_tmp[j50] = fRec60_perm[j50];
				
			}
			for (int j52 = 0; (j52 < 4); j52 = (j52 + 1)) {
				fRec45_tmp[j52] = fRec45_perm[j52];
				
			}
			fYec12_idx = ((fYec12_idx + fYec12_idx_save) & 16383);
			for (int j56 = 0; (j56 < 4); j56 = (j56 + 1)) {
				fYec13_tmp[j56] = fYec13_perm[j56];
				
			}
			for (int j58 = 0; (j58 < 4); j58 = (j58 + 1)) {
				fRec43_tmp[j58] = fRec43_perm[j58];
				
			}
			for (int j60 = 0; (j60 < 4); j60 = (j60 + 1)) {
				fRec41_tmp[j60] = fRec41_perm[j60];
				
			}
			fYec14_idx = ((fYec14_idx + fYec14_idx_save) & 16383);
			for (int j64 = 0; (j64 < 4); j64 = (j64 + 1)) {
				fYec15_tmp[j64] = fYec15_perm[j64];
				
			}
			for (int j66 = 0; (j66 < 4); j66 = (j66 + 1)) {
				fRec63_tmp[j66] = fRec63_perm[j66];
				
			}
			for (int j68 = 0; (j68 < 4); j68 = (j68 + 1)) {
				fRec42_tmp[j68] = fRec42_perm[j68];
				
			}
			fYec16_idx = ((fYec16_idx + fYec16_idx_save) & 16383);
			for (int j72 = 0; (j72 < 4); j72 = (j72 + 1)) {
				fYec17_tmp[j72] = fYec17_perm[j72];
				
			}
			for (int j74 = 0; (j74 < 4); j74 = (j74 + 1)) {
				fRec40_tmp[j74] = fRec40_perm[j74];
				
			}
			for (int j76 = 0; (j76 < 4); j76 = (j76 + 1)) {
				fRec38_tmp[j76] = fRec38_perm[j76];
				
			}
			fYec18_idx = ((fYec18_idx + fYec18_idx_save) & 16383);
			for (int j80 = 0; (j80 < 4); j80 = (j80 + 1)) {
				fYec19_tmp[j80] = fYec19_perm[j80];
				
			}
			for (int j82 = 0; (j82 < 4); j82 = (j82 + 1)) {
				fRec66_tmp[j82] = fRec66_perm[j82];
				
			}
			for (int j84 = 0; (j84 < 4); j84 = (j84 + 1)) {
				fRec39_tmp[j84] = fRec39_perm[j84];
				
			}
			fYec20_idx = ((fYec20_idx + fYec20_idx_save) & 1023);
			fYec21_idx = ((fYec21_idx + fYec21_idx_save) & 16383);
			for (int j88 = 0; (j88 < 4); j88 = (j88 + 1)) {
				fYec22_tmp[j88] = fYec22_perm[j88];
				
			}
			for (int j90 = 0; (j90 < 4); j90 = (j90 + 1)) {
				fRec37_tmp[j90] = fRec37_perm[j90];
				
			}
			fYec23_idx = ((fYec23_idx + fYec23_idx_save) & 1023);
			fYec24_idx = ((fYec24_idx + fYec24_idx_save) & 16383);
			for (int j94 = 0; (j94 < 4); j94 = (j94 + 1)) {
				fYec25_tmp[j94] = fYec25_perm[j94];
				
			}
			for (int j96 = 0; (j96 < 4); j96 = (j96 + 1)) {
				fRec69_tmp[j96] = fRec69_perm[j96];
				
			}
			fYec26_idx = ((fYec26_idx + fYec26_idx_save) & 16383);
			for (int j100 = 0; (j100 < 4); j100 = (j100 + 1)) {
				fYec27_tmp[j100] = fYec27_perm[j100];
				
			}
			for (int j102 = 0; (j102 < 4); j102 = (j102 + 1)) {
				fRec36_tmp[j102] = fRec36_perm[j102];
				
			}
			for (int j104 = 0; (j104 < 4); j104 = (j104 + 1)) {
				fRec34_tmp[j104] = fRec34_perm[j104];
				
			}
			fYec28_idx = ((fYec28_idx + fYec28_idx_save) & 16383);
			for (int j108 = 0; (j108 < 4); j108 = (j108 + 1)) {
				fYec29_tmp[j108] = fYec29_perm[j108];
				
			}
			for (int j110 = 0; (j110 < 4); j110 = (j110 + 1)) {
				fRec72_tmp[j110] = fRec72_perm[j110];
				
			}
			for (int j112 = 0; (j112 < 4); j112 = (j112 + 1)) {
				fRec35_tmp[j112] = fRec35_perm[j112];
				
			}
			fYec30_idx = ((fYec30_idx + fYec30_idx_save) & 16383);
			for (int j116 = 0; (j116 < 4); j116 = (j116 + 1)) {
				fYec31_tmp[j116] = fYec31_perm[j116];
				
			}
			for (int j118 = 0; (j118 < 4); j118 = (j118 + 1)) {
				fRec33_tmp[j118] = fRec33_perm[j118];
				
			}
			for (int j120 = 0; (j120 < 4); j120 = (j120 + 1)) {
				fRec31_tmp[j120] = fRec31_perm[j120];
				
			}
			fYec32_idx = ((fYec32_idx + fYec32_idx_save) & 16383);
			for (int j124 = 0; (j124 < 4); j124 = (j124 + 1)) {
				fYec33_tmp[j124] = fYec33_perm[j124];
				
			}
			for (int j126 = 0; (j126 < 4); j126 = (j126 + 1)) {
				fRec75_tmp[j126] = fRec75_perm[j126];
				
			}
			for (int j128 = 0; (j128 < 4); j128 = (j128 + 1)) {
				fRec32_tmp[j128] = fRec32_perm[j128];
				
			}
			fYec34_idx = ((fYec34_idx + fYec34_idx_save) & 16383);
			for (int j132 = 0; (j132 < 4); j132 = (j132 + 1)) {
				fYec35_tmp[j132] = fYec35_perm[j132];
				
			}
			for (int j134 = 0; (j134 < 4); j134 = (j134 + 1)) {
				fRec30_tmp[j134] = fRec30_perm[j134];
				
			}
			for (int j136 = 0; (j136 < 4); j136 = (j136 + 1)) {
				fRec28_tmp[j136] = fRec28_perm[j136];
				
			}
			fYec36_idx = ((fYec36_idx + fYec36_idx_save) & 16383);
			for (int j140 = 0; (j140 < 4); j140 = (j140 + 1)) {
				fYec37_tmp[j140] = fYec37_perm[j140];
				
			}
			for (int j142 = 0; (j142 < 4); j142 = (j142 + 1)) {
				fRec78_tmp[j142] = fRec78_perm[j142];
				
			}
			for (int j144 = 0; (j144 < 4); j144 = (j144 + 1)) {
				fRec29_tmp[j144] = fRec29_perm[j144];
				
			}
			fYec38_idx = ((fYec38_idx + fYec38_idx_save) & 16383);
			for (int j148 = 0; (j148 < 4); j148 = (j148 + 1)) {
				fYec39_tmp[j148] = fYec39_perm[j148];
				
			}
			for (int j150 = 0; (j150 < 4); j150 = (j150 + 1)) {
				fRec27_tmp[j150] = fRec27_perm[j150];
				
			}
			for (int j152 = 0; (j152 < 4); j152 = (j152 + 1)) {
				fRec25_tmp[j152] = fRec25_perm[j152];
				
			}
			fYec40_idx = ((fYec40_idx + fYec40_idx_save) & 16383);
			for (int j156 = 0; (j156 < 4); j156 = (j156 + 1)) {
				fYec41_tmp[j156] = fYec41_perm[j156];
				
			}
			for (int j158 = 0; (j158 < 4); j158 = (j158 + 1)) {
				fRec81_tmp[j158] = fRec81_perm[j158];
				
			}
			for (int j160 = 0; (j160 < 4); j160 = (j160 + 1)) {
				fRec26_tmp[j160] = fRec26_perm[j160];
				
			}
			fYec42_idx = ((fYec42_idx + fYec42_idx_save) & 16383);
			for (int j164 = 0; (j164 < 4); j164 = (j164 + 1)) {
				fYec43_tmp[j164] = fYec43_perm[j164];
				
			}
			for (int j166 = 0; (j166 < 4); j166 = (j166 + 1)) {
				fRec24_tmp[j166] = fRec24_perm[j166];
				
			}
			for (int j168 = 0; (j168 < 4); j168 = (j168 + 1)) {
				fRec22_tmp[j168] = fRec22_perm[j168];
				
			}
			fYec44_idx = ((fYec44_idx + fYec44_idx_save) & 16383);
			for (int j172 = 0; (j172 < 4); j172 = (j172 + 1)) {
				fYec45_tmp[j172] = fYec45_perm[j172];
				
			}
			for (int j174 = 0; (j174 < 4); j174 = (j174 + 1)) {
				fRec84_tmp[j174] = fRec84_perm[j174];
				
			}
			for (int j176 = 0; (j176 < 4); j176 = (j176 + 1)) {
				fRec23_tmp[j176] = fRec23_perm[j176];
				
			}
			fYec46_idx = ((fYec46_idx + fYec46_idx_save) & 16383);
			fYec47_idx = ((fYec47_idx + fYec47_idx_save) & 16383);
			for (int j180 = 0; (j180 < 4); j180 = (j180 + 1)) {
				fYec48_tmp[j180] = fYec48_perm[j180];
				
			}
			for (int j182 = 0; (j182 < 4); j182 = (j182 + 1)) {
				fRec21_tmp[j182] = fRec21_perm[j182];
				
			}
			for (int j184 = 0; (j184 < 4); j184 = (j184 + 1)) {
				fRec20_tmp[j184] = fRec20_perm[j184];
				
			}
			for (int j186 = 0; (j186 < 4); j186 = (j186 + 1)) {
				fRec19_tmp[j186] = fRec19_perm[j186];
				
			}
			for (int j188 = 0; (j188 < 4); j188 = (j188 + 1)) {
				fRec18_tmp[j188] = fRec18_perm[j188];
				
			}
			for (int j190 = 0; (j190 < 4); j190 = (j190 + 1)) {
				fRec17_tmp[j190] = fRec17_perm[j190];
				
			}
			for (int j192 = 0; (j192 < 4); j192 = (j192 + 1)) {
				fRec92_tmp[j192] = fRec92_perm[j192];
				
			}
			for (int j194 = 0; (j194 < 4); j194 = (j194 + 1)) {
				fRec91_tmp[j194] = fRec91_perm[j194];
				
			}
			for (int j196 = 0; (j196 < 4); j196 = (j196 + 1)) {
				fRec90_tmp[j196] = fRec90_perm[j196];
				
			}
			for (int j198 = 0; (j198 < 4); j198 = (j198 + 1)) {
				fYec49_tmp[j198] = fYec49_perm[j198];
				
			}
			for (int j200 = 0; (j200 < 4); j200 = (j200 + 1)) {
				fRec89_tmp[j200] = fRec89_perm[j200];
				
			}
			for (int j202 = 0; (j202 < 4); j202 = (j202 + 1)) {
				fRec88_tmp[j202] = fRec88_perm[j202];
				
			}
			for (int j204 = 0; (j204 < 4); j204 = (j204 + 1)) {
				fRec87_tmp[j204] = fRec87_perm[j204];
				
			}
			for (int j206 = 0; (j206 < 4); j206 = (j206 + 1)) {
				fRec95_tmp[j206] = fRec95_perm[j206];
				
			}
			for (int j208 = 0; (j208 < 4); j208 = (j208 + 1)) {
				fRec94_tmp[j208] = fRec94_perm[j208];
				
			}
			for (int j210 = 0; (j210 < 4); j210 = (j210 + 1)) {
				fRec93_tmp[j210] = fRec93_perm[j210];
				
			}
			fYec50_idx = ((fYec50_idx + fYec50_idx_save) & 1023);
			for (int j212 = 0; (j212 < 4); j212 = (j212 + 1)) {
				fRec14_tmp[j212] = fRec14_perm[j212];
				
			}
			fYec51_idx = ((fYec51_idx + fYec51_idx_save) & 16383);
			fYec52_idx = ((fYec52_idx + fYec52_idx_save) & 16383);
			for (int j216 = 0; (j216 < 4); j216 = (j216 + 1)) {
				fYec53_tmp[j216] = fYec53_perm[j216];
				
			}
			for (int j218 = 0; (j218 < 4); j218 = (j218 + 1)) {
				fRec101_tmp[j218] = fRec101_perm[j218];
				
			}
			for (int j220 = 0; (j220 < 4); j220 = (j220 + 1)) {
				fRec100_tmp[j220] = fRec100_perm[j220];
				
			}
			for (int j222 = 0; (j222 < 4); j222 = (j222 + 1)) {
				fRec99_tmp[j222] = fRec99_perm[j222];
				
			}
			for (int j224 = 0; (j224 < 4); j224 = (j224 + 1)) {
				fRec98_tmp[j224] = fRec98_perm[j224];
				
			}
			for (int j226 = 0; (j226 < 4); j226 = (j226 + 1)) {
				fRec97_tmp[j226] = fRec97_perm[j226];
				
			}
			for (int j228 = 0; (j228 < 4); j228 = (j228 + 1)) {
				fRec108_tmp[j228] = fRec108_perm[j228];
				
			}
			for (int j230 = 0; (j230 < 4); j230 = (j230 + 1)) {
				fRec107_tmp[j230] = fRec107_perm[j230];
				
			}
			for (int j232 = 0; (j232 < 4); j232 = (j232 + 1)) {
				fRec106_tmp[j232] = fRec106_perm[j232];
				
			}
			for (int j234 = 0; (j234 < 4); j234 = (j234 + 1)) {
				fYec54_tmp[j234] = fYec54_perm[j234];
				
			}
			for (int j236 = 0; (j236 < 4); j236 = (j236 + 1)) {
				fRec105_tmp[j236] = fRec105_perm[j236];
				
			}
			for (int j238 = 0; (j238 < 4); j238 = (j238 + 1)) {
				fRec104_tmp[j238] = fRec104_perm[j238];
				
			}
			for (int j240 = 0; (j240 < 4); j240 = (j240 + 1)) {
				fRec103_tmp[j240] = fRec103_perm[j240];
				
			}
			for (int j242 = 0; (j242 < 4); j242 = (j242 + 1)) {
				fRec111_tmp[j242] = fRec111_perm[j242];
				
			}
			for (int j244 = 0; (j244 < 4); j244 = (j244 + 1)) {
				fRec110_tmp[j244] = fRec110_perm[j244];
				
			}
			for (int j246 = 0; (j246 < 4); j246 = (j246 + 1)) {
				fRec109_tmp[j246] = fRec109_perm[j246];
				
			}
			fYec55_idx = ((fYec55_idx + fYec55_idx_save) & 1023);
			for (int j248 = 0; (j248 < 4); j248 = (j248 + 1)) {
				fRec96_tmp[j248] = fRec96_perm[j248];
				
			}
			fYec56_idx = ((fYec56_idx + fYec56_idx_save) & 16383);
			for (int j250 = 0; (j250 < 4); j250 = (j250 + 1)) {
				fYec57_tmp[j250] = fYec57_perm[j250];
				
			}
			for (int j252 = 0; (j252 < 4); j252 = (j252 + 1)) {
				fRec13_tmp[j252] = fRec13_perm[j252];
				
			}
			for (int j254 = 0; (j254 < 4); j254 = (j254 + 1)) {
				fRec11_tmp[j254] = fRec11_perm[j254];
				
			}
			fYec58_idx = ((fYec58_idx + fYec58_idx_save) & 16383);
			for (int j258 = 0; (j258 < 4); j258 = (j258 + 1)) {
				fYec59_tmp[j258] = fYec59_perm[j258];
				
			}
			for (int j260 = 0; (j260 < 4); j260 = (j260 + 1)) {
				fRec112_tmp[j260] = fRec112_perm[j260];
				
			}
			for (int j262 = 0; (j262 < 4); j262 = (j262 + 1)) {
				fRec12_tmp[j262] = fRec12_perm[j262];
				
			}
			fYec60_idx = ((fYec60_idx + fYec60_idx_save) & 16383);
			for (int j264 = 0; (j264 < 4); j264 = (j264 + 1)) {
				fYec61_tmp[j264] = fYec61_perm[j264];
				
			}
			for (int j266 = 0; (j266 < 4); j266 = (j266 + 1)) {
				fRec10_tmp[j266] = fRec10_perm[j266];
				
			}
			for (int j268 = 0; (j268 < 4); j268 = (j268 + 1)) {
				fRec8_tmp[j268] = fRec8_perm[j268];
				
			}
			fYec62_idx = ((fYec62_idx + fYec62_idx_save) & 16383);
			for (int j270 = 0; (j270 < 4); j270 = (j270 + 1)) {
				fYec63_tmp[j270] = fYec63_perm[j270];
				
			}
			for (int j272 = 0; (j272 < 4); j272 = (j272 + 1)) {
				fRec114_tmp[j272] = fRec114_perm[j272];
				
			}
			for (int j274 = 0; (j274 < 4); j274 = (j274 + 1)) {
				fRec9_tmp[j274] = fRec9_perm[j274];
				
			}
			fYec64_idx = ((fYec64_idx + fYec64_idx_save) & 16383);
			for (int j276 = 0; (j276 < 4); j276 = (j276 + 1)) {
				fYec65_tmp[j276] = fYec65_perm[j276];
				
			}
			for (int j278 = 0; (j278 < 4); j278 = (j278 + 1)) {
				fRec7_tmp[j278] = fRec7_perm[j278];
				
			}
			for (int j280 = 0; (j280 < 4); j280 = (j280 + 1)) {
				fRec5_tmp[j280] = fRec5_perm[j280];
				
			}
			fYec66_idx = ((fYec66_idx + fYec66_idx_save) & 16383);
			for (int j284 = 0; (j284 < 4); j284 = (j284 + 1)) {
				fYec67_tmp[j284] = fYec67_perm[j284];
				
			}
			for (int j286 = 0; (j286 < 4); j286 = (j286 + 1)) {
				fRec115_tmp[j286] = fRec115_perm[j286];
				
			}
			for (int j288 = 0; (j288 < 4); j288 = (j288 + 1)) {
				fRec6_tmp[j288] = fRec6_perm[j288];
				
			}
			fYec68_idx = ((fYec68_idx + fYec68_idx_save) & 16383);
			for (int j292 = 0; (j292 < 4); j292 = (j292 + 1)) {
				fYec69_tmp[j292] = fYec69_perm[j292];
				
			}
			for (int j294 = 0; (j294 < 4); j294 = (j294 + 1)) {
				fRec4_tmp[j294] = fRec4_perm[j294];
				
			}
			for (int j296 = 0; (j296 < 4); j296 = (j296 + 1)) {
				fRec2_tmp[j296] = fRec2_perm[j296];
				
			}
			fYec70_idx = ((fYec70_idx + fYec70_idx_save) & 16383);
			for (int j298 = 0; (j298 < 4); j298 = (j298 + 1)) {
				fYec71_tmp[j298] = fYec71_perm[j298];
				
			}
			for (int j300 = 0; (j300 < 4); j300 = (j300 + 1)) {
				fRec118_tmp[j300] = fRec118_perm[j300];
				
			}
			for (int j302 = 0; (j302 < 4); j302 = (j302 + 1)) {
				fRec3_tmp[j302] = fRec3_perm[j302];
				
			}
			for (int j304 = 0; (j304 < 4); j304 = (j304 + 1)) {
				fRec0_tmp[j304] = fRec0_perm[j304];
				
			}
			for (int j306 = 0; (j306 < 4); j306 = (j306 + 1)) {
				fRec1_tmp[j306] = fRec1_perm[j306];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec1[i] = ((0.760313988f * fRec0[(i - 1)]) - (0.649555743f * fRec50[(i - 1)]));
				fZec2[i] = ((0.760313988f * fRec1[(i - 1)]) - (0.649555743f * fRec51[(i - 1)]));
				fYec0[((i + fYec0_idx) & 16383)] = ((0.707106769f * fZec1[i]) - (0.707106769f * fZec2[i]));
				fYec1[i] = fYec0[(((i + fYec0_idx) - std::min<int>(8192, std::max<int>(0, int(fZec3[i])))) & 16383)];
				fRec52[i] = (fYec1[(i - 1)] - (((fZec4[i] + (2.0f - fRec53[i])) * (fRec52[(i - 1)] - fYec1[i])) / (fRec53[i] - fZec4[i])));
				fRec50[i] = fRec52[i];
				fYec2[((i + fYec2_idx) & 16383)] = ((0.707106769f * fZec1[i]) + (0.707106769f * fZec2[i]));
				fYec3[i] = fYec2[(((i + fYec2_idx) - std::min<int>(8192, std::max<int>(0, int(fZec5[i])))) & 16383)];
				fRec54[i] = (fYec3[(i - 1)] - (((fZec6[i] + (2.0f - fRec55[i])) * (fRec54[(i - 1)] - fYec3[i])) / (fRec55[i] - fZec6[i])));
				fRec51[i] = fRec54[i];
				fZec7[i] = ((0.760313988f * fRec50[(i - 1)]) + (0.649555743f * fRec0[(i - 1)]));
				fZec8[i] = ((0.760313988f * fZec7[i]) - (0.649555743f * fRec47[(i - 1)]));
				fZec9[i] = ((0.760313988f * fRec51[(i - 1)]) + (0.649555743f * fRec1[(i - 1)]));
				fZec10[i] = ((0.760313988f * fZec9[i]) - (0.649555743f * fRec48[(i - 1)]));
				fYec4[((i + fYec4_idx) & 16383)] = ((0.707106769f * fZec8[i]) - (0.707106769f * fZec10[i]));
				fYec5[i] = fYec4[(((i + fYec4_idx) - std::min<int>(8192, std::max<int>(0, int(fZec11[i])))) & 16383)];
				fRec49[i] = (fYec5[(i - 1)] - (((fZec12[i] + (2.0f - fRec56[i])) * (fRec49[(i - 1)] - fYec5[i])) / (fRec56[i] - fZec12[i])));
				fRec47[i] = fRec49[i];
				fYec6[((i + fYec6_idx) & 16383)] = ((0.707106769f * fZec8[i]) + (0.707106769f * fZec10[i]));
				fYec7[i] = fYec6[(((i + fYec6_idx) - std::min<int>(8192, std::max<int>(0, int(fZec13[i])))) & 16383)];
				fRec57[i] = (fYec7[(i - 1)] - (((fZec14[i] + (2.0f - fRec58[i])) * (fRec57[(i - 1)] - fYec7[i])) / (fRec58[i] - fZec14[i])));
				fRec48[i] = fRec57[i];
				fZec15[i] = ((0.760313988f * fRec47[(i - 1)]) + (0.649555743f * fZec7[i]));
				fZec16[i] = ((0.760313988f * fZec15[i]) - (0.649555743f * fRec44[(i - 1)]));
				fZec17[i] = ((0.760313988f * fRec48[(i - 1)]) + (0.649555743f * fZec9[i]));
				fZec18[i] = ((0.760313988f * fZec17[i]) - (0.649555743f * fRec45[(i - 1)]));
				fYec8[((i + fYec8_idx) & 16383)] = ((0.707106769f * fZec16[i]) - (0.707106769f * fZec18[i]));
				fYec9[i] = fYec8[(((i + fYec8_idx) - std::min<int>(8192, std::max<int>(0, int(fZec19[i])))) & 16383)];
				fRec46[i] = (fYec9[(i - 1)] - (((fZec20[i] + (2.0f - fRec59[i])) * (fRec46[(i - 1)] - fYec9[i])) / (fRec59[i] - fZec20[i])));
				fRec44[i] = fRec46[i];
				fYec10[((i + fYec10_idx) & 16383)] = ((0.707106769f * fZec16[i]) + (0.707106769f * fZec18[i]));
				fYec11[i] = fYec10[(((i + fYec10_idx) - std::min<int>(8192, std::max<int>(0, int(fZec21[i])))) & 16383)];
				fRec60[i] = (fYec11[(i - 1)] - (((fZec22[i] + (2.0f - fRec61[i])) * (fRec60[(i - 1)] - fYec11[i])) / (fRec61[i] - fZec22[i])));
				fRec45[i] = fRec60[i];
				fZec23[i] = ((0.760313988f * fRec44[(i - 1)]) + (0.649555743f * fZec15[i]));
				fZec24[i] = ((0.760313988f * fZec23[i]) - (0.649555743f * fRec41[(i - 1)]));
				fZec25[i] = ((0.760313988f * fRec45[(i - 1)]) + (0.649555743f * fZec17[i]));
				fZec26[i] = ((0.760313988f * fZec25[i]) - (0.649555743f * fRec42[(i - 1)]));
				fYec12[((i + fYec12_idx) & 16383)] = ((0.707106769f * fZec24[i]) - (0.707106769f * fZec26[i]));
				fYec13[i] = fYec12[(((i + fYec12_idx) - std::min<int>(8192, std::max<int>(0, int(fZec27[i])))) & 16383)];
				fRec43[i] = (fYec13[(i - 1)] - (((fZec28[i] + (2.0f - fRec62[i])) * (fRec43[(i - 1)] - fYec13[i])) / (fRec62[i] - fZec28[i])));
				fRec41[i] = fRec43[i];
				fYec14[((i + fYec14_idx) & 16383)] = ((0.707106769f * fZec24[i]) + (0.707106769f * fZec26[i]));
				fYec15[i] = fYec14[(((i + fYec14_idx) - std::min<int>(8192, std::max<int>(0, int(fZec29[i])))) & 16383)];
				fRec63[i] = (fYec15[(i - 1)] - (((fZec30[i] + (2.0f - fRec64[i])) * (fRec63[(i - 1)] - fYec15[i])) / (fRec64[i] - fZec30[i])));
				fRec42[i] = fRec63[i];
				fZec31[i] = ((0.760313988f * fRec41[(i - 1)]) + (0.649555743f * fZec23[i]));
				fZec32[i] = ((0.760313988f * fZec31[i]) - (0.649555743f * fRec38[(i - 1)]));
				fZec33[i] = ((0.760313988f * fRec42[(i - 1)]) + (0.649555743f * fZec25[i]));
				fZec34[i] = ((0.760313988f * fZec33[i]) - (0.649555743f * fRec39[(i - 1)]));
				fYec16[((i + fYec16_idx) & 16383)] = ((0.707106769f * fZec32[i]) - (0.707106769f * fZec34[i]));
				fYec17[i] = fYec16[(((i + fYec16_idx) - std::min<int>(8192, std::max<int>(0, int(fZec35[i])))) & 16383)];
				fRec40[i] = (fYec17[(i - 1)] - (((fZec36[i] + (2.0f - fRec65[i])) * (fRec40[(i - 1)] - fYec17[i])) / (fRec65[i] - fZec36[i])));
				fRec38[i] = fRec40[i];
				fYec18[((i + fYec18_idx) & 16383)] = ((0.707106769f * fZec32[i]) + (0.707106769f * fZec34[i]));
				fYec19[i] = fYec18[(((i + fYec18_idx) - std::min<int>(8192, std::max<int>(0, int(fZec37[i])))) & 16383)];
				fRec66[i] = (fYec19[(i - 1)] - (((fZec38[i] + (2.0f - fRec67[i])) * (fRec66[(i - 1)] - fYec19[i])) / (fRec67[i] - fZec38[i])));
				fRec39[i] = fRec66[i];
				fYec20[((i + fYec20_idx) & 1023)] = ((0.760313988f * fRec38[(i - 1)]) + (0.649555743f * fZec31[i]));
				fYec21[((i + fYec21_idx) & 16383)] = (((((fYec20[(((i + fYec20_idx) - iZec42[i]) & 1023)] * fZec45[i]) * fZec47[i]) * fZec49[i]) * fZec51[i]) + (fZec52[i] * ((((((fYec20[(((i + fYec20_idx) - iZec53[i]) & 1023)] * fZec54[i]) * fZec55[i]) * fZec56[i]) + (0.5f * (((fZec44[i] * fYec20[(((i + fYec20_idx) - iZec57[i]) & 1023)]) * fZec58[i]) * fZec59[i]))) + (0.166666672f * ((fZec60[i] * fYec20[(((i + fYec20_idx) - iZec61[i]) & 1023)]) * fZec62[i]))) + (0.0416666679f * (fZec63[i] * fYec20[(((i + fYec20_idx) - iZec64[i]) & 1023)])))));
				fYec22[i] = fYec21[(((i + fYec21_idx) - std::min<int>(8192, std::max<int>(0, int(fZec65[i])))) & 16383)];
				fRec37[i] = (fYec22[(i - 1)] - (((fZec66[i] + (2.0f - fRec68[i])) * (fRec37[(i - 1)] - fYec22[i])) / (fRec68[i] - fZec66[i])));
				fYec23[((i + fYec23_idx) & 1023)] = ((0.760313988f * fRec39[(i - 1)]) + (0.649555743f * fZec33[i]));
				fYec24[((i + fYec24_idx) & 16383)] = (((((fYec23[(((i + fYec23_idx) - std::min<int>(512, std::max<int>(0, iZec69[i]))) & 1023)] * (0.0f - fZec71[i])) * (0.0f - (0.5f * fZec72[i]))) * (0.0f - (0.333333343f * fZec73[i]))) * (0.0f - (0.25f * fZec74[i]))) + ((fSlow26 + (fZec67[i] + (5.0f - fZec70[i]))) * ((((((fYec23[(((i + fYec23_idx) - std::min<int>(512, std::max<int>(0, (iZec69[i] + 1)))) & 1023)] * (0.0f - fZec72[i])) * (0.0f - (0.5f * fZec73[i]))) * (0.0f - (0.333333343f * fZec74[i]))) + (0.5f * (((fZec71[i] * fYec23[(((i + fYec23_idx) - std::min<int>(512, std::max<int>(0, (iZec69[i] + 2)))) & 1023)]) * (0.0f - fZec73[i])) * (0.0f - (0.5f * fZec74[i]))))) + (0.166666672f * ((fZec75[i] * fYec23[(((i + fYec23_idx) - std::min<int>(512, std::max<int>(0, (iZec69[i] + 3)))) & 1023)]) * (0.0f - fZec74[i])))) + (0.0416666679f * ((fZec75[i] * fZec73[i]) * fYec23[(((i + fYec23_idx) - std::min<int>(512, std::max<int>(0, (iZec69[i] + 4)))) & 1023)])))));
				fYec25[i] = fYec24[(((i + fYec24_idx) - std::min<int>(8192, std::max<int>(0, int(fZec76[i])))) & 16383)];
				fRec69[i] = (fYec25[(i - 1)] - (((fZec77[i] + (2.0f - fRec70[i])) * (fRec69[(i - 1)] - fYec25[i])) / (fRec70[i] - fZec77[i])));
				fZec78[i] = ((0.760313988f * fRec37[i]) - (0.649555743f * fRec34[(i - 1)]));
				fZec79[i] = ((0.760313988f * fRec69[i]) - (0.649555743f * fRec35[(i - 1)]));
				fYec26[((i + fYec26_idx) & 16383)] = ((0.707106769f * fZec78[i]) - (0.707106769f * fZec79[i]));
				fYec27[i] = fYec26[(((i + fYec26_idx) - std::min<int>(8192, std::max<int>(0, int(fZec80[i])))) & 16383)];
				fRec36[i] = (fYec27[(i - 1)] - (((fZec81[i] + (2.0f - fRec71[i])) * (fRec36[(i - 1)] - fYec27[i])) / (fRec71[i] - fZec81[i])));
				fRec34[i] = fRec36[i];
				fYec28[((i + fYec28_idx) & 16383)] = ((0.707106769f * fZec78[i]) + (0.707106769f * fZec79[i]));
				fYec29[i] = fYec28[(((i + fYec28_idx) - std::min<int>(8192, std::max<int>(0, int(fZec82[i])))) & 16383)];
				fRec72[i] = (fYec29[(i - 1)] - (((fZec83[i] + (2.0f - fRec73[i])) * (fRec72[(i - 1)] - fYec29[i])) / (fRec73[i] - fZec83[i])));
				fRec35[i] = fRec72[i];
				fZec84[i] = ((0.760313988f * fRec34[(i - 1)]) + (0.649555743f * fRec37[i]));
				fZec85[i] = ((0.760313988f * fZec84[i]) - (0.649555743f * fRec31[(i - 1)]));
				fZec86[i] = ((0.760313988f * fRec35[(i - 1)]) + (0.649555743f * fRec69[i]));
				fZec87[i] = ((0.760313988f * fZec86[i]) - (0.649555743f * fRec32[(i - 1)]));
				fYec30[((i + fYec30_idx) & 16383)] = ((0.707106769f * fZec85[i]) - (0.707106769f * fZec87[i]));
				fYec31[i] = fYec30[(((i + fYec30_idx) - std::min<int>(8192, std::max<int>(0, int(fZec88[i])))) & 16383)];
				fRec33[i] = (fYec31[(i - 1)] - (((fZec89[i] + (2.0f - fRec74[i])) * (fRec33[(i - 1)] - fYec31[i])) / (fRec74[i] - fZec89[i])));
				fRec31[i] = fRec33[i];
				fYec32[((i + fYec32_idx) & 16383)] = ((0.707106769f * fZec85[i]) + (0.707106769f * fZec87[i]));
				fYec33[i] = fYec32[(((i + fYec32_idx) - iZec91[i]) & 16383)];
				fRec75[i] = (fYec33[(i - 1)] - ((fZec93[i] * (fRec75[(i - 1)] - fYec33[i])) / fZec94[i]));
				fRec32[i] = fRec75[i];
				fZec95[i] = ((0.760313988f * fRec31[(i - 1)]) + (0.649555743f * fZec84[i]));
				fZec96[i] = ((0.760313988f * fZec95[i]) - (0.649555743f * fRec28[(i - 1)]));
				fZec97[i] = ((0.760313988f * fRec32[(i - 1)]) + (0.649555743f * fZec86[i]));
				fZec98[i] = ((0.760313988f * fZec97[i]) - (0.649555743f * fRec29[(i - 1)]));
				fYec34[((i + fYec34_idx) & 16383)] = ((0.707106769f * fZec96[i]) - (0.707106769f * fZec98[i]));
				fYec35[i] = fYec34[(((i + fYec34_idx) - std::min<int>(8192, std::max<int>(0, int(fZec99[i])))) & 16383)];
				fRec30[i] = (fYec35[(i - 1)] - (((fZec100[i] + (2.0f - fRec77[i])) * (fRec30[(i - 1)] - fYec35[i])) / (fRec77[i] - fZec100[i])));
				fRec28[i] = fRec30[i];
				fYec36[((i + fYec36_idx) & 16383)] = ((0.707106769f * fZec96[i]) + (0.707106769f * fZec98[i]));
				fYec37[i] = fYec36[(((i + fYec36_idx) - iZec102[i]) & 16383)];
				fRec78[i] = (fYec37[(i - 1)] - ((fZec104[i] * (fRec78[(i - 1)] - fYec37[i])) / fZec105[i]));
				fRec29[i] = fRec78[i];
				fZec106[i] = ((0.760313988f * fRec28[(i - 1)]) + (0.649555743f * fZec95[i]));
				fZec107[i] = ((0.760313988f * fZec106[i]) - (0.649555743f * fRec25[(i - 1)]));
				fZec108[i] = ((0.760313988f * fRec29[(i - 1)]) + (0.649555743f * fZec97[i]));
				fZec109[i] = ((0.760313988f * fZec108[i]) - (0.649555743f * fRec26[(i - 1)]));
				fYec38[((i + fYec38_idx) & 16383)] = ((0.707106769f * fZec107[i]) - (0.707106769f * fZec109[i]));
				fYec39[i] = fYec38[(((i + fYec38_idx) - iZec111[i]) & 16383)];
				fRec27[i] = (fYec39[(i - 1)] - ((fZec113[i] * (fRec27[(i - 1)] - fYec39[i])) / fZec114[i]));
				fRec25[i] = fRec27[i];
				fYec40[((i + fYec40_idx) & 16383)] = ((0.707106769f * fZec107[i]) + (0.707106769f * fZec109[i]));
				fYec41[i] = fYec40[(((i + fYec40_idx) - iZec116[i]) & 16383)];
				fRec81[i] = (fYec41[(i - 1)] - ((fZec118[i] * (fRec81[(i - 1)] - fYec41[i])) / fZec119[i]));
				fRec26[i] = fRec81[i];
				fZec120[i] = ((0.760313988f * fRec25[(i - 1)]) + (0.649555743f * fZec106[i]));
				fZec121[i] = ((0.760313988f * fZec120[i]) - (0.649555743f * fRec22[(i - 1)]));
				fZec122[i] = ((0.760313988f * fRec26[(i - 1)]) + (0.649555743f * fZec108[i]));
				fZec123[i] = ((0.760313988f * fZec122[i]) - (0.649555743f * fRec23[(i - 1)]));
				fYec42[((i + fYec42_idx) & 16383)] = ((0.707106769f * fZec121[i]) - (0.707106769f * fZec123[i]));
				fYec43[i] = fYec42[(((i + fYec42_idx) - std::min<int>(8192, std::max<int>(0, int(fZec124[i])))) & 16383)];
				fRec24[i] = (fYec43[(i - 1)] - (((fZec125[i] + (2.0f - fRec83[i])) * (fRec24[(i - 1)] - fYec43[i])) / (fRec83[i] - fZec125[i])));
				fRec22[i] = fRec24[i];
				fYec44[((i + fYec44_idx) & 16383)] = ((0.707106769f * fZec121[i]) + (0.707106769f * fZec123[i]));
				fYec45[i] = fYec44[(((i + fYec44_idx) - iZec127[i]) & 16383)];
				fRec84[i] = (fYec45[(i - 1)] - ((fZec129[i] * (fRec84[(i - 1)] - fYec45[i])) / fZec130[i]));
				fRec23[i] = fRec84[i];
				fYec46[((i + fYec46_idx) & 16383)] = ((0.760313988f * fRec22[(i - 1)]) + (0.649555743f * fZec120[i]));
				fYec47[((i + fYec47_idx) & 16383)] = (((((fYec46[(((i + fYec46_idx) - std::min<int>(8192, iZec134[i])) & 16383)] * fZec137[i]) * fZec139[i]) * fZec141[i]) * fZec143[i]) + (fZec144[i] * ((((((fYec46[(((i + fYec46_idx) - std::min<int>(8192, iZec145[i])) & 16383)] * fZec146[i]) * fZec147[i]) * fZec148[i]) + (0.5f * (((fZec136[i] * fYec46[(((i + fYec46_idx) - std::min<int>(8192, iZec149[i])) & 16383)]) * fZec150[i]) * fZec151[i]))) + (0.166666672f * ((fZec152[i] * fYec46[(((i + fYec46_idx) - std::min<int>(8192, iZec153[i])) & 16383)]) * fZec154[i]))) + (0.0416666679f * (fZec155[i] * fYec46[(((i + fYec46_idx) - std::min<int>(8192, iZec156[i])) & 16383)])))));
				fYec48[i] = fYec47[(((i + fYec47_idx) - std::min<int>(8192, std::max<int>(0, int(fZec157[i])))) & 16383)];
				fRec21[i] = (fYec48[(i - 1)] - (((fZec158[i] + (2.0f - fRec86[i])) * (fRec21[(i - 1)] - fYec48[i])) / (fRec86[i] - fZec158[i])));
				fRec20[i] = ((fSlow55 * fRec21[(i - 1)]) - (fSlow56 * ((fSlow57 * fRec20[(i - 1)]) - (fSlow53 * fRec21[i]))));
				fRec19[i] = (fRec20[i] - (fSlow58 * ((fSlow59 * fRec19[(i - 2)]) + (fSlow62 * fRec19[(i - 1)]))));
				fRec18[i] = ((fSlow58 * (((fSlow61 * fRec19[i]) + (fSlow63 * fRec19[(i - 1)])) + (fSlow61 * fRec19[(i - 2)]))) - (fSlow65 * ((fSlow66 * fRec18[(i - 2)]) + (fSlow62 * fRec18[(i - 1)]))));
				fZec159[i] = (fSlow73 * fRec17[(i - 1)]);
				fRec17[i] = ((fSlow65 * (((fSlow61 * fRec18[i]) + (fSlow63 * fRec18[(i - 1)])) + (fSlow61 * fRec18[(i - 2)]))) - (fSlow69 * ((fSlow70 * fRec17[(i - 2)]) + fZec159[i])));
				fRec92[i] = (0.0f - (fSlow56 * ((fSlow57 * fRec92[(i - 1)]) - (fRec21[i] + fRec21[(i - 1)]))));
				fRec91[i] = (fRec92[i] - (fSlow58 * ((fSlow59 * fRec91[(i - 2)]) + (fSlow62 * fRec91[(i - 1)]))));
				fRec90[i] = ((fSlow58 * (fRec91[(i - 2)] + (fRec91[i] + (2.0f * fRec91[(i - 1)])))) - (fSlow65 * ((fSlow66 * fRec90[(i - 2)]) + (fSlow62 * fRec90[(i - 1)]))));
				fZec160[i] = (fRec90[(i - 2)] + (fRec90[i] + (2.0f * fRec90[(i - 1)])));
				fYec49[i] = (fSlow65 * fZec160[i]);
				fRec89[i] = ((fSlow75 * fYec49[(i - 1)]) - (fSlow76 * ((fSlow77 * fRec89[(i - 1)]) - (fSlow78 * fZec160[i]))));
				fRec88[i] = (fRec89[i] - (fSlow79 * ((fSlow80 * fRec88[(i - 2)]) + (fSlow73 * fRec88[(i - 1)]))));
				fRec87[i] = ((fSlow79 * (((fSlow72 * fRec88[i]) + (fSlow81 * fRec88[(i - 1)])) + (fSlow72 * fRec88[(i - 2)]))) - (fSlow82 * ((fSlow83 * fRec87[(i - 2)]) + (fSlow73 * fRec87[(i - 1)]))));
				fRec95[i] = (0.0f - (fSlow76 * ((fSlow77 * fRec95[(i - 1)]) - (fYec49[i] + fYec49[(i - 1)]))));
				fRec94[i] = (fRec95[i] - (fSlow79 * ((fSlow80 * fRec94[(i - 2)]) + (fSlow73 * fRec94[(i - 1)]))));
				fRec93[i] = ((fSlow79 * (fRec94[(i - 2)] + (fRec94[i] + (2.0f * fRec94[(i - 1)])))) - (fSlow82 * ((fSlow83 * fRec93[(i - 2)]) + (fSlow73 * fRec93[(i - 1)]))));
				fYec50[((i + fYec50_idx) & 1023)] = ((fSlow86 * ((fSlow87 * (fRec17[(i - 2)] + (fSlow69 * (fZec159[i] + (fSlow70 * fRec17[i]))))) + (fSlow82 * ((fSlow88 * (((fSlow72 * fRec87[i]) + (fSlow81 * fRec87[(i - 1)])) + (fSlow72 * fRec87[(i - 2)]))) + (fSlow89 * (fRec93[(i - 2)] + (fRec93[i] + (2.0f * fRec93[(i - 1)])))))))) + float(input0[i]));
				fRec14[i] = ((fSlow84 * fRec14[(i - 1)]) + (fSlow85 * (((((fZec137[i] * fZec139[i]) * fZec141[i]) * fZec143[i]) * fYec50[(((i + fYec50_idx) - std::min<int>(512, iZec134[i])) & 1023)]) + (fZec144[i] * ((((((fZec146[i] * fZec147[i]) * fZec148[i]) * fYec50[(((i + fYec50_idx) - std::min<int>(512, iZec145[i])) & 1023)]) + (0.5f * (((fZec136[i] * fZec150[i]) * fZec151[i]) * fYec50[(((i + fYec50_idx) - std::min<int>(512, iZec149[i])) & 1023)]))) + (0.166666672f * ((fZec152[i] * fZec154[i]) * fYec50[(((i + fYec50_idx) - std::min<int>(512, iZec153[i])) & 1023)]))) + (0.0416666679f * (fZec155[i] * fYec50[(((i + fYec50_idx) - std::min<int>(512, iZec156[i])) & 1023)])))))));
				fYec51[((i + fYec51_idx) & 16383)] = ((0.760313988f * fRec23[(i - 1)]) + (0.649555743f * fZec122[i]));
				fYec52[((i + fYec52_idx) & 16383)] = (((((fYec51[(((i + fYec51_idx) - std::min<int>(8192, std::max<int>(0, iZec163[i]))) & 16383)] * (0.0f - fZec165[i])) * (0.0f - (0.5f * fZec166[i]))) * (0.0f - (0.333333343f * fZec167[i]))) * (0.0f - (0.25f * fZec168[i]))) + ((fSlow26 + (fZec161[i] + (5.0f - fZec164[i]))) * ((((((fYec51[(((i + fYec51_idx) - std::min<int>(8192, std::max<int>(0, (iZec163[i] + 1)))) & 16383)] * (0.0f - fZec166[i])) * (0.0f - (0.5f * fZec167[i]))) * (0.0f - (0.333333343f * fZec168[i]))) + (0.5f * (((fZec165[i] * fYec51[(((i + fYec51_idx) - std::min<int>(8192, std::max<int>(0, (iZec163[i] + 2)))) & 16383)]) * (0.0f - fZec167[i])) * (0.0f - (0.5f * fZec168[i]))))) + (0.166666672f * ((fZec169[i] * fYec51[(((i + fYec51_idx) - std::min<int>(8192, std::max<int>(0, (iZec163[i] + 3)))) & 16383)]) * (0.0f - fZec168[i])))) + (0.0416666679f * ((fZec169[i] * fZec167[i]) * fYec51[(((i + fYec51_idx) - std::min<int>(8192, std::max<int>(0, (iZec163[i] + 4)))) & 16383)])))));
				fYec53[i] = fYec52[(((i + fYec52_idx) - std::min<int>(8192, std::max<int>(0, int(fZec170[i])))) & 16383)];
				fRec101[i] = (fYec53[(i - 1)] - (((fZec171[i] + (2.0f - fRec102[i])) * (fRec101[(i - 1)] - fYec53[i])) / (fRec102[i] - fZec171[i])));
				fRec100[i] = ((fSlow55 * fRec101[(i - 1)]) - (fSlow56 * ((fSlow57 * fRec100[(i - 1)]) - (fSlow53 * fRec101[i]))));
				fRec99[i] = (fRec100[i] - (fSlow58 * ((fSlow59 * fRec99[(i - 2)]) + (fSlow62 * fRec99[(i - 1)]))));
				fRec98[i] = ((fSlow58 * (((fSlow61 * fRec99[i]) + (fSlow63 * fRec99[(i - 1)])) + (fSlow61 * fRec99[(i - 2)]))) - (fSlow65 * ((fSlow66 * fRec98[(i - 2)]) + (fSlow62 * fRec98[(i - 1)]))));
				fZec172[i] = (fSlow73 * fRec97[(i - 1)]);
				fRec97[i] = ((fSlow65 * (((fSlow61 * fRec98[i]) + (fSlow63 * fRec98[(i - 1)])) + (fSlow61 * fRec98[(i - 2)]))) - (fSlow69 * ((fSlow70 * fRec97[(i - 2)]) + fZec172[i])));
				fRec108[i] = (0.0f - (fSlow56 * ((fSlow57 * fRec108[(i - 1)]) - (fRec101[i] + fRec101[(i - 1)]))));
				fRec107[i] = (fRec108[i] - (fSlow58 * ((fSlow59 * fRec107[(i - 2)]) + (fSlow62 * fRec107[(i - 1)]))));
				fRec106[i] = ((fSlow58 * (fRec107[(i - 2)] + (fRec107[i] + (2.0f * fRec107[(i - 1)])))) - (fSlow65 * ((fSlow66 * fRec106[(i - 2)]) + (fSlow62 * fRec106[(i - 1)]))));
				fZec173[i] = (fRec106[(i - 2)] + (fRec106[i] + (2.0f * fRec106[(i - 1)])));
				fYec54[i] = (fSlow65 * fZec173[i]);
				fRec105[i] = ((fSlow75 * fYec54[(i - 1)]) - (fSlow76 * ((fSlow77 * fRec105[(i - 1)]) - (fSlow78 * fZec173[i]))));
				fRec104[i] = (fRec105[i] - (fSlow79 * ((fSlow80 * fRec104[(i - 2)]) + (fSlow73 * fRec104[(i - 1)]))));
				fRec103[i] = ((fSlow79 * (((fSlow72 * fRec104[i]) + (fSlow81 * fRec104[(i - 1)])) + (fSlow72 * fRec104[(i - 2)]))) - (fSlow82 * ((fSlow83 * fRec103[(i - 2)]) + (fSlow73 * fRec103[(i - 1)]))));
				fRec111[i] = (0.0f - (fSlow76 * ((fSlow77 * fRec111[(i - 1)]) - (fYec54[i] + fYec54[(i - 1)]))));
				fRec110[i] = (fRec111[i] - (fSlow79 * ((fSlow80 * fRec110[(i - 2)]) + (fSlow73 * fRec110[(i - 1)]))));
				fRec109[i] = ((fSlow79 * (fRec110[(i - 2)] + (fRec110[i] + (2.0f * fRec110[(i - 1)])))) - (fSlow82 * ((fSlow83 * fRec109[(i - 2)]) + (fSlow73 * fRec109[(i - 1)]))));
				fYec55[((i + fYec55_idx) & 1023)] = ((fSlow86 * ((fSlow87 * (fRec97[(i - 2)] + (fSlow69 * (fZec172[i] + (fSlow70 * fRec97[i]))))) + (fSlow82 * ((fSlow88 * (((fSlow72 * fRec103[i]) + (fSlow81 * fRec103[(i - 1)])) + (fSlow72 * fRec103[(i - 2)]))) + (fSlow89 * (fRec109[(i - 2)] + (fRec109[i] + (2.0f * fRec109[(i - 1)])))))))) + float(input1[i]));
				fRec96[i] = ((fSlow84 * fRec96[(i - 1)]) + (fSlow85 * (((((fZec45[i] * fZec47[i]) * fZec49[i]) * fZec51[i]) * fYec55[(((i + fYec55_idx) - iZec42[i]) & 1023)]) + (fZec52[i] * ((((((fZec54[i] * fZec55[i]) * fZec56[i]) * fYec55[(((i + fYec55_idx) - iZec53[i]) & 1023)]) + (0.5f * (((fZec44[i] * fZec58[i]) * fZec59[i]) * fYec55[(((i + fYec55_idx) - iZec57[i]) & 1023)]))) + (0.166666672f * ((fZec60[i] * fZec62[i]) * fYec55[(((i + fYec55_idx) - iZec61[i]) & 1023)]))) + (0.0416666679f * (fZec63[i] * fYec55[(((i + fYec55_idx) - iZec64[i]) & 1023)])))))));
				fZec174[i] = ((fSlow93 * fRec14[i]) - (fSlow94 * fRec11[(i - 1)]));
				fZec175[i] = ((fSlow93 * fRec96[i]) - (fSlow94 * fRec12[(i - 1)]));
				fYec56[((i + fYec56_idx) & 16383)] = ((0.707106769f * fZec174[i]) - (0.707106769f * fZec175[i]));
				fYec57[i] = fYec56[(((i + fYec56_idx) - iZec91[i]) & 16383)];
				fRec13[i] = (fYec57[(i - 1)] - ((fZec93[i] * (fRec13[(i - 1)] - fYec57[i])) / fZec94[i]));
				fRec11[i] = fRec13[i];
				fYec58[((i + fYec58_idx) & 16383)] = ((0.707106769f * fZec174[i]) + (0.707106769f * fZec175[i]));
				fYec59[i] = fYec58[(((i + fYec58_idx) - std::min<int>(8192, std::max<int>(0, int(fZec176[i])))) & 16383)];
				fRec112[i] = (fYec59[(i - 1)] - (((fZec177[i] + (2.0f - fRec113[i])) * (fRec112[(i - 1)] - fYec59[i])) / (fRec113[i] - fZec177[i])));
				fRec12[i] = fRec112[i];
				fZec178[i] = ((fSlow93 * fRec11[(i - 1)]) + (fSlow94 * fRec14[i]));
				fZec179[i] = ((fSlow93 * fZec178[i]) - (fSlow94 * fRec8[(i - 1)]));
				fZec180[i] = ((fSlow93 * fRec12[(i - 1)]) + (fSlow94 * fRec96[i]));
				fZec181[i] = ((fSlow93 * fZec180[i]) - (fSlow94 * fRec9[(i - 1)]));
				fYec60[((i + fYec60_idx) & 16383)] = ((0.707106769f * fZec179[i]) - (0.707106769f * fZec181[i]));
				fYec61[i] = fYec60[(((i + fYec60_idx) - iZec111[i]) & 16383)];
				fRec10[i] = (fYec61[(i - 1)] - ((fZec113[i] * (fRec10[(i - 1)] - fYec61[i])) / fZec114[i]));
				fRec8[i] = fRec10[i];
				fYec62[((i + fYec62_idx) & 16383)] = ((0.707106769f * fZec179[i]) + (0.707106769f * fZec181[i]));
				fYec63[i] = fYec62[(((i + fYec62_idx) - iZec102[i]) & 16383)];
				fRec114[i] = (fYec63[(i - 1)] - ((fZec104[i] * (fRec114[(i - 1)] - fYec63[i])) / fZec105[i]));
				fRec9[i] = fRec114[i];
				fZec182[i] = ((fSlow93 * fRec8[(i - 1)]) + (fSlow94 * fZec178[i]));
				fZec183[i] = ((fSlow93 * fZec182[i]) - (fSlow94 * fRec5[(i - 1)]));
				fZec184[i] = ((fSlow93 * fRec9[(i - 1)]) + (fSlow94 * fZec180[i]));
				fZec185[i] = ((fSlow93 * fZec184[i]) - (fSlow94 * fRec6[(i - 1)]));
				fYec64[((i + fYec64_idx) & 16383)] = ((0.707106769f * fZec183[i]) - (0.707106769f * fZec185[i]));
				fYec65[i] = fYec64[(((i + fYec64_idx) - iZec116[i]) & 16383)];
				fRec7[i] = (fYec65[(i - 1)] - ((fZec118[i] * (fRec7[(i - 1)] - fYec65[i])) / fZec119[i]));
				fRec5[i] = fRec7[i];
				fYec66[((i + fYec66_idx) & 16383)] = ((0.707106769f * fZec183[i]) + (0.707106769f * fZec185[i]));
				fYec67[i] = fYec66[(((i + fYec66_idx) - std::min<int>(8192, std::max<int>(0, int(fZec186[i])))) & 16383)];
				fRec115[i] = (fYec67[(i - 1)] - (((fZec187[i] + (2.0f - fRec116[i])) * (fRec115[(i - 1)] - fYec67[i])) / (fRec116[i] - fZec187[i])));
				fRec6[i] = fRec115[i];
				fZec188[i] = ((fSlow93 * fRec5[(i - 1)]) + (fSlow94 * fZec182[i]));
				fZec189[i] = ((fSlow93 * fZec188[i]) - (fSlow94 * fRec2[(i - 1)]));
				fZec190[i] = ((fSlow93 * fRec6[(i - 1)]) + (fSlow94 * fZec184[i]));
				fZec191[i] = ((fSlow93 * fZec190[i]) - (fSlow94 * fRec3[(i - 1)]));
				fYec68[((i + fYec68_idx) & 16383)] = ((0.707106769f * fZec189[i]) - (0.707106769f * fZec191[i]));
				fYec69[i] = fYec68[(((i + fYec68_idx) - std::min<int>(8192, std::max<int>(0, int(fZec192[i])))) & 16383)];
				fRec4[i] = (fYec69[(i - 1)] - (((fZec193[i] + (2.0f - fRec117[i])) * (fRec4[(i - 1)] - fYec69[i])) / (fRec117[i] - fZec193[i])));
				fRec2[i] = fRec4[i];
				fYec70[((i + fYec70_idx) & 16383)] = ((0.707106769f * fZec189[i]) + (0.707106769f * fZec191[i]));
				fYec71[i] = fYec70[(((i + fYec70_idx) - iZec127[i]) & 16383)];
				fRec118[i] = (fYec71[(i - 1)] - ((fZec129[i] * (fRec118[(i - 1)] - fYec71[i])) / fZec130[i]));
				fRec3[i] = fRec118[i];
				fRec0[i] = ((fSlow93 * fRec2[(i - 1)]) + (fSlow94 * fZec188[i]));
				fRec1[i] = ((fSlow93 * fRec3[(i - 1)]) + (fSlow94 * fZec190[i]));
				
			}
			/* Post code */
			fYec70_idx_save = vsize;
			for (int j299 = 0; (j299 < 4); j299 = (j299 + 1)) {
				fYec71_perm[j299] = fYec71_tmp[(vsize + j299)];
				
			}
			for (int j301 = 0; (j301 < 4); j301 = (j301 + 1)) {
				fRec118_perm[j301] = fRec118_tmp[(vsize + j301)];
				
			}
			fYec68_idx_save = vsize;
			for (int j293 = 0; (j293 < 4); j293 = (j293 + 1)) {
				fYec69_perm[j293] = fYec69_tmp[(vsize + j293)];
				
			}
			fYec66_idx_save = vsize;
			for (int j285 = 0; (j285 < 4); j285 = (j285 + 1)) {
				fYec67_perm[j285] = fYec67_tmp[(vsize + j285)];
				
			}
			for (int j287 = 0; (j287 < 4); j287 = (j287 + 1)) {
				fRec115_perm[j287] = fRec115_tmp[(vsize + j287)];
				
			}
			fYec64_idx_save = vsize;
			for (int j277 = 0; (j277 < 4); j277 = (j277 + 1)) {
				fYec65_perm[j277] = fYec65_tmp[(vsize + j277)];
				
			}
			fYec62_idx_save = vsize;
			for (int j271 = 0; (j271 < 4); j271 = (j271 + 1)) {
				fYec63_perm[j271] = fYec63_tmp[(vsize + j271)];
				
			}
			for (int j273 = 0; (j273 < 4); j273 = (j273 + 1)) {
				fRec114_perm[j273] = fRec114_tmp[(vsize + j273)];
				
			}
			fYec60_idx_save = vsize;
			for (int j265 = 0; (j265 < 4); j265 = (j265 + 1)) {
				fYec61_perm[j265] = fYec61_tmp[(vsize + j265)];
				
			}
			fYec58_idx_save = vsize;
			for (int j259 = 0; (j259 < 4); j259 = (j259 + 1)) {
				fYec59_perm[j259] = fYec59_tmp[(vsize + j259)];
				
			}
			for (int j261 = 0; (j261 < 4); j261 = (j261 + 1)) {
				fRec112_perm[j261] = fRec112_tmp[(vsize + j261)];
				
			}
			fYec56_idx_save = vsize;
			for (int j251 = 0; (j251 < 4); j251 = (j251 + 1)) {
				fYec57_perm[j251] = fYec57_tmp[(vsize + j251)];
				
			}
			fYec55_idx_save = vsize;
			for (int j243 = 0; (j243 < 4); j243 = (j243 + 1)) {
				fRec111_perm[j243] = fRec111_tmp[(vsize + j243)];
				
			}
			for (int j245 = 0; (j245 < 4); j245 = (j245 + 1)) {
				fRec110_perm[j245] = fRec110_tmp[(vsize + j245)];
				
			}
			for (int j247 = 0; (j247 < 4); j247 = (j247 + 1)) {
				fRec109_perm[j247] = fRec109_tmp[(vsize + j247)];
				
			}
			for (int j235 = 0; (j235 < 4); j235 = (j235 + 1)) {
				fYec54_perm[j235] = fYec54_tmp[(vsize + j235)];
				
			}
			for (int j229 = 0; (j229 < 4); j229 = (j229 + 1)) {
				fRec108_perm[j229] = fRec108_tmp[(vsize + j229)];
				
			}
			for (int j231 = 0; (j231 < 4); j231 = (j231 + 1)) {
				fRec107_perm[j231] = fRec107_tmp[(vsize + j231)];
				
			}
			for (int j233 = 0; (j233 < 4); j233 = (j233 + 1)) {
				fRec106_perm[j233] = fRec106_tmp[(vsize + j233)];
				
			}
			for (int j237 = 0; (j237 < 4); j237 = (j237 + 1)) {
				fRec105_perm[j237] = fRec105_tmp[(vsize + j237)];
				
			}
			for (int j239 = 0; (j239 < 4); j239 = (j239 + 1)) {
				fRec104_perm[j239] = fRec104_tmp[(vsize + j239)];
				
			}
			for (int j241 = 0; (j241 < 4); j241 = (j241 + 1)) {
				fRec103_perm[j241] = fRec103_tmp[(vsize + j241)];
				
			}
			fYec51_idx_save = vsize;
			fYec52_idx_save = vsize;
			for (int j217 = 0; (j217 < 4); j217 = (j217 + 1)) {
				fYec53_perm[j217] = fYec53_tmp[(vsize + j217)];
				
			}
			for (int j219 = 0; (j219 < 4); j219 = (j219 + 1)) {
				fRec101_perm[j219] = fRec101_tmp[(vsize + j219)];
				
			}
			for (int j221 = 0; (j221 < 4); j221 = (j221 + 1)) {
				fRec100_perm[j221] = fRec100_tmp[(vsize + j221)];
				
			}
			for (int j223 = 0; (j223 < 4); j223 = (j223 + 1)) {
				fRec99_perm[j223] = fRec99_tmp[(vsize + j223)];
				
			}
			for (int j225 = 0; (j225 < 4); j225 = (j225 + 1)) {
				fRec98_perm[j225] = fRec98_tmp[(vsize + j225)];
				
			}
			for (int j227 = 0; (j227 < 4); j227 = (j227 + 1)) {
				fRec97_perm[j227] = fRec97_tmp[(vsize + j227)];
				
			}
			for (int j249 = 0; (j249 < 4); j249 = (j249 + 1)) {
				fRec96_perm[j249] = fRec96_tmp[(vsize + j249)];
				
			}
			fYec50_idx_save = vsize;
			for (int j207 = 0; (j207 < 4); j207 = (j207 + 1)) {
				fRec95_perm[j207] = fRec95_tmp[(vsize + j207)];
				
			}
			for (int j209 = 0; (j209 < 4); j209 = (j209 + 1)) {
				fRec94_perm[j209] = fRec94_tmp[(vsize + j209)];
				
			}
			for (int j211 = 0; (j211 < 4); j211 = (j211 + 1)) {
				fRec93_perm[j211] = fRec93_tmp[(vsize + j211)];
				
			}
			for (int j199 = 0; (j199 < 4); j199 = (j199 + 1)) {
				fYec49_perm[j199] = fYec49_tmp[(vsize + j199)];
				
			}
			for (int j193 = 0; (j193 < 4); j193 = (j193 + 1)) {
				fRec92_perm[j193] = fRec92_tmp[(vsize + j193)];
				
			}
			for (int j195 = 0; (j195 < 4); j195 = (j195 + 1)) {
				fRec91_perm[j195] = fRec91_tmp[(vsize + j195)];
				
			}
			for (int j197 = 0; (j197 < 4); j197 = (j197 + 1)) {
				fRec90_perm[j197] = fRec90_tmp[(vsize + j197)];
				
			}
			for (int j201 = 0; (j201 < 4); j201 = (j201 + 1)) {
				fRec89_perm[j201] = fRec89_tmp[(vsize + j201)];
				
			}
			for (int j203 = 0; (j203 < 4); j203 = (j203 + 1)) {
				fRec88_perm[j203] = fRec88_tmp[(vsize + j203)];
				
			}
			for (int j205 = 0; (j205 < 4); j205 = (j205 + 1)) {
				fRec87_perm[j205] = fRec87_tmp[(vsize + j205)];
				
			}
			fYec46_idx_save = vsize;
			fYec47_idx_save = vsize;
			for (int j181 = 0; (j181 < 4); j181 = (j181 + 1)) {
				fYec48_perm[j181] = fYec48_tmp[(vsize + j181)];
				
			}
			fYec44_idx_save = vsize;
			for (int j173 = 0; (j173 < 4); j173 = (j173 + 1)) {
				fYec45_perm[j173] = fYec45_tmp[(vsize + j173)];
				
			}
			for (int j175 = 0; (j175 < 4); j175 = (j175 + 1)) {
				fRec84_perm[j175] = fRec84_tmp[(vsize + j175)];
				
			}
			fYec42_idx_save = vsize;
			for (int j165 = 0; (j165 < 4); j165 = (j165 + 1)) {
				fYec43_perm[j165] = fYec43_tmp[(vsize + j165)];
				
			}
			fYec40_idx_save = vsize;
			for (int j157 = 0; (j157 < 4); j157 = (j157 + 1)) {
				fYec41_perm[j157] = fYec41_tmp[(vsize + j157)];
				
			}
			for (int j159 = 0; (j159 < 4); j159 = (j159 + 1)) {
				fRec81_perm[j159] = fRec81_tmp[(vsize + j159)];
				
			}
			fYec38_idx_save = vsize;
			for (int j149 = 0; (j149 < 4); j149 = (j149 + 1)) {
				fYec39_perm[j149] = fYec39_tmp[(vsize + j149)];
				
			}
			fYec36_idx_save = vsize;
			for (int j141 = 0; (j141 < 4); j141 = (j141 + 1)) {
				fYec37_perm[j141] = fYec37_tmp[(vsize + j141)];
				
			}
			for (int j143 = 0; (j143 < 4); j143 = (j143 + 1)) {
				fRec78_perm[j143] = fRec78_tmp[(vsize + j143)];
				
			}
			fYec34_idx_save = vsize;
			for (int j133 = 0; (j133 < 4); j133 = (j133 + 1)) {
				fYec35_perm[j133] = fYec35_tmp[(vsize + j133)];
				
			}
			fYec32_idx_save = vsize;
			for (int j125 = 0; (j125 < 4); j125 = (j125 + 1)) {
				fYec33_perm[j125] = fYec33_tmp[(vsize + j125)];
				
			}
			for (int j127 = 0; (j127 < 4); j127 = (j127 + 1)) {
				fRec75_perm[j127] = fRec75_tmp[(vsize + j127)];
				
			}
			fYec30_idx_save = vsize;
			for (int j117 = 0; (j117 < 4); j117 = (j117 + 1)) {
				fYec31_perm[j117] = fYec31_tmp[(vsize + j117)];
				
			}
			fYec28_idx_save = vsize;
			for (int j109 = 0; (j109 < 4); j109 = (j109 + 1)) {
				fYec29_perm[j109] = fYec29_tmp[(vsize + j109)];
				
			}
			for (int j111 = 0; (j111 < 4); j111 = (j111 + 1)) {
				fRec72_perm[j111] = fRec72_tmp[(vsize + j111)];
				
			}
			fYec26_idx_save = vsize;
			for (int j101 = 0; (j101 < 4); j101 = (j101 + 1)) {
				fYec27_perm[j101] = fYec27_tmp[(vsize + j101)];
				
			}
			fYec23_idx_save = vsize;
			fYec24_idx_save = vsize;
			for (int j95 = 0; (j95 < 4); j95 = (j95 + 1)) {
				fYec25_perm[j95] = fYec25_tmp[(vsize + j95)];
				
			}
			for (int j97 = 0; (j97 < 4); j97 = (j97 + 1)) {
				fRec69_perm[j97] = fRec69_tmp[(vsize + j97)];
				
			}
			fYec20_idx_save = vsize;
			fYec21_idx_save = vsize;
			for (int j89 = 0; (j89 < 4); j89 = (j89 + 1)) {
				fYec22_perm[j89] = fYec22_tmp[(vsize + j89)];
				
			}
			fYec18_idx_save = vsize;
			for (int j81 = 0; (j81 < 4); j81 = (j81 + 1)) {
				fYec19_perm[j81] = fYec19_tmp[(vsize + j81)];
				
			}
			for (int j83 = 0; (j83 < 4); j83 = (j83 + 1)) {
				fRec66_perm[j83] = fRec66_tmp[(vsize + j83)];
				
			}
			fYec16_idx_save = vsize;
			for (int j73 = 0; (j73 < 4); j73 = (j73 + 1)) {
				fYec17_perm[j73] = fYec17_tmp[(vsize + j73)];
				
			}
			fYec14_idx_save = vsize;
			for (int j65 = 0; (j65 < 4); j65 = (j65 + 1)) {
				fYec15_perm[j65] = fYec15_tmp[(vsize + j65)];
				
			}
			for (int j67 = 0; (j67 < 4); j67 = (j67 + 1)) {
				fRec63_perm[j67] = fRec63_tmp[(vsize + j67)];
				
			}
			fYec12_idx_save = vsize;
			for (int j57 = 0; (j57 < 4); j57 = (j57 + 1)) {
				fYec13_perm[j57] = fYec13_tmp[(vsize + j57)];
				
			}
			fYec10_idx_save = vsize;
			for (int j49 = 0; (j49 < 4); j49 = (j49 + 1)) {
				fYec11_perm[j49] = fYec11_tmp[(vsize + j49)];
				
			}
			for (int j51 = 0; (j51 < 4); j51 = (j51 + 1)) {
				fRec60_perm[j51] = fRec60_tmp[(vsize + j51)];
				
			}
			fYec8_idx_save = vsize;
			for (int j41 = 0; (j41 < 4); j41 = (j41 + 1)) {
				fYec9_perm[j41] = fYec9_tmp[(vsize + j41)];
				
			}
			fYec6_idx_save = vsize;
			for (int j33 = 0; (j33 < 4); j33 = (j33 + 1)) {
				fYec7_perm[j33] = fYec7_tmp[(vsize + j33)];
				
			}
			for (int j35 = 0; (j35 < 4); j35 = (j35 + 1)) {
				fRec57_perm[j35] = fRec57_tmp[(vsize + j35)];
				
			}
			fYec4_idx_save = vsize;
			for (int j25 = 0; (j25 < 4); j25 = (j25 + 1)) {
				fYec5_perm[j25] = fYec5_tmp[(vsize + j25)];
				
			}
			fYec2_idx_save = vsize;
			for (int j17 = 0; (j17 < 4); j17 = (j17 + 1)) {
				fYec3_perm[j17] = fYec3_tmp[(vsize + j17)];
				
			}
			for (int j19 = 0; (j19 < 4); j19 = (j19 + 1)) {
				fRec54_perm[j19] = fRec54_tmp[(vsize + j19)];
				
			}
			fYec0_idx_save = vsize;
			for (int j9 = 0; (j9 < 4); j9 = (j9 + 1)) {
				fYec1_perm[j9] = fYec1_tmp[(vsize + j9)];
				
			}
			for (int j11 = 0; (j11 < 4); j11 = (j11 + 1)) {
				fRec52_perm[j11] = fRec52_tmp[(vsize + j11)];
				
			}
			for (int j13 = 0; (j13 < 4); j13 = (j13 + 1)) {
				fRec50_perm[j13] = fRec50_tmp[(vsize + j13)];
				
			}
			for (int j21 = 0; (j21 < 4); j21 = (j21 + 1)) {
				fRec51_perm[j21] = fRec51_tmp[(vsize + j21)];
				
			}
			for (int j27 = 0; (j27 < 4); j27 = (j27 + 1)) {
				fRec49_perm[j27] = fRec49_tmp[(vsize + j27)];
				
			}
			for (int j29 = 0; (j29 < 4); j29 = (j29 + 1)) {
				fRec47_perm[j29] = fRec47_tmp[(vsize + j29)];
				
			}
			for (int j37 = 0; (j37 < 4); j37 = (j37 + 1)) {
				fRec48_perm[j37] = fRec48_tmp[(vsize + j37)];
				
			}
			for (int j43 = 0; (j43 < 4); j43 = (j43 + 1)) {
				fRec46_perm[j43] = fRec46_tmp[(vsize + j43)];
				
			}
			for (int j45 = 0; (j45 < 4); j45 = (j45 + 1)) {
				fRec44_perm[j45] = fRec44_tmp[(vsize + j45)];
				
			}
			for (int j53 = 0; (j53 < 4); j53 = (j53 + 1)) {
				fRec45_perm[j53] = fRec45_tmp[(vsize + j53)];
				
			}
			for (int j59 = 0; (j59 < 4); j59 = (j59 + 1)) {
				fRec43_perm[j59] = fRec43_tmp[(vsize + j59)];
				
			}
			for (int j61 = 0; (j61 < 4); j61 = (j61 + 1)) {
				fRec41_perm[j61] = fRec41_tmp[(vsize + j61)];
				
			}
			for (int j69 = 0; (j69 < 4); j69 = (j69 + 1)) {
				fRec42_perm[j69] = fRec42_tmp[(vsize + j69)];
				
			}
			for (int j75 = 0; (j75 < 4); j75 = (j75 + 1)) {
				fRec40_perm[j75] = fRec40_tmp[(vsize + j75)];
				
			}
			for (int j77 = 0; (j77 < 4); j77 = (j77 + 1)) {
				fRec38_perm[j77] = fRec38_tmp[(vsize + j77)];
				
			}
			for (int j85 = 0; (j85 < 4); j85 = (j85 + 1)) {
				fRec39_perm[j85] = fRec39_tmp[(vsize + j85)];
				
			}
			for (int j91 = 0; (j91 < 4); j91 = (j91 + 1)) {
				fRec37_perm[j91] = fRec37_tmp[(vsize + j91)];
				
			}
			for (int j103 = 0; (j103 < 4); j103 = (j103 + 1)) {
				fRec36_perm[j103] = fRec36_tmp[(vsize + j103)];
				
			}
			for (int j105 = 0; (j105 < 4); j105 = (j105 + 1)) {
				fRec34_perm[j105] = fRec34_tmp[(vsize + j105)];
				
			}
			for (int j113 = 0; (j113 < 4); j113 = (j113 + 1)) {
				fRec35_perm[j113] = fRec35_tmp[(vsize + j113)];
				
			}
			for (int j119 = 0; (j119 < 4); j119 = (j119 + 1)) {
				fRec33_perm[j119] = fRec33_tmp[(vsize + j119)];
				
			}
			for (int j121 = 0; (j121 < 4); j121 = (j121 + 1)) {
				fRec31_perm[j121] = fRec31_tmp[(vsize + j121)];
				
			}
			for (int j129 = 0; (j129 < 4); j129 = (j129 + 1)) {
				fRec32_perm[j129] = fRec32_tmp[(vsize + j129)];
				
			}
			for (int j135 = 0; (j135 < 4); j135 = (j135 + 1)) {
				fRec30_perm[j135] = fRec30_tmp[(vsize + j135)];
				
			}
			for (int j137 = 0; (j137 < 4); j137 = (j137 + 1)) {
				fRec28_perm[j137] = fRec28_tmp[(vsize + j137)];
				
			}
			for (int j145 = 0; (j145 < 4); j145 = (j145 + 1)) {
				fRec29_perm[j145] = fRec29_tmp[(vsize + j145)];
				
			}
			for (int j151 = 0; (j151 < 4); j151 = (j151 + 1)) {
				fRec27_perm[j151] = fRec27_tmp[(vsize + j151)];
				
			}
			for (int j153 = 0; (j153 < 4); j153 = (j153 + 1)) {
				fRec25_perm[j153] = fRec25_tmp[(vsize + j153)];
				
			}
			for (int j161 = 0; (j161 < 4); j161 = (j161 + 1)) {
				fRec26_perm[j161] = fRec26_tmp[(vsize + j161)];
				
			}
			for (int j167 = 0; (j167 < 4); j167 = (j167 + 1)) {
				fRec24_perm[j167] = fRec24_tmp[(vsize + j167)];
				
			}
			for (int j169 = 0; (j169 < 4); j169 = (j169 + 1)) {
				fRec22_perm[j169] = fRec22_tmp[(vsize + j169)];
				
			}
			for (int j177 = 0; (j177 < 4); j177 = (j177 + 1)) {
				fRec23_perm[j177] = fRec23_tmp[(vsize + j177)];
				
			}
			for (int j183 = 0; (j183 < 4); j183 = (j183 + 1)) {
				fRec21_perm[j183] = fRec21_tmp[(vsize + j183)];
				
			}
			for (int j185 = 0; (j185 < 4); j185 = (j185 + 1)) {
				fRec20_perm[j185] = fRec20_tmp[(vsize + j185)];
				
			}
			for (int j187 = 0; (j187 < 4); j187 = (j187 + 1)) {
				fRec19_perm[j187] = fRec19_tmp[(vsize + j187)];
				
			}
			for (int j189 = 0; (j189 < 4); j189 = (j189 + 1)) {
				fRec18_perm[j189] = fRec18_tmp[(vsize + j189)];
				
			}
			for (int j191 = 0; (j191 < 4); j191 = (j191 + 1)) {
				fRec17_perm[j191] = fRec17_tmp[(vsize + j191)];
				
			}
			for (int j213 = 0; (j213 < 4); j213 = (j213 + 1)) {
				fRec14_perm[j213] = fRec14_tmp[(vsize + j213)];
				
			}
			for (int j253 = 0; (j253 < 4); j253 = (j253 + 1)) {
				fRec13_perm[j253] = fRec13_tmp[(vsize + j253)];
				
			}
			for (int j255 = 0; (j255 < 4); j255 = (j255 + 1)) {
				fRec11_perm[j255] = fRec11_tmp[(vsize + j255)];
				
			}
			for (int j263 = 0; (j263 < 4); j263 = (j263 + 1)) {
				fRec12_perm[j263] = fRec12_tmp[(vsize + j263)];
				
			}
			for (int j267 = 0; (j267 < 4); j267 = (j267 + 1)) {
				fRec10_perm[j267] = fRec10_tmp[(vsize + j267)];
				
			}
			for (int j269 = 0; (j269 < 4); j269 = (j269 + 1)) {
				fRec8_perm[j269] = fRec8_tmp[(vsize + j269)];
				
			}
			for (int j275 = 0; (j275 < 4); j275 = (j275 + 1)) {
				fRec9_perm[j275] = fRec9_tmp[(vsize + j275)];
				
			}
			for (int j279 = 0; (j279 < 4); j279 = (j279 + 1)) {
				fRec7_perm[j279] = fRec7_tmp[(vsize + j279)];
				
			}
			for (int j281 = 0; (j281 < 4); j281 = (j281 + 1)) {
				fRec5_perm[j281] = fRec5_tmp[(vsize + j281)];
				
			}
			for (int j289 = 0; (j289 < 4); j289 = (j289 + 1)) {
				fRec6_perm[j289] = fRec6_tmp[(vsize + j289)];
				
			}
			for (int j295 = 0; (j295 < 4); j295 = (j295 + 1)) {
				fRec4_perm[j295] = fRec4_tmp[(vsize + j295)];
				
			}
			for (int j297 = 0; (j297 < 4); j297 = (j297 + 1)) {
				fRec2_perm[j297] = fRec2_tmp[(vsize + j297)];
				
			}
			for (int j303 = 0; (j303 < 4); j303 = (j303 + 1)) {
				fRec3_perm[j303] = fRec3_tmp[(vsize + j303)];
				
			}
			for (int j305 = 0; (j305 < 4); j305 = (j305 + 1)) {
				fRec0_perm[j305] = fRec0_tmp[(vsize + j305)];
				
			}
			for (int j307 = 0; (j307 < 4); j307 = (j307 + 1)) {
				fRec1_perm[j307] = fRec1_tmp[(vsize + j307)];
				
			}
			/* Vectorizable loop 170 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				output0[i] = FAUSTFLOAT(fRec0[i]);
				
			}
			/* Vectorizable loop 171 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				output1[i] = FAUSTFLOAT(fRec1[i]);
				
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
			for (int j2 = 0; (j2 < 4); j2 = (j2 + 1)) {
				iVec0_tmp[j2] = iVec0_perm[j2];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iVec0[i] = 1;
				
			}
			/* Post code */
			for (int j3 = 0; (j3 < 4); j3 = (j3 + 1)) {
				iVec0_perm[j3] = iVec0_tmp[(vsize + j3)];
				
			}
			/* Vectorizable loop 1 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec0[i] = (1 - iVec0[(i - 1)]);
				
			}
			/* Recursive loop 2 */
			/* Pre code */
			for (int j0 = 0; (j0 < 4); j0 = (j0 + 1)) {
				fRec15_tmp[j0] = fRec15_perm[j0];
				
			}
			for (int j4 = 0; (j4 < 4); j4 = (j4 + 1)) {
				fRec16_tmp[j4] = fRec16_perm[j4];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec15[i] = ((fSlow1 * fRec16[(i - 1)]) + (fSlow2 * fRec15[(i - 1)]));
				fRec16[i] = ((float(iZec0[i]) + (fSlow2 * fRec16[(i - 1)])) - (fSlow1 * fRec15[(i - 1)]));
				
			}
			/* Post code */
			for (int j1 = 0; (j1 < 4); j1 = (j1 + 1)) {
				fRec15_perm[j1] = fRec15_tmp[(vsize + j1)];
				
			}
			for (int j5 = 0; (j5 < 4); j5 = (j5 + 1)) {
				fRec16_perm[j5] = fRec16_tmp[(vsize + j5)];
				
			}
			/* Vectorizable loop 3 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec39[i] = (fSlow26 * (fRec16[i] + 1.0f));
				
			}
			/* Vectorizable loop 4 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec131[i] = (fSlow26 * (fRec15[i] + 1.0f));
				
			}
			/* Vectorizable loop 5 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec40[i] = (fZec39[i] + 3.50000501f);
				
			}
			/* Vectorizable loop 6 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec67[i] = (fSlow29 * fRec16[i]);
				
			}
			/* Vectorizable loop 7 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec132[i] = (fZec131[i] + 3.50000501f);
				
			}
			/* Vectorizable loop 8 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec161[i] = (fSlow29 * fRec15[i]);
				
			}
			/* Vectorizable loop 9 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec43[i] = std::floor(fZec40[i]);
				
			}
			/* Vectorizable loop 10 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec68[i] = (fSlow26 + (fZec67[i] + 3.50000501f));
				
			}
			/* Recursive loop 11 */
			/* Pre code */
			for (int j122 = 0; (j122 < 4); j122 = (j122 + 1)) {
				fRec76_tmp[j122] = fRec76_perm[j122];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec76[i] = (fSlow37 + (0.999899983f * (fRec76[(i - 1)] + float((iSlow36 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j123 = 0; (j123 < 4); j123 = (j123 + 1)) {
				fRec76_perm[j123] = fRec76_tmp[(vsize + j123)];
				
			}
			/* Recursive loop 12 */
			/* Pre code */
			for (int j138 = 0; (j138 < 4); j138 = (j138 + 1)) {
				fRec79_tmp[j138] = fRec79_perm[j138];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec79[i] = (fSlow41 + (0.999899983f * (fRec79[(i - 1)] + float((iSlow40 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j139 = 0; (j139 < 4); j139 = (j139 + 1)) {
				fRec79_perm[j139] = fRec79_tmp[(vsize + j139)];
				
			}
			/* Recursive loop 13 */
			/* Pre code */
			for (int j146 = 0; (j146 < 4); j146 = (j146 + 1)) {
				fRec80_tmp[j146] = fRec80_perm[j146];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec80[i] = (fSlow43 + (0.999899983f * (fRec80[(i - 1)] + float((iSlow42 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j147 = 0; (j147 < 4); j147 = (j147 + 1)) {
				fRec80_perm[j147] = fRec80_tmp[(vsize + j147)];
				
			}
			/* Recursive loop 14 */
			/* Pre code */
			for (int j154 = 0; (j154 < 4); j154 = (j154 + 1)) {
				fRec82_tmp[j154] = fRec82_perm[j154];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec82[i] = (fSlow45 + (0.999899983f * (fRec82[(i - 1)] + float((iSlow44 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j155 = 0; (j155 < 4); j155 = (j155 + 1)) {
				fRec82_perm[j155] = fRec82_tmp[(vsize + j155)];
				
			}
			/* Recursive loop 15 */
			/* Pre code */
			for (int j170 = 0; (j170 < 4); j170 = (j170 + 1)) {
				fRec85_tmp[j170] = fRec85_perm[j170];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec85[i] = (fSlow49 + (0.999899983f * (fRec85[(i - 1)] + float((iSlow48 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j171 = 0; (j171 < 4); j171 = (j171 + 1)) {
				fRec85_perm[j171] = fRec85_tmp[(vsize + j171)];
				
			}
			/* Vectorizable loop 16 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec135[i] = std::floor(fZec132[i]);
				
			}
			/* Vectorizable loop 17 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec162[i] = (fSlow26 + (fZec161[i] + 3.50000501f));
				
			}
			/* Recursive loop 18 */
			/* Pre code */
			for (int j6 = 0; (j6 < 4); j6 = (j6 + 1)) {
				fRec53_tmp[j6] = fRec53_perm[j6];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec53[i] = (fSlow5 + (0.999899983f * (fRec53[(i - 1)] + float((iSlow4 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j7 = 0; (j7 < 4); j7 = (j7 + 1)) {
				fRec53_perm[j7] = fRec53_tmp[(vsize + j7)];
				
			}
			/* Recursive loop 19 */
			/* Pre code */
			for (int j22 = 0; (j22 < 4); j22 = (j22 + 1)) {
				fRec56_tmp[j22] = fRec56_perm[j22];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec56[i] = (fSlow9 + (0.999899983f * (fRec56[(i - 1)] + float((iSlow8 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j23 = 0; (j23 < 4); j23 = (j23 + 1)) {
				fRec56_perm[j23] = fRec56_tmp[(vsize + j23)];
				
			}
			/* Recursive loop 20 */
			/* Pre code */
			for (int j14 = 0; (j14 < 4); j14 = (j14 + 1)) {
				fRec55_tmp[j14] = fRec55_perm[j14];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec55[i] = (fSlow7 + (0.999899983f * (fRec55[(i - 1)] + float((iSlow6 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j15 = 0; (j15 < 4); j15 = (j15 + 1)) {
				fRec55_perm[j15] = fRec55_tmp[(vsize + j15)];
				
			}
			/* Recursive loop 21 */
			/* Pre code */
			for (int j38 = 0; (j38 < 4); j38 = (j38 + 1)) {
				fRec59_tmp[j38] = fRec59_perm[j38];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec59[i] = (fSlow13 + (0.999899983f * (fRec59[(i - 1)] + float((iSlow12 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j39 = 0; (j39 < 4); j39 = (j39 + 1)) {
				fRec59_perm[j39] = fRec59_tmp[(vsize + j39)];
				
			}
			/* Recursive loop 22 */
			/* Pre code */
			for (int j30 = 0; (j30 < 4); j30 = (j30 + 1)) {
				fRec58_tmp[j30] = fRec58_perm[j30];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec58[i] = (fSlow11 + (0.999899983f * (fRec58[(i - 1)] + float((iSlow10 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j31 = 0; (j31 < 4); j31 = (j31 + 1)) {
				fRec58_perm[j31] = fRec58_tmp[(vsize + j31)];
				
			}
			/* Recursive loop 23 */
			/* Pre code */
			for (int j46 = 0; (j46 < 4); j46 = (j46 + 1)) {
				fRec61_tmp[j46] = fRec61_perm[j46];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec61[i] = (fSlow15 + (0.999899983f * (fRec61[(i - 1)] + float((iSlow14 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j47 = 0; (j47 < 4); j47 = (j47 + 1)) {
				fRec61_perm[j47] = fRec61_tmp[(vsize + j47)];
				
			}
			/* Recursive loop 24 */
			/* Pre code */
			for (int j54 = 0; (j54 < 4); j54 = (j54 + 1)) {
				fRec62_tmp[j54] = fRec62_perm[j54];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec62[i] = (fSlow17 + (0.999899983f * (fRec62[(i - 1)] + float((iSlow16 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j55 = 0; (j55 < 4); j55 = (j55 + 1)) {
				fRec62_perm[j55] = fRec62_tmp[(vsize + j55)];
				
			}
			/* Recursive loop 25 */
			/* Pre code */
			for (int j62 = 0; (j62 < 4); j62 = (j62 + 1)) {
				fRec64_tmp[j62] = fRec64_perm[j62];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec64[i] = (fSlow19 + (0.999899983f * (fRec64[(i - 1)] + float((iSlow18 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j63 = 0; (j63 < 4); j63 = (j63 + 1)) {
				fRec64_perm[j63] = fRec64_tmp[(vsize + j63)];
				
			}
			/* Recursive loop 26 */
			/* Pre code */
			for (int j70 = 0; (j70 < 4); j70 = (j70 + 1)) {
				fRec65_tmp[j70] = fRec65_perm[j70];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec65[i] = (fSlow21 + (0.999899983f * (fRec65[(i - 1)] + float((iSlow20 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j71 = 0; (j71 < 4); j71 = (j71 + 1)) {
				fRec65_perm[j71] = fRec65_tmp[(vsize + j71)];
				
			}
			/* Recursive loop 27 */
			/* Pre code */
			for (int j78 = 0; (j78 < 4); j78 = (j78 + 1)) {
				fRec67_tmp[j78] = fRec67_perm[j78];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec67[i] = (fSlow23 + (0.999899983f * (fRec67[(i - 1)] + float((iSlow22 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j79 = 0; (j79 < 4); j79 = (j79 + 1)) {
				fRec67_perm[j79] = fRec67_tmp[(vsize + j79)];
				
			}
			/* Recursive loop 28 */
			/* Pre code */
			for (int j86 = 0; (j86 < 4); j86 = (j86 + 1)) {
				fRec68_tmp[j86] = fRec68_perm[j86];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec68[i] = (fSlow25 + (0.995000005f * (fRec68[(i - 1)] + float((iSlow24 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j87 = 0; (j87 < 4); j87 = (j87 + 1)) {
				fRec68_perm[j87] = fRec68_tmp[(vsize + j87)];
				
			}
			/* Vectorizable loop 29 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec44[i] = (fZec39[i] + (4.0f - fZec43[i]));
				
			}
			/* Vectorizable loop 30 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec46[i] = (fZec39[i] + (3.0f - fZec43[i]));
				
			}
			/* Recursive loop 31 */
			/* Pre code */
			for (int j92 = 0; (j92 < 4); j92 = (j92 + 1)) {
				fRec70_tmp[j92] = fRec70_perm[j92];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec70[i] = (fSlow28 + (0.995000005f * (fRec70[(i - 1)] + float((iSlow27 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j93 = 0; (j93 < 4); j93 = (j93 + 1)) {
				fRec70_perm[j93] = fRec70_tmp[(vsize + j93)];
				
			}
			/* Vectorizable loop 32 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec70[i] = std::floor(fZec68[i]);
				
			}
			/* Recursive loop 33 */
			/* Pre code */
			for (int j98 = 0; (j98 < 4); j98 = (j98 + 1)) {
				fRec71_tmp[j98] = fRec71_perm[j98];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec71[i] = (fSlow31 + (0.999899983f * (fRec71[(i - 1)] + float((iSlow30 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j99 = 0; (j99 < 4); j99 = (j99 + 1)) {
				fRec71_perm[j99] = fRec71_tmp[(vsize + j99)];
				
			}
			/* Recursive loop 34 */
			/* Pre code */
			for (int j106 = 0; (j106 < 4); j106 = (j106 + 1)) {
				fRec73_tmp[j106] = fRec73_perm[j106];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec73[i] = (fSlow33 + (0.999899983f * (fRec73[(i - 1)] + float((iSlow32 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j107 = 0; (j107 < 4); j107 = (j107 + 1)) {
				fRec73_perm[j107] = fRec73_tmp[(vsize + j107)];
				
			}
			/* Recursive loop 35 */
			/* Pre code */
			for (int j114 = 0; (j114 < 4); j114 = (j114 + 1)) {
				fRec74_tmp[j114] = fRec74_perm[j114];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec74[i] = (fSlow35 + (0.999899983f * (fRec74[(i - 1)] + float((iSlow34 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j115 = 0; (j115 < 4); j115 = (j115 + 1)) {
				fRec74_perm[j115] = fRec74_tmp[(vsize + j115)];
				
			}
			/* Vectorizable loop 36 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec90[i] = (fRec76[i] + -1.49998999f);
				
			}
			/* Recursive loop 37 */
			/* Pre code */
			for (int j130 = 0; (j130 < 4); j130 = (j130 + 1)) {
				fRec77_tmp[j130] = fRec77_perm[j130];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec77[i] = (fSlow39 + (0.999899983f * (fRec77[(i - 1)] + float((iSlow38 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j131 = 0; (j131 < 4); j131 = (j131 + 1)) {
				fRec77_perm[j131] = fRec77_tmp[(vsize + j131)];
				
			}
			/* Vectorizable loop 38 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec101[i] = (fRec79[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 39 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec110[i] = (fRec80[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 40 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec115[i] = (fRec82[i] + -1.49998999f);
				
			}
			/* Recursive loop 41 */
			/* Pre code */
			for (int j162 = 0; (j162 < 4); j162 = (j162 + 1)) {
				fRec83_tmp[j162] = fRec83_perm[j162];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec83[i] = (fSlow47 + (0.999899983f * (fRec83[(i - 1)] + float((iSlow46 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j163 = 0; (j163 < 4); j163 = (j163 + 1)) {
				fRec83_perm[j163] = fRec83_tmp[(vsize + j163)];
				
			}
			/* Vectorizable loop 42 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec126[i] = (fRec85[i] + -1.49998999f);
				
			}
			/* Recursive loop 43 */
			/* Pre code */
			for (int j178 = 0; (j178 < 4); j178 = (j178 + 1)) {
				fRec86_tmp[j178] = fRec86_perm[j178];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec86[i] = (fSlow51 + (0.995000005f * (fRec86[(i - 1)] + float((iSlow50 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j179 = 0; (j179 < 4); j179 = (j179 + 1)) {
				fRec86_perm[j179] = fRec86_tmp[(vsize + j179)];
				
			}
			/* Vectorizable loop 44 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec136[i] = (fZec131[i] + (4.0f - fZec135[i]));
				
			}
			/* Vectorizable loop 45 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec138[i] = (fZec131[i] + (3.0f - fZec135[i]));
				
			}
			/* Recursive loop 46 */
			/* Pre code */
			for (int j214 = 0; (j214 < 4); j214 = (j214 + 1)) {
				fRec102_tmp[j214] = fRec102_perm[j214];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec102[i] = (fSlow91 + (0.995000005f * (fRec102[(i - 1)] + float((iSlow90 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j215 = 0; (j215 < 4); j215 = (j215 + 1)) {
				fRec102_perm[j215] = fRec102_tmp[(vsize + j215)];
				
			}
			/* Vectorizable loop 47 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec164[i] = std::floor(fZec162[i]);
				
			}
			/* Recursive loop 48 */
			/* Pre code */
			for (int j256 = 0; (j256 < 4); j256 = (j256 + 1)) {
				fRec113_tmp[j256] = fRec113_perm[j256];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec113[i] = (fSlow96 + (0.999899983f * (fRec113[(i - 1)] + float((iSlow95 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j257 = 0; (j257 < 4); j257 = (j257 + 1)) {
				fRec113_perm[j257] = fRec113_tmp[(vsize + j257)];
				
			}
			/* Recursive loop 49 */
			/* Pre code */
			for (int j282 = 0; (j282 < 4); j282 = (j282 + 1)) {
				fRec116_tmp[j282] = fRec116_perm[j282];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec116[i] = (fSlow98 + (0.999899983f * (fRec116[(i - 1)] + float((iSlow97 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j283 = 0; (j283 < 4); j283 = (j283 + 1)) {
				fRec116_perm[j283] = fRec116_tmp[(vsize + j283)];
				
			}
			/* Recursive loop 50 */
			/* Pre code */
			for (int j290 = 0; (j290 < 4); j290 = (j290 + 1)) {
				fRec117_tmp[j290] = fRec117_perm[j290];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fRec117[i] = (fSlow100 + (0.999899983f * (fRec117[(i - 1)] + float((iSlow99 * iZec0[i])))));
				
			}
			/* Post code */
			for (int j291 = 0; (j291 < 4); j291 = (j291 + 1)) {
				fRec117_perm[j291] = fRec117_tmp[(vsize + j291)];
				
			}
			/* Vectorizable loop 51 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec3[i] = (fRec53[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 52 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec5[i] = (fRec55[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 53 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec13[i] = (fRec58[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 54 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec11[i] = (fRec56[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 55 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec19[i] = (fRec59[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 56 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec21[i] = (fRec61[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 57 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec27[i] = (fRec62[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 58 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec29[i] = (fRec64[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 59 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec35[i] = (fRec65[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 60 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec37[i] = (fRec67[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 61 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec41[i] = int(fZec40[i]);
				
			}
			/* Vectorizable loop 62 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec48[i] = (fZec39[i] + (2.0f - fZec43[i]));
				
			}
			/* Vectorizable loop 63 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec50[i] = (fZec39[i] + (1.0f - fZec43[i]));
				
			}
			/* Vectorizable loop 64 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec60[i] = (fZec44[i] * fZec46[i]);
				
			}
			/* Vectorizable loop 65 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec65[i] = (fRec68[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 66 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec71[i] = (fSlow26 + (fZec67[i] + (4.0f - fZec70[i])));
				
			}
			/* Vectorizable loop 67 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec72[i] = (fSlow26 + (fZec67[i] + (3.0f - fZec70[i])));
				
			}
			/* Vectorizable loop 68 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec76[i] = (fRec70[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 69 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec80[i] = (fRec71[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 70 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec82[i] = (fRec73[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 71 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec88[i] = (fRec74[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 72 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec92[i] = std::floor(fZec90[i]);
				
			}
			/* Vectorizable loop 73 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec99[i] = (fRec77[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 74 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec103[i] = std::floor(fZec101[i]);
				
			}
			/* Vectorizable loop 75 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec112[i] = std::floor(fZec110[i]);
				
			}
			/* Vectorizable loop 76 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec117[i] = std::floor(fZec115[i]);
				
			}
			/* Vectorizable loop 77 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec124[i] = (fRec83[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 78 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec128[i] = std::floor(fZec126[i]);
				
			}
			/* Vectorizable loop 79 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec133[i] = int(fZec132[i]);
				
			}
			/* Vectorizable loop 80 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec140[i] = (fZec131[i] + (2.0f - fZec135[i]));
				
			}
			/* Vectorizable loop 81 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec142[i] = (fZec131[i] + (1.0f - fZec135[i]));
				
			}
			/* Vectorizable loop 82 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec152[i] = (fZec136[i] * fZec138[i]);
				
			}
			/* Vectorizable loop 83 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec157[i] = (fRec86[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 84 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec165[i] = (fSlow26 + (fZec161[i] + (4.0f - fZec164[i])));
				
			}
			/* Vectorizable loop 85 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec166[i] = (fSlow26 + (fZec161[i] + (3.0f - fZec164[i])));
				
			}
			/* Vectorizable loop 86 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec170[i] = (fRec102[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 87 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec176[i] = (fRec113[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 88 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec186[i] = (fRec116[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 89 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec192[i] = (fRec117[i] + -1.49998999f);
				
			}
			/* Vectorizable loop 90 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec4[i] = std::floor(fZec3[i]);
				
			}
			/* Vectorizable loop 91 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec6[i] = std::floor(fZec5[i]);
				
			}
			/* Vectorizable loop 92 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec14[i] = std::floor(fZec13[i]);
				
			}
			/* Vectorizable loop 93 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec12[i] = std::floor(fZec11[i]);
				
			}
			/* Vectorizable loop 94 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec20[i] = std::floor(fZec19[i]);
				
			}
			/* Vectorizable loop 95 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec22[i] = std::floor(fZec21[i]);
				
			}
			/* Vectorizable loop 96 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec28[i] = std::floor(fZec27[i]);
				
			}
			/* Vectorizable loop 97 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec30[i] = std::floor(fZec29[i]);
				
			}
			/* Vectorizable loop 98 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec36[i] = std::floor(fZec35[i]);
				
			}
			/* Vectorizable loop 99 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec38[i] = std::floor(fZec37[i]);
				
			}
			/* Vectorizable loop 100 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec42[i] = std::min<int>(512, std::max<int>(0, iZec41[i]));
				
			}
			/* Vectorizable loop 101 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec45[i] = (0.0f - fZec44[i]);
				
			}
			/* Vectorizable loop 102 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec47[i] = (0.0f - (0.5f * fZec46[i]));
				
			}
			/* Vectorizable loop 103 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec49[i] = (0.0f - (0.333333343f * fZec48[i]));
				
			}
			/* Vectorizable loop 104 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec51[i] = (0.0f - (0.25f * fZec50[i]));
				
			}
			/* Vectorizable loop 105 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec52[i] = (fZec39[i] + (5.0f - fZec43[i]));
				
			}
			/* Vectorizable loop 106 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec53[i] = std::min<int>(512, std::max<int>(0, (iZec41[i] + 1)));
				
			}
			/* Vectorizable loop 107 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec54[i] = (0.0f - fZec46[i]);
				
			}
			/* Vectorizable loop 108 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec55[i] = (0.0f - (0.5f * fZec48[i]));
				
			}
			/* Vectorizable loop 109 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec56[i] = (0.0f - (0.333333343f * fZec50[i]));
				
			}
			/* Vectorizable loop 110 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec57[i] = std::min<int>(512, std::max<int>(0, (iZec41[i] + 2)));
				
			}
			/* Vectorizable loop 111 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec58[i] = (0.0f - fZec48[i]);
				
			}
			/* Vectorizable loop 112 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec59[i] = (0.0f - (0.5f * fZec50[i]));
				
			}
			/* Vectorizable loop 113 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec61[i] = std::min<int>(512, std::max<int>(0, (iZec41[i] + 3)));
				
			}
			/* Vectorizable loop 114 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec62[i] = (0.0f - fZec50[i]);
				
			}
			/* Vectorizable loop 115 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec63[i] = (fZec60[i] * fZec48[i]);
				
			}
			/* Vectorizable loop 116 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec64[i] = std::min<int>(512, std::max<int>(0, (iZec41[i] + 4)));
				
			}
			/* Vectorizable loop 117 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec66[i] = std::floor(fZec65[i]);
				
			}
			/* Vectorizable loop 118 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec69[i] = int(fZec68[i]);
				
			}
			/* Vectorizable loop 119 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec73[i] = (fSlow26 + (fZec67[i] + (2.0f - fZec70[i])));
				
			}
			/* Vectorizable loop 120 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec74[i] = (fSlow26 + (fZec67[i] + (1.0f - fZec70[i])));
				
			}
			/* Vectorizable loop 121 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec75[i] = (fZec71[i] * fZec72[i]);
				
			}
			/* Vectorizable loop 122 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec77[i] = std::floor(fZec76[i]);
				
			}
			/* Vectorizable loop 123 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec81[i] = std::floor(fZec80[i]);
				
			}
			/* Vectorizable loop 124 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec83[i] = std::floor(fZec82[i]);
				
			}
			/* Vectorizable loop 125 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec89[i] = std::floor(fZec88[i]);
				
			}
			/* Vectorizable loop 126 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec91[i] = std::min<int>(8192, std::max<int>(0, int(fZec90[i])));
				
			}
			/* Vectorizable loop 127 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec93[i] = (fZec92[i] + (2.0f - fRec76[i]));
				
			}
			/* Vectorizable loop 128 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec94[i] = (fRec76[i] - fZec92[i]);
				
			}
			/* Vectorizable loop 129 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec100[i] = std::floor(fZec99[i]);
				
			}
			/* Vectorizable loop 130 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec102[i] = std::min<int>(8192, std::max<int>(0, int(fZec101[i])));
				
			}
			/* Vectorizable loop 131 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec104[i] = (fZec103[i] + (2.0f - fRec79[i]));
				
			}
			/* Vectorizable loop 132 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec105[i] = (fRec79[i] - fZec103[i]);
				
			}
			/* Vectorizable loop 133 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec111[i] = std::min<int>(8192, std::max<int>(0, int(fZec110[i])));
				
			}
			/* Vectorizable loop 134 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec113[i] = (fZec112[i] + (2.0f - fRec80[i]));
				
			}
			/* Vectorizable loop 135 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec114[i] = (fRec80[i] - fZec112[i]);
				
			}
			/* Vectorizable loop 136 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec116[i] = std::min<int>(8192, std::max<int>(0, int(fZec115[i])));
				
			}
			/* Vectorizable loop 137 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec118[i] = (fZec117[i] + (2.0f - fRec82[i]));
				
			}
			/* Vectorizable loop 138 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec119[i] = (fRec82[i] - fZec117[i]);
				
			}
			/* Vectorizable loop 139 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec125[i] = std::floor(fZec124[i]);
				
			}
			/* Vectorizable loop 140 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec127[i] = std::min<int>(8192, std::max<int>(0, int(fZec126[i])));
				
			}
			/* Vectorizable loop 141 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec129[i] = (fZec128[i] + (2.0f - fRec85[i]));
				
			}
			/* Vectorizable loop 142 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec130[i] = (fRec85[i] - fZec128[i]);
				
			}
			/* Vectorizable loop 143 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec134[i] = std::max<int>(0, iZec133[i]);
				
			}
			/* Vectorizable loop 144 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec137[i] = (0.0f - fZec136[i]);
				
			}
			/* Vectorizable loop 145 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec139[i] = (0.0f - (0.5f * fZec138[i]));
				
			}
			/* Vectorizable loop 146 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec141[i] = (0.0f - (0.333333343f * fZec140[i]));
				
			}
			/* Vectorizable loop 147 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec143[i] = (0.0f - (0.25f * fZec142[i]));
				
			}
			/* Vectorizable loop 148 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec144[i] = (fZec131[i] + (5.0f - fZec135[i]));
				
			}
			/* Vectorizable loop 149 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec145[i] = std::max<int>(0, (iZec133[i] + 1));
				
			}
			/* Vectorizable loop 150 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec146[i] = (0.0f - fZec138[i]);
				
			}
			/* Vectorizable loop 151 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec147[i] = (0.0f - (0.5f * fZec140[i]));
				
			}
			/* Vectorizable loop 152 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec148[i] = (0.0f - (0.333333343f * fZec142[i]));
				
			}
			/* Vectorizable loop 153 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec149[i] = std::max<int>(0, (iZec133[i] + 2));
				
			}
			/* Vectorizable loop 154 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec150[i] = (0.0f - fZec140[i]);
				
			}
			/* Vectorizable loop 155 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec151[i] = (0.0f - (0.5f * fZec142[i]));
				
			}
			/* Vectorizable loop 156 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec153[i] = std::max<int>(0, (iZec133[i] + 3));
				
			}
			/* Vectorizable loop 157 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec154[i] = (0.0f - fZec142[i]);
				
			}
			/* Vectorizable loop 158 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec155[i] = (fZec152[i] * fZec140[i]);
				
			}
			/* Vectorizable loop 159 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec156[i] = std::max<int>(0, (iZec133[i] + 4));
				
			}
			/* Vectorizable loop 160 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec158[i] = std::floor(fZec157[i]);
				
			}
			/* Vectorizable loop 161 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				iZec163[i] = int(fZec162[i]);
				
			}
			/* Vectorizable loop 162 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec167[i] = (fSlow26 + (fZec161[i] + (2.0f - fZec164[i])));
				
			}
			/* Vectorizable loop 163 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec168[i] = (fSlow26 + (fZec161[i] + (1.0f - fZec164[i])));
				
			}
			/* Vectorizable loop 164 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec169[i] = (fZec165[i] * fZec166[i]);
				
			}
			/* Vectorizable loop 165 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec171[i] = std::floor(fZec170[i]);
				
			}
			/* Vectorizable loop 166 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec177[i] = std::floor(fZec176[i]);
				
			}
			/* Vectorizable loop 167 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec187[i] = std::floor(fZec186[i]);
				
			}
			/* Vectorizable loop 168 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec193[i] = std::floor(fZec192[i]);
				
			}
			/* Recursive loop 169 */
			/* Pre code */
			fYec0_idx = ((fYec0_idx + fYec0_idx_save) & 16383);
			for (int j8 = 0; (j8 < 4); j8 = (j8 + 1)) {
				fYec1_tmp[j8] = fYec1_perm[j8];
				
			}
			for (int j10 = 0; (j10 < 4); j10 = (j10 + 1)) {
				fRec52_tmp[j10] = fRec52_perm[j10];
				
			}
			for (int j12 = 0; (j12 < 4); j12 = (j12 + 1)) {
				fRec50_tmp[j12] = fRec50_perm[j12];
				
			}
			fYec2_idx = ((fYec2_idx + fYec2_idx_save) & 16383);
			for (int j16 = 0; (j16 < 4); j16 = (j16 + 1)) {
				fYec3_tmp[j16] = fYec3_perm[j16];
				
			}
			for (int j18 = 0; (j18 < 4); j18 = (j18 + 1)) {
				fRec54_tmp[j18] = fRec54_perm[j18];
				
			}
			for (int j20 = 0; (j20 < 4); j20 = (j20 + 1)) {
				fRec51_tmp[j20] = fRec51_perm[j20];
				
			}
			fYec4_idx = ((fYec4_idx + fYec4_idx_save) & 16383);
			for (int j24 = 0; (j24 < 4); j24 = (j24 + 1)) {
				fYec5_tmp[j24] = fYec5_perm[j24];
				
			}
			for (int j26 = 0; (j26 < 4); j26 = (j26 + 1)) {
				fRec49_tmp[j26] = fRec49_perm[j26];
				
			}
			for (int j28 = 0; (j28 < 4); j28 = (j28 + 1)) {
				fRec47_tmp[j28] = fRec47_perm[j28];
				
			}
			fYec6_idx = ((fYec6_idx + fYec6_idx_save) & 16383);
			for (int j32 = 0; (j32 < 4); j32 = (j32 + 1)) {
				fYec7_tmp[j32] = fYec7_perm[j32];
				
			}
			for (int j34 = 0; (j34 < 4); j34 = (j34 + 1)) {
				fRec57_tmp[j34] = fRec57_perm[j34];
				
			}
			for (int j36 = 0; (j36 < 4); j36 = (j36 + 1)) {
				fRec48_tmp[j36] = fRec48_perm[j36];
				
			}
			fYec8_idx = ((fYec8_idx + fYec8_idx_save) & 16383);
			for (int j40 = 0; (j40 < 4); j40 = (j40 + 1)) {
				fYec9_tmp[j40] = fYec9_perm[j40];
				
			}
			for (int j42 = 0; (j42 < 4); j42 = (j42 + 1)) {
				fRec46_tmp[j42] = fRec46_perm[j42];
				
			}
			for (int j44 = 0; (j44 < 4); j44 = (j44 + 1)) {
				fRec44_tmp[j44] = fRec44_perm[j44];
				
			}
			fYec10_idx = ((fYec10_idx + fYec10_idx_save) & 16383);
			for (int j48 = 0; (j48 < 4); j48 = (j48 + 1)) {
				fYec11_tmp[j48] = fYec11_perm[j48];
				
			}
			for (int j50 = 0; (j50 < 4); j50 = (j50 + 1)) {
				fRec60_tmp[j50] = fRec60_perm[j50];
				
			}
			for (int j52 = 0; (j52 < 4); j52 = (j52 + 1)) {
				fRec45_tmp[j52] = fRec45_perm[j52];
				
			}
			fYec12_idx = ((fYec12_idx + fYec12_idx_save) & 16383);
			for (int j56 = 0; (j56 < 4); j56 = (j56 + 1)) {
				fYec13_tmp[j56] = fYec13_perm[j56];
				
			}
			for (int j58 = 0; (j58 < 4); j58 = (j58 + 1)) {
				fRec43_tmp[j58] = fRec43_perm[j58];
				
			}
			for (int j60 = 0; (j60 < 4); j60 = (j60 + 1)) {
				fRec41_tmp[j60] = fRec41_perm[j60];
				
			}
			fYec14_idx = ((fYec14_idx + fYec14_idx_save) & 16383);
			for (int j64 = 0; (j64 < 4); j64 = (j64 + 1)) {
				fYec15_tmp[j64] = fYec15_perm[j64];
				
			}
			for (int j66 = 0; (j66 < 4); j66 = (j66 + 1)) {
				fRec63_tmp[j66] = fRec63_perm[j66];
				
			}
			for (int j68 = 0; (j68 < 4); j68 = (j68 + 1)) {
				fRec42_tmp[j68] = fRec42_perm[j68];
				
			}
			fYec16_idx = ((fYec16_idx + fYec16_idx_save) & 16383);
			for (int j72 = 0; (j72 < 4); j72 = (j72 + 1)) {
				fYec17_tmp[j72] = fYec17_perm[j72];
				
			}
			for (int j74 = 0; (j74 < 4); j74 = (j74 + 1)) {
				fRec40_tmp[j74] = fRec40_perm[j74];
				
			}
			for (int j76 = 0; (j76 < 4); j76 = (j76 + 1)) {
				fRec38_tmp[j76] = fRec38_perm[j76];
				
			}
			fYec18_idx = ((fYec18_idx + fYec18_idx_save) & 16383);
			for (int j80 = 0; (j80 < 4); j80 = (j80 + 1)) {
				fYec19_tmp[j80] = fYec19_perm[j80];
				
			}
			for (int j82 = 0; (j82 < 4); j82 = (j82 + 1)) {
				fRec66_tmp[j82] = fRec66_perm[j82];
				
			}
			for (int j84 = 0; (j84 < 4); j84 = (j84 + 1)) {
				fRec39_tmp[j84] = fRec39_perm[j84];
				
			}
			fYec20_idx = ((fYec20_idx + fYec20_idx_save) & 1023);
			fYec21_idx = ((fYec21_idx + fYec21_idx_save) & 16383);
			for (int j88 = 0; (j88 < 4); j88 = (j88 + 1)) {
				fYec22_tmp[j88] = fYec22_perm[j88];
				
			}
			for (int j90 = 0; (j90 < 4); j90 = (j90 + 1)) {
				fRec37_tmp[j90] = fRec37_perm[j90];
				
			}
			fYec23_idx = ((fYec23_idx + fYec23_idx_save) & 1023);
			fYec24_idx = ((fYec24_idx + fYec24_idx_save) & 16383);
			for (int j94 = 0; (j94 < 4); j94 = (j94 + 1)) {
				fYec25_tmp[j94] = fYec25_perm[j94];
				
			}
			for (int j96 = 0; (j96 < 4); j96 = (j96 + 1)) {
				fRec69_tmp[j96] = fRec69_perm[j96];
				
			}
			fYec26_idx = ((fYec26_idx + fYec26_idx_save) & 16383);
			for (int j100 = 0; (j100 < 4); j100 = (j100 + 1)) {
				fYec27_tmp[j100] = fYec27_perm[j100];
				
			}
			for (int j102 = 0; (j102 < 4); j102 = (j102 + 1)) {
				fRec36_tmp[j102] = fRec36_perm[j102];
				
			}
			for (int j104 = 0; (j104 < 4); j104 = (j104 + 1)) {
				fRec34_tmp[j104] = fRec34_perm[j104];
				
			}
			fYec28_idx = ((fYec28_idx + fYec28_idx_save) & 16383);
			for (int j108 = 0; (j108 < 4); j108 = (j108 + 1)) {
				fYec29_tmp[j108] = fYec29_perm[j108];
				
			}
			for (int j110 = 0; (j110 < 4); j110 = (j110 + 1)) {
				fRec72_tmp[j110] = fRec72_perm[j110];
				
			}
			for (int j112 = 0; (j112 < 4); j112 = (j112 + 1)) {
				fRec35_tmp[j112] = fRec35_perm[j112];
				
			}
			fYec30_idx = ((fYec30_idx + fYec30_idx_save) & 16383);
			for (int j116 = 0; (j116 < 4); j116 = (j116 + 1)) {
				fYec31_tmp[j116] = fYec31_perm[j116];
				
			}
			for (int j118 = 0; (j118 < 4); j118 = (j118 + 1)) {
				fRec33_tmp[j118] = fRec33_perm[j118];
				
			}
			for (int j120 = 0; (j120 < 4); j120 = (j120 + 1)) {
				fRec31_tmp[j120] = fRec31_perm[j120];
				
			}
			fYec32_idx = ((fYec32_idx + fYec32_idx_save) & 16383);
			for (int j124 = 0; (j124 < 4); j124 = (j124 + 1)) {
				fYec33_tmp[j124] = fYec33_perm[j124];
				
			}
			for (int j126 = 0; (j126 < 4); j126 = (j126 + 1)) {
				fRec75_tmp[j126] = fRec75_perm[j126];
				
			}
			for (int j128 = 0; (j128 < 4); j128 = (j128 + 1)) {
				fRec32_tmp[j128] = fRec32_perm[j128];
				
			}
			fYec34_idx = ((fYec34_idx + fYec34_idx_save) & 16383);
			for (int j132 = 0; (j132 < 4); j132 = (j132 + 1)) {
				fYec35_tmp[j132] = fYec35_perm[j132];
				
			}
			for (int j134 = 0; (j134 < 4); j134 = (j134 + 1)) {
				fRec30_tmp[j134] = fRec30_perm[j134];
				
			}
			for (int j136 = 0; (j136 < 4); j136 = (j136 + 1)) {
				fRec28_tmp[j136] = fRec28_perm[j136];
				
			}
			fYec36_idx = ((fYec36_idx + fYec36_idx_save) & 16383);
			for (int j140 = 0; (j140 < 4); j140 = (j140 + 1)) {
				fYec37_tmp[j140] = fYec37_perm[j140];
				
			}
			for (int j142 = 0; (j142 < 4); j142 = (j142 + 1)) {
				fRec78_tmp[j142] = fRec78_perm[j142];
				
			}
			for (int j144 = 0; (j144 < 4); j144 = (j144 + 1)) {
				fRec29_tmp[j144] = fRec29_perm[j144];
				
			}
			fYec38_idx = ((fYec38_idx + fYec38_idx_save) & 16383);
			for (int j148 = 0; (j148 < 4); j148 = (j148 + 1)) {
				fYec39_tmp[j148] = fYec39_perm[j148];
				
			}
			for (int j150 = 0; (j150 < 4); j150 = (j150 + 1)) {
				fRec27_tmp[j150] = fRec27_perm[j150];
				
			}
			for (int j152 = 0; (j152 < 4); j152 = (j152 + 1)) {
				fRec25_tmp[j152] = fRec25_perm[j152];
				
			}
			fYec40_idx = ((fYec40_idx + fYec40_idx_save) & 16383);
			for (int j156 = 0; (j156 < 4); j156 = (j156 + 1)) {
				fYec41_tmp[j156] = fYec41_perm[j156];
				
			}
			for (int j158 = 0; (j158 < 4); j158 = (j158 + 1)) {
				fRec81_tmp[j158] = fRec81_perm[j158];
				
			}
			for (int j160 = 0; (j160 < 4); j160 = (j160 + 1)) {
				fRec26_tmp[j160] = fRec26_perm[j160];
				
			}
			fYec42_idx = ((fYec42_idx + fYec42_idx_save) & 16383);
			for (int j164 = 0; (j164 < 4); j164 = (j164 + 1)) {
				fYec43_tmp[j164] = fYec43_perm[j164];
				
			}
			for (int j166 = 0; (j166 < 4); j166 = (j166 + 1)) {
				fRec24_tmp[j166] = fRec24_perm[j166];
				
			}
			for (int j168 = 0; (j168 < 4); j168 = (j168 + 1)) {
				fRec22_tmp[j168] = fRec22_perm[j168];
				
			}
			fYec44_idx = ((fYec44_idx + fYec44_idx_save) & 16383);
			for (int j172 = 0; (j172 < 4); j172 = (j172 + 1)) {
				fYec45_tmp[j172] = fYec45_perm[j172];
				
			}
			for (int j174 = 0; (j174 < 4); j174 = (j174 + 1)) {
				fRec84_tmp[j174] = fRec84_perm[j174];
				
			}
			for (int j176 = 0; (j176 < 4); j176 = (j176 + 1)) {
				fRec23_tmp[j176] = fRec23_perm[j176];
				
			}
			fYec46_idx = ((fYec46_idx + fYec46_idx_save) & 16383);
			fYec47_idx = ((fYec47_idx + fYec47_idx_save) & 16383);
			for (int j180 = 0; (j180 < 4); j180 = (j180 + 1)) {
				fYec48_tmp[j180] = fYec48_perm[j180];
				
			}
			for (int j182 = 0; (j182 < 4); j182 = (j182 + 1)) {
				fRec21_tmp[j182] = fRec21_perm[j182];
				
			}
			for (int j184 = 0; (j184 < 4); j184 = (j184 + 1)) {
				fRec20_tmp[j184] = fRec20_perm[j184];
				
			}
			for (int j186 = 0; (j186 < 4); j186 = (j186 + 1)) {
				fRec19_tmp[j186] = fRec19_perm[j186];
				
			}
			for (int j188 = 0; (j188 < 4); j188 = (j188 + 1)) {
				fRec18_tmp[j188] = fRec18_perm[j188];
				
			}
			for (int j190 = 0; (j190 < 4); j190 = (j190 + 1)) {
				fRec17_tmp[j190] = fRec17_perm[j190];
				
			}
			for (int j192 = 0; (j192 < 4); j192 = (j192 + 1)) {
				fRec92_tmp[j192] = fRec92_perm[j192];
				
			}
			for (int j194 = 0; (j194 < 4); j194 = (j194 + 1)) {
				fRec91_tmp[j194] = fRec91_perm[j194];
				
			}
			for (int j196 = 0; (j196 < 4); j196 = (j196 + 1)) {
				fRec90_tmp[j196] = fRec90_perm[j196];
				
			}
			for (int j198 = 0; (j198 < 4); j198 = (j198 + 1)) {
				fYec49_tmp[j198] = fYec49_perm[j198];
				
			}
			for (int j200 = 0; (j200 < 4); j200 = (j200 + 1)) {
				fRec89_tmp[j200] = fRec89_perm[j200];
				
			}
			for (int j202 = 0; (j202 < 4); j202 = (j202 + 1)) {
				fRec88_tmp[j202] = fRec88_perm[j202];
				
			}
			for (int j204 = 0; (j204 < 4); j204 = (j204 + 1)) {
				fRec87_tmp[j204] = fRec87_perm[j204];
				
			}
			for (int j206 = 0; (j206 < 4); j206 = (j206 + 1)) {
				fRec95_tmp[j206] = fRec95_perm[j206];
				
			}
			for (int j208 = 0; (j208 < 4); j208 = (j208 + 1)) {
				fRec94_tmp[j208] = fRec94_perm[j208];
				
			}
			for (int j210 = 0; (j210 < 4); j210 = (j210 + 1)) {
				fRec93_tmp[j210] = fRec93_perm[j210];
				
			}
			fYec50_idx = ((fYec50_idx + fYec50_idx_save) & 1023);
			for (int j212 = 0; (j212 < 4); j212 = (j212 + 1)) {
				fRec14_tmp[j212] = fRec14_perm[j212];
				
			}
			fYec51_idx = ((fYec51_idx + fYec51_idx_save) & 16383);
			fYec52_idx = ((fYec52_idx + fYec52_idx_save) & 16383);
			for (int j216 = 0; (j216 < 4); j216 = (j216 + 1)) {
				fYec53_tmp[j216] = fYec53_perm[j216];
				
			}
			for (int j218 = 0; (j218 < 4); j218 = (j218 + 1)) {
				fRec101_tmp[j218] = fRec101_perm[j218];
				
			}
			for (int j220 = 0; (j220 < 4); j220 = (j220 + 1)) {
				fRec100_tmp[j220] = fRec100_perm[j220];
				
			}
			for (int j222 = 0; (j222 < 4); j222 = (j222 + 1)) {
				fRec99_tmp[j222] = fRec99_perm[j222];
				
			}
			for (int j224 = 0; (j224 < 4); j224 = (j224 + 1)) {
				fRec98_tmp[j224] = fRec98_perm[j224];
				
			}
			for (int j226 = 0; (j226 < 4); j226 = (j226 + 1)) {
				fRec97_tmp[j226] = fRec97_perm[j226];
				
			}
			for (int j228 = 0; (j228 < 4); j228 = (j228 + 1)) {
				fRec108_tmp[j228] = fRec108_perm[j228];
				
			}
			for (int j230 = 0; (j230 < 4); j230 = (j230 + 1)) {
				fRec107_tmp[j230] = fRec107_perm[j230];
				
			}
			for (int j232 = 0; (j232 < 4); j232 = (j232 + 1)) {
				fRec106_tmp[j232] = fRec106_perm[j232];
				
			}
			for (int j234 = 0; (j234 < 4); j234 = (j234 + 1)) {
				fYec54_tmp[j234] = fYec54_perm[j234];
				
			}
			for (int j236 = 0; (j236 < 4); j236 = (j236 + 1)) {
				fRec105_tmp[j236] = fRec105_perm[j236];
				
			}
			for (int j238 = 0; (j238 < 4); j238 = (j238 + 1)) {
				fRec104_tmp[j238] = fRec104_perm[j238];
				
			}
			for (int j240 = 0; (j240 < 4); j240 = (j240 + 1)) {
				fRec103_tmp[j240] = fRec103_perm[j240];
				
			}
			for (int j242 = 0; (j242 < 4); j242 = (j242 + 1)) {
				fRec111_tmp[j242] = fRec111_perm[j242];
				
			}
			for (int j244 = 0; (j244 < 4); j244 = (j244 + 1)) {
				fRec110_tmp[j244] = fRec110_perm[j244];
				
			}
			for (int j246 = 0; (j246 < 4); j246 = (j246 + 1)) {
				fRec109_tmp[j246] = fRec109_perm[j246];
				
			}
			fYec55_idx = ((fYec55_idx + fYec55_idx_save) & 1023);
			for (int j248 = 0; (j248 < 4); j248 = (j248 + 1)) {
				fRec96_tmp[j248] = fRec96_perm[j248];
				
			}
			fYec56_idx = ((fYec56_idx + fYec56_idx_save) & 16383);
			for (int j250 = 0; (j250 < 4); j250 = (j250 + 1)) {
				fYec57_tmp[j250] = fYec57_perm[j250];
				
			}
			for (int j252 = 0; (j252 < 4); j252 = (j252 + 1)) {
				fRec13_tmp[j252] = fRec13_perm[j252];
				
			}
			for (int j254 = 0; (j254 < 4); j254 = (j254 + 1)) {
				fRec11_tmp[j254] = fRec11_perm[j254];
				
			}
			fYec58_idx = ((fYec58_idx + fYec58_idx_save) & 16383);
			for (int j258 = 0; (j258 < 4); j258 = (j258 + 1)) {
				fYec59_tmp[j258] = fYec59_perm[j258];
				
			}
			for (int j260 = 0; (j260 < 4); j260 = (j260 + 1)) {
				fRec112_tmp[j260] = fRec112_perm[j260];
				
			}
			for (int j262 = 0; (j262 < 4); j262 = (j262 + 1)) {
				fRec12_tmp[j262] = fRec12_perm[j262];
				
			}
			fYec60_idx = ((fYec60_idx + fYec60_idx_save) & 16383);
			for (int j264 = 0; (j264 < 4); j264 = (j264 + 1)) {
				fYec61_tmp[j264] = fYec61_perm[j264];
				
			}
			for (int j266 = 0; (j266 < 4); j266 = (j266 + 1)) {
				fRec10_tmp[j266] = fRec10_perm[j266];
				
			}
			for (int j268 = 0; (j268 < 4); j268 = (j268 + 1)) {
				fRec8_tmp[j268] = fRec8_perm[j268];
				
			}
			fYec62_idx = ((fYec62_idx + fYec62_idx_save) & 16383);
			for (int j270 = 0; (j270 < 4); j270 = (j270 + 1)) {
				fYec63_tmp[j270] = fYec63_perm[j270];
				
			}
			for (int j272 = 0; (j272 < 4); j272 = (j272 + 1)) {
				fRec114_tmp[j272] = fRec114_perm[j272];
				
			}
			for (int j274 = 0; (j274 < 4); j274 = (j274 + 1)) {
				fRec9_tmp[j274] = fRec9_perm[j274];
				
			}
			fYec64_idx = ((fYec64_idx + fYec64_idx_save) & 16383);
			for (int j276 = 0; (j276 < 4); j276 = (j276 + 1)) {
				fYec65_tmp[j276] = fYec65_perm[j276];
				
			}
			for (int j278 = 0; (j278 < 4); j278 = (j278 + 1)) {
				fRec7_tmp[j278] = fRec7_perm[j278];
				
			}
			for (int j280 = 0; (j280 < 4); j280 = (j280 + 1)) {
				fRec5_tmp[j280] = fRec5_perm[j280];
				
			}
			fYec66_idx = ((fYec66_idx + fYec66_idx_save) & 16383);
			for (int j284 = 0; (j284 < 4); j284 = (j284 + 1)) {
				fYec67_tmp[j284] = fYec67_perm[j284];
				
			}
			for (int j286 = 0; (j286 < 4); j286 = (j286 + 1)) {
				fRec115_tmp[j286] = fRec115_perm[j286];
				
			}
			for (int j288 = 0; (j288 < 4); j288 = (j288 + 1)) {
				fRec6_tmp[j288] = fRec6_perm[j288];
				
			}
			fYec68_idx = ((fYec68_idx + fYec68_idx_save) & 16383);
			for (int j292 = 0; (j292 < 4); j292 = (j292 + 1)) {
				fYec69_tmp[j292] = fYec69_perm[j292];
				
			}
			for (int j294 = 0; (j294 < 4); j294 = (j294 + 1)) {
				fRec4_tmp[j294] = fRec4_perm[j294];
				
			}
			for (int j296 = 0; (j296 < 4); j296 = (j296 + 1)) {
				fRec2_tmp[j296] = fRec2_perm[j296];
				
			}
			fYec70_idx = ((fYec70_idx + fYec70_idx_save) & 16383);
			for (int j298 = 0; (j298 < 4); j298 = (j298 + 1)) {
				fYec71_tmp[j298] = fYec71_perm[j298];
				
			}
			for (int j300 = 0; (j300 < 4); j300 = (j300 + 1)) {
				fRec118_tmp[j300] = fRec118_perm[j300];
				
			}
			for (int j302 = 0; (j302 < 4); j302 = (j302 + 1)) {
				fRec3_tmp[j302] = fRec3_perm[j302];
				
			}
			for (int j304 = 0; (j304 < 4); j304 = (j304 + 1)) {
				fRec0_tmp[j304] = fRec0_perm[j304];
				
			}
			for (int j306 = 0; (j306 < 4); j306 = (j306 + 1)) {
				fRec1_tmp[j306] = fRec1_perm[j306];
				
			}
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				fZec1[i] = ((0.760313988f * fRec0[(i - 1)]) - (0.649555743f * fRec50[(i - 1)]));
				fZec2[i] = ((0.760313988f * fRec1[(i - 1)]) - (0.649555743f * fRec51[(i - 1)]));
				fYec0[((i + fYec0_idx) & 16383)] = ((0.707106769f * fZec1[i]) - (0.707106769f * fZec2[i]));
				fYec1[i] = fYec0[(((i + fYec0_idx) - std::min<int>(8192, std::max<int>(0, int(fZec3[i])))) & 16383)];
				fRec52[i] = (fYec1[(i - 1)] - (((fZec4[i] + (2.0f - fRec53[i])) * (fRec52[(i - 1)] - fYec1[i])) / (fRec53[i] - fZec4[i])));
				fRec50[i] = fRec52[i];
				fYec2[((i + fYec2_idx) & 16383)] = ((0.707106769f * fZec1[i]) + (0.707106769f * fZec2[i]));
				fYec3[i] = fYec2[(((i + fYec2_idx) - std::min<int>(8192, std::max<int>(0, int(fZec5[i])))) & 16383)];
				fRec54[i] = (fYec3[(i - 1)] - (((fZec6[i] + (2.0f - fRec55[i])) * (fRec54[(i - 1)] - fYec3[i])) / (fRec55[i] - fZec6[i])));
				fRec51[i] = fRec54[i];
				fZec7[i] = ((0.760313988f * fRec50[(i - 1)]) + (0.649555743f * fRec0[(i - 1)]));
				fZec8[i] = ((0.760313988f * fZec7[i]) - (0.649555743f * fRec47[(i - 1)]));
				fZec9[i] = ((0.760313988f * fRec51[(i - 1)]) + (0.649555743f * fRec1[(i - 1)]));
				fZec10[i] = ((0.760313988f * fZec9[i]) - (0.649555743f * fRec48[(i - 1)]));
				fYec4[((i + fYec4_idx) & 16383)] = ((0.707106769f * fZec8[i]) - (0.707106769f * fZec10[i]));
				fYec5[i] = fYec4[(((i + fYec4_idx) - std::min<int>(8192, std::max<int>(0, int(fZec11[i])))) & 16383)];
				fRec49[i] = (fYec5[(i - 1)] - (((fZec12[i] + (2.0f - fRec56[i])) * (fRec49[(i - 1)] - fYec5[i])) / (fRec56[i] - fZec12[i])));
				fRec47[i] = fRec49[i];
				fYec6[((i + fYec6_idx) & 16383)] = ((0.707106769f * fZec8[i]) + (0.707106769f * fZec10[i]));
				fYec7[i] = fYec6[(((i + fYec6_idx) - std::min<int>(8192, std::max<int>(0, int(fZec13[i])))) & 16383)];
				fRec57[i] = (fYec7[(i - 1)] - (((fZec14[i] + (2.0f - fRec58[i])) * (fRec57[(i - 1)] - fYec7[i])) / (fRec58[i] - fZec14[i])));
				fRec48[i] = fRec57[i];
				fZec15[i] = ((0.760313988f * fRec47[(i - 1)]) + (0.649555743f * fZec7[i]));
				fZec16[i] = ((0.760313988f * fZec15[i]) - (0.649555743f * fRec44[(i - 1)]));
				fZec17[i] = ((0.760313988f * fRec48[(i - 1)]) + (0.649555743f * fZec9[i]));
				fZec18[i] = ((0.760313988f * fZec17[i]) - (0.649555743f * fRec45[(i - 1)]));
				fYec8[((i + fYec8_idx) & 16383)] = ((0.707106769f * fZec16[i]) - (0.707106769f * fZec18[i]));
				fYec9[i] = fYec8[(((i + fYec8_idx) - std::min<int>(8192, std::max<int>(0, int(fZec19[i])))) & 16383)];
				fRec46[i] = (fYec9[(i - 1)] - (((fZec20[i] + (2.0f - fRec59[i])) * (fRec46[(i - 1)] - fYec9[i])) / (fRec59[i] - fZec20[i])));
				fRec44[i] = fRec46[i];
				fYec10[((i + fYec10_idx) & 16383)] = ((0.707106769f * fZec16[i]) + (0.707106769f * fZec18[i]));
				fYec11[i] = fYec10[(((i + fYec10_idx) - std::min<int>(8192, std::max<int>(0, int(fZec21[i])))) & 16383)];
				fRec60[i] = (fYec11[(i - 1)] - (((fZec22[i] + (2.0f - fRec61[i])) * (fRec60[(i - 1)] - fYec11[i])) / (fRec61[i] - fZec22[i])));
				fRec45[i] = fRec60[i];
				fZec23[i] = ((0.760313988f * fRec44[(i - 1)]) + (0.649555743f * fZec15[i]));
				fZec24[i] = ((0.760313988f * fZec23[i]) - (0.649555743f * fRec41[(i - 1)]));
				fZec25[i] = ((0.760313988f * fRec45[(i - 1)]) + (0.649555743f * fZec17[i]));
				fZec26[i] = ((0.760313988f * fZec25[i]) - (0.649555743f * fRec42[(i - 1)]));
				fYec12[((i + fYec12_idx) & 16383)] = ((0.707106769f * fZec24[i]) - (0.707106769f * fZec26[i]));
				fYec13[i] = fYec12[(((i + fYec12_idx) - std::min<int>(8192, std::max<int>(0, int(fZec27[i])))) & 16383)];
				fRec43[i] = (fYec13[(i - 1)] - (((fZec28[i] + (2.0f - fRec62[i])) * (fRec43[(i - 1)] - fYec13[i])) / (fRec62[i] - fZec28[i])));
				fRec41[i] = fRec43[i];
				fYec14[((i + fYec14_idx) & 16383)] = ((0.707106769f * fZec24[i]) + (0.707106769f * fZec26[i]));
				fYec15[i] = fYec14[(((i + fYec14_idx) - std::min<int>(8192, std::max<int>(0, int(fZec29[i])))) & 16383)];
				fRec63[i] = (fYec15[(i - 1)] - (((fZec30[i] + (2.0f - fRec64[i])) * (fRec63[(i - 1)] - fYec15[i])) / (fRec64[i] - fZec30[i])));
				fRec42[i] = fRec63[i];
				fZec31[i] = ((0.760313988f * fRec41[(i - 1)]) + (0.649555743f * fZec23[i]));
				fZec32[i] = ((0.760313988f * fZec31[i]) - (0.649555743f * fRec38[(i - 1)]));
				fZec33[i] = ((0.760313988f * fRec42[(i - 1)]) + (0.649555743f * fZec25[i]));
				fZec34[i] = ((0.760313988f * fZec33[i]) - (0.649555743f * fRec39[(i - 1)]));
				fYec16[((i + fYec16_idx) & 16383)] = ((0.707106769f * fZec32[i]) - (0.707106769f * fZec34[i]));
				fYec17[i] = fYec16[(((i + fYec16_idx) - std::min<int>(8192, std::max<int>(0, int(fZec35[i])))) & 16383)];
				fRec40[i] = (fYec17[(i - 1)] - (((fZec36[i] + (2.0f - fRec65[i])) * (fRec40[(i - 1)] - fYec17[i])) / (fRec65[i] - fZec36[i])));
				fRec38[i] = fRec40[i];
				fYec18[((i + fYec18_idx) & 16383)] = ((0.707106769f * fZec32[i]) + (0.707106769f * fZec34[i]));
				fYec19[i] = fYec18[(((i + fYec18_idx) - std::min<int>(8192, std::max<int>(0, int(fZec37[i])))) & 16383)];
				fRec66[i] = (fYec19[(i - 1)] - (((fZec38[i] + (2.0f - fRec67[i])) * (fRec66[(i - 1)] - fYec19[i])) / (fRec67[i] - fZec38[i])));
				fRec39[i] = fRec66[i];
				fYec20[((i + fYec20_idx) & 1023)] = ((0.760313988f * fRec38[(i - 1)]) + (0.649555743f * fZec31[i]));
				fYec21[((i + fYec21_idx) & 16383)] = (((((fYec20[(((i + fYec20_idx) - iZec42[i]) & 1023)] * fZec45[i]) * fZec47[i]) * fZec49[i]) * fZec51[i]) + (fZec52[i] * ((((((fYec20[(((i + fYec20_idx) - iZec53[i]) & 1023)] * fZec54[i]) * fZec55[i]) * fZec56[i]) + (0.5f * (((fZec44[i] * fYec20[(((i + fYec20_idx) - iZec57[i]) & 1023)]) * fZec58[i]) * fZec59[i]))) + (0.166666672f * ((fZec60[i] * fYec20[(((i + fYec20_idx) - iZec61[i]) & 1023)]) * fZec62[i]))) + (0.0416666679f * (fZec63[i] * fYec20[(((i + fYec20_idx) - iZec64[i]) & 1023)])))));
				fYec22[i] = fYec21[(((i + fYec21_idx) - std::min<int>(8192, std::max<int>(0, int(fZec65[i])))) & 16383)];
				fRec37[i] = (fYec22[(i - 1)] - (((fZec66[i] + (2.0f - fRec68[i])) * (fRec37[(i - 1)] - fYec22[i])) / (fRec68[i] - fZec66[i])));
				fYec23[((i + fYec23_idx) & 1023)] = ((0.760313988f * fRec39[(i - 1)]) + (0.649555743f * fZec33[i]));
				fYec24[((i + fYec24_idx) & 16383)] = (((((fYec23[(((i + fYec23_idx) - std::min<int>(512, std::max<int>(0, iZec69[i]))) & 1023)] * (0.0f - fZec71[i])) * (0.0f - (0.5f * fZec72[i]))) * (0.0f - (0.333333343f * fZec73[i]))) * (0.0f - (0.25f * fZec74[i]))) + ((fSlow26 + (fZec67[i] + (5.0f - fZec70[i]))) * ((((((fYec23[(((i + fYec23_idx) - std::min<int>(512, std::max<int>(0, (iZec69[i] + 1)))) & 1023)] * (0.0f - fZec72[i])) * (0.0f - (0.5f * fZec73[i]))) * (0.0f - (0.333333343f * fZec74[i]))) + (0.5f * (((fZec71[i] * fYec23[(((i + fYec23_idx) - std::min<int>(512, std::max<int>(0, (iZec69[i] + 2)))) & 1023)]) * (0.0f - fZec73[i])) * (0.0f - (0.5f * fZec74[i]))))) + (0.166666672f * ((fZec75[i] * fYec23[(((i + fYec23_idx) - std::min<int>(512, std::max<int>(0, (iZec69[i] + 3)))) & 1023)]) * (0.0f - fZec74[i])))) + (0.0416666679f * ((fZec75[i] * fZec73[i]) * fYec23[(((i + fYec23_idx) - std::min<int>(512, std::max<int>(0, (iZec69[i] + 4)))) & 1023)])))));
				fYec25[i] = fYec24[(((i + fYec24_idx) - std::min<int>(8192, std::max<int>(0, int(fZec76[i])))) & 16383)];
				fRec69[i] = (fYec25[(i - 1)] - (((fZec77[i] + (2.0f - fRec70[i])) * (fRec69[(i - 1)] - fYec25[i])) / (fRec70[i] - fZec77[i])));
				fZec78[i] = ((0.760313988f * fRec37[i]) - (0.649555743f * fRec34[(i - 1)]));
				fZec79[i] = ((0.760313988f * fRec69[i]) - (0.649555743f * fRec35[(i - 1)]));
				fYec26[((i + fYec26_idx) & 16383)] = ((0.707106769f * fZec78[i]) - (0.707106769f * fZec79[i]));
				fYec27[i] = fYec26[(((i + fYec26_idx) - std::min<int>(8192, std::max<int>(0, int(fZec80[i])))) & 16383)];
				fRec36[i] = (fYec27[(i - 1)] - (((fZec81[i] + (2.0f - fRec71[i])) * (fRec36[(i - 1)] - fYec27[i])) / (fRec71[i] - fZec81[i])));
				fRec34[i] = fRec36[i];
				fYec28[((i + fYec28_idx) & 16383)] = ((0.707106769f * fZec78[i]) + (0.707106769f * fZec79[i]));
				fYec29[i] = fYec28[(((i + fYec28_idx) - std::min<int>(8192, std::max<int>(0, int(fZec82[i])))) & 16383)];
				fRec72[i] = (fYec29[(i - 1)] - (((fZec83[i] + (2.0f - fRec73[i])) * (fRec72[(i - 1)] - fYec29[i])) / (fRec73[i] - fZec83[i])));
				fRec35[i] = fRec72[i];
				fZec84[i] = ((0.760313988f * fRec34[(i - 1)]) + (0.649555743f * fRec37[i]));
				fZec85[i] = ((0.760313988f * fZec84[i]) - (0.649555743f * fRec31[(i - 1)]));
				fZec86[i] = ((0.760313988f * fRec35[(i - 1)]) + (0.649555743f * fRec69[i]));
				fZec87[i] = ((0.760313988f * fZec86[i]) - (0.649555743f * fRec32[(i - 1)]));
				fYec30[((i + fYec30_idx) & 16383)] = ((0.707106769f * fZec85[i]) - (0.707106769f * fZec87[i]));
				fYec31[i] = fYec30[(((i + fYec30_idx) - std::min<int>(8192, std::max<int>(0, int(fZec88[i])))) & 16383)];
				fRec33[i] = (fYec31[(i - 1)] - (((fZec89[i] + (2.0f - fRec74[i])) * (fRec33[(i - 1)] - fYec31[i])) / (fRec74[i] - fZec89[i])));
				fRec31[i] = fRec33[i];
				fYec32[((i + fYec32_idx) & 16383)] = ((0.707106769f * fZec85[i]) + (0.707106769f * fZec87[i]));
				fYec33[i] = fYec32[(((i + fYec32_idx) - iZec91[i]) & 16383)];
				fRec75[i] = (fYec33[(i - 1)] - ((fZec93[i] * (fRec75[(i - 1)] - fYec33[i])) / fZec94[i]));
				fRec32[i] = fRec75[i];
				fZec95[i] = ((0.760313988f * fRec31[(i - 1)]) + (0.649555743f * fZec84[i]));
				fZec96[i] = ((0.760313988f * fZec95[i]) - (0.649555743f * fRec28[(i - 1)]));
				fZec97[i] = ((0.760313988f * fRec32[(i - 1)]) + (0.649555743f * fZec86[i]));
				fZec98[i] = ((0.760313988f * fZec97[i]) - (0.649555743f * fRec29[(i - 1)]));
				fYec34[((i + fYec34_idx) & 16383)] = ((0.707106769f * fZec96[i]) - (0.707106769f * fZec98[i]));
				fYec35[i] = fYec34[(((i + fYec34_idx) - std::min<int>(8192, std::max<int>(0, int(fZec99[i])))) & 16383)];
				fRec30[i] = (fYec35[(i - 1)] - (((fZec100[i] + (2.0f - fRec77[i])) * (fRec30[(i - 1)] - fYec35[i])) / (fRec77[i] - fZec100[i])));
				fRec28[i] = fRec30[i];
				fYec36[((i + fYec36_idx) & 16383)] = ((0.707106769f * fZec96[i]) + (0.707106769f * fZec98[i]));
				fYec37[i] = fYec36[(((i + fYec36_idx) - iZec102[i]) & 16383)];
				fRec78[i] = (fYec37[(i - 1)] - ((fZec104[i] * (fRec78[(i - 1)] - fYec37[i])) / fZec105[i]));
				fRec29[i] = fRec78[i];
				fZec106[i] = ((0.760313988f * fRec28[(i - 1)]) + (0.649555743f * fZec95[i]));
				fZec107[i] = ((0.760313988f * fZec106[i]) - (0.649555743f * fRec25[(i - 1)]));
				fZec108[i] = ((0.760313988f * fRec29[(i - 1)]) + (0.649555743f * fZec97[i]));
				fZec109[i] = ((0.760313988f * fZec108[i]) - (0.649555743f * fRec26[(i - 1)]));
				fYec38[((i + fYec38_idx) & 16383)] = ((0.707106769f * fZec107[i]) - (0.707106769f * fZec109[i]));
				fYec39[i] = fYec38[(((i + fYec38_idx) - iZec111[i]) & 16383)];
				fRec27[i] = (fYec39[(i - 1)] - ((fZec113[i] * (fRec27[(i - 1)] - fYec39[i])) / fZec114[i]));
				fRec25[i] = fRec27[i];
				fYec40[((i + fYec40_idx) & 16383)] = ((0.707106769f * fZec107[i]) + (0.707106769f * fZec109[i]));
				fYec41[i] = fYec40[(((i + fYec40_idx) - iZec116[i]) & 16383)];
				fRec81[i] = (fYec41[(i - 1)] - ((fZec118[i] * (fRec81[(i - 1)] - fYec41[i])) / fZec119[i]));
				fRec26[i] = fRec81[i];
				fZec120[i] = ((0.760313988f * fRec25[(i - 1)]) + (0.649555743f * fZec106[i]));
				fZec121[i] = ((0.760313988f * fZec120[i]) - (0.649555743f * fRec22[(i - 1)]));
				fZec122[i] = ((0.760313988f * fRec26[(i - 1)]) + (0.649555743f * fZec108[i]));
				fZec123[i] = ((0.760313988f * fZec122[i]) - (0.649555743f * fRec23[(i - 1)]));
				fYec42[((i + fYec42_idx) & 16383)] = ((0.707106769f * fZec121[i]) - (0.707106769f * fZec123[i]));
				fYec43[i] = fYec42[(((i + fYec42_idx) - std::min<int>(8192, std::max<int>(0, int(fZec124[i])))) & 16383)];
				fRec24[i] = (fYec43[(i - 1)] - (((fZec125[i] + (2.0f - fRec83[i])) * (fRec24[(i - 1)] - fYec43[i])) / (fRec83[i] - fZec125[i])));
				fRec22[i] = fRec24[i];
				fYec44[((i + fYec44_idx) & 16383)] = ((0.707106769f * fZec121[i]) + (0.707106769f * fZec123[i]));
				fYec45[i] = fYec44[(((i + fYec44_idx) - iZec127[i]) & 16383)];
				fRec84[i] = (fYec45[(i - 1)] - ((fZec129[i] * (fRec84[(i - 1)] - fYec45[i])) / fZec130[i]));
				fRec23[i] = fRec84[i];
				fYec46[((i + fYec46_idx) & 16383)] = ((0.760313988f * fRec22[(i - 1)]) + (0.649555743f * fZec120[i]));
				fYec47[((i + fYec47_idx) & 16383)] = (((((fYec46[(((i + fYec46_idx) - std::min<int>(8192, iZec134[i])) & 16383)] * fZec137[i]) * fZec139[i]) * fZec141[i]) * fZec143[i]) + (fZec144[i] * ((((((fYec46[(((i + fYec46_idx) - std::min<int>(8192, iZec145[i])) & 16383)] * fZec146[i]) * fZec147[i]) * fZec148[i]) + (0.5f * (((fZec136[i] * fYec46[(((i + fYec46_idx) - std::min<int>(8192, iZec149[i])) & 16383)]) * fZec150[i]) * fZec151[i]))) + (0.166666672f * ((fZec152[i] * fYec46[(((i + fYec46_idx) - std::min<int>(8192, iZec153[i])) & 16383)]) * fZec154[i]))) + (0.0416666679f * (fZec155[i] * fYec46[(((i + fYec46_idx) - std::min<int>(8192, iZec156[i])) & 16383)])))));
				fYec48[i] = fYec47[(((i + fYec47_idx) - std::min<int>(8192, std::max<int>(0, int(fZec157[i])))) & 16383)];
				fRec21[i] = (fYec48[(i - 1)] - (((fZec158[i] + (2.0f - fRec86[i])) * (fRec21[(i - 1)] - fYec48[i])) / (fRec86[i] - fZec158[i])));
				fRec20[i] = ((fSlow55 * fRec21[(i - 1)]) - (fSlow56 * ((fSlow57 * fRec20[(i - 1)]) - (fSlow53 * fRec21[i]))));
				fRec19[i] = (fRec20[i] - (fSlow58 * ((fSlow59 * fRec19[(i - 2)]) + (fSlow62 * fRec19[(i - 1)]))));
				fRec18[i] = ((fSlow58 * (((fSlow61 * fRec19[i]) + (fSlow63 * fRec19[(i - 1)])) + (fSlow61 * fRec19[(i - 2)]))) - (fSlow65 * ((fSlow66 * fRec18[(i - 2)]) + (fSlow62 * fRec18[(i - 1)]))));
				fZec159[i] = (fSlow73 * fRec17[(i - 1)]);
				fRec17[i] = ((fSlow65 * (((fSlow61 * fRec18[i]) + (fSlow63 * fRec18[(i - 1)])) + (fSlow61 * fRec18[(i - 2)]))) - (fSlow69 * ((fSlow70 * fRec17[(i - 2)]) + fZec159[i])));
				fRec92[i] = (0.0f - (fSlow56 * ((fSlow57 * fRec92[(i - 1)]) - (fRec21[i] + fRec21[(i - 1)]))));
				fRec91[i] = (fRec92[i] - (fSlow58 * ((fSlow59 * fRec91[(i - 2)]) + (fSlow62 * fRec91[(i - 1)]))));
				fRec90[i] = ((fSlow58 * (fRec91[(i - 2)] + (fRec91[i] + (2.0f * fRec91[(i - 1)])))) - (fSlow65 * ((fSlow66 * fRec90[(i - 2)]) + (fSlow62 * fRec90[(i - 1)]))));
				fZec160[i] = (fRec90[(i - 2)] + (fRec90[i] + (2.0f * fRec90[(i - 1)])));
				fYec49[i] = (fSlow65 * fZec160[i]);
				fRec89[i] = ((fSlow75 * fYec49[(i - 1)]) - (fSlow76 * ((fSlow77 * fRec89[(i - 1)]) - (fSlow78 * fZec160[i]))));
				fRec88[i] = (fRec89[i] - (fSlow79 * ((fSlow80 * fRec88[(i - 2)]) + (fSlow73 * fRec88[(i - 1)]))));
				fRec87[i] = ((fSlow79 * (((fSlow72 * fRec88[i]) + (fSlow81 * fRec88[(i - 1)])) + (fSlow72 * fRec88[(i - 2)]))) - (fSlow82 * ((fSlow83 * fRec87[(i - 2)]) + (fSlow73 * fRec87[(i - 1)]))));
				fRec95[i] = (0.0f - (fSlow76 * ((fSlow77 * fRec95[(i - 1)]) - (fYec49[i] + fYec49[(i - 1)]))));
				fRec94[i] = (fRec95[i] - (fSlow79 * ((fSlow80 * fRec94[(i - 2)]) + (fSlow73 * fRec94[(i - 1)]))));
				fRec93[i] = ((fSlow79 * (fRec94[(i - 2)] + (fRec94[i] + (2.0f * fRec94[(i - 1)])))) - (fSlow82 * ((fSlow83 * fRec93[(i - 2)]) + (fSlow73 * fRec93[(i - 1)]))));
				fYec50[((i + fYec50_idx) & 1023)] = ((fSlow86 * ((fSlow87 * (fRec17[(i - 2)] + (fSlow69 * (fZec159[i] + (fSlow70 * fRec17[i]))))) + (fSlow82 * ((fSlow88 * (((fSlow72 * fRec87[i]) + (fSlow81 * fRec87[(i - 1)])) + (fSlow72 * fRec87[(i - 2)]))) + (fSlow89 * (fRec93[(i - 2)] + (fRec93[i] + (2.0f * fRec93[(i - 1)])))))))) + float(input0[i]));
				fRec14[i] = ((fSlow84 * fRec14[(i - 1)]) + (fSlow85 * (((((fZec137[i] * fZec139[i]) * fZec141[i]) * fZec143[i]) * fYec50[(((i + fYec50_idx) - std::min<int>(512, iZec134[i])) & 1023)]) + (fZec144[i] * ((((((fZec146[i] * fZec147[i]) * fZec148[i]) * fYec50[(((i + fYec50_idx) - std::min<int>(512, iZec145[i])) & 1023)]) + (0.5f * (((fZec136[i] * fZec150[i]) * fZec151[i]) * fYec50[(((i + fYec50_idx) - std::min<int>(512, iZec149[i])) & 1023)]))) + (0.166666672f * ((fZec152[i] * fZec154[i]) * fYec50[(((i + fYec50_idx) - std::min<int>(512, iZec153[i])) & 1023)]))) + (0.0416666679f * (fZec155[i] * fYec50[(((i + fYec50_idx) - std::min<int>(512, iZec156[i])) & 1023)])))))));
				fYec51[((i + fYec51_idx) & 16383)] = ((0.760313988f * fRec23[(i - 1)]) + (0.649555743f * fZec122[i]));
				fYec52[((i + fYec52_idx) & 16383)] = (((((fYec51[(((i + fYec51_idx) - std::min<int>(8192, std::max<int>(0, iZec163[i]))) & 16383)] * (0.0f - fZec165[i])) * (0.0f - (0.5f * fZec166[i]))) * (0.0f - (0.333333343f * fZec167[i]))) * (0.0f - (0.25f * fZec168[i]))) + ((fSlow26 + (fZec161[i] + (5.0f - fZec164[i]))) * ((((((fYec51[(((i + fYec51_idx) - std::min<int>(8192, std::max<int>(0, (iZec163[i] + 1)))) & 16383)] * (0.0f - fZec166[i])) * (0.0f - (0.5f * fZec167[i]))) * (0.0f - (0.333333343f * fZec168[i]))) + (0.5f * (((fZec165[i] * fYec51[(((i + fYec51_idx) - std::min<int>(8192, std::max<int>(0, (iZec163[i] + 2)))) & 16383)]) * (0.0f - fZec167[i])) * (0.0f - (0.5f * fZec168[i]))))) + (0.166666672f * ((fZec169[i] * fYec51[(((i + fYec51_idx) - std::min<int>(8192, std::max<int>(0, (iZec163[i] + 3)))) & 16383)]) * (0.0f - fZec168[i])))) + (0.0416666679f * ((fZec169[i] * fZec167[i]) * fYec51[(((i + fYec51_idx) - std::min<int>(8192, std::max<int>(0, (iZec163[i] + 4)))) & 16383)])))));
				fYec53[i] = fYec52[(((i + fYec52_idx) - std::min<int>(8192, std::max<int>(0, int(fZec170[i])))) & 16383)];
				fRec101[i] = (fYec53[(i - 1)] - (((fZec171[i] + (2.0f - fRec102[i])) * (fRec101[(i - 1)] - fYec53[i])) / (fRec102[i] - fZec171[i])));
				fRec100[i] = ((fSlow55 * fRec101[(i - 1)]) - (fSlow56 * ((fSlow57 * fRec100[(i - 1)]) - (fSlow53 * fRec101[i]))));
				fRec99[i] = (fRec100[i] - (fSlow58 * ((fSlow59 * fRec99[(i - 2)]) + (fSlow62 * fRec99[(i - 1)]))));
				fRec98[i] = ((fSlow58 * (((fSlow61 * fRec99[i]) + (fSlow63 * fRec99[(i - 1)])) + (fSlow61 * fRec99[(i - 2)]))) - (fSlow65 * ((fSlow66 * fRec98[(i - 2)]) + (fSlow62 * fRec98[(i - 1)]))));
				fZec172[i] = (fSlow73 * fRec97[(i - 1)]);
				fRec97[i] = ((fSlow65 * (((fSlow61 * fRec98[i]) + (fSlow63 * fRec98[(i - 1)])) + (fSlow61 * fRec98[(i - 2)]))) - (fSlow69 * ((fSlow70 * fRec97[(i - 2)]) + fZec172[i])));
				fRec108[i] = (0.0f - (fSlow56 * ((fSlow57 * fRec108[(i - 1)]) - (fRec101[i] + fRec101[(i - 1)]))));
				fRec107[i] = (fRec108[i] - (fSlow58 * ((fSlow59 * fRec107[(i - 2)]) + (fSlow62 * fRec107[(i - 1)]))));
				fRec106[i] = ((fSlow58 * (fRec107[(i - 2)] + (fRec107[i] + (2.0f * fRec107[(i - 1)])))) - (fSlow65 * ((fSlow66 * fRec106[(i - 2)]) + (fSlow62 * fRec106[(i - 1)]))));
				fZec173[i] = (fRec106[(i - 2)] + (fRec106[i] + (2.0f * fRec106[(i - 1)])));
				fYec54[i] = (fSlow65 * fZec173[i]);
				fRec105[i] = ((fSlow75 * fYec54[(i - 1)]) - (fSlow76 * ((fSlow77 * fRec105[(i - 1)]) - (fSlow78 * fZec173[i]))));
				fRec104[i] = (fRec105[i] - (fSlow79 * ((fSlow80 * fRec104[(i - 2)]) + (fSlow73 * fRec104[(i - 1)]))));
				fRec103[i] = ((fSlow79 * (((fSlow72 * fRec104[i]) + (fSlow81 * fRec104[(i - 1)])) + (fSlow72 * fRec104[(i - 2)]))) - (fSlow82 * ((fSlow83 * fRec103[(i - 2)]) + (fSlow73 * fRec103[(i - 1)]))));
				fRec111[i] = (0.0f - (fSlow76 * ((fSlow77 * fRec111[(i - 1)]) - (fYec54[i] + fYec54[(i - 1)]))));
				fRec110[i] = (fRec111[i] - (fSlow79 * ((fSlow80 * fRec110[(i - 2)]) + (fSlow73 * fRec110[(i - 1)]))));
				fRec109[i] = ((fSlow79 * (fRec110[(i - 2)] + (fRec110[i] + (2.0f * fRec110[(i - 1)])))) - (fSlow82 * ((fSlow83 * fRec109[(i - 2)]) + (fSlow73 * fRec109[(i - 1)]))));
				fYec55[((i + fYec55_idx) & 1023)] = ((fSlow86 * ((fSlow87 * (fRec97[(i - 2)] + (fSlow69 * (fZec172[i] + (fSlow70 * fRec97[i]))))) + (fSlow82 * ((fSlow88 * (((fSlow72 * fRec103[i]) + (fSlow81 * fRec103[(i - 1)])) + (fSlow72 * fRec103[(i - 2)]))) + (fSlow89 * (fRec109[(i - 2)] + (fRec109[i] + (2.0f * fRec109[(i - 1)])))))))) + float(input1[i]));
				fRec96[i] = ((fSlow84 * fRec96[(i - 1)]) + (fSlow85 * (((((fZec45[i] * fZec47[i]) * fZec49[i]) * fZec51[i]) * fYec55[(((i + fYec55_idx) - iZec42[i]) & 1023)]) + (fZec52[i] * ((((((fZec54[i] * fZec55[i]) * fZec56[i]) * fYec55[(((i + fYec55_idx) - iZec53[i]) & 1023)]) + (0.5f * (((fZec44[i] * fZec58[i]) * fZec59[i]) * fYec55[(((i + fYec55_idx) - iZec57[i]) & 1023)]))) + (0.166666672f * ((fZec60[i] * fZec62[i]) * fYec55[(((i + fYec55_idx) - iZec61[i]) & 1023)]))) + (0.0416666679f * (fZec63[i] * fYec55[(((i + fYec55_idx) - iZec64[i]) & 1023)])))))));
				fZec174[i] = ((fSlow93 * fRec14[i]) - (fSlow94 * fRec11[(i - 1)]));
				fZec175[i] = ((fSlow93 * fRec96[i]) - (fSlow94 * fRec12[(i - 1)]));
				fYec56[((i + fYec56_idx) & 16383)] = ((0.707106769f * fZec174[i]) - (0.707106769f * fZec175[i]));
				fYec57[i] = fYec56[(((i + fYec56_idx) - iZec91[i]) & 16383)];
				fRec13[i] = (fYec57[(i - 1)] - ((fZec93[i] * (fRec13[(i - 1)] - fYec57[i])) / fZec94[i]));
				fRec11[i] = fRec13[i];
				fYec58[((i + fYec58_idx) & 16383)] = ((0.707106769f * fZec174[i]) + (0.707106769f * fZec175[i]));
				fYec59[i] = fYec58[(((i + fYec58_idx) - std::min<int>(8192, std::max<int>(0, int(fZec176[i])))) & 16383)];
				fRec112[i] = (fYec59[(i - 1)] - (((fZec177[i] + (2.0f - fRec113[i])) * (fRec112[(i - 1)] - fYec59[i])) / (fRec113[i] - fZec177[i])));
				fRec12[i] = fRec112[i];
				fZec178[i] = ((fSlow93 * fRec11[(i - 1)]) + (fSlow94 * fRec14[i]));
				fZec179[i] = ((fSlow93 * fZec178[i]) - (fSlow94 * fRec8[(i - 1)]));
				fZec180[i] = ((fSlow93 * fRec12[(i - 1)]) + (fSlow94 * fRec96[i]));
				fZec181[i] = ((fSlow93 * fZec180[i]) - (fSlow94 * fRec9[(i - 1)]));
				fYec60[((i + fYec60_idx) & 16383)] = ((0.707106769f * fZec179[i]) - (0.707106769f * fZec181[i]));
				fYec61[i] = fYec60[(((i + fYec60_idx) - iZec111[i]) & 16383)];
				fRec10[i] = (fYec61[(i - 1)] - ((fZec113[i] * (fRec10[(i - 1)] - fYec61[i])) / fZec114[i]));
				fRec8[i] = fRec10[i];
				fYec62[((i + fYec62_idx) & 16383)] = ((0.707106769f * fZec179[i]) + (0.707106769f * fZec181[i]));
				fYec63[i] = fYec62[(((i + fYec62_idx) - iZec102[i]) & 16383)];
				fRec114[i] = (fYec63[(i - 1)] - ((fZec104[i] * (fRec114[(i - 1)] - fYec63[i])) / fZec105[i]));
				fRec9[i] = fRec114[i];
				fZec182[i] = ((fSlow93 * fRec8[(i - 1)]) + (fSlow94 * fZec178[i]));
				fZec183[i] = ((fSlow93 * fZec182[i]) - (fSlow94 * fRec5[(i - 1)]));
				fZec184[i] = ((fSlow93 * fRec9[(i - 1)]) + (fSlow94 * fZec180[i]));
				fZec185[i] = ((fSlow93 * fZec184[i]) - (fSlow94 * fRec6[(i - 1)]));
				fYec64[((i + fYec64_idx) & 16383)] = ((0.707106769f * fZec183[i]) - (0.707106769f * fZec185[i]));
				fYec65[i] = fYec64[(((i + fYec64_idx) - iZec116[i]) & 16383)];
				fRec7[i] = (fYec65[(i - 1)] - ((fZec118[i] * (fRec7[(i - 1)] - fYec65[i])) / fZec119[i]));
				fRec5[i] = fRec7[i];
				fYec66[((i + fYec66_idx) & 16383)] = ((0.707106769f * fZec183[i]) + (0.707106769f * fZec185[i]));
				fYec67[i] = fYec66[(((i + fYec66_idx) - std::min<int>(8192, std::max<int>(0, int(fZec186[i])))) & 16383)];
				fRec115[i] = (fYec67[(i - 1)] - (((fZec187[i] + (2.0f - fRec116[i])) * (fRec115[(i - 1)] - fYec67[i])) / (fRec116[i] - fZec187[i])));
				fRec6[i] = fRec115[i];
				fZec188[i] = ((fSlow93 * fRec5[(i - 1)]) + (fSlow94 * fZec182[i]));
				fZec189[i] = ((fSlow93 * fZec188[i]) - (fSlow94 * fRec2[(i - 1)]));
				fZec190[i] = ((fSlow93 * fRec6[(i - 1)]) + (fSlow94 * fZec184[i]));
				fZec191[i] = ((fSlow93 * fZec190[i]) - (fSlow94 * fRec3[(i - 1)]));
				fYec68[((i + fYec68_idx) & 16383)] = ((0.707106769f * fZec189[i]) - (0.707106769f * fZec191[i]));
				fYec69[i] = fYec68[(((i + fYec68_idx) - std::min<int>(8192, std::max<int>(0, int(fZec192[i])))) & 16383)];
				fRec4[i] = (fYec69[(i - 1)] - (((fZec193[i] + (2.0f - fRec117[i])) * (fRec4[(i - 1)] - fYec69[i])) / (fRec117[i] - fZec193[i])));
				fRec2[i] = fRec4[i];
				fYec70[((i + fYec70_idx) & 16383)] = ((0.707106769f * fZec189[i]) + (0.707106769f * fZec191[i]));
				fYec71[i] = fYec70[(((i + fYec70_idx) - iZec127[i]) & 16383)];
				fRec118[i] = (fYec71[(i - 1)] - ((fZec129[i] * (fRec118[(i - 1)] - fYec71[i])) / fZec130[i]));
				fRec3[i] = fRec118[i];
				fRec0[i] = ((fSlow93 * fRec2[(i - 1)]) + (fSlow94 * fZec188[i]));
				fRec1[i] = ((fSlow93 * fRec3[(i - 1)]) + (fSlow94 * fZec190[i]));
				
			}
			/* Post code */
			fYec70_idx_save = vsize;
			for (int j299 = 0; (j299 < 4); j299 = (j299 + 1)) {
				fYec71_perm[j299] = fYec71_tmp[(vsize + j299)];
				
			}
			for (int j301 = 0; (j301 < 4); j301 = (j301 + 1)) {
				fRec118_perm[j301] = fRec118_tmp[(vsize + j301)];
				
			}
			fYec68_idx_save = vsize;
			for (int j293 = 0; (j293 < 4); j293 = (j293 + 1)) {
				fYec69_perm[j293] = fYec69_tmp[(vsize + j293)];
				
			}
			fYec66_idx_save = vsize;
			for (int j285 = 0; (j285 < 4); j285 = (j285 + 1)) {
				fYec67_perm[j285] = fYec67_tmp[(vsize + j285)];
				
			}
			for (int j287 = 0; (j287 < 4); j287 = (j287 + 1)) {
				fRec115_perm[j287] = fRec115_tmp[(vsize + j287)];
				
			}
			fYec64_idx_save = vsize;
			for (int j277 = 0; (j277 < 4); j277 = (j277 + 1)) {
				fYec65_perm[j277] = fYec65_tmp[(vsize + j277)];
				
			}
			fYec62_idx_save = vsize;
			for (int j271 = 0; (j271 < 4); j271 = (j271 + 1)) {
				fYec63_perm[j271] = fYec63_tmp[(vsize + j271)];
				
			}
			for (int j273 = 0; (j273 < 4); j273 = (j273 + 1)) {
				fRec114_perm[j273] = fRec114_tmp[(vsize + j273)];
				
			}
			fYec60_idx_save = vsize;
			for (int j265 = 0; (j265 < 4); j265 = (j265 + 1)) {
				fYec61_perm[j265] = fYec61_tmp[(vsize + j265)];
				
			}
			fYec58_idx_save = vsize;
			for (int j259 = 0; (j259 < 4); j259 = (j259 + 1)) {
				fYec59_perm[j259] = fYec59_tmp[(vsize + j259)];
				
			}
			for (int j261 = 0; (j261 < 4); j261 = (j261 + 1)) {
				fRec112_perm[j261] = fRec112_tmp[(vsize + j261)];
				
			}
			fYec56_idx_save = vsize;
			for (int j251 = 0; (j251 < 4); j251 = (j251 + 1)) {
				fYec57_perm[j251] = fYec57_tmp[(vsize + j251)];
				
			}
			fYec55_idx_save = vsize;
			for (int j243 = 0; (j243 < 4); j243 = (j243 + 1)) {
				fRec111_perm[j243] = fRec111_tmp[(vsize + j243)];
				
			}
			for (int j245 = 0; (j245 < 4); j245 = (j245 + 1)) {
				fRec110_perm[j245] = fRec110_tmp[(vsize + j245)];
				
			}
			for (int j247 = 0; (j247 < 4); j247 = (j247 + 1)) {
				fRec109_perm[j247] = fRec109_tmp[(vsize + j247)];
				
			}
			for (int j235 = 0; (j235 < 4); j235 = (j235 + 1)) {
				fYec54_perm[j235] = fYec54_tmp[(vsize + j235)];
				
			}
			for (int j229 = 0; (j229 < 4); j229 = (j229 + 1)) {
				fRec108_perm[j229] = fRec108_tmp[(vsize + j229)];
				
			}
			for (int j231 = 0; (j231 < 4); j231 = (j231 + 1)) {
				fRec107_perm[j231] = fRec107_tmp[(vsize + j231)];
				
			}
			for (int j233 = 0; (j233 < 4); j233 = (j233 + 1)) {
				fRec106_perm[j233] = fRec106_tmp[(vsize + j233)];
				
			}
			for (int j237 = 0; (j237 < 4); j237 = (j237 + 1)) {
				fRec105_perm[j237] = fRec105_tmp[(vsize + j237)];
				
			}
			for (int j239 = 0; (j239 < 4); j239 = (j239 + 1)) {
				fRec104_perm[j239] = fRec104_tmp[(vsize + j239)];
				
			}
			for (int j241 = 0; (j241 < 4); j241 = (j241 + 1)) {
				fRec103_perm[j241] = fRec103_tmp[(vsize + j241)];
				
			}
			fYec51_idx_save = vsize;
			fYec52_idx_save = vsize;
			for (int j217 = 0; (j217 < 4); j217 = (j217 + 1)) {
				fYec53_perm[j217] = fYec53_tmp[(vsize + j217)];
				
			}
			for (int j219 = 0; (j219 < 4); j219 = (j219 + 1)) {
				fRec101_perm[j219] = fRec101_tmp[(vsize + j219)];
				
			}
			for (int j221 = 0; (j221 < 4); j221 = (j221 + 1)) {
				fRec100_perm[j221] = fRec100_tmp[(vsize + j221)];
				
			}
			for (int j223 = 0; (j223 < 4); j223 = (j223 + 1)) {
				fRec99_perm[j223] = fRec99_tmp[(vsize + j223)];
				
			}
			for (int j225 = 0; (j225 < 4); j225 = (j225 + 1)) {
				fRec98_perm[j225] = fRec98_tmp[(vsize + j225)];
				
			}
			for (int j227 = 0; (j227 < 4); j227 = (j227 + 1)) {
				fRec97_perm[j227] = fRec97_tmp[(vsize + j227)];
				
			}
			for (int j249 = 0; (j249 < 4); j249 = (j249 + 1)) {
				fRec96_perm[j249] = fRec96_tmp[(vsize + j249)];
				
			}
			fYec50_idx_save = vsize;
			for (int j207 = 0; (j207 < 4); j207 = (j207 + 1)) {
				fRec95_perm[j207] = fRec95_tmp[(vsize + j207)];
				
			}
			for (int j209 = 0; (j209 < 4); j209 = (j209 + 1)) {
				fRec94_perm[j209] = fRec94_tmp[(vsize + j209)];
				
			}
			for (int j211 = 0; (j211 < 4); j211 = (j211 + 1)) {
				fRec93_perm[j211] = fRec93_tmp[(vsize + j211)];
				
			}
			for (int j199 = 0; (j199 < 4); j199 = (j199 + 1)) {
				fYec49_perm[j199] = fYec49_tmp[(vsize + j199)];
				
			}
			for (int j193 = 0; (j193 < 4); j193 = (j193 + 1)) {
				fRec92_perm[j193] = fRec92_tmp[(vsize + j193)];
				
			}
			for (int j195 = 0; (j195 < 4); j195 = (j195 + 1)) {
				fRec91_perm[j195] = fRec91_tmp[(vsize + j195)];
				
			}
			for (int j197 = 0; (j197 < 4); j197 = (j197 + 1)) {
				fRec90_perm[j197] = fRec90_tmp[(vsize + j197)];
				
			}
			for (int j201 = 0; (j201 < 4); j201 = (j201 + 1)) {
				fRec89_perm[j201] = fRec89_tmp[(vsize + j201)];
				
			}
			for (int j203 = 0; (j203 < 4); j203 = (j203 + 1)) {
				fRec88_perm[j203] = fRec88_tmp[(vsize + j203)];
				
			}
			for (int j205 = 0; (j205 < 4); j205 = (j205 + 1)) {
				fRec87_perm[j205] = fRec87_tmp[(vsize + j205)];
				
			}
			fYec46_idx_save = vsize;
			fYec47_idx_save = vsize;
			for (int j181 = 0; (j181 < 4); j181 = (j181 + 1)) {
				fYec48_perm[j181] = fYec48_tmp[(vsize + j181)];
				
			}
			fYec44_idx_save = vsize;
			for (int j173 = 0; (j173 < 4); j173 = (j173 + 1)) {
				fYec45_perm[j173] = fYec45_tmp[(vsize + j173)];
				
			}
			for (int j175 = 0; (j175 < 4); j175 = (j175 + 1)) {
				fRec84_perm[j175] = fRec84_tmp[(vsize + j175)];
				
			}
			fYec42_idx_save = vsize;
			for (int j165 = 0; (j165 < 4); j165 = (j165 + 1)) {
				fYec43_perm[j165] = fYec43_tmp[(vsize + j165)];
				
			}
			fYec40_idx_save = vsize;
			for (int j157 = 0; (j157 < 4); j157 = (j157 + 1)) {
				fYec41_perm[j157] = fYec41_tmp[(vsize + j157)];
				
			}
			for (int j159 = 0; (j159 < 4); j159 = (j159 + 1)) {
				fRec81_perm[j159] = fRec81_tmp[(vsize + j159)];
				
			}
			fYec38_idx_save = vsize;
			for (int j149 = 0; (j149 < 4); j149 = (j149 + 1)) {
				fYec39_perm[j149] = fYec39_tmp[(vsize + j149)];
				
			}
			fYec36_idx_save = vsize;
			for (int j141 = 0; (j141 < 4); j141 = (j141 + 1)) {
				fYec37_perm[j141] = fYec37_tmp[(vsize + j141)];
				
			}
			for (int j143 = 0; (j143 < 4); j143 = (j143 + 1)) {
				fRec78_perm[j143] = fRec78_tmp[(vsize + j143)];
				
			}
			fYec34_idx_save = vsize;
			for (int j133 = 0; (j133 < 4); j133 = (j133 + 1)) {
				fYec35_perm[j133] = fYec35_tmp[(vsize + j133)];
				
			}
			fYec32_idx_save = vsize;
			for (int j125 = 0; (j125 < 4); j125 = (j125 + 1)) {
				fYec33_perm[j125] = fYec33_tmp[(vsize + j125)];
				
			}
			for (int j127 = 0; (j127 < 4); j127 = (j127 + 1)) {
				fRec75_perm[j127] = fRec75_tmp[(vsize + j127)];
				
			}
			fYec30_idx_save = vsize;
			for (int j117 = 0; (j117 < 4); j117 = (j117 + 1)) {
				fYec31_perm[j117] = fYec31_tmp[(vsize + j117)];
				
			}
			fYec28_idx_save = vsize;
			for (int j109 = 0; (j109 < 4); j109 = (j109 + 1)) {
				fYec29_perm[j109] = fYec29_tmp[(vsize + j109)];
				
			}
			for (int j111 = 0; (j111 < 4); j111 = (j111 + 1)) {
				fRec72_perm[j111] = fRec72_tmp[(vsize + j111)];
				
			}
			fYec26_idx_save = vsize;
			for (int j101 = 0; (j101 < 4); j101 = (j101 + 1)) {
				fYec27_perm[j101] = fYec27_tmp[(vsize + j101)];
				
			}
			fYec23_idx_save = vsize;
			fYec24_idx_save = vsize;
			for (int j95 = 0; (j95 < 4); j95 = (j95 + 1)) {
				fYec25_perm[j95] = fYec25_tmp[(vsize + j95)];
				
			}
			for (int j97 = 0; (j97 < 4); j97 = (j97 + 1)) {
				fRec69_perm[j97] = fRec69_tmp[(vsize + j97)];
				
			}
			fYec20_idx_save = vsize;
			fYec21_idx_save = vsize;
			for (int j89 = 0; (j89 < 4); j89 = (j89 + 1)) {
				fYec22_perm[j89] = fYec22_tmp[(vsize + j89)];
				
			}
			fYec18_idx_save = vsize;
			for (int j81 = 0; (j81 < 4); j81 = (j81 + 1)) {
				fYec19_perm[j81] = fYec19_tmp[(vsize + j81)];
				
			}
			for (int j83 = 0; (j83 < 4); j83 = (j83 + 1)) {
				fRec66_perm[j83] = fRec66_tmp[(vsize + j83)];
				
			}
			fYec16_idx_save = vsize;
			for (int j73 = 0; (j73 < 4); j73 = (j73 + 1)) {
				fYec17_perm[j73] = fYec17_tmp[(vsize + j73)];
				
			}
			fYec14_idx_save = vsize;
			for (int j65 = 0; (j65 < 4); j65 = (j65 + 1)) {
				fYec15_perm[j65] = fYec15_tmp[(vsize + j65)];
				
			}
			for (int j67 = 0; (j67 < 4); j67 = (j67 + 1)) {
				fRec63_perm[j67] = fRec63_tmp[(vsize + j67)];
				
			}
			fYec12_idx_save = vsize;
			for (int j57 = 0; (j57 < 4); j57 = (j57 + 1)) {
				fYec13_perm[j57] = fYec13_tmp[(vsize + j57)];
				
			}
			fYec10_idx_save = vsize;
			for (int j49 = 0; (j49 < 4); j49 = (j49 + 1)) {
				fYec11_perm[j49] = fYec11_tmp[(vsize + j49)];
				
			}
			for (int j51 = 0; (j51 < 4); j51 = (j51 + 1)) {
				fRec60_perm[j51] = fRec60_tmp[(vsize + j51)];
				
			}
			fYec8_idx_save = vsize;
			for (int j41 = 0; (j41 < 4); j41 = (j41 + 1)) {
				fYec9_perm[j41] = fYec9_tmp[(vsize + j41)];
				
			}
			fYec6_idx_save = vsize;
			for (int j33 = 0; (j33 < 4); j33 = (j33 + 1)) {
				fYec7_perm[j33] = fYec7_tmp[(vsize + j33)];
				
			}
			for (int j35 = 0; (j35 < 4); j35 = (j35 + 1)) {
				fRec57_perm[j35] = fRec57_tmp[(vsize + j35)];
				
			}
			fYec4_idx_save = vsize;
			for (int j25 = 0; (j25 < 4); j25 = (j25 + 1)) {
				fYec5_perm[j25] = fYec5_tmp[(vsize + j25)];
				
			}
			fYec2_idx_save = vsize;
			for (int j17 = 0; (j17 < 4); j17 = (j17 + 1)) {
				fYec3_perm[j17] = fYec3_tmp[(vsize + j17)];
				
			}
			for (int j19 = 0; (j19 < 4); j19 = (j19 + 1)) {
				fRec54_perm[j19] = fRec54_tmp[(vsize + j19)];
				
			}
			fYec0_idx_save = vsize;
			for (int j9 = 0; (j9 < 4); j9 = (j9 + 1)) {
				fYec1_perm[j9] = fYec1_tmp[(vsize + j9)];
				
			}
			for (int j11 = 0; (j11 < 4); j11 = (j11 + 1)) {
				fRec52_perm[j11] = fRec52_tmp[(vsize + j11)];
				
			}
			for (int j13 = 0; (j13 < 4); j13 = (j13 + 1)) {
				fRec50_perm[j13] = fRec50_tmp[(vsize + j13)];
				
			}
			for (int j21 = 0; (j21 < 4); j21 = (j21 + 1)) {
				fRec51_perm[j21] = fRec51_tmp[(vsize + j21)];
				
			}
			for (int j27 = 0; (j27 < 4); j27 = (j27 + 1)) {
				fRec49_perm[j27] = fRec49_tmp[(vsize + j27)];
				
			}
			for (int j29 = 0; (j29 < 4); j29 = (j29 + 1)) {
				fRec47_perm[j29] = fRec47_tmp[(vsize + j29)];
				
			}
			for (int j37 = 0; (j37 < 4); j37 = (j37 + 1)) {
				fRec48_perm[j37] = fRec48_tmp[(vsize + j37)];
				
			}
			for (int j43 = 0; (j43 < 4); j43 = (j43 + 1)) {
				fRec46_perm[j43] = fRec46_tmp[(vsize + j43)];
				
			}
			for (int j45 = 0; (j45 < 4); j45 = (j45 + 1)) {
				fRec44_perm[j45] = fRec44_tmp[(vsize + j45)];
				
			}
			for (int j53 = 0; (j53 < 4); j53 = (j53 + 1)) {
				fRec45_perm[j53] = fRec45_tmp[(vsize + j53)];
				
			}
			for (int j59 = 0; (j59 < 4); j59 = (j59 + 1)) {
				fRec43_perm[j59] = fRec43_tmp[(vsize + j59)];
				
			}
			for (int j61 = 0; (j61 < 4); j61 = (j61 + 1)) {
				fRec41_perm[j61] = fRec41_tmp[(vsize + j61)];
				
			}
			for (int j69 = 0; (j69 < 4); j69 = (j69 + 1)) {
				fRec42_perm[j69] = fRec42_tmp[(vsize + j69)];
				
			}
			for (int j75 = 0; (j75 < 4); j75 = (j75 + 1)) {
				fRec40_perm[j75] = fRec40_tmp[(vsize + j75)];
				
			}
			for (int j77 = 0; (j77 < 4); j77 = (j77 + 1)) {
				fRec38_perm[j77] = fRec38_tmp[(vsize + j77)];
				
			}
			for (int j85 = 0; (j85 < 4); j85 = (j85 + 1)) {
				fRec39_perm[j85] = fRec39_tmp[(vsize + j85)];
				
			}
			for (int j91 = 0; (j91 < 4); j91 = (j91 + 1)) {
				fRec37_perm[j91] = fRec37_tmp[(vsize + j91)];
				
			}
			for (int j103 = 0; (j103 < 4); j103 = (j103 + 1)) {
				fRec36_perm[j103] = fRec36_tmp[(vsize + j103)];
				
			}
			for (int j105 = 0; (j105 < 4); j105 = (j105 + 1)) {
				fRec34_perm[j105] = fRec34_tmp[(vsize + j105)];
				
			}
			for (int j113 = 0; (j113 < 4); j113 = (j113 + 1)) {
				fRec35_perm[j113] = fRec35_tmp[(vsize + j113)];
				
			}
			for (int j119 = 0; (j119 < 4); j119 = (j119 + 1)) {
				fRec33_perm[j119] = fRec33_tmp[(vsize + j119)];
				
			}
			for (int j121 = 0; (j121 < 4); j121 = (j121 + 1)) {
				fRec31_perm[j121] = fRec31_tmp[(vsize + j121)];
				
			}
			for (int j129 = 0; (j129 < 4); j129 = (j129 + 1)) {
				fRec32_perm[j129] = fRec32_tmp[(vsize + j129)];
				
			}
			for (int j135 = 0; (j135 < 4); j135 = (j135 + 1)) {
				fRec30_perm[j135] = fRec30_tmp[(vsize + j135)];
				
			}
			for (int j137 = 0; (j137 < 4); j137 = (j137 + 1)) {
				fRec28_perm[j137] = fRec28_tmp[(vsize + j137)];
				
			}
			for (int j145 = 0; (j145 < 4); j145 = (j145 + 1)) {
				fRec29_perm[j145] = fRec29_tmp[(vsize + j145)];
				
			}
			for (int j151 = 0; (j151 < 4); j151 = (j151 + 1)) {
				fRec27_perm[j151] = fRec27_tmp[(vsize + j151)];
				
			}
			for (int j153 = 0; (j153 < 4); j153 = (j153 + 1)) {
				fRec25_perm[j153] = fRec25_tmp[(vsize + j153)];
				
			}
			for (int j161 = 0; (j161 < 4); j161 = (j161 + 1)) {
				fRec26_perm[j161] = fRec26_tmp[(vsize + j161)];
				
			}
			for (int j167 = 0; (j167 < 4); j167 = (j167 + 1)) {
				fRec24_perm[j167] = fRec24_tmp[(vsize + j167)];
				
			}
			for (int j169 = 0; (j169 < 4); j169 = (j169 + 1)) {
				fRec22_perm[j169] = fRec22_tmp[(vsize + j169)];
				
			}
			for (int j177 = 0; (j177 < 4); j177 = (j177 + 1)) {
				fRec23_perm[j177] = fRec23_tmp[(vsize + j177)];
				
			}
			for (int j183 = 0; (j183 < 4); j183 = (j183 + 1)) {
				fRec21_perm[j183] = fRec21_tmp[(vsize + j183)];
				
			}
			for (int j185 = 0; (j185 < 4); j185 = (j185 + 1)) {
				fRec20_perm[j185] = fRec20_tmp[(vsize + j185)];
				
			}
			for (int j187 = 0; (j187 < 4); j187 = (j187 + 1)) {
				fRec19_perm[j187] = fRec19_tmp[(vsize + j187)];
				
			}
			for (int j189 = 0; (j189 < 4); j189 = (j189 + 1)) {
				fRec18_perm[j189] = fRec18_tmp[(vsize + j189)];
				
			}
			for (int j191 = 0; (j191 < 4); j191 = (j191 + 1)) {
				fRec17_perm[j191] = fRec17_tmp[(vsize + j191)];
				
			}
			for (int j213 = 0; (j213 < 4); j213 = (j213 + 1)) {
				fRec14_perm[j213] = fRec14_tmp[(vsize + j213)];
				
			}
			for (int j253 = 0; (j253 < 4); j253 = (j253 + 1)) {
				fRec13_perm[j253] = fRec13_tmp[(vsize + j253)];
				
			}
			for (int j255 = 0; (j255 < 4); j255 = (j255 + 1)) {
				fRec11_perm[j255] = fRec11_tmp[(vsize + j255)];
				
			}
			for (int j263 = 0; (j263 < 4); j263 = (j263 + 1)) {
				fRec12_perm[j263] = fRec12_tmp[(vsize + j263)];
				
			}
			for (int j267 = 0; (j267 < 4); j267 = (j267 + 1)) {
				fRec10_perm[j267] = fRec10_tmp[(vsize + j267)];
				
			}
			for (int j269 = 0; (j269 < 4); j269 = (j269 + 1)) {
				fRec8_perm[j269] = fRec8_tmp[(vsize + j269)];
				
			}
			for (int j275 = 0; (j275 < 4); j275 = (j275 + 1)) {
				fRec9_perm[j275] = fRec9_tmp[(vsize + j275)];
				
			}
			for (int j279 = 0; (j279 < 4); j279 = (j279 + 1)) {
				fRec7_perm[j279] = fRec7_tmp[(vsize + j279)];
				
			}
			for (int j281 = 0; (j281 < 4); j281 = (j281 + 1)) {
				fRec5_perm[j281] = fRec5_tmp[(vsize + j281)];
				
			}
			for (int j289 = 0; (j289 < 4); j289 = (j289 + 1)) {
				fRec6_perm[j289] = fRec6_tmp[(vsize + j289)];
				
			}
			for (int j295 = 0; (j295 < 4); j295 = (j295 + 1)) {
				fRec4_perm[j295] = fRec4_tmp[(vsize + j295)];
				
			}
			for (int j297 = 0; (j297 < 4); j297 = (j297 + 1)) {
				fRec2_perm[j297] = fRec2_tmp[(vsize + j297)];
				
			}
			for (int j303 = 0; (j303 < 4); j303 = (j303 + 1)) {
				fRec3_perm[j303] = fRec3_tmp[(vsize + j303)];
				
			}
			for (int j305 = 0; (j305 < 4); j305 = (j305 + 1)) {
				fRec0_perm[j305] = fRec0_tmp[(vsize + j305)];
				
			}
			for (int j307 = 0; (j307 < 4); j307 = (j307 + 1)) {
				fRec1_perm[j307] = fRec1_tmp[(vsize + j307)];
				
			}
			/* Vectorizable loop 170 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				output0[i] = FAUSTFLOAT(fRec0[i]);
				
			}
			/* Vectorizable loop 171 */
			/* Compute code */
			for (int i = 0; (i < vsize); i = (i + 1)) {
				output1[i] = FAUSTFLOAT(fRec1[i]);
				
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
