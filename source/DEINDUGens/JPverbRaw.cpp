//----------------------------------------------------------
// name: "JPverbRaw"
// version: "1.1"
// author: "Julian Parker, bug fixes by Till Bovermann"
// license: "GPL2+"
// copyright: "(c) Julian Parker 2013"
//
// Code generated with Faust 0.12.0 (http://faust.grame.fr)
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
	int 	iVec0[2];
	FAUSTFLOAT 	fslider1;
	FAUSTFLOAT 	fslider2;
	float 	fConst0;
	float 	fConst1;
	FAUSTFLOAT 	fslider3;
	float 	fRec15[2];
	int 	iTempPerm0;
	float 	fRec16[2];
	float 	fTempPerm1;
	float 	fTempPerm2;
	float 	fTempPerm3;
	float 	fTempPerm4;
	float 	fTempPerm5;
	FAUSTFLOAT 	fslider4;
	FAUSTFLOAT 	fslider5;
	FAUSTFLOAT 	fslider6;
	float 	fConst2;
	FAUSTFLOAT 	fslider7;
	float 	fTempPerm6;
	float 	fTempPerm7;
	float 	fTempPerm8;
	int 	IOTA;
	float 	fVec1[16384];
	float 	fRec53[2];
	float 	fTempPerm9;
	int 	iTempPerm10;
	float 	fTempPerm11;
	float 	fTempPerm12;
	float 	fTempPerm13;
	float 	fRec52[2];
	float 	fRec50[2];
	float 	fTempPerm14;
	float 	fVec2[16384];
	float 	fRec55[2];
	float 	fTempPerm15;
	int 	iTempPerm16;
	float 	fTempPerm17;
	float 	fTempPerm18;
	float 	fTempPerm19;
	float 	fRec54[2];
	float 	fRec51[2];
	float 	fTempPerm20;
	float 	fTempPerm21;
	float 	fTempPerm22;
	float 	fTempPerm23;
	float 	fTempPerm24;
	float 	fVec3[16384];
	float 	fRec56[2];
	float 	fTempPerm25;
	int 	iTempPerm26;
	float 	fTempPerm27;
	float 	fTempPerm28;
	float 	fTempPerm29;
	float 	fRec49[2];
	float 	fRec47[2];
	float 	fTempPerm30;
	float 	fVec4[16384];
	float 	fRec58[2];
	float 	fTempPerm31;
	int 	iTempPerm32;
	float 	fTempPerm33;
	float 	fTempPerm34;
	float 	fTempPerm35;
	float 	fRec57[2];
	float 	fRec48[2];
	float 	fTempPerm36;
	float 	fTempPerm37;
	float 	fTempPerm38;
	float 	fTempPerm39;
	float 	fTempPerm40;
	float 	fVec5[16384];
	float 	fRec59[2];
	float 	fTempPerm41;
	int 	iTempPerm42;
	float 	fTempPerm43;
	float 	fTempPerm44;
	float 	fTempPerm45;
	float 	fRec46[2];
	float 	fRec44[2];
	float 	fTempPerm46;
	float 	fVec6[16384];
	float 	fRec61[2];
	float 	fTempPerm47;
	int 	iTempPerm48;
	float 	fTempPerm49;
	float 	fTempPerm50;
	float 	fTempPerm51;
	float 	fRec60[2];
	float 	fRec45[2];
	float 	fTempPerm52;
	float 	fTempPerm53;
	float 	fTempPerm54;
	float 	fTempPerm55;
	float 	fTempPerm56;
	float 	fVec7[16384];
	float 	fRec62[2];
	float 	fTempPerm57;
	int 	iTempPerm58;
	float 	fTempPerm59;
	float 	fTempPerm60;
	float 	fTempPerm61;
	float 	fRec43[2];
	float 	fRec41[2];
	float 	fTempPerm62;
	float 	fVec8[16384];
	float 	fRec64[2];
	float 	fTempPerm63;
	int 	iTempPerm64;
	float 	fTempPerm65;
	float 	fTempPerm66;
	float 	fTempPerm67;
	float 	fRec63[2];
	float 	fRec42[2];
	float 	fTempPerm68;
	float 	fTempPerm69;
	float 	fTempPerm70;
	float 	fTempPerm71;
	float 	fTempPerm72;
	float 	fVec9[16384];
	float 	fRec65[2];
	float 	fTempPerm73;
	int 	iTempPerm74;
	float 	fTempPerm75;
	float 	fTempPerm76;
	float 	fTempPerm77;
	float 	fRec40[2];
	float 	fRec38[2];
	float 	fTempPerm78;
	float 	fVec10[16384];
	float 	fRec67[2];
	float 	fTempPerm79;
	int 	iTempPerm80;
	float 	fTempPerm81;
	float 	fTempPerm82;
	float 	fTempPerm83;
	float 	fRec66[2];
	float 	fRec39[2];
	float 	fTempPerm84;
	float 	fVec11[1024];
	float 	fTempPerm85;
	float 	fTempPerm86;
	int 	iTempPerm87;
	int 	iTempPerm88;
	float 	fTempPerm89;
	float 	fTempPerm90;
	float 	fTempPerm91;
	float 	fTempPerm92;
	float 	fTempPerm93;
	float 	fTempPerm94;
	float 	fTempPerm95;
	float 	fTempPerm96;
	float 	fTempPerm97;
	float 	fTempPerm98;
	int 	iTempPerm99;
	float 	fTempPerm100;
	float 	fTempPerm101;
	float 	fTempPerm102;
	int 	iTempPerm103;
	float 	fTempPerm104;
	float 	fTempPerm105;
	float 	fTempPerm106;
	int 	iTempPerm107;
	float 	fTempPerm108;
	int 	iTempPerm109;
	float 	fTempPerm110;
	float 	fVec12[16384];
	float 	fRec68[2];
	float 	fTempPerm111;
	int 	iTempPerm112;
	float 	fTempPerm113;
	float 	fTempPerm114;
	float 	fTempPerm115;
	float 	fRec37[2];
	float 	fTempPerm116;
	float 	fTempPerm117;
	float 	fVec13[1024];
	float 	fTempPerm118;
	float 	fTempPerm119;
	int 	iTempPerm120;
	float 	fTempPerm121;
	float 	fTempPerm122;
	float 	fTempPerm123;
	float 	fTempPerm124;
	float 	fTempPerm125;
	float 	fTempPerm126;
	float 	fTempPerm127;
	float 	fVec14[16384];
	float 	fRec70[2];
	float 	fTempPerm128;
	int 	iTempPerm129;
	float 	fTempPerm130;
	float 	fTempPerm131;
	float 	fTempPerm132;
	float 	fRec69[2];
	float 	fTempPerm133;
	float 	fTempPerm134;
	float 	fVec15[16384];
	float 	fRec71[2];
	float 	fTempPerm135;
	int 	iTempPerm136;
	float 	fTempPerm137;
	float 	fTempPerm138;
	float 	fTempPerm139;
	float 	fRec36[2];
	float 	fRec34[2];
	float 	fTempPerm140;
	float 	fVec16[16384];
	float 	fRec73[2];
	float 	fTempPerm141;
	int 	iTempPerm142;
	float 	fTempPerm143;
	float 	fTempPerm144;
	float 	fTempPerm145;
	float 	fRec72[2];
	float 	fRec35[2];
	float 	fTempPerm146;
	float 	fTempPerm147;
	float 	fTempPerm148;
	float 	fTempPerm149;
	float 	fTempPerm150;
	float 	fVec17[16384];
	float 	fRec74[2];
	float 	fTempPerm151;
	int 	iTempPerm152;
	float 	fTempPerm153;
	float 	fTempPerm154;
	float 	fTempPerm155;
	float 	fRec33[2];
	float 	fRec31[2];
	float 	fTempPerm156;
	float 	fVec18[16384];
	float 	fRec76[2];
	float 	fTempPerm157;
	int 	iTempPerm158;
	int 	iTempPerm159;
	float 	fTempPerm160;
	float 	fTempPerm161;
	float 	fTempPerm162;
	float 	fTempPerm163;
	float 	fRec75[2];
	float 	fRec32[2];
	float 	fTempPerm164;
	float 	fTempPerm165;
	float 	fTempPerm166;
	float 	fTempPerm167;
	float 	fTempPerm168;
	float 	fVec19[16384];
	float 	fRec77[2];
	float 	fTempPerm169;
	int 	iTempPerm170;
	float 	fTempPerm171;
	float 	fTempPerm172;
	float 	fTempPerm173;
	float 	fRec30[2];
	float 	fRec28[2];
	float 	fTempPerm174;
	float 	fVec20[16384];
	float 	fRec79[2];
	float 	fTempPerm175;
	int 	iTempPerm176;
	int 	iTempPerm177;
	float 	fTempPerm178;
	float 	fTempPerm179;
	float 	fTempPerm180;
	float 	fTempPerm181;
	float 	fRec78[2];
	float 	fRec29[2];
	float 	fTempPerm182;
	float 	fTempPerm183;
	float 	fTempPerm184;
	float 	fTempPerm185;
	float 	fTempPerm186;
	float 	fVec21[16384];
	float 	fRec80[2];
	float 	fTempPerm187;
	int 	iTempPerm188;
	int 	iTempPerm189;
	float 	fTempPerm190;
	float 	fTempPerm191;
	float 	fTempPerm192;
	float 	fTempPerm193;
	float 	fRec27[2];
	float 	fRec25[2];
	float 	fTempPerm194;
	float 	fVec22[16384];
	float 	fRec82[2];
	float 	fTempPerm195;
	int 	iTempPerm196;
	int 	iTempPerm197;
	float 	fTempPerm198;
	float 	fTempPerm199;
	float 	fTempPerm200;
	float 	fTempPerm201;
	float 	fRec81[2];
	float 	fRec26[2];
	float 	fTempPerm202;
	float 	fTempPerm203;
	float 	fTempPerm204;
	float 	fTempPerm205;
	float 	fTempPerm206;
	float 	fVec23[16384];
	float 	fRec83[2];
	float 	fTempPerm207;
	int 	iTempPerm208;
	float 	fTempPerm209;
	float 	fTempPerm210;
	float 	fTempPerm211;
	float 	fRec24[2];
	float 	fRec22[2];
	float 	fTempPerm212;
	float 	fVec24[16384];
	float 	fRec85[2];
	float 	fTempPerm213;
	int 	iTempPerm214;
	int 	iTempPerm215;
	float 	fTempPerm216;
	float 	fTempPerm217;
	float 	fTempPerm218;
	float 	fTempPerm219;
	float 	fRec84[2];
	float 	fRec23[2];
	float 	fTempPerm220;
	float 	fVec25[16384];
	int 	iTempPerm221;
	int 	iTempPerm222;
	float 	fTempPerm223;
	float 	fTempPerm224;
	float 	fTempPerm225;
	float 	fTempPerm226;
	float 	fTempPerm227;
	float 	fTempPerm228;
	int 	iTempPerm229;
	float 	fTempPerm230;
	float 	fTempPerm231;
	float 	fTempPerm232;
	int 	iTempPerm233;
	float 	fTempPerm234;
	float 	fTempPerm235;
	int 	iTempPerm236;
	float 	fTempPerm237;
	int 	iTempPerm238;
	float 	fTempPerm239;
	float 	fTempPerm240;
	float 	fVec26[16384];
	float 	fRec86[2];
	float 	fTempPerm241;
	int 	iTempPerm242;
	float 	fTempPerm243;
	float 	fTempPerm244;
	float 	fTempPerm245;
	float 	fRec21[2];
	float 	fRec20[2];
	float 	fRec19[3];
	float 	fRec18[3];
	FAUSTFLOAT 	fslider8;
	float 	fTempPerm246;
	float 	fRec17[3];
	FAUSTFLOAT 	fslider9;
	float 	fRec92[2];
	float 	fRec91[3];
	float 	fRec90[3];
	float 	fTempPerm247;
	float 	fTempPerm248;
	float 	fVec27[2];
	float 	fRec89[2];
	float 	fRec88[3];
	float 	fRec87[3];
	FAUSTFLOAT 	fslider10;
	float 	fRec95[2];
	float 	fRec94[3];
	float 	fRec93[3];
	float 	fTempPerm249;
	float 	fVec28[1024];
	float 	fRec14[2];
	float 	fTempPerm250;
	float 	fTempPerm251;
	float 	fVec29[16384];
	float 	fTempPerm252;
	float 	fTempPerm253;
	int 	iTempPerm254;
	float 	fTempPerm255;
	float 	fTempPerm256;
	float 	fTempPerm257;
	float 	fTempPerm258;
	float 	fTempPerm259;
	float 	fTempPerm260;
	float 	fTempPerm261;
	float 	fVec30[16384];
	float 	fRec102[2];
	float 	fTempPerm262;
	int 	iTempPerm263;
	float 	fTempPerm264;
	float 	fTempPerm265;
	float 	fTempPerm266;
	float 	fRec101[2];
	float 	fRec100[2];
	float 	fRec99[3];
	float 	fRec98[3];
	float 	fTempPerm267;
	float 	fRec97[3];
	float 	fRec108[2];
	float 	fRec107[3];
	float 	fRec106[3];
	float 	fTempPerm268;
	float 	fTempPerm269;
	float 	fVec31[2];
	float 	fRec105[2];
	float 	fRec104[3];
	float 	fRec103[3];
	float 	fRec111[2];
	float 	fRec110[3];
	float 	fRec109[3];
	float 	fTempPerm270;
	float 	fVec32[1024];
	float 	fRec96[2];
	float 	fTempPerm271;
	float 	fTempPerm272;
	float 	fVec33[16384];
	float 	fRec13[2];
	float 	fRec11[2];
	float 	fTempPerm273;
	float 	fVec34[16384];
	float 	fRec113[2];
	float 	fTempPerm274;
	int 	iTempPerm275;
	float 	fTempPerm276;
	float 	fTempPerm277;
	float 	fTempPerm278;
	float 	fRec112[2];
	float 	fRec12[2];
	float 	fTempPerm279;
	float 	fTempPerm280;
	float 	fTempPerm281;
	float 	fTempPerm282;
	float 	fTempPerm283;
	float 	fVec35[16384];
	float 	fRec10[2];
	float 	fRec8[2];
	float 	fTempPerm284;
	float 	fVec36[16384];
	float 	fRec114[2];
	float 	fRec9[2];
	float 	fTempPerm285;
	float 	fTempPerm286;
	float 	fTempPerm287;
	float 	fTempPerm288;
	float 	fTempPerm289;
	float 	fVec37[16384];
	float 	fRec7[2];
	float 	fRec5[2];
	float 	fTempPerm290;
	float 	fVec38[16384];
	float 	fRec116[2];
	float 	fTempPerm291;
	int 	iTempPerm292;
	float 	fTempPerm293;
	float 	fTempPerm294;
	float 	fTempPerm295;
	float 	fRec115[2];
	float 	fRec6[2];
	float 	fTempPerm296;
	float 	fTempPerm297;
	float 	fTempPerm298;
	float 	fTempPerm299;
	float 	fTempPerm300;
	float 	fVec39[16384];
	float 	fRec117[2];
	float 	fTempPerm301;
	int 	iTempPerm302;
	float 	fTempPerm303;
	float 	fTempPerm304;
	float 	fTempPerm305;
	float 	fRec4[2];
	float 	fRec2[2];
	float 	fTempPerm306;
	float 	fVec40[16384];
	float 	fRec118[2];
	float 	fRec3[2];
	float 	fRec0[2];
	float 	fRec1[2];
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "JPverbRaw");
		m->declare("version", "1.1");
		m->declare("author", "Julian Parker, bug fixes by Till Bovermann");
		m->declare("license", "GPL2+");
		m->declare("copyright", "(c) Julian Parker 2013");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.0");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("analyzers.lib/name", "Faust Analyzer Library");
		m->declare("analyzers.lib/version", "0.0");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
	}

	virtual int getNumInputs() { return 2; }
	virtual int getNumOutputs() { return 2; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(1.92e+05f, max(1.0f, (float)fSamplingFreq));
		fConst1 = (6.2831855f / fConst0);
		iTempPerm0 = 0;
		fTempPerm1 = 0;
		fTempPerm2 = 0;
		fTempPerm3 = 0;
		fTempPerm4 = 0;
		fTempPerm5 = 0;
		fConst2 = (3.1415927f / fConst0);
		fTempPerm6 = 0;
		fTempPerm7 = 0;
		fTempPerm8 = 0;
		fTempPerm9 = 0;
		iTempPerm10 = 0;
		fTempPerm11 = 0;
		fTempPerm12 = 0;
		fTempPerm13 = 0;
		fTempPerm14 = 0;
		fTempPerm15 = 0;
		iTempPerm16 = 0;
		fTempPerm17 = 0;
		fTempPerm18 = 0;
		fTempPerm19 = 0;
		fTempPerm20 = 0;
		fTempPerm21 = 0;
		fTempPerm22 = 0;
		fTempPerm23 = 0;
		fTempPerm24 = 0;
		fTempPerm25 = 0;
		iTempPerm26 = 0;
		fTempPerm27 = 0;
		fTempPerm28 = 0;
		fTempPerm29 = 0;
		fTempPerm30 = 0;
		fTempPerm31 = 0;
		iTempPerm32 = 0;
		fTempPerm33 = 0;
		fTempPerm34 = 0;
		fTempPerm35 = 0;
		fTempPerm36 = 0;
		fTempPerm37 = 0;
		fTempPerm38 = 0;
		fTempPerm39 = 0;
		fTempPerm40 = 0;
		fTempPerm41 = 0;
		iTempPerm42 = 0;
		fTempPerm43 = 0;
		fTempPerm44 = 0;
		fTempPerm45 = 0;
		fTempPerm46 = 0;
		fTempPerm47 = 0;
		iTempPerm48 = 0;
		fTempPerm49 = 0;
		fTempPerm50 = 0;
		fTempPerm51 = 0;
		fTempPerm52 = 0;
		fTempPerm53 = 0;
		fTempPerm54 = 0;
		fTempPerm55 = 0;
		fTempPerm56 = 0;
		fTempPerm57 = 0;
		iTempPerm58 = 0;
		fTempPerm59 = 0;
		fTempPerm60 = 0;
		fTempPerm61 = 0;
		fTempPerm62 = 0;
		fTempPerm63 = 0;
		iTempPerm64 = 0;
		fTempPerm65 = 0;
		fTempPerm66 = 0;
		fTempPerm67 = 0;
		fTempPerm68 = 0;
		fTempPerm69 = 0;
		fTempPerm70 = 0;
		fTempPerm71 = 0;
		fTempPerm72 = 0;
		fTempPerm73 = 0;
		iTempPerm74 = 0;
		fTempPerm75 = 0;
		fTempPerm76 = 0;
		fTempPerm77 = 0;
		fTempPerm78 = 0;
		fTempPerm79 = 0;
		iTempPerm80 = 0;
		fTempPerm81 = 0;
		fTempPerm82 = 0;
		fTempPerm83 = 0;
		fTempPerm84 = 0;
		fTempPerm85 = 0;
		fTempPerm86 = 0;
		iTempPerm87 = 0;
		iTempPerm88 = 0;
		fTempPerm89 = 0;
		fTempPerm90 = 0;
		fTempPerm91 = 0;
		fTempPerm92 = 0;
		fTempPerm93 = 0;
		fTempPerm94 = 0;
		fTempPerm95 = 0;
		fTempPerm96 = 0;
		fTempPerm97 = 0;
		fTempPerm98 = 0;
		iTempPerm99 = 0;
		fTempPerm100 = 0;
		fTempPerm101 = 0;
		fTempPerm102 = 0;
		iTempPerm103 = 0;
		fTempPerm104 = 0;
		fTempPerm105 = 0;
		fTempPerm106 = 0;
		iTempPerm107 = 0;
		fTempPerm108 = 0;
		iTempPerm109 = 0;
		fTempPerm110 = 0;
		fTempPerm111 = 0;
		iTempPerm112 = 0;
		fTempPerm113 = 0;
		fTempPerm114 = 0;
		fTempPerm115 = 0;
		fTempPerm116 = 0;
		fTempPerm117 = 0;
		fTempPerm118 = 0;
		fTempPerm119 = 0;
		iTempPerm120 = 0;
		fTempPerm121 = 0;
		fTempPerm122 = 0;
		fTempPerm123 = 0;
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
		fTempPerm135 = 0;
		iTempPerm136 = 0;
		fTempPerm137 = 0;
		fTempPerm138 = 0;
		fTempPerm139 = 0;
		fTempPerm140 = 0;
		fTempPerm141 = 0;
		iTempPerm142 = 0;
		fTempPerm143 = 0;
		fTempPerm144 = 0;
		fTempPerm145 = 0;
		fTempPerm146 = 0;
		fTempPerm147 = 0;
		fTempPerm148 = 0;
		fTempPerm149 = 0;
		fTempPerm150 = 0;
		fTempPerm151 = 0;
		iTempPerm152 = 0;
		fTempPerm153 = 0;
		fTempPerm154 = 0;
		fTempPerm155 = 0;
		fTempPerm156 = 0;
		fTempPerm157 = 0;
		iTempPerm158 = 0;
		iTempPerm159 = 0;
		fTempPerm160 = 0;
		fTempPerm161 = 0;
		fTempPerm162 = 0;
		fTempPerm163 = 0;
		fTempPerm164 = 0;
		fTempPerm165 = 0;
		fTempPerm166 = 0;
		fTempPerm167 = 0;
		fTempPerm168 = 0;
		fTempPerm169 = 0;
		iTempPerm170 = 0;
		fTempPerm171 = 0;
		fTempPerm172 = 0;
		fTempPerm173 = 0;
		fTempPerm174 = 0;
		fTempPerm175 = 0;
		iTempPerm176 = 0;
		iTempPerm177 = 0;
		fTempPerm178 = 0;
		fTempPerm179 = 0;
		fTempPerm180 = 0;
		fTempPerm181 = 0;
		fTempPerm182 = 0;
		fTempPerm183 = 0;
		fTempPerm184 = 0;
		fTempPerm185 = 0;
		fTempPerm186 = 0;
		fTempPerm187 = 0;
		iTempPerm188 = 0;
		iTempPerm189 = 0;
		fTempPerm190 = 0;
		fTempPerm191 = 0;
		fTempPerm192 = 0;
		fTempPerm193 = 0;
		fTempPerm194 = 0;
		fTempPerm195 = 0;
		iTempPerm196 = 0;
		iTempPerm197 = 0;
		fTempPerm198 = 0;
		fTempPerm199 = 0;
		fTempPerm200 = 0;
		fTempPerm201 = 0;
		fTempPerm202 = 0;
		fTempPerm203 = 0;
		fTempPerm204 = 0;
		fTempPerm205 = 0;
		fTempPerm206 = 0;
		fTempPerm207 = 0;
		iTempPerm208 = 0;
		fTempPerm209 = 0;
		fTempPerm210 = 0;
		fTempPerm211 = 0;
		fTempPerm212 = 0;
		fTempPerm213 = 0;
		iTempPerm214 = 0;
		iTempPerm215 = 0;
		fTempPerm216 = 0;
		fTempPerm217 = 0;
		fTempPerm218 = 0;
		fTempPerm219 = 0;
		fTempPerm220 = 0;
		iTempPerm221 = 0;
		iTempPerm222 = 0;
		fTempPerm223 = 0;
		fTempPerm224 = 0;
		fTempPerm225 = 0;
		fTempPerm226 = 0;
		fTempPerm227 = 0;
		fTempPerm228 = 0;
		iTempPerm229 = 0;
		fTempPerm230 = 0;
		fTempPerm231 = 0;
		fTempPerm232 = 0;
		iTempPerm233 = 0;
		fTempPerm234 = 0;
		fTempPerm235 = 0;
		iTempPerm236 = 0;
		fTempPerm237 = 0;
		iTempPerm238 = 0;
		fTempPerm239 = 0;
		fTempPerm240 = 0;
		fTempPerm241 = 0;
		iTempPerm242 = 0;
		fTempPerm243 = 0;
		fTempPerm244 = 0;
		fTempPerm245 = 0;
		fTempPerm246 = 0;
		fTempPerm247 = 0;
		fTempPerm248 = 0;
		fTempPerm249 = 0;
		fTempPerm250 = 0;
		fTempPerm251 = 0;
		fTempPerm252 = 0;
		fTempPerm253 = 0;
		iTempPerm254 = 0;
		fTempPerm255 = 0;
		fTempPerm256 = 0;
		fTempPerm257 = 0;
		fTempPerm258 = 0;
		fTempPerm259 = 0;
		fTempPerm260 = 0;
		fTempPerm261 = 0;
		fTempPerm262 = 0;
		iTempPerm263 = 0;
		fTempPerm264 = 0;
		fTempPerm265 = 0;
		fTempPerm266 = 0;
		fTempPerm267 = 0;
		fTempPerm268 = 0;
		fTempPerm269 = 0;
		fTempPerm270 = 0;
		fTempPerm271 = 0;
		fTempPerm272 = 0;
		fTempPerm273 = 0;
		fTempPerm274 = 0;
		iTempPerm275 = 0;
		fTempPerm276 = 0;
		fTempPerm277 = 0;
		fTempPerm278 = 0;
		fTempPerm279 = 0;
		fTempPerm280 = 0;
		fTempPerm281 = 0;
		fTempPerm282 = 0;
		fTempPerm283 = 0;
		fTempPerm284 = 0;
		fTempPerm285 = 0;
		fTempPerm286 = 0;
		fTempPerm287 = 0;
		fTempPerm288 = 0;
		fTempPerm289 = 0;
		fTempPerm290 = 0;
		fTempPerm291 = 0;
		iTempPerm292 = 0;
		fTempPerm293 = 0;
		fTempPerm294 = 0;
		fTempPerm295 = 0;
		fTempPerm296 = 0;
		fTempPerm297 = 0;
		fTempPerm298 = 0;
		fTempPerm299 = 0;
		fTempPerm300 = 0;
		fTempPerm301 = 0;
		iTempPerm302 = 0;
		fTempPerm303 = 0;
		fTempPerm304 = 0;
		fTempPerm305 = 0;
		fTempPerm306 = 0;
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.707f;
		fslider1 = 0.0f;
		fslider2 = 0.1f;
		fslider3 = 2.0f;
		fslider4 = 1.0f;
		fslider5 = 1.0f;
		fslider6 = 1.0f;
		fslider7 = 2e+03f;
		fslider8 = 5e+02f;
		fslider9 = 1.0f;
		fslider10 = 1.0f;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) iVec0[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
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
		for (int i=0; i<16384; i++) fVec12[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<1024; i++) fVec13[i] = 0;
		for (int i=0; i<16384; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<16384; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<16384; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<16384; i++) fVec17[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<16384; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<16384; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<16384; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<16384; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<16384; i++) fVec22[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<16384; i++) fVec23[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<16384; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<16384; i++) fVec25[i] = 0;
		for (int i=0; i<16384; i++) fVec26[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<3; i++) fRec19[i] = 0;
		for (int i=0; i<3; i++) fRec18[i] = 0;
		for (int i=0; i<3; i++) fRec17[i] = 0;
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
		ui_interface->openVerticalBox("JPverbRaw");
		ui_interface->addHorizontalSlider("damp", &fslider1, 0.0f, 0.0f, 0.999f, 0.0001f);
		ui_interface->addHorizontalSlider("earlyDiff", &fslider0, 0.707f, 0.0f, 0.99f, 0.001f);
		ui_interface->addHorizontalSlider("highBand", &fslider7, 2e+03f, 1e+03f, 1e+04f, 0.1f);
		ui_interface->addHorizontalSlider("highX", &fslider6, 1.0f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("lowBand", &fslider8, 5e+02f, 1e+02f, 6e+03f, 0.1f);
		ui_interface->addHorizontalSlider("lowX", &fslider10, 1.0f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("mDepth", &fslider2, 0.1f, 0.0f, 1.0f, 0.001f);
		ui_interface->addHorizontalSlider("mFreq", &fslider3, 2.0f, 0.0f, 1e+01f, 0.01f);
		ui_interface->addHorizontalSlider("midX", &fslider9, 1.0f, 0.0f, 1.0f, 0.01f);
		ui_interface->addHorizontalSlider("size", &fslider4, 1.0f, 0.5f, 3.0f, 0.01f);
		ui_interface->addHorizontalSlider("t60", &fslider5, 1.0f, 0.1f, 6e+01f, 0.1f);
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		//zone1
		//zone2
		float 	fSlow0 = float(fslider0);
		float 	fSlow1 = cosf(fSlow0);
		float 	fSlow2 = sinf(fSlow0);
		float 	fSlow3 = (0 - fSlow2);
		float 	fSlow4 = float(fslider1);
		float 	fSlow5 = (1.0f - fSlow4);
		float 	fSlow6 = float(fslider2);
		float 	fSlow7 = (50 * fSlow6);
		float 	fSlow8 = (fConst1 * float(fslider3));
		float 	fSlow9 = sinf(fSlow8);
		float 	fSlow10 = cosf(fSlow8);
		float 	fSlow11 = (0 - fSlow9);
		float 	fSlow12 = float(fslider4);
		float 	fSlow13 = powf(10,(0 - (0.51f * (((1.25f * fSlow12) + -0.25f) / float(fslider5)))));
		float 	fSlow14 = float(fslider6);
		float 	fSlow15 = tanf((fConst2 * float(fslider7)));
		float 	fSlow16 = (1.0f / fSlow15);
		float 	fSlow17 = (((fSlow16 + 0.618034f) / fSlow15) + 1);
		float 	fSlow18 = (1.0f / fSlow17);
		float 	fSlow19 = (1.0f / faustpower<2>(fSlow15));
		float 	fSlow20 = (1.0f / (((fSlow16 + 1.618034f) / fSlow15) + 1));
		float 	fSlow21 = (fSlow16 + 1);
		float 	fSlow22 = (0 - ((1 - fSlow16) / fSlow21));
		float 	fSlow23 = (1.0f / fSlow21);
		int 	iSlow24 = primes((10 * fSlow12));
		float 	fSlow25 = (0.0001f * iSlow24);
		int 	iSlow26 = primes((110 * fSlow12));
		float 	fSlow27 = (0.0001f * iSlow26);
		int 	iSlow28 = primes((40 * fSlow12));
		float 	fSlow29 = (0.0001f * iSlow28);
		int 	iSlow30 = primes((140 * fSlow12));
		float 	fSlow31 = (0.0001f * iSlow30);
		int 	iSlow32 = primes((70 * fSlow12));
		float 	fSlow33 = (0.0001f * iSlow32);
		int 	iSlow34 = primes((170 * fSlow12));
		float 	fSlow35 = (0.0001f * iSlow34);
		int 	iSlow36 = primes((100 * fSlow12));
		float 	fSlow37 = (0.0001f * iSlow36);
		int 	iSlow38 = primes((200 * fSlow12));
		float 	fSlow39 = (0.0001f * iSlow38);
		int 	iSlow40 = primes((130 * fSlow12));
		float 	fSlow41 = (0.0001f * iSlow40);
		int 	iSlow42 = primes((230 * fSlow12));
		float 	fSlow43 = (0.0001f * iSlow42);
		int 	iSlow44 = primes((54 * fSlow12));
		float 	fSlow45 = (0.005f * iSlow44);
		float 	fSlow46 = (0 - (5e+01f * fSlow6));
		int 	iSlow47 = primes((204 * fSlow12));
		float 	fSlow48 = (0.005f * iSlow47);
		int 	iSlow49 = primes((125 * fSlow12));
		float 	fSlow50 = (0.0001f * iSlow49);
		int 	iSlow51 = primes((25 * fSlow12));
		float 	fSlow52 = (0.0001f * iSlow51);
		int 	iSlow53 = primes((155 * fSlow12));
		float 	fSlow54 = (0.0001f * iSlow53);
		int 	iSlow55 = primes((55 * fSlow12));
		float 	fSlow56 = (0.0001f * iSlow55);
		int 	iSlow57 = primes((185 * fSlow12));
		float 	fSlow58 = (0.0001f * iSlow57);
		int 	iSlow59 = primes((85 * fSlow12));
		float 	fSlow60 = (0.0001f * iSlow59);
		int 	iSlow61 = primes((215 * fSlow12));
		float 	fSlow62 = (0.0001f * iSlow61);
		int 	iSlow63 = primes((115 * fSlow12));
		float 	fSlow64 = (0.0001f * iSlow63);
		int 	iSlow65 = primes((245 * fSlow12));
		float 	fSlow66 = (0.0001f * iSlow65);
		int 	iSlow67 = primes((145 * fSlow12));
		float 	fSlow68 = (0.0001f * iSlow67);
		int 	iSlow69 = primes((134 * fSlow12));
		float 	fSlow70 = (0.005f * iSlow69);
		float 	fSlow71 = (0 - fSlow16);
		float 	fSlow72 = (2 * (1 - fSlow19));
		float 	fSlow73 = (((fSlow16 + -1.618034f) / fSlow15) + 1);
		float 	fSlow74 = (2 * (0 - fSlow19));
		float 	fSlow75 = (((fSlow16 + -0.618034f) / fSlow15) + 1);
		float 	fSlow76 = tanf((fConst2 * float(fslider8)));
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
		float 	fSlow90 = (1.0f / (fSlow76 * fSlow17));
		float 	fSlow91 = (((fSlow77 + -1.618034f) / fSlow76) + 1);
		float 	fSlow92 = (((fSlow77 + -0.618034f) / fSlow76) + 1);
		float 	fSlow93 = float(fslider10);
		int 	iSlow94 = primes((34 * fSlow12));
		float 	fSlow95 = (0.005f * iSlow94);
		int 	iSlow96 = primes((240 * fSlow12));
		float 	fSlow97 = (0.0001f * iSlow96);
		int 	iSlow98 = primes((190 * fSlow12));
		float 	fSlow99 = (0.0001f * iSlow98);
		int 	iSlow100 = primes((175 * fSlow12));
		float 	fSlow101 = (0.0001f * iSlow100);
		//zone2b
		//zone3
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		//LoopGraphScalar
		for (int i=0; i<count; i++) {
			iVec0[0] = 1;
			fRec15[0] = ((fSlow9 * fRec16[1]) + (fSlow10 * fRec15[1]));
			iTempPerm0 = (1 - iVec0[1]);
			fRec16[0] = (((fSlow10 * fRec16[1]) + (fSlow11 * fRec15[1])) + iTempPerm0);
			fTempPerm1 = (fSlow7 * (fRec15[0] + 1));
			fTempPerm2 = (fTempPerm1 + 3.500005f);
			fTempPerm3 = floorf(fTempPerm2);
			fTempPerm4 = (fTempPerm1 + (1.0f - fTempPerm3));
			fTempPerm5 = (0 - (0.25f * fTempPerm4));
			fTempPerm6 = ((0.760314f * fRec0[1]) - (0.64955574f * fRec50[1]));
			fTempPerm7 = ((0.760314f * fRec1[1]) - (0.64955574f * fRec51[1]));
			fTempPerm8 = ((0.70710677f * fTempPerm6) - (0.70710677f * fTempPerm7));
			fVec1[IOTA&16383] = fTempPerm8;
			fRec53[0] = (fSlow25 + (0.9999f * ((iSlow24 * iTempPerm0) + fRec53[1])));
			fTempPerm9 = (fRec53[0] + -1.49999f);
			iTempPerm10 = int(min(8192, max(0, int(fTempPerm9))));
			fTempPerm11 = floorf(fTempPerm9);
			fTempPerm12 = (fTempPerm11 + (2.0f - fRec53[0]));
			fTempPerm13 = (fRec53[0] - fTempPerm11);
			fRec52[0] = (fVec1[(IOTA-int((iTempPerm10 + 1)))&16383] + ((fRec52[1] * (0 - (fTempPerm12 / fTempPerm13))) + ((fTempPerm12 * fVec1[(IOTA-iTempPerm10)&16383]) / fTempPerm13)));
			fRec50[0] = fRec52[0];
			fTempPerm14 = ((0.70710677f * fTempPerm6) + (0.70710677f * fTempPerm7));
			fVec2[IOTA&16383] = fTempPerm14;
			fRec55[0] = (fSlow27 + (0.9999f * ((iSlow26 * iTempPerm0) + fRec55[1])));
			fTempPerm15 = (fRec55[0] + -1.49999f);
			iTempPerm16 = int(min(8192, max(0, int(fTempPerm15))));
			fTempPerm17 = floorf(fTempPerm15);
			fTempPerm18 = (fTempPerm17 + (2.0f - fRec55[0]));
			fTempPerm19 = (fRec55[0] - fTempPerm17);
			fRec54[0] = (fVec2[(IOTA-int((iTempPerm16 + 1)))&16383] + ((fRec54[1] * (0 - (fTempPerm18 / fTempPerm19))) + ((fTempPerm18 * fVec2[(IOTA-iTempPerm16)&16383]) / fTempPerm19)));
			fRec51[0] = fRec54[0];
			fTempPerm20 = ((0.760314f * fRec50[1]) + (0.64955574f * fRec0[1]));
			fTempPerm21 = ((0.760314f * fTempPerm20) - (0.64955574f * fRec47[1]));
			fTempPerm22 = ((0.760314f * fRec51[1]) + (0.64955574f * fRec1[1]));
			fTempPerm23 = ((0.760314f * fTempPerm22) - (0.64955574f * fRec48[1]));
			fTempPerm24 = ((0.70710677f * fTempPerm21) - (0.70710677f * fTempPerm23));
			fVec3[IOTA&16383] = fTempPerm24;
			fRec56[0] = (fSlow29 + (0.9999f * ((iSlow28 * iTempPerm0) + fRec56[1])));
			fTempPerm25 = (fRec56[0] + -1.49999f);
			iTempPerm26 = int(min(8192, max(0, int(fTempPerm25))));
			fTempPerm27 = floorf(fTempPerm25);
			fTempPerm28 = (fTempPerm27 + (2.0f - fRec56[0]));
			fTempPerm29 = (fRec56[0] - fTempPerm27);
			fRec49[0] = (fVec3[(IOTA-int((iTempPerm26 + 1)))&16383] + ((fRec49[1] * (0 - (fTempPerm28 / fTempPerm29))) + ((fTempPerm28 * fVec3[(IOTA-iTempPerm26)&16383]) / fTempPerm29)));
			fRec47[0] = fRec49[0];
			fTempPerm30 = ((0.70710677f * fTempPerm21) + (0.70710677f * fTempPerm23));
			fVec4[IOTA&16383] = fTempPerm30;
			fRec58[0] = (fSlow31 + (0.9999f * ((iSlow30 * iTempPerm0) + fRec58[1])));
			fTempPerm31 = (fRec58[0] + -1.49999f);
			iTempPerm32 = int(min(8192, max(0, int(fTempPerm31))));
			fTempPerm33 = floorf(fTempPerm31);
			fTempPerm34 = (fTempPerm33 + (2.0f - fRec58[0]));
			fTempPerm35 = (fRec58[0] - fTempPerm33);
			fRec57[0] = (fVec4[(IOTA-int((iTempPerm32 + 1)))&16383] + ((fRec57[1] * (0 - (fTempPerm34 / fTempPerm35))) + ((fTempPerm34 * fVec4[(IOTA-iTempPerm32)&16383]) / fTempPerm35)));
			fRec48[0] = fRec57[0];
			fTempPerm36 = ((0.760314f * fRec47[1]) + (0.64955574f * fTempPerm20));
			fTempPerm37 = ((0.760314f * fTempPerm36) - (0.64955574f * fRec44[1]));
			fTempPerm38 = ((0.760314f * fRec48[1]) + (0.64955574f * fTempPerm22));
			fTempPerm39 = ((0.760314f * fTempPerm38) - (0.64955574f * fRec45[1]));
			fTempPerm40 = ((0.70710677f * fTempPerm37) - (0.70710677f * fTempPerm39));
			fVec5[IOTA&16383] = fTempPerm40;
			fRec59[0] = (fSlow33 + (0.9999f * ((iSlow32 * iTempPerm0) + fRec59[1])));
			fTempPerm41 = (fRec59[0] + -1.49999f);
			iTempPerm42 = int(min(8192, max(0, int(fTempPerm41))));
			fTempPerm43 = floorf(fTempPerm41);
			fTempPerm44 = (fTempPerm43 + (2.0f - fRec59[0]));
			fTempPerm45 = (fRec59[0] - fTempPerm43);
			fRec46[0] = (fVec5[(IOTA-int((iTempPerm42 + 1)))&16383] + ((fRec46[1] * (0 - (fTempPerm44 / fTempPerm45))) + ((fTempPerm44 * fVec5[(IOTA-iTempPerm42)&16383]) / fTempPerm45)));
			fRec44[0] = fRec46[0];
			fTempPerm46 = ((0.70710677f * fTempPerm37) + (0.70710677f * fTempPerm39));
			fVec6[IOTA&16383] = fTempPerm46;
			fRec61[0] = (fSlow35 + (0.9999f * ((iSlow34 * iTempPerm0) + fRec61[1])));
			fTempPerm47 = (fRec61[0] + -1.49999f);
			iTempPerm48 = int(min(8192, max(0, int(fTempPerm47))));
			fTempPerm49 = floorf(fTempPerm47);
			fTempPerm50 = (fTempPerm49 + (2.0f - fRec61[0]));
			fTempPerm51 = (fRec61[0] - fTempPerm49);
			fRec60[0] = (fVec6[(IOTA-int((iTempPerm48 + 1)))&16383] + ((fRec60[1] * (0 - (fTempPerm50 / fTempPerm51))) + ((fTempPerm50 * fVec6[(IOTA-iTempPerm48)&16383]) / fTempPerm51)));
			fRec45[0] = fRec60[0];
			fTempPerm52 = ((0.760314f * fRec44[1]) + (0.64955574f * fTempPerm36));
			fTempPerm53 = ((0.760314f * fTempPerm52) - (0.64955574f * fRec41[1]));
			fTempPerm54 = ((0.760314f * fRec45[1]) + (0.64955574f * fTempPerm38));
			fTempPerm55 = ((0.760314f * fTempPerm54) - (0.64955574f * fRec42[1]));
			fTempPerm56 = ((0.70710677f * fTempPerm53) - (0.70710677f * fTempPerm55));
			fVec7[IOTA&16383] = fTempPerm56;
			fRec62[0] = (fSlow37 + (0.9999f * ((iSlow36 * iTempPerm0) + fRec62[1])));
			fTempPerm57 = (fRec62[0] + -1.49999f);
			iTempPerm58 = int(min(8192, max(0, int(fTempPerm57))));
			fTempPerm59 = floorf(fTempPerm57);
			fTempPerm60 = (fTempPerm59 + (2.0f - fRec62[0]));
			fTempPerm61 = (fRec62[0] - fTempPerm59);
			fRec43[0] = (fVec7[(IOTA-int((iTempPerm58 + 1)))&16383] + ((fRec43[1] * (0 - (fTempPerm60 / fTempPerm61))) + ((fTempPerm60 * fVec7[(IOTA-iTempPerm58)&16383]) / fTempPerm61)));
			fRec41[0] = fRec43[0];
			fTempPerm62 = ((0.70710677f * fTempPerm53) + (0.70710677f * fTempPerm55));
			fVec8[IOTA&16383] = fTempPerm62;
			fRec64[0] = (fSlow39 + (0.9999f * ((iSlow38 * iTempPerm0) + fRec64[1])));
			fTempPerm63 = (fRec64[0] + -1.49999f);
			iTempPerm64 = int(min(8192, max(0, int(fTempPerm63))));
			fTempPerm65 = floorf(fTempPerm63);
			fTempPerm66 = (fTempPerm65 + (2.0f - fRec64[0]));
			fTempPerm67 = (fRec64[0] - fTempPerm65);
			fRec63[0] = (fVec8[(IOTA-int((iTempPerm64 + 1)))&16383] + ((fRec63[1] * (0 - (fTempPerm66 / fTempPerm67))) + ((fTempPerm66 * fVec8[(IOTA-iTempPerm64)&16383]) / fTempPerm67)));
			fRec42[0] = fRec63[0];
			fTempPerm68 = ((0.760314f * fRec41[1]) + (0.64955574f * fTempPerm52));
			fTempPerm69 = ((0.760314f * fTempPerm68) - (0.64955574f * fRec38[1]));
			fTempPerm70 = ((0.760314f * fRec42[1]) + (0.64955574f * fTempPerm54));
			fTempPerm71 = ((0.760314f * fTempPerm70) - (0.64955574f * fRec39[1]));
			fTempPerm72 = ((0.70710677f * fTempPerm69) - (0.70710677f * fTempPerm71));
			fVec9[IOTA&16383] = fTempPerm72;
			fRec65[0] = (fSlow41 + (0.9999f * ((iSlow40 * iTempPerm0) + fRec65[1])));
			fTempPerm73 = (fRec65[0] + -1.49999f);
			iTempPerm74 = int(min(8192, max(0, int(fTempPerm73))));
			fTempPerm75 = floorf(fTempPerm73);
			fTempPerm76 = (fTempPerm75 + (2.0f - fRec65[0]));
			fTempPerm77 = (fRec65[0] - fTempPerm75);
			fRec40[0] = (fVec9[(IOTA-int((iTempPerm74 + 1)))&16383] + ((fRec40[1] * (0 - (fTempPerm76 / fTempPerm77))) + ((fTempPerm76 * fVec9[(IOTA-iTempPerm74)&16383]) / fTempPerm77)));
			fRec38[0] = fRec40[0];
			fTempPerm78 = ((0.70710677f * fTempPerm69) + (0.70710677f * fTempPerm71));
			fVec10[IOTA&16383] = fTempPerm78;
			fRec67[0] = (fSlow43 + (0.9999f * ((iSlow42 * iTempPerm0) + fRec67[1])));
			fTempPerm79 = (fRec67[0] + -1.49999f);
			iTempPerm80 = int(min(8192, max(0, int(fTempPerm79))));
			fTempPerm81 = floorf(fTempPerm79);
			fTempPerm82 = (fTempPerm81 + (2.0f - fRec67[0]));
			fTempPerm83 = (fRec67[0] - fTempPerm81);
			fRec66[0] = (fVec10[(IOTA-int((iTempPerm80 + 1)))&16383] + ((fRec66[1] * (0 - (fTempPerm82 / fTempPerm83))) + ((fTempPerm82 * fVec10[(IOTA-iTempPerm80)&16383]) / fTempPerm83)));
			fRec39[0] = fRec66[0];
			fTempPerm84 = ((0.760314f * fRec38[1]) + (0.64955574f * fTempPerm68));
			fVec11[IOTA&1023] = fTempPerm84;
			fTempPerm85 = (fSlow7 * (fRec16[0] + 1));
			fTempPerm86 = (fTempPerm85 + 3.500005f);
			iTempPerm87 = int(fTempPerm86);
			iTempPerm88 = int(min(512, max(0, iTempPerm87)));
			fTempPerm89 = floorf(fTempPerm86);
			fTempPerm90 = (fTempPerm85 + (4.0f - fTempPerm89));
			fTempPerm91 = (0 - fTempPerm90);
			fTempPerm92 = (fTempPerm85 + (3.0f - fTempPerm89));
			fTempPerm93 = (0 - (0.5f * fTempPerm92));
			fTempPerm94 = (fTempPerm85 + (2.0f - fTempPerm89));
			fTempPerm95 = (0 - (0.33333334f * fTempPerm94));
			fTempPerm96 = (fTempPerm85 + (1.0f - fTempPerm89));
			fTempPerm97 = (0 - (0.25f * fTempPerm96));
			fTempPerm98 = (fTempPerm85 + (5.0f - fTempPerm89));
			iTempPerm99 = int(min(512, max(0, (iTempPerm87 + 1))));
			fTempPerm100 = (0 - fTempPerm92);
			fTempPerm101 = (0 - (0.5f * fTempPerm94));
			fTempPerm102 = (0 - (0.33333334f * fTempPerm96));
			iTempPerm103 = int(min(512, max(0, (iTempPerm87 + 2))));
			fTempPerm104 = (0 - fTempPerm94);
			fTempPerm105 = (0 - (0.5f * fTempPerm96));
			fTempPerm106 = (fTempPerm90 * fTempPerm92);
			iTempPerm107 = int(min(512, max(0, (iTempPerm87 + 3))));
			fTempPerm108 = (0 - fTempPerm96);
			iTempPerm109 = int(min(512, max(0, (iTempPerm87 + 4))));
			fTempPerm110 = (((((fVec11[(IOTA-iTempPerm88)&1023] * fTempPerm91) * fTempPerm93) * fTempPerm95) * fTempPerm97) + (fTempPerm98 * ((((((fVec11[(IOTA-iTempPerm99)&1023] * fTempPerm100) * fTempPerm101) * fTempPerm102) + (0.5f * (((fTempPerm90 * fVec11[(IOTA-iTempPerm103)&1023]) * fTempPerm104) * fTempPerm105))) + (0.16666667f * ((fTempPerm106 * fVec11[(IOTA-iTempPerm107)&1023]) * fTempPerm108))) + (0.041666668f * ((fTempPerm106 * fTempPerm94) * fVec11[(IOTA-iTempPerm109)&1023])))));
			fVec12[IOTA&16383] = fTempPerm110;
			fRec68[0] = (fSlow45 + (0.995f * ((iSlow44 * iTempPerm0) + fRec68[1])));
			fTempPerm111 = (fRec68[0] + -1.49999f);
			iTempPerm112 = int(min(8192, max(0, int(fTempPerm111))));
			fTempPerm113 = floorf(fTempPerm111);
			fTempPerm114 = (fTempPerm113 + (2.0f - fRec68[0]));
			fTempPerm115 = (fRec68[0] - fTempPerm113);
			fRec37[0] = (fVec12[(IOTA-int((iTempPerm112 + 1)))&16383] + ((fRec37[1] * (0 - (fTempPerm114 / fTempPerm115))) + ((fTempPerm114 * fVec12[(IOTA-iTempPerm112)&16383]) / fTempPerm115)));
			fTempPerm116 = ((0.760314f * fRec37[0]) - (0.64955574f * fRec34[1]));
			fTempPerm117 = ((0.760314f * fRec39[1]) + (0.64955574f * fTempPerm70));
			fVec13[IOTA&1023] = fTempPerm117;
			fTempPerm118 = (fSlow46 * fRec16[0]);
			fTempPerm119 = (fSlow7 + (fTempPerm118 + 3.500005f));
			iTempPerm120 = int(fTempPerm119);
			fTempPerm121 = floorf(fTempPerm119);
			fTempPerm122 = (fSlow7 + (fTempPerm118 + (4.0f - fTempPerm121)));
			fTempPerm123 = (fSlow7 + (fTempPerm118 + (3.0f - fTempPerm121)));
			fTempPerm124 = (fSlow7 + (fTempPerm118 + (2.0f - fTempPerm121)));
			fTempPerm125 = (fSlow7 + (fTempPerm118 + (1.0f - fTempPerm121)));
			fTempPerm126 = (fTempPerm122 * fTempPerm123);
			fTempPerm127 = (((((fVec13[(IOTA-int(min(512, max(0, iTempPerm120))))&1023] * (0 - fTempPerm122)) * (0 - (0.5f * fTempPerm123))) * (0 - (0.33333334f * fTempPerm124))) * (0 - (0.25f * fTempPerm125))) + ((fSlow7 + (fTempPerm118 + (5.0f - fTempPerm121))) * ((((((fVec13[(IOTA-int(min(512, max(0, (iTempPerm120 + 1)))))&1023] * (0 - fTempPerm123)) * (0 - (0.5f * fTempPerm124))) * (0 - (0.33333334f * fTempPerm125))) + (0.5f * (((fTempPerm122 * fVec13[(IOTA-int(min(512, max(0, (iTempPerm120 + 2)))))&1023]) * (0 - fTempPerm124)) * (0 - (0.5f * fTempPerm125))))) + (0.16666667f * ((fTempPerm126 * fVec13[(IOTA-int(min(512, max(0, (iTempPerm120 + 3)))))&1023]) * (0 - fTempPerm125)))) + (0.041666668f * ((fTempPerm126 * fTempPerm124) * fVec13[(IOTA-int(min(512, max(0, (iTempPerm120 + 4)))))&1023])))));
			fVec14[IOTA&16383] = fTempPerm127;
			fRec70[0] = (fSlow48 + (0.995f * ((iSlow47 * iTempPerm0) + fRec70[1])));
			fTempPerm128 = (fRec70[0] + -1.49999f);
			iTempPerm129 = int(min(8192, max(0, int(fTempPerm128))));
			fTempPerm130 = floorf(fTempPerm128);
			fTempPerm131 = (fTempPerm130 + (2.0f - fRec70[0]));
			fTempPerm132 = (fRec70[0] - fTempPerm130);
			fRec69[0] = (fVec14[(IOTA-int((iTempPerm129 + 1)))&16383] + ((fRec69[1] * (0 - (fTempPerm131 / fTempPerm132))) + ((fTempPerm131 * fVec14[(IOTA-iTempPerm129)&16383]) / fTempPerm132)));
			fTempPerm133 = ((0.760314f * fRec69[0]) - (0.64955574f * fRec35[1]));
			fTempPerm134 = ((0.70710677f * fTempPerm116) - (0.70710677f * fTempPerm133));
			fVec15[IOTA&16383] = fTempPerm134;
			fRec71[0] = (fSlow50 + (0.9999f * ((iSlow49 * iTempPerm0) + fRec71[1])));
			fTempPerm135 = (fRec71[0] + -1.49999f);
			iTempPerm136 = int(min(8192, max(0, int(fTempPerm135))));
			fTempPerm137 = floorf(fTempPerm135);
			fTempPerm138 = (fTempPerm137 + (2.0f - fRec71[0]));
			fTempPerm139 = (fRec71[0] - fTempPerm137);
			fRec36[0] = (fVec15[(IOTA-int((iTempPerm136 + 1)))&16383] + ((fRec36[1] * (0 - (fTempPerm138 / fTempPerm139))) + ((fTempPerm138 * fVec15[(IOTA-iTempPerm136)&16383]) / fTempPerm139)));
			fRec34[0] = fRec36[0];
			fTempPerm140 = ((0.70710677f * fTempPerm116) + (0.70710677f * fTempPerm133));
			fVec16[IOTA&16383] = fTempPerm140;
			fRec73[0] = (fSlow52 + (0.9999f * ((iSlow51 * iTempPerm0) + fRec73[1])));
			fTempPerm141 = (fRec73[0] + -1.49999f);
			iTempPerm142 = int(min(8192, max(0, int(fTempPerm141))));
			fTempPerm143 = floorf(fTempPerm141);
			fTempPerm144 = (fTempPerm143 + (2.0f - fRec73[0]));
			fTempPerm145 = (fRec73[0] - fTempPerm143);
			fRec72[0] = (fVec16[(IOTA-int((iTempPerm142 + 1)))&16383] + ((fRec72[1] * (0 - (fTempPerm144 / fTempPerm145))) + ((fTempPerm144 * fVec16[(IOTA-iTempPerm142)&16383]) / fTempPerm145)));
			fRec35[0] = fRec72[0];
			fTempPerm146 = ((0.760314f * fRec34[1]) + (0.64955574f * fRec37[0]));
			fTempPerm147 = ((0.760314f * fTempPerm146) - (0.64955574f * fRec31[1]));
			fTempPerm148 = ((0.760314f * fRec35[1]) + (0.64955574f * fRec69[0]));
			fTempPerm149 = ((0.760314f * fTempPerm148) - (0.64955574f * fRec32[1]));
			fTempPerm150 = ((0.70710677f * fTempPerm147) - (0.70710677f * fTempPerm149));
			fVec17[IOTA&16383] = fTempPerm150;
			fRec74[0] = (fSlow54 + (0.9999f * ((iSlow53 * iTempPerm0) + fRec74[1])));
			fTempPerm151 = (fRec74[0] + -1.49999f);
			iTempPerm152 = int(min(8192, max(0, int(fTempPerm151))));
			fTempPerm153 = floorf(fTempPerm151);
			fTempPerm154 = (fTempPerm153 + (2.0f - fRec74[0]));
			fTempPerm155 = (fRec74[0] - fTempPerm153);
			fRec33[0] = (fVec17[(IOTA-int((iTempPerm152 + 1)))&16383] + ((fRec33[1] * (0 - (fTempPerm154 / fTempPerm155))) + ((fTempPerm154 * fVec17[(IOTA-iTempPerm152)&16383]) / fTempPerm155)));
			fRec31[0] = fRec33[0];
			fTempPerm156 = ((0.70710677f * fTempPerm147) + (0.70710677f * fTempPerm149));
			fVec18[IOTA&16383] = fTempPerm156;
			fRec76[0] = (fSlow56 + (0.9999f * ((iSlow55 * iTempPerm0) + fRec76[1])));
			fTempPerm157 = (fRec76[0] + -1.49999f);
			iTempPerm158 = int(min(8192, max(0, int(fTempPerm157))));
			iTempPerm159 = int((iTempPerm158 + 1));
			fTempPerm160 = floorf(fTempPerm157);
			fTempPerm161 = (fTempPerm160 + (2.0f - fRec76[0]));
			fTempPerm162 = (fRec76[0] - fTempPerm160);
			fTempPerm163 = (0 - (fTempPerm161 / fTempPerm162));
			fRec75[0] = (fVec18[(IOTA-iTempPerm159)&16383] + ((fTempPerm163 * fRec75[1]) + ((fTempPerm161 * fVec18[(IOTA-iTempPerm158)&16383]) / fTempPerm162)));
			fRec32[0] = fRec75[0];
			fTempPerm164 = ((0.760314f * fRec31[1]) + (0.64955574f * fTempPerm146));
			fTempPerm165 = ((0.760314f * fTempPerm164) - (0.64955574f * fRec28[1]));
			fTempPerm166 = ((0.760314f * fRec32[1]) + (0.64955574f * fTempPerm148));
			fTempPerm167 = ((0.760314f * fTempPerm166) - (0.64955574f * fRec29[1]));
			fTempPerm168 = ((0.70710677f * fTempPerm165) - (0.70710677f * fTempPerm167));
			fVec19[IOTA&16383] = fTempPerm168;
			fRec77[0] = (fSlow58 + (0.9999f * ((iSlow57 * iTempPerm0) + fRec77[1])));
			fTempPerm169 = (fRec77[0] + -1.49999f);
			iTempPerm170 = int(min(8192, max(0, int(fTempPerm169))));
			fTempPerm171 = floorf(fTempPerm169);
			fTempPerm172 = (fTempPerm171 + (2.0f - fRec77[0]));
			fTempPerm173 = (fRec77[0] - fTempPerm171);
			fRec30[0] = (fVec19[(IOTA-int((iTempPerm170 + 1)))&16383] + ((fRec30[1] * (0 - (fTempPerm172 / fTempPerm173))) + ((fTempPerm172 * fVec19[(IOTA-iTempPerm170)&16383]) / fTempPerm173)));
			fRec28[0] = fRec30[0];
			fTempPerm174 = ((0.70710677f * fTempPerm165) + (0.70710677f * fTempPerm167));
			fVec20[IOTA&16383] = fTempPerm174;
			fRec79[0] = (fSlow60 + (0.9999f * ((iSlow59 * iTempPerm0) + fRec79[1])));
			fTempPerm175 = (fRec79[0] + -1.49999f);
			iTempPerm176 = int(min(8192, max(0, int(fTempPerm175))));
			iTempPerm177 = int((iTempPerm176 + 1));
			fTempPerm178 = floorf(fTempPerm175);
			fTempPerm179 = (fTempPerm178 + (2.0f - fRec79[0]));
			fTempPerm180 = (fRec79[0] - fTempPerm178);
			fTempPerm181 = (0 - (fTempPerm179 / fTempPerm180));
			fRec78[0] = (fVec20[(IOTA-iTempPerm177)&16383] + ((fRec78[1] * fTempPerm181) + ((fTempPerm179 * fVec20[(IOTA-iTempPerm176)&16383]) / fTempPerm180)));
			fRec29[0] = fRec78[0];
			fTempPerm182 = ((0.760314f * fRec28[1]) + (0.64955574f * fTempPerm164));
			fTempPerm183 = ((0.760314f * fTempPerm182) - (0.64955574f * fRec25[1]));
			fTempPerm184 = ((0.760314f * fRec29[1]) + (0.64955574f * fTempPerm166));
			fTempPerm185 = ((0.760314f * fTempPerm184) - (0.64955574f * fRec26[1]));
			fTempPerm186 = ((0.70710677f * fTempPerm183) - (0.70710677f * fTempPerm185));
			fVec21[IOTA&16383] = fTempPerm186;
			fRec80[0] = (fSlow62 + (0.9999f * ((iSlow61 * iTempPerm0) + fRec80[1])));
			fTempPerm187 = (fRec80[0] + -1.49999f);
			iTempPerm188 = int(min(8192, max(0, int(fTempPerm187))));
			iTempPerm189 = int((iTempPerm188 + 1));
			fTempPerm190 = floorf(fTempPerm187);
			fTempPerm191 = (fTempPerm190 + (2.0f - fRec80[0]));
			fTempPerm192 = (fRec80[0] - fTempPerm190);
			fTempPerm193 = (0 - (fTempPerm191 / fTempPerm192));
			fRec27[0] = (fVec21[(IOTA-iTempPerm189)&16383] + ((fTempPerm193 * fRec27[1]) + ((fTempPerm191 * fVec21[(IOTA-iTempPerm188)&16383]) / fTempPerm192)));
			fRec25[0] = fRec27[0];
			fTempPerm194 = ((0.70710677f * fTempPerm183) + (0.70710677f * fTempPerm185));
			fVec22[IOTA&16383] = fTempPerm194;
			fRec82[0] = (fSlow64 + (0.9999f * ((iSlow63 * iTempPerm0) + fRec82[1])));
			fTempPerm195 = (fRec82[0] + -1.49999f);
			iTempPerm196 = int(min(8192, max(0, int(fTempPerm195))));
			iTempPerm197 = int((iTempPerm196 + 1));
			fTempPerm198 = floorf(fTempPerm195);
			fTempPerm199 = (fTempPerm198 + (2.0f - fRec82[0]));
			fTempPerm200 = (fRec82[0] - fTempPerm198);
			fTempPerm201 = (0 - (fTempPerm199 / fTempPerm200));
			fRec81[0] = (fVec22[(IOTA-iTempPerm197)&16383] + ((fTempPerm201 * fRec81[1]) + ((fTempPerm199 * fVec22[(IOTA-iTempPerm196)&16383]) / fTempPerm200)));
			fRec26[0] = fRec81[0];
			fTempPerm202 = ((0.760314f * fRec25[1]) + (0.64955574f * fTempPerm182));
			fTempPerm203 = ((0.760314f * fTempPerm202) - (0.64955574f * fRec22[1]));
			fTempPerm204 = ((0.760314f * fRec26[1]) + (0.64955574f * fTempPerm184));
			fTempPerm205 = ((0.760314f * fTempPerm204) - (0.64955574f * fRec23[1]));
			fTempPerm206 = ((0.70710677f * fTempPerm203) - (0.70710677f * fTempPerm205));
			fVec23[IOTA&16383] = fTempPerm206;
			fRec83[0] = (fSlow66 + (0.9999f * ((iSlow65 * iTempPerm0) + fRec83[1])));
			fTempPerm207 = (fRec83[0] + -1.49999f);
			iTempPerm208 = int(min(8192, max(0, int(fTempPerm207))));
			fTempPerm209 = floorf(fTempPerm207);
			fTempPerm210 = (fTempPerm209 + (2.0f - fRec83[0]));
			fTempPerm211 = (fRec83[0] - fTempPerm209);
			fRec24[0] = (fVec23[(IOTA-int((iTempPerm208 + 1)))&16383] + ((fRec24[1] * (0 - (fTempPerm210 / fTempPerm211))) + ((fTempPerm210 * fVec23[(IOTA-iTempPerm208)&16383]) / fTempPerm211)));
			fRec22[0] = fRec24[0];
			fTempPerm212 = ((0.70710677f * fTempPerm203) + (0.70710677f * fTempPerm205));
			fVec24[IOTA&16383] = fTempPerm212;
			fRec85[0] = (fSlow68 + (0.9999f * ((iSlow67 * iTempPerm0) + fRec85[1])));
			fTempPerm213 = (fRec85[0] + -1.49999f);
			iTempPerm214 = int(min(8192, max(0, int(fTempPerm213))));
			iTempPerm215 = int((iTempPerm214 + 1));
			fTempPerm216 = floorf(fTempPerm213);
			fTempPerm217 = (fTempPerm216 + (2.0f - fRec85[0]));
			fTempPerm218 = (fRec85[0] - fTempPerm216);
			fTempPerm219 = (0 - (fTempPerm217 / fTempPerm218));
			fRec84[0] = (fVec24[(IOTA-iTempPerm215)&16383] + ((fRec84[1] * fTempPerm219) + ((fTempPerm217 * fVec24[(IOTA-iTempPerm214)&16383]) / fTempPerm218)));
			fRec23[0] = fRec84[0];
			fTempPerm220 = ((0.760314f * fRec22[1]) + (0.64955574f * fTempPerm202));
			fVec25[IOTA&16383] = fTempPerm220;
			iTempPerm221 = int(fTempPerm2);
			iTempPerm222 = max(0, iTempPerm221);
			fTempPerm223 = (fTempPerm1 + (4.0f - fTempPerm3));
			fTempPerm224 = (0 - fTempPerm223);
			fTempPerm225 = (fTempPerm1 + (3.0f - fTempPerm3));
			fTempPerm226 = (0 - (0.5f * fTempPerm225));
			fTempPerm227 = (fTempPerm1 + (2.0f - fTempPerm3));
			fTempPerm228 = (0 - (0.33333334f * fTempPerm227));
			iTempPerm229 = max(0, (iTempPerm221 + 1));
			fTempPerm230 = (0 - fTempPerm225);
			fTempPerm231 = (0 - (0.5f * fTempPerm227));
			fTempPerm232 = (0 - (0.33333334f * fTempPerm4));
			iTempPerm233 = max(0, (iTempPerm221 + 2));
			fTempPerm234 = (0 - fTempPerm227);
			fTempPerm235 = (0 - (0.5f * fTempPerm4));
			iTempPerm236 = max(0, (iTempPerm221 + 3));
			fTempPerm237 = (0 - fTempPerm4);
			iTempPerm238 = max(0, (iTempPerm221 + 4));
			fTempPerm239 = (fTempPerm1 + (5.0f - fTempPerm3));
			fTempPerm240 = (((((fTempPerm5 * fVec25[(IOTA-int(min(8192, iTempPerm222)))&16383]) * fTempPerm224) * fTempPerm226) * fTempPerm228) + (((((((fVec25[(IOTA-int(min(8192, iTempPerm229)))&16383] * fTempPerm230) * fTempPerm231) * fTempPerm232) + (0.5f * (((fVec25[(IOTA-int(min(8192, iTempPerm233)))&16383] * fTempPerm234) * fTempPerm235) * fTempPerm223))) + (0.16666667f * (((fVec25[(IOTA-int(min(8192, iTempPerm236)))&16383] * fTempPerm237) * fTempPerm223) * fTempPerm225))) + (0.041666668f * (((fVec25[(IOTA-int(min(8192, iTempPerm238)))&16383] * fTempPerm223) * fTempPerm225) * fTempPerm227))) * fTempPerm239));
			fVec26[IOTA&16383] = fTempPerm240;
			fRec86[0] = (fSlow70 + (0.995f * ((iSlow69 * iTempPerm0) + fRec86[1])));
			fTempPerm241 = (fRec86[0] + -1.49999f);
			iTempPerm242 = int(min(8192, max(0, int(fTempPerm241))));
			fTempPerm243 = floorf(fTempPerm241);
			fTempPerm244 = (fTempPerm243 + (2.0f - fRec86[0]));
			fTempPerm245 = (fRec86[0] - fTempPerm243);
			fRec21[0] = (fVec26[(IOTA-int((iTempPerm242 + 1)))&16383] + ((fRec21[1] * (0 - (fTempPerm244 / fTempPerm245))) + ((fTempPerm244 * fVec26[(IOTA-iTempPerm242)&16383]) / fTempPerm245)));
			fRec20[0] = ((fSlow22 * fRec20[1]) + (fSlow23 * ((fSlow16 * fRec21[0]) + (fSlow71 * fRec21[1]))));
			fRec19[0] = (fRec20[0] - (fSlow20 * ((fSlow72 * fRec19[1]) + (fSlow73 * fRec19[2]))));
			fRec18[0] = ((fSlow20 * (((fSlow19 * fRec19[0]) + (fSlow74 * fRec19[1])) + (fSlow19 * fRec19[2]))) - (fSlow18 * ((fSlow72 * fRec18[1]) + (fSlow75 * fRec18[2]))));
			fTempPerm246 = (fSlow81 * fRec17[1]);
			fRec17[0] = ((fSlow18 * (((fSlow19 * fRec18[0]) + (fSlow74 * fRec18[1])) + (fSlow19 * fRec18[2]))) - (fSlow78 * ((fSlow79 * fRec17[2]) + fTempPerm246)));
			fRec92[0] = ((fSlow22 * fRec92[1]) + (fSlow23 * (fRec21[0] + fRec21[1])));
			fRec91[0] = (fRec92[0] - (fSlow20 * ((fSlow72 * fRec91[1]) + (fSlow73 * fRec91[2]))));
			fRec90[0] = ((fSlow20 * (fRec91[2] + (fRec91[0] + (2.0f * fRec91[1])))) - (fSlow18 * ((fSlow72 * fRec90[1]) + (fSlow75 * fRec90[2]))));
			fTempPerm247 = (fRec90[2] + (fRec90[0] + (2.0f * fRec90[1])));
			fTempPerm248 = (fSlow18 * fTempPerm247);
			fVec27[0] = fTempPerm248;
			fRec89[0] = ((fSlow87 * fRec89[1]) + (fSlow88 * ((fSlow89 * fVec27[1]) + (fSlow90 * fTempPerm247))));
			fRec88[0] = (fRec89[0] - (fSlow85 * ((fSlow81 * fRec88[1]) + (fSlow91 * fRec88[2]))));
			fRec87[0] = ((fSlow85 * (((fSlow80 * fRec88[0]) + (fSlow84 * fRec88[1])) + (fSlow80 * fRec88[2]))) - (fSlow82 * ((fSlow81 * fRec87[1]) + (fSlow92 * fRec87[2]))));
			fRec95[0] = ((fSlow87 * fRec95[1]) + (fSlow88 * (fVec27[1] + fVec27[0])));
			fRec94[0] = (fRec95[0] - (fSlow85 * ((fSlow81 * fRec94[1]) + (fSlow91 * fRec94[2]))));
			fRec93[0] = ((fSlow85 * (fRec94[2] + (fRec94[0] + (2.0f * fRec94[1])))) - (fSlow82 * ((fSlow81 * fRec93[1]) + (fSlow92 * fRec93[2]))));
			fTempPerm249 = ((float)input0[i] + (fSlow13 * ((fSlow14 * (fRec17[2] + (fSlow78 * (fTempPerm246 + (fSlow79 * fRec17[0]))))) + (fSlow82 * ((fSlow83 * (((fSlow84 * fRec87[1]) + (fSlow80 * fRec87[0])) + (fSlow80 * fRec87[2]))) + (fSlow93 * (fRec93[2] + (fRec93[0] + (2.0f * fRec93[1])))))))));
			fVec28[IOTA&1023] = fTempPerm249;
			fRec14[0] = ((fSlow4 * fRec14[1]) + (fSlow5 * (((((fTempPerm5 * fVec28[(IOTA-int(min(512, iTempPerm222)))&1023]) * fTempPerm224) * fTempPerm226) * fTempPerm228) + (((((((fTempPerm230 * fTempPerm231) * fTempPerm232) * fVec28[(IOTA-int(min(512, iTempPerm229)))&1023]) + (0.5f * (((fTempPerm234 * fTempPerm235) * fVec28[(IOTA-int(min(512, iTempPerm233)))&1023]) * fTempPerm223))) + (0.16666667f * (((fTempPerm237 * fVec28[(IOTA-int(min(512, iTempPerm236)))&1023]) * fTempPerm223) * fTempPerm225))) + (0.041666668f * (((fVec28[(IOTA-int(min(512, iTempPerm238)))&1023] * fTempPerm223) * fTempPerm225) * fTempPerm227))) * fTempPerm239))));
			fTempPerm250 = ((fSlow3 * fRec11[1]) + (fSlow1 * fRec14[0]));
			fTempPerm251 = ((0.760314f * fRec23[1]) + (0.64955574f * fTempPerm204));
			fVec29[IOTA&16383] = fTempPerm251;
			fTempPerm252 = (fSlow46 * fRec15[0]);
			fTempPerm253 = (fSlow7 + (fTempPerm252 + 3.500005f));
			iTempPerm254 = int(fTempPerm253);
			fTempPerm255 = floorf(fTempPerm253);
			fTempPerm256 = (fSlow7 + (fTempPerm252 + (4.0f - fTempPerm255)));
			fTempPerm257 = (fSlow7 + (fTempPerm252 + (3.0f - fTempPerm255)));
			fTempPerm258 = (fSlow7 + (fTempPerm252 + (2.0f - fTempPerm255)));
			fTempPerm259 = (fSlow7 + (fTempPerm252 + (1.0f - fTempPerm255)));
			fTempPerm260 = (fTempPerm256 * fTempPerm257);
			fTempPerm261 = (((((fVec29[(IOTA-int(min(8192, max(0, iTempPerm254))))&16383] * (0 - fTempPerm256)) * (0 - (0.5f * fTempPerm257))) * (0 - (0.33333334f * fTempPerm258))) * (0 - (0.25f * fTempPerm259))) + ((fSlow7 + (fTempPerm252 + (5.0f - fTempPerm255))) * ((((((fVec29[(IOTA-int(min(8192, max(0, (iTempPerm254 + 1)))))&16383] * (0 - fTempPerm257)) * (0 - (0.5f * fTempPerm258))) * (0 - (0.33333334f * fTempPerm259))) + (0.5f * (((fTempPerm256 * fVec29[(IOTA-int(min(8192, max(0, (iTempPerm254 + 2)))))&16383]) * (0 - fTempPerm258)) * (0 - (0.5f * fTempPerm259))))) + (0.16666667f * ((fTempPerm260 * fVec29[(IOTA-int(min(8192, max(0, (iTempPerm254 + 3)))))&16383]) * (0 - fTempPerm259)))) + (0.041666668f * ((fTempPerm260 * fTempPerm258) * fVec29[(IOTA-int(min(8192, max(0, (iTempPerm254 + 4)))))&16383])))));
			fVec30[IOTA&16383] = fTempPerm261;
			fRec102[0] = (fSlow95 + (0.995f * ((iSlow94 * iTempPerm0) + fRec102[1])));
			fTempPerm262 = (fRec102[0] + -1.49999f);
			iTempPerm263 = int(min(8192, max(0, int(fTempPerm262))));
			fTempPerm264 = floorf(fTempPerm262);
			fTempPerm265 = (fTempPerm264 + (2.0f - fRec102[0]));
			fTempPerm266 = (fRec102[0] - fTempPerm264);
			fRec101[0] = (fVec30[(IOTA-int((iTempPerm263 + 1)))&16383] + ((fRec101[1] * (0 - (fTempPerm265 / fTempPerm266))) + ((fTempPerm265 * fVec30[(IOTA-iTempPerm263)&16383]) / fTempPerm266)));
			fRec100[0] = ((fSlow22 * fRec100[1]) + (fSlow23 * ((fSlow16 * fRec101[0]) + (fSlow71 * fRec101[1]))));
			fRec99[0] = (fRec100[0] - (fSlow20 * ((fSlow72 * fRec99[1]) + (fSlow73 * fRec99[2]))));
			fRec98[0] = ((fSlow20 * (((fSlow19 * fRec99[0]) + (fSlow74 * fRec99[1])) + (fSlow19 * fRec99[2]))) - (fSlow18 * ((fSlow72 * fRec98[1]) + (fSlow75 * fRec98[2]))));
			fTempPerm267 = (fSlow81 * fRec97[1]);
			fRec97[0] = ((fSlow18 * (((fSlow19 * fRec98[0]) + (fSlow74 * fRec98[1])) + (fSlow19 * fRec98[2]))) - (fSlow78 * (fTempPerm267 + (fSlow79 * fRec97[2]))));
			fRec108[0] = ((fSlow22 * fRec108[1]) + (fSlow23 * (fRec101[0] + fRec101[1])));
			fRec107[0] = (fRec108[0] - (fSlow20 * ((fSlow72 * fRec107[1]) + (fSlow73 * fRec107[2]))));
			fRec106[0] = ((fSlow20 * (fRec107[2] + (fRec107[0] + (2.0f * fRec107[1])))) - (fSlow18 * ((fSlow72 * fRec106[1]) + (fSlow75 * fRec106[2]))));
			fTempPerm268 = (fRec106[2] + (fRec106[0] + (2.0f * fRec106[1])));
			fTempPerm269 = (fSlow18 * fTempPerm268);
			fVec31[0] = fTempPerm269;
			fRec105[0] = ((fSlow87 * fRec105[1]) + (fSlow88 * ((fSlow90 * fTempPerm268) + (fSlow89 * fVec31[1]))));
			fRec104[0] = (fRec105[0] - (fSlow85 * ((fSlow81 * fRec104[1]) + (fSlow91 * fRec104[2]))));
			fRec103[0] = ((fSlow85 * (((fSlow80 * fRec104[0]) + (fSlow84 * fRec104[1])) + (fSlow80 * fRec104[2]))) - (fSlow82 * ((fSlow81 * fRec103[1]) + (fSlow92 * fRec103[2]))));
			fRec111[0] = ((fSlow87 * fRec111[1]) + (fSlow88 * (fVec31[1] + fVec31[0])));
			fRec110[0] = (fRec111[0] - (fSlow85 * ((fSlow81 * fRec110[1]) + (fSlow91 * fRec110[2]))));
			fRec109[0] = ((fSlow85 * (fRec110[2] + (fRec110[0] + (2.0f * fRec110[1])))) - (fSlow82 * ((fSlow81 * fRec109[1]) + (fSlow92 * fRec109[2]))));
			fTempPerm270 = ((float)input1[i] + (fSlow13 * ((fSlow14 * (fRec97[2] + (fSlow78 * (fTempPerm267 + (fSlow79 * fRec97[0]))))) + (fSlow82 * ((fSlow83 * (((fSlow84 * fRec103[1]) + (fSlow80 * fRec103[0])) + (fSlow80 * fRec103[2]))) + (fSlow93 * (fRec109[2] + (fRec109[0] + (2.0f * fRec109[1])))))))));
			fVec32[IOTA&1023] = fTempPerm270;
			fRec96[0] = ((fSlow5 * (((((fVec32[(IOTA-iTempPerm88)&1023] * fTempPerm91) * fTempPerm93) * fTempPerm95) * fTempPerm97) + (((((((fVec32[(IOTA-iTempPerm99)&1023] * fTempPerm100) * fTempPerm101) * fTempPerm102) + (0.5f * (((fVec32[(IOTA-iTempPerm103)&1023] * fTempPerm90) * fTempPerm104) * fTempPerm105))) + (0.16666667f * (((fVec32[(IOTA-iTempPerm107)&1023] * fTempPerm90) * fTempPerm92) * fTempPerm108))) + (0.041666668f * (((fVec32[(IOTA-iTempPerm109)&1023] * fTempPerm90) * fTempPerm92) * fTempPerm94))) * fTempPerm98))) + (fSlow4 * fRec96[1]));
			fTempPerm271 = ((fSlow1 * fRec96[0]) + (fSlow3 * fRec12[1]));
			fTempPerm272 = ((0.70710677f * fTempPerm250) - (0.70710677f * fTempPerm271));
			fVec33[IOTA&16383] = fTempPerm272;
			fRec13[0] = ((((fVec33[(IOTA-iTempPerm158)&16383] * fTempPerm161) / fTempPerm162) + fVec33[(IOTA-iTempPerm159)&16383]) + (fRec13[1] * fTempPerm163));
			fRec11[0] = fRec13[0];
			fTempPerm273 = ((0.70710677f * fTempPerm250) + (0.70710677f * fTempPerm271));
			fVec34[IOTA&16383] = fTempPerm273;
			fRec113[0] = (fSlow97 + (0.9999f * ((iSlow96 * iTempPerm0) + fRec113[1])));
			fTempPerm274 = (fRec113[0] + -1.49999f);
			iTempPerm275 = int(min(8192, max(0, int(fTempPerm274))));
			fTempPerm276 = floorf(fTempPerm274);
			fTempPerm277 = (fTempPerm276 + (2.0f - fRec113[0]));
			fTempPerm278 = (fRec113[0] - fTempPerm276);
			fRec112[0] = (fVec34[(IOTA-int((iTempPerm275 + 1)))&16383] + ((fRec112[1] * (0 - (fTempPerm277 / fTempPerm278))) + ((fTempPerm277 * fVec34[(IOTA-iTempPerm275)&16383]) / fTempPerm278)));
			fRec12[0] = fRec112[0];
			fTempPerm279 = ((fSlow1 * fRec11[1]) + (fSlow2 * fRec14[0]));
			fTempPerm280 = ((fSlow1 * fTempPerm279) + (fSlow3 * fRec8[1]));
			fTempPerm281 = ((fSlow1 * fRec12[1]) + (fSlow2 * fRec96[0]));
			fTempPerm282 = ((fSlow3 * fRec9[1]) + (fSlow1 * fTempPerm281));
			fTempPerm283 = ((0.70710677f * fTempPerm280) - (0.70710677f * fTempPerm282));
			fVec35[IOTA&16383] = fTempPerm283;
			fRec10[0] = ((((fVec35[(IOTA-iTempPerm188)&16383] * fTempPerm191) / fTempPerm192) + fVec35[(IOTA-iTempPerm189)&16383]) + (fRec10[1] * fTempPerm193));
			fRec8[0] = fRec10[0];
			fTempPerm284 = ((0.70710677f * fTempPerm280) + (0.70710677f * fTempPerm282));
			fVec36[IOTA&16383] = fTempPerm284;
			fRec114[0] = (fVec36[(IOTA-iTempPerm177)&16383] + ((fRec114[1] * fTempPerm181) + ((fVec36[(IOTA-iTempPerm176)&16383] * fTempPerm179) / fTempPerm180)));
			fRec9[0] = fRec114[0];
			fTempPerm285 = ((fSlow1 * fRec8[1]) + (fSlow2 * fTempPerm279));
			fTempPerm286 = ((fSlow1 * fTempPerm285) + (fSlow3 * fRec5[1]));
			fTempPerm287 = ((fSlow1 * fRec9[1]) + (fSlow2 * fTempPerm281));
			fTempPerm288 = ((fSlow3 * fRec6[1]) + (fSlow1 * fTempPerm287));
			fTempPerm289 = ((0.70710677f * fTempPerm286) - (0.70710677f * fTempPerm288));
			fVec37[IOTA&16383] = fTempPerm289;
			fRec7[0] = ((((fVec37[(IOTA-iTempPerm196)&16383] * fTempPerm199) / fTempPerm200) + fVec37[(IOTA-iTempPerm197)&16383]) + (fRec7[1] * fTempPerm201));
			fRec5[0] = fRec7[0];
			fTempPerm290 = ((0.70710677f * fTempPerm286) + (0.70710677f * fTempPerm288));
			fVec38[IOTA&16383] = fTempPerm290;
			fRec116[0] = (fSlow99 + (0.9999f * ((iSlow98 * iTempPerm0) + fRec116[1])));
			fTempPerm291 = (fRec116[0] + -1.49999f);
			iTempPerm292 = int(min(8192, max(0, int(fTempPerm291))));
			fTempPerm293 = floorf(fTempPerm291);
			fTempPerm294 = (fTempPerm293 + (2.0f - fRec116[0]));
			fTempPerm295 = (fRec116[0] - fTempPerm293);
			fRec115[0] = (fVec38[(IOTA-int((iTempPerm292 + 1)))&16383] + ((fRec115[1] * (0 - (fTempPerm294 / fTempPerm295))) + ((fTempPerm294 * fVec38[(IOTA-iTempPerm292)&16383]) / fTempPerm295)));
			fRec6[0] = fRec115[0];
			fTempPerm296 = ((fSlow1 * fRec5[1]) + (fSlow2 * fTempPerm285));
			fTempPerm297 = ((fSlow1 * fTempPerm296) + (fSlow3 * fRec2[1]));
			fTempPerm298 = ((fSlow1 * fRec6[1]) + (fSlow2 * fTempPerm287));
			fTempPerm299 = ((fSlow3 * fRec3[1]) + (fSlow1 * fTempPerm298));
			fTempPerm300 = ((0.70710677f * fTempPerm297) - (0.70710677f * fTempPerm299));
			fVec39[IOTA&16383] = fTempPerm300;
			fRec117[0] = (fSlow101 + (0.9999f * ((iSlow100 * iTempPerm0) + fRec117[1])));
			fTempPerm301 = (fRec117[0] + -1.49999f);
			iTempPerm302 = int(min(8192, max(0, int(fTempPerm301))));
			fTempPerm303 = floorf(fTempPerm301);
			fTempPerm304 = (fTempPerm303 + (2.0f - fRec117[0]));
			fTempPerm305 = (fRec117[0] - fTempPerm303);
			fRec4[0] = ((((fVec39[(IOTA-iTempPerm302)&16383] * fTempPerm304) / fTempPerm305) + fVec39[(IOTA-int((iTempPerm302 + 1)))&16383]) + (fRec4[1] * (0 - (fTempPerm304 / fTempPerm305))));
			fRec2[0] = fRec4[0];
			fTempPerm306 = ((0.70710677f * fTempPerm297) + (0.70710677f * fTempPerm299));
			fVec40[IOTA&16383] = fTempPerm306;
			fRec118[0] = (fVec40[(IOTA-iTempPerm215)&16383] + ((fRec118[1] * fTempPerm219) + ((fVec40[(IOTA-iTempPerm214)&16383] * fTempPerm217) / fTempPerm218)));
			fRec3[0] = fRec118[0];
			fRec0[0] = ((fSlow1 * fRec2[1]) + (fSlow2 * fTempPerm296));
			fRec1[0] = ((fSlow1 * fRec3[1]) + (fSlow2 * fTempPerm298));
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
			fRec17[2] = fRec17[1]; fRec17[1] = fRec17[0];
			fRec18[2] = fRec18[1]; fRec18[1] = fRec18[0];
			fRec19[2] = fRec19[1]; fRec19[1] = fRec19[0];
			fRec20[1] = fRec20[0];
			fRec21[1] = fRec21[0];
			fRec86[1] = fRec86[0];
			fRec23[1] = fRec23[0];
			fRec84[1] = fRec84[0];
			fRec85[1] = fRec85[0];
			fRec22[1] = fRec22[0];
			fRec24[1] = fRec24[0];
			fRec83[1] = fRec83[0];
			fRec26[1] = fRec26[0];
			fRec81[1] = fRec81[0];
			fRec82[1] = fRec82[0];
			fRec25[1] = fRec25[0];
			fRec27[1] = fRec27[0];
			fRec80[1] = fRec80[0];
			fRec29[1] = fRec29[0];
			fRec78[1] = fRec78[0];
			fRec79[1] = fRec79[0];
			fRec28[1] = fRec28[0];
			fRec30[1] = fRec30[0];
			fRec77[1] = fRec77[0];
			fRec32[1] = fRec32[0];
			fRec75[1] = fRec75[0];
			fRec76[1] = fRec76[0];
			fRec31[1] = fRec31[0];
			fRec33[1] = fRec33[0];
			fRec74[1] = fRec74[0];
			fRec35[1] = fRec35[0];
			fRec72[1] = fRec72[0];
			fRec73[1] = fRec73[0];
			fRec34[1] = fRec34[0];
			fRec36[1] = fRec36[0];
			fRec71[1] = fRec71[0];
			fRec69[1] = fRec69[0];
			fRec70[1] = fRec70[0];
			fRec37[1] = fRec37[0];
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
			fRec16[1] = fRec16[0];
			fRec15[1] = fRec15[0];
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
