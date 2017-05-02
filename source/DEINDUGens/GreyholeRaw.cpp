//----------------------------------------------------------
// name: "GreyholeRaw"
// version: "1.0"
// author: "Julian Parker, bug fixes by Till Bovermann"
// license: "GPL2+"
// copyright: "(c) Julian Parker 2013"
//
// Code generated with Faust 0.9.96 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with  */
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
	class SIG0 {
	  private:
		int fSamplingFreq;
		static int 	iWave0[1302];
		int 	idxiWave0;
	  public:
		int getNumInputs() { return 0; }
		int getNumOutputs() { return 1; }
		void init(int samplingFreq) {
			fSamplingFreq = samplingFreq;
			idxiWave0 = 0;
		}
		void fill (int count, int output[]) {
			for (int i=0; i<count; i++) {
				output[i] = iWave0[idxiWave0];
				// post processing
				idxiWave0 = (idxiWave0 + 1) % 1302;
			}
		}
	};


	FAUSTFLOAT 	fslider0;
	float 	fVec0[2];
	int 	iVec1[2];
	FAUSTFLOAT 	fslider1;
	float 	fVec2[2];
	static int 	itbl0[1302];
	FAUSTFLOAT 	fslider2;
	float 	fRec6[2];
	FAUSTFLOAT 	fslider3;
	float 	fVec3[2];
	float 	fConst0;
	FAUSTFLOAT 	fslider4;
	float 	fRec7[2];
	float 	fRec8[2];
	float 	fRec9[2];
	float 	fRec10[2];
	float 	fConst1;
	FAUSTFLOAT 	fslider5;
	float 	fVec4[2];
	float 	fConst2;
	FAUSTFLOAT 	fslider6;
	float 	fVec5[2];
	float 	fRec11[2];
	float 	fRec12[2];
	int 	IOTA;
	float 	fVec6[131072];
	float 	fRec16[2];
	float 	fRec20[2];
	float 	fRec24[2];
	float 	fVec7[131072];
	float 	fVec8[16384];
	float 	fRec23[2];
	float 	fRec21[2];
	float 	fRec26[2];
	float 	fVec9[16384];
	float 	fRec25[2];
	float 	fRec22[2];
	float 	fVec10[16384];
	float 	fRec19[2];
	float 	fRec17[2];
	float 	fRec28[2];
	float 	fVec11[16384];
	float 	fRec27[2];
	float 	fRec18[2];
	float 	fVec12[16384];
	float 	fRec15[2];
	float 	fRec13[2];
	float 	fRec30[2];
	float 	fVec13[16384];
	float 	fRec29[2];
	float 	fRec14[2];
	float 	fVec14[16384];
	float 	fRec5[2];
	float 	fRec3[2];
	float 	fRec32[2];
	float 	fVec15[16384];
	float 	fRec31[2];
	float 	fRec4[2];
	float 	fRec36[2];
	float 	fVec16[16384];
	float 	fRec48[2];
	float 	fRec46[2];
	float 	fVec17[16384];
	float 	fRec49[2];
	float 	fRec47[2];
	float 	fVec18[16384];
	float 	fRec45[2];
	float 	fRec43[2];
	float 	fVec19[16384];
	float 	fRec50[2];
	float 	fRec44[2];
	float 	fVec20[16384];
	float 	fRec42[2];
	float 	fRec40[2];
	float 	fVec21[16384];
	float 	fRec51[2];
	float 	fRec41[2];
	float 	fVec22[16384];
	float 	fRec39[2];
	float 	fRec37[2];
	float 	fVec23[16384];
	float 	fRec52[2];
	float 	fRec38[2];
	float 	fRec56[2];
	float 	fVec24[16384];
	float 	fRec68[2];
	float 	fRec66[2];
	float 	fVec25[16384];
	float 	fRec69[2];
	float 	fRec67[2];
	float 	fVec26[16384];
	float 	fRec65[2];
	float 	fRec63[2];
	float 	fVec27[16384];
	float 	fRec70[2];
	float 	fRec64[2];
	float 	fVec28[16384];
	float 	fRec62[2];
	float 	fRec60[2];
	float 	fVec29[16384];
	float 	fRec71[2];
	float 	fRec61[2];
	float 	fVec30[16384];
	float 	fRec59[2];
	float 	fRec57[2];
	float 	fVec31[16384];
	float 	fRec72[2];
	float 	fRec58[2];
	float 	fRec76[2];
	float 	fVec32[16384];
	float 	fRec88[2];
	float 	fRec86[2];
	float 	fVec33[16384];
	float 	fRec89[2];
	float 	fRec87[2];
	float 	fVec34[16384];
	float 	fRec85[2];
	float 	fRec83[2];
	float 	fVec35[16384];
	float 	fRec90[2];
	float 	fRec84[2];
	float 	fVec36[16384];
	float 	fRec82[2];
	float 	fRec80[2];
	float 	fVec37[16384];
	float 	fRec91[2];
	float 	fRec81[2];
	float 	fVec38[16384];
	float 	fRec79[2];
	float 	fRec77[2];
	float 	fVec39[16384];
	float 	fRec92[2];
	float 	fRec78[2];
	float 	fRec96[2];
	float 	fVec40[16384];
	float 	fRec108[2];
	float 	fRec106[2];
	float 	fVec41[16384];
	float 	fRec109[2];
	float 	fRec107[2];
	float 	fVec42[16384];
	float 	fRec105[2];
	float 	fRec103[2];
	float 	fVec43[16384];
	float 	fRec110[2];
	float 	fRec104[2];
	float 	fVec44[16384];
	float 	fRec102[2];
	float 	fRec100[2];
	float 	fVec45[16384];
	float 	fRec111[2];
	float 	fRec101[2];
	float 	fVec46[16384];
	float 	fRec99[2];
	float 	fRec97[2];
	float 	fVec47[16384];
	float 	fRec112[2];
	float 	fRec98[2];
	float 	fVec48[16384];
	float 	fRec95[2];
	float 	fRec93[2];
	float 	fRec114[2];
	float 	fVec49[16384];
	float 	fRec113[2];
	float 	fRec94[2];
	float 	fVec50[16384];
	float 	fRec75[2];
	float 	fRec73[2];
	float 	fRec116[2];
	float 	fVec51[16384];
	float 	fRec115[2];
	float 	fRec74[2];
	float 	fVec52[16384];
	float 	fRec55[2];
	float 	fRec53[2];
	float 	fRec118[2];
	float 	fVec53[16384];
	float 	fRec117[2];
	float 	fRec54[2];
	float 	fVec54[16384];
	float 	fRec35[2];
	float 	fRec33[2];
	float 	fRec120[2];
	float 	fVec55[16384];
	float 	fRec119[2];
	float 	fRec34[2];
	float 	fRec124[2];
	float 	fVec56[16384];
	float 	fRec148[2];
	float 	fRec146[2];
	float 	fVec57[16384];
	float 	fRec149[2];
	float 	fRec147[2];
	float 	fVec58[16384];
	float 	fRec145[2];
	float 	fRec143[2];
	float 	fVec59[16384];
	float 	fRec150[2];
	float 	fRec144[2];
	float 	fVec60[16384];
	float 	fRec142[2];
	float 	fRec140[2];
	float 	fVec61[16384];
	float 	fRec151[2];
	float 	fRec141[2];
	float 	fVec62[16384];
	float 	fRec139[2];
	float 	fRec137[2];
	float 	fVec63[16384];
	float 	fRec152[2];
	float 	fRec138[2];
	float 	fVec64[16384];
	float 	fRec136[2];
	float 	fRec134[2];
	float 	fVec65[16384];
	float 	fRec153[2];
	float 	fRec135[2];
	float 	fVec66[16384];
	float 	fRec133[2];
	float 	fRec131[2];
	float 	fVec67[16384];
	float 	fRec154[2];
	float 	fRec132[2];
	float 	fVec68[16384];
	float 	fRec130[2];
	float 	fRec128[2];
	float 	fVec69[16384];
	float 	fRec155[2];
	float 	fRec129[2];
	float 	fVec70[16384];
	float 	fRec127[2];
	float 	fRec125[2];
	float 	fVec71[16384];
	float 	fRec156[2];
	float 	fRec126[2];
	float 	fRec163[2];
	float 	fRec167[2];
	float 	fVec72[16384];
	float 	fRec191[2];
	float 	fRec189[2];
	float 	fVec73[16384];
	float 	fRec192[2];
	float 	fRec190[2];
	float 	fVec74[16384];
	float 	fRec188[2];
	float 	fRec186[2];
	float 	fVec75[16384];
	float 	fRec193[2];
	float 	fRec187[2];
	float 	fVec76[16384];
	float 	fRec185[2];
	float 	fRec183[2];
	float 	fVec77[16384];
	float 	fRec194[2];
	float 	fRec184[2];
	float 	fVec78[16384];
	float 	fRec182[2];
	float 	fRec180[2];
	float 	fVec79[16384];
	float 	fRec195[2];
	float 	fRec181[2];
	float 	fVec80[16384];
	float 	fRec179[2];
	float 	fRec177[2];
	float 	fVec81[16384];
	float 	fRec196[2];
	float 	fRec178[2];
	float 	fVec82[16384];
	float 	fRec208[2];
	float 	fRec206[2];
	float 	fVec83[16384];
	float 	fRec209[2];
	float 	fRec207[2];
	float 	fVec84[16384];
	float 	fRec205[2];
	float 	fRec203[2];
	float 	fVec85[16384];
	float 	fRec210[2];
	float 	fRec204[2];
	float 	fVec86[16384];
	float 	fRec202[2];
	float 	fRec200[2];
	float 	fVec87[16384];
	float 	fRec211[2];
	float 	fRec201[2];
	float 	fVec88[16384];
	float 	fRec199[2];
	float 	fRec197[2];
	float 	fVec89[16384];
	float 	fRec212[2];
	float 	fRec198[2];
	float 	fVec90[16384];
	float 	fRec176[2];
	float 	fRec174[2];
	float 	fVec91[16384];
	float 	fRec213[2];
	float 	fRec175[2];
	float 	fVec92[16384];
	float 	fRec225[2];
	float 	fRec223[2];
	float 	fVec93[16384];
	float 	fRec226[2];
	float 	fRec224[2];
	float 	fVec94[16384];
	float 	fRec222[2];
	float 	fRec220[2];
	float 	fVec95[16384];
	float 	fRec227[2];
	float 	fRec221[2];
	float 	fVec96[16384];
	float 	fRec219[2];
	float 	fRec217[2];
	float 	fVec97[16384];
	float 	fRec228[2];
	float 	fRec218[2];
	float 	fVec98[16384];
	float 	fRec216[2];
	float 	fRec214[2];
	float 	fVec99[16384];
	float 	fRec229[2];
	float 	fRec215[2];
	float 	fVec100[16384];
	float 	fRec173[2];
	float 	fRec171[2];
	float 	fVec101[16384];
	float 	fRec230[2];
	float 	fRec172[2];
	float 	fVec102[16384];
	float 	fRec170[2];
	float 	fRec168[2];
	float 	fVec103[16384];
	float 	fRec231[2];
	float 	fRec169[2];
	float 	fVec104[16384];
	float 	fRec166[2];
	float 	fRec164[2];
	float 	fRec233[2];
	float 	fVec105[16384];
	float 	fRec232[2];
	float 	fRec165[2];
	float 	fVec106[16384];
	float 	fRec245[2];
	float 	fRec243[2];
	float 	fVec107[16384];
	float 	fRec246[2];
	float 	fRec244[2];
	float 	fVec108[16384];
	float 	fRec242[2];
	float 	fRec240[2];
	float 	fVec109[16384];
	float 	fRec247[2];
	float 	fRec241[2];
	float 	fVec110[16384];
	float 	fRec239[2];
	float 	fRec237[2];
	float 	fVec111[16384];
	float 	fRec248[2];
	float 	fRec238[2];
	float 	fVec112[16384];
	float 	fRec236[2];
	float 	fRec234[2];
	float 	fVec113[16384];
	float 	fRec249[2];
	float 	fRec235[2];
	float 	fVec114[16384];
	float 	fRec162[2];
	float 	fRec160[2];
	float 	fRec251[2];
	float 	fVec115[16384];
	float 	fRec250[2];
	float 	fRec161[2];
	float 	fVec116[16384];
	float 	fRec263[2];
	float 	fRec261[2];
	float 	fVec117[16384];
	float 	fRec264[2];
	float 	fRec262[2];
	float 	fVec118[16384];
	float 	fRec260[2];
	float 	fRec258[2];
	float 	fVec119[16384];
	float 	fRec265[2];
	float 	fRec259[2];
	float 	fVec120[16384];
	float 	fRec257[2];
	float 	fRec255[2];
	float 	fVec121[16384];
	float 	fRec266[2];
	float 	fRec256[2];
	float 	fVec122[16384];
	float 	fRec254[2];
	float 	fRec252[2];
	float 	fVec123[16384];
	float 	fRec267[2];
	float 	fRec253[2];
	float 	fVec124[16384];
	float 	fRec268[2];
	float 	fRec159[2];
	float 	fRec157[2];
	float 	fVec125[16384];
	float 	fRec270[2];
	float 	fRec269[2];
	float 	fRec158[2];
	float 	fVec126[16384];
	float 	fRec123[2];
	float 	fRec121[2];
	float 	fVec127[16384];
	float 	fRec272[2];
	float 	fRec271[2];
	float 	fRec122[2];
	float 	fRec2[2];
	float 	fRec0[1024];
	float 	fRec273[2];
	float 	fRec1[1024];
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "GreyholeRaw");
		m->declare("version", "1.0");
		m->declare("author", "Julian Parker, bug fixes by Till Bovermann");
		m->declare("license", "GPL2+");
		m->declare("copyright", "(c) Julian Parker 2013");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.0");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.0");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
	}

	virtual int getNumInputs() { return 2; }
	virtual int getNumOutputs() { return 2; }
	static void classInit(int samplingFreq) {
		SIG0 sig0;
		sig0.init(samplingFreq);
		sig0.fill(1302,itbl0);
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(1.92e+05f, max(1e+03f, (float)fSamplingFreq));
		fConst1 = (0.00056689343f * fConst0);
		fConst2 = (3.1415927f / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.0f;
		fslider1 = 0.5f;
		fslider2 = 1.0f;
		fslider3 = 0.9f;
		fslider4 = 0.2f;
		fslider5 = 0.1f;
		fslider6 = 2.0f;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fVec0[i] = 0;
		for (int i=0; i<2; i++) iVec1[i] = 0;
		for (int i=0; i<2; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		IOTA = 0;
		for (int i=0; i<131072; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<131072; i++) fVec7[i] = 0;
		for (int i=0; i<16384; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<16384; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<16384; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<16384; i++) fVec11[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<16384; i++) fVec12[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<16384; i++) fVec13[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<16384; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<16384; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<16384; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<16384; i++) fVec17[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<16384; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<16384; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<16384; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<16384; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<16384; i++) fVec22[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<16384; i++) fVec23[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<16384; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<16384; i++) fVec25[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<16384; i++) fVec26[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<16384; i++) fVec27[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<16384; i++) fVec28[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<16384; i++) fVec29[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<16384; i++) fVec30[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<16384; i++) fVec31[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<16384; i++) fVec32[i] = 0;
		for (int i=0; i<2; i++) fRec88[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<16384; i++) fVec33[i] = 0;
		for (int i=0; i<2; i++) fRec89[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<16384; i++) fVec34[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<16384; i++) fVec35[i] = 0;
		for (int i=0; i<2; i++) fRec90[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<16384; i++) fVec36[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<16384; i++) fVec37[i] = 0;
		for (int i=0; i<2; i++) fRec91[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<16384; i++) fVec38[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<16384; i++) fVec39[i] = 0;
		for (int i=0; i<2; i++) fRec92[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec96[i] = 0;
		for (int i=0; i<16384; i++) fVec40[i] = 0;
		for (int i=0; i<2; i++) fRec108[i] = 0;
		for (int i=0; i<2; i++) fRec106[i] = 0;
		for (int i=0; i<16384; i++) fVec41[i] = 0;
		for (int i=0; i<2; i++) fRec109[i] = 0;
		for (int i=0; i<2; i++) fRec107[i] = 0;
		for (int i=0; i<16384; i++) fVec42[i] = 0;
		for (int i=0; i<2; i++) fRec105[i] = 0;
		for (int i=0; i<2; i++) fRec103[i] = 0;
		for (int i=0; i<16384; i++) fVec43[i] = 0;
		for (int i=0; i<2; i++) fRec110[i] = 0;
		for (int i=0; i<2; i++) fRec104[i] = 0;
		for (int i=0; i<16384; i++) fVec44[i] = 0;
		for (int i=0; i<2; i++) fRec102[i] = 0;
		for (int i=0; i<2; i++) fRec100[i] = 0;
		for (int i=0; i<16384; i++) fVec45[i] = 0;
		for (int i=0; i<2; i++) fRec111[i] = 0;
		for (int i=0; i<2; i++) fRec101[i] = 0;
		for (int i=0; i<16384; i++) fVec46[i] = 0;
		for (int i=0; i<2; i++) fRec99[i] = 0;
		for (int i=0; i<2; i++) fRec97[i] = 0;
		for (int i=0; i<16384; i++) fVec47[i] = 0;
		for (int i=0; i<2; i++) fRec112[i] = 0;
		for (int i=0; i<2; i++) fRec98[i] = 0;
		for (int i=0; i<16384; i++) fVec48[i] = 0;
		for (int i=0; i<2; i++) fRec95[i] = 0;
		for (int i=0; i<2; i++) fRec93[i] = 0;
		for (int i=0; i<2; i++) fRec114[i] = 0;
		for (int i=0; i<16384; i++) fVec49[i] = 0;
		for (int i=0; i<2; i++) fRec113[i] = 0;
		for (int i=0; i<2; i++) fRec94[i] = 0;
		for (int i=0; i<16384; i++) fVec50[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec116[i] = 0;
		for (int i=0; i<16384; i++) fVec51[i] = 0;
		for (int i=0; i<2; i++) fRec115[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<16384; i++) fVec52[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec118[i] = 0;
		for (int i=0; i<16384; i++) fVec53[i] = 0;
		for (int i=0; i<2; i++) fRec117[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<16384; i++) fVec54[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec120[i] = 0;
		for (int i=0; i<16384; i++) fVec55[i] = 0;
		for (int i=0; i<2; i++) fRec119[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec124[i] = 0;
		for (int i=0; i<16384; i++) fVec56[i] = 0;
		for (int i=0; i<2; i++) fRec148[i] = 0;
		for (int i=0; i<2; i++) fRec146[i] = 0;
		for (int i=0; i<16384; i++) fVec57[i] = 0;
		for (int i=0; i<2; i++) fRec149[i] = 0;
		for (int i=0; i<2; i++) fRec147[i] = 0;
		for (int i=0; i<16384; i++) fVec58[i] = 0;
		for (int i=0; i<2; i++) fRec145[i] = 0;
		for (int i=0; i<2; i++) fRec143[i] = 0;
		for (int i=0; i<16384; i++) fVec59[i] = 0;
		for (int i=0; i<2; i++) fRec150[i] = 0;
		for (int i=0; i<2; i++) fRec144[i] = 0;
		for (int i=0; i<16384; i++) fVec60[i] = 0;
		for (int i=0; i<2; i++) fRec142[i] = 0;
		for (int i=0; i<2; i++) fRec140[i] = 0;
		for (int i=0; i<16384; i++) fVec61[i] = 0;
		for (int i=0; i<2; i++) fRec151[i] = 0;
		for (int i=0; i<2; i++) fRec141[i] = 0;
		for (int i=0; i<16384; i++) fVec62[i] = 0;
		for (int i=0; i<2; i++) fRec139[i] = 0;
		for (int i=0; i<2; i++) fRec137[i] = 0;
		for (int i=0; i<16384; i++) fVec63[i] = 0;
		for (int i=0; i<2; i++) fRec152[i] = 0;
		for (int i=0; i<2; i++) fRec138[i] = 0;
		for (int i=0; i<16384; i++) fVec64[i] = 0;
		for (int i=0; i<2; i++) fRec136[i] = 0;
		for (int i=0; i<2; i++) fRec134[i] = 0;
		for (int i=0; i<16384; i++) fVec65[i] = 0;
		for (int i=0; i<2; i++) fRec153[i] = 0;
		for (int i=0; i<2; i++) fRec135[i] = 0;
		for (int i=0; i<16384; i++) fVec66[i] = 0;
		for (int i=0; i<2; i++) fRec133[i] = 0;
		for (int i=0; i<2; i++) fRec131[i] = 0;
		for (int i=0; i<16384; i++) fVec67[i] = 0;
		for (int i=0; i<2; i++) fRec154[i] = 0;
		for (int i=0; i<2; i++) fRec132[i] = 0;
		for (int i=0; i<16384; i++) fVec68[i] = 0;
		for (int i=0; i<2; i++) fRec130[i] = 0;
		for (int i=0; i<2; i++) fRec128[i] = 0;
		for (int i=0; i<16384; i++) fVec69[i] = 0;
		for (int i=0; i<2; i++) fRec155[i] = 0;
		for (int i=0; i<2; i++) fRec129[i] = 0;
		for (int i=0; i<16384; i++) fVec70[i] = 0;
		for (int i=0; i<2; i++) fRec127[i] = 0;
		for (int i=0; i<2; i++) fRec125[i] = 0;
		for (int i=0; i<16384; i++) fVec71[i] = 0;
		for (int i=0; i<2; i++) fRec156[i] = 0;
		for (int i=0; i<2; i++) fRec126[i] = 0;
		for (int i=0; i<2; i++) fRec163[i] = 0;
		for (int i=0; i<2; i++) fRec167[i] = 0;
		for (int i=0; i<16384; i++) fVec72[i] = 0;
		for (int i=0; i<2; i++) fRec191[i] = 0;
		for (int i=0; i<2; i++) fRec189[i] = 0;
		for (int i=0; i<16384; i++) fVec73[i] = 0;
		for (int i=0; i<2; i++) fRec192[i] = 0;
		for (int i=0; i<2; i++) fRec190[i] = 0;
		for (int i=0; i<16384; i++) fVec74[i] = 0;
		for (int i=0; i<2; i++) fRec188[i] = 0;
		for (int i=0; i<2; i++) fRec186[i] = 0;
		for (int i=0; i<16384; i++) fVec75[i] = 0;
		for (int i=0; i<2; i++) fRec193[i] = 0;
		for (int i=0; i<2; i++) fRec187[i] = 0;
		for (int i=0; i<16384; i++) fVec76[i] = 0;
		for (int i=0; i<2; i++) fRec185[i] = 0;
		for (int i=0; i<2; i++) fRec183[i] = 0;
		for (int i=0; i<16384; i++) fVec77[i] = 0;
		for (int i=0; i<2; i++) fRec194[i] = 0;
		for (int i=0; i<2; i++) fRec184[i] = 0;
		for (int i=0; i<16384; i++) fVec78[i] = 0;
		for (int i=0; i<2; i++) fRec182[i] = 0;
		for (int i=0; i<2; i++) fRec180[i] = 0;
		for (int i=0; i<16384; i++) fVec79[i] = 0;
		for (int i=0; i<2; i++) fRec195[i] = 0;
		for (int i=0; i<2; i++) fRec181[i] = 0;
		for (int i=0; i<16384; i++) fVec80[i] = 0;
		for (int i=0; i<2; i++) fRec179[i] = 0;
		for (int i=0; i<2; i++) fRec177[i] = 0;
		for (int i=0; i<16384; i++) fVec81[i] = 0;
		for (int i=0; i<2; i++) fRec196[i] = 0;
		for (int i=0; i<2; i++) fRec178[i] = 0;
		for (int i=0; i<16384; i++) fVec82[i] = 0;
		for (int i=0; i<2; i++) fRec208[i] = 0;
		for (int i=0; i<2; i++) fRec206[i] = 0;
		for (int i=0; i<16384; i++) fVec83[i] = 0;
		for (int i=0; i<2; i++) fRec209[i] = 0;
		for (int i=0; i<2; i++) fRec207[i] = 0;
		for (int i=0; i<16384; i++) fVec84[i] = 0;
		for (int i=0; i<2; i++) fRec205[i] = 0;
		for (int i=0; i<2; i++) fRec203[i] = 0;
		for (int i=0; i<16384; i++) fVec85[i] = 0;
		for (int i=0; i<2; i++) fRec210[i] = 0;
		for (int i=0; i<2; i++) fRec204[i] = 0;
		for (int i=0; i<16384; i++) fVec86[i] = 0;
		for (int i=0; i<2; i++) fRec202[i] = 0;
		for (int i=0; i<2; i++) fRec200[i] = 0;
		for (int i=0; i<16384; i++) fVec87[i] = 0;
		for (int i=0; i<2; i++) fRec211[i] = 0;
		for (int i=0; i<2; i++) fRec201[i] = 0;
		for (int i=0; i<16384; i++) fVec88[i] = 0;
		for (int i=0; i<2; i++) fRec199[i] = 0;
		for (int i=0; i<2; i++) fRec197[i] = 0;
		for (int i=0; i<16384; i++) fVec89[i] = 0;
		for (int i=0; i<2; i++) fRec212[i] = 0;
		for (int i=0; i<2; i++) fRec198[i] = 0;
		for (int i=0; i<16384; i++) fVec90[i] = 0;
		for (int i=0; i<2; i++) fRec176[i] = 0;
		for (int i=0; i<2; i++) fRec174[i] = 0;
		for (int i=0; i<16384; i++) fVec91[i] = 0;
		for (int i=0; i<2; i++) fRec213[i] = 0;
		for (int i=0; i<2; i++) fRec175[i] = 0;
		for (int i=0; i<16384; i++) fVec92[i] = 0;
		for (int i=0; i<2; i++) fRec225[i] = 0;
		for (int i=0; i<2; i++) fRec223[i] = 0;
		for (int i=0; i<16384; i++) fVec93[i] = 0;
		for (int i=0; i<2; i++) fRec226[i] = 0;
		for (int i=0; i<2; i++) fRec224[i] = 0;
		for (int i=0; i<16384; i++) fVec94[i] = 0;
		for (int i=0; i<2; i++) fRec222[i] = 0;
		for (int i=0; i<2; i++) fRec220[i] = 0;
		for (int i=0; i<16384; i++) fVec95[i] = 0;
		for (int i=0; i<2; i++) fRec227[i] = 0;
		for (int i=0; i<2; i++) fRec221[i] = 0;
		for (int i=0; i<16384; i++) fVec96[i] = 0;
		for (int i=0; i<2; i++) fRec219[i] = 0;
		for (int i=0; i<2; i++) fRec217[i] = 0;
		for (int i=0; i<16384; i++) fVec97[i] = 0;
		for (int i=0; i<2; i++) fRec228[i] = 0;
		for (int i=0; i<2; i++) fRec218[i] = 0;
		for (int i=0; i<16384; i++) fVec98[i] = 0;
		for (int i=0; i<2; i++) fRec216[i] = 0;
		for (int i=0; i<2; i++) fRec214[i] = 0;
		for (int i=0; i<16384; i++) fVec99[i] = 0;
		for (int i=0; i<2; i++) fRec229[i] = 0;
		for (int i=0; i<2; i++) fRec215[i] = 0;
		for (int i=0; i<16384; i++) fVec100[i] = 0;
		for (int i=0; i<2; i++) fRec173[i] = 0;
		for (int i=0; i<2; i++) fRec171[i] = 0;
		for (int i=0; i<16384; i++) fVec101[i] = 0;
		for (int i=0; i<2; i++) fRec230[i] = 0;
		for (int i=0; i<2; i++) fRec172[i] = 0;
		for (int i=0; i<16384; i++) fVec102[i] = 0;
		for (int i=0; i<2; i++) fRec170[i] = 0;
		for (int i=0; i<2; i++) fRec168[i] = 0;
		for (int i=0; i<16384; i++) fVec103[i] = 0;
		for (int i=0; i<2; i++) fRec231[i] = 0;
		for (int i=0; i<2; i++) fRec169[i] = 0;
		for (int i=0; i<16384; i++) fVec104[i] = 0;
		for (int i=0; i<2; i++) fRec166[i] = 0;
		for (int i=0; i<2; i++) fRec164[i] = 0;
		for (int i=0; i<2; i++) fRec233[i] = 0;
		for (int i=0; i<16384; i++) fVec105[i] = 0;
		for (int i=0; i<2; i++) fRec232[i] = 0;
		for (int i=0; i<2; i++) fRec165[i] = 0;
		for (int i=0; i<16384; i++) fVec106[i] = 0;
		for (int i=0; i<2; i++) fRec245[i] = 0;
		for (int i=0; i<2; i++) fRec243[i] = 0;
		for (int i=0; i<16384; i++) fVec107[i] = 0;
		for (int i=0; i<2; i++) fRec246[i] = 0;
		for (int i=0; i<2; i++) fRec244[i] = 0;
		for (int i=0; i<16384; i++) fVec108[i] = 0;
		for (int i=0; i<2; i++) fRec242[i] = 0;
		for (int i=0; i<2; i++) fRec240[i] = 0;
		for (int i=0; i<16384; i++) fVec109[i] = 0;
		for (int i=0; i<2; i++) fRec247[i] = 0;
		for (int i=0; i<2; i++) fRec241[i] = 0;
		for (int i=0; i<16384; i++) fVec110[i] = 0;
		for (int i=0; i<2; i++) fRec239[i] = 0;
		for (int i=0; i<2; i++) fRec237[i] = 0;
		for (int i=0; i<16384; i++) fVec111[i] = 0;
		for (int i=0; i<2; i++) fRec248[i] = 0;
		for (int i=0; i<2; i++) fRec238[i] = 0;
		for (int i=0; i<16384; i++) fVec112[i] = 0;
		for (int i=0; i<2; i++) fRec236[i] = 0;
		for (int i=0; i<2; i++) fRec234[i] = 0;
		for (int i=0; i<16384; i++) fVec113[i] = 0;
		for (int i=0; i<2; i++) fRec249[i] = 0;
		for (int i=0; i<2; i++) fRec235[i] = 0;
		for (int i=0; i<16384; i++) fVec114[i] = 0;
		for (int i=0; i<2; i++) fRec162[i] = 0;
		for (int i=0; i<2; i++) fRec160[i] = 0;
		for (int i=0; i<2; i++) fRec251[i] = 0;
		for (int i=0; i<16384; i++) fVec115[i] = 0;
		for (int i=0; i<2; i++) fRec250[i] = 0;
		for (int i=0; i<2; i++) fRec161[i] = 0;
		for (int i=0; i<16384; i++) fVec116[i] = 0;
		for (int i=0; i<2; i++) fRec263[i] = 0;
		for (int i=0; i<2; i++) fRec261[i] = 0;
		for (int i=0; i<16384; i++) fVec117[i] = 0;
		for (int i=0; i<2; i++) fRec264[i] = 0;
		for (int i=0; i<2; i++) fRec262[i] = 0;
		for (int i=0; i<16384; i++) fVec118[i] = 0;
		for (int i=0; i<2; i++) fRec260[i] = 0;
		for (int i=0; i<2; i++) fRec258[i] = 0;
		for (int i=0; i<16384; i++) fVec119[i] = 0;
		for (int i=0; i<2; i++) fRec265[i] = 0;
		for (int i=0; i<2; i++) fRec259[i] = 0;
		for (int i=0; i<16384; i++) fVec120[i] = 0;
		for (int i=0; i<2; i++) fRec257[i] = 0;
		for (int i=0; i<2; i++) fRec255[i] = 0;
		for (int i=0; i<16384; i++) fVec121[i] = 0;
		for (int i=0; i<2; i++) fRec266[i] = 0;
		for (int i=0; i<2; i++) fRec256[i] = 0;
		for (int i=0; i<16384; i++) fVec122[i] = 0;
		for (int i=0; i<2; i++) fRec254[i] = 0;
		for (int i=0; i<2; i++) fRec252[i] = 0;
		for (int i=0; i<16384; i++) fVec123[i] = 0;
		for (int i=0; i<2; i++) fRec267[i] = 0;
		for (int i=0; i<2; i++) fRec253[i] = 0;
		for (int i=0; i<16384; i++) fVec124[i] = 0;
		for (int i=0; i<2; i++) fRec268[i] = 0;
		for (int i=0; i<2; i++) fRec159[i] = 0;
		for (int i=0; i<2; i++) fRec157[i] = 0;
		for (int i=0; i<16384; i++) fVec125[i] = 0;
		for (int i=0; i<2; i++) fRec270[i] = 0;
		for (int i=0; i<2; i++) fRec269[i] = 0;
		for (int i=0; i<2; i++) fRec158[i] = 0;
		for (int i=0; i<16384; i++) fVec126[i] = 0;
		for (int i=0; i<2; i++) fRec123[i] = 0;
		for (int i=0; i<2; i++) fRec121[i] = 0;
		for (int i=0; i<16384; i++) fVec127[i] = 0;
		for (int i=0; i<2; i++) fRec272[i] = 0;
		for (int i=0; i<2; i++) fRec271[i] = 0;
		for (int i=0; i<2; i++) fRec122[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<1024; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec273[i] = 0;
		for (int i=0; i<1024; i++) fRec1[i] = 0;
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
		ui_interface->addHorizontalSlider("damping", &fslider0, 0.0f, 0.0f, 0.99f, 0.001f);
		ui_interface->addHorizontalSlider("delayTime", &fslider4, 0.2f, 0.001f, 1.45f, 0.0001f);
		ui_interface->addHorizontalSlider("diffusion", &fslider1, 0.5f, 0.0f, 0.99f, 0.0001f);
		ui_interface->addHorizontalSlider("feedback", &fslider3, 0.9f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("modDepth", &fslider5, 0.1f, 0.0f, 1.0f, 0.001f);
		ui_interface->addHorizontalSlider("modFreq", &fslider6, 2.0f, 0.0f, 1e+01f, 0.01f);
		ui_interface->addHorizontalSlider("size", &fslider2, 1.0f, 0.5f, 3.0f, 0.0001f);
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = float(fslider0);
		float 	fSlow1 = float(fslider1);
		float 	fSlow2 = float(fslider2);
		int 	iSlow3 = int((10 * fSlow2));
		float 	fSlow4 = float(fslider3);
		float 	fSlow5 = floorf(min((float)65533, (fConst0 * float(fslider4))));
		float 	fSlow6 = float(fslider5);
		float 	fSlow7 = float(fslider6);
		float 	fSlow8 = (23 * fSlow2);
		int 	iSlow9 = int(fSlow8);
		float 	fSlow10 = (36 * fSlow2);
		int 	iSlow11 = int(fSlow10);
		float 	fSlow12 = (49 * fSlow2);
		int 	iSlow13 = int(fSlow12);
		int 	iSlow14 = int((fSlow12 + 10));
		int 	iSlow15 = int((fSlow10 + 10));
		int 	iSlow16 = int((fSlow8 + 10));
		int 	iSlow17 = int((10 * (fSlow2 + 1)));
		float 	fSlow18 = (29 * fSlow2);
		int 	iSlow19 = int(fSlow18);
		float 	fSlow20 = (42 * fSlow2);
		int 	iSlow21 = int(fSlow20);
		float 	fSlow22 = (55 * fSlow2);
		int 	iSlow23 = int(fSlow22);
		float 	fSlow24 = (68 * fSlow2);
		int 	iSlow25 = int(fSlow24);
		int 	iSlow26 = int((fSlow24 + 10));
		int 	iSlow27 = int((fSlow22 + 10));
		int 	iSlow28 = int((fSlow20 + 10));
		int 	iSlow29 = int((fSlow18 + 10));
		float 	fSlow30 = (48 * fSlow2);
		int 	iSlow31 = int(fSlow30);
		float 	fSlow32 = (74 * fSlow2);
		int 	iSlow33 = int(fSlow32);
		float 	fSlow34 = (87 * fSlow2);
		int 	iSlow35 = int(fSlow34);
		int 	iSlow36 = int((fSlow34 + 10));
		int 	iSlow37 = int((fSlow32 + 10));
		float 	fSlow38 = (61 * fSlow2);
		int 	iSlow39 = int(fSlow38);
		int 	iSlow40 = int((fSlow38 + 10));
		int 	iSlow41 = int((fSlow30 + 10));
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			fVec0[0] = fSlow0;
			iVec1[0] = 1;
			float fTemp0 = (fSlow0 + fVec0[1]);
			float fTemp1 = (1.0f - (0.5f * fTemp0));
			fVec2[0] = fSlow1;
			float fTemp2 = (0.5f * (fSlow1 + fVec2[1]));
			float fTemp3 = sinf(fTemp2);
			float fTemp4 = (0 - fTemp2);
			float fTemp5 = sinf(fTemp4);
			float fTemp6 = cosf(fTemp2);
			int iTemp7 = itbl0[iSlow3];
			int iTemp8 = (1 - iVec1[1]);
			fRec6[0] = ((0.001f * iTemp7) + (0.999f * ((iTemp8 * iTemp7) + fRec6[1])));
			float fTemp9 = (fRec6[0] + -1.49999f);
			float fTemp10 = floorf(fTemp9);
			float fTemp11 = (fTemp10 + (2.0f - fRec6[0]));
			float fTemp12 = (0 - fTemp3);
			fVec3[0] = fSlow4;
			float fTemp13 = (fSlow4 + fVec3[1]);
			float fTemp14 = ((int((fRec7[1] != 0.0f)))?((int(((fRec8[1] > 0.0f) & (fRec8[1] < 1.0f))))?fRec7[1]:0):((int(((fRec8[1] == 0.0f) & (fSlow5 != fRec9[1]))))?4.5351473e-05f:((int(((fRec8[1] == 1.0f) & (fSlow5 != fRec10[1]))))?-4.5351473e-05f:0)));
			fRec7[0] = fTemp14;
			fRec8[0] = max(0.0f, min(1.0f, (fRec8[1] + fTemp14)));
			fRec9[0] = ((int(((fRec8[1] >= 1.0f) & (fRec10[1] != fSlow5))))?fSlow5:fRec9[1]);
			fRec10[0] = ((int(((fRec8[1] <= 0.0f) & (fRec9[1] != fSlow5))))?fSlow5:fRec10[1]);
			fVec4[0] = fSlow6;
			float fTemp15 = (fSlow6 + fVec4[1]);
			fVec5[0] = fSlow7;
			float fTemp16 = (fConst2 * (fSlow7 + fVec5[1]));
			float fTemp17 = sinf(fTemp16);
			float fTemp18 = cosf(fTemp16);
			fRec11[0] = ((fTemp17 * fRec12[1]) + (fTemp18 * fRec11[1]));
			fRec12[0] = (((fTemp18 * fRec12[1]) + ((0 - fTemp17) * fRec11[1])) + iTemp8);
			float fTemp19 = (fConst1 * (fTemp15 * (fRec12[0] + 1.0f)));
			float fTemp20 = (fTemp19 + 8.500005f);
			float fTemp21 = floorf(fTemp20);
			float fTemp22 = (fTemp19 + (8.0f - fTemp21));
			float fTemp23 = (fTemp19 + (7.0f - fTemp21));
			float fTemp24 = (fTemp19 + (6.0f - fTemp21));
			int iTemp25 = int(fTemp20);
			float fTemp26 = ((((((0 - (0.5f * fTemp22)) * (0 - (0.33333334f * fTemp23))) * (0 - (0.25f * fTemp24))) * fRec0[(IOTA-int((int(min(512, max(0, iTemp25))) + 1)))&1023]) * (fTemp21 + (-9.0f - fTemp19))) + ((fTemp19 + (1e+01f - fTemp21)) * (((((0 - (0.5f * fTemp23)) * (0 - (0.33333334f * fTemp24))) * fRec0[(IOTA-int((int(min(512, max(0, (iTemp25 + 1)))) + 1)))&1023]) * (fTemp21 + (-8.0f - fTemp19))) + ((fTemp19 + (9.0f - fTemp21)) * ((0.041666668f * ((fTemp22 * fTemp23) * fRec0[(IOTA-int((int(min(512, max(0, (iTemp25 + 4)))) + 1)))&1023])) + ((0.16666667f * ((fTemp22 * fRec0[(IOTA-int((int(min(512, max(0, (iTemp25 + 3)))) + 1)))&1023]) * (fTemp21 + (-6.0f - fTemp19)))) + (0.5f * (((0 - (0.5f * fTemp24)) * fRec0[(IOTA-int((int(min(512, max(0, (iTemp25 + 2)))) + 1)))&1023]) * (fTemp21 + (-7.0f - fTemp19))))))))));
			fVec6[IOTA&131071] = fTemp26;
			int iTemp27 = int(min((float)65536, max((float)0, fRec10[0])));
			float fTemp28 = (1.0f - fRec8[0]);
			int iTemp29 = int(min((float)65536, max((float)0, fRec9[0])));
			float fTemp30 = ((float)input0[i] + (0.5f * (fTemp13 * ((fRec8[0] * fVec6[(IOTA-iTemp27)&131071]) + (fTemp28 * fVec6[(IOTA-iTemp29)&131071])))));
			float fTemp31 = (fTemp6 * fTemp30);
			float fTemp32 = ((fTemp12 * fRec3[1]) + fTemp31);
			float fTemp33 = (fTemp3 * fTemp32);
			int iTemp34 = itbl0[iSlow9];
			fRec16[0] = ((0.001f * iTemp34) + (0.999f * ((iTemp8 * iTemp34) + fRec16[1])));
			float fTemp35 = (fRec16[0] + -1.49999f);
			float fTemp36 = floorf(fTemp35);
			float fTemp37 = (fTemp36 + (2.0f - fRec16[0]));
			float fTemp38 = ((fTemp6 * fTemp32) + (fTemp12 * fRec13[1]));
			float fTemp39 = (fTemp3 * fTemp38);
			int iTemp40 = itbl0[iSlow11];
			fRec20[0] = ((0.001f * iTemp40) + (0.999f * ((iTemp8 * iTemp40) + fRec20[1])));
			float fTemp41 = (fRec20[0] + -1.49999f);
			float fTemp42 = floorf(fTemp41);
			float fTemp43 = (fTemp42 + (2.0f - fRec20[0]));
			float fTemp44 = ((fTemp6 * fTemp38) + (fTemp12 * fRec17[1]));
			float fTemp45 = (fTemp3 * fTemp44);
			int iTemp46 = itbl0[iSlow13];
			fRec24[0] = ((0.0001f * iTemp46) + (0.9999f * ((iTemp8 * iTemp46) + fRec24[1])));
			float fTemp47 = (fRec24[0] + -1.49999f);
			float fTemp48 = floorf(fTemp47);
			float fTemp49 = (fTemp48 + (2.0f - fRec24[0]));
			float fTemp50 = ((fTemp12 * fRec21[1]) + (fTemp6 * fTemp44));
			float fTemp51 = (fConst1 * (fTemp15 * (fRec11[0] + 1.0f)));
			float fTemp52 = (fTemp51 + 8.500005f);
			float fTemp53 = floorf(fTemp52);
			float fTemp54 = (fTemp51 + (8.0f - fTemp53));
			float fTemp55 = (fTemp51 + (7.0f - fTemp53));
			float fTemp56 = (fTemp51 + (6.0f - fTemp53));
			int iTemp57 = int(fTemp52);
			float fTemp58 = ((((((0 - (0.5f * fTemp54)) * (0 - (0.33333334f * fTemp55))) * (0 - (0.25f * fTemp56))) * fRec1[(IOTA-int((int(min(512, max(0, iTemp57))) + 1)))&1023]) * (fTemp53 + (-9.0f - fTemp51))) + ((fTemp51 + (1e+01f - fTemp53)) * (((((0 - (0.5f * fTemp55)) * (0 - (0.33333334f * fTemp56))) * fRec1[(IOTA-int((int(min(512, max(0, (iTemp57 + 1)))) + 1)))&1023]) * (fTemp53 + (-8.0f - fTemp51))) + ((fTemp51 + (9.0f - fTemp53)) * ((0.041666668f * ((fTemp54 * fTemp55) * fRec1[(IOTA-int((int(min(512, max(0, (iTemp57 + 4)))) + 1)))&1023])) + ((0.16666667f * ((fTemp54 * fRec1[(IOTA-int((int(min(512, max(0, (iTemp57 + 3)))) + 1)))&1023]) * (fTemp53 + (-6.0f - fTemp51)))) + (0.5f * (((0 - (0.5f * fTemp56)) * fRec1[(IOTA-int((int(min(512, max(0, (iTemp57 + 2)))) + 1)))&1023]) * (fTemp53 + (-7.0f - fTemp51))))))))));
			fVec7[IOTA&131071] = fTemp58;
			float fTemp59 = ((float)input1[i] + (0.5f * (fTemp13 * ((fRec8[0] * fVec7[(IOTA-iTemp27)&131071]) + (fTemp28 * fVec7[(IOTA-iTemp29)&131071])))));
			float fTemp60 = (fTemp6 * fTemp59);
			float fTemp61 = ((fTemp12 * fRec4[1]) + fTemp60);
			float fTemp62 = ((fTemp12 * fRec14[1]) + (fTemp6 * fTemp61));
			float fTemp63 = ((fTemp12 * fRec18[1]) + (fTemp6 * fTemp62));
			float fTemp64 = ((fTemp12 * fRec22[1]) + (fTemp6 * fTemp63));
			float fTemp65 = ((6.123234e-17f * fTemp50) - fTemp64);
			fVec8[IOTA&16383] = fTemp65;
			int iTemp66 = int(min(8192, max(0, int(fTemp47))));
			float fTemp67 = (fRec24[0] - fTemp48);
			float fTemp68 = (0 - (fTemp49 / fTemp67));
			int iTemp69 = int((iTemp66 + 1));
			fRec23[0] = (((fTemp49 * fVec8[(IOTA-iTemp66)&16383]) / fTemp67) + ((fTemp68 * fRec23[1]) + fVec8[(IOTA-iTemp69)&16383]));
			fRec21[0] = fRec23[0];
			int iTemp70 = itbl0[iSlow14];
			fRec26[0] = ((0.0001f * iTemp70) + (0.9999f * ((iTemp8 * iTemp70) + fRec26[1])));
			float fTemp71 = (fRec26[0] + -1.49999f);
			float fTemp72 = floorf(fTemp71);
			float fTemp73 = (fTemp72 + (2.0f - fRec26[0]));
			float fTemp74 = (fTemp50 + (6.123234e-17f * fTemp64));
			fVec9[IOTA&16383] = fTemp74;
			int iTemp75 = int(min(8192, max(0, int(fTemp71))));
			float fTemp76 = (fRec26[0] - fTemp72);
			int iTemp77 = int((iTemp75 + 1));
			float fTemp78 = (0 - (fTemp73 / fTemp76));
			fRec25[0] = (((fTemp73 * fVec9[(IOTA-iTemp75)&16383]) / fTemp76) + (fVec9[(IOTA-iTemp77)&16383] + (fTemp78 * fRec25[1])));
			fRec22[0] = fRec25[0];
			float fTemp79 = (fTemp6 * fRec21[1]);
			float fTemp80 = ((fTemp3 * fTemp63) + (fTemp6 * fRec22[1]));
			float fTemp81 = ((6.123234e-17f * (fTemp45 + fTemp79)) - fTemp80);
			fVec10[IOTA&16383] = fTemp81;
			int iTemp82 = int(min(8192, max(0, int(fTemp41))));
			float fTemp83 = (fRec20[0] - fTemp42);
			float fTemp84 = (0 - (fTemp43 / fTemp83));
			int iTemp85 = int((iTemp82 + 1));
			fRec19[0] = (((fTemp43 * fVec10[(IOTA-iTemp82)&16383]) / fTemp83) + ((fTemp84 * fRec19[1]) + fVec10[(IOTA-iTemp85)&16383]));
			fRec17[0] = fRec19[0];
			int iTemp86 = itbl0[iSlow15];
			fRec28[0] = ((0.001f * iTemp86) + (0.999f * ((iTemp8 * iTemp86) + fRec28[1])));
			float fTemp87 = (fRec28[0] + -1.49999f);
			float fTemp88 = floorf(fTemp87);
			float fTemp89 = (fTemp88 + (2.0f - fRec28[0]));
			float fTemp90 = ((fTemp45 + (6.123234e-17f * fTemp80)) + fTemp79);
			fVec11[IOTA&16383] = fTemp90;
			int iTemp91 = int(min(8192, max(0, int(fTemp87))));
			float fTemp92 = (fRec28[0] - fTemp88);
			int iTemp93 = int((iTemp91 + 1));
			float fTemp94 = (0 - (fTemp89 / fTemp92));
			fRec27[0] = (((fTemp89 * fVec11[(IOTA-iTemp91)&16383]) / fTemp92) + (fVec11[(IOTA-iTemp93)&16383] + (fTemp94 * fRec27[1])));
			fRec18[0] = fRec27[0];
			float fTemp95 = (fTemp6 * fRec17[1]);
			float fTemp96 = ((fTemp3 * fTemp62) + (fTemp6 * fRec18[1]));
			float fTemp97 = ((6.123234e-17f * (fTemp39 + fTemp95)) - fTemp96);
			fVec12[IOTA&16383] = fTemp97;
			int iTemp98 = int(min(8192, max(0, int(fTemp35))));
			float fTemp99 = (fRec16[0] - fTemp36);
			float fTemp100 = (0 - (fTemp37 / fTemp99));
			int iTemp101 = int((iTemp98 + 1));
			fRec15[0] = (((fTemp37 * fVec12[(IOTA-iTemp98)&16383]) / fTemp99) + ((fTemp100 * fRec15[1]) + fVec12[(IOTA-iTemp101)&16383]));
			fRec13[0] = fRec15[0];
			int iTemp102 = itbl0[iSlow16];
			fRec30[0] = ((0.001f * iTemp102) + (0.999f * ((iTemp8 * iTemp102) + fRec30[1])));
			float fTemp103 = (fRec30[0] + -1.49999f);
			float fTemp104 = floorf(fTemp103);
			float fTemp105 = (fTemp104 + (2.0f - fRec30[0]));
			float fTemp106 = ((fTemp39 + (6.123234e-17f * fTemp96)) + fTemp95);
			fVec13[IOTA&16383] = fTemp106;
			int iTemp107 = int(min(8192, max(0, int(fTemp103))));
			float fTemp108 = (fRec30[0] - fTemp104);
			int iTemp109 = int((iTemp107 + 1));
			float fTemp110 = (0 - (fTemp105 / fTemp108));
			fRec29[0] = (((fTemp105 * fVec13[(IOTA-iTemp107)&16383]) / fTemp108) + (fVec13[(IOTA-iTemp109)&16383] + (fTemp110 * fRec29[1])));
			fRec14[0] = fRec29[0];
			float fTemp111 = (fTemp6 * fRec13[1]);
			float fTemp112 = ((fTemp3 * fTemp61) + (fTemp6 * fRec14[1]));
			float fTemp113 = ((6.123234e-17f * (fTemp33 + fTemp111)) - fTemp112);
			fVec14[IOTA&16383] = fTemp113;
			int iTemp114 = int(min(8192, max(0, int(fTemp9))));
			float fTemp115 = (fRec6[0] - fTemp10);
			float fTemp116 = (0 - (fTemp11 / fTemp115));
			int iTemp117 = int((iTemp114 + 1));
			fRec5[0] = (((fTemp11 * fVec14[(IOTA-iTemp114)&16383]) / fTemp115) + ((fTemp116 * fRec5[1]) + fVec14[(IOTA-iTemp117)&16383]));
			fRec3[0] = fRec5[0];
			int iTemp118 = itbl0[iSlow17];
			fRec32[0] = ((0.001f * iTemp118) + (0.999f * ((iTemp8 * iTemp118) + fRec32[1])));
			float fTemp119 = (fRec32[0] + -1.49999f);
			float fTemp120 = floorf(fTemp119);
			float fTemp121 = (fTemp120 + (2.0f - fRec32[0]));
			float fTemp122 = ((fTemp33 + (6.123234e-17f * fTemp112)) + fTemp111);
			fVec15[IOTA&16383] = fTemp122;
			int iTemp123 = int(min(8192, max(0, int(fTemp119))));
			float fTemp124 = (fRec32[0] - fTemp120);
			int iTemp125 = int((iTemp123 + 1));
			float fTemp126 = (0 - (fTemp121 / fTemp124));
			fRec31[0] = (((fTemp121 * fVec15[(IOTA-iTemp123)&16383]) / fTemp124) + (fVec15[(IOTA-iTemp125)&16383] + (fTemp126 * fRec31[1])));
			fRec4[0] = fRec31[0];
			float fTemp127 = (fTemp3 * fTemp30);
			float fTemp128 = cosf(fTemp4);
			int iTemp129 = itbl0[iSlow19];
			fRec36[0] = ((0.001f * iTemp129) + (0.999f * ((iTemp129 * iTemp8) + fRec36[1])));
			float fTemp130 = (fRec36[0] + -1.49999f);
			float fTemp131 = floorf(fTemp130);
			float fTemp132 = (fTemp131 + (2.0f - fRec36[0]));
			float fTemp133 = (0 - fTemp5);
			float fTemp134 = (fTemp133 * fRec33[1]);
			float fTemp135 = ((fTemp12 * fRec37[1]) + fTemp31);
			float fTemp136 = ((fTemp12 * fRec40[1]) + (fTemp6 * fTemp135));
			float fTemp137 = ((fTemp12 * fRec43[1]) + (fTemp6 * fTemp136));
			float fTemp138 = ((fTemp12 * fRec46[1]) + (fTemp6 * fTemp137));
			float fTemp139 = ((fTemp12 * fRec38[1]) + fTemp60);
			float fTemp140 = ((fTemp12 * fRec41[1]) + (fTemp6 * fTemp139));
			float fTemp141 = ((fTemp12 * fRec44[1]) + (fTemp6 * fTemp140));
			float fTemp142 = ((fTemp12 * fRec47[1]) + (fTemp6 * fTemp141));
			float fTemp143 = ((6.123234e-17f * fTemp138) - fTemp142);
			fVec16[IOTA&16383] = fTemp143;
			fRec48[0] = (((fTemp49 * fVec16[(IOTA-iTemp66)&16383]) / fTemp67) + ((fRec48[1] * fTemp68) + fVec16[(IOTA-iTemp69)&16383]));
			fRec46[0] = fRec48[0];
			float fTemp144 = (fTemp138 + (6.123234e-17f * fTemp142));
			fVec17[IOTA&16383] = fTemp144;
			fRec49[0] = (((fTemp73 * fVec17[(IOTA-iTemp75)&16383]) / fTemp76) + ((fRec49[1] * fTemp78) + fVec17[(IOTA-iTemp77)&16383]));
			fRec47[0] = fRec49[0];
			float fTemp145 = ((fTemp6 * fRec46[1]) + (fTemp3 * fTemp137));
			float fTemp146 = ((fTemp6 * fRec47[1]) + (fTemp3 * fTemp141));
			float fTemp147 = ((6.123234e-17f * fTemp145) - fTemp146);
			fVec18[IOTA&16383] = fTemp147;
			fRec45[0] = (((fTemp43 * fVec18[(IOTA-iTemp82)&16383]) / fTemp83) + ((fRec45[1] * fTemp84) + fVec18[(IOTA-iTemp85)&16383]));
			fRec43[0] = fRec45[0];
			float fTemp148 = (fTemp145 + (6.123234e-17f * fTemp146));
			fVec19[IOTA&16383] = fTemp148;
			fRec50[0] = (fVec19[(IOTA-iTemp93)&16383] + ((fRec50[1] * fTemp94) + ((fTemp89 * fVec19[(IOTA-iTemp91)&16383]) / fTemp92)));
			fRec44[0] = fRec50[0];
			float fTemp149 = ((fTemp6 * fRec43[1]) + (fTemp3 * fTemp136));
			float fTemp150 = ((fTemp6 * fRec44[1]) + (fTemp3 * fTemp140));
			float fTemp151 = ((6.123234e-17f * fTemp149) - fTemp150);
			fVec20[IOTA&16383] = fTemp151;
			fRec42[0] = (((fTemp37 * fVec20[(IOTA-iTemp98)&16383]) / fTemp99) + ((fRec42[1] * fTemp100) + fVec20[(IOTA-iTemp101)&16383]));
			fRec40[0] = fRec42[0];
			float fTemp152 = (fTemp149 + (6.123234e-17f * fTemp150));
			fVec21[IOTA&16383] = fTemp152;
			fRec51[0] = (fVec21[(IOTA-iTemp109)&16383] + ((fRec51[1] * fTemp110) + ((fTemp105 * fVec21[(IOTA-iTemp107)&16383]) / fTemp108)));
			fRec41[0] = fRec51[0];
			float fTemp153 = ((fTemp6 * fRec40[1]) + (fTemp3 * fTemp135));
			float fTemp154 = ((fTemp6 * fRec41[1]) + (fTemp3 * fTemp139));
			float fTemp155 = ((6.123234e-17f * fTemp153) - fTemp154);
			fVec22[IOTA&16383] = fTemp155;
			fRec39[0] = (((fTemp11 * fVec22[(IOTA-iTemp114)&16383]) / fTemp115) + ((fRec39[1] * fTemp116) + fVec22[(IOTA-iTemp117)&16383]));
			fRec37[0] = fRec39[0];
			float fTemp156 = (fTemp153 + (6.123234e-17f * fTemp154));
			fVec23[IOTA&16383] = fTemp156;
			fRec52[0] = (fVec23[(IOTA-iTemp125)&16383] + ((fRec52[1] * fTemp126) + ((fTemp121 * fVec23[(IOTA-iTemp123)&16383]) / fTemp124)));
			fRec38[0] = fRec52[0];
			float fTemp157 = ((fTemp6 * fRec37[1]) + fTemp127);
			float fTemp158 = (fTemp5 * (fTemp134 + (fTemp128 * fTemp157)));
			int iTemp159 = itbl0[iSlow21];
			fRec56[0] = ((0.001f * iTemp159) + (0.999f * ((iTemp8 * iTemp159) + fRec56[1])));
			float fTemp160 = (fRec56[0] + -1.49999f);
			float fTemp161 = floorf(fTemp160);
			float fTemp162 = (fTemp161 + (2.0f - fRec56[0]));
			float fTemp163 = (fTemp133 * fRec53[1]);
			float fTemp164 = ((fTemp12 * fRec57[1]) + fTemp31);
			float fTemp165 = (fTemp3 * fTemp164);
			float fTemp166 = ((fTemp6 * fTemp164) + (fTemp12 * fRec60[1]));
			float fTemp167 = (fTemp3 * fTemp166);
			float fTemp168 = ((fTemp6 * fTemp166) + (fTemp12 * fRec63[1]));
			float fTemp169 = (fTemp3 * fTemp168);
			float fTemp170 = ((fTemp12 * fRec66[1]) + (fTemp6 * fTemp168));
			float fTemp171 = ((fTemp12 * fRec58[1]) + fTemp60);
			float fTemp172 = ((fTemp12 * fRec61[1]) + (fTemp6 * fTemp171));
			float fTemp173 = ((fTemp12 * fRec64[1]) + (fTemp6 * fTemp172));
			float fTemp174 = ((fTemp12 * fRec67[1]) + (fTemp6 * fTemp173));
			float fTemp175 = ((6.123234e-17f * fTemp170) - fTemp174);
			fVec24[IOTA&16383] = fTemp175;
			fRec68[0] = (((fTemp49 * fVec24[(IOTA-iTemp66)&16383]) / fTemp67) + (fVec24[(IOTA-iTemp69)&16383] + (fTemp68 * fRec68[1])));
			fRec66[0] = fRec68[0];
			float fTemp176 = (fTemp170 + (6.123234e-17f * fTemp174));
			fVec25[IOTA&16383] = fTemp176;
			fRec69[0] = (((fTemp73 * fVec25[(IOTA-iTemp75)&16383]) / fTemp76) + (fVec25[(IOTA-iTemp77)&16383] + (fTemp78 * fRec69[1])));
			fRec67[0] = fRec69[0];
			float fTemp177 = (fTemp6 * fRec66[1]);
			float fTemp178 = ((fTemp3 * fTemp173) + (fTemp6 * fRec67[1]));
			float fTemp179 = ((6.123234e-17f * (fTemp169 + fTemp177)) - fTemp178);
			fVec26[IOTA&16383] = fTemp179;
			fRec65[0] = (((fTemp43 * fVec26[(IOTA-iTemp82)&16383]) / fTemp83) + ((fTemp84 * fRec65[1]) + fVec26[(IOTA-iTemp85)&16383]));
			fRec63[0] = fRec65[0];
			float fTemp180 = ((fTemp169 + (6.123234e-17f * fTemp178)) + fTemp177);
			fVec27[IOTA&16383] = fTemp180;
			fRec70[0] = ((((fTemp89 * fVec27[(IOTA-iTemp91)&16383]) / fTemp92) + fVec27[(IOTA-iTemp93)&16383]) + (fTemp94 * fRec70[1]));
			fRec64[0] = fRec70[0];
			float fTemp181 = (fTemp6 * fRec63[1]);
			float fTemp182 = ((fTemp3 * fTemp172) + (fTemp6 * fRec64[1]));
			float fTemp183 = ((6.123234e-17f * (fTemp167 + fTemp181)) - fTemp182);
			fVec28[IOTA&16383] = fTemp183;
			fRec62[0] = (((fTemp37 * fVec28[(IOTA-iTemp98)&16383]) / fTemp99) + ((fTemp100 * fRec62[1]) + fVec28[(IOTA-iTemp101)&16383]));
			fRec60[0] = fRec62[0];
			float fTemp184 = ((fTemp167 + (6.123234e-17f * fTemp182)) + fTemp181);
			fVec29[IOTA&16383] = fTemp184;
			fRec71[0] = ((((fTemp105 * fVec29[(IOTA-iTemp107)&16383]) / fTemp108) + fVec29[(IOTA-iTemp109)&16383]) + (fTemp110 * fRec71[1]));
			fRec61[0] = fRec71[0];
			float fTemp185 = (fTemp6 * fRec60[1]);
			float fTemp186 = ((fTemp3 * fTemp171) + (fTemp6 * fRec61[1]));
			float fTemp187 = ((6.123234e-17f * (fTemp165 + fTemp185)) - fTemp186);
			fVec30[IOTA&16383] = fTemp187;
			fRec59[0] = (((fTemp11 * fVec30[(IOTA-iTemp114)&16383]) / fTemp115) + ((fTemp116 * fRec59[1]) + fVec30[(IOTA-iTemp117)&16383]));
			fRec57[0] = fRec59[0];
			float fTemp188 = ((fTemp165 + (6.123234e-17f * fTemp186)) + fTemp185);
			fVec31[IOTA&16383] = fTemp188;
			fRec72[0] = ((((fTemp121 * fVec31[(IOTA-iTemp123)&16383]) / fTemp124) + fVec31[(IOTA-iTemp125)&16383]) + (fTemp126 * fRec72[1]));
			fRec58[0] = fRec72[0];
			float fTemp189 = ((fTemp6 * fRec57[1]) + fTemp127);
			float fTemp190 = (fTemp128 * fTemp189);
			float fTemp191 = (fTemp5 * (fTemp163 + (fTemp128 * (fTemp190 + fTemp134))));
			int iTemp192 = itbl0[iSlow23];
			fRec76[0] = ((0.001f * iTemp192) + (0.999f * ((iTemp8 * iTemp192) + fRec76[1])));
			float fTemp193 = (fRec76[0] + -1.49999f);
			float fTemp194 = floorf(fTemp193);
			float fTemp195 = (fTemp194 + (2.0f - fRec76[0]));
			float fTemp196 = (fTemp133 * fRec73[1]);
			float fTemp197 = ((fTemp12 * fRec77[1]) + fTemp31);
			float fTemp198 = ((fTemp12 * fRec80[1]) + (fTemp6 * fTemp197));
			float fTemp199 = ((fTemp12 * fRec83[1]) + (fTemp6 * fTemp198));
			float fTemp200 = ((fTemp12 * fRec86[1]) + (fTemp6 * fTemp199));
			float fTemp201 = ((fTemp12 * fRec78[1]) + fTemp60);
			float fTemp202 = ((fTemp12 * fRec81[1]) + (fTemp6 * fTemp201));
			float fTemp203 = ((fTemp12 * fRec84[1]) + (fTemp6 * fTemp202));
			float fTemp204 = ((fTemp12 * fRec87[1]) + (fTemp6 * fTemp203));
			float fTemp205 = ((6.123234e-17f * fTemp200) - fTemp204);
			fVec32[IOTA&16383] = fTemp205;
			fRec88[0] = (((fTemp49 * fVec32[(IOTA-iTemp66)&16383]) / fTemp67) + ((fRec88[1] * fTemp68) + fVec32[(IOTA-iTemp69)&16383]));
			fRec86[0] = fRec88[0];
			float fTemp206 = (fTemp200 + (6.123234e-17f * fTemp204));
			fVec33[IOTA&16383] = fTemp206;
			fRec89[0] = (((fTemp73 * fVec33[(IOTA-iTemp75)&16383]) / fTemp76) + ((fRec89[1] * fTemp78) + fVec33[(IOTA-iTemp77)&16383]));
			fRec87[0] = fRec89[0];
			float fTemp207 = ((fTemp6 * fRec86[1]) + (fTemp3 * fTemp199));
			float fTemp208 = ((fTemp6 * fRec87[1]) + (fTemp3 * fTemp203));
			float fTemp209 = ((6.123234e-17f * fTemp207) - fTemp208);
			fVec34[IOTA&16383] = fTemp209;
			fRec85[0] = (((fTemp43 * fVec34[(IOTA-iTemp82)&16383]) / fTemp83) + ((fRec85[1] * fTemp84) + fVec34[(IOTA-iTemp85)&16383]));
			fRec83[0] = fRec85[0];
			float fTemp210 = (fTemp207 + (6.123234e-17f * fTemp208));
			fVec35[IOTA&16383] = fTemp210;
			fRec90[0] = (fVec35[(IOTA-iTemp93)&16383] + ((fRec90[1] * fTemp94) + ((fTemp89 * fVec35[(IOTA-iTemp91)&16383]) / fTemp92)));
			fRec84[0] = fRec90[0];
			float fTemp211 = ((fTemp6 * fRec83[1]) + (fTemp3 * fTemp198));
			float fTemp212 = ((fTemp6 * fRec84[1]) + (fTemp3 * fTemp202));
			float fTemp213 = ((6.123234e-17f * fTemp211) - fTemp212);
			fVec36[IOTA&16383] = fTemp213;
			fRec82[0] = (((fTemp37 * fVec36[(IOTA-iTemp98)&16383]) / fTemp99) + ((fRec82[1] * fTemp100) + fVec36[(IOTA-iTemp101)&16383]));
			fRec80[0] = fRec82[0];
			float fTemp214 = (fTemp211 + (6.123234e-17f * fTemp212));
			fVec37[IOTA&16383] = fTemp214;
			fRec91[0] = (fVec37[(IOTA-iTemp109)&16383] + ((fRec91[1] * fTemp110) + ((fTemp105 * fVec37[(IOTA-iTemp107)&16383]) / fTemp108)));
			fRec81[0] = fRec91[0];
			float fTemp215 = ((fTemp6 * fRec80[1]) + (fTemp3 * fTemp197));
			float fTemp216 = ((fTemp6 * fRec81[1]) + (fTemp3 * fTemp201));
			float fTemp217 = ((6.123234e-17f * fTemp215) - fTemp216);
			fVec38[IOTA&16383] = fTemp217;
			fRec79[0] = (((fTemp11 * fVec38[(IOTA-iTemp114)&16383]) / fTemp115) + ((fRec79[1] * fTemp116) + fVec38[(IOTA-iTemp117)&16383]));
			fRec77[0] = fRec79[0];
			float fTemp218 = (fTemp215 + (6.123234e-17f * fTemp216));
			fVec39[IOTA&16383] = fTemp218;
			fRec92[0] = (fVec39[(IOTA-iTemp125)&16383] + ((fRec92[1] * fTemp126) + ((fTemp121 * fVec39[(IOTA-iTemp123)&16383]) / fTemp124)));
			fRec78[0] = fRec92[0];
			float fTemp219 = ((fTemp6 * fRec77[1]) + fTemp127);
			float fTemp220 = (fTemp128 * fTemp219);
			float fTemp221 = (fTemp5 * (fTemp196 + (fTemp128 * (fTemp163 + (fTemp128 * (fTemp220 + fTemp134))))));
			int iTemp222 = itbl0[iSlow25];
			fRec96[0] = ((0.0001f * iTemp222) + (0.9999f * ((iTemp8 * iTemp222) + fRec96[1])));
			float fTemp223 = (fRec96[0] + -1.49999f);
			float fTemp224 = floorf(fTemp223);
			float fTemp225 = (fTemp224 + (2.0f - fRec96[0]));
			float fTemp226 = ((fTemp12 * fRec97[1]) + fTemp31);
			float fTemp227 = ((fTemp12 * fRec100[1]) + (fTemp6 * fTemp226));
			float fTemp228 = ((fTemp12 * fRec103[1]) + (fTemp6 * fTemp227));
			float fTemp229 = ((fTemp12 * fRec106[1]) + (fTemp6 * fTemp228));
			float fTemp230 = (fTemp60 + (fTemp12 * fRec98[1]));
			float fTemp231 = ((fTemp12 * fRec101[1]) + (fTemp6 * fTemp230));
			float fTemp232 = ((fTemp12 * fRec104[1]) + (fTemp6 * fTemp231));
			float fTemp233 = ((fTemp12 * fRec107[1]) + (fTemp6 * fTemp232));
			float fTemp234 = ((6.123234e-17f * fTemp229) - fTemp233);
			fVec40[IOTA&16383] = fTemp234;
			fRec108[0] = (((fTemp49 * fVec40[(IOTA-iTemp66)&16383]) / fTemp67) + ((fTemp68 * fRec108[1]) + fVec40[(IOTA-iTemp69)&16383]));
			fRec106[0] = fRec108[0];
			float fTemp235 = (fTemp229 + (6.123234e-17f * fTemp233));
			fVec41[IOTA&16383] = fTemp235;
			fRec109[0] = (((fTemp73 * fVec41[(IOTA-iTemp75)&16383]) / fTemp76) + (fVec41[(IOTA-iTemp77)&16383] + (fTemp78 * fRec109[1])));
			fRec107[0] = fRec109[0];
			float fTemp236 = ((fTemp6 * fRec106[1]) + (fTemp3 * fTemp228));
			float fTemp237 = ((fTemp6 * fRec107[1]) + (fTemp3 * fTemp232));
			float fTemp238 = ((6.123234e-17f * fTemp236) - fTemp237);
			fVec42[IOTA&16383] = fTemp238;
			fRec105[0] = (fVec42[(IOTA-iTemp85)&16383] + ((fTemp84 * fRec105[1]) + ((fTemp43 * fVec42[(IOTA-iTemp82)&16383]) / fTemp83)));
			fRec103[0] = fRec105[0];
			float fTemp239 = (fTemp236 + (6.123234e-17f * fTemp237));
			fVec43[IOTA&16383] = fTemp239;
			fRec110[0] = ((((fTemp89 * fVec43[(IOTA-iTemp91)&16383]) / fTemp92) + fVec43[(IOTA-iTemp93)&16383]) + (fTemp94 * fRec110[1]));
			fRec104[0] = fRec110[0];
			float fTemp240 = ((fTemp6 * fRec103[1]) + (fTemp3 * fTemp227));
			float fTemp241 = ((fTemp6 * fRec104[1]) + (fTemp3 * fTemp231));
			float fTemp242 = ((6.123234e-17f * fTemp240) - fTemp241);
			fVec44[IOTA&16383] = fTemp242;
			fRec102[0] = (((fTemp37 * fVec44[(IOTA-iTemp98)&16383]) / fTemp99) + (fVec44[(IOTA-iTemp101)&16383] + (fTemp100 * fRec102[1])));
			fRec100[0] = fRec102[0];
			float fTemp243 = (fTemp240 + (6.123234e-17f * fTemp241));
			fVec45[IOTA&16383] = fTemp243;
			fRec111[0] = ((((fTemp105 * fVec45[(IOTA-iTemp107)&16383]) / fTemp108) + fVec45[(IOTA-iTemp109)&16383]) + (fTemp110 * fRec111[1]));
			fRec101[0] = fRec111[0];
			float fTemp244 = ((fTemp6 * fRec100[1]) + (fTemp3 * fTemp226));
			float fTemp245 = ((fTemp6 * fRec101[1]) + (fTemp3 * fTemp230));
			float fTemp246 = ((6.123234e-17f * fTemp244) - fTemp245);
			fVec46[IOTA&16383] = fTemp246;
			fRec99[0] = (((fTemp11 * fVec46[(IOTA-iTemp114)&16383]) / fTemp115) + (fVec46[(IOTA-iTemp117)&16383] + (fTemp116 * fRec99[1])));
			fRec97[0] = fRec99[0];
			float fTemp247 = (fTemp244 + (6.123234e-17f * fTemp245));
			fVec47[IOTA&16383] = fTemp247;
			fRec112[0] = (((fTemp121 * fVec47[(IOTA-iTemp123)&16383]) / fTemp124) + (fVec47[(IOTA-iTemp125)&16383] + (fTemp126 * fRec112[1])));
			fRec98[0] = fRec112[0];
			float fTemp248 = ((fTemp6 * fRec97[1]) + fTemp127);
			float fTemp249 = (fTemp128 * fTemp248);
			float fTemp250 = ((fTemp133 * fRec93[1]) + (fTemp128 * (fTemp196 + (fTemp128 * (fTemp163 + (fTemp128 * (fTemp134 + fTemp249)))))));
			float fTemp251 = (fTemp133 * fRec74[1]);
			float fTemp252 = (fTemp133 * fRec54[1]);
			float fTemp253 = (fTemp133 * fRec34[1]);
			float fTemp254 = (fTemp3 * fTemp59);
			float fTemp255 = ((fTemp6 * fRec98[1]) + fTemp254);
			float fTemp256 = (fTemp128 * fTemp255);
			float fTemp257 = ((fTemp133 * fRec94[1]) + (fTemp128 * (fTemp251 + (fTemp128 * (fTemp252 + (fTemp128 * (fTemp253 + fTemp256)))))));
			float fTemp258 = ((6.123234e-17f * fTemp250) - fTemp257);
			fVec48[IOTA&16383] = fTemp258;
			int iTemp259 = int(min(8192, max(0, int(fTemp223))));
			float fTemp260 = (fRec96[0] - fTemp224);
			float fTemp261 = (0 - (fTemp225 / fTemp260));
			int iTemp262 = int((iTemp259 + 1));
			fRec95[0] = (((fTemp225 * fVec48[(IOTA-iTemp259)&16383]) / fTemp260) + ((fTemp261 * fRec95[1]) + fVec48[(IOTA-iTemp262)&16383]));
			fRec93[0] = fRec95[0];
			int iTemp263 = itbl0[iSlow26];
			fRec114[0] = ((0.0001f * iTemp263) + (0.9999f * ((iTemp8 * iTemp263) + fRec114[1])));
			float fTemp264 = (fRec114[0] + -1.49999f);
			float fTemp265 = floorf(fTemp264);
			float fTemp266 = (fTemp265 + (2.0f - fRec114[0]));
			float fTemp267 = (fTemp250 + (6.123234e-17f * fTemp257));
			fVec49[IOTA&16383] = fTemp267;
			int iTemp268 = int(min(8192, max(0, int(fTemp264))));
			float fTemp269 = (fRec114[0] - fTemp265);
			int iTemp270 = int((iTemp268 + 1));
			float fTemp271 = (0 - (fTemp266 / fTemp269));
			fRec113[0] = (((fTemp266 * fVec49[(IOTA-iTemp268)&16383]) / fTemp269) + (fVec49[(IOTA-iTemp270)&16383] + (fTemp271 * fRec113[1])));
			fRec94[0] = fRec113[0];
			float fTemp272 = (fTemp128 * fRec93[1]);
			float fTemp273 = ((fTemp6 * fRec78[1]) + fTemp254);
			float fTemp274 = (fTemp273 * fTemp128);
			float fTemp275 = ((fTemp5 * (fTemp251 + (fTemp128 * (fTemp252 + (fTemp128 * (fTemp274 + fTemp253)))))) + (fTemp128 * fRec94[1]));
			float fTemp276 = ((6.123234e-17f * (fTemp221 + fTemp272)) - fTemp275);
			fVec50[IOTA&16383] = fTemp276;
			int iTemp277 = int(min(8192, max(0, int(fTemp193))));
			float fTemp278 = (fRec76[0] - fTemp194);
			float fTemp279 = (0 - (fTemp195 / fTemp278));
			int iTemp280 = int((iTemp277 + 1));
			fRec75[0] = (((fTemp195 * fVec50[(IOTA-iTemp277)&16383]) / fTemp278) + ((fTemp279 * fRec75[1]) + fVec50[(IOTA-iTemp280)&16383]));
			fRec73[0] = fRec75[0];
			int iTemp281 = itbl0[iSlow27];
			fRec116[0] = ((0.001f * iTemp281) + (0.999f * ((iTemp8 * iTemp281) + fRec116[1])));
			float fTemp282 = (fRec116[0] + -1.49999f);
			float fTemp283 = floorf(fTemp282);
			float fTemp284 = (fTemp283 + (2.0f - fRec116[0]));
			float fTemp285 = ((fTemp221 + (6.123234e-17f * fTemp275)) + fTemp272);
			fVec51[IOTA&16383] = fTemp285;
			int iTemp286 = int(min(8192, max(0, int(fTemp282))));
			float fTemp287 = (fRec116[0] - fTemp283);
			int iTemp288 = int((iTemp286 + 1));
			float fTemp289 = (0 - (fTemp284 / fTemp287));
			fRec115[0] = (((fTemp284 * fVec51[(IOTA-iTemp286)&16383]) / fTemp287) + (fVec51[(IOTA-iTemp288)&16383] + (fTemp289 * fRec115[1])));
			fRec74[0] = fRec115[0];
			float fTemp290 = (fTemp128 * fRec73[1]);
			float fTemp291 = ((fTemp6 * fRec58[1]) + fTemp254);
			float fTemp292 = (fTemp128 * fTemp291);
			float fTemp293 = ((fTemp5 * (fTemp252 + (fTemp128 * (fTemp292 + fTemp253)))) + (fTemp128 * fRec74[1]));
			float fTemp294 = ((6.123234e-17f * (fTemp191 + fTemp290)) - fTemp293);
			fVec52[IOTA&16383] = fTemp294;
			int iTemp295 = int(min(8192, max(0, int(fTemp160))));
			float fTemp296 = (fRec56[0] - fTemp161);
			float fTemp297 = (0 - (fTemp162 / fTemp296));
			int iTemp298 = int((iTemp295 + 1));
			fRec55[0] = (((fTemp162 * fVec52[(IOTA-iTemp295)&16383]) / fTemp296) + ((fTemp297 * fRec55[1]) + fVec52[(IOTA-iTemp298)&16383]));
			fRec53[0] = fRec55[0];
			int iTemp299 = itbl0[iSlow28];
			fRec118[0] = ((0.001f * iTemp299) + (0.999f * ((iTemp299 * iTemp8) + fRec118[1])));
			float fTemp300 = (fRec118[0] + -1.49999f);
			float fTemp301 = floorf(fTemp300);
			float fTemp302 = (fTemp301 + (2.0f - fRec118[0]));
			float fTemp303 = ((fTemp191 + (6.123234e-17f * fTemp293)) + fTemp290);
			fVec53[IOTA&16383] = fTemp303;
			int iTemp304 = int(min(8192, max(0, int(fTemp300))));
			float fTemp305 = (fRec118[0] - fTemp301);
			int iTemp306 = int((iTemp304 + 1));
			float fTemp307 = (0 - (fTemp302 / fTemp305));
			fRec117[0] = (((fTemp302 * fVec53[(IOTA-iTemp304)&16383]) / fTemp305) + (fVec53[(IOTA-iTemp306)&16383] + (fTemp307 * fRec117[1])));
			fRec54[0] = fRec117[0];
			float fTemp308 = (fTemp128 * fRec53[1]);
			float fTemp309 = ((fTemp6 * fRec38[1]) + fTemp254);
			float fTemp310 = ((fTemp5 * (fTemp253 + (fTemp128 * fTemp309))) + (fTemp128 * fRec54[1]));
			float fTemp311 = ((6.123234e-17f * (fTemp158 + fTemp308)) - fTemp310);
			fVec54[IOTA&16383] = fTemp311;
			int iTemp312 = int(min(8192, max(0, int(fTemp130))));
			float fTemp313 = (fRec36[0] - fTemp131);
			float fTemp314 = (0 - (fTemp132 / fTemp313));
			int iTemp315 = int((iTemp312 + 1));
			fRec35[0] = (((fTemp132 * fVec54[(IOTA-iTemp312)&16383]) / fTemp313) + ((fTemp314 * fRec35[1]) + fVec54[(IOTA-iTemp315)&16383]));
			fRec33[0] = fRec35[0];
			int iTemp316 = itbl0[iSlow29];
			fRec120[0] = ((0.001f * iTemp316) + (0.999f * ((iTemp316 * iTemp8) + fRec120[1])));
			float fTemp317 = (fRec120[0] + -1.49999f);
			float fTemp318 = floorf(fTemp317);
			float fTemp319 = (fTemp318 + (2.0f - fRec120[0]));
			float fTemp320 = ((fTemp158 + (6.123234e-17f * fTemp310)) + fTemp308);
			fVec55[IOTA&16383] = fTemp320;
			int iTemp321 = int(min(8192, max(0, int(fTemp317))));
			float fTemp322 = (fRec120[0] - fTemp318);
			int iTemp323 = int((iTemp321 + 1));
			float fTemp324 = (0 - (fTemp319 / fTemp322));
			fRec119[0] = (((fTemp319 * fVec55[(IOTA-iTemp321)&16383]) / fTemp322) + (fVec55[(IOTA-iTemp323)&16383] + (fTemp324 * fRec119[1])));
			fRec34[0] = fRec119[0];
			int iTemp325 = itbl0[iSlow31];
			fRec124[0] = ((0.001f * iTemp325) + (0.999f * ((iTemp325 * iTemp8) + fRec124[1])));
			float fTemp326 = (fRec124[0] + -1.49999f);
			float fTemp327 = floorf(fTemp326);
			float fTemp328 = (fTemp327 + (2.0f - fRec124[0]));
			float fTemp329 = (fTemp12 * fRec121[1]);
			float fTemp330 = (fTemp133 * fRec131[1]);
			float fTemp331 = (fTemp133 * fRec128[1]);
			float fTemp332 = (fTemp133 * fRec125[1]);
			float fTemp333 = ((fTemp12 * fRec137[1]) + fTemp31);
			float fTemp334 = ((fTemp12 * fRec140[1]) + (fTemp6 * fTemp333));
			float fTemp335 = ((fTemp12 * fRec143[1]) + (fTemp6 * fTemp334));
			float fTemp336 = ((fTemp12 * fRec146[1]) + (fTemp6 * fTemp335));
			float fTemp337 = (fTemp60 + (fTemp12 * fRec138[1]));
			float fTemp338 = ((fTemp12 * fRec141[1]) + (fTemp6 * fTemp337));
			float fTemp339 = ((fTemp12 * fRec144[1]) + (fTemp6 * fTemp338));
			float fTemp340 = ((fTemp12 * fRec147[1]) + (fTemp6 * fTemp339));
			float fTemp341 = ((6.123234e-17f * fTemp336) - fTemp340);
			fVec56[IOTA&16383] = fTemp341;
			fRec148[0] = (((fTemp49 * fVec56[(IOTA-iTemp66)&16383]) / fTemp67) + ((fTemp68 * fRec148[1]) + fVec56[(IOTA-iTemp69)&16383]));
			fRec146[0] = fRec148[0];
			float fTemp342 = (fTemp336 + (6.123234e-17f * fTemp340));
			fVec57[IOTA&16383] = fTemp342;
			fRec149[0] = (((fTemp73 * fVec57[(IOTA-iTemp75)&16383]) / fTemp76) + (fVec57[(IOTA-iTemp77)&16383] + (fTemp78 * fRec149[1])));
			fRec147[0] = fRec149[0];
			float fTemp343 = ((fTemp6 * fRec146[1]) + (fTemp3 * fTemp335));
			float fTemp344 = ((fTemp6 * fRec147[1]) + (fTemp3 * fTemp339));
			float fTemp345 = ((6.123234e-17f * fTemp343) - fTemp344);
			fVec58[IOTA&16383] = fTemp345;
			fRec145[0] = (((fTemp43 * fVec58[(IOTA-iTemp82)&16383]) / fTemp83) + (fVec58[(IOTA-iTemp85)&16383] + (fTemp84 * fRec145[1])));
			fRec143[0] = fRec145[0];
			float fTemp346 = (fTemp343 + (6.123234e-17f * fTemp344));
			fVec59[IOTA&16383] = fTemp346;
			fRec150[0] = (((fTemp89 * fVec59[(IOTA-iTemp91)&16383]) / fTemp92) + (fVec59[(IOTA-iTemp93)&16383] + (fTemp94 * fRec150[1])));
			fRec144[0] = fRec150[0];
			float fTemp347 = ((fTemp6 * fRec143[1]) + (fTemp3 * fTemp334));
			float fTemp348 = ((fTemp6 * fRec144[1]) + (fTemp3 * fTemp338));
			float fTemp349 = ((6.123234e-17f * fTemp347) - fTemp348);
			fVec60[IOTA&16383] = fTemp349;
			fRec142[0] = (((fTemp37 * fVec60[(IOTA-iTemp98)&16383]) / fTemp99) + (fVec60[(IOTA-iTemp101)&16383] + (fTemp100 * fRec142[1])));
			fRec140[0] = fRec142[0];
			float fTemp350 = (fTemp347 + (6.123234e-17f * fTemp348));
			fVec61[IOTA&16383] = fTemp350;
			fRec151[0] = (((fTemp105 * fVec61[(IOTA-iTemp107)&16383]) / fTemp108) + (fVec61[(IOTA-iTemp109)&16383] + (fTemp110 * fRec151[1])));
			fRec141[0] = fRec151[0];
			float fTemp351 = ((fTemp6 * fRec140[1]) + (fTemp3 * fTemp333));
			float fTemp352 = ((fTemp6 * fRec141[1]) + (fTemp3 * fTemp337));
			float fTemp353 = ((6.123234e-17f * fTemp351) - fTemp352);
			fVec62[IOTA&16383] = fTemp353;
			fRec139[0] = (((fTemp11 * fVec62[(IOTA-iTemp114)&16383]) / fTemp115) + (fVec62[(IOTA-iTemp117)&16383] + (fTemp116 * fRec139[1])));
			fRec137[0] = fRec139[0];
			float fTemp354 = (fTemp351 + (6.123234e-17f * fTemp352));
			fVec63[IOTA&16383] = fTemp354;
			fRec152[0] = (((fTemp121 * fVec63[(IOTA-iTemp123)&16383]) / fTemp124) + (fVec63[(IOTA-iTemp125)&16383] + (fTemp126 * fRec152[1])));
			fRec138[0] = fRec152[0];
			float fTemp355 = (fTemp128 * ((fTemp6 * fRec137[1]) + fTemp127));
			float fTemp356 = ((fTemp133 * fRec134[1]) + (fTemp128 * (fTemp330 + (fTemp128 * (fTemp331 + (fTemp128 * (fTemp332 + fTemp355)))))));
			float fTemp357 = (fTemp133 * fRec126[1]);
			float fTemp358 = (fTemp128 * ((fTemp6 * fRec138[1]) + fTemp254));
			float fTemp359 = (fTemp133 * fRec129[1]);
			float fTemp360 = (fTemp133 * fRec132[1]);
			float fTemp361 = ((fTemp128 * ((fTemp128 * ((fTemp128 * (fTemp357 + fTemp358)) + fTemp359)) + fTemp360)) + (fTemp133 * fRec135[1]));
			float fTemp362 = ((6.123234e-17f * fTemp356) - fTemp361);
			fVec64[IOTA&16383] = fTemp362;
			fRec136[0] = (((fTemp225 * fVec64[(IOTA-iTemp259)&16383]) / fTemp260) + ((fTemp261 * fRec136[1]) + fVec64[(IOTA-iTemp262)&16383]));
			fRec134[0] = fRec136[0];
			float fTemp363 = (fTemp356 + (6.123234e-17f * fTemp361));
			fVec65[IOTA&16383] = fTemp363;
			fRec153[0] = ((((fTemp266 * fVec65[(IOTA-iTemp268)&16383]) / fTemp269) + fVec65[(IOTA-iTemp270)&16383]) + (fTemp271 * fRec153[1]));
			fRec135[0] = fRec153[0];
			float fTemp364 = ((fTemp128 * fRec134[1]) + (fTemp5 * (fTemp330 + (fTemp128 * (fTemp331 + (fTemp128 * (fTemp332 + fTemp249)))))));
			float fTemp365 = ((fTemp128 * fRec135[1]) + (fTemp5 * (fTemp360 + (fTemp128 * (fTemp359 + (fTemp128 * (fTemp357 + fTemp256)))))));
			float fTemp366 = ((6.123234e-17f * fTemp364) - fTemp365);
			fVec66[IOTA&16383] = fTemp366;
			fRec133[0] = (((fTemp195 * fVec66[(IOTA-iTemp277)&16383]) / fTemp278) + ((fTemp279 * fRec133[1]) + fVec66[(IOTA-iTemp280)&16383]));
			fRec131[0] = fRec133[0];
			float fTemp367 = (fTemp364 + (6.123234e-17f * fTemp365));
			fVec67[IOTA&16383] = fTemp367;
			fRec154[0] = ((((fTemp284 * fVec67[(IOTA-iTemp286)&16383]) / fTemp287) + fVec67[(IOTA-iTemp288)&16383]) + (fTemp289 * fRec154[1]));
			fRec132[0] = fRec154[0];
			float fTemp368 = ((fTemp128 * fRec131[1]) + (fTemp5 * (fTemp331 + (fTemp128 * (fTemp220 + fTemp332)))));
			float fTemp369 = ((fTemp128 * fRec132[1]) + (fTemp5 * (fTemp359 + (fTemp128 * (fTemp274 + fTemp357)))));
			float fTemp370 = ((6.123234e-17f * fTemp368) - fTemp369);
			fVec68[IOTA&16383] = fTemp370;
			fRec130[0] = (((fTemp162 * fVec68[(IOTA-iTemp295)&16383]) / fTemp296) + ((fTemp297 * fRec130[1]) + fVec68[(IOTA-iTemp298)&16383]));
			fRec128[0] = fRec130[0];
			float fTemp371 = (fTemp368 + (6.123234e-17f * fTemp369));
			fVec69[IOTA&16383] = fTemp371;
			fRec155[0] = ((((fTemp302 * fVec69[(IOTA-iTemp304)&16383]) / fTemp305) + fVec69[(IOTA-iTemp306)&16383]) + (fTemp307 * fRec155[1]));
			fRec129[0] = fRec155[0];
			float fTemp372 = ((fTemp128 * fRec128[1]) + (fTemp5 * (fTemp332 + fTemp190)));
			float fTemp373 = ((fTemp128 * fRec129[1]) + (fTemp5 * (fTemp357 + fTemp292)));
			float fTemp374 = ((6.123234e-17f * fTemp372) - fTemp373);
			fVec70[IOTA&16383] = fTemp374;
			fRec127[0] = (((fTemp132 * fVec70[(IOTA-iTemp312)&16383]) / fTemp313) + ((fTemp314 * fRec127[1]) + fVec70[(IOTA-iTemp315)&16383]));
			fRec125[0] = fRec127[0];
			float fTemp375 = (fTemp372 + (6.123234e-17f * fTemp373));
			fVec71[IOTA&16383] = fTemp375;
			fRec156[0] = ((((fTemp319 * fVec71[(IOTA-iTemp321)&16383]) / fTemp322) + fVec71[(IOTA-iTemp323)&16383]) + (fTemp324 * fRec156[1]));
			fRec126[0] = fRec156[0];
			float fTemp376 = (fTemp3 * (fTemp329 + (fTemp6 * ((fTemp128 * fRec125[1]) + (fTemp5 * fTemp157)))));
			int iTemp377 = itbl0[iSlow33];
			fRec163[0] = ((0.001f * iTemp377) + (0.999f * ((iTemp8 * iTemp377) + fRec163[1])));
			float fTemp378 = (fRec163[0] + -1.49999f);
			float fTemp379 = floorf(fTemp378);
			float fTemp380 = (fTemp379 + (2.0f - fRec163[0]));
			int iTemp381 = itbl0[iSlow35];
			fRec167[0] = ((0.0001f * iTemp381) + (0.9999f * ((iTemp381 * iTemp8) + fRec167[1])));
			float fTemp382 = (fRec167[0] + -1.49999f);
			float fTemp383 = floorf(fTemp382);
			float fTemp384 = (fTemp383 + (2.0f - fRec167[0]));
			float fTemp385 = (fTemp12 * fRec160[1]);
			float fTemp386 = (fTemp12 * fRec157[1]);
			float fTemp387 = (fTemp133 * fRec174[1]);
			float fTemp388 = (fTemp133 * fRec171[1]);
			float fTemp389 = (fTemp133 * fRec168[1]);
			float fTemp390 = ((fTemp12 * fRec180[1]) + fTemp31);
			float fTemp391 = ((fTemp6 * fTemp390) + (fTemp12 * fRec183[1]));
			float fTemp392 = (fTemp391 * fTemp3);
			float fTemp393 = ((fTemp6 * fTemp391) + (fTemp12 * fRec186[1]));
			float fTemp394 = (fTemp393 * fTemp3);
			float fTemp395 = ((fTemp6 * fTemp393) + (fTemp12 * fRec189[1]));
			float fTemp396 = ((fTemp12 * fRec181[1]) + fTemp60);
			float fTemp397 = ((fTemp12 * fRec184[1]) + (fTemp6 * fTemp396));
			float fTemp398 = ((fTemp12 * fRec187[1]) + (fTemp6 * fTemp397));
			float fTemp399 = ((fTemp12 * fRec190[1]) + (fTemp6 * fTemp398));
			float fTemp400 = ((6.123234e-17f * fTemp395) - fTemp399);
			fVec72[IOTA&16383] = fTemp400;
			fRec191[0] = ((((fVec72[(IOTA-iTemp66)&16383] * fTemp49) / fTemp67) + fVec72[(IOTA-iTemp69)&16383]) + (fTemp68 * fRec191[1]));
			fRec189[0] = fRec191[0];
			float fTemp401 = (fTemp395 + (6.123234e-17f * fTemp399));
			fVec73[IOTA&16383] = fTemp401;
			fRec192[0] = (((fVec73[(IOTA-iTemp75)&16383] * fTemp73) / fTemp76) + (fVec73[(IOTA-iTemp77)&16383] + (fTemp78 * fRec192[1])));
			fRec190[0] = fRec192[0];
			float fTemp402 = (fTemp6 * fRec189[1]);
			float fTemp403 = ((fTemp398 * fTemp3) + (fTemp6 * fRec190[1]));
			float fTemp404 = ((6.123234e-17f * (fTemp394 + fTemp402)) - fTemp403);
			fVec74[IOTA&16383] = fTemp404;
			fRec188[0] = (((fVec74[(IOTA-iTemp82)&16383] * fTemp43) / fTemp83) + ((fTemp84 * fRec188[1]) + fVec74[(IOTA-iTemp85)&16383]));
			fRec186[0] = fRec188[0];
			float fTemp405 = ((fTemp394 + (6.123234e-17f * fTemp403)) + fTemp402);
			fVec75[IOTA&16383] = fTemp405;
			fRec193[0] = (((fVec75[(IOTA-iTemp91)&16383] * fTemp89) / fTemp92) + (fVec75[(IOTA-iTemp93)&16383] + (fTemp94 * fRec193[1])));
			fRec187[0] = fRec193[0];
			float fTemp406 = (fTemp6 * fRec186[1]);
			float fTemp407 = ((fTemp397 * fTemp3) + (fTemp6 * fRec187[1]));
			float fTemp408 = ((6.123234e-17f * (fTemp392 + fTemp406)) - fTemp407);
			fVec76[IOTA&16383] = fTemp408;
			fRec185[0] = (((fVec76[(IOTA-iTemp98)&16383] * fTemp37) / fTemp99) + (fVec76[(IOTA-iTemp101)&16383] + (fTemp100 * fRec185[1])));
			fRec183[0] = fRec185[0];
			float fTemp409 = ((fTemp392 + (6.123234e-17f * fTemp407)) + fTemp406);
			fVec77[IOTA&16383] = fTemp409;
			fRec194[0] = (((fVec77[(IOTA-iTemp107)&16383] * fTemp105) / fTemp108) + (fVec77[(IOTA-iTemp109)&16383] + (fTemp110 * fRec194[1])));
			fRec184[0] = fRec194[0];
			float fTemp410 = ((fTemp390 * fTemp3) + (fTemp6 * fRec183[1]));
			float fTemp411 = ((fTemp6 * fRec184[1]) + (fTemp396 * fTemp3));
			float fTemp412 = ((6.123234e-17f * fTemp410) - fTemp411);
			fVec78[IOTA&16383] = fTemp412;
			fRec182[0] = (((fTemp11 * fVec78[(IOTA-iTemp114)&16383]) / fTemp115) + (fVec78[(IOTA-iTemp117)&16383] + (fTemp116 * fRec182[1])));
			fRec180[0] = fRec182[0];
			float fTemp413 = (fTemp410 + (6.123234e-17f * fTemp411));
			fVec79[IOTA&16383] = fTemp413;
			fRec195[0] = (((fTemp121 * fVec79[(IOTA-iTemp123)&16383]) / fTemp124) + (fVec79[(IOTA-iTemp125)&16383] + (fTemp126 * fRec195[1])));
			fRec181[0] = fRec195[0];
			float fTemp414 = ((fTemp133 * fRec177[1]) + (fTemp128 * (fTemp387 + (fTemp128 * (fTemp388 + (fTemp128 * (fTemp389 + (fTemp128 * ((fTemp6 * fRec180[1]) + fTemp127)))))))));
			float fTemp415 = (fTemp133 * fRec175[1]);
			float fTemp416 = (fTemp133 * fRec172[1]);
			float fTemp417 = (fTemp133 * fRec169[1]);
			float fTemp418 = ((fTemp133 * fRec178[1]) + (fTemp128 * (fTemp415 + (fTemp128 * (fTemp416 + (fTemp128 * (fTemp417 + (fTemp128 * ((fTemp6 * fRec181[1]) + fTemp254)))))))));
			float fTemp419 = ((6.123234e-17f * fTemp414) - fTemp418);
			fVec80[IOTA&16383] = fTemp419;
			fRec179[0] = (((fTemp225 * fVec80[(IOTA-iTemp259)&16383]) / fTemp260) + ((fTemp261 * fRec179[1]) + fVec80[(IOTA-iTemp262)&16383]));
			fRec177[0] = fRec179[0];
			float fTemp420 = (fTemp414 + (6.123234e-17f * fTemp418));
			fVec81[IOTA&16383] = fTemp420;
			fRec196[0] = (((fTemp266 * fVec81[(IOTA-iTemp268)&16383]) / fTemp269) + (fVec81[(IOTA-iTemp270)&16383] + (fTemp271 * fRec196[1])));
			fRec178[0] = fRec196[0];
			float fTemp421 = ((fTemp12 * fRec197[1]) + fTemp31);
			float fTemp422 = ((fTemp12 * fRec200[1]) + (fTemp6 * fTemp421));
			float fTemp423 = ((fTemp12 * fRec203[1]) + (fTemp6 * fTemp422));
			float fTemp424 = ((fTemp12 * fRec206[1]) + (fTemp6 * fTemp423));
			float fTemp425 = (fTemp60 + (fTemp12 * fRec198[1]));
			float fTemp426 = ((fTemp12 * fRec201[1]) + (fTemp6 * fTemp425));
			float fTemp427 = ((fTemp12 * fRec204[1]) + (fTemp6 * fTemp426));
			float fTemp428 = ((fTemp12 * fRec207[1]) + (fTemp6 * fTemp427));
			float fTemp429 = ((6.123234e-17f * fTemp424) - fTemp428);
			fVec82[IOTA&16383] = fTemp429;
			fRec208[0] = (((fTemp49 * fVec82[(IOTA-iTemp66)&16383]) / fTemp67) + ((fTemp68 * fRec208[1]) + fVec82[(IOTA-iTemp69)&16383]));
			fRec206[0] = fRec208[0];
			float fTemp430 = (fTemp424 + (6.123234e-17f * fTemp428));
			fVec83[IOTA&16383] = fTemp430;
			fRec209[0] = (((fTemp73 * fVec83[(IOTA-iTemp75)&16383]) / fTemp76) + ((fTemp78 * fRec209[1]) + fVec83[(IOTA-iTemp77)&16383]));
			fRec207[0] = fRec209[0];
			float fTemp431 = ((fTemp6 * fRec206[1]) + (fTemp3 * fTemp423));
			float fTemp432 = ((fTemp6 * fRec207[1]) + (fTemp3 * fTemp427));
			float fTemp433 = ((6.123234e-17f * fTemp431) - fTemp432);
			fVec84[IOTA&16383] = fTemp433;
			fRec205[0] = (((fTemp43 * fVec84[(IOTA-iTemp82)&16383]) / fTemp83) + (fVec84[(IOTA-iTemp85)&16383] + (fTemp84 * fRec205[1])));
			fRec203[0] = fRec205[0];
			float fTemp434 = (fTemp431 + (6.123234e-17f * fTemp432));
			fVec85[IOTA&16383] = fTemp434;
			fRec210[0] = (((fTemp89 * fVec85[(IOTA-iTemp91)&16383]) / fTemp92) + (fVec85[(IOTA-iTemp93)&16383] + (fTemp94 * fRec210[1])));
			fRec204[0] = fRec210[0];
			float fTemp435 = ((fTemp6 * fRec203[1]) + (fTemp3 * fTemp422));
			float fTemp436 = ((fTemp6 * fRec204[1]) + (fTemp3 * fTemp426));
			float fTemp437 = ((6.123234e-17f * fTemp435) - fTemp436);
			fVec86[IOTA&16383] = fTemp437;
			fRec202[0] = (((fTemp37 * fVec86[(IOTA-iTemp98)&16383]) / fTemp99) + (fVec86[(IOTA-iTemp101)&16383] + (fTemp100 * fRec202[1])));
			fRec200[0] = fRec202[0];
			float fTemp438 = (fTemp435 + (6.123234e-17f * fTemp436));
			fVec87[IOTA&16383] = fTemp438;
			fRec211[0] = (((fTemp105 * fVec87[(IOTA-iTemp107)&16383]) / fTemp108) + ((fTemp110 * fRec211[1]) + fVec87[(IOTA-iTemp109)&16383]));
			fRec201[0] = fRec211[0];
			float fTemp439 = ((fTemp6 * fRec200[1]) + (fTemp3 * fTemp421));
			float fTemp440 = ((fTemp6 * fRec201[1]) + (fTemp3 * fTemp425));
			float fTemp441 = ((6.123234e-17f * fTemp439) - fTemp440);
			fVec88[IOTA&16383] = fTemp441;
			fRec199[0] = (((fTemp11 * fVec88[(IOTA-iTemp114)&16383]) / fTemp115) + (fVec88[(IOTA-iTemp117)&16383] + (fTemp116 * fRec199[1])));
			fRec197[0] = fRec199[0];
			float fTemp442 = (fTemp439 + (6.123234e-17f * fTemp440));
			fVec89[IOTA&16383] = fTemp442;
			fRec212[0] = (((fTemp121 * fVec89[(IOTA-iTemp123)&16383]) / fTemp124) + (fVec89[(IOTA-iTemp125)&16383] + (fTemp126 * fRec212[1])));
			fRec198[0] = fRec212[0];
			float fTemp443 = (fTemp128 * ((fTemp6 * fRec197[1]) + fTemp127));
			float fTemp444 = ((fTemp128 * fRec177[1]) + (fTemp5 * (fTemp387 + (fTemp128 * (fTemp388 + (fTemp128 * (fTemp443 + fTemp389)))))));
			float fTemp445 = (fTemp128 * ((fTemp6 * fRec198[1]) + fTemp254));
			float fTemp446 = ((fTemp128 * fRec178[1]) + (fTemp5 * (fTemp415 + (fTemp128 * (fTemp416 + (fTemp128 * (fTemp445 + fTemp417)))))));
			float fTemp447 = ((6.123234e-17f * fTemp444) - fTemp446);
			fVec90[IOTA&16383] = fTemp447;
			fRec176[0] = (((fTemp195 * fVec90[(IOTA-iTemp277)&16383]) / fTemp278) + (fVec90[(IOTA-iTemp280)&16383] + (fTemp279 * fRec176[1])));
			fRec174[0] = fRec176[0];
			float fTemp448 = (fTemp444 + (6.123234e-17f * fTemp446));
			fVec91[IOTA&16383] = fTemp448;
			fRec213[0] = (((fTemp284 * fVec91[(IOTA-iTemp286)&16383]) / fTemp287) + (fVec91[(IOTA-iTemp288)&16383] + (fTemp289 * fRec213[1])));
			fRec175[0] = fRec213[0];
			float fTemp449 = ((fTemp12 * fRec214[1]) + fTemp31);
			float fTemp450 = ((fTemp12 * fRec217[1]) + (fTemp6 * fTemp449));
			float fTemp451 = ((fTemp12 * fRec220[1]) + (fTemp6 * fTemp450));
			float fTemp452 = ((fTemp12 * fRec223[1]) + (fTemp6 * fTemp451));
			float fTemp453 = (fTemp60 + (fTemp12 * fRec215[1]));
			float fTemp454 = ((fTemp12 * fRec218[1]) + (fTemp6 * fTemp453));
			float fTemp455 = ((fTemp12 * fRec221[1]) + (fTemp6 * fTemp454));
			float fTemp456 = ((fTemp12 * fRec224[1]) + (fTemp6 * fTemp455));
			float fTemp457 = ((6.123234e-17f * fTemp452) - fTemp456);
			fVec92[IOTA&16383] = fTemp457;
			fRec225[0] = (((fTemp49 * fVec92[(IOTA-iTemp66)&16383]) / fTemp67) + ((fTemp68 * fRec225[1]) + fVec92[(IOTA-iTemp69)&16383]));
			fRec223[0] = fRec225[0];
			float fTemp458 = (fTemp452 + (6.123234e-17f * fTemp456));
			fVec93[IOTA&16383] = fTemp458;
			fRec226[0] = (((fTemp73 * fVec93[(IOTA-iTemp75)&16383]) / fTemp76) + (fVec93[(IOTA-iTemp77)&16383] + (fTemp78 * fRec226[1])));
			fRec224[0] = fRec226[0];
			float fTemp459 = ((fTemp6 * fRec223[1]) + (fTemp3 * fTemp451));
			float fTemp460 = ((fTemp6 * fRec224[1]) + (fTemp3 * fTemp455));
			float fTemp461 = ((6.123234e-17f * fTemp459) - fTemp460);
			fVec94[IOTA&16383] = fTemp461;
			fRec222[0] = (((fTemp43 * fVec94[(IOTA-iTemp82)&16383]) / fTemp83) + (fVec94[(IOTA-iTemp85)&16383] + (fTemp84 * fRec222[1])));
			fRec220[0] = fRec222[0];
			float fTemp462 = (fTemp459 + (6.123234e-17f * fTemp460));
			fVec95[IOTA&16383] = fTemp462;
			fRec227[0] = (((fTemp89 * fVec95[(IOTA-iTemp91)&16383]) / fTemp92) + ((fTemp94 * fRec227[1]) + fVec95[(IOTA-iTemp93)&16383]));
			fRec221[0] = fRec227[0];
			float fTemp463 = ((fTemp6 * fRec220[1]) + (fTemp3 * fTemp450));
			float fTemp464 = ((fTemp6 * fRec221[1]) + (fTemp3 * fTemp454));
			float fTemp465 = ((6.123234e-17f * fTemp463) - fTemp464);
			fVec96[IOTA&16383] = fTemp465;
			fRec219[0] = (((fTemp37 * fVec96[(IOTA-iTemp98)&16383]) / fTemp99) + (fVec96[(IOTA-iTemp101)&16383] + (fTemp100 * fRec219[1])));
			fRec217[0] = fRec219[0];
			float fTemp466 = (fTemp463 + (6.123234e-17f * fTemp464));
			fVec97[IOTA&16383] = fTemp466;
			fRec228[0] = (((fTemp105 * fVec97[(IOTA-iTemp107)&16383]) / fTemp108) + (fVec97[(IOTA-iTemp109)&16383] + (fTemp110 * fRec228[1])));
			fRec218[0] = fRec228[0];
			float fTemp467 = ((fTemp6 * fRec217[1]) + (fTemp3 * fTemp449));
			float fTemp468 = ((fTemp6 * fRec218[1]) + (fTemp3 * fTemp453));
			float fTemp469 = ((6.123234e-17f * fTemp467) - fTemp468);
			fVec98[IOTA&16383] = fTemp469;
			fRec216[0] = (((fTemp11 * fVec98[(IOTA-iTemp114)&16383]) / fTemp115) + (fVec98[(IOTA-iTemp117)&16383] + (fTemp116 * fRec216[1])));
			fRec214[0] = fRec216[0];
			float fTemp470 = (fTemp467 + (6.123234e-17f * fTemp468));
			fVec99[IOTA&16383] = fTemp470;
			fRec229[0] = (((fTemp121 * fVec99[(IOTA-iTemp123)&16383]) / fTemp124) + (fVec99[(IOTA-iTemp125)&16383] + (fTemp126 * fRec229[1])));
			fRec215[0] = fRec229[0];
			float fTemp471 = (fTemp128 * ((fTemp6 * fRec214[1]) + fTemp127));
			float fTemp472 = ((fTemp128 * fRec174[1]) + (fTemp5 * (fTemp388 + (fTemp128 * (fTemp471 + fTemp389)))));
			float fTemp473 = (fTemp128 * ((fTemp6 * fRec215[1]) + fTemp254));
			float fTemp474 = ((fTemp128 * fRec175[1]) + (fTemp5 * (fTemp416 + (fTemp128 * (fTemp473 + fTemp417)))));
			float fTemp475 = ((6.123234e-17f * fTemp472) - fTemp474);
			fVec100[IOTA&16383] = fTemp475;
			fRec173[0] = (((fTemp162 * fVec100[(IOTA-iTemp295)&16383]) / fTemp296) + (fVec100[(IOTA-iTemp298)&16383] + (fTemp297 * fRec173[1])));
			fRec171[0] = fRec173[0];
			float fTemp476 = (fTemp472 + (6.123234e-17f * fTemp474));
			fVec101[IOTA&16383] = fTemp476;
			fRec230[0] = (((fTemp302 * fVec101[(IOTA-iTemp304)&16383]) / fTemp305) + (fVec101[(IOTA-iTemp306)&16383] + (fTemp307 * fRec230[1])));
			fRec172[0] = fRec230[0];
			float fTemp477 = ((fTemp128 * fRec171[1]) + (fTemp5 * (fTemp355 + fTemp389)));
			float fTemp478 = ((fTemp128 * fRec172[1]) + (fTemp5 * (fTemp358 + fTemp417)));
			float fTemp479 = ((6.123234e-17f * fTemp477) - fTemp478);
			fVec102[IOTA&16383] = fTemp479;
			fRec170[0] = (((fTemp132 * fVec102[(IOTA-iTemp312)&16383]) / fTemp313) + (fVec102[(IOTA-iTemp315)&16383] + (fTemp314 * fRec170[1])));
			fRec168[0] = fRec170[0];
			float fTemp480 = (fTemp477 + (6.123234e-17f * fTemp478));
			fVec103[IOTA&16383] = fTemp480;
			fRec231[0] = (((fTemp319 * fVec103[(IOTA-iTemp321)&16383]) / fTemp322) + (fVec103[(IOTA-iTemp323)&16383] + (fTemp324 * fRec231[1])));
			fRec169[0] = fRec231[0];
			float fTemp481 = ((fTemp12 * fRec164[1]) + (fTemp6 * (fTemp385 + (fTemp6 * (fTemp386 + (fTemp6 * (fTemp329 + (fTemp6 * ((fTemp128 * fRec168[1]) + (fTemp5 * fTemp248))))))))));
			float fTemp482 = (fTemp12 * fRec161[1]);
			float fTemp483 = (fTemp12 * fRec158[1]);
			float fTemp484 = (fTemp12 * fRec122[1]);
			float fTemp485 = ((fTemp12 * fRec165[1]) + (fTemp6 * (fTemp482 + (fTemp6 * (fTemp483 + (fTemp6 * (fTemp484 + (fTemp6 * ((fTemp128 * fRec169[1]) + (fTemp5 * fTemp255))))))))));
			float fTemp486 = ((6.123234e-17f * fTemp481) - fTemp485);
			fVec104[IOTA&16383] = fTemp486;
			int iTemp487 = int(min(8192, max(0, int(fTemp382))));
			float fTemp488 = (fRec167[0] - fTemp383);
			fRec166[0] = (((fTemp384 * fVec104[(IOTA-iTemp487)&16383]) / fTemp488) + ((fRec166[1] * (0 - (fTemp384 / fTemp488))) + fVec104[(IOTA-int((iTemp487 + 1)))&16383]));
			fRec164[0] = fRec166[0];
			int iTemp489 = itbl0[iSlow36];
			fRec233[0] = ((0.0001f * iTemp489) + (0.9999f * ((iTemp8 * iTemp489) + fRec233[1])));
			float fTemp490 = (fRec233[0] + -1.49999f);
			float fTemp491 = floorf(fTemp490);
			float fTemp492 = (fTemp491 + (2.0f - fRec233[0]));
			float fTemp493 = (fRec233[0] - fTemp491);
			float fTemp494 = (fTemp481 + (6.123234e-17f * fTemp485));
			fVec105[IOTA&16383] = fTemp494;
			int iTemp495 = int(min(8192, max(0, int(fTemp490))));
			fRec232[0] = (((fRec232[1] * (0 - (fTemp492 / fTemp493))) + fVec105[(IOTA-int((iTemp495 + 1)))&16383]) + ((fTemp492 * fVec105[(IOTA-iTemp495)&16383]) / fTemp493));
			fRec165[0] = fRec232[0];
			float fTemp496 = (fTemp6 * fRec164[1]);
			float fTemp497 = (fTemp133 * fRec240[1]);
			float fTemp498 = (fTemp133 * fRec237[1]);
			float fTemp499 = (fTemp133 * fRec234[1]);
			float fTemp500 = ((fTemp133 * fRec243[1]) + (fTemp128 * (fTemp497 + (fTemp128 * (fTemp498 + (fTemp128 * (fTemp499 + fTemp443)))))));
			float fTemp501 = (fTemp133 * fRec235[1]);
			float fTemp502 = (fTemp133 * fRec238[1]);
			float fTemp503 = (fTemp133 * fRec241[1]);
			float fTemp504 = ((fTemp128 * ((fTemp128 * ((fTemp128 * (fTemp501 + fTemp445)) + fTemp502)) + fTemp503)) + (fTemp133 * fRec244[1]));
			float fTemp505 = ((6.123234e-17f * fTemp500) - fTemp504);
			fVec106[IOTA&16383] = fTemp505;
			fRec245[0] = (((fTemp225 * fVec106[(IOTA-iTemp259)&16383]) / fTemp260) + ((fRec245[1] * fTemp261) + fVec106[(IOTA-iTemp262)&16383]));
			fRec243[0] = fRec245[0];
			float fTemp506 = (fTemp500 + (6.123234e-17f * fTemp504));
			fVec107[IOTA&16383] = fTemp506;
			fRec246[0] = (fVec107[(IOTA-iTemp270)&16383] + ((fRec246[1] * fTemp271) + ((fTemp266 * fVec107[(IOTA-iTemp268)&16383]) / fTemp269)));
			fRec244[0] = fRec246[0];
			float fTemp507 = ((fTemp128 * fRec243[1]) + (fTemp5 * (fTemp497 + (fTemp128 * (fTemp498 + (fTemp128 * (fTemp499 + fTemp471)))))));
			float fTemp508 = ((fTemp128 * fRec244[1]) + (fTemp5 * (fTemp503 + (fTemp128 * (fTemp502 + (fTemp128 * (fTemp501 + fTemp473)))))));
			float fTemp509 = ((6.123234e-17f * fTemp507) - fTemp508);
			fVec108[IOTA&16383] = fTemp509;
			fRec242[0] = (((fTemp195 * fVec108[(IOTA-iTemp277)&16383]) / fTemp278) + ((fRec242[1] * fTemp279) + fVec108[(IOTA-iTemp280)&16383]));
			fRec240[0] = fRec242[0];
			float fTemp510 = (fTemp507 + (6.123234e-17f * fTemp508));
			fVec109[IOTA&16383] = fTemp510;
			fRec247[0] = (fVec109[(IOTA-iTemp288)&16383] + ((fRec247[1] * fTemp289) + ((fTemp284 * fVec109[(IOTA-iTemp286)&16383]) / fTemp287)));
			fRec241[0] = fRec247[0];
			float fTemp511 = ((fTemp128 * fRec240[1]) + (fTemp5 * (fTemp498 + (fTemp128 * (fTemp499 + fTemp355)))));
			float fTemp512 = ((fTemp128 * fRec241[1]) + (fTemp5 * (fTemp502 + (fTemp128 * (fTemp501 + fTemp358)))));
			float fTemp513 = ((6.123234e-17f * fTemp511) - fTemp512);
			fVec110[IOTA&16383] = fTemp513;
			fRec239[0] = (((fTemp162 * fVec110[(IOTA-iTemp295)&16383]) / fTemp296) + ((fRec239[1] * fTemp297) + fVec110[(IOTA-iTemp298)&16383]));
			fRec237[0] = fRec239[0];
			float fTemp514 = (fTemp511 + (6.123234e-17f * fTemp512));
			fVec111[IOTA&16383] = fTemp514;
			fRec248[0] = (fVec111[(IOTA-iTemp306)&16383] + ((fRec248[1] * fTemp307) + ((fTemp302 * fVec111[(IOTA-iTemp304)&16383]) / fTemp305)));
			fRec238[0] = fRec248[0];
			float fTemp515 = ((fTemp128 * fRec237[1]) + (fTemp5 * (fTemp499 + fTemp249)));
			float fTemp516 = ((fTemp128 * fRec238[1]) + (fTemp5 * (fTemp501 + fTemp256)));
			float fTemp517 = ((6.123234e-17f * fTemp515) - fTemp516);
			fVec112[IOTA&16383] = fTemp517;
			fRec236[0] = (((fTemp132 * fVec112[(IOTA-iTemp312)&16383]) / fTemp313) + ((fRec236[1] * fTemp314) + fVec112[(IOTA-iTemp315)&16383]));
			fRec234[0] = fRec236[0];
			float fTemp518 = (fTemp515 + (6.123234e-17f * fTemp516));
			fVec113[IOTA&16383] = fTemp518;
			fRec249[0] = (fVec113[(IOTA-iTemp323)&16383] + ((fRec249[1] * fTemp324) + ((fTemp319 * fVec113[(IOTA-iTemp321)&16383]) / fTemp322)));
			fRec235[0] = fRec249[0];
			float fTemp519 = (fTemp3 * (fTemp385 + (fTemp6 * (fTemp386 + (fTemp6 * (fTemp329 + (fTemp6 * ((fTemp5 * fTemp219) + (fTemp128 * fRec234[1])))))))));
			float fTemp520 = ((fTemp6 * fRec165[1]) + (fTemp3 * ((fTemp6 * ((fTemp6 * ((fTemp6 * ((fTemp128 * fRec235[1]) + (fTemp273 * fTemp5))) + fTemp484)) + fTemp483)) + fTemp482)));
			float fTemp521 = ((6.123234e-17f * (fTemp496 + fTemp519)) - fTemp520);
			fVec114[IOTA&16383] = fTemp521;
			int iTemp522 = int(min(8192, max(0, int(fTemp378))));
			float fTemp523 = (fRec163[0] - fTemp379);
			fRec162[0] = (((fTemp380 * fVec114[(IOTA-iTemp522)&16383]) / fTemp523) + ((fRec162[1] * (0 - (fTemp380 / fTemp523))) + fVec114[(IOTA-int((iTemp522 + 1)))&16383]));
			fRec160[0] = fRec162[0];
			int iTemp524 = itbl0[iSlow37];
			fRec251[0] = ((0.001f * iTemp524) + (0.999f * ((iTemp8 * iTemp524) + fRec251[1])));
			float fTemp525 = (fRec251[0] + -1.49999f);
			float fTemp526 = floorf(fTemp525);
			float fTemp527 = (fTemp526 + (2.0f - fRec251[0]));
			float fTemp528 = ((fTemp496 + (6.123234e-17f * fTemp520)) + fTemp519);
			fVec115[IOTA&16383] = fTemp528;
			int iTemp529 = int(min(8192, max(0, int(fTemp525))));
			float fTemp530 = (fRec251[0] - fTemp526);
			fRec250[0] = (((fTemp527 * fVec115[(IOTA-iTemp529)&16383]) / fTemp530) + ((fRec250[1] * (0 - (fTemp527 / fTemp530))) + fVec115[(IOTA-int((iTemp529 + 1)))&16383]));
			fRec161[0] = fRec250[0];
			float fTemp531 = (fTemp133 * fRec258[1]);
			float fTemp532 = (fTemp133 * fRec255[1]);
			float fTemp533 = (fTemp133 * fRec252[1]);
			float fTemp534 = ((fTemp133 * fRec261[1]) + (fTemp128 * (fTemp531 + (fTemp128 * (fTemp532 + (fTemp128 * (fTemp533 + fTemp471)))))));
			float fTemp535 = (fTemp133 * fRec253[1]);
			float fTemp536 = (fTemp133 * fRec256[1]);
			float fTemp537 = (fTemp133 * fRec259[1]);
			float fTemp538 = ((fTemp128 * ((fTemp128 * ((fTemp128 * (fTemp535 + fTemp473)) + fTemp536)) + fTemp537)) + (fTemp133 * fRec262[1]));
			float fTemp539 = ((6.123234e-17f * fTemp534) - fTemp538);
			fVec116[IOTA&16383] = fTemp539;
			fRec263[0] = (((fTemp225 * fVec116[(IOTA-iTemp259)&16383]) / fTemp260) + ((fTemp261 * fRec263[1]) + fVec116[(IOTA-iTemp262)&16383]));
			fRec261[0] = fRec263[0];
			float fTemp540 = (fTemp534 + (6.123234e-17f * fTemp538));
			fVec117[IOTA&16383] = fTemp540;
			fRec264[0] = ((((fTemp266 * fVec117[(IOTA-iTemp268)&16383]) / fTemp269) + fVec117[(IOTA-iTemp270)&16383]) + (fTemp271 * fRec264[1]));
			fRec262[0] = fRec264[0];
			float fTemp541 = ((fTemp128 * fRec261[1]) + (fTemp5 * (fTemp531 + (fTemp128 * (fTemp532 + (fTemp128 * (fTemp533 + fTemp355)))))));
			float fTemp542 = ((fTemp128 * fRec262[1]) + (fTemp5 * (fTemp537 + (fTemp128 * (fTemp536 + (fTemp128 * (fTemp535 + fTemp358)))))));
			float fTemp543 = ((6.123234e-17f * fTemp541) - fTemp542);
			fVec118[IOTA&16383] = fTemp543;
			fRec260[0] = (((fTemp195 * fVec118[(IOTA-iTemp277)&16383]) / fTemp278) + ((fTemp279 * fRec260[1]) + fVec118[(IOTA-iTemp280)&16383]));
			fRec258[0] = fRec260[0];
			float fTemp544 = (fTemp541 + (6.123234e-17f * fTemp542));
			fVec119[IOTA&16383] = fTemp544;
			fRec265[0] = ((((fTemp284 * fVec119[(IOTA-iTemp286)&16383]) / fTemp287) + fVec119[(IOTA-iTemp288)&16383]) + (fTemp289 * fRec265[1]));
			fRec259[0] = fRec265[0];
			float fTemp545 = ((fTemp128 * fRec258[1]) + (fTemp5 * (fTemp532 + (fTemp128 * (fTemp533 + fTemp249)))));
			float fTemp546 = ((fTemp128 * fRec259[1]) + (fTemp5 * (fTemp536 + (fTemp128 * (fTemp535 + fTemp256)))));
			float fTemp547 = ((6.123234e-17f * fTemp545) - fTemp546);
			fVec120[IOTA&16383] = fTemp547;
			fRec257[0] = (((fTemp162 * fVec120[(IOTA-iTemp295)&16383]) / fTemp296) + ((fTemp297 * fRec257[1]) + fVec120[(IOTA-iTemp298)&16383]));
			fRec255[0] = fRec257[0];
			float fTemp548 = (fTemp545 + (6.123234e-17f * fTemp546));
			fVec121[IOTA&16383] = fTemp548;
			fRec266[0] = ((((fTemp302 * fVec121[(IOTA-iTemp304)&16383]) / fTemp305) + fVec121[(IOTA-iTemp306)&16383]) + (fTemp307 * fRec266[1]));
			fRec256[0] = fRec266[0];
			float fTemp549 = ((fTemp128 * fRec255[1]) + (fTemp5 * (fTemp533 + fTemp220)));
			float fTemp550 = ((fTemp128 * fRec256[1]) + (fTemp5 * (fTemp535 + fTemp274)));
			float fTemp551 = ((6.123234e-17f * fTemp549) - fTemp550);
			fVec122[IOTA&16383] = fTemp551;
			fRec254[0] = (((fTemp132 * fVec122[(IOTA-iTemp312)&16383]) / fTemp313) + ((fTemp314 * fRec254[1]) + fVec122[(IOTA-iTemp315)&16383]));
			fRec252[0] = fRec254[0];
			float fTemp552 = (fTemp549 + (6.123234e-17f * fTemp550));
			fVec123[IOTA&16383] = fTemp552;
			fRec267[0] = ((((fTemp319 * fVec123[(IOTA-iTemp321)&16383]) / fTemp322) + fVec123[(IOTA-iTemp323)&16383]) + (fTemp324 * fRec267[1]));
			fRec253[0] = fRec267[0];
			float fTemp553 = ((fTemp6 * fRec160[1]) + (fTemp3 * (fTemp386 + (fTemp6 * (fTemp329 + (fTemp6 * ((fTemp128 * fRec252[1]) + (fTemp5 * fTemp189))))))));
			float fTemp554 = ((fTemp3 * ((fTemp6 * ((fTemp6 * ((fTemp128 * fRec253[1]) + (fTemp5 * fTemp291))) + fTemp484)) + fTemp483)) + (fTemp6 * fRec161[1]));
			float fTemp555 = ((6.123234e-17f * fTemp553) - fTemp554);
			fVec124[IOTA&16383] = fTemp555;
			int iTemp556 = itbl0[iSlow39];
			fRec268[0] = ((0.001f * iTemp556) + (0.999f * ((iTemp556 * iTemp8) + fRec268[1])));
			float fTemp557 = (fRec268[0] + -1.49999f);
			int iTemp558 = int(min(8192, max(0, int(fTemp557))));
			float fTemp559 = floorf(fTemp557);
			float fTemp560 = (fTemp559 + (2.0f - fRec268[0]));
			float fTemp561 = (fRec268[0] - fTemp559);
			fRec159[0] = ((fVec124[(IOTA-int((iTemp558 + 1)))&16383] + ((fVec124[(IOTA-iTemp558)&16383] * fTemp560) / fTemp561)) + (fRec159[1] * (0 - (fTemp560 / fTemp561))));
			fRec157[0] = fRec159[0];
			float fTemp562 = (fTemp553 + (6.123234e-17f * fTemp554));
			fVec125[IOTA&16383] = fTemp562;
			int iTemp563 = itbl0[iSlow40];
			fRec270[0] = ((0.001f * iTemp563) + (0.999f * ((iTemp563 * iTemp8) + fRec270[1])));
			float fTemp564 = (fRec270[0] + -1.49999f);
			int iTemp565 = int(min(8192, max(0, int(fTemp564))));
			float fTemp566 = floorf(fTemp564);
			float fTemp567 = (fTemp566 + (2.0f - fRec270[0]));
			float fTemp568 = (fRec270[0] - fTemp566);
			fRec269[0] = ((fVec125[(IOTA-int((iTemp565 + 1)))&16383] + ((fVec125[(IOTA-iTemp565)&16383] * fTemp567) / fTemp568)) + (fRec269[1] * (0 - (fTemp567 / fTemp568))));
			fRec158[0] = fRec269[0];
			float fTemp569 = (fTemp6 * fRec157[1]);
			float fTemp570 = ((fTemp6 * fRec158[1]) + (fTemp3 * ((fTemp6 * ((fTemp128 * fRec126[1]) + (fTemp5 * fTemp309))) + fTemp484)));
			float fTemp571 = ((6.123234e-17f * (fTemp376 + fTemp569)) - fTemp570);
			fVec126[IOTA&16383] = fTemp571;
			int iTemp572 = int(min(8192, max(0, int(fTemp326))));
			float fTemp573 = (fRec124[0] - fTemp327);
			fRec123[0] = (((fTemp328 * fVec126[(IOTA-iTemp572)&16383]) / fTemp573) + (fVec126[(IOTA-int((iTemp572 + 1)))&16383] + ((0 - (fTemp328 / fTemp573)) * fRec123[1])));
			fRec121[0] = fRec123[0];
			float fTemp574 = ((fTemp376 + (6.123234e-17f * fTemp570)) + fTemp569);
			fVec127[IOTA&16383] = fTemp574;
			int iTemp575 = itbl0[iSlow41];
			fRec272[0] = ((0.001f * iTemp575) + (0.999f * ((iTemp8 * iTemp575) + fRec272[1])));
			float fTemp576 = (fRec272[0] + -1.49999f);
			int iTemp577 = int(min(8192, max(0, int(fTemp576))));
			float fTemp578 = floorf(fTemp576);
			float fTemp579 = (fTemp578 + (2.0f - fRec272[0]));
			float fTemp580 = (fRec272[0] - fTemp578);
			fRec271[0] = ((fVec127[(IOTA-int((iTemp577 + 1)))&16383] + ((fVec127[(IOTA-iTemp577)&16383] * fTemp579) / fTemp580)) + (fRec271[1] * (0 - (fTemp579 / fTemp580))));
			fRec122[0] = fRec271[0];
			fRec2[0] = ((fTemp1 * ((fTemp3 * ((fTemp5 * ((fTemp6 * fRec3[1]) + fTemp127)) + (fTemp128 * fRec33[1]))) + (fTemp6 * fRec121[1]))) + (0.5f * (fTemp0 * fRec2[1])));
			fRec0[IOTA&1023] = fRec2[0];
			fRec273[0] = ((0.5f * (fTemp0 * fRec273[1])) + (fTemp1 * ((fTemp3 * ((fTemp5 * ((fTemp6 * fRec4[1]) + fTemp254)) + (fTemp128 * fRec34[1]))) + (fTemp6 * fRec122[1]))));
			fRec1[IOTA&1023] = fRec273[0];
			output0[i] = (FAUSTFLOAT)fRec0[(IOTA-0)&1023];
			output1[i] = (FAUSTFLOAT)fRec1[(IOTA-0)&1023];
			// post processing
			fRec273[1] = fRec273[0];
			fRec2[1] = fRec2[0];
			fRec122[1] = fRec122[0];
			fRec271[1] = fRec271[0];
			fRec272[1] = fRec272[0];
			fRec121[1] = fRec121[0];
			fRec123[1] = fRec123[0];
			fRec158[1] = fRec158[0];
			fRec269[1] = fRec269[0];
			fRec270[1] = fRec270[0];
			fRec157[1] = fRec157[0];
			fRec159[1] = fRec159[0];
			fRec268[1] = fRec268[0];
			fRec253[1] = fRec253[0];
			fRec267[1] = fRec267[0];
			fRec252[1] = fRec252[0];
			fRec254[1] = fRec254[0];
			fRec256[1] = fRec256[0];
			fRec266[1] = fRec266[0];
			fRec255[1] = fRec255[0];
			fRec257[1] = fRec257[0];
			fRec259[1] = fRec259[0];
			fRec265[1] = fRec265[0];
			fRec258[1] = fRec258[0];
			fRec260[1] = fRec260[0];
			fRec262[1] = fRec262[0];
			fRec264[1] = fRec264[0];
			fRec261[1] = fRec261[0];
			fRec263[1] = fRec263[0];
			fRec161[1] = fRec161[0];
			fRec250[1] = fRec250[0];
			fRec251[1] = fRec251[0];
			fRec160[1] = fRec160[0];
			fRec162[1] = fRec162[0];
			fRec235[1] = fRec235[0];
			fRec249[1] = fRec249[0];
			fRec234[1] = fRec234[0];
			fRec236[1] = fRec236[0];
			fRec238[1] = fRec238[0];
			fRec248[1] = fRec248[0];
			fRec237[1] = fRec237[0];
			fRec239[1] = fRec239[0];
			fRec241[1] = fRec241[0];
			fRec247[1] = fRec247[0];
			fRec240[1] = fRec240[0];
			fRec242[1] = fRec242[0];
			fRec244[1] = fRec244[0];
			fRec246[1] = fRec246[0];
			fRec243[1] = fRec243[0];
			fRec245[1] = fRec245[0];
			fRec165[1] = fRec165[0];
			fRec232[1] = fRec232[0];
			fRec233[1] = fRec233[0];
			fRec164[1] = fRec164[0];
			fRec166[1] = fRec166[0];
			fRec169[1] = fRec169[0];
			fRec231[1] = fRec231[0];
			fRec168[1] = fRec168[0];
			fRec170[1] = fRec170[0];
			fRec172[1] = fRec172[0];
			fRec230[1] = fRec230[0];
			fRec171[1] = fRec171[0];
			fRec173[1] = fRec173[0];
			fRec215[1] = fRec215[0];
			fRec229[1] = fRec229[0];
			fRec214[1] = fRec214[0];
			fRec216[1] = fRec216[0];
			fRec218[1] = fRec218[0];
			fRec228[1] = fRec228[0];
			fRec217[1] = fRec217[0];
			fRec219[1] = fRec219[0];
			fRec221[1] = fRec221[0];
			fRec227[1] = fRec227[0];
			fRec220[1] = fRec220[0];
			fRec222[1] = fRec222[0];
			fRec224[1] = fRec224[0];
			fRec226[1] = fRec226[0];
			fRec223[1] = fRec223[0];
			fRec225[1] = fRec225[0];
			fRec175[1] = fRec175[0];
			fRec213[1] = fRec213[0];
			fRec174[1] = fRec174[0];
			fRec176[1] = fRec176[0];
			fRec198[1] = fRec198[0];
			fRec212[1] = fRec212[0];
			fRec197[1] = fRec197[0];
			fRec199[1] = fRec199[0];
			fRec201[1] = fRec201[0];
			fRec211[1] = fRec211[0];
			fRec200[1] = fRec200[0];
			fRec202[1] = fRec202[0];
			fRec204[1] = fRec204[0];
			fRec210[1] = fRec210[0];
			fRec203[1] = fRec203[0];
			fRec205[1] = fRec205[0];
			fRec207[1] = fRec207[0];
			fRec209[1] = fRec209[0];
			fRec206[1] = fRec206[0];
			fRec208[1] = fRec208[0];
			fRec178[1] = fRec178[0];
			fRec196[1] = fRec196[0];
			fRec177[1] = fRec177[0];
			fRec179[1] = fRec179[0];
			fRec181[1] = fRec181[0];
			fRec195[1] = fRec195[0];
			fRec180[1] = fRec180[0];
			fRec182[1] = fRec182[0];
			fRec184[1] = fRec184[0];
			fRec194[1] = fRec194[0];
			fRec183[1] = fRec183[0];
			fRec185[1] = fRec185[0];
			fRec187[1] = fRec187[0];
			fRec193[1] = fRec193[0];
			fRec186[1] = fRec186[0];
			fRec188[1] = fRec188[0];
			fRec190[1] = fRec190[0];
			fRec192[1] = fRec192[0];
			fRec189[1] = fRec189[0];
			fRec191[1] = fRec191[0];
			fRec167[1] = fRec167[0];
			fRec163[1] = fRec163[0];
			fRec126[1] = fRec126[0];
			fRec156[1] = fRec156[0];
			fRec125[1] = fRec125[0];
			fRec127[1] = fRec127[0];
			fRec129[1] = fRec129[0];
			fRec155[1] = fRec155[0];
			fRec128[1] = fRec128[0];
			fRec130[1] = fRec130[0];
			fRec132[1] = fRec132[0];
			fRec154[1] = fRec154[0];
			fRec131[1] = fRec131[0];
			fRec133[1] = fRec133[0];
			fRec135[1] = fRec135[0];
			fRec153[1] = fRec153[0];
			fRec134[1] = fRec134[0];
			fRec136[1] = fRec136[0];
			fRec138[1] = fRec138[0];
			fRec152[1] = fRec152[0];
			fRec137[1] = fRec137[0];
			fRec139[1] = fRec139[0];
			fRec141[1] = fRec141[0];
			fRec151[1] = fRec151[0];
			fRec140[1] = fRec140[0];
			fRec142[1] = fRec142[0];
			fRec144[1] = fRec144[0];
			fRec150[1] = fRec150[0];
			fRec143[1] = fRec143[0];
			fRec145[1] = fRec145[0];
			fRec147[1] = fRec147[0];
			fRec149[1] = fRec149[0];
			fRec146[1] = fRec146[0];
			fRec148[1] = fRec148[0];
			fRec124[1] = fRec124[0];
			fRec34[1] = fRec34[0];
			fRec119[1] = fRec119[0];
			fRec120[1] = fRec120[0];
			fRec33[1] = fRec33[0];
			fRec35[1] = fRec35[0];
			fRec54[1] = fRec54[0];
			fRec117[1] = fRec117[0];
			fRec118[1] = fRec118[0];
			fRec53[1] = fRec53[0];
			fRec55[1] = fRec55[0];
			fRec74[1] = fRec74[0];
			fRec115[1] = fRec115[0];
			fRec116[1] = fRec116[0];
			fRec73[1] = fRec73[0];
			fRec75[1] = fRec75[0];
			fRec94[1] = fRec94[0];
			fRec113[1] = fRec113[0];
			fRec114[1] = fRec114[0];
			fRec93[1] = fRec93[0];
			fRec95[1] = fRec95[0];
			fRec98[1] = fRec98[0];
			fRec112[1] = fRec112[0];
			fRec97[1] = fRec97[0];
			fRec99[1] = fRec99[0];
			fRec101[1] = fRec101[0];
			fRec111[1] = fRec111[0];
			fRec100[1] = fRec100[0];
			fRec102[1] = fRec102[0];
			fRec104[1] = fRec104[0];
			fRec110[1] = fRec110[0];
			fRec103[1] = fRec103[0];
			fRec105[1] = fRec105[0];
			fRec107[1] = fRec107[0];
			fRec109[1] = fRec109[0];
			fRec106[1] = fRec106[0];
			fRec108[1] = fRec108[0];
			fRec96[1] = fRec96[0];
			fRec78[1] = fRec78[0];
			fRec92[1] = fRec92[0];
			fRec77[1] = fRec77[0];
			fRec79[1] = fRec79[0];
			fRec81[1] = fRec81[0];
			fRec91[1] = fRec91[0];
			fRec80[1] = fRec80[0];
			fRec82[1] = fRec82[0];
			fRec84[1] = fRec84[0];
			fRec90[1] = fRec90[0];
			fRec83[1] = fRec83[0];
			fRec85[1] = fRec85[0];
			fRec87[1] = fRec87[0];
			fRec89[1] = fRec89[0];
			fRec86[1] = fRec86[0];
			fRec88[1] = fRec88[0];
			fRec76[1] = fRec76[0];
			fRec58[1] = fRec58[0];
			fRec72[1] = fRec72[0];
			fRec57[1] = fRec57[0];
			fRec59[1] = fRec59[0];
			fRec61[1] = fRec61[0];
			fRec71[1] = fRec71[0];
			fRec60[1] = fRec60[0];
			fRec62[1] = fRec62[0];
			fRec64[1] = fRec64[0];
			fRec70[1] = fRec70[0];
			fRec63[1] = fRec63[0];
			fRec65[1] = fRec65[0];
			fRec67[1] = fRec67[0];
			fRec69[1] = fRec69[0];
			fRec66[1] = fRec66[0];
			fRec68[1] = fRec68[0];
			fRec56[1] = fRec56[0];
			fRec38[1] = fRec38[0];
			fRec52[1] = fRec52[0];
			fRec37[1] = fRec37[0];
			fRec39[1] = fRec39[0];
			fRec41[1] = fRec41[0];
			fRec51[1] = fRec51[0];
			fRec40[1] = fRec40[0];
			fRec42[1] = fRec42[0];
			fRec44[1] = fRec44[0];
			fRec50[1] = fRec50[0];
			fRec43[1] = fRec43[0];
			fRec45[1] = fRec45[0];
			fRec47[1] = fRec47[0];
			fRec49[1] = fRec49[0];
			fRec46[1] = fRec46[0];
			fRec48[1] = fRec48[0];
			fRec36[1] = fRec36[0];
			fRec4[1] = fRec4[0];
			fRec31[1] = fRec31[0];
			fRec32[1] = fRec32[0];
			fRec3[1] = fRec3[0];
			fRec5[1] = fRec5[0];
			fRec14[1] = fRec14[0];
			fRec29[1] = fRec29[0];
			fRec30[1] = fRec30[0];
			fRec13[1] = fRec13[0];
			fRec15[1] = fRec15[0];
			fRec18[1] = fRec18[0];
			fRec27[1] = fRec27[0];
			fRec28[1] = fRec28[0];
			fRec17[1] = fRec17[0];
			fRec19[1] = fRec19[0];
			fRec22[1] = fRec22[0];
			fRec25[1] = fRec25[0];
			fRec26[1] = fRec26[0];
			fRec21[1] = fRec21[0];
			fRec23[1] = fRec23[0];
			fRec24[1] = fRec24[0];
			fRec20[1] = fRec20[0];
			fRec16[1] = fRec16[0];
			IOTA = IOTA+1;
			fRec12[1] = fRec12[0];
			fRec11[1] = fRec11[0];
			fVec5[1] = fVec5[0];
			fVec4[1] = fVec4[0];
			fRec10[1] = fRec10[0];
			fRec9[1] = fRec9[0];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
			fVec3[1] = fVec3[0];
			fRec6[1] = fRec6[0];
			fVec2[1] = fVec2[0];
			iVec1[1] = iVec1[0];
			fVec0[1] = fVec0[0];
		}
	}
};


int 	mydsp::SIG0::iWave0[1302] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,1051,1061,1063,1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,1153,1163,1171,1181,1187,1193,1201,1213,1217,1223,1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,1297,1301,1303,1307,1319,1321,1327,1361,1367,1373,1381,1399,1409,1423,1427,1429,1433,1439,1447,1451,1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,1523,1531,1543,1549,1553,1559,1567,1571,1579,1583,1597,1601,1607,1609,1613,1619,1621,1627,1637,1657,1663,1667,1669,1693,1697,1699,1709,1721,1723,1733,1741,1747,1753,1759,1777,1783,1787,1789,1801,1811,1823,1831,1847,1861,1867,1871,1873,1877,1879,1889,1901,1907,1913,1931,1933,1949,1951,1973,1979,1987,1993,1997,1999,2003,2011,2017,2027,2029,2039,2053,2063,2069,2081,2083,2087,2089,2099,2111,2113,2129,2131,2137,2141,2143,2153,2161,2179,2203,2207,2213,2221,2237,2239,2243,2251,2267,2269,2273,2281,2287,2293,2297,2309,2311,2333,2339,2341,2347,2351,2357,2371,2377,2381,2383,2389,2393,2399,2411,2417,2423,2437,2441,2447,2459,2467,2473,2477,2503,2521,2531,2539,2543,2549,2551,2557,2579,2591,2593,2609,2617,2621,2633,2647,2657,2659,2663,2671,2677,2683,2687,2689,2693,2699,2707,2711,2713,2719,2729,2731,2741,2749,2753,2767,2777,2789,2791,2797,2801,2803,2819,2833,2837,2843,2851,2857,2861,2879,2887,2897,2903,2909,2917,2927,2939,2953,2957,2963,2969,2971,2999,3001,3011,3019,3023,3037,3041,3049,3061,3067,3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,3181,3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,3259,3271,3299,3301,3307,3313,3319,3323,3329,3331,3343,3347,3359,3361,3371,3373,3389,3391,3407,3413,3433,3449,3457,3461,3463,3467,3469,3491,3499,3511,3517,3527,3529,3533,3539,3541,3547,3557,3559,3571,3581,3583,3593,3607,3613,3617,3623,3631,3637,3643,3659,3671,3673,3677,3691,3697,3701,3709,3719,3727,3733,3739,3761,3767,3769,3779,3793,3797,3803,3821,3823,3833,3847,3851,3853,3863,3877,3881,3889,3907,3911,3917,3919,3923,3929,3931,3943,3947,3967,3989,4001,4003,4007,4013,4019,4021,4027,4049,4051,4057,4073,4079,4091,4093,4099,4111,4127,4129,4133,4139,4153,4157,4159,4177,4201,4211,4217,4219,4229,4231,4241,4243,4253,4259,4261,4271,4273,4283,4289,4297,4327,4337,4339,4349,4357,4363,4373,4391,4397,4409,4421,4423,4441,4447,4451,4457,4463,4481,4483,4493,4507,4513,4517,4519,4523,4547,4549,4561,4567,4583,4591,4597,4603,4621,4637,4639,4643,4649,4651,4657,4663,4673,4679,4691,4703,4721,4723,4729,4733,4751,4759,4783,4787,4789,4793,4799,4801,4813,4817,4831,4861,4871,4877,4889,4903,4909,4919,4931,4933,4937,4943,4951,4957,4967,4969,4973,4987,4993,4999,5003,5009,5011,5021,5023,5039,5051,5059,5077,5081,5087,5099,5101,5107,5113,5119,5147,5153,5167,5171,5179,5189,5197,5209,5227,5231,5233,5237,5261,5273,5279,5281,5297,5303,5309,5323,5333,5347,5351,5381,5387,5393,5399,5407,5413,5417,5419,5431,5437,5441,5443,5449,5471,5477,5479,5483,5501,5503,5507,5519,5521,5527,5531,5557,5563,5569,5573,5581,5591,5623,5639,5641,5647,5651,5653,5657,5659,5669,5683,5689,5693,5701,5711,5717,5737,5741,5743,5749,5779,5783,5791,5801,5807,5813,5821,5827,5839,5843,5849,5851,5857,5861,5867,5869,5879,5881,5897,5903,5923,5927,5939,5953,5981,5987,6007,6011,6029,6037,6043,6047,6053,6067,6073,6079,6089,6091,6101,6113,6121,6131,6133,6143,6151,6163,6173,6197,6199,6203,6211,6217,6221,6229,6247,6257,6263,6269,6271,6277,6287,6299,6301,6311,6317,6323,6329,6337,6343,6353,6359,6361,6367,6373,6379,6389,6397,6421,6427,6449,6451,6469,6473,6481,6491,6521,6529,6547,6551,6553,6563,6569,6571,6577,6581,6599,6607,6619,6637,6653,6659,6661,6673,6679,6689,6691,6701,6703,6709,6719,6733,6737,6761,6763,6779,6781,6791,6793,6803,6823,6827,6829,6833,6841,6857,6863,6869,6871,6883,6899,6907,6911,6917,6947,6949,6959,6961,6967,6971,6977,6983,6991,6997,7001,7013,7019,7027,7039,7043,7057,7069,7079,7103,7109,7121,7127,7129,7151,7159,7177,7187,7193,7207,7211,7213,7219,7229,7237,7243,7247,7253,7283,7297,7307,7309,7321,7331,7333,7349,7351,7369,7393,7411,7417,7433,7451,7457,7459,7477,7481,7487,7489,7499,7507,7517,7523,7529,7537,7541,7547,7549,7559,7561,7573,7577,7583,7589,7591,7603,7607,7621,7639,7643,7649,7669,7673,7681,7687,7691,7699,7703,7717,7723,7727,7741,7753,7757,7759,7789,7793,7817,7823,7829,7841,7853,7867,7873,7877,7879,7883,7901,7907,7919,7927,7933,7937,7949,7951,7963,7993,8009,8011,8017,8039,8053,8059,8069,8081,8087,8089,8093,8101,8111,8117,8123,8147,8161,8167,8171,8179,8191,8209,8219,8221,8231,8233,8237,8243,8263,8269,8273,8287,8291,8293,8297,8311,8317,8329,8353,8363,8369,8377,8387,8389,8419,8423,8429,8431,8443,8447,8461,8467,8501,8513,8521,8527,8537,8539,8543,8563,8573,8581,8597,8599,8609,8623,8627,8629,8641,8647,8663,8669,8677,8681,8689,8693,8699,8707,8713,8719,8731,8737,8741,8747,8753,8761,8779,8783,8803,8807,8819,8821,8831,8837,8839,8849,8861,8863,8867,8887,8893,8923,8929,8933,8941,8951,8963,8969,8971,8999,9001,9007,9011,9013,9029,9041,9043,9049,9059,9067,9091,9103,9109,9127,9133,9137,9151,9157,9161,9173,9181,9187,9199,9203,9209,9221,9227,9239,9241,9257,9277,9281,9283,9293,9311,9319,9323,9337,9341,9343,9349,9371,9377,9391,9397,9403,9413,9419,9421,9431,9433,9437,9439,9461,9463,9467,9473,9479,9491,9497,9511,9521,9533,9539,9547,9551,9587,9601,9613,9619,9623,9629,9631,9643,9649,9661,9677,9679,9689,9697,9719,9721,9733,9739,9743,9749,9767,9769,9781,9787,9791,9803,9811,9817,9829,9833,9839,9851,9857,9859,9871,9883,9887,9901,9907,9923,9929,9931,9941,9949,9967,9973,10007,10009,10037,10039,10061,10067,10069,10079,10091,10093,10099,10103,10111,10133,10139,10141,10151,10159,10163,10169,10177,10181,10193,10211,10223,10243,10247,10253,10259,10267,10271,10273,10289,10301,10303,10313,10321,10331,10333,10337,10343,10357,10369,10391,10399,10427,10429,10433,10453,10457,10459,10463,10477,10487,10499,10501,10513,10529,10531,10559,10567,10589,10597,10601,10607,10613,10627,10631,10639,10651,10657,10663,10667};
int 	mydsp::itbl0[1302];

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
