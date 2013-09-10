//-----------------------------------------------------
// name: "JP_dreamverb_proto"
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
template <int N> inline long double faustpower(long double x) 	{ return powl(x,N); }
template <int N> inline int faustpower(int x)              { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline int faustpower<0>(int x)            { return 1; }
template <> 	 inline int faustpower<1>(int x)            { return x; }
#endif
// If other than 'faust2sc --prefix Faust' is used, sed this as well:
#define SC_FAUST_PREFIX "DEIND"

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
	float 	fRec0[2];
	FAUSTFLOAT 	fslider1;
	FAUSTFLOAT 	fslider2;
	float 	fRec15[2];
	FAUSTFLOAT 	fslider3;
	FAUSTFLOAT 	fslider4;
	float 	fRec17[2];
	int 	iConst0;
	float 	fConst1;
	FAUSTFLOAT 	fslider5;
	float 	fRec21[2];
	int 	IOTA;
	float 	fVec1[65536];
	float 	fRec61[2];
	float 	fVec2[65536];
	float 	fRec62[2];
	float 	fVec3[65536];
	float 	fRec57[2];
	float 	fVec4[65536];
	float 	fRec58[2];
	float 	fVec5[65536];
	float 	fRec53[2];
	float 	fVec6[65536];
	float 	fRec54[2];
	float 	fVec7[65536];
	float 	fRec49[2];
	float 	fVec8[65536];
	float 	fRec50[2];
	float 	fVec9[65536];
	float 	fRec45[2];
	float 	fVec10[65536];
	float 	fRec46[2];
	float 	fVec11[65536];
	FAUSTFLOAT 	fslider6;
	float 	fRec65[2];
	FAUSTFLOAT 	fslider7;
	float 	fRec68[2];
	float 	fConst2;
	float 	fRec66[2];
	float 	fRec67[2];
	float 	fVec12[65536];
	float 	fVec13[65536];
	float 	fRec41[2];
	float 	fVec14[65536];
	float 	fRec42[2];
	float 	fVec15[65536];
	float 	fRec37[2];
	float 	fVec16[65536];
	float 	fRec38[2];
	float 	fVec17[65536];
	float 	fRec33[2];
	float 	fVec18[65536];
	float 	fRec34[2];
	float 	fVec19[65536];
	float 	fRec29[2];
	float 	fVec20[65536];
	float 	fRec30[2];
	float 	fVec21[65536];
	float 	fRec25[2];
	float 	fVec22[65536];
	float 	fRec26[2];
	float 	fVec23[65536];
	float 	fVec24[2];
	float 	fRec24[2];
	float 	fRec23[3];
	float 	fRec22[3];
	float 	fVec25[2];
	float 	fRec20[2];
	float 	fRec19[3];
	float 	fRec18[3];
	FAUSTFLOAT 	fslider8;
	float 	fRec69[2];
	float 	fRec72[2];
	float 	fRec71[3];
	float 	fRec70[3];
	FAUSTFLOAT 	fslider9;
	float 	fRec73[2];
	float 	fRec77[2];
	float 	fRec76[3];
	float 	fRec75[3];
	float 	fRec74[3];
	FAUSTFLOAT 	fslider10;
	float 	fRec78[2];
	FAUSTFLOAT 	fslider11;
	float 	fRec79[2];
	float 	fRec16[2];
	float 	fVec26[65536];
	float 	fVec27[65536];
	float 	fVec28[2];
	float 	fRec86[2];
	float 	fRec85[3];
	float 	fRec84[3];
	float 	fVec29[2];
	float 	fRec83[2];
	float 	fRec82[3];
	float 	fRec81[3];
	float 	fRec89[2];
	float 	fRec88[3];
	float 	fRec87[3];
	float 	fRec93[2];
	float 	fRec92[3];
	float 	fRec91[3];
	float 	fRec90[3];
	float 	fRec80[2];
	float 	fVec30[65536];
	float 	fVec31[65536];
	float 	fRec11[2];
	float 	fVec32[65536];
	float 	fRec12[2];
	float 	fVec33[65536];
	float 	fRec7[2];
	float 	fVec34[65536];
	float 	fRec8[2];
	float 	fVec35[65536];
	float 	fRec3[2];
	float 	fVec36[65536];
	float 	fRec4[2];
	float 	fRec1[2];
	float 	fRec2[2];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("name", "JP_dreamverb_proto");
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
		fslider0 = 0.5f;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		fslider1 = 0.707f;
		fslider2 = 1.0f;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		fslider3 = 1.0f;
		fslider4 = 5e+02f;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (3.141592653589793f / float(iConst0));
		fslider5 = 2e+03f;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		IOTA = 0;
		for (int i=0; i<65536; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<65536; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<65536; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<65536; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<65536; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<65536; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<65536; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<65536; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<65536; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<65536; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<65536; i++) fVec11[i] = 0;
		fslider6 = 5.0f;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		fslider7 = 2.0f;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		fConst2 = (6.283185307179586f / float(iConst0));
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<65536; i++) fVec12[i] = 0;
		for (int i=0; i<65536; i++) fVec13[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<65536; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<65536; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<65536; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<65536; i++) fVec17[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<65536; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<65536; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<65536; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<65536; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<65536; i++) fVec22[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<65536; i++) fVec23[i] = 0;
		for (int i=0; i<2; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<3; i++) fRec23[i] = 0;
		for (int i=0; i<3; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fVec25[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<3; i++) fRec19[i] = 0;
		for (int i=0; i<3; i++) fRec18[i] = 0;
		fslider8 = 1.0f;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<3; i++) fRec71[i] = 0;
		for (int i=0; i<3; i++) fRec70[i] = 0;
		fslider9 = 1.0f;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<3; i++) fRec76[i] = 0;
		for (int i=0; i<3; i++) fRec75[i] = 0;
		for (int i=0; i<3; i++) fRec74[i] = 0;
		fslider10 = 1.0f;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		fslider11 = 0.0f;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<65536; i++) fVec26[i] = 0;
		for (int i=0; i<65536; i++) fVec27[i] = 0;
		for (int i=0; i<2; i++) fVec28[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<3; i++) fRec85[i] = 0;
		for (int i=0; i<3; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fVec29[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<3; i++) fRec82[i] = 0;
		for (int i=0; i<3; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec89[i] = 0;
		for (int i=0; i<3; i++) fRec88[i] = 0;
		for (int i=0; i<3; i++) fRec87[i] = 0;
		for (int i=0; i<2; i++) fRec93[i] = 0;
		for (int i=0; i<3; i++) fRec92[i] = 0;
		for (int i=0; i<3; i++) fRec91[i] = 0;
		for (int i=0; i<3; i++) fRec90[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<65536; i++) fVec30[i] = 0;
		for (int i=0; i<65536; i++) fVec31[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<65536; i++) fVec32[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<65536; i++) fVec33[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<65536; i++) fVec34[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<65536; i++) fVec35[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<65536; i++) fVec36[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("reverb");
		interface->addHorizontalSlider("damp", &fslider11, 0.0f, 0.0f, 0.999f, 0.0001f);
		interface->addHorizontalSlider("earlyDiff", &fslider1, 0.707f, 0.0f, 0.99f, 0.001f);
		interface->addHorizontalSlider("highBand", &fslider5, 2e+03f, 1e+03f, 1e+04f, 0.1f);
		interface->addHorizontalSlider("highX", &fslider10, 1.0f, 0.0f, 1.0f, 0.01f);
		interface->addHorizontalSlider("lowBand", &fslider4, 5e+02f, 1e+02f, 6e+03f, 0.1f);
		interface->addHorizontalSlider("lowX", &fslider9, 1.0f, 0.0f, 1.0f, 0.01f);
		interface->addHorizontalSlider("mDepth", &fslider6, 5.0f, 0.0f, 5e+01f, 0.01f);
		interface->addHorizontalSlider("mFreq", &fslider7, 2.0f, 0.0f, 1e+01f, 0.01f);
		interface->addHorizontalSlider("midX", &fslider8, 1.0f, 0.0f, 1.0f, 0.01f);
		interface->addHorizontalSlider("size", &fslider3, 1.0f, 0.7f, 5.0f, 0.01f);
		interface->addHorizontalSlider("t60", &fslider2, 1.0f, 0.1f, 6e+01f, 0.1f);
		interface->addHorizontalSlider("wet", &fslider0, 0.5f, 0.0f, 1.0f, 0.01f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = (0.010000000000000009f * float(fslider0));
		float 	fSlow1 = float(fslider1);
		float 	fSlow2 = (0 - fSlow1);
		float 	fSlow3 = (0.010000000000000009f * float(fslider2));
		float 	fSlow4 = float(fslider3);
		float 	fSlow5 = (0.45000000000000007f * ((1.25f * fSlow4) - 0.25f));
		float 	fSlow6 = (0.010000000000000009f * float(fslider4));
		float 	fSlow7 = (0.010000000000000009f * float(fslider5));
		int 	iSlow8 = int((int((primes((10 * fSlow4)) - 1)) & 65535));
		int 	iSlow9 = int((int((primes((100 * fSlow4)) - 1)) & 65535));
		int 	iSlow10 = int((int((primes((30 * fSlow4)) - 1)) & 65535));
		int 	iSlow11 = int((int((primes((105 * fSlow4)) - 1)) & 65535));
		int 	iSlow12 = int((int((primes((50 * fSlow4)) - 1)) & 65535));
		int 	iSlow13 = int((int((primes((110 * fSlow4)) - 1)) & 65535));
		int 	iSlow14 = int((int((primes((70 * fSlow4)) - 1)) & 65535));
		int 	iSlow15 = int((int((primes((115 * fSlow4)) - 1)) & 65535));
		int 	iSlow16 = int((int((primes((90 * fSlow4)) - 1)) & 65535));
		int 	iSlow17 = int((int((primes((120 * fSlow4)) - 1)) & 65535));
		float 	fSlow18 = (0.010000000000000009f * float(fslider6));
		float 	fSlow19 = (0.010000000000000009f * float(fslider7));
		int 	iSlow20 = primes((24 * fSlow4));
		float 	fSlow21 = (iSlow20 - 2.49999f);
		float 	fSlow22 = (iSlow20 - 3.0f);
		float 	fSlow23 = (iSlow20 - 1.0f);
		float 	fSlow24 = (iSlow20 - 5.0f);
		float 	fSlow25 = (iSlow20 - 2.0f);
		float 	fSlow26 = (iSlow20 - 4.0f);
		int 	iSlow27 = primes((174 * fSlow4));
		float 	fSlow28 = (iSlow27 - 2.49999f);
		float 	fSlow29 = (iSlow27 - 3.0f);
		float 	fSlow30 = (iSlow27 - 1.0f);
		float 	fSlow31 = (iSlow27 - 5.0f);
		float 	fSlow32 = (iSlow27 - 2.0f);
		float 	fSlow33 = (iSlow27 - 4.0f);
		int 	iSlow34 = int((int((primes((95 * fSlow4)) - 1)) & 65535));
		int 	iSlow35 = int((int((primes((23 * fSlow4)) - 1)) & 65535));
		int 	iSlow36 = int((int((primes((135 * fSlow4)) - 1)) & 65535));
		int 	iSlow37 = int((int((primes((37 * fSlow4)) - 1)) & 65535));
		int 	iSlow38 = int((int((primes((155 * fSlow4)) - 1)) & 65535));
		int 	iSlow39 = int((int((primes((44 * fSlow4)) - 1)) & 65535));
		int 	iSlow40 = int((int((primes((175 * fSlow4)) - 1)) & 65535));
		int 	iSlow41 = int((int((primes((51 * fSlow4)) - 1)) & 65535));
		float 	fSlow42 = (0.010000000000000009f * float(fslider8));
		float 	fSlow43 = (0.010000000000000009f * float(fslider9));
		float 	fSlow44 = (0.010000000000000009f * float(fslider10));
		float 	fSlow45 = (0.010000000000000009f * float(fslider11));
		int 	iSlow46 = primes((124 * fSlow4));
		float 	fSlow47 = (iSlow46 - 2.49999f);
		float 	fSlow48 = (iSlow46 - 3.0f);
		float 	fSlow49 = (iSlow46 - 1.0f);
		float 	fSlow50 = (iSlow46 - 5.0f);
		float 	fSlow51 = (iSlow46 - 2.0f);
		float 	fSlow52 = (iSlow46 - 4.0f);
		int 	iSlow53 = int((int((primes((25 * fSlow4)) - 1)) & 65535));
		int 	iSlow54 = int((int((primes((180 * fSlow4)) - 1)) & 65535));
		int 	iSlow55 = int((int((primes((65 * fSlow4)) - 1)) & 65535));
		int 	iSlow56 = int((int((primes((147 * fSlow4)) - 1)) & 65535));
		int 	iSlow57 = int((int((primes((114 * fSlow4)) - 1)) & 65535));
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			iVec0[0] = 1;
			fRec0[0] = (fSlow0 + (0.99f * fRec0[1]));
			float fTemp0 = (1 - fRec0[0]);
			float fTemp1 = (float)input0[i];
			fRec15[0] = (fSlow3 + (0.99f * fRec15[1]));
			float fTemp2 = powf(10,(0 - (fSlow5 / fRec15[0])));
			fRec17[0] = (fSlow6 + (0.99f * fRec17[1]));
			float fTemp3 = tanf((fConst1 * fRec17[0]));
			float fTemp4 = (1.0f / fTemp3);
			float fTemp5 = (1 + ((0.6180339887498947f + fTemp4) / fTemp3));
			float fTemp6 = faustpower<2>(fTemp3);
			float fTemp7 = (1.0f / fTemp6);
			float fTemp8 = (1 - fTemp7);
			float fTemp9 = (1 + ((fTemp4 - 0.6180339887498947f) / fTemp3));
			float fTemp10 = (1 + ((1.6180339887498947f + fTemp4) / fTemp3));
			float fTemp11 = (1 + ((fTemp4 - 1.6180339887498947f) / fTemp3));
			float fTemp12 = (1 + fTemp4);
			fRec21[0] = (fSlow7 + (0.99f * fRec21[1]));
			float fTemp13 = tanf((fConst1 * fRec21[0]));
			float fTemp14 = (1.0f / fTemp13);
			float fTemp15 = (1 + ((0.6180339887498947f + fTemp14) / fTemp13));
			float fTemp16 = (fTemp3 * fTemp15);
			float fTemp17 = faustpower<2>(fTemp13);
			float fTemp18 = (1.0f / fTemp17);
			float fTemp19 = (1 - fTemp18);
			float fTemp20 = (1 + ((fTemp14 - 0.6180339887498947f) / fTemp13));
			float fTemp21 = (1 + ((1.6180339887498947f + fTemp14) / fTemp13));
			float fTemp22 = (1 + ((fTemp14 - 1.6180339887498947f) / fTemp13));
			float fTemp23 = (1 + fTemp14);
			float fTemp24 = (0 - ((1 - fTemp14) / fTemp23));
			float fTemp25 = (fRec2[1] - (0.707f * fRec62[1]));
			float fTemp26 = (fRec1[1] - (0.707f * fRec61[1]));
			fVec1[IOTA&65535] = ((0.7071067811865476f * fTemp26) - (0.7071067811865475f * fTemp25));
			fRec61[0] = fVec1[(IOTA-iSlow8)&65535];
			fVec2[IOTA&65535] = ((0.7071067811865476f * fTemp25) + (0.7071067811865475f * fTemp26));
			fRec62[0] = fVec2[(IOTA-iSlow9)&65535];
			float 	fRec63 = (0.707f * fTemp26);
			float 	fRec64 = (0.707f * fTemp25);
			float fTemp27 = ((fRec64 + fRec62[1]) - (0.707f * fRec58[1]));
			float fTemp28 = ((fRec63 + fRec61[1]) - (0.707f * fRec57[1]));
			fVec3[IOTA&65535] = ((0.7071067811865476f * fTemp28) - (0.7071067811865475f * fTemp27));
			fRec57[0] = fVec3[(IOTA-iSlow10)&65535];
			fVec4[IOTA&65535] = ((0.7071067811865476f * fTemp27) + (0.7071067811865475f * fTemp28));
			fRec58[0] = fVec4[(IOTA-iSlow11)&65535];
			float 	fRec59 = (0.707f * fTemp28);
			float 	fRec60 = (0.707f * fTemp27);
			float fTemp29 = ((fRec60 + fRec58[1]) - (0.707f * fRec54[1]));
			float fTemp30 = ((fRec59 + fRec57[1]) - (0.707f * fRec53[1]));
			fVec5[IOTA&65535] = ((0.7071067811865476f * fTemp30) - (0.7071067811865475f * fTemp29));
			fRec53[0] = fVec5[(IOTA-iSlow12)&65535];
			fVec6[IOTA&65535] = ((0.7071067811865476f * fTemp29) + (0.7071067811865475f * fTemp30));
			fRec54[0] = fVec6[(IOTA-iSlow13)&65535];
			float 	fRec55 = (0.707f * fTemp30);
			float 	fRec56 = (0.707f * fTemp29);
			float fTemp31 = ((fRec56 + fRec54[1]) - (0.707f * fRec50[1]));
			float fTemp32 = ((fRec55 + fRec53[1]) - (0.707f * fRec49[1]));
			fVec7[IOTA&65535] = ((0.7071067811865476f * fTemp32) - (0.7071067811865475f * fTemp31));
			fRec49[0] = fVec7[(IOTA-iSlow14)&65535];
			fVec8[IOTA&65535] = ((0.7071067811865476f * fTemp31) + (0.7071067811865475f * fTemp32));
			fRec50[0] = fVec8[(IOTA-iSlow15)&65535];
			float 	fRec51 = (0.707f * fTemp32);
			float 	fRec52 = (0.707f * fTemp31);
			float fTemp33 = ((fRec52 + fRec50[1]) - (0.707f * fRec46[1]));
			float fTemp34 = ((fRec51 + fRec49[1]) - (0.707f * fRec45[1]));
			fVec9[IOTA&65535] = ((0.7071067811865476f * fTemp34) - (0.7071067811865475f * fTemp33));
			fRec45[0] = fVec9[(IOTA-iSlow16)&65535];
			fVec10[IOTA&65535] = ((0.7071067811865476f * fTemp33) + (0.7071067811865475f * fTemp34));
			fRec46[0] = fVec10[(IOTA-iSlow17)&65535];
			float 	fRec47 = (0.707f * fTemp34);
			float 	fRec48 = (0.707f * fTemp33);
			float fTemp35 = (fRec48 + fRec46[1]);
			fVec11[IOTA&65535] = fTemp35;
			fRec65[0] = (fSlow18 + (0.99f * fRec65[1]));
			float fTemp36 = (0 - fRec65[0]);
			fRec68[0] = (fSlow19 + (0.99f * fRec68[1]));
			float fTemp37 = (fConst2 * fRec68[0]);
			float fTemp38 = sinf(fTemp37);
			float fTemp39 = cosf(fTemp37);
			fRec66[0] = ((fRec66[1] * fTemp39) + (fTemp38 * fRec67[1]));
			fRec67[0] = ((1 + (((0 - fTemp38) * fRec66[1]) + (fTemp39 * fRec67[1]))) - iVec0[1]);
			float fTemp40 = (fRec65[0] + (fRec67[0] * fTemp36));
			int iTemp41 = int((fSlow21 + fTemp40));
			float fTemp42 = ((fSlow22 + fTemp40) - iTemp41);
			float fTemp43 = ((fSlow23 + fTemp40) - iTemp41);
			float fTemp44 = ((fTemp40 + fSlow24) - iTemp41);
			float fTemp45 = ((fSlow25 + fTemp40) - iTemp41);
			float fTemp46 = ((fSlow26 + fTemp40) - iTemp41);
			float fTemp47 = ((fTemp43 * fTemp46) * fTemp42);
			float fTemp48 = (((0.041666666666666664f * ((fTemp47 * fTemp45) * fVec11[(IOTA-int((int((4 + iTemp41)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp43 * fTemp42) * fTemp45) * fTemp44)) * fVec11[(IOTA-int((int((3 + iTemp41)) & 65535)))&65535]) + ((0 - (fTemp47 * fTemp44)) * fVec11[(IOTA-int((int((1 + iTemp41)) & 65535)))&65535])) + (6.0f * (((fTemp46 * fTemp45) * fTemp44) * ((0.25f * (fTemp43 * fVec11[(IOTA-int((int((2 + iTemp41)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp42 * fVec11[(IOTA-int((iTemp41 & 65535)))&65535])))))))) - (0.707f * fRec42[1]));
			float fTemp49 = (fRec47 + fRec45[1]);
			fVec12[IOTA&65535] = fTemp49;
			float fTemp50 = (fRec65[0] * (1 + fRec67[0]));
			int iTemp51 = int((fSlow28 + fTemp50));
			float fTemp52 = ((fTemp50 + fSlow29) - iTemp51);
			float fTemp53 = ((fTemp50 + fSlow30) - iTemp51);
			float fTemp54 = ((fTemp50 + fSlow31) - iTemp51);
			float fTemp55 = ((fTemp50 + fSlow32) - iTemp51);
			float fTemp56 = ((fTemp50 + fSlow33) - iTemp51);
			float fTemp57 = ((fTemp53 * fTemp56) * fTemp52);
			float fTemp58 = (((0.041666666666666664f * ((fTemp57 * fTemp55) * fVec12[(IOTA-int((int((4 + iTemp51)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp53 * fTemp52) * fTemp55) * fTemp54)) * fVec12[(IOTA-int((int((3 + iTemp51)) & 65535)))&65535]) + ((0 - (fTemp57 * fTemp54)) * fVec12[(IOTA-int((int((1 + iTemp51)) & 65535)))&65535])) + (6.0f * (((fTemp56 * fTemp55) * fTemp54) * ((0.25f * (fTemp53 * fVec12[(IOTA-int((int((2 + iTemp51)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp52 * fVec12[(IOTA-int((iTemp51 & 65535)))&65535])))))))) - (0.707f * fRec41[1]));
			fVec13[IOTA&65535] = ((0.7071067811865476f * fTemp58) - (0.7071067811865475f * fTemp48));
			fRec41[0] = fVec13[(IOTA-iSlow34)&65535];
			fVec14[IOTA&65535] = ((0.7071067811865476f * fTemp48) + (0.7071067811865475f * fTemp58));
			fRec42[0] = fVec14[(IOTA-iSlow35)&65535];
			float 	fRec43 = (0.707f * fTemp58);
			float 	fRec44 = (0.707f * fTemp48);
			float fTemp59 = ((fRec44 + fRec42[1]) - (0.707f * fRec38[1]));
			float fTemp60 = ((fRec43 + fRec41[1]) - (0.707f * fRec37[1]));
			fVec15[IOTA&65535] = ((0.7071067811865476f * fTemp60) - (0.7071067811865475f * fTemp59));
			fRec37[0] = fVec15[(IOTA-iSlow15)&65535];
			fVec16[IOTA&65535] = ((0.7071067811865476f * fTemp59) + (0.7071067811865475f * fTemp60));
			fRec38[0] = fVec16[(IOTA-iSlow10)&65535];
			float 	fRec39 = (0.707f * fTemp60);
			float 	fRec40 = (0.707f * fTemp59);
			float fTemp61 = ((fRec40 + fRec38[1]) - (0.707f * fRec34[1]));
			float fTemp62 = ((fRec39 + fRec37[1]) - (0.707f * fRec33[1]));
			fVec17[IOTA&65535] = ((0.7071067811865476f * fTemp62) - (0.7071067811865475f * fTemp61));
			fRec33[0] = fVec17[(IOTA-iSlow36)&65535];
			fVec18[IOTA&65535] = ((0.7071067811865476f * fTemp61) + (0.7071067811865475f * fTemp62));
			fRec34[0] = fVec18[(IOTA-iSlow37)&65535];
			float 	fRec35 = (0.707f * fTemp62);
			float 	fRec36 = (0.707f * fTemp61);
			float fTemp63 = ((fRec36 + fRec34[1]) - (0.707f * fRec30[1]));
			float fTemp64 = ((fRec35 + fRec33[1]) - (0.707f * fRec29[1]));
			fVec19[IOTA&65535] = ((0.7071067811865476f * fTemp64) - (0.7071067811865475f * fTemp63));
			fRec29[0] = fVec19[(IOTA-iSlow38)&65535];
			fVec20[IOTA&65535] = ((0.7071067811865476f * fTemp63) + (0.7071067811865475f * fTemp64));
			fRec30[0] = fVec20[(IOTA-iSlow39)&65535];
			float 	fRec31 = (0.707f * fTemp64);
			float 	fRec32 = (0.707f * fTemp63);
			float fTemp65 = ((fRec32 + fRec30[1]) - (0.707f * fRec26[1]));
			float fTemp66 = ((fRec31 + fRec29[1]) - (0.707f * fRec25[1]));
			fVec21[IOTA&65535] = ((0.7071067811865476f * fTemp66) - (0.7071067811865475f * fTemp65));
			fRec25[0] = fVec21[(IOTA-iSlow40)&65535];
			fVec22[IOTA&65535] = ((0.7071067811865476f * fTemp65) + (0.7071067811865475f * fTemp66));
			fRec26[0] = fVec22[(IOTA-iSlow41)&65535];
			float 	fRec27 = (0.707f * fTemp66);
			float 	fRec28 = (0.707f * fTemp65);
			float fTemp67 = (fRec28 + fRec26[1]);
			fVec23[IOTA&65535] = fTemp67;
			float fTemp68 = (fRec65[0] + (fRec66[0] * fTemp36));
			int iTemp69 = int((fSlow21 + fTemp68));
			float fTemp70 = ((fTemp68 + fSlow22) - iTemp69);
			float fTemp71 = ((fTemp68 + fSlow23) - iTemp69);
			float fTemp72 = ((fTemp68 + fSlow24) - iTemp69);
			float fTemp73 = ((fTemp68 + fSlow25) - iTemp69);
			float fTemp74 = ((fTemp68 + fSlow26) - iTemp69);
			float fTemp75 = ((fTemp71 * fTemp74) * fTemp70);
			float fTemp76 = ((0.041666666666666664f * ((fTemp75 * fTemp73) * fVec23[(IOTA-int((int((4 + iTemp69)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp71 * fTemp70) * fTemp73) * fTemp72)) * fVec23[(IOTA-int((int((3 + iTemp69)) & 65535)))&65535]) + ((0 - (fTemp75 * fTemp72)) * fVec23[(IOTA-int((int((1 + iTemp69)) & 65535)))&65535])) + (6.0f * (((fTemp74 * fTemp73) * fTemp72) * ((0.25f * (fTemp71 * fVec23[(IOTA-int((int((2 + iTemp69)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp70 * fVec23[(IOTA-int((iTemp69 & 65535)))&65535]))))))));
			fVec24[0] = fTemp76;
			fRec24[0] = (((fVec24[1] + fVec24[0]) / fTemp23) + (fTemp24 * fRec24[1]));
			fRec23[0] = (fRec24[0] - (((fTemp22 * fRec23[2]) + (2 * (fTemp19 * fRec23[1]))) / fTemp21));
			fRec22[0] = (((fRec23[2] + (fRec23[0] + (2 * fRec23[1]))) / fTemp21) - (((fTemp20 * fRec22[2]) + (2 * (fRec22[1] * fTemp19))) / fTemp15));
			float fTemp77 = (fRec22[2] + (fRec22[0] + (2 * fRec22[1])));
			float fTemp78 = (0 - fTemp4);
			float fTemp79 = (fTemp77 / fTemp15);
			fVec25[0] = fTemp79;
			float fTemp80 = (0 - ((1 - fTemp4) / fTemp12));
			fRec20[0] = ((fTemp80 * fRec20[1]) + (((fVec25[1] * fTemp78) + (fTemp77 / fTemp16)) / fTemp12));
			fRec19[0] = (fRec20[0] - (((fTemp11 * fRec19[2]) + (2 * (fTemp8 * fRec19[1]))) / fTemp10));
			float fTemp81 = (0 - fTemp7);
			fRec18[0] = (((((2 * (fRec19[1] * fTemp81)) + (fRec19[0] / fTemp6)) + (fRec19[2] / fTemp6)) / fTemp10) - (((fTemp9 * fRec18[2]) + (2 * (fTemp8 * fRec18[1]))) / fTemp5));
			fRec69[0] = (fSlow42 + (0.99f * fRec69[1]));
			fRec72[0] = (((fVec25[1] + fVec25[0]) / fTemp12) + (fTemp80 * fRec72[1]));
			fRec71[0] = (fRec72[0] - (((fTemp11 * fRec71[2]) + (2 * (fTemp8 * fRec71[1]))) / fTemp10));
			fRec70[0] = (((fRec71[2] + (fRec71[0] + (2 * fRec71[1]))) / fTemp10) - (((fRec70[2] * fTemp9) + (2 * (fRec70[1] * fTemp8))) / fTemp5));
			fRec73[0] = (fSlow43 + (0.99f * fRec73[1]));
			float fTemp82 = (1 + ((1.618033988749895f + fTemp4) / fTemp3));
			float fTemp83 = (1 + ((fTemp4 - 1.618033988749895f) / fTemp3));
			float fTemp84 = (2 * (fTemp8 * fRec74[1]));
			float fTemp85 = (0 - fTemp14);
			fRec77[0] = ((((fVec24[1] * fTemp85) + (fVec24[0] / fTemp13)) / fTemp23) + (fTemp24 * fRec77[1]));
			fRec76[0] = (fRec77[0] - (((fTemp22 * fRec76[2]) + (2 * (fTemp19 * fRec76[1]))) / fTemp21));
			float fTemp86 = (0 - fTemp18);
			fRec75[0] = (((((2 * (fRec76[1] * fTemp86)) + (fRec76[0] / fTemp17)) + (fRec76[2] / fTemp17)) / fTemp21) - (((fTemp20 * fRec75[2]) + (2 * (fTemp19 * fRec75[1]))) / fTemp15));
			fRec74[0] = (((((fRec75[0] / fTemp17) + (2 * (fRec75[1] * fTemp86))) + (fRec75[2] / fTemp17)) / fTemp15) - (((fRec74[2] * fTemp83) + fTemp84) / fTemp82));
			fRec78[0] = (fSlow44 + (0.99f * fRec78[1]));
			fRec79[0] = (fSlow45 + (0.99f * fRec79[1]));
			float fTemp87 = (1.0f - fRec79[0]);
			fRec16[0] = ((fRec79[0] * fRec16[1]) + (fTemp87 * ((fRec78[0] * (fRec74[2] + ((fTemp84 + (fRec74[0] * fTemp83)) / fTemp82))) + (((fRec73[0] * (fRec70[2] + (fRec70[0] + (2 * fRec70[1])))) + (fRec69[0] * (((fRec18[0] / fTemp6) + (2 * (fRec18[1] * fTemp81))) + (fRec18[2] / fTemp6)))) / fTemp5))));
			float fTemp88 = (float)input1[i];
			float fTemp89 = (fTemp88 + (fRec16[0] * fTemp2));
			fVec26[IOTA&65535] = fTemp89;
			int iTemp90 = int((fTemp50 - 0.49998999999999993f));
			float fTemp91 = (fTemp50 - (1.0f + iTemp90));
			float fTemp92 = ((1.0f + fTemp50) - iTemp90);
			float fTemp93 = (fTemp50 - (3.0f + iTemp90));
			float fTemp94 = (fTemp50 - iTemp90);
			float fTemp95 = (fTemp50 - (2.0f + iTemp90));
			float fTemp96 = ((fTemp92 * fTemp95) * fTemp91);
			float fTemp97 = (((0.041666666666666664f * ((fTemp96 * fTemp94) * fVec26[(IOTA-int((int((4 + iTemp90)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp92 * fTemp91) * fTemp94) * fTemp93)) * fVec26[(IOTA-int((int((3 + iTemp90)) & 65535)))&65535]) + ((0 - (fTemp96 * fTemp93)) * fVec26[(IOTA-int((int((1 + iTemp90)) & 65535)))&65535])) + (6.0f * (((fTemp95 * fTemp94) * fTemp93) * ((0.25f * (fTemp92 * fVec26[(IOTA-int((int((2 + iTemp90)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp91 * fVec26[(IOTA-int((iTemp90 & 65535)))&65535])))))))) + (fSlow2 * fRec12[1]));
			float fTemp98 = (fRec27 + fRec25[1]);
			fVec27[IOTA&65535] = fTemp98;
			float fTemp99 = (fRec65[0] * (1 + fRec66[0]));
			int iTemp100 = int((fSlow47 + fTemp99));
			float fTemp101 = ((fTemp99 + fSlow48) - iTemp100);
			float fTemp102 = ((fTemp99 + fSlow49) - iTemp100);
			float fTemp103 = ((fTemp99 + fSlow50) - iTemp100);
			float fTemp104 = ((fTemp99 + fSlow51) - iTemp100);
			float fTemp105 = ((fTemp99 + fSlow52) - iTemp100);
			float fTemp106 = ((fTemp102 * fTemp105) * fTemp101);
			float fTemp107 = ((0.041666666666666664f * ((fTemp106 * fTemp104) * fVec27[(IOTA-int((int((4 + iTemp100)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp102 * fTemp101) * fTemp104) * fTemp103)) * fVec27[(IOTA-int((int((3 + iTemp100)) & 65535)))&65535]) + ((0 - (fTemp106 * fTemp103)) * fVec27[(IOTA-int((int((1 + iTemp100)) & 65535)))&65535])) + (6.0f * (((fTemp105 * fTemp104) * fTemp103) * ((0.25f * (fTemp102 * fVec27[(IOTA-int((int((2 + iTemp100)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp101 * fVec27[(IOTA-int((iTemp100 & 65535)))&65535]))))))));
			fVec28[0] = fTemp107;
			fRec86[0] = ((fTemp24 * fRec86[1]) + ((fVec28[1] + fVec28[0]) / fTemp23));
			fRec85[0] = (fRec86[0] - (((fTemp22 * fRec85[2]) + (2 * (fTemp19 * fRec85[1]))) / fTemp21));
			fRec84[0] = (((fRec85[2] + (fRec85[0] + (2 * fRec85[1]))) / fTemp21) - (((fTemp20 * fRec84[2]) + (2 * (fTemp19 * fRec84[1]))) / fTemp15));
			float fTemp108 = (fRec84[2] + (fRec84[0] + (2 * fRec84[1])));
			float fTemp109 = (fTemp108 / fTemp15);
			fVec29[0] = fTemp109;
			fRec83[0] = ((fTemp80 * fRec83[1]) + (((fTemp78 * fVec29[1]) + (fTemp108 / fTemp16)) / fTemp12));
			fRec82[0] = (fRec83[0] - (((fTemp11 * fRec82[2]) + (2 * (fTemp8 * fRec82[1]))) / fTemp10));
			fRec81[0] = (((((2 * (fTemp81 * fRec82[1])) + (fRec82[0] / fTemp6)) + (fRec82[2] / fTemp6)) / fTemp10) - (((fTemp9 * fRec81[2]) + (2 * (fTemp8 * fRec81[1]))) / fTemp5));
			fRec89[0] = ((fTemp80 * fRec89[1]) + ((fVec29[1] + fVec29[0]) / fTemp12));
			fRec88[0] = (fRec89[0] - (((fTemp11 * fRec88[2]) + (2 * (fTemp8 * fRec88[1]))) / fTemp10));
			fRec87[0] = (((fRec88[2] + (fRec88[0] + (2 * fRec88[1]))) / fTemp10) - (((fTemp9 * fRec87[2]) + (2 * (fTemp8 * fRec87[1]))) / fTemp5));
			float fTemp110 = (2 * (fTemp8 * fRec90[1]));
			fRec93[0] = ((((fTemp85 * fVec28[1]) + (fVec28[0] / fTemp13)) / fTemp23) + (fTemp24 * fRec93[1]));
			fRec92[0] = (fRec93[0] - (((fTemp22 * fRec92[2]) + (2 * (fTemp19 * fRec92[1]))) / fTemp21));
			fRec91[0] = (((((2 * (fTemp86 * fRec92[1])) + (fRec92[0] / fTemp17)) + (fRec92[2] / fTemp17)) / fTemp21) - (((fTemp20 * fRec91[2]) + (2 * (fTemp19 * fRec91[1]))) / fTemp15));
			fRec90[0] = (((((2 * (fTemp86 * fRec91[1])) + (fRec91[0] / fTemp17)) + (fRec91[2] / fTemp17)) / fTemp15) - (((fTemp83 * fRec90[2]) + fTemp110) / fTemp82));
			fRec80[0] = ((fTemp87 * ((fRec78[0] * (fRec90[2] + ((fTemp110 + (fRec90[0] * fTemp83)) / fTemp82))) + (((fRec73[0] * (fRec87[2] + (fRec87[0] + (2 * fRec87[1])))) + (fRec69[0] * (((2 * (fTemp81 * fRec81[1])) + (fRec81[0] / fTemp6)) + (fRec81[2] / fTemp6)))) / fTemp5))) + (fRec79[0] * fRec80[1]));
			float fTemp111 = (fTemp1 + (fRec80[0] * fTemp2));
			fVec30[IOTA&65535] = fTemp111;
			int iTemp112 = int((fTemp99 - 0.49998999999999993f));
			float fTemp113 = (fTemp99 - (1.0f + iTemp112));
			float fTemp114 = ((1.0f + fTemp99) - iTemp112);
			float fTemp115 = (fTemp99 - (3.0f + iTemp112));
			float fTemp116 = (fTemp99 - iTemp112);
			float fTemp117 = (fTemp99 - (2.0f + iTemp112));
			float fTemp118 = ((fTemp114 * fTemp117) * fTemp113);
			float fTemp119 = (((0.041666666666666664f * ((fTemp118 * fTemp116) * fVec30[(IOTA-int((int((4 + iTemp112)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp114 * fTemp113) * fTemp116) * fTemp115)) * fVec30[(IOTA-int((int((3 + iTemp112)) & 65535)))&65535]) + ((0 - (fTemp118 * fTemp115)) * fVec30[(IOTA-int((int((1 + iTemp112)) & 65535)))&65535])) + (6.0f * (((fTemp117 * fTemp116) * fTemp115) * ((0.25f * (fTemp114 * fVec30[(IOTA-int((int((2 + iTemp112)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp113 * fVec30[(IOTA-int((iTemp112 & 65535)))&65535])))))))) + (fSlow2 * fRec11[1]));
			fVec31[IOTA&65535] = ((0.7071067811865476f * fTemp119) - (0.7071067811865475f * fTemp97));
			fRec11[0] = fVec31[(IOTA-iSlow53)&65535];
			fVec32[IOTA&65535] = ((0.7071067811865476f * fTemp97) + (0.7071067811865475f * fTemp119));
			fRec12[0] = fVec32[(IOTA-iSlow54)&65535];
			float 	fRec13 = (fSlow1 * fTemp119);
			float 	fRec14 = (fSlow1 * fTemp97);
			float fTemp120 = ((fRec14 + fRec12[1]) + (fSlow2 * fRec8[1]));
			float fTemp121 = ((fRec13 + fRec11[1]) + (fSlow2 * fRec7[1]));
			fVec33[IOTA&65535] = ((0.7071067811865476f * fTemp121) - (0.7071067811865475f * fTemp120));
			fRec7[0] = fVec33[(IOTA-iSlow55)&65535];
			fVec34[IOTA&65535] = ((0.7071067811865476f * fTemp120) + (0.7071067811865475f * fTemp121));
			fRec8[0] = fVec34[(IOTA-iSlow56)&65535];
			float 	fRec9 = (fSlow1 * fTemp121);
			float 	fRec10 = (fSlow1 * fTemp120);
			float fTemp122 = ((fRec10 + fRec8[1]) + (fSlow2 * fRec4[1]));
			float fTemp123 = ((fRec9 + fRec7[1]) + (fSlow2 * fRec3[1]));
			fVec35[IOTA&65535] = ((0.7071067811865476f * fTemp123) - (0.7071067811865475f * fTemp122));
			fRec3[0] = fVec35[(IOTA-iSlow11)&65535];
			fVec36[IOTA&65535] = ((0.7071067811865476f * fTemp122) + (0.7071067811865475f * fTemp123));
			fRec4[0] = fVec36[(IOTA-iSlow57)&65535];
			float 	fRec5 = (fSlow1 * fTemp123);
			float 	fRec6 = (fSlow1 * fTemp122);
			fRec1[0] = (fRec5 + fRec3[1]);
			fRec2[0] = (fRec6 + fRec4[1]);
			output0[i] = (FAUSTFLOAT)((fRec1[0] * fRec0[0]) + (fTemp1 * fTemp0));
			output1[i] = (FAUSTFLOAT)((fTemp88 * fTemp0) + (fRec0[0] * fRec2[0]));
			// post processing
			fRec2[1] = fRec2[0];
			fRec1[1] = fRec1[0];
			fRec4[1] = fRec4[0];
			fRec3[1] = fRec3[0];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
			fRec12[1] = fRec12[0];
			fRec11[1] = fRec11[0];
			fRec80[1] = fRec80[0];
			fRec90[2] = fRec90[1]; fRec90[1] = fRec90[0];
			fRec91[2] = fRec91[1]; fRec91[1] = fRec91[0];
			fRec92[2] = fRec92[1]; fRec92[1] = fRec92[0];
			fRec93[1] = fRec93[0];
			fRec87[2] = fRec87[1]; fRec87[1] = fRec87[0];
			fRec88[2] = fRec88[1]; fRec88[1] = fRec88[0];
			fRec89[1] = fRec89[0];
			fRec81[2] = fRec81[1]; fRec81[1] = fRec81[0];
			fRec82[2] = fRec82[1]; fRec82[1] = fRec82[0];
			fRec83[1] = fRec83[0];
			fVec29[1] = fVec29[0];
			fRec84[2] = fRec84[1]; fRec84[1] = fRec84[0];
			fRec85[2] = fRec85[1]; fRec85[1] = fRec85[0];
			fRec86[1] = fRec86[0];
			fVec28[1] = fVec28[0];
			fRec16[1] = fRec16[0];
			fRec79[1] = fRec79[0];
			fRec78[1] = fRec78[0];
			fRec74[2] = fRec74[1]; fRec74[1] = fRec74[0];
			fRec75[2] = fRec75[1]; fRec75[1] = fRec75[0];
			fRec76[2] = fRec76[1]; fRec76[1] = fRec76[0];
			fRec77[1] = fRec77[0];
			fRec73[1] = fRec73[0];
			fRec70[2] = fRec70[1]; fRec70[1] = fRec70[0];
			fRec71[2] = fRec71[1]; fRec71[1] = fRec71[0];
			fRec72[1] = fRec72[0];
			fRec69[1] = fRec69[0];
			fRec18[2] = fRec18[1]; fRec18[1] = fRec18[0];
			fRec19[2] = fRec19[1]; fRec19[1] = fRec19[0];
			fRec20[1] = fRec20[0];
			fVec25[1] = fVec25[0];
			fRec22[2] = fRec22[1]; fRec22[1] = fRec22[0];
			fRec23[2] = fRec23[1]; fRec23[1] = fRec23[0];
			fRec24[1] = fRec24[0];
			fVec24[1] = fVec24[0];
			fRec26[1] = fRec26[0];
			fRec25[1] = fRec25[0];
			fRec30[1] = fRec30[0];
			fRec29[1] = fRec29[0];
			fRec34[1] = fRec34[0];
			fRec33[1] = fRec33[0];
			fRec38[1] = fRec38[0];
			fRec37[1] = fRec37[0];
			fRec42[1] = fRec42[0];
			fRec41[1] = fRec41[0];
			fRec67[1] = fRec67[0];
			fRec66[1] = fRec66[0];
			fRec68[1] = fRec68[0];
			fRec65[1] = fRec65[0];
			fRec46[1] = fRec46[0];
			fRec45[1] = fRec45[0];
			fRec50[1] = fRec50[0];
			fRec49[1] = fRec49[0];
			fRec54[1] = fRec54[0];
			fRec53[1] = fRec53[0];
			fRec58[1] = fRec58[0];
			fRec57[1] = fRec57[0];
			fRec62[1] = fRec62[0];
			fRec61[1] = fRec61[0];
			IOTA = IOTA+1;
			fRec21[1] = fRec21[0];
			fRec17[1] = fRec17[0];
			fRec15[1] = fRec15[0];
			fRec0[1] = fRec0[0];
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
//FAUST_EXPORT int api_version(void) { return sc_api_version; }
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
