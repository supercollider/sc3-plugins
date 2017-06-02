//----------------------------------------------------------
// name: "HOAEncoder4"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2014"
//
// Code generated with Faust 0.9.73-mr2 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with  */
#include <math.h>
#ifndef FAUSTPOWER
#define FAUSTPOWER
#include <cmath>
template <int N> inline float faustpower(float x)          { return powf(x,N); } 
template <int N> inline double faustpower(double x)        { return pow(x,N); }
template <int N> inline int faustpower(int x)              { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline int faustpower<0>(int x)            { return 1; }
template <> 	 inline int faustpower<1>(int x)            { return x; }
template <> 	 inline int faustpower<2>(int x)            { return x*x; }
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

/************************************************************************
    IMPORTANT NOTE : this file contains two clearly delimited sections :
    the ARCHITECTURE section (in two parts) and the USER section. Each section
    is governed by its own copyright and license. Please check individually
    each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
    Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; If not, see <http://www.gnu.org/licenses/>.

    EXCEPTION : As a special exception, you may create a larger work
    that contains this FAUST architecture section and distribute
    that work under terms of your choice, so long as this FAUST
    architecture section is not modified.

 ************************************************************************
 ************************************************************************/
 
/******************************************************************************
*******************************************************************************

								FAUST DSP

*******************************************************************************
*******************************************************************************/

#ifndef __dsp__
#define __dsp__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

class UI;

//----------------------------------------------------------------
//  Signal processor definition
//----------------------------------------------------------------

class dsp {

    protected:

        int fSamplingFreq;

    public:

        dsp() {}
        virtual ~dsp() {}

        virtual int getNumInputs() = 0;
        virtual int getNumOutputs() = 0;
        virtual void buildUserInterface(UI* ui_interface) = 0;
        virtual void init(int samplingRate) = 0;
        virtual void instanceInit(int samplingRate) = 0;
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

//----------------------------------------------------------------
//  Generic decorator
//----------------------------------------------------------------

class decorator_dsp : public dsp {

    protected:

        dsp* fDSP;

    public:

        decorator_dsp(dsp* dsp = 0):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) { fDSP->buildUserInterface(ui_interface); }
        virtual void init(int samplingRate) { fDSP->init(samplingRate); }
        virtual void instanceInit(int samplingRate) { fDSP->instanceInit(samplingRate); }
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
       
};

//----------------------------------------------------------------
// On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
// flags to avoid costly denormals
//----------------------------------------------------------------

#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif
/************************************************************************
    FAUST Architecture File
    Copyright (C) 2003-2016 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; If not, see <http://www.gnu.org/licenses/>.

    EXCEPTION : As a special exception, you may create a larger work
    that contains this FAUST architecture section and distribute
    that work under terms of your choice, so long as this FAUST
    architecture section is not modified.


 ************************************************************************
 ************************************************************************/
 
#ifndef FAUST_UI_H
#define FAUST_UI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust User Interface
 * This abstract class contains only the method that the faust compiler can
 * generate to describe a DSP interface.
 ******************************************************************************/

class UI
{

    public:

        UI() {}

        virtual ~UI() {}

        // -- widget's layouts

        virtual void openTabBox(const char* label) = 0;
        virtual void openHorizontalBox(const char* label) = 0;
        virtual void openVerticalBox(const char* label) = 0;
        virtual void closeBox() = 0;

        // -- active widgets

        virtual void addButton(const char* label, FAUSTFLOAT* zone) = 0;
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) = 0;
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;

        // -- passive widgets

        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;

        // -- metadata declarations

        virtual void declare(FAUSTFLOAT*, const char*, const char*) {}
};

//----------------------------------------------------------------
//  Generic decorator
//----------------------------------------------------------------

class DecoratorUI : public UI
{
    protected:
    
        UI* fUI;

    public:
    
        DecoratorUI(UI* ui = 0):fUI(ui)
        {}

        virtual ~DecoratorUI() { delete fUI; }

        // -- widget's layouts
        virtual void openTabBox(const char* label)          { fUI->openTabBox(label); }
        virtual void openHorizontalBox(const char* label)   { fUI->openHorizontalBox(label); }
        virtual void openVerticalBox(const char* label)     { fUI->openVerticalBox(label); }
        virtual void closeBox()                             { fUI->closeBox(); }

