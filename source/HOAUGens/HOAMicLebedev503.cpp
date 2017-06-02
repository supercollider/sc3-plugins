//----------------------------------------------------------
// name: "HOAMicLebedev503"
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
		m->declare("name", "HOAMicLebedev503");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/lebedev.lib/name", "Lebdev grids and weights");
		m->declare("lib/lebedev.lib/version", "1.0");
		m->declare("lib/lebedev.lib/author", "Pierre Lecomte");
		m->declare("lib/lebedev.lib/license", "GPL");
		m->declare("lib/lebedev.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("math.lib/name", "Faust Math Library");
		m->declare("math.lib/version", "2.0");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/license", "LGPL with exception");
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
		m->declare("basic.lib/name", "Faust Basic Element Library");
		m->declare("basic.lib/version", "0.0");
		m->declare("signal.lib/name", "Faust Signal Routing Library");
		m->declare("signal.lib/version", "0.0");
	}

	virtual int getNumInputs() { return 50; }
	virtual int getNumOutputs() { return 16; }
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
		for (int i=0; i<count; i++) {
			double fTemp0 = (double)input0[i];
			double fTemp1 = (double)input1[i];
			double fTemp2 = (double)input2[i];
			double fTemp3 = (double)input3[i];
			double fTemp4 = (double)input4[i];
			double fTemp5 = (double)input5[i];
			double fTemp6 = (double)input6[i];
			double fTemp7 = (double)input7[i];
			double fTemp8 = (double)input8[i];
			double fTemp9 = (double)input9[i];
			double fTemp10 = (((fTemp6 + fTemp7) + fTemp8) + fTemp9);
			double fTemp11 = (double)input10[i];
			double fTemp12 = (double)input11[i];
			double fTemp13 = (double)input12[i];
			double fTemp14 = (double)input13[i];
			double fTemp15 = (double)input14[i];
			double fTemp16 = (double)input15[i];
			double fTemp17 = (double)input16[i];
			double fTemp18 = (double)input17[i];
			double fTemp19 = (double)input18[i];
			double fTemp20 = (double)input19[i];
			double fTemp21 = (double)input20[i];
			double fTemp22 = (double)input21[i];
			double fTemp23 = (((fTemp19 + fTemp20) + fTemp21) + fTemp22);
			double fTemp24 = (double)input22[i];
			double fTemp25 = (double)input23[i];
			double fTemp26 = (double)input24[i];
			double fTemp27 = (double)input25[i];
			double fTemp28 = ((((fTemp23 + fTemp24) + fTemp25) + fTemp26) + fTemp27);
			double fTemp29 = (double)input26[i];
			double fTemp30 = (double)input27[i];
			double fTemp31 = (fTemp29 + fTemp30);
			double fTemp32 = (double)input28[i];
			double fTemp33 = (double)input29[i];
			double fTemp34 = ((fTemp31 + fTemp32) + fTemp33);
			double fTemp35 = (double)input30[i];
			double fTemp36 = (double)input31[i];
			double fTemp37 = (double)input32[i];
			double fTemp38 = (double)input33[i];
			double fTemp39 = (double)input34[i];
			double fTemp40 = (double)input35[i];
			double fTemp41 = (double)input36[i];
			double fTemp42 = (double)input37[i];
			double fTemp43 = (double)input38[i];
			double fTemp44 = (double)input39[i];
			double fTemp45 = (double)input40[i];
			double fTemp46 = (double)input41[i];
			double fTemp47 = (double)input42[i];
			double fTemp48 = (double)input43[i];
			double fTemp49 = (double)input44[i];
			double fTemp50 = (double)input45[i];
			double fTemp51 = (double)input46[i];
			double fTemp52 = (double)input47[i];
			double fTemp53 = (double)input48[i];
			double fTemp54 = (double)input49[i];
			fRec0[0] = (fSlow0 + (0.999 * fRec0[1]));
			output0[i] = (FAUSTFLOAT)(((((0.012698412698412698 * (((((fTemp0 + fTemp1) + fTemp2) + fTemp3) + fTemp4) + fTemp5)) + (0.022574955908289243 * ((((((((fTemp10 + fTemp11) + fTemp12) + fTemp13) + fTemp14) + fTemp15) + fTemp16) + fTemp17) + fTemp18))) + (0.02109375 * fTemp28)) + (0.02017333553791887 * ((((((((((((((((((((fTemp34 + fTemp35) + fTemp36) + fTemp37) + fTemp38) + fTemp39) + fTemp40) + fTemp41) + fTemp42) + fTemp43) + fTemp44) + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) + fTemp50) + fTemp51) + fTemp52) + fTemp53) + fTemp54))) * fRec0[0]);
			double fTemp55 = (fTemp52 + (fTemp31 + fTemp51));
			double fTemp56 = (fTemp32 + fTemp53);
			double fTemp57 = (fTemp36 + fTemp37);
			double fTemp58 = (fTemp19 + fTemp24);
			double fTemp59 = (fTemp21 + fTemp26);
			double fTemp60 = (fTemp58 - fTemp59);
			double fTemp61 = (fTemp38 + fTemp46);
			double fTemp62 = (fTemp35 + fTemp43);
			double fTemp63 = (fTemp2 - fTemp4);
			double fTemp64 = (fTemp11 - fTemp13);
			double fTemp65 = (fTemp8 + fTemp17);
			double fTemp66 = (fTemp20 + fTemp25);
			double fTemp67 = (fTemp22 + fTemp27);
			double fTemp68 = (fTemp39 + fTemp47);
			double fTemp69 = (fTemp42 + fTemp50);
			double fTemp70 = (fTemp33 + fTemp54);
			output1[i] = (FAUSTFLOAT)(fRec0[0] * (((0.010535180889746065 * (fTemp55 - fTemp56)) + ((0.03160554266923824 * ((fTemp45 + (fTemp57 + fTemp44)) - (fTemp49 + (fTemp48 + (fTemp40 + fTemp41))))) + ((0.021093749999999994 * fTemp60) + ((0.027648561470568506 * ((fTemp16 + (fTemp7 + fTemp12)) - (fTemp18 + (fTemp9 + fTemp14)))) + ((2.693524415809287e-18 * fTemp3) + ((0.010535180889746096 * fTemp61) + ((0.010535180889746094 * fTemp62) + ((((0.0219942959691286 * fTemp63) + (0.0276485614705685 * fTemp64)) + (3.3859722305960555e-18 * fTemp65)) + (0.021093749999999998 * fTemp66))))))))) - ((((0.02109375 * fTemp67) + (0.01053518088974609 * fTemp68)) + (0.010535180889746113 * fTemp69)) + (0.010535180889746067 * fTemp70))));
			double fTemp71 = (fTemp34 - (fTemp54 + (fTemp53 + (fTemp51 + fTemp52))));
			double fTemp72 = (fTemp0 - fTemp5);
			double fTemp73 = (fTemp10 - (fTemp18 + (fTemp17 + (fTemp15 + fTemp16))));
			double fTemp74 = (fTemp23 - (fTemp27 + (fTemp26 + (fTemp24 + fTemp25))));
			double fTemp75 = (((((((fTemp35 + fTemp36) + fTemp37) + fTemp38) + fTemp39) + fTemp40) + fTemp41) + fTemp42);
			double fTemp76 = (fTemp75 - (fTemp50 + (fTemp49 + (fTemp48 + (fTemp47 + (fTemp46 + (fTemp45 + (fTemp43 + fTemp44))))))));
			output2[i] = (FAUSTFLOAT)(fRec0[0] * ((0.03160554266923825 * fTemp71) + ((((0.0219942959691286 * fTemp72) + (0.0276485614705685 * fTemp73)) + (0.02109375 * fTemp74)) + (0.010535180889746075 * fTemp76))));
			double fTemp77 = (fTemp62 - (fTemp47 + (fTemp46 + (fTemp38 + fTemp39))));
			double fTemp78 = (fTemp41 + fTemp49);
			double fTemp79 = (fTemp36 + fTemp44);
			double fTemp80 = (fTemp7 + fTemp16);
			double fTemp81 = (fTemp1 - fTemp3);
			double fTemp82 = (fTemp29 + fTemp33);
			double fTemp83 = (fTemp30 + fTemp52);
			double fTemp84 = ((fTemp54 + (fTemp51 + fTemp82)) - fTemp83);
			double fTemp85 = (fTemp66 - fTemp67);
			double fTemp86 = (fTemp40 + fTemp48);
			double fTemp87 = (fTemp9 + fTemp18);
			double fTemp88 = (fTemp12 - fTemp14);
			double fTemp89 = (fTemp37 + fTemp45);
			output3[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.027648561470568506 * ((fTemp15 + (fTemp6 + fTemp11)) - (fTemp17 + (fTemp8 + fTemp13)))) + ((0.03160554266923824 * fTemp77) + ((0.010535180889746074 * fTemp78) + ((0.01053518088974608 * fTemp79) + ((0.021093749999999998 * fTemp58) + ((1.6929861152980278e-18 * fTemp80) + ((1.3467622079046435e-18 * ((fTemp0 + fTemp2) + fTemp5)) + ((0.0219942959691286 * fTemp81) + (0.03160554266923823 * fTemp69))))))))) + (0.010535180889746065 * fTemp84)) - ((0.021093749999999994 * fTemp85) + ((4.040286623713931e-18 * fTemp4) + ((0.010535180889746067 * fTemp56) + ((0.010535180889746086 * fTemp86) + ((((5.078958345894083e-18 * fTemp87) + (0.0276485614705685 * fTemp88)) + (0.02109375 * fTemp59)) + (0.010535180889746079 * fTemp89))))))));
			double fTemp90 = (fTemp19 + fTemp21);
			double fTemp91 = (fTemp20 + fTemp22);
			double fTemp92 = (fTemp30 + fTemp33);
			double fTemp93 = (fTemp29 + fTemp32);
			output4[i] = (FAUSTFLOAT)(fRec0[0] * (((0.043716214137085485 * ((fTemp11 + fTemp13) - (fTemp12 + fTemp14))) + ((0.027231914153020897 * ((fTemp26 + (fTemp24 + fTemp90)) - (fTemp27 + (fTemp25 + fTemp91)))) + ((9.034355539207464e-18 * fTemp4) + ((3.0114518464024883e-18 * fTemp2) + ((((2.6768460856911014e-18 * fTemp80) + ((8.030538257073304e-18 * fTemp87) + (0.02130845252067657 * fTemp68))) + (0.021308452520676556 * fTemp79)) + (0.021308452520676563 * fTemp86)))))) - (((0.021308452520676584 * (fTemp61 - fTemp62)) + ((6.0229036928049765e-18 * fTemp3) + (((0.021308452520676553 * fTemp89) + ((5.353692171382203e-18 * fTemp65) + (0.021308452520676615 * fTemp69))) + (0.021308452520676542 * fTemp78)))) + (0.007102817506892165 * ((fTemp54 + (fTemp52 + fTemp92)) - (fTemp53 + (fTemp51 + fTemp93)))))));
			double fTemp94 = ((fTemp19 + fTemp26) - (fTemp21 + fTemp24));
			double fTemp95 = ((fTemp20 + fTemp27) - (fTemp22 + fTemp25));
			double fTemp96 = (fTemp8 - fTemp17);
			double fTemp97 = (fTemp35 - fTemp43);
			double fTemp98 = (fTemp38 - fTemp46);
			double fTemp99 = (fTemp42 - fTemp50);
			double fTemp100 = (fTemp39 - fTemp47);
			double fTemp101 = (fTemp33 - fTemp54);
			output5[i] = (FAUSTFLOAT)(fRec0[0] * ((((((0.027231914153020904 * fTemp94) + ((0.043716214137085485 * ((fTemp7 + fTemp18) - (fTemp9 + fTemp16))) + ((0.02723191415302091 * fTemp95) + ((0.021308452520676546 * ((fTemp36 + fTemp48) - (fTemp40 + fTemp44))) + ((0.021308452520676528 * ((fTemp31 + fTemp53) - (fTemp52 + (fTemp32 + fTemp51)))) + (0.021308452520676553 * ((fTemp37 + fTemp49) - (fTemp41 + fTemp45)))))))) + (5.353692171382201e-18 * fTemp96)) + (0.007102817506892191 * fTemp97)) + (0.007102817506892193 * fTemp98)) - ((0.007102817506892205 * fTemp99) + ((0.007102817506892187 * fTemp100) + (0.02130845252067653 * fTemp101)))));
			double fTemp102 = (fTemp0 + fTemp5);
			output6[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.012619783999998805 * (fTemp18 + (fTemp17 + (fTemp16 + (fTemp10 + fTemp15))))) + ((0.02839451399999733 * fTemp102) + (5.236595479505967e-18 * fTemp28))) + (0.03280650879687195 * (fTemp54 + (fTemp53 + (fTemp52 + (fTemp34 + fTemp51)))))) - (((0.014197256999998666 * (fTemp4 + (fTemp3 + (fTemp1 + fTemp2)))) + (0.02523956799999763 * (fTemp14 + (fTemp13 + (fTemp11 + fTemp12))))) + (0.01640325439843596 * (fTemp50 + (fTemp49 + (fTemp48 + (fTemp47 + (fTemp46 + (fTemp45 + (fTemp44 + (fTemp75 + fTemp43))))))))))));
			double fTemp103 = ((fTemp35 + fTemp46) - (fTemp38 + fTemp43));
			double fTemp104 = (fTemp7 - fTemp16);
			double fTemp105 = (fTemp36 - fTemp44);
			double fTemp106 = (fTemp41 - fTemp49);
			double fTemp107 = (fTemp9 - fTemp18);
			double fTemp108 = (fTemp37 - fTemp45);
			double fTemp109 = (fTemp40 - fTemp48);
			double fTemp110 = (fTemp32 - fTemp53);
			output7[i] = (FAUSTFLOAT)(fRec0[0] * (((((((0.043716214137085485 * ((fTemp6 + fTemp17) - (fTemp8 + fTemp15))) + ((0.021308452520676542 * fTemp103) + ((0.02723191415302091 * fTemp94) + (3.0114518464024883e-18 * fTemp72)))) + (0.02130845252067654 * fTemp99)) + (2.6768460856911007e-18 * fTemp104)) + (0.007102817506892183 * fTemp105)) + (0.0071028175068921775 * fTemp106)) - (((((((0.027231914153020904 * fTemp95) + (0.021308452520676528 * ((fTemp54 + (fTemp30 + fTemp51)) - (fTemp52 + fTemp82)))) + (0.021308452520676546 * fTemp100)) + (8.0305382570733e-18 * fTemp107)) + (0.007102817506892181 * fTemp108)) + (0.007102817506892185 * fTemp109)) + (0.02130845252067653 * fTemp110))));
			double fTemp111 = (fTemp7 + fTemp9);
			double fTemp112 = (fTemp6 + fTemp8);
			double fTemp113 = (fTemp29 + fTemp51);
			output8[i] = (FAUSTFLOAT)(fRec0[0] * (((9.219912161207984e-35 * fTemp102) + ((1.3384230428455503e-17 * fTemp13) + ((2.6768460856911007e-18 * fTemp11) + (((1.6674738251076273e-18 * fTemp58) + ((8.337369125538138e-18 * fTemp59) + ((((0.028411270027568724 * fTemp62) + (0.028411270027568717 * fTemp61)) + (0.02841127002756873 * fTemp68)) + (0.028411270027568696 * fTemp69)))) + (2.1746106811858183e-18 * fTemp56))))) - ((((0.024590370452110585 * ((fTemp2 + fTemp4) - (fTemp1 + fTemp3))) + ((0.02185810706854275 * ((fTemp18 + (fTemp16 + fTemp111)) - (fTemp17 + (fTemp15 + fTemp112)))) + ((1.8737922599837705e-17 * fTemp14) + ((8.0305382570733e-18 * fTemp12) + ((0.028411270027568734 * fTemp86) + ((0.028411270027568748 * fTemp78) + ((0.028411270027568745 * fTemp89) + ((0.02841127002756874 * fTemp79) + (((1.167231677575339e-17 * fTemp67) + (5.002421475322882e-18 * fTemp66)) + (3.044454953660145e-18 * fTemp70)))))))))) + (1.1422201709353029e-18 * fTemp113)) + (1.3047664087114909e-18 * fTemp83))));
			double fTemp114 = (fTemp87 - fTemp80);
			output9[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.01669440503167441 * fTemp114) + ((0.033388810063348796 * fTemp12) + ((0.0333888100633488 * fTemp11) + ((9.75821927113805e-18 * fTemp3) + (((0.020818537549440803 * fTemp78) + ((0.016982082124440745 * fTemp58) + (((6.133424905712859e-18 * fTemp65) + (0.01698208212444074 * fTemp66)) + (0.03007122090474784 * fTemp61)))) + (0.020818537549440772 * fTemp86)))))) + (0.002313170838826743 * fTemp55)) - ((0.03007122090474782 * (fTemp68 - fTemp62)) + ((0.03338881006334882 * fTemp14) + ((0.03338881006334885 * fTemp13) + (((0.02081853754944079 * fTemp89) + ((0.020818537549440783 * fTemp79) + ((0.016982082124440766 * fTemp59) + ((((0.026560635762986527 * fTemp63) + (0.01698208212444075 * fTemp67)) + (0.030071220904747842 * fTemp69)) + (0.0023131708388267444 * fTemp70))))) + (0.002313170838826747 * fTemp56)))))));
			output10[i] = (FAUSTFLOAT)(fRec0[0] * (((((((((0.041597435974919175 * ((fTemp27 + (fTemp25 + fTemp90)) - (fTemp26 + (fTemp24 + fTemp91)))) + (0.016998264729033888 * fTemp103)) + (0.016998264729033836 * ((fTemp54 + (fTemp52 + fTemp93)) - (fTemp53 + (fTemp51 + fTemp92))))) + (0.01699826472903388 * fTemp100)) + (5.007920464891509e-18 * fTemp104)) + (1.5023761394674526e-17 * fTemp107)) + (0.016998264729033867 * fTemp105)) + (0.016998264729033874 * fTemp109)) - ((0.016998264729033857 * fTemp106) + ((0.016998264729033864 * fTemp108) + ((1.0015840929783018e-17 * fTemp96) + (0.016998264729033916 * fTemp99))))));
			output11[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.01612596984427559 * (fTemp86 - fTemp79)) + ((0.016125969844275594 * (fTemp78 - fTemp89)) + ((0.013154264250377145 * fTemp60) + ((((4.750930503007937e-18 * fTemp65) + (0.013154264250377147 * fTemp66)) + (0.005375323281425202 * fTemp68)) + (0.005375323281425214 * fTemp69))))) + (0.030460165261409405 * fTemp55)) - ((0.03879429159750975 * fTemp114) + ((2.5195613817171963e-18 * fTemp3) + ((0.030460165261409402 * fTemp56) + ((0.02586286106500651 * fTemp88) + (((0.005375323281425206 * fTemp61) + ((((0.020573779994945588 * fTemp63) + (0.025862861065006505 * fTemp64)) + (0.013154264250377148 * fTemp67)) + (0.005375323281425205 * fTemp62))) + (0.030460165261409412 * fTemp70))))))));
			output12[i] = (FAUSTFLOAT)(fRec0[0] * (((0.0263335984839893 * fTemp71) + (0.03359684204526464 * fTemp72)) - (((0.010558468816293319 * fTemp73) + (0.021480823570105497 * fTemp74)) + (0.020481687709769415 * fTemp76))));
			double fTemp115 = (fTemp65 - (fTemp6 + fTemp15));
			output13[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.030460165261409402 * fTemp84) + ((3.779342072575794e-18 * fTemp4) + ((0.0053753232814252 * fTemp86) + ((((0.025862861065006505 * fTemp88) + (2.3754652515039685e-18 * fTemp80)) + (0.013154264250377147 * fTemp58)) + (0.005375323281425197 * fTemp89))))) + (5.0391227634343926e-18 * fTemp102)) - ((0.03879429159750975 * fTemp115) + ((0.013154264250377145 * fTemp85) + ((0.01612596984427559 * fTemp77) + (((0.030460165261409412 * fTemp56) + ((((0.013154264250377148 * fTemp59) + ((7.126395754511905e-18 * fTemp87) + ((0.020573779994945588 * fTemp81) + ((0.02586286106500651 * fTemp64) + (0.016125969844275587 * fTemp69))))) + (0.005375323281425198 * fTemp79)) + (0.005375323281425194 * fTemp78))) + (1.2597806908585981e-18 * fTemp2)))))));
			output14[i] = (FAUSTFLOAT)(fRec0[0] * (((5.204217622867659e-18 * fTemp110) + ((0.02266435297204512 * fTemp99) + ((0.02266435297204515 * fTemp100) + ((0.022664352972045138 * fTemp98) + (((0.04089277388695433 * ((fTemp18 + (fTemp16 + fTemp112)) - (fTemp17 + (fTemp15 + fTemp111)))) + ((1.273554170485543e-17 * (fTemp21 - fTemp26)) + ((2.4393594688416384e-34 * fTemp72) + (2.5471083409710867e-18 * (fTemp19 - fTemp24))))) + (0.022664352972045144 * fTemp97)))))) - ((0.022664352972045165 * fTemp106) + ((0.022664352972045155 * fTemp109) + ((7.285904672014723e-18 * fTemp101) + ((0.022664352972045162 * (fTemp57 - (fTemp44 + fTemp45))) + ((1.7829758386797603e-17 * (fTemp22 - fTemp27)) + ((7.641325022913258e-18 * (fTemp20 - fTemp25)) + ((2.7335294515958808e-18 * (fTemp29 - fTemp51)) + (3.1225305737205953e-18 * (fTemp30 - fTemp52)))))))))));
			output15[i] = (FAUSTFLOAT)(fRec0[0] * ((((6.097910338853647e-51 * fTemp102) + ((0.03338881006334876 * fTemp13) + ((0.03338881006334882 * fTemp12) + ((1.4637328906707076e-17 * fTemp4) + (((0.0300712209047478 * fTemp86) + ((0.03007122090474779 * fTemp89) + ((0.01698208212444072 * fTemp59) + ((9.20013735856929e-18 * fTemp87) + ((0.026560635762986527 * fTemp81) + (((0.01698208212444075 * fTemp66) + (0.02081853754944074 * fTemp62)) + (0.02081853754944071 * fTemp69))))))) + (0.002313170838826741 * fTemp56)))))) + (0.0023131708388267444 * fTemp83)) - ((0.002313170838826745 * fTemp113) + (((0.01698208212444074 * (fTemp27 + (fTemp24 + (fTemp19 + fTemp22)))) + ((0.0333888100633488 * (fTemp11 + fTemp14)) + ((4.879109635569025e-18 * fTemp2) + ((0.03007122090474778 * fTemp78) + ((0.030071220904747797 * fTemp79) + ((3.0667124528564294e-18 * fTemp80) + (((0.020818537549440724 * fTemp61) + (0.020818537549440744 * fTemp68)) + (0.0023131708388267436 * fTemp70)))))))) + (0.01669440503167441 * fTemp115)))));
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
