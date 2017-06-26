/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2016"
license: "GPL"
name: "HOABeamDirac2HOA5"
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
double mydsp_faustpower3_f(double value) {
	return ((value * value) * value);
	
}
double mydsp_faustpower4_f(double value) {
	return (((value * value) * value) * value);
	
}
double mydsp_faustpower5_f(double value) {
	return ((((value * value) * value) * value) * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fCheckbox0;
	int fSamplingFreq;
	double fConst0;
	double fConst1;
	FAUSTFLOAT fEntry0;
	double fRec0[2];
	double fRec1[2];
	FAUSTFLOAT fVslider0;
	FAUSTFLOAT fVslider1;
	FAUSTFLOAT fVslider2;
	double fRec2[2];
	double fRec3[2];
	double fRec4[2];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
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
		m->declare("name", "HOABeamDirac2HOA5");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 36;
		
	}
	virtual int getNumOutputs() {
		return 36;
		
	}
	virtual int getInputRate(int channel) {
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
			case 26: {
				rate = 1;
				break;
			}
			case 27: {
				rate = 1;
				break;
			}
			case 28: {
				rate = 1;
				break;
			}
			case 29: {
				rate = 1;
				break;
			}
			case 30: {
				rate = 1;
				break;
			}
			case 31: {
				rate = 1;
				break;
			}
			case 32: {
				rate = 1;
				break;
			}
			case 33: {
				rate = 1;
				break;
			}
			case 34: {
				rate = 1;
				break;
			}
			case 35: {
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
			case 26: {
				rate = 1;
				break;
			}
			case 27: {
				rate = 1;
				break;
			}
			case 28: {
				rate = 1;
				break;
			}
			case 29: {
				rate = 1;
				break;
			}
			case 30: {
				rate = 1;
				break;
			}
			case 31: {
				rate = 1;
				break;
			}
			case 32: {
				rate = 1;
				break;
			}
			case 33: {
				rate = 1;
				break;
			}
			case 34: {
				rate = 1;
				break;
			}
			case 35: {
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
		fConst1 = (1.0 / fConst0);
		
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = FAUSTFLOAT(0.0);
		fEntry0 = FAUSTFLOAT(1.0);
		fVslider0 = FAUSTFLOAT(0.0);
		fVslider1 = FAUSTFLOAT(0.0);
		fVslider2 = FAUSTFLOAT(0.0);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec0[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec1[l1] = 0.0;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec2[l2] = 0.0;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec3[l3] = 0.0;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec4[l4] = 0.0;
			
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
		ui_interface->openHorizontalBox("Parameters");
		ui_interface->declare(0, "2", "");
		ui_interface->openVerticalBox("On/Off");
		ui_interface->addCheckButton("On", &fCheckbox0);
		ui_interface->closeBox();
		ui_interface->declare(&fEntry0, "3", "");
		ui_interface->declare(&fEntry0, "unit", "s");
		ui_interface->addNumEntry("Crossfade", &fEntry0, 1.0, 0.10000000000000001, 10.0, 0.10000000000000001);
		ui_interface->declare(&fVslider2, "4", "");
		ui_interface->declare(&fVslider2, "unit", "dB");
		ui_interface->addVerticalSlider("Gain", &fVslider2, 0.0, -20.0, 20.0, 0.10000000000000001);
		ui_interface->declare(&fVslider1, "5", "");
		ui_interface->addVerticalSlider("Azimuth", &fVslider1, 0.0, -3.1415926535897931, 3.1415926535897931, 0.10000000000000001);
		ui_interface->declare(&fVslider0, "6", "");
		ui_interface->addVerticalSlider("Elevation", &fVslider0, 0.0, -1.5707963267948966, 1.5707963267948966, 0.10000000000000001);
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* input2 = inputs[2];
		FAUSTFLOAT* input3 = inputs[3];
		FAUSTFLOAT* input4 = inputs[4];
		FAUSTFLOAT* input5 = inputs[5];
		FAUSTFLOAT* input6 = inputs[6];
		FAUSTFLOAT* input7 = inputs[7];
		FAUSTFLOAT* input8 = inputs[8];
		FAUSTFLOAT* input9 = inputs[9];
		FAUSTFLOAT* input10 = inputs[10];
		FAUSTFLOAT* input11 = inputs[11];
		FAUSTFLOAT* input12 = inputs[12];
		FAUSTFLOAT* input13 = inputs[13];
		FAUSTFLOAT* input14 = inputs[14];
		FAUSTFLOAT* input15 = inputs[15];
		FAUSTFLOAT* input16 = inputs[16];
		FAUSTFLOAT* input17 = inputs[17];
		FAUSTFLOAT* input18 = inputs[18];
		FAUSTFLOAT* input19 = inputs[19];
		FAUSTFLOAT* input20 = inputs[20];
		FAUSTFLOAT* input21 = inputs[21];
		FAUSTFLOAT* input22 = inputs[22];
		FAUSTFLOAT* input23 = inputs[23];
		FAUSTFLOAT* input24 = inputs[24];
		FAUSTFLOAT* input25 = inputs[25];
		FAUSTFLOAT* input26 = inputs[26];
		FAUSTFLOAT* input27 = inputs[27];
		FAUSTFLOAT* input28 = inputs[28];
		FAUSTFLOAT* input29 = inputs[29];
		FAUSTFLOAT* input30 = inputs[30];
		FAUSTFLOAT* input31 = inputs[31];
		FAUSTFLOAT* input32 = inputs[32];
		FAUSTFLOAT* input33 = inputs[33];
		FAUSTFLOAT* input34 = inputs[34];
		FAUSTFLOAT* input35 = inputs[35];
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
		FAUSTFLOAT* output26 = outputs[26];
		FAUSTFLOAT* output27 = outputs[27];
		FAUSTFLOAT* output28 = outputs[28];
		FAUSTFLOAT* output29 = outputs[29];
		FAUSTFLOAT* output30 = outputs[30];
		FAUSTFLOAT* output31 = outputs[31];
		FAUSTFLOAT* output32 = outputs[32];
		FAUSTFLOAT* output33 = outputs[33];
		FAUSTFLOAT* output34 = outputs[34];
		FAUSTFLOAT* output35 = outputs[35];
		double fSlow0 = double(fCheckbox0);
		int iSlow1 = int(fSlow0);
		double fSlow2 = double(fEntry0);
		double fSlow3 = (fConst1 / fSlow2);
		int iSlow4 = (fSlow0 > 0.0);
		double fSlow5 = (fConst0 * fSlow2);
		int iSlow6 = ((1.0 - fSlow0) > 0.0);
		double fSlow7 = double(fVslider0);
		double fSlow8 = cos(fSlow7);
		double fSlow9 = double(fVslider1);
		double fSlow10 = (3.0 * fSlow9);
		double fSlow11 = sin(fSlow10);
		double fSlow12 = (2.0916500663351889 * (fSlow11 * fSlow8));
		double fSlow13 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider2))));
		double fSlow14 = (2.0 * fSlow9);
		double fSlow15 = sin(fSlow14);
		double fSlow16 = sin(fSlow7);
		double fSlow17 = (5.123475382979799 * (fSlow15 * fSlow16));
		double fSlow18 = sin(fSlow9);
		double fSlow19 = mydsp_faustpower2_f(fSlow16);
		double fSlow20 = (5.0 * fSlow19);
		double fSlow21 = (fSlow20 + -1.0);
		double fSlow22 = (fSlow18 * fSlow21);
		double fSlow23 = (1.6201851746019651 * fSlow22);
		double fSlow24 = cos(fSlow9);
		double fSlow25 = (fSlow24 * fSlow21);
		double fSlow26 = (1.6201851746019651 * fSlow25);
		double fSlow27 = cos(fSlow10);
		double fSlow28 = mydsp_faustpower2_f(fSlow8);
		double fSlow29 = (2.0916500663351889 * (fSlow27 * fSlow28));
		double fSlow30 = (4.0 * fSlow9);
		double fSlow31 = sin(fSlow30);
		double fSlow32 = mydsp_faustpower3_f(fSlow8);
		double fSlow33 = (2.2185299186623562 * (fSlow31 * fSlow32));
		double fSlow34 = (7.0 * fSlow19);
		double fSlow35 = (fSlow34 + -1.0);
		double fSlow36 = (fSlow15 * fSlow35);
		double fSlow37 = (1.6770509831248424 * (fSlow36 * fSlow8));
		double fSlow38 = cos(fSlow14);
		double fSlow39 = (fSlow38 * fSlow35);
		double fSlow40 = (1.6770509831248424 * (fSlow39 * fSlow8));
		double fSlow41 = cos(fSlow30);
		double fSlow42 = (2.2185299186623562 * (fSlow41 * fSlow32));
		double fSlow43 = (5.0 * fSlow9);
		double fSlow44 = sin(fSlow43);
		double fSlow45 = mydsp_faustpower4_f(fSlow8);
		double fSlow46 = (2.3268138086232857 * (fSlow44 * fSlow45));
		double fSlow47 = ((9.0 * fSlow19) + -1.0);
		double fSlow48 = (fSlow11 * fSlow47);
		double fSlow49 = (1.734304615688951 * (fSlow48 * fSlow28));
		double fSlow50 = ((((21.0 * fSlow19) + -14.0) * fSlow19) + 1.0);
		double fSlow51 = (fSlow18 * fSlow50);
		double fSlow52 = (1.6056540723331412 * fSlow51);
		double fSlow53 = (fSlow24 * fSlow50);
		double fSlow54 = (1.6056540723331412 * fSlow53);
		double fSlow55 = (fSlow27 * fSlow47);
		double fSlow56 = (1.734304615688951 * (fSlow55 * fSlow28));
		double fSlow57 = cos(fSlow43);
		double fSlow58 = (2.3268138086232857 * (fSlow57 * fSlow45));
		double fSlow59 = (fSlow20 + -3.0);
		double fSlow60 = (1.3228756555322954 * fSlow59);
		double fSlow61 = (fSlow38 * fSlow28);
		double fSlow62 = (5.123475382979799 * fSlow61);
		double fSlow63 = (fSlow11 * fSlow32);
		double fSlow64 = (6.2749501990055663 * fSlow63);
		double fSlow65 = (fSlow27 * fSlow32);
		double fSlow66 = (6.2749501990055663 * fSlow65);
		double fSlow67 = (fSlow31 * fSlow45);
		double fSlow68 = (7.3580313263807184 * fSlow67);
		double fSlow69 = ((((63.0 * fSlow19) + -70.0) * fSlow19) + 15.0);
		double fSlow70 = (0.41457809879442498 * fSlow69);
		double fSlow71 = (fSlow41 * fSlow45);
		double fSlow72 = (7.3580313263807184 * fSlow71);
		double fSlow73 = (fSlow34 + -3.0);
		double fSlow74 = sin((2.0 * fSlow7));
		double fSlow75 = (1.1858541225631423 * (fSlow73 * fSlow74));
		double fSlow76 = ((((35.0 * fSlow19) + -30.0) * fSlow19) + 3.0);
		double fSlow77 = (0.375 * fSlow76);
		double fSlow78 = (fSlow18 * fSlow8);
		double fSlow79 = (fSlow24 * fSlow8);
		double fSlow80 = (fSlow15 * fSlow28);
		double fSlow81 = (fSlow18 * fSlow74);
		double fSlow82 = (fSlow24 * fSlow74);
		double fSlow83 = ((3.0 * fSlow19) + -1.0);
		double fSlow84 = (fSlow80 * fSlow16);
		double fSlow85 = (8.4963227339832148 * fSlow84);
		double fSlow86 = (fSlow61 * fSlow16);
		double fSlow87 = (8.4963227339832148 * fSlow86);
		double fSlow88 = (0.13783222385544802 * fSlow78);
		double fSlow89 = (0.13783222385544802 * fSlow16);
		double fSlow90 = (0.13783222385544802 * fSlow79);
		double fSlow91 = (0.15410111101537496 * fSlow80);
		double fSlow92 = (0.15410111101537496 * fSlow81);
		double fSlow93 = (0.088970317927147144 * fSlow83);
		double fSlow94 = (0.15410111101537496 * fSlow82);
		double fSlow95 = (0.15410111101537496 * fSlow61);
		double fSlow96 = (0.16644822363786807 * fSlow63);
		double fSlow97 = (0.40771321650543829 * fSlow84);
		double fSlow98 = (0.12893023963105413 * (fSlow22 * fSlow8));
		double fSlow99 = (0.10527109983694811 * (fSlow59 * fSlow16));
		double fSlow100 = (0.12893023963105413 * (fSlow25 * fSlow8));
		double fSlow101 = (0.40771321650543829 * fSlow86);
		double fSlow102 = (0.16644822363786807 * fSlow65);
		double fSlow103 = (0.17654500147618726 * fSlow67);
		double fSlow104 = (0.49934467091360413 * (fSlow63 * fSlow16));
		double fSlow105 = (0.13345547689072071 * (fSlow36 * fSlow28));
		double fSlow106 = (0.094367272695913204 * ((fSlow18 * fSlow73) * fSlow74));
		double fSlow107 = (0.029841551829730376 * fSlow76);
		double fSlow108 = (0.094367272695913204 * ((fSlow24 * fSlow73) * fSlow74));
		double fSlow109 = (0.13345547689072071 * (fSlow39 * fSlow28));
		double fSlow110 = (0.49934467091360413 * (fSlow65 * fSlow16));
		double fSlow111 = (0.17654500147618726 * fSlow71);
		double fSlow112 = mydsp_faustpower5_f(fSlow8);
		double fSlow113 = (0.18516195964843765 * (fSlow44 * fSlow112));
		double fSlow114 = (0.58553352850925322 * (fSlow67 * fSlow16));
		double fSlow115 = (0.13801157620699323 * (fSlow48 * fSlow32));
		double fSlow116 = (0.67611588060873762 * (((fSlow15 * fSlow83) * fSlow28) * fSlow16));
		double fSlow117 = (0.12777389125372554 * (fSlow51 * fSlow8));
		double fSlow118 = (0.032991076860386437 * (fSlow69 * fSlow16));
		double fSlow119 = (0.12777389125372554 * (fSlow53 * fSlow8));
		double fSlow120 = (0.67611588060873762 * (((fSlow38 * fSlow83) * fSlow28) * fSlow16));
		double fSlow121 = (0.13801157620699323 * (fSlow55 * fSlow32));
		double fSlow122 = (0.58553352850925322 * (fSlow71 * fSlow16));
		double fSlow123 = (0.18516195964843765 * (fSlow57 * fSlow112));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec0[0] = (iSlow4?0.0:min(fSlow5, (fRec0[1] + 1.0)));
			double fTemp0 = double(input0[i]);
			fRec1[0] = (iSlow6?fSlow5:max(0.0, (fRec1[1] + -1.0)));
			fRec2[0] = (fSlow13 + (0.999 * fRec2[1]));
			fRec3[0] = (iSlow4?fSlow5:max(0.0, (fRec3[1] + -1.0)));
			double fTemp1 = (fRec2[0] * fRec3[0]);
			double fTemp2 = double(input9[i]);
			fRec4[0] = (iSlow6?0.0:min(fSlow5, (fRec4[1] + 1.0)));
			double fTemp3 = (fRec2[0] * fRec4[0]);
			double fTemp4 = double(input10[i]);
			double fTemp5 = double(input11[i]);
			double fTemp6 = double(input13[i]);
			double fTemp7 = double(input15[i]);
			double fTemp8 = double(input16[i]);
			double fTemp9 = double(input18[i]);
			double fTemp10 = double(input22[i]);
			double fTemp11 = double(input24[i]);
			double fTemp12 = double(input25[i]);
			double fTemp13 = double(input27[i]);
			double fTemp14 = double(input29[i]);
			double fTemp15 = double(input31[i]);
			double fTemp16 = double(input33[i]);
			double fTemp17 = double(input35[i]);
			double fTemp18 = double(input12[i]);
			double fTemp19 = double(input14[i]);
			double fTemp20 = double(input17[i]);
			double fTemp21 = double(input23[i]);
			double fTemp22 = double(input26[i]);
			double fTemp23 = double(input30[i]);
			double fTemp24 = double(input34[i]);
			double fTemp25 = double(input19[i]);
			double fTemp26 = double(input21[i]);
			double fTemp27 = double(input20[i]);
			double fTemp28 = double(input1[i]);
			double fTemp29 = double(input2[i]);
			double fTemp30 = double(input3[i]);
			double fTemp31 = double(input4[i]);
			double fTemp32 = double(input5[i]);
			double fTemp33 = double(input7[i]);
			double fTemp34 = double(input8[i]);
			double fTemp35 = double(input6[i]);
			double fTemp36 = double(input28[i]);
			double fTemp37 = double(input32[i]);
			double fTemp38 = ((fSlow8 * ((((((((((((((fSlow8 * ((fSlow12 * (iSlow1?(fSlow3 * (fTemp3 * fTemp2)):(fSlow3 * (fTemp1 * fTemp2)))) + (fSlow17 * (iSlow1?(fSlow3 * (fTemp3 * fTemp4)):(fSlow3 * (fTemp1 * fTemp4)))))) + (fSlow23 * (iSlow1?(fSlow3 * (fTemp3 * fTemp5)):(fSlow3 * (fTemp1 * fTemp5))))) + (fSlow26 * (iSlow1?(fSlow3 * (fTemp3 * fTemp6)):(fSlow3 * (fTemp1 * fTemp6))))) + (fSlow29 * (iSlow1?(fSlow3 * (fTemp3 * fTemp7)):(fSlow3 * (fTemp1 * fTemp7))))) + (fSlow33 * (iSlow1?(fSlow3 * (fTemp3 * fTemp8)):(fSlow3 * (fTemp1 * fTemp8))))) + (fSlow37 * (iSlow1?(fSlow3 * (fTemp3 * fTemp9)):(fSlow3 * (fTemp1 * fTemp9))))) + (fSlow40 * (iSlow1?(fSlow3 * (fTemp3 * fTemp10)):(fSlow3 * (fTemp1 * fTemp10))))) + (fSlow42 * (iSlow1?(fSlow3 * (fTemp3 * fTemp11)):(fSlow3 * (fTemp1 * fTemp11))))) + (fSlow46 * (iSlow1?(fSlow3 * (fTemp3 * fTemp12)):(fSlow3 * (fTemp1 * fTemp12))))) + (fSlow49 * (iSlow1?(fSlow3 * (fTemp3 * fTemp13)):(fSlow3 * (fTemp1 * fTemp13))))) + (fSlow52 * (iSlow1?(fSlow3 * (fTemp3 * fTemp14)):(fSlow3 * (fTemp1 * fTemp14))))) + (fSlow54 * (iSlow1?(fSlow3 * (fTemp3 * fTemp15)):(fSlow3 * (fTemp1 * fTemp15))))) + (fSlow56 * (iSlow1?(fSlow3 * (fTemp3 * fTemp16)):(fSlow3 * (fTemp1 * fTemp16))))) + (fSlow58 * (iSlow1?(fSlow3 * (fTemp3 * fTemp17)):(fSlow3 * (fTemp1 * fTemp17)))))) + ((fSlow16 * (((((((fSlow60 * (iSlow1?(fSlow3 * (fTemp3 * fTemp18)):(fSlow3 * (fTemp1 * fTemp18)))) + (fSlow62 * (iSlow1?(fSlow3 * (fTemp3 * fTemp19)):(fSlow3 * (fTemp1 * fTemp19))))) + (fSlow64 * (iSlow1?(fSlow3 * (fTemp3 * fTemp20)):(fSlow3 * (fTemp1 * fTemp20))))) + (fSlow66 * (iSlow1?(fSlow3 * (fTemp3 * fTemp21)):(fSlow3 * (fTemp1 * fTemp21))))) + (fSlow68 * (iSlow1?(fSlow3 * (fTemp3 * fTemp22)):(fSlow3 * (fTemp1 * fTemp22))))) + (fSlow70 * (iSlow1?(fSlow3 * (fTemp3 * fTemp23)):(fSlow3 * (fTemp1 * fTemp23))))) + (fSlow72 * (iSlow1?(fSlow3 * (fTemp3 * fTemp24)):(fSlow3 * (fTemp1 * fTemp24)))))) + (((fSlow75 * ((fSlow18 * (iSlow1?(fSlow3 * (fTemp3 * fTemp25)):(fSlow3 * (fTemp1 * fTemp25)))) + (fSlow24 * (iSlow1?(fSlow3 * (fTemp3 * fTemp26)):(fSlow3 * (fTemp1 * fTemp26)))))) + ((fSlow77 * (iSlow1?(fSlow3 * (fTemp3 * fTemp27)):(fSlow3 * (fTemp1 * fTemp27)))) + (((iSlow1?(fSlow3 * (fTemp3 * fTemp0)):(fSlow3 * (fTemp1 * fTemp0))) + (1.7320508075688772 * (((fSlow78 * (iSlow1?(fSlow3 * (fTemp3 * fTemp28)):(fSlow3 * (fTemp1 * fTemp28)))) + (fSlow16 * (iSlow1?(fSlow3 * (fTemp3 * fTemp29)):(fSlow3 * (fTemp1 * fTemp29))))) + (fSlow79 * (iSlow1?(fSlow3 * (fTemp3 * fTemp30)):(fSlow3 * (fTemp1 * fTemp30))))))) + (1.9364916731037085 * ((((fSlow80 * (iSlow1?(fSlow3 * (fTemp3 * fTemp31)):(fSlow3 * (fTemp1 * fTemp31)))) + (fSlow81 * (iSlow1?(fSlow3 * (fTemp3 * fTemp32)):(fSlow3 * (fTemp1 * fTemp32))))) + (fSlow82 * (iSlow1?(fSlow3 * (fTemp3 * fTemp33)):(fSlow3 * (fTemp1 * fTemp33))))) + (fSlow61 * (iSlow1?(fSlow3 * (fTemp3 * fTemp34)):(fSlow3 * (fTemp1 * fTemp34))))))))) + (fSlow83 * (((1.1180339887498949 * (iSlow1?(fSlow3 * (fTemp3 * fTemp35)):(fSlow3 * (fTemp1 * fTemp35)))) + (fSlow85 * (iSlow1?(fSlow3 * (fTemp3 * fTemp36)):(fSlow3 * (fTemp1 * fTemp36))))) + (fSlow87 * (iSlow1?(fSlow3 * (fTemp3 * fTemp37)):(fSlow3 * (fTemp1 * fTemp37)))))))));
			output0[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp0)):(fSlow3 * (fRec0[0] * fTemp0))) + (0.079577471545947673 * fTemp38)));
			output1[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp28)):(fSlow3 * (fRec0[0] * fTemp28))) + (fSlow88 * fTemp38)));
			output2[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp29)):(fSlow3 * (fRec0[0] * fTemp29))) + (fSlow89 * fTemp38)));
			output3[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp30)):(fSlow3 * (fRec0[0] * fTemp30))) + (fSlow90 * fTemp38)));
			output4[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp31)):(fSlow3 * (fRec0[0] * fTemp31))) + (fSlow91 * fTemp38)));
			output5[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp32)):(fSlow3 * (fRec0[0] * fTemp32))) + (fSlow92 * fTemp38)));
			output6[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp35)):(fSlow3 * (fRec0[0] * fTemp35))) + (fSlow93 * fTemp38)));
			output7[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp33)):(fSlow3 * (fRec0[0] * fTemp33))) + (fSlow94 * fTemp38)));
			output8[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp34)):(fSlow3 * (fRec0[0] * fTemp34))) + (fSlow95 * fTemp38)));
			output9[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp2)):(fSlow3 * (fRec0[0] * fTemp2))) + (fSlow96 * fTemp38)));
			output10[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp4)):(fSlow3 * (fRec0[0] * fTemp4))) + (fSlow97 * fTemp38)));
			output11[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp5)):(fSlow3 * (fRec0[0] * fTemp5))) + (fSlow98 * fTemp38)));
			output12[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp18)):(fSlow3 * (fRec0[0] * fTemp18))) + (fSlow99 * fTemp38)));
			output13[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp6)):(fSlow3 * (fRec0[0] * fTemp6))) + (fSlow100 * fTemp38)));
			output14[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp19)):(fSlow3 * (fRec0[0] * fTemp19))) + (fSlow101 * fTemp38)));
			output15[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp7)):(fSlow3 * (fRec0[0] * fTemp7))) + (fSlow102 * fTemp38)));
			output16[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp8)):(fSlow3 * (fRec0[0] * fTemp8))) + (fSlow103 * fTemp38)));
			output17[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp20)):(fSlow3 * (fRec0[0] * fTemp20))) + (fSlow104 * fTemp38)));
			output18[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp9)):(fSlow3 * (fRec0[0] * fTemp9))) + (fSlow105 * fTemp38)));
			output19[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp25)):(fSlow3 * (fRec0[0] * fTemp25))) + (fSlow106 * fTemp38)));
			output20[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp27)):(fSlow3 * (fRec0[0] * fTemp27))) + (fSlow107 * fTemp38)));
			output21[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp26)):(fSlow3 * (fRec0[0] * fTemp26))) + (fSlow108 * fTemp38)));
			output22[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp10)):(fSlow3 * (fRec0[0] * fTemp10))) + (fSlow109 * fTemp38)));
			output23[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp21)):(fSlow3 * (fRec0[0] * fTemp21))) + (fSlow110 * fTemp38)));
			output24[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp11)):(fSlow3 * (fRec0[0] * fTemp11))) + (fSlow111 * fTemp38)));
			output25[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp12)):(fSlow3 * (fRec0[0] * fTemp12))) + (fSlow113 * fTemp38)));
			output26[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp22)):(fSlow3 * (fRec0[0] * fTemp22))) + (fSlow114 * fTemp38)));
			output27[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp13)):(fSlow3 * (fRec0[0] * fTemp13))) + (fSlow115 * fTemp38)));
			output28[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp36)):(fSlow3 * (fRec0[0] * fTemp36))) + (fSlow116 * fTemp38)));
			output29[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp14)):(fSlow3 * (fRec0[0] * fTemp14))) + (fSlow117 * fTemp38)));
			output30[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp23)):(fSlow3 * (fRec0[0] * fTemp23))) + (fSlow118 * fTemp38)));
			output31[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp15)):(fSlow3 * (fRec0[0] * fTemp15))) + (fSlow119 * fTemp38)));
			output32[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp37)):(fSlow3 * (fRec0[0] * fTemp37))) + (fSlow120 * fTemp38)));
			output33[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp16)):(fSlow3 * (fRec0[0] * fTemp16))) + (fSlow121 * fTemp38)));
			output34[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp24)):(fSlow3 * (fRec0[0] * fTemp24))) + (fSlow122 * fTemp38)));
			output35[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp17)):(fSlow3 * (fRec0[0] * fTemp17))) + (fSlow123 * fTemp38)));
			fRec0[1] = fRec0[0];
			fRec1[1] = fRec1[0];
			fRec2[1] = fRec2[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			
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
