//----------------------------------------------------------
// name: "HOAMicLebedev505"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2014"
//
// Code generated with Faust 0.9.95 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with  */
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
	FAUSTFLOAT 	fslider0;
	double 	fRec0[2];
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOAMicLebedev505");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/lebedev.lib/name", "Lebdev grids and weights");
		m->declare("lib/lebedev.lib/version", "1.0");
		m->declare("lib/lebedev.lib/author", "Pierre Lecomte");
		m->declare("lib/lebedev.lib/license", "GPL");
		m->declare("lib/lebedev.lib/copyright", "(c) Pierre Lecomte 2014");
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
		m->declare("math.lib/name", "Faust Math Library");
		m->declare("math.lib/version", "2.0");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("basic.lib/name", "Faust Basic Element Library");
		m->declare("basic.lib/version", "0.0");
		m->declare("signal.lib/name", "Faust Signal Routing Library");
		m->declare("signal.lib/version", "0.0");
	}

	virtual int getNumInputs() { return 50; }
	virtual int getNumOutputs() { return 36; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec0[i] = 0;
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
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("Outputs");
		ui_interface->declare(&fslider0, "1", "");
		ui_interface->declare(&fslider0, "style", "knob");
		ui_interface->declare(&fslider0, "unit", "dB");
		ui_interface->addHorizontalSlider("Gain", &fslider0, 0.0, -1e+01, 5e+01, 0.1);
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider0))));
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
		FAUSTFLOAT* input25 = input[25];
		FAUSTFLOAT* input26 = input[26];
		FAUSTFLOAT* input27 = input[27];
		FAUSTFLOAT* input28 = input[28];
		FAUSTFLOAT* input29 = input[29];
		FAUSTFLOAT* input30 = input[30];
		FAUSTFLOAT* input31 = input[31];
		FAUSTFLOAT* input32 = input[32];
		FAUSTFLOAT* input33 = input[33];
		FAUSTFLOAT* input34 = input[34];
		FAUSTFLOAT* input35 = input[35];
		FAUSTFLOAT* input36 = input[36];
		FAUSTFLOAT* input37 = input[37];
		FAUSTFLOAT* input38 = input[38];
		FAUSTFLOAT* input39 = input[39];
		FAUSTFLOAT* input40 = input[40];
		FAUSTFLOAT* input41 = input[41];
		FAUSTFLOAT* input42 = input[42];
		FAUSTFLOAT* input43 = input[43];
		FAUSTFLOAT* input44 = input[44];
		FAUSTFLOAT* input45 = input[45];
		FAUSTFLOAT* input46 = input[46];
		FAUSTFLOAT* input47 = input[47];
		FAUSTFLOAT* input48 = input[48];
		FAUSTFLOAT* input49 = input[49];
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
		for (int i=0; i<count; i++) {
			double fTemp0 = (double)input5[i];
			double fTemp1 = (double)input4[i];
			double fTemp2 = (double)input3[i];
			double fTemp3 = (double)input2[i];
			double fTemp4 = (double)input0[i];
			double fTemp5 = (double)input1[i];
			double fTemp6 = (double)input17[i];
			double fTemp7 = (double)input16[i];
			double fTemp8 = (double)input15[i];
			double fTemp9 = (double)input14[i];
			double fTemp10 = (double)input13[i];
			double fTemp11 = (double)input12[i];
			double fTemp12 = (double)input11[i];
			double fTemp13 = (double)input10[i];
			double fTemp14 = (double)input9[i];
			double fTemp15 = (double)input8[i];
			double fTemp16 = (double)input6[i];
			double fTemp17 = (double)input7[i];
			double fTemp18 = (fTemp14 + (fTemp15 + (fTemp16 + fTemp17)));
			double fTemp19 = (double)input25[i];
			double fTemp20 = (double)input24[i];
			double fTemp21 = (double)input23[i];
			double fTemp22 = (double)input22[i];
			double fTemp23 = (double)input21[i];
			double fTemp24 = (double)input20[i];
			double fTemp25 = (double)input18[i];
			double fTemp26 = (double)input19[i];
			double fTemp27 = (fTemp25 + fTemp26);
			double fTemp28 = (fTemp23 + (fTemp24 + fTemp27));
			double fTemp29 = (fTemp19 + (fTemp20 + (fTemp21 + (fTemp22 + fTemp28))));
			double fTemp30 = (double)input49[i];
			double fTemp31 = (double)input48[i];
			double fTemp32 = (double)input47[i];
			double fTemp33 = (double)input46[i];
			double fTemp34 = (double)input45[i];
			double fTemp35 = (double)input44[i];
			double fTemp36 = (double)input43[i];
			double fTemp37 = (double)input42[i];
			double fTemp38 = (double)input41[i];
			double fTemp39 = (double)input40[i];
			double fTemp40 = (double)input39[i];
			double fTemp41 = (double)input38[i];
			double fTemp42 = (double)input37[i];
			double fTemp43 = (double)input36[i];
			double fTemp44 = (double)input35[i];
			double fTemp45 = (double)input34[i];
			double fTemp46 = (double)input33[i];
			double fTemp47 = (double)input32[i];
			double fTemp48 = (double)input31[i];
			double fTemp49 = (double)input30[i];
			double fTemp50 = (double)input29[i];
			double fTemp51 = (double)input28[i];
			double fTemp52 = (double)input26[i];
			double fTemp53 = (double)input27[i];
			double fTemp54 = (fTemp52 + fTemp53);
			double fTemp55 = (fTemp50 + (fTemp51 + fTemp54));
			fRec0[0] = (fSlow0 + (0.999 * fRec0[1]));
			output0[i] = (FAUSTFLOAT)(((((0.012698412698412698 * (fTemp0 + (fTemp1 + (fTemp2 + (fTemp3 + (fTemp4 + fTemp5)))))) + (0.022574955908289243 * (fTemp6 + (fTemp7 + (fTemp8 + (fTemp9 + (fTemp10 + (fTemp11 + (fTemp12 + (fTemp13 + fTemp18)))))))))) + (0.02109375 * fTemp29)) + (0.02017333553791887 * (fTemp30 + (fTemp31 + (fTemp32 + (fTemp33 + (fTemp34 + (fTemp35 + (fTemp36 + (fTemp37 + (fTemp38 + (fTemp39 + (fTemp40 + (fTemp41 + (fTemp42 + (fTemp43 + (fTemp44 + (fTemp45 + (fTemp46 + (fTemp47 + (fTemp48 + (fTemp49 + fTemp55)))))))))))))))))))))) * fRec0[0]);
			double fTemp56 = (fTemp15 + fTemp7);
			double fTemp57 = (fTemp3 - fTemp1);
			double fTemp58 = (fTemp13 - fTemp11);
			double fTemp59 = (fTemp26 + fTemp21);
			double fTemp60 = (fTemp49 + fTemp41);
			double fTemp61 = (fTemp46 + fTemp38);
			double fTemp62 = (fTemp51 + fTemp31);
			double fTemp63 = ((fTemp33 + fTemp54) + fTemp32);
			double fTemp64 = (fTemp24 + fTemp20);
			double fTemp65 = (fTemp25 + fTemp22);
			double fTemp66 = (fTemp64 - fTemp65);
			double fTemp67 = (fTemp48 + fTemp47);
			double fTemp68 = (fTemp50 + fTemp30);
			double fTemp69 = (fTemp42 + fTemp34);
			double fTemp70 = (fTemp23 + fTemp19);
			double fTemp71 = (fTemp45 + fTemp37);
			output1[i] = (FAUSTFLOAT)(fRec0[0] * ((((((3.3859722305960555e-18 * fTemp56) + (((2.693524415809287e-18 * fTemp2) + (0.0219942959691286 * fTemp57)) + (0.0276485614705685 * fTemp58))) + (0.021093749999999998 * fTemp59)) + (0.010535180889746094 * fTemp60)) + (0.010535180889746096 * fTemp61)) - ((0.010535180889746065 * (fTemp62 - fTemp63)) + ((0.021093749999999994 * fTemp66) + ((0.027648561470568506 * ((fTemp6 + (fTemp14 + fTemp10)) - (fTemp8 + (fTemp17 + fTemp12)))) + ((0.03160554266923824 * ((fTemp35 + (fTemp36 + (fTemp44 + fTemp43))) - (fTemp39 + (fTemp40 + fTemp67)))) + ((0.010535180889746067 * fTemp68) + ((0.010535180889746113 * fTemp69) + ((0.02109375 * fTemp70) + (0.01053518088974609 * fTemp71))))))))));
			double fTemp72 = (fTemp4 - fTemp0);
			double fTemp73 = (fTemp42 + (fTemp43 + (fTemp44 + (fTemp45 + (fTemp46 + (fTemp47 + (fTemp49 + fTemp48)))))));
			double fTemp74 = ((fTemp34 + (fTemp35 + (fTemp36 + (fTemp37 + (fTemp38 + (fTemp39 + (fTemp41 + fTemp40))))))) - fTemp73);
			double fTemp75 = ((fTemp19 + (fTemp20 + (fTemp22 + fTemp21))) - fTemp28);
			double fTemp76 = ((fTemp6 + (fTemp7 + (fTemp9 + fTemp8))) - fTemp18);
			double fTemp77 = ((fTemp30 + (fTemp31 + (fTemp33 + fTemp32))) - fTemp55);
			output2[i] = (FAUSTFLOAT)(fRec0[0] * ((0.0219942959691286 * fTemp72) - ((0.010535180889746075 * fTemp74) + (((0.02109375 * fTemp75) + (0.0276485614705685 * fTemp76)) + (0.03160554266923825 * fTemp77)))));
			double fTemp78 = (fTemp70 - fTemp59);
			double fTemp79 = (fTemp43 + fTemp35);
			double fTemp80 = (fTemp48 + fTemp40);
			double fTemp81 = (fTemp17 + fTemp8);
			double fTemp82 = (fTemp5 - fTemp2);
			double fTemp83 = ((fTemp37 + (fTemp38 + (fTemp46 + fTemp45))) - fTemp60);
			double fTemp84 = (fTemp53 + fTemp32);
			double fTemp85 = (fTemp52 + fTemp50);
			double fTemp86 = (fTemp84 - (fTemp30 + (fTemp33 + fTemp85)));
			double fTemp87 = (fTemp44 + fTemp36);
			double fTemp88 = (fTemp47 + fTemp39);
			double fTemp89 = (fTemp14 + fTemp6);
			double fTemp90 = (fTemp12 - fTemp10);
			output3[i] = (FAUSTFLOAT)(fRec0[0] * (((0.021093749999999994 * fTemp78) + ((0.010535180889746074 * fTemp79) + ((0.01053518088974608 * fTemp80) + ((0.021093749999999998 * fTemp65) + ((1.6929861152980278e-18 * fTemp81) + ((1.3467622079046435e-18 * (fTemp0 + (fTemp4 + fTemp3))) + ((0.0219942959691286 * fTemp82) + (0.03160554266923823 * fTemp69)))))))) - ((0.03160554266923824 * fTemp83) + ((0.027648561470568506 * ((fTemp7 + (fTemp15 + fTemp11)) - (fTemp9 + (fTemp16 + fTemp13)))) + ((0.010535180889746065 * fTemp86) + ((0.010535180889746086 * fTemp87) + ((0.010535180889746079 * fTemp88) + ((0.02109375 * fTemp64) + ((5.078958345894083e-18 * fTemp89) + ((0.0276485614705685 * fTemp90) + ((0.010535180889746067 * fTemp62) + (4.040286623713931e-18 * fTemp1))))))))))));
			double fTemp91 = (fTemp52 + fTemp51);
			double fTemp92 = (fTemp53 + fTemp50);
			double fTemp93 = ((fTemp31 + (fTemp33 + fTemp91)) - (fTemp30 + (fTemp32 + fTemp92)));
			double fTemp94 = (fTemp60 - fTemp61);
			double fTemp95 = (fTemp25 + fTemp24);
			double fTemp96 = (fTemp26 + fTemp23);
			double fTemp97 = ((fTemp20 + (fTemp22 + fTemp95)) - (fTemp19 + (fTemp21 + fTemp96)));
			double fTemp98 = ((fTemp13 + fTemp11) - (fTemp12 + fTemp10));
			output4[i] = (FAUSTFLOAT)(fRec0[0] * (((0.007102817506892165 * fTemp93) + ((0.021308452520676584 * fTemp94) + ((0.027231914153020897 * fTemp97) + ((0.043716214137085485 * fTemp98) + ((0.021308452520676563 * fTemp87) + ((0.021308452520676556 * fTemp80) + ((8.030538257073304e-18 * fTemp89) + ((2.6768460856911014e-18 * fTemp81) + (((3.0114518464024883e-18 * fTemp3) + (9.034355539207464e-18 * fTemp1)) + (0.02130845252067657 * fTemp71)))))))))) - ((0.021308452520676542 * fTemp79) + ((0.021308452520676553 * fTemp88) + (((6.0229036928049765e-18 * fTemp2) + (5.353692171382203e-18 * fTemp56)) + (0.021308452520676615 * fTemp69))))));
			double fTemp99 = (fTemp15 - fTemp7);
			double fTemp100 = (fTemp49 - fTemp41);
			double fTemp101 = (fTemp46 - fTemp38);
			double fTemp102 = ((fTemp48 + fTemp36) - (fTemp44 + fTemp40));
			double fTemp103 = ((fTemp47 + fTemp35) - (fTemp43 + fTemp39));
			double fTemp104 = (fTemp24 + fTemp22);
			double fTemp105 = (fTemp104 - (fTemp25 + fTemp20));
			double fTemp106 = ((fTemp23 + fTemp21) - (fTemp26 + fTemp19));
			double fTemp107 = ((fTemp14 + fTemp8) - (fTemp17 + fTemp6));
			double fTemp108 = (fTemp50 - fTemp30);
			double fTemp109 = (fTemp45 - fTemp37);
			double fTemp110 = (fTemp42 - fTemp34);
			output5[i] = (FAUSTFLOAT)(fRec0[0] * ((((((5.353692171382201e-18 * fTemp99) + (0.007102817506892191 * fTemp100)) + (0.007102817506892193 * fTemp101)) + (0.021308452520676546 * fTemp102)) + (0.021308452520676553 * fTemp103)) - (((0.027231914153020904 * fTemp105) + ((0.02723191415302091 * fTemp106) + ((0.043716214137085485 * fTemp107) + ((0.02130845252067653 * fTemp108) + ((0.007102817506892187 * fTemp109) + (0.007102817506892205 * fTemp110)))))) + (0.021308452520676528 * ((fTemp32 + (fTemp51 + fTemp33)) - (fTemp31 + fTemp54))))));
			double fTemp111 = (fTemp4 + fTemp0);
			double fTemp112 = ((((fTemp9 + fTemp18) + fTemp8) + fTemp7) + fTemp6);
			double fTemp113 = ((((fTemp33 + fTemp55) + fTemp32) + fTemp31) + fTemp30);
			double fTemp114 = (((fTemp5 + fTemp3) + fTemp2) + fTemp1);
			double fTemp115 = (((fTemp13 + fTemp12) + fTemp11) + fTemp10);
			double fTemp116 = ((((((((fTemp41 + fTemp73) + fTemp40) + fTemp39) + fTemp38) + fTemp37) + fTemp36) + fTemp35) + fTemp34);
			output6[i] = (FAUSTFLOAT)((((5.236595479505967e-18 * fTemp29) + (((0.02839451399999733 * fTemp111) + (0.012619783999998805 * fTemp112)) + (0.03280650879687195 * fTemp113))) - (((0.014197256999998666 * fTemp114) + (0.02523956799999763 * fTemp115)) + (0.01640325439843596 * fTemp116))) * fRec0[0]);
			double fTemp117 = (fTemp43 - fTemp35);
			double fTemp118 = (fTemp48 - fTemp40);
			double fTemp119 = (fTemp17 - fTemp8);
			double fTemp120 = ((fTemp15 + fTemp9) - (fTemp16 + fTemp7));
			double fTemp121 = (fTemp46 + fTemp41);
			double fTemp122 = (fTemp121 - (fTemp49 + fTemp38));
			double fTemp123 = (fTemp51 - fTemp31);
			double fTemp124 = (fTemp44 - fTemp36);
			double fTemp125 = (fTemp47 - fTemp39);
			double fTemp126 = (fTemp14 - fTemp6);
			double fTemp127 = ((fTemp30 + (fTemp53 + fTemp33)) - (fTemp32 + fTemp85));
			output7[i] = (FAUSTFLOAT)(fRec0[0] * (((0.027231914153020904 * fTemp106) + ((3.0114518464024883e-18 * fTemp72) + ((0.0071028175068921775 * fTemp117) + ((0.007102817506892183 * fTemp118) + ((2.6768460856911007e-18 * fTemp119) + (0.02130845252067654 * fTemp110)))))) - (((0.043716214137085485 * fTemp120) + ((0.021308452520676542 * fTemp122) + ((0.02723191415302091 * fTemp105) + ((0.02130845252067653 * fTemp123) + ((0.007102817506892185 * fTemp124) + ((0.007102817506892181 * fTemp125) + ((8.0305382570733e-18 * fTemp126) + (0.021308452520676546 * fTemp109)))))))) + (0.021308452520676528 * fTemp127))));
			double fTemp128 = ((fTemp5 + fTemp2) - (fTemp3 + fTemp1));
			double fTemp129 = (fTemp16 + fTemp15);
			double fTemp130 = (fTemp17 + fTemp14);
			double fTemp131 = ((fTemp7 + (fTemp9 + fTemp129)) - (fTemp6 + (fTemp8 + fTemp130)));
			double fTemp132 = (fTemp52 + fTemp33);
			output8[i] = (FAUSTFLOAT)(fRec0[0] * (((0.024590370452110585 * fTemp128) + ((0.02185810706854275 * fTemp131) + ((8.337369125538138e-18 * fTemp64) + ((1.6674738251076273e-18 * fTemp65) + ((0.028411270027568696 * fTemp69) + ((0.02841127002756873 * fTemp71) + ((0.028411270027568717 * fTemp61) + ((0.028411270027568724 * fTemp60) + ((1.3384230428455503e-17 * fTemp11) + ((2.6768460856911007e-18 * fTemp13) + ((9.219912161207984e-35 * fTemp111) + (2.1746106811858183e-18 * fTemp62)))))))))))) - ((0.028411270027568748 * fTemp79) + (((0.028411270027568745 * fTemp88) + ((0.02841127002756874 * fTemp80) + ((3.044454953660145e-18 * fTemp68) + ((1.167231677575339e-17 * fTemp70) + ((5.002421475322882e-18 * fTemp59) + ((((8.0305382570733e-18 * fTemp12) + (1.8737922599837705e-17 * fTemp10)) + (1.1422201709353029e-18 * fTemp132)) + (1.3047664087114909e-18 * fTemp84))))))) + (0.028411270027568734 * fTemp87)))));
			double fTemp133 = (fTemp60 - fTemp71);
			double fTemp134 = (6.133424905712859e-18 * fTemp56);
			double fTemp135 = (fTemp81 - fTemp89);
			output9[i] = (FAUSTFLOAT)(fRec0[0] * (((0.03007122090474782 * fTemp133) + ((0.020818537549440803 * fTemp79) + ((0.020818537549440772 * fTemp87) + ((0.016982082124440745 * fTemp65) + ((0.03007122090474784 * fTemp61) + ((0.01698208212444074 * fTemp59) + (fTemp134 + ((0.033388810063348796 * fTemp12) + ((0.0333888100633488 * fTemp13) + ((9.75821927113805e-18 * fTemp2) + (0.002313170838826743 * fTemp63))))))))))) - ((0.01669440503167441 * fTemp135) + ((0.02081853754944079 * fTemp88) + ((0.020818537549440783 * fTemp80) + ((0.016982082124440766 * fTemp64) + ((0.0023131708388267444 * fTemp68) + ((0.030071220904747842 * fTemp69) + ((0.01698208212444075 * fTemp70) + ((0.026560635762986527 * fTemp57) + ((0.03338881006334882 * fTemp10) + ((0.03338881006334885 * fTemp11) + (0.002313170838826747 * fTemp62)))))))))))));
			double fTemp136 = ((fTemp19 + (fTemp21 + fTemp95)) - (fTemp20 + (fTemp22 + fTemp96)));
			double fTemp137 = ((fTemp30 + (fTemp32 + fTemp91)) - (fTemp31 + (fTemp33 + fTemp92)));
			output10[i] = (FAUSTFLOAT)(fRec0[0] * (((((0.016998264729033874 * fTemp124) + ((0.016998264729033867 * fTemp118) + ((5.007920464891509e-18 * fTemp119) + ((1.5023761394674526e-17 * fTemp126) + (0.01699826472903388 * fTemp109))))) + (0.041597435974919175 * fTemp136)) + (0.016998264729033836 * fTemp137)) - ((0.016998264729033888 * fTemp122) + (((0.016998264729033864 * fTemp125) + ((1.0015840929783018e-17 * fTemp99) + (0.016998264729033916 * fTemp110))) + (0.016998264729033857 * fTemp117)))));
			double fTemp138 = (fTemp80 - fTemp87);
			double fTemp139 = (fTemp88 - fTemp79);
			output11[i] = (FAUSTFLOAT)(fRec0[0] * (((0.03879429159750975 * fTemp135) + ((0.005375323281425214 * fTemp69) + ((0.005375323281425202 * fTemp71) + ((0.013154264250377147 * fTemp59) + ((4.750930503007937e-18 * fTemp56) + (0.030460165261409405 * fTemp63)))))) - ((0.01612596984427559 * fTemp138) + ((0.016125969844275594 * fTemp139) + ((0.013154264250377145 * fTemp66) + ((0.02586286106500651 * fTemp90) + ((0.030460165261409412 * fTemp68) + ((0.005375323281425206 * fTemp61) + ((0.005375323281425205 * fTemp60) + ((0.013154264250377148 * fTemp70) + ((0.025862861065006505 * fTemp58) + ((0.020573779994945588 * fTemp57) + ((2.5195613817171963e-18 * fTemp2) + (0.030460165261409402 * fTemp62))))))))))))));
			output12[i] = (FAUSTFLOAT)(fRec0[0] * (((((0.03359684204526464 * fTemp72) + (0.010558468816293319 * fTemp76)) + (0.021480823570105497 * fTemp75)) + (0.020481687709769415 * fTemp74)) - (0.0263335984839893 * fTemp77)));
			double fTemp140 = ((fTemp16 + fTemp9) - fTemp56);
			output13[i] = (FAUSTFLOAT)(fRec0[0] * (((0.03879429159750975 * fTemp140) + ((0.01612596984427559 * fTemp83) + ((0.013154264250377145 * fTemp78) + ((0.0053753232814252 * fTemp87) + ((0.005375323281425197 * fTemp88) + ((0.013154264250377147 * fTemp65) + ((2.3754652515039685e-18 * fTemp81) + (((3.779342072575794e-18 * fTemp1) + (5.0391227634343926e-18 * fTemp111)) + (0.025862861065006505 * fTemp90))))))))) - ((0.030460165261409402 * fTemp86) + ((0.005375323281425194 * fTemp79) + ((0.005375323281425198 * fTemp80) + ((0.013154264250377148 * fTemp64) + ((7.126395754511905e-18 * fTemp89) + ((0.020573779994945588 * fTemp82) + ((0.016125969844275587 * fTemp69) + ((0.02586286106500651 * fTemp58) + ((1.2597806908585981e-18 * fTemp3) + (0.030460165261409412 * fTemp62))))))))))));
			double fTemp141 = ((fTemp6 + (fTemp8 + fTemp129)) - (fTemp7 + (fTemp9 + fTemp130)));
			double fTemp142 = (fTemp24 - fTemp20);
			double fTemp143 = (fTemp25 - fTemp22);
			double fTemp144 = (fTemp53 - fTemp32);
			double fTemp145 = (fTemp52 - fTemp33);
			double fTemp146 = (fTemp23 - fTemp19);
			double fTemp147 = (fTemp26 - fTemp21);
			output14[i] = (FAUSTFLOAT)(fRec0[0] * (((0.04089277388695433 * fTemp141) + ((0.022664352972045162 * ((fTemp40 + fTemp39) - fTemp67)) + ((2.4393594688416384e-34 * fTemp72) + ((1.273554170485543e-17 * fTemp142) + ((2.5471083409710867e-18 * fTemp143) + ((((0.02266435297204515 * fTemp109) + ((0.022664352972045144 * fTemp100) + (0.022664352972045138 * fTemp101))) + (0.02266435297204512 * fTemp110)) + (5.204217622867659e-18 * fTemp123))))))) - ((3.1225305737205953e-18 * fTemp144) + ((2.7335294515958808e-18 * fTemp145) + ((1.7829758386797603e-17 * fTemp146) + ((7.641325022913258e-18 * fTemp147) + ((0.022664352972045165 * fTemp117) + ((0.022664352972045155 * fTemp124) + (7.285904672014723e-18 * fTemp108)))))))));
			double fTemp148 = (3.0667124528564294e-18 * fTemp81);
			double fTemp149 = (fTemp25 + fTemp23);
			double fTemp150 = (fTemp19 + (fTemp22 + fTemp149));
			double fTemp151 = (fTemp13 + fTemp10);
			output15[i] = (FAUSTFLOAT)(fRec0[0] * (((0.01669440503167441 * fTemp140) + ((0.0300712209047478 * fTemp87) + ((0.03007122090474779 * fTemp88) + ((0.01698208212444072 * fTemp64) + ((9.20013735856929e-18 * fTemp89) + ((0.026560635762986527 * fTemp82) + ((0.02081853754944071 * fTemp69) + ((0.02081853754944074 * fTemp60) + ((0.01698208212444075 * fTemp59) + ((0.0023131708388267444 * fTemp84) + ((0.03338881006334876 * fTemp11) + ((0.03338881006334882 * fTemp12) + ((1.4637328906707076e-17 * fTemp1) + ((6.097910338853647e-51 * fTemp111) + (0.002313170838826741 * fTemp62))))))))))))))) - ((0.03007122090474778 * fTemp79) + ((0.030071220904747797 * fTemp80) + (fTemp148 + (((0.020818537549440744 * fTemp71) + ((0.020818537549440724 * fTemp61) + ((0.01698208212444074 * fTemp150) + ((0.0333888100633488 * fTemp151) + ((4.879109635569025e-18 * fTemp3) + (0.002313170838826745 * fTemp132)))))) + (0.0023131708388267436 * fTemp68)))))));
			output16[i] = (FAUSTFLOAT)(fRec0[0] * (((0.035508216952135595 * (fTemp88 - fTemp80)) + ((0.035508216952135616 * fTemp94) + ((0.03550821695213559 * fTemp79) + ((1.2735541704855428e-17 * fTemp64) + ((2.5471083409710855e-18 * fTemp65) + ((1.2683132068209864e-18 * fTemp68) + ((1.7829758386797597e-17 * fTemp70) + ((7.641325022913257e-18 * fTemp59) + ((5.435628029232799e-19 * fTemp84) + ((4.293397433999001e-17 * fTemp10) + ((3.0667124528564293e-17 * fTemp11) + ((1.8400274717138574e-17 * fTemp12) + ((6.133424905712858e-18 * fTemp13) + (9.059380048721333e-19 * fTemp62)))))))))))))) - ((0.03550821695213561 * (fTemp87 - fTemp71)) + ((9.200137358569287e-18 * fTemp89) + (fTemp148 + ((0.03550821695213564 * fTemp69) + (fTemp134 + ((((6.900103018926964e-18 * fTemp3) + (1.3800206037853928e-17 * fTemp2)) + (2.0700309056780893e-17 * fTemp1)) + (4.758463994196736e-19 * fTemp132)))))))));
			output17[i] = (FAUSTFLOAT)(fRec0[0] * (((0.027200442744256398 * ((fTemp49 + fTemp37) - (fTemp45 + fTemp41))) + ((0.035414181017315374 * fTemp107) + ((0.006277025248674523 * fTemp144) + ((0.006277025248674522 * fTemp145) + ((0.02941382905783859 * fTemp143) + ((0.02941382905783858 * fTemp147) + ((0.018831075746023662 * fTemp117) + ((0.01883107574602363 * fTemp124) + ((1.3010959028184069e-17 * fTemp99) + (0.027200442744256408 * fTemp101)))))))))) - ((0.029413829057838594 * fTemp146) + ((0.02941382905783863 * fTemp142) + (((0.01883107574602365 * fTemp125) + ((0.01883107574602364 * fTemp118) + ((0.027200442744256415 * fTemp110) + (0.006277025248674527 * fTemp108)))) + (0.0062770252486745335 * fTemp123))))));
			output18[i] = (FAUSTFLOAT)(fRec0[0] * (((0.02907849643359095 * fTemp93) + ((0.031444705933590805 * fTemp97) + ((0.006710422253905624 * fTemp79) + ((0.006710422253905626 * fTemp88) + ((1.738662534172072e-17 * fTemp89) + ((5.795541780573573e-18 * fTemp81) + ((5.215987602516216e-18 * fTemp2) + (0.006710422253905646 * fTemp69)))))))) - ((0.006710422253905637 * fTemp94) + ((0.037859351999996446 * fTemp98) + ((0.00671042225390563 * fTemp87) + ((0.006710422253905627 * fTemp80) + ((0.0067104222539056315 * fTemp71) + (((2.607993801258108e-18 * fTemp3) + (7.823981403774323e-18 * fTemp1)) + (1.1591083561147146e-17 * fTemp56)))))))));
			output19[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.011117382399029452 * fTemp146) + (((1.6392267574774099e-18 * fTemp99) + (0.010280801007450529 * fTemp109)) + (0.010280801007450555 * fTemp110))) + (0.03558738810271335 * ((fTemp30 + fTemp54) - (fTemp32 + (fTemp50 + fTemp33))))) - ((0.01111738239902945 * ((fTemp20 + fTemp27) - (fTemp21 + fTemp104))) + ((0.013385302265262959 * fTemp107) + ((0.03084240302235157 * fTemp103) + ((0.030842403022351562 * fTemp102) + ((0.03558738810271334 * fTemp123) + ((0.010280801007450534 * fTemp100) + (0.010280801007450538 * fTemp101)))))))));
			output20[i] = (FAUSTFLOAT)(((((((0.014285714285714285 * fTemp114) + (0.0380952380952381 * fTemp111)) + (0.025396825396825397 * fTemp115)) + (0.004251405423280436 * fTemp116)) + (0.014254712301587407 * fTemp113)) - ((0.027513227513227514 * fTemp112) + (0.024609375000000013 * fTemp29))) * fRec0[0]);
			output21[i] = (FAUSTFLOAT)(fRec0[0] * (((0.01111738239902945 * ((fTemp19 + (fTemp26 + fTemp22)) - (fTemp21 + fTemp149))) + ((0.03084240302235156 * fTemp122) + ((7.376520408648356e-18 * fTemp72) + ((0.011117382399029452 * fTemp142) + (((0.010280801007450517 * fTemp125) + ((8.196133787387049e-19 * fTemp119) + (0.030842403022351562 * fTemp109))) + (0.010280801007450525 * fTemp124)))))) - ((0.03558738810271334 * fTemp127) + ((0.013385302265262959 * fTemp120) + ((0.03558738810271335 * fTemp123) + ((0.010280801007450515 * fTemp117) + ((0.010280801007450522 * fTemp118) + ((2.4588401362161147e-18 * fTemp126) + (0.030842403022351552 * fTemp110)))))))));
			double fTemp152 = (fTemp38 + (fTemp41 + (fTemp49 + fTemp46)));
			output22[i] = (FAUSTFLOAT)(fRec0[0] * (((0.04732418999999555 * fTemp131) + ((0.00894722967187417 * (fTemp36 + (fTemp40 + (fTemp48 + fTemp44)))) + ((0.008947229671874174 * fTemp79) + ((0.008947229671874172 * fTemp88) + ((9.62716461792544e-18 * fTemp64) + ((1.925432923585088e-18 * fTemp65) + ((1.6227516985606004e-17 * fTemp10) + ((6.954650136688288e-18 * fTemp12) + ((4.79080689136032e-34 * fTemp111) + (8.902721895353721e-18 * fTemp62)))))))))) - ((0.021295885499998 * fTemp128) + ((0.008947229671874163 * fTemp152) + ((1.246381065349521e-17 * fTemp68) + ((0.008947229671874156 * fTemp69) + ((0.008947229671874167 * fTemp71) + ((1.3478030465095617e-17 * fTemp70) + ((5.776298770755264e-18 * fTemp59) + ((5.341633137212233e-18 * fTemp84) + ((4.676178873339891e-18 * fTemp132) + ((2.3182167122294292e-18 * fTemp13) + (1.1591083561147147e-17 * fTemp11)))))))))))));
			output23[i] = (FAUSTFLOAT)(fRec0[0] * (((((0.006277025248674527 * fTemp144) + (((((0.027200442744256377 * fTemp124) + ((0.027200442744256363 * fTemp125) + (((0.018831075746023607 * fTemp100) + (0.018831075746023575 * fTemp110)) + (1.9516438542276106e-17 * fTemp126)))) + (0.006277025248674517 * fTemp123)) + (0.029413829057838594 * fTemp147)) + (0.02941382905783855 * fTemp142))) + (1.829373101656094e-50 * fTemp72)) + (0.029413829057838584 * ((fTemp22 + fTemp19) - fTemp149))) - ((0.035414181017315374 * fTemp120) + ((0.0062770252486745275 * fTemp145) + ((0.027200442744256356 * fTemp117) + ((0.027200442744256374 * fTemp118) + ((((0.018831075746023586 * fTemp101) + (0.01883107574602361 * fTemp109)) + (0.006277025248674525 * fTemp108)) + (6.5054795140920346e-18 * fTemp119))))))));
			output24[i] = (FAUSTFLOAT)(((((0.0103565632777062 * fTemp87) + ((0.010356563277706236 * fTemp88) + ((0.010356563277706217 * fTemp80) + (((0.010356563277706178 * fTemp71) + ((0.010356563277706144 * fTemp61) + ((((0.028171808490950554 * fTemp114) + (3.9603917912335094e-67 * fTemp111)) + (0.012520803773755805 * fTemp112)) + (0.010356563277706152 * fTemp60)))) + (0.010356563277706062 * fTemp69))))) + (0.010356563277706251 * fTemp79)) - ((0.02079871798745958 * fTemp29) + ((0.05008321509502321 * fTemp115) + (0.001479509039672308 * fTemp113)))) * fRec0[0]);
			output25[i] = (FAUSTFLOAT)(fRec0[0] * (((0.009285676600414948 * fTemp135) + ((0.0014035855152518362 * fTemp79) + ((0.001403585515251904 * fTemp87) + ((0.012594249175901172 * fTemp64) + ((0.0004678618384172915 * fTemp68) + ((0.012594249175901195 * fTemp70) + ((5.6858370633078216e-18 * fTemp56) + ((0.029546842014263944 * fTemp57) + ((0.037142706401659856 * fTemp10) + ((0.037142706401659786 * fTemp11) + ((1.8092222748840437e-17 * fTemp2) + (0.00046786183841729064 * fTemp62)))))))))))) - (((((0.012594249175901169 * fTemp65) + (((0.012594249175901188 * fTemp59) + ((((0.03714270640165978 * fTemp13) + (0.03714270640165983 * fTemp12)) + (0.00046786183841729075 * fTemp132)) + (0.00046786183841729124 * fTemp84))) + (0.03696108523496621 * fTemp69))) + (0.0014035855152518813 * fTemp80)) + (0.0014035855152518585 * fTemp88)) + (0.03696108523496622 * (fTemp71 - fTemp152)))));
			output26[i] = (FAUSTFLOAT)(fRec0[0] * ((((3.4141388008531875e-17 * fTemp146) + ((2.4386705720379917e-17 * fTemp142) + ((4.877341144075983e-18 * fTemp143) + ((1.4632023432227946e-17 * fTemp147) + ((2.7178140146164e-18 * fTemp123) + ((0.03550821695213557 * fTemp117) + ((0.035508216952135574 * fTemp125) + (3.80493962046296e-18 * fTemp108)))))))) + (1.63068840876984e-18 * fTemp144)) - ((((0.03550821695213559 * fTemp124) + ((0.03550821695213558 * fTemp118) + ((7.19207820986228e-18 * fTemp119) + ((2.1576234629586837e-17 * fTemp126) + ((1.438415641972456e-17 * fTemp99) + (0.03550821695213562 * fTemp110)))))) + (1.427539198259021e-18 * fTemp145)) + (0.035508216952135595 * ((fTemp37 + fTemp121) - (fTemp38 + (fTemp49 + fTemp45)))))));
			output27[i] = (FAUSTFLOAT)(fRec0[0] * (((0.0031385126243372867 * fTemp88) + ((0.003138512624337286 * fTemp80) + ((0.028161597282885743 * fTemp65) + ((0.004533407124042753 * fTemp69) + ((0.028161597282885732 * fTemp59) + ((1.779948545556168e-17 * fTemp56) + ((0.02202291575478033 * fTemp57) + ((0.012205326872422689 * fTemp84) + (((0.027684538794142663 * fTemp11) + (0.02768453879414263 * fTemp10)) + (0.012205326872422687 * fTemp132)))))))))) - ((0.04844794288974959 * fTemp135) + ((0.00453340712404275 * fTemp133) + ((0.0031385126243372885 * fTemp79) + ((0.003138512624337284 * fTemp87) + ((0.02816159728288578 * fTemp64) + ((0.012205326872422696 * fTemp68) + ((0.0045334071240427515 * fTemp61) + ((0.028161597282885746 * fTemp70) + ((0.027684538794142618 * fTemp12) + ((0.027684538794142628 * fTemp13) + ((8.091087986095064e-18 * fTemp2) + (0.01220532687242271 * fTemp62))))))))))))));
			output28[i] = (FAUSTFLOAT)(fRec0[0] * (((0.04100135723151817 * fTemp137) + ((1.531697961461448e-17 * fTemp136) + ((0.020500678615759146 * fTemp122) + ((0.020500678615759105 * fTemp117) + ((0.020500678615759115 * fTemp125) + ((4.152348290496825e-18 * fTemp119) + ((1.2457044871490476e-17 * fTemp126) + (0.020500678615759178 * fTemp110)))))))) - (((0.02050067861575912 * fTemp118) + ((8.30469658099365e-18 * fTemp99) + (0.020500678615759133 * fTemp109))) + (0.02050067861575913 * fTemp124))));
			output29[i] = (FAUSTFLOAT)(fRec0[0] * (((0.02607257280619568 * fTemp66) + (((((((2.496963962185008e-18 * fTemp2) + (0.020389258061373223 * fTemp57)) + (0.025630902468550196 * fTemp58)) + (0.02607257280619569 * fTemp70)) + (0.0009685660236671092 * fTemp71)) + (0.0009685660236671116 * fTemp69)) + (0.0256309024685502 * fTemp90))) - ((0.03519123219323861 * (fTemp68 - fTemp63)) + ((0.0029056980710013254 * fTemp138) + ((0.002905698071001326 * fTemp139) + ((0.01922317685141265 * fTemp135) + (((((2.3541602000525996e-18 * fTemp56) + (0.0351912321932386 * fTemp62)) + (0.026072572806195685 * fTemp59)) + (0.0009685660236671096 * fTemp60)) + (0.00096856602366711 * fTemp61))))))));
			output30[i] = (FAUSTFLOAT)(fRec0[0] * (((((0.02812594988253908 * fTemp76) + (0.042115870353719366 * fTemp72)) + (0.0067319093514117785 * fTemp75)) + (0.0007502480158729096 * fTemp77)) - (0.023090966710758377 * fTemp74)));
			output31[i] = (FAUSTFLOAT)(fRec0[0] * (((0.0009685660236671088 * fTemp87) + (((0.02607257280619569 * fTemp64) + ((3.5312403000788995e-18 * fTemp89) + ((0.020389258061373223 * fTemp82) + ((((1.248481981092504e-18 * fTemp3) + (9.987855848740033e-18 * fTemp111)) + (0.0256309024685502 * fTemp58)) + (0.0029056980710013254 * fTemp71))))) + (0.0009685660236671081 * fTemp88))) - ((0.01922317685141265 * fTemp140) + ((0.0029056980710013246 * fTemp94) + ((0.0351912321932386 * fTemp86) + ((0.02607257280619568 * fTemp78) + ((((((((3.745445943277512e-18 * fTemp1) + (0.03519123219323861 * fTemp62)) + (0.002905698071001324 * fTemp69)) + (0.025630902468550196 * fTemp90)) + (1.1770801000262998e-18 * fTemp81)) + (0.026072572806195685 * fTemp65)) + (0.0009685660236671086 * fTemp80)) + (0.0009685660236671078 * fTemp79))))))));
			output32[i] = (FAUSTFLOAT)(fRec0[0] * (((0.03390649690496764 * fTemp141) + (((((((0.02733423815434549 * fTemp124) + ((0.0273342381543455 * fTemp125) + (0.027334238154345495 * fTemp118))) + (0.027334238154345505 * fTemp117)) + (1.2553045223568977e-17 * fTemp123)) + (9.37894502882144e-34 * fTemp143)) + (4.6894725144107197e-33 * fTemp142)) + (8.09044008694836e-34 * fTemp72))) - (((6.593521123148732e-18 * fTemp145) + (((((0.02733423815434545 * fTemp110) + (((0.027334238154345478 * fTemp100) + (0.027334238154345474 * fTemp101)) + (0.02733423815434548 * fTemp109))) + (1.7574263312996565e-17 * fTemp108)) + (2.813683508646432e-33 * fTemp147)) + (6.565261520175007e-33 * fTemp146))) + (7.531827134141385e-18 * fTemp144))));
			output33[i] = (FAUSTFLOAT)(fRec0[0] * (((0.012205326872422696 * (fTemp84 - fTemp132)) + ((0.04844794288974959 * fTemp140) + ((0.004533407124042744 * fTemp79) + ((0.004533407124042746 * fTemp80) + ((0.028161597282885704 * fTemp64) + ((2.6699228183342524e-17 * fTemp89) + ((0.00313851262433728 * fTemp71) + ((0.0031385126243372767 * fTemp61) + ((0.028161597282885746 * fTemp59) + ((0.02768453879414262 * fTemp151) + ((4.045543993047532e-18 * fTemp3) + ((4.0448961198413007e-50 * fTemp111) + (0.012205326872422675 * fTemp62))))))))))))) - ((0.004533407124042747 * fTemp87) + ((0.0045334071240427445 * fTemp88) + ((8.89974272778084e-18 * fTemp81) + ((0.02202291575478033 * fTemp82) + ((0.01220532687242269 * fTemp68) + (((((0.027684538794142587 * fTemp11) + ((1.2136631979142595e-17 * fTemp1) + (0.02768453879414263 * fTemp12))) + (0.02816159728288574 * fTemp150)) + (0.0031385126243372798 * fTemp60)) + (0.0031385126243372746 * fTemp69)))))))));
			output34[i] = (FAUSTFLOAT)(fRec0[0] * (((((1.3135133594325083e-66 * fTemp72) + (((((0.010356563277706213 * fTemp118) + ((0.010356563277706059 * fTemp110) + (((0.010356563277706147 * fTemp100) + (0.010356563277706139 * fTemp101)) + (0.010356563277706175 * fTemp109)))) + (0.01035656327770623 * fTemp125)) + (0.010356563277706192 * fTemp124)) + (0.010356563277706244 * fTemp117))) + (0.039826512815546306 * fTemp75)) + (0.004438527119016925 * fTemp77)) - (0.02936388767304044 * fTemp76)));
			output35[i] = (FAUSTFLOAT)(fRec0[0] * (((0.03696108523496622 * ((fTemp35 + (fTemp40 + (fTemp48 + fTemp43))) - (fTemp36 + (fTemp39 + (fTemp47 + fTemp44))))) + ((0.009285676600414948 * fTemp140) + ((0.012594249175901169 * fTemp64) + ((2.8429185316539108e-18 * fTemp81) + ((0.029546842014263944 * fTemp82) + ((0.0014035855152519131 * fTemp71) + ((0.0014035855152519582 * fTemp61) + ((0.012594249175901155 * fTemp59) + ((0.00046786183841728994 * fTemp84) + ((0.03714270640165978 * fTemp11) + ((0.03714270640165973 * fTemp12) + ((9.046111374420218e-18 * fTemp3) + ((2.5434040020076175e-83 * fTemp111) + (0.00046786183841729053 * fTemp62)))))))))))))) - ((0.012594249175901174 * fTemp65) + ((2.5023430732567692e-17 * fTemp89) + (((0.0014035855152521124 * fTemp69) + ((0.0014035855152519602 * fTemp60) + ((0.012594249175901146 * fTemp70) + (((0.03714270640165971 * fTemp10) + ((7.962406901797708e-17 * fTemp1) + (0.037142706401659786 * fTemp13))) + (0.0004678618384172905 * fTemp132))))) + (0.00046786183841728967 * fTemp68))))));
			// post processing
			fRec0[1] = fRec0[0];
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
