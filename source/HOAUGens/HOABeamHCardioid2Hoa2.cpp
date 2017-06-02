//----------------------------------------------------------
// name: "HOABeamHCardioid2Hoa2"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2016"
//
// Code generated with Faust 0.9.95 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with  */
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
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fslider1;
	FAUSTFLOAT 	fslider2;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("route.lib/name", "Faust Signal Routing Library");
		m->declare("route.lib/version", "0.0");
		m->declare("basic.lib/name", "Faust Basic Element Library");
		m->declare("basic.lib/version", "0.0");
		m->declare("lib/ymn.lib/name", "Spherical Harmonics library");
		m->declare("lib/ymn.lib/version", "1.0");
		m->declare("lib/ymn.lib/author", "Pierre Lecomte");
		m->declare("lib/ymn.lib/license", "GPL");
		m->declare("lib/ymn.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/cijk.lib/name", "Cijk matrix");
		m->declare("lib/cijk.lib/version", "10");
		m->declare("lib/cijk.lib/author", "Pierre Lecomte");
		m->declare("lib/cijk.lib/license", "GPL");
		m->declare("lib/cijk.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("signal.lib/name", "Faust Signal Routing Library");
		m->declare("signal.lib/version", "0.0");
		m->declare("math.lib/name", "Faust Math Library");
		m->declare("math.lib/version", "2.0");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("name", "HOABeamHCardioid2Hoa2");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
	}

	virtual int getNumInputs() { return 9; }
	virtual int getNumOutputs() { return 25; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.0;
		fslider1 = 0.0;
		fslider2 = 0.0;
	}
	virtual void instanceClear() {
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
		ui_interface->declare(0, "3", "");
		ui_interface->openHorizontalBox("Parameters");
		ui_interface->declare(&fslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth", &fslider2, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(&fslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation", &fslider1, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(&fslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Order", &fslider0, 0.0, 0.0, 2.0, 1.0);
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		int 	iSlow0 = int(double(fslider0));
		int 	iSlow1 = int((iSlow0 >= 2));
		double 	fSlow2 = double(fslider1);
		double 	fSlow3 = sin(fSlow2);
		double 	fSlow4 = cos(fSlow2);
		double 	fSlow5 = double(fslider2);
		double 	fSlow6 = sin(fSlow5);
		double 	fSlow7 = cos(fSlow5);
		double 	fSlow8 = ((3 * faustpower<2>(fSlow3)) + -1);
		double 	fSlow9 = (0.124224 * fSlow8);
		double 	fSlow10 = sin((2 * fSlow2));
		double 	fSlow11 = faustpower<2>(fSlow4);
		double 	fSlow12 = (2 * fSlow5);
		double 	fSlow13 = cos(fSlow12);
		double 	fSlow14 = sin(fSlow12);
		int 	iSlow15 = int((iSlow0 >= 1));
		double 	fSlow16 = (0.16666405622891672 * fSlow10);
		double 	fSlow17 = (0.19245000000000004 * fSlow4);
		double 	fSlow18 = (0.055554711936 * fSlow8);
		double 	fSlow19 = (fSlow11 * fSlow13);
		double 	fSlow20 = (0.16666405622891672 * fSlow19);
		double 	fSlow21 = (0.11112 - (fSlow18 + fSlow20));
		double 	fSlow22 = (0.14907119265000002 * fSlow3);
		double 	fSlow23 = (0.14907119265000002 * fSlow7);
		double 	fSlow24 = (fSlow4 * fSlow14);
		double 	fSlow25 = (0.16666405622891672 * fSlow24);
		double 	fSlow26 = (fSlow4 * fSlow6);
		double 	fSlow27 = (0 - (0.08606633430000002 * fSlow26));
		double 	fSlow28 = (0 - (0.14907119265000002 * fSlow26));
		double 	fSlow29 = (0.43301700000000004 * fSlow6);
		double 	fSlow30 = (0.335413669149 * fSlow7);
		double 	fSlow31 = (0.335413669149 * fSlow3);
		double 	fSlow32 = (0 - (0.193651264638 * fSlow26));
		double 	fSlow33 = (0 - (0.335413669149 * fSlow26));
		double 	fSlow34 = ((0.111109299648 * fSlow8) + 0.11112);
		double 	fSlow35 = (0.14907119265000002 * fSlow4);
		double 	fSlow36 = (0.335413669149 * fSlow4);
		double 	fSlow37 = (0.19245000000000004 * fSlow7);
		double 	fSlow38 = (0.14907119265000002 * fSlow6);
		double 	fSlow39 = (fSlow4 * fSlow7);
		double 	fSlow40 = (0 - (0.08606633430000002 * fSlow39));
		double 	fSlow41 = (fSlow20 + (0.11112 - fSlow18));
		double 	fSlow42 = (0 - (0.193651264638 * fSlow39));
		double 	fSlow43 = (0.335413669149 * fSlow6);
		double 	fSlow44 = (0.2151622795194362 * fSlow24);
		double 	fSlow45 = (0.11904563149935221 * fSlow10);
		double 	fSlow46 = (0.079363856448 * fSlow8);
		double 	fSlow47 = (0.11112 - fSlow46);
		double 	fSlow48 = (fSlow11 * fSlow14);
		double 	fSlow49 = (0 - (0.13746223165253885 * fSlow48));
		double 	fSlow50 = (0.2151622795194362 * fSlow10);
		double 	fSlow51 = (0.039681866111999996 * fSlow8);
		double 	fSlow52 = (0.11904563149935221 * fSlow19);
		double 	fSlow53 = (fSlow51 + (0.11112 - fSlow52));
		double 	fSlow54 = (0.11904563149935221 * fSlow48);
		double 	fSlow55 = (0.11904563149935221 * fSlow7);
		double 	fSlow56 = (0.06873100824512966 * fSlow6);
		double 	fSlow57 = (fSlow10 * fSlow6);
		double 	fSlow58 = (0 - (0.11904563149935221 * fSlow57));
		double 	fSlow59 = (0.17213247615000002 * fSlow3);
		double 	fSlow60 = (fSlow46 + 0.11112);
		double 	fSlow61 = (0.06873100824512966 * fSlow10);
		double 	fSlow62 = (0 - (0.13746223165253885 * fSlow19));
		double 	fSlow63 = (0.387302096259 * fSlow3);
		double 	fSlow64 = ((fSlow51 + fSlow52) + 0.11112);
		double 	fSlow65 = (fSlow4 * fSlow13);
		double 	fSlow66 = (0.2151622795194362 * fSlow65);
		double 	fSlow67 = (fSlow10 * fSlow7);
		double 	fSlow68 = (0.11904563149935221 * fSlow67);
		double 	fSlow69 = (0.335413669149 * fSlow39);
		double 	fSlow70 = (0.17251367312221164 * fSlow4);
		double 	fSlow71 = (0.347186102328 * fSlow4);
		double 	fSlow72 = (0 - (0.03984099900000001 * fSlow39));
		double 	fSlow73 = (0 - (0.044542895106113686 * fSlow48));
		double 	fSlow74 = (0.044542895106113686 * fSlow19);
		double 	fSlow75 = (0.08908562668799999 * fSlow8);
		double 	fSlow76 = (fSlow74 + fSlow75);
		double 	fSlow77 = (0.15936380355000002 * fSlow3);
		double 	fSlow78 = (0.1781713652621752 * fSlow10);
		double 	fSlow79 = (0 - (0.08964317934 * fSlow39));
		double 	fSlow80 = (0.35857228434300004 * fSlow3);
		double 	fSlow81 = (0 - (0.09759004785000003 * fSlow26));
		double 	fSlow82 = (0 - (0.10910728580834947 * fSlow67));
		double 	fSlow83 = (0.10910718143999999 * fSlow8);
		double 	fSlow84 = (0 - (0.10910728580834947 * fSlow57));
		double 	fSlow85 = (0.16903075950000004 * fSlow3);
		double 	fSlow86 = (0 - (0.09759004785000003 * fSlow39));
		double 	fSlow87 = (0.38032316127 * fSlow3);
		double 	fSlow88 = (0 - (0.219579889581 * fSlow26));
		double 	fSlow89 = (0 - (0.219579889581 * fSlow39));
		double 	fSlow90 = (0.13801301565000001 * fSlow4);
		double 	fSlow91 = (0 - (0.03984099900000001 * fSlow26));
		double 	fSlow92 = (fSlow75 - fSlow74);
		double 	fSlow93 = (0.31053251232900003 * fSlow39);
		double 	fSlow94 = (0 - (0.08964317934 * fSlow26));
		double 	fSlow95 = (0 - (0.1259881623 * fSlow26));
		double 	fSlow96 = (0 - (0.14085684693651698 * fSlow57));
		double 	fSlow97 = (0 - (0.28347631111800004 * fSlow26));
		double 	fSlow98 = (0 - (0.17251367312221164 * fSlow48));
		double 	fSlow99 = (0 - (0.15430333080000003 * fSlow26));
		double 	fSlow100 = (0.15430333080000003 * fSlow7);
		double 	fSlow101 = (0.17251367312221164 * fSlow65);
		double 	fSlow102 = (0 - (0.347186102328 * fSlow26));
		double 	fSlow103 = (0.347186102328 * fSlow39);
		double 	fSlow104 = (0.18184526118496958 * fSlow11);
		double 	fSlow105 = (0.068731027392 * fSlow8);
		double 	fSlow106 = (0.13746223165253885 * fSlow10);
		double 	fSlow107 = (0 - (0.048600210211011685 * fSlow67));
		double 	fSlow108 = (0.097200435264 * fSlow8);
		double 	fSlow109 = (0.048600210211011685 * fSlow19);
		double 	fSlow110 = (fSlow108 + fSlow109);
		double 	fSlow111 = (0 - (0.048600210211011685 * fSlow48));
		double 	fSlow112 = (0.030737437765308104 * fSlow11);
		double 	fSlow113 = (0 - (0.12294996622351191 * fSlow67));
		double 	fSlow114 = (0 - (0.12294996622351191 * fSlow57));
		double 	fSlow115 = (0.10647773203199999 * fSlow8);
		double 	fSlow116 = (fSlow108 - fSlow109);
		double 	fSlow117 = (0 - (0.048600210211011685 * fSlow57));
		double 	fSlow118 = (0.16835609239505758 * fSlow67);
		double 	fSlow119 = (0 - (0.13746223165253885 * fSlow57));
		double 	fSlow120 = (0.13746223165253885 * fSlow67);
		double 	fSlow121 = (0 - (0.12858399051272834 * fSlow57));
		double 	fSlow122 = (0 - (0.12858399051272834 * fSlow48));
		double 	fSlow123 = (0 - (0.18184526118496958 * fSlow48));
		double 	fSlow124 = (0.18184526118496958 * fSlow19);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* input2 = input[2];
		FAUSTFLOAT* input3 = input[3];
		FAUSTFLOAT* input4 = input[4];
		FAUSTFLOAT* input5 = input[5];
		FAUSTFLOAT* input6 = input[6];
		FAUSTFLOAT* input7 = input[7];
		FAUSTFLOAT* input8 = input[8];
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
		for (int i=0; i<count; i++) {
			double fTemp0 = (double)input2[i];
			double fTemp1 = (double)input1[i];
			double fTemp2 = (double)input3[i];
			double fTemp3 = ((fSlow6 * fTemp1) + (fSlow7 * fTemp2));
			double fTemp4 = ((fSlow3 * fTemp0) + (fSlow4 * fTemp3));
			double fTemp5 = (double)input0[i];
			double fTemp6 = (double)input6[i];
			double fTemp7 = (double)input5[i];
			double fTemp8 = (double)input7[i];
			double fTemp9 = ((fSlow6 * fTemp7) + (fSlow7 * fTemp8));
			double fTemp10 = (double)input8[i];
			double fTemp11 = (double)input4[i];
			double fTemp12 = ((fSlow13 * fTemp10) + (fSlow14 * fTemp11));
			output0[i] = (FAUSTFLOAT)((iSlow1)?(((0.19245000000000004 * fTemp4) + ((0.11112 * fTemp5) + (fSlow9 * fTemp6))) + (0.2151622795194362 * ((fSlow10 * fTemp9) + (fSlow11 * fTemp12)))):((iSlow15)?((0.24993 * fTemp5) + (0.43301700000000004 * fTemp4)):fTemp5));
			double fTemp13 = (fSlow16 * fTemp0);
			output1[i] = (FAUSTFLOAT)((iSlow1)?(((((fSlow6 * (fTemp13 + (fSlow17 * fTemp5))) + ((fSlow21 * fTemp1) + (fSlow22 * fTemp7))) + (fSlow4 * ((fSlow23 * fTemp11) + (fSlow25 * fTemp2)))) + (fSlow27 * fTemp6)) + (fSlow28 * fTemp10)):((iSlow15)?(((((0.24993 * fTemp1) + (fSlow4 * ((fSlow29 * fTemp5) + (fSlow30 * fTemp11)))) + (fSlow31 * fTemp7)) + (fSlow32 * fTemp6)) + (fSlow33 * fTemp10)):fTemp1));
			double fTemp14 = (0.43301700000000004 * fTemp5);
			output2[i] = (FAUSTFLOAT)((iSlow1)?((fSlow3 * ((0.19245000000000004 * fTemp5) + (0.17213247615000002 * fTemp6))) + ((fSlow16 * fTemp3) + ((fSlow34 * fTemp0) + (fSlow35 * fTemp9)))):((iSlow15)?(((0.24993 * fTemp0) + (fSlow3 * (fTemp14 + (0.387302096259 * fTemp6)))) + (fSlow36 * fTemp9)):fTemp0));
			output3[i] = (FAUSTFLOAT)((iSlow1)?((fSlow4 * ((fSlow25 * fTemp1) + ((fSlow37 * fTemp5) + (fSlow38 * fTemp11)))) + ((fSlow7 * (fTemp13 + (fSlow35 * fTemp10))) + ((fSlow40 * fTemp6) + ((fSlow41 * fTemp2) + (fSlow22 * fTemp8))))):((iSlow15)?(((fSlow31 * fTemp8) + ((0.24993 * fTemp2) + (fSlow42 * fTemp6))) + (fSlow4 * ((fSlow43 * fTemp11) + (fSlow7 * (fTemp14 + (0.335413669149 * fTemp10)))))):fTemp2));
			double fTemp15 = ((fSlow6 * fTemp2) + (fSlow7 * fTemp1));
			double fTemp16 = ((fSlow7 * fTemp7) + (fSlow6 * fTemp8));
			output4[i] = (FAUSTFLOAT)((iSlow1)?((fSlow4 * ((fSlow44 * fTemp5) + (0.14907119265000002 * fTemp15))) + ((fSlow45 * fTemp16) + ((fSlow47 * fTemp11) + (fSlow49 * fTemp6)))):((iSlow15)?((fSlow36 * fTemp15) + (0.24993 * fTemp11)):fTemp11));
			double fTemp17 = (fSlow35 * fTemp0);
			output5[i] = (FAUSTFLOAT)((iSlow1)?((((((fSlow22 * fTemp1) + (fSlow6 * ((fSlow50 * fTemp5) + fTemp17))) + (fSlow53 * fTemp7)) + (fSlow54 * fTemp8)) + (fSlow10 * ((fSlow55 * fTemp11) + (fSlow56 * fTemp6)))) + (fSlow58 * fTemp10)):((iSlow15)?((0.335413669149 * ((fSlow3 * fTemp1) + (fSlow26 * fTemp0))) + (0.24993 * fTemp7)):fTemp7));
			output6[i] = (FAUSTFLOAT)((iSlow1)?(((((((fSlow27 * fTemp1) + ((fSlow9 * fTemp5) + (fSlow59 * fTemp0))) + (fSlow40 * fTemp2)) + (fSlow49 * fTemp11)) + (fSlow60 * fTemp6)) + (fSlow61 * fTemp9)) + (fSlow62 * fTemp10)):((iSlow15)?((fSlow42 * fTemp2) + ((fSlow32 * fTemp1) + ((fSlow63 * fTemp0) + (0.24993 * fTemp6)))):fTemp6));
			output7[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow64 * fTemp8) + ((fSlow22 * fTemp2) + (0.11904563149935221 * ((fSlow57 * fTemp11) + (fSlow48 * fTemp7))))) + (fSlow7 * (fTemp17 + (fSlow10 * ((0.11904563149935221 * fTemp10) + ((0.2151622795194362 * fTemp5) + (0.06873100824512966 * fTemp6))))))):((iSlow15)?((0.335413669149 * ((fSlow39 * fTemp0) + (fSlow3 * fTemp2))) + (0.24993 * fTemp8)):fTemp8));
			output8[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow62 * fTemp6) + ((fSlow4 * ((fSlow23 * fTemp2) + (fSlow66 * fTemp5))) + ((fSlow58 * fTemp7) + ((fSlow28 * fTemp1) + (fSlow68 * fTemp8))))) + (fSlow47 * fTemp10)):((iSlow15)?((fSlow33 * fTemp1) + ((fSlow69 * fTemp2) + (0.24993 * fTemp10))):fTemp10));
			double fTemp18 = ((fSlow7 * fTemp11) + (fSlow6 * fTemp10));
			output9[i] = (FAUSTFLOAT)((iSlow1)?(fSlow4 * ((fSlow70 * ((fSlow13 * fTemp1) + (fSlow14 * fTemp2))) + (0.15430333080000003 * fTemp18))):((iSlow15)?(fSlow71 * fTemp18):0));
			double fTemp19 = ((fSlow3 * fTemp11) + (fSlow4 * fTemp16));
			output10[i] = (FAUSTFLOAT)((iSlow1)?((0.14085684693651698 * ((fSlow48 * fTemp0) + (fSlow10 * fTemp15))) + (0.1259881623 * fTemp19)):((iSlow15)?(0.28347631111800004 * fTemp19):0));
			double fTemp20 = (fSlow78 * fTemp0);
			output11[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow72 * fTemp11) + ((fSlow73 * fTemp2) + ((fSlow76 * fTemp1) + (fSlow77 * fTemp7)))) + (fSlow6 * (fTemp20 + (fSlow4 * ((0.13801301565000001 * fTemp6) + (0.03984099900000001 * fTemp10)))))):((iSlow15)?(((fSlow79 * fTemp11) + (fSlow80 * fTemp7)) + (fSlow26 * ((0.08964317934 * fTemp10) + (0.31053251232900003 * fTemp6)))):0));
			output12[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow81 * fTemp7) + ((fSlow82 * fTemp2) + ((fSlow83 * fTemp0) + ((fSlow84 * fTemp1) + (fSlow85 * fTemp6))))) + (fSlow86 * fTemp8)):((iSlow15)?(((fSlow87 * fTemp6) + (fSlow88 * fTemp7)) + (fSlow89 * fTemp8)):0));
			output13[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow7 * (fTemp20 + (fSlow90 * fTemp6))) + ((fSlow91 * fTemp11) + ((fSlow92 * fTemp2) + ((fSlow73 * fTemp1) + (fSlow77 * fTemp8))))) + (fSlow72 * fTemp10)):((iSlow15)?(((fSlow93 * fTemp6) + ((fSlow94 * fTemp11) + (fSlow80 * fTemp8))) + (fSlow79 * fTemp10)):0));
			double fTemp21 = ((fSlow39 * fTemp8) + (fSlow3 * fTemp10));
			output14[i] = (FAUSTFLOAT)((iSlow1)?((fSlow95 * fTemp7) + ((0.14085684693651698 * ((fSlow67 * fTemp2) + (fSlow19 * fTemp0))) + ((fSlow96 * fTemp1) + (0.1259881623 * fTemp21)))):((iSlow15)?((fSlow97 * fTemp7) + (0.28347631111800004 * fTemp21)):0));
			output15[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow98 * fTemp1) + (fSlow99 * fTemp11)) + (fSlow4 * ((fSlow100 * fTemp10) + (fSlow101 * fTemp2)))):((iSlow15)?((fSlow102 * fTemp11) + (fSlow103 * fTemp10)):0));
			output16[i] = (FAUSTFLOAT)((iSlow1)?(fSlow104 * ((fSlow13 * fTemp11) + (fSlow14 * fTemp10))):0);
			output17[i] = (FAUSTFLOAT)((iSlow1)?(0.12858399051272834 * ((fSlow10 * fTemp18) + (fSlow11 * ((fSlow13 * fTemp7) + (fSlow14 * fTemp8))))):0);
			output18[i] = (FAUSTFLOAT)((iSlow1)?((fSlow54 * fTemp6) + ((fSlow105 * fTemp11) + (fSlow106 * fTemp16))):0);
			output19[i] = (FAUSTFLOAT)((iSlow1)?((((fSlow107 * fTemp11) + (fSlow110 * fTemp7)) + (fSlow111 * fTemp8)) + (fSlow57 * ((0.16835609239505758 * fTemp6) + (0.048600210211011685 * fTemp10)))):0);
			output20[i] = (FAUSTFLOAT)((iSlow1)?((fSlow112 * fTemp12) + ((fSlow113 * fTemp8) + ((fSlow114 * fTemp7) + (fSlow115 * fTemp6)))):0);
			output21[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow116 * fTemp8) + ((fSlow111 * fTemp7) + ((fSlow117 * fTemp11) + (fSlow118 * fTemp6)))) + (fSlow107 * fTemp10)):0);
			output22[i] = (FAUSTFLOAT)((iSlow1)?((fSlow105 * fTemp10) + ((fSlow52 * fTemp6) + ((fSlow119 * fTemp7) + (fSlow120 * fTemp8)))):0);
			output23[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow121 * fTemp11) + (fSlow122 * fTemp7)) + (0.12858399051272834 * ((fSlow67 * fTemp10) + (fSlow19 * fTemp8)))):0);
			output24[i] = (FAUSTFLOAT)((iSlow1)?((fSlow123 * fTemp11) + (fSlow124 * fTemp10)):0);
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
