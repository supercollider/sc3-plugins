/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2016"
license: "GPL"
name: "HOABeamHCardio2HOA2"
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


double sin(double dummy0);
double cos(double dummy0);
double mydsp_faustpower2_f(double value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
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
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/cijk.lib/author", "Pierre Lecomte");
		m->declare("lib/cijk.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/cijk.lib/license", "GPL");
		m->declare("lib/cijk.lib/name", "Cijk matrix");
		m->declare("lib/cijk.lib/version", "10");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
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
		m->declare("name", "HOABeamHCardio2HOA2");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "0.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 9;
		
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
		ui_interface->declare(0, "3", "");
		ui_interface->openHorizontalBox("Parameters");
		ui_interface->declare(&fHslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth", &fHslider1, 0.0, -3.1415926535897931, 3.1415926535897931, 0.10000000000000001);
		ui_interface->declare(&fHslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation", &fHslider2, 0.0, -1.5707963267948966, 1.5707963267948966, 0.10000000000000001);
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Order", &fHslider0, 0.0, 0.0, 2.0, 1.0);
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
		int iSlow0 = int(double(fHslider0));
		int iSlow1 = (iSlow0 >= 2);
		int iSlow2 = (iSlow0 >= 1);
		double fSlow3 = double(fHslider1);
		double fSlow4 = sin(fSlow3);
		double fSlow5 = double(fHslider2);
		double fSlow6 = cos(fSlow5);
		double fSlow7 = (fSlow4 * fSlow6);
		double fSlow8 = sin(fSlow5);
		double fSlow9 = cos(fSlow3);
		double fSlow10 = (fSlow9 * fSlow6);
		double fSlow11 = ((3.0 * mydsp_faustpower2_f(fSlow8)) + -1.0);
		double fSlow12 = (0.124224 * fSlow11);
		double fSlow13 = (2.0 * fSlow3);
		double fSlow14 = sin(fSlow13);
		double fSlow15 = mydsp_faustpower2_f(fSlow6);
		double fSlow16 = (fSlow14 * fSlow15);
		double fSlow17 = sin((2.0 * fSlow5));
		double fSlow18 = (fSlow4 * fSlow17);
		double fSlow19 = (fSlow9 * fSlow17);
		double fSlow20 = cos(fSlow13);
		double fSlow21 = (fSlow20 * fSlow15);
		double fSlow22 = (0.43301700000000004 * fSlow4);
		double fSlow23 = (0.335413669149 * fSlow9);
		double fSlow24 = (0.335413669149 * fSlow8);
		double fSlow25 = (0.0 - (0.193651264638 * fSlow7));
		double fSlow26 = (0.0 - (0.335413669149 * fSlow7));
		double fSlow27 = (0.14907119265000002 * fSlow9);
		double fSlow28 = (fSlow14 * fSlow6);
		double fSlow29 = (0.16666405622891672 * fSlow28);
		double fSlow30 = (0.19245000000000004 * fSlow6);
		double fSlow31 = (0.16666405622891672 * fSlow17);
		double fSlow32 = (0.055554711935999999 * fSlow11);
		double fSlow33 = (0.16666405622891672 * fSlow21);
		double fSlow34 = (0.11112 - (fSlow32 + fSlow33));
		double fSlow35 = (0.14907119265000002 * fSlow8);
		double fSlow36 = (0.0 - (0.086066334300000019 * fSlow7));
		double fSlow37 = (0.0 - (0.14907119265000002 * fSlow7));
		double fSlow38 = (0.335413669149 * fSlow6);
		double fSlow39 = ((0.111109299648 * fSlow11) + 0.11112);
		double fSlow40 = (0.14907119265000002 * fSlow6);
		double fSlow41 = (0.43301700000000004 * fSlow9);
		double fSlow42 = (0.335413669149 * fSlow4);
		double fSlow43 = (0.0 - (0.193651264638 * fSlow10));
		double fSlow44 = (0.0 - (0.086066334300000019 * fSlow10));
		double fSlow45 = (0.19245000000000004 * fSlow9);
		double fSlow46 = (0.14907119265000002 * fSlow4);
		double fSlow47 = (fSlow33 + (0.11112 - fSlow32));
		double fSlow48 = (0.11904563149935221 * fSlow17);
		double fSlow49 = (0.2151622795194362 * fSlow28);
		double fSlow50 = (0.079363856447999995 * fSlow11);
		double fSlow51 = (0.11112 - fSlow50);
		double fSlow52 = (0.0 - (0.13746223165253885 * fSlow16));
		double fSlow53 = (0.2151622795194362 * fSlow17);
		double fSlow54 = (0.068731008245129663 * fSlow17);
		double fSlow55 = (0.039681866111999996 * fSlow11);
		double fSlow56 = (0.11904563149935221 * fSlow21);
		double fSlow57 = (fSlow55 + (0.11112 - fSlow56));
		double fSlow58 = (0.0 - (0.11904563149935221 * fSlow18));
		double fSlow59 = (0.38730209625899997 * fSlow8);
		double fSlow60 = (0.17213247615000002 * fSlow8);
		double fSlow61 = (fSlow50 + 0.11112);
		double fSlow62 = (0.0 - (0.13746223165253885 * fSlow21));
		double fSlow63 = ((fSlow55 + fSlow56) + 0.11112);
		double fSlow64 = (0.335413669149 * fSlow10);
		double fSlow65 = (fSlow20 * fSlow6);
		double fSlow66 = (0.2151622795194362 * fSlow65);
		double fSlow67 = (0.11904563149935221 * fSlow19);
		double fSlow68 = (0.34718610232800001 * fSlow6);
		double fSlow69 = (0.15430333080000003 * fSlow9);
		double fSlow70 = (0.17251367312221164 * fSlow6);
		double fSlow71 = (0.15430333080000003 * fSlow4);
		double fSlow72 = (0.0 - (0.089643179340000004 * fSlow10));
		double fSlow73 = (0.35857228434300004 * fSlow8);
		double fSlow74 = (0.0 - (0.039840999000000009 * fSlow10));
		double fSlow75 = (0.0 - (0.044542895106113686 * fSlow16));
		double fSlow76 = (0.089085626687999991 * fSlow11);
		double fSlow77 = (0.044542895106113686 * fSlow21);
		double fSlow78 = (fSlow76 + fSlow77);
		double fSlow79 = (0.15936380355000002 * fSlow8);
		double fSlow80 = (0.1781713652621752 * fSlow17);
		double fSlow81 = (0.13801301565000001 * fSlow6);
		double fSlow82 = (0.039840999000000009 * fSlow6);
		double fSlow83 = (0.0 - (0.219579889581 * fSlow7));
		double fSlow84 = (0.38032316126999999 * fSlow8);
		double fSlow85 = (0.0 - (0.219579889581 * fSlow10));
		double fSlow86 = (0.0 - (0.097590047850000028 * fSlow7));
		double fSlow87 = (0.0 - (0.10910728580834947 * fSlow19));
		double fSlow88 = (0.10910718143999999 * fSlow11);
		double fSlow89 = (0.0 - (0.10910728580834947 * fSlow18));
		double fSlow90 = (0.16903075950000004 * fSlow8);
		double fSlow91 = (0.0 - (0.097590047850000028 * fSlow10));
		double fSlow92 = (0.31053251232900003 * fSlow10);
		double fSlow93 = (0.0 - (0.089643179340000004 * fSlow7));
		double fSlow94 = (0.0 - (0.039840999000000009 * fSlow7));
		double fSlow95 = (fSlow76 - fSlow77);
		double fSlow96 = (0.0 - (0.28347631111800004 * fSlow7));
		double fSlow97 = (0.0 - (0.12598816230000001 * fSlow7));
		double fSlow98 = (0.0 - (0.14085684693651698 * fSlow18));
		double fSlow99 = (0.0 - (0.34718610232800001 * fSlow7));
		double fSlow100 = (0.34718610232800001 * fSlow10);
		double fSlow101 = (0.17251367312221164 * fSlow65);
		double fSlow102 = (0.0 - (0.17251367312221164 * fSlow16));
		double fSlow103 = (0.0 - (0.15430333080000003 * fSlow7));
		double fSlow104 = (0.18184526118496958 * fSlow15);
		double fSlow105 = (0.11904563149935221 * fSlow16);
		double fSlow106 = (0.068731027392000002 * fSlow11);
		double fSlow107 = (0.13746223165253885 * fSlow17);
		double fSlow108 = (0.0 - (0.048600210211011685 * fSlow19));
		double fSlow109 = (0.097200435264000004 * fSlow11);
		double fSlow110 = (0.048600210211011685 * fSlow21);
		double fSlow111 = (fSlow109 + fSlow110);
		double fSlow112 = (0.0 - (0.048600210211011685 * fSlow16));
		double fSlow113 = (0.030737437765308104 * fSlow15);
		double fSlow114 = (0.0 - (0.12294996622351191 * fSlow19));
		double fSlow115 = (0.0 - (0.12294996622351191 * fSlow18));
		double fSlow116 = (0.10647773203199999 * fSlow11);
		double fSlow117 = (0.0 - (0.048600210211011685 * fSlow18));
		double fSlow118 = (0.16835609239505758 * fSlow19);
		double fSlow119 = (fSlow109 - fSlow110);
		double fSlow120 = (0.0 - (0.13746223165253885 * fSlow18));
		double fSlow121 = (0.13746223165253885 * fSlow19);
		double fSlow122 = (0.0 - (0.12858399051272834 * fSlow18));
		double fSlow123 = (0.0 - (0.12858399051272834 * fSlow16));
		double fSlow124 = (0.0 - (0.18184526118496958 * fSlow16));
		double fSlow125 = (0.18184526118496958 * fSlow21);
		for (int i = 0; (i < count); i = (i + 1)) {
			double fTemp0 = double(input0[i]);
			double fTemp1 = double(input1[i]);
			double fTemp2 = double(input2[i]);
			double fTemp3 = double(input3[i]);
			double fTemp4 = (((fSlow7 * fTemp1) + (fSlow8 * fTemp2)) + (fSlow10 * fTemp3));
			double fTemp5 = double(input6[i]);
			double fTemp6 = double(input4[i]);
			double fTemp7 = double(input5[i]);
			double fTemp8 = double(input7[i]);
			double fTemp9 = double(input8[i]);
			output0[i] = FAUSTFLOAT((iSlow1?((0.11112 * fTemp0) + (((0.19245000000000004 * fTemp4) + (fSlow12 * fTemp5)) + (0.2151622795194362 * ((((fSlow16 * fTemp6) + (fSlow18 * fTemp7)) + (fSlow19 * fTemp8)) + (fSlow21 * fTemp9))))):(iSlow2?((0.24993000000000001 * fTemp0) + (0.43301700000000004 * fTemp4)):fTemp0)));
			double fTemp10 = (fSlow31 * fTemp2);
			output1[i] = FAUSTFLOAT((iSlow1?((((fSlow6 * ((fSlow27 * fTemp6) + (fSlow29 * fTemp3))) + ((fSlow4 * ((fSlow30 * fTemp0) + fTemp10)) + ((fSlow34 * fTemp1) + (fSlow35 * fTemp7)))) + (fSlow36 * fTemp5)) + (fSlow37 * fTemp9)):(iSlow2?((0.24993000000000001 * fTemp1) + ((((fSlow6 * ((fSlow22 * fTemp0) + (fSlow23 * fTemp6))) + (fSlow24 * fTemp7)) + (fSlow25 * fTemp5)) + (fSlow26 * fTemp9))):fTemp1)));
			double fTemp11 = ((fSlow4 * fTemp7) + (fSlow9 * fTemp8));
			output2[i] = FAUSTFLOAT((iSlow1?((fSlow8 * ((0.19245000000000004 * fTemp0) + (0.17213247615000002 * fTemp5))) + ((fSlow31 * ((fSlow4 * fTemp1) + (fSlow9 * fTemp3))) + ((fSlow39 * fTemp2) + (fSlow40 * fTemp11)))):(iSlow2?((0.24993000000000001 * fTemp2) + ((fSlow8 * ((0.43301700000000004 * fTemp0) + (0.38730209625899997 * fTemp5))) + (fSlow38 * fTemp11))):fTemp2)));
			output3[i] = FAUSTFLOAT((iSlow1?((fSlow9 * (fTemp10 + (fSlow40 * fTemp9))) + ((fSlow44 * fTemp5) + ((fSlow6 * (((fSlow45 * fTemp0) + (fSlow29 * fTemp1)) + (fSlow46 * fTemp6))) + ((fSlow47 * fTemp3) + (fSlow35 * fTemp8))))):(iSlow2?((0.24993000000000001 * fTemp3) + ((fSlow6 * (((fSlow41 * fTemp0) + (fSlow42 * fTemp6)) + (fSlow23 * fTemp9))) + ((fSlow43 * fTemp5) + (fSlow24 * fTemp8)))):fTemp3)));
			double fTemp12 = ((fSlow9 * fTemp7) + (fSlow4 * fTemp8));
			output4[i] = FAUSTFLOAT((iSlow1?((fSlow48 * fTemp12) + (((fSlow6 * (((fSlow27 * fTemp1) + (fSlow49 * fTemp0)) + (fSlow46 * fTemp3))) + (fSlow51 * fTemp6)) + (fSlow52 * fTemp5))):(iSlow2?((fSlow38 * ((fSlow9 * fTemp1) + (fSlow4 * fTemp3))) + (0.24993000000000001 * fTemp6)):fTemp6)));
			double fTemp13 = (fSlow19 * fTemp6);
			double fTemp14 = (fSlow16 * fTemp8);
			double fTemp15 = (((fSlow40 * fTemp2) + (fSlow53 * fTemp0)) + (fSlow54 * fTemp5));
			output5[i] = FAUSTFLOAT((iSlow1?(((0.11904563149935221 * (fTemp13 + fTemp14)) + ((fSlow4 * fTemp15) + ((fSlow35 * fTemp1) + (fSlow57 * fTemp7)))) + (fSlow58 * fTemp9)):(iSlow2?((0.335413669149 * ((fSlow8 * fTemp1) + (fSlow7 * fTemp2))) + (0.24993000000000001 * fTemp7)):fTemp7)));
			output6[i] = FAUSTFLOAT((iSlow1?(((fSlow54 * fTemp11) + (((((fSlow36 * fTemp1) + ((fSlow12 * fTemp0) + (fSlow60 * fTemp2))) + (fSlow44 * fTemp3)) + (fSlow52 * fTemp6)) + (fSlow61 * fTemp5))) + (fSlow62 * fTemp9)):(iSlow2?((((fSlow25 * fTemp1) + (fSlow59 * fTemp2)) + (fSlow43 * fTemp3)) + (0.24993000000000001 * fTemp5)):fTemp5)));
			output7[i] = FAUSTFLOAT((iSlow1?((fSlow9 * (fTemp15 + (fSlow48 * fTemp9))) + (((fSlow35 * fTemp3) + (0.11904563149935221 * ((fSlow18 * fTemp6) + (fSlow16 * fTemp7)))) + (fSlow63 * fTemp8))):(iSlow2?((0.335413669149 * ((fSlow10 * fTemp2) + (fSlow8 * fTemp3))) + (0.24993000000000001 * fTemp8)):fTemp8)));
			output8[i] = FAUSTFLOAT((iSlow1?(((fSlow62 * fTemp5) + ((fSlow58 * fTemp7) + ((fSlow6 * ((fSlow27 * fTemp3) + (fSlow66 * fTemp0))) + ((fSlow37 * fTemp1) + (fSlow67 * fTemp8))))) + (fSlow51 * fTemp9)):(iSlow2?(((fSlow26 * fTemp1) + (fSlow64 * fTemp3)) + (0.24993000000000001 * fTemp9)):fTemp9)));
			output9[i] = FAUSTFLOAT((iSlow1?(fSlow6 * (((fSlow69 * fTemp6) + (fSlow70 * ((fSlow14 * fTemp3) + (fSlow20 * fTemp1)))) + (fSlow71 * fTemp9))):(iSlow2?(fSlow68 * ((fSlow9 * fTemp6) + (fSlow4 * fTemp9))):0.0)));
			double fTemp16 = (((fSlow8 * fTemp6) + (fSlow10 * fTemp7)) + (fSlow7 * fTemp8));
			output10[i] = FAUSTFLOAT((iSlow1?((0.14085684693651698 * (((fSlow19 * fTemp1) + (fSlow16 * fTemp2)) + (fSlow18 * fTemp3))) + (0.12598816230000001 * fTemp16)):(iSlow2?(0.28347631111800004 * fTemp16):0.0)));
			double fTemp17 = ((fSlow80 * fTemp2) + (fSlow81 * fTemp5));
			output11[i] = FAUSTFLOAT((iSlow1?(((fSlow74 * fTemp6) + ((fSlow75 * fTemp3) + ((fSlow78 * fTemp1) + (fSlow79 * fTemp7)))) + (fSlow4 * (fTemp17 + (fSlow82 * fTemp9)))):(iSlow2?(((fSlow72 * fTemp6) + (fSlow73 * fTemp7)) + (fSlow7 * ((0.31053251232900003 * fTemp5) + (0.089643179340000004 * fTemp9)))):0.0)));
			output12[i] = FAUSTFLOAT((iSlow1?(((fSlow86 * fTemp7) + ((fSlow87 * fTemp3) + ((fSlow88 * fTemp2) + ((fSlow89 * fTemp1) + (fSlow90 * fTemp5))))) + (fSlow91 * fTemp8)):(iSlow2?(((fSlow83 * fTemp7) + (fSlow84 * fTemp5)) + (fSlow85 * fTemp8)):0.0)));
			output13[i] = FAUSTFLOAT((iSlow1?(((fSlow9 * fTemp17) + ((fSlow94 * fTemp6) + ((fSlow95 * fTemp3) + ((fSlow75 * fTemp1) + (fSlow79 * fTemp8))))) + (fSlow74 * fTemp9)):(iSlow2?(((fSlow92 * fTemp5) + ((fSlow93 * fTemp6) + (fSlow73 * fTemp8))) + (fSlow72 * fTemp9)):0.0)));
			double fTemp18 = ((fSlow10 * fTemp8) + (fSlow8 * fTemp9));
			output14[i] = FAUSTFLOAT((iSlow1?((fSlow97 * fTemp7) + ((0.14085684693651698 * ((fSlow19 * fTemp3) + (fSlow21 * fTemp2))) + ((fSlow98 * fTemp1) + (0.12598816230000001 * fTemp18)))):(iSlow2?((fSlow96 * fTemp7) + (0.28347631111800004 * fTemp18)):0.0)));
			output15[i] = FAUSTFLOAT((iSlow1?((fSlow6 * ((fSlow69 * fTemp9) + (fSlow101 * fTemp3))) + ((fSlow102 * fTemp1) + (fSlow103 * fTemp6))):(iSlow2?((fSlow99 * fTemp6) + (fSlow100 * fTemp9)):0.0)));
			output16[i] = FAUSTFLOAT((iSlow1?(fSlow104 * ((fSlow20 * fTemp6) + (fSlow14 * fTemp9))):0.0));
			output17[i] = FAUSTFLOAT((iSlow1?(0.12858399051272834 * ((fTemp14 + (fTemp13 + (fSlow21 * fTemp7))) + (fSlow18 * fTemp9))):0.0));
			output18[i] = FAUSTFLOAT((iSlow1?((fSlow105 * fTemp5) + ((fSlow106 * fTemp6) + (fSlow107 * fTemp12))):0.0));
			output19[i] = FAUSTFLOAT((iSlow1?((fSlow18 * ((0.16835609239505758 * fTemp5) + (0.048600210211011685 * fTemp9))) + (((fSlow108 * fTemp6) + (fSlow111 * fTemp7)) + (fSlow112 * fTemp8))):0.0));
			output20[i] = FAUSTFLOAT((iSlow1?((fSlow113 * ((fSlow14 * fTemp6) + (fSlow20 * fTemp9))) + ((fSlow114 * fTemp8) + ((fSlow115 * fTemp7) + (fSlow116 * fTemp5)))):0.0));
			output21[i] = FAUSTFLOAT((iSlow1?((((fSlow112 * fTemp7) + ((fSlow117 * fTemp6) + (fSlow118 * fTemp5))) + (fSlow119 * fTemp8)) + (fSlow108 * fTemp9)):0.0));
			output22[i] = FAUSTFLOAT((iSlow1?(((fSlow56 * fTemp5) + ((fSlow120 * fTemp7) + (fSlow121 * fTemp8))) + (fSlow106 * fTemp9)):0.0));
			output23[i] = FAUSTFLOAT((iSlow1?(((fSlow122 * fTemp6) + (fSlow123 * fTemp7)) + (0.12858399051272834 * ((fSlow19 * fTemp9) + (fSlow21 * fTemp8)))):0.0));
			output24[i] = FAUSTFLOAT((iSlow1?((fSlow124 * fTemp6) + (fSlow125 * fTemp9)):0.0));
			
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