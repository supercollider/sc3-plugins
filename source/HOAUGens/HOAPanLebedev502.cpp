//----------------------------------------------------------
// name: "HOAPanLebedev502"
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
	FAUSTFLOAT 	fentry0;
	FAUSTFLOAT 	fcheckbox0;
	FAUSTFLOAT 	fcheckbox1;
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fslider1;
	double 	fRec1[2];
	FAUSTFLOAT 	fcheckbox2;
	FAUSTFLOAT 	fslider2;
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
	FAUSTFLOAT 	fcheckbox4;
	double 	fConst4;
	double 	fConst5;
	double 	fRec14[2];
	double 	fRec15[2];
	double 	fRec13[2];
	double 	fRec16[2];
	double 	fVec0[2];
	double 	fRec17[2];
	double 	fRec19[2];
	double 	fRec20[2];
	double 	fRec18[2];
	double 	fRec22[2];
	double 	fRec23[2];
	double 	fRec21[2];
	double 	fRec24[2];
	double 	fVec1[2];
	double 	fRec25[2];
	double 	fRec27[2];
	double 	fRec28[2];
	double 	fRec26[2];
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fRec29[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec30[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec31[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fRec32[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec33[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec34[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec35[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec36[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec37[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec38[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec39[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec40[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec41[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec42[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec43[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec44[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec45[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec47[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec48[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec49[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec50[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec51[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec52[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec53[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec54[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec55[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec56[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec57[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec58[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec59[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec60[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec61[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec62[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec63[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec64[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec65[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec66[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec67[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec68[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec69[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec70[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec71[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec72[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec73[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec74[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec75[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec76[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec77[2];
	FAUSTFLOAT 	fbargraph49;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOAPanLebedev502");
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
		fConst2 = (3.4e+02 / fConst0);
		fConst3 = (1.7e+02 / fConst0);
		fConst4 = (5.1e+02 / fConst0);
		fConst5 = (1.02e+03 / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fentry0 = 1.07;
		fcheckbox0 = 0.0;
		fcheckbox1 = 0.0;
		fslider0 = 2.0;
		fslider1 = 0.0;
		fcheckbox2 = 0.0;
		fslider2 = 2.0;
		fslider3 = 0.0;
		fcheckbox3 = 0.0;
		fslider4 = 0.0;
		fslider5 = 0.0;
		fslider6 = 0.0;
		fslider7 = 0.0;
		fcheckbox4 = 0.0;
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
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fVec0[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
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
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
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
		ui_interface->declare(&fslider1, "0+1", "");
		ui_interface->declare(&fslider1, "osc", "/gain_0 -20 20");
		ui_interface->declare(&fslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  0", &fslider1, 0.0, -3e+01, 2e+01, 0.1);
		ui_interface->declare(&fslider0, "0+2", "");
		ui_interface->declare(&fslider0, "osc", "/radius_0 0.5 50");
		ui_interface->declare(&fslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  0", &fslider0, 2.0, 0.5, 5e+01, 0.01);
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
		ui_interface->addCheckButton("Yes", &fcheckbox1);
		ui_interface->closeBox();
		ui_interface->declare(&fslider3, "1+1", "");
		ui_interface->declare(&fslider3, "osc", "/gain_1 -20 20");
		ui_interface->declare(&fslider3, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  1", &fslider3, 0.0, -3e+01, 2e+01, 0.1);
		ui_interface->declare(&fslider2, "1+2", "");
		ui_interface->declare(&fslider2, "osc", "/radius_1 0.5 50");
		ui_interface->declare(&fslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  1", &fslider2, 2.0, 0.5, 5e+01, 0.01);
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
		ui_interface->addCheckButton("Yes", &fcheckbox2);
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openVerticalBox("Mute Order");
		ui_interface->addCheckButton("0", &fcheckbox0);
		ui_interface->addCheckButton("1", &fcheckbox3);
		ui_interface->addCheckButton("2", &fcheckbox4);
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
		ui_interface->addVerticalBargraph("0x7f8a41601220", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph1, "osc", "/output2");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41603770", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph2, "osc", "/output3");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41606390", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph3, "osc", "/output4");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41609000", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph4, "osc", "/output5");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4160bcd0", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph5, "osc", "/output6");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4160e5b0", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph6, "osc", "/output7");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41611600", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph7, "osc", "/output8");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41614260", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph8, "osc", "/output9");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41616f90", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph9, "osc", "/output10");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41619d60", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph10, "osc", "/output11");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4161cd80", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph11, "osc", "/output12");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4161fe10", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph12, "osc", "/output13");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41622ef0", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph13, "osc", "/output14");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41626060", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph14, "osc", "/output15");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41628dc0", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph15, "osc", "/output16");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4162bbc0", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph16, "osc", "/output17");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4162ea10", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph17, "osc", "/output18");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a416318e0", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph18, "osc", "/output19");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41634c80", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph19, "osc", "/output20");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41637ed0", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph20, "osc", "/output21");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4163b180", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph21, "osc", "/output22");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4163e4b0", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph22, "osc", "/output23");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a416415b0", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph23, "osc", "/output24");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41644710", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph24, "osc", "/output25");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a416478f0", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph25, "osc", "/output26");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4164ab40", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph26, "osc", "/output27");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4164e490", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph27, "osc", "/output28");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41650ff0", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph28, "osc", "/output29");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41653bb0", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph29, "osc", "/output30");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a416567e0", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph30, "osc", "/output31");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41659ee0", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph31, "osc", "/output32");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4165d220", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph32, "osc", "/output33");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a416605d0", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph33, "osc", "/output34");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a416639f0", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph34, "osc", "/output35");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41666e80", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fbargraph35, "osc", "/output36");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4166a380", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fbargraph36, "osc", "/output37");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4166d860", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fbargraph37, "osc", "/output38");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41670e20", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fbargraph38, "osc", "/output39");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41673be0", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fbargraph39, "osc", "/output40");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41676a00", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fbargraph40, "osc", "/output41");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41679890", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fbargraph41, "osc", "/output42");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4167c790", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fbargraph42, "osc", "/output43");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4167f700", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fbargraph43, "osc", "/output44");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a416826e0", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fbargraph44, "osc", "/output45");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41685730", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fbargraph45, "osc", "/output46");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a416887f0", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fbargraph46, "osc", "/output47");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4168b920", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fbargraph47, "osc", "/output48");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a4168eac0", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fbargraph48, "osc", "/output49");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41691cd0", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fbargraph49, "osc", "/output50");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f8a41694f50", &fbargraph49, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = double(fentry0);
		double 	fSlow1 = (fSlow0 * (1 - double(fcheckbox0)));
		double 	fSlow2 = double(fcheckbox1);
		double 	fSlow3 = double(fslider0);
		double 	fSlow4 = (1 - (fSlow2 * (1 - (1.0 / fSlow3))));
		double 	fSlow5 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider1))));
		double 	fSlow6 = double(fcheckbox2);
		double 	fSlow7 = double(fslider2);
		double 	fSlow8 = (1 - (fSlow6 * (1 - (1.0 / fSlow7))));
		double 	fSlow9 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider3))));
		double 	fSlow10 = (3 * (1 - double(fcheckbox3)));
		double 	fSlow11 = double(fslider4);
		double 	fSlow12 = sin(fSlow11);
		double 	fSlow13 = cos(fSlow11);
		double 	fSlow14 = double(fslider5);
		double 	fSlow15 = (fSlow13 * cos(fSlow14));
		double 	fSlow16 = (6.123233995736766e-17 * fSlow15);
		double 	fSlow17 = (fSlow12 + fSlow16);
		double 	fSlow18 = (fConst3 / fSlow3);
		double 	fSlow19 = (fSlow18 + 1);
		double 	fSlow20 = (fConst2 / (fSlow3 * fSlow19));
		double 	fSlow21 = (fConst3 / fSlow0);
		double 	fSlow22 = (1.0 / (fSlow21 + 1));
		double 	fSlow23 = (fSlow2 * fSlow0);
		double 	fSlow24 = ((fSlow23 * fSlow19) / fSlow3);
		double 	fSlow25 = (fConst2 / fSlow0);
		double 	fSlow26 = ((1 - fSlow2) * fSlow0);
		double 	fSlow27 = double(fslider6);
		double 	fSlow28 = sin(fSlow27);
		double 	fSlow29 = cos(fSlow27);
		double 	fSlow30 = double(fslider7);
		double 	fSlow31 = (fSlow29 * cos(fSlow30));
		double 	fSlow32 = (6.123233995736766e-17 * fSlow31);
		double 	fSlow33 = (fSlow28 + fSlow32);
		double 	fSlow34 = (fConst3 / fSlow7);
		double 	fSlow35 = (fSlow34 + 1);
		double 	fSlow36 = (fConst2 / (fSlow7 * fSlow35));
		double 	fSlow37 = (fSlow0 * fSlow6);
		double 	fSlow38 = ((fSlow37 * fSlow35) / fSlow7);
		double 	fSlow39 = (fSlow0 * (1 - fSlow6));
		double 	fSlow40 = (2.5 * (1 - double(fcheckbox4)));
		double 	fSlow41 = ((3 * faustpower<2>(fSlow33)) + -1);
		double 	fSlow42 = faustpower<2>(fSlow21);
		double 	fSlow43 = (1.0 / (((fConst4 / fSlow0) + (3 * fSlow42)) + 1));
		double 	fSlow44 = (12 * fSlow42);
		double 	fSlow45 = (fSlow44 + (fConst5 / fSlow0));
		double 	fSlow46 = faustpower<2>(fSlow34);
		double 	fSlow47 = (((fConst4 / fSlow7) + (3 * fSlow46)) + 1);
		double 	fSlow48 = ((fSlow37 * fSlow47) / fSlow7);
		double 	fSlow49 = (1.0 / fSlow47);
		double 	fSlow50 = (12 * fSlow46);
		double 	fSlow51 = ((fConst5 / fSlow7) + fSlow50);
		double 	fSlow52 = ((3 * faustpower<2>(fSlow17)) + -1);
		double 	fSlow53 = faustpower<2>(fSlow18);
		double 	fSlow54 = (((fConst4 / fSlow3) + (3 * fSlow53)) + 1);
		double 	fSlow55 = ((fSlow23 * fSlow54) / fSlow3);
		double 	fSlow56 = (1.0 / fSlow54);
		double 	fSlow57 = (12 * fSlow53);
		double 	fSlow58 = ((fConst5 / fSlow3) + fSlow57);
		double 	fSlow59 = ((3 * faustpower<2>(fSlow31)) + -1);
		double 	fSlow60 = ((3 * faustpower<2>(fSlow15)) + -1);
		double 	fSlow61 = (fSlow29 * cos((fSlow30 + -1.5707963267948966)));
		double 	fSlow62 = (fSlow13 * cos((fSlow14 + -1.5707963267948966)));
		double 	fSlow63 = ((3 * faustpower<2>(fSlow61)) + -1);
		double 	fSlow64 = ((3 * faustpower<2>(fSlow62)) + -1);
		double 	fSlow65 = (fSlow13 * cos((fSlow14 + -3.141592653589793)));
		double 	fSlow66 = ((3 * faustpower<2>(fSlow65)) + -1);
		double 	fSlow67 = (fSlow29 * cos((fSlow30 + -3.141592653589793)));
		double 	fSlow68 = ((3 * faustpower<2>(fSlow67)) + -1);
		double 	fSlow69 = (fSlow13 * cos((fSlow14 + -4.71238898038469)));
		double 	fSlow70 = ((3 * faustpower<2>(fSlow69)) + -1);
		double 	fSlow71 = (fSlow29 * cos((fSlow30 + -4.71238898038469)));
		double 	fSlow72 = ((3 * faustpower<2>(fSlow71)) + -1);
		double 	fSlow73 = (fSlow16 - fSlow12);
		double 	fSlow74 = (fSlow32 - fSlow28);
		double 	fSlow75 = ((3 * faustpower<2>(fSlow73)) + -1);
		double 	fSlow76 = ((3 * faustpower<2>(fSlow74)) + -1);
		double 	fSlow77 = (0.7071067811865476 * fSlow15);
		double 	fSlow78 = (0.7071067811865475 * fSlow12);
		double 	fSlow79 = (fSlow77 + fSlow78);
		double 	fSlow80 = (0.7071067811865476 * fSlow31);
		double 	fSlow81 = (0.7071067811865475 * fSlow28);
		double 	fSlow82 = (fSlow80 + fSlow81);
		double 	fSlow83 = ((3 * faustpower<2>(fSlow79)) + -1);
		double 	fSlow84 = ((3 * faustpower<2>(fSlow82)) + -1);
		double 	fSlow85 = (0.7071067811865476 * fSlow61);
		double 	fSlow86 = (fSlow81 + fSlow85);
		double 	fSlow87 = ((3 * faustpower<2>(fSlow86)) + -1);
		double 	fSlow88 = (0.7071067811865476 * fSlow62);
		double 	fSlow89 = (fSlow78 + fSlow88);
		double 	fSlow90 = ((3 * faustpower<2>(fSlow89)) + -1);
		double 	fSlow91 = (0.7071067811865476 * fSlow65);
		double 	fSlow92 = (fSlow78 + fSlow91);
		double 	fSlow93 = (0.7071067811865476 * fSlow67);
		double 	fSlow94 = (fSlow81 + fSlow93);
		double 	fSlow95 = ((3 * faustpower<2>(fSlow92)) + -1);
		double 	fSlow96 = ((3 * faustpower<2>(fSlow94)) + -1);
		double 	fSlow97 = (0.7071067811865476 * fSlow71);
		double 	fSlow98 = (fSlow81 + fSlow97);
		double 	fSlow99 = (0.7071067811865476 * fSlow69);
		double 	fSlow100 = (fSlow78 + fSlow99);
		double 	fSlow101 = ((3 * faustpower<2>(fSlow98)) + -1);
		double 	fSlow102 = ((3 * faustpower<2>(fSlow100)) + -1);
		double 	fSlow103 = (fSlow13 * cos((fSlow14 + -0.7853981633974483)));
		double 	fSlow104 = ((3 * faustpower<2>(fSlow103)) + -1);
		double 	fSlow105 = (fSlow29 * cos((fSlow30 + -0.7853981633974483)));
		double 	fSlow106 = ((3 * faustpower<2>(fSlow105)) + -1);
		double 	fSlow107 = (fSlow29 * cos((fSlow30 + -2.356194490192345)));
		double 	fSlow108 = ((3 * faustpower<2>(fSlow107)) + -1);
		double 	fSlow109 = (fSlow13 * cos((fSlow14 + -2.356194490192345)));
		double 	fSlow110 = ((3 * faustpower<2>(fSlow109)) + -1);
		double 	fSlow111 = (fSlow13 * cos((fSlow14 + -3.9269908169872414)));
		double 	fSlow112 = ((3 * faustpower<2>(fSlow111)) + -1);
		double 	fSlow113 = (fSlow29 * cos((fSlow30 + -3.9269908169872414)));
		double 	fSlow114 = ((3 * faustpower<2>(fSlow113)) + -1);
		double 	fSlow115 = (fSlow13 * cos((fSlow14 + -5.497787143782138)));
		double 	fSlow116 = ((3 * faustpower<2>(fSlow115)) + -1);
		double 	fSlow117 = (fSlow29 * cos((fSlow30 + -5.497787143782138)));
		double 	fSlow118 = ((3 * faustpower<2>(fSlow117)) + -1);
		double 	fSlow119 = (fSlow77 - fSlow78);
		double 	fSlow120 = (fSlow80 - fSlow81);
		double 	fSlow121 = ((3 * faustpower<2>(fSlow119)) + -1);
		double 	fSlow122 = ((3 * faustpower<2>(fSlow120)) + -1);
		double 	fSlow123 = (fSlow85 - fSlow81);
		double 	fSlow124 = ((3 * faustpower<2>(fSlow123)) + -1);
		double 	fSlow125 = (fSlow88 - fSlow78);
		double 	fSlow126 = ((3 * faustpower<2>(fSlow125)) + -1);
		double 	fSlow127 = (fSlow91 - fSlow78);
		double 	fSlow128 = ((3 * faustpower<2>(fSlow127)) + -1);
		double 	fSlow129 = (fSlow93 - fSlow81);
		double 	fSlow130 = ((3 * faustpower<2>(fSlow129)) + -1);
		double 	fSlow131 = (fSlow99 - fSlow78);
		double 	fSlow132 = ((3 * faustpower<2>(fSlow131)) + -1);
		double 	fSlow133 = (fSlow97 - fSlow81);
		double 	fSlow134 = ((3 * faustpower<2>(fSlow133)) + -1);
		double 	fSlow135 = (0.8164965809277259 * fSlow105);
		double 	fSlow136 = (0.5773502691896258 * fSlow28);
		double 	fSlow137 = (fSlow135 + fSlow136);
		double 	fSlow138 = ((3 * faustpower<2>(fSlow137)) + -1);
		double 	fSlow139 = (0.8164965809277259 * fSlow103);
		double 	fSlow140 = (0.5773502691896258 * fSlow12);
		double 	fSlow141 = (fSlow139 + fSlow140);
		double 	fSlow142 = ((3 * faustpower<2>(fSlow141)) + -1);
		double 	fSlow143 = (0.8164965809277259 * fSlow109);
		double 	fSlow144 = (fSlow140 + fSlow143);
		double 	fSlow145 = ((3 * faustpower<2>(fSlow144)) + -1);
		double 	fSlow146 = (0.8164965809277259 * fSlow107);
		double 	fSlow147 = (fSlow136 + fSlow146);
		double 	fSlow148 = ((3 * faustpower<2>(fSlow147)) + -1);
		double 	fSlow149 = (0.8164965809277259 * fSlow113);
		double 	fSlow150 = (fSlow136 + fSlow149);
		double 	fSlow151 = (0.8164965809277259 * fSlow111);
		double 	fSlow152 = (fSlow140 + fSlow151);
		double 	fSlow153 = ((3 * faustpower<2>(fSlow150)) + -1);
		double 	fSlow154 = ((3 * faustpower<2>(fSlow152)) + -1);
		double 	fSlow155 = (0.8164965809277259 * fSlow115);
		double 	fSlow156 = (fSlow140 + fSlow155);
		double 	fSlow157 = (0.8164965809277259 * fSlow117);
		double 	fSlow158 = (fSlow136 + fSlow157);
		double 	fSlow159 = ((3 * faustpower<2>(fSlow156)) + -1);
		double 	fSlow160 = ((3 * faustpower<2>(fSlow158)) + -1);
		double 	fSlow161 = (fSlow135 - fSlow136);
		double 	fSlow162 = ((3 * faustpower<2>(fSlow161)) + -1);
		double 	fSlow163 = (fSlow139 - fSlow140);
		double 	fSlow164 = ((3 * faustpower<2>(fSlow163)) + -1);
		double 	fSlow165 = (fSlow143 - fSlow140);
		double 	fSlow166 = ((3 * faustpower<2>(fSlow165)) + -1);
		double 	fSlow167 = (fSlow146 - fSlow136);
		double 	fSlow168 = ((3 * faustpower<2>(fSlow167)) + -1);
		double 	fSlow169 = (fSlow151 - fSlow140);
		double 	fSlow170 = ((3 * faustpower<2>(fSlow169)) + -1);
		double 	fSlow171 = (fSlow149 - fSlow136);
		double 	fSlow172 = ((3 * faustpower<2>(fSlow171)) + -1);
		double 	fSlow173 = (fSlow155 - fSlow140);
		double 	fSlow174 = ((3 * faustpower<2>(fSlow173)) + -1);
		double 	fSlow175 = (fSlow157 - fSlow136);
		double 	fSlow176 = ((3 * faustpower<2>(fSlow175)) + -1);
		double 	fSlow177 = (0.4264014327112203 * (fSlow13 * cos((fSlow14 + -0.7853981633974484))));
		double 	fSlow178 = (0.9045340337332911 * fSlow12);
		double 	fSlow179 = (fSlow177 + fSlow178);
		double 	fSlow180 = (0.4264014327112203 * (fSlow29 * cos((fSlow30 + -0.7853981633974484))));
		double 	fSlow181 = (0.9045340337332911 * fSlow28);
		double 	fSlow182 = (fSlow180 + fSlow181);
		double 	fSlow183 = ((3 * faustpower<2>(fSlow182)) + -1);
		double 	fSlow184 = ((3 * faustpower<2>(fSlow179)) + -1);
		double 	fSlow185 = (0.4264014327112203 * fSlow107);
		double 	fSlow186 = (fSlow181 + fSlow185);
		double 	fSlow187 = ((3 * faustpower<2>(fSlow186)) + -1);
		double 	fSlow188 = (0.4264014327112203 * fSlow109);
		double 	fSlow189 = (fSlow178 + fSlow188);
		double 	fSlow190 = ((3 * faustpower<2>(fSlow189)) + -1);
		double 	fSlow191 = (0.4264014327112203 * fSlow113);
		double 	fSlow192 = (fSlow181 + fSlow191);
		double 	fSlow193 = ((3 * faustpower<2>(fSlow192)) + -1);
		double 	fSlow194 = (0.4264014327112203 * fSlow111);
		double 	fSlow195 = (fSlow178 + fSlow194);
		double 	fSlow196 = ((3 * faustpower<2>(fSlow195)) + -1);
		double 	fSlow197 = (0.4264014327112203 * fSlow115);
		double 	fSlow198 = (fSlow178 + fSlow197);
		double 	fSlow199 = ((3 * faustpower<2>(fSlow198)) + -1);
		double 	fSlow200 = (0.4264014327112203 * fSlow117);
		double 	fSlow201 = (fSlow181 + fSlow200);
		double 	fSlow202 = ((3 * faustpower<2>(fSlow201)) + -1);
		double 	fSlow203 = (0.9534625892455924 * (fSlow29 * cos((fSlow30 + -0.32175055439664263))));
		double 	fSlow204 = (0.3015113445777635 * fSlow28);
		double 	fSlow205 = (fSlow203 + fSlow204);
		double 	fSlow206 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -0.32175055439664263))));
		double 	fSlow207 = (0.3015113445777635 * fSlow12);
		double 	fSlow208 = (fSlow206 + fSlow207);
		double 	fSlow209 = ((3 * faustpower<2>(fSlow205)) + -1);
		double 	fSlow210 = ((3 * faustpower<2>(fSlow208)) + -1);
		double 	fSlow211 = (0.9534625892455924 * (fSlow29 * cos((fSlow30 + -1.2490457723982544))));
		double 	fSlow212 = (fSlow204 + fSlow211);
		double 	fSlow213 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -1.2490457723982544))));
		double 	fSlow214 = (fSlow207 + fSlow213);
		double 	fSlow215 = ((3 * faustpower<2>(fSlow212)) + -1);
		double 	fSlow216 = ((3 * faustpower<2>(fSlow214)) + -1);
		double 	fSlow217 = (0.9534625892455924 * (fSlow29 * cos((fSlow30 + -1.8925468811915387))));
		double 	fSlow218 = (fSlow204 + fSlow217);
		double 	fSlow219 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -1.8925468811915387))));
		double 	fSlow220 = (fSlow207 + fSlow219);
		double 	fSlow221 = ((3 * faustpower<2>(fSlow218)) + -1);
		double 	fSlow222 = ((3 * faustpower<2>(fSlow220)) + -1);
		double 	fSlow223 = (0.9534625892455924 * (fSlow29 * cos((fSlow30 + -2.8198420991931505))));
		double 	fSlow224 = (fSlow204 + fSlow223);
		double 	fSlow225 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -2.8198420991931505))));
		double 	fSlow226 = (fSlow207 + fSlow225);
		double 	fSlow227 = ((3 * faustpower<2>(fSlow224)) + -1);
		double 	fSlow228 = ((3 * faustpower<2>(fSlow226)) + -1);
		double 	fSlow229 = (0.9534625892455924 * (fSlow29 * cos((fSlow30 + -3.4633432079864357))));
		double 	fSlow230 = (fSlow204 + fSlow229);
		double 	fSlow231 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -3.4633432079864357))));
		double 	fSlow232 = (fSlow207 + fSlow231);
		double 	fSlow233 = ((3 * faustpower<2>(fSlow230)) + -1);
		double 	fSlow234 = ((3 * faustpower<2>(fSlow232)) + -1);
		double 	fSlow235 = (0.9534625892455924 * (fSlow29 * cos((fSlow30 + -4.3906384259880475))));
		double 	fSlow236 = (fSlow204 + fSlow235);
		double 	fSlow237 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -4.3906384259880475))));
		double 	fSlow238 = (fSlow207 + fSlow237);
		double 	fSlow239 = ((3 * faustpower<2>(fSlow236)) + -1);
		double 	fSlow240 = ((3 * faustpower<2>(fSlow238)) + -1);
		double 	fSlow241 = (0.9534625892455924 * (fSlow29 * cos((fSlow30 + -5.034139534781332))));
		double 	fSlow242 = (fSlow204 + fSlow241);
		double 	fSlow243 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -5.034139534781332))));
		double 	fSlow244 = (fSlow207 + fSlow243);
		double 	fSlow245 = ((3 * faustpower<2>(fSlow242)) + -1);
		double 	fSlow246 = ((3 * faustpower<2>(fSlow244)) + -1);
		double 	fSlow247 = (0.9534625892455924 * (fSlow29 * cos((fSlow30 + -5.961434752782943))));
		double 	fSlow248 = (fSlow204 + fSlow247);
		double 	fSlow249 = (0.9534625892455924 * (fSlow13 * cos((fSlow14 + -5.961434752782943))));
		double 	fSlow250 = (fSlow207 + fSlow249);
		double 	fSlow251 = ((3 * faustpower<2>(fSlow248)) + -1);
		double 	fSlow252 = ((3 * faustpower<2>(fSlow250)) + -1);
		double 	fSlow253 = (fSlow206 - fSlow207);
		double 	fSlow254 = ((3 * faustpower<2>(fSlow253)) + -1);
		double 	fSlow255 = (fSlow203 - fSlow204);
		double 	fSlow256 = ((3 * faustpower<2>(fSlow255)) + -1);
		double 	fSlow257 = (fSlow213 - fSlow207);
		double 	fSlow258 = ((3 * faustpower<2>(fSlow257)) + -1);
		double 	fSlow259 = (fSlow211 - fSlow204);
		double 	fSlow260 = ((3 * faustpower<2>(fSlow259)) + -1);
		double 	fSlow261 = (fSlow219 - fSlow207);
		double 	fSlow262 = (fSlow217 - fSlow204);
		double 	fSlow263 = ((3 * faustpower<2>(fSlow262)) + -1);
		double 	fSlow264 = ((3 * faustpower<2>(fSlow261)) + -1);
		double 	fSlow265 = (fSlow225 - fSlow207);
		double 	fSlow266 = ((3 * faustpower<2>(fSlow265)) + -1);
		double 	fSlow267 = (fSlow223 - fSlow204);
		double 	fSlow268 = ((3 * faustpower<2>(fSlow267)) + -1);
		double 	fSlow269 = (fSlow231 - fSlow207);
		double 	fSlow270 = ((3 * faustpower<2>(fSlow269)) + -1);
		double 	fSlow271 = (fSlow229 - fSlow204);
		double 	fSlow272 = ((3 * faustpower<2>(fSlow271)) + -1);
		double 	fSlow273 = (fSlow237 - fSlow207);
		double 	fSlow274 = ((3 * faustpower<2>(fSlow273)) + -1);
		double 	fSlow275 = (fSlow235 - fSlow204);
		double 	fSlow276 = ((3 * faustpower<2>(fSlow275)) + -1);
		double 	fSlow277 = (fSlow243 - fSlow207);
		double 	fSlow278 = ((3 * faustpower<2>(fSlow277)) + -1);
		double 	fSlow279 = (fSlow241 - fSlow204);
		double 	fSlow280 = ((3 * faustpower<2>(fSlow279)) + -1);
		double 	fSlow281 = (fSlow249 - fSlow207);
		double 	fSlow282 = ((3 * faustpower<2>(fSlow281)) + -1);
		double 	fSlow283 = (fSlow247 - fSlow204);
		double 	fSlow284 = ((3 * faustpower<2>(fSlow283)) + -1);
		double 	fSlow285 = (fSlow177 - fSlow178);
		double 	fSlow286 = ((3 * faustpower<2>(fSlow285)) + -1);
		double 	fSlow287 = (fSlow180 - fSlow181);
		double 	fSlow288 = ((3 * faustpower<2>(fSlow287)) + -1);
		double 	fSlow289 = (fSlow188 - fSlow178);
		double 	fSlow290 = ((3 * faustpower<2>(fSlow289)) + -1);
		double 	fSlow291 = (fSlow185 - fSlow181);
		double 	fSlow292 = ((3 * faustpower<2>(fSlow291)) + -1);
		double 	fSlow293 = (fSlow194 - fSlow178);
		double 	fSlow294 = ((3 * faustpower<2>(fSlow293)) + -1);
		double 	fSlow295 = (fSlow191 - fSlow181);
		double 	fSlow296 = ((3 * faustpower<2>(fSlow295)) + -1);
		double 	fSlow297 = (fSlow197 - fSlow178);
		double 	fSlow298 = ((3 * faustpower<2>(fSlow297)) + -1);
		double 	fSlow299 = (fSlow200 - fSlow181);
		double 	fSlow300 = ((3 * faustpower<2>(fSlow299)) + -1);
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
			double fTemp0 = ((double)input0[i] * fRec1[0]);
			fRec2[0] = (fSlow9 + (0.999 * fRec2[1]));
			double fTemp1 = ((double)input1[i] * fRec2[0]);
			double fTemp2 = (fSlow1 * ((fSlow4 * fTemp0) + (fSlow8 * fTemp1)));
			fRec5[0] = (fRec4[1] + fRec5[1]);
			fRec4[0] = (fSlow22 * ((fSlow24 * fTemp0) - (fSlow25 * fRec5[0])));
			fRec3[0] = (fRec4[0] + fRec3[1]);
			double fTemp3 = (fSlow26 * fTemp0);
			fRec7[0] = (fRec6[1] + fRec7[1]);
			fRec6[0] = (fSlow22 * (fTemp3 - (fSlow25 * fRec7[0])));
			double fTemp4 = ((fSlow20 * (fRec3[0] - fRec4[0])) + (fRec4[0] + fRec6[0]));
			fRec10[0] = (fRec9[1] + fRec10[1]);
			fRec9[0] = (fSlow22 * ((fSlow38 * fTemp1) - (fSlow25 * fRec10[0])));
			fRec8[0] = (fRec9[0] + fRec8[1]);
			double fTemp5 = (fSlow39 * fTemp1);
			fRec12[0] = (fRec11[1] + fRec12[1]);
			fRec11[0] = (fSlow22 * (fTemp5 - (fSlow25 * fRec12[0])));
			double fTemp6 = ((fSlow36 * (fRec8[0] - fRec9[0])) + (fRec9[0] + fRec11[0]));
			fRec14[0] = (fRec13[1] + fRec14[1]);
			fRec15[0] = (fRec14[1] + fRec15[1]);
			fRec13[0] = (0 - (fSlow43 * (((fSlow45 * fRec14[0]) + (fSlow44 * fRec15[0])) - (fSlow48 * fTemp1))));
			fRec16[0] = (fRec13[0] + fRec16[1]);
			double fTemp7 = (fRec16[0] - fRec13[0]);
			fVec0[0] = fTemp7;
			fRec17[0] = (fRec17[1] + fVec0[1]);
			fRec19[0] = (fRec18[1] + fRec19[1]);
			fRec20[0] = (fRec19[1] + fRec20[1]);
			fRec18[0] = (0 - (fSlow43 * (((fSlow45 * fRec19[0]) + (fSlow44 * fRec20[0])) - fTemp5)));
			double fTemp8 = ((fRec13[0] + (fSlow49 * ((fSlow51 * fVec0[0]) + (fSlow50 * fRec17[0])))) + fRec18[0]);
			fRec22[0] = (fRec21[1] + fRec22[1]);
			fRec23[0] = (fRec22[1] + fRec23[1]);
			fRec21[0] = (fSlow43 * ((fSlow55 * fTemp0) - ((fSlow45 * fRec22[0]) + (fSlow44 * fRec23[0]))));
			fRec24[0] = (fRec21[0] + fRec24[1]);
			double fTemp9 = (fRec24[0] - fRec21[0]);
			fVec1[0] = fTemp9;
			fRec25[0] = (fRec25[1] + fVec1[1]);
			fRec27[0] = (fRec26[1] + fRec27[1]);
			fRec28[0] = (fRec27[1] + fRec28[1]);
			fRec26[0] = (0 - (fSlow43 * (((fSlow45 * fRec27[0]) + (fSlow44 * fRec28[0])) - fTemp3)));
			double fTemp10 = ((fRec21[0] + (fSlow56 * ((fSlow58 * fVec1[0]) + (fSlow57 * fRec25[0])))) + fRec26[0]);
			double fTemp11 = (0.012698412698412698 * ((fTemp2 + (fSlow10 * ((fSlow17 * fTemp4) + (fSlow33 * fTemp6)))) + (fSlow40 * ((fSlow41 * fTemp8) + (fSlow52 * fTemp10)))));
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fbargraph0 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp11;
			double fTemp12 = (0.012698412698412698 * ((fTemp2 + (fSlow40 * ((fSlow59 * fTemp8) + (fSlow60 * fTemp10)))) + (fSlow10 * ((fSlow15 * fTemp4) + (fSlow31 * fTemp6)))));
			fRec29[0] = max((fRec29[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fbargraph1 = fRec29[0];
			output1[i] = (FAUSTFLOAT)fTemp12;
			double fTemp13 = (0.012698412698412698 * ((fSlow10 * ((fSlow61 * fTemp6) + (fSlow62 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow63 * fTemp8) + (fSlow64 * fTemp10))))));
			fRec30[0] = max((fRec30[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fbargraph2 = fRec30[0];
			output2[i] = (FAUSTFLOAT)fTemp13;
			double fTemp14 = (0.012698412698412698 * ((fTemp2 + (fSlow40 * ((fSlow66 * fTemp10) + (fSlow68 * fTemp8)))) + (fSlow10 * ((fSlow65 * fTemp4) + (fSlow67 * fTemp6)))));
			fRec31[0] = max((fRec31[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fbargraph3 = fRec31[0];
			output3[i] = (FAUSTFLOAT)fTemp14;
			double fTemp15 = (0.012698412698412698 * ((fTemp2 + (fSlow40 * ((fSlow70 * fTemp10) + (fSlow72 * fTemp8)))) + (fSlow10 * ((fSlow71 * fTemp6) + (fSlow69 * fTemp4)))));
			fRec32[0] = max((fRec32[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fbargraph4 = fRec32[0];
			output4[i] = (FAUSTFLOAT)fTemp15;
			double fTemp16 = (0.012698412698412698 * ((fSlow10 * ((fSlow73 * fTemp4) + (fSlow74 * fTemp6))) + (fTemp2 + (fSlow40 * ((fSlow75 * fTemp10) + (fSlow76 * fTemp8))))));
			fRec33[0] = max((fRec33[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fbargraph5 = fRec33[0];
			output5[i] = (FAUSTFLOAT)fTemp16;
			double fTemp17 = (0.022574955908289243 * ((fTemp2 + (fSlow10 * ((fSlow79 * fTemp4) + (fSlow82 * fTemp6)))) + (fSlow40 * ((fSlow83 * fTemp10) + (fSlow84 * fTemp8)))));
			fRec34[0] = max((fRec34[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fbargraph6 = fRec34[0];
			output6[i] = (FAUSTFLOAT)fTemp17;
			double fTemp18 = (0.022574955908289243 * ((fTemp2 + (fSlow40 * ((fSlow87 * fTemp8) + (fSlow90 * fTemp10)))) + (fSlow10 * ((fSlow86 * fTemp6) + (fSlow89 * fTemp4)))));
			fRec35[0] = max((fRec35[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fbargraph7 = fRec35[0];
			output7[i] = (FAUSTFLOAT)fTemp18;
			double fTemp19 = (0.022574955908289243 * ((fSlow10 * ((fSlow92 * fTemp4) + (fSlow94 * fTemp6))) + (fTemp2 + (fSlow40 * ((fSlow95 * fTemp10) + (fSlow96 * fTemp8))))));
			fRec36[0] = max((fRec36[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fbargraph8 = fRec36[0];
			output8[i] = (FAUSTFLOAT)fTemp19;
			double fTemp20 = (0.022574955908289243 * ((fSlow10 * ((fSlow98 * fTemp6) + (fSlow100 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow101 * fTemp8) + (fSlow102 * fTemp10))))));
			fRec37[0] = max((fRec37[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph9 = fRec37[0];
			output9[i] = (FAUSTFLOAT)fTemp20;
			double fTemp21 = (0.022574955908289243 * ((fTemp2 + (fSlow40 * ((fSlow104 * fTemp10) + (fSlow106 * fTemp8)))) + (fSlow10 * ((fSlow105 * fTemp6) + (fSlow103 * fTemp4)))));
			fRec38[0] = max((fRec38[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fbargraph10 = fRec38[0];
			output10[i] = (FAUSTFLOAT)fTemp21;
			double fTemp22 = (0.022574955908289243 * ((fTemp2 + (fSlow40 * ((fSlow108 * fTemp8) + (fSlow110 * fTemp10)))) + (fSlow10 * ((fSlow107 * fTemp6) + (fSlow109 * fTemp4)))));
			fRec39[0] = max((fRec39[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fbargraph11 = fRec39[0];
			output11[i] = (FAUSTFLOAT)fTemp22;
			double fTemp23 = (0.022574955908289243 * ((fTemp2 + (fSlow40 * ((fSlow112 * fTemp10) + (fSlow114 * fTemp8)))) + (fSlow10 * ((fSlow113 * fTemp6) + (fSlow111 * fTemp4)))));
			fRec40[0] = max((fRec40[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fbargraph12 = fRec40[0];
			output12[i] = (FAUSTFLOAT)fTemp23;
			double fTemp24 = (0.022574955908289243 * ((fTemp2 + (fSlow40 * ((fSlow116 * fTemp10) + (fSlow118 * fTemp8)))) + (fSlow10 * ((fSlow115 * fTemp4) + (fSlow117 * fTemp6)))));
			fRec41[0] = max((fRec41[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fbargraph13 = fRec41[0];
			output13[i] = (FAUSTFLOAT)fTemp24;
			double fTemp25 = (0.022574955908289243 * ((fSlow10 * ((fSlow119 * fTemp4) + (fSlow120 * fTemp6))) + (fTemp2 + (fSlow40 * ((fSlow121 * fTemp10) + (fSlow122 * fTemp8))))));
			fRec42[0] = max((fRec42[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fbargraph14 = fRec42[0];
			output14[i] = (FAUSTFLOAT)fTemp25;
			double fTemp26 = (0.022574955908289243 * ((fTemp2 + (fSlow40 * ((fSlow124 * fTemp8) + (fSlow126 * fTemp10)))) + (fSlow10 * ((fSlow123 * fTemp6) + (fSlow125 * fTemp4)))));
			fRec43[0] = max((fRec43[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fbargraph15 = fRec43[0];
			output15[i] = (FAUSTFLOAT)fTemp26;
			double fTemp27 = (0.022574955908289243 * ((fSlow40 * ((fSlow128 * fTemp10) + (fSlow130 * fTemp8))) + (fTemp2 + (fSlow10 * ((fSlow127 * fTemp4) + (fSlow129 * fTemp6))))));
			fRec44[0] = max((fRec44[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fbargraph16 = fRec44[0];
			output16[i] = (FAUSTFLOAT)fTemp27;
			double fTemp28 = (0.022574955908289243 * ((fSlow40 * ((fSlow132 * fTemp10) + (fSlow134 * fTemp8))) + (fTemp2 + (fSlow10 * ((fSlow131 * fTemp4) + (fSlow133 * fTemp6))))));
			fRec45[0] = max((fRec45[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph17 = fRec45[0];
			output17[i] = (FAUSTFLOAT)fTemp28;
			double fTemp29 = (0.02109375 * ((fTemp2 + (fSlow40 * ((fSlow138 * fTemp8) + (fSlow142 * fTemp10)))) + (fSlow10 * ((fSlow141 * fTemp4) + (fSlow137 * fTemp6)))));
			fRec46[0] = max((fRec46[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph18 = fRec46[0];
			output18[i] = (FAUSTFLOAT)fTemp29;
			double fTemp30 = (0.02109375 * ((fTemp2 + (fSlow40 * ((fSlow145 * fTemp10) + (fSlow148 * fTemp8)))) + (fSlow10 * ((fSlow147 * fTemp6) + (fSlow144 * fTemp4)))));
			fRec47[0] = max((fRec47[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fbargraph19 = fRec47[0];
			output19[i] = (FAUSTFLOAT)fTemp30;
			double fTemp31 = (0.02109375 * ((fSlow10 * ((fSlow150 * fTemp6) + (fSlow152 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow153 * fTemp8) + (fSlow154 * fTemp10))))));
			fRec48[0] = max((fRec48[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fbargraph20 = fRec48[0];
			output20[i] = (FAUSTFLOAT)fTemp31;
			double fTemp32 = (0.02109375 * ((fSlow10 * ((fSlow156 * fTemp4) + (fSlow158 * fTemp6))) + (fTemp2 + (fSlow40 * ((fSlow159 * fTemp10) + (fSlow160 * fTemp8))))));
			fRec49[0] = max((fRec49[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph21 = fRec49[0];
			output21[i] = (FAUSTFLOAT)fTemp32;
			double fTemp33 = (0.02109375 * ((fTemp2 + (fSlow40 * ((fSlow162 * fTemp8) + (fSlow164 * fTemp10)))) + (fSlow10 * ((fSlow161 * fTemp6) + (fSlow163 * fTemp4)))));
			fRec50[0] = max((fRec50[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph22 = fRec50[0];
			output22[i] = (FAUSTFLOAT)fTemp33;
			double fTemp34 = (0.02109375 * ((fSlow40 * ((fSlow166 * fTemp10) + (fSlow168 * fTemp8))) + (fTemp2 + (fSlow10 * ((fSlow165 * fTemp4) + (fSlow167 * fTemp6))))));
			fRec51[0] = max((fRec51[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fbargraph23 = fRec51[0];
			output23[i] = (FAUSTFLOAT)fTemp34;
			double fTemp35 = (0.02109375 * ((fSlow40 * ((fSlow170 * fTemp10) + (fSlow172 * fTemp8))) + (fTemp2 + (fSlow10 * ((fSlow169 * fTemp4) + (fSlow171 * fTemp6))))));
			fRec52[0] = max((fRec52[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fbargraph24 = fRec52[0];
			output24[i] = (FAUSTFLOAT)fTemp35;
			double fTemp36 = (0.02109375 * ((fSlow40 * ((fSlow174 * fTemp10) + (fSlow176 * fTemp8))) + (fTemp2 + (fSlow10 * ((fSlow173 * fTemp4) + (fSlow175 * fTemp6))))));
			fRec53[0] = max((fRec53[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph25 = fRec53[0];
			output25[i] = (FAUSTFLOAT)fTemp36;
			double fTemp37 = (0.02017333553791887 * ((fTemp2 + (fSlow10 * ((fSlow179 * fTemp4) + (fSlow182 * fTemp6)))) + (fSlow40 * ((fSlow183 * fTemp8) + (fSlow184 * fTemp10)))));
			fRec54[0] = max((fRec54[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fbargraph26 = fRec54[0];
			output26[i] = (FAUSTFLOAT)fTemp37;
			double fTemp38 = (0.02017333553791887 * ((fTemp2 + (fSlow40 * ((fSlow187 * fTemp8) + (fSlow190 * fTemp10)))) + (fSlow10 * ((fSlow189 * fTemp4) + (fSlow186 * fTemp6)))));
			fRec55[0] = max((fRec55[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fbargraph27 = fRec55[0];
			output27[i] = (FAUSTFLOAT)fTemp38;
			double fTemp39 = (0.02017333553791887 * ((fTemp2 + (fSlow40 * ((fSlow193 * fTemp8) + (fSlow196 * fTemp10)))) + (fSlow10 * ((fSlow195 * fTemp4) + (fSlow192 * fTemp6)))));
			fRec56[0] = max((fRec56[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fbargraph28 = fRec56[0];
			output28[i] = (FAUSTFLOAT)fTemp39;
			double fTemp40 = (0.02017333553791887 * ((fTemp2 + (fSlow40 * ((fSlow199 * fTemp10) + (fSlow202 * fTemp8)))) + (fSlow10 * ((fSlow201 * fTemp6) + (fSlow198 * fTemp4)))));
			fRec57[0] = max((fRec57[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fbargraph29 = fRec57[0];
			output29[i] = (FAUSTFLOAT)fTemp40;
			double fTemp41 = (0.02017333553791887 * ((fSlow10 * ((fSlow205 * fTemp6) + (fSlow208 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow209 * fTemp8) + (fSlow210 * fTemp10))))));
			fRec58[0] = max((fRec58[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fbargraph30 = fRec58[0];
			output30[i] = (FAUSTFLOAT)fTemp41;
			double fTemp42 = (0.02017333553791887 * ((fSlow10 * ((fSlow212 * fTemp6) + (fSlow214 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow215 * fTemp8) + (fSlow216 * fTemp10))))));
			fRec59[0] = max((fRec59[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fbargraph31 = fRec59[0];
			output31[i] = (FAUSTFLOAT)fTemp42;
			double fTemp43 = (0.02017333553791887 * ((fSlow10 * ((fSlow218 * fTemp6) + (fSlow220 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow221 * fTemp8) + (fSlow222 * fTemp10))))));
			fRec60[0] = max((fRec60[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fbargraph32 = fRec60[0];
			output32[i] = (FAUSTFLOAT)fTemp43;
			double fTemp44 = (0.02017333553791887 * ((fSlow10 * ((fSlow224 * fTemp6) + (fSlow226 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow227 * fTemp8) + (fSlow228 * fTemp10))))));
			fRec61[0] = max((fRec61[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fbargraph33 = fRec61[0];
			output33[i] = (FAUSTFLOAT)fTemp44;
			double fTemp45 = (0.02017333553791887 * ((fSlow10 * ((fSlow230 * fTemp6) + (fSlow232 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow233 * fTemp8) + (fSlow234 * fTemp10))))));
			fRec62[0] = max((fRec62[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fbargraph34 = fRec62[0];
			output34[i] = (FAUSTFLOAT)fTemp45;
			double fTemp46 = (0.02017333553791887 * ((fSlow10 * ((fSlow236 * fTemp6) + (fSlow238 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow239 * fTemp8) + (fSlow240 * fTemp10))))));
			fRec63[0] = max((fRec63[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fbargraph35 = fRec63[0];
			output35[i] = (FAUSTFLOAT)fTemp46;
			double fTemp47 = (0.02017333553791887 * ((fSlow10 * ((fSlow242 * fTemp6) + (fSlow244 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow245 * fTemp8) + (fSlow246 * fTemp10))))));
			fRec64[0] = max((fRec64[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fbargraph36 = fRec64[0];
			output36[i] = (FAUSTFLOAT)fTemp47;
			double fTemp48 = (0.02017333553791887 * ((fSlow10 * ((fSlow248 * fTemp6) + (fSlow250 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow251 * fTemp8) + (fSlow252 * fTemp10))))));
			fRec65[0] = max((fRec65[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fbargraph37 = fRec65[0];
			output37[i] = (FAUSTFLOAT)fTemp48;
			double fTemp49 = (0.02017333553791887 * ((fSlow40 * ((fSlow254 * fTemp10) + (fSlow256 * fTemp8))) + (fTemp2 + (fSlow10 * ((fSlow253 * fTemp4) + (fSlow255 * fTemp6))))));
			fRec66[0] = max((fRec66[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fbargraph38 = fRec66[0];
			output38[i] = (FAUSTFLOAT)fTemp49;
			double fTemp50 = (0.02017333553791887 * (((fSlow40 * ((fSlow258 * fTemp10) + (fSlow260 * fTemp8))) + (fSlow10 * ((fSlow257 * fTemp4) + (fSlow259 * fTemp6)))) + fTemp2));
			fRec67[0] = max((fRec67[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fbargraph39 = fRec67[0];
			output39[i] = (FAUSTFLOAT)fTemp50;
			double fTemp51 = (0.02017333553791887 * (((fSlow10 * ((fSlow261 * fTemp4) + (fSlow262 * fTemp6))) + (fSlow40 * ((fSlow263 * fTemp8) + (fSlow264 * fTemp10)))) + fTemp2));
			fRec68[0] = max((fRec68[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fbargraph40 = fRec68[0];
			output40[i] = (FAUSTFLOAT)fTemp51;
			double fTemp52 = (0.02017333553791887 * (((fSlow40 * ((fSlow266 * fTemp10) + (fSlow268 * fTemp8))) + (fSlow10 * ((fSlow265 * fTemp4) + (fSlow267 * fTemp6)))) + fTemp2));
			fRec69[0] = max((fRec69[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fbargraph41 = fRec69[0];
			output41[i] = (FAUSTFLOAT)fTemp52;
			double fTemp53 = (0.02017333553791887 * (((fSlow40 * ((fSlow270 * fTemp10) + (fSlow272 * fTemp8))) + (fSlow10 * ((fSlow269 * fTemp4) + (fSlow271 * fTemp6)))) + fTemp2));
			fRec70[0] = max((fRec70[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fbargraph42 = fRec70[0];
			output42[i] = (FAUSTFLOAT)fTemp53;
			double fTemp54 = (0.02017333553791887 * (((fSlow40 * ((fSlow274 * fTemp10) + (fSlow276 * fTemp8))) + (fSlow10 * ((fSlow273 * fTemp4) + (fSlow275 * fTemp6)))) + fTemp2));
			fRec71[0] = max((fRec71[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fbargraph43 = fRec71[0];
			output43[i] = (FAUSTFLOAT)fTemp54;
			double fTemp55 = (0.02017333553791887 * (((fSlow40 * ((fSlow278 * fTemp10) + (fSlow280 * fTemp8))) + (fSlow10 * ((fSlow277 * fTemp4) + (fSlow279 * fTemp6)))) + fTemp2));
			fRec72[0] = max((fRec72[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fbargraph44 = fRec72[0];
			output44[i] = (FAUSTFLOAT)fTemp55;
			double fTemp56 = (0.02017333553791887 * (((fSlow40 * ((fSlow282 * fTemp10) + (fSlow284 * fTemp8))) + (fSlow10 * ((fSlow281 * fTemp4) + (fSlow283 * fTemp6)))) + fTemp2));
			fRec73[0] = max((fRec73[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fbargraph45 = fRec73[0];
			output45[i] = (FAUSTFLOAT)fTemp56;
			double fTemp57 = (0.02017333553791887 * (((fSlow40 * ((fSlow286 * fTemp10) + (fSlow288 * fTemp8))) + (fSlow10 * ((fSlow285 * fTemp4) + (fSlow287 * fTemp6)))) + fTemp2));
			fRec74[0] = max((fRec74[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fbargraph46 = fRec74[0];
			output46[i] = (FAUSTFLOAT)fTemp57;
			double fTemp58 = (0.02017333553791887 * (((fSlow40 * ((fSlow290 * fTemp10) + (fSlow292 * fTemp8))) + (fSlow10 * ((fSlow289 * fTemp4) + (fSlow291 * fTemp6)))) + fTemp2));
			fRec75[0] = max((fRec75[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fbargraph47 = fRec75[0];
			output47[i] = (FAUSTFLOAT)fTemp58;
			double fTemp59 = (0.02017333553791887 * (((fSlow40 * ((fSlow294 * fTemp10) + (fSlow296 * fTemp8))) + (fSlow10 * ((fSlow293 * fTemp4) + (fSlow295 * fTemp6)))) + fTemp2));
			fRec76[0] = max((fRec76[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fbargraph48 = fRec76[0];
			output48[i] = (FAUSTFLOAT)fTemp59;
			double fTemp60 = (0.02017333553791887 * (((fSlow40 * ((fSlow298 * fTemp10) + (fSlow300 * fTemp8))) + (fSlow10 * ((fSlow297 * fTemp4) + (fSlow299 * fTemp6)))) + fTemp2));
			fRec77[0] = max((fRec77[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fbargraph49 = fRec77[0];
			output49[i] = (FAUSTFLOAT)fTemp60;
			// post processing
			fRec77[1] = fRec77[0];
			fRec76[1] = fRec76[0];
			fRec75[1] = fRec75[0];
			fRec74[1] = fRec74[0];
			fRec73[1] = fRec73[0];
			fRec72[1] = fRec72[0];
			fRec71[1] = fRec71[0];
			fRec70[1] = fRec70[0];
			fRec69[1] = fRec69[0];
			fRec68[1] = fRec68[0];
			fRec67[1] = fRec67[0];
			fRec66[1] = fRec66[0];
			fRec65[1] = fRec65[0];
			fRec64[1] = fRec64[0];
			fRec63[1] = fRec63[0];
			fRec62[1] = fRec62[0];
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
			fRec0[1] = fRec0[0];
			fRec26[1] = fRec26[0];
			fRec28[1] = fRec28[0];
			fRec27[1] = fRec27[0];
			fRec25[1] = fRec25[0];
			fVec1[1] = fVec1[0];
			fRec24[1] = fRec24[0];
			fRec21[1] = fRec21[0];
			fRec23[1] = fRec23[0];
			fRec22[1] = fRec22[0];
			fRec18[1] = fRec18[0];
			fRec20[1] = fRec20[0];
			fRec19[1] = fRec19[0];
			fRec17[1] = fRec17[0];
			fVec0[1] = fVec0[0];
			fRec16[1] = fRec16[0];
			fRec13[1] = fRec13[0];
			fRec15[1] = fRec15[0];
			fRec14[1] = fRec14[0];
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
