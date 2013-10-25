//-----------------------------------------------------
// name: "JPverbLiteRaw"
// version: "1.0"
// author: "Julian Parker"
// license: "none"
// copyright: "(c) Julian Parker 2013"
//
// Code generated with Faust 0.9.58 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with : "primes" */
#include <jprev.h>
// If other than 'faust2sc --prefix Faust' is used, sed this as well:
#if !defined(SC_FAUST_PREFIX)
#define SC_FAUST_PREFIX "Faust"
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
	FAUSTFLOAT 	fslider0;
	int 	iVec0[2];
	FAUSTFLOAT 	fslider1;
	FAUSTFLOAT 	fslider2;
	FAUSTFLOAT 	fslider3;
	int 	IOTA;
	float 	fVec1[16384];
	float 	fRec35[2];
	float 	fRec34[2];
	float 	fRec32[2];
	float 	fVec2[16384];
	float 	fRec37[2];
	float 	fRec36[2];
	float 	fRec33[2];
	float 	fVec3[16384];
	float 	fRec38[2];
	float 	fRec31[2];
	float 	fRec29[2];
	float 	fVec4[16384];
	float 	fRec40[2];
	float 	fRec39[2];
	float 	fRec30[2];
	float 	fVec5[16384];
	float 	fRec41[2];
	float 	fRec28[2];
	float 	fRec26[2];
	float 	fVec6[16384];
	float 	fRec43[2];
	float 	fRec42[2];
	float 	fRec27[2];
	float 	fVec7[16384];
	float 	fRec44[2];
	float 	fRec25[2];
	float 	fVec8[16384];
	float 	fRec46[2];
	float 	fRec45[2];
	float 	fVec9[16384];
	float 	fRec47[2];
	float 	fRec24[2];
	float 	fRec22[2];
	float 	fVec10[16384];
	float 	fRec49[2];
	float 	fRec48[2];
	float 	fRec23[2];
	float 	fVec11[16384];
	float 	fRec50[2];
	float 	fRec21[2];
	float 	fRec19[2];
	float 	fVec12[16384];
	float 	fRec52[2];
	float 	fRec51[2];
	float 	fRec20[2];
	float 	fVec13[16384];
	float 	fRec53[2];
	float 	fRec18[2];
	float 	fRec16[2];
	float 	fVec14[16384];
	float 	fRec55[2];
	float 	fRec54[2];
	float 	fRec17[2];
	float 	fVec15[16384];
	float 	fRec56[2];
	float 	fRec15[2];
	float 	fRec14[2];
	float 	fVec16[16384];
	float 	fRec59[2];
	float 	fRec58[2];
	float 	fRec57[2];
	float 	fVec17[16384];
	float 	fRec13[2];
	float 	fRec11[2];
	float 	fVec18[16384];
	float 	fRec61[2];
	float 	fRec60[2];
	float 	fRec12[2];
	float 	fVec19[16384];
	float 	fRec62[2];
	float 	fRec10[2];
	float 	fRec8[2];
	float 	fVec20[16384];
	float 	fRec63[2];
	float 	fRec9[2];
	float 	fVec21[16384];
	float 	fRec64[2];
	float 	fRec7[2];
	float 	fRec5[2];
	float 	fVec22[16384];
	float 	fRec66[2];
	float 	fRec65[2];
	float 	fRec6[2];
	float 	fVec23[16384];
	float 	fRec67[2];
	float 	fRec4[2];
	float 	fRec2[2];
	float 	fVec24[16384];
	float 	fRec69[2];
	float 	fRec68[2];
	float 	fRec3[2];
	float 	fRec0[2];
	float 	fRec1[2];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("name", "JPverbLiteRaw");
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
		fslider0 = 0.707f;
		for (int i=0; i<2; i++) iVec0[i] = 0;
		fslider1 = 0.0f;
		fslider2 = 1.0f;
		fslider3 = 1.0f;
		IOTA = 0;
		for (int i=0; i<16384; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<16384; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<16384; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<16384; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<16384; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<16384; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<16384; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<16384; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<16384; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<16384; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<16384; i++) fVec11[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<16384; i++) fVec12[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<16384; i++) fVec13[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<16384; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<16384; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<16384; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<16384; i++) fVec17[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<16384; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<16384; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<16384; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<16384; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<16384; i++) fVec22[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<16384; i++) fVec23[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<16384; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("JPverbLiteRaw");
		interface->addHorizontalSlider("damp", &fslider1, 0.0f, 0.0f, 0.999f, 0.0001f);
		interface->addHorizontalSlider("earlyDiff", &fslider0, 0.707f, 0.0f, 0.99f, 0.001f);
		interface->addHorizontalSlider("size", &fslider2, 1.0f, 0.5f, 3.0f, 0.01f);
		interface->addHorizontalSlider("t60", &fslider3, 1.0f, 0.1f, 6e+01f, 0.1f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = fslider0;
		float 	fSlow1 = cosf(fSlow0);
		float 	fSlow2 = sinf(fSlow0);
		float 	fSlow3 = (0 - fSlow2);
		float 	fSlow4 = fslider1;
		float 	fSlow5 = (1.0f - fSlow4);
		float 	fSlow6 = fslider2;
		float 	fSlow7 = powf(10,(0 - (0.51f * (((1.25f * fSlow6) - 0.25f) / fslider3))));
		int 	iSlow8 = primes((10 * fSlow6));
		float 	fSlow9 = (9.999999999998899e-05f * iSlow8);
		int 	iSlow10 = primes((110 * fSlow6));
		float 	fSlow11 = (9.999999999998899e-05f * iSlow10);
		int 	iSlow12 = primes((40 * fSlow6));
		float 	fSlow13 = (9.999999999998899e-05f * iSlow12);
		int 	iSlow14 = primes((140 * fSlow6));
		float 	fSlow15 = (9.999999999998899e-05f * iSlow14);
		int 	iSlow16 = primes((70 * fSlow6));
		float 	fSlow17 = (9.999999999998899e-05f * iSlow16);
		int 	iSlow18 = primes((170 * fSlow6));
		float 	fSlow19 = (9.999999999998899e-05f * iSlow18);
		int 	iSlow20 = primes((54 * fSlow6));
		float 	fSlow21 = (0.0050000000000000044f * iSlow20);
		int 	iSlow22 = primes((204 * fSlow6));
		float 	fSlow23 = (0.0050000000000000044f * iSlow22);
		int 	iSlow24 = primes((125 * fSlow6));
		float 	fSlow25 = (9.999999999998899e-05f * iSlow24);
		int 	iSlow26 = primes((25 * fSlow6));
		float 	fSlow27 = (9.999999999998899e-05f * iSlow26);
		int 	iSlow28 = primes((155 * fSlow6));
		float 	fSlow29 = (9.999999999998899e-05f * iSlow28);
		int 	iSlow30 = primes((55 * fSlow6));
		float 	fSlow31 = (9.999999999998899e-05f * iSlow30);
		int 	iSlow32 = primes((185 * fSlow6));
		float 	fSlow33 = (9.999999999998899e-05f * iSlow32);
		int 	iSlow34 = primes((85 * fSlow6));
		float 	fSlow35 = (9.999999999998899e-05f * iSlow34);
		int 	iSlow36 = primes((134 * fSlow6));
		float 	fSlow37 = (0.0050000000000000044f * iSlow36);
		int 	iSlow38 = primes((34 * fSlow6));
		float 	fSlow39 = (0.0050000000000000044f * iSlow38);
		int 	iSlow40 = primes((240 * fSlow6));
		float 	fSlow41 = (9.999999999998899e-05f * iSlow40);
		int 	iSlow42 = primes((215 * fSlow6));
		float 	fSlow43 = (9.999999999998899e-05f * iSlow42);
		int 	iSlow44 = primes((115 * fSlow6));
		float 	fSlow45 = (9.999999999998899e-05f * iSlow44);
		int 	iSlow46 = primes((190 * fSlow6));
		float 	fSlow47 = (9.999999999998899e-05f * iSlow46);
		int 	iSlow48 = primes((175 * fSlow6));
		float 	fSlow49 = (9.999999999998899e-05f * iSlow48);
		int 	iSlow50 = primes((145 * fSlow6));
		float 	fSlow51 = (9.999999999998899e-05f * iSlow50);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			iVec0[0] = 1;
			float fTemp0 = ((0.7603139617444397f * fRec0[1]) - (0.6495557555564224f * fRec32[1]));
			float fTemp1 = ((0.7603139617444397f * fRec1[1]) - (0.6495557555564224f * fRec33[1]));
			float fTemp2 = ((0.7071067811865476f * fTemp0) - (0.7071067811865475f * fTemp1));
			fVec1[IOTA&16383] = fTemp2;
			int iTemp3 = (1 - iVec0[1]);
			fRec35[0] = (fSlow9 + (0.9999f * ((iSlow8 * iTemp3) + fRec35[1])));
			int iTemp4 = int((fRec35[0] - 1.49999f));
			int iTemp5 = int((iTemp4 & 8191));
			float fTemp6 = ((2.0f + iTemp4) - fRec35[0]);
			float fTemp7 = (fRec35[0] - iTemp4);
			fRec34[0] = (fVec1[(IOTA-int((1 + iTemp5)))&16383] + ((fRec34[1] * (0 - (fTemp6 / fTemp7))) + ((fTemp6 * fVec1[(IOTA-iTemp5)&16383]) / fTemp7)));
			fRec32[0] = fRec34[0];
			float fTemp8 = ((0.7071067811865475f * fTemp0) + (0.7071067811865476f * fTemp1));
			fVec2[IOTA&16383] = fTemp8;
			fRec37[0] = (fSlow11 + (0.9999f * ((iSlow10 * iTemp3) + fRec37[1])));
			int iTemp9 = int((fRec37[0] - 1.49999f));
			int iTemp10 = int((iTemp9 & 8191));
			float fTemp11 = ((2.0f + iTemp9) - fRec37[0]);
			float fTemp12 = (fRec37[0] - iTemp9);
			fRec36[0] = (fVec2[(IOTA-int((1 + iTemp10)))&16383] + ((fRec36[1] * (0 - (fTemp11 / fTemp12))) + ((fTemp11 * fVec2[(IOTA-iTemp10)&16383]) / fTemp12)));
			fRec33[0] = fRec36[0];
			float fTemp13 = ((0.7603139617444397f * fRec32[1]) + (0.6495557555564224f * fRec0[1]));
			float fTemp14 = ((0.7603139617444397f * fTemp13) - (0.6495557555564224f * fRec29[1]));
			float fTemp15 = ((0.7603139617444397f * fRec33[1]) + (0.6495557555564224f * fRec1[1]));
			float fTemp16 = ((0.7603139617444397f * fTemp15) - (0.6495557555564224f * fRec30[1]));
			float fTemp17 = ((0.7071067811865476f * fTemp14) - (0.7071067811865475f * fTemp16));
			fVec3[IOTA&16383] = fTemp17;
			fRec38[0] = (fSlow13 + (0.9999f * ((iSlow12 * iTemp3) + fRec38[1])));
			int iTemp18 = int((fRec38[0] - 1.49999f));
			int iTemp19 = int((iTemp18 & 8191));
			float fTemp20 = ((2.0f + iTemp18) - fRec38[0]);
			float fTemp21 = (fRec38[0] - iTemp18);
			fRec31[0] = (fVec3[(IOTA-int((1 + iTemp19)))&16383] + ((fRec31[1] * (0 - (fTemp20 / fTemp21))) + ((fTemp20 * fVec3[(IOTA-iTemp19)&16383]) / fTemp21)));
			fRec29[0] = fRec31[0];
			float fTemp22 = ((0.7071067811865475f * fTemp14) + (0.7071067811865476f * fTemp16));
			fVec4[IOTA&16383] = fTemp22;
			fRec40[0] = (fSlow15 + (0.9999f * ((iSlow14 * iTemp3) + fRec40[1])));
			int iTemp23 = int((fRec40[0] - 1.49999f));
			int iTemp24 = int((iTemp23 & 8191));
			float fTemp25 = ((2.0f + iTemp23) - fRec40[0]);
			float fTemp26 = (fRec40[0] - iTemp23);
			fRec39[0] = (fVec4[(IOTA-int((1 + iTemp24)))&16383] + ((fRec39[1] * (0 - (fTemp25 / fTemp26))) + ((fTemp25 * fVec4[(IOTA-iTemp24)&16383]) / fTemp26)));
			fRec30[0] = fRec39[0];
			float fTemp27 = ((0.7603139617444397f * fRec29[1]) + (0.6495557555564224f * fTemp13));
			float fTemp28 = ((0.7603139617444397f * fTemp27) - (0.6495557555564224f * fRec26[1]));
			float fTemp29 = ((0.7603139617444397f * fRec30[1]) + (0.6495557555564224f * fTemp15));
			float fTemp30 = ((0.7603139617444397f * fTemp29) - (0.6495557555564224f * fRec27[1]));
			float fTemp31 = ((0.7071067811865476f * fTemp28) - (0.7071067811865475f * fTemp30));
			fVec5[IOTA&16383] = fTemp31;
			fRec41[0] = (fSlow17 + (0.9999f * ((iSlow16 * iTemp3) + fRec41[1])));
			int iTemp32 = int((fRec41[0] - 1.49999f));
			int iTemp33 = int((iTemp32 & 8191));
			float fTemp34 = ((2.0f + iTemp32) - fRec41[0]);
			float fTemp35 = (fRec41[0] - iTemp32);
			fRec28[0] = (fVec5[(IOTA-int((1 + iTemp33)))&16383] + ((fRec28[1] * (0 - (fTemp34 / fTemp35))) + ((fTemp34 * fVec5[(IOTA-iTemp33)&16383]) / fTemp35)));
			fRec26[0] = fRec28[0];
			float fTemp36 = ((0.7071067811865475f * fTemp28) + (0.7071067811865476f * fTemp30));
			fVec6[IOTA&16383] = fTemp36;
			fRec43[0] = (fSlow19 + (0.9999f * ((iSlow18 * iTemp3) + fRec43[1])));
			int iTemp37 = int((fRec43[0] - 1.49999f));
			int iTemp38 = int((iTemp37 & 8191));
			float fTemp39 = ((2.0f + iTemp37) - fRec43[0]);
			float fTemp40 = (fRec43[0] - iTemp37);
			fRec42[0] = (fVec6[(IOTA-int((1 + iTemp38)))&16383] + ((fRec42[1] * (0 - (fTemp39 / fTemp40))) + ((fTemp39 * fVec6[(IOTA-iTemp38)&16383]) / fTemp40)));
			fRec27[0] = fRec42[0];
			float fTemp41 = ((0.7603139617444397f * fRec26[1]) + (0.6495557555564224f * fTemp27));
			fVec7[IOTA&16383] = fTemp41;
			fRec44[0] = (fSlow21 + (0.995f * ((iSlow20 * iTemp3) + fRec44[1])));
			int iTemp42 = int((fRec44[0] - 1.49999f));
			int iTemp43 = int((iTemp42 & 8191));
			float fTemp44 = ((2.0f + iTemp42) - fRec44[0]);
			float fTemp45 = (fRec44[0] - iTemp42);
			fRec25[0] = (fVec7[(IOTA-int((1 + iTemp43)))&16383] + ((fRec25[1] * (0 - (fTemp44 / fTemp45))) + ((fTemp44 * fVec7[(IOTA-iTemp43)&16383]) / fTemp45)));
			float fTemp46 = ((0.7603139617444397f * fRec25[0]) - (0.6495557555564224f * fRec22[1]));
			float fTemp47 = ((0.7603139617444397f * fRec27[1]) + (0.6495557555564224f * fTemp29));
			fVec8[IOTA&16383] = fTemp47;
			fRec46[0] = (fSlow23 + (0.995f * ((iSlow22 * iTemp3) + fRec46[1])));
			int iTemp48 = int((fRec46[0] - 1.49999f));
			int iTemp49 = int((iTemp48 & 8191));
			float fTemp50 = ((2.0f + iTemp48) - fRec46[0]);
			float fTemp51 = (fRec46[0] - iTemp48);
			fRec45[0] = (fVec8[(IOTA-int((1 + iTemp49)))&16383] + ((fRec45[1] * (0 - (fTemp50 / fTemp51))) + ((fTemp50 * fVec8[(IOTA-iTemp49)&16383]) / fTemp51)));
			float fTemp52 = ((0.7603139617444397f * fRec45[0]) - (0.6495557555564224f * fRec23[1]));
			float fTemp53 = ((0.7071067811865476f * fTemp46) - (0.7071067811865475f * fTemp52));
			fVec9[IOTA&16383] = fTemp53;
			fRec47[0] = (fSlow25 + (0.9999f * ((iSlow24 * iTemp3) + fRec47[1])));
			int iTemp54 = int((fRec47[0] - 1.49999f));
			int iTemp55 = int((iTemp54 & 8191));
			float fTemp56 = ((2.0f + iTemp54) - fRec47[0]);
			float fTemp57 = (fRec47[0] - iTemp54);
			fRec24[0] = (fVec9[(IOTA-int((1 + iTemp55)))&16383] + ((fRec24[1] * (0 - (fTemp56 / fTemp57))) + ((fTemp56 * fVec9[(IOTA-iTemp55)&16383]) / fTemp57)));
			fRec22[0] = fRec24[0];
			float fTemp58 = ((0.7071067811865475f * fTemp46) + (0.7071067811865476f * fTemp52));
			fVec10[IOTA&16383] = fTemp58;
			fRec49[0] = (fSlow27 + (0.9999f * ((iSlow26 * iTemp3) + fRec49[1])));
			int iTemp59 = int((fRec49[0] - 1.49999f));
			int iTemp60 = int((iTemp59 & 8191));
			float fTemp61 = ((2.0f + iTemp59) - fRec49[0]);
			float fTemp62 = (fRec49[0] - iTemp59);
			fRec48[0] = (fVec10[(IOTA-int((1 + iTemp60)))&16383] + ((fRec48[1] * (0 - (fTemp61 / fTemp62))) + ((fTemp61 * fVec10[(IOTA-iTemp60)&16383]) / fTemp62)));
			fRec23[0] = fRec48[0];
			float fTemp63 = ((0.7603139617444397f * fRec22[1]) + (0.6495557555564224f * fRec25[0]));
			float fTemp64 = ((0.7603139617444397f * fTemp63) - (0.6495557555564224f * fRec19[1]));
			float fTemp65 = ((0.7603139617444397f * fRec23[1]) + (0.6495557555564224f * fRec45[0]));
			float fTemp66 = ((0.7603139617444397f * fTemp65) - (0.6495557555564224f * fRec20[1]));
			float fTemp67 = ((0.7071067811865476f * fTemp64) - (0.7071067811865475f * fTemp66));
			fVec11[IOTA&16383] = fTemp67;
			fRec50[0] = (fSlow29 + (0.9999f * ((iSlow28 * iTemp3) + fRec50[1])));
			int iTemp68 = int((fRec50[0] - 1.49999f));
			int iTemp69 = int((iTemp68 & 8191));
			float fTemp70 = ((2.0f + iTemp68) - fRec50[0]);
			float fTemp71 = (fRec50[0] - iTemp68);
			fRec21[0] = (fVec11[(IOTA-int((1 + iTemp69)))&16383] + ((fRec21[1] * (0 - (fTemp70 / fTemp71))) + ((fTemp70 * fVec11[(IOTA-iTemp69)&16383]) / fTemp71)));
			fRec19[0] = fRec21[0];
			float fTemp72 = ((0.7071067811865475f * fTemp64) + (0.7071067811865476f * fTemp66));
			fVec12[IOTA&16383] = fTemp72;
			fRec52[0] = (fSlow31 + (0.9999f * ((iSlow30 * iTemp3) + fRec52[1])));
			int iTemp73 = int((fRec52[0] - 1.49999f));
			int iTemp74 = int((iTemp73 & 8191));
			int iTemp75 = int((1 + iTemp74));
			float fTemp76 = ((2.0f + iTemp73) - fRec52[0]);
			float fTemp77 = (fRec52[0] - iTemp73);
			float fTemp78 = (0 - (fTemp76 / fTemp77));
			fRec51[0] = (fVec12[(IOTA-iTemp75)&16383] + ((fTemp78 * fRec51[1]) + ((fTemp76 * fVec12[(IOTA-iTemp74)&16383]) / fTemp77)));
			fRec20[0] = fRec51[0];
			float fTemp79 = ((0.7603139617444397f * fRec19[1]) + (0.6495557555564224f * fTemp63));
			float fTemp80 = ((0.7603139617444397f * fTemp79) - (0.6495557555564224f * fRec16[1]));
			float fTemp81 = ((0.7603139617444397f * fRec20[1]) + (0.6495557555564224f * fTemp65));
			float fTemp82 = ((0.7603139617444397f * fTemp81) - (0.6495557555564224f * fRec17[1]));
			float fTemp83 = ((0.7071067811865476f * fTemp80) - (0.7071067811865475f * fTemp82));
			fVec13[IOTA&16383] = fTemp83;
			fRec53[0] = (fSlow33 + (0.9999f * ((iSlow32 * iTemp3) + fRec53[1])));
			int iTemp84 = int((fRec53[0] - 1.49999f));
			int iTemp85 = int((iTemp84 & 8191));
			float fTemp86 = ((2.0f + iTemp84) - fRec53[0]);
			float fTemp87 = (fRec53[0] - iTemp84);
			fRec18[0] = (fVec13[(IOTA-int((1 + iTemp85)))&16383] + ((fRec18[1] * (0 - (fTemp86 / fTemp87))) + ((fTemp86 * fVec13[(IOTA-iTemp85)&16383]) / fTemp87)));
			fRec16[0] = fRec18[0];
			float fTemp88 = ((0.7071067811865475f * fTemp80) + (0.7071067811865476f * fTemp82));
			fVec14[IOTA&16383] = fTemp88;
			fRec55[0] = (fSlow35 + (0.9999f * ((iSlow34 * iTemp3) + fRec55[1])));
			int iTemp89 = int((fRec55[0] - 1.49999f));
			int iTemp90 = int((iTemp89 & 8191));
			int iTemp91 = int((1 + iTemp90));
			float fTemp92 = ((2.0f + iTemp89) - fRec55[0]);
			float fTemp93 = (fRec55[0] - iTemp89);
			float fTemp94 = (0 - (fTemp92 / fTemp93));
			fRec54[0] = (fVec14[(IOTA-iTemp91)&16383] + ((fRec54[1] * fTemp94) + ((fTemp92 * fVec14[(IOTA-iTemp90)&16383]) / fTemp93)));
			fRec17[0] = fRec54[0];
			float fTemp95 = ((0.7603139617444397f * fRec16[1]) + (0.6495557555564224f * fTemp79));
			fVec15[IOTA&16383] = fTemp95;
			fRec56[0] = (fSlow37 + (0.995f * ((iSlow36 * iTemp3) + fRec56[1])));
			int iTemp96 = int((fRec56[0] - 1.49999f));
			int iTemp97 = int((iTemp96 & 8191));
			float fTemp98 = ((2.0f + iTemp96) - fRec56[0]);
			float fTemp99 = (fRec56[0] - iTemp96);
			fRec15[0] = (fVec15[(IOTA-int((1 + iTemp97)))&16383] + ((fRec15[1] * (0 - (fTemp98 / fTemp99))) + ((fTemp98 * fVec15[(IOTA-iTemp97)&16383]) / fTemp99)));
			fRec14[0] = ((fSlow4 * fRec14[1]) + (fSlow5 * ((float)input0[i] + (fSlow7 * fRec15[0]))));
			float fTemp100 = ((fSlow3 * fRec11[1]) + (fSlow1 * fRec14[0]));
			float fTemp101 = ((0.7603139617444397f * fRec17[1]) + (0.6495557555564224f * fTemp81));
			fVec16[IOTA&16383] = fTemp101;
			fRec59[0] = (fSlow39 + (0.995f * ((iSlow38 * iTemp3) + fRec59[1])));
			int iTemp102 = int((fRec59[0] - 1.49999f));
			int iTemp103 = int((iTemp102 & 8191));
			float fTemp104 = ((2.0f + iTemp102) - fRec59[0]);
			float fTemp105 = (fRec59[0] - iTemp102);
			fRec58[0] = (fVec16[(IOTA-int((1 + iTemp103)))&16383] + ((fRec58[1] * (0 - (fTemp104 / fTemp105))) + ((fTemp104 * fVec16[(IOTA-iTemp103)&16383]) / fTemp105)));
			fRec57[0] = ((fSlow4 * fRec57[1]) + (fSlow5 * ((float)input1[i] + (fSlow7 * fRec58[0]))));
			float fTemp106 = ((fSlow3 * fRec12[1]) + (fSlow1 * fRec57[0]));
			float fTemp107 = ((0.7071067811865476f * fTemp100) - (0.7071067811865475f * fTemp106));
			fVec17[IOTA&16383] = fTemp107;
			fRec13[0] = (fVec17[(IOTA-iTemp75)&16383] + ((fRec13[1] * fTemp78) + ((fTemp76 * fVec17[(IOTA-iTemp74)&16383]) / fTemp77)));
			fRec11[0] = fRec13[0];
			float fTemp108 = ((0.7071067811865475f * fTemp100) + (0.7071067811865476f * fTemp106));
			fVec18[IOTA&16383] = fTemp108;
			fRec61[0] = (fSlow41 + (0.9999f * ((iSlow40 * iTemp3) + fRec61[1])));
			int iTemp109 = int((fRec61[0] - 1.49999f));
			int iTemp110 = int((iTemp109 & 8191));
			float fTemp111 = ((2.0f + iTemp109) - fRec61[0]);
			float fTemp112 = (fRec61[0] - iTemp109);
			fRec60[0] = (fVec18[(IOTA-int((1 + iTemp110)))&16383] + ((fRec60[1] * (0 - (fTemp111 / fTemp112))) + ((fTemp111 * fVec18[(IOTA-iTemp110)&16383]) / fTemp112)));
			fRec12[0] = fRec60[0];
			float fTemp113 = ((fSlow1 * fRec11[1]) + (fSlow2 * fRec14[0]));
			float fTemp114 = ((fSlow3 * fRec8[1]) + (fSlow1 * fTemp113));
			float fTemp115 = ((fSlow1 * fRec12[1]) + (fSlow2 * fRec57[0]));
			float fTemp116 = ((fSlow3 * fRec9[1]) + (fSlow1 * fTemp115));
			float fTemp117 = ((0.7071067811865476f * fTemp114) - (0.7071067811865475f * fTemp116));
			fVec19[IOTA&16383] = fTemp117;
			fRec62[0] = (fSlow43 + (0.9999f * ((iSlow42 * iTemp3) + fRec62[1])));
			int iTemp118 = int((fRec62[0] - 1.49999f));
			int iTemp119 = int((iTemp118 & 8191));
			float fTemp120 = ((2.0f + iTemp118) - fRec62[0]);
			float fTemp121 = (fRec62[0] - iTemp118);
			fRec10[0] = (fVec19[(IOTA-int((1 + iTemp119)))&16383] + ((fRec10[1] * (0 - (fTemp120 / fTemp121))) + ((fTemp120 * fVec19[(IOTA-iTemp119)&16383]) / fTemp121)));
			fRec8[0] = fRec10[0];
			float fTemp122 = ((0.7071067811865475f * fTemp114) + (0.7071067811865476f * fTemp116));
			fVec20[IOTA&16383] = fTemp122;
			fRec63[0] = (fVec20[(IOTA-iTemp91)&16383] + ((fTemp94 * fRec63[1]) + ((fTemp92 * fVec20[(IOTA-iTemp90)&16383]) / fTemp93)));
			fRec9[0] = fRec63[0];
			float fTemp123 = ((fSlow1 * fRec8[1]) + (fSlow2 * fTemp113));
			float fTemp124 = ((fSlow3 * fRec5[1]) + (fSlow1 * fTemp123));
			float fTemp125 = ((fSlow1 * fRec9[1]) + (fSlow2 * fTemp115));
			float fTemp126 = ((fSlow3 * fRec6[1]) + (fSlow1 * fTemp125));
			float fTemp127 = ((0.7071067811865476f * fTemp124) - (0.7071067811865475f * fTemp126));
			fVec21[IOTA&16383] = fTemp127;
			fRec64[0] = (fSlow45 + (0.9999f * ((iSlow44 * iTemp3) + fRec64[1])));
			int iTemp128 = int((fRec64[0] - 1.49999f));
			int iTemp129 = int((iTemp128 & 8191));
			float fTemp130 = ((2.0f + iTemp128) - fRec64[0]);
			float fTemp131 = (fRec64[0] - iTemp128);
			fRec7[0] = (fVec21[(IOTA-int((1 + iTemp129)))&16383] + ((fRec7[1] * (0 - (fTemp130 / fTemp131))) + ((fTemp130 * fVec21[(IOTA-iTemp129)&16383]) / fTemp131)));
			fRec5[0] = fRec7[0];
			float fTemp132 = ((0.7071067811865475f * fTemp124) + (0.7071067811865476f * fTemp126));
			fVec22[IOTA&16383] = fTemp132;
			fRec66[0] = (fSlow47 + (0.9999f * ((iSlow46 * iTemp3) + fRec66[1])));
			int iTemp133 = int((fRec66[0] - 1.49999f));
			int iTemp134 = int((iTemp133 & 8191));
			float fTemp135 = ((2.0f + iTemp133) - fRec66[0]);
			float fTemp136 = (fRec66[0] - iTemp133);
			fRec65[0] = (fVec22[(IOTA-int((1 + iTemp134)))&16383] + ((fRec65[1] * (0 - (fTemp135 / fTemp136))) + ((fTemp135 * fVec22[(IOTA-iTemp134)&16383]) / fTemp136)));
			fRec6[0] = fRec65[0];
			float fTemp137 = ((fSlow1 * fRec5[1]) + (fSlow2 * fTemp123));
			float fTemp138 = ((fSlow3 * fRec2[1]) + (fSlow1 * fTemp137));
			float fTemp139 = ((fSlow1 * fRec6[1]) + (fSlow2 * fTemp125));
			float fTemp140 = ((fSlow3 * fRec3[1]) + (fSlow1 * fTemp139));
			float fTemp141 = ((0.7071067811865476f * fTemp138) - (0.7071067811865475f * fTemp140));
			fVec23[IOTA&16383] = fTemp141;
			fRec67[0] = (fSlow49 + (0.9999f * ((iSlow48 * iTemp3) + fRec67[1])));
			int iTemp142 = int((fRec67[0] - 1.49999f));
			int iTemp143 = int((iTemp142 & 8191));
			float fTemp144 = ((2.0f + iTemp142) - fRec67[0]);
			float fTemp145 = (fRec67[0] - iTemp142);
			fRec4[0] = (fVec23[(IOTA-int((1 + iTemp143)))&16383] + ((fRec4[1] * (0 - (fTemp144 / fTemp145))) + ((fTemp144 * fVec23[(IOTA-iTemp143)&16383]) / fTemp145)));
			fRec2[0] = fRec4[0];
			float fTemp146 = ((0.7071067811865475f * fTemp138) + (0.7071067811865476f * fTemp140));
			fVec24[IOTA&16383] = fTemp146;
			fRec69[0] = (fSlow51 + (0.9999f * ((iSlow50 * iTemp3) + fRec69[1])));
			int iTemp147 = int((fRec69[0] - 1.49999f));
			int iTemp148 = int((iTemp147 & 8191));
			float fTemp149 = ((2.0f + iTemp147) - fRec69[0]);
			float fTemp150 = (fRec69[0] - iTemp147);
			fRec68[0] = (fVec24[(IOTA-int((1 + iTemp148)))&16383] + ((fRec68[1] * (0 - (fTemp149 / fTemp150))) + ((fTemp149 * fVec24[(IOTA-iTemp148)&16383]) / fTemp150)));
			fRec3[0] = fRec68[0];
			fRec0[0] = ((fSlow1 * fRec2[1]) + (fSlow2 * fTemp137));
			fRec1[0] = ((fSlow1 * fRec3[1]) + (fSlow2 * fTemp139));
			output0[i] = (FAUSTFLOAT)fRec0[0];
			output1[i] = (FAUSTFLOAT)fRec1[0];
			// post processing
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec3[1] = fRec3[0];
			fRec68[1] = fRec68[0];
			fRec69[1] = fRec69[0];
			fRec2[1] = fRec2[0];
			fRec4[1] = fRec4[0];
			fRec67[1] = fRec67[0];
			fRec6[1] = fRec6[0];
			fRec65[1] = fRec65[0];
			fRec66[1] = fRec66[0];
			fRec5[1] = fRec5[0];
			fRec7[1] = fRec7[0];
			fRec64[1] = fRec64[0];
			fRec9[1] = fRec9[0];
			fRec63[1] = fRec63[0];
			fRec8[1] = fRec8[0];
			fRec10[1] = fRec10[0];
			fRec62[1] = fRec62[0];
			fRec12[1] = fRec12[0];
			fRec60[1] = fRec60[0];
			fRec61[1] = fRec61[0];
			fRec11[1] = fRec11[0];
			fRec13[1] = fRec13[0];
			fRec57[1] = fRec57[0];
			fRec58[1] = fRec58[0];
			fRec59[1] = fRec59[0];
			fRec14[1] = fRec14[0];
			fRec15[1] = fRec15[0];
			fRec56[1] = fRec56[0];
			fRec17[1] = fRec17[0];
			fRec54[1] = fRec54[0];
			fRec55[1] = fRec55[0];
			fRec16[1] = fRec16[0];
			fRec18[1] = fRec18[0];
			fRec53[1] = fRec53[0];
			fRec20[1] = fRec20[0];
			fRec51[1] = fRec51[0];
			fRec52[1] = fRec52[0];
			fRec19[1] = fRec19[0];
			fRec21[1] = fRec21[0];
			fRec50[1] = fRec50[0];
			fRec23[1] = fRec23[0];
			fRec48[1] = fRec48[0];
			fRec49[1] = fRec49[0];
			fRec22[1] = fRec22[0];
			fRec24[1] = fRec24[0];
			fRec47[1] = fRec47[0];
			fRec45[1] = fRec45[0];
			fRec46[1] = fRec46[0];
			fRec25[1] = fRec25[0];
			fRec44[1] = fRec44[0];
			fRec27[1] = fRec27[0];
			fRec42[1] = fRec42[0];
			fRec43[1] = fRec43[0];
			fRec26[1] = fRec26[0];
			fRec28[1] = fRec28[0];
			fRec41[1] = fRec41[0];
			fRec30[1] = fRec30[0];
			fRec39[1] = fRec39[0];
			fRec40[1] = fRec40[0];
			fRec29[1] = fRec29[0];
			fRec31[1] = fRec31[0];
			fRec38[1] = fRec38[0];
			fRec33[1] = fRec33[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec32[1] = fRec32[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
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
