/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2016"
license: "GPL"
name: "HOARotator4"
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

double mydsp_faustpower2_f(double value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif
#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fHslider2;
	int fSamplingFreq;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
		m->declare("license", "GPL");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("name", "HOARotator4");
		m->declare("version", "1.0");
		m->declare("ymn.lib/author", "Pierre Lecomte");
		m->declare("ymn.lib/copyright", "(c) Pierre Lecomte 2017");
		m->declare("ymn.lib/license", "GPL");
		m->declare("ymn.lib/name", "Spherical Harmonics library");
		m->declare("ymn.lib/version", "2.0");
	}

	virtual int getNumInputs() {
		return 25;
		
	}
	virtual int getNumOutputs() {
		return 25;
		
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
		fHslider0 = FAUSTFLOAT(0.0);
		fHslider1 = FAUSTFLOAT(0.0);
		fHslider2 = FAUSTFLOAT(0.0);
		
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
		ui_interface->openVerticalBox("HOARotator4");
		ui_interface->declare(&fHslider2, "osc", "/picth 0 360");
		ui_interface->addHorizontalSlider("Pitch", &fHslider2, 0.0, -3.1415926535897931, 3.1415926535897931, 0.01);
		ui_interface->declare(&fHslider1, "osc", "/roll 0 360");
		ui_interface->addHorizontalSlider("Roll", &fHslider1, 0.0, -3.1415926535897931, 3.1415926535897931, 0.01);
		ui_interface->declare(&fHslider0, "osc", "/yaw 0 360");
		ui_interface->addHorizontalSlider("Yaw", &fHslider0, 0.0, -3.1415926535897931, 3.1415926535897931, 0.01);
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
		double fSlow0 = double(fHslider0);
		double fSlow1 = cos(fSlow0);
		double fSlow2 = double(fHslider1);
		double fSlow3 = cos(fSlow2);
		double fSlow4 = (fSlow1 * fSlow3);
		double fSlow5 = sin(fSlow0);
		double fSlow6 = sin(fSlow2);
		double fSlow7 = (fSlow5 * fSlow6);
		double fSlow8 = double(fHslider2);
		double fSlow9 = sin(fSlow8);
		double fSlow10 = (fSlow4 - (fSlow7 * fSlow9));
		double fSlow11 = cos(fSlow8);
		double fSlow12 = (0.0 - fSlow11);
		double fSlow13 = (fSlow6 * fSlow12);
		double fSlow14 = (fSlow6 * fSlow9);
		double fSlow15 = (fSlow5 * fSlow3);
		double fSlow16 = ((fSlow14 * fSlow1) + fSlow15);
		double fSlow17 = (fSlow6 * fSlow1);
		double fSlow18 = (fSlow5 * fSlow9);
		double fSlow19 = (fSlow17 + (fSlow18 * fSlow3));
		double fSlow20 = (fSlow3 * fSlow11);
		double fSlow21 = (fSlow9 * fSlow1);
		double fSlow22 = ((fSlow21 * (0.0 - fSlow3)) + fSlow7);
		double fSlow23 = (fSlow5 * fSlow12);
		double fSlow24 = (fSlow1 * fSlow11);
		double fSlow25 = ((fSlow24 * fSlow10) + (fSlow23 * fSlow16));
		double fSlow26 = mydsp_faustpower2_f(fSlow12);
		double fSlow27 = ((fSlow9 * fSlow10) + (fSlow7 * fSlow26));
		double fSlow28 = (1.7320508075688772 * (fSlow14 * fSlow12));
		double fSlow29 = ((fSlow9 * fSlow16) + ((fSlow17 * fSlow11) * fSlow12));
		double fSlow30 = ((fSlow24 * fSlow16) - (fSlow23 * fSlow10));
		double fSlow31 = ((fSlow19 * fSlow16) + (fSlow22 * fSlow10));
		double fSlow32 = ((fSlow20 * fSlow10) + (1.0000000000000002 * ((fSlow6 * fSlow19) * fSlow12)));
		double fSlow33 = (1.7320508075688772 * (((fSlow6 * fSlow3) * fSlow11) * fSlow12));
		double fSlow34 = ((fSlow20 * fSlow16) + (1.0000000000000002 * ((fSlow6 * fSlow22) * fSlow12)));
		double fSlow35 = ((fSlow22 * fSlow16) - (fSlow19 * fSlow10));
		double fSlow36 = (((fSlow5 * fSlow1) * fSlow11) * fSlow12);
		double fSlow37 = (fSlow16 * fSlow10);
		double fSlow38 = ((1.1547005383792515 * (fSlow22 * fSlow19)) - (0.57735026918962573 * (fSlow36 + fSlow37)));
		double fSlow39 = (fSlow6 * fSlow10);
		double fSlow40 = ((1.1547005383792515 * (fSlow20 * fSlow19)) - (0.57735026918962573 * ((fSlow18 + fSlow39) * fSlow12)));
		double fSlow41 = mydsp_faustpower2_f(fSlow3);
		double fSlow42 = mydsp_faustpower2_f(fSlow11);
		double fSlow43 = (fSlow41 * fSlow42);
		double fSlow44 = mydsp_faustpower2_f(fSlow9);
		double fSlow45 = mydsp_faustpower2_f(fSlow6);
		double fSlow46 = (fSlow45 * fSlow26);
		double fSlow47 = (fSlow44 + fSlow46);
		double fSlow48 = (fSlow43 - (0.5 * fSlow47));
		double fSlow49 = (fSlow21 * fSlow11);
		double fSlow50 = (fSlow13 * fSlow16);
		double fSlow51 = ((1.1547005383792515 * (fSlow20 * fSlow22)) - (0.57735026918962573 * (fSlow49 + fSlow50)));
		double fSlow52 = (mydsp_faustpower2_f(fSlow1) * fSlow42);
		double fSlow53 = mydsp_faustpower2_f(fSlow16);
		double fSlow54 = (mydsp_faustpower2_f(fSlow5) * fSlow26);
		double fSlow55 = mydsp_faustpower2_f(fSlow10);
		double fSlow56 = ((0.57735026918962573 * (mydsp_faustpower2_f(fSlow22) - mydsp_faustpower2_f(fSlow19))) - (0.28867513459481287 * ((fSlow52 + fSlow53) - (fSlow54 + fSlow55))));
		double fSlow57 = (((fSlow5 * fSlow22) * fSlow12) + (fSlow24 * fSlow19));
		double fSlow58 = (((fSlow15 * fSlow11) * fSlow12) + (1.0000000000000002 * (fSlow9 * fSlow19)));
		double fSlow59 = (1.7320508075688772 * ((fSlow9 * fSlow3) * fSlow11));
		double fSlow60 = ((fSlow4 * fSlow42) + (1.0000000000000002 * (fSlow9 * fSlow22)));
		double fSlow61 = ((fSlow24 * fSlow22) - ((fSlow5 * fSlow19) * fSlow12));
		double fSlow62 = (fSlow36 - fSlow37);
		double fSlow63 = (fSlow18 - fSlow39);
		double fSlow64 = (fSlow12 * fSlow63);
		double fSlow65 = (fSlow44 - fSlow46);
		double fSlow66 = (0.8660254037844386 * fSlow65);
		double fSlow67 = (fSlow49 - fSlow50);
		double fSlow68 = ((fSlow52 + fSlow55) - (fSlow54 + fSlow53));
		double fSlow69 = (0.5 * fSlow68);
		double fSlow70 = (0.5 * (fSlow10 * fSlow68));
		double fSlow71 = ((fSlow24 * fSlow25) + (fSlow23 * fSlow30));
		double fSlow72 = (fSlow16 * fSlow62);
		double fSlow73 = (fSlow70 + (fSlow71 + fSlow72));
		double fSlow74 = (fSlow24 * fSlow30);
		double fSlow75 = (0.5 * (fSlow16 * fSlow68));
		double fSlow76 = (fSlow23 * fSlow25);
		double fSlow77 = (fSlow10 * fSlow62);
		double fSlow78 = ((fSlow74 + fSlow75) - (fSlow76 + fSlow77));
		double fSlow79 = (fSlow9 * fSlow25);
		double fSlow80 = (fSlow13 * fSlow62);
		double fSlow81 = (fSlow79 + fSlow80);
		double fSlow82 = (fSlow9 * fSlow30);
		double fSlow83 = (0.5 * (fSlow13 * fSlow68));
		double fSlow84 = (fSlow82 + fSlow83);
		double fSlow85 = (1.7320508075688772 * fSlow44);
		double fSlow86 = (fSlow66 + fSlow85);
		double fSlow87 = (0.9128709291752769 * (fSlow13 * fSlow86));
		double fSlow88 = (fSlow9 * fSlow27);
		double fSlow89 = (fSlow6 * fSlow26);
		double fSlow90 = (fSlow89 * fSlow63);
		double fSlow91 = (fSlow88 + fSlow90);
		double fSlow92 = (fSlow9 * fSlow29);
		double fSlow93 = (fSlow13 * fSlow67);
		double fSlow94 = (fSlow92 + fSlow93);
		double fSlow95 = (((fSlow22 * fSlow25) + (fSlow19 * fSlow30)) + ((fSlow61 * fSlow10) + ((((fSlow31 * fSlow1) * fSlow11) + ((fSlow35 * fSlow5) * fSlow12)) + (fSlow57 * fSlow16))));
		double fSlow96 = (((fSlow22 * fSlow30) - (fSlow19 * fSlow25)) + ((((fSlow35 * fSlow1) * fSlow11) + (fSlow61 * fSlow16)) - (((fSlow31 * fSlow5) * fSlow12) + (fSlow57 * fSlow10))));
		double fSlow97 = (((fSlow61 * fSlow6) * fSlow12) + ((fSlow20 * fSlow30) + (fSlow35 * fSlow9)));
		double fSlow98 = ((fSlow20 * fSlow27) + ((fSlow32 * fSlow9) + ((fSlow58 * fSlow6) * fSlow12)));
		double fSlow99 = ((fSlow20 * fSlow29) + ((fSlow34 * fSlow9) + ((fSlow60 * fSlow6) * fSlow12)));
		double fSlow100 = (((fSlow57 * fSlow6) * fSlow12) + ((fSlow20 * fSlow25) + (fSlow31 * fSlow9)));
		double fSlow101 = (3.872983346207417 * (((fSlow14 * fSlow3) * fSlow11) * fSlow12));
		double fSlow102 = (((0.5163977794943222 * ((fSlow31 * fSlow22) + (fSlow35 * fSlow19))) + (0.44721359549995798 * ((fSlow38 * fSlow16) + (fSlow56 * fSlow10)))) - (0.12909944487358055 * (fSlow71 - (fSlow72 + fSlow70))));
		double fSlow103 = (((1.2649110640673518 * ((fSlow31 * fSlow3) * fSlow11)) + (1.0954451150103324 * ((fSlow38 * fSlow6) * fSlow12))) - (0.31622776601683794 * (fSlow79 - fSlow80)));
		double fSlow104 = ((((fSlow32 * fSlow3) * fSlow11) + (0.8660254037844386 * ((fSlow40 * fSlow6) * fSlow12))) - (0.25 * (fSlow88 - fSlow90)));
		double fSlow105 = ((1.6329931618554518 * fSlow43) + (0.81649658092772603 * fSlow48));
		double fSlow106 = (fSlow105 - (0.23570226039551584 * (fSlow85 - fSlow66)));
		double fSlow107 = ((fSlow106 * fSlow6) * fSlow12);
		double fSlow108 = ((((fSlow34 * fSlow3) * fSlow11) + (0.8660254037844386 * ((fSlow51 * fSlow6) * fSlow12))) - (0.25 * (fSlow92 - fSlow93)));
		double fSlow109 = (((1.2649110640673518 * ((fSlow35 * fSlow3) * fSlow11)) + (1.0954451150103324 * ((fSlow56 * fSlow6) * fSlow12))) - (0.31622776601683794 * (fSlow82 - fSlow83)));
		double fSlow110 = (((0.5163977794943222 * ((fSlow35 * fSlow22) - (fSlow31 * fSlow19))) + (0.44721359549995798 * ((fSlow56 * fSlow16) - (fSlow38 * fSlow10)))) - (0.12909944487358055 * ((fSlow74 + fSlow77) - (fSlow76 + fSlow75))));
		double fSlow111 = (((fSlow57 * fSlow1) * fSlow11) + ((fSlow61 * fSlow5) * fSlow12));
		double fSlow112 = (fSlow31 * fSlow16);
		double fSlow113 = (fSlow35 * fSlow10);
		double fSlow114 = ((0.54772255750516607 * ((fSlow38 * fSlow22) + (fSlow56 * fSlow19))) - (0.31622776601683794 * ((fSlow111 + fSlow112) + fSlow113)));
		double fSlow115 = (fSlow57 * fSlow9);
		double fSlow116 = ((fSlow31 * fSlow6) * fSlow12);
		double fSlow117 = ((1.3416407864998738 * ((fSlow38 * fSlow3) * fSlow11)) - (0.7745966692414834 * (fSlow115 + fSlow116)));
		double fSlow118 = (fSlow58 * fSlow9);
		double fSlow119 = ((fSlow32 * fSlow6) * fSlow12);
		double fSlow120 = ((1.0606601717798212 * ((fSlow40 * fSlow3) * fSlow11)) - (0.61237243569579447 * (fSlow118 + fSlow119)));
		double fSlow121 = (fSlow48 - (0.99999999999999989 * fSlow47));
		double fSlow122 = ((fSlow121 * fSlow3) * fSlow11);
		double fSlow123 = (fSlow60 * fSlow9);
		double fSlow124 = ((fSlow34 * fSlow6) * fSlow12);
		double fSlow125 = ((1.0606601717798212 * ((fSlow51 * fSlow3) * fSlow11)) - (0.61237243569579447 * (fSlow123 + fSlow124)));
		double fSlow126 = (fSlow61 * fSlow9);
		double fSlow127 = ((fSlow35 * fSlow6) * fSlow12);
		double fSlow128 = ((1.3416407864998738 * ((fSlow56 * fSlow3) * fSlow11)) - (0.7745966692414834 * (fSlow126 + fSlow127)));
		double fSlow129 = ((fSlow61 * fSlow1) * fSlow11);
		double fSlow130 = (fSlow35 * fSlow16);
		double fSlow131 = ((fSlow57 * fSlow5) * fSlow12);
		double fSlow132 = (fSlow31 * fSlow10);
		double fSlow133 = ((0.54772255750516607 * ((fSlow56 * fSlow22) - (fSlow38 * fSlow19))) - (0.31622776601683794 * ((fSlow129 + fSlow130) - (fSlow131 + fSlow132))));
		double fSlow134 = (fSlow24 * fSlow62);
		double fSlow135 = (fSlow10 * fSlow30);
		double fSlow136 = (0.5 * (fSlow23 * fSlow68));
		double fSlow137 = (fSlow16 * fSlow25);
		double fSlow138 = (((0.5163977794943222 * ((fSlow57 * fSlow22) + (fSlow61 * fSlow19))) + (0.44721359549995798 * (((fSlow38 * fSlow1) * fSlow11) + ((fSlow56 * fSlow5) * fSlow12)))) - (0.12909944487358055 * (((fSlow134 + fSlow135) + fSlow136) + fSlow137)));
		double fSlow139 = (fSlow9 * fSlow62);
		double fSlow140 = (fSlow13 * fSlow25);
		double fSlow141 = (((1.2649110640673518 * ((fSlow57 * fSlow3) * fSlow11)) + (1.0954451150103324 * (fSlow38 * fSlow9))) - (0.31622776601683794 * (fSlow139 + fSlow140)));
		double fSlow142 = (fSlow9 * fSlow63);
		double fSlow143 = (fSlow6 * fSlow27);
		double fSlow144 = ((((fSlow58 * fSlow3) * fSlow11) + (0.8660254037844386 * (fSlow40 * fSlow9))) - (0.25 * ((fSlow142 + fSlow143) * fSlow12)));
		double fSlow145 = (1.7320508075688772 * fSlow46);
		double fSlow146 = (fSlow105 - (0.23570226039551584 * (fSlow66 + fSlow145)));
		double fSlow147 = (fSlow146 * fSlow9);
		double fSlow148 = (fSlow9 * fSlow67);
		double fSlow149 = (fSlow13 * fSlow29);
		double fSlow150 = ((((fSlow60 * fSlow3) * fSlow11) + (0.8660254037844386 * (fSlow51 * fSlow9))) - (0.25 * (fSlow148 + fSlow149)));
		double fSlow151 = (0.5 * (fSlow9 * fSlow68));
		double fSlow152 = (fSlow13 * fSlow30);
		double fSlow153 = (((1.2649110640673518 * ((fSlow61 * fSlow3) * fSlow11)) + (1.0954451150103324 * (fSlow56 * fSlow9))) - (0.31622776601683794 * (fSlow151 + fSlow152)));
		double fSlow154 = (0.5 * (fSlow24 * fSlow68));
		double fSlow155 = (fSlow16 * fSlow30);
		double fSlow156 = (fSlow23 * fSlow62);
		double fSlow157 = (fSlow10 * fSlow25);
		double fSlow158 = (((0.5163977794943222 * ((fSlow61 * fSlow22) - (fSlow57 * fSlow19))) + (0.44721359549995798 * (((fSlow56 * fSlow1) * fSlow11) - ((fSlow38 * fSlow5) * fSlow12)))) - (0.12909944487358055 * ((fSlow154 + fSlow155) - (fSlow156 + fSlow157))));
		double fSlow159 = (((fSlow22 * fSlow62) + (0.5 * (fSlow19 * fSlow68))) + (fSlow111 - (fSlow112 + fSlow113)));
		double fSlow160 = (((0.5 * (fSlow22 * fSlow68)) - (fSlow19 * fSlow62)) + ((fSlow129 + fSlow132) - (fSlow131 + fSlow130)));
		double fSlow161 = (((0.5 * (fSlow20 * fSlow68)) + fSlow126) - fSlow127);
		double fSlow162 = (fSlow20 * fSlow12);
		double fSlow163 = ((fSlow162 * fSlow63) + (fSlow118 - fSlow119));
		double fSlow164 = ((fSlow20 * fSlow67) + (fSlow123 - fSlow124));
		double fSlow165 = (((fSlow20 * fSlow62) + fSlow115) - fSlow116);
		double fSlow166 = (1.9364916731037085 * (fSlow20 * fSlow65));
		double fSlow167 = ((fSlow134 + fSlow136) - (fSlow135 + fSlow137));
		double fSlow168 = ((fSlow154 + fSlow157) - (fSlow156 + fSlow155));
		double fSlow169 = (fSlow139 - fSlow140);
		double fSlow170 = (fSlow151 - fSlow152);
		double fSlow171 = (fSlow66 - fSlow145);
		double fSlow172 = (0.9128709291752769 * (fSlow9 * fSlow171));
		double fSlow173 = (fSlow142 - fSlow143);
		double fSlow174 = (fSlow12 * fSlow173);
		double fSlow175 = (fSlow148 - fSlow149);
		double fSlow176 = ((fSlow24 * fSlow73) + (fSlow23 * fSlow78));
		double fSlow177 = ((fSlow16 * fSlow167) + (fSlow10 * fSlow168));
		double fSlow178 = (fSlow176 + fSlow177);
		double fSlow179 = ((fSlow24 * fSlow78) - (fSlow23 * fSlow73));
		double fSlow180 = ((fSlow16 * fSlow168) - (fSlow10 * fSlow167));
		double fSlow181 = (fSlow179 + fSlow180);
		double fSlow182 = (fSlow9 * fSlow73);
		double fSlow183 = (fSlow13 * fSlow167);
		double fSlow184 = (fSlow182 + fSlow183);
		double fSlow185 = (fSlow9 * fSlow78);
		double fSlow186 = (fSlow13 * fSlow168);
		double fSlow187 = (fSlow185 + fSlow186);
		double fSlow188 = (fSlow9 * fSlow81);
		double fSlow189 = (fSlow13 * fSlow169);
		double fSlow190 = (fSlow188 + fSlow189);
		double fSlow191 = (fSlow9 * fSlow84);
		double fSlow192 = (fSlow13 * fSlow170);
		double fSlow193 = (fSlow191 + fSlow192);
		double fSlow194 = (0.85391256382996661 * ((((fSlow171 + fSlow86) * fSlow6) * fSlow9) * fSlow12));
		double fSlow195 = (fSlow89 * fSlow173);
		double fSlow196 = (fSlow9 * fSlow91);
		double fSlow197 = (fSlow195 + fSlow196);
		double fSlow198 = (fSlow9 * fSlow94);
		double fSlow199 = (fSlow13 * fSlow175);
		double fSlow200 = (fSlow198 + fSlow199);
		double fSlow201 = (fSlow100 * fSlow9);
		double fSlow202 = ((fSlow165 * fSlow6) * fSlow12);
		double fSlow203 = ((fSlow20 * fSlow81) + (fSlow201 + fSlow202));
		double fSlow204 = (fSlow97 * fSlow9);
		double fSlow205 = ((fSlow161 * fSlow6) * fSlow12);
		double fSlow206 = ((fSlow20 * fSlow84) + (fSlow204 + fSlow205));
		double fSlow207 = (fSlow99 * fSlow9);
		double fSlow208 = ((fSlow164 * fSlow6) * fSlow12);
		double fSlow209 = ((0.66143782776614757 * (fSlow20 * fSlow94)) + (0.66143782776614768 * (fSlow207 + fSlow208)));
		double fSlow210 = (3.872983346207417 * fSlow44);
		double fSlow211 = (1.9364916731037085 * fSlow65);
		double fSlow212 = ((((((0.81009258730098255 * (fSlow210 + fSlow211)) + (0.60380736442455996 * fSlow86)) * fSlow6) * fSlow3) * fSlow11) * fSlow12);
		double fSlow213 = (((fSlow95 * fSlow1) * fSlow11) + ((fSlow96 * fSlow5) * fSlow12));
		double fSlow214 = ((fSlow159 * fSlow16) + (fSlow160 * fSlow10));
		double fSlow215 = ((0.17677669529663689 * ((fSlow22 * fSlow73) + (fSlow19 * fSlow78))) + (0.17677669529663687 * (fSlow213 + fSlow214)));
		double fSlow216 = (fSlow95 * fSlow9);
		double fSlow217 = ((fSlow159 * fSlow6) * fSlow12);
		double fSlow218 = ((0.5 * (fSlow20 * fSlow73)) + (0.49999999999999994 * (fSlow216 + fSlow217)));
		double fSlow219 = (fSlow98 * fSlow9);
		double fSlow220 = ((fSlow163 * fSlow6) * fSlow12);
		double fSlow221 = ((0.66143782776614757 * (fSlow20 * fSlow91)) + (0.66143782776614768 * (fSlow219 + fSlow220)));
		double fSlow222 = (fSlow96 * fSlow9);
		double fSlow223 = ((fSlow160 * fSlow6) * fSlow12);
		double fSlow224 = ((0.5 * (fSlow20 * fSlow78)) + (0.49999999999999994 * (fSlow222 + fSlow223)));
		double fSlow225 = (((fSlow96 * fSlow1) * fSlow11) - ((fSlow95 * fSlow5) * fSlow12));
		double fSlow226 = ((fSlow160 * fSlow16) - (fSlow159 * fSlow10));
		double fSlow227 = ((0.17677669529663689 * ((fSlow22 * fSlow78) - (fSlow19 * fSlow73))) + (0.17677669529663687 * (fSlow225 + fSlow226)));
		double fSlow228 = (((0.1889822365046136 * ((fSlow95 * fSlow22) + (fSlow96 * fSlow19))) + (0.36596252735569995 * ((fSlow158 * fSlow10) + ((((fSlow102 * fSlow1) * fSlow11) + ((fSlow110 * fSlow5) * fSlow12)) + (fSlow138 * fSlow16))))) - (0.0472455591261534 * (fSlow176 - fSlow177)));
		double fSlow229 = (((0.53452248382484868 * ((fSlow95 * fSlow3) * fSlow11)) + (1.0350983390135313 * ((fSlow102 * fSlow9) + ((fSlow138 * fSlow6) * fSlow12)))) - (0.1336306209562122 * (fSlow182 - fSlow183)));
		double fSlow230 = ((((fSlow100 * fSlow3) * fSlow11) + (0.79056941504209488 * ((fSlow103 * fSlow9) + ((fSlow141 * fSlow6) * fSlow12)))) - (0.24999999999999997 * (fSlow188 - fSlow189)));
		double fSlow231 = ((0.70710678118654757 * (((fSlow98 * fSlow3) * fSlow11) + ((fSlow104 * fSlow9) + ((fSlow144 * fSlow6) * fSlow12)))) - (0.18257418583505536 * (0.0 - (0.96824583655185426 * (fSlow195 - fSlow196)))));
		double fSlow232 = ((((((0.68465319688145765 * (fSlow146 + fSlow106)) + (3.3541019662496843 * fSlow43)) - (0.16137430609197573 * (fSlow86 - fSlow171))) * fSlow6) * fSlow9) * fSlow12);
		double fSlow233 = ((0.70710678118654757 * (((fSlow99 * fSlow3) * fSlow11) + ((fSlow108 * fSlow9) + ((fSlow150 * fSlow6) * fSlow12)))) - (0.17677669529663687 * (fSlow198 - fSlow199)));
		double fSlow234 = ((((fSlow97 * fSlow3) * fSlow11) + (0.79056941504209488 * ((fSlow109 * fSlow9) + ((fSlow153 * fSlow6) * fSlow12)))) - (0.24999999999999997 * (fSlow191 - fSlow192)));
		double fSlow235 = (((0.53452248382484868 * ((fSlow96 * fSlow3) * fSlow11)) + (1.0350983390135313 * ((fSlow110 * fSlow9) + ((fSlow158 * fSlow6) * fSlow12)))) - (0.1336306209562122 * (fSlow185 - fSlow186)));
		double fSlow236 = (((0.1889822365046136 * ((fSlow96 * fSlow22) - (fSlow95 * fSlow19))) + (0.36596252735569995 * ((((fSlow110 * fSlow1) * fSlow11) + (fSlow158 * fSlow16)) - (((fSlow102 * fSlow5) * fSlow12) + (fSlow138 * fSlow10))))) - (0.0472455591261534 * (fSlow179 - fSlow180)));
		double fSlow237 = (((0.51754916950676566 * ((fSlow102 * fSlow22) + (fSlow110 * fSlow19))) + (0.42257712736425829 * ((fSlow114 * fSlow16) + (fSlow133 * fSlow10)))) - (0.066815310478106085 * (fSlow213 - fSlow214)));
		double fSlow238 = (((1.4638501094227998 * ((fSlow102 * fSlow3) * fSlow11)) + (1.1952286093343938 * ((fSlow114 * fSlow6) * fSlow12))) - (0.1889822365046136 * (fSlow216 - fSlow217)));
		double fSlow239 = (((1.1180339887498949 * ((fSlow103 * fSlow3) * fSlow11)) + (0.9128709291752769 * ((fSlow117 * fSlow6) * fSlow12))) - (0.35355339059327379 * (fSlow201 - fSlow202)));
		double fSlow240 = ((((fSlow104 * fSlow3) * fSlow11) + (0.81649658092772603 * ((fSlow120 * fSlow6) * fSlow12))) - (0.25 * (fSlow219 - fSlow220)));
		double fSlow241 = (0.79056941504209488 * fSlow121);
		double fSlow242 = (((((((0.96824583655185426 * fSlow106) + fSlow241) - (0.30618621784789724 * (fSlow210 - fSlow211))) * fSlow6) * fSlow3) * fSlow11) * fSlow12);
		double fSlow243 = ((((fSlow108 * fSlow3) * fSlow11) + (0.81649658092772603 * ((fSlow125 * fSlow6) * fSlow12))) - (0.25 * (fSlow207 - fSlow208)));
		double fSlow244 = (((1.1180339887498949 * ((fSlow109 * fSlow3) * fSlow11)) + (0.9128709291752769 * ((fSlow128 * fSlow6) * fSlow12))) - (0.35355339059327379 * (fSlow204 - fSlow205)));
		double fSlow245 = (((1.4638501094227998 * ((fSlow110 * fSlow3) * fSlow11)) + (1.1952286093343938 * ((fSlow133 * fSlow6) * fSlow12))) - (0.1889822365046136 * (fSlow222 - fSlow223)));
		double fSlow246 = (((0.51754916950676566 * ((fSlow110 * fSlow22) - (fSlow102 * fSlow19))) + (0.42257712736425829 * ((fSlow133 * fSlow16) - (fSlow114 * fSlow10)))) - (0.066815310478106085 * (fSlow225 - fSlow226)));
		double fSlow247 = (((fSlow138 * fSlow1) * fSlow11) + ((fSlow158 * fSlow5) * fSlow12));
		double fSlow248 = (fSlow102 * fSlow16);
		double fSlow249 = (fSlow110 * fSlow10);
		double fSlow250 = ((0.53452248382484879 * ((fSlow114 * fSlow22) + (fSlow133 * fSlow19))) - (0.32732683535398854 * ((fSlow247 + fSlow248) + fSlow249)));
		double fSlow251 = (fSlow138 * fSlow9);
		double fSlow252 = ((fSlow102 * fSlow6) * fSlow12);
		double fSlow253 = ((1.5118578920369088 * ((fSlow114 * fSlow3) * fSlow11)) - (0.92582009977255142 * (fSlow251 + fSlow252)));
		double fSlow254 = (fSlow141 * fSlow9);
		double fSlow255 = ((fSlow103 * fSlow6) * fSlow12);
		double fSlow256 = ((1.1547005383792515 * ((fSlow117 * fSlow3) * fSlow11)) - (0.70710678118654757 * (fSlow254 + fSlow255)));
		double fSlow257 = (fSlow144 * fSlow9);
		double fSlow258 = ((fSlow104 * fSlow6) * fSlow12);
		double fSlow259 = ((1.0327955589886444 * ((fSlow120 * fSlow3) * fSlow11)) - (0.63245553203367588 * (fSlow257 + fSlow258)));
		double fSlow260 = (fSlow146 * fSlow44);
		double fSlow261 = ((fSlow106 * fSlow45) * fSlow26);
		double fSlow262 = (((fSlow121 * fSlow41) * fSlow42) - (0.61237243569579447 * (fSlow260 + fSlow261)));
		double fSlow263 = (fSlow150 * fSlow9);
		double fSlow264 = ((fSlow108 * fSlow6) * fSlow12);
		double fSlow265 = ((1.0327955589886444 * ((fSlow125 * fSlow3) * fSlow11)) - (0.63245553203367588 * (fSlow263 + fSlow264)));
		double fSlow266 = (fSlow153 * fSlow9);
		double fSlow267 = ((fSlow109 * fSlow6) * fSlow12);
		double fSlow268 = ((1.1547005383792515 * ((fSlow128 * fSlow3) * fSlow11)) - (0.70710678118654757 * (fSlow266 + fSlow267)));
		double fSlow269 = (fSlow158 * fSlow9);
		double fSlow270 = ((fSlow110 * fSlow6) * fSlow12);
		double fSlow271 = ((1.5118578920369088 * ((fSlow133 * fSlow3) * fSlow11)) - (0.92582009977255142 * (fSlow269 + fSlow270)));
		double fSlow272 = ((fSlow158 * fSlow1) * fSlow11);
		double fSlow273 = (fSlow110 * fSlow16);
		double fSlow274 = ((fSlow138 * fSlow5) * fSlow12);
		double fSlow275 = (fSlow102 * fSlow10);
		double fSlow276 = ((0.53452248382484879 * ((fSlow133 * fSlow22) - (fSlow114 * fSlow19))) - (0.32732683535398854 * ((fSlow272 + fSlow273) - (fSlow274 + fSlow275))));
		double fSlow277 = (((fSlow159 * fSlow1) * fSlow11) + ((fSlow160 * fSlow5) * fSlow12));
		double fSlow278 = ((fSlow95 * fSlow16) + (fSlow96 * fSlow10));
		double fSlow279 = (((0.51754916950676566 * ((fSlow138 * fSlow22) + (fSlow158 * fSlow19))) + (0.42257712736425829 * (((fSlow114 * fSlow1) * fSlow11) + ((fSlow133 * fSlow5) * fSlow12)))) - (0.066815310478106085 * (fSlow277 + fSlow278)));
		double fSlow280 = (fSlow159 * fSlow9);
		double fSlow281 = ((fSlow95 * fSlow6) * fSlow12);
		double fSlow282 = (((1.4638501094227998 * ((fSlow138 * fSlow3) * fSlow11)) + (1.1952286093343938 * (fSlow114 * fSlow9))) - (0.1889822365046136 * (fSlow280 + fSlow281)));
		double fSlow283 = (fSlow165 * fSlow9);
		double fSlow284 = ((fSlow100 * fSlow6) * fSlow12);
		double fSlow285 = (((1.1180339887498949 * ((fSlow141 * fSlow3) * fSlow11)) + (0.9128709291752769 * (fSlow117 * fSlow9))) - (0.35355339059327379 * (fSlow283 + fSlow284)));
		double fSlow286 = (fSlow163 * fSlow9);
		double fSlow287 = ((fSlow98 * fSlow6) * fSlow12);
		double fSlow288 = ((((fSlow144 * fSlow3) * fSlow11) + (0.81649658092772603 * (fSlow120 * fSlow9))) - (0.25 * (fSlow286 + fSlow287)));
		double fSlow289 = (3.872983346207417 * fSlow46);
		double fSlow290 = (((((fSlow241 + (0.96824583655185426 * fSlow146)) - (0.30618621784789724 * (fSlow289 + fSlow211))) * fSlow9) * fSlow3) * fSlow11);
		double fSlow291 = (fSlow164 * fSlow9);
		double fSlow292 = ((fSlow99 * fSlow6) * fSlow12);
		double fSlow293 = ((((fSlow150 * fSlow3) * fSlow11) + (0.81649658092772603 * (fSlow125 * fSlow9))) - (0.25 * (fSlow291 + fSlow292)));
		double fSlow294 = (fSlow161 * fSlow9);
		double fSlow295 = ((fSlow97 * fSlow6) * fSlow12);
		double fSlow296 = (((1.1180339887498949 * ((fSlow153 * fSlow3) * fSlow11)) + (0.9128709291752769 * (fSlow128 * fSlow9))) - (0.35355339059327379 * (fSlow294 + fSlow295)));
		double fSlow297 = (fSlow160 * fSlow9);
		double fSlow298 = ((fSlow96 * fSlow6) * fSlow12);
		double fSlow299 = (((1.4638501094227998 * ((fSlow158 * fSlow3) * fSlow11)) + (1.1952286093343938 * (fSlow133 * fSlow9))) - (0.1889822365046136 * (fSlow297 + fSlow298)));
		double fSlow300 = (((fSlow160 * fSlow1) * fSlow11) - ((fSlow159 * fSlow5) * fSlow12));
		double fSlow301 = ((fSlow96 * fSlow16) - (fSlow95 * fSlow10));
		double fSlow302 = (((0.51754916950676566 * ((fSlow158 * fSlow22) - (fSlow138 * fSlow19))) + (0.42257712736425829 * (((fSlow133 * fSlow1) * fSlow11) - ((fSlow114 * fSlow5) * fSlow12)))) - (0.066815310478106085 * (fSlow300 + fSlow301)));
		double fSlow303 = ((fSlow24 * fSlow167) + (fSlow23 * fSlow168));
		double fSlow304 = ((fSlow10 * fSlow78) + (fSlow16 * fSlow73));
		double fSlow305 = (((0.1889822365046136 * ((fSlow159 * fSlow22) + (fSlow160 * fSlow19))) + (0.36596252735569995 * (fSlow247 - (fSlow248 + fSlow249)))) - (0.0472455591261534 * (fSlow303 + fSlow304)));
		double fSlow306 = (fSlow9 * fSlow167);
		double fSlow307 = (fSlow13 * fSlow73);
		double fSlow308 = (((0.53452248382484868 * ((fSlow159 * fSlow3) * fSlow11)) + (1.0350983390135313 * (fSlow251 - fSlow252))) - (0.1336306209562122 * (fSlow306 + fSlow307)));
		double fSlow309 = (fSlow9 * fSlow169);
		double fSlow310 = (fSlow13 * fSlow81);
		double fSlow311 = ((((fSlow165 * fSlow3) * fSlow11) + (0.79056941504209488 * (fSlow254 - fSlow255))) - (0.24999999999999997 * (fSlow309 + fSlow310)));
		double fSlow312 = (fSlow9 * fSlow173);
		double fSlow313 = (fSlow6 * fSlow91);
		double fSlow314 = ((0.70710678118654757 * (((fSlow163 * fSlow3) * fSlow11) + (fSlow257 - fSlow258))) - (0.17677669529663687 * ((fSlow312 + fSlow313) * fSlow12)));
		double fSlow315 = (fSlow44 * fSlow171);
		double fSlow316 = (fSlow46 * fSlow86);
		double fSlow317 = (((1.6770509831248421 * (fSlow43 * fSlow65)) + (0.68465319688145765 * (fSlow260 - fSlow261))) - (0.16137430609197573 * (fSlow315 + fSlow316)));
		double fSlow318 = (fSlow9 * fSlow175);
		double fSlow319 = (fSlow13 * fSlow94);
		double fSlow320 = ((0.70710678118654757 * (((fSlow164 * fSlow3) * fSlow11) + (fSlow263 - fSlow264))) - (0.17677669529663687 * (fSlow318 + fSlow319)));
		double fSlow321 = (fSlow9 * fSlow170);
		double fSlow322 = (fSlow13 * fSlow84);
		double fSlow323 = ((((fSlow161 * fSlow3) * fSlow11) + (0.79056941504209488 * (fSlow266 - fSlow267))) - (0.24999999999999997 * (fSlow321 + fSlow322)));
		double fSlow324 = (fSlow9 * fSlow168);
		double fSlow325 = (fSlow13 * fSlow78);
		double fSlow326 = (((0.53452248382484868 * ((fSlow160 * fSlow3) * fSlow11)) + (1.0350983390135313 * (fSlow269 - fSlow270))) - (0.1336306209562122 * (fSlow324 + fSlow325)));
		double fSlow327 = ((fSlow24 * fSlow168) - (fSlow23 * fSlow167));
		double fSlow328 = ((fSlow16 * fSlow78) - (fSlow10 * fSlow73));
		double fSlow329 = (((0.1889822365046136 * ((fSlow160 * fSlow22) - (fSlow159 * fSlow19))) + (0.36596252735569995 * ((fSlow272 + fSlow275) - (fSlow274 + fSlow273)))) - (0.0472455591261534 * (fSlow327 + fSlow328)));
		double fSlow330 = ((fSlow20 * fSlow169) + (fSlow283 - fSlow284));
		double fSlow331 = ((fSlow20 * fSlow170) + (fSlow294 - fSlow295));
		double fSlow332 = ((0.66143782776614757 * (fSlow20 * fSlow175)) + (0.66143782776614768 * (fSlow291 - fSlow292)));
		double fSlow333 = (((((0.81009258730098255 * (fSlow211 - fSlow289)) + (0.60380736442455996 * fSlow171)) * fSlow9) * fSlow3) * fSlow11);
		double fSlow334 = ((0.17677669529663689 * ((fSlow22 * fSlow167) + (fSlow19 * fSlow168))) + (0.17677669529663687 * (fSlow277 - fSlow278)));
		double fSlow335 = ((0.5 * (fSlow20 * fSlow167)) + (0.49999999999999994 * (fSlow280 - fSlow281)));
		double fSlow336 = ((0.66143782776614757 * (fSlow162 * fSlow173)) + (0.66143782776614768 * (fSlow286 - fSlow287)));
		double fSlow337 = ((0.5 * (fSlow20 * fSlow168)) + (0.49999999999999994 * (fSlow297 - fSlow298)));
		double fSlow338 = ((0.17677669529663689 * ((fSlow22 * fSlow168) - (fSlow19 * fSlow167))) + (0.17677669529663687 * (fSlow300 - fSlow301)));
		double fSlow339 = (fSlow303 - fSlow304);
		double fSlow340 = (fSlow327 - fSlow328);
		double fSlow341 = (fSlow306 - fSlow307);
		double fSlow342 = (fSlow324 - fSlow325);
		double fSlow343 = (fSlow309 - fSlow310);
		double fSlow344 = (fSlow321 - fSlow322);
		double fSlow345 = (0.85391256382996661 * (fSlow315 - fSlow316));
		double fSlow346 = (fSlow12 * (fSlow312 - fSlow313));
		double fSlow347 = (fSlow318 - fSlow319);
		for (int i = 0; (i < count); i = (i + 1)) {
			output0[i] = FAUSTFLOAT(double(input0[i]));
			double fTemp0 = double(input1[i]);
			double fTemp1 = double(input2[i]);
			double fTemp2 = double(input3[i]);
			output1[i] = FAUSTFLOAT((((fSlow10 * fTemp0) + (fSlow13 * fTemp1)) + (fSlow16 * fTemp2)));
			output2[i] = FAUSTFLOAT((((fSlow19 * fTemp0) + (fSlow20 * fTemp1)) + (fSlow22 * fTemp2)));
			output3[i] = FAUSTFLOAT((((fSlow23 * fTemp0) + (fSlow9 * fTemp1)) + (fSlow24 * fTemp2)));
			double fTemp3 = double(input4[i]);
			double fTemp4 = double(input5[i]);
			double fTemp5 = double(input6[i]);
			double fTemp6 = double(input7[i]);
			double fTemp7 = double(input8[i]);
			output4[i] = FAUSTFLOAT((((((fSlow25 * fTemp3) + (fSlow27 * fTemp4)) + (fSlow28 * fTemp5)) + (fSlow29 * fTemp6)) + (fSlow30 * fTemp7)));
			output5[i] = FAUSTFLOAT((((((fSlow31 * fTemp3) + (fSlow32 * fTemp4)) + (fSlow33 * fTemp5)) + (fSlow34 * fTemp6)) + (fSlow35 * fTemp7)));
			output6[i] = FAUSTFLOAT((((((fSlow38 * fTemp3) + (fSlow40 * fTemp4)) + (fSlow48 * fTemp5)) + (fSlow51 * fTemp6)) + (fSlow56 * fTemp7)));
			output7[i] = FAUSTFLOAT((((((fSlow57 * fTemp3) + (fSlow58 * fTemp4)) + (fSlow59 * fTemp5)) + (fSlow60 * fTemp6)) + (fSlow61 * fTemp7)));
			output8[i] = FAUSTFLOAT((((((fSlow62 * fTemp3) + (fSlow64 * fTemp4)) + (fSlow66 * fTemp5)) + (fSlow67 * fTemp6)) + (fSlow69 * fTemp7)));
			double fTemp8 = double(input9[i]);
			double fTemp9 = double(input15[i]);
			double fTemp10 = double(input10[i]);
			double fTemp11 = double(input14[i]);
			double fTemp12 = double(input12[i]);
			double fTemp13 = double(input11[i]);
			double fTemp14 = double(input13[i]);
			output9[i] = FAUSTFLOAT(((0.5 * ((fSlow73 * fTemp8) + (fSlow78 * fTemp9))) + ((1.2247448713915889 * ((fSlow81 * fTemp10) + (fSlow84 * fTemp11))) + ((fSlow87 * fTemp12) + (0.96824583655185426 * ((fSlow91 * fTemp13) + (fSlow94 * fTemp14)))))));
			output10[i] = FAUSTFLOAT(((0.40824829046386302 * ((fSlow95 * fTemp8) + (fSlow96 * fTemp9))) + ((fSlow97 * fTemp11) + ((0.79056941504209488 * ((fSlow98 * fTemp13) + (fSlow99 * fTemp14))) + ((fSlow100 * fTemp10) + (fSlow101 * fTemp12))))));
			output11[i] = FAUSTFLOAT((((((((fSlow102 * fTemp8) + (fSlow103 * fTemp10)) + (fSlow104 * fTemp13)) + (fSlow107 * fTemp12)) + (fSlow108 * fTemp14)) + (fSlow109 * fTemp11)) + (fSlow110 * fTemp9)));
			output12[i] = FAUSTFLOAT((((((((fSlow114 * fTemp8) + (fSlow117 * fTemp10)) + (fSlow120 * fTemp13)) + (fSlow122 * fTemp12)) + (fSlow125 * fTemp14)) + (fSlow128 * fTemp11)) + (fSlow133 * fTemp9)));
			output13[i] = FAUSTFLOAT((((((((fSlow138 * fTemp8) + (fSlow141 * fTemp10)) + (fSlow144 * fTemp13)) + (fSlow147 * fTemp12)) + (fSlow150 * fTemp14)) + (fSlow153 * fTemp11)) + (fSlow158 * fTemp9)));
			output14[i] = FAUSTFLOAT(((0.40824829046386302 * ((fSlow159 * fTemp8) + (fSlow160 * fTemp9))) + ((fSlow161 * fTemp11) + ((0.79056941504209488 * ((fSlow163 * fTemp13) + (fSlow164 * fTemp14))) + ((fSlow165 * fTemp10) + (fSlow166 * fTemp12))))));
			output15[i] = FAUSTFLOAT(((0.5 * ((fSlow167 * fTemp8) + (fSlow168 * fTemp9))) + ((1.2247448713915889 * ((fSlow169 * fTemp10) + (fSlow170 * fTemp11))) + ((fSlow172 * fTemp12) + (0.96824583655185426 * ((fSlow174 * fTemp13) + (fSlow175 * fTemp14)))))));
			double fTemp15 = double(input16[i]);
			double fTemp16 = double(input24[i]);
			double fTemp17 = double(input17[i]);
			double fTemp18 = double(input23[i]);
			double fTemp19 = double(input18[i]);
			double fTemp20 = double(input22[i]);
			double fTemp21 = double(input20[i]);
			double fTemp22 = double(input19[i]);
			double fTemp23 = double(input21[i]);
			output16[i] = FAUSTFLOAT(((0.25 * ((fSlow178 * fTemp15) + (fSlow181 * fTemp16))) + ((0.70710678118654757 * ((fSlow184 * fTemp17) + (fSlow187 * fTemp18))) + ((1.3228756555322954 * ((fSlow190 * fTemp19) + (fSlow193 * fTemp20))) + ((fSlow194 * fTemp21) + (0.93541434669348533 * ((fSlow197 * fTemp22) + (fSlow200 * fTemp23))))))));
			output17[i] = FAUSTFLOAT(((((0.93541434669348533 * ((fSlow203 * fTemp19) + (fSlow206 * fTemp20))) + ((fSlow209 * fTemp23) + ((fSlow212 * fTemp21) + (((fSlow215 * fTemp15) + (fSlow218 * fTemp17)) + (fSlow221 * fTemp22))))) + (fSlow224 * fTemp18)) + (fSlow227 * fTemp16)));
			output18[i] = FAUSTFLOAT((((((((((fSlow228 * fTemp15) + (fSlow229 * fTemp17)) + (fSlow230 * fTemp19)) + (fSlow231 * fTemp22)) + (fSlow232 * fTemp21)) + (fSlow233 * fTemp23)) + (fSlow234 * fTemp20)) + (fSlow235 * fTemp18)) + (fSlow236 * fTemp16)));
			output19[i] = FAUSTFLOAT((((((((((fSlow237 * fTemp15) + (fSlow238 * fTemp17)) + (fSlow239 * fTemp19)) + (fSlow240 * fTemp22)) + (fSlow242 * fTemp21)) + (fSlow243 * fTemp23)) + (fSlow244 * fTemp20)) + (fSlow245 * fTemp18)) + (fSlow246 * fTemp16)));
			output20[i] = FAUSTFLOAT((((((((((fSlow250 * fTemp15) + (fSlow253 * fTemp17)) + (fSlow256 * fTemp19)) + (fSlow259 * fTemp22)) + (fSlow262 * fTemp21)) + (fSlow265 * fTemp23)) + (fSlow268 * fTemp20)) + (fSlow271 * fTemp18)) + (fSlow276 * fTemp16)));
			output21[i] = FAUSTFLOAT((((((((((fSlow279 * fTemp15) + (fSlow282 * fTemp17)) + (fSlow285 * fTemp19)) + (fSlow288 * fTemp22)) + (fSlow290 * fTemp21)) + (fSlow293 * fTemp23)) + (fSlow296 * fTemp20)) + (fSlow299 * fTemp18)) + (fSlow302 * fTemp16)));
			output22[i] = FAUSTFLOAT((((((((((fSlow305 * fTemp15) + (fSlow308 * fTemp17)) + (fSlow311 * fTemp19)) + (fSlow314 * fTemp22)) + (fSlow317 * fTemp21)) + (fSlow320 * fTemp23)) + (fSlow323 * fTemp20)) + (fSlow326 * fTemp18)) + (fSlow329 * fTemp16)));
			output23[i] = FAUSTFLOAT(((((0.93541434669348533 * ((fSlow330 * fTemp19) + (fSlow331 * fTemp20))) + ((fSlow332 * fTemp23) + ((fSlow333 * fTemp21) + (((fSlow334 * fTemp15) + (fSlow335 * fTemp17)) + (fSlow336 * fTemp22))))) + (fSlow337 * fTemp18)) + (fSlow338 * fTemp16)));
			output24[i] = FAUSTFLOAT(((0.25 * ((fSlow339 * fTemp15) + (fSlow340 * fTemp16))) + ((0.70710678118654757 * ((fSlow341 * fTemp17) + (fSlow342 * fTemp18))) + ((1.3228756555322954 * ((fSlow343 * fTemp19) + (fSlow344 * fTemp20))) + ((fSlow345 * fTemp21) + (0.93541434669348533 * ((fSlow346 * fTemp22) + (fSlow347 * fTemp23))))))));
			
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
