//----------------------------------------------------------
// name: "HOADecLebedev501"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2014"
//
// Code generated with Faust 0.9.95 (http://faust.grame.fr)
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
struct Meta;

/**
* Signal processor definition.
*/

class dsp {

    public:

        dsp() {}
        virtual ~dsp() {}

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the UI* parameter with instance specific calls
         * to 'addBtton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the UI* user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Returns the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /** Global init, calls the following methods :
         * - static class 'classInit' : static table initialisation
         * - 'instanceInit' : constants and instance table initialisation
         *
         * @param samplingRate - the sampling rate in Herz
         */
        virtual void init(int samplingRate) = 0;
    
        /** Init instance state
         *
         * @param samplingRate - the sampling rate in Herz
         */
        virtual void instanceInit(int samplingRate) = 0;
    
        /** Init instance constant state
         *
         * @param samplingRate - the sampling rate in Herz
         */
        virtual void instanceConstants(int samplingRate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (delay lines...) */
        virtual void instanceClear() = 0;
    
        /**  
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual dsp* clone() = 0;
    
        /**
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value metadata).
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with sucessive in/out audio buffers.
         *
         * @param count - the nomber of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, doucbe or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, doucbe or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation : alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the nomber of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, doucbe or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, doucbe or quad)
         *
         */
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

/**
 * Generic DSP decorator.
 */

class decorator_dsp : public dsp {

    protected:

        dsp* fDSP;

    public:

        decorator_dsp(dsp* dsp = 0):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) { fDSP->buildUserInterface(ui_interface); }
        virtual int getSampleRate() { return fDSP->getSampleRate(); }
        virtual void init(int samplingRate) { fDSP->init(samplingRate); }
        virtual void instanceInit(int samplingRate) { fDSP->instanceInit(samplingRate); }
        virtual void instanceConstants(int samplingRate) { fDSP->instanceConstants(samplingRate); }
        virtual void instanceResetUserInterface() { fDSP->instanceResetUserInterface(); }
        virtual void instanceClear() { fDSP->instanceClear(); }
        virtual decorator_dsp* clone() { return new decorator_dsp(fDSP->clone()); }
        virtual void metadata(Meta* m) { return fDSP->metadata(m); }
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
       
};

/**
 * On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
 * flags to avoid costly denormals.
 */

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

inline int int2pow2(int x)		{ int r = 0; while ((1<<r) < x) r++; return r; }

inline long lopt(char* argv[], const char* name, long def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return atoi(argv[i+1]);
	return def;
}

inline bool isopt(char* argv[], const char* name)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return true;
	return false;
}

