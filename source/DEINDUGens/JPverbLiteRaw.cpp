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
	FAUSTFLOAT 	fslider0;
	int 	iVec0[2];
	FAUSTFLOAT 	fslider1;
	FAUSTFLOAT 	fslider2;
	int 	IOTA;
	float 	fVec1[16384];
	int 	iConst0;
	float 	fConst1;
	float 	fRec47[2];
	float 	fRec46[2];
	float 	fRec44[2];
	float 	fVec2[16384];
	int 	iConst2;
	float 	fConst3;
	float 	fRec49[2];
	float 	fRec48[2];
	float 	fRec45[2];
	float 	fVec3[16384];
	int 	iConst4;
	float 	fConst5;
	float 	fRec50[2];
	float 	fRec43[2];
	float 	fRec41[2];
	float 	fVec4[16384];
	int 	iConst6;
	float 	fConst7;
	float 	fRec52[2];
	float 	fRec51[2];
	float 	fRec42[2];
	float 	fVec5[16384];
	int 	iConst8;
	float 	fConst9;
	float 	fRec53[2];
	float 	fRec40[2];
	float 	fRec38[2];
	float 	fVec6[16384];
	int 	iConst10;
	float 	fConst11;
	float 	fRec55[2];
	float 	fRec54[2];
	float 	fRec39[2];
	float 	fVec7[16384];
	int 	iConst12;
	float 	fConst13;
	float 	fRec56[2];
	float 	fRec37[2];
	float 	fRec35[2];
	float 	fVec8[16384];
	int 	iConst14;
	float 	fConst15;
	float 	fRec58[2];
	float 	fRec57[2];
	float 	fRec36[2];
	float 	fVec9[16384];
	int 	iConst16;
	float 	fConst17;
	float 	fRec59[2];
	float 	fRec34[2];
	float 	fRec32[2];
	float 	fVec10[16384];
	int 	iConst18;
	float 	fConst19;
	float 	fRec61[2];
	float 	fRec60[2];
	float 	fRec33[2];
	float 	fVec11[16384];
	int 	iConst20;
	float 	fConst21;
	float 	fRec62[2];
	float 	fRec31[2];
	float 	fVec12[16384];
	int 	iConst22;
	float 	fConst23;
	float 	fRec64[2];
	float 	fRec63[2];
	float 	fVec13[16384];
	int 	iConst24;
	float 	fConst25;
	float 	fRec65[2];
	float 	fRec30[2];
	float 	fRec28[2];
	float 	fVec14[16384];
	int 	iConst26;
	float 	fConst27;
	float 	fRec67[2];
	float 	fRec66[2];
	float 	fRec29[2];
	float 	fVec15[16384];
	int 	iConst28;
	float 	fConst29;
	float 	fRec68[2];
	float 	fRec27[2];
	float 	fRec25[2];
	float 	fVec16[16384];
	int 	iConst30;
	float 	fConst31;
	float 	fRec70[2];
	float 	fRec69[2];
	float 	fRec26[2];
	float 	fVec17[16384];
	int 	iConst32;
	float 	fConst33;
	float 	fRec71[2];
	float 	fRec24[2];
	float 	fRec22[2];
	float 	fVec18[16384];
	int 	iConst34;
	float 	fConst35;
	float 	fRec73[2];
	float 	fRec72[2];
	float 	fRec23[2];
	float 	fVec19[16384];
	int 	iConst36;
	float 	fConst37;
	float 	fRec74[2];
	float 	fRec21[2];
	float 	fRec19[2];
	float 	fVec20[16384];
	int 	iConst38;
	float 	fConst39;
	float 	fRec76[2];
	float 	fRec75[2];
	float 	fRec20[2];
	float 	fVec21[16384];
	int 	iConst40;
	float 	fConst41;
	float 	fRec77[2];
	float 	fRec18[2];
	float 	fRec16[2];
	float 	fVec22[16384];
	int 	iConst42;
	float 	fConst43;
	float 	fRec79[2];
	float 	fRec78[2];
	float 	fRec17[2];
	float 	fVec23[16384];
	int 	iConst44;
	float 	fConst45;
	float 	fRec80[2];
	float 	fRec15[2];
	float 	fRec14[2];
	float 	fVec24[16384];
	int 	iConst46;
	float 	fConst47;
	float 	fRec83[2];
	float 	fRec82[2];
	float 	fRec81[2];
	float 	fVec25[16384];
	float 	fRec13[2];
	float 	fRec11[2];
	float 	fVec26[16384];
	int 	iConst48;
	float 	fConst49;
	float 	fRec85[2];
	float 	fRec84[2];
	float 	fRec12[2];
	float 	fVec27[16384];
	float 	fRec10[2];
	float 	fRec8[2];
	float 	fVec28[16384];
	float 	fRec86[2];
	float 	fRec9[2];
	float 	fVec29[16384];
	float 	fRec7[2];
	float 	fRec5[2];
	float 	fVec30[16384];
	int 	iConst50;
	float 	fConst51;
	float 	fRec88[2];
	float 	fRec87[2];
	float 	fRec6[2];
	float 	fVec31[16384];
	int 	iConst52;
	float 	fConst53;
	float 	fRec89[2];
	float 	fRec4[2];
	float 	fRec2[2];
	float 	fVec32[16384];
	float 	fRec90[2];
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
		IOTA = 0;
		for (int i=0; i<16384; i++) fVec1[i] = 0;
		iConst0 = primes(10);
		fConst1 = (9.999999999998899e-05f * iConst0);
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<16384; i++) fVec2[i] = 0;
		iConst2 = primes(110);
		fConst3 = (9.999999999998899e-05f * iConst2);
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<16384; i++) fVec3[i] = 0;
		iConst4 = primes(40);
		fConst5 = (9.999999999998899e-05f * iConst4);
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<16384; i++) fVec4[i] = 0;
		iConst6 = primes(140);
		fConst7 = (9.999999999998899e-05f * iConst6);
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<16384; i++) fVec5[i] = 0;
		iConst8 = primes(70);
		fConst9 = (9.999999999998899e-05f * iConst8);
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<16384; i++) fVec6[i] = 0;
		iConst10 = primes(170);
		fConst11 = (9.999999999998899e-05f * iConst10);
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<16384; i++) fVec7[i] = 0;
		iConst12 = primes(100);
		fConst13 = (9.999999999998899e-05f * iConst12);
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<16384; i++) fVec8[i] = 0;
		iConst14 = primes(200);
		fConst15 = (9.999999999998899e-05f * iConst14);
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<16384; i++) fVec9[i] = 0;
		iConst16 = primes(130);
		fConst17 = (9.999999999998899e-05f * iConst16);
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<16384; i++) fVec10[i] = 0;
		iConst18 = primes(230);
		fConst19 = (9.999999999998899e-05f * iConst18);
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<16384; i++) fVec11[i] = 0;
		iConst20 = primes(54);
		fConst21 = (0.0050000000000000044f * iConst20);
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<16384; i++) fVec12[i] = 0;
		iConst22 = primes(204);
		fConst23 = (0.0050000000000000044f * iConst22);
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<16384; i++) fVec13[i] = 0;
		iConst24 = primes(125);
		fConst25 = (9.999999999998899e-05f * iConst24);
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<16384; i++) fVec14[i] = 0;
		iConst26 = primes(25);
		fConst27 = (9.999999999998899e-05f * iConst26);
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<16384; i++) fVec15[i] = 0;
		iConst28 = primes(155);
		fConst29 = (9.999999999998899e-05f * iConst28);
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<16384; i++) fVec16[i] = 0;
		iConst30 = primes(55);
		fConst31 = (9.999999999998899e-05f * iConst30);
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<16384; i++) fVec17[i] = 0;
		iConst32 = primes(185);
		fConst33 = (9.999999999998899e-05f * iConst32);
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<16384; i++) fVec18[i] = 0;
		iConst34 = primes(85);
		fConst35 = (9.999999999998899e-05f * iConst34);
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<16384; i++) fVec19[i] = 0;
		iConst36 = primes(215);
		fConst37 = (9.999999999998899e-05f * iConst36);
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<16384; i++) fVec20[i] = 0;
		iConst38 = primes(115);
		fConst39 = (9.999999999998899e-05f * iConst38);
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<16384; i++) fVec21[i] = 0;
		iConst40 = primes(245);
		fConst41 = (9.999999999998899e-05f * iConst40);
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<16384; i++) fVec22[i] = 0;
		iConst42 = primes(145);
		fConst43 = (9.999999999998899e-05f * iConst42);
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<16384; i++) fVec23[i] = 0;
		iConst44 = primes(134);
		fConst45 = (0.0050000000000000044f * iConst44);
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<16384; i++) fVec24[i] = 0;
		iConst46 = primes(34);
		fConst47 = (0.0050000000000000044f * iConst46);
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<16384; i++) fVec25[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<16384; i++) fVec26[i] = 0;
		iConst48 = primes(240);
		fConst49 = (9.999999999998899e-05f * iConst48);
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<16384; i++) fVec27[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<16384; i++) fVec28[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<16384; i++) fVec29[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<16384; i++) fVec30[i] = 0;
		iConst50 = primes(190);
		fConst51 = (9.999999999998899e-05f * iConst50);
		for (int i=0; i<2; i++) fRec88[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<16384; i++) fVec31[i] = 0;
		iConst52 = primes(175);
		fConst53 = (9.999999999998899e-05f * iConst52);
		for (int i=0; i<2; i++) fRec89[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<16384; i++) fVec32[i] = 0;
		for (int i=0; i<2; i++) fRec90[i] = 0;
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
		interface->addHorizontalSlider("t60", &fslider2, 1.0f, 0.1f, 6e+01f, 0.1f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = fslider0;
		float 	fSlow1 = cosf(fSlow0);
		float 	fSlow2 = sinf(fSlow0);
		float 	fSlow3 = (0 - fSlow2);
		float 	fSlow4 = fslider1;
		float 	fSlow5 = (1.0f - fSlow4);
		float 	fSlow6 = powf(10,(0 - (0.51f / fslider2)));
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			iVec0[0] = 1;
			float fTemp0 = ((0.7603139617444397f * fRec0[1]) - (0.6495557555564224f * fRec44[1]));
			float fTemp1 = ((0.7603139617444397f * fRec1[1]) - (0.6495557555564224f * fRec45[1]));
			float fTemp2 = ((0.7071067811865476f * fTemp0) - (0.7071067811865475f * fTemp1));
			fVec1[IOTA&16383] = fTemp2;
			int iTemp3 = (1 - iVec0[1]);
			fRec47[0] = (fConst1 + (0.9999f * ((iConst0 * iTemp3) + fRec47[1])));
			int iTemp4 = int((fRec47[0] - 1.49999f));
			int iTemp5 = int((iTemp4 & 8191));
			float fTemp6 = ((2.0f + iTemp4) - fRec47[0]);
			float fTemp7 = (fRec47[0] - iTemp4);
			fRec46[0] = (fVec1[(IOTA-int((1 + iTemp5)))&16383] + ((fRec46[1] * (0 - (fTemp6 / fTemp7))) + ((fTemp6 * fVec1[(IOTA-iTemp5)&16383]) / fTemp7)));
			fRec44[0] = fRec46[0];
			float fTemp8 = ((0.7071067811865475f * fTemp0) + (0.7071067811865476f * fTemp1));
			fVec2[IOTA&16383] = fTemp8;
			fRec49[0] = (fConst3 + (0.9999f * ((iConst2 * iTemp3) + fRec49[1])));
			int iTemp9 = int((fRec49[0] - 1.49999f));
			int iTemp10 = int((iTemp9 & 8191));
			float fTemp11 = ((2.0f + iTemp9) - fRec49[0]);
			float fTemp12 = (fRec49[0] - iTemp9);
			fRec48[0] = (fVec2[(IOTA-int((1 + iTemp10)))&16383] + ((fRec48[1] * (0 - (fTemp11 / fTemp12))) + ((fTemp11 * fVec2[(IOTA-iTemp10)&16383]) / fTemp12)));
			fRec45[0] = fRec48[0];
			float fTemp13 = ((0.7603139617444397f * fRec44[1]) + (0.6495557555564224f * fRec0[1]));
			float fTemp14 = ((0.7603139617444397f * fTemp13) - (0.6495557555564224f * fRec41[1]));
			float fTemp15 = ((0.7603139617444397f * fRec45[1]) + (0.6495557555564224f * fRec1[1]));
			float fTemp16 = ((0.7603139617444397f * fTemp15) - (0.6495557555564224f * fRec42[1]));
			float fTemp17 = ((0.7071067811865476f * fTemp14) - (0.7071067811865475f * fTemp16));
			fVec3[IOTA&16383] = fTemp17;
			fRec50[0] = (fConst5 + (0.9999f * ((iConst4 * iTemp3) + fRec50[1])));
			int iTemp18 = int((fRec50[0] - 1.49999f));
			int iTemp19 = int((iTemp18 & 8191));
			float fTemp20 = ((2.0f + iTemp18) - fRec50[0]);
			float fTemp21 = (fRec50[0] - iTemp18);
			fRec43[0] = (fVec3[(IOTA-int((1 + iTemp19)))&16383] + ((fRec43[1] * (0 - (fTemp20 / fTemp21))) + ((fTemp20 * fVec3[(IOTA-iTemp19)&16383]) / fTemp21)));
			fRec41[0] = fRec43[0];
			float fTemp22 = ((0.7071067811865475f * fTemp14) + (0.7071067811865476f * fTemp16));
			fVec4[IOTA&16383] = fTemp22;
			fRec52[0] = (fConst7 + (0.9999f * ((iConst6 * iTemp3) + fRec52[1])));
			int iTemp23 = int((fRec52[0] - 1.49999f));
			int iTemp24 = int((iTemp23 & 8191));
			float fTemp25 = ((2.0f + iTemp23) - fRec52[0]);
			float fTemp26 = (fRec52[0] - iTemp23);
			fRec51[0] = (fVec4[(IOTA-int((1 + iTemp24)))&16383] + ((fRec51[1] * (0 - (fTemp25 / fTemp26))) + ((fTemp25 * fVec4[(IOTA-iTemp24)&16383]) / fTemp26)));
			fRec42[0] = fRec51[0];
			float fTemp27 = ((0.7603139617444397f * fRec41[1]) + (0.6495557555564224f * fTemp13));
			float fTemp28 = ((0.7603139617444397f * fTemp27) - (0.6495557555564224f * fRec38[1]));
			float fTemp29 = ((0.7603139617444397f * fRec42[1]) + (0.6495557555564224f * fTemp15));
			float fTemp30 = ((0.7603139617444397f * fTemp29) - (0.6495557555564224f * fRec39[1]));
			float fTemp31 = ((0.7071067811865476f * fTemp28) - (0.7071067811865475f * fTemp30));
			fVec5[IOTA&16383] = fTemp31;
			fRec53[0] = (fConst9 + (0.9999f * ((iConst8 * iTemp3) + fRec53[1])));
			int iTemp32 = int((fRec53[0] - 1.49999f));
			int iTemp33 = int((iTemp32 & 8191));
			float fTemp34 = ((2.0f + iTemp32) - fRec53[0]);
			float fTemp35 = (fRec53[0] - iTemp32);
			fRec40[0] = (fVec5[(IOTA-int((1 + iTemp33)))&16383] + ((fRec40[1] * (0 - (fTemp34 / fTemp35))) + ((fTemp34 * fVec5[(IOTA-iTemp33)&16383]) / fTemp35)));
			fRec38[0] = fRec40[0];
			float fTemp36 = ((0.7071067811865475f * fTemp28) + (0.7071067811865476f * fTemp30));
			fVec6[IOTA&16383] = fTemp36;
			fRec55[0] = (fConst11 + (0.9999f * ((iConst10 * iTemp3) + fRec55[1])));
			int iTemp37 = int((fRec55[0] - 1.49999f));
			int iTemp38 = int((iTemp37 & 8191));
			float fTemp39 = ((2.0f + iTemp37) - fRec55[0]);
			float fTemp40 = (fRec55[0] - iTemp37);
			fRec54[0] = (fVec6[(IOTA-int((1 + iTemp38)))&16383] + ((fRec54[1] * (0 - (fTemp39 / fTemp40))) + ((fTemp39 * fVec6[(IOTA-iTemp38)&16383]) / fTemp40)));
			fRec39[0] = fRec54[0];
			float fTemp41 = ((0.7603139617444397f * fRec38[1]) + (0.6495557555564224f * fTemp27));
			float fTemp42 = ((0.7603139617444397f * fTemp41) - (0.6495557555564224f * fRec35[1]));
			float fTemp43 = ((0.7603139617444397f * fRec39[1]) + (0.6495557555564224f * fTemp29));
			float fTemp44 = ((0.7603139617444397f * fTemp43) - (0.6495557555564224f * fRec36[1]));
			float fTemp45 = ((0.7071067811865476f * fTemp42) - (0.7071067811865475f * fTemp44));
			fVec7[IOTA&16383] = fTemp45;
			fRec56[0] = (fConst13 + (0.9999f * ((iConst12 * iTemp3) + fRec56[1])));
			int iTemp46 = int((fRec56[0] - 1.49999f));
			int iTemp47 = int((iTemp46 & 8191));
			float fTemp48 = ((2.0f + iTemp46) - fRec56[0]);
			float fTemp49 = (fRec56[0] - iTemp46);
			fRec37[0] = (fVec7[(IOTA-int((1 + iTemp47)))&16383] + ((fRec37[1] * (0 - (fTemp48 / fTemp49))) + ((fTemp48 * fVec7[(IOTA-iTemp47)&16383]) / fTemp49)));
			fRec35[0] = fRec37[0];
			float fTemp50 = ((0.7071067811865475f * fTemp42) + (0.7071067811865476f * fTemp44));
			fVec8[IOTA&16383] = fTemp50;
			fRec58[0] = (fConst15 + (0.9999f * ((iConst14 * iTemp3) + fRec58[1])));
			int iTemp51 = int((fRec58[0] - 1.49999f));
			int iTemp52 = int((iTemp51 & 8191));
			float fTemp53 = ((2.0f + iTemp51) - fRec58[0]);
			float fTemp54 = (fRec58[0] - iTemp51);
			fRec57[0] = (fVec8[(IOTA-int((1 + iTemp52)))&16383] + ((fRec57[1] * (0 - (fTemp53 / fTemp54))) + ((fTemp53 * fVec8[(IOTA-iTemp52)&16383]) / fTemp54)));
			fRec36[0] = fRec57[0];
			float fTemp55 = ((0.7603139617444397f * fRec35[1]) + (0.6495557555564224f * fTemp41));
			float fTemp56 = ((0.7603139617444397f * fTemp55) - (0.6495557555564224f * fRec32[1]));
			float fTemp57 = ((0.7603139617444397f * fRec36[1]) + (0.6495557555564224f * fTemp43));
			float fTemp58 = ((0.7603139617444397f * fTemp57) - (0.6495557555564224f * fRec33[1]));
			float fTemp59 = ((0.7071067811865476f * fTemp56) - (0.7071067811865475f * fTemp58));
			fVec9[IOTA&16383] = fTemp59;
			fRec59[0] = (fConst17 + (0.9999f * ((iConst16 * iTemp3) + fRec59[1])));
			int iTemp60 = int((fRec59[0] - 1.49999f));
			int iTemp61 = int((iTemp60 & 8191));
			float fTemp62 = ((2.0f + iTemp60) - fRec59[0]);
			float fTemp63 = (fRec59[0] - iTemp60);
			fRec34[0] = (fVec9[(IOTA-int((1 + iTemp61)))&16383] + ((fRec34[1] * (0 - (fTemp62 / fTemp63))) + ((fTemp62 * fVec9[(IOTA-iTemp61)&16383]) / fTemp63)));
			fRec32[0] = fRec34[0];
			float fTemp64 = ((0.7071067811865475f * fTemp56) + (0.7071067811865476f * fTemp58));
			fVec10[IOTA&16383] = fTemp64;
			fRec61[0] = (fConst19 + (0.9999f * ((iConst18 * iTemp3) + fRec61[1])));
			int iTemp65 = int((fRec61[0] - 1.49999f));
			int iTemp66 = int((iTemp65 & 8191));
			float fTemp67 = ((2.0f + iTemp65) - fRec61[0]);
			float fTemp68 = (fRec61[0] - iTemp65);
			fRec60[0] = (fVec10[(IOTA-int((1 + iTemp66)))&16383] + ((fRec60[1] * (0 - (fTemp67 / fTemp68))) + ((fTemp67 * fVec10[(IOTA-iTemp66)&16383]) / fTemp68)));
			fRec33[0] = fRec60[0];
			float fTemp69 = ((0.7603139617444397f * fRec32[1]) + (0.6495557555564224f * fTemp55));
			fVec11[IOTA&16383] = fTemp69;
			fRec62[0] = (fConst21 + (0.995f * ((iConst20 * iTemp3) + fRec62[1])));
			int iTemp70 = int((fRec62[0] - 1.49999f));
			int iTemp71 = int((iTemp70 & 8191));
			float fTemp72 = ((2.0f + iTemp70) - fRec62[0]);
			float fTemp73 = (fRec62[0] - iTemp70);
			fRec31[0] = (fVec11[(IOTA-int((1 + iTemp71)))&16383] + ((fRec31[1] * (0 - (fTemp72 / fTemp73))) + ((fTemp72 * fVec11[(IOTA-iTemp71)&16383]) / fTemp73)));
			float fTemp74 = ((0.7603139617444397f * fRec31[0]) - (0.6495557555564224f * fRec28[1]));
			float fTemp75 = ((0.7603139617444397f * fRec33[1]) + (0.6495557555564224f * fTemp57));
			fVec12[IOTA&16383] = fTemp75;
			fRec64[0] = (fConst23 + (0.995f * ((iConst22 * iTemp3) + fRec64[1])));
			int iTemp76 = int((fRec64[0] - 1.49999f));
			int iTemp77 = int((iTemp76 & 8191));
			float fTemp78 = ((2.0f + iTemp76) - fRec64[0]);
			float fTemp79 = (fRec64[0] - iTemp76);
			fRec63[0] = (fVec12[(IOTA-int((1 + iTemp77)))&16383] + ((fRec63[1] * (0 - (fTemp78 / fTemp79))) + ((fTemp78 * fVec12[(IOTA-iTemp77)&16383]) / fTemp79)));
			float fTemp80 = ((0.7603139617444397f * fRec63[0]) - (0.6495557555564224f * fRec29[1]));
			float fTemp81 = ((0.7071067811865476f * fTemp74) - (0.7071067811865475f * fTemp80));
			fVec13[IOTA&16383] = fTemp81;
			fRec65[0] = (fConst25 + (0.9999f * ((iConst24 * iTemp3) + fRec65[1])));
			int iTemp82 = int((fRec65[0] - 1.49999f));
			int iTemp83 = int((iTemp82 & 8191));
			float fTemp84 = ((2.0f + iTemp82) - fRec65[0]);
			float fTemp85 = (fRec65[0] - iTemp82);
			fRec30[0] = (fVec13[(IOTA-int((1 + iTemp83)))&16383] + ((fRec30[1] * (0 - (fTemp84 / fTemp85))) + ((fTemp84 * fVec13[(IOTA-iTemp83)&16383]) / fTemp85)));
			fRec28[0] = fRec30[0];
			float fTemp86 = ((0.7071067811865475f * fTemp74) + (0.7071067811865476f * fTemp80));
			fVec14[IOTA&16383] = fTemp86;
			fRec67[0] = (fConst27 + (0.9999f * ((iConst26 * iTemp3) + fRec67[1])));
			int iTemp87 = int((fRec67[0] - 1.49999f));
			int iTemp88 = int((iTemp87 & 8191));
			float fTemp89 = ((2.0f + iTemp87) - fRec67[0]);
			float fTemp90 = (fRec67[0] - iTemp87);
			fRec66[0] = (fVec14[(IOTA-int((1 + iTemp88)))&16383] + ((fRec66[1] * (0 - (fTemp89 / fTemp90))) + ((fTemp89 * fVec14[(IOTA-iTemp88)&16383]) / fTemp90)));
			fRec29[0] = fRec66[0];
			float fTemp91 = ((0.7603139617444397f * fRec28[1]) + (0.6495557555564224f * fRec31[0]));
			float fTemp92 = ((0.7603139617444397f * fTemp91) - (0.6495557555564224f * fRec25[1]));
			float fTemp93 = ((0.7603139617444397f * fRec29[1]) + (0.6495557555564224f * fRec63[0]));
			float fTemp94 = ((0.7603139617444397f * fTemp93) - (0.6495557555564224f * fRec26[1]));
			float fTemp95 = ((0.7071067811865476f * fTemp92) - (0.7071067811865475f * fTemp94));
			fVec15[IOTA&16383] = fTemp95;
			fRec68[0] = (fConst29 + (0.9999f * ((iConst28 * iTemp3) + fRec68[1])));
			int iTemp96 = int((fRec68[0] - 1.49999f));
			int iTemp97 = int((iTemp96 & 8191));
			float fTemp98 = ((2.0f + iTemp96) - fRec68[0]);
			float fTemp99 = (fRec68[0] - iTemp96);
			fRec27[0] = (fVec15[(IOTA-int((1 + iTemp97)))&16383] + ((fRec27[1] * (0 - (fTemp98 / fTemp99))) + ((fTemp98 * fVec15[(IOTA-iTemp97)&16383]) / fTemp99)));
			fRec25[0] = fRec27[0];
			float fTemp100 = ((0.7071067811865475f * fTemp92) + (0.7071067811865476f * fTemp94));
			fVec16[IOTA&16383] = fTemp100;
			fRec70[0] = (fConst31 + (0.9999f * ((iConst30 * iTemp3) + fRec70[1])));
			int iTemp101 = int((fRec70[0] - 1.49999f));
			int iTemp102 = int((iTemp101 & 8191));
			int iTemp103 = int((1 + iTemp102));
			float fTemp104 = ((2.0f + iTemp101) - fRec70[0]);
			float fTemp105 = (fRec70[0] - iTemp101);
			float fTemp106 = (0 - (fTemp104 / fTemp105));
			fRec69[0] = (fVec16[(IOTA-iTemp103)&16383] + ((fTemp106 * fRec69[1]) + ((fTemp104 * fVec16[(IOTA-iTemp102)&16383]) / fTemp105)));
			fRec26[0] = fRec69[0];
			float fTemp107 = ((0.7603139617444397f * fRec25[1]) + (0.6495557555564224f * fTemp91));
			float fTemp108 = ((0.7603139617444397f * fTemp107) - (0.6495557555564224f * fRec22[1]));
			float fTemp109 = ((0.7603139617444397f * fRec26[1]) + (0.6495557555564224f * fTemp93));
			float fTemp110 = ((0.7603139617444397f * fTemp109) - (0.6495557555564224f * fRec23[1]));
			float fTemp111 = ((0.7071067811865476f * fTemp108) - (0.7071067811865475f * fTemp110));
			fVec17[IOTA&16383] = fTemp111;
			fRec71[0] = (fConst33 + (0.9999f * ((iConst32 * iTemp3) + fRec71[1])));
			int iTemp112 = int((fRec71[0] - 1.49999f));
			int iTemp113 = int((iTemp112 & 8191));
			float fTemp114 = ((2.0f + iTemp112) - fRec71[0]);
			float fTemp115 = (fRec71[0] - iTemp112);
			fRec24[0] = (fVec17[(IOTA-int((1 + iTemp113)))&16383] + ((fRec24[1] * (0 - (fTemp114 / fTemp115))) + ((fTemp114 * fVec17[(IOTA-iTemp113)&16383]) / fTemp115)));
			fRec22[0] = fRec24[0];
			float fTemp116 = ((0.7071067811865475f * fTemp108) + (0.7071067811865476f * fTemp110));
			fVec18[IOTA&16383] = fTemp116;
			fRec73[0] = (fConst35 + (0.9999f * ((iConst34 * iTemp3) + fRec73[1])));
			int iTemp117 = int((fRec73[0] - 1.49999f));
			int iTemp118 = int((iTemp117 & 8191));
			int iTemp119 = int((1 + iTemp118));
			float fTemp120 = ((2.0f + iTemp117) - fRec73[0]);
			float fTemp121 = (fRec73[0] - iTemp117);
			float fTemp122 = (0 - (fTemp120 / fTemp121));
			fRec72[0] = (fVec18[(IOTA-iTemp119)&16383] + ((fRec72[1] * fTemp122) + ((fTemp120 * fVec18[(IOTA-iTemp118)&16383]) / fTemp121)));
			fRec23[0] = fRec72[0];
			float fTemp123 = ((0.7603139617444397f * fRec22[1]) + (0.6495557555564224f * fTemp107));
			float fTemp124 = ((0.7603139617444397f * fTemp123) - (0.6495557555564224f * fRec19[1]));
			float fTemp125 = ((0.7603139617444397f * fRec23[1]) + (0.6495557555564224f * fTemp109));
			float fTemp126 = ((0.7603139617444397f * fTemp125) - (0.6495557555564224f * fRec20[1]));
			float fTemp127 = ((0.7071067811865476f * fTemp124) - (0.7071067811865475f * fTemp126));
			fVec19[IOTA&16383] = fTemp127;
			fRec74[0] = (fConst37 + (0.9999f * ((iConst36 * iTemp3) + fRec74[1])));
			int iTemp128 = int((fRec74[0] - 1.49999f));
			int iTemp129 = int((iTemp128 & 8191));
			int iTemp130 = int((1 + iTemp129));
			float fTemp131 = ((2.0f + iTemp128) - fRec74[0]);
			float fTemp132 = (fRec74[0] - iTemp128);
			float fTemp133 = (0 - (fTemp131 / fTemp132));
			fRec21[0] = (fVec19[(IOTA-iTemp130)&16383] + ((fTemp133 * fRec21[1]) + ((fTemp131 * fVec19[(IOTA-iTemp129)&16383]) / fTemp132)));
			fRec19[0] = fRec21[0];
			float fTemp134 = ((0.7071067811865475f * fTemp124) + (0.7071067811865476f * fTemp126));
			fVec20[IOTA&16383] = fTemp134;
			fRec76[0] = (fConst39 + (0.9999f * ((iConst38 * iTemp3) + fRec76[1])));
			int iTemp135 = int((fRec76[0] - 1.49999f));
			int iTemp136 = int((iTemp135 & 8191));
			int iTemp137 = int((1 + iTemp136));
			float fTemp138 = ((2.0f + iTemp135) - fRec76[0]);
			float fTemp139 = (fRec76[0] - iTemp135);
			float fTemp140 = (0 - (fTemp138 / fTemp139));
			fRec75[0] = (fVec20[(IOTA-iTemp137)&16383] + ((fTemp140 * fRec75[1]) + ((fTemp138 * fVec20[(IOTA-iTemp136)&16383]) / fTemp139)));
			fRec20[0] = fRec75[0];
			float fTemp141 = ((0.7603139617444397f * fRec19[1]) + (0.6495557555564224f * fTemp123));
			float fTemp142 = ((0.7603139617444397f * fTemp141) - (0.6495557555564224f * fRec16[1]));
			float fTemp143 = ((0.7603139617444397f * fRec20[1]) + (0.6495557555564224f * fTemp125));
			float fTemp144 = ((0.7603139617444397f * fTemp143) - (0.6495557555564224f * fRec17[1]));
			float fTemp145 = ((0.7071067811865476f * fTemp142) - (0.7071067811865475f * fTemp144));
			fVec21[IOTA&16383] = fTemp145;
			fRec77[0] = (fConst41 + (0.9999f * ((iConst40 * iTemp3) + fRec77[1])));
			int iTemp146 = int((fRec77[0] - 1.49999f));
			int iTemp147 = int((iTemp146 & 8191));
			float fTemp148 = ((2.0f + iTemp146) - fRec77[0]);
			float fTemp149 = (fRec77[0] - iTemp146);
			fRec18[0] = (fVec21[(IOTA-int((1 + iTemp147)))&16383] + ((fRec18[1] * (0 - (fTemp148 / fTemp149))) + ((fTemp148 * fVec21[(IOTA-iTemp147)&16383]) / fTemp149)));
			fRec16[0] = fRec18[0];
			float fTemp150 = ((0.7071067811865475f * fTemp142) + (0.7071067811865476f * fTemp144));
			fVec22[IOTA&16383] = fTemp150;
			fRec79[0] = (fConst43 + (0.9999f * ((iConst42 * iTemp3) + fRec79[1])));
			int iTemp151 = int((fRec79[0] - 1.49999f));
			int iTemp152 = int((iTemp151 & 8191));
			int iTemp153 = int((1 + iTemp152));
			float fTemp154 = ((2.0f + iTemp151) - fRec79[0]);
			float fTemp155 = (fRec79[0] - iTemp151);
			float fTemp156 = (0 - (fTemp154 / fTemp155));
			fRec78[0] = (fVec22[(IOTA-iTemp153)&16383] + ((fRec78[1] * fTemp156) + ((fTemp154 * fVec22[(IOTA-iTemp152)&16383]) / fTemp155)));
			fRec17[0] = fRec78[0];
			float fTemp157 = ((0.7603139617444397f * fRec16[1]) + (0.6495557555564224f * fTemp141));
			fVec23[IOTA&16383] = fTemp157;
			fRec80[0] = (fConst45 + (0.995f * ((iConst44 * iTemp3) + fRec80[1])));
			int iTemp158 = int((fRec80[0] - 1.49999f));
			int iTemp159 = int((iTemp158 & 8191));
			float fTemp160 = ((2.0f + iTemp158) - fRec80[0]);
			float fTemp161 = (fRec80[0] - iTemp158);
			fRec15[0] = (fVec23[(IOTA-int((1 + iTemp159)))&16383] + ((fRec15[1] * (0 - (fTemp160 / fTemp161))) + ((fTemp160 * fVec23[(IOTA-iTemp159)&16383]) / fTemp161)));
			fRec14[0] = ((fSlow4 * fRec14[1]) + (fSlow5 * ((float)input0[i] + (fSlow6 * fRec15[0]))));
			float fTemp162 = ((fSlow3 * fRec11[1]) + (fSlow1 * fRec14[0]));
			float fTemp163 = ((0.7603139617444397f * fRec17[1]) + (0.6495557555564224f * fTemp143));
			fVec24[IOTA&16383] = fTemp163;
			fRec83[0] = (fConst47 + (0.995f * ((iConst46 * iTemp3) + fRec83[1])));
			int iTemp164 = int((fRec83[0] - 1.49999f));
			int iTemp165 = int((iTemp164 & 8191));
			float fTemp166 = ((2.0f + iTemp164) - fRec83[0]);
			float fTemp167 = (fRec83[0] - iTemp164);
			fRec82[0] = (fVec24[(IOTA-int((1 + iTemp165)))&16383] + ((fRec82[1] * (0 - (fTemp166 / fTemp167))) + ((fTemp166 * fVec24[(IOTA-iTemp165)&16383]) / fTemp167)));
			fRec81[0] = ((fSlow4 * fRec81[1]) + (fSlow5 * ((float)input1[i] + (fSlow6 * fRec82[0]))));
			float fTemp168 = ((fSlow3 * fRec12[1]) + (fSlow1 * fRec81[0]));
			float fTemp169 = ((0.7071067811865476f * fTemp162) - (0.7071067811865475f * fTemp168));
			fVec25[IOTA&16383] = fTemp169;
			fRec13[0] = (fVec25[(IOTA-iTemp103)&16383] + ((fRec13[1] * fTemp106) + ((fTemp104 * fVec25[(IOTA-iTemp102)&16383]) / fTemp105)));
			fRec11[0] = fRec13[0];
			float fTemp170 = ((0.7071067811865475f * fTemp162) + (0.7071067811865476f * fTemp168));
			fVec26[IOTA&16383] = fTemp170;
			fRec85[0] = (fConst49 + (0.9999f * ((iConst48 * iTemp3) + fRec85[1])));
			int iTemp171 = int((fRec85[0] - 1.49999f));
			int iTemp172 = int((iTemp171 & 8191));
			float fTemp173 = ((2.0f + iTemp171) - fRec85[0]);
			float fTemp174 = (fRec85[0] - iTemp171);
			fRec84[0] = (fVec26[(IOTA-int((1 + iTemp172)))&16383] + ((fRec84[1] * (0 - (fTemp173 / fTemp174))) + ((fTemp173 * fVec26[(IOTA-iTemp172)&16383]) / fTemp174)));
			fRec12[0] = fRec84[0];
			float fTemp175 = ((fSlow1 * fRec11[1]) + (fSlow2 * fRec14[0]));
			float fTemp176 = ((fSlow3 * fRec8[1]) + (fSlow1 * fTemp175));
			float fTemp177 = ((fSlow1 * fRec12[1]) + (fSlow2 * fRec81[0]));
			float fTemp178 = ((fSlow3 * fRec9[1]) + (fSlow1 * fTemp177));
			float fTemp179 = ((0.7071067811865476f * fTemp176) - (0.7071067811865475f * fTemp178));
			fVec27[IOTA&16383] = fTemp179;
			fRec10[0] = (fVec27[(IOTA-iTemp130)&16383] + ((fRec10[1] * fTemp133) + ((fTemp131 * fVec27[(IOTA-iTemp129)&16383]) / fTemp132)));
			fRec8[0] = fRec10[0];
			float fTemp180 = ((0.7071067811865475f * fTemp176) + (0.7071067811865476f * fTemp178));
			fVec28[IOTA&16383] = fTemp180;
			fRec86[0] = (fVec28[(IOTA-iTemp119)&16383] + ((fTemp122 * fRec86[1]) + ((fTemp120 * fVec28[(IOTA-iTemp118)&16383]) / fTemp121)));
			fRec9[0] = fRec86[0];
			float fTemp181 = ((fSlow1 * fRec8[1]) + (fSlow2 * fTemp175));
			float fTemp182 = ((fSlow3 * fRec5[1]) + (fSlow1 * fTemp181));
			float fTemp183 = ((fSlow1 * fRec9[1]) + (fSlow2 * fTemp177));
			float fTemp184 = ((fSlow3 * fRec6[1]) + (fSlow1 * fTemp183));
			float fTemp185 = ((0.7071067811865476f * fTemp182) - (0.7071067811865475f * fTemp184));
			fVec29[IOTA&16383] = fTemp185;
			fRec7[0] = (fVec29[(IOTA-iTemp137)&16383] + ((fRec7[1] * fTemp140) + ((fTemp138 * fVec29[(IOTA-iTemp136)&16383]) / fTemp139)));
			fRec5[0] = fRec7[0];
			float fTemp186 = ((0.7071067811865475f * fTemp182) + (0.7071067811865476f * fTemp184));
			fVec30[IOTA&16383] = fTemp186;
			fRec88[0] = (fConst51 + (0.9999f * ((iConst50 * iTemp3) + fRec88[1])));
			int iTemp187 = int((fRec88[0] - 1.49999f));
			int iTemp188 = int((iTemp187 & 8191));
			float fTemp189 = ((2.0f + iTemp187) - fRec88[0]);
			float fTemp190 = (fRec88[0] - iTemp187);
			fRec87[0] = (fVec30[(IOTA-int((1 + iTemp188)))&16383] + ((fRec87[1] * (0 - (fTemp189 / fTemp190))) + ((fTemp189 * fVec30[(IOTA-iTemp188)&16383]) / fTemp190)));
			fRec6[0] = fRec87[0];
			float fTemp191 = ((fSlow1 * fRec5[1]) + (fSlow2 * fTemp181));
			float fTemp192 = ((fSlow3 * fRec2[1]) + (fSlow1 * fTemp191));
			float fTemp193 = ((fSlow1 * fRec6[1]) + (fSlow2 * fTemp183));
			float fTemp194 = ((fSlow3 * fRec3[1]) + (fSlow1 * fTemp193));
			float fTemp195 = ((0.7071067811865476f * fTemp192) - (0.7071067811865475f * fTemp194));
			fVec31[IOTA&16383] = fTemp195;
			fRec89[0] = (fConst53 + (0.9999f * ((iConst52 * iTemp3) + fRec89[1])));
			int iTemp196 = int((fRec89[0] - 1.49999f));
			int iTemp197 = int((iTemp196 & 8191));
			float fTemp198 = ((2.0f + iTemp196) - fRec89[0]);
			float fTemp199 = (fRec89[0] - iTemp196);
			fRec4[0] = (fVec31[(IOTA-int((1 + iTemp197)))&16383] + ((fRec4[1] * (0 - (fTemp198 / fTemp199))) + ((fTemp198 * fVec31[(IOTA-iTemp197)&16383]) / fTemp199)));
			fRec2[0] = fRec4[0];
			float fTemp200 = ((0.7071067811865475f * fTemp192) + (0.7071067811865476f * fTemp194));
			fVec32[IOTA&16383] = fTemp200;
			fRec90[0] = (fVec32[(IOTA-iTemp153)&16383] + ((fTemp156 * fRec90[1]) + ((fTemp154 * fVec32[(IOTA-iTemp152)&16383]) / fTemp155)));
			fRec3[0] = fRec90[0];
			fRec0[0] = ((fSlow1 * fRec2[1]) + (fSlow2 * fTemp191));
			fRec1[0] = ((fSlow1 * fRec3[1]) + (fSlow2 * fTemp193));
			output0[i] = (FAUSTFLOAT)fRec0[0];
			output1[i] = (FAUSTFLOAT)fRec1[0];
			// post processing
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec3[1] = fRec3[0];
			fRec90[1] = fRec90[0];
			fRec2[1] = fRec2[0];
			fRec4[1] = fRec4[0];
			fRec89[1] = fRec89[0];
			fRec6[1] = fRec6[0];
			fRec87[1] = fRec87[0];
			fRec88[1] = fRec88[0];
			fRec5[1] = fRec5[0];
			fRec7[1] = fRec7[0];
			fRec9[1] = fRec9[0];
			fRec86[1] = fRec86[0];
			fRec8[1] = fRec8[0];
			fRec10[1] = fRec10[0];
			fRec12[1] = fRec12[0];
			fRec84[1] = fRec84[0];
			fRec85[1] = fRec85[0];
			fRec11[1] = fRec11[0];
			fRec13[1] = fRec13[0];
			fRec81[1] = fRec81[0];
			fRec82[1] = fRec82[0];
			fRec83[1] = fRec83[0];
			fRec14[1] = fRec14[0];
			fRec15[1] = fRec15[0];
			fRec80[1] = fRec80[0];
			fRec17[1] = fRec17[0];
			fRec78[1] = fRec78[0];
			fRec79[1] = fRec79[0];
			fRec16[1] = fRec16[0];
			fRec18[1] = fRec18[0];
			fRec77[1] = fRec77[0];
			fRec20[1] = fRec20[0];
			fRec75[1] = fRec75[0];
			fRec76[1] = fRec76[0];
			fRec19[1] = fRec19[0];
			fRec21[1] = fRec21[0];
			fRec74[1] = fRec74[0];
			fRec23[1] = fRec23[0];
			fRec72[1] = fRec72[0];
			fRec73[1] = fRec73[0];
			fRec22[1] = fRec22[0];
			fRec24[1] = fRec24[0];
			fRec71[1] = fRec71[0];
			fRec26[1] = fRec26[0];
			fRec69[1] = fRec69[0];
			fRec70[1] = fRec70[0];
			fRec25[1] = fRec25[0];
			fRec27[1] = fRec27[0];
			fRec68[1] = fRec68[0];
			fRec29[1] = fRec29[0];
			fRec66[1] = fRec66[0];
			fRec67[1] = fRec67[0];
			fRec28[1] = fRec28[0];
			fRec30[1] = fRec30[0];
			fRec65[1] = fRec65[0];
			fRec63[1] = fRec63[0];
			fRec64[1] = fRec64[0];
			fRec31[1] = fRec31[0];
			fRec62[1] = fRec62[0];
			fRec33[1] = fRec33[0];
			fRec60[1] = fRec60[0];
			fRec61[1] = fRec61[0];
			fRec32[1] = fRec32[0];
			fRec34[1] = fRec34[0];
			fRec59[1] = fRec59[0];
			fRec36[1] = fRec36[0];
			fRec57[1] = fRec57[0];
			fRec58[1] = fRec58[0];
			fRec35[1] = fRec35[0];
			fRec37[1] = fRec37[0];
			fRec56[1] = fRec56[0];
			fRec39[1] = fRec39[0];
			fRec54[1] = fRec54[0];
			fRec55[1] = fRec55[0];
			fRec38[1] = fRec38[0];
			fRec40[1] = fRec40[0];
			fRec53[1] = fRec53[0];
			fRec42[1] = fRec42[0];
			fRec51[1] = fRec51[0];
			fRec52[1] = fRec52[0];
			fRec41[1] = fRec41[0];
			fRec43[1] = fRec43[0];
			fRec50[1] = fRec50[0];
			fRec45[1] = fRec45[0];
			fRec48[1] = fRec48[0];
			fRec49[1] = fRec49[0];
			fRec44[1] = fRec44[0];
			fRec46[1] = fRec46[0];
			fRec47[1] = fRec47[0];
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
