//----------------------------------------------------------
// name: "HOAConverter4"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2016"
//
// Code generated with Faust 0.9.73-mr2 (http://faust.grame.fr)
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
	double 	fConst0;
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fslider1;
	double 	fRec1[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec3[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec4[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fRec5[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec2[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec6[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec7[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec9[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec10[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec11[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec8[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec13[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec14[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec12[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec15[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec16[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec17[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec19[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec20[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec21[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec18[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec23[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec24[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec22[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec26[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec25[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec28[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec27[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec29[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec30[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec31[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec33[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec32[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec35[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec34[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec37[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec36[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec39[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec38[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec41[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec40[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec43[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec42[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec45[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec44[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec47[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec49[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec48[2];
	FAUSTFLOAT 	fbargraph49;
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("name", "HOAConverter4");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("signal.lib/name", "Faust Signal Routing Library");
		m->declare("signal.lib/version", "0.0");
		m->declare("route.lib/name", "Faust Signal Routing Library");
		m->declare("route.lib/version", "0.0");
		m->declare("basic.lib/name", "Faust Basic Element Library");
		m->declare("basic.lib/version", "0.0");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
	}

	virtual int getNumInputs() 	{ return 25; }
	virtual int getNumOutputs() 	{ return 25; }
	virtual int getInputRate (int i) 	{int rate[]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; return rate[i]; }
	virtual int getOutputRate(int i) 	{int rate[]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; return rate[i]; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = (8e+01 / double(min(192000, max(1, fSamplingFreq))));
		fslider0 = 1.0;
		fslider1 = 1.0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("0x00");
		interface->declare(&fslider0, "0", "");
		interface->declare(&fslider0, "style", "knob");
		interface->addHorizontalSlider("Input (1-ACN_N3D,2-ACN_SN3D, 3-FuMa)", &fslider0, 1.0, 1.0, 3.0, 1.0);
		interface->declare(&fslider1, "0", "");
		interface->declare(&fslider1, "style", "knob");
		interface->addHorizontalSlider("Output (1-ACN_N3D,2-ACN_SN3D, 3-FuMa)", &fslider1, 1.0, 1.0, 3.0, 1.0);
		interface->declare(0, "1", "");
		interface->openHorizontalBox("Inputs");
		interface->openHorizontalBox("0");
		interface->openVerticalBox("0");
		interface->declare(&fbargraph0, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea222c50", &fbargraph0, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("1");
		interface->openVerticalBox("1");
		interface->declare(&fbargraph2, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea226980", &fbargraph2, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("2");
		interface->declare(&fbargraph3, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea228150", &fbargraph3, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("3");
		interface->declare(&fbargraph4, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea229c10", &fbargraph4, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("2");
		interface->openVerticalBox("4");
		interface->declare(&fbargraph8, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea231700", &fbargraph8, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("5");
		interface->declare(&fbargraph12, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea2385a0", &fbargraph12, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("6");
		interface->declare(&fbargraph10, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea234b20", &fbargraph10, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("7");
		interface->declare(&fbargraph13, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea23a1c0", &fbargraph13, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("8");
		interface->declare(&fbargraph9, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea232f00", &fbargraph9, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("3");
		interface->openVerticalBox("9");
		interface->declare(&fbargraph18, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea244bb0", &fbargraph18, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("10");
		interface->declare(&fbargraph22, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea24bd40", &fbargraph22, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("11");
		interface->declare(&fbargraph25, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea251810", &fbargraph25, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("12");
		interface->declare(&fbargraph20, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea2481d0", &fbargraph20, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("13");
		interface->declare(&fbargraph23, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea24d6d0", &fbargraph23, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("14");
		interface->declare(&fbargraph27, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea255a50", &fbargraph27, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("15");
		interface->declare(&fbargraph19, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea246540", &fbargraph19, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("4");
		interface->openVerticalBox("16");
		interface->declare(&fbargraph32, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea260e70", &fbargraph32, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("17");
		interface->declare(&fbargraph34, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea264950", &fbargraph34, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("18");
		interface->declare(&fbargraph36, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea268290", &fbargraph36, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("19");
		interface->declare(&fbargraph38, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea26bcb0", &fbargraph38, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("20");
		interface->declare(&fbargraph40, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea26f7b0", &fbargraph40, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("21");
		interface->declare(&fbargraph42, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea273390", &fbargraph42, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("22");
		interface->declare(&fbargraph44, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea277050", &fbargraph44, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("23");
		interface->declare(&fbargraph46, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea27adf0", &fbargraph46, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("24");
		interface->declare(&fbargraph48, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea27ec70", &fbargraph48, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->closeBox();
		interface->declare(0, "2", "");
		interface->openHorizontalBox("Outputs");
		interface->openHorizontalBox("0");
		interface->openVerticalBox("0");
		interface->declare(&fbargraph1, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea224c40", &fbargraph1, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("1");
		interface->openVerticalBox("1");
		interface->declare(&fbargraph5, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea22ba40", &fbargraph5, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("2");
		interface->declare(&fbargraph6, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea22db00", &fbargraph6, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("3");
		interface->declare(&fbargraph7, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea22f890", &fbargraph7, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("2");
		interface->openVerticalBox("4");
		interface->declare(&fbargraph11, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea236790", &fbargraph11, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("5");
		interface->declare(&fbargraph14, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea23c290", &fbargraph14, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("6");
		interface->declare(&fbargraph15, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea23e4b0", &fbargraph15, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("7");
		interface->declare(&fbargraph16, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea240590", &fbargraph16, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("8");
		interface->declare(&fbargraph17, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea242b60", &fbargraph17, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("3");
		interface->openVerticalBox("9");
		interface->declare(&fbargraph21, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea24a250", &fbargraph21, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("10");
		interface->declare(&fbargraph24, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea24fb10", &fbargraph24, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("11");
		interface->declare(&fbargraph26, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea253cc0", &fbargraph26, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("12");
		interface->declare(&fbargraph28, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea257c10", &fbargraph28, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("13");
		interface->declare(&fbargraph29, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea259fe0", &fbargraph29, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("14");
		interface->declare(&fbargraph30, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea25c290", &fbargraph30, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("15");
		interface->declare(&fbargraph31, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea25eec0", &fbargraph31, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->openHorizontalBox("4");
		interface->openVerticalBox("16");
		interface->declare(&fbargraph33, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea262ab0", &fbargraph33, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("17");
		interface->declare(&fbargraph35, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea266370", &fbargraph35, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("18");
		interface->declare(&fbargraph37, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea269d20", &fbargraph37, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("19");
		interface->declare(&fbargraph39, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea26d7b0", &fbargraph39, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("20");
		interface->declare(&fbargraph41, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea271320", &fbargraph41, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("21");
		interface->declare(&fbargraph43, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea274f70", &fbargraph43, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("22");
		interface->declare(&fbargraph45, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea278ca0", &fbargraph45, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("23");
		interface->declare(&fbargraph47, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea27cab0", &fbargraph47, -7e+01, 6.0);
		interface->closeBox();
		interface->openVerticalBox("24");
		interface->declare(&fbargraph49, "unit", "dB");
		interface->addVerticalBargraph("0x7fafea2809a0", &fbargraph49, -7e+01, 6.0);
		interface->closeBox();
		interface->closeBox();
		interface->closeBox();
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		int 	iSlow0 = int((((3 * rint(double(fslider0))) + rint(double(fslider1))) - 4));
		int 	iSlow1 = int((iSlow0 >= 5));
		int 	iSlow2 = int((iSlow0 >= 7));
		int 	iSlow3 = int((iSlow0 >= 8));
		double fTemp0;
		double fTemp1;
		double fTemp2;
		int 	iSlow4 = int((iSlow0 >= 6));
		double fTemp3;
		int 	iSlow5 = int((iSlow0 >= 3));
		int 	iSlow6 = int((iSlow0 >= 2));
		double fTemp4;
		double fTemp5;
		double fTemp6;
		double fTemp7;
		double fTemp8;
		double fTemp9;
		double fTemp10;
		double fTemp11;
		int 	iSlow7 = int((iSlow0 >= 4));
		double fTemp12;
		double fTemp13;
		int 	iSlow8 = int((iSlow0 >= 1));
		double fTemp14;
		double fTemp15;
		double fTemp16;
		double fTemp17;
		double fTemp18;
		double fTemp19;
		double fTemp20;
		double fTemp21;
		double fTemp22;
		double fTemp23;
		double fTemp24;
		double fTemp25;
		double fTemp26;
		double fTemp27;
		double fTemp28;
		double fTemp29;
		double fTemp30;
		double fTemp31;
		double fTemp32;
		double fTemp33;
		double fTemp34;
		double fTemp35;
		double fTemp36;
		double fTemp37;
		double fTemp38;
		double fTemp39;
		double fTemp40;
		double fTemp41;
		double fTemp42;
		double fTemp43;
		double fTemp44;
		double fTemp45;
		double fTemp46;
		double fTemp47;
		double fTemp48;
		double fTemp49;
		double fTemp50;
		double fTemp51;
		double fTemp52;
		double fTemp53;
		double fTemp54;
		double fTemp55;
		double fTemp56;
		double fTemp57;
		double fTemp58;
		double fTemp59;
		double fTemp60;
		double fTemp61;
		double fTemp62;
		double fTemp63;
		double fTemp64;
		double fTemp65;
		double fTemp66;
		double fTemp67;
		double fTemp68;
		double fTemp69;
		double fTemp70;
		double fTemp71;
		double fTemp72;
		double fTemp73;
		double fTemp74;
		double fTemp75;
		double fTemp76;
		double fTemp77;
		double fTemp78;
		double fTemp79;
		double fTemp80;
		double fTemp81;
		double fTemp82;
		double fTemp83;
		double fTemp84;
		double fTemp85;
		double fTemp86;
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* input2 = input[2];
		FAUSTFLOAT* input3 = input[3];
		FAUSTFLOAT* input4 = input[4];
		FAUSTFLOAT* input5 = input[5];
		FAUSTFLOAT* input6 = input[6];
		FAUSTFLOAT* input7 = input[7];
		FAUSTFLOAT* input8 = input[8];
		FAUSTFLOAT* input9 = input[9];
		FAUSTFLOAT* input10 = input[10];
		FAUSTFLOAT* input11 = input[11];
		FAUSTFLOAT* input12 = input[12];
		FAUSTFLOAT* input13 = input[13];
		FAUSTFLOAT* input14 = input[14];
		FAUSTFLOAT* input15 = input[15];
		FAUSTFLOAT* input16 = input[16];
		FAUSTFLOAT* input17 = input[17];
		FAUSTFLOAT* input18 = input[18];
		FAUSTFLOAT* input19 = input[19];
		FAUSTFLOAT* input20 = input[20];
		FAUSTFLOAT* input21 = input[21];
		FAUSTFLOAT* input22 = input[22];
		FAUSTFLOAT* input23 = input[23];
		FAUSTFLOAT* input24 = input[24];
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
			fTemp0 = (double)input0[i];
			fRec1[0] = max((fRec1[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fbargraph0 = fRec1[0];
			fTemp1 = fTemp0;
			fTemp2 = (1.4142135623730951 * fTemp1);
			fTemp3 = (0.7071067811865475 * fTemp1);
			fTemp4 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp1:fTemp2):((iSlow4)?fTemp2:fTemp3)):((iSlow5)?fTemp1:((iSlow6)?fTemp3:fTemp1)));
			fRec0[0] = max((fRec0[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fbargraph1 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp4;
			fTemp5 = (double)input1[i];
			fRec3[0] = max((fRec3[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fbargraph2 = fRec3[0];
			fTemp6 = fTemp5;
			fTemp7 = (double)input2[i];
			fRec4[0] = max((fRec4[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp7))))));
			fbargraph3 = fRec4[0];
			fTemp8 = fTemp7;
			fTemp9 = (1.7320508075688772 * fTemp8);
			fTemp10 = (double)input3[i];
			fRec5[0] = max((fRec5[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fbargraph4 = fRec5[0];
			fTemp11 = fTemp10;
			fTemp12 = (1.7320508075688772 * fTemp6);
			fTemp13 = (0.5773502691896258 * fTemp11);
			fTemp14 = (0.5773502691896258 * fTemp6);
			fTemp15 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp6:fTemp8):((iSlow4)?fTemp9:fTemp11)):((iSlow5)?((iSlow7)?fTemp6:fTemp12):((iSlow6)?fTemp13:((iSlow8)?fTemp14:fTemp6))));
			fRec2[0] = max((fRec2[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fbargraph5 = fRec2[0];
			output1[i] = (FAUSTFLOAT)fTemp15;
			fTemp16 = (1.7320508075688772 * fTemp11);
			fTemp17 = (0.5773502691896258 * fTemp8);
			fTemp18 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp8:fTemp11):((iSlow4)?fTemp16:fTemp6)):((iSlow5)?((iSlow7)?fTemp8:fTemp9):((iSlow6)?fTemp14:((iSlow8)?fTemp17:fTemp8))));
			fRec6[0] = max((fRec6[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fbargraph6 = fRec6[0];
			output2[i] = (FAUSTFLOAT)fTemp18;
			fTemp19 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp11:fTemp6):((iSlow4)?fTemp12:fTemp8)):((iSlow5)?((iSlow7)?fTemp11:fTemp16):((iSlow6)?fTemp17:((iSlow8)?fTemp13:fTemp11))));
			fRec7[0] = max((fRec7[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fbargraph7 = fRec7[0];
			output3[i] = (FAUSTFLOAT)fTemp19;
			fTemp20 = (double)input4[i];
			fRec9[0] = max((fRec9[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph8 = fRec9[0];
			fTemp21 = fTemp20;
			fTemp22 = (double)input8[i];
			fRec10[0] = max((fRec10[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fbargraph9 = fRec10[0];
			fTemp23 = fTemp22;
			fTemp24 = (double)input6[i];
			fRec11[0] = max((fRec11[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fbargraph10 = fRec11[0];
			fTemp25 = fTemp24;
			fTemp26 = (2.23606797749979 * fTemp21);
			fTemp27 = (0.4472135954999579 * fTemp25);
			fTemp28 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp21:(0.8660254037844387 * fTemp23)):((iSlow4)?(1.9364916731037085 * fTemp23):fTemp25)):((iSlow5)?((iSlow7)?fTemp21:fTemp26):((iSlow6)?fTemp27:((iSlow8)?(0.4472135954999579 * fTemp21):fTemp21))));
			fRec8[0] = max((fRec8[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph11 = fRec8[0];
			output4[i] = (FAUSTFLOAT)fTemp28;
			fTemp29 = (double)input5[i];
			fRec13[0] = max((fRec13[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph12 = fRec13[0];
			fTemp30 = fTemp29;
			fTemp31 = (double)input7[i];
			fRec14[0] = max((fRec14[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fbargraph13 = fRec14[0];
			fTemp32 = fTemp31;
			fTemp33 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp30:(0.8660254037844387 * fTemp25)):((iSlow4)?(1.9364916731037085 * fTemp25):(1.1547005383792515 * fTemp32))):((iSlow5)?((iSlow7)?fTemp30:(2.23606797749979 * fTemp30)):((iSlow6)?(0.5163977794943222 * fTemp32):((iSlow8)?(0.4472135954999579 * fTemp30):fTemp30))));
			fRec12[0] = max((fRec12[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph14 = fRec12[0];
			output5[i] = (FAUSTFLOAT)fTemp33;
			fTemp34 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp25:fTemp21):((iSlow4)?fTemp26:(1.1547005383792515 * fTemp30))):((iSlow5)?((iSlow7)?fTemp25:(2.23606797749979 * fTemp25)):((iSlow6)?(0.5163977794943222 * fTemp30):((iSlow8)?fTemp27:fTemp25))));
			fRec15[0] = max((fRec15[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fbargraph15 = fRec15[0];
			output6[i] = (FAUSTFLOAT)fTemp34;
			fTemp35 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp32:(0.8660254037844387 * fTemp30)):((iSlow4)?(1.9364916731037085 * fTemp30):(1.1547005383792515 * fTemp23))):((iSlow5)?((iSlow7)?fTemp32:(2.23606797749979 * fTemp32)):((iSlow6)?(0.5163977794943222 * fTemp23):((iSlow8)?(0.4472135954999579 * fTemp32):fTemp32))));
			fRec16[0] = max((fRec16[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fbargraph16 = fRec16[0];
			output7[i] = (FAUSTFLOAT)fTemp35;
			fTemp36 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp23:(0.8660254037844387 * fTemp32)):((iSlow4)?(1.9364916731037085 * fTemp32):(1.1547005383792515 * fTemp21))):((iSlow5)?((iSlow7)?fTemp23:(2.23606797749979 * fTemp23)):((iSlow6)?(0.5163977794943222 * fTemp21):((iSlow8)?(0.4472135954999579 * fTemp23):fTemp23))));
			fRec17[0] = max((fRec17[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph17 = fRec17[0];
			output8[i] = (FAUSTFLOAT)fTemp36;
			fTemp37 = (double)input9[i];
			fRec19[0] = max((fRec19[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fbargraph18 = fRec19[0];
			fTemp38 = fTemp37;
			fTemp39 = (double)input15[i];
			fRec20[0] = max((fRec20[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fbargraph19 = fRec20[0];
			fTemp40 = fTemp39;
			fTemp41 = (double)input12[i];
			fRec21[0] = max((fRec21[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fbargraph20 = fRec21[0];
			fTemp42 = fTemp41;
			fTemp43 = (2.6457513110645907 * fTemp38);
			fTemp44 = (0.3779644730092272 * fTemp42);
			fTemp45 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp38:(0.7905694150420948 * fTemp40)):((iSlow4)?(2.091650066335189 * fTemp40):fTemp42)):((iSlow5)?((iSlow7)?fTemp38:fTemp43):((iSlow6)?fTemp44:((iSlow8)?(0.3779644730092272 * fTemp38):fTemp38))));
			fRec18[0] = max((fRec18[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fbargraph21 = fRec18[0];
			output9[i] = (FAUSTFLOAT)fTemp45;
			fTemp46 = (double)input10[i];
			fRec23[0] = max((fRec23[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fbargraph22 = fRec23[0];
			fTemp47 = fTemp46;
			fTemp48 = (double)input13[i];
			fRec24[0] = max((fRec24[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fbargraph23 = fRec24[0];
			fTemp49 = fTemp48;
			fTemp50 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp47:(0.7453559924999299 * fTemp49)):((iSlow4)?(1.9720265943665387 * fTemp49):(1.1858541225631423 * fTemp49))):((iSlow5)?((iSlow7)?fTemp47:(2.6457513110645907 * fTemp47)):((iSlow6)?(0.4482107285003976 * fTemp49):((iSlow8)?(0.3779644730092272 * fTemp47):fTemp47))));
			fRec22[0] = max((fRec22[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fbargraph24 = fRec22[0];
			output10[i] = (FAUSTFLOAT)fTemp50;
			fTemp51 = (double)input11[i];
			fRec26[0] = max((fRec26[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fbargraph25 = fRec26[0];
			fTemp52 = fTemp51;
			fTemp53 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp52:(0.8432740427115678 * fTemp52)):((iSlow4)?(2.2310934040908683 * fTemp52):(1.1858541225631423 * fTemp52))):((iSlow5)?((iSlow7)?fTemp52:(2.6457513110645907 * fTemp52)):((iSlow6)?(0.4482107285003976 * fTemp52):((iSlow8)?(0.3779644730092272 * fTemp52):fTemp52))));
			fRec25[0] = max((fRec25[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fbargraph26 = fRec25[0];
			output11[i] = (FAUSTFLOAT)fTemp53;
			fTemp54 = (double)input14[i];
			fRec28[0] = max((fRec28[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fbargraph27 = fRec28[0];
			fTemp55 = fTemp54;
			fTemp56 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp42:fTemp38):((iSlow4)?fTemp43:(1.3416407864998738 * fTemp55))):((iSlow5)?((iSlow7)?fTemp42:(2.6457513110645907 * fTemp42)):((iSlow6)?(0.50709255283711 * fTemp55):((iSlow8)?fTemp44:fTemp42))));
			fRec27[0] = max((fRec27[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fbargraph28 = fRec27[0];
			output12[i] = (FAUSTFLOAT)fTemp56;
			fTemp57 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp49:(0.8432740427115678 * fTemp47)):((iSlow4)?(2.2310934040908683 * fTemp47):(1.3416407864998738 * fTemp47))):((iSlow5)?((iSlow7)?fTemp49:(2.6457513110645907 * fTemp49)):((iSlow6)?(0.50709255283711 * fTemp47):((iSlow8)?(0.3779644730092272 * fTemp49):fTemp49))));
			fRec29[0] = max((fRec29[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fbargraph29 = fRec29[0];
			output13[i] = (FAUSTFLOAT)fTemp57;
			fTemp58 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp55:(0.7453559924999299 * fTemp42)):((iSlow4)?(1.9720265943665387 * fTemp42):(1.2649110640673518 * fTemp40))):((iSlow5)?((iSlow7)?fTemp55:(2.6457513110645907 * fTemp55)):((iSlow6)?(0.47809144373375745 * fTemp40):((iSlow8)?(0.3779644730092272 * fTemp55):fTemp55))));
			fRec30[0] = max((fRec30[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fbargraph30 = fRec30[0];
			output14[i] = (FAUSTFLOAT)fTemp58;
			fTemp59 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp40:(0.7905694150420948 * fTemp55)):((iSlow4)?(2.091650066335189 * fTemp55):(1.2649110640673518 * fTemp38))):((iSlow5)?((iSlow7)?fTemp40:(2.6457513110645907 * fTemp40)):((iSlow6)?(0.47809144373375745 * fTemp38):((iSlow8)?(0.3779644730092272 * fTemp40):fTemp40))));
			fRec31[0] = max((fRec31[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fbargraph31 = fRec31[0];
			output15[i] = (FAUSTFLOAT)fTemp59;
			fTemp60 = (double)input16[i];
			fRec33[0] = max((fRec33[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fbargraph32 = fRec33[0];
			fTemp61 = fTemp60;
			fTemp62 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp61:0.0):0):((iSlow5)?((iSlow7)?fTemp61:(3.0 * fTemp61)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp61):fTemp61))));
			fRec32[0] = max((fRec32[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fbargraph33 = fRec32[0];
			output16[i] = (FAUSTFLOAT)fTemp62;
			fTemp63 = (double)input17[i];
			fRec35[0] = max((fRec35[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fbargraph34 = fRec35[0];
			fTemp64 = fTemp63;
			fTemp65 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp64:0.0):0):((iSlow5)?((iSlow7)?fTemp64:(3.0 * fTemp64)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp64):fTemp64))));
			fRec34[0] = max((fRec34[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fbargraph35 = fRec34[0];
			output17[i] = (FAUSTFLOAT)fTemp65;
			fTemp66 = (double)input18[i];
			fRec37[0] = max((fRec37[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fbargraph36 = fRec37[0];
			fTemp67 = fTemp66;
			fTemp68 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp67:0.0):0):((iSlow5)?((iSlow7)?fTemp67:(3.0 * fTemp67)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp67):fTemp67))));
			fRec36[0] = max((fRec36[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fbargraph37 = fRec36[0];
			output18[i] = (FAUSTFLOAT)fTemp68;
			fTemp69 = (double)input19[i];
			fRec39[0] = max((fRec39[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp69))))));
			fbargraph38 = fRec39[0];
			fTemp70 = fTemp69;
			fTemp71 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp70:0.0):0):((iSlow5)?((iSlow7)?fTemp70:(3.0 * fTemp70)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp70):fTemp70))));
			fRec38[0] = max((fRec38[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fbargraph39 = fRec38[0];
			output19[i] = (FAUSTFLOAT)fTemp71;
			fTemp72 = (double)input20[i];
			fRec41[0] = max((fRec41[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp72))))));
			fbargraph40 = fRec41[0];
			fTemp73 = fTemp72;
			fTemp74 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp73:0.0):0):((iSlow5)?((iSlow7)?fTemp73:(3.0 * fTemp73)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp73):fTemp73))));
			fRec40[0] = max((fRec40[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp74))))));
			fbargraph41 = fRec40[0];
			output20[i] = (FAUSTFLOAT)fTemp74;
			fTemp75 = (double)input21[i];
			fRec43[0] = max((fRec43[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp75))))));
			fbargraph42 = fRec43[0];
			fTemp76 = fTemp75;
			fTemp77 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp76:0.0):0):((iSlow5)?((iSlow7)?fTemp76:(3.0 * fTemp76)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp76):fTemp76))));
			fRec42[0] = max((fRec42[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp77))))));
			fbargraph43 = fRec42[0];
			output21[i] = (FAUSTFLOAT)fTemp77;
			fTemp78 = (double)input22[i];
			fRec45[0] = max((fRec45[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fbargraph44 = fRec45[0];
			fTemp79 = fTemp78;
			fTemp80 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp79:0.0):0):((iSlow5)?((iSlow7)?fTemp79:(3.0 * fTemp79)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp79):fTemp79))));
			fRec44[0] = max((fRec44[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp80))))));
			fbargraph45 = fRec44[0];
			output22[i] = (FAUSTFLOAT)fTemp80;
			fTemp81 = (double)input23[i];
			fRec47[0] = max((fRec47[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp81))))));
			fbargraph46 = fRec47[0];
			fTemp82 = fTemp81;
			fTemp83 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp82:0.0):0):((iSlow5)?((iSlow7)?fTemp82:(3.0 * fTemp82)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp82):fTemp82))));
			fRec46[0] = max((fRec46[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp83))))));
			fbargraph47 = fRec46[0];
			output23[i] = (FAUSTFLOAT)fTemp83;
			fTemp84 = (double)input24[i];
			fRec49[0] = max((fRec49[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp84))))));
			fbargraph48 = fRec49[0];
			fTemp85 = fTemp84;
			fTemp86 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp85:0.0):0):((iSlow5)?((iSlow7)?fTemp85:(3.0 * fTemp85)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp85):fTemp85))));
			fRec48[0] = max((fRec48[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp86))))));
			fbargraph49 = fRec48[0];
			output24[i] = (FAUSTFLOAT)fTemp86;
			// post processing
			fRec48[1] = fRec48[0];
			fRec49[1] = fRec49[0];
			fRec46[1] = fRec46[0];
			fRec47[1] = fRec47[0];
			fRec44[1] = fRec44[0];
			fRec45[1] = fRec45[0];
			fRec42[1] = fRec42[0];
			fRec43[1] = fRec43[0];
			fRec40[1] = fRec40[0];
			fRec41[1] = fRec41[0];
			fRec38[1] = fRec38[0];
			fRec39[1] = fRec39[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			fRec32[1] = fRec32[0];
			fRec33[1] = fRec33[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec29[1] = fRec29[0];
			fRec27[1] = fRec27[0];
			fRec28[1] = fRec28[0];
			fRec25[1] = fRec25[0];
			fRec26[1] = fRec26[0];
			fRec22[1] = fRec22[0];
			fRec24[1] = fRec24[0];
			fRec23[1] = fRec23[0];
			fRec18[1] = fRec18[0];
			fRec21[1] = fRec21[0];
			fRec20[1] = fRec20[0];
			fRec19[1] = fRec19[0];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			fRec15[1] = fRec15[0];
			fRec12[1] = fRec12[0];
			fRec14[1] = fRec14[0];
			fRec13[1] = fRec13[0];
			fRec8[1] = fRec8[0];
			fRec11[1] = fRec11[0];
			fRec10[1] = fRec10[0];
			fRec9[1] = fRec9[0];
			fRec7[1] = fRec7[0];
			fRec6[1] = fRec6[0];
			fRec2[1] = fRec2[0];
			fRec5[1] = fRec5[0];
			fRec4[1] = fRec4[0];
			fRec3[1] = fRec3[0];
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
