//-----------------------------------------------------
// name: "JPverb"
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
	int 	iConst0;
	float 	fConst1;
	float 	fRec7[2];
	FAUSTFLOAT 	fslider1;
	float 	fConst2;
	float 	fRec13[2];
	float 	fConst3;
	float 	fRec19[2];
	float 	fConst4;
	float 	fRec25[2];
	FAUSTFLOAT 	fslider2;
	FAUSTFLOAT 	fslider3;
	float 	fConst5;
	FAUSTFLOAT 	fslider4;
	float 	fConst6;
	float 	fRec38[2];
	float 	fConst7;
	float 	fRec49[2];
	float 	fConst8;
	float 	fRec55[2];
	float 	fConst9;
	float 	fRec61[2];
	float 	fConst10;
	float 	fRec67[2];
	float 	fConst11;
	float 	fRec73[2];
	float 	fConst12;
	float 	fRec79[2];
	float 	fConst13;
	float 	fRec85[2];
	float 	fConst14;
	float 	fRec91[2];
	int 	IOTA;
	float 	fVec1[131072];
	float 	fRec90[2];
	float 	fRec86[2];
	float 	fConst15;
	float 	fRec93[2];
	float 	fVec2[131072];
	float 	fRec92[2];
	float 	fRec87[2];
	float 	fVec3[131072];
	float 	fRec84[2];
	float 	fRec80[2];
	float 	fConst16;
	float 	fRec95[2];
	float 	fVec4[131072];
	float 	fRec94[2];
	float 	fRec81[2];
	float 	fVec5[131072];
	float 	fRec78[2];
	float 	fRec74[2];
	float 	fConst17;
	float 	fRec97[2];
	float 	fVec6[131072];
	float 	fRec96[2];
	float 	fRec75[2];
	float 	fVec7[131072];
	float 	fRec72[2];
	float 	fRec68[2];
	float 	fConst18;
	float 	fRec99[2];
	float 	fVec8[131072];
	float 	fRec98[2];
	float 	fRec69[2];
	float 	fVec9[131072];
	float 	fRec66[2];
	float 	fRec62[2];
	float 	fConst19;
	float 	fRec101[2];
	float 	fVec10[131072];
	float 	fRec100[2];
	float 	fRec63[2];
	float 	fVec11[65536];
	FAUSTFLOAT 	fslider5;
	float 	fConst20;
	float 	fRec102[2];
	float 	fRec103[2];
	FAUSTFLOAT 	fslider6;
	float 	fConst21;
	float 	fConst22;
	float 	fRec104[2];
	float 	fConst23;
	float 	fVec12[65536];
	float 	fConst24;
	float 	fRec105[2];
	float 	fVec13[131072];
	float 	fRec60[2];
	float 	fRec56[2];
	float 	fConst25;
	float 	fRec107[2];
	float 	fVec14[131072];
	float 	fRec106[2];
	float 	fRec57[2];
	float 	fVec15[131072];
	float 	fRec54[2];
	float 	fRec50[2];
	float 	fRec109[2];
	float 	fVec16[131072];
	float 	fRec108[2];
	float 	fRec51[2];
	float 	fVec17[131072];
	float 	fRec48[2];
	float 	fRec44[2];
	float 	fConst26;
	float 	fRec111[2];
	float 	fVec18[131072];
	float 	fRec110[2];
	float 	fRec45[2];
	float 	fVec19[131072];
	float 	fRec43[2];
	float 	fRec39[2];
	float 	fRec113[2];
	float 	fVec20[131072];
	float 	fRec112[2];
	float 	fRec40[2];
	float 	fVec21[131072];
	float 	fRec37[2];
	float 	fRec33[2];
	float 	fConst27;
	float 	fRec115[2];
	float 	fVec22[131072];
	float 	fRec114[2];
	float 	fRec34[2];
	float 	fVec23[65536];
	float 	fConst28;
	float 	fRec116[2];
	float 	fVec24[2];
	float 	fRec32[2];
	float 	fRec31[3];
	float 	fRec30[3];
	float 	fVec25[2];
	float 	fRec29[2];
	float 	fRec28[3];
	float 	fRec27[3];
	FAUSTFLOAT 	fslider7;
	float 	fRec119[2];
	float 	fRec118[3];
	float 	fRec117[3];
	FAUSTFLOAT 	fslider8;
	float 	fRec123[2];
	float 	fRec122[3];
	float 	fRec121[3];
	float 	fRec120[3];
	FAUSTFLOAT 	fslider9;
	FAUSTFLOAT 	fslider10;
	float 	fConst29;
	float 	fVec26[65536];
	float 	fRec26[2];
	float 	fVec27[65536];
	float 	fConst30;
	float 	fRec131[2];
	float 	fVec28[2];
	float 	fRec130[2];
	float 	fRec129[3];
	float 	fRec128[3];
	float 	fVec29[2];
	float 	fRec127[2];
	float 	fRec126[3];
	float 	fRec125[3];
	float 	fRec134[2];
	float 	fRec133[3];
	float 	fRec132[3];
	float 	fRec138[2];
	float 	fRec137[3];
	float 	fRec136[3];
	float 	fRec135[3];
	float 	fVec30[65536];
	float 	fRec124[2];
	float 	fVec31[131072];
	float 	fRec24[2];
	float 	fRec20[2];
	float 	fConst31;
	float 	fRec140[2];
	float 	fVec32[131072];
	float 	fRec139[2];
	float 	fRec21[2];
	float 	fVec33[131072];
	float 	fRec18[2];
	float 	fRec14[2];
	float 	fVec34[131072];
	float 	fRec141[2];
	float 	fRec15[2];
	float 	fVec35[131072];
	float 	fRec12[2];
	float 	fRec8[2];
	float 	fConst32;
	float 	fRec143[2];
	float 	fVec36[131072];
	float 	fRec142[2];
	float 	fRec9[2];
	float 	fVec37[131072];
	float 	fRec6[2];
	float 	fRec2[2];
	float 	fVec38[131072];
	float 	fRec144[2];
	float 	fRec3[2];
	float 	fRec0[2];
	float 	fRec1[2];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("name", "JPverb");
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
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (float(7717500) / float(iConst0));
		for (int i=0; i<2; i++) fRec7[i] = 0;
		fslider1 = 0.707f;
		fConst2 = (float(5071500) / float(iConst0));
		for (int i=0; i<2; i++) fRec13[i] = 0;
		fConst3 = (float(9481500) / float(iConst0));
		for (int i=0; i<2; i++) fRec19[i] = 0;
		fConst4 = (float(2425500) / float(iConst0));
		for (int i=0; i<2; i++) fRec25[i] = 0;
		fslider2 = 0.0f;
		fslider3 = 5e+02f;
		fConst5 = (3.141592653589793f / float(iConst0));
		fslider4 = 2e+03f;
		fConst6 = (float(10804500) / float(iConst0));
		for (int i=0; i<2; i++) fRec38[i] = 0;
		fConst7 = (float(8158500) / float(iConst0));
		for (int i=0; i<2; i++) fRec49[i] = 0;
		fConst8 = (float(6835500) / float(iConst0));
		for (int i=0; i<2; i++) fRec55[i] = 0;
		fConst9 = (float(5512500) / float(iConst0));
		for (int i=0; i<2; i++) fRec61[i] = 0;
		fConst10 = (float(5733000) / float(iConst0));
		for (int i=0; i<2; i++) fRec67[i] = 0;
		fConst11 = (float(4410000) / float(iConst0));
		for (int i=0; i<2; i++) fRec73[i] = 0;
		fConst12 = (float(3087000) / float(iConst0));
		for (int i=0; i<2; i++) fRec79[i] = 0;
		fConst13 = (float(1764000) / float(iConst0));
		for (int i=0; i<2; i++) fRec85[i] = 0;
		fConst14 = (float(441000) / float(iConst0));
		for (int i=0; i<2; i++) fRec91[i] = 0;
		IOTA = 0;
		for (int i=0; i<131072; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec90[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		fConst15 = (float(4851000) / float(iConst0));
		for (int i=0; i<2; i++) fRec93[i] = 0;
		for (int i=0; i<131072; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec92[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<131072; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		fConst16 = (float(6174000) / float(iConst0));
		for (int i=0; i<2; i++) fRec95[i] = 0;
		for (int i=0; i<131072; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fRec94[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<131072; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		fConst17 = (float(7497000) / float(iConst0));
		for (int i=0; i<2; i++) fRec97[i] = 0;
		for (int i=0; i<131072; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec96[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<131072; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		fConst18 = (float(8820000) / float(iConst0));
		for (int i=0; i<2; i++) fRec99[i] = 0;
		for (int i=0; i<131072; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec98[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<131072; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		fConst19 = (float(10143000) / float(iConst0));
		for (int i=0; i<2; i++) fRec101[i] = 0;
		for (int i=0; i<131072; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec100[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<65536; i++) fVec11[i] = 0;
		fslider5 = 2.0f;
		fConst20 = (6.283185307179586f / float(iConst0));
		for (int i=0; i<2; i++) fRec102[i] = 0;
		for (int i=0; i<2; i++) fRec103[i] = 0;
		fslider6 = 0.1f;
		fConst21 = (2.205e+06f / float(iConst0));
		fConst22 = (float(8996400) / float(iConst0));
		for (int i=0; i<2; i++) fRec104[i] = 0;
		fConst23 = (float(2205000) / float(iConst0));
		for (int i=0; i<65536; i++) fVec12[i] = 0;
		fConst24 = (float(2381400) / float(iConst0));
		for (int i=0; i<2; i++) fRec105[i] = 0;
		for (int i=0; i<131072; i++) fVec13[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		fConst25 = (float(1102500) / float(iConst0));
		for (int i=0; i<2; i++) fRec107[i] = 0;
		for (int i=0; i<131072; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec106[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<131072; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec109[i] = 0;
		for (int i=0; i<131072; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec108[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<131072; i++) fVec17[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		fConst26 = (float(3748500) / float(iConst0));
		for (int i=0; i<2; i++) fRec111[i] = 0;
		for (int i=0; i<131072; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec110[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<131072; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec113[i] = 0;
		for (int i=0; i<131072; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fRec112[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<131072; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		fConst27 = (float(6394500) / float(iConst0));
		for (int i=0; i<2; i++) fRec115[i] = 0;
		for (int i=0; i<131072; i++) fVec22[i] = 0;
		for (int i=0; i<2; i++) fRec114[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<65536; i++) fVec23[i] = 0;
		fConst28 = (float(1499400) / float(iConst0));
		for (int i=0; i<2; i++) fRec116[i] = 0;
		for (int i=0; i<2; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<3; i++) fRec31[i] = 0;
		for (int i=0; i<3; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fVec25[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<3; i++) fRec28[i] = 0;
		for (int i=0; i<3; i++) fRec27[i] = 0;
		fslider7 = 1.0f;
		for (int i=0; i<2; i++) fRec119[i] = 0;
		for (int i=0; i<3; i++) fRec118[i] = 0;
		for (int i=0; i<3; i++) fRec117[i] = 0;
		fslider8 = 1.0f;
		for (int i=0; i<2; i++) fRec123[i] = 0;
		for (int i=0; i<3; i++) fRec122[i] = 0;
		for (int i=0; i<3; i++) fRec121[i] = 0;
		for (int i=0; i<3; i++) fRec120[i] = 0;
		fslider9 = 1.0f;
		fslider10 = 1.0f;
		fConst29 = (float(55125) / float(iConst0));
		for (int i=0; i<65536; i++) fVec26[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<65536; i++) fVec27[i] = 0;
		fConst30 = (float(5909400) / float(iConst0));
		for (int i=0; i<2; i++) fRec131[i] = 0;
		for (int i=0; i<2; i++) fVec28[i] = 0;
		for (int i=0; i<2; i++) fRec130[i] = 0;
		for (int i=0; i<3; i++) fRec129[i] = 0;
		for (int i=0; i<3; i++) fRec128[i] = 0;
		for (int i=0; i<2; i++) fVec29[i] = 0;
		for (int i=0; i<2; i++) fRec127[i] = 0;
		for (int i=0; i<3; i++) fRec126[i] = 0;
		for (int i=0; i<3; i++) fRec125[i] = 0;
		for (int i=0; i<2; i++) fRec134[i] = 0;
		for (int i=0; i<3; i++) fRec133[i] = 0;
		for (int i=0; i<3; i++) fRec132[i] = 0;
		for (int i=0; i<2; i++) fRec138[i] = 0;
		for (int i=0; i<3; i++) fRec137[i] = 0;
		for (int i=0; i<3; i++) fRec136[i] = 0;
		for (int i=0; i<3; i++) fRec135[i] = 0;
		for (int i=0; i<65536; i++) fVec30[i] = 0;
		for (int i=0; i<2; i++) fRec124[i] = 0;
		for (int i=0; i<131072; i++) fVec31[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		fConst31 = (float(10584000) / float(iConst0));
		for (int i=0; i<2; i++) fRec140[i] = 0;
		for (int i=0; i<131072; i++) fVec32[i] = 0;
		for (int i=0; i<2; i++) fRec139[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<131072; i++) fVec33[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<131072; i++) fVec34[i] = 0;
		for (int i=0; i<2; i++) fRec141[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<131072; i++) fVec35[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		fConst32 = (float(8379000) / float(iConst0));
		for (int i=0; i<2; i++) fRec143[i] = 0;
		for (int i=0; i<131072; i++) fVec36[i] = 0;
		for (int i=0; i<2; i++) fRec142[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<131072; i++) fVec37[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<131072; i++) fVec38[i] = 0;
		for (int i=0; i<2; i++) fRec144[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("JPverb");
		interface->addHorizontalSlider("damp", &fslider2, 0.0f, 0.0f, 0.999f, 0.0001f);
		interface->addHorizontalSlider("earlyDiff", &fslider1, 0.707f, 0.0f, 0.99f, 0.001f);
		interface->addHorizontalSlider("highBand", &fslider4, 2e+03f, 1e+03f, 1e+04f, 0.1f);
		interface->addHorizontalSlider("highX", &fslider9, 1.0f, 0.0f, 1.0f, 0.01f);
		interface->addHorizontalSlider("lowBand", &fslider3, 5e+02f, 1e+02f, 6e+03f, 0.1f);
		interface->addHorizontalSlider("lowX", &fslider8, 1.0f, 0.0f, 1.0f, 0.01f);
		interface->addHorizontalSlider("mDepth", &fslider6, 0.1f, 0.0f, 1.0f, 0.001f);
		interface->addHorizontalSlider("mFreq", &fslider5, 2.0f, 0.0f, 1e+01f, 0.01f);
		interface->addHorizontalSlider("midX", &fslider7, 1.0f, 0.0f, 1.0f, 0.01f);
		interface->addHorizontalSlider("size", &fslider0, 1.0f, 0.5f, 3.0f, 0.01f);
		interface->addHorizontalSlider("t60", &fslider10, 1.0f, 0.1f, 6e+01f, 0.1f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = float(fslider0);
		float 	fSlow1 = (0.010000000000000009f * primes((fConst1 * fSlow0)));
		float 	fSlow2 = float(fslider1);
		int 	iSlow3 = primes((fConst2 * fSlow0));
		float 	fSlow4 = (0.010000000000000009f * iSlow3);
		float 	fSlow5 = (0.010000000000000009f * primes((fConst3 * fSlow0)));
		int 	iSlow6 = primes((fConst4 * fSlow0));
		float 	fSlow7 = (0.010000000000000009f * iSlow6);
		float 	fSlow8 = float(fslider2);
		float 	fSlow9 = tanf((fConst5 * float(fslider3)));
		float 	fSlow10 = (1.0f / faustpower<2>(fSlow9));
		float 	fSlow11 = (2 * (1 - fSlow10));
		float 	fSlow12 = (1.0f / fSlow9);
		float 	fSlow13 = (1 + ((fSlow12 - 0.6180339887498947f) / fSlow9));
		float 	fSlow14 = (1.0f / (1 + ((fSlow12 + 0.6180339887498947f) / fSlow9)));
		float 	fSlow15 = (1 + ((fSlow12 - 1.6180339887498947f) / fSlow9));
		float 	fSlow16 = (1.0f / (1 + ((fSlow12 + 1.6180339887498947f) / fSlow9)));
		float 	fSlow17 = tanf((fConst5 * float(fslider4)));
		float 	fSlow18 = (1.0f / faustpower<2>(fSlow17));
		float 	fSlow19 = (2 * (1 - fSlow18));
		float 	fSlow20 = (1.0f / fSlow17);
		float 	fSlow21 = (1 + ((fSlow20 - 0.6180339887498947f) / fSlow17));
		float 	fSlow22 = (1 + ((0.6180339887498947f + fSlow20) / fSlow17));
		float 	fSlow23 = (1.0f / fSlow22);
		float 	fSlow24 = (1 + ((fSlow20 - 1.6180339887498947f) / fSlow17));
		float 	fSlow25 = (1.0f / (1 + ((1.6180339887498947f + fSlow20) / fSlow17)));
		float 	fSlow26 = (1 + fSlow20);
		float 	fSlow27 = (0 - ((1 - fSlow20) / fSlow26));
		float 	fSlow28 = (0.010000000000000009f * primes((fConst6 * fSlow0)));
		float 	fSlow29 = (0.010000000000000009f * primes((fConst7 * fSlow0)));
		float 	fSlow30 = (0.010000000000000009f * primes((fConst8 * fSlow0)));
		float 	fSlow31 = (0.010000000000000009f * primes((fConst9 * fSlow0)));
		float 	fSlow32 = (0.010000000000000009f * primes((fConst10 * fSlow0)));
		float 	fSlow33 = (0.010000000000000009f * primes((fConst11 * fSlow0)));
		float 	fSlow34 = (0.010000000000000009f * primes((fConst12 * fSlow0)));
		float 	fSlow35 = (0.010000000000000009f * primes((fConst13 * fSlow0)));
		float 	fSlow36 = (0.010000000000000009f * primes((fConst14 * fSlow0)));
		float 	fSlow37 = (0.0010000000000000009f * primes((fConst15 * fSlow0)));
		float 	fSlow38 = (0.0010000000000000009f * primes((fConst16 * fSlow0)));
		float 	fSlow39 = (0.0010000000000000009f * primes((fConst17 * fSlow0)));
		float 	fSlow40 = (0.0010000000000000009f * primes((fConst18 * fSlow0)));
		float 	fSlow41 = (0.0010000000000000009f * primes((fConst19 * fSlow0)));
		float 	fSlow42 = (fConst20 * float(fslider5));
		float 	fSlow43 = sinf(fSlow42);
		float 	fSlow44 = cosf(fSlow42);
		float 	fSlow45 = (0 - fSlow43);
		float 	fSlow46 = float(fslider6);
		float 	fSlow47 = (0 - (fConst21 * fSlow46));
		float 	fSlow48 = (0.0010000000000000009f * primes((fConst22 * fSlow0)));
		float 	fSlow49 = (fConst23 * fSlow46);
		float 	fSlow50 = (fSlow49 - 2.49999f);
		float 	fSlow51 = (fSlow49 - 3.0f);
		float 	fSlow52 = (fSlow49 - 1.0f);
		float 	fSlow53 = (fSlow49 - 5.0f);
		float 	fSlow54 = (fSlow49 - 2.0f);
		float 	fSlow55 = (fSlow49 - 4.0f);
		float 	fSlow56 = (0.0010000000000000009f * primes((fConst24 * fSlow0)));
		float 	fSlow57 = (0.0010000000000000009f * primes((fConst25 * fSlow0)));
		float 	fSlow58 = (0.0010000000000000009f * iSlow6);
		float 	fSlow59 = (0.0010000000000000009f * primes((fConst26 * fSlow0)));
		float 	fSlow60 = (0.0010000000000000009f * iSlow3);
		float 	fSlow61 = (0.0010000000000000009f * primes((fConst27 * fSlow0)));
		float 	fSlow62 = (0.0010000000000000009f * primes((fConst28 * fSlow0)));
		float 	fSlow63 = (1.0f / fSlow26);
		float 	fSlow64 = (1.0f / (fSlow9 * fSlow22));
		float 	fSlow65 = (0 - fSlow12);
		float 	fSlow66 = (1 + fSlow12);
		float 	fSlow67 = (1.0f / fSlow66);
		float 	fSlow68 = (0 - ((1 - fSlow12) / fSlow66));
		float 	fSlow69 = (2 * (0 - fSlow10));
		float 	fSlow70 = float(fslider7);
		float 	fSlow71 = float(fslider8);
		float 	fSlow72 = (1 + ((fSlow12 - 1.618033988749895f) / fSlow9));
		float 	fSlow73 = (1.0f / (1 + ((1.618033988749895f + fSlow12) / fSlow9)));
		float 	fSlow74 = (0 - fSlow20);
		float 	fSlow75 = (2 * (0 - fSlow18));
		float 	fSlow76 = float(fslider9);
		float 	fSlow77 = powf(10,(0 - (0.51f * (((fConst29 * fSlow0) - 0.25f) / float(fslider10)))));
		float 	fSlow78 = (1.0f - fSlow8);
		float 	fSlow79 = (0.0010000000000000009f * primes((fConst30 * fSlow0)));
		float 	fSlow80 = (0.0010000000000000009f * primes((fConst31 * fSlow0)));
		float 	fSlow81 = (0.0010000000000000009f * primes((fConst32 * fSlow0)));
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			iVec0[0] = 1;
			fRec7[0] = (fSlow1 + (0.99f * fRec7[1]));
			int iTemp0 = int((fRec7[0] - 1.49999f));
			float fTemp1 = (fRec7[0] - iTemp0);
			float fTemp2 = ((2.0f + iTemp0) - fRec7[0]);
			fRec13[0] = (fSlow4 + (0.99f * fRec13[1]));
			int iTemp3 = int((fRec13[0] - 1.49999f));
			float fTemp4 = (fRec13[0] - iTemp3);
			float fTemp5 = ((2.0f + iTemp3) - fRec13[0]);
			fRec19[0] = (fSlow5 + (0.99f * fRec19[1]));
			int iTemp6 = int((fRec19[0] - 1.49999f));
			float fTemp7 = (fRec19[0] - iTemp6);
			float fTemp8 = ((2.0f + iTemp6) - fRec19[0]);
			float fTemp9 = (0 - (fTemp8 / fTemp7));
			fRec25[0] = (fSlow7 + (0.99f * fRec25[1]));
			int iTemp10 = int((fRec25[0] - 1.49999f));
			float fTemp11 = (fRec25[0] - iTemp10);
			float fTemp12 = ((2.0f + iTemp10) - fRec25[0]);
			fRec38[0] = (fSlow28 + (0.99f * fRec38[1]));
			int iTemp13 = int((fRec38[0] - 1.49999f));
			float fTemp14 = (fRec38[0] - iTemp13);
			float fTemp15 = ((2.0f + iTemp13) - fRec38[0]);
			fRec49[0] = (fSlow29 + (0.99f * fRec49[1]));
			int iTemp16 = int((fRec49[0] - 1.49999f));
			float fTemp17 = (fRec49[0] - iTemp16);
			float fTemp18 = ((2.0f + iTemp16) - fRec49[0]);
			fRec55[0] = (fSlow30 + (0.99f * fRec55[1]));
			int iTemp19 = int((fRec55[0] - 1.49999f));
			float fTemp20 = (fRec55[0] - iTemp19);
			float fTemp21 = ((2.0f + iTemp19) - fRec55[0]);
			fRec61[0] = (fSlow31 + (0.99f * fRec61[1]));
			int iTemp22 = int((fRec61[0] - 1.49999f));
			float fTemp23 = (fRec61[0] - iTemp22);
			float fTemp24 = ((2.0f + iTemp22) - fRec61[0]);
			fRec67[0] = (fSlow32 + (0.99f * fRec67[1]));
			int iTemp25 = int((fRec67[0] - 1.49999f));
			float fTemp26 = (fRec67[0] - iTemp25);
			float fTemp27 = ((2.0f + iTemp25) - fRec67[0]);
			fRec73[0] = (fSlow33 + (0.99f * fRec73[1]));
			int iTemp28 = int((fRec73[0] - 1.49999f));
			float fTemp29 = (fRec73[0] - iTemp28);
			float fTemp30 = ((2.0f + iTemp28) - fRec73[0]);
			fRec79[0] = (fSlow34 + (0.99f * fRec79[1]));
			int iTemp31 = int((fRec79[0] - 1.49999f));
			float fTemp32 = (fRec79[0] - iTemp31);
			float fTemp33 = ((2.0f + iTemp31) - fRec79[0]);
			fRec85[0] = (fSlow35 + (0.99f * fRec85[1]));
			int iTemp34 = int((fRec85[0] - 1.49999f));
			float fTemp35 = (fRec85[0] - iTemp34);
			float fTemp36 = ((2.0f + iTemp34) - fRec85[0]);
			fRec91[0] = (fSlow36 + (0.99f * fRec91[1]));
			int iTemp37 = int((fRec91[0] - 1.49999f));
			float fTemp38 = (fRec91[0] - iTemp37);
			float fTemp39 = ((2.0f + iTemp37) - fRec91[0]);
			float fTemp40 = (fRec1[1] - (0.707f * fRec87[1]));
			float fTemp41 = (fRec0[1] - (0.707f * fRec86[1]));
			float fTemp42 = ((0.7071067811865476f * fTemp41) - (0.7071067811865475f * fTemp40));
			fVec1[IOTA&131071] = fTemp42;
			int iTemp43 = int((iTemp37 & 65535));
			fRec90[0] = ((fVec1[(IOTA-int((1 + iTemp43)))&131071] + ((fVec1[(IOTA-iTemp43)&131071] * fTemp39) / fTemp38)) + ((0 - (fTemp39 / fTemp38)) * fRec90[1]));
			fRec86[0] = fRec90[0];
			fRec93[0] = (fSlow37 + (0.999f * fRec93[1]));
			int iTemp44 = int((fRec93[0] - 1.49999f));
			float fTemp45 = (fRec93[0] - iTemp44);
			float fTemp46 = ((2.0f + iTemp44) - fRec93[0]);
			float fTemp47 = ((0.7071067811865476f * fTemp40) + (0.7071067811865475f * fTemp41));
			fVec2[IOTA&131071] = fTemp47;
			int iTemp48 = int((iTemp44 & 65535));
			fRec92[0] = ((fVec2[(IOTA-int((1 + iTemp48)))&131071] + ((fVec2[(IOTA-iTemp48)&131071] * fTemp46) / fTemp45)) + ((0 - (fTemp46 / fTemp45)) * fRec92[1]));
			fRec87[0] = fRec92[0];
			float 	fRec88 = (0.707f * fTemp41);
			float 	fRec89 = (0.707f * fTemp40);
			float fTemp49 = ((fRec89 + fRec87[1]) - (0.707f * fRec81[1]));
			float fTemp50 = ((fRec88 + fRec86[1]) - (0.707f * fRec80[1]));
			float fTemp51 = ((0.7071067811865476f * fTemp50) - (0.7071067811865475f * fTemp49));
			fVec3[IOTA&131071] = fTemp51;
			int iTemp52 = int((iTemp34 & 65535));
			fRec84[0] = ((fVec3[(IOTA-int((1 + iTemp52)))&131071] + ((fVec3[(IOTA-iTemp52)&131071] * fTemp36) / fTemp35)) + ((0 - (fTemp36 / fTemp35)) * fRec84[1]));
			fRec80[0] = fRec84[0];
			fRec95[0] = (fSlow38 + (0.999f * fRec95[1]));
			int iTemp53 = int((fRec95[0] - 1.49999f));
			float fTemp54 = (fRec95[0] - iTemp53);
			float fTemp55 = ((2.0f + iTemp53) - fRec95[0]);
			float fTemp56 = ((0.7071067811865476f * fTemp49) + (0.7071067811865475f * fTemp50));
			fVec4[IOTA&131071] = fTemp56;
			int iTemp57 = int((iTemp53 & 65535));
			fRec94[0] = ((fVec4[(IOTA-int((1 + iTemp57)))&131071] + ((fVec4[(IOTA-iTemp57)&131071] * fTemp55) / fTemp54)) + ((0 - (fTemp55 / fTemp54)) * fRec94[1]));
			fRec81[0] = fRec94[0];
			float 	fRec82 = (0.707f * fTemp50);
			float 	fRec83 = (0.707f * fTemp49);
			float fTemp58 = ((fRec83 + fRec81[1]) - (0.707f * fRec75[1]));
			float fTemp59 = ((fRec82 + fRec80[1]) - (0.707f * fRec74[1]));
			float fTemp60 = ((0.7071067811865476f * fTemp59) - (0.7071067811865475f * fTemp58));
			fVec5[IOTA&131071] = fTemp60;
			int iTemp61 = int((iTemp31 & 65535));
			fRec78[0] = ((fVec5[(IOTA-int((1 + iTemp61)))&131071] + ((fVec5[(IOTA-iTemp61)&131071] * fTemp33) / fTemp32)) + ((0 - (fTemp33 / fTemp32)) * fRec78[1]));
			fRec74[0] = fRec78[0];
			fRec97[0] = (fSlow39 + (0.999f * fRec97[1]));
			int iTemp62 = int((fRec97[0] - 1.49999f));
			float fTemp63 = (fRec97[0] - iTemp62);
			float fTemp64 = ((2.0f + iTemp62) - fRec97[0]);
			float fTemp65 = ((0.7071067811865476f * fTemp58) + (0.7071067811865475f * fTemp59));
			fVec6[IOTA&131071] = fTemp65;
			int iTemp66 = int((iTemp62 & 65535));
			fRec96[0] = ((fVec6[(IOTA-int((1 + iTemp66)))&131071] + ((fVec6[(IOTA-iTemp66)&131071] * fTemp64) / fTemp63)) + ((0 - (fTemp64 / fTemp63)) * fRec96[1]));
			fRec75[0] = fRec96[0];
			float 	fRec76 = (0.707f * fTemp59);
			float 	fRec77 = (0.707f * fTemp58);
			float fTemp67 = ((fRec77 + fRec75[1]) - (0.707f * fRec69[1]));
			float fTemp68 = ((fRec76 + fRec74[1]) - (0.707f * fRec68[1]));
			float fTemp69 = ((0.7071067811865476f * fTemp68) - (0.7071067811865475f * fTemp67));
			fVec7[IOTA&131071] = fTemp69;
			int iTemp70 = int((iTemp28 & 65535));
			fRec72[0] = ((fVec7[(IOTA-int((1 + iTemp70)))&131071] + ((fVec7[(IOTA-iTemp70)&131071] * fTemp30) / fTemp29)) + ((0 - (fTemp30 / fTemp29)) * fRec72[1]));
			fRec68[0] = fRec72[0];
			fRec99[0] = (fSlow40 + (0.999f * fRec99[1]));
			int iTemp71 = int((fRec99[0] - 1.49999f));
			float fTemp72 = (fRec99[0] - iTemp71);
			float fTemp73 = ((2.0f + iTemp71) - fRec99[0]);
			float fTemp74 = ((0.7071067811865476f * fTemp67) + (0.7071067811865475f * fTemp68));
			fVec8[IOTA&131071] = fTemp74;
			int iTemp75 = int((iTemp71 & 65535));
			fRec98[0] = ((fVec8[(IOTA-int((1 + iTemp75)))&131071] + ((fVec8[(IOTA-iTemp75)&131071] * fTemp73) / fTemp72)) + ((0 - (fTemp73 / fTemp72)) * fRec98[1]));
			fRec69[0] = fRec98[0];
			float 	fRec70 = (0.707f * fTemp68);
			float 	fRec71 = (0.707f * fTemp67);
			float fTemp76 = ((fRec71 + fRec69[1]) - (0.707f * fRec63[1]));
			float fTemp77 = ((fRec70 + fRec68[1]) - (0.707f * fRec62[1]));
			float fTemp78 = ((0.7071067811865476f * fTemp77) - (0.7071067811865475f * fTemp76));
			fVec9[IOTA&131071] = fTemp78;
			int iTemp79 = int((iTemp25 & 65535));
			fRec66[0] = ((fVec9[(IOTA-int((1 + iTemp79)))&131071] + ((fVec9[(IOTA-iTemp79)&131071] * fTemp27) / fTemp26)) + ((0 - (fTemp27 / fTemp26)) * fRec66[1]));
			fRec62[0] = fRec66[0];
			fRec101[0] = (fSlow41 + (0.999f * fRec101[1]));
			int iTemp80 = int((fRec101[0] - 1.49999f));
			float fTemp81 = (fRec101[0] - iTemp80);
			float fTemp82 = ((2.0f + iTemp80) - fRec101[0]);
			float fTemp83 = ((0.7071067811865476f * fTemp76) + (0.7071067811865475f * fTemp77));
			fVec10[IOTA&131071] = fTemp83;
			int iTemp84 = int((iTemp80 & 65535));
			fRec100[0] = ((fVec10[(IOTA-int((1 + iTemp84)))&131071] + ((fVec10[(IOTA-iTemp84)&131071] * fTemp82) / fTemp81)) + ((0 - (fTemp82 / fTemp81)) * fRec100[1]));
			fRec63[0] = fRec100[0];
			float 	fRec64 = (0.707f * fTemp77);
			float 	fRec65 = (0.707f * fTemp76);
			float fTemp85 = (fRec65 + fRec63[1]);
			fVec11[IOTA&65535] = fTemp85;
			fRec102[0] = ((fSlow44 * fRec102[1]) + (fSlow43 * fRec103[1]));
			fRec103[0] = ((1 + ((fSlow45 * fRec102[1]) + (fSlow44 * fRec103[1]))) - iVec0[1]);
			fRec104[0] = (fSlow48 + (0.999f * fRec104[1]));
			float fTemp86 = (fRec104[0] + (fSlow47 * fRec103[0]));
			int iTemp87 = int((fSlow50 + fTemp86));
			float fTemp88 = ((fSlow51 + fTemp86) - iTemp87);
			float fTemp89 = ((fSlow52 + fTemp86) - iTemp87);
			float fTemp90 = ((fTemp86 + fSlow53) - iTemp87);
			float fTemp91 = ((fSlow54 + fTemp86) - iTemp87);
			float fTemp92 = ((fSlow55 + fTemp86) - iTemp87);
			float fTemp93 = ((fTemp89 * fTemp92) * fTemp88);
			float fTemp94 = (((0.041666666666666664f * ((fTemp93 * fTemp91) * fVec11[(IOTA-int((int((4 + iTemp87)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp89 * fTemp88) * fTemp91) * fTemp90)) * fVec11[(IOTA-int((int((3 + iTemp87)) & 65535)))&65535]) + ((0 - (fTemp93 * fTemp90)) * fVec11[(IOTA-int((int((1 + iTemp87)) & 65535)))&65535])) + (6.0f * (((fTemp92 * fTemp91) * fTemp90) * ((0.25f * (fTemp89 * fVec11[(IOTA-int((int((2 + iTemp87)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp88 * fVec11[(IOTA-int((iTemp87 & 65535)))&65535])))))))) - (0.707f * fRec57[1]));
			float fTemp95 = (fRec64 + fRec62[1]);
			fVec12[IOTA&65535] = fTemp95;
			float fTemp96 = (fSlow49 * (1 + fRec103[0]));
			fRec105[0] = (fSlow56 + (0.999f * fRec105[1]));
			float fTemp97 = (fRec105[0] + fTemp96);
			int iTemp98 = int((fTemp97 - 2.49999f));
			float fTemp99 = (fTemp97 - (3.0f + iTemp98));
			float fTemp100 = (fTemp97 - (1.0f + iTemp98));
			float fTemp101 = (fTemp97 - (5.0f + iTemp98));
			float fTemp102 = (fTemp97 - (2.0f + iTemp98));
			float fTemp103 = (fTemp97 - (4.0f + iTemp98));
			float fTemp104 = ((fTemp100 * fTemp103) * fTemp99);
			float fTemp105 = (((0.041666666666666664f * ((fTemp104 * fTemp102) * fVec12[(IOTA-int((int((4 + iTemp98)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp100 * fTemp99) * fTemp102) * fTemp101)) * fVec12[(IOTA-int((int((3 + iTemp98)) & 65535)))&65535]) + ((0 - (fTemp104 * fTemp101)) * fVec12[(IOTA-int((int((1 + iTemp98)) & 65535)))&65535])) + (6.0f * (((fTemp103 * fTemp102) * fTemp101) * ((0.25f * (fTemp100 * fVec12[(IOTA-int((int((2 + iTemp98)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp99 * fVec12[(IOTA-int((iTemp98 & 65535)))&65535])))))))) - (0.707f * fRec56[1]));
			float fTemp106 = ((0.7071067811865476f * fTemp105) - (0.7071067811865475f * fTemp94));
			fVec13[IOTA&131071] = fTemp106;
			int iTemp107 = int((iTemp22 & 65535));
			fRec60[0] = ((fVec13[(IOTA-int((1 + iTemp107)))&131071] + ((fVec13[(IOTA-iTemp107)&131071] * fTemp24) / fTemp23)) + ((0 - (fTemp24 / fTemp23)) * fRec60[1]));
			fRec56[0] = fRec60[0];
			fRec107[0] = (fSlow57 + (0.999f * fRec107[1]));
			int iTemp108 = int((fRec107[0] - 1.49999f));
			float fTemp109 = (fRec107[0] - iTemp108);
			float fTemp110 = ((2.0f + iTemp108) - fRec107[0]);
			float fTemp111 = ((0.7071067811865476f * fTemp94) + (0.7071067811865475f * fTemp105));
			fVec14[IOTA&131071] = fTemp111;
			int iTemp112 = int((iTemp108 & 65535));
			fRec106[0] = ((fVec14[(IOTA-int((1 + iTemp112)))&131071] + ((fVec14[(IOTA-iTemp112)&131071] * fTemp110) / fTemp109)) + ((0 - (fTemp110 / fTemp109)) * fRec106[1]));
			fRec57[0] = fRec106[0];
			float 	fRec58 = (0.707f * fTemp105);
			float 	fRec59 = (0.707f * fTemp94);
			float fTemp113 = ((fRec59 + fRec57[1]) - (0.707f * fRec51[1]));
			float fTemp114 = ((fRec58 + fRec56[1]) - (0.707f * fRec50[1]));
			float fTemp115 = ((0.7071067811865476f * fTemp114) - (0.7071067811865475f * fTemp113));
			fVec15[IOTA&131071] = fTemp115;
			int iTemp116 = int((iTemp19 & 65535));
			fRec54[0] = ((fVec15[(IOTA-int((1 + iTemp116)))&131071] + ((fVec15[(IOTA-iTemp116)&131071] * fTemp21) / fTemp20)) + ((0 - (fTemp21 / fTemp20)) * fRec54[1]));
			fRec50[0] = fRec54[0];
			fRec109[0] = (fSlow58 + (0.999f * fRec109[1]));
			int iTemp117 = int((fRec109[0] - 1.49999f));
			float fTemp118 = (fRec109[0] - iTemp117);
			float fTemp119 = ((2.0f + iTemp117) - fRec109[0]);
			float fTemp120 = ((0.7071067811865476f * fTemp113) + (0.7071067811865475f * fTemp114));
			fVec16[IOTA&131071] = fTemp120;
			int iTemp121 = int((iTemp117 & 65535));
			fRec108[0] = ((fVec16[(IOTA-int((1 + iTemp121)))&131071] + ((fVec16[(IOTA-iTemp121)&131071] * fTemp119) / fTemp118)) + ((0 - (fTemp119 / fTemp118)) * fRec108[1]));
			fRec51[0] = fRec108[0];
			float 	fRec52 = (0.707f * fTemp114);
			float 	fRec53 = (0.707f * fTemp113);
			float fTemp122 = ((fRec53 + fRec51[1]) - (0.707f * fRec45[1]));
			float fTemp123 = ((fRec52 + fRec50[1]) - (0.707f * fRec44[1]));
			float fTemp124 = ((0.7071067811865476f * fTemp123) - (0.7071067811865475f * fTemp122));
			fVec17[IOTA&131071] = fTemp124;
			int iTemp125 = int((iTemp16 & 65535));
			fRec48[0] = ((fVec17[(IOTA-int((1 + iTemp125)))&131071] + ((fVec17[(IOTA-iTemp125)&131071] * fTemp18) / fTemp17)) + ((0 - (fTemp18 / fTemp17)) * fRec48[1]));
			fRec44[0] = fRec48[0];
			fRec111[0] = (fSlow59 + (0.999f * fRec111[1]));
			int iTemp126 = int((fRec111[0] - 1.49999f));
			float fTemp127 = (fRec111[0] - iTemp126);
			float fTemp128 = ((2.0f + iTemp126) - fRec111[0]);
			float fTemp129 = (0 - (fTemp128 / fTemp127));
			float fTemp130 = ((0.7071067811865476f * fTemp122) + (0.7071067811865475f * fTemp123));
			fVec18[IOTA&131071] = fTemp130;
			int iTemp131 = int((iTemp126 & 65535));
			int iTemp132 = int((1 + iTemp131));
			fRec110[0] = ((fVec18[(IOTA-iTemp132)&131071] + ((fVec18[(IOTA-iTemp131)&131071] * fTemp128) / fTemp127)) + (fTemp129 * fRec110[1]));
			fRec45[0] = fRec110[0];
			float 	fRec46 = (0.707f * fTemp123);
			float 	fRec47 = (0.707f * fTemp122);
			float fTemp133 = ((fRec47 + fRec45[1]) - (0.707f * fRec40[1]));
			float fTemp134 = ((fRec46 + fRec44[1]) - (0.707f * fRec39[1]));
			float fTemp135 = ((0.7071067811865476f * fTemp134) - (0.7071067811865475f * fTemp133));
			fVec19[IOTA&131071] = fTemp135;
			int iTemp136 = int((iTemp6 & 65535));
			int iTemp137 = int((1 + iTemp136));
			fRec43[0] = ((fVec19[(IOTA-iTemp137)&131071] + ((fVec19[(IOTA-iTemp136)&131071] * fTemp8) / fTemp7)) + (fTemp9 * fRec43[1]));
			fRec39[0] = fRec43[0];
			fRec113[0] = (fSlow60 + (0.999f * fRec113[1]));
			int iTemp138 = int((fRec113[0] - 1.49999f));
			float fTemp139 = (fRec113[0] - iTemp138);
			float fTemp140 = ((2.0f + iTemp138) - fRec113[0]);
			float fTemp141 = ((0.7071067811865476f * fTemp133) + (0.7071067811865475f * fTemp134));
			fVec20[IOTA&131071] = fTemp141;
			int iTemp142 = int((iTemp138 & 65535));
			fRec112[0] = ((fVec20[(IOTA-int((1 + iTemp142)))&131071] + ((fVec20[(IOTA-iTemp142)&131071] * fTemp140) / fTemp139)) + ((0 - (fTemp140 / fTemp139)) * fRec112[1]));
			fRec40[0] = fRec112[0];
			float 	fRec41 = (0.707f * fTemp134);
			float 	fRec42 = (0.707f * fTemp133);
			float fTemp143 = ((fRec42 + fRec40[1]) - (0.707f * fRec34[1]));
			float fTemp144 = ((fRec41 + fRec39[1]) - (0.707f * fRec33[1]));
			float fTemp145 = ((0.7071067811865476f * fTemp144) - (0.7071067811865475f * fTemp143));
			fVec21[IOTA&131071] = fTemp145;
			int iTemp146 = int((iTemp13 & 65535));
			fRec37[0] = ((fVec21[(IOTA-int((1 + iTemp146)))&131071] + ((fVec21[(IOTA-iTemp146)&131071] * fTemp15) / fTemp14)) + ((0 - (fTemp15 / fTemp14)) * fRec37[1]));
			fRec33[0] = fRec37[0];
			fRec115[0] = (fSlow61 + (0.999f * fRec115[1]));
			int iTemp147 = int((fRec115[0] - 1.49999f));
			float fTemp148 = (fRec115[0] - iTemp147);
			float fTemp149 = ((2.0f + iTemp147) - fRec115[0]);
			float fTemp150 = (0 - (fTemp149 / fTemp148));
			float fTemp151 = ((0.7071067811865476f * fTemp143) + (0.7071067811865475f * fTemp144));
			fVec22[IOTA&131071] = fTemp151;
			int iTemp152 = int((iTemp147 & 65535));
			int iTemp153 = int((1 + iTemp152));
			fRec114[0] = ((fVec22[(IOTA-iTemp153)&131071] + ((fVec22[(IOTA-iTemp152)&131071] * fTemp149) / fTemp148)) + (fTemp150 * fRec114[1]));
			fRec34[0] = fRec114[0];
			float 	fRec35 = (0.707f * fTemp144);
			float 	fRec36 = (0.707f * fTemp143);
			float fTemp154 = (fRec36 + fRec34[1]);
			fVec23[IOTA&65535] = fTemp154;
			fRec116[0] = (fSlow62 + (0.999f * fRec116[1]));
			float fTemp155 = (fRec116[0] + (fSlow47 * fRec102[0]));
			int iTemp156 = int((fSlow50 + fTemp155));
			float fTemp157 = ((fTemp155 + fSlow51) - iTemp156);
			float fTemp158 = ((fTemp155 + fSlow52) - iTemp156);
			float fTemp159 = ((fTemp155 + fSlow53) - iTemp156);
			float fTemp160 = ((fTemp155 + fSlow54) - iTemp156);
			float fTemp161 = ((fTemp155 + fSlow55) - iTemp156);
			float fTemp162 = ((fTemp158 * fTemp161) * fTemp157);
			float fTemp163 = ((0.041666666666666664f * ((fTemp162 * fTemp160) * fVec23[(IOTA-int((int((4 + iTemp156)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp158 * fTemp157) * fTemp160) * fTemp159)) * fVec23[(IOTA-int((int((3 + iTemp156)) & 65535)))&65535]) + ((0 - (fTemp162 * fTemp159)) * fVec23[(IOTA-int((int((1 + iTemp156)) & 65535)))&65535])) + (6.0f * (((fTemp161 * fTemp160) * fTemp159) * ((0.25f * (fTemp158 * fVec23[(IOTA-int((int((2 + iTemp156)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp157 * fVec23[(IOTA-int((iTemp156 & 65535)))&65535]))))))));
			fVec24[0] = fTemp163;
			fRec32[0] = ((fSlow63 * (fVec24[1] + fVec24[0])) + (fSlow27 * fRec32[1]));
			fRec31[0] = (fRec32[0] - (fSlow25 * ((fSlow24 * fRec31[2]) + (fSlow19 * fRec31[1]))));
			fRec30[0] = ((fSlow25 * (fRec31[2] + (fRec31[0] + (2 * fRec31[1])))) - (fSlow23 * ((fSlow21 * fRec30[2]) + (fSlow19 * fRec30[1]))));
			float fTemp164 = (fRec30[2] + (fRec30[0] + (2 * fRec30[1])));
			float fTemp165 = (fSlow23 * fTemp164);
			fVec25[0] = fTemp165;
			fRec29[0] = ((fSlow68 * fRec29[1]) + (fSlow67 * ((fSlow65 * fVec25[1]) + (fSlow64 * fTemp164))));
			fRec28[0] = (fRec29[0] - (fSlow16 * ((fSlow15 * fRec28[2]) + (fSlow11 * fRec28[1]))));
			fRec27[0] = ((fSlow16 * (((fSlow10 * fRec28[0]) + (fSlow69 * fRec28[1])) + (fSlow10 * fRec28[2]))) - (fSlow14 * ((fSlow13 * fRec27[2]) + (fSlow11 * fRec27[1]))));
			fRec119[0] = ((fSlow67 * (fVec25[1] + fVec25[0])) + (fSlow68 * fRec119[1]));
			fRec118[0] = (fRec119[0] - (fSlow16 * ((fSlow15 * fRec118[2]) + (fSlow11 * fRec118[1]))));
			fRec117[0] = ((fSlow16 * (fRec118[2] + (fRec118[0] + (2 * fRec118[1])))) - (fSlow14 * ((fSlow13 * fRec117[2]) + (fSlow11 * fRec117[1]))));
			float fTemp166 = (fSlow11 * fRec120[1]);
			fRec123[0] = ((fSlow63 * ((fSlow74 * fVec24[1]) + (fSlow20 * fVec24[0]))) + (fSlow27 * fRec123[1]));
			fRec122[0] = (fRec123[0] - (fSlow25 * ((fSlow24 * fRec122[2]) + (fSlow19 * fRec122[1]))));
			fRec121[0] = ((fSlow25 * (((fSlow18 * fRec122[0]) + (fSlow75 * fRec122[1])) + (fSlow18 * fRec122[2]))) - (fSlow23 * ((fSlow21 * fRec121[2]) + (fSlow19 * fRec121[1]))));
			fRec120[0] = ((fSlow23 * (((fSlow18 * fRec121[0]) + (fSlow75 * fRec121[1])) + (fSlow18 * fRec121[2]))) - (fSlow73 * ((fSlow72 * fRec120[2]) + fTemp166)));
			float fTemp167 = ((float)input1[i] + (fSlow77 * ((fSlow76 * (fRec120[2] + (fSlow73 * (fTemp166 + (fSlow72 * fRec120[0]))))) + (fSlow14 * ((fSlow71 * (fRec117[2] + (fRec117[0] + (2 * fRec117[1])))) + (fSlow70 * (((fSlow10 * fRec27[0]) + (fSlow69 * fRec27[1])) + (fSlow10 * fRec27[2]))))))));
			fVec26[IOTA&65535] = fTemp167;
			int iTemp168 = int((fTemp96 - 0.49998999999999993f));
			float fTemp169 = (fTemp96 - (1.0f + iTemp168));
			float fTemp170 = ((1.0f + fTemp96) - iTemp168);
			float fTemp171 = (fTemp96 - (3.0f + iTemp168));
			float fTemp172 = (fTemp96 - iTemp168);
			float fTemp173 = (fTemp96 - (2.0f + iTemp168));
			float fTemp174 = ((fTemp170 * fTemp173) * fTemp169);
			fRec26[0] = ((fSlow78 * ((0.041666666666666664f * ((fTemp174 * fTemp172) * fVec26[(IOTA-int((int((4 + iTemp168)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp170 * fTemp169) * fTemp172) * fTemp171)) * fVec26[(IOTA-int((int((3 + iTemp168)) & 65535)))&65535]) + ((0 - (fTemp174 * fTemp171)) * fVec26[(IOTA-int((int((1 + iTemp168)) & 65535)))&65535])) + (6.0f * (((fTemp173 * fTemp172) * fTemp171) * ((0.25f * (fTemp170 * fVec26[(IOTA-int((int((2 + iTemp168)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp169 * fVec26[(IOTA-int((iTemp168 & 65535)))&65535]))))))))) + (fSlow8 * fRec26[1]));
			float fTemp175 = (fRec26[0] - (fSlow2 * fRec21[1]));
			float fTemp176 = (fRec35 + fRec33[1]);
			fVec27[IOTA&65535] = fTemp176;
			float fTemp177 = (fSlow49 * (1 + fRec102[0]));
			fRec131[0] = (fSlow79 + (0.999f * fRec131[1]));
			float fTemp178 = (fRec131[0] + fTemp177);
			int iTemp179 = int((fTemp178 - 2.49999f));
			float fTemp180 = (fTemp178 - (3.0f + iTemp179));
			float fTemp181 = (fTemp178 - (1.0f + iTemp179));
			float fTemp182 = (fTemp178 - (5.0f + iTemp179));
			float fTemp183 = (fTemp178 - (2.0f + iTemp179));
			float fTemp184 = (fTemp178 - (4.0f + iTemp179));
			float fTemp185 = ((fTemp181 * fTemp184) * fTemp180);
			float fTemp186 = ((0.041666666666666664f * ((fTemp185 * fTemp183) * fVec27[(IOTA-int((int((4 + iTemp179)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp181 * fTemp180) * fTemp183) * fTemp182)) * fVec27[(IOTA-int((int((3 + iTemp179)) & 65535)))&65535]) + ((0 - (fTemp185 * fTemp182)) * fVec27[(IOTA-int((int((1 + iTemp179)) & 65535)))&65535])) + (6.0f * (((fTemp184 * fTemp183) * fTemp182) * ((0.25f * (fTemp181 * fVec27[(IOTA-int((int((2 + iTemp179)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp180 * fVec27[(IOTA-int((iTemp179 & 65535)))&65535]))))))));
			fVec28[0] = fTemp186;
			fRec130[0] = ((fSlow27 * fRec130[1]) + (fSlow63 * (fVec28[1] + fVec28[0])));
			fRec129[0] = (fRec130[0] - (fSlow25 * ((fSlow24 * fRec129[2]) + (fSlow19 * fRec129[1]))));
			fRec128[0] = ((fSlow25 * (fRec129[2] + (fRec129[0] + (2 * fRec129[1])))) - (fSlow23 * ((fSlow21 * fRec128[2]) + (fSlow19 * fRec128[1]))));
			float fTemp187 = (fRec128[2] + (fRec128[0] + (2 * fRec128[1])));
			float fTemp188 = (fSlow23 * fTemp187);
			fVec29[0] = fTemp188;
			fRec127[0] = ((fSlow67 * (fVec29[1] + fVec29[0])) + (fSlow68 * fRec127[1]));
			fRec126[0] = (fRec127[0] - (fSlow16 * ((fSlow15 * fRec126[2]) + (fSlow11 * fRec126[1]))));
			fRec125[0] = ((fSlow16 * (fRec126[2] + (fRec126[0] + (2 * fRec126[1])))) - (fSlow14 * ((fSlow13 * fRec125[2]) + (fSlow11 * fRec125[1]))));
			fRec134[0] = ((fSlow68 * fRec134[1]) + (fSlow67 * ((fSlow65 * fVec29[1]) + (fSlow64 * fTemp187))));
			fRec133[0] = (fRec134[0] - (fSlow16 * ((fSlow15 * fRec133[2]) + (fSlow11 * fRec133[1]))));
			fRec132[0] = ((fSlow16 * (((fSlow10 * fRec133[0]) + (fSlow69 * fRec133[1])) + (fSlow10 * fRec133[2]))) - (fSlow14 * ((fSlow13 * fRec132[2]) + (fSlow11 * fRec132[1]))));
			float fTemp189 = (fSlow11 * fRec135[1]);
			fRec138[0] = ((fSlow63 * ((fSlow74 * fVec28[1]) + (fSlow20 * fVec28[0]))) + (fSlow27 * fRec138[1]));
			fRec137[0] = (fRec138[0] - (fSlow25 * ((fSlow24 * fRec137[2]) + (fSlow19 * fRec137[1]))));
			fRec136[0] = ((fSlow25 * (((fSlow18 * fRec137[0]) + (fSlow75 * fRec137[1])) + (fSlow18 * fRec137[2]))) - (fSlow23 * ((fSlow21 * fRec136[2]) + (fSlow19 * fRec136[1]))));
			fRec135[0] = ((fSlow23 * (((fSlow18 * fRec136[0]) + (fSlow75 * fRec136[1])) + (fSlow18 * fRec136[2]))) - (fSlow73 * ((fSlow72 * fRec135[2]) + fTemp189)));
			float fTemp190 = ((float)input0[i] + (fSlow77 * ((fSlow76 * (fRec135[2] + (fSlow73 * (fTemp189 + (fSlow72 * fRec135[0]))))) + (fSlow14 * ((fSlow70 * (((fSlow10 * fRec132[0]) + (fSlow69 * fRec132[1])) + (fSlow10 * fRec132[2]))) + (fSlow71 * (fRec125[2] + (fRec125[0] + (2 * fRec125[1])))))))));
			fVec30[IOTA&65535] = fTemp190;
			int iTemp191 = int((fTemp177 - 0.49998999999999993f));
			float fTemp192 = (fTemp177 - (1.0f + iTemp191));
			float fTemp193 = ((1.0f + fTemp177) - iTemp191);
			float fTemp194 = (fTemp177 - (3.0f + iTemp191));
			float fTemp195 = (fTemp177 - iTemp191);
			float fTemp196 = (fTemp177 - (2.0f + iTemp191));
			float fTemp197 = ((fTemp193 * fTemp196) * fTemp192);
			fRec124[0] = ((fSlow78 * ((0.041666666666666664f * ((fTemp197 * fTemp195) * fVec30[(IOTA-int((int((4 + iTemp191)) & 65535)))&65535])) + (0.16666666666666666f * ((((0 - (((fTemp193 * fTemp192) * fTemp195) * fTemp194)) * fVec30[(IOTA-int((int((3 + iTemp191)) & 65535)))&65535]) + ((0 - (fTemp197 * fTemp194)) * fVec30[(IOTA-int((int((1 + iTemp191)) & 65535)))&65535])) + (6.0f * (((fTemp196 * fTemp195) * fTemp194) * ((0.25f * (fTemp193 * fVec30[(IOTA-int((int((2 + iTemp191)) & 65535)))&65535])) + (0.041666666666666664f * (fTemp192 * fVec30[(IOTA-int((iTemp191 & 65535)))&65535]))))))))) + (fSlow8 * fRec124[1]));
			float fTemp198 = (fRec124[0] - (fSlow2 * fRec20[1]));
			float fTemp199 = ((0.7071067811865476f * fTemp198) - (0.7071067811865475f * fTemp175));
			fVec31[IOTA&131071] = fTemp199;
			int iTemp200 = int((iTemp10 & 65535));
			fRec24[0] = ((fVec31[(IOTA-int((1 + iTemp200)))&131071] + ((fVec31[(IOTA-iTemp200)&131071] * fTemp12) / fTemp11)) + ((0 - (fTemp12 / fTemp11)) * fRec24[1]));
			fRec20[0] = fRec24[0];
			fRec140[0] = (fSlow80 + (0.999f * fRec140[1]));
			int iTemp201 = int((fRec140[0] - 1.49999f));
			float fTemp202 = (fRec140[0] - iTemp201);
			float fTemp203 = ((2.0f + iTemp201) - fRec140[0]);
			float fTemp204 = ((0.7071067811865476f * fTemp175) + (0.7071067811865475f * fTemp198));
			fVec32[IOTA&131071] = fTemp204;
			int iTemp205 = int((iTemp201 & 65535));
			fRec139[0] = ((fVec32[(IOTA-int((1 + iTemp205)))&131071] + ((fVec32[(IOTA-iTemp205)&131071] * fTemp203) / fTemp202)) + ((0 - (fTemp203 / fTemp202)) * fRec139[1]));
			fRec21[0] = fRec139[0];
			float 	fRec22 = (fSlow2 * fTemp198);
			float 	fRec23 = (fSlow2 * fTemp175);
			float fTemp206 = ((fRec23 + fRec21[1]) - (fSlow2 * fRec15[1]));
			float fTemp207 = ((fRec22 + fRec20[1]) - (fSlow2 * fRec14[1]));
			float fTemp208 = ((0.7071067811865476f * fTemp207) - (0.7071067811865475f * fTemp206));
			fVec33[IOTA&131071] = fTemp208;
			fRec18[0] = ((fVec33[(IOTA-iTemp137)&131071] + ((fTemp8 * fVec33[(IOTA-iTemp136)&131071]) / fTemp7)) + (fTemp9 * fRec18[1]));
			fRec14[0] = fRec18[0];
			float fTemp209 = ((0.7071067811865476f * fTemp206) + (0.7071067811865475f * fTemp207));
			fVec34[IOTA&131071] = fTemp209;
			fRec141[0] = ((fVec34[(IOTA-iTemp132)&131071] + ((fTemp128 * fVec34[(IOTA-iTemp131)&131071]) / fTemp127)) + (fTemp129 * fRec141[1]));
			fRec15[0] = fRec141[0];
			float 	fRec16 = (fSlow2 * fTemp207);
			float 	fRec17 = (fSlow2 * fTemp206);
			float fTemp210 = ((fRec17 + fRec15[1]) - (fSlow2 * fRec9[1]));
			float fTemp211 = ((fRec16 + fRec14[1]) - (fSlow2 * fRec8[1]));
			float fTemp212 = ((0.7071067811865476f * fTemp211) - (0.7071067811865475f * fTemp210));
			fVec35[IOTA&131071] = fTemp212;
			int iTemp213 = int((iTemp3 & 65535));
			fRec12[0] = ((fVec35[(IOTA-int((1 + iTemp213)))&131071] + ((fVec35[(IOTA-iTemp213)&131071] * fTemp5) / fTemp4)) + ((0 - (fTemp5 / fTemp4)) * fRec12[1]));
			fRec8[0] = fRec12[0];
			fRec143[0] = (fSlow81 + (0.999f * fRec143[1]));
			int iTemp214 = int((fRec143[0] - 1.49999f));
			float fTemp215 = (fRec143[0] - iTemp214);
			float fTemp216 = ((2.0f + iTemp214) - fRec143[0]);
			float fTemp217 = ((0.7071067811865476f * fTemp210) + (0.7071067811865475f * fTemp211));
			fVec36[IOTA&131071] = fTemp217;
			int iTemp218 = int((iTemp214 & 65535));
			fRec142[0] = ((fVec36[(IOTA-int((1 + iTemp218)))&131071] + ((fVec36[(IOTA-iTemp218)&131071] * fTemp216) / fTemp215)) + ((0 - (fTemp216 / fTemp215)) * fRec142[1]));
			fRec9[0] = fRec142[0];
			float 	fRec10 = (fSlow2 * fTemp211);
			float 	fRec11 = (fSlow2 * fTemp210);
			float fTemp219 = ((fRec11 + fRec9[1]) - (fSlow2 * fRec3[1]));
			float fTemp220 = ((fRec10 + fRec8[1]) - (fSlow2 * fRec2[1]));
			float fTemp221 = ((0.7071067811865476f * fTemp220) - (0.7071067811865475f * fTemp219));
			fVec37[IOTA&131071] = fTemp221;
			int iTemp222 = int((iTemp0 & 65535));
			fRec6[0] = ((fVec37[(IOTA-int((1 + iTemp222)))&131071] + ((fVec37[(IOTA-iTemp222)&131071] * fTemp2) / fTemp1)) + ((0 - (fTemp2 / fTemp1)) * fRec6[1]));
			fRec2[0] = fRec6[0];
			float fTemp223 = ((0.7071067811865476f * fTemp219) + (0.7071067811865475f * fTemp220));
			fVec38[IOTA&131071] = fTemp223;
			fRec144[0] = ((fVec38[(IOTA-iTemp153)&131071] + ((fTemp149 * fVec38[(IOTA-iTemp152)&131071]) / fTemp148)) + (fTemp150 * fRec144[1]));
			fRec3[0] = fRec144[0];
			float 	fRec4 = (fSlow2 * fTemp220);
			float 	fRec5 = (fSlow2 * fTemp219);
			fRec0[0] = (fRec4 + fRec2[1]);
			fRec1[0] = (fRec5 + fRec3[1]);
			output0[i] = (FAUSTFLOAT)fRec0[0];
			output1[i] = (FAUSTFLOAT)fRec1[0];
			// post processing
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec3[1] = fRec3[0];
			fRec144[1] = fRec144[0];
			fRec2[1] = fRec2[0];
			fRec6[1] = fRec6[0];
			fRec9[1] = fRec9[0];
			fRec142[1] = fRec142[0];
			fRec143[1] = fRec143[0];
			fRec8[1] = fRec8[0];
			fRec12[1] = fRec12[0];
			fRec15[1] = fRec15[0];
			fRec141[1] = fRec141[0];
			fRec14[1] = fRec14[0];
			fRec18[1] = fRec18[0];
			fRec21[1] = fRec21[0];
			fRec139[1] = fRec139[0];
			fRec140[1] = fRec140[0];
			fRec20[1] = fRec20[0];
			fRec24[1] = fRec24[0];
			fRec124[1] = fRec124[0];
			fRec135[2] = fRec135[1]; fRec135[1] = fRec135[0];
			fRec136[2] = fRec136[1]; fRec136[1] = fRec136[0];
			fRec137[2] = fRec137[1]; fRec137[1] = fRec137[0];
			fRec138[1] = fRec138[0];
			fRec132[2] = fRec132[1]; fRec132[1] = fRec132[0];
			fRec133[2] = fRec133[1]; fRec133[1] = fRec133[0];
			fRec134[1] = fRec134[0];
			fRec125[2] = fRec125[1]; fRec125[1] = fRec125[0];
			fRec126[2] = fRec126[1]; fRec126[1] = fRec126[0];
			fRec127[1] = fRec127[0];
			fVec29[1] = fVec29[0];
			fRec128[2] = fRec128[1]; fRec128[1] = fRec128[0];
			fRec129[2] = fRec129[1]; fRec129[1] = fRec129[0];
			fRec130[1] = fRec130[0];
			fVec28[1] = fVec28[0];
			fRec131[1] = fRec131[0];
			fRec26[1] = fRec26[0];
			fRec120[2] = fRec120[1]; fRec120[1] = fRec120[0];
			fRec121[2] = fRec121[1]; fRec121[1] = fRec121[0];
			fRec122[2] = fRec122[1]; fRec122[1] = fRec122[0];
			fRec123[1] = fRec123[0];
			fRec117[2] = fRec117[1]; fRec117[1] = fRec117[0];
			fRec118[2] = fRec118[1]; fRec118[1] = fRec118[0];
			fRec119[1] = fRec119[0];
			fRec27[2] = fRec27[1]; fRec27[1] = fRec27[0];
			fRec28[2] = fRec28[1]; fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fVec25[1] = fVec25[0];
			fRec30[2] = fRec30[1]; fRec30[1] = fRec30[0];
			fRec31[2] = fRec31[1]; fRec31[1] = fRec31[0];
			fRec32[1] = fRec32[0];
			fVec24[1] = fVec24[0];
			fRec116[1] = fRec116[0];
			fRec34[1] = fRec34[0];
			fRec114[1] = fRec114[0];
			fRec115[1] = fRec115[0];
			fRec33[1] = fRec33[0];
			fRec37[1] = fRec37[0];
			fRec40[1] = fRec40[0];
			fRec112[1] = fRec112[0];
			fRec113[1] = fRec113[0];
			fRec39[1] = fRec39[0];
			fRec43[1] = fRec43[0];
			fRec45[1] = fRec45[0];
			fRec110[1] = fRec110[0];
			fRec111[1] = fRec111[0];
			fRec44[1] = fRec44[0];
			fRec48[1] = fRec48[0];
			fRec51[1] = fRec51[0];
			fRec108[1] = fRec108[0];
			fRec109[1] = fRec109[0];
			fRec50[1] = fRec50[0];
			fRec54[1] = fRec54[0];
			fRec57[1] = fRec57[0];
			fRec106[1] = fRec106[0];
			fRec107[1] = fRec107[0];
			fRec56[1] = fRec56[0];
			fRec60[1] = fRec60[0];
			fRec105[1] = fRec105[0];
			fRec104[1] = fRec104[0];
			fRec103[1] = fRec103[0];
			fRec102[1] = fRec102[0];
			fRec63[1] = fRec63[0];
			fRec100[1] = fRec100[0];
			fRec101[1] = fRec101[0];
			fRec62[1] = fRec62[0];
			fRec66[1] = fRec66[0];
			fRec69[1] = fRec69[0];
			fRec98[1] = fRec98[0];
			fRec99[1] = fRec99[0];
			fRec68[1] = fRec68[0];
			fRec72[1] = fRec72[0];
			fRec75[1] = fRec75[0];
			fRec96[1] = fRec96[0];
			fRec97[1] = fRec97[0];
			fRec74[1] = fRec74[0];
			fRec78[1] = fRec78[0];
			fRec81[1] = fRec81[0];
			fRec94[1] = fRec94[0];
			fRec95[1] = fRec95[0];
			fRec80[1] = fRec80[0];
			fRec84[1] = fRec84[0];
			fRec87[1] = fRec87[0];
			fRec92[1] = fRec92[0];
			fRec93[1] = fRec93[0];
			fRec86[1] = fRec86[0];
			fRec90[1] = fRec90[0];
			IOTA = IOTA+1;
			fRec91[1] = fRec91[0];
			fRec85[1] = fRec85[0];
			fRec79[1] = fRec79[0];
			fRec73[1] = fRec73[0];
			fRec67[1] = fRec67[0];
			fRec61[1] = fRec61[0];
			fRec55[1] = fRec55[0];
			fRec49[1] = fRec49[0];
			fRec38[1] = fRec38[0];
			fRec25[1] = fRec25[0];
			fRec19[1] = fRec19[0];
			fRec13[1] = fRec13[0];
			fRec7[1] = fRec7[0];
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