        // -- active widgets
        virtual void addButton(const char* label, FAUSTFLOAT* zone)         { fUI->addButton(label, zone); }
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone)    { fUI->addCheckButton(label, zone); }
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
            { fUI->addVerticalSlider(label, zone, init, min, max, step); }
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) 	
            { fUI->addHorizontalSlider(label, zone, init, min, max, step); }
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) 			
            { fUI->addNumEntry(label, zone, init, min, max, step); }

        // -- passive widgets	
        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) 
            { fUI->addHorizontalBargraph(label, zone, min, max); }
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
            { fUI->addVerticalBargraph(label, zone, min, max); }

        virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val) { fUI->declare(zone, key, val); }

};

#endif
/************************************************************************
 ************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

 ************************************************************************
 ************************************************************************/
 
#ifndef __misc__
#define __misc__

#include <algorithm>
#include <map>
#include <string.h>
#include <stdlib.h>

/************************************************************************
 ************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

 ************************************************************************
 ************************************************************************/
 
#ifndef __meta__
#define __meta__

struct Meta
{
    virtual void declare(const char* key, const char* value) = 0;
    virtual ~Meta() {};
};

#endif

using std::max;
using std::min;

struct XXXX_Meta : std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key]=value; }
};

struct MY_Meta : Meta, std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key]=value; }
};

inline int lsr(int x, int n)	{ return int(((unsigned int)x) >> n); }
inline int int2pow2(int x)		{ int r=0; while ((1<<r)<x) r++; return r; }

long lopt(char* argv[], const char* name, long def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return atoi(argv[i+1]);
	return def;
}

bool isopt(char* argv[], const char* name)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return true;
	return false;
}

const char* lopts(char* argv[], const char* name, const char* def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return argv[i+1];
	return def;
}

