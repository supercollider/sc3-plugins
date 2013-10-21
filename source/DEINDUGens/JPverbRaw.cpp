//-----------------------------------------------------
// name: "JPverbRaw"
// version: "1.0"
// author: "Julian Parker"
// license: "none"
// copyright: "(c) Julian Parker 2013"
//
// Code generated with Faust 0.9.62 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with : "primes" */
#include <jprev.h>
#include <math.h>
#ifndef FAUSTPOWER
#define FAUSTPOWER
#include <cmath>
template <int N> inline float faustpower(float x)          { return powf(x,N); } 
template <int N> inline double faustpower(double x)        { return pow(x,N); }
template <int N> inline int faustpower(int x)              { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline int faustpower<0>(int x)            { return 1; }
template <> 	 inline int faustpower<1>(int x)            { return x; }
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

#include <faust/audio/dsp.h>
#include <faust/gui/UI.h>
#include <faust/misc.h>

using namespace std;

#if defined(__GNUC__) && __GNUC__ >= 4
# define FAUST_EXPORT __attribute__((visibility("default")))
#else
# define FAUST_EXPORT /* NOP */
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

typedef long double quad;

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
  private:
	int 	iVec0[2];
	FAUSTFLOAT 	fslider0;
	float 	fRec5[2];
	FAUSTFLOAT 	fslider1;
	float 	fRec9[2];
	float 	fRec13[2];
	float 	fRec17[2];
	FAUSTFLOAT 	fslider2;
	int 	iConst0;
	float 	fConst1;
	FAUSTFLOAT 	fslider3;
	float 	fRec26[2];
	float 	fRec30[2];
	float 	fRec37[2];
	float 	fRec41[2];
	float 	fRec45[2];
	float 	fRec47[2];
	float 	fRec51[2];
	float 	fRec55[2];
	float 	fRec59[2];
	float 	fRec63[2];
	float 	fRec67[2];
	int 	IOTA;
	float 	fVec1[16384];
	float 	fRec66[2];
	float 	fRec64[2];
	float 	fRec69[2];
	float 	fVec2[16384];
	float 	fRec68[2];
	float 	fRec65[2];
	float 	fVec3[16384];
	float 	fRec62[2];
	float 	fRec60[2];
	float 	fRec71[2];
	float 	fVec4[16384];
	float 	fRec70[2];
	float 	fRec61[2];
	float 	fVec5[16384];
	float 	fRec58[2];
	float 	fRec56[2];
	float 	fRec73[2];
	float 	fVec6[16384];
	float 	fRec72[2];
	float 	fRec57[2];
	float 	fVec7[16384];
	float 	fRec54[2];
	float 	fRec52[2];
	float 	fRec75[2];
	float 	fVec8[16384];
	float 	fRec74[2];
	float 	fRec53[2];
	float 	fVec9[16384];
	float 	fRec50[2];
	float 	fRec48[2];
	float 	fRec77[2];
	float 	fVec10[16384];
	float 	fRec76[2];
	float 	fRec49[2];
	float 	fVec11[512];
	FAUSTFLOAT 	fslider4;
	float 	fConst2;
	float 	fRec78[2];
	float 	fRec79[2];
	FAUSTFLOAT 	fslider5;
	float 	fVec12[16384];
	float 	fRec46[2];
	float 	fRec81[2];
	float 	fVec13[512];
	float 	fVec14[16384];
	float 	fRec80[2];
	float 	fVec15[16384];
	float 	fRec44[2];
	float 	fRec42[2];
	float 	fRec83[2];
	float 	fVec16[16384];
	float 	fRec82[2];
	float 	fRec43[2];
	float 	fVec17[16384];
	float 	fRec40[2];
	float 	fRec38[2];
	float 	fVec18[16384];
	float 	fRec84[2];
	float 	fRec39[2];
	float 	fVec19[16384];
	float 	fRec36[2];
	float 	fRec34[2];
	float 	fRec86[2];
	float 	fVec20[16384];
	float 	fRec85[2];
	float 	fRec35[2];
	float 	fVec21[16384];
	float 	fRec33[2];
	float 	fRec31[2];
	float 	fVec22[16384];
	float 	fRec87[2];
	float 	fRec32[2];
	float 	fVec23[16384];
	float 	fRec29[2];
	float 	fRec27[2];
	float 	fRec89[2];
	float 	fVec24[16384];
	float 	fRec88[2];
	float 	fRec28[2];
	float 	fVec25[8192];
	float 	fVec26[16384];
	float 	fRec25[2];
	float 	fRec24[2];
	float 	fRec23[3];
	float 	fRec22[3];
	float 	fVec27[2];
	float 	fRec21[2];
	float 	fRec20[3];
	float 	fRec19[3];
	FAUSTFLOAT 	fslider6;
	float 	fRec92[2];
	float 	fRec91[3];
	float 	fRec90[3];
	FAUSTFLOAT 	fslider7;
	float 	fRec96[2];
	float 	fRec95[3];
	float 	fRec94[3];
	float 	fRec93[3];
	FAUSTFLOAT 	fslider8;
	FAUSTFLOAT 	fslider9;
	float 	fVec28[512];
	FAUSTFLOAT 	fslider10;
	float 	fRec18[2];
	float 	fRec105[2];
	float 	fVec29[8192];
	float 	fVec30[16384];
	float 	fRec104[2];
	float 	fRec103[2];
	float 	fRec102[3];
	float 	fRec101[3];
	float 	fVec31[2];
	float 	fRec100[2];
	float 	fRec99[3];
	float 	fRec98[3];
	float 	fRec108[2];
	float 	fRec107[3];
	float 	fRec106[3];
	float 	fRec112[2];
	float 	fRec111[3];
	float 	fRec110[3];
	float 	fRec109[3];
	float 	fVec32[512];
	float 	fRec97[2];
	float 	fVec33[16384];
	float 	fRec16[2];
	float 	fRec14[2];
	float 	fRec114[2];
	float 	fVec34[16384];
	float 	fRec113[2];
	float 	fRec15[2];
	float 	fVec35[16384];
	float 	fRec12[2];
	float 	fRec10[2];
	float 	fVec36[16384];
	float 	fRec115[2];
	float 	fRec11[2];
	float 	fVec37[16384];
	float 	fRec8[2];
	float 	fRec6[2];
	float 	fRec117[2];
	float 	fVec38[16384];
	float 	fRec116[2];
	float 	fRec7[2];
	float 	fVec39[16384];
	float 	fRec4[2];
	float 	fRec2[2];
	float 	fVec40[16384];
	float 	fRec118[2];
	float 	fRec3[2];
	float 	fRec0[2];
	float 	fRec1[2];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("name", "JPverbRaw");
		m->declare("version", "1.0");
		m->declare("author", "Julian Parker");
		m->declare("license", "none");
		m->declare("copyright", "(c) Julian Parker 2013");
		m->declare("oscillator.lib/name", "Faust Oscillator Library");
		m->declare("oscillator.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("oscillator.lib/copyright", "Julius O. Smith III");
		m->declare("oscillator.lib/version", "1.11");
		m->declare("oscillator.lib/license", "STK-4.3");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/version", "1.0");
		m->declare("music.lib/license", "LGPL with exception");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("filter.lib/name", "Faust Filter Library");
		m->declare("filter.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("filter.lib/copyright", "Julius O. Smith III");
		m->declare("filter.lib/version", "1.29");
		m->declare("filter.lib/license", "STK-4.3");
		m->declare("filter.lib/reference", "https://ccrma.stanford.edu/~jos/filters/");
	}

	virtual int getNumInputs() 	{ return 2; }
	virtual int getNumOutputs() 	{ return 2; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		for (int i=0; i<2; i++) iVec0[i] = 0;
		fslider0 = 1.0f;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		fslider1 = 0.707f;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		fslider2 = 5e+02f;
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (3.141592653589793f / float(iConst0));
		fslider3 = 2e+03f;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		IOTA = 0;
		for (int i=0; i<16384; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<16384; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<16384; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<16384; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<16384; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<16384; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<16384; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<16384; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<16384; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<16384; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<512; i++) fVec11[i] = 0;
		fslider4 = 2.0f;
		fConst2 = (6.283185307179586f / float(iConst0));
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		fslider5 = 0.1f;
		for (int i=0; i<16384; i++) fVec12[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<512; i++) fVec13[i] = 0;
		for (int i=0; i<16384; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<16384; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<16384; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<16384; i++) fVec17[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<16384; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<16384; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<16384; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<16384; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<16384; i++) fVec22[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<16384; i++) fVec23[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec89[i] = 0;
		for (int i=0; i<16384; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fRec88[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<8192; i++) fVec25[i] = 0;
		for (int i=0; i<16384; i++) fVec26[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<3; i++) fRec23[i] = 0;
		for (int i=0; i<3; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fVec27[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<3; i++) fRec20[i] = 0;
		for (int i=0; i<3; i++) fRec19[i] = 0;
		fslider6 = 1.0f;
		for (int i=0; i<2; i++) fRec92[i] = 0;
		for (int i=0; i<3; i++) fRec91[i] = 0;
		for (int i=0; i<3; i++) fRec90[i] = 0;
		fslider7 = 1.0f;
		for (int i=0; i<2; i++) fRec96[i] = 0;
		for (int i=0; i<3; i++) fRec95[i] = 0;
		for (int i=0; i<3; i++) fRec94[i] = 0;
		for (int i=0; i<3; i++) fRec93[i] = 0;
		fslider8 = 1.0f;
		fslider9 = 1.0f;
		for (int i=0; i<512; i++) fVec28[i] = 0;
		fslider10 = 0.0f;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec105[i] = 0;
		for (int i=0; i<8192; i++) fVec29[i] = 0;
		for (int i=0; i<16384; i++) fVec30[i] = 0;
		for (int i=0; i<2; i++) fRec104[i] = 0;
		for (int i=0; i<2; i++) fRec103[i] = 0;
		for (int i=0; i<3; i++) fRec102[i] = 0;
		for (int i=0; i<3; i++) fRec101[i] = 0;
		for (int i=0; i<2; i++) fVec31[i] = 0;
		for (int i=0; i<2; i++) fRec100[i] = 0;
		for (int i=0; i<3; i++) fRec99[i] = 0;
		for (int i=0; i<3; i++) fRec98[i] = 0;
		for (int i=0; i<2; i++) fRec108[i] = 0;
		for (int i=0; i<3; i++) fRec107[i] = 0;
		for (int i=0; i<3; i++) fRec106[i] = 0;
		for (int i=0; i<2; i++) fRec112[i] = 0;
		for (int i=0; i<3; i++) fRec111[i] = 0;
		for (int i=0; i<3; i++) fRec110[i] = 0;
		for (int i=0; i<3; i++) fRec109[i] = 0;
		for (int i=0; i<512; i++) fVec32[i] = 0;
		for (int i=0; i<2; i++) fRec97[i] = 0;
		for (int i=0; i<16384; i++) fVec33[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec114[i] = 0;
		for (int i=0; i<16384; i++) fVec34[i] = 0;
		for (int i=0; i<2; i++) fRec113[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<16384; i++) fVec35[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<16384; i++) fVec36[i] = 0;
		for (int i=0; i<2; i++) fRec115[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<16384; i++) fVec37[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec117[i] = 0;
		for (int i=0; i<16384; i++) fVec38[i] = 0;
		for (int i=0; i<2; i++) fRec116[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<16384; i++) fVec39[i] = 0;
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
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("JPverbRaw");
		interface->addHorizontalSlider("damp", &fslider10, 0.0f, 0.0f, 0.999f, 0.0001f);
		interface->addHorizontalSlider("earlyDiff", &fslider1, 0.707f, 0.0f, 0.99f, 0.001f);
		interface->addHorizontalSlider("highBand", &fslider3, 2e+03f, 1e+03f, 1e+04f, 0.1f);
		interface->addHorizontalSlider("highX", &fslider8, 1.0f, 0.0f, 1.0f, 0.01f);
		interface->addHorizontalSlider("lowBand", &fslider2, 5e+02f, 1e+02f, 6e+03f, 0.1f);
		interface->addHorizontalSlider("lowX", &fslider7, 1.0f, 0.0f, 1.0f, 0.01f);
		interface->addHorizontalSlider("mDepth", &fslider5, 0.1f, 0.0f, 1.0f, 0.001f);
		interface->addHorizontalSlider("mFreq", &fslider4, 2.0f, 0.0f, 1e+01f, 0.01f);
		interface->addHorizontalSlider("midX", &fslider6, 1.0f, 0.0f, 1.0f, 0.01f);
		interface->addHorizontalSlider("size", &fslider0, 1.0f, 0.5f, 3.0f, 0.01f);
		interface->addHorizontalSlider("t60", &fslider9, 1.0f, 0.1f, 6e+01f, 0.1f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = float(fslider0);
		int 	iSlow1 = primes((175 * fSlow0));
		float 	fSlow2 = (9.999999999998899e-05f * iSlow1);
		float 	fSlow3 = float(fslider1);
		float 	fSlow4 = sinf(fSlow3);
		float 	fSlow5 = (0 - fSlow4);
		int 	iSlow6 = primes((115 * fSlow0));
		float 	fSlow7 = (9.999999999998899e-05f * iSlow6);
		int 	iSlow8 = primes((215 * fSlow0));
		float 	fSlow9 = (9.999999999998899e-05f * iSlow8);
		int 	iSlow10 = primes((55 * fSlow0));
		float 	fSlow11 = (9.999999999998899e-05f * iSlow10);
		float 	fSlow12 = tanf((fConst1 * float(fslider2)));
		float 	fSlow13 = (1.0f / faustpower<2>(fSlow12));
		float 	fSlow14 = (2 * (1 - fSlow13));
		float 	fSlow15 = (1.0f / fSlow12);
		float 	fSlow16 = (1 + ((fSlow15 - 0.6180339887498947f) / fSlow12));
		float 	fSlow17 = (1.0f / (1 + ((fSlow15 + 0.6180339887498947f) / fSlow12)));
		float 	fSlow18 = (1 + ((fSlow15 - 1.6180339887498947f) / fSlow12));
		float 	fSlow19 = (1.0f / (1 + ((fSlow15 + 1.6180339887498947f) / fSlow12)));
		float 	fSlow20 = tanf((fConst1 * float(fslider3)));
		float 	fSlow21 = (1.0f / faustpower<2>(fSlow20));
		float 	fSlow22 = (2 * (1 - fSlow21));
		float 	fSlow23 = (1.0f / fSlow20);
		float 	fSlow24 = (1 + ((fSlow23 - 0.6180339887498947f) / fSlow20));
		float 	fSlow25 = (1 + ((0.6180339887498947f + fSlow23) / fSlow20));
		float 	fSlow26 = (1.0f / fSlow25);
		float 	fSlow27 = (1 + ((fSlow23 - 1.6180339887498947f) / fSlow20));
		float 	fSlow28 = (1.0f / (1 + ((1.6180339887498947f + fSlow23) / fSlow20)));
		float 	fSlow29 = (1 + fSlow23);
		float 	fSlow30 = (0 - ((1 - fSlow23) / fSlow29));
		int 	iSlow31 = primes((34 * fSlow0));
		float 	fSlow32 = (0.0050000000000000044f * iSlow31);
		int 	iSlow33 = primes((245 * fSlow0));
		float 	fSlow34 = (9.999999999998899e-05f * iSlow33);
		int 	iSlow35 = primes((185 * fSlow0));
		float 	fSlow36 = (9.999999999998899e-05f * iSlow35);
		int 	iSlow37 = primes((155 * fSlow0));
		float 	fSlow38 = (9.999999999998899e-05f * iSlow37);
		int 	iSlow39 = primes((125 * fSlow0));
		float 	fSlow40 = (9.999999999998899e-05f * iSlow39);
		int 	iSlow41 = primes((204 * fSlow0));
		float 	fSlow42 = (0.0050000000000000044f * iSlow41);
		int 	iSlow43 = primes((130 * fSlow0));
		float 	fSlow44 = (9.999999999998899e-05f * iSlow43);
		int 	iSlow45 = primes((100 * fSlow0));
		float 	fSlow46 = (9.999999999998899e-05f * iSlow45);
		int 	iSlow47 = primes((70 * fSlow0));
		float 	fSlow48 = (9.999999999998899e-05f * iSlow47);
		int 	iSlow49 = primes((40 * fSlow0));
		float 	fSlow50 = (9.999999999998899e-05f * iSlow49);
		int 	iSlow51 = primes((10 * fSlow0));
		float 	fSlow52 = (9.999999999998899e-05f * iSlow51);
		int 	iSlow53 = primes((110 * fSlow0));
		float 	fSlow54 = (9.999999999998899e-05f * iSlow53);
		int 	iSlow55 = primes((140 * fSlow0));
		float 	fSlow56 = (9.999999999998899e-05f * iSlow55);
		int 	iSlow57 = primes((170 * fSlow0));
		float 	fSlow58 = (9.999999999998899e-05f * iSlow57);
		int 	iSlow59 = primes((200 * fSlow0));
		float 	fSlow60 = (9.999999999998899e-05f * iSlow59);
		int 	iSlow61 = primes((230 * fSlow0));
		float 	fSlow62 = (9.999999999998899e-05f * iSlow61);
		float 	fSlow63 = (fConst2 * float(fslider4));
		float 	fSlow64 = sinf(fSlow63);
		float 	fSlow65 = cosf(fSlow63);
		float 	fSlow66 = (0 - fSlow64);
		float 	fSlow67 = float(fslider5);
		float 	fSlow68 = (0 - (5e+01f * fSlow67));
		float 	fSlow69 = (50 * fSlow67);
		float 	fSlow70 = (fSlow69 + 3.50001f);
		float 	fSlow71 = (3.0f + fSlow69);
		float 	fSlow72 = (5.0f + fSlow69);
		float 	fSlow73 = (1.0f + fSlow69);
		float 	fSlow74 = (fSlow69 + 4.0f);
		float 	fSlow75 = (2.0f + fSlow69);
		int 	iSlow76 = primes((54 * fSlow0));
		float 	fSlow77 = (0.0050000000000000044f * iSlow76);
		int 	iSlow78 = primes((25 * fSlow0));
		float 	fSlow79 = (9.999999999998899e-05f * iSlow78);
		int 	iSlow80 = primes((85 * fSlow0));
		float 	fSlow81 = (9.999999999998899e-05f * iSlow80);
		int 	iSlow82 = primes((145 * fSlow0));
		float 	fSlow83 = (9.999999999998899e-05f * iSlow82);
		float 	fSlow84 = (1.0f / fSlow29);
		float 	fSlow85 = (1.0f / (fSlow12 * fSlow25));
		float 	fSlow86 = (0 - fSlow15);
		float 	fSlow87 = (1 + fSlow15);
		float 	fSlow88 = (1.0f / fSlow87);
		float 	fSlow89 = (0 - ((1 - fSlow15) / fSlow87));
		float 	fSlow90 = (2 * (0 - fSlow13));
		float 	fSlow91 = float(fslider6);
		float 	fSlow92 = float(fslider7);
		float 	fSlow93 = (1 + ((fSlow15 - 1.618033988749895f) / fSlow12));
		float 	fSlow94 = (1.0f / (1 + ((1.618033988749895f + fSlow15) / fSlow12)));
		float 	fSlow95 = (0 - fSlow23);
		float 	fSlow96 = (2 * (0 - fSlow21));
		float 	fSlow97 = float(fslider8);
		float 	fSlow98 = powf(10,(0 - (0.51f * (((1.25f * fSlow0) - 0.25f) / float(fslider9)))));
		float 	fSlow99 = float(fslider10);
		float 	fSlow100 = (1.0f - fSlow99);
		float 	fSlow101 = cosf(fSlow3);
		int 	iSlow102 = primes((134 * fSlow0));
		float 	fSlow103 = (0.0050000000000000044f * iSlow102);
		int 	iSlow104 = primes((240 * fSlow0));
		float 	fSlow105 = (9.999999999998899e-05f * iSlow104);
		int 	iSlow106 = primes((190 * fSlow0));
		float 	fSlow107 = (9.999999999998899e-05f * iSlow106);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			iVec0[0] = 1;
			int iTemp0 = (1 - iVec0[1]);
			fRec5[0] = (fSlow2 + (0.9999f * ((iSlow1 * iTemp0) + fRec5[1])));
			int iTemp1 = int((fRec5[0] - 1.49999f));
			float fTemp2 = (fRec5[0] - iTemp1);
			float fTemp3 = ((2.0f + iTemp1) - fRec5[0]);
			fRec9[0] = (fSlow7 + (0.9999f * ((iSlow6 * iTemp0) + fRec9[1])));
			int iTemp4 = int((fRec9[0] - 1.49999f));
			float fTemp5 = (fRec9[0] - iTemp4);
			float fTemp6 = ((2.0f + iTemp4) - fRec9[0]);
			float fTemp7 = (0 - (fTemp6 / fTemp5));
			fRec13[0] = (fSlow9 + (0.9999f * ((iSlow8 * iTemp0) + fRec13[1])));
			int iTemp8 = int((fRec13[0] - 1.49999f));
			float fTemp9 = (fRec13[0] - iTemp8);
			float fTemp10 = ((2.0f + iTemp8) - fRec13[0]);
			float fTemp11 = (0 - (fTemp10 / fTemp9));
			fRec17[0] = (fSlow11 + (0.9999f * ((iSlow10 * iTemp0) + fRec17[1])));
			int iTemp12 = int((fRec17[0] - 1.49999f));
			float fTemp13 = (fRec17[0] - iTemp12);
			float fTemp14 = ((2.0f + iTemp12) - fRec17[0]);
			float fTemp15 = (0 - (fTemp14 / fTemp13));
			fRec26[0] = (fSlow32 + (0.995f * ((iSlow31 * iTemp0) + fRec26[1])));
			int iTemp16 = int((fRec26[0] - 1.49999f));
			float fTemp17 = (fRec26[0] - iTemp16);
			float fTemp18 = ((2.0f + iTemp16) - fRec26[0]);
			fRec30[0] = (fSlow34 + (0.9999f * ((iSlow33 * iTemp0) + fRec30[1])));
			int iTemp19 = int((fRec30[0] - 1.49999f));
			float fTemp20 = (fRec30[0] - iTemp19);
			float fTemp21 = ((2.0f + iTemp19) - fRec30[0]);
			fRec37[0] = (fSlow36 + (0.9999f * ((iSlow35 * iTemp0) + fRec37[1])));
			int iTemp22 = int((fRec37[0] - 1.49999f));
			float fTemp23 = (fRec37[0] - iTemp22);
			float fTemp24 = ((2.0f + iTemp22) - fRec37[0]);
			fRec41[0] = (fSlow38 + (0.9999f * ((iSlow37 * iTemp0) + fRec41[1])));
			int iTemp25 = int((fRec41[0] - 1.49999f));
			float fTemp26 = (fRec41[0] - iTemp25);
			float fTemp27 = ((2.0f + iTemp25) - fRec41[0]);
			fRec45[0] = (fSlow40 + (0.9999f * ((iSlow39 * iTemp0) + fRec45[1])));
			int iTemp28 = int((fRec45[0] - 1.49999f));
			float fTemp29 = (fRec45[0] - iTemp28);
			float fTemp30 = ((2.0f + iTemp28) - fRec45[0]);
			fRec47[0] = (fSlow42 + (0.995f * ((iSlow41 * iTemp0) + fRec47[1])));
			int iTemp31 = int((fRec47[0] - 1.49999f));
			float fTemp32 = (fRec47[0] - iTemp31);
			float fTemp33 = ((2.0f + iTemp31) - fRec47[0]);
			fRec51[0] = (fSlow44 + (0.9999f * ((iSlow43 * iTemp0) + fRec51[1])));
			int iTemp34 = int((fRec51[0] - 1.49999f));
			float fTemp35 = (fRec51[0] - iTemp34);
			float fTemp36 = ((2.0f + iTemp34) - fRec51[0]);
			fRec55[0] = (fSlow46 + (0.9999f * ((iSlow45 * iTemp0) + fRec55[1])));
			int iTemp37 = int((fRec55[0] - 1.49999f));
			float fTemp38 = (fRec55[0] - iTemp37);
			float fTemp39 = ((2.0f + iTemp37) - fRec55[0]);
			fRec59[0] = (fSlow48 + (0.9999f * ((iSlow47 * iTemp0) + fRec59[1])));
			int iTemp40 = int((fRec59[0] - 1.49999f));
			float fTemp41 = (fRec59[0] - iTemp40);
			float fTemp42 = ((2.0f + iTemp40) - fRec59[0]);
			fRec63[0] = (fSlow50 + (0.9999f * ((iSlow49 * iTemp0) + fRec63[1])));
			int iTemp43 = int((fRec63[0] - 1.49999f));
			float fTemp44 = (fRec63[0] - iTemp43);
			float fTemp45 = ((2.0f + iTemp43) - fRec63[0]);
			fRec67[0] = (fSlow52 + (0.9999f * ((iSlow51 * iTemp0) + fRec67[1])));
			int iTemp46 = int((fRec67[0] - 1.49999f));
			float fTemp47 = (fRec67[0] - iTemp46);
			float fTemp48 = ((2.0f + iTemp46) - fRec67[0]);
			float fTemp49 = ((0.7603139617444397f * fRec1[1]) - (0.6495557555564224f * fRec65[1]));
			float fTemp50 = ((0.7603139617444397f * fRec0[1]) - (0.6495557555564224f * fRec64[1]));
			float fTemp51 = ((0.7071067811865476f * fTemp50) - (0.7071067811865475f * fTemp49));
			fVec1[IOTA&16383] = fTemp51;
			int iTemp52 = int((iTemp46 & 8191));
			fRec66[0] = ((fVec1[(IOTA-int((1 + iTemp52)))&16383] + ((fVec1[(IOTA-iTemp52)&16383] * fTemp48) / fTemp47)) + ((0 - (fTemp48 / fTemp47)) * fRec66[1]));
			fRec64[0] = fRec66[0];
			fRec69[0] = (fSlow54 + (0.9999f * ((iSlow53 * iTemp0) + fRec69[1])));
			int iTemp53 = int((fRec69[0] - 1.49999f));
			float fTemp54 = (fRec69[0] - iTemp53);
			float fTemp55 = ((2.0f + iTemp53) - fRec69[0]);
			float fTemp56 = ((0.7071067811865476f * fTemp49) + (0.7071067811865475f * fTemp50));
			fVec2[IOTA&16383] = fTemp56;
			int iTemp57 = int((iTemp53 & 8191));
			fRec68[0] = ((fVec2[(IOTA-int((1 + iTemp57)))&16383] + ((fVec2[(IOTA-iTemp57)&16383] * fTemp55) / fTemp54)) + ((0 - (fTemp55 / fTemp54)) * fRec68[1]));
			fRec65[0] = fRec68[0];
			float fTemp58 = ((0.6495557555564224f * fRec1[1]) + (0.7603139617444397f * fRec65[1]));
			float fTemp59 = ((0.7603139617444397f * fTemp58) - (0.6495557555564224f * fRec61[1]));
			float fTemp60 = ((0.6495557555564224f * fRec0[1]) + (0.7603139617444397f * fRec64[1]));
			float fTemp61 = ((0.7603139617444397f * fTemp60) - (0.6495557555564224f * fRec60[1]));
			float fTemp62 = ((0.7071067811865476f * fTemp61) - (0.7071067811865475f * fTemp59));
			fVec3[IOTA&16383] = fTemp62;
			int iTemp63 = int((iTemp43 & 8191));
			fRec62[0] = ((fVec3[(IOTA-int((1 + iTemp63)))&16383] + ((fVec3[(IOTA-iTemp63)&16383] * fTemp45) / fTemp44)) + ((0 - (fTemp45 / fTemp44)) * fRec62[1]));
			fRec60[0] = fRec62[0];
			fRec71[0] = (fSlow56 + (0.9999f * ((iSlow55 * iTemp0) + fRec71[1])));
			int iTemp64 = int((fRec71[0] - 1.49999f));
			float fTemp65 = (fRec71[0] - iTemp64);
			float fTemp66 = ((2.0f + iTemp64) - fRec71[0]);
			float fTemp67 = ((0.7071067811865476f * fTemp59) + (0.7071067811865475f * fTemp61));
			fVec4[IOTA&16383] = fTemp67;
			int iTemp68 = int((iTemp64 & 8191));
			fRec70[0] = ((fVec4[(IOTA-int((1 + iTemp68)))&16383] + ((fVec4[(IOTA-iTemp68)&16383] * fTemp66) / fTemp65)) + ((0 - (fTemp66 / fTemp65)) * fRec70[1]));
			fRec61[0] = fRec70[0];
			float fTemp69 = ((0.6495557555564224f * fTemp58) + (0.7603139617444397f * fRec61[1]));
			float fTemp70 = ((0.7603139617444397f * fTemp69) - (0.6495557555564224f * fRec57[1]));
			float fTemp71 = ((0.6495557555564224f * fTemp60) + (0.7603139617444397f * fRec60[1]));
			float fTemp72 = ((0.7603139617444397f * fTemp71) - (0.6495557555564224f * fRec56[1]));
			float fTemp73 = ((0.7071067811865476f * fTemp72) - (0.7071067811865475f * fTemp70));
			fVec5[IOTA&16383] = fTemp73;
			int iTemp74 = int((iTemp40 & 8191));
			fRec58[0] = ((fVec5[(IOTA-int((1 + iTemp74)))&16383] + ((fVec5[(IOTA-iTemp74)&16383] * fTemp42) / fTemp41)) + ((0 - (fTemp42 / fTemp41)) * fRec58[1]));
			fRec56[0] = fRec58[0];
			fRec73[0] = (fSlow58 + (0.9999f * ((iSlow57 * iTemp0) + fRec73[1])));
			int iTemp75 = int((fRec73[0] - 1.49999f));
			float fTemp76 = (fRec73[0] - iTemp75);
			float fTemp77 = ((2.0f + iTemp75) - fRec73[0]);
			float fTemp78 = ((0.7071067811865476f * fTemp70) + (0.7071067811865475f * fTemp72));
			fVec6[IOTA&16383] = fTemp78;
			int iTemp79 = int((iTemp75 & 8191));
			fRec72[0] = ((fVec6[(IOTA-int((1 + iTemp79)))&16383] + ((fVec6[(IOTA-iTemp79)&16383] * fTemp77) / fTemp76)) + ((0 - (fTemp77 / fTemp76)) * fRec72[1]));
			fRec57[0] = fRec72[0];
			float fTemp80 = ((0.6495557555564224f * fTemp69) + (0.7603139617444397f * fRec57[1]));
			float fTemp81 = ((0.7603139617444397f * fTemp80) - (0.6495557555564224f * fRec53[1]));
			float fTemp82 = ((0.6495557555564224f * fTemp71) + (0.7603139617444397f * fRec56[1]));
			float fTemp83 = ((0.7603139617444397f * fTemp82) - (0.6495557555564224f * fRec52[1]));
			float fTemp84 = ((0.7071067811865476f * fTemp83) - (0.7071067811865475f * fTemp81));
			fVec7[IOTA&16383] = fTemp84;
			int iTemp85 = int((iTemp37 & 8191));
			fRec54[0] = ((fVec7[(IOTA-int((1 + iTemp85)))&16383] + ((fVec7[(IOTA-iTemp85)&16383] * fTemp39) / fTemp38)) + ((0 - (fTemp39 / fTemp38)) * fRec54[1]));
			fRec52[0] = fRec54[0];
			fRec75[0] = (fSlow60 + (0.9999f * ((iSlow59 * iTemp0) + fRec75[1])));
			int iTemp86 = int((fRec75[0] - 1.49999f));
			float fTemp87 = (fRec75[0] - iTemp86);
			float fTemp88 = ((2.0f + iTemp86) - fRec75[0]);
			float fTemp89 = ((0.7071067811865476f * fTemp81) + (0.7071067811865475f * fTemp83));
			fVec8[IOTA&16383] = fTemp89;
			int iTemp90 = int((iTemp86 & 8191));
			fRec74[0] = ((fVec8[(IOTA-int((1 + iTemp90)))&16383] + ((fVec8[(IOTA-iTemp90)&16383] * fTemp88) / fTemp87)) + ((0 - (fTemp88 / fTemp87)) * fRec74[1]));
			fRec53[0] = fRec74[0];
			float fTemp91 = ((0.6495557555564224f * fTemp80) + (0.7603139617444397f * fRec53[1]));
			float fTemp92 = ((0.7603139617444397f * fTemp91) - (0.6495557555564224f * fRec49[1]));
			float fTemp93 = ((0.6495557555564224f * fTemp82) + (0.7603139617444397f * fRec52[1]));
			float fTemp94 = ((0.7603139617444397f * fTemp93) - (0.6495557555564224f * fRec48[1]));
			float fTemp95 = ((0.7071067811865476f * fTemp94) - (0.7071067811865475f * fTemp92));
			fVec9[IOTA&16383] = fTemp95;
			int iTemp96 = int((iTemp34 & 8191));
			fRec50[0] = ((fVec9[(IOTA-int((1 + iTemp96)))&16383] + ((fVec9[(IOTA-iTemp96)&16383] * fTemp36) / fTemp35)) + ((0 - (fTemp36 / fTemp35)) * fRec50[1]));
			fRec48[0] = fRec50[0];
			fRec77[0] = (fSlow62 + (0.9999f * ((iSlow61 * iTemp0) + fRec77[1])));
			int iTemp97 = int((fRec77[0] - 1.49999f));
			float fTemp98 = (fRec77[0] - iTemp97);
			float fTemp99 = ((2.0f + iTemp97) - fRec77[0]);
			float fTemp100 = ((0.7071067811865476f * fTemp92) + (0.7071067811865475f * fTemp94));
			fVec10[IOTA&16383] = fTemp100;
			int iTemp101 = int((iTemp97 & 8191));
			fRec76[0] = ((fVec10[(IOTA-int((1 + iTemp101)))&16383] + ((fVec10[(IOTA-iTemp101)&16383] * fTemp99) / fTemp98)) + ((0 - (fTemp99 / fTemp98)) * fRec76[1]));
			fRec49[0] = fRec76[0];
			float fTemp102 = ((0.6495557555564224f * fTemp91) + (0.7603139617444397f * fRec49[1]));
			fVec11[IOTA&511] = fTemp102;
			fRec78[0] = ((fSlow65 * fRec78[1]) + (fSlow64 * fRec79[1]));
			fRec79[0] = ((1 + ((fSlow66 * fRec78[1]) + (fSlow65 * fRec79[1]))) - iVec0[1]);
			float fTemp103 = (fSlow68 * fRec79[0]);
			int iTemp104 = int((fSlow70 + fTemp103));
			float fTemp105 = ((fSlow71 + fTemp103) - iTemp104);
			float fTemp106 = ((fSlow72 + fTemp103) - iTemp104);
			float fTemp107 = ((fTemp103 + fSlow73) - iTemp104);
			float fTemp108 = ((fSlow74 + fTemp103) - iTemp104);
			float fTemp109 = ((fSlow75 + fTemp103) - iTemp104);
			float fTemp110 = ((fTemp106 * fTemp109) * fTemp105);
			float fTemp111 = ((0.041666666666666664f * ((fTemp110 * fTemp108) * fVec11[(IOTA-int((int((4 + iTemp104)) & 511)))&511])) + (0.16666666666666666f * (((fVec11[(IOTA-int((int((3 + iTemp104)) & 511)))&511] * (0 - (((fTemp106 * fTemp105) * fTemp108) * fTemp107))) + ((0 - (fTemp110 * fTemp107)) * fVec11[(IOTA-int((int((1 + iTemp104)) & 511)))&511])) + (6.0f * (((fTemp109 * fTemp108) * fTemp107) * ((0.25f * (fTemp106 * fVec11[(IOTA-int((int((2 + iTemp104)) & 511)))&511])) + (0.041666666666666664f * (fTemp105 * fVec11[(IOTA-int((iTemp104 & 511)))&511]))))))));
			fVec12[IOTA&16383] = fTemp111;
			int iTemp112 = int((iTemp31 & 8191));
			fRec46[0] = ((fVec12[(IOTA-int((1 + iTemp112)))&16383] + ((fVec12[(IOTA-iTemp112)&16383] * fTemp33) / fTemp32)) + ((0 - (fTemp33 / fTemp32)) * fRec46[1]));
			float fTemp113 = ((0.7603139617444397f * fRec46[0]) - (0.6495557555564224f * fRec43[1]));
			fRec81[0] = (fSlow77 + (0.995f * ((iSlow76 * iTemp0) + fRec81[1])));
			int iTemp114 = int((fRec81[0] - 1.49999f));
			float fTemp115 = (fRec81[0] - iTemp114);
			float fTemp116 = ((2.0f + iTemp114) - fRec81[0]);
			float fTemp117 = ((0.6495557555564224f * fTemp93) + (0.7603139617444397f * fRec48[1]));
			fVec13[IOTA&511] = fTemp117;
			float fTemp118 = (fSlow69 * (1 + fRec79[0]));
			int iTemp119 = int((3.50001f + fTemp118));
			int iTemp120 = int((iTemp119 & 511));
			float fTemp121 = ((3.0f + fTemp118) - iTemp119);
			int iTemp122 = int((int((2 + iTemp119)) & 511));
			float fTemp123 = ((5.0f + fTemp118) - iTemp119);
			float fTemp124 = ((1.0f + fTemp118) - iTemp119);
			float fTemp125 = ((fTemp118 + 4.0f) - iTemp119);
			float fTemp126 = ((2.0f + fTemp118) - iTemp119);
			float fTemp127 = ((fTemp126 * fTemp125) * fTemp124);
			int iTemp128 = int((int((1 + iTemp119)) & 511));
			float fTemp129 = ((fTemp123 * fTemp126) * fTemp121);
			float fTemp130 = (0 - (fTemp129 * fTemp124));
			int iTemp131 = int((int((3 + iTemp119)) & 511));
			float fTemp132 = (0 - (((fTemp123 * fTemp121) * fTemp125) * fTemp124));
			int iTemp133 = int((int((4 + iTemp119)) & 511));
			float fTemp134 = (fTemp129 * fTemp125);
			float fTemp135 = ((0.041666666666666664f * (fTemp134 * fVec13[(IOTA-iTemp133)&511])) + (0.16666666666666666f * (((fTemp132 * fVec13[(IOTA-iTemp131)&511]) + (fTemp130 * fVec13[(IOTA-iTemp128)&511])) + (6.0f * (fTemp127 * ((0.25f * (fTemp123 * fVec13[(IOTA-iTemp122)&511])) + (0.041666666666666664f * (fTemp121 * fVec13[(IOTA-iTemp120)&511]))))))));
			fVec14[IOTA&16383] = fTemp135;
			int iTemp136 = int((iTemp114 & 8191));
			fRec80[0] = ((fVec14[(IOTA-int((1 + iTemp136)))&16383] + ((fVec14[(IOTA-iTemp136)&16383] * fTemp116) / fTemp115)) + ((0 - (fTemp116 / fTemp115)) * fRec80[1]));
			float fTemp137 = ((0.7603139617444397f * fRec80[0]) - (0.6495557555564224f * fRec42[1]));
			float fTemp138 = ((0.7071067811865476f * fTemp137) - (0.7071067811865475f * fTemp113));
			fVec15[IOTA&16383] = fTemp138;
			int iTemp139 = int((iTemp28 & 8191));
			fRec44[0] = ((fVec15[(IOTA-int((1 + iTemp139)))&16383] + ((fVec15[(IOTA-iTemp139)&16383] * fTemp30) / fTemp29)) + ((0 - (fTemp30 / fTemp29)) * fRec44[1]));
			fRec42[0] = fRec44[0];
			fRec83[0] = (fSlow79 + (0.9999f * ((iSlow78 * iTemp0) + fRec83[1])));
			int iTemp140 = int((fRec83[0] - 1.49999f));
			float fTemp141 = (fRec83[0] - iTemp140);
			float fTemp142 = ((2.0f + iTemp140) - fRec83[0]);
			float fTemp143 = ((0.7071067811865476f * fTemp113) + (0.7071067811865475f * fTemp137));
			fVec16[IOTA&16383] = fTemp143;
			int iTemp144 = int((iTemp140 & 8191));
			fRec82[0] = ((fVec16[(IOTA-int((1 + iTemp144)))&16383] + ((fVec16[(IOTA-iTemp144)&16383] * fTemp142) / fTemp141)) + ((0 - (fTemp142 / fTemp141)) * fRec82[1]));
			fRec43[0] = fRec82[0];
			float fTemp145 = ((0.6495557555564224f * fRec46[0]) + (0.7603139617444397f * fRec43[1]));
			float fTemp146 = ((0.7603139617444397f * fTemp145) - (0.6495557555564224f * fRec39[1]));
			float fTemp147 = ((0.6495557555564224f * fRec80[0]) + (0.7603139617444397f * fRec42[1]));
			float fTemp148 = ((0.7603139617444397f * fTemp147) - (0.6495557555564224f * fRec38[1]));
			float fTemp149 = ((0.7071067811865476f * fTemp148) - (0.7071067811865475f * fTemp146));
			fVec17[IOTA&16383] = fTemp149;
			int iTemp150 = int((iTemp25 & 8191));
			fRec40[0] = ((fVec17[(IOTA-int((1 + iTemp150)))&16383] + ((fVec17[(IOTA-iTemp150)&16383] * fTemp27) / fTemp26)) + ((0 - (fTemp27 / fTemp26)) * fRec40[1]));
			fRec38[0] = fRec40[0];
			float fTemp151 = ((0.7071067811865476f * fTemp146) + (0.7071067811865475f * fTemp148));
			fVec18[IOTA&16383] = fTemp151;
			int iTemp152 = int((iTemp12 & 8191));
			int iTemp153 = int((1 + iTemp152));
			fRec84[0] = ((fVec18[(IOTA-iTemp153)&16383] + ((fVec18[(IOTA-iTemp152)&16383] * fTemp14) / fTemp13)) + (fTemp15 * fRec84[1]));
			fRec39[0] = fRec84[0];
			float fTemp154 = ((0.6495557555564224f * fTemp145) + (0.7603139617444397f * fRec39[1]));
			float fTemp155 = ((0.7603139617444397f * fTemp154) - (0.6495557555564224f * fRec35[1]));
			float fTemp156 = ((0.6495557555564224f * fTemp147) + (0.7603139617444397f * fRec38[1]));
			float fTemp157 = ((0.7603139617444397f * fTemp156) - (0.6495557555564224f * fRec34[1]));
			float fTemp158 = ((0.7071067811865476f * fTemp157) - (0.7071067811865475f * fTemp155));
			fVec19[IOTA&16383] = fTemp158;
			int iTemp159 = int((iTemp22 & 8191));
			fRec36[0] = ((fVec19[(IOTA-int((1 + iTemp159)))&16383] + ((fVec19[(IOTA-iTemp159)&16383] * fTemp24) / fTemp23)) + ((0 - (fTemp24 / fTemp23)) * fRec36[1]));
			fRec34[0] = fRec36[0];
			fRec86[0] = (fSlow81 + (0.9999f * ((iSlow80 * iTemp0) + fRec86[1])));
			int iTemp160 = int((fRec86[0] - 1.49999f));
			float fTemp161 = (fRec86[0] - iTemp160);
			float fTemp162 = ((2.0f + iTemp160) - fRec86[0]);
			float fTemp163 = (0 - (fTemp162 / fTemp161));
			float fTemp164 = ((0.7071067811865476f * fTemp155) + (0.7071067811865475f * fTemp157));
			fVec20[IOTA&16383] = fTemp164;
			int iTemp165 = int((iTemp160 & 8191));
			int iTemp166 = int((1 + iTemp165));
			fRec85[0] = ((fVec20[(IOTA-iTemp166)&16383] + ((fVec20[(IOTA-iTemp165)&16383] * fTemp162) / fTemp161)) + (fTemp163 * fRec85[1]));
			fRec35[0] = fRec85[0];
			float fTemp167 = ((0.6495557555564224f * fTemp154) + (0.7603139617444397f * fRec35[1]));
			float fTemp168 = ((0.7603139617444397f * fTemp167) - (0.6495557555564224f * fRec32[1]));
			float fTemp169 = ((0.6495557555564224f * fTemp156) + (0.7603139617444397f * fRec34[1]));
			float fTemp170 = ((0.7603139617444397f * fTemp169) - (0.6495557555564224f * fRec31[1]));
			float fTemp171 = ((0.7071067811865476f * fTemp170) - (0.7071067811865475f * fTemp168));
			fVec21[IOTA&16383] = fTemp171;
			int iTemp172 = int((iTemp8 & 8191));
			int iTemp173 = int((1 + iTemp172));
			fRec33[0] = ((fVec21[(IOTA-iTemp173)&16383] + ((fVec21[(IOTA-iTemp172)&16383] * fTemp10) / fTemp9)) + (fTemp11 * fRec33[1]));
			fRec31[0] = fRec33[0];
			float fTemp174 = ((0.7071067811865476f * fTemp168) + (0.7071067811865475f * fTemp170));
			fVec22[IOTA&16383] = fTemp174;
			int iTemp175 = int((iTemp4 & 8191));
			int iTemp176 = int((1 + iTemp175));
			fRec87[0] = ((fVec22[(IOTA-iTemp176)&16383] + ((fVec22[(IOTA-iTemp175)&16383] * fTemp6) / fTemp5)) + (fTemp7 * fRec87[1]));
			fRec32[0] = fRec87[0];
			float fTemp177 = ((0.6495557555564224f * fTemp167) + (0.7603139617444397f * fRec32[1]));
			float fTemp178 = ((0.7603139617444397f * fTemp177) - (0.6495557555564224f * fRec28[1]));
			float fTemp179 = ((0.6495557555564224f * fTemp169) + (0.7603139617444397f * fRec31[1]));
			float fTemp180 = ((0.7603139617444397f * fTemp179) - (0.6495557555564224f * fRec27[1]));
			float fTemp181 = ((0.7071067811865476f * fTemp180) - (0.7071067811865475f * fTemp178));
			fVec23[IOTA&16383] = fTemp181;
			int iTemp182 = int((iTemp19 & 8191));
			fRec29[0] = ((fVec23[(IOTA-int((1 + iTemp182)))&16383] + ((fVec23[(IOTA-iTemp182)&16383] * fTemp21) / fTemp20)) + ((0 - (fTemp21 / fTemp20)) * fRec29[1]));
			fRec27[0] = fRec29[0];
			fRec89[0] = (fSlow83 + (0.9999f * ((iSlow82 * iTemp0) + fRec89[1])));
			int iTemp183 = int((fRec89[0] - 1.49999f));
			float fTemp184 = (fRec89[0] - iTemp183);
			float fTemp185 = ((2.0f + iTemp183) - fRec89[0]);
			float fTemp186 = (0 - (fTemp185 / fTemp184));
			float fTemp187 = ((0.7071067811865476f * fTemp178) + (0.7071067811865475f * fTemp180));
			fVec24[IOTA&16383] = fTemp187;
			int iTemp188 = int((iTemp183 & 8191));
			int iTemp189 = int((1 + iTemp188));
			fRec88[0] = ((fVec24[(IOTA-iTemp189)&16383] + ((fVec24[(IOTA-iTemp188)&16383] * fTemp185) / fTemp184)) + (fTemp186 * fRec88[1]));
			fRec28[0] = fRec88[0];
			float fTemp190 = ((0.6495557555564224f * fTemp177) + (0.7603139617444397f * fRec28[1]));
			fVec25[IOTA&8191] = fTemp190;
			float fTemp191 = (fSlow68 * fRec78[0]);
			int iTemp192 = int((fSlow70 + fTemp191));
			float fTemp193 = ((fTemp191 + fSlow71) - iTemp192);
			float fTemp194 = ((fTemp191 + fSlow72) - iTemp192);
			float fTemp195 = ((fTemp191 + fSlow73) - iTemp192);
			float fTemp196 = ((fTemp191 + fSlow74) - iTemp192);
			float fTemp197 = ((fTemp191 + fSlow75) - iTemp192);
			float fTemp198 = ((fTemp194 * fTemp197) * fTemp193);
			float fTemp199 = ((0.041666666666666664f * ((fTemp198 * fTemp196) * fVec25[(IOTA-int((int((4 + iTemp192)) & 8191)))&8191])) + (0.16666666666666666f * ((((0 - (((fTemp194 * fTemp193) * fTemp196) * fTemp195)) * fVec25[(IOTA-int((int((3 + iTemp192)) & 8191)))&8191]) + ((0 - (fTemp198 * fTemp195)) * fVec25[(IOTA-int((int((1 + iTemp192)) & 8191)))&8191])) + (6.0f * (((fTemp197 * fTemp196) * fTemp195) * ((0.25f * (fTemp194 * fVec25[(IOTA-int((int((2 + iTemp192)) & 8191)))&8191])) + (0.041666666666666664f * (fTemp193 * fVec25[(IOTA-int((iTemp192 & 8191)))&8191]))))))));
			fVec26[IOTA&16383] = fTemp199;
			int iTemp200 = int((iTemp16 & 8191));
			fRec25[0] = ((fVec26[(IOTA-int((1 + iTemp200)))&16383] + ((fVec26[(IOTA-iTemp200)&16383] * fTemp18) / fTemp17)) + ((0 - (fTemp18 / fTemp17)) * fRec25[1]));
			fRec24[0] = ((fSlow84 * (fRec25[0] + fRec25[1])) + (fSlow30 * fRec24[1]));
			fRec23[0] = (fRec24[0] - (fSlow28 * ((fSlow27 * fRec23[2]) + (fSlow22 * fRec23[1]))));
			fRec22[0] = ((fSlow28 * (fRec23[2] + (fRec23[0] + (2 * fRec23[1])))) - (fSlow26 * ((fSlow24 * fRec22[2]) + (fSlow22 * fRec22[1]))));
			float fTemp201 = (fRec22[2] + (fRec22[0] + (2 * fRec22[1])));
			float fTemp202 = (fSlow26 * fTemp201);
			fVec27[0] = fTemp202;
			fRec21[0] = ((fSlow89 * fRec21[1]) + (fSlow88 * ((fSlow86 * fVec27[1]) + (fSlow85 * fTemp201))));
			fRec20[0] = (fRec21[0] - (fSlow19 * ((fSlow18 * fRec20[2]) + (fSlow14 * fRec20[1]))));
			fRec19[0] = ((fSlow19 * (((fSlow13 * fRec20[0]) + (fSlow90 * fRec20[1])) + (fSlow13 * fRec20[2]))) - (fSlow17 * ((fSlow16 * fRec19[2]) + (fSlow14 * fRec19[1]))));
			fRec92[0] = ((fSlow88 * (fVec27[1] + fVec27[0])) + (fSlow89 * fRec92[1]));
			fRec91[0] = (fRec92[0] - (fSlow19 * ((fSlow18 * fRec91[2]) + (fSlow14 * fRec91[1]))));
			fRec90[0] = ((fSlow19 * (fRec91[2] + (fRec91[0] + (2 * fRec91[1])))) - (fSlow17 * ((fSlow16 * fRec90[2]) + (fSlow14 * fRec90[1]))));
			float fTemp203 = (fSlow14 * fRec93[1]);
			fRec96[0] = ((fSlow84 * ((fSlow95 * fRec25[1]) + (fSlow23 * fRec25[0]))) + (fSlow30 * fRec96[1]));
			fRec95[0] = (fRec96[0] - (fSlow28 * ((fSlow27 * fRec95[2]) + (fSlow22 * fRec95[1]))));
			fRec94[0] = ((fSlow28 * (((fSlow21 * fRec95[0]) + (fSlow96 * fRec95[1])) + (fSlow21 * fRec95[2]))) - (fSlow26 * ((fSlow24 * fRec94[2]) + (fSlow22 * fRec94[1]))));
			fRec93[0] = ((fSlow26 * (((fSlow21 * fRec94[0]) + (fSlow96 * fRec94[1])) + (fSlow21 * fRec94[2]))) - (fSlow94 * ((fSlow93 * fRec93[2]) + fTemp203)));
			float fTemp204 = ((float)input1[i] + (fSlow98 * ((fSlow97 * (fRec93[2] + (fSlow94 * (fTemp203 + (fSlow93 * fRec93[0]))))) + (fSlow17 * ((fSlow92 * (fRec90[2] + (fRec90[0] + (2 * fRec90[1])))) + (fSlow91 * (((fSlow13 * fRec19[0]) + (fSlow90 * fRec19[1])) + (fSlow13 * fRec19[2]))))))));
			fVec28[IOTA&511] = fTemp204;
			fRec18[0] = ((fSlow99 * fRec18[1]) + (fSlow100 * ((0.041666666666666664f * (fTemp134 * fVec28[(IOTA-iTemp133)&511])) + (0.16666666666666666f * (((fTemp132 * fVec28[(IOTA-iTemp131)&511]) + (fTemp130 * fVec28[(IOTA-iTemp128)&511])) + (6.0f * (fTemp127 * ((0.25f * (fTemp123 * fVec28[(IOTA-iTemp122)&511])) + (0.041666666666666664f * (fTemp121 * fVec28[(IOTA-iTemp120)&511]))))))))));
			float fTemp205 = ((fSlow101 * fRec18[0]) + (fSlow5 * fRec15[1]));
			fRec105[0] = (fSlow103 + (0.995f * ((iSlow102 * iTemp0) + fRec105[1])));
			int iTemp206 = int((fRec105[0] - 1.49999f));
			float fTemp207 = (fRec105[0] - iTemp206);
			float fTemp208 = ((2.0f + iTemp206) - fRec105[0]);
			float fTemp209 = ((0.6495557555564224f * fTemp179) + (0.7603139617444397f * fRec27[1]));
			fVec29[IOTA&8191] = fTemp209;
			float fTemp210 = (fSlow69 * (1 + fRec78[0]));
			int iTemp211 = int((3.50001f + fTemp210));
			float fTemp212 = ((3.0f + fTemp210) - iTemp211);
			int iTemp213 = int((2 + iTemp211));
			float fTemp214 = ((5.0f + fTemp210) - iTemp211);
			float fTemp215 = ((1.0f + fTemp210) - iTemp211);
			float fTemp216 = ((4.0f + fTemp210) - iTemp211);
			float fTemp217 = ((2.0f + fTemp210) - iTemp211);
			float fTemp218 = ((fTemp217 * fTemp216) * fTemp215);
			int iTemp219 = int((1 + iTemp211));
			float fTemp220 = ((fTemp214 * fTemp217) * fTemp212);
			float fTemp221 = (0 - (fTemp220 * fTemp215));
			int iTemp222 = int((3 + iTemp211));
			float fTemp223 = (0 - (((fTemp214 * fTemp212) * fTemp216) * fTemp215));
			int iTemp224 = int((4 + iTemp211));
			float fTemp225 = (fTemp220 * fTemp216);
			float fTemp226 = ((0.041666666666666664f * (fTemp225 * fVec29[(IOTA-int((iTemp224 & 8191)))&8191])) + (0.16666666666666666f * (((fTemp223 * fVec29[(IOTA-int((iTemp222 & 8191)))&8191]) + (fTemp221 * fVec29[(IOTA-int((iTemp219 & 8191)))&8191])) + (6.0f * (fTemp218 * ((0.25f * (fTemp214 * fVec29[(IOTA-int((iTemp213 & 8191)))&8191])) + (0.041666666666666664f * (fTemp212 * fVec29[(IOTA-int((iTemp211 & 8191)))&8191]))))))));
			fVec30[IOTA&16383] = fTemp226;
			int iTemp227 = int((iTemp206 & 8191));
			fRec104[0] = ((fVec30[(IOTA-int((1 + iTemp227)))&16383] + ((fVec30[(IOTA-iTemp227)&16383] * fTemp208) / fTemp207)) + ((0 - (fTemp208 / fTemp207)) * fRec104[1]));
			fRec103[0] = ((fSlow30 * fRec103[1]) + (fSlow84 * (fRec104[0] + fRec104[1])));
			fRec102[0] = (fRec103[0] - (fSlow28 * ((fSlow27 * fRec102[2]) + (fSlow22 * fRec102[1]))));
			fRec101[0] = ((fSlow28 * (fRec102[2] + (fRec102[0] + (2 * fRec102[1])))) - (fSlow26 * ((fSlow24 * fRec101[2]) + (fSlow22 * fRec101[1]))));
			float fTemp228 = (fRec101[2] + (fRec101[0] + (2 * fRec101[1])));
			float fTemp229 = (fSlow26 * fTemp228);
			fVec31[0] = fTemp229;
			fRec100[0] = ((fSlow88 * (fVec31[1] + fVec31[0])) + (fSlow89 * fRec100[1]));
			fRec99[0] = (fRec100[0] - (fSlow19 * ((fSlow18 * fRec99[2]) + (fSlow14 * fRec99[1]))));
			fRec98[0] = ((fSlow19 * (fRec99[2] + (fRec99[0] + (2 * fRec99[1])))) - (fSlow17 * ((fSlow16 * fRec98[2]) + (fSlow14 * fRec98[1]))));
			fRec108[0] = ((fSlow89 * fRec108[1]) + (fSlow88 * ((fSlow86 * fVec31[1]) + (fSlow85 * fTemp228))));
			fRec107[0] = (fRec108[0] - (fSlow19 * ((fSlow18 * fRec107[2]) + (fSlow14 * fRec107[1]))));
			fRec106[0] = ((fSlow19 * (((fSlow13 * fRec107[0]) + (fSlow90 * fRec107[1])) + (fSlow13 * fRec107[2]))) - (fSlow17 * ((fSlow16 * fRec106[2]) + (fSlow14 * fRec106[1]))));
			float fTemp230 = (fSlow14 * fRec109[1]);
			fRec112[0] = ((fSlow84 * ((fSlow95 * fRec104[1]) + (fSlow23 * fRec104[0]))) + (fSlow30 * fRec112[1]));
			fRec111[0] = (fRec112[0] - (fSlow28 * ((fSlow27 * fRec111[2]) + (fSlow22 * fRec111[1]))));
			fRec110[0] = ((fSlow28 * (((fSlow21 * fRec111[0]) + (fSlow96 * fRec111[1])) + (fSlow21 * fRec111[2]))) - (fSlow26 * ((fSlow24 * fRec110[2]) + (fSlow22 * fRec110[1]))));
			fRec109[0] = ((fSlow26 * (((fSlow21 * fRec110[0]) + (fSlow96 * fRec110[1])) + (fSlow21 * fRec110[2]))) - (fSlow94 * ((fSlow93 * fRec109[2]) + fTemp230)));
			float fTemp231 = ((float)input0[i] + (fSlow98 * ((fSlow97 * (fRec109[2] + (fSlow94 * (fTemp230 + (fSlow93 * fRec109[0]))))) + (fSlow17 * ((fSlow91 * (((fSlow13 * fRec106[0]) + (fSlow90 * fRec106[1])) + (fSlow13 * fRec106[2]))) + (fSlow92 * (fRec98[2] + (fRec98[0] + (2 * fRec98[1])))))))));
			fVec32[IOTA&511] = fTemp231;
			fRec97[0] = ((fSlow99 * fRec97[1]) + (fSlow100 * ((0.041666666666666664f * (fTemp225 * fVec32[(IOTA-int((iTemp224 & 511)))&511])) + (0.16666666666666666f * (((fTemp223 * fVec32[(IOTA-int((iTemp222 & 511)))&511]) + (fTemp221 * fVec32[(IOTA-int((iTemp219 & 511)))&511])) + (6.0f * (fTemp218 * ((0.25f * (fTemp214 * fVec32[(IOTA-int((iTemp213 & 511)))&511])) + (0.041666666666666664f * (fTemp212 * fVec32[(IOTA-int((iTemp211 & 511)))&511]))))))))));
			float fTemp232 = ((fSlow101 * fRec97[0]) + (fSlow5 * fRec14[1]));
			float fTemp233 = ((0.7071067811865476f * fTemp232) - (0.7071067811865475f * fTemp205));
			fVec33[IOTA&16383] = fTemp233;
			fRec16[0] = ((fVec33[(IOTA-iTemp153)&16383] + ((fTemp14 * fVec33[(IOTA-iTemp152)&16383]) / fTemp13)) + (fTemp15 * fRec16[1]));
			fRec14[0] = fRec16[0];
			fRec114[0] = (fSlow105 + (0.9999f * ((iSlow104 * iTemp0) + fRec114[1])));
			int iTemp234 = int((fRec114[0] - 1.49999f));
			float fTemp235 = (fRec114[0] - iTemp234);
			float fTemp236 = ((2.0f + iTemp234) - fRec114[0]);
			float fTemp237 = ((0.7071067811865476f * fTemp205) + (0.7071067811865475f * fTemp232));
			fVec34[IOTA&16383] = fTemp237;
			int iTemp238 = int((iTemp234 & 8191));
			fRec113[0] = ((fVec34[(IOTA-int((1 + iTemp238)))&16383] + ((fVec34[(IOTA-iTemp238)&16383] * fTemp236) / fTemp235)) + ((0 - (fTemp236 / fTemp235)) * fRec113[1]));
			fRec15[0] = fRec113[0];
			float fTemp239 = ((fSlow4 * fRec18[0]) + (fSlow101 * fRec15[1]));
			float fTemp240 = ((fSlow101 * fTemp239) + (fSlow5 * fRec11[1]));
			float fTemp241 = ((fSlow4 * fRec97[0]) + (fSlow101 * fRec14[1]));
			float fTemp242 = ((fSlow101 * fTemp241) + (fSlow5 * fRec10[1]));
			float fTemp243 = ((0.7071067811865476f * fTemp242) - (0.7071067811865475f * fTemp240));
			fVec35[IOTA&16383] = fTemp243;
			fRec12[0] = ((fVec35[(IOTA-iTemp173)&16383] + ((fTemp10 * fVec35[(IOTA-iTemp172)&16383]) / fTemp9)) + (fTemp11 * fRec12[1]));
			fRec10[0] = fRec12[0];
			float fTemp244 = ((0.7071067811865476f * fTemp240) + (0.7071067811865475f * fTemp242));
			fVec36[IOTA&16383] = fTemp244;
			fRec115[0] = ((fVec36[(IOTA-iTemp166)&16383] + ((fTemp162 * fVec36[(IOTA-iTemp165)&16383]) / fTemp161)) + (fTemp163 * fRec115[1]));
			fRec11[0] = fRec115[0];
			float fTemp245 = ((fSlow4 * fTemp239) + (fSlow101 * fRec11[1]));
			float fTemp246 = ((fSlow101 * fTemp245) + (fSlow5 * fRec7[1]));
			float fTemp247 = ((fSlow4 * fTemp241) + (fSlow101 * fRec10[1]));
			float fTemp248 = ((fSlow101 * fTemp247) + (fSlow5 * fRec6[1]));
			float fTemp249 = ((0.7071067811865476f * fTemp248) - (0.7071067811865475f * fTemp246));
			fVec37[IOTA&16383] = fTemp249;
			fRec8[0] = ((fVec37[(IOTA-iTemp176)&16383] + ((fTemp6 * fVec37[(IOTA-iTemp175)&16383]) / fTemp5)) + (fTemp7 * fRec8[1]));
			fRec6[0] = fRec8[0];
			fRec117[0] = (fSlow107 + (0.9999f * ((iSlow106 * iTemp0) + fRec117[1])));
			int iTemp250 = int((fRec117[0] - 1.49999f));
			float fTemp251 = (fRec117[0] - iTemp250);
			float fTemp252 = ((2.0f + iTemp250) - fRec117[0]);
			float fTemp253 = ((0.7071067811865476f * fTemp246) + (0.7071067811865475f * fTemp248));
			fVec38[IOTA&16383] = fTemp253;
			int iTemp254 = int((iTemp250 & 8191));
			fRec116[0] = ((fVec38[(IOTA-int((1 + iTemp254)))&16383] + ((fVec38[(IOTA-iTemp254)&16383] * fTemp252) / fTemp251)) + ((0 - (fTemp252 / fTemp251)) * fRec116[1]));
			fRec7[0] = fRec116[0];
			float fTemp255 = ((fSlow4 * fTemp245) + (fSlow101 * fRec7[1]));
			float fTemp256 = ((fSlow101 * fTemp255) + (fSlow5 * fRec3[1]));
			float fTemp257 = ((fSlow4 * fTemp247) + (fSlow101 * fRec6[1]));
			float fTemp258 = ((fSlow101 * fTemp257) + (fSlow5 * fRec2[1]));
			float fTemp259 = ((0.7071067811865476f * fTemp258) - (0.7071067811865475f * fTemp256));
			fVec39[IOTA&16383] = fTemp259;
			int iTemp260 = int((iTemp1 & 8191));
			fRec4[0] = ((fVec39[(IOTA-int((1 + iTemp260)))&16383] + ((fVec39[(IOTA-iTemp260)&16383] * fTemp3) / fTemp2)) + ((0 - (fTemp3 / fTemp2)) * fRec4[1]));
			fRec2[0] = fRec4[0];
			float fTemp261 = ((0.7071067811865476f * fTemp256) + (0.7071067811865475f * fTemp258));
			fVec40[IOTA&16383] = fTemp261;
			fRec118[0] = ((fVec40[(IOTA-iTemp189)&16383] + ((fTemp185 * fVec40[(IOTA-iTemp188)&16383]) / fTemp184)) + (fTemp186 * fRec118[1]));
			fRec3[0] = fRec118[0];
			fRec0[0] = ((fSlow4 * fTemp257) + (fSlow101 * fRec2[1]));
			fRec1[0] = ((fSlow4 * fTemp255) + (fSlow101 * fRec3[1]));
			output0[i] = (FAUSTFLOAT)fRec0[0];
			output1[i] = (FAUSTFLOAT)fRec1[0];
			// post processing
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec3[1] = fRec3[0];
			fRec118[1] = fRec118[0];
			fRec2[1] = fRec2[0];
			fRec4[1] = fRec4[0];
			fRec7[1] = fRec7[0];
			fRec116[1] = fRec116[0];
			fRec117[1] = fRec117[0];
			fRec6[1] = fRec6[0];
			fRec8[1] = fRec8[0];
			fRec11[1] = fRec11[0];
			fRec115[1] = fRec115[0];
			fRec10[1] = fRec10[0];
			fRec12[1] = fRec12[0];
			fRec15[1] = fRec15[0];
			fRec113[1] = fRec113[0];
			fRec114[1] = fRec114[0];
			fRec14[1] = fRec14[0];
			fRec16[1] = fRec16[0];
			fRec97[1] = fRec97[0];
			fRec109[2] = fRec109[1]; fRec109[1] = fRec109[0];
			fRec110[2] = fRec110[1]; fRec110[1] = fRec110[0];
			fRec111[2] = fRec111[1]; fRec111[1] = fRec111[0];
			fRec112[1] = fRec112[0];
			fRec106[2] = fRec106[1]; fRec106[1] = fRec106[0];
			fRec107[2] = fRec107[1]; fRec107[1] = fRec107[0];
			fRec108[1] = fRec108[0];
			fRec98[2] = fRec98[1]; fRec98[1] = fRec98[0];
			fRec99[2] = fRec99[1]; fRec99[1] = fRec99[0];
			fRec100[1] = fRec100[0];
			fVec31[1] = fVec31[0];
			fRec101[2] = fRec101[1]; fRec101[1] = fRec101[0];
			fRec102[2] = fRec102[1]; fRec102[1] = fRec102[0];
			fRec103[1] = fRec103[0];
			fRec104[1] = fRec104[0];
			fRec105[1] = fRec105[0];
			fRec18[1] = fRec18[0];
			fRec93[2] = fRec93[1]; fRec93[1] = fRec93[0];
			fRec94[2] = fRec94[1]; fRec94[1] = fRec94[0];
			fRec95[2] = fRec95[1]; fRec95[1] = fRec95[0];
			fRec96[1] = fRec96[0];
			fRec90[2] = fRec90[1]; fRec90[1] = fRec90[0];
			fRec91[2] = fRec91[1]; fRec91[1] = fRec91[0];
			fRec92[1] = fRec92[0];
			fRec19[2] = fRec19[1]; fRec19[1] = fRec19[0];
			fRec20[2] = fRec20[1]; fRec20[1] = fRec20[0];
			fRec21[1] = fRec21[0];
			fVec27[1] = fVec27[0];
			fRec22[2] = fRec22[1]; fRec22[1] = fRec22[0];
			fRec23[2] = fRec23[1]; fRec23[1] = fRec23[0];
			fRec24[1] = fRec24[0];
			fRec25[1] = fRec25[0];
			fRec28[1] = fRec28[0];
			fRec88[1] = fRec88[0];
			fRec89[1] = fRec89[0];
			fRec27[1] = fRec27[0];
			fRec29[1] = fRec29[0];
			fRec32[1] = fRec32[0];
			fRec87[1] = fRec87[0];
			fRec31[1] = fRec31[0];
			fRec33[1] = fRec33[0];
			fRec35[1] = fRec35[0];
			fRec85[1] = fRec85[0];
			fRec86[1] = fRec86[0];
			fRec34[1] = fRec34[0];
			fRec36[1] = fRec36[0];
			fRec39[1] = fRec39[0];
			fRec84[1] = fRec84[0];
			fRec38[1] = fRec38[0];
			fRec40[1] = fRec40[0];
			fRec43[1] = fRec43[0];
			fRec82[1] = fRec82[0];
			fRec83[1] = fRec83[0];
			fRec42[1] = fRec42[0];
			fRec44[1] = fRec44[0];
			fRec80[1] = fRec80[0];
			fRec81[1] = fRec81[0];
			fRec46[1] = fRec46[0];
			fRec79[1] = fRec79[0];
			fRec78[1] = fRec78[0];
			fRec49[1] = fRec49[0];
			fRec76[1] = fRec76[0];
			fRec77[1] = fRec77[0];
			fRec48[1] = fRec48[0];
			fRec50[1] = fRec50[0];
			fRec53[1] = fRec53[0];
			fRec74[1] = fRec74[0];
			fRec75[1] = fRec75[0];
			fRec52[1] = fRec52[0];
			fRec54[1] = fRec54[0];
			fRec57[1] = fRec57[0];
			fRec72[1] = fRec72[0];
			fRec73[1] = fRec73[0];
			fRec56[1] = fRec56[0];
			fRec58[1] = fRec58[0];
			fRec61[1] = fRec61[0];
			fRec70[1] = fRec70[0];
			fRec71[1] = fRec71[0];
			fRec60[1] = fRec60[0];
			fRec62[1] = fRec62[0];
			fRec65[1] = fRec65[0];
			fRec68[1] = fRec68[0];
			fRec69[1] = fRec69[0];
			fRec64[1] = fRec64[0];
			fRec66[1] = fRec66[0];
			IOTA = IOTA+1;
			fRec67[1] = fRec67[0];
			fRec63[1] = fRec63[0];
			fRec59[1] = fRec59[0];
			fRec55[1] = fRec55[0];
			fRec51[1] = fRec51[0];
			fRec47[1] = fRec47[0];
			fRec45[1] = fRec45[0];
			fRec41[1] = fRec41[0];
			fRec37[1] = fRec37[0];
			fRec30[1] = fRec30[0];
			fRec26[1] = fRec26[0];
			fRec17[1] = fRec17[0];
			fRec13[1] = fRec13[0];
			fRec9[1] = fRec9[0];
			fRec5[1] = fRec5[0];
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
    FAUSTCLASS  mDSP;
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

    int getNumAudioInputs() { return mDSP.getNumInputs(); }
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

static InterfaceTable *ft;

// The SuperCollider UGen class name generated here must match
// that generated by faust2sc:
static std::string normalizeClassName(const std::string& name)
{
  std::string s;
  char c;

  unsigned int i=0;
  bool upnext=true;
  while (c=name[i++]) {
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
    int curControl    = unit->mDSP.getNumInputs();
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
    unit->mDSP.compute(inNumSamples, unit->mInBuf, unit->mOutBuf);
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
    unit->mDSP.compute(inNumSamples, unit->mInBufCopy, unit->mOutBuf);
}

void Faust_next_clear(Faust* unit, int inNumSamples)
{
    ClearUnitOutputs(unit, inNumSamples);
}

void Faust_Ctor(Faust* unit)  // module constructor
{
    // init dsp
    unit->mDSP.instanceInit((int)SAMPLERATE);

    // allocate controls
    unit->mNumControls = g_numControls;
    ControlAllocator ca(unit->mControls);
    unit->mDSP.buildUserInterface(&ca);
    unit->mInBufCopy  = 0;
    unit->mInBufValue = 0;

    // check input/output channel configuration
    const size_t numInputs  = unit->mDSP.getNumInputs() + unit->mNumControls;
    const size_t numOutputs = unit->mDSP.getNumOutputs();

    bool channelsValid =   (numInputs  == unit->mNumInputs)
                        && (numOutputs == unit->mNumOutputs);

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
            // Allocate memory for input buffer copies (numInputs * bufLength)
            // and linear interpolation state (numInputs)
            // = numInputs * (bufLength + 1)
            unit->mInBufValue = (float*)RTAlloc(unit->mWorld, unit->getNumAudioInputs()*sizeof(float));
            // Aquire memory for interpolator state.
            float* mem = (float*)RTAlloc(unit->mWorld, unit->getNumAudioInputs()*BUFLENGTH*sizeof(float));
            for (int i=0; i < unit->getNumAudioInputs(); ++i) {
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
}

#ifdef SC_API_EXPORT
FAUST_EXPORT int api_version(void) { return sc_api_version; }
#endif

FAUST_EXPORT void load(InterfaceTable* inTable)
{
    ft = inTable;

    MetaData meta;
    FAUSTCLASS::metadata(&meta);

    std::string name = meta["name"];

    if (name.empty()) {
        name = fileNameToUnitName(__FILE__);
    }

    name = normalizeClassName(name);

#if !defined(NDEBUG) & defined(SC_API_EXPORT)
    Print("Faust: supercollider.cpp: sc_api_version = %d\n",sc_api_version);
#endif

    if (name.empty()) {
        // Catch empty name
        Print("Faust [supercollider.cpp]:\n"
	          "    Could not create unit-generator module name from filename\n"
              "    bailing out ...\n");
        return;
    }

    if (strncmp(name.c_str(),SC_FAUST_PREFIX,strlen(SC_FAUST_PREFIX))!=0){
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
