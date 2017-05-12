//----------------------------------------------------------
// name: "GreyholeRaw"
// version: "1.0"
// author: "Julian Parker, bug fixes by Till Bovermann"
// license: "GPL2+"
// copyright: "(c) Julian Parker 2013"
//
// Code generated with Faust 0.9.98 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with : "primes" */
#include <jprev.h>
#include <math.h>
// If other than 'faust2sc --prefix Faust' is used, sed this as well:
#if !defined(SC_FAUST_PREFIX)
# define SC_FAUST_PREFIX "Faust"
#endif

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

#include <map>
#include <string>
#include <string.h>
#include <SC_PlugIn.h>

#include <faust/dsp/dsp.h>
#include <faust/gui/UI.h>
#include <faust/misc.h>

using namespace std;

#if defined(__GNUC__) && __GNUC__ >= 4
# define FAUST_EXPORT __attribute__((visibility("default")))
#else
# define FAUST_EXPORT  SC_API_EXPORT
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
    { }

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
    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) { }
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) { }

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


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
  private:
	float 	fConst0;
	float 	fConst1;
	FAUSTFLOAT 	fslider0;
	float 	fVec0_perm[4];
	float 	fRec39_perm[4];
	int 	iVec1_perm[4];
	float 	fRec40_perm[4];
	FAUSTFLOAT 	fslider1;
	float 	fRec41_perm[4];
	float 	fRec42_perm[4];
	float 	fRec43_perm[4];
	float 	fRec44_perm[4];
	FAUSTFLOAT 	fslider2;
	float 	fRec45_perm[4];
	FAUSTFLOAT 	fslider3;
	float 	fVec2_perm[4];
	float 	fConst2;
	FAUSTFLOAT 	fslider4;
	float 	fVec3_perm[4];
	float 	fYec0[131072];
	int 	fYec0_idx;
	int 	fYec0_idx_save;
	FAUSTFLOAT 	fslider5;
	float 	fVec4_perm[4];
	float 	fYec1[131072];
	int 	fYec1_idx;
	int 	fYec1_idx_save;
	float 	fYec2[16384];
	int 	fYec2_idx;
	int 	fYec2_idx_save;
	float 	fRec38_perm[4];
	float 	fRec36_perm[4];
	float 	fRec47_perm[4];
	float 	fYec3[16384];
	int 	fYec3_idx;
	int 	fYec3_idx_save;
	float 	fRec46_perm[4];
	float 	fRec37_perm[4];
	float 	fRec48_perm[4];
	float 	fYec4[16384];
	int 	fYec4_idx;
	int 	fYec4_idx_save;
	float 	fRec35_perm[4];
	float 	fRec33_perm[4];
	float 	fRec50_perm[4];
	float 	fYec5[16384];
	int 	fYec5_idx;
	int 	fYec5_idx_save;
	float 	fRec49_perm[4];
	float 	fRec34_perm[4];
	float 	fRec51_perm[4];
	float 	fYec6[16384];
	int 	fYec6_idx;
	int 	fYec6_idx_save;
	float 	fRec32_perm[4];
	float 	fRec30_perm[4];
	float 	fRec53_perm[4];
	float 	fYec7[16384];
	int 	fYec7_idx;
	int 	fYec7_idx_save;
	float 	fRec52_perm[4];
	float 	fRec31_perm[4];
	float 	fRec54_perm[4];
	float 	fYec8[16384];
	int 	fYec8_idx;
	int 	fYec8_idx_save;
	float 	fRec29_perm[4];
	float 	fRec27_perm[4];
	float 	fRec56_perm[4];
	float 	fYec9[16384];
	int 	fYec9_idx;
	int 	fYec9_idx_save;
	float 	fRec55_perm[4];
	float 	fRec28_perm[4];
	float 	fRec57_perm[4];
	float 	fYec10[16384];
	int 	fYec10_idx;
	int 	fYec10_idx_save;
	float 	fRec26_perm[4];
	float 	fRec24_perm[4];
	float 	fRec59_perm[4];
	float 	fYec11[16384];
	int 	fYec11_idx;
	int 	fYec11_idx_save;
	float 	fRec58_perm[4];
	float 	fRec25_perm[4];
	float 	fRec60_perm[4];
	float 	fYec12[16384];
	int 	fYec12_idx;
	int 	fYec12_idx_save;
	float 	fRec23_perm[4];
	float 	fRec21_perm[4];
	float 	fRec62_perm[4];
	float 	fYec13[16384];
	int 	fYec13_idx;
	int 	fYec13_idx_save;
	float 	fRec61_perm[4];
	float 	fRec22_perm[4];
	float 	fRec63_perm[4];
	float 	fYec14[16384];
	int 	fYec14_idx;
	int 	fYec14_idx_save;
	float 	fRec20_perm[4];
	float 	fRec18_perm[4];
	float 	fRec65_perm[4];
	float 	fYec15[16384];
	int 	fYec15_idx;
	int 	fYec15_idx_save;
	float 	fRec64_perm[4];
	float 	fRec19_perm[4];
	float 	fRec66_perm[4];
	float 	fYec16[16384];
	int 	fYec16_idx;
	int 	fYec16_idx_save;
	float 	fRec17_perm[4];
	float 	fRec15_perm[4];
	float 	fRec68_perm[4];
	float 	fYec17[16384];
	int 	fYec17_idx;
	int 	fYec17_idx_save;
	float 	fRec67_perm[4];
	float 	fRec16_perm[4];
	float 	fRec69_perm[4];
	float 	fYec18[16384];
	int 	fYec18_idx;
	int 	fYec18_idx_save;
	float 	fRec14_perm[4];
	float 	fRec12_perm[4];
	float 	fRec71_perm[4];
	float 	fYec19[16384];
	int 	fYec19_idx;
	int 	fYec19_idx_save;
	float 	fRec70_perm[4];
	float 	fRec13_perm[4];
	float 	fRec72_perm[4];
	float 	fYec20[16384];
	int 	fYec20_idx;
	int 	fYec20_idx_save;
	float 	fRec11_perm[4];
	float 	fRec9_perm[4];
	float 	fRec74_perm[4];
	float 	fYec21[16384];
	int 	fYec21_idx;
	int 	fYec21_idx_save;
	float 	fRec73_perm[4];
	float 	fRec10_perm[4];
	float 	fRec75_perm[4];
	float 	fYec22[16384];
	int 	fYec22_idx;
	int 	fYec22_idx_save;
	float 	fRec8_perm[4];
	float 	fRec6_perm[4];
	float 	fRec77_perm[4];
	float 	fYec23[16384];
	int 	fYec23_idx;
	int 	fYec23_idx_save;
	float 	fRec76_perm[4];
	float 	fRec7_perm[4];
	float 	fRec78_perm[4];
	float 	fYec24[16384];
	int 	fYec24_idx;
	int 	fYec24_idx_save;
	float 	fRec5_perm[4];
	float 	fRec3_perm[4];
	float 	fRec80_perm[4];
	float 	fYec25[16384];
	int 	fYec25_idx;
	int 	fYec25_idx_save;
	float 	fRec79_perm[4];
	float 	fRec4_perm[4];
	FAUSTFLOAT 	fslider6;
	float 	fVec5_perm[4];
	float 	fRec2_perm[4];
	float 	fRec0[1024];
	int 	fRec0_idx;
	int 	fRec0_idx_save;
	float 	fRec81_perm[4];
	float 	fRec1[1024];
	int 	fRec1_idx;
	int 	fRec1_idx_save;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("name", "GreyholeRaw");
		m->declare("version", "1.0");
		m->declare("author", "Julian Parker, bug fixes by Till Bovermann");
		m->declare("license", "GPL2+");
		m->declare("copyright", "(c) Julian Parker 2013");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.0");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.0");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
	}

	virtual int getNumInputs() { return 2; }
	virtual int getNumOutputs() { return 2; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(1.92e+05f, max(1e+03f, (float)fSamplingFreq));
		fConst1 = (3.1415927f / fConst0);
		fConst2 = (0.00056689343f * fConst0);
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 2.0f;
		fslider1 = 0.2f;
		fslider2 = 1.0f;
		fslider3 = 0.5f;
		fslider4 = 0.1f;
		fslider5 = 0.9f;
		fslider6 = 0.0f;
	}
	virtual void instanceClear() {
		for (int i=0; i<4; i++) fVec0_perm[i]=0;
		for (int i=0; i<4; i++) fRec39_perm[i]=0;
		for (int i=0; i<4; i++) iVec1_perm[i]=0;
		for (int i=0; i<4; i++) fRec40_perm[i]=0;
		for (int i=0; i<4; i++) fRec41_perm[i]=0;
		for (int i=0; i<4; i++) fRec42_perm[i]=0;
		for (int i=0; i<4; i++) fRec43_perm[i]=0;
		for (int i=0; i<4; i++) fRec44_perm[i]=0;
		for (int i=0; i<4; i++) fRec45_perm[i]=0;
		for (int i=0; i<4; i++) fVec2_perm[i]=0;
		for (int i=0; i<4; i++) fVec3_perm[i]=0;
		for (int i=0; i<131072; i++) fYec0[i]=0;
		fYec0_idx = 0;
		fYec0_idx_save = 0;
		for (int i=0; i<4; i++) fVec4_perm[i]=0;
		for (int i=0; i<131072; i++) fYec1[i]=0;
		fYec1_idx = 0;
		fYec1_idx_save = 0;
		for (int i=0; i<16384; i++) fYec2[i]=0;
		fYec2_idx = 0;
		fYec2_idx_save = 0;
		for (int i=0; i<4; i++) fRec38_perm[i]=0;
		for (int i=0; i<4; i++) fRec36_perm[i]=0;
		for (int i=0; i<4; i++) fRec47_perm[i]=0;
		for (int i=0; i<16384; i++) fYec3[i]=0;
		fYec3_idx = 0;
		fYec3_idx_save = 0;
		for (int i=0; i<4; i++) fRec46_perm[i]=0;
		for (int i=0; i<4; i++) fRec37_perm[i]=0;
		for (int i=0; i<4; i++) fRec48_perm[i]=0;
		for (int i=0; i<16384; i++) fYec4[i]=0;
		fYec4_idx = 0;
		fYec4_idx_save = 0;
		for (int i=0; i<4; i++) fRec35_perm[i]=0;
		for (int i=0; i<4; i++) fRec33_perm[i]=0;
		for (int i=0; i<4; i++) fRec50_perm[i]=0;
		for (int i=0; i<16384; i++) fYec5[i]=0;
		fYec5_idx = 0;
		fYec5_idx_save = 0;
		for (int i=0; i<4; i++) fRec49_perm[i]=0;
		for (int i=0; i<4; i++) fRec34_perm[i]=0;
		for (int i=0; i<4; i++) fRec51_perm[i]=0;
		for (int i=0; i<16384; i++) fYec6[i]=0;
		fYec6_idx = 0;
		fYec6_idx_save = 0;
		for (int i=0; i<4; i++) fRec32_perm[i]=0;
		for (int i=0; i<4; i++) fRec30_perm[i]=0;
		for (int i=0; i<4; i++) fRec53_perm[i]=0;
		for (int i=0; i<16384; i++) fYec7[i]=0;
		fYec7_idx = 0;
		fYec7_idx_save = 0;
		for (int i=0; i<4; i++) fRec52_perm[i]=0;
		for (int i=0; i<4; i++) fRec31_perm[i]=0;
		for (int i=0; i<4; i++) fRec54_perm[i]=0;
		for (int i=0; i<16384; i++) fYec8[i]=0;
		fYec8_idx = 0;
		fYec8_idx_save = 0;
		for (int i=0; i<4; i++) fRec29_perm[i]=0;
		for (int i=0; i<4; i++) fRec27_perm[i]=0;
		for (int i=0; i<4; i++) fRec56_perm[i]=0;
		for (int i=0; i<16384; i++) fYec9[i]=0;
		fYec9_idx = 0;
		fYec9_idx_save = 0;
		for (int i=0; i<4; i++) fRec55_perm[i]=0;
		for (int i=0; i<4; i++) fRec28_perm[i]=0;
		for (int i=0; i<4; i++) fRec57_perm[i]=0;
		for (int i=0; i<16384; i++) fYec10[i]=0;
		fYec10_idx = 0;
		fYec10_idx_save = 0;
		for (int i=0; i<4; i++) fRec26_perm[i]=0;
		for (int i=0; i<4; i++) fRec24_perm[i]=0;
		for (int i=0; i<4; i++) fRec59_perm[i]=0;
		for (int i=0; i<16384; i++) fYec11[i]=0;
		fYec11_idx = 0;
		fYec11_idx_save = 0;
		for (int i=0; i<4; i++) fRec58_perm[i]=0;
		for (int i=0; i<4; i++) fRec25_perm[i]=0;
		for (int i=0; i<4; i++) fRec60_perm[i]=0;
		for (int i=0; i<16384; i++) fYec12[i]=0;
		fYec12_idx = 0;
		fYec12_idx_save = 0;
		for (int i=0; i<4; i++) fRec23_perm[i]=0;
		for (int i=0; i<4; i++) fRec21_perm[i]=0;
		for (int i=0; i<4; i++) fRec62_perm[i]=0;
		for (int i=0; i<16384; i++) fYec13[i]=0;
		fYec13_idx = 0;
		fYec13_idx_save = 0;
		for (int i=0; i<4; i++) fRec61_perm[i]=0;
		for (int i=0; i<4; i++) fRec22_perm[i]=0;
		for (int i=0; i<4; i++) fRec63_perm[i]=0;
		for (int i=0; i<16384; i++) fYec14[i]=0;
		fYec14_idx = 0;
		fYec14_idx_save = 0;
		for (int i=0; i<4; i++) fRec20_perm[i]=0;
		for (int i=0; i<4; i++) fRec18_perm[i]=0;
		for (int i=0; i<4; i++) fRec65_perm[i]=0;
		for (int i=0; i<16384; i++) fYec15[i]=0;
		fYec15_idx = 0;
		fYec15_idx_save = 0;
		for (int i=0; i<4; i++) fRec64_perm[i]=0;
		for (int i=0; i<4; i++) fRec19_perm[i]=0;
		for (int i=0; i<4; i++) fRec66_perm[i]=0;
		for (int i=0; i<16384; i++) fYec16[i]=0;
		fYec16_idx = 0;
		fYec16_idx_save = 0;
		for (int i=0; i<4; i++) fRec17_perm[i]=0;
		for (int i=0; i<4; i++) fRec15_perm[i]=0;
		for (int i=0; i<4; i++) fRec68_perm[i]=0;
		for (int i=0; i<16384; i++) fYec17[i]=0;
		fYec17_idx = 0;
		fYec17_idx_save = 0;
		for (int i=0; i<4; i++) fRec67_perm[i]=0;
		for (int i=0; i<4; i++) fRec16_perm[i]=0;
		for (int i=0; i<4; i++) fRec69_perm[i]=0;
		for (int i=0; i<16384; i++) fYec18[i]=0;
		fYec18_idx = 0;
		fYec18_idx_save = 0;
		for (int i=0; i<4; i++) fRec14_perm[i]=0;
		for (int i=0; i<4; i++) fRec12_perm[i]=0;
		for (int i=0; i<4; i++) fRec71_perm[i]=0;
		for (int i=0; i<16384; i++) fYec19[i]=0;
		fYec19_idx = 0;
		fYec19_idx_save = 0;
		for (int i=0; i<4; i++) fRec70_perm[i]=0;
		for (int i=0; i<4; i++) fRec13_perm[i]=0;
		for (int i=0; i<4; i++) fRec72_perm[i]=0;
		for (int i=0; i<16384; i++) fYec20[i]=0;
		fYec20_idx = 0;
		fYec20_idx_save = 0;
		for (int i=0; i<4; i++) fRec11_perm[i]=0;
		for (int i=0; i<4; i++) fRec9_perm[i]=0;
		for (int i=0; i<4; i++) fRec74_perm[i]=0;
		for (int i=0; i<16384; i++) fYec21[i]=0;
		fYec21_idx = 0;
		fYec21_idx_save = 0;
		for (int i=0; i<4; i++) fRec73_perm[i]=0;
		for (int i=0; i<4; i++) fRec10_perm[i]=0;
		for (int i=0; i<4; i++) fRec75_perm[i]=0;
		for (int i=0; i<16384; i++) fYec22[i]=0;
		fYec22_idx = 0;
		fYec22_idx_save = 0;
		for (int i=0; i<4; i++) fRec8_perm[i]=0;
		for (int i=0; i<4; i++) fRec6_perm[i]=0;
		for (int i=0; i<4; i++) fRec77_perm[i]=0;
		for (int i=0; i<16384; i++) fYec23[i]=0;
		fYec23_idx = 0;
		fYec23_idx_save = 0;
		for (int i=0; i<4; i++) fRec76_perm[i]=0;
		for (int i=0; i<4; i++) fRec7_perm[i]=0;
		for (int i=0; i<4; i++) fRec78_perm[i]=0;
		for (int i=0; i<16384; i++) fYec24[i]=0;
		fYec24_idx = 0;
		fYec24_idx_save = 0;
		for (int i=0; i<4; i++) fRec5_perm[i]=0;
		for (int i=0; i<4; i++) fRec3_perm[i]=0;
		for (int i=0; i<4; i++) fRec80_perm[i]=0;
		for (int i=0; i<16384; i++) fYec25[i]=0;
		fYec25_idx = 0;
		fYec25_idx_save = 0;
		for (int i=0; i<4; i++) fRec79_perm[i]=0;
		for (int i=0; i<4; i++) fRec4_perm[i]=0;
		for (int i=0; i<4; i++) fVec5_perm[i]=0;
		for (int i=0; i<4; i++) fRec2_perm[i]=0;
		for (int i=0; i<1024; i++) fRec0[i]=0;
		fRec0_idx = 0;
		fRec0_idx_save = 0;
		for (int i=0; i<4; i++) fRec81_perm[i]=0;
		for (int i=0; i<1024; i++) fRec1[i]=0;
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
		ui_interface->openVerticalBox("0x00");
		ui_interface->addHorizontalSlider("damping", &fslider6, 0.0f, 0.0f, 0.99f, 0.001f);
		ui_interface->addHorizontalSlider("delayTime", &fslider1, 0.2f, 0.001f, 1.45f, 0.0001f);
		ui_interface->addHorizontalSlider("diffusion", &fslider3, 0.5f, 0.0f, 0.99f, 0.0001f);
		ui_interface->addHorizontalSlider("feedback", &fslider5, 0.9f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("modDepth", &fslider4, 0.1f, 0.0f, 1.0f, 0.001f);
		ui_interface->addHorizontalSlider("modFreq", &fslider0, 2.0f, 0.0f, 1e+01f, 0.01f);
		ui_interface->addHorizontalSlider("size", &fslider2, 1.0f, 0.5f, 3.0f, 0.0001f);
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fVec0_tmp[8+4];
		float 	fZec0[8];
		float 	fZec1[8];
		float 	fZec2[8];
		float 	fRec39_tmp[8+4];
		int 	iVec1_tmp[8+4];
		int 	iZec3[8];
		float 	fRec40_tmp[8+4];
		float 	fZec4[8];
		float 	fRec41_tmp[8+4];
		float 	fRec42_tmp[8+4];
		float 	fRec43_tmp[8+4];
		float 	fRec44_tmp[8+4];
		float 	fRec45_tmp[8+4];
		float 	fVec2_tmp[8+4];
		float 	fZec5[8];
		float 	fZec6[8];
		float 	fZec7[8];
		float 	fZec8[8];
		float 	fVec3_tmp[8+4];
		float 	fZec9[8];
		float 	fZec10[8];
		float 	fZec11[8];
		int 	iZec12[8];
		float 	fZec13[8];
		float 	fZec14[8];
		float 	fZec15[8];
		float 	fZec16[8];
		float 	fZec17[8];
		float 	fZec18[8];
		int 	iZec19[8];
		float 	fZec20[8];
		int 	iZec21[8];
		float 	fVec4_tmp[8+4];
		float 	fZec22[8];
		float 	fZec23[8];
		float 	fZec24[8];
		float 	fZec25[8];
		float 	fZec26[8];
		float 	fZec27[8];
		float 	fZec28[8];
		float 	fZec29[8];
		int 	iZec30[8];
		float 	fZec31[8];
		float 	fZec32[8];
		float 	fZec33[8];
		float 	fZec34[8];
		float 	fZec35[8];
		float 	fZec36[8];
		float 	fZec37[8];
		float 	fZec38[8];
		float 	fZec39[8];
		float 	fZec40[8];
		float 	fZec41[8];
		float 	fZec42[8];
		int 	iZec43[8];
		float 	fZec44[8];
		float 	fZec45[8];
		float 	fZec46[8];
		float 	fRec38_tmp[8+4];
		float 	fRec36_tmp[8+4];
		float 	fRec47_tmp[8+4];
		float 	fZec47[8];
		int 	iZec48[8];
		float 	fZec49[8];
		float 	fZec50[8];
		float 	fZec51[8];
		float 	fRec46_tmp[8+4];
		float 	fRec37_tmp[8+4];
		float 	fRec48_tmp[8+4];
		float 	fZec52[8];
		float 	fZec53[8];
		float 	fZec54[8];
		int 	iZec55[8];
		float 	fZec56[8];
		float 	fZec57[8];
		float 	fZec58[8];
		float 	fRec35_tmp[8+4];
		float 	fRec33_tmp[8+4];
		float 	fRec50_tmp[8+4];
		float 	fZec59[8];
		int 	iZec60[8];
		float 	fZec61[8];
		float 	fZec62[8];
		float 	fZec63[8];
		float 	fRec49_tmp[8+4];
		float 	fRec34_tmp[8+4];
		float 	fRec51_tmp[8+4];
		float 	fZec64[8];
		float 	fZec65[8];
		float 	fZec66[8];
		int 	iZec67[8];
		float 	fZec68[8];
		float 	fZec69[8];
		float 	fZec70[8];
		float 	fRec32_tmp[8+4];
		float 	fRec30_tmp[8+4];
		float 	fRec53_tmp[8+4];
		float 	fZec71[8];
		int 	iZec72[8];
		float 	fZec73[8];
		float 	fZec74[8];
		float 	fZec75[8];
		float 	fRec52_tmp[8+4];
		float 	fRec31_tmp[8+4];
		float 	fRec54_tmp[8+4];
		float 	fZec76[8];
		float 	fZec77[8];
		float 	fZec78[8];
		int 	iZec79[8];
		float 	fZec80[8];
		float 	fZec81[8];
		float 	fZec82[8];
		float 	fRec29_tmp[8+4];
		float 	fRec27_tmp[8+4];
		float 	fRec56_tmp[8+4];
		float 	fZec83[8];
		int 	iZec84[8];
		float 	fZec85[8];
		float 	fZec86[8];
		float 	fZec87[8];
		float 	fRec55_tmp[8+4];
		float 	fRec28_tmp[8+4];
		float 	fRec57_tmp[8+4];
		float 	fZec88[8];
		float 	fZec89[8];
		float 	fZec90[8];
		float 	fZec91[8];
		float 	fZec92[8];
		float 	fZec93[8];
		float 	fZec94[8];
		float 	fZec95[8];
		float 	fZec96[8];
		float 	fZec97[8];
		float 	fZec98[8];
		float 	fZec99[8];
		float 	fZec100[8];
		float 	fZec101[8];
		float 	fZec102[8];
		float 	fZec103[8];
		int 	iZec104[8];
		float 	fZec105[8];
		float 	fZec106[8];
		float 	fZec107[8];
		float 	fRec26_tmp[8+4];
		float 	fRec24_tmp[8+4];
		float 	fRec59_tmp[8+4];
		float 	fZec108[8];
		int 	iZec109[8];
		float 	fZec110[8];
		float 	fZec111[8];
		float 	fZec112[8];
		float 	fRec58_tmp[8+4];
		float 	fRec25_tmp[8+4];
		float 	fRec60_tmp[8+4];
		float 	fZec113[8];
		float 	fZec114[8];
		float 	fZec115[8];
		int 	iZec116[8];
		float 	fZec117[8];
		float 	fZec118[8];
		float 	fZec119[8];
		float 	fRec23_tmp[8+4];
		float 	fRec21_tmp[8+4];
		float 	fRec62_tmp[8+4];
		float 	fZec120[8];
		int 	iZec121[8];
		float 	fZec122[8];
		float 	fZec123[8];
		float 	fZec124[8];
		float 	fRec61_tmp[8+4];
		float 	fRec22_tmp[8+4];
		float 	fRec63_tmp[8+4];
		float 	fZec125[8];
		float 	fZec126[8];
		float 	fZec127[8];
		int 	iZec128[8];
		float 	fZec129[8];
		float 	fZec130[8];
		float 	fZec131[8];
		float 	fRec20_tmp[8+4];
		float 	fRec18_tmp[8+4];
		float 	fRec65_tmp[8+4];
		float 	fZec132[8];
		int 	iZec133[8];
		float 	fZec134[8];
		float 	fZec135[8];
		float 	fZec136[8];
		float 	fRec64_tmp[8+4];
		float 	fRec19_tmp[8+4];
		float 	fRec66_tmp[8+4];
		float 	fZec137[8];
		float 	fZec138[8];
		float 	fZec139[8];
		int 	iZec140[8];
		float 	fZec141[8];
		float 	fZec142[8];
		float 	fZec143[8];
		float 	fRec17_tmp[8+4];
		float 	fRec15_tmp[8+4];
		float 	fRec68_tmp[8+4];
		float 	fZec144[8];
		int 	iZec145[8];
		float 	fZec146[8];
		float 	fZec147[8];
		float 	fZec148[8];
		float 	fRec67_tmp[8+4];
		float 	fRec16_tmp[8+4];
		float 	fRec69_tmp[8+4];
		float 	fZec149[8];
		float 	fZec150[8];
		float 	fZec151[8];
		float 	fZec152[8];
		float 	fZec153[8];
		float 	fZec154[8];
		float 	fZec155[8];
		float 	fZec156[8];
		float 	fZec157[8];
		float 	fZec158[8];
		float 	fZec159[8];
		float 	fZec160[8];
		int 	iZec161[8];
		float 	fZec162[8];
		float 	fZec163[8];
		float 	fZec164[8];
		float 	fRec14_tmp[8+4];
		float 	fRec12_tmp[8+4];
		float 	fRec71_tmp[8+4];
		float 	fZec165[8];
		int 	iZec166[8];
		float 	fZec167[8];
		float 	fZec168[8];
		float 	fZec169[8];
		float 	fRec70_tmp[8+4];
		float 	fRec13_tmp[8+4];
		float 	fRec72_tmp[8+4];
		float 	fZec170[8];
		float 	fZec171[8];
		float 	fZec172[8];
		int 	iZec173[8];
		float 	fZec174[8];
		float 	fZec175[8];
		float 	fZec176[8];
		float 	fRec11_tmp[8+4];
		float 	fRec9_tmp[8+4];
		float 	fRec74_tmp[8+4];
		float 	fZec177[8];
		int 	iZec178[8];
		float 	fZec179[8];
		float 	fZec180[8];
		float 	fZec181[8];
		float 	fRec73_tmp[8+4];
		float 	fRec10_tmp[8+4];
		float 	fRec75_tmp[8+4];
		float 	fZec182[8];
		float 	fZec183[8];
		float 	fZec184[8];
		int 	iZec185[8];
		float 	fZec186[8];
		float 	fZec187[8];
		float 	fZec188[8];
		float 	fRec8_tmp[8+4];
		float 	fRec6_tmp[8+4];
		float 	fRec77_tmp[8+4];
		float 	fZec189[8];
		int 	iZec190[8];
		float 	fZec191[8];
		float 	fZec192[8];
		float 	fZec193[8];
		float 	fRec76_tmp[8+4];
		float 	fRec7_tmp[8+4];
		float 	fRec78_tmp[8+4];
		float 	fZec194[8];
		float 	fZec195[8];
		float 	fZec196[8];
		int 	iZec197[8];
		float 	fZec198[8];
		float 	fZec199[8];
		float 	fZec200[8];
		float 	fRec5_tmp[8+4];
		float 	fRec3_tmp[8+4];
		float 	fRec80_tmp[8+4];
		float 	fZec201[8];
		int 	iZec202[8];
		float 	fZec203[8];
		float 	fZec204[8];
		float 	fZec205[8];
		float 	fRec79_tmp[8+4];
		float 	fRec4_tmp[8+4];
		float 	fVec5_tmp[8+4];
		float 	fZec206[8];
		float 	fZec207[8];
		float 	fRec2_tmp[8+4];
		float 	fRec81_tmp[8+4];
		float 	fSlow0 = float(fslider0);
		float* 	fVec0 = &fVec0_tmp[4];
		float* 	fRec39 = &fRec39_tmp[4];
		int* 	iVec1 = &iVec1_tmp[4];
		float* 	fRec40 = &fRec40_tmp[4];
		float 	fSlow1 = floorf(min((float)65533, (fConst0 * float(fslider1))));
		float* 	fRec41 = &fRec41_tmp[4];
		float* 	fRec42 = &fRec42_tmp[4];
		float* 	fRec43 = &fRec43_tmp[4];
		float* 	fRec44 = &fRec44_tmp[4];
		float 	fSlow2 = float(fslider2);
		float 	fSlow3 = (49 * fSlow2);
		int 	iSlow4 = primes(fSlow3);
		float 	fSlow5 = (0.0001f * iSlow4);
		float* 	fRec45 = &fRec45_tmp[4];
		float 	fSlow6 = float(fslider3);
		float* 	fVec2 = &fVec2_tmp[4];
		float 	fSlow7 = float(fslider4);
		float* 	fVec3 = &fVec3_tmp[4];
		float 	fSlow8 = float(fslider5);
		float* 	fVec4 = &fVec4_tmp[4];
		float* 	fRec38 = &fRec38_tmp[4];
		float* 	fRec36 = &fRec36_tmp[4];
		int 	iSlow9 = primes((fSlow3 + 10));
		float 	fSlow10 = (0.0001f * iSlow9);
		float* 	fRec47 = &fRec47_tmp[4];
		float* 	fRec46 = &fRec46_tmp[4];
		float* 	fRec37 = &fRec37_tmp[4];
		float 	fSlow11 = (36 * fSlow2);
		int 	iSlow12 = primes(fSlow11);
		float 	fSlow13 = (0.001f * iSlow12);
		float* 	fRec48 = &fRec48_tmp[4];
		float* 	fRec35 = &fRec35_tmp[4];
		float* 	fRec33 = &fRec33_tmp[4];
		int 	iSlow14 = primes((fSlow11 + 10));
		float 	fSlow15 = (0.001f * iSlow14);
		float* 	fRec50 = &fRec50_tmp[4];
		float* 	fRec49 = &fRec49_tmp[4];
		float* 	fRec34 = &fRec34_tmp[4];
		float 	fSlow16 = (23 * fSlow2);
		int 	iSlow17 = primes(fSlow16);
		float 	fSlow18 = (0.001f * iSlow17);
		float* 	fRec51 = &fRec51_tmp[4];
		float* 	fRec32 = &fRec32_tmp[4];
		float* 	fRec30 = &fRec30_tmp[4];
		int 	iSlow19 = primes((fSlow16 + 10));
		float 	fSlow20 = (0.001f * iSlow19);
		float* 	fRec53 = &fRec53_tmp[4];
		float* 	fRec52 = &fRec52_tmp[4];
		float* 	fRec31 = &fRec31_tmp[4];
		int 	iSlow21 = primes((10 * fSlow2));
		float 	fSlow22 = (0.001f * iSlow21);
		float* 	fRec54 = &fRec54_tmp[4];
		float* 	fRec29 = &fRec29_tmp[4];
		float* 	fRec27 = &fRec27_tmp[4];
		int 	iSlow23 = primes((10 * (fSlow2 + 1)));
		float 	fSlow24 = (0.001f * iSlow23);
		float* 	fRec56 = &fRec56_tmp[4];
		float* 	fRec55 = &fRec55_tmp[4];
		float* 	fRec28 = &fRec28_tmp[4];
		float 	fSlow25 = (68 * fSlow2);
		int 	iSlow26 = primes(fSlow25);
		float 	fSlow27 = (0.0001f * iSlow26);
		float* 	fRec57 = &fRec57_tmp[4];
		float* 	fRec26 = &fRec26_tmp[4];
		float* 	fRec24 = &fRec24_tmp[4];
		int 	iSlow28 = primes((fSlow25 + 10));
		float 	fSlow29 = (0.0001f * iSlow28);
		float* 	fRec59 = &fRec59_tmp[4];
		float* 	fRec58 = &fRec58_tmp[4];
		float* 	fRec25 = &fRec25_tmp[4];
		float 	fSlow30 = (55 * fSlow2);
		int 	iSlow31 = primes(fSlow30);
		float 	fSlow32 = (0.001f * iSlow31);
		float* 	fRec60 = &fRec60_tmp[4];
		float* 	fRec23 = &fRec23_tmp[4];
		float* 	fRec21 = &fRec21_tmp[4];
		int 	iSlow33 = primes((fSlow30 + 10));
		float 	fSlow34 = (0.001f * iSlow33);
		float* 	fRec62 = &fRec62_tmp[4];
		float* 	fRec61 = &fRec61_tmp[4];
		float* 	fRec22 = &fRec22_tmp[4];
		float 	fSlow35 = (42 * fSlow2);
		int 	iSlow36 = primes(fSlow35);
		float 	fSlow37 = (0.001f * iSlow36);
		float* 	fRec63 = &fRec63_tmp[4];
		float* 	fRec20 = &fRec20_tmp[4];
		float* 	fRec18 = &fRec18_tmp[4];
		int 	iSlow38 = primes((fSlow35 + 10));
		float 	fSlow39 = (0.001f * iSlow38);
		float* 	fRec65 = &fRec65_tmp[4];
		float* 	fRec64 = &fRec64_tmp[4];
		float* 	fRec19 = &fRec19_tmp[4];
		float 	fSlow40 = (29 * fSlow2);
		int 	iSlow41 = primes(fSlow40);
		float 	fSlow42 = (0.001f * iSlow41);
		float* 	fRec66 = &fRec66_tmp[4];
		float* 	fRec17 = &fRec17_tmp[4];
		float* 	fRec15 = &fRec15_tmp[4];
		int 	iSlow43 = primes((fSlow40 + 10));
		float 	fSlow44 = (0.001f * iSlow43);
		float* 	fRec68 = &fRec68_tmp[4];
		float* 	fRec67 = &fRec67_tmp[4];
		float* 	fRec16 = &fRec16_tmp[4];
		float 	fSlow45 = (87 * fSlow2);
		int 	iSlow46 = primes(fSlow45);
		float 	fSlow47 = (0.0001f * iSlow46);
		float* 	fRec69 = &fRec69_tmp[4];
		float* 	fRec14 = &fRec14_tmp[4];
		float* 	fRec12 = &fRec12_tmp[4];
		int 	iSlow48 = primes((fSlow45 + 10));
		float 	fSlow49 = (0.0001f * iSlow48);
		float* 	fRec71 = &fRec71_tmp[4];
		float* 	fRec70 = &fRec70_tmp[4];
		float* 	fRec13 = &fRec13_tmp[4];
		float 	fSlow50 = (74 * fSlow2);
		int 	iSlow51 = primes(fSlow50);
		float 	fSlow52 = (0.001f * iSlow51);
		float* 	fRec72 = &fRec72_tmp[4];
		float* 	fRec11 = &fRec11_tmp[4];
		float* 	fRec9 = &fRec9_tmp[4];
		int 	iSlow53 = primes((fSlow50 + 10));
		float 	fSlow54 = (0.001f * iSlow53);
		float* 	fRec74 = &fRec74_tmp[4];
		float* 	fRec73 = &fRec73_tmp[4];
		float* 	fRec10 = &fRec10_tmp[4];
		float 	fSlow55 = (61 * fSlow2);
		int 	iSlow56 = primes(fSlow55);
		float 	fSlow57 = (0.001f * iSlow56);
		float* 	fRec75 = &fRec75_tmp[4];
		float* 	fRec8 = &fRec8_tmp[4];
		float* 	fRec6 = &fRec6_tmp[4];
		int 	iSlow58 = primes((fSlow55 + 10));
		float 	fSlow59 = (0.001f * iSlow58);
		float* 	fRec77 = &fRec77_tmp[4];
		float* 	fRec76 = &fRec76_tmp[4];
		float* 	fRec7 = &fRec7_tmp[4];
		float 	fSlow60 = (48 * fSlow2);
		int 	iSlow61 = primes(fSlow60);
		float 	fSlow62 = (0.001f * iSlow61);
		float* 	fRec78 = &fRec78_tmp[4];
		float* 	fRec5 = &fRec5_tmp[4];
		float* 	fRec3 = &fRec3_tmp[4];
		int 	iSlow63 = primes((fSlow60 + 10));
		float 	fSlow64 = (0.001f * iSlow63);
		float* 	fRec80 = &fRec80_tmp[4];
		float* 	fRec79 = &fRec79_tmp[4];
		float* 	fRec4 = &fRec4_tmp[4];
		float 	fSlow65 = float(fslider6);
		float* 	fVec5 = &fVec5_tmp[4];
		float* 	fRec2 = &fRec2_tmp[4];
		float* 	fRec81 = &fRec81_tmp[4];
		int index;
		int fullcount = count;
		for (index = 0; index <= fullcount - 8; index += 8) {
			// compute by blocks of 8 samples
			const int count = 8;
			FAUSTFLOAT* input0 = &input[0][index];
			FAUSTFLOAT* input1 = &input[1][index];
			FAUSTFLOAT* output0 = &output[0][index];
			FAUSTFLOAT* output1 = &output[1][index];
			// SECTION : 1
			// LOOP 0x7fedbd1afd00
			// pre processing
			for (int i=0; i<4; i++) fVec0_tmp[i]=fVec0_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fVec0[i] = fSlow0;
			}
			// post processing
			for (int i=0; i<4; i++) fVec0_perm[i]=fVec0_tmp[count+i];
			
			// SECTION : 2
			// LOOP 0x7fedbd1af500
			// exec code
			for (int i=0; i<count; i++) {
				fZec0[i] = (fConst1 * (fSlow0 + fVec0[i-1]));
			}
			
			// LOOP 0x7fedbd1b1ff0
			// pre processing
			for (int i=0; i<4; i++) iVec1_tmp[i]=iVec1_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				iVec1[i] = 1;
			}
			// post processing
			for (int i=0; i<4; i++) iVec1_perm[i]=iVec1_tmp[count+i];
			
			// SECTION : 3
			// LOOP 0x7fedbd1af280
			// exec code
			for (int i=0; i<count; i++) {
				fZec1[i] = sinf(fZec0[i]);
			}
			
			// LOOP 0x7fedbd1b10a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec2[i] = cosf(fZec0[i]);
			}
			
			// LOOP 0x7fedbd1b1ef0
			// exec code
			for (int i=0; i<count; i++) {
				iZec3[i] = (1 - iVec1[i-1]);
			}
			
			// LOOP 0x7fedbd1ba960
			// pre processing
			for (int i=0; i<4; i++) fVec3_tmp[i]=fVec3_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fVec3[i] = fSlow7;
			}
			// post processing
			for (int i=0; i<4; i++) fVec3_perm[i]=fVec3_tmp[count+i];
			
			// SECTION : 4
			// LOOP 0x7fedbd1af110
			// pre processing
			for (int i=0; i<4; i++) fRec39_tmp[i]=fRec39_perm[i];
			for (int i=0; i<4; i++) fRec40_tmp[i]=fRec40_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec39[i] = ((fZec1[i] * fRec40[i-1]) + (fZec2[i] * fRec39[i-1]));
				fRec40[i] = (((fZec2[i] * fRec40[i-1]) + ((0 - fZec1[i]) * fRec39[i-1])) + iZec3[i]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec40_perm[i]=fRec40_tmp[count+i];
			for (int i=0; i<4; i++) fRec39_perm[i]=fRec39_tmp[count+i];
			
			// LOOP 0x7fedbd1ba880
			// exec code
			for (int i=0; i<count; i++) {
				fZec9[i] = (fSlow7 + fVec3[i-1]);
			}
			
			// SECTION : 5
			// LOOP 0x7fedbd1b88d0
			// pre processing
			for (int i=0; i<4; i++) fVec2_tmp[i]=fVec2_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fVec2[i] = fSlow6;
			}
			// post processing
			for (int i=0; i<4; i++) fVec2_perm[i]=fVec2_tmp[count+i];
			
			// LOOP 0x7fedbd1ba510
			// exec code
			for (int i=0; i<count; i++) {
				fZec10[i] = (fConst2 * (fZec9[i] * (fRec40[i] + 1.0f)));
			}
			
			// LOOP 0x7fedbd1c5420
			// exec code
			for (int i=0; i<count; i++) {
				fZec28[i] = (fConst2 * (fZec9[i] * (fRec39[i] + 1.0f)));
			}
			
			// SECTION : 6
			// LOOP 0x7fedbd0be930
			// pre processing
			for (int i=0; i<4; i++) fRec47_tmp[i]=fRec47_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec47[i] = (fSlow10 + (0.9999f * ((iSlow9 * iZec3[i]) + fRec47[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec47_perm[i]=fRec47_tmp[count+i];
			
			// LOOP 0x7fedbd0cdef0
			// pre processing
			for (int i=0; i<4; i++) fRec59_tmp[i]=fRec59_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec59[i] = (fSlow29 + (0.9999f * ((iSlow28 * iZec3[i]) + fRec59[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec59_perm[i]=fRec59_tmp[count+i];
			
			// LOOP 0x7fedbd0d7540
			// pre processing
			for (int i=0; i<4; i++) fRec60_tmp[i]=fRec60_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec60[i] = (fSlow32 + (0.999f * ((iSlow31 * iZec3[i]) + fRec60[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec60_perm[i]=fRec60_tmp[count+i];
			
			// LOOP 0x7fedbd0e1950
			// pre processing
			for (int i=0; i<4; i++) fRec62_tmp[i]=fRec62_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec62[i] = (fSlow34 + (0.999f * ((iSlow33 * iZec3[i]) + fRec62[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec62_perm[i]=fRec62_tmp[count+i];
			
			// LOOP 0x7fedbd0eba60
			// pre processing
			for (int i=0; i<4; i++) fRec63_tmp[i]=fRec63_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec63[i] = (fSlow37 + (0.999f * ((iSlow36 * iZec3[i]) + fRec63[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec63_perm[i]=fRec63_tmp[count+i];
			
			// LOOP 0x7fedbd0f6910
			// pre processing
			for (int i=0; i<4; i++) fRec65_tmp[i]=fRec65_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec65[i] = (fSlow39 + (0.999f * ((iSlow38 * iZec3[i]) + fRec65[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec65_perm[i]=fRec65_tmp[count+i];
			
			// LOOP 0x7fedbd1b6e00
			// pre processing
			for (int i=0; i<4; i++) fRec45_tmp[i]=fRec45_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec45[i] = (fSlow5 + (0.9999f * ((iSlow4 * iZec3[i]) + fRec45[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec45_perm[i]=fRec45_tmp[count+i];
			
			// LOOP 0x7fedbd1b87c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec5[i] = (0.5f * (fSlow6 + fVec2[i-1]));
			}
			
			// LOOP 0x7fedbd1ba430
			// exec code
			for (int i=0; i<count; i++) {
				fZec11[i] = (fZec10[i] + 8.500005f);
			}
			
			// LOOP 0x7fedbd1c5340
			// exec code
			for (int i=0; i<count; i++) {
				fZec29[i] = (fZec28[i] + 8.500005f);
			}
			
			// LOOP 0x7fedbd1ceaa0
			// pre processing
			for (int i=0; i<4; i++) fRec48_tmp[i]=fRec48_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec48[i] = (fSlow13 + (0.999f * ((iSlow12 * iZec3[i]) + fRec48[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec48_perm[i]=fRec48_tmp[count+i];
			
			// LOOP 0x7fedbd1d5fc0
			// pre processing
			for (int i=0; i<4; i++) fRec50_tmp[i]=fRec50_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec50[i] = (fSlow15 + (0.999f * ((iSlow14 * iZec3[i]) + fRec50[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec50_perm[i]=fRec50_tmp[count+i];
			
			// LOOP 0x7fedbd1dcdc0
			// pre processing
			for (int i=0; i<4; i++) fRec51_tmp[i]=fRec51_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec51[i] = (fSlow18 + (0.999f * ((iSlow17 * iZec3[i]) + fRec51[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec51_perm[i]=fRec51_tmp[count+i];
			
			// LOOP 0x7fedbd1e4c00
			// pre processing
			for (int i=0; i<4; i++) fRec53_tmp[i]=fRec53_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec53[i] = (fSlow20 + (0.999f * ((iSlow19 * iZec3[i]) + fRec53[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec53_perm[i]=fRec53_tmp[count+i];
			
			// LOOP 0x7fedbd1ec010
			// pre processing
			for (int i=0; i<4; i++) fRec54_tmp[i]=fRec54_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec54[i] = (fSlow22 + (0.999f * ((iSlow21 * iZec3[i]) + fRec54[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec54_perm[i]=fRec54_tmp[count+i];
			
			// LOOP 0x7fedbd1f4780
			// pre processing
			for (int i=0; i<4; i++) fRec56_tmp[i]=fRec56_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec56[i] = (fSlow24 + (0.999f * ((iSlow23 * iZec3[i]) + fRec56[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec56_perm[i]=fRec56_tmp[count+i];
			
			// LOOP 0x7fedbd300d90
			// pre processing
			for (int i=0; i<4; i++) fRec57_tmp[i]=fRec57_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec57[i] = (fSlow27 + (0.9999f * ((iSlow26 * iZec3[i]) + fRec57[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec57_perm[i]=fRec57_tmp[count+i];
			
			// LOOP 0x7fedbd30e750
			// pre processing
			for (int i=0; i<4; i++) fRec69_tmp[i]=fRec69_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec69[i] = (fSlow47 + (0.9999f * ((iSlow46 * iZec3[i]) + fRec69[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec69_perm[i]=fRec69_tmp[count+i];
			
			// LOOP 0x7fedbd405580
			// pre processing
			for (int i=0; i<4; i++) fRec66_tmp[i]=fRec66_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec66[i] = (fSlow42 + (0.999f * ((iSlow41 * iZec3[i]) + fRec66[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec66_perm[i]=fRec66_tmp[count+i];
			
			// LOOP 0x7fedbd40cda0
			// pre processing
			for (int i=0; i<4; i++) fRec68_tmp[i]=fRec68_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec68[i] = (fSlow44 + (0.999f * ((iSlow43 * iZec3[i]) + fRec68[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec68_perm[i]=fRec68_tmp[count+i];
			
			// LOOP 0x7fedbd41d6d0
			// pre processing
			for (int i=0; i<4; i++) fRec71_tmp[i]=fRec71_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec71[i] = (fSlow49 + (0.9999f * ((iSlow48 * iZec3[i]) + fRec71[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec71_perm[i]=fRec71_tmp[count+i];
			
			// LOOP 0x7fedbd42a130
			// pre processing
			for (int i=0; i<4; i++) fRec72_tmp[i]=fRec72_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec72[i] = (fSlow52 + (0.999f * ((iSlow51 * iZec3[i]) + fRec72[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec72_perm[i]=fRec72_tmp[count+i];
			
			// LOOP 0x7fedbd437410
			// pre processing
			for (int i=0; i<4; i++) fRec74_tmp[i]=fRec74_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec74[i] = (fSlow54 + (0.999f * ((iSlow53 * iZec3[i]) + fRec74[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec74_perm[i]=fRec74_tmp[count+i];
			
			// LOOP 0x7fedbd444920
			// pre processing
			for (int i=0; i<4; i++) fRec75_tmp[i]=fRec75_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec75[i] = (fSlow57 + (0.999f * ((iSlow56 * iZec3[i]) + fRec75[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec75_perm[i]=fRec75_tmp[count+i];
			
			// LOOP 0x7fedbd4526b0
			// pre processing
			for (int i=0; i<4; i++) fRec77_tmp[i]=fRec77_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec77[i] = (fSlow59 + (0.999f * ((iSlow58 * iZec3[i]) + fRec77[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec77_perm[i]=fRec77_tmp[count+i];
			
			// LOOP 0x7fedbd460750
			// pre processing
			for (int i=0; i<4; i++) fRec78_tmp[i]=fRec78_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec78[i] = (fSlow62 + (0.999f * ((iSlow61 * iZec3[i]) + fRec78[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec78_perm[i]=fRec78_tmp[count+i];
			
			// LOOP 0x7fedbd46efa0
			// pre processing
			for (int i=0; i<4; i++) fRec80_tmp[i]=fRec80_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec80[i] = (fSlow64 + (0.999f * ((iSlow63 * iZec3[i]) + fRec80[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec80_perm[i]=fRec80_tmp[count+i];
			
			// SECTION : 7
			// LOOP 0x7fedbc67fdf0
			// exec code
			for (int i=0; i<count; i++) {
				fZec144[i] = (fRec68[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0ba340
			// exec code
			for (int i=0; i<count; i++) {
				fZec42[i] = (fRec45[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0c0000
			// exec code
			for (int i=0; i<count; i++) {
				fZec47[i] = (fRec47[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0c6a50
			// exec code
			for (int i=0; i<count; i++) {
				fZec103[i] = (fRec57[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0cf670
			// exec code
			for (int i=0; i<count; i++) {
				fZec108[i] = (fRec59[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0d9a70
			// exec code
			for (int i=0; i<count; i++) {
				fZec115[i] = (fRec60[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0e2f90
			// exec code
			for (int i=0; i<count; i++) {
				fZec120[i] = (fRec62[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0edf70
			// exec code
			for (int i=0; i<count; i++) {
				fZec127[i] = (fRec63[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0f7f50
			// exec code
			for (int i=0; i<count; i++) {
				fZec132[i] = (fRec65[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1bca60
			// exec code
			for (int i=0; i<count; i++) {
				fZec13[i] = floorf(fZec11[i]);
			}
			
			// LOOP 0x7fedbd1c6860
			// exec code
			for (int i=0; i<count; i++) {
				fZec31[i] = floorf(fZec29[i]);
			}
			
			// LOOP 0x7fedbd1d10e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec54[i] = (fRec48[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1d75f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec59[i] = (fRec50[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1df270
			// exec code
			for (int i=0; i<count; i++) {
				fZec66[i] = (fRec51[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1e5ce0
			// exec code
			for (int i=0; i<count; i++) {
				fZec71[i] = (fRec53[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1ee360
			// exec code
			for (int i=0; i<count; i++) {
				fZec78[i] = (fRec54[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1f5eb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec83[i] = (fRec56[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd302090
			// exec code
			for (int i=0; i<count; i++) {
				fZec88[i] = (0 - fZec5[i]);
			}
			
			// LOOP 0x7fedbd407a70
			// exec code
			for (int i=0; i<count; i++) {
				fZec139[i] = (fRec66[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd414ad0
			// exec code
			for (int i=0; i<count; i++) {
				fZec160[i] = (fRec69[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd41ee50
			// exec code
			for (int i=0; i<count; i++) {
				fZec165[i] = (fRec71[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd42c600
			// exec code
			for (int i=0; i<count; i++) {
				fZec172[i] = (fRec72[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd438a70
			// exec code
			for (int i=0; i<count; i++) {
				fZec177[i] = (fRec74[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd446e10
			// exec code
			for (int i=0; i<count; i++) {
				fZec184[i] = (fRec75[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd453cf0
			// exec code
			for (int i=0; i<count; i++) {
				fZec189[i] = (fRec77[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd462c40
			// exec code
			for (int i=0; i<count; i++) {
				fZec196[i] = (fRec78[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd4705e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec201[i] = (fRec80[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd47e030
			// pre processing
			for (int i=0; i<4; i++) fVec5_tmp[i]=fVec5_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fVec5[i] = fSlow65;
			}
			// post processing
			for (int i=0; i<4; i++) fVec5_perm[i]=fVec5_tmp[count+i];
			
			// SECTION : 8
			// LOOP 0x7fedbd0bb2c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec44[i] = floorf(fZec42[i]);
			}
			
			// LOOP 0x7fedbd0c0f00
			// exec code
			for (int i=0; i<count; i++) {
				fZec49[i] = floorf(fZec47[i]);
			}
			
			// LOOP 0x7fedbd0c3740
			// exec code
			for (int i=0; i<count; i++) {
				fZec91[i] = sinf(fZec88[i]);
			}
			
			// LOOP 0x7fedbd0c76a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec105[i] = floorf(fZec103[i]);
			}
			
			// LOOP 0x7fedbd0d05e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec110[i] = floorf(fZec108[i]);
			}
			
			// LOOP 0x7fedbd0da6c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec117[i] = floorf(fZec115[i]);
			}
			
			// LOOP 0x7fedbd0e3f00
			// exec code
			for (int i=0; i<count; i++) {
				fZec122[i] = floorf(fZec120[i]);
			}
			
			// LOOP 0x7fedbd0eebc0
			// exec code
			for (int i=0; i<count; i++) {
				fZec129[i] = floorf(fZec127[i]);
			}
			
			// LOOP 0x7fedbd0f8ec0
			// exec code
			for (int i=0; i<count; i++) {
				fZec134[i] = floorf(fZec132[i]);
			}
			
			// LOOP 0x7fedbd1b2de0
			// pre processing
			for (int i=0; i<4; i++) fRec41_tmp[i]=fRec41_perm[i];
			for (int i=0; i<4; i++) fRec42_tmp[i]=fRec42_perm[i];
			for (int i=0; i<4; i++) fRec43_tmp[i]=fRec43_perm[i];
			for (int i=0; i<4; i++) fRec44_tmp[i]=fRec44_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec4[i] = ((int((fRec41[i-1] != 0.0f)))?((int(((fRec42[i-1] > 0.0f) & (fRec42[i-1] < 1.0f))))?fRec41[i-1]:0):((int(((fRec42[i-1] == 0.0f) & (fSlow1 != fRec43[i-1]))))?4.5351473e-05f:((int(((fRec42[i-1] == 1.0f) & (fSlow1 != fRec44[i-1]))))?-4.5351473e-05f:0)));
				fRec41[i] = fZec4[i];
				fRec42[i] = max(0.0f, min(1.0f, (fRec42[i-1] + fZec4[i])));
				fRec43[i] = ((int(((fRec42[i-1] >= 1.0f) & (fRec44[i-1] != fSlow1))))?fSlow1:fRec43[i-1]);
				fRec44[i] = ((int(((fRec42[i-1] <= 0.0f) & (fRec43[i-1] != fSlow1))))?fSlow1:fRec44[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec44_perm[i]=fRec44_tmp[count+i];
			for (int i=0; i<4; i++) fRec43_perm[i]=fRec43_tmp[count+i];
			for (int i=0; i<4; i++) fRec42_perm[i]=fRec42_tmp[count+i];
			for (int i=0; i<4; i++) fRec41_perm[i]=fRec41_tmp[count+i];
			
			// LOOP 0x7fedbd1b86e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec6[i] = sinf(fZec5[i]);
			}
			
			// LOOP 0x7fedbd1bc810
			// exec code
			for (int i=0; i<count; i++) {
				fZec14[i] = (fZec10[i] + (9.0f - fZec13[i]));
			}
			
			// LOOP 0x7fedbd1bd520
			// exec code
			for (int i=0; i<count; i++) {
				fZec15[i] = (fZec10[i] + (8.0f - fZec13[i]));
			}
			
			// LOOP 0x7fedbd1c6780
			// exec code
			for (int i=0; i<count; i++) {
				fZec32[i] = (fZec28[i] + (9.0f - fZec31[i]));
			}
			
			// LOOP 0x7fedbd1c71f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec33[i] = (fZec28[i] + (8.0f - fZec31[i]));
			}
			
			// LOOP 0x7fedbd1d1f70
			// exec code
			for (int i=0; i<count; i++) {
				fZec56[i] = floorf(fZec54[i]);
			}
			
			// LOOP 0x7fedbd1d84e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec61[i] = floorf(fZec59[i]);
			}
			
			// LOOP 0x7fedbd1e0000
			// exec code
			for (int i=0; i<count; i++) {
				fZec68[i] = floorf(fZec66[i]);
			}
			
			// LOOP 0x7fedbd1e6b90
			// exec code
			for (int i=0; i<count; i++) {
				fZec73[i] = floorf(fZec71[i]);
			}
			
			// LOOP 0x7fedbd1eefa0
			// exec code
			for (int i=0; i<count; i++) {
				fZec80[i] = floorf(fZec78[i]);
			}
			
			// LOOP 0x7fedbd1f6db0
			// exec code
			for (int i=0; i<count; i++) {
				fZec85[i] = floorf(fZec83[i]);
			}
			
			// LOOP 0x7fedbd22df70
			// pre processing
			for (int i=0; i<4; i++) fVec4_tmp[i]=fVec4_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fVec4[i] = fSlow8;
			}
			// post processing
			for (int i=0; i<4; i++) fVec4_perm[i]=fVec4_tmp[count+i];
			
			// LOOP 0x7fedbd308000
			// exec code
			for (int i=0; i<count; i++) {
				fZec146[i] = floorf(fZec144[i]);
			}
			
			// LOOP 0x7fedbd4086c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec141[i] = floorf(fZec139[i]);
			}
			
			// LOOP 0x7fedbd4119e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec162[i] = floorf(fZec160[i]);
			}
			
			// LOOP 0x7fedbd41fdc0
			// exec code
			for (int i=0; i<count; i++) {
				fZec167[i] = floorf(fZec165[i]);
			}
			
			// LOOP 0x7fedbd42d250
			// exec code
			for (int i=0; i<count; i++) {
				fZec174[i] = floorf(fZec172[i]);
			}
			
			// LOOP 0x7fedbd4399e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec179[i] = floorf(fZec177[i]);
			}
			
			// LOOP 0x7fedbd447a60
			// exec code
			for (int i=0; i<count; i++) {
				fZec186[i] = floorf(fZec184[i]);
			}
			
			// LOOP 0x7fedbd454c60
			// exec code
			for (int i=0; i<count; i++) {
				fZec191[i] = floorf(fZec189[i]);
			}
			
			// LOOP 0x7fedbd463890
			// exec code
			for (int i=0; i<count; i++) {
				fZec198[i] = floorf(fZec196[i]);
			}
			
			// LOOP 0x7fedbd471550
			// exec code
			for (int i=0; i<count; i++) {
				fZec203[i] = floorf(fZec201[i]);
			}
			
			// LOOP 0x7fedbd47df50
			// exec code
			for (int i=0; i<count; i++) {
				fZec206[i] = (fSlow65 + fVec5[i-1]);
			}
			
			// SECTION : 9
			// LOOP 0x7fedbc6994d0
			// exec code
			for (int i=0; i<count; i++) {
				iZec145[i] = int(min(8192, max(0, int(fZec144[i]))));
			}
			
			// LOOP 0x7fedbd0b33c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec8[i] = cosf(fZec5[i]);
			}
			
			// LOOP 0x7fedbd0ba1f0
			// exec code
			for (int i=0; i<count; i++) {
				iZec43[i] = int(min(8192, max(0, int(fZec42[i]))));
			}
			
			// LOOP 0x7fedbd0bb120
			// exec code
			for (int i=0; i<count; i++) {
				fZec45[i] = (fZec44[i] + (2.0f - fRec45[i]));
			}
			
			// LOOP 0x7fedbd0bbbd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec46[i] = (fRec45[i] - fZec44[i]);
			}
			
			// LOOP 0x7fedbd0bff20
			// exec code
			for (int i=0; i<count; i++) {
				iZec48[i] = int(min(8192, max(0, int(fZec47[i]))));
			}
			
			// LOOP 0x7fedbd0c0d60
			// exec code
			for (int i=0; i<count; i++) {
				fZec50[i] = (fZec49[i] + (2.0f - fRec47[i]));
			}
			
			// LOOP 0x7fedbd0c1760
			// exec code
			for (int i=0; i<count; i++) {
				fZec51[i] = (fRec47[i] - fZec49[i]);
			}
			
			// LOOP 0x7fedbd0c3660
			// exec code
			for (int i=0; i<count; i++) {
				fZec92[i] = (0 - fZec91[i]);
			}
			
			// LOOP 0x7fedbd0c68f0
			// exec code
			for (int i=0; i<count; i++) {
				iZec104[i] = int(min(8192, max(0, int(fZec103[i]))));
			}
			
			// LOOP 0x7fedbd0c75c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec106[i] = (fZec105[i] + (2.0f - fRec57[i]));
			}
			
			// LOOP 0x7fedbd0c7fe0
			// exec code
			for (int i=0; i<count; i++) {
				fZec107[i] = (fRec57[i] - fZec105[i]);
			}
			
			// LOOP 0x7fedbd0cf590
			// exec code
			for (int i=0; i<count; i++) {
				iZec109[i] = int(min(8192, max(0, int(fZec108[i]))));
			}
			
			// LOOP 0x7fedbd0d0460
			// exec code
			for (int i=0; i<count; i++) {
				fZec111[i] = (fZec110[i] + (2.0f - fRec59[i]));
			}
			
			// LOOP 0x7fedbd0d0e30
			// exec code
			for (int i=0; i<count; i++) {
				fZec112[i] = (fRec59[i] - fZec110[i]);
			}
			
			// LOOP 0x7fedbd0d9990
			// exec code
			for (int i=0; i<count; i++) {
				iZec116[i] = int(min(8192, max(0, int(fZec115[i]))));
			}
			
			// LOOP 0x7fedbd0da5e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec118[i] = (fZec117[i] + (2.0f - fRec60[i]));
			}
			
			// LOOP 0x7fedbd0db030
			// exec code
			for (int i=0; i<count; i++) {
				fZec119[i] = (fRec60[i] - fZec117[i]);
			}
			
			// LOOP 0x7fedbd0e2eb0
			// exec code
			for (int i=0; i<count; i++) {
				iZec121[i] = int(min(8192, max(0, int(fZec120[i]))));
			}
			
			// LOOP 0x7fedbd0e3d80
			// exec code
			for (int i=0; i<count; i++) {
				fZec123[i] = (fZec122[i] + (2.0f - fRec62[i]));
			}
			
			// LOOP 0x7fedbd0e4750
			// exec code
			for (int i=0; i<count; i++) {
				fZec124[i] = (fRec62[i] - fZec122[i]);
			}
			
			// LOOP 0x7fedbd0ede90
			// exec code
			for (int i=0; i<count; i++) {
				iZec128[i] = int(min(8192, max(0, int(fZec127[i]))));
			}
			
			// LOOP 0x7fedbd0eeae0
			// exec code
			for (int i=0; i<count; i++) {
				fZec130[i] = (fZec129[i] + (2.0f - fRec63[i]));
			}
			
			// LOOP 0x7fedbd0ef530
			// exec code
			for (int i=0; i<count; i++) {
				fZec131[i] = (fRec63[i] - fZec129[i]);
			}
			
			// LOOP 0x7fedbd0f7e70
			// exec code
			for (int i=0; i<count; i++) {
				iZec133[i] = int(min(8192, max(0, int(fZec132[i]))));
			}
			
			// LOOP 0x7fedbd0f8d40
			// exec code
			for (int i=0; i<count; i++) {
				fZec135[i] = (fZec134[i] + (2.0f - fRec65[i]));
			}
			
			// LOOP 0x7fedbd0f9710
			// exec code
			for (int i=0; i<count; i++) {
				fZec136[i] = (fRec65[i] - fZec134[i]);
			}
			
			// LOOP 0x7fedbd1b8600
			// exec code
			for (int i=0; i<count; i++) {
				fZec7[i] = (0 - fZec6[i]);
			}
			
			// LOOP 0x7fedbd1ba350
			// exec code
			for (int i=0; i<count; i++) {
				iZec12[i] = int(fZec11[i]);
			}
			
			// LOOP 0x7fedbd1bdba0
			// exec code
			for (int i=0; i<count; i++) {
				fZec16[i] = (fZec10[i] + (7.0f - fZec13[i]));
			}
			
			// LOOP 0x7fedbd1be4d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec17[i] = (fZec10[i] + (6.0f - fZec13[i]));
			}
			
			// LOOP 0x7fedbd1c1280
			// exec code
			for (int i=0; i<count; i++) {
				fZec18[i] = (fZec14[i] * fZec15[i]);
			}
			
			// LOOP 0x7fedbd1c5260
			// exec code
			for (int i=0; i<count; i++) {
				iZec30[i] = int(fZec29[i]);
			}
			
			// LOOP 0x7fedbd1c79d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec34[i] = (fZec28[i] + (7.0f - fZec31[i]));
			}
			
			// LOOP 0x7fedbd1c8200
			// exec code
			for (int i=0; i<count; i++) {
				fZec35[i] = (fZec28[i] + (6.0f - fZec31[i]));
			}
			
			// LOOP 0x7fedbd1cab00
			// exec code
			for (int i=0; i<count; i++) {
				fZec36[i] = (fZec32[i] * fZec33[i]);
			}
			
			// LOOP 0x7fedbd1d0fc0
			// exec code
			for (int i=0; i<count; i++) {
				iZec55[i] = int(min(8192, max(0, int(fZec54[i]))));
			}
			
			// LOOP 0x7fedbd1d1df0
			// exec code
			for (int i=0; i<count; i++) {
				fZec57[i] = (fZec56[i] + (2.0f - fRec48[i]));
			}
			
			// LOOP 0x7fedbd1d27a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec58[i] = (fRec48[i] - fZec56[i]);
			}
			
			// LOOP 0x7fedbd1d74d0
			// exec code
			for (int i=0; i<count; i++) {
				iZec60[i] = int(min(8192, max(0, int(fZec59[i]))));
			}
			
			// LOOP 0x7fedbd1d8340
			// exec code
			for (int i=0; i<count; i++) {
				fZec62[i] = (fZec61[i] + (2.0f - fRec50[i]));
			}
			
			// LOOP 0x7fedbd1d8d40
			// exec code
			for (int i=0; i<count; i++) {
				fZec63[i] = (fRec50[i] - fZec61[i]);
			}
			
			// LOOP 0x7fedbd1df190
			// exec code
			for (int i=0; i<count; i++) {
				iZec67[i] = int(min(8192, max(0, int(fZec66[i]))));
			}
			
			// LOOP 0x7fedbd1dff20
			// exec code
			for (int i=0; i<count; i++) {
				fZec69[i] = (fZec68[i] + (2.0f - fRec51[i]));
			}
			
			// LOOP 0x7fedbd1e07f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec70[i] = (fRec51[i] - fZec68[i]);
			}
			
			// LOOP 0x7fedbd1e5c00
			// exec code
			for (int i=0; i<count; i++) {
				iZec72[i] = int(min(8192, max(0, int(fZec71[i]))));
			}
			
			// LOOP 0x7fedbd1e6a30
			// exec code
			for (int i=0; i<count; i++) {
				fZec74[i] = (fZec73[i] + (2.0f - fRec53[i]));
			}
			
			// LOOP 0x7fedbd1e73a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec75[i] = (fRec53[i] - fZec73[i]);
			}
			
			// LOOP 0x7fedbd1ee280
			// exec code
			for (int i=0; i<count; i++) {
				iZec79[i] = int(min(8192, max(0, int(fZec78[i]))));
			}
			
			// LOOP 0x7fedbd1eeec0
			// exec code
			for (int i=0; i<count; i++) {
				fZec81[i] = (fZec80[i] + (2.0f - fRec54[i]));
			}
			
			// LOOP 0x7fedbd1ef930
			// exec code
			for (int i=0; i<count; i++) {
				fZec82[i] = (fRec54[i] - fZec80[i]);
			}
			
			// LOOP 0x7fedbd1f5dd0
			// exec code
			for (int i=0; i<count; i++) {
				iZec84[i] = int(min(8192, max(0, int(fZec83[i]))));
			}
			
			// LOOP 0x7fedbd1f6c10
			// exec code
			for (int i=0; i<count; i++) {
				fZec86[i] = (fZec85[i] + (2.0f - fRec56[i]));
			}
			
			// LOOP 0x7fedbd1f7610
			// exec code
			for (int i=0; i<count; i++) {
				fZec87[i] = (fRec56[i] - fZec85[i]);
			}
			
			// LOOP 0x7fedbd22c640
			// exec code
			for (int i=0; i<count; i++) {
				iZec19[i] = int(min((float)65536, max((float)0, fRec43[i])));
			}
			
			// LOOP 0x7fedbd22cd80
			// exec code
			for (int i=0; i<count; i++) {
				fZec20[i] = (1.0f - fRec42[i]);
			}
			
			// LOOP 0x7fedbd22d4b0
			// exec code
			for (int i=0; i<count; i++) {
				iZec21[i] = int(min((float)65536, max((float)0, fRec44[i])));
			}
			
			// LOOP 0x7fedbd22de90
			// exec code
			for (int i=0; i<count; i++) {
				fZec22[i] = (fSlow8 + fVec4[i-1]);
			}
			
			// LOOP 0x7fedbd301fb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec89[i] = cosf(fZec88[i]);
			}
			
			// LOOP 0x7fedbd307f20
			// exec code
			for (int i=0; i<count; i++) {
				fZec147[i] = (fZec146[i] + (2.0f - fRec68[i]));
			}
			
			// LOOP 0x7fedbd308440
			// exec code
			for (int i=0; i<count; i++) {
				fZec148[i] = (fRec68[i] - fZec146[i]);
			}
			
			// LOOP 0x7fedbd407990
			// exec code
			for (int i=0; i<count; i++) {
				iZec140[i] = int(min(8192, max(0, int(fZec139[i]))));
			}
			
			// LOOP 0x7fedbd4085e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec142[i] = (fZec141[i] + (2.0f - fRec66[i]));
			}
			
			// LOOP 0x7fedbd409030
			// exec code
			for (int i=0; i<count; i++) {
				fZec143[i] = (fRec66[i] - fZec141[i]);
			}
			
			// LOOP 0x7fedbd411900
			// exec code
			for (int i=0; i<count; i++) {
				fZec163[i] = (fZec162[i] + (2.0f - fRec69[i]));
			}
			
			// LOOP 0x7fedbd4122a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec164[i] = (fRec69[i] - fZec162[i]);
			}
			
			// LOOP 0x7fedbd4148e0
			// exec code
			for (int i=0; i<count; i++) {
				iZec161[i] = int(min(8192, max(0, int(fZec160[i]))));
			}
			
			// LOOP 0x7fedbd41ed70
			// exec code
			for (int i=0; i<count; i++) {
				iZec166[i] = int(min(8192, max(0, int(fZec165[i]))));
			}
			
			// LOOP 0x7fedbd41fc40
			// exec code
			for (int i=0; i<count; i++) {
				fZec168[i] = (fZec167[i] + (2.0f - fRec71[i]));
			}
			
			// LOOP 0x7fedbd420610
			// exec code
			for (int i=0; i<count; i++) {
				fZec169[i] = (fRec71[i] - fZec167[i]);
			}
			
			// LOOP 0x7fedbd42c520
			// exec code
			for (int i=0; i<count; i++) {
				iZec173[i] = int(min(8192, max(0, int(fZec172[i]))));
			}
			
			// LOOP 0x7fedbd42d170
			// exec code
			for (int i=0; i<count; i++) {
				fZec175[i] = (fZec174[i] + (2.0f - fRec72[i]));
			}
			
			// LOOP 0x7fedbd42dbc0
			// exec code
			for (int i=0; i<count; i++) {
				fZec176[i] = (fRec72[i] - fZec174[i]);
			}
			
			// LOOP 0x7fedbd438990
			// exec code
			for (int i=0; i<count; i++) {
				iZec178[i] = int(min(8192, max(0, int(fZec177[i]))));
			}
			
			// LOOP 0x7fedbd439860
			// exec code
			for (int i=0; i<count; i++) {
				fZec180[i] = (fZec179[i] + (2.0f - fRec74[i]));
			}
			
			// LOOP 0x7fedbd43a230
			// exec code
			for (int i=0; i<count; i++) {
				fZec181[i] = (fRec74[i] - fZec179[i]);
			}
			
			// LOOP 0x7fedbd446d30
			// exec code
			for (int i=0; i<count; i++) {
				iZec185[i] = int(min(8192, max(0, int(fZec184[i]))));
			}
			
			// LOOP 0x7fedbd447980
			// exec code
			for (int i=0; i<count; i++) {
				fZec187[i] = (fZec186[i] + (2.0f - fRec75[i]));
			}
			
			// LOOP 0x7fedbd4483d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec188[i] = (fRec75[i] - fZec186[i]);
			}
			
			// LOOP 0x7fedbd453c10
			// exec code
			for (int i=0; i<count; i++) {
				iZec190[i] = int(min(8192, max(0, int(fZec189[i]))));
			}
			
			// LOOP 0x7fedbd454ae0
			// exec code
			for (int i=0; i<count; i++) {
				fZec192[i] = (fZec191[i] + (2.0f - fRec77[i]));
			}
			
			// LOOP 0x7fedbd4554b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec193[i] = (fRec77[i] - fZec191[i]);
			}
			
			// LOOP 0x7fedbd462b60
			// exec code
			for (int i=0; i<count; i++) {
				iZec197[i] = int(min(8192, max(0, int(fZec196[i]))));
			}
			
			// LOOP 0x7fedbd4637b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec199[i] = (fZec198[i] + (2.0f - fRec78[i]));
			}
			
			// LOOP 0x7fedbd464200
			// exec code
			for (int i=0; i<count; i++) {
				fZec200[i] = (fRec78[i] - fZec198[i]);
			}
			
			// LOOP 0x7fedbd470500
			// exec code
			for (int i=0; i<count; i++) {
				iZec202[i] = int(min(8192, max(0, int(fZec201[i]))));
			}
			
			// LOOP 0x7fedbd4713d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec204[i] = (fZec203[i] + (2.0f - fRec80[i]));
			}
			
			// LOOP 0x7fedbd471da0
			// exec code
			for (int i=0; i<count; i++) {
				fZec205[i] = (fRec80[i] - fZec203[i]);
			}
			
			// LOOP 0x7fedbd47de60
			// exec code
			for (int i=0; i<count; i++) {
				fZec207[i] = (1.0f - (0.5f * fZec206[i]));
			}
			
			// SECTION : 10
			// LOOP 0x7fedbd1aa2c0
			// pre processing
			fYec0_idx = (fYec0_idx+fYec0_idx_save)&131071;
			fYec1_idx = (fYec1_idx+fYec1_idx_save)&131071;
			fYec2_idx = (fYec2_idx+fYec2_idx_save)&16383;
			for (int i=0; i<4; i++) fRec38_tmp[i]=fRec38_perm[i];
			for (int i=0; i<4; i++) fRec36_tmp[i]=fRec36_perm[i];
			fYec3_idx = (fYec3_idx+fYec3_idx_save)&16383;
			for (int i=0; i<4; i++) fRec46_tmp[i]=fRec46_perm[i];
			for (int i=0; i<4; i++) fRec37_tmp[i]=fRec37_perm[i];
			fYec4_idx = (fYec4_idx+fYec4_idx_save)&16383;
			for (int i=0; i<4; i++) fRec35_tmp[i]=fRec35_perm[i];
			for (int i=0; i<4; i++) fRec33_tmp[i]=fRec33_perm[i];
			fYec5_idx = (fYec5_idx+fYec5_idx_save)&16383;
			for (int i=0; i<4; i++) fRec49_tmp[i]=fRec49_perm[i];
			for (int i=0; i<4; i++) fRec34_tmp[i]=fRec34_perm[i];
			fYec6_idx = (fYec6_idx+fYec6_idx_save)&16383;
			for (int i=0; i<4; i++) fRec32_tmp[i]=fRec32_perm[i];
			for (int i=0; i<4; i++) fRec30_tmp[i]=fRec30_perm[i];
			fYec7_idx = (fYec7_idx+fYec7_idx_save)&16383;
			for (int i=0; i<4; i++) fRec52_tmp[i]=fRec52_perm[i];
			for (int i=0; i<4; i++) fRec31_tmp[i]=fRec31_perm[i];
			fYec8_idx = (fYec8_idx+fYec8_idx_save)&16383;
			for (int i=0; i<4; i++) fRec29_tmp[i]=fRec29_perm[i];
			for (int i=0; i<4; i++) fRec27_tmp[i]=fRec27_perm[i];
			fYec9_idx = (fYec9_idx+fYec9_idx_save)&16383;
			for (int i=0; i<4; i++) fRec55_tmp[i]=fRec55_perm[i];
			for (int i=0; i<4; i++) fRec28_tmp[i]=fRec28_perm[i];
			fYec10_idx = (fYec10_idx+fYec10_idx_save)&16383;
			for (int i=0; i<4; i++) fRec26_tmp[i]=fRec26_perm[i];
			for (int i=0; i<4; i++) fRec24_tmp[i]=fRec24_perm[i];
			fYec11_idx = (fYec11_idx+fYec11_idx_save)&16383;
			for (int i=0; i<4; i++) fRec58_tmp[i]=fRec58_perm[i];
			for (int i=0; i<4; i++) fRec25_tmp[i]=fRec25_perm[i];
			fYec12_idx = (fYec12_idx+fYec12_idx_save)&16383;
			for (int i=0; i<4; i++) fRec23_tmp[i]=fRec23_perm[i];
			for (int i=0; i<4; i++) fRec21_tmp[i]=fRec21_perm[i];
			fYec13_idx = (fYec13_idx+fYec13_idx_save)&16383;
			for (int i=0; i<4; i++) fRec61_tmp[i]=fRec61_perm[i];
			for (int i=0; i<4; i++) fRec22_tmp[i]=fRec22_perm[i];
			fYec14_idx = (fYec14_idx+fYec14_idx_save)&16383;
			for (int i=0; i<4; i++) fRec20_tmp[i]=fRec20_perm[i];
			for (int i=0; i<4; i++) fRec18_tmp[i]=fRec18_perm[i];
			fYec15_idx = (fYec15_idx+fYec15_idx_save)&16383;
			for (int i=0; i<4; i++) fRec64_tmp[i]=fRec64_perm[i];
			for (int i=0; i<4; i++) fRec19_tmp[i]=fRec19_perm[i];
			fYec16_idx = (fYec16_idx+fYec16_idx_save)&16383;
			for (int i=0; i<4; i++) fRec17_tmp[i]=fRec17_perm[i];
			for (int i=0; i<4; i++) fRec15_tmp[i]=fRec15_perm[i];
			fYec17_idx = (fYec17_idx+fYec17_idx_save)&16383;
			for (int i=0; i<4; i++) fRec67_tmp[i]=fRec67_perm[i];
			for (int i=0; i<4; i++) fRec16_tmp[i]=fRec16_perm[i];
			fYec18_idx = (fYec18_idx+fYec18_idx_save)&16383;
			for (int i=0; i<4; i++) fRec14_tmp[i]=fRec14_perm[i];
			for (int i=0; i<4; i++) fRec12_tmp[i]=fRec12_perm[i];
			fYec19_idx = (fYec19_idx+fYec19_idx_save)&16383;
			for (int i=0; i<4; i++) fRec70_tmp[i]=fRec70_perm[i];
			for (int i=0; i<4; i++) fRec13_tmp[i]=fRec13_perm[i];
			fYec20_idx = (fYec20_idx+fYec20_idx_save)&16383;
			for (int i=0; i<4; i++) fRec11_tmp[i]=fRec11_perm[i];
			for (int i=0; i<4; i++) fRec9_tmp[i]=fRec9_perm[i];
			fYec21_idx = (fYec21_idx+fYec21_idx_save)&16383;
			for (int i=0; i<4; i++) fRec73_tmp[i]=fRec73_perm[i];
			for (int i=0; i<4; i++) fRec10_tmp[i]=fRec10_perm[i];
			fYec22_idx = (fYec22_idx+fYec22_idx_save)&16383;
			for (int i=0; i<4; i++) fRec8_tmp[i]=fRec8_perm[i];
			for (int i=0; i<4; i++) fRec6_tmp[i]=fRec6_perm[i];
			fYec23_idx = (fYec23_idx+fYec23_idx_save)&16383;
			for (int i=0; i<4; i++) fRec76_tmp[i]=fRec76_perm[i];
			for (int i=0; i<4; i++) fRec7_tmp[i]=fRec7_perm[i];
			fYec24_idx = (fYec24_idx+fYec24_idx_save)&16383;
			for (int i=0; i<4; i++) fRec5_tmp[i]=fRec5_perm[i];
			for (int i=0; i<4; i++) fRec3_tmp[i]=fRec3_perm[i];
			fYec25_idx = (fYec25_idx+fYec25_idx_save)&16383;
			for (int i=0; i<4; i++) fRec79_tmp[i]=fRec79_perm[i];
			for (int i=0; i<4; i++) fRec4_tmp[i]=fRec4_perm[i];
			for (int i=0; i<4; i++) fRec2_tmp[i]=fRec2_perm[i];
			fRec0_idx = (fRec0_idx+fRec0_idx_save)&1023;
			for (int i=0; i<4; i++) fRec81_tmp[i]=fRec81_perm[i];
			fRec1_idx = (fRec1_idx+fRec1_idx_save)&1023;
			// exec code
			for (int i=0; i<count; i++) {
				fYec0[(fYec0_idx+i)&131071] = (((((fRec0[(fRec0_idx+i-int((int(min(512, max(0, iZec12[i]))) + 1)))&1023] * (0 - fZec14[i])) * (0 - (0.5f * fZec15[i]))) * (0 - (0.33333334f * fZec16[i]))) * (0 - (0.25f * fZec17[i]))) + ((fZec10[i] + (1e+01f - fZec13[i])) * ((((((fRec0[(fRec0_idx+i-int((int(min(512, max(0, (iZec12[i] + 1)))) + 1)))&1023] * (0 - fZec15[i])) * (0 - (0.5f * fZec16[i]))) * (0 - (0.33333334f * fZec17[i]))) + (0.5f * (((fZec14[i] * fRec0[(fRec0_idx+i-int((int(min(512, max(0, (iZec12[i] + 2)))) + 1)))&1023]) * (0 - fZec16[i])) * (0 - (0.5f * fZec17[i]))))) + (0.16666667f * ((fZec18[i] * fRec0[(fRec0_idx+i-int((int(min(512, max(0, (iZec12[i] + 3)))) + 1)))&1023]) * (0 - fZec17[i])))) + (0.041666668f * ((fZec18[i] * fZec16[i]) * fRec0[(fRec0_idx+i-int((int(min(512, max(0, (iZec12[i] + 4)))) + 1)))&1023])))));
				fZec23[i] = ((float)input0[i] + (0.5f * (((fYec0[(fYec0_idx+i-iZec19[i])&131071] * fZec20[i]) + (fYec0[(fYec0_idx+i-iZec21[i])&131071] * fRec42[i])) * fZec22[i])));
				fZec24[i] = ((fZec7[i] * fRec27[i-1]) + (fZec8[i] * fZec23[i]));
				fZec25[i] = ((fZec7[i] * fRec30[i-1]) + (fZec8[i] * fZec24[i]));
				fZec26[i] = ((fZec7[i] * fRec33[i-1]) + (fZec8[i] * fZec25[i]));
				fZec27[i] = ((fZec7[i] * fRec36[i-1]) + (fZec8[i] * fZec26[i]));
				fYec1[(fYec1_idx+i)&131071] = (((((fRec1[(fRec1_idx+i-int((int(min(512, max(0, iZec30[i]))) + 1)))&1023] * (0 - fZec32[i])) * (0 - (0.5f * fZec33[i]))) * (0 - (0.33333334f * fZec34[i]))) * (0 - (0.25f * fZec35[i]))) + ((fZec28[i] + (1e+01f - fZec31[i])) * ((((((fRec1[(fRec1_idx+i-int((int(min(512, max(0, (iZec30[i] + 1)))) + 1)))&1023] * (0 - fZec33[i])) * (0 - (0.5f * fZec34[i]))) * (0 - (0.33333334f * fZec35[i]))) + (0.5f * (((fZec32[i] * fRec1[(fRec1_idx+i-int((int(min(512, max(0, (iZec30[i] + 2)))) + 1)))&1023]) * (0 - fZec34[i])) * (0 - (0.5f * fZec35[i]))))) + (0.16666667f * ((fZec36[i] * fRec1[(fRec1_idx+i-int((int(min(512, max(0, (iZec30[i] + 3)))) + 1)))&1023]) * (0 - fZec35[i])))) + (0.041666668f * ((fZec36[i] * fZec34[i]) * fRec1[(fRec1_idx+i-int((int(min(512, max(0, (iZec30[i] + 4)))) + 1)))&1023])))));
				fZec37[i] = ((float)input1[i] + (0.5f * (fZec22[i] * ((fYec1[(fYec1_idx+i-iZec21[i])&131071] * fRec42[i]) + (fYec1[(fYec1_idx+i-iZec19[i])&131071] * fZec20[i])))));
				fZec38[i] = ((fZec7[i] * fRec28[i-1]) + (fZec8[i] * fZec37[i]));
				fZec39[i] = ((fZec7[i] * fRec31[i-1]) + (fZec8[i] * fZec38[i]));
				fZec40[i] = ((fZec7[i] * fRec34[i-1]) + (fZec8[i] * fZec39[i]));
				fZec41[i] = ((fZec7[i] * fRec37[i-1]) + (fZec8[i] * fZec40[i]));
				fYec2[(fYec2_idx+i)&16383] = ((6.123234e-17f * fZec27[i]) - fZec41[i]);
				fRec38[i] = (fYec2[(fYec2_idx+i-int((iZec43[i] + 1)))&16383] + ((fRec38[i-1] * (0 - (fZec45[i] / fZec46[i]))) + ((fZec45[i] * fYec2[(fYec2_idx+i-iZec43[i])&16383]) / fZec46[i])));
				fRec36[i] = fRec38[i];
				fYec3[(fYec3_idx+i)&16383] = (fZec27[i] + (6.123234e-17f * fZec41[i]));
				fRec46[i] = (fYec3[(fYec3_idx+i-int((iZec48[i] + 1)))&16383] + ((fRec46[i-1] * (0 - (fZec50[i] / fZec51[i]))) + ((fZec50[i] * fYec3[(fYec3_idx+i-iZec48[i])&16383]) / fZec51[i])));
				fRec37[i] = fRec46[i];
				fZec52[i] = ((fRec36[i-1] * fZec8[i]) + (fZec6[i] * fZec26[i]));
				fZec53[i] = ((fZec8[i] * fRec37[i-1]) + (fZec6[i] * fZec40[i]));
				fYec4[(fYec4_idx+i)&16383] = ((6.123234e-17f * fZec52[i]) - fZec53[i]);
				fRec35[i] = (fYec4[(fYec4_idx+i-int((iZec55[i] + 1)))&16383] + ((fRec35[i-1] * (0 - (fZec57[i] / fZec58[i]))) + ((fZec57[i] * fYec4[(fYec4_idx+i-iZec55[i])&16383]) / fZec58[i])));
				fRec33[i] = fRec35[i];
				fYec5[(fYec5_idx+i)&16383] = (fZec52[i] + (6.123234e-17f * fZec53[i]));
				fRec49[i] = (fYec5[(fYec5_idx+i-int((iZec60[i] + 1)))&16383] + ((fRec49[i-1] * (0 - (fZec62[i] / fZec63[i]))) + ((fZec62[i] * fYec5[(fYec5_idx+i-iZec60[i])&16383]) / fZec63[i])));
				fRec34[i] = fRec49[i];
				fZec64[i] = ((fRec33[i-1] * fZec8[i]) + (fZec6[i] * fZec25[i]));
				fZec65[i] = ((fZec8[i] * fRec34[i-1]) + (fZec6[i] * fZec39[i]));
				fYec6[(fYec6_idx+i)&16383] = ((6.123234e-17f * fZec64[i]) - fZec65[i]);
				fRec32[i] = (fYec6[(fYec6_idx+i-int((iZec67[i] + 1)))&16383] + (((0 - (fZec69[i] / fZec70[i])) * fRec32[i-1]) + ((fZec69[i] * fYec6[(fYec6_idx+i-iZec67[i])&16383]) / fZec70[i])));
				fRec30[i] = fRec32[i];
				fYec7[(fYec7_idx+i)&16383] = ((6.123234e-17f * fZec65[i]) + fZec64[i]);
				fRec52[i] = (fYec7[(fYec7_idx+i-int((iZec72[i] + 1)))&16383] + ((fRec52[i-1] * (0 - (fZec74[i] / fZec75[i]))) + ((fYec7[(fYec7_idx+i-iZec72[i])&16383] * fZec74[i]) / fZec75[i])));
				fRec31[i] = fRec52[i];
				fZec76[i] = ((fRec30[i-1] * fZec8[i]) + (fZec6[i] * fZec24[i]));
				fZec77[i] = ((fZec8[i] * fRec31[i-1]) + (fZec6[i] * fZec38[i]));
				fYec8[(fYec8_idx+i)&16383] = ((6.123234e-17f * fZec76[i]) - fZec77[i]);
				fRec29[i] = ((((fYec8[(fYec8_idx+i-iZec79[i])&16383] * fZec81[i]) / fZec82[i]) + fYec8[(fYec8_idx+i-int((iZec79[i] + 1)))&16383]) + (fRec29[i-1] * (0 - (fZec81[i] / fZec82[i]))));
				fRec27[i] = fRec29[i];
				fYec9[(fYec9_idx+i)&16383] = (fZec76[i] + (6.123234e-17f * fZec77[i]));
				fRec55[i] = (fYec9[(fYec9_idx+i-int((iZec84[i] + 1)))&16383] + ((fRec55[i-1] * (0 - (fZec86[i] / fZec87[i]))) + ((fZec86[i] * fYec9[(fYec9_idx+i-iZec84[i])&16383]) / fZec87[i])));
				fRec28[i] = fRec55[i];
				fZec90[i] = ((fRec27[i-1] * fZec8[i]) + (fZec6[i] * fZec23[i]));
				fZec93[i] = ((fZec90[i] * fZec89[i]) + (fZec92[i] * fRec15[i-1]));
				fZec94[i] = ((fZec89[i] * fZec93[i]) + (fZec92[i] * fRec18[i-1]));
				fZec95[i] = ((fZec89[i] * fZec94[i]) + (fZec92[i] * fRec21[i-1]));
				fZec96[i] = (fZec89[i] * fZec95[i]);
				fZec97[i] = (fRec24[i-1] * fZec92[i]);
				fZec98[i] = ((fZec8[i] * fRec28[i-1]) + (fZec6[i] * fZec37[i]));
				fZec99[i] = ((fRec16[i-1] * fZec92[i]) + (fZec89[i] * fZec98[i]));
				fZec100[i] = ((fRec19[i-1] * fZec92[i]) + (fZec89[i] * fZec99[i]));
				fZec101[i] = ((fRec22[i-1] * fZec92[i]) + (fZec89[i] * fZec100[i]));
				fZec102[i] = ((fRec25[i-1] * fZec92[i]) + (fZec89[i] * fZec101[i]));
				fYec10[(fYec10_idx+i)&16383] = ((6.123234e-17f * (fZec96[i] + fZec97[i])) - fZec102[i]);
				fRec26[i] = ((((fYec10[(fYec10_idx+i-iZec104[i])&16383] * fZec106[i]) / fZec107[i]) + fYec10[(fYec10_idx+i-int((iZec104[i] + 1)))&16383]) + (fRec26[i-1] * (0 - (fZec106[i] / fZec107[i]))));
				fRec24[i] = fRec26[i];
				fYec11[(fYec11_idx+i)&16383] = ((fZec96[i] + (6.123234e-17f * fZec102[i])) + fZec97[i]);
				fRec58[i] = (fYec11[(fYec11_idx+i-int((iZec109[i] + 1)))&16383] + ((fRec58[i-1] * (0 - (fZec111[i] / fZec112[i]))) + ((fZec111[i] * fYec11[(fYec11_idx+i-iZec109[i])&16383]) / fZec112[i])));
				fRec25[i] = fRec58[i];
				fZec113[i] = ((fZec89[i] * fRec24[i-1]) + (fZec95[i] * fZec91[i]));
				fZec114[i] = ((fZec89[i] * fRec25[i-1]) + (fZec101[i] * fZec91[i]));
				fYec12[(fYec12_idx+i)&16383] = ((6.123234e-17f * fZec113[i]) - fZec114[i]);
				fRec23[i] = ((((fYec12[(fYec12_idx+i-iZec116[i])&16383] * fZec118[i]) / fZec119[i]) + fYec12[(fYec12_idx+i-int((iZec116[i] + 1)))&16383]) + (fRec23[i-1] * (0 - (fZec118[i] / fZec119[i]))));
				fRec21[i] = fRec23[i];
				fYec13[(fYec13_idx+i)&16383] = (fZec113[i] + (6.123234e-17f * fZec114[i]));
				fRec61[i] = (fYec13[(fYec13_idx+i-int((iZec121[i] + 1)))&16383] + ((fRec61[i-1] * (0 - (fZec123[i] / fZec124[i]))) + ((fZec123[i] * fYec13[(fYec13_idx+i-iZec121[i])&16383]) / fZec124[i])));
				fRec22[i] = fRec61[i];
				fZec125[i] = ((fZec89[i] * fRec21[i-1]) + (fZec94[i] * fZec91[i]));
				fZec126[i] = ((fZec89[i] * fRec22[i-1]) + (fZec100[i] * fZec91[i]));
				fYec14[(fYec14_idx+i)&16383] = ((6.123234e-17f * fZec125[i]) - fZec126[i]);
				fRec20[i] = ((((fYec14[(fYec14_idx+i-iZec128[i])&16383] * fZec130[i]) / fZec131[i]) + fYec14[(fYec14_idx+i-int((iZec128[i] + 1)))&16383]) + (fRec20[i-1] * (0 - (fZec130[i] / fZec131[i]))));
				fRec18[i] = fRec20[i];
				fYec15[(fYec15_idx+i)&16383] = (fZec125[i] + (6.123234e-17f * fZec126[i]));
				fRec64[i] = (fYec15[(fYec15_idx+i-int((iZec133[i] + 1)))&16383] + ((fRec64[i-1] * (0 - (fZec135[i] / fZec136[i]))) + ((fZec135[i] * fYec15[(fYec15_idx+i-iZec133[i])&16383]) / fZec136[i])));
				fRec19[i] = fRec64[i];
				fZec137[i] = ((fZec89[i] * fRec18[i-1]) + (fZec93[i] * fZec91[i]));
				fZec138[i] = ((fZec89[i] * fRec19[i-1]) + (fZec99[i] * fZec91[i]));
				fYec16[(fYec16_idx+i)&16383] = ((6.123234e-17f * fZec137[i]) - fZec138[i]);
				fRec17[i] = ((((fYec16[(fYec16_idx+i-iZec140[i])&16383] * fZec142[i]) / fZec143[i]) + fYec16[(fYec16_idx+i-int((iZec140[i] + 1)))&16383]) + (fRec17[i-1] * (0 - (fZec142[i] / fZec143[i]))));
				fRec15[i] = fRec17[i];
				fYec17[(fYec17_idx+i)&16383] = (fZec137[i] + (6.123234e-17f * fZec138[i]));
				fRec67[i] = (fYec17[(fYec17_idx+i-int((iZec145[i] + 1)))&16383] + ((fRec67[i-1] * (0 - (fZec147[i] / fZec148[i]))) + ((fZec147[i] * fYec17[(fYec17_idx+i-iZec145[i])&16383]) / fZec148[i])));
				fRec16[i] = fRec67[i];
				fZec149[i] = ((fZec89[i] * fRec15[i-1]) + (fZec90[i] * fZec91[i]));
				fZec150[i] = ((fZec7[i] * fRec3[i-1]) + (fZec8[i] * fZec149[i]));
				fZec151[i] = ((fZec8[i] * fZec150[i]) + (fZec7[i] * fRec6[i-1]));
				fZec152[i] = ((fZec151[i] * fZec8[i]) + (fZec7[i] * fRec9[i-1]));
				fZec153[i] = (fZec152[i] * fZec8[i]);
				fZec154[i] = (fRec12[i-1] * fZec7[i]);
				fZec155[i] = ((fZec89[i] * fRec16[i-1]) + (fZec98[i] * fZec91[i]));
				fZec156[i] = ((fRec4[i-1] * fZec7[i]) + (fZec155[i] * fZec8[i]));
				fZec157[i] = ((fRec7[i-1] * fZec7[i]) + (fZec156[i] * fZec8[i]));
				fZec158[i] = ((fRec10[i-1] * fZec7[i]) + (fZec157[i] * fZec8[i]));
				fZec159[i] = ((fRec13[i-1] * fZec7[i]) + (fZec158[i] * fZec8[i]));
				fYec18[(fYec18_idx+i)&16383] = ((6.123234e-17f * (fZec153[i] + fZec154[i])) - fZec159[i]);
				fRec14[i] = ((((fYec18[(fYec18_idx+i-iZec161[i])&16383] * fZec163[i]) / fZec164[i]) + fYec18[(fYec18_idx+i-int((iZec161[i] + 1)))&16383]) + (fRec14[i-1] * (0 - (fZec163[i] / fZec164[i]))));
				fRec12[i] = fRec14[i];
				fYec19[(fYec19_idx+i)&16383] = ((fZec153[i] + (6.123234e-17f * fZec159[i])) + fZec154[i]);
				fRec70[i] = (fYec19[(fYec19_idx+i-int((iZec166[i] + 1)))&16383] + ((fRec70[i-1] * (0 - (fZec168[i] / fZec169[i]))) + ((fZec168[i] * fYec19[(fYec19_idx+i-iZec166[i])&16383]) / fZec169[i])));
				fRec13[i] = fRec70[i];
				fZec170[i] = ((fRec12[i-1] * fZec8[i]) + (fZec152[i] * fZec6[i]));
				fZec171[i] = ((fRec13[i-1] * fZec8[i]) + (fZec158[i] * fZec6[i]));
				fYec20[(fYec20_idx+i)&16383] = ((6.123234e-17f * fZec170[i]) - fZec171[i]);
				fRec11[i] = ((((fYec20[(fYec20_idx+i-iZec173[i])&16383] * fZec175[i]) / fZec176[i]) + fYec20[(fYec20_idx+i-int((iZec173[i] + 1)))&16383]) + (fRec11[i-1] * (0 - (fZec175[i] / fZec176[i]))));
				fRec9[i] = fRec11[i];
				fYec21[(fYec21_idx+i)&16383] = (fZec170[i] + (6.123234e-17f * fZec171[i]));
				fRec73[i] = (fYec21[(fYec21_idx+i-int((iZec178[i] + 1)))&16383] + ((fRec73[i-1] * (0 - (fZec180[i] / fZec181[i]))) + ((fZec180[i] * fYec21[(fYec21_idx+i-iZec178[i])&16383]) / fZec181[i])));
				fRec10[i] = fRec73[i];
				fZec182[i] = ((fRec9[i-1] * fZec8[i]) + (fZec151[i] * fZec6[i]));
				fZec183[i] = ((fRec10[i-1] * fZec8[i]) + (fZec157[i] * fZec6[i]));
				fYec22[(fYec22_idx+i)&16383] = ((6.123234e-17f * fZec182[i]) - fZec183[i]);
				fRec8[i] = ((((fYec22[(fYec22_idx+i-iZec185[i])&16383] * fZec187[i]) / fZec188[i]) + fYec22[(fYec22_idx+i-int((iZec185[i] + 1)))&16383]) + (fRec8[i-1] * (0 - (fZec187[i] / fZec188[i]))));
				fRec6[i] = fRec8[i];
				fYec23[(fYec23_idx+i)&16383] = (fZec182[i] + (6.123234e-17f * fZec183[i]));
				fRec76[i] = (fYec23[(fYec23_idx+i-int((iZec190[i] + 1)))&16383] + ((fRec76[i-1] * (0 - (fZec192[i] / fZec193[i]))) + ((fZec192[i] * fYec23[(fYec23_idx+i-iZec190[i])&16383]) / fZec193[i])));
				fRec7[i] = fRec76[i];
				fZec194[i] = ((fRec6[i-1] * fZec8[i]) + (fZec6[i] * fZec150[i]));
				fZec195[i] = ((fRec7[i-1] * fZec8[i]) + (fZec156[i] * fZec6[i]));
				fYec24[(fYec24_idx+i)&16383] = ((6.123234e-17f * fZec194[i]) - fZec195[i]);
				fRec5[i] = ((((fYec24[(fYec24_idx+i-iZec197[i])&16383] * fZec199[i]) / fZec200[i]) + fYec24[(fYec24_idx+i-int((iZec197[i] + 1)))&16383]) + ((0 - (fZec199[i] / fZec200[i])) * fRec5[i-1]));
				fRec3[i] = fRec5[i];
				fYec25[(fYec25_idx+i)&16383] = (fZec194[i] + (6.123234e-17f * fZec195[i]));
				fRec79[i] = (fYec25[(fYec25_idx+i-int((iZec202[i] + 1)))&16383] + ((fRec79[i-1] * (0 - (fZec204[i] / fZec205[i]))) + ((fZec204[i] * fYec25[(fYec25_idx+i-iZec202[i])&16383]) / fZec205[i])));
				fRec4[i] = fRec79[i];
				fRec2[i] = ((((fRec3[i-1] * fZec8[i]) + (fZec6[i] * fZec149[i])) * fZec207[i]) + (0.5f * (fZec206[i] * fRec2[i-1])));
				fRec0[(fRec0_idx+i)&1023] = fRec2[i];
				fRec81[i] = ((0.5f * (fZec206[i] * fRec81[i-1])) + (fZec207[i] * ((fRec4[i-1] * fZec8[i]) + (fZec155[i] * fZec6[i]))));
				fRec1[(fRec1_idx+i)&1023] = fRec81[i];
			}
			// post processing
			fRec1_idx_save = count;
			for (int i=0; i<4; i++) fRec81_perm[i]=fRec81_tmp[count+i];
			fRec0_idx_save = count;
			for (int i=0; i<4; i++) fRec2_perm[i]=fRec2_tmp[count+i];
			for (int i=0; i<4; i++) fRec4_perm[i]=fRec4_tmp[count+i];
			for (int i=0; i<4; i++) fRec79_perm[i]=fRec79_tmp[count+i];
			fYec25_idx_save = count;
			for (int i=0; i<4; i++) fRec3_perm[i]=fRec3_tmp[count+i];
			for (int i=0; i<4; i++) fRec5_perm[i]=fRec5_tmp[count+i];
			fYec24_idx_save = count;
			for (int i=0; i<4; i++) fRec7_perm[i]=fRec7_tmp[count+i];
			for (int i=0; i<4; i++) fRec76_perm[i]=fRec76_tmp[count+i];
			fYec23_idx_save = count;
			for (int i=0; i<4; i++) fRec6_perm[i]=fRec6_tmp[count+i];
			for (int i=0; i<4; i++) fRec8_perm[i]=fRec8_tmp[count+i];
			fYec22_idx_save = count;
			for (int i=0; i<4; i++) fRec10_perm[i]=fRec10_tmp[count+i];
			for (int i=0; i<4; i++) fRec73_perm[i]=fRec73_tmp[count+i];
			fYec21_idx_save = count;
			for (int i=0; i<4; i++) fRec9_perm[i]=fRec9_tmp[count+i];
			for (int i=0; i<4; i++) fRec11_perm[i]=fRec11_tmp[count+i];
			fYec20_idx_save = count;
			for (int i=0; i<4; i++) fRec13_perm[i]=fRec13_tmp[count+i];
			for (int i=0; i<4; i++) fRec70_perm[i]=fRec70_tmp[count+i];
			fYec19_idx_save = count;
			for (int i=0; i<4; i++) fRec12_perm[i]=fRec12_tmp[count+i];
			for (int i=0; i<4; i++) fRec14_perm[i]=fRec14_tmp[count+i];
			fYec18_idx_save = count;
			for (int i=0; i<4; i++) fRec16_perm[i]=fRec16_tmp[count+i];
			for (int i=0; i<4; i++) fRec67_perm[i]=fRec67_tmp[count+i];
			fYec17_idx_save = count;
			for (int i=0; i<4; i++) fRec15_perm[i]=fRec15_tmp[count+i];
			for (int i=0; i<4; i++) fRec17_perm[i]=fRec17_tmp[count+i];
			fYec16_idx_save = count;
			for (int i=0; i<4; i++) fRec19_perm[i]=fRec19_tmp[count+i];
			for (int i=0; i<4; i++) fRec64_perm[i]=fRec64_tmp[count+i];
			fYec15_idx_save = count;
			for (int i=0; i<4; i++) fRec18_perm[i]=fRec18_tmp[count+i];
			for (int i=0; i<4; i++) fRec20_perm[i]=fRec20_tmp[count+i];
			fYec14_idx_save = count;
			for (int i=0; i<4; i++) fRec22_perm[i]=fRec22_tmp[count+i];
			for (int i=0; i<4; i++) fRec61_perm[i]=fRec61_tmp[count+i];
			fYec13_idx_save = count;
			for (int i=0; i<4; i++) fRec21_perm[i]=fRec21_tmp[count+i];
			for (int i=0; i<4; i++) fRec23_perm[i]=fRec23_tmp[count+i];
			fYec12_idx_save = count;
			for (int i=0; i<4; i++) fRec25_perm[i]=fRec25_tmp[count+i];
			for (int i=0; i<4; i++) fRec58_perm[i]=fRec58_tmp[count+i];
			fYec11_idx_save = count;
			for (int i=0; i<4; i++) fRec24_perm[i]=fRec24_tmp[count+i];
			for (int i=0; i<4; i++) fRec26_perm[i]=fRec26_tmp[count+i];
			fYec10_idx_save = count;
			for (int i=0; i<4; i++) fRec28_perm[i]=fRec28_tmp[count+i];
			for (int i=0; i<4; i++) fRec55_perm[i]=fRec55_tmp[count+i];
			fYec9_idx_save = count;
			for (int i=0; i<4; i++) fRec27_perm[i]=fRec27_tmp[count+i];
			for (int i=0; i<4; i++) fRec29_perm[i]=fRec29_tmp[count+i];
			fYec8_idx_save = count;
			for (int i=0; i<4; i++) fRec31_perm[i]=fRec31_tmp[count+i];
			for (int i=0; i<4; i++) fRec52_perm[i]=fRec52_tmp[count+i];
			fYec7_idx_save = count;
			for (int i=0; i<4; i++) fRec30_perm[i]=fRec30_tmp[count+i];
			for (int i=0; i<4; i++) fRec32_perm[i]=fRec32_tmp[count+i];
			fYec6_idx_save = count;
			for (int i=0; i<4; i++) fRec34_perm[i]=fRec34_tmp[count+i];
			for (int i=0; i<4; i++) fRec49_perm[i]=fRec49_tmp[count+i];
			fYec5_idx_save = count;
			for (int i=0; i<4; i++) fRec33_perm[i]=fRec33_tmp[count+i];
			for (int i=0; i<4; i++) fRec35_perm[i]=fRec35_tmp[count+i];
			fYec4_idx_save = count;
			for (int i=0; i<4; i++) fRec37_perm[i]=fRec37_tmp[count+i];
			for (int i=0; i<4; i++) fRec46_perm[i]=fRec46_tmp[count+i];
			fYec3_idx_save = count;
			for (int i=0; i<4; i++) fRec36_perm[i]=fRec36_tmp[count+i];
			for (int i=0; i<4; i++) fRec38_perm[i]=fRec38_tmp[count+i];
			fYec2_idx_save = count;
			fYec1_idx_save = count;
			fYec0_idx_save = count;
			
			// SECTION : 11
			// LOOP 0x7fedbd1aa1e0
			// exec code
			for (int i=0; i<count; i++) {
				output0[i] = (FAUSTFLOAT)fRec0[(fRec0_idx+i)&1023];
			}
			
			// LOOP 0x7fedbd48dbc0
			// exec code
			for (int i=0; i<count; i++) {
				output1[i] = (FAUSTFLOAT)fRec1[(fRec1_idx+i)&1023];
			}
			
		}
		if (index < fullcount) {
			// compute the remaining samples if any
			int count = fullcount-index;
			FAUSTFLOAT* input0 = &input[0][index];
			FAUSTFLOAT* input1 = &input[1][index];
			FAUSTFLOAT* output0 = &output[0][index];
			FAUSTFLOAT* output1 = &output[1][index];
			// SECTION : 1
			// LOOP 0x7fedbd1afd00
			// pre processing
			for (int i=0; i<4; i++) fVec0_tmp[i]=fVec0_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fVec0[i] = fSlow0;
			}
			// post processing
			for (int i=0; i<4; i++) fVec0_perm[i]=fVec0_tmp[count+i];
			
			// SECTION : 2
			// LOOP 0x7fedbd1af500
			// exec code
			for (int i=0; i<count; i++) {
				fZec0[i] = (fConst1 * (fSlow0 + fVec0[i-1]));
			}
			
			// LOOP 0x7fedbd1b1ff0
			// pre processing
			for (int i=0; i<4; i++) iVec1_tmp[i]=iVec1_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				iVec1[i] = 1;
			}
			// post processing
			for (int i=0; i<4; i++) iVec1_perm[i]=iVec1_tmp[count+i];
			
			// SECTION : 3
			// LOOP 0x7fedbd1af280
			// exec code
			for (int i=0; i<count; i++) {
				fZec1[i] = sinf(fZec0[i]);
			}
			
			// LOOP 0x7fedbd1b10a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec2[i] = cosf(fZec0[i]);
			}
			
			// LOOP 0x7fedbd1b1ef0
			// exec code
			for (int i=0; i<count; i++) {
				iZec3[i] = (1 - iVec1[i-1]);
			}
			
			// LOOP 0x7fedbd1ba960
			// pre processing
			for (int i=0; i<4; i++) fVec3_tmp[i]=fVec3_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fVec3[i] = fSlow7;
			}
			// post processing
			for (int i=0; i<4; i++) fVec3_perm[i]=fVec3_tmp[count+i];
			
			// SECTION : 4
			// LOOP 0x7fedbd1af110
			// pre processing
			for (int i=0; i<4; i++) fRec39_tmp[i]=fRec39_perm[i];
			for (int i=0; i<4; i++) fRec40_tmp[i]=fRec40_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec39[i] = ((fZec1[i] * fRec40[i-1]) + (fZec2[i] * fRec39[i-1]));
				fRec40[i] = (((fZec2[i] * fRec40[i-1]) + ((0 - fZec1[i]) * fRec39[i-1])) + iZec3[i]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec40_perm[i]=fRec40_tmp[count+i];
			for (int i=0; i<4; i++) fRec39_perm[i]=fRec39_tmp[count+i];
			
			// LOOP 0x7fedbd1ba880
			// exec code
			for (int i=0; i<count; i++) {
				fZec9[i] = (fSlow7 + fVec3[i-1]);
			}
			
			// SECTION : 5
			// LOOP 0x7fedbd1b88d0
			// pre processing
			for (int i=0; i<4; i++) fVec2_tmp[i]=fVec2_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fVec2[i] = fSlow6;
			}
			// post processing
			for (int i=0; i<4; i++) fVec2_perm[i]=fVec2_tmp[count+i];
			
			// LOOP 0x7fedbd1ba510
			// exec code
			for (int i=0; i<count; i++) {
				fZec10[i] = (fConst2 * (fZec9[i] * (fRec40[i] + 1.0f)));
			}
			
			// LOOP 0x7fedbd1c5420
			// exec code
			for (int i=0; i<count; i++) {
				fZec28[i] = (fConst2 * (fZec9[i] * (fRec39[i] + 1.0f)));
			}
			
			// SECTION : 6
			// LOOP 0x7fedbd0be930
			// pre processing
			for (int i=0; i<4; i++) fRec47_tmp[i]=fRec47_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec47[i] = (fSlow10 + (0.9999f * ((iSlow9 * iZec3[i]) + fRec47[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec47_perm[i]=fRec47_tmp[count+i];
			
			// LOOP 0x7fedbd0cdef0
			// pre processing
			for (int i=0; i<4; i++) fRec59_tmp[i]=fRec59_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec59[i] = (fSlow29 + (0.9999f * ((iSlow28 * iZec3[i]) + fRec59[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec59_perm[i]=fRec59_tmp[count+i];
			
			// LOOP 0x7fedbd0d7540
			// pre processing
			for (int i=0; i<4; i++) fRec60_tmp[i]=fRec60_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec60[i] = (fSlow32 + (0.999f * ((iSlow31 * iZec3[i]) + fRec60[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec60_perm[i]=fRec60_tmp[count+i];
			
			// LOOP 0x7fedbd0e1950
			// pre processing
			for (int i=0; i<4; i++) fRec62_tmp[i]=fRec62_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec62[i] = (fSlow34 + (0.999f * ((iSlow33 * iZec3[i]) + fRec62[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec62_perm[i]=fRec62_tmp[count+i];
			
			// LOOP 0x7fedbd0eba60
			// pre processing
			for (int i=0; i<4; i++) fRec63_tmp[i]=fRec63_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec63[i] = (fSlow37 + (0.999f * ((iSlow36 * iZec3[i]) + fRec63[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec63_perm[i]=fRec63_tmp[count+i];
			
			// LOOP 0x7fedbd0f6910
			// pre processing
			for (int i=0; i<4; i++) fRec65_tmp[i]=fRec65_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec65[i] = (fSlow39 + (0.999f * ((iSlow38 * iZec3[i]) + fRec65[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec65_perm[i]=fRec65_tmp[count+i];
			
			// LOOP 0x7fedbd1b6e00
			// pre processing
			for (int i=0; i<4; i++) fRec45_tmp[i]=fRec45_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec45[i] = (fSlow5 + (0.9999f * ((iSlow4 * iZec3[i]) + fRec45[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec45_perm[i]=fRec45_tmp[count+i];
			
			// LOOP 0x7fedbd1b87c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec5[i] = (0.5f * (fSlow6 + fVec2[i-1]));
			}
			
			// LOOP 0x7fedbd1ba430
			// exec code
			for (int i=0; i<count; i++) {
				fZec11[i] = (fZec10[i] + 8.500005f);
			}
			
			// LOOP 0x7fedbd1c5340
			// exec code
			for (int i=0; i<count; i++) {
				fZec29[i] = (fZec28[i] + 8.500005f);
			}
			
			// LOOP 0x7fedbd1ceaa0
			// pre processing
			for (int i=0; i<4; i++) fRec48_tmp[i]=fRec48_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec48[i] = (fSlow13 + (0.999f * ((iSlow12 * iZec3[i]) + fRec48[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec48_perm[i]=fRec48_tmp[count+i];
			
			// LOOP 0x7fedbd1d5fc0
			// pre processing
			for (int i=0; i<4; i++) fRec50_tmp[i]=fRec50_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec50[i] = (fSlow15 + (0.999f * ((iSlow14 * iZec3[i]) + fRec50[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec50_perm[i]=fRec50_tmp[count+i];
			
			// LOOP 0x7fedbd1dcdc0
			// pre processing
			for (int i=0; i<4; i++) fRec51_tmp[i]=fRec51_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec51[i] = (fSlow18 + (0.999f * ((iSlow17 * iZec3[i]) + fRec51[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec51_perm[i]=fRec51_tmp[count+i];
			
			// LOOP 0x7fedbd1e4c00
			// pre processing
			for (int i=0; i<4; i++) fRec53_tmp[i]=fRec53_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec53[i] = (fSlow20 + (0.999f * ((iSlow19 * iZec3[i]) + fRec53[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec53_perm[i]=fRec53_tmp[count+i];
			
			// LOOP 0x7fedbd1ec010
			// pre processing
			for (int i=0; i<4; i++) fRec54_tmp[i]=fRec54_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec54[i] = (fSlow22 + (0.999f * ((iSlow21 * iZec3[i]) + fRec54[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec54_perm[i]=fRec54_tmp[count+i];
			
			// LOOP 0x7fedbd1f4780
			// pre processing
			for (int i=0; i<4; i++) fRec56_tmp[i]=fRec56_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec56[i] = (fSlow24 + (0.999f * ((iSlow23 * iZec3[i]) + fRec56[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec56_perm[i]=fRec56_tmp[count+i];
			
			// LOOP 0x7fedbd300d90
			// pre processing
			for (int i=0; i<4; i++) fRec57_tmp[i]=fRec57_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec57[i] = (fSlow27 + (0.9999f * ((iSlow26 * iZec3[i]) + fRec57[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec57_perm[i]=fRec57_tmp[count+i];
			
			// LOOP 0x7fedbd30e750
			// pre processing
			for (int i=0; i<4; i++) fRec69_tmp[i]=fRec69_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec69[i] = (fSlow47 + (0.9999f * ((iSlow46 * iZec3[i]) + fRec69[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec69_perm[i]=fRec69_tmp[count+i];
			
			// LOOP 0x7fedbd405580
			// pre processing
			for (int i=0; i<4; i++) fRec66_tmp[i]=fRec66_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec66[i] = (fSlow42 + (0.999f * ((iSlow41 * iZec3[i]) + fRec66[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec66_perm[i]=fRec66_tmp[count+i];
			
			// LOOP 0x7fedbd40cda0
			// pre processing
			for (int i=0; i<4; i++) fRec68_tmp[i]=fRec68_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec68[i] = (fSlow44 + (0.999f * ((iSlow43 * iZec3[i]) + fRec68[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec68_perm[i]=fRec68_tmp[count+i];
			
			// LOOP 0x7fedbd41d6d0
			// pre processing
			for (int i=0; i<4; i++) fRec71_tmp[i]=fRec71_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec71[i] = (fSlow49 + (0.9999f * ((iSlow48 * iZec3[i]) + fRec71[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec71_perm[i]=fRec71_tmp[count+i];
			
			// LOOP 0x7fedbd42a130
			// pre processing
			for (int i=0; i<4; i++) fRec72_tmp[i]=fRec72_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec72[i] = (fSlow52 + (0.999f * ((iSlow51 * iZec3[i]) + fRec72[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec72_perm[i]=fRec72_tmp[count+i];
			
			// LOOP 0x7fedbd437410
			// pre processing
			for (int i=0; i<4; i++) fRec74_tmp[i]=fRec74_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec74[i] = (fSlow54 + (0.999f * ((iSlow53 * iZec3[i]) + fRec74[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec74_perm[i]=fRec74_tmp[count+i];
			
			// LOOP 0x7fedbd444920
			// pre processing
			for (int i=0; i<4; i++) fRec75_tmp[i]=fRec75_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec75[i] = (fSlow57 + (0.999f * ((iSlow56 * iZec3[i]) + fRec75[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec75_perm[i]=fRec75_tmp[count+i];
			
			// LOOP 0x7fedbd4526b0
			// pre processing
			for (int i=0; i<4; i++) fRec77_tmp[i]=fRec77_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec77[i] = (fSlow59 + (0.999f * ((iSlow58 * iZec3[i]) + fRec77[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec77_perm[i]=fRec77_tmp[count+i];
			
			// LOOP 0x7fedbd460750
			// pre processing
			for (int i=0; i<4; i++) fRec78_tmp[i]=fRec78_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec78[i] = (fSlow62 + (0.999f * ((iSlow61 * iZec3[i]) + fRec78[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec78_perm[i]=fRec78_tmp[count+i];
			
			// LOOP 0x7fedbd46efa0
			// pre processing
			for (int i=0; i<4; i++) fRec80_tmp[i]=fRec80_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec80[i] = (fSlow64 + (0.999f * ((iSlow63 * iZec3[i]) + fRec80[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec80_perm[i]=fRec80_tmp[count+i];
			
			// SECTION : 7
			// LOOP 0x7fedbc67fdf0
			// exec code
			for (int i=0; i<count; i++) {
				fZec144[i] = (fRec68[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0ba340
			// exec code
			for (int i=0; i<count; i++) {
				fZec42[i] = (fRec45[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0c0000
			// exec code
			for (int i=0; i<count; i++) {
				fZec47[i] = (fRec47[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0c6a50
			// exec code
			for (int i=0; i<count; i++) {
				fZec103[i] = (fRec57[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0cf670
			// exec code
			for (int i=0; i<count; i++) {
				fZec108[i] = (fRec59[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0d9a70
			// exec code
			for (int i=0; i<count; i++) {
				fZec115[i] = (fRec60[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0e2f90
			// exec code
			for (int i=0; i<count; i++) {
				fZec120[i] = (fRec62[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0edf70
			// exec code
			for (int i=0; i<count; i++) {
				fZec127[i] = (fRec63[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd0f7f50
			// exec code
			for (int i=0; i<count; i++) {
				fZec132[i] = (fRec65[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1bca60
			// exec code
			for (int i=0; i<count; i++) {
				fZec13[i] = floorf(fZec11[i]);
			}
			
			// LOOP 0x7fedbd1c6860
			// exec code
			for (int i=0; i<count; i++) {
				fZec31[i] = floorf(fZec29[i]);
			}
			
			// LOOP 0x7fedbd1d10e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec54[i] = (fRec48[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1d75f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec59[i] = (fRec50[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1df270
			// exec code
			for (int i=0; i<count; i++) {
				fZec66[i] = (fRec51[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1e5ce0
			// exec code
			for (int i=0; i<count; i++) {
				fZec71[i] = (fRec53[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1ee360
			// exec code
			for (int i=0; i<count; i++) {
				fZec78[i] = (fRec54[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd1f5eb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec83[i] = (fRec56[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd302090
			// exec code
			for (int i=0; i<count; i++) {
				fZec88[i] = (0 - fZec5[i]);
			}
			
			// LOOP 0x7fedbd407a70
			// exec code
			for (int i=0; i<count; i++) {
				fZec139[i] = (fRec66[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd414ad0
			// exec code
			for (int i=0; i<count; i++) {
				fZec160[i] = (fRec69[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd41ee50
			// exec code
			for (int i=0; i<count; i++) {
				fZec165[i] = (fRec71[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd42c600
			// exec code
			for (int i=0; i<count; i++) {
				fZec172[i] = (fRec72[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd438a70
			// exec code
			for (int i=0; i<count; i++) {
				fZec177[i] = (fRec74[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd446e10
			// exec code
			for (int i=0; i<count; i++) {
				fZec184[i] = (fRec75[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd453cf0
			// exec code
			for (int i=0; i<count; i++) {
				fZec189[i] = (fRec77[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd462c40
			// exec code
			for (int i=0; i<count; i++) {
				fZec196[i] = (fRec78[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd4705e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec201[i] = (fRec80[i] + -1.49999f);
			}
			
			// LOOP 0x7fedbd47e030
			// pre processing
			for (int i=0; i<4; i++) fVec5_tmp[i]=fVec5_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fVec5[i] = fSlow65;
			}
			// post processing
			for (int i=0; i<4; i++) fVec5_perm[i]=fVec5_tmp[count+i];
			
			// SECTION : 8
			// LOOP 0x7fedbd0bb2c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec44[i] = floorf(fZec42[i]);
			}
			
			// LOOP 0x7fedbd0c0f00
			// exec code
			for (int i=0; i<count; i++) {
				fZec49[i] = floorf(fZec47[i]);
			}
			
			// LOOP 0x7fedbd0c3740
			// exec code
			for (int i=0; i<count; i++) {
				fZec91[i] = sinf(fZec88[i]);
			}
			
			// LOOP 0x7fedbd0c76a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec105[i] = floorf(fZec103[i]);
			}
			
			// LOOP 0x7fedbd0d05e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec110[i] = floorf(fZec108[i]);
			}
			
			// LOOP 0x7fedbd0da6c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec117[i] = floorf(fZec115[i]);
			}
			
			// LOOP 0x7fedbd0e3f00
			// exec code
			for (int i=0; i<count; i++) {
				fZec122[i] = floorf(fZec120[i]);
			}
			
			// LOOP 0x7fedbd0eebc0
			// exec code
			for (int i=0; i<count; i++) {
				fZec129[i] = floorf(fZec127[i]);
			}
			
			// LOOP 0x7fedbd0f8ec0
			// exec code
			for (int i=0; i<count; i++) {
				fZec134[i] = floorf(fZec132[i]);
			}
			
			// LOOP 0x7fedbd1b2de0
			// pre processing
			for (int i=0; i<4; i++) fRec41_tmp[i]=fRec41_perm[i];
			for (int i=0; i<4; i++) fRec42_tmp[i]=fRec42_perm[i];
			for (int i=0; i<4; i++) fRec43_tmp[i]=fRec43_perm[i];
			for (int i=0; i<4; i++) fRec44_tmp[i]=fRec44_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec4[i] = ((int((fRec41[i-1] != 0.0f)))?((int(((fRec42[i-1] > 0.0f) & (fRec42[i-1] < 1.0f))))?fRec41[i-1]:0):((int(((fRec42[i-1] == 0.0f) & (fSlow1 != fRec43[i-1]))))?4.5351473e-05f:((int(((fRec42[i-1] == 1.0f) & (fSlow1 != fRec44[i-1]))))?-4.5351473e-05f:0)));
				fRec41[i] = fZec4[i];
				fRec42[i] = max(0.0f, min(1.0f, (fRec42[i-1] + fZec4[i])));
				fRec43[i] = ((int(((fRec42[i-1] >= 1.0f) & (fRec44[i-1] != fSlow1))))?fSlow1:fRec43[i-1]);
				fRec44[i] = ((int(((fRec42[i-1] <= 0.0f) & (fRec43[i-1] != fSlow1))))?fSlow1:fRec44[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec44_perm[i]=fRec44_tmp[count+i];
			for (int i=0; i<4; i++) fRec43_perm[i]=fRec43_tmp[count+i];
			for (int i=0; i<4; i++) fRec42_perm[i]=fRec42_tmp[count+i];
			for (int i=0; i<4; i++) fRec41_perm[i]=fRec41_tmp[count+i];
			
			// LOOP 0x7fedbd1b86e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec6[i] = sinf(fZec5[i]);
			}
			
			// LOOP 0x7fedbd1bc810
			// exec code
			for (int i=0; i<count; i++) {
				fZec14[i] = (fZec10[i] + (9.0f - fZec13[i]));
			}
			
			// LOOP 0x7fedbd1bd520
			// exec code
			for (int i=0; i<count; i++) {
				fZec15[i] = (fZec10[i] + (8.0f - fZec13[i]));
			}
			
			// LOOP 0x7fedbd1c6780
			// exec code
			for (int i=0; i<count; i++) {
				fZec32[i] = (fZec28[i] + (9.0f - fZec31[i]));
			}
			
			// LOOP 0x7fedbd1c71f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec33[i] = (fZec28[i] + (8.0f - fZec31[i]));
			}
			
			// LOOP 0x7fedbd1d1f70
			// exec code
			for (int i=0; i<count; i++) {
				fZec56[i] = floorf(fZec54[i]);
			}
			
			// LOOP 0x7fedbd1d84e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec61[i] = floorf(fZec59[i]);
			}
			
			// LOOP 0x7fedbd1e0000
			// exec code
			for (int i=0; i<count; i++) {
				fZec68[i] = floorf(fZec66[i]);
			}
			
			// LOOP 0x7fedbd1e6b90
			// exec code
			for (int i=0; i<count; i++) {
				fZec73[i] = floorf(fZec71[i]);
			}
			
			// LOOP 0x7fedbd1eefa0
			// exec code
			for (int i=0; i<count; i++) {
				fZec80[i] = floorf(fZec78[i]);
			}
			
			// LOOP 0x7fedbd1f6db0
			// exec code
			for (int i=0; i<count; i++) {
				fZec85[i] = floorf(fZec83[i]);
			}
			
			// LOOP 0x7fedbd22df70
			// pre processing
			for (int i=0; i<4; i++) fVec4_tmp[i]=fVec4_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fVec4[i] = fSlow8;
			}
			// post processing
			for (int i=0; i<4; i++) fVec4_perm[i]=fVec4_tmp[count+i];
			
			// LOOP 0x7fedbd308000
			// exec code
			for (int i=0; i<count; i++) {
				fZec146[i] = floorf(fZec144[i]);
			}
			
			// LOOP 0x7fedbd4086c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec141[i] = floorf(fZec139[i]);
			}
			
			// LOOP 0x7fedbd4119e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec162[i] = floorf(fZec160[i]);
			}
			
			// LOOP 0x7fedbd41fdc0
			// exec code
			for (int i=0; i<count; i++) {
				fZec167[i] = floorf(fZec165[i]);
			}
			
			// LOOP 0x7fedbd42d250
			// exec code
			for (int i=0; i<count; i++) {
				fZec174[i] = floorf(fZec172[i]);
			}
			
			// LOOP 0x7fedbd4399e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec179[i] = floorf(fZec177[i]);
			}
			
			// LOOP 0x7fedbd447a60
			// exec code
			for (int i=0; i<count; i++) {
				fZec186[i] = floorf(fZec184[i]);
			}
			
			// LOOP 0x7fedbd454c60
			// exec code
			for (int i=0; i<count; i++) {
				fZec191[i] = floorf(fZec189[i]);
			}
			
			// LOOP 0x7fedbd463890
			// exec code
			for (int i=0; i<count; i++) {
				fZec198[i] = floorf(fZec196[i]);
			}
			
			// LOOP 0x7fedbd471550
			// exec code
			for (int i=0; i<count; i++) {
				fZec203[i] = floorf(fZec201[i]);
			}
			
			// LOOP 0x7fedbd47df50
			// exec code
			for (int i=0; i<count; i++) {
				fZec206[i] = (fSlow65 + fVec5[i-1]);
			}
			
			// SECTION : 9
			// LOOP 0x7fedbc6994d0
			// exec code
			for (int i=0; i<count; i++) {
				iZec145[i] = int(min(8192, max(0, int(fZec144[i]))));
			}
			
			// LOOP 0x7fedbd0b33c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec8[i] = cosf(fZec5[i]);
			}
			
			// LOOP 0x7fedbd0ba1f0
			// exec code
			for (int i=0; i<count; i++) {
				iZec43[i] = int(min(8192, max(0, int(fZec42[i]))));
			}
			
			// LOOP 0x7fedbd0bb120
			// exec code
			for (int i=0; i<count; i++) {
				fZec45[i] = (fZec44[i] + (2.0f - fRec45[i]));
			}
			
			// LOOP 0x7fedbd0bbbd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec46[i] = (fRec45[i] - fZec44[i]);
			}
			
			// LOOP 0x7fedbd0bff20
			// exec code
			for (int i=0; i<count; i++) {
				iZec48[i] = int(min(8192, max(0, int(fZec47[i]))));
			}
			
			// LOOP 0x7fedbd0c0d60
			// exec code
			for (int i=0; i<count; i++) {
				fZec50[i] = (fZec49[i] + (2.0f - fRec47[i]));
			}
			
			// LOOP 0x7fedbd0c1760
			// exec code
			for (int i=0; i<count; i++) {
				fZec51[i] = (fRec47[i] - fZec49[i]);
			}
			
			// LOOP 0x7fedbd0c3660
			// exec code
			for (int i=0; i<count; i++) {
				fZec92[i] = (0 - fZec91[i]);
			}
			
			// LOOP 0x7fedbd0c68f0
			// exec code
			for (int i=0; i<count; i++) {
				iZec104[i] = int(min(8192, max(0, int(fZec103[i]))));
			}
			
			// LOOP 0x7fedbd0c75c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec106[i] = (fZec105[i] + (2.0f - fRec57[i]));
			}
			
			// LOOP 0x7fedbd0c7fe0
			// exec code
			for (int i=0; i<count; i++) {
				fZec107[i] = (fRec57[i] - fZec105[i]);
			}
			
			// LOOP 0x7fedbd0cf590
			// exec code
			for (int i=0; i<count; i++) {
				iZec109[i] = int(min(8192, max(0, int(fZec108[i]))));
			}
			
			// LOOP 0x7fedbd0d0460
			// exec code
			for (int i=0; i<count; i++) {
				fZec111[i] = (fZec110[i] + (2.0f - fRec59[i]));
			}
			
			// LOOP 0x7fedbd0d0e30
			// exec code
			for (int i=0; i<count; i++) {
				fZec112[i] = (fRec59[i] - fZec110[i]);
			}
			
			// LOOP 0x7fedbd0d9990
			// exec code
			for (int i=0; i<count; i++) {
				iZec116[i] = int(min(8192, max(0, int(fZec115[i]))));
			}
			
			// LOOP 0x7fedbd0da5e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec118[i] = (fZec117[i] + (2.0f - fRec60[i]));
			}
			
			// LOOP 0x7fedbd0db030
			// exec code
			for (int i=0; i<count; i++) {
				fZec119[i] = (fRec60[i] - fZec117[i]);
			}
			
			// LOOP 0x7fedbd0e2eb0
			// exec code
			for (int i=0; i<count; i++) {
				iZec121[i] = int(min(8192, max(0, int(fZec120[i]))));
			}
			
			// LOOP 0x7fedbd0e3d80
			// exec code
			for (int i=0; i<count; i++) {
				fZec123[i] = (fZec122[i] + (2.0f - fRec62[i]));
			}
			
			// LOOP 0x7fedbd0e4750
			// exec code
			for (int i=0; i<count; i++) {
				fZec124[i] = (fRec62[i] - fZec122[i]);
			}
			
			// LOOP 0x7fedbd0ede90
			// exec code
			for (int i=0; i<count; i++) {
				iZec128[i] = int(min(8192, max(0, int(fZec127[i]))));
			}
			
			// LOOP 0x7fedbd0eeae0
			// exec code
			for (int i=0; i<count; i++) {
				fZec130[i] = (fZec129[i] + (2.0f - fRec63[i]));
			}
			
			// LOOP 0x7fedbd0ef530
			// exec code
			for (int i=0; i<count; i++) {
				fZec131[i] = (fRec63[i] - fZec129[i]);
			}
			
			// LOOP 0x7fedbd0f7e70
			// exec code
			for (int i=0; i<count; i++) {
				iZec133[i] = int(min(8192, max(0, int(fZec132[i]))));
			}
			
			// LOOP 0x7fedbd0f8d40
			// exec code
			for (int i=0; i<count; i++) {
				fZec135[i] = (fZec134[i] + (2.0f - fRec65[i]));
			}
			
			// LOOP 0x7fedbd0f9710
			// exec code
			for (int i=0; i<count; i++) {
				fZec136[i] = (fRec65[i] - fZec134[i]);
			}
			
			// LOOP 0x7fedbd1b8600
			// exec code
			for (int i=0; i<count; i++) {
				fZec7[i] = (0 - fZec6[i]);
			}
			
			// LOOP 0x7fedbd1ba350
			// exec code
			for (int i=0; i<count; i++) {
				iZec12[i] = int(fZec11[i]);
			}
			
			// LOOP 0x7fedbd1bdba0
			// exec code
			for (int i=0; i<count; i++) {
				fZec16[i] = (fZec10[i] + (7.0f - fZec13[i]));
			}
			
			// LOOP 0x7fedbd1be4d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec17[i] = (fZec10[i] + (6.0f - fZec13[i]));
			}
			
			// LOOP 0x7fedbd1c1280
			// exec code
			for (int i=0; i<count; i++) {
				fZec18[i] = (fZec14[i] * fZec15[i]);
			}
			
			// LOOP 0x7fedbd1c5260
			// exec code
			for (int i=0; i<count; i++) {
				iZec30[i] = int(fZec29[i]);
			}
			
			// LOOP 0x7fedbd1c79d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec34[i] = (fZec28[i] + (7.0f - fZec31[i]));
			}
			
			// LOOP 0x7fedbd1c8200
			// exec code
			for (int i=0; i<count; i++) {
				fZec35[i] = (fZec28[i] + (6.0f - fZec31[i]));
			}
			
			// LOOP 0x7fedbd1cab00
			// exec code
			for (int i=0; i<count; i++) {
				fZec36[i] = (fZec32[i] * fZec33[i]);
			}
			
			// LOOP 0x7fedbd1d0fc0
			// exec code
			for (int i=0; i<count; i++) {
				iZec55[i] = int(min(8192, max(0, int(fZec54[i]))));
			}
			
			// LOOP 0x7fedbd1d1df0
			// exec code
			for (int i=0; i<count; i++) {
				fZec57[i] = (fZec56[i] + (2.0f - fRec48[i]));
			}
			
			// LOOP 0x7fedbd1d27a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec58[i] = (fRec48[i] - fZec56[i]);
			}
			
			// LOOP 0x7fedbd1d74d0
			// exec code
			for (int i=0; i<count; i++) {
				iZec60[i] = int(min(8192, max(0, int(fZec59[i]))));
			}
			
			// LOOP 0x7fedbd1d8340
			// exec code
			for (int i=0; i<count; i++) {
				fZec62[i] = (fZec61[i] + (2.0f - fRec50[i]));
			}
			
			// LOOP 0x7fedbd1d8d40
			// exec code
			for (int i=0; i<count; i++) {
				fZec63[i] = (fRec50[i] - fZec61[i]);
			}
			
			// LOOP 0x7fedbd1df190
			// exec code
			for (int i=0; i<count; i++) {
				iZec67[i] = int(min(8192, max(0, int(fZec66[i]))));
			}
			
			// LOOP 0x7fedbd1dff20
			// exec code
			for (int i=0; i<count; i++) {
				fZec69[i] = (fZec68[i] + (2.0f - fRec51[i]));
			}
			
			// LOOP 0x7fedbd1e07f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec70[i] = (fRec51[i] - fZec68[i]);
			}
			
			// LOOP 0x7fedbd1e5c00
			// exec code
			for (int i=0; i<count; i++) {
				iZec72[i] = int(min(8192, max(0, int(fZec71[i]))));
			}
			
			// LOOP 0x7fedbd1e6a30
			// exec code
			for (int i=0; i<count; i++) {
				fZec74[i] = (fZec73[i] + (2.0f - fRec53[i]));
			}
			
			// LOOP 0x7fedbd1e73a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec75[i] = (fRec53[i] - fZec73[i]);
			}
			
			// LOOP 0x7fedbd1ee280
			// exec code
			for (int i=0; i<count; i++) {
				iZec79[i] = int(min(8192, max(0, int(fZec78[i]))));
			}
			
			// LOOP 0x7fedbd1eeec0
			// exec code
			for (int i=0; i<count; i++) {
				fZec81[i] = (fZec80[i] + (2.0f - fRec54[i]));
			}
			
			// LOOP 0x7fedbd1ef930
			// exec code
			for (int i=0; i<count; i++) {
				fZec82[i] = (fRec54[i] - fZec80[i]);
			}
			
			// LOOP 0x7fedbd1f5dd0
			// exec code
			for (int i=0; i<count; i++) {
				iZec84[i] = int(min(8192, max(0, int(fZec83[i]))));
			}
			
			// LOOP 0x7fedbd1f6c10
			// exec code
			for (int i=0; i<count; i++) {
				fZec86[i] = (fZec85[i] + (2.0f - fRec56[i]));
			}
			
			// LOOP 0x7fedbd1f7610
			// exec code
			for (int i=0; i<count; i++) {
				fZec87[i] = (fRec56[i] - fZec85[i]);
			}
			
			// LOOP 0x7fedbd22c640
			// exec code
			for (int i=0; i<count; i++) {
				iZec19[i] = int(min((float)65536, max((float)0, fRec43[i])));
			}
			
			// LOOP 0x7fedbd22cd80
			// exec code
			for (int i=0; i<count; i++) {
				fZec20[i] = (1.0f - fRec42[i]);
			}
			
			// LOOP 0x7fedbd22d4b0
			// exec code
			for (int i=0; i<count; i++) {
				iZec21[i] = int(min((float)65536, max((float)0, fRec44[i])));
			}
			
			// LOOP 0x7fedbd22de90
			// exec code
			for (int i=0; i<count; i++) {
				fZec22[i] = (fSlow8 + fVec4[i-1]);
			}
			
			// LOOP 0x7fedbd301fb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec89[i] = cosf(fZec88[i]);
			}
			
			// LOOP 0x7fedbd307f20
			// exec code
			for (int i=0; i<count; i++) {
				fZec147[i] = (fZec146[i] + (2.0f - fRec68[i]));
			}
			
			// LOOP 0x7fedbd308440
			// exec code
			for (int i=0; i<count; i++) {
				fZec148[i] = (fRec68[i] - fZec146[i]);
			}
			
			// LOOP 0x7fedbd407990
			// exec code
			for (int i=0; i<count; i++) {
				iZec140[i] = int(min(8192, max(0, int(fZec139[i]))));
			}
			
			// LOOP 0x7fedbd4085e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec142[i] = (fZec141[i] + (2.0f - fRec66[i]));
			}
			
			// LOOP 0x7fedbd409030
			// exec code
			for (int i=0; i<count; i++) {
				fZec143[i] = (fRec66[i] - fZec141[i]);
			}
			
			// LOOP 0x7fedbd411900
			// exec code
			for (int i=0; i<count; i++) {
				fZec163[i] = (fZec162[i] + (2.0f - fRec69[i]));
			}
			
			// LOOP 0x7fedbd4122a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec164[i] = (fRec69[i] - fZec162[i]);
			}
			
			// LOOP 0x7fedbd4148e0
			// exec code
			for (int i=0; i<count; i++) {
				iZec161[i] = int(min(8192, max(0, int(fZec160[i]))));
			}
			
			// LOOP 0x7fedbd41ed70
			// exec code
			for (int i=0; i<count; i++) {
				iZec166[i] = int(min(8192, max(0, int(fZec165[i]))));
			}
			
			// LOOP 0x7fedbd41fc40
			// exec code
			for (int i=0; i<count; i++) {
				fZec168[i] = (fZec167[i] + (2.0f - fRec71[i]));
			}
			
			// LOOP 0x7fedbd420610
			// exec code
			for (int i=0; i<count; i++) {
				fZec169[i] = (fRec71[i] - fZec167[i]);
			}
			
			// LOOP 0x7fedbd42c520
			// exec code
			for (int i=0; i<count; i++) {
				iZec173[i] = int(min(8192, max(0, int(fZec172[i]))));
			}
			
			// LOOP 0x7fedbd42d170
			// exec code
			for (int i=0; i<count; i++) {
				fZec175[i] = (fZec174[i] + (2.0f - fRec72[i]));
			}
			
			// LOOP 0x7fedbd42dbc0
			// exec code
			for (int i=0; i<count; i++) {
				fZec176[i] = (fRec72[i] - fZec174[i]);
			}
			
			// LOOP 0x7fedbd438990
			// exec code
			for (int i=0; i<count; i++) {
				iZec178[i] = int(min(8192, max(0, int(fZec177[i]))));
			}
			
			// LOOP 0x7fedbd439860
			// exec code
			for (int i=0; i<count; i++) {
				fZec180[i] = (fZec179[i] + (2.0f - fRec74[i]));
			}
			
			// LOOP 0x7fedbd43a230
			// exec code
			for (int i=0; i<count; i++) {
				fZec181[i] = (fRec74[i] - fZec179[i]);
			}
			
			// LOOP 0x7fedbd446d30
			// exec code
			for (int i=0; i<count; i++) {
				iZec185[i] = int(min(8192, max(0, int(fZec184[i]))));
			}
			
			// LOOP 0x7fedbd447980
			// exec code
			for (int i=0; i<count; i++) {
				fZec187[i] = (fZec186[i] + (2.0f - fRec75[i]));
			}
			
			// LOOP 0x7fedbd4483d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec188[i] = (fRec75[i] - fZec186[i]);
			}
			
			// LOOP 0x7fedbd453c10
			// exec code
			for (int i=0; i<count; i++) {
				iZec190[i] = int(min(8192, max(0, int(fZec189[i]))));
			}
			
			// LOOP 0x7fedbd454ae0
			// exec code
			for (int i=0; i<count; i++) {
				fZec192[i] = (fZec191[i] + (2.0f - fRec77[i]));
			}
			
			// LOOP 0x7fedbd4554b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec193[i] = (fRec77[i] - fZec191[i]);
			}
			
			// LOOP 0x7fedbd462b60
			// exec code
			for (int i=0; i<count; i++) {
				iZec197[i] = int(min(8192, max(0, int(fZec196[i]))));
			}
			
			// LOOP 0x7fedbd4637b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec199[i] = (fZec198[i] + (2.0f - fRec78[i]));
			}
			
			// LOOP 0x7fedbd464200
			// exec code
			for (int i=0; i<count; i++) {
				fZec200[i] = (fRec78[i] - fZec198[i]);
			}
			
			// LOOP 0x7fedbd470500
			// exec code
			for (int i=0; i<count; i++) {
				iZec202[i] = int(min(8192, max(0, int(fZec201[i]))));
			}
			
			// LOOP 0x7fedbd4713d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec204[i] = (fZec203[i] + (2.0f - fRec80[i]));
			}
			
			// LOOP 0x7fedbd471da0
			// exec code
			for (int i=0; i<count; i++) {
				fZec205[i] = (fRec80[i] - fZec203[i]);
			}
			
			// LOOP 0x7fedbd47de60
			// exec code
			for (int i=0; i<count; i++) {
				fZec207[i] = (1.0f - (0.5f * fZec206[i]));
			}
			
			// SECTION : 10
			// LOOP 0x7fedbd1aa2c0
			// pre processing
			fYec0_idx = (fYec0_idx+fYec0_idx_save)&131071;
			fYec1_idx = (fYec1_idx+fYec1_idx_save)&131071;
			fYec2_idx = (fYec2_idx+fYec2_idx_save)&16383;
			for (int i=0; i<4; i++) fRec38_tmp[i]=fRec38_perm[i];
			for (int i=0; i<4; i++) fRec36_tmp[i]=fRec36_perm[i];
			fYec3_idx = (fYec3_idx+fYec3_idx_save)&16383;
			for (int i=0; i<4; i++) fRec46_tmp[i]=fRec46_perm[i];
			for (int i=0; i<4; i++) fRec37_tmp[i]=fRec37_perm[i];
			fYec4_idx = (fYec4_idx+fYec4_idx_save)&16383;
			for (int i=0; i<4; i++) fRec35_tmp[i]=fRec35_perm[i];
			for (int i=0; i<4; i++) fRec33_tmp[i]=fRec33_perm[i];
			fYec5_idx = (fYec5_idx+fYec5_idx_save)&16383;
			for (int i=0; i<4; i++) fRec49_tmp[i]=fRec49_perm[i];
			for (int i=0; i<4; i++) fRec34_tmp[i]=fRec34_perm[i];
			fYec6_idx = (fYec6_idx+fYec6_idx_save)&16383;
			for (int i=0; i<4; i++) fRec32_tmp[i]=fRec32_perm[i];
			for (int i=0; i<4; i++) fRec30_tmp[i]=fRec30_perm[i];
			fYec7_idx = (fYec7_idx+fYec7_idx_save)&16383;
			for (int i=0; i<4; i++) fRec52_tmp[i]=fRec52_perm[i];
			for (int i=0; i<4; i++) fRec31_tmp[i]=fRec31_perm[i];
			fYec8_idx = (fYec8_idx+fYec8_idx_save)&16383;
			for (int i=0; i<4; i++) fRec29_tmp[i]=fRec29_perm[i];
			for (int i=0; i<4; i++) fRec27_tmp[i]=fRec27_perm[i];
			fYec9_idx = (fYec9_idx+fYec9_idx_save)&16383;
			for (int i=0; i<4; i++) fRec55_tmp[i]=fRec55_perm[i];
			for (int i=0; i<4; i++) fRec28_tmp[i]=fRec28_perm[i];
			fYec10_idx = (fYec10_idx+fYec10_idx_save)&16383;
			for (int i=0; i<4; i++) fRec26_tmp[i]=fRec26_perm[i];
			for (int i=0; i<4; i++) fRec24_tmp[i]=fRec24_perm[i];
			fYec11_idx = (fYec11_idx+fYec11_idx_save)&16383;
			for (int i=0; i<4; i++) fRec58_tmp[i]=fRec58_perm[i];
			for (int i=0; i<4; i++) fRec25_tmp[i]=fRec25_perm[i];
			fYec12_idx = (fYec12_idx+fYec12_idx_save)&16383;
			for (int i=0; i<4; i++) fRec23_tmp[i]=fRec23_perm[i];
			for (int i=0; i<4; i++) fRec21_tmp[i]=fRec21_perm[i];
			fYec13_idx = (fYec13_idx+fYec13_idx_save)&16383;
			for (int i=0; i<4; i++) fRec61_tmp[i]=fRec61_perm[i];
			for (int i=0; i<4; i++) fRec22_tmp[i]=fRec22_perm[i];
			fYec14_idx = (fYec14_idx+fYec14_idx_save)&16383;
			for (int i=0; i<4; i++) fRec20_tmp[i]=fRec20_perm[i];
			for (int i=0; i<4; i++) fRec18_tmp[i]=fRec18_perm[i];
			fYec15_idx = (fYec15_idx+fYec15_idx_save)&16383;
			for (int i=0; i<4; i++) fRec64_tmp[i]=fRec64_perm[i];
			for (int i=0; i<4; i++) fRec19_tmp[i]=fRec19_perm[i];
			fYec16_idx = (fYec16_idx+fYec16_idx_save)&16383;
			for (int i=0; i<4; i++) fRec17_tmp[i]=fRec17_perm[i];
			for (int i=0; i<4; i++) fRec15_tmp[i]=fRec15_perm[i];
			fYec17_idx = (fYec17_idx+fYec17_idx_save)&16383;
			for (int i=0; i<4; i++) fRec67_tmp[i]=fRec67_perm[i];
			for (int i=0; i<4; i++) fRec16_tmp[i]=fRec16_perm[i];
			fYec18_idx = (fYec18_idx+fYec18_idx_save)&16383;
			for (int i=0; i<4; i++) fRec14_tmp[i]=fRec14_perm[i];
			for (int i=0; i<4; i++) fRec12_tmp[i]=fRec12_perm[i];
			fYec19_idx = (fYec19_idx+fYec19_idx_save)&16383;
			for (int i=0; i<4; i++) fRec70_tmp[i]=fRec70_perm[i];
			for (int i=0; i<4; i++) fRec13_tmp[i]=fRec13_perm[i];
			fYec20_idx = (fYec20_idx+fYec20_idx_save)&16383;
			for (int i=0; i<4; i++) fRec11_tmp[i]=fRec11_perm[i];
			for (int i=0; i<4; i++) fRec9_tmp[i]=fRec9_perm[i];
			fYec21_idx = (fYec21_idx+fYec21_idx_save)&16383;
			for (int i=0; i<4; i++) fRec73_tmp[i]=fRec73_perm[i];
			for (int i=0; i<4; i++) fRec10_tmp[i]=fRec10_perm[i];
			fYec22_idx = (fYec22_idx+fYec22_idx_save)&16383;
			for (int i=0; i<4; i++) fRec8_tmp[i]=fRec8_perm[i];
			for (int i=0; i<4; i++) fRec6_tmp[i]=fRec6_perm[i];
			fYec23_idx = (fYec23_idx+fYec23_idx_save)&16383;
			for (int i=0; i<4; i++) fRec76_tmp[i]=fRec76_perm[i];
			for (int i=0; i<4; i++) fRec7_tmp[i]=fRec7_perm[i];
			fYec24_idx = (fYec24_idx+fYec24_idx_save)&16383;
			for (int i=0; i<4; i++) fRec5_tmp[i]=fRec5_perm[i];
			for (int i=0; i<4; i++) fRec3_tmp[i]=fRec3_perm[i];
			fYec25_idx = (fYec25_idx+fYec25_idx_save)&16383;
			for (int i=0; i<4; i++) fRec79_tmp[i]=fRec79_perm[i];
			for (int i=0; i<4; i++) fRec4_tmp[i]=fRec4_perm[i];
			for (int i=0; i<4; i++) fRec2_tmp[i]=fRec2_perm[i];
			fRec0_idx = (fRec0_idx+fRec0_idx_save)&1023;
			for (int i=0; i<4; i++) fRec81_tmp[i]=fRec81_perm[i];
			fRec1_idx = (fRec1_idx+fRec1_idx_save)&1023;
			// exec code
			for (int i=0; i<count; i++) {
				fYec0[(fYec0_idx+i)&131071] = (((((fRec0[(fRec0_idx+i-int((int(min(512, max(0, iZec12[i]))) + 1)))&1023] * (0 - fZec14[i])) * (0 - (0.5f * fZec15[i]))) * (0 - (0.33333334f * fZec16[i]))) * (0 - (0.25f * fZec17[i]))) + ((fZec10[i] + (1e+01f - fZec13[i])) * ((((((fRec0[(fRec0_idx+i-int((int(min(512, max(0, (iZec12[i] + 1)))) + 1)))&1023] * (0 - fZec15[i])) * (0 - (0.5f * fZec16[i]))) * (0 - (0.33333334f * fZec17[i]))) + (0.5f * (((fZec14[i] * fRec0[(fRec0_idx+i-int((int(min(512, max(0, (iZec12[i] + 2)))) + 1)))&1023]) * (0 - fZec16[i])) * (0 - (0.5f * fZec17[i]))))) + (0.16666667f * ((fZec18[i] * fRec0[(fRec0_idx+i-int((int(min(512, max(0, (iZec12[i] + 3)))) + 1)))&1023]) * (0 - fZec17[i])))) + (0.041666668f * ((fZec18[i] * fZec16[i]) * fRec0[(fRec0_idx+i-int((int(min(512, max(0, (iZec12[i] + 4)))) + 1)))&1023])))));
				fZec23[i] = ((float)input0[i] + (0.5f * (((fYec0[(fYec0_idx+i-iZec19[i])&131071] * fZec20[i]) + (fYec0[(fYec0_idx+i-iZec21[i])&131071] * fRec42[i])) * fZec22[i])));
				fZec24[i] = ((fZec7[i] * fRec27[i-1]) + (fZec8[i] * fZec23[i]));
				fZec25[i] = ((fZec7[i] * fRec30[i-1]) + (fZec8[i] * fZec24[i]));
				fZec26[i] = ((fZec7[i] * fRec33[i-1]) + (fZec8[i] * fZec25[i]));
				fZec27[i] = ((fZec7[i] * fRec36[i-1]) + (fZec8[i] * fZec26[i]));
				fYec1[(fYec1_idx+i)&131071] = (((((fRec1[(fRec1_idx+i-int((int(min(512, max(0, iZec30[i]))) + 1)))&1023] * (0 - fZec32[i])) * (0 - (0.5f * fZec33[i]))) * (0 - (0.33333334f * fZec34[i]))) * (0 - (0.25f * fZec35[i]))) + ((fZec28[i] + (1e+01f - fZec31[i])) * ((((((fRec1[(fRec1_idx+i-int((int(min(512, max(0, (iZec30[i] + 1)))) + 1)))&1023] * (0 - fZec33[i])) * (0 - (0.5f * fZec34[i]))) * (0 - (0.33333334f * fZec35[i]))) + (0.5f * (((fZec32[i] * fRec1[(fRec1_idx+i-int((int(min(512, max(0, (iZec30[i] + 2)))) + 1)))&1023]) * (0 - fZec34[i])) * (0 - (0.5f * fZec35[i]))))) + (0.16666667f * ((fZec36[i] * fRec1[(fRec1_idx+i-int((int(min(512, max(0, (iZec30[i] + 3)))) + 1)))&1023]) * (0 - fZec35[i])))) + (0.041666668f * ((fZec36[i] * fZec34[i]) * fRec1[(fRec1_idx+i-int((int(min(512, max(0, (iZec30[i] + 4)))) + 1)))&1023])))));
				fZec37[i] = ((float)input1[i] + (0.5f * (fZec22[i] * ((fYec1[(fYec1_idx+i-iZec21[i])&131071] * fRec42[i]) + (fYec1[(fYec1_idx+i-iZec19[i])&131071] * fZec20[i])))));
				fZec38[i] = ((fZec7[i] * fRec28[i-1]) + (fZec8[i] * fZec37[i]));
				fZec39[i] = ((fZec7[i] * fRec31[i-1]) + (fZec8[i] * fZec38[i]));
				fZec40[i] = ((fZec7[i] * fRec34[i-1]) + (fZec8[i] * fZec39[i]));
				fZec41[i] = ((fZec7[i] * fRec37[i-1]) + (fZec8[i] * fZec40[i]));
				fYec2[(fYec2_idx+i)&16383] = ((6.123234e-17f * fZec27[i]) - fZec41[i]);
				fRec38[i] = (fYec2[(fYec2_idx+i-int((iZec43[i] + 1)))&16383] + ((fRec38[i-1] * (0 - (fZec45[i] / fZec46[i]))) + ((fZec45[i] * fYec2[(fYec2_idx+i-iZec43[i])&16383]) / fZec46[i])));
				fRec36[i] = fRec38[i];
				fYec3[(fYec3_idx+i)&16383] = (fZec27[i] + (6.123234e-17f * fZec41[i]));
				fRec46[i] = (fYec3[(fYec3_idx+i-int((iZec48[i] + 1)))&16383] + ((fRec46[i-1] * (0 - (fZec50[i] / fZec51[i]))) + ((fZec50[i] * fYec3[(fYec3_idx+i-iZec48[i])&16383]) / fZec51[i])));
				fRec37[i] = fRec46[i];
				fZec52[i] = ((fRec36[i-1] * fZec8[i]) + (fZec6[i] * fZec26[i]));
				fZec53[i] = ((fZec8[i] * fRec37[i-1]) + (fZec6[i] * fZec40[i]));
				fYec4[(fYec4_idx+i)&16383] = ((6.123234e-17f * fZec52[i]) - fZec53[i]);
				fRec35[i] = (fYec4[(fYec4_idx+i-int((iZec55[i] + 1)))&16383] + ((fRec35[i-1] * (0 - (fZec57[i] / fZec58[i]))) + ((fZec57[i] * fYec4[(fYec4_idx+i-iZec55[i])&16383]) / fZec58[i])));
				fRec33[i] = fRec35[i];
				fYec5[(fYec5_idx+i)&16383] = (fZec52[i] + (6.123234e-17f * fZec53[i]));
				fRec49[i] = (fYec5[(fYec5_idx+i-int((iZec60[i] + 1)))&16383] + ((fRec49[i-1] * (0 - (fZec62[i] / fZec63[i]))) + ((fZec62[i] * fYec5[(fYec5_idx+i-iZec60[i])&16383]) / fZec63[i])));
				fRec34[i] = fRec49[i];
				fZec64[i] = ((fRec33[i-1] * fZec8[i]) + (fZec6[i] * fZec25[i]));
				fZec65[i] = ((fZec8[i] * fRec34[i-1]) + (fZec6[i] * fZec39[i]));
				fYec6[(fYec6_idx+i)&16383] = ((6.123234e-17f * fZec64[i]) - fZec65[i]);
				fRec32[i] = (fYec6[(fYec6_idx+i-int((iZec67[i] + 1)))&16383] + (((0 - (fZec69[i] / fZec70[i])) * fRec32[i-1]) + ((fZec69[i] * fYec6[(fYec6_idx+i-iZec67[i])&16383]) / fZec70[i])));
				fRec30[i] = fRec32[i];
				fYec7[(fYec7_idx+i)&16383] = ((6.123234e-17f * fZec65[i]) + fZec64[i]);
				fRec52[i] = (fYec7[(fYec7_idx+i-int((iZec72[i] + 1)))&16383] + ((fRec52[i-1] * (0 - (fZec74[i] / fZec75[i]))) + ((fYec7[(fYec7_idx+i-iZec72[i])&16383] * fZec74[i]) / fZec75[i])));
				fRec31[i] = fRec52[i];
				fZec76[i] = ((fRec30[i-1] * fZec8[i]) + (fZec6[i] * fZec24[i]));
				fZec77[i] = ((fZec8[i] * fRec31[i-1]) + (fZec6[i] * fZec38[i]));
				fYec8[(fYec8_idx+i)&16383] = ((6.123234e-17f * fZec76[i]) - fZec77[i]);
				fRec29[i] = ((((fYec8[(fYec8_idx+i-iZec79[i])&16383] * fZec81[i]) / fZec82[i]) + fYec8[(fYec8_idx+i-int((iZec79[i] + 1)))&16383]) + (fRec29[i-1] * (0 - (fZec81[i] / fZec82[i]))));
				fRec27[i] = fRec29[i];
				fYec9[(fYec9_idx+i)&16383] = (fZec76[i] + (6.123234e-17f * fZec77[i]));
				fRec55[i] = (fYec9[(fYec9_idx+i-int((iZec84[i] + 1)))&16383] + ((fRec55[i-1] * (0 - (fZec86[i] / fZec87[i]))) + ((fZec86[i] * fYec9[(fYec9_idx+i-iZec84[i])&16383]) / fZec87[i])));
				fRec28[i] = fRec55[i];
				fZec90[i] = ((fRec27[i-1] * fZec8[i]) + (fZec6[i] * fZec23[i]));
				fZec93[i] = ((fZec90[i] * fZec89[i]) + (fZec92[i] * fRec15[i-1]));
				fZec94[i] = ((fZec89[i] * fZec93[i]) + (fZec92[i] * fRec18[i-1]));
				fZec95[i] = ((fZec89[i] * fZec94[i]) + (fZec92[i] * fRec21[i-1]));
				fZec96[i] = (fZec89[i] * fZec95[i]);
				fZec97[i] = (fRec24[i-1] * fZec92[i]);
				fZec98[i] = ((fZec8[i] * fRec28[i-1]) + (fZec6[i] * fZec37[i]));
				fZec99[i] = ((fRec16[i-1] * fZec92[i]) + (fZec89[i] * fZec98[i]));
				fZec100[i] = ((fRec19[i-1] * fZec92[i]) + (fZec89[i] * fZec99[i]));
				fZec101[i] = ((fRec22[i-1] * fZec92[i]) + (fZec89[i] * fZec100[i]));
				fZec102[i] = ((fRec25[i-1] * fZec92[i]) + (fZec89[i] * fZec101[i]));
				fYec10[(fYec10_idx+i)&16383] = ((6.123234e-17f * (fZec96[i] + fZec97[i])) - fZec102[i]);
				fRec26[i] = ((((fYec10[(fYec10_idx+i-iZec104[i])&16383] * fZec106[i]) / fZec107[i]) + fYec10[(fYec10_idx+i-int((iZec104[i] + 1)))&16383]) + (fRec26[i-1] * (0 - (fZec106[i] / fZec107[i]))));
				fRec24[i] = fRec26[i];
				fYec11[(fYec11_idx+i)&16383] = ((fZec96[i] + (6.123234e-17f * fZec102[i])) + fZec97[i]);
				fRec58[i] = (fYec11[(fYec11_idx+i-int((iZec109[i] + 1)))&16383] + ((fRec58[i-1] * (0 - (fZec111[i] / fZec112[i]))) + ((fZec111[i] * fYec11[(fYec11_idx+i-iZec109[i])&16383]) / fZec112[i])));
				fRec25[i] = fRec58[i];
				fZec113[i] = ((fZec89[i] * fRec24[i-1]) + (fZec95[i] * fZec91[i]));
				fZec114[i] = ((fZec89[i] * fRec25[i-1]) + (fZec101[i] * fZec91[i]));
				fYec12[(fYec12_idx+i)&16383] = ((6.123234e-17f * fZec113[i]) - fZec114[i]);
				fRec23[i] = ((((fYec12[(fYec12_idx+i-iZec116[i])&16383] * fZec118[i]) / fZec119[i]) + fYec12[(fYec12_idx+i-int((iZec116[i] + 1)))&16383]) + (fRec23[i-1] * (0 - (fZec118[i] / fZec119[i]))));
				fRec21[i] = fRec23[i];
				fYec13[(fYec13_idx+i)&16383] = (fZec113[i] + (6.123234e-17f * fZec114[i]));
				fRec61[i] = (fYec13[(fYec13_idx+i-int((iZec121[i] + 1)))&16383] + ((fRec61[i-1] * (0 - (fZec123[i] / fZec124[i]))) + ((fZec123[i] * fYec13[(fYec13_idx+i-iZec121[i])&16383]) / fZec124[i])));
				fRec22[i] = fRec61[i];
				fZec125[i] = ((fZec89[i] * fRec21[i-1]) + (fZec94[i] * fZec91[i]));
				fZec126[i] = ((fZec89[i] * fRec22[i-1]) + (fZec100[i] * fZec91[i]));
				fYec14[(fYec14_idx+i)&16383] = ((6.123234e-17f * fZec125[i]) - fZec126[i]);
				fRec20[i] = ((((fYec14[(fYec14_idx+i-iZec128[i])&16383] * fZec130[i]) / fZec131[i]) + fYec14[(fYec14_idx+i-int((iZec128[i] + 1)))&16383]) + (fRec20[i-1] * (0 - (fZec130[i] / fZec131[i]))));
				fRec18[i] = fRec20[i];
				fYec15[(fYec15_idx+i)&16383] = (fZec125[i] + (6.123234e-17f * fZec126[i]));
				fRec64[i] = (fYec15[(fYec15_idx+i-int((iZec133[i] + 1)))&16383] + ((fRec64[i-1] * (0 - (fZec135[i] / fZec136[i]))) + ((fZec135[i] * fYec15[(fYec15_idx+i-iZec133[i])&16383]) / fZec136[i])));
				fRec19[i] = fRec64[i];
				fZec137[i] = ((fZec89[i] * fRec18[i-1]) + (fZec93[i] * fZec91[i]));
				fZec138[i] = ((fZec89[i] * fRec19[i-1]) + (fZec99[i] * fZec91[i]));
				fYec16[(fYec16_idx+i)&16383] = ((6.123234e-17f * fZec137[i]) - fZec138[i]);
				fRec17[i] = ((((fYec16[(fYec16_idx+i-iZec140[i])&16383] * fZec142[i]) / fZec143[i]) + fYec16[(fYec16_idx+i-int((iZec140[i] + 1)))&16383]) + (fRec17[i-1] * (0 - (fZec142[i] / fZec143[i]))));
				fRec15[i] = fRec17[i];
				fYec17[(fYec17_idx+i)&16383] = (fZec137[i] + (6.123234e-17f * fZec138[i]));
				fRec67[i] = (fYec17[(fYec17_idx+i-int((iZec145[i] + 1)))&16383] + ((fRec67[i-1] * (0 - (fZec147[i] / fZec148[i]))) + ((fZec147[i] * fYec17[(fYec17_idx+i-iZec145[i])&16383]) / fZec148[i])));
				fRec16[i] = fRec67[i];
				fZec149[i] = ((fZec89[i] * fRec15[i-1]) + (fZec90[i] * fZec91[i]));
				fZec150[i] = ((fZec7[i] * fRec3[i-1]) + (fZec8[i] * fZec149[i]));
				fZec151[i] = ((fZec8[i] * fZec150[i]) + (fZec7[i] * fRec6[i-1]));
				fZec152[i] = ((fZec151[i] * fZec8[i]) + (fZec7[i] * fRec9[i-1]));
				fZec153[i] = (fZec152[i] * fZec8[i]);
				fZec154[i] = (fRec12[i-1] * fZec7[i]);
				fZec155[i] = ((fZec89[i] * fRec16[i-1]) + (fZec98[i] * fZec91[i]));
				fZec156[i] = ((fRec4[i-1] * fZec7[i]) + (fZec155[i] * fZec8[i]));
				fZec157[i] = ((fRec7[i-1] * fZec7[i]) + (fZec156[i] * fZec8[i]));
				fZec158[i] = ((fRec10[i-1] * fZec7[i]) + (fZec157[i] * fZec8[i]));
				fZec159[i] = ((fRec13[i-1] * fZec7[i]) + (fZec158[i] * fZec8[i]));
				fYec18[(fYec18_idx+i)&16383] = ((6.123234e-17f * (fZec153[i] + fZec154[i])) - fZec159[i]);
				fRec14[i] = ((((fYec18[(fYec18_idx+i-iZec161[i])&16383] * fZec163[i]) / fZec164[i]) + fYec18[(fYec18_idx+i-int((iZec161[i] + 1)))&16383]) + (fRec14[i-1] * (0 - (fZec163[i] / fZec164[i]))));
				fRec12[i] = fRec14[i];
				fYec19[(fYec19_idx+i)&16383] = ((fZec153[i] + (6.123234e-17f * fZec159[i])) + fZec154[i]);
				fRec70[i] = (fYec19[(fYec19_idx+i-int((iZec166[i] + 1)))&16383] + ((fRec70[i-1] * (0 - (fZec168[i] / fZec169[i]))) + ((fZec168[i] * fYec19[(fYec19_idx+i-iZec166[i])&16383]) / fZec169[i])));
				fRec13[i] = fRec70[i];
				fZec170[i] = ((fRec12[i-1] * fZec8[i]) + (fZec152[i] * fZec6[i]));
				fZec171[i] = ((fRec13[i-1] * fZec8[i]) + (fZec158[i] * fZec6[i]));
				fYec20[(fYec20_idx+i)&16383] = ((6.123234e-17f * fZec170[i]) - fZec171[i]);
				fRec11[i] = ((((fYec20[(fYec20_idx+i-iZec173[i])&16383] * fZec175[i]) / fZec176[i]) + fYec20[(fYec20_idx+i-int((iZec173[i] + 1)))&16383]) + (fRec11[i-1] * (0 - (fZec175[i] / fZec176[i]))));
				fRec9[i] = fRec11[i];
				fYec21[(fYec21_idx+i)&16383] = (fZec170[i] + (6.123234e-17f * fZec171[i]));
				fRec73[i] = (fYec21[(fYec21_idx+i-int((iZec178[i] + 1)))&16383] + ((fRec73[i-1] * (0 - (fZec180[i] / fZec181[i]))) + ((fZec180[i] * fYec21[(fYec21_idx+i-iZec178[i])&16383]) / fZec181[i])));
				fRec10[i] = fRec73[i];
				fZec182[i] = ((fRec9[i-1] * fZec8[i]) + (fZec151[i] * fZec6[i]));
				fZec183[i] = ((fRec10[i-1] * fZec8[i]) + (fZec157[i] * fZec6[i]));
				fYec22[(fYec22_idx+i)&16383] = ((6.123234e-17f * fZec182[i]) - fZec183[i]);
				fRec8[i] = ((((fYec22[(fYec22_idx+i-iZec185[i])&16383] * fZec187[i]) / fZec188[i]) + fYec22[(fYec22_idx+i-int((iZec185[i] + 1)))&16383]) + (fRec8[i-1] * (0 - (fZec187[i] / fZec188[i]))));
				fRec6[i] = fRec8[i];
				fYec23[(fYec23_idx+i)&16383] = (fZec182[i] + (6.123234e-17f * fZec183[i]));
				fRec76[i] = (fYec23[(fYec23_idx+i-int((iZec190[i] + 1)))&16383] + ((fRec76[i-1] * (0 - (fZec192[i] / fZec193[i]))) + ((fZec192[i] * fYec23[(fYec23_idx+i-iZec190[i])&16383]) / fZec193[i])));
				fRec7[i] = fRec76[i];
				fZec194[i] = ((fRec6[i-1] * fZec8[i]) + (fZec6[i] * fZec150[i]));
				fZec195[i] = ((fRec7[i-1] * fZec8[i]) + (fZec156[i] * fZec6[i]));
				fYec24[(fYec24_idx+i)&16383] = ((6.123234e-17f * fZec194[i]) - fZec195[i]);
				fRec5[i] = ((((fYec24[(fYec24_idx+i-iZec197[i])&16383] * fZec199[i]) / fZec200[i]) + fYec24[(fYec24_idx+i-int((iZec197[i] + 1)))&16383]) + ((0 - (fZec199[i] / fZec200[i])) * fRec5[i-1]));
				fRec3[i] = fRec5[i];
				fYec25[(fYec25_idx+i)&16383] = (fZec194[i] + (6.123234e-17f * fZec195[i]));
				fRec79[i] = (fYec25[(fYec25_idx+i-int((iZec202[i] + 1)))&16383] + ((fRec79[i-1] * (0 - (fZec204[i] / fZec205[i]))) + ((fZec204[i] * fYec25[(fYec25_idx+i-iZec202[i])&16383]) / fZec205[i])));
				fRec4[i] = fRec79[i];
				fRec2[i] = ((((fRec3[i-1] * fZec8[i]) + (fZec6[i] * fZec149[i])) * fZec207[i]) + (0.5f * (fZec206[i] * fRec2[i-1])));
				fRec0[(fRec0_idx+i)&1023] = fRec2[i];
				fRec81[i] = ((0.5f * (fZec206[i] * fRec81[i-1])) + (fZec207[i] * ((fRec4[i-1] * fZec8[i]) + (fZec155[i] * fZec6[i]))));
				fRec1[(fRec1_idx+i)&1023] = fRec81[i];
			}
			// post processing
			fRec1_idx_save = count;
			for (int i=0; i<4; i++) fRec81_perm[i]=fRec81_tmp[count+i];
			fRec0_idx_save = count;
			for (int i=0; i<4; i++) fRec2_perm[i]=fRec2_tmp[count+i];
			for (int i=0; i<4; i++) fRec4_perm[i]=fRec4_tmp[count+i];
			for (int i=0; i<4; i++) fRec79_perm[i]=fRec79_tmp[count+i];
			fYec25_idx_save = count;
			for (int i=0; i<4; i++) fRec3_perm[i]=fRec3_tmp[count+i];
			for (int i=0; i<4; i++) fRec5_perm[i]=fRec5_tmp[count+i];
			fYec24_idx_save = count;
			for (int i=0; i<4; i++) fRec7_perm[i]=fRec7_tmp[count+i];
			for (int i=0; i<4; i++) fRec76_perm[i]=fRec76_tmp[count+i];
			fYec23_idx_save = count;
			for (int i=0; i<4; i++) fRec6_perm[i]=fRec6_tmp[count+i];
			for (int i=0; i<4; i++) fRec8_perm[i]=fRec8_tmp[count+i];
			fYec22_idx_save = count;
			for (int i=0; i<4; i++) fRec10_perm[i]=fRec10_tmp[count+i];
			for (int i=0; i<4; i++) fRec73_perm[i]=fRec73_tmp[count+i];
			fYec21_idx_save = count;
			for (int i=0; i<4; i++) fRec9_perm[i]=fRec9_tmp[count+i];
			for (int i=0; i<4; i++) fRec11_perm[i]=fRec11_tmp[count+i];
			fYec20_idx_save = count;
			for (int i=0; i<4; i++) fRec13_perm[i]=fRec13_tmp[count+i];
			for (int i=0; i<4; i++) fRec70_perm[i]=fRec70_tmp[count+i];
			fYec19_idx_save = count;
			for (int i=0; i<4; i++) fRec12_perm[i]=fRec12_tmp[count+i];
			for (int i=0; i<4; i++) fRec14_perm[i]=fRec14_tmp[count+i];
			fYec18_idx_save = count;
			for (int i=0; i<4; i++) fRec16_perm[i]=fRec16_tmp[count+i];
			for (int i=0; i<4; i++) fRec67_perm[i]=fRec67_tmp[count+i];
			fYec17_idx_save = count;
			for (int i=0; i<4; i++) fRec15_perm[i]=fRec15_tmp[count+i];
			for (int i=0; i<4; i++) fRec17_perm[i]=fRec17_tmp[count+i];
			fYec16_idx_save = count;
			for (int i=0; i<4; i++) fRec19_perm[i]=fRec19_tmp[count+i];
			for (int i=0; i<4; i++) fRec64_perm[i]=fRec64_tmp[count+i];
			fYec15_idx_save = count;
			for (int i=0; i<4; i++) fRec18_perm[i]=fRec18_tmp[count+i];
			for (int i=0; i<4; i++) fRec20_perm[i]=fRec20_tmp[count+i];
			fYec14_idx_save = count;
			for (int i=0; i<4; i++) fRec22_perm[i]=fRec22_tmp[count+i];
			for (int i=0; i<4; i++) fRec61_perm[i]=fRec61_tmp[count+i];
			fYec13_idx_save = count;
			for (int i=0; i<4; i++) fRec21_perm[i]=fRec21_tmp[count+i];
			for (int i=0; i<4; i++) fRec23_perm[i]=fRec23_tmp[count+i];
			fYec12_idx_save = count;
			for (int i=0; i<4; i++) fRec25_perm[i]=fRec25_tmp[count+i];
			for (int i=0; i<4; i++) fRec58_perm[i]=fRec58_tmp[count+i];
			fYec11_idx_save = count;
			for (int i=0; i<4; i++) fRec24_perm[i]=fRec24_tmp[count+i];
			for (int i=0; i<4; i++) fRec26_perm[i]=fRec26_tmp[count+i];
			fYec10_idx_save = count;
			for (int i=0; i<4; i++) fRec28_perm[i]=fRec28_tmp[count+i];
			for (int i=0; i<4; i++) fRec55_perm[i]=fRec55_tmp[count+i];
			fYec9_idx_save = count;
			for (int i=0; i<4; i++) fRec27_perm[i]=fRec27_tmp[count+i];
			for (int i=0; i<4; i++) fRec29_perm[i]=fRec29_tmp[count+i];
			fYec8_idx_save = count;
			for (int i=0; i<4; i++) fRec31_perm[i]=fRec31_tmp[count+i];
			for (int i=0; i<4; i++) fRec52_perm[i]=fRec52_tmp[count+i];
			fYec7_idx_save = count;
			for (int i=0; i<4; i++) fRec30_perm[i]=fRec30_tmp[count+i];
			for (int i=0; i<4; i++) fRec32_perm[i]=fRec32_tmp[count+i];
			fYec6_idx_save = count;
			for (int i=0; i<4; i++) fRec34_perm[i]=fRec34_tmp[count+i];
			for (int i=0; i<4; i++) fRec49_perm[i]=fRec49_tmp[count+i];
			fYec5_idx_save = count;
			for (int i=0; i<4; i++) fRec33_perm[i]=fRec33_tmp[count+i];
			for (int i=0; i<4; i++) fRec35_perm[i]=fRec35_tmp[count+i];
			fYec4_idx_save = count;
			for (int i=0; i<4; i++) fRec37_perm[i]=fRec37_tmp[count+i];
			for (int i=0; i<4; i++) fRec46_perm[i]=fRec46_tmp[count+i];
			fYec3_idx_save = count;
			for (int i=0; i<4; i++) fRec36_perm[i]=fRec36_tmp[count+i];
			for (int i=0; i<4; i++) fRec38_perm[i]=fRec38_tmp[count+i];
			fYec2_idx_save = count;
			fYec1_idx_save = count;
			fYec0_idx_save = count;
			
			// SECTION : 11
			// LOOP 0x7fedbd1aa1e0
			// exec code
			for (int i=0; i<count; i++) {
				output0[i] = (FAUSTFLOAT)fRec0[(fRec0_idx+i)&1023];
			}
			
			// LOOP 0x7fedbd48dbc0
			// exec code
			for (int i=0; i<count; i++) {
				output1[i] = (FAUSTFLOAT)fRec1[(fRec1_idx+i)&1023];
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
    g_unitName = strdup(name.c_str());

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
    int numControls   = unit->mNumControls;
    int curControl    = unit->mDSP->getNumInputs();
    for (int i=0; i < numControls; ++i) {
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
    #if !defined(NDEBUG)
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

#if !defined(NDEBUG) & defined(SC_API_EXPORT)
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

#if !defined(NDEBUG)
    Print("Faust: %s numControls=%d\n", name.c_str(), g_numControls);
#endif // NDEBUG
}

// EOF
