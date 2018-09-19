/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2015"
license: "GPL"
name: "HOAMirror10"
version: "1.0"
Code generated with Faust 2.5.21 (https://faust.grame.fr)
Compilation options: cpp, -double -ftz 0
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

// The prefix is set to "Faust" in the faust2supercollider script, otherwise set empty
#if !defined(SC_FAUST_PREFIX)
#define SC_FAUST_PREFIX ""
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
    
        /**
         * Global init, calls the following methods:
         * - static class 'classInit': static tables initialization
         * - 'instanceInit': constants and instance state initialization
         *
         * @param samplingRate - the sampling rate in Hertz
         */
        virtual void init(int samplingRate) = 0;

        /**
         * Init instance state
         *
         * @param samplingRate - the sampling rate in Hertz
         */
        virtual void instanceInit(int samplingRate) = 0;

        /**
         * Init instance constant state
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
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value) metadata.
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with successive in/out audio buffers.
         *
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the number of frames to compute
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
        virtual void metadata(Meta* m) { fDSP->metadata(m); }
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

#ifndef __UI_H__
#define __UI_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

struct Soundfile;

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
    
        // -- soundfiles
    
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) = 0;

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
    {}

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
    
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}

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
    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}

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

#include <cmath>


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif
#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fCheckbox0;
	FAUSTFLOAT fCheckbox1;
	FAUSTFLOAT fCheckbox2;
	int fSamplingFreq;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2015");
		m->declare("license", "GPL");
		m->declare("name", "HOAMirror10");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 121;
		
	}
	virtual int getNumOutputs() {
		return 121;
		
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
			case 36: {
				rate = 1;
				break;
			}
			case 37: {
				rate = 1;
				break;
			}
			case 38: {
				rate = 1;
				break;
			}
			case 39: {
				rate = 1;
				break;
			}
			case 40: {
				rate = 1;
				break;
			}
			case 41: {
				rate = 1;
				break;
			}
			case 42: {
				rate = 1;
				break;
			}
			case 43: {
				rate = 1;
				break;
			}
			case 44: {
				rate = 1;
				break;
			}
			case 45: {
				rate = 1;
				break;
			}
			case 46: {
				rate = 1;
				break;
			}
			case 47: {
				rate = 1;
				break;
			}
			case 48: {
				rate = 1;
				break;
			}
			case 49: {
				rate = 1;
				break;
			}
			case 50: {
				rate = 1;
				break;
			}
			case 51: {
				rate = 1;
				break;
			}
			case 52: {
				rate = 1;
				break;
			}
			case 53: {
				rate = 1;
				break;
			}
			case 54: {
				rate = 1;
				break;
			}
			case 55: {
				rate = 1;
				break;
			}
			case 56: {
				rate = 1;
				break;
			}
			case 57: {
				rate = 1;
				break;
			}
			case 58: {
				rate = 1;
				break;
			}
			case 59: {
				rate = 1;
				break;
			}
			case 60: {
				rate = 1;
				break;
			}
			case 61: {
				rate = 1;
				break;
			}
			case 62: {
				rate = 1;
				break;
			}
			case 63: {
				rate = 1;
				break;
			}
			case 64: {
				rate = 1;
				break;
			}
			case 65: {
				rate = 1;
				break;
			}
			case 66: {
				rate = 1;
				break;
			}
			case 67: {
				rate = 1;
				break;
			}
			case 68: {
				rate = 1;
				break;
			}
			case 69: {
				rate = 1;
				break;
			}
			case 70: {
				rate = 1;
				break;
			}
			case 71: {
				rate = 1;
				break;
			}
			case 72: {
				rate = 1;
				break;
			}
			case 73: {
				rate = 1;
				break;
			}
			case 74: {
				rate = 1;
				break;
			}
			case 75: {
				rate = 1;
				break;
			}
			case 76: {
				rate = 1;
				break;
			}
			case 77: {
				rate = 1;
				break;
			}
			case 78: {
				rate = 1;
				break;
			}
			case 79: {
				rate = 1;
				break;
			}
			case 80: {
				rate = 1;
				break;
			}
			case 81: {
				rate = 1;
				break;
			}
			case 82: {
				rate = 1;
				break;
			}
			case 83: {
				rate = 1;
				break;
			}
			case 84: {
				rate = 1;
				break;
			}
			case 85: {
				rate = 1;
				break;
			}
			case 86: {
				rate = 1;
				break;
			}
			case 87: {
				rate = 1;
				break;
			}
			case 88: {
				rate = 1;
				break;
			}
			case 89: {
				rate = 1;
				break;
			}
			case 90: {
				rate = 1;
				break;
			}
			case 91: {
				rate = 1;
				break;
			}
			case 92: {
				rate = 1;
				break;
			}
			case 93: {
				rate = 1;
				break;
			}
			case 94: {
				rate = 1;
				break;
			}
			case 95: {
				rate = 1;
				break;
			}
			case 96: {
				rate = 1;
				break;
			}
			case 97: {
				rate = 1;
				break;
			}
			case 98: {
				rate = 1;
				break;
			}
			case 99: {
				rate = 1;
				break;
			}
			case 100: {
				rate = 1;
				break;
			}
			case 101: {
				rate = 1;
				break;
			}
			case 102: {
				rate = 1;
				break;
			}
			case 103: {
				rate = 1;
				break;
			}
			case 104: {
				rate = 1;
				break;
			}
			case 105: {
				rate = 1;
				break;
			}
			case 106: {
				rate = 1;
				break;
			}
			case 107: {
				rate = 1;
				break;
			}
			case 108: {
				rate = 1;
				break;
			}
			case 109: {
				rate = 1;
				break;
			}
			case 110: {
				rate = 1;
				break;
			}
			case 111: {
				rate = 1;
				break;
			}
			case 112: {
				rate = 1;
				break;
			}
			case 113: {
				rate = 1;
				break;
			}
			case 114: {
				rate = 1;
				break;
			}
			case 115: {
				rate = 1;
				break;
			}
			case 116: {
				rate = 1;
				break;
			}
			case 117: {
				rate = 1;
				break;
			}
			case 118: {
				rate = 1;
				break;
			}
			case 119: {
				rate = 1;
				break;
			}
			case 120: {
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
			case 36: {
				rate = 1;
				break;
			}
			case 37: {
				rate = 1;
				break;
			}
			case 38: {
				rate = 1;
				break;
			}
			case 39: {
				rate = 1;
				break;
			}
			case 40: {
				rate = 1;
				break;
			}
			case 41: {
				rate = 1;
				break;
			}
			case 42: {
				rate = 1;
				break;
			}
			case 43: {
				rate = 1;
				break;
			}
			case 44: {
				rate = 1;
				break;
			}
			case 45: {
				rate = 1;
				break;
			}
			case 46: {
				rate = 1;
				break;
			}
			case 47: {
				rate = 1;
				break;
			}
			case 48: {
				rate = 1;
				break;
			}
			case 49: {
				rate = 1;
				break;
			}
			case 50: {
				rate = 1;
				break;
			}
			case 51: {
				rate = 1;
				break;
			}
			case 52: {
				rate = 1;
				break;
			}
			case 53: {
				rate = 1;
				break;
			}
			case 54: {
				rate = 1;
				break;
			}
			case 55: {
				rate = 1;
				break;
			}
			case 56: {
				rate = 1;
				break;
			}
			case 57: {
				rate = 1;
				break;
			}
			case 58: {
				rate = 1;
				break;
			}
			case 59: {
				rate = 1;
				break;
			}
			case 60: {
				rate = 1;
				break;
			}
			case 61: {
				rate = 1;
				break;
			}
			case 62: {
				rate = 1;
				break;
			}
			case 63: {
				rate = 1;
				break;
			}
			case 64: {
				rate = 1;
				break;
			}
			case 65: {
				rate = 1;
				break;
			}
			case 66: {
				rate = 1;
				break;
			}
			case 67: {
				rate = 1;
				break;
			}
			case 68: {
				rate = 1;
				break;
			}
			case 69: {
				rate = 1;
				break;
			}
			case 70: {
				rate = 1;
				break;
			}
			case 71: {
				rate = 1;
				break;
			}
			case 72: {
				rate = 1;
				break;
			}
			case 73: {
				rate = 1;
				break;
			}
			case 74: {
				rate = 1;
				break;
			}
			case 75: {
				rate = 1;
				break;
			}
			case 76: {
				rate = 1;
				break;
			}
			case 77: {
				rate = 1;
				break;
			}
			case 78: {
				rate = 1;
				break;
			}
			case 79: {
				rate = 1;
				break;
			}
			case 80: {
				rate = 1;
				break;
			}
			case 81: {
				rate = 1;
				break;
			}
			case 82: {
				rate = 1;
				break;
			}
			case 83: {
				rate = 1;
				break;
			}
			case 84: {
				rate = 1;
				break;
			}
			case 85: {
				rate = 1;
				break;
			}
			case 86: {
				rate = 1;
				break;
			}
			case 87: {
				rate = 1;
				break;
			}
			case 88: {
				rate = 1;
				break;
			}
			case 89: {
				rate = 1;
				break;
			}
			case 90: {
				rate = 1;
				break;
			}
			case 91: {
				rate = 1;
				break;
			}
			case 92: {
				rate = 1;
				break;
			}
			case 93: {
				rate = 1;
				break;
			}
			case 94: {
				rate = 1;
				break;
			}
			case 95: {
				rate = 1;
				break;
			}
			case 96: {
				rate = 1;
				break;
			}
			case 97: {
				rate = 1;
				break;
			}
			case 98: {
				rate = 1;
				break;
			}
			case 99: {
				rate = 1;
				break;
			}
			case 100: {
				rate = 1;
				break;
			}
			case 101: {
				rate = 1;
				break;
			}
			case 102: {
				rate = 1;
				break;
			}
			case 103: {
				rate = 1;
				break;
			}
			case 104: {
				rate = 1;
				break;
			}
			case 105: {
				rate = 1;
				break;
			}
			case 106: {
				rate = 1;
				break;
			}
			case 107: {
				rate = 1;
				break;
			}
			case 108: {
				rate = 1;
				break;
			}
			case 109: {
				rate = 1;
				break;
			}
			case 110: {
				rate = 1;
				break;
			}
			case 111: {
				rate = 1;
				break;
			}
			case 112: {
				rate = 1;
				break;
			}
			case 113: {
				rate = 1;
				break;
			}
			case 114: {
				rate = 1;
				break;
			}
			case 115: {
				rate = 1;
				break;
			}
			case 116: {
				rate = 1;
				break;
			}
			case 117: {
				rate = 1;
				break;
			}
			case 118: {
				rate = 1;
				break;
			}
			case 119: {
				rate = 1;
				break;
			}
			case 120: {
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
		
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = FAUSTFLOAT(0.0);
		fCheckbox1 = FAUSTFLOAT(0.0);
		fCheckbox2 = FAUSTFLOAT(0.0);
		
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
		ui_interface->openVerticalBox("HOA scene mirroring");
		ui_interface->addCheckButton("front-back", &fCheckbox0);
		ui_interface->addCheckButton("left-right", &fCheckbox1);
		ui_interface->addCheckButton("up-down", &fCheckbox2);
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
		FAUSTFLOAT* input36 = inputs[36];
		FAUSTFLOAT* input37 = inputs[37];
		FAUSTFLOAT* input38 = inputs[38];
		FAUSTFLOAT* input39 = inputs[39];
		FAUSTFLOAT* input40 = inputs[40];
		FAUSTFLOAT* input41 = inputs[41];
		FAUSTFLOAT* input42 = inputs[42];
		FAUSTFLOAT* input43 = inputs[43];
		FAUSTFLOAT* input44 = inputs[44];
		FAUSTFLOAT* input45 = inputs[45];
		FAUSTFLOAT* input46 = inputs[46];
		FAUSTFLOAT* input47 = inputs[47];
		FAUSTFLOAT* input48 = inputs[48];
		FAUSTFLOAT* input49 = inputs[49];
		FAUSTFLOAT* input50 = inputs[50];
		FAUSTFLOAT* input51 = inputs[51];
		FAUSTFLOAT* input52 = inputs[52];
		FAUSTFLOAT* input53 = inputs[53];
		FAUSTFLOAT* input54 = inputs[54];
		FAUSTFLOAT* input55 = inputs[55];
		FAUSTFLOAT* input56 = inputs[56];
		FAUSTFLOAT* input57 = inputs[57];
		FAUSTFLOAT* input58 = inputs[58];
		FAUSTFLOAT* input59 = inputs[59];
		FAUSTFLOAT* input60 = inputs[60];
		FAUSTFLOAT* input61 = inputs[61];
		FAUSTFLOAT* input62 = inputs[62];
		FAUSTFLOAT* input63 = inputs[63];
		FAUSTFLOAT* input64 = inputs[64];
		FAUSTFLOAT* input65 = inputs[65];
		FAUSTFLOAT* input66 = inputs[66];
		FAUSTFLOAT* input67 = inputs[67];
		FAUSTFLOAT* input68 = inputs[68];
		FAUSTFLOAT* input69 = inputs[69];
		FAUSTFLOAT* input70 = inputs[70];
		FAUSTFLOAT* input71 = inputs[71];
		FAUSTFLOAT* input72 = inputs[72];
		FAUSTFLOAT* input73 = inputs[73];
		FAUSTFLOAT* input74 = inputs[74];
		FAUSTFLOAT* input75 = inputs[75];
		FAUSTFLOAT* input76 = inputs[76];
		FAUSTFLOAT* input77 = inputs[77];
		FAUSTFLOAT* input78 = inputs[78];
		FAUSTFLOAT* input79 = inputs[79];
		FAUSTFLOAT* input80 = inputs[80];
		FAUSTFLOAT* input81 = inputs[81];
		FAUSTFLOAT* input82 = inputs[82];
		FAUSTFLOAT* input83 = inputs[83];
		FAUSTFLOAT* input84 = inputs[84];
		FAUSTFLOAT* input85 = inputs[85];
		FAUSTFLOAT* input86 = inputs[86];
		FAUSTFLOAT* input87 = inputs[87];
		FAUSTFLOAT* input88 = inputs[88];
		FAUSTFLOAT* input89 = inputs[89];
		FAUSTFLOAT* input90 = inputs[90];
		FAUSTFLOAT* input91 = inputs[91];
		FAUSTFLOAT* input92 = inputs[92];
		FAUSTFLOAT* input93 = inputs[93];
		FAUSTFLOAT* input94 = inputs[94];
		FAUSTFLOAT* input95 = inputs[95];
		FAUSTFLOAT* input96 = inputs[96];
		FAUSTFLOAT* input97 = inputs[97];
		FAUSTFLOAT* input98 = inputs[98];
		FAUSTFLOAT* input99 = inputs[99];
		FAUSTFLOAT* input100 = inputs[100];
		FAUSTFLOAT* input101 = inputs[101];
		FAUSTFLOAT* input102 = inputs[102];
		FAUSTFLOAT* input103 = inputs[103];
		FAUSTFLOAT* input104 = inputs[104];
		FAUSTFLOAT* input105 = inputs[105];
		FAUSTFLOAT* input106 = inputs[106];
		FAUSTFLOAT* input107 = inputs[107];
		FAUSTFLOAT* input108 = inputs[108];
		FAUSTFLOAT* input109 = inputs[109];
		FAUSTFLOAT* input110 = inputs[110];
		FAUSTFLOAT* input111 = inputs[111];
		FAUSTFLOAT* input112 = inputs[112];
		FAUSTFLOAT* input113 = inputs[113];
		FAUSTFLOAT* input114 = inputs[114];
		FAUSTFLOAT* input115 = inputs[115];
		FAUSTFLOAT* input116 = inputs[116];
		FAUSTFLOAT* input117 = inputs[117];
		FAUSTFLOAT* input118 = inputs[118];
		FAUSTFLOAT* input119 = inputs[119];
		FAUSTFLOAT* input120 = inputs[120];
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
		FAUSTFLOAT* output36 = outputs[36];
		FAUSTFLOAT* output37 = outputs[37];
		FAUSTFLOAT* output38 = outputs[38];
		FAUSTFLOAT* output39 = outputs[39];
		FAUSTFLOAT* output40 = outputs[40];
		FAUSTFLOAT* output41 = outputs[41];
		FAUSTFLOAT* output42 = outputs[42];
		FAUSTFLOAT* output43 = outputs[43];
		FAUSTFLOAT* output44 = outputs[44];
		FAUSTFLOAT* output45 = outputs[45];
		FAUSTFLOAT* output46 = outputs[46];
		FAUSTFLOAT* output47 = outputs[47];
		FAUSTFLOAT* output48 = outputs[48];
		FAUSTFLOAT* output49 = outputs[49];
		FAUSTFLOAT* output50 = outputs[50];
		FAUSTFLOAT* output51 = outputs[51];
		FAUSTFLOAT* output52 = outputs[52];
		FAUSTFLOAT* output53 = outputs[53];
		FAUSTFLOAT* output54 = outputs[54];
		FAUSTFLOAT* output55 = outputs[55];
		FAUSTFLOAT* output56 = outputs[56];
		FAUSTFLOAT* output57 = outputs[57];
		FAUSTFLOAT* output58 = outputs[58];
		FAUSTFLOAT* output59 = outputs[59];
		FAUSTFLOAT* output60 = outputs[60];
		FAUSTFLOAT* output61 = outputs[61];
		FAUSTFLOAT* output62 = outputs[62];
		FAUSTFLOAT* output63 = outputs[63];
		FAUSTFLOAT* output64 = outputs[64];
		FAUSTFLOAT* output65 = outputs[65];
		FAUSTFLOAT* output66 = outputs[66];
		FAUSTFLOAT* output67 = outputs[67];
		FAUSTFLOAT* output68 = outputs[68];
		FAUSTFLOAT* output69 = outputs[69];
		FAUSTFLOAT* output70 = outputs[70];
		FAUSTFLOAT* output71 = outputs[71];
		FAUSTFLOAT* output72 = outputs[72];
		FAUSTFLOAT* output73 = outputs[73];
		FAUSTFLOAT* output74 = outputs[74];
		FAUSTFLOAT* output75 = outputs[75];
		FAUSTFLOAT* output76 = outputs[76];
		FAUSTFLOAT* output77 = outputs[77];
		FAUSTFLOAT* output78 = outputs[78];
		FAUSTFLOAT* output79 = outputs[79];
		FAUSTFLOAT* output80 = outputs[80];
		FAUSTFLOAT* output81 = outputs[81];
		FAUSTFLOAT* output82 = outputs[82];
		FAUSTFLOAT* output83 = outputs[83];
		FAUSTFLOAT* output84 = outputs[84];
		FAUSTFLOAT* output85 = outputs[85];
		FAUSTFLOAT* output86 = outputs[86];
		FAUSTFLOAT* output87 = outputs[87];
		FAUSTFLOAT* output88 = outputs[88];
		FAUSTFLOAT* output89 = outputs[89];
		FAUSTFLOAT* output90 = outputs[90];
		FAUSTFLOAT* output91 = outputs[91];
		FAUSTFLOAT* output92 = outputs[92];
		FAUSTFLOAT* output93 = outputs[93];
		FAUSTFLOAT* output94 = outputs[94];
		FAUSTFLOAT* output95 = outputs[95];
		FAUSTFLOAT* output96 = outputs[96];
		FAUSTFLOAT* output97 = outputs[97];
		FAUSTFLOAT* output98 = outputs[98];
		FAUSTFLOAT* output99 = outputs[99];
		FAUSTFLOAT* output100 = outputs[100];
		FAUSTFLOAT* output101 = outputs[101];
		FAUSTFLOAT* output102 = outputs[102];
		FAUSTFLOAT* output103 = outputs[103];
		FAUSTFLOAT* output104 = outputs[104];
		FAUSTFLOAT* output105 = outputs[105];
		FAUSTFLOAT* output106 = outputs[106];
		FAUSTFLOAT* output107 = outputs[107];
		FAUSTFLOAT* output108 = outputs[108];
		FAUSTFLOAT* output109 = outputs[109];
		FAUSTFLOAT* output110 = outputs[110];
		FAUSTFLOAT* output111 = outputs[111];
		FAUSTFLOAT* output112 = outputs[112];
		FAUSTFLOAT* output113 = outputs[113];
		FAUSTFLOAT* output114 = outputs[114];
		FAUSTFLOAT* output115 = outputs[115];
		FAUSTFLOAT* output116 = outputs[116];
		FAUSTFLOAT* output117 = outputs[117];
		FAUSTFLOAT* output118 = outputs[118];
		FAUSTFLOAT* output119 = outputs[119];
		FAUSTFLOAT* output120 = outputs[120];
		int iSlow0 = (double(fCheckbox0) == 1.0);
		int iSlow1 = (iSlow0 & 0);
		int iSlow2 = (double(fCheckbox1) == 1.0);
		int iSlow3 = (iSlow2 & 0);
		int iSlow4 = (double(fCheckbox2) == 1.0);
		int iSlow5 = (iSlow4 & 0);
		int iSlow6 = (iSlow2 & 1);
		int iSlow7 = (iSlow4 & 1);
		int iSlow8 = (iSlow0 & 1);
		for (int i = 0; (i < count); i = (i + 1)) {
			double fTemp0 = double(input0[i]);
			double fTemp1 = (iSlow5?(0.0 - fTemp0):fTemp0);
			double fTemp2 = (iSlow3?(0.0 - fTemp1):fTemp1);
			output0[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp2):fTemp2));
			double fTemp3 = double(input1[i]);
			double fTemp4 = (iSlow5?(0.0 - fTemp3):fTemp3);
			double fTemp5 = (iSlow6?(0.0 - fTemp4):fTemp4);
			output1[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp5):fTemp5));
			double fTemp6 = double(input2[i]);
			double fTemp7 = (iSlow7?(0.0 - fTemp6):fTemp6);
			double fTemp8 = (iSlow3?(0.0 - fTemp7):fTemp7);
			output2[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp8):fTemp8));
			double fTemp9 = double(input3[i]);
			double fTemp10 = (iSlow5?(0.0 - fTemp9):fTemp9);
			double fTemp11 = (iSlow3?(0.0 - fTemp10):fTemp10);
			output3[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp11):fTemp11));
			double fTemp12 = double(input4[i]);
			double fTemp13 = (iSlow5?(0.0 - fTemp12):fTemp12);
			double fTemp14 = (iSlow6?(0.0 - fTemp13):fTemp13);
			output4[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp14):fTemp14));
			double fTemp15 = double(input5[i]);
			double fTemp16 = (iSlow7?(0.0 - fTemp15):fTemp15);
			double fTemp17 = (iSlow6?(0.0 - fTemp16):fTemp16);
			output5[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp17):fTemp17));
			double fTemp18 = double(input6[i]);
			double fTemp19 = (iSlow5?(0.0 - fTemp18):fTemp18);
			double fTemp20 = (iSlow3?(0.0 - fTemp19):fTemp19);
			output6[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp20):fTemp20));
			double fTemp21 = double(input7[i]);
			double fTemp22 = (iSlow7?(0.0 - fTemp21):fTemp21);
			double fTemp23 = (iSlow3?(0.0 - fTemp22):fTemp22);
			output7[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp23):fTemp23));
			double fTemp24 = double(input8[i]);
			double fTemp25 = (iSlow5?(0.0 - fTemp24):fTemp24);
			double fTemp26 = (iSlow3?(0.0 - fTemp25):fTemp25);
			output8[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp26):fTemp26));
			double fTemp27 = double(input9[i]);
			double fTemp28 = (iSlow5?(0.0 - fTemp27):fTemp27);
			double fTemp29 = (iSlow6?(0.0 - fTemp28):fTemp28);
			output9[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp29):fTemp29));
			double fTemp30 = double(input10[i]);
			double fTemp31 = (iSlow7?(0.0 - fTemp30):fTemp30);
			double fTemp32 = (iSlow6?(0.0 - fTemp31):fTemp31);
			output10[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp32):fTemp32));
			double fTemp33 = double(input11[i]);
			double fTemp34 = (iSlow5?(0.0 - fTemp33):fTemp33);
			double fTemp35 = (iSlow6?(0.0 - fTemp34):fTemp34);
			output11[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp35):fTemp35));
			double fTemp36 = double(input12[i]);
			double fTemp37 = (iSlow7?(0.0 - fTemp36):fTemp36);
			double fTemp38 = (iSlow3?(0.0 - fTemp37):fTemp37);
			output12[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp38):fTemp38));
			double fTemp39 = double(input13[i]);
			double fTemp40 = (iSlow5?(0.0 - fTemp39):fTemp39);
			double fTemp41 = (iSlow3?(0.0 - fTemp40):fTemp40);
			output13[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp41):fTemp41));
			double fTemp42 = double(input14[i]);
			double fTemp43 = (iSlow7?(0.0 - fTemp42):fTemp42);
			double fTemp44 = (iSlow3?(0.0 - fTemp43):fTemp43);
			output14[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp44):fTemp44));
			double fTemp45 = double(input15[i]);
			double fTemp46 = (iSlow5?(0.0 - fTemp45):fTemp45);
			double fTemp47 = (iSlow3?(0.0 - fTemp46):fTemp46);
			output15[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp47):fTemp47));
			double fTemp48 = double(input16[i]);
			double fTemp49 = (iSlow5?(0.0 - fTemp48):fTemp48);
			double fTemp50 = (iSlow6?(0.0 - fTemp49):fTemp49);
			output16[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp50):fTemp50));
			double fTemp51 = double(input17[i]);
			double fTemp52 = (iSlow7?(0.0 - fTemp51):fTemp51);
			double fTemp53 = (iSlow6?(0.0 - fTemp52):fTemp52);
			output17[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp53):fTemp53));
			double fTemp54 = double(input18[i]);
			double fTemp55 = (iSlow5?(0.0 - fTemp54):fTemp54);
			double fTemp56 = (iSlow6?(0.0 - fTemp55):fTemp55);
			output18[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp56):fTemp56));
			double fTemp57 = double(input19[i]);
			double fTemp58 = (iSlow7?(0.0 - fTemp57):fTemp57);
			double fTemp59 = (iSlow6?(0.0 - fTemp58):fTemp58);
			output19[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp59):fTemp59));
			double fTemp60 = double(input20[i]);
			double fTemp61 = (iSlow5?(0.0 - fTemp60):fTemp60);
			double fTemp62 = (iSlow3?(0.0 - fTemp61):fTemp61);
			output20[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp62):fTemp62));
			double fTemp63 = double(input21[i]);
			double fTemp64 = (iSlow7?(0.0 - fTemp63):fTemp63);
			double fTemp65 = (iSlow3?(0.0 - fTemp64):fTemp64);
			output21[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp65):fTemp65));
			double fTemp66 = double(input22[i]);
			double fTemp67 = (iSlow5?(0.0 - fTemp66):fTemp66);
			double fTemp68 = (iSlow3?(0.0 - fTemp67):fTemp67);
			output22[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp68):fTemp68));
			double fTemp69 = double(input23[i]);
			double fTemp70 = (iSlow7?(0.0 - fTemp69):fTemp69);
			double fTemp71 = (iSlow3?(0.0 - fTemp70):fTemp70);
			output23[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp71):fTemp71));
			double fTemp72 = double(input24[i]);
			double fTemp73 = (iSlow5?(0.0 - fTemp72):fTemp72);
			double fTemp74 = (iSlow3?(0.0 - fTemp73):fTemp73);
			output24[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp74):fTemp74));
			double fTemp75 = double(input25[i]);
			double fTemp76 = (iSlow5?(0.0 - fTemp75):fTemp75);
			double fTemp77 = (iSlow6?(0.0 - fTemp76):fTemp76);
			output25[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp77):fTemp77));
			double fTemp78 = double(input26[i]);
			double fTemp79 = (iSlow7?(0.0 - fTemp78):fTemp78);
			double fTemp80 = (iSlow6?(0.0 - fTemp79):fTemp79);
			output26[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp80):fTemp80));
			double fTemp81 = double(input27[i]);
			double fTemp82 = (iSlow5?(0.0 - fTemp81):fTemp81);
			double fTemp83 = (iSlow6?(0.0 - fTemp82):fTemp82);
			output27[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp83):fTemp83));
			double fTemp84 = double(input28[i]);
			double fTemp85 = (iSlow7?(0.0 - fTemp84):fTemp84);
			double fTemp86 = (iSlow6?(0.0 - fTemp85):fTemp85);
			output28[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp86):fTemp86));
			double fTemp87 = double(input29[i]);
			double fTemp88 = (iSlow5?(0.0 - fTemp87):fTemp87);
			double fTemp89 = (iSlow6?(0.0 - fTemp88):fTemp88);
			output29[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp89):fTemp89));
			double fTemp90 = double(input30[i]);
			double fTemp91 = (iSlow7?(0.0 - fTemp90):fTemp90);
			double fTemp92 = (iSlow3?(0.0 - fTemp91):fTemp91);
			output30[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp92):fTemp92));
			double fTemp93 = double(input31[i]);
			double fTemp94 = (iSlow5?(0.0 - fTemp93):fTemp93);
			double fTemp95 = (iSlow3?(0.0 - fTemp94):fTemp94);
			output31[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp95):fTemp95));
			double fTemp96 = double(input32[i]);
			double fTemp97 = (iSlow7?(0.0 - fTemp96):fTemp96);
			double fTemp98 = (iSlow3?(0.0 - fTemp97):fTemp97);
			output32[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp98):fTemp98));
			double fTemp99 = double(input33[i]);
			double fTemp100 = (iSlow5?(0.0 - fTemp99):fTemp99);
			double fTemp101 = (iSlow3?(0.0 - fTemp100):fTemp100);
			output33[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp101):fTemp101));
			double fTemp102 = double(input34[i]);
			double fTemp103 = (iSlow7?(0.0 - fTemp102):fTemp102);
			double fTemp104 = (iSlow3?(0.0 - fTemp103):fTemp103);
			output34[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp104):fTemp104));
			double fTemp105 = double(input35[i]);
			double fTemp106 = (iSlow5?(0.0 - fTemp105):fTemp105);
			double fTemp107 = (iSlow3?(0.0 - fTemp106):fTemp106);
			output35[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp107):fTemp107));
			double fTemp108 = double(input36[i]);
			double fTemp109 = (iSlow5?(0.0 - fTemp108):fTemp108);
			double fTemp110 = (iSlow6?(0.0 - fTemp109):fTemp109);
			output36[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp110):fTemp110));
			double fTemp111 = double(input37[i]);
			double fTemp112 = (iSlow7?(0.0 - fTemp111):fTemp111);
			double fTemp113 = (iSlow6?(0.0 - fTemp112):fTemp112);
			output37[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp113):fTemp113));
			double fTemp114 = double(input38[i]);
			double fTemp115 = (iSlow5?(0.0 - fTemp114):fTemp114);
			double fTemp116 = (iSlow6?(0.0 - fTemp115):fTemp115);
			output38[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp116):fTemp116));
			double fTemp117 = double(input39[i]);
			double fTemp118 = (iSlow7?(0.0 - fTemp117):fTemp117);
			double fTemp119 = (iSlow6?(0.0 - fTemp118):fTemp118);
			output39[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp119):fTemp119));
			double fTemp120 = double(input40[i]);
			double fTemp121 = (iSlow5?(0.0 - fTemp120):fTemp120);
			double fTemp122 = (iSlow6?(0.0 - fTemp121):fTemp121);
			output40[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp122):fTemp122));
			double fTemp123 = double(input41[i]);
			double fTemp124 = (iSlow7?(0.0 - fTemp123):fTemp123);
			double fTemp125 = (iSlow6?(0.0 - fTemp124):fTemp124);
			output41[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp125):fTemp125));
			double fTemp126 = double(input42[i]);
			double fTemp127 = (iSlow5?(0.0 - fTemp126):fTemp126);
			double fTemp128 = (iSlow3?(0.0 - fTemp127):fTemp127);
			output42[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp128):fTemp128));
			double fTemp129 = double(input43[i]);
			double fTemp130 = (iSlow7?(0.0 - fTemp129):fTemp129);
			double fTemp131 = (iSlow3?(0.0 - fTemp130):fTemp130);
			output43[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp131):fTemp131));
			double fTemp132 = double(input44[i]);
			double fTemp133 = (iSlow5?(0.0 - fTemp132):fTemp132);
			double fTemp134 = (iSlow3?(0.0 - fTemp133):fTemp133);
			output44[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp134):fTemp134));
			double fTemp135 = double(input45[i]);
			double fTemp136 = (iSlow7?(0.0 - fTemp135):fTemp135);
			double fTemp137 = (iSlow3?(0.0 - fTemp136):fTemp136);
			output45[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp137):fTemp137));
			double fTemp138 = double(input46[i]);
			double fTemp139 = (iSlow5?(0.0 - fTemp138):fTemp138);
			double fTemp140 = (iSlow3?(0.0 - fTemp139):fTemp139);
			output46[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp140):fTemp140));
			double fTemp141 = double(input47[i]);
			double fTemp142 = (iSlow7?(0.0 - fTemp141):fTemp141);
			double fTemp143 = (iSlow3?(0.0 - fTemp142):fTemp142);
			output47[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp143):fTemp143));
			double fTemp144 = double(input48[i]);
			double fTemp145 = (iSlow5?(0.0 - fTemp144):fTemp144);
			double fTemp146 = (iSlow3?(0.0 - fTemp145):fTemp145);
			output48[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp146):fTemp146));
			double fTemp147 = double(input49[i]);
			double fTemp148 = (iSlow5?(0.0 - fTemp147):fTemp147);
			double fTemp149 = (iSlow6?(0.0 - fTemp148):fTemp148);
			output49[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp149):fTemp149));
			double fTemp150 = double(input50[i]);
			double fTemp151 = (iSlow7?(0.0 - fTemp150):fTemp150);
			double fTemp152 = (iSlow6?(0.0 - fTemp151):fTemp151);
			output50[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp152):fTemp152));
			double fTemp153 = double(input51[i]);
			double fTemp154 = (iSlow5?(0.0 - fTemp153):fTemp153);
			double fTemp155 = (iSlow6?(0.0 - fTemp154):fTemp154);
			output51[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp155):fTemp155));
			double fTemp156 = double(input52[i]);
			double fTemp157 = (iSlow7?(0.0 - fTemp156):fTemp156);
			double fTemp158 = (iSlow6?(0.0 - fTemp157):fTemp157);
			output52[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp158):fTemp158));
			double fTemp159 = double(input53[i]);
			double fTemp160 = (iSlow5?(0.0 - fTemp159):fTemp159);
			double fTemp161 = (iSlow6?(0.0 - fTemp160):fTemp160);
			output53[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp161):fTemp161));
			double fTemp162 = double(input54[i]);
			double fTemp163 = (iSlow7?(0.0 - fTemp162):fTemp162);
			double fTemp164 = (iSlow6?(0.0 - fTemp163):fTemp163);
			output54[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp164):fTemp164));
			double fTemp165 = double(input55[i]);
			double fTemp166 = (iSlow5?(0.0 - fTemp165):fTemp165);
			double fTemp167 = (iSlow6?(0.0 - fTemp166):fTemp166);
			output55[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp167):fTemp167));
			double fTemp168 = double(input56[i]);
			double fTemp169 = (iSlow7?(0.0 - fTemp168):fTemp168);
			double fTemp170 = (iSlow3?(0.0 - fTemp169):fTemp169);
			output56[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp170):fTemp170));
			double fTemp171 = double(input57[i]);
			double fTemp172 = (iSlow5?(0.0 - fTemp171):fTemp171);
			double fTemp173 = (iSlow3?(0.0 - fTemp172):fTemp172);
			output57[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp173):fTemp173));
			double fTemp174 = double(input58[i]);
			double fTemp175 = (iSlow7?(0.0 - fTemp174):fTemp174);
			double fTemp176 = (iSlow3?(0.0 - fTemp175):fTemp175);
			output58[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp176):fTemp176));
			double fTemp177 = double(input59[i]);
			double fTemp178 = (iSlow5?(0.0 - fTemp177):fTemp177);
			double fTemp179 = (iSlow3?(0.0 - fTemp178):fTemp178);
			output59[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp179):fTemp179));
			double fTemp180 = double(input60[i]);
			double fTemp181 = (iSlow7?(0.0 - fTemp180):fTemp180);
			double fTemp182 = (iSlow3?(0.0 - fTemp181):fTemp181);
			output60[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp182):fTemp182));
			double fTemp183 = double(input61[i]);
			double fTemp184 = (iSlow5?(0.0 - fTemp183):fTemp183);
			double fTemp185 = (iSlow3?(0.0 - fTemp184):fTemp184);
			output61[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp185):fTemp185));
			double fTemp186 = double(input62[i]);
			double fTemp187 = (iSlow7?(0.0 - fTemp186):fTemp186);
			double fTemp188 = (iSlow3?(0.0 - fTemp187):fTemp187);
			output62[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp188):fTemp188));
			double fTemp189 = double(input63[i]);
			double fTemp190 = (iSlow5?(0.0 - fTemp189):fTemp189);
			double fTemp191 = (iSlow3?(0.0 - fTemp190):fTemp190);
			output63[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp191):fTemp191));
			double fTemp192 = double(input64[i]);
			double fTemp193 = (iSlow5?(0.0 - fTemp192):fTemp192);
			double fTemp194 = (iSlow6?(0.0 - fTemp193):fTemp193);
			output64[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp194):fTemp194));
			double fTemp195 = double(input65[i]);
			double fTemp196 = (iSlow7?(0.0 - fTemp195):fTemp195);
			double fTemp197 = (iSlow6?(0.0 - fTemp196):fTemp196);
			output65[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp197):fTemp197));
			double fTemp198 = double(input66[i]);
			double fTemp199 = (iSlow5?(0.0 - fTemp198):fTemp198);
			double fTemp200 = (iSlow6?(0.0 - fTemp199):fTemp199);
			output66[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp200):fTemp200));
			double fTemp201 = double(input67[i]);
			double fTemp202 = (iSlow7?(0.0 - fTemp201):fTemp201);
			double fTemp203 = (iSlow6?(0.0 - fTemp202):fTemp202);
			output67[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp203):fTemp203));
			double fTemp204 = double(input68[i]);
			double fTemp205 = (iSlow5?(0.0 - fTemp204):fTemp204);
			double fTemp206 = (iSlow6?(0.0 - fTemp205):fTemp205);
			output68[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp206):fTemp206));
			double fTemp207 = double(input69[i]);
			double fTemp208 = (iSlow7?(0.0 - fTemp207):fTemp207);
			double fTemp209 = (iSlow6?(0.0 - fTemp208):fTemp208);
			output69[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp209):fTemp209));
			double fTemp210 = double(input70[i]);
			double fTemp211 = (iSlow5?(0.0 - fTemp210):fTemp210);
			double fTemp212 = (iSlow6?(0.0 - fTemp211):fTemp211);
			output70[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp212):fTemp212));
			double fTemp213 = double(input71[i]);
			double fTemp214 = (iSlow7?(0.0 - fTemp213):fTemp213);
			double fTemp215 = (iSlow6?(0.0 - fTemp214):fTemp214);
			output71[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp215):fTemp215));
			double fTemp216 = double(input72[i]);
			double fTemp217 = (iSlow5?(0.0 - fTemp216):fTemp216);
			double fTemp218 = (iSlow3?(0.0 - fTemp217):fTemp217);
			output72[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp218):fTemp218));
			double fTemp219 = double(input73[i]);
			double fTemp220 = (iSlow7?(0.0 - fTemp219):fTemp219);
			double fTemp221 = (iSlow3?(0.0 - fTemp220):fTemp220);
			output73[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp221):fTemp221));
			double fTemp222 = double(input74[i]);
			double fTemp223 = (iSlow5?(0.0 - fTemp222):fTemp222);
			double fTemp224 = (iSlow3?(0.0 - fTemp223):fTemp223);
			output74[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp224):fTemp224));
			double fTemp225 = double(input75[i]);
			double fTemp226 = (iSlow7?(0.0 - fTemp225):fTemp225);
			double fTemp227 = (iSlow3?(0.0 - fTemp226):fTemp226);
			output75[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp227):fTemp227));
			double fTemp228 = double(input76[i]);
			double fTemp229 = (iSlow5?(0.0 - fTemp228):fTemp228);
			double fTemp230 = (iSlow3?(0.0 - fTemp229):fTemp229);
			output76[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp230):fTemp230));
			double fTemp231 = double(input77[i]);
			double fTemp232 = (iSlow7?(0.0 - fTemp231):fTemp231);
			double fTemp233 = (iSlow3?(0.0 - fTemp232):fTemp232);
			output77[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp233):fTemp233));
			double fTemp234 = double(input78[i]);
			double fTemp235 = (iSlow5?(0.0 - fTemp234):fTemp234);
			double fTemp236 = (iSlow3?(0.0 - fTemp235):fTemp235);
			output78[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp236):fTemp236));
			double fTemp237 = double(input79[i]);
			double fTemp238 = (iSlow7?(0.0 - fTemp237):fTemp237);
			double fTemp239 = (iSlow3?(0.0 - fTemp238):fTemp238);
			output79[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp239):fTemp239));
			double fTemp240 = double(input80[i]);
			double fTemp241 = (iSlow5?(0.0 - fTemp240):fTemp240);
			double fTemp242 = (iSlow3?(0.0 - fTemp241):fTemp241);
			output80[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp242):fTemp242));
			double fTemp243 = double(input81[i]);
			double fTemp244 = (iSlow5?(0.0 - fTemp243):fTemp243);
			double fTemp245 = (iSlow6?(0.0 - fTemp244):fTemp244);
			output81[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp245):fTemp245));
			double fTemp246 = double(input82[i]);
			double fTemp247 = (iSlow7?(0.0 - fTemp246):fTemp246);
			double fTemp248 = (iSlow6?(0.0 - fTemp247):fTemp247);
			output82[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp248):fTemp248));
			double fTemp249 = double(input83[i]);
			double fTemp250 = (iSlow5?(0.0 - fTemp249):fTemp249);
			double fTemp251 = (iSlow6?(0.0 - fTemp250):fTemp250);
			output83[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp251):fTemp251));
			double fTemp252 = double(input84[i]);
			double fTemp253 = (iSlow7?(0.0 - fTemp252):fTemp252);
			double fTemp254 = (iSlow6?(0.0 - fTemp253):fTemp253);
			output84[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp254):fTemp254));
			double fTemp255 = double(input85[i]);
			double fTemp256 = (iSlow5?(0.0 - fTemp255):fTemp255);
			double fTemp257 = (iSlow6?(0.0 - fTemp256):fTemp256);
			output85[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp257):fTemp257));
			double fTemp258 = double(input86[i]);
			double fTemp259 = (iSlow7?(0.0 - fTemp258):fTemp258);
			double fTemp260 = (iSlow6?(0.0 - fTemp259):fTemp259);
			output86[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp260):fTemp260));
			double fTemp261 = double(input87[i]);
			double fTemp262 = (iSlow5?(0.0 - fTemp261):fTemp261);
			double fTemp263 = (iSlow6?(0.0 - fTemp262):fTemp262);
			output87[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp263):fTemp263));
			double fTemp264 = double(input88[i]);
			double fTemp265 = (iSlow7?(0.0 - fTemp264):fTemp264);
			double fTemp266 = (iSlow6?(0.0 - fTemp265):fTemp265);
			output88[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp266):fTemp266));
			double fTemp267 = double(input89[i]);
			double fTemp268 = (iSlow5?(0.0 - fTemp267):fTemp267);
			double fTemp269 = (iSlow6?(0.0 - fTemp268):fTemp268);
			output89[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp269):fTemp269));
			double fTemp270 = double(input90[i]);
			double fTemp271 = (iSlow7?(0.0 - fTemp270):fTemp270);
			double fTemp272 = (iSlow3?(0.0 - fTemp271):fTemp271);
			output90[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp272):fTemp272));
			double fTemp273 = double(input91[i]);
			double fTemp274 = (iSlow5?(0.0 - fTemp273):fTemp273);
			double fTemp275 = (iSlow3?(0.0 - fTemp274):fTemp274);
			output91[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp275):fTemp275));
			double fTemp276 = double(input92[i]);
			double fTemp277 = (iSlow7?(0.0 - fTemp276):fTemp276);
			double fTemp278 = (iSlow3?(0.0 - fTemp277):fTemp277);
			output92[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp278):fTemp278));
			double fTemp279 = double(input93[i]);
			double fTemp280 = (iSlow5?(0.0 - fTemp279):fTemp279);
			double fTemp281 = (iSlow3?(0.0 - fTemp280):fTemp280);
			output93[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp281):fTemp281));
			double fTemp282 = double(input94[i]);
			double fTemp283 = (iSlow7?(0.0 - fTemp282):fTemp282);
			double fTemp284 = (iSlow3?(0.0 - fTemp283):fTemp283);
			output94[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp284):fTemp284));
			double fTemp285 = double(input95[i]);
			double fTemp286 = (iSlow5?(0.0 - fTemp285):fTemp285);
			double fTemp287 = (iSlow3?(0.0 - fTemp286):fTemp286);
			output95[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp287):fTemp287));
			double fTemp288 = double(input96[i]);
			double fTemp289 = (iSlow7?(0.0 - fTemp288):fTemp288);
			double fTemp290 = (iSlow3?(0.0 - fTemp289):fTemp289);
			output96[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp290):fTemp290));
			double fTemp291 = double(input97[i]);
			double fTemp292 = (iSlow5?(0.0 - fTemp291):fTemp291);
			double fTemp293 = (iSlow3?(0.0 - fTemp292):fTemp292);
			output97[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp293):fTemp293));
			double fTemp294 = double(input98[i]);
			double fTemp295 = (iSlow7?(0.0 - fTemp294):fTemp294);
			double fTemp296 = (iSlow3?(0.0 - fTemp295):fTemp295);
			output98[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp296):fTemp296));
			double fTemp297 = double(input99[i]);
			double fTemp298 = (iSlow5?(0.0 - fTemp297):fTemp297);
			double fTemp299 = (iSlow3?(0.0 - fTemp298):fTemp298);
			output99[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp299):fTemp299));
			double fTemp300 = double(input100[i]);
			double fTemp301 = (iSlow5?(0.0 - fTemp300):fTemp300);
			double fTemp302 = (iSlow6?(0.0 - fTemp301):fTemp301);
			output100[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp302):fTemp302));
			double fTemp303 = double(input101[i]);
			double fTemp304 = (iSlow7?(0.0 - fTemp303):fTemp303);
			double fTemp305 = (iSlow6?(0.0 - fTemp304):fTemp304);
			output101[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp305):fTemp305));
			double fTemp306 = double(input102[i]);
			double fTemp307 = (iSlow5?(0.0 - fTemp306):fTemp306);
			double fTemp308 = (iSlow6?(0.0 - fTemp307):fTemp307);
			output102[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp308):fTemp308));
			double fTemp309 = double(input103[i]);
			double fTemp310 = (iSlow7?(0.0 - fTemp309):fTemp309);
			double fTemp311 = (iSlow6?(0.0 - fTemp310):fTemp310);
			output103[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp311):fTemp311));
			double fTemp312 = double(input104[i]);
			double fTemp313 = (iSlow5?(0.0 - fTemp312):fTemp312);
			double fTemp314 = (iSlow6?(0.0 - fTemp313):fTemp313);
			output104[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp314):fTemp314));
			double fTemp315 = double(input105[i]);
			double fTemp316 = (iSlow7?(0.0 - fTemp315):fTemp315);
			double fTemp317 = (iSlow6?(0.0 - fTemp316):fTemp316);
			output105[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp317):fTemp317));
			double fTemp318 = double(input106[i]);
			double fTemp319 = (iSlow5?(0.0 - fTemp318):fTemp318);
			double fTemp320 = (iSlow6?(0.0 - fTemp319):fTemp319);
			output106[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp320):fTemp320));
			double fTemp321 = double(input107[i]);
			double fTemp322 = (iSlow7?(0.0 - fTemp321):fTemp321);
			double fTemp323 = (iSlow6?(0.0 - fTemp322):fTemp322);
			output107[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp323):fTemp323));
			double fTemp324 = double(input108[i]);
			double fTemp325 = (iSlow5?(0.0 - fTemp324):fTemp324);
			double fTemp326 = (iSlow6?(0.0 - fTemp325):fTemp325);
			output108[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp326):fTemp326));
			double fTemp327 = double(input109[i]);
			double fTemp328 = (iSlow7?(0.0 - fTemp327):fTemp327);
			double fTemp329 = (iSlow6?(0.0 - fTemp328):fTemp328);
			output109[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp329):fTemp329));
			double fTemp330 = double(input110[i]);
			double fTemp331 = (iSlow5?(0.0 - fTemp330):fTemp330);
			double fTemp332 = (iSlow3?(0.0 - fTemp331):fTemp331);
			output110[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp332):fTemp332));
			double fTemp333 = double(input111[i]);
			double fTemp334 = (iSlow7?(0.0 - fTemp333):fTemp333);
			double fTemp335 = (iSlow3?(0.0 - fTemp334):fTemp334);
			output111[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp335):fTemp335));
			double fTemp336 = double(input112[i]);
			double fTemp337 = (iSlow5?(0.0 - fTemp336):fTemp336);
			double fTemp338 = (iSlow3?(0.0 - fTemp337):fTemp337);
			output112[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp338):fTemp338));
			double fTemp339 = double(input113[i]);
			double fTemp340 = (iSlow7?(0.0 - fTemp339):fTemp339);
			double fTemp341 = (iSlow3?(0.0 - fTemp340):fTemp340);
			output113[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp341):fTemp341));
			double fTemp342 = double(input114[i]);
			double fTemp343 = (iSlow5?(0.0 - fTemp342):fTemp342);
			double fTemp344 = (iSlow3?(0.0 - fTemp343):fTemp343);
			output114[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp344):fTemp344));
			double fTemp345 = double(input115[i]);
			double fTemp346 = (iSlow7?(0.0 - fTemp345):fTemp345);
			double fTemp347 = (iSlow3?(0.0 - fTemp346):fTemp346);
			output115[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp347):fTemp347));
			double fTemp348 = double(input116[i]);
			double fTemp349 = (iSlow5?(0.0 - fTemp348):fTemp348);
			double fTemp350 = (iSlow3?(0.0 - fTemp349):fTemp349);
			output116[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp350):fTemp350));
			double fTemp351 = double(input117[i]);
			double fTemp352 = (iSlow7?(0.0 - fTemp351):fTemp351);
			double fTemp353 = (iSlow3?(0.0 - fTemp352):fTemp352);
			output117[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp353):fTemp353));
			double fTemp354 = double(input118[i]);
			double fTemp355 = (iSlow5?(0.0 - fTemp354):fTemp354);
			double fTemp356 = (iSlow3?(0.0 - fTemp355):fTemp355);
			output118[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp356):fTemp356));
			double fTemp357 = double(input119[i]);
			double fTemp358 = (iSlow7?(0.0 - fTemp357):fTemp357);
			double fTemp359 = (iSlow3?(0.0 - fTemp358):fTemp358);
			output119[i] = FAUSTFLOAT((iSlow8?(0.0 - fTemp359):fTemp359));
			double fTemp360 = double(input120[i]);
			double fTemp361 = (iSlow5?(0.0 - fTemp360):fTemp360);
			double fTemp362 = (iSlow3?(0.0 - fTemp361):fTemp361);
			output120[i] = FAUSTFLOAT((iSlow1?(0.0 - fTemp362):fTemp362));
			
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
    #if defined(F2SC_DEBUG_MES)
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

#if defined(F2SC_DEBUG_MES) & defined(SC_API_EXPORT)
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

#if defined(F2SC_DEBUG_MES)
    Print("Faust: %s numControls=%d\n", name.c_str(), g_numControls);
#endif // F2SC_DEBUG_MES
}

#ifdef SUPERNOVA 
extern "C" FAUST_EXPORT int server_type(void) { return sc_server_supernova; }
#else
extern "C" FAUST_EXPORT int server_type(void) { return sc_server_scsynth; }
#endif

// EOF

#endif
