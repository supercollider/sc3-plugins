//----------------------------------------------------------
// name: "HOAPanLebedev504"
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
	double 	fRec71[2];
	double 	fRec70[2];
	double 	fRec69[2];
	double 	fRec73[2];
	double 	fRec72[2];
	double 	fRec77[2];
	double 	fRec78[2];
	double 	fRec76[2];
	double 	fRec79[2];
	double 	fVec7[2];
	double 	fRec80[2];
	double 	fRec81[2];
	double 	fRec75[2];
	double 	fRec74[2];
	double 	fRec84[2];
	double 	fRec85[2];
	double 	fRec83[2];
	double 	fRec86[2];
	double 	fRec82[2];
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fRec87[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec88[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec89[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fRec90[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec91[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec92[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec93[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec94[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec95[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec96[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec97[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec98[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec99[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec100[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec101[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec102[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec103[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec104[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec105[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec106[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec107[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec108[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec109[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec110[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec111[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec112[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec113[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec114[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec115[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec116[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec117[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec118[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec119[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec120[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec121[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec122[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec123[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec124[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec125[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec126[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec127[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec128[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec129[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec130[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec131[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec132[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec133[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec134[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec135[2];
	FAUSTFLOAT 	fbargraph49;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOAPanLebedev504");
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
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
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
		for (int i=0; i<2; i++) fRec124[i] = 0;
		for (int i=0; i<2; i++) fRec125[i] = 0;
		for (int i=0; i<2; i++) fRec126[i] = 0;
		for (int i=0; i<2; i++) fRec127[i] = 0;
		for (int i=0; i<2; i++) fRec128[i] = 0;
		for (int i=0; i<2; i++) fRec129[i] = 0;
		for (int i=0; i<2; i++) fRec130[i] = 0;
		for (int i=0; i<2; i++) fRec131[i] = 0;
		for (int i=0; i<2; i++) fRec132[i] = 0;
		for (int i=0; i<2; i++) fRec133[i] = 0;
		for (int i=0; i<2; i++) fRec134[i] = 0;
		for (int i=0; i<2; i++) fRec135[i] = 0;
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
		ui_interface->addVerticalBargraph("0x7f905ab00060", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph1, "osc", "/output2");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ab040b0", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph2, "osc", "/output3");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9059b009a0", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph3, "osc", "/output4");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9059b050f0", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph4, "osc", "/output5");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9059b097e0", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph5, "osc", "/output6");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9059b0d8b0", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph6, "osc", "/output7");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9059b120f0", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph7, "osc", "/output8");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9059b16390", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph8, "osc", "/output9");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9059b1a9b0", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph9, "osc", "/output10");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9059b1eec0", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph10, "osc", "/output11");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f9059b23790", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph11, "osc", "/output12");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a79c5b0", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph12, "osc", "/output13");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7a0dd0", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph13, "osc", "/output14");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7a5820", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph14, "osc", "/output15");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7a9dc0", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph15, "osc", "/output16");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7ae080", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph16, "osc", "/output17");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7b2570", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph17, "osc", "/output18");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7b6ac0", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph18, "osc", "/output19");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7bb990", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph19, "osc", "/output20");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7c0250", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph20, "osc", "/output21");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7c4b50", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph21, "osc", "/output22");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7c9580", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph22, "osc", "/output23");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7cddd0", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph23, "osc", "/output24");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7d26c0", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph24, "osc", "/output25");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7d6fe0", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph25, "osc", "/output26");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7db930", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph26, "osc", "/output27");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7e06b0", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph27, "osc", "/output28");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7e47e0", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph28, "osc", "/output29");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7e8c40", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph29, "osc", "/output30");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7ecf60", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph30, "osc", "/output31");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7f1de0", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph31, "osc", "/output32");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7f6840", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph32, "osc", "/output33");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905a7fb2f0", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph33, "osc", "/output34");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac03de0", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph34, "osc", "/output35");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac08980", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fbargraph35, "osc", "/output36");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac0d5c0", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fbargraph36, "osc", "/output37");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac12070", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fbargraph37, "osc", "/output38");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac16f10", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fbargraph38, "osc", "/output39");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac1b240", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fbargraph39, "osc", "/output40");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac1f950", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fbargraph40, "osc", "/output41");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac23ef0", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fbargraph41, "osc", "/output42");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac284e0", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fbargraph42, "osc", "/output43");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac2cb60", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fbargraph43, "osc", "/output44");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac31250", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fbargraph44, "osc", "/output45");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac359b0", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fbargraph45, "osc", "/output46");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac39fc0", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fbargraph46, "osc", "/output47");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac3e9f0", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fbargraph47, "osc", "/output48");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac432a0", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fbargraph48, "osc", "/output49");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac47bc0", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fbargraph49, "osc", "/output50");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f905ac4c550", &fbargraph49, -7e+01, 6.0);
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
		double 	fSlow21 = (((fConst4 / fSlow14) + (11.4878004768713 * fSlow16)) + 1);
		double 	fSlow22 = ((fSlow20 * fSlow21) / fSlow14);
		double 	fSlow23 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider3))));
		double 	fSlow24 = (45.9512019074852 * fSlow12);
		double 	fSlow25 = (fSlow24 + (fConst5 / fSlow10));
		double 	fSlow26 = (1.0 / fSlow21);
		double 	fSlow27 = (45.9512019074852 * fSlow16);
		double 	fSlow28 = ((fConst5 / fSlow14) + fSlow27);
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
		double 	fSlow50 = (((fConst4 / fSlow44) + (11.4878004768713 * fSlow46)) + 1);
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
		double 	fSlow61 = ((3 * fSlow8) + -1);
		double 	fSlow62 = (1.0 / (((fConst7 / fSlow10) + (3 * fSlow12)) + 1));
		double 	fSlow63 = (((fConst7 / fSlow14) + (3 * fSlow16)) + 1);
		double 	fSlow64 = ((fSlow20 * fSlow63) / fSlow14);
		double 	fSlow65 = (12 * fSlow12);
		double 	fSlow66 = (fSlow65 + (fConst8 / fSlow10));
		double 	fSlow67 = (1.0 / fSlow63);
		double 	fSlow68 = (12 * fSlow16);
		double 	fSlow69 = ((fConst8 / fSlow14) + fSlow68);
		double 	fSlow70 = ((3 * fSlow42) + -1);
		double 	fSlow71 = (((fConst7 / fSlow44) + (3 * fSlow46)) + 1);
		double 	fSlow72 = ((fSlow49 * fSlow71) / fSlow44);
		double 	fSlow73 = (1.0 / fSlow71);
		double 	fSlow74 = (12 * fSlow46);
		double 	fSlow75 = ((fConst8 / fSlow44) + fSlow74);
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
		double 	fSlow99 = (fSlow45 + 1);
		double 	fSlow100 = (fConst9 / (fSlow44 * fSlow99));
		double 	fSlow101 = ((fSlow49 * fSlow99) / fSlow44);
		double 	fSlow102 = (3.5 * (fSlow85 * ((5 * fSlow42) + -3)));
		double 	fSlow103 = ((fConst11 / fSlow44) + 1);
		double 	fSlow104 = (fConst10 / (fSlow44 * fSlow103));
		double 	fSlow105 = (((fConst12 / fSlow44) + (6.45943269348338 * fSlow46)) + 1);
		double 	fSlow106 = ((fSlow49 * fSlow105) / fSlow44);
		double 	fSlow107 = (1.0 / fSlow105);
		double 	fSlow108 = (25.83773077393352 * fSlow46);
		double 	fSlow109 = ((fConst13 / fSlow44) + fSlow108);
		double 	fSlow110 = faustpower<2>(fSlow39);
		double 	fSlow111 = ((3 * fSlow110) + -1);
		double 	fSlow112 = faustpower<2>(fSlow5);
		double 	fSlow113 = ((3 * fSlow112) + -1);
		double 	fSlow114 = ((fSlow112 * ((35 * fSlow112) + -30)) + 3);
		double 	fSlow115 = ((fSlow110 * ((35 * fSlow110) + -30)) + 3);
		double 	fSlow116 = (3.5 * fSlow85);
		double 	fSlow117 = ((5 * faustpower<3>(fSlow5)) - (3 * fSlow5));
		double 	fSlow118 = ((5 * faustpower<3>(fSlow39)) - (3 * fSlow39));
		double 	fSlow119 = (fSlow37 * cos((fSlow38 + -1.5707963267948966)));
		double 	fSlow120 = faustpower<2>(fSlow119);
		double 	fSlow121 = ((3 * fSlow120) + -1);
		double 	fSlow122 = (fSlow3 * cos((fSlow4 + -1.5707963267948966)));
		double 	fSlow123 = faustpower<2>(fSlow122);
		double 	fSlow124 = ((3 * fSlow123) + -1);
		double 	fSlow125 = ((5 * faustpower<3>(fSlow119)) - (3 * fSlow119));
		double 	fSlow126 = ((5 * faustpower<3>(fSlow122)) - (3 * fSlow122));
		double 	fSlow127 = ((fSlow123 * ((35 * fSlow123) + -30)) + 3);
		double 	fSlow128 = ((fSlow120 * ((35 * fSlow120) + -30)) + 3);
		double 	fSlow129 = (fSlow37 * cos((fSlow38 + -3.141592653589793)));
		double 	fSlow130 = ((5 * faustpower<3>(fSlow129)) - (3 * fSlow129));
		double 	fSlow131 = (fSlow3 * cos((fSlow4 + -3.141592653589793)));
		double 	fSlow132 = ((5 * faustpower<3>(fSlow131)) - (3 * fSlow131));
		double 	fSlow133 = faustpower<2>(fSlow131);
		double 	fSlow134 = ((fSlow133 * ((35 * fSlow133) + -30)) + 3);
		double 	fSlow135 = faustpower<2>(fSlow129);
		double 	fSlow136 = ((fSlow135 * ((35 * fSlow135) + -30)) + 3);
		double 	fSlow137 = ((3 * fSlow133) + -1);
		double 	fSlow138 = ((3 * fSlow135) + -1);
		double 	fSlow139 = (fSlow3 * cos((fSlow4 + -4.71238898038469)));
		double 	fSlow140 = ((5 * faustpower<3>(fSlow139)) - (3 * fSlow139));
		double 	fSlow141 = (fSlow37 * cos((fSlow38 + -4.71238898038469)));
		double 	fSlow142 = ((5 * faustpower<3>(fSlow141)) - (3 * fSlow141));
		double 	fSlow143 = faustpower<2>(fSlow141);
		double 	fSlow144 = ((3 * fSlow143) + -1);
		double 	fSlow145 = faustpower<2>(fSlow139);
		double 	fSlow146 = ((3 * fSlow145) + -1);
		double 	fSlow147 = ((fSlow143 * ((35 * fSlow143) + -30)) + 3);
		double 	fSlow148 = ((fSlow145 * ((35 * fSlow145) + -30)) + 3);
		double 	fSlow149 = (fSlow40 - fSlow36);
		double 	fSlow150 = faustpower<2>(fSlow149);
		double 	fSlow151 = (3.5 * (fSlow85 * ((5 * fSlow150) + -3)));
		double 	fSlow152 = (fSlow6 - fSlow2);
		double 	fSlow153 = faustpower<2>(fSlow152);
		double 	fSlow154 = (3.5 * (fSlow85 * ((5 * fSlow153) + -3)));
		double 	fSlow155 = ((fSlow153 * ((35 * fSlow153) + -30)) + 3);
		double 	fSlow156 = ((fSlow150 * ((35 * fSlow150) + -30)) + 3);
		double 	fSlow157 = ((3 * fSlow153) + -1);
		double 	fSlow158 = ((3 * fSlow150) + -1);
		double 	fSlow159 = (0.7071067811865476 * fSlow39);
		double 	fSlow160 = (0.7071067811865475 * fSlow36);
		double 	fSlow161 = (fSlow159 + fSlow160);
		double 	fSlow162 = faustpower<2>(fSlow161);
		double 	fSlow163 = (3.5 * (fSlow85 * ((5 * fSlow162) + -3)));
		double 	fSlow164 = (0.7071067811865476 * fSlow5);
		double 	fSlow165 = (0.7071067811865475 * fSlow2);
		double 	fSlow166 = (fSlow164 + fSlow165);
		double 	fSlow167 = faustpower<2>(fSlow166);
		double 	fSlow168 = (3.5 * (fSlow85 * ((5 * fSlow167) + -3)));
		double 	fSlow169 = ((fSlow167 * ((35 * fSlow167) + -30)) + 3);
		double 	fSlow170 = ((fSlow162 * ((35 * fSlow162) + -30)) + 3);
		double 	fSlow171 = ((3 * fSlow167) + -1);
		double 	fSlow172 = ((3 * fSlow162) + -1);
		double 	fSlow173 = (0.7071067811865476 * fSlow122);
		double 	fSlow174 = (fSlow165 + fSlow173);
		double 	fSlow175 = faustpower<2>(fSlow174);
		double 	fSlow176 = ((3 * fSlow175) + -1);
		double 	fSlow177 = (0.7071067811865476 * fSlow119);
		double 	fSlow178 = (fSlow160 + fSlow177);
		double 	fSlow179 = faustpower<2>(fSlow178);
		double 	fSlow180 = ((3 * fSlow179) + -1);
		double 	fSlow181 = ((fSlow175 * ((35 * fSlow175) + -30)) + 3);
		double 	fSlow182 = ((fSlow179 * ((35 * fSlow179) + -30)) + 3);
		double 	fSlow183 = (3.5 * (fSlow85 * ((5 * fSlow179) + -3)));
		double 	fSlow184 = (3.5 * (fSlow85 * ((5 * fSlow175) + -3)));
		double 	fSlow185 = (0.7071067811865476 * fSlow131);
		double 	fSlow186 = (fSlow165 + fSlow185);
		double 	fSlow187 = faustpower<2>(fSlow186);
		double 	fSlow188 = ((3 * fSlow187) + -1);
		double 	fSlow189 = (0.7071067811865476 * fSlow129);
		double 	fSlow190 = (fSlow160 + fSlow189);
		double 	fSlow191 = faustpower<2>(fSlow190);
		double 	fSlow192 = ((3 * fSlow191) + -1);
		double 	fSlow193 = (3.5 * (fSlow85 * ((5 * fSlow187) + -3)));
		double 	fSlow194 = (3.5 * (((5 * fSlow191) + -3) * fSlow85));
		double 	fSlow195 = ((fSlow187 * ((35 * fSlow187) + -30)) + 3);
		double 	fSlow196 = ((fSlow191 * ((35 * fSlow191) + -30)) + 3);
		double 	fSlow197 = (0.7071067811865476 * fSlow139);
		double 	fSlow198 = (fSlow197 + fSlow165);
		double 	fSlow199 = faustpower<2>(fSlow198);
		double 	fSlow200 = ((3 * fSlow199) + -1);
		double 	fSlow201 = (0.7071067811865476 * fSlow141);
		double 	fSlow202 = (fSlow160 + fSlow201);
		double 	fSlow203 = faustpower<2>(fSlow202);
		double 	fSlow204 = ((3 * fSlow203) + -1);
		double 	fSlow205 = (3.5 * (fSlow85 * ((5 * fSlow203) + -3)));
		double 	fSlow206 = (3.5 * (fSlow85 * ((5 * fSlow199) + -3)));
		double 	fSlow207 = ((fSlow203 * ((35 * fSlow203) + -30)) + 3);
		double 	fSlow208 = ((fSlow199 * ((35 * fSlow199) + -30)) + 3);
		double 	fSlow209 = (fSlow3 * cos((fSlow4 + -0.7853981633974483)));
		double 	fSlow210 = (fSlow37 * cos((fSlow38 + -0.7853981633974483)));
		double 	fSlow211 = faustpower<2>(fSlow209);
		double 	fSlow212 = ((fSlow211 * ((35 * fSlow211) + -30)) + 3);
		double 	fSlow213 = faustpower<2>(fSlow210);
		double 	fSlow214 = ((fSlow213 * ((35 * fSlow213) + -30)) + 3);
		double 	fSlow215 = ((5 * faustpower<3>(fSlow210)) - (3 * fSlow210));
		double 	fSlow216 = ((5 * faustpower<3>(fSlow209)) - (3 * fSlow209));
		double 	fSlow217 = ((3 * fSlow211) + -1);
		double 	fSlow218 = ((3 * fSlow213) + -1);
		double 	fSlow219 = (fSlow3 * cos((fSlow4 + -2.356194490192345)));
		double 	fSlow220 = (fSlow37 * cos((fSlow38 + -2.356194490192345)));
		double 	fSlow221 = faustpower<2>(fSlow219);
		double 	fSlow222 = ((fSlow221 * ((35 * fSlow221) + -30)) + 3);
		double 	fSlow223 = faustpower<2>(fSlow220);
		double 	fSlow224 = ((fSlow223 * ((35 * fSlow223) + -30)) + 3);
		double 	fSlow225 = ((5 * faustpower<3>(fSlow220)) - (3 * fSlow220));
		double 	fSlow226 = ((5 * faustpower<3>(fSlow219)) - (3 * fSlow219));
		double 	fSlow227 = ((3 * fSlow221) + -1);
		double 	fSlow228 = ((3 * fSlow223) + -1);
		double 	fSlow229 = (fSlow37 * cos((fSlow38 + -3.9269908169872414)));
		double 	fSlow230 = faustpower<2>(fSlow229);
		double 	fSlow231 = ((3 * fSlow230) + -1);
		double 	fSlow232 = (fSlow3 * cos((fSlow4 + -3.9269908169872414)));
		double 	fSlow233 = faustpower<2>(fSlow232);
		double 	fSlow234 = ((3 * fSlow233) + -1);
		double 	fSlow235 = ((5 * faustpower<3>(fSlow229)) - (3 * fSlow229));
		double 	fSlow236 = ((5 * faustpower<3>(fSlow232)) - (3 * fSlow232));
		double 	fSlow237 = ((fSlow233 * ((35 * fSlow233) + -30)) + 3);
		double 	fSlow238 = ((fSlow230 * ((35 * fSlow230) + -30)) + 3);
		double 	fSlow239 = (fSlow3 * cos((fSlow4 + -5.497787143782138)));
		double 	fSlow240 = faustpower<2>(fSlow239);
		double 	fSlow241 = ((fSlow240 * ((35 * fSlow240) + -30)) + 3);
		double 	fSlow242 = (fSlow37 * cos((fSlow38 + -5.497787143782138)));
		double 	fSlow243 = faustpower<2>(fSlow242);
		double 	fSlow244 = ((fSlow243 * ((35 * fSlow243) + -30)) + 3);
		double 	fSlow245 = ((5 * faustpower<3>(fSlow239)) - (3 * fSlow239));
		double 	fSlow246 = ((5 * faustpower<3>(fSlow242)) - (3 * fSlow242));
		double 	fSlow247 = ((3 * fSlow240) + -1);
		double 	fSlow248 = ((3 * fSlow243) + -1);
		double 	fSlow249 = (fSlow164 - fSlow165);
		double 	fSlow250 = faustpower<2>(fSlow249);
		double 	fSlow251 = ((3 * fSlow250) + -1);
		double 	fSlow252 = (fSlow159 - fSlow160);
		double 	fSlow253 = faustpower<2>(fSlow252);
		double 	fSlow254 = ((3 * fSlow253) + -1);
		double 	fSlow255 = ((fSlow250 * ((35 * fSlow250) + -30)) + 3);
		double 	fSlow256 = ((fSlow253 * ((35 * fSlow253) + -30)) + 3);
		double 	fSlow257 = (3.5 * (fSlow85 * ((5 * fSlow253) + -3)));
		double 	fSlow258 = (3.5 * (fSlow85 * ((5 * fSlow250) + -3)));
		double 	fSlow259 = (fSlow177 - fSlow160);
		double 	fSlow260 = faustpower<2>(fSlow259);
		double 	fSlow261 = ((fSlow260 * ((35 * fSlow260) + -30)) + 3);
		double 	fSlow262 = (fSlow173 - fSlow165);
		double 	fSlow263 = faustpower<2>(fSlow262);
		double 	fSlow264 = ((fSlow263 * ((35 * fSlow263) + -30)) + 3);
		double 	fSlow265 = (3.5 * (fSlow85 * ((5 * fSlow260) + -3)));
		double 	fSlow266 = (3.5 * (fSlow85 * ((5 * fSlow263) + -3)));
		double 	fSlow267 = ((3 * fSlow260) + -1);
		double 	fSlow268 = ((3 * fSlow263) + -1);
		double 	fSlow269 = (fSlow189 - fSlow160);
		double 	fSlow270 = faustpower<2>(fSlow269);
		double 	fSlow271 = ((fSlow270 * ((35 * fSlow270) + -30)) + 3);
		double 	fSlow272 = (fSlow185 - fSlow165);
		double 	fSlow273 = faustpower<2>(fSlow272);
		double 	fSlow274 = ((fSlow273 * ((35 * fSlow273) + -30)) + 3);
		double 	fSlow275 = (3.5 * (fSlow85 * ((5 * fSlow270) + -3)));
		double 	fSlow276 = (3.5 * (fSlow85 * ((5 * fSlow273) + -3)));
		double 	fSlow277 = ((3 * fSlow270) + -1);
		double 	fSlow278 = ((3 * fSlow273) + -1);
		double 	fSlow279 = (fSlow197 - fSlow165);
		double 	fSlow280 = faustpower<2>(fSlow279);
		double 	fSlow281 = ((fSlow280 * ((35 * fSlow280) + -30)) + 3);
		double 	fSlow282 = (fSlow201 - fSlow160);
		double 	fSlow283 = faustpower<2>(fSlow282);
		double 	fSlow284 = ((fSlow283 * ((35 * fSlow283) + -30)) + 3);
		double 	fSlow285 = (3.5 * (fSlow85 * ((5 * fSlow283) + -3)));
		double 	fSlow286 = (3.5 * (fSlow85 * ((5 * fSlow280) + -3)));
		double 	fSlow287 = ((3 * fSlow283) + -1);
		double 	fSlow288 = ((3 * fSlow280) + -1);
		double 	fSlow289 = (0.8164965809277259 * fSlow209);
		double 	fSlow290 = (0.5773502691896258 * fSlow2);
		double 	fSlow291 = (fSlow289 + fSlow290);
		double 	fSlow292 = faustpower<2>(fSlow291);
		double 	fSlow293 = ((fSlow292 * ((35 * fSlow292) + -30)) + 3);
		double 	fSlow294 = (0.8164965809277259 * fSlow210);
		double 	fSlow295 = (0.5773502691896258 * fSlow36);
		double 	fSlow296 = (fSlow294 + fSlow295);
		double 	fSlow297 = faustpower<2>(fSlow296);
		double 	fSlow298 = ((fSlow297 * ((35 * fSlow297) + -30)) + 3);
		double 	fSlow299 = ((3 * fSlow297) + -1);
		double 	fSlow300 = ((3 * fSlow292) + -1);
		double 	fSlow301 = (3.5 * (fSlow85 * ((5 * fSlow297) + -3)));
		double 	fSlow302 = (3.5 * (fSlow85 * ((5 * fSlow292) + -3)));
		double 	fSlow303 = (0.8164965809277259 * fSlow220);
		double 	fSlow304 = (fSlow295 + fSlow303);
		double 	fSlow305 = faustpower<2>(fSlow304);
		double 	fSlow306 = ((3 * fSlow305) + -1);
		double 	fSlow307 = (0.8164965809277259 * fSlow219);
		double 	fSlow308 = (fSlow290 + fSlow307);
		double 	fSlow309 = faustpower<2>(fSlow308);
		double 	fSlow310 = ((3 * fSlow309) + -1);
		double 	fSlow311 = ((fSlow305 * ((35 * fSlow305) + -30)) + 3);
		double 	fSlow312 = ((fSlow309 * ((35 * fSlow309) + -30)) + 3);
		double 	fSlow313 = (3.5 * (fSlow85 * ((5 * fSlow305) + -3)));
		double 	fSlow314 = (3.5 * (fSlow85 * ((5 * fSlow309) + -3)));
		double 	fSlow315 = (0.8164965809277259 * fSlow229);
		double 	fSlow316 = (fSlow295 + fSlow315);
		double 	fSlow317 = faustpower<2>(fSlow316);
		double 	fSlow318 = ((3 * fSlow317) + -1);
		double 	fSlow319 = (0.8164965809277259 * fSlow232);
		double 	fSlow320 = (fSlow290 + fSlow319);
		double 	fSlow321 = faustpower<2>(fSlow320);
		double 	fSlow322 = ((3 * fSlow321) + -1);
		double 	fSlow323 = (3.5 * (fSlow85 * ((5 * fSlow317) + -3)));
		double 	fSlow324 = (3.5 * (fSlow85 * ((5 * fSlow321) + -3)));
		double 	fSlow325 = ((fSlow317 * ((35 * fSlow317) + -30)) + 3);
		double 	fSlow326 = ((fSlow321 * ((35 * fSlow321) + -30)) + 3);
		double 	fSlow327 = (0.8164965809277259 * fSlow242);
		double 	fSlow328 = (fSlow295 + fSlow327);
		double 	fSlow329 = faustpower<2>(fSlow328);
		double 	fSlow330 = (3.5 * (fSlow85 * ((5 * fSlow329) + -3)));
		double 	fSlow331 = (0.8164965809277259 * fSlow239);
		double 	fSlow332 = (fSlow290 + fSlow331);
		double 	fSlow333 = faustpower<2>(fSlow332);
		double 	fSlow334 = (3.5 * (fSlow85 * ((5 * fSlow333) + -3)));
		double 	fSlow335 = ((3 * fSlow333) + -1);
		double 	fSlow336 = ((3 * fSlow329) + -1);
		double 	fSlow337 = ((fSlow329 * ((35 * fSlow329) + -30)) + 3);
		double 	fSlow338 = ((fSlow333 * ((35 * fSlow333) + -30)) + 3);
		double 	fSlow339 = (fSlow294 - fSlow295);
		double 	fSlow340 = faustpower<2>(fSlow339);
		double 	fSlow341 = (3.5 * (fSlow85 * ((5 * fSlow340) + -3)));
		double 	fSlow342 = (fSlow289 - fSlow290);
		double 	fSlow343 = faustpower<2>(fSlow342);
		double 	fSlow344 = (3.5 * (fSlow85 * ((5 * fSlow343) + -3)));
		double 	fSlow345 = ((3 * fSlow343) + -1);
		double 	fSlow346 = ((3 * fSlow340) + -1);
		double 	fSlow347 = ((fSlow343 * ((35 * fSlow343) + -30)) + 3);
		double 	fSlow348 = ((fSlow340 * ((35 * fSlow340) + -30)) + 3);
		double 	fSlow349 = (fSlow307 - fSlow290);
		double 	fSlow350 = faustpower<2>(fSlow349);
		double 	fSlow351 = ((3 * fSlow350) + -1);
		double 	fSlow352 = (fSlow303 - fSlow295);
		double 	fSlow353 = faustpower<2>(fSlow352);
		double 	fSlow354 = ((3 * fSlow353) + -1);
		double 	fSlow355 = ((fSlow350 * ((35 * fSlow350) + -30)) + 3);
		double 	fSlow356 = ((fSlow353 * ((35 * fSlow353) + -30)) + 3);
		double 	fSlow357 = (3.5 * (fSlow85 * ((5 * fSlow353) + -3)));
		double 	fSlow358 = (3.5 * (fSlow85 * ((5 * fSlow350) + -3)));
		double 	fSlow359 = (fSlow315 - fSlow295);
		double 	fSlow360 = faustpower<2>(fSlow359);
		double 	fSlow361 = ((fSlow360 * ((35 * fSlow360) + -30)) + 3);
		double 	fSlow362 = (fSlow319 - fSlow290);
		double 	fSlow363 = faustpower<2>(fSlow362);
		double 	fSlow364 = ((fSlow363 * ((35 * fSlow363) + -30)) + 3);
		double 	fSlow365 = ((3 * fSlow360) + -1);
		double 	fSlow366 = ((3 * fSlow363) + -1);
		double 	fSlow367 = (3.5 * (fSlow85 * ((5 * fSlow360) + -3)));
		double 	fSlow368 = (3.5 * (fSlow85 * ((5 * fSlow363) + -3)));
		double 	fSlow369 = (fSlow327 - fSlow295);
		double 	fSlow370 = faustpower<2>(fSlow369);
		double 	fSlow371 = (3.5 * (fSlow85 * ((5 * fSlow370) + -3)));
		double 	fSlow372 = (fSlow331 - fSlow290);
		double 	fSlow373 = faustpower<2>(fSlow372);
		double 	fSlow374 = (3.5 * (fSlow85 * ((5 * fSlow373) + -3)));
		double 	fSlow375 = ((3 * fSlow373) + -1);
		double 	fSlow376 = ((3 * fSlow370) + -1);
		double 	fSlow377 = ((fSlow373 * ((35 * fSlow373) + -30)) + 3);
		double 	fSlow378 = ((fSlow370 * ((35 * fSlow370) + -30)) + 3);
		double 	fSlow379 = (0.4264014327112203 * (fSlow37 * cos((fSlow38 + -0.7853981633974484))));
		double 	fSlow380 = (0.9045340337332911 * fSlow36);
		double 	fSlow381 = (fSlow379 + fSlow380);
		double 	fSlow382 = faustpower<2>(fSlow381);
		double 	fSlow383 = (3.5 * (fSlow85 * ((5 * fSlow382) + -3)));
		double 	fSlow384 = (0.4264014327112203 * (fSlow3 * cos((fSlow4 + -0.7853981633974484))));
		double 	fSlow385 = (0.9045340337332911 * fSlow2);
		double 	fSlow386 = (fSlow384 + fSlow385);
		double 	fSlow387 = faustpower<2>(fSlow386);
		double 	fSlow388 = (3.5 * (fSlow85 * ((5 * fSlow387) + -3)));
		double 	fSlow389 = ((fSlow387 * ((35 * fSlow387) + -30)) + 3);
		double 	fSlow390 = ((fSlow382 * ((35 * fSlow382) + -30)) + 3);
		double 	fSlow391 = ((3 * fSlow382) + -1);
		double 	fSlow392 = ((3 * fSlow387) + -1);
		double 	fSlow393 = (0.4264014327112203 * fSlow219);
		double 	fSlow394 = (fSlow385 + fSlow393);
		double 	fSlow395 = faustpower<2>(fSlow394);
		double 	fSlow396 = ((fSlow395 * ((35 * fSlow395) + -30)) + 3);
		double 	fSlow397 = (0.4264014327112203 * fSlow220);
		double 	fSlow398 = (fSlow380 + fSlow397);
		double 	fSlow399 = faustpower<2>(fSlow398);
		double 	fSlow400 = ((fSlow399 * ((35 * fSlow399) + -30)) + 3);
		double 	fSlow401 = (3.5 * (fSlow85 * ((5 * fSlow399) + -3)));
		double 	fSlow402 = (3.5 * (fSlow85 * ((5 * fSlow395) + -3)));
		double 	fSlow403 = ((3 * fSlow399) + -1);
		double 	fSlow404 = ((3 * fSlow395) + -1);
		double 	fSlow405 = (0.4264014327112203 * fSlow229);
		double 	fSlow406 = (fSlow380 + fSlow405);
		double 	fSlow407 = faustpower<2>(fSlow406);
		double 	fSlow408 = (3.5 * (fSlow85 * ((5 * fSlow407) + -3)));
		double 	fSlow409 = (0.4264014327112203 * fSlow232);
		double 	fSlow410 = (fSlow385 + fSlow409);
		double 	fSlow411 = faustpower<2>(fSlow410);
		double 	fSlow412 = (3.5 * (fSlow85 * ((5 * fSlow411) + -3)));
		double 	fSlow413 = ((3 * fSlow411) + -1);
		double 	fSlow414 = ((3 * fSlow407) + -1);
		double 	fSlow415 = ((fSlow411 * ((35 * fSlow411) + -30)) + 3);
		double 	fSlow416 = ((fSlow407 * ((35 * fSlow407) + -30)) + 3);
		double 	fSlow417 = (0.4264014327112203 * fSlow242);
		double 	fSlow418 = (fSlow380 + fSlow417);
		double 	fSlow419 = faustpower<2>(fSlow418);
		double 	fSlow420 = (3.5 * (fSlow85 * ((5 * fSlow419) + -3)));
		double 	fSlow421 = (0.4264014327112203 * fSlow239);
		double 	fSlow422 = (fSlow385 + fSlow421);
		double 	fSlow423 = faustpower<2>(fSlow422);
		double 	fSlow424 = (3.5 * (fSlow85 * ((5 * fSlow423) + -3)));
		double 	fSlow425 = ((3 * fSlow423) + -1);
		double 	fSlow426 = ((3 * fSlow419) + -1);
		double 	fSlow427 = ((fSlow419 * ((35 * fSlow419) + -30)) + 3);
		double 	fSlow428 = ((fSlow423 * ((35 * fSlow423) + -30)) + 3);
		double 	fSlow429 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -0.32175055439664263))));
		double 	fSlow430 = (0.3015113445777635 * fSlow36);
		double 	fSlow431 = (fSlow429 + fSlow430);
		double 	fSlow432 = faustpower<2>(fSlow431);
		double 	fSlow433 = ((fSlow432 * ((35 * fSlow432) + -30)) + 3);
		double 	fSlow434 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -0.32175055439664263))));
		double 	fSlow435 = (0.3015113445777635 * fSlow2);
		double 	fSlow436 = (fSlow434 + fSlow435);
		double 	fSlow437 = faustpower<2>(fSlow436);
		double 	fSlow438 = ((fSlow437 * ((35 * fSlow437) + -30)) + 3);
		double 	fSlow439 = ((3 * fSlow432) + -1);
		double 	fSlow440 = ((3 * fSlow437) + -1);
		double 	fSlow441 = (3.5 * (fSlow85 * ((5 * fSlow432) + -3)));
		double 	fSlow442 = (3.5 * (fSlow85 * ((5 * fSlow437) + -3)));
		double 	fSlow443 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -1.2490457723982544))));
		double 	fSlow444 = (fSlow430 + fSlow443);
		double 	fSlow445 = faustpower<2>(fSlow444);
		double 	fSlow446 = (3.5 * (fSlow85 * ((5 * fSlow445) + -3)));
		double 	fSlow447 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -1.2490457723982544))));
		double 	fSlow448 = (fSlow435 + fSlow447);
		double 	fSlow449 = faustpower<2>(fSlow448);
		double 	fSlow450 = (3.5 * (fSlow85 * ((5 * fSlow449) + -3)));
		double 	fSlow451 = ((fSlow449 * ((35 * fSlow449) + -30)) + 3);
		double 	fSlow452 = ((fSlow445 * ((35 * fSlow445) + -30)) + 3);
		double 	fSlow453 = ((3 * fSlow445) + -1);
		double 	fSlow454 = ((3 * fSlow449) + -1);
		double 	fSlow455 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -1.8925468811915387))));
		double 	fSlow456 = (fSlow430 + fSlow455);
		double 	fSlow457 = faustpower<2>(fSlow456);
		double 	fSlow458 = (3.5 * (fSlow85 * ((5 * fSlow457) + -3)));
		double 	fSlow459 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -1.8925468811915387))));
		double 	fSlow460 = (fSlow435 + fSlow459);
		double 	fSlow461 = faustpower<2>(fSlow460);
		double 	fSlow462 = (3.5 * (fSlow85 * ((5 * fSlow461) + -3)));
		double 	fSlow463 = ((3 * fSlow461) + -1);
		double 	fSlow464 = ((3 * fSlow457) + -1);
		double 	fSlow465 = ((fSlow457 * ((35 * fSlow457) + -30)) + 3);
		double 	fSlow466 = ((fSlow461 * ((35 * fSlow461) + -30)) + 3);
		double 	fSlow467 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -2.8198420991931505))));
		double 	fSlow468 = (fSlow435 + fSlow467);
		double 	fSlow469 = faustpower<2>(fSlow468);
		double 	fSlow470 = ((3 * fSlow469) + -1);
		double 	fSlow471 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -2.8198420991931505))));
		double 	fSlow472 = (fSlow430 + fSlow471);
		double 	fSlow473 = faustpower<2>(fSlow472);
		double 	fSlow474 = ((3 * fSlow473) + -1);
		double 	fSlow475 = ((fSlow469 * ((35 * fSlow469) + -30)) + 3);
		double 	fSlow476 = ((fSlow473 * ((35 * fSlow473) + -30)) + 3);
		double 	fSlow477 = (3.5 * (fSlow85 * ((5 * fSlow473) + -3)));
		double 	fSlow478 = (3.5 * (fSlow85 * ((5 * fSlow469) + -3)));
		double 	fSlow479 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -3.4633432079864357))));
		double 	fSlow480 = (fSlow479 + fSlow430);
		double 	fSlow481 = faustpower<2>(fSlow480);
		double 	fSlow482 = (3.5 * (fSlow85 * ((5 * fSlow481) + -3)));
		double 	fSlow483 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -3.4633432079864357))));
		double 	fSlow484 = (fSlow435 + fSlow483);
		double 	fSlow485 = faustpower<2>(fSlow484);
		double 	fSlow486 = (3.5 * (fSlow85 * ((5 * fSlow485) + -3)));
		double 	fSlow487 = ((3 * fSlow481) + -1);
		double 	fSlow488 = ((3 * fSlow485) + -1);
		double 	fSlow489 = ((((35 * fSlow485) + -30) * fSlow485) + 3);
		double 	fSlow490 = ((fSlow481 * ((35 * fSlow481) + -30)) + 3);
		double 	fSlow491 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -4.3906384259880475))));
		double 	fSlow492 = (fSlow491 + fSlow430);
		double 	fSlow493 = faustpower<2>(fSlow492);
		double 	fSlow494 = ((3 * fSlow493) + -1);
		double 	fSlow495 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -4.3906384259880475))));
		double 	fSlow496 = (fSlow495 + fSlow435);
		double 	fSlow497 = faustpower<2>(fSlow496);
		double 	fSlow498 = ((3 * fSlow497) + -1);
		double 	fSlow499 = (3.5 * (fSlow85 * ((5 * fSlow493) + -3)));
		double 	fSlow500 = (3.5 * (fSlow85 * ((5 * fSlow497) + -3)));
		double 	fSlow501 = ((fSlow493 * ((35 * fSlow493) + -30)) + 3);
		double 	fSlow502 = ((fSlow497 * ((35 * fSlow497) + -30)) + 3);
		double 	fSlow503 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -5.034139534781332))));
		double 	fSlow504 = (fSlow503 + fSlow435);
		double 	fSlow505 = faustpower<2>(fSlow504);
		double 	fSlow506 = ((fSlow505 * ((35 * fSlow505) + -30)) + 3);
		double 	fSlow507 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -5.034139534781332))));
		double 	fSlow508 = (fSlow507 + fSlow430);
		double 	fSlow509 = faustpower<2>(fSlow508);
		double 	fSlow510 = ((fSlow509 * ((35 * fSlow509) + -30)) + 3);
		double 	fSlow511 = (3.5 * (fSlow85 * ((5 * fSlow509) + -3)));
		double 	fSlow512 = (3.5 * (fSlow85 * ((5 * fSlow505) + -3)));
		double 	fSlow513 = ((3 * fSlow509) + -1);
		double 	fSlow514 = ((3 * fSlow505) + -1);
		double 	fSlow515 = (0.9534625892455924 * (fSlow37 * cos((fSlow38 + -5.961434752782943))));
		double 	fSlow516 = (fSlow515 + fSlow430);
		double 	fSlow517 = faustpower<2>(fSlow516);
		double 	fSlow518 = (3.5 * (fSlow85 * ((5 * fSlow517) + -3)));
		double 	fSlow519 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -5.961434752782943))));
		double 	fSlow520 = (fSlow519 + fSlow435);
		double 	fSlow521 = faustpower<2>(fSlow520);
		double 	fSlow522 = (3.5 * (fSlow85 * ((5 * fSlow521) + -3)));
		double 	fSlow523 = ((3 * fSlow521) + -1);
		double 	fSlow524 = ((3 * fSlow517) + -1);
		double 	fSlow525 = ((fSlow517 * ((35 * fSlow517) + -30)) + 3);
		double 	fSlow526 = ((fSlow521 * ((35 * fSlow521) + -30)) + 3);
		double 	fSlow527 = (fSlow429 - fSlow430);
		double 	fSlow528 = faustpower<2>(fSlow527);
		double 	fSlow529 = ((fSlow528 * ((35 * fSlow528) + -30)) + 3);
		double 	fSlow530 = (fSlow434 - fSlow435);
		double 	fSlow531 = faustpower<2>(fSlow530);
		double 	fSlow532 = ((fSlow531 * ((35 * fSlow531) + -30)) + 3);
		double 	fSlow533 = (3.5 * (((5 * fSlow528) + -3) * fSlow85));
		double 	fSlow534 = (3.5 * (fSlow85 * ((5 * fSlow531) + -3)));
		double 	fSlow535 = ((3 * fSlow528) + -1);
		double 	fSlow536 = ((3 * fSlow531) + -1);
		double 	fSlow537 = (fSlow447 - fSlow435);
		double 	fSlow538 = faustpower<2>(fSlow537);
		double 	fSlow539 = ((3 * fSlow538) + -1);
		double 	fSlow540 = (fSlow443 - fSlow430);
		double 	fSlow541 = faustpower<2>(fSlow540);
		double 	fSlow542 = ((3 * fSlow541) + -1);
		double 	fSlow543 = (3.5 * (fSlow85 * ((5 * fSlow541) + -3)));
		double 	fSlow544 = (3.5 * (fSlow85 * ((5 * fSlow538) + -3)));
		double 	fSlow545 = ((fSlow538 * ((35 * fSlow538) + -30)) + 3);
		double 	fSlow546 = ((fSlow541 * ((35 * fSlow541) + -30)) + 3);
		double 	fSlow547 = (fSlow459 - fSlow435);
		double 	fSlow548 = faustpower<2>(fSlow547);
		double 	fSlow549 = ((3 * fSlow548) + -1);
		double 	fSlow550 = (fSlow455 - fSlow430);
		double 	fSlow551 = faustpower<2>(fSlow550);
		double 	fSlow552 = ((3 * fSlow551) + -1);
		double 	fSlow553 = (3.5 * (fSlow85 * ((5 * fSlow551) + -3)));
		double 	fSlow554 = (3.5 * (fSlow85 * ((5 * fSlow548) + -3)));
		double 	fSlow555 = ((fSlow548 * ((35 * fSlow548) + -30)) + 3);
		double 	fSlow556 = ((fSlow551 * ((35 * fSlow551) + -30)) + 3);
		double 	fSlow557 = (fSlow471 - fSlow430);
		double 	fSlow558 = faustpower<2>(fSlow557);
		double 	fSlow559 = (3.5 * (fSlow85 * ((5 * fSlow558) + -3)));
		double 	fSlow560 = (fSlow467 - fSlow435);
		double 	fSlow561 = faustpower<2>(fSlow560);
		double 	fSlow562 = (3.5 * (fSlow85 * ((5 * fSlow561) + -3)));
		double 	fSlow563 = ((3 * fSlow558) + -1);
		double 	fSlow564 = ((3 * fSlow561) + -1);
		double 	fSlow565 = ((fSlow561 * ((35 * fSlow561) + -30)) + 3);
		double 	fSlow566 = ((fSlow558 * ((35 * fSlow558) + -30)) + 3);
		double 	fSlow567 = (fSlow479 - fSlow430);
		double 	fSlow568 = faustpower<2>(fSlow567);
		double 	fSlow569 = (3.5 * (fSlow85 * ((5 * fSlow568) + -3)));
		double 	fSlow570 = (fSlow483 - fSlow435);
		double 	fSlow571 = faustpower<2>(fSlow570);
		double 	fSlow572 = (3.5 * (fSlow85 * ((5 * fSlow571) + -3)));
		double 	fSlow573 = ((3 * fSlow571) + -1);
		double 	fSlow574 = ((3 * fSlow568) + -1);
		double 	fSlow575 = ((fSlow568 * ((35 * fSlow568) + -30)) + 3);
		double 	fSlow576 = ((fSlow571 * ((35 * fSlow571) + -30)) + 3);
		double 	fSlow577 = (fSlow491 - fSlow430);
		double 	fSlow578 = faustpower<2>(fSlow577);
		double 	fSlow579 = (3.5 * (fSlow85 * ((5 * fSlow578) + -3)));
		double 	fSlow580 = (fSlow495 - fSlow435);
		double 	fSlow581 = faustpower<2>(fSlow580);
		double 	fSlow582 = (3.5 * (fSlow85 * ((5 * fSlow581) + -3)));
		double 	fSlow583 = ((3 * fSlow578) + -1);
		double 	fSlow584 = ((3 * fSlow581) + -1);
		double 	fSlow585 = ((fSlow581 * ((35 * fSlow581) + -30)) + 3);
		double 	fSlow586 = ((fSlow578 * ((35 * fSlow578) + -30)) + 3);
		double 	fSlow587 = (fSlow507 - fSlow430);
		double 	fSlow588 = faustpower<2>(fSlow587);
		double 	fSlow589 = (3.5 * (fSlow85 * ((5 * fSlow588) + -3)));
		double 	fSlow590 = (fSlow503 - fSlow435);
		double 	fSlow591 = faustpower<2>(fSlow590);
		double 	fSlow592 = (3.5 * (fSlow85 * ((5 * fSlow591) + -3)));
		double 	fSlow593 = ((3 * fSlow591) + -1);
		double 	fSlow594 = ((3 * fSlow588) + -1);
		double 	fSlow595 = ((fSlow588 * ((35 * fSlow588) + -30)) + 3);
		double 	fSlow596 = ((fSlow591 * ((35 * fSlow591) + -30)) + 3);
		double 	fSlow597 = (fSlow519 - fSlow435);
		double 	fSlow598 = faustpower<2>(fSlow597);
		double 	fSlow599 = ((((35 * fSlow598) + -30) * fSlow598) + 3);
		double 	fSlow600 = (fSlow515 - fSlow430);
		double 	fSlow601 = faustpower<2>(fSlow600);
		double 	fSlow602 = ((fSlow601 * ((35 * fSlow601) + -30)) + 3);
		double 	fSlow603 = (3.5 * (((5 * fSlow601) + -3) * fSlow85));
		double 	fSlow604 = (3.5 * (((5 * fSlow598) + -3) * fSlow85));
		double 	fSlow605 = ((3 * fSlow601) + -1);
		double 	fSlow606 = ((3 * fSlow598) + -1);
		double 	fSlow607 = (fSlow379 - fSlow380);
		double 	fSlow608 = faustpower<2>(fSlow607);
		double 	fSlow609 = (3.5 * (((5 * fSlow608) + -3) * fSlow85));
		double 	fSlow610 = (fSlow384 - fSlow385);
		double 	fSlow611 = faustpower<2>(fSlow610);
		double 	fSlow612 = (3.5 * (((5 * fSlow611) + -3) * fSlow85));
		double 	fSlow613 = ((fSlow611 * ((35 * fSlow611) + -30)) + 3);
		double 	fSlow614 = ((fSlow608 * ((35 * fSlow608) + -30)) + 3);
		double 	fSlow615 = ((3 * fSlow611) + -1);
		double 	fSlow616 = ((3 * fSlow608) + -1);
		double 	fSlow617 = (fSlow397 - fSlow380);
		double 	fSlow618 = faustpower<2>(fSlow617);
		double 	fSlow619 = (3.5 * (((5 * fSlow618) + -3) * fSlow85));
		double 	fSlow620 = (fSlow393 - fSlow385);
		double 	fSlow621 = faustpower<2>(fSlow620);
		double 	fSlow622 = (3.5 * (((5 * fSlow621) + -3) * fSlow85));
		double 	fSlow623 = ((fSlow621 * ((35 * fSlow621) + -30)) + 3);
		double 	fSlow624 = ((fSlow618 * ((35 * fSlow618) + -30)) + 3);
		double 	fSlow625 = ((3 * fSlow621) + -1);
		double 	fSlow626 = ((3 * fSlow618) + -1);
		double 	fSlow627 = (fSlow405 - fSlow380);
		double 	fSlow628 = faustpower<2>(fSlow627);
		double 	fSlow629 = (3.5 * (((5 * fSlow628) + -3) * fSlow85));
		double 	fSlow630 = (fSlow409 - fSlow385);
		double 	fSlow631 = faustpower<2>(fSlow630);
		double 	fSlow632 = (3.5 * (((5 * fSlow631) + -3) * fSlow85));
		double 	fSlow633 = ((fSlow631 * ((35 * fSlow631) + -30)) + 3);
		double 	fSlow634 = ((fSlow628 * ((35 * fSlow628) + -30)) + 3);
		double 	fSlow635 = ((3 * fSlow631) + -1);
		double 	fSlow636 = ((3 * fSlow628) + -1);
		double 	fSlow637 = (fSlow417 - fSlow380);
		double 	fSlow638 = faustpower<2>(fSlow637);
		double 	fSlow639 = (3.5 * (((5 * fSlow638) + -3) * fSlow85));
		double 	fSlow640 = (fSlow421 - fSlow385);
		double 	fSlow641 = faustpower<2>(fSlow640);
		double 	fSlow642 = (3.5 * (((5 * fSlow641) + -3) * fSlow85));
		double 	fSlow643 = ((fSlow641 * ((35 * fSlow641) + -30)) + 3);
		double 	fSlow644 = ((fSlow638 * ((35 * fSlow638) + -30)) + 3);
		double 	fSlow645 = ((3 * fSlow641) + -1);
		double 	fSlow646 = ((3 * fSlow638) + -1);
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
			double fTemp0 = (fRec3[0] * (double)input0[i]);
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
			double fTemp5 = (fRec24[0] * (double)input1[i]);
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
			fRec35[0] = (fSlow62 * ((fSlow64 * fTemp0) - ((fSlow66 * fRec36[0]) + (fSlow65 * fRec37[0]))));
			fRec38[0] = (fRec35[0] + fRec38[1]);
			double fTemp10 = (fRec38[0] - fRec35[0]);
			fVec4[0] = fTemp10;
			fRec39[0] = (fRec39[1] + fVec4[1]);
			fRec41[0] = (fRec40[1] + fRec41[1]);
			fRec42[0] = (fRec41[1] + fRec42[1]);
			fRec40[0] = (0 - (fSlow62 * (((fSlow66 * fRec41[0]) + (fSlow65 * fRec42[0])) - fTemp3)));
			double fTemp11 = ((fRec35[0] + (fSlow67 * ((fSlow69 * fVec4[0]) + (fSlow68 * fRec39[0])))) + fRec40[0]);
			fRec44[0] = (fRec43[1] + fRec44[1]);
			fRec45[0] = (fRec44[1] + fRec45[1]);
			fRec43[0] = (0 - (fSlow62 * (((fSlow66 * fRec44[0]) + (fSlow65 * fRec45[0])) - (fSlow72 * fTemp5))));
			fRec46[0] = (fRec43[0] + fRec46[1]);
			double fTemp12 = (fRec46[0] - fRec43[0]);
			fVec5[0] = fTemp12;
			fRec47[0] = (fRec47[1] + fVec5[1]);
			fRec49[0] = (fRec48[1] + fRec49[1]);
			fRec50[0] = (fRec49[1] + fRec50[1]);
			fRec48[0] = (0 - (fSlow62 * (((fSlow66 * fRec49[0]) + (fSlow65 * fRec50[0])) - fTemp8)));
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
			fRec71[0] = (fRec70[1] + fRec71[1]);
			fRec70[0] = (fSlow82 * ((fSlow101 * fTemp5) - (fSlow84 * fRec71[0])));
			fRec69[0] = (fRec70[0] + fRec69[1]);
			fRec73[0] = (fRec72[1] + fRec73[1]);
			fRec72[0] = (fSlow82 * (fTemp8 - (fSlow84 * fRec73[0])));
			double fTemp19 = ((fSlow100 * (fRec69[0] - fRec70[0])) + (fRec70[0] + fRec72[0]));
			double fTemp20 = (fSlow79 * fTemp19);
			fRec77[0] = (fRec76[1] + fRec77[1]);
			fRec78[0] = (fRec77[1] + fRec78[1]);
			fRec76[0] = (0 - (fSlow90 * (((fSlow94 * fRec77[0]) + (fSlow93 * fRec78[0])) - (fSlow106 * fTemp5))));
			fRec79[0] = (fRec76[0] + fRec79[1]);
			double fTemp21 = (fRec79[0] - fRec76[0]);
			fVec7[0] = fTemp21;
			fRec80[0] = (fRec80[1] + fVec7[1]);
			fRec81[0] = (fRec75[1] + fRec81[1]);
			fRec75[0] = (fSlow89 * ((fSlow103 * (fRec76[0] + (fSlow107 * ((fSlow109 * fVec7[0]) + (fSlow108 * fRec80[0]))))) - (fSlow98 * fRec81[0])));
			fRec74[0] = (fRec75[0] + fRec74[1]);
			fRec84[0] = (fRec83[1] + fRec84[1]);
			fRec85[0] = (fRec84[1] + fRec85[1]);
			fRec83[0] = (0 - (fSlow90 * (((fSlow94 * fRec84[0]) + (fSlow93 * fRec85[0])) - fTemp8)));
			fRec86[0] = (fRec82[1] + fRec86[1]);
			fRec82[0] = (fSlow89 * (fRec83[0] - (fSlow98 * fRec86[0])));
			double fTemp22 = ((fSlow104 * (fRec74[0] - fRec75[0])) + (fRec75[0] + fRec82[0]));
			double fTemp23 = (0.012698412698412698 * ((fSlow0 * ((fSlow9 * fTemp4) + (fSlow43 * fTemp9))) + ((fSlow60 * ((fSlow61 * fTemp11) + (fSlow70 * fTemp13))) + (fTemp14 + ((fSlow7 * (fTemp16 + (fSlow86 * fTemp18))) + (fSlow41 * (fTemp20 + (fSlow102 * fTemp22))))))));
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fbargraph0 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp23;
			double fTemp24 = (0.012698412698412698 * ((fSlow60 * ((fSlow111 * fTemp13) + (fSlow113 * fTemp11))) + (((fSlow0 * ((fSlow114 * fTemp4) + (fSlow115 * fTemp9))) + (fTemp14 + (fSlow79 * ((fSlow5 * fTemp15) + (fSlow39 * fTemp19))))) + (fSlow116 * ((fSlow117 * fTemp18) + (fSlow118 * fTemp22))))));
			fRec87[0] = max((fRec87[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fbargraph1 = fRec87[0];
			output1[i] = (FAUSTFLOAT)fTemp24;
			double fTemp25 = (0.012698412698412698 * (((fSlow60 * ((fSlow121 * fTemp13) + (fSlow124 * fTemp11))) + ((fTemp14 + (fSlow116 * ((fSlow125 * fTemp22) + (fSlow126 * fTemp18)))) + (fSlow0 * ((fSlow127 * fTemp4) + (fSlow128 * fTemp9))))) + (fSlow79 * ((fSlow122 * fTemp15) + (fSlow119 * fTemp19)))));
			fRec88[0] = max((fRec88[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fbargraph2 = fRec88[0];
			output2[i] = (FAUSTFLOAT)fTemp25;
			double fTemp26 = (0.012698412698412698 * ((fSlow116 * ((fSlow130 * fTemp22) + (fSlow132 * fTemp18))) + (((fTemp14 + (fSlow0 * ((fSlow134 * fTemp4) + (fSlow136 * fTemp9)))) + (fSlow60 * ((fSlow137 * fTemp11) + (fSlow138 * fTemp13)))) + (fSlow79 * ((fSlow129 * fTemp19) + (fSlow131 * fTemp15))))));
			fRec89[0] = max((fRec89[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fbargraph3 = fRec89[0];
			output3[i] = (FAUSTFLOAT)fTemp26;
			double fTemp27 = (0.012698412698412698 * ((fSlow116 * ((fSlow140 * fTemp18) + (fSlow142 * fTemp22))) + (((fSlow79 * ((fSlow139 * fTemp15) + (fSlow141 * fTemp19))) + (fTemp14 + (fSlow60 * ((fSlow144 * fTemp13) + (fSlow146 * fTemp11))))) + (fSlow0 * ((fSlow147 * fTemp9) + (fSlow148 * fTemp4))))));
			fRec90[0] = max((fRec90[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fbargraph4 = fRec90[0];
			output4[i] = (FAUSTFLOAT)fTemp27;
			double fTemp28 = (0.012698412698412698 * ((((fSlow149 * (fTemp20 + (fSlow151 * fTemp22))) + (fTemp14 + (fSlow152 * (fTemp16 + (fSlow154 * fTemp18))))) + (fSlow0 * ((fSlow155 * fTemp4) + (fSlow156 * fTemp9)))) + (fSlow60 * ((fSlow157 * fTemp11) + (fSlow158 * fTemp13)))));
			fRec91[0] = max((fRec91[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph5 = fRec91[0];
			output5[i] = (FAUSTFLOAT)fTemp28;
			double fTemp29 = (0.022574955908289243 * ((((fSlow161 * (fTemp20 + (fSlow163 * fTemp22))) + (fTemp14 + (fSlow166 * (fTemp16 + (fSlow168 * fTemp18))))) + (fSlow0 * ((fSlow169 * fTemp4) + (fSlow170 * fTemp9)))) + (fSlow60 * ((fSlow171 * fTemp11) + (fSlow172 * fTemp13)))));
			fRec92[0] = max((fRec92[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph6 = fRec92[0];
			output6[i] = (FAUSTFLOAT)fTemp29;
			double fTemp30 = (0.022574955908289243 * ((fSlow60 * ((fSlow176 * fTemp11) + (fSlow180 * fTemp13))) + ((fSlow0 * ((fSlow181 * fTemp4) + (fSlow182 * fTemp9))) + ((fSlow178 * (fTemp20 + (fSlow183 * fTemp22))) + (fTemp14 + (fSlow174 * (fTemp16 + (fSlow184 * fTemp18))))))));
			fRec93[0] = max((fRec93[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fbargraph7 = fRec93[0];
			output7[i] = (FAUSTFLOAT)fTemp30;
			double fTemp31 = (0.022574955908289243 * (((fSlow60 * ((fSlow188 * fTemp11) + (fSlow192 * fTemp13))) + ((fSlow186 * (fTemp16 + (fSlow193 * fTemp18))) + (fTemp14 + (fSlow190 * (fTemp20 + (fSlow194 * fTemp22)))))) + (fSlow0 * ((fSlow195 * fTemp4) + (fSlow196 * fTemp9)))));
			fRec94[0] = max((fRec94[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fbargraph8 = fRec94[0];
			output8[i] = (FAUSTFLOAT)fTemp31;
			double fTemp32 = (0.022574955908289243 * (((fSlow60 * ((fSlow200 * fTemp11) + (fSlow204 * fTemp13))) + ((fSlow202 * (fTemp20 + (fSlow205 * fTemp22))) + (fTemp14 + (fSlow198 * (fTemp16 + (fSlow206 * fTemp18)))))) + (fSlow0 * ((fSlow207 * fTemp9) + (fSlow208 * fTemp4)))));
			fRec95[0] = max((fRec95[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph9 = fRec95[0];
			output9[i] = (FAUSTFLOAT)fTemp32;
			double fTemp33 = (0.022574955908289243 * ((fSlow79 * ((fSlow209 * fTemp15) + (fSlow210 * fTemp19))) + (((fTemp14 + (fSlow0 * ((fSlow212 * fTemp4) + (fSlow214 * fTemp9)))) + (fSlow116 * ((fSlow215 * fTemp22) + (fSlow216 * fTemp18)))) + (fSlow60 * ((fSlow217 * fTemp11) + (fSlow218 * fTemp13))))));
			fRec96[0] = max((fRec96[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph10 = fRec96[0];
			output10[i] = (FAUSTFLOAT)fTemp33;
			double fTemp34 = (0.022574955908289243 * ((fSlow79 * ((fSlow219 * fTemp15) + (fSlow220 * fTemp19))) + (((fTemp14 + (fSlow0 * ((fSlow222 * fTemp4) + (fSlow224 * fTemp9)))) + (fSlow116 * ((fSlow225 * fTemp22) + (fSlow226 * fTemp18)))) + (fSlow60 * ((fSlow227 * fTemp11) + (fSlow228 * fTemp13))))));
			fRec97[0] = max((fRec97[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fbargraph11 = fRec97[0];
			output11[i] = (FAUSTFLOAT)fTemp34;
			double fTemp35 = (0.022574955908289243 * (((fSlow60 * ((fSlow231 * fTemp13) + (fSlow234 * fTemp11))) + ((fTemp14 + (fSlow116 * ((fSlow235 * fTemp22) + (fSlow236 * fTemp18)))) + (fSlow0 * ((fSlow237 * fTemp4) + (fSlow238 * fTemp9))))) + (fSlow79 * ((fSlow232 * fTemp15) + (fSlow229 * fTemp19)))));
			fRec98[0] = max((fRec98[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fbargraph12 = fRec98[0];
			output12[i] = (FAUSTFLOAT)fTemp35;
			double fTemp36 = (0.022574955908289243 * ((fSlow0 * ((fSlow241 * fTemp4) + (fSlow244 * fTemp9))) + ((fSlow79 * ((fSlow242 * fTemp19) + (fSlow239 * fTemp15))) + ((fTemp14 + (fSlow116 * ((fSlow245 * fTemp18) + (fSlow246 * fTemp22)))) + (fSlow60 * ((fSlow247 * fTemp11) + (fSlow248 * fTemp13)))))));
			fRec99[0] = max((fRec99[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph13 = fRec99[0];
			output13[i] = (FAUSTFLOAT)fTemp36;
			double fTemp37 = (0.022574955908289243 * ((fSlow60 * ((fSlow251 * fTemp11) + (fSlow254 * fTemp13))) + ((fSlow0 * ((fSlow255 * fTemp4) + (fSlow256 * fTemp9))) + ((fSlow252 * (fTemp20 + (fSlow257 * fTemp22))) + (fTemp14 + (fSlow249 * (fTemp16 + (fSlow258 * fTemp18))))))));
			fRec100[0] = max((fRec100[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fbargraph14 = fRec100[0];
			output14[i] = (FAUSTFLOAT)fTemp37;
			double fTemp38 = (0.022574955908289243 * ((fSlow0 * ((fSlow261 * fTemp9) + (fSlow264 * fTemp4))) + (((fSlow259 * (fTemp20 + (fSlow265 * fTemp22))) + (fTemp14 + (fSlow262 * (fTemp16 + (fSlow266 * fTemp18))))) + (fSlow60 * ((fSlow267 * fTemp13) + (fSlow268 * fTemp11))))));
			fRec101[0] = max((fRec101[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fbargraph15 = fRec101[0];
			output15[i] = (FAUSTFLOAT)fTemp38;
			double fTemp39 = (0.022574955908289243 * ((fSlow0 * ((fSlow271 * fTemp9) + (fSlow274 * fTemp4))) + (((fSlow269 * (fTemp20 + (fSlow275 * fTemp22))) + (fTemp14 + (fSlow272 * (fTemp16 + (fSlow276 * fTemp18))))) + (fSlow60 * ((fSlow277 * fTemp13) + (fSlow278 * fTemp11))))));
			fRec102[0] = max((fRec102[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fbargraph16 = fRec102[0];
			output16[i] = (FAUSTFLOAT)fTemp39;
			double fTemp40 = (0.022574955908289243 * ((fSlow0 * ((fSlow281 * fTemp4) + (fSlow284 * fTemp9))) + (((fSlow282 * (fTemp20 + (fSlow285 * fTemp22))) + (fTemp14 + (fSlow279 * (fTemp16 + (fSlow286 * fTemp18))))) + (fSlow60 * ((fSlow287 * fTemp13) + (fSlow288 * fTemp11))))));
			fRec103[0] = max((fRec103[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fbargraph17 = fRec103[0];
			output17[i] = (FAUSTFLOAT)fTemp40;
			double fTemp41 = (0.02109375 * ((fSlow0 * ((fSlow293 * fTemp4) + (fSlow298 * fTemp9))) + ((fSlow60 * ((fSlow299 * fTemp13) + (fSlow300 * fTemp11))) + ((fSlow296 * (fTemp20 + (fSlow301 * fTemp22))) + (fTemp14 + (fSlow291 * (fTemp16 + (fSlow302 * fTemp18))))))));
			fRec104[0] = max((fRec104[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fbargraph18 = fRec104[0];
			output18[i] = (FAUSTFLOAT)fTemp41;
			double fTemp42 = (0.02109375 * ((fSlow60 * ((fSlow306 * fTemp13) + (fSlow310 * fTemp11))) + ((fSlow0 * ((fSlow311 * fTemp9) + (fSlow312 * fTemp4))) + ((fSlow304 * (fTemp20 + (fSlow313 * fTemp22))) + (fTemp14 + (fSlow308 * (fTemp16 + (fSlow314 * fTemp18))))))));
			fRec105[0] = max((fRec105[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fbargraph19 = fRec105[0];
			output19[i] = (FAUSTFLOAT)fTemp42;
			double fTemp43 = (0.02109375 * (((fSlow60 * ((fSlow318 * fTemp13) + (fSlow322 * fTemp11))) + ((fSlow316 * (fTemp20 + (fSlow323 * fTemp22))) + (fTemp14 + (fSlow320 * (fTemp16 + (fSlow324 * fTemp18)))))) + (fSlow0 * ((fSlow325 * fTemp9) + (fSlow326 * fTemp4)))));
			fRec106[0] = max((fRec106[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fbargraph20 = fRec106[0];
			output20[i] = (FAUSTFLOAT)fTemp43;
			double fTemp44 = (0.02109375 * ((((fSlow328 * (fTemp20 + (fSlow330 * fTemp22))) + (fTemp14 + (fSlow332 * (fTemp16 + (fSlow334 * fTemp18))))) + (fSlow60 * ((fSlow335 * fTemp11) + (fSlow336 * fTemp13)))) + (fSlow0 * ((fSlow337 * fTemp9) + (fSlow338 * fTemp4)))));
			fRec107[0] = max((fRec107[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fbargraph21 = fRec107[0];
			output21[i] = (FAUSTFLOAT)fTemp44;
			double fTemp45 = (0.02109375 * ((((fSlow339 * (fTemp20 + (fSlow341 * fTemp22))) + (fTemp14 + (fSlow342 * (fTemp16 + (fSlow344 * fTemp18))))) + (fSlow60 * ((fSlow345 * fTemp11) + (fSlow346 * fTemp13)))) + (fSlow0 * ((fSlow347 * fTemp4) + (fSlow348 * fTemp9)))));
			fRec108[0] = max((fRec108[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fbargraph22 = fRec108[0];
			output22[i] = (FAUSTFLOAT)fTemp45;
			double fTemp46 = (0.02109375 * ((fSlow60 * ((fSlow351 * fTemp11) + (fSlow354 * fTemp13))) + ((fSlow0 * ((fSlow355 * fTemp4) + (fSlow356 * fTemp9))) + ((fSlow352 * (fTemp20 + (fSlow357 * fTemp22))) + (fTemp14 + (fSlow349 * (fTemp16 + (fSlow358 * fTemp18))))))));
			fRec109[0] = max((fRec109[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fbargraph23 = fRec109[0];
			output23[i] = (FAUSTFLOAT)fTemp46;
			double fTemp47 = (0.02109375 * ((fSlow0 * ((fSlow361 * fTemp9) + (fSlow364 * fTemp4))) + ((fSlow60 * ((fSlow365 * fTemp13) + (fSlow366 * fTemp11))) + ((fSlow359 * (fTemp20 + (fSlow367 * fTemp22))) + (fTemp14 + (fSlow362 * (fTemp16 + (fSlow368 * fTemp18))))))));
			fRec110[0] = max((fRec110[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fbargraph24 = fRec110[0];
			output24[i] = (FAUSTFLOAT)fTemp47;
			double fTemp48 = (0.02109375 * ((((fSlow369 * (fTemp20 + (fSlow371 * fTemp22))) + (fTemp14 + (fSlow372 * (fTemp16 + (fSlow374 * fTemp18))))) + (fSlow60 * ((fSlow375 * fTemp11) + (fSlow376 * fTemp13)))) + (fSlow0 * ((fSlow377 * fTemp4) + (fSlow378 * fTemp9)))));
			fRec111[0] = max((fRec111[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fbargraph25 = fRec111[0];
			output25[i] = (FAUSTFLOAT)fTemp48;
			double fTemp49 = (0.02017333553791887 * ((((fSlow381 * (fTemp20 + (fSlow383 * fTemp22))) + (fTemp14 + (fSlow386 * (fTemp16 + (fSlow388 * fTemp18))))) + (fSlow0 * ((fSlow389 * fTemp4) + (fSlow390 * fTemp9)))) + (fSlow60 * ((fSlow391 * fTemp13) + (fSlow392 * fTemp11)))));
			fRec112[0] = max((fRec112[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fbargraph26 = fRec112[0];
			output26[i] = (FAUSTFLOAT)fTemp49;
			double fTemp50 = (0.02017333553791887 * ((fSlow0 * ((fSlow396 * fTemp4) + (fSlow400 * fTemp9))) + (((fSlow398 * (fTemp20 + (fSlow401 * fTemp22))) + (fTemp14 + (fSlow394 * (fTemp16 + (fSlow402 * fTemp18))))) + (fSlow60 * ((fSlow403 * fTemp13) + (fSlow404 * fTemp11))))));
			fRec113[0] = max((fRec113[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fbargraph27 = fRec113[0];
			output27[i] = (FAUSTFLOAT)fTemp50;
			double fTemp51 = (0.02017333553791887 * ((((fSlow406 * (fTemp20 + (fSlow408 * fTemp22))) + (fTemp14 + (fSlow410 * (fTemp16 + (fSlow412 * fTemp18))))) + (fSlow60 * ((fSlow413 * fTemp11) + (fSlow414 * fTemp13)))) + (fSlow0 * ((fSlow415 * fTemp4) + (fSlow416 * fTemp9)))));
			fRec114[0] = max((fRec114[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fbargraph28 = fRec114[0];
			output28[i] = (FAUSTFLOAT)fTemp51;
			double fTemp52 = (0.02017333553791887 * ((((fSlow418 * (fTemp20 + (fSlow420 * fTemp22))) + (fTemp14 + (fSlow422 * (fTemp16 + (fSlow424 * fTemp18))))) + (fSlow60 * ((fSlow425 * fTemp11) + (fSlow426 * fTemp13)))) + (fSlow0 * ((fSlow427 * fTemp9) + (fSlow428 * fTemp4)))));
			fRec115[0] = max((fRec115[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fbargraph29 = fRec115[0];
			output29[i] = (FAUSTFLOAT)fTemp52;
			double fTemp53 = (0.02017333553791887 * ((fSlow0 * ((fSlow433 * fTemp9) + (fSlow438 * fTemp4))) + ((fSlow60 * ((fSlow439 * fTemp13) + (fSlow440 * fTemp11))) + ((fSlow431 * (fTemp20 + (fSlow441 * fTemp22))) + (fTemp14 + (fSlow436 * (fTemp16 + (fSlow442 * fTemp18))))))));
			fRec116[0] = max((fRec116[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fbargraph30 = fRec116[0];
			output30[i] = (FAUSTFLOAT)fTemp53;
			double fTemp54 = (0.02017333553791887 * ((((fSlow444 * (fTemp20 + (fSlow446 * fTemp22))) + (fTemp14 + (fSlow448 * (fTemp16 + (fSlow450 * fTemp18))))) + (fSlow0 * ((fSlow451 * fTemp4) + (fSlow452 * fTemp9)))) + (fSlow60 * ((fSlow453 * fTemp13) + (fSlow454 * fTemp11)))));
			fRec117[0] = max((fRec117[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fbargraph31 = fRec117[0];
			output31[i] = (FAUSTFLOAT)fTemp54;
			double fTemp55 = (0.02017333553791887 * ((((fSlow456 * (fTemp20 + (fSlow458 * fTemp22))) + (fTemp14 + (fSlow460 * (fTemp16 + (fSlow462 * fTemp18))))) + (fSlow60 * ((fSlow463 * fTemp11) + (fSlow464 * fTemp13)))) + (fSlow0 * ((fSlow465 * fTemp9) + (fSlow466 * fTemp4)))));
			fRec118[0] = max((fRec118[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fbargraph32 = fRec118[0];
			output32[i] = (FAUSTFLOAT)fTemp55;
			double fTemp56 = (0.02017333553791887 * ((fSlow60 * ((fSlow470 * fTemp11) + (fSlow474 * fTemp13))) + ((fSlow0 * ((fSlow475 * fTemp4) + (fSlow476 * fTemp9))) + ((fSlow472 * (fTemp20 + (fSlow477 * fTemp22))) + (fTemp14 + (fSlow468 * (fTemp16 + (fSlow478 * fTemp18))))))));
			fRec119[0] = max((fRec119[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fbargraph33 = fRec119[0];
			output33[i] = (FAUSTFLOAT)fTemp56;
			double fTemp57 = (0.02017333553791887 * ((((fSlow480 * (fTemp20 + (fSlow482 * fTemp22))) + (fTemp14 + (fSlow484 * (fTemp16 + (fSlow486 * fTemp18))))) + (fSlow60 * ((fSlow487 * fTemp13) + (fSlow488 * fTemp11)))) + (fSlow0 * ((fSlow489 * fTemp4) + (fSlow490 * fTemp9)))));
			fRec120[0] = max((fRec120[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fbargraph34 = fRec120[0];
			output34[i] = (FAUSTFLOAT)fTemp57;
			double fTemp58 = (0.02017333553791887 * (((fSlow60 * ((fSlow494 * fTemp13) + (fSlow498 * fTemp11))) + ((fSlow492 * (fTemp20 + (fSlow499 * fTemp22))) + (fTemp14 + (fSlow496 * (fTemp16 + (fSlow500 * fTemp18)))))) + (fSlow0 * ((fSlow501 * fTemp9) + (fSlow502 * fTemp4)))));
			fRec121[0] = max((fRec121[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fbargraph35 = fRec121[0];
			output35[i] = (FAUSTFLOAT)fTemp58;
			double fTemp59 = (0.02017333553791887 * ((fSlow0 * ((fSlow506 * fTemp4) + (fSlow510 * fTemp9))) + (((fSlow508 * (fTemp20 + (fSlow511 * fTemp22))) + (fTemp14 + (fSlow504 * (fTemp16 + (fSlow512 * fTemp18))))) + (fSlow60 * ((fSlow513 * fTemp13) + (fSlow514 * fTemp11))))));
			fRec122[0] = max((fRec122[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fbargraph36 = fRec122[0];
			output36[i] = (FAUSTFLOAT)fTemp59;
			double fTemp60 = (0.02017333553791887 * ((((fSlow516 * (fTemp20 + (fSlow518 * fTemp22))) + (fTemp14 + (fSlow520 * (fTemp16 + (fSlow522 * fTemp18))))) + (fSlow60 * ((fSlow523 * fTemp11) + (fSlow524 * fTemp13)))) + (fSlow0 * ((fSlow525 * fTemp9) + (fSlow526 * fTemp4)))));
			fRec123[0] = max((fRec123[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fbargraph37 = fRec123[0];
			output37[i] = (FAUSTFLOAT)fTemp60;
			double fTemp61 = (0.02017333553791887 * ((fSlow0 * ((fSlow529 * fTemp9) + (fSlow532 * fTemp4))) + (((fSlow527 * (fTemp20 + (fSlow533 * fTemp22))) + (fTemp14 + (fSlow530 * (fTemp16 + (fSlow534 * fTemp18))))) + (fSlow60 * ((fSlow535 * fTemp13) + (fSlow536 * fTemp11))))));
			fRec124[0] = max((fRec124[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fbargraph38 = fRec124[0];
			output38[i] = (FAUSTFLOAT)fTemp61;
			double fTemp62 = (0.02017333553791887 * (((fSlow60 * ((fSlow539 * fTemp11) + (fSlow542 * fTemp13))) + ((fSlow540 * (fTemp20 + (fSlow543 * fTemp22))) + (fTemp14 + (fSlow537 * (fTemp16 + (fSlow544 * fTemp18)))))) + (fSlow0 * ((fSlow545 * fTemp4) + (fSlow546 * fTemp9)))));
			fRec125[0] = max((fRec125[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fbargraph39 = fRec125[0];
			output39[i] = (FAUSTFLOAT)fTemp62;
			double fTemp63 = (0.02017333553791887 * (((fSlow60 * ((fSlow549 * fTemp11) + (fSlow552 * fTemp13))) + ((fSlow550 * (fTemp20 + (fSlow553 * fTemp22))) + (fTemp14 + (fSlow547 * (fTemp16 + (fSlow554 * fTemp18)))))) + (fSlow0 * ((fSlow555 * fTemp4) + (fSlow556 * fTemp9)))));
			fRec126[0] = max((fRec126[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fbargraph40 = fRec126[0];
			output40[i] = (FAUSTFLOAT)fTemp63;
			double fTemp64 = (0.02017333553791887 * ((((fSlow557 * (fTemp20 + (fSlow559 * fTemp22))) + (fTemp14 + (fSlow560 * (fTemp16 + (fSlow562 * fTemp18))))) + (fSlow60 * ((fSlow563 * fTemp13) + (fSlow564 * fTemp11)))) + (fSlow0 * ((fSlow565 * fTemp4) + (fSlow566 * fTemp9)))));
			fRec127[0] = max((fRec127[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fbargraph41 = fRec127[0];
			output41[i] = (FAUSTFLOAT)fTemp64;
			double fTemp65 = (0.02017333553791887 * ((((fSlow567 * (fTemp20 + (fSlow569 * fTemp22))) + (fTemp14 + (fSlow570 * (fTemp16 + (fSlow572 * fTemp18))))) + (fSlow60 * ((fSlow573 * fTemp11) + (fSlow574 * fTemp13)))) + (fSlow0 * ((fSlow575 * fTemp9) + (fSlow576 * fTemp4)))));
			fRec128[0] = max((fRec128[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fbargraph42 = fRec128[0];
			output42[i] = (FAUSTFLOAT)fTemp65;
			double fTemp66 = (0.02017333553791887 * ((((fSlow577 * (fTemp20 + (fSlow579 * fTemp22))) + (fTemp14 + (fSlow580 * (fTemp16 + (fSlow582 * fTemp18))))) + (fSlow60 * ((fSlow583 * fTemp13) + (fSlow584 * fTemp11)))) + (fSlow0 * ((fSlow585 * fTemp4) + (fSlow586 * fTemp9)))));
			fRec129[0] = max((fRec129[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fbargraph43 = fRec129[0];
			output43[i] = (FAUSTFLOAT)fTemp66;
			double fTemp67 = (0.02017333553791887 * ((((fSlow587 * (fTemp20 + (fSlow589 * fTemp22))) + (fTemp14 + (fSlow590 * (fTemp16 + (fSlow592 * fTemp18))))) + (fSlow60 * ((fSlow593 * fTemp11) + (fSlow594 * fTemp13)))) + (fSlow0 * ((fSlow595 * fTemp9) + (fSlow596 * fTemp4)))));
			fRec130[0] = max((fRec130[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp67))))));
			fbargraph44 = fRec130[0];
			output44[i] = (FAUSTFLOAT)fTemp67;
			double fTemp68 = (0.02017333553791887 * ((fSlow0 * ((fSlow599 * fTemp4) + (fSlow602 * fTemp9))) + (((fSlow600 * (fTemp20 + (fSlow603 * fTemp22))) + (fTemp14 + (fSlow597 * (fTemp16 + (fSlow604 * fTemp18))))) + (fSlow60 * ((fSlow605 * fTemp13) + (fSlow606 * fTemp11))))));
			fRec131[0] = max((fRec131[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fbargraph45 = fRec131[0];
			output45[i] = (FAUSTFLOAT)fTemp68;
			double fTemp69 = (0.02017333553791887 * ((((fSlow607 * (fTemp20 + (fSlow609 * fTemp22))) + (fTemp14 + (fSlow610 * (fTemp16 + (fSlow612 * fTemp18))))) + (fSlow0 * ((fSlow613 * fTemp4) + (fSlow614 * fTemp9)))) + (fSlow60 * ((fSlow615 * fTemp11) + (fSlow616 * fTemp13)))));
			fRec132[0] = max((fRec132[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp69))))));
			fbargraph46 = fRec132[0];
			output46[i] = (FAUSTFLOAT)fTemp69;
			double fTemp70 = (0.02017333553791887 * ((((fSlow617 * (fTemp20 + (fSlow619 * fTemp22))) + (fTemp14 + (fSlow620 * (fTemp16 + (fSlow622 * fTemp18))))) + (fSlow0 * ((fSlow623 * fTemp4) + (fSlow624 * fTemp9)))) + (fSlow60 * ((fSlow625 * fTemp11) + (fSlow626 * fTemp13)))));
			fRec133[0] = max((fRec133[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp70))))));
			fbargraph47 = fRec133[0];
			output47[i] = (FAUSTFLOAT)fTemp70;
			double fTemp71 = (0.02017333553791887 * ((((fSlow627 * (fTemp20 + (fSlow629 * fTemp22))) + (fTemp14 + (fSlow630 * (fTemp16 + (fSlow632 * fTemp18))))) + (fSlow0 * ((fSlow633 * fTemp4) + (fSlow634 * fTemp9)))) + (fSlow60 * ((fSlow635 * fTemp11) + (fSlow636 * fTemp13)))));
			fRec134[0] = max((fRec134[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fbargraph48 = fRec134[0];
			output48[i] = (FAUSTFLOAT)fTemp71;
			double fTemp72 = (0.02017333553791887 * ((((fSlow637 * (fTemp20 + (fSlow639 * fTemp22))) + (fTemp14 + (fSlow640 * (fTemp16 + (fSlow642 * fTemp18))))) + (fSlow0 * ((fSlow643 * fTemp4) + (fSlow644 * fTemp9)))) + (fSlow60 * ((fSlow645 * fTemp11) + (fSlow646 * fTemp13)))));
			fRec135[0] = max((fRec135[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp72))))));
			fbargraph49 = fRec135[0];
			output49[i] = (FAUSTFLOAT)fTemp72;
			// post processing
			fRec135[1] = fRec135[0];
			fRec134[1] = fRec134[0];
			fRec133[1] = fRec133[0];
			fRec132[1] = fRec132[0];
			fRec131[1] = fRec131[0];
			fRec130[1] = fRec130[0];
			fRec129[1] = fRec129[0];
			fRec128[1] = fRec128[0];
			fRec127[1] = fRec127[0];
			fRec126[1] = fRec126[0];
			fRec125[1] = fRec125[0];
			fRec124[1] = fRec124[0];
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
			fRec0[1] = fRec0[0];
			fRec82[1] = fRec82[0];
			fRec86[1] = fRec86[0];
			fRec83[1] = fRec83[0];
			fRec85[1] = fRec85[0];
			fRec84[1] = fRec84[0];
			fRec74[1] = fRec74[0];
			fRec75[1] = fRec75[0];
			fRec81[1] = fRec81[0];
			fRec80[1] = fRec80[0];
			fVec7[1] = fVec7[0];
			fRec79[1] = fRec79[0];
			fRec76[1] = fRec76[0];
			fRec78[1] = fRec78[0];
			fRec77[1] = fRec77[0];
			fRec72[1] = fRec72[0];
			fRec73[1] = fRec73[0];
			fRec69[1] = fRec69[0];
			fRec70[1] = fRec70[0];
			fRec71[1] = fRec71[0];
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
