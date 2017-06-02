//----------------------------------------------------------
// name: "HOAPanLebedev503"
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
	FAUSTFLOAT 	fcheckbox1;
	FAUSTFLOAT 	fslider2;
	FAUSTFLOAT 	fslider3;
	double 	fRec2[2];
	double 	fConst4;
	double 	fRec3[2];
	double 	fRec4[2];
	double 	fRec1[2];
	double 	fRec5[2];
	double 	fVec0[2];
	double 	fRec6[2];
	double 	fRec8[2];
	double 	fRec9[2];
	double 	fRec7[2];
	FAUSTFLOAT 	fslider4;
	FAUSTFLOAT 	fslider5;
	FAUSTFLOAT 	fcheckbox2;
	FAUSTFLOAT 	fslider6;
	FAUSTFLOAT 	fslider7;
	double 	fRec11[2];
	double 	fRec12[2];
	double 	fRec13[2];
	double 	fRec10[2];
	double 	fRec14[2];
	double 	fVec1[2];
	double 	fRec15[2];
	double 	fRec17[2];
	double 	fRec18[2];
	double 	fRec16[2];
	FAUSTFLOAT 	fcheckbox3;
	double 	fConst5;
	double 	fConst6;
	double 	fConst7;
	double 	fRec23[2];
	double 	fRec22[2];
	double 	fConst8;
	double 	fRec21[2];
	double 	fRec24[2];
	double 	fVec2[2];
	double 	fRec25[2];
	double 	fRec26[2];
	double 	fRec20[2];
	double 	fRec19[2];
	double 	fRec30[2];
	double 	fRec29[2];
	double 	fRec28[2];
	double 	fRec31[2];
	double 	fRec27[2];
	FAUSTFLOAT 	fcheckbox4;
	double 	fConst9;
	double 	fRec34[2];
	double 	fRec33[2];
	double 	fRec32[2];
	double 	fRec36[2];
	double 	fRec35[2];
	double 	fRec39[2];
	double 	fRec38[2];
	double 	fRec37[2];
	double 	fRec41[2];
	double 	fRec40[2];
	double 	fRec46[2];
	double 	fRec45[2];
	double 	fRec44[2];
	double 	fRec47[2];
	double 	fVec3[2];
	double 	fRec48[2];
	double 	fRec49[2];
	double 	fRec43[2];
	double 	fRec42[2];
	double 	fRec53[2];
	double 	fRec52[2];
	double 	fRec51[2];
	double 	fRec54[2];
	double 	fRec50[2];
	FAUSTFLOAT 	fcheckbox5;
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fRec55[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec56[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec57[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fRec58[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec59[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec60[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec61[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec62[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec63[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec64[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec65[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec66[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec67[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec68[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec69[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec70[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec71[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec72[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec73[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec74[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec75[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec76[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec77[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec78[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec79[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec80[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec81[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec82[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec83[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec84[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec85[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec86[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec87[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec88[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec89[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec90[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec91[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec92[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec93[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec94[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec95[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec96[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec97[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec98[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec99[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec100[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec101[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec102[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec103[2];
	FAUSTFLOAT 	fbargraph49;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOAPanLebedev503");
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
		fConst2 = (5.1e+02 / fConst0);
		fConst3 = (1.7e+02 / fConst0);
		fConst4 = (1.02e+03 / fConst0);
		fConst5 = (789.5430205728705 / fConst0);
		fConst6 = (394.7715102864353 / fConst0);
		fConst7 = (625.2284897135664 / fConst0);
		fConst8 = (1250.4569794271329 / fConst0);
		fConst9 = (3.4e+02 / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fcheckbox0 = 0.0;
		fslider0 = 0.0;
		fslider1 = 0.0;
		fentry0 = 1.07;
		fcheckbox1 = 0.0;
		fslider2 = 2.0;
		fslider3 = 0.0;
		fslider4 = 0.0;
		fslider5 = 0.0;
		fcheckbox2 = 0.0;
		fslider6 = 2.0;
		fslider7 = 0.0;
		fcheckbox3 = 0.0;
		fcheckbox4 = 0.0;
		fcheckbox5 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fVec0[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
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
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
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
		ui_interface->declare(&fslider7, "0+1", "");
		ui_interface->declare(&fslider7, "osc", "/gain_0 -20 20");
		ui_interface->declare(&fslider7, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  0", &fslider7, 0.0, -3e+01, 2e+01, 0.1);
		ui_interface->declare(&fslider6, "0+2", "");
		ui_interface->declare(&fslider6, "osc", "/radius_0 0.5 50");
		ui_interface->declare(&fslider6, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  0", &fslider6, 2.0, 0.5, 5e+01, 0.01);
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
		ui_interface->declare(&fslider3, "1+1", "");
		ui_interface->declare(&fslider3, "osc", "/gain_1 -20 20");
		ui_interface->declare(&fslider3, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  1", &fslider3, 0.0, -3e+01, 2e+01, 0.1);
		ui_interface->declare(&fslider2, "1+2", "");
		ui_interface->declare(&fslider2, "osc", "/radius_1 0.5 50");
		ui_interface->declare(&fslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  1", &fslider2, 2.0, 0.5, 5e+01, 0.01);
		ui_interface->declare(&fslider1, "1+3", "");
		ui_interface->declare(&fslider1, "osc", "/azimuth_1 0 360");
		ui_interface->declare(&fslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth  1", &fslider1, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(&fslider0, "1+4", "");
		ui_interface->declare(&fslider0, "osc", "/elevation_1 -90 90");
		ui_interface->declare(&fslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation  1", &fslider0, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(0, "1+5", "");
		ui_interface->openHorizontalBox("Spherical Wave");
		ui_interface->addCheckButton("Yes", &fcheckbox1);
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openVerticalBox("Mute Order");
		ui_interface->addCheckButton("0", &fcheckbox5);
		ui_interface->addCheckButton("1", &fcheckbox4);
		ui_interface->addCheckButton("2", &fcheckbox0);
		ui_interface->addCheckButton("3", &fcheckbox3);
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
		ui_interface->addVerticalBargraph("0x7f83d6637360", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph1, "osc", "/output2");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d663a5f0", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph2, "osc", "/output3");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d663de10", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph3, "osc", "/output4");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d6641690", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph4, "osc", "/output5");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d6645010", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph5, "osc", "/output6");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66483d0", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph6, "osc", "/output7");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d664bf60", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph7, "osc", "/output8");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d664f670", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph8, "osc", "/output9");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d6652e10", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph9, "osc", "/output10");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d6656640", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph10, "osc", "/output11");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d665a250", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph11, "osc", "/output12");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d665de10", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph12, "osc", "/output13");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d6661c50", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph13, "osc", "/output14");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66659c0", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph14, "osc", "/output15");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d6669300", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph15, "osc", "/output16");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d666cbf0", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph16, "osc", "/output17");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d6670550", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph17, "osc", "/output18");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d6673f20", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph18, "osc", "/output19");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66780c0", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph19, "osc", "/output20");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d667bdd0", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph20, "osc", "/output21");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d667fb90", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph21, "osc", "/output22");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66839a0", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph22, "osc", "/output23");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d6687590", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph23, "osc", "/output24");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d668b200", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph24, "osc", "/output25");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d668eed0", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph25, "osc", "/output26");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d6692c10", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph26, "osc", "/output27");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d6696d20", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph27, "osc", "/output28");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d669a3a0", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph28, "osc", "/output29");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d669da00", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph29, "osc", "/output30");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66a1170", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph30, "osc", "/output31");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66a5340", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph31, "osc", "/output32");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66a90d0", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph32, "osc", "/output33");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66acf60", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph33, "osc", "/output34");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66b0ec0", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph34, "osc", "/output35");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66b4e30", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fbargraph35, "osc", "/output36");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66b8e00", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fbargraph36, "osc", "/output37");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66bce20", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fbargraph37, "osc", "/output38");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66c0eb0", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fbargraph38, "osc", "/output39");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66c4780", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fbargraph39, "osc", "/output40");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66c8060", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fbargraph40, "osc", "/output41");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66cb980", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fbargraph41, "osc", "/output42");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66cf340", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fbargraph42, "osc", "/output43");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66d2da0", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fbargraph43, "osc", "/output44");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66d6810", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fbargraph44, "osc", "/output45");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66da350", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fbargraph45, "osc", "/output46");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66dded0", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fbargraph46, "osc", "/output47");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66e1ac0", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fbargraph47, "osc", "/output48");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66e5740", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fbargraph48, "osc", "/output49");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66e9430", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fbargraph49, "osc", "/output50");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f83d66ed190", &fbargraph49, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = (2.5 * (1 - double(fcheckbox0)));
		double 	fSlow1 = double(fslider0);
		double 	fSlow2 = sin(fSlow1);
		double 	fSlow3 = cos(fSlow1);
		double 	fSlow4 = double(fslider1);
		double 	fSlow5 = (fSlow3 * cos(fSlow4));
		double 	fSlow6 = (6.123233995736766e-17 * fSlow5);
		double 	fSlow7 = (fSlow2 + fSlow6);
		double 	fSlow8 = faustpower<2>(fSlow7);
		double 	fSlow9 = ((3 * fSlow8) + -1);
		double 	fSlow10 = double(fentry0);
		double 	fSlow11 = (fConst3 / fSlow10);
		double 	fSlow12 = faustpower<2>(fSlow11);
		double 	fSlow13 = (1.0 / (((fConst2 / fSlow10) + (3 * fSlow12)) + 1));
		double 	fSlow14 = double(fcheckbox1);
		double 	fSlow15 = (fSlow10 * fSlow14);
		double 	fSlow16 = double(fslider2);
		double 	fSlow17 = (fConst3 / fSlow16);
		double 	fSlow18 = faustpower<2>(fSlow17);
		double 	fSlow19 = (((fConst2 / fSlow16) + (3 * fSlow18)) + 1);
		double 	fSlow20 = ((fSlow15 * fSlow19) / fSlow16);
		double 	fSlow21 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider3))));
		double 	fSlow22 = (12 * fSlow12);
		double 	fSlow23 = (fSlow22 + (fConst4 / fSlow10));
		double 	fSlow24 = (1.0 / fSlow19);
		double 	fSlow25 = (12 * fSlow18);
		double 	fSlow26 = ((fConst4 / fSlow16) + fSlow25);
		double 	fSlow27 = (fSlow10 * (1 - fSlow14));
		double 	fSlow28 = double(fslider4);
		double 	fSlow29 = sin(fSlow28);
		double 	fSlow30 = cos(fSlow28);
		double 	fSlow31 = double(fslider5);
		double 	fSlow32 = (fSlow30 * cos(fSlow31));
		double 	fSlow33 = (6.123233995736766e-17 * fSlow32);
		double 	fSlow34 = (fSlow29 + fSlow33);
		double 	fSlow35 = faustpower<2>(fSlow34);
		double 	fSlow36 = ((3 * fSlow35) + -1);
		double 	fSlow37 = double(fcheckbox2);
		double 	fSlow38 = (fSlow37 * fSlow10);
		double 	fSlow39 = double(fslider6);
		double 	fSlow40 = (fConst3 / fSlow39);
		double 	fSlow41 = faustpower<2>(fSlow40);
		double 	fSlow42 = (((fConst2 / fSlow39) + (3 * fSlow41)) + 1);
		double 	fSlow43 = ((fSlow38 * fSlow42) / fSlow39);
		double 	fSlow44 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider7))));
		double 	fSlow45 = (1.0 / fSlow42);
		double 	fSlow46 = (12 * fSlow41);
		double 	fSlow47 = ((fConst4 / fSlow39) + fSlow46);
		double 	fSlow48 = ((1 - fSlow37) * fSlow10);
		double 	fSlow49 = (1 - double(fcheckbox3));
		double 	fSlow50 = (3.5 * (fSlow49 * ((5 * fSlow35) + -3)));
		double 	fSlow51 = ((fConst6 / fSlow39) + 1);
		double 	fSlow52 = (fConst5 / (fSlow39 * fSlow51));
		double 	fSlow53 = (1.0 / ((fConst6 / fSlow10) + 1));
		double 	fSlow54 = (1.0 / (((6.45943269348338 * fSlow12) + (fConst7 / fSlow10)) + 1));
		double 	fSlow55 = (((fConst7 / fSlow39) + (6.45943269348338 * fSlow41)) + 1);
		double 	fSlow56 = ((fSlow38 * fSlow55) / fSlow39);
		double 	fSlow57 = (25.83773077393352 * fSlow12);
		double 	fSlow58 = (fSlow57 + (fConst8 / fSlow10));
		double 	fSlow59 = (1.0 / fSlow55);
		double 	fSlow60 = (25.83773077393352 * fSlow41);
		double 	fSlow61 = (fSlow60 + (fConst8 / fSlow39));
		double 	fSlow62 = (fConst5 / fSlow10);
		double 	fSlow63 = (3 * (1 - double(fcheckbox4)));
		double 	fSlow64 = (fSlow40 + 1);
		double 	fSlow65 = (fConst9 / (fSlow39 * fSlow64));
		double 	fSlow66 = (1.0 / (fSlow11 + 1));
		double 	fSlow67 = ((fSlow38 * fSlow64) / fSlow39);
		double 	fSlow68 = (fConst9 / fSlow10);
		double 	fSlow69 = (fSlow17 + 1);
		double 	fSlow70 = (fConst9 / (fSlow16 * fSlow69));
		double 	fSlow71 = ((fSlow15 * fSlow69) / fSlow16);
		double 	fSlow72 = (3.5 * (fSlow49 * ((5 * fSlow8) + -3)));
		double 	fSlow73 = ((fConst6 / fSlow16) + 1);
		double 	fSlow74 = (fConst5 / (fSlow16 * fSlow73));
		double 	fSlow75 = (((fConst7 / fSlow16) + (6.45943269348338 * fSlow18)) + 1);
		double 	fSlow76 = ((fSlow15 * fSlow75) / fSlow16);
		double 	fSlow77 = (1.0 / fSlow75);
		double 	fSlow78 = (25.83773077393352 * fSlow18);
		double 	fSlow79 = ((fConst8 / fSlow16) + fSlow78);
		double 	fSlow80 = (fSlow10 * (1 - double(fcheckbox5)));
		double 	fSlow81 = (1 - (fSlow37 * (1 - (1.0 / fSlow39))));
		double 	fSlow82 = (1 - (fSlow14 * (1 - (1.0 / fSlow16))));
		double 	fSlow83 = (3.5 * fSlow49);
		double 	fSlow84 = ((5 * faustpower<3>(fSlow5)) - (3 * fSlow5));
		double 	fSlow85 = ((5 * faustpower<3>(fSlow32)) - (3 * fSlow32));
		double 	fSlow86 = ((3 * faustpower<2>(fSlow5)) + -1);
		double 	fSlow87 = ((3 * faustpower<2>(fSlow32)) + -1);
		double 	fSlow88 = (fSlow3 * cos((fSlow4 + -1.5707963267948966)));
		double 	fSlow89 = ((5 * faustpower<3>(fSlow88)) - (3 * fSlow88));
		double 	fSlow90 = (fSlow30 * cos((fSlow31 + -1.5707963267948966)));
		double 	fSlow91 = ((5 * faustpower<3>(fSlow90)) - (3 * fSlow90));
		double 	fSlow92 = ((3 * faustpower<2>(fSlow90)) + -1);
		double 	fSlow93 = ((3 * faustpower<2>(fSlow88)) + -1);
		double 	fSlow94 = (fSlow3 * cos((fSlow4 + -3.141592653589793)));
		double 	fSlow95 = ((5 * faustpower<3>(fSlow94)) - (3 * fSlow94));
		double 	fSlow96 = (fSlow30 * cos((fSlow31 + -3.141592653589793)));
		double 	fSlow97 = ((5 * faustpower<3>(fSlow96)) - (3 * fSlow96));
		double 	fSlow98 = ((3 * faustpower<2>(fSlow96)) + -1);
		double 	fSlow99 = ((3 * faustpower<2>(fSlow94)) + -1);
		double 	fSlow100 = (fSlow3 * cos((fSlow4 + -4.71238898038469)));
		double 	fSlow101 = ((5 * faustpower<3>(fSlow100)) - (3 * fSlow100));
		double 	fSlow102 = (fSlow30 * cos((fSlow31 + -4.71238898038469)));
		double 	fSlow103 = ((5 * faustpower<3>(fSlow102)) - (3 * fSlow102));
		double 	fSlow104 = ((3 * faustpower<2>(fSlow102)) + -1);
		double 	fSlow105 = ((3 * faustpower<2>(fSlow100)) + -1);
		double 	fSlow106 = (fSlow6 - fSlow2);
		double 	fSlow107 = faustpower<2>(fSlow106);
		double 	fSlow108 = ((3 * fSlow107) + -1);
		double 	fSlow109 = (fSlow33 - fSlow29);
		double 	fSlow110 = faustpower<2>(fSlow109);
		double 	fSlow111 = ((3 * fSlow110) + -1);
		double 	fSlow112 = (3.5 * (fSlow49 * ((5 * fSlow107) + -3)));
		double 	fSlow113 = (3.5 * (fSlow49 * ((5 * fSlow110) + -3)));
		double 	fSlow114 = (0.7071067811865476 * fSlow5);
		double 	fSlow115 = (0.7071067811865475 * fSlow2);
		double 	fSlow116 = (fSlow114 + fSlow115);
		double 	fSlow117 = faustpower<2>(fSlow116);
		double 	fSlow118 = (3.5 * (fSlow49 * ((5 * fSlow117) + -3)));
		double 	fSlow119 = (0.7071067811865476 * fSlow32);
		double 	fSlow120 = (0.7071067811865475 * fSlow29);
		double 	fSlow121 = (fSlow119 + fSlow120);
		double 	fSlow122 = faustpower<2>(fSlow121);
		double 	fSlow123 = (3.5 * (fSlow49 * ((5 * fSlow122) + -3)));
		double 	fSlow124 = ((3 * fSlow122) + -1);
		double 	fSlow125 = ((3 * fSlow117) + -1);
		double 	fSlow126 = (0.7071067811865476 * fSlow88);
		double 	fSlow127 = (fSlow115 + fSlow126);
		double 	fSlow128 = faustpower<2>(fSlow127);
		double 	fSlow129 = (3.5 * (fSlow49 * ((5 * fSlow128) + -3)));
		double 	fSlow130 = (0.7071067811865476 * fSlow90);
		double 	fSlow131 = (fSlow120 + fSlow130);
		double 	fSlow132 = faustpower<2>(fSlow131);
		double 	fSlow133 = (3.5 * (fSlow49 * ((5 * fSlow132) + -3)));
		double 	fSlow134 = ((3 * fSlow132) + -1);
		double 	fSlow135 = ((3 * fSlow128) + -1);
		double 	fSlow136 = (0.7071067811865476 * fSlow94);
		double 	fSlow137 = (fSlow115 + fSlow136);
		double 	fSlow138 = faustpower<2>(fSlow137);
		double 	fSlow139 = (3.5 * (fSlow49 * ((5 * fSlow138) + -3)));
		double 	fSlow140 = (0.7071067811865476 * fSlow96);
		double 	fSlow141 = (fSlow120 + fSlow140);
		double 	fSlow142 = faustpower<2>(fSlow141);
		double 	fSlow143 = (3.5 * (fSlow49 * ((5 * fSlow142) + -3)));
		double 	fSlow144 = ((3 * fSlow142) + -1);
		double 	fSlow145 = ((3 * fSlow138) + -1);
		double 	fSlow146 = (0.7071067811865476 * fSlow102);
		double 	fSlow147 = (fSlow120 + fSlow146);
		double 	fSlow148 = faustpower<2>(fSlow147);
		double 	fSlow149 = (3.5 * (fSlow49 * ((5 * fSlow148) + -3)));
		double 	fSlow150 = (0.7071067811865476 * fSlow100);
		double 	fSlow151 = (fSlow115 + fSlow150);
		double 	fSlow152 = faustpower<2>(fSlow151);
		double 	fSlow153 = (3.5 * (fSlow49 * ((5 * fSlow152) + -3)));
		double 	fSlow154 = ((3 * fSlow152) + -1);
		double 	fSlow155 = ((3 * fSlow148) + -1);
		double 	fSlow156 = (fSlow3 * cos((fSlow4 + -0.7853981633974483)));
		double 	fSlow157 = ((3 * faustpower<2>(fSlow156)) + -1);
		double 	fSlow158 = (fSlow30 * cos((fSlow31 + -0.7853981633974483)));
		double 	fSlow159 = ((3 * faustpower<2>(fSlow158)) + -1);
		double 	fSlow160 = ((5 * faustpower<3>(fSlow156)) - (3 * fSlow156));
		double 	fSlow161 = ((5 * faustpower<3>(fSlow158)) - (3 * fSlow158));
		double 	fSlow162 = (fSlow30 * cos((fSlow31 + -2.356194490192345)));
		double 	fSlow163 = ((5 * faustpower<3>(fSlow162)) - (3 * fSlow162));
		double 	fSlow164 = (fSlow3 * cos((fSlow4 + -2.356194490192345)));
		double 	fSlow165 = ((5 * faustpower<3>(fSlow164)) - (3 * fSlow164));
		double 	fSlow166 = ((3 * faustpower<2>(fSlow162)) + -1);
		double 	fSlow167 = ((3 * faustpower<2>(fSlow164)) + -1);
		double 	fSlow168 = (fSlow30 * cos((fSlow31 + -3.9269908169872414)));
		double 	fSlow169 = ((3 * faustpower<2>(fSlow168)) + -1);
		double 	fSlow170 = (fSlow3 * cos((fSlow4 + -3.9269908169872414)));
		double 	fSlow171 = ((3 * faustpower<2>(fSlow170)) + -1);
		double 	fSlow172 = ((5 * faustpower<3>(fSlow170)) - (3 * fSlow170));
		double 	fSlow173 = ((5 * faustpower<3>(fSlow168)) - (3 * fSlow168));
		double 	fSlow174 = (fSlow30 * cos((fSlow31 + -5.497787143782138)));
		double 	fSlow175 = ((5 * faustpower<3>(fSlow174)) - (3 * fSlow174));
		double 	fSlow176 = (fSlow3 * cos((fSlow4 + -5.497787143782138)));
		double 	fSlow177 = ((5 * faustpower<3>(fSlow176)) - (3 * fSlow176));
		double 	fSlow178 = ((3 * faustpower<2>(fSlow174)) + -1);
		double 	fSlow179 = ((3 * faustpower<2>(fSlow176)) + -1);
		double 	fSlow180 = (fSlow119 - fSlow120);
		double 	fSlow181 = faustpower<2>(fSlow180);
		double 	fSlow182 = ((3 * fSlow181) + -1);
		double 	fSlow183 = (fSlow114 - fSlow115);
		double 	fSlow184 = faustpower<2>(fSlow183);
		double 	fSlow185 = ((3 * fSlow184) + -1);
		double 	fSlow186 = (3.5 * (fSlow49 * ((5 * fSlow181) + -3)));
		double 	fSlow187 = (3.5 * (fSlow49 * ((5 * fSlow184) + -3)));
		double 	fSlow188 = (fSlow130 - fSlow120);
		double 	fSlow189 = faustpower<2>(fSlow188);
		double 	fSlow190 = ((3 * fSlow189) + -1);
		double 	fSlow191 = (fSlow126 - fSlow115);
		double 	fSlow192 = faustpower<2>(fSlow191);
		double 	fSlow193 = ((3 * fSlow192) + -1);
		double 	fSlow194 = (3.5 * (fSlow49 * ((5 * fSlow189) + -3)));
		double 	fSlow195 = (3.5 * (fSlow49 * ((5 * fSlow192) + -3)));
		double 	fSlow196 = (fSlow140 - fSlow120);
		double 	fSlow197 = faustpower<2>(fSlow196);
		double 	fSlow198 = ((3 * fSlow197) + -1);
		double 	fSlow199 = (fSlow136 - fSlow115);
		double 	fSlow200 = faustpower<2>(fSlow199);
		double 	fSlow201 = ((3 * fSlow200) + -1);
		double 	fSlow202 = (3.5 * (fSlow49 * ((5 * fSlow197) + -3)));
		double 	fSlow203 = (3.5 * (fSlow49 * ((5 * fSlow200) + -3)));
		double 	fSlow204 = (fSlow146 - fSlow120);
		double 	fSlow205 = faustpower<2>(fSlow204);
		double 	fSlow206 = ((3 * fSlow205) + -1);
		double 	fSlow207 = (fSlow150 - fSlow115);
		double 	fSlow208 = faustpower<2>(fSlow207);
		double 	fSlow209 = ((3 * fSlow208) + -1);
		double 	fSlow210 = (3.5 * (fSlow49 * ((5 * fSlow205) + -3)));
		double 	fSlow211 = (3.5 * (fSlow49 * ((5 * fSlow208) + -3)));
		double 	fSlow212 = (0.5773502691896258 * fSlow2);
		double 	fSlow213 = (0.8164965809277259 * fSlow156);
		double 	fSlow214 = (fSlow212 + fSlow213);
		double 	fSlow215 = faustpower<2>(fSlow214);
		double 	fSlow216 = ((3 * fSlow215) + -1);
		double 	fSlow217 = (0.8164965809277259 * fSlow158);
		double 	fSlow218 = (0.5773502691896258 * fSlow29);
		double 	fSlow219 = (fSlow217 + fSlow218);
		double 	fSlow220 = faustpower<2>(fSlow219);
		double 	fSlow221 = ((3 * fSlow220) + -1);
		double 	fSlow222 = (3.5 * (fSlow49 * ((5 * fSlow220) + -3)));
		double 	fSlow223 = (3.5 * (fSlow49 * ((5 * fSlow215) + -3)));
		double 	fSlow224 = (0.8164965809277259 * fSlow162);
		double 	fSlow225 = (fSlow218 + fSlow224);
		double 	fSlow226 = faustpower<2>(fSlow225);
		double 	fSlow227 = (3.5 * (fSlow49 * ((5 * fSlow226) + -3)));
		double 	fSlow228 = (0.8164965809277259 * fSlow164);
		double 	fSlow229 = (fSlow228 + fSlow212);
		double 	fSlow230 = faustpower<2>(fSlow229);
		double 	fSlow231 = (3.5 * (fSlow49 * ((5 * fSlow230) + -3)));
		double 	fSlow232 = ((3 * fSlow226) + -1);
		double 	fSlow233 = ((3 * fSlow230) + -1);
		double 	fSlow234 = (0.8164965809277259 * fSlow168);
		double 	fSlow235 = (fSlow234 + fSlow218);
		double 	fSlow236 = faustpower<2>(fSlow235);
		double 	fSlow237 = ((3 * fSlow236) + -1);
		double 	fSlow238 = (0.8164965809277259 * fSlow170);
		double 	fSlow239 = (fSlow238 + fSlow212);
		double 	fSlow240 = faustpower<2>(fSlow239);
		double 	fSlow241 = ((3 * fSlow240) + -1);
		double 	fSlow242 = (3.5 * (fSlow49 * ((5 * fSlow236) + -3)));
		double 	fSlow243 = (3.5 * (fSlow49 * ((5 * fSlow240) + -3)));
		double 	fSlow244 = (0.8164965809277259 * fSlow174);
		double 	fSlow245 = (fSlow244 + fSlow218);
		double 	fSlow246 = faustpower<2>(fSlow245);
		double 	fSlow247 = (3.5 * (fSlow49 * ((5 * fSlow246) + -3)));
		double 	fSlow248 = (0.8164965809277259 * fSlow176);
		double 	fSlow249 = (fSlow248 + fSlow212);
		double 	fSlow250 = faustpower<2>(fSlow249);
		double 	fSlow251 = (3.5 * (fSlow49 * ((5 * fSlow250) + -3)));
		double 	fSlow252 = ((3 * fSlow246) + -1);
		double 	fSlow253 = ((3 * fSlow250) + -1);
		double 	fSlow254 = (fSlow217 - fSlow218);
		double 	fSlow255 = faustpower<2>(fSlow254);
		double 	fSlow256 = ((3 * fSlow255) + -1);
		double 	fSlow257 = (fSlow213 - fSlow212);
		double 	fSlow258 = faustpower<2>(fSlow257);
		double 	fSlow259 = ((3 * fSlow258) + -1);
		double 	fSlow260 = (3.5 * (fSlow49 * ((5 * fSlow255) + -3)));
		double 	fSlow261 = (3.5 * (fSlow49 * ((5 * fSlow258) + -3)));
		double 	fSlow262 = (fSlow224 - fSlow218);
		double 	fSlow263 = faustpower<2>(fSlow262);
		double 	fSlow264 = (3.5 * (fSlow49 * ((5 * fSlow263) + -3)));
		double 	fSlow265 = (fSlow228 - fSlow212);
		double 	fSlow266 = faustpower<2>(fSlow265);
		double 	fSlow267 = (3.5 * (fSlow49 * ((5 * fSlow266) + -3)));
		double 	fSlow268 = ((3 * fSlow263) + -1);
		double 	fSlow269 = ((3 * fSlow266) + -1);
		double 	fSlow270 = (fSlow234 - fSlow218);
		double 	fSlow271 = faustpower<2>(fSlow270);
		double 	fSlow272 = (3.5 * (fSlow49 * ((5 * fSlow271) + -3)));
		double 	fSlow273 = (fSlow238 - fSlow212);
		double 	fSlow274 = faustpower<2>(fSlow273);
		double 	fSlow275 = (3.5 * (fSlow49 * ((5 * fSlow274) + -3)));
		double 	fSlow276 = ((3 * fSlow271) + -1);
		double 	fSlow277 = ((3 * fSlow274) + -1);
		double 	fSlow278 = (fSlow244 - fSlow218);
		double 	fSlow279 = faustpower<2>(fSlow278);
		double 	fSlow280 = (3.5 * (fSlow49 * ((5 * fSlow279) + -3)));
		double 	fSlow281 = (fSlow248 - fSlow212);
		double 	fSlow282 = faustpower<2>(fSlow281);
		double 	fSlow283 = (3.5 * (fSlow49 * ((5 * fSlow282) + -3)));
		double 	fSlow284 = ((3 * fSlow279) + -1);
		double 	fSlow285 = ((3 * fSlow282) + -1);
		double 	fSlow286 = (0.4264014327112203 * (fSlow30 * cos((fSlow31 + -0.7853981633974484))));
		double 	fSlow287 = (0.9045340337332911 * fSlow29);
		double 	fSlow288 = (fSlow286 + fSlow287);
		double 	fSlow289 = faustpower<2>(fSlow288);
		double 	fSlow290 = (3.5 * (fSlow49 * ((5 * fSlow289) + -3)));
		double 	fSlow291 = (0.4264014327112203 * (fSlow3 * cos((fSlow4 + -0.7853981633974484))));
		double 	fSlow292 = (0.9045340337332911 * fSlow2);
		double 	fSlow293 = (fSlow291 + fSlow292);
		double 	fSlow294 = faustpower<2>(fSlow293);
		double 	fSlow295 = (3.5 * (fSlow49 * ((5 * fSlow294) + -3)));
		double 	fSlow296 = ((3 * fSlow289) + -1);
		double 	fSlow297 = ((3 * fSlow294) + -1);
		double 	fSlow298 = (0.4264014327112203 * fSlow162);
		double 	fSlow299 = (fSlow287 + fSlow298);
		double 	fSlow300 = faustpower<2>(fSlow299);
		double 	fSlow301 = ((3 * fSlow300) + -1);
		double 	fSlow302 = (0.4264014327112203 * fSlow164);
		double 	fSlow303 = (fSlow292 + fSlow302);
		double 	fSlow304 = faustpower<2>(fSlow303);
		double 	fSlow305 = ((3 * fSlow304) + -1);
		double 	fSlow306 = (3.5 * (fSlow49 * ((5 * fSlow300) + -3)));
		double 	fSlow307 = (3.5 * (fSlow49 * ((5 * fSlow304) + -3)));
		double 	fSlow308 = (0.4264014327112203 * fSlow168);
		double 	fSlow309 = (fSlow287 + fSlow308);
		double 	fSlow310 = faustpower<2>(fSlow309);
		double 	fSlow311 = (3.5 * (fSlow49 * ((5 * fSlow310) + -3)));
		double 	fSlow312 = (0.4264014327112203 * fSlow170);
		double 	fSlow313 = (fSlow292 + fSlow312);
		double 	fSlow314 = faustpower<2>(fSlow313);
		double 	fSlow315 = (3.5 * (fSlow49 * ((5 * fSlow314) + -3)));
		double 	fSlow316 = ((3 * fSlow314) + -1);
		double 	fSlow317 = ((3 * fSlow310) + -1);
		double 	fSlow318 = (0.4264014327112203 * fSlow174);
		double 	fSlow319 = (fSlow287 + fSlow318);
		double 	fSlow320 = faustpower<2>(fSlow319);
		double 	fSlow321 = ((3 * fSlow320) + -1);
		double 	fSlow322 = (0.4264014327112203 * fSlow176);
		double 	fSlow323 = (fSlow292 + fSlow322);
		double 	fSlow324 = faustpower<2>(fSlow323);
		double 	fSlow325 = ((3 * fSlow324) + -1);
		double 	fSlow326 = (3.5 * (fSlow49 * ((5 * fSlow320) + -3)));
		double 	fSlow327 = (3.5 * (fSlow49 * ((5 * fSlow324) + -3)));
		double 	fSlow328 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -0.32175055439664263))));
		double 	fSlow329 = (0.3015113445777635 * fSlow2);
		double 	fSlow330 = (fSlow328 + fSlow329);
		double 	fSlow331 = faustpower<2>(fSlow330);
		double 	fSlow332 = ((3 * fSlow331) + -1);
		double 	fSlow333 = (0.9534625892455924 * (fSlow30 * cos((fSlow31 + -0.32175055439664263))));
		double 	fSlow334 = (0.3015113445777635 * fSlow29);
		double 	fSlow335 = (fSlow333 + fSlow334);
		double 	fSlow336 = faustpower<2>(fSlow335);
		double 	fSlow337 = ((3 * fSlow336) + -1);
		double 	fSlow338 = (3.5 * (fSlow49 * ((5 * fSlow336) + -3)));
		double 	fSlow339 = (3.5 * (fSlow49 * ((5 * fSlow331) + -3)));
		double 	fSlow340 = (0.9534625892455924 * (fSlow30 * cos((fSlow31 + -1.2490457723982544))));
		double 	fSlow341 = (fSlow334 + fSlow340);
		double 	fSlow342 = faustpower<2>(fSlow341);
		double 	fSlow343 = (3.5 * (fSlow49 * ((5 * fSlow342) + -3)));
		double 	fSlow344 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -1.2490457723982544))));
		double 	fSlow345 = (fSlow329 + fSlow344);
		double 	fSlow346 = faustpower<2>(fSlow345);
		double 	fSlow347 = (3.5 * (fSlow49 * ((5 * fSlow346) + -3)));
		double 	fSlow348 = ((3 * fSlow342) + -1);
		double 	fSlow349 = ((3 * fSlow346) + -1);
		double 	fSlow350 = (0.9534625892455924 * (fSlow30 * cos((fSlow31 + -1.8925468811915387))));
		double 	fSlow351 = (fSlow334 + fSlow350);
		double 	fSlow352 = faustpower<2>(fSlow351);
		double 	fSlow353 = (3.5 * (fSlow49 * ((5 * fSlow352) + -3)));
		double 	fSlow354 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -1.8925468811915387))));
		double 	fSlow355 = (fSlow329 + fSlow354);
		double 	fSlow356 = faustpower<2>(fSlow355);
		double 	fSlow357 = (3.5 * (fSlow49 * ((5 * fSlow356) + -3)));
		double 	fSlow358 = ((3 * fSlow352) + -1);
		double 	fSlow359 = ((3 * fSlow356) + -1);
		double 	fSlow360 = (0.9534625892455924 * (fSlow30 * cos((fSlow31 + -2.8198420991931505))));
		double 	fSlow361 = (fSlow334 + fSlow360);
		double 	fSlow362 = faustpower<2>(fSlow361);
		double 	fSlow363 = ((3 * fSlow362) + -1);
		double 	fSlow364 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -2.8198420991931505))));
		double 	fSlow365 = (fSlow329 + fSlow364);
		double 	fSlow366 = faustpower<2>(fSlow365);
		double 	fSlow367 = ((3 * fSlow366) + -1);
		double 	fSlow368 = (3.5 * (fSlow49 * ((5 * fSlow362) + -3)));
		double 	fSlow369 = (3.5 * (fSlow49 * ((5 * fSlow366) + -3)));
		double 	fSlow370 = (0.9534625892455924 * (fSlow30 * cos((fSlow31 + -3.4633432079864357))));
		double 	fSlow371 = (fSlow334 + fSlow370);
		double 	fSlow372 = faustpower<2>(fSlow371);
		double 	fSlow373 = ((3 * fSlow372) + -1);
		double 	fSlow374 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -3.4633432079864357))));
		double 	fSlow375 = (fSlow329 + fSlow374);
		double 	fSlow376 = faustpower<2>(fSlow375);
		double 	fSlow377 = ((3 * fSlow376) + -1);
		double 	fSlow378 = (3.5 * (fSlow49 * ((5 * fSlow372) + -3)));
		double 	fSlow379 = (3.5 * (fSlow49 * ((5 * fSlow376) + -3)));
		double 	fSlow380 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -4.3906384259880475))));
		double 	fSlow381 = (fSlow329 + fSlow380);
		double 	fSlow382 = faustpower<2>(fSlow381);
		double 	fSlow383 = ((3 * fSlow382) + -1);
		double 	fSlow384 = (0.9534625892455924 * (fSlow30 * cos((fSlow31 + -4.3906384259880475))));
		double 	fSlow385 = (fSlow334 + fSlow384);
		double 	fSlow386 = faustpower<2>(fSlow385);
		double 	fSlow387 = ((3 * fSlow386) + -1);
		double 	fSlow388 = (3.5 * (fSlow49 * ((5 * fSlow386) + -3)));
		double 	fSlow389 = (3.5 * (fSlow49 * ((5 * fSlow382) + -3)));
		double 	fSlow390 = (0.9534625892455924 * (fSlow30 * cos((fSlow31 + -5.034139534781332))));
		double 	fSlow391 = (fSlow334 + fSlow390);
		double 	fSlow392 = faustpower<2>(fSlow391);
		double 	fSlow393 = ((3 * fSlow392) + -1);
		double 	fSlow394 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -5.034139534781332))));
		double 	fSlow395 = (fSlow329 + fSlow394);
		double 	fSlow396 = faustpower<2>(fSlow395);
		double 	fSlow397 = ((3 * fSlow396) + -1);
		double 	fSlow398 = (3.5 * (fSlow49 * ((5 * fSlow392) + -3)));
		double 	fSlow399 = (3.5 * (fSlow49 * ((5 * fSlow396) + -3)));
		double 	fSlow400 = (0.9534625892455924 * (fSlow30 * cos((fSlow31 + -5.961434752782943))));
		double 	fSlow401 = (fSlow334 + fSlow400);
		double 	fSlow402 = faustpower<2>(fSlow401);
		double 	fSlow403 = (3.5 * (fSlow49 * ((5 * fSlow402) + -3)));
		double 	fSlow404 = (0.9534625892455924 * (fSlow3 * cos((fSlow4 + -5.961434752782943))));
		double 	fSlow405 = (fSlow329 + fSlow404);
		double 	fSlow406 = faustpower<2>(fSlow405);
		double 	fSlow407 = (3.5 * (fSlow49 * ((5 * fSlow406) + -3)));
		double 	fSlow408 = ((3 * fSlow406) + -1);
		double 	fSlow409 = ((3 * fSlow402) + -1);
		double 	fSlow410 = (fSlow328 - fSlow329);
		double 	fSlow411 = faustpower<2>(fSlow410);
		double 	fSlow412 = ((3 * fSlow411) + -1);
		double 	fSlow413 = (fSlow333 - fSlow334);
		double 	fSlow414 = faustpower<2>(fSlow413);
		double 	fSlow415 = ((3 * fSlow414) + -1);
		double 	fSlow416 = (3.5 * (fSlow49 * ((5 * fSlow414) + -3)));
		double 	fSlow417 = (3.5 * (fSlow49 * ((5 * fSlow411) + -3)));
		double 	fSlow418 = (fSlow344 - fSlow329);
		double 	fSlow419 = faustpower<2>(fSlow418);
		double 	fSlow420 = ((3 * fSlow419) + -1);
		double 	fSlow421 = (fSlow340 - fSlow334);
		double 	fSlow422 = faustpower<2>(fSlow421);
		double 	fSlow423 = ((3 * fSlow422) + -1);
		double 	fSlow424 = (3.5 * (fSlow49 * ((5 * fSlow422) + -3)));
		double 	fSlow425 = (3.5 * (fSlow49 * ((5 * fSlow419) + -3)));
		double 	fSlow426 = (fSlow350 - fSlow334);
		double 	fSlow427 = faustpower<2>(fSlow426);
		double 	fSlow428 = (3.5 * (fSlow49 * ((5 * fSlow427) + -3)));
		double 	fSlow429 = (fSlow354 - fSlow329);
		double 	fSlow430 = faustpower<2>(fSlow429);
		double 	fSlow431 = (3.5 * (fSlow49 * ((5 * fSlow430) + -3)));
		double 	fSlow432 = ((3 * fSlow427) + -1);
		double 	fSlow433 = ((3 * fSlow430) + -1);
		double 	fSlow434 = (fSlow360 - fSlow334);
		double 	fSlow435 = faustpower<2>(fSlow434);
		double 	fSlow436 = (3.5 * (fSlow49 * ((5 * fSlow435) + -3)));
		double 	fSlow437 = (fSlow364 - fSlow329);
		double 	fSlow438 = faustpower<2>(fSlow437);
		double 	fSlow439 = (3.5 * (fSlow49 * ((5 * fSlow438) + -3)));
		double 	fSlow440 = ((3 * fSlow435) + -1);
		double 	fSlow441 = ((3 * fSlow438) + -1);
		double 	fSlow442 = (fSlow370 - fSlow334);
		double 	fSlow443 = faustpower<2>(fSlow442);
		double 	fSlow444 = ((3 * fSlow443) + -1);
		double 	fSlow445 = (fSlow374 - fSlow329);
		double 	fSlow446 = faustpower<2>(fSlow445);
		double 	fSlow447 = ((3 * fSlow446) + -1);
		double 	fSlow448 = (3.5 * (fSlow49 * ((5 * fSlow443) + -3)));
		double 	fSlow449 = (3.5 * (fSlow49 * ((5 * fSlow446) + -3)));
		double 	fSlow450 = (fSlow384 - fSlow334);
		double 	fSlow451 = faustpower<2>(fSlow450);
		double 	fSlow452 = (3.5 * (fSlow49 * ((5 * fSlow451) + -3)));
		double 	fSlow453 = (fSlow380 - fSlow329);
		double 	fSlow454 = faustpower<2>(fSlow453);
		double 	fSlow455 = (3.5 * (fSlow49 * ((5 * fSlow454) + -3)));
		double 	fSlow456 = ((3 * fSlow454) + -1);
		double 	fSlow457 = ((3 * fSlow451) + -1);
		double 	fSlow458 = (fSlow394 - fSlow329);
		double 	fSlow459 = faustpower<2>(fSlow458);
		double 	fSlow460 = ((3 * fSlow459) + -1);
		double 	fSlow461 = (fSlow390 - fSlow334);
		double 	fSlow462 = faustpower<2>(fSlow461);
		double 	fSlow463 = ((3 * fSlow462) + -1);
		double 	fSlow464 = (3.5 * (fSlow49 * ((5 * fSlow462) + -3)));
		double 	fSlow465 = (3.5 * (fSlow49 * ((5 * fSlow459) + -3)));
		double 	fSlow466 = (fSlow404 - fSlow329);
		double 	fSlow467 = faustpower<2>(fSlow466);
		double 	fSlow468 = ((3 * fSlow467) + -1);
		double 	fSlow469 = (fSlow400 - fSlow334);
		double 	fSlow470 = faustpower<2>(fSlow469);
		double 	fSlow471 = ((3 * fSlow470) + -1);
		double 	fSlow472 = (3.5 * (fSlow49 * ((5 * fSlow470) + -3)));
		double 	fSlow473 = (3.5 * (fSlow49 * ((5 * fSlow467) + -3)));
		double 	fSlow474 = (fSlow286 - fSlow287);
		double 	fSlow475 = faustpower<2>(fSlow474);
		double 	fSlow476 = ((3 * fSlow475) + -1);
		double 	fSlow477 = (fSlow291 - fSlow292);
		double 	fSlow478 = faustpower<2>(fSlow477);
		double 	fSlow479 = ((3 * fSlow478) + -1);
		double 	fSlow480 = (3.5 * (fSlow49 * ((5 * fSlow475) + -3)));
		double 	fSlow481 = (3.5 * (fSlow49 * ((5 * fSlow478) + -3)));
		double 	fSlow482 = (fSlow298 - fSlow287);
		double 	fSlow483 = faustpower<2>(fSlow482);
		double 	fSlow484 = ((3 * fSlow483) + -1);
		double 	fSlow485 = (fSlow302 - fSlow292);
		double 	fSlow486 = faustpower<2>(fSlow485);
		double 	fSlow487 = ((3 * fSlow486) + -1);
		double 	fSlow488 = (3.5 * (fSlow49 * ((5 * fSlow483) + -3)));
		double 	fSlow489 = (3.5 * (fSlow49 * ((5 * fSlow486) + -3)));
		double 	fSlow490 = (fSlow312 - fSlow292);
		double 	fSlow491 = faustpower<2>(fSlow490);
		double 	fSlow492 = ((3 * fSlow491) + -1);
		double 	fSlow493 = (fSlow308 - fSlow287);
		double 	fSlow494 = faustpower<2>(fSlow493);
		double 	fSlow495 = ((3 * fSlow494) + -1);
		double 	fSlow496 = (3.5 * (fSlow49 * ((5 * fSlow494) + -3)));
		double 	fSlow497 = (3.5 * (fSlow49 * ((5 * fSlow491) + -3)));
		double 	fSlow498 = (fSlow318 - fSlow287);
		double 	fSlow499 = faustpower<2>(fSlow498);
		double 	fSlow500 = (3.5 * (fSlow49 * ((5 * fSlow499) + -3)));
		double 	fSlow501 = (fSlow322 - fSlow292);
		double 	fSlow502 = faustpower<2>(fSlow501);
		double 	fSlow503 = (3.5 * (fSlow49 * ((5 * fSlow502) + -3)));
		double 	fSlow504 = ((3 * fSlow502) + -1);
		double 	fSlow505 = ((3 * fSlow499) + -1);
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
			fRec2[0] = (fSlow21 + (0.999 * fRec2[1]));
			double fTemp0 = ((double)input1[i] * fRec2[0]);
			fRec3[0] = (fRec1[1] + fRec3[1]);
			fRec4[0] = (fRec3[1] + fRec4[1]);
			fRec1[0] = (fSlow13 * ((fSlow20 * fTemp0) - ((fSlow23 * fRec3[0]) + (fSlow22 * fRec4[0]))));
			fRec5[0] = (fRec1[0] + fRec5[1]);
			double fTemp1 = (fRec5[0] - fRec1[0]);
			fVec0[0] = fTemp1;
			fRec6[0] = (fRec6[1] + fVec0[1]);
			double fTemp2 = (fSlow27 * fTemp0);
			fRec8[0] = (fRec7[1] + fRec8[1]);
			fRec9[0] = (fRec8[1] + fRec9[1]);
			fRec7[0] = (fSlow13 * (fTemp2 - ((fSlow23 * fRec8[0]) + (fSlow22 * fRec9[0]))));
			double fTemp3 = ((fRec1[0] + (fSlow24 * ((fSlow26 * fVec0[0]) + (fSlow25 * fRec6[0])))) + fRec7[0]);
			fRec11[0] = (fSlow44 + (0.999 * fRec11[1]));
			double fTemp4 = ((double)input0[i] * fRec11[0]);
			fRec12[0] = (fRec10[1] + fRec12[1]);
			fRec13[0] = (fRec12[1] + fRec13[1]);
			fRec10[0] = (fSlow13 * ((fSlow43 * fTemp4) - ((fSlow23 * fRec12[0]) + (fSlow22 * fRec13[0]))));
			fRec14[0] = (fRec10[0] + fRec14[1]);
			double fTemp5 = (fRec14[0] - fRec10[0]);
			fVec1[0] = fTemp5;
			fRec15[0] = (fRec15[1] + fVec1[1]);
			fRec17[0] = (fRec16[1] + fRec17[1]);
			fRec18[0] = (fRec17[1] + fRec18[1]);
			double fTemp6 = (fSlow48 * fTemp4);
			fRec16[0] = (0 - (fSlow13 * (((fSlow23 * fRec17[0]) + (fSlow22 * fRec18[0])) - fTemp6)));
			double fTemp7 = ((fRec10[0] + (fSlow45 * ((fSlow47 * fVec1[0]) + (fSlow46 * fRec15[0])))) + fRec16[0]);
			fRec23[0] = (fRec21[1] + fRec23[1]);
			fRec22[0] = (fRec22[1] + fRec23[1]);
			fRec21[0] = (fSlow54 * ((fSlow56 * fTemp4) - ((fSlow57 * fRec22[0]) + (fSlow58 * fRec23[0]))));
			fRec24[0] = (fRec21[0] + fRec24[1]);
			double fTemp8 = (fRec24[0] - fRec21[0]);
			fVec2[0] = fTemp8;
			fRec25[0] = (fRec25[1] + fVec2[1]);
			fRec26[0] = (fRec20[1] + fRec26[1]);
			fRec20[0] = (fSlow53 * ((fSlow51 * (fRec21[0] + (fSlow59 * ((fSlow61 * fVec2[0]) + (fSlow60 * fRec25[0]))))) - (fSlow62 * fRec26[0])));
			fRec19[0] = (fRec20[0] + fRec19[1]);
			fRec30[0] = (fRec28[1] + fRec30[1]);
			fRec29[0] = (fRec29[1] + fRec30[1]);
			fRec28[0] = (0 - (fSlow54 * (((fSlow57 * fRec29[0]) + (fSlow58 * fRec30[0])) - fTemp6)));
			fRec31[0] = (fRec27[1] + fRec31[1]);
			fRec27[0] = (fSlow53 * (fRec28[0] - (fSlow62 * fRec31[0])));
			double fTemp9 = ((fSlow52 * (fRec19[0] - fRec20[0])) + (fRec20[0] + fRec27[0]));
			fRec34[0] = (fRec33[1] + fRec34[1]);
			fRec33[0] = (fSlow66 * ((fSlow67 * fTemp4) - (fSlow68 * fRec34[0])));
			fRec32[0] = (fRec33[0] + fRec32[1]);
			fRec36[0] = (fRec35[1] + fRec36[1]);
			fRec35[0] = (fSlow66 * (fTemp6 - (fSlow68 * fRec36[0])));
			double fTemp10 = ((fSlow65 * (fRec32[0] - fRec33[0])) + (fRec33[0] + fRec35[0]));
			double fTemp11 = (fSlow63 * fTemp10);
			fRec39[0] = (fRec38[1] + fRec39[1]);
			fRec38[0] = (fSlow66 * ((fSlow71 * fTemp0) - (fSlow68 * fRec39[0])));
			fRec37[0] = (fRec38[0] + fRec37[1]);
			fRec41[0] = (fRec40[1] + fRec41[1]);
			fRec40[0] = (fSlow66 * (fTemp2 - (fSlow68 * fRec41[0])));
			double fTemp12 = ((fSlow70 * (fRec37[0] - fRec38[0])) + (fRec38[0] + fRec40[0]));
			double fTemp13 = (fSlow63 * fTemp12);
			fRec46[0] = (fRec44[1] + fRec46[1]);
			fRec45[0] = (fRec45[1] + fRec46[1]);
			fRec44[0] = (0 - (fSlow54 * (((fSlow57 * fRec45[0]) + (fSlow58 * fRec46[0])) - (fSlow76 * fTemp0))));
			fRec47[0] = (fRec44[0] + fRec47[1]);
			double fTemp14 = (fRec47[0] - fRec44[0]);
			fVec3[0] = fTemp14;
			fRec48[0] = (fRec48[1] + fVec3[1]);
			fRec49[0] = (fRec43[1] + fRec49[1]);
			fRec43[0] = (fSlow53 * ((fSlow73 * (fRec44[0] + (fSlow77 * ((fSlow79 * fVec3[0]) + (fSlow78 * fRec48[0]))))) - (fSlow62 * fRec49[0])));
			fRec42[0] = (fRec43[0] + fRec42[1]);
			fRec53[0] = (fRec51[1] + fRec53[1]);
			fRec52[0] = (fRec52[1] + fRec53[1]);
			fRec51[0] = (0 - (fSlow54 * (((fSlow57 * fRec52[0]) + (fSlow58 * fRec53[0])) - fTemp2)));
			fRec54[0] = (fRec50[1] + fRec54[1]);
			fRec50[0] = (fSlow53 * (fRec51[0] - (fSlow62 * fRec54[0])));
			double fTemp15 = ((fSlow74 * (fRec42[0] - fRec43[0])) + (fRec43[0] + fRec50[0]));
			double fTemp16 = (fSlow80 * ((fSlow81 * fTemp4) + (fSlow82 * fTemp0)));
			double fTemp17 = (0.012698412698412698 * ((fSlow0 * ((fSlow9 * fTemp3) + (fSlow36 * fTemp7))) + (((fSlow34 * ((fSlow50 * fTemp9) + fTemp11)) + (fSlow7 * (fTemp13 + (fSlow72 * fTemp15)))) + fTemp16)));
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fbargraph0 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp17;
			double fTemp18 = (0.012698412698412698 * ((fSlow63 * ((fSlow5 * fTemp12) + (fSlow32 * fTemp10))) + ((fSlow83 * ((fSlow84 * fTemp15) + (fSlow85 * fTemp9))) + (fTemp16 + (fSlow0 * ((fSlow86 * fTemp3) + (fSlow87 * fTemp7)))))));
			fRec55[0] = max((fRec55[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fbargraph1 = fRec55[0];
			output1[i] = (FAUSTFLOAT)fTemp18;
			double fTemp19 = (0.012698412698412698 * (((fTemp16 + (fSlow83 * ((fSlow89 * fTemp15) + (fSlow91 * fTemp9)))) + (fSlow63 * ((fSlow88 * fTemp12) + (fSlow90 * fTemp10)))) + (fSlow0 * ((fSlow92 * fTemp7) + (fSlow93 * fTemp3)))));
			fRec56[0] = max((fRec56[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fbargraph2 = fRec56[0];
			output2[i] = (FAUSTFLOAT)fTemp19;
			double fTemp20 = (0.012698412698412698 * (((fTemp16 + (fSlow83 * ((fSlow95 * fTemp15) + (fSlow97 * fTemp9)))) + (fSlow63 * ((fSlow94 * fTemp12) + (fSlow96 * fTemp10)))) + (fSlow0 * ((fSlow98 * fTemp7) + (fSlow99 * fTemp3)))));
			fRec57[0] = max((fRec57[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph3 = fRec57[0];
			output3[i] = (FAUSTFLOAT)fTemp20;
			double fTemp21 = (0.012698412698412698 * (((fTemp16 + (fSlow83 * ((fSlow101 * fTemp15) + (fSlow103 * fTemp9)))) + (fSlow63 * ((fSlow100 * fTemp12) + (fSlow102 * fTemp10)))) + (fSlow0 * ((fSlow104 * fTemp7) + (fSlow105 * fTemp3)))));
			fRec58[0] = max((fRec58[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fbargraph4 = fRec58[0];
			output4[i] = (FAUSTFLOAT)fTemp21;
			double fTemp22 = (0.012698412698412698 * ((fSlow0 * ((fSlow108 * fTemp3) + (fSlow111 * fTemp7))) + ((fSlow106 * (fTemp13 + (fSlow112 * fTemp15))) + (fTemp16 + (fSlow109 * (fTemp11 + (fSlow113 * fTemp9)))))));
			fRec59[0] = max((fRec59[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fbargraph5 = fRec59[0];
			output5[i] = (FAUSTFLOAT)fTemp22;
			double fTemp23 = (0.022574955908289243 * (((fSlow116 * (fTemp13 + (fSlow118 * fTemp15))) + (fTemp16 + (fSlow121 * (fTemp11 + (fSlow123 * fTemp9))))) + (fSlow0 * ((fSlow124 * fTemp7) + (fSlow125 * fTemp3)))));
			fRec60[0] = max((fRec60[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fbargraph6 = fRec60[0];
			output6[i] = (FAUSTFLOAT)fTemp23;
			double fTemp24 = (0.022574955908289243 * (((fSlow127 * (fTemp13 + (fSlow129 * fTemp15))) + (fTemp16 + (fSlow131 * (fTemp11 + (fSlow133 * fTemp9))))) + (fSlow0 * ((fSlow134 * fTemp7) + (fSlow135 * fTemp3)))));
			fRec61[0] = max((fRec61[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fbargraph7 = fRec61[0];
			output7[i] = (FAUSTFLOAT)fTemp24;
			double fTemp25 = (0.022574955908289243 * (((fSlow137 * (fTemp13 + (fSlow139 * fTemp15))) + (fTemp16 + (fSlow141 * (fTemp11 + (fSlow143 * fTemp9))))) + (fSlow0 * ((fSlow144 * fTemp7) + (fSlow145 * fTemp3)))));
			fRec62[0] = max((fRec62[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fbargraph8 = fRec62[0];
			output8[i] = (FAUSTFLOAT)fTemp25;
			double fTemp26 = (0.022574955908289243 * (((fTemp16 + (fSlow147 * (fTemp11 + (fSlow149 * fTemp9)))) + (fSlow151 * (fTemp13 + (fSlow153 * fTemp15)))) + (fSlow0 * ((fSlow154 * fTemp3) + (fSlow155 * fTemp7)))));
			fRec63[0] = max((fRec63[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fbargraph9 = fRec63[0];
			output9[i] = (FAUSTFLOAT)fTemp26;
			double fTemp27 = (0.022574955908289243 * (((fSlow0 * ((fSlow157 * fTemp3) + (fSlow159 * fTemp7))) + ((fSlow63 * ((fSlow158 * fTemp10) + (fSlow156 * fTemp12))) + (fSlow83 * ((fSlow160 * fTemp15) + (fSlow161 * fTemp9))))) + fTemp16));
			fRec64[0] = max((fRec64[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fbargraph10 = fRec64[0];
			output10[i] = (FAUSTFLOAT)fTemp27;
			double fTemp28 = (0.022574955908289243 * (((fSlow83 * ((fSlow163 * fTemp9) + (fSlow165 * fTemp15))) + ((fSlow63 * ((fSlow162 * fTemp10) + (fSlow164 * fTemp12))) + (fSlow0 * ((fSlow166 * fTemp7) + (fSlow167 * fTemp3))))) + fTemp16));
			fRec65[0] = max((fRec65[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph11 = fRec65[0];
			output11[i] = (FAUSTFLOAT)fTemp28;
			double fTemp29 = (0.022574955908289243 * ((((fSlow0 * ((fSlow169 * fTemp7) + (fSlow171 * fTemp3))) + (fSlow63 * ((fSlow168 * fTemp10) + (fSlow170 * fTemp12)))) + (fSlow83 * ((fSlow172 * fTemp15) + (fSlow173 * fTemp9)))) + fTemp16));
			fRec66[0] = max((fRec66[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph12 = fRec66[0];
			output12[i] = (FAUSTFLOAT)fTemp29;
			double fTemp30 = (0.022574955908289243 * (((fSlow83 * ((fSlow175 * fTemp9) + (fSlow177 * fTemp15))) + ((fSlow63 * ((fSlow174 * fTemp10) + (fSlow176 * fTemp12))) + (fSlow0 * ((fSlow178 * fTemp7) + (fSlow179 * fTemp3))))) + fTemp16));
			fRec67[0] = max((fRec67[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fbargraph13 = fRec67[0];
			output13[i] = (FAUSTFLOAT)fTemp30;
			double fTemp31 = (0.022574955908289243 * (((fSlow0 * ((fSlow182 * fTemp7) + (fSlow185 * fTemp3))) + ((fSlow180 * ((fSlow186 * fTemp9) + fTemp11)) + (fSlow183 * ((fSlow187 * fTemp15) + fTemp13)))) + fTemp16));
			fRec68[0] = max((fRec68[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fbargraph14 = fRec68[0];
			output14[i] = (FAUSTFLOAT)fTemp31;
			double fTemp32 = (0.022574955908289243 * (((fSlow0 * ((fSlow190 * fTemp7) + (fSlow193 * fTemp3))) + ((fSlow188 * ((fSlow194 * fTemp9) + fTemp11)) + (fSlow191 * ((fSlow195 * fTemp15) + fTemp13)))) + fTemp16));
			fRec69[0] = max((fRec69[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph15 = fRec69[0];
			output15[i] = (FAUSTFLOAT)fTemp32;
			double fTemp33 = (0.022574955908289243 * (((fSlow0 * ((fSlow198 * fTemp7) + (fSlow201 * fTemp3))) + ((fSlow196 * ((fSlow202 * fTemp9) + fTemp11)) + (fSlow199 * ((fSlow203 * fTemp15) + fTemp13)))) + fTemp16));
			fRec70[0] = max((fRec70[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph16 = fRec70[0];
			output16[i] = (FAUSTFLOAT)fTemp33;
			double fTemp34 = (0.022574955908289243 * (((fSlow0 * ((fSlow206 * fTemp7) + (fSlow209 * fTemp3))) + ((fSlow204 * ((fSlow210 * fTemp9) + fTemp11)) + (fSlow207 * ((fSlow211 * fTemp15) + fTemp13)))) + fTemp16));
			fRec71[0] = max((fRec71[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fbargraph17 = fRec71[0];
			output17[i] = (FAUSTFLOAT)fTemp34;
			double fTemp35 = (0.02109375 * (((fSlow0 * ((fSlow216 * fTemp3) + (fSlow221 * fTemp7))) + ((fSlow219 * ((fSlow222 * fTemp9) + fTemp11)) + (fSlow214 * ((fSlow223 * fTemp15) + fTemp13)))) + fTemp16));
			fRec72[0] = max((fRec72[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fbargraph18 = fRec72[0];
			output18[i] = (FAUSTFLOAT)fTemp35;
			double fTemp36 = (0.02109375 * ((((fSlow225 * ((fSlow227 * fTemp9) + fTemp11)) + (fSlow229 * ((fSlow231 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow232 * fTemp7) + (fSlow233 * fTemp3)))) + fTemp16));
			fRec73[0] = max((fRec73[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph19 = fRec73[0];
			output19[i] = (FAUSTFLOAT)fTemp36;
			double fTemp37 = (0.02109375 * (((fSlow0 * ((fSlow237 * fTemp7) + (fSlow241 * fTemp3))) + ((fSlow235 * ((fSlow242 * fTemp9) + fTemp11)) + (fSlow239 * ((fSlow243 * fTemp15) + fTemp13)))) + fTemp16));
			fRec74[0] = max((fRec74[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fbargraph20 = fRec74[0];
			output20[i] = (FAUSTFLOAT)fTemp37;
			double fTemp38 = (0.02109375 * ((((fSlow245 * ((fSlow247 * fTemp9) + fTemp11)) + (fSlow249 * ((fSlow251 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow252 * fTemp7) + (fSlow253 * fTemp3)))) + fTemp16));
			fRec75[0] = max((fRec75[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fbargraph21 = fRec75[0];
			output21[i] = (FAUSTFLOAT)fTemp38;
			double fTemp39 = (0.02109375 * (((fSlow0 * ((fSlow256 * fTemp7) + (fSlow259 * fTemp3))) + ((fSlow254 * ((fSlow260 * fTemp9) + fTemp11)) + (fSlow257 * ((fSlow261 * fTemp15) + fTemp13)))) + fTemp16));
			fRec76[0] = max((fRec76[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fbargraph22 = fRec76[0];
			output22[i] = (FAUSTFLOAT)fTemp39;
			double fTemp40 = (0.02109375 * ((((fSlow262 * ((fSlow264 * fTemp9) + fTemp11)) + (fSlow265 * ((fSlow267 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow268 * fTemp7) + (fSlow269 * fTemp3)))) + fTemp16));
			fRec77[0] = max((fRec77[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fbargraph23 = fRec77[0];
			output23[i] = (FAUSTFLOAT)fTemp40;
			double fTemp41 = (0.02109375 * ((((fSlow270 * ((fSlow272 * fTemp9) + fTemp11)) + (fSlow273 * ((fSlow275 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow276 * fTemp7) + (fSlow277 * fTemp3)))) + fTemp16));
			fRec78[0] = max((fRec78[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fbargraph24 = fRec78[0];
			output24[i] = (FAUSTFLOAT)fTemp41;
			double fTemp42 = (0.02109375 * ((((fSlow278 * ((fSlow280 * fTemp9) + fTemp11)) + (fSlow281 * ((fSlow283 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow284 * fTemp7) + (fSlow285 * fTemp3)))) + fTemp16));
			fRec79[0] = max((fRec79[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fbargraph25 = fRec79[0];
			output25[i] = (FAUSTFLOAT)fTemp42;
			double fTemp43 = (0.02017333553791887 * ((((fSlow288 * ((fSlow290 * fTemp9) + fTemp11)) + (fSlow293 * ((fSlow295 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow296 * fTemp7) + (fSlow297 * fTemp3)))) + fTemp16));
			fRec80[0] = max((fRec80[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fbargraph26 = fRec80[0];
			output26[i] = (FAUSTFLOAT)fTemp43;
			double fTemp44 = (0.02017333553791887 * (((fSlow0 * ((fSlow301 * fTemp7) + (fSlow305 * fTemp3))) + ((fSlow299 * ((fSlow306 * fTemp9) + fTemp11)) + (fSlow303 * ((fSlow307 * fTemp15) + fTemp13)))) + fTemp16));
			fRec81[0] = max((fRec81[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fbargraph27 = fRec81[0];
			output27[i] = (FAUSTFLOAT)fTemp44;
			double fTemp45 = (0.02017333553791887 * ((((fSlow309 * ((fSlow311 * fTemp9) + fTemp11)) + (fSlow313 * ((fSlow315 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow316 * fTemp3) + (fSlow317 * fTemp7)))) + fTemp16));
			fRec82[0] = max((fRec82[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fbargraph28 = fRec82[0];
			output28[i] = (FAUSTFLOAT)fTemp45;
			double fTemp46 = (0.02017333553791887 * (((fSlow0 * ((fSlow321 * fTemp7) + (fSlow325 * fTemp3))) + ((fSlow319 * ((fSlow326 * fTemp9) + fTemp11)) + (fSlow323 * ((fSlow327 * fTemp15) + fTemp13)))) + fTemp16));
			fRec83[0] = max((fRec83[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fbargraph29 = fRec83[0];
			output29[i] = (FAUSTFLOAT)fTemp46;
			double fTemp47 = (0.02017333553791887 * (((fSlow0 * ((fSlow332 * fTemp3) + (fSlow337 * fTemp7))) + ((fSlow335 * ((fSlow338 * fTemp9) + fTemp11)) + (fSlow330 * ((fSlow339 * fTemp15) + fTemp13)))) + fTemp16));
			fRec84[0] = max((fRec84[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fbargraph30 = fRec84[0];
			output30[i] = (FAUSTFLOAT)fTemp47;
			double fTemp48 = (0.02017333553791887 * ((((fSlow341 * ((fSlow343 * fTemp9) + fTemp11)) + (fSlow345 * ((fSlow347 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow348 * fTemp7) + (fSlow349 * fTemp3)))) + fTemp16));
			fRec85[0] = max((fRec85[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fbargraph31 = fRec85[0];
			output31[i] = (FAUSTFLOAT)fTemp48;
			double fTemp49 = (0.02017333553791887 * ((((fSlow351 * ((fSlow353 * fTemp9) + fTemp11)) + (fSlow355 * ((fSlow357 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow358 * fTemp7) + (fSlow359 * fTemp3)))) + fTemp16));
			fRec86[0] = max((fRec86[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fbargraph32 = fRec86[0];
			output32[i] = (FAUSTFLOAT)fTemp49;
			double fTemp50 = (0.02017333553791887 * (((fSlow0 * ((fSlow363 * fTemp7) + (fSlow367 * fTemp3))) + ((fSlow361 * ((fSlow368 * fTemp9) + fTemp11)) + (fSlow365 * ((fSlow369 * fTemp15) + fTemp13)))) + fTemp16));
			fRec87[0] = max((fRec87[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fbargraph33 = fRec87[0];
			output33[i] = (FAUSTFLOAT)fTemp50;
			double fTemp51 = (0.02017333553791887 * (((fSlow0 * ((fSlow373 * fTemp7) + (fSlow377 * fTemp3))) + ((fSlow371 * ((fSlow378 * fTemp9) + fTemp11)) + (fSlow375 * ((fSlow379 * fTemp15) + fTemp13)))) + fTemp16));
			fRec88[0] = max((fRec88[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fbargraph34 = fRec88[0];
			output34[i] = (FAUSTFLOAT)fTemp51;
			double fTemp52 = (0.02017333553791887 * (((fSlow0 * ((fSlow383 * fTemp3) + (fSlow387 * fTemp7))) + ((fSlow385 * ((fSlow388 * fTemp9) + fTemp11)) + (fSlow381 * ((fSlow389 * fTemp15) + fTemp13)))) + fTemp16));
			fRec89[0] = max((fRec89[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fbargraph35 = fRec89[0];
			output35[i] = (FAUSTFLOAT)fTemp52;
			double fTemp53 = (0.02017333553791887 * (((fSlow0 * ((fSlow393 * fTemp7) + (fSlow397 * fTemp3))) + ((fSlow391 * ((fSlow398 * fTemp9) + fTemp11)) + (fSlow395 * ((fSlow399 * fTemp15) + fTemp13)))) + fTemp16));
			fRec90[0] = max((fRec90[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fbargraph36 = fRec90[0];
			output36[i] = (FAUSTFLOAT)fTemp53;
			double fTemp54 = (0.02017333553791887 * ((((fSlow401 * ((fSlow403 * fTemp9) + fTemp11)) + (fSlow405 * ((fSlow407 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow408 * fTemp3) + (fSlow409 * fTemp7)))) + fTemp16));
			fRec91[0] = max((fRec91[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fbargraph37 = fRec91[0];
			output37[i] = (FAUSTFLOAT)fTemp54;
			double fTemp55 = (0.02017333553791887 * (((fSlow0 * ((fSlow412 * fTemp3) + (fSlow415 * fTemp7))) + ((fSlow413 * ((fSlow416 * fTemp9) + fTemp11)) + (fSlow410 * ((fSlow417 * fTemp15) + fTemp13)))) + fTemp16));
			fRec92[0] = max((fRec92[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fbargraph38 = fRec92[0];
			output38[i] = (FAUSTFLOAT)fTemp55;
			double fTemp56 = (0.02017333553791887 * (((fSlow0 * ((fSlow420 * fTemp3) + (fSlow423 * fTemp7))) + ((fSlow421 * ((fSlow424 * fTemp9) + fTemp11)) + (fSlow418 * ((fSlow425 * fTemp15) + fTemp13)))) + fTemp16));
			fRec93[0] = max((fRec93[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fbargraph39 = fRec93[0];
			output39[i] = (FAUSTFLOAT)fTemp56;
			double fTemp57 = (0.02017333553791887 * ((((fSlow426 * ((fSlow428 * fTemp9) + fTemp11)) + (fSlow429 * ((fSlow431 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow432 * fTemp7) + (fSlow433 * fTemp3)))) + fTemp16));
			fRec94[0] = max((fRec94[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fbargraph40 = fRec94[0];
			output40[i] = (FAUSTFLOAT)fTemp57;
			double fTemp58 = (0.02017333553791887 * ((((fSlow434 * ((fSlow436 * fTemp9) + fTemp11)) + (fSlow437 * ((fSlow439 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow440 * fTemp7) + (fSlow441 * fTemp3)))) + fTemp16));
			fRec95[0] = max((fRec95[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fbargraph41 = fRec95[0];
			output41[i] = (FAUSTFLOAT)fTemp58;
			double fTemp59 = (0.02017333553791887 * (((fSlow0 * ((fSlow444 * fTemp7) + (fSlow447 * fTemp3))) + ((fSlow442 * ((fSlow448 * fTemp9) + fTemp11)) + (fSlow445 * ((fSlow449 * fTemp15) + fTemp13)))) + fTemp16));
			fRec96[0] = max((fRec96[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fbargraph42 = fRec96[0];
			output42[i] = (FAUSTFLOAT)fTemp59;
			double fTemp60 = (0.02017333553791887 * ((((fSlow450 * ((fSlow452 * fTemp9) + fTemp11)) + (fSlow453 * ((fSlow455 * fTemp15) + fTemp13))) + (fSlow0 * ((fSlow456 * fTemp3) + (fSlow457 * fTemp7)))) + fTemp16));
			fRec97[0] = max((fRec97[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fbargraph43 = fRec97[0];
			output43[i] = (FAUSTFLOAT)fTemp60;
			double fTemp61 = (0.02017333553791887 * (((fSlow0 * ((fSlow460 * fTemp3) + (fSlow463 * fTemp7))) + ((fSlow461 * ((fSlow464 * fTemp9) + fTemp11)) + (fSlow458 * ((fSlow465 * fTemp15) + fTemp13)))) + fTemp16));
			fRec98[0] = max((fRec98[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fbargraph44 = fRec98[0];
			output44[i] = (FAUSTFLOAT)fTemp61;
			double fTemp62 = (0.02017333553791887 * (((fSlow0 * ((fSlow468 * fTemp3) + (fSlow471 * fTemp7))) + ((fSlow469 * ((fSlow472 * fTemp9) + fTemp11)) + (fSlow466 * ((fSlow473 * fTemp15) + fTemp13)))) + fTemp16));
			fRec99[0] = max((fRec99[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fbargraph45 = fRec99[0];
			output45[i] = (FAUSTFLOAT)fTemp62;
			double fTemp63 = (0.02017333553791887 * (((fSlow0 * ((fSlow476 * fTemp7) + (fSlow479 * fTemp3))) + ((fSlow474 * ((fSlow480 * fTemp9) + fTemp11)) + (fSlow477 * ((fSlow481 * fTemp15) + fTemp13)))) + fTemp16));
			fRec100[0] = max((fRec100[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fbargraph46 = fRec100[0];
			output46[i] = (FAUSTFLOAT)fTemp63;
			double fTemp64 = (0.02017333553791887 * (((fSlow0 * ((fSlow484 * fTemp7) + (fSlow487 * fTemp3))) + ((fSlow482 * ((fSlow488 * fTemp9) + fTemp11)) + (fSlow485 * ((fSlow489 * fTemp15) + fTemp13)))) + fTemp16));
			fRec101[0] = max((fRec101[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fbargraph47 = fRec101[0];
			output47[i] = (FAUSTFLOAT)fTemp64;
			double fTemp65 = (0.02017333553791887 * (((fSlow0 * ((fSlow492 * fTemp3) + (fSlow495 * fTemp7))) + ((fSlow493 * ((fSlow496 * fTemp9) + fTemp11)) + (fSlow490 * ((fSlow497 * fTemp15) + fTemp13)))) + fTemp16));
			fRec102[0] = max((fRec102[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fbargraph48 = fRec102[0];
			output48[i] = (FAUSTFLOAT)fTemp65;
			double fTemp66 = (0.02017333553791887 * ((fTemp16 + ((fSlow498 * ((fSlow500 * fTemp9) + fTemp11)) + (fSlow501 * ((fSlow503 * fTemp15) + fTemp13)))) + (fSlow0 * ((fSlow504 * fTemp3) + (fSlow505 * fTemp7)))));
			fRec103[0] = max((fRec103[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fbargraph49 = fRec103[0];
			output49[i] = (FAUSTFLOAT)fTemp66;
			// post processing
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
			fRec81[1] = fRec81[0];
			fRec80[1] = fRec80[0];
			fRec79[1] = fRec79[0];
			fRec78[1] = fRec78[0];
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
			fRec0[1] = fRec0[0];
			fRec50[1] = fRec50[0];
			fRec54[1] = fRec54[0];
			fRec51[1] = fRec51[0];
			fRec52[1] = fRec52[0];
			fRec53[1] = fRec53[0];
			fRec42[1] = fRec42[0];
			fRec43[1] = fRec43[0];
			fRec49[1] = fRec49[0];
			fRec48[1] = fRec48[0];
			fVec3[1] = fVec3[0];
			fRec47[1] = fRec47[0];
			fRec44[1] = fRec44[0];
			fRec45[1] = fRec45[0];
			fRec46[1] = fRec46[0];
			fRec40[1] = fRec40[0];
			fRec41[1] = fRec41[0];
			fRec37[1] = fRec37[0];
			fRec38[1] = fRec38[0];
			fRec39[1] = fRec39[0];
			fRec35[1] = fRec35[0];
			fRec36[1] = fRec36[0];
			fRec32[1] = fRec32[0];
			fRec33[1] = fRec33[0];
			fRec34[1] = fRec34[0];
			fRec27[1] = fRec27[0];
			fRec31[1] = fRec31[0];
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec30[1] = fRec30[0];
			fRec19[1] = fRec19[0];
			fRec20[1] = fRec20[0];
			fRec26[1] = fRec26[0];
			fRec25[1] = fRec25[0];
			fVec2[1] = fVec2[0];
			fRec24[1] = fRec24[0];
			fRec21[1] = fRec21[0];
			fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			fRec16[1] = fRec16[0];
			fRec18[1] = fRec18[0];
			fRec17[1] = fRec17[0];
			fRec15[1] = fRec15[0];
			fVec1[1] = fVec1[0];
			fRec14[1] = fRec14[0];
			fRec10[1] = fRec10[0];
			fRec13[1] = fRec13[0];
			fRec12[1] = fRec12[0];
			fRec11[1] = fRec11[0];
			fRec7[1] = fRec7[0];
			fRec9[1] = fRec9[0];
			fRec8[1] = fRec8[0];
			fRec6[1] = fRec6[0];
			fVec0[1] = fVec0[0];
			fRec5[1] = fRec5[0];
			fRec1[1] = fRec1[0];
			fRec4[1] = fRec4[0];
			fRec3[1] = fRec3[0];
			fRec2[1] = fRec2[0];
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
