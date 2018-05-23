//----------------------------------------------------------
// name: "GreyholeRaw"
// version: "1.0"
// author: "Julian Parker, bug fixes by Till Bovermann"
// license: "GPL2+"
// copyright: "(c) Julian Parker 2013"
//
// Code generated with Faust 0.12.0 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with : "primes" */
#include <jprev.h>
#include <math.h>
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

// If other than 'faust2sc --prefix Faust' is used, sed this as well:
#if !defined(SC_FAUST_PREFIX)
#define SC_FAUST_PREFIX "Faust"
#endif

#include <map>
#include <string>
#include <string.h>
#include <SC_PlugIn.h>

#include <faust/dsp/dsp.h>
#include <faust/gui/UI.h>
#include <faust/misc.h>

using namespace std;

#if defined(__GNUC__) && __GNUC__ >= 4
    #define FAUST_EXPORT __attribute__((visibility("default")))
#else
    #define FAUST_EXPORT  SC_API_EXPORT
#endif

#ifdef WIN32
    #define STRDUP _strdup
#else
    #define STRDUP strdup
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
	float 	fVec0[2];
	int 	iVec1[2];
	float 	fTempPerm0;
	FAUSTFLOAT 	fslider1;
	float 	fVec2[2];
	float 	fTempPerm1;
	float 	fTempPerm2;
	float 	fTempPerm3;
	float 	fTempPerm4;
	float 	fTempPerm5;
	float 	fTempPerm6;
	float 	fTempPerm7;
	float 	fTempPerm8;
	FAUSTFLOAT 	fslider2;
	float 	fVec3[2];
	float 	fTempPerm9;
	float 	fConst0;
	float 	fConst1;
	FAUSTFLOAT 	fslider3;
	float 	fVec4[2];
	float 	fTempPerm10;
	float 	fConst2;
	FAUSTFLOAT 	fslider4;
	float 	fVec5[2];
	float 	fTempPerm11;
	float 	fTempPerm12;
	float 	fTempPerm13;
	float 	fRec27[2];
	int 	iTempPerm14;
	float 	fRec28[2];
	float 	fTempPerm15;
	float 	fTempPerm16;
	int 	iTempPerm17;
	float 	fTempPerm18;
	float 	fTempPerm19;
	float 	fTempPerm20;
	float 	fTempPerm21;
	float 	fTempPerm22;
	float 	fTempPerm23;
	float 	fTempPerm24;
	int 	IOTA;
	float 	fVec6[131072];
	FAUSTFLOAT 	fslider5;
	float 	fTempPerm25;
	float 	fRec29[2];
	float 	fRec30[2];
	float 	fRec31[2];
	float 	fRec32[2];
	int 	iTempPerm26;
	int 	iTempPerm27;
	float 	fTempPerm28;
	float 	fTempPerm29;
	float 	fTempPerm30;
	float 	fTempPerm31;
	float 	fTempPerm32;
	float 	fTempPerm33;
	float 	fVec7[16384];
	FAUSTFLOAT 	fslider6;
	float 	fRec45[2];
	float 	fTempPerm34;
	int 	iTempPerm35;
	float 	fTempPerm36;
	float 	fTempPerm37;
	float 	fTempPerm38;
	float 	fRec44[2];
	float 	fRec42[2];
	float 	fTempPerm39;
	float 	fTempPerm40;
	float 	fTempPerm41;
	float 	fTempPerm42;
	float 	fTempPerm43;
	float 	fTempPerm44;
	float 	fTempPerm45;
	int 	iTempPerm46;
	float 	fTempPerm47;
	float 	fTempPerm48;
	float 	fVec8[131072];
	float 	fTempPerm49;
	float 	fTempPerm50;
	float 	fTempPerm51;
	float 	fTempPerm52;
	float 	fTempPerm53;
	float 	fVec9[16384];
	float 	fRec47[2];
	float 	fTempPerm54;
	int 	iTempPerm55;
	float 	fTempPerm56;
	float 	fTempPerm57;
	float 	fTempPerm58;
	float 	fRec46[2];
	float 	fRec43[2];
	float 	fTempPerm59;
	float 	fVec10[16384];
	float 	fRec48[2];
	float 	fTempPerm60;
	int 	iTempPerm61;
	float 	fTempPerm62;
	float 	fTempPerm63;
	float 	fTempPerm64;
	float 	fRec41[2];
	float 	fRec39[2];
	float 	fTempPerm65;
	float 	fVec11[16384];
	float 	fRec50[2];
	float 	fTempPerm66;
	int 	iTempPerm67;
	float 	fTempPerm68;
	float 	fTempPerm69;
	float 	fTempPerm70;
	float 	fRec49[2];
	float 	fRec40[2];
	float 	fTempPerm71;
	float 	fVec12[16384];
	float 	fRec51[2];
	float 	fTempPerm72;
	int 	iTempPerm73;
	float 	fTempPerm74;
	float 	fTempPerm75;
	float 	fTempPerm76;
	float 	fRec38[2];
	float 	fRec36[2];
	float 	fTempPerm77;
	float 	fVec13[16384];
	float 	fRec53[2];
	float 	fTempPerm78;
	int 	iTempPerm79;
	float 	fTempPerm80;
	float 	fTempPerm81;
	float 	fTempPerm82;
	float 	fRec52[2];
	float 	fRec37[2];
	float 	fTempPerm83;
	float 	fVec14[16384];
	float 	fRec54[2];
	float 	fTempPerm84;
	int 	iTempPerm85;
	float 	fTempPerm86;
	float 	fTempPerm87;
	float 	fTempPerm88;
	float 	fRec35[2];
	float 	fRec33[2];
	float 	fTempPerm89;
	float 	fVec15[16384];
	float 	fRec56[2];
	float 	fTempPerm90;
	int 	iTempPerm91;
	float 	fTempPerm92;
	float 	fTempPerm93;
	float 	fTempPerm94;
	float 	fRec55[2];
	float 	fRec34[2];
	float 	fTempPerm95;
	float 	fTempPerm96;
	float 	fTempPerm97;
	float 	fTempPerm98;
	float 	fTempPerm99;
	float 	fVec16[16384];
	float 	fRec57[2];
	float 	fTempPerm100;
	int 	iTempPerm101;
	float 	fTempPerm102;
	float 	fTempPerm103;
	float 	fTempPerm104;
	float 	fRec26[2];
	float 	fRec24[2];
	float 	fTempPerm105;
	float 	fTempPerm106;
	float 	fTempPerm107;
	float 	fTempPerm108;
	float 	fTempPerm109;
	float 	fVec17[16384];
	float 	fRec59[2];
	float 	fTempPerm110;
	int 	iTempPerm111;
	float 	fTempPerm112;
	float 	fTempPerm113;
	float 	fTempPerm114;
	float 	fRec58[2];
	float 	fRec25[2];
	float 	fTempPerm115;
	float 	fVec18[16384];
	float 	fRec60[2];
	float 	fTempPerm116;
	int 	iTempPerm117;
	float 	fTempPerm118;
	float 	fTempPerm119;
	float 	fTempPerm120;
	float 	fRec23[2];
	float 	fRec21[2];
	float 	fTempPerm121;
	float 	fVec19[16384];
	float 	fRec62[2];
	float 	fTempPerm122;
	int 	iTempPerm123;
	float 	fTempPerm124;
	float 	fTempPerm125;
	float 	fTempPerm126;
	float 	fRec61[2];
	float 	fRec22[2];
	float 	fTempPerm127;
	float 	fVec20[16384];
	float 	fRec63[2];
	float 	fTempPerm128;
	int 	iTempPerm129;
	float 	fTempPerm130;
	float 	fTempPerm131;
	float 	fTempPerm132;
	float 	fRec20[2];
	float 	fRec18[2];
	float 	fTempPerm133;
	float 	fVec21[16384];
	float 	fRec65[2];
	float 	fTempPerm134;
	int 	iTempPerm135;
	float 	fTempPerm136;
	float 	fTempPerm137;
	float 	fTempPerm138;
	float 	fRec64[2];
	float 	fRec19[2];
	float 	fTempPerm139;
	float 	fVec22[16384];
	float 	fRec66[2];
	float 	fTempPerm140;
	int 	iTempPerm141;
	float 	fTempPerm142;
	float 	fTempPerm143;
	float 	fTempPerm144;
	float 	fRec17[2];
	float 	fRec15[2];
	float 	fTempPerm145;
	float 	fVec23[16384];
	float 	fRec68[2];
	float 	fTempPerm146;
	int 	iTempPerm147;
	float 	fTempPerm148;
	float 	fTempPerm149;
	float 	fTempPerm150;
	float 	fRec67[2];
	float 	fRec16[2];
	float 	fTempPerm151;
	float 	fTempPerm152;
	float 	fTempPerm153;
	float 	fTempPerm154;
	float 	fTempPerm155;
	float 	fVec24[16384];
	float 	fRec69[2];
	float 	fTempPerm156;
	int 	iTempPerm157;
	float 	fTempPerm158;
	float 	fTempPerm159;
	float 	fTempPerm160;
	float 	fRec14[2];
	float 	fRec12[2];
	float 	fTempPerm161;
	float 	fTempPerm162;
	float 	fTempPerm163;
	float 	fTempPerm164;
	float 	fTempPerm165;
	float 	fVec25[16384];
	float 	fRec71[2];
	float 	fTempPerm166;
	int 	iTempPerm167;
	float 	fTempPerm168;
	float 	fTempPerm169;
	float 	fTempPerm170;
	float 	fRec70[2];
	float 	fRec13[2];
	float 	fTempPerm171;
	float 	fVec26[16384];
	float 	fRec72[2];
	float 	fTempPerm172;
	int 	iTempPerm173;
	float 	fTempPerm174;
	float 	fTempPerm175;
	float 	fTempPerm176;
	float 	fRec11[2];
	float 	fRec9[2];
	float 	fTempPerm177;
	float 	fVec27[16384];
	float 	fRec74[2];
	float 	fTempPerm178;
	int 	iTempPerm179;
	float 	fTempPerm180;
	float 	fTempPerm181;
	float 	fTempPerm182;
	float 	fRec73[2];
	float 	fRec10[2];
	float 	fTempPerm183;
	float 	fVec28[16384];
	float 	fRec75[2];
	float 	fTempPerm184;
	int 	iTempPerm185;
	float 	fTempPerm186;
	float 	fTempPerm187;
	float 	fTempPerm188;
	float 	fRec8[2];
	float 	fRec6[2];
	float 	fTempPerm189;
	float 	fVec29[16384];
	float 	fRec77[2];
	float 	fTempPerm190;
	int 	iTempPerm191;
	float 	fTempPerm192;
	float 	fTempPerm193;
	float 	fTempPerm194;
	float 	fRec76[2];
	float 	fRec7[2];
	float 	fTempPerm195;
	float 	fVec30[16384];
	float 	fRec78[2];
	float 	fTempPerm196;
	int 	iTempPerm197;
	float 	fTempPerm198;
	float 	fTempPerm199;
	float 	fTempPerm200;
	float 	fRec5[2];
	float 	fRec3[2];
	float 	fTempPerm201;
	float 	fVec31[16384];
	float 	fRec80[2];
	float 	fTempPerm202;
	int 	iTempPerm203;
	float 	fTempPerm204;
	float 	fTempPerm205;
	float 	fTempPerm206;
	float 	fRec79[2];
	float 	fRec4[2];
	float 	fTempPerm207;
	float 	fRec2[2];
	float 	fRec0[1024];
	float 	fRec81[2];
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
		m->declare("maths.lib/version", "2.1");
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
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fTempPerm0 = 0;
		fTempPerm1 = 0;
		fTempPerm2 = 0;
		fTempPerm3 = 0;
		fTempPerm4 = 0;
		fTempPerm5 = 0;
		fTempPerm6 = 0;
		fTempPerm7 = 0;
		fTempPerm8 = 0;
		fTempPerm9 = 0;
		fConst0 = min(1.92e+05f, max(1.0f, (float)fSamplingFreq));
		fConst1 = (0.00056689343f * fConst0);
		fTempPerm10 = 0;
		fConst2 = (3.1415927f / fConst0);
		fTempPerm11 = 0;
		fTempPerm12 = 0;
		fTempPerm13 = 0;
		iTempPerm14 = 0;
		fTempPerm15 = 0;
		fTempPerm16 = 0;
		iTempPerm17 = 0;
		fTempPerm18 = 0;
		fTempPerm19 = 0;
		fTempPerm20 = 0;
		fTempPerm21 = 0;
		fTempPerm22 = 0;
		fTempPerm23 = 0;
		fTempPerm24 = 0;
		fTempPerm25 = 0;
		iTempPerm26 = 0;
		iTempPerm27 = 0;
		fTempPerm28 = 0;
		fTempPerm29 = 0;
		fTempPerm30 = 0;
		fTempPerm31 = 0;
		fTempPerm32 = 0;
		fTempPerm33 = 0;
		fTempPerm34 = 0;
		iTempPerm35 = 0;
		fTempPerm36 = 0;
		fTempPerm37 = 0;
		fTempPerm38 = 0;
		fTempPerm39 = 0;
		fTempPerm40 = 0;
		fTempPerm41 = 0;
		fTempPerm42 = 0;
		fTempPerm43 = 0;
		fTempPerm44 = 0;
		fTempPerm45 = 0;
		iTempPerm46 = 0;
		fTempPerm47 = 0;
		fTempPerm48 = 0;
		fTempPerm49 = 0;
		fTempPerm50 = 0;
		fTempPerm51 = 0;
		fTempPerm52 = 0;
		fTempPerm53 = 0;
		fTempPerm54 = 0;
		iTempPerm55 = 0;
		fTempPerm56 = 0;
		fTempPerm57 = 0;
		fTempPerm58 = 0;
		fTempPerm59 = 0;
		fTempPerm60 = 0;
		iTempPerm61 = 0;
		fTempPerm62 = 0;
		fTempPerm63 = 0;
		fTempPerm64 = 0;
		fTempPerm65 = 0;
		fTempPerm66 = 0;
		iTempPerm67 = 0;
		fTempPerm68 = 0;
		fTempPerm69 = 0;
		fTempPerm70 = 0;
		fTempPerm71 = 0;
		fTempPerm72 = 0;
		iTempPerm73 = 0;
		fTempPerm74 = 0;
		fTempPerm75 = 0;
		fTempPerm76 = 0;
		fTempPerm77 = 0;
		fTempPerm78 = 0;
		iTempPerm79 = 0;
		fTempPerm80 = 0;
		fTempPerm81 = 0;
		fTempPerm82 = 0;
		fTempPerm83 = 0;
		fTempPerm84 = 0;
		iTempPerm85 = 0;
		fTempPerm86 = 0;
		fTempPerm87 = 0;
		fTempPerm88 = 0;
		fTempPerm89 = 0;
		fTempPerm90 = 0;
		iTempPerm91 = 0;
		fTempPerm92 = 0;
		fTempPerm93 = 0;
		fTempPerm94 = 0;
		fTempPerm95 = 0;
		fTempPerm96 = 0;
		fTempPerm97 = 0;
		fTempPerm98 = 0;
		fTempPerm99 = 0;
		fTempPerm100 = 0;
		iTempPerm101 = 0;
		fTempPerm102 = 0;
		fTempPerm103 = 0;
		fTempPerm104 = 0;
		fTempPerm105 = 0;
		fTempPerm106 = 0;
		fTempPerm107 = 0;
		fTempPerm108 = 0;
		fTempPerm109 = 0;
		fTempPerm110 = 0;
		iTempPerm111 = 0;
		fTempPerm112 = 0;
		fTempPerm113 = 0;
		fTempPerm114 = 0;
		fTempPerm115 = 0;
		fTempPerm116 = 0;
		iTempPerm117 = 0;
		fTempPerm118 = 0;
		fTempPerm119 = 0;
		fTempPerm120 = 0;
		fTempPerm121 = 0;
		fTempPerm122 = 0;
		iTempPerm123 = 0;
		fTempPerm124 = 0;
		fTempPerm125 = 0;
		fTempPerm126 = 0;
		fTempPerm127 = 0;
		fTempPerm128 = 0;
		iTempPerm129 = 0;
		fTempPerm130 = 0;
		fTempPerm131 = 0;
		fTempPerm132 = 0;
		fTempPerm133 = 0;
		fTempPerm134 = 0;
		iTempPerm135 = 0;
		fTempPerm136 = 0;
		fTempPerm137 = 0;
		fTempPerm138 = 0;
		fTempPerm139 = 0;
		fTempPerm140 = 0;
		iTempPerm141 = 0;
		fTempPerm142 = 0;
		fTempPerm143 = 0;
		fTempPerm144 = 0;
		fTempPerm145 = 0;
		fTempPerm146 = 0;
		iTempPerm147 = 0;
		fTempPerm148 = 0;
		fTempPerm149 = 0;
		fTempPerm150 = 0;
		fTempPerm151 = 0;
		fTempPerm152 = 0;
		fTempPerm153 = 0;
		fTempPerm154 = 0;
		fTempPerm155 = 0;
		fTempPerm156 = 0;
		iTempPerm157 = 0;
		fTempPerm158 = 0;
		fTempPerm159 = 0;
		fTempPerm160 = 0;
		fTempPerm161 = 0;
		fTempPerm162 = 0;
		fTempPerm163 = 0;
		fTempPerm164 = 0;
		fTempPerm165 = 0;
		fTempPerm166 = 0;
		iTempPerm167 = 0;
		fTempPerm168 = 0;
		fTempPerm169 = 0;
		fTempPerm170 = 0;
		fTempPerm171 = 0;
		fTempPerm172 = 0;
		iTempPerm173 = 0;
		fTempPerm174 = 0;
		fTempPerm175 = 0;
		fTempPerm176 = 0;
		fTempPerm177 = 0;
		fTempPerm178 = 0;
		iTempPerm179 = 0;
		fTempPerm180 = 0;
		fTempPerm181 = 0;
		fTempPerm182 = 0;
		fTempPerm183 = 0;
		fTempPerm184 = 0;
		iTempPerm185 = 0;
		fTempPerm186 = 0;
		fTempPerm187 = 0;
		fTempPerm188 = 0;
		fTempPerm189 = 0;
		fTempPerm190 = 0;
		iTempPerm191 = 0;
		fTempPerm192 = 0;
		fTempPerm193 = 0;
		fTempPerm194 = 0;
		fTempPerm195 = 0;
		fTempPerm196 = 0;
		iTempPerm197 = 0;
		fTempPerm198 = 0;
		fTempPerm199 = 0;
		fTempPerm200 = 0;
		fTempPerm201 = 0;
		fTempPerm202 = 0;
		iTempPerm203 = 0;
		fTempPerm204 = 0;
		fTempPerm205 = 0;
		fTempPerm206 = 0;
		fTempPerm207 = 0;
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.0f;
		fslider1 = 0.5f;
		fslider2 = 0.9f;
		fslider3 = 0.1f;
		fslider4 = 2.0f;
		fslider5 = 0.2f;
		fslider6 = 1.0f;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fVec0[i] = 0;
		for (int i=0; i<2; i++) iVec1[i] = 0;
		for (int i=0; i<2; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		IOTA = 0;
		for (int i=0; i<131072; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<16384; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<131072; i++) fVec8[i] = 0;
		for (int i=0; i<16384; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<16384; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<16384; i++) fVec11[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<16384; i++) fVec12[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<16384; i++) fVec13[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<16384; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<16384; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<16384; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<16384; i++) fVec17[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<16384; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<16384; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<16384; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<16384; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<16384; i++) fVec22[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<16384; i++) fVec23[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<16384; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<16384; i++) fVec25[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<16384; i++) fVec26[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<16384; i++) fVec27[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<16384; i++) fVec28[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<16384; i++) fVec29[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<16384; i++) fVec30[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<16384; i++) fVec31[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
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
		ui_interface->openVerticalBox("GreyholeRaw");
		ui_interface->addHorizontalSlider("damping", &fslider0, 0.0f, 0.0f, 0.99f, 0.001f);
		ui_interface->addHorizontalSlider("delayTime", &fslider5, 0.2f, 0.001f, 1.45f, 0.0001f);
		ui_interface->addHorizontalSlider("diffusion", &fslider1, 0.5f, 0.0f, 0.99f, 0.0001f);
		ui_interface->addHorizontalSlider("feedback", &fslider2, 0.9f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("modDepth", &fslider3, 0.1f, 0.0f, 1.0f, 0.001f);
		ui_interface->addHorizontalSlider("modFreq", &fslider4, 2.0f, 0.0f, 1e+01f, 0.01f);
		ui_interface->addHorizontalSlider("size", &fslider6, 1.0f, 0.5f, 3.0f, 0.0001f);
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		//zone1
		//zone2
		float 	fSlow0 = float(fslider0);
		float 	fSlow1 = float(fslider1);
		float 	fSlow2 = float(fslider2);
		float 	fSlow3 = float(fslider3);
		float 	fSlow4 = float(fslider4);
		float 	fSlow5 = floorf(min((float)65533, (fConst0 * float(fslider5))));
		float 	fSlow6 = float(fslider6);
		float 	fSlow7 = (49 * fSlow6);
		int 	iSlow8 = primes(fSlow7);
		float 	fSlow9 = (0.0001f * iSlow8);
		int 	iSlow10 = primes((fSlow7 + 10));
		float 	fSlow11 = (0.0001f * iSlow10);
		float 	fSlow12 = (36 * fSlow6);
		int 	iSlow13 = primes(fSlow12);
		float 	fSlow14 = (0.001f * iSlow13);
		int 	iSlow15 = primes((fSlow12 + 10));
		float 	fSlow16 = (0.001f * iSlow15);
		float 	fSlow17 = (23 * fSlow6);
		int 	iSlow18 = primes(fSlow17);
		float 	fSlow19 = (0.001f * iSlow18);
		int 	iSlow20 = primes((fSlow17 + 10));
		float 	fSlow21 = (0.001f * iSlow20);
		int 	iSlow22 = primes((10 * fSlow6));
		float 	fSlow23 = (0.001f * iSlow22);
		int 	iSlow24 = primes((10 * (fSlow6 + 1)));
		float 	fSlow25 = (0.001f * iSlow24);
		float 	fSlow26 = (68 * fSlow6);
		int 	iSlow27 = primes(fSlow26);
		float 	fSlow28 = (0.0001f * iSlow27);
		int 	iSlow29 = primes((fSlow26 + 10));
		float 	fSlow30 = (0.0001f * iSlow29);
		float 	fSlow31 = (55 * fSlow6);
		int 	iSlow32 = primes(fSlow31);
		float 	fSlow33 = (0.001f * iSlow32);
		int 	iSlow34 = primes((fSlow31 + 10));
		float 	fSlow35 = (0.001f * iSlow34);
		float 	fSlow36 = (42 * fSlow6);
		int 	iSlow37 = primes(fSlow36);
		float 	fSlow38 = (0.001f * iSlow37);
		int 	iSlow39 = primes((fSlow36 + 10));
		float 	fSlow40 = (0.001f * iSlow39);
		float 	fSlow41 = (29 * fSlow6);
		int 	iSlow42 = primes(fSlow41);
		float 	fSlow43 = (0.001f * iSlow42);
		int 	iSlow44 = primes((fSlow41 + 10));
		float 	fSlow45 = (0.001f * iSlow44);
		float 	fSlow46 = (87 * fSlow6);
		int 	iSlow47 = primes(fSlow46);
		float 	fSlow48 = (0.0001f * iSlow47);
		int 	iSlow49 = primes((fSlow46 + 10));
		float 	fSlow50 = (0.0001f * iSlow49);
		float 	fSlow51 = (74 * fSlow6);
		int 	iSlow52 = primes(fSlow51);
		float 	fSlow53 = (0.001f * iSlow52);
		int 	iSlow54 = primes((fSlow51 + 10));
		float 	fSlow55 = (0.001f * iSlow54);
		float 	fSlow56 = (61 * fSlow6);
		int 	iSlow57 = primes(fSlow56);
		float 	fSlow58 = (0.001f * iSlow57);
		int 	iSlow59 = primes((fSlow56 + 10));
		float 	fSlow60 = (0.001f * iSlow59);
		float 	fSlow61 = (48 * fSlow6);
		int 	iSlow62 = primes(fSlow61);
		float 	fSlow63 = (0.001f * iSlow62);
		int 	iSlow64 = primes((fSlow61 + 10));
		float 	fSlow65 = (0.001f * iSlow64);
		//zone2b
		//zone3
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		//LoopGraphScalar
		for (int i=0; i<count; i++) {
			fVec0[0] = fSlow0;
			iVec1[0] = 1;
			fTempPerm0 = (fSlow0 + fVec0[1]);
			fVec2[0] = fSlow1;
			fTempPerm1 = (0.5f * (fSlow1 + fVec2[1]));
			fTempPerm2 = cosf(fTempPerm1);
			fTempPerm3 = sinf(fTempPerm1);
			fTempPerm4 = (0 - fTempPerm3);
			fTempPerm5 = (0 - fTempPerm1);
			fTempPerm6 = cosf(fTempPerm5);
			fTempPerm7 = sinf(fTempPerm5);
			fTempPerm8 = (0 - fTempPerm7);
			fVec3[0] = fSlow2;
			fTempPerm9 = (fSlow2 + fVec3[1]);
			fVec4[0] = fSlow3;
			fTempPerm10 = (fSlow3 + fVec4[1]);
			fVec5[0] = fSlow4;
			fTempPerm11 = (fConst2 * (fSlow4 + fVec5[1]));
			fTempPerm12 = sinf(fTempPerm11);
			fTempPerm13 = cosf(fTempPerm11);
			fRec27[0] = ((fTempPerm12 * fRec28[1]) + (fTempPerm13 * fRec27[1]));
			iTempPerm14 = (1 - iVec1[1]);
			fRec28[0] = (((fTempPerm13 * fRec28[1]) + ((0 - fTempPerm12) * fRec27[1])) + iTempPerm14);
			fTempPerm15 = (fConst1 * (fTempPerm10 * (fRec27[0] + 1.0f)));
			fTempPerm16 = (fTempPerm15 + 8.500005f);
			iTempPerm17 = int(fTempPerm16);
			fTempPerm18 = floorf(fTempPerm16);
			fTempPerm19 = (fTempPerm15 + (9.0f - fTempPerm18));
			fTempPerm20 = (fTempPerm15 + (8.0f - fTempPerm18));
			fTempPerm21 = (fTempPerm15 + (7.0f - fTempPerm18));
			fTempPerm22 = (fTempPerm15 + (6.0f - fTempPerm18));
			fTempPerm23 = (fTempPerm19 * fTempPerm20);
			fTempPerm24 = (((((fRec1[(IOTA-int((int(min(512, max(0, iTempPerm17))) + 1)))&1023] * (0 - fTempPerm19)) * (0 - (0.5f * fTempPerm20))) * (0 - (0.33333334f * fTempPerm21))) * (0 - (0.25f * fTempPerm22))) + ((fTempPerm15 + (1e+01f - fTempPerm18)) * ((((((fRec1[(IOTA-int((int(min(512, max(0, (iTempPerm17 + 1)))) + 1)))&1023] * (0 - fTempPerm20)) * (0 - (0.5f * fTempPerm21))) * (0 - (0.33333334f * fTempPerm22))) + (0.5f * (((fTempPerm19 * fRec1[(IOTA-int((int(min(512, max(0, (iTempPerm17 + 2)))) + 1)))&1023]) * (0 - fTempPerm21)) * (0 - (0.5f * fTempPerm22))))) + (0.16666667f * ((fTempPerm23 * fRec1[(IOTA-int((int(min(512, max(0, (iTempPerm17 + 3)))) + 1)))&1023]) * (0 - fTempPerm22)))) + (0.041666668f * ((fTempPerm23 * fTempPerm21) * fRec1[(IOTA-int((int(min(512, max(0, (iTempPerm17 + 4)))) + 1)))&1023])))));
			fVec6[IOTA&131071] = fTempPerm24;
			fTempPerm25 = ((int((fRec29[1] != 0.0f)))?((int(((fRec30[1] > 0.0f) & (fRec30[1] < 1.0f))))?fRec29[1]:0):((int(((fRec30[1] == 0.0f) & (fSlow5 != fRec31[1]))))?4.5351473e-05f:((int(((fRec30[1] == 1.0f) & (fSlow5 != fRec32[1]))))?-4.5351473e-05f:0)));
			fRec29[0] = fTempPerm25;
			fRec30[0] = max(0.0f, min(1.0f, (fRec30[1] + fTempPerm25)));
			fRec31[0] = ((int(((fRec30[1] >= 1.0f) & (fRec32[1] != fSlow5))))?fSlow5:fRec31[1]);
			fRec32[0] = ((int(((fRec30[1] <= 0.0f) & (fRec31[1] != fSlow5))))?fSlow5:fRec32[1]);
			iTempPerm26 = int(min((float)65536, max((float)0, fRec32[0])));
			iTempPerm27 = int(min((float)65536, max((float)0, fRec31[0])));
			fTempPerm28 = (1.0f - fRec30[0]);
			fTempPerm29 = ((float)input1[i] + (0.5f * (fTempPerm9 * ((fVec6[(IOTA-iTempPerm26)&131071] * fRec30[0]) + (fVec6[(IOTA-iTempPerm27)&131071] * fTempPerm28)))));
			fTempPerm30 = ((fRec34[1] * fTempPerm4) + (fTempPerm2 * fTempPerm29));
			fTempPerm31 = ((fRec37[1] * fTempPerm4) + (fTempPerm2 * fTempPerm30));
			fTempPerm32 = ((fRec40[1] * fTempPerm4) + (fTempPerm2 * fTempPerm31));
			fTempPerm33 = (0 - ((fRec43[1] * fTempPerm4) + (fTempPerm2 * fTempPerm32)));
			fVec7[IOTA&16383] = fTempPerm33;
			fRec45[0] = (fSlow9 + (0.9999f * ((iSlow8 * iTempPerm14) + fRec45[1])));
			fTempPerm34 = (fRec45[0] + -1.49999f);
			iTempPerm35 = int(min(8192, max(0, int(fTempPerm34))));
			fTempPerm36 = floorf(fTempPerm34);
			fTempPerm37 = (fTempPerm36 + (2.0f - fRec45[0]));
			fTempPerm38 = (fRec45[0] - fTempPerm36);
			fRec44[0] = ((((fVec7[(IOTA-iTempPerm35)&16383] * fTempPerm37) / fTempPerm38) + fVec7[(IOTA-int((iTempPerm35 + 1)))&16383]) + (fRec44[1] * (0 - (fTempPerm37 / fTempPerm38))));
			fRec42[0] = fRec44[0];
			fTempPerm39 = (fConst1 * (fTempPerm10 * (fRec28[0] + 1.0f)));
			fTempPerm40 = (fTempPerm39 + 8.500005f);
			fTempPerm41 = floorf(fTempPerm40);
			fTempPerm42 = (fTempPerm39 + (9.0f - fTempPerm41));
			fTempPerm43 = (fTempPerm39 + (8.0f - fTempPerm41));
			fTempPerm44 = (fTempPerm39 + (7.0f - fTempPerm41));
			fTempPerm45 = (fTempPerm39 + (6.0f - fTempPerm41));
			iTempPerm46 = int(fTempPerm40);
			fTempPerm47 = (fTempPerm42 * fTempPerm43);
			fTempPerm48 = ((((((0 - fTempPerm42) * (0 - (0.5f * fTempPerm43))) * (0 - (0.33333334f * fTempPerm44))) * (0 - (0.25f * fTempPerm45))) * fRec0[(IOTA-int((int(min(512, max(0, iTempPerm46))) + 1)))&1023]) + ((fTempPerm39 + (1e+01f - fTempPerm41)) * ((((((fRec0[(IOTA-int((int(min(512, max(0, (iTempPerm46 + 1)))) + 1)))&1023] * (0 - fTempPerm43)) * (0 - (0.5f * fTempPerm44))) * (0 - (0.33333334f * fTempPerm45))) + (0.5f * (((fTempPerm42 * fRec0[(IOTA-int((int(min(512, max(0, (iTempPerm46 + 2)))) + 1)))&1023]) * (0 - fTempPerm44)) * (0 - (0.5f * fTempPerm45))))) + (0.16666667f * ((fTempPerm47 * fRec0[(IOTA-int((int(min(512, max(0, (iTempPerm46 + 3)))) + 1)))&1023]) * (0 - fTempPerm45)))) + (0.041666668f * ((fTempPerm47 * fTempPerm44) * fRec0[(IOTA-int((int(min(512, max(0, (iTempPerm46 + 4)))) + 1)))&1023])))));
			fVec8[IOTA&131071] = fTempPerm48;
			fTempPerm49 = ((float)input0[i] + (0.5f * (((fVec8[(IOTA-iTempPerm27)&131071] * fTempPerm28) + (fVec8[(IOTA-iTempPerm26)&131071] * fRec30[0])) * fTempPerm9)));
			fTempPerm50 = ((fTempPerm2 * fTempPerm49) + (fTempPerm4 * fRec33[1]));
			fTempPerm51 = ((fTempPerm2 * fTempPerm50) + (fTempPerm4 * fRec36[1]));
			fTempPerm52 = ((fTempPerm2 * fTempPerm51) + (fTempPerm4 * fRec39[1]));
			fTempPerm53 = ((fTempPerm2 * fTempPerm52) + (fTempPerm4 * fRec42[1]));
			fVec9[IOTA&16383] = fTempPerm53;
			fRec47[0] = (fSlow11 + (0.9999f * ((iSlow10 * iTempPerm14) + fRec47[1])));
			fTempPerm54 = (fRec47[0] + -1.49999f);
			iTempPerm55 = int(min(8192, max(0, int(fTempPerm54))));
			fTempPerm56 = floorf(fTempPerm54);
			fTempPerm57 = (fTempPerm56 + (2.0f - fRec47[0]));
			fTempPerm58 = (fRec47[0] - fTempPerm56);
			fRec46[0] = (fVec9[(IOTA-int((iTempPerm55 + 1)))&16383] + ((fRec46[1] * (0 - (fTempPerm57 / fTempPerm58))) + ((fTempPerm57 * fVec9[(IOTA-iTempPerm55)&16383]) / fTempPerm58)));
			fRec43[0] = fRec46[0];
			fTempPerm59 = (0 - ((fTempPerm2 * fRec43[1]) + (fTempPerm32 * fTempPerm3)));
			fVec10[IOTA&16383] = fTempPerm59;
			fRec48[0] = (fSlow14 + (0.999f * ((iSlow13 * iTempPerm14) + fRec48[1])));
			fTempPerm60 = (fRec48[0] + -1.49999f);
			iTempPerm61 = int(min(8192, max(0, int(fTempPerm60))));
			fTempPerm62 = floorf(fTempPerm60);
			fTempPerm63 = (fTempPerm62 + (2.0f - fRec48[0]));
			fTempPerm64 = (fRec48[0] - fTempPerm62);
			fRec41[0] = ((((fVec10[(IOTA-iTempPerm61)&16383] * fTempPerm63) / fTempPerm64) + fVec10[(IOTA-int((iTempPerm61 + 1)))&16383]) + (fRec41[1] * (0 - (fTempPerm63 / fTempPerm64))));
			fRec39[0] = fRec41[0];
			fTempPerm65 = ((fTempPerm2 * fRec42[1]) + (fTempPerm52 * fTempPerm3));
			fVec11[IOTA&16383] = fTempPerm65;
			fRec50[0] = (fSlow16 + (0.999f * ((iSlow15 * iTempPerm14) + fRec50[1])));
			fTempPerm66 = (fRec50[0] + -1.49999f);
			iTempPerm67 = int(min(8192, max(0, int(fTempPerm66))));
			fTempPerm68 = floorf(fTempPerm66);
			fTempPerm69 = (fTempPerm68 + (2.0f - fRec50[0]));
			fTempPerm70 = (fRec50[0] - fTempPerm68);
			fRec49[0] = (fVec11[(IOTA-int((iTempPerm67 + 1)))&16383] + ((fRec49[1] * (0 - (fTempPerm69 / fTempPerm70))) + ((fTempPerm69 * fVec11[(IOTA-iTempPerm67)&16383]) / fTempPerm70)));
			fRec40[0] = fRec49[0];
			fTempPerm71 = (0 - ((fTempPerm2 * fRec40[1]) + (fTempPerm31 * fTempPerm3)));
			fVec12[IOTA&16383] = fTempPerm71;
			fRec51[0] = (fSlow19 + (0.999f * ((iSlow18 * iTempPerm14) + fRec51[1])));
			fTempPerm72 = (fRec51[0] + -1.49999f);
			iTempPerm73 = int(min(8192, max(0, int(fTempPerm72))));
			fTempPerm74 = floorf(fTempPerm72);
			fTempPerm75 = (fTempPerm74 + (2.0f - fRec51[0]));
			fTempPerm76 = (fRec51[0] - fTempPerm74);
			fRec38[0] = ((((fVec12[(IOTA-iTempPerm73)&16383] * fTempPerm75) / fTempPerm76) + fVec12[(IOTA-int((iTempPerm73 + 1)))&16383]) + (fRec38[1] * (0 - (fTempPerm75 / fTempPerm76))));
			fRec36[0] = fRec38[0];
			fTempPerm77 = ((fTempPerm2 * fRec39[1]) + (fTempPerm51 * fTempPerm3));
			fVec13[IOTA&16383] = fTempPerm77;
			fRec53[0] = (fSlow21 + (0.999f * ((iSlow20 * iTempPerm14) + fRec53[1])));
			fTempPerm78 = (fRec53[0] + -1.49999f);
			iTempPerm79 = int(min(8192, max(0, int(fTempPerm78))));
			fTempPerm80 = floorf(fTempPerm78);
			fTempPerm81 = (fTempPerm80 + (2.0f - fRec53[0]));
			fTempPerm82 = (fRec53[0] - fTempPerm80);
			fRec52[0] = (fVec13[(IOTA-int((iTempPerm79 + 1)))&16383] + ((fRec52[1] * (0 - (fTempPerm81 / fTempPerm82))) + ((fTempPerm81 * fVec13[(IOTA-iTempPerm79)&16383]) / fTempPerm82)));
			fRec37[0] = fRec52[0];
			fTempPerm83 = (0 - ((fTempPerm2 * fRec37[1]) + (fTempPerm30 * fTempPerm3)));
			fVec14[IOTA&16383] = fTempPerm83;
			fRec54[0] = (fSlow23 + (0.999f * ((iSlow22 * iTempPerm14) + fRec54[1])));
			fTempPerm84 = (fRec54[0] + -1.49999f);
			iTempPerm85 = int(min(8192, max(0, int(fTempPerm84))));
			fTempPerm86 = floorf(fTempPerm84);
			fTempPerm87 = (fTempPerm86 + (2.0f - fRec54[0]));
			fTempPerm88 = (fRec54[0] - fTempPerm86);
			fRec35[0] = ((((fVec14[(IOTA-iTempPerm85)&16383] * fTempPerm87) / fTempPerm88) + fVec14[(IOTA-int((iTempPerm85 + 1)))&16383]) + ((0 - (fTempPerm87 / fTempPerm88)) * fRec35[1]));
			fRec33[0] = fRec35[0];
			fTempPerm89 = ((fTempPerm2 * fRec36[1]) + (fTempPerm50 * fTempPerm3));
			fVec15[IOTA&16383] = fTempPerm89;
			fRec56[0] = (fSlow25 + (0.999f * ((iSlow24 * iTempPerm14) + fRec56[1])));
			fTempPerm90 = (fRec56[0] + -1.49999f);
			iTempPerm91 = int(min(8192, max(0, int(fTempPerm90))));
			fTempPerm92 = floorf(fTempPerm90);
			fTempPerm93 = (fTempPerm92 + (2.0f - fRec56[0]));
			fTempPerm94 = (fRec56[0] - fTempPerm92);
			fRec55[0] = (fVec15[(IOTA-int((iTempPerm91 + 1)))&16383] + ((fRec55[1] * (0 - (fTempPerm93 / fTempPerm94))) + ((fTempPerm93 * fVec15[(IOTA-iTempPerm91)&16383]) / fTempPerm94)));
			fRec34[0] = fRec55[0];
			fTempPerm95 = ((fTempPerm29 * fTempPerm3) + (fTempPerm2 * fRec34[1]));
			fTempPerm96 = ((fRec16[1] * fTempPerm8) + (fTempPerm6 * fTempPerm95));
			fTempPerm97 = ((fRec19[1] * fTempPerm8) + (fTempPerm6 * fTempPerm96));
			fTempPerm98 = ((fRec22[1] * fTempPerm8) + (fTempPerm6 * fTempPerm97));
			fTempPerm99 = (0 - ((fRec25[1] * fTempPerm8) + (fTempPerm6 * fTempPerm98)));
			fVec16[IOTA&16383] = fTempPerm99;
			fRec57[0] = (fSlow28 + (0.9999f * ((iSlow27 * iTempPerm14) + fRec57[1])));
			fTempPerm100 = (fRec57[0] + -1.49999f);
			iTempPerm101 = int(min(8192, max(0, int(fTempPerm100))));
			fTempPerm102 = floorf(fTempPerm100);
			fTempPerm103 = (fTempPerm102 + (2.0f - fRec57[0]));
			fTempPerm104 = (fRec57[0] - fTempPerm102);
			fRec26[0] = ((((fVec16[(IOTA-iTempPerm101)&16383] * fTempPerm103) / fTempPerm104) + fVec16[(IOTA-int((iTempPerm101 + 1)))&16383]) + (fRec26[1] * (0 - (fTempPerm103 / fTempPerm104))));
			fRec24[0] = fRec26[0];
			fTempPerm105 = ((fTempPerm2 * fRec33[1]) + (fTempPerm49 * fTempPerm3));
			fTempPerm106 = ((fTempPerm105 * fTempPerm6) + (fTempPerm8 * fRec15[1]));
			fTempPerm107 = ((fTempPerm6 * fTempPerm106) + (fTempPerm8 * fRec18[1]));
			fTempPerm108 = ((fTempPerm6 * fTempPerm107) + (fTempPerm8 * fRec21[1]));
			fTempPerm109 = ((fTempPerm6 * fTempPerm108) + (fRec24[1] * fTempPerm8));
			fVec17[IOTA&16383] = fTempPerm109;
			fRec59[0] = (fSlow30 + (0.9999f * ((iSlow29 * iTempPerm14) + fRec59[1])));
			fTempPerm110 = (fRec59[0] + -1.49999f);
			iTempPerm111 = int(min(8192, max(0, int(fTempPerm110))));
			fTempPerm112 = floorf(fTempPerm110);
			fTempPerm113 = (fTempPerm112 + (2.0f - fRec59[0]));
			fTempPerm114 = (fRec59[0] - fTempPerm112);
			fRec58[0] = (fVec17[(IOTA-int((iTempPerm111 + 1)))&16383] + ((fRec58[1] * (0 - (fTempPerm113 / fTempPerm114))) + ((fTempPerm113 * fVec17[(IOTA-iTempPerm111)&16383]) / fTempPerm114)));
			fRec25[0] = fRec58[0];
			fTempPerm115 = (0 - ((fTempPerm6 * fRec25[1]) + (fTempPerm98 * fTempPerm7)));
			fVec18[IOTA&16383] = fTempPerm115;
			fRec60[0] = (fSlow33 + (0.999f * ((iSlow32 * iTempPerm14) + fRec60[1])));
			fTempPerm116 = (fRec60[0] + -1.49999f);
			iTempPerm117 = int(min(8192, max(0, int(fTempPerm116))));
			fTempPerm118 = floorf(fTempPerm116);
			fTempPerm119 = (fTempPerm118 + (2.0f - fRec60[0]));
			fTempPerm120 = (fRec60[0] - fTempPerm118);
			fRec23[0] = ((((fVec18[(IOTA-iTempPerm117)&16383] * fTempPerm119) / fTempPerm120) + fVec18[(IOTA-int((iTempPerm117 + 1)))&16383]) + (fRec23[1] * (0 - (fTempPerm119 / fTempPerm120))));
			fRec21[0] = fRec23[0];
			fTempPerm121 = ((fTempPerm6 * fRec24[1]) + (fTempPerm108 * fTempPerm7));
			fVec19[IOTA&16383] = fTempPerm121;
			fRec62[0] = (fSlow35 + (0.999f * ((iSlow34 * iTempPerm14) + fRec62[1])));
			fTempPerm122 = (fRec62[0] + -1.49999f);
			iTempPerm123 = int(min(8192, max(0, int(fTempPerm122))));
			fTempPerm124 = floorf(fTempPerm122);
			fTempPerm125 = (fTempPerm124 + (2.0f - fRec62[0]));
			fTempPerm126 = (fRec62[0] - fTempPerm124);
			fRec61[0] = (fVec19[(IOTA-int((iTempPerm123 + 1)))&16383] + ((fRec61[1] * (0 - (fTempPerm125 / fTempPerm126))) + ((fTempPerm125 * fVec19[(IOTA-iTempPerm123)&16383]) / fTempPerm126)));
			fRec22[0] = fRec61[0];
			fTempPerm127 = (0 - ((fTempPerm6 * fRec22[1]) + (fTempPerm97 * fTempPerm7)));
			fVec20[IOTA&16383] = fTempPerm127;
			fRec63[0] = (fSlow38 + (0.999f * ((iSlow37 * iTempPerm14) + fRec63[1])));
			fTempPerm128 = (fRec63[0] + -1.49999f);
			iTempPerm129 = int(min(8192, max(0, int(fTempPerm128))));
			fTempPerm130 = floorf(fTempPerm128);
			fTempPerm131 = (fTempPerm130 + (2.0f - fRec63[0]));
			fTempPerm132 = (fRec63[0] - fTempPerm130);
			fRec20[0] = ((((fVec20[(IOTA-iTempPerm129)&16383] * fTempPerm131) / fTempPerm132) + fVec20[(IOTA-int((iTempPerm129 + 1)))&16383]) + (fRec20[1] * (0 - (fTempPerm131 / fTempPerm132))));
			fRec18[0] = fRec20[0];
			fTempPerm133 = ((fTempPerm6 * fRec21[1]) + (fTempPerm107 * fTempPerm7));
			fVec21[IOTA&16383] = fTempPerm133;
			fRec65[0] = (fSlow40 + (0.999f * ((iSlow39 * iTempPerm14) + fRec65[1])));
			fTempPerm134 = (fRec65[0] + -1.49999f);
			iTempPerm135 = int(min(8192, max(0, int(fTempPerm134))));
			fTempPerm136 = floorf(fTempPerm134);
			fTempPerm137 = (fTempPerm136 + (2.0f - fRec65[0]));
			fTempPerm138 = (fRec65[0] - fTempPerm136);
			fRec64[0] = (fVec21[(IOTA-int((iTempPerm135 + 1)))&16383] + ((fRec64[1] * (0 - (fTempPerm137 / fTempPerm138))) + ((fTempPerm137 * fVec21[(IOTA-iTempPerm135)&16383]) / fTempPerm138)));
			fRec19[0] = fRec64[0];
			fTempPerm139 = (0 - ((fTempPerm6 * fRec19[1]) + (fTempPerm96 * fTempPerm7)));
			fVec22[IOTA&16383] = fTempPerm139;
			fRec66[0] = (fSlow43 + (0.999f * ((iSlow42 * iTempPerm14) + fRec66[1])));
			fTempPerm140 = (fRec66[0] + -1.49999f);
			iTempPerm141 = int(min(8192, max(0, int(fTempPerm140))));
			fTempPerm142 = floorf(fTempPerm140);
			fTempPerm143 = (fTempPerm142 + (2.0f - fRec66[0]));
			fTempPerm144 = (fRec66[0] - fTempPerm142);
			fRec17[0] = ((((fVec22[(IOTA-iTempPerm141)&16383] * fTempPerm143) / fTempPerm144) + fVec22[(IOTA-int((iTempPerm141 + 1)))&16383]) + (fRec17[1] * (0 - (fTempPerm143 / fTempPerm144))));
			fRec15[0] = fRec17[0];
			fTempPerm145 = ((fTempPerm6 * fRec18[1]) + (fTempPerm106 * fTempPerm7));
			fVec23[IOTA&16383] = fTempPerm145;
			fRec68[0] = (fSlow45 + (0.999f * ((iSlow44 * iTempPerm14) + fRec68[1])));
			fTempPerm146 = (fRec68[0] + -1.49999f);
			iTempPerm147 = int(min(8192, max(0, int(fTempPerm146))));
			fTempPerm148 = floorf(fTempPerm146);
			fTempPerm149 = (fTempPerm148 + (2.0f - fRec68[0]));
			fTempPerm150 = (fRec68[0] - fTempPerm148);
			fRec67[0] = (fVec23[(IOTA-int((iTempPerm147 + 1)))&16383] + ((fRec67[1] * (0 - (fTempPerm149 / fTempPerm150))) + ((fTempPerm149 * fVec23[(IOTA-iTempPerm147)&16383]) / fTempPerm150)));
			fRec16[0] = fRec67[0];
			fTempPerm151 = ((fTempPerm6 * fRec16[1]) + (fTempPerm95 * fTempPerm7));
			fTempPerm152 = ((fTempPerm4 * fRec4[1]) + (fTempPerm2 * fTempPerm151));
			fTempPerm153 = ((fTempPerm4 * fRec7[1]) + (fTempPerm2 * fTempPerm152));
			fTempPerm154 = ((fTempPerm4 * fRec10[1]) + (fTempPerm2 * fTempPerm153));
			fTempPerm155 = (0 - ((fTempPerm4 * fRec13[1]) + (fTempPerm2 * fTempPerm154)));
			fVec24[IOTA&16383] = fTempPerm155;
			fRec69[0] = (fSlow48 + (0.9999f * ((iSlow47 * iTempPerm14) + fRec69[1])));
			fTempPerm156 = (fRec69[0] + -1.49999f);
			iTempPerm157 = int(min(8192, max(0, int(fTempPerm156))));
			fTempPerm158 = floorf(fTempPerm156);
			fTempPerm159 = (fTempPerm158 + (2.0f - fRec69[0]));
			fTempPerm160 = (fRec69[0] - fTempPerm158);
			fRec14[0] = (fVec24[(IOTA-int((iTempPerm157 + 1)))&16383] + ((fRec14[1] * (0 - (fTempPerm159 / fTempPerm160))) + ((fTempPerm159 * fVec24[(IOTA-iTempPerm157)&16383]) / fTempPerm160)));
			fRec12[0] = fRec14[0];
			fTempPerm161 = ((fTempPerm6 * fRec15[1]) + (fTempPerm105 * fTempPerm7));
			fTempPerm162 = ((fTempPerm4 * fRec3[1]) + (fTempPerm2 * fTempPerm161));
			fTempPerm163 = ((fTempPerm4 * fRec6[1]) + (fTempPerm2 * fTempPerm162));
			fTempPerm164 = ((fTempPerm4 * fRec9[1]) + (fTempPerm2 * fTempPerm163));
			fTempPerm165 = ((fRec12[1] * fTempPerm4) + (fTempPerm2 * fTempPerm164));
			fVec25[IOTA&16383] = fTempPerm165;
			fRec71[0] = (fSlow50 + (0.9999f * ((iSlow49 * iTempPerm14) + fRec71[1])));
			fTempPerm166 = (fRec71[0] + -1.49999f);
			iTempPerm167 = int(min(8192, max(0, int(fTempPerm166))));
			fTempPerm168 = floorf(fTempPerm166);
			fTempPerm169 = (fTempPerm168 + (2.0f - fRec71[0]));
			fTempPerm170 = (fRec71[0] - fTempPerm168);
			fRec70[0] = (fVec25[(IOTA-int((iTempPerm167 + 1)))&16383] + ((fRec70[1] * (0 - (fTempPerm169 / fTempPerm170))) + ((fTempPerm169 * fVec25[(IOTA-iTempPerm167)&16383]) / fTempPerm170)));
			fRec13[0] = fRec70[0];
			fTempPerm171 = (0 - ((fTempPerm2 * fRec13[1]) + (fTempPerm3 * fTempPerm154)));
			fVec26[IOTA&16383] = fTempPerm171;
			fRec72[0] = (fSlow53 + (0.999f * ((iSlow52 * iTempPerm14) + fRec72[1])));
			fTempPerm172 = (fRec72[0] + -1.49999f);
			iTempPerm173 = int(min(8192, max(0, int(fTempPerm172))));
			fTempPerm174 = floorf(fTempPerm172);
			fTempPerm175 = (fTempPerm174 + (2.0f - fRec72[0]));
			fTempPerm176 = (fRec72[0] - fTempPerm174);
			fRec11[0] = (fVec26[(IOTA-int((iTempPerm173 + 1)))&16383] + ((fRec11[1] * (0 - (fTempPerm175 / fTempPerm176))) + ((fTempPerm175 * fVec26[(IOTA-iTempPerm173)&16383]) / fTempPerm176)));
			fRec9[0] = fRec11[0];
			fTempPerm177 = ((fTempPerm2 * fRec12[1]) + (fTempPerm3 * fTempPerm164));
			fVec27[IOTA&16383] = fTempPerm177;
			fRec74[0] = (fSlow55 + (0.999f * ((iSlow54 * iTempPerm14) + fRec74[1])));
			fTempPerm178 = (fRec74[0] + -1.49999f);
			iTempPerm179 = int(min(8192, max(0, int(fTempPerm178))));
			fTempPerm180 = floorf(fTempPerm178);
			fTempPerm181 = (fTempPerm180 + (2.0f - fRec74[0]));
			fTempPerm182 = (fRec74[0] - fTempPerm180);
			fRec73[0] = (fVec27[(IOTA-int((iTempPerm179 + 1)))&16383] + ((fRec73[1] * (0 - (fTempPerm181 / fTempPerm182))) + ((fTempPerm181 * fVec27[(IOTA-iTempPerm179)&16383]) / fTempPerm182)));
			fRec10[0] = fRec73[0];
			fTempPerm183 = (0 - ((fTempPerm2 * fRec10[1]) + (fTempPerm3 * fTempPerm153)));
			fVec28[IOTA&16383] = fTempPerm183;
			fRec75[0] = (fSlow58 + (0.999f * ((iSlow57 * iTempPerm14) + fRec75[1])));
			fTempPerm184 = (fRec75[0] + -1.49999f);
			iTempPerm185 = int(min(8192, max(0, int(fTempPerm184))));
			fTempPerm186 = floorf(fTempPerm184);
			fTempPerm187 = (fTempPerm186 + (2.0f - fRec75[0]));
			fTempPerm188 = (fRec75[0] - fTempPerm186);
			fRec8[0] = (fVec28[(IOTA-int((iTempPerm185 + 1)))&16383] + ((fRec8[1] * (0 - (fTempPerm187 / fTempPerm188))) + ((fTempPerm187 * fVec28[(IOTA-iTempPerm185)&16383]) / fTempPerm188)));
			fRec6[0] = fRec8[0];
			fTempPerm189 = ((fTempPerm2 * fRec9[1]) + (fTempPerm3 * fTempPerm163));
			fVec29[IOTA&16383] = fTempPerm189;
			fRec77[0] = (fSlow60 + (0.999f * ((iSlow59 * iTempPerm14) + fRec77[1])));
			fTempPerm190 = (fRec77[0] + -1.49999f);
			iTempPerm191 = int(min(8192, max(0, int(fTempPerm190))));
			fTempPerm192 = floorf(fTempPerm190);
			fTempPerm193 = (fTempPerm192 + (2.0f - fRec77[0]));
			fTempPerm194 = (fRec77[0] - fTempPerm192);
			fRec76[0] = (fVec29[(IOTA-int((iTempPerm191 + 1)))&16383] + ((fRec76[1] * (0 - (fTempPerm193 / fTempPerm194))) + ((fTempPerm193 * fVec29[(IOTA-iTempPerm191)&16383]) / fTempPerm194)));
			fRec7[0] = fRec76[0];
			fTempPerm195 = (0 - ((fTempPerm2 * fRec7[1]) + (fTempPerm3 * fTempPerm152)));
			fVec30[IOTA&16383] = fTempPerm195;
			fRec78[0] = (fSlow63 + (0.999f * ((iSlow62 * iTempPerm14) + fRec78[1])));
			fTempPerm196 = (fRec78[0] + -1.49999f);
			iTempPerm197 = int(min(8192, max(0, int(fTempPerm196))));
			fTempPerm198 = floorf(fTempPerm196);
			fTempPerm199 = (fTempPerm198 + (2.0f - fRec78[0]));
			fTempPerm200 = (fRec78[0] - fTempPerm198);
			fRec5[0] = (fVec30[(IOTA-int((iTempPerm197 + 1)))&16383] + ((fRec5[1] * (0 - (fTempPerm199 / fTempPerm200))) + ((fTempPerm199 * fVec30[(IOTA-iTempPerm197)&16383]) / fTempPerm200)));
			fRec3[0] = fRec5[0];
			fTempPerm201 = ((fTempPerm2 * fRec6[1]) + (fTempPerm3 * fTempPerm162));
			fVec31[IOTA&16383] = fTempPerm201;
			fRec80[0] = (fSlow65 + (0.999f * ((iSlow64 * iTempPerm14) + fRec80[1])));
			fTempPerm202 = (fRec80[0] + -1.49999f);
			iTempPerm203 = int(min(8192, max(0, int(fTempPerm202))));
			fTempPerm204 = floorf(fTempPerm202);
			fTempPerm205 = (fTempPerm204 + (2.0f - fRec80[0]));
			fTempPerm206 = (fRec80[0] - fTempPerm204);
			fRec79[0] = (fVec31[(IOTA-int((iTempPerm203 + 1)))&16383] + ((fRec79[1] * (0 - (fTempPerm205 / fTempPerm206))) + ((fTempPerm205 * fVec31[(IOTA-iTempPerm203)&16383]) / fTempPerm206)));
			fRec4[0] = fRec79[0];
			fTempPerm207 = (1.0f - (0.5f * fTempPerm0));
			fRec2[0] = ((0.5f * (fTempPerm0 * fRec2[1])) + (((fTempPerm2 * fRec3[1]) + (fTempPerm3 * fTempPerm161)) * fTempPerm207));
			fRec0[IOTA&1023] = fRec2[0];
			fRec81[0] = ((0.5f * (fTempPerm0 * fRec81[1])) + (fTempPerm207 * ((fTempPerm2 * fRec4[1]) + (fTempPerm3 * fTempPerm151))));
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
			fRec78[1] = fRec78[0];
			fRec7[1] = fRec7[0];
			fRec76[1] = fRec76[0];
			fRec77[1] = fRec77[0];
			fRec6[1] = fRec6[0];
			fRec8[1] = fRec8[0];
			fRec75[1] = fRec75[0];
			fRec10[1] = fRec10[0];
			fRec73[1] = fRec73[0];
			fRec74[1] = fRec74[0];
			fRec9[1] = fRec9[0];
			fRec11[1] = fRec11[0];
			fRec72[1] = fRec72[0];
			fRec13[1] = fRec13[0];
			fRec70[1] = fRec70[0];
			fRec71[1] = fRec71[0];
			fRec12[1] = fRec12[0];
			fRec14[1] = fRec14[0];
			fRec69[1] = fRec69[0];
			fRec16[1] = fRec16[0];
			fRec67[1] = fRec67[0];
			fRec68[1] = fRec68[0];
			fRec15[1] = fRec15[0];
			fRec17[1] = fRec17[0];
			fRec66[1] = fRec66[0];
			fRec19[1] = fRec19[0];
			fRec64[1] = fRec64[0];
			fRec65[1] = fRec65[0];
			fRec18[1] = fRec18[0];
			fRec20[1] = fRec20[0];
			fRec63[1] = fRec63[0];
			fRec22[1] = fRec22[0];
			fRec61[1] = fRec61[0];
			fRec62[1] = fRec62[0];
			fRec21[1] = fRec21[0];
			fRec23[1] = fRec23[0];
			fRec60[1] = fRec60[0];
			fRec25[1] = fRec25[0];
			fRec58[1] = fRec58[0];
			fRec59[1] = fRec59[0];
			fRec24[1] = fRec24[0];
			fRec26[1] = fRec26[0];
			fRec57[1] = fRec57[0];
			fRec34[1] = fRec34[0];
			fRec55[1] = fRec55[0];
			fRec56[1] = fRec56[0];
			fRec33[1] = fRec33[0];
			fRec35[1] = fRec35[0];
			fRec54[1] = fRec54[0];
			fRec37[1] = fRec37[0];
			fRec52[1] = fRec52[0];
			fRec53[1] = fRec53[0];
			fRec36[1] = fRec36[0];
			fRec38[1] = fRec38[0];
			fRec51[1] = fRec51[0];
			fRec40[1] = fRec40[0];
			fRec49[1] = fRec49[0];
			fRec50[1] = fRec50[0];
			fRec39[1] = fRec39[0];
			fRec41[1] = fRec41[0];
			fRec48[1] = fRec48[0];
			fRec43[1] = fRec43[0];
			fRec46[1] = fRec46[0];
			fRec47[1] = fRec47[0];
			fRec42[1] = fRec42[0];
			fRec44[1] = fRec44[0];
			fRec45[1] = fRec45[0];
			fRec32[1] = fRec32[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec29[1] = fRec29[0];
			IOTA = IOTA+1;
			fRec28[1] = fRec28[0];
			fRec27[1] = fRec27[0];
			fVec5[1] = fVec5[0];
			fVec4[1] = fVec4[0];
			fVec3[1] = fVec3[0];
			fVec2[1] = fVec2[0];
			iVec1[1] = iVec1[0];
			fVec0[1] = fVec0[0];
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
    g_unitName = STRDUP(name.c_str());

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
    size_t numControls = unit->mNumControls;
    int curControl = unit->mDSP->getNumInputs();
    for (int i = 0; i < numControls; ++i) {
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

#ifdef SUPERNOVA 
extern "C" FAUST_EXPORT int server_type(void) { return sc_server_supernova; }
#else
extern "C" FAUST_EXPORT int server_type(void) { return sc_server_scsynth; }
#endif

// EOF
