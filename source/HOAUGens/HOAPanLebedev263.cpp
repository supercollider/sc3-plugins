/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAPanLebedev263"
version: "1.0"
Code generated with Faust 2.2.0 (http://faust.grame.fr)
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

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

// If other than 'faust2sc --prefix Faust' is used, sed this as well:
#if !defined(SC_FAUST_PREFIX)
#define SC_FAUST_PREFIX "Faust"
#endif

#include <map>
#include <string>
#include <string.h>
#include <SC_PlugIn.h>

/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
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
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

class UI;
struct Meta;

/**
 * DSP memory manager.
 */

struct dsp_memory_manager {
    
    virtual ~dsp_memory_manager() {}
    
    virtual void* allocate(size_t size) = 0;
    virtual void destroy(void* ptr) = 0;
    
};

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
         * Trigger the ui_interface parameter with instance specific calls
         * to 'addBtton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
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
         * @param samplingRate - the sampling rate in Hertz
         */
        virtual void instanceInit(int samplingRate) = 0;

        /** Init instance constant state
         *
         * @param samplingRate - the sampling rate in Hertz
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
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the nomber of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
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
        // Beware: subclasses usually have to overload the two 'compute' methods
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
    
};

/**
 * DSP factory class.
 */

class dsp_factory {
    
    protected:
    
        // So that to force sub-classes to use deleteDSPFactory(dsp_factory* factory);
        virtual ~dsp_factory() {}
    
    public:
    
        virtual std::string getName() = 0;
        virtual std::string getSHAKey() = 0;
        virtual std::string getDSPCode() = 0;
        virtual dsp* createDSPInstance() = 0;
    
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
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
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
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
 ************************************************************************/

#ifndef FAUST_UI_H
#define FAUST_UI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
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

#endif
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
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
 ************************************************************************/
 
#ifndef __misc__
#define __misc__

#include <algorithm>
#include <map>
#include <string.h>
#include <stdlib.h>

/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
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
    #define FAUST_EXPORT __attribute__((visibility("default")))
#else
    #define FAUST_EXPORT  SC_API_EXPORT
#endif

#ifdef WIN32
    #define STRDUP _strdup
#else
    #define STRDUP strdup
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

#include <math.h>

double cos(double dummy0);
double sin(double dummy0);
double pow(double dummy0, double dummy1);
double mydsp_faustpower2_f(double value) {
	return (value * value);
	
}
double fabs(double dummy0);
double log10(double dummy0);
double mydsp_faustpower3_f(double value) {
	return ((value * value) * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
	
 private:
	
	int fSamplingFreq;
	double fConst0;
	double fConst1;
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fCheckbox0;
	double fConst2;
	double fConst3;
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fEntry0;
	FAUSTFLOAT fCheckbox1;
	FAUSTFLOAT fHslider3;
	double fRec3[2];
	double fRec4[2];
	double fRec2[2];
	double fRec1[2];
	double fRec6[2];
	double fRec5[2];
	FAUSTFLOAT fCheckbox2;
	double fConst4;
	double fConst5;
	double fConst6;
	double fConst7;
	double fRec10[2];
	double fRec11[2];
	double fRec9[2];
	double fRec12[2];
	double fVec0[2];
	double fRec13[2];
	double fRec14[2];
	double fRec8[2];
	double fRec7[2];
	double fRec17[2];
	double fRec18[2];
	double fRec16[2];
	double fRec19[2];
	double fRec15[2];
	FAUSTFLOAT fCheckbox3;
	FAUSTFLOAT fCheckbox4;
	double fConst8;
	double fConst9;
	double fRec22[2];
	double fRec23[2];
	double fRec21[2];
	double fRec20[2];
	double fVec1[2];
	double fRec24[2];
	double fRec26[2];
	double fRec27[2];
	double fRec25[2];
	double fRec0[2];
	FAUSTFLOAT fVbargraph0;
	double fRec28[2];
	FAUSTFLOAT fVbargraph1;
	double fRec29[2];
	FAUSTFLOAT fVbargraph2;
	double fRec30[2];
	FAUSTFLOAT fVbargraph3;
	double fRec31[2];
	FAUSTFLOAT fVbargraph4;
	double fRec32[2];
	FAUSTFLOAT fVbargraph5;
	double fRec33[2];
	FAUSTFLOAT fVbargraph6;
	double fRec34[2];
	FAUSTFLOAT fVbargraph7;
	double fRec35[2];
	FAUSTFLOAT fVbargraph8;
	double fRec36[2];
	FAUSTFLOAT fVbargraph9;
	double fRec37[2];
	FAUSTFLOAT fVbargraph10;
	double fRec38[2];
	FAUSTFLOAT fVbargraph11;
	double fRec39[2];
	FAUSTFLOAT fVbargraph12;
	double fRec40[2];
	FAUSTFLOAT fVbargraph13;
	double fRec41[2];
	FAUSTFLOAT fVbargraph14;
	double fRec42[2];
	FAUSTFLOAT fVbargraph15;
	double fRec43[2];
	FAUSTFLOAT fVbargraph16;
	double fRec44[2];
	FAUSTFLOAT fVbargraph17;
	double fRec45[2];
	FAUSTFLOAT fVbargraph18;
	double fRec46[2];
	FAUSTFLOAT fVbargraph19;
	double fRec47[2];
	FAUSTFLOAT fVbargraph20;
	double fRec48[2];
	FAUSTFLOAT fVbargraph21;
	double fRec49[2];
	FAUSTFLOAT fVbargraph22;
	double fRec50[2];
	FAUSTFLOAT fVbargraph23;
	double fRec51[2];
	FAUSTFLOAT fVbargraph24;
	double fRec52[2];
	FAUSTFLOAT fVbargraph25;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
		m->declare("lib/lebedev.lib/author", "Pierre Lecomte");
		m->declare("lib/lebedev.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/lebedev.lib/license", "GPL");
		m->declare("lib/lebedev.lib/name", "Lebdev grids and weights");
		m->declare("lib/lebedev.lib/version", "1.0");
		m->declare("lib/nfc.lib/author", "Pierre Lecomte");
		m->declare("lib/nfc.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/nfc.lib/license", "GPL");
		m->declare("lib/nfc.lib/name", "NF And NFC Filters Library");
		m->declare("lib/nfc.lib/version", "1.0");
		m->declare("lib/ymn.lib/author", "Pierre Lecomte");
		m->declare("lib/ymn.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/ymn.lib/license", "GPL");
		m->declare("lib/ymn.lib/name", "Spherical Harmonics library");
		m->declare("lib/ymn.lib/version", "1.0");
		m->declare("license", "GPL");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("name", "HOAPanLebedev263");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 1;
		
	}
	virtual int getNumOutputs() {
		return 26;
		
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			case 1: {
				rate = 1;
				break;
			}
			case 2: {
				rate = 1;
				break;
			}
			case 3: {
				rate = 1;
				break;
			}
			case 4: {
				rate = 1;
				break;
			}
			case 5: {
				rate = 1;
				break;
			}
			case 6: {
				rate = 1;
				break;
			}
			case 7: {
				rate = 1;
				break;
			}
			case 8: {
				rate = 1;
				break;
			}
			case 9: {
				rate = 1;
				break;
			}
			case 10: {
				rate = 1;
				break;
			}
			case 11: {
				rate = 1;
				break;
			}
			case 12: {
				rate = 1;
				break;
			}
			case 13: {
				rate = 1;
				break;
			}
			case 14: {
				rate = 1;
				break;
			}
			case 15: {
				rate = 1;
				break;
			}
			case 16: {
				rate = 1;
				break;
			}
			case 17: {
				rate = 1;
				break;
			}
			case 18: {
				rate = 1;
				break;
			}
			case 19: {
				rate = 1;
				break;
			}
			case 20: {
				rate = 1;
				break;
			}
			case 21: {
				rate = 1;
				break;
			}
			case 22: {
				rate = 1;
				break;
			}
			case 23: {
				rate = 1;
				break;
			}
			case 24: {
				rate = 1;
				break;
			}
			case 25: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	
	static void classInit(int samplingFreq) {
		
	}
	
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(192000.0, max(1000.0, double(fSamplingFreq)));
		fConst1 = (80.0 / fConst0);
		fConst2 = (340.0 / fConst0);
		fConst3 = (170.0 / fConst0);
		fConst4 = (789.54302057287055 / fConst0);
		fConst5 = (394.77151028643527 / fConst0);
		fConst6 = (625.22848971356643 / fConst0);
		fConst7 = (1250.4569794271329 / fConst0);
		fConst8 = (510.0 / fConst0);
		fConst9 = (1020.0 / fConst0);
		
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.0);
		fHslider1 = FAUSTFLOAT(0.0);
		fCheckbox0 = FAUSTFLOAT(0.0);
		fHslider2 = FAUSTFLOAT(2.0);
		fEntry0 = FAUSTFLOAT(1.0700000000000001);
		fCheckbox1 = FAUSTFLOAT(0.0);
		fHslider3 = FAUSTFLOAT(0.0);
		fCheckbox2 = FAUSTFLOAT(0.0);
		fCheckbox3 = FAUSTFLOAT(0.0);
		fCheckbox4 = FAUSTFLOAT(0.0);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec3[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec4[l1] = 0.0;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec2[l2] = 0.0;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec1[l3] = 0.0;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec6[l4] = 0.0;
			
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec5[l5] = 0.0;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec10[l6] = 0.0;
			
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec11[l7] = 0.0;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec9[l8] = 0.0;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fRec12[l9] = 0.0;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fVec0[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec13[l11] = 0.0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec14[l12] = 0.0;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec8[l13] = 0.0;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec7[l14] = 0.0;
			
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec17[l15] = 0.0;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fRec18[l16] = 0.0;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec16[l17] = 0.0;
			
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec19[l18] = 0.0;
			
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec15[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec22[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec23[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec21[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec20[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fVec1[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec24[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec26[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec27[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec25[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec0[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec28[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec29[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec30[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec31[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec32[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec33[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec34[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec35[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec36[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec37[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec38[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec39[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec40[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec41[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec42[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec43[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec44[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec45[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec46[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec47[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec48[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec49[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec50[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec51[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec52[l54] = 0.0;
			
		}
		
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
		ui_interface->openVerticalBox("HOAPanLebedev263");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fHslider3, "0+1", "");
		ui_interface->declare(&fHslider3, "osc", "/gain_0 -20 20");
		ui_interface->declare(&fHslider3, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  0", &fHslider3, 0.0, -30.0, 20.0, 0.10000000000000001);
		ui_interface->declare(&fHslider2, "0+2", "");
		ui_interface->declare(&fHslider2, "osc", "/radius_0 0.5 50");
		ui_interface->declare(&fHslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  0", &fHslider2, 2.0, 0.5, 50.0, 0.01);
		ui_interface->declare(&fHslider0, "0+3", "");
		ui_interface->declare(&fHslider0, "osc", "/azimuth_0 0 360");
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth  0", &fHslider0, 0.0, -3.1415926535897931, 3.1415926535897931, 0.10000000000000001);
		ui_interface->declare(&fHslider1, "0+4", "");
		ui_interface->declare(&fHslider1, "osc", "/elevation_0 -90 90");
		ui_interface->declare(&fHslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation  0", &fHslider1, 0.0, -1.5707963267948966, 1.5707963267948966, 0.10000000000000001);
		ui_interface->declare(0, "0+5", "");
		ui_interface->openHorizontalBox("Spherical Wave");
		ui_interface->addCheckButton("Yes", &fCheckbox1);
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openVerticalBox("Mute Order");
		ui_interface->addCheckButton("0", &fCheckbox3);
		ui_interface->addCheckButton("1", &fCheckbox0);
		ui_interface->addCheckButton("2", &fCheckbox4);
		ui_interface->addCheckButton("3", &fCheckbox2);
		ui_interface->closeBox();
		ui_interface->declare(&fEntry0, "~", "");
		ui_interface->addNumEntry("Speaker Radius", &fEntry0, 1.0700000000000001, 0.5, 10.0, 0.01);
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("Outputs");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph0, "osc", "/output1");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x353e620", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "osc", "/output2");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3546d40", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph2, "osc", "/output3");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35501f0", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph3, "osc", "/output4");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3559780", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph4, "osc", "/output5");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3562df0", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph5, "osc", "/output6");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x356ab10", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph6, "osc", "/output7");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3573660", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph7, "osc", "/output8");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x357b930", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph8, "osc", "/output9");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3583ce0", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph9, "osc", "/output10");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x358c180", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph10, "osc", "/output11");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3595d30", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph11, "osc", "/output12");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x359f9c0", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph12, "osc", "/output13");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35a9730", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph13, "osc", "/output14");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35b3580", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph14, "osc", "/output15");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35bba80", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph15, "osc", "/output16");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35c4060", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph16, "osc", "/output17");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35cc720", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph17, "osc", "/output18");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35d4ec0", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph18, "osc", "/output19");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35de490", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph19, "osc", "/output20");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35e71e0", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph20, "osc", "/output21");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35f0010", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph21, "osc", "/output22");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35f8f20", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph22, "osc", "/output23");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3601b20", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph23, "osc", "/output24");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x360a800", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph24, "osc", "/output25");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36135c0", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph25, "osc", "/output26");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x361c460", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		FAUSTFLOAT* output2 = outputs[2];
		FAUSTFLOAT* output3 = outputs[3];
		FAUSTFLOAT* output4 = outputs[4];
		FAUSTFLOAT* output5 = outputs[5];
		FAUSTFLOAT* output6 = outputs[6];
		FAUSTFLOAT* output7 = outputs[7];
		FAUSTFLOAT* output8 = outputs[8];
		FAUSTFLOAT* output9 = outputs[9];
		FAUSTFLOAT* output10 = outputs[10];
		FAUSTFLOAT* output11 = outputs[11];
		FAUSTFLOAT* output12 = outputs[12];
		FAUSTFLOAT* output13 = outputs[13];
		FAUSTFLOAT* output14 = outputs[14];
		FAUSTFLOAT* output15 = outputs[15];
		FAUSTFLOAT* output16 = outputs[16];
		FAUSTFLOAT* output17 = outputs[17];
		FAUSTFLOAT* output18 = outputs[18];
		FAUSTFLOAT* output19 = outputs[19];
		FAUSTFLOAT* output20 = outputs[20];
		FAUSTFLOAT* output21 = outputs[21];
		FAUSTFLOAT* output22 = outputs[22];
		FAUSTFLOAT* output23 = outputs[23];
		FAUSTFLOAT* output24 = outputs[24];
		FAUSTFLOAT* output25 = outputs[25];
		double fSlow0 = double(fHslider0);
		double fSlow1 = cos(fSlow0);
		double fSlow2 = double(fHslider1);
		double fSlow3 = cos(fSlow2);
		double fSlow4 = (fSlow1 * fSlow3);
		double fSlow5 = (6.123233995736766e-17 * fSlow4);
		double fSlow6 = sin(fSlow2);
		double fSlow7 = (fSlow5 + fSlow6);
		double fSlow8 = (1.0 - double(fCheckbox0));
		double fSlow9 = (3.0 * fSlow8);
		double fSlow10 = double(fHslider2);
		double fSlow11 = (fConst3 / fSlow10);
		double fSlow12 = (fSlow11 + 1.0);
		double fSlow13 = (fConst2 / (fSlow12 * fSlow10));
		double fSlow14 = double(fEntry0);
		double fSlow15 = (fConst3 / fSlow14);
		double fSlow16 = (1.0 / (fSlow15 + 1.0));
		double fSlow17 = double(fCheckbox1);
		double fSlow18 = (((fSlow12 * fSlow17) * fSlow14) / fSlow10);
		double fSlow19 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fHslider3))));
		double fSlow20 = (fConst2 / fSlow14);
		double fSlow21 = (1.0 - fSlow17);
		double fSlow22 = (fSlow21 * fSlow14);
		double fSlow23 = mydsp_faustpower2_f(fSlow7);
		double fSlow24 = (1.0 - double(fCheckbox2));
		double fSlow25 = (3.5 * (((5.0 * fSlow23) + -3.0) * fSlow24));
		double fSlow26 = ((fConst5 / fSlow10) + 1.0);
		double fSlow27 = (fConst4 / (fSlow26 * fSlow10));
		double fSlow28 = (1.0 / ((fConst5 / fSlow14) + 1.0));
		double fSlow29 = mydsp_faustpower2_f(fSlow15);
		double fSlow30 = (1.0 / (((fConst6 / fSlow14) + (6.4594326934833797 * fSlow29)) + 1.0));
		double fSlow31 = mydsp_faustpower2_f(fSlow11);
		double fSlow32 = (((fConst6 / fSlow10) + (6.4594326934833797 * fSlow31)) + 1.0);
		double fSlow33 = (((fSlow32 * fSlow17) * fSlow14) / fSlow10);
		double fSlow34 = (25.837730773933519 * fSlow29);
		double fSlow35 = (fSlow34 + (fConst7 / fSlow14));
		double fSlow36 = (1.0 / fSlow32);
		double fSlow37 = (25.837730773933519 * fSlow31);
		double fSlow38 = ((fConst7 / fSlow10) + fSlow37);
		double fSlow39 = (fConst4 / fSlow14);
		double fSlow40 = ((((fSlow17 / fSlow10) + fSlow21) * fSlow14) * (1.0 - double(fCheckbox3)));
		double fSlow41 = (1.0 - double(fCheckbox4));
		double fSlow42 = (2.5 * (((3.0 * fSlow23) + -1.0) * fSlow41));
		double fSlow43 = (((fConst8 / fSlow10) + (3.0 * fSlow31)) + 1.0);
		double fSlow44 = (1.0 / fSlow43);
		double fSlow45 = (12.0 * fSlow31);
		double fSlow46 = ((fConst9 / fSlow10) + fSlow45);
		double fSlow47 = (1.0 / (((fConst8 / fSlow14) + (3.0 * fSlow29)) + 1.0));
		double fSlow48 = (((fSlow43 * fSlow17) * fSlow14) / fSlow10);
		double fSlow49 = (12.0 * fSlow29);
		double fSlow50 = (fSlow49 + (fConst9 / fSlow14));
		double fSlow51 = (3.0 * ((fSlow1 * fSlow8) * fSlow3));
		double fSlow52 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow4)) + -1.0) * fSlow41));
		double fSlow53 = (3.5 * (((5.0 * mydsp_faustpower3_f(fSlow4)) - (3.0 * fSlow4)) * fSlow24));
		double fSlow54 = cos((fSlow0 + -1.5707963267948966));
		double fSlow55 = (3.0 * ((fSlow54 * fSlow8) * fSlow3));
		double fSlow56 = (fSlow54 * fSlow3);
		double fSlow57 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow56)) + -1.0) * fSlow41));
		double fSlow58 = (3.5 * (((5.0 * mydsp_faustpower3_f(fSlow56)) - (3.0 * fSlow56)) * fSlow24));
		double fSlow59 = cos((fSlow0 + -3.1415926535897931));
		double fSlow60 = (3.0 * ((fSlow59 * fSlow8) * fSlow3));
		double fSlow61 = (fSlow59 * fSlow3);
		double fSlow62 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow61)) + -1.0) * fSlow41));
		double fSlow63 = (3.5 * (((5.0 * mydsp_faustpower3_f(fSlow61)) - (3.0 * fSlow61)) * fSlow24));
		double fSlow64 = cos((fSlow0 + -4.7123889803846897));
		double fSlow65 = (3.0 * ((fSlow64 * fSlow8) * fSlow3));
		double fSlow66 = (fSlow64 * fSlow3);
		double fSlow67 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow66)) + -1.0) * fSlow41));
		double fSlow68 = (3.5 * (((5.0 * mydsp_faustpower3_f(fSlow66)) - (3.0 * fSlow66)) * fSlow24));
		double fSlow69 = (fSlow5 - fSlow6);
		double fSlow70 = mydsp_faustpower2_f(fSlow69);
		double fSlow71 = (3.5 * (((5.0 * fSlow70) + -3.0) * fSlow24));
		double fSlow72 = (2.5 * (((3.0 * fSlow70) + -1.0) * fSlow41));
		double fSlow73 = (0.70710678118654757 * fSlow4);
		double fSlow74 = (0.70710678118654746 * fSlow6);
		double fSlow75 = (fSlow73 + fSlow74);
		double fSlow76 = mydsp_faustpower2_f(fSlow75);
		double fSlow77 = (3.5 * (((5.0 * fSlow76) + -3.0) * fSlow24));
		double fSlow78 = (2.5 * (((3.0 * fSlow76) + -1.0) * fSlow41));
		double fSlow79 = (0.70710678118654757 * fSlow56);
		double fSlow80 = (fSlow74 + fSlow79);
		double fSlow81 = mydsp_faustpower2_f(fSlow80);
		double fSlow82 = (3.5 * (((5.0 * fSlow81) + -3.0) * fSlow24));
		double fSlow83 = (2.5 * (((3.0 * fSlow81) + -1.0) * fSlow41));
		double fSlow84 = (0.70710678118654757 * fSlow61);
		double fSlow85 = (fSlow74 + fSlow84);
		double fSlow86 = mydsp_faustpower2_f(fSlow85);
		double fSlow87 = (3.5 * (((5.0 * fSlow86) + -3.0) * fSlow24));
		double fSlow88 = (2.5 * (((3.0 * fSlow86) + -1.0) * fSlow41));
		double fSlow89 = (0.70710678118654757 * fSlow66);
		double fSlow90 = (fSlow74 + fSlow89);
		double fSlow91 = mydsp_faustpower2_f(fSlow90);
		double fSlow92 = (3.5 * (((5.0 * fSlow91) + -3.0) * fSlow24));
		double fSlow93 = (2.5 * (((3.0 * fSlow91) + -1.0) * fSlow41));
		double fSlow94 = cos((fSlow0 + -0.78539816339744828));
		double fSlow95 = (3.0 * ((fSlow94 * fSlow8) * fSlow3));
		double fSlow96 = (fSlow94 * fSlow3);
		double fSlow97 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow96)) + -1.0) * fSlow41));
		double fSlow98 = (3.5 * (((5.0 * mydsp_faustpower3_f(fSlow96)) - (3.0 * fSlow96)) * fSlow24));
		double fSlow99 = cos((fSlow0 + -2.3561944901923448));
		double fSlow100 = (3.0 * ((fSlow99 * fSlow8) * fSlow3));
		double fSlow101 = (fSlow99 * fSlow3);
		double fSlow102 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow101)) + -1.0) * fSlow41));
		double fSlow103 = (3.5 * (((5.0 * mydsp_faustpower3_f(fSlow101)) - (3.0 * fSlow101)) * fSlow24));
		double fSlow104 = cos((fSlow0 + -3.9269908169872414));
		double fSlow105 = (3.0 * ((fSlow104 * fSlow8) * fSlow3));
		double fSlow106 = (fSlow104 * fSlow3);
		double fSlow107 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow106)) + -1.0) * fSlow41));
		double fSlow108 = (3.5 * (((5.0 * mydsp_faustpower3_f(fSlow106)) - (3.0 * fSlow106)) * fSlow24));
		double fSlow109 = cos((fSlow0 + -5.497787143782138));
		double fSlow110 = (3.0 * ((fSlow109 * fSlow8) * fSlow3));
		double fSlow111 = (fSlow109 * fSlow3);
		double fSlow112 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow111)) + -1.0) * fSlow41));
		double fSlow113 = (3.5 * (((5.0 * mydsp_faustpower3_f(fSlow111)) - (3.0 * fSlow111)) * fSlow24));
		double fSlow114 = (fSlow73 - fSlow74);
		double fSlow115 = mydsp_faustpower2_f(fSlow114);
		double fSlow116 = (3.5 * (((5.0 * fSlow115) + -3.0) * fSlow24));
		double fSlow117 = (2.5 * (((3.0 * fSlow115) + -1.0) * fSlow41));
		double fSlow118 = (fSlow79 - fSlow74);
		double fSlow119 = mydsp_faustpower2_f(fSlow118);
		double fSlow120 = (3.5 * (((5.0 * fSlow119) + -3.0) * fSlow24));
		double fSlow121 = (2.5 * (((3.0 * fSlow119) + -1.0) * fSlow41));
		double fSlow122 = (fSlow84 - fSlow74);
		double fSlow123 = mydsp_faustpower2_f(fSlow122);
		double fSlow124 = (3.5 * (((5.0 * fSlow123) + -3.0) * fSlow24));
		double fSlow125 = (2.5 * (((3.0 * fSlow123) + -1.0) * fSlow41));
		double fSlow126 = (fSlow89 - fSlow74);
		double fSlow127 = mydsp_faustpower2_f(fSlow126);
		double fSlow128 = (3.5 * (((5.0 * fSlow127) + -3.0) * fSlow24));
		double fSlow129 = (2.5 * (((3.0 * fSlow127) + -1.0) * fSlow41));
		double fSlow130 = (0.81649658092772592 * fSlow96);
		double fSlow131 = (0.57735026918962584 * fSlow6);
		double fSlow132 = (fSlow130 + fSlow131);
		double fSlow133 = mydsp_faustpower2_f(fSlow132);
		double fSlow134 = (3.5 * (((5.0 * fSlow133) + -3.0) * fSlow24));
		double fSlow135 = (2.5 * (((3.0 * fSlow133) + -1.0) * fSlow41));
		double fSlow136 = (0.81649658092772592 * fSlow101);
		double fSlow137 = (fSlow131 + fSlow136);
		double fSlow138 = mydsp_faustpower2_f(fSlow137);
		double fSlow139 = (3.5 * (((5.0 * fSlow138) + -3.0) * fSlow24));
		double fSlow140 = (2.5 * (((3.0 * fSlow138) + -1.0) * fSlow41));
		double fSlow141 = (0.81649658092772592 * fSlow106);
		double fSlow142 = (fSlow131 + fSlow141);
		double fSlow143 = mydsp_faustpower2_f(fSlow142);
		double fSlow144 = (3.5 * (((5.0 * fSlow143) + -3.0) * fSlow24));
		double fSlow145 = (2.5 * (((3.0 * fSlow143) + -1.0) * fSlow41));
		double fSlow146 = (0.81649658092772592 * fSlow111);
		double fSlow147 = (fSlow131 + fSlow146);
		double fSlow148 = mydsp_faustpower2_f(fSlow147);
		double fSlow149 = (3.5 * (((5.0 * fSlow148) + -3.0) * fSlow24));
		double fSlow150 = (2.5 * (((3.0 * fSlow148) + -1.0) * fSlow41));
		double fSlow151 = (fSlow130 - fSlow131);
		double fSlow152 = mydsp_faustpower2_f(fSlow151);
		double fSlow153 = (3.5 * (((5.0 * fSlow152) + -3.0) * fSlow24));
		double fSlow154 = (2.5 * (((3.0 * fSlow152) + -1.0) * fSlow41));
		double fSlow155 = (fSlow136 - fSlow131);
		double fSlow156 = mydsp_faustpower2_f(fSlow155);
		double fSlow157 = (3.5 * (((5.0 * fSlow156) + -3.0) * fSlow24));
		double fSlow158 = (2.5 * (((3.0 * fSlow156) + -1.0) * fSlow41));
		double fSlow159 = (fSlow141 - fSlow131);
		double fSlow160 = mydsp_faustpower2_f(fSlow159);
		double fSlow161 = (3.5 * (((5.0 * fSlow160) + -3.0) * fSlow24));
		double fSlow162 = (2.5 * (((3.0 * fSlow160) + -1.0) * fSlow41));
		double fSlow163 = (fSlow146 - fSlow131);
		double fSlow164 = mydsp_faustpower2_f(fSlow163);
		double fSlow165 = (3.5 * (((5.0 * fSlow164) + -3.0) * fSlow24));
		double fSlow166 = (2.5 * (((3.0 * fSlow164) + -1.0) * fSlow41));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec3[0] = (fSlow19 + (0.999 * fRec3[1]));
			double fTemp0 = (fRec3[0] * double(input0[i]));
			fRec4[0] = (fRec4[1] + fRec2[1]);
			fRec2[0] = (fSlow16 * ((fSlow18 * fTemp0) - (fSlow20 * fRec4[0])));
			fRec1[0] = (fRec2[0] + fRec1[1]);
			double fTemp1 = (fSlow22 * fTemp0);
			fRec6[0] = (fRec6[1] + fRec5[1]);
			fRec5[0] = (fSlow16 * (fTemp1 - (fSlow20 * fRec6[0])));
			double fTemp2 = ((fSlow13 * (fRec1[0] - fRec2[0])) + (fRec2[0] + fRec5[0]));
			double fTemp3 = (fSlow9 * fTemp2);
			fRec10[0] = (fRec10[1] + fRec9[1]);
			fRec11[0] = (fRec11[1] + fRec10[1]);
			fRec9[0] = (fSlow30 * ((fSlow33 * fTemp0) - ((fSlow35 * fRec10[0]) + (fSlow34 * fRec11[0]))));
			fRec12[0] = (fRec9[0] + fRec12[1]);
			double fTemp4 = (fRec12[0] - fRec9[0]);
			fVec0[0] = fTemp4;
			fRec13[0] = (fRec13[1] + fVec0[1]);
			fRec14[0] = (fRec14[1] + fRec8[1]);
			fRec8[0] = (fSlow28 * ((fSlow26 * (fRec9[0] + (fSlow36 * ((fSlow38 * fTemp4) + (fSlow37 * fRec13[0]))))) - (fSlow39 * fRec14[0])));
			fRec7[0] = (fRec8[0] + fRec7[1]);
			fRec17[0] = (fRec17[1] + fRec16[1]);
			fRec18[0] = (fRec18[1] + fRec17[1]);
			fRec16[0] = (fSlow30 * (fTemp1 - ((fSlow35 * fRec17[0]) + (fSlow34 * fRec18[0]))));
			fRec19[0] = (fRec19[1] + fRec15[1]);
			fRec15[0] = (fSlow28 * (fRec16[0] - (fSlow39 * fRec19[0])));
			double fTemp5 = ((fSlow27 * (fRec7[0] - fRec8[0])) + (fRec8[0] + fRec15[0]));
			double fTemp6 = (fSlow40 * fTemp0);
			fRec22[0] = (fRec22[1] + fRec21[1]);
			fRec23[0] = (fRec23[1] + fRec22[1]);
			fRec21[0] = (fSlow47 * ((fSlow48 * fTemp0) - ((fSlow50 * fRec22[0]) + (fSlow49 * fRec23[0]))));
			fRec20[0] = (fRec21[0] + fRec20[1]);
			double fTemp7 = (fRec20[0] - fRec21[0]);
			fVec1[0] = fTemp7;
			fRec24[0] = (fRec24[1] + fVec1[1]);
			fRec26[0] = (fRec26[1] + fRec25[1]);
			fRec27[0] = (fRec27[1] + fRec26[1]);
			fRec25[0] = (fSlow47 * (fTemp1 - ((fSlow50 * fRec26[0]) + (fSlow49 * fRec27[0]))));
			double fTemp8 = ((fSlow44 * ((fSlow46 * fTemp7) + (fSlow45 * fRec24[0]))) + (fRec21[0] + fRec25[0]));
			double fTemp9 = (0.047619047619047623 * ((fSlow7 * (fTemp3 + (fSlow25 * fTemp5))) + (fTemp6 + (fSlow42 * fTemp8))));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph0 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp9);
			double fTemp10 = (0.047619047619047623 * (((fTemp6 + (fSlow51 * fTemp2)) + (fSlow52 * fTemp8)) + (fSlow53 * fTemp5)));
			fRec28[0] = max((fRec28[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fVbargraph1 = FAUSTFLOAT(fRec28[0]);
			output1[i] = FAUSTFLOAT(fTemp10);
			double fTemp11 = (0.047619047619047623 * (((fTemp6 + (fSlow55 * fTemp2)) + (fSlow57 * fTemp8)) + (fSlow58 * fTemp5)));
			fRec29[0] = max((fRec29[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fVbargraph2 = FAUSTFLOAT(fRec29[0]);
			output2[i] = FAUSTFLOAT(fTemp11);
			double fTemp12 = (0.047619047619047623 * (((fTemp6 + (fSlow60 * fTemp2)) + (fSlow62 * fTemp8)) + (fSlow63 * fTemp5)));
			fRec30[0] = max((fRec30[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph3 = FAUSTFLOAT(fRec30[0]);
			output3[i] = FAUSTFLOAT(fTemp12);
			double fTemp13 = (0.047619047619047623 * (((fTemp6 + (fSlow65 * fTemp2)) + (fSlow67 * fTemp8)) + (fSlow68 * fTemp5)));
			fRec31[0] = max((fRec31[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fVbargraph4 = FAUSTFLOAT(fRec31[0]);
			output4[i] = FAUSTFLOAT(fTemp13);
			double fTemp14 = (0.047619047619047623 * ((fSlow69 * (fTemp3 + (fSlow71 * fTemp5))) + (fTemp6 + (fSlow72 * fTemp8))));
			fRec32[0] = max((fRec32[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fVbargraph5 = FAUSTFLOAT(fRec32[0]);
			output5[i] = FAUSTFLOAT(fTemp14);
			double fTemp15 = (0.038095238095238099 * ((fSlow75 * (fTemp3 + (fSlow77 * fTemp5))) + (fTemp6 + (fSlow78 * fTemp8))));
			fRec33[0] = max((fRec33[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph6 = FAUSTFLOAT(fRec33[0]);
			output6[i] = FAUSTFLOAT(fTemp15);
			double fTemp16 = (0.038095238095238099 * ((fSlow80 * (fTemp3 + (fSlow82 * fTemp5))) + (fTemp6 + (fSlow83 * fTemp8))));
			fRec34[0] = max((fRec34[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph7 = FAUSTFLOAT(fRec34[0]);
			output7[i] = FAUSTFLOAT(fTemp16);
			double fTemp17 = (0.038095238095238099 * ((fSlow85 * (fTemp3 + (fSlow87 * fTemp5))) + (fTemp6 + (fSlow88 * fTemp8))));
			fRec35[0] = max((fRec35[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph8 = FAUSTFLOAT(fRec35[0]);
			output8[i] = FAUSTFLOAT(fTemp17);
			double fTemp18 = (0.038095238095238099 * ((fSlow90 * (fTemp3 + (fSlow92 * fTemp5))) + (fTemp6 + (fSlow93 * fTemp8))));
			fRec36[0] = max((fRec36[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph9 = FAUSTFLOAT(fRec36[0]);
			output9[i] = FAUSTFLOAT(fTemp18);
			double fTemp19 = (0.038095238095238099 * (((fTemp6 + (fSlow95 * fTemp2)) + (fSlow97 * fTemp8)) + (fSlow98 * fTemp5)));
			fRec37[0] = max((fRec37[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph10 = FAUSTFLOAT(fRec37[0]);
			output10[i] = FAUSTFLOAT(fTemp19);
			double fTemp20 = (0.038095238095238099 * (((fTemp6 + (fSlow100 * fTemp2)) + (fSlow102 * fTemp8)) + (fSlow103 * fTemp5)));
			fRec38[0] = max((fRec38[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph11 = FAUSTFLOAT(fRec38[0]);
			output11[i] = FAUSTFLOAT(fTemp20);
			double fTemp21 = (0.038095238095238099 * (((fTemp6 + (fSlow105 * fTemp2)) + (fSlow107 * fTemp8)) + (fSlow108 * fTemp5)));
			fRec39[0] = max((fRec39[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph12 = FAUSTFLOAT(fRec39[0]);
			output12[i] = FAUSTFLOAT(fTemp21);
			double fTemp22 = (0.038095238095238099 * (((fTemp6 + (fSlow110 * fTemp2)) + (fSlow112 * fTemp8)) + (fSlow113 * fTemp5)));
			fRec40[0] = max((fRec40[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph13 = FAUSTFLOAT(fRec40[0]);
			output13[i] = FAUSTFLOAT(fTemp22);
			double fTemp23 = (0.038095238095238099 * ((fSlow114 * (fTemp3 + (fSlow116 * fTemp5))) + (fTemp6 + (fSlow117 * fTemp8))));
			fRec41[0] = max((fRec41[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fVbargraph14 = FAUSTFLOAT(fRec41[0]);
			output14[i] = FAUSTFLOAT(fTemp23);
			double fTemp24 = (0.038095238095238099 * ((fSlow118 * (fTemp3 + (fSlow120 * fTemp5))) + (fTemp6 + (fSlow121 * fTemp8))));
			fRec42[0] = max((fRec42[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph15 = FAUSTFLOAT(fRec42[0]);
			output15[i] = FAUSTFLOAT(fTemp24);
			double fTemp25 = (0.038095238095238099 * ((fSlow122 * (fTemp3 + (fSlow124 * fTemp5))) + (fTemp6 + (fSlow125 * fTemp8))));
			fRec43[0] = max((fRec43[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph16 = FAUSTFLOAT(fRec43[0]);
			output16[i] = FAUSTFLOAT(fTemp25);
			double fTemp26 = (0.038095238095238099 * ((fSlow126 * (fTemp3 + (fSlow128 * fTemp5))) + (fTemp6 + (fSlow129 * fTemp8))));
			fRec44[0] = max((fRec44[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fVbargraph17 = FAUSTFLOAT(fRec44[0]);
			output17[i] = FAUSTFLOAT(fTemp26);
			double fTemp27 = (0.03214285714285714 * ((fSlow132 * (fTemp3 + (fSlow134 * fTemp5))) + (fTemp6 + (fSlow135 * fTemp8))));
			fRec45[0] = max((fRec45[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph18 = FAUSTFLOAT(fRec45[0]);
			output18[i] = FAUSTFLOAT(fTemp27);
			double fTemp28 = (0.03214285714285714 * ((fSlow137 * (fTemp3 + (fSlow139 * fTemp5))) + (fTemp6 + (fSlow140 * fTemp8))));
			fRec46[0] = max((fRec46[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fVbargraph19 = FAUSTFLOAT(fRec46[0]);
			output19[i] = FAUSTFLOAT(fTemp28);
			double fTemp29 = (0.03214285714285714 * ((fSlow142 * (fTemp3 + (fSlow144 * fTemp5))) + (fTemp6 + (fSlow145 * fTemp8))));
			fRec47[0] = max((fRec47[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph20 = FAUSTFLOAT(fRec47[0]);
			output20[i] = FAUSTFLOAT(fTemp29);
			double fTemp30 = (0.03214285714285714 * ((fSlow147 * (fTemp3 + (fSlow149 * fTemp5))) + (fTemp6 + (fSlow150 * fTemp8))));
			fRec48[0] = max((fRec48[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph21 = FAUSTFLOAT(fRec48[0]);
			output21[i] = FAUSTFLOAT(fTemp30);
			double fTemp31 = (0.03214285714285714 * ((fSlow151 * (fTemp3 + (fSlow153 * fTemp5))) + (fTemp6 + (fSlow154 * fTemp8))));
			fRec49[0] = max((fRec49[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph22 = FAUSTFLOAT(fRec49[0]);
			output22[i] = FAUSTFLOAT(fTemp31);
			double fTemp32 = (0.03214285714285714 * ((fSlow155 * (fTemp3 + (fSlow157 * fTemp5))) + (fTemp6 + (fSlow158 * fTemp8))));
			fRec50[0] = max((fRec50[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph23 = FAUSTFLOAT(fRec50[0]);
			output23[i] = FAUSTFLOAT(fTemp32);
			double fTemp33 = (0.03214285714285714 * ((fSlow159 * (fTemp3 + (fSlow161 * fTemp5))) + (fTemp6 + (fSlow162 * fTemp8))));
			fRec51[0] = max((fRec51[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph24 = FAUSTFLOAT(fRec51[0]);
			output24[i] = FAUSTFLOAT(fTemp33);
			double fTemp34 = (0.03214285714285714 * ((fSlow163 * (fTemp3 + (fSlow165 * fTemp5))) + (fTemp6 + (fSlow166 * fTemp8))));
			fRec52[0] = max((fRec52[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fVbargraph25 = FAUSTFLOAT(fRec52[0]);
			output25[i] = FAUSTFLOAT(fTemp34);
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec2[1] = fRec2[0];
			fRec1[1] = fRec1[0];
			fRec6[1] = fRec6[0];
			fRec5[1] = fRec5[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			fRec9[1] = fRec9[0];
			fRec12[1] = fRec12[0];
			fVec0[1] = fVec0[0];
			fRec13[1] = fRec13[0];
			fRec14[1] = fRec14[0];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
			fRec17[1] = fRec17[0];
			fRec18[1] = fRec18[0];
			fRec16[1] = fRec16[0];
			fRec19[1] = fRec19[0];
			fRec15[1] = fRec15[0];
			fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			fRec21[1] = fRec21[0];
			fRec20[1] = fRec20[0];
			fVec1[1] = fVec1[0];
			fRec24[1] = fRec24[0];
			fRec26[1] = fRec26[0];
			fRec27[1] = fRec27[0];
			fRec25[1] = fRec25[0];
			fRec0[1] = fRec0[0];
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec30[1] = fRec30[0];
			fRec31[1] = fRec31[0];
			fRec32[1] = fRec32[0];
			fRec33[1] = fRec33[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec38[1] = fRec38[0];
			fRec39[1] = fRec39[0];
			fRec40[1] = fRec40[0];
			fRec41[1] = fRec41[0];
			fRec42[1] = fRec42[0];
			fRec43[1] = fRec43[0];
			fRec44[1] = fRec44[0];
			fRec45[1] = fRec45[0];
			fRec46[1] = fRec46[0];
			fRec47[1] = fRec47[0];
			fRec48[1] = fRec48[0];
			fRec49[1] = fRec49[0];
			fRec50[1] = fRec50[0];
			fRec51[1] = fRec51[0];
			fRec52[1] = fRec52[0];
			
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
    g_unitName = STRDUP(name.c_str());

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

static InterfaceTable* ft;

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
    FAUST_EXPORT int api_version(void);
#endif
    FAUST_EXPORT void load(InterfaceTable*);
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
    size_t numControls = unit->mNumControls;
    int curControl = unit->mDSP->getNumInputs();
    for (int i = 0; i < numControls; ++i) {
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
                for (int i = 0; i < unit->getNumAudioInputs(); ++i) {
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
    mydsp* tmp_dsp = new FAUSTCLASS;
    tmp_dsp->metadata(&meta);
    delete tmp_dsp;
 
    std::string name = meta["name"];

    if (name.empty()) {
        name = fileNameToUnitName(__FILE__);
    }
  
    name = normalizeClassName(name);

#if !defined(NDEBUG) & defined(SC_API_EXPORT)
    Print("Faust: supercollider.cpp: sc_api_version = %d\n", sc_api_version);
#endif

    if (name.empty()) {
        // Catch empty name
        Print("Faust [supercollider.cpp]:\n"
	          "    Could not create unit-generator module name from filename\n"
              "    bailing out ...\n");
        return;
    }

    if (strncmp(name.c_str(), SC_FAUST_PREFIX, strlen(SC_FAUST_PREFIX)) != 0) {
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

#endif
