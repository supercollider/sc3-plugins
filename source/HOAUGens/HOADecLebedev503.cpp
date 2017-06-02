//----------------------------------------------------------
// name: "HOADecLebedev503"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2014"
//
// Code generated with Faust 0.9.95 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with  */
#include <math.h>
#ifndef FAUSTPOWER
#define FAUSTPOWER
#include <cmath>
template <int N> inline int faustpower(int x)              { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline int faustpower<0>(int x)            { return 1; }
template <> 	 inline int faustpower<1>(int x)            { return x; }
template <> 	 inline int faustpower<2>(int x)            { return x*x; }
template <int N> inline double faustpower(double x)          { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline double faustpower<0>(double x)        { return 1; }
template <> 	 inline double faustpower<1>(double x)        { return x; }
template <> 	 inline double faustpower<2>(double x)        { return x*x; }
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
	double 	fConst4;
	double 	fConst5;
	double 	fRec7[2];
	double 	fRec8[2];
	double 	fRec6[2];
	double 	fConst6;
	double 	fRec9[2];
	double 	fRec5[2];
	double 	fRec4[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec13[2];
	double 	fRec14[2];
	double 	fRec12[2];
	double 	fRec15[2];
	double 	fRec11[2];
	double 	fRec10[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec19[2];
	double 	fRec20[2];
	double 	fRec18[2];
	double 	fRec21[2];
	double 	fRec17[2];
	double 	fRec16[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fRec25[2];
	double 	fRec26[2];
	double 	fRec24[2];
	double 	fRec27[2];
	double 	fRec23[2];
	double 	fRec22[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fConst7;
	double 	fConst8;
	double 	fRec30[2];
	double 	fRec31[2];
	double 	fRec29[2];
	double 	fRec28[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec34[2];
	double 	fRec35[2];
	double 	fRec33[2];
	double 	fRec32[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec38[2];
	double 	fRec39[2];
	double 	fRec37[2];
	double 	fRec36[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fConst9;
	double 	fRec42[2];
	double 	fRec41[2];
	double 	fRec40[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec45[2];
	double 	fRec44[2];
	double 	fRec43[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec50[2];
	double 	fRec51[2];
	double 	fRec49[2];
	double 	fRec48[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec54[2];
	double 	fRec53[2];
	double 	fRec52[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec58[2];
	double 	fRec59[2];
	double 	fRec57[2];
	double 	fRec60[2];
	double 	fRec56[2];
	double 	fRec55[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec64[2];
	double 	fRec65[2];
	double 	fRec63[2];
	double 	fRec66[2];
	double 	fRec62[2];
	double 	fRec61[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec47[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec67[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec68[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec69[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec70[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec75[2];
	double 	fRec76[2];
	double 	fRec74[2];
	double 	fRec77[2];
	double 	fRec73[2];
	double 	fRec72[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec80[2];
	double 	fRec81[2];
	double 	fRec79[2];
	double 	fRec78[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec71[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec82[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec83[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec84[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec85[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec86[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec87[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec88[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec89[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec90[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec91[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec92[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec93[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec94[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec95[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec96[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec97[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec98[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec99[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec100[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec101[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec102[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec103[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec104[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec105[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec106[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec107[2];
	FAUSTFLOAT 	fbargraph49;
	double 	fRec108[2];
	FAUSTFLOAT 	fbargraph50;
	double 	fRec109[2];
	FAUSTFLOAT 	fbargraph51;
	double 	fRec110[2];
	FAUSTFLOAT 	fbargraph52;
	double 	fRec111[2];
	FAUSTFLOAT 	fbargraph53;
	double 	fRec112[2];
	FAUSTFLOAT 	fbargraph54;
	double 	fRec113[2];
	FAUSTFLOAT 	fbargraph55;
	double 	fRec114[2];
	FAUSTFLOAT 	fbargraph56;
	double 	fRec115[2];
	FAUSTFLOAT 	fbargraph57;
	double 	fRec116[2];
	FAUSTFLOAT 	fbargraph58;
	double 	fRec117[2];
	FAUSTFLOAT 	fbargraph59;
	double 	fRec118[2];
	FAUSTFLOAT 	fbargraph60;
	double 	fRec119[2];
	FAUSTFLOAT 	fbargraph61;
	double 	fRec120[2];
	FAUSTFLOAT 	fbargraph62;
	double 	fRec121[2];
	FAUSTFLOAT 	fbargraph63;
	double 	fRec122[2];
	FAUSTFLOAT 	fbargraph64;
	double 	fRec123[2];
	FAUSTFLOAT 	fbargraph65;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("math.lib/name", "Faust Math Library");
		m->declare("math.lib/version", "2.0");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("name", "HOADecLebedev503");
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
	}

	virtual int getNumInputs() { return 16; }
	virtual int getNumOutputs() { return 50; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(1.92e+05, max(1.0, (double)fSamplingFreq));
		fConst1 = (8e+01 / fConst0);
		fConst2 = (394.7715102864353 / fConst0);
		fConst3 = (625.2284897135664 / fConst0);
		fConst4 = (1.7e+02 / fConst0);
		fConst5 = (1250.4569794271329 / fConst0);
		fConst6 = (789.5430205728705 / fConst0);
		fConst7 = (5.1e+02 / fConst0);
		fConst8 = (1.02e+03 / fConst0);
		fConst9 = (3.4e+02 / fConst0);
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
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<2; i++) fRec88[i] = 0;
		for (int i=0; i<2; i++) fRec89[i] = 0;
		for (int i=0; i<2; i++) fRec90[i] = 0;
		for (int i=0; i<2; i++) fRec91[i] = 0;
		for (int i=0; i<2; i++) fRec92[i] = 0;
		for (int i=0; i<2; i++) fRec93[i] = 0;
		for (int i=0; i<2; i++) fRec94[i] = 0;
		for (int i=0; i<2; i++) fRec95[i] = 0;
		for (int i=0; i<2; i++) fRec96[i] = 0;
		for (int i=0; i<2; i++) fRec97[i] = 0;
		for (int i=0; i<2; i++) fRec98[i] = 0;
		for (int i=0; i<2; i++) fRec99[i] = 0;
		for (int i=0; i<2; i++) fRec100[i] = 0;
		for (int i=0; i<2; i++) fRec101[i] = 0;
		for (int i=0; i<2; i++) fRec102[i] = 0;
		for (int i=0; i<2; i++) fRec103[i] = 0;
		for (int i=0; i<2; i++) fRec104[i] = 0;
		for (int i=0; i<2; i++) fRec105[i] = 0;
		for (int i=0; i<2; i++) fRec106[i] = 0;
		for (int i=0; i<2; i++) fRec107[i] = 0;
		for (int i=0; i<2; i++) fRec108[i] = 0;
		for (int i=0; i<2; i++) fRec109[i] = 0;
		for (int i=0; i<2; i++) fRec110[i] = 0;
		for (int i=0; i<2; i++) fRec111[i] = 0;
		for (int i=0; i<2; i++) fRec112[i] = 0;
		for (int i=0; i<2; i++) fRec113[i] = 0;
		for (int i=0; i<2; i++) fRec114[i] = 0;
		for (int i=0; i<2; i++) fRec115[i] = 0;
		for (int i=0; i<2; i++) fRec116[i] = 0;
		for (int i=0; i<2; i++) fRec117[i] = 0;
		for (int i=0; i<2; i++) fRec118[i] = 0;
		for (int i=0; i<2; i++) fRec119[i] = 0;
		for (int i=0; i<2; i++) fRec120[i] = 0;
		for (int i=0; i<2; i++) fRec121[i] = 0;
		for (int i=0; i<2; i++) fRec122[i] = 0;
		for (int i=0; i<2; i++) fRec123[i] = 0;
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
		ui_interface->addVerticalBargraph("0x7ff56ee3b5c0", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee646d0", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee587b0", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee5b070", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee61ea0", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee7e610", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee55ab0", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee52bb0", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee4fb20", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee6bf40", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee7b5f0", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee683f0", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee4be80", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee484a0", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee449c0", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee40e30", &fbargraph1, -7e+01, 6.0);
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
		ui_interface->declare(&fbargraph10, "osc", "/output1");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee5ceb0", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph11, "osc", "/output2");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee5eeb0", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph16, "osc", "/output3");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee6de20", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph17, "osc", "/output4");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee701e0", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph18, "osc", "/output5");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee729a0", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph19, "osc", "/output6");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee74660", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph20, "osc", "/output7");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee77800", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph23, "osc", "/output8");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee816e0", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph24, "osc", "/output9");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee84420", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph25, "osc", "/output10");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee87610", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph26, "osc", "/output11");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee8a130", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph27, "osc", "/output12");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee8cad0", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph28, "osc", "/output13");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee8f180", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph29, "osc", "/output14");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee91830", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph30, "osc", "/output15");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee93880", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph31, "osc", "/output16");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee964d0", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph32, "osc", "/output17");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee98d30", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph33, "osc", "/output18");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ee9b7e0", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph34, "osc", "/output19");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eea0080", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph35, "osc", "/output20");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eea3650", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph36, "osc", "/output21");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eea6b30", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph37, "osc", "/output22");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eea9c30", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph38, "osc", "/output23");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eeacb10", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph39, "osc", "/output24");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eeaf7d0", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph40, "osc", "/output25");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eeb2590", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph41, "osc", "/output26");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eeb5310", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph42, "osc", "/output27");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eeb9290", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph43, "osc", "/output28");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eebbbd0", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph44, "osc", "/output29");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eebed80", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph45, "osc", "/output30");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eec2240", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph46, "osc", "/output31");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eec6470", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph47, "osc", "/output32");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eec9f80", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph48, "osc", "/output33");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eecd700", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph49, "osc", "/output34");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eed0b80", &fbargraph49, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph50, "osc", "/output35");
		ui_interface->declare(&fbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eed4220", &fbargraph50, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fbargraph51, "osc", "/output36");
		ui_interface->declare(&fbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec4d980", &fbargraph51, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fbargraph52, "osc", "/output37");
		ui_interface->declare(&fbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec512c0", &fbargraph52, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fbargraph53, "osc", "/output38");
		ui_interface->declare(&fbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec54f30", &fbargraph53, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fbargraph54, "osc", "/output39");
		ui_interface->declare(&fbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec57a90", &fbargraph54, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fbargraph55, "osc", "/output40");
		ui_interface->declare(&fbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec5a380", &fbargraph55, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fbargraph56, "osc", "/output41");
		ui_interface->declare(&fbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec5cc40", &fbargraph56, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fbargraph57, "osc", "/output42");
		ui_interface->declare(&fbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec5f780", &fbargraph57, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fbargraph58, "osc", "/output43");
		ui_interface->declare(&fbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec62100", &fbargraph58, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fbargraph59, "osc", "/output44");
		ui_interface->declare(&fbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec64e10", &fbargraph59, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fbargraph60, "osc", "/output45");
		ui_interface->declare(&fbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec67930", &fbargraph60, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fbargraph61, "osc", "/output46");
		ui_interface->declare(&fbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec6a4d0", &fbargraph61, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fbargraph62, "osc", "/output47");
		ui_interface->declare(&fbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec6cf00", &fbargraph62, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fbargraph63, "osc", "/output48");
		ui_interface->declare(&fbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec6fa70", &fbargraph63, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fbargraph64, "osc", "/output49");
		ui_interface->declare(&fbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56ec729d0", &fbargraph64, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fbargraph65, "osc", "/output50");
		ui_interface->declare(&fbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff56eed7470", &fbargraph65, -7e+01, 6.0);
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
		double 	fSlow5 = (1.0 / ((fConst2 / fSlow2) + 1));
		double 	fSlow6 = (fConst4 / fSlow2);
		double 	fSlow7 = faustpower<2>(fSlow6);
		double 	fSlow8 = (1.0 / (((fConst3 / fSlow2) + (6.45943269348338 * fSlow7)) + 1));
		double 	fSlow9 = (25.83773077393352 * fSlow7);
		double 	fSlow10 = (fSlow9 + (fConst5 / fSlow2));
		double 	fSlow11 = (fSlow1 * fSlow2);
		double 	fSlow12 = (fConst6 / fSlow2);
		double 	fSlow13 = (1 - fSlow1);
		double 	fSlow14 = (1.0 / (((3 * fSlow7) + (fConst7 / fSlow2)) + 1));
		double 	fSlow15 = (12 * fSlow7);
		double 	fSlow16 = (fSlow15 + (fConst8 / fSlow2));
		double 	fSlow17 = (1.0 / (fSlow6 + 1));
		double 	fSlow18 = (fConst9 / fSlow2);
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
			fRec7[0] = (fRec6[1] + fRec7[1]);
			fRec8[0] = (fRec7[1] + fRec8[1]);
			double fTemp3 = ((double)input15[i] * fRec3[0]);
			fRec6[0] = (0 - (fSlow8 * (((fSlow10 * fRec7[0]) + (fSlow9 * fRec8[0])) - (fSlow11 * fTemp3))));
			fRec9[0] = (fRec5[1] + fRec9[1]);
			fRec5[0] = (fSlow5 * (fRec6[0] - (fSlow12 * fRec9[0])));
			double fTemp4 = (fRec5[0] + (fSlow13 * fTemp3));
			fRec4[0] = max((fRec4[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fbargraph1 = fRec4[0];
			double fTemp5 = fTemp4;
			double fTemp6 = (6.097910338853647e-51 * fTemp5);
			fRec13[0] = (fRec12[1] + fRec13[1]);
			fRec14[0] = (fRec13[1] + fRec14[1]);
			double fTemp7 = ((double)input14[i] * fRec3[0]);
			fRec12[0] = (0 - (fSlow8 * (((fSlow10 * fRec13[0]) + (fSlow9 * fRec14[0])) - (fSlow11 * fTemp7))));
			fRec15[0] = (fRec11[1] + fRec15[1]);
			fRec11[0] = (fSlow5 * (fRec12[0] - (fSlow12 * fRec15[0])));
			double fTemp8 = (fRec11[0] + (fSlow13 * fTemp7));
			fRec10[0] = max((fRec10[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fbargraph2 = fRec10[0];
			double fTemp9 = fTemp8;
			double fTemp10 = (2.4393594688416384e-34 * fTemp9);
			fRec19[0] = (fRec18[1] + fRec19[1]);
			fRec20[0] = (fRec19[1] + fRec20[1]);
			double fTemp11 = ((double)input13[i] * fRec3[0]);
			fRec18[0] = (0 - (fSlow8 * (((fSlow10 * fRec19[0]) + (fSlow9 * fRec20[0])) - (fSlow11 * fTemp11))));
			fRec21[0] = (fRec17[1] + fRec21[1]);
			fRec17[0] = (fSlow5 * (fRec18[0] - (fSlow12 * fRec21[0])));
			double fTemp12 = (fRec17[0] + (fSlow13 * fTemp11));
			fRec16[0] = max((fRec16[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fbargraph3 = fRec16[0];
			double fTemp13 = fTemp12;
			double fTemp14 = (5.0391227634343926e-18 * fTemp13);
			double fTemp15 = ((double)input12[i] * fRec3[0]);
			fRec25[0] = (fRec24[1] + fRec25[1]);
			fRec26[0] = (fRec25[1] + fRec26[1]);
			fRec24[0] = (fSlow8 * ((fSlow11 * fTemp15) - ((fSlow10 * fRec25[0]) + (fSlow9 * fRec26[0]))));
			fRec27[0] = (fRec23[1] + fRec27[1]);
			fRec23[0] = (fSlow5 * (fRec24[0] - (fSlow12 * fRec27[0])));
			double fTemp16 = ((fSlow13 * fTemp15) + fRec23[0]);
			fRec22[0] = max((fRec22[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fbargraph4 = fRec22[0];
			double fTemp17 = fTemp16;
			double fTemp18 = (0.03359684204526464 * fTemp17);
			double fTemp19 = ((double)input8[i] * fRec3[0]);
			fRec30[0] = (fRec29[1] + fRec30[1]);
			fRec31[0] = (fRec30[1] + fRec31[1]);
			fRec29[0] = (0 - (fSlow14 * (((fSlow16 * fRec30[0]) + (fSlow15 * fRec31[0])) - (fSlow11 * fTemp19))));
			double fTemp20 = ((fSlow13 * fTemp19) + fRec29[0]);
			fRec28[0] = max((fRec28[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph5 = fRec28[0];
			double fTemp21 = fTemp20;
			double fTemp22 = (9.219912161207984e-35 * fTemp21);
			fRec34[0] = (fRec33[1] + fRec34[1]);
			fRec35[0] = (fRec34[1] + fRec35[1]);
			double fTemp23 = ((double)input7[i] * fRec3[0]);
			fRec33[0] = (0 - (fSlow14 * (((fSlow16 * fRec34[0]) + (fSlow15 * fRec35[0])) - (fSlow11 * fTemp23))));
			double fTemp24 = (fRec33[0] + (fSlow13 * fTemp23));
			fRec32[0] = max((fRec32[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fbargraph6 = fRec32[0];
			double fTemp25 = fTemp24;
			double fTemp26 = (3.0114518464024883e-18 * fTemp25);
			double fTemp27 = ((double)input6[i] * fRec3[0]);
			fRec38[0] = (fRec37[1] + fRec38[1]);
			fRec39[0] = (fRec38[1] + fRec39[1]);
			fRec37[0] = (fSlow14 * ((fSlow11 * fTemp27) - ((fSlow16 * fRec38[0]) + (fSlow15 * fRec39[0]))));
			double fTemp28 = (fRec37[0] + (fSlow13 * fTemp27));
			fRec36[0] = max((fRec36[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph7 = fRec36[0];
			double fTemp29 = fTemp28;
			double fTemp30 = (0.02839451399999733 * fTemp29);
			double fTemp31 = ((double)input2[i] * fRec3[0]);
			fRec42[0] = (fRec41[1] + fRec42[1]);
			fRec41[0] = (fSlow17 * ((fSlow11 * fTemp31) - (fSlow18 * fRec42[0])));
			double fTemp32 = ((fSlow13 * fTemp31) + fRec41[0]);
			fRec40[0] = max((fRec40[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph8 = fRec40[0];
			double fTemp33 = fTemp32;
			double fTemp34 = (0.0219942959691286 * fTemp33);
			double fTemp35 = ((double)input3[i] * fRec3[0]);
			fRec45[0] = (fRec44[1] + fRec45[1]);
			fRec44[0] = (fSlow17 * ((fSlow11 * fTemp35) - (fSlow18 * fRec45[0])));
			double fTemp36 = (fRec44[0] + (fSlow13 * fTemp35));
			fRec43[0] = max((fRec43[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph9 = fRec43[0];
			double fTemp37 = fTemp36;
			double fTemp38 = (1.3467622079046435e-18 * fTemp37);
			double fTemp39 = (fRec1[0] * (fTemp2 + (fTemp6 + (fTemp10 + (fTemp14 + (fTemp18 + (fTemp22 + (fTemp26 + (fTemp30 + (fTemp34 + fTemp38))))))))));
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fbargraph10 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp39;
			double fTemp40 = (0.026560635762986527 * fTemp5);
			double fTemp41 = (0.0219942959691286 * fTemp37);
			double fTemp42 = (0.024590370452110585 * fTemp21);
			double fTemp43 = (0.014197256999998666 * fTemp29);
			double fTemp44 = (0.020573779994945588 * fTemp13);
			double fTemp45 = (fRec1[0] * ((fTemp2 + (fTemp40 + (fTemp41 + fTemp42))) - (fTemp43 + fTemp44)));
			fRec46[0] = max((fRec46[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fbargraph11 = fRec46[0];
			output1[i] = (FAUSTFLOAT)fTemp45;
			double fTemp46 = ((double)input4[i] * fRec3[0]);
			fRec50[0] = (fRec49[1] + fRec50[1]);
			fRec51[0] = (fRec50[1] + fRec51[1]);
			fRec49[0] = (0 - (fSlow14 * (((fSlow16 * fRec50[0]) + (fSlow15 * fRec51[0])) - (fSlow11 * fTemp46))));
			double fTemp47 = ((fSlow13 * fTemp46) + fRec49[0]);
			fRec48[0] = max((fRec48[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fbargraph12 = fRec48[0];
			double fTemp48 = fTemp47;
			double fTemp49 = ((double)input1[i] * fRec3[0]);
			fRec54[0] = (fRec53[1] + fRec54[1]);
			fRec53[0] = (fSlow17 * ((fSlow11 * fTemp49) - (fSlow18 * fRec54[0])));
			double fTemp50 = (fRec53[0] + (fSlow13 * fTemp49));
			fRec52[0] = max((fRec52[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fbargraph13 = fRec52[0];
			double fTemp51 = fTemp50;
			double fTemp52 = (0.0219942959691286 * fTemp51);
			fRec58[0] = (fRec57[1] + fRec58[1]);
			fRec59[0] = (fRec58[1] + fRec59[1]);
			double fTemp53 = ((double)input11[i] * fRec3[0]);
			fRec57[0] = (0 - (fSlow8 * (((fSlow10 * fRec58[0]) + (fSlow9 * fRec59[0])) - (fSlow11 * fTemp53))));
			fRec60[0] = (fRec56[1] + fRec60[1]);
			fRec56[0] = (fSlow5 * (fRec57[0] - (fSlow12 * fRec60[0])));
			double fTemp54 = (fRec56[0] + (fSlow13 * fTemp53));
			fRec55[0] = max((fRec55[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fbargraph14 = fRec55[0];
			double fTemp55 = fTemp54;
			double fTemp56 = (0.020573779994945588 * fTemp55);
			fRec64[0] = (fRec63[1] + fRec64[1]);
			fRec65[0] = (fRec64[1] + fRec65[1]);
			double fTemp57 = ((double)input9[i] * fRec3[0]);
			fRec63[0] = (0 - (fSlow8 * (((fSlow10 * fRec64[0]) + (fSlow9 * fRec65[0])) - (fSlow11 * fTemp57))));
			fRec66[0] = (fRec62[1] + fRec66[1]);
			fRec62[0] = (fSlow5 * (fRec63[0] - (fSlow12 * fRec66[0])));
			double fTemp58 = (fRec62[0] + (fSlow13 * fTemp57));
			fRec61[0] = max((fRec61[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fbargraph15 = fRec61[0];
			double fTemp59 = fTemp58;
			double fTemp60 = (0.026560635762986527 * fTemp59);
			double fTemp61 = (fRec1[0] * ((fTemp2 + ((3.0114518464024883e-18 * fTemp48) + (fTemp38 + fTemp52))) - (fTemp56 + (fTemp60 + ((4.879109635569025e-18 * fTemp5) + ((1.2597806908585981e-18 * fTemp13) + (fTemp42 + fTemp43)))))));
			fRec47[0] = max((fRec47[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fbargraph16 = fRec47[0];
			output2[i] = (FAUSTFLOAT)fTemp61;
			double fTemp62 = (fRec1[0] * ((fTemp2 + ((9.75821927113805e-18 * fTemp59) + ((2.693524415809287e-18 * fTemp51) + (fTemp42 + fTemp44)))) - ((2.5195613817171963e-18 * fTemp55) + ((6.0229036928049765e-18 * fTemp48) + (fTemp40 + (fTemp41 + fTemp43))))));
			fRec67[0] = max((fRec67[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fbargraph17 = fRec67[0];
			output3[i] = (FAUSTFLOAT)fTemp62;
			double fTemp63 = (fRec1[0] * ((fTemp2 + (fTemp56 + (fTemp60 + ((9.034355539207464e-18 * fTemp48) + ((3.779342072575794e-18 * fTemp13) + (1.4637328906707076e-17 * fTemp5)))))) - (fTemp52 + (fTemp42 + (fTemp43 + (4.040286623713931e-18 * fTemp37))))));
			fRec68[0] = max((fRec68[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fbargraph18 = fRec68[0];
			output4[i] = (FAUSTFLOAT)fTemp63;
			double fTemp64 = (fRec1[0] * ((fTemp2 + (fTemp6 + (fTemp14 + (fTemp22 + (fTemp38 + fTemp30))))) - (fTemp10 + (fTemp18 + (fTemp34 + fTemp26)))));
			fRec69[0] = max((fRec69[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fbargraph19 = fRec69[0];
			output5[i] = (FAUSTFLOAT)fTemp64;
			double fTemp65 = (0.022574955908289243 * fTemp1);
			double fTemp66 = (0.01669440503167441 * fTemp5);
			double fTemp67 = (0.04089277388695433 * fTemp9);
			double fTemp68 = (0.03879429159750975 * fTemp13);
			double fTemp69 = (0.02185810706854275 * fTemp21);
			double fTemp70 = (0.043716214137085485 * fTemp25);
			double fTemp71 = (0.012619783999998805 * fTemp29);
			double fTemp72 = (0.0276485614705685 * fTemp33);
			double fTemp73 = (0.027648561470568506 * fTemp37);
			double fTemp74 = (fTemp72 + fTemp73);
			double fTemp75 = (0.010558468816293319 * fTemp17);
			double fTemp76 = (fRec1[0] * ((fTemp65 + (fTemp66 + (fTemp67 + (fTemp68 + (fTemp69 + (fTemp70 + (fTemp71 + fTemp74))))))) - fTemp75));
			fRec70[0] = max((fRec70[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp76))))));
			fbargraph20 = fRec70[0];
			output6[i] = (FAUSTFLOAT)fTemp76;
			fRec75[0] = (fRec74[1] + fRec75[1]);
			fRec76[0] = (fRec75[1] + fRec76[1]);
			double fTemp77 = ((double)input10[i] * fRec3[0]);
			fRec74[0] = (0 - (fSlow8 * (((fSlow10 * fRec75[0]) + (fSlow9 * fRec76[0])) - (fSlow11 * fTemp77))));
			fRec77[0] = (fRec73[1] + fRec77[1]);
			fRec73[0] = (fSlow5 * (fRec74[0] - (fSlow12 * fRec77[0])));
			double fTemp78 = (fRec73[0] + (fSlow13 * fTemp77));
			fRec72[0] = max((fRec72[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fbargraph21 = fRec72[0];
			double fTemp79 = fTemp78;
			double fTemp80 = (5.007920464891509e-18 * fTemp79);
			double fTemp81 = ((double)input5[i] * fRec3[0]);
			fRec80[0] = (fRec79[1] + fRec80[1]);
			fRec81[0] = (fRec80[1] + fRec81[1]);
			fRec79[0] = (0 - (fSlow14 * (((fSlow16 * fRec80[0]) + (fSlow15 * fRec81[0])) - (fSlow11 * fTemp81))));
			double fTemp82 = ((fSlow13 * fTemp81) + fRec79[0]);
			fRec78[0] = max((fRec78[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp82))))));
			fbargraph22 = fRec78[0];
			double fTemp83 = fTemp82;
			double fTemp84 = (0.043716214137085485 * fTemp83);
			double fTemp85 = (0.03879429159750975 * fTemp55);
			double fTemp86 = (2.6768460856911014e-18 * fTemp48);
			double fTemp87 = (0.027648561470568506 * fTemp51);
			double fTemp88 = (2.3754652515039685e-18 * fTemp13);
			double fTemp89 = (2.6768460856911007e-18 * fTemp25);
			double fTemp90 = (1.6929861152980278e-18 * fTemp37);
			double fTemp91 = (0.01669440503167441 * fTemp59);
			double fTemp92 = (3.0667124528564294e-18 * fTemp5);
			double fTemp93 = (fRec1[0] * ((fTemp65 + (fTemp80 + (fTemp84 + (fTemp85 + (fTemp86 + (fTemp87 + (fTemp88 + (fTemp89 + (fTemp71 + (fTemp72 + fTemp90)))))))))) - (fTemp91 + (fTemp92 + (fTemp67 + (fTemp69 + fTemp75))))));
			fRec71[0] = max((fRec71[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fbargraph23 = fRec71[0];
			output7[i] = (FAUSTFLOAT)fTemp93;
			double fTemp94 = (5.353692171382201e-18 * fTemp83);
			double fTemp95 = (4.750930503007937e-18 * fTemp55);
			double fTemp96 = (6.133424905712859e-18 * fTemp59);
			double fTemp97 = (3.3859722305960555e-18 * fTemp51);
			double fTemp98 = (fTemp72 + fTemp71);
			double fTemp99 = (1.0015840929783018e-17 * fTemp79);
			double fTemp100 = (5.353692171382203e-18 * fTemp48);
			double fTemp101 = (fRec1[0] * ((fTemp65 + (fTemp94 + (fTemp95 + (fTemp96 + (fTemp97 + (fTemp67 + (fTemp69 + fTemp98))))))) - (fTemp99 + (fTemp100 + (fTemp66 + (fTemp68 + (fTemp75 + (fTemp73 + fTemp70))))))));
			fRec82[0] = max((fRec82[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp101))))));
			fbargraph24 = fRec82[0];
			output8[i] = (FAUSTFLOAT)fTemp101;
			double fTemp102 = (1.5023761394674526e-17 * fTemp79);
			double fTemp103 = (8.030538257073304e-18 * fTemp48);
			double fTemp104 = (9.20013735856929e-18 * fTemp5);
			double fTemp105 = (5.078958345894083e-18 * fTemp37);
			double fTemp106 = (8.0305382570733e-18 * fTemp25);
			double fTemp107 = (7.126395754511905e-18 * fTemp13);
			double fTemp108 = (fRec1[0] * ((fTemp65 + (fTemp102 + (fTemp91 + (fTemp103 + (fTemp104 + fTemp98))))) - (fTemp84 + (fTemp85 + (fTemp87 + (fTemp67 + ((fTemp75 + (fTemp69 + (fTemp105 + fTemp106))) + fTemp107)))))));
			fRec83[0] = max((fRec83[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp108))))));
			fbargraph25 = fRec83[0];
			output9[i] = (FAUSTFLOAT)fTemp108;
			double fTemp109 = (0.043716214137085485 * fTemp48);
			double fTemp110 = (0.0276485614705685 * fTemp51);
			double fTemp111 = (0.025862861065006505 * fTemp55);
			double fTemp112 = (0.02523956799999763 * fTemp29);
			double fTemp113 = (0.02586286106500651 * fTemp13);
			double fTemp114 = (fRec1[0] * ((fTemp65 + (fTemp109 + (fTemp110 + (fTemp73 + (2.6768460856911007e-18 * fTemp21))))) - ((fTemp111 + (fTemp112 + fTemp113)) + (0.0333888100633488 * (fTemp5 - fTemp59)))));
			fRec84[0] = max((fRec84[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp114))))));
			fbargraph26 = fRec84[0];
			output10[i] = (FAUSTFLOAT)fTemp114;
			double fTemp115 = (0.025862861065006505 * fTemp13);
			double fTemp116 = (0.02586286106500651 * fTemp55);
			double fTemp117 = (0.0276485614705685 * fTemp37);
			double fTemp118 = (fRec1[0] * ((fTemp65 + ((0.033388810063348796 * fTemp59) + (fTemp87 + (fTemp115 + (0.03338881006334882 * fTemp5))))) - (fTemp116 + (fTemp109 + ((8.0305382570733e-18 * fTemp21) + (fTemp112 + fTemp117))))));
			fRec85[0] = max((fRec85[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp118))))));
			fbargraph27 = fRec85[0];
			output11[i] = (FAUSTFLOAT)fTemp118;
			double fTemp119 = (fRec1[0] * ((fTemp65 + (fTemp111 + (fTemp109 + ((0.03338881006334876 * fTemp5) + (fTemp113 + (1.3384230428455503e-17 * fTemp21)))))) - ((fTemp110 + (fTemp73 + fTemp112)) + (0.03338881006334885 * fTemp59))));
			fRec86[0] = max((fRec86[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp119))))));
			fbargraph28 = fRec86[0];
			output12[i] = (FAUSTFLOAT)fTemp119;
			double fTemp120 = (fRec1[0] * ((fTemp65 + (fTemp117 + fTemp116)) - ((0.03338881006334882 * fTemp59) + (fTemp109 + (fTemp87 + ((0.0333888100633488 * fTemp5) + (fTemp115 + (fTemp112 + (1.8737922599837705e-17 * fTemp21)))))))));
			fRec87[0] = max((fRec87[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp120))))));
			fbargraph29 = fRec87[0];
			output13[i] = (FAUSTFLOAT)fTemp120;
			double fTemp121 = (fRec1[0] * ((fTemp65 + (fTemp66 + (fTemp68 + (fTemp75 + (fTemp69 + (fTemp73 + fTemp71)))))) - (fTemp67 + (fTemp72 + fTemp70))));
			fRec88[0] = max((fRec88[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp121))))));
			fbargraph30 = fRec88[0];
			output14[i] = (FAUSTFLOAT)fTemp121;
			double fTemp122 = (fRec1[0] * ((fTemp65 + (fTemp85 + (fTemp86 + (fTemp87 + (fTemp67 + (fTemp88 + (fTemp75 + (fTemp71 + fTemp90)))))))) - (fTemp80 + (fTemp84 + (fTemp91 + (fTemp92 + (fTemp69 + (fTemp72 + fTemp89))))))));
			fRec89[0] = max((fRec89[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp122))))));
			fbargraph31 = fRec89[0];
			output15[i] = (FAUSTFLOAT)fTemp122;
			double fTemp123 = (fRec1[0] * ((fTemp65 + (fTemp99 + (fTemp95 + (fTemp96 + (fTemp97 + (fTemp75 + (fTemp69 + (fTemp71 + fTemp70)))))))) - (fTemp94 + (fTemp100 + (fTemp66 + (fTemp67 + (fTemp68 + fTemp74)))))));
			fRec90[0] = max((fRec90[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp123))))));
			fbargraph32 = fRec90[0];
			output16[i] = (FAUSTFLOAT)fTemp123;
			double fTemp124 = (fRec1[0] * ((fTemp65 + (fTemp84 + (fTemp91 + (fTemp103 + (fTemp104 + (fTemp67 + (fTemp75 + (fTemp71 + fTemp106)))))))) - (fTemp102 + (fTemp85 + (fTemp87 + (fTemp107 + (fTemp69 + (fTemp72 + fTemp105))))))));
			fRec91[0] = max((fRec91[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp124))))));
			fbargraph33 = fRec91[0];
			output17[i] = (FAUSTFLOAT)fTemp124;
			double fTemp125 = (fTemp33 + fTemp1);
			double fTemp126 = (0.02109375 * fTemp125);
			double fTemp127 = (0.041597435974919175 * fTemp79);
			double fTemp128 = (0.027231914153020904 * fTemp83);
			double fTemp129 = (0.013154264250377145 * fTemp55);
			double fTemp130 = (0.016982082124440745 * fTemp59);
			double fTemp131 = (0.027231914153020897 * fTemp48);
			double fTemp132 = (0.021093749999999994 * fTemp51);
			double fTemp133 = (2.5471083409710867e-18 * fTemp9);
			double fTemp134 = (0.013154264250377147 * fTemp13);
			double fTemp135 = (1.6674738251076273e-18 * fTemp21);
			double fTemp136 = (0.02723191415302091 * fTemp25);
			double fTemp137 = (5.236595479505967e-18 * fTemp29);
			double fTemp138 = ((0.021093749999999998 * fTemp37) + fTemp137);
			double fTemp139 = (0.021480823570105497 * fTemp17);
			double fTemp140 = (0.01698208212444074 * fTemp5);
			double fTemp141 = (fRec1[0] * ((fTemp126 + (fTemp127 + (fTemp128 + (fTemp129 + (fTemp130 + (fTemp131 + (fTemp132 + (fTemp133 + (fTemp134 + (fTemp135 + (fTemp136 + fTemp138))))))))))) - (fTemp139 + fTemp140)));
			fRec92[0] = max((fRec92[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp141))))));
			fbargraph34 = fRec92[0];
			output18[i] = (FAUSTFLOAT)fTemp141;
			double fTemp142 = (0.02723191415302091 * fTemp83);
			double fTemp143 = (0.013154264250377147 * fTemp55);
			double fTemp144 = (0.01698208212444074 * fTemp59);
			double fTemp145 = (0.021093749999999998 * fTemp51);
			double fTemp146 = (0.01698208212444075 * fTemp5);
			double fTemp147 = (7.641325022913258e-18 * fTemp9);
			double fTemp148 = (0.021093749999999994 * fTemp37);
			double fTemp149 = (0.027231914153020904 * fTemp25);
			double fTemp150 = (5.002421475322882e-18 * fTemp21);
			double fTemp151 = (0.013154264250377145 * fTemp13);
			double fTemp152 = (fRec1[0] * ((fTemp126 + (fTemp142 + (fTemp143 + (fTemp144 + (fTemp145 + (fTemp137 + fTemp146)))))) - (fTemp127 + (fTemp131 + (fTemp147 + ((fTemp139 + ((fTemp148 + fTemp149) + fTemp150)) + fTemp151))))));
			fRec93[0] = max((fRec93[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp152))))));
			fbargraph35 = fRec93[0];
			output19[i] = (FAUSTFLOAT)fTemp152;
			double fTemp153 = (0.01698208212444072 * fTemp5);
			double fTemp154 = (1.273554170485543e-17 * fTemp9);
			double fTemp155 = (8.337369125538138e-18 * fTemp21);
			double fTemp156 = (0.016982082124440766 * fTemp59);
			double fTemp157 = (0.013154264250377148 * fTemp13);
			double fTemp158 = (fRec1[0] * (((0.02109375 * (fTemp125 - fTemp37)) + (fTemp127 + (fTemp131 + (fTemp153 + (fTemp154 + (fTemp137 + fTemp155)))))) - (fTemp128 + (fTemp129 + (fTemp156 + (fTemp132 + ((fTemp136 + fTemp139) + fTemp157)))))));
			fRec94[0] = max((fRec94[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp158))))));
			fbargraph36 = fRec94[0];
			output20[i] = (FAUSTFLOAT)fTemp158;
			double fTemp159 = (fTemp137 + fTemp148);
			double fTemp160 = (0.013154264250377148 * fTemp55);
			double fTemp161 = (0.01698208212444075 * fTemp59);
			double fTemp162 = (1.167231677575339e-17 * fTemp21);
			double fTemp163 = (1.7829758386797603e-17 * fTemp9);
			double fTemp164 = (fRec1[0] * (((0.02109375 * (fTemp125 - fTemp51)) + (fTemp151 + (fTemp149 + fTemp159))) - (fTemp127 + (fTemp142 + (fTemp160 + (fTemp161 + (fTemp131 + (fTemp140 + ((fTemp139 + fTemp162) + fTemp163)))))))));
			fRec95[0] = max((fRec95[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp164))))));
			fbargraph37 = fRec95[0];
			output21[i] = (FAUSTFLOAT)fTemp164;
			double fTemp165 = (0.02109375 * (fTemp1 - fTemp33));
			double fTemp166 = (fRec1[0] * ((fTemp165 + (fTemp129 + (fTemp130 + (fTemp131 + (fTemp132 + (fTemp134 + (fTemp139 + (fTemp135 + fTemp138)))))))) - (fTemp127 + (fTemp128 + (fTemp140 + (fTemp136 + fTemp133))))));
			fRec96[0] = max((fRec96[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp166))))));
			fbargraph38 = fRec96[0];
			output22[i] = (FAUSTFLOAT)fTemp166;
			double fTemp167 = (fRec1[0] * ((fTemp165 + (fTemp127 + (fTemp143 + (fTemp144 + (fTemp145 + (fTemp146 + (fTemp147 + (fTemp139 + (fTemp137 + fTemp149))))))))) - (fTemp142 + (fTemp131 + (fTemp151 + (fTemp148 + fTemp150))))));
			fRec97[0] = max((fRec97[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp167))))));
			fbargraph39 = fRec97[0];
			output23[i] = (FAUSTFLOAT)fTemp167;
			double fTemp168 = (fRec1[0] * (((0.02109375 * (fTemp1 - (fTemp33 + fTemp37))) + (fTemp128 + (fTemp131 + (fTemp153 + (fTemp139 + (fTemp155 + (fTemp137 + fTemp136))))))) - (fTemp127 + (fTemp129 + (fTemp156 + (fTemp132 + (fTemp154 + fTemp157)))))));
			fRec98[0] = max((fRec98[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp168))))));
			fbargraph40 = fRec98[0];
			output24[i] = (FAUSTFLOAT)fTemp168;
			double fTemp169 = (fRec1[0] * (((0.02109375 * (fTemp1 - (fTemp33 + fTemp51))) + (fTemp127 + (fTemp142 + (fTemp163 + (fTemp151 + (fTemp139 + fTemp159)))))) - (fTemp160 + (fTemp161 + (fTemp131 + (fTemp140 + (fTemp149 + fTemp162)))))));
			fRec99[0] = max((fRec99[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp169))))));
			fbargraph41 = fRec99[0];
			output25[i] = (FAUSTFLOAT)fTemp169;
			double fTemp170 = (0.02017333553791887 * fTemp1);
			double fTemp171 = (0.021308452520676528 * (fTemp25 + fTemp83));
			double fTemp172 = (0.010535180889746065 * (fTemp37 + fTemp51));
			double fTemp173 = (0.016998264729033836 * fTemp79);
			double fTemp174 = (0.030460165261409405 * fTemp55);
			double fTemp175 = (0.002313170838826743 * fTemp59);
			double fTemp176 = (0.007102817506892165 * fTemp48);
			double fTemp177 = (0.030460165261409402 * fTemp13);
			double fTemp178 = (0.0263335984839893 * fTemp17);
			double fTemp179 = (0.03280650879687195 * fTemp29);
			double fTemp180 = (0.03160554266923825 * fTemp33);
			double fTemp181 = (fTemp179 + fTemp180);
			double fTemp182 = (fTemp178 + fTemp181);
			double fTemp183 = (0.002313170838826745 * fTemp5);
			double fTemp184 = (1.1422201709353029e-18 * fTemp21);
			double fTemp185 = (2.7335294515958808e-18 * fTemp9);
			double fTemp186 = (fRec1[0] * ((fTemp170 + (fTemp171 + (fTemp172 + (fTemp173 + (fTemp174 + (fTemp175 + (fTemp176 + (fTemp177 + fTemp182)))))))) - (fTemp183 + (fTemp184 + fTemp185))));
			fRec100[0] = max((fRec100[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp186))))));
			fbargraph42 = fRec100[0];
			output26[i] = (FAUSTFLOAT)fTemp186;
			double fTemp187 = (0.0023131708388267444 * fTemp5);
			double fTemp188 = (0.021308452520676528 * (fTemp25 - fTemp83));
			double fTemp189 = (0.010535180889746065 * (fTemp37 - fTemp51));
			double fTemp190 = (3.1225305737205953e-18 * fTemp9);
			double fTemp191 = (1.3047664087114909e-18 * fTemp21);
			double fTemp192 = (fRec1[0] * ((fTemp170 + (fTemp174 + (fTemp175 + (fTemp187 + fTemp182)))) - (fTemp188 + (fTemp189 + (fTemp173 + (fTemp176 + (fTemp190 + (fTemp177 + fTemp191))))))));
			fRec101[0] = max((fRec101[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp192))))));
			fbargraph43 = fRec101[0];
			output27[i] = (FAUSTFLOAT)fTemp192;
			double fTemp193 = (0.002313170838826741 * fTemp5);
			double fTemp194 = (5.204217622867659e-18 * fTemp9);
			double fTemp195 = (2.1746106811858183e-18 * fTemp21);
			double fTemp196 = (0.021308452520676528 * fTemp83);
			double fTemp197 = (0.030460165261409402 * fTemp55);
			double fTemp198 = (0.002313170838826747 * fTemp59);
			double fTemp199 = (0.010535180889746065 * fTemp51);
			double fTemp200 = (0.010535180889746067 * fTemp37);
			double fTemp201 = (0.02130845252067653 * fTemp25);
			double fTemp202 = (0.030460165261409412 * fTemp13);
			double fTemp203 = (fRec1[0] * ((fTemp170 + (fTemp173 + (fTemp176 + (fTemp193 + (fTemp194 + (fTemp178 + (fTemp195 + fTemp181))))))) - (fTemp196 + (fTemp197 + (fTemp198 + (fTemp199 + ((fTemp200 + fTemp201) + fTemp202)))))));
			fRec102[0] = max((fRec102[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp203))))));
			fbargraph44 = fRec102[0];
			output28[i] = (FAUSTFLOAT)fTemp203;
			double fTemp204 = (0.021308452520676528 * fTemp25);
			double fTemp205 = (0.010535180889746065 * fTemp37);
			double fTemp206 = (0.02130845252067653 * fTemp83);
			double fTemp207 = (0.030460165261409412 * fTemp55);
			double fTemp208 = (0.0023131708388267444 * fTemp59);
			double fTemp209 = (0.010535180889746067 * fTemp51);
			double fTemp210 = (0.0023131708388267436 * fTemp5);
			double fTemp211 = (3.044454953660145e-18 * fTemp21);
			double fTemp212 = (7.285904672014723e-18 * fTemp9);
			double fTemp213 = (fRec1[0] * ((fTemp170 + (fTemp177 + (fTemp178 + (fTemp204 + (fTemp179 + (fTemp180 + fTemp205)))))) - (fTemp173 + (fTemp206 + (fTemp207 + (fTemp208 + (fTemp176 + (fTemp209 + (fTemp210 + (fTemp211 + fTemp212))))))))));
			fRec103[0] = max((fRec103[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp213))))));
			fbargraph45 = fRec103[0];
			output29[i] = (FAUSTFLOAT)fTemp213;
			double fTemp214 = (0.016998264729033888 * fTemp79);
			double fTemp215 = (0.007102817506892191 * fTemp83);
			double fTemp216 = (0.03007122090474782 * fTemp59);
			double fTemp217 = (0.021308452520676584 * fTemp48);
			double fTemp218 = (0.010535180889746094 * fTemp51);
			double fTemp219 = (0.02081853754944074 * fTemp5);
			double fTemp220 = (0.022664352972045144 * fTemp9);
			double fTemp221 = (0.028411270027568724 * fTemp21);
			double fTemp222 = (0.021308452520676542 * fTemp25);
			double fTemp223 = (0.010535180889746075 * fTemp33);
			double fTemp224 = (0.03160554266923824 * fTemp37);
			double fTemp225 = (fTemp223 + fTemp224);
			double fTemp226 = (0.005375323281425205 * fTemp55);
			double fTemp227 = (0.01612596984427559 * fTemp13);
			double fTemp228 = (0.01640325439843596 * fTemp29);
			double fTemp229 = (0.020481687709769415 * fTemp17);
			double fTemp230 = (fTemp228 + fTemp229);
			double fTemp231 = (fRec1[0] * ((fTemp170 + (fTemp214 + (fTemp215 + (fTemp216 + (fTemp217 + (fTemp218 + (fTemp219 + (fTemp220 + (fTemp221 + (fTemp222 + fTemp225)))))))))) - (fTemp226 + (fTemp227 + fTemp230))));
			fRec104[0] = max((fRec104[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp231))))));
			fbargraph46 = fRec104[0];
			output30[i] = (FAUSTFLOAT)fTemp231;
			double fTemp232 = (0.016998264729033867 * fTemp79);
			double fTemp233 = (0.021308452520676546 * fTemp83);
			double fTemp234 = (0.021308452520676556 * fTemp48);
			double fTemp235 = (0.03160554266923824 * fTemp51);
			double fTemp236 = (0.007102817506892183 * fTemp25);
			double fTemp237 = (0.01053518088974608 * fTemp37);
			double fTemp238 = (0.01612596984427559 * fTemp55);
			double fTemp239 = (0.020818537549440783 * fTemp59);
			double fTemp240 = (0.030071220904747797 * fTemp5);
			double fTemp241 = (0.022664352972045162 * fTemp9);
			double fTemp242 = (0.005375323281425198 * fTemp13);
			double fTemp243 = (0.02841127002756874 * fTemp21);
			double fTemp244 = (fRec1[0] * ((fTemp170 + (fTemp232 + (fTemp233 + (fTemp234 + (fTemp235 + (fTemp236 + (fTemp223 + fTemp237))))))) - (fTemp238 + (fTemp239 + (fTemp240 + (fTemp241 + (fTemp242 + (fTemp229 + (fTemp228 + fTemp243)))))))));
			fRec105[0] = max((fRec105[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp244))))));
			fbargraph47 = fRec105[0];
			output31[i] = (FAUSTFLOAT)fTemp244;
			double fTemp245 = (0.03007122090474779 * fTemp5);
			double fTemp246 = (0.005375323281425197 * fTemp13);
			double fTemp247 = (0.016998264729033864 * fTemp79);
			double fTemp248 = (0.016125969844275594 * fTemp55);
			double fTemp249 = (0.02081853754944079 * fTemp59);
			double fTemp250 = (0.007102817506892181 * fTemp25);
			double fTemp251 = (0.010535180889746079 * fTemp37);
			double fTemp252 = (0.028411270027568745 * fTemp21);
			double fTemp253 = (fRec1[0] * ((fTemp170 + (fTemp235 + (fTemp245 + (fTemp223 + fTemp246)))) - ((0.021308452520676553 * (fTemp48 - fTemp83)) + (fTemp247 + (fTemp248 + (fTemp249 + (fTemp241 + (fTemp229 + ((fTemp250 + (fTemp228 + fTemp251)) + fTemp252)))))))));
			fRec106[0] = max((fRec106[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp253))))));
			fbargraph48 = fRec106[0];
			output32[i] = (FAUSTFLOAT)fTemp253;
			double fTemp254 = (0.007102817506892193 * fTemp83);
			double fTemp255 = (0.03007122090474784 * fTemp59);
			double fTemp256 = (0.010535180889746096 * fTemp51);
			double fTemp257 = (0.022664352972045138 * fTemp9);
			double fTemp258 = (0.028411270027568717 * fTemp21);
			double fTemp259 = (0.005375323281425206 * fTemp55);
			double fTemp260 = (fTemp224 + fTemp228);
			double fTemp261 = (0.020818537549440724 * fTemp5);
			double fTemp262 = (fRec1[0] * (((fTemp254 + (fTemp255 + (fTemp256 + (fTemp257 + (fTemp227 + (fTemp223 + fTemp258)))))) + fTemp170) - (fTemp214 + (fTemp259 + (fTemp217 + ((fTemp229 + (fTemp222 + fTemp260)) + fTemp261))))));
			fRec107[0] = max((fRec107[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp262))))));
			fbargraph49 = fRec107[0];
			output33[i] = (FAUSTFLOAT)fTemp262;
			double fTemp263 = (0.01699826472903388 * fTemp79);
			double fTemp264 = (0.005375323281425202 * fTemp55);
			double fTemp265 = (0.02130845252067657 * fTemp48);
			double fTemp266 = (0.02266435297204515 * fTemp9);
			double fTemp267 = (0.02841127002756873 * fTemp21);
			double fTemp268 = (0.007102817506892187 * fTemp83);
			double fTemp269 = (0.01053518088974609 * fTemp51);
			double fTemp270 = (0.021308452520676546 * fTemp25);
			double fTemp271 = (0.020818537549440744 * fTemp5);
			double fTemp272 = (fRec1[0] * (((fTemp263 + (fTemp264 + (fTemp265 + (fTemp266 + (fTemp227 + (fTemp223 + fTemp267)))))) + fTemp170) - (fTemp268 + (fTemp216 + (fTemp269 + ((fTemp229 + (fTemp260 + fTemp270)) + fTemp271))))));
			fRec108[0] = max((fRec108[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp272))))));
			fbargraph50 = fRec108[0];
			output34[i] = (FAUSTFLOAT)fTemp272;
			double fTemp273 = (0.016998264729033874 * fTemp79);
			double fTemp274 = (0.020818537549440772 * fTemp59);
			double fTemp275 = (0.021308452520676563 * fTemp48);
			double fTemp276 = (0.0300712209047478 * fTemp5);
			double fTemp277 = (0.0053753232814252 * fTemp13);
			double fTemp278 = (0.022664352972045155 * fTemp9);
			double fTemp279 = (0.028411270027568734 * fTemp21);
			double fTemp280 = (0.010535180889746086 * fTemp37);
			double fTemp281 = (0.007102817506892185 * fTemp25);
			double fTemp282 = (fRec1[0] * ((fTemp170 + (fTemp273 + (fTemp238 + (fTemp274 + (fTemp275 + (fTemp276 + (fTemp223 + fTemp277))))))) - (fTemp233 + (fTemp235 + (fTemp278 + (fTemp229 + (fTemp279 + ((fTemp228 + fTemp280) + fTemp281))))))));
			fRec109[0] = max((fRec109[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp282))))));
			fbargraph51 = fRec109[0];
			output35[i] = (FAUSTFLOAT)fTemp282;
			double fTemp283 = (0.020818537549440803 * fTemp59);
			double fTemp284 = (0.0071028175068921775 * fTemp25);
			double fTemp285 = (0.010535180889746074 * fTemp37);
			double fTemp286 = (0.016998264729033857 * fTemp79);
			double fTemp287 = (0.021308452520676553 * fTemp83);
			double fTemp288 = (0.021308452520676542 * fTemp48);
			double fTemp289 = (0.005375323281425194 * fTemp13);
			double fTemp290 = (0.028411270027568748 * fTemp21);
			double fTemp291 = (0.022664352972045165 * fTemp9);
			double fTemp292 = (0.03007122090474778 * fTemp5);
			double fTemp293 = (fRec1[0] * ((fTemp170 + (fTemp248 + (fTemp283 + (fTemp284 + (fTemp223 + fTemp285))))) - (fTemp286 + (fTemp287 + (fTemp288 + (fTemp235 + (((fTemp289 + (fTemp229 + (fTemp228 + fTemp290))) + fTemp291) + fTemp292)))))));
			fRec110[0] = max((fRec110[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp293))))));
			fbargraph52 = fRec110[0];
			output36[i] = (FAUSTFLOAT)fTemp293;
			double fTemp294 = (0.005375323281425214 * fTemp55);
			double fTemp295 = (0.02081853754944071 * fTemp5);
			double fTemp296 = (0.02266435297204512 * fTemp9);
			double fTemp297 = (0.028411270027568696 * fTemp21);
			double fTemp298 = (0.02130845252067654 * fTemp25);
			double fTemp299 = (0.03160554266923823 * fTemp37);
			double fTemp300 = (0.016998264729033916 * fTemp79);
			double fTemp301 = (0.007102817506892205 * fTemp83);
			double fTemp302 = (0.030071220904747842 * fTemp59);
			double fTemp303 = (0.021308452520676615 * fTemp48);
			double fTemp304 = (0.010535180889746113 * fTemp51);
			double fTemp305 = (0.016125969844275587 * fTemp13);
			double fTemp306 = (fRec1[0] * ((fTemp170 + (fTemp294 + (fTemp295 + (fTemp296 + (fTemp297 + (fTemp298 + (fTemp223 + fTemp299))))))) - (fTemp300 + (fTemp301 + (fTemp302 + (fTemp303 + (fTemp304 + (fTemp230 + fTemp305))))))));
			fRec111[0] = max((fRec111[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp306))))));
			fbargraph53 = fRec111[0];
			output37[i] = (FAUSTFLOAT)fTemp306;
			double fTemp307 = (fTemp223 + fTemp228);
			double fTemp308 = (fRec1[0] * ((fTemp170 + (fTemp216 + (fTemp217 + (fTemp218 + (fTemp219 + (fTemp229 + (fTemp221 + fTemp224))))))) - (fTemp214 + (fTemp215 + (fTemp226 + (fTemp220 + (fTemp227 + (fTemp222 + fTemp307))))))));
			fRec112[0] = max((fRec112[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp308))))));
			fbargraph54 = fRec112[0];
			output38[i] = (FAUSTFLOAT)fTemp308;
			double fTemp309 = (fRec1[0] * ((fTemp170 + (fTemp234 + (fTemp235 + (fTemp241 + (fTemp229 + fTemp237))))) - (fTemp232 + (fTemp233 + (fTemp238 + (fTemp239 + (fTemp240 + (fTemp242 + (fTemp243 + (fTemp236 + fTemp307))))))))));
			fRec113[0] = max((fRec113[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp309))))));
			fbargraph55 = fRec113[0];
			output39[i] = (FAUSTFLOAT)fTemp309;
			double fTemp310 = (fRec1[0] * ((fTemp170 + (fTemp247 + (fTemp235 + (fTemp245 + (fTemp241 + (fTemp246 + (fTemp229 + fTemp250))))))) - ((0.021308452520676553 * (fTemp48 + fTemp83)) + (fTemp248 + (fTemp249 + (fTemp252 + (fTemp228 + (fTemp223 + fTemp251))))))));
			fRec114[0] = max((fRec114[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp310))))));
			fbargraph56 = fRec114[0];
			output40[i] = (FAUSTFLOAT)fTemp310;
			double fTemp311 = (fTemp228 + fTemp225);
			double fTemp312 = (fRec1[0] * ((fTemp170 + (fTemp214 + (fTemp255 + (fTemp256 + (fTemp227 + (fTemp229 + (fTemp222 + fTemp258))))))) - (fTemp254 + (fTemp259 + (fTemp217 + (fTemp261 + (fTemp257 + fTemp311)))))));
			fRec115[0] = max((fRec115[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp312))))));
			fbargraph57 = fRec115[0];
			output41[i] = (FAUSTFLOAT)fTemp312;
			double fTemp313 = (fRec1[0] * ((fTemp170 + (fTemp268 + (fTemp264 + (fTemp265 + (fTemp227 + (fTemp229 + (fTemp267 + fTemp270))))))) - (fTemp263 + (fTemp216 + (fTemp269 + (fTemp271 + (fTemp266 + fTemp311)))))));
			fRec116[0] = max((fRec116[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp313))))));
			fbargraph58 = fRec116[0];
			output42[i] = (FAUSTFLOAT)fTemp313;
			double fTemp314 = (fRec1[0] * ((fTemp170 + (fTemp233 + (fTemp238 + (fTemp274 + (fTemp275 + (fTemp276 + (fTemp278 + (fTemp277 + (fTemp229 + fTemp281))))))))) - (fTemp273 + (fTemp235 + (fTemp279 + (fTemp228 + (fTemp223 + fTemp280)))))));
			fRec117[0] = max((fRec117[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp314))))));
			fbargraph59 = fRec117[0];
			output43[i] = (FAUSTFLOAT)fTemp314;
			double fTemp315 = (fRec1[0] * ((fTemp170 + (fTemp286 + (fTemp287 + (fTemp248 + (fTemp283 + (fTemp291 + (fTemp229 + fTemp285))))))) - (fTemp288 + (fTemp235 + (fTemp292 + (fTemp289 + (fTemp290 + (fTemp284 + fTemp307))))))));
			fRec118[0] = max((fRec118[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp315))))));
			fbargraph60 = fRec118[0];
			output44[i] = (FAUSTFLOAT)fTemp315;
			double fTemp316 = (fRec1[0] * ((fTemp170 + (fTemp300 + (fTemp301 + (fTemp294 + (fTemp295 + (fTemp229 + (fTemp299 + fTemp297))))))) - (fTemp302 + (fTemp303 + (fTemp304 + (fTemp296 + (fTemp305 + (fTemp298 + fTemp307))))))));
			fRec119[0] = max((fRec119[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp316))))));
			fbargraph61 = fRec119[0];
			output45[i] = (FAUSTFLOAT)fTemp316;
			double fTemp317 = (fRec1[0] * ((fTemp170 + (fTemp172 + (fTemp174 + (fTemp175 + (fTemp176 + (fTemp185 + (fTemp179 + fTemp177))))))) - (fTemp171 + (fTemp173 + (fTemp183 + (fTemp178 + (fTemp180 + fTemp184)))))));
			fRec120[0] = max((fRec120[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp317))))));
			fbargraph62 = fRec120[0];
			output46[i] = (FAUSTFLOAT)fTemp317;
			double fTemp318 = (fRec1[0] * ((fTemp170 + (fTemp188 + (fTemp173 + (fTemp174 + (fTemp175 + (fTemp187 + (fTemp179 + fTemp190))))))) - (fTemp189 + (fTemp176 + (fTemp177 + (fTemp178 + (fTemp180 + fTemp191)))))));
			fRec121[0] = max((fRec121[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp318))))));
			fbargraph63 = fRec121[0];
			output47[i] = (FAUSTFLOAT)fTemp318;
			double fTemp319 = (fRec1[0] * ((fTemp170 + (fTemp196 + (fTemp176 + (fTemp193 + (fTemp195 + (fTemp179 + fTemp201)))))) - (fTemp173 + (fTemp197 + (fTemp198 + (fTemp199 + (fTemp194 + (fTemp202 + (fTemp178 + (fTemp180 + fTemp200))))))))));
			fRec122[0] = max((fRec122[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp319))))));
			fbargraph64 = fRec122[0];
			output48[i] = (FAUSTFLOAT)fTemp319;
			double fTemp320 = (fRec1[0] * ((fTemp170 + (fTemp173 + (fTemp206 + (fTemp212 + (fTemp177 + (fTemp179 + fTemp205)))))) - (fTemp207 + (fTemp208 + (fTemp176 + (fTemp209 + (fTemp210 + (fTemp178 + (fTemp211 + (fTemp180 + fTemp204))))))))));
			fRec123[0] = max((fRec123[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp320))))));
			fbargraph65 = fRec123[0];
			output49[i] = (FAUSTFLOAT)fTemp320;
			// post processing
			fRec123[1] = fRec123[0];
			fRec122[1] = fRec122[0];
			fRec121[1] = fRec121[0];
			fRec120[1] = fRec120[0];
			fRec119[1] = fRec119[0];
			fRec118[1] = fRec118[0];
			fRec117[1] = fRec117[0];
			fRec116[1] = fRec116[0];
			fRec115[1] = fRec115[0];
			fRec114[1] = fRec114[0];
			fRec113[1] = fRec113[0];
			fRec112[1] = fRec112[0];
			fRec111[1] = fRec111[0];
			fRec110[1] = fRec110[0];
			fRec109[1] = fRec109[0];
			fRec108[1] = fRec108[0];
			fRec107[1] = fRec107[0];
			fRec106[1] = fRec106[0];
			fRec105[1] = fRec105[0];
			fRec104[1] = fRec104[0];
			fRec103[1] = fRec103[0];
			fRec102[1] = fRec102[0];
			fRec101[1] = fRec101[0];
			fRec100[1] = fRec100[0];
			fRec99[1] = fRec99[0];
			fRec98[1] = fRec98[0];
			fRec97[1] = fRec97[0];
			fRec96[1] = fRec96[0];
			fRec95[1] = fRec95[0];
			fRec94[1] = fRec94[0];
			fRec93[1] = fRec93[0];
			fRec92[1] = fRec92[0];
			fRec91[1] = fRec91[0];
			fRec90[1] = fRec90[0];
			fRec89[1] = fRec89[0];
			fRec88[1] = fRec88[0];
			fRec87[1] = fRec87[0];
			fRec86[1] = fRec86[0];
			fRec85[1] = fRec85[0];
			fRec84[1] = fRec84[0];
			fRec83[1] = fRec83[0];
			fRec82[1] = fRec82[0];
			fRec71[1] = fRec71[0];
			fRec78[1] = fRec78[0];
			fRec79[1] = fRec79[0];
			fRec81[1] = fRec81[0];
			fRec80[1] = fRec80[0];
			fRec72[1] = fRec72[0];
			fRec73[1] = fRec73[0];
			fRec77[1] = fRec77[0];
			fRec74[1] = fRec74[0];
			fRec76[1] = fRec76[0];
			fRec75[1] = fRec75[0];
			fRec70[1] = fRec70[0];
			fRec69[1] = fRec69[0];
			fRec68[1] = fRec68[0];
			fRec67[1] = fRec67[0];
			fRec47[1] = fRec47[0];
			fRec61[1] = fRec61[0];
			fRec62[1] = fRec62[0];
			fRec66[1] = fRec66[0];
			fRec63[1] = fRec63[0];
			fRec65[1] = fRec65[0];
			fRec64[1] = fRec64[0];
			fRec55[1] = fRec55[0];
			fRec56[1] = fRec56[0];
			fRec60[1] = fRec60[0];
			fRec57[1] = fRec57[0];
			fRec59[1] = fRec59[0];
			fRec58[1] = fRec58[0];
			fRec52[1] = fRec52[0];
			fRec53[1] = fRec53[0];
			fRec54[1] = fRec54[0];
			fRec48[1] = fRec48[0];
			fRec49[1] = fRec49[0];
			fRec51[1] = fRec51[0];
			fRec50[1] = fRec50[0];
			fRec46[1] = fRec46[0];
			fRec0[1] = fRec0[0];
			fRec43[1] = fRec43[0];
			fRec44[1] = fRec44[0];
			fRec45[1] = fRec45[0];
			fRec40[1] = fRec40[0];
			fRec41[1] = fRec41[0];
			fRec42[1] = fRec42[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec39[1] = fRec39[0];
			fRec38[1] = fRec38[0];
			fRec32[1] = fRec32[0];
			fRec33[1] = fRec33[0];
			fRec35[1] = fRec35[0];
			fRec34[1] = fRec34[0];
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			fRec27[1] = fRec27[0];
			fRec24[1] = fRec24[0];
			fRec26[1] = fRec26[0];
			fRec25[1] = fRec25[0];
			fRec16[1] = fRec16[0];
			fRec17[1] = fRec17[0];
			fRec21[1] = fRec21[0];
			fRec18[1] = fRec18[0];
			fRec20[1] = fRec20[0];
			fRec19[1] = fRec19[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			fRec15[1] = fRec15[0];
			fRec12[1] = fRec12[0];
			fRec14[1] = fRec14[0];
			fRec13[1] = fRec13[0];
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec9[1] = fRec9[0];
			fRec6[1] = fRec6[0];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
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
