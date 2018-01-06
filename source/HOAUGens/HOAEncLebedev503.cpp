/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAEncLebedev503"
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


double pow(double dummy0, double dummy1);

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fHslider0;
	double fRec0[2];
	int fSamplingFreq;
	
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
		m->declare("name", "HOAEncLebedev503");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 50;
		
	}
	virtual int getNumOutputs() {
		return 16;
		
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
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec0[l0] = 0.0;
			
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
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("Outputs");
		ui_interface->declare(&fHslider0, "1", "");
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->declare(&fHslider0, "unit", "dB");
		ui_interface->addHorizontalSlider("Gain", &fHslider0, 0.0, -10.0, 50.0, 0.10000000000000001);
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
		double fSlow0 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fHslider0))));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec0[0] = (fSlow0 + (0.999 * fRec0[1]));
			double fTemp0 = double(input0[i]);
			double fTemp1 = double(input1[i]);
			double fTemp2 = double(input2[i]);
			double fTemp3 = double(input3[i]);
			double fTemp4 = double(input4[i]);
			double fTemp5 = double(input5[i]);
			double fTemp6 = double(input6[i]);
			double fTemp7 = double(input7[i]);
			double fTemp8 = double(input8[i]);
			double fTemp9 = double(input9[i]);
			double fTemp10 = (((fTemp6 + fTemp7) + fTemp8) + fTemp9);
			double fTemp11 = double(input10[i]);
			double fTemp12 = double(input11[i]);
			double fTemp13 = double(input12[i]);
			double fTemp14 = double(input13[i]);
			double fTemp15 = double(input14[i]);
			double fTemp16 = double(input15[i]);
			double fTemp17 = double(input16[i]);
			double fTemp18 = double(input17[i]);
			double fTemp19 = double(input18[i]);
			double fTemp20 = double(input19[i]);
			double fTemp21 = double(input20[i]);
			double fTemp22 = double(input21[i]);
			double fTemp23 = (((fTemp19 + fTemp20) + fTemp21) + fTemp22);
			double fTemp24 = double(input22[i]);
			double fTemp25 = double(input23[i]);
			double fTemp26 = double(input24[i]);
			double fTemp27 = double(input25[i]);
			double fTemp28 = ((((fTemp23 + fTemp24) + fTemp25) + fTemp26) + fTemp27);
			double fTemp29 = double(input26[i]);
			double fTemp30 = double(input27[i]);
			double fTemp31 = (fTemp29 + fTemp30);
			double fTemp32 = double(input28[i]);
			double fTemp33 = double(input29[i]);
			double fTemp34 = ((fTemp31 + fTemp32) + fTemp33);
			double fTemp35 = double(input30[i]);
			double fTemp36 = double(input31[i]);
			double fTemp37 = double(input32[i]);
			double fTemp38 = double(input33[i]);
			double fTemp39 = double(input34[i]);
			double fTemp40 = double(input35[i]);
			double fTemp41 = double(input36[i]);
			double fTemp42 = double(input37[i]);
			double fTemp43 = double(input38[i]);
			double fTemp44 = double(input39[i]);
			double fTemp45 = double(input40[i]);
			double fTemp46 = double(input41[i]);
			double fTemp47 = double(input42[i]);
			double fTemp48 = double(input43[i]);
			double fTemp49 = double(input44[i]);
			double fTemp50 = double(input45[i]);
			double fTemp51 = double(input46[i]);
			double fTemp52 = double(input47[i]);
			double fTemp53 = double(input48[i]);
			double fTemp54 = double(input49[i]);
			output0[i] = FAUSTFLOAT((fRec0[0] * ((((0.012698412698412698 * (((((fTemp0 + fTemp1) + fTemp2) + fTemp3) + fTemp4) + fTemp5)) + (0.022574955908289243 * ((((((((fTemp10 + fTemp11) + fTemp12) + fTemp13) + fTemp14) + fTemp15) + fTemp16) + fTemp17) + fTemp18))) + (0.021093750000000001 * fTemp28)) + (0.02017333553791887 * ((((((((((((((((((((fTemp34 + fTemp35) + fTemp36) + fTemp37) + fTemp38) + fTemp39) + fTemp40) + fTemp41) + fTemp42) + fTemp43) + fTemp44) + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) + fTemp50) + fTemp51) + fTemp52) + fTemp53) + fTemp54)))));
			double fTemp55 = (fTemp36 + fTemp37);
			double fTemp56 = (fTemp38 + fTemp46);
			double fTemp57 = (fTemp19 - fTemp21);
			double fTemp58 = ((fTemp57 + fTemp24) - fTemp26);
			double fTemp59 = (fTemp7 - fTemp9);
			double fTemp60 = (fTemp2 - fTemp4);
			double fTemp61 = (fTemp11 - fTemp13);
			double fTemp62 = (fTemp8 + fTemp17);
			double fTemp63 = (fTemp20 + fTemp25);
			double fTemp64 = (fTemp35 + fTemp43);
			double fTemp65 = (fTemp31 - fTemp32);
			double fTemp66 = (fTemp22 + fTemp27);
			double fTemp67 = (fTemp39 + fTemp47);
			double fTemp68 = (fTemp42 + fTemp50);
			double fTemp69 = (fTemp33 + fTemp54);
			output1[i] = FAUSTFLOAT((fRec0[0] * (((2.6935244158092871e-18 * fTemp3) + (((0.031605542669238242 * ((((((fTemp55 - fTemp40) - fTemp41) + fTemp44) + fTemp45) - fTemp48) - fTemp49)) + ((0.010535180889746096 * fTemp56) + (((0.021093749999999994 * fTemp58) + (((0.027648561470568506 * ((((fTemp59 + fTemp12) - fTemp14) + fTemp16) - fTemp18)) + (((0.021994295969128601 * fTemp60) + (0.027648561470568499 * fTemp61)) + (3.3859722305960555e-18 * fTemp62))) + (0.021093749999999998 * fTemp63))) + (0.010535180889746094 * fTemp64)))) + (0.010535180889746065 * (((fTemp65 + fTemp51) + fTemp52) - fTemp53)))) - ((((0.021093750000000001 * fTemp66) + (0.010535180889746089 * fTemp67)) + (0.010535180889746113 * fTemp68)) + (0.010535180889746067 * fTemp69)))));
			double fTemp70 = (fTemp0 - fTemp5);
			double fTemp71 = ((((fTemp10 - fTemp15) - fTemp16) - fTemp17) - fTemp18);
			double fTemp72 = ((((fTemp23 - fTemp24) - fTemp25) - fTemp26) - fTemp27);
			double fTemp73 = (((((((fTemp35 + fTemp36) + fTemp37) + fTemp38) + fTemp39) + fTemp40) + fTemp41) + fTemp42);
			double fTemp74 = ((((((((fTemp73 - fTemp43) - fTemp44) - fTemp45) - fTemp46) - fTemp47) - fTemp48) - fTemp49) - fTemp50);
			double fTemp75 = ((((fTemp34 - fTemp51) - fTemp52) - fTemp53) - fTemp54);
			output2[i] = FAUSTFLOAT((fRec0[0] * (((((0.021994295969128601 * fTemp70) + (0.027648561470568499 * fTemp71)) + (0.021093750000000001 * fTemp72)) + (0.010535180889746075 * fTemp74)) + (0.031605542669238249 * fTemp75))));
			double fTemp76 = (fTemp29 - fTemp30);
			double fTemp77 = ((fTemp76 + fTemp51) - fTemp52);
			double fTemp78 = (fTemp41 + fTemp49);
			double fTemp79 = (fTemp35 - fTemp38);
			double fTemp80 = ((((fTemp79 - fTemp39) + fTemp43) - fTemp46) - fTemp47);
			double fTemp81 = (fTemp36 + fTemp44);
			double fTemp82 = (fTemp19 + fTemp24);
			double fTemp83 = (fTemp6 - fTemp8);
			double fTemp84 = (fTemp7 + fTemp16);
			double fTemp85 = (fTemp1 - fTemp3);
			double fTemp86 = (fTemp9 + fTemp18);
			double fTemp87 = (fTemp21 + fTemp26);
			double fTemp88 = (fTemp37 + fTemp45);
			double fTemp89 = (fTemp40 + fTemp48);
			double fTemp90 = (fTemp32 + fTemp53);
			output3[i] = FAUSTFLOAT((fRec0[0] * (((0.027648561470568496 * fTemp14) + ((0.010535180889746065 * fTemp77) + ((0.010535180889746074 * fTemp78) + ((0.031605542669238242 * fTemp80) + ((0.010535180889746081 * fTemp81) + ((0.021093749999999998 * fTemp82) + ((0.027648561470568506 * ((((fTemp83 + fTemp11) - fTemp13) + fTemp15) - fTemp17)) + ((1.6929861152980278e-18 * fTemp84) + ((1.3467622079046435e-18 * ((fTemp0 + fTemp2) + fTemp5)) + ((0.021994295969128601 * fTemp85) + (((0.021093749999999991 * fTemp66) + (0.031605542669238228 * fTemp68)) + (0.010535180889746063 * fTemp69)))))))))))) - ((0.027648561470568499 * fTemp12) + ((4.0402866237139308e-18 * fTemp4) + ((((((5.0789583458940831e-18 * fTemp86) + (0.021093749999999994 * fTemp63)) + (0.021093750000000001 * fTemp87)) + (0.010535180889746079 * fTemp88)) + (0.010535180889746086 * fTemp89)) + (0.010535180889746067 * fTemp90)))))));
			double fTemp91 = (((fTemp19 - fTemp20) + fTemp21) - fTemp22);
			double fTemp92 = ((fTemp76 + fTemp32) - fTemp33);
			output4[i] = FAUSTFLOAT((fRec0[0] * (((9.0343555392074636e-18 * fTemp4) + ((3.0114518464024883e-18 * fTemp2) + (((0.027231914153020897 * ((((fTemp91 + fTemp24) - fTemp25) + fTemp26) - fTemp27)) + ((0.043716214137085485 * (((fTemp11 - fTemp12) + fTemp13) - fTemp14)) + (((0.021308452520676556 * fTemp81) + ((8.0305382570733035e-18 * fTemp86) + ((2.6768460856911014e-18 * fTemp84) + ((0.021308452520676577 * fTemp64) + (0.02130845252067657 * fTemp67))))) + (0.021308452520676563 * fTemp89)))) + (0.0071028175068921654 * ((((fTemp92 + fTemp51) - fTemp52) + fTemp53) - fTemp54))))) - ((6.0229036928049765e-18 * fTemp3) + ((0.021308452520676542 * fTemp78) + ((0.021308452520676546 * fTemp88) + (((5.3536921713822029e-18 * fTemp62) + (0.021308452520676584 * fTemp56)) + (0.021308452520676618 * fTemp68))))))));
			double fTemp93 = (fTemp37 - fTemp41);
			double fTemp94 = (fTemp36 - fTemp40);
			double fTemp95 = (fTemp38 - fTemp46);
			double fTemp96 = (fTemp20 - fTemp22);
			double fTemp97 = (fTemp8 - fTemp17);
			double fTemp98 = ((fTemp57 - fTemp24) + fTemp26);
			double fTemp99 = (fTemp35 - fTemp43);
			double fTemp100 = (fTemp39 - fTemp47);
			double fTemp101 = (fTemp42 - fTemp50);
			double fTemp102 = (fTemp33 - fTemp54);
			output5[i] = FAUSTFLOAT((fRec0[0] * ((((0.021308452520676553 * ((fTemp93 - fTemp45) + fTemp49)) + ((0.021308452520676546 * ((fTemp94 - fTemp44) + fTemp48)) + ((0.0071028175068921931 * fTemp95) + (((0.027231914153020911 * ((fTemp96 - fTemp25) + fTemp27)) + (((5.3536921713822013e-18 * fTemp97) + (0.043716214137085485 * ((fTemp59 - fTemp16) + fTemp18))) + (0.027231914153020904 * fTemp98))) + (0.0071028175068921914 * fTemp99))))) + (0.021308452520676528 * (((fTemp65 - fTemp51) - fTemp52) + fTemp53))) - (((0.0071028175068921871 * fTemp100) + (0.0071028175068922053 * fTemp101)) + (0.021308452520676532 * fTemp102)))));
			double fTemp103 = (fTemp0 + fTemp5);
			output6[i] = FAUSTFLOAT((fRec0[0] * ((((0.012619783999998805 * ((((fTemp10 + fTemp15) + fTemp16) + fTemp17) + fTemp18)) + ((0.028394513999997331 * fTemp103) + (5.236595479505967e-18 * fTemp28))) + (0.032806508796871948 * ((((fTemp34 + fTemp51) + fTemp52) + fTemp53) + fTemp54))) - (((0.014197256999998666 * (((fTemp1 + fTemp2) + fTemp3) + fTemp4)) + (0.025239567999997631 * (((fTemp11 + fTemp12) + fTemp13) + fTemp14))) + (0.01640325439843596 * ((((((((fTemp73 + fTemp43) + fTemp44) + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) + fTemp50))))));
			double fTemp104 = (fTemp41 - fTemp49);
			double fTemp105 = ((fTemp79 - fTemp43) + fTemp46);
			double fTemp106 = (fTemp36 - fTemp44);
			double fTemp107 = (fTemp22 - fTemp27);
			double fTemp108 = (fTemp7 - fTemp16);
			double fTemp109 = (fTemp37 - fTemp45);
			double fTemp110 = (fTemp20 - fTemp25);
			double fTemp111 = (fTemp9 - fTemp18);
			double fTemp112 = (fTemp40 - fTemp48);
			double fTemp113 = (fTemp32 - fTemp53);
			output7[i] = FAUSTFLOAT((fRec0[0] * (((0.021308452520676528 * ((fTemp76 - fTemp51) + fTemp52)) + ((0.0071028175068921775 * fTemp104) + ((0.021308452520676542 * fTemp105) + ((0.0071028175068921827 * fTemp106) + ((0.027231914153020901 * fTemp107) + ((0.043716214137085485 * ((fTemp83 - fTemp15) + fTemp17)) + ((2.6768460856911007e-18 * fTemp108) + ((((3.0114518464024883e-18 * fTemp70) + (0.027231914153020911 * fTemp98)) + (0.021308452520676539 * fTemp101)) + (0.021308452520676525 * fTemp102))))))))) - ((((0.007102817506892181 * fTemp109) + ((0.027231914153020904 * fTemp110) + ((8.0305382570733005e-18 * fTemp111) + (0.021308452520676546 * fTemp100)))) + (0.0071028175068921853 * fTemp112)) + (0.021308452520676532 * fTemp113)))));
			double fTemp114 = (((fTemp6 - fTemp7) + fTemp8) - fTemp9);
			double fTemp115 = (fTemp30 + fTemp52);
			double fTemp116 = (fTemp29 + fTemp51);
			output8[i] = FAUSTFLOAT((fRec0[0] * (((1.3384230428455503e-17 * fTemp13) + ((2.6768460856911007e-18 * fTemp11) + ((0.021858107068542749 * ((((fTemp114 + fTemp15) - fTemp16) + fTemp17) - fTemp18)) + ((0.024590370452110585 * (((fTemp1 - fTemp2) + fTemp3) - fTemp4)) + ((9.2199121612079837e-35 * fTemp103) + (((8.3373691255381378e-18 * fTemp87) + ((1.6674738251076273e-18 * fTemp82) + ((((0.028411270027568724 * fTemp64) + (0.028411270027568717 * fTemp56)) + (0.028411270027568731 * fTemp67)) + (0.028411270027568696 * fTemp68)))) + (2.1746106811858183e-18 * fTemp90))))))) - ((1.8737922599837705e-17 * fTemp14) + ((8.0305382570733005e-18 * fTemp12) + ((1.3047664087114909e-18 * fTemp115) + ((1.1422201709353029e-18 * fTemp116) + ((0.028411270027568748 * fTemp78) + ((0.028411270027568734 * fTemp89) + ((0.028411270027568745 * fTemp88) + ((0.028411270027568741 * fTemp81) + (((5.0024214753228816e-18 * fTemp63) + (1.167231677575339e-17 * fTemp66)) + (3.0444549536601452e-18 * fTemp69)))))))))))));
			double fTemp117 = (0.033388810063348803 * fTemp11);
			double fTemp118 = ((fTemp31 + fTemp51) + fTemp52);
			double fTemp119 = ((fTemp59 + fTemp16) - fTemp18);
			output9[i] = FAUSTFLOAT((fRec0[0] * (((0.033388810063348796 * fTemp12) + (fTemp117 + ((9.7582192711380499e-18 * fTemp3) + (((0.020818537549440803 * fTemp78) + ((0.020818537549440772 * fTemp89) + ((0.016982082124440745 * fTemp82) + ((0.030071220904747838 * fTemp56) + (((6.1334249057128588e-18 * fTemp62) + (0.016982082124440741 * fTemp63)) + (0.030071220904747824 * fTemp64)))))) + (0.0023131708388267431 * fTemp118))))) - ((0.033388810063348817 * fTemp14) + ((0.033388810063348852 * fTemp13) + ((0.016694405031674409 * fTemp119) + ((0.002313170838826747 * fTemp90) + ((0.02081853754944079 * fTemp88) + ((0.020818537549440783 * fTemp81) + ((0.016982082124440766 * fTemp87) + (((((0.026560635762986527 * fTemp60) + (0.016982082124440748 * fTemp66)) + (0.030071220904747821 * fTemp67)) + (0.030071220904747842 * fTemp68)) + (0.0023131708388267444 * fTemp69))))))))))));
			output10[i] = FAUSTFLOAT((fRec0[0] * ((((0.041597435974919175 * ((((fTemp91 - fTemp24) + fTemp25) - fTemp26) + fTemp27)) + ((0.016998264729033874 * fTemp112) + ((0.016998264729033888 * fTemp105) + ((0.016998264729033867 * fTemp106) + ((1.5023761394674526e-17 * fTemp111) + ((5.0079204648915091e-18 * fTemp108) + (0.016998264729033881 * fTemp100))))))) + (0.016998264729033836 * ((((fTemp92 - fTemp51) + fTemp52) - fTemp53) + fTemp54))) - ((0.016998264729033857 * fTemp104) + ((0.016998264729033864 * fTemp109) + ((1.0015840929783018e-17 * fTemp97) + (0.016998264729033919 * fTemp101)))))));
			output11[i] = FAUSTFLOAT((fRec0[0] * ((((0.038794291597509752 * fTemp119) + ((0.0053753232814252143 * fTemp68) + (((0.013154264250377145 * fTemp58) + ((4.7509305030079371e-18 * fTemp62) + (0.013154264250377147 * fTemp63))) + (0.0053753232814252021 * fTemp67)))) + (0.030460165261409405 * fTemp118)) - ((2.5195613817171963e-18 * fTemp3) + ((0.016125969844275594 * ((fTemp93 + fTemp45) - fTemp49)) + ((0.016125969844275591 * ((fTemp94 + fTemp44) - fTemp48)) + ((0.025862861065006511 * (fTemp12 - fTemp14)) + ((0.030460165261409402 * fTemp90) + ((((((0.020573779994945588 * fTemp60) + (0.025862861065006505 * fTemp61)) + (0.013154264250377148 * fTemp66)) + (0.0053753232814252047 * fTemp64)) + (0.0053753232814252056 * fTemp56)) + (0.030460165261409412 * fTemp69))))))))));
			output12[i] = FAUSTFLOAT((fRec0[0] * (((0.033596842045264641 * fTemp70) + (0.0263335984839893 * fTemp75)) - (((0.010558468816293319 * fTemp71) + (0.021480823570105497 * fTemp72)) + (0.020481687709769415 * fTemp74)))));
			double fTemp120 = ((fTemp83 + fTemp15) - fTemp17);
			output13[i] = FAUSTFLOAT((fRec0[0] * (((0.025862861065006505 * fTemp12) + ((3.7793420725757941e-18 * fTemp4) + ((0.038794291597509752 * fTemp120) + ((5.0391227634343926e-18 * fTemp103) + ((0.030460165261409402 * fTemp77) + ((0.0053753232814252004 * fTemp89) + ((0.0053753232814251969 * fTemp88) + ((0.013154264250377147 * fTemp82) + ((2.3754652515039685e-18 * fTemp84) + (0.030460165261409398 * fTemp69)))))))))) - ((0.025862861065006501 * fTemp14) + ((1.2597806908585981e-18 * fTemp2) + ((0.013154264250377145 * ((fTemp96 + fTemp25) - fTemp27)) + (((0.0053753232814251943 * fTemp78) + ((0.016125969844275591 * fTemp80) + ((0.0053753232814251978 * fTemp81) + ((0.013154264250377148 * fTemp87) + ((7.1263957545119048e-18 * fTemp86) + ((0.020573779994945588 * fTemp85) + ((0.025862861065006511 * fTemp61) + (0.016125969844275587 * fTemp68)))))))) + (0.030460165261409412 * fTemp90))))))));
			output14[i] = FAUSTFLOAT((fRec0[0] * (((0.022664352972045151 * (((fTemp39 - fTemp40) - fTemp47) + fTemp48)) + ((1.2735541704855431e-17 * (fTemp21 - fTemp26)) + ((2.5471083409710867e-18 * (fTemp19 - fTemp24)) + ((0.04089277388695433 * ((((fTemp114 - fTemp15) + fTemp16) - fTemp17) + fTemp18)) + (((((2.4393594688416384e-34 * fTemp70) + (0.022664352972045144 * fTemp99)) + (0.022664352972045138 * fTemp95)) + (0.02266435297204512 * fTemp101)) + (5.2042176228676594e-18 * fTemp113)))))) - ((3.1225305737205953e-18 * (fTemp30 - fTemp52)) + ((2.7335294515958808e-18 * (fTemp29 - fTemp51)) + ((0.022664352972045162 * ((fTemp55 - fTemp44) - fTemp45)) + ((0.022664352972045165 * fTemp104) + ((1.7829758386797603e-17 * fTemp107) + ((7.6413250229132581e-18 * fTemp110) + (7.2859046720147234e-18 * fTemp102))))))))));
			output15[i] = FAUSTFLOAT((fRec0[0] * (((0.033388810063348762 * fTemp13) + ((0.033388810063348817 * fTemp12) + ((1.4637328906707076e-17 * fTemp4) + ((0.016694405031674409 * fTemp120) + ((0.0023131708388267444 * fTemp115) + ((6.0979103388536467e-51 * fTemp103) + (((0.0300712209047478 * fTemp89) + ((0.03007122090474779 * fTemp88) + ((0.016982082124440721 * fTemp87) + ((9.2001373585692901e-18 * fTemp86) + ((0.026560635762986527 * fTemp85) + (((0.016982082124440748 * fTemp63) + (0.020818537549440738 * fTemp64)) + (0.02081853754944071 * fTemp68))))))) + (0.0023131708388267409 * fTemp90)))))))) - ((0.033388810063348796 * fTemp14) + (fTemp117 + ((4.879109635569025e-18 * fTemp2) + ((0.016982082124440741 * (((fTemp19 + fTemp22) + fTemp24) + fTemp27)) + ((0.0023131708388267449 * fTemp116) + ((0.030071220904747779 * fTemp78) + ((0.030071220904747797 * fTemp81) + ((3.0667124528564294e-18 * fTemp84) + (((0.020818537549440724 * fTemp56) + (0.020818537549440744 * fTemp67)) + (0.0023131708388267431 * fTemp69)))))))))))));
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
