//----------------------------------------------------------
// name: "HOAPanLebedev501"
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
	FAUSTFLOAT 	fentry0;
	FAUSTFLOAT 	fcheckbox0;
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fcheckbox1;
	FAUSTFLOAT 	fslider1;
	double 	fRec1[2];
	FAUSTFLOAT 	fslider2;
	FAUSTFLOAT 	fcheckbox2;
	FAUSTFLOAT 	fslider3;
	double 	fRec2[2];
	FAUSTFLOAT 	fcheckbox3;
	FAUSTFLOAT 	fslider4;
	FAUSTFLOAT 	fslider5;
	double 	fConst2;
	double 	fConst3;
	double 	fRec5[2];
	double 	fRec4[2];
	double 	fRec3[2];
	double 	fRec7[2];
	double 	fRec6[2];
	FAUSTFLOAT 	fslider6;
	FAUSTFLOAT 	fslider7;
	double 	fRec10[2];
	double 	fRec9[2];
	double 	fRec8[2];
	double 	fRec12[2];
	double 	fRec11[2];
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fRec13[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec14[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec15[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fRec16[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec17[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec18[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec19[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec20[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec21[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec22[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec23[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec24[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec25[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec26[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec27[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec28[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec29[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec30[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec31[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec32[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec33[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec34[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec35[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec36[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec37[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec38[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec39[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec40[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec41[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec42[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec43[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec44[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec45[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec47[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec48[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec49[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec50[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec51[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec52[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec53[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec54[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec55[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec56[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec57[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec58[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec59[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec60[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec61[2];
	FAUSTFLOAT 	fbargraph49;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOAPanLebedev501");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/nfc.lib/name", "NF And NFC Filters Library");
		m->declare("lib/nfc.lib/version", "1.0");
		m->declare("lib/nfc.lib/author", "Pierre Lecomte");
		m->declare("lib/nfc.lib/license", "GPL");
		m->declare("lib/nfc.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/ymn.lib/name", "Spherical Harmonics library");
		m->declare("lib/ymn.lib/version", "1.0");
		m->declare("lib/ymn.lib/author", "Pierre Lecomte");
		m->declare("lib/ymn.lib/license", "GPL");
		m->declare("lib/ymn.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("math.lib/name", "Faust Math Library");
		m->declare("math.lib/version", "2.0");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/license", "LGPL with exception");
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
	}

	virtual int getNumInputs() { return 2; }
	virtual int getNumOutputs() { return 50; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(1.92e+05, max(1.0, (double)fSamplingFreq));
		fConst1 = (8e+01 / fConst0);
		fConst2 = (3.4e+02 / fConst0);
		fConst3 = (1.7e+02 / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fentry0 = 1.07;
		fcheckbox0 = 0.0;
		fslider0 = 2.0;
		fcheckbox1 = 0.0;
		fslider1 = 0.0;
		fslider2 = 2.0;
		fcheckbox2 = 0.0;
		fslider3 = 0.0;
		fcheckbox3 = 0.0;
		fslider4 = 0.0;
		fslider5 = 0.0;
		fslider6 = 0.0;
		fslider7 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
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
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fslider3, "0+1", "");
		ui_interface->declare(&fslider3, "osc", "/gain_0 -20 20");
		ui_interface->declare(&fslider3, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  0", &fslider3, 0.0, -3e+01, 2e+01, 0.1);
		ui_interface->declare(&fslider2, "0+2", "");
		ui_interface->declare(&fslider2, "osc", "/radius_0 0.5 50");
		ui_interface->declare(&fslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  0", &fslider2, 2.0, 0.5, 5e+01, 0.01);
		ui_interface->declare(&fslider5, "0+3", "");
		ui_interface->declare(&fslider5, "osc", "/azimuth_0 0 360");
		ui_interface->declare(&fslider5, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth  0", &fslider5, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(&fslider4, "0+4", "");
		ui_interface->declare(&fslider4, "osc", "/elevation_0 -90 90");
		ui_interface->declare(&fslider4, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation  0", &fslider4, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(0, "0+5", "");
		ui_interface->openHorizontalBox("Spherical Wave");
		ui_interface->addCheckButton("Yes", &fcheckbox2);
		ui_interface->closeBox();
		ui_interface->declare(&fslider1, "1+1", "");
		ui_interface->declare(&fslider1, "osc", "/gain_1 -20 20");
		ui_interface->declare(&fslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  1", &fslider1, 0.0, -3e+01, 2e+01, 0.1);
		ui_interface->declare(&fslider0, "1+2", "");
		ui_interface->declare(&fslider0, "osc", "/radius_1 0.5 50");
		ui_interface->declare(&fslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  1", &fslider0, 2.0, 0.5, 5e+01, 0.01);
		ui_interface->declare(&fslider7, "1+3", "");
		ui_interface->declare(&fslider7, "osc", "/azimuth_1 0 360");
		ui_interface->declare(&fslider7, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth  1", &fslider7, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(&fslider6, "1+4", "");
		ui_interface->declare(&fslider6, "osc", "/elevation_1 -90 90");
		ui_interface->declare(&fslider6, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation  1", &fslider6, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(0, "1+5", "");
		ui_interface->openHorizontalBox("Spherical Wave");
		ui_interface->addCheckButton("Yes", &fcheckbox1);
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openVerticalBox("Mute Order");
		ui_interface->addCheckButton("0", &fcheckbox0);
		ui_interface->addCheckButton("1", &fcheckbox3);
		ui_interface->closeBox();
		ui_interface->declare(&fentry0, "~", "");
		ui_interface->addNumEntry("Speaker Radius", &fentry0, 1.07, 0.5, 1e+01, 0.01);
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openVerticalBox("Outputs");
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("1-25");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph0, "osc", "/output1");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1129180", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph1, "osc", "/output2");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac112ac50", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph2, "osc", "/output3");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac112ce20", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph3, "osc", "/output4");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac112f090", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph4, "osc", "/output5");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1131380", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph5, "osc", "/output6");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac11332d0", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph6, "osc", "/output7");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1135990", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph7, "osc", "/output8");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1137c40", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph8, "osc", "/output9");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1139f40", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph9, "osc", "/output10");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac113c2b0", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph10, "osc", "/output11");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac113e840", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph11, "osc", "/output12");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1140e30", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph12, "osc", "/output13");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac11434b0", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph13, "osc", "/output14");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1145b80", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph14, "osc", "/output15");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1147e80", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph15, "osc", "/output16");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac114a1f0", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph16, "osc", "/output17");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac114c5d0", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph17, "osc", "/output18");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac114ea20", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph18, "osc", "/output19");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1151630", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph19, "osc", "/output20");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1153db0", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph20, "osc", "/output21");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac11565f0", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph21, "osc", "/output22");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1158ea0", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph22, "osc", "/output23");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac115b510", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph23, "osc", "/output24");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac115dc00", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph24, "osc", "/output25");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac11603d0", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph25, "osc", "/output26");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1162b20", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph26, "osc", "/output27");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1165790", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph27, "osc", "/output28");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1167820", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph28, "osc", "/output29");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1169960", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph29, "osc", "/output30");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac116bb10", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph30, "osc", "/output31");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac116e7a0", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph31, "osc", "/output32");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1171060", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph32, "osc", "/output33");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1173990", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph33, "osc", "/output34");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1176330", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph34, "osc", "/output35");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1178d40", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fbargraph35, "osc", "/output36");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac117b7c0", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fbargraph36, "osc", "/output37");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac117e250", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fbargraph37, "osc", "/output38");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1180d90", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fbargraph38, "osc", "/output39");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac11830a0", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fbargraph39, "osc", "/output40");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1185410", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fbargraph40, "osc", "/output41");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac11877f0", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fbargraph41, "osc", "/output42");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1189c40", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fbargraph42, "osc", "/output43");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac118c100", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fbargraph43, "osc", "/output44");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac118e630", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fbargraph44, "osc", "/output45");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1190bd0", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fbargraph45, "osc", "/output46");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac11931e0", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fbargraph46, "osc", "/output47");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1195860", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fbargraph47, "osc", "/output48");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac1197f50", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fbargraph48, "osc", "/output49");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac119a6b0", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fbargraph49, "osc", "/output50");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcac119ce80", &fbargraph49, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = double(fentry0);
		double 	fSlow1 = (fSlow0 * (1 - double(fcheckbox0)));
		double 	fSlow2 = double(fslider0);
		double 	fSlow3 = double(fcheckbox1);
		double 	fSlow4 = (1 - ((1 - (1.0 / fSlow2)) * fSlow3));
		double 	fSlow5 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider1))));
		double 	fSlow6 = double(fslider2);
		double 	fSlow7 = double(fcheckbox2);
		double 	fSlow8 = (1 - ((1 - (1.0 / fSlow6)) * fSlow7));
		double 	fSlow9 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider3))));
		double 	fSlow10 = (3 * (1 - double(fcheckbox3)));
		double 	fSlow11 = double(fslider4);
		double 	fSlow12 = sin(fSlow11);
		double 	fSlow13 = cos(fSlow11);
		double 	fSlow14 = double(fslider5);
		double 	fSlow15 = (fSlow13 * cos(fSlow14));
		double 	fSlow16 = (6.123233995736766e-17 * fSlow15);
		double 	fSlow17 = (fSlow12 + fSlow16);
		double 	fSlow18 = ((fConst3 / fSlow6) + 1);
		double 	fSlow19 = (fConst2 / (fSlow6 * fSlow18));
		double 	fSlow20 = (1.0 / ((fConst3 / fSlow0) + 1));
		double 	fSlow21 = (((fSlow7 * fSlow0) * fSlow18) / fSlow6);
		double 	fSlow22 = (fConst2 / fSlow0);
		double 	fSlow23 = ((1 - fSlow7) * fSlow0);
		double 	fSlow24 = double(fslider6);
		double 	fSlow25 = sin(fSlow24);
		double 	fSlow26 = cos(fSlow24);
		double 	fSlow27 = double(fslider7);
		double 	fSlow28 = (fSlow26 * cos(fSlow27));
		double 	fSlow29 = (6.123233995736766e-17 * fSlow28);
		double 	fSlow30 = (fSlow25 + fSlow29);
		double 	fSlow31 = ((fConst3 / fSlow2) + 1);
		double 	fSlow32 = (fConst2 / (fSlow2 * fSlow31));
		double 	fSlow33 = (((fSlow0 * fSlow3) * fSlow31) / fSlow2);
		double 	fSlow34 = (fSlow0 * (1 - fSlow3));
		double 	fSlow35 = (fSlow13 * cos((fSlow14 + -1.5707963267948966)));
		double 	fSlow36 = (fSlow26 * cos((fSlow27 + -1.5707963267948966)));
		double 	fSlow37 = (fSlow13 * cos((fSlow14 + -3.141592653589793)));
		double 	fSlow38 = (fSlow26 * cos((fSlow27 + -3.141592653589793)));
		double 	fSlow39 = (fSlow13 * cos((fSlow14 + -4.71238898038469)));
		double 	fSlow40 = (fSlow26 * cos((fSlow27 + -4.71238898038469)));
		double 	fSlow41 = (fSlow16 - fSlow12);
		double 	fSlow42 = (fSlow29 - fSlow25);
		double 	fSlow43 = (0.7071067811865476 * fSlow28);
		double 	fSlow44 = (0.7071067811865475 * fSlow25);
		double 	fSlow45 = (fSlow43 + fSlow44);
		double 	fSlow46 = (0.7071067811865476 * fSlow15);
		double 	fSlow47 = (0.7071067811865475 * fSlow12);
		double 	fSlow48 = (fSlow46 + fSlow47);
		double 	fSlow49 = (0.7071067811865476 * fSlow36);
		double 	fSlow50 = (fSlow44 + fSlow49);
		double 	fSlow51 = (0.7071067811865476 * fSlow35);
		double 	fSlow52 = (fSlow47 + fSlow51);
		double 	fSlow53 = (0.7071067811865476 * fSlow37);
		double 	fSlow54 = (fSlow47 + fSlow53);
		double 	fSlow55 = (0.7071067811865476 * fSlow38);
		double 	fSlow56 = (fSlow44 + fSlow55);
		double 	fSlow57 = (0.7071067811865476 * fSlow39);
		double 	fSlow58 = (fSlow47 + fSlow57);
		double 	fSlow59 = (0.7071067811865476 * fSlow40);
		double 	fSlow60 = (fSlow44 + fSlow59);
		double 	fSlow61 = (fSlow13 * cos((fSlow14 + -0.7853981633974483)));
		double 	fSlow62 = (fSlow26 * cos((fSlow27 + -0.7853981633974483)));
		double 	fSlow63 = (fSlow13 * cos((fSlow14 + -2.356194490192345)));
		double 	fSlow64 = (fSlow26 * cos((fSlow27 + -2.356194490192345)));
		double 	fSlow65 = (fSlow13 * cos((fSlow14 + -3.9269908169872414)));
		double 	fSlow66 = (fSlow26 * cos((fSlow27 + -3.9269908169872414)));
		double 	fSlow67 = (fSlow13 * cos((fSlow14 + -5.497787143782138)));
		double 	fSlow68 = (fSlow26 * cos((fSlow27 + -5.497787143782138)));
		double 	fSlow69 = (fSlow46 - fSlow47);
		double 	fSlow70 = (fSlow43 - fSlow44);
		double 	fSlow71 = (fSlow51 - fSlow47);
		double 	fSlow72 = (fSlow49 - fSlow44);
		double 	fSlow73 = (fSlow53 - fSlow47);
		double 	fSlow74 = (fSlow55 - fSlow44);
		double 	fSlow75 = (fSlow59 - fSlow44);
		double 	fSlow76 = (fSlow57 - fSlow47);
		double 	fSlow77 = (0.8164965809277259 * fSlow61);
		double 	fSlow78 = (0.5773502691896258 * fSlow12);
		double 	fSlow79 = (fSlow77 + fSlow78);
		double 	fSlow80 = (0.8164965809277259 * fSlow62);
		double 	fSlow81 = (0.5773502691896258 * fSlow25);
		double 	fSlow82 = (fSlow80 + fSlow81);
		double 	fSlow83 = (0.8164965809277259 * fSlow63);
		double 	fSlow84 = (fSlow78 + fSlow83);
		double 	fSlow85 = (0.8164965809277259 * fSlow64);
		double 	fSlow86 = (fSlow81 + fSlow85);
		double 	fSlow87 = (0.8164965809277259 * fSlow65);
		double 	fSlow88 = (fSlow78 + fSlow87);
		double 	fSlow89 = (0.8164965809277259 * fSlow66);
		double 	fSlow90 = (fSlow81 + fSlow89);
		double 	fSlow91 = (0.8164965809277259 * fSlow67);
		double 	fSlow92 = (fSlow78 + fSlow91);
		double 	fSlow93 = (0.8164965809277259 * fSlow68);
		double 	fSlow94 = (fSlow81 + fSlow93);
		double 	fSlow95 = (fSlow80 - fSlow81);
		double 	fSlow96 = (fSlow77 - fSlow78);
		double 	fSlow97 = (fSlow85 - fSlow81);
		double 	fSlow98 = (fSlow83 - fSlow78);
		double 	fSlow99 = (fSlow89 - fSlow81);
		double 	fSlow100 = (fSlow87 - fSlow78);
		double 	fSlow101 = (fSlow93 - fSlow81);
		double 	fSlow102 = (fSlow91 - fSlow78);
		double 	fSlow103 = (0.4264014327112203 * (fSlow13 * cos((fSlow14 + -0.7853981633974484))));
		double 	fSlow104 = (0.9045340337332911 * fSlow12);
		double 	fSlow105 = (fSlow103 + fSlow104);
		double 	fSlow106 = (0.4264014327112203 * (fSlow26 * cos((fSlow27 + -0.7853981633974484))));
		double 	fSlow107 = (0.9045340337332911 * fSlow25);
		double 	fSlow108 = (fSlow106 + fSlow107);
		double 	fSlow109 = (0.4264014327112203 * fSlow64);
		double 	fSlow110 = (fSlow107 + fSlow109);
		double 	fSlow111 = (0.4264014327112203 * fSlow63);
		double 	fSlow112 = (fSlow104 + fSlow111);
		double 	fSlow113 = (0.4264014327112203 * fSlow66);
		double 	fSlow114 = (fSlow107 + fSlow113);
		double 	fSlow115 = (0.4264014327112203 * fSlow65);
		double 	fSlow116 = (fSlow104 + fSlow115);
		double 	fSlow117 = (0.4264014327112203 * fSlow68);
		double 	fSlow118 = (fSlow107 + fSlow117);
		double 	fSlow119 = (0.4264014327112203 * fSlow67);
		double 	fSlow120 = (fSlow104 + fSlow119);
		double 	fSlow121 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -0.32175055439664263))));
		double 	fSlow122 = (0.3015113445777635 * fSlow12);
		double 	fSlow123 = (fSlow121 + fSlow122);
		double 	fSlow124 = (0.9534625892455924 * (fSlow26 * cos((fSlow27 + -0.32175055439664263))));
		double 	fSlow125 = (0.3015113445777635 * fSlow25);
		double 	fSlow126 = (fSlow124 + fSlow125);
		double 	fSlow127 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -1.2490457723982544))));
		double 	fSlow128 = (fSlow122 + fSlow127);
		double 	fSlow129 = (0.9534625892455924 * (fSlow26 * cos((fSlow27 + -1.2490457723982544))));
		double 	fSlow130 = (fSlow125 + fSlow129);
		double 	fSlow131 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -1.8925468811915387))));
		double 	fSlow132 = (fSlow122 + fSlow131);
		double 	fSlow133 = (0.9534625892455924 * (fSlow26 * cos((fSlow27 + -1.8925468811915387))));
		double 	fSlow134 = (fSlow125 + fSlow133);
		double 	fSlow135 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -2.8198420991931505))));
		double 	fSlow136 = (fSlow122 + fSlow135);
		double 	fSlow137 = (0.9534625892455924 * (fSlow26 * cos((fSlow27 + -2.8198420991931505))));
		double 	fSlow138 = (fSlow125 + fSlow137);
		double 	fSlow139 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -3.4633432079864357))));
		double 	fSlow140 = (fSlow122 + fSlow139);
		double 	fSlow141 = (0.9534625892455924 * (fSlow26 * cos((fSlow27 + -3.4633432079864357))));
		double 	fSlow142 = (fSlow125 + fSlow141);
		double 	fSlow143 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -4.3906384259880475))));
		double 	fSlow144 = (fSlow122 + fSlow143);
		double 	fSlow145 = (0.9534625892455924 * (fSlow26 * cos((fSlow27 + -4.3906384259880475))));
		double 	fSlow146 = (fSlow125 + fSlow145);
		double 	fSlow147 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -5.034139534781332))));
		double 	fSlow148 = (fSlow122 + fSlow147);
		double 	fSlow149 = (0.9534625892455924 * (fSlow26 * cos((fSlow27 + -5.034139534781332))));
		double 	fSlow150 = (fSlow125 + fSlow149);
		double 	fSlow151 = (0.9534625892455924 * (cos((fSlow14 + -5.961434752782943)) * fSlow13));
		double 	fSlow152 = (fSlow151 + fSlow122);
		double 	fSlow153 = (0.9534625892455924 * (cos((fSlow27 + -5.961434752782943)) * fSlow26));
		double 	fSlow154 = (fSlow153 + fSlow125);
		double 	fSlow155 = (fSlow124 - fSlow125);
		double 	fSlow156 = (fSlow121 - fSlow122);
		double 	fSlow157 = (fSlow129 - fSlow125);
		double 	fSlow158 = (fSlow127 - fSlow122);
		double 	fSlow159 = (fSlow133 - fSlow125);
		double 	fSlow160 = (fSlow131 - fSlow122);
		double 	fSlow161 = (fSlow137 - fSlow125);
		double 	fSlow162 = (fSlow135 - fSlow122);
		double 	fSlow163 = (fSlow141 - fSlow125);
		double 	fSlow164 = (fSlow139 - fSlow122);
		double 	fSlow165 = (fSlow145 - fSlow125);
		double 	fSlow166 = (fSlow143 - fSlow122);
		double 	fSlow167 = (fSlow149 - fSlow125);
		double 	fSlow168 = (fSlow147 - fSlow122);
		double 	fSlow169 = (fSlow153 - fSlow125);
		double 	fSlow170 = (fSlow151 - fSlow122);
		double 	fSlow171 = (fSlow106 - fSlow107);
		double 	fSlow172 = (fSlow103 - fSlow104);
		double 	fSlow173 = (fSlow109 - fSlow107);
		double 	fSlow174 = (fSlow111 - fSlow104);
		double 	fSlow175 = (fSlow113 - fSlow107);
		double 	fSlow176 = (fSlow115 - fSlow104);
		double 	fSlow177 = (fSlow117 - fSlow107);
		double 	fSlow178 = (fSlow119 - fSlow104);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
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
			fRec1[0] = (fSlow5 + (0.999 * fRec1[1]));
			double fTemp0 = (fRec1[0] * (double)input1[i]);
			fRec2[0] = (fSlow9 + (0.999 * fRec2[1]));
			double fTemp1 = (fRec2[0] * (double)input0[i]);
			double fTemp2 = (fSlow1 * ((fSlow4 * fTemp0) + (fSlow8 * fTemp1)));
			fRec5[0] = (fRec4[1] + fRec5[1]);
			fRec4[0] = (fSlow20 * ((fSlow21 * fTemp1) - (fSlow22 * fRec5[0])));
			fRec3[0] = (fRec4[0] + fRec3[1]);
			fRec7[0] = (fRec6[1] + fRec7[1]);
			fRec6[0] = (fSlow20 * ((fSlow23 * fTemp1) - (fSlow22 * fRec7[0])));
			double fTemp3 = ((fSlow19 * (fRec3[0] - fRec4[0])) + (fRec4[0] + fRec6[0]));
			fRec10[0] = (fRec9[1] + fRec10[1]);
			fRec9[0] = (fSlow20 * ((fSlow33 * fTemp0) - (fSlow22 * fRec10[0])));
			fRec8[0] = (fRec9[0] + fRec8[1]);
			fRec12[0] = (fRec11[1] + fRec12[1]);
			fRec11[0] = (fSlow20 * ((fSlow34 * fTemp0) - (fSlow22 * fRec12[0])));
			double fTemp4 = ((fSlow32 * (fRec8[0] - fRec9[0])) + (fRec9[0] + fRec11[0]));
			double fTemp5 = (0.012698412698412698 * (fTemp2 + (fSlow10 * ((fSlow17 * fTemp3) + (fSlow30 * fTemp4)))));
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fbargraph0 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp5;
			double fTemp6 = (0.012698412698412698 * (fTemp2 + (fSlow10 * ((fSlow28 * fTemp4) + (fSlow15 * fTemp3)))));
			fRec13[0] = max((fRec13[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fbargraph1 = fRec13[0];
			output1[i] = (FAUSTFLOAT)fTemp6;
			double fTemp7 = (0.012698412698412698 * (fTemp2 + (fSlow10 * ((fSlow35 * fTemp3) + (fSlow36 * fTemp4)))));
			fRec14[0] = max((fRec14[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp7))))));
			fbargraph2 = fRec14[0];
			output2[i] = (FAUSTFLOAT)fTemp7;
			double fTemp8 = (0.012698412698412698 * (fTemp2 + (fSlow10 * ((fSlow37 * fTemp3) + (fSlow38 * fTemp4)))));
			fRec15[0] = max((fRec15[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fbargraph3 = fRec15[0];
			output3[i] = (FAUSTFLOAT)fTemp8;
			double fTemp9 = (0.012698412698412698 * (fTemp2 + (fSlow10 * ((fSlow39 * fTemp3) + (fSlow40 * fTemp4)))));
			fRec16[0] = max((fRec16[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fbargraph4 = fRec16[0];
			output4[i] = (FAUSTFLOAT)fTemp9;
			double fTemp10 = (0.012698412698412698 * (fTemp2 + (fSlow10 * ((fSlow41 * fTemp3) + (fSlow42 * fTemp4)))));
			fRec17[0] = max((fRec17[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fbargraph5 = fRec17[0];
			output5[i] = (FAUSTFLOAT)fTemp10;
			double fTemp11 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow45 * fTemp4) + (fSlow48 * fTemp3)))));
			fRec18[0] = max((fRec18[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fbargraph6 = fRec18[0];
			output6[i] = (FAUSTFLOAT)fTemp11;
			double fTemp12 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow50 * fTemp4) + (fSlow52 * fTemp3)))));
			fRec19[0] = max((fRec19[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fbargraph7 = fRec19[0];
			output7[i] = (FAUSTFLOAT)fTemp12;
			double fTemp13 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow54 * fTemp3) + (fSlow56 * fTemp4)))));
			fRec20[0] = max((fRec20[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fbargraph8 = fRec20[0];
			output8[i] = (FAUSTFLOAT)fTemp13;
			double fTemp14 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow58 * fTemp3) + (fSlow60 * fTemp4)))));
			fRec21[0] = max((fRec21[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fbargraph9 = fRec21[0];
			output9[i] = (FAUSTFLOAT)fTemp14;
			double fTemp15 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow61 * fTemp3) + (fSlow62 * fTemp4)))));
			fRec22[0] = max((fRec22[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fbargraph10 = fRec22[0];
			output10[i] = (FAUSTFLOAT)fTemp15;
			double fTemp16 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow63 * fTemp3) + (fSlow64 * fTemp4)))));
			fRec23[0] = max((fRec23[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fbargraph11 = fRec23[0];
			output11[i] = (FAUSTFLOAT)fTemp16;
			double fTemp17 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow65 * fTemp3) + (fSlow66 * fTemp4)))));
			fRec24[0] = max((fRec24[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fbargraph12 = fRec24[0];
			output12[i] = (FAUSTFLOAT)fTemp17;
			double fTemp18 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow67 * fTemp3) + (fSlow68 * fTemp4)))));
			fRec25[0] = max((fRec25[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fbargraph13 = fRec25[0];
			output13[i] = (FAUSTFLOAT)fTemp18;
			double fTemp19 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow69 * fTemp3) + (fSlow70 * fTemp4)))));
			fRec26[0] = max((fRec26[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fbargraph14 = fRec26[0];
			output14[i] = (FAUSTFLOAT)fTemp19;
			double fTemp20 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow71 * fTemp3) + (fSlow72 * fTemp4)))));
			fRec27[0] = max((fRec27[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph15 = fRec27[0];
			output15[i] = (FAUSTFLOAT)fTemp20;
			double fTemp21 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow73 * fTemp3) + (fSlow74 * fTemp4)))));
			fRec28[0] = max((fRec28[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fbargraph16 = fRec28[0];
			output16[i] = (FAUSTFLOAT)fTemp21;
			double fTemp22 = (0.022574955908289243 * (fTemp2 + (fSlow10 * ((fSlow75 * fTemp4) + (fSlow76 * fTemp3)))));
			fRec29[0] = max((fRec29[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fbargraph17 = fRec29[0];
			output17[i] = (FAUSTFLOAT)fTemp22;
			double fTemp23 = (0.02109375 * (fTemp2 + (fSlow10 * ((fSlow79 * fTemp3) + (fSlow82 * fTemp4)))));
			fRec30[0] = max((fRec30[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fbargraph18 = fRec30[0];
			output18[i] = (FAUSTFLOAT)fTemp23;
			double fTemp24 = (0.02109375 * (fTemp2 + (fSlow10 * ((fSlow84 * fTemp3) + (fSlow86 * fTemp4)))));
			fRec31[0] = max((fRec31[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fbargraph19 = fRec31[0];
			output19[i] = (FAUSTFLOAT)fTemp24;
			double fTemp25 = (0.02109375 * (fTemp2 + (fSlow10 * ((fSlow88 * fTemp3) + (fSlow90 * fTemp4)))));
			fRec32[0] = max((fRec32[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fbargraph20 = fRec32[0];
			output20[i] = (FAUSTFLOAT)fTemp25;
			double fTemp26 = (0.02109375 * (fTemp2 + (fSlow10 * ((fSlow92 * fTemp3) + (fSlow94 * fTemp4)))));
			fRec33[0] = max((fRec33[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fbargraph21 = fRec33[0];
			output21[i] = (FAUSTFLOAT)fTemp26;
			double fTemp27 = (0.02109375 * (fTemp2 + (fSlow10 * ((fSlow95 * fTemp4) + (fSlow96 * fTemp3)))));
			fRec34[0] = max((fRec34[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fbargraph22 = fRec34[0];
			output22[i] = (FAUSTFLOAT)fTemp27;
			double fTemp28 = (0.02109375 * (fTemp2 + (fSlow10 * ((fSlow97 * fTemp4) + (fSlow98 * fTemp3)))));
			fRec35[0] = max((fRec35[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph23 = fRec35[0];
			output23[i] = (FAUSTFLOAT)fTemp28;
			double fTemp29 = (0.02109375 * (fTemp2 + (fSlow10 * ((fSlow99 * fTemp4) + (fSlow100 * fTemp3)))));
			fRec36[0] = max((fRec36[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph24 = fRec36[0];
			output24[i] = (FAUSTFLOAT)fTemp29;
			double fTemp30 = (0.02109375 * (fTemp2 + (fSlow10 * ((fSlow101 * fTemp4) + (fSlow102 * fTemp3)))));
			fRec37[0] = max((fRec37[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fbargraph25 = fRec37[0];
			output25[i] = (FAUSTFLOAT)fTemp30;
			double fTemp31 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow105 * fTemp3) + (fSlow108 * fTemp4)))));
			fRec38[0] = max((fRec38[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fbargraph26 = fRec38[0];
			output26[i] = (FAUSTFLOAT)fTemp31;
			double fTemp32 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow110 * fTemp4) + (fSlow112 * fTemp3)))));
			fRec39[0] = max((fRec39[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph27 = fRec39[0];
			output27[i] = (FAUSTFLOAT)fTemp32;
			double fTemp33 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow114 * fTemp4) + (fSlow116 * fTemp3)))));
			fRec40[0] = max((fRec40[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph28 = fRec40[0];
			output28[i] = (FAUSTFLOAT)fTemp33;
			double fTemp34 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow118 * fTemp4) + (fSlow120 * fTemp3)))));
			fRec41[0] = max((fRec41[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fbargraph29 = fRec41[0];
			output29[i] = (FAUSTFLOAT)fTemp34;
			double fTemp35 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow123 * fTemp3) + (fSlow126 * fTemp4)))));
			fRec42[0] = max((fRec42[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fbargraph30 = fRec42[0];
			output30[i] = (FAUSTFLOAT)fTemp35;
			double fTemp36 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow128 * fTemp3) + (fSlow130 * fTemp4)))));
			fRec43[0] = max((fRec43[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph31 = fRec43[0];
			output31[i] = (FAUSTFLOAT)fTemp36;
			double fTemp37 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow132 * fTemp3) + (fSlow134 * fTemp4)))));
			fRec44[0] = max((fRec44[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fbargraph32 = fRec44[0];
			output32[i] = (FAUSTFLOAT)fTemp37;
			double fTemp38 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow136 * fTemp3) + (fSlow138 * fTemp4)))));
			fRec45[0] = max((fRec45[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fbargraph33 = fRec45[0];
			output33[i] = (FAUSTFLOAT)fTemp38;
			double fTemp39 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow140 * fTemp3) + (fSlow142 * fTemp4)))));
			fRec46[0] = max((fRec46[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fbargraph34 = fRec46[0];
			output34[i] = (FAUSTFLOAT)fTemp39;
			double fTemp40 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow144 * fTemp3) + (fSlow146 * fTemp4)))));
			fRec47[0] = max((fRec47[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fbargraph35 = fRec47[0];
			output35[i] = (FAUSTFLOAT)fTemp40;
			double fTemp41 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow148 * fTemp3) + (fSlow150 * fTemp4)))));
			fRec48[0] = max((fRec48[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fbargraph36 = fRec48[0];
			output36[i] = (FAUSTFLOAT)fTemp41;
			double fTemp42 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow152 * fTemp3) + (fSlow154 * fTemp4)))));
			fRec49[0] = max((fRec49[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fbargraph37 = fRec49[0];
			output37[i] = (FAUSTFLOAT)fTemp42;
			double fTemp43 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow155 * fTemp4) + (fSlow156 * fTemp3)))));
			fRec50[0] = max((fRec50[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fbargraph38 = fRec50[0];
			output38[i] = (FAUSTFLOAT)fTemp43;
			double fTemp44 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow157 * fTemp4) + (fSlow158 * fTemp3)))));
			fRec51[0] = max((fRec51[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fbargraph39 = fRec51[0];
			output39[i] = (FAUSTFLOAT)fTemp44;
			double fTemp45 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow159 * fTemp4) + (fSlow160 * fTemp3)))));
			fRec52[0] = max((fRec52[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fbargraph40 = fRec52[0];
			output40[i] = (FAUSTFLOAT)fTemp45;
			double fTemp46 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow161 * fTemp4) + (fSlow162 * fTemp3)))));
			fRec53[0] = max((fRec53[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fbargraph41 = fRec53[0];
			output41[i] = (FAUSTFLOAT)fTemp46;
			double fTemp47 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow163 * fTemp4) + (fSlow164 * fTemp3)))));
			fRec54[0] = max((fRec54[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fbargraph42 = fRec54[0];
			output42[i] = (FAUSTFLOAT)fTemp47;
			double fTemp48 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow165 * fTemp4) + (fSlow166 * fTemp3)))));
			fRec55[0] = max((fRec55[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fbargraph43 = fRec55[0];
			output43[i] = (FAUSTFLOAT)fTemp48;
			double fTemp49 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow167 * fTemp4) + (fSlow168 * fTemp3)))));
			fRec56[0] = max((fRec56[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fbargraph44 = fRec56[0];
			output44[i] = (FAUSTFLOAT)fTemp49;
			double fTemp50 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow169 * fTemp4) + (fSlow170 * fTemp3)))));
			fRec57[0] = max((fRec57[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fbargraph45 = fRec57[0];
			output45[i] = (FAUSTFLOAT)fTemp50;
			double fTemp51 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow171 * fTemp4) + (fSlow172 * fTemp3)))));
			fRec58[0] = max((fRec58[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fbargraph46 = fRec58[0];
			output46[i] = (FAUSTFLOAT)fTemp51;
			double fTemp52 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow173 * fTemp4) + (fSlow174 * fTemp3)))));
			fRec59[0] = max((fRec59[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fbargraph47 = fRec59[0];
			output47[i] = (FAUSTFLOAT)fTemp52;
			double fTemp53 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow175 * fTemp4) + (fSlow176 * fTemp3)))));
			fRec60[0] = max((fRec60[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fbargraph48 = fRec60[0];
			output48[i] = (FAUSTFLOAT)fTemp53;
			double fTemp54 = (0.02017333553791887 * (fTemp2 + (fSlow10 * ((fSlow177 * fTemp4) + (fSlow178 * fTemp3)))));
			fRec61[0] = max((fRec61[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fbargraph49 = fRec61[0];
			output49[i] = (FAUSTFLOAT)fTemp54;
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
			fRec14[1] = fRec14[0];
			fRec13[1] = fRec13[0];
			fRec0[1] = fRec0[0];
			fRec11[1] = fRec11[0];
			fRec12[1] = fRec12[0];
			fRec8[1] = fRec8[0];
			fRec9[1] = fRec9[0];
			fRec10[1] = fRec10[0];
			fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec2[1] = fRec2[0];
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
