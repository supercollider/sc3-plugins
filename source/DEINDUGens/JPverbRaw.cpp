//----------------------------------------------------------
// version: "1.1"
// author: "Julian Parker, bug fixes by Till Bovermann"
// license: "GPL2+"
// copyright: "(c) Julian Parker 2013"
// name: "JPverbRaw"
//
// Code generated with Faust 0.9.98 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with : "primes" */
#include <jprev.h>
#include <math.h>
#ifndef FAUSTPOWER
#define FAUSTPOWER
#include <cmath>
template <int N> inline int faustpower(int x)              { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline int faustpower<0>(int x)            { return 1; }
template <> 	 inline int faustpower<1>(int x)            { return x; }
template <> 	 inline int faustpower<2>(int x)            { return x*x; }
template <int N> inline float faustpower(float x)            { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline float faustpower<0>(float x)          { return 1; }
template <> 	 inline float faustpower<1>(float x)          { return x; }
template <> 	 inline float faustpower<2>(float x)          { return x*x; }
#endif
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
	float 	fRec15_perm[4];
	int 	iVec0_perm[4];
	float 	fRec16_perm[4];
	FAUSTFLOAT 	fslider1;
	float 	fRec53_perm[4];
	float 	fYec0[16384];
	int 	fYec0_idx;
	int 	fYec0_idx_save;
	float 	fRec52_perm[4];
	float 	fRec50_perm[4];
	float 	fRec55_perm[4];
	float 	fYec1[16384];
	int 	fYec1_idx;
	int 	fYec1_idx_save;
	float 	fRec54_perm[4];
	float 	fRec51_perm[4];
	float 	fRec56_perm[4];
	float 	fYec2[16384];
	int 	fYec2_idx;
	int 	fYec2_idx_save;
	float 	fRec49_perm[4];
	float 	fRec47_perm[4];
	float 	fRec58_perm[4];
	float 	fYec3[16384];
	int 	fYec3_idx;
	int 	fYec3_idx_save;
	float 	fRec57_perm[4];
	float 	fRec48_perm[4];
	float 	fRec59_perm[4];
	float 	fYec4[16384];
	int 	fYec4_idx;
	int 	fYec4_idx_save;
	float 	fRec46_perm[4];
	float 	fRec44_perm[4];
	float 	fRec61_perm[4];
	float 	fYec5[16384];
	int 	fYec5_idx;
	int 	fYec5_idx_save;
	float 	fRec60_perm[4];
	float 	fRec45_perm[4];
	float 	fRec62_perm[4];
	float 	fYec6[16384];
	int 	fYec6_idx;
	int 	fYec6_idx_save;
	float 	fRec43_perm[4];
	float 	fRec41_perm[4];
	float 	fRec64_perm[4];
	float 	fYec7[16384];
	int 	fYec7_idx;
	int 	fYec7_idx_save;
	float 	fRec63_perm[4];
	float 	fRec42_perm[4];
	float 	fRec65_perm[4];
	float 	fYec8[16384];
	int 	fYec8_idx;
	int 	fYec8_idx_save;
	float 	fRec40_perm[4];
	float 	fRec38_perm[4];
	float 	fRec67_perm[4];
	float 	fYec9[16384];
	int 	fYec9_idx;
	int 	fYec9_idx_save;
	float 	fRec66_perm[4];
	float 	fRec39_perm[4];
	float 	fRec68_perm[4];
	float 	fYec10[1024];
	int 	fYec10_idx;
	int 	fYec10_idx_save;
	FAUSTFLOAT 	fslider2;
	float 	fYec11[16384];
	int 	fYec11_idx;
	int 	fYec11_idx_save;
	float 	fRec37_perm[4];
	float 	fRec70_perm[4];
	float 	fYec12[1024];
	int 	fYec12_idx;
	int 	fYec12_idx_save;
	float 	fYec13[16384];
	int 	fYec13_idx;
	int 	fYec13_idx_save;
	float 	fRec69_perm[4];
	float 	fRec71_perm[4];
	float 	fYec14[16384];
	int 	fYec14_idx;
	int 	fYec14_idx_save;
	float 	fRec36_perm[4];
	float 	fRec34_perm[4];
	float 	fRec73_perm[4];
	float 	fYec15[16384];
	int 	fYec15_idx;
	int 	fYec15_idx_save;
	float 	fRec72_perm[4];
	float 	fRec35_perm[4];
	float 	fRec74_perm[4];
	float 	fYec16[16384];
	int 	fYec16_idx;
	int 	fYec16_idx_save;
	float 	fRec33_perm[4];
	float 	fRec31_perm[4];
	float 	fRec76_perm[4];
	float 	fYec17[16384];
	int 	fYec17_idx;
	int 	fYec17_idx_save;
	float 	fRec75_perm[4];
	float 	fRec32_perm[4];
	float 	fRec77_perm[4];
	float 	fYec18[16384];
	int 	fYec18_idx;
	int 	fYec18_idx_save;
	float 	fRec30_perm[4];
	float 	fRec28_perm[4];
	float 	fRec79_perm[4];
	float 	fYec19[16384];
	int 	fYec19_idx;
	int 	fYec19_idx_save;
	float 	fRec78_perm[4];
	float 	fRec29_perm[4];
	float 	fRec80_perm[4];
	float 	fYec20[16384];
	int 	fYec20_idx;
	int 	fYec20_idx_save;
	float 	fRec27_perm[4];
	float 	fRec25_perm[4];
	float 	fRec82_perm[4];
	float 	fYec21[16384];
	int 	fYec21_idx;
	int 	fYec21_idx_save;
	float 	fRec81_perm[4];
	float 	fRec26_perm[4];
	float 	fRec83_perm[4];
	float 	fYec22[16384];
	int 	fYec22_idx;
	int 	fYec22_idx_save;
	float 	fRec24_perm[4];
	float 	fRec22_perm[4];
	float 	fRec85_perm[4];
	float 	fYec23[16384];
	int 	fYec23_idx;
	int 	fYec23_idx_save;
	float 	fRec84_perm[4];
	float 	fRec23_perm[4];
	float 	fRec86_perm[4];
	float 	fYec24[16384];
	int 	fYec24_idx;
	int 	fYec24_idx_save;
	float 	fYec25[16384];
	int 	fYec25_idx;
	int 	fYec25_idx_save;
	float 	fRec21_perm[4];
	float 	fConst2;
	FAUSTFLOAT 	fslider3;
	float 	fRec20_perm[4];
	float 	fRec19_perm[4];
	float 	fRec18_perm[4];
	FAUSTFLOAT 	fslider4;
	float 	fRec17_perm[4];
	float 	fRec92_perm[4];
	float 	fRec91_perm[4];
	float 	fRec90_perm[4];
	float 	fYec26_perm[4];
	float 	fRec89_perm[4];
	float 	fRec88_perm[4];
	float 	fRec87_perm[4];
	float 	fRec95_perm[4];
	float 	fRec94_perm[4];
	float 	fRec93_perm[4];
	FAUSTFLOAT 	fslider5;
	FAUSTFLOAT 	fslider6;
	FAUSTFLOAT 	fslider7;
	FAUSTFLOAT 	fslider8;
	FAUSTFLOAT 	fslider9;
	float 	fYec27[1024];
	int 	fYec27_idx;
	int 	fYec27_idx_save;
	float 	fRec14_perm[4];
	float 	fRec102_perm[4];
	float 	fYec28[16384];
	int 	fYec28_idx;
	int 	fYec28_idx_save;
	float 	fYec29[16384];
	int 	fYec29_idx;
	int 	fYec29_idx_save;
	float 	fRec101_perm[4];
	float 	fRec100_perm[4];
	float 	fRec99_perm[4];
	float 	fRec98_perm[4];
	float 	fRec97_perm[4];
	float 	fRec108_perm[4];
	float 	fRec107_perm[4];
	float 	fRec106_perm[4];
	float 	fYec30_perm[4];
	float 	fRec105_perm[4];
	float 	fRec104_perm[4];
	float 	fRec103_perm[4];
	float 	fRec111_perm[4];
	float 	fRec110_perm[4];
	float 	fRec109_perm[4];
	float 	fYec31[1024];
	int 	fYec31_idx;
	int 	fYec31_idx_save;
	float 	fRec96_perm[4];
	FAUSTFLOAT 	fslider10;
	float 	fYec32[16384];
	int 	fYec32_idx;
	int 	fYec32_idx_save;
	float 	fRec13_perm[4];
	float 	fRec11_perm[4];
	float 	fRec113_perm[4];
	float 	fYec33[16384];
	int 	fYec33_idx;
	int 	fYec33_idx_save;
	float 	fRec112_perm[4];
	float 	fRec12_perm[4];
	float 	fYec34[16384];
	int 	fYec34_idx;
	int 	fYec34_idx_save;
	float 	fRec10_perm[4];
	float 	fRec8_perm[4];
	float 	fYec35[16384];
	int 	fYec35_idx;
	int 	fYec35_idx_save;
	float 	fRec114_perm[4];
	float 	fRec9_perm[4];
	float 	fYec36[16384];
	int 	fYec36_idx;
	int 	fYec36_idx_save;
	float 	fRec7_perm[4];
	float 	fRec5_perm[4];
	float 	fRec116_perm[4];
	float 	fYec37[16384];
	int 	fYec37_idx;
	int 	fYec37_idx_save;
	float 	fRec115_perm[4];
	float 	fRec6_perm[4];
	float 	fRec117_perm[4];
	float 	fYec38[16384];
	int 	fYec38_idx;
	int 	fYec38_idx_save;
	float 	fRec4_perm[4];
	float 	fRec2_perm[4];
	float 	fYec39[16384];
	int 	fYec39_idx;
	int 	fYec39_idx_save;
	float 	fRec118_perm[4];
	float 	fRec3_perm[4];
	float 	fRec0_perm[4];
	float 	fRec1_perm[4];
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
		m->declare("version", "1.1");
		m->declare("author", "Julian Parker, bug fixes by Till Bovermann");
		m->declare("license", "GPL2+");
		m->declare("copyright", "(c) Julian Parker 2013");
		m->declare("name", "JPverbRaw");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.0");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.0");
		m->declare("analyzers.lib/name", "Faust Analyzer Library");
		m->declare("analyzers.lib/version", "0.0");
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
		fConst1 = (6.2831855f / fConst0);
		fConst2 = (3.1415927f / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 2.0f;
		fslider1 = 1.0f;
		fslider2 = 0.1f;
		fslider3 = 2e+03f;
		fslider4 = 5e+02f;
		fslider5 = 0.0f;
		fslider6 = 1.0f;
		fslider7 = 1.0f;
		fslider8 = 1.0f;
		fslider9 = 1.0f;
		fslider10 = 0.707f;
	}
	virtual void instanceClear() {
		for (int i=0; i<4; i++) fRec15_perm[i]=0;
		for (int i=0; i<4; i++) iVec0_perm[i]=0;
		for (int i=0; i<4; i++) fRec16_perm[i]=0;
		for (int i=0; i<4; i++) fRec53_perm[i]=0;
		for (int i=0; i<16384; i++) fYec0[i]=0;
		fYec0_idx = 0;
		fYec0_idx_save = 0;
		for (int i=0; i<4; i++) fRec52_perm[i]=0;
		for (int i=0; i<4; i++) fRec50_perm[i]=0;
		for (int i=0; i<4; i++) fRec55_perm[i]=0;
		for (int i=0; i<16384; i++) fYec1[i]=0;
		fYec1_idx = 0;
		fYec1_idx_save = 0;
		for (int i=0; i<4; i++) fRec54_perm[i]=0;
		for (int i=0; i<4; i++) fRec51_perm[i]=0;
		for (int i=0; i<4; i++) fRec56_perm[i]=0;
		for (int i=0; i<16384; i++) fYec2[i]=0;
		fYec2_idx = 0;
		fYec2_idx_save = 0;
		for (int i=0; i<4; i++) fRec49_perm[i]=0;
		for (int i=0; i<4; i++) fRec47_perm[i]=0;
		for (int i=0; i<4; i++) fRec58_perm[i]=0;
		for (int i=0; i<16384; i++) fYec3[i]=0;
		fYec3_idx = 0;
		fYec3_idx_save = 0;
		for (int i=0; i<4; i++) fRec57_perm[i]=0;
		for (int i=0; i<4; i++) fRec48_perm[i]=0;
		for (int i=0; i<4; i++) fRec59_perm[i]=0;
		for (int i=0; i<16384; i++) fYec4[i]=0;
		fYec4_idx = 0;
		fYec4_idx_save = 0;
		for (int i=0; i<4; i++) fRec46_perm[i]=0;
		for (int i=0; i<4; i++) fRec44_perm[i]=0;
		for (int i=0; i<4; i++) fRec61_perm[i]=0;
		for (int i=0; i<16384; i++) fYec5[i]=0;
		fYec5_idx = 0;
		fYec5_idx_save = 0;
		for (int i=0; i<4; i++) fRec60_perm[i]=0;
		for (int i=0; i<4; i++) fRec45_perm[i]=0;
		for (int i=0; i<4; i++) fRec62_perm[i]=0;
		for (int i=0; i<16384; i++) fYec6[i]=0;
		fYec6_idx = 0;
		fYec6_idx_save = 0;
		for (int i=0; i<4; i++) fRec43_perm[i]=0;
		for (int i=0; i<4; i++) fRec41_perm[i]=0;
		for (int i=0; i<4; i++) fRec64_perm[i]=0;
		for (int i=0; i<16384; i++) fYec7[i]=0;
		fYec7_idx = 0;
		fYec7_idx_save = 0;
		for (int i=0; i<4; i++) fRec63_perm[i]=0;
		for (int i=0; i<4; i++) fRec42_perm[i]=0;
		for (int i=0; i<4; i++) fRec65_perm[i]=0;
		for (int i=0; i<16384; i++) fYec8[i]=0;
		fYec8_idx = 0;
		fYec8_idx_save = 0;
		for (int i=0; i<4; i++) fRec40_perm[i]=0;
		for (int i=0; i<4; i++) fRec38_perm[i]=0;
		for (int i=0; i<4; i++) fRec67_perm[i]=0;
		for (int i=0; i<16384; i++) fYec9[i]=0;
		fYec9_idx = 0;
		fYec9_idx_save = 0;
		for (int i=0; i<4; i++) fRec66_perm[i]=0;
		for (int i=0; i<4; i++) fRec39_perm[i]=0;
		for (int i=0; i<4; i++) fRec68_perm[i]=0;
		for (int i=0; i<1024; i++) fYec10[i]=0;
		fYec10_idx = 0;
		fYec10_idx_save = 0;
		for (int i=0; i<16384; i++) fYec11[i]=0;
		fYec11_idx = 0;
		fYec11_idx_save = 0;
		for (int i=0; i<4; i++) fRec37_perm[i]=0;
		for (int i=0; i<4; i++) fRec70_perm[i]=0;
		for (int i=0; i<1024; i++) fYec12[i]=0;
		fYec12_idx = 0;
		fYec12_idx_save = 0;
		for (int i=0; i<16384; i++) fYec13[i]=0;
		fYec13_idx = 0;
		fYec13_idx_save = 0;
		for (int i=0; i<4; i++) fRec69_perm[i]=0;
		for (int i=0; i<4; i++) fRec71_perm[i]=0;
		for (int i=0; i<16384; i++) fYec14[i]=0;
		fYec14_idx = 0;
		fYec14_idx_save = 0;
		for (int i=0; i<4; i++) fRec36_perm[i]=0;
		for (int i=0; i<4; i++) fRec34_perm[i]=0;
		for (int i=0; i<4; i++) fRec73_perm[i]=0;
		for (int i=0; i<16384; i++) fYec15[i]=0;
		fYec15_idx = 0;
		fYec15_idx_save = 0;
		for (int i=0; i<4; i++) fRec72_perm[i]=0;
		for (int i=0; i<4; i++) fRec35_perm[i]=0;
		for (int i=0; i<4; i++) fRec74_perm[i]=0;
		for (int i=0; i<16384; i++) fYec16[i]=0;
		fYec16_idx = 0;
		fYec16_idx_save = 0;
		for (int i=0; i<4; i++) fRec33_perm[i]=0;
		for (int i=0; i<4; i++) fRec31_perm[i]=0;
		for (int i=0; i<4; i++) fRec76_perm[i]=0;
		for (int i=0; i<16384; i++) fYec17[i]=0;
		fYec17_idx = 0;
		fYec17_idx_save = 0;
		for (int i=0; i<4; i++) fRec75_perm[i]=0;
		for (int i=0; i<4; i++) fRec32_perm[i]=0;
		for (int i=0; i<4; i++) fRec77_perm[i]=0;
		for (int i=0; i<16384; i++) fYec18[i]=0;
		fYec18_idx = 0;
		fYec18_idx_save = 0;
		for (int i=0; i<4; i++) fRec30_perm[i]=0;
		for (int i=0; i<4; i++) fRec28_perm[i]=0;
		for (int i=0; i<4; i++) fRec79_perm[i]=0;
		for (int i=0; i<16384; i++) fYec19[i]=0;
		fYec19_idx = 0;
		fYec19_idx_save = 0;
		for (int i=0; i<4; i++) fRec78_perm[i]=0;
		for (int i=0; i<4; i++) fRec29_perm[i]=0;
		for (int i=0; i<4; i++) fRec80_perm[i]=0;
		for (int i=0; i<16384; i++) fYec20[i]=0;
		fYec20_idx = 0;
		fYec20_idx_save = 0;
		for (int i=0; i<4; i++) fRec27_perm[i]=0;
		for (int i=0; i<4; i++) fRec25_perm[i]=0;
		for (int i=0; i<4; i++) fRec82_perm[i]=0;
		for (int i=0; i<16384; i++) fYec21[i]=0;
		fYec21_idx = 0;
		fYec21_idx_save = 0;
		for (int i=0; i<4; i++) fRec81_perm[i]=0;
		for (int i=0; i<4; i++) fRec26_perm[i]=0;
		for (int i=0; i<4; i++) fRec83_perm[i]=0;
		for (int i=0; i<16384; i++) fYec22[i]=0;
		fYec22_idx = 0;
		fYec22_idx_save = 0;
		for (int i=0; i<4; i++) fRec24_perm[i]=0;
		for (int i=0; i<4; i++) fRec22_perm[i]=0;
		for (int i=0; i<4; i++) fRec85_perm[i]=0;
		for (int i=0; i<16384; i++) fYec23[i]=0;
		fYec23_idx = 0;
		fYec23_idx_save = 0;
		for (int i=0; i<4; i++) fRec84_perm[i]=0;
		for (int i=0; i<4; i++) fRec23_perm[i]=0;
		for (int i=0; i<4; i++) fRec86_perm[i]=0;
		for (int i=0; i<16384; i++) fYec24[i]=0;
		fYec24_idx = 0;
		fYec24_idx_save = 0;
		for (int i=0; i<16384; i++) fYec25[i]=0;
		fYec25_idx = 0;
		fYec25_idx_save = 0;
		for (int i=0; i<4; i++) fRec21_perm[i]=0;
		for (int i=0; i<4; i++) fRec20_perm[i]=0;
		for (int i=0; i<4; i++) fRec19_perm[i]=0;
		for (int i=0; i<4; i++) fRec18_perm[i]=0;
		for (int i=0; i<4; i++) fRec17_perm[i]=0;
		for (int i=0; i<4; i++) fRec92_perm[i]=0;
		for (int i=0; i<4; i++) fRec91_perm[i]=0;
		for (int i=0; i<4; i++) fRec90_perm[i]=0;
		for (int i=0; i<4; i++) fYec26_perm[i]=0;
		for (int i=0; i<4; i++) fRec89_perm[i]=0;
		for (int i=0; i<4; i++) fRec88_perm[i]=0;
		for (int i=0; i<4; i++) fRec87_perm[i]=0;
		for (int i=0; i<4; i++) fRec95_perm[i]=0;
		for (int i=0; i<4; i++) fRec94_perm[i]=0;
		for (int i=0; i<4; i++) fRec93_perm[i]=0;
		for (int i=0; i<1024; i++) fYec27[i]=0;
		fYec27_idx = 0;
		fYec27_idx_save = 0;
		for (int i=0; i<4; i++) fRec14_perm[i]=0;
		for (int i=0; i<4; i++) fRec102_perm[i]=0;
		for (int i=0; i<16384; i++) fYec28[i]=0;
		fYec28_idx = 0;
		fYec28_idx_save = 0;
		for (int i=0; i<16384; i++) fYec29[i]=0;
		fYec29_idx = 0;
		fYec29_idx_save = 0;
		for (int i=0; i<4; i++) fRec101_perm[i]=0;
		for (int i=0; i<4; i++) fRec100_perm[i]=0;
		for (int i=0; i<4; i++) fRec99_perm[i]=0;
		for (int i=0; i<4; i++) fRec98_perm[i]=0;
		for (int i=0; i<4; i++) fRec97_perm[i]=0;
		for (int i=0; i<4; i++) fRec108_perm[i]=0;
		for (int i=0; i<4; i++) fRec107_perm[i]=0;
		for (int i=0; i<4; i++) fRec106_perm[i]=0;
		for (int i=0; i<4; i++) fYec30_perm[i]=0;
		for (int i=0; i<4; i++) fRec105_perm[i]=0;
		for (int i=0; i<4; i++) fRec104_perm[i]=0;
		for (int i=0; i<4; i++) fRec103_perm[i]=0;
		for (int i=0; i<4; i++) fRec111_perm[i]=0;
		for (int i=0; i<4; i++) fRec110_perm[i]=0;
		for (int i=0; i<4; i++) fRec109_perm[i]=0;
		for (int i=0; i<1024; i++) fYec31[i]=0;
		fYec31_idx = 0;
		fYec31_idx_save = 0;
		for (int i=0; i<4; i++) fRec96_perm[i]=0;
		for (int i=0; i<16384; i++) fYec32[i]=0;
		fYec32_idx = 0;
		fYec32_idx_save = 0;
		for (int i=0; i<4; i++) fRec13_perm[i]=0;
		for (int i=0; i<4; i++) fRec11_perm[i]=0;
		for (int i=0; i<4; i++) fRec113_perm[i]=0;
		for (int i=0; i<16384; i++) fYec33[i]=0;
		fYec33_idx = 0;
		fYec33_idx_save = 0;
		for (int i=0; i<4; i++) fRec112_perm[i]=0;
		for (int i=0; i<4; i++) fRec12_perm[i]=0;
		for (int i=0; i<16384; i++) fYec34[i]=0;
		fYec34_idx = 0;
		fYec34_idx_save = 0;
		for (int i=0; i<4; i++) fRec10_perm[i]=0;
		for (int i=0; i<4; i++) fRec8_perm[i]=0;
		for (int i=0; i<16384; i++) fYec35[i]=0;
		fYec35_idx = 0;
		fYec35_idx_save = 0;
		for (int i=0; i<4; i++) fRec114_perm[i]=0;
		for (int i=0; i<4; i++) fRec9_perm[i]=0;
		for (int i=0; i<16384; i++) fYec36[i]=0;
		fYec36_idx = 0;
		fYec36_idx_save = 0;
		for (int i=0; i<4; i++) fRec7_perm[i]=0;
		for (int i=0; i<4; i++) fRec5_perm[i]=0;
		for (int i=0; i<4; i++) fRec116_perm[i]=0;
		for (int i=0; i<16384; i++) fYec37[i]=0;
		fYec37_idx = 0;
		fYec37_idx_save = 0;
		for (int i=0; i<4; i++) fRec115_perm[i]=0;
		for (int i=0; i<4; i++) fRec6_perm[i]=0;
		for (int i=0; i<4; i++) fRec117_perm[i]=0;
		for (int i=0; i<16384; i++) fYec38[i]=0;
		fYec38_idx = 0;
		fYec38_idx_save = 0;
		for (int i=0; i<4; i++) fRec4_perm[i]=0;
		for (int i=0; i<4; i++) fRec2_perm[i]=0;
		for (int i=0; i<16384; i++) fYec39[i]=0;
		fYec39_idx = 0;
		fYec39_idx_save = 0;
		for (int i=0; i<4; i++) fRec118_perm[i]=0;
		for (int i=0; i<4; i++) fRec3_perm[i]=0;
		for (int i=0; i<4; i++) fRec0_perm[i]=0;
		for (int i=0; i<4; i++) fRec1_perm[i]=0;
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
		ui_interface->addHorizontalSlider("damp", &fslider5, 0.0f, 0.0f, 0.999f, 0.0001f);
		ui_interface->addHorizontalSlider("earlyDiff", &fslider10, 0.707f, 0.0f, 0.99f, 0.001f);
		ui_interface->addHorizontalSlider("highBand", &fslider3, 2e+03f, 1e+03f, 1e+04f, 0.1f);
		ui_interface->addHorizontalSlider("highX", &fslider7, 1.0f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("lowBand", &fslider4, 5e+02f, 1e+02f, 6e+03f, 0.1f);
		ui_interface->addHorizontalSlider("lowX", &fslider9, 1.0f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("mDepth", &fslider2, 0.1f, 0.0f, 1.0f, 0.001f);
		ui_interface->addHorizontalSlider("mFreq", &fslider0, 2.0f, 0.0f, 1e+01f, 0.01f);
		ui_interface->addHorizontalSlider("midX", &fslider8, 1.0f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("size", &fslider1, 1.0f, 0.5f, 3.0f, 0.01f);
		ui_interface->addHorizontalSlider("t60", &fslider6, 1.0f, 0.1f, 6e+01f, 0.1f);
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fRec15_tmp[8+4];
		int 	iVec0_tmp[8+4];
		int 	iZec0[8];
		float 	fRec16_tmp[8+4];
		float 	fRec53_tmp[8+4];
		float 	fZec1[8];
		float 	fZec2[8];
		float 	fZec3[8];
		int 	iZec4[8];
		float 	fZec5[8];
		float 	fZec6[8];
		float 	fZec7[8];
		float 	fRec52_tmp[8+4];
		float 	fRec50_tmp[8+4];
		float 	fRec55_tmp[8+4];
		float 	fZec8[8];
		int 	iZec9[8];
		float 	fZec10[8];
		float 	fZec11[8];
		float 	fZec12[8];
		float 	fRec54_tmp[8+4];
		float 	fRec51_tmp[8+4];
		float 	fRec56_tmp[8+4];
		float 	fZec13[8];
		float 	fZec14[8];
		float 	fZec15[8];
		float 	fZec16[8];
		float 	fZec17[8];
		int 	iZec18[8];
		float 	fZec19[8];
		float 	fZec20[8];
		float 	fZec21[8];
		float 	fRec49_tmp[8+4];
		float 	fRec47_tmp[8+4];
		float 	fRec58_tmp[8+4];
		float 	fZec22[8];
		int 	iZec23[8];
		float 	fZec24[8];
		float 	fZec25[8];
		float 	fZec26[8];
		float 	fRec57_tmp[8+4];
		float 	fRec48_tmp[8+4];
		float 	fRec59_tmp[8+4];
		float 	fZec27[8];
		float 	fZec28[8];
		float 	fZec29[8];
		float 	fZec30[8];
		float 	fZec31[8];
		int 	iZec32[8];
		float 	fZec33[8];
		float 	fZec34[8];
		float 	fZec35[8];
		float 	fRec46_tmp[8+4];
		float 	fRec44_tmp[8+4];
		float 	fRec61_tmp[8+4];
		float 	fZec36[8];
		int 	iZec37[8];
		float 	fZec38[8];
		float 	fZec39[8];
		float 	fZec40[8];
		float 	fRec60_tmp[8+4];
		float 	fRec45_tmp[8+4];
		float 	fRec62_tmp[8+4];
		float 	fZec41[8];
		float 	fZec42[8];
		float 	fZec43[8];
		float 	fZec44[8];
		float 	fZec45[8];
		int 	iZec46[8];
		float 	fZec47[8];
		float 	fZec48[8];
		float 	fZec49[8];
		float 	fRec43_tmp[8+4];
		float 	fRec41_tmp[8+4];
		float 	fRec64_tmp[8+4];
		float 	fZec50[8];
		int 	iZec51[8];
		float 	fZec52[8];
		float 	fZec53[8];
		float 	fZec54[8];
		float 	fRec63_tmp[8+4];
		float 	fRec42_tmp[8+4];
		float 	fRec65_tmp[8+4];
		float 	fZec55[8];
		float 	fZec56[8];
		float 	fZec57[8];
		float 	fZec58[8];
		float 	fZec59[8];
		int 	iZec60[8];
		float 	fZec61[8];
		float 	fZec62[8];
		float 	fZec63[8];
		float 	fRec40_tmp[8+4];
		float 	fRec38_tmp[8+4];
		float 	fRec67_tmp[8+4];
		float 	fZec64[8];
		int 	iZec65[8];
		float 	fZec66[8];
		float 	fZec67[8];
		float 	fZec68[8];
		float 	fRec66_tmp[8+4];
		float 	fRec39_tmp[8+4];
		float 	fRec68_tmp[8+4];
		float 	fZec69[8];
		float 	fZec70[8];
		int 	iZec71[8];
		int 	iZec72[8];
		float 	fZec73[8];
		float 	fZec74[8];
		float 	fZec75[8];
		float 	fZec76[8];
		float 	fZec77[8];
		float 	fZec78[8];
		float 	fZec79[8];
		float 	fZec80[8];
		float 	fZec81[8];
		float 	fZec82[8];
		int 	iZec83[8];
		float 	fZec84[8];
		float 	fZec85[8];
		float 	fZec86[8];
		int 	iZec87[8];
		float 	fZec88[8];
		float 	fZec89[8];
		float 	fZec90[8];
		int 	iZec91[8];
		float 	fZec92[8];
		float 	fZec93[8];
		int 	iZec94[8];
		float 	fZec95[8];
		int 	iZec96[8];
		float 	fZec97[8];
		float 	fZec98[8];
		float 	fZec99[8];
		float 	fRec37_tmp[8+4];
		float 	fRec70_tmp[8+4];
		float 	fZec100[8];
		float 	fZec101[8];
		int 	iZec102[8];
		float 	fZec103[8];
		float 	fZec104[8];
		float 	fZec105[8];
		float 	fZec106[8];
		float 	fZec107[8];
		float 	fZec108[8];
		float 	fZec109[8];
		int 	iZec110[8];
		float 	fZec111[8];
		float 	fZec112[8];
		float 	fZec113[8];
		float 	fRec69_tmp[8+4];
		float 	fRec71_tmp[8+4];
		float 	fZec114[8];
		float 	fZec115[8];
		float 	fZec116[8];
		int 	iZec117[8];
		float 	fZec118[8];
		float 	fZec119[8];
		float 	fZec120[8];
		float 	fRec36_tmp[8+4];
		float 	fRec34_tmp[8+4];
		float 	fRec73_tmp[8+4];
		float 	fZec121[8];
		int 	iZec122[8];
		float 	fZec123[8];
		float 	fZec124[8];
		float 	fZec125[8];
		float 	fRec72_tmp[8+4];
		float 	fRec35_tmp[8+4];
		float 	fRec74_tmp[8+4];
		float 	fZec126[8];
		float 	fZec127[8];
		float 	fZec128[8];
		float 	fZec129[8];
		float 	fZec130[8];
		int 	iZec131[8];
		float 	fZec132[8];
		float 	fZec133[8];
		float 	fZec134[8];
		float 	fRec33_tmp[8+4];
		float 	fRec31_tmp[8+4];
		float 	fRec76_tmp[8+4];
		float 	fZec135[8];
		int 	iZec136[8];
		int 	iZec137[8];
		float 	fZec138[8];
		float 	fZec139[8];
		float 	fZec140[8];
		float 	fZec141[8];
		float 	fRec75_tmp[8+4];
		float 	fRec32_tmp[8+4];
		float 	fRec77_tmp[8+4];
		float 	fZec142[8];
		float 	fZec143[8];
		float 	fZec144[8];
		float 	fZec145[8];
		float 	fZec146[8];
		int 	iZec147[8];
		float 	fZec148[8];
		float 	fZec149[8];
		float 	fZec150[8];
		float 	fRec30_tmp[8+4];
		float 	fRec28_tmp[8+4];
		float 	fRec79_tmp[8+4];
		float 	fZec151[8];
		int 	iZec152[8];
		int 	iZec153[8];
		float 	fZec154[8];
		float 	fZec155[8];
		float 	fZec156[8];
		float 	fZec157[8];
		float 	fRec78_tmp[8+4];
		float 	fRec29_tmp[8+4];
		float 	fRec80_tmp[8+4];
		float 	fZec158[8];
		float 	fZec159[8];
		float 	fZec160[8];
		float 	fZec161[8];
		float 	fZec162[8];
		int 	iZec163[8];
		float 	fZec164[8];
		float 	fZec165[8];
		float 	fZec166[8];
		int 	iZec167[8];
		float 	fZec168[8];
		float 	fRec27_tmp[8+4];
		float 	fRec25_tmp[8+4];
		float 	fRec82_tmp[8+4];
		float 	fZec169[8];
		int 	iZec170[8];
		int 	iZec171[8];
		float 	fZec172[8];
		float 	fZec173[8];
		float 	fZec174[8];
		float 	fZec175[8];
		float 	fRec81_tmp[8+4];
		float 	fRec26_tmp[8+4];
		float 	fRec83_tmp[8+4];
		float 	fZec176[8];
		float 	fZec177[8];
		float 	fZec178[8];
		float 	fZec179[8];
		float 	fZec180[8];
		int 	iZec181[8];
		float 	fZec182[8];
		float 	fZec183[8];
		float 	fZec184[8];
		float 	fRec24_tmp[8+4];
		float 	fRec22_tmp[8+4];
		float 	fRec85_tmp[8+4];
		float 	fZec185[8];
		int 	iZec186[8];
		int 	iZec187[8];
		float 	fZec188[8];
		float 	fZec189[8];
		float 	fZec190[8];
		float 	fZec191[8];
		float 	fRec84_tmp[8+4];
		float 	fRec23_tmp[8+4];
		float 	fRec86_tmp[8+4];
		float 	fZec192[8];
		float 	fZec193[8];
		int 	iZec194[8];
		int 	iZec195[8];
		float 	fZec196[8];
		float 	fZec197[8];
		float 	fZec198[8];
		float 	fZec199[8];
		float 	fZec200[8];
		float 	fZec201[8];
		float 	fZec202[8];
		float 	fZec203[8];
		float 	fZec204[8];
		float 	fZec205[8];
		int 	iZec206[8];
		float 	fZec207[8];
		float 	fZec208[8];
		float 	fZec209[8];
		int 	iZec210[8];
		float 	fZec211[8];
		float 	fZec212[8];
		float 	fZec213[8];
		int 	iZec214[8];
		float 	fZec215[8];
		float 	fZec216[8];
		int 	iZec217[8];
		float 	fZec218[8];
		int 	iZec219[8];
		float 	fZec220[8];
		float 	fZec221[8];
		float 	fZec222[8];
		float 	fRec21_tmp[8+4];
		float 	fRec20_tmp[8+4];
		float 	fRec19_tmp[8+4];
		float 	fRec18_tmp[8+4];
		float 	fZec223[8];
		float 	fRec17_tmp[8+4];
		float 	fRec92_tmp[8+4];
		float 	fRec91_tmp[8+4];
		float 	fRec90_tmp[8+4];
		float 	fZec224[8];
		float 	fYec26_tmp[8+4];
		float 	fRec89_tmp[8+4];
		float 	fRec88_tmp[8+4];
		float 	fRec87_tmp[8+4];
		float 	fRec95_tmp[8+4];
		float 	fRec94_tmp[8+4];
		float 	fRec93_tmp[8+4];
		float 	fRec14_tmp[8+4];
		float 	fRec102_tmp[8+4];
		float 	fZec225[8];
		float 	fZec226[8];
		int 	iZec227[8];
		float 	fZec228[8];
		float 	fZec229[8];
		float 	fZec230[8];
		float 	fZec231[8];
		float 	fZec232[8];
		float 	fZec233[8];
		float 	fZec234[8];
		int 	iZec235[8];
		float 	fZec236[8];
		float 	fZec237[8];
		float 	fZec238[8];
		float 	fRec101_tmp[8+4];
		float 	fRec100_tmp[8+4];
		float 	fRec99_tmp[8+4];
		float 	fRec98_tmp[8+4];
		float 	fZec239[8];
		float 	fRec97_tmp[8+4];
		float 	fRec108_tmp[8+4];
		float 	fRec107_tmp[8+4];
		float 	fRec106_tmp[8+4];
		float 	fZec240[8];
		float 	fYec30_tmp[8+4];
		float 	fRec105_tmp[8+4];
		float 	fRec104_tmp[8+4];
		float 	fRec103_tmp[8+4];
		float 	fRec111_tmp[8+4];
		float 	fRec110_tmp[8+4];
		float 	fRec109_tmp[8+4];
		float 	fRec96_tmp[8+4];
		float 	fZec241[8];
		float 	fZec242[8];
		float 	fRec13_tmp[8+4];
		float 	fRec11_tmp[8+4];
		float 	fRec113_tmp[8+4];
		float 	fZec243[8];
		int 	iZec244[8];
		float 	fZec245[8];
		float 	fZec246[8];
		float 	fZec247[8];
		float 	fRec112_tmp[8+4];
		float 	fRec12_tmp[8+4];
		float 	fZec248[8];
		float 	fZec249[8];
		float 	fZec250[8];
		float 	fZec251[8];
		float 	fRec10_tmp[8+4];
		float 	fRec8_tmp[8+4];
		float 	fRec114_tmp[8+4];
		float 	fRec9_tmp[8+4];
		float 	fZec252[8];
		float 	fZec253[8];
		float 	fZec254[8];
		float 	fZec255[8];
		float 	fRec7_tmp[8+4];
		float 	fRec5_tmp[8+4];
		float 	fRec116_tmp[8+4];
		float 	fZec256[8];
		int 	iZec257[8];
		float 	fZec258[8];
		float 	fZec259[8];
		float 	fZec260[8];
		float 	fRec115_tmp[8+4];
		float 	fRec6_tmp[8+4];
		float 	fRec117_tmp[8+4];
		float 	fZec261[8];
		float 	fZec262[8];
		float 	fZec263[8];
		float 	fZec264[8];
		float 	fZec265[8];
		int 	iZec266[8];
		float 	fZec267[8];
		float 	fZec268[8];
		float 	fZec269[8];
		float 	fRec4_tmp[8+4];
		float 	fRec2_tmp[8+4];
		float 	fRec118_tmp[8+4];
		float 	fRec3_tmp[8+4];
		float 	fRec0_tmp[8+4];
		float 	fRec1_tmp[8+4];
		float 	fSlow0 = (fConst1 * float(fslider0));
		float 	fSlow1 = sinf(fSlow0);
		float 	fSlow2 = cosf(fSlow0);
		float* 	fRec15 = &fRec15_tmp[4];
		float 	fSlow3 = (0 - fSlow1);
		int* 	iVec0 = &iVec0_tmp[4];
		float* 	fRec16 = &fRec16_tmp[4];
		float 	fSlow4 = float(fslider1);
		int 	iSlow5 = primes((10 * fSlow4));
		float 	fSlow6 = (0.0001f * iSlow5);
		float* 	fRec53 = &fRec53_tmp[4];
		float* 	fRec52 = &fRec52_tmp[4];
		float* 	fRec50 = &fRec50_tmp[4];
		int 	iSlow7 = primes((110 * fSlow4));
		float 	fSlow8 = (0.0001f * iSlow7);
		float* 	fRec55 = &fRec55_tmp[4];
		float* 	fRec54 = &fRec54_tmp[4];
		float* 	fRec51 = &fRec51_tmp[4];
		int 	iSlow9 = primes((40 * fSlow4));
		float 	fSlow10 = (0.0001f * iSlow9);
		float* 	fRec56 = &fRec56_tmp[4];
		float* 	fRec49 = &fRec49_tmp[4];
		float* 	fRec47 = &fRec47_tmp[4];
		int 	iSlow11 = primes((140 * fSlow4));
		float 	fSlow12 = (0.0001f * iSlow11);
		float* 	fRec58 = &fRec58_tmp[4];
		float* 	fRec57 = &fRec57_tmp[4];
		float* 	fRec48 = &fRec48_tmp[4];
		int 	iSlow13 = primes((70 * fSlow4));
		float 	fSlow14 = (0.0001f * iSlow13);
		float* 	fRec59 = &fRec59_tmp[4];
		float* 	fRec46 = &fRec46_tmp[4];
		float* 	fRec44 = &fRec44_tmp[4];
		int 	iSlow15 = primes((170 * fSlow4));
		float 	fSlow16 = (0.0001f * iSlow15);
		float* 	fRec61 = &fRec61_tmp[4];
		float* 	fRec60 = &fRec60_tmp[4];
		float* 	fRec45 = &fRec45_tmp[4];
		int 	iSlow17 = primes((100 * fSlow4));
		float 	fSlow18 = (0.0001f * iSlow17);
		float* 	fRec62 = &fRec62_tmp[4];
		float* 	fRec43 = &fRec43_tmp[4];
		float* 	fRec41 = &fRec41_tmp[4];
		int 	iSlow19 = primes((200 * fSlow4));
		float 	fSlow20 = (0.0001f * iSlow19);
		float* 	fRec64 = &fRec64_tmp[4];
		float* 	fRec63 = &fRec63_tmp[4];
		float* 	fRec42 = &fRec42_tmp[4];
		int 	iSlow21 = primes((130 * fSlow4));
		float 	fSlow22 = (0.0001f * iSlow21);
		float* 	fRec65 = &fRec65_tmp[4];
		float* 	fRec40 = &fRec40_tmp[4];
		float* 	fRec38 = &fRec38_tmp[4];
		int 	iSlow23 = primes((230 * fSlow4));
		float 	fSlow24 = (0.0001f * iSlow23);
		float* 	fRec67 = &fRec67_tmp[4];
		float* 	fRec66 = &fRec66_tmp[4];
		float* 	fRec39 = &fRec39_tmp[4];
		int 	iSlow25 = primes((54 * fSlow4));
		float 	fSlow26 = (0.005f * iSlow25);
		float* 	fRec68 = &fRec68_tmp[4];
		float 	fSlow27 = float(fslider2);
		float 	fSlow28 = (50 * fSlow27);
		float* 	fRec37 = &fRec37_tmp[4];
		int 	iSlow29 = primes((204 * fSlow4));
		float 	fSlow30 = (0.005f * iSlow29);
		float* 	fRec70 = &fRec70_tmp[4];
		float 	fSlow31 = (0 - (5e+01f * fSlow27));
		float* 	fRec69 = &fRec69_tmp[4];
		int 	iSlow32 = primes((125 * fSlow4));
		float 	fSlow33 = (0.0001f * iSlow32);
		float* 	fRec71 = &fRec71_tmp[4];
		float* 	fRec36 = &fRec36_tmp[4];
		float* 	fRec34 = &fRec34_tmp[4];
		int 	iSlow34 = primes((25 * fSlow4));
		float 	fSlow35 = (0.0001f * iSlow34);
		float* 	fRec73 = &fRec73_tmp[4];
		float* 	fRec72 = &fRec72_tmp[4];
		float* 	fRec35 = &fRec35_tmp[4];
		int 	iSlow36 = primes((155 * fSlow4));
		float 	fSlow37 = (0.0001f * iSlow36);
		float* 	fRec74 = &fRec74_tmp[4];
		float* 	fRec33 = &fRec33_tmp[4];
		float* 	fRec31 = &fRec31_tmp[4];
		int 	iSlow38 = primes((55 * fSlow4));
		float 	fSlow39 = (0.0001f * iSlow38);
		float* 	fRec76 = &fRec76_tmp[4];
		float* 	fRec75 = &fRec75_tmp[4];
		float* 	fRec32 = &fRec32_tmp[4];
		int 	iSlow40 = primes((185 * fSlow4));
		float 	fSlow41 = (0.0001f * iSlow40);
		float* 	fRec77 = &fRec77_tmp[4];
		float* 	fRec30 = &fRec30_tmp[4];
		float* 	fRec28 = &fRec28_tmp[4];
		int 	iSlow42 = primes((85 * fSlow4));
		float 	fSlow43 = (0.0001f * iSlow42);
		float* 	fRec79 = &fRec79_tmp[4];
		float* 	fRec78 = &fRec78_tmp[4];
		float* 	fRec29 = &fRec29_tmp[4];
		int 	iSlow44 = primes((215 * fSlow4));
		float 	fSlow45 = (0.0001f * iSlow44);
		float* 	fRec80 = &fRec80_tmp[4];
		float* 	fRec27 = &fRec27_tmp[4];
		float* 	fRec25 = &fRec25_tmp[4];
		int 	iSlow46 = primes((115 * fSlow4));
		float 	fSlow47 = (0.0001f * iSlow46);
		float* 	fRec82 = &fRec82_tmp[4];
		float* 	fRec81 = &fRec81_tmp[4];
		float* 	fRec26 = &fRec26_tmp[4];
		int 	iSlow48 = primes((245 * fSlow4));
		float 	fSlow49 = (0.0001f * iSlow48);
		float* 	fRec83 = &fRec83_tmp[4];
		float* 	fRec24 = &fRec24_tmp[4];
		float* 	fRec22 = &fRec22_tmp[4];
		int 	iSlow50 = primes((145 * fSlow4));
		float 	fSlow51 = (0.0001f * iSlow50);
		float* 	fRec85 = &fRec85_tmp[4];
		float* 	fRec84 = &fRec84_tmp[4];
		float* 	fRec23 = &fRec23_tmp[4];
		int 	iSlow52 = primes((134 * fSlow4));
		float 	fSlow53 = (0.005f * iSlow52);
		float* 	fRec86 = &fRec86_tmp[4];
		float* 	fRec21 = &fRec21_tmp[4];
		float 	fSlow54 = tanf((fConst2 * float(fslider3)));
		float 	fSlow55 = (1.0f / fSlow54);
		float 	fSlow56 = (fSlow55 + 1);
		float 	fSlow57 = (1.0f / fSlow56);
		float 	fSlow58 = (0 - fSlow55);
		float 	fSlow59 = (0 - ((1 - fSlow55) / fSlow56));
		float* 	fRec20 = &fRec20_tmp[4];
		float 	fSlow60 = (1.0f / (((fSlow55 + 1.618034f) / fSlow54) + 1));
		float 	fSlow61 = (1.0f / faustpower<2>(fSlow54));
		float 	fSlow62 = (2 * (1 - fSlow61));
		float 	fSlow63 = (((fSlow55 + -1.618034f) / fSlow54) + 1);
		float* 	fRec19 = &fRec19_tmp[4];
		float 	fSlow64 = (2 * (0 - fSlow61));
		float 	fSlow65 = (((fSlow55 + 0.618034f) / fSlow54) + 1);
		float 	fSlow66 = (1.0f / fSlow65);
		float 	fSlow67 = (((fSlow55 + -0.618034f) / fSlow54) + 1);
		float* 	fRec18 = &fRec18_tmp[4];
		float 	fSlow68 = tanf((fConst2 * float(fslider4)));
		float 	fSlow69 = (1.0f / fSlow68);
		float 	fSlow70 = (1.0f / (((fSlow69 + 1.618034f) / fSlow68) + 1));
		float 	fSlow71 = (((fSlow69 + -1.618034f) / fSlow68) + 1);
		float 	fSlow72 = (1.0f / faustpower<2>(fSlow68));
		float 	fSlow73 = (2 * (1 - fSlow72));
		float* 	fRec17 = &fRec17_tmp[4];
		float* 	fRec92 = &fRec92_tmp[4];
		float* 	fRec91 = &fRec91_tmp[4];
		float* 	fRec90 = &fRec90_tmp[4];
		float 	fSlow74 = (fSlow69 + 1);
		float 	fSlow75 = (0 - ((1 - fSlow69) / fSlow74));
		float 	fSlow76 = (1.0f / fSlow74);
		float 	fSlow77 = (0 - fSlow69);
		float* 	fYec26 = &fYec26_tmp[4];
		float 	fSlow78 = (1.0f / (fSlow65 * fSlow68));
		float* 	fRec89 = &fRec89_tmp[4];
		float 	fSlow79 = (1.0f / (((fSlow69 + 1.618034f) / fSlow68) + 1));
		float 	fSlow80 = (((fSlow69 + -1.618034f) / fSlow68) + 1);
		float* 	fRec88 = &fRec88_tmp[4];
		float 	fSlow81 = (2 * (0 - fSlow72));
		float 	fSlow82 = (1.0f / (((fSlow69 + 0.618034f) / fSlow68) + 1));
		float 	fSlow83 = (((fSlow69 + -0.618034f) / fSlow68) + 1);
		float* 	fRec87 = &fRec87_tmp[4];
		float* 	fRec95 = &fRec95_tmp[4];
		float* 	fRec94 = &fRec94_tmp[4];
		float* 	fRec93 = &fRec93_tmp[4];
		float 	fSlow84 = float(fslider5);
		float 	fSlow85 = (1.0f - fSlow84);
		float 	fSlow86 = powf(10,(0 - (0.51f * (((1.25f * fSlow4) + -0.25f) / float(fslider6)))));
		float 	fSlow87 = float(fslider7);
		float 	fSlow88 = float(fslider8);
		float 	fSlow89 = float(fslider9);
		float* 	fRec14 = &fRec14_tmp[4];
		int 	iSlow90 = primes((34 * fSlow4));
		float 	fSlow91 = (0.005f * iSlow90);
		float* 	fRec102 = &fRec102_tmp[4];
		float* 	fRec101 = &fRec101_tmp[4];
		float* 	fRec100 = &fRec100_tmp[4];
		float* 	fRec99 = &fRec99_tmp[4];
		float* 	fRec98 = &fRec98_tmp[4];
		float* 	fRec97 = &fRec97_tmp[4];
		float* 	fRec108 = &fRec108_tmp[4];
		float* 	fRec107 = &fRec107_tmp[4];
		float* 	fRec106 = &fRec106_tmp[4];
		float* 	fYec30 = &fYec30_tmp[4];
		float* 	fRec105 = &fRec105_tmp[4];
		float* 	fRec104 = &fRec104_tmp[4];
		float* 	fRec103 = &fRec103_tmp[4];
		float* 	fRec111 = &fRec111_tmp[4];
		float* 	fRec110 = &fRec110_tmp[4];
		float* 	fRec109 = &fRec109_tmp[4];
		float* 	fRec96 = &fRec96_tmp[4];
		float 	fSlow92 = float(fslider10);
		float 	fSlow93 = cosf(fSlow92);
		float 	fSlow94 = sinf(fSlow92);
		float 	fSlow95 = (0 - fSlow94);
		float* 	fRec13 = &fRec13_tmp[4];
		float* 	fRec11 = &fRec11_tmp[4];
		int 	iSlow96 = primes((240 * fSlow4));
		float 	fSlow97 = (0.0001f * iSlow96);
		float* 	fRec113 = &fRec113_tmp[4];
		float* 	fRec112 = &fRec112_tmp[4];
		float* 	fRec12 = &fRec12_tmp[4];
		float* 	fRec10 = &fRec10_tmp[4];
		float* 	fRec8 = &fRec8_tmp[4];
		float* 	fRec114 = &fRec114_tmp[4];
		float* 	fRec9 = &fRec9_tmp[4];
		float* 	fRec7 = &fRec7_tmp[4];
		float* 	fRec5 = &fRec5_tmp[4];
		int 	iSlow98 = primes((190 * fSlow4));
		float 	fSlow99 = (0.0001f * iSlow98);
		float* 	fRec116 = &fRec116_tmp[4];
		float* 	fRec115 = &fRec115_tmp[4];
		float* 	fRec6 = &fRec6_tmp[4];
		int 	iSlow100 = primes((175 * fSlow4));
		float 	fSlow101 = (0.0001f * iSlow100);
		float* 	fRec117 = &fRec117_tmp[4];
		float* 	fRec4 = &fRec4_tmp[4];
		float* 	fRec2 = &fRec2_tmp[4];
		float* 	fRec118 = &fRec118_tmp[4];
		float* 	fRec3 = &fRec3_tmp[4];
		float* 	fRec0 = &fRec0_tmp[4];
		float* 	fRec1 = &fRec1_tmp[4];
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
			// LOOP 0x7fc57170af00
			// pre processing
			for (int i=0; i<4; i++) iVec0_tmp[i]=iVec0_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				iVec0[i] = 1;
			}
			// post processing
			for (int i=0; i<4; i++) iVec0_perm[i]=iVec0_tmp[count+i];
			
			// SECTION : 2
			// LOOP 0x7fc57170ae20
			// exec code
			for (int i=0; i<count; i++) {
				iZec0[i] = (1 - iVec0[i-1]);
			}
			
			// SECTION : 3
			// LOOP 0x7fc571708fb0
			// pre processing
			for (int i=0; i<4; i++) fRec15_tmp[i]=fRec15_perm[i];
			for (int i=0; i<4; i++) fRec16_tmp[i]=fRec16_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec15[i] = ((fSlow1 * fRec16[i-1]) + (fSlow2 * fRec15[i-1]));
				fRec16[i] = (((fSlow2 * fRec16[i-1]) + (fSlow3 * fRec15[i-1])) + iZec0[i]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec16_perm[i]=fRec16_tmp[count+i];
			for (int i=0; i<4; i++) fRec15_perm[i]=fRec15_tmp[count+i];
			
			// SECTION : 4
			// LOOP 0x7fc571684150
			// exec code
			for (int i=0; i<count; i++) {
				fZec69[i] = (fSlow28 * (fRec16[i] + 1));
			}
			
			// LOOP 0x7fc5716a0800
			// exec code
			for (int i=0; i<count; i++) {
				fZec192[i] = (fSlow28 * (fRec15[i] + 1));
			}
			
			// SECTION : 5
			// LOOP 0x7fc5709f3660
			// pre processing
			for (int i=0; i<4; i++) fRec82_tmp[i]=fRec82_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec82[i] = (fSlow47 + (0.9999f * ((iSlow46 * iZec0[i]) + fRec82[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec82_perm[i]=fRec82_tmp[count+i];
			
			// LOOP 0x7fc5715df5e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec100[i] = (fSlow31 * fRec16[i]);
			}
			
			// LOOP 0x7fc571684070
			// exec code
			for (int i=0; i<count; i++) {
				fZec70[i] = (fZec69[i] + 3.500005f);
			}
			
			// LOOP 0x7fc571693460
			// pre processing
			for (int i=0; i<4; i++) fRec80_tmp[i]=fRec80_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec80[i] = (fSlow45 + (0.9999f * ((iSlow44 * iZec0[i]) + fRec80[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec80_perm[i]=fRec80_tmp[count+i];
			
			// LOOP 0x7fc5716a0720
			// exec code
			for (int i=0; i<count; i++) {
				fZec193[i] = (fZec192[i] + 3.500005f);
			}
			
			// LOOP 0x7fc57180feb0
			// pre processing
			for (int i=0; i<4; i++) fRec76_tmp[i]=fRec76_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec76[i] = (fSlow39 + (0.9999f * ((iSlow38 * iZec0[i]) + fRec76[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec76_perm[i]=fRec76_tmp[count+i];
			
			// LOOP 0x7fc5718286e0
			// pre processing
			for (int i=0; i<4; i++) fRec79_tmp[i]=fRec79_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec79[i] = (fSlow43 + (0.9999f * ((iSlow42 * iZec0[i]) + fRec79[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec79_perm[i]=fRec79_tmp[count+i];
			
			// LOOP 0x7fc571844850
			// pre processing
			for (int i=0; i<4; i++) fRec85_tmp[i]=fRec85_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec85[i] = (fSlow51 + (0.9999f * ((iSlow50 * iZec0[i]) + fRec85[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec85_perm[i]=fRec85_tmp[count+i];
			
			// LOOP 0x7fc57190e9f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec225[i] = (fSlow31 * fRec15[i]);
			}
			
			// SECTION : 6
			// LOOP 0x7fc57158f810
			// pre processing
			for (int i=0; i<4; i++) fRec56_tmp[i]=fRec56_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec56[i] = (fSlow10 + (0.9999f * ((iSlow9 * iZec0[i]) + fRec56[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec56_perm[i]=fRec56_tmp[count+i];
			
			// LOOP 0x7fc571592cf0
			// pre processing
			for (int i=0; i<4; i++) fRec59_tmp[i]=fRec59_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec59[i] = (fSlow14 + (0.9999f * ((iSlow13 * iZec0[i]) + fRec59[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec59_perm[i]=fRec59_tmp[count+i];
			
			// LOOP 0x7fc5715a9b20
			// pre processing
			for (int i=0; i<4; i++) fRec58_tmp[i]=fRec58_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec58[i] = (fSlow12 + (0.9999f * ((iSlow11 * iZec0[i]) + fRec58[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec58_perm[i]=fRec58_tmp[count+i];
			
			// LOOP 0x7fc5715af780
			// pre processing
			for (int i=0; i<4; i++) fRec55_tmp[i]=fRec55_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec55[i] = (fSlow8 + (0.9999f * ((iSlow7 * iZec0[i]) + fRec55[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec55_perm[i]=fRec55_tmp[count+i];
			
			// LOOP 0x7fc5715b6770
			// pre processing
			for (int i=0; i<4; i++) fRec61_tmp[i]=fRec61_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec61[i] = (fSlow16 + (0.9999f * ((iSlow15 * iZec0[i]) + fRec61[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec61_perm[i]=fRec61_tmp[count+i];
			
			// LOOP 0x7fc5715bd380
			// pre processing
			for (int i=0; i<4; i++) fRec62_tmp[i]=fRec62_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec62[i] = (fSlow18 + (0.9999f * ((iSlow17 * iZec0[i]) + fRec62[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec62_perm[i]=fRec62_tmp[count+i];
			
			// LOOP 0x7fc5715c6920
			// pre processing
			for (int i=0; i<4; i++) fRec64_tmp[i]=fRec64_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec64[i] = (fSlow20 + (0.9999f * ((iSlow19 * iZec0[i]) + fRec64[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec64_perm[i]=fRec64_tmp[count+i];
			
			// LOOP 0x7fc5715ce270
			// pre processing
			for (int i=0; i<4; i++) fRec65_tmp[i]=fRec65_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec65[i] = (fSlow22 + (0.9999f * ((iSlow21 * iZec0[i]) + fRec65[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec65_perm[i]=fRec65_tmp[count+i];
			
			// LOOP 0x7fc5715ddc80
			// pre processing
			for (int i=0; i<4; i++) fRec70_tmp[i]=fRec70_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec70[i] = (fSlow30 + (0.995f * ((iSlow29 * iZec0[i]) + fRec70[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec70_perm[i]=fRec70_tmp[count+i];
			
			// LOOP 0x7fc5715df500
			// exec code
			for (int i=0; i<count; i++) {
				fZec101[i] = (fSlow28 + (fZec100[i] + 3.500005f));
			}
			
			// LOOP 0x7fc5715ead90
			// pre processing
			for (int i=0; i<4; i++) fRec71_tmp[i]=fRec71_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec71[i] = (fSlow33 + (0.9999f * ((iSlow32 * iZec0[i]) + fRec71[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec71_perm[i]=fRec71_tmp[count+i];
			
			// LOOP 0x7fc5715f56d0
			// pre processing
			for (int i=0; i<4; i++) fRec73_tmp[i]=fRec73_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec73[i] = (fSlow35 + (0.9999f * ((iSlow34 * iZec0[i]) + fRec73[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec73_perm[i]=fRec73_tmp[count+i];
			
			// LOOP 0x7fc571679e90
			// pre processing
			for (int i=0; i<4; i++) fRec67_tmp[i]=fRec67_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec67[i] = (fSlow24 + (0.9999f * ((iSlow23 * iZec0[i]) + fRec67[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec67_perm[i]=fRec67_tmp[count+i];
			
			// LOOP 0x7fc5716824c0
			// pre processing
			for (int i=0; i<4; i++) fRec68_tmp[i]=fRec68_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec68[i] = (fSlow26 + (0.995f * ((iSlow25 * iZec0[i]) + fRec68[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec68_perm[i]=fRec68_tmp[count+i];
			
			// LOOP 0x7fc571685ba0
			// exec code
			for (int i=0; i<count; i++) {
				fZec73[i] = floorf(fZec70[i]);
			}
			
			// LOOP 0x7fc571688430
			// exec code
			for (int i=0; i<count; i++) {
				fZec151[i] = (fRec79[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571696850
			// exec code
			for (int i=0; i<count; i++) {
				fZec162[i] = (fRec80[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5716a1b60
			// exec code
			for (int i=0; i<count; i++) {
				fZec196[i] = floorf(fZec193[i]);
			}
			
			// LOOP 0x7fc5716d1680
			// pre processing
			for (int i=0; i<4; i++) fRec116_tmp[i]=fRec116_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec116[i] = (fSlow99 + (0.9999f * ((iSlow98 * iZec0[i]) + fRec116[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec116_perm[i]=fRec116_tmp[count+i];
			
			// LOOP 0x7fc5716e7a50
			// pre processing
			for (int i=0; i<4; i++) fRec117_tmp[i]=fRec117_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec117[i] = (fSlow101 + (0.9999f * ((iSlow100 * iZec0[i]) + fRec117[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec117_perm[i]=fRec117_tmp[count+i];
			
			// LOOP 0x7fc571711590
			// pre processing
			for (int i=0; i<4; i++) fRec53_tmp[i]=fRec53_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec53[i] = (fSlow6 + (0.9999f * ((iSlow5 * iZec0[i]) + fRec53[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec53_perm[i]=fRec53_tmp[count+i];
			
			// LOOP 0x7fc571803db0
			// pre processing
			for (int i=0; i<4; i++) fRec74_tmp[i]=fRec74_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec74[i] = (fSlow37 + (0.9999f * ((iSlow36 * iZec0[i]) + fRec74[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec74_perm[i]=fRec74_tmp[count+i];
			
			// LOOP 0x7fc571811790
			// exec code
			for (int i=0; i<count; i++) {
				fZec135[i] = (fRec76[i] + -1.49999f);
			}
			
			// LOOP 0x7fc57181ba30
			// pre processing
			for (int i=0; i<4; i++) fRec77_tmp[i]=fRec77_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec77[i] = (fSlow41 + (0.9999f * ((iSlow40 * iZec0[i]) + fRec77[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec77_perm[i]=fRec77_tmp[count+i];
			
			// LOOP 0x7fc57182a590
			// exec code
			for (int i=0; i<count; i++) {
				fZec169[i] = (fRec82[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571836320
			// pre processing
			for (int i=0; i<4; i++) fRec83_tmp[i]=fRec83_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec83[i] = (fSlow49 + (0.9999f * ((iSlow48 * iZec0[i]) + fRec83[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec83_perm[i]=fRec83_tmp[count+i];
			
			// LOOP 0x7fc571846150
			// exec code
			for (int i=0; i<count; i++) {
				fZec185[i] = (fRec85[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571852c60
			// pre processing
			for (int i=0; i<4; i++) fRec86_tmp[i]=fRec86_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec86[i] = (fSlow53 + (0.995f * ((iSlow52 * iZec0[i]) + fRec86[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec86_perm[i]=fRec86_tmp[count+i];
			
			// LOOP 0x7fc5718978e0
			// pre processing
			for (int i=0; i<4; i++) fRec113_tmp[i]=fRec113_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec113[i] = (fSlow97 + (0.9999f * ((iSlow96 * iZec0[i]) + fRec113[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec113_perm[i]=fRec113_tmp[count+i];
			
			// LOOP 0x7fc57190d080
			// pre processing
			for (int i=0; i<4; i++) fRec102_tmp[i]=fRec102_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec102[i] = (fSlow91 + (0.995f * ((iSlow90 * iZec0[i]) + fRec102[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec102_perm[i]=fRec102_tmp[count+i];
			
			// LOOP 0x7fc57190e910
			// exec code
			for (int i=0; i<count; i++) {
				fZec226[i] = (fSlow28 + (fZec225[i] + 3.500005f));
			}
			
			// SECTION : 7
			// LOOP 0x7fc57158c150
			// exec code
			for (int i=0; i<count; i++) {
				fZec31[i] = (fRec59[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571592780
			// exec code
			for (int i=0; i<count; i++) {
				fZec3[i] = (fRec53[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715a5c60
			// exec code
			for (int i=0; i<count; i++) {
				fZec17[i] = (fRec56[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715ab2d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec22[i] = (fRec58[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715b0f10
			// exec code
			for (int i=0; i<count; i++) {
				fZec8[i] = (fRec55[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715b7f20
			// exec code
			for (int i=0; i<count; i++) {
				fZec36[i] = (fRec61[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715c1300
			// exec code
			for (int i=0; i<count; i++) {
				fZec45[i] = (fRec62[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715c8100
			// exec code
			for (int i=0; i<count; i++) {
				fZec50[i] = (fRec64[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715d6d80
			// exec code
			for (int i=0; i<count; i++) {
				fZec95[i] = (fRec68[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715e0820
			// exec code
			for (int i=0; i<count; i++) {
				fZec103[i] = floorf(fZec101[i]);
			}
			
			// LOOP 0x7fc5715e71a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec109[i] = (fRec70[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715ed640
			// exec code
			for (int i=0; i<count; i++) {
				fZec116[i] = (fRec71[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715f6ef0
			// exec code
			for (int i=0; i<count; i++) {
				fZec121[i] = (fRec73[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571673ca0
			// exec code
			for (int i=0; i<count; i++) {
				fZec59[i] = (fRec65[i] + -1.49999f);
			}
			
			// LOOP 0x7fc57167b670
			// exec code
			for (int i=0; i<count; i++) {
				fZec64[i] = (fRec67[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571685a30
			// exec code
			for (int i=0; i<count; i++) {
				fZec74[i] = (fZec69[i] + (4.0f - fZec73[i]));
			}
			
			// LOOP 0x7fc571686910
			// exec code
			for (int i=0; i<count; i++) {
				fZec76[i] = (fZec69[i] + (3.0f - fZec73[i]));
			}
			
			// LOOP 0x7fc5716896a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec154[i] = floorf(fZec151[i]);
			}
			
			// LOOP 0x7fc571697510
			// exec code
			for (int i=0; i<count; i++) {
				fZec164[i] = floorf(fZec162[i]);
			}
			
			// LOOP 0x7fc5716a1a50
			// exec code
			for (int i=0; i<count; i++) {
				fZec197[i] = (fZec192[i] + (4.0f - fZec196[i]));
			}
			
			// LOOP 0x7fc5716a2890
			// exec code
			for (int i=0; i<count; i++) {
				fZec199[i] = (fZec192[i] + (3.0f - fZec196[i]));
			}
			
			// LOOP 0x7fc5716ab4e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec218[i] = (fRec86[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5716d2ed0
			// exec code
			for (int i=0; i<count; i++) {
				fZec256[i] = (fRec116[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5716ead30
			// exec code
			for (int i=0; i<count; i++) {
				fZec265[i] = (fRec117[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5718071a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec130[i] = (fRec74[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5718129d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec138[i] = floorf(fZec135[i]);
			}
			
			// LOOP 0x7fc57181edd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec146[i] = (fRec77[i] + -1.49999f);
			}
			
			// LOOP 0x7fc57182b7d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec172[i] = floorf(fZec169[i]);
			}
			
			// LOOP 0x7fc5718396c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec180[i] = (fRec83[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571847390
			// exec code
			for (int i=0; i<count; i++) {
				fZec188[i] = floorf(fZec185[i]);
			}
			
			// LOOP 0x7fc5718990d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec243[i] = (fRec113[i] + -1.49999f);
			}
			
			// LOOP 0x7fc57190f980
			// exec code
			for (int i=0; i<count; i++) {
				fZec228[i] = floorf(fZec226[i]);
			}
			
			// LOOP 0x7fc5719167e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec234[i] = (fRec102[i] + -1.49999f);
			}
			
			// SECTION : 8
			// LOOP 0x7fc57158cdb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec33[i] = floorf(fZec31[i]);
			}
			
			// LOOP 0x7fc5715a68c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec19[i] = floorf(fZec17[i]);
			}
			
			// LOOP 0x7fc5715ac190
			// exec code
			for (int i=0; i<count; i++) {
				fZec24[i] = floorf(fZec22[i]);
			}
			
			// LOOP 0x7fc5715ad300
			// exec code
			for (int i=0; i<count; i++) {
				fZec5[i] = floorf(fZec3[i]);
			}
			
			// LOOP 0x7fc5715b1dd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec10[i] = floorf(fZec8[i]);
			}
			
			// LOOP 0x7fc5715b8de0
			// exec code
			for (int i=0; i<count; i++) {
				fZec38[i] = floorf(fZec36[i]);
			}
			
			// LOOP 0x7fc5715c1f60
			// exec code
			for (int i=0; i<count; i++) {
				fZec47[i] = floorf(fZec45[i]);
			}
			
			// LOOP 0x7fc5715c8fe0
			// exec code
			for (int i=0; i<count; i++) {
				fZec52[i] = floorf(fZec50[i]);
			}
			
			// LOOP 0x7fc5715d3670
			// exec code
			for (int i=0; i<count; i++) {
				fZec90[i] = (fZec74[i] * fZec76[i]);
			}
			
			// LOOP 0x7fc5715d79a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec97[i] = floorf(fZec95[i]);
			}
			
			// LOOP 0x7fc5715e0700
			// exec code
			for (int i=0; i<count; i++) {
				fZec104[i] = (fSlow28 + (fZec100[i] + (4.0f - fZec103[i])));
			}
			
			// LOOP 0x7fc5715e1450
			// exec code
			for (int i=0; i<count; i++) {
				fZec105[i] = (fSlow28 + (fZec100[i] + (3.0f - fZec103[i])));
			}
			
			// LOOP 0x7fc5715e8570
			// exec code
			for (int i=0; i<count; i++) {
				fZec111[i] = floorf(fZec109[i]);
			}
			
			// LOOP 0x7fc5715ee260
			// exec code
			for (int i=0; i<count; i++) {
				fZec118[i] = floorf(fZec116[i]);
			}
			
			// LOOP 0x7fc5715f7e30
			// exec code
			for (int i=0; i<count; i++) {
				fZec123[i] = floorf(fZec121[i]);
			}
			
			// LOOP 0x7fc571674900
			// exec code
			for (int i=0; i<count; i++) {
				fZec61[i] = floorf(fZec59[i]);
			}
			
			// LOOP 0x7fc57167c550
			// exec code
			for (int i=0; i<count; i++) {
				fZec66[i] = floorf(fZec64[i]);
			}
			
			// LOOP 0x7fc571683f90
			// exec code
			for (int i=0; i<count; i++) {
				iZec71[i] = int(fZec70[i]);
			}
			
			// LOOP 0x7fc5716875a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec78[i] = (fZec69[i] + (2.0f - fZec73[i]));
			}
			
			// LOOP 0x7fc571688350
			// exec code
			for (int i=0; i<count; i++) {
				iZec152[i] = int(min(8192, max(0, int(fZec151[i]))));
			}
			
			// LOOP 0x7fc5716895c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec155[i] = (fZec154[i] + (2.0f - fRec79[i]));
			}
			
			// LOOP 0x7fc571689ef0
			// exec code
			for (int i=0; i<count; i++) {
				fZec156[i] = (fRec79[i] - fZec154[i]);
			}
			
			// LOOP 0x7fc571696730
			// exec code
			for (int i=0; i<count; i++) {
				iZec163[i] = int(min(8192, max(0, int(fZec162[i]))));
			}
			
			// LOOP 0x7fc571697430
			// exec code
			for (int i=0; i<count; i++) {
				fZec165[i] = (fZec164[i] + (2.0f - fRec80[i]));
			}
			
			// LOOP 0x7fc571697e80
			// exec code
			for (int i=0; i<count; i++) {
				fZec166[i] = (fRec80[i] - fZec164[i]);
			}
			
			// LOOP 0x7fc5716a0640
			// exec code
			for (int i=0; i<count; i++) {
				iZec194[i] = int(fZec193[i]);
			}
			
			// LOOP 0x7fc5716a34f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec201[i] = (fZec192[i] + (2.0f - fZec196[i]));
			}
			
			// LOOP 0x7fc5716a3f60
			// exec code
			for (int i=0; i<count; i++) {
				fZec203[i] = (fZec192[i] + (1.0f - fZec196[i]));
			}
			
			// LOOP 0x7fc5716a7f50
			// exec code
			for (int i=0; i<count; i++) {
				fZec213[i] = (fZec197[i] * fZec199[i]);
			}
			
			// LOOP 0x7fc5716ac130
			// exec code
			for (int i=0; i<count; i++) {
				fZec220[i] = floorf(fZec218[i]);
			}
			
			// LOOP 0x7fc5716d3e10
			// exec code
			for (int i=0; i<count; i++) {
				fZec258[i] = floorf(fZec256[i]);
			}
			
			// LOOP 0x7fc5716eb980
			// exec code
			for (int i=0; i<count; i++) {
				fZec267[i] = floorf(fZec265[i]);
			}
			
			// LOOP 0x7fc57170f050
			// exec code
			for (int i=0; i<count; i++) {
				fZec80[i] = (fZec69[i] + (1.0f - fZec73[i]));
			}
			
			// LOOP 0x7fc571807e60
			// exec code
			for (int i=0; i<count; i++) {
				fZec132[i] = floorf(fZec130[i]);
			}
			
			// LOOP 0x7fc5718116b0
			// exec code
			for (int i=0; i<count; i++) {
				iZec136[i] = int(min(8192, max(0, int(fZec135[i]))));
			}
			
			// LOOP 0x7fc5718128f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec139[i] = (fZec138[i] + (2.0f - fRec76[i]));
			}
			
			// LOOP 0x7fc571813240
			// exec code
			for (int i=0; i<count; i++) {
				fZec140[i] = (fRec76[i] - fZec138[i]);
			}
			
			// LOOP 0x7fc57181fa70
			// exec code
			for (int i=0; i<count; i++) {
				fZec148[i] = floorf(fZec146[i]);
			}
			
			// LOOP 0x7fc57182a4b0
			// exec code
			for (int i=0; i<count; i++) {
				iZec170[i] = int(min(8192, max(0, int(fZec169[i]))));
			}
			
			// LOOP 0x7fc57182b6f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec173[i] = (fZec172[i] + (2.0f - fRec82[i]));
			}
			
			// LOOP 0x7fc57182c040
			// exec code
			for (int i=0; i<count; i++) {
				fZec174[i] = (fRec82[i] - fZec172[i]);
			}
			
			// LOOP 0x7fc57183a360
			// exec code
			for (int i=0; i<count; i++) {
				fZec182[i] = floorf(fZec180[i]);
			}
			
			// LOOP 0x7fc571846070
			// exec code
			for (int i=0; i<count; i++) {
				iZec186[i] = int(min(8192, max(0, int(fZec185[i]))));
			}
			
			// LOOP 0x7fc5718472b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec189[i] = (fZec188[i] + (2.0f - fRec85[i]));
			}
			
			// LOOP 0x7fc571847c00
			// exec code
			for (int i=0; i<count; i++) {
				fZec190[i] = (fRec85[i] - fZec188[i]);
			}
			
			// LOOP 0x7fc57189a010
			// exec code
			for (int i=0; i<count; i++) {
				fZec245[i] = floorf(fZec243[i]);
			}
			
			// LOOP 0x7fc57190f860
			// exec code
			for (int i=0; i<count; i++) {
				fZec229[i] = (fSlow28 + (fZec225[i] + (4.0f - fZec228[i])));
			}
			
			// LOOP 0x7fc571910580
			// exec code
			for (int i=0; i<count; i++) {
				fZec230[i] = (fSlow28 + (fZec225[i] + (3.0f - fZec228[i])));
			}
			
			// LOOP 0x7fc571917710
			// exec code
			for (int i=0; i<count; i++) {
				fZec236[i] = floorf(fZec234[i]);
			}
			
			// SECTION : 9
			// LOOP 0x7fc57158c030
			// exec code
			for (int i=0; i<count; i++) {
				iZec32[i] = int(min(8192, max(0, int(fZec31[i]))));
			}
			
			// LOOP 0x7fc57158ccd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec34[i] = (fZec33[i] + (2.0f - fRec59[i]));
			}
			
			// LOOP 0x7fc57158d6b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec35[i] = (fRec59[i] - fZec33[i]);
			}
			
			// LOOP 0x7fc571592650
			// exec code
			for (int i=0; i<count; i++) {
				iZec4[i] = int(min(8192, max(0, int(fZec3[i]))));
			}
			
			// LOOP 0x7fc5715a5b40
			// exec code
			for (int i=0; i<count; i++) {
				iZec18[i] = int(min(8192, max(0, int(fZec17[i]))));
			}
			
			// LOOP 0x7fc5715a67e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec20[i] = (fZec19[i] + (2.0f - fRec56[i]));
			}
			
			// LOOP 0x7fc5715a71c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec21[i] = (fRec56[i] - fZec19[i]);
			}
			
			// LOOP 0x7fc5715ab1b0
			// exec code
			for (int i=0; i<count; i++) {
				iZec23[i] = int(min(8192, max(0, int(fZec22[i]))));
			}
			
			// LOOP 0x7fc5715ac010
			// exec code
			for (int i=0; i<count; i++) {
				fZec25[i] = (fZec24[i] + (2.0f - fRec58[i]));
			}
			
			// LOOP 0x7fc5715ac9c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec26[i] = (fRec58[i] - fZec24[i]);
			}
			
			// LOOP 0x7fc5715ad1b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec6[i] = (fZec5[i] + (2.0f - fRec53[i]));
			}
			
			// LOOP 0x7fc5715adbe0
			// exec code
			for (int i=0; i<count; i++) {
				fZec7[i] = (fRec53[i] - fZec5[i]);
			}
			
			// LOOP 0x7fc5715b0df0
			// exec code
			for (int i=0; i<count; i++) {
				iZec9[i] = int(min(8192, max(0, int(fZec8[i]))));
			}
			
			// LOOP 0x7fc5715b1c50
			// exec code
			for (int i=0; i<count; i++) {
				fZec11[i] = (fZec10[i] + (2.0f - fRec55[i]));
			}
			
			// LOOP 0x7fc5715b2600
			// exec code
			for (int i=0; i<count; i++) {
				fZec12[i] = (fRec55[i] - fZec10[i]);
			}
			
			// LOOP 0x7fc5715b7e00
			// exec code
			for (int i=0; i<count; i++) {
				iZec37[i] = int(min(8192, max(0, int(fZec36[i]))));
			}
			
			// LOOP 0x7fc5715b8c60
			// exec code
			for (int i=0; i<count; i++) {
				fZec39[i] = (fZec38[i] + (2.0f - fRec61[i]));
			}
			
			// LOOP 0x7fc5715b9610
			// exec code
			for (int i=0; i<count; i++) {
				fZec40[i] = (fRec61[i] - fZec38[i]);
			}
			
			// LOOP 0x7fc5715c11e0
			// exec code
			for (int i=0; i<count; i++) {
				iZec46[i] = int(min(8192, max(0, int(fZec45[i]))));
			}
			
			// LOOP 0x7fc5715c1e80
			// exec code
			for (int i=0; i<count; i++) {
				fZec48[i] = (fZec47[i] + (2.0f - fRec62[i]));
			}
			
			// LOOP 0x7fc5715c28f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec49[i] = (fRec62[i] - fZec47[i]);
			}
			
			// LOOP 0x7fc5715c8020
			// exec code
			for (int i=0; i<count; i++) {
				iZec51[i] = int(min(8192, max(0, int(fZec50[i]))));
			}
			
			// LOOP 0x7fc5715c8e40
			// exec code
			for (int i=0; i<count; i++) {
				fZec53[i] = (fZec52[i] + (2.0f - fRec64[i]));
			}
			
			// LOOP 0x7fc5715c9840
			// exec code
			for (int i=0; i<count; i++) {
				fZec54[i] = (fRec64[i] - fZec52[i]);
			}
			
			// LOOP 0x7fc5715cfef0
			// exec code
			for (int i=0; i<count; i++) {
				fZec82[i] = (fZec69[i] + (5.0f - fZec73[i]));
			}
			
			// LOOP 0x7fc5715d0560
			// exec code
			for (int i=0; i<count; i++) {
				iZec83[i] = int(min(512, max(0, (iZec71[i] + 1))));
			}
			
			// LOOP 0x7fc5715d0cf0
			// exec code
			for (int i=0; i<count; i++) {
				fZec84[i] = (0 - fZec76[i]);
			}
			
			// LOOP 0x7fc5715d12d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec85[i] = (0 - (0.5f * fZec78[i]));
			}
			
			// LOOP 0x7fc5715d1930
			// exec code
			for (int i=0; i<count; i++) {
				fZec86[i] = (0 - (0.33333334f * fZec80[i]));
			}
			
			// LOOP 0x7fc5715d1f30
			// exec code
			for (int i=0; i<count; i++) {
				iZec87[i] = int(min(512, max(0, (iZec71[i] + 2))));
			}
			
			// LOOP 0x7fc5715d28c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec88[i] = (0 - fZec78[i]);
			}
			
			// LOOP 0x7fc5715d2e70
			// exec code
			for (int i=0; i<count; i++) {
				fZec89[i] = (0 - (0.5f * fZec80[i]));
			}
			
			// LOOP 0x7fc5715d3b10
			// exec code
			for (int i=0; i<count; i++) {
				iZec91[i] = int(min(512, max(0, (iZec71[i] + 3))));
			}
			
			// LOOP 0x7fc5715d4510
			// exec code
			for (int i=0; i<count; i++) {
				fZec92[i] = (0 - fZec80[i]);
			}
			
			// LOOP 0x7fc5715d4f30
			// exec code
			for (int i=0; i<count; i++) {
				fZec93[i] = (fZec90[i] * fZec78[i]);
			}
			
			// LOOP 0x7fc5715d51d0
			// exec code
			for (int i=0; i<count; i++) {
				iZec94[i] = int(min(512, max(0, (iZec71[i] + 4))));
			}
			
			// LOOP 0x7fc5715d6ca0
			// exec code
			for (int i=0; i<count; i++) {
				iZec96[i] = int(min(8192, max(0, int(fZec95[i]))));
			}
			
			// LOOP 0x7fc5715d78c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec98[i] = (fZec97[i] + (2.0f - fRec68[i]));
			}
			
			// LOOP 0x7fc5715d8330
			// exec code
			for (int i=0; i<count; i++) {
				fZec99[i] = (fRec68[i] - fZec97[i]);
			}
			
			// LOOP 0x7fc5715df420
			// exec code
			for (int i=0; i<count; i++) {
				iZec102[i] = int(fZec101[i]);
			}
			
			// LOOP 0x7fc5715e1c90
			// exec code
			for (int i=0; i<count; i++) {
				fZec106[i] = (fSlow28 + (fZec100[i] + (2.0f - fZec103[i])));
			}
			
			// LOOP 0x7fc5715e25d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec107[i] = (fSlow28 + (fZec100[i] + (1.0f - fZec103[i])));
			}
			
			// LOOP 0x7fc5715e4c90
			// exec code
			for (int i=0; i<count; i++) {
				fZec108[i] = (fZec104[i] * fZec105[i]);
			}
			
			// LOOP 0x7fc5715e70a0
			// exec code
			for (int i=0; i<count; i++) {
				iZec110[i] = int(min(8192, max(0, int(fZec109[i]))));
			}
			
			// LOOP 0x7fc5715e83f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec112[i] = (fZec111[i] + (2.0f - fRec70[i]));
			}
			
			// LOOP 0x7fc5715e8dc0
			// exec code
			for (int i=0; i<count; i++) {
				fZec113[i] = (fRec70[i] - fZec111[i]);
			}
			
			// LOOP 0x7fc5715ed560
			// exec code
			for (int i=0; i<count; i++) {
				iZec117[i] = int(min(8192, max(0, int(fZec116[i]))));
			}
			
			// LOOP 0x7fc5715ee180
			// exec code
			for (int i=0; i<count; i++) {
				fZec119[i] = (fZec118[i] + (2.0f - fRec71[i]));
			}
			
			// LOOP 0x7fc5715eebd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec120[i] = (fRec71[i] - fZec118[i]);
			}
			
			// LOOP 0x7fc5715f6e10
			// exec code
			for (int i=0; i<count; i++) {
				iZec122[i] = int(min(8192, max(0, int(fZec121[i]))));
			}
			
			// LOOP 0x7fc5715f7cb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec124[i] = (fZec123[i] + (2.0f - fRec73[i]));
			}
			
			// LOOP 0x7fc5715f8680
			// exec code
			for (int i=0; i<count; i++) {
				fZec125[i] = (fRec73[i] - fZec123[i]);
			}
			
			// LOOP 0x7fc571673b80
			// exec code
			for (int i=0; i<count; i++) {
				iZec60[i] = int(min(8192, max(0, int(fZec59[i]))));
			}
			
			// LOOP 0x7fc571674820
			// exec code
			for (int i=0; i<count; i++) {
				fZec62[i] = (fZec61[i] + (2.0f - fRec65[i]));
			}
			
			// LOOP 0x7fc571675200
			// exec code
			for (int i=0; i<count; i++) {
				fZec63[i] = (fRec65[i] - fZec61[i]);
			}
			
			// LOOP 0x7fc57167b590
			// exec code
			for (int i=0; i<count; i++) {
				iZec65[i] = int(min(8192, max(0, int(fZec64[i]))));
			}
			
			// LOOP 0x7fc57167c3b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec67[i] = (fZec66[i] + (2.0f - fRec67[i]));
			}
			
			// LOOP 0x7fc57167cdb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec68[i] = (fRec67[i] - fZec66[i]);
			}
			
			// LOOP 0x7fc571683e10
			// exec code
			for (int i=0; i<count; i++) {
				iZec72[i] = int(min(512, max(0, iZec71[i])));
			}
			
			// LOOP 0x7fc571685950
			// exec code
			for (int i=0; i<count; i++) {
				fZec75[i] = (0 - fZec74[i]);
			}
			
			// LOOP 0x7fc571686830
			// exec code
			for (int i=0; i<count; i++) {
				fZec77[i] = (0 - (0.5f * fZec76[i]));
			}
			
			// LOOP 0x7fc5716874c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec79[i] = (0 - (0.33333334f * fZec78[i]));
			}
			
			// LOOP 0x7fc571687ff0
			// exec code
			for (int i=0; i<count; i++) {
				fZec81[i] = (0 - (0.25f * fZec80[i]));
			}
			
			// LOOP 0x7fc571688270
			// exec code
			for (int i=0; i<count; i++) {
				iZec153[i] = int((iZec152[i] + 1));
			}
			
			// LOOP 0x7fc571689460
			// exec code
			for (int i=0; i<count; i++) {
				fZec157[i] = (0 - (fZec155[i] / fZec156[i]));
			}
			
			// LOOP 0x7fc571698460
			// exec code
			for (int i=0; i<count; i++) {
				iZec167[i] = int((iZec163[i] + 1));
			}
			
			// LOOP 0x7fc5716989e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec168[i] = (0 - (fZec165[i] / fZec166[i]));
			}
			
			// LOOP 0x7fc5716a0560
			// exec code
			for (int i=0; i<count; i++) {
				iZec195[i] = max(0, iZec194[i]);
			}
			
			// LOOP 0x7fc5716a1910
			// exec code
			for (int i=0; i<count; i++) {
				fZec198[i] = (0 - fZec197[i]);
			}
			
			// LOOP 0x7fc5716a26a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec200[i] = (0 - (0.5f * fZec199[i]));
			}
			
			// LOOP 0x7fc5716a3410
			// exec code
			for (int i=0; i<count; i++) {
				fZec202[i] = (0 - (0.33333334f * fZec201[i]));
			}
			
			// LOOP 0x7fc5716a3cc0
			// exec code
			for (int i=0; i<count; i++) {
				fZec204[i] = (0 - (0.25f * fZec203[i]));
			}
			
			// LOOP 0x7fc5716a49d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec205[i] = (fZec192[i] + (5.0f - fZec196[i]));
			}
			
			// LOOP 0x7fc5716a4ed0
			// exec code
			for (int i=0; i<count; i++) {
				iZec206[i] = max(0, (iZec194[i] + 1));
			}
			
			// LOOP 0x7fc5716a55f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec207[i] = (0 - fZec199[i]);
			}
			
			// LOOP 0x7fc5716a5a90
			// exec code
			for (int i=0; i<count; i++) {
				fZec208[i] = (0 - (0.5f * fZec201[i]));
			}
			
			// LOOP 0x7fc5716a62f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec209[i] = (0 - (0.33333334f * fZec203[i]));
			}
			
			// LOOP 0x7fc5716a6860
			// exec code
			for (int i=0; i<count; i++) {
				iZec210[i] = max(0, (iZec194[i] + 2));
			}
			
			// LOOP 0x7fc5716a70a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec211[i] = (0 - fZec201[i]);
			}
			
			// LOOP 0x7fc5716a7710
			// exec code
			for (int i=0; i<count; i++) {
				fZec212[i] = (0 - (0.5f * fZec203[i]));
			}
			
			// LOOP 0x7fc5716a7d60
			// exec code
			for (int i=0; i<count; i++) {
				fZec216[i] = (fZec213[i] * fZec201[i]);
			}
			
			// LOOP 0x7fc5716a83a0
			// exec code
			for (int i=0; i<count; i++) {
				iZec214[i] = max(0, (iZec194[i] + 3));
			}
			
			// LOOP 0x7fc5716a8be0
			// exec code
			for (int i=0; i<count; i++) {
				fZec215[i] = (0 - fZec203[i]);
			}
			
			// LOOP 0x7fc5716a9970
			// exec code
			for (int i=0; i<count; i++) {
				iZec217[i] = max(0, (iZec194[i] + 4));
			}
			
			// LOOP 0x7fc5716ab400
			// exec code
			for (int i=0; i<count; i++) {
				iZec219[i] = int(min(8192, max(0, int(fZec218[i]))));
			}
			
			// LOOP 0x7fc5716ac050
			// exec code
			for (int i=0; i<count; i++) {
				fZec221[i] = (fZec220[i] + (2.0f - fRec86[i]));
			}
			
			// LOOP 0x7fc5716acaa0
			// exec code
			for (int i=0; i<count; i++) {
				fZec222[i] = (fRec86[i] - fZec220[i]);
			}
			
			// LOOP 0x7fc5716d2df0
			// exec code
			for (int i=0; i<count; i++) {
				iZec257[i] = int(min(8192, max(0, int(fZec256[i]))));
			}
			
			// LOOP 0x7fc5716d3c90
			// exec code
			for (int i=0; i<count; i++) {
				fZec259[i] = (fZec258[i] + (2.0f - fRec116[i]));
			}
			
			// LOOP 0x7fc5716d4670
			// exec code
			for (int i=0; i<count; i++) {
				fZec260[i] = (fRec116[i] - fZec258[i]);
			}
			
			// LOOP 0x7fc5716eabd0
			// exec code
			for (int i=0; i<count; i++) {
				iZec266[i] = int(min(8192, max(0, int(fZec265[i]))));
			}
			
			// LOOP 0x7fc5716eb8a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec268[i] = (fZec267[i] + (2.0f - fRec117[i]));
			}
			
			// LOOP 0x7fc5716ec220
			// exec code
			for (int i=0; i<count; i++) {
				fZec269[i] = (fRec117[i] - fZec267[i]);
			}
			
			// LOOP 0x7fc571807080
			// exec code
			for (int i=0; i<count; i++) {
				iZec131[i] = int(min(8192, max(0, int(fZec130[i]))));
			}
			
			// LOOP 0x7fc571807d80
			// exec code
			for (int i=0; i<count; i++) {
				fZec133[i] = (fZec132[i] + (2.0f - fRec74[i]));
			}
			
			// LOOP 0x7fc5718087d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec134[i] = (fRec74[i] - fZec132[i]);
			}
			
			// LOOP 0x7fc5718115d0
			// exec code
			for (int i=0; i<count; i++) {
				iZec137[i] = int((iZec136[i] + 1));
			}
			
			// LOOP 0x7fc571812750
			// exec code
			for (int i=0; i<count; i++) {
				fZec141[i] = (0 - (fZec139[i] / fZec140[i]));
			}
			
			// LOOP 0x7fc57181ecb0
			// exec code
			for (int i=0; i<count; i++) {
				iZec147[i] = int(min(8192, max(0, int(fZec146[i]))));
			}
			
			// LOOP 0x7fc57181f990
			// exec code
			for (int i=0; i<count; i++) {
				fZec149[i] = (fZec148[i] + (2.0f - fRec77[i]));
			}
			
			// LOOP 0x7fc5718203e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec150[i] = (fRec77[i] - fZec148[i]);
			}
			
			// LOOP 0x7fc57182a3d0
			// exec code
			for (int i=0; i<count; i++) {
				iZec171[i] = int((iZec170[i] + 1));
			}
			
			// LOOP 0x7fc57182b550
			// exec code
			for (int i=0; i<count; i++) {
				fZec175[i] = (0 - (fZec173[i] / fZec174[i]));
			}
			
			// LOOP 0x7fc5718395a0
			// exec code
			for (int i=0; i<count; i++) {
				iZec181[i] = int(min(8192, max(0, int(fZec180[i]))));
			}
			
			// LOOP 0x7fc57183a280
			// exec code
			for (int i=0; i<count; i++) {
				fZec183[i] = (fZec182[i] + (2.0f - fRec83[i]));
			}
			
			// LOOP 0x7fc57183acd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec184[i] = (fRec83[i] - fZec182[i]);
			}
			
			// LOOP 0x7fc571845f90
			// exec code
			for (int i=0; i<count; i++) {
				iZec187[i] = int((iZec186[i] + 1));
			}
			
			// LOOP 0x7fc571847110
			// exec code
			for (int i=0; i<count; i++) {
				fZec191[i] = (0 - (fZec189[i] / fZec190[i]));
			}
			
			// LOOP 0x7fc571898ff0
			// exec code
			for (int i=0; i<count; i++) {
				iZec244[i] = int(min(8192, max(0, int(fZec243[i]))));
			}
			
			// LOOP 0x7fc571899e90
			// exec code
			for (int i=0; i<count; i++) {
				fZec246[i] = (fZec245[i] + (2.0f - fRec113[i]));
			}
			
			// LOOP 0x7fc57189a870
			// exec code
			for (int i=0; i<count; i++) {
				fZec247[i] = (fRec113[i] - fZec245[i]);
			}
			
			// LOOP 0x7fc57190e830
			// exec code
			for (int i=0; i<count; i++) {
				iZec227[i] = int(fZec226[i]);
			}
			
			// LOOP 0x7fc571910e20
			// exec code
			for (int i=0; i<count; i++) {
				fZec231[i] = (fSlow28 + (fZec225[i] + (2.0f - fZec228[i])));
			}
			
			// LOOP 0x7fc571911760
			// exec code
			for (int i=0; i<count; i++) {
				fZec232[i] = (fSlow28 + (fZec225[i] + (1.0f - fZec228[i])));
			}
			
			// LOOP 0x7fc571913ed0
			// exec code
			for (int i=0; i<count; i++) {
				fZec233[i] = (fZec229[i] * fZec230[i]);
			}
			
			// LOOP 0x7fc5719166c0
			// exec code
			for (int i=0; i<count; i++) {
				iZec235[i] = int(min(8192, max(0, int(fZec234[i]))));
			}
			
			// LOOP 0x7fc571917590
			// exec code
			for (int i=0; i<count; i++) {
				fZec237[i] = (fZec236[i] + (2.0f - fRec102[i]));
			}
			
			// LOOP 0x7fc571917f90
			// exec code
			for (int i=0; i<count; i++) {
				fZec238[i] = (fRec102[i] - fZec236[i]);
			}
			
			// SECTION : 10
			// LOOP 0x7fc571706ea0
			// pre processing
			fYec0_idx = (fYec0_idx+fYec0_idx_save)&16383;
			for (int i=0; i<4; i++) fRec52_tmp[i]=fRec52_perm[i];
			for (int i=0; i<4; i++) fRec50_tmp[i]=fRec50_perm[i];
			fYec1_idx = (fYec1_idx+fYec1_idx_save)&16383;
			for (int i=0; i<4; i++) fRec54_tmp[i]=fRec54_perm[i];
			for (int i=0; i<4; i++) fRec51_tmp[i]=fRec51_perm[i];
			fYec2_idx = (fYec2_idx+fYec2_idx_save)&16383;
			for (int i=0; i<4; i++) fRec49_tmp[i]=fRec49_perm[i];
			for (int i=0; i<4; i++) fRec47_tmp[i]=fRec47_perm[i];
			fYec3_idx = (fYec3_idx+fYec3_idx_save)&16383;
			for (int i=0; i<4; i++) fRec57_tmp[i]=fRec57_perm[i];
			for (int i=0; i<4; i++) fRec48_tmp[i]=fRec48_perm[i];
			fYec4_idx = (fYec4_idx+fYec4_idx_save)&16383;
			for (int i=0; i<4; i++) fRec46_tmp[i]=fRec46_perm[i];
			for (int i=0; i<4; i++) fRec44_tmp[i]=fRec44_perm[i];
			fYec5_idx = (fYec5_idx+fYec5_idx_save)&16383;
			for (int i=0; i<4; i++) fRec60_tmp[i]=fRec60_perm[i];
			for (int i=0; i<4; i++) fRec45_tmp[i]=fRec45_perm[i];
			fYec6_idx = (fYec6_idx+fYec6_idx_save)&16383;
			for (int i=0; i<4; i++) fRec43_tmp[i]=fRec43_perm[i];
			for (int i=0; i<4; i++) fRec41_tmp[i]=fRec41_perm[i];
			fYec7_idx = (fYec7_idx+fYec7_idx_save)&16383;
			for (int i=0; i<4; i++) fRec63_tmp[i]=fRec63_perm[i];
			for (int i=0; i<4; i++) fRec42_tmp[i]=fRec42_perm[i];
			fYec8_idx = (fYec8_idx+fYec8_idx_save)&16383;
			for (int i=0; i<4; i++) fRec40_tmp[i]=fRec40_perm[i];
			for (int i=0; i<4; i++) fRec38_tmp[i]=fRec38_perm[i];
			fYec9_idx = (fYec9_idx+fYec9_idx_save)&16383;
			for (int i=0; i<4; i++) fRec66_tmp[i]=fRec66_perm[i];
			for (int i=0; i<4; i++) fRec39_tmp[i]=fRec39_perm[i];
			fYec10_idx = (fYec10_idx+fYec10_idx_save)&1023;
			fYec11_idx = (fYec11_idx+fYec11_idx_save)&16383;
			for (int i=0; i<4; i++) fRec37_tmp[i]=fRec37_perm[i];
			fYec12_idx = (fYec12_idx+fYec12_idx_save)&1023;
			fYec13_idx = (fYec13_idx+fYec13_idx_save)&16383;
			for (int i=0; i<4; i++) fRec69_tmp[i]=fRec69_perm[i];
			fYec14_idx = (fYec14_idx+fYec14_idx_save)&16383;
			for (int i=0; i<4; i++) fRec36_tmp[i]=fRec36_perm[i];
			for (int i=0; i<4; i++) fRec34_tmp[i]=fRec34_perm[i];
			fYec15_idx = (fYec15_idx+fYec15_idx_save)&16383;
			for (int i=0; i<4; i++) fRec72_tmp[i]=fRec72_perm[i];
			for (int i=0; i<4; i++) fRec35_tmp[i]=fRec35_perm[i];
			fYec16_idx = (fYec16_idx+fYec16_idx_save)&16383;
			for (int i=0; i<4; i++) fRec33_tmp[i]=fRec33_perm[i];
			for (int i=0; i<4; i++) fRec31_tmp[i]=fRec31_perm[i];
			fYec17_idx = (fYec17_idx+fYec17_idx_save)&16383;
			for (int i=0; i<4; i++) fRec75_tmp[i]=fRec75_perm[i];
			for (int i=0; i<4; i++) fRec32_tmp[i]=fRec32_perm[i];
			fYec18_idx = (fYec18_idx+fYec18_idx_save)&16383;
			for (int i=0; i<4; i++) fRec30_tmp[i]=fRec30_perm[i];
			for (int i=0; i<4; i++) fRec28_tmp[i]=fRec28_perm[i];
			fYec19_idx = (fYec19_idx+fYec19_idx_save)&16383;
			for (int i=0; i<4; i++) fRec78_tmp[i]=fRec78_perm[i];
			for (int i=0; i<4; i++) fRec29_tmp[i]=fRec29_perm[i];
			fYec20_idx = (fYec20_idx+fYec20_idx_save)&16383;
			for (int i=0; i<4; i++) fRec27_tmp[i]=fRec27_perm[i];
			for (int i=0; i<4; i++) fRec25_tmp[i]=fRec25_perm[i];
			fYec21_idx = (fYec21_idx+fYec21_idx_save)&16383;
			for (int i=0; i<4; i++) fRec81_tmp[i]=fRec81_perm[i];
			for (int i=0; i<4; i++) fRec26_tmp[i]=fRec26_perm[i];
			fYec22_idx = (fYec22_idx+fYec22_idx_save)&16383;
			for (int i=0; i<4; i++) fRec24_tmp[i]=fRec24_perm[i];
			for (int i=0; i<4; i++) fRec22_tmp[i]=fRec22_perm[i];
			fYec23_idx = (fYec23_idx+fYec23_idx_save)&16383;
			for (int i=0; i<4; i++) fRec84_tmp[i]=fRec84_perm[i];
			for (int i=0; i<4; i++) fRec23_tmp[i]=fRec23_perm[i];
			fYec24_idx = (fYec24_idx+fYec24_idx_save)&16383;
			fYec25_idx = (fYec25_idx+fYec25_idx_save)&16383;
			for (int i=0; i<4; i++) fRec21_tmp[i]=fRec21_perm[i];
			for (int i=0; i<4; i++) fRec20_tmp[i]=fRec20_perm[i];
			for (int i=0; i<4; i++) fRec19_tmp[i]=fRec19_perm[i];
			for (int i=0; i<4; i++) fRec18_tmp[i]=fRec18_perm[i];
			for (int i=0; i<4; i++) fRec17_tmp[i]=fRec17_perm[i];
			for (int i=0; i<4; i++) fRec92_tmp[i]=fRec92_perm[i];
			for (int i=0; i<4; i++) fRec91_tmp[i]=fRec91_perm[i];
			for (int i=0; i<4; i++) fRec90_tmp[i]=fRec90_perm[i];
			for (int i=0; i<4; i++) fYec26_tmp[i]=fYec26_perm[i];
			for (int i=0; i<4; i++) fRec89_tmp[i]=fRec89_perm[i];
			for (int i=0; i<4; i++) fRec88_tmp[i]=fRec88_perm[i];
			for (int i=0; i<4; i++) fRec87_tmp[i]=fRec87_perm[i];
			for (int i=0; i<4; i++) fRec95_tmp[i]=fRec95_perm[i];
			for (int i=0; i<4; i++) fRec94_tmp[i]=fRec94_perm[i];
			for (int i=0; i<4; i++) fRec93_tmp[i]=fRec93_perm[i];
			fYec27_idx = (fYec27_idx+fYec27_idx_save)&1023;
			for (int i=0; i<4; i++) fRec14_tmp[i]=fRec14_perm[i];
			fYec28_idx = (fYec28_idx+fYec28_idx_save)&16383;
			fYec29_idx = (fYec29_idx+fYec29_idx_save)&16383;
			for (int i=0; i<4; i++) fRec101_tmp[i]=fRec101_perm[i];
			for (int i=0; i<4; i++) fRec100_tmp[i]=fRec100_perm[i];
			for (int i=0; i<4; i++) fRec99_tmp[i]=fRec99_perm[i];
			for (int i=0; i<4; i++) fRec98_tmp[i]=fRec98_perm[i];
			for (int i=0; i<4; i++) fRec97_tmp[i]=fRec97_perm[i];
			for (int i=0; i<4; i++) fRec108_tmp[i]=fRec108_perm[i];
			for (int i=0; i<4; i++) fRec107_tmp[i]=fRec107_perm[i];
			for (int i=0; i<4; i++) fRec106_tmp[i]=fRec106_perm[i];
			for (int i=0; i<4; i++) fYec30_tmp[i]=fYec30_perm[i];
			for (int i=0; i<4; i++) fRec105_tmp[i]=fRec105_perm[i];
			for (int i=0; i<4; i++) fRec104_tmp[i]=fRec104_perm[i];
			for (int i=0; i<4; i++) fRec103_tmp[i]=fRec103_perm[i];
			for (int i=0; i<4; i++) fRec111_tmp[i]=fRec111_perm[i];
			for (int i=0; i<4; i++) fRec110_tmp[i]=fRec110_perm[i];
			for (int i=0; i<4; i++) fRec109_tmp[i]=fRec109_perm[i];
			fYec31_idx = (fYec31_idx+fYec31_idx_save)&1023;
			for (int i=0; i<4; i++) fRec96_tmp[i]=fRec96_perm[i];
			fYec32_idx = (fYec32_idx+fYec32_idx_save)&16383;
			for (int i=0; i<4; i++) fRec13_tmp[i]=fRec13_perm[i];
			for (int i=0; i<4; i++) fRec11_tmp[i]=fRec11_perm[i];
			fYec33_idx = (fYec33_idx+fYec33_idx_save)&16383;
			for (int i=0; i<4; i++) fRec112_tmp[i]=fRec112_perm[i];
			for (int i=0; i<4; i++) fRec12_tmp[i]=fRec12_perm[i];
			fYec34_idx = (fYec34_idx+fYec34_idx_save)&16383;
			for (int i=0; i<4; i++) fRec10_tmp[i]=fRec10_perm[i];
			for (int i=0; i<4; i++) fRec8_tmp[i]=fRec8_perm[i];
			fYec35_idx = (fYec35_idx+fYec35_idx_save)&16383;
			for (int i=0; i<4; i++) fRec114_tmp[i]=fRec114_perm[i];
			for (int i=0; i<4; i++) fRec9_tmp[i]=fRec9_perm[i];
			fYec36_idx = (fYec36_idx+fYec36_idx_save)&16383;
			for (int i=0; i<4; i++) fRec7_tmp[i]=fRec7_perm[i];
			for (int i=0; i<4; i++) fRec5_tmp[i]=fRec5_perm[i];
			fYec37_idx = (fYec37_idx+fYec37_idx_save)&16383;
			for (int i=0; i<4; i++) fRec115_tmp[i]=fRec115_perm[i];
			for (int i=0; i<4; i++) fRec6_tmp[i]=fRec6_perm[i];
			fYec38_idx = (fYec38_idx+fYec38_idx_save)&16383;
			for (int i=0; i<4; i++) fRec4_tmp[i]=fRec4_perm[i];
			for (int i=0; i<4; i++) fRec2_tmp[i]=fRec2_perm[i];
			fYec39_idx = (fYec39_idx+fYec39_idx_save)&16383;
			for (int i=0; i<4; i++) fRec118_tmp[i]=fRec118_perm[i];
			for (int i=0; i<4; i++) fRec3_tmp[i]=fRec3_perm[i];
			for (int i=0; i<4; i++) fRec0_tmp[i]=fRec0_perm[i];
			for (int i=0; i<4; i++) fRec1_tmp[i]=fRec1_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec1[i] = ((0.760314f * fRec0[i-1]) - (0.64955574f * fRec50[i-1]));
				fZec2[i] = ((0.760314f * fRec1[i-1]) - (0.64955574f * fRec51[i-1]));
				fYec0[(fYec0_idx+i)&16383] = ((0.70710677f * fZec1[i]) - (0.70710677f * fZec2[i]));
				fRec52[i] = (fYec0[(fYec0_idx+i-int((iZec4[i] + 1)))&16383] + ((fRec52[i-1] * (0 - (fZec6[i] / fZec7[i]))) + ((fZec6[i] * fYec0[(fYec0_idx+i-iZec4[i])&16383]) / fZec7[i])));
				fRec50[i] = fRec52[i];
				fYec1[(fYec1_idx+i)&16383] = ((0.70710677f * fZec1[i]) + (0.70710677f * fZec2[i]));
				fRec54[i] = (fYec1[(fYec1_idx+i-int((iZec9[i] + 1)))&16383] + ((fRec54[i-1] * (0 - (fZec11[i] / fZec12[i]))) + ((fZec11[i] * fYec1[(fYec1_idx+i-iZec9[i])&16383]) / fZec12[i])));
				fRec51[i] = fRec54[i];
				fZec13[i] = ((0.64955574f * fRec0[i-1]) + (0.760314f * fRec50[i-1]));
				fZec14[i] = ((0.760314f * fZec13[i]) - (0.64955574f * fRec47[i-1]));
				fZec15[i] = ((0.760314f * fRec51[i-1]) + (0.64955574f * fRec1[i-1]));
				fZec16[i] = ((0.760314f * fZec15[i]) - (0.64955574f * fRec48[i-1]));
				fYec2[(fYec2_idx+i)&16383] = ((0.70710677f * fZec14[i]) - (0.70710677f * fZec16[i]));
				fRec49[i] = ((((fYec2[(fYec2_idx+i-iZec18[i])&16383] * fZec20[i]) / fZec21[i]) + fYec2[(fYec2_idx+i-int((iZec18[i] + 1)))&16383]) + (fRec49[i-1] * (0 - (fZec20[i] / fZec21[i]))));
				fRec47[i] = fRec49[i];
				fYec3[(fYec3_idx+i)&16383] = ((0.70710677f * fZec14[i]) + (0.70710677f * fZec16[i]));
				fRec57[i] = (fYec3[(fYec3_idx+i-int((iZec23[i] + 1)))&16383] + ((fRec57[i-1] * (0 - (fZec25[i] / fZec26[i]))) + ((fZec25[i] * fYec3[(fYec3_idx+i-iZec23[i])&16383]) / fZec26[i])));
				fRec48[i] = fRec57[i];
				fZec27[i] = ((0.760314f * fRec47[i-1]) + (0.64955574f * fZec13[i]));
				fZec28[i] = ((0.760314f * fZec27[i]) - (0.64955574f * fRec44[i-1]));
				fZec29[i] = ((0.760314f * fRec48[i-1]) + (0.64955574f * fZec15[i]));
				fZec30[i] = ((0.760314f * fZec29[i]) - (0.64955574f * fRec45[i-1]));
				fYec4[(fYec4_idx+i)&16383] = ((0.70710677f * fZec28[i]) - (0.70710677f * fZec30[i]));
				fRec46[i] = ((((fYec4[(fYec4_idx+i-iZec32[i])&16383] * fZec34[i]) / fZec35[i]) + fYec4[(fYec4_idx+i-int((iZec32[i] + 1)))&16383]) + (fRec46[i-1] * (0 - (fZec34[i] / fZec35[i]))));
				fRec44[i] = fRec46[i];
				fYec5[(fYec5_idx+i)&16383] = ((0.70710677f * fZec28[i]) + (0.70710677f * fZec30[i]));
				fRec60[i] = (fYec5[(fYec5_idx+i-int((iZec37[i] + 1)))&16383] + ((fRec60[i-1] * (0 - (fZec39[i] / fZec40[i]))) + ((fZec39[i] * fYec5[(fYec5_idx+i-iZec37[i])&16383]) / fZec40[i])));
				fRec45[i] = fRec60[i];
				fZec41[i] = ((0.760314f * fRec44[i-1]) + (0.64955574f * fZec27[i]));
				fZec42[i] = ((0.760314f * fZec41[i]) - (0.64955574f * fRec41[i-1]));
				fZec43[i] = ((0.760314f * fRec45[i-1]) + (0.64955574f * fZec29[i]));
				fZec44[i] = ((0.760314f * fZec43[i]) - (0.64955574f * fRec42[i-1]));
				fYec6[(fYec6_idx+i)&16383] = ((0.70710677f * fZec42[i]) - (0.70710677f * fZec44[i]));
				fRec43[i] = ((((fYec6[(fYec6_idx+i-iZec46[i])&16383] * fZec48[i]) / fZec49[i]) + fYec6[(fYec6_idx+i-int((iZec46[i] + 1)))&16383]) + (fRec43[i-1] * (0 - (fZec48[i] / fZec49[i]))));
				fRec41[i] = fRec43[i];
				fYec7[(fYec7_idx+i)&16383] = ((0.70710677f * fZec42[i]) + (0.70710677f * fZec44[i]));
				fRec63[i] = (fYec7[(fYec7_idx+i-int((iZec51[i] + 1)))&16383] + ((fRec63[i-1] * (0 - (fZec53[i] / fZec54[i]))) + ((fZec53[i] * fYec7[(fYec7_idx+i-iZec51[i])&16383]) / fZec54[i])));
				fRec42[i] = fRec63[i];
				fZec55[i] = ((0.760314f * fRec41[i-1]) + (0.64955574f * fZec41[i]));
				fZec56[i] = ((0.760314f * fZec55[i]) - (0.64955574f * fRec38[i-1]));
				fZec57[i] = ((0.760314f * fRec42[i-1]) + (0.64955574f * fZec43[i]));
				fZec58[i] = ((0.760314f * fZec57[i]) - (0.64955574f * fRec39[i-1]));
				fYec8[(fYec8_idx+i)&16383] = ((0.70710677f * fZec56[i]) - (0.70710677f * fZec58[i]));
				fRec40[i] = ((((fYec8[(fYec8_idx+i-iZec60[i])&16383] * fZec62[i]) / fZec63[i]) + fYec8[(fYec8_idx+i-int((iZec60[i] + 1)))&16383]) + (fRec40[i-1] * (0 - (fZec62[i] / fZec63[i]))));
				fRec38[i] = fRec40[i];
				fYec9[(fYec9_idx+i)&16383] = ((0.70710677f * fZec56[i]) + (0.70710677f * fZec58[i]));
				fRec66[i] = (fYec9[(fYec9_idx+i-int((iZec65[i] + 1)))&16383] + ((fRec66[i-1] * (0 - (fZec67[i] / fZec68[i]))) + ((fZec67[i] * fYec9[(fYec9_idx+i-iZec65[i])&16383]) / fZec68[i])));
				fRec39[i] = fRec66[i];
				fYec10[(fYec10_idx+i)&1023] = ((0.760314f * fRec38[i-1]) + (0.64955574f * fZec55[i]));
				fYec11[(fYec11_idx+i)&16383] = (((((fYec10[(fYec10_idx+i-iZec72[i])&1023] * fZec75[i]) * fZec77[i]) * fZec79[i]) * fZec81[i]) + (fZec82[i] * ((((((fYec10[(fYec10_idx+i-iZec83[i])&1023] * fZec84[i]) * fZec85[i]) * fZec86[i]) + (0.5f * (((fZec74[i] * fYec10[(fYec10_idx+i-iZec87[i])&1023]) * fZec88[i]) * fZec89[i]))) + (0.16666667f * ((fZec90[i] * fYec10[(fYec10_idx+i-iZec91[i])&1023]) * fZec92[i]))) + (0.041666668f * (fZec93[i] * fYec10[(fYec10_idx+i-iZec94[i])&1023])))));
				fRec37[i] = ((((fYec11[(fYec11_idx+i-iZec96[i])&16383] * fZec98[i]) / fZec99[i]) + fYec11[(fYec11_idx+i-int((iZec96[i] + 1)))&16383]) + (fRec37[i-1] * (0 - (fZec98[i] / fZec99[i]))));
				fYec12[(fYec12_idx+i)&1023] = ((0.760314f * fRec39[i-1]) + (0.64955574f * fZec57[i]));
				fYec13[(fYec13_idx+i)&16383] = (((((fYec12[(fYec12_idx+i-int(min(512, max(0, iZec102[i]))))&1023] * (0 - fZec104[i])) * (0 - (0.5f * fZec105[i]))) * (0 - (0.33333334f * fZec106[i]))) * (0 - (0.25f * fZec107[i]))) + ((fSlow28 + (fZec100[i] + (5.0f - fZec103[i]))) * ((((((fYec12[(fYec12_idx+i-int(min(512, max(0, (iZec102[i] + 1)))))&1023] * (0 - fZec105[i])) * (0 - (0.5f * fZec106[i]))) * (0 - (0.33333334f * fZec107[i]))) + (0.5f * (((fZec104[i] * fYec12[(fYec12_idx+i-int(min(512, max(0, (iZec102[i] + 2)))))&1023]) * (0 - fZec106[i])) * (0 - (0.5f * fZec107[i]))))) + (0.16666667f * ((fZec108[i] * fYec12[(fYec12_idx+i-int(min(512, max(0, (iZec102[i] + 3)))))&1023]) * (0 - fZec107[i])))) + (0.041666668f * ((fZec108[i] * fZec106[i]) * fYec12[(fYec12_idx+i-int(min(512, max(0, (iZec102[i] + 4)))))&1023])))));
				fRec69[i] = (fYec13[(fYec13_idx+i-int((iZec110[i] + 1)))&16383] + ((fRec69[i-1] * (0 - (fZec112[i] / fZec113[i]))) + ((fZec112[i] * fYec13[(fYec13_idx+i-iZec110[i])&16383]) / fZec113[i])));
				fZec114[i] = ((0.760314f * fRec37[i]) - (0.64955574f * fRec34[i-1]));
				fZec115[i] = ((0.760314f * fRec69[i]) - (0.64955574f * fRec35[i-1]));
				fYec14[(fYec14_idx+i)&16383] = ((0.70710677f * fZec114[i]) - (0.70710677f * fZec115[i]));
				fRec36[i] = ((((fYec14[(fYec14_idx+i-iZec117[i])&16383] * fZec119[i]) / fZec120[i]) + fYec14[(fYec14_idx+i-int((iZec117[i] + 1)))&16383]) + (fRec36[i-1] * (0 - (fZec119[i] / fZec120[i]))));
				fRec34[i] = fRec36[i];
				fYec15[(fYec15_idx+i)&16383] = ((0.70710677f * fZec114[i]) + (0.70710677f * fZec115[i]));
				fRec72[i] = (fYec15[(fYec15_idx+i-int((iZec122[i] + 1)))&16383] + ((fRec72[i-1] * (0 - (fZec124[i] / fZec125[i]))) + ((fZec124[i] * fYec15[(fYec15_idx+i-iZec122[i])&16383]) / fZec125[i])));
				fRec35[i] = fRec72[i];
				fZec126[i] = ((0.760314f * fRec34[i-1]) + (0.64955574f * fRec37[i]));
				fZec127[i] = ((0.760314f * fZec126[i]) - (0.64955574f * fRec31[i-1]));
				fZec128[i] = ((0.760314f * fRec35[i-1]) + (0.64955574f * fRec69[i]));
				fZec129[i] = ((0.760314f * fZec128[i]) - (0.64955574f * fRec32[i-1]));
				fYec16[(fYec16_idx+i)&16383] = ((0.70710677f * fZec127[i]) - (0.70710677f * fZec129[i]));
				fRec33[i] = ((((fYec16[(fYec16_idx+i-iZec131[i])&16383] * fZec133[i]) / fZec134[i]) + fYec16[(fYec16_idx+i-int((iZec131[i] + 1)))&16383]) + (fRec33[i-1] * (0 - (fZec133[i] / fZec134[i]))));
				fRec31[i] = fRec33[i];
				fYec17[(fYec17_idx+i)&16383] = ((0.70710677f * fZec127[i]) + (0.70710677f * fZec129[i]));
				fRec75[i] = (fYec17[(fYec17_idx+i-iZec137[i])&16383] + ((fRec75[i-1] * fZec141[i]) + ((fYec17[(fYec17_idx+i-iZec136[i])&16383] * fZec139[i]) / fZec140[i])));
				fRec32[i] = fRec75[i];
				fZec142[i] = ((0.760314f * fRec31[i-1]) + (0.64955574f * fZec126[i]));
				fZec143[i] = ((0.760314f * fZec142[i]) - (0.64955574f * fRec28[i-1]));
				fZec144[i] = ((0.760314f * fRec32[i-1]) + (0.64955574f * fZec128[i]));
				fZec145[i] = ((0.760314f * fZec144[i]) - (0.64955574f * fRec29[i-1]));
				fYec18[(fYec18_idx+i)&16383] = ((0.70710677f * fZec143[i]) - (0.70710677f * fZec145[i]));
				fRec30[i] = ((((fYec18[(fYec18_idx+i-iZec147[i])&16383] * fZec149[i]) / fZec150[i]) + fYec18[(fYec18_idx+i-int((iZec147[i] + 1)))&16383]) + (fRec30[i-1] * (0 - (fZec149[i] / fZec150[i]))));
				fRec28[i] = fRec30[i];
				fYec19[(fYec19_idx+i)&16383] = ((0.70710677f * fZec143[i]) + (0.70710677f * fZec145[i]));
				fRec78[i] = (fYec19[(fYec19_idx+i-iZec153[i])&16383] + ((fRec78[i-1] * fZec157[i]) + ((fZec155[i] * fYec19[(fYec19_idx+i-iZec152[i])&16383]) / fZec156[i])));
				fRec29[i] = fRec78[i];
				fZec158[i] = ((0.760314f * fRec28[i-1]) + (0.64955574f * fZec142[i]));
				fZec159[i] = ((0.760314f * fZec158[i]) - (0.64955574f * fRec25[i-1]));
				fZec160[i] = ((0.760314f * fRec29[i-1]) + (0.64955574f * fZec144[i]));
				fZec161[i] = ((0.760314f * fZec160[i]) - (0.64955574f * fRec26[i-1]));
				fYec20[(fYec20_idx+i)&16383] = ((0.70710677f * fZec159[i]) - (0.70710677f * fZec161[i]));
				fRec27[i] = ((((fYec20[(fYec20_idx+i-iZec163[i])&16383] * fZec165[i]) / fZec166[i]) + fYec20[(fYec20_idx+i-iZec167[i])&16383]) + (fZec168[i] * fRec27[i-1]));
				fRec25[i] = fRec27[i];
				fYec21[(fYec21_idx+i)&16383] = ((0.70710677f * fZec159[i]) + (0.70710677f * fZec161[i]));
				fRec81[i] = (fYec21[(fYec21_idx+i-iZec171[i])&16383] + ((fRec81[i-1] * fZec175[i]) + ((fYec21[(fYec21_idx+i-iZec170[i])&16383] * fZec173[i]) / fZec174[i])));
				fRec26[i] = fRec81[i];
				fZec176[i] = ((0.760314f * fRec25[i-1]) + (0.64955574f * fZec158[i]));
				fZec177[i] = ((0.760314f * fZec176[i]) - (0.64955574f * fRec22[i-1]));
				fZec178[i] = ((0.760314f * fRec26[i-1]) + (0.64955574f * fZec160[i]));
				fZec179[i] = ((0.760314f * fZec178[i]) - (0.64955574f * fRec23[i-1]));
				fYec22[(fYec22_idx+i)&16383] = ((0.70710677f * fZec177[i]) - (0.70710677f * fZec179[i]));
				fRec24[i] = ((((fYec22[(fYec22_idx+i-iZec181[i])&16383] * fZec183[i]) / fZec184[i]) + fYec22[(fYec22_idx+i-int((iZec181[i] + 1)))&16383]) + (fRec24[i-1] * (0 - (fZec183[i] / fZec184[i]))));
				fRec22[i] = fRec24[i];
				fYec23[(fYec23_idx+i)&16383] = ((0.70710677f * fZec177[i]) + (0.70710677f * fZec179[i]));
				fRec84[i] = (fYec23[(fYec23_idx+i-iZec187[i])&16383] + ((fRec84[i-1] * fZec191[i]) + ((fZec189[i] * fYec23[(fYec23_idx+i-iZec186[i])&16383]) / fZec190[i])));
				fRec23[i] = fRec84[i];
				fYec24[(fYec24_idx+i)&16383] = ((0.760314f * fRec22[i-1]) + (0.64955574f * fZec176[i]));
				fYec25[(fYec25_idx+i)&16383] = (((((fYec24[(fYec24_idx+i-int(min(8192, iZec195[i])))&16383] * fZec198[i]) * fZec200[i]) * fZec202[i]) * fZec204[i]) + (fZec205[i] * ((((((fYec24[(fYec24_idx+i-int(min(8192, iZec206[i])))&16383] * fZec207[i]) * fZec208[i]) * fZec209[i]) + (0.5f * (((fZec197[i] * fYec24[(fYec24_idx+i-int(min(8192, iZec210[i])))&16383]) * fZec211[i]) * fZec212[i]))) + (0.16666667f * ((fZec213[i] * fYec24[(fYec24_idx+i-int(min(8192, iZec214[i])))&16383]) * fZec215[i]))) + (0.041666668f * (fZec216[i] * fYec24[(fYec24_idx+i-int(min(8192, iZec217[i])))&16383])))));
				fRec21[i] = ((((fYec25[(fYec25_idx+i-iZec219[i])&16383] * fZec221[i]) / fZec222[i]) + fYec25[(fYec25_idx+i-int((iZec219[i] + 1)))&16383]) + (fRec21[i-1] * (0 - (fZec221[i] / fZec222[i]))));
				fRec20[i] = ((fSlow57 * ((fSlow55 * fRec21[i]) + (fSlow58 * fRec21[i-1]))) + (fSlow59 * fRec20[i-1]));
				fRec19[i] = (fRec20[i] - (fSlow60 * ((fSlow62 * fRec19[i-1]) + (fSlow63 * fRec19[i-2]))));
				fRec18[i] = ((fSlow60 * (((fSlow61 * fRec19[i]) + (fSlow64 * fRec19[i-1])) + (fSlow61 * fRec19[i-2]))) - (fSlow66 * ((fSlow62 * fRec18[i-1]) + (fSlow67 * fRec18[i-2]))));
				fZec223[i] = (fSlow73 * fRec17[i-1]);
				fRec17[i] = ((fSlow66 * (((fSlow61 * fRec18[i]) + (fSlow64 * fRec18[i-1])) + (fSlow61 * fRec18[i-2]))) - (fSlow70 * ((fSlow71 * fRec17[i-2]) + fZec223[i])));
				fRec92[i] = ((fSlow59 * fRec92[i-1]) + (fSlow57 * (fRec21[i] + fRec21[i-1])));
				fRec91[i] = (fRec92[i] - (fSlow60 * ((fSlow62 * fRec91[i-1]) + (fSlow63 * fRec91[i-2]))));
				fRec90[i] = ((fSlow60 * (fRec91[i-2] + (fRec91[i] + (2.0f * fRec91[i-1])))) - (fSlow66 * ((fSlow62 * fRec90[i-1]) + (fSlow67 * fRec90[i-2]))));
				fZec224[i] = (fRec90[i-2] + (fRec90[i] + (2.0f * fRec90[i-1])));
				fYec26[i] = (fSlow66 * fZec224[i]);
				fRec89[i] = ((fSlow75 * fRec89[i-1]) + (fSlow76 * ((fSlow77 * fYec26[i-1]) + (fSlow78 * fZec224[i]))));
				fRec88[i] = (fRec89[i] - (fSlow79 * ((fSlow73 * fRec88[i-1]) + (fSlow80 * fRec88[i-2]))));
				fRec87[i] = ((fSlow79 * (((fSlow72 * fRec88[i]) + (fSlow81 * fRec88[i-1])) + (fSlow72 * fRec88[i-2]))) - (fSlow82 * ((fSlow73 * fRec87[i-1]) + (fSlow83 * fRec87[i-2]))));
				fRec95[i] = ((fSlow75 * fRec95[i-1]) + (fSlow76 * (fYec26[i] + fYec26[i-1])));
				fRec94[i] = (fRec95[i] - (fSlow79 * ((fSlow73 * fRec94[i-1]) + (fSlow80 * fRec94[i-2]))));
				fRec93[i] = ((fSlow79 * (fRec94[i-2] + (fRec94[i] + (2.0f * fRec94[i-1])))) - (fSlow82 * ((fSlow73 * fRec93[i-1]) + (fSlow83 * fRec93[i-2]))));
				fYec27[(fYec27_idx+i)&1023] = ((float)input0[i] + (fSlow86 * ((fSlow87 * (fRec17[i-2] + (fSlow70 * (fZec223[i] + (fSlow71 * fRec17[i]))))) + (fSlow82 * ((fSlow88 * (((fSlow72 * fRec87[i]) + (fSlow81 * fRec87[i-1])) + (fSlow72 * fRec87[i-2]))) + (fSlow89 * (fRec93[i-2] + (fRec93[i] + (2.0f * fRec93[i-1])))))))));
				fRec14[i] = ((fSlow85 * (((((fZec198[i] * fZec200[i]) * fZec202[i]) * fZec204[i]) * fYec27[(fYec27_idx+i-int(min(512, iZec195[i])))&1023]) + (fZec205[i] * ((((0.5f * (((fZec197[i] * fZec211[i]) * fZec212[i]) * fYec27[(fYec27_idx+i-int(min(512, iZec210[i])))&1023])) + (((fZec207[i] * fZec208[i]) * fZec209[i]) * fYec27[(fYec27_idx+i-int(min(512, iZec206[i])))&1023])) + (0.16666667f * ((fZec213[i] * fZec215[i]) * fYec27[(fYec27_idx+i-int(min(512, iZec214[i])))&1023]))) + (0.041666668f * (fZec216[i] * fYec27[(fYec27_idx+i-int(min(512, iZec217[i])))&1023])))))) + (fSlow84 * fRec14[i-1]));
				fYec28[(fYec28_idx+i)&16383] = ((0.760314f * fRec23[i-1]) + (0.64955574f * fZec178[i]));
				fYec29[(fYec29_idx+i)&16383] = (((((fYec28[(fYec28_idx+i-int(min(8192, max(0, iZec227[i]))))&16383] * (0 - fZec229[i])) * (0 - (0.5f * fZec230[i]))) * (0 - (0.33333334f * fZec231[i]))) * (0 - (0.25f * fZec232[i]))) + ((fSlow28 + (fZec225[i] + (5.0f - fZec228[i]))) * ((((((fYec28[(fYec28_idx+i-int(min(8192, max(0, (iZec227[i] + 1)))))&16383] * (0 - fZec230[i])) * (0 - (0.5f * fZec231[i]))) * (0 - (0.33333334f * fZec232[i]))) + (0.5f * (((fZec229[i] * fYec28[(fYec28_idx+i-int(min(8192, max(0, (iZec227[i] + 2)))))&16383]) * (0 - fZec231[i])) * (0 - (0.5f * fZec232[i]))))) + (0.16666667f * ((fZec233[i] * fYec28[(fYec28_idx+i-int(min(8192, max(0, (iZec227[i] + 3)))))&16383]) * (0 - fZec232[i])))) + (0.041666668f * ((fZec233[i] * fZec231[i]) * fYec28[(fYec28_idx+i-int(min(8192, max(0, (iZec227[i] + 4)))))&16383])))));
				fRec101[i] = (fYec29[(fYec29_idx+i-int((iZec235[i] + 1)))&16383] + ((fRec101[i-1] * (0 - (fZec237[i] / fZec238[i]))) + ((fZec237[i] * fYec29[(fYec29_idx+i-iZec235[i])&16383]) / fZec238[i])));
				fRec100[i] = ((fSlow59 * fRec100[i-1]) + (fSlow57 * ((fSlow55 * fRec101[i]) + (fSlow58 * fRec101[i-1]))));
				fRec99[i] = (fRec100[i] - (fSlow60 * ((fSlow62 * fRec99[i-1]) + (fSlow63 * fRec99[i-2]))));
				fRec98[i] = ((fSlow60 * (((fSlow61 * fRec99[i]) + (fSlow64 * fRec99[i-1])) + (fSlow61 * fRec99[i-2]))) - (fSlow66 * ((fSlow62 * fRec98[i-1]) + (fSlow67 * fRec98[i-2]))));
				fZec239[i] = (fSlow73 * fRec97[i-1]);
				fRec97[i] = ((fSlow66 * (((fSlow61 * fRec98[i]) + (fSlow64 * fRec98[i-1])) + (fSlow61 * fRec98[i-2]))) - (fSlow70 * (fZec239[i] + (fSlow71 * fRec97[i-2]))));
				fRec108[i] = ((fSlow59 * fRec108[i-1]) + (fSlow57 * (fRec101[i] + fRec101[i-1])));
				fRec107[i] = (fRec108[i] - (fSlow60 * ((fSlow62 * fRec107[i-1]) + (fSlow63 * fRec107[i-2]))));
				fRec106[i] = ((fSlow60 * (fRec107[i-2] + (fRec107[i] + (2.0f * fRec107[i-1])))) - (fSlow66 * ((fSlow62 * fRec106[i-1]) + (fSlow67 * fRec106[i-2]))));
				fZec240[i] = (fRec106[i-2] + (fRec106[i] + (2.0f * fRec106[i-1])));
				fYec30[i] = (fSlow66 * fZec240[i]);
				fRec105[i] = ((fSlow75 * fRec105[i-1]) + (fSlow76 * ((fSlow78 * fZec240[i]) + (fSlow77 * fYec30[i-1]))));
				fRec104[i] = (fRec105[i] - (fSlow79 * ((fSlow73 * fRec104[i-1]) + (fSlow80 * fRec104[i-2]))));
				fRec103[i] = ((fSlow79 * (((fSlow72 * fRec104[i]) + (fSlow81 * fRec104[i-1])) + (fSlow72 * fRec104[i-2]))) - (fSlow82 * ((fSlow73 * fRec103[i-1]) + (fSlow83 * fRec103[i-2]))));
				fRec111[i] = ((fSlow75 * fRec111[i-1]) + (fSlow76 * (fYec30[i-1] + fYec30[i])));
				fRec110[i] = (fRec111[i] - (fSlow79 * ((fSlow73 * fRec110[i-1]) + (fSlow80 * fRec110[i-2]))));
				fRec109[i] = ((fSlow79 * (fRec110[i-2] + (fRec110[i] + (2.0f * fRec110[i-1])))) - (fSlow82 * ((fSlow73 * fRec109[i-1]) + (fSlow83 * fRec109[i-2]))));
				fYec31[(fYec31_idx+i)&1023] = ((float)input1[i] + (fSlow86 * ((fSlow87 * (fRec97[i-2] + (fSlow70 * (fZec239[i] + (fSlow71 * fRec97[i]))))) + (fSlow82 * ((fSlow88 * (((fSlow72 * fRec103[i]) + (fSlow81 * fRec103[i-1])) + (fSlow72 * fRec103[i-2]))) + (fSlow89 * (fRec109[i-2] + (fRec109[i] + (2.0f * fRec109[i-1])))))))));
				fRec96[i] = ((fSlow84 * fRec96[i-1]) + (fSlow85 * (((((fZec75[i] * fZec77[i]) * fZec79[i]) * fZec81[i]) * fYec31[(fYec31_idx+i-iZec72[i])&1023]) + (fZec82[i] * ((((((fZec84[i] * fZec85[i]) * fZec86[i]) * fYec31[(fYec31_idx+i-iZec83[i])&1023]) + (0.5f * (((fZec74[i] * fZec88[i]) * fZec89[i]) * fYec31[(fYec31_idx+i-iZec87[i])&1023]))) + (0.16666667f * ((fZec90[i] * fZec92[i]) * fYec31[(fYec31_idx+i-iZec91[i])&1023]))) + (0.041666668f * (fZec93[i] * fYec31[(fYec31_idx+i-iZec94[i])&1023])))))));
				fZec241[i] = ((fSlow93 * fRec14[i]) + (fSlow95 * fRec11[i-1]));
				fZec242[i] = ((fSlow95 * fRec12[i-1]) + (fSlow93 * fRec96[i]));
				fYec32[(fYec32_idx+i)&16383] = ((0.70710677f * fZec241[i]) - (0.70710677f * fZec242[i]));
				fRec13[i] = ((((fYec32[(fYec32_idx+i-iZec136[i])&16383] * fZec139[i]) / fZec140[i]) + fYec32[(fYec32_idx+i-iZec137[i])&16383]) + (fRec13[i-1] * fZec141[i]));
				fRec11[i] = fRec13[i];
				fYec33[(fYec33_idx+i)&16383] = ((0.70710677f * fZec241[i]) + (0.70710677f * fZec242[i]));
				fRec112[i] = (fYec33[(fYec33_idx+i-int((iZec244[i] + 1)))&16383] + ((fRec112[i-1] * (0 - (fZec246[i] / fZec247[i]))) + ((fZec246[i] * fYec33[(fYec33_idx+i-iZec244[i])&16383]) / fZec247[i])));
				fRec12[i] = fRec112[i];
				fZec248[i] = ((fSlow93 * fRec11[i-1]) + (fSlow94 * fRec14[i]));
				fZec249[i] = ((fSlow93 * fZec248[i]) + (fSlow95 * fRec8[i-1]));
				fZec250[i] = ((fSlow93 * fRec12[i-1]) + (fSlow94 * fRec96[i]));
				fZec251[i] = ((fSlow95 * fRec9[i-1]) + (fSlow93 * fZec250[i]));
				fYec34[(fYec34_idx+i)&16383] = ((0.70710677f * fZec249[i]) - (0.70710677f * fZec251[i]));
				fRec10[i] = ((((fYec34[(fYec34_idx+i-iZec163[i])&16383] * fZec165[i]) / fZec166[i]) + fYec34[(fYec34_idx+i-iZec167[i])&16383]) + (fRec10[i-1] * fZec168[i]));
				fRec8[i] = fRec10[i];
				fYec35[(fYec35_idx+i)&16383] = ((0.70710677f * fZec249[i]) + (0.70710677f * fZec251[i]));
				fRec114[i] = (fYec35[(fYec35_idx+i-iZec153[i])&16383] + ((fZec157[i] * fRec114[i-1]) + ((fZec155[i] * fYec35[(fYec35_idx+i-iZec152[i])&16383]) / fZec156[i])));
				fRec9[i] = fRec114[i];
				fZec252[i] = ((fSlow93 * fRec8[i-1]) + (fSlow94 * fZec248[i]));
				fZec253[i] = ((fSlow93 * fZec252[i]) + (fSlow95 * fRec5[i-1]));
				fZec254[i] = ((fSlow93 * fRec9[i-1]) + (fSlow94 * fZec250[i]));
				fZec255[i] = ((fSlow95 * fRec6[i-1]) + (fSlow93 * fZec254[i]));
				fYec36[(fYec36_idx+i)&16383] = ((0.70710677f * fZec253[i]) - (0.70710677f * fZec255[i]));
				fRec7[i] = ((((fYec36[(fYec36_idx+i-iZec170[i])&16383] * fZec173[i]) / fZec174[i]) + fYec36[(fYec36_idx+i-iZec171[i])&16383]) + (fRec7[i-1] * fZec175[i]));
				fRec5[i] = fRec7[i];
				fYec37[(fYec37_idx+i)&16383] = ((0.70710677f * fZec253[i]) + (0.70710677f * fZec255[i]));
				fRec115[i] = (fYec37[(fYec37_idx+i-int((iZec257[i] + 1)))&16383] + ((fRec115[i-1] * (0 - (fZec259[i] / fZec260[i]))) + ((fZec259[i] * fYec37[(fYec37_idx+i-iZec257[i])&16383]) / fZec260[i])));
				fRec6[i] = fRec115[i];
				fZec261[i] = ((fSlow93 * fRec5[i-1]) + (fSlow94 * fZec252[i]));
				fZec262[i] = ((fSlow93 * fZec261[i]) + (fSlow95 * fRec2[i-1]));
				fZec263[i] = ((fSlow93 * fRec6[i-1]) + (fSlow94 * fZec254[i]));
				fZec264[i] = ((fSlow95 * fRec3[i-1]) + (fSlow93 * fZec263[i]));
				fYec38[(fYec38_idx+i)&16383] = ((0.70710677f * fZec262[i]) - (0.70710677f * fZec264[i]));
				fRec4[i] = ((((fYec38[(fYec38_idx+i-iZec266[i])&16383] * fZec268[i]) / fZec269[i]) + fYec38[(fYec38_idx+i-int((iZec266[i] + 1)))&16383]) + (fRec4[i-1] * (0 - (fZec268[i] / fZec269[i]))));
				fRec2[i] = fRec4[i];
				fYec39[(fYec39_idx+i)&16383] = ((0.70710677f * fZec262[i]) + (0.70710677f * fZec264[i]));
				fRec118[i] = (fYec39[(fYec39_idx+i-iZec187[i])&16383] + ((fZec191[i] * fRec118[i-1]) + ((fZec189[i] * fYec39[(fYec39_idx+i-iZec186[i])&16383]) / fZec190[i])));
				fRec3[i] = fRec118[i];
				fRec0[i] = ((fSlow93 * fRec2[i-1]) + (fSlow94 * fZec261[i]));
				fRec1[i] = ((fSlow93 * fRec3[i-1]) + (fSlow94 * fZec263[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec1_perm[i]=fRec1_tmp[count+i];
			for (int i=0; i<4; i++) fRec0_perm[i]=fRec0_tmp[count+i];
			for (int i=0; i<4; i++) fRec3_perm[i]=fRec3_tmp[count+i];
			for (int i=0; i<4; i++) fRec118_perm[i]=fRec118_tmp[count+i];
			fYec39_idx_save = count;
			for (int i=0; i<4; i++) fRec2_perm[i]=fRec2_tmp[count+i];
			for (int i=0; i<4; i++) fRec4_perm[i]=fRec4_tmp[count+i];
			fYec38_idx_save = count;
			for (int i=0; i<4; i++) fRec6_perm[i]=fRec6_tmp[count+i];
			for (int i=0; i<4; i++) fRec115_perm[i]=fRec115_tmp[count+i];
			fYec37_idx_save = count;
			for (int i=0; i<4; i++) fRec5_perm[i]=fRec5_tmp[count+i];
			for (int i=0; i<4; i++) fRec7_perm[i]=fRec7_tmp[count+i];
			fYec36_idx_save = count;
			for (int i=0; i<4; i++) fRec9_perm[i]=fRec9_tmp[count+i];
			for (int i=0; i<4; i++) fRec114_perm[i]=fRec114_tmp[count+i];
			fYec35_idx_save = count;
			for (int i=0; i<4; i++) fRec8_perm[i]=fRec8_tmp[count+i];
			for (int i=0; i<4; i++) fRec10_perm[i]=fRec10_tmp[count+i];
			fYec34_idx_save = count;
			for (int i=0; i<4; i++) fRec12_perm[i]=fRec12_tmp[count+i];
			for (int i=0; i<4; i++) fRec112_perm[i]=fRec112_tmp[count+i];
			fYec33_idx_save = count;
			for (int i=0; i<4; i++) fRec11_perm[i]=fRec11_tmp[count+i];
			for (int i=0; i<4; i++) fRec13_perm[i]=fRec13_tmp[count+i];
			fYec32_idx_save = count;
			for (int i=0; i<4; i++) fRec96_perm[i]=fRec96_tmp[count+i];
			fYec31_idx_save = count;
			for (int i=0; i<4; i++) fRec109_perm[i]=fRec109_tmp[count+i];
			for (int i=0; i<4; i++) fRec110_perm[i]=fRec110_tmp[count+i];
			for (int i=0; i<4; i++) fRec111_perm[i]=fRec111_tmp[count+i];
			for (int i=0; i<4; i++) fRec103_perm[i]=fRec103_tmp[count+i];
			for (int i=0; i<4; i++) fRec104_perm[i]=fRec104_tmp[count+i];
			for (int i=0; i<4; i++) fRec105_perm[i]=fRec105_tmp[count+i];
			for (int i=0; i<4; i++) fYec30_perm[i]=fYec30_tmp[count+i];
			for (int i=0; i<4; i++) fRec106_perm[i]=fRec106_tmp[count+i];
			for (int i=0; i<4; i++) fRec107_perm[i]=fRec107_tmp[count+i];
			for (int i=0; i<4; i++) fRec108_perm[i]=fRec108_tmp[count+i];
			for (int i=0; i<4; i++) fRec97_perm[i]=fRec97_tmp[count+i];
			for (int i=0; i<4; i++) fRec98_perm[i]=fRec98_tmp[count+i];
			for (int i=0; i<4; i++) fRec99_perm[i]=fRec99_tmp[count+i];
			for (int i=0; i<4; i++) fRec100_perm[i]=fRec100_tmp[count+i];
			for (int i=0; i<4; i++) fRec101_perm[i]=fRec101_tmp[count+i];
			fYec29_idx_save = count;
			fYec28_idx_save = count;
			for (int i=0; i<4; i++) fRec14_perm[i]=fRec14_tmp[count+i];
			fYec27_idx_save = count;
			for (int i=0; i<4; i++) fRec93_perm[i]=fRec93_tmp[count+i];
			for (int i=0; i<4; i++) fRec94_perm[i]=fRec94_tmp[count+i];
			for (int i=0; i<4; i++) fRec95_perm[i]=fRec95_tmp[count+i];
			for (int i=0; i<4; i++) fRec87_perm[i]=fRec87_tmp[count+i];
			for (int i=0; i<4; i++) fRec88_perm[i]=fRec88_tmp[count+i];
			for (int i=0; i<4; i++) fRec89_perm[i]=fRec89_tmp[count+i];
			for (int i=0; i<4; i++) fYec26_perm[i]=fYec26_tmp[count+i];
			for (int i=0; i<4; i++) fRec90_perm[i]=fRec90_tmp[count+i];
			for (int i=0; i<4; i++) fRec91_perm[i]=fRec91_tmp[count+i];
			for (int i=0; i<4; i++) fRec92_perm[i]=fRec92_tmp[count+i];
			for (int i=0; i<4; i++) fRec17_perm[i]=fRec17_tmp[count+i];
			for (int i=0; i<4; i++) fRec18_perm[i]=fRec18_tmp[count+i];
			for (int i=0; i<4; i++) fRec19_perm[i]=fRec19_tmp[count+i];
			for (int i=0; i<4; i++) fRec20_perm[i]=fRec20_tmp[count+i];
			for (int i=0; i<4; i++) fRec21_perm[i]=fRec21_tmp[count+i];
			fYec25_idx_save = count;
			fYec24_idx_save = count;
			for (int i=0; i<4; i++) fRec23_perm[i]=fRec23_tmp[count+i];
			for (int i=0; i<4; i++) fRec84_perm[i]=fRec84_tmp[count+i];
			fYec23_idx_save = count;
			for (int i=0; i<4; i++) fRec22_perm[i]=fRec22_tmp[count+i];
			for (int i=0; i<4; i++) fRec24_perm[i]=fRec24_tmp[count+i];
			fYec22_idx_save = count;
			for (int i=0; i<4; i++) fRec26_perm[i]=fRec26_tmp[count+i];
			for (int i=0; i<4; i++) fRec81_perm[i]=fRec81_tmp[count+i];
			fYec21_idx_save = count;
			for (int i=0; i<4; i++) fRec25_perm[i]=fRec25_tmp[count+i];
			for (int i=0; i<4; i++) fRec27_perm[i]=fRec27_tmp[count+i];
			fYec20_idx_save = count;
			for (int i=0; i<4; i++) fRec29_perm[i]=fRec29_tmp[count+i];
			for (int i=0; i<4; i++) fRec78_perm[i]=fRec78_tmp[count+i];
			fYec19_idx_save = count;
			for (int i=0; i<4; i++) fRec28_perm[i]=fRec28_tmp[count+i];
			for (int i=0; i<4; i++) fRec30_perm[i]=fRec30_tmp[count+i];
			fYec18_idx_save = count;
			for (int i=0; i<4; i++) fRec32_perm[i]=fRec32_tmp[count+i];
			for (int i=0; i<4; i++) fRec75_perm[i]=fRec75_tmp[count+i];
			fYec17_idx_save = count;
			for (int i=0; i<4; i++) fRec31_perm[i]=fRec31_tmp[count+i];
			for (int i=0; i<4; i++) fRec33_perm[i]=fRec33_tmp[count+i];
			fYec16_idx_save = count;
			for (int i=0; i<4; i++) fRec35_perm[i]=fRec35_tmp[count+i];
			for (int i=0; i<4; i++) fRec72_perm[i]=fRec72_tmp[count+i];
			fYec15_idx_save = count;
			for (int i=0; i<4; i++) fRec34_perm[i]=fRec34_tmp[count+i];
			for (int i=0; i<4; i++) fRec36_perm[i]=fRec36_tmp[count+i];
			fYec14_idx_save = count;
			for (int i=0; i<4; i++) fRec69_perm[i]=fRec69_tmp[count+i];
			fYec13_idx_save = count;
			fYec12_idx_save = count;
			for (int i=0; i<4; i++) fRec37_perm[i]=fRec37_tmp[count+i];
			fYec11_idx_save = count;
			fYec10_idx_save = count;
			for (int i=0; i<4; i++) fRec39_perm[i]=fRec39_tmp[count+i];
			for (int i=0; i<4; i++) fRec66_perm[i]=fRec66_tmp[count+i];
			fYec9_idx_save = count;
			for (int i=0; i<4; i++) fRec38_perm[i]=fRec38_tmp[count+i];
			for (int i=0; i<4; i++) fRec40_perm[i]=fRec40_tmp[count+i];
			fYec8_idx_save = count;
			for (int i=0; i<4; i++) fRec42_perm[i]=fRec42_tmp[count+i];
			for (int i=0; i<4; i++) fRec63_perm[i]=fRec63_tmp[count+i];
			fYec7_idx_save = count;
			for (int i=0; i<4; i++) fRec41_perm[i]=fRec41_tmp[count+i];
			for (int i=0; i<4; i++) fRec43_perm[i]=fRec43_tmp[count+i];
			fYec6_idx_save = count;
			for (int i=0; i<4; i++) fRec45_perm[i]=fRec45_tmp[count+i];
			for (int i=0; i<4; i++) fRec60_perm[i]=fRec60_tmp[count+i];
			fYec5_idx_save = count;
			for (int i=0; i<4; i++) fRec44_perm[i]=fRec44_tmp[count+i];
			for (int i=0; i<4; i++) fRec46_perm[i]=fRec46_tmp[count+i];
			fYec4_idx_save = count;
			for (int i=0; i<4; i++) fRec48_perm[i]=fRec48_tmp[count+i];
			for (int i=0; i<4; i++) fRec57_perm[i]=fRec57_tmp[count+i];
			fYec3_idx_save = count;
			for (int i=0; i<4; i++) fRec47_perm[i]=fRec47_tmp[count+i];
			for (int i=0; i<4; i++) fRec49_perm[i]=fRec49_tmp[count+i];
			fYec2_idx_save = count;
			for (int i=0; i<4; i++) fRec51_perm[i]=fRec51_tmp[count+i];
			for (int i=0; i<4; i++) fRec54_perm[i]=fRec54_tmp[count+i];
			fYec1_idx_save = count;
			for (int i=0; i<4; i++) fRec50_perm[i]=fRec50_tmp[count+i];
			for (int i=0; i<4; i++) fRec52_perm[i]=fRec52_tmp[count+i];
			fYec0_idx_save = count;
			
			// SECTION : 11
			// LOOP 0x7fc571706dc0
			// exec code
			for (int i=0; i<count; i++) {
				output0[i] = (FAUSTFLOAT)fRec0[i];
			}
			
			// LOOP 0x7fc571a19e90
			// exec code
			for (int i=0; i<count; i++) {
				output1[i] = (FAUSTFLOAT)fRec1[i];
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
			// LOOP 0x7fc57170af00
			// pre processing
			for (int i=0; i<4; i++) iVec0_tmp[i]=iVec0_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				iVec0[i] = 1;
			}
			// post processing
			for (int i=0; i<4; i++) iVec0_perm[i]=iVec0_tmp[count+i];
			
			// SECTION : 2
			// LOOP 0x7fc57170ae20
			// exec code
			for (int i=0; i<count; i++) {
				iZec0[i] = (1 - iVec0[i-1]);
			}
			
			// SECTION : 3
			// LOOP 0x7fc571708fb0
			// pre processing
			for (int i=0; i<4; i++) fRec15_tmp[i]=fRec15_perm[i];
			for (int i=0; i<4; i++) fRec16_tmp[i]=fRec16_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec15[i] = ((fSlow1 * fRec16[i-1]) + (fSlow2 * fRec15[i-1]));
				fRec16[i] = (((fSlow2 * fRec16[i-1]) + (fSlow3 * fRec15[i-1])) + iZec0[i]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec16_perm[i]=fRec16_tmp[count+i];
			for (int i=0; i<4; i++) fRec15_perm[i]=fRec15_tmp[count+i];
			
			// SECTION : 4
			// LOOP 0x7fc571684150
			// exec code
			for (int i=0; i<count; i++) {
				fZec69[i] = (fSlow28 * (fRec16[i] + 1));
			}
			
			// LOOP 0x7fc5716a0800
			// exec code
			for (int i=0; i<count; i++) {
				fZec192[i] = (fSlow28 * (fRec15[i] + 1));
			}
			
			// SECTION : 5
			// LOOP 0x7fc5709f3660
			// pre processing
			for (int i=0; i<4; i++) fRec82_tmp[i]=fRec82_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec82[i] = (fSlow47 + (0.9999f * ((iSlow46 * iZec0[i]) + fRec82[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec82_perm[i]=fRec82_tmp[count+i];
			
			// LOOP 0x7fc5715df5e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec100[i] = (fSlow31 * fRec16[i]);
			}
			
			// LOOP 0x7fc571684070
			// exec code
			for (int i=0; i<count; i++) {
				fZec70[i] = (fZec69[i] + 3.500005f);
			}
			
			// LOOP 0x7fc571693460
			// pre processing
			for (int i=0; i<4; i++) fRec80_tmp[i]=fRec80_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec80[i] = (fSlow45 + (0.9999f * ((iSlow44 * iZec0[i]) + fRec80[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec80_perm[i]=fRec80_tmp[count+i];
			
			// LOOP 0x7fc5716a0720
			// exec code
			for (int i=0; i<count; i++) {
				fZec193[i] = (fZec192[i] + 3.500005f);
			}
			
			// LOOP 0x7fc57180feb0
			// pre processing
			for (int i=0; i<4; i++) fRec76_tmp[i]=fRec76_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec76[i] = (fSlow39 + (0.9999f * ((iSlow38 * iZec0[i]) + fRec76[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec76_perm[i]=fRec76_tmp[count+i];
			
			// LOOP 0x7fc5718286e0
			// pre processing
			for (int i=0; i<4; i++) fRec79_tmp[i]=fRec79_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec79[i] = (fSlow43 + (0.9999f * ((iSlow42 * iZec0[i]) + fRec79[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec79_perm[i]=fRec79_tmp[count+i];
			
			// LOOP 0x7fc571844850
			// pre processing
			for (int i=0; i<4; i++) fRec85_tmp[i]=fRec85_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec85[i] = (fSlow51 + (0.9999f * ((iSlow50 * iZec0[i]) + fRec85[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec85_perm[i]=fRec85_tmp[count+i];
			
			// LOOP 0x7fc57190e9f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec225[i] = (fSlow31 * fRec15[i]);
			}
			
			// SECTION : 6
			// LOOP 0x7fc57158f810
			// pre processing
			for (int i=0; i<4; i++) fRec56_tmp[i]=fRec56_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec56[i] = (fSlow10 + (0.9999f * ((iSlow9 * iZec0[i]) + fRec56[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec56_perm[i]=fRec56_tmp[count+i];
			
			// LOOP 0x7fc571592cf0
			// pre processing
			for (int i=0; i<4; i++) fRec59_tmp[i]=fRec59_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec59[i] = (fSlow14 + (0.9999f * ((iSlow13 * iZec0[i]) + fRec59[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec59_perm[i]=fRec59_tmp[count+i];
			
			// LOOP 0x7fc5715a9b20
			// pre processing
			for (int i=0; i<4; i++) fRec58_tmp[i]=fRec58_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec58[i] = (fSlow12 + (0.9999f * ((iSlow11 * iZec0[i]) + fRec58[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec58_perm[i]=fRec58_tmp[count+i];
			
			// LOOP 0x7fc5715af780
			// pre processing
			for (int i=0; i<4; i++) fRec55_tmp[i]=fRec55_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec55[i] = (fSlow8 + (0.9999f * ((iSlow7 * iZec0[i]) + fRec55[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec55_perm[i]=fRec55_tmp[count+i];
			
			// LOOP 0x7fc5715b6770
			// pre processing
			for (int i=0; i<4; i++) fRec61_tmp[i]=fRec61_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec61[i] = (fSlow16 + (0.9999f * ((iSlow15 * iZec0[i]) + fRec61[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec61_perm[i]=fRec61_tmp[count+i];
			
			// LOOP 0x7fc5715bd380
			// pre processing
			for (int i=0; i<4; i++) fRec62_tmp[i]=fRec62_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec62[i] = (fSlow18 + (0.9999f * ((iSlow17 * iZec0[i]) + fRec62[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec62_perm[i]=fRec62_tmp[count+i];
			
			// LOOP 0x7fc5715c6920
			// pre processing
			for (int i=0; i<4; i++) fRec64_tmp[i]=fRec64_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec64[i] = (fSlow20 + (0.9999f * ((iSlow19 * iZec0[i]) + fRec64[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec64_perm[i]=fRec64_tmp[count+i];
			
			// LOOP 0x7fc5715ce270
			// pre processing
			for (int i=0; i<4; i++) fRec65_tmp[i]=fRec65_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec65[i] = (fSlow22 + (0.9999f * ((iSlow21 * iZec0[i]) + fRec65[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec65_perm[i]=fRec65_tmp[count+i];
			
			// LOOP 0x7fc5715ddc80
			// pre processing
			for (int i=0; i<4; i++) fRec70_tmp[i]=fRec70_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec70[i] = (fSlow30 + (0.995f * ((iSlow29 * iZec0[i]) + fRec70[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec70_perm[i]=fRec70_tmp[count+i];
			
			// LOOP 0x7fc5715df500
			// exec code
			for (int i=0; i<count; i++) {
				fZec101[i] = (fSlow28 + (fZec100[i] + 3.500005f));
			}
			
			// LOOP 0x7fc5715ead90
			// pre processing
			for (int i=0; i<4; i++) fRec71_tmp[i]=fRec71_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec71[i] = (fSlow33 + (0.9999f * ((iSlow32 * iZec0[i]) + fRec71[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec71_perm[i]=fRec71_tmp[count+i];
			
			// LOOP 0x7fc5715f56d0
			// pre processing
			for (int i=0; i<4; i++) fRec73_tmp[i]=fRec73_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec73[i] = (fSlow35 + (0.9999f * ((iSlow34 * iZec0[i]) + fRec73[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec73_perm[i]=fRec73_tmp[count+i];
			
			// LOOP 0x7fc571679e90
			// pre processing
			for (int i=0; i<4; i++) fRec67_tmp[i]=fRec67_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec67[i] = (fSlow24 + (0.9999f * ((iSlow23 * iZec0[i]) + fRec67[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec67_perm[i]=fRec67_tmp[count+i];
			
			// LOOP 0x7fc5716824c0
			// pre processing
			for (int i=0; i<4; i++) fRec68_tmp[i]=fRec68_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec68[i] = (fSlow26 + (0.995f * ((iSlow25 * iZec0[i]) + fRec68[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec68_perm[i]=fRec68_tmp[count+i];
			
			// LOOP 0x7fc571685ba0
			// exec code
			for (int i=0; i<count; i++) {
				fZec73[i] = floorf(fZec70[i]);
			}
			
			// LOOP 0x7fc571688430
			// exec code
			for (int i=0; i<count; i++) {
				fZec151[i] = (fRec79[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571696850
			// exec code
			for (int i=0; i<count; i++) {
				fZec162[i] = (fRec80[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5716a1b60
			// exec code
			for (int i=0; i<count; i++) {
				fZec196[i] = floorf(fZec193[i]);
			}
			
			// LOOP 0x7fc5716d1680
			// pre processing
			for (int i=0; i<4; i++) fRec116_tmp[i]=fRec116_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec116[i] = (fSlow99 + (0.9999f * ((iSlow98 * iZec0[i]) + fRec116[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec116_perm[i]=fRec116_tmp[count+i];
			
			// LOOP 0x7fc5716e7a50
			// pre processing
			for (int i=0; i<4; i++) fRec117_tmp[i]=fRec117_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec117[i] = (fSlow101 + (0.9999f * ((iSlow100 * iZec0[i]) + fRec117[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec117_perm[i]=fRec117_tmp[count+i];
			
			// LOOP 0x7fc571711590
			// pre processing
			for (int i=0; i<4; i++) fRec53_tmp[i]=fRec53_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec53[i] = (fSlow6 + (0.9999f * ((iSlow5 * iZec0[i]) + fRec53[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec53_perm[i]=fRec53_tmp[count+i];
			
			// LOOP 0x7fc571803db0
			// pre processing
			for (int i=0; i<4; i++) fRec74_tmp[i]=fRec74_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec74[i] = (fSlow37 + (0.9999f * ((iSlow36 * iZec0[i]) + fRec74[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec74_perm[i]=fRec74_tmp[count+i];
			
			// LOOP 0x7fc571811790
			// exec code
			for (int i=0; i<count; i++) {
				fZec135[i] = (fRec76[i] + -1.49999f);
			}
			
			// LOOP 0x7fc57181ba30
			// pre processing
			for (int i=0; i<4; i++) fRec77_tmp[i]=fRec77_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec77[i] = (fSlow41 + (0.9999f * ((iSlow40 * iZec0[i]) + fRec77[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec77_perm[i]=fRec77_tmp[count+i];
			
			// LOOP 0x7fc57182a590
			// exec code
			for (int i=0; i<count; i++) {
				fZec169[i] = (fRec82[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571836320
			// pre processing
			for (int i=0; i<4; i++) fRec83_tmp[i]=fRec83_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec83[i] = (fSlow49 + (0.9999f * ((iSlow48 * iZec0[i]) + fRec83[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec83_perm[i]=fRec83_tmp[count+i];
			
			// LOOP 0x7fc571846150
			// exec code
			for (int i=0; i<count; i++) {
				fZec185[i] = (fRec85[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571852c60
			// pre processing
			for (int i=0; i<4; i++) fRec86_tmp[i]=fRec86_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec86[i] = (fSlow53 + (0.995f * ((iSlow52 * iZec0[i]) + fRec86[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec86_perm[i]=fRec86_tmp[count+i];
			
			// LOOP 0x7fc5718978e0
			// pre processing
			for (int i=0; i<4; i++) fRec113_tmp[i]=fRec113_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec113[i] = (fSlow97 + (0.9999f * ((iSlow96 * iZec0[i]) + fRec113[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec113_perm[i]=fRec113_tmp[count+i];
			
			// LOOP 0x7fc57190d080
			// pre processing
			for (int i=0; i<4; i++) fRec102_tmp[i]=fRec102_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec102[i] = (fSlow91 + (0.995f * ((iSlow90 * iZec0[i]) + fRec102[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec102_perm[i]=fRec102_tmp[count+i];
			
			// LOOP 0x7fc57190e910
			// exec code
			for (int i=0; i<count; i++) {
				fZec226[i] = (fSlow28 + (fZec225[i] + 3.500005f));
			}
			
			// SECTION : 7
			// LOOP 0x7fc57158c150
			// exec code
			for (int i=0; i<count; i++) {
				fZec31[i] = (fRec59[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571592780
			// exec code
			for (int i=0; i<count; i++) {
				fZec3[i] = (fRec53[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715a5c60
			// exec code
			for (int i=0; i<count; i++) {
				fZec17[i] = (fRec56[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715ab2d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec22[i] = (fRec58[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715b0f10
			// exec code
			for (int i=0; i<count; i++) {
				fZec8[i] = (fRec55[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715b7f20
			// exec code
			for (int i=0; i<count; i++) {
				fZec36[i] = (fRec61[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715c1300
			// exec code
			for (int i=0; i<count; i++) {
				fZec45[i] = (fRec62[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715c8100
			// exec code
			for (int i=0; i<count; i++) {
				fZec50[i] = (fRec64[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715d6d80
			// exec code
			for (int i=0; i<count; i++) {
				fZec95[i] = (fRec68[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715e0820
			// exec code
			for (int i=0; i<count; i++) {
				fZec103[i] = floorf(fZec101[i]);
			}
			
			// LOOP 0x7fc5715e71a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec109[i] = (fRec70[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715ed640
			// exec code
			for (int i=0; i<count; i++) {
				fZec116[i] = (fRec71[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5715f6ef0
			// exec code
			for (int i=0; i<count; i++) {
				fZec121[i] = (fRec73[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571673ca0
			// exec code
			for (int i=0; i<count; i++) {
				fZec59[i] = (fRec65[i] + -1.49999f);
			}
			
			// LOOP 0x7fc57167b670
			// exec code
			for (int i=0; i<count; i++) {
				fZec64[i] = (fRec67[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571685a30
			// exec code
			for (int i=0; i<count; i++) {
				fZec74[i] = (fZec69[i] + (4.0f - fZec73[i]));
			}
			
			// LOOP 0x7fc571686910
			// exec code
			for (int i=0; i<count; i++) {
				fZec76[i] = (fZec69[i] + (3.0f - fZec73[i]));
			}
			
			// LOOP 0x7fc5716896a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec154[i] = floorf(fZec151[i]);
			}
			
			// LOOP 0x7fc571697510
			// exec code
			for (int i=0; i<count; i++) {
				fZec164[i] = floorf(fZec162[i]);
			}
			
			// LOOP 0x7fc5716a1a50
			// exec code
			for (int i=0; i<count; i++) {
				fZec197[i] = (fZec192[i] + (4.0f - fZec196[i]));
			}
			
			// LOOP 0x7fc5716a2890
			// exec code
			for (int i=0; i<count; i++) {
				fZec199[i] = (fZec192[i] + (3.0f - fZec196[i]));
			}
			
			// LOOP 0x7fc5716ab4e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec218[i] = (fRec86[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5716d2ed0
			// exec code
			for (int i=0; i<count; i++) {
				fZec256[i] = (fRec116[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5716ead30
			// exec code
			for (int i=0; i<count; i++) {
				fZec265[i] = (fRec117[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5718071a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec130[i] = (fRec74[i] + -1.49999f);
			}
			
			// LOOP 0x7fc5718129d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec138[i] = floorf(fZec135[i]);
			}
			
			// LOOP 0x7fc57181edd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec146[i] = (fRec77[i] + -1.49999f);
			}
			
			// LOOP 0x7fc57182b7d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec172[i] = floorf(fZec169[i]);
			}
			
			// LOOP 0x7fc5718396c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec180[i] = (fRec83[i] + -1.49999f);
			}
			
			// LOOP 0x7fc571847390
			// exec code
			for (int i=0; i<count; i++) {
				fZec188[i] = floorf(fZec185[i]);
			}
			
			// LOOP 0x7fc5718990d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec243[i] = (fRec113[i] + -1.49999f);
			}
			
			// LOOP 0x7fc57190f980
			// exec code
			for (int i=0; i<count; i++) {
				fZec228[i] = floorf(fZec226[i]);
			}
			
			// LOOP 0x7fc5719167e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec234[i] = (fRec102[i] + -1.49999f);
			}
			
			// SECTION : 8
			// LOOP 0x7fc57158cdb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec33[i] = floorf(fZec31[i]);
			}
			
			// LOOP 0x7fc5715a68c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec19[i] = floorf(fZec17[i]);
			}
			
			// LOOP 0x7fc5715ac190
			// exec code
			for (int i=0; i<count; i++) {
				fZec24[i] = floorf(fZec22[i]);
			}
			
			// LOOP 0x7fc5715ad300
			// exec code
			for (int i=0; i<count; i++) {
				fZec5[i] = floorf(fZec3[i]);
			}
			
			// LOOP 0x7fc5715b1dd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec10[i] = floorf(fZec8[i]);
			}
			
			// LOOP 0x7fc5715b8de0
			// exec code
			for (int i=0; i<count; i++) {
				fZec38[i] = floorf(fZec36[i]);
			}
			
			// LOOP 0x7fc5715c1f60
			// exec code
			for (int i=0; i<count; i++) {
				fZec47[i] = floorf(fZec45[i]);
			}
			
			// LOOP 0x7fc5715c8fe0
			// exec code
			for (int i=0; i<count; i++) {
				fZec52[i] = floorf(fZec50[i]);
			}
			
			// LOOP 0x7fc5715d3670
			// exec code
			for (int i=0; i<count; i++) {
				fZec90[i] = (fZec74[i] * fZec76[i]);
			}
			
			// LOOP 0x7fc5715d79a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec97[i] = floorf(fZec95[i]);
			}
			
			// LOOP 0x7fc5715e0700
			// exec code
			for (int i=0; i<count; i++) {
				fZec104[i] = (fSlow28 + (fZec100[i] + (4.0f - fZec103[i])));
			}
			
			// LOOP 0x7fc5715e1450
			// exec code
			for (int i=0; i<count; i++) {
				fZec105[i] = (fSlow28 + (fZec100[i] + (3.0f - fZec103[i])));
			}
			
			// LOOP 0x7fc5715e8570
			// exec code
			for (int i=0; i<count; i++) {
				fZec111[i] = floorf(fZec109[i]);
			}
			
			// LOOP 0x7fc5715ee260
			// exec code
			for (int i=0; i<count; i++) {
				fZec118[i] = floorf(fZec116[i]);
			}
			
			// LOOP 0x7fc5715f7e30
			// exec code
			for (int i=0; i<count; i++) {
				fZec123[i] = floorf(fZec121[i]);
			}
			
			// LOOP 0x7fc571674900
			// exec code
			for (int i=0; i<count; i++) {
				fZec61[i] = floorf(fZec59[i]);
			}
			
			// LOOP 0x7fc57167c550
			// exec code
			for (int i=0; i<count; i++) {
				fZec66[i] = floorf(fZec64[i]);
			}
			
			// LOOP 0x7fc571683f90
			// exec code
			for (int i=0; i<count; i++) {
				iZec71[i] = int(fZec70[i]);
			}
			
			// LOOP 0x7fc5716875a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec78[i] = (fZec69[i] + (2.0f - fZec73[i]));
			}
			
			// LOOP 0x7fc571688350
			// exec code
			for (int i=0; i<count; i++) {
				iZec152[i] = int(min(8192, max(0, int(fZec151[i]))));
			}
			
			// LOOP 0x7fc5716895c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec155[i] = (fZec154[i] + (2.0f - fRec79[i]));
			}
			
			// LOOP 0x7fc571689ef0
			// exec code
			for (int i=0; i<count; i++) {
				fZec156[i] = (fRec79[i] - fZec154[i]);
			}
			
			// LOOP 0x7fc571696730
			// exec code
			for (int i=0; i<count; i++) {
				iZec163[i] = int(min(8192, max(0, int(fZec162[i]))));
			}
			
			// LOOP 0x7fc571697430
			// exec code
			for (int i=0; i<count; i++) {
				fZec165[i] = (fZec164[i] + (2.0f - fRec80[i]));
			}
			
			// LOOP 0x7fc571697e80
			// exec code
			for (int i=0; i<count; i++) {
				fZec166[i] = (fRec80[i] - fZec164[i]);
			}
			
			// LOOP 0x7fc5716a0640
			// exec code
			for (int i=0; i<count; i++) {
				iZec194[i] = int(fZec193[i]);
			}
			
			// LOOP 0x7fc5716a34f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec201[i] = (fZec192[i] + (2.0f - fZec196[i]));
			}
			
			// LOOP 0x7fc5716a3f60
			// exec code
			for (int i=0; i<count; i++) {
				fZec203[i] = (fZec192[i] + (1.0f - fZec196[i]));
			}
			
			// LOOP 0x7fc5716a7f50
			// exec code
			for (int i=0; i<count; i++) {
				fZec213[i] = (fZec197[i] * fZec199[i]);
			}
			
			// LOOP 0x7fc5716ac130
			// exec code
			for (int i=0; i<count; i++) {
				fZec220[i] = floorf(fZec218[i]);
			}
			
			// LOOP 0x7fc5716d3e10
			// exec code
			for (int i=0; i<count; i++) {
				fZec258[i] = floorf(fZec256[i]);
			}
			
			// LOOP 0x7fc5716eb980
			// exec code
			for (int i=0; i<count; i++) {
				fZec267[i] = floorf(fZec265[i]);
			}
			
			// LOOP 0x7fc57170f050
			// exec code
			for (int i=0; i<count; i++) {
				fZec80[i] = (fZec69[i] + (1.0f - fZec73[i]));
			}
			
			// LOOP 0x7fc571807e60
			// exec code
			for (int i=0; i<count; i++) {
				fZec132[i] = floorf(fZec130[i]);
			}
			
			// LOOP 0x7fc5718116b0
			// exec code
			for (int i=0; i<count; i++) {
				iZec136[i] = int(min(8192, max(0, int(fZec135[i]))));
			}
			
			// LOOP 0x7fc5718128f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec139[i] = (fZec138[i] + (2.0f - fRec76[i]));
			}
			
			// LOOP 0x7fc571813240
			// exec code
			for (int i=0; i<count; i++) {
				fZec140[i] = (fRec76[i] - fZec138[i]);
			}
			
			// LOOP 0x7fc57181fa70
			// exec code
			for (int i=0; i<count; i++) {
				fZec148[i] = floorf(fZec146[i]);
			}
			
			// LOOP 0x7fc57182a4b0
			// exec code
			for (int i=0; i<count; i++) {
				iZec170[i] = int(min(8192, max(0, int(fZec169[i]))));
			}
			
			// LOOP 0x7fc57182b6f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec173[i] = (fZec172[i] + (2.0f - fRec82[i]));
			}
			
			// LOOP 0x7fc57182c040
			// exec code
			for (int i=0; i<count; i++) {
				fZec174[i] = (fRec82[i] - fZec172[i]);
			}
			
			// LOOP 0x7fc57183a360
			// exec code
			for (int i=0; i<count; i++) {
				fZec182[i] = floorf(fZec180[i]);
			}
			
			// LOOP 0x7fc571846070
			// exec code
			for (int i=0; i<count; i++) {
				iZec186[i] = int(min(8192, max(0, int(fZec185[i]))));
			}
			
			// LOOP 0x7fc5718472b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec189[i] = (fZec188[i] + (2.0f - fRec85[i]));
			}
			
			// LOOP 0x7fc571847c00
			// exec code
			for (int i=0; i<count; i++) {
				fZec190[i] = (fRec85[i] - fZec188[i]);
			}
			
			// LOOP 0x7fc57189a010
			// exec code
			for (int i=0; i<count; i++) {
				fZec245[i] = floorf(fZec243[i]);
			}
			
			// LOOP 0x7fc57190f860
			// exec code
			for (int i=0; i<count; i++) {
				fZec229[i] = (fSlow28 + (fZec225[i] + (4.0f - fZec228[i])));
			}
			
			// LOOP 0x7fc571910580
			// exec code
			for (int i=0; i<count; i++) {
				fZec230[i] = (fSlow28 + (fZec225[i] + (3.0f - fZec228[i])));
			}
			
			// LOOP 0x7fc571917710
			// exec code
			for (int i=0; i<count; i++) {
				fZec236[i] = floorf(fZec234[i]);
			}
			
			// SECTION : 9
			// LOOP 0x7fc57158c030
			// exec code
			for (int i=0; i<count; i++) {
				iZec32[i] = int(min(8192, max(0, int(fZec31[i]))));
			}
			
			// LOOP 0x7fc57158ccd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec34[i] = (fZec33[i] + (2.0f - fRec59[i]));
			}
			
			// LOOP 0x7fc57158d6b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec35[i] = (fRec59[i] - fZec33[i]);
			}
			
			// LOOP 0x7fc571592650
			// exec code
			for (int i=0; i<count; i++) {
				iZec4[i] = int(min(8192, max(0, int(fZec3[i]))));
			}
			
			// LOOP 0x7fc5715a5b40
			// exec code
			for (int i=0; i<count; i++) {
				iZec18[i] = int(min(8192, max(0, int(fZec17[i]))));
			}
			
			// LOOP 0x7fc5715a67e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec20[i] = (fZec19[i] + (2.0f - fRec56[i]));
			}
			
			// LOOP 0x7fc5715a71c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec21[i] = (fRec56[i] - fZec19[i]);
			}
			
			// LOOP 0x7fc5715ab1b0
			// exec code
			for (int i=0; i<count; i++) {
				iZec23[i] = int(min(8192, max(0, int(fZec22[i]))));
			}
			
			// LOOP 0x7fc5715ac010
			// exec code
			for (int i=0; i<count; i++) {
				fZec25[i] = (fZec24[i] + (2.0f - fRec58[i]));
			}
			
			// LOOP 0x7fc5715ac9c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec26[i] = (fRec58[i] - fZec24[i]);
			}
			
			// LOOP 0x7fc5715ad1b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec6[i] = (fZec5[i] + (2.0f - fRec53[i]));
			}
			
			// LOOP 0x7fc5715adbe0
			// exec code
			for (int i=0; i<count; i++) {
				fZec7[i] = (fRec53[i] - fZec5[i]);
			}
			
			// LOOP 0x7fc5715b0df0
			// exec code
			for (int i=0; i<count; i++) {
				iZec9[i] = int(min(8192, max(0, int(fZec8[i]))));
			}
			
			// LOOP 0x7fc5715b1c50
			// exec code
			for (int i=0; i<count; i++) {
				fZec11[i] = (fZec10[i] + (2.0f - fRec55[i]));
			}
			
			// LOOP 0x7fc5715b2600
			// exec code
			for (int i=0; i<count; i++) {
				fZec12[i] = (fRec55[i] - fZec10[i]);
			}
			
			// LOOP 0x7fc5715b7e00
			// exec code
			for (int i=0; i<count; i++) {
				iZec37[i] = int(min(8192, max(0, int(fZec36[i]))));
			}
			
			// LOOP 0x7fc5715b8c60
			// exec code
			for (int i=0; i<count; i++) {
				fZec39[i] = (fZec38[i] + (2.0f - fRec61[i]));
			}
			
			// LOOP 0x7fc5715b9610
			// exec code
			for (int i=0; i<count; i++) {
				fZec40[i] = (fRec61[i] - fZec38[i]);
			}
			
			// LOOP 0x7fc5715c11e0
			// exec code
			for (int i=0; i<count; i++) {
				iZec46[i] = int(min(8192, max(0, int(fZec45[i]))));
			}
			
			// LOOP 0x7fc5715c1e80
			// exec code
			for (int i=0; i<count; i++) {
				fZec48[i] = (fZec47[i] + (2.0f - fRec62[i]));
			}
			
			// LOOP 0x7fc5715c28f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec49[i] = (fRec62[i] - fZec47[i]);
			}
			
			// LOOP 0x7fc5715c8020
			// exec code
			for (int i=0; i<count; i++) {
				iZec51[i] = int(min(8192, max(0, int(fZec50[i]))));
			}
			
			// LOOP 0x7fc5715c8e40
			// exec code
			for (int i=0; i<count; i++) {
				fZec53[i] = (fZec52[i] + (2.0f - fRec64[i]));
			}
			
			// LOOP 0x7fc5715c9840
			// exec code
			for (int i=0; i<count; i++) {
				fZec54[i] = (fRec64[i] - fZec52[i]);
			}
			
			// LOOP 0x7fc5715cfef0
			// exec code
			for (int i=0; i<count; i++) {
				fZec82[i] = (fZec69[i] + (5.0f - fZec73[i]));
			}
			
			// LOOP 0x7fc5715d0560
			// exec code
			for (int i=0; i<count; i++) {
				iZec83[i] = int(min(512, max(0, (iZec71[i] + 1))));
			}
			
			// LOOP 0x7fc5715d0cf0
			// exec code
			for (int i=0; i<count; i++) {
				fZec84[i] = (0 - fZec76[i]);
			}
			
			// LOOP 0x7fc5715d12d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec85[i] = (0 - (0.5f * fZec78[i]));
			}
			
			// LOOP 0x7fc5715d1930
			// exec code
			for (int i=0; i<count; i++) {
				fZec86[i] = (0 - (0.33333334f * fZec80[i]));
			}
			
			// LOOP 0x7fc5715d1f30
			// exec code
			for (int i=0; i<count; i++) {
				iZec87[i] = int(min(512, max(0, (iZec71[i] + 2))));
			}
			
			// LOOP 0x7fc5715d28c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec88[i] = (0 - fZec78[i]);
			}
			
			// LOOP 0x7fc5715d2e70
			// exec code
			for (int i=0; i<count; i++) {
				fZec89[i] = (0 - (0.5f * fZec80[i]));
			}
			
			// LOOP 0x7fc5715d3b10
			// exec code
			for (int i=0; i<count; i++) {
				iZec91[i] = int(min(512, max(0, (iZec71[i] + 3))));
			}
			
			// LOOP 0x7fc5715d4510
			// exec code
			for (int i=0; i<count; i++) {
				fZec92[i] = (0 - fZec80[i]);
			}
			
			// LOOP 0x7fc5715d4f30
			// exec code
			for (int i=0; i<count; i++) {
				fZec93[i] = (fZec90[i] * fZec78[i]);
			}
			
			// LOOP 0x7fc5715d51d0
			// exec code
			for (int i=0; i<count; i++) {
				iZec94[i] = int(min(512, max(0, (iZec71[i] + 4))));
			}
			
			// LOOP 0x7fc5715d6ca0
			// exec code
			for (int i=0; i<count; i++) {
				iZec96[i] = int(min(8192, max(0, int(fZec95[i]))));
			}
			
			// LOOP 0x7fc5715d78c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec98[i] = (fZec97[i] + (2.0f - fRec68[i]));
			}
			
			// LOOP 0x7fc5715d8330
			// exec code
			for (int i=0; i<count; i++) {
				fZec99[i] = (fRec68[i] - fZec97[i]);
			}
			
			// LOOP 0x7fc5715df420
			// exec code
			for (int i=0; i<count; i++) {
				iZec102[i] = int(fZec101[i]);
			}
			
			// LOOP 0x7fc5715e1c90
			// exec code
			for (int i=0; i<count; i++) {
				fZec106[i] = (fSlow28 + (fZec100[i] + (2.0f - fZec103[i])));
			}
			
			// LOOP 0x7fc5715e25d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec107[i] = (fSlow28 + (fZec100[i] + (1.0f - fZec103[i])));
			}
			
			// LOOP 0x7fc5715e4c90
			// exec code
			for (int i=0; i<count; i++) {
				fZec108[i] = (fZec104[i] * fZec105[i]);
			}
			
			// LOOP 0x7fc5715e70a0
			// exec code
			for (int i=0; i<count; i++) {
				iZec110[i] = int(min(8192, max(0, int(fZec109[i]))));
			}
			
			// LOOP 0x7fc5715e83f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec112[i] = (fZec111[i] + (2.0f - fRec70[i]));
			}
			
			// LOOP 0x7fc5715e8dc0
			// exec code
			for (int i=0; i<count; i++) {
				fZec113[i] = (fRec70[i] - fZec111[i]);
			}
			
			// LOOP 0x7fc5715ed560
			// exec code
			for (int i=0; i<count; i++) {
				iZec117[i] = int(min(8192, max(0, int(fZec116[i]))));
			}
			
			// LOOP 0x7fc5715ee180
			// exec code
			for (int i=0; i<count; i++) {
				fZec119[i] = (fZec118[i] + (2.0f - fRec71[i]));
			}
			
			// LOOP 0x7fc5715eebd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec120[i] = (fRec71[i] - fZec118[i]);
			}
			
			// LOOP 0x7fc5715f6e10
			// exec code
			for (int i=0; i<count; i++) {
				iZec122[i] = int(min(8192, max(0, int(fZec121[i]))));
			}
			
			// LOOP 0x7fc5715f7cb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec124[i] = (fZec123[i] + (2.0f - fRec73[i]));
			}
			
			// LOOP 0x7fc5715f8680
			// exec code
			for (int i=0; i<count; i++) {
				fZec125[i] = (fRec73[i] - fZec123[i]);
			}
			
			// LOOP 0x7fc571673b80
			// exec code
			for (int i=0; i<count; i++) {
				iZec60[i] = int(min(8192, max(0, int(fZec59[i]))));
			}
			
			// LOOP 0x7fc571674820
			// exec code
			for (int i=0; i<count; i++) {
				fZec62[i] = (fZec61[i] + (2.0f - fRec65[i]));
			}
			
			// LOOP 0x7fc571675200
			// exec code
			for (int i=0; i<count; i++) {
				fZec63[i] = (fRec65[i] - fZec61[i]);
			}
			
			// LOOP 0x7fc57167b590
			// exec code
			for (int i=0; i<count; i++) {
				iZec65[i] = int(min(8192, max(0, int(fZec64[i]))));
			}
			
			// LOOP 0x7fc57167c3b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec67[i] = (fZec66[i] + (2.0f - fRec67[i]));
			}
			
			// LOOP 0x7fc57167cdb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec68[i] = (fRec67[i] - fZec66[i]);
			}
			
			// LOOP 0x7fc571683e10
			// exec code
			for (int i=0; i<count; i++) {
				iZec72[i] = int(min(512, max(0, iZec71[i])));
			}
			
			// LOOP 0x7fc571685950
			// exec code
			for (int i=0; i<count; i++) {
				fZec75[i] = (0 - fZec74[i]);
			}
			
			// LOOP 0x7fc571686830
			// exec code
			for (int i=0; i<count; i++) {
				fZec77[i] = (0 - (0.5f * fZec76[i]));
			}
			
			// LOOP 0x7fc5716874c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec79[i] = (0 - (0.33333334f * fZec78[i]));
			}
			
			// LOOP 0x7fc571687ff0
			// exec code
			for (int i=0; i<count; i++) {
				fZec81[i] = (0 - (0.25f * fZec80[i]));
			}
			
			// LOOP 0x7fc571688270
			// exec code
			for (int i=0; i<count; i++) {
				iZec153[i] = int((iZec152[i] + 1));
			}
			
			// LOOP 0x7fc571689460
			// exec code
			for (int i=0; i<count; i++) {
				fZec157[i] = (0 - (fZec155[i] / fZec156[i]));
			}
			
			// LOOP 0x7fc571698460
			// exec code
			for (int i=0; i<count; i++) {
				iZec167[i] = int((iZec163[i] + 1));
			}
			
			// LOOP 0x7fc5716989e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec168[i] = (0 - (fZec165[i] / fZec166[i]));
			}
			
			// LOOP 0x7fc5716a0560
			// exec code
			for (int i=0; i<count; i++) {
				iZec195[i] = max(0, iZec194[i]);
			}
			
			// LOOP 0x7fc5716a1910
			// exec code
			for (int i=0; i<count; i++) {
				fZec198[i] = (0 - fZec197[i]);
			}
			
			// LOOP 0x7fc5716a26a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec200[i] = (0 - (0.5f * fZec199[i]));
			}
			
			// LOOP 0x7fc5716a3410
			// exec code
			for (int i=0; i<count; i++) {
				fZec202[i] = (0 - (0.33333334f * fZec201[i]));
			}
			
			// LOOP 0x7fc5716a3cc0
			// exec code
			for (int i=0; i<count; i++) {
				fZec204[i] = (0 - (0.25f * fZec203[i]));
			}
			
			// LOOP 0x7fc5716a49d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec205[i] = (fZec192[i] + (5.0f - fZec196[i]));
			}
			
			// LOOP 0x7fc5716a4ed0
			// exec code
			for (int i=0; i<count; i++) {
				iZec206[i] = max(0, (iZec194[i] + 1));
			}
			
			// LOOP 0x7fc5716a55f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec207[i] = (0 - fZec199[i]);
			}
			
			// LOOP 0x7fc5716a5a90
			// exec code
			for (int i=0; i<count; i++) {
				fZec208[i] = (0 - (0.5f * fZec201[i]));
			}
			
			// LOOP 0x7fc5716a62f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec209[i] = (0 - (0.33333334f * fZec203[i]));
			}
			
			// LOOP 0x7fc5716a6860
			// exec code
			for (int i=0; i<count; i++) {
				iZec210[i] = max(0, (iZec194[i] + 2));
			}
			
			// LOOP 0x7fc5716a70a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec211[i] = (0 - fZec201[i]);
			}
			
			// LOOP 0x7fc5716a7710
			// exec code
			for (int i=0; i<count; i++) {
				fZec212[i] = (0 - (0.5f * fZec203[i]));
			}
			
			// LOOP 0x7fc5716a7d60
			// exec code
			for (int i=0; i<count; i++) {
				fZec216[i] = (fZec213[i] * fZec201[i]);
			}
			
			// LOOP 0x7fc5716a83a0
			// exec code
			for (int i=0; i<count; i++) {
				iZec214[i] = max(0, (iZec194[i] + 3));
			}
			
			// LOOP 0x7fc5716a8be0
			// exec code
			for (int i=0; i<count; i++) {
				fZec215[i] = (0 - fZec203[i]);
			}
			
			// LOOP 0x7fc5716a9970
			// exec code
			for (int i=0; i<count; i++) {
				iZec217[i] = max(0, (iZec194[i] + 4));
			}
			
			// LOOP 0x7fc5716ab400
			// exec code
			for (int i=0; i<count; i++) {
				iZec219[i] = int(min(8192, max(0, int(fZec218[i]))));
			}
			
			// LOOP 0x7fc5716ac050
			// exec code
			for (int i=0; i<count; i++) {
				fZec221[i] = (fZec220[i] + (2.0f - fRec86[i]));
			}
			
			// LOOP 0x7fc5716acaa0
			// exec code
			for (int i=0; i<count; i++) {
				fZec222[i] = (fRec86[i] - fZec220[i]);
			}
			
			// LOOP 0x7fc5716d2df0
			// exec code
			for (int i=0; i<count; i++) {
				iZec257[i] = int(min(8192, max(0, int(fZec256[i]))));
			}
			
			// LOOP 0x7fc5716d3c90
			// exec code
			for (int i=0; i<count; i++) {
				fZec259[i] = (fZec258[i] + (2.0f - fRec116[i]));
			}
			
			// LOOP 0x7fc5716d4670
			// exec code
			for (int i=0; i<count; i++) {
				fZec260[i] = (fRec116[i] - fZec258[i]);
			}
			
			// LOOP 0x7fc5716eabd0
			// exec code
			for (int i=0; i<count; i++) {
				iZec266[i] = int(min(8192, max(0, int(fZec265[i]))));
			}
			
			// LOOP 0x7fc5716eb8a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec268[i] = (fZec267[i] + (2.0f - fRec117[i]));
			}
			
			// LOOP 0x7fc5716ec220
			// exec code
			for (int i=0; i<count; i++) {
				fZec269[i] = (fRec117[i] - fZec267[i]);
			}
			
			// LOOP 0x7fc571807080
			// exec code
			for (int i=0; i<count; i++) {
				iZec131[i] = int(min(8192, max(0, int(fZec130[i]))));
			}
			
			// LOOP 0x7fc571807d80
			// exec code
			for (int i=0; i<count; i++) {
				fZec133[i] = (fZec132[i] + (2.0f - fRec74[i]));
			}
			
			// LOOP 0x7fc5718087d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec134[i] = (fRec74[i] - fZec132[i]);
			}
			
			// LOOP 0x7fc5718115d0
			// exec code
			for (int i=0; i<count; i++) {
				iZec137[i] = int((iZec136[i] + 1));
			}
			
			// LOOP 0x7fc571812750
			// exec code
			for (int i=0; i<count; i++) {
				fZec141[i] = (0 - (fZec139[i] / fZec140[i]));
			}
			
			// LOOP 0x7fc57181ecb0
			// exec code
			for (int i=0; i<count; i++) {
				iZec147[i] = int(min(8192, max(0, int(fZec146[i]))));
			}
			
			// LOOP 0x7fc57181f990
			// exec code
			for (int i=0; i<count; i++) {
				fZec149[i] = (fZec148[i] + (2.0f - fRec77[i]));
			}
			
			// LOOP 0x7fc5718203e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec150[i] = (fRec77[i] - fZec148[i]);
			}
			
			// LOOP 0x7fc57182a3d0
			// exec code
			for (int i=0; i<count; i++) {
				iZec171[i] = int((iZec170[i] + 1));
			}
			
			// LOOP 0x7fc57182b550
			// exec code
			for (int i=0; i<count; i++) {
				fZec175[i] = (0 - (fZec173[i] / fZec174[i]));
			}
			
			// LOOP 0x7fc5718395a0
			// exec code
			for (int i=0; i<count; i++) {
				iZec181[i] = int(min(8192, max(0, int(fZec180[i]))));
			}
			
			// LOOP 0x7fc57183a280
			// exec code
			for (int i=0; i<count; i++) {
				fZec183[i] = (fZec182[i] + (2.0f - fRec83[i]));
			}
			
			// LOOP 0x7fc57183acd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec184[i] = (fRec83[i] - fZec182[i]);
			}
			
			// LOOP 0x7fc571845f90
			// exec code
			for (int i=0; i<count; i++) {
				iZec187[i] = int((iZec186[i] + 1));
			}
			
			// LOOP 0x7fc571847110
			// exec code
			for (int i=0; i<count; i++) {
				fZec191[i] = (0 - (fZec189[i] / fZec190[i]));
			}
			
			// LOOP 0x7fc571898ff0
			// exec code
			for (int i=0; i<count; i++) {
				iZec244[i] = int(min(8192, max(0, int(fZec243[i]))));
			}
			
			// LOOP 0x7fc571899e90
			// exec code
			for (int i=0; i<count; i++) {
				fZec246[i] = (fZec245[i] + (2.0f - fRec113[i]));
			}
			
			// LOOP 0x7fc57189a870
			// exec code
			for (int i=0; i<count; i++) {
				fZec247[i] = (fRec113[i] - fZec245[i]);
			}
			
			// LOOP 0x7fc57190e830
			// exec code
			for (int i=0; i<count; i++) {
				iZec227[i] = int(fZec226[i]);
			}
			
			// LOOP 0x7fc571910e20
			// exec code
			for (int i=0; i<count; i++) {
				fZec231[i] = (fSlow28 + (fZec225[i] + (2.0f - fZec228[i])));
			}
			
			// LOOP 0x7fc571911760
			// exec code
			for (int i=0; i<count; i++) {
				fZec232[i] = (fSlow28 + (fZec225[i] + (1.0f - fZec228[i])));
			}
			
			// LOOP 0x7fc571913ed0
			// exec code
			for (int i=0; i<count; i++) {
				fZec233[i] = (fZec229[i] * fZec230[i]);
			}
			
			// LOOP 0x7fc5719166c0
			// exec code
			for (int i=0; i<count; i++) {
				iZec235[i] = int(min(8192, max(0, int(fZec234[i]))));
			}
			
			// LOOP 0x7fc571917590
			// exec code
			for (int i=0; i<count; i++) {
				fZec237[i] = (fZec236[i] + (2.0f - fRec102[i]));
			}
			
			// LOOP 0x7fc571917f90
			// exec code
			for (int i=0; i<count; i++) {
				fZec238[i] = (fRec102[i] - fZec236[i]);
			}
			
			// SECTION : 10
			// LOOP 0x7fc571706ea0
			// pre processing
			fYec0_idx = (fYec0_idx+fYec0_idx_save)&16383;
			for (int i=0; i<4; i++) fRec52_tmp[i]=fRec52_perm[i];
			for (int i=0; i<4; i++) fRec50_tmp[i]=fRec50_perm[i];
			fYec1_idx = (fYec1_idx+fYec1_idx_save)&16383;
			for (int i=0; i<4; i++) fRec54_tmp[i]=fRec54_perm[i];
			for (int i=0; i<4; i++) fRec51_tmp[i]=fRec51_perm[i];
			fYec2_idx = (fYec2_idx+fYec2_idx_save)&16383;
			for (int i=0; i<4; i++) fRec49_tmp[i]=fRec49_perm[i];
			for (int i=0; i<4; i++) fRec47_tmp[i]=fRec47_perm[i];
			fYec3_idx = (fYec3_idx+fYec3_idx_save)&16383;
			for (int i=0; i<4; i++) fRec57_tmp[i]=fRec57_perm[i];
			for (int i=0; i<4; i++) fRec48_tmp[i]=fRec48_perm[i];
			fYec4_idx = (fYec4_idx+fYec4_idx_save)&16383;
			for (int i=0; i<4; i++) fRec46_tmp[i]=fRec46_perm[i];
			for (int i=0; i<4; i++) fRec44_tmp[i]=fRec44_perm[i];
			fYec5_idx = (fYec5_idx+fYec5_idx_save)&16383;
			for (int i=0; i<4; i++) fRec60_tmp[i]=fRec60_perm[i];
			for (int i=0; i<4; i++) fRec45_tmp[i]=fRec45_perm[i];
			fYec6_idx = (fYec6_idx+fYec6_idx_save)&16383;
			for (int i=0; i<4; i++) fRec43_tmp[i]=fRec43_perm[i];
			for (int i=0; i<4; i++) fRec41_tmp[i]=fRec41_perm[i];
			fYec7_idx = (fYec7_idx+fYec7_idx_save)&16383;
			for (int i=0; i<4; i++) fRec63_tmp[i]=fRec63_perm[i];
			for (int i=0; i<4; i++) fRec42_tmp[i]=fRec42_perm[i];
			fYec8_idx = (fYec8_idx+fYec8_idx_save)&16383;
			for (int i=0; i<4; i++) fRec40_tmp[i]=fRec40_perm[i];
			for (int i=0; i<4; i++) fRec38_tmp[i]=fRec38_perm[i];
			fYec9_idx = (fYec9_idx+fYec9_idx_save)&16383;
			for (int i=0; i<4; i++) fRec66_tmp[i]=fRec66_perm[i];
			for (int i=0; i<4; i++) fRec39_tmp[i]=fRec39_perm[i];
			fYec10_idx = (fYec10_idx+fYec10_idx_save)&1023;
			fYec11_idx = (fYec11_idx+fYec11_idx_save)&16383;
			for (int i=0; i<4; i++) fRec37_tmp[i]=fRec37_perm[i];
			fYec12_idx = (fYec12_idx+fYec12_idx_save)&1023;
			fYec13_idx = (fYec13_idx+fYec13_idx_save)&16383;
			for (int i=0; i<4; i++) fRec69_tmp[i]=fRec69_perm[i];
			fYec14_idx = (fYec14_idx+fYec14_idx_save)&16383;
			for (int i=0; i<4; i++) fRec36_tmp[i]=fRec36_perm[i];
			for (int i=0; i<4; i++) fRec34_tmp[i]=fRec34_perm[i];
			fYec15_idx = (fYec15_idx+fYec15_idx_save)&16383;
			for (int i=0; i<4; i++) fRec72_tmp[i]=fRec72_perm[i];
			for (int i=0; i<4; i++) fRec35_tmp[i]=fRec35_perm[i];
			fYec16_idx = (fYec16_idx+fYec16_idx_save)&16383;
			for (int i=0; i<4; i++) fRec33_tmp[i]=fRec33_perm[i];
			for (int i=0; i<4; i++) fRec31_tmp[i]=fRec31_perm[i];
			fYec17_idx = (fYec17_idx+fYec17_idx_save)&16383;
			for (int i=0; i<4; i++) fRec75_tmp[i]=fRec75_perm[i];
			for (int i=0; i<4; i++) fRec32_tmp[i]=fRec32_perm[i];
			fYec18_idx = (fYec18_idx+fYec18_idx_save)&16383;
			for (int i=0; i<4; i++) fRec30_tmp[i]=fRec30_perm[i];
			for (int i=0; i<4; i++) fRec28_tmp[i]=fRec28_perm[i];
			fYec19_idx = (fYec19_idx+fYec19_idx_save)&16383;
			for (int i=0; i<4; i++) fRec78_tmp[i]=fRec78_perm[i];
			for (int i=0; i<4; i++) fRec29_tmp[i]=fRec29_perm[i];
			fYec20_idx = (fYec20_idx+fYec20_idx_save)&16383;
			for (int i=0; i<4; i++) fRec27_tmp[i]=fRec27_perm[i];
			for (int i=0; i<4; i++) fRec25_tmp[i]=fRec25_perm[i];
			fYec21_idx = (fYec21_idx+fYec21_idx_save)&16383;
			for (int i=0; i<4; i++) fRec81_tmp[i]=fRec81_perm[i];
			for (int i=0; i<4; i++) fRec26_tmp[i]=fRec26_perm[i];
			fYec22_idx = (fYec22_idx+fYec22_idx_save)&16383;
			for (int i=0; i<4; i++) fRec24_tmp[i]=fRec24_perm[i];
			for (int i=0; i<4; i++) fRec22_tmp[i]=fRec22_perm[i];
			fYec23_idx = (fYec23_idx+fYec23_idx_save)&16383;
			for (int i=0; i<4; i++) fRec84_tmp[i]=fRec84_perm[i];
			for (int i=0; i<4; i++) fRec23_tmp[i]=fRec23_perm[i];
			fYec24_idx = (fYec24_idx+fYec24_idx_save)&16383;
			fYec25_idx = (fYec25_idx+fYec25_idx_save)&16383;
			for (int i=0; i<4; i++) fRec21_tmp[i]=fRec21_perm[i];
			for (int i=0; i<4; i++) fRec20_tmp[i]=fRec20_perm[i];
			for (int i=0; i<4; i++) fRec19_tmp[i]=fRec19_perm[i];
			for (int i=0; i<4; i++) fRec18_tmp[i]=fRec18_perm[i];
			for (int i=0; i<4; i++) fRec17_tmp[i]=fRec17_perm[i];
			for (int i=0; i<4; i++) fRec92_tmp[i]=fRec92_perm[i];
			for (int i=0; i<4; i++) fRec91_tmp[i]=fRec91_perm[i];
			for (int i=0; i<4; i++) fRec90_tmp[i]=fRec90_perm[i];
			for (int i=0; i<4; i++) fYec26_tmp[i]=fYec26_perm[i];
			for (int i=0; i<4; i++) fRec89_tmp[i]=fRec89_perm[i];
			for (int i=0; i<4; i++) fRec88_tmp[i]=fRec88_perm[i];
			for (int i=0; i<4; i++) fRec87_tmp[i]=fRec87_perm[i];
			for (int i=0; i<4; i++) fRec95_tmp[i]=fRec95_perm[i];
			for (int i=0; i<4; i++) fRec94_tmp[i]=fRec94_perm[i];
			for (int i=0; i<4; i++) fRec93_tmp[i]=fRec93_perm[i];
			fYec27_idx = (fYec27_idx+fYec27_idx_save)&1023;
			for (int i=0; i<4; i++) fRec14_tmp[i]=fRec14_perm[i];
			fYec28_idx = (fYec28_idx+fYec28_idx_save)&16383;
			fYec29_idx = (fYec29_idx+fYec29_idx_save)&16383;
			for (int i=0; i<4; i++) fRec101_tmp[i]=fRec101_perm[i];
			for (int i=0; i<4; i++) fRec100_tmp[i]=fRec100_perm[i];
			for (int i=0; i<4; i++) fRec99_tmp[i]=fRec99_perm[i];
			for (int i=0; i<4; i++) fRec98_tmp[i]=fRec98_perm[i];
			for (int i=0; i<4; i++) fRec97_tmp[i]=fRec97_perm[i];
			for (int i=0; i<4; i++) fRec108_tmp[i]=fRec108_perm[i];
			for (int i=0; i<4; i++) fRec107_tmp[i]=fRec107_perm[i];
			for (int i=0; i<4; i++) fRec106_tmp[i]=fRec106_perm[i];
			for (int i=0; i<4; i++) fYec30_tmp[i]=fYec30_perm[i];
			for (int i=0; i<4; i++) fRec105_tmp[i]=fRec105_perm[i];
			for (int i=0; i<4; i++) fRec104_tmp[i]=fRec104_perm[i];
			for (int i=0; i<4; i++) fRec103_tmp[i]=fRec103_perm[i];
			for (int i=0; i<4; i++) fRec111_tmp[i]=fRec111_perm[i];
			for (int i=0; i<4; i++) fRec110_tmp[i]=fRec110_perm[i];
			for (int i=0; i<4; i++) fRec109_tmp[i]=fRec109_perm[i];
			fYec31_idx = (fYec31_idx+fYec31_idx_save)&1023;
			for (int i=0; i<4; i++) fRec96_tmp[i]=fRec96_perm[i];
			fYec32_idx = (fYec32_idx+fYec32_idx_save)&16383;
			for (int i=0; i<4; i++) fRec13_tmp[i]=fRec13_perm[i];
			for (int i=0; i<4; i++) fRec11_tmp[i]=fRec11_perm[i];
			fYec33_idx = (fYec33_idx+fYec33_idx_save)&16383;
			for (int i=0; i<4; i++) fRec112_tmp[i]=fRec112_perm[i];
			for (int i=0; i<4; i++) fRec12_tmp[i]=fRec12_perm[i];
			fYec34_idx = (fYec34_idx+fYec34_idx_save)&16383;
			for (int i=0; i<4; i++) fRec10_tmp[i]=fRec10_perm[i];
			for (int i=0; i<4; i++) fRec8_tmp[i]=fRec8_perm[i];
			fYec35_idx = (fYec35_idx+fYec35_idx_save)&16383;
			for (int i=0; i<4; i++) fRec114_tmp[i]=fRec114_perm[i];
			for (int i=0; i<4; i++) fRec9_tmp[i]=fRec9_perm[i];
			fYec36_idx = (fYec36_idx+fYec36_idx_save)&16383;
			for (int i=0; i<4; i++) fRec7_tmp[i]=fRec7_perm[i];
			for (int i=0; i<4; i++) fRec5_tmp[i]=fRec5_perm[i];
			fYec37_idx = (fYec37_idx+fYec37_idx_save)&16383;
			for (int i=0; i<4; i++) fRec115_tmp[i]=fRec115_perm[i];
			for (int i=0; i<4; i++) fRec6_tmp[i]=fRec6_perm[i];
			fYec38_idx = (fYec38_idx+fYec38_idx_save)&16383;
			for (int i=0; i<4; i++) fRec4_tmp[i]=fRec4_perm[i];
			for (int i=0; i<4; i++) fRec2_tmp[i]=fRec2_perm[i];
			fYec39_idx = (fYec39_idx+fYec39_idx_save)&16383;
			for (int i=0; i<4; i++) fRec118_tmp[i]=fRec118_perm[i];
			for (int i=0; i<4; i++) fRec3_tmp[i]=fRec3_perm[i];
			for (int i=0; i<4; i++) fRec0_tmp[i]=fRec0_perm[i];
			for (int i=0; i<4; i++) fRec1_tmp[i]=fRec1_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec1[i] = ((0.760314f * fRec0[i-1]) - (0.64955574f * fRec50[i-1]));
				fZec2[i] = ((0.760314f * fRec1[i-1]) - (0.64955574f * fRec51[i-1]));
				fYec0[(fYec0_idx+i)&16383] = ((0.70710677f * fZec1[i]) - (0.70710677f * fZec2[i]));
				fRec52[i] = (fYec0[(fYec0_idx+i-int((iZec4[i] + 1)))&16383] + ((fRec52[i-1] * (0 - (fZec6[i] / fZec7[i]))) + ((fZec6[i] * fYec0[(fYec0_idx+i-iZec4[i])&16383]) / fZec7[i])));
				fRec50[i] = fRec52[i];
				fYec1[(fYec1_idx+i)&16383] = ((0.70710677f * fZec1[i]) + (0.70710677f * fZec2[i]));
				fRec54[i] = (fYec1[(fYec1_idx+i-int((iZec9[i] + 1)))&16383] + ((fRec54[i-1] * (0 - (fZec11[i] / fZec12[i]))) + ((fZec11[i] * fYec1[(fYec1_idx+i-iZec9[i])&16383]) / fZec12[i])));
				fRec51[i] = fRec54[i];
				fZec13[i] = ((0.64955574f * fRec0[i-1]) + (0.760314f * fRec50[i-1]));
				fZec14[i] = ((0.760314f * fZec13[i]) - (0.64955574f * fRec47[i-1]));
				fZec15[i] = ((0.760314f * fRec51[i-1]) + (0.64955574f * fRec1[i-1]));
				fZec16[i] = ((0.760314f * fZec15[i]) - (0.64955574f * fRec48[i-1]));
				fYec2[(fYec2_idx+i)&16383] = ((0.70710677f * fZec14[i]) - (0.70710677f * fZec16[i]));
				fRec49[i] = ((((fYec2[(fYec2_idx+i-iZec18[i])&16383] * fZec20[i]) / fZec21[i]) + fYec2[(fYec2_idx+i-int((iZec18[i] + 1)))&16383]) + (fRec49[i-1] * (0 - (fZec20[i] / fZec21[i]))));
				fRec47[i] = fRec49[i];
				fYec3[(fYec3_idx+i)&16383] = ((0.70710677f * fZec14[i]) + (0.70710677f * fZec16[i]));
				fRec57[i] = (fYec3[(fYec3_idx+i-int((iZec23[i] + 1)))&16383] + ((fRec57[i-1] * (0 - (fZec25[i] / fZec26[i]))) + ((fZec25[i] * fYec3[(fYec3_idx+i-iZec23[i])&16383]) / fZec26[i])));
				fRec48[i] = fRec57[i];
				fZec27[i] = ((0.760314f * fRec47[i-1]) + (0.64955574f * fZec13[i]));
				fZec28[i] = ((0.760314f * fZec27[i]) - (0.64955574f * fRec44[i-1]));
				fZec29[i] = ((0.760314f * fRec48[i-1]) + (0.64955574f * fZec15[i]));
				fZec30[i] = ((0.760314f * fZec29[i]) - (0.64955574f * fRec45[i-1]));
				fYec4[(fYec4_idx+i)&16383] = ((0.70710677f * fZec28[i]) - (0.70710677f * fZec30[i]));
				fRec46[i] = ((((fYec4[(fYec4_idx+i-iZec32[i])&16383] * fZec34[i]) / fZec35[i]) + fYec4[(fYec4_idx+i-int((iZec32[i] + 1)))&16383]) + (fRec46[i-1] * (0 - (fZec34[i] / fZec35[i]))));
				fRec44[i] = fRec46[i];
				fYec5[(fYec5_idx+i)&16383] = ((0.70710677f * fZec28[i]) + (0.70710677f * fZec30[i]));
				fRec60[i] = (fYec5[(fYec5_idx+i-int((iZec37[i] + 1)))&16383] + ((fRec60[i-1] * (0 - (fZec39[i] / fZec40[i]))) + ((fZec39[i] * fYec5[(fYec5_idx+i-iZec37[i])&16383]) / fZec40[i])));
				fRec45[i] = fRec60[i];
				fZec41[i] = ((0.760314f * fRec44[i-1]) + (0.64955574f * fZec27[i]));
				fZec42[i] = ((0.760314f * fZec41[i]) - (0.64955574f * fRec41[i-1]));
				fZec43[i] = ((0.760314f * fRec45[i-1]) + (0.64955574f * fZec29[i]));
				fZec44[i] = ((0.760314f * fZec43[i]) - (0.64955574f * fRec42[i-1]));
				fYec6[(fYec6_idx+i)&16383] = ((0.70710677f * fZec42[i]) - (0.70710677f * fZec44[i]));
				fRec43[i] = ((((fYec6[(fYec6_idx+i-iZec46[i])&16383] * fZec48[i]) / fZec49[i]) + fYec6[(fYec6_idx+i-int((iZec46[i] + 1)))&16383]) + (fRec43[i-1] * (0 - (fZec48[i] / fZec49[i]))));
				fRec41[i] = fRec43[i];
				fYec7[(fYec7_idx+i)&16383] = ((0.70710677f * fZec42[i]) + (0.70710677f * fZec44[i]));
				fRec63[i] = (fYec7[(fYec7_idx+i-int((iZec51[i] + 1)))&16383] + ((fRec63[i-1] * (0 - (fZec53[i] / fZec54[i]))) + ((fZec53[i] * fYec7[(fYec7_idx+i-iZec51[i])&16383]) / fZec54[i])));
				fRec42[i] = fRec63[i];
				fZec55[i] = ((0.760314f * fRec41[i-1]) + (0.64955574f * fZec41[i]));
				fZec56[i] = ((0.760314f * fZec55[i]) - (0.64955574f * fRec38[i-1]));
				fZec57[i] = ((0.760314f * fRec42[i-1]) + (0.64955574f * fZec43[i]));
				fZec58[i] = ((0.760314f * fZec57[i]) - (0.64955574f * fRec39[i-1]));
				fYec8[(fYec8_idx+i)&16383] = ((0.70710677f * fZec56[i]) - (0.70710677f * fZec58[i]));
				fRec40[i] = ((((fYec8[(fYec8_idx+i-iZec60[i])&16383] * fZec62[i]) / fZec63[i]) + fYec8[(fYec8_idx+i-int((iZec60[i] + 1)))&16383]) + (fRec40[i-1] * (0 - (fZec62[i] / fZec63[i]))));
				fRec38[i] = fRec40[i];
				fYec9[(fYec9_idx+i)&16383] = ((0.70710677f * fZec56[i]) + (0.70710677f * fZec58[i]));
				fRec66[i] = (fYec9[(fYec9_idx+i-int((iZec65[i] + 1)))&16383] + ((fRec66[i-1] * (0 - (fZec67[i] / fZec68[i]))) + ((fZec67[i] * fYec9[(fYec9_idx+i-iZec65[i])&16383]) / fZec68[i])));
				fRec39[i] = fRec66[i];
				fYec10[(fYec10_idx+i)&1023] = ((0.760314f * fRec38[i-1]) + (0.64955574f * fZec55[i]));
				fYec11[(fYec11_idx+i)&16383] = (((((fYec10[(fYec10_idx+i-iZec72[i])&1023] * fZec75[i]) * fZec77[i]) * fZec79[i]) * fZec81[i]) + (fZec82[i] * ((((((fYec10[(fYec10_idx+i-iZec83[i])&1023] * fZec84[i]) * fZec85[i]) * fZec86[i]) + (0.5f * (((fZec74[i] * fYec10[(fYec10_idx+i-iZec87[i])&1023]) * fZec88[i]) * fZec89[i]))) + (0.16666667f * ((fZec90[i] * fYec10[(fYec10_idx+i-iZec91[i])&1023]) * fZec92[i]))) + (0.041666668f * (fZec93[i] * fYec10[(fYec10_idx+i-iZec94[i])&1023])))));
				fRec37[i] = ((((fYec11[(fYec11_idx+i-iZec96[i])&16383] * fZec98[i]) / fZec99[i]) + fYec11[(fYec11_idx+i-int((iZec96[i] + 1)))&16383]) + (fRec37[i-1] * (0 - (fZec98[i] / fZec99[i]))));
				fYec12[(fYec12_idx+i)&1023] = ((0.760314f * fRec39[i-1]) + (0.64955574f * fZec57[i]));
				fYec13[(fYec13_idx+i)&16383] = (((((fYec12[(fYec12_idx+i-int(min(512, max(0, iZec102[i]))))&1023] * (0 - fZec104[i])) * (0 - (0.5f * fZec105[i]))) * (0 - (0.33333334f * fZec106[i]))) * (0 - (0.25f * fZec107[i]))) + ((fSlow28 + (fZec100[i] + (5.0f - fZec103[i]))) * ((((((fYec12[(fYec12_idx+i-int(min(512, max(0, (iZec102[i] + 1)))))&1023] * (0 - fZec105[i])) * (0 - (0.5f * fZec106[i]))) * (0 - (0.33333334f * fZec107[i]))) + (0.5f * (((fZec104[i] * fYec12[(fYec12_idx+i-int(min(512, max(0, (iZec102[i] + 2)))))&1023]) * (0 - fZec106[i])) * (0 - (0.5f * fZec107[i]))))) + (0.16666667f * ((fZec108[i] * fYec12[(fYec12_idx+i-int(min(512, max(0, (iZec102[i] + 3)))))&1023]) * (0 - fZec107[i])))) + (0.041666668f * ((fZec108[i] * fZec106[i]) * fYec12[(fYec12_idx+i-int(min(512, max(0, (iZec102[i] + 4)))))&1023])))));
				fRec69[i] = (fYec13[(fYec13_idx+i-int((iZec110[i] + 1)))&16383] + ((fRec69[i-1] * (0 - (fZec112[i] / fZec113[i]))) + ((fZec112[i] * fYec13[(fYec13_idx+i-iZec110[i])&16383]) / fZec113[i])));
				fZec114[i] = ((0.760314f * fRec37[i]) - (0.64955574f * fRec34[i-1]));
				fZec115[i] = ((0.760314f * fRec69[i]) - (0.64955574f * fRec35[i-1]));
				fYec14[(fYec14_idx+i)&16383] = ((0.70710677f * fZec114[i]) - (0.70710677f * fZec115[i]));
				fRec36[i] = ((((fYec14[(fYec14_idx+i-iZec117[i])&16383] * fZec119[i]) / fZec120[i]) + fYec14[(fYec14_idx+i-int((iZec117[i] + 1)))&16383]) + (fRec36[i-1] * (0 - (fZec119[i] / fZec120[i]))));
				fRec34[i] = fRec36[i];
				fYec15[(fYec15_idx+i)&16383] = ((0.70710677f * fZec114[i]) + (0.70710677f * fZec115[i]));
				fRec72[i] = (fYec15[(fYec15_idx+i-int((iZec122[i] + 1)))&16383] + ((fRec72[i-1] * (0 - (fZec124[i] / fZec125[i]))) + ((fZec124[i] * fYec15[(fYec15_idx+i-iZec122[i])&16383]) / fZec125[i])));
				fRec35[i] = fRec72[i];
				fZec126[i] = ((0.760314f * fRec34[i-1]) + (0.64955574f * fRec37[i]));
				fZec127[i] = ((0.760314f * fZec126[i]) - (0.64955574f * fRec31[i-1]));
				fZec128[i] = ((0.760314f * fRec35[i-1]) + (0.64955574f * fRec69[i]));
				fZec129[i] = ((0.760314f * fZec128[i]) - (0.64955574f * fRec32[i-1]));
				fYec16[(fYec16_idx+i)&16383] = ((0.70710677f * fZec127[i]) - (0.70710677f * fZec129[i]));
				fRec33[i] = ((((fYec16[(fYec16_idx+i-iZec131[i])&16383] * fZec133[i]) / fZec134[i]) + fYec16[(fYec16_idx+i-int((iZec131[i] + 1)))&16383]) + (fRec33[i-1] * (0 - (fZec133[i] / fZec134[i]))));
				fRec31[i] = fRec33[i];
				fYec17[(fYec17_idx+i)&16383] = ((0.70710677f * fZec127[i]) + (0.70710677f * fZec129[i]));
				fRec75[i] = (fYec17[(fYec17_idx+i-iZec137[i])&16383] + ((fRec75[i-1] * fZec141[i]) + ((fYec17[(fYec17_idx+i-iZec136[i])&16383] * fZec139[i]) / fZec140[i])));
				fRec32[i] = fRec75[i];
				fZec142[i] = ((0.760314f * fRec31[i-1]) + (0.64955574f * fZec126[i]));
				fZec143[i] = ((0.760314f * fZec142[i]) - (0.64955574f * fRec28[i-1]));
				fZec144[i] = ((0.760314f * fRec32[i-1]) + (0.64955574f * fZec128[i]));
				fZec145[i] = ((0.760314f * fZec144[i]) - (0.64955574f * fRec29[i-1]));
				fYec18[(fYec18_idx+i)&16383] = ((0.70710677f * fZec143[i]) - (0.70710677f * fZec145[i]));
				fRec30[i] = ((((fYec18[(fYec18_idx+i-iZec147[i])&16383] * fZec149[i]) / fZec150[i]) + fYec18[(fYec18_idx+i-int((iZec147[i] + 1)))&16383]) + (fRec30[i-1] * (0 - (fZec149[i] / fZec150[i]))));
				fRec28[i] = fRec30[i];
				fYec19[(fYec19_idx+i)&16383] = ((0.70710677f * fZec143[i]) + (0.70710677f * fZec145[i]));
				fRec78[i] = (fYec19[(fYec19_idx+i-iZec153[i])&16383] + ((fRec78[i-1] * fZec157[i]) + ((fZec155[i] * fYec19[(fYec19_idx+i-iZec152[i])&16383]) / fZec156[i])));
				fRec29[i] = fRec78[i];
				fZec158[i] = ((0.760314f * fRec28[i-1]) + (0.64955574f * fZec142[i]));
				fZec159[i] = ((0.760314f * fZec158[i]) - (0.64955574f * fRec25[i-1]));
				fZec160[i] = ((0.760314f * fRec29[i-1]) + (0.64955574f * fZec144[i]));
				fZec161[i] = ((0.760314f * fZec160[i]) - (0.64955574f * fRec26[i-1]));
				fYec20[(fYec20_idx+i)&16383] = ((0.70710677f * fZec159[i]) - (0.70710677f * fZec161[i]));
				fRec27[i] = ((((fYec20[(fYec20_idx+i-iZec163[i])&16383] * fZec165[i]) / fZec166[i]) + fYec20[(fYec20_idx+i-iZec167[i])&16383]) + (fZec168[i] * fRec27[i-1]));
				fRec25[i] = fRec27[i];
				fYec21[(fYec21_idx+i)&16383] = ((0.70710677f * fZec159[i]) + (0.70710677f * fZec161[i]));
				fRec81[i] = (fYec21[(fYec21_idx+i-iZec171[i])&16383] + ((fRec81[i-1] * fZec175[i]) + ((fYec21[(fYec21_idx+i-iZec170[i])&16383] * fZec173[i]) / fZec174[i])));
				fRec26[i] = fRec81[i];
				fZec176[i] = ((0.760314f * fRec25[i-1]) + (0.64955574f * fZec158[i]));
				fZec177[i] = ((0.760314f * fZec176[i]) - (0.64955574f * fRec22[i-1]));
				fZec178[i] = ((0.760314f * fRec26[i-1]) + (0.64955574f * fZec160[i]));
				fZec179[i] = ((0.760314f * fZec178[i]) - (0.64955574f * fRec23[i-1]));
				fYec22[(fYec22_idx+i)&16383] = ((0.70710677f * fZec177[i]) - (0.70710677f * fZec179[i]));
				fRec24[i] = ((((fYec22[(fYec22_idx+i-iZec181[i])&16383] * fZec183[i]) / fZec184[i]) + fYec22[(fYec22_idx+i-int((iZec181[i] + 1)))&16383]) + (fRec24[i-1] * (0 - (fZec183[i] / fZec184[i]))));
				fRec22[i] = fRec24[i];
				fYec23[(fYec23_idx+i)&16383] = ((0.70710677f * fZec177[i]) + (0.70710677f * fZec179[i]));
				fRec84[i] = (fYec23[(fYec23_idx+i-iZec187[i])&16383] + ((fRec84[i-1] * fZec191[i]) + ((fZec189[i] * fYec23[(fYec23_idx+i-iZec186[i])&16383]) / fZec190[i])));
				fRec23[i] = fRec84[i];
				fYec24[(fYec24_idx+i)&16383] = ((0.760314f * fRec22[i-1]) + (0.64955574f * fZec176[i]));
				fYec25[(fYec25_idx+i)&16383] = (((((fYec24[(fYec24_idx+i-int(min(8192, iZec195[i])))&16383] * fZec198[i]) * fZec200[i]) * fZec202[i]) * fZec204[i]) + (fZec205[i] * ((((((fYec24[(fYec24_idx+i-int(min(8192, iZec206[i])))&16383] * fZec207[i]) * fZec208[i]) * fZec209[i]) + (0.5f * (((fZec197[i] * fYec24[(fYec24_idx+i-int(min(8192, iZec210[i])))&16383]) * fZec211[i]) * fZec212[i]))) + (0.16666667f * ((fZec213[i] * fYec24[(fYec24_idx+i-int(min(8192, iZec214[i])))&16383]) * fZec215[i]))) + (0.041666668f * (fZec216[i] * fYec24[(fYec24_idx+i-int(min(8192, iZec217[i])))&16383])))));
				fRec21[i] = ((((fYec25[(fYec25_idx+i-iZec219[i])&16383] * fZec221[i]) / fZec222[i]) + fYec25[(fYec25_idx+i-int((iZec219[i] + 1)))&16383]) + (fRec21[i-1] * (0 - (fZec221[i] / fZec222[i]))));
				fRec20[i] = ((fSlow57 * ((fSlow55 * fRec21[i]) + (fSlow58 * fRec21[i-1]))) + (fSlow59 * fRec20[i-1]));
				fRec19[i] = (fRec20[i] - (fSlow60 * ((fSlow62 * fRec19[i-1]) + (fSlow63 * fRec19[i-2]))));
				fRec18[i] = ((fSlow60 * (((fSlow61 * fRec19[i]) + (fSlow64 * fRec19[i-1])) + (fSlow61 * fRec19[i-2]))) - (fSlow66 * ((fSlow62 * fRec18[i-1]) + (fSlow67 * fRec18[i-2]))));
				fZec223[i] = (fSlow73 * fRec17[i-1]);
				fRec17[i] = ((fSlow66 * (((fSlow61 * fRec18[i]) + (fSlow64 * fRec18[i-1])) + (fSlow61 * fRec18[i-2]))) - (fSlow70 * ((fSlow71 * fRec17[i-2]) + fZec223[i])));
				fRec92[i] = ((fSlow59 * fRec92[i-1]) + (fSlow57 * (fRec21[i] + fRec21[i-1])));
				fRec91[i] = (fRec92[i] - (fSlow60 * ((fSlow62 * fRec91[i-1]) + (fSlow63 * fRec91[i-2]))));
				fRec90[i] = ((fSlow60 * (fRec91[i-2] + (fRec91[i] + (2.0f * fRec91[i-1])))) - (fSlow66 * ((fSlow62 * fRec90[i-1]) + (fSlow67 * fRec90[i-2]))));
				fZec224[i] = (fRec90[i-2] + (fRec90[i] + (2.0f * fRec90[i-1])));
				fYec26[i] = (fSlow66 * fZec224[i]);
				fRec89[i] = ((fSlow75 * fRec89[i-1]) + (fSlow76 * ((fSlow77 * fYec26[i-1]) + (fSlow78 * fZec224[i]))));
				fRec88[i] = (fRec89[i] - (fSlow79 * ((fSlow73 * fRec88[i-1]) + (fSlow80 * fRec88[i-2]))));
				fRec87[i] = ((fSlow79 * (((fSlow72 * fRec88[i]) + (fSlow81 * fRec88[i-1])) + (fSlow72 * fRec88[i-2]))) - (fSlow82 * ((fSlow73 * fRec87[i-1]) + (fSlow83 * fRec87[i-2]))));
				fRec95[i] = ((fSlow75 * fRec95[i-1]) + (fSlow76 * (fYec26[i] + fYec26[i-1])));
				fRec94[i] = (fRec95[i] - (fSlow79 * ((fSlow73 * fRec94[i-1]) + (fSlow80 * fRec94[i-2]))));
				fRec93[i] = ((fSlow79 * (fRec94[i-2] + (fRec94[i] + (2.0f * fRec94[i-1])))) - (fSlow82 * ((fSlow73 * fRec93[i-1]) + (fSlow83 * fRec93[i-2]))));
				fYec27[(fYec27_idx+i)&1023] = ((float)input0[i] + (fSlow86 * ((fSlow87 * (fRec17[i-2] + (fSlow70 * (fZec223[i] + (fSlow71 * fRec17[i]))))) + (fSlow82 * ((fSlow88 * (((fSlow72 * fRec87[i]) + (fSlow81 * fRec87[i-1])) + (fSlow72 * fRec87[i-2]))) + (fSlow89 * (fRec93[i-2] + (fRec93[i] + (2.0f * fRec93[i-1])))))))));
				fRec14[i] = ((fSlow85 * (((((fZec198[i] * fZec200[i]) * fZec202[i]) * fZec204[i]) * fYec27[(fYec27_idx+i-int(min(512, iZec195[i])))&1023]) + (fZec205[i] * ((((0.5f * (((fZec197[i] * fZec211[i]) * fZec212[i]) * fYec27[(fYec27_idx+i-int(min(512, iZec210[i])))&1023])) + (((fZec207[i] * fZec208[i]) * fZec209[i]) * fYec27[(fYec27_idx+i-int(min(512, iZec206[i])))&1023])) + (0.16666667f * ((fZec213[i] * fZec215[i]) * fYec27[(fYec27_idx+i-int(min(512, iZec214[i])))&1023]))) + (0.041666668f * (fZec216[i] * fYec27[(fYec27_idx+i-int(min(512, iZec217[i])))&1023])))))) + (fSlow84 * fRec14[i-1]));
				fYec28[(fYec28_idx+i)&16383] = ((0.760314f * fRec23[i-1]) + (0.64955574f * fZec178[i]));
				fYec29[(fYec29_idx+i)&16383] = (((((fYec28[(fYec28_idx+i-int(min(8192, max(0, iZec227[i]))))&16383] * (0 - fZec229[i])) * (0 - (0.5f * fZec230[i]))) * (0 - (0.33333334f * fZec231[i]))) * (0 - (0.25f * fZec232[i]))) + ((fSlow28 + (fZec225[i] + (5.0f - fZec228[i]))) * ((((((fYec28[(fYec28_idx+i-int(min(8192, max(0, (iZec227[i] + 1)))))&16383] * (0 - fZec230[i])) * (0 - (0.5f * fZec231[i]))) * (0 - (0.33333334f * fZec232[i]))) + (0.5f * (((fZec229[i] * fYec28[(fYec28_idx+i-int(min(8192, max(0, (iZec227[i] + 2)))))&16383]) * (0 - fZec231[i])) * (0 - (0.5f * fZec232[i]))))) + (0.16666667f * ((fZec233[i] * fYec28[(fYec28_idx+i-int(min(8192, max(0, (iZec227[i] + 3)))))&16383]) * (0 - fZec232[i])))) + (0.041666668f * ((fZec233[i] * fZec231[i]) * fYec28[(fYec28_idx+i-int(min(8192, max(0, (iZec227[i] + 4)))))&16383])))));
				fRec101[i] = (fYec29[(fYec29_idx+i-int((iZec235[i] + 1)))&16383] + ((fRec101[i-1] * (0 - (fZec237[i] / fZec238[i]))) + ((fZec237[i] * fYec29[(fYec29_idx+i-iZec235[i])&16383]) / fZec238[i])));
				fRec100[i] = ((fSlow59 * fRec100[i-1]) + (fSlow57 * ((fSlow55 * fRec101[i]) + (fSlow58 * fRec101[i-1]))));
				fRec99[i] = (fRec100[i] - (fSlow60 * ((fSlow62 * fRec99[i-1]) + (fSlow63 * fRec99[i-2]))));
				fRec98[i] = ((fSlow60 * (((fSlow61 * fRec99[i]) + (fSlow64 * fRec99[i-1])) + (fSlow61 * fRec99[i-2]))) - (fSlow66 * ((fSlow62 * fRec98[i-1]) + (fSlow67 * fRec98[i-2]))));
				fZec239[i] = (fSlow73 * fRec97[i-1]);
				fRec97[i] = ((fSlow66 * (((fSlow61 * fRec98[i]) + (fSlow64 * fRec98[i-1])) + (fSlow61 * fRec98[i-2]))) - (fSlow70 * (fZec239[i] + (fSlow71 * fRec97[i-2]))));
				fRec108[i] = ((fSlow59 * fRec108[i-1]) + (fSlow57 * (fRec101[i] + fRec101[i-1])));
				fRec107[i] = (fRec108[i] - (fSlow60 * ((fSlow62 * fRec107[i-1]) + (fSlow63 * fRec107[i-2]))));
				fRec106[i] = ((fSlow60 * (fRec107[i-2] + (fRec107[i] + (2.0f * fRec107[i-1])))) - (fSlow66 * ((fSlow62 * fRec106[i-1]) + (fSlow67 * fRec106[i-2]))));
				fZec240[i] = (fRec106[i-2] + (fRec106[i] + (2.0f * fRec106[i-1])));
				fYec30[i] = (fSlow66 * fZec240[i]);
				fRec105[i] = ((fSlow75 * fRec105[i-1]) + (fSlow76 * ((fSlow78 * fZec240[i]) + (fSlow77 * fYec30[i-1]))));
				fRec104[i] = (fRec105[i] - (fSlow79 * ((fSlow73 * fRec104[i-1]) + (fSlow80 * fRec104[i-2]))));
				fRec103[i] = ((fSlow79 * (((fSlow72 * fRec104[i]) + (fSlow81 * fRec104[i-1])) + (fSlow72 * fRec104[i-2]))) - (fSlow82 * ((fSlow73 * fRec103[i-1]) + (fSlow83 * fRec103[i-2]))));
				fRec111[i] = ((fSlow75 * fRec111[i-1]) + (fSlow76 * (fYec30[i-1] + fYec30[i])));
				fRec110[i] = (fRec111[i] - (fSlow79 * ((fSlow73 * fRec110[i-1]) + (fSlow80 * fRec110[i-2]))));
				fRec109[i] = ((fSlow79 * (fRec110[i-2] + (fRec110[i] + (2.0f * fRec110[i-1])))) - (fSlow82 * ((fSlow73 * fRec109[i-1]) + (fSlow83 * fRec109[i-2]))));
				fYec31[(fYec31_idx+i)&1023] = ((float)input1[i] + (fSlow86 * ((fSlow87 * (fRec97[i-2] + (fSlow70 * (fZec239[i] + (fSlow71 * fRec97[i]))))) + (fSlow82 * ((fSlow88 * (((fSlow72 * fRec103[i]) + (fSlow81 * fRec103[i-1])) + (fSlow72 * fRec103[i-2]))) + (fSlow89 * (fRec109[i-2] + (fRec109[i] + (2.0f * fRec109[i-1])))))))));
				fRec96[i] = ((fSlow84 * fRec96[i-1]) + (fSlow85 * (((((fZec75[i] * fZec77[i]) * fZec79[i]) * fZec81[i]) * fYec31[(fYec31_idx+i-iZec72[i])&1023]) + (fZec82[i] * ((((((fZec84[i] * fZec85[i]) * fZec86[i]) * fYec31[(fYec31_idx+i-iZec83[i])&1023]) + (0.5f * (((fZec74[i] * fZec88[i]) * fZec89[i]) * fYec31[(fYec31_idx+i-iZec87[i])&1023]))) + (0.16666667f * ((fZec90[i] * fZec92[i]) * fYec31[(fYec31_idx+i-iZec91[i])&1023]))) + (0.041666668f * (fZec93[i] * fYec31[(fYec31_idx+i-iZec94[i])&1023])))))));
				fZec241[i] = ((fSlow93 * fRec14[i]) + (fSlow95 * fRec11[i-1]));
				fZec242[i] = ((fSlow95 * fRec12[i-1]) + (fSlow93 * fRec96[i]));
				fYec32[(fYec32_idx+i)&16383] = ((0.70710677f * fZec241[i]) - (0.70710677f * fZec242[i]));
				fRec13[i] = ((((fYec32[(fYec32_idx+i-iZec136[i])&16383] * fZec139[i]) / fZec140[i]) + fYec32[(fYec32_idx+i-iZec137[i])&16383]) + (fRec13[i-1] * fZec141[i]));
				fRec11[i] = fRec13[i];
				fYec33[(fYec33_idx+i)&16383] = ((0.70710677f * fZec241[i]) + (0.70710677f * fZec242[i]));
				fRec112[i] = (fYec33[(fYec33_idx+i-int((iZec244[i] + 1)))&16383] + ((fRec112[i-1] * (0 - (fZec246[i] / fZec247[i]))) + ((fZec246[i] * fYec33[(fYec33_idx+i-iZec244[i])&16383]) / fZec247[i])));
				fRec12[i] = fRec112[i];
				fZec248[i] = ((fSlow93 * fRec11[i-1]) + (fSlow94 * fRec14[i]));
				fZec249[i] = ((fSlow93 * fZec248[i]) + (fSlow95 * fRec8[i-1]));
				fZec250[i] = ((fSlow93 * fRec12[i-1]) + (fSlow94 * fRec96[i]));
				fZec251[i] = ((fSlow95 * fRec9[i-1]) + (fSlow93 * fZec250[i]));
				fYec34[(fYec34_idx+i)&16383] = ((0.70710677f * fZec249[i]) - (0.70710677f * fZec251[i]));
				fRec10[i] = ((((fYec34[(fYec34_idx+i-iZec163[i])&16383] * fZec165[i]) / fZec166[i]) + fYec34[(fYec34_idx+i-iZec167[i])&16383]) + (fRec10[i-1] * fZec168[i]));
				fRec8[i] = fRec10[i];
				fYec35[(fYec35_idx+i)&16383] = ((0.70710677f * fZec249[i]) + (0.70710677f * fZec251[i]));
				fRec114[i] = (fYec35[(fYec35_idx+i-iZec153[i])&16383] + ((fZec157[i] * fRec114[i-1]) + ((fZec155[i] * fYec35[(fYec35_idx+i-iZec152[i])&16383]) / fZec156[i])));
				fRec9[i] = fRec114[i];
				fZec252[i] = ((fSlow93 * fRec8[i-1]) + (fSlow94 * fZec248[i]));
				fZec253[i] = ((fSlow93 * fZec252[i]) + (fSlow95 * fRec5[i-1]));
				fZec254[i] = ((fSlow93 * fRec9[i-1]) + (fSlow94 * fZec250[i]));
				fZec255[i] = ((fSlow95 * fRec6[i-1]) + (fSlow93 * fZec254[i]));
				fYec36[(fYec36_idx+i)&16383] = ((0.70710677f * fZec253[i]) - (0.70710677f * fZec255[i]));
				fRec7[i] = ((((fYec36[(fYec36_idx+i-iZec170[i])&16383] * fZec173[i]) / fZec174[i]) + fYec36[(fYec36_idx+i-iZec171[i])&16383]) + (fRec7[i-1] * fZec175[i]));
				fRec5[i] = fRec7[i];
				fYec37[(fYec37_idx+i)&16383] = ((0.70710677f * fZec253[i]) + (0.70710677f * fZec255[i]));
				fRec115[i] = (fYec37[(fYec37_idx+i-int((iZec257[i] + 1)))&16383] + ((fRec115[i-1] * (0 - (fZec259[i] / fZec260[i]))) + ((fZec259[i] * fYec37[(fYec37_idx+i-iZec257[i])&16383]) / fZec260[i])));
				fRec6[i] = fRec115[i];
				fZec261[i] = ((fSlow93 * fRec5[i-1]) + (fSlow94 * fZec252[i]));
				fZec262[i] = ((fSlow93 * fZec261[i]) + (fSlow95 * fRec2[i-1]));
				fZec263[i] = ((fSlow93 * fRec6[i-1]) + (fSlow94 * fZec254[i]));
				fZec264[i] = ((fSlow95 * fRec3[i-1]) + (fSlow93 * fZec263[i]));
				fYec38[(fYec38_idx+i)&16383] = ((0.70710677f * fZec262[i]) - (0.70710677f * fZec264[i]));
				fRec4[i] = ((((fYec38[(fYec38_idx+i-iZec266[i])&16383] * fZec268[i]) / fZec269[i]) + fYec38[(fYec38_idx+i-int((iZec266[i] + 1)))&16383]) + (fRec4[i-1] * (0 - (fZec268[i] / fZec269[i]))));
				fRec2[i] = fRec4[i];
				fYec39[(fYec39_idx+i)&16383] = ((0.70710677f * fZec262[i]) + (0.70710677f * fZec264[i]));
				fRec118[i] = (fYec39[(fYec39_idx+i-iZec187[i])&16383] + ((fZec191[i] * fRec118[i-1]) + ((fZec189[i] * fYec39[(fYec39_idx+i-iZec186[i])&16383]) / fZec190[i])));
				fRec3[i] = fRec118[i];
				fRec0[i] = ((fSlow93 * fRec2[i-1]) + (fSlow94 * fZec261[i]));
				fRec1[i] = ((fSlow93 * fRec3[i-1]) + (fSlow94 * fZec263[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec1_perm[i]=fRec1_tmp[count+i];
			for (int i=0; i<4; i++) fRec0_perm[i]=fRec0_tmp[count+i];
			for (int i=0; i<4; i++) fRec3_perm[i]=fRec3_tmp[count+i];
			for (int i=0; i<4; i++) fRec118_perm[i]=fRec118_tmp[count+i];
			fYec39_idx_save = count;
			for (int i=0; i<4; i++) fRec2_perm[i]=fRec2_tmp[count+i];
			for (int i=0; i<4; i++) fRec4_perm[i]=fRec4_tmp[count+i];
			fYec38_idx_save = count;
			for (int i=0; i<4; i++) fRec6_perm[i]=fRec6_tmp[count+i];
			for (int i=0; i<4; i++) fRec115_perm[i]=fRec115_tmp[count+i];
			fYec37_idx_save = count;
			for (int i=0; i<4; i++) fRec5_perm[i]=fRec5_tmp[count+i];
			for (int i=0; i<4; i++) fRec7_perm[i]=fRec7_tmp[count+i];
			fYec36_idx_save = count;
			for (int i=0; i<4; i++) fRec9_perm[i]=fRec9_tmp[count+i];
			for (int i=0; i<4; i++) fRec114_perm[i]=fRec114_tmp[count+i];
			fYec35_idx_save = count;
			for (int i=0; i<4; i++) fRec8_perm[i]=fRec8_tmp[count+i];
			for (int i=0; i<4; i++) fRec10_perm[i]=fRec10_tmp[count+i];
			fYec34_idx_save = count;
			for (int i=0; i<4; i++) fRec12_perm[i]=fRec12_tmp[count+i];
			for (int i=0; i<4; i++) fRec112_perm[i]=fRec112_tmp[count+i];
			fYec33_idx_save = count;
			for (int i=0; i<4; i++) fRec11_perm[i]=fRec11_tmp[count+i];
			for (int i=0; i<4; i++) fRec13_perm[i]=fRec13_tmp[count+i];
			fYec32_idx_save = count;
			for (int i=0; i<4; i++) fRec96_perm[i]=fRec96_tmp[count+i];
			fYec31_idx_save = count;
			for (int i=0; i<4; i++) fRec109_perm[i]=fRec109_tmp[count+i];
			for (int i=0; i<4; i++) fRec110_perm[i]=fRec110_tmp[count+i];
			for (int i=0; i<4; i++) fRec111_perm[i]=fRec111_tmp[count+i];
			for (int i=0; i<4; i++) fRec103_perm[i]=fRec103_tmp[count+i];
			for (int i=0; i<4; i++) fRec104_perm[i]=fRec104_tmp[count+i];
			for (int i=0; i<4; i++) fRec105_perm[i]=fRec105_tmp[count+i];
			for (int i=0; i<4; i++) fYec30_perm[i]=fYec30_tmp[count+i];
			for (int i=0; i<4; i++) fRec106_perm[i]=fRec106_tmp[count+i];
			for (int i=0; i<4; i++) fRec107_perm[i]=fRec107_tmp[count+i];
			for (int i=0; i<4; i++) fRec108_perm[i]=fRec108_tmp[count+i];
			for (int i=0; i<4; i++) fRec97_perm[i]=fRec97_tmp[count+i];
			for (int i=0; i<4; i++) fRec98_perm[i]=fRec98_tmp[count+i];
			for (int i=0; i<4; i++) fRec99_perm[i]=fRec99_tmp[count+i];
			for (int i=0; i<4; i++) fRec100_perm[i]=fRec100_tmp[count+i];
			for (int i=0; i<4; i++) fRec101_perm[i]=fRec101_tmp[count+i];
			fYec29_idx_save = count;
			fYec28_idx_save = count;
			for (int i=0; i<4; i++) fRec14_perm[i]=fRec14_tmp[count+i];
			fYec27_idx_save = count;
			for (int i=0; i<4; i++) fRec93_perm[i]=fRec93_tmp[count+i];
			for (int i=0; i<4; i++) fRec94_perm[i]=fRec94_tmp[count+i];
			for (int i=0; i<4; i++) fRec95_perm[i]=fRec95_tmp[count+i];
			for (int i=0; i<4; i++) fRec87_perm[i]=fRec87_tmp[count+i];
			for (int i=0; i<4; i++) fRec88_perm[i]=fRec88_tmp[count+i];
			for (int i=0; i<4; i++) fRec89_perm[i]=fRec89_tmp[count+i];
			for (int i=0; i<4; i++) fYec26_perm[i]=fYec26_tmp[count+i];
			for (int i=0; i<4; i++) fRec90_perm[i]=fRec90_tmp[count+i];
			for (int i=0; i<4; i++) fRec91_perm[i]=fRec91_tmp[count+i];
			for (int i=0; i<4; i++) fRec92_perm[i]=fRec92_tmp[count+i];
			for (int i=0; i<4; i++) fRec17_perm[i]=fRec17_tmp[count+i];
			for (int i=0; i<4; i++) fRec18_perm[i]=fRec18_tmp[count+i];
			for (int i=0; i<4; i++) fRec19_perm[i]=fRec19_tmp[count+i];
			for (int i=0; i<4; i++) fRec20_perm[i]=fRec20_tmp[count+i];
			for (int i=0; i<4; i++) fRec21_perm[i]=fRec21_tmp[count+i];
			fYec25_idx_save = count;
			fYec24_idx_save = count;
			for (int i=0; i<4; i++) fRec23_perm[i]=fRec23_tmp[count+i];
			for (int i=0; i<4; i++) fRec84_perm[i]=fRec84_tmp[count+i];
			fYec23_idx_save = count;
			for (int i=0; i<4; i++) fRec22_perm[i]=fRec22_tmp[count+i];
			for (int i=0; i<4; i++) fRec24_perm[i]=fRec24_tmp[count+i];
			fYec22_idx_save = count;
			for (int i=0; i<4; i++) fRec26_perm[i]=fRec26_tmp[count+i];
			for (int i=0; i<4; i++) fRec81_perm[i]=fRec81_tmp[count+i];
			fYec21_idx_save = count;
			for (int i=0; i<4; i++) fRec25_perm[i]=fRec25_tmp[count+i];
			for (int i=0; i<4; i++) fRec27_perm[i]=fRec27_tmp[count+i];
			fYec20_idx_save = count;
			for (int i=0; i<4; i++) fRec29_perm[i]=fRec29_tmp[count+i];
			for (int i=0; i<4; i++) fRec78_perm[i]=fRec78_tmp[count+i];
			fYec19_idx_save = count;
			for (int i=0; i<4; i++) fRec28_perm[i]=fRec28_tmp[count+i];
			for (int i=0; i<4; i++) fRec30_perm[i]=fRec30_tmp[count+i];
			fYec18_idx_save = count;
			for (int i=0; i<4; i++) fRec32_perm[i]=fRec32_tmp[count+i];
			for (int i=0; i<4; i++) fRec75_perm[i]=fRec75_tmp[count+i];
			fYec17_idx_save = count;
			for (int i=0; i<4; i++) fRec31_perm[i]=fRec31_tmp[count+i];
			for (int i=0; i<4; i++) fRec33_perm[i]=fRec33_tmp[count+i];
			fYec16_idx_save = count;
			for (int i=0; i<4; i++) fRec35_perm[i]=fRec35_tmp[count+i];
			for (int i=0; i<4; i++) fRec72_perm[i]=fRec72_tmp[count+i];
			fYec15_idx_save = count;
			for (int i=0; i<4; i++) fRec34_perm[i]=fRec34_tmp[count+i];
			for (int i=0; i<4; i++) fRec36_perm[i]=fRec36_tmp[count+i];
			fYec14_idx_save = count;
			for (int i=0; i<4; i++) fRec69_perm[i]=fRec69_tmp[count+i];
			fYec13_idx_save = count;
			fYec12_idx_save = count;
			for (int i=0; i<4; i++) fRec37_perm[i]=fRec37_tmp[count+i];
			fYec11_idx_save = count;
			fYec10_idx_save = count;
			for (int i=0; i<4; i++) fRec39_perm[i]=fRec39_tmp[count+i];
			for (int i=0; i<4; i++) fRec66_perm[i]=fRec66_tmp[count+i];
			fYec9_idx_save = count;
			for (int i=0; i<4; i++) fRec38_perm[i]=fRec38_tmp[count+i];
			for (int i=0; i<4; i++) fRec40_perm[i]=fRec40_tmp[count+i];
			fYec8_idx_save = count;
			for (int i=0; i<4; i++) fRec42_perm[i]=fRec42_tmp[count+i];
			for (int i=0; i<4; i++) fRec63_perm[i]=fRec63_tmp[count+i];
			fYec7_idx_save = count;
			for (int i=0; i<4; i++) fRec41_perm[i]=fRec41_tmp[count+i];
			for (int i=0; i<4; i++) fRec43_perm[i]=fRec43_tmp[count+i];
			fYec6_idx_save = count;
			for (int i=0; i<4; i++) fRec45_perm[i]=fRec45_tmp[count+i];
			for (int i=0; i<4; i++) fRec60_perm[i]=fRec60_tmp[count+i];
			fYec5_idx_save = count;
			for (int i=0; i<4; i++) fRec44_perm[i]=fRec44_tmp[count+i];
			for (int i=0; i<4; i++) fRec46_perm[i]=fRec46_tmp[count+i];
			fYec4_idx_save = count;
			for (int i=0; i<4; i++) fRec48_perm[i]=fRec48_tmp[count+i];
			for (int i=0; i<4; i++) fRec57_perm[i]=fRec57_tmp[count+i];
			fYec3_idx_save = count;
			for (int i=0; i<4; i++) fRec47_perm[i]=fRec47_tmp[count+i];
			for (int i=0; i<4; i++) fRec49_perm[i]=fRec49_tmp[count+i];
			fYec2_idx_save = count;
			for (int i=0; i<4; i++) fRec51_perm[i]=fRec51_tmp[count+i];
			for (int i=0; i<4; i++) fRec54_perm[i]=fRec54_tmp[count+i];
			fYec1_idx_save = count;
			for (int i=0; i<4; i++) fRec50_perm[i]=fRec50_tmp[count+i];
			for (int i=0; i<4; i++) fRec52_perm[i]=fRec52_tmp[count+i];
			fYec0_idx_save = count;
			
			// SECTION : 11
			// LOOP 0x7fc571706dc0
			// exec code
			for (int i=0; i<count; i++) {
				output0[i] = (FAUSTFLOAT)fRec0[i];
			}
			
			// LOOP 0x7fc571a19e90
			// exec code
			for (int i=0; i<count; i++) {
				output1[i] = (FAUSTFLOAT)fRec1[i];
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
