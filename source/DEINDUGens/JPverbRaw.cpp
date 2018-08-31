//----------------------------------------------------------
// copyright: "(c) Julian Parker 2013"
// name: "JPverbRaw"
// version: "1.1"
// author: "Julian Parker, bug fixes by Till Bovermann"
// license: "GPL2+"
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
	FAUSTFLOAT 	fslider0;
	int 	iVec0[2];
	FAUSTFLOAT 	fslider1;
	FAUSTFLOAT 	fslider2;
	FAUSTFLOAT 	fslider3;
	FAUSTFLOAT 	fslider4;
	float 	fConst0;
	float 	fConst1;
	FAUSTFLOAT 	fslider5;
	float 	fRec45[2];
	int 	IOTA;
	float 	fVec1[16384];
	float 	fRec52[2];
	float 	fRec51[2];
	float 	fRec49[2];
	float 	fVec2[16384];
	float 	fRec54[2];
	float 	fRec53[2];
	float 	fRec50[2];
	float 	fVec3[16384];
	float 	fRec55[2];
	float 	fRec48[2];
	float 	fRec46[2];
	float 	fVec4[16384];
	float 	fRec57[2];
	float 	fRec56[2];
	float 	fRec47[2];
	float 	fVec5[16384];
	float 	fRec44[2];
	float 	fRec42[2];
	float 	fVec6[16384];
	float 	fRec59[2];
	float 	fRec58[2];
	float 	fRec43[2];
	float 	fVec7[16384];
	float 	fRec60[2];
	float 	fRec41[2];
	float 	fRec39[2];
	float 	fVec8[16384];
	float 	fRec62[2];
	float 	fRec61[2];
	float 	fRec40[2];
	float 	fVec9[16384];
	float 	fRec63[2];
	float 	fRec38[2];
	float 	fRec36[2];
	float 	fRec65[2];
	float 	fVec10[16384];
	float 	fRec64[2];
	float 	fRec37[2];
	float 	fVec11[1024];
	FAUSTFLOAT 	fslider6;
	float 	fConst2;
	FAUSTFLOAT 	fslider7;
	float 	fRec66[2];
	float 	fRec67[2];
	float 	fVec12[16384];
	float 	fRec68[2];
	float 	fRec35[2];
	float 	fVec13[1024];
	float 	fVec14[16384];
	float 	fRec70[2];
	float 	fRec69[2];
	float 	fVec15[16384];
	float 	fRec71[2];
	float 	fRec34[2];
	float 	fRec32[2];
	float 	fVec16[16384];
	float 	fRec73[2];
	float 	fRec72[2];
	float 	fRec33[2];
	float 	fVec17[16384];
	float 	fRec74[2];
	float 	fRec31[2];
	float 	fRec29[2];
	float 	fVec18[16384];
	float 	fRec76[2];
	float 	fRec75[2];
	float 	fRec30[2];
	float 	fVec19[16384];
	float 	fRec77[2];
	float 	fRec28[2];
	float 	fRec26[2];
	float 	fVec20[16384];
	float 	fRec79[2];
	float 	fRec78[2];
	float 	fRec27[2];
	float 	fVec21[16384];
	float 	fRec80[2];
	float 	fRec25[2];
	float 	fRec23[2];
	float 	fVec22[16384];
	float 	fRec82[2];
	float 	fRec81[2];
	float 	fRec24[2];
	float 	fVec23[16384];
	float 	fRec83[2];
	float 	fRec22[2];
	float 	fRec20[2];
	float 	fVec24[16384];
	float 	fRec85[2];
	float 	fRec84[2];
	float 	fRec21[2];
	float 	fVec25[16384];
	float 	fVec26[16384];
	float 	fRec86[2];
	float 	fRec19[2];
	float 	fRec18[2];
	float 	fRec17[3];
	float 	fRec16[3];
	FAUSTFLOAT 	fslider8;
	float 	fRec15[3];
	FAUSTFLOAT 	fslider9;
	float 	fRec92[2];
	float 	fRec91[3];
	float 	fRec90[3];
	float 	fVec27[2];
	float 	fRec89[2];
	float 	fRec88[3];
	float 	fRec87[3];
	FAUSTFLOAT 	fslider10;
	float 	fRec95[2];
	float 	fRec94[3];
	float 	fRec93[3];
	float 	fVec28[1024];
	float 	fRec14[2];
	float 	fVec29[16384];
	float 	fVec30[16384];
	float 	fRec102[2];
	float 	fRec101[2];
	float 	fRec100[2];
	float 	fRec99[3];
	float 	fRec98[3];
	float 	fRec97[3];
	float 	fRec108[2];
	float 	fRec107[3];
	float 	fRec106[3];
	float 	fVec31[2];
	float 	fRec105[2];
	float 	fRec104[3];
	float 	fRec103[3];
	float 	fRec111[2];
	float 	fRec110[3];
	float 	fRec109[3];
	float 	fVec32[1024];
	float 	fRec96[2];
	float 	fVec33[16384];
	float 	fRec13[2];
	float 	fRec11[2];
	float 	fVec34[16384];
	float 	fRec113[2];
	float 	fRec112[2];
	float 	fRec12[2];
	float 	fVec35[16384];
	float 	fRec10[2];
	float 	fRec8[2];
	float 	fVec36[16384];
	float 	fRec114[2];
	float 	fRec9[2];
	float 	fVec37[16384];
	float 	fRec7[2];
	float 	fRec5[2];
	float 	fVec38[16384];
	float 	fRec116[2];
	float 	fRec115[2];
	float 	fRec6[2];
	float 	fVec39[16384];
	float 	fRec117[2];
	float 	fRec4[2];
	float 	fRec2[2];
	float 	fVec40[16384];
	float 	fRec118[2];
	float 	fRec3[2];
	float 	fRec0[2];
	float 	fRec1[2];
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("copyright", "(c) Julian Parker 2013");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("name", "JPverbRaw");
		m->declare("version", "1.1");
		m->declare("author", "Julian Parker, bug fixes by Till Bovermann");
		m->declare("license", "GPL2+");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.0");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.0");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
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
		fConst1 = (3.1415927f / fConst0);
		fConst2 = (6.2831855f / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.707f;
		fslider1 = 0.0f;
		fslider2 = 1.0f;
		fslider3 = 1.0f;
		fslider4 = 1.0f;
		fslider5 = 2e+03f;
		fslider6 = 0.1f;
		fslider7 = 2.0f;
		fslider8 = 5e+02f;
		fslider9 = 1.0f;
		fslider10 = 1.0f;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) iVec0[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		IOTA = 0;
		for (int i=0; i<16384; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<16384; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<16384; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<16384; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<16384; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<16384; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<16384; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<16384; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<16384; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<16384; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<1024; i++) fVec11[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<16384; i++) fVec12[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<1024; i++) fVec13[i] = 0;
		for (int i=0; i<16384; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<16384; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<16384; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<16384; i++) fVec17[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<16384; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<16384; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<16384; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<16384; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<16384; i++) fVec22[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<16384; i++) fVec23[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<16384; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<16384; i++) fVec25[i] = 0;
		for (int i=0; i<16384; i++) fVec26[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<3; i++) fRec17[i] = 0;
		for (int i=0; i<3; i++) fRec16[i] = 0;
		for (int i=0; i<3; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec92[i] = 0;
		for (int i=0; i<3; i++) fRec91[i] = 0;
		for (int i=0; i<3; i++) fRec90[i] = 0;
		for (int i=0; i<2; i++) fVec27[i] = 0;
		for (int i=0; i<2; i++) fRec89[i] = 0;
		for (int i=0; i<3; i++) fRec88[i] = 0;
		for (int i=0; i<3; i++) fRec87[i] = 0;
		for (int i=0; i<2; i++) fRec95[i] = 0;
		for (int i=0; i<3; i++) fRec94[i] = 0;
		for (int i=0; i<3; i++) fRec93[i] = 0;
		for (int i=0; i<1024; i++) fVec28[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<16384; i++) fVec29[i] = 0;
		for (int i=0; i<16384; i++) fVec30[i] = 0;
		for (int i=0; i<2; i++) fRec102[i] = 0;
		for (int i=0; i<2; i++) fRec101[i] = 0;
		for (int i=0; i<2; i++) fRec100[i] = 0;
		for (int i=0; i<3; i++) fRec99[i] = 0;
		for (int i=0; i<3; i++) fRec98[i] = 0;
		for (int i=0; i<3; i++) fRec97[i] = 0;
		for (int i=0; i<2; i++) fRec108[i] = 0;
		for (int i=0; i<3; i++) fRec107[i] = 0;
		for (int i=0; i<3; i++) fRec106[i] = 0;
		for (int i=0; i<2; i++) fVec31[i] = 0;
		for (int i=0; i<2; i++) fRec105[i] = 0;
		for (int i=0; i<3; i++) fRec104[i] = 0;
		for (int i=0; i<3; i++) fRec103[i] = 0;
		for (int i=0; i<2; i++) fRec111[i] = 0;
		for (int i=0; i<3; i++) fRec110[i] = 0;
		for (int i=0; i<3; i++) fRec109[i] = 0;
		for (int i=0; i<1024; i++) fVec32[i] = 0;
		for (int i=0; i<2; i++) fRec96[i] = 0;
		for (int i=0; i<16384; i++) fVec33[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<16384; i++) fVec34[i] = 0;
		for (int i=0; i<2; i++) fRec113[i] = 0;
		for (int i=0; i<2; i++) fRec112[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<16384; i++) fVec35[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<16384; i++) fVec36[i] = 0;
		for (int i=0; i<2; i++) fRec114[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<16384; i++) fVec37[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<16384; i++) fVec38[i] = 0;
		for (int i=0; i<2; i++) fRec116[i] = 0;
		for (int i=0; i<2; i++) fRec115[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<16384; i++) fVec39[i] = 0;
		for (int i=0; i<2; i++) fRec117[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<16384; i++) fVec40[i] = 0;
		for (int i=0; i<2; i++) fRec118[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
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
		ui_interface->addHorizontalSlider("damp", &fslider1, 0.0f, 0.0f, 0.999f, 0.0001f);
		ui_interface->addHorizontalSlider("earlyDiff", &fslider0, 0.707f, 0.0f, 0.99f, 0.001f);
		ui_interface->addHorizontalSlider("highBand", &fslider5, 2e+03f, 1e+03f, 1e+04f, 0.1f);
		ui_interface->addHorizontalSlider("highX", &fslider4, 1.0f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("lowBand", &fslider8, 5e+02f, 1e+02f, 6e+03f, 0.1f);
		ui_interface->addHorizontalSlider("lowX", &fslider10, 1.0f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("mDepth", &fslider6, 0.1f, 0.0f, 1.0f, 0.001f);
		ui_interface->addHorizontalSlider("mFreq", &fslider7, 2.0f, 0.0f, 1e+01f, 0.01f);
		ui_interface->addHorizontalSlider("midX", &fslider9, 1.0f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("size", &fslider2, 1.0f, 0.5f, 3.0f, 0.01f);
		ui_interface->addHorizontalSlider("t60", &fslider3, 1.0f, 0.1f, 6e+01f, 0.1f);
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = float(fslider0);
		float 	fSlow1 = cosf(fSlow0);
		float 	fSlow2 = sinf(fSlow0);
		float 	fSlow3 = (0 - fSlow2);
		float 	fSlow4 = float(fslider1);
		float 	fSlow5 = (1.0f - fSlow4);
		float 	fSlow6 = float(fslider2);
		float 	fSlow7 = powf(10,(0 - (0.51f * (((1.25f * fSlow6) + -0.25f) / float(fslider3)))));
		float 	fSlow8 = float(fslider4);
		float 	fSlow9 = tanf((fConst1 * float(fslider5)));
		float 	fSlow10 = (1.0f / fSlow9);
		float 	fSlow11 = (((fSlow10 + 0.618034f) / fSlow9) + 1);
		float 	fSlow12 = (1.0f / fSlow11);
		float 	fSlow13 = (1.0f / faustpower<2>(fSlow9));
		float 	fSlow14 = (2 * (0 - fSlow13));
		float 	fSlow15 = (1.0f / (((fSlow10 + 1.618034f) / fSlow9) + 1));
		float 	fSlow16 = (fSlow10 + 1);
		float 	fSlow17 = (1.0f / fSlow16);
		int 	iSlow18 = primes((70 * fSlow6));
		float 	fSlow19 = (0.0001f * iSlow18);
		int 	iSlow20 = primes((10 * fSlow6));
		float 	fSlow21 = (0.0001f * iSlow20);
		int 	iSlow22 = primes((110 * fSlow6));
		float 	fSlow23 = (0.0001f * iSlow22);
		int 	iSlow24 = primes((40 * fSlow6));
		float 	fSlow25 = (0.0001f * iSlow24);
		int 	iSlow26 = primes((140 * fSlow6));
		float 	fSlow27 = (0.0001f * iSlow26);
		int 	iSlow28 = primes((170 * fSlow6));
		float 	fSlow29 = (0.0001f * iSlow28);
		int 	iSlow30 = primes((100 * fSlow6));
		float 	fSlow31 = (0.0001f * iSlow30);
		int 	iSlow32 = primes((200 * fSlow6));
		float 	fSlow33 = (0.0001f * iSlow32);
		int 	iSlow34 = primes((130 * fSlow6));
		float 	fSlow35 = (0.0001f * iSlow34);
		int 	iSlow36 = primes((230 * fSlow6));
		float 	fSlow37 = (0.0001f * iSlow36);
		float 	fSlow38 = float(fslider6);
		float 	fSlow39 = (50 * fSlow38);
		float 	fSlow40 = (fConst2 * float(fslider7));
		float 	fSlow41 = sinf(fSlow40);
		float 	fSlow42 = cosf(fSlow40);
		float 	fSlow43 = (0 - fSlow41);
		int 	iSlow44 = primes((54 * fSlow6));
		float 	fSlow45 = (0.005f * iSlow44);
		float 	fSlow46 = (0 - (5e+01f * fSlow38));
		int 	iSlow47 = primes((204 * fSlow6));
		float 	fSlow48 = (0.005f * iSlow47);
		int 	iSlow49 = primes((125 * fSlow6));
		float 	fSlow50 = (0.0001f * iSlow49);
		int 	iSlow51 = primes((25 * fSlow6));
		float 	fSlow52 = (0.0001f * iSlow51);
		int 	iSlow53 = primes((155 * fSlow6));
		float 	fSlow54 = (0.0001f * iSlow53);
		int 	iSlow55 = primes((55 * fSlow6));
		float 	fSlow56 = (0.0001f * iSlow55);
		int 	iSlow57 = primes((185 * fSlow6));
		float 	fSlow58 = (0.0001f * iSlow57);
		int 	iSlow59 = primes((85 * fSlow6));
		float 	fSlow60 = (0.0001f * iSlow59);
		int 	iSlow61 = primes((215 * fSlow6));
		float 	fSlow62 = (0.0001f * iSlow61);
		int 	iSlow63 = primes((115 * fSlow6));
		float 	fSlow64 = (0.0001f * iSlow63);
		int 	iSlow65 = primes((245 * fSlow6));
		float 	fSlow66 = (0.0001f * iSlow65);
		int 	iSlow67 = primes((145 * fSlow6));
		float 	fSlow68 = (0.0001f * iSlow67);
		int 	iSlow69 = primes((134 * fSlow6));
		float 	fSlow70 = (0.005f * iSlow69);
		float 	fSlow71 = (0 - fSlow10);
		float 	fSlow72 = (0 - ((1 - fSlow10) / fSlow16));
		float 	fSlow73 = (2 * (1 - fSlow13));
		float 	fSlow74 = (((fSlow10 + -1.618034f) / fSlow9) + 1);
		float 	fSlow75 = (((fSlow10 + -0.618034f) / fSlow9) + 1);
		float 	fSlow76 = tanf((fConst1 * float(fslider8)));
		float 	fSlow77 = (1.0f / fSlow76);
		float 	fSlow78 = (1.0f / (((fSlow77 + 1.618034f) / fSlow76) + 1));
		float 	fSlow79 = (((fSlow77 + -1.618034f) / fSlow76) + 1);
		float 	fSlow80 = (1.0f / faustpower<2>(fSlow76));
		float 	fSlow81 = (2 * (1 - fSlow80));
		float 	fSlow82 = (1.0f / (((fSlow77 + 0.618034f) / fSlow76) + 1));
		float 	fSlow83 = float(fslider9);
		float 	fSlow84 = (2 * (0 - fSlow80));
		float 	fSlow85 = (1.0f / (((fSlow77 + 1.618034f) / fSlow76) + 1));
		float 	fSlow86 = (fSlow77 + 1);
		float 	fSlow87 = (0 - ((1 - fSlow77) / fSlow86));
		float 	fSlow88 = (1.0f / fSlow86);
		float 	fSlow89 = (0 - fSlow77);
		float 	fSlow90 = (1.0f / (fSlow11 * fSlow76));
		float 	fSlow91 = (((fSlow77 + -1.618034f) / fSlow76) + 1);
		float 	fSlow92 = (((fSlow77 + -0.618034f) / fSlow76) + 1);
		float 	fSlow93 = float(fslider10);
		int 	iSlow94 = primes((34 * fSlow6));
		float 	fSlow95 = (0.005f * iSlow94);
		int 	iSlow96 = primes((240 * fSlow6));
		float 	fSlow97 = (0.0001f * iSlow96);
		int 	iSlow98 = primes((190 * fSlow6));
		float 	fSlow99 = (0.0001f * iSlow98);
		int 	iSlow100 = primes((175 * fSlow6));
		float 	fSlow101 = (0.0001f * iSlow100);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			iVec0[0] = 1;
			int iTemp0 = (1 - iVec0[1]);
			fRec45[0] = (fSlow19 + (0.9999f * ((iSlow18 * iTemp0) + fRec45[1])));
			float fTemp1 = (fRec45[0] + -1.49999f);
			float fTemp2 = floorf(fTemp1);
			float fTemp3 = (fTemp2 + (2.0f - fRec45[0]));
			float fTemp4 = (fRec45[0] - fTemp2);
			float fTemp5 = ((0.760314f * fRec0[1]) - (0.64955574f * fRec49[1]));
			float fTemp6 = ((0.760314f * fRec1[1]) - (0.64955574f * fRec50[1]));
			float fTemp7 = ((0.70710677f * fTemp5) - (0.70710677f * fTemp6));
			fVec1[IOTA&16383] = fTemp7;
			fRec52[0] = (fSlow21 + (0.9999f * ((iSlow20 * iTemp0) + fRec52[1])));
			float fTemp8 = (fRec52[0] + -1.49999f);
			int iTemp9 = int(min(8192, max(0, int(fTemp8))));
			float fTemp10 = floorf(fTemp8);
			float fTemp11 = (fTemp10 + (2.0f - fRec52[0]));
			float fTemp12 = (fRec52[0] - fTemp10);
			fRec51[0] = (fVec1[(IOTA-int((iTemp9 + 1)))&16383] + ((fRec51[1] * (0 - (fTemp11 / fTemp12))) + ((fTemp11 * fVec1[(IOTA-iTemp9)&16383]) / fTemp12)));
			fRec49[0] = fRec51[0];
			float fTemp13 = ((0.70710677f * fTemp5) + (0.70710677f * fTemp6));
			fVec2[IOTA&16383] = fTemp13;
			fRec54[0] = (fSlow23 + (0.9999f * ((iSlow22 * iTemp0) + fRec54[1])));
			float fTemp14 = (fRec54[0] + -1.49999f);
			int iTemp15 = int(min(8192, max(0, int(fTemp14))));
			float fTemp16 = floorf(fTemp14);
			float fTemp17 = (fTemp16 + (2.0f - fRec54[0]));
			float fTemp18 = (fRec54[0] - fTemp16);
			fRec53[0] = (fVec2[(IOTA-int((iTemp15 + 1)))&16383] + ((fRec53[1] * (0 - (fTemp17 / fTemp18))) + ((fTemp17 * fVec2[(IOTA-iTemp15)&16383]) / fTemp18)));
			fRec50[0] = fRec53[0];
			float fTemp19 = ((0.760314f * fRec49[1]) + (0.64955574f * fRec0[1]));
			float fTemp20 = ((0.760314f * fTemp19) - (0.64955574f * fRec46[1]));
			float fTemp21 = ((0.760314f * fRec50[1]) + (0.64955574f * fRec1[1]));
			float fTemp22 = ((0.760314f * fTemp21) - (0.64955574f * fRec47[1]));
			float fTemp23 = ((0.70710677f * fTemp20) - (0.70710677f * fTemp22));
			fVec3[IOTA&16383] = fTemp23;
			fRec55[0] = (fSlow25 + (0.9999f * ((iSlow24 * iTemp0) + fRec55[1])));
			float fTemp24 = (fRec55[0] + -1.49999f);
			int iTemp25 = int(min(8192, max(0, int(fTemp24))));
			float fTemp26 = floorf(fTemp24);
			float fTemp27 = (fTemp26 + (2.0f - fRec55[0]));
			float fTemp28 = (fRec55[0] - fTemp26);
			fRec48[0] = (fVec3[(IOTA-int((iTemp25 + 1)))&16383] + ((fRec48[1] * (0 - (fTemp27 / fTemp28))) + ((fTemp27 * fVec3[(IOTA-iTemp25)&16383]) / fTemp28)));
			fRec46[0] = fRec48[0];
			float fTemp29 = ((0.70710677f * fTemp20) + (0.70710677f * fTemp22));
			fVec4[IOTA&16383] = fTemp29;
			fRec57[0] = (fSlow27 + (0.9999f * ((iSlow26 * iTemp0) + fRec57[1])));
			float fTemp30 = (fRec57[0] + -1.49999f);
			int iTemp31 = int(min(8192, max(0, int(fTemp30))));
			float fTemp32 = floorf(fTemp30);
			float fTemp33 = (fTemp32 + (2.0f - fRec57[0]));
			float fTemp34 = (fRec57[0] - fTemp32);
			fRec56[0] = (fVec4[(IOTA-int((iTemp31 + 1)))&16383] + ((fRec56[1] * (0 - (fTemp33 / fTemp34))) + ((fTemp33 * fVec4[(IOTA-iTemp31)&16383]) / fTemp34)));
			fRec47[0] = fRec56[0];
			float fTemp35 = ((0.760314f * fRec46[1]) + (0.64955574f * fTemp19));
			float fTemp36 = ((0.760314f * fTemp35) - (0.64955574f * fRec42[1]));
			float fTemp37 = ((0.760314f * fRec47[1]) + (0.64955574f * fTemp21));
			float fTemp38 = ((0.760314f * fTemp37) - (0.64955574f * fRec43[1]));
			float fTemp39 = ((0.70710677f * fTemp36) - (0.70710677f * fTemp38));
			fVec5[IOTA&16383] = fTemp39;
			int iTemp40 = int(min(8192, max(0, int(fTemp1))));
			fRec44[0] = (((fRec44[1] * (0 - (fTemp3 / fTemp4))) + ((fTemp3 * fVec5[(IOTA-iTemp40)&16383]) / fTemp4)) + fVec5[(IOTA-int((iTemp40 + 1)))&16383]);
			fRec42[0] = fRec44[0];
			float fTemp41 = ((0.70710677f * fTemp36) + (0.70710677f * fTemp38));
			fVec6[IOTA&16383] = fTemp41;
			fRec59[0] = (fSlow29 + (0.9999f * ((iSlow28 * iTemp0) + fRec59[1])));
			float fTemp42 = (fRec59[0] + -1.49999f);
			int iTemp43 = int(min(8192, max(0, int(fTemp42))));
			float fTemp44 = floorf(fTemp42);
			float fTemp45 = (fTemp44 + (2.0f - fRec59[0]));
			float fTemp46 = (fRec59[0] - fTemp44);
			fRec58[0] = (fVec6[(IOTA-int((iTemp43 + 1)))&16383] + ((fRec58[1] * (0 - (fTemp45 / fTemp46))) + ((fTemp45 * fVec6[(IOTA-iTemp43)&16383]) / fTemp46)));
			fRec43[0] = fRec58[0];
			float fTemp47 = ((0.760314f * fRec42[1]) + (0.64955574f * fTemp35));
			float fTemp48 = ((0.760314f * fTemp47) - (0.64955574f * fRec39[1]));
			float fTemp49 = ((0.760314f * fRec43[1]) + (0.64955574f * fTemp37));
			float fTemp50 = ((0.760314f * fTemp49) - (0.64955574f * fRec40[1]));
			float fTemp51 = ((0.70710677f * fTemp48) - (0.70710677f * fTemp50));
			fVec7[IOTA&16383] = fTemp51;
			fRec60[0] = (fSlow31 + (0.9999f * ((iSlow30 * iTemp0) + fRec60[1])));
			float fTemp52 = (fRec60[0] + -1.49999f);
			int iTemp53 = int(min(8192, max(0, int(fTemp52))));
			float fTemp54 = floorf(fTemp52);
			float fTemp55 = (fTemp54 + (2.0f - fRec60[0]));
			float fTemp56 = (fRec60[0] - fTemp54);
			fRec41[0] = ((((fVec7[(IOTA-iTemp53)&16383] * fTemp55) / fTemp56) + fVec7[(IOTA-int((iTemp53 + 1)))&16383]) + (fRec41[1] * (0 - (fTemp55 / fTemp56))));
			fRec39[0] = fRec41[0];
			float fTemp57 = ((0.70710677f * fTemp48) + (0.70710677f * fTemp50));
			fVec8[IOTA&16383] = fTemp57;
			fRec62[0] = (fSlow33 + (0.9999f * ((iSlow32 * iTemp0) + fRec62[1])));
			float fTemp58 = (fRec62[0] + -1.49999f);
			int iTemp59 = int(min(8192, max(0, int(fTemp58))));
			float fTemp60 = floorf(fTemp58);
			float fTemp61 = (fTemp60 + (2.0f - fRec62[0]));
			float fTemp62 = (fRec62[0] - fTemp60);
			fRec61[0] = (fVec8[(IOTA-int((iTemp59 + 1)))&16383] + ((fRec61[1] * (0 - (fTemp61 / fTemp62))) + ((fTemp61 * fVec8[(IOTA-iTemp59)&16383]) / fTemp62)));
			fRec40[0] = fRec61[0];
			float fTemp63 = ((0.760314f * fRec39[1]) + (0.64955574f * fTemp47));
			float fTemp64 = ((0.760314f * fTemp63) - (0.64955574f * fRec36[1]));
			float fTemp65 = ((0.760314f * fRec40[1]) + (0.64955574f * fTemp49));
			float fTemp66 = ((0.760314f * fTemp65) - (0.64955574f * fRec37[1]));
			float fTemp67 = ((0.70710677f * fTemp64) - (0.70710677f * fTemp66));
			fVec9[IOTA&16383] = fTemp67;
			fRec63[0] = (fSlow35 + (0.9999f * ((iSlow34 * iTemp0) + fRec63[1])));
			float fTemp68 = (fRec63[0] + -1.49999f);
			int iTemp69 = int(min(8192, max(0, int(fTemp68))));
			float fTemp70 = floorf(fTemp68);
			float fTemp71 = (fTemp70 + (2.0f - fRec63[0]));
			float fTemp72 = (fRec63[0] - fTemp70);
			fRec38[0] = ((((fVec9[(IOTA-iTemp69)&16383] * fTemp71) / fTemp72) + fVec9[(IOTA-int((iTemp69 + 1)))&16383]) + (fRec38[1] * (0 - (fTemp71 / fTemp72))));
			fRec36[0] = fRec38[0];
			fRec65[0] = (fSlow37 + (0.9999f * ((iSlow36 * iTemp0) + fRec65[1])));
			float fTemp73 = (fRec65[0] + -1.49999f);
			float fTemp74 = floorf(fTemp73);
			float fTemp75 = (fTemp74 + (2.0f - fRec65[0]));
			float fTemp76 = (fRec65[0] - fTemp74);
			float fTemp77 = ((0.70710677f * fTemp64) + (0.70710677f * fTemp66));
			fVec10[IOTA&16383] = fTemp77;
			int iTemp78 = int(min(8192, max(0, int(fTemp73))));
			fRec64[0] = (((fRec64[1] * (0 - (fTemp75 / fTemp76))) + ((fTemp75 * fVec10[(IOTA-iTemp78)&16383]) / fTemp76)) + fVec10[(IOTA-int((iTemp78 + 1)))&16383]);
			fRec37[0] = fRec64[0];
			float fTemp79 = ((0.760314f * fRec36[1]) + (0.64955574f * fTemp63));
			fVec11[IOTA&1023] = fTemp79;
			fRec66[0] = ((fSlow41 * fRec67[1]) + (fSlow42 * fRec66[1]));
			fRec67[0] = (((fSlow42 * fRec67[1]) + (fSlow43 * fRec66[1])) + iTemp0);
			float fTemp80 = (fSlow39 * (fRec67[0] + 1));
			float fTemp81 = (fTemp80 + 3.500005f);
			int iTemp82 = int(fTemp81);
			int iTemp83 = int(min(512, max(0, iTemp82)));
			float fTemp84 = floorf(fTemp81);
			float fTemp85 = (fTemp80 + (4.0f - fTemp84));
			float fTemp86 = (0 - fTemp85);
			float fTemp87 = (fTemp80 + (3.0f - fTemp84));
			float fTemp88 = (0 - (0.5f * fTemp87));
			float fTemp89 = (fTemp80 + (2.0f - fTemp84));
			float fTemp90 = (0 - (0.33333334f * fTemp89));
			float fTemp91 = (fTemp80 + (1.0f - fTemp84));
			float fTemp92 = (0 - (0.25f * fTemp91));
			float fTemp93 = (fTemp80 + (5.0f - fTemp84));
			int iTemp94 = int(min(512, max(0, (iTemp82 + 1))));
			float fTemp95 = (0 - fTemp87);
			float fTemp96 = (0 - (0.5f * fTemp89));
			float fTemp97 = (0 - (0.33333334f * fTemp91));
			int iTemp98 = int(min(512, max(0, (iTemp82 + 2))));
			float fTemp99 = (0 - fTemp89);
			float fTemp100 = (0 - (0.5f * fTemp91));
			float fTemp101 = (fTemp85 * fTemp87);
			int iTemp102 = int(min(512, max(0, (iTemp82 + 3))));
			float fTemp103 = (0 - fTemp91);
			float fTemp104 = (fTemp101 * fTemp89);
			int iTemp105 = int(min(512, max(0, (iTemp82 + 4))));
			float fTemp106 = (((((fVec11[(IOTA-iTemp83)&1023] * fTemp86) * fTemp88) * fTemp90) * fTemp92) + (fTemp93 * ((((((fVec11[(IOTA-iTemp94)&1023] * fTemp95) * fTemp96) * fTemp97) + (0.5f * (((fTemp85 * fVec11[(IOTA-iTemp98)&1023]) * fTemp99) * fTemp100))) + (0.16666667f * ((fTemp101 * fVec11[(IOTA-iTemp102)&1023]) * fTemp103))) + (0.041666668f * (fTemp104 * fVec11[(IOTA-iTemp105)&1023])))));
			fVec12[IOTA&16383] = fTemp106;
			fRec68[0] = (fSlow45 + (0.995f * ((iSlow44 * iTemp0) + fRec68[1])));
			float fTemp107 = (fRec68[0] + -1.49999f);
			int iTemp108 = int(min(8192, max(0, int(fTemp107))));
			float fTemp109 = floorf(fTemp107);
			float fTemp110 = (fTemp109 + (2.0f - fRec68[0]));
			float fTemp111 = (fRec68[0] - fTemp109);
			fRec35[0] = (fVec12[(IOTA-int((iTemp108 + 1)))&16383] + ((fRec35[1] * (0 - (fTemp110 / fTemp111))) + ((fVec12[(IOTA-iTemp108)&16383] * fTemp110) / fTemp111)));
			float fTemp112 = ((0.760314f * fRec35[0]) - (0.64955574f * fRec32[1]));
			float fTemp113 = ((0.760314f * fRec37[1]) + (0.64955574f * fTemp65));
			fVec13[IOTA&1023] = fTemp113;
			float fTemp114 = (fSlow46 * fRec67[0]);
			float fTemp115 = (fSlow39 + (fTemp114 + 3.500005f));
			int iTemp116 = int(fTemp115);
			float fTemp117 = floorf(fTemp115);
			float fTemp118 = (fSlow39 + (fTemp114 + (4.0f - fTemp117)));
			float fTemp119 = (fSlow39 + (fTemp114 + (3.0f - fTemp117)));
			float fTemp120 = (fSlow39 + (fTemp114 + (2.0f - fTemp117)));
			float fTemp121 = (fSlow39 + (fTemp114 + (1.0f - fTemp117)));
			float fTemp122 = (fTemp118 * fTemp119);
			float fTemp123 = (((((fVec13[(IOTA-int(min(512, max(0, iTemp116))))&1023] * (0 - fTemp118)) * (0 - (0.5f * fTemp119))) * (0 - (0.33333334f * fTemp120))) * (0 - (0.25f * fTemp121))) + ((fSlow39 + (fTemp114 + (5.0f - fTemp117))) * ((((((fVec13[(IOTA-int(min(512, max(0, (iTemp116 + 1)))))&1023] * (0 - fTemp119)) * (0 - (0.5f * fTemp120))) * (0 - (0.33333334f * fTemp121))) + (0.5f * (((fTemp118 * fVec13[(IOTA-int(min(512, max(0, (iTemp116 + 2)))))&1023]) * (0 - fTemp120)) * (0 - (0.5f * fTemp121))))) + (0.16666667f * ((fTemp122 * fVec13[(IOTA-int(min(512, max(0, (iTemp116 + 3)))))&1023]) * (0 - fTemp121)))) + (0.041666668f * ((fTemp122 * fTemp120) * fVec13[(IOTA-int(min(512, max(0, (iTemp116 + 4)))))&1023])))));
			fVec14[IOTA&16383] = fTemp123;
			fRec70[0] = (fSlow48 + (0.995f * ((iSlow47 * iTemp0) + fRec70[1])));
			float fTemp124 = (fRec70[0] + -1.49999f);
			int iTemp125 = int(min(8192, max(0, int(fTemp124))));
			float fTemp126 = floorf(fTemp124);
			float fTemp127 = (fTemp126 + (2.0f - fRec70[0]));
			float fTemp128 = (fRec70[0] - fTemp126);
			fRec69[0] = (fVec14[(IOTA-int((iTemp125 + 1)))&16383] + ((fRec69[1] * (0 - (fTemp127 / fTemp128))) + ((fTemp127 * fVec14[(IOTA-iTemp125)&16383]) / fTemp128)));
			float fTemp129 = ((0.760314f * fRec69[0]) - (0.64955574f * fRec33[1]));
			float fTemp130 = ((0.70710677f * fTemp112) - (0.70710677f * fTemp129));
			fVec15[IOTA&16383] = fTemp130;
			fRec71[0] = (fSlow50 + (0.9999f * ((iSlow49 * iTemp0) + fRec71[1])));
			float fTemp131 = (fRec71[0] + -1.49999f);
			int iTemp132 = int(min(8192, max(0, int(fTemp131))));
			float fTemp133 = floorf(fTemp131);
			float fTemp134 = (fTemp133 + (2.0f - fRec71[0]));
			float fTemp135 = (fRec71[0] - fTemp133);
			fRec34[0] = ((((fVec15[(IOTA-iTemp132)&16383] * fTemp134) / fTemp135) + fVec15[(IOTA-int((iTemp132 + 1)))&16383]) + (fRec34[1] * (0 - (fTemp134 / fTemp135))));
			fRec32[0] = fRec34[0];
			float fTemp136 = ((0.70710677f * fTemp112) + (0.70710677f * fTemp129));
			fVec16[IOTA&16383] = fTemp136;
			fRec73[0] = (fSlow52 + (0.9999f * ((iSlow51 * iTemp0) + fRec73[1])));
			float fTemp137 = (fRec73[0] + -1.49999f);
			int iTemp138 = int(min(8192, max(0, int(fTemp137))));
			float fTemp139 = floorf(fTemp137);
			float fTemp140 = (fTemp139 + (2.0f - fRec73[0]));
			float fTemp141 = (fRec73[0] - fTemp139);
			fRec72[0] = (fVec16[(IOTA-int((iTemp138 + 1)))&16383] + ((fRec72[1] * (0 - (fTemp140 / fTemp141))) + ((fTemp140 * fVec16[(IOTA-iTemp138)&16383]) / fTemp141)));
			fRec33[0] = fRec72[0];
			float fTemp142 = ((0.760314f * fRec32[1]) + (0.64955574f * fRec35[0]));
			float fTemp143 = ((0.760314f * fTemp142) - (0.64955574f * fRec29[1]));
			float fTemp144 = ((0.760314f * fRec33[1]) + (0.64955574f * fRec69[0]));
			float fTemp145 = ((0.760314f * fTemp144) - (0.64955574f * fRec30[1]));
			float fTemp146 = ((0.70710677f * fTemp143) - (0.70710677f * fTemp145));
			fVec17[IOTA&16383] = fTemp146;
			fRec74[0] = (fSlow54 + (0.9999f * ((iSlow53 * iTemp0) + fRec74[1])));
			float fTemp147 = (fRec74[0] + -1.49999f);
			int iTemp148 = int(min(8192, max(0, int(fTemp147))));
			float fTemp149 = floorf(fTemp147);
			float fTemp150 = (fTemp149 + (2.0f - fRec74[0]));
			float fTemp151 = (fRec74[0] - fTemp149);
			fRec31[0] = ((((fVec17[(IOTA-iTemp148)&16383] * fTemp150) / fTemp151) + fVec17[(IOTA-int((iTemp148 + 1)))&16383]) + (fRec31[1] * (0 - (fTemp150 / fTemp151))));
			fRec29[0] = fRec31[0];
			float fTemp152 = ((0.70710677f * fTemp143) + (0.70710677f * fTemp145));
			fVec18[IOTA&16383] = fTemp152;
			fRec76[0] = (fSlow56 + (0.9999f * ((iSlow55 * iTemp0) + fRec76[1])));
			float fTemp153 = (fRec76[0] + -1.49999f);
			int iTemp154 = int(min(8192, max(0, int(fTemp153))));
			int iTemp155 = int((iTemp154 + 1));
			float fTemp156 = floorf(fTemp153);
			float fTemp157 = (fTemp156 + (2.0f - fRec76[0]));
			float fTemp158 = (fRec76[0] - fTemp156);
			float fTemp159 = (0 - (fTemp157 / fTemp158));
			fRec75[0] = (fVec18[(IOTA-iTemp155)&16383] + ((fRec75[1] * fTemp159) + ((fTemp157 * fVec18[(IOTA-iTemp154)&16383]) / fTemp158)));
			fRec30[0] = fRec75[0];
			float fTemp160 = ((0.760314f * fRec29[1]) + (0.64955574f * fTemp142));
			float fTemp161 = ((0.760314f * fTemp160) - (0.64955574f * fRec26[1]));
			float fTemp162 = ((0.760314f * fRec30[1]) + (0.64955574f * fTemp144));
			float fTemp163 = ((0.760314f * fTemp162) - (0.64955574f * fRec27[1]));
			float fTemp164 = ((0.70710677f * fTemp161) - (0.70710677f * fTemp163));
			fVec19[IOTA&16383] = fTemp164;
			fRec77[0] = (fSlow58 + (0.9999f * ((iSlow57 * iTemp0) + fRec77[1])));
			float fTemp165 = (fRec77[0] + -1.49999f);
			int iTemp166 = int(min(8192, max(0, int(fTemp165))));
			float fTemp167 = floorf(fTemp165);
			float fTemp168 = (fTemp167 + (2.0f - fRec77[0]));
			float fTemp169 = (fRec77[0] - fTemp167);
			fRec28[0] = (fVec19[(IOTA-int((iTemp166 + 1)))&16383] + ((fRec28[1] * (0 - (fTemp168 / fTemp169))) + ((fTemp168 * fVec19[(IOTA-iTemp166)&16383]) / fTemp169)));
			fRec26[0] = fRec28[0];
			float fTemp170 = ((0.70710677f * fTemp161) + (0.70710677f * fTemp163));
			fVec20[IOTA&16383] = fTemp170;
			fRec79[0] = (fSlow60 + (0.9999f * ((iSlow59 * iTemp0) + fRec79[1])));
			float fTemp171 = (fRec79[0] + -1.49999f);
			int iTemp172 = int(min(8192, max(0, int(fTemp171))));
			int iTemp173 = int((iTemp172 + 1));
			float fTemp174 = floorf(fTemp171);
			float fTemp175 = (fTemp174 + (2.0f - fRec79[0]));
			float fTemp176 = (fRec79[0] - fTemp174);
			float fTemp177 = (0 - (fTemp175 / fTemp176));
			fRec78[0] = (fVec20[(IOTA-iTemp173)&16383] + ((fRec78[1] * fTemp177) + ((fTemp175 * fVec20[(IOTA-iTemp172)&16383]) / fTemp176)));
			fRec27[0] = fRec78[0];
			float fTemp178 = ((0.760314f * fRec26[1]) + (0.64955574f * fTemp160));
			float fTemp179 = ((0.760314f * fTemp178) - (0.64955574f * fRec23[1]));
			float fTemp180 = ((0.760314f * fRec27[1]) + (0.64955574f * fTemp162));
			float fTemp181 = ((0.760314f * fTemp180) - (0.64955574f * fRec24[1]));
			float fTemp182 = ((0.70710677f * fTemp179) - (0.70710677f * fTemp181));
			fVec21[IOTA&16383] = fTemp182;
			fRec80[0] = (fSlow62 + (0.9999f * ((iSlow61 * iTemp0) + fRec80[1])));
			float fTemp183 = (fRec80[0] + -1.49999f);
			int iTemp184 = int(min(8192, max(0, int(fTemp183))));
			int iTemp185 = int((iTemp184 + 1));
			float fTemp186 = floorf(fTemp183);
			float fTemp187 = (fTemp186 + (2.0f - fRec80[0]));
			float fTemp188 = (fRec80[0] - fTemp186);
			float fTemp189 = (0 - (fTemp187 / fTemp188));
			fRec25[0] = (fVec21[(IOTA-iTemp185)&16383] + ((fTemp189 * fRec25[1]) + ((fTemp187 * fVec21[(IOTA-iTemp184)&16383]) / fTemp188)));
			fRec23[0] = fRec25[0];
			float fTemp190 = ((0.70710677f * fTemp179) + (0.70710677f * fTemp181));
			fVec22[IOTA&16383] = fTemp190;
			fRec82[0] = (fSlow64 + (0.9999f * ((iSlow63 * iTemp0) + fRec82[1])));
			float fTemp191 = (fRec82[0] + -1.49999f);
			int iTemp192 = int(min(8192, max(0, int(fTemp191))));
			int iTemp193 = int((iTemp192 + 1));
			float fTemp194 = floorf(fTemp191);
			float fTemp195 = (fTemp194 + (2.0f - fRec82[0]));
			float fTemp196 = (fRec82[0] - fTemp194);
			float fTemp197 = (0 - (fTemp195 / fTemp196));
			fRec81[0] = (fVec22[(IOTA-iTemp193)&16383] + ((fTemp197 * fRec81[1]) + ((fTemp195 * fVec22[(IOTA-iTemp192)&16383]) / fTemp196)));
			fRec24[0] = fRec81[0];
			float fTemp198 = ((0.760314f * fRec23[1]) + (0.64955574f * fTemp178));
			float fTemp199 = ((0.760314f * fTemp198) - (0.64955574f * fRec20[1]));
			float fTemp200 = ((0.760314f * fRec24[1]) + (0.64955574f * fTemp180));
			float fTemp201 = ((0.760314f * fTemp200) - (0.64955574f * fRec21[1]));
			float fTemp202 = ((0.70710677f * fTemp199) - (0.70710677f * fTemp201));
			fVec23[IOTA&16383] = fTemp202;
			fRec83[0] = (fSlow66 + (0.9999f * ((iSlow65 * iTemp0) + fRec83[1])));
			float fTemp203 = (fRec83[0] + -1.49999f);
			int iTemp204 = int(min(8192, max(0, int(fTemp203))));
			float fTemp205 = floorf(fTemp203);
			float fTemp206 = (fTemp205 + (2.0f - fRec83[0]));
			float fTemp207 = (fRec83[0] - fTemp205);
			fRec22[0] = (fVec23[(IOTA-int((iTemp204 + 1)))&16383] + ((fRec22[1] * (0 - (fTemp206 / fTemp207))) + ((fTemp206 * fVec23[(IOTA-iTemp204)&16383]) / fTemp207)));
			fRec20[0] = fRec22[0];
			float fTemp208 = ((0.70710677f * fTemp199) + (0.70710677f * fTemp201));
			fVec24[IOTA&16383] = fTemp208;
			fRec85[0] = (fSlow68 + (0.9999f * ((iSlow67 * iTemp0) + fRec85[1])));
			float fTemp209 = (fRec85[0] + -1.49999f);
			int iTemp210 = int(min(8192, max(0, int(fTemp209))));
			int iTemp211 = int((iTemp210 + 1));
			float fTemp212 = floorf(fTemp209);
			float fTemp213 = (fTemp212 + (2.0f - fRec85[0]));
			float fTemp214 = (fRec85[0] - fTemp212);
			float fTemp215 = (0 - (fTemp213 / fTemp214));
			fRec84[0] = (fVec24[(IOTA-iTemp211)&16383] + ((fRec84[1] * fTemp215) + ((fTemp213 * fVec24[(IOTA-iTemp210)&16383]) / fTemp214)));
			fRec21[0] = fRec84[0];
			float fTemp216 = ((0.760314f * fRec20[1]) + (0.64955574f * fTemp198));
			fVec25[IOTA&16383] = fTemp216;
			float fTemp217 = (fSlow39 * (fRec66[0] + 1));
			float fTemp218 = (fTemp217 + 3.500005f);
			int iTemp219 = int(fTemp218);
			int iTemp220 = max(0, iTemp219);
			float fTemp221 = floorf(fTemp218);
			float fTemp222 = (fTemp217 + (4.0f - fTemp221));
			float fTemp223 = (0 - fTemp222);
			float fTemp224 = (fTemp217 + (3.0f - fTemp221));
			float fTemp225 = (0 - (0.5f * fTemp224));
			float fTemp226 = (fTemp217 + (2.0f - fTemp221));
			float fTemp227 = (0 - (0.33333334f * fTemp226));
			float fTemp228 = (fTemp217 + (1.0f - fTemp221));
			float fTemp229 = (0 - (0.25f * fTemp228));
			float fTemp230 = (fTemp217 + (5.0f - fTemp221));
			int iTemp231 = max(0, (iTemp219 + 1));
			float fTemp232 = (0 - fTemp224);
			float fTemp233 = (0 - (0.5f * fTemp226));
			float fTemp234 = (0 - (0.33333334f * fTemp228));
			int iTemp235 = max(0, (iTemp219 + 2));
			float fTemp236 = (0 - fTemp226);
			float fTemp237 = (0 - (0.5f * fTemp228));
			float fTemp238 = (fTemp222 * fTemp224);
			int iTemp239 = max(0, (iTemp219 + 3));
			float fTemp240 = (0 - fTemp228);
			int iTemp241 = max(0, (iTemp219 + 4));
			float fTemp242 = (((((fVec25[(IOTA-int(min(8192, iTemp220)))&16383] * fTemp223) * fTemp225) * fTemp227) * fTemp229) + (fTemp230 * ((((((fVec25[(IOTA-int(min(8192, iTemp231)))&16383] * fTemp232) * fTemp233) * fTemp234) + (0.5f * (((fTemp222 * fVec25[(IOTA-int(min(8192, iTemp235)))&16383]) * fTemp236) * fTemp237))) + (0.16666667f * ((fTemp238 * fVec25[(IOTA-int(min(8192, iTemp239)))&16383]) * fTemp240))) + (0.041666668f * ((fTemp238 * fTemp226) * fVec25[(IOTA-int(min(8192, iTemp241)))&16383])))));
			fVec26[IOTA&16383] = fTemp242;
			fRec86[0] = (fSlow70 + (0.995f * ((iSlow69 * iTemp0) + fRec86[1])));
			float fTemp243 = (fRec86[0] + -1.49999f);
			int iTemp244 = int(min(8192, max(0, int(fTemp243))));
			float fTemp245 = floorf(fTemp243);
			float fTemp246 = (fTemp245 + (2.0f - fRec86[0]));
			float fTemp247 = (fRec86[0] - fTemp245);
			fRec19[0] = (fVec26[(IOTA-int((iTemp244 + 1)))&16383] + ((fRec19[1] * (0 - (fTemp246 / fTemp247))) + ((fTemp246 * fVec26[(IOTA-iTemp244)&16383]) / fTemp247)));
			fRec18[0] = ((fSlow17 * ((fSlow10 * fRec19[0]) + (fSlow71 * fRec19[1]))) + (fSlow72 * fRec18[1]));
			fRec17[0] = (fRec18[0] - (fSlow15 * ((fSlow73 * fRec17[1]) + (fSlow74 * fRec17[2]))));
			fRec16[0] = ((fSlow15 * (((fSlow14 * fRec17[1]) + (fSlow13 * fRec17[0])) + (fSlow13 * fRec17[2]))) - (fSlow12 * ((fSlow73 * fRec16[1]) + (fSlow75 * fRec16[2]))));
			float fTemp248 = (fSlow81 * fRec15[1]);
			fRec15[0] = ((fSlow12 * (((fSlow14 * fRec16[1]) + (fSlow13 * fRec16[0])) + (fSlow13 * fRec16[2]))) - (fSlow78 * ((fSlow79 * fRec15[2]) + fTemp248)));
			fRec92[0] = ((fSlow72 * fRec92[1]) + (fSlow17 * (fRec19[0] + fRec19[1])));
			fRec91[0] = (fRec92[0] - (fSlow15 * ((fSlow73 * fRec91[1]) + (fSlow74 * fRec91[2]))));
			fRec90[0] = ((fSlow15 * (fRec91[2] + ((2.0f * fRec91[1]) + fRec91[0]))) - (fSlow12 * ((fSlow73 * fRec90[1]) + (fSlow75 * fRec90[2]))));
			float fTemp249 = (fRec90[2] + ((2.0f * fRec90[1]) + fRec90[0]));
			float fTemp250 = (fSlow12 * fTemp249);
			fVec27[0] = fTemp250;
			fRec89[0] = ((fSlow87 * fRec89[1]) + (fSlow88 * ((fSlow89 * fVec27[1]) + (fSlow90 * fTemp249))));
			fRec88[0] = (fRec89[0] - (fSlow85 * ((fSlow81 * fRec88[1]) + (fSlow91 * fRec88[2]))));
			fRec87[0] = ((fSlow85 * (((fSlow80 * fRec88[0]) + (fSlow84 * fRec88[1])) + (fSlow80 * fRec88[2]))) - (fSlow82 * ((fSlow81 * fRec87[1]) + (fSlow92 * fRec87[2]))));
			fRec95[0] = ((fSlow87 * fRec95[1]) + (fSlow88 * (fVec27[0] + fVec27[1])));
			fRec94[0] = (fRec95[0] - (fSlow85 * ((fSlow81 * fRec94[1]) + (fSlow91 * fRec94[2]))));
			fRec93[0] = ((fSlow85 * (fRec94[2] + ((2.0f * fRec94[1]) + fRec94[0]))) - (fSlow82 * ((fSlow81 * fRec93[1]) + (fSlow92 * fRec93[2]))));
			float fTemp251 = ((float)input0[i] + (fSlow7 * ((fSlow8 * (fRec15[2] + (fSlow78 * (fTemp248 + (fSlow79 * fRec15[0]))))) + (fSlow82 * ((fSlow83 * (((fSlow84 * fRec87[1]) + (fSlow80 * fRec87[0])) + (fSlow80 * fRec87[2]))) + (fSlow93 * (fRec93[2] + ((2.0f * fRec93[1]) + fRec93[0]))))))));
			fVec28[IOTA&1023] = fTemp251;
			fRec14[0] = ((fSlow5 * (((((fVec28[(IOTA-int(min(512, iTemp220)))&1023] * fTemp223) * fTemp225) * fTemp227) * fTemp229) + (((((((fVec28[(IOTA-int(min(512, iTemp231)))&1023] * fTemp232) * fTemp233) * fTemp234) + (0.5f * (((fVec28[(IOTA-int(min(512, iTemp235)))&1023] * fTemp222) * fTemp236) * fTemp237))) + (0.16666667f * (((fVec28[(IOTA-int(min(512, iTemp239)))&1023] * fTemp222) * fTemp224) * fTemp240))) + (0.041666668f * (((fVec28[(IOTA-int(min(512, iTemp241)))&1023] * fTemp222) * fTemp224) * fTemp226))) * fTemp230))) + (fSlow4 * fRec14[1]));
			float fTemp252 = ((fSlow1 * fRec14[0]) + (fSlow3 * fRec11[1]));
			float fTemp253 = ((0.760314f * fRec21[1]) + (0.64955574f * fTemp200));
			fVec29[IOTA&16383] = fTemp253;
			float fTemp254 = (fSlow46 * fRec66[0]);
			float fTemp255 = (fSlow39 + (fTemp254 + 3.500005f));
			int iTemp256 = int(fTemp255);
			float fTemp257 = floorf(fTemp255);
			float fTemp258 = (fSlow39 + (fTemp254 + (4.0f - fTemp257)));
			float fTemp259 = (fSlow39 + (fTemp254 + (3.0f - fTemp257)));
			float fTemp260 = (fSlow39 + (fTemp254 + (2.0f - fTemp257)));
			float fTemp261 = (fSlow39 + (fTemp254 + (1.0f - fTemp257)));
			float fTemp262 = ((((0.041666668f * (((fVec29[(IOTA-int(min(8192, max(0, (iTemp256 + 4)))))&16383] * fTemp258) * fTemp259) * fTemp260)) + (((((fVec29[(IOTA-int(min(8192, max(0, (iTemp256 + 1)))))&16383] * (0 - fTemp259)) * (0 - (0.5f * fTemp260))) * (0 - (0.33333334f * fTemp261))) + (0.5f * (((fTemp258 * fVec29[(IOTA-int(min(8192, max(0, (iTemp256 + 2)))))&16383]) * (0 - fTemp260)) * (0 - (0.5f * fTemp261))))) + (0.16666667f * (((fTemp258 * fTemp259) * fVec29[(IOTA-int(min(8192, max(0, (iTemp256 + 3)))))&16383]) * (0 - fTemp261))))) * (fSlow39 + (fTemp254 + (5.0f - fTemp257)))) + ((((fVec29[(IOTA-int(min(8192, max(0, iTemp256))))&16383] * (0 - fTemp258)) * (0 - (0.5f * fTemp259))) * (0 - (0.33333334f * fTemp260))) * (0 - (0.25f * fTemp261))));
			fVec30[IOTA&16383] = fTemp262;
			fRec102[0] = (fSlow95 + (0.995f * ((iSlow94 * iTemp0) + fRec102[1])));
			float fTemp263 = (fRec102[0] + -1.49999f);
			int iTemp264 = int(min(8192, max(0, int(fTemp263))));
			float fTemp265 = floorf(fTemp263);
			float fTemp266 = (fTemp265 + (2.0f - fRec102[0]));
			float fTemp267 = (fRec102[0] - fTemp265);
			fRec101[0] = ((((fVec30[(IOTA-iTemp264)&16383] * fTemp266) / fTemp267) + fVec30[(IOTA-int((iTemp264 + 1)))&16383]) + (fRec101[1] * (0 - (fTemp266 / fTemp267))));
			fRec100[0] = ((fSlow17 * ((fSlow10 * fRec101[0]) + (fSlow71 * fRec101[1]))) + (fSlow72 * fRec100[1]));
			fRec99[0] = (fRec100[0] - (fSlow15 * ((fSlow73 * fRec99[1]) + (fSlow74 * fRec99[2]))));
			fRec98[0] = ((fSlow15 * (((fSlow14 * fRec99[1]) + (fSlow13 * fRec99[0])) + (fSlow13 * fRec99[2]))) - (fSlow12 * ((fSlow73 * fRec98[1]) + (fSlow75 * fRec98[2]))));
			float fTemp268 = (fSlow81 * fRec97[1]);
			fRec97[0] = ((fSlow12 * (((fSlow14 * fRec98[1]) + (fSlow13 * fRec98[0])) + (fSlow13 * fRec98[2]))) - (fSlow78 * (fTemp268 + (fSlow79 * fRec97[2]))));
			fRec108[0] = ((fSlow72 * fRec108[1]) + (fSlow17 * (fRec101[1] + fRec101[0])));
			fRec107[0] = (fRec108[0] - (fSlow15 * ((fSlow73 * fRec107[1]) + (fSlow74 * fRec107[2]))));
			fRec106[0] = ((fSlow15 * (fRec107[2] + ((2.0f * fRec107[1]) + fRec107[0]))) - (fSlow12 * ((fSlow73 * fRec106[1]) + (fSlow75 * fRec106[2]))));
			float fTemp269 = (fRec106[2] + ((2.0f * fRec106[1]) + fRec106[0]));
			float fTemp270 = (fSlow12 * fTemp269);
			fVec31[0] = fTemp270;
			fRec105[0] = ((fSlow87 * fRec105[1]) + (fSlow88 * ((fSlow90 * fTemp269) + (fSlow89 * fVec31[1]))));
			fRec104[0] = (fRec105[0] - (fSlow85 * ((fSlow81 * fRec104[1]) + (fSlow91 * fRec104[2]))));
			fRec103[0] = ((fSlow85 * (((fSlow80 * fRec104[0]) + (fSlow84 * fRec104[1])) + (fSlow80 * fRec104[2]))) - (fSlow82 * ((fSlow81 * fRec103[1]) + (fSlow92 * fRec103[2]))));
			fRec111[0] = ((fSlow87 * fRec111[1]) + (fSlow88 * (fVec31[0] + fVec31[1])));
			fRec110[0] = (fRec111[0] - (fSlow85 * ((fSlow81 * fRec110[1]) + (fSlow91 * fRec110[2]))));
			fRec109[0] = ((fSlow85 * (fRec110[2] + (fRec110[0] + (2.0f * fRec110[1])))) - (fSlow82 * ((fSlow81 * fRec109[1]) + (fSlow92 * fRec109[2]))));
			float fTemp271 = ((float)input1[i] + (fSlow7 * ((fSlow8 * (fRec97[2] + (fSlow78 * (fTemp268 + (fSlow79 * fRec97[0]))))) + (fSlow82 * ((fSlow83 * (((fSlow84 * fRec103[1]) + (fSlow80 * fRec103[0])) + (fSlow80 * fRec103[2]))) + (fSlow93 * (fRec109[2] + (fRec109[0] + (2.0f * fRec109[1])))))))));
			fVec32[IOTA&1023] = fTemp271;
			fRec96[0] = ((fSlow4 * fRec96[1]) + (fSlow5 * (((((fTemp86 * fTemp88) * fTemp90) * fTemp92) * fVec32[(IOTA-iTemp83)&1023]) + (fTemp93 * ((((((fTemp95 * fTemp96) * fTemp97) * fVec32[(IOTA-iTemp94)&1023]) + (0.5f * (((fTemp85 * fTemp99) * fTemp100) * fVec32[(IOTA-iTemp98)&1023]))) + (0.16666667f * ((fTemp101 * fTemp103) * fVec32[(IOTA-iTemp102)&1023]))) + (0.041666668f * (fTemp104 * fVec32[(IOTA-iTemp105)&1023])))))));
			float fTemp272 = ((fSlow3 * fRec12[1]) + (fSlow1 * fRec96[0]));
			float fTemp273 = ((0.70710677f * fTemp252) - (0.70710677f * fTemp272));
			fVec33[IOTA&16383] = fTemp273;
			fRec13[0] = (fVec33[(IOTA-iTemp155)&16383] + ((fRec13[1] * fTemp159) + ((fTemp157 * fVec33[(IOTA-iTemp154)&16383]) / fTemp158)));
			fRec11[0] = fRec13[0];
			float fTemp274 = ((0.70710677f * fTemp252) + (0.70710677f * fTemp272));
			fVec34[IOTA&16383] = fTemp274;
			fRec113[0] = (fSlow97 + (0.9999f * ((iSlow96 * iTemp0) + fRec113[1])));
			float fTemp275 = (fRec113[0] + -1.49999f);
			int iTemp276 = int(min(8192, max(0, int(fTemp275))));
			float fTemp277 = floorf(fTemp275);
			float fTemp278 = (fTemp277 + (2.0f - fRec113[0]));
			float fTemp279 = (fRec113[0] - fTemp277);
			fRec112[0] = (fVec34[(IOTA-int((iTemp276 + 1)))&16383] + ((fRec112[1] * (0 - (fTemp278 / fTemp279))) + ((fTemp278 * fVec34[(IOTA-iTemp276)&16383]) / fTemp279)));
			fRec12[0] = fRec112[0];
			float fTemp280 = ((fSlow1 * fRec11[1]) + (fSlow2 * fRec14[0]));
			float fTemp281 = ((fSlow3 * fRec8[1]) + (fSlow1 * fTemp280));
			float fTemp282 = ((fSlow1 * fRec12[1]) + (fSlow2 * fRec96[0]));
			float fTemp283 = ((fSlow3 * fRec9[1]) + (fSlow1 * fTemp282));
			float fTemp284 = ((0.70710677f * fTemp281) - (0.70710677f * fTemp283));
			fVec35[IOTA&16383] = fTemp284;
			fRec10[0] = (fVec35[(IOTA-iTemp185)&16383] + ((fRec10[1] * fTemp189) + ((fTemp187 * fVec35[(IOTA-iTemp184)&16383]) / fTemp188)));
			fRec8[0] = fRec10[0];
			float fTemp285 = ((0.70710677f * fTemp281) + (0.70710677f * fTemp283));
			fVec36[IOTA&16383] = fTemp285;
			fRec114[0] = (fVec36[(IOTA-iTemp173)&16383] + ((fTemp177 * fRec114[1]) + ((fTemp175 * fVec36[(IOTA-iTemp172)&16383]) / fTemp176)));
			fRec9[0] = fRec114[0];
			float fTemp286 = ((fSlow1 * fRec8[1]) + (fSlow2 * fTemp280));
			float fTemp287 = ((fSlow3 * fRec5[1]) + (fSlow1 * fTemp286));
			float fTemp288 = ((fSlow1 * fRec9[1]) + (fSlow2 * fTemp282));
			float fTemp289 = ((fSlow3 * fRec6[1]) + (fSlow1 * fTemp288));
			float fTemp290 = ((0.70710677f * fTemp287) - (0.70710677f * fTemp289));
			fVec37[IOTA&16383] = fTemp290;
			fRec7[0] = (fVec37[(IOTA-iTemp193)&16383] + ((fRec7[1] * fTemp197) + ((fTemp195 * fVec37[(IOTA-iTemp192)&16383]) / fTemp196)));
			fRec5[0] = fRec7[0];
			float fTemp291 = ((0.70710677f * fTemp287) + (0.70710677f * fTemp289));
			fVec38[IOTA&16383] = fTemp291;
			fRec116[0] = (fSlow99 + (0.9999f * ((iSlow98 * iTemp0) + fRec116[1])));
			float fTemp292 = (fRec116[0] + -1.49999f);
			int iTemp293 = int(min(8192, max(0, int(fTemp292))));
			float fTemp294 = floorf(fTemp292);
			float fTemp295 = (fTemp294 + (2.0f - fRec116[0]));
			float fTemp296 = (fRec116[0] - fTemp294);
			fRec115[0] = (fVec38[(IOTA-int((iTemp293 + 1)))&16383] + ((fRec115[1] * (0 - (fTemp295 / fTemp296))) + ((fTemp295 * fVec38[(IOTA-iTemp293)&16383]) / fTemp296)));
			fRec6[0] = fRec115[0];
			float fTemp297 = ((fSlow1 * fRec5[1]) + (fSlow2 * fTemp286));
			float fTemp298 = ((fSlow3 * fRec2[1]) + (fSlow1 * fTemp297));
			float fTemp299 = ((fSlow1 * fRec6[1]) + (fSlow2 * fTemp288));
			float fTemp300 = ((fSlow1 * fTemp299) + (fSlow3 * fRec3[1]));
			float fTemp301 = ((0.70710677f * fTemp298) - (0.70710677f * fTemp300));
			fVec39[IOTA&16383] = fTemp301;
			fRec117[0] = (fSlow101 + (0.9999f * ((iSlow100 * iTemp0) + fRec117[1])));
			float fTemp302 = (fRec117[0] + -1.49999f);
			int iTemp303 = int(min(8192, max(0, int(fTemp302))));
			float fTemp304 = floorf(fTemp302);
			float fTemp305 = (fTemp304 + (2.0f - fRec117[0]));
			float fTemp306 = (fRec117[0] - fTemp304);
			fRec4[0] = ((((fVec39[(IOTA-iTemp303)&16383] * fTemp305) / fTemp306) + fVec39[(IOTA-int((iTemp303 + 1)))&16383]) + (fRec4[1] * (0 - (fTemp305 / fTemp306))));
			fRec2[0] = fRec4[0];
			float fTemp307 = ((0.70710677f * fTemp298) + (0.70710677f * fTemp300));
			fVec40[IOTA&16383] = fTemp307;
			fRec118[0] = (fVec40[(IOTA-iTemp211)&16383] + ((fRec118[1] * fTemp215) + ((fVec40[(IOTA-iTemp210)&16383] * fTemp213) / fTemp214)));
			fRec3[0] = fRec118[0];
			fRec0[0] = ((fSlow1 * fRec2[1]) + (fSlow2 * fTemp297));
			fRec1[0] = ((fSlow1 * fRec3[1]) + (fSlow2 * fTemp299));
			output0[i] = (FAUSTFLOAT)fRec0[0];
			output1[i] = (FAUSTFLOAT)fRec1[0];
			// post processing
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec3[1] = fRec3[0];
			fRec118[1] = fRec118[0];
			fRec2[1] = fRec2[0];
			fRec4[1] = fRec4[0];
			fRec117[1] = fRec117[0];
			fRec6[1] = fRec6[0];
			fRec115[1] = fRec115[0];
			fRec116[1] = fRec116[0];
			fRec5[1] = fRec5[0];
			fRec7[1] = fRec7[0];
			fRec9[1] = fRec9[0];
			fRec114[1] = fRec114[0];
			fRec8[1] = fRec8[0];
			fRec10[1] = fRec10[0];
			fRec12[1] = fRec12[0];
			fRec112[1] = fRec112[0];
			fRec113[1] = fRec113[0];
			fRec11[1] = fRec11[0];
			fRec13[1] = fRec13[0];
			fRec96[1] = fRec96[0];
			fRec109[2] = fRec109[1]; fRec109[1] = fRec109[0];
			fRec110[2] = fRec110[1]; fRec110[1] = fRec110[0];
			fRec111[1] = fRec111[0];
			fRec103[2] = fRec103[1]; fRec103[1] = fRec103[0];
			fRec104[2] = fRec104[1]; fRec104[1] = fRec104[0];
			fRec105[1] = fRec105[0];
			fVec31[1] = fVec31[0];
			fRec106[2] = fRec106[1]; fRec106[1] = fRec106[0];
			fRec107[2] = fRec107[1]; fRec107[1] = fRec107[0];
			fRec108[1] = fRec108[0];
			fRec97[2] = fRec97[1]; fRec97[1] = fRec97[0];
			fRec98[2] = fRec98[1]; fRec98[1] = fRec98[0];
			fRec99[2] = fRec99[1]; fRec99[1] = fRec99[0];
			fRec100[1] = fRec100[0];
			fRec101[1] = fRec101[0];
			fRec102[1] = fRec102[0];
			fRec14[1] = fRec14[0];
			fRec93[2] = fRec93[1]; fRec93[1] = fRec93[0];
			fRec94[2] = fRec94[1]; fRec94[1] = fRec94[0];
			fRec95[1] = fRec95[0];
			fRec87[2] = fRec87[1]; fRec87[1] = fRec87[0];
			fRec88[2] = fRec88[1]; fRec88[1] = fRec88[0];
			fRec89[1] = fRec89[0];
			fVec27[1] = fVec27[0];
			fRec90[2] = fRec90[1]; fRec90[1] = fRec90[0];
			fRec91[2] = fRec91[1]; fRec91[1] = fRec91[0];
			fRec92[1] = fRec92[0];
			fRec15[2] = fRec15[1]; fRec15[1] = fRec15[0];
			fRec16[2] = fRec16[1]; fRec16[1] = fRec16[0];
			fRec17[2] = fRec17[1]; fRec17[1] = fRec17[0];
			fRec18[1] = fRec18[0];
			fRec19[1] = fRec19[0];
			fRec86[1] = fRec86[0];
			fRec21[1] = fRec21[0];
			fRec84[1] = fRec84[0];
			fRec85[1] = fRec85[0];
			fRec20[1] = fRec20[0];
			fRec22[1] = fRec22[0];
			fRec83[1] = fRec83[0];
			fRec24[1] = fRec24[0];
			fRec81[1] = fRec81[0];
			fRec82[1] = fRec82[0];
			fRec23[1] = fRec23[0];
			fRec25[1] = fRec25[0];
			fRec80[1] = fRec80[0];
			fRec27[1] = fRec27[0];
			fRec78[1] = fRec78[0];
			fRec79[1] = fRec79[0];
			fRec26[1] = fRec26[0];
			fRec28[1] = fRec28[0];
			fRec77[1] = fRec77[0];
			fRec30[1] = fRec30[0];
			fRec75[1] = fRec75[0];
			fRec76[1] = fRec76[0];
			fRec29[1] = fRec29[0];
			fRec31[1] = fRec31[0];
			fRec74[1] = fRec74[0];
			fRec33[1] = fRec33[0];
			fRec72[1] = fRec72[0];
			fRec73[1] = fRec73[0];
			fRec32[1] = fRec32[0];
			fRec34[1] = fRec34[0];
			fRec71[1] = fRec71[0];
			fRec69[1] = fRec69[0];
			fRec70[1] = fRec70[0];
			fRec35[1] = fRec35[0];
			fRec68[1] = fRec68[0];
			fRec67[1] = fRec67[0];
			fRec66[1] = fRec66[0];
			fRec37[1] = fRec37[0];
			fRec64[1] = fRec64[0];
			fRec65[1] = fRec65[0];
			fRec36[1] = fRec36[0];
			fRec38[1] = fRec38[0];
			fRec63[1] = fRec63[0];
			fRec40[1] = fRec40[0];
			fRec61[1] = fRec61[0];
			fRec62[1] = fRec62[0];
			fRec39[1] = fRec39[0];
			fRec41[1] = fRec41[0];
			fRec60[1] = fRec60[0];
			fRec43[1] = fRec43[0];
			fRec58[1] = fRec58[0];
			fRec59[1] = fRec59[0];
			fRec42[1] = fRec42[0];
			fRec44[1] = fRec44[0];
			fRec47[1] = fRec47[0];
			fRec56[1] = fRec56[0];
			fRec57[1] = fRec57[0];
			fRec46[1] = fRec46[0];
			fRec48[1] = fRec48[0];
			fRec55[1] = fRec55[0];
			fRec50[1] = fRec50[0];
			fRec53[1] = fRec53[0];
			fRec54[1] = fRec54[0];
			fRec49[1] = fRec49[0];
			fRec51[1] = fRec51[0];
			fRec52[1] = fRec52[0];
			IOTA = IOTA+1;
			fRec45[1] = fRec45[0];
			iVec0[1] = iVec0[0];
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
