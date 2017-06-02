//----------------------------------------------------------
// name: "HOAPanLebedev505"
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
	FAUSTFLOAT 	fcheckbox0;
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fslider1;
	double 	fConst2;
	FAUSTFLOAT 	fentry0;
	double 	fConst3;
	FAUSTFLOAT 	fslider2;
	double 	fConst4;
	FAUSTFLOAT 	fcheckbox1;
	FAUSTFLOAT 	fslider3;
	double 	fRec3[2];
	double 	fConst5;
	double 	fRec4[2];
	double 	fRec5[2];
	double 	fRec2[2];
	double 	fRec6[2];
	double 	fVec0[2];
	double 	fRec7[2];
	double 	fConst6;
	double 	fRec8[2];
	double 	fRec9[2];
	double 	fRec1[2];
	double 	fRec10[2];
	double 	fVec1[2];
	double 	fRec11[2];
	double 	fRec13[2];
	double 	fRec14[2];
	double 	fRec16[2];
	double 	fRec17[2];
	double 	fRec15[2];
	double 	fRec12[2];
	FAUSTFLOAT 	fslider4;
	FAUSTFLOAT 	fslider5;
	double 	fRec19[2];
	double 	fRec20[2];
	FAUSTFLOAT 	fslider6;
	double 	fRec22[2];
	double 	fRec23[2];
	FAUSTFLOAT 	fcheckbox2;
	FAUSTFLOAT 	fslider7;
	double 	fRec24[2];
	double 	fRec21[2];
	double 	fRec25[2];
	double 	fVec2[2];
	double 	fRec26[2];
	double 	fRec18[2];
	double 	fRec27[2];
	double 	fVec3[2];
	double 	fRec28[2];
	double 	fRec30[2];
	double 	fRec31[2];
	double 	fRec33[2];
	double 	fRec34[2];
	double 	fRec32[2];
	double 	fRec29[2];
	FAUSTFLOAT 	fcheckbox3;
	double 	fConst7;
	double 	fConst8;
	double 	fRec36[2];
	double 	fRec37[2];
	double 	fRec35[2];
	double 	fRec38[2];
	double 	fVec4[2];
	double 	fRec39[2];
	double 	fRec41[2];
	double 	fRec42[2];
	double 	fRec40[2];
	double 	fRec44[2];
	double 	fRec45[2];
	double 	fRec43[2];
	double 	fRec46[2];
	double 	fVec5[2];
	double 	fRec47[2];
	double 	fRec49[2];
	double 	fRec50[2];
	double 	fRec48[2];
	FAUSTFLOAT 	fcheckbox4;
	FAUSTFLOAT 	fcheckbox5;
	double 	fConst9;
	double 	fRec53[2];
	double 	fRec52[2];
	double 	fRec51[2];
	double 	fRec55[2];
	double 	fRec54[2];
	FAUSTFLOAT 	fcheckbox6;
	double 	fConst10;
	double 	fConst11;
	double 	fConst12;
	double 	fConst13;
	double 	fRec59[2];
	double 	fRec60[2];
	double 	fRec58[2];
	double 	fRec61[2];
	double 	fVec6[2];
	double 	fRec62[2];
	double 	fRec63[2];
	double 	fRec57[2];
	double 	fRec56[2];
	double 	fRec66[2];
	double 	fRec67[2];
	double 	fRec65[2];
	double 	fRec68[2];
	double 	fRec64[2];
	FAUSTFLOAT 	fcheckbox7;
	double 	fConst14;
	double 	fConst15;
	double 	fConst16;
	double 	fConst17;
	double 	fConst18;
	double 	fRec73[2];
	double 	fRec74[2];
	double 	fRec72[2];
	double 	fRec75[2];
	double 	fVec7[2];
	double 	fRec76[2];
	double 	fConst19;
	double 	fRec77[2];
	double 	fRec78[2];
	double 	fRec71[2];
	double 	fRec79[2];
	double 	fVec8[2];
	double 	fRec80[2];
	double 	fRec81[2];
	double 	fRec70[2];
	double 	fRec69[2];
	double 	fRec84[2];
	double 	fRec85[2];
	double 	fRec87[2];
	double 	fRec88[2];
	double 	fRec86[2];
	double 	fRec83[2];
	double 	fRec89[2];
	double 	fRec82[2];
	double 	fRec92[2];
	double 	fRec91[2];
	double 	fRec90[2];
	double 	fRec94[2];
	double 	fRec93[2];
	double 	fRec98[2];
	double 	fRec99[2];
	double 	fRec97[2];
	double 	fRec100[2];
	double 	fVec9[2];
	double 	fRec101[2];
	double 	fRec102[2];
	double 	fRec96[2];
	double 	fRec95[2];
	double 	fRec105[2];
	double 	fRec106[2];
	double 	fRec104[2];
	double 	fRec107[2];
	double 	fRec103[2];
	double 	fRec111[2];
	double 	fRec112[2];
	double 	fRec114[2];
	double 	fRec115[2];
	double 	fRec113[2];
	double 	fRec116[2];
	double 	fVec10[2];
	double 	fRec117[2];
	double 	fRec110[2];
	double 	fRec118[2];
	double 	fVec11[2];
	double 	fRec119[2];
	double 	fRec120[2];
	double 	fRec109[2];
	double 	fRec108[2];
	double 	fRec123[2];
	double 	fRec124[2];
	double 	fRec126[2];
	double 	fRec127[2];
	double 	fRec125[2];
	double 	fRec122[2];
	double 	fRec128[2];
	double 	fRec121[2];
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fRec129[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec130[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec131[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fRec132[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec133[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec134[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec135[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec136[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec137[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec138[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec139[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec140[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec141[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec142[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec143[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec144[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec145[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec146[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec147[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec148[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec149[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec150[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec151[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec152[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec153[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec154[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec155[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec156[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec157[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec158[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec159[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec160[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec161[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec162[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec163[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec164[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec165[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec166[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec167[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec168[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec169[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec170[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec171[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec172[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec173[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec174[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec175[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec176[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec177[2];
	FAUSTFLOAT 	fbargraph49;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOAPanLebedev505");
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

	virtual int getNumInputs() { return 2; }
	virtual int getNumOutputs() { return 50; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(1.92e+05, max(1.0, (double)fSamplingFreq));
		fConst1 = (8e+01 / fConst0);
		fConst2 = (1.7e+02 / fConst0);
		fConst3 = (984.7116049589241 / fConst0);
		fConst4 = (715.2883950410759 / fConst0);
		fConst5 = (1430.5767900821518 / fConst0);
		fConst6 = (1969.4232099178482 / fConst0);
		fConst7 = (5.1e+02 / fConst0);
		fConst8 = (1.02e+03 / fConst0);
		fConst9 = (3.4e+02 / fConst0);
		fConst10 = (789.5430205728705 / fConst0);
		fConst11 = (394.7715102864353 / fConst0);
		fConst12 = (625.2284897135664 / fConst0);
		fConst13 = (1250.4569794271329 / fConst0);
		fConst14 = (1239.8911224120607 / fConst0);
		fConst15 = (619.9455612060303 / fConst0);
		fConst16 = (1139.6651757122002 / fConst0);
		fConst17 = (790.3892630817644 / fConst0);
		fConst18 = (1580.7785261635288 / fConst0);
		fConst19 = (2279.3303514244003 / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fcheckbox0 = 0.0;
		fslider0 = 0.0;
		fslider1 = 0.0;
		fentry0 = 1.07;
		fslider2 = 2.0;
		fcheckbox1 = 0.0;
		fslider3 = 0.0;
		fslider4 = 0.0;
		fslider5 = 0.0;
		fslider6 = 2.0;
		fcheckbox2 = 0.0;
		fslider7 = 0.0;
		fcheckbox3 = 0.0;
		fcheckbox4 = 0.0;
		fcheckbox5 = 0.0;
		fcheckbox6 = 0.0;
		fcheckbox7 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fVec0[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<2; i++) fRec88[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fRec89[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec92[i] = 0;
		for (int i=0; i<2; i++) fRec91[i] = 0;
		for (int i=0; i<2; i++) fRec90[i] = 0;
		for (int i=0; i<2; i++) fRec94[i] = 0;
		for (int i=0; i<2; i++) fRec93[i] = 0;
		for (int i=0; i<2; i++) fRec98[i] = 0;
		for (int i=0; i<2; i++) fRec99[i] = 0;
		for (int i=0; i<2; i++) fRec97[i] = 0;
		for (int i=0; i<2; i++) fRec100[i] = 0;
		for (int i=0; i<2; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec101[i] = 0;
		for (int i=0; i<2; i++) fRec102[i] = 0;
		for (int i=0; i<2; i++) fRec96[i] = 0;
		for (int i=0; i<2; i++) fRec95[i] = 0;
		for (int i=0; i<2; i++) fRec105[i] = 0;
		for (int i=0; i<2; i++) fRec106[i] = 0;
		for (int i=0; i<2; i++) fRec104[i] = 0;
		for (int i=0; i<2; i++) fRec107[i] = 0;
		for (int i=0; i<2; i++) fRec103[i] = 0;
		for (int i=0; i<2; i++) fRec111[i] = 0;
		for (int i=0; i<2; i++) fRec112[i] = 0;
		for (int i=0; i<2; i++) fRec114[i] = 0;
		for (int i=0; i<2; i++) fRec115[i] = 0;
		for (int i=0; i<2; i++) fRec113[i] = 0;
		for (int i=0; i<2; i++) fRec116[i] = 0;
		for (int i=0; i<2; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec117[i] = 0;
		for (int i=0; i<2; i++) fRec110[i] = 0;
		for (int i=0; i<2; i++) fRec118[i] = 0;
		for (int i=0; i<2; i++) fVec11[i] = 0;
		for (int i=0; i<2; i++) fRec119[i] = 0;
		for (int i=0; i<2; i++) fRec120[i] = 0;
		for (int i=0; i<2; i++) fRec109[i] = 0;
		for (int i=0; i<2; i++) fRec108[i] = 0;
		for (int i=0; i<2; i++) fRec123[i] = 0;
		for (int i=0; i<2; i++) fRec124[i] = 0;
		for (int i=0; i<2; i++) fRec126[i] = 0;
		for (int i=0; i<2; i++) fRec127[i] = 0;
		for (int i=0; i<2; i++) fRec125[i] = 0;
		for (int i=0; i<2; i++) fRec122[i] = 0;
		for (int i=0; i<2; i++) fRec128[i] = 0;
		for (int i=0; i<2; i++) fRec121[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec129[i] = 0;
		for (int i=0; i<2; i++) fRec130[i] = 0;
		for (int i=0; i<2; i++) fRec131[i] = 0;
		for (int i=0; i<2; i++) fRec132[i] = 0;
		for (int i=0; i<2; i++) fRec133[i] = 0;
		for (int i=0; i<2; i++) fRec134[i] = 0;
		for (int i=0; i<2; i++) fRec135[i] = 0;
		for (int i=0; i<2; i++) fRec136[i] = 0;
		for (int i=0; i<2; i++) fRec137[i] = 0;
		for (int i=0; i<2; i++) fRec138[i] = 0;
		for (int i=0; i<2; i++) fRec139[i] = 0;
		for (int i=0; i<2; i++) fRec140[i] = 0;
		for (int i=0; i<2; i++) fRec141[i] = 0;
		for (int i=0; i<2; i++) fRec142[i] = 0;
		for (int i=0; i<2; i++) fRec143[i] = 0;
		for (int i=0; i<2; i++) fRec144[i] = 0;
		for (int i=0; i<2; i++) fRec145[i] = 0;
		for (int i=0; i<2; i++) fRec146[i] = 0;
		for (int i=0; i<2; i++) fRec147[i] = 0;
		for (int i=0; i<2; i++) fRec148[i] = 0;
		for (int i=0; i<2; i++) fRec149[i] = 0;
		for (int i=0; i<2; i++) fRec150[i] = 0;
		for (int i=0; i<2; i++) fRec151[i] = 0;
		for (int i=0; i<2; i++) fRec152[i] = 0;
		for (int i=0; i<2; i++) fRec153[i] = 0;
		for (int i=0; i<2; i++) fRec154[i] = 0;
		for (int i=0; i<2; i++) fRec155[i] = 0;
		for (int i=0; i<2; i++) fRec156[i] = 0;
		for (int i=0; i<2; i++) fRec157[i] = 0;
		for (int i=0; i<2; i++) fRec158[i] = 0;
		for (int i=0; i<2; i++) fRec159[i] = 0;
		for (int i=0; i<2; i++) fRec160[i] = 0;
		for (int i=0; i<2; i++) fRec161[i] = 0;
		for (int i=0; i<2; i++) fRec162[i] = 0;
		for (int i=0; i<2; i++) fRec163[i] = 0;
		for (int i=0; i<2; i++) fRec164[i] = 0;
		for (int i=0; i<2; i++) fRec165[i] = 0;
		for (int i=0; i<2; i++) fRec166[i] = 0;
		for (int i=0; i<2; i++) fRec167[i] = 0;
		for (int i=0; i<2; i++) fRec168[i] = 0;
		for (int i=0; i<2; i++) fRec169[i] = 0;
		for (int i=0; i<2; i++) fRec170[i] = 0;
		for (int i=0; i<2; i++) fRec171[i] = 0;
		for (int i=0; i<2; i++) fRec172[i] = 0;
		for (int i=0; i<2; i++) fRec173[i] = 0;
		for (int i=0; i<2; i++) fRec174[i] = 0;
		for (int i=0; i<2; i++) fRec175[i] = 0;
		for (int i=0; i<2; i++) fRec176[i] = 0;
		for (int i=0; i<2; i++) fRec177[i] = 0;
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
		ui_interface->declare(&fslider1, "0+3", "");
		ui_interface->declare(&fslider1, "osc", "/azimuth_0 0 360");
		ui_interface->declare(&fslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth  0", &fslider1, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(&fslider0, "0+4", "");
		ui_interface->declare(&fslider0, "osc", "/elevation_0 -90 90");
		ui_interface->declare(&fslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation  0", &fslider0, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(0, "0+5", "");
		ui_interface->openHorizontalBox("Spherical Wave");
		ui_interface->addCheckButton("Yes", &fcheckbox1);
		ui_interface->closeBox();
		ui_interface->declare(&fslider7, "1+1", "");
		ui_interface->declare(&fslider7, "osc", "/gain_1 -20 20");
		ui_interface->declare(&fslider7, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  1", &fslider7, 0.0, -3e+01, 2e+01, 0.1);
		ui_interface->declare(&fslider6, "1+2", "");
		ui_interface->declare(&fslider6, "osc", "/radius_1 0.5 50");
		ui_interface->declare(&fslider6, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  1", &fslider6, 2.0, 0.5, 5e+01, 0.01);
		ui_interface->declare(&fslider5, "1+3", "");
		ui_interface->declare(&fslider5, "osc", "/azimuth_1 0 360");
		ui_interface->declare(&fslider5, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth  1", &fslider5, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(&fslider4, "1+4", "");
		ui_interface->declare(&fslider4, "osc", "/elevation_1 -90 90");
		ui_interface->declare(&fslider4, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation  1", &fslider4, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(0, "1+5", "");
		ui_interface->openHorizontalBox("Spherical Wave");
		ui_interface->addCheckButton("Yes", &fcheckbox2);
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openVerticalBox("Mute Order");
		ui_interface->addCheckButton("0", &fcheckbox4);
		ui_interface->addCheckButton("1", &fcheckbox5);
		ui_interface->addCheckButton("2", &fcheckbox3);
		ui_interface->addCheckButton("3", &fcheckbox6);
		ui_interface->addCheckButton("4", &fcheckbox0);
		ui_interface->addCheckButton("5", &fcheckbox7);
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
		ui_interface->addVerticalBargraph("0x7f9b6e4152b0", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph1, "osc", "/output2");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e41a390", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph2, "osc", "/output3");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e41f700", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph3, "osc", "/output4");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e424e70", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph4, "osc", "/output5");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e42a480", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph5, "osc", "/output6");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e42f510", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph6, "osc", "/output7");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e434dc0", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph7, "osc", "/output8");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e43a200", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph8, "osc", "/output9");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e43f670", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph9, "osc", "/output10");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e444b40", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph10, "osc", "/output11");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e44a360", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph11, "osc", "/output12");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e44fc00", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph12, "osc", "/output13");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e455510", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph13, "osc", "/output14");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e45ae70", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph14, "osc", "/output15");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e460270", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph15, "osc", "/output16");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e4656c0", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph16, "osc", "/output17");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e46aba0", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph17, "osc", "/output18");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e4700e0", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph18, "osc", "/output19");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e475e00", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph19, "osc", "/output20");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e47b6c0", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph20, "osc", "/output21");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e480fb0", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph21, "osc", "/output22");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e1511a0", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph22, "osc", "/output23");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e1568d0", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph23, "osc", "/output24");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e15c0a0", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph24, "osc", "/output25");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e1618f0", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph25, "osc", "/output26");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e2d6f80", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph26, "osc", "/output27");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e2f6eb0", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph27, "osc", "/output28");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e2fbf50", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph28, "osc", "/output29");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e505580", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph29, "osc", "/output30");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e50a9b0", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph30, "osc", "/output31");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e510870", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph31, "osc", "/output32");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e516370", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph32, "osc", "/output33");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e51bea0", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph33, "osc", "/output34");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e521a30", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph34, "osc", "/output35");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e527620", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fbargraph35, "osc", "/output36");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e52d270", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fbargraph36, "osc", "/output37");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e532ef0", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fbargraph37, "osc", "/output38");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e538bd0", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fbargraph38, "osc", "/output39");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e53e090", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fbargraph39, "osc", "/output40");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e5435b0", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fbargraph40, "osc", "/output41");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e548b30", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fbargraph41, "osc", "/output42");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e54e110", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fbargraph42, "osc", "/output43");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e5535b0", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fbargraph43, "osc", "/output44");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e558e50", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fbargraph44, "osc", "/output45");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e55e540", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fbargraph45, "osc", "/output46");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e563d00", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fbargraph46, "osc", "/output47");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e5694d0", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fbargraph47, "osc", "/output48");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e56ed10", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fbargraph48, "osc", "/output49");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e574620", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fbargraph49, "osc", "/output50");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9b6e579f40", &fbargraph49, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = (1.125 * (1 - double(fcheckbox0)));
		double 	fSlow1 = double(fslider0);
		double 	fSlow2 = sin(fSlow1);
		double 	fSlow3 = cos(fSlow1);
		double 	fSlow4 = double(fslider1);
		double 	fSlow5 = (fSlow3 * cos(fSlow4));
		double 	fSlow6 = (6.123233995736766e-17 * fSlow5);
		double 	fSlow7 = (fSlow2 + fSlow6);
		double 	fSlow8 = faustpower<2>(fSlow7);
		double 	fSlow9 = ((fSlow8 * ((35 * fSlow8) + -30)) + 3);
		double 	fSlow10 = double(fentry0);
		double 	fSlow11 = (fConst2 / fSlow10);
		double 	fSlow12 = faustpower<2>(fSlow11);
		double 	fSlow13 = (1.0 / (((9.1401308902779 * fSlow12) + (fConst3 / fSlow10)) + 1));
		double 	fSlow14 = double(fslider2);
		double 	fSlow15 = (fConst2 / fSlow14);
		double 	fSlow16 = faustpower<2>(fSlow15);
		double 	fSlow17 = (((fConst3 / fSlow14) + (9.1401308902779 * fSlow16)) + 1);
		double 	fSlow18 = (1.0 / (((fConst4 / fSlow10) + (11.4878004768713 * fSlow12)) + 1));
		double 	fSlow19 = double(fcheckbox1);
		double 	fSlow20 = (fSlow19 * fSlow10);
		double 	fSlow21 = (((11.4878004768713 * fSlow16) + (fConst4 / fSlow14)) + 1);
		double 	fSlow22 = ((fSlow20 * fSlow21) / fSlow14);
		double 	fSlow23 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider3))));
		double 	fSlow24 = (45.9512019074852 * fSlow12);
		double 	fSlow25 = (fSlow24 + (fConst5 / fSlow10));
		double 	fSlow26 = (1.0 / fSlow21);
		double 	fSlow27 = (45.9512019074852 * fSlow16);
		double 	fSlow28 = (fSlow27 + (fConst5 / fSlow14));
		double 	fSlow29 = (36.5605235611116 * fSlow12);
		double 	fSlow30 = (fSlow29 + (fConst6 / fSlow10));
		double 	fSlow31 = (1.0 / fSlow17);
		double 	fSlow32 = (36.5605235611116 * fSlow16);
		double 	fSlow33 = ((fConst6 / fSlow14) + fSlow32);
		double 	fSlow34 = ((1 - fSlow19) * fSlow10);
		double 	fSlow35 = double(fslider4);
		double 	fSlow36 = sin(fSlow35);
		double 	fSlow37 = cos(fSlow35);
		double 	fSlow38 = double(fslider5);
		double 	fSlow39 = (fSlow37 * cos(fSlow38));
		double 	fSlow40 = (6.123233995736766e-17 * fSlow39);
		double 	fSlow41 = (fSlow36 + fSlow40);
		double 	fSlow42 = faustpower<2>(fSlow41);
		double 	fSlow43 = ((fSlow42 * ((35 * fSlow42) + -30)) + 3);
		double 	fSlow44 = double(fslider6);
		double 	fSlow45 = (fConst2 / fSlow44);
		double 	fSlow46 = faustpower<2>(fSlow45);
		double 	fSlow47 = (((fConst3 / fSlow44) + (9.1401308902779 * fSlow46)) + 1);
		double 	fSlow48 = double(fcheckbox2);
		double 	fSlow49 = (fSlow10 * fSlow48);
		double 	fSlow50 = (((11.4878004768713 * fSlow46) + (fConst4 / fSlow44)) + 1);
		double 	fSlow51 = ((fSlow49 * fSlow50) / fSlow44);
		double 	fSlow52 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider7))));
		double 	fSlow53 = (1.0 / fSlow50);
		double 	fSlow54 = (45.9512019074852 * fSlow46);
		double 	fSlow55 = ((fConst5 / fSlow44) + fSlow54);
		double 	fSlow56 = (1.0 / fSlow47);
		double 	fSlow57 = (36.5605235611116 * fSlow46);
		double 	fSlow58 = ((fConst6 / fSlow44) + fSlow57);
		double 	fSlow59 = (fSlow10 * (1 - fSlow48));
		double 	fSlow60 = (2.5 * (1 - double(fcheckbox3)));
		double 	fSlow61 = ((3 * fSlow42) + -1);
		double 	fSlow62 = (1.0 / (((fConst7 / fSlow10) + (3 * fSlow12)) + 1));
		double 	fSlow63 = (12 * fSlow12);
		double 	fSlow64 = (fSlow63 + (fConst8 / fSlow10));
		double 	fSlow65 = (((fConst7 / fSlow44) + (3 * fSlow46)) + 1);
		double 	fSlow66 = ((fSlow49 * fSlow65) / fSlow44);
		double 	fSlow67 = (1.0 / fSlow65);
		double 	fSlow68 = (12 * fSlow46);
		double 	fSlow69 = ((fConst8 / fSlow44) + fSlow68);
		double 	fSlow70 = ((3 * fSlow8) + -1);
		double 	fSlow71 = (((fConst7 / fSlow14) + (3 * fSlow16)) + 1);
		double 	fSlow72 = ((fSlow20 * fSlow71) / fSlow14);
		double 	fSlow73 = (1.0 / fSlow71);
		double 	fSlow74 = (12 * fSlow16);
		double 	fSlow75 = ((fConst8 / fSlow14) + fSlow74);
		double 	fSlow76 = (fSlow10 * (1 - double(fcheckbox4)));
		double 	fSlow77 = (1 - (fSlow19 * (1 - (1.0 / fSlow14))));
		double 	fSlow78 = (1 - (fSlow48 * (1 - (1.0 / fSlow44))));
		double 	fSlow79 = (3 * (1 - double(fcheckbox5)));
		double 	fSlow80 = (fSlow15 + 1);
		double 	fSlow81 = (fConst9 / (fSlow14 * fSlow80));
		double 	fSlow82 = (1.0 / (fSlow11 + 1));
		double 	fSlow83 = ((fSlow20 * fSlow80) / fSlow14);
		double 	fSlow84 = (fConst9 / fSlow10);
		double 	fSlow85 = (1 - double(fcheckbox6));
		double 	fSlow86 = (3.5 * (fSlow85 * ((5 * fSlow8) + -3)));
		double 	fSlow87 = ((fConst11 / fSlow14) + 1);
		double 	fSlow88 = (fConst10 / (fSlow14 * fSlow87));
		double 	fSlow89 = (1.0 / ((fConst11 / fSlow10) + 1));
		double 	fSlow90 = (1.0 / (((fConst12 / fSlow10) + (6.45943269348338 * fSlow12)) + 1));
		double 	fSlow91 = (((fConst12 / fSlow14) + (6.45943269348338 * fSlow16)) + 1);
		double 	fSlow92 = ((fSlow20 * fSlow91) / fSlow14);
		double 	fSlow93 = (25.83773077393352 * fSlow12);
		double 	fSlow94 = (fSlow93 + (fConst13 / fSlow10));
		double 	fSlow95 = (1.0 / fSlow91);
		double 	fSlow96 = (25.83773077393352 * fSlow16);
		double 	fSlow97 = ((fConst13 / fSlow14) + fSlow96);
		double 	fSlow98 = (fConst10 / fSlow10);
		double 	fSlow99 = (1 - double(fcheckbox7));
		double 	fSlow100 = (1.375 * (fSlow99 * ((fSlow8 * ((63 * fSlow8) + -70)) + 15)));
		double 	fSlow101 = ((fConst15 / fSlow14) + 1);
		double 	fSlow102 = (fConst14 / (fSlow14 * fSlow101));
		double 	fSlow103 = (1.0 / ((fConst15 / fSlow10) + 1));
		double 	fSlow104 = (1.0 / (((14.2724805132801 * fSlow12) + (fConst16 / fSlow10)) + 1));
		double 	fSlow105 = (((fConst16 / fSlow14) + (14.2724805132801 * fSlow16)) + 1);
		double 	fSlow106 = (1.0 / (((fConst17 / fSlow10) + (18.1563153134523 * fSlow12)) + 1));
		double 	fSlow107 = (((fConst17 / fSlow14) + (18.1563153134523 * fSlow16)) + 1);
		double 	fSlow108 = ((fSlow20 * fSlow107) / fSlow14);
		double 	fSlow109 = (72.6252612538092 * fSlow12);
		double 	fSlow110 = (fSlow109 + (fConst18 / fSlow10));
		double 	fSlow111 = (1.0 / fSlow107);
		double 	fSlow112 = (72.6252612538092 * fSlow16);
		double 	fSlow113 = (fSlow112 + (fConst18 / fSlow14));
		double 	fSlow114 = (57.0899220531204 * fSlow12);
		double 	fSlow115 = (fSlow114 + (fConst19 / fSlow10));
		double 	fSlow116 = (1.0 / fSlow105);
		double 	fSlow117 = (57.0899220531204 * fSlow16);
		double 	fSlow118 = ((fConst19 / fSlow14) + fSlow117);
		double 	fSlow119 = (fConst14 / fSlow10);
		double 	fSlow120 = (fSlow45 + 1);
		double 	fSlow121 = (fConst9 / (fSlow44 * fSlow120));
		double 	fSlow122 = ((fSlow49 * fSlow120) / fSlow44);
		double 	fSlow123 = (3.5 * (fSlow85 * ((5 * fSlow42) + -3)));
		double 	fSlow124 = ((fConst11 / fSlow44) + 1);
		double 	fSlow125 = (fConst10 / (fSlow44 * fSlow124));
		double 	fSlow126 = (((fConst12 / fSlow44) + (6.45943269348338 * fSlow46)) + 1);
		double 	fSlow127 = ((fSlow49 * fSlow126) / fSlow44);
		double 	fSlow128 = (1.0 / fSlow126);
		double 	fSlow129 = (25.83773077393352 * fSlow46);
		double 	fSlow130 = ((fConst13 / fSlow44) + fSlow129);
		double 	fSlow131 = (1.375 * (fSlow99 * ((fSlow42 * ((63 * fSlow42) + -70)) + 15)));
		double 	fSlow132 = ((fConst15 / fSlow44) + 1);
		double 	fSlow133 = (fConst14 / (fSlow44 * fSlow132));
		double 	fSlow134 = (((fConst16 / fSlow44) + (14.2724805132801 * fSlow46)) + 1);
		double 	fSlow135 = (((fConst17 / fSlow44) + (18.1563153134523 * fSlow46)) + 1);
		double 	fSlow136 = ((fSlow49 * fSlow135) / fSlow44);
		double 	fSlow137 = (1.0 / fSlow135);
		double 	fSlow138 = (72.6252612538092 * fSlow46);
		double 	fSlow139 = ((fConst18 / fSlow44) + fSlow138);
		double 	fSlow140 = (1.0 / fSlow134);
		double 	fSlow141 = (57.0899220531204 * fSlow46);
		double 	fSlow142 = ((fConst19 / fSlow44) + fSlow141);
		double 	fSlow143 = (3.5 * fSlow85);
		double 	fSlow144 = faustpower<3>(fSlow5);
		double 	fSlow145 = ((5 * fSlow144) - (3 * fSlow5));
		double 	fSlow146 = faustpower<3>(fSlow39);
		double 	fSlow147 = ((5 * fSlow146) - (3 * fSlow39));
		double 	fSlow148 = (1.375 * fSlow99);
		double 	fSlow149 = faustpower<2>(fSlow5);
		double 	fSlow150 = ((15 * fSlow5) + (fSlow144 * ((63 * fSlow149) + -70)));
		double 	fSlow151 = faustpower<2>(fSlow39);
		double 	fSlow152 = ((15 * fSlow39) + (fSlow146 * ((63 * fSlow151) + -70)));
		double 	fSlow153 = ((3 * fSlow149) + -1);
		double 	fSlow154 = ((3 * fSlow151) + -1);
		double 	fSlow155 = ((fSlow149 * ((35 * fSlow149) + -30)) + 3);
		double 	fSlow156 = ((fSlow151 * ((35 * fSlow151) + -30)) + 3);
		double 	fSlow157 = (fSlow37 * cos((fSlow38 + -1.5707963267948966)));
		double 	fSlow158 = faustpower<2>(fSlow157);
		double 	fSlow159 = ((fSlow158 * ((35 * fSlow158) + -30)) + 3);
		double 	fSlow160 = (fSlow3 * cos((fSlow4 + -1.5707963267948966)));
		double 	fSlow161 = faustpower<2>(fSlow160);
		double 	fSlow162 = ((fSlow161 * ((35 * fSlow161) + -30)) + 3);
		double 	fSlow163 = faustpower<3>(fSlow160);
		double 	fSlow164 = ((15 * fSlow160) + (fSlow163 * ((63 * fSlow161) + -70)));
		double 	fSlow165 = faustpower<3>(fSlow157);
		double 	fSlow166 = ((15 * fSlow157) + (fSlow165 * ((63 * fSlow158) + -70)));
		double 	fSlow167 = ((5 * fSlow165) - (3 * fSlow157));
		double 	fSlow168 = ((5 * fSlow163) - (3 * fSlow160));
		double 	fSlow169 = ((3 * fSlow158) + -1);
		double 	fSlow170 = ((3 * fSlow161) + -1);
		double 	fSlow171 = (fSlow3 * cos((fSlow4 + -3.141592653589793)));
		double 	fSlow172 = faustpower<2>(fSlow171);
		double 	fSlow173 = ((3 * fSlow172) + -1);
		double 	fSlow174 = (fSlow37 * cos((fSlow38 + -3.141592653589793)));
		double 	fSlow175 = faustpower<2>(fSlow174);
		double 	fSlow176 = ((3 * fSlow175) + -1);
		double 	fSlow177 = faustpower<3>(fSlow174);
		double 	fSlow178 = ((5 * fSlow177) - (3 * fSlow174));
		double 	fSlow179 = faustpower<3>(fSlow171);
		double 	fSlow180 = ((5 * fSlow179) - (3 * fSlow171));
		double 	fSlow181 = ((15 * fSlow171) + (fSlow179 * ((63 * fSlow172) + -70)));
		double 	fSlow182 = ((15 * fSlow174) + (fSlow177 * ((63 * fSlow175) + -70)));
		double 	fSlow183 = ((fSlow175 * ((35 * fSlow175) + -30)) + 3);
		double 	fSlow184 = ((fSlow172 * ((35 * fSlow172) + -30)) + 3);
		double 	fSlow185 = (fSlow3 * cos((fSlow4 + -4.71238898038469)));
		double 	fSlow186 = faustpower<2>(fSlow185);
		double 	fSlow187 = ((3 * fSlow186) + -1);
		double 	fSlow188 = (fSlow37 * cos((fSlow38 + -4.71238898038469)));
		double 	fSlow189 = faustpower<2>(fSlow188);
		double 	fSlow190 = ((3 * fSlow189) + -1);
		double 	fSlow191 = faustpower<3>(fSlow188);
		double 	fSlow192 = ((5 * fSlow191) - (3 * fSlow188));
		double 	fSlow193 = faustpower<3>(fSlow185);
		double 	fSlow194 = ((5 * fSlow193) - (3 * fSlow185));
		double 	fSlow195 = ((15 * fSlow185) + (fSlow193 * ((63 * fSlow186) + -70)));
		double 	fSlow196 = ((15 * fSlow188) + (fSlow191 * ((63 * fSlow189) + -70)));
		double 	fSlow197 = ((fSlow189 * ((35 * fSlow189) + -30)) + 3);
		double 	fSlow198 = ((fSlow186 * ((35 * fSlow186) + -30)) + 3);
		double 	fSlow199 = (fSlow6 - fSlow2);
		double 	fSlow200 = faustpower<2>(fSlow199);
		double 	fSlow201 = ((fSlow200 * ((35 * fSlow200) + -30)) + 3);
		double 	fSlow202 = (fSlow40 - fSlow36);
		double 	fSlow203 = faustpower<2>(fSlow202);
		double 	fSlow204 = ((fSlow203 * ((35 * fSlow203) + -30)) + 3);
		double 	fSlow205 = (1.375 * (fSlow99 * ((fSlow203 * ((63 * fSlow203) + -70)) + 15)));
		double 	fSlow206 = (3.5 * (fSlow85 * ((5 * fSlow203) + -3)));
		double 	fSlow207 = (3.5 * (fSlow85 * ((5 * fSlow200) + -3)));
		double 	fSlow208 = (1.375 * (fSlow99 * ((fSlow200 * ((63 * fSlow200) + -70)) + 15)));
		double 	fSlow209 = ((3 * fSlow203) + -1);
		double 	fSlow210 = ((3 * fSlow200) + -1);
		double 	fSlow211 = (0.7071067811865476 * fSlow39);
		double 	fSlow212 = (0.7071067811865475 * fSlow36);
		double 	fSlow213 = (fSlow211 + fSlow212);
		double 	fSlow214 = faustpower<2>(fSlow213);
		double 	fSlow215 = (3.5 * (fSlow85 * ((5 * fSlow214) + -3)));
		double 	fSlow216 = (1.375 * (fSlow99 * ((fSlow214 * ((63 * fSlow214) + -70)) + 15)));
		double 	fSlow217 = (0.7071067811865476 * fSlow5);
		double 	fSlow218 = (0.7071067811865475 * fSlow2);
		double 	fSlow219 = (fSlow217 + fSlow218);
		double 	fSlow220 = faustpower<2>(fSlow219);
		double 	fSlow221 = (3.5 * (fSlow85 * ((5 * fSlow220) + -3)));
		double 	fSlow222 = (1.375 * (fSlow99 * ((fSlow220 * ((63 * fSlow220) + -70)) + 15)));
		double 	fSlow223 = ((3 * fSlow220) + -1);
		double 	fSlow224 = ((3 * fSlow214) + -1);
		double 	fSlow225 = ((fSlow214 * ((35 * fSlow214) + -30)) + 3);
		double 	fSlow226 = ((fSlow220 * ((35 * fSlow220) + -30)) + 3);
		double 	fSlow227 = (0.7071067811865476 * fSlow160);
		double 	fSlow228 = (fSlow218 + fSlow227);
		double 	fSlow229 = faustpower<2>(fSlow228);
		double 	fSlow230 = ((fSlow229 * ((35 * fSlow229) + -30)) + 3);
		double 	fSlow231 = (0.7071067811865476 * fSlow157);
		double 	fSlow232 = (fSlow212 + fSlow231);
		double 	fSlow233 = faustpower<2>(fSlow232);
		double 	fSlow234 = ((fSlow233 * ((35 * fSlow233) + -30)) + 3);
		double 	fSlow235 = ((3 * fSlow229) + -1);
		double 	fSlow236 = ((3 * fSlow233) + -1);
		double 	fSlow237 = (1.375 * (fSlow99 * ((fSlow233 * ((63 * fSlow233) + -70)) + 15)));
		double 	fSlow238 = (3.5 * (fSlow85 * ((5 * fSlow233) + -3)));
		double 	fSlow239 = (3.5 * (fSlow85 * ((5 * fSlow229) + -3)));
		double 	fSlow240 = (1.375 * (fSlow99 * ((fSlow229 * ((63 * fSlow229) + -70)) + 15)));
		double 	fSlow241 = (0.7071067811865476 * fSlow171);
		double 	fSlow242 = (fSlow218 + fSlow241);
		double 	fSlow243 = faustpower<2>(fSlow242);
		double 	fSlow244 = ((fSlow243 * ((35 * fSlow243) + -30)) + 3);
		double 	fSlow245 = (0.7071067811865476 * fSlow174);
		double 	fSlow246 = (fSlow212 + fSlow245);
		double 	fSlow247 = faustpower<2>(fSlow246);
		double 	fSlow248 = ((fSlow247 * ((35 * fSlow247) + -30)) + 3);
		double 	fSlow249 = ((3 * fSlow243) + -1);
		double 	fSlow250 = ((3 * fSlow247) + -1);
		double 	fSlow251 = (1.375 * (fSlow99 * ((fSlow247 * ((63 * fSlow247) + -70)) + 15)));
		double 	fSlow252 = (3.5 * (fSlow85 * ((5 * fSlow247) + -3)));
		double 	fSlow253 = (3.5 * (fSlow85 * ((5 * fSlow243) + -3)));
		double 	fSlow254 = (1.375 * (fSlow99 * ((fSlow243 * ((63 * fSlow243) + -70)) + 15)));
		double 	fSlow255 = (0.7071067811865476 * fSlow185);
		double 	fSlow256 = (fSlow218 + fSlow255);
		double 	fSlow257 = faustpower<2>(fSlow256);
		double 	fSlow258 = ((fSlow257 * ((35 * fSlow257) + -30)) + 3);
		double 	fSlow259 = (0.7071067811865476 * fSlow188);
		double 	fSlow260 = (fSlow212 + fSlow259);
		double 	fSlow261 = faustpower<2>(fSlow260);
		double 	fSlow262 = ((fSlow261 * ((35 * fSlow261) + -30)) + 3);
		double 	fSlow263 = ((3 * fSlow257) + -1);
		double 	fSlow264 = ((3 * fSlow261) + -1);
		double 	fSlow265 = (1.375 * (fSlow99 * ((fSlow261 * ((63 * fSlow261) + -70)) + 15)));
		double 	fSlow266 = (3.5 * (fSlow85 * ((5 * fSlow261) + -3)));
		double 	fSlow267 = (3.5 * (fSlow85 * ((5 * fSlow257) + -3)));
		double 	fSlow268 = (1.375 * (fSlow99 * ((fSlow257 * ((63 * fSlow257) + -70)) + 15)));
		double 	fSlow269 = (fSlow3 * cos((fSlow4 + -0.7853981633974483)));
		double 	fSlow270 = faustpower<2>(fSlow269);
		double 	fSlow271 = ((3 * fSlow270) + -1);
		double 	fSlow272 = (fSlow37 * cos((fSlow38 + -0.7853981633974483)));
		double 	fSlow273 = faustpower<2>(fSlow272);
		double 	fSlow274 = ((3 * fSlow273) + -1);
		double 	fSlow275 = faustpower<3>(fSlow272);
		double 	fSlow276 = ((5 * fSlow275) - (3 * fSlow272));
		double 	fSlow277 = faustpower<3>(fSlow269);
		double 	fSlow278 = ((5 * fSlow277) - (3 * fSlow269));
		double 	fSlow279 = ((15 * fSlow269) + (fSlow277 * ((63 * fSlow270) + -70)));
		double 	fSlow280 = ((15 * fSlow272) + (fSlow275 * ((63 * fSlow273) + -70)));
		double 	fSlow281 = ((fSlow273 * ((35 * fSlow273) + -30)) + 3);
		double 	fSlow282 = ((fSlow270 * ((35 * fSlow270) + -30)) + 3);
		double 	fSlow283 = (fSlow3 * cos((fSlow4 + -2.356194490192345)));
		double 	fSlow284 = faustpower<2>(fSlow283);
		double 	fSlow285 = ((3 * fSlow284) + -1);
		double 	fSlow286 = (fSlow37 * cos((fSlow38 + -2.356194490192345)));
		double 	fSlow287 = faustpower<2>(fSlow286);
		double 	fSlow288 = ((3 * fSlow287) + -1);
		double 	fSlow289 = faustpower<3>(fSlow286);
		double 	fSlow290 = ((5 * fSlow289) - (3 * fSlow286));
		double 	fSlow291 = faustpower<3>(fSlow283);
		double 	fSlow292 = ((5 * fSlow291) - (3 * fSlow283));
		double 	fSlow293 = ((15 * fSlow283) + (fSlow291 * ((63 * fSlow284) + -70)));
		double 	fSlow294 = ((15 * fSlow286) + (fSlow289 * ((63 * fSlow287) + -70)));
		double 	fSlow295 = ((fSlow287 * ((35 * fSlow287) + -30)) + 3);
		double 	fSlow296 = ((fSlow284 * ((35 * fSlow284) + -30)) + 3);
		double 	fSlow297 = (fSlow3 * cos((fSlow4 + -3.9269908169872414)));
		double 	fSlow298 = faustpower<2>(fSlow297);
		double 	fSlow299 = ((3 * fSlow298) + -1);
		double 	fSlow300 = (fSlow37 * cos((fSlow38 + -3.9269908169872414)));
		double 	fSlow301 = faustpower<2>(fSlow300);
		double 	fSlow302 = ((3 * fSlow301) + -1);
		double 	fSlow303 = faustpower<3>(fSlow300);
		double 	fSlow304 = ((5 * fSlow303) - (3 * fSlow300));
		double 	fSlow305 = faustpower<3>(fSlow297);
		double 	fSlow306 = ((5 * fSlow305) - (3 * fSlow297));
		double 	fSlow307 = ((15 * fSlow297) + (fSlow305 * ((63 * fSlow298) + -70)));
		double 	fSlow308 = ((15 * fSlow300) + (fSlow303 * ((63 * fSlow301) + -70)));
		double 	fSlow309 = ((fSlow301 * ((35 * fSlow301) + -30)) + 3);
		double 	fSlow310 = ((fSlow298 * ((35 * fSlow298) + -30)) + 3);
		double 	fSlow311 = (fSlow3 * cos((fSlow4 + -5.497787143782138)));
		double 	fSlow312 = faustpower<2>(fSlow311);
		double 	fSlow313 = ((3 * fSlow312) + -1);
		double 	fSlow314 = (fSlow37 * cos((fSlow38 + -5.497787143782138)));
		double 	fSlow315 = faustpower<2>(fSlow314);
		double 	fSlow316 = ((3 * fSlow315) + -1);
		double 	fSlow317 = faustpower<3>(fSlow314);
		double 	fSlow318 = ((5 * fSlow317) - (3 * fSlow314));
		double 	fSlow319 = faustpower<3>(fSlow311);
		double 	fSlow320 = ((5 * fSlow319) - (3 * fSlow311));
		double 	fSlow321 = ((15 * fSlow311) + (fSlow319 * ((63 * fSlow312) + -70)));
		double 	fSlow322 = ((15 * fSlow314) + (fSlow317 * ((63 * fSlow315) + -70)));
		double 	fSlow323 = ((fSlow315 * ((35 * fSlow315) + -30)) + 3);
		double 	fSlow324 = ((fSlow312 * ((35 * fSlow312) + -30)) + 3);
		double 	fSlow325 = (fSlow211 - fSlow212);
		double 	fSlow326 = faustpower<2>(fSlow325);
		double 	fSlow327 = (3.5 * (fSlow85 * ((5 * fSlow326) + -3)));
		double 	fSlow328 = (1.375 * (fSlow99 * ((fSlow326 * ((63 * fSlow326) + -70)) + 15)));
		double 	fSlow329 = (fSlow217 - fSlow218);
		double 	fSlow330 = faustpower<2>(fSlow329);
		double 	fSlow331 = (1.375 * (fSlow99 * ((fSlow330 * ((63 * fSlow330) + -70)) + 15)));
		double 	fSlow332 = (3.5 * (fSlow85 * ((5 * fSlow330) + -3)));
		double 	fSlow333 = ((3 * fSlow330) + -1);
		double 	fSlow334 = ((3 * fSlow326) + -1);
		double 	fSlow335 = ((fSlow326 * ((35 * fSlow326) + -30)) + 3);
		double 	fSlow336 = ((fSlow330 * ((35 * fSlow330) + -30)) + 3);
		double 	fSlow337 = (fSlow227 - fSlow218);
		double 	fSlow338 = faustpower<2>(fSlow337);
		double 	fSlow339 = ((fSlow338 * ((35 * fSlow338) + -30)) + 3);
		double 	fSlow340 = (fSlow231 - fSlow212);
		double 	fSlow341 = faustpower<2>(fSlow340);
		double 	fSlow342 = ((fSlow341 * ((35 * fSlow341) + -30)) + 3);
		double 	fSlow343 = (1.375 * (fSlow99 * ((fSlow341 * ((63 * fSlow341) + -70)) + 15)));
		double 	fSlow344 = (3.5 * (fSlow85 * ((5 * fSlow341) + -3)));
		double 	fSlow345 = (3.5 * (fSlow85 * ((5 * fSlow338) + -3)));
		double 	fSlow346 = (1.375 * (fSlow99 * ((fSlow338 * ((63 * fSlow338) + -70)) + 15)));
		double 	fSlow347 = ((3 * fSlow341) + -1);
		double 	fSlow348 = ((3 * fSlow338) + -1);
		double 	fSlow349 = (fSlow241 - fSlow218);
		double 	fSlow350 = faustpower<2>(fSlow349);
		double 	fSlow351 = ((fSlow350 * ((35 * fSlow350) + -30)) + 3);
		double 	fSlow352 = (fSlow245 - fSlow212);
		double 	fSlow353 = faustpower<2>(fSlow352);
		double 	fSlow354 = ((fSlow353 * ((35 * fSlow353) + -30)) + 3);
		double 	fSlow355 = (1.375 * (fSlow99 * ((fSlow353 * ((63 * fSlow353) + -70)) + 15)));
		double 	fSlow356 = (3.5 * (fSlow85 * ((5 * fSlow353) + -3)));
		double 	fSlow357 = (3.5 * (fSlow85 * ((5 * fSlow350) + -3)));
		double 	fSlow358 = (1.375 * (fSlow99 * ((fSlow350 * ((63 * fSlow350) + -70)) + 15)));
		double 	fSlow359 = ((3 * fSlow353) + -1);
		double 	fSlow360 = ((3 * fSlow350) + -1);
		double 	fSlow361 = (fSlow255 - fSlow218);
		double 	fSlow362 = faustpower<2>(fSlow361);
		double 	fSlow363 = ((fSlow362 * ((35 * fSlow362) + -30)) + 3);
		double 	fSlow364 = (fSlow259 - fSlow212);
		double 	fSlow365 = faustpower<2>(fSlow364);
		double 	fSlow366 = ((fSlow365 * ((35 * fSlow365) + -30)) + 3);
		double 	fSlow367 = (1.375 * (fSlow99 * ((fSlow365 * ((63 * fSlow365) + -70)) + 15)));
		double 	fSlow368 = (3.5 * (fSlow85 * ((5 * fSlow365) + -3)));
		double 	fSlow369 = (3.5 * (fSlow85 * ((5 * fSlow362) + -3)));
		double 	fSlow370 = (1.375 * (fSlow99 * ((fSlow362 * ((63 * fSlow362) + -70)) + 15)));
		double 	fSlow371 = ((3 * fSlow365) + -1);
		double 	fSlow372 = ((3 * fSlow362) + -1);
		double 	fSlow373 = (0.8164965809277259 * fSlow272);
		double 	fSlow374 = (0.5773502691896258 * fSlow36);
		double 	fSlow375 = (fSlow373 + fSlow374);
		double 	fSlow376 = faustpower<2>(fSlow375);
		double 	fSlow377 = (3.5 * (fSlow85 * ((5 * fSlow376) + -3)));
		double 	fSlow378 = (1.375 * (fSlow99 * ((fSlow376 * ((63 * fSlow376) + -70)) + 15)));
		double 	fSlow379 = (0.8164965809277259 * fSlow269);
		double 	fSlow380 = (0.5773502691896258 * fSlow2);
		double 	fSlow381 = (fSlow379 + fSlow380);
		double 	fSlow382 = faustpower<2>(fSlow381);
		double 	fSlow383 = (3.5 * (fSlow85 * ((5 * fSlow382) + -3)));
		double 	fSlow384 = (1.375 * (fSlow99 * ((fSlow382 * ((63 * fSlow382) + -70)) + 15)));
		double 	fSlow385 = ((3 * fSlow382) + -1);
		double 	fSlow386 = ((3 * fSlow376) + -1);
		double 	fSlow387 = ((fSlow376 * ((35 * fSlow376) + -30)) + 3);
		double 	fSlow388 = ((fSlow382 * ((35 * fSlow382) + -30)) + 3);
		double 	fSlow389 = (0.8164965809277259 * fSlow283);
		double 	fSlow390 = (fSlow380 + fSlow389);
		double 	fSlow391 = faustpower<2>(fSlow390);
		double 	fSlow392 = ((fSlow391 * ((35 * fSlow391) + -30)) + 3);
		double 	fSlow393 = (0.8164965809277259 * fSlow286);
		double 	fSlow394 = (fSlow374 + fSlow393);
		double 	fSlow395 = faustpower<2>(fSlow394);
		double 	fSlow396 = ((fSlow395 * ((35 * fSlow395) + -30)) + 3);
		double 	fSlow397 = ((3 * fSlow391) + -1);
		double 	fSlow398 = ((3 * fSlow395) + -1);
		double 	fSlow399 = (1.375 * (fSlow99 * ((fSlow395 * ((63 * fSlow395) + -70)) + 15)));
		double 	fSlow400 = (3.5 * (fSlow85 * ((5 * fSlow395) + -3)));
		double 	fSlow401 = (3.5 * (fSlow85 * ((5 * fSlow391) + -3)));
		double 	fSlow402 = (1.375 * (fSlow99 * ((fSlow391 * ((63 * fSlow391) + -70)) + 15)));
		double 	fSlow403 = (0.8164965809277259 * fSlow297);
		double 	fSlow404 = (fSlow380 + fSlow403);
		double 	fSlow405 = faustpower<2>(fSlow404);
		double 	fSlow406 = ((fSlow405 * ((35 * fSlow405) + -30)) + 3);
		double 	fSlow407 = (0.8164965809277259 * fSlow300);
		double 	fSlow408 = (fSlow374 + fSlow407);
		double 	fSlow409 = faustpower<2>(fSlow408);
		double 	fSlow410 = ((fSlow409 * ((35 * fSlow409) + -30)) + 3);
		double 	fSlow411 = ((3 * fSlow405) + -1);
		double 	fSlow412 = ((3 * fSlow409) + -1);
		double 	fSlow413 = (1.375 * (fSlow99 * ((fSlow409 * ((63 * fSlow409) + -70)) + 15)));
		double 	fSlow414 = (3.5 * (fSlow85 * ((5 * fSlow409) + -3)));
		double 	fSlow415 = (3.5 * (fSlow85 * ((5 * fSlow405) + -3)));
		double 	fSlow416 = (1.375 * (fSlow99 * ((fSlow405 * ((63 * fSlow405) + -70)) + 15)));
		double 	fSlow417 = (0.8164965809277259 * fSlow314);
		double 	fSlow418 = (fSlow417 + fSlow374);
		double 	fSlow419 = faustpower<2>(fSlow418);
		double 	fSlow420 = ((fSlow419 * ((35 * fSlow419) + -30)) + 3);
		double 	fSlow421 = (0.8164965809277259 * fSlow311);
		double 	fSlow422 = (fSlow380 + fSlow421);
		double 	fSlow423 = faustpower<2>(fSlow422);
		double 	fSlow424 = ((((35 * fSlow423) + -30) * fSlow423) + 3);
		double 	fSlow425 = (3.5 * (fSlow85 * ((5 * fSlow419) + -3)));
		double 	fSlow426 = (1.375 * (fSlow99 * ((fSlow419 * ((63 * fSlow419) + -70)) + 15)));
		double 	fSlow427 = (3.5 * (fSlow85 * ((5 * fSlow423) + -3)));
		double 	fSlow428 = (1.375 * (fSlow99 * ((fSlow423 * ((63 * fSlow423) + -70)) + 15)));
		double 	fSlow429 = ((3 * fSlow419) + -1);
		double 	fSlow430 = ((3 * fSlow423) + -1);
		double 	fSlow431 = (fSlow373 - fSlow374);
		double 	fSlow432 = faustpower<2>(fSlow431);
		double 	fSlow433 = ((3 * fSlow432) + -1);
		double 	fSlow434 = (fSlow379 - fSlow380);
		double 	fSlow435 = faustpower<2>(fSlow434);
		double 	fSlow436 = ((3 * fSlow435) + -1);
		double 	fSlow437 = ((fSlow435 * ((35 * fSlow435) + -30)) + 3);
		double 	fSlow438 = ((fSlow432 * ((35 * fSlow432) + -30)) + 3);
		double 	fSlow439 = (3.5 * (fSlow85 * ((5 * fSlow435) + -3)));
		double 	fSlow440 = (1.375 * (fSlow99 * ((fSlow435 * ((63 * fSlow435) + -70)) + 15)));
		double 	fSlow441 = (3.5 * (fSlow85 * ((5 * fSlow432) + -3)));
		double 	fSlow442 = (1.375 * (fSlow99 * ((fSlow432 * ((63 * fSlow432) + -70)) + 15)));
		double 	fSlow443 = (fSlow389 - fSlow380);
		double 	fSlow444 = faustpower<2>(fSlow443);
		double 	fSlow445 = ((3 * fSlow444) + -1);
		double 	fSlow446 = (fSlow393 - fSlow374);
		double 	fSlow447 = faustpower<2>(fSlow446);
		double 	fSlow448 = ((3 * fSlow447) + -1);
		double 	fSlow449 = ((fSlow444 * ((35 * fSlow444) + -30)) + 3);
		double 	fSlow450 = ((fSlow447 * ((35 * fSlow447) + -30)) + 3);
		double 	fSlow451 = (3.5 * (fSlow85 * ((5 * fSlow444) + -3)));
		double 	fSlow452 = (1.375 * (fSlow99 * ((fSlow444 * ((63 * fSlow444) + -70)) + 15)));
		double 	fSlow453 = (3.5 * (fSlow85 * ((5 * fSlow447) + -3)));
		double 	fSlow454 = (1.375 * (fSlow99 * ((fSlow447 * ((63 * fSlow447) + -70)) + 15)));
		double 	fSlow455 = (fSlow403 - fSlow380);
		double 	fSlow456 = faustpower<2>(fSlow455);
		double 	fSlow457 = ((3 * fSlow456) + -1);
		double 	fSlow458 = (fSlow407 - fSlow374);
		double 	fSlow459 = faustpower<2>(fSlow458);
		double 	fSlow460 = ((3 * fSlow459) + -1);
		double 	fSlow461 = ((fSlow456 * ((35 * fSlow456) + -30)) + 3);
		double 	fSlow462 = ((fSlow459 * ((35 * fSlow459) + -30)) + 3);
		double 	fSlow463 = (1.375 * (fSlow99 * ((fSlow456 * ((63 * fSlow456) + -70)) + 15)));
		double 	fSlow464 = (3.5 * (fSlow85 * ((5 * fSlow456) + -3)));
		double 	fSlow465 = (3.5 * (fSlow85 * ((5 * fSlow459) + -3)));
		double 	fSlow466 = (1.375 * (fSlow99 * ((fSlow459 * ((63 * fSlow459) + -70)) + 15)));
		double 	fSlow467 = (fSlow421 - fSlow380);
		double 	fSlow468 = faustpower<2>(fSlow467);
		double 	fSlow469 = ((3 * fSlow468) + -1);
		double 	fSlow470 = (fSlow417 - fSlow374);
		double 	fSlow471 = faustpower<2>(fSlow470);
		double 	fSlow472 = ((3 * fSlow471) + -1);
		double 	fSlow473 = ((fSlow468 * ((35 * fSlow468) + -30)) + 3);
		double 	fSlow474 = ((fSlow471 * ((35 * fSlow471) + -30)) + 3);
		double 	fSlow475 = (1.375 * (fSlow99 * ((fSlow468 * ((63 * fSlow468) + -70)) + 15)));
		double 	fSlow476 = (3.5 * (fSlow85 * ((5 * fSlow468) + -3)));
		double 	fSlow477 = (3.5 * (fSlow85 * ((5 * fSlow471) + -3)));
		double 	fSlow478 = (1.375 * (fSlow99 * ((fSlow471 * ((63 * fSlow471) + -70)) + 15)));
		double 	fSlow479 = (0.4264014327112203 * (fSlow3 * cos((fSlow4 + -0.7853981633974484))));
		double 	fSlow480 = (0.9045340337332911 * fSlow2);
		double 	fSlow481 = (fSlow479 + fSlow480);
		double 	fSlow482 = faustpower<2>(fSlow481);
		double 	fSlow483 = ((fSlow482 * ((35 * fSlow482) + -30)) + 3);
		double 	fSlow484 = (0.4264014327112203 * (fSlow37 * cos((fSlow38 + -0.7853981633974484))));
		double 	fSlow485 = (0.9045340337332911 * fSlow36);
		double 	fSlow486 = (fSlow484 + fSlow485);
		double 	fSlow487 = faustpower<2>(fSlow486);
		double 	fSlow488 = ((fSlow487 * ((35 * fSlow487) + -30)) + 3);
		double 	fSlow489 = (3.5 * (fSlow85 * ((5 * fSlow482) + -3)));
		double 	fSlow490 = (1.375 * (fSlow99 * ((fSlow482 * ((63 * fSlow482) + -70)) + 15)));
		double 	fSlow491 = (3.5 * (fSlow85 * ((5 * fSlow487) + -3)));
		double 	fSlow492 = (1.375 * (fSlow99 * ((fSlow487 * ((63 * fSlow487) + -70)) + 15)));
		double 	fSlow493 = ((3 * fSlow482) + -1);
		double 	fSlow494 = ((3 * fSlow487) + -1);
		double 	fSlow495 = (0.4264014327112203 * fSlow283);
		double 	fSlow496 = (fSlow480 + fSlow495);
		double 	fSlow497 = faustpower<2>(fSlow496);
		double 	fSlow498 = (3.5 * (fSlow85 * ((5 * fSlow497) + -3)));
		double 	fSlow499 = (1.375 * (fSlow99 * ((fSlow497 * ((63 * fSlow497) + -70)) + 15)));
		double 	fSlow500 = (0.4264014327112203 * fSlow286);
		double 	fSlow501 = (fSlow485 + fSlow500);
		double 	fSlow502 = faustpower<2>(fSlow501);
		double 	fSlow503 = (1.375 * (fSlow99 * ((fSlow502 * ((63 * fSlow502) + -70)) + 15)));
		double 	fSlow504 = (3.5 * (fSlow85 * ((5 * fSlow502) + -3)));
		double 	fSlow505 = ((fSlow497 * ((35 * fSlow497) + -30)) + 3);
		double 	fSlow506 = ((((35 * fSlow502) + -30) * fSlow502) + 3);
		double 	fSlow507 = ((3 * fSlow497) + -1);
		double 	fSlow508 = ((3 * fSlow502) + -1);
		double 	fSlow509 = (0.4264014327112203 * fSlow300);
		double 	fSlow510 = (fSlow485 + fSlow509);
		double 	fSlow511 = faustpower<2>(fSlow510);
		double 	fSlow512 = ((3 * fSlow511) + -1);
		double 	fSlow513 = (0.4264014327112203 * fSlow297);
		double 	fSlow514 = (fSlow480 + fSlow513);
		double 	fSlow515 = faustpower<2>(fSlow514);
		double 	fSlow516 = ((3 * fSlow515) + -1);
		double 	fSlow517 = (3.5 * (fSlow85 * ((5 * fSlow515) + -3)));
		double 	fSlow518 = (1.375 * (fSlow99 * ((((63 * fSlow515) + -70) * fSlow515) + 15)));
		double 	fSlow519 = (3.5 * (fSlow85 * ((5 * fSlow511) + -3)));
		double 	fSlow520 = (1.375 * (fSlow99 * ((((63 * fSlow511) + -70) * fSlow511) + 15)));
		double 	fSlow521 = ((fSlow511 * ((35 * fSlow511) + -30)) + 3);
		double 	fSlow522 = ((fSlow515 * ((35 * fSlow515) + -30)) + 3);
		double 	fSlow523 = (0.4264014327112203 * fSlow311);
		double 	fSlow524 = (fSlow480 + fSlow523);
		double 	fSlow525 = faustpower<2>(fSlow524);
		double 	fSlow526 = ((3 * fSlow525) + -1);
		double 	fSlow527 = (0.4264014327112203 * fSlow314);
		double 	fSlow528 = (fSlow485 + fSlow527);
		double 	fSlow529 = faustpower<2>(fSlow528);
		double 	fSlow530 = ((3 * fSlow529) + -1);
		double 	fSlow531 = ((fSlow525 * ((35 * fSlow525) + -30)) + 3);
		double 	fSlow532 = ((fSlow529 * ((35 * fSlow529) + -30)) + 3);
		double 	fSlow533 = (3.5 * (fSlow85 * ((5 * fSlow525) + -3)));
		double 	fSlow534 = (1.375 * (fSlow99 * ((((63 * fSlow525) + -70) * fSlow525) + 15)));
		double 	fSlow535 = (3.5 * (fSlow85 * ((5 * fSlow529) + -3)));
		double 	fSlow536 = (1.375 * (fSlow99 * ((((63 * fSlow529) + -70) * fSlow529) + 15)));
		double 	fSlow537 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -0.32175055439664263))));
		double 	fSlow538 = (0.3015113445777635 * fSlow2);
		double 	fSlow539 = (fSlow537 + fSlow538);
		double 	fSlow540 = faustpower<2>(fSlow539);
		double 	fSlow541 = ((fSlow540 * ((35 * fSlow540) + -30)) + 3);
		double 	fSlow542 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -0.32175055439664263))));
		double 	fSlow543 = (0.3015113445777635 * fSlow36);
		double 	fSlow544 = (fSlow542 + fSlow543);
		double 	fSlow545 = faustpower<2>(fSlow544);
		double 	fSlow546 = ((fSlow545 * ((35 * fSlow545) + -30)) + 3);
		double 	fSlow547 = (3.5 * (fSlow85 * ((5 * fSlow540) + -3)));
		double 	fSlow548 = (1.375 * (fSlow99 * ((((63 * fSlow540) + -70) * fSlow540) + 15)));
		double 	fSlow549 = (3.5 * (fSlow85 * ((5 * fSlow545) + -3)));
		double 	fSlow550 = (1.375 * (fSlow99 * ((((63 * fSlow545) + -70) * fSlow545) + 15)));
		double 	fSlow551 = ((3 * fSlow540) + -1);
		double 	fSlow552 = ((3 * fSlow545) + -1);
		double 	fSlow553 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -1.2490457723982544))));
		double 	fSlow554 = (fSlow538 + fSlow553);
		double 	fSlow555 = faustpower<2>(fSlow554);
		double 	fSlow556 = ((3 * fSlow555) + -1);
		double 	fSlow557 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -1.2490457723982544))));
		double 	fSlow558 = (fSlow543 + fSlow557);
		double 	fSlow559 = faustpower<2>(fSlow558);
		double 	fSlow560 = ((3 * fSlow559) + -1);
		double 	fSlow561 = ((fSlow555 * ((35 * fSlow555) + -30)) + 3);
		double 	fSlow562 = ((fSlow559 * ((35 * fSlow559) + -30)) + 3);
		double 	fSlow563 = (3.5 * (fSlow85 * ((5 * fSlow555) + -3)));
		double 	fSlow564 = (1.375 * (fSlow99 * ((((63 * fSlow555) + -70) * fSlow555) + 15)));
		double 	fSlow565 = (3.5 * (fSlow85 * ((5 * fSlow559) + -3)));
		double 	fSlow566 = (1.375 * (fSlow99 * ((((63 * fSlow559) + -70) * fSlow559) + 15)));
		double 	fSlow567 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -1.8925468811915387))));
		double 	fSlow568 = (fSlow538 + fSlow567);
		double 	fSlow569 = faustpower<2>(fSlow568);
		double 	fSlow570 = ((3 * fSlow569) + -1);
		double 	fSlow571 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -1.8925468811915387))));
		double 	fSlow572 = (fSlow543 + fSlow571);
		double 	fSlow573 = faustpower<2>(fSlow572);
		double 	fSlow574 = ((3 * fSlow573) + -1);
		double 	fSlow575 = ((fSlow569 * ((35 * fSlow569) + -30)) + 3);
		double 	fSlow576 = ((fSlow573 * ((35 * fSlow573) + -30)) + 3);
		double 	fSlow577 = (3.5 * (fSlow85 * ((5 * fSlow569) + -3)));
		double 	fSlow578 = (1.375 * (fSlow99 * ((((63 * fSlow569) + -70) * fSlow569) + 15)));
		double 	fSlow579 = (3.5 * (fSlow85 * ((5 * fSlow573) + -3)));
		double 	fSlow580 = (1.375 * (fSlow99 * ((((63 * fSlow573) + -70) * fSlow573) + 15)));
		double 	fSlow581 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -2.8198420991931505))));
		double 	fSlow582 = (fSlow538 + fSlow581);
		double 	fSlow583 = faustpower<2>(fSlow582);
		double 	fSlow584 = ((3 * fSlow583) + -1);
		double 	fSlow585 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -2.8198420991931505))));
		double 	fSlow586 = (fSlow543 + fSlow585);
		double 	fSlow587 = faustpower<2>(fSlow586);
		double 	fSlow588 = ((3 * fSlow587) + -1);
		double 	fSlow589 = ((fSlow583 * ((35 * fSlow583) + -30)) + 3);
		double 	fSlow590 = ((fSlow587 * ((35 * fSlow587) + -30)) + 3);
		double 	fSlow591 = (3.5 * (fSlow85 * ((5 * fSlow583) + -3)));
		double 	fSlow592 = (1.375 * (fSlow99 * ((((63 * fSlow583) + -70) * fSlow583) + 15)));
		double 	fSlow593 = (3.5 * (fSlow85 * ((5 * fSlow587) + -3)));
		double 	fSlow594 = (1.375 * (fSlow99 * ((((63 * fSlow587) + -70) * fSlow587) + 15)));
		double 	fSlow595 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -3.4633432079864357))));
		double 	fSlow596 = (fSlow538 + fSlow595);
		double 	fSlow597 = faustpower<2>(fSlow596);
		double 	fSlow598 = ((3 * fSlow597) + -1);
		double 	fSlow599 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -3.4633432079864357))));
		double 	fSlow600 = (fSlow543 + fSlow599);
		double 	fSlow601 = faustpower<2>(fSlow600);
		double 	fSlow602 = ((3 * fSlow601) + -1);
		double 	fSlow603 = ((fSlow597 * ((35 * fSlow597) + -30)) + 3);
		double 	fSlow604 = ((fSlow601 * ((35 * fSlow601) + -30)) + 3);
		double 	fSlow605 = (3.5 * (fSlow85 * ((5 * fSlow597) + -3)));
		double 	fSlow606 = (1.375 * (fSlow99 * ((((63 * fSlow597) + -70) * fSlow597) + 15)));
		double 	fSlow607 = (3.5 * (fSlow85 * ((5 * fSlow601) + -3)));
		double 	fSlow608 = (1.375 * (fSlow99 * ((((63 * fSlow601) + -70) * fSlow601) + 15)));
		double 	fSlow609 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -4.3906384259880475))));
		double 	fSlow610 = (fSlow538 + fSlow609);
		double 	fSlow611 = faustpower<2>(fSlow610);
		double 	fSlow612 = ((3 * fSlow611) + -1);
		double 	fSlow613 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -4.3906384259880475))));
		double 	fSlow614 = (fSlow543 + fSlow613);
		double 	fSlow615 = faustpower<2>(fSlow614);
		double 	fSlow616 = ((3 * fSlow615) + -1);
		double 	fSlow617 = ((fSlow611 * ((35 * fSlow611) + -30)) + 3);
		double 	fSlow618 = ((fSlow615 * ((35 * fSlow615) + -30)) + 3);
		double 	fSlow619 = (3.5 * (fSlow85 * ((5 * fSlow611) + -3)));
		double 	fSlow620 = (1.375 * (fSlow99 * ((((63 * fSlow611) + -70) * fSlow611) + 15)));
		double 	fSlow621 = (3.5 * (fSlow85 * ((5 * fSlow615) + -3)));
		double 	fSlow622 = (1.375 * (fSlow99 * ((((63 * fSlow615) + -70) * fSlow615) + 15)));
		double 	fSlow623 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -5.034139534781332))));
		double 	fSlow624 = (fSlow538 + fSlow623);
		double 	fSlow625 = faustpower<2>(fSlow624);
		double 	fSlow626 = ((3 * fSlow625) + -1);
		double 	fSlow627 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -5.034139534781332))));
		double 	fSlow628 = (fSlow543 + fSlow627);
		double 	fSlow629 = faustpower<2>(fSlow628);
		double 	fSlow630 = ((3 * fSlow629) + -1);
		double 	fSlow631 = ((fSlow625 * ((35 * fSlow625) + -30)) + 3);
		double 	fSlow632 = ((fSlow629 * ((35 * fSlow629) + -30)) + 3);
		double 	fSlow633 = (3.5 * (fSlow85 * ((5 * fSlow625) + -3)));
		double 	fSlow634 = (1.375 * (fSlow99 * ((((63 * fSlow625) + -70) * fSlow625) + 15)));
		double 	fSlow635 = (3.5 * (fSlow85 * ((5 * fSlow629) + -3)));
		double 	fSlow636 = (1.375 * (fSlow99 * ((((63 * fSlow629) + -70) * fSlow629) + 15)));
		double 	fSlow637 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -5.961434752782943))));
		double 	fSlow638 = (fSlow538 + fSlow637);
		double 	fSlow639 = faustpower<2>(fSlow638);
		double 	fSlow640 = ((3 * fSlow639) + -1);
		double 	fSlow641 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -5.961434752782943))));
		double 	fSlow642 = (fSlow543 + fSlow641);
		double 	fSlow643 = faustpower<2>(fSlow642);
		double 	fSlow644 = ((3 * fSlow643) + -1);
		double 	fSlow645 = ((fSlow639 * ((35 * fSlow639) + -30)) + 3);
		double 	fSlow646 = ((fSlow643 * ((35 * fSlow643) + -30)) + 3);
		double 	fSlow647 = (3.5 * (fSlow85 * ((5 * fSlow639) + -3)));
		double 	fSlow648 = (1.375 * (fSlow99 * ((((63 * fSlow639) + -70) * fSlow639) + 15)));
		double 	fSlow649 = (3.5 * (fSlow85 * ((5 * fSlow643) + -3)));
		double 	fSlow650 = (1.375 * (fSlow99 * ((((63 * fSlow643) + -70) * fSlow643) + 15)));
		double 	fSlow651 = (fSlow537 - fSlow538);
		double 	fSlow652 = faustpower<2>(fSlow651);
		double 	fSlow653 = ((3 * fSlow652) + -1);
		double 	fSlow654 = (fSlow542 - fSlow543);
		double 	fSlow655 = faustpower<2>(fSlow654);
		double 	fSlow656 = ((3 * fSlow655) + -1);
		double 	fSlow657 = ((fSlow652 * ((35 * fSlow652) + -30)) + 3);
		double 	fSlow658 = ((fSlow655 * ((35 * fSlow655) + -30)) + 3);
		double 	fSlow659 = (1.375 * (fSlow99 * ((((63 * fSlow652) + -70) * fSlow652) + 15)));
		double 	fSlow660 = (3.5 * (fSlow85 * ((5 * fSlow652) + -3)));
		double 	fSlow661 = (3.5 * (fSlow85 * ((5 * fSlow655) + -3)));
		double 	fSlow662 = (1.375 * (fSlow99 * ((((63 * fSlow655) + -70) * fSlow655) + 15)));
		double 	fSlow663 = (fSlow553 - fSlow538);
		double 	fSlow664 = faustpower<2>(fSlow663);
		double 	fSlow665 = ((3 * fSlow664) + -1);
		double 	fSlow666 = (fSlow557 - fSlow543);
		double 	fSlow667 = faustpower<2>(fSlow666);
		double 	fSlow668 = ((3 * fSlow667) + -1);
		double 	fSlow669 = ((fSlow664 * ((35 * fSlow664) + -30)) + 3);
		double 	fSlow670 = ((fSlow667 * ((35 * fSlow667) + -30)) + 3);
		double 	fSlow671 = (1.375 * (fSlow99 * ((((63 * fSlow664) + -70) * fSlow664) + 15)));
		double 	fSlow672 = (3.5 * (fSlow85 * ((5 * fSlow664) + -3)));
		double 	fSlow673 = (3.5 * (fSlow85 * ((5 * fSlow667) + -3)));
		double 	fSlow674 = (1.375 * (fSlow99 * ((((63 * fSlow667) + -70) * fSlow667) + 15)));
		double 	fSlow675 = (fSlow567 - fSlow538);
		double 	fSlow676 = faustpower<2>(fSlow675);
		double 	fSlow677 = ((3 * fSlow676) + -1);
		double 	fSlow678 = (fSlow571 - fSlow543);
		double 	fSlow679 = faustpower<2>(fSlow678);
		double 	fSlow680 = ((3 * fSlow679) + -1);
		double 	fSlow681 = ((fSlow676 * ((35 * fSlow676) + -30)) + 3);
		double 	fSlow682 = ((fSlow679 * ((35 * fSlow679) + -30)) + 3);
		double 	fSlow683 = (1.375 * (fSlow99 * ((((63 * fSlow676) + -70) * fSlow676) + 15)));
		double 	fSlow684 = (3.5 * (fSlow85 * ((5 * fSlow676) + -3)));
		double 	fSlow685 = (3.5 * (fSlow85 * ((5 * fSlow679) + -3)));
		double 	fSlow686 = (1.375 * (fSlow99 * ((((63 * fSlow679) + -70) * fSlow679) + 15)));
		double 	fSlow687 = (fSlow581 - fSlow538);
		double 	fSlow688 = faustpower<2>(fSlow687);
		double 	fSlow689 = ((3 * fSlow688) + -1);
		double 	fSlow690 = (fSlow585 - fSlow543);
		double 	fSlow691 = faustpower<2>(fSlow690);
		double 	fSlow692 = ((3 * fSlow691) + -1);
		double 	fSlow693 = ((fSlow688 * ((35 * fSlow688) + -30)) + 3);
		double 	fSlow694 = ((fSlow691 * ((35 * fSlow691) + -30)) + 3);
		double 	fSlow695 = (1.375 * (fSlow99 * ((((63 * fSlow688) + -70) * fSlow688) + 15)));
		double 	fSlow696 = (3.5 * (fSlow85 * ((5 * fSlow688) + -3)));
		double 	fSlow697 = (3.5 * (fSlow85 * ((5 * fSlow691) + -3)));
		double 	fSlow698 = (1.375 * (fSlow99 * ((((63 * fSlow691) + -70) * fSlow691) + 15)));
		double 	fSlow699 = (fSlow595 - fSlow538);
		double 	fSlow700 = faustpower<2>(fSlow699);
		double 	fSlow701 = ((3 * fSlow700) + -1);
		double 	fSlow702 = (fSlow599 - fSlow543);
		double 	fSlow703 = faustpower<2>(fSlow702);
		double 	fSlow704 = ((3 * fSlow703) + -1);
		double 	fSlow705 = ((fSlow700 * ((35 * fSlow700) + -30)) + 3);
		double 	fSlow706 = ((fSlow703 * ((35 * fSlow703) + -30)) + 3);
		double 	fSlow707 = (1.375 * (fSlow99 * ((((63 * fSlow700) + -70) * fSlow700) + 15)));
		double 	fSlow708 = (3.5 * (fSlow85 * ((5 * fSlow700) + -3)));
		double 	fSlow709 = (3.5 * (fSlow85 * ((5 * fSlow703) + -3)));
		double 	fSlow710 = (1.375 * (fSlow99 * ((((63 * fSlow703) + -70) * fSlow703) + 15)));
		double 	fSlow711 = (fSlow613 - fSlow543);
		double 	fSlow712 = faustpower<2>(fSlow711);
		double 	fSlow713 = (3.5 * (fSlow85 * ((5 * fSlow712) + -3)));
		double 	fSlow714 = (1.375 * (fSlow99 * ((fSlow712 * ((63 * fSlow712) + -70)) + 15)));
		double 	fSlow715 = (fSlow609 - fSlow538);
		double 	fSlow716 = faustpower<2>(fSlow715);
		double 	fSlow717 = (3.5 * (fSlow85 * ((5 * fSlow716) + -3)));
		double 	fSlow718 = (1.375 * (fSlow99 * ((fSlow716 * ((63 * fSlow716) + -70)) + 15)));
		double 	fSlow719 = ((3 * fSlow712) + -1);
		double 	fSlow720 = ((3 * fSlow716) + -1);
		double 	fSlow721 = ((fSlow716 * ((35 * fSlow716) + -30)) + 3);
		double 	fSlow722 = ((fSlow712 * ((35 * fSlow712) + -30)) + 3);
		double 	fSlow723 = (fSlow627 - fSlow543);
		double 	fSlow724 = faustpower<2>(fSlow723);
		double 	fSlow725 = (3.5 * (fSlow85 * ((5 * fSlow724) + -3)));
		double 	fSlow726 = (1.375 * (fSlow99 * ((fSlow724 * ((63 * fSlow724) + -70)) + 15)));
		double 	fSlow727 = (fSlow623 - fSlow538);
		double 	fSlow728 = faustpower<2>(fSlow727);
		double 	fSlow729 = (1.375 * (fSlow99 * ((fSlow728 * ((63 * fSlow728) + -70)) + 15)));
		double 	fSlow730 = (3.5 * (fSlow85 * ((5 * fSlow728) + -3)));
		double 	fSlow731 = ((3 * fSlow724) + -1);
		double 	fSlow732 = ((3 * fSlow728) + -1);
		double 	fSlow733 = ((fSlow724 * ((35 * fSlow724) + -30)) + 3);
		double 	fSlow734 = ((fSlow728 * ((35 * fSlow728) + -30)) + 3);
		double 	fSlow735 = (fSlow641 - fSlow543);
		double 	fSlow736 = faustpower<2>(fSlow735);
		double 	fSlow737 = ((3 * fSlow736) + -1);
		double 	fSlow738 = (fSlow637 - fSlow538);
		double 	fSlow739 = faustpower<2>(fSlow738);
		double 	fSlow740 = ((3 * fSlow739) + -1);
		double 	fSlow741 = (3.5 * (fSlow85 * ((5 * fSlow736) + -3)));
		double 	fSlow742 = (1.375 * (fSlow99 * ((fSlow736 * ((63 * fSlow736) + -70)) + 15)));
		double 	fSlow743 = (3.5 * (fSlow85 * ((5 * fSlow739) + -3)));
		double 	fSlow744 = (1.375 * (fSlow99 * ((fSlow739 * ((63 * fSlow739) + -70)) + 15)));
		double 	fSlow745 = ((fSlow739 * ((35 * fSlow739) + -30)) + 3);
		double 	fSlow746 = ((fSlow736 * ((35 * fSlow736) + -30)) + 3);
		double 	fSlow747 = (fSlow484 - fSlow485);
		double 	fSlow748 = faustpower<2>(fSlow747);
		double 	fSlow749 = ((3 * fSlow748) + -1);
		double 	fSlow750 = (fSlow479 - fSlow480);
		double 	fSlow751 = faustpower<2>(fSlow750);
		double 	fSlow752 = ((3 * fSlow751) + -1);
		double 	fSlow753 = (3.5 * (fSlow85 * ((5 * fSlow748) + -3)));
		double 	fSlow754 = (1.375 * (fSlow99 * ((fSlow748 * ((63 * fSlow748) + -70)) + 15)));
		double 	fSlow755 = (3.5 * (fSlow85 * ((5 * fSlow751) + -3)));
		double 	fSlow756 = (1.375 * (fSlow99 * ((fSlow751 * ((63 * fSlow751) + -70)) + 15)));
		double 	fSlow757 = ((fSlow748 * ((35 * fSlow748) + -30)) + 3);
		double 	fSlow758 = ((fSlow751 * ((35 * fSlow751) + -30)) + 3);
		double 	fSlow759 = (fSlow500 - fSlow485);
		double 	fSlow760 = faustpower<2>(fSlow759);
		double 	fSlow761 = (3.5 * (fSlow85 * ((5 * fSlow760) + -3)));
		double 	fSlow762 = (1.375 * (fSlow99 * ((fSlow760 * ((63 * fSlow760) + -70)) + 15)));
		double 	fSlow763 = (fSlow495 - fSlow480);
		double 	fSlow764 = faustpower<2>(fSlow763);
		double 	fSlow765 = (1.375 * (fSlow99 * ((fSlow764 * ((63 * fSlow764) + -70)) + 15)));
		double 	fSlow766 = (3.5 * (fSlow85 * ((5 * fSlow764) + -3)));
		double 	fSlow767 = ((3 * fSlow760) + -1);
		double 	fSlow768 = ((3 * fSlow764) + -1);
		double 	fSlow769 = ((fSlow760 * ((35 * fSlow760) + -30)) + 3);
		double 	fSlow770 = ((fSlow764 * ((35 * fSlow764) + -30)) + 3);
		double 	fSlow771 = (fSlow509 - fSlow485);
		double 	fSlow772 = faustpower<2>(fSlow771);
		double 	fSlow773 = ((3 * fSlow772) + -1);
		double 	fSlow774 = (fSlow513 - fSlow480);
		double 	fSlow775 = faustpower<2>(fSlow774);
		double 	fSlow776 = ((3 * fSlow775) + -1);
		double 	fSlow777 = (3.5 * (fSlow85 * ((5 * fSlow772) + -3)));
		double 	fSlow778 = (1.375 * (fSlow99 * ((fSlow772 * ((63 * fSlow772) + -70)) + 15)));
		double 	fSlow779 = (3.5 * (fSlow85 * ((5 * fSlow775) + -3)));
		double 	fSlow780 = (1.375 * (fSlow99 * ((fSlow775 * ((63 * fSlow775) + -70)) + 15)));
		double 	fSlow781 = ((fSlow775 * ((35 * fSlow775) + -30)) + 3);
		double 	fSlow782 = ((fSlow772 * ((35 * fSlow772) + -30)) + 3);
		double 	fSlow783 = (fSlow527 - fSlow485);
		double 	fSlow784 = faustpower<2>(fSlow783);
		double 	fSlow785 = ((3 * fSlow784) + -1);
		double 	fSlow786 = (fSlow523 - fSlow480);
		double 	fSlow787 = faustpower<2>(fSlow786);
		double 	fSlow788 = ((3 * fSlow787) + -1);
		double 	fSlow789 = (3.5 * (fSlow85 * ((5 * fSlow784) + -3)));
		double 	fSlow790 = (1.375 * (fSlow99 * ((fSlow784 * ((63 * fSlow784) + -70)) + 15)));
		double 	fSlow791 = (3.5 * (fSlow85 * ((5 * fSlow787) + -3)));
		double 	fSlow792 = (1.375 * (fSlow99 * ((fSlow787 * ((63 * fSlow787) + -70)) + 15)));
		double 	fSlow793 = ((fSlow784 * ((35 * fSlow784) + -30)) + 3);
		double 	fSlow794 = ((fSlow787 * ((35 * fSlow787) + -30)) + 3);
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
			fRec3[0] = (fSlow23 + (0.999 * fRec3[1]));
			double fTemp0 = ((double)input0[i] * fRec3[0]);
			fRec4[0] = (fRec2[1] + fRec4[1]);
			fRec5[0] = (fRec4[1] + fRec5[1]);
			fRec2[0] = (fSlow18 * ((fSlow22 * fTemp0) - ((fSlow25 * fRec4[0]) + (fSlow24 * fRec5[0]))));
			fRec6[0] = (fRec2[0] + fRec6[1]);
			double fTemp1 = (fRec6[0] - fRec2[0]);
			fVec0[0] = fTemp1;
			fRec7[0] = (fRec7[1] + fVec0[1]);
			fRec8[0] = (fRec1[1] + fRec8[1]);
			fRec9[0] = (fRec8[1] + fRec9[1]);
			fRec1[0] = (fSlow13 * ((fSlow17 * (fRec2[0] + (fSlow26 * ((fSlow28 * fVec0[0]) + (fSlow27 * fRec7[0]))))) - ((fSlow30 * fRec8[0]) + (fSlow29 * fRec9[0]))));
			fRec10[0] = (fRec1[0] + fRec10[1]);
			double fTemp2 = (fRec10[0] - fRec1[0]);
			fVec1[0] = fTemp2;
			fRec11[0] = (fRec11[1] + fVec1[1]);
			fRec13[0] = (fRec12[1] + fRec13[1]);
			fRec14[0] = (fRec13[1] + fRec14[1]);
			fRec16[0] = (fRec15[1] + fRec16[1]);
			fRec17[0] = (fRec16[1] + fRec17[1]);
			double fTemp3 = (fSlow34 * fTemp0);
			fRec15[0] = (0 - (fSlow18 * (((fSlow25 * fRec16[0]) + (fSlow24 * fRec17[0])) - fTemp3)));
			fRec12[0] = (0 - (fSlow13 * (((fSlow30 * fRec13[0]) + (fSlow29 * fRec14[0])) - fRec15[0])));
			double fTemp4 = ((fRec1[0] + (fSlow31 * ((fSlow33 * fVec1[0]) + (fSlow32 * fRec11[0])))) + fRec12[0]);
			fRec19[0] = (fRec18[1] + fRec19[1]);
			fRec20[0] = (fRec19[1] + fRec20[1]);
			fRec22[0] = (fRec21[1] + fRec22[1]);
			fRec23[0] = (fRec22[1] + fRec23[1]);
			fRec24[0] = (fSlow52 + (0.999 * fRec24[1]));
			double fTemp5 = ((double)input1[i] * fRec24[0]);
			fRec21[0] = (0 - (fSlow18 * (((fSlow25 * fRec22[0]) + (fSlow24 * fRec23[0])) - (fSlow51 * fTemp5))));
			fRec25[0] = (fRec21[0] + fRec25[1]);
			double fTemp6 = (fRec25[0] - fRec21[0]);
			fVec2[0] = fTemp6;
			fRec26[0] = (fRec26[1] + fVec2[1]);
			fRec18[0] = (0 - (fSlow13 * (((fSlow30 * fRec19[0]) + (fSlow29 * fRec20[0])) - (fSlow47 * (fRec21[0] + (fSlow53 * ((fSlow55 * fVec2[0]) + (fSlow54 * fRec26[0]))))))));
			fRec27[0] = (fRec18[0] + fRec27[1]);
			double fTemp7 = (fRec27[0] - fRec18[0]);
			fVec3[0] = fTemp7;
			fRec28[0] = (fRec28[1] + fVec3[1]);
			fRec30[0] = (fRec29[1] + fRec30[1]);
			fRec31[0] = (fRec30[1] + fRec31[1]);
			fRec33[0] = (fRec32[1] + fRec33[1]);
			fRec34[0] = (fRec33[1] + fRec34[1]);
			double fTemp8 = (fSlow59 * fTemp5);
			fRec32[0] = (0 - (fSlow18 * (((fSlow25 * fRec33[0]) + (fSlow24 * fRec34[0])) - fTemp8)));
			fRec29[0] = (0 - (fSlow13 * (((fSlow30 * fRec30[0]) + (fSlow29 * fRec31[0])) - fRec32[0])));
			double fTemp9 = ((fRec18[0] + (fSlow56 * ((fSlow58 * fVec3[0]) + (fSlow57 * fRec28[0])))) + fRec29[0]);
			fRec36[0] = (fRec35[1] + fRec36[1]);
			fRec37[0] = (fRec36[1] + fRec37[1]);
			fRec35[0] = (0 - (fSlow62 * (((fSlow64 * fRec36[0]) + (fSlow63 * fRec37[0])) - (fSlow66 * fTemp5))));
			fRec38[0] = (fRec35[0] + fRec38[1]);
			double fTemp10 = (fRec38[0] - fRec35[0]);
			fVec4[0] = fTemp10;
			fRec39[0] = (fRec39[1] + fVec4[1]);
			fRec41[0] = (fRec40[1] + fRec41[1]);
			fRec42[0] = (fRec41[1] + fRec42[1]);
			fRec40[0] = (0 - (fSlow62 * (((fSlow64 * fRec41[0]) + (fSlow63 * fRec42[0])) - fTemp8)));
			double fTemp11 = ((fRec35[0] + (fSlow67 * ((fSlow69 * fVec4[0]) + (fSlow68 * fRec39[0])))) + fRec40[0]);
			fRec44[0] = (fRec43[1] + fRec44[1]);
			fRec45[0] = (fRec44[1] + fRec45[1]);
			fRec43[0] = (fSlow62 * ((fSlow72 * fTemp0) - ((fSlow64 * fRec44[0]) + (fSlow63 * fRec45[0]))));
			fRec46[0] = (fRec43[0] + fRec46[1]);
			double fTemp12 = (fRec46[0] - fRec43[0]);
			fVec5[0] = fTemp12;
			fRec47[0] = (fRec47[1] + fVec5[1]);
			fRec49[0] = (fRec48[1] + fRec49[1]);
			fRec50[0] = (fRec49[1] + fRec50[1]);
			fRec48[0] = (0 - (fSlow62 * (((fSlow64 * fRec49[0]) + (fSlow63 * fRec50[0])) - fTemp3)));
			double fTemp13 = ((fRec43[0] + (fSlow73 * ((fSlow75 * fVec5[0]) + (fSlow74 * fRec47[0])))) + fRec48[0]);
			double fTemp14 = (fSlow76 * ((fSlow77 * fTemp0) + (fSlow78 * fTemp5)));
			fRec53[0] = (fRec52[1] + fRec53[1]);
			fRec52[0] = (fSlow82 * ((fSlow83 * fTemp0) - (fSlow84 * fRec53[0])));
			fRec51[0] = (fRec52[0] + fRec51[1]);
			fRec55[0] = (fRec54[1] + fRec55[1]);
			fRec54[0] = (fSlow82 * (fTemp3 - (fSlow84 * fRec55[0])));
			double fTemp15 = ((fSlow81 * (fRec51[0] - fRec52[0])) + (fRec52[0] + fRec54[0]));
			double fTemp16 = (fSlow79 * fTemp15);
			fRec59[0] = (fRec58[1] + fRec59[1]);
			fRec60[0] = (fRec59[1] + fRec60[1]);
			fRec58[0] = (fSlow90 * ((fSlow92 * fTemp0) - ((fSlow94 * fRec59[0]) + (fSlow93 * fRec60[0]))));
			fRec61[0] = (fRec58[0] + fRec61[1]);
			double fTemp17 = (fRec61[0] - fRec58[0]);
			fVec6[0] = fTemp17;
			fRec62[0] = (fRec62[1] + fVec6[1]);
			fRec63[0] = (fRec57[1] + fRec63[1]);
			fRec57[0] = (fSlow89 * ((fSlow87 * (fRec58[0] + (fSlow95 * ((fSlow97 * fVec6[0]) + (fSlow96 * fRec62[0]))))) - (fSlow98 * fRec63[0])));
			fRec56[0] = (fRec57[0] + fRec56[1]);
			fRec66[0] = (fRec65[1] + fRec66[1]);
			fRec67[0] = (fRec66[1] + fRec67[1]);
			fRec65[0] = (0 - (fSlow90 * (((fSlow94 * fRec66[0]) + (fSlow93 * fRec67[0])) - fTemp3)));
			fRec68[0] = (fRec64[1] + fRec68[1]);
			fRec64[0] = (fSlow89 * (fRec65[0] - (fSlow98 * fRec68[0])));
			double fTemp18 = ((fSlow88 * (fRec56[0] - fRec57[0])) + (fRec57[0] + fRec64[0]));
			fRec73[0] = (fRec72[1] + fRec73[1]);
			fRec74[0] = (fRec73[1] + fRec74[1]);
			fRec72[0] = (fSlow106 * ((fSlow108 * fTemp0) - ((fSlow110 * fRec73[0]) + (fSlow109 * fRec74[0]))));
			fRec75[0] = (fRec72[0] + fRec75[1]);
			double fTemp19 = (fRec75[0] - fRec72[0]);
			fVec7[0] = fTemp19;
			fRec76[0] = (fRec76[1] + fVec7[1]);
			fRec77[0] = (fRec71[1] + fRec77[1]);
			fRec78[0] = (fRec77[1] + fRec78[1]);
			fRec71[0] = (fSlow104 * ((fSlow105 * (fRec72[0] + (fSlow111 * ((fSlow113 * fVec7[0]) + (fSlow112 * fRec76[0]))))) - ((fSlow115 * fRec77[0]) + (fSlow114 * fRec78[0]))));
			fRec79[0] = (fRec71[0] + fRec79[1]);
			double fTemp20 = (fRec79[0] - fRec71[0]);
			fVec8[0] = fTemp20;
			fRec80[0] = (fRec80[1] + fVec8[1]);
			fRec81[0] = (fRec70[1] + fRec81[1]);
			fRec70[0] = (fSlow103 * ((fSlow101 * (fRec71[0] + (fSlow116 * ((fSlow118 * fVec8[0]) + (fSlow117 * fRec80[0]))))) - (fSlow119 * fRec81[0])));
			fRec69[0] = (fRec70[0] + fRec69[1]);
			fRec84[0] = (fRec83[1] + fRec84[1]);
			fRec85[0] = (fRec84[1] + fRec85[1]);
			fRec87[0] = (fRec86[1] + fRec87[1]);
			fRec88[0] = (fRec87[1] + fRec88[1]);
			fRec86[0] = (0 - (fSlow106 * (((fSlow110 * fRec87[0]) + (fSlow109 * fRec88[0])) - fTemp3)));
			fRec83[0] = (0 - (fSlow104 * (((fSlow115 * fRec84[0]) + (fSlow114 * fRec85[0])) - fRec86[0])));
			fRec89[0] = (fRec82[1] + fRec89[1]);
			fRec82[0] = (fSlow103 * (fRec83[0] - (fSlow119 * fRec89[0])));
			double fTemp21 = ((fSlow102 * (fRec69[0] - fRec70[0])) + (fRec70[0] + fRec82[0]));
			fRec92[0] = (fRec91[1] + fRec92[1]);
			fRec91[0] = (fSlow82 * ((fSlow122 * fTemp5) - (fSlow84 * fRec92[0])));
			fRec90[0] = (fRec91[0] + fRec90[1]);
			fRec94[0] = (fRec93[1] + fRec94[1]);
			fRec93[0] = (fSlow82 * (fTemp8 - (fSlow84 * fRec94[0])));
			double fTemp22 = ((fSlow121 * (fRec90[0] - fRec91[0])) + (fRec91[0] + fRec93[0]));
			double fTemp23 = (fSlow79 * fTemp22);
			fRec98[0] = (fRec97[1] + fRec98[1]);
			fRec99[0] = (fRec98[1] + fRec99[1]);
			fRec97[0] = (0 - (fSlow90 * (((fSlow94 * fRec98[0]) + (fSlow93 * fRec99[0])) - (fSlow127 * fTemp5))));
			fRec100[0] = (fRec97[0] + fRec100[1]);
			double fTemp24 = (fRec100[0] - fRec97[0]);
			fVec9[0] = fTemp24;
			fRec101[0] = (fRec101[1] + fVec9[1]);
			fRec102[0] = (fRec96[1] + fRec102[1]);
			fRec96[0] = (fSlow89 * ((fSlow124 * (fRec97[0] + (fSlow128 * ((fSlow130 * fVec9[0]) + (fSlow129 * fRec101[0]))))) - (fSlow98 * fRec102[0])));
			fRec95[0] = (fRec96[0] + fRec95[1]);
			fRec105[0] = (fRec104[1] + fRec105[1]);
			fRec106[0] = (fRec105[1] + fRec106[1]);
			fRec104[0] = (0 - (fSlow90 * (((fSlow94 * fRec105[0]) + (fSlow93 * fRec106[0])) - fTemp8)));
			fRec107[0] = (fRec103[1] + fRec107[1]);
			fRec103[0] = (fSlow89 * (fRec104[0] - (fSlow98 * fRec107[0])));
			double fTemp25 = ((fSlow125 * (fRec95[0] - fRec96[0])) + (fRec96[0] + fRec103[0]));
			fRec111[0] = (fRec110[1] + fRec111[1]);
			fRec112[0] = (fRec111[1] + fRec112[1]);
			fRec114[0] = (fRec113[1] + fRec114[1]);
			fRec115[0] = (fRec114[1] + fRec115[1]);
			fRec113[0] = (0 - (fSlow106 * (((fSlow110 * fRec114[0]) + (fSlow109 * fRec115[0])) - (fSlow136 * fTemp5))));
			fRec116[0] = (fRec113[0] + fRec116[1]);
			double fTemp26 = (fRec116[0] - fRec113[0]);
			fVec10[0] = fTemp26;
			fRec117[0] = (fRec117[1] + fVec10[1]);
			fRec110[0] = (0 - (fSlow104 * (((fSlow115 * fRec111[0]) + (fSlow114 * fRec112[0])) - (fSlow134 * (fRec113[0] + (fSlow137 * ((fSlow139 * fVec10[0]) + (fSlow138 * fRec117[0]))))))));
			fRec118[0] = (fRec110[0] + fRec118[1]);
			double fTemp27 = (fRec118[0] - fRec110[0]);
			fVec11[0] = fTemp27;
			fRec119[0] = (fRec119[1] + fVec11[1]);
			fRec120[0] = (fRec109[1] + fRec120[1]);
			fRec109[0] = (fSlow103 * ((fSlow132 * (fRec110[0] + (fSlow140 * ((fSlow142 * fVec11[0]) + (fSlow141 * fRec119[0]))))) - (fSlow119 * fRec120[0])));
			fRec108[0] = (fRec109[0] + fRec108[1]);
			fRec123[0] = (fRec122[1] + fRec123[1]);
			fRec124[0] = (fRec123[1] + fRec124[1]);
			fRec126[0] = (fRec125[1] + fRec126[1]);
			fRec127[0] = (fRec126[1] + fRec127[1]);
			fRec125[0] = (0 - (fSlow106 * (((fSlow110 * fRec126[0]) + (fSlow109 * fRec127[0])) - fTemp8)));
			fRec122[0] = (0 - (fSlow104 * (((fSlow115 * fRec123[0]) + (fSlow114 * fRec124[0])) - fRec125[0])));
			fRec128[0] = (fRec121[1] + fRec128[1]);
			fRec121[0] = (fSlow103 * (fRec122[0] - (fSlow119 * fRec128[0])));
			double fTemp28 = ((fSlow133 * (fRec108[0] - fRec109[0])) + (fRec109[0] + fRec121[0]));
			double fTemp29 = (0.012698412698412698 * ((fSlow0 * ((fSlow9 * fTemp4) + (fSlow43 * fTemp9))) + ((fSlow60 * ((fSlow61 * fTemp11) + (fSlow70 * fTemp13))) + (fTemp14 + ((fSlow7 * ((fTemp16 + (fSlow86 * fTemp18)) + (fSlow100 * fTemp21))) + (fSlow41 * ((fTemp23 + (fSlow123 * fTemp25)) + (fSlow131 * fTemp28))))))));
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph0 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp29;
			double fTemp30 = (0.012698412698412698 * (((((fTemp14 + (fSlow143 * ((fSlow145 * fTemp18) + (fSlow147 * fTemp25)))) + (fSlow148 * ((fSlow150 * fTemp21) + (fSlow152 * fTemp28)))) + (fSlow60 * ((fSlow153 * fTemp13) + (fSlow154 * fTemp11)))) + (fSlow0 * ((fSlow155 * fTemp4) + (fSlow156 * fTemp9)))) + (fSlow79 * ((fSlow39 * fTemp22) + (fSlow5 * fTemp15)))));
			fRec129[0] = max((fRec129[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fbargraph1 = fRec129[0];
			output1[i] = (FAUSTFLOAT)fTemp30;
			double fTemp31 = (0.012698412698412698 * (((((fTemp14 + (fSlow0 * ((fSlow159 * fTemp9) + (fSlow162 * fTemp4)))) + (fSlow148 * ((fSlow164 * fTemp21) + (fSlow166 * fTemp28)))) + (fSlow143 * ((fSlow167 * fTemp25) + (fSlow168 * fTemp18)))) + (fSlow60 * ((fSlow169 * fTemp11) + (fSlow170 * fTemp13)))) + (fSlow79 * ((fSlow160 * fTemp15) + (fSlow157 * fTemp22)))));
			fRec130[0] = max((fRec130[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fbargraph2 = fRec130[0];
			output2[i] = (FAUSTFLOAT)fTemp31;
			double fTemp32 = (0.012698412698412698 * ((fSlow60 * ((fSlow173 * fTemp13) + (fSlow176 * fTemp11))) + ((fSlow143 * ((fSlow178 * fTemp25) + (fSlow180 * fTemp18))) + ((fSlow148 * ((fSlow181 * fTemp21) + (fSlow182 * fTemp28))) + ((fSlow0 * ((fSlow183 * fTemp9) + (fSlow184 * fTemp4))) + (fTemp14 + (fSlow79 * ((fSlow171 * fTemp15) + (fSlow174 * fTemp22)))))))));
			fRec131[0] = max((fRec131[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph3 = fRec131[0];
			output3[i] = (FAUSTFLOAT)fTemp32;
			double fTemp33 = (0.012698412698412698 * ((fSlow60 * ((fSlow187 * fTemp13) + (fSlow190 * fTemp11))) + ((fSlow143 * ((fSlow192 * fTemp25) + (fSlow194 * fTemp18))) + ((fSlow148 * ((fSlow195 * fTemp21) + (fSlow196 * fTemp28))) + ((fSlow0 * ((fSlow197 * fTemp9) + (fSlow198 * fTemp4))) + (fTemp14 + (fSlow79 * ((fSlow185 * fTemp15) + (fSlow188 * fTemp22)))))))));
			fRec132[0] = max((fRec132[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph4 = fRec132[0];
			output4[i] = (FAUSTFLOAT)fTemp33;
			double fTemp34 = (0.012698412698412698 * ((fSlow0 * ((fSlow201 * fTemp4) + (fSlow204 * fTemp9))) + (((fSlow202 * ((fSlow205 * fTemp28) + (fTemp23 + (fSlow206 * fTemp25)))) + (fTemp14 + (fSlow199 * ((fTemp16 + (fSlow207 * fTemp18)) + (fSlow208 * fTemp21))))) + (fSlow60 * ((fSlow209 * fTemp11) + (fSlow210 * fTemp13))))));
			fRec133[0] = max((fRec133[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fbargraph5 = fRec133[0];
			output5[i] = (FAUSTFLOAT)fTemp34;
			double fTemp35 = (0.022574955908289243 * ((((fSlow213 * ((fTemp23 + (fSlow215 * fTemp25)) + (fSlow216 * fTemp28))) + (fTemp14 + (fSlow219 * ((fTemp16 + (fSlow221 * fTemp18)) + (fSlow222 * fTemp21))))) + (fSlow60 * ((fSlow223 * fTemp13) + (fSlow224 * fTemp11)))) + (fSlow0 * ((fSlow225 * fTemp9) + (fSlow226 * fTemp4)))));
			fRec134[0] = max((fRec134[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fbargraph6 = fRec134[0];
			output6[i] = (FAUSTFLOAT)fTemp35;
			double fTemp36 = (0.022574955908289243 * ((fSlow0 * ((fSlow230 * fTemp4) + (fSlow234 * fTemp9))) + ((fSlow60 * ((fSlow235 * fTemp13) + (fSlow236 * fTemp11))) + ((fSlow232 * ((fSlow237 * fTemp28) + (fTemp23 + (fSlow238 * fTemp25)))) + (fTemp14 + (fSlow228 * ((fTemp16 + (fSlow239 * fTemp18)) + (fSlow240 * fTemp21))))))));
			fRec135[0] = max((fRec135[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph7 = fRec135[0];
			output7[i] = (FAUSTFLOAT)fTemp36;
			double fTemp37 = (0.022574955908289243 * ((fSlow0 * ((fSlow244 * fTemp4) + (fSlow248 * fTemp9))) + ((fSlow60 * ((fSlow249 * fTemp13) + (fSlow250 * fTemp11))) + ((fSlow246 * ((fSlow251 * fTemp28) + (fTemp23 + (fSlow252 * fTemp25)))) + (fTemp14 + (fSlow242 * ((fTemp16 + (fSlow253 * fTemp18)) + (fSlow254 * fTemp21))))))));
			fRec136[0] = max((fRec136[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fbargraph8 = fRec136[0];
			output8[i] = (FAUSTFLOAT)fTemp37;
			double fTemp38 = (0.022574955908289243 * ((fSlow0 * ((fSlow258 * fTemp4) + (fSlow262 * fTemp9))) + ((fSlow60 * ((fSlow263 * fTemp13) + (fSlow264 * fTemp11))) + ((fSlow260 * ((fSlow265 * fTemp28) + (fTemp23 + (fSlow266 * fTemp25)))) + (fTemp14 + (fSlow256 * ((fTemp16 + (fSlow267 * fTemp18)) + (fSlow268 * fTemp21))))))));
			fRec137[0] = max((fRec137[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fbargraph9 = fRec137[0];
			output9[i] = (FAUSTFLOAT)fTemp38;
			double fTemp39 = (0.022574955908289243 * ((fSlow60 * ((fSlow271 * fTemp13) + (fSlow274 * fTemp11))) + ((fSlow143 * ((fSlow276 * fTemp25) + (fSlow278 * fTemp18))) + ((fSlow148 * ((fSlow279 * fTemp21) + (fSlow280 * fTemp28))) + ((fSlow0 * ((fSlow281 * fTemp9) + (fSlow282 * fTemp4))) + (fTemp14 + (fSlow79 * ((fSlow269 * fTemp15) + (fSlow272 * fTemp22)))))))));
			fRec138[0] = max((fRec138[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fbargraph10 = fRec138[0];
			output10[i] = (FAUSTFLOAT)fTemp39;
			double fTemp40 = (0.022574955908289243 * ((fSlow60 * ((fSlow285 * fTemp13) + (fSlow288 * fTemp11))) + ((fSlow143 * ((fSlow290 * fTemp25) + (fSlow292 * fTemp18))) + ((fSlow148 * ((fSlow293 * fTemp21) + (fSlow294 * fTemp28))) + ((fSlow0 * ((fSlow295 * fTemp9) + (fSlow296 * fTemp4))) + (fTemp14 + (fSlow79 * ((fSlow283 * fTemp15) + (fSlow286 * fTemp22)))))))));
			fRec139[0] = max((fRec139[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fbargraph11 = fRec139[0];
			output11[i] = (FAUSTFLOAT)fTemp40;
			double fTemp41 = (0.022574955908289243 * ((fSlow60 * ((fSlow299 * fTemp13) + (fSlow302 * fTemp11))) + ((fSlow143 * ((fSlow304 * fTemp25) + (fSlow306 * fTemp18))) + ((fSlow148 * ((fSlow307 * fTemp21) + (fSlow308 * fTemp28))) + ((fSlow0 * ((fSlow309 * fTemp9) + (fSlow310 * fTemp4))) + (fTemp14 + (fSlow79 * ((fSlow297 * fTemp15) + (fSlow300 * fTemp22)))))))));
			fRec140[0] = max((fRec140[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fbargraph12 = fRec140[0];
			output12[i] = (FAUSTFLOAT)fTemp41;
			double fTemp42 = (0.022574955908289243 * ((fSlow60 * ((fSlow313 * fTemp13) + (fSlow316 * fTemp11))) + ((fSlow143 * ((fSlow318 * fTemp25) + (fSlow320 * fTemp18))) + ((fSlow148 * ((fSlow321 * fTemp21) + (fSlow322 * fTemp28))) + ((fSlow0 * ((fSlow323 * fTemp9) + (fSlow324 * fTemp4))) + (fTemp14 + (fSlow79 * ((fSlow311 * fTemp15) + (fSlow314 * fTemp22)))))))));
			fRec141[0] = max((fRec141[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fbargraph13 = fRec141[0];
			output13[i] = (FAUSTFLOAT)fTemp42;
			double fTemp43 = (0.022574955908289243 * ((((fSlow325 * ((fTemp23 + (fSlow327 * fTemp25)) + (fSlow328 * fTemp28))) + (fTemp14 + (fSlow329 * ((fSlow331 * fTemp21) + (fTemp16 + (fSlow332 * fTemp18)))))) + (fSlow60 * ((fSlow333 * fTemp13) + (fSlow334 * fTemp11)))) + (fSlow0 * ((fSlow335 * fTemp9) + (fSlow336 * fTemp4)))));
			fRec142[0] = max((fRec142[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fbargraph14 = fRec142[0];
			output14[i] = (FAUSTFLOAT)fTemp43;
			double fTemp44 = (0.022574955908289243 * ((fSlow0 * ((fSlow339 * fTemp4) + (fSlow342 * fTemp9))) + (((fSlow340 * ((fSlow343 * fTemp28) + (fTemp23 + (fSlow344 * fTemp25)))) + (fTemp14 + (fSlow337 * ((fTemp16 + (fSlow345 * fTemp18)) + (fSlow346 * fTemp21))))) + (fSlow60 * ((fSlow347 * fTemp11) + (fSlow348 * fTemp13))))));
			fRec143[0] = max((fRec143[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fbargraph15 = fRec143[0];
			output15[i] = (FAUSTFLOAT)fTemp44;
			double fTemp45 = (0.022574955908289243 * ((fSlow0 * ((fSlow351 * fTemp4) + (fSlow354 * fTemp9))) + (((fSlow352 * ((fSlow355 * fTemp28) + (fTemp23 + (fSlow356 * fTemp25)))) + (fTemp14 + (fSlow349 * ((fTemp16 + (fSlow357 * fTemp18)) + (fSlow358 * fTemp21))))) + (fSlow60 * ((fSlow359 * fTemp11) + (fSlow360 * fTemp13))))));
			fRec144[0] = max((fRec144[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fbargraph16 = fRec144[0];
			output16[i] = (FAUSTFLOAT)fTemp45;
			double fTemp46 = (0.022574955908289243 * ((fSlow0 * ((fSlow363 * fTemp4) + (fSlow366 * fTemp9))) + (((fSlow364 * ((fSlow367 * fTemp28) + (fTemp23 + (fSlow368 * fTemp25)))) + (fTemp14 + (fSlow361 * ((fTemp16 + (fSlow369 * fTemp18)) + (fSlow370 * fTemp21))))) + (fSlow60 * ((fSlow371 * fTemp11) + (fSlow372 * fTemp13))))));
			fRec145[0] = max((fRec145[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fbargraph17 = fRec145[0];
			output17[i] = (FAUSTFLOAT)fTemp46;
			double fTemp47 = (0.02109375 * ((((fSlow375 * ((fTemp23 + (fSlow377 * fTemp25)) + (fSlow378 * fTemp28))) + (fTemp14 + (fSlow381 * ((fTemp16 + (fSlow383 * fTemp18)) + (fSlow384 * fTemp21))))) + (fSlow60 * ((fSlow385 * fTemp13) + (fSlow386 * fTemp11)))) + (fSlow0 * ((fSlow387 * fTemp9) + (fSlow388 * fTemp4)))));
			fRec146[0] = max((fRec146[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fbargraph18 = fRec146[0];
			output18[i] = (FAUSTFLOAT)fTemp47;
			double fTemp48 = (0.02109375 * ((fSlow0 * ((fSlow392 * fTemp4) + (fSlow396 * fTemp9))) + ((fSlow60 * ((fSlow397 * fTemp13) + (fSlow398 * fTemp11))) + ((fSlow394 * ((fSlow399 * fTemp28) + (fTemp23 + (fSlow400 * fTemp25)))) + (fTemp14 + (fSlow390 * ((fTemp16 + (fSlow401 * fTemp18)) + (fSlow402 * fTemp21))))))));
			fRec147[0] = max((fRec147[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fbargraph19 = fRec147[0];
			output19[i] = (FAUSTFLOAT)fTemp48;
			double fTemp49 = (0.02109375 * ((fSlow0 * ((fSlow406 * fTemp4) + (fSlow410 * fTemp9))) + ((fSlow60 * ((fSlow411 * fTemp13) + (fSlow412 * fTemp11))) + ((fSlow408 * ((fSlow413 * fTemp28) + (fTemp23 + (fSlow414 * fTemp25)))) + (fTemp14 + (fSlow404 * ((fTemp16 + (fSlow415 * fTemp18)) + (fSlow416 * fTemp21))))))));
			fRec148[0] = max((fRec148[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fbargraph20 = fRec148[0];
			output20[i] = (FAUSTFLOAT)fTemp49;
			double fTemp50 = (0.02109375 * (((fSlow0 * ((fSlow420 * fTemp9) + (fSlow424 * fTemp4))) + ((fSlow418 * ((fTemp23 + (fSlow425 * fTemp25)) + (fSlow426 * fTemp28))) + (fTemp14 + (fSlow422 * ((fTemp16 + (fSlow427 * fTemp18)) + (fSlow428 * fTemp21)))))) + (fSlow60 * ((fSlow429 * fTemp11) + (fSlow430 * fTemp13)))));
			fRec149[0] = max((fRec149[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fbargraph21 = fRec149[0];
			output21[i] = (FAUSTFLOAT)fTemp50;
			double fTemp51 = (0.02109375 * (((((fSlow60 * ((fSlow433 * fTemp11) + (fSlow436 * fTemp13))) + (fSlow0 * ((fSlow437 * fTemp4) + (fSlow438 * fTemp9)))) + fTemp14) + (fSlow434 * ((fTemp16 + (fSlow439 * fTemp18)) + (fSlow440 * fTemp21)))) + (fSlow431 * ((fTemp23 + (fSlow441 * fTemp25)) + (fSlow442 * fTemp28)))));
			fRec150[0] = max((fRec150[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fbargraph22 = fRec150[0];
			output22[i] = (FAUSTFLOAT)fTemp51;
			double fTemp52 = (0.02109375 * (((fSlow60 * ((fSlow445 * fTemp13) + (fSlow448 * fTemp11))) + ((fSlow0 * ((fSlow449 * fTemp4) + (fSlow450 * fTemp9))) + ((fSlow443 * (((fSlow451 * fTemp18) + (fSlow452 * fTemp21)) + fTemp16)) + (fSlow446 * (((fSlow453 * fTemp25) + (fSlow454 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec151[0] = max((fRec151[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fbargraph23 = fRec151[0];
			output23[i] = (FAUSTFLOAT)fTemp52;
			double fTemp53 = (0.02109375 * (((fSlow60 * ((fSlow457 * fTemp13) + (fSlow460 * fTemp11))) + ((fSlow0 * ((fSlow461 * fTemp4) + (fSlow462 * fTemp9))) + ((fSlow455 * (((fSlow463 * fTemp21) + (fSlow464 * fTemp18)) + fTemp16)) + (fSlow458 * (((fSlow465 * fTemp25) + (fSlow466 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec152[0] = max((fRec152[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fbargraph24 = fRec152[0];
			output24[i] = (FAUSTFLOAT)fTemp53;
			double fTemp54 = (0.02109375 * (((fSlow60 * ((fSlow469 * fTemp13) + (fSlow472 * fTemp11))) + ((fSlow0 * ((fSlow473 * fTemp4) + (fSlow474 * fTemp9))) + ((fSlow467 * (((fSlow475 * fTemp21) + (fSlow476 * fTemp18)) + fTemp16)) + (fSlow470 * (((fSlow477 * fTemp25) + (fSlow478 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec153[0] = max((fRec153[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fbargraph25 = fRec153[0];
			output25[i] = (FAUSTFLOAT)fTemp54;
			double fTemp55 = (0.02017333553791887 * (((fSlow0 * ((fSlow483 * fTemp4) + (fSlow488 * fTemp9))) + (((fSlow481 * (((fSlow489 * fTemp18) + (fSlow490 * fTemp21)) + fTemp16)) + (fSlow486 * (((fSlow491 * fTemp25) + (fSlow492 * fTemp28)) + fTemp23))) + (fSlow60 * ((fSlow493 * fTemp13) + (fSlow494 * fTemp11))))) + fTemp14));
			fRec154[0] = max((fRec154[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fbargraph26 = fRec154[0];
			output26[i] = (FAUSTFLOAT)fTemp55;
			double fTemp56 = (0.02017333553791887 * (((((fSlow496 * (((fSlow498 * fTemp18) + (fSlow499 * fTemp21)) + fTemp16)) + (fSlow501 * (((fSlow503 * fTemp28) + (fSlow504 * fTemp25)) + fTemp23))) + (fSlow0 * ((fSlow505 * fTemp4) + (fSlow506 * fTemp9)))) + (fSlow60 * ((fSlow507 * fTemp13) + (fSlow508 * fTemp11)))) + fTemp14));
			fRec155[0] = max((fRec155[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fbargraph27 = fRec155[0];
			output27[i] = (FAUSTFLOAT)fTemp56;
			double fTemp57 = (0.02017333553791887 * (((fSlow60 * ((fSlow512 * fTemp11) + (fSlow516 * fTemp13))) + (((fSlow514 * (((fSlow517 * fTemp18) + (fSlow518 * fTemp21)) + fTemp16)) + (fSlow510 * (((fSlow519 * fTemp25) + (fSlow520 * fTemp28)) + fTemp23))) + (fSlow0 * ((fSlow521 * fTemp9) + (fSlow522 * fTemp4))))) + fTemp14));
			fRec156[0] = max((fRec156[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fbargraph28 = fRec156[0];
			output28[i] = (FAUSTFLOAT)fTemp57;
			double fTemp58 = (0.02017333553791887 * (((fSlow60 * ((fSlow526 * fTemp13) + (fSlow530 * fTemp11))) + ((fSlow0 * ((fSlow531 * fTemp4) + (fSlow532 * fTemp9))) + ((fSlow524 * (((fSlow533 * fTemp18) + (fSlow534 * fTemp21)) + fTemp16)) + (fSlow528 * (((fSlow535 * fTemp25) + (fSlow536 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec157[0] = max((fRec157[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fbargraph29 = fRec157[0];
			output29[i] = (FAUSTFLOAT)fTemp58;
			double fTemp59 = (0.02017333553791887 * (((fSlow0 * ((fSlow541 * fTemp4) + (fSlow546 * fTemp9))) + (((fSlow539 * (((fSlow547 * fTemp18) + (fSlow548 * fTemp21)) + fTemp16)) + (fSlow544 * (((fSlow549 * fTemp25) + (fSlow550 * fTemp28)) + fTemp23))) + (fSlow60 * ((fSlow551 * fTemp13) + (fSlow552 * fTemp11))))) + fTemp14));
			fRec158[0] = max((fRec158[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fbargraph30 = fRec158[0];
			output30[i] = (FAUSTFLOAT)fTemp59;
			double fTemp60 = (0.02017333553791887 * (((fSlow60 * ((fSlow556 * fTemp13) + (fSlow560 * fTemp11))) + ((fSlow0 * ((fSlow561 * fTemp4) + (fSlow562 * fTemp9))) + ((fSlow554 * (((fSlow563 * fTemp18) + (fSlow564 * fTemp21)) + fTemp16)) + (fSlow558 * (((fSlow565 * fTemp25) + (fSlow566 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec159[0] = max((fRec159[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fbargraph31 = fRec159[0];
			output31[i] = (FAUSTFLOAT)fTemp60;
			double fTemp61 = (0.02017333553791887 * (((fSlow60 * ((fSlow570 * fTemp13) + (fSlow574 * fTemp11))) + ((fSlow0 * ((fSlow575 * fTemp4) + (fSlow576 * fTemp9))) + ((fSlow568 * (((fSlow577 * fTemp18) + (fSlow578 * fTemp21)) + fTemp16)) + (fSlow572 * (((fSlow579 * fTemp25) + (fSlow580 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec160[0] = max((fRec160[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fbargraph32 = fRec160[0];
			output32[i] = (FAUSTFLOAT)fTemp61;
			double fTemp62 = (0.02017333553791887 * (((fSlow60 * ((fSlow584 * fTemp13) + (fSlow588 * fTemp11))) + ((fSlow0 * ((fSlow589 * fTemp4) + (fSlow590 * fTemp9))) + ((fSlow582 * (((fSlow591 * fTemp18) + (fSlow592 * fTemp21)) + fTemp16)) + (fSlow586 * (((fSlow593 * fTemp25) + (fSlow594 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec161[0] = max((fRec161[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fbargraph33 = fRec161[0];
			output33[i] = (FAUSTFLOAT)fTemp62;
			double fTemp63 = (0.02017333553791887 * (((fSlow60 * ((fSlow598 * fTemp13) + (fSlow602 * fTemp11))) + ((fSlow0 * ((fSlow603 * fTemp4) + (fSlow604 * fTemp9))) + ((fSlow596 * (((fSlow605 * fTemp18) + (fSlow606 * fTemp21)) + fTemp16)) + (fSlow600 * (((fSlow607 * fTemp25) + (fSlow608 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec162[0] = max((fRec162[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fbargraph34 = fRec162[0];
			output34[i] = (FAUSTFLOAT)fTemp63;
			double fTemp64 = (0.02017333553791887 * (((fSlow60 * ((fSlow612 * fTemp13) + (fSlow616 * fTemp11))) + ((fSlow0 * ((fSlow617 * fTemp4) + (fSlow618 * fTemp9))) + ((fSlow610 * (((fSlow619 * fTemp18) + (fSlow620 * fTemp21)) + fTemp16)) + (fSlow614 * (((fSlow621 * fTemp25) + (fSlow622 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec163[0] = max((fRec163[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fbargraph35 = fRec163[0];
			output35[i] = (FAUSTFLOAT)fTemp64;
			double fTemp65 = (0.02017333553791887 * (((fSlow60 * ((fSlow626 * fTemp13) + (fSlow630 * fTemp11))) + ((fSlow0 * ((fSlow631 * fTemp4) + (fSlow632 * fTemp9))) + ((fSlow624 * (((fSlow633 * fTemp18) + (fSlow634 * fTemp21)) + fTemp16)) + (fSlow628 * (((fSlow635 * fTemp25) + (fSlow636 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec164[0] = max((fRec164[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fbargraph36 = fRec164[0];
			output36[i] = (FAUSTFLOAT)fTemp65;
			double fTemp66 = (0.02017333553791887 * (((fSlow60 * ((fSlow640 * fTemp13) + (fSlow644 * fTemp11))) + ((fSlow0 * ((fSlow645 * fTemp4) + (fSlow646 * fTemp9))) + ((fSlow638 * (((fSlow647 * fTemp18) + (fSlow648 * fTemp21)) + fTemp16)) + (fSlow642 * (((fSlow649 * fTemp25) + (fSlow650 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec165[0] = max((fRec165[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fbargraph37 = fRec165[0];
			output37[i] = (FAUSTFLOAT)fTemp66;
			double fTemp67 = (0.02017333553791887 * (((fSlow60 * ((fSlow653 * fTemp13) + (fSlow656 * fTemp11))) + ((fSlow0 * ((fSlow657 * fTemp4) + (fSlow658 * fTemp9))) + ((fSlow651 * (((fSlow659 * fTemp21) + (fSlow660 * fTemp18)) + fTemp16)) + (fSlow654 * (((fSlow661 * fTemp25) + (fSlow662 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec166[0] = max((fRec166[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp67))))));
			fbargraph38 = fRec166[0];
			output38[i] = (FAUSTFLOAT)fTemp67;
			double fTemp68 = (0.02017333553791887 * (((fSlow60 * ((fSlow665 * fTemp13) + (fSlow668 * fTemp11))) + ((fSlow0 * ((fSlow669 * fTemp4) + (fSlow670 * fTemp9))) + ((fSlow663 * (((fSlow671 * fTemp21) + (fSlow672 * fTemp18)) + fTemp16)) + (fSlow666 * (((fSlow673 * fTemp25) + (fSlow674 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec167[0] = max((fRec167[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fbargraph39 = fRec167[0];
			output39[i] = (FAUSTFLOAT)fTemp68;
			double fTemp69 = (0.02017333553791887 * (((fSlow60 * ((fSlow677 * fTemp13) + (fSlow680 * fTemp11))) + ((fSlow0 * ((fSlow681 * fTemp4) + (fSlow682 * fTemp9))) + ((fSlow675 * (((fSlow683 * fTemp21) + (fSlow684 * fTemp18)) + fTemp16)) + (fSlow678 * (((fSlow685 * fTemp25) + (fSlow686 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec168[0] = max((fRec168[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp69))))));
			fbargraph40 = fRec168[0];
			output40[i] = (FAUSTFLOAT)fTemp69;
			double fTemp70 = (0.02017333553791887 * (((fSlow60 * ((fSlow689 * fTemp13) + (fSlow692 * fTemp11))) + ((fSlow0 * ((fSlow693 * fTemp4) + (fSlow694 * fTemp9))) + ((fSlow687 * (((fSlow695 * fTemp21) + (fSlow696 * fTemp18)) + fTemp16)) + (fSlow690 * (((fSlow697 * fTemp25) + (fSlow698 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec169[0] = max((fRec169[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp70))))));
			fbargraph41 = fRec169[0];
			output41[i] = (FAUSTFLOAT)fTemp70;
			double fTemp71 = (0.02017333553791887 * (((fSlow60 * ((fSlow701 * fTemp13) + (fSlow704 * fTemp11))) + ((fSlow0 * ((fSlow705 * fTemp4) + (fSlow706 * fTemp9))) + ((fSlow699 * (((fSlow707 * fTemp21) + (fSlow708 * fTemp18)) + fTemp16)) + (fSlow702 * (((fSlow709 * fTemp25) + (fSlow710 * fTemp28)) + fTemp23))))) + fTemp14));
			fRec170[0] = max((fRec170[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fbargraph42 = fRec170[0];
			output42[i] = (FAUSTFLOAT)fTemp71;
			double fTemp72 = (0.02017333553791887 * ((((fSlow711 * ((fTemp23 + (fSlow713 * fTemp25)) + (fSlow714 * fTemp28))) + (fTemp14 + (fSlow715 * (((fSlow717 * fTemp18) + fTemp16) + (fSlow718 * fTemp21))))) + (fSlow60 * ((fSlow719 * fTemp11) + (fSlow720 * fTemp13)))) + (fSlow0 * ((fSlow721 * fTemp4) + (fSlow722 * fTemp9)))));
			fRec171[0] = max((fRec171[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp72))))));
			fbargraph43 = fRec171[0];
			output43[i] = (FAUSTFLOAT)fTemp72;
			double fTemp73 = (0.02017333553791887 * ((((fSlow723 * ((fTemp23 + (fSlow725 * fTemp25)) + (fSlow726 * fTemp28))) + (fTemp14 + (fSlow727 * ((fSlow729 * fTemp21) + (fTemp16 + (fSlow730 * fTemp18)))))) + (fSlow60 * ((fSlow731 * fTemp11) + (fSlow732 * fTemp13)))) + (fSlow0 * ((fSlow733 * fTemp9) + (fSlow734 * fTemp4)))));
			fRec172[0] = max((fRec172[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp73))))));
			fbargraph44 = fRec172[0];
			output44[i] = (FAUSTFLOAT)fTemp73;
			double fTemp74 = (0.02017333553791887 * (((fSlow60 * ((fSlow737 * fTemp11) + (fSlow740 * fTemp13))) + ((fSlow735 * ((fTemp23 + (fSlow741 * fTemp25)) + (fSlow742 * fTemp28))) + (fTemp14 + (fSlow738 * ((fTemp16 + (fSlow743 * fTemp18)) + (fSlow744 * fTemp21)))))) + (fSlow0 * ((fSlow745 * fTemp4) + (fSlow746 * fTemp9)))));
			fRec173[0] = max((fRec173[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp74))))));
			fbargraph45 = fRec173[0];
			output45[i] = (FAUSTFLOAT)fTemp74;
			double fTemp75 = (0.02017333553791887 * ((fSlow60 * ((fSlow749 * fTemp11) + (fSlow752 * fTemp13))) + (((fSlow747 * ((fTemp23 + (fSlow753 * fTemp25)) + (fSlow754 * fTemp28))) + (fTemp14 + (fSlow750 * ((fTemp16 + (fSlow755 * fTemp18)) + (fSlow756 * fTemp21))))) + (fSlow0 * ((fSlow757 * fTemp9) + (fSlow758 * fTemp4))))));
			fRec174[0] = max((fRec174[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp75))))));
			fbargraph46 = fRec174[0];
			output46[i] = (FAUSTFLOAT)fTemp75;
			double fTemp76 = (0.02017333553791887 * ((((fSlow759 * ((fTemp23 + (fSlow761 * fTemp25)) + (fSlow762 * fTemp28))) + (fTemp14 + (fSlow763 * ((fSlow765 * fTemp21) + (fTemp16 + (fSlow766 * fTemp18)))))) + (fSlow60 * ((fSlow767 * fTemp11) + (fSlow768 * fTemp13)))) + (fSlow0 * ((fSlow769 * fTemp9) + (fSlow770 * fTemp4)))));
			fRec175[0] = max((fRec175[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp76))))));
			fbargraph47 = fRec175[0];
			output47[i] = (FAUSTFLOAT)fTemp76;
			double fTemp77 = (0.02017333553791887 * (((fSlow60 * ((fSlow773 * fTemp11) + (fSlow776 * fTemp13))) + ((fSlow771 * ((fTemp23 + (fSlow777 * fTemp25)) + (fSlow778 * fTemp28))) + (fTemp14 + (fSlow774 * ((fTemp16 + (fSlow779 * fTemp18)) + (fSlow780 * fTemp21)))))) + (fSlow0 * ((fSlow781 * fTemp4) + (fSlow782 * fTemp9)))));
			fRec176[0] = max((fRec176[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp77))))));
			fbargraph48 = fRec176[0];
			output48[i] = (FAUSTFLOAT)fTemp77;
			double fTemp78 = (0.02017333553791887 * ((fSlow60 * ((fSlow785 * fTemp11) + (fSlow788 * fTemp13))) + (((fSlow783 * ((fTemp23 + (fSlow789 * fTemp25)) + (fSlow790 * fTemp28))) + (fTemp14 + (fSlow786 * ((fTemp16 + (fSlow791 * fTemp18)) + (fSlow792 * fTemp21))))) + (fSlow0 * ((fSlow793 * fTemp9) + (fSlow794 * fTemp4))))));
			fRec177[0] = max((fRec177[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fbargraph49 = fRec177[0];
			output49[i] = (FAUSTFLOAT)fTemp78;
			// post processing
			fRec177[1] = fRec177[0];
			fRec176[1] = fRec176[0];
			fRec175[1] = fRec175[0];
			fRec174[1] = fRec174[0];
			fRec173[1] = fRec173[0];
			fRec172[1] = fRec172[0];
			fRec171[1] = fRec171[0];
			fRec170[1] = fRec170[0];
			fRec169[1] = fRec169[0];
			fRec168[1] = fRec168[0];
			fRec167[1] = fRec167[0];
			fRec166[1] = fRec166[0];
			fRec165[1] = fRec165[0];
			fRec164[1] = fRec164[0];
			fRec163[1] = fRec163[0];
			fRec162[1] = fRec162[0];
			fRec161[1] = fRec161[0];
			fRec160[1] = fRec160[0];
			fRec159[1] = fRec159[0];
			fRec158[1] = fRec158[0];
			fRec157[1] = fRec157[0];
			fRec156[1] = fRec156[0];
			fRec155[1] = fRec155[0];
			fRec154[1] = fRec154[0];
			fRec153[1] = fRec153[0];
			fRec152[1] = fRec152[0];
			fRec151[1] = fRec151[0];
			fRec150[1] = fRec150[0];
			fRec149[1] = fRec149[0];
			fRec148[1] = fRec148[0];
			fRec147[1] = fRec147[0];
			fRec146[1] = fRec146[0];
			fRec145[1] = fRec145[0];
			fRec144[1] = fRec144[0];
			fRec143[1] = fRec143[0];
			fRec142[1] = fRec142[0];
			fRec141[1] = fRec141[0];
			fRec140[1] = fRec140[0];
			fRec139[1] = fRec139[0];
			fRec138[1] = fRec138[0];
			fRec137[1] = fRec137[0];
			fRec136[1] = fRec136[0];
			fRec135[1] = fRec135[0];
			fRec134[1] = fRec134[0];
			fRec133[1] = fRec133[0];
			fRec132[1] = fRec132[0];
			fRec131[1] = fRec131[0];
			fRec130[1] = fRec130[0];
			fRec129[1] = fRec129[0];
			fRec0[1] = fRec0[0];
			fRec121[1] = fRec121[0];
			fRec128[1] = fRec128[0];
			fRec122[1] = fRec122[0];
			fRec125[1] = fRec125[0];
			fRec127[1] = fRec127[0];
			fRec126[1] = fRec126[0];
			fRec124[1] = fRec124[0];
			fRec123[1] = fRec123[0];
			fRec108[1] = fRec108[0];
			fRec109[1] = fRec109[0];
			fRec120[1] = fRec120[0];
			fRec119[1] = fRec119[0];
			fVec11[1] = fVec11[0];
			fRec118[1] = fRec118[0];
			fRec110[1] = fRec110[0];
			fRec117[1] = fRec117[0];
			fVec10[1] = fVec10[0];
			fRec116[1] = fRec116[0];
			fRec113[1] = fRec113[0];
			fRec115[1] = fRec115[0];
			fRec114[1] = fRec114[0];
			fRec112[1] = fRec112[0];
			fRec111[1] = fRec111[0];
			fRec103[1] = fRec103[0];
			fRec107[1] = fRec107[0];
			fRec104[1] = fRec104[0];
			fRec106[1] = fRec106[0];
			fRec105[1] = fRec105[0];
			fRec95[1] = fRec95[0];
			fRec96[1] = fRec96[0];
			fRec102[1] = fRec102[0];
			fRec101[1] = fRec101[0];
			fVec9[1] = fVec9[0];
			fRec100[1] = fRec100[0];
			fRec97[1] = fRec97[0];
			fRec99[1] = fRec99[0];
			fRec98[1] = fRec98[0];
			fRec93[1] = fRec93[0];
			fRec94[1] = fRec94[0];
			fRec90[1] = fRec90[0];
			fRec91[1] = fRec91[0];
			fRec92[1] = fRec92[0];
			fRec82[1] = fRec82[0];
			fRec89[1] = fRec89[0];
			fRec83[1] = fRec83[0];
			fRec86[1] = fRec86[0];
			fRec88[1] = fRec88[0];
			fRec87[1] = fRec87[0];
			fRec85[1] = fRec85[0];
			fRec84[1] = fRec84[0];
			fRec69[1] = fRec69[0];
			fRec70[1] = fRec70[0];
			fRec81[1] = fRec81[0];
			fRec80[1] = fRec80[0];
			fVec8[1] = fVec8[0];
			fRec79[1] = fRec79[0];
			fRec71[1] = fRec71[0];
			fRec78[1] = fRec78[0];
			fRec77[1] = fRec77[0];
			fRec76[1] = fRec76[0];
			fVec7[1] = fVec7[0];
			fRec75[1] = fRec75[0];
			fRec72[1] = fRec72[0];
			fRec74[1] = fRec74[0];
			fRec73[1] = fRec73[0];
			fRec64[1] = fRec64[0];
			fRec68[1] = fRec68[0];
			fRec65[1] = fRec65[0];
			fRec67[1] = fRec67[0];
			fRec66[1] = fRec66[0];
			fRec56[1] = fRec56[0];
			fRec57[1] = fRec57[0];
			fRec63[1] = fRec63[0];
			fRec62[1] = fRec62[0];
			fVec6[1] = fVec6[0];
			fRec61[1] = fRec61[0];
			fRec58[1] = fRec58[0];
			fRec60[1] = fRec60[0];
			fRec59[1] = fRec59[0];
			fRec54[1] = fRec54[0];
			fRec55[1] = fRec55[0];
			fRec51[1] = fRec51[0];
			fRec52[1] = fRec52[0];
			fRec53[1] = fRec53[0];
			fRec48[1] = fRec48[0];
			fRec50[1] = fRec50[0];
			fRec49[1] = fRec49[0];
			fRec47[1] = fRec47[0];
			fVec5[1] = fVec5[0];
			fRec46[1] = fRec46[0];
			fRec43[1] = fRec43[0];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec40[1] = fRec40[0];
			fRec42[1] = fRec42[0];
			fRec41[1] = fRec41[0];
			fRec39[1] = fRec39[0];
			fVec4[1] = fVec4[0];
			fRec38[1] = fRec38[0];
			fRec35[1] = fRec35[0];
			fRec37[1] = fRec37[0];
			fRec36[1] = fRec36[0];
			fRec29[1] = fRec29[0];
			fRec32[1] = fRec32[0];
			fRec34[1] = fRec34[0];
			fRec33[1] = fRec33[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec28[1] = fRec28[0];
			fVec3[1] = fVec3[0];
			fRec27[1] = fRec27[0];
			fRec18[1] = fRec18[0];
			fRec26[1] = fRec26[0];
			fVec2[1] = fVec2[0];
			fRec25[1] = fRec25[0];
			fRec21[1] = fRec21[0];
			fRec24[1] = fRec24[0];
			fRec23[1] = fRec23[0];
			fRec22[1] = fRec22[0];
			fRec20[1] = fRec20[0];
			fRec19[1] = fRec19[0];
			fRec12[1] = fRec12[0];
			fRec15[1] = fRec15[0];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			fRec14[1] = fRec14[0];
			fRec13[1] = fRec13[0];
			fRec11[1] = fRec11[0];
			fVec1[1] = fVec1[0];
			fRec10[1] = fRec10[0];
			fRec1[1] = fRec1[0];
			fRec9[1] = fRec9[0];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
			fVec0[1] = fVec0[0];
			fRec6[1] = fRec6[0];
			fRec2[1] = fRec2[0];
			fRec5[1] = fRec5[0];
			fRec4[1] = fRec4[0];
			fRec3[1] = fRec3[0];
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