inline const char* lopts(char* argv[], const char* name, const char* def)
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
	double 	fConst0;
	double 	fConst1;
	FAUSTFLOAT 	fslider0;
	double 	fRec1[2];
	FAUSTFLOAT 	fcheckbox0;
	FAUSTFLOAT 	fentry0;
	FAUSTFLOAT 	fslider1;
	double 	fRec3[2];
	double 	fRec2[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fConst2;
	double 	fConst3;
	double 	fRec6[2];
	double 	fRec5[2];
	double 	fRec4[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec9[2];
	double 	fRec8[2];
	double 	fRec7[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fRec10[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec14[2];
	double 	fRec13[2];
	double 	fRec12[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec11[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec15[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec16[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec17[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec18[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec19[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec20[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec21[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec22[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec23[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec24[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec25[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec26[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec27[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec28[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec29[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec30[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec31[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec32[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec33[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec34[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec35[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec36[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec37[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec38[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec39[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec40[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec41[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec42[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec43[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec44[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec45[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec47[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec48[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec49[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec50[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec51[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec52[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec53[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec54[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec55[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec56[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec57[2];
	FAUSTFLOAT 	fbargraph49;
	double 	fRec58[2];
	FAUSTFLOAT 	fbargraph50;
	double 	fRec59[2];
	FAUSTFLOAT 	fbargraph51;
	double 	fRec60[2];
	FAUSTFLOAT 	fbargraph52;
	double 	fRec61[2];
	FAUSTFLOAT 	fbargraph53;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOADecLebedev501");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/ymn.lib/name", "Spherical Harmonics library");
		m->declare("lib/ymn.lib/version", "1.0");
		m->declare("lib/ymn.lib/author", "Pierre Lecomte");
		m->declare("lib/ymn.lib/license", "GPL");
		m->declare("lib/ymn.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/nfc.lib/name", "NF And NFC Filters Library");
		m->declare("lib/nfc.lib/version", "1.0");
		m->declare("lib/nfc.lib/author", "Pierre Lecomte");
		m->declare("lib/nfc.lib/license", "GPL");
		m->declare("lib/nfc.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/lebedev.lib/name", "Lebdev grids and weights");
		m->declare("lib/lebedev.lib/version", "1.0");
		m->declare("lib/lebedev.lib/author", "Pierre Lecomte");
		m->declare("lib/lebedev.lib/license", "GPL");
		m->declare("lib/lebedev.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("signal.lib/name", "Faust Signal Routing Library");
		m->declare("signal.lib/version", "0.0");
		m->declare("basic.lib/name", "Faust Basic Element Library");
		m->declare("basic.lib/version", "0.0");
		m->declare("math.lib/name", "Faust Math Library");
		m->declare("math.lib/version", "2.0");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/license", "LGPL with exception");
	}

	virtual int getNumInputs() { return 4; }
	virtual int getNumOutputs() { return 50; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(1.92e+05, max(1.0, (double)fSamplingFreq));
		fConst1 = (8e+01 / fConst0);
		fConst2 = (1.7e+02 / fConst0);
		fConst3 = (3.4e+02 / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.0;
		fcheckbox0 = 0.0;
		fentry0 = 1.07;
		fslider1 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
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
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("B-Format");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348beb40", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348c94b0", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348c1d80", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348c45e0", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(&fslider1, "1", "");
		ui_interface->declare(&fslider1, "osc", "/levelin -10 60");
		ui_interface->declare(&fslider1, "unit", "dB");
		ui_interface->addVerticalSlider("Inputs Gain", &fslider1, 0.0, -1e+01, 6e+01, 0.1);
		ui_interface->declare(&fslider0, "2", "");
		ui_interface->declare(&fslider0, "osc", "/levelout -10 60");
		ui_interface->declare(&fslider0, "unit", "dB");
		ui_interface->addVerticalSlider("Outputs Gain", &fslider0, 0.0, -1e+01, 6e+01, 0.1);
		ui_interface->declare(0, "3", "");
		ui_interface->openVerticalBox("NFC");
		ui_interface->addCheckButton("Yes", &fcheckbox0);
		ui_interface->closeBox();
		ui_interface->declare(&fentry0, "4", "");
		ui_interface->addNumEntry("Speakers Radius", &fentry0, 1.07, 0.5, 1e+01, 0.01);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 1-25");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph3, "osc", "/output1");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348c5c50", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph4, "osc", "/output2");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348c6c90", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph6, "osc", "/output3");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348ca980", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph7, "osc", "/output4");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348cc2f0", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph8, "osc", "/output5");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348cdb70", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph9, "osc", "/output6");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348cf3f0", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph10, "osc", "/output7");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348d1260", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph11, "osc", "/output8");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348d2ea0", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph12, "osc", "/output9");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348d4aa0", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph13, "osc", "/output10");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348d6760", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph14, "osc", "/output11");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348d8230", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph15, "osc", "/output12");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348d9e40", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph16, "osc", "/output13");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348db7f0", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph17, "osc", "/output14");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348dd370", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph18, "osc", "/output15");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348def50", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph19, "osc", "/output16");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348e0c90", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph20, "osc", "/output17");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348e2950", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph21, "osc", "/output18");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348e4770", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph22, "osc", "/output19");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348e6b80", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph23, "osc", "/output20");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348e8c10", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph24, "osc", "/output21");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348eacb0", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph25, "osc", "/output22");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348ecc70", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph26, "osc", "/output23");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348eec90", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph27, "osc", "/output24");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348f0c70", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph28, "osc", "/output25");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348f3010", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph29, "osc", "/output26");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348f52b0", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph30, "osc", "/output27");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348f6f90", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph31, "osc", "/output28");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348f8890", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph32, "osc", "/output29");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348fa2f0", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph33, "osc", "/output30");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa5348fbe80", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph34, "osc", "/output31");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa534a01c80", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph35, "osc", "/output32");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa534a03930", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph36, "osc", "/output33");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa534a05580", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph37, "osc", "/output34");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa534a071b0", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph38, "osc", "/output35");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa534a08eb0", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fbargraph39, "osc", "/output36");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa534a0ab10", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fbargraph40, "osc", "/output37");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa534a0c8d0", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fbargraph41, "osc", "/output38");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa534a0e960", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fbargraph42, "osc", "/output39");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa534a105a0", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fbargraph43, "osc", "/output40");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa534a122e0", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fbargraph44, "osc", "/output41");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa534a140e0", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fbargraph45, "osc", "/output42");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa531caef60", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fbargraph46, "osc", "/output43");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa531cb0d00", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fbargraph47, "osc", "/output44");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa531cb2c00", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fbargraph48, "osc", "/output45");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa531cb4a80", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fbargraph49, "osc", "/output46");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa531cb6ab0", &fbargraph49, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fbargraph50, "osc", "/output47");
		ui_interface->declare(&fbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa533dda8a0", &fbargraph50, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fbargraph51, "osc", "/output48");
		ui_interface->declare(&fbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa533ddc7c0", &fbargraph51, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fbargraph52, "osc", "/output49");
		ui_interface->declare(&fbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa533dde8f0", &fbargraph52, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fbargraph53, "osc", "/output50");
		ui_interface->declare(&fbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa533de0ae0", &fbargraph53, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider0))));
		double 	fSlow1 = double(fcheckbox0);
		double 	fSlow2 = double(fentry0);
		double 	fSlow3 = (1 - (fSlow1 * (1 - fSlow2)));
		double 	fSlow4 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider1))));
		double 	fSlow5 = (1 - fSlow1);
		double 	fSlow6 = (1.0 / ((fConst2 / fSlow2) + 1));
		double 	fSlow7 = (fSlow1 * fSlow2);
		double 	fSlow8 = (fConst3 / fSlow2);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* input2 = input[2];
		FAUSTFLOAT* input3 = input[3];
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
		FAUSTFLOAT* output25 = output[25];
		FAUSTFLOAT* output26 = output[26];
		FAUSTFLOAT* output27 = output[27];
		FAUSTFLOAT* output28 = output[28];
		FAUSTFLOAT* output29 = output[29];
		FAUSTFLOAT* output30 = output[30];
		FAUSTFLOAT* output31 = output[31];
		FAUSTFLOAT* output32 = output[32];
		FAUSTFLOAT* output33 = output[33];
		FAUSTFLOAT* output34 = output[34];
		FAUSTFLOAT* output35 = output[35];
		FAUSTFLOAT* output36 = output[36];
		FAUSTFLOAT* output37 = output[37];
		FAUSTFLOAT* output38 = output[38];
		FAUSTFLOAT* output39 = output[39];
		FAUSTFLOAT* output40 = output[40];
		FAUSTFLOAT* output41 = output[41];
		FAUSTFLOAT* output42 = output[42];
		FAUSTFLOAT* output43 = output[43];
		FAUSTFLOAT* output44 = output[44];
		FAUSTFLOAT* output45 = output[45];
		FAUSTFLOAT* output46 = output[46];
		FAUSTFLOAT* output47 = output[47];
		FAUSTFLOAT* output48 = output[48];
		FAUSTFLOAT* output49 = output[49];
		for (int i=0; i<count; i++) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			fRec3[0] = (fSlow4 + (0.999 * fRec3[1]));
			double fTemp0 = (fSlow3 * ((double)input0[i] * fRec3[0]));
			fRec2[0] = max((fRec2[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fbargraph0 = fRec2[0];
			double fTemp1 = fTemp0;
			double fTemp2 = (0.012698412698412698 * fTemp1);
			double fTemp3 = ((double)input2[i] * fRec3[0]);
			fRec6[0] = (fRec5[1] + fRec6[1]);
			fRec5[0] = (fSlow6 * ((fSlow7 * fTemp3) - (fSlow8 * fRec6[0])));
			double fTemp4 = ((fSlow5 * fTemp3) + fRec5[0]);
			fRec4[0] = max((fRec4[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fbargraph1 = fRec4[0];
			double fTemp5 = fTemp4;
			double fTemp6 = (0.0219942959691286 * fTemp5);
			double fTemp7 = ((double)input3[i] * fRec3[0]);
			fRec9[0] = (fRec8[1] + fRec9[1]);
			fRec8[0] = (fSlow6 * ((fSlow7 * fTemp7) - (fSlow8 * fRec9[0])));
			double fTemp8 = (fRec8[0] + (fSlow5 * fTemp7));
			fRec7[0] = max((fRec7[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fbargraph2 = fRec7[0];
			double fTemp9 = fTemp8;
			double fTemp10 = (1.3467622079046435e-18 * fTemp9);
			double fTemp11 = (fRec1[0] * (fTemp2 + (fTemp6 + fTemp10)));
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fbargraph3 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp11;
			double fTemp12 = (0.0219942959691286 * fTemp9);
			double fTemp13 = (fRec1[0] * (fTemp12 + fTemp2));
			fRec10[0] = max((fRec10[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fbargraph4 = fRec10[0];
			output1[i] = (FAUSTFLOAT)fTemp13;
			double fTemp14 = ((double)input1[i] * fRec3[0]);
			fRec14[0] = (fRec13[1] + fRec14[1]);
			fRec13[0] = (fSlow6 * ((fSlow7 * fTemp14) - (fSlow8 * fRec14[0])));
			double fTemp15 = (fRec13[0] + (fSlow5 * fTemp14));
			fRec12[0] = max((fRec12[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fbargraph5 = fRec12[0];
			double fTemp16 = fTemp15;
			double fTemp17 = (0.0219942959691286 * fTemp16);
			double fTemp18 = (fRec1[0] * (fTemp2 + (fTemp10 + fTemp17)));
			fRec11[0] = max((fRec11[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fbargraph6 = fRec11[0];
			output2[i] = (FAUSTFLOAT)fTemp18;
			double fTemp19 = (fRec1[0] * (((2.693524415809287e-18 * fTemp16) + fTemp2) - fTemp12));
			fRec15[0] = max((fRec15[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fbargraph7 = fRec15[0];
			output3[i] = (FAUSTFLOAT)fTemp19;
			double fTemp20 = (fRec1[0] * (fTemp2 - (fTemp17 + (4.040286623713931e-18 * fTemp9))));
			fRec16[0] = max((fRec16[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph8 = fRec16[0];
			output4[i] = (FAUSTFLOAT)fTemp20;
			double fTemp21 = (fRec1[0] * ((fTemp10 + fTemp2) - fTemp6));
			fRec17[0] = max((fRec17[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fbargraph9 = fRec17[0];
			output5[i] = (FAUSTFLOAT)fTemp21;
			double fTemp22 = (0.0276485614705685 * fTemp5);
			double fTemp23 = (0.027648561470568506 * fTemp9);
			double fTemp24 = (fTemp22 + fTemp23);
			double fTemp25 = (0.022574955908289243 * fTemp1);
			double fTemp26 = (fRec1[0] * (fTemp24 + fTemp25));
			fRec18[0] = max((fRec18[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fbargraph10 = fRec18[0];
			output6[i] = (FAUSTFLOAT)fTemp26;
			double fTemp27 = (0.027648561470568506 * fTemp16);
			double fTemp28 = (1.6929861152980278e-18 * fTemp9);
			double fTemp29 = (fRec1[0] * (fTemp25 + (fTemp27 + (fTemp22 + fTemp28))));
			fRec19[0] = max((fRec19[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph11 = fRec19[0];
			output7[i] = (FAUSTFLOAT)fTemp29;
			double fTemp30 = (3.3859722305960555e-18 * fTemp16);
			double fTemp31 = (fRec1[0] * ((fTemp25 + (fTemp22 + fTemp30)) - fTemp23));
			fRec20[0] = max((fRec20[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fbargraph12 = fRec20[0];
			output8[i] = (FAUSTFLOAT)fTemp31;
			double fTemp32 = (5.078958345894083e-18 * fTemp9);
			double fTemp33 = (fRec1[0] * ((fTemp22 + fTemp25) - (fTemp27 + fTemp32)));
			fRec21[0] = max((fRec21[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph13 = fRec21[0];
			output9[i] = (FAUSTFLOAT)fTemp33;
			double fTemp34 = (fTemp23 + (0.0276485614705685 * fTemp16));
			double fTemp35 = (fRec1[0] * (fTemp34 + fTemp25));
			fRec22[0] = max((fRec22[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fbargraph14 = fRec22[0];
			output10[i] = (FAUSTFLOAT)fTemp35;
			double fTemp36 = (0.0276485614705685 * fTemp9);
			double fTemp37 = (fRec1[0] * ((fTemp27 + fTemp25) - fTemp36));
			fRec23[0] = max((fRec23[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fbargraph15 = fRec23[0];
			output11[i] = (FAUSTFLOAT)fTemp37;
			double fTemp38 = (fRec1[0] * (fTemp25 - fTemp34));
			fRec24[0] = max((fRec24[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fbargraph16 = fRec24[0];
			output12[i] = (FAUSTFLOAT)fTemp38;
			double fTemp39 = (fRec1[0] * ((fTemp36 + fTemp25) - fTemp27));
			fRec25[0] = max((fRec25[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fbargraph17 = fRec25[0];
			output13[i] = (FAUSTFLOAT)fTemp39;
			double fTemp40 = (fRec1[0] * ((fTemp23 + fTemp25) - fTemp22));
			fRec26[0] = max((fRec26[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fbargraph18 = fRec26[0];
			output14[i] = (FAUSTFLOAT)fTemp40;
			double fTemp41 = (fRec1[0] * ((fTemp25 + (fTemp27 + fTemp28)) - fTemp22));
			fRec27[0] = max((fRec27[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fbargraph19 = fRec27[0];
			output15[i] = (FAUSTFLOAT)fTemp41;
			double fTemp42 = (fRec1[0] * ((fTemp30 + fTemp25) - fTemp24));
			fRec28[0] = max((fRec28[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fbargraph20 = fRec28[0];
			output16[i] = (FAUSTFLOAT)fTemp42;
			double fTemp43 = (fRec1[0] * (fTemp25 - (fTemp27 + (fTemp22 + fTemp32))));
			fRec29[0] = max((fRec29[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fbargraph21 = fRec29[0];
			output17[i] = (FAUSTFLOAT)fTemp43;
			double fTemp44 = (0.021093749999999994 * fTemp16);
			double fTemp45 = (fTemp44 + (0.021093749999999998 * fTemp9));
			double fTemp46 = (fTemp5 + fTemp1);
			double fTemp47 = (0.02109375 * fTemp46);
			double fTemp48 = (fRec1[0] * (fTemp45 + fTemp47));
			fRec30[0] = max((fRec30[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fbargraph22 = fRec30[0];
			output18[i] = (FAUSTFLOAT)fTemp48;
			double fTemp49 = (0.021093749999999998 * fTemp16);
			double fTemp50 = (0.021093749999999994 * fTemp9);
			double fTemp51 = (fRec1[0] * ((fTemp49 + fTemp47) - fTemp50));
			fRec31[0] = max((fRec31[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fbargraph23 = fRec31[0];
			output19[i] = (FAUSTFLOAT)fTemp51;
			double fTemp52 = (fRec1[0] * (0 - (fTemp44 + (0.02109375 * (fTemp9 - fTemp46)))));
			fRec32[0] = max((fRec32[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fbargraph24 = fRec32[0];
			output20[i] = (FAUSTFLOAT)fTemp52;
			double fTemp53 = (fRec1[0] * (fTemp50 + (0.02109375 * (fTemp46 - fTemp16))));
			fRec33[0] = max((fRec33[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fbargraph25 = fRec33[0];
			output21[i] = (FAUSTFLOAT)fTemp53;
			double fTemp54 = (0.02109375 * (fTemp1 - fTemp5));
			double fTemp55 = (fRec1[0] * (fTemp45 + fTemp54));
			fRec34[0] = max((fRec34[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fbargraph26 = fRec34[0];
			output22[i] = (FAUSTFLOAT)fTemp55;
			double fTemp56 = (fRec1[0] * ((fTemp49 + fTemp54) - fTemp50));
			fRec35[0] = max((fRec35[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fbargraph27 = fRec35[0];
			output23[i] = (FAUSTFLOAT)fTemp56;
			double fTemp57 = (fRec1[0] * (0 - (fTemp44 + (0.02109375 * ((fTemp5 + fTemp9) - fTemp1)))));
			fRec36[0] = max((fRec36[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fbargraph28 = fRec36[0];
			output24[i] = (FAUSTFLOAT)fTemp57;
			double fTemp58 = (fRec1[0] * (fTemp50 - (0.02109375 * ((fTemp5 + fTemp16) - fTemp1))));
			fRec37[0] = max((fRec37[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fbargraph29 = fRec37[0];
			output25[i] = (FAUSTFLOAT)fTemp58;
			double fTemp59 = (0.02017333553791887 * fTemp1);
			double fTemp60 = (0.03160554266923825 * fTemp5);
			double fTemp61 = (0.010535180889746065 * (fTemp9 + fTemp16));
			double fTemp62 = (fRec1[0] * (fTemp59 + (fTemp60 + fTemp61)));
			fRec38[0] = max((fRec38[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fbargraph30 = fRec38[0];
			output26[i] = (FAUSTFLOAT)fTemp62;
			double fTemp63 = (fTemp60 + fTemp59);
			double fTemp64 = (0.010535180889746065 * (fTemp9 - fTemp16));
			double fTemp65 = (fRec1[0] * (fTemp63 - fTemp64));
			fRec39[0] = max((fRec39[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fbargraph31 = fRec39[0];
			output27[i] = (FAUSTFLOAT)fTemp65;
			double fTemp66 = (0.010535180889746065 * fTemp16);
			double fTemp67 = (0.010535180889746067 * fTemp9);
			double fTemp68 = (fRec1[0] * (fTemp63 - (fTemp66 + fTemp67)));
			fRec40[0] = max((fRec40[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fbargraph32 = fRec40[0];
			output28[i] = (FAUSTFLOAT)fTemp68;
			double fTemp69 = (0.010535180889746065 * fTemp9);
			double fTemp70 = (0.010535180889746067 * fTemp16);
			double fTemp71 = (fRec1[0] * ((fTemp59 + (fTemp60 + fTemp69)) - fTemp70));
			fRec41[0] = max((fRec41[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fbargraph33 = fRec41[0];
			output29[i] = (FAUSTFLOAT)fTemp71;
			double fTemp72 = (0.010535180889746094 * fTemp16);
			double fTemp73 = (0.010535180889746075 * fTemp5);
			double fTemp74 = (0.03160554266923824 * fTemp9);
			double fTemp75 = (fTemp73 + fTemp74);
			double fTemp76 = (fRec1[0] * (fTemp59 + (fTemp72 + fTemp75)));
			fRec42[0] = max((fRec42[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp76))))));
			fbargraph34 = fRec42[0];
			output30[i] = (FAUSTFLOAT)fTemp76;
			double fTemp77 = (0.03160554266923824 * fTemp16);
			double fTemp78 = (0.01053518088974608 * fTemp9);
			double fTemp79 = (fRec1[0] * (fTemp59 + (fTemp77 + (fTemp73 + fTemp78))));
			fRec43[0] = max((fRec43[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp79))))));
			fbargraph35 = fRec43[0];
			output31[i] = (FAUSTFLOAT)fTemp79;
			double fTemp80 = (fTemp73 + fTemp77);
			double fTemp81 = (0.010535180889746079 * fTemp9);
			double fTemp82 = (fRec1[0] * ((fTemp80 + fTemp59) - fTemp81));
			fRec44[0] = max((fRec44[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp82))))));
			fbargraph36 = fRec44[0];
			output32[i] = (FAUSTFLOAT)fTemp82;
			double fTemp83 = (0.010535180889746096 * fTemp16);
			double fTemp84 = (fRec1[0] * ((fTemp59 + (fTemp73 + fTemp83)) - fTemp74));
			fRec45[0] = max((fRec45[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp84))))));
			fbargraph37 = fRec45[0];
			output33[i] = (FAUSTFLOAT)fTemp84;
			double fTemp85 = (fTemp73 + fTemp59);
			double fTemp86 = (0.01053518088974609 * fTemp16);
			double fTemp87 = (fRec1[0] * (fTemp85 - (fTemp74 + fTemp86)));
			fRec46[0] = max((fRec46[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp87))))));
			fbargraph38 = fRec46[0];
			output34[i] = (FAUSTFLOAT)fTemp87;
			double fTemp88 = (0.010535180889746086 * fTemp9);
			double fTemp89 = (fRec1[0] * (fTemp85 - (fTemp77 + fTemp88)));
			fRec47[0] = max((fRec47[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp89))))));
			fbargraph39 = fRec47[0];
			output35[i] = (FAUSTFLOAT)fTemp89;
			double fTemp90 = (0.010535180889746074 * fTemp9);
			double fTemp91 = (fRec1[0] * ((fTemp59 + (fTemp73 + fTemp90)) - fTemp77));
			fRec48[0] = max((fRec48[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp91))))));
			fbargraph40 = fRec48[0];
			output36[i] = (FAUSTFLOAT)fTemp91;
			double fTemp92 = (0.03160554266923823 * fTemp9);
			double fTemp93 = (0.010535180889746113 * fTemp16);
			double fTemp94 = (fRec1[0] * ((fTemp59 + (fTemp73 + fTemp92)) - fTemp93));
			fRec49[0] = max((fRec49[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp94))))));
			fbargraph41 = fRec49[0];
			output37[i] = (FAUSTFLOAT)fTemp94;
			double fTemp95 = (fRec1[0] * ((fTemp59 + (fTemp72 + fTemp74)) - fTemp73));
			fRec50[0] = max((fRec50[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp95))))));
			fbargraph42 = fRec50[0];
			output38[i] = (FAUSTFLOAT)fTemp95;
			double fTemp96 = (fRec1[0] * ((fTemp59 + (fTemp77 + fTemp78)) - fTemp73));
			fRec51[0] = max((fRec51[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp96))))));
			fbargraph43 = fRec51[0];
			output39[i] = (FAUSTFLOAT)fTemp96;
			double fTemp97 = (fRec1[0] * ((fTemp77 + fTemp59) - (fTemp73 + fTemp81)));
			fRec52[0] = max((fRec52[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp97))))));
			fbargraph44 = fRec52[0];
			output40[i] = (FAUSTFLOAT)fTemp97;
			double fTemp98 = (fRec1[0] * ((fTemp83 + fTemp59) - fTemp75));
			fRec53[0] = max((fRec53[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp98))))));
			fbargraph45 = fRec53[0];
			output41[i] = (FAUSTFLOAT)fTemp98;
			double fTemp99 = (fRec1[0] * (fTemp59 - (fTemp86 + fTemp75)));
			fRec54[0] = max((fRec54[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp99))))));
			fbargraph46 = fRec54[0];
			output42[i] = (FAUSTFLOAT)fTemp99;
			double fTemp100 = (fRec1[0] * (fTemp59 - (fTemp77 + (fTemp73 + fTemp88))));
			fRec55[0] = max((fRec55[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp100))))));
			fbargraph47 = fRec55[0];
			output43[i] = (FAUSTFLOAT)fTemp100;
			double fTemp101 = (fRec1[0] * ((fTemp90 + fTemp59) - fTemp80));
			fRec56[0] = max((fRec56[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp101))))));
			fbargraph48 = fRec56[0];
			output44[i] = (FAUSTFLOAT)fTemp101;
			double fTemp102 = (fRec1[0] * ((fTemp92 + fTemp59) - (fTemp73 + fTemp93)));
			fRec57[0] = max((fRec57[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp102))))));
			fbargraph49 = fRec57[0];
			output45[i] = (FAUSTFLOAT)fTemp102;
			double fTemp103 = (fRec1[0] * ((fTemp61 + fTemp59) - fTemp60));
			fRec58[0] = max((fRec58[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp103))))));
			fbargraph50 = fRec58[0];
			output46[i] = (FAUSTFLOAT)fTemp103;
			double fTemp104 = (fRec1[0] * (fTemp59 - (fTemp60 + fTemp64)));
			fRec59[0] = max((fRec59[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp104))))));
			fbargraph51 = fRec59[0];
			output47[i] = (FAUSTFLOAT)fTemp104;
			double fTemp105 = (fRec1[0] * (fTemp59 - (fTemp66 + (fTemp60 + fTemp67))));
			fRec60[0] = max((fRec60[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp105))))));
			fbargraph52 = fRec60[0];
			output48[i] = (FAUSTFLOAT)fTemp105;
			double fTemp106 = (fRec1[0] * ((fTemp69 + fTemp59) - (fTemp60 + fTemp70)));
			fRec61[0] = max((fRec61[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp106))))));
			fbargraph53 = fRec61[0];
			output49[i] = (FAUSTFLOAT)fTemp106;
			// post processing
			fRec61[1] = fRec61[0];
			fRec60[1] = fRec60[0];
			fRec59[1] = fRec59[0];
			fRec58[1] = fRec58[0];
			fRec57[1] = fRec57[0];
			fRec56[1] = fRec56[0];
			fRec55[1] = fRec55[0];
			fRec54[1] = fRec54[0];
			fRec53[1] = fRec53[0];
			fRec52[1] = fRec52[0];
			fRec51[1] = fRec51[0];
			fRec50[1] = fRec50[0];
			fRec49[1] = fRec49[0];
			fRec48[1] = fRec48[0];
			fRec47[1] = fRec47[0];
			fRec46[1] = fRec46[0];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec43[1] = fRec43[0];
			fRec42[1] = fRec42[0];
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fRec39[1] = fRec39[0];
			fRec38[1] = fRec38[0];
			fRec37[1] = fRec37[0];
			fRec36[1] = fRec36[0];
			fRec35[1] = fRec35[0];
			fRec34[1] = fRec34[0];
			fRec33[1] = fRec33[0];
			fRec32[1] = fRec32[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
			fRec27[1] = fRec27[0];
			fRec26[1] = fRec26[0];
			fRec25[1] = fRec25[0];
			fRec24[1] = fRec24[0];
			fRec23[1] = fRec23[0];
			fRec22[1] = fRec22[0];
			fRec21[1] = fRec21[0];
			fRec20[1] = fRec20[0];
			fRec19[1] = fRec19[0];
			fRec18[1] = fRec18[0];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			fRec15[1] = fRec15[0];
			fRec11[1] = fRec11[0];
			fRec12[1] = fRec12[0];
			fRec13[1] = fRec13[0];
			fRec14[1] = fRec14[0];
			fRec10[1] = fRec10[0];
			fRec0[1] = fRec0[0];
			fRec7[1] = fRec7[0];
			fRec8[1] = fRec8[0];
			fRec9[1] = fRec9[0];
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec6[1] = fRec6[0];
			fRec2[1] = fRec2[0];
			fRec3[1] = fRec3[0];
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
    mydsp tmp_dsp;
    tmp_dsp.metadata(&meta);

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

    if (strncmp(name.c_str(),SC_FAUST_PREFIX,strlen(SC_FAUST_PREFIX))!=0) {
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