#endif


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


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
  private:
	int 	iConst0;
	double 	fConst1;
	FAUSTFLOAT 	fcheckbox0;
	FAUSTFLOAT 	fentry0;
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fslider1;
	double 	fRec1[2];
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph0;
	FAUSTFLOAT 	fslider2;
	FAUSTFLOAT 	fslider3;
	double 	fConst2;
	double 	fConst3;
	double 	fRec5[2];
	double 	fRec4[2];
	double 	fRec3[2];
	double 	fRec2[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec6[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec7[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fConst4;
	double 	fConst5;
	double 	fRec11[2];
	double 	fRec12[2];
	double 	fRec10[2];
	double 	fRec9[2];
	double 	fVec0[2];
	double 	fRec13[2];
	double 	fRec8[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec14[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec15[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec16[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec17[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fConst6;
	double 	fConst7;
	double 	fConst8;
	double 	fConst9;
	double 	fRec22[2];
	double 	fRec23[2];
	double 	fRec21[2];
	double 	fRec24[2];
	double 	fVec1[2];
	double 	fRec25[2];
	double 	fRec26[2];
	double 	fRec20[2];
	double 	fRec19[2];
	double 	fRec18[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec27[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec28[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec29[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec30[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec31[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec32[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fConst10;
	double 	fConst11;
	double 	fConst12;
	double 	fConst13;
	double 	fRec37[2];
	double 	fRec38[2];
	double 	fRec36[2];
	double 	fRec39[2];
	double 	fVec2[2];
	double 	fRec40[2];
	double 	fRec41[2];
	double 	fRec42[2];
	double 	fRec35[2];
	double 	fRec34[2];
	double 	fVec3[2];
	double 	fRec43[2];
	double 	fRec33[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec44[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec45[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec47[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec48[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec49[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec50[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec51[2];
	FAUSTFLOAT 	fbargraph24;
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("name", "HOAEncoder4");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/nfc.lib/name", "NF And NFC Filters Library");
		m->declare("lib/nfc.lib/version", "1.0");
		m->declare("lib/nfc.lib/author", "Pierre Lecomte");
		m->declare("lib/nfc.lib/license", "GPL");
		m->declare("lib/nfc.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("lib/ymn.lib/name", "Spherical Harmonics library");
		m->declare("lib/ymn.lib/version", "1.0");
		m->declare("lib/ymn.lib/author", "Pierre Lecomte");
		m->declare("lib/ymn.lib/license", "GPL");
		m->declare("lib/ymn.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("basic.lib/name", "Faust Basic Element Library");
		m->declare("basic.lib/version", "0.0");
		m->declare("signal.lib/name", "Faust Signal Routing Library");
		m->declare("signal.lib/version", "0.0");
	}

	virtual int getNumInputs() 	{ return 1; }
	virtual int getNumOutputs() 	{ return 25; }
	virtual int getInputRate (int i) 	{int rate[]={1}; return rate[i]; }
	virtual int getOutputRate(int i) 	{int rate[]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; return rate[i]; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (8e+01 / double(iConst0));
		fcheckbox0 = 0.0;
		fentry0 = 1.07;
		fslider0 = 2.0;
		fslider1 = 0.0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		fslider2 = 0.0;
		fslider3 = 0.0;
		fConst2 = (3.4e+02 / double(iConst0));
		fConst3 = (1.7e+02 / double(iConst0));
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		fConst4 = (5.1e+02 / double(iConst0));
		fConst5 = (1.02e+03 / double(iConst0));
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fVec0[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		fConst6 = (789.5430205728705 / double(iConst0));
		fConst7 = (394.7715102864353 / double(iConst0));
		fConst8 = (625.2284897135664 / double(iConst0));
		fConst9 = (1250.4569794271329 / double(iConst0));
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		fConst10 = (984.7116049589241 / double(iConst0));
		fConst11 = (1969.4232099178482 / double(iConst0));
		fConst12 = (715.2883950410759 / double(iConst0));
		fConst13 = (1430.5767900821518 / double(iConst0));
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openHorizontalBox("0x00");
		interface->openVerticalBox("Parameters");
		interface->openHorizontalBox("Source  0");
		interface->declare(&fslider1, "0+1", "");
		interface->declare(&fslider1, "osc", "/gain_0 -20 20");
		interface->declare(&fslider1, "style", "knob");
		interface->addHorizontalSlider("Gain  0", &fslider1, 0.0, -2e+01, 2e+01, 0.1);
		interface->declare(&fslider0, "0+2", "");
		interface->declare(&fslider0, "osc", "/radius_0 0.5 50");
		interface->declare(&fslider0, "style", "knob");
		interface->addHorizontalSlider("Radius  0", &fslider0, 2.0, 0.5, 5e+01, 0.01);
		interface->declare(&fslider3, "0+3", "");
		interface->declare(&fslider3, "osc", "/azimuth_0 0 360");
		interface->declare(&fslider3, "style", "knob");
		interface->addHorizontalSlider("Azimuth  0", &fslider3, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		interface->declare(&fslider2, "0+4", "");
		interface->declare(&fslider2, "osc", "/elevation_0 -90 90");
		interface->declare(&fslider2, "style", "knob");
		interface->addHorizontalSlider("Elevation  0", &fslider2, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		interface->declare(0, "0+5", "");
		interface->openHorizontalBox("Spherical Wave");
		interface->addCheckButton("Yes", &fcheckbox0);
		interface->closeBox();
		interface->declare(&fentry0, "~", "");
		interface->addNumEntry("Speaker Radius  0", &fentry0, 1.07, 0.5, 1e+01, 0.01);
		interface->closeBox();
		interface->closeBox();
		interface->declare(0, "~", "");
		interface->openVerticalBox("Outputs");
		interface->openHorizontalBox("0");
		interface->openVerticalBox("0");
		interface->declare(&fbargraph0, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec6ae40", &fbargraph0, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("1");
		interface->openVerticalBox("1");
		interface->declare(&fbargraph1, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec70380", &fbargraph1, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("2");
		interface->declare(&fbargraph2, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec71dd0", &fbargraph2, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("3");
		interface->declare(&fbargraph3, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec73a30", &fbargraph3, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("2");
		interface->openVerticalBox("4");
		interface->declare(&fbargraph4, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec79a20", &fbargraph4, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("5");
		interface->declare(&fbargraph5, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec7b6c0", &fbargraph5, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("6");
		interface->declare(&fbargraph6, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec7d490", &fbargraph6, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("7");
		interface->declare(&fbargraph7, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec7f0c0", &fbargraph7, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("8");
		interface->declare(&fbargraph8, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec80f20", &fbargraph8, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("3");
		interface->openVerticalBox("9");
		interface->declare(&fbargraph9, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec88f50", &fbargraph9, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("10");
		interface->declare(&fbargraph10, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec8ac60", &fbargraph10, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("11");
		interface->declare(&fbargraph11, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec8cc10", &fbargraph11, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("12");
		interface->declare(&fbargraph12, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec8e9c0", &fbargraph12, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("13");
		interface->declare(&fbargraph13, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec906d0", &fbargraph13, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("14");
		interface->declare(&fbargraph14, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec92450", &fbargraph14, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("15");
		interface->declare(&fbargraph15, "unit", "dB");
		interface->addVerticalBargraph("0x7f980ec94400", &fbargraph15, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("4");
		interface->openVerticalBox("16");
		interface->declare(&fbargraph16, "unit", "dB");
		interface->addVerticalBargraph("0x7f980e93c860", &fbargraph16, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("17");
		interface->declare(&fbargraph17, "unit", "dB");
		interface->addVerticalBargraph("0x7f980e93e5b0", &fbargraph17, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("18");
		interface->declare(&fbargraph18, "unit", "dB");
		interface->addVerticalBargraph("0x7f980e940760", &fbargraph18, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("19");
		interface->declare(&fbargraph19, "unit", "dB");
		interface->addVerticalBargraph("0x7f980e942720", &fbargraph19, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("20");
		interface->declare(&fbargraph20, "unit", "dB");
		interface->addVerticalBargraph("0x7f980e9448f0", &fbargraph20, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("21");
		interface->declare(&fbargraph21, "unit", "dB");
		interface->addVerticalBargraph("0x7f980e946690", &fbargraph21, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("22");
		interface->declare(&fbargraph22, "unit", "dB");
		interface->addVerticalBargraph("0x7f980e9484f0", &fbargraph22, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("23");
		interface->declare(&fbargraph23, "unit", "dB");
		interface->addVerticalBargraph("0x7f980e94a3c0", &fbargraph23, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("24");
		interface->declare(&fbargraph24, "unit", "dB");
		interface->addVerticalBargraph("0x7f980e94c430", &fbargraph24, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->closeBox();
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = double(fcheckbox0);
		double 	fSlow1 = (1 - fSlow0);
		double 	fSlow2 = double(fentry0);
		double 	fSlow3 = (fSlow0 * fSlow2);
		double 	fSlow4 = double(fslider0);
		double 	fSlow5 = (fSlow1 + (fSlow3 / fSlow4));
		double 	fSlow6 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider1))));
		double fTemp0;
		double fTemp1;
		double 	fSlow7 = double(fslider2);
		double 	fSlow8 = cos(fSlow7);
		double 	fSlow9 = double(fslider3);
		double 	fSlow10 = sin(fSlow9);
		double 	fSlow11 = (1.7320508075688772 * (fSlow8 * fSlow10));
		double 	fSlow12 = (fConst3 / fSlow4);
		double 	fSlow13 = (1 + fSlow12);
		double 	fSlow14 = (fConst2 / (fSlow4 * fSlow13));
		double 	fSlow15 = (fConst3 / fSlow2);
		double 	fSlow16 = (1.0 / (1 + fSlow15));
		double 	fSlow17 = ((fSlow3 * fSlow13) / fSlow4);
		double 	fSlow18 = (fConst2 / fSlow2);
		double fTemp2;
		double fTemp3;
		double fTemp4;
		double 	fSlow19 = sin(fSlow7);
		double 	fSlow20 = (1.7320508075688772 * fSlow19);
		double fTemp5;
		double 	fSlow21 = cos(fSlow9);
		double 	fSlow22 = (1.7320508075688772 * (fSlow8 * fSlow21));
		double fTemp6;
		double 	fSlow23 = faustpower<2>(fSlow8);
		double 	fSlow24 = (2 * fSlow9);
		double 	fSlow25 = sin(fSlow24);
		double 	fSlow26 = (1.9364916731037085 * (fSlow23 * fSlow25));
		double 	fSlow27 = faustpower<2>(fSlow12);
		double 	fSlow28 = (1 + ((fConst4 / fSlow4) + (3 * fSlow27)));
		double 	fSlow29 = (1.0 / fSlow28);
		double 	fSlow30 = (12 * fSlow27);
		double 	fSlow31 = (fSlow30 + (fConst5 / fSlow4));
		double 	fSlow32 = faustpower<2>(fSlow15);
		double 	fSlow33 = (1.0 / (1 + ((fConst4 / fSlow2) + (3 * fSlow32))));
		double 	fSlow34 = ((fSlow3 * fSlow28) / fSlow4);
		double 	fSlow35 = (12 * fSlow32);
		double 	fSlow36 = (fSlow35 + (fConst5 / fSlow2));
		double fTemp7;
		double fTemp8;
		double fTemp9;
		double 	fSlow37 = sin((2 * fSlow7));
		double 	fSlow38 = (1.9364916731037085 * (fSlow37 * fSlow10));
		double fTemp10;
		double 	fSlow39 = faustpower<2>(fSlow19);
		double 	fSlow40 = (1.118033988749895 * ((3 * fSlow39) - 1));
		double fTemp11;
		double 	fSlow41 = (1.9364916731037085 * (fSlow37 * fSlow21));
		double fTemp12;
		double 	fSlow42 = cos(fSlow24);
		double 	fSlow43 = (1.9364916731037085 * (fSlow23 * fSlow42));
		double fTemp13;
		double 	fSlow44 = faustpower<3>(fSlow8);
		double 	fSlow45 = (3 * fSlow9);
		double 	fSlow46 = sin(fSlow45);
		double 	fSlow47 = (2.091650066335189 * (fSlow44 * fSlow46));
		double 	fSlow48 = (1 + (fConst7 / fSlow4));
		double 	fSlow49 = (fConst6 / (fSlow4 * fSlow48));
		double 	fSlow50 = (1.0 / (1 + (fConst7 / fSlow2)));
		double 	fSlow51 = (1.0 / (1 + ((fConst8 / fSlow2) + (6.45943269348338 * fSlow32))));
		double 	fSlow52 = (1 + ((fConst8 / fSlow4) + (6.45943269348338 * fSlow27)));
		double 	fSlow53 = ((fSlow3 * fSlow52) / fSlow4);
		double 	fSlow54 = (25.83773077393352 * fSlow32);
		double 	fSlow55 = (fSlow54 + (fConst9 / fSlow2));
		double 	fSlow56 = (1.0 / fSlow52);
		double 	fSlow57 = (25.83773077393352 * fSlow27);
		double 	fSlow58 = (fSlow57 + (fConst9 / fSlow4));
		double fTemp14;
		double 	fSlow59 = (fConst6 / fSlow2);
		double fTemp15;
		double fTemp16;
		double 	fSlow60 = (fSlow23 * fSlow19);
		double 	fSlow61 = (5.123475382979799 * (fSlow60 * fSlow25));
		double fTemp17;
		double 	fSlow62 = (5 * fSlow39);
		double 	fSlow63 = (fSlow8 * (fSlow62 - 1));
		double 	fSlow64 = (1.620185174601965 * (fSlow63 * fSlow10));
		double fTemp18;
		double 	fSlow65 = (1.3228756555322954 * (fSlow19 * (fSlow62 - 3)));
		double fTemp19;
		double 	fSlow66 = (1.620185174601965 * (fSlow63 * fSlow21));
		double fTemp20;
		double 	fSlow67 = (5.123475382979799 * (fSlow60 * fSlow42));
		double fTemp21;
		double 	fSlow68 = cos(fSlow45);
		double 	fSlow69 = (2.091650066335189 * (fSlow44 * fSlow68));
		double fTemp22;
		double 	fSlow70 = faustpower<4>(fSlow8);
		double 	fSlow71 = (4 * fSlow9);
		double 	fSlow72 = (2.218529918662356 * (fSlow70 * sin(fSlow71)));
		double 	fSlow73 = (1 + ((fConst10 / fSlow4) + (9.1401308902779 * fSlow27)));
		double 	fSlow74 = (1.0 / fSlow73);
		double 	fSlow75 = (36.5605235611116 * fSlow27);
		double 	fSlow76 = ((fConst11 / fSlow4) + fSlow75);
		double 	fSlow77 = (1.0 / (1 + ((9.1401308902779 * fSlow32) + (fConst10 / fSlow2))));
		double 	fSlow78 = (1.0 / (1 + ((fConst12 / fSlow2) + (11.4878004768713 * fSlow32))));
		double 	fSlow79 = (1 + ((fConst12 / fSlow4) + (11.4878004768713 * fSlow27)));
		double 	fSlow80 = ((fSlow3 * fSlow79) / fSlow4);
		double 	fSlow81 = (45.9512019074852 * fSlow32);
		double 	fSlow82 = (fSlow81 + (fConst13 / fSlow2));
		double 	fSlow83 = (1.0 / fSlow79);
		double 	fSlow84 = (45.9512019074852 * fSlow27);
		double 	fSlow85 = (fSlow84 + (fConst13 / fSlow4));
		double fTemp23;
		double 	fSlow86 = (36.5605235611116 * fSlow32);
		double 	fSlow87 = (fSlow86 + (fConst11 / fSlow2));
		double fTemp24;
		double fTemp25;
		double fTemp26;
		double 	fSlow88 = (fSlow44 * fSlow19);
		double 	fSlow89 = (6.274950199005566 * (fSlow88 * fSlow46));
		double fTemp27;
		double 	fSlow90 = (7 * fSlow39);
		double 	fSlow91 = (fSlow23 * (fSlow90 - 1));
		double 	fSlow92 = (1.6770509831248424 * (fSlow91 * fSlow25));
		double fTemp28;
		double 	fSlow93 = (fSlow37 * (fSlow90 - 3));
		double 	fSlow94 = (1.1858541225631423 * (fSlow93 * fSlow10));
		double fTemp29;
		double 	fSlow95 = (0.375 * (3 + (fSlow39 * ((35 * fSlow39) - 30))));
		double fTemp30;
		double 	fSlow96 = (1.1858541225631423 * (fSlow93 * fSlow21));
		double fTemp31;
		double 	fSlow97 = (1.6770509831248424 * (fSlow91 * fSlow42));
		double fTemp32;
		double 	fSlow98 = (6.274950199005566 * (fSlow88 * fSlow68));
		double fTemp33;
		double 	fSlow99 = (2.218529918662356 * (fSlow70 * cos(fSlow71)));
		double fTemp34;
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		FAUSTFLOAT* output2 = output[2];
		FAUSTFLOAT* output3 = output[3];
		FAUSTFLOAT* output4 = output[4];
		FAUSTFLOAT* output5 = output[5];
		FAUSTFLOAT* output6 = output[6];
		FAUSTFLOAT* output7 = output[7];
		FAUSTFLOAT* output8 = output[8];
		FAUSTFLOAT* output9 = output[9];
		FAUSTFLOAT* output10 = output[10];
		FAUSTFLOAT* output11 = output[11];
		FAUSTFLOAT* output12 = output[12];
		FAUSTFLOAT* output13 = output[13];
		FAUSTFLOAT* output14 = output[14];
		FAUSTFLOAT* output15 = output[15];
		FAUSTFLOAT* output16 = output[16];
		FAUSTFLOAT* output17 = output[17];
		FAUSTFLOAT* output18 = output[18];
		FAUSTFLOAT* output19 = output[19];
		FAUSTFLOAT* output20 = output[20];
		FAUSTFLOAT* output21 = output[21];
		FAUSTFLOAT* output22 = output[22];
		FAUSTFLOAT* output23 = output[23];
		FAUSTFLOAT* output24 = output[24];
		for (int i=0; i<count; i++) {
			fRec1[0] = ((0.999 * fRec1[1]) + fSlow6);
			fTemp0 = ((double)input0[i] * fRec1[0]);
			fTemp1 = (fSlow5 * fTemp0);
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp1))))));
			fbargraph0 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp1;
			fRec5[0] = (fRec5[1] + fRec4[1]);
			fRec4[0] = (fSlow16 * ((fSlow17 * fTemp0) - (fSlow18 * fRec5[0])));
			fRec3[0] = (fRec4[0] + fRec3[1]);
			fTemp2 = (fSlow1 * fTemp0);
			fTemp3 = ((fSlow14 * (fRec3[0] - fRec4[0])) + (fRec4[0] + fTemp2));
			fTemp4 = (fSlow11 * fTemp3);
			fRec2[0] = max((fRec2[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fbargraph1 = fRec2[0];
			output1[i] = (FAUSTFLOAT)fTemp4;
			fTemp5 = (fSlow20 * fTemp3);
			fRec6[0] = max((fRec6[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fbargraph2 = fRec6[0];
			output2[i] = (FAUSTFLOAT)fTemp5;
			fTemp6 = (fSlow22 * fTemp3);
			fRec7[0] = max((fRec7[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fbargraph3 = fRec7[0];
			output3[i] = (FAUSTFLOAT)fTemp6;
			fRec11[0] = (fRec11[1] + fRec10[1]);
			fRec12[0] = (fRec12[1] + fRec11[1]);
			fRec10[0] = (fSlow33 * ((fSlow34 * fTemp0) - ((fSlow36 * fRec11[0]) + (fSlow35 * fRec12[0]))));
			fRec9[0] = (fRec10[0] + fRec9[1]);
			fTemp7 = (fRec9[0] - fRec10[0]);
			fVec0[0] = fTemp7;
			fRec13[0] = (fRec13[1] + fVec0[1]);
			fTemp8 = ((fSlow29 * ((fSlow31 * fVec0[0]) + (fSlow30 * fRec13[0]))) + (fRec10[0] + fTemp2));
			fTemp9 = (fSlow26 * fTemp8);
			fRec8[0] = max((fRec8[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fbargraph4 = fRec8[0];
			output4[i] = (FAUSTFLOAT)fTemp9;
			fTemp10 = (fSlow38 * fTemp8);
			fRec14[0] = max((fRec14[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fbargraph5 = fRec14[0];
			output5[i] = (FAUSTFLOAT)fTemp10;
			fTemp11 = (fSlow40 * fTemp8);
			fRec15[0] = max((fRec15[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fbargraph6 = fRec15[0];
			output6[i] = (FAUSTFLOAT)fTemp11;
			fTemp12 = (fSlow41 * fTemp8);
			fRec16[0] = max((fRec16[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fbargraph7 = fRec16[0];
			output7[i] = (FAUSTFLOAT)fTemp12;
			fTemp13 = (fSlow43 * fTemp8);
			fRec17[0] = max((fRec17[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fbargraph8 = fRec17[0];
			output8[i] = (FAUSTFLOAT)fTemp13;
			fRec22[0] = (fRec22[1] + fRec21[1]);
			fRec23[0] = (fRec23[1] + fRec22[1]);
			fRec21[0] = (fSlow51 * ((fSlow53 * fTemp0) - ((fSlow55 * fRec22[0]) + (fSlow54 * fRec23[0]))));
			fRec24[0] = (fRec21[0] + fRec24[1]);
			fTemp14 = (fRec24[0] - fRec21[0]);
			fVec1[0] = fTemp14;
			fRec25[0] = (fRec25[1] + fVec1[1]);
			fRec26[0] = (fRec26[1] + fRec20[1]);
			fRec20[0] = (fSlow50 * ((fSlow48 * (fRec21[0] + (fSlow56 * ((fSlow58 * fVec1[0]) + (fSlow57 * fRec25[0]))))) - (fSlow59 * fRec26[0])));
			fRec19[0] = (fRec20[0] + fRec19[1]);
			fTemp15 = ((fSlow49 * (fRec19[0] - fRec20[0])) + (fRec20[0] + fTemp2));
			fTemp16 = (fSlow47 * fTemp15);
			fRec18[0] = max((fRec18[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fbargraph9 = fRec18[0];
			output9[i] = (FAUSTFLOAT)fTemp16;
			fTemp17 = (fSlow61 * fTemp15);
			fRec27[0] = max((fRec27[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fbargraph10 = fRec27[0];
			output10[i] = (FAUSTFLOAT)fTemp17;
			fTemp18 = (fSlow64 * fTemp15);
			fRec28[0] = max((fRec28[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fbargraph11 = fRec28[0];
			output11[i] = (FAUSTFLOAT)fTemp18;
			fTemp19 = (fSlow65 * fTemp15);
			fRec29[0] = max((fRec29[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fbargraph12 = fRec29[0];
			output12[i] = (FAUSTFLOAT)fTemp19;
			fTemp20 = (fSlow66 * fTemp15);
			fRec30[0] = max((fRec30[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph13 = fRec30[0];
			output13[i] = (FAUSTFLOAT)fTemp20;
			fTemp21 = (fSlow67 * fTemp15);
			fRec31[0] = max((fRec31[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fbargraph14 = fRec31[0];
			output14[i] = (FAUSTFLOAT)fTemp21;
			fTemp22 = (fSlow69 * fTemp15);
			fRec32[0] = max((fRec32[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fbargraph15 = fRec32[0];
			output15[i] = (FAUSTFLOAT)fTemp22;
			fRec37[0] = (fRec37[1] + fRec36[1]);
			fRec38[0] = (fRec38[1] + fRec37[1]);
			fRec36[0] = (fSlow78 * ((fSlow80 * fTemp0) - ((fSlow82 * fRec37[0]) + (fSlow81 * fRec38[0]))));
			fRec39[0] = (fRec36[0] + fRec39[1]);
			fTemp23 = (fRec39[0] - fRec36[0]);
			fVec2[0] = fTemp23;
			fRec40[0] = (fRec40[1] + fVec2[1]);
			fRec41[0] = (fRec41[1] + fRec35[1]);
			fRec42[0] = (fRec42[1] + fRec41[1]);
			fRec35[0] = (fSlow77 * ((fSlow73 * (fRec36[0] + (fSlow83 * ((fSlow85 * fVec2[0]) + (fSlow84 * fRec40[0]))))) - ((fSlow87 * fRec41[0]) + (fSlow86 * fRec42[0]))));
			fRec34[0] = (fRec35[0] + fRec34[1]);
			fTemp24 = (fRec34[0] - fRec35[0]);
			fVec3[0] = fTemp24;
			fRec43[0] = (fRec43[1] + fVec3[1]);
			fTemp25 = ((fSlow74 * ((fSlow76 * fVec3[0]) + (fSlow75 * fRec43[0]))) + (fRec35[0] + fTemp2));
			fTemp26 = (fSlow72 * fTemp25);
			fRec33[0] = max((fRec33[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fbargraph16 = fRec33[0];
			output16[i] = (FAUSTFLOAT)fTemp26;
			fTemp27 = (fSlow89 * fTemp25);
			fRec44[0] = max((fRec44[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fbargraph17 = fRec44[0];
			output17[i] = (FAUSTFLOAT)fTemp27;
			fTemp28 = (fSlow92 * fTemp25);
			fRec45[0] = max((fRec45[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph18 = fRec45[0];
			output18[i] = (FAUSTFLOAT)fTemp28;
			fTemp29 = (fSlow94 * fTemp25);
			fRec46[0] = max((fRec46[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph19 = fRec46[0];
			output19[i] = (FAUSTFLOAT)fTemp29;
			fTemp30 = (fSlow95 * fTemp25);
			fRec47[0] = max((fRec47[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fbargraph20 = fRec47[0];
			output20[i] = (FAUSTFLOAT)fTemp30;
			fTemp31 = (fSlow96 * fTemp25);
			fRec48[0] = max((fRec48[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fbargraph21 = fRec48[0];
			output21[i] = (FAUSTFLOAT)fTemp31;
			fTemp32 = (fSlow97 * fTemp25);
			fRec49[0] = max((fRec49[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph22 = fRec49[0];
			output22[i] = (FAUSTFLOAT)fTemp32;
			fTemp33 = (fSlow98 * fTemp25);
			fRec50[0] = max((fRec50[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph23 = fRec50[0];
			output23[i] = (FAUSTFLOAT)fTemp33;
			fTemp34 = (fSlow99 * fTemp25);
			fRec51[0] = max((fRec51[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fbargraph24 = fRec51[0];
			output24[i] = (FAUSTFLOAT)fTemp34;
			// post processing
			fRec51[1] = fRec51[0];
			fRec50[1] = fRec50[0];
			fRec49[1] = fRec49[0];
			fRec48[1] = fRec48[0];
			fRec47[1] = fRec47[0];
			fRec46[1] = fRec46[0];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec33[1] = fRec33[0];
			fRec43[1] = fRec43[0];
			fVec3[1] = fVec3[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			fRec42[1] = fRec42[0];
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fVec2[1] = fVec2[0];
			fRec39[1] = fRec39[0];
			fRec36[1] = fRec36[0];
			fRec38[1] = fRec38[0];
			fRec37[1] = fRec37[0];
			fRec32[1] = fRec32[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
			fRec27[1] = fRec27[0];
			fRec18[1] = fRec18[0];
			fRec19[1] = fRec19[0];
			fRec20[1] = fRec20[0];
			fRec26[1] = fRec26[0];
			fRec25[1] = fRec25[0];
			fVec1[1] = fVec1[0];
			fRec24[1] = fRec24[0];
			fRec21[1] = fRec21[0];
			fRec23[1] = fRec23[0];
			fRec22[1] = fRec22[0];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			fRec15[1] = fRec15[0];
			fRec14[1] = fRec14[0];
			fRec8[1] = fRec8[0];
			fRec13[1] = fRec13[0];
			fVec0[1] = fVec0[0];
			fRec9[1] = fRec9[0];
			fRec10[1] = fRec10[0];
			fRec12[1] = fRec12[0];
			fRec11[1] = fRec11[0];
			fRec7[1] = fRec7[0];
			fRec6[1] = fRec6[0];
			fRec2[1] = fRec2[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec0[1] = fRec0[0];
			fRec1[1] = fRec1[0];
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
