/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAEncLebedev505"
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
		m->declare("name", "HOAEncLebedev505");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 50;
		
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
			double fTemp21 = (fTemp19 + fTemp20);
			double fTemp22 = double(input20[i]);
			double fTemp23 = double(input21[i]);
			double fTemp24 = ((fTemp21 + fTemp22) + fTemp23);
			double fTemp25 = double(input22[i]);
			double fTemp26 = double(input23[i]);
			double fTemp27 = double(input24[i]);
			double fTemp28 = double(input25[i]);
			double fTemp29 = ((((fTemp24 + fTemp25) + fTemp26) + fTemp27) + fTemp28);
			double fTemp30 = double(input26[i]);
			double fTemp31 = double(input27[i]);
			double fTemp32 = (fTemp30 + fTemp31);
			double fTemp33 = double(input28[i]);
			double fTemp34 = double(input29[i]);
			double fTemp35 = ((fTemp32 + fTemp33) + fTemp34);
			double fTemp36 = double(input30[i]);
			double fTemp37 = double(input31[i]);
			double fTemp38 = double(input32[i]);
			double fTemp39 = double(input33[i]);
			double fTemp40 = double(input34[i]);
			double fTemp41 = double(input35[i]);
			double fTemp42 = double(input36[i]);
			double fTemp43 = double(input37[i]);
			double fTemp44 = double(input38[i]);
			double fTemp45 = double(input39[i]);
			double fTemp46 = double(input40[i]);
			double fTemp47 = double(input41[i]);
			double fTemp48 = double(input42[i]);
			double fTemp49 = double(input43[i]);
			double fTemp50 = double(input44[i]);
			double fTemp51 = double(input45[i]);
			double fTemp52 = double(input46[i]);
			double fTemp53 = double(input47[i]);
			double fTemp54 = double(input48[i]);
			double fTemp55 = double(input49[i]);
			output0[i] = FAUSTFLOAT((fRec0[0] * ((((0.012698412698412698 * (((((fTemp0 + fTemp1) + fTemp2) + fTemp3) + fTemp4) + fTemp5)) + (0.022574955908289243 * ((((((((fTemp10 + fTemp11) + fTemp12) + fTemp13) + fTemp14) + fTemp15) + fTemp16) + fTemp17) + fTemp18))) + (0.021093750000000001 * fTemp29)) + (0.02017333553791887 * ((((((((((((((((((((fTemp35 + fTemp36) + fTemp37) + fTemp38) + fTemp39) + fTemp40) + fTemp41) + fTemp42) + fTemp43) + fTemp44) + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) + fTemp50) + fTemp51) + fTemp52) + fTemp53) + fTemp54) + fTemp55)))));
			double fTemp56 = (fTemp37 + fTemp38);
			double fTemp57 = (fTemp39 + fTemp47);
			double fTemp58 = (fTemp19 - fTemp22);
			double fTemp59 = ((fTemp58 + fTemp25) - fTemp27);
			double fTemp60 = (fTemp7 - fTemp9);
			double fTemp61 = (fTemp2 - fTemp4);
			double fTemp62 = (fTemp11 - fTemp13);
			double fTemp63 = (fTemp8 + fTemp17);
			double fTemp64 = (fTemp20 + fTemp26);
			double fTemp65 = (fTemp36 + fTemp44);
			double fTemp66 = (fTemp32 - fTemp33);
			double fTemp67 = (fTemp23 + fTemp28);
			double fTemp68 = (fTemp40 + fTemp48);
			double fTemp69 = (fTemp43 + fTemp51);
			double fTemp70 = (fTemp34 + fTemp55);
			output1[i] = FAUSTFLOAT((fRec0[0] * (((2.6935244158092871e-18 * fTemp3) + (((0.031605542669238242 * ((((((fTemp56 - fTemp41) - fTemp42) + fTemp45) + fTemp46) - fTemp49) - fTemp50)) + ((0.010535180889746096 * fTemp57) + (((0.021093749999999994 * fTemp59) + (((0.027648561470568506 * ((((fTemp60 + fTemp12) - fTemp14) + fTemp16) - fTemp18)) + (((0.021994295969128601 * fTemp61) + (0.027648561470568499 * fTemp62)) + (3.3859722305960555e-18 * fTemp63))) + (0.021093749999999998 * fTemp64))) + (0.010535180889746094 * fTemp65)))) + (0.010535180889746065 * (((fTemp66 + fTemp52) + fTemp53) - fTemp54)))) - ((((0.021093750000000001 * fTemp67) + (0.010535180889746089 * fTemp68)) + (0.010535180889746113 * fTemp69)) + (0.010535180889746067 * fTemp70)))));
			double fTemp71 = (fTemp0 - fTemp5);
			double fTemp72 = ((((fTemp10 - fTemp15) - fTemp16) - fTemp17) - fTemp18);
			double fTemp73 = ((((fTemp24 - fTemp25) - fTemp26) - fTemp27) - fTemp28);
			double fTemp74 = (((((((fTemp36 + fTemp37) + fTemp38) + fTemp39) + fTemp40) + fTemp41) + fTemp42) + fTemp43);
			double fTemp75 = ((((((((fTemp74 - fTemp44) - fTemp45) - fTemp46) - fTemp47) - fTemp48) - fTemp49) - fTemp50) - fTemp51);
			double fTemp76 = ((((fTemp35 - fTemp52) - fTemp53) - fTemp54) - fTemp55);
			output2[i] = FAUSTFLOAT((fRec0[0] * (((((0.021994295969128601 * fTemp71) + (0.027648561470568499 * fTemp72)) + (0.021093750000000001 * fTemp73)) + (0.010535180889746075 * fTemp75)) + (0.031605542669238249 * fTemp76))));
			double fTemp77 = (fTemp30 - fTemp31);
			double fTemp78 = ((fTemp77 + fTemp52) - fTemp53);
			double fTemp79 = (fTemp42 + fTemp50);
			double fTemp80 = (fTemp36 - fTemp39);
			double fTemp81 = ((((fTemp80 - fTemp40) + fTemp44) - fTemp47) - fTemp48);
			double fTemp82 = (fTemp37 + fTemp45);
			double fTemp83 = (fTemp19 + fTemp25);
			double fTemp84 = (fTemp6 - fTemp8);
			double fTemp85 = (fTemp7 + fTemp16);
			double fTemp86 = (fTemp1 - fTemp3);
			double fTemp87 = (fTemp9 + fTemp18);
			double fTemp88 = (fTemp22 + fTemp27);
			double fTemp89 = (fTemp38 + fTemp46);
			double fTemp90 = (fTemp41 + fTemp49);
			double fTemp91 = (fTemp33 + fTemp54);
			output3[i] = FAUSTFLOAT((fRec0[0] * (((0.027648561470568496 * fTemp14) + ((0.010535180889746065 * fTemp78) + ((0.010535180889746074 * fTemp79) + ((0.031605542669238242 * fTemp81) + ((0.010535180889746081 * fTemp82) + ((0.021093749999999998 * fTemp83) + ((0.027648561470568506 * ((((fTemp84 + fTemp11) - fTemp13) + fTemp15) - fTemp17)) + ((1.6929861152980278e-18 * fTemp85) + ((1.3467622079046435e-18 * ((fTemp0 + fTemp2) + fTemp5)) + ((0.021994295969128601 * fTemp86) + (((0.021093749999999991 * fTemp67) + (0.031605542669238228 * fTemp69)) + (0.010535180889746063 * fTemp70)))))))))))) - ((0.027648561470568499 * fTemp12) + ((4.0402866237139308e-18 * fTemp4) + ((((((5.0789583458940831e-18 * fTemp87) + (0.021093749999999994 * fTemp64)) + (0.021093750000000001 * fTemp88)) + (0.010535180889746079 * fTemp89)) + (0.010535180889746086 * fTemp90)) + (0.010535180889746067 * fTemp91)))))));
			double fTemp92 = (fTemp19 - fTemp20);
			double fTemp93 = ((fTemp92 + fTemp22) - fTemp23);
			double fTemp94 = ((((fTemp93 + fTemp25) - fTemp26) + fTemp27) - fTemp28);
			double fTemp95 = (((fTemp11 - fTemp12) + fTemp13) - fTemp14);
			double fTemp96 = ((fTemp77 + fTemp33) - fTemp34);
			double fTemp97 = ((((fTemp96 + fTemp52) - fTemp53) + fTemp54) - fTemp55);
			output4[i] = FAUSTFLOAT((fRec0[0] * (((9.0343555392074636e-18 * fTemp4) + ((3.0114518464024883e-18 * fTemp2) + (((0.027231914153020897 * fTemp94) + ((0.043716214137085485 * fTemp95) + (((0.021308452520676556 * fTemp82) + ((8.0305382570733035e-18 * fTemp87) + ((2.6768460856911014e-18 * fTemp85) + ((0.021308452520676577 * fTemp65) + (0.02130845252067657 * fTemp68))))) + (0.021308452520676563 * fTemp90)))) + (0.0071028175068921654 * fTemp97)))) - ((6.0229036928049765e-18 * fTemp3) + ((0.021308452520676542 * fTemp79) + ((0.021308452520676546 * fTemp89) + (((5.3536921713822029e-18 * fTemp63) + (0.021308452520676584 * fTemp57)) + (0.021308452520676618 * fTemp69))))))));
			double fTemp98 = (fTemp38 - fTemp42);
			double fTemp99 = ((fTemp98 - fTemp46) + fTemp50);
			double fTemp100 = (fTemp37 - fTemp41);
			double fTemp101 = ((fTemp100 - fTemp45) + fTemp49);
			double fTemp102 = (fTemp39 - fTemp47);
			double fTemp103 = (fTemp20 - fTemp23);
			double fTemp104 = (fTemp8 - fTemp17);
			double fTemp105 = ((fTemp60 - fTemp16) + fTemp18);
			double fTemp106 = ((fTemp58 - fTemp25) + fTemp27);
			double fTemp107 = (fTemp36 - fTemp44);
			double fTemp108 = (fTemp40 - fTemp48);
			double fTemp109 = (fTemp43 - fTemp51);
			double fTemp110 = (fTemp34 - fTemp55);
			output5[i] = FAUSTFLOAT((fRec0[0] * ((((0.021308452520676553 * fTemp99) + ((0.021308452520676546 * fTemp101) + ((0.0071028175068921931 * fTemp102) + (((0.027231914153020911 * ((fTemp103 - fTemp26) + fTemp28)) + (((5.3536921713822013e-18 * fTemp104) + (0.043716214137085485 * fTemp105)) + (0.027231914153020904 * fTemp106))) + (0.0071028175068921914 * fTemp107))))) + (0.021308452520676528 * (((fTemp66 - fTemp52) - fTemp53) + fTemp54))) - (((0.0071028175068921871 * fTemp108) + (0.0071028175068922053 * fTemp109)) + (0.021308452520676532 * fTemp110)))));
			double fTemp111 = ((((fTemp10 + fTemp15) + fTemp16) + fTemp17) + fTemp18);
			double fTemp112 = (fTemp0 + fTemp5);
			double fTemp113 = ((((fTemp35 + fTemp52) + fTemp53) + fTemp54) + fTemp55);
			double fTemp114 = (((fTemp1 + fTemp2) + fTemp3) + fTemp4);
			double fTemp115 = (((fTemp11 + fTemp12) + fTemp13) + fTemp14);
			double fTemp116 = ((((((((fTemp74 + fTemp44) + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) + fTemp50) + fTemp51);
			output6[i] = FAUSTFLOAT((fRec0[0] * ((((0.012619783999998805 * fTemp111) + ((0.028394513999997331 * fTemp112) + (5.236595479505967e-18 * fTemp29))) + (0.032806508796871948 * fTemp113)) - (((0.014197256999998666 * fTemp114) + (0.025239567999997631 * fTemp115)) + (0.01640325439843596 * fTemp116)))));
			double fTemp117 = ((fTemp77 - fTemp52) + fTemp53);
			double fTemp118 = (fTemp42 - fTemp50);
			double fTemp119 = ((fTemp80 - fTemp44) + fTemp47);
			double fTemp120 = (fTemp37 - fTemp45);
			double fTemp121 = (fTemp23 - fTemp28);
			double fTemp122 = ((fTemp84 - fTemp15) + fTemp17);
			double fTemp123 = (fTemp7 - fTemp16);
			double fTemp124 = (fTemp38 - fTemp46);
			double fTemp125 = (fTemp20 - fTemp26);
			double fTemp126 = (fTemp9 - fTemp18);
			double fTemp127 = (fTemp41 - fTemp49);
			double fTemp128 = (fTemp33 - fTemp54);
			output7[i] = FAUSTFLOAT((fRec0[0] * (((0.021308452520676528 * fTemp117) + ((0.0071028175068921775 * fTemp118) + ((0.021308452520676542 * fTemp119) + ((0.0071028175068921827 * fTemp120) + ((0.027231914153020901 * fTemp121) + ((0.043716214137085485 * fTemp122) + ((2.6768460856911007e-18 * fTemp123) + ((((3.0114518464024883e-18 * fTemp71) + (0.027231914153020911 * fTemp106)) + (0.021308452520676539 * fTemp109)) + (0.021308452520676525 * fTemp110))))))))) - ((((0.007102817506892181 * fTemp124) + ((0.027231914153020904 * fTemp125) + ((8.0305382570733005e-18 * fTemp126) + (0.021308452520676546 * fTemp108)))) + (0.0071028175068921853 * fTemp127)) + (0.021308452520676532 * fTemp128)))));
			double fTemp129 = (((fTemp6 - fTemp7) + fTemp8) - fTemp9);
			double fTemp130 = ((((fTemp129 + fTemp15) - fTemp16) + fTemp17) - fTemp18);
			double fTemp131 = (((fTemp1 - fTemp2) + fTemp3) - fTemp4);
			double fTemp132 = (fTemp31 + fTemp53);
			double fTemp133 = (fTemp30 + fTemp52);
			output8[i] = FAUSTFLOAT((fRec0[0] * (((1.3384230428455503e-17 * fTemp13) + ((2.6768460856911007e-18 * fTemp11) + ((0.021858107068542749 * fTemp130) + ((0.024590370452110585 * fTemp131) + ((9.2199121612079837e-35 * fTemp112) + (((8.3373691255381378e-18 * fTemp88) + ((1.6674738251076273e-18 * fTemp83) + ((((0.028411270027568724 * fTemp65) + (0.028411270027568717 * fTemp57)) + (0.028411270027568731 * fTemp68)) + (0.028411270027568696 * fTemp69)))) + (2.1746106811858183e-18 * fTemp91))))))) - ((1.8737922599837705e-17 * fTemp14) + ((8.0305382570733005e-18 * fTemp12) + ((1.3047664087114909e-18 * fTemp132) + ((1.1422201709353029e-18 * fTemp133) + ((0.028411270027568748 * fTemp79) + ((0.028411270027568734 * fTemp90) + ((0.028411270027568745 * fTemp89) + ((0.028411270027568741 * fTemp82) + (((5.0024214753228816e-18 * fTemp64) + (1.167231677575339e-17 * fTemp67)) + (3.0444549536601452e-18 * fTemp70)))))))))))));
			double fTemp134 = (0.033388810063348803 * fTemp11);
			double fTemp135 = (6.1334249057128588e-18 * fTemp63);
			double fTemp136 = ((fTemp32 + fTemp52) + fTemp53);
			double fTemp137 = ((fTemp60 + fTemp16) - fTemp18);
			output9[i] = FAUSTFLOAT((fRec0[0] * (((0.033388810063348796 * fTemp12) + (fTemp134 + ((9.7582192711380499e-18 * fTemp3) + (((0.020818537549440803 * fTemp79) + ((0.020818537549440772 * fTemp90) + ((0.016982082124440745 * fTemp83) + ((0.030071220904747838 * fTemp57) + ((fTemp135 + (0.016982082124440741 * fTemp64)) + (0.030071220904747824 * fTemp65)))))) + (0.0023131708388267431 * fTemp136))))) - ((0.033388810063348817 * fTemp14) + ((0.033388810063348852 * fTemp13) + ((0.016694405031674409 * fTemp137) + ((0.002313170838826747 * fTemp91) + ((0.02081853754944079 * fTemp89) + ((0.020818537549440783 * fTemp82) + ((0.016982082124440766 * fTemp88) + (((((0.026560635762986527 * fTemp61) + (0.016982082124440748 * fTemp67)) + (0.030071220904747821 * fTemp68)) + (0.030071220904747842 * fTemp69)) + (0.0023131708388267444 * fTemp70))))))))))));
			double fTemp138 = ((((fTemp93 - fTemp25) + fTemp26) - fTemp27) + fTemp28);
			double fTemp139 = ((((fTemp96 - fTemp52) + fTemp53) - fTemp54) + fTemp55);
			output10[i] = FAUSTFLOAT((fRec0[0] * ((((0.041597435974919175 * fTemp138) + ((0.016998264729033874 * fTemp127) + ((0.016998264729033888 * fTemp119) + ((0.016998264729033867 * fTemp120) + ((1.5023761394674526e-17 * fTemp126) + ((5.0079204648915091e-18 * fTemp123) + (0.016998264729033881 * fTemp108))))))) + (0.016998264729033836 * fTemp139)) - ((0.016998264729033857 * fTemp118) + ((0.016998264729033864 * fTemp124) + ((1.0015840929783018e-17 * fTemp104) + (0.016998264729033919 * fTemp109)))))));
			double fTemp140 = ((fTemp98 + fTemp46) - fTemp50);
			double fTemp141 = ((fTemp100 + fTemp45) - fTemp49);
			double fTemp142 = (fTemp12 - fTemp14);
			output11[i] = FAUSTFLOAT((fRec0[0] * ((((0.038794291597509752 * fTemp137) + ((0.0053753232814252143 * fTemp69) + (((0.013154264250377145 * fTemp59) + ((4.7509305030079371e-18 * fTemp63) + (0.013154264250377147 * fTemp64))) + (0.0053753232814252021 * fTemp68)))) + (0.030460165261409405 * fTemp136)) - ((2.5195613817171963e-18 * fTemp3) + ((0.016125969844275594 * fTemp140) + ((0.016125969844275591 * fTemp141) + ((0.025862861065006511 * fTemp142) + ((0.030460165261409402 * fTemp91) + ((((((0.020573779994945588 * fTemp61) + (0.025862861065006505 * fTemp62)) + (0.013154264250377148 * fTemp67)) + (0.0053753232814252047 * fTemp65)) + (0.0053753232814252056 * fTemp57)) + (0.030460165261409412 * fTemp70))))))))));
			output12[i] = FAUSTFLOAT((fRec0[0] * (((0.033596842045264641 * fTemp71) + (0.0263335984839893 * fTemp76)) - (((0.010558468816293319 * fTemp72) + (0.021480823570105497 * fTemp73)) + (0.020481687709769415 * fTemp75)))));
			double fTemp143 = ((fTemp84 + fTemp15) - fTemp17);
			output13[i] = FAUSTFLOAT((fRec0[0] * (((0.025862861065006505 * fTemp12) + ((3.7793420725757941e-18 * fTemp4) + ((0.038794291597509752 * fTemp143) + ((5.0391227634343926e-18 * fTemp112) + ((0.030460165261409402 * fTemp78) + ((0.0053753232814252004 * fTemp90) + ((0.0053753232814251969 * fTemp89) + ((0.013154264250377147 * fTemp83) + ((2.3754652515039685e-18 * fTemp85) + (0.030460165261409398 * fTemp70)))))))))) - ((0.025862861065006501 * fTemp14) + ((1.2597806908585981e-18 * fTemp2) + ((0.013154264250377145 * ((fTemp103 + fTemp26) - fTemp28)) + (((0.0053753232814251943 * fTemp79) + ((0.016125969844275591 * fTemp81) + ((0.0053753232814251978 * fTemp82) + ((0.013154264250377148 * fTemp88) + ((7.1263957545119048e-18 * fTemp87) + ((0.020573779994945588 * fTemp86) + ((0.025862861065006511 * fTemp62) + (0.016125969844275587 * fTemp69)))))))) + (0.030460165261409412 * fTemp91))))))));
			double fTemp144 = (fTemp40 - fTemp41);
			double fTemp145 = (fTemp22 - fTemp27);
			double fTemp146 = (fTemp19 - fTemp25);
			double fTemp147 = ((((fTemp129 - fTemp15) + fTemp16) - fTemp17) + fTemp18);
			double fTemp148 = (fTemp30 - fTemp52);
			double fTemp149 = (fTemp31 - fTemp53);
			output14[i] = FAUSTFLOAT((fRec0[0] * (((0.022664352972045151 * ((fTemp144 - fTemp48) + fTemp49)) + ((1.2735541704855431e-17 * fTemp145) + ((2.5471083409710867e-18 * fTemp146) + ((0.04089277388695433 * fTemp147) + (((((2.4393594688416384e-34 * fTemp71) + (0.022664352972045144 * fTemp107)) + (0.022664352972045138 * fTemp102)) + (0.02266435297204512 * fTemp109)) + (5.2042176228676594e-18 * fTemp128)))))) - ((2.7335294515958808e-18 * fTemp148) + ((3.1225305737205953e-18 * fTemp149) + ((0.022664352972045162 * ((fTemp56 - fTemp45) - fTemp46)) + ((0.022664352972045165 * fTemp118) + ((1.7829758386797603e-17 * fTemp121) + ((7.6413250229132581e-18 * fTemp125) + (7.2859046720147234e-18 * fTemp110))))))))));
			double fTemp150 = (3.0667124528564294e-18 * fTemp85);
			output15[i] = FAUSTFLOAT((fRec0[0] * (((0.033388810063348762 * fTemp13) + ((0.033388810063348817 * fTemp12) + ((1.4637328906707076e-17 * fTemp4) + ((0.016694405031674409 * fTemp143) + ((0.0023131708388267444 * fTemp132) + ((6.0979103388536467e-51 * fTemp112) + (((0.0300712209047478 * fTemp90) + ((0.03007122090474779 * fTemp89) + ((0.016982082124440721 * fTemp88) + ((9.2001373585692901e-18 * fTemp87) + ((0.026560635762986527 * fTemp86) + (((0.016982082124440748 * fTemp64) + (0.020818537549440738 * fTemp65)) + (0.02081853754944071 * fTemp69))))))) + (0.0023131708388267409 * fTemp91)))))))) - ((0.033388810063348796 * fTemp14) + (fTemp134 + ((4.879109635569025e-18 * fTemp2) + ((0.016982082124440741 * (((fTemp19 + fTemp23) + fTemp25) + fTemp28)) + ((0.0023131708388267449 * fTemp133) + ((0.030071220904747779 * fTemp79) + ((0.030071220904747797 * fTemp82) + (fTemp150 + (((0.020818537549440724 * fTemp57) + (0.020818537549440744 * fTemp68)) + (0.0023131708388267431 * fTemp70)))))))))))));
			double fTemp151 = ((fTemp80 + fTemp44) - fTemp47);
			double fTemp152 = (fTemp37 - fTemp38);
			output16[i] = FAUSTFLOAT((fRec0[0] * (((4.2933974339990009e-17 * fTemp14) + ((3.0667124528564293e-17 * fTemp13) + ((1.8400274717138574e-17 * fTemp12) + ((6.133424905712858e-18 * fTemp11) + ((0.035508216952135609 * ((fTemp144 + fTemp48) - fTemp49)) + ((0.035508216952135616 * fTemp151) + ((5.4356280292327993e-19 * fTemp132) + ((9.0593800487213328e-19 * fTemp91) + ((0.035508216952135588 * fTemp79) + ((1.2735541704855428e-17 * fTemp88) + ((2.5471083409710855e-18 * fTemp83) + (((7.6413250229132566e-18 * fTemp64) + (1.7829758386797597e-17 * fTemp67)) + (1.2683132068209864e-18 * fTemp70))))))))))))) - ((2.0700309056780893e-17 * fTemp4) + ((1.3800206037853928e-17 * fTemp3) + ((6.9001030189269641e-18 * fTemp2) + ((0.035508216952135595 * ((fTemp152 + fTemp45) - fTemp46)) + (((9.200137358569287e-18 * fTemp87) + (fTemp150 + (fTemp135 + (0.035508216952135643 * fTemp69)))) + (4.7584639941967359e-19 * fTemp133)))))))));
			double fTemp153 = (fTemp36 - fTemp40);
			output17[i] = FAUSTFLOAT((fRec0[0] * (((0.027200442744256398 * ((fTemp153 - fTemp44) + fTemp48)) + ((0.0062770252486745223 * fTemp148) + (((0.029413829057838591 * fTemp146) + ((((0.02941382905783858 * fTemp125) + ((1.3010959028184069e-17 * fTemp104) + (0.027200442744256408 * fTemp102))) + (0.018831075746023631 * fTemp127)) + (0.018831075746023662 * fTemp118))) + (0.0062770252486745231 * fTemp149)))) - ((0.029413829057838629 * fTemp145) + ((0.0062770252486745335 * fTemp128) + ((0.018831075746023648 * fTemp124) + ((0.018831075746023641 * fTemp120) + ((0.029413829057838594 * fTemp121) + (((0.035414181017315374 * fTemp105) + (0.027200442744256415 * fTemp109)) + (0.0062770252486745266 * fTemp110))))))))));
			output18[i] = FAUSTFLOAT((fRec0[0] * (((5.2159876025162161e-18 * fTemp3) + (((0.031444705933590805 * fTemp94) + ((0.0067104222539056237 * fTemp79) + ((0.0067104222539056254 * fTemp89) + ((1.7386625341720719e-17 * fTemp87) + ((5.7955417805735729e-18 * fTemp85) + ((0.0067104222539056367 * fTemp57) + (0.0067104222539056471 * fTemp69))))))) + (0.02907849643359095 * fTemp97))) - ((7.823981403774323e-18 * fTemp4) + ((2.607993801258108e-18 * fTemp2) + ((0.037859351999996446 * fTemp95) + (((0.0067104222539056272 * fTemp82) + (((1.1591083561147146e-17 * fTemp63) + (0.006710422253905635 * fTemp65)) + (0.0067104222539056315 * fTemp68))) + (0.0067104222539056298 * fTemp90))))))));
			double fTemp154 = (fTemp32 - fTemp34);
			output19[i] = FAUSTFLOAT((fRec0[0] * ((((0.011117382399029452 * fTemp121) + ((((1.6392267574774099e-18 * fTemp104) + (0.013385302265262959 * fTemp105)) + (0.010280801007450529 * fTemp108)) + (0.010280801007450555 * fTemp109))) + (0.035587388102713349 * (((fTemp154 - fTemp52) - fTemp53) + fTemp55))) - ((0.01111738239902945 * ((((fTemp21 - fTemp22) - fTemp25) - fTemp26) + fTemp27)) + ((((0.030842403022351562 * fTemp101) + ((0.010280801007450534 * fTemp107) + (0.010280801007450538 * fTemp102))) + (0.030842403022351569 * fTemp99)) + (0.035587388102713342 * fTemp128))))));
			output20[i] = FAUSTFLOAT((fRec0[0] * ((((((0.014285714285714285 * fTemp114) + (0.038095238095238099 * fTemp112)) + (0.025396825396825397 * fTemp115)) + (0.0042514054232804356 * fTemp116)) + (0.014254712301587407 * fTemp113)) - ((0.027513227513227514 * fTemp111) + (0.024609375000000013 * fTemp29)))));
			output21[i] = FAUSTFLOAT((fRec0[0] * (((0.011117382399029452 * fTemp145) + ((0.035587388102713342 * fTemp117) + ((0.010280801007450525 * fTemp127) + ((0.010280801007450517 * fTemp124) + ((0.013385302265262959 * fTemp122) + ((8.1961337873870493e-19 * fTemp123) + (((7.3765204086483564e-18 * fTemp71) + (0.030842403022351562 * fTemp108)) + (0.035587388102713335 * fTemp110)))))))) - ((0.01111738239902945 * ((fTemp92 - fTemp25) + fTemp26)) + (((0.010280801007450515 * fTemp118) + ((0.030842403022351559 * fTemp119) + ((0.010280801007450522 * fTemp120) + ((0.011117382399029449 * fTemp121) + ((2.4588401362161147e-18 * fTemp126) + (0.030842403022351552 * fTemp109)))))) + (0.035587388102713349 * fTemp128))))));
			double fTemp155 = (fTemp36 + fTemp39);
			output22[i] = FAUSTFLOAT((fRec0[0] * (((1.6227516985606004e-17 * fTemp14) + ((6.9546501366882881e-18 * fTemp12) + ((0.047324189999995547 * fTemp130) + ((4.7908068913603203e-34 * fTemp112) + (((((((1.9254329235850881e-18 * fTemp83) + (9.6271646179254396e-18 * fTemp88)) + (0.0089472296718741701 * fTemp82)) + (0.0089472296718741719 * fTemp89)) + (0.0089472296718741684 * fTemp90)) + (0.0089472296718741736 * fTemp79)) + (8.902721895353721e-18 * fTemp91)))))) - ((1.1591083561147147e-17 * fTemp13) + ((2.3182167122294292e-18 * fTemp11) + ((0.0089472296718741632 * ((fTemp155 + fTemp44) + fTemp47)) + ((5.3416331372122328e-18 * fTemp132) + ((4.6761788733398907e-18 * fTemp133) + ((0.021295885499997998 * fTemp131) + (((((5.7762987707552641e-18 * fTemp64) + (1.3478030465095617e-17 * fTemp67)) + (0.0089472296718741667 * fTemp68)) + (0.0089472296718741563 * fTemp69)) + (1.246381065349521e-17 * fTemp70)))))))))));
			output23[i] = FAUSTFLOAT((fRec0[0] * (((0.0062770252486745266 * fTemp149) + ((0.029413829057838549 * fTemp145) + (((0.027200442744256377 * fTemp127) + ((0.027200442744256363 * fTemp124) + ((0.029413829057838594 * fTemp125) + ((1.9516438542276106e-17 * fTemp126) + ((0.035414181017315374 * fTemp122) + (((1.8293731016560941e-50 * fTemp71) + (0.018831075746023603 * fTemp107)) + (0.018831075746023575 * fTemp109))))))) + (0.0062770252486745171 * fTemp128)))) - ((0.0062770252486745275 * fTemp148) + ((0.029413829057838584 * fTemp146) + ((0.027200442744256356 * fTemp118) + ((0.027200442744256374 * fTemp120) + ((0.02941382905783858 * fTemp121) + ((6.5054795140920346e-18 * fTemp123) + (((0.018831075746023586 * fTemp102) + (0.01883107574602361 * fTemp108)) + (0.0062770252486745231 * fTemp110)))))))))));
			output24[i] = FAUSTFLOAT((fRec0[0] * (((0.012520803773755805 * fTemp111) + ((3.9603917912335094e-67 * fTemp112) + ((0.028171808490950554 * fTemp114) + ((0.010356563277706251 * fTemp79) + ((0.010356563277706199 * fTemp90) + ((0.010356563277706232 * fTemp89) + ((0.010356563277706217 * fTemp82) + ((((0.010356563277706152 * fTemp65) + (0.010356563277706145 * fTemp57)) + (0.010356563277706178 * fTemp68)) + (0.010356563277706064 * fTemp69))))))))) - (((0.050083215095023212 * fTemp115) + (0.020798717987459581 * fTemp29)) + (0.0014795090396723079 * fTemp113)))));
			output25[i] = FAUSTFLOAT((fRec0[0] * (((0.037142706401659856 * fTemp14) + ((0.037142706401659786 * fTemp13) + ((1.8092222748840437e-17 * fTemp3) + ((0.036961085234966218 * ((((fTemp155 - fTemp40) + fTemp44) + fTemp47) - fTemp48)) + ((0.0092856766004149483 * fTemp137) + ((0.00046786183841729064 * fTemp91) + ((0.0014035855152518362 * fTemp79) + ((0.001403585515251904 * fTemp90) + ((0.012594249175901172 * fTemp88) + ((((0.029546842014263944 * fTemp61) + (5.6858370633078216e-18 * fTemp63)) + (0.012594249175901195 * fTemp67)) + (0.00046786183841729151 * fTemp70))))))))))) - ((0.037142706401659828 * fTemp12) + ((0.037142706401659779 * fTemp11) + ((((0.0014035855152518585 * fTemp89) + ((0.0014035855152518815 * fTemp82) + ((0.012594249175901169 * fTemp83) + ((0.012594249175901188 * fTemp64) + (0.036961085234966211 * fTemp69))))) + (0.00046786183841729075 * fTemp133)) + (0.00046786183841729124 * fTemp132)))))));
			output26[i] = FAUSTFLOAT((fRec0[0] * (((0.035508216952135595 * ((((fTemp80 + fTemp40) - fTemp44) + fTemp47) - fTemp48)) + ((1.63068840876984e-18 * fTemp149) + ((2.4386705720379917e-17 * fTemp145) + ((4.8773411440759829e-18 * fTemp146) + ((2.7178140146164e-18 * fTemp128) + ((0.035508216952135567 * fTemp118) + ((0.035508216952135574 * fTemp124) + ((3.4141388008531875e-17 * fTemp121) + ((1.4632023432227946e-17 * fTemp125) + (3.8049396204629603e-18 * fTemp110)))))))))) - (((0.035508216952135588 * fTemp127) + ((0.035508216952135581 * fTemp120) + ((2.1576234629586837e-17 * fTemp126) + ((7.1920782098622802e-18 * fTemp123) + ((1.438415641972456e-17 * fTemp104) + (0.035508216952135623 * fTemp109)))))) + (1.427539198259021e-18 * fTemp148)))));
			output27[i] = FAUSTFLOAT((fRec0[0] * (((0.027684538794142632 * fTemp14) + ((0.027684538794142663 * fTemp13) + ((((0.0031385126243372867 * fTemp89) + ((0.0031385126243372858 * fTemp82) + ((0.028161597282885743 * fTemp83) + ((((0.022022915754780329 * fTemp61) + (1.779948545556168e-17 * fTemp63)) + (0.028161597282885732 * fTemp64)) + (0.0045334071240427532 * fTemp69))))) + (0.012205326872422687 * fTemp133)) + (0.012205326872422689 * fTemp132)))) - ((0.027684538794142618 * fTemp12) + ((0.027684538794142628 * fTemp11) + ((8.0910879860950644e-18 * fTemp3) + ((0.0045334071240427497 * ((fTemp153 + fTemp44) - fTemp48)) + ((0.048447942889749587 * fTemp137) + ((0.012205326872422709 * fTemp91) + ((0.0031385126243372885 * fTemp79) + ((0.0031385126243372841 * fTemp90) + ((0.028161597282885781 * fTemp88) + (((0.028161597282885746 * fTemp67) + (0.0045334071240427515 * fTemp57)) + (0.012205326872422696 * fTemp70))))))))))))));
			output28[i] = FAUSTFLOAT((fRec0[0] * ((((1.5316979614614479e-17 * fTemp138) + ((0.020500678615759105 * fTemp118) + ((0.020500678615759112 * fTemp124) + ((1.2457044871490476e-17 * fTemp126) + ((4.1523482904968253e-18 * fTemp123) + ((0.020500678615759146 * fTemp102) + (0.020500678615759181 * fTemp109))))))) + (0.041001357231518168 * fTemp139)) - (((0.020500678615759119 * fTemp120) + (((8.3046965809936507e-18 * fTemp104) + (0.020500678615759143 * fTemp107)) + (0.020500678615759133 * fTemp108))) + (0.020500678615759129 * fTemp127)))));
			output29[i] = FAUSTFLOAT((fRec0[0] * (((2.4969639621850082e-18 * fTemp3) + (((0.025630902468550199 * fTemp142) + (((((0.020389258061373223 * fTemp61) + (0.025630902468550196 * fTemp62)) + (0.026072572806195689 * fTemp67)) + (0.0009685660236671092 * fTemp68)) + (0.00096856602366711159 * fTemp69))) + (0.035191232193238609 * (((fTemp154 + fTemp52) + fTemp53) - fTemp55)))) - ((0.0029056980710013259 * fTemp140) + ((0.0029056980710013254 * fTemp141) + ((0.01922317685141265 * fTemp137) + ((((((2.3541602000525996e-18 * fTemp63) + (0.026072572806195685 * fTemp64)) + (0.026072572806195678 * fTemp59)) + (0.00096856602366710964 * fTemp65)) + (0.00096856602366710996 * fTemp57)) + (0.035191232193238602 * fTemp91))))))));
			output30[i] = FAUSTFLOAT((fRec0[0] * (((0.042115870353719366 * fTemp71) + (0.023090966710758377 * fTemp75)) - (((0.028125949882539081 * fTemp72) + (0.0067319093514117785 * fTemp73)) + (0.00075024801587290956 * fTemp76)))));
			output31[i] = FAUSTFLOAT((fRec0[0] * (((0.025630902468550189 * fTemp14) + ((1.2484819810925041e-18 * fTemp2) + ((9.9878558487400326e-18 * fTemp112) + ((0.035191232193238602 * fTemp78) + ((0.00096856602366710877 * fTemp90) + ((0.00096856602366710812 * fTemp89) + ((0.026072572806195689 * fTemp88) + ((3.5312403000788995e-18 * fTemp87) + ((0.020389258061373223 * fTemp86) + ((((0.025630902468550199 * fTemp62) + (0.026072572806195678 * fTemp64)) + (0.0029056980710013254 * fTemp68)) + (0.035191232193238595 * fTemp70))))))))))) - ((0.025630902468550196 * fTemp12) + ((3.745445943277512e-18 * fTemp4) + ((0.0029056980710013246 * fTemp151) + ((0.01922317685141265 * fTemp143) + (((0.00096856602366710779 * fTemp79) + ((0.00096856602366710855 * fTemp82) + ((0.026072572806195685 * fTemp83) + ((1.1770801000262998e-18 * fTemp85) + ((0.026072572806195675 * fTemp67) + (0.0029056980710013241 * fTemp69)))))) + (0.035191232193238609 * fTemp91)))))))));
			output32[i] = FAUSTFLOAT((fRec0[0] * (((4.6894725144107197e-33 * fTemp145) + ((9.3789450288214403e-34 * fTemp146) + ((0.033906496904967637 * fTemp147) + ((((((8.0904400869483595e-34 * fTemp71) + (0.027334238154345495 * fTemp120)) + (0.027334238154345498 * fTemp124)) + (0.027334238154345485 * fTemp127)) + (0.027334238154345505 * fTemp118)) + (1.2553045223568977e-17 * fTemp128))))) - ((6.5935211231487317e-18 * fTemp148) + ((7.5318271341413852e-18 * fTemp149) + ((6.5652615201750074e-33 * fTemp121) + ((2.8136835086464319e-33 * fTemp125) + (((((0.027334238154345478 * fTemp107) + (0.027334238154345474 * fTemp102)) + (0.027334238154345481 * fTemp108)) + (0.02733423815434545 * fTemp109)) + (1.7574263312996565e-17 * fTemp110)))))))));
			output33[i] = FAUSTFLOAT((fRec0[0] * (((0.027684538794142618 * fTemp14) + ((0.027684538794142621 * fTemp11) + ((4.0455439930475322e-18 * fTemp2) + ((0.048447942889749587 * fTemp143) + ((4.0448961198413007e-50 * fTemp112) + ((((0.0045334071240427463 * fTemp82) + ((0.028161597282885704 * fTemp88) + ((2.6699228183342524e-17 * fTemp87) + (((0.028161597282885746 * fTemp64) + (0.0031385126243372767 * fTemp57)) + (0.0031385126243372802 * fTemp68))))) + (0.0045334071240427437 * fTemp79)) + (0.012205326872422675 * fTemp91))))))) - ((0.027684538794142587 * fTemp13) + ((0.027684538794142632 * fTemp12) + ((1.2136631979142595e-17 * fTemp4) + ((0.012205326872422696 * fTemp78) + ((0.0045334071240427471 * fTemp90) + ((0.0045334071240427445 * fTemp89) + ((0.028161597282885739 * fTemp83) + ((8.8997427277808398e-18 * fTemp85) + ((0.022022915754780329 * fTemp86) + ((((0.028161597282885732 * fTemp67) + (0.0031385126243372789 * fTemp65)) + (0.0031385126243372746 * fTemp69)) + (0.012205326872422689 * fTemp70))))))))))))));
			output34[i] = FAUSTFLOAT((fRec0[0] * (((0.010356563277706246 * fTemp118) + ((0.010356563277706192 * fTemp127) + ((0.010356563277706229 * fTemp124) + ((0.010356563277706213 * fTemp120) + ((((((1.3135133594325083e-66 * fTemp71) + (0.029363887673040441 * fTemp72)) + (0.010356563277706147 * fTemp107)) + (0.01035656327770614 * fTemp102)) + (0.010356563277706175 * fTemp108)) + (0.01035656327770606 * fTemp109)))))) - ((0.039826512815546306 * fTemp73) + (0.0044385271190169251 * fTemp76)))));
			output35[i] = FAUSTFLOAT((fRec0[0] * (((0.037142706401659779 * fTemp13) + ((0.037142706401659731 * fTemp12) + ((9.0461113744202184e-18 * fTemp2) + ((0.036961085234966218 * ((((((fTemp152 - fTemp41) + fTemp42) + fTemp45) - fTemp46) - fTemp49) + fTemp50)) + ((0.0092856766004149483 * fTemp143) + ((0.00046786183841728994 * fTemp132) + ((2.5434040020076175e-83 * fTemp112) + (((0.012594249175901169 * fTemp88) + ((2.8429185316539108e-18 * fTemp85) + ((0.029546842014263944 * fTemp86) + (((0.012594249175901155 * fTemp64) + (0.0014035855152519582 * fTemp57)) + (0.0014035855152519131 * fTemp68))))) + (0.00046786183841729053 * fTemp91))))))))) - ((0.03714270640165971 * fTemp14) + ((0.037142706401659786 * fTemp11) + ((7.962406901797708e-17 * fTemp4) + ((0.00046786183841729048 * fTemp133) + ((0.012594249175901174 * fTemp83) + ((2.5023430732567692e-17 * fTemp87) + ((((0.012594249175901146 * fTemp67) + (0.0014035855152519602 * fTemp65)) + (0.0014035855152521122 * fTemp69)) + (0.00046786183841728967 * fTemp70)))))))))));
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
