//----------------------------------------------------------
// name: "HOABeamHCardioid2Hoa3"
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
		m->declare("name", "HOABeamHCardioid2Hoa3");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
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
	}

	virtual int getNumInputs() { return 16; }
	virtual int getNumOutputs() { return 36; }
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
		double 	fSlow6 = cos(fSlow5);
		double 	fSlow7 = sin(fSlow5);
		double 	fSlow8 = ((3 * faustpower<2>(fSlow3)) + -1);
		double 	fSlow9 = (0.124224 * fSlow8);
		double 	fSlow10 = sin((2 * fSlow2));
		double 	fSlow11 = faustpower<2>(fSlow4);
		double 	fSlow12 = (2 * fSlow5);
		double 	fSlow13 = cos(fSlow12);
		double 	fSlow14 = sin(fSlow12);
		int 	iSlow15 = int((iSlow0 >= 1));
		double 	fSlow16 = (0.14907119265000002 * fSlow6);
		double 	fSlow17 = (0.17251367312221164 * fSlow13);
		double 	fSlow18 = (0.16666405622891672 * fSlow14);
		double 	fSlow19 = (fSlow11 * fSlow14);
		double 	fSlow20 = (0 - (0.17251367312221164 * fSlow19));
		double 	fSlow21 = (fSlow10 * fSlow7);
		double 	fSlow22 = (0 - (0.14085684693651698 * fSlow21));
		double 	fSlow23 = (0 - (0.044542895106113686 * fSlow19));
		double 	fSlow24 = (0 - (0.10910728580834947 * fSlow21));
		double 	fSlow25 = (0.08908562668799999 * fSlow8);
		double 	fSlow26 = (fSlow11 * fSlow13);
		double 	fSlow27 = (0.044542895106113686 * fSlow26);
		double 	fSlow28 = (fSlow25 + fSlow27);
		double 	fSlow29 = (fSlow4 * fSlow7);
		double 	fSlow30 = (0 - (0.14907119265000002 * fSlow29));
		double 	fSlow31 = (0.19245000000000004 * fSlow4);
		double 	fSlow32 = (0.16666405622891672 * fSlow10);
		double 	fSlow33 = (0.055554711936 * fSlow8);
		double 	fSlow34 = (0.16666405622891672 * fSlow26);
		double 	fSlow35 = (0.11112 - (fSlow33 + fSlow34));
		double 	fSlow36 = (0.14907119265000002 * fSlow3);
		double 	fSlow37 = (0 - (0.08606633430000002 * fSlow29));
		double 	fSlow38 = (fSlow10 * fSlow6);
		double 	fSlow39 = (0.14085684693651698 * fSlow38);
		double 	fSlow40 = (0.43301700000000004 * fSlow7);
		double 	fSlow41 = (0.335413669149 * fSlow6);
		double 	fSlow42 = (0.335413669149 * fSlow3);
		double 	fSlow43 = (0 - (0.193651264638 * fSlow29));
		double 	fSlow44 = (0 - (0.335413669149 * fSlow29));
		double 	fSlow45 = (0.14085684693651698 * fSlow4);
		double 	fSlow46 = ((0.111109299648 * fSlow8) + 0.11112);
		double 	fSlow47 = (0.10910718143999999 * fSlow8);
		double 	fSlow48 = (0.335413669149 * fSlow4);
		double 	fSlow49 = (0.19245000000000004 * fSlow6);
		double 	fSlow50 = (0.14907119265000002 * fSlow7);
		double 	fSlow51 = (0.14085684693651698 * fSlow10);
		double 	fSlow52 = (fSlow25 - fSlow27);
		double 	fSlow53 = (0 - (0.10910728580834947 * fSlow38));
		double 	fSlow54 = (0.14907119265000002 * fSlow4);
		double 	fSlow55 = (fSlow4 * fSlow6);
		double 	fSlow56 = (0 - (0.08606633430000002 * fSlow55));
		double 	fSlow57 = (fSlow34 + (0.11112 - fSlow33));
		double 	fSlow58 = (0 - (0.193651264638 * fSlow55));
		double 	fSlow59 = (0.335413669149 * fSlow7);
		double 	fSlow60 = (fSlow4 * fSlow14);
		double 	fSlow61 = (0.2151622795194362 * fSlow60);
		double 	fSlow62 = (0.15430333080000003 * fSlow6);
		double 	fSlow63 = (0 - (0.15430333080000003 * fSlow29));
		double 	fSlow64 = (0 - (0.03984099900000001 * fSlow29));
		double 	fSlow65 = (0.11904563149935221 * fSlow10);
		double 	fSlow66 = (0 - (0.13746223165253885 * fSlow19));
		double 	fSlow67 = (0.079363856448 * fSlow8);
		double 	fSlow68 = (0.11112 - fSlow67);
		double 	fSlow69 = (0.1259881623 * fSlow3);
		double 	fSlow70 = (0 - (0.03984099900000001 * fSlow55));
		double 	fSlow71 = (0.347186102328 * fSlow6);
		double 	fSlow72 = (0.28347631111800004 * fSlow3);
		double 	fSlow73 = (0 - (0.08964317934 * fSlow55));
		double 	fSlow74 = (0 - (0.08964317934 * fSlow29));
		double 	fSlow75 = (0 - (0.347186102328 * fSlow29));
		double 	fSlow76 = (0 - (0.11904563149935221 * fSlow21));
		double 	fSlow77 = (0.11904563149935221 * fSlow6);
		double 	fSlow78 = (0.06873100824512966 * fSlow7);
		double 	fSlow79 = (0.2151622795194362 * fSlow10);
		double 	fSlow80 = (0.039681866111999996 * fSlow8);
		double 	fSlow81 = (0.11904563149935221 * fSlow26);
		double 	fSlow82 = (fSlow80 + (0.11112 - fSlow81));
		double 	fSlow83 = (0.11904563149935221 * fSlow60);
		double 	fSlow84 = (0.1259881623 * fSlow6);
		double 	fSlow85 = (0 - (0.09759004785000003 * fSlow29));
		double 	fSlow86 = (0 - (0.1259881623 * fSlow29));
		double 	fSlow87 = (0.28347631111800004 * fSlow55);
		double 	fSlow88 = (0.35857228434300004 * fSlow3);
		double 	fSlow89 = (0 - (0.219579889581 * fSlow29));
		double 	fSlow90 = (0 - (0.28347631111800004 * fSlow29));
		double 	fSlow91 = (0.06873100824512966 * fSlow10);
		double 	fSlow92 = (0 - (0.13746223165253885 * fSlow26));
		double 	fSlow93 = (fSlow67 + 0.11112);
		double 	fSlow94 = (0.13801301565000001 * fSlow4);
		double 	fSlow95 = (0.31053251232900003 * fSlow4);
		double 	fSlow96 = (0.1259881623 * fSlow4);
		double 	fSlow97 = ((fSlow80 + fSlow81) + 0.11112);
		double 	fSlow98 = (0 - (0.09759004785000003 * fSlow55));
		double 	fSlow99 = (0.28347631111800004 * fSlow4);
		double 	fSlow100 = (0 - (0.219579889581 * fSlow55));
		double 	fSlow101 = (fSlow4 * fSlow13);
		double 	fSlow102 = (0.2151622795194362 * fSlow101);
		double 	fSlow103 = (0.15430333080000003 * fSlow4);
		double 	fSlow104 = (0.17251367312221164 * fSlow4);
		double 	fSlow105 = (0.11340041877160077 * fSlow10);
		double 	fSlow106 = (0 - (0.07172068811905223 * fSlow19));
		double 	fSlow107 = (0.11112 - (0.09259110374400001 * fSlow8));
		double 	fSlow108 = (0 - (0.07172068811905223 * fSlow26));
		double 	fSlow109 = (0.347186102328 * fSlow4);
		double 	fSlow110 = (0 - (0.14344159140038398 * fSlow19));
		double 	fSlow111 = (0.0878395702892508 * fSlow7);
		double 	fSlow112 = (0 - (0.11340041877160077 * fSlow21));
		double 	fSlow113 = (0.1781713652621752 * fSlow10);
		double 	fSlow114 = (0.15936380355000002 * fSlow3);
		double 	fSlow115 = (0.11110937081927783 * fSlow26);
		double 	fSlow116 = (fSlow33 + (0.11112 - fSlow115));
		double 	fSlow117 = (0 - (0.0878395702892508 * fSlow21));
		double 	fSlow118 = (0.0878395702892508 * fSlow6);
		double 	fSlow119 = (0.04536029660600802 * fSlow7);
		double 	fSlow120 = (0.16903075950000004 * fSlow3);
		double 	fSlow121 = ((0.07407290784 * fSlow8) + 0.11112);
		double 	fSlow122 = (0.04536029660600802 * fSlow10);
		double 	fSlow123 = (0 - (0.14344159140038398 * fSlow26));
		double 	fSlow124 = (0.38032316127 * fSlow3);
		double 	fSlow125 = (0.11110937081927783 * fSlow19);
		double 	fSlow126 = (0.04536029660600802 * fSlow6);
		double 	fSlow127 = ((fSlow33 + fSlow115) + 0.11112);
		double 	fSlow128 = (0.31053251232900003 * fSlow55);
		double 	fSlow129 = (0.11340041877160077 * fSlow7);
		double 	fSlow130 = (0.07172068811905223 * fSlow14);
		double 	fSlow131 = (0.11340041877160077 * fSlow38);
		double 	fSlow132 = (0.347186102328 * fSlow55);
		double 	fSlow133 = (0.18184526118496958 * fSlow4);
		double 	fSlow134 = (0.353557081449 * fSlow4);
		double 	fSlow135 = (0.11111100750000003 * fSlow3);
		double 	fSlow136 = (0.13608274215000005 * fSlow4);
		double 	fSlow137 = (0.25000236495 * fSlow3);
		double 	fSlow138 = (0.30618935181900003 * fSlow4);
		double 	fSlow139 = (0.02969561235 * fSlow7);
		double 	fSlow140 = (0.13746223165253885 * fSlow10);
		double 	fSlow141 = (0.14547853605000002 * fSlow3);
		double 	fSlow142 = (0.068731027392 * fSlow8);
		double 	fSlow143 = (0 - (0.02969561235 * fSlow55));
		double 	fSlow144 = (0 - (0.066815822151 * fSlow55));
		double 	fSlow145 = (0.327330107793 * fSlow3);
		double 	fSlow146 = (0.066815822151 * fSlow7);
		double 	fSlow147 = (0.16264988730000002 * fSlow3);
		double 	fSlow148 = (0 - (0.048600210211011685 * fSlow38));
		double 	fSlow149 = (0.097200435264 * fSlow8);
		double 	fSlow150 = (0.048600210211011685 * fSlow26);
		double 	fSlow151 = (fSlow149 + fSlow150);
		double 	fSlow152 = (0 - (0.048600210211011685 * fSlow19));
		double 	fSlow153 = (0 - (0.05143437945000002 * fSlow55));
		double 	fSlow154 = (0.365966049618 * fSlow3);
		double 	fSlow155 = (0 - (0.11572855643700003 * fSlow55));
		double 	fSlow156 = (0 - (0.10286875890000004 * fSlow29));
		double 	fSlow157 = (0.030737437765308104 * fSlow11);
		double 	fSlow158 = (0 - (0.12294996622351191 * fSlow38));
		double 	fSlow159 = (0 - (0.12294996622351191 * fSlow21));
		double 	fSlow160 = (0.10647773203199999 * fSlow8);
		double 	fSlow161 = (0.16798421640000002 * fSlow3);
		double 	fSlow162 = (0 - (0.10286875890000004 * fSlow55));
		double 	fSlow163 = (0 - (0.23145711287400006 * fSlow29));
		double 	fSlow164 = (0.377968414824 * fSlow3);
		double 	fSlow165 = (0 - (0.23145711287400006 * fSlow55));
		double 	fSlow166 = (0 - (0.05143437945000002 * fSlow29));
		double 	fSlow167 = (fSlow149 - fSlow150);
		double 	fSlow168 = (0 - (0.048600210211011685 * fSlow21));
		double 	fSlow169 = (0.1328032017 * fSlow4);
		double 	fSlow170 = (0.16835609239505758 * fSlow10);
		double 	fSlow171 = (0.29881030912199996 * fSlow55);
		double 	fSlow172 = (0 - (0.11572855643700003 * fSlow29));
		double 	fSlow173 = (0 - (0.11501081430000001 * fSlow29));
		double 	fSlow174 = (0 - (0.02969561235 * fSlow29));
		double 	fSlow175 = (0 - (0.13746223165253885 * fSlow21));
		double 	fSlow176 = (0.11501081430000001 * fSlow4);
		double 	fSlow177 = (0 - (0.25877702143799997 * fSlow29));
		double 	fSlow178 = (0 - (0.066815822151 * fSlow29));
		double 	fSlow179 = (0.25877702143799997 * fSlow55);
		double 	fSlow180 = (0 - (0.13608274215000005 * fSlow29));
		double 	fSlow181 = (0 - (0.12858399051272834 * fSlow19));
		double 	fSlow182 = (0 - (0.12858399051272834 * fSlow21));
		double 	fSlow183 = (0.13608274215000005 * fSlow55);
		double 	fSlow184 = (0 - (0.30618935181900003 * fSlow29));
		double 	fSlow185 = (0.30618935181900003 * fSlow55);
		double 	fSlow186 = (0.18184526118496958 * fSlow101);
		double 	fSlow187 = (0.15713484765000002 * fSlow6);
		double 	fSlow188 = (0 - (0.18184526118496958 * fSlow19));
		double 	fSlow189 = (0 - (0.15713484765000002 * fSlow29));
		double 	fSlow190 = (0 - (0.353557081449 * fSlow29));
		double 	fSlow191 = (0.353557081449 * fSlow55);
		double 	fSlow192 = (0.18727466614636304 * fSlow11);
		double 	fSlow193 = (0.14506240885200392 * fSlow11);
		double 	fSlow194 = (0.11844296195441828 * fSlow10);
		double 	fSlow195 = (0.055834588607999995 * fSlow8);
		double 	fSlow196 = (0.1367661816782935 * fSlow10);
		double 	fSlow197 = (0.10812313354182758 * fSlow11);
		double 	fSlow198 = (0 - (0.034191437838433605 * fSlow38));
		double 	fSlow199 = (0.0837518208 * fSlow8);
		double 	fSlow200 = (0.10812313354182758 * fSlow19);
		double 	fSlow201 = (0.034191437838433605 * fSlow7);
		double 	fSlow202 = (0 - (0.012923184413636137 * fSlow19));
		double 	fSlow203 = (0.012923184413636137 * fSlow26);
		double 	fSlow204 = (0 - (0.0633104249371965 * fSlow38));
		double 	fSlow205 = (0.100102556352 * fSlow8);
		double 	fSlow206 = (0.05005126462409077 * fSlow26);
		double 	fSlow207 = (fSlow205 + fSlow206);
		double 	fSlow208 = (0 - (0.05005126462409077 * fSlow19));
		double 	fSlow209 = (0.04086663239824459 * fSlow11);
		double 	fSlow210 = (0 - (0.12923184413636135 * fSlow38));
		double 	fSlow211 = (0 - (0.12923184413636135 * fSlow21));
		double 	fSlow212 = (0.10551735628799999 * fSlow8);
		double 	fSlow213 = (fSlow205 - fSlow206);
		double 	fSlow214 = (0 - (0.0633104249371965 * fSlow21));
		double 	fSlow215 = (0.1634667447552579 * fSlow38);
		double 	fSlow216 = (0.012923184413636137 * fSlow11);
		double 	fSlow217 = (0.10812313354182758 * fSlow26);
		double 	fSlow218 = (0 - (0.13242313106619366 * fSlow21));
		double 	fSlow219 = (0 - (0.034191437838433605 * fSlow21));
		double 	fSlow220 = (0.13242313106619366 * fSlow38);
		double 	fSlow221 = (0 - fSlow200);
		double 	fSlow222 = (0 - (0.1367661816782935 * fSlow21));
		double 	fSlow223 = (0.1367661816782935 * fSlow38);
		double 	fSlow224 = (0 - (0.14506240885200392 * fSlow19));
		double 	fSlow225 = (0 - (0.11844296195441828 * fSlow21));
		double 	fSlow226 = (0.11844296195441828 * fSlow38);
		double 	fSlow227 = (0.14506240885200392 * fSlow26);
		double 	fSlow228 = (0 - (0.18727466614636304 * fSlow19));
		double 	fSlow229 = (0.18727466614636304 * fSlow26);
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
		for (int i=0; i<count; i++) {
			double fTemp0 = (double)input2[i];
			double fTemp1 = (double)input3[i];
			double fTemp2 = (double)input1[i];
			double fTemp3 = ((fSlow6 * fTemp1) + (fSlow7 * fTemp2));
			double fTemp4 = ((fSlow3 * fTemp0) + (fSlow4 * fTemp3));
			double fTemp5 = (double)input0[i];
			double fTemp6 = (double)input6[i];
			double fTemp7 = (double)input5[i];
			double fTemp8 = (double)input7[i];
			double fTemp9 = ((fSlow7 * fTemp7) + (fSlow6 * fTemp8));
			double fTemp10 = (double)input8[i];
			double fTemp11 = (double)input4[i];
			double fTemp12 = ((fSlow13 * fTemp10) + (fSlow14 * fTemp11));
			output0[i] = (FAUSTFLOAT)((iSlow1)?(((0.19245000000000004 * fTemp4) + ((0.11112 * fTemp5) + (fSlow9 * fTemp6))) + (0.2151622795194362 * ((fSlow10 * fTemp9) + (fSlow11 * fTemp12)))):((iSlow15)?((0.24993 * fTemp5) + (0.43301700000000004 * fTemp4)):fTemp5));
			double fTemp13 = (double)input9[i];
			double fTemp14 = (double)input15[i];
			double fTemp15 = (double)input14[i];
			double fTemp16 = (double)input13[i];
			double fTemp17 = (double)input12[i];
			double fTemp18 = (double)input11[i];
			double fTemp19 = (fSlow32 * fTemp0);
			double fTemp20 = (double)input10[i];
			output1[i] = (FAUSTFLOAT)((iSlow1)?((fSlow4 * ((fSlow16 * fTemp11) + (fSlow4 * ((fSlow17 * fTemp13) + (fSlow18 * fTemp1))))) + ((fSlow20 * fTemp14) + ((fSlow22 * fTemp15) + ((fSlow23 * fTemp16) + ((fSlow24 * fTemp17) + ((fSlow28 * fTemp18) + (((fSlow30 * fTemp10) + (((fSlow7 * ((fSlow31 * fTemp5) + fTemp19)) + ((fSlow35 * fTemp2) + (fSlow36 * fTemp7))) + (fSlow37 * fTemp6))) + (fSlow39 * fTemp20)))))))):((iSlow15)?(((((0.24993 * fTemp2) + (fSlow4 * ((fSlow40 * fTemp5) + (fSlow41 * fTemp11)))) + (fSlow42 * fTemp7)) + (fSlow43 * fTemp6)) + (fSlow44 * fTemp10)):fTemp2));
			double fTemp21 = ((fSlow13 * fTemp15) + (fSlow14 * fTemp20));
			double fTemp22 = ((fSlow7 * fTemp18) + (fSlow6 * fTemp16));
			double fTemp23 = (0.43301700000000004 * fTemp5);
			output2[i] = (FAUSTFLOAT)((iSlow1)?((fSlow4 * ((0.14907119265000002 * fTemp9) + (fSlow45 * fTemp21))) + ((((fSlow46 * fTemp0) + (fSlow3 * ((0.19245000000000004 * fTemp5) + (0.17213247615000002 * fTemp6)))) + (fSlow47 * fTemp17)) + (fSlow10 * ((0.16666405622891672 * fTemp3) + (0.1781713652621752 * fTemp22))))):((iSlow15)?(((0.24993 * fTemp0) + (fSlow3 * (fTemp23 + (0.387302096259 * fTemp6)))) + (fSlow48 * fTemp9)):fTemp0));
			double fTemp24 = ((fSlow7 * fTemp20) + (fSlow6 * fTemp15));
			output3[i] = (FAUSTFLOAT)((iSlow1)?((fSlow4 * (((fSlow49 * fTemp5) + (fSlow50 * fTemp11)) + (fSlow4 * ((fSlow14 * ((0.16666405622891672 * fTemp2) + (0.17251367312221164 * fTemp13))) + (fSlow17 * fTemp14))))) + ((fSlow51 * fTemp24) + ((fSlow52 * fTemp16) + ((fSlow53 * fTemp17) + (((fSlow6 * (fTemp19 + (fSlow54 * fTemp10))) + ((fSlow56 * fTemp6) + ((fSlow57 * fTemp1) + (fSlow36 * fTemp8)))) + (fSlow23 * fTemp18)))))):((iSlow15)?(((fSlow42 * fTemp8) + ((0.24993 * fTemp1) + (fSlow58 * fTemp6))) + (fSlow4 * ((fSlow59 * fTemp11) + (fSlow6 * (fTemp23 + (0.335413669149 * fTemp10)))))):fTemp1));
			double fTemp25 = ((fSlow7 * fTemp1) + (fSlow6 * fTemp2));
			double fTemp26 = ((fSlow7 * fTemp8) + (fSlow6 * fTemp7));
			output4[i] = (FAUSTFLOAT)((iSlow1)?((fSlow4 * ((fSlow61 * fTemp5) + ((fSlow62 * fTemp13) + (0.14907119265000002 * fTemp25)))) + ((fSlow63 * fTemp14) + ((fSlow64 * fTemp16) + (((fSlow65 * fTemp26) + ((fSlow66 * fTemp6) + ((fSlow68 * fTemp11) + (fSlow69 * fTemp20)))) + (fSlow70 * fTemp18))))):((iSlow15)?((((((0.24993 * fTemp11) + (fSlow4 * ((0.335413669149 * fTemp25) + (fSlow71 * fTemp13)))) + (fSlow72 * fTemp20)) + (fSlow73 * fTemp18)) + (fSlow74 * fTemp16)) + (fSlow75 * fTemp14)):fTemp11));
			double fTemp27 = (fSlow54 * fTemp0);
			output5[i] = (FAUSTFLOAT)((iSlow1)?((((((fSlow76 * fTemp10) + ((fSlow10 * ((fSlow77 * fTemp11) + (fSlow78 * fTemp6))) + ((fSlow7 * (fTemp27 + (fSlow79 * fTemp5))) + (fSlow82 * fTemp7)))) + (fSlow4 * ((fSlow83 * fTemp8) + (fSlow84 * fTemp20)))) + (fSlow85 * fTemp17)) + (fSlow3 * ((0.14907119265000002 * fTemp2) + (0.15936380355000002 * fTemp18)))) + (fSlow86 * fTemp15)):((iSlow15)?((((((0.335413669149 * ((fSlow3 * fTemp2) + (fSlow29 * fTemp0))) + (0.24993 * fTemp7)) + (fSlow87 * fTemp20)) + (fSlow88 * fTemp18)) + (fSlow89 * fTemp17)) + (fSlow90 * fTemp15)):fTemp7));
			output6[i] = (FAUSTFLOAT)((iSlow1)?((fSlow3 * ((0.16903075950000004 * fTemp17) + (0.17213247615000002 * fTemp0))) + ((fSlow91 * fTemp9) + ((fSlow92 * fTemp10) + ((fSlow93 * fTemp6) + ((fSlow66 * fTemp11) + ((fSlow56 * fTemp1) + (((fSlow9 * fTemp5) + (fSlow37 * fTemp2)) + (fSlow94 * fTemp22)))))))):((iSlow15)?(((fSlow3 * ((0.387302096259 * fTemp0) + (0.38032316127 * fTemp17))) + ((fSlow58 * fTemp1) + ((fSlow43 * fTemp2) + (0.24993 * fTemp6)))) + (fSlow95 * fTemp22)):fTemp6));
			output7[i] = (FAUSTFLOAT)((iSlow1)?((((((fSlow6 * ((fSlow10 * (((0.06873100824512966 * fTemp6) + (0.2151622795194362 * fTemp5)) + (0.11904563149935221 * fTemp10))) + fTemp27)) + (fSlow96 * fTemp24)) + (0.11904563149935221 * ((fSlow21 * fTemp11) + (fSlow19 * fTemp7)))) + (fSlow97 * fTemp8)) + (fSlow98 * fTemp17)) + (fSlow3 * ((0.14907119265000002 * fTemp1) + (0.15936380355000002 * fTemp16)))):((iSlow15)?((fSlow99 * fTemp24) + ((fSlow88 * fTemp16) + (((0.335413669149 * ((fSlow55 * fTemp0) + (fSlow3 * fTemp1))) + (0.24993 * fTemp8)) + (fSlow100 * fTemp17)))):fTemp8));
			output8[i] = (FAUSTFLOAT)((iSlow1)?((fSlow4 * (((fSlow102 * fTemp5) + (fSlow16 * fTemp1)) + (fSlow7 * ((0.15430333080000003 * fTemp13) + (0.03984099900000001 * fTemp18))))) + (((fSlow70 * fTemp16) + ((fSlow68 * fTemp10) + ((fSlow92 * fTemp6) + ((fSlow76 * fTemp7) + ((fSlow30 * fTemp2) + (fSlow69 * fTemp15)))))) + (fSlow6 * ((fSlow103 * fTemp14) + (fSlow65 * fTemp8))))):((iSlow15)?(((fSlow73 * fTemp16) + (((fSlow44 * fTemp2) + (0.24993 * fTemp10)) + (fSlow72 * fTemp15))) + (fSlow4 * ((fSlow7 * ((0.347186102328 * fTemp13) + (0.08964317934 * fTemp18))) + (fSlow6 * ((0.347186102328 * fTemp14) + (0.335413669149 * fTemp1)))))):fTemp10));
			double fTemp28 = ((fSlow6 * fTemp11) + (fSlow7 * fTemp10));
			double fTemp29 = ((fSlow6 * fTemp20) + (fSlow7 * fTemp15));
			output9[i] = (FAUSTFLOAT)((iSlow1)?((fSlow4 * ((0.15430333080000003 * fTemp28) + (fSlow104 * ((fSlow14 * fTemp1) + (fSlow13 * fTemp2))))) + ((fSlow105 * fTemp29) + ((fSlow106 * fTemp16) + ((fSlow107 * fTemp13) + (fSlow108 * fTemp18))))):((iSlow15)?((0.24993 * fTemp13) + (fSlow109 * fTemp28)):fTemp13));
			double fTemp30 = ((fSlow3 * fTemp11) + (fSlow4 * fTemp26));
			output10[i] = (FAUSTFLOAT)((iSlow1)?((((0.14085684693651698 * ((fSlow19 * fTemp0) + (fSlow10 * fTemp25))) + ((0.1259881623 * fTemp30) + ((0.11112 * fTemp20) + (fSlow110 * fTemp17)))) + (fSlow10 * ((fSlow111 * fTemp16) + (fSlow6 * ((0.11340041877160077 * fTemp13) + (0.0878395702892508 * fTemp18)))))) + (fSlow112 * fTemp14)):((iSlow15)?((0.24993 * fTemp20) + (0.28347631111800004 * fTemp30)):fTemp20));
			output11[i] = (FAUSTFLOAT)((iSlow1)?((((((((((fSlow7 * ((fSlow4 * ((0.13801301565000001 * fTemp6) + (0.03984099900000001 * fTemp10))) + (fSlow113 * fTemp0))) + (fSlow114 * fTemp7)) + (fSlow28 * fTemp2)) + (fSlow23 * fTemp1)) + (fSlow70 * fTemp11)) + (fSlow108 * fTemp13)) + (fSlow116 * fTemp18)) + (fSlow117 * fTemp15)) + (fSlow10 * ((fSlow118 * fTemp20) + (fSlow119 * fTemp17)))) + (fSlow19 * ((0.07172068811905223 * fTemp14) + (0.11110937081927783 * fTemp16)))):((iSlow15)?(((fSlow73 * fTemp11) + ((fSlow88 * fTemp7) + (0.24993 * fTemp18))) + (fSlow29 * ((0.08964317934 * fTemp10) + (0.31053251232900003 * fTemp6)))):fTemp18));
			output12[i] = (FAUSTFLOAT)((iSlow1)?(((((((fSlow85 * fTemp7) + ((fSlow53 * fTemp1) + ((fSlow47 * fTemp0) + ((fSlow24 * fTemp2) + (fSlow120 * fTemp6))))) + (fSlow98 * fTemp8)) + (fSlow110 * fTemp20)) + (fSlow121 * fTemp17)) + (fSlow122 * fTemp22)) + (fSlow123 * fTemp15)):((iSlow15)?((((fSlow124 * fTemp6) + (0.24993 * fTemp17)) + (fSlow100 * fTemp8)) + (fSlow89 * fTemp7)):fTemp17));
			output13[i] = (FAUSTFLOAT)((iSlow1)?(((((((((((fSlow6 * ((fSlow10 * ((0.1781713652621752 * fTemp0) + (0.0878395702892508 * fTemp15))) + (fSlow94 * fTemp6))) + (fSlow114 * fTemp8)) + (fSlow23 * fTemp2)) + (fSlow52 * fTemp1)) + (fSlow64 * fTemp11)) + (fSlow70 * fTemp10)) + (fSlow106 * fTemp13)) + (fSlow125 * fTemp18)) + (fSlow10 * ((fSlow111 * fTemp20) + (fSlow126 * fTemp17)))) + (fSlow127 * fTemp16)) + (fSlow108 * fTemp14)):((iSlow15)?((fSlow73 * fTemp10) + ((fSlow128 * fTemp6) + ((fSlow74 * fTemp11) + ((fSlow88 * fTemp8) + (0.24993 * fTemp16))))):fTemp16));
			double fTemp31 = ((fSlow55 * fTemp8) + (fSlow3 * fTemp10));
			output14[i] = (FAUSTFLOAT)((iSlow1)?((((((((fSlow10 * ((fSlow6 * ((0.0878395702892508 * fTemp16) + (0.11340041877160077 * fTemp14))) + (fSlow129 * fTemp13))) + (0.11112 * fTemp15)) + (0.1259881623 * fTemp31)) + (fSlow22 * fTemp2)) + (fSlow86 * fTemp7)) + (0.14085684693651698 * ((fSlow38 * fTemp1) + (fSlow26 * fTemp0)))) + (fSlow123 * fTemp17)) + (fSlow117 * fTemp18)):((iSlow15)?((0.28347631111800004 * fTemp31) + ((fSlow90 * fTemp7) + (0.24993 * fTemp15))):fTemp15));
			output15[i] = (FAUSTFLOAT)((iSlow1)?(((((((fSlow4 * ((fSlow4 * ((fSlow130 * fTemp18) + (fSlow17 * fTemp1))) + (fSlow62 * fTemp10))) + (fSlow131 * fTemp15)) + (fSlow20 * fTemp2)) + (fSlow63 * fTemp11)) + (fSlow112 * fTemp20)) + (fSlow108 * fTemp16)) + (fSlow107 * fTemp14)):((iSlow15)?((fSlow75 * fTemp11) + ((fSlow132 * fTemp10) + (0.24993 * fTemp14))):fTemp14));
			double fTemp32 = ((fSlow6 * fTemp13) + (fSlow7 * fTemp14));
			output16[i] = (FAUSTFLOAT)((iSlow1)?(fSlow4 * ((0.15713484765000002 * fTemp32) + (fSlow133 * ((fSlow13 * fTemp11) + (fSlow14 * fTemp10))))):((iSlow15)?(fSlow134 * fTemp32):0));
			output17[i] = (FAUSTFLOAT)((iSlow1)?(((0.12858399051272834 * ((fSlow10 * fTemp28) + (fSlow11 * ((fSlow14 * fTemp8) + (fSlow13 * fTemp7))))) + (fSlow135 * fTemp13)) + (fSlow136 * fTemp29)):((iSlow15)?((fSlow137 * fTemp13) + (fSlow138 * fTemp29)):0));
			double fTemp33 = ((fSlow7 * fTemp16) + (fSlow6 * fTemp18));
			output18[i] = (FAUSTFLOAT)((iSlow1)?((((fSlow4 * (((0.11501081430000001 * fTemp33) + (fSlow139 * fTemp14)) + (fSlow83 * fTemp6))) + ((fSlow140 * fTemp26) + (fSlow141 * fTemp20))) + (fSlow142 * fTemp11)) + (fSlow143 * fTemp13)):((iSlow15)?(((fSlow144 * fTemp13) + (fSlow145 * fTemp20)) + (fSlow4 * ((fSlow146 * fTemp14) + (0.25877702143799997 * fTemp33)))):0));
			output19[i] = (FAUSTFLOAT)((iSlow1)?((((((fSlow7 * ((fSlow4 * ((0.1328032017 * fTemp17) + (0.05143437945000002 * fTemp15))) + (fSlow10 * ((0.16835609239505758 * fTemp6) + (0.048600210211011685 * fTemp10))))) + (fSlow147 * fTemp18)) + (fSlow148 * fTemp11)) + (fSlow151 * fTemp7)) + (fSlow152 * fTemp8)) + (fSlow153 * fTemp20)):((iSlow15)?(((fSlow154 * fTemp18) + (fSlow155 * fTemp20)) + (fSlow29 * ((0.11572855643700003 * fTemp15) + (0.29881030912199996 * fTemp17)))):0));
			output20[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow156 * fTemp18) + ((fSlow157 * fTemp12) + ((fSlow158 * fTemp8) + ((fSlow159 * fTemp7) + ((fSlow160 * fTemp6) + (fSlow161 * fTemp17)))))) + (fSlow162 * fTemp16)):((iSlow15)?(((fSlow163 * fTemp18) + (fSlow164 * fTemp17)) + (fSlow165 * fTemp16)):0));
			output21[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow153 * fTemp15) + ((fSlow166 * fTemp20) + ((fSlow148 * fTemp10) + ((fSlow167 * fTemp8) + ((fSlow152 * fTemp7) + ((fSlow168 * fTemp11) + (fSlow147 * fTemp16))))))) + (fSlow6 * ((fSlow169 * fTemp17) + (fSlow170 * fTemp6)))):((iSlow15)?(((fSlow171 * fTemp17) + ((fSlow172 * fTemp20) + (fSlow154 * fTemp16))) + (fSlow155 * fTemp15)):0));
			output22[i] = (FAUSTFLOAT)((iSlow1)?((((fSlow173 * fTemp18) + ((fSlow174 * fTemp13) + ((fSlow142 * fTemp10) + ((fSlow81 * fTemp6) + ((fSlow175 * fTemp7) + (fSlow141 * fTemp15)))))) + (fSlow143 * fTemp14)) + (fSlow6 * ((fSlow140 * fTemp8) + (fSlow176 * fTemp16)))):((iSlow15)?(((fSlow177 * fTemp18) + ((fSlow178 * fTemp13) + ((fSlow179 * fTemp16) + (fSlow145 * fTemp15)))) + (fSlow144 * fTemp14)):0));
			output23[i] = (FAUSTFLOAT)((iSlow1)?((fSlow180 * fTemp20) + ((0.12858399051272834 * ((fSlow38 * fTemp10) + (fSlow26 * fTemp8))) + ((fSlow181 * fTemp7) + ((fSlow182 * fTemp11) + ((fSlow183 * fTemp15) + (fSlow135 * fTemp14)))))):((iSlow15)?((fSlow137 * fTemp14) + ((fSlow184 * fTemp20) + (fSlow185 * fTemp15))):0));
			output24[i] = (FAUSTFLOAT)((iSlow1)?((fSlow4 * ((fSlow186 * fTemp10) + (fSlow187 * fTemp14))) + ((fSlow188 * fTemp11) + (fSlow189 * fTemp13))):((iSlow15)?((fSlow190 * fTemp13) + (fSlow191 * fTemp14)):0));
			output25[i] = (FAUSTFLOAT)((iSlow1)?(fSlow192 * ((fSlow14 * fTemp14) + (fSlow13 * fTemp13))):0);
			output26[i] = (FAUSTFLOAT)((iSlow1)?((fSlow193 * ((fSlow13 * fTemp20) + (fSlow14 * fTemp15))) + (fSlow194 * fTemp32)):0);
			output27[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow195 * fTemp13) + (fSlow196 * fTemp29)) + (fSlow197 * ((fSlow14 * fTemp16) + (fSlow13 * fTemp18)))):0);
			output28[i] = (FAUSTFLOAT)((iSlow1)?((((fSlow198 * fTemp13) + (fSlow199 * fTemp20)) + (fSlow200 * fTemp17)) + (fSlow10 * ((fSlow201 * fTemp14) + (0.13242313106619366 * fTemp33)))):0);
			output29[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow202 * fTemp14) + ((((fSlow203 * fTemp13) + (fSlow204 * fTemp20)) + (fSlow207 * fTemp18)) + (fSlow208 * fTemp16))) + (fSlow21 * ((0.1634667447552579 * fTemp17) + (0.0633104249371965 * fTemp15)))):0);
			output30[i] = (FAUSTFLOAT)((iSlow1)?((fSlow209 * fTemp21) + ((fSlow210 * fTemp16) + ((fSlow211 * fTemp18) + (fSlow212 * fTemp17)))):0);
			output31[i] = (FAUSTFLOAT)((iSlow1)?(((fSlow204 * fTemp15) + ((fSlow213 * fTemp16) + ((fSlow208 * fTemp18) + ((fSlow214 * fTemp20) + (fSlow215 * fTemp17))))) + (fSlow216 * ((fSlow14 * fTemp13) + (fSlow13 * fTemp14)))):0);
			output32[i] = (FAUSTFLOAT)((iSlow1)?((fSlow198 * fTemp14) + ((fSlow199 * fTemp15) + ((fSlow217 * fTemp17) + ((fSlow218 * fTemp18) + ((fSlow219 * fTemp13) + (fSlow220 * fTemp16)))))):0);
			output33[i] = (FAUSTFLOAT)((iSlow1)?((fSlow195 * fTemp14) + ((fSlow217 * fTemp16) + ((fSlow221 * fTemp18) + ((fSlow222 * fTemp20) + (fSlow223 * fTemp15))))):0);
			output34[i] = (FAUSTFLOAT)((iSlow1)?((fSlow224 * fTemp20) + ((fSlow225 * fTemp13) + ((fSlow226 * fTemp14) + (fSlow227 * fTemp15)))):0);
			output35[i] = (FAUSTFLOAT)((iSlow1)?((fSlow228 * fTemp13) + (fSlow229 * fTemp14)):0);
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
