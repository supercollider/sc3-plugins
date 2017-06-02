//----------------------------------------------------------
// name: "HOAPanLebedev262"
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
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOAPanLebedev262");
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
	virtual int getNumOutputs() { return 26; }
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
		ui_interface->openHorizontalBox("Outputs");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph0, "osc", "/output1");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7806b0", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph1, "osc", "/output2");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d782a20", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph2, "osc", "/output3");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7854c0", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph3, "osc", "/output4");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d787fb0", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph4, "osc", "/output5");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d78ab00", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph5, "osc", "/output6");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d78d260", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph6, "osc", "/output7");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d790130", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph7, "osc", "/output8");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d792c10", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph8, "osc", "/output9");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d795740", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph9, "osc", "/output10");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7982f0", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph10, "osc", "/output11");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d79b0c0", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph11, "osc", "/output12");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d79df10", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph12, "osc", "/output13");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7a0e20", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph13, "osc", "/output14");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7a3dd0", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph14, "osc", "/output15");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7a6970", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph15, "osc", "/output16");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7a95b0", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph16, "osc", "/output17");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7ac270", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph17, "osc", "/output18");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7aefb0", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph18, "osc", "/output19");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7b24b0", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph19, "osc", "/output20");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7b5580", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph20, "osc", "/output21");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7b86b0", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph21, "osc", "/output22");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7bb850", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph22, "osc", "/output23");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7be7b0", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph23, "osc", "/output24");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7c1760", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph24, "osc", "/output25");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7c4790", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph25, "osc", "/output26");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fd38d7c7810", &fbargraph25, -7e+01, 6.0);
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
		double 	fSlow128 = (fSlow93 - fSlow81);
		double 	fSlow129 = ((3 * faustpower<2>(fSlow128)) + -1);
		double 	fSlow130 = ((3 * faustpower<2>(fSlow127)) + -1);
		double 	fSlow131 = (fSlow97 - fSlow81);
		double 	fSlow132 = (fSlow99 - fSlow78);
		double 	fSlow133 = ((3 * faustpower<2>(fSlow132)) + -1);
		double 	fSlow134 = ((3 * faustpower<2>(fSlow131)) + -1);
		double 	fSlow135 = (0.8164965809277259 * fSlow103);
		double 	fSlow136 = (0.5773502691896258 * fSlow12);
		double 	fSlow137 = (fSlow135 + fSlow136);
		double 	fSlow138 = ((3 * faustpower<2>(fSlow137)) + -1);
		double 	fSlow139 = (0.8164965809277259 * fSlow105);
		double 	fSlow140 = (0.5773502691896258 * fSlow28);
		double 	fSlow141 = (fSlow139 + fSlow140);
		double 	fSlow142 = ((3 * faustpower<2>(fSlow141)) + -1);
		double 	fSlow143 = (0.8164965809277259 * fSlow109);
		double 	fSlow144 = (fSlow136 + fSlow143);
		double 	fSlow145 = ((3 * faustpower<2>(fSlow144)) + -1);
		double 	fSlow146 = (0.8164965809277259 * fSlow107);
		double 	fSlow147 = (fSlow140 + fSlow146);
		double 	fSlow148 = ((3 * faustpower<2>(fSlow147)) + -1);
		double 	fSlow149 = (0.8164965809277259 * fSlow113);
		double 	fSlow150 = (fSlow140 + fSlow149);
		double 	fSlow151 = (0.8164965809277259 * fSlow111);
		double 	fSlow152 = (fSlow136 + fSlow151);
		double 	fSlow153 = ((3 * faustpower<2>(fSlow152)) + -1);
		double 	fSlow154 = ((3 * faustpower<2>(fSlow150)) + -1);
		double 	fSlow155 = (0.8164965809277259 * fSlow117);
		double 	fSlow156 = (fSlow140 + fSlow155);
		double 	fSlow157 = ((3 * faustpower<2>(fSlow156)) + -1);
		double 	fSlow158 = (0.8164965809277259 * fSlow115);
		double 	fSlow159 = (fSlow136 + fSlow158);
		double 	fSlow160 = ((3 * faustpower<2>(fSlow159)) + -1);
		double 	fSlow161 = (fSlow135 - fSlow136);
		double 	fSlow162 = (fSlow139 - fSlow140);
		double 	fSlow163 = ((3 * faustpower<2>(fSlow162)) + -1);
		double 	fSlow164 = ((3 * faustpower<2>(fSlow161)) + -1);
		double 	fSlow165 = (fSlow146 - fSlow140);
		double 	fSlow166 = (fSlow143 - fSlow136);
		double 	fSlow167 = ((3 * faustpower<2>(fSlow166)) + -1);
		double 	fSlow168 = ((3 * faustpower<2>(fSlow165)) + -1);
		double 	fSlow169 = (fSlow151 - fSlow136);
		double 	fSlow170 = ((3 * faustpower<2>(fSlow169)) + -1);
		double 	fSlow171 = (fSlow149 - fSlow140);
		double 	fSlow172 = ((3 * faustpower<2>(fSlow171)) + -1);
		double 	fSlow173 = (fSlow158 - fSlow136);
		double 	fSlow174 = ((3 * faustpower<2>(fSlow173)) + -1);
		double 	fSlow175 = (fSlow155 - fSlow140);
		double 	fSlow176 = ((3 * faustpower<2>(fSlow175)) + -1);
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
			double fTemp11 = (0.04761904761904762 * ((fTemp2 + (fSlow10 * ((fSlow17 * fTemp4) + (fSlow33 * fTemp6)))) + (fSlow40 * ((fSlow41 * fTemp8) + (fSlow52 * fTemp10)))));
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fbargraph0 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp11;
			double fTemp12 = (0.04761904761904762 * ((fTemp2 + (fSlow40 * ((fSlow59 * fTemp8) + (fSlow60 * fTemp10)))) + (fSlow10 * ((fSlow15 * fTemp4) + (fSlow31 * fTemp6)))));
			fRec29[0] = max((fRec29[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fbargraph1 = fRec29[0];
			output1[i] = (FAUSTFLOAT)fTemp12;
			double fTemp13 = (0.04761904761904762 * ((fSlow10 * ((fSlow61 * fTemp6) + (fSlow62 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow63 * fTemp8) + (fSlow64 * fTemp10))))));
			fRec30[0] = max((fRec30[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fbargraph2 = fRec30[0];
			output2[i] = (FAUSTFLOAT)fTemp13;
			double fTemp14 = (0.04761904761904762 * ((fTemp2 + (fSlow40 * ((fSlow66 * fTemp10) + (fSlow68 * fTemp8)))) + (fSlow10 * ((fSlow65 * fTemp4) + (fSlow67 * fTemp6)))));
			fRec31[0] = max((fRec31[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fbargraph3 = fRec31[0];
			output3[i] = (FAUSTFLOAT)fTemp14;
			double fTemp15 = (0.04761904761904762 * ((fTemp2 + (fSlow40 * ((fSlow70 * fTemp10) + (fSlow72 * fTemp8)))) + (fSlow10 * ((fSlow71 * fTemp6) + (fSlow69 * fTemp4)))));
			fRec32[0] = max((fRec32[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fbargraph4 = fRec32[0];
			output4[i] = (FAUSTFLOAT)fTemp15;
			double fTemp16 = (0.04761904761904762 * ((fSlow10 * ((fSlow73 * fTemp4) + (fSlow74 * fTemp6))) + (fTemp2 + (fSlow40 * ((fSlow75 * fTemp10) + (fSlow76 * fTemp8))))));
			fRec33[0] = max((fRec33[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fbargraph5 = fRec33[0];
			output5[i] = (FAUSTFLOAT)fTemp16;
			double fTemp17 = (0.0380952380952381 * ((fTemp2 + (fSlow10 * ((fSlow79 * fTemp4) + (fSlow82 * fTemp6)))) + (fSlow40 * ((fSlow83 * fTemp10) + (fSlow84 * fTemp8)))));
			fRec34[0] = max((fRec34[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fbargraph6 = fRec34[0];
			output6[i] = (FAUSTFLOAT)fTemp17;
			double fTemp18 = (0.0380952380952381 * ((fTemp2 + (fSlow40 * ((fSlow87 * fTemp8) + (fSlow90 * fTemp10)))) + (fSlow10 * ((fSlow86 * fTemp6) + (fSlow89 * fTemp4)))));
			fRec35[0] = max((fRec35[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fbargraph7 = fRec35[0];
			output7[i] = (FAUSTFLOAT)fTemp18;
			double fTemp19 = (0.0380952380952381 * ((fSlow10 * ((fSlow92 * fTemp4) + (fSlow94 * fTemp6))) + (fTemp2 + (fSlow40 * ((fSlow95 * fTemp10) + (fSlow96 * fTemp8))))));
			fRec36[0] = max((fRec36[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fbargraph8 = fRec36[0];
			output8[i] = (FAUSTFLOAT)fTemp19;
			double fTemp20 = (0.0380952380952381 * ((fSlow10 * ((fSlow98 * fTemp6) + (fSlow100 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow101 * fTemp8) + (fSlow102 * fTemp10))))));
			fRec37[0] = max((fRec37[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph9 = fRec37[0];
			output9[i] = (FAUSTFLOAT)fTemp20;
			double fTemp21 = (0.0380952380952381 * ((fTemp2 + (fSlow40 * ((fSlow104 * fTemp10) + (fSlow106 * fTemp8)))) + (fSlow10 * ((fSlow105 * fTemp6) + (fSlow103 * fTemp4)))));
			fRec38[0] = max((fRec38[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fbargraph10 = fRec38[0];
			output10[i] = (FAUSTFLOAT)fTemp21;
			double fTemp22 = (0.0380952380952381 * ((fTemp2 + (fSlow40 * ((fSlow108 * fTemp8) + (fSlow110 * fTemp10)))) + (fSlow10 * ((fSlow107 * fTemp6) + (fSlow109 * fTemp4)))));
			fRec39[0] = max((fRec39[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fbargraph11 = fRec39[0];
			output11[i] = (FAUSTFLOAT)fTemp22;
			double fTemp23 = (0.0380952380952381 * ((fTemp2 + (fSlow40 * ((fSlow112 * fTemp10) + (fSlow114 * fTemp8)))) + (fSlow10 * ((fSlow113 * fTemp6) + (fSlow111 * fTemp4)))));
			fRec40[0] = max((fRec40[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fbargraph12 = fRec40[0];
			output12[i] = (FAUSTFLOAT)fTemp23;
			double fTemp24 = (0.0380952380952381 * ((fTemp2 + (fSlow40 * ((fSlow116 * fTemp10) + (fSlow118 * fTemp8)))) + (fSlow10 * ((fSlow115 * fTemp4) + (fSlow117 * fTemp6)))));
			fRec41[0] = max((fRec41[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fbargraph13 = fRec41[0];
			output13[i] = (FAUSTFLOAT)fTemp24;
			double fTemp25 = (0.0380952380952381 * ((fSlow10 * ((fSlow119 * fTemp4) + (fSlow120 * fTemp6))) + (fTemp2 + (fSlow40 * ((fSlow121 * fTemp10) + (fSlow122 * fTemp8))))));
			fRec42[0] = max((fRec42[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fbargraph14 = fRec42[0];
			output14[i] = (FAUSTFLOAT)fTemp25;
			double fTemp26 = (0.0380952380952381 * ((fTemp2 + (fSlow40 * ((fSlow124 * fTemp8) + (fSlow126 * fTemp10)))) + (fSlow10 * ((fSlow123 * fTemp6) + (fSlow125 * fTemp4)))));
			fRec43[0] = max((fRec43[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fbargraph15 = fRec43[0];
			output15[i] = (FAUSTFLOAT)fTemp26;
			double fTemp27 = (0.0380952380952381 * (((fSlow10 * ((fSlow127 * fTemp4) + (fSlow128 * fTemp6))) + (fSlow40 * ((fSlow129 * fTemp8) + (fSlow130 * fTemp10)))) + fTemp2));
			fRec44[0] = max((fRec44[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fbargraph16 = fRec44[0];
			output16[i] = (FAUSTFLOAT)fTemp27;
			double fTemp28 = (0.0380952380952381 * (((fSlow10 * ((fSlow131 * fTemp6) + (fSlow132 * fTemp4))) + (fSlow40 * ((fSlow133 * fTemp10) + (fSlow134 * fTemp8)))) + fTemp2));
			fRec45[0] = max((fRec45[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph17 = fRec45[0];
			output17[i] = (FAUSTFLOAT)fTemp28;
			double fTemp29 = (0.03214285714285714 * (((fSlow40 * ((fSlow138 * fTemp10) + (fSlow142 * fTemp8))) + (fSlow10 * ((fSlow137 * fTemp4) + (fSlow141 * fTemp6)))) + fTemp2));
			fRec46[0] = max((fRec46[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph18 = fRec46[0];
			output18[i] = (FAUSTFLOAT)fTemp29;
			double fTemp30 = (0.03214285714285714 * (((fSlow40 * ((fSlow145 * fTemp10) + (fSlow148 * fTemp8))) + (fSlow10 * ((fSlow144 * fTemp4) + (fSlow147 * fTemp6)))) + fTemp2));
			fRec47[0] = max((fRec47[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fbargraph19 = fRec47[0];
			output19[i] = (FAUSTFLOAT)fTemp30;
			double fTemp31 = (0.03214285714285714 * (((fSlow10 * ((fSlow150 * fTemp6) + (fSlow152 * fTemp4))) + (fSlow40 * ((fSlow153 * fTemp10) + (fSlow154 * fTemp8)))) + fTemp2));
			fRec48[0] = max((fRec48[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fbargraph20 = fRec48[0];
			output20[i] = (FAUSTFLOAT)fTemp31;
			double fTemp32 = (0.03214285714285714 * (((fSlow40 * ((fSlow157 * fTemp8) + (fSlow160 * fTemp10))) + (fSlow10 * ((fSlow156 * fTemp6) + (fSlow159 * fTemp4)))) + fTemp2));
			fRec49[0] = max((fRec49[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph21 = fRec49[0];
			output21[i] = (FAUSTFLOAT)fTemp32;
			double fTemp33 = (0.03214285714285714 * (((fSlow10 * ((fSlow161 * fTemp4) + (fSlow162 * fTemp6))) + (fSlow40 * ((fSlow163 * fTemp8) + (fSlow164 * fTemp10)))) + fTemp2));
			fRec50[0] = max((fRec50[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph22 = fRec50[0];
			output22[i] = (FAUSTFLOAT)fTemp33;
			double fTemp34 = (0.03214285714285714 * ((fSlow10 * ((fSlow165 * fTemp6) + (fSlow166 * fTemp4))) + (fTemp2 + (fSlow40 * ((fSlow167 * fTemp10) + (fSlow168 * fTemp8))))));
			fRec51[0] = max((fRec51[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fbargraph23 = fRec51[0];
			output23[i] = (FAUSTFLOAT)fTemp34;
			double fTemp35 = (0.03214285714285714 * ((fTemp2 + (fSlow40 * ((fSlow170 * fTemp10) + (fSlow172 * fTemp8)))) + (fSlow10 * ((fSlow171 * fTemp6) + (fSlow169 * fTemp4)))));
			fRec52[0] = max((fRec52[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fbargraph24 = fRec52[0];
			output24[i] = (FAUSTFLOAT)fTemp35;
			double fTemp36 = (0.03214285714285714 * ((fSlow40 * ((fSlow174 * fTemp10) + (fSlow176 * fTemp8))) + (fTemp2 + (fSlow10 * ((fSlow173 * fTemp4) + (fSlow175 * fTemp6))))));
			fRec53[0] = max((fRec53[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph25 = fRec53[0];
			output25[i] = (FAUSTFLOAT)fTemp36;
			// post processing
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
