//----------------------------------------------------------
// name: "JPverbRaw"
// version: "1.1"
// author: "Julian Parker, bug fixes by Till Bovermann"
// license: "GPL2+"
// copyright: "(c) Julian Parker 2013"
//
// Code generated with Faust 0.9.97 (http://faust.grame.fr)
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
	float 	fConst0;
	float 	fConst1;
	FAUSTFLOAT 	fslider4;
	FAUSTFLOAT 	fslider5;
	FAUSTFLOAT 	fslider6;
	int 	IOTA;
	float 	fVec1[16384];
	float 	fRec53[2];
	float 	fRec52[2];
	float 	fRec50[2];
	float 	fVec2[16384];
	float 	fRec55[2];
	float 	fRec54[2];
	float 	fRec51[2];
	float 	fVec3[16384];
	float 	fRec56[2];
	float 	fRec49[2];
	float 	fRec47[2];
	float 	fVec4[16384];
	float 	fRec58[2];
	float 	fRec57[2];
	float 	fRec48[2];
	float 	fVec5[16384];
	float 	fRec59[2];
	float 	fRec46[2];
	float 	fRec44[2];
	float 	fVec6[16384];
	float 	fRec61[2];
	float 	fRec60[2];
	float 	fRec45[2];
	float 	fVec7[16384];
	float 	fRec62[2];
	float 	fRec43[2];
	float 	fRec41[2];
	float 	fVec8[16384];
	float 	fRec64[2];
	float 	fRec63[2];
	float 	fRec42[2];
	float 	fVec9[16384];
	float 	fRec65[2];
	float 	fRec40[2];
	float 	fRec38[2];
	float 	fVec10[16384];
	float 	fRec67[2];
	float 	fRec66[2];
	float 	fRec39[2];
	float 	fVec11[1024];
	FAUSTFLOAT 	fslider7;
	float 	fConst2;
	FAUSTFLOAT 	fslider8;
	float 	fRec68[2];
	float 	fRec69[2];
	float 	fVec12[16384];
	float 	fRec70[2];
	float 	fRec37[2];
	float 	fVec13[1024];
	float 	fVec14[16384];
	float 	fRec72[2];
	float 	fRec71[2];
	float 	fVec15[16384];
	float 	fRec73[2];
	float 	fRec36[2];
	float 	fRec34[2];
	float 	fVec16[16384];
	float 	fRec75[2];
	float 	fRec74[2];
	float 	fRec35[2];
	float 	fVec17[16384];
	float 	fRec76[2];
	float 	fRec33[2];
	float 	fRec31[2];
	float 	fVec18[16384];
	float 	fRec78[2];
	float 	fRec77[2];
	float 	fRec32[2];
	float 	fVec19[16384];
	float 	fRec79[2];
	float 	fRec30[2];
	float 	fRec28[2];
	float 	fVec20[16384];
	float 	fRec81[2];
	float 	fRec80[2];
	float 	fRec29[2];
	float 	fVec21[16384];
	float 	fRec82[2];
	float 	fRec27[2];
	float 	fRec25[2];
	float 	fVec22[16384];
	float 	fRec84[2];
	float 	fRec83[2];
	float 	fRec26[2];
	float 	fVec23[16384];
	float 	fRec85[2];
	float 	fRec24[2];
	float 	fRec22[2];
	float 	fVec24[16384];
	float 	fRec87[2];
	float 	fRec86[2];
	float 	fRec23[2];
	float 	fVec25[16384];
	float 	fVec26[16384];
	float 	fRec88[2];
	float 	fRec21[2];
	float 	fRec20[2];
	float 	fRec19[3];
	float 	fRec18[3];
	float 	fVec27[2];
	float 	fRec17[2];
	float 	fRec16[3];
	float 	fRec15[3];
	FAUSTFLOAT 	fslider9;
	float 	fRec91[2];
	float 	fRec90[3];
	float 	fRec89[3];
	FAUSTFLOAT 	fslider10;
	float 	fRec95[2];
	float 	fRec94[3];
	float 	fRec93[3];
	float 	fRec92[3];
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
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.0");
		m->declare("name", "JPverbRaw");
		m->declare("version", "1.1");
		m->declare("author", "Julian Parker, bug fixes by Till Bovermann");
		m->declare("license", "GPL2+");
		m->declare("copyright", "(c) Julian Parker 2013");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.0");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("analyzers.lib/name", "Faust Analyzer Library");
		m->declare("analyzers.lib/version", "0.0");
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
		fslider4 = 5e+02f;
		fslider5 = 1.0f;
		fslider6 = 2e+03f;
		fslider7 = 0.1f;
		fslider8 = 2.0f;
		fslider9 = 1.0f;
		fslider10 = 1.0f;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) iVec0[i] = 0;
		IOTA = 0;
		for (int i=0; i<16384; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<16384; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<16384; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<16384; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<16384; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<16384; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<16384; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<16384; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<16384; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<16384; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<1024; i++) fVec11[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<16384; i++) fVec12[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<1024; i++) fVec13[i] = 0;
		for (int i=0; i<16384; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<16384; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<16384; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<16384; i++) fVec17[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<16384; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<16384; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<16384; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<16384; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<16384; i++) fVec22[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<16384; i++) fVec23[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<16384; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<16384; i++) fVec25[i] = 0;
		for (int i=0; i<16384; i++) fVec26[i] = 0;
		for (int i=0; i<2; i++) fRec88[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<3; i++) fRec19[i] = 0;
		for (int i=0; i<3; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fVec27[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<3; i++) fRec16[i] = 0;
		for (int i=0; i<3; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec91[i] = 0;
		for (int i=0; i<3; i++) fRec90[i] = 0;
		for (int i=0; i<3; i++) fRec89[i] = 0;
		for (int i=0; i<2; i++) fRec95[i] = 0;
		for (int i=0; i<3; i++) fRec94[i] = 0;
		for (int i=0; i<3; i++) fRec93[i] = 0;
		for (int i=0; i<3; i++) fRec92[i] = 0;
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
		ui_interface->addHorizontalSlider("highBand", &fslider6, 2e+03f, 1e+03f, 1e+04f, 0.1f);
		ui_interface->addHorizontalSlider("highX", &fslider10, 1.0f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("lowBand", &fslider4, 5e+02f, 1e+02f, 6e+03f, 0.1f);
		ui_interface->addHorizontalSlider("lowX", &fslider9, 1.0f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("mDepth", &fslider7, 0.1f, 0.0f, 1.0f, 0.001f);
		ui_interface->addHorizontalSlider("mFreq", &fslider8, 2.0f, 0.0f, 1e+01f, 0.01f);
		ui_interface->addHorizontalSlider("midX", &fslider5, 1.0f, 0.0f, 1.0f, 0.01f);
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
		float 	fSlow8 = tanf((fConst1 * float(fslider4)));
		float 	fSlow9 = (1.0f / fSlow8);
		float 	fSlow10 = (1.0f / (((fSlow9 + 0.618034f) / fSlow8) + 1));
		float 	fSlow11 = float(fslider5);
		float 	fSlow12 = (1.0f / faustpower<2>(fSlow8));
		float 	fSlow13 = (1.0f / (((fSlow9 + 1.618034f) / fSlow8) + 1));
		float 	fSlow14 = (fSlow9 + 1);
		float 	fSlow15 = (1.0f / fSlow14);
		float 	fSlow16 = (0 - fSlow9);
		float 	fSlow17 = tanf((fConst1 * float(fslider6)));
		float 	fSlow18 = (1.0f / fSlow17);
		float 	fSlow19 = (((fSlow18 + 0.618034f) / fSlow17) + 1);
		float 	fSlow20 = (1.0f / fSlow19);
		float 	fSlow21 = (1.0f / (((fSlow18 + 1.618034f) / fSlow17) + 1));
		float 	fSlow22 = (fSlow18 + 1);
		float 	fSlow23 = (0 - ((1 - fSlow18) / fSlow22));
		float 	fSlow24 = (1.0f / fSlow22);
		int 	iSlow25 = primes((10 * fSlow6));
		float 	fSlow26 = (0.0001f * iSlow25);
		int 	iSlow27 = primes((110 * fSlow6));
		float 	fSlow28 = (0.0001f * iSlow27);
		int 	iSlow29 = primes((40 * fSlow6));
		float 	fSlow30 = (0.0001f * iSlow29);
		int 	iSlow31 = primes((140 * fSlow6));
		float 	fSlow32 = (0.0001f * iSlow31);
		int 	iSlow33 = primes((70 * fSlow6));
		float 	fSlow34 = (0.0001f * iSlow33);
		int 	iSlow35 = primes((170 * fSlow6));
		float 	fSlow36 = (0.0001f * iSlow35);
		int 	iSlow37 = primes((100 * fSlow6));
		float 	fSlow38 = (0.0001f * iSlow37);
		int 	iSlow39 = primes((200 * fSlow6));
		float 	fSlow40 = (0.0001f * iSlow39);
		int 	iSlow41 = primes((130 * fSlow6));
		float 	fSlow42 = (0.0001f * iSlow41);
		int 	iSlow43 = primes((230 * fSlow6));
		float 	fSlow44 = (0.0001f * iSlow43);
		float 	fSlow45 = float(fslider7);
		float 	fSlow46 = (50 * fSlow45);
		float 	fSlow47 = (fConst2 * float(fslider8));
		float 	fSlow48 = sinf(fSlow47);
		float 	fSlow49 = cosf(fSlow47);
		float 	fSlow50 = (0 - fSlow48);
		int 	iSlow51 = primes((54 * fSlow6));
		float 	fSlow52 = (0.005f * iSlow51);
		float 	fSlow53 = (0 - (5e+01f * fSlow45));
		int 	iSlow54 = primes((204 * fSlow6));
		float 	fSlow55 = (0.005f * iSlow54);
		int 	iSlow56 = primes((125 * fSlow6));
		float 	fSlow57 = (0.0001f * iSlow56);
		int 	iSlow58 = primes((25 * fSlow6));
		float 	fSlow59 = (0.0001f * iSlow58);
		int 	iSlow60 = primes((155 * fSlow6));
		float 	fSlow61 = (0.0001f * iSlow60);
		int 	iSlow62 = primes((55 * fSlow6));
		float 	fSlow63 = (0.0001f * iSlow62);
		int 	iSlow64 = primes((185 * fSlow6));
		float 	fSlow65 = (0.0001f * iSlow64);
		int 	iSlow66 = primes((85 * fSlow6));
		float 	fSlow67 = (0.0001f * iSlow66);
		int 	iSlow68 = primes((215 * fSlow6));
		float 	fSlow69 = (0.0001f * iSlow68);
		int 	iSlow70 = primes((115 * fSlow6));
		float 	fSlow71 = (0.0001f * iSlow70);
		int 	iSlow72 = primes((245 * fSlow6));
		float 	fSlow73 = (0.0001f * iSlow72);
		int 	iSlow74 = primes((145 * fSlow6));
		float 	fSlow75 = (0.0001f * iSlow74);
		int 	iSlow76 = primes((134 * fSlow6));
		float 	fSlow77 = (0.005f * iSlow76);
		float 	fSlow78 = (1.0f / faustpower<2>(fSlow17));
		float 	fSlow79 = (2 * (1 - fSlow78));
		float 	fSlow80 = (((fSlow18 + -1.618034f) / fSlow17) + 1);
		float 	fSlow81 = (((fSlow18 + -0.618034f) / fSlow17) + 1);
		float 	fSlow82 = (1.0f / (fSlow8 * fSlow19));
		float 	fSlow83 = (0 - ((1 - fSlow9) / fSlow14));
		float 	fSlow84 = (2 * (1 - fSlow12));
		float 	fSlow85 = (((fSlow9 + -1.618034f) / fSlow8) + 1);
		float 	fSlow86 = (2 * (0 - fSlow12));
		float 	fSlow87 = (((fSlow9 + -0.618034f) / fSlow8) + 1);
		float 	fSlow88 = float(fslider9);
		float 	fSlow89 = float(fslider10);
		float 	fSlow90 = (2 * (0 - fSlow78));
		float 	fSlow91 = (0 - fSlow18);
		float 	fSlow92 = (1.0f / (((fSlow9 + 1.618034f) / fSlow8) + 1));
		float 	fSlow93 = (((fSlow9 + -1.618034f) / fSlow8) + 1);
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
			float fTemp0 = ((0.760314f * fRec0[1]) - (0.64955574f * fRec50[1]));
			float fTemp1 = ((0.760314f * fRec1[1]) - (0.64955574f * fRec51[1]));
			float fTemp2 = ((0.70710677f * fTemp0) - (0.70710677f * fTemp1));
			fVec1[IOTA&16383] = fTemp2;
			int iTemp3 = (1 - iVec0[1]);
			fRec53[0] = (fSlow26 + (0.9999f * ((iSlow25 * iTemp3) + fRec53[1])));
			float fTemp4 = (fRec53[0] + -1.49999f);
			int iTemp5 = int(min(8192, max(0, int(fTemp4))));
			float fTemp6 = floorf(fTemp4);
			float fTemp7 = (fTemp6 + (2.0f - fRec53[0]));
			float fTemp8 = (fRec53[0] - fTemp6);
			fRec52[0] = (fVec1[(IOTA-int((iTemp5 + 1)))&16383] + ((fRec52[1] * (0 - (fTemp7 / fTemp8))) + ((fTemp7 * fVec1[(IOTA-iTemp5)&16383]) / fTemp8)));
			fRec50[0] = fRec52[0];
			float fTemp9 = ((0.70710677f * fTemp0) + (0.70710677f * fTemp1));
			fVec2[IOTA&16383] = fTemp9;
			fRec55[0] = (fSlow28 + (0.9999f * ((iSlow27 * iTemp3) + fRec55[1])));
			float fTemp10 = (fRec55[0] + -1.49999f);
			int iTemp11 = int(min(8192, max(0, int(fTemp10))));
			float fTemp12 = floorf(fTemp10);
			float fTemp13 = (fTemp12 + (2.0f - fRec55[0]));
			float fTemp14 = (fRec55[0] - fTemp12);
			fRec54[0] = (fVec2[(IOTA-int((iTemp11 + 1)))&16383] + ((fRec54[1] * (0 - (fTemp13 / fTemp14))) + ((fTemp13 * fVec2[(IOTA-iTemp11)&16383]) / fTemp14)));
			fRec51[0] = fRec54[0];
			float fTemp15 = ((0.760314f * fRec50[1]) + (0.64955574f * fRec0[1]));
			float fTemp16 = ((0.760314f * fTemp15) - (0.64955574f * fRec47[1]));
			float fTemp17 = ((0.760314f * fRec51[1]) + (0.64955574f * fRec1[1]));
			float fTemp18 = ((0.760314f * fTemp17) - (0.64955574f * fRec48[1]));
			float fTemp19 = ((0.70710677f * fTemp16) - (0.70710677f * fTemp18));
			fVec3[IOTA&16383] = fTemp19;
			fRec56[0] = (fSlow30 + (0.9999f * ((iSlow29 * iTemp3) + fRec56[1])));
			float fTemp20 = (fRec56[0] + -1.49999f);
			int iTemp21 = int(min(8192, max(0, int(fTemp20))));
			float fTemp22 = floorf(fTemp20);
			float fTemp23 = (fTemp22 + (2.0f - fRec56[0]));
			float fTemp24 = (fRec56[0] - fTemp22);
			fRec49[0] = (fVec3[(IOTA-int((iTemp21 + 1)))&16383] + ((fRec49[1] * (0 - (fTemp23 / fTemp24))) + ((fTemp23 * fVec3[(IOTA-iTemp21)&16383]) / fTemp24)));
			fRec47[0] = fRec49[0];
			float fTemp25 = ((0.70710677f * fTemp16) + (0.70710677f * fTemp18));
			fVec4[IOTA&16383] = fTemp25;
			fRec58[0] = (fSlow32 + (0.9999f * ((iSlow31 * iTemp3) + fRec58[1])));
			float fTemp26 = (fRec58[0] + -1.49999f);
			int iTemp27 = int(min(8192, max(0, int(fTemp26))));
			float fTemp28 = floorf(fTemp26);
			float fTemp29 = (fTemp28 + (2.0f - fRec58[0]));
			float fTemp30 = (fRec58[0] - fTemp28);
			fRec57[0] = (fVec4[(IOTA-int((iTemp27 + 1)))&16383] + ((fRec57[1] * (0 - (fTemp29 / fTemp30))) + ((fTemp29 * fVec4[(IOTA-iTemp27)&16383]) / fTemp30)));
			fRec48[0] = fRec57[0];
			float fTemp31 = ((0.760314f * fRec47[1]) + (0.64955574f * fTemp15));
			float fTemp32 = ((0.760314f * fTemp31) - (0.64955574f * fRec44[1]));
			float fTemp33 = ((0.760314f * fRec48[1]) + (0.64955574f * fTemp17));
			float fTemp34 = ((0.760314f * fTemp33) - (0.64955574f * fRec45[1]));
			float fTemp35 = ((0.70710677f * fTemp32) - (0.70710677f * fTemp34));
			fVec5[IOTA&16383] = fTemp35;
			fRec59[0] = (fSlow34 + (0.9999f * ((iSlow33 * iTemp3) + fRec59[1])));
			float fTemp36 = (fRec59[0] + -1.49999f);
			int iTemp37 = int(min(8192, max(0, int(fTemp36))));
			float fTemp38 = floorf(fTemp36);
			float fTemp39 = (fTemp38 + (2.0f - fRec59[0]));
			float fTemp40 = (fRec59[0] - fTemp38);
			fRec46[0] = (fVec5[(IOTA-int((iTemp37 + 1)))&16383] + ((fRec46[1] * (0 - (fTemp39 / fTemp40))) + ((fTemp39 * fVec5[(IOTA-iTemp37)&16383]) / fTemp40)));
			fRec44[0] = fRec46[0];
			float fTemp41 = ((0.70710677f * fTemp32) + (0.70710677f * fTemp34));
			fVec6[IOTA&16383] = fTemp41;
			fRec61[0] = (fSlow36 + (0.9999f * ((iSlow35 * iTemp3) + fRec61[1])));
			float fTemp42 = (fRec61[0] + -1.49999f);
			int iTemp43 = int(min(8192, max(0, int(fTemp42))));
			float fTemp44 = floorf(fTemp42);
			float fTemp45 = (fTemp44 + (2.0f - fRec61[0]));
			float fTemp46 = (fRec61[0] - fTemp44);
			fRec60[0] = (fVec6[(IOTA-int((iTemp43 + 1)))&16383] + ((fRec60[1] * (0 - (fTemp45 / fTemp46))) + ((fTemp45 * fVec6[(IOTA-iTemp43)&16383]) / fTemp46)));
			fRec45[0] = fRec60[0];
			float fTemp47 = ((0.760314f * fRec44[1]) + (0.64955574f * fTemp31));
			float fTemp48 = ((0.760314f * fTemp47) - (0.64955574f * fRec41[1]));
			float fTemp49 = ((0.760314f * fRec45[1]) + (0.64955574f * fTemp33));
			float fTemp50 = ((0.760314f * fTemp49) - (0.64955574f * fRec42[1]));
			float fTemp51 = ((0.70710677f * fTemp48) - (0.70710677f * fTemp50));
			fVec7[IOTA&16383] = fTemp51;
			fRec62[0] = (fSlow38 + (0.9999f * ((iSlow37 * iTemp3) + fRec62[1])));
			float fTemp52 = (fRec62[0] + -1.49999f);
			int iTemp53 = int(min(8192, max(0, int(fTemp52))));
			float fTemp54 = floorf(fTemp52);
			float fTemp55 = (fTemp54 + (2.0f - fRec62[0]));
			float fTemp56 = (fRec62[0] - fTemp54);
			fRec43[0] = (fVec7[(IOTA-int((iTemp53 + 1)))&16383] + ((fRec43[1] * (0 - (fTemp55 / fTemp56))) + ((fTemp55 * fVec7[(IOTA-iTemp53)&16383]) / fTemp56)));
			fRec41[0] = fRec43[0];
			float fTemp57 = ((0.70710677f * fTemp48) + (0.70710677f * fTemp50));
			fVec8[IOTA&16383] = fTemp57;
			fRec64[0] = (fSlow40 + (0.9999f * ((iSlow39 * iTemp3) + fRec64[1])));
			float fTemp58 = (fRec64[0] + -1.49999f);
			int iTemp59 = int(min(8192, max(0, int(fTemp58))));
			float fTemp60 = floorf(fTemp58);
			float fTemp61 = (fTemp60 + (2.0f - fRec64[0]));
			float fTemp62 = (fRec64[0] - fTemp60);
			fRec63[0] = (fVec8[(IOTA-int((iTemp59 + 1)))&16383] + ((fRec63[1] * (0 - (fTemp61 / fTemp62))) + ((fTemp61 * fVec8[(IOTA-iTemp59)&16383]) / fTemp62)));
			fRec42[0] = fRec63[0];
			float fTemp63 = ((0.760314f * fRec41[1]) + (0.64955574f * fTemp47));
			float fTemp64 = ((0.760314f * fTemp63) - (0.64955574f * fRec38[1]));
			float fTemp65 = ((0.760314f * fRec42[1]) + (0.64955574f * fTemp49));
			float fTemp66 = ((0.760314f * fTemp65) - (0.64955574f * fRec39[1]));
			float fTemp67 = ((0.70710677f * fTemp64) - (0.70710677f * fTemp66));
			fVec9[IOTA&16383] = fTemp67;
			fRec65[0] = (fSlow42 + (0.9999f * ((iSlow41 * iTemp3) + fRec65[1])));
			float fTemp68 = (fRec65[0] + -1.49999f);
			int iTemp69 = int(min(8192, max(0, int(fTemp68))));
			float fTemp70 = floorf(fTemp68);
			float fTemp71 = (fTemp70 + (2.0f - fRec65[0]));
			float fTemp72 = (fRec65[0] - fTemp70);
			fRec40[0] = (fVec9[(IOTA-int((iTemp69 + 1)))&16383] + ((fRec40[1] * (0 - (fTemp71 / fTemp72))) + ((fTemp71 * fVec9[(IOTA-iTemp69)&16383]) / fTemp72)));
			fRec38[0] = fRec40[0];
			float fTemp73 = ((0.70710677f * fTemp64) + (0.70710677f * fTemp66));
			fVec10[IOTA&16383] = fTemp73;
			fRec67[0] = (fSlow44 + (0.9999f * ((iSlow43 * iTemp3) + fRec67[1])));
			float fTemp74 = (fRec67[0] + -1.49999f);
			int iTemp75 = int(min(8192, max(0, int(fTemp74))));
			float fTemp76 = floorf(fTemp74);
			float fTemp77 = (fTemp76 + (2.0f - fRec67[0]));
			float fTemp78 = (fRec67[0] - fTemp76);
			fRec66[0] = (fVec10[(IOTA-int((iTemp75 + 1)))&16383] + ((fRec66[1] * (0 - (fTemp77 / fTemp78))) + ((fTemp77 * fVec10[(IOTA-iTemp75)&16383]) / fTemp78)));
			fRec39[0] = fRec66[0];
			float fTemp79 = ((0.760314f * fRec38[1]) + (0.64955574f * fTemp63));
			fVec11[IOTA&1023] = fTemp79;
			fRec68[0] = ((fSlow48 * fRec69[1]) + (fSlow49 * fRec68[1]));
			fRec69[0] = (((fSlow49 * fRec69[1]) + (fSlow50 * fRec68[1])) + iTemp3);
			float fTemp80 = (fSlow46 * (fRec69[0] + 1));
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
			fRec70[0] = (fSlow52 + (0.995f * ((iSlow51 * iTemp3) + fRec70[1])));
			float fTemp107 = (fRec70[0] + -1.49999f);
			int iTemp108 = int(min(8192, max(0, int(fTemp107))));
			float fTemp109 = floorf(fTemp107);
			float fTemp110 = (fTemp109 + (2.0f - fRec70[0]));
			float fTemp111 = (fRec70[0] - fTemp109);
			fRec37[0] = (fVec12[(IOTA-int((iTemp108 + 1)))&16383] + ((fRec37[1] * (0 - (fTemp110 / fTemp111))) + ((fTemp110 * fVec12[(IOTA-iTemp108)&16383]) / fTemp111)));
			float fTemp112 = ((0.760314f * fRec37[0]) - (0.64955574f * fRec34[1]));
			float fTemp113 = ((0.760314f * fRec39[1]) + (0.64955574f * fTemp65));
			fVec13[IOTA&1023] = fTemp113;
			float fTemp114 = (fSlow53 * fRec69[0]);
			float fTemp115 = (fSlow46 + (fTemp114 + 3.500005f));
			int iTemp116 = int(fTemp115);
			float fTemp117 = floorf(fTemp115);
			float fTemp118 = (fSlow46 + (fTemp114 + (1.0f - fTemp117)));
			float fTemp119 = (fSlow46 + (fTemp114 + (4.0f - fTemp117)));
			float fTemp120 = (fSlow46 + (fTemp114 + (3.0f - fTemp117)));
			float fTemp121 = (fSlow46 + (fTemp114 + (2.0f - fTemp117)));
			float fTemp122 = (((((0.16666667f * (((fVec13[(IOTA-int(min(512, max(0, (iTemp116 + 3)))))&1023] * (0 - fTemp118)) * fTemp119) * fTemp120)) + ((((fVec13[(IOTA-int(min(512, max(0, (iTemp116 + 1)))))&1023] * (0 - fTemp120)) * (0 - (0.5f * fTemp121))) * (0 - (0.33333334f * fTemp118))) + (0.5f * (((fTemp119 * fVec13[(IOTA-int(min(512, max(0, (iTemp116 + 2)))))&1023]) * (0 - fTemp121)) * (0 - (0.5f * fTemp118)))))) + (0.041666668f * (((fVec13[(IOTA-int(min(512, max(0, (iTemp116 + 4)))))&1023] * fTemp119) * fTemp120) * fTemp121))) * (fSlow46 + (fTemp114 + (5.0f - fTemp117)))) + ((((fVec13[(IOTA-int(min(512, max(0, iTemp116))))&1023] * (0 - fTemp119)) * (0 - (0.5f * fTemp120))) * (0 - (0.33333334f * fTemp121))) * (0 - (0.25f * fTemp118))));
			fVec14[IOTA&16383] = fTemp122;
			fRec72[0] = (fSlow55 + (0.995f * ((iSlow54 * iTemp3) + fRec72[1])));
			float fTemp123 = (fRec72[0] + -1.49999f);
			int iTemp124 = int(min(8192, max(0, int(fTemp123))));
			float fTemp125 = floorf(fTemp123);
			float fTemp126 = (fTemp125 + (2.0f - fRec72[0]));
			float fTemp127 = (fRec72[0] - fTemp125);
			fRec71[0] = ((((fVec14[(IOTA-iTemp124)&16383] * fTemp126) / fTemp127) + fVec14[(IOTA-int((iTemp124 + 1)))&16383]) + (fRec71[1] * (0 - (fTemp126 / fTemp127))));
			float fTemp128 = ((0.760314f * fRec71[0]) - (0.64955574f * fRec35[1]));
			float fTemp129 = ((0.70710677f * fTemp112) - (0.70710677f * fTemp128));
			fVec15[IOTA&16383] = fTemp129;
			fRec73[0] = (fSlow57 + (0.9999f * ((iSlow56 * iTemp3) + fRec73[1])));
			float fTemp130 = (fRec73[0] + -1.49999f);
			int iTemp131 = int(min(8192, max(0, int(fTemp130))));
			float fTemp132 = floorf(fTemp130);
			float fTemp133 = (fTemp132 + (2.0f - fRec73[0]));
			float fTemp134 = (fRec73[0] - fTemp132);
			fRec36[0] = (fVec15[(IOTA-int((iTemp131 + 1)))&16383] + ((fRec36[1] * (0 - (fTemp133 / fTemp134))) + ((fTemp133 * fVec15[(IOTA-iTemp131)&16383]) / fTemp134)));
			fRec34[0] = fRec36[0];
			float fTemp135 = ((0.70710677f * fTemp112) + (0.70710677f * fTemp128));
			fVec16[IOTA&16383] = fTemp135;
			fRec75[0] = (fSlow59 + (0.9999f * ((iSlow58 * iTemp3) + fRec75[1])));
			float fTemp136 = (fRec75[0] + -1.49999f);
			int iTemp137 = int(min(8192, max(0, int(fTemp136))));
			float fTemp138 = floorf(fTemp136);
			float fTemp139 = (fTemp138 + (2.0f - fRec75[0]));
			float fTemp140 = (fRec75[0] - fTemp138);
			fRec74[0] = (fVec16[(IOTA-int((iTemp137 + 1)))&16383] + ((fRec74[1] * (0 - (fTemp139 / fTemp140))) + ((fTemp139 * fVec16[(IOTA-iTemp137)&16383]) / fTemp140)));
			fRec35[0] = fRec74[0];
			float fTemp141 = ((0.760314f * fRec34[1]) + (0.64955574f * fRec37[0]));
			float fTemp142 = ((0.760314f * fTemp141) - (0.64955574f * fRec31[1]));
			float fTemp143 = ((0.760314f * fRec35[1]) + (0.64955574f * fRec71[0]));
			float fTemp144 = ((0.760314f * fTemp143) - (0.64955574f * fRec32[1]));
			float fTemp145 = ((0.70710677f * fTemp142) - (0.70710677f * fTemp144));
			fVec17[IOTA&16383] = fTemp145;
			fRec76[0] = (fSlow61 + (0.9999f * ((iSlow60 * iTemp3) + fRec76[1])));
			float fTemp146 = (fRec76[0] + -1.49999f);
			int iTemp147 = int(min(8192, max(0, int(fTemp146))));
			float fTemp148 = floorf(fTemp146);
			float fTemp149 = (fTemp148 + (2.0f - fRec76[0]));
			float fTemp150 = (fRec76[0] - fTemp148);
			fRec33[0] = (fVec17[(IOTA-int((iTemp147 + 1)))&16383] + ((fRec33[1] * (0 - (fTemp149 / fTemp150))) + ((fTemp149 * fVec17[(IOTA-iTemp147)&16383]) / fTemp150)));
			fRec31[0] = fRec33[0];
			float fTemp151 = ((0.70710677f * fTemp142) + (0.70710677f * fTemp144));
			fVec18[IOTA&16383] = fTemp151;
			fRec78[0] = (fSlow63 + (0.9999f * ((iSlow62 * iTemp3) + fRec78[1])));
			float fTemp152 = (fRec78[0] + -1.49999f);
			int iTemp153 = int(min(8192, max(0, int(fTemp152))));
			int iTemp154 = int((iTemp153 + 1));
			float fTemp155 = floorf(fTemp152);
			float fTemp156 = (fTemp155 + (2.0f - fRec78[0]));
			float fTemp157 = (fRec78[0] - fTemp155);
			float fTemp158 = (0 - (fTemp156 / fTemp157));
			fRec77[0] = (fVec18[(IOTA-iTemp154)&16383] + ((fTemp158 * fRec77[1]) + ((fTemp156 * fVec18[(IOTA-iTemp153)&16383]) / fTemp157)));
			fRec32[0] = fRec77[0];
			float fTemp159 = ((0.760314f * fRec31[1]) + (0.64955574f * fTemp141));
			float fTemp160 = ((0.760314f * fTemp159) - (0.64955574f * fRec28[1]));
			float fTemp161 = ((0.760314f * fRec32[1]) + (0.64955574f * fTemp143));
			float fTemp162 = ((0.760314f * fTemp161) - (0.64955574f * fRec29[1]));
			float fTemp163 = ((0.70710677f * fTemp160) - (0.70710677f * fTemp162));
			fVec19[IOTA&16383] = fTemp163;
			fRec79[0] = (fSlow65 + (0.9999f * ((iSlow64 * iTemp3) + fRec79[1])));
			float fTemp164 = (fRec79[0] + -1.49999f);
			int iTemp165 = int(min(8192, max(0, int(fTemp164))));
			float fTemp166 = floorf(fTemp164);
			float fTemp167 = (fTemp166 + (2.0f - fRec79[0]));
			float fTemp168 = (fRec79[0] - fTemp166);
			fRec30[0] = (fVec19[(IOTA-int((iTemp165 + 1)))&16383] + ((fRec30[1] * (0 - (fTemp167 / fTemp168))) + ((fTemp167 * fVec19[(IOTA-iTemp165)&16383]) / fTemp168)));
			fRec28[0] = fRec30[0];
			float fTemp169 = ((0.70710677f * fTemp160) + (0.70710677f * fTemp162));
			fVec20[IOTA&16383] = fTemp169;
			fRec81[0] = (fSlow67 + (0.9999f * ((iSlow66 * iTemp3) + fRec81[1])));
			float fTemp170 = (fRec81[0] + -1.49999f);
			int iTemp171 = int(min(8192, max(0, int(fTemp170))));
			int iTemp172 = int((iTemp171 + 1));
			float fTemp173 = floorf(fTemp170);
			float fTemp174 = (fTemp173 + (2.0f - fRec81[0]));
			float fTemp175 = (fRec81[0] - fTemp173);
			float fTemp176 = (0 - (fTemp174 / fTemp175));
			fRec80[0] = (fVec20[(IOTA-iTemp172)&16383] + ((fRec80[1] * fTemp176) + ((fTemp174 * fVec20[(IOTA-iTemp171)&16383]) / fTemp175)));
			fRec29[0] = fRec80[0];
			float fTemp177 = ((0.760314f * fRec28[1]) + (0.64955574f * fTemp159));
			float fTemp178 = ((0.760314f * fTemp177) - (0.64955574f * fRec25[1]));
			float fTemp179 = ((0.760314f * fRec29[1]) + (0.64955574f * fTemp161));
			float fTemp180 = ((0.760314f * fTemp179) - (0.64955574f * fRec26[1]));
			float fTemp181 = ((0.70710677f * fTemp178) - (0.70710677f * fTemp180));
			fVec21[IOTA&16383] = fTemp181;
			fRec82[0] = (fSlow69 + (0.9999f * ((iSlow68 * iTemp3) + fRec82[1])));
			float fTemp182 = (fRec82[0] + -1.49999f);
			int iTemp183 = int(min(8192, max(0, int(fTemp182))));
			int iTemp184 = int((iTemp183 + 1));
			float fTemp185 = floorf(fTemp182);
			float fTemp186 = (fTemp185 + (2.0f - fRec82[0]));
			float fTemp187 = (fRec82[0] - fTemp185);
			float fTemp188 = (0 - (fTemp186 / fTemp187));
			fRec27[0] = (fVec21[(IOTA-iTemp184)&16383] + ((fTemp188 * fRec27[1]) + ((fTemp186 * fVec21[(IOTA-iTemp183)&16383]) / fTemp187)));
			fRec25[0] = fRec27[0];
			float fTemp189 = ((0.70710677f * fTemp178) + (0.70710677f * fTemp180));
			fVec22[IOTA&16383] = fTemp189;
			fRec84[0] = (fSlow71 + (0.9999f * ((iSlow70 * iTemp3) + fRec84[1])));
			float fTemp190 = (fRec84[0] + -1.49999f);
			int iTemp191 = int(min(8192, max(0, int(fTemp190))));
			int iTemp192 = int((iTemp191 + 1));
			float fTemp193 = floorf(fTemp190);
			float fTemp194 = (fTemp193 + (2.0f - fRec84[0]));
			float fTemp195 = (fRec84[0] - fTemp193);
			float fTemp196 = (0 - (fTemp194 / fTemp195));
			fRec83[0] = (fVec22[(IOTA-iTemp192)&16383] + ((fRec83[1] * fTemp196) + ((fVec22[(IOTA-iTemp191)&16383] * fTemp194) / fTemp195)));
			fRec26[0] = fRec83[0];
			float fTemp197 = ((0.760314f * fRec25[1]) + (0.64955574f * fTemp177));
			float fTemp198 = ((0.760314f * fTemp197) - (0.64955574f * fRec22[1]));
			float fTemp199 = ((0.760314f * fRec26[1]) + (0.64955574f * fTemp179));
			float fTemp200 = ((0.760314f * fTemp199) - (0.64955574f * fRec23[1]));
			float fTemp201 = ((0.70710677f * fTemp198) - (0.70710677f * fTemp200));
			fVec23[IOTA&16383] = fTemp201;
			fRec85[0] = (fSlow73 + (0.9999f * ((iSlow72 * iTemp3) + fRec85[1])));
			float fTemp202 = (fRec85[0] + -1.49999f);
			int iTemp203 = int(min(8192, max(0, int(fTemp202))));
			float fTemp204 = floorf(fTemp202);
			float fTemp205 = (fTemp204 + (2.0f - fRec85[0]));
			float fTemp206 = (fRec85[0] - fTemp204);
			fRec24[0] = (fVec23[(IOTA-int((iTemp203 + 1)))&16383] + ((fRec24[1] * (0 - (fTemp205 / fTemp206))) + ((fTemp205 * fVec23[(IOTA-iTemp203)&16383]) / fTemp206)));
			fRec22[0] = fRec24[0];
			float fTemp207 = ((0.70710677f * fTemp198) + (0.70710677f * fTemp200));
			fVec24[IOTA&16383] = fTemp207;
			fRec87[0] = (fSlow75 + (0.9999f * ((iSlow74 * iTemp3) + fRec87[1])));
			float fTemp208 = (fRec87[0] + -1.49999f);
			int iTemp209 = int(min(8192, max(0, int(fTemp208))));
			int iTemp210 = int((iTemp209 + 1));
			float fTemp211 = floorf(fTemp208);
			float fTemp212 = (fTemp211 + (2.0f - fRec87[0]));
			float fTemp213 = (fRec87[0] - fTemp211);
			float fTemp214 = (0 - (fTemp212 / fTemp213));
			fRec86[0] = (fVec24[(IOTA-iTemp210)&16383] + ((fRec86[1] * fTemp214) + ((fTemp212 * fVec24[(IOTA-iTemp209)&16383]) / fTemp213)));
			fRec23[0] = fRec86[0];
			float fTemp215 = ((0.760314f * fRec22[1]) + (0.64955574f * fTemp197));
			fVec25[IOTA&16383] = fTemp215;
			float fTemp216 = (fSlow46 * (fRec68[0] + 1));
			float fTemp217 = (fTemp216 + 3.500005f);
			int iTemp218 = int(fTemp217);
			int iTemp219 = max(0, iTemp218);
			float fTemp220 = floorf(fTemp217);
			float fTemp221 = (fTemp216 + (4.0f - fTemp220));
			float fTemp222 = (0 - fTemp221);
			float fTemp223 = (fTemp216 + (3.0f - fTemp220));
			float fTemp224 = (0 - (0.5f * fTemp223));
			float fTemp225 = (fTemp216 + (2.0f - fTemp220));
			float fTemp226 = (0 - (0.33333334f * fTemp225));
			float fTemp227 = (fTemp216 + (1.0f - fTemp220));
			float fTemp228 = (0 - (0.25f * fTemp227));
			float fTemp229 = (fTemp216 + (5.0f - fTemp220));
			int iTemp230 = max(0, (iTemp218 + 1));
			float fTemp231 = (0 - fTemp223);
			float fTemp232 = (0 - (0.5f * fTemp225));
			float fTemp233 = (0 - (0.33333334f * fTemp227));
			int iTemp234 = max(0, (iTemp218 + 2));
			float fTemp235 = (0 - fTemp225);
			float fTemp236 = (0 - (0.5f * fTemp227));
			float fTemp237 = (fTemp221 * fTemp223);
			int iTemp238 = max(0, (iTemp218 + 3));
			float fTemp239 = (0 - fTemp227);
			int iTemp240 = max(0, (iTemp218 + 4));
			float fTemp241 = (((((fVec25[(IOTA-int(min(8192, iTemp219)))&16383] * fTemp222) * fTemp224) * fTemp226) * fTemp228) + (fTemp229 * ((((((fVec25[(IOTA-int(min(8192, iTemp230)))&16383] * fTemp231) * fTemp232) * fTemp233) + (0.5f * (((fTemp221 * fVec25[(IOTA-int(min(8192, iTemp234)))&16383]) * fTemp235) * fTemp236))) + (0.16666667f * ((fTemp237 * fVec25[(IOTA-int(min(8192, iTemp238)))&16383]) * fTemp239))) + (0.041666668f * ((fTemp237 * fTemp225) * fVec25[(IOTA-int(min(8192, iTemp240)))&16383])))));
			fVec26[IOTA&16383] = fTemp241;
			fRec88[0] = (fSlow77 + (0.995f * ((iSlow76 * iTemp3) + fRec88[1])));
			float fTemp242 = (fRec88[0] + -1.49999f);
			int iTemp243 = int(min(8192, max(0, int(fTemp242))));
			float fTemp244 = floorf(fTemp242);
			float fTemp245 = (fTemp244 + (2.0f - fRec88[0]));
			float fTemp246 = (fRec88[0] - fTemp244);
			fRec21[0] = (fVec26[(IOTA-int((iTemp243 + 1)))&16383] + ((fRec21[1] * (0 - (fTemp245 / fTemp246))) + ((fTemp245 * fVec26[(IOTA-iTemp243)&16383]) / fTemp246)));
			fRec20[0] = ((fSlow23 * fRec20[1]) + (fSlow24 * (fRec21[1] + fRec21[0])));
			fRec19[0] = (fRec20[0] - (fSlow21 * ((fSlow79 * fRec19[1]) + (fSlow80 * fRec19[2]))));
			fRec18[0] = ((fSlow21 * (fRec19[2] + (fRec19[0] + (2.0f * fRec19[1])))) - (fSlow20 * ((fSlow79 * fRec18[1]) + (fSlow81 * fRec18[2]))));
			float fTemp247 = (((2.0f * fRec18[1]) + fRec18[0]) + fRec18[2]);
			float fTemp248 = (fSlow20 * fTemp247);
			fVec27[0] = fTemp248;
			fRec17[0] = ((fSlow15 * ((fSlow16 * fVec27[1]) + (fSlow82 * fTemp247))) + (fSlow83 * fRec17[1]));
			fRec16[0] = (fRec17[0] - (fSlow13 * ((fSlow84 * fRec16[1]) + (fSlow85 * fRec16[2]))));
			fRec15[0] = ((fSlow13 * (((fSlow12 * fRec16[0]) + (fSlow86 * fRec16[1])) + (fSlow12 * fRec16[2]))) - (fSlow10 * ((fSlow84 * fRec15[1]) + (fSlow87 * fRec15[2]))));
			fRec91[0] = ((fSlow83 * fRec91[1]) + (fSlow15 * (fVec27[0] + fVec27[1])));
			fRec90[0] = (fRec91[0] - (fSlow13 * ((fSlow84 * fRec90[1]) + (fSlow85 * fRec90[2]))));
			fRec89[0] = ((fSlow13 * (fRec90[2] + (fRec90[0] + (2.0f * fRec90[1])))) - (fSlow10 * ((fSlow84 * fRec89[1]) + (fSlow87 * fRec89[2]))));
			fRec95[0] = ((fSlow23 * fRec95[1]) + (fSlow24 * ((fSlow18 * fRec21[0]) + (fSlow91 * fRec21[1]))));
			fRec94[0] = (fRec95[0] - (fSlow21 * ((fSlow79 * fRec94[1]) + (fSlow80 * fRec94[2]))));
			fRec93[0] = ((fSlow21 * (((fSlow90 * fRec94[1]) + (fSlow78 * fRec94[0])) + (fSlow78 * fRec94[2]))) - (fSlow20 * ((fSlow79 * fRec93[1]) + (fSlow81 * fRec93[2]))));
			float fTemp249 = (fSlow84 * fRec92[1]);
			fRec92[0] = ((fSlow20 * (((fSlow90 * fRec93[1]) + (fSlow78 * fRec93[0])) + (fSlow78 * fRec93[2]))) - (fSlow92 * ((fSlow93 * fRec92[2]) + fTemp249)));
			float fTemp250 = ((float)input0[i] + (fSlow7 * ((fSlow10 * ((fSlow11 * (((fSlow12 * fRec15[0]) + (fSlow86 * fRec15[1])) + (fSlow12 * fRec15[2]))) + (fSlow88 * (fRec89[2] + (fRec89[0] + (2.0f * fRec89[1])))))) + (fSlow89 * (fRec92[2] + (fSlow92 * (fTemp249 + (fSlow93 * fRec92[0]))))))));
			fVec28[IOTA&1023] = fTemp250;
			fRec14[0] = ((fSlow4 * fRec14[1]) + (fSlow5 * (((((fVec28[(IOTA-int(min(512, iTemp219)))&1023] * fTemp222) * fTemp224) * fTemp226) * fTemp228) + (((((((fVec28[(IOTA-int(min(512, iTemp230)))&1023] * fTemp231) * fTemp232) * fTemp233) + (0.5f * (((fVec28[(IOTA-int(min(512, iTemp234)))&1023] * fTemp221) * fTemp235) * fTemp236))) + (0.16666667f * (((fVec28[(IOTA-int(min(512, iTemp238)))&1023] * fTemp221) * fTemp223) * fTemp239))) + (0.041666668f * (((fVec28[(IOTA-int(min(512, iTemp240)))&1023] * fTemp221) * fTemp223) * fTemp225))) * fTemp229))));
			float fTemp251 = ((fSlow3 * fRec11[1]) + (fSlow1 * fRec14[0]));
			float fTemp252 = ((0.760314f * fRec23[1]) + (0.64955574f * fTemp199));
			fVec29[IOTA&16383] = fTemp252;
			float fTemp253 = (fSlow53 * fRec68[0]);
			float fTemp254 = (fSlow46 + (fTemp253 + 3.500005f));
			int iTemp255 = int(fTemp254);
			float fTemp256 = floorf(fTemp254);
			float fTemp257 = (fSlow46 + (fTemp253 + (4.0f - fTemp256)));
			float fTemp258 = (fSlow46 + (fTemp253 + (3.0f - fTemp256)));
			float fTemp259 = (fSlow46 + (fTemp253 + (2.0f - fTemp256)));
			float fTemp260 = (fSlow46 + (fTemp253 + (1.0f - fTemp256)));
			float fTemp261 = (fTemp257 * fTemp258);
			float fTemp262 = (((((fVec29[(IOTA-int(min(8192, max(0, iTemp255))))&16383] * (0 - fTemp257)) * (0 - (0.5f * fTemp258))) * (0 - (0.33333334f * fTemp259))) * (0 - (0.25f * fTemp260))) + ((fSlow46 + (fTemp253 + (5.0f - fTemp256))) * ((((((fVec29[(IOTA-int(min(8192, max(0, (iTemp255 + 1)))))&16383] * (0 - fTemp258)) * (0 - (0.5f * fTemp259))) * (0 - (0.33333334f * fTemp260))) + (0.5f * (((fTemp257 * fVec29[(IOTA-int(min(8192, max(0, (iTemp255 + 2)))))&16383]) * (0 - fTemp259)) * (0 - (0.5f * fTemp260))))) + (0.16666667f * ((fTemp261 * fVec29[(IOTA-int(min(8192, max(0, (iTemp255 + 3)))))&16383]) * (0 - fTemp260)))) + (0.041666668f * ((fTemp261 * fTemp259) * fVec29[(IOTA-int(min(8192, max(0, (iTemp255 + 4)))))&16383])))));
			fVec30[IOTA&16383] = fTemp262;
			fRec102[0] = (fSlow95 + (0.995f * ((iSlow94 * iTemp3) + fRec102[1])));
			float fTemp263 = (fRec102[0] + -1.49999f);
			int iTemp264 = int(min(8192, max(0, int(fTemp263))));
			float fTemp265 = floorf(fTemp263);
			float fTemp266 = (fTemp265 + (2.0f - fRec102[0]));
			float fTemp267 = (fRec102[0] - fTemp265);
			fRec101[0] = (fVec30[(IOTA-int((iTemp264 + 1)))&16383] + ((fRec101[1] * (0 - (fTemp266 / fTemp267))) + ((fTemp266 * fVec30[(IOTA-iTemp264)&16383]) / fTemp267)));
			fRec100[0] = ((fSlow23 * fRec100[1]) + (fSlow24 * ((fSlow18 * fRec101[0]) + (fSlow91 * fRec101[1]))));
			fRec99[0] = (fRec100[0] - (fSlow21 * ((fSlow79 * fRec99[1]) + (fSlow80 * fRec99[2]))));
			fRec98[0] = ((fSlow21 * (((fSlow90 * fRec99[1]) + (fSlow78 * fRec99[0])) + (fSlow78 * fRec99[2]))) - (fSlow20 * ((fSlow79 * fRec98[1]) + (fSlow81 * fRec98[2]))));
			float fTemp268 = (fSlow84 * fRec97[1]);
			fRec97[0] = ((fSlow20 * (((fSlow90 * fRec98[1]) + (fSlow78 * fRec98[0])) + (fSlow78 * fRec98[2]))) - (fSlow92 * (fTemp268 + (fSlow93 * fRec97[2]))));
			fRec108[0] = ((fSlow23 * fRec108[1]) + (fSlow24 * (fRec101[0] + fRec101[1])));
			fRec107[0] = (fRec108[0] - (fSlow21 * ((fSlow79 * fRec107[1]) + (fSlow80 * fRec107[2]))));
			fRec106[0] = ((fSlow21 * (fRec107[2] + (fRec107[0] + (2.0f * fRec107[1])))) - (fSlow20 * ((fSlow79 * fRec106[1]) + (fSlow81 * fRec106[2]))));
			float fTemp269 = (fRec106[2] + (fRec106[0] + (2.0f * fRec106[1])));
			float fTemp270 = (fSlow20 * fTemp269);
			fVec31[0] = fTemp270;
			fRec105[0] = ((fSlow83 * fRec105[1]) + (fSlow15 * ((fSlow16 * fVec31[1]) + (fSlow82 * fTemp269))));
			fRec104[0] = (fRec105[0] - (fSlow13 * ((fSlow84 * fRec104[1]) + (fSlow85 * fRec104[2]))));
			fRec103[0] = ((fSlow13 * (((fSlow12 * fRec104[0]) + (fSlow86 * fRec104[1])) + (fSlow12 * fRec104[2]))) - (fSlow10 * ((fSlow84 * fRec103[1]) + (fSlow87 * fRec103[2]))));
			fRec111[0] = ((fSlow83 * fRec111[1]) + (fSlow15 * (fVec31[0] + fVec31[1])));
			fRec110[0] = (fRec111[0] - (fSlow13 * ((fSlow84 * fRec110[1]) + (fSlow85 * fRec110[2]))));
			fRec109[0] = ((fSlow13 * (fRec110[2] + (fRec110[0] + (2.0f * fRec110[1])))) - (fSlow10 * ((fSlow84 * fRec109[1]) + (fSlow87 * fRec109[2]))));
			float fTemp271 = ((float)input1[i] + (fSlow7 * ((fSlow89 * ((fSlow92 * ((fSlow93 * fRec97[0]) + fTemp268)) + fRec97[2])) + (fSlow10 * ((fSlow11 * (((fSlow12 * fRec103[0]) + (fSlow86 * fRec103[1])) + (fSlow12 * fRec103[2]))) + (fSlow88 * (fRec109[2] + (fRec109[0] + (2.0f * fRec109[1])))))))));
			fVec32[IOTA&1023] = fTemp271;
			fRec96[0] = ((fSlow5 * ((((0.041666668f * (fTemp104 * fVec32[(IOTA-iTemp105)&1023])) + ((0.16666667f * ((fTemp101 * fTemp103) * fVec32[(IOTA-iTemp102)&1023])) + ((0.5f * (((fTemp85 * fTemp99) * fTemp100) * fVec32[(IOTA-iTemp98)&1023])) + (((fTemp95 * fTemp96) * fTemp97) * fVec32[(IOTA-iTemp94)&1023])))) * fTemp93) + ((((fTemp86 * fTemp88) * fTemp90) * fTemp92) * fVec32[(IOTA-iTemp83)&1023]))) + (fSlow4 * fRec96[1]));
			float fTemp272 = ((fSlow1 * fRec96[0]) + (fSlow3 * fRec12[1]));
			float fTemp273 = ((0.70710677f * fTemp251) - (0.70710677f * fTemp272));
			fVec33[IOTA&16383] = fTemp273;
			fRec13[0] = (fVec33[(IOTA-iTemp154)&16383] + ((fRec13[1] * fTemp158) + ((fTemp156 * fVec33[(IOTA-iTemp153)&16383]) / fTemp157)));
			fRec11[0] = fRec13[0];
			float fTemp274 = ((0.70710677f * fTemp251) + (0.70710677f * fTemp272));
			fVec34[IOTA&16383] = fTemp274;
			fRec113[0] = (fSlow97 + (0.9999f * ((iSlow96 * iTemp3) + fRec113[1])));
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
			fRec10[0] = (fVec35[(IOTA-iTemp184)&16383] + ((fTemp188 * fRec10[1]) + ((fTemp186 * fVec35[(IOTA-iTemp183)&16383]) / fTemp187)));
			fRec8[0] = fRec10[0];
			float fTemp285 = ((0.70710677f * fTemp281) + (0.70710677f * fTemp283));
			fVec36[IOTA&16383] = fTemp285;
			fRec114[0] = (fVec36[(IOTA-iTemp172)&16383] + ((fTemp176 * fRec114[1]) + ((fTemp174 * fVec36[(IOTA-iTemp171)&16383]) / fTemp175)));
			fRec9[0] = fRec114[0];
			float fTemp286 = ((fSlow1 * fRec8[1]) + (fSlow2 * fTemp280));
			float fTemp287 = ((fSlow3 * fRec5[1]) + (fSlow1 * fTemp286));
			float fTemp288 = ((fSlow1 * fRec9[1]) + (fSlow2 * fTemp282));
			float fTemp289 = ((fSlow3 * fRec6[1]) + (fSlow1 * fTemp288));
			float fTemp290 = ((0.70710677f * fTemp287) - (0.70710677f * fTemp289));
			fVec37[IOTA&16383] = fTemp290;
			fRec7[0] = (fVec37[(IOTA-iTemp192)&16383] + ((fRec7[1] * fTemp196) + ((fTemp194 * fVec37[(IOTA-iTemp191)&16383]) / fTemp195)));
			fRec5[0] = fRec7[0];
			float fTemp291 = ((0.70710677f * fTemp287) + (0.70710677f * fTemp289));
			fVec38[IOTA&16383] = fTemp291;
			fRec116[0] = (fSlow99 + (0.9999f * ((iSlow98 * iTemp3) + fRec116[1])));
			float fTemp292 = (fRec116[0] + -1.49999f);
			int iTemp293 = int(min(8192, max(0, int(fTemp292))));
			float fTemp294 = floorf(fTemp292);
			float fTemp295 = (fTemp294 + (2.0f - fRec116[0]));
			float fTemp296 = (fRec116[0] - fTemp294);
			fRec115[0] = ((((fVec38[(IOTA-iTemp293)&16383] * fTemp295) / fTemp296) + fVec38[(IOTA-int((iTemp293 + 1)))&16383]) + (fRec115[1] * (0 - (fTemp295 / fTemp296))));
			fRec6[0] = fRec115[0];
			float fTemp297 = ((fSlow1 * fRec5[1]) + (fSlow2 * fTemp286));
			float fTemp298 = ((fSlow3 * fRec2[1]) + (fSlow1 * fTemp297));
			float fTemp299 = ((fSlow1 * fRec6[1]) + (fSlow2 * fTemp288));
			float fTemp300 = ((fSlow3 * fRec3[1]) + (fSlow1 * fTemp299));
			float fTemp301 = ((0.70710677f * fTemp298) - (0.70710677f * fTemp300));
			fVec39[IOTA&16383] = fTemp301;
			fRec117[0] = (fSlow101 + (0.9999f * ((iSlow100 * iTemp3) + fRec117[1])));
			float fTemp302 = (fRec117[0] + -1.49999f);
			int iTemp303 = int(min(8192, max(0, int(fTemp302))));
			float fTemp304 = floorf(fTemp302);
			float fTemp305 = (fTemp304 + (2.0f - fRec117[0]));
			float fTemp306 = (fRec117[0] - fTemp304);
			fRec4[0] = (fVec39[(IOTA-int((iTemp303 + 1)))&16383] + ((fRec4[1] * (0 - (fTemp305 / fTemp306))) + ((fTemp305 * fVec39[(IOTA-iTemp303)&16383]) / fTemp306)));
			fRec2[0] = fRec4[0];
			float fTemp307 = ((0.70710677f * fTemp298) + (0.70710677f * fTemp300));
			fVec40[IOTA&16383] = fTemp307;
			fRec118[0] = (fVec40[(IOTA-iTemp210)&16383] + ((fRec118[1] * fTemp214) + ((fVec40[(IOTA-iTemp209)&16383] * fTemp212) / fTemp213)));
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
			fRec92[2] = fRec92[1]; fRec92[1] = fRec92[0];
			fRec93[2] = fRec93[1]; fRec93[1] = fRec93[0];
			fRec94[2] = fRec94[1]; fRec94[1] = fRec94[0];
			fRec95[1] = fRec95[0];
			fRec89[2] = fRec89[1]; fRec89[1] = fRec89[0];
			fRec90[2] = fRec90[1]; fRec90[1] = fRec90[0];
			fRec91[1] = fRec91[0];
			fRec15[2] = fRec15[1]; fRec15[1] = fRec15[0];
			fRec16[2] = fRec16[1]; fRec16[1] = fRec16[0];
			fRec17[1] = fRec17[0];
			fVec27[1] = fVec27[0];
			fRec18[2] = fRec18[1]; fRec18[1] = fRec18[0];
			fRec19[2] = fRec19[1]; fRec19[1] = fRec19[0];
			fRec20[1] = fRec20[0];
			fRec21[1] = fRec21[0];
			fRec88[1] = fRec88[0];
			fRec23[1] = fRec23[0];
			fRec86[1] = fRec86[0];
			fRec87[1] = fRec87[0];
			fRec22[1] = fRec22[0];
			fRec24[1] = fRec24[0];
			fRec85[1] = fRec85[0];
			fRec26[1] = fRec26[0];
			fRec83[1] = fRec83[0];
			fRec84[1] = fRec84[0];
			fRec25[1] = fRec25[0];
			fRec27[1] = fRec27[0];
			fRec82[1] = fRec82[0];
			fRec29[1] = fRec29[0];
			fRec80[1] = fRec80[0];
			fRec81[1] = fRec81[0];
			fRec28[1] = fRec28[0];
			fRec30[1] = fRec30[0];
			fRec79[1] = fRec79[0];
			fRec32[1] = fRec32[0];
			fRec77[1] = fRec77[0];
			fRec78[1] = fRec78[0];
			fRec31[1] = fRec31[0];
			fRec33[1] = fRec33[0];
			fRec76[1] = fRec76[0];
			fRec35[1] = fRec35[0];
			fRec74[1] = fRec74[0];
			fRec75[1] = fRec75[0];
			fRec34[1] = fRec34[0];
			fRec36[1] = fRec36[0];
			fRec73[1] = fRec73[0];
			fRec71[1] = fRec71[0];
			fRec72[1] = fRec72[0];
			fRec37[1] = fRec37[0];
			fRec70[1] = fRec70[0];
			fRec69[1] = fRec69[0];
			fRec68[1] = fRec68[0];
			fRec39[1] = fRec39[0];
			fRec66[1] = fRec66[0];
			fRec67[1] = fRec67[0];
			fRec38[1] = fRec38[0];
			fRec40[1] = fRec40[0];
			fRec65[1] = fRec65[0];
			fRec42[1] = fRec42[0];
			fRec63[1] = fRec63[0];
			fRec64[1] = fRec64[0];
			fRec41[1] = fRec41[0];
			fRec43[1] = fRec43[0];
			fRec62[1] = fRec62[0];
			fRec45[1] = fRec45[0];
			fRec60[1] = fRec60[0];
			fRec61[1] = fRec61[0];
			fRec44[1] = fRec44[0];
			fRec46[1] = fRec46[0];
			fRec59[1] = fRec59[0];
			fRec48[1] = fRec48[0];
			fRec57[1] = fRec57[0];
			fRec58[1] = fRec58[0];
			fRec47[1] = fRec47[0];
			fRec49[1] = fRec49[0];
			fRec56[1] = fRec56[0];
			fRec51[1] = fRec51[0];
			fRec54[1] = fRec54[0];
			fRec55[1] = fRec55[0];
			fRec50[1] = fRec50[0];
			fRec52[1] = fRec52[0];
			fRec53[1] = fRec53[0];
			IOTA = IOTA+1;
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
    int api_version(void);
#endif
    void load(InterfaceTable*);
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
