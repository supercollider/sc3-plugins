//-----------------------------------------------------
// name: "GreyholeRaw"
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
	float 	fVec1[2];
	FAUSTFLOAT 	fslider1;
	float 	fRec6[2];
	float 	fRec10[2];
	float 	fRec14[2];
	float 	fRec18[2];
	FAUSTFLOAT 	fslider2;
	float 	fVec2[2];
	float 	fRec22[2];
	float 	fRec26[2];
	float 	fRec30[2];
	float 	fRec34[2];
	float 	fRec38[2];
	float 	fRec42[2];
	float 	fRec46[2];
	float 	fRec50[2];
	FAUSTFLOAT 	fslider3;
	float 	fVec3[2];
	int 	iConst0;
	float 	fConst1;
	float 	fRec51[2];
	float 	fRec52[2];
	FAUSTFLOAT 	fslider4;
	float 	fVec4[2];
	float 	fConst2;
	int 	IOTA;
	float 	fVec5[65536];
	FAUSTFLOAT 	fslider5;
	float 	fRec53[2];
	float 	fRec54[2];
	float 	fRec55[2];
	float 	fRec56[2];
	FAUSTFLOAT 	fslider6;
	float 	fVec6[2];
	float 	fVec7[65536];
	float 	fVec8[16384];
	float 	fRec49[2];
	float 	fRec47[2];
	float 	fRec58[2];
	float 	fVec9[16384];
	float 	fRec57[2];
	float 	fRec48[2];
	float 	fVec10[16384];
	float 	fRec45[2];
	float 	fRec43[2];
	float 	fRec60[2];
	float 	fVec11[16384];
	float 	fRec59[2];
	float 	fRec44[2];
	float 	fVec12[16384];
	float 	fRec41[2];
	float 	fRec39[2];
	float 	fRec62[2];
	float 	fVec13[16384];
	float 	fRec61[2];
	float 	fRec40[2];
	float 	fVec14[16384];
	float 	fRec37[2];
	float 	fRec35[2];
	float 	fRec64[2];
	float 	fVec15[16384];
	float 	fRec63[2];
	float 	fRec36[2];
	float 	fVec16[16384];
	float 	fRec33[2];
	float 	fRec31[2];
	float 	fRec66[2];
	float 	fVec17[16384];
	float 	fRec65[2];
	float 	fRec32[2];
	float 	fVec18[16384];
	float 	fRec29[2];
	float 	fRec27[2];
	float 	fRec68[2];
	float 	fVec19[16384];
	float 	fRec67[2];
	float 	fRec28[2];
	float 	fVec20[16384];
	float 	fRec25[2];
	float 	fRec23[2];
	float 	fRec70[2];
	float 	fVec21[16384];
	float 	fRec69[2];
	float 	fRec24[2];
	float 	fVec22[16384];
	float 	fRec21[2];
	float 	fRec19[2];
	float 	fRec72[2];
	float 	fVec23[16384];
	float 	fRec71[2];
	float 	fRec20[2];
	float 	fVec24[16384];
	float 	fRec17[2];
	float 	fRec15[2];
	float 	fRec74[2];
	float 	fVec25[16384];
	float 	fRec73[2];
	float 	fRec16[2];
	float 	fVec26[16384];
	float 	fRec13[2];
	float 	fRec11[2];
	float 	fRec76[2];
	float 	fVec27[16384];
	float 	fRec75[2];
	float 	fRec12[2];
	float 	fVec28[16384];
	float 	fRec9[2];
	float 	fRec7[2];
	float 	fRec78[2];
	float 	fVec29[16384];
	float 	fRec77[2];
	float 	fRec8[2];
	float 	fVec30[16384];
	float 	fRec5[2];
	float 	fRec3[2];
	float 	fRec80[2];
	float 	fVec31[16384];
	float 	fRec79[2];
	float 	fRec4[2];
	float 	fRec2[2];
	float 	fRec0[1024];
	float 	fRec81[2];
	float 	fRec1[1024];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("name", "GreyholeRaw");
		m->declare("version", "1.0");
		m->declare("author", "Julian Parker");
		m->declare("license", "none");
		m->declare("copyright", "(c) Julian Parker 2013");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("maxmsp.lib/name", "MaxMSP compatibility Library");
		m->declare("maxmsp.lib/author", "GRAME");
		m->declare("maxmsp.lib/copyright", "GRAME");
		m->declare("maxmsp.lib/version", "1.1");
		m->declare("maxmsp.lib/license", "LGPL");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/version", "1.0");
		m->declare("music.lib/license", "LGPL with exception");
		m->declare("oscillator.lib/name", "Faust Oscillator Library");
		m->declare("oscillator.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("oscillator.lib/copyright", "Julius O. Smith III");
		m->declare("oscillator.lib/version", "1.11");
		m->declare("oscillator.lib/license", "STK-4.3");
		m->declare("filter.lib/name", "Faust Filter Library");
		m->declare("filter.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("filter.lib/copyright", "Julius O. Smith III");
		m->declare("filter.lib/version", "1.29");
		m->declare("filter.lib/license", "STK-4.3");
		m->declare("filter.lib/reference", "https://ccrma.stanford.edu/~jos/filters/");
		m->declare("reduce.lib/name", "Reduce Library");
		m->declare("reduce.lib/author", "Yann Orlarey (orlarey at grame.fr)");
		m->declare("reduce.lib/copyright", "Grame");
		m->declare("reduce.lib/version", "0.1");
		m->declare("reduce.lib/license", "LGPL");
		m->declare("effect.lib/name", "Faust Audio Effect Library");
		m->declare("effect.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("effect.lib/copyright", "Julius O. Smith III");
		m->declare("effect.lib/version", "1.33");
		m->declare("effect.lib/license", "STK-4.3");
	}

	virtual int getNumInputs() 	{ return 2; }
	virtual int getNumOutputs() 	{ return 2; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		for (int i=0; i<2; i++) iVec0[i] = 0;
		fslider0 = 0.0f;
		for (int i=0; i<2; i++) fVec1[i] = 0;
		fslider1 = 1.0f;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		fslider2 = 0.5f;
		for (int i=0; i<2; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		fslider3 = 2.0f;
		for (int i=0; i<2; i++) fVec3[i] = 0;
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (3.141592653589793f / float(iConst0));
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		fslider4 = 0.1f;
		for (int i=0; i<2; i++) fVec4[i] = 0;
		fConst2 = (0.0005668934240362812f * iConst0);
		IOTA = 0;
		for (int i=0; i<65536; i++) fVec5[i] = 0;
		fslider5 = 0.2f;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		fslider6 = 0.9f;
		for (int i=0; i<2; i++) fVec6[i] = 0;
		for (int i=0; i<65536; i++) fVec7[i] = 0;
		for (int i=0; i<16384; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<16384; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<16384; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<16384; i++) fVec11[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<16384; i++) fVec12[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<16384; i++) fVec13[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<16384; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<16384; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<16384; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<16384; i++) fVec17[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<16384; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<16384; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<16384; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<16384; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<16384; i++) fVec22[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<16384; i++) fVec23[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<16384; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<16384; i++) fVec25[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<16384; i++) fVec26[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<16384; i++) fVec27[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<16384; i++) fVec28[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<16384; i++) fVec29[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<16384; i++) fVec30[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<16384; i++) fVec31[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<1024; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<1024; i++) fRec1[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("GreyholeRaw");
		interface->addHorizontalSlider("damping", &fslider0, 0.0f, 0.0f, 0.99f, 0.001f);
		interface->addHorizontalSlider("delayTime", &fslider5, 0.2f, 0.001f, 1.45f, 0.0001f);
		interface->addHorizontalSlider("diffusion", &fslider2, 0.5f, 0.0f, 0.99f, 0.0001f);
		interface->addHorizontalSlider("feedback", &fslider6, 0.9f, 0.0f, 1.0f, 0.01f);
		interface->addHorizontalSlider("modDepth", &fslider4, 0.1f, 0.0f, 1.0f, 0.001f);
		interface->addHorizontalSlider("modFreq", &fslider3, 2.0f, 0.0f, 1e+01f, 0.01f);
		interface->addHorizontalSlider("size", &fslider1, 1.0f, 0.5f, 3.0f, 0.0001f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = float(fslider0);
		float 	fSlow1 = float(fslider1);
		float 	fSlow2 = (48 * fSlow1);
		int 	iSlow3 = primes(fSlow2);
		float 	fSlow4 = (0.0010000000000000009f * iSlow3);
		float 	fSlow5 = (61 * fSlow1);
		int 	iSlow6 = primes(fSlow5);
		float 	fSlow7 = (0.0010000000000000009f * iSlow6);
		float 	fSlow8 = (74 * fSlow1);
		int 	iSlow9 = primes(fSlow8);
		float 	fSlow10 = (0.0010000000000000009f * iSlow9);
		float 	fSlow11 = (87 * fSlow1);
		int 	iSlow12 = primes(fSlow11);
		float 	fSlow13 = (9.999999999998899e-05f * iSlow12);
		float 	fSlow14 = float(fslider2);
		float 	fSlow15 = (29 * fSlow1);
		int 	iSlow16 = primes(fSlow15);
		float 	fSlow17 = (0.0010000000000000009f * iSlow16);
		float 	fSlow18 = (42 * fSlow1);
		int 	iSlow19 = primes(fSlow18);
		float 	fSlow20 = (0.0010000000000000009f * iSlow19);
		float 	fSlow21 = (55 * fSlow1);
		int 	iSlow22 = primes(fSlow21);
		float 	fSlow23 = (0.0010000000000000009f * iSlow22);
		float 	fSlow24 = (68 * fSlow1);
		int 	iSlow25 = primes(fSlow24);
		float 	fSlow26 = (9.999999999998899e-05f * iSlow25);
		int 	iSlow27 = primes((10 * fSlow1));
		float 	fSlow28 = (0.0010000000000000009f * iSlow27);
		float 	fSlow29 = (23 * fSlow1);
		int 	iSlow30 = primes(fSlow29);
		float 	fSlow31 = (0.0010000000000000009f * iSlow30);
		float 	fSlow32 = (36 * fSlow1);
		int 	iSlow33 = primes(fSlow32);
		float 	fSlow34 = (0.0010000000000000009f * iSlow33);
		float 	fSlow35 = (49 * fSlow1);
		int 	iSlow36 = primes(fSlow35);
		float 	fSlow37 = (9.999999999998899e-05f * iSlow36);
		float 	fSlow38 = float(fslider3);
		float 	fSlow39 = float(fslider4);
		float 	fSlow40 = floorf(min((float)65533, (iConst0 * float(fslider5))));
		float 	fSlow41 = float(fslider6);
		int 	iSlow42 = primes((10 + fSlow35));
		float 	fSlow43 = (9.999999999998899e-05f * iSlow42);
		int 	iSlow44 = primes((10 + fSlow32));
		float 	fSlow45 = (0.0010000000000000009f * iSlow44);
		int 	iSlow46 = primes((10 + fSlow29));
		float 	fSlow47 = (0.0010000000000000009f * iSlow46);
		int 	iSlow48 = primes((10 * (1 + fSlow1)));
		float 	fSlow49 = (0.0010000000000000009f * iSlow48);
		int 	iSlow50 = primes((10 + fSlow24));
		float 	fSlow51 = (9.999999999998899e-05f * iSlow50);
		int 	iSlow52 = primes((10 + fSlow21));
		float 	fSlow53 = (0.0010000000000000009f * iSlow52);
		int 	iSlow54 = primes((10 + fSlow18));
		float 	fSlow55 = (0.0010000000000000009f * iSlow54);
		int 	iSlow56 = primes((10 + fSlow15));
		float 	fSlow57 = (0.0010000000000000009f * iSlow56);
		int 	iSlow58 = primes((10 + fSlow11));
		float 	fSlow59 = (9.999999999998899e-05f * iSlow58);
		int 	iSlow60 = primes((10 + fSlow8));
		float 	fSlow61 = (0.0010000000000000009f * iSlow60);
		int 	iSlow62 = primes((10 + fSlow5));
		float 	fSlow63 = (0.0010000000000000009f * iSlow62);
		int 	iSlow64 = primes((10 + fSlow2));
		float 	fSlow65 = (0.0010000000000000009f * iSlow64);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			iVec0[0] = 1;
			fVec1[0] = fSlow0;
			float fTemp0 = (fSlow0 + fVec1[1]);
			int iTemp1 = (1 - iVec0[1]);
			fRec6[0] = (fSlow4 + (0.999f * ((iSlow3 * iTemp1) + fRec6[1])));
			int iTemp2 = int((fRec6[0] - 1.49999f));
			float fTemp3 = (fRec6[0] - iTemp2);
			float fTemp4 = ((2.0f + iTemp2) - fRec6[0]);
			fRec10[0] = (fSlow7 + (0.999f * ((iSlow6 * iTemp1) + fRec10[1])));
			int iTemp5 = int((fRec10[0] - 1.49999f));
			float fTemp6 = (fRec10[0] - iTemp5);
			float fTemp7 = ((2.0f + iTemp5) - fRec10[0]);
			fRec14[0] = (fSlow10 + (0.999f * ((iSlow9 * iTemp1) + fRec14[1])));
			int iTemp8 = int((fRec14[0] - 1.49999f));
			float fTemp9 = (fRec14[0] - iTemp8);
			float fTemp10 = ((2.0f + iTemp8) - fRec14[0]);
			fRec18[0] = (fSlow13 + (0.9999f * ((iSlow12 * iTemp1) + fRec18[1])));
			int iTemp11 = int((fRec18[0] - 1.49999f));
			float fTemp12 = (fRec18[0] - iTemp11);
			float fTemp13 = ((2.0f + iTemp11) - fRec18[0]);
			fVec2[0] = fSlow14;
			float fTemp14 = (0.5f * (fSlow14 + fVec2[1]));
			float fTemp15 = sinf(fTemp14);
			float fTemp16 = (0 - fTemp15);
			float fTemp17 = (fTemp16 * fRec15[1]);
			fRec22[0] = (fSlow17 + (0.999f * ((iSlow16 * iTemp1) + fRec22[1])));
			int iTemp18 = int((fRec22[0] - 1.49999f));
			float fTemp19 = (fRec22[0] - iTemp18);
			float fTemp20 = ((2.0f + iTemp18) - fRec22[0]);
			fRec26[0] = (fSlow20 + (0.999f * ((iSlow19 * iTemp1) + fRec26[1])));
			int iTemp21 = int((fRec26[0] - 1.49999f));
			float fTemp22 = (fRec26[0] - iTemp21);
			float fTemp23 = ((2.0f + iTemp21) - fRec26[0]);
			fRec30[0] = (fSlow23 + (0.999f * ((iSlow22 * iTemp1) + fRec30[1])));
			int iTemp24 = int((fRec30[0] - 1.49999f));
			float fTemp25 = (fRec30[0] - iTemp24);
			float fTemp26 = ((2.0f + iTemp24) - fRec30[0]);
			fRec34[0] = (fSlow26 + (0.9999f * ((iSlow25 * iTemp1) + fRec34[1])));
			int iTemp27 = int((fRec34[0] - 1.49999f));
			float fTemp28 = (fRec34[0] - iTemp27);
			float fTemp29 = ((2.0f + iTemp27) - fRec34[0]);
			float fTemp30 = (0 - fTemp14);
			float fTemp31 = sinf(fTemp30);
			float fTemp32 = (0 - fTemp31);
			float fTemp33 = (fTemp32 * fRec31[1]);
			fRec38[0] = (fSlow28 + (0.999f * ((iSlow27 * iTemp1) + fRec38[1])));
			int iTemp34 = int((fRec38[0] - 1.49999f));
			float fTemp35 = (fRec38[0] - iTemp34);
			float fTemp36 = ((2.0f + iTemp34) - fRec38[0]);
			fRec42[0] = (fSlow31 + (0.999f * ((iSlow30 * iTemp1) + fRec42[1])));
			int iTemp37 = int((fRec42[0] - 1.49999f));
			float fTemp38 = (fRec42[0] - iTemp37);
			float fTemp39 = ((2.0f + iTemp37) - fRec42[0]);
			fRec46[0] = (fSlow34 + (0.999f * ((iSlow33 * iTemp1) + fRec46[1])));
			int iTemp40 = int((fRec46[0] - 1.49999f));
			float fTemp41 = (fRec46[0] - iTemp40);
			float fTemp42 = ((2.0f + iTemp40) - fRec46[0]);
			fRec50[0] = (fSlow37 + (0.9999f * ((iSlow36 * iTemp1) + fRec50[1])));
			int iTemp43 = int((fRec50[0] - 1.49999f));
			float fTemp44 = (fRec50[0] - iTemp43);
			float fTemp45 = ((2.0f + iTemp43) - fRec50[0]);
			float fTemp46 = (fTemp16 * fRec47[1]);
			fVec3[0] = fSlow38;
			float fTemp47 = (fConst1 * (fSlow38 + fVec3[1]));
			float fTemp48 = sinf(fTemp47);
			float fTemp49 = cosf(fTemp47);
			fRec51[0] = ((fTemp49 * fRec51[1]) + (fTemp48 * fRec52[1]));
			fRec52[0] = ((1 + (((0 - fTemp48) * fRec51[1]) + (fTemp49 * fRec52[1]))) - iVec0[1]);
			fVec4[0] = fSlow39;
			float fTemp50 = (fSlow39 + fVec4[1]);
			float fTemp51 = (fConst2 * (fTemp50 * (1.0f + fRec52[0])));
			int iTemp52 = int((8.50001f + fTemp51));
			float fTemp53 = ((8.0f + fTemp51) - iTemp52);
			float fTemp54 = ((1e+01f + fTemp51) - iTemp52);
			float fTemp55 = ((6.0f + fTemp51) - iTemp52);
			float fTemp56 = ((9.0f + fTemp51) - iTemp52);
			float fTemp57 = ((7.0f + fTemp51) - iTemp52);
			float fTemp58 = ((fTemp57 * fTemp54) * fTemp53);
			float fTemp59 = ((0.041666666666666664f * ((fTemp58 * fTemp56) * fRec0[(IOTA-int((1 + int((int((4 + iTemp52)) & 511)))))&1023])) + (0.16666666666666666f * ((((0 - (((fTemp54 * fTemp53) * fTemp56) * fTemp55)) * fRec0[(IOTA-int((1 + int((int((3 + iTemp52)) & 511)))))&1023]) + ((0 - (fTemp58 * fTemp55)) * fRec0[(IOTA-int((1 + int((int((1 + iTemp52)) & 511)))))&1023])) + (6.0f * (((fTemp57 * fTemp56) * fTemp55) * ((0.25f * (fTemp54 * fRec0[(IOTA-int((1 + int((int((2 + iTemp52)) & 511)))))&1023])) + (0.041666666666666664f * (fTemp53 * fRec0[(IOTA-int((1 + int((iTemp52 & 511)))))&1023]))))))));
			fVec5[IOTA&65535] = fTemp59;
			float fTemp60 = ((int((fRec53[1] != 0.0f)))?((int(((fRec54[1] > 0.0f) & (fRec54[1] < 1.0f))))?fRec53[1]:0):((int(((fRec54[1] == 0.0f) & (fSlow40 != fRec55[1]))))?4.5351473922902495e-05f:((int(((fRec54[1] == 1.0f) & (fSlow40 != fRec56[1]))))?-4.5351473922902495e-05f:0)));
			fRec53[0] = fTemp60;
			fRec54[0] = max(0.0f, min(1.0f, (fRec54[1] + fTemp60)));
			fRec55[0] = ((int(((fRec54[1] >= 1.0f) & (fRec56[1] != fSlow40))))?fSlow40:fRec55[1]);
			fRec56[0] = ((int(((fRec54[1] <= 0.0f) & (fRec55[1] != fSlow40))))?fSlow40:fRec56[1]);
			int iTemp61 = int((int(fRec55[0]) & 65535));
			float fTemp62 = (1.0f - fRec54[0]);
			int iTemp63 = int((int(fRec56[0]) & 65535));
			fVec6[0] = fSlow41;
			float fTemp64 = (fSlow41 + fVec6[1]);
			float fTemp65 = ((float)input0[i] + (0.5f * (fTemp64 * ((fRec54[0] * fVec5[(IOTA-iTemp63)&65535]) + (fTemp62 * fVec5[(IOTA-iTemp61)&65535])))));
			float fTemp66 = cosf(fTemp14);
			float fTemp67 = ((fTemp66 * fTemp65) + (fTemp16 * fRec35[1]));
			float fTemp68 = ((fTemp66 * fTemp67) + (fTemp16 * fRec39[1]));
			float fTemp69 = ((fTemp66 * fTemp68) + (fTemp16 * fRec43[1]));
			float fTemp70 = (fTemp66 * fTemp69);
			float fTemp71 = (fConst2 * (fTemp50 * (1.0f + fRec51[0])));
			int iTemp72 = int((8.50001f + fTemp71));
			float fTemp73 = ((fTemp71 + 8.0f) - iTemp72);
			float fTemp74 = ((1e+01f + fTemp71) - iTemp72);
			float fTemp75 = ((6.0f + fTemp71) - iTemp72);
			float fTemp76 = ((fTemp71 + 9.0f) - iTemp72);
			float fTemp77 = ((7.0f + fTemp71) - iTemp72);
			float fTemp78 = ((fTemp74 * fTemp77) * fTemp73);
			float fTemp79 = ((0.041666666666666664f * ((fTemp78 * fTemp76) * fRec1[(IOTA-int((1 + int((int((4 + iTemp72)) & 511)))))&1023])) + (0.16666666666666666f * ((((0 - (((fTemp74 * fTemp73) * fTemp76) * fTemp75)) * fRec1[(IOTA-int((1 + int((int((3 + iTemp72)) & 511)))))&1023]) + ((0 - (fTemp78 * fTemp75)) * fRec1[(IOTA-int((1 + int((int((1 + iTemp72)) & 511)))))&1023])) + (6.0f * (((fTemp77 * fTemp76) * fTemp75) * ((0.25f * (fTemp74 * fRec1[(IOTA-int((1 + int((int((2 + iTemp72)) & 511)))))&1023])) + (0.041666666666666664f * (fTemp73 * fRec1[(IOTA-int((1 + int((iTemp72 & 511)))))&1023]))))))));
			fVec7[IOTA&65535] = fTemp79;
			float fTemp80 = ((float)input1[i] + (0.5f * (fTemp64 * ((fRec54[0] * fVec7[(IOTA-iTemp63)&65535]) + (fTemp62 * fVec7[(IOTA-iTemp61)&65535])))));
			float fTemp81 = ((fTemp80 * fTemp66) + (fTemp16 * fRec36[1]));
			float fTemp82 = ((fTemp66 * fTemp81) + (fTemp16 * fRec40[1]));
			float fTemp83 = ((fTemp66 * fTemp82) + (fTemp16 * fRec44[1]));
			float fTemp84 = ((fTemp66 * fTemp83) + (fTemp16 * fRec48[1]));
			float fTemp85 = (0 - (fTemp84 - (6.123233995736766e-17f * (fTemp70 + fTemp46))));
			fVec8[IOTA&16383] = fTemp85;
			int iTemp86 = int((iTemp43 & 8191));
			fRec49[0] = ((fVec8[(IOTA-int((1 + iTemp86)))&16383] + ((fVec8[(IOTA-iTemp86)&16383] * fTemp45) / fTemp44)) + ((0 - (fTemp45 / fTemp44)) * fRec49[1]));
			fRec47[0] = fRec49[0];
			fRec58[0] = (fSlow43 + (0.9999f * ((iSlow42 * iTemp1) + fRec58[1])));
			int iTemp87 = int((fRec58[0] - 1.49999f));
			float fTemp88 = (fRec58[0] - iTemp87);
			float fTemp89 = ((2.0f + iTemp87) - fRec58[0]);
			float fTemp90 = (fTemp46 + ((6.123233995736766e-17f * fTemp84) + fTemp70));
			fVec9[IOTA&16383] = fTemp90;
			int iTemp91 = int((iTemp87 & 8191));
			fRec57[0] = ((fVec9[(IOTA-int((1 + iTemp91)))&16383] + ((fVec9[(IOTA-iTemp91)&16383] * fTemp89) / fTemp88)) + ((0 - (fTemp89 / fTemp88)) * fRec57[1]));
			fRec48[0] = fRec57[0];
			float fTemp92 = (fTemp66 * fRec47[1]);
			float fTemp93 = (fTemp15 * fTemp69);
			float fTemp94 = ((fTemp15 * fTemp83) + (fTemp66 * fRec48[1]));
			float fTemp95 = (0 - (fTemp94 - (6.123233995736766e-17f * (fTemp93 + fTemp92))));
			fVec10[IOTA&16383] = fTemp95;
			int iTemp96 = int((iTemp40 & 8191));
			fRec45[0] = ((fVec10[(IOTA-int((1 + iTemp96)))&16383] + ((fVec10[(IOTA-iTemp96)&16383] * fTemp42) / fTemp41)) + ((0 - (fTemp42 / fTemp41)) * fRec45[1]));
			fRec43[0] = fRec45[0];
			fRec60[0] = (fSlow45 + (0.999f * ((iSlow44 * iTemp1) + fRec60[1])));
			int iTemp97 = int((fRec60[0] - 1.49999f));
			float fTemp98 = (fRec60[0] - iTemp97);
			float fTemp99 = ((2.0f + iTemp97) - fRec60[0]);
			float fTemp100 = (fTemp92 + ((6.123233995736766e-17f * fTemp94) + fTemp93));
			fVec11[IOTA&16383] = fTemp100;
			int iTemp101 = int((iTemp97 & 8191));
			fRec59[0] = ((fVec11[(IOTA-int((1 + iTemp101)))&16383] + ((fVec11[(IOTA-iTemp101)&16383] * fTemp99) / fTemp98)) + ((0 - (fTemp99 / fTemp98)) * fRec59[1]));
			fRec44[0] = fRec59[0];
			float fTemp102 = (fTemp66 * fRec43[1]);
			float fTemp103 = (fTemp15 * fTemp68);
			float fTemp104 = ((fTemp15 * fTemp82) + (fTemp66 * fRec44[1]));
			float fTemp105 = (0 - (fTemp104 - (6.123233995736766e-17f * (fTemp103 + fTemp102))));
			fVec12[IOTA&16383] = fTemp105;
			int iTemp106 = int((iTemp37 & 8191));
			fRec41[0] = ((fVec12[(IOTA-int((1 + iTemp106)))&16383] + ((fVec12[(IOTA-iTemp106)&16383] * fTemp39) / fTemp38)) + ((0 - (fTemp39 / fTemp38)) * fRec41[1]));
			fRec39[0] = fRec41[0];
			fRec62[0] = (fSlow47 + (0.999f * ((iSlow46 * iTemp1) + fRec62[1])));
			int iTemp107 = int((fRec62[0] - 1.49999f));
			float fTemp108 = (fRec62[0] - iTemp107);
			float fTemp109 = ((2.0f + iTemp107) - fRec62[0]);
			float fTemp110 = (fTemp102 + ((6.123233995736766e-17f * fTemp104) + fTemp103));
			fVec13[IOTA&16383] = fTemp110;
			int iTemp111 = int((iTemp107 & 8191));
			fRec61[0] = ((fVec13[(IOTA-int((1 + iTemp111)))&16383] + ((fVec13[(IOTA-iTemp111)&16383] * fTemp109) / fTemp108)) + ((0 - (fTemp109 / fTemp108)) * fRec61[1]));
			fRec40[0] = fRec61[0];
			float fTemp112 = (fTemp66 * fRec39[1]);
			float fTemp113 = (fTemp15 * fTemp67);
			float fTemp114 = ((fTemp15 * fTemp81) + (fTemp66 * fRec40[1]));
			float fTemp115 = (0 - (fTemp114 - (6.123233995736766e-17f * (fTemp113 + fTemp112))));
			fVec14[IOTA&16383] = fTemp115;
			int iTemp116 = int((iTemp34 & 8191));
			fRec37[0] = ((fVec14[(IOTA-int((1 + iTemp116)))&16383] + ((fVec14[(IOTA-iTemp116)&16383] * fTemp36) / fTemp35)) + ((0 - (fTemp36 / fTemp35)) * fRec37[1]));
			fRec35[0] = fRec37[0];
			fRec64[0] = (fSlow49 + (0.999f * ((iSlow48 * iTemp1) + fRec64[1])));
			int iTemp117 = int((fRec64[0] - 1.49999f));
			float fTemp118 = (fRec64[0] - iTemp117);
			float fTemp119 = ((2.0f + iTemp117) - fRec64[0]);
			float fTemp120 = (fTemp112 + ((6.123233995736766e-17f * fTemp114) + fTemp113));
			fVec15[IOTA&16383] = fTemp120;
			int iTemp121 = int((iTemp117 & 8191));
			fRec63[0] = ((fVec15[(IOTA-int((1 + iTemp121)))&16383] + ((fVec15[(IOTA-iTemp121)&16383] * fTemp119) / fTemp118)) + ((0 - (fTemp119 / fTemp118)) * fRec63[1]));
			fRec36[0] = fRec63[0];
			float fTemp122 = ((fTemp15 * fTemp65) + (fTemp66 * fRec35[1]));
			float fTemp123 = cosf(fTemp30);
			float fTemp124 = ((fTemp123 * fTemp122) + (fTemp32 * fRec19[1]));
			float fTemp125 = ((fTemp123 * fTemp124) + (fTemp32 * fRec23[1]));
			float fTemp126 = ((fTemp123 * fTemp125) + (fTemp32 * fRec27[1]));
			float fTemp127 = (fTemp123 * fTemp126);
			float fTemp128 = ((fTemp15 * fTemp80) + (fTemp66 * fRec36[1]));
			float fTemp129 = ((fTemp128 * fTemp123) + (fTemp32 * fRec20[1]));
			float fTemp130 = ((fTemp123 * fTemp129) + (fTemp32 * fRec24[1]));
			float fTemp131 = ((fTemp123 * fTemp130) + (fTemp32 * fRec28[1]));
			float fTemp132 = ((fTemp123 * fTemp131) + (fTemp32 * fRec32[1]));
			float fTemp133 = (0 - (fTemp132 - (6.123233995736766e-17f * (fTemp127 + fTemp33))));
			fVec16[IOTA&16383] = fTemp133;
			int iTemp134 = int((iTemp27 & 8191));
			fRec33[0] = ((fVec16[(IOTA-int((1 + iTemp134)))&16383] + ((fVec16[(IOTA-iTemp134)&16383] * fTemp29) / fTemp28)) + ((0 - (fTemp29 / fTemp28)) * fRec33[1]));
			fRec31[0] = fRec33[0];
			fRec66[0] = (fSlow51 + (0.9999f * ((iSlow50 * iTemp1) + fRec66[1])));
			int iTemp135 = int((fRec66[0] - 1.49999f));
			float fTemp136 = (fRec66[0] - iTemp135);
			float fTemp137 = ((2.0f + iTemp135) - fRec66[0]);
			float fTemp138 = (fTemp33 + ((6.123233995736766e-17f * fTemp132) + fTemp127));
			fVec17[IOTA&16383] = fTemp138;
			int iTemp139 = int((iTemp135 & 8191));
			fRec65[0] = ((fVec17[(IOTA-int((1 + iTemp139)))&16383] + ((fVec17[(IOTA-iTemp139)&16383] * fTemp137) / fTemp136)) + ((0 - (fTemp137 / fTemp136)) * fRec65[1]));
			fRec32[0] = fRec65[0];
			float fTemp140 = (fTemp123 * fRec31[1]);
			float fTemp141 = (fTemp31 * fTemp126);
			float fTemp142 = ((fTemp31 * fTemp131) + (fTemp123 * fRec32[1]));
			float fTemp143 = (0 - (fTemp142 - (6.123233995736766e-17f * (fTemp141 + fTemp140))));
			fVec18[IOTA&16383] = fTemp143;
			int iTemp144 = int((iTemp24 & 8191));
			fRec29[0] = ((fVec18[(IOTA-int((1 + iTemp144)))&16383] + ((fVec18[(IOTA-iTemp144)&16383] * fTemp26) / fTemp25)) + ((0 - (fTemp26 / fTemp25)) * fRec29[1]));
			fRec27[0] = fRec29[0];
			fRec68[0] = (fSlow53 + (0.999f * ((iSlow52 * iTemp1) + fRec68[1])));
			int iTemp145 = int((fRec68[0] - 1.49999f));
			float fTemp146 = (fRec68[0] - iTemp145);
			float fTemp147 = ((2.0f + iTemp145) - fRec68[0]);
			float fTemp148 = (fTemp140 + ((6.123233995736766e-17f * fTemp142) + fTemp141));
			fVec19[IOTA&16383] = fTemp148;
			int iTemp149 = int((iTemp145 & 8191));
			fRec67[0] = ((fVec19[(IOTA-int((1 + iTemp149)))&16383] + ((fVec19[(IOTA-iTemp149)&16383] * fTemp147) / fTemp146)) + ((0 - (fTemp147 / fTemp146)) * fRec67[1]));
			fRec28[0] = fRec67[0];
			float fTemp150 = (fTemp123 * fRec27[1]);
			float fTemp151 = (fTemp31 * fTemp125);
			float fTemp152 = ((fTemp31 * fTemp130) + (fTemp123 * fRec28[1]));
			float fTemp153 = (0 - (fTemp152 - (6.123233995736766e-17f * (fTemp151 + fTemp150))));
			fVec20[IOTA&16383] = fTemp153;
			int iTemp154 = int((iTemp21 & 8191));
			fRec25[0] = ((fVec20[(IOTA-int((1 + iTemp154)))&16383] + ((fVec20[(IOTA-iTemp154)&16383] * fTemp23) / fTemp22)) + ((0 - (fTemp23 / fTemp22)) * fRec25[1]));
			fRec23[0] = fRec25[0];
			fRec70[0] = (fSlow55 + (0.999f * ((iSlow54 * iTemp1) + fRec70[1])));
			int iTemp155 = int((fRec70[0] - 1.49999f));
			float fTemp156 = (fRec70[0] - iTemp155);
			float fTemp157 = ((2.0f + iTemp155) - fRec70[0]);
			float fTemp158 = (fTemp150 + ((6.123233995736766e-17f * fTemp152) + fTemp151));
			fVec21[IOTA&16383] = fTemp158;
			int iTemp159 = int((iTemp155 & 8191));
			fRec69[0] = ((fVec21[(IOTA-int((1 + iTemp159)))&16383] + ((fVec21[(IOTA-iTemp159)&16383] * fTemp157) / fTemp156)) + ((0 - (fTemp157 / fTemp156)) * fRec69[1]));
			fRec24[0] = fRec69[0];
			float fTemp160 = (fTemp123 * fRec23[1]);
			float fTemp161 = (fTemp31 * fTemp124);
			float fTemp162 = ((fTemp31 * fTemp129) + (fTemp123 * fRec24[1]));
			float fTemp163 = (0 - (fTemp162 - (6.123233995736766e-17f * (fTemp161 + fTemp160))));
			fVec22[IOTA&16383] = fTemp163;
			int iTemp164 = int((iTemp18 & 8191));
			fRec21[0] = ((fVec22[(IOTA-int((1 + iTemp164)))&16383] + ((fVec22[(IOTA-iTemp164)&16383] * fTemp20) / fTemp19)) + ((0 - (fTemp20 / fTemp19)) * fRec21[1]));
			fRec19[0] = fRec21[0];
			fRec72[0] = (fSlow57 + (0.999f * ((iSlow56 * iTemp1) + fRec72[1])));
			int iTemp165 = int((fRec72[0] - 1.49999f));
			float fTemp166 = (fRec72[0] - iTemp165);
			float fTemp167 = ((2.0f + iTemp165) - fRec72[0]);
			float fTemp168 = (fTemp160 + ((6.123233995736766e-17f * fTemp162) + fTemp161));
			fVec23[IOTA&16383] = fTemp168;
			int iTemp169 = int((iTemp165 & 8191));
			fRec71[0] = ((fVec23[(IOTA-int((1 + iTemp169)))&16383] + ((fVec23[(IOTA-iTemp169)&16383] * fTemp167) / fTemp166)) + ((0 - (fTemp167 / fTemp166)) * fRec71[1]));
			fRec20[0] = fRec71[0];
			float fTemp170 = ((fTemp31 * fTemp122) + (fTemp123 * fRec19[1]));
			float fTemp171 = ((fTemp66 * fTemp170) + (fTemp16 * fRec3[1]));
			float fTemp172 = ((fTemp66 * fTemp171) + (fTemp16 * fRec7[1]));
			float fTemp173 = ((fTemp66 * fTemp172) + (fTemp16 * fRec11[1]));
			float fTemp174 = (fTemp66 * fTemp173);
			float fTemp175 = ((fTemp31 * fTemp128) + (fTemp123 * fRec20[1]));
			float fTemp176 = ((fTemp66 * fTemp175) + (fTemp16 * fRec4[1]));
			float fTemp177 = ((fTemp66 * fTemp176) + (fTemp16 * fRec8[1]));
			float fTemp178 = ((fTemp66 * fTemp177) + (fTemp16 * fRec12[1]));
			float fTemp179 = ((fTemp66 * fTemp178) + (fTemp16 * fRec16[1]));
			float fTemp180 = (0 - (fTemp179 - (6.123233995736766e-17f * (fTemp174 + fTemp17))));
			fVec24[IOTA&16383] = fTemp180;
			int iTemp181 = int((iTemp11 & 8191));
			fRec17[0] = ((fVec24[(IOTA-int((1 + iTemp181)))&16383] + ((fVec24[(IOTA-iTemp181)&16383] * fTemp13) / fTemp12)) + ((0 - (fTemp13 / fTemp12)) * fRec17[1]));
			fRec15[0] = fRec17[0];
			fRec74[0] = (fSlow59 + (0.9999f * ((iSlow58 * iTemp1) + fRec74[1])));
			int iTemp182 = int((fRec74[0] - 1.49999f));
			float fTemp183 = (fRec74[0] - iTemp182);
			float fTemp184 = ((2.0f + iTemp182) - fRec74[0]);
			float fTemp185 = (fTemp17 + ((6.123233995736766e-17f * fTemp179) + fTemp174));
			fVec25[IOTA&16383] = fTemp185;
			int iTemp186 = int((iTemp182 & 8191));
			fRec73[0] = ((fVec25[(IOTA-int((1 + iTemp186)))&16383] + ((fVec25[(IOTA-iTemp186)&16383] * fTemp184) / fTemp183)) + ((0 - (fTemp184 / fTemp183)) * fRec73[1]));
			fRec16[0] = fRec73[0];
			float fTemp187 = (fTemp66 * fRec15[1]);
			float fTemp188 = (fTemp15 * fTemp173);
			float fTemp189 = ((fTemp15 * fTemp178) + (fTemp66 * fRec16[1]));
			float fTemp190 = (0 - (fTemp189 - (6.123233995736766e-17f * (fTemp188 + fTemp187))));
			fVec26[IOTA&16383] = fTemp190;
			int iTemp191 = int((iTemp8 & 8191));
			fRec13[0] = ((fVec26[(IOTA-int((1 + iTemp191)))&16383] + ((fVec26[(IOTA-iTemp191)&16383] * fTemp10) / fTemp9)) + ((0 - (fTemp10 / fTemp9)) * fRec13[1]));
			fRec11[0] = fRec13[0];
			fRec76[0] = (fSlow61 + (0.999f * ((iSlow60 * iTemp1) + fRec76[1])));
			int iTemp192 = int((fRec76[0] - 1.49999f));
			float fTemp193 = (fRec76[0] - iTemp192);
			float fTemp194 = ((2.0f + iTemp192) - fRec76[0]);
			float fTemp195 = (fTemp187 + ((6.123233995736766e-17f * fTemp189) + fTemp188));
			fVec27[IOTA&16383] = fTemp195;
			int iTemp196 = int((iTemp192 & 8191));
			fRec75[0] = ((fVec27[(IOTA-int((1 + iTemp196)))&16383] + ((fVec27[(IOTA-iTemp196)&16383] * fTemp194) / fTemp193)) + ((0 - (fTemp194 / fTemp193)) * fRec75[1]));
			fRec12[0] = fRec75[0];
			float fTemp197 = (fTemp66 * fRec11[1]);
			float fTemp198 = (fTemp15 * fTemp172);
			float fTemp199 = ((fTemp15 * fTemp177) + (fTemp66 * fRec12[1]));
			float fTemp200 = (0 - (fTemp199 - (6.123233995736766e-17f * (fTemp198 + fTemp197))));
			fVec28[IOTA&16383] = fTemp200;
			int iTemp201 = int((iTemp5 & 8191));
			fRec9[0] = ((fVec28[(IOTA-int((1 + iTemp201)))&16383] + ((fVec28[(IOTA-iTemp201)&16383] * fTemp7) / fTemp6)) + ((0 - (fTemp7 / fTemp6)) * fRec9[1]));
			fRec7[0] = fRec9[0];
			fRec78[0] = (fSlow63 + (0.999f * ((iSlow62 * iTemp1) + fRec78[1])));
			int iTemp202 = int((fRec78[0] - 1.49999f));
			float fTemp203 = (fRec78[0] - iTemp202);
			float fTemp204 = ((2.0f + iTemp202) - fRec78[0]);
			float fTemp205 = (fTemp197 + ((6.123233995736766e-17f * fTemp199) + fTemp198));
			fVec29[IOTA&16383] = fTemp205;
			int iTemp206 = int((iTemp202 & 8191));
			fRec77[0] = ((fVec29[(IOTA-int((1 + iTemp206)))&16383] + ((fVec29[(IOTA-iTemp206)&16383] * fTemp204) / fTemp203)) + ((0 - (fTemp204 / fTemp203)) * fRec77[1]));
			fRec8[0] = fRec77[0];
			float fTemp207 = (fTemp66 * fRec7[1]);
			float fTemp208 = (fTemp15 * fTemp171);
			float fTemp209 = ((fTemp15 * fTemp176) + (fTemp66 * fRec8[1]));
			float fTemp210 = (0 - (fTemp209 - (6.123233995736766e-17f * (fTemp208 + fTemp207))));
			fVec30[IOTA&16383] = fTemp210;
			int iTemp211 = int((iTemp2 & 8191));
			fRec5[0] = ((fVec30[(IOTA-int((1 + iTemp211)))&16383] + ((fVec30[(IOTA-iTemp211)&16383] * fTemp4) / fTemp3)) + ((0 - (fTemp4 / fTemp3)) * fRec5[1]));
			fRec3[0] = fRec5[0];
			fRec80[0] = (fSlow65 + (0.999f * ((iSlow64 * iTemp1) + fRec80[1])));
			int iTemp212 = int((fRec80[0] - 1.49999f));
			float fTemp213 = (fRec80[0] - iTemp212);
			float fTemp214 = ((2.0f + iTemp212) - fRec80[0]);
			float fTemp215 = (fTemp207 + ((6.123233995736766e-17f * fTemp209) + fTemp208));
			fVec31[IOTA&16383] = fTemp215;
			int iTemp216 = int((iTemp212 & 8191));
			fRec79[0] = ((fVec31[(IOTA-int((1 + iTemp216)))&16383] + ((fVec31[(IOTA-iTemp216)&16383] * fTemp214) / fTemp213)) + ((0 - (fTemp214 / fTemp213)) * fRec79[1]));
			fRec4[0] = fRec79[0];
			float fTemp217 = (1.0f - (0.5f * fTemp0));
			fRec2[0] = ((fTemp217 * ((fTemp15 * fTemp170) + (fTemp66 * fRec3[1]))) + (0.5f * (fTemp0 * fRec2[1])));
			fRec0[IOTA&1023] = fRec2[0];
			fRec81[0] = ((fTemp217 * ((fTemp15 * fTemp175) + (fTemp66 * fRec4[1]))) + (0.5f * (fTemp0 * fRec81[1])));
			fRec1[IOTA&1023] = fRec81[0];
			output0[i] = (FAUSTFLOAT)fRec0[(IOTA-0)&1023];
			output1[i] = (FAUSTFLOAT)fRec1[(IOTA-0)&1023];
			// post processing
			fRec81[1] = fRec81[0];
			fRec2[1] = fRec2[0];
			fRec4[1] = fRec4[0];
			fRec79[1] = fRec79[0];
			fRec80[1] = fRec80[0];
			fRec3[1] = fRec3[0];
			fRec5[1] = fRec5[0];
			fRec8[1] = fRec8[0];
			fRec77[1] = fRec77[0];
			fRec78[1] = fRec78[0];
			fRec7[1] = fRec7[0];
			fRec9[1] = fRec9[0];
			fRec12[1] = fRec12[0];
			fRec75[1] = fRec75[0];
			fRec76[1] = fRec76[0];
			fRec11[1] = fRec11[0];
			fRec13[1] = fRec13[0];
			fRec16[1] = fRec16[0];
			fRec73[1] = fRec73[0];
			fRec74[1] = fRec74[0];
			fRec15[1] = fRec15[0];
			fRec17[1] = fRec17[0];
			fRec20[1] = fRec20[0];
			fRec71[1] = fRec71[0];
			fRec72[1] = fRec72[0];
			fRec19[1] = fRec19[0];
			fRec21[1] = fRec21[0];
			fRec24[1] = fRec24[0];
			fRec69[1] = fRec69[0];
			fRec70[1] = fRec70[0];
			fRec23[1] = fRec23[0];
			fRec25[1] = fRec25[0];
			fRec28[1] = fRec28[0];
			fRec67[1] = fRec67[0];
			fRec68[1] = fRec68[0];
			fRec27[1] = fRec27[0];
			fRec29[1] = fRec29[0];
			fRec32[1] = fRec32[0];
			fRec65[1] = fRec65[0];
			fRec66[1] = fRec66[0];
			fRec31[1] = fRec31[0];
			fRec33[1] = fRec33[0];
			fRec36[1] = fRec36[0];
			fRec63[1] = fRec63[0];
			fRec64[1] = fRec64[0];
			fRec35[1] = fRec35[0];
			fRec37[1] = fRec37[0];
			fRec40[1] = fRec40[0];
			fRec61[1] = fRec61[0];
			fRec62[1] = fRec62[0];
			fRec39[1] = fRec39[0];
			fRec41[1] = fRec41[0];
			fRec44[1] = fRec44[0];
			fRec59[1] = fRec59[0];
			fRec60[1] = fRec60[0];
			fRec43[1] = fRec43[0];
			fRec45[1] = fRec45[0];
			fRec48[1] = fRec48[0];
			fRec57[1] = fRec57[0];
			fRec58[1] = fRec58[0];
			fRec47[1] = fRec47[0];
			fRec49[1] = fRec49[0];
			fVec6[1] = fVec6[0];
			fRec56[1] = fRec56[0];
			fRec55[1] = fRec55[0];
			fRec54[1] = fRec54[0];
			fRec53[1] = fRec53[0];
			IOTA = IOTA+1;
			fVec4[1] = fVec4[0];
			fRec52[1] = fRec52[0];
			fRec51[1] = fRec51[0];
			fVec3[1] = fVec3[0];
			fRec50[1] = fRec50[0];
			fRec46[1] = fRec46[0];
			fRec42[1] = fRec42[0];
			fRec38[1] = fRec38[0];
			fRec34[1] = fRec34[0];
			fRec30[1] = fRec30[0];
			fRec26[1] = fRec26[0];
			fRec22[1] = fRec22[0];
			fVec2[1] = fVec2[0];
			fRec18[1] = fRec18[0];
			fRec14[1] = fRec14[0];
			fRec10[1] = fRec10[0];
			fRec6[1] = fRec6[0];
			fVec1[1] = fVec1[0];
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
