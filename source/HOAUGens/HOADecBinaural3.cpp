/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2015"
license: "GPL)"
name: "HOADecBinaural3"
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
#include <math.h>


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif
#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp : public dsp {
	
 private:
	
	int fSamplingFreq;
	double fConst0;
	FAUSTFLOAT fVslider0;
	double fRec1[2];
	FAUSTFLOAT fVslider1;
	double fRec3[2];
	double fRec2[2];
	FAUSTFLOAT fVbargraph0;
	int IOTA;
	double fVec0[128];
	double fRec4[2];
	FAUSTFLOAT fVbargraph1;
	double fVec1[128];
	double fRec5[2];
	FAUSTFLOAT fVbargraph2;
	double fVec2[128];
	double fRec6[2];
	FAUSTFLOAT fVbargraph3;
	double fVec3[128];
	double fRec7[2];
	FAUSTFLOAT fVbargraph4;
	double fVec4[128];
	double fRec8[2];
	FAUSTFLOAT fVbargraph5;
	double fVec5[128];
	double fRec9[2];
	FAUSTFLOAT fVbargraph6;
	double fVec6[128];
	double fRec10[2];
	FAUSTFLOAT fVbargraph7;
	double fVec7[128];
	double fRec11[2];
	FAUSTFLOAT fVbargraph8;
	double fVec8[128];
	double fRec12[2];
	FAUSTFLOAT fVbargraph9;
	double fVec9[128];
	double fRec13[2];
	FAUSTFLOAT fVbargraph10;
	double fVec10[128];
	double fRec14[2];
	FAUSTFLOAT fVbargraph11;
	double fVec11[128];
	double fRec15[2];
	FAUSTFLOAT fVbargraph12;
	double fVec12[128];
	double fRec16[2];
	FAUSTFLOAT fVbargraph13;
	double fVec13[128];
	double fRec17[2];
	FAUSTFLOAT fVbargraph14;
	double fVec14[128];
	double fRec18[2];
	FAUSTFLOAT fVbargraph15;
	double fVec15[128];
	double fRec0[2];
	FAUSTFLOAT fHbargraph0;
	double fRec19[2];
	FAUSTFLOAT fHbargraph1;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2015");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("gui.lib/author", "Pierre Lecomte");
		m->declare("gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("gui.lib/license", "GPL");
		m->declare("gui.lib/name", "GUI Library");
		m->declare("gui.lib/version", "1.0");
		m->declare("license", "GPL)");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("name", "HOADecBinaural3");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 16;
		
	}
	virtual int getNumOutputs() {
		return 2;
		
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
		fConst0 = (80.0 / min(192000.0, max(1.0, double(fSamplingFreq))));
		
	}
	
	virtual void instanceResetUserInterface() {
		fVslider0 = FAUSTFLOAT(0.0);
		fVslider1 = FAUSTFLOAT(0.0);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec1[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec3[l1] = 0.0;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec2[l2] = 0.0;
			
		}
		IOTA = 0;
		for (int l3 = 0; (l3 < 128); l3 = (l3 + 1)) {
			fVec0[l3] = 0.0;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec4[l4] = 0.0;
			
		}
		for (int l5 = 0; (l5 < 128); l5 = (l5 + 1)) {
			fVec1[l5] = 0.0;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec5[l6] = 0.0;
			
		}
		for (int l7 = 0; (l7 < 128); l7 = (l7 + 1)) {
			fVec2[l7] = 0.0;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec6[l8] = 0.0;
			
		}
		for (int l9 = 0; (l9 < 128); l9 = (l9 + 1)) {
			fVec3[l9] = 0.0;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec7[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 128); l11 = (l11 + 1)) {
			fVec4[l11] = 0.0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec8[l12] = 0.0;
			
		}
		for (int l13 = 0; (l13 < 128); l13 = (l13 + 1)) {
			fVec5[l13] = 0.0;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec9[l14] = 0.0;
			
		}
		for (int l15 = 0; (l15 < 128); l15 = (l15 + 1)) {
			fVec6[l15] = 0.0;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fRec10[l16] = 0.0;
			
		}
		for (int l17 = 0; (l17 < 128); l17 = (l17 + 1)) {
			fVec7[l17] = 0.0;
			
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec11[l18] = 0.0;
			
		}
		for (int l19 = 0; (l19 < 128); l19 = (l19 + 1)) {
			fVec8[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec12[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 128); l21 = (l21 + 1)) {
			fVec9[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec13[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 128); l23 = (l23 + 1)) {
			fVec10[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec14[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 128); l25 = (l25 + 1)) {
			fVec11[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec15[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 128); l27 = (l27 + 1)) {
			fVec12[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec16[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 128); l29 = (l29 + 1)) {
			fVec13[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec17[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 128); l31 = (l31 + 1)) {
			fVec14[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec18[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 128); l33 = (l33 + 1)) {
			fVec15[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec0[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec19[l35] = 0.0;
			
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
		ui_interface->openVerticalBox("HOADecBinaural3");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x5671530", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x541fa40", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x562ef70", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x5608df0", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x55de970", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x55ae700", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x558aab0", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x55627b0", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x55435e0", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x551bb50", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x54f0900", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x54c8730", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x54a17d0", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x5478040", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x54244b0", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x542ab30", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(&fVslider1, "1", "");
		ui_interface->declare(&fVslider1, "osc", "/levelin -10 10");
		ui_interface->declare(&fVslider1, "unit", "dB");
		ui_interface->addVerticalSlider("Inputs Gain", &fVslider1, 0.0, -10.0, 10.0, 0.10000000000000001);
		ui_interface->declare(&fVslider0, "2", "");
		ui_interface->declare(&fVslider0, "osc", "/levelout -10 10");
		ui_interface->declare(&fVslider0, "unit", "dB");
		ui_interface->addVerticalSlider("Outputs Gain", &fVslider0, 0.0, -10.0, 10.0, 0.10000000000000001);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("Outputs");
		ui_interface->openHorizontalBox("Left");
		ui_interface->declare(&fHbargraph0, "unit", "dB");
		ui_interface->addHorizontalBargraph("0x599a1f0", &fHbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Right");
		ui_interface->declare(&fHbargraph1, "unit", "dB");
		ui_interface->addHorizontalBargraph("0x5c61110", &fHbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
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
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		double fSlow0 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider0))));
		double fSlow1 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider1))));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			fRec3[0] = (fSlow1 + (0.999 * fRec3[1]));
			double fTemp0 = (fRec3[0] * double(input1[i]));
			fRec2[0] = max((fRec2[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fVbargraph0 = FAUSTFLOAT(fRec2[0]);
			fVec0[(IOTA & 127)] = fTemp0;
			double fTemp1 = (fRec3[0] * double(input14[i]));
			fRec4[0] = max((fRec4[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp1))))));
			fVbargraph1 = FAUSTFLOAT(fRec4[0]);
			fVec1[(IOTA & 127)] = fTemp1;
			double fTemp2 = (fRec3[0] * double(input15[i]));
			fRec5[0] = max((fRec5[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp2))))));
			fVbargraph2 = FAUSTFLOAT(fRec5[0]);
			fVec2[(IOTA & 127)] = fTemp2;
			double fTemp3 = (fRec3[0] * double(input13[i]));
			fRec6[0] = max((fRec6[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp3))))));
			fVbargraph3 = FAUSTFLOAT(fRec6[0]);
			fVec3[(IOTA & 127)] = fTemp3;
			double fTemp4 = (fRec3[0] * double(input12[i]));
			fRec7[0] = max((fRec7[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fVbargraph4 = FAUSTFLOAT(fRec7[0]);
			fVec4[(IOTA & 127)] = fTemp4;
			double fTemp5 = (fRec3[0] * double(input11[i]));
			fRec8[0] = max((fRec8[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fVbargraph5 = FAUSTFLOAT(fRec8[0]);
			fVec5[(IOTA & 127)] = fTemp5;
			double fTemp6 = (fRec3[0] * double(input10[i]));
			fRec9[0] = max((fRec9[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fVbargraph6 = FAUSTFLOAT(fRec9[0]);
			fVec6[(IOTA & 127)] = fTemp6;
			double fTemp7 = (fRec3[0] * double(input9[i]));
			fRec10[0] = max((fRec10[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp7))))));
			fVbargraph7 = FAUSTFLOAT(fRec10[0]);
			fVec7[(IOTA & 127)] = fTemp7;
			double fTemp8 = (fRec3[0] * double(input8[i]));
			fRec11[0] = max((fRec11[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fVbargraph8 = FAUSTFLOAT(fRec11[0]);
			fVec8[(IOTA & 127)] = fTemp8;
			double fTemp9 = (fRec3[0] * double(input7[i]));
			fRec12[0] = max((fRec12[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph9 = FAUSTFLOAT(fRec12[0]);
			fVec9[(IOTA & 127)] = fTemp9;
			double fTemp10 = (fRec3[0] * double(input6[i]));
			fRec13[0] = max((fRec13[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fVbargraph10 = FAUSTFLOAT(fRec13[0]);
			fVec10[(IOTA & 127)] = fTemp10;
			double fTemp11 = (fRec3[0] * double(input5[i]));
			fRec14[0] = max((fRec14[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fVbargraph11 = FAUSTFLOAT(fRec14[0]);
			fVec11[(IOTA & 127)] = fTemp11;
			double fTemp12 = (fRec3[0] * double(input4[i]));
			fRec15[0] = max((fRec15[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph12 = FAUSTFLOAT(fRec15[0]);
			fVec12[(IOTA & 127)] = fTemp12;
			double fTemp13 = (fRec3[0] * double(input3[i]));
			fRec16[0] = max((fRec16[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fVbargraph13 = FAUSTFLOAT(fRec16[0]);
			fVec13[(IOTA & 127)] = fTemp13;
			double fTemp14 = (fRec3[0] * double(input2[i]));
			fRec17[0] = max((fRec17[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fVbargraph14 = FAUSTFLOAT(fRec17[0]);
			fVec14[(IOTA & 127)] = fTemp14;
			double fTemp15 = (fRec3[0] * double(input0[i]));
			fRec18[0] = max((fRec18[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph15 = FAUSTFLOAT(fRec18[0]);
			fVec15[(IOTA & 127)] = fTemp15;
			double fTemp16 = (fRec1[0] * (((0.00010376700000000001 * (fVec0[((IOTA - 71) & 127)] - fVec1[((IOTA - 119) & 127)])) + ((1.4034199999999999e-06 * fVec2[((IOTA - 126) & 127)]) + ((1.2211099999999999e-06 * fVec2[((IOTA - 125) & 127)]) + ((1.2893400000000001e-05 * fVec2[((IOTA - 124) & 127)]) + ((2.6002200000000003e-05 * fVec2[((IOTA - 123) & 127)]) + ((2.6415500000000001e-05 * fVec2[((IOTA - 122) & 127)]) + ((2.8177200000000002e-05 * fVec2[((IOTA - 121) & 127)]) + ((1.2612900000000001e-05 * fVec2[((IOTA - 120) & 127)]) + ((0.00041671500000000006 * fVec2[((IOTA - 104) & 127)]) + ((0.00031062500000000003 * fVec2[((IOTA - 100) & 127)]) + ((7.3607000000000002e-05 * fVec2[((IOTA - 99) & 127)]) + ((0.00050056999999999999 * fVec2[((IOTA - 96) & 127)]) + ((0.0027204999999999998 * fVec2[((IOTA - 95) & 127)]) + ((0.0020446800000000001 * fVec2[((IOTA - 94) & 127)]) + ((8.3015299999999998e-05 * fVec2[((IOTA - 93) & 127)]) + ((0.0022225799999999996 * fVec2[((IOTA - 92) & 127)]) + ((0.0024016700000000003 * fVec2[((IOTA - 91) & 127)]) + ((0.00063865800000000006 * fVec2[((IOTA - 86) & 127)]) + ((0.00058693299999999999 * fVec2[((IOTA - 84) & 127)]) + ((0.0010148100000000001 * fVec2[((IOTA - 79) & 127)]) + ((0.00042316100000000002 * fVec2[((IOTA - 80) & 127)]) + ((0.00028691900000000001 * fVec2[((IOTA - 73) & 127)]) + ((0.0021912699999999999 * fVec2[((IOTA - 72) & 127)]) + ((0.00113353 * fVec2[((IOTA - 71) & 127)]) + ((0.000557951 * fVec2[((IOTA - 66) & 127)]) + ((0.00043863399999999997 * fVec2[((IOTA - 63) & 127)]) + ((0.00314139 * fVec2[((IOTA - 62) & 127)]) + ((0.0052284000000000002 * fVec2[((IOTA - 61) & 127)]) + ((0.0025010699999999998 * fVec2[((IOTA - 60) & 127)]) + ((0.0012393300000000001 * fVec2[((IOTA - 57) & 127)]) + ((0.0018594100000000001 * fVec2[((IOTA - 54) & 127)]) + ((0.0034922200000000003 * fVec2[((IOTA - 53) & 127)]) + ((0.00127944 * fVec2[((IOTA - 52) & 127)]) + ((0.0016422699999999999 * fVec2[((IOTA - 51) & 127)]) + ((0.00289419 * fVec2[((IOTA - 50) & 127)]) + ((0.0053105499999999998 * fVec2[((IOTA - 49) & 127)]) + ((0.0065292000000000006 * fVec2[((IOTA - 48) & 127)]) + ((0.00110392 * fVec2[((IOTA - 47) & 127)]) + ((0.0039351999999999998 * fVec2[((IOTA - 42) & 127)]) + ((0.00132128 * fVec2[((IOTA - 41) & 127)]) + ((0.0068892700000000003 * fVec2[((IOTA - 39) & 127)]) + ((0.0013770800000000001 * fVec2[((IOTA - 38) & 127)]) + ((0.031043400000000002 * fVec2[((IOTA - 30) & 127)]) + ((0.0012180400000000001 * fVec2[((IOTA - 29) & 127)]) + ((0.026485700000000001 * fVec2[((IOTA - 26) & 127)]) + ((0.026438899999999998 * fVec2[((IOTA - 23) & 127)]) + ((0.0094311100000000012 * fVec2[((IOTA - 22) & 127)]) + ((0.0253549 * fVec2[((IOTA - 21) & 127)]) + ((0.034643899999999998 * fVec2[((IOTA - 20) & 127)]) + ((0.0140976 * fVec2[((IOTA - 19) & 127)]) + ((0.00406018 * fVec2[((IOTA - 16) & 127)]) + ((0.019651700000000001 * fVec2[((IOTA - 12) & 127)]) + ((0.0098021300000000009 * fVec2[((IOTA - 11) & 127)]) + ((0.00308077 * fVec2[((IOTA - 9) & 127)]) + ((0.00323953 * fVec2[((IOTA - 7) & 127)]) + ((0.0012123500000000001 * fVec2[((IOTA - 5) & 127)]) + (((4.4620600000000002e-07 * fVec1[((IOTA - 126) & 127)]) + ((2.9040900000000002e-05 * fVec1[((IOTA - 122) & 127)]) + ((3.3390499999999998e-05 * fVec1[((IOTA - 121) & 127)]) + ((2.7818400000000001e-05 * fVec1[((IOTA - 118) & 127)]) + ((1.2691900000000001e-05 * fVec1[((IOTA - 113) & 127)]) + ((0.00037104300000000005 * fVec1[((IOTA - 111) & 127)]) + ((0.00041838800000000006 * fVec1[((IOTA - 110) & 127)]) + ((0.0025975199999999999 * fVec1[((IOTA - 92) & 127)]) + ((0.00015330900000000002 * fVec1[((IOTA - 91) & 127)]) + ((0.00154597 * fVec1[((IOTA - 89) & 127)]) + ((0.00067549000000000005 * fVec1[((IOTA - 88) & 127)]) + ((0.00107274 * fVec1[((IOTA - 85) & 127)]) + ((0.00062359600000000002 * fVec1[((IOTA - 80) & 127)]) + ((0.000178293 * fVec1[((IOTA - 79) & 127)]) + ((0.00080254300000000007 * fVec1[((IOTA - 76) & 127)]) + ((0.00065406300000000008 * fVec1[((IOTA - 68) & 127)]) + ((0.00014068899999999999 * fVec1[((IOTA - 67) & 127)]) + ((0.0019257899999999999 * fVec1[((IOTA - 65) & 127)]) + ((0.00162886 * fVec1[((IOTA - 64) & 127)]) + ((0.00050768500000000002 * fVec1[((IOTA - 63) & 127)]) + ((0.0025198 * fVec1[((IOTA - 60) & 127)]) + ((0.00117113 * fVec1[((IOTA - 59) & 127)]) + ((0.00015039900000000001 * fVec1[((IOTA - 58) & 127)]) + ((0.000455995 * fVec1[((IOTA - 56) & 127)]) + ((0.00278239 * fVec1[((IOTA - 55) & 127)]) + ((0.0010278799999999999 * fVec1[((IOTA - 54) & 127)]) + ((0.011156999999999999 * fVec1[((IOTA - 45) & 127)]) + ((0.0043641399999999999 * fVec1[((IOTA - 44) & 127)]) + ((0.0039441099999999998 * fVec1[((IOTA - 42) & 127)]) + ((0.0097659400000000007 * fVec1[((IOTA - 39) & 127)]) + ((0.0076369999999999997 * fVec1[((IOTA - 36) & 127)]) + ((0.015424400000000001 * fVec1[((IOTA - 35) & 127)]) + ((0.0018063700000000001 * fVec1[((IOTA - 32) & 127)]) + ((0.00736515 * fVec1[((IOTA - 31) & 127)]) + ((0.0126468 * fVec1[((IOTA - 30) & 127)]) + ((0.0011752499999999999 * fVec1[((IOTA - 27) & 127)]) + ((0.022162999999999999 * fVec1[((IOTA - 26) & 127)]) + ((0.0071126699999999998 * fVec1[((IOTA - 25) & 127)]) + ((0.018755900000000002 * fVec1[((IOTA - 24) & 127)]) + ((0.0013016799999999999 * fVec1[((IOTA - 23) & 127)]) + ((0.0137094 * fVec1[((IOTA - 21) & 127)]) + ((0.024051599999999999 * fVec1[((IOTA - 18) & 127)]) + ((0.015724599999999998 * fVec1[((IOTA - 15) & 127)]) + ((0.014135400000000001 * fVec1[((IOTA - 12) & 127)]) + ((0.0011849499999999999 * fVec1[((IOTA - 9) & 127)]) + ((0.00070355200000000008 * fVec1[((IOTA - 7) & 127)]) + ((5.9634000000000005e-05 * fVec1[((IOTA - 6) & 127)]) + ((0.000133299 * fVec1[((IOTA - 5) & 127)]) + ((0.00039401200000000001 * fVec1[((IOTA - 3) & 127)]) + ((3.0720999999999999e-05 * fVec1[((IOTA - 1) & 127)]) + ((8.3709499999999999e-06 * fTemp1) + ((0.0011730600000000001 * fVec1[((IOTA - 95) & 127)]) + ((0.00057686100000000002 * fVec1[((IOTA - 106) & 127)]) + ((0.00059161900000000004 * fVec1[((IOTA - 105) & 127)]) + ((0.00049454399999999997 * fVec1[((IOTA - 104) & 127)]) + ((0.00061440799999999999 * fVec1[((IOTA - 103) & 127)]) + ((0.00061735800000000003 * fVec1[((IOTA - 102) & 127)]) + ((0.000728026 * fVec1[((IOTA - 101) & 127)]) + ((0.0016346700000000002 * fVec1[((IOTA - 94) & 127)]) + ((0.00038062800000000004 * fVec1[((IOTA - 100) & 127)]) + ((0.00062138299999999998 * fVec1[((IOTA - 99) & 127)]) + ((0.00075514600000000005 * fVec1[((IOTA - 98) & 127)]) + ((0.0021457000000000004 * fVec1[((IOTA - 93) & 127)]) + ((3.0765800000000003e-05 * fVec3[((IOTA - 119) & 127)]) + ((7.0563399999999996e-05 * fVec3[((IOTA - 118) & 127)]) + ((1.38519e-05 * fVec3[((IOTA - 115) & 127)]) + ((0.00014198999999999999 * fVec3[((IOTA - 114) & 127)]) + ((9.8497200000000003e-05 * fVec3[((IOTA - 113) & 127)]) + ((0.000185267 * fVec3[((IOTA - 112) & 127)]) + ((9.1395000000000002e-05 * fVec3[((IOTA - 111) & 127)]) + ((8.0491800000000001e-05 * fVec3[((IOTA - 110) & 127)]) + ((0.00064522800000000008 * fVec3[((IOTA - 109) & 127)]) + ((0.00058569100000000005 * fVec3[((IOTA - 108) & 127)]) + ((0.00033563700000000001 * fVec3[((IOTA - 107) & 127)]) + ((0.00035424600000000001 * fVec3[((IOTA - 106) & 127)]) + ((0.00062789300000000003 * fVec3[((IOTA - 105) & 127)]) + ((0.00034429799999999999 * fVec3[((IOTA - 104) & 127)]) + ((2.9823200000000003e-05 * fVec3[((IOTA - 103) & 127)]) + ((0.00061507300000000005 * fVec3[((IOTA - 102) & 127)]) + ((0.00037771400000000002 * fVec3[((IOTA - 101) & 127)]) + ((0.00015985700000000001 * fVec3[((IOTA - 100) & 127)]) + ((0.000190946 * fVec3[((IOTA - 99) & 127)]) + ((0.00050743800000000003 * fVec3[((IOTA - 98) & 127)]) + ((0.00056488300000000007 * fVec3[((IOTA - 97) & 127)]) + ((8.1509000000000009e-05 * fVec3[((IOTA - 96) & 127)]) + ((0.00037732899999999998 * fVec3[((IOTA - 95) & 127)]) + ((0.00041022299999999998 * fVec3[((IOTA - 93) & 127)]) + ((0.00085697799999999999 * fVec3[((IOTA - 92) & 127)]) + ((0.000105676 * fVec3[((IOTA - 83) & 127)]) + ((0.00056615599999999995 * fVec3[((IOTA - 7) & 127)]) + ((0.00058234700000000009 * fVec3[((IOTA - 5) & 127)]) + ((0.00027969900000000001 * fVec3[((IOTA - 3) & 127)]) + ((4.34537e-05 * fVec3[((IOTA - 1) & 127)]) + ((0.011357900000000001 * fVec3[((IOTA - 14) & 127)]) + ((0.0037318799999999999 * fVec3[((IOTA - 12) & 127)]) + ((0.00052492499999999998 * fVec3[((IOTA - 75) & 127)]) + ((0.0022562899999999998 * fVec3[((IOTA - 74) & 127)]) + ((0.000321723 * fVec3[((IOTA - 73) & 127)]) + ((0.00080229500000000007 * fVec3[((IOTA - 72) & 127)]) + ((0.0019222600000000001 * fVec3[((IOTA - 71) & 127)]) + ((0.0028435500000000002 * fVec3[((IOTA - 70) & 127)]) + ((4.2787900000000004e-05 * fVec3[((IOTA - 69) & 127)]) + ((0.0024381200000000002 * fVec3[((IOTA - 67) & 127)]) + ((0.0023961899999999999 * fVec3[((IOTA - 66) & 127)]) + ((0.0021430799999999999 * fVec3[((IOTA - 65) & 127)]) + ((0.0030028400000000001 * fVec3[((IOTA - 64) & 127)]) + ((0.00058073100000000002 * fVec3[((IOTA - 63) & 127)]) + ((0.0017794099999999999 * fVec3[((IOTA - 51) & 127)]) + ((0.00225201 * fVec3[((IOTA - 48) & 127)]) + ((0.0035764999999999998 * fVec3[((IOTA - 47) & 127)]) + ((0.0050062100000000005 * fVec3[((IOTA - 46) & 127)]) + ((0.00090678400000000011 * fVec3[((IOTA - 45) & 127)]) + ((0.0060474300000000003 * fVec3[((IOTA - 42) & 127)]) + ((0.0049879100000000008 * fVec3[((IOTA - 41) & 127)]) + ((0.0040529299999999997 * fVec3[((IOTA - 40) & 127)]) + ((0.0083491300000000015 * fVec3[((IOTA - 39) & 127)]) + ((0.011238600000000001 * fVec3[((IOTA - 36) & 127)]) + ((7.6854400000000007e-05 * fVec3[((IOTA - 34) & 127)]) + ((0.0100301 * fVec3[((IOTA - 33) & 127)]) + ((0.0053935399999999996 * fVec3[((IOTA - 32) & 127)]) + ((0.0073630199999999996 * fVec3[((IOTA - 28) & 127)]) + ((0.016863099999999999 * fVec3[((IOTA - 27) & 127)]) + ((0.0087242399999999994 * fVec3[((IOTA - 24) & 127)]) + ((0.027741399999999999 * fVec3[((IOTA - 22) & 127)]) + ((0.0366919 * fVec3[((IOTA - 19) & 127)]) + ((0.0010972500000000001 * fVec3[((IOTA - 11) & 127)]) + ((0.00061125900000000006 * fVec3[((IOTA - 9) & 127)]) + ((7.1292100000000001e-05 * fVec4[((IOTA - 115) & 127)]) + ((6.8217500000000009e-05 * fVec3[((IOTA - 10) & 127)]) + ((0.00018952700000000002 * fVec4[((IOTA - 111) & 127)]) + ((0.00035912999999999999 * fVec4[((IOTA - 110) & 127)]) + ((0.00051251200000000002 * fVec4[((IOTA - 109) & 127)]) + ((0.00046264500000000003 * fVec4[((IOTA - 108) & 127)]) + ((0.00014609200000000001 * fVec4[((IOTA - 107) & 127)]) + ((0.000122237 * fVec4[((IOTA - 106) & 127)]) + ((2.0314199999999999e-05 * fVec4[((IOTA - 105) & 127)]) + ((3.2517300000000001e-05 * fVec4[((IOTA - 104) & 127)]) + ((7.679690000000001e-05 * fVec4[((IOTA - 103) & 127)]) + ((0.00081181999999999999 * fVec4[((IOTA - 102) & 127)]) + ((0.0010270300000000002 * fVec4[((IOTA - 101) & 127)]) + ((0.00033934900000000002 * fVec4[((IOTA - 100) & 127)]) + ((0.00067179499999999999 * fVec4[((IOTA - 99) & 127)]) + ((0.0015498300000000001 * fVec4[((IOTA - 98) & 127)]) + ((0.00193557 * fVec4[((IOTA - 97) & 127)]) + ((0.0012977900000000001 * fVec4[((IOTA - 96) & 127)]) + ((0.00173953 * fVec4[((IOTA - 92) & 127)]) + ((0.0020737100000000003 * fVec4[((IOTA - 91) & 127)]) + ((0.00132314 * fVec4[((IOTA - 90) & 127)]) + ((0.00050203100000000001 * fVec4[((IOTA - 89) & 127)]) + ((2.5817700000000002e-05 * fVec4[((IOTA - 81) & 127)]) + ((0.0008419320000000001 * fVec4[((IOTA - 80) & 127)]) + ((0.00087629800000000007 * fVec4[((IOTA - 79) & 127)]) + ((0.000245352 * fVec4[((IOTA - 70) & 127)]) + ((0.0012620800000000001 * fVec4[((IOTA - 69) & 127)]) + ((0.0023085100000000002 * fVec4[((IOTA - 68) & 127)]) + ((0.002405 * fVec4[((IOTA - 67) & 127)]) + ((0.014772 * fVec4[((IOTA - 34) & 127)]) + ((0.0058612900000000008 * fVec4[((IOTA - 31) & 127)]) + ((0.0057233800000000001 * fVec4[((IOTA - 28) & 127)]) + ((0.0152522 * fVec4[((IOTA - 27) & 127)]) + ((0.0102524 * fVec4[((IOTA - 26) & 127)]) + ((0.018531200000000001 * fVec4[((IOTA - 25) & 127)]) + ((0.044155600000000003 * fVec4[((IOTA - 22) & 127)]) + ((0.0081954799999999998 * fVec4[((IOTA - 21) & 127)]) + ((0.022452899999999998 * fVec4[((IOTA - 16) & 127)]) + ((0.0121954 * fVec4[((IOTA - 15) & 127)]) + ((0.0109563 * fVec4[((IOTA - 12) & 127)]) + ((0.00016957000000000002 * fVec4[((IOTA - 11) & 127)]) + ((0.00010783300000000001 * fVec4[((IOTA - 10) & 127)]) + ((0.00024862500000000004 * fVec4[((IOTA - 9) & 127)]) + ((0.00084071099999999995 * fVec4[((IOTA - 7) & 127)]) + ((9.0162999999999997e-05 * fVec4[((IOTA - 4) & 127)]) + ((0.00036489900000000002 * fVec4[((IOTA - 3) & 127)]) + ((4.0528500000000005e-06 * fTemp4) + ((0.00025800500000000001 * fVec4[((IOTA - 40) & 127)]) + ((0.0064505600000000001 * fVec4[((IOTA - 38) & 127)]) + ((0.0014007000000000002 * fVec4[((IOTA - 66) & 127)]) + ((0.00093808200000000002 * fVec4[((IOTA - 64) & 127)]) + ((0.0028319 * fVec4[((IOTA - 63) & 127)]) + ((0.00071589500000000003 * fVec4[((IOTA - 60) & 127)]) + ((0.0012767 * fVec4[((IOTA - 59) & 127)]) + ((0.0016556800000000001 * fVec4[((IOTA - 58) & 127)]) + ((0.0023854900000000001 * fVec4[((IOTA - 57) & 127)]) + ((0.00071006400000000005 * fVec4[((IOTA - 56) & 127)]) + ((0.0023205399999999998 * fVec4[((IOTA - 54) & 127)]) + ((0.0019624999999999998 * fVec4[((IOTA - 52) & 127)]) + ((0.0022978399999999998 * fVec4[((IOTA - 51) & 127)]) + ((0.00051752200000000008 * fVec4[((IOTA - 48) & 127)]) + ((0.0059472300000000004 * fVec4[((IOTA - 46) & 127)]) + ((0.0056101400000000004 * fVec4[((IOTA - 45) & 127)]) + ((1.16687e-06 * fVec5[((IOTA - 126) & 127)]) + ((8.0474799999999991e-06 * fVec5[((IOTA - 125) & 127)]) + ((1.1716500000000002e-05 * fVec5[((IOTA - 124) & 127)]) + ((1.6095900000000003e-05 * fVec5[((IOTA - 123) & 127)]) + ((1.2353500000000001e-05 * fVec5[((IOTA - 121) & 127)]) + ((5.0538800000000005e-05 * fVec5[((IOTA - 120) & 127)]) + ((6.90663e-05 * fVec5[((IOTA - 119) & 127)]) + ((7.1261200000000008e-05 * fVec5[((IOTA - 110) & 127)]) + ((0.00013404500000000001 * fVec5[((IOTA - 107) & 127)]) + ((0.000144139 * fVec5[((IOTA - 103) & 127)]) + ((0.00074598600000000002 * fVec5[((IOTA - 102) & 127)]) + ((0.00041806100000000001 * fVec5[((IOTA - 101) & 127)]) + ((0.00099729399999999991 * fVec5[((IOTA - 97) & 127)]) + ((0.00072742900000000003 * fVec5[((IOTA - 96) & 127)]) + ((0.00095647000000000006 * fVec5[((IOTA - 94) & 127)]) + ((0.0019078999999999999 * fVec5[((IOTA - 93) & 127)]) + ((0.00037591000000000002 * fVec5[((IOTA - 92) & 127)]) + ((0.00091705400000000015 * fVec5[((IOTA - 91) & 127)]) + ((0.0019979199999999998 * fVec5[((IOTA - 90) & 127)]) + ((0.00084190100000000004 * fVec5[((IOTA - 89) & 127)]) + ((0.00062404300000000002 * fVec5[((IOTA - 85) & 127)]) + ((0.0010226500000000002 * fVec5[((IOTA - 84) & 127)]) + ((0.00016474200000000003 * fVec5[((IOTA - 82) & 127)]) + ((0.00111031 * fVec5[((IOTA - 81) & 127)]) + ((0.0010574600000000001 * fVec5[((IOTA - 80) & 127)]) + ((0.00226954 * fVec5[((IOTA - 62) & 127)]) + ((0.0017599800000000002 * fVec5[((IOTA - 61) & 127)]) + ((0.00071835900000000001 * fVec5[((IOTA - 58) & 127)]) + ((0.0012819700000000001 * fVec5[((IOTA - 57) & 127)]) + ((0.000143156 * fVec5[((IOTA - 55) & 127)]) + ((0.00051545500000000006 * fVec5[((IOTA - 54) & 127)]) + ((0.00011347700000000001 * fVec5[((IOTA - 53) & 127)]) + ((0.00065408100000000004 * fVec5[((IOTA - 52) & 127)]) + ((0.0046855900000000008 * fVec5[((IOTA - 51) & 127)]) + ((0.0086597399999999991 * fVec5[((IOTA - 50) & 127)]) + ((0.0090186299999999997 * fVec5[((IOTA - 49) & 127)]) + ((0.00194699 * fVec5[((IOTA - 48) & 127)]) + ((0.00031685400000000004 * fVec5[((IOTA - 46) & 127)]) + ((0.0069549800000000004 * fVec5[((IOTA - 45) & 127)]) + ((0.011236100000000001 * fVec5[((IOTA - 44) & 127)]) + ((0.0059433200000000002 * fVec5[((IOTA - 43) & 127)]) + ((0.00041302300000000005 * fVec5[((IOTA - 40) & 127)]) + ((0.00281839 * fVec5[((IOTA - 39) & 127)]) + ((0.019649799999999999 * fVec5[((IOTA - 30) & 127)]) + ((0.0053291099999999997 * fVec5[((IOTA - 26) & 127)]) + ((0.0035138299999999999 * fVec5[((IOTA - 25) & 127)]) + ((0.024438399999999999 * fVec5[((IOTA - 22) & 127)]) + ((0.0101949 * fVec5[((IOTA - 21) & 127)]) + ((0.026200899999999999 * fVec5[((IOTA - 20) & 127)]) + ((0.0667629 * fVec5[((IOTA - 19) & 127)]) + ((0.0107382 * fVec5[((IOTA - 18) & 127)]) + ((0.0291912 * fVec5[((IOTA - 17) & 127)]) + ((0.029706100000000003 * fVec5[((IOTA - 16) & 127)]) + ((0.011985200000000001 * fVec5[((IOTA - 15) & 127)]) + ((0.00355895 * fVec5[((IOTA - 10) & 127)]) + ((0.0019467900000000001 * fVec5[((IOTA - 8) & 127)]) + ((0.00139101 * fVec5[((IOTA - 6) & 127)]) + ((0.00016791500000000001 * fVec5[((IOTA - 4) & 127)]) + ((0.00029155500000000004 * fVec5[((IOTA - 2) & 127)]) + ((0.000305641 * fVec5[((IOTA - 68) & 127)]) + ((0.000773294 * fVec5[((IOTA - 67) & 127)]) + ((0.0012639700000000001 * fVec5[((IOTA - 77) & 127)]) + ((0.00089859899999999993 * fVec5[((IOTA - 76) & 127)]) + ((0.00095380500000000006 * fVec5[((IOTA - 72) & 127)]) + ((1.3822700000000001e-06 * fVec6[((IOTA - 126) & 127)]) + ((4.3207799999999997e-06 * fVec6[((IOTA - 125) & 127)]) + ((7.0508400000000002e-07 * fVec6[((IOTA - 123) & 127)]) + ((8.1874899999999997e-07 * fVec6[((IOTA - 122) & 127)]) + ((4.9958900000000007e-05 * fVec6[((IOTA - 121) & 127)]) + ((4.8874300000000007e-05 * fVec6[((IOTA - 120) & 127)]) + ((3.0540600000000004e-05 * fVec6[((IOTA - 119) & 127)]) + ((8.9455700000000012e-05 * fVec6[((IOTA - 116) & 127)]) + ((7.5434700000000005e-05 * fVec6[((IOTA - 115) & 127)]) + ((0.00014591800000000001 * fVec6[((IOTA - 114) & 127)]) + ((0.00032979500000000002 * fVec6[((IOTA - 109) & 127)]) + ((0.00027301799999999998 * fVec6[((IOTA - 108) & 127)]) + ((2.9295999999999999e-06 * fVec6[((IOTA - 106) & 127)]) + ((0.00041821300000000002 * fVec6[((IOTA - 105) & 127)]) + ((0.00016436200000000001 * fVec6[((IOTA - 104) & 127)]) + ((1.0304700000000001e-05 * fVec6[((IOTA - 103) & 127)]) + ((9.9420300000000014e-05 * fVec6[((IOTA - 102) & 127)]) + ((0.000143957 * fVec6[((IOTA - 101) & 127)]) + ((0.00068593000000000007 * fVec6[((IOTA - 97) & 127)]) + ((0.0022403200000000001 * fVec6[((IOTA - 96) & 127)]) + ((0.00054845200000000005 * fVec6[((IOTA - 95) & 127)]) + ((0.0013290999999999999 * fVec6[((IOTA - 90) & 127)]) + ((0.0010531200000000001 * fVec6[((IOTA - 86) & 127)]) + ((0.00043581100000000003 * fVec6[((IOTA - 85) & 127)]) + ((0.00043643700000000002 * fVec6[((IOTA - 84) & 127)]) + ((0.00088380000000000002 * fVec6[((IOTA - 83) & 127)]) + ((0.00047224300000000001 * fVec6[((IOTA - 79) & 127)]) + ((0.00094611000000000001 * fVec6[((IOTA - 78) & 127)]) + ((0.0014756400000000001 * fVec6[((IOTA - 65) & 127)]) + ((0.0014824 * fVec6[((IOTA - 64) & 127)]) + ((0.000313016 * fVec6[((IOTA - 63) & 127)]) + ((0.00046532900000000006 * fVec6[((IOTA - 61) & 127)]) + ((0.0014774499999999999 * fVec6[((IOTA - 57) & 127)]) + ((0.0052267299999999997 * fVec6[((IOTA - 56) & 127)]) + ((0.0049627400000000002 * fVec6[((IOTA - 55) & 127)]) + ((0.00034213000000000001 * fVec6[((IOTA - 54) & 127)]) + ((0.00171871 * fVec6[((IOTA - 52) & 127)]) + ((0.00204601 * fVec6[((IOTA - 51) & 127)]) + ((0.00172897 * fVec6[((IOTA - 44) & 127)]) + ((0.0059491199999999996 * fVec6[((IOTA - 43) & 127)]) + ((0.0069924600000000007 * fVec6[((IOTA - 42) & 127)]) + ((0.0046671199999999994 * fVec6[((IOTA - 38) & 127)]) + ((0.0074278399999999998 * fVec6[((IOTA - 37) & 127)]) + ((0.0038431400000000001 * fVec6[((IOTA - 33) & 127)]) + ((0.001851 * fVec6[((IOTA - 32) & 127)]) + ((0.00023100900000000004 * fVec6[((IOTA - 31) & 127)]) + ((0.0018550299999999999 * fVec6[((IOTA - 30) & 127)]) + ((0.0068345300000000001 * fVec6[((IOTA - 29) & 127)]) + ((0.0065994599999999997 * fVec6[((IOTA - 28) & 127)]) + ((0.0086483799999999989 * fVec6[((IOTA - 26) & 127)]) + ((0.034778900000000001 * fVec6[((IOTA - 25) & 127)]) + ((0.0033660500000000002 * fVec6[((IOTA - 21) & 127)]) + ((0.0166362 * fVec6[((IOTA - 19) & 127)]) + ((0.013249500000000001 * fVec6[((IOTA - 18) & 127)]) + ((0.0149275 * fVec6[((IOTA - 15) & 127)]) + ((0.00153611 * fVec6[((IOTA - 12) & 127)]) + ((0.00022380100000000001 * fVec6[((IOTA - 11) & 127)]) + ((0.0002062 * fVec6[((IOTA - 10) & 127)]) + ((0.00029508100000000004 * fVec6[((IOTA - 9) & 127)]) + ((9.3157600000000004e-05 * fVec6[((IOTA - 7) & 127)]) + ((0.00083150800000000001 * fVec6[((IOTA - 6) & 127)]) + ((0.000139085 * fVec6[((IOTA - 3) & 127)]) + ((0.000100796 * fVec6[((IOTA - 2) & 127)]) + ((5.4869299999999995e-06 * fTemp6) + ((0.00034605900000000002 * fVec6[((IOTA - 68) & 127)]) + ((0.00166238 * fVec6[((IOTA - 67) & 127)]) + ((0.00061167100000000005 * fVec6[((IOTA - 66) & 127)]) + ((0.0029536099999999997 * fVec6[((IOTA - 75) & 127)]) + ((0.0014493400000000001 * fVec6[((IOTA - 74) & 127)]) + ((0.00033464100000000005 * fVec6[((IOTA - 71) & 127)]) + ((0.000284002 * fVec6[((IOTA - 70) & 127)]) + ((1.2067399999999999e-06 * fVec7[((IOTA - 126) & 127)]) + ((1.4484800000000001e-05 * fVec7[((IOTA - 123) & 127)]) + ((2.1146600000000005e-05 * fVec7[((IOTA - 122) & 127)]) + ((3.7560900000000002e-05 * fVec7[((IOTA - 121) & 127)]) + ((5.5921400000000003e-05 * fVec7[((IOTA - 120) & 127)]) + ((2.8070300000000005e-05 * fVec7[((IOTA - 119) & 127)]) + ((3.5723000000000003e-05 * fVec7[((IOTA - 118) & 127)]) + ((6.8163800000000006e-05 * fVec7[((IOTA - 117) & 127)]) + ((2.3925e-06 * fVec7[((IOTA - 116) & 127)]) + ((0.00014829900000000001 * fVec7[((IOTA - 108) & 127)]) + ((0.00037316500000000001 * fVec7[((IOTA - 105) & 127)]) + ((0.00039246300000000001 * fVec7[((IOTA - 104) & 127)]) + ((0.00069076200000000004 * fVec7[((IOTA - 98) & 127)]) + ((0.00059752000000000004 * fVec7[((IOTA - 97) & 127)]) + ((3.8900100000000004e-05 * fVec7[((IOTA - 96) & 127)]) + ((0.00104006 * fVec7[((IOTA - 95) & 127)]) + ((0.0024621700000000001 * fVec7[((IOTA - 94) & 127)]) + ((0.0011040900000000001 * fVec7[((IOTA - 93) & 127)]) + ((0.00025218300000000001 * fVec7[((IOTA - 92) & 127)]) + ((0.000582052 * fVec7[((IOTA - 87) & 127)]) + ((0.00181963 * fVec7[((IOTA - 86) & 127)]) + ((0.00027008400000000003 * fVec7[((IOTA - 83) & 127)]) + ((3.9284900000000001e-05 * fVec7[((IOTA - 82) & 127)]) + ((0.0023190699999999999 * fVec7[((IOTA - 78) & 127)]) + ((0.0031104100000000001 * fVec7[((IOTA - 77) & 127)]) + ((0.0018716399999999999 * fVec7[((IOTA - 76) & 127)]) + ((0.00173413 * fVec7[((IOTA - 75) & 127)]) + ((0.00046513499999999998 * fVec7[((IOTA - 74) & 127)]) + ((0.00070995800000000001 * fVec7[((IOTA - 73) & 127)]) + ((0.00136597 * fVec7[((IOTA - 72) & 127)]) + ((0.0005198920000000001 * fVec7[((IOTA - 66) & 127)]) + ((0.00252236 * fVec7[((IOTA - 65) & 127)]) + ((0.00148653 * fVec7[((IOTA - 64) & 127)]) + ((0.0031196700000000002 * fVec7[((IOTA - 54) & 127)]) + ((0.0014963299999999999 * fVec7[((IOTA - 53) & 127)]) + ((0.0021004299999999999 * fVec7[((IOTA - 52) & 127)]) + ((0.0077831999999999997 * fVec7[((IOTA - 51) & 127)]) + ((0.0044508500000000001 * fVec7[((IOTA - 50) & 127)]) + ((0.0021174599999999998 * fVec7[((IOTA - 49) & 127)]) + ((0.0040522299999999995 * fVec7[((IOTA - 48) & 127)]) + ((0.0064639199999999997 * fVec7[((IOTA - 47) & 127)]) + ((0.0059821600000000003 * fVec7[((IOTA - 46) & 127)]) + ((0.0034409700000000002 * fVec7[((IOTA - 45) & 127)]) + ((0.0067079800000000005 * fVec7[((IOTA - 44) & 127)]) + ((0.0079398100000000003 * fVec7[((IOTA - 43) & 127)]) + ((0.0101911 * fVec7[((IOTA - 42) & 127)]) + ((0.0084519999999999994 * fVec7[((IOTA - 41) & 127)]) + ((0.0027287100000000001 * fVec7[((IOTA - 40) & 127)]) + ((0.0016985100000000001 * fVec7[((IOTA - 35) & 127)]) + ((0.057691300000000001 * fVec7[((IOTA - 23) & 127)]) + ((0.016169199999999998 * fVec7[((IOTA - 22) & 127)]) + ((0.044694299999999999 * fVec7[((IOTA - 20) & 127)]) + ((0.073524500000000007 * fVec7[((IOTA - 19) & 127)]) + ((0.046224100000000004 * fVec7[((IOTA - 18) & 127)]) + ((0.039818899999999997 * fVec7[((IOTA - 16) & 127)]) + ((0.030653800000000002 * fVec7[((IOTA - 15) & 127)]) + ((0.0028312599999999999 * fVec7[((IOTA - 10) & 127)]) + ((0.00023471600000000002 * fVec7[((IOTA - 8) & 127)]) + ((0.0016646499999999999 * fVec7[((IOTA - 6) & 127)]) + ((0.00018944099999999999 * fVec7[((IOTA - 4) & 127)]) + ((0.00041035500000000005 * fVec7[((IOTA - 2) & 127)]) + ((0.00049729599999999994 * fVec7[((IOTA - 61) & 127)]) + ((0.0017160000000000001 * fVec7[((IOTA - 60) & 127)]) + ((0.000571158 * fVec7[((IOTA - 59) & 127)]) + ((1.06705e-06 * fVec8[((IOTA - 126) & 127)]) + ((3.8319600000000005e-05 * fVec8[((IOTA - 122) & 127)]) + ((5.611290000000001e-05 * fVec8[((IOTA - 121) & 127)]) + ((4.3469500000000001e-05 * fVec8[((IOTA - 120) & 127)]) + ((4.8748200000000002e-05 * fVec8[((IOTA - 117) & 127)]) + ((0.000144938 * fVec8[((IOTA - 110) & 127)]) + ((7.2644700000000013e-05 * fVec8[((IOTA - 104) & 127)]) + ((1.0278999999999999e-06 * fVec8[((IOTA - 103) & 127)]) + ((0.0011606799999999999 * fVec8[((IOTA - 96) & 127)]) + ((0.0031629900000000001 * fVec8[((IOTA - 95) & 127)]) + ((0.00078389600000000007 * fVec8[((IOTA - 94) & 127)]) + ((3.61728e-05 * fVec8[((IOTA - 91) & 127)]) + ((0.0022532999999999997 * fVec8[((IOTA - 86) & 127)]) + ((0.00053547700000000002 * fVec8[((IOTA - 77) & 127)]) + ((0.00078204200000000005 * fVec8[((IOTA - 79) & 127)]) + ((0.00089492 * fVec8[((IOTA - 76) & 127)]) + ((0.00098582100000000001 * fVec8[((IOTA - 75) & 127)]) + ((0.0010638000000000002 * fVec8[((IOTA - 74) & 127)]) + ((0.0020713300000000001 * fVec8[((IOTA - 72) & 127)]) + ((0.00287405 * fVec8[((IOTA - 71) & 127)]) + ((0.0018535400000000001 * fVec8[((IOTA - 70) & 127)]) + ((0.0024114399999999999 * fVec8[((IOTA - 69) & 127)]) + ((0.0054627 * fVec8[((IOTA - 68) & 127)]) + ((0.0053478500000000003 * fVec8[((IOTA - 67) & 127)]) + ((0.0036506200000000003 * fVec8[((IOTA - 66) & 127)]) + ((0.0039108500000000004 * fVec8[((IOTA - 65) & 127)]) + ((0.00124406 * fVec8[((IOTA - 64) & 127)]) + ((0.00066612700000000008 * fVec8[((IOTA - 53) & 127)]) + ((0.00021471700000000001 * fVec8[((IOTA - 42) & 127)]) + ((0.0083305600000000007 * fVec8[((IOTA - 38) & 127)]) + ((0.0016041499999999999 * fVec8[((IOTA - 34) & 127)]) + ((0.018119700000000002 * fVec8[((IOTA - 33) & 127)]) + ((0.036438700000000004 * fVec8[((IOTA - 30) & 127)]) + ((0.027385400000000001 * fVec8[((IOTA - 29) & 127)]) + ((0.0048108300000000003 * fVec8[((IOTA - 28) & 127)]) + ((0.040978300000000002 * fVec8[((IOTA - 27) & 127)]) + ((0.049985600000000005 * fVec8[((IOTA - 26) & 127)]) + ((0.038770699999999998 * fVec8[((IOTA - 24) & 127)]) + ((0.10758200000000001 * fVec8[((IOTA - 23) & 127)]) + ((0.021392099999999997 * fVec8[((IOTA - 22) & 127)]) + ((0.041771900000000008 * fVec8[((IOTA - 20) & 127)]) + ((0.055177400000000001 * fVec8[((IOTA - 19) & 127)]) + ((0.0108086 * fVec8[((IOTA - 18) & 127)]) + ((0.0026607500000000004 * fVec8[((IOTA - 10) & 127)]) + ((0.00066903899999999996 * fVec8[((IOTA - 8) & 127)]) + ((0.0013556799999999999 * fVec8[((IOTA - 6) & 127)]) + ((0.00012807300000000001 * fVec8[((IOTA - 4) & 127)]) + ((0.000378556 * fVec8[((IOTA - 2) & 127)]) + ((4.9495899999999995e-07 * fVec9[((IOTA - 126) & 127)]) + ((2.773e-06 * fVec9[((IOTA - 125) & 127)]) + ((5.3135e-06 * fVec9[((IOTA - 121) & 127)]) + ((2.8457499999999998e-06 * fVec9[((IOTA - 117) & 127)]) + ((2.4558300000000005e-05 * fVec9[((IOTA - 116) & 127)]) + ((5.2591200000000006e-05 * fVec9[((IOTA - 112) & 127)]) + ((0.00024796200000000001 * fVec9[((IOTA - 111) & 127)]) + ((0.00014221200000000001 * fVec9[((IOTA - 110) & 127)]) + ((0.00010309 * fVec9[((IOTA - 108) & 127)]) + ((0.00018337600000000001 * fVec9[((IOTA - 107) & 127)]) + ((0.00035608600000000003 * fVec9[((IOTA - 106) & 127)]) + ((0.00034403800000000002 * fVec9[((IOTA - 105) & 127)]) + ((0.00025383300000000002 * fVec9[((IOTA - 104) & 127)]) + ((0.00028535700000000003 * fVec9[((IOTA - 103) & 127)]) + ((0.00070562000000000001 * fVec9[((IOTA - 102) & 127)]) + ((0.0005151 * fVec9[((IOTA - 101) & 127)]) + ((0.00032077100000000002 * fVec9[((IOTA - 100) & 127)]) + ((9.0654800000000018e-05 * fVec9[((IOTA - 99) & 127)]) + ((0.00071143700000000009 * fVec9[((IOTA - 98) & 127)]) + ((0.0014366699999999999 * fVec9[((IOTA - 97) & 127)]) + ((0.00124614 * fVec9[((IOTA - 94) & 127)]) + ((0.0022713 * fVec9[((IOTA - 93) & 127)]) + ((0.00127749 * fVec9[((IOTA - 86) & 127)]) + ((6.2936500000000003e-05 * fVec9[((IOTA - 77) & 127)]) + ((2.0687900000000002e-05 * fVec9[((IOTA - 76) & 127)]) + ((0.00074920100000000001 * fVec9[((IOTA - 72) & 127)]) + ((0.00108661 * fVec9[((IOTA - 71) & 127)]) + ((0.00066226100000000003 * fVec9[((IOTA - 70) & 127)]) + ((0.00221192 * fVec9[((IOTA - 68) & 127)]) + ((0.0036181899999999999 * fVec9[((IOTA - 67) & 127)]) + ((3.8066800000000006e-05 * fVec9[((IOTA - 66) & 127)]) + ((2.7286800000000004e-05 * fVec9[((IOTA - 64) & 127)]) + ((0.0014662800000000001 * fVec9[((IOTA - 63) & 127)]) + ((0.0025716599999999999 * fVec9[((IOTA - 62) & 127)]) + ((0.00040080500000000001 * fVec9[((IOTA - 61) & 127)]) + ((0.0022166500000000001 * fVec9[((IOTA - 59) & 127)]) + ((0.0027539400000000003 * fVec9[((IOTA - 58) & 127)]) + ((0.000547636 * fVec9[((IOTA - 57) & 127)]) + ((0.0034324300000000002 * fVec9[((IOTA - 56) & 127)]) + ((0.0028814800000000001 * fVec9[((IOTA - 55) & 127)]) + ((0.00020467599999999999 * fVec9[((IOTA - 54) & 127)]) + ((0.0025276200000000004 * fVec9[((IOTA - 51) & 127)]) + ((0.0045955500000000003 * fVec9[((IOTA - 50) & 127)]) + ((0.0021549399999999997 * fVec9[((IOTA - 44) & 127)]) + ((0.0013761300000000001 * fVec9[((IOTA - 41) & 127)]) + ((0.002183 * fVec9[((IOTA - 38) & 127)]) + ((0.0030387000000000001 * fVec9[((IOTA - 36) & 127)]) + ((0.021556799999999997 * fVec9[((IOTA - 35) & 127)]) + ((0.023527200000000002 * fVec9[((IOTA - 32) & 127)]) + ((0.0021948500000000004 * fVec9[((IOTA - 31) & 127)]) + ((0.00051807200000000006 * fVec9[((IOTA - 29) & 127)]) + ((0.024944000000000001 * fVec9[((IOTA - 26) & 127)]) + ((0.0035849099999999997 * fVec9[((IOTA - 25) & 127)]) + ((0.011300600000000001 * fVec9[((IOTA - 19) & 127)]) + ((0.0094331200000000014 * fVec9[((IOTA - 18) & 127)]) + ((0.0071840999999999997 * fVec9[((IOTA - 15) & 127)]) + ((0.00027813700000000002 * fVec9[((IOTA - 12) & 127)]) + ((0.00027818999999999999 * fVec9[((IOTA - 11) & 127)]) + ((0.00033955200000000002 * fVec9[((IOTA - 10) & 127)]) + ((0.00022609 * fVec9[((IOTA - 9) & 127)]) + ((0.00016021500000000001 * fVec9[((IOTA - 7) & 127)]) + ((0.000240886 * fVec9[((IOTA - 6) & 127)]) + ((2.4972599999999999e-05 * fVec9[((IOTA - 3) & 127)]) + ((5.2352700000000004e-05 * fVec9[((IOTA - 2) & 127)]) + ((4.2462999999999995e-06 * fTemp9) + ((0.00070097300000000003 * fVec9[((IOTA - 85) & 127)]) + ((8.9630500000000016e-08 * fVec10[((IOTA - 126) & 127)]) + ((4.86084e-05 * fVec10[((IOTA - 119) & 127)]) + ((3.0215200000000005e-05 * fVec10[((IOTA - 118) & 127)]) + ((0.00010084700000000001 * fVec10[((IOTA - 110) & 127)]) + ((0.00034166200000000001 * fVec10[((IOTA - 109) & 127)]) + ((0.000204718 * fVec10[((IOTA - 108) & 127)]) + ((0.00033797299999999998 * fVec10[((IOTA - 107) & 127)]) + ((0.00045785700000000004 * fVec10[((IOTA - 106) & 127)]) + ((0.00013590400000000001 * fVec10[((IOTA - 105) & 127)]) + ((6.0929900000000008e-05 * fVec10[((IOTA - 104) & 127)]) + ((0.000208667 * fVec10[((IOTA - 102) & 127)]) + ((0.00030785900000000001 * fVec10[((IOTA - 101) & 127)]) + ((0.00026147399999999998 * fVec10[((IOTA - 100) & 127)]) + ((0.00048677700000000003 * fVec10[((IOTA - 99) & 127)]) + ((0.0012107800000000001 * fVec10[((IOTA - 98) & 127)]) + ((0.00076490500000000008 * fVec10[((IOTA - 97) & 127)]) + ((0.00058266999999999998 * fVec10[((IOTA - 94) & 127)]) + ((0.00082486899999999999 * fVec10[((IOTA - 93) & 127)]) + ((0.0011506799999999998 * fVec10[((IOTA - 92) & 127)]) + ((3.5632800000000002e-05 * fVec10[((IOTA - 91) & 127)]) + ((3.64663e-05 * fVec10[((IOTA - 90) & 127)]) + ((0.00056137899999999998 * fVec10[((IOTA - 88) & 127)]) + ((0.00015598000000000001 * fVec10[((IOTA - 87) & 127)]) + ((0.00076870700000000009 * fVec10[((IOTA - 84) & 127)]) + ((0.0014322200000000001 * fVec10[((IOTA - 83) & 127)]) + ((0.00046067700000000005 * fVec10[((IOTA - 82) & 127)]) + ((0.00064690400000000001 * fVec10[((IOTA - 79) & 127)]) + ((0.00271718 * fVec10[((IOTA - 75) & 127)]) + ((6.2586200000000001e-05 * fVec10[((IOTA - 72) & 127)]) + ((0.00117567 * fVec10[((IOTA - 71) & 127)]) + ((0.0012521400000000001 * fVec10[((IOTA - 70) & 127)]) + ((0.00055114699999999999 * fVec10[((IOTA - 66) & 127)]) + ((0.00110439 * fVec10[((IOTA - 65) & 127)]) + ((0.0016155 * fVec10[((IOTA - 64) & 127)]) + ((0.0021511500000000001 * fVec10[((IOTA - 63) & 127)]) + ((0.0019438999999999999 * fVec10[((IOTA - 60) & 127)]) + ((0.00122438 * fVec10[((IOTA - 52) & 127)]) + ((0.0070393900000000004 * fVec10[((IOTA - 39) & 127)]) + ((0.0048936500000000003 * fVec10[((IOTA - 37) & 127)]) + ((0.0095701499999999995 * fVec10[((IOTA - 36) & 127)]) + ((0.015800600000000001 * fVec10[((IOTA - 33) & 127)]) + ((0.011886499999999999 * fVec10[((IOTA - 32) & 127)]) + ((0.0058739299999999994 * fVec10[((IOTA - 30) & 127)]) + ((0.024750999999999999 * fVec10[((IOTA - 29) & 127)]) + ((0.0086730599999999998 * fVec10[((IOTA - 28) & 127)]) + ((0.023463500000000002 * fVec10[((IOTA - 26) & 127)]) + ((0.052758300000000001 * fVec10[((IOTA - 25) & 127)]) + ((0.0124978 * fVec10[((IOTA - 24) & 127)]) + ((0.035996800000000002 * fVec10[((IOTA - 22) & 127)]) + ((0.013752100000000001 * fVec10[((IOTA - 21) & 127)]) + ((0.0018844000000000001 * fVec10[((IOTA - 20) & 127)]) + ((0.0448946 * fVec10[((IOTA - 19) & 127)]) + ((0.00033159299999999998 * fVec10[((IOTA - 18) & 127)]) + ((0.00255288 * fVec10[((IOTA - 10) & 127)]) + ((0.00094661300000000001 * fVec10[((IOTA - 8) & 127)]) + ((0.0010133200000000001 * fVec10[((IOTA - 6) & 127)]) + ((0.00025746299999999998 * fVec10[((IOTA - 2) & 127)]) + ((3.43292e-08 * fVec11[((IOTA - 126) & 127)]) + ((3.3588899999999999e-06 * fVec11[((IOTA - 125) & 127)]) + ((3.9804799999999997e-06 * fVec11[((IOTA - 124) & 127)]) + ((2.0105900000000003e-05 * fVec11[((IOTA - 123) & 127)]) + ((7.7608600000000008e-05 * fVec11[((IOTA - 119) & 127)]) + ((5.0789799999999999e-05 * fVec11[((IOTA - 116) & 127)]) + ((0.00020361699999999999 * fVec11[((IOTA - 115) & 127)]) + ((0.00030080800000000001 * fVec11[((IOTA - 114) & 127)]) + ((3.2623300000000003e-05 * fVec11[((IOTA - 113) & 127)]) + ((0.00026027500000000002 * fVec11[((IOTA - 110) & 127)]) + ((0.00020187200000000002 * fVec11[((IOTA - 109) & 127)]) + ((0.00040571999999999998 * fVec11[((IOTA - 108) & 127)]) + ((0.00053009599999999997 * fVec11[((IOTA - 107) & 127)]) + ((0.00050674400000000005 * fVec11[((IOTA - 106) & 127)]) + ((0.00049718700000000006 * fVec11[((IOTA - 105) & 127)]) + ((0.0011330000000000001 * fVec11[((IOTA - 104) & 127)]) + ((0.0016271899999999999 * fVec11[((IOTA - 103) & 127)]) + ((0.00069229100000000004 * fVec11[((IOTA - 102) & 127)]) + ((0.00046662400000000006 * fVec11[((IOTA - 101) & 127)]) + ((0.00069381500000000008 * fVec11[((IOTA - 100) & 127)]) + ((0.00038071900000000001 * fVec11[((IOTA - 99) & 127)]) + ((0.00084254300000000007 * fVec11[((IOTA - 98) & 127)]) + ((0.00156339 * fVec11[((IOTA - 97) & 127)]) + ((0.0034697200000000004 * fVec11[((IOTA - 96) & 127)]) + ((0.0030116200000000004 * fVec11[((IOTA - 95) & 127)]) + ((0.00075465500000000002 * fVec11[((IOTA - 94) & 127)]) + ((0.00068181600000000002 * fVec11[((IOTA - 93) & 127)]) + ((0.00071934100000000008 * fVec11[((IOTA - 92) & 127)]) + ((0.00040184499999999996 * fVec11[((IOTA - 91) & 127)]) + ((0.00055143000000000011 * fVec11[((IOTA - 90) & 127)]) + ((0.0016723300000000001 * fVec11[((IOTA - 89) & 127)]) + ((0.0017063 * fVec11[((IOTA - 88) & 127)]) + ((0.0013395900000000001 * fVec11[((IOTA - 87) & 127)]) + ((0.0034494400000000002 * fVec11[((IOTA - 85) & 127)]) + ((0.0025590399999999998 * fVec11[((IOTA - 84) & 127)]) + ((0.0014532900000000001 * fVec11[((IOTA - 83) & 127)]) + ((0.00127111 * fVec11[((IOTA - 78) & 127)]) + ((0.0011545000000000001 * fVec11[((IOTA - 77) & 127)]) + ((0.00061171300000000006 * fVec11[((IOTA - 76) & 127)]) + ((0.00020694100000000001 * fVec11[((IOTA - 75) & 127)]) + ((0.00064708400000000003 * fVec11[((IOTA - 74) & 127)]) + ((0.0010632999999999999 * fVec11[((IOTA - 73) & 127)]) + ((0.000609699 * fVec11[((IOTA - 72) & 127)]) + ((0.00058977500000000006 * fVec11[((IOTA - 71) & 127)]) + ((0.0005591420000000001 * fVec11[((IOTA - 67) & 127)]) + ((8.8419900000000004e-05 * fVec11[((IOTA - 65) & 127)]) + ((0.00150508 * fVec11[((IOTA - 64) & 127)]) + ((0.0014054500000000001 * fVec11[((IOTA - 60) & 127)]) + ((0.00215198 * fVec11[((IOTA - 54) & 127)]) + ((0.000219724 * fVec11[((IOTA - 52) & 127)]) + ((0.0017431900000000001 * fVec11[((IOTA - 51) & 127)]) + ((0.000292148 * fVec11[((IOTA - 49) & 127)]) + ((0.00356967 * fVec11[((IOTA - 48) & 127)]) + ((0.00055458199999999997 * fVec11[((IOTA - 47) & 127)]) + ((0.0018462400000000001 * fVec11[((IOTA - 45) & 127)]) + ((0.0038023500000000003 * fVec11[((IOTA - 42) & 127)]) + ((0.0029068700000000002 * fVec11[((IOTA - 40) & 127)]) + ((0.0047945900000000005 * fVec11[((IOTA - 39) & 127)]) + ((0.0038672100000000003 * fVec11[((IOTA - 38) & 127)]) + ((4.1971900000000004e-05 * fVec11[((IOTA - 36) & 127)]) + ((0.0060181399999999999 * fVec11[((IOTA - 35) & 127)]) + ((0.0049045500000000006 * fVec11[((IOTA - 33) & 127)]) + ((0.0091428700000000009 * fVec11[((IOTA - 32) & 127)]) + ((0.0099856200000000006 * fVec11[((IOTA - 29) & 127)]) + ((0.010922899999999999 * fVec11[((IOTA - 28) & 127)]) + ((0.0013328999999999999 * fVec11[((IOTA - 24) & 127)]) + ((0.0016006100000000001 * fVec11[((IOTA - 23) & 127)]) + ((0.0099153599999999998 * fVec11[((IOTA - 22) & 127)]) + ((0.013603000000000001 * fVec11[((IOTA - 21) & 127)]) + ((0.016723499999999999 * fVec11[((IOTA - 20) & 127)]) + ((0.045127499999999994 * fVec11[((IOTA - 17) & 127)]) + ((0.0059659600000000002 * fVec11[((IOTA - 14) & 127)]) + ((0.0027371000000000001 * fVec11[((IOTA - 13) & 127)]) + ((4.6184399999999998e-05 * fVec11[((IOTA - 10) & 127)]) + ((0.0025209700000000004 * fVec11[((IOTA - 8) & 127)]) + ((0.00036855699999999999 * fVec11[((IOTA - 6) & 127)]) + ((0.00070276500000000005 * fVec11[((IOTA - 5) & 127)]) + ((0.00090358699999999997 * fVec11[((IOTA - 4) & 127)]) + ((0.000231406 * fVec11[((IOTA - 2) & 127)]) + ((4.4178200000000003e-05 * fVec11[((IOTA - 1) & 127)]) + ((1.57811e-05 * fTemp11) + ((2.5137800000000002e-07 * fVec12[((IOTA - 126) & 127)]) + ((6.1820100000000001e-06 * fVec12[((IOTA - 125) & 127)]) + ((3.82797e-06 * fVec12[((IOTA - 124) & 127)]) + ((2.1185900000000001e-06 * fVec12[((IOTA - 123) & 127)]) + ((2.7248300000000001e-05 * fVec12[((IOTA - 121) & 127)]) + ((3.8722400000000007e-05 * fVec12[((IOTA - 120) & 127)]) + ((8.2140799999999984e-06 * fVec12[((IOTA - 119) & 127)]) + ((3.8470500000000004e-05 * fVec12[((IOTA - 117) & 127)]) + ((9.096330000000001e-05 * fVec12[((IOTA - 116) & 127)]) + ((8.7317000000000004e-05 * fVec12[((IOTA - 114) & 127)]) + ((0.000227166 * fVec12[((IOTA - 113) & 127)]) + ((8.3273200000000014e-05 * fVec12[((IOTA - 112) & 127)]) + ((8.2866000000000002e-05 * fVec12[((IOTA - 111) & 127)]) + ((0.00014514899999999999 * fVec12[((IOTA - 110) & 127)]) + ((0.00010655300000000001 * fVec12[((IOTA - 108) & 127)]) + ((0.00020428199999999999 * fVec12[((IOTA - 107) & 127)]) + ((5.2586900000000003e-05 * fVec12[((IOTA - 106) & 127)]) + ((0.00036701000000000003 * fVec12[((IOTA - 105) & 127)]) + ((0.00050770799999999994 * fVec12[((IOTA - 104) & 127)]) + ((0.00025426099999999998 * fVec12[((IOTA - 101) & 127)]) + ((0.00018379600000000001 * fVec12[((IOTA - 98) & 127)]) + ((0.00035138200000000002 * fVec12[((IOTA - 97) & 127)]) + ((7.6149300000000013e-05 * fVec12[((IOTA - 95) & 127)]) + ((0.0015575400000000001 * fVec12[((IOTA - 94) & 127)]) + ((0.00063264199999999999 * fVec12[((IOTA - 93) & 127)]) + ((0.00014839199999999998 * fVec12[((IOTA - 92) & 127)]) + ((0.0026214800000000003 * fVec11[((IOTA - 86) & 127)]) + ((0.00086251900000000007 * fVec12[((IOTA - 87) & 127)]) + ((0.0019024199999999999 * fVec12[((IOTA - 86) & 127)]) + ((0.00129144 * fVec12[((IOTA - 83) & 127)]) + ((0.00047882000000000003 * fVec12[((IOTA - 78) & 127)]) + ((0.0013118799999999999 * fVec12[((IOTA - 75) & 127)]) + ((0.0011614100000000001 * fVec12[((IOTA - 74) & 127)]) + ((0.0014712700000000002 * fVec12[((IOTA - 68) & 127)]) + ((0.0026238199999999998 * fVec12[((IOTA - 67) & 127)]) + ((8.7116700000000006e-05 * fVec12[((IOTA - 66) & 127)]) + ((0.0020513699999999998 * fVec12[((IOTA - 62) & 127)]) + ((0.00160801 * fVec12[((IOTA - 61) & 127)]) + ((0.00147938 * fVec12[((IOTA - 59) & 127)]) + ((0.00013115699999999999 * fVec12[((IOTA - 58) & 127)]) + ((0.00050519600000000007 * fVec12[((IOTA - 57) & 127)]) + ((0.0016339300000000002 * fVec12[((IOTA - 56) & 127)]) + ((0.0011073599999999999 * fVec12[((IOTA - 52) & 127)]) + ((0.0016051100000000001 * fVec12[((IOTA - 50) & 127)]) + ((0.0079808199999999996 * fVec12[((IOTA - 49) & 127)]) + ((0.00196889 * fVec12[((IOTA - 48) & 127)]) + ((9.226730000000001e-05 * fVec12[((IOTA - 40) & 127)]) + ((0.0053560100000000005 * fVec12[((IOTA - 39) & 127)]) + ((0.0053043999999999999 * fVec12[((IOTA - 38) & 127)]) + ((0.00291413 * fVec12[((IOTA - 37) & 127)]) + ((0.010230900000000001 * fVec12[((IOTA - 36) & 127)]) + ((0.0073240200000000005 * fVec12[((IOTA - 35) & 127)]) + ((0.0073238899999999996 * fVec12[((IOTA - 31) & 127)]) + ((0.0105234 * fVec12[((IOTA - 30) & 127)]) + ((0.0034125200000000005 * fVec12[((IOTA - 26) & 127)]) + ((0.0069197900000000003 * fVec12[((IOTA - 25) & 127)]) + ((0.031673199999999999 * fVec12[((IOTA - 18) & 127)]) + ((0.0274497 * fVec12[((IOTA - 17) & 127)]) + ((0.0030908899999999998 * fVec12[((IOTA - 16) & 127)]) + ((0.0103427 * fVec12[((IOTA - 15) & 127)]) + ((0.0120915 * fVec12[((IOTA - 13) & 127)]) + ((0.0038904299999999998 * fVec12[((IOTA - 10) & 127)]) + ((0.0024724199999999999 * fVec12[((IOTA - 8) & 127)]) + ((0.0012886100000000001 * fVec12[((IOTA - 6) & 127)]) + ((0.00084856599999999993 * fVec12[((IOTA - 4) & 127)]) + ((0.00020222799999999999 * fVec12[((IOTA - 2) & 127)]) + ((2.0016700000000001e-05 * fVec12[((IOTA - 1) & 127)]) + ((1.4622100000000002e-05 * fTemp12) + ((0.0044432300000000003 * fVec12[((IOTA - 45) & 127)]) + ((0.000208879 * fVec12[((IOTA - 44) & 127)]) + ((2.4533100000000001e-06 * fVec13[((IOTA - 126) & 127)]) + ((1.70273e-06 * fVec13[((IOTA - 125) & 127)]) + ((8.0341699999999997e-06 * fVec13[((IOTA - 123) & 127)]) + ((3.8827600000000008e-05 * fVec13[((IOTA - 122) & 127)]) + ((0.000101833 * fVec13[((IOTA - 121) & 127)]) + ((0.000105639 * fVec13[((IOTA - 120) & 127)]) + ((3.9296600000000006e-05 * fVec13[((IOTA - 119) & 127)]) + ((0.0013145500000000001 * fVec12[((IOTA - 42) & 127)]) + ((3.85491e-05 * fVec13[((IOTA - 117) & 127)]) + ((0.00018456300000000003 * fVec13[((IOTA - 116) & 127)]) + ((0.000184012 * fVec13[((IOTA - 115) & 127)]) + ((0.0019589099999999999 * fVec13[((IOTA - 96) & 127)]) + ((0.0031713800000000001 * fVec13[((IOTA - 95) & 127)]) + ((0.00068253400000000005 * fVec13[((IOTA - 94) & 127)]) + ((0.00056285600000000003 * fVec13[((IOTA - 91) & 127)]) + ((0.0014036900000000002 * fVec13[((IOTA - 90) & 127)]) + ((0.00337025 * fVec13[((IOTA - 87) & 127)]) + ((0.0052786200000000004 * fVec13[((IOTA - 86) & 127)]) + ((0.0015441700000000001 * fVec13[((IOTA - 85) & 127)]) + ((0.00036768800000000002 * fVec13[((IOTA - 84) & 127)]) + ((0.00069545300000000007 * fVec13[((IOTA - 83) & 127)]) + ((0.0010311999999999999 * fVec13[((IOTA - 65) & 127)]) + ((0.00113331 * fVec13[((IOTA - 64) & 127)]) + ((0.0014698599999999999 * fVec13[((IOTA - 63) & 127)]) + ((0.00070883900000000006 * fVec13[((IOTA - 62) & 127)]) + ((0.0020663499999999998 * fVec13[((IOTA - 57) & 127)]) + ((0.00288332 * fVec13[((IOTA - 56) & 127)]) + ((0.0008817050000000001 * fVec13[((IOTA - 55) & 127)]) + ((0.0035919900000000002 * fVec13[((IOTA - 54) & 127)]) + ((0.0016634199999999999 * fVec13[((IOTA - 53) & 127)]) + ((0.0012247 * fVec13[((IOTA - 52) & 127)]) + ((0.00117636 * fVec13[((IOTA - 51) & 127)]) + ((0.0053477200000000003 * fVec13[((IOTA - 49) & 127)]) + ((0.00571347 * fVec13[((IOTA - 48) & 127)]) + ((0.0068109299999999998 * fVec13[((IOTA - 45) & 127)]) + ((0.00503769 * fVec13[((IOTA - 44) & 127)]) + ((0.0033690899999999999 * fVec13[((IOTA - 43) & 127)]) + ((0.0081425600000000001 * fVec13[((IOTA - 42) & 127)]) + ((0.0048383499999999999 * fVec13[((IOTA - 39) & 127)]) + ((1.73416e-05 * fVec13[((IOTA - 38) & 127)]) + ((0.0011593500000000002 * fVec13[((IOTA - 37) & 127)]) + ((0.00341282 * fVec13[((IOTA - 36) & 127)]) + ((0.0053672899999999994 * fVec13[((IOTA - 34) & 127)]) + ((0.0060068300000000003 * fVec13[((IOTA - 33) & 127)]) + ((0.036282299999999996 * fVec13[((IOTA - 30) & 127)]) + ((0.0096774400000000007 * fVec13[((IOTA - 29) & 127)]) + ((0.03066 * fVec13[((IOTA - 26) & 127)]) + ((0.0019489700000000002 * fVec13[((IOTA - 23) & 127)]) + ((9.4065400000000002e-05 * fVec13[((IOTA - 22) & 127)]) + ((0.000702438 * fVec13[((IOTA - 20) & 127)]) + ((0.0174746 * fVec13[((IOTA - 18) & 127)]) + ((0.021190899999999999 * fVec13[((IOTA - 17) & 127)]) + ((0.0025345699999999999 * fVec13[((IOTA - 16) & 127)]) + ((0.0025992000000000003 * fVec13[((IOTA - 15) & 127)]) + ((0.0061892400000000004 * fVec13[((IOTA - 13) & 127)]) + ((0.0020100199999999999 * fVec13[((IOTA - 10) & 127)]) + ((0.00148817 * fVec13[((IOTA - 8) & 127)]) + ((0.00053443399999999997 * fVec13[((IOTA - 6) & 127)]) + ((0.00069470999999999997 * fVec13[((IOTA - 4) & 127)]) + ((0.00013874000000000002 * fVec13[((IOTA - 2) & 127)]) + ((2.49699e-05 * fVec13[((IOTA - 1) & 127)]) + ((9.9362800000000004e-06 * fTemp13) + ((2.4082099999999999e-06 * fVec14[((IOTA - 126) & 127)]) + ((8.464169999999999e-06 * fVec14[((IOTA - 125) & 127)]) + ((8.8836499999999992e-06 * fVec14[((IOTA - 124) & 127)]) + ((8.7967399999999994e-06 * fVec14[((IOTA - 123) & 127)]) + ((2.4375700000000001e-05 * fVec14[((IOTA - 122) & 127)]) + ((8.0066800000000004e-05 * fVec14[((IOTA - 121) & 127)]) + ((9.9426100000000014e-05 * fVec14[((IOTA - 120) & 127)]) + ((9.0816700000000014e-05 * fVec14[((IOTA - 119) & 127)]) + ((5.80835e-05 * fVec14[((IOTA - 118) & 127)]) + ((7.7452699999999998e-05 * fVec14[((IOTA - 117) & 127)]) + ((9.5024799999999997e-07 * fVec14[((IOTA - 116) & 127)]) + ((5.1149400000000002e-05 * fVec14[((IOTA - 98) & 127)]) + ((0.00032476299999999999 * fVec14[((IOTA - 97) & 127)]) + ((0.0012872900000000002 * fVec14[((IOTA - 94) & 127)]) + ((0.0022912900000000001 * fVec14[((IOTA - 93) & 127)]) + ((0.00119739 * fVec14[((IOTA - 92) & 127)]) + ((0.0019518999999999999 * fVec14[((IOTA - 90) & 127)]) + ((0.0043689900000000005 * fVec14[((IOTA - 89) & 127)]) + ((0.00375801 * fVec14[((IOTA - 88) & 127)]) + ((0.0016994499999999999 * fVec14[((IOTA - 87) & 127)]) + ((0.0032713800000000004 * fVec14[((IOTA - 86) & 127)]) + ((0.0036832600000000003 * fVec14[((IOTA - 85) & 127)]) + ((0.00089328000000000009 * fVec14[((IOTA - 84) & 127)]) + ((0.00072158200000000002 * fVec14[((IOTA - 83) & 127)]) + ((0.00038337700000000001 * fVec14[((IOTA - 80) & 127)]) + ((0.0016079900000000001 * fVec14[((IOTA - 79) & 127)]) + ((0.0020680200000000003 * fVec14[((IOTA - 78) & 127)]) + ((0.00028545500000000005 * fVec14[((IOTA - 77) & 127)]) + ((0.00025900400000000002 * fVec14[((IOTA - 75) & 127)]) + ((3.3718100000000005e-05 * fVec14[((IOTA - 63) & 127)]) + ((0.0016691100000000001 * fVec14[((IOTA - 62) & 127)]) + ((0.00089519800000000015 * fVec14[((IOTA - 60) & 127)]) + ((0.00079125400000000002 * fVec14[((IOTA - 59) & 127)]) + ((0.00099548200000000001 * fVec14[((IOTA - 58) & 127)]) + ((0.0024358800000000001 * fVec14[((IOTA - 54) & 127)]) + ((0.0056765999999999995 * fVec14[((IOTA - 53) & 127)]) + ((0.0027909200000000001 * fVec14[((IOTA - 52) & 127)]) + ((0.00077908600000000012 * fVec14[((IOTA - 51) & 127)]) + ((0.0016214599999999999 * fVec14[((IOTA - 50) & 127)]) + ((0.0042173100000000002 * fVec14[((IOTA - 48) & 127)]) + ((0.0091871399999999999 * fVec14[((IOTA - 47) & 127)]) + ((0.0013077200000000001 * fVec14[((IOTA - 46) & 127)]) + ((0.0021427500000000001 * fVec14[((IOTA - 45) & 127)]) + ((0.0022329500000000005 * fVec14[((IOTA - 44) & 127)]) + ((0.0028589500000000003 * fVec14[((IOTA - 42) & 127)]) + ((0.0070971000000000003 * fVec14[((IOTA - 41) & 127)]) + ((0.0053105900000000004 * fVec14[((IOTA - 40) & 127)]) + ((0.0030150300000000001 * fVec14[((IOTA - 39) & 127)]) + ((0.00053429399999999994 * fVec14[((IOTA - 36) & 127)]) + ((0.0164669 * fVec14[((IOTA - 35) & 127)]) + ((0.0037660800000000002 * fVec14[((IOTA - 33) & 127)]) + ((0.0086483100000000011 * fVec14[((IOTA - 32) & 127)]) + ((0.021576300000000003 * fVec14[((IOTA - 29) & 127)]) + ((0.021685099999999999 * fVec14[((IOTA - 28) & 127)]) + ((0.0057969700000000002 * fVec14[((IOTA - 26) & 127)]) + ((0.0197594 * fVec14[((IOTA - 21) & 127)]) + ((0.0121098 * fVec14[((IOTA - 20) & 127)]) + ((0.032535399999999999 * fVec14[((IOTA - 17) & 127)]) + ((0.00050339700000000005 * fVec14[((IOTA - 14) & 127)]) + ((0.00086802899999999998 * fVec14[((IOTA - 13) & 127)]) + ((0.0016583500000000001 * fVec14[((IOTA - 8) & 127)]) + ((0.00031507000000000003 * fVec14[((IOTA - 5) & 127)]) + ((0.00049461800000000005 * fVec14[((IOTA - 4) & 127)]) + ((0.00015840400000000001 * fVec14[((IOTA - 2) & 127)]) + ((2.8519800000000004e-05 * fVec14[((IOTA - 1) & 127)]) + ((4.4578800000000004e-06 * fTemp14) + ((0.0057622799999999998 * fVec14[((IOTA - 24) & 127)]) + ((1.1796500000000001e-05 * fVec0[((IOTA - 114) & 127)]) + ((0.00041981599999999999 * fVec0[((IOTA - 107) & 127)]) + ((5.3605900000000008e-05 * fVec0[((IOTA - 105) & 127)]) + ((0.00046969700000000004 * fVec0[((IOTA - 104) & 127)]) + ((0.00047704000000000008 * fVec0[((IOTA - 103) & 127)]) + ((0.00043236800000000009 * fVec0[((IOTA - 102) & 127)]) + ((0.00070973900000000003 * fVec0[((IOTA - 101) & 127)]) + ((0.000705385 * fVec0[((IOTA - 100) & 127)]) + ((0.00045614100000000008 * fVec0[((IOTA - 99) & 127)]) + ((0.00092227100000000001 * fVec0[((IOTA - 98) & 127)]) + ((0.00081300600000000002 * fVec0[((IOTA - 97) & 127)]) + ((0.000623519 * fVec0[((IOTA - 96) & 127)]) + ((0.0013659500000000001 * fVec0[((IOTA - 95) & 127)]) + ((0.00214314 * fVec0[((IOTA - 94) & 127)]) + ((0.0016777999999999999 * fVec0[((IOTA - 93) & 127)]) + ((0.00108099 * fVec0[((IOTA - 92) & 127)]) + ((6.7984800000000007e-05 * fVec0[((IOTA - 90) & 127)]) + ((0.000264467 * fVec0[((IOTA - 89) & 127)]) + ((0.00045975200000000007 * fVec0[((IOTA - 88) & 127)]) + ((0.0013345100000000001 * fVec0[((IOTA - 87) & 127)]) + ((0.0013671599999999999 * fVec0[((IOTA - 86) & 127)]) + ((0.00088433700000000001 * fVec0[((IOTA - 85) & 127)]) + ((0.0010498400000000002 * fVec0[((IOTA - 84) & 127)]) + ((0.00300031 * fVec0[((IOTA - 83) & 127)]) + ((0.00033132700000000002 * fVec0[((IOTA - 82) & 127)]) + ((2.3648600000000003e-05 * fVec0[((IOTA - 80) & 127)]) + ((0.00058793100000000009 * fVec0[((IOTA - 79) & 127)]) + ((0.0027816299999999998 * fVec0[((IOTA - 78) & 127)]) + ((0.0017765299999999999 * fVec0[((IOTA - 77) & 127)]) + ((0.00046795000000000002 * fVec0[((IOTA - 73) & 127)]) + ((0.0016152100000000002 * fVec0[((IOTA - 72) & 127)]) + ((0.058508199999999996 * fVec0[((IOTA - 22) & 127)]) + ((0.050635700000000006 * fVec0[((IOTA - 21) & 127)]) + ((0.041341200000000002 * fVec0[((IOTA - 20) & 127)]) + ((0.0214629 * fVec0[((IOTA - 19) & 127)]) + ((0.029345699999999999 * fVec0[((IOTA - 18) & 127)]) + ((0.088954799999999987 * fVec0[((IOTA - 17) & 127)]) + ((0.063658400000000004 * fVec0[((IOTA - 16) & 127)]) + ((0.078143400000000002 * fVec0[((IOTA - 15) & 127)]) + ((0.060886200000000001 * fVec0[((IOTA - 14) & 127)]) + ((0.077131900000000003 * fVec0[((IOTA - 13) & 127)]) + ((0.090551499999999993 * fVec0[((IOTA - 12) & 127)]) + ((0.0054581200000000003 * fVec0[((IOTA - 11) & 127)]) + ((0.0018110400000000001 * fVec0[((IOTA - 9) & 127)]) + ((0.00194177 * fVec0[((IOTA - 7) & 127)]) + ((0.00114689 * fVec0[((IOTA - 5) & 127)]) + ((0.00058690300000000006 * fVec0[((IOTA - 4) & 127)]) + ((0.0012299100000000001 * fVec0[((IOTA - 3) & 127)]) + ((0.00015469 * fVec0[((IOTA - 1) & 127)]) + (((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((0.0079812000000000008 * fVec15[((IOTA - 37) & 127)]) + ((0.0141227 * fVec15[((IOTA - 36) & 127)]) + ((0.0044680700000000002 * fVec15[((IOTA - 35) & 127)]) + ((0.020023599999999999 * fVec15[((IOTA - 34) & 127)]) + ((0.023001999999999998 * fVec15[((IOTA - 33) & 127)]) + ((0.0057082499999999998 * fVec15[((IOTA - 32) & 127)]) + ((0.011366300000000001 * fVec15[((IOTA - 31) & 127)]) + ((0.0266261 * fVec15[((IOTA - 30) & 127)]) + ((0.0087064099999999995 * fVec15[((IOTA - 29) & 127)]) + ((0.030624600000000002 * fVec15[((IOTA - 27) & 127)]) + ((0.0069301900000000001 * fVec15[((IOTA - 38) & 127)]) + (((((((((((((((((((((((((2.5118200000000003e-05 * fTemp15) + (0.00012370799999999999 * fVec15[((IOTA - 1) & 127)])) + (0.00096838400000000008 * fVec15[((IOTA - 3) & 127)])) + (0.00059949000000000005 * fVec15[((IOTA - 4) & 127)])) + (0.00076772799999999997 * fVec15[((IOTA - 5) & 127)])) + (0.00014117900000000001 * fVec15[((IOTA - 6) & 127)])) + (0.0018634200000000002 * fVec15[((IOTA - 7) & 127)])) + (0.00024957099999999997 * fVec15[((IOTA - 8) & 127)])) + (0.0011589199999999999 * fVec15[((IOTA - 9) & 127)])) + (0.00050879500000000004 * fVec15[((IOTA - 10) & 127)])) + (0.0036302599999999997 * fVec15[((IOTA - 11) & 127)])) + (0.056510400000000002 * fVec15[((IOTA - 12) & 127)])) + (0.049342300000000006 * fVec15[((IOTA - 13) & 127)])) + (0.0449074 * fVec15[((IOTA - 14) & 127)])) + (0.064749100000000004 * fVec15[((IOTA - 15) & 127)])) + (0.059606500000000007 * fVec15[((IOTA - 16) & 127)])) + (0.073495100000000008 * fVec15[((IOTA - 17) & 127)])) + (0.034514800000000005 * fVec15[((IOTA - 18) & 127)])) + (0.055772500000000003 * fVec15[((IOTA - 19) & 127)])) + (0.06596840000000001 * fVec15[((IOTA - 20) & 127)])) + (0.057809200000000005 * fVec15[((IOTA - 21) & 127)])) + (0.096895100000000012 * fVec15[((IOTA - 22) & 127)])) + (0.021260899999999999 * fVec15[((IOTA - 23) & 127)])) + (0.031746299999999998 * fVec15[((IOTA - 25) & 127)])) + (0.0528391 * fVec15[((IOTA - 26) & 127)]))))))))))))) + (0.011589100000000001 * fVec15[((IOTA - 39) & 127)])) + (0.0064973000000000001 * fVec15[((IOTA - 40) & 127)])) + (0.0056945699999999995 * fVec15[((IOTA - 41) & 127)])) + (0.0074329900000000004 * fVec15[((IOTA - 42) & 127)])) + (0.0028857800000000001 * fVec15[((IOTA - 43) & 127)])) + (0.0079248200000000008 * fVec15[((IOTA - 44) & 127)])) + (0.015005800000000001 * fVec15[((IOTA - 45) & 127)])) + (0.00250454 * fVec15[((IOTA - 46) & 127)])) + (0.00306785 * fVec15[((IOTA - 48) & 127)])) + (0.00175234 * fVec15[((IOTA - 49) & 127)])) + (0.00086377000000000014 * fVec15[((IOTA - 50) & 127)])) + (0.0036652500000000001 * fVec15[((IOTA - 51) & 127)])) + (0.0028833999999999999 * fVec15[((IOTA - 52) & 127)])) + (0.00325793 * fVec15[((IOTA - 53) & 127)])) + (0.00305273 * fVec15[((IOTA - 54) & 127)])) + (0.0012461600000000001 * fVec15[((IOTA - 55) & 127)])) + (0.0012118500000000002 * fVec15[((IOTA - 56) & 127)])) + (0.0050028199999999998 * fVec15[((IOTA - 57) & 127)])) + (0.0036129299999999999 * fVec15[((IOTA - 58) & 127)])) + (0.0031211899999999998 * fVec15[((IOTA - 59) & 127)])) + (0.0042250600000000001 * fVec15[((IOTA - 60) & 127)])) + (0.0035775299999999998 * fVec15[((IOTA - 61) & 127)])) + (0.0068040700000000006 * fVec15[((IOTA - 62) & 127)])) + (0.0062385100000000001 * fVec15[((IOTA - 63) & 127)])) + (0.0042152999999999999 * fVec15[((IOTA - 64) & 127)])) + (0.0027403499999999999 * fVec15[((IOTA - 65) & 127)])) + (0.0023089200000000003 * fVec15[((IOTA - 66) & 127)])) + (0.0034284599999999999 * fVec15[((IOTA - 67) & 127)])) + (0.0022559300000000002 * fVec15[((IOTA - 68) & 127)])) + (0.0014647 * fVec15[((IOTA - 69) & 127)])) + (0.00185049 * fVec15[((IOTA - 70) & 127)])) + (0.0032107500000000001 * fVec15[((IOTA - 71) & 127)])) + (0.0052356299999999998 * fVec15[((IOTA - 72) & 127)])) + (0.0030868900000000001 * fVec15[((IOTA - 73) & 127)])) + (0.0029129100000000003 * fVec15[((IOTA - 74) & 127)])) + (0.0063743100000000002 * fVec15[((IOTA - 75) & 127)])) + (0.0041945800000000007 * fVec15[((IOTA - 76) & 127)])) + (0.00082847699999999999 * fVec15[((IOTA - 77) & 127)])) + (0.0024405 * fVec15[((IOTA - 78) & 127)])) + (0.0041905200000000005 * fVec15[((IOTA - 79) & 127)])) + (0.0024365300000000001 * fVec15[((IOTA - 80) & 127)])) + (0.00056369199999999997 * fVec15[((IOTA - 82) & 127)])) + (0.0015701700000000001 * fVec15[((IOTA - 83) & 127)])) + (0.0016799500000000001 * fVec15[((IOTA - 85) & 127)])) + (0.0051757600000000006 * fVec15[((IOTA - 86) & 127)])) + (0.0037083100000000003 * fVec15[((IOTA - 87) & 127)])) + (0.00041329900000000006 * fVec15[((IOTA - 90) & 127)])) + (2.3955000000000004e-05 * fVec15[((IOTA - 93) & 127)])) + (0.0021461499999999999 * fVec15[((IOTA - 94) & 127)])) + (0.00272945 * fVec15[((IOTA - 95) & 127)])) + (0.00054874000000000004 * fVec15[((IOTA - 96) & 127)])) + (0.00058470199999999999 * fVec15[((IOTA - 98) & 127)])) + (0.00061282500000000011 * fVec15[((IOTA - 99) & 127)])) + (0.00050683400000000006 * fVec15[((IOTA - 100) & 127)])) + (0.00087727600000000008 * fVec15[((IOTA - 101) & 127)])) + (0.00154024 * fVec15[((IOTA - 102) & 127)])) + (0.00185348 * fVec15[((IOTA - 103) & 127)])) + (0.0012065399999999999 * fVec15[((IOTA - 104) & 127)])) + (0.0014059200000000002 * fVec15[((IOTA - 105) & 127)])) + (0.00136699 * fVec15[((IOTA - 106) & 127)])) + (0.0012923000000000001 * fVec15[((IOTA - 107) & 127)])) + (0.0009436510000000001 * fVec15[((IOTA - 108) & 127)])) + (0.00083451500000000008 * fVec15[((IOTA - 109) & 127)])) + (0.00098166300000000002 * fVec15[((IOTA - 110) & 127)])) + (0.00067625599999999997 * fVec15[((IOTA - 111) & 127)])) + (0.00050102000000000002 * fVec15[((IOTA - 112) & 127)])) + (0.00053358400000000003 * fVec15[((IOTA - 113) & 127)])) + (0.00054736599999999998 * fVec15[((IOTA - 114) & 127)])) + (0.00045382400000000002 * fVec15[((IOTA - 115) & 127)])) + (0.00034160000000000001 * fVec15[((IOTA - 116) & 127)])) + (0.00025855300000000002 * fVec15[((IOTA - 117) & 127)])) + (0.00016872499999999999 * fVec15[((IOTA - 118) & 127)])) + (0.00016404399999999999 * fVec15[((IOTA - 119) & 127)])) + (0.00010654600000000001 * fVec15[((IOTA - 120) & 127)])) + (4.2541300000000006e-05 * fVec15[((IOTA - 121) & 127)])) + (1.8717900000000003e-05 * fVec15[((IOTA - 122) & 127)])) + (2.09787e-05 * fVec15[((IOTA - 123) & 127)])) + (5.7126099999999992e-06 * fVec15[((IOTA - 124) & 127)])) + (5.0813899999999995e-06 * fVec15[((IOTA - 125) & 127)])) + (2.0625300000000001e-06 * fVec15[((IOTA - 126) & 127)])) + (3.3955100000000003e-05 * fTemp0))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.000682276 * fVec2[((IOTA - 3) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))) - ((6.4276900000000001e-05 * fVec2[((IOTA - 119) & 127)]) + ((9.164930000000001e-05 * fVec2[((IOTA - 118) & 127)]) + ((5.7182200000000003e-05 * fVec2[((IOTA - 117) & 127)]) + ((0.00012319500000000002 * fVec2[((IOTA - 116) & 127)]) + ((0.000211704 * fVec2[((IOTA - 115) & 127)]) + ((0.00034920800000000005 * fVec2[((IOTA - 114) & 127)]) + ((0.00033244200000000001 * fVec2[((IOTA - 113) & 127)]) + ((0.00025251100000000002 * fVec2[((IOTA - 112) & 127)]) + ((0.000318859 * fVec2[((IOTA - 111) & 127)]) + ((0.00039344000000000005 * fVec2[((IOTA - 110) & 127)]) + ((0.00033167 * fVec2[((IOTA - 109) & 127)]) + ((0.000465384 * fVec2[((IOTA - 108) & 127)]) + ((0.00039361300000000001 * fVec2[((IOTA - 107) & 127)]) + ((0.00042075400000000002 * fVec2[((IOTA - 106) & 127)]) + ((0.00037080799999999998 * fVec2[((IOTA - 105) & 127)]) + ((1.6587200000000003e-05 * fVec2[((IOTA - 103) & 127)]) + ((0.00091861900000000008 * fVec2[((IOTA - 102) & 127)]) + ((0.00025743700000000001 * fVec2[((IOTA - 101) & 127)]) + ((1.0584400000000002e-05 * fVec2[((IOTA - 98) & 127)]) + ((0.00040115600000000006 * fVec2[((IOTA - 97) & 127)]) + ((0.0014677900000000001 * fVec2[((IOTA - 90) & 127)]) + ((0.0010665400000000002 * fVec2[((IOTA - 89) & 127)]) + ((0.0011811600000000001 * fVec2[((IOTA - 88) & 127)]) + ((0.00093001200000000003 * fVec2[((IOTA - 87) & 127)]) + ((0.00025672299999999999 * fVec2[((IOTA - 85) & 127)]) + ((3.6903100000000002e-05 * fVec2[((IOTA - 83) & 127)]) + ((0.00082218600000000003 * fVec2[((IOTA - 82) & 127)]) + ((0.000382178 * fVec2[((IOTA - 81) & 127)]) + ((0.00030066700000000001 * fVec2[((IOTA - 77) & 127)]) + ((0.00056339900000000002 * fVec2[((IOTA - 78) & 127)]) + ((0.00075356000000000002 * fVec2[((IOTA - 76) & 127)]) + ((0.0017828800000000001 * fVec2[((IOTA - 75) & 127)]) + ((0.00080830800000000009 * fVec2[((IOTA - 74) & 127)]) + ((0.00187645 * fVec2[((IOTA - 70) & 127)]) + ((0.0013177900000000001 * fVec2[((IOTA - 69) & 127)]) + ((0.0026788900000000002 * fVec2[((IOTA - 68) & 127)]) + ((0.0014275899999999998 * fVec2[((IOTA - 67) & 127)]) + ((0.00077358800000000008 * fVec2[((IOTA - 65) & 127)]) + ((0.00030791900000000003 * fVec2[((IOTA - 64) & 127)]) + ((0.0018273299999999998 * fVec2[((IOTA - 59) & 127)]) + ((0.000741378 * fVec2[((IOTA - 58) & 127)]) + ((0.0021143999999999998 * fVec2[((IOTA - 56) & 127)]) + ((0.0023650199999999998 * fVec2[((IOTA - 55) & 127)]) + ((0.00137963 * fVec2[((IOTA - 46) & 127)]) + ((0.0022811300000000001 * fVec2[((IOTA - 45) & 127)]) + ((0.0077523699999999997 * fVec2[((IOTA - 44) & 127)]) + ((0.0087150300000000003 * fVec2[((IOTA - 43) & 127)]) + ((0.0035589599999999999 * fVec2[((IOTA - 40) & 127)]) + ((0.009139089999999999 * fVec2[((IOTA - 37) & 127)]) + ((0.0022023799999999999 * fVec2[((IOTA - 36) & 127)]) + ((0.00043536700000000003 * fVec2[((IOTA - 35) & 127)]) + ((0.0065159300000000005 * fVec2[((IOTA - 34) & 127)]) + ((0.0023690899999999999 * fVec2[((IOTA - 33) & 127)]) + ((0.017782599999999999 * fVec2[((IOTA - 32) & 127)]) + ((0.0058807299999999998 * fVec2[((IOTA - 31) & 127)]) + ((0.033734800000000002 * fVec2[((IOTA - 28) & 127)]) + ((0.00030687999999999999 * fVec2[((IOTA - 27) & 127)]) + ((0.027150500000000001 * fVec2[((IOTA - 25) & 127)]) + ((0.018427300000000001 * fVec2[((IOTA - 24) & 127)]) + ((0.037316000000000002 * fVec2[((IOTA - 18) & 127)]) + ((0.014252300000000001 * fVec2[((IOTA - 17) & 127)]) + ((0.019724700000000001 * fVec2[((IOTA - 15) & 127)]) + ((0.00039235000000000002 * fVec2[((IOTA - 14) & 127)]) + ((0.016522600000000002 * fVec2[((IOTA - 13) & 127)]) + ((0.0057737600000000002 * fVec2[((IOTA - 10) & 127)]) + ((0.00298131 * fVec2[((IOTA - 8) & 127)]) + ((0.0021578599999999997 * fVec2[((IOTA - 6) & 127)]) + ((0.0006632360000000001 * fVec2[((IOTA - 4) & 127)]) + ((0.00032355000000000003 * fVec2[((IOTA - 2) & 127)]) + ((4.3569800000000004e-05 * fVec2[((IOTA - 1) & 127)]) + (((7.3541399999999999e-06 * fVec1[((IOTA - 125) & 127)]) + ((1.9871400000000001e-05 * fVec1[((IOTA - 124) & 127)]) + ((1.68007e-05 * fVec1[((IOTA - 123) & 127)]) + ((4.0472700000000003e-05 * fVec1[((IOTA - 120) & 127)]) + ((2.0089899999999999e-06 * fVec1[((IOTA - 117) & 127)]) + ((9.4388500000000013e-05 * fVec1[((IOTA - 116) & 127)]) + ((4.0048000000000007e-05 * fVec1[((IOTA - 115) & 127)]) + ((0.00013508199999999999 * fVec1[((IOTA - 114) & 127)]) + ((1.4516400000000002e-05 * fVec1[((IOTA - 112) & 127)]) + ((4.0951399999999994e-06 * fVec1[((IOTA - 109) & 127)]) + ((0.00038113700000000003 * fVec1[((IOTA - 108) & 127)]) + ((0.00038692400000000002 * fVec1[((IOTA - 90) & 127)]) + ((0.0011128800000000001 * fVec1[((IOTA - 87) & 127)]) + ((0.00018401499999999999 * fVec1[((IOTA - 86) & 127)]) + ((8.3155100000000006e-05 * fVec1[((IOTA - 84) & 127)]) + ((0.00055631600000000006 * fVec1[((IOTA - 83) & 127)]) + ((0.0013639500000000001 * fVec1[((IOTA - 82) & 127)]) + ((0.0011469 * fVec1[((IOTA - 81) & 127)]) + ((0.0022757299999999997 * fVec1[((IOTA - 78) & 127)]) + ((0.00076008400000000006 * fVec1[((IOTA - 77) & 127)]) + ((0.00049322500000000002 * fVec1[((IOTA - 75) & 127)]) + ((7.2509299999999999e-06 * fVec1[((IOTA - 74) & 127)]) + ((0.00102811 * fVec1[((IOTA - 73) & 127)]) + ((0.00084844100000000013 * fVec1[((IOTA - 72) & 127)]) + ((0.00073845000000000009 * fVec1[((IOTA - 71) & 127)]) + ((0.00068371000000000003 * fVec1[((IOTA - 70) & 127)]) + ((7.2511700000000006e-05 * fVec1[((IOTA - 69) & 127)]) + ((0.00078590300000000001 * fVec1[((IOTA - 66) & 127)]) + ((0.00095934000000000015 * fVec1[((IOTA - 62) & 127)]) + ((0.000404712 * fVec1[((IOTA - 61) & 127)]) + ((0.00023097600000000001 * fVec1[((IOTA - 57) & 127)]) + ((0.00218833 * fVec1[((IOTA - 53) & 127)]) + ((0.0010072700000000002 * fVec1[((IOTA - 52) & 127)]) + ((0.00137375 * fVec1[((IOTA - 51) & 127)]) + ((0.00083448600000000006 * fVec1[((IOTA - 50) & 127)]) + ((0.00081536399999999995 * fVec1[((IOTA - 49) & 127)]) + ((0.00054543100000000008 * fVec1[((IOTA - 48) & 127)]) + ((0.0035057600000000001 * fVec1[((IOTA - 47) & 127)]) + ((0.0042825600000000004 * fVec1[((IOTA - 46) & 127)]) + ((0.0073746600000000008 * fVec1[((IOTA - 43) & 127)]) + ((0.0031813100000000001 * fVec1[((IOTA - 41) & 127)]) + ((0.0031712400000000001 * fVec1[((IOTA - 40) & 127)]) + ((0.0019387000000000002 * fVec1[((IOTA - 38) & 127)]) + ((0.010274799999999999 * fVec1[((IOTA - 37) & 127)]) + ((0.0160001 * fVec1[((IOTA - 34) & 127)]) + ((0.015218700000000002 * fVec1[((IOTA - 33) & 127)]) + ((0.0051433299999999998 * fVec1[((IOTA - 29) & 127)]) + ((0.019054000000000001 * fVec1[((IOTA - 28) & 127)]) + ((0.029220799999999998 * fVec1[((IOTA - 22) & 127)]) + ((0.019108199999999999 * fVec1[((IOTA - 20) & 127)]) + ((0.0101244 * fVec1[((IOTA - 19) & 127)]) + ((0.0243991 * fVec1[((IOTA - 17) & 127)]) + ((0.0071943800000000002 * fVec1[((IOTA - 16) & 127)]) + ((0.0065491000000000004 * fVec1[((IOTA - 14) & 127)]) + ((0.00345408 * fVec1[((IOTA - 13) & 127)]) + ((0.000172325 * fVec1[((IOTA - 11) & 127)]) + ((4.8090900000000004e-05 * fVec1[((IOTA - 10) & 127)]) + ((0.0012270600000000001 * fVec1[((IOTA - 8) & 127)]) + ((0.00054008200000000011 * fVec1[((IOTA - 4) & 127)]) + ((2.44107e-05 * fVec1[((IOTA - 2) & 127)]) + ((0.00057432400000000008 * fVec1[((IOTA - 96) & 127)]) + ((0.00025360600000000002 * fVec1[((IOTA - 107) & 127)]) + ((8.5443400000000003e-05 * fVec1[((IOTA - 97) & 127)]) + ((5.2891500000000002e-07 * fVec3[((IOTA - 126) & 127)]) + ((7.5122799999999995e-06 * fVec3[((IOTA - 125) & 127)]) + ((1.6393300000000001e-05 * fVec3[((IOTA - 124) & 127)]) + ((2.3049300000000003e-05 * fVec3[((IOTA - 123) & 127)]) + ((2.3497200000000003e-05 * fVec3[((IOTA - 122) & 127)]) + ((3.1160000000000007e-05 * fVec3[((IOTA - 121) & 127)]) + ((2.6824200000000003e-05 * fVec3[((IOTA - 120) & 127)]) + ((2.2243600000000002e-05 * fVec3[((IOTA - 117) & 127)]) + ((4.78325e-05 * fVec3[((IOTA - 116) & 127)]) + ((0.00013262100000000003 * fVec3[((IOTA - 94) & 127)]) + ((0.00061235200000000004 * fVec3[((IOTA - 91) & 127)]) + ((0.00083076200000000008 * fVec3[((IOTA - 90) & 127)]) + ((0.00094943600000000012 * fVec3[((IOTA - 89) & 127)]) + ((0.00016757299999999999 * fVec3[((IOTA - 88) & 127)]) + ((0.00022032700000000001 * fVec3[((IOTA - 87) & 127)]) + ((0.00016127700000000001 * fVec3[((IOTA - 86) & 127)]) + ((0.00191763 * fVec3[((IOTA - 85) & 127)]) + ((0.00189814 * fVec3[((IOTA - 84) & 127)]) + ((0.0011356999999999999 * fVec3[((IOTA - 82) & 127)]) + ((0.0018585000000000001 * fVec3[((IOTA - 81) & 127)]) + ((0.0012131000000000002 * fVec3[((IOTA - 80) & 127)]) + ((0.00032995300000000002 * fVec3[((IOTA - 6) & 127)]) + ((0.00050478700000000003 * fVec3[((IOTA - 4) & 127)]) + ((0.000105713 * fVec3[((IOTA - 2) & 127)]) + ((2.5231199999999997e-06 * fTemp3) + ((0.0066805500000000004 * fVec3[((IOTA - 18) & 127)]) + ((0.00678927 * fVec3[((IOTA - 17) & 127)]) + ((0.0091073999999999999 * fVec3[((IOTA - 16) & 127)]) + ((0.0130569 * fVec3[((IOTA - 15) & 127)]) + ((0.0042139100000000004 * fVec3[((IOTA - 13) & 127)]) + ((0.00025399999999999999 * fVec3[((IOTA - 79) & 127)]) + ((0.00035679200000000004 * fVec3[((IOTA - 78) & 127)]) + ((0.00097909699999999995 * fVec3[((IOTA - 77) & 127)]) + ((0.0021205199999999999 * fVec3[((IOTA - 76) & 127)]) + ((0.0008138310000000001 * fVec3[((IOTA - 68) & 127)]) + ((0.00074145100000000007 * fVec3[((IOTA - 62) & 127)]) + ((0.00047705600000000001 * fVec3[((IOTA - 61) & 127)]) + ((0.0034899700000000002 * fVec3[((IOTA - 60) & 127)]) + ((0.0025903700000000003 * fVec3[((IOTA - 59) & 127)]) + ((0.0027718299999999999 * fVec3[((IOTA - 58) & 127)]) + ((0.00315957 * fVec3[((IOTA - 57) & 127)]) + ((0.0041676700000000001 * fVec3[((IOTA - 56) & 127)]) + ((0.0040106600000000001 * fVec3[((IOTA - 55) & 127)]) + ((0.00024623100000000002 * fVec3[((IOTA - 54) & 127)]) + ((0.00327415 * fVec3[((IOTA - 53) & 127)]) + ((0.0026408399999999998 * fVec3[((IOTA - 52) & 127)]) + ((0.00050010199999999999 * fVec3[((IOTA - 50) & 127)]) + ((0.00058122499999999999 * fVec3[((IOTA - 49) & 127)]) + ((0.0072108700000000003 * fVec3[((IOTA - 44) & 127)]) + ((0.0018301400000000001 * fVec3[((IOTA - 43) & 127)]) + ((0.00058247500000000005 * fVec3[((IOTA - 38) & 127)]) + ((0.0085527399999999996 * fVec3[((IOTA - 37) & 127)]) + ((0.0064438700000000008 * fVec3[((IOTA - 35) & 127)]) + ((0.0030920700000000001 * fVec3[((IOTA - 31) & 127)]) + ((0.026311300000000003 * fVec3[((IOTA - 30) & 127)]) + ((0.0045847700000000002 * fVec3[((IOTA - 29) & 127)]) + ((0.014478500000000002 * fVec3[((IOTA - 26) & 127)]) + ((0.014742699999999999 * fVec3[((IOTA - 25) & 127)]) + ((0.0040668100000000006 * fVec3[((IOTA - 23) & 127)]) + ((0.010288200000000001 * fVec3[((IOTA - 21) & 127)]) + ((0.016673299999999999 * fVec3[((IOTA - 20) & 127)]) + ((7.5180799999999995e-07 * fVec4[((IOTA - 126) & 127)]) + ((7.4192699999999999e-06 * fVec4[((IOTA - 125) & 127)]) + ((7.8058900000000002e-06 * fVec4[((IOTA - 124) & 127)]) + ((7.6831699999999991e-06 * fVec4[((IOTA - 123) & 127)]) + ((4.3886799999999996e-07 * fVec4[((IOTA - 122) & 127)]) + ((2.5728499999999999e-05 * fVec4[((IOTA - 121) & 127)]) + ((3.8519100000000002e-06 * fVec4[((IOTA - 120) & 127)]) + ((8.4650599999999985e-06 * fVec4[((IOTA - 119) & 127)]) + ((4.1315700000000002e-05 * fVec4[((IOTA - 118) & 127)]) + ((0.00014156200000000002 * fVec4[((IOTA - 117) & 127)]) + ((0.00013138900000000001 * fVec4[((IOTA - 116) & 127)]) + ((2.3594499999999999e-05 * fVec4[((IOTA - 114) & 127)]) + ((5.5362000000000003e-05 * fVec4[((IOTA - 113) & 127)]) + ((4.5982300000000002e-05 * fVec4[((IOTA - 112) & 127)]) + ((0.00085768200000000002 * fVec3[((IOTA - 8) & 127)]) + ((0.00020877800000000001 * fVec4[((IOTA - 95) & 127)]) + ((0.0014440100000000001 * fVec4[((IOTA - 94) & 127)]) + ((7.5196800000000005e-05 * fVec4[((IOTA - 93) & 127)]) + ((0.00021774900000000002 * fVec4[((IOTA - 88) & 127)]) + ((0.00050493599999999999 * fVec4[((IOTA - 87) & 127)]) + ((0.00129094 * fVec4[((IOTA - 86) & 127)]) + ((0.00131875 * fVec4[((IOTA - 85) & 127)]) + ((0.00025035200000000002 * fVec4[((IOTA - 84) & 127)]) + ((0.0016813900000000001 * fVec4[((IOTA - 83) & 127)]) + ((0.00079413300000000004 * fVec4[((IOTA - 82) & 127)]) + ((0.00148109 * fVec4[((IOTA - 78) & 127)]) + ((0.00187816 * fVec4[((IOTA - 77) & 127)]) + ((0.0015205800000000001 * fVec4[((IOTA - 76) & 127)]) + ((0.00021227600000000001 * fVec4[((IOTA - 75) & 127)]) + ((0.00359524 * fVec4[((IOTA - 74) & 127)]) + ((0.0041234900000000005 * fVec4[((IOTA - 73) & 127)]) + ((0.00089886800000000004 * fVec4[((IOTA - 72) & 127)]) + ((0.0012871 * fVec4[((IOTA - 71) & 127)]) + ((0.0018243199999999999 * fVec4[((IOTA - 37) & 127)]) + ((0.0072907000000000007 * fVec4[((IOTA - 36) & 127)]) + ((0.0082141100000000002 * fVec4[((IOTA - 35) & 127)]) + ((0.0015148500000000001 * fVec4[((IOTA - 33) & 127)]) + ((0.0039258799999999996 * fVec4[((IOTA - 32) & 127)]) + ((0.0105468 * fVec4[((IOTA - 30) & 127)]) + ((0.0083919700000000003 * fVec4[((IOTA - 29) & 127)]) + ((0.045109500000000004 * fVec4[((IOTA - 24) & 127)]) + ((0.0130615 * fVec4[((IOTA - 23) & 127)]) + ((0.0068680599999999996 * fVec4[((IOTA - 20) & 127)]) + ((0.0158055 * fVec4[((IOTA - 19) & 127)]) + ((0.0196975 * fVec4[((IOTA - 18) & 127)]) + ((0.012670500000000001 * fVec4[((IOTA - 17) & 127)]) + ((0.017149600000000001 * fVec4[((IOTA - 14) & 127)]) + ((0.00039274200000000002 * fVec4[((IOTA - 13) & 127)]) + ((0.00063474100000000008 * fVec4[((IOTA - 8) & 127)]) + ((0.00022428400000000002 * fVec4[((IOTA - 6) & 127)]) + ((0.00061255200000000004 * fVec4[((IOTA - 5) & 127)]) + ((0.000177443 * fVec4[((IOTA - 2) & 127)]) + ((2.38744e-05 * fVec4[((IOTA - 1) & 127)]) + ((0.0036414500000000001 * fVec4[((IOTA - 42) & 127)]) + ((0.0024519899999999998 * fVec4[((IOTA - 41) & 127)]) + ((0.0018163100000000002 * fVec4[((IOTA - 65) & 127)]) + ((0.0033862499999999999 * fVec4[((IOTA - 62) & 127)]) + ((0.00095073000000000011 * fVec4[((IOTA - 61) & 127)]) + ((0.00032132100000000001 * fVec4[((IOTA - 55) & 127)]) + ((0.0011200699999999999 * fVec4[((IOTA - 53) & 127)]) + ((0.00093541399999999997 * fVec4[((IOTA - 39) & 127)]) + ((0.0028892500000000003 * fVec4[((IOTA - 50) & 127)]) + ((0.000955261 * fVec4[((IOTA - 49) & 127)]) + ((6.3343100000000014e-05 * fVec4[((IOTA - 47) & 127)]) + ((0.00694716 * fVec4[((IOTA - 44) & 127)]) + ((0.0026067200000000003 * fVec4[((IOTA - 43) & 127)]) + ((5.1650600000000005e-06 * fVec5[((IOTA - 122) & 127)]) + ((6.2288800000000013e-05 * fVec5[((IOTA - 118) & 127)]) + ((0.000107544 * fVec5[((IOTA - 117) & 127)]) + ((3.1495500000000004e-05 * fVec5[((IOTA - 116) & 127)]) + ((1.20038e-05 * fVec5[((IOTA - 115) & 127)]) + ((2.5178300000000003e-05 * fVec5[((IOTA - 114) & 127)]) + ((0.000470283 * fVec5[((IOTA - 113) & 127)]) + ((0.00042444499999999997 * fVec5[((IOTA - 112) & 127)]) + ((0.00022263799999999999 * fVec5[((IOTA - 111) & 127)]) + ((4.14666e-05 * fVec5[((IOTA - 109) & 127)]) + ((0.000344395 * fVec5[((IOTA - 108) & 127)]) + ((0.00011064600000000001 * fVec5[((IOTA - 106) & 127)]) + ((0.00030248100000000001 * fVec5[((IOTA - 105) & 127)]) + ((0.00041449600000000004 * fVec5[((IOTA - 104) & 127)]) + ((0.00012015400000000002 * fVec5[((IOTA - 100) & 127)]) + ((0.00051881099999999999 * fVec5[((IOTA - 99) & 127)]) + ((6.7970200000000002e-05 * fVec5[((IOTA - 98) & 127)]) + ((0.00034321600000000003 * fVec5[((IOTA - 95) & 127)]) + ((0.00044988999999999999 * fVec5[((IOTA - 88) & 127)]) + ((0.00256128 * fVec5[((IOTA - 87) & 127)]) + ((0.0023223300000000001 * fVec5[((IOTA - 86) & 127)]) + ((0.00014500400000000001 * fVec5[((IOTA - 83) & 127)]) + ((0.0011457399999999999 * fVec5[((IOTA - 79) & 127)]) + ((0.0017324700000000001 * fVec5[((IOTA - 63) & 127)]) + ((0.00083292900000000005 * fVec5[((IOTA - 60) & 127)]) + ((4.4803300000000005e-05 * fVec5[((IOTA - 59) & 127)]) + ((0.000591623 * fVec5[((IOTA - 56) & 127)]) + ((0.0017609399999999999 * fVec5[((IOTA - 47) & 127)]) + ((0.00027506900000000002 * fVec5[((IOTA - 42) & 127)]) + ((0.0020045600000000003 * fVec5[((IOTA - 41) & 127)]) + ((0.00894526 * fVec5[((IOTA - 38) & 127)]) + ((0.0040081499999999994 * fVec5[((IOTA - 37) & 127)]) + ((0.0022934100000000001 * fVec5[((IOTA - 36) & 127)]) + ((0.0078181599999999993 * fVec5[((IOTA - 35) & 127)]) + ((0.0060735200000000007 * fVec5[((IOTA - 34) & 127)]) + ((0.020101600000000001 * fVec5[((IOTA - 33) & 127)]) + ((0.011363099999999999 * fVec5[((IOTA - 32) & 127)]) + ((0.0048202200000000001 * fVec5[((IOTA - 31) & 127)]) + ((0.015268600000000002 * fVec5[((IOTA - 29) & 127)]) + ((0.054586100000000005 * fVec5[((IOTA - 28) & 127)]) + ((0.0076356100000000001 * fVec5[((IOTA - 27) & 127)]) + ((0.0064172300000000003 * fVec5[((IOTA - 24) & 127)]) + ((0.025168300000000001 * fVec5[((IOTA - 23) & 127)]) + ((0.0044173700000000003 * fVec5[((IOTA - 14) & 127)]) + ((0.047073999999999998 * fVec5[((IOTA - 13) & 127)]) + ((0.050409800000000005 * fVec5[((IOTA - 12) & 127)]) + ((0.0088254800000000001 * fVec5[((IOTA - 11) & 127)]) + ((0.0028401100000000003 * fVec5[((IOTA - 9) & 127)]) + ((0.0010134199999999999 * fVec5[((IOTA - 7) & 127)]) + ((0.00048772500000000005 * fVec5[((IOTA - 5) & 127)]) + ((0.00019671200000000001 * fVec5[((IOTA - 3) & 127)]) + ((1.9656400000000004e-05 * fVec5[((IOTA - 1) & 127)]) + ((1.1230600000000001e-05 * fTemp5) + ((0.00048183400000000005 * fVec5[((IOTA - 66) & 127)]) + ((0.00095134899999999999 * fVec5[((IOTA - 78) & 127)]) + ((0.00214124 * fVec5[((IOTA - 75) & 127)]) + ((0.0040635300000000001 * fVec5[((IOTA - 74) & 127)]) + ((0.00039275000000000003 * fVec5[((IOTA - 73) & 127)]) + ((0.0024424899999999999 * fVec5[((IOTA - 64) & 127)]) + ((0.0021799499999999999 * fVec5[((IOTA - 71) & 127)]) + ((0.0024713999999999999 * fVec5[((IOTA - 70) & 127)]) + ((0.00148032 * fVec5[((IOTA - 69) & 127)]) + ((1.2052900000000001e-05 * fVec6[((IOTA - 124) & 127)]) + ((7.5001999999999998e-06 * fVec6[((IOTA - 118) & 127)]) + ((3.8559700000000003e-05 * fVec6[((IOTA - 117) & 127)]) + ((0.00012688900000000001 * fVec6[((IOTA - 113) & 127)]) + ((7.1436400000000013e-05 * fVec6[((IOTA - 112) & 127)]) + ((8.3677800000000007e-05 * fVec6[((IOTA - 111) & 127)]) + ((0.000142659 * fVec6[((IOTA - 110) & 127)]) + ((0.00011731000000000001 * fVec6[((IOTA - 107) & 127)]) + ((0.00029102700000000002 * fVec6[((IOTA - 100) & 127)]) + ((0.000736785 * fVec6[((IOTA - 99) & 127)]) + ((0.00088049500000000002 * fVec6[((IOTA - 98) & 127)]) + ((0.00076679700000000005 * fVec6[((IOTA - 94) & 127)]) + ((0.0012207399999999999 * fVec6[((IOTA - 93) & 127)]) + ((0.00193305 * fVec6[((IOTA - 92) & 127)]) + ((0.00063554600000000007 * fVec6[((IOTA - 91) & 127)]) + ((0.00026668900000000002 * fVec6[((IOTA - 89) & 127)]) + ((0.00319579 * fVec6[((IOTA - 88) & 127)]) + ((0.0011806200000000001 * fVec6[((IOTA - 87) & 127)]) + ((0.00029940000000000001 * fVec6[((IOTA - 82) & 127)]) + ((0.0028643700000000002 * fVec6[((IOTA - 81) & 127)]) + ((0.0023513099999999997 * fVec6[((IOTA - 80) & 127)]) + ((0.0017465 * fVec6[((IOTA - 77) & 127)]) + ((0.00128761 * fVec6[((IOTA - 62) & 127)]) + ((0.0015227400000000001 * fVec6[((IOTA - 60) & 127)]) + ((0.00373734 * fVec6[((IOTA - 59) & 127)]) + ((0.0014280500000000002 * fVec6[((IOTA - 58) & 127)]) + ((0.0026737700000000002 * fVec6[((IOTA - 53) & 127)]) + ((0.0024428499999999999 * fVec6[((IOTA - 50) & 127)]) + ((0.0025408400000000004 * fVec6[((IOTA - 49) & 127)]) + ((0.0022300800000000002 * fVec6[((IOTA - 48) & 127)]) + ((0.00082497000000000007 * fVec6[((IOTA - 47) & 127)]) + ((0.0010773099999999999 * fVec6[((IOTA - 46) & 127)]) + ((6.9417900000000012e-05 * fVec6[((IOTA - 45) & 127)]) + ((0.0043865800000000002 * fVec6[((IOTA - 41) & 127)]) + ((0.0046617300000000002 * fVec6[((IOTA - 40) & 127)]) + ((0.0029096 * fVec6[((IOTA - 39) & 127)]) + ((0.00817616 * fVec6[((IOTA - 36) & 127)]) + ((0.0060419900000000006 * fVec6[((IOTA - 35) & 127)]) + ((0.0085528800000000006 * fVec6[((IOTA - 34) & 127)]) + ((0.030350199999999997 * fVec6[((IOTA - 27) & 127)]) + ((0.00316892 * fVec6[((IOTA - 24) & 127)]) + ((0.025193300000000002 * fVec6[((IOTA - 23) & 127)]) + ((0.0021023399999999999 * fVec6[((IOTA - 22) & 127)]) + ((0.0073916000000000008 * fVec6[((IOTA - 20) & 127)]) + ((0.017867400000000002 * fVec6[((IOTA - 17) & 127)]) + ((0.010231500000000001 * fVec6[((IOTA - 16) & 127)]) + ((0.00146583 * fVec6[((IOTA - 14) & 127)]) + ((0.0064234600000000006 * fVec6[((IOTA - 13) & 127)]) + ((0.00078510700000000006 * fVec6[((IOTA - 8) & 127)]) + ((0.00035025799999999999 * fVec6[((IOTA - 5) & 127)]) + ((0.00013798200000000001 * fVec6[((IOTA - 4) & 127)]) + ((3.8530500000000007e-05 * fVec6[((IOTA - 1) & 127)]) + ((0.00021222400000000003 * fVec6[((IOTA - 76) & 127)]) + ((0.00041598900000000002 * fVec6[((IOTA - 73) & 127)]) + ((0.00030018800000000001 * fVec6[((IOTA - 72) & 127)]) + ((0.0020025400000000001 * fVec5[((IOTA - 65) & 127)]) + ((0.00152436 * fVec6[((IOTA - 69) & 127)]) + ((1.6178099999999998e-07 * fVec7[((IOTA - 125) & 127)]) + ((1.7421599999999998e-07 * fVec7[((IOTA - 124) & 127)]) + ((0.00021674500000000001 * fVec7[((IOTA - 115) & 127)]) + ((0.00028279200000000003 * fVec7[((IOTA - 114) & 127)]) + ((5.2515800000000003e-05 * fVec7[((IOTA - 113) & 127)]) + ((0.000133249 * fVec7[((IOTA - 112) & 127)]) + ((0.00038273500000000004 * fVec7[((IOTA - 111) & 127)]) + ((0.00043214699999999997 * fVec7[((IOTA - 110) & 127)]) + ((0.00027134800000000003 * fVec7[((IOTA - 109) & 127)]) + ((3.3736500000000005e-05 * fVec7[((IOTA - 107) & 127)]) + ((6.6819399999999999e-05 * fVec7[((IOTA - 106) & 127)]) + ((0.00038405399999999999 * fVec7[((IOTA - 103) & 127)]) + ((0.0010818799999999999 * fVec7[((IOTA - 102) & 127)]) + ((0.000419827 * fVec7[((IOTA - 101) & 127)]) + ((0.000472497 * fVec7[((IOTA - 100) & 127)]) + ((0.00040792200000000007 * fVec7[((IOTA - 99) & 127)]) + ((0.0017046100000000001 * fVec7[((IOTA - 91) & 127)]) + ((0.0017854000000000001 * fVec7[((IOTA - 90) & 127)]) + ((0.000146465 * fVec7[((IOTA - 89) & 127)]) + ((0.0011246299999999999 * fVec7[((IOTA - 88) & 127)]) + ((0.000121433 * fVec7[((IOTA - 85) & 127)]) + ((0.0015572399999999999 * fVec7[((IOTA - 84) & 127)]) + ((0.00286542 * fVec7[((IOTA - 81) & 127)]) + ((0.0035030399999999998 * fVec7[((IOTA - 80) & 127)]) + ((0.0013357099999999999 * fVec7[((IOTA - 79) & 127)]) + ((0.0028925200000000004 * fVec7[((IOTA - 71) & 127)]) + ((0.00488274 * fVec7[((IOTA - 70) & 127)]) + ((0.0016347499999999999 * fVec7[((IOTA - 69) & 127)]) + ((0.0014653400000000001 * fVec7[((IOTA - 68) & 127)]) + ((0.0016001099999999999 * fVec7[((IOTA - 67) & 127)]) + ((0.0032972399999999999 * fVec7[((IOTA - 39) & 127)]) + ((0.010154000000000002 * fVec7[((IOTA - 38) & 127)]) + ((0.019400900000000002 * fVec7[((IOTA - 37) & 127)]) + ((0.0108342 * fVec7[((IOTA - 36) & 127)]) + ((0.021263200000000003 * fVec7[((IOTA - 34) & 127)]) + ((0.020498300000000001 * fVec7[((IOTA - 33) & 127)]) + ((0.0187685 * fVec7[((IOTA - 32) & 127)]) + ((0.0086318699999999998 * fVec7[((IOTA - 31) & 127)]) + ((0.0119454 * fVec7[((IOTA - 30) & 127)]) + ((0.024111899999999999 * fVec7[((IOTA - 29) & 127)]) + ((0.0154331 * fVec7[((IOTA - 28) & 127)]) + ((0.011326600000000001 * fVec7[((IOTA - 27) & 127)]) + ((0.0041025300000000001 * fVec7[((IOTA - 26) & 127)]) + ((0.035046500000000001 * fVec7[((IOTA - 25) & 127)]) + ((0.0109835 * fVec7[((IOTA - 24) & 127)]) + ((0.008624660000000001 * fVec7[((IOTA - 21) & 127)]) + ((0.0030574400000000002 * fVec7[((IOTA - 17) & 127)]) + ((0.016297900000000001 * fVec7[((IOTA - 14) & 127)]) + ((0.053418799999999995 * fVec7[((IOTA - 13) & 127)]) + ((0.071327800000000011 * fVec7[((IOTA - 12) & 127)]) + ((0.0074569299999999996 * fVec7[((IOTA - 11) & 127)]) + ((0.00196452 * fVec7[((IOTA - 9) & 127)]) + ((1.4284499999999999e-06 * fVec7[((IOTA - 7) & 127)]) + ((0.00065664800000000002 * fVec7[((IOTA - 5) & 127)]) + ((0.00033454700000000003 * fVec7[((IOTA - 3) & 127)]) + ((4.19701e-05 * fVec7[((IOTA - 1) & 127)]) + ((1.6075699999999999e-05 * fTemp7) + ((0.00045853400000000008 * fVec7[((IOTA - 58) & 127)]) + ((0.0015582 * fVec7[((IOTA - 57) & 127)]) + ((0.0015001299999999999 * fVec7[((IOTA - 56) & 127)]) + ((0.0012102200000000001 * fVec7[((IOTA - 55) & 127)]) + ((2.4437900000000002e-05 * fVec7[((IOTA - 63) & 127)]) + ((0.00012724599999999999 * fVec7[((IOTA - 62) & 127)]) + ((2.1103299999999999e-06 * fVec8[((IOTA - 125) & 127)]) + ((5.5217400000000003e-06 * fVec8[((IOTA - 124) & 127)]) + ((3.11789e-06 * fVec8[((IOTA - 123) & 127)]) + ((9.0368300000000007e-05 * fVec8[((IOTA - 119) & 127)]) + ((0.00011532 * fVec8[((IOTA - 118) & 127)]) + ((1.9649999999999998e-06 * fVec8[((IOTA - 116) & 127)]) + ((0.00021442500000000002 * fVec8[((IOTA - 115) & 127)]) + ((0.00030905200000000004 * fVec8[((IOTA - 114) & 127)]) + ((4.7581900000000006e-05 * fVec8[((IOTA - 113) & 127)]) + ((0.00016311100000000002 * fVec8[((IOTA - 112) & 127)]) + ((0.00016924900000000001 * fVec8[((IOTA - 111) & 127)]) + ((0.00014172800000000001 * (fVec12[((IOTA - 109) & 127)] - fVec9[((IOTA - 78) & 127)])) + ((0.0010940099999999999 * fVec8[((IOTA - 85) & 127)]) + ((0.0017591500000000001 * fVec8[((IOTA - 84) & 127)]) + ((0.00040582000000000004 * fVec8[((IOTA - 83) & 127)]) + ((3.0118399999999995e-06 * fVec8[((IOTA - 109) & 127)]) + ((0.00053920599999999997 * fVec8[((IOTA - 108) & 127)]) + ((0.00034291200000000001 * fVec8[((IOTA - 107) & 127)]) + ((0.00026214100000000002 * fVec8[((IOTA - 106) & 127)]) + ((0.00023320700000000001 * fVec8[((IOTA - 105) & 127)]) + ((0.00041387999999999999 * fVec8[((IOTA - 102) & 127)]) + ((0.000138245 * fVec8[((IOTA - 101) & 127)]) + ((0.000478309 * fVec8[((IOTA - 100) & 127)]) + ((0.00091847499999999998 * fVec8[((IOTA - 99) & 127)]) + ((0.0013005599999999999 * fVec8[((IOTA - 98) & 127)]) + ((0.00113754 * fVec8[((IOTA - 97) & 127)]) + ((0.0026714199999999999 * fVec8[((IOTA - 93) & 127)]) + ((0.0021342599999999998 * fVec8[((IOTA - 92) & 127)]) + ((0.00024966300000000001 * fVec8[((IOTA - 90) & 127)]) + ((0.0023507699999999999 * fVec8[((IOTA - 89) & 127)]) + ((0.0046170200000000003 * fVec8[((IOTA - 88) & 127)]) + ((0.0013482500000000001 * fVec8[((IOTA - 87) & 127)]) + ((0.00225807 * fVec8[((IOTA - 82) & 127)]) + ((0.00237535 * fVec8[((IOTA - 81) & 127)]) + ((0.000120611 * fVec8[((IOTA - 80) & 127)]) + ((0.00060378299999999999 * fVec8[((IOTA - 78) & 127)]) + ((0.00026133000000000005 * fVec8[((IOTA - 73) & 127)]) + ((0.0020077300000000001 * fVec8[((IOTA - 63) & 127)]) + ((0.0032089100000000001 * fVec8[((IOTA - 62) & 127)]) + ((0.0010079800000000001 * fVec8[((IOTA - 61) & 127)]) + ((0.00063087099999999997 * fVec8[((IOTA - 60) & 127)]) + ((0.0044288800000000005 * fVec8[((IOTA - 59) & 127)]) + ((0.0047678900000000003 * fVec8[((IOTA - 58) & 127)]) + ((0.0024688500000000003 * fVec8[((IOTA - 57) & 127)]) + ((0.00277838 * fVec8[((IOTA - 56) & 127)]) + ((0.0075612199999999996 * fVec8[((IOTA - 55) & 127)]) + ((0.00761051 * fVec8[((IOTA - 54) & 127)]) + ((0.0018776900000000002 * fVec8[((IOTA - 52) & 127)]) + ((0.0082384199999999998 * fVec8[((IOTA - 51) & 127)]) + ((0.0050166199999999994 * fVec8[((IOTA - 50) & 127)]) + ((0.0032369600000000001 * fVec8[((IOTA - 49) & 127)]) + ((0.0040625900000000005 * fVec8[((IOTA - 48) & 127)]) + ((0.011856999999999999 * fVec8[((IOTA - 47) & 127)]) + ((0.0091953899999999995 * fVec8[((IOTA - 46) & 127)]) + ((0.0061361499999999999 * fVec8[((IOTA - 45) & 127)]) + ((0.015667299999999999 * fVec8[((IOTA - 44) & 127)]) + ((0.0121152 * fVec8[((IOTA - 43) & 127)]) + ((0.0102805 * fVec8[((IOTA - 41) & 127)]) + ((0.0249734 * fVec8[((IOTA - 40) & 127)]) + ((0.0028042699999999998 * fVec8[((IOTA - 39) & 127)]) + ((0.0112762 * fVec8[((IOTA - 37) & 127)]) + ((0.0070063399999999998 * fVec8[((IOTA - 36) & 127)]) + ((0.0037140800000000002 * fVec8[((IOTA - 35) & 127)]) + ((0.0077227400000000005 * fVec8[((IOTA - 32) & 127)]) + ((0.0031788599999999999 * fVec8[((IOTA - 31) & 127)]) + ((0.0055445599999999996 * fVec8[((IOTA - 25) & 127)]) + ((0.014456700000000001 * fVec8[((IOTA - 21) & 127)]) + ((0.043969699999999994 * fVec8[((IOTA - 17) & 127)]) + ((0.010103500000000001 * fVec8[((IOTA - 16) & 127)]) + ((0.026084999999999997 * fVec8[((IOTA - 15) & 127)]) + ((0.040577500000000002 * fVec8[((IOTA - 14) & 127)]) + ((0.0703236 * fVec8[((IOTA - 13) & 127)]) + ((0.086424600000000004 * fVec8[((IOTA - 12) & 127)]) + ((0.00526349 * fVec8[((IOTA - 11) & 127)]) + ((0.0021171000000000002 * fVec8[((IOTA - 9) & 127)]) + ((0.00027211800000000001 * fVec8[((IOTA - 7) & 127)]) + ((0.00074305500000000006 * fVec8[((IOTA - 5) & 127)]) + ((0.00070165199999999998 * fVec8[((IOTA - 3) & 127)]) + ((7.2115300000000004e-05 * fVec8[((IOTA - 1) & 127)]) + (((6.8184199999999995e-06 * fVec9[((IOTA - 124) & 127)]) + ((9.6015999999999997e-06 * fVec9[((IOTA - 123) & 127)]) + ((2.6161300000000003e-05 * fVec9[((IOTA - 122) & 127)]) + ((7.66841e-06 * fVec9[((IOTA - 120) & 127)]) + ((9.9956200000000017e-05 * fVec9[((IOTA - 119) & 127)]) + ((9.8288800000000007e-05 * fVec9[((IOTA - 118) & 127)]) + ((0.00016569499999999999 * fVec9[((IOTA - 115) & 127)]) + ((0.000131304 * fVec9[((IOTA - 114) & 127)]) + ((7.4421400000000012e-05 * fVec9[((IOTA - 113) & 127)]) + ((9.0750500000000005e-05 * fVec9[((IOTA - 109) & 127)]) + ((0.00081881900000000014 * fVec9[((IOTA - 96) & 127)]) + ((0.0015964400000000002 * fVec9[((IOTA - 95) & 127)]) + ((3.5872199999999999e-05 * fVec9[((IOTA - 92) & 127)]) + ((0.0020068299999999998 * fVec9[((IOTA - 91) & 127)]) + ((0.00050415000000000008 * fVec9[((IOTA - 90) & 127)]) + ((0.00041316100000000005 * fVec9[((IOTA - 89) & 127)]) + ((0.0011488700000000002 * fVec9[((IOTA - 88) & 127)]) + ((0.0011271300000000001 * fVec9[((IOTA - 87) & 127)]) + ((0.0018691199999999999 * fVec9[((IOTA - 81) & 127)]) + ((0.0023870700000000003 * fVec9[((IOTA - 80) & 127)]) + ((0.0014690600000000001 * fVec9[((IOTA - 79) & 127)]) + ((0.00020293100000000003 * fVec9[((IOTA - 75) & 127)]) + ((0.00042256999999999999 * fVec9[((IOTA - 74) & 127)]) + ((4.8196899999999999e-05 * fVec9[((IOTA - 73) & 127)]) + ((0.00081876599999999996 * fVec9[((IOTA - 69) & 127)]) + ((0.00116132 * fVec9[((IOTA - 65) & 127)]) + ((0.0012919000000000001 * fVec9[((IOTA - 60) & 127)]) + ((0.0018522 * fVec9[((IOTA - 53) & 127)]) + ((0.0040265300000000004 * fVec9[((IOTA - 52) & 127)]) + ((0.00164123 * fVec9[((IOTA - 49) & 127)]) + ((0.0035543000000000003 * fVec9[((IOTA - 48) & 127)]) + ((0.0019447000000000002 * fVec9[((IOTA - 47) & 127)]) + ((0.0045421799999999998 * fVec9[((IOTA - 46) & 127)]) + ((0.0032057800000000001 * fVec9[((IOTA - 45) & 127)]) + ((0.0098560599999999998 * fVec9[((IOTA - 43) & 127)]) + ((0.0055212400000000002 * fVec9[((IOTA - 42) & 127)]) + ((0.0039102499999999997 * fVec9[((IOTA - 40) & 127)]) + ((0.0014029000000000001 * fVec9[((IOTA - 39) & 127)]) + ((0.00081971100000000009 * fVec9[((IOTA - 37) & 127)]) + ((0.015326500000000002 * fVec9[((IOTA - 34) & 127)]) + ((0.0039000900000000002 * fVec9[((IOTA - 33) & 127)]) + ((0.0107983 * fVec9[((IOTA - 30) & 127)]) + ((0.0016720200000000002 * fVec9[((IOTA - 28) & 127)]) + ((0.0115642 * fVec9[((IOTA - 27) & 127)]) + ((0.0083424600000000012 * fVec9[((IOTA - 24) & 127)]) + ((0.0016058699999999999 * fVec9[((IOTA - 23) & 127)]) + ((0.00574575 * fVec9[((IOTA - 22) & 127)]) + ((0.0048227900000000004 * fVec9[((IOTA - 21) & 127)]) + ((0.0053663600000000006 * fVec9[((IOTA - 20) & 127)]) + ((0.0110708 * fVec9[((IOTA - 17) & 127)]) + ((0.0074311999999999998 * fVec9[((IOTA - 16) & 127)]) + ((0.00037238400000000002 * fVec9[((IOTA - 14) & 127)]) + ((0.00252045 * fVec9[((IOTA - 13) & 127)]) + ((0.00052853700000000004 * fVec9[((IOTA - 8) & 127)]) + ((0.00017706000000000002 * fVec9[((IOTA - 5) & 127)]) + ((0.00017067000000000002 * fVec9[((IOTA - 4) & 127)]) + ((2.8269799999999997e-06 * fVec9[((IOTA - 1) & 127)]) + ((0.0028610799999999998 * fVec9[((IOTA - 84) & 127)]) + ((0.00119961 * fVec9[((IOTA - 83) & 127)]) + ((3.5305499999999996e-06 * fVec10[((IOTA - 125) & 127)]) + ((4.1155299999999994e-06 * fVec10[((IOTA - 124) & 127)]) + ((8.4151000000000001e-06 * fVec10[((IOTA - 123) & 127)]) + ((2.8368800000000002e-05 * fVec10[((IOTA - 122) & 127)]) + ((6.1171800000000003e-05 * fVec10[((IOTA - 121) & 127)]) + ((4.5063100000000006e-05 * fVec10[((IOTA - 120) & 127)]) + ((0.00013187999999999999 * fVec10[((IOTA - 117) & 127)]) + ((9.0214000000000001e-05 * fVec10[((IOTA - 116) & 127)]) + ((4.5684500000000006e-05 * fVec10[((IOTA - 115) & 127)]) + ((0.0011123800000000001 * fVec9[((IOTA - 82) & 127)]) + ((0.00010922300000000001 * fVec10[((IOTA - 114) & 127)]) + ((0.00012787900000000001 * fVec10[((IOTA - 113) & 127)]) + ((0.00010810400000000001 * fVec10[((IOTA - 112) & 127)]) + ((8.4602100000000008e-05 * fVec10[((IOTA - 111) & 127)]) + ((0.00014129499999999999 * fVec10[((IOTA - 103) & 127)]) + ((0.0011713800000000001 * fVec10[((IOTA - 96) & 127)]) + ((0.00048376700000000002 * fVec10[((IOTA - 95) & 127)]) + ((0.00040982500000000005 * fVec10[((IOTA - 89) & 127)]) + ((0.00192194 * fVec10[((IOTA - 86) & 127)]) + ((0.00102436 * fVec10[((IOTA - 85) & 127)]) + ((0.00100778 * fVec10[((IOTA - 81) & 127)]) + ((0.00035581800000000004 * fVec10[((IOTA - 80) & 127)]) + ((0.00108387 * fVec10[((IOTA - 78) & 127)]) + ((0.00355741 * fVec10[((IOTA - 77) & 127)]) + ((0.00203951 * fVec10[((IOTA - 76) & 127)]) + ((0.00099792900000000005 * fVec10[((IOTA - 74) & 127)]) + ((0.0044921400000000004 * fVec10[((IOTA - 73) & 127)]) + ((0.00086068200000000009 * fVec10[((IOTA - 69) & 127)]) + ((0.0026953699999999999 * fVec10[((IOTA - 68) & 127)]) + ((0.00069267700000000005 * fVec10[((IOTA - 67) & 127)]) + ((0.0014122200000000001 * fVec10[((IOTA - 62) & 127)]) + ((0.00059297400000000002 * fVec10[((IOTA - 61) & 127)]) + ((0.00053443700000000002 * fVec10[((IOTA - 59) & 127)]) + ((0.0013940900000000002 * fVec10[((IOTA - 58) & 127)]) + ((0.0012833600000000001 * fVec10[((IOTA - 57) & 127)]) + ((0.0031110899999999999 * fVec10[((IOTA - 56) & 127)]) + ((0.0050713199999999998 * fVec10[((IOTA - 55) & 127)]) + ((0.0050443700000000003 * fVec10[((IOTA - 54) & 127)]) + ((0.00419002 * fVec10[((IOTA - 53) & 127)]) + ((0.0032877399999999999 * fVec10[((IOTA - 51) & 127)]) + ((0.0071723899999999998 * fVec10[((IOTA - 43) & 127)]) + ((0.0036162899999999999 * fVec10[((IOTA - 42) & 127)]) + ((0.010498400000000001 * fVec10[((IOTA - 41) & 127)]) + ((0.0021260799999999998 * fVec10[((IOTA - 40) & 127)]) + ((0.0065524800000000003 * fVec10[((IOTA - 38) & 127)]) + ((0.011496599999999999 * fVec10[((IOTA - 35) & 127)]) + ((0.0010427100000000001 * fVec10[((IOTA - 34) & 127)]) + ((0.0092546800000000012 * fVec10[((IOTA - 31) & 127)]) + ((0.0026388100000000001 * fVec10[((IOTA - 27) & 127)]) + ((0.0128167 * fVec10[((IOTA - 23) & 127)]) + ((0.011590800000000002 * fVec10[((IOTA - 17) & 127)]) + ((0.0038800599999999998 * fVec10[((IOTA - 16) & 127)]) + ((0.022263399999999999 * fVec10[((IOTA - 15) & 127)]) + ((0.020153300000000002 * fVec10[((IOTA - 14) & 127)]) + ((0.042437800000000005 * fVec10[((IOTA - 13) & 127)]) + ((0.047133599999999998 * fVec10[((IOTA - 12) & 127)]) + ((0.00567696 * fVec10[((IOTA - 11) & 127)]) + ((0.00143681 * fVec10[((IOTA - 9) & 127)]) + ((0.00086101400000000001 * fVec10[((IOTA - 7) & 127)]) + ((0.00058492199999999998 * fVec10[((IOTA - 5) & 127)]) + ((6.7829099999999997e-06 * fVec10[((IOTA - 4) & 127)]) + ((0.00025529600000000002 * fVec10[((IOTA - 3) & 127)]) + ((2.9285600000000003e-05 * fVec10[((IOTA - 1) & 127)]) + ((3.8661599999999993e-06 * fTemp10) + ((0.00909628 * fVec10[((IOTA - 50) & 127)]) + ((0.0036924600000000003 * fVec10[((IOTA - 49) & 127)]) + ((0.0046017100000000002 * fVec10[((IOTA - 48) & 127)]) + ((0.0020962000000000003 * fVec10[((IOTA - 45) & 127)]) + ((0.0073180300000000005 * fVec10[((IOTA - 47) & 127)]) + ((0.0070144200000000004 * fVec10[((IOTA - 46) & 127)]) + ((1.70851e-05 * fVec11[((IOTA - 122) & 127)]) + ((3.0657300000000002e-05 * fVec11[((IOTA - 121) & 127)]) + ((5.3340699999999993e-07 * fVec11[((IOTA - 120) & 127)]) + ((0.0052343400000000005 * fVec10[((IOTA - 44) & 127)]) + ((1.03751e-05 * fVec11[((IOTA - 118) & 127)]) + ((8.3314900000000018e-05 * fVec11[((IOTA - 117) & 127)]) + ((9.1851000000000012e-05 * fVec11[((IOTA - 112) & 127)]) + ((2.7205700000000003e-05 * fVec11[((IOTA - 111) & 127)]) + ((0.00152143 * fVec11[((IOTA - 82) & 127)]) + ((0.0032921200000000004 * fVec11[((IOTA - 81) & 127)]) + ((0.00193753 * fVec11[((IOTA - 80) & 127)]) + ((0.00062551900000000005 * fVec11[((IOTA - 79) & 127)]) + ((0.00096127300000000012 * fVec11[((IOTA - 70) & 127)]) + ((0.0042903999999999998 * fVec11[((IOTA - 69) & 127)]) + ((0.0037726500000000002 * fVec11[((IOTA - 68) & 127)]) + ((0.00041918000000000001 * fVec11[((IOTA - 66) & 127)]) + ((0.00047510500000000003 * fVec11[((IOTA - 63) & 127)]) + ((0.0017495700000000002 * fVec11[((IOTA - 62) & 127)]) + ((0.0013881099999999999 * fVec11[((IOTA - 61) & 127)]) + ((0.00142376 * fVec11[((IOTA - 59) & 127)]) + ((0.0013234399999999999 * fVec11[((IOTA - 58) & 127)]) + ((0.0023363800000000003 * fVec11[((IOTA - 57) & 127)]) + ((0.0047624399999999997 * fVec11[((IOTA - 56) & 127)]) + ((0.00035963800000000001 * fVec11[((IOTA - 55) & 127)]) + ((0.0014325799999999999 * fVec11[((IOTA - 53) & 127)]) + ((0.0035146700000000001 * fVec11[((IOTA - 50) & 127)]) + ((0.00013482199999999999 * fVec11[((IOTA - 46) & 127)]) + ((0.0031359700000000001 * fVec11[((IOTA - 44) & 127)]) + ((0.0033774600000000001 * fVec11[((IOTA - 43) & 127)]) + ((0.00043131800000000003 * fVec11[((IOTA - 41) & 127)]) + ((0.0017748 * fVec11[((IOTA - 37) & 127)]) + ((0.0095376600000000016 * fVec11[((IOTA - 34) & 127)]) + ((0.00354222 * fVec11[((IOTA - 31) & 127)]) + ((0.0099954400000000013 * fVec11[((IOTA - 30) & 127)]) + ((0.00581921 * fVec11[((IOTA - 27) & 127)]) + ((0.00064315100000000001 * fVec11[((IOTA - 26) & 127)]) + ((0.016502200000000002 * fVec11[((IOTA - 25) & 127)]) + ((0.026153100000000002 * fVec11[((IOTA - 19) & 127)]) + ((0.015518799999999999 * fVec11[((IOTA - 18) & 127)]) + ((0.011748099999999999 * fVec11[((IOTA - 16) & 127)]) + ((0.024044800000000002 * fVec11[((IOTA - 15) & 127)]) + ((0.011073200000000002 * fVec11[((IOTA - 12) & 127)]) + ((0.00016208700000000002 * fVec11[((IOTA - 11) & 127)]) + ((0.00023927800000000002 * fVec11[((IOTA - 9) & 127)]) + ((0.00099840900000000002 * fVec11[((IOTA - 7) & 127)]) + ((0.00026075800000000004 * fVec11[((IOTA - 3) & 127)]) + ((9.0296599999999999e-06 * fVec12[((IOTA - 122) & 127)]) + ((1.9893500000000002e-05 * fVec12[((IOTA - 118) & 127)]) + ((3.7877500000000005e-05 * fVec12[((IOTA - 115) & 127)]) + ((0.00027007400000000003 * fVec12[((IOTA - 103) & 127)]) + ((0.00044415800000000005 * fVec12[((IOTA - 102) & 127)]) + ((0.00031515600000000003 * fVec12[((IOTA - 100) & 127)]) + ((0.00068420899999999997 * fVec12[((IOTA - 99) & 127)]) + ((0.00049268100000000002 * fVec12[((IOTA - 96) & 127)]) + ((0.0015305800000000001 * fVec12[((IOTA - 91) & 127)]) + ((0.0017284399999999999 * fVec12[((IOTA - 90) & 127)]) + ((0.00127253 * fVec12[((IOTA - 89) & 127)]) + ((0.00166752 * fVec12[((IOTA - 88) & 127)]) + ((0.00033490500000000003 * fVec12[((IOTA - 85) & 127)]) + ((0.0011273899999999998 * fVec12[((IOTA - 84) & 127)]) + ((6.2119200000000007e-05 * fVec12[((IOTA - 82) & 127)]) + ((0.0017261400000000001 * fVec12[((IOTA - 81) & 127)]) + ((0.00048162000000000004 * fVec12[((IOTA - 80) & 127)]) + ((0.00071563500000000006 * fVec12[((IOTA - 79) & 127)]) + ((1.14088e-05 * fVec12[((IOTA - 77) & 127)]) + ((0.0014551499999999999 * fVec12[((IOTA - 76) & 127)]) + ((0.00078290800000000002 * fVec12[((IOTA - 73) & 127)]) + ((0.0013603899999999999 * fVec12[((IOTA - 72) & 127)]) + ((0.00076972400000000006 * fVec12[((IOTA - 71) & 127)]) + ((0.0011577500000000001 * fVec12[((IOTA - 70) & 127)]) + ((0.0027559100000000003 * fVec12[((IOTA - 69) & 127)]) + ((0.0011045300000000001 * fVec12[((IOTA - 65) & 127)]) + ((0.0019003900000000001 * fVec12[((IOTA - 64) & 127)]) + ((5.8659000000000002e-05 * fVec12[((IOTA - 63) & 127)]) + ((0.00034302399999999999 * fVec12[((IOTA - 60) & 127)]) + ((0.0015042599999999999 * fVec12[((IOTA - 55) & 127)]) + ((0.0038506399999999998 * fVec12[((IOTA - 54) & 127)]) + ((0.00280679 * fVec12[((IOTA - 53) & 127)]) + ((0.00191281 * fVec12[((IOTA - 51) & 127)]) + ((0.0030454200000000001 * fVec12[((IOTA - 47) & 127)]) + ((0.00241033 * fVec12[((IOTA - 41) & 127)]) + ((0.0054447699999999998 * fVec12[((IOTA - 34) & 127)]) + ((0.0027451799999999998 * fVec12[((IOTA - 33) & 127)]) + ((0.00192986 * fVec12[((IOTA - 32) & 127)]) + ((0.0050891900000000004 * fVec12[((IOTA - 29) & 127)]) + ((0.010143899999999999 * fVec12[((IOTA - 28) & 127)]) + ((0.0039649200000000002 * fVec12[((IOTA - 27) & 127)]) + ((0.014927200000000002 * fVec12[((IOTA - 24) & 127)]) + ((0.010792999999999999 * fVec12[((IOTA - 23) & 127)]) + ((0.008510160000000001 * fVec12[((IOTA - 22) & 127)]) + ((0.0223719 * fVec12[((IOTA - 21) & 127)]) + ((0.0123166 * fVec12[((IOTA - 20) & 127)]) + ((0.017947399999999999 * fVec12[((IOTA - 19) & 127)]) + ((0.0073694100000000007 * fVec12[((IOTA - 14) & 127)]) + ((0.0142655 * fVec12[((IOTA - 12) & 127)]) + ((0.0067548000000000009 * fVec12[((IOTA - 11) & 127)]) + ((0.0025108100000000005 * fVec12[((IOTA - 9) & 127)]) + ((0.00235191 * fVec12[((IOTA - 7) & 127)]) + ((0.00091979399999999993 * fVec12[((IOTA - 5) & 127)]) + ((0.00050699800000000004 * fVec12[((IOTA - 3) & 127)]) + ((0.0010344 * fVec12[((IOTA - 46) & 127)]) + ((0.0021753000000000002 * fVec12[((IOTA - 43) & 127)]) + ((6.9716599999999994e-06 * fVec13[((IOTA - 124) & 127)]) + ((9.8485200000000004e-06 * fVec13[((IOTA - 118) & 127)]) + ((0.00010357900000000001 * fVec13[((IOTA - 114) & 127)]) + ((0.00042897299999999997 * fVec13[((IOTA - 113) & 127)]) + ((0.00029024100000000006 * fVec13[((IOTA - 112) & 127)]) + ((9.8297200000000012e-05 * fVec13[((IOTA - 111) & 127)]) + ((0.00020804999999999999 * fVec13[((IOTA - 110) & 127)]) + ((0.00054109500000000001 * fVec13[((IOTA - 109) & 127)]) + ((0.00083962700000000002 * fVec13[((IOTA - 108) & 127)]) + ((0.00056389100000000007 * fVec13[((IOTA - 107) & 127)]) + ((0.00051816800000000006 * fVec13[((IOTA - 106) & 127)]) + ((0.00065456900000000003 * fVec13[((IOTA - 105) & 127)]) + ((0.00088825400000000011 * fVec13[((IOTA - 104) & 127)]) + ((0.000446193 * fVec13[((IOTA - 103) & 127)]) + ((0.00071199100000000003 * fVec13[((IOTA - 102) & 127)]) + ((0.0012468500000000001 * fVec13[((IOTA - 101) & 127)]) + ((0.0012435300000000001 * fVec13[((IOTA - 100) & 127)]) + ((0.0010388900000000002 * fVec13[((IOTA - 99) & 127)]) + ((0.00170268 * fVec13[((IOTA - 98) & 127)]) + ((0.00126004 * fVec13[((IOTA - 97) & 127)]) + ((0.0013548899999999999 * fVec13[((IOTA - 93) & 127)]) + ((0.0020163799999999999 * fVec13[((IOTA - 92) & 127)]) + ((0.0010302600000000001 * fVec13[((IOTA - 89) & 127)]) + ((0.00142103 * fVec13[((IOTA - 88) & 127)]) + ((0.0020040399999999999 * fVec13[((IOTA - 82) & 127)]) + ((0.0030592000000000002 * fVec13[((IOTA - 81) & 127)]) + ((0.00082976100000000015 * fVec13[((IOTA - 80) & 127)]) + ((0.00083032800000000008 * fVec13[((IOTA - 79) & 127)]) + ((0.00146794 * fVec13[((IOTA - 78) & 127)]) + ((0.0029912300000000001 * fVec13[((IOTA - 77) & 127)]) + ((0.0019008400000000002 * fVec13[((IOTA - 76) & 127)]) + ((0.00032308700000000001 * fVec13[((IOTA - 75) & 127)]) + ((0.00121188 * fVec13[((IOTA - 74) & 127)]) + ((0.0015286800000000001 * fVec13[((IOTA - 73) & 127)]) + ((0.0016313200000000001 * fVec13[((IOTA - 72) & 127)]) + ((0.0019843199999999999 * fVec13[((IOTA - 71) & 127)]) + ((0.0017147500000000001 * fVec13[((IOTA - 69) & 127)]) + ((4.5241600000000004e-05 * fVec13[((IOTA - 68) & 127)]) + ((0.00193403 * fVec13[((IOTA - 67) & 127)]) + ((0.00103439 * fVec13[((IOTA - 66) & 127)]) + ((0.00025587599999999999 * fVec13[((IOTA - 61) & 127)]) + ((0.0014905299999999999 * fVec13[((IOTA - 60) & 127)]) + ((0.000502175 * fVec13[((IOTA - 59) & 127)]) + ((0.00055048599999999999 * fVec13[((IOTA - 58) & 127)]) + ((0.00049892100000000004 * fVec13[((IOTA - 50) & 127)]) + ((0.0029968600000000001 * fVec13[((IOTA - 47) & 127)]) + ((0.0024887899999999998 * fVec13[((IOTA - 46) & 127)]) + ((0.0030320900000000003 * fVec13[((IOTA - 41) & 127)]) + ((0.0076587399999999998 * fVec13[((IOTA - 40) & 127)]) + ((0.010064699999999999 * fVec13[((IOTA - 35) & 127)]) + ((0.030136699999999999 * fVec13[((IOTA - 32) & 127)]) + ((0.017341200000000001 * fVec13[((IOTA - 31) & 127)]) + ((0.039170099999999999 * fVec13[((IOTA - 28) & 127)]) + ((0.00031563800000000003 * fVec13[((IOTA - 27) & 127)]) + ((0.00069887100000000011 * fVec13[((IOTA - 25) & 127)]) + ((0.025458500000000002 * fVec13[((IOTA - 24) & 127)]) + ((0.0120184 * fVec13[((IOTA - 21) & 127)]) + ((0.011691100000000001 * fVec13[((IOTA - 19) & 127)]) + ((0.0053095800000000004 * fVec13[((IOTA - 14) & 127)]) + ((0.0072171800000000001 * fVec13[((IOTA - 12) & 127)]) + ((0.0030320200000000003 * fVec13[((IOTA - 11) & 127)]) + ((0.00131485 * fVec13[((IOTA - 9) & 127)]) + ((0.0011081700000000001 * fVec13[((IOTA - 7) & 127)]) + ((0.00036208399999999998 * fVec13[((IOTA - 5) & 127)]) + ((0.00017773600000000001 * fVec13[((IOTA - 3) & 127)]) + ((0.0031219500000000001 * fVec13[((IOTA - 70) & 127)]) + ((6.574880000000001e-05 * fVec14[((IOTA - 115) & 127)]) + ((7.5893800000000015e-05 * fVec14[((IOTA - 114) & 127)]) + ((0.00055035400000000008 * fVec14[((IOTA - 113) & 127)]) + ((0.00054230400000000007 * fVec14[((IOTA - 112) & 127)]) + ((0.00038862600000000004 * fVec14[((IOTA - 111) & 127)]) + ((0.00038355300000000002 * fVec14[((IOTA - 110) & 127)]) + ((0.0010789999999999999 * fVec14[((IOTA - 109) & 127)]) + ((0.00127535 * fVec14[((IOTA - 108) & 127)]) + ((0.00026040700000000004 * fVec14[((IOTA - 107) & 127)]) + ((0.00064190699999999994 * fVec14[((IOTA - 106) & 127)]) + ((0.0012643700000000001 * fVec14[((IOTA - 105) & 127)]) + ((0.0014951399999999998 * fVec14[((IOTA - 104) & 127)]) + ((0.00096466700000000011 * fVec14[((IOTA - 103) & 127)]) + ((0.00062200899999999997 * fVec14[((IOTA - 102) & 127)]) + ((0.0013151 * fVec14[((IOTA - 101) & 127)]) + ((0.00155276 * fVec14[((IOTA - 100) & 127)]) + ((0.00147963 * fVec14[((IOTA - 99) & 127)]) + ((0.0012132899999999999 * fVec14[((IOTA - 96) & 127)]) + ((5.4447799999999994e-06 * fVec14[((IOTA - 91) & 127)]) + ((0.00060830200000000002 * fVec14[((IOTA - 82) & 127)]) + ((0.00073753600000000001 * fVec14[((IOTA - 81) & 127)]) + ((4.1098800000000007e-05 * fVec14[((IOTA - 76) & 127)]) + ((0.00031511100000000003 * fVec14[((IOTA - 74) & 127)]) + ((0.0011385200000000001 * fVec14[((IOTA - 73) & 127)]) + ((0.0023925600000000002 * fVec14[((IOTA - 72) & 127)]) + ((0.00191902 * fVec14[((IOTA - 71) & 127)]) + ((0.00073584900000000001 * fVec14[((IOTA - 70) & 127)]) + ((0.0033698500000000002 * fVec14[((IOTA - 69) & 127)]) + ((0.0046562499999999998 * fVec14[((IOTA - 68) & 127)]) + ((0.0026088700000000001 * fVec14[((IOTA - 67) & 127)]) + ((0.0010497099999999999 * fVec14[((IOTA - 95) & 127)]) + ((0.0015381499999999998 * fVec14[((IOTA - 66) & 127)]) + ((0.00177024 * fVec14[((IOTA - 65) & 127)]) + ((0.0027169400000000002 * fVec14[((IOTA - 64) & 127)]) + ((0.0011334100000000001 * fVec14[((IOTA - 61) & 127)]) + ((0.0012374899999999999 * fVec14[((IOTA - 57) & 127)]) + ((0.00350462 * fVec14[((IOTA - 56) & 127)]) + ((0.00029472900000000003 * fVec14[((IOTA - 55) & 127)]) + ((0.00161133 * fVec14[((IOTA - 49) & 127)]) + ((0.0043256300000000004 * fVec14[((IOTA - 43) & 127)]) + ((0.0039263700000000002 * fVec14[((IOTA - 38) & 127)]) + ((0.0193136 * fVec14[((IOTA - 37) & 127)]) + ((0.0137585 * fVec14[((IOTA - 34) & 127)]) + ((0.016165499999999999 * fVec14[((IOTA - 31) & 127)]) + ((0.0200181 * fVec14[((IOTA - 30) & 127)]) + ((0.00436148 * fVec14[((IOTA - 27) & 127)]) + ((0.0057804000000000006 * fVec14[((IOTA - 22) & 127)]) + ((0.037111700000000004 * fVec14[((IOTA - 19) & 127)]) + ((0.014907300000000002 * fVec14[((IOTA - 18) & 127)]) + ((0.0092683000000000001 * fVec14[((IOTA - 16) & 127)]) + ((0.0158698 * fVec14[((IOTA - 15) & 127)]) + ((0.00489909 * fVec14[((IOTA - 12) & 127)]) + ((0.00036272099999999999 * fVec14[((IOTA - 11) & 127)]) + ((0.00043871700000000003 * fVec14[((IOTA - 10) & 127)]) + ((0.00018132700000000001 * fVec14[((IOTA - 9) & 127)]) + ((0.00097058900000000011 * fVec14[((IOTA - 7) & 127)]) + ((0.00016036100000000001 * fVec14[((IOTA - 6) & 127)]) + ((0.00019245000000000002 * fVec14[((IOTA - 3) & 127)]) + ((0.0115485 * fVec14[((IOTA - 25) & 127)]) + ((1.8656099999999998e-06 * fVec0[((IOTA - 126) & 127)]) + ((2.5639300000000001e-06 * fVec0[((IOTA - 125) & 127)]) + ((8.3185099999999991e-06 * fVec0[((IOTA - 124) & 127)]) + ((3.8801999999999999e-05 * fVec0[((IOTA - 123) & 127)]) + ((6.5230500000000005e-05 * fVec0[((IOTA - 122) & 127)]) + ((4.81323e-05 * fVec0[((IOTA - 121) & 127)]) + ((3.1175000000000006e-05 * fVec0[((IOTA - 120) & 127)]) + ((1.1129000000000001e-05 * fVec0[((IOTA - 119) & 127)]) + ((0.00016385400000000001 * fVec0[((IOTA - 118) & 127)]) + ((0.000137362 * fVec0[((IOTA - 117) & 127)]) + ((9.139900000000001e-05 * fVec0[((IOTA - 116) & 127)]) + ((0.00012562199999999998 * fVec0[((IOTA - 115) & 127)]) + ((0.00025063300000000005 * fVec0[((IOTA - 113) & 127)]) + ((0.000272806 * fVec0[((IOTA - 112) & 127)]) + ((0.000314307 * fVec0[((IOTA - 111) & 127)]) + ((0.00026935700000000002 * fVec0[((IOTA - 110) & 127)]) + ((0.00027303400000000002 * fVec0[((IOTA - 109) & 127)]) + ((8.74662e-06 * fVec0[((IOTA - 108) & 127)]) + ((0.0032127699999999998 * fVec14[((IOTA - 23) & 127)]) + ((0.00035653000000000003 * fVec0[((IOTA - 106) & 127)]) + ((0.000138672 * fVec0[((IOTA - 91) & 127)]) + ((0.0011749499999999999 * fVec0[((IOTA - 81) & 127)]) + ((0.00092974400000000003 * fVec0[((IOTA - 76) & 127)]) + ((0.00189912 * fVec0[((IOTA - 75) & 127)]) + ((0.0018498099999999999 * fVec0[((IOTA - 74) & 127)]) + ((0.00102903 * fVec0[((IOTA - 70) & 127)]) + ((0.0023879000000000001 * fVec0[((IOTA - 69) & 127)]) + ((0.0030104600000000004 * fVec0[((IOTA - 68) & 127)]) + ((0.00157916 * fVec0[((IOTA - 67) & 127)]) + ((0.0021747000000000003 * fVec0[((IOTA - 66) & 127)]) + ((0.0024374800000000001 * fVec0[((IOTA - 65) & 127)]) + ((0.0017056100000000002 * fVec0[((IOTA - 64) & 127)]) + ((0.00246014 * fVec0[((IOTA - 63) & 127)]) + ((0.0044470300000000003 * fVec0[((IOTA - 62) & 127)]) + ((0.0047484400000000005 * fVec0[((IOTA - 61) & 127)]) + ((0.0027951100000000004 * fVec0[((IOTA - 60) & 127)]) + ((0.0047547600000000002 * fVec0[((IOTA - 59) & 127)]) + ((0.0037710400000000002 * fVec0[((IOTA - 58) & 127)]) + ((0.0045419900000000001 * fVec0[((IOTA - 57) & 127)]) + ((0.010441600000000001 * fVec0[((IOTA - 56) & 127)]) + ((0.0100582 * fVec0[((IOTA - 55) & 127)]) + ((0.0076609499999999997 * fVec0[((IOTA - 54) & 127)]) + ((0.0068764300000000002 * fVec0[((IOTA - 53) & 127)]) + ((0.0095819500000000005 * fVec0[((IOTA - 52) & 127)]) + ((0.015913200000000002 * fVec0[((IOTA - 51) & 127)]) + ((0.015119400000000002 * fVec0[((IOTA - 50) & 127)]) + ((0.0128559 * fVec0[((IOTA - 49) & 127)]) + ((0.016606200000000002 * fVec0[((IOTA - 48) & 127)]) + ((0.016194200000000002 * fVec0[((IOTA - 47) & 127)]) + ((0.0116756 * fVec0[((IOTA - 46) & 127)]) + ((0.016079200000000002 * fVec0[((IOTA - 44) & 127)]) + ((0.0188738 * fVec0[((IOTA - 43) & 127)]) + ((0.022184499999999999 * fVec0[((IOTA - 42) & 127)]) + ((0.020373700000000002 * fVec0[((IOTA - 41) & 127)]) + ((0.0086227300000000003 * fVec0[((IOTA - 40) & 127)]) + ((0.015368999999999999 * fVec0[((IOTA - 39) & 127)]) + ((0.025263300000000002 * fVec0[((IOTA - 38) & 127)]) + ((0.026966700000000003 * fVec0[((IOTA - 37) & 127)]) + ((0.023967600000000002 * fVec0[((IOTA - 36) & 127)]) + ((0.0180915 * fVec0[((IOTA - 35) & 127)]) + ((0.026436999999999999 * fVec0[((IOTA - 34) & 127)]) + ((0.035762599999999999 * fVec0[((IOTA - 33) & 127)]) + ((0.019380700000000001 * fVec0[((IOTA - 32) & 127)]) + ((0.0052166499999999998 * fVec0[((IOTA - 31) & 127)]) + ((0.012525700000000001 * fVec0[((IOTA - 30) & 127)]) + ((0.039007399999999998 * fVec0[((IOTA - 29) & 127)]) + ((0.039406500000000004 * fVec0[((IOTA - 28) & 127)]) + ((0.0160175 * fVec0[((IOTA - 27) & 127)]) + ((0.0095142000000000004 * fVec0[((IOTA - 26) & 127)]) + ((0.00060328500000000006 * fVec0[((IOTA - 25) & 127)]) + ((0.042794800000000008 * fVec0[((IOTA - 24) & 127)]) + ((0.034687500000000003 * fVec0[((IOTA - 23) & 127)]) + ((0.0011779100000000001 * fVec0[((IOTA - 10) & 127)]) + ((0.000167329 * fVec0[((IOTA - 8) & 127)]) + ((0.00041459900000000003 * fVec0[((IOTA - 6) & 127)]) + ((0.00031506200000000001 * fVec0[((IOTA - 2) & 127)]) + ((0.00054486499999999995 * fVec15[((IOTA - 97) & 127)]) + ((0.0024969700000000003 * fVec15[((IOTA - 92) & 127)]) + ((0.0014838199999999998 * fVec15[((IOTA - 91) & 127)]) + ((0.00134669 * fVec15[((IOTA - 89) & 127)]) + ((0.00141242 * fVec15[((IOTA - 88) & 127)]) + ((0.000311911 * fVec15[((IOTA - 84) & 127)]) + ((((((0.00017378000000000001 * fVec15[((IOTA - 2) & 127)]) + (0.015415000000000002 * fVec15[((IOTA - 24) & 127)])) + (0.0114164 * fVec15[((IOTA - 28) & 127)])) + (0.0029673899999999999 * fVec15[((IOTA - 47) & 127)])) + (0.0010694400000000001 * fVec15[((IOTA - 81) & 127)])) + (0.008875280000000001 * fVec0[((IOTA - 45) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (1.68655e-05 * fTemp8))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (2.0610900000000003e-05 * fTemp2))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
			fRec0[0] = max((fRec0[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fHbargraph0 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp16);
			double fTemp17 = (fRec1[0] * (((9.5870599999999998e-07 * fVec2[((IOTA - 126) & 127)]) + ((6.1298299999999997e-06 * fVec2[((IOTA - 125) & 127)]) + ((1.7529100000000001e-05 * fVec2[((IOTA - 124) & 127)]) + ((1.3215300000000002e-05 * fVec2[((IOTA - 123) & 127)]) + ((3.73611e-06 * fVec2[((IOTA - 122) & 127)]) + ((1.4631700000000003e-05 * fVec2[((IOTA - 121) & 127)]) + ((7.9383399999999993e-06 * fVec2[((IOTA - 120) & 127)]) + ((0.000163275 * fVec2[((IOTA - 108) & 127)]) + ((5.2133700000000005e-05 * fVec2[((IOTA - 104) & 127)]) + ((1.7659600000000003e-05 * fVec2[((IOTA - 103) & 127)]) + ((0.00026651699999999997 * fVec2[((IOTA - 101) & 127)]) + ((0.00044441800000000003 * fVec2[((IOTA - 100) & 127)]) + ((0.000204947 * fVec2[((IOTA - 97) & 127)]) + ((0.0021076800000000002 * fVec2[((IOTA - 96) & 127)]) + ((0.0019922500000000001 * fVec2[((IOTA - 95) & 127)]) + ((0.000140679 * fVec2[((IOTA - 94) & 127)]) + ((0.0011280300000000001 * fVec2[((IOTA - 93) & 127)]) + ((0.0019903999999999998 * fVec2[((IOTA - 92) & 127)]) + ((0.000397475 * fVec2[((IOTA - 91) & 127)]) + ((0.00035911300000000004 * fVec2[((IOTA - 85) & 127)]) + ((0.000127816 * fVec2[((IOTA - 81) & 127)]) + ((0.00177585 * fVec2[((IOTA - 79) & 127)]) + ((0.0014990399999999999 * fVec2[((IOTA - 80) & 127)]) + ((5.3717400000000002e-05 * fVec2[((IOTA - 73) & 127)]) + ((0.00100658 * fVec2[((IOTA - 72) & 127)]) + ((0.00017406600000000001 * fVec2[((IOTA - 71) & 127)]) + ((0.00071912600000000005 * fVec2[((IOTA - 67) & 127)]) + ((0.00072212600000000002 * fVec2[((IOTA - 64) & 127)]) + ((0.0049928100000000003 * fVec2[((IOTA - 63) & 127)]) + ((0.0037463600000000002 * fVec2[((IOTA - 62) & 127)]) + ((0.0011032699999999999 * fVec2[((IOTA - 61) & 127)]) + ((1.5565300000000002e-05 * fVec2[((IOTA - 59) & 127)]) + ((0.0010646199999999998 * fVec2[((IOTA - 55) & 127)]) + ((0.0014746 * fVec2[((IOTA - 54) & 127)]) + ((0.0030302600000000003 * fVec2[((IOTA - 53) & 127)]) + ((0.0025201400000000001 * fVec2[((IOTA - 52) & 127)]) + ((0.0023052000000000003 * fVec2[((IOTA - 51) & 127)]) + ((0.0064089099999999994 * fVec2[((IOTA - 50) & 127)]) + ((0.0062176200000000001 * fVec2[((IOTA - 49) & 127)]) + ((0.00136571 * fVec2[((IOTA - 48) & 127)]) + ((0.000484007 * fVec2[((IOTA - 47) & 127)]) + ((0.0053646300000000004 * fVec2[((IOTA - 43) & 127)]) + ((0.00049839000000000003 * fVec2[((IOTA - 40) & 127)]) + ((0.0029793300000000001 * fVec2[((IOTA - 39) & 127)]) + ((0.00011673199999999999 * fVec2[((IOTA - 38) & 127)]) + ((0.0021503299999999998 * fVec2[((IOTA - 34) & 127)]) + ((0.0040663799999999996 * fVec2[((IOTA - 31) & 127)]) + ((0.022960699999999997 * fVec2[((IOTA - 30) & 127)]) + ((0.016656999999999998 * fVec2[((IOTA - 26) & 127)]) + ((0.026276299999999999 * fVec2[((IOTA - 23) & 127)]) + ((0.020495800000000002 * fVec2[((IOTA - 22) & 127)]) + ((0.023659500000000003 * fVec2[((IOTA - 21) & 127)]) + ((0.0210797 * fVec2[((IOTA - 20) & 127)]) + ((0.027862700000000001 * fVec2[((IOTA - 19) & 127)]) + ((0.015660400000000001 * fVec2[((IOTA - 16) & 127)]) + ((0.0069313100000000004 * fVec2[((IOTA - 12) & 127)]) + ((0.0015239000000000001 * fVec2[((IOTA - 11) & 127)]) + ((0.0018694900000000001 * fVec2[((IOTA - 7) & 127)]) + ((0.00091357700000000016 * fVec2[((IOTA - 5) & 127)]) + ((0.00034025 * fVec2[((IOTA - 3) & 127)]) + (((2.1476600000000004e-05 * fVec1[((IOTA - 122) & 127)]) + ((2.6092200000000004e-05 * fVec1[((IOTA - 121) & 127)]) + ((4.1403999999999999e-05 * fVec1[((IOTA - 116) & 127)]) + ((0.00020260300000000001 * fVec1[((IOTA - 113) & 127)]) + ((0.00014776200000000001 * fVec1[((IOTA - 112) & 127)]) + ((0.00045834100000000002 * fVec1[((IOTA - 109) & 127)]) + ((0.00029239199999999999 * fVec1[((IOTA - 108) & 127)]) + ((0.00098381599999999999 * fVec1[((IOTA - 90) & 127)]) + ((0.0011919700000000001 * fVec1[((IOTA - 89) & 127)]) + ((0.00086149400000000008 * fVec1[((IOTA - 77) & 127)]) + ((0.00058334300000000006 * fVec1[((IOTA - 76) & 127)]) + ((0.0013006599999999999 * fVec1[((IOTA - 75) & 127)]) + ((0.00041085900000000007 * fVec1[((IOTA - 71) & 127)]) + ((0.0012948999999999999 * fVec1[((IOTA - 66) & 127)]) + ((0.0018707700000000001 * fVec1[((IOTA - 65) & 127)]) + ((0.00060252200000000008 * fVec1[((IOTA - 64) & 127)]) + ((0.0005644370000000001 * fVec1[((IOTA - 62) & 127)]) + ((0.00017768299999999999 * fVec1[((IOTA - 59) & 127)]) + ((0.0039611100000000003 * fVec1[((IOTA - 58) & 127)]) + ((0.0042174700000000001 * fVec1[((IOTA - 57) & 127)]) + ((0.0044953900000000001 * fVec1[((IOTA - 56) & 127)]) + ((0.0022670200000000002 * fVec1[((IOTA - 55) & 127)]) + ((0.0021868500000000002 * fVec1[((IOTA - 52) & 127)]) + ((9.3079100000000001e-05 * fVec1[((IOTA - 51) & 127)]) + ((0.0021948699999999998 * fVec1[((IOTA - 49) & 127)]) + ((0.0045924300000000006 * fVec1[((IOTA - 46) & 127)]) + ((0.0084135899999999986 * fVec1[((IOTA - 45) & 127)]) + ((0.00067984600000000012 * fVec1[((IOTA - 40) & 127)]) + ((0.00402003 * fVec1[((IOTA - 39) & 127)]) + ((0.017721000000000001 * fVec1[((IOTA - 36) & 127)]) + ((0.0035177100000000003 * fVec1[((IOTA - 33) & 127)]) + ((0.012616499999999999 * fVec1[((IOTA - 32) & 127)]) + ((0.0087009700000000006 * fVec1[((IOTA - 31) & 127)]) + ((0.00866514 * fVec1[((IOTA - 26) & 127)]) + ((0.0044089000000000003 * fVec1[((IOTA - 25) & 127)]) + ((0.023826100000000003 * fVec1[((IOTA - 24) & 127)]) + ((0.0116758 * fVec1[((IOTA - 23) & 127)]) + ((0.0080310599999999996 * fVec1[((IOTA - 19) & 127)]) + ((0.0123386 * fVec1[((IOTA - 18) & 127)]) + ((0.0091981400000000005 * fVec1[((IOTA - 15) & 127)]) + ((0.00114058 * fVec1[((IOTA - 13) & 127)]) + ((0.0133545 * fVec1[((IOTA - 12) & 127)]) + ((0.00042513400000000003 * fVec1[((IOTA - 10) & 127)]) + ((0.00031202200000000002 * fVec1[((IOTA - 9) & 127)]) + ((0.00059449899999999996 * fVec1[((IOTA - 7) & 127)]) + ((9.0752000000000015e-05 * fVec1[((IOTA - 3) & 127)]) + ((2.8761299999999999e-06 * fVec1[((IOTA - 1) & 127)]) + ((3.51439e-07 * fTemp1) + ((0.00051877400000000006 * fVec1[((IOTA - 105) & 127)]) + ((0.00062261900000000004 * fVec1[((IOTA - 104) & 127)]) + ((0.000143232 * fVec1[((IOTA - 103) & 127)]) + ((9.2107400000000005e-05 * fVec1[((IOTA - 101) & 127)]) + ((0.000784532 * fVec1[((IOTA - 100) & 127)]) + ((0.00071355700000000008 * fVec1[((IOTA - 99) & 127)]) + ((0.00065499700000000004 * fVec1[((IOTA - 98) & 127)]) + ((0.00027689300000000001 * fVec1[((IOTA - 97) & 127)]) + ((5.6064900000000006e-05 * fVec3[((IOTA - 119) & 127)]) + ((2.9367600000000003e-05 * fVec3[((IOTA - 118) & 127)]) + ((3.3220999999999999e-05 * fVec3[((IOTA - 117) & 127)]) + ((0.00012805500000000002 * fVec3[((IOTA - 114) & 127)]) + ((0.000103477 * fVec3[((IOTA - 113) & 127)]) + ((0.00025872700000000005 * fVec3[((IOTA - 112) & 127)]) + ((0.00029786899999999998 * fVec3[((IOTA - 111) & 127)]) + ((0.00033988700000000003 * fVec3[((IOTA - 110) & 127)]) + ((9.3482300000000004e-05 * fVec3[((IOTA - 109) & 127)]) + ((0.00013677700000000001 * fVec3[((IOTA - 108) & 127)]) + ((0.00057155300000000003 * fVec3[((IOTA - 107) & 127)]) + ((0.00044084399999999997 * fVec3[((IOTA - 106) & 127)]) + ((0.00058204100000000005 * fVec3[((IOTA - 105) & 127)]) + ((0.00042623200000000002 * fVec3[((IOTA - 104) & 127)]) + ((0.00025379299999999999 * fVec3[((IOTA - 103) & 127)]) + ((0.0004933950000000001 * fVec3[((IOTA - 102) & 127)]) + ((0.00052014400000000005 * fVec3[((IOTA - 99) & 127)]) + ((0.0012122999999999999 * fVec3[((IOTA - 98) & 127)]) + ((0.00045970700000000006 * fVec3[((IOTA - 97) & 127)]) + ((0.00024474399999999997 * fVec3[((IOTA - 96) & 127)]) + ((0.00061810000000000001 * fVec3[((IOTA - 95) & 127)]) + ((4.9281200000000005e-05 * fVec3[((IOTA - 94) & 127)]) + ((0.00026790500000000003 * fVec3[((IOTA - 93) & 127)]) + ((0.00057291299999999998 * fVec3[((IOTA - 88) & 127)]) + ((0.00027621200000000002 * fVec3[((IOTA - 87) & 127)]) + ((0.0015819200000000001 * fVec3[((IOTA - 80) & 127)]) + ((0.00043852399999999998 * fVec3[((IOTA - 7) & 127)]) + ((3.5476400000000005e-05 * fVec3[((IOTA - 6) & 127)]) + ((0.00012342900000000001 * fVec3[((IOTA - 5) & 127)]) + ((0.00013495299999999999 * fVec3[((IOTA - 3) & 127)]) + ((6.5829500000000002e-06 * fVec3[((IOTA - 1) & 127)]) + ((0.00068458300000000011 * fVec3[((IOTA - 16) & 127)]) + ((0.0022406600000000002 * fVec3[((IOTA - 14) & 127)]) + ((0.00121477 * fVec3[((IOTA - 12) & 127)]) + ((0.00034622800000000002 * fVec3[((IOTA - 79) & 127)]) + ((0.0021050499999999998 * fVec3[((IOTA - 75) & 127)]) + ((0.00017197100000000001 * fVec3[((IOTA - 73) & 127)]) + ((0.00328831 * fVec3[((IOTA - 72) & 127)]) + ((0.0033849700000000002 * fVec3[((IOTA - 71) & 127)]) + ((0.00106408 * fVec3[((IOTA - 68) & 127)]) + ((0.0026545100000000001 * fVec3[((IOTA - 67) & 127)]) + ((0.0023114799999999999 * fVec3[((IOTA - 66) & 127)]) + ((0.0010799700000000002 * fVec3[((IOTA - 65) & 127)]) + ((0.000133325 * fVec3[((IOTA - 64) & 127)]) + ((0.00104922 * fVec3[((IOTA - 63) & 127)]) + ((0.00064313500000000008 * fVec3[((IOTA - 62) & 127)]) + ((0.000829111 * fVec3[((IOTA - 50) & 127)]) + ((0.0047209000000000001 * fVec3[((IOTA - 49) & 127)]) + ((0.0040045700000000007 * fVec3[((IOTA - 48) & 127)]) + ((0.0028235399999999998 * fVec3[((IOTA - 47) & 127)]) + ((0.0024041399999999999 * fVec3[((IOTA - 46) & 127)]) + ((0.00123226 * fVec3[((IOTA - 43) & 127)]) + ((0.0055503799999999997 * fVec3[((IOTA - 42) & 127)]) + ((0.0055164099999999994 * fVec3[((IOTA - 41) & 127)]) + ((0.009551539999999999 * fVec3[((IOTA - 40) & 127)]) + ((0.0071602499999999999 * fVec3[((IOTA - 37) & 127)]) + ((0.0059806600000000005 * fVec3[((IOTA - 34) & 127)]) + ((0.0033412400000000001 * fVec3[((IOTA - 33) & 127)]) + ((0.0013020600000000001 * fVec3[((IOTA - 32) & 127)]) + ((0.010603599999999999 * fVec3[((IOTA - 29) & 127)]) + ((0.0047975099999999996 * fVec3[((IOTA - 28) & 127)]) + ((0.00037618999999999999 * fVec3[((IOTA - 25) & 127)]) + ((0.025127100000000003 * fVec3[((IOTA - 24) & 127)]) + ((0.0074241100000000003 * fVec3[((IOTA - 22) & 127)]) + ((0.032307200000000001 * fVec3[((IOTA - 19) & 127)]) + ((0.00015070900000000001 * fVec3[((IOTA - 11) & 127)]) + ((2.7837800000000002e-06 * fVec4[((IOTA - 122) & 127)]) + ((8.3043399999999988e-06 * fVec4[((IOTA - 121) & 127)]) + ((5.6696200000000008e-05 * fVec4[((IOTA - 115) & 127)]) + (((0.00013219300000000001 * fVec4[((IOTA - 112) & 127)]) + ((0.00031032800000000001 * fVec4[((IOTA - 111) & 127)]) + ((0.000209402 * fVec4[((IOTA - 110) & 127)]) + ((0.000392538 * fVec4[((IOTA - 109) & 127)]) + ((0.00021252900000000001 * fVec4[((IOTA - 108) & 127)]) + ((0.00022341099999999999 * fVec4[((IOTA - 107) & 127)]) + ((0.000455971 * fVec4[((IOTA - 106) & 127)]) + ((0.00023670100000000002 * fVec4[((IOTA - 105) & 127)]) + ((9.3216599999999998e-05 * fVec4[((IOTA - 104) & 127)]) + ((0.00011625700000000002 * fVec4[((IOTA - 103) & 127)]) + ((0.00053898200000000002 * fVec4[((IOTA - 102) & 127)]) + ((0.00063969199999999997 * fVec4[((IOTA - 101) & 127)]) + ((0.00049445900000000004 * fVec4[((IOTA - 100) & 127)]) + ((0.00113736 * fVec4[((IOTA - 99) & 127)]) + ((0.0011671499999999998 * fVec4[((IOTA - 98) & 127)]) + ((0.00080747300000000007 * fVec4[((IOTA - 97) & 127)]) + ((0.00098782800000000006 * fVec4[((IOTA - 96) & 127)]) + ((0.000116086 * fVec4[((IOTA - 95) & 127)]) + ((0.0010682000000000001 * fVec4[((IOTA - 92) & 127)]) + ((0.0010581200000000001 * fVec4[((IOTA - 91) & 127)]) + ((0.00023935499999999999 * fVec4[((IOTA - 90) & 127)]) + ((6.07349e-05 * fVec4[((IOTA - 77) & 127)]) + ((0.00024441799999999999 * fVec4[((IOTA - 71) & 127)]) + ((0.0022682500000000003 * fVec4[((IOTA - 70) & 127)]) + ((0.0013828600000000001 * fVec4[((IOTA - 69) & 127)]) + ((0.00019294500000000003 * fVec4[((IOTA - 68) & 127)]) + ((0.0022136499999999997 * fVec4[((IOTA - 67) & 127)]) + ((0.007966260000000001 * fVec4[((IOTA - 35) & 127)]) + ((0.014713700000000001 * fVec4[((IOTA - 34) & 127)]) + ((0.0070875299999999999 * fVec4[((IOTA - 29) & 127)]) + ((0.014908999999999999 * fVec4[((IOTA - 28) & 127)]) + ((0.012678 * fVec4[((IOTA - 27) & 127)]) + ((0.018085500000000001 * fVec4[((IOTA - 23) & 127)]) + ((0.041963100000000003 * fVec4[((IOTA - 22) & 127)]) + ((0.023017300000000001 * fVec4[((IOTA - 16) & 127)]) + ((0.0059658100000000002 * fVec4[((IOTA - 15) & 127)]) + ((0.0035753 * fVec4[((IOTA - 13) & 127)]) + ((0.010657300000000001 * fVec4[((IOTA - 12) & 127)]) + ((0.00023747699999999999 * fVec4[((IOTA - 10) & 127)]) + ((0.00079964000000000012 * fVec4[((IOTA - 7) & 127)]) + ((0.000231547 * fVec4[((IOTA - 4) & 127)]) + ((0.00023603400000000001 * fVec4[((IOTA - 3) & 127)]) + ((3.2773799999999996e-06 * fTemp4) + ((0.0030411100000000001 * fVec4[((IOTA - 42) & 127)]) + ((0.0032564600000000001 * fVec4[((IOTA - 38) & 127)]) + ((0.0023189199999999999 * fVec4[((IOTA - 66) & 127)]) + ((0.0022684799999999998 * fVec4[((IOTA - 65) & 127)]) + ((0.00027487300000000002 * fVec4[((IOTA - 64) & 127)]) + ((0.00097751499999999998 * fVec4[((IOTA - 62) & 127)]) + ((0.00025304199999999998 * fVec4[((IOTA - 61) & 127)]) + ((0.00036951399999999999 * fVec4[((IOTA - 60) & 127)]) + ((0.0020696600000000001 * fVec4[((IOTA - 59) & 127)]) + ((0.00039934300000000003 * fVec4[((IOTA - 58) & 127)]) + ((0.00168114 * fVec4[((IOTA - 55) & 127)]) + ((0.0019406200000000001 * fVec4[((IOTA - 54) & 127)]) + ((0.0061597700000000002 * fVec4[((IOTA - 39) & 127)]) + ((0.00213182 * fVec4[((IOTA - 48) & 127)]) + ((0.0044656900000000005 * fVec4[((IOTA - 47) & 127)]) + ((0.0055347900000000004 * fVec4[((IOTA - 46) & 127)]) + ((8.1563499999999999e-05 * fVec5[((IOTA - 118) & 127)]) + ((7.0606100000000001e-05 * fVec5[((IOTA - 117) & 127)]) + ((4.6020600000000005e-05 * fVec5[((IOTA - 116) & 127)]) + ((2.3044100000000002e-05 * fVec5[((IOTA - 115) & 127)]) + ((3.8369700000000006e-05 * fVec5[((IOTA - 114) & 127)]) + ((0.00029687600000000001 * fVec5[((IOTA - 113) & 127)]) + ((0.00024899699999999999 * fVec5[((IOTA - 112) & 127)]) + ((8.0695200000000013e-05 * fVec5[((IOTA - 111) & 127)]) + ((4.5071899999999998e-05 * fVec5[((IOTA - 110) & 127)]) + ((0.00031248700000000002 * fVec5[((IOTA - 109) & 127)]) + ((0.00029415900000000005 * fVec5[((IOTA - 108) & 127)]) + ((8.8138200000000003e-05 * fVec5[((IOTA - 107) & 127)]) + ((0.00030124200000000001 * fVec5[((IOTA - 106) & 127)]) + ((4.2708300000000006e-05 * fVec5[((IOTA - 105) & 127)]) + ((6.9614300000000009e-05 * fVec5[((IOTA - 103) & 127)]) + ((0.000321589 * fVec5[((IOTA - 99) & 127)]) + ((0.00016036600000000003 * fVec5[((IOTA - 98) & 127)]) + ((0.0012729099999999999 * fVec5[((IOTA - 88) & 127)]) + ((0.0021085400000000003 * fVec5[((IOTA - 87) & 127)]) + ((0.0005086 * fVec5[((IOTA - 86) & 127)]) + ((0.00014791000000000001 * fVec5[((IOTA - 85) & 127)]) + ((0.00032534600000000001 * fVec5[((IOTA - 81) & 127)]) + ((0.00099685500000000005 * fVec5[((IOTA - 80) & 127)]) + ((0.00052548700000000005 * fVec5[((IOTA - 61) & 127)]) + ((0.00033912900000000003 * fVec5[((IOTA - 58) & 127)]) + ((0.0021074099999999997 * fVec5[((IOTA - 55) & 127)]) + ((0.0012717499999999999 * fVec5[((IOTA - 54) & 127)]) + ((0.00060490900000000005 * fVec5[((IOTA - 48) & 127)]) + ((0.0040936100000000001 * fVec5[((IOTA - 42) & 127)]) + ((0.0020292500000000002 * fVec5[((IOTA - 41) & 127)]) + ((0.0030470000000000002 * fVec5[((IOTA - 38) & 127)]) + ((0.0096155300000000006 * fVec5[((IOTA - 37) & 127)]) + ((0.00534952 * fVec5[((IOTA - 36) & 127)]) + ((0.010443599999999999 * fVec5[((IOTA - 35) & 127)]) + ((0.014278200000000001 * fVec5[((IOTA - 34) & 127)]) + ((0.007845340000000001 * fVec5[((IOTA - 33) & 127)]) + ((0.006265520000000001 * fVec5[((IOTA - 32) & 127)]) + ((0.0039480399999999999 * fVec5[((IOTA - 31) & 127)]) + ((0.030915100000000001 * fVec5[((IOTA - 29) & 127)]) + ((0.038624800000000001 * fVec5[((IOTA - 28) & 127)]) + ((0.0106274 * fVec5[((IOTA - 26) & 127)]) + ((0.0056327499999999997 * fVec5[((IOTA - 25) & 127)]) + ((0.00093373200000000005 * fVec5[((IOTA - 24) & 127)]) + ((0.0144447 * fVec5[((IOTA - 23) & 127)]) + ((0.0027656299999999998 * fVec5[((IOTA - 14) & 127)]) + ((0.048321599999999999 * fVec5[((IOTA - 13) & 127)]) + ((0.046080900000000001 * fVec5[((IOTA - 12) & 127)]) + ((0.0051723899999999998 * fVec5[((IOTA - 11) & 127)]) + ((0.0011151400000000001 * fVec5[((IOTA - 9) & 127)]) + ((0.00019390000000000002 * fVec5[((IOTA - 4) & 127)]) + ((1.3112700000000001e-05 * fTemp5) + ((0.0018384600000000001 * fVec5[((IOTA - 68) & 127)]) + ((0.00195065 * fVec5[((IOTA - 67) & 127)]) + ((0.0012429300000000002 * fVec5[((IOTA - 66) & 127)]) + ((0.00058476299999999997 * fVec5[((IOTA - 76) & 127)]) + ((0.00186454 * fVec5[((IOTA - 75) & 127)]) + ((0.0013951499999999999 * fVec5[((IOTA - 74) & 127)]) + ((0.00129178 * fVec5[((IOTA - 73) & 127)]) + ((0.0010098400000000001 * fVec5[((IOTA - 64) & 127)]) + ((0.0016883999999999998 * fVec5[((IOTA - 72) & 127)]) + ((0.0018129000000000001 * fVec5[((IOTA - 71) & 127)]) + ((2.4813999999999997e-06 * fVec6[((IOTA - 124) & 127)]) + ((1.3482700000000002e-05 * fVec6[((IOTA - 118) & 127)]) + ((6.7211899999999999e-05 * fVec6[((IOTA - 114) & 127)]) + ((0.00013360000000000002 * fVec6[((IOTA - 113) & 127)]) + ((2.4868199999999998e-07 * fVec6[((IOTA - 112) & 127)]) + ((0.00021603300000000002 * fVec6[((IOTA - 108) & 127)]) + ((0.00013044 * fVec6[((IOTA - 107) & 127)]) + ((0.00031798600000000003 * fVec6[((IOTA - 103) & 127)]) + ((2.5636600000000001e-05 * fVec6[((IOTA - 102) & 127)]) + ((0.00013589400000000001 * fVec6[((IOTA - 101) & 127)]) + ((0.000417124 * fVec6[((IOTA - 100) & 127)]) + ((0.00028324699999999998 * fVec6[((IOTA - 99) & 127)]) + ((0.00072084000000000004 * fVec6[((IOTA - 98) & 127)]) + ((0.0013214800000000001 * fVec6[((IOTA - 94) & 127)]) + ((0.00147025 * fVec6[((IOTA - 93) & 127)]) + ((0.00043513300000000004 * fVec6[((IOTA - 92) & 127)]) + ((0.00047923700000000003 * fVec6[((IOTA - 91) & 127)]) + ((0.00045356600000000003 * fVec6[((IOTA - 90) & 127)]) + ((0.0013659900000000001 * fVec6[((IOTA - 89) & 127)]) + ((0.0012231700000000002 * fVec6[((IOTA - 88) & 127)]) + ((7.6576200000000007e-05 * fVec6[((IOTA - 87) & 127)]) + ((0.00029935000000000004 * fVec6[((IOTA - 83) & 127)]) + ((0.0033423799999999998 * fVec6[((IOTA - 82) & 127)]) + ((0.00229648 * fVec6[((IOTA - 81) & 127)]) + ((0.00063050200000000002 * fVec6[((IOTA - 79) & 127)]) + ((0.00022606700000000002 * fVec6[((IOTA - 77) & 127)]) + ((0.00061629700000000003 * fVec6[((IOTA - 62) & 127)]) + ((0.0020653400000000001 * fVec6[((IOTA - 61) & 127)]) + ((0.0027092700000000002 * fVec6[((IOTA - 60) & 127)]) + ((0.00101213 * fVec6[((IOTA - 59) & 127)]) + ((9.5653400000000015e-05 * fVec6[((IOTA - 58) & 127)]) + ((0.00156251 * fVec6[((IOTA - 54) & 127)]) + ((0.00015190600000000002 * fVec6[((IOTA - 53) & 127)]) + ((0.0026093900000000001 * fVec6[((IOTA - 51) & 127)]) + ((0.0030938500000000004 * fVec6[((IOTA - 50) & 127)]) + ((0.0031350200000000001 * fVec6[((IOTA - 49) & 127)]) + ((0.0040226100000000002 * fVec6[((IOTA - 48) & 127)]) + ((0.0049862600000000002 * fVec6[((IOTA - 44) & 127)]) + ((0.0058413700000000002 * fVec6[((IOTA - 41) & 127)]) + ((0.0045412800000000008 * fVec6[((IOTA - 40) & 127)]) + ((0.0065134700000000004 * fVec6[((IOTA - 37) & 127)]) + ((0.0086880100000000012 * fVec6[((IOTA - 35) & 127)]) + ((0.0107473 * fVec6[((IOTA - 34) & 127)]) + ((0.0015977300000000001 * fVec6[((IOTA - 32) & 127)]) + ((0.0063447800000000004 * fVec6[((IOTA - 30) & 127)]) + ((0.013484899999999999 * fVec6[((IOTA - 27) & 127)]) + ((0.0020287299999999999 * fVec6[((IOTA - 25) & 127)]) + ((0.018480799999999999 * fVec6[((IOTA - 24) & 127)]) + ((0.0056198899999999998 * fVec6[((IOTA - 22) & 127)]) + ((0.017673000000000001 * fVec6[((IOTA - 21) & 127)]) + ((0.0109137 * fVec6[((IOTA - 18) & 127)]) + ((0.0020388099999999998 * fVec6[((IOTA - 17) & 127)]) + ((0.00335082 * fVec6[((IOTA - 16) & 127)]) + ((0.00123416 * fVec6[((IOTA - 14) & 127)]) + ((0.0021818300000000001 * fVec6[((IOTA - 13) & 127)]) + ((0.00012594799999999999 * fVec6[((IOTA - 12) & 127)]) + ((0.00011811100000000002 * fVec6[((IOTA - 11) & 127)]) + ((0.00098081100000000005 * fVec6[((IOTA - 9) & 127)]) + ((0.00044730900000000001 * fVec6[((IOTA - 5) & 127)]) + ((1.2689500000000001e-05 * fVec6[((IOTA - 1) & 127)]) + ((0.000166844 * fVec6[((IOTA - 67) & 127)]) + ((0.00169188 * fVec5[((IOTA - 65) & 127)]) + ((0.00043061600000000004 * fVec6[((IOTA - 70) & 127)]) + ((0.00033741000000000001 * fVec6[((IOTA - 69) & 127)]) + ((9.3007799999999994e-06 * fVec7[((IOTA - 123) & 127)]) + ((8.1740999999999985e-06 * fVec7[((IOTA - 119) & 127)]) + ((3.2763499999999999e-05 * fVec7[((IOTA - 118) & 127)]) + ((5.73293e-05 * fVec7[((IOTA - 116) & 127)]) + ((0.00023654000000000004 * fVec7[((IOTA - 115) & 127)]) + ((0.00019556700000000001 * fVec7[((IOTA - 114) & 127)]) + ((5.6889900000000006e-05 * fVec7[((IOTA - 113) & 127)]) + ((0.00024025900000000002 * fVec7[((IOTA - 112) & 127)]) + ((0.00064424100000000004 * fVec7[((IOTA - 111) & 127)]) + ((0.00037747700000000003 * fVec7[((IOTA - 110) & 127)]) + ((3.6385200000000001e-06 * fVec7[((IOTA - 108) & 127)]) + ((0.00031980600000000001 * fVec7[((IOTA - 107) & 127)]) + ((0.00046841100000000007 * fVec7[((IOTA - 104) & 127)]) + ((0.00039931500000000002 * fVec7[((IOTA - 103) & 127)]) + ((3.42585e-06 * fVec7[((IOTA - 102) & 127)]) + ((0.00028306300000000001 * fVec7[((IOTA - 101) & 127)]) + ((0.0012796000000000001 * fVec7[((IOTA - 92) & 127)]) + ((0.0028355700000000004 * fVec7[((IOTA - 91) & 127)]) + ((0.00027312000000000001 * fVec7[((IOTA - 90) & 127)]) + ((0.00081654800000000006 * fVec7[((IOTA - 85) & 127)]) + ((7.8246599999999997e-05 * fVec7[((IOTA - 84) & 127)]) + ((0.0027393600000000001 * fVec7[((IOTA - 82) & 127)]) + ((0.00270886 * fVec7[((IOTA - 81) & 127)]) + ((0.0012424999999999999 * fVec7[((IOTA - 80) & 127)]) + ((0.00085482200000000009 * fVec7[((IOTA - 79) & 127)]) + ((0.00154955 * fVec7[((IOTA - 72) & 127)]) + ((0.0022211500000000003 * fVec7[((IOTA - 71) & 127)]) + ((0.0016530499999999999 * fVec7[((IOTA - 70) & 127)]) + ((0.0011901600000000002 * fVec7[((IOTA - 69) & 127)]) + ((0.0015993299999999999 * fVec7[((IOTA - 68) & 127)]) + ((0.00083150600000000009 * fVec7[((IOTA - 67) & 127)]) + ((0.0010392999999999999 * fVec7[((IOTA - 64) & 127)]) + ((0.00199607 * fVec7[((IOTA - 54) & 127)]) + ((0.00078433799999999998 * fVec7[((IOTA - 53) & 127)]) + ((0.00289646 * fVec7[((IOTA - 41) & 127)]) + ((0.0028975399999999997 * fVec7[((IOTA - 39) & 127)]) + ((0.018965700000000002 * fVec7[((IOTA - 38) & 127)]) + ((0.0134784 * fVec7[((IOTA - 37) & 127)]) + ((0.0031132099999999999 * fVec7[((IOTA - 36) & 127)]) + ((0.0063885599999999997 * fVec7[((IOTA - 35) & 127)]) + ((0.019759800000000001 * fVec7[((IOTA - 34) & 127)]) + ((0.027778900000000002 * fVec7[((IOTA - 33) & 127)]) + ((0.025745200000000003 * fVec7[((IOTA - 32) & 127)]) + ((0.0028674200000000003 * fVec7[((IOTA - 30) & 127)]) + ((0.023753799999999999 * fVec7[((IOTA - 29) & 127)]) + ((0.020933799999999999 * fVec7[((IOTA - 28) & 127)]) + ((0.0134953 * fVec7[((IOTA - 27) & 127)]) + ((0.024289200000000004 * fVec7[((IOTA - 26) & 127)]) + ((0.0073439100000000004 * fVec7[((IOTA - 25) & 127)]) + ((0.0091244499999999992 * fVec7[((IOTA - 22) & 127)]) + ((0.027517900000000001 * fVec7[((IOTA - 14) & 127)]) + ((0.060376700000000005 * fVec7[((IOTA - 13) & 127)]) + ((0.060888600000000008 * fVec7[((IOTA - 12) & 127)]) + ((0.0051214199999999998 * fVec7[((IOTA - 11) & 127)]) + ((0.00126033 * fVec7[((IOTA - 9) & 127)]) + ((2.0820699999999999e-05 * fVec7[((IOTA - 8) & 127)]) + ((0.00053298400000000002 * fVec7[((IOTA - 5) & 127)]) + ((6.2782600000000011e-05 * fVec7[((IOTA - 4) & 127)]) + ((7.6560500000000007e-05 * fVec7[((IOTA - 3) & 127)]) + ((2.9599300000000003e-05 * fVec7[((IOTA - 1) & 127)]) + ((1.8915500000000002e-05 * fTemp7) + ((0.0019319900000000002 * fVec7[((IOTA - 58) & 127)]) + ((0.00051400900000000006 * fVec7[((IOTA - 55) & 127)]) + ((0.0012512000000000001 * fVec7[((IOTA - 63) & 127)]) + (((1.0336899999999999e-06 * fVec8[((IOTA - 126) & 127)]) + ((1.07578e-06 * fVec8[((IOTA - 124) & 127)]) + ((1.0896600000000002e-05 * fVec8[((IOTA - 123) & 127)]) + ((5.7825300000000005e-06 * fVec8[((IOTA - 121) & 127)]) + ((2.9587800000000002e-05 * fVec8[((IOTA - 120) & 127)]) + ((0.00010999399999999999 * fVec8[((IOTA - 104) & 127)]) + ((1.2511200000000001e-05 * fVec8[((IOTA - 103) & 127)]) + ((3.3373e-05 * fVec8[((IOTA - 101) & 127)]) + ((0.0022768000000000003 * fVec8[((IOTA - 96) & 127)]) + ((0.0020205099999999997 * fVec8[((IOTA - 95) & 127)]) + ((0.00027500499999999999 * fVec8[((IOTA - 91) & 127)]) + ((9.9177200000000003e-05 * fVec8[((IOTA - 87) & 127)]) + ((0.0006416280000000001 * fVec8[((IOTA - 80) & 127)]) + ((0.0018692700000000001 * fVec8[((IOTA - 77) & 127)]) + ((0.000129173 * fVec8[((IOTA - 79) & 127)]) + ((0.0010500000000000002 * fVec8[((IOTA - 76) & 127)]) + ((0.0013042499999999999 * fVec8[((IOTA - 72) & 127)]) + ((0.0025056699999999998 * fVec8[((IOTA - 71) & 127)]) + ((0.0044664700000000002 * fVec8[((IOTA - 70) & 127)]) + ((0.0047280300000000003 * fVec8[((IOTA - 69) & 127)]) + ((0.0037914799999999999 * fVec8[((IOTA - 68) & 127)]) + ((0.0063763400000000003 * fVec8[((IOTA - 67) & 127)]) + ((0.0065615300000000003 * fVec8[((IOTA - 66) & 127)]) + ((0.00222449 * fVec8[((IOTA - 65) & 127)]) + ((0.0014331000000000001 * fVec8[((IOTA - 36) & 127)]) + ((0.0043624700000000002 * fVec8[((IOTA - 34) & 127)]) + ((0.0071496000000000007 * fVec8[((IOTA - 33) & 127)]) + ((0.009442299999999999 * fVec8[((IOTA - 31) & 127)]) + ((0.041430000000000002 * fVec8[((IOTA - 30) & 127)]) + ((0.032932100000000006 * fVec8[((IOTA - 29) & 127)]) + ((0.0056186100000000004 * fVec8[((IOTA - 28) & 127)]) + ((0.024621200000000003 * fVec8[((IOTA - 27) & 127)]) + ((0.042141200000000004 * fVec8[((IOTA - 26) & 127)]) + ((0.028794800000000002 * fVec8[((IOTA - 25) & 127)]) + ((0.06345640000000001 * fVec8[((IOTA - 24) & 127)]) + ((0.07541260000000001 * fVec8[((IOTA - 23) & 127)]) + ((0.00046772600000000007 * fVec8[((IOTA - 21) & 127)]) + ((0.049093400000000002 * fVec8[((IOTA - 20) & 127)]) + ((0.054047100000000001 * fVec8[((IOTA - 19) & 127)]) + ((0.0108471 * fVec8[((IOTA - 16) & 127)]) + ((0.0010471900000000001 * fVec8[((IOTA - 7) & 127)]) + ((0.00047446700000000006 * fVec8[((IOTA - 6) & 127)]) + ((0.00031437800000000003 * fVec8[((IOTA - 2) & 127)]) + ((1.5753899999999999e-07 * fVec9[((IOTA - 126) & 127)]) + ((3.3066799999999999e-06 * fVec9[((IOTA - 125) & 127)]) + ((9.6444300000000008e-05 * fVec9[((IOTA - 110) & 127)]) + ((0.00034096799999999998 * fVec9[((IOTA - 109) & 127)]) + ((1.0190999999999999e-05 * fVec9[((IOTA - 108) & 127)]) + ((0.00023396100000000001 * fVec9[((IOTA - 106) & 127)]) + ((0.00052374700000000008 * fVec9[((IOTA - 105) & 127)]) + ((0.00036215100000000001 * fVec9[((IOTA - 104) & 127)]) + ((0.00063805800000000005 * fVec9[((IOTA - 102) & 127)]) + ((0.0011957500000000002 * fVec9[((IOTA - 101) & 127)]) + ((0.0010047600000000002 * fVec9[((IOTA - 100) & 127)]) + ((0.00095472699999999994 * fVec9[((IOTA - 99) & 127)]) + ((0.00092775000000000008 * fVec9[((IOTA - 98) & 127)]) + ((0.00122649 * fVec9[((IOTA - 97) & 127)]) + ((0.0011230999999999999 * fVec9[((IOTA - 93) & 127)]) + ((0.00125953 * fVec9[((IOTA - 92) & 127)]) + ((0.00019295400000000001 * fVec9[((IOTA - 91) & 127)]) + ((0.00055969600000000004 * fVec9[((IOTA - 89) & 127)]) + ((0.000103257 * fVec9[((IOTA - 76) & 127)]) + ((0.00171093 * fVec9[((IOTA - 73) & 127)]) + ((0.0020383699999999999 * fVec9[((IOTA - 72) & 127)]) + ((0.0013637999999999999 * fVec9[((IOTA - 69) & 127)]) + ((0.0024843999999999999 * fVec9[((IOTA - 68) & 127)]) + ((0.00074355600000000003 * fVec9[((IOTA - 67) & 127)]) + ((0.00089791299999999997 * fVec9[((IOTA - 65) & 127)]) + ((0.0028104300000000005 * fVec9[((IOTA - 64) & 127)]) + ((0.0015055300000000001 * fVec9[((IOTA - 63) & 127)]) + ((0.00129704 * fVec9[((IOTA - 62) & 127)]) + ((0.00045463200000000001 * fVec9[((IOTA - 61) & 127)]) + ((0.00292619 * fVec9[((IOTA - 60) & 127)]) + ((0.0039499399999999999 * fVec9[((IOTA - 59) & 127)]) + ((0.00042873700000000005 * fVec9[((IOTA - 57) & 127)]) + ((0.00077501600000000001 * fVec9[((IOTA - 56) & 127)]) + ((0.0057435900000000007 * fVec9[((IOTA - 52) & 127)]) + ((0.0064549899999999999 * fVec9[((IOTA - 45) & 127)]) + ((0.0094244500000000009 * fVec9[((IOTA - 39) & 127)]) + ((0.0011423000000000002 * fVec9[((IOTA - 38) & 127)]) + ((0.0177702 * fVec9[((IOTA - 36) & 127)]) + ((0.0096359600000000007 * fVec9[((IOTA - 35) & 127)]) + ((0.019155500000000002 * fVec9[((IOTA - 33) & 127)]) + ((0.019293299999999999 * fVec9[((IOTA - 32) & 127)]) + ((0.0068834500000000002 * fVec9[((IOTA - 29) & 127)]) + ((0.036199000000000002 * fVec9[((IOTA - 26) & 127)]) + ((0.028412400000000001 * fVec9[((IOTA - 23) & 127)]) + ((0.016930500000000001 * fVec9[((IOTA - 20) & 127)]) + ((0.0106017 * fVec9[((IOTA - 19) & 127)]) + ((0.000263627 * fVec9[((IOTA - 17) & 127)]) + ((0.00025129900000000002 * fVec9[((IOTA - 15) & 127)]) + ((0.00047378999999999998 * fVec9[((IOTA - 14) & 127)]) + ((0.000359551 * fVec9[((IOTA - 11) & 127)]) + ((0.00050858999999999995 * fVec9[((IOTA - 10) & 127)]) + ((0.00025865000000000003 * fVec9[((IOTA - 8) & 127)]) + ((0.00030013000000000001 * fVec9[((IOTA - 7) & 127)]) + ((0.00012962100000000001 * fVec9[((IOTA - 4) & 127)]) + ((5.1984399999999995e-06 * fVec9[((IOTA - 2) & 127)]) + ((2.0081900000000002e-05 * fVec9[((IOTA - 1) & 127)]) + ((5.35095e-06 * fTemp9) + (((6.5997900000000001e-08 * fVec10[((IOTA - 126) & 127)]) + ((2.1505000000000002e-05 * fVec10[((IOTA - 111) & 127)]) + ((0.00025875799999999999 * fVec10[((IOTA - 110) & 127)]) + ((0.00016541 * fVec10[((IOTA - 109) & 127)]) + ((0.00012994100000000001 * fVec10[((IOTA - 108) & 127)]) + ((0.000501459 * fVec10[((IOTA - 107) & 127)]) + ((0.00061807300000000002 * fVec10[((IOTA - 106) & 127)]) + ((0.00041380000000000003 * fVec10[((IOTA - 105) & 127)]) + ((4.2136500000000006e-05 * fVec10[((IOTA - 102) & 127)]) + ((1.1025e-06 * fVec10[((IOTA - 101) & 127)]) + ((0.000102185 * fVec10[((IOTA - 100) & 127)]) + ((0.0010346200000000002 * fVec10[((IOTA - 99) & 127)]) + ((0.0015065 * fVec10[((IOTA - 98) & 127)]) + ((0.00015973800000000001 * fVec10[((IOTA - 94) & 127)]) + ((0.00065440400000000003 * fVec10[((IOTA - 93) & 127)]) + ((0.00049362799999999997 * fVec10[((IOTA - 91) & 127)]) + ((0.0010192300000000001 * fVec10[((IOTA - 90) & 127)]) + ((7.1971000000000006e-05 * fVec10[((IOTA - 85) & 127)]) + ((0.00051516900000000011 * fVec10[((IOTA - 84) & 127)]) + ((0.00076406400000000006 * fVec10[((IOTA - 83) & 127)]) + ((9.4944000000000018e-05 * fVec10[((IOTA - 81) & 127)]) + ((0.00095418100000000002 * fVec10[((IOTA - 80) & 127)]) + ((0.00034814200000000002 * fVec10[((IOTA - 76) & 127)]) + ((8.0286900000000005e-05 * fVec10[((IOTA - 71) & 127)]) + ((0.00159566 * fVec10[((IOTA - 70) & 127)]) + ((1.1146700000000001e-05 * fVec10[((IOTA - 67) & 127)]) + ((0.0016114299999999999 * fVec10[((IOTA - 66) & 127)]) + ((0.0030693399999999998 * fVec10[((IOTA - 65) & 127)]) + ((0.0029336900000000001 * fVec10[((IOTA - 64) & 127)]) + ((0.00021672300000000002 * fVec10[((IOTA - 61) & 127)]) + ((0.0015251900000000001 * fVec10[((IOTA - 60) & 127)]) + ((0.00063413500000000008 * fVec10[((IOTA - 59) & 127)]) + ((0.0015717800000000001 * fVec10[((IOTA - 40) & 127)]) + ((0.0086705299999999992 * fVec10[((IOTA - 37) & 127)]) + ((0.00411948 * fVec10[((IOTA - 34) & 127)]) + ((0.0107584 * fVec10[((IOTA - 33) & 127)]) + ((0.0029588800000000001 * fVec10[((IOTA - 32) & 127)]) + ((0.0139764 * fVec10[((IOTA - 30) & 127)]) + ((0.0093965200000000002 * fVec10[((IOTA - 29) & 127)]) + ((0.016385799999999999 * fVec10[((IOTA - 28) & 127)]) + ((0.015254500000000001 * fVec10[((IOTA - 27) & 127)]) + ((0.0099207400000000008 * fVec10[((IOTA - 26) & 127)]) + ((0.041638400000000006 * fVec10[((IOTA - 25) & 127)]) + ((0.020153599999999997 * fVec10[((IOTA - 24) & 127)]) + ((0.00032049699999999999 * fVec10[((IOTA - 23) & 127)]) + ((0.034925999999999999 * fVec10[((IOTA - 22) & 127)]) + ((0.0069752599999999996 * fVec10[((IOTA - 21) & 127)]) + ((0.0037277 * fVec10[((IOTA - 20) & 127)]) + ((0.045202099999999995 * fVec10[((IOTA - 19) & 127)]) + ((0.00526948 * fVec10[((IOTA - 16) & 127)]) + ((0.000358363 * fVec10[((IOTA - 10) & 127)]) + ((0.00023124899999999999 * fVec10[((IOTA - 7) & 127)]) + ((0.00020135900000000002 * fVec10[((IOTA - 6) & 127)]) + (((7.2115600000000003e-07 * fVec11[((IOTA - 126) & 127)]) + ((3.4760399999999997e-08 * fVec11[((IOTA - 123) & 127)]) + ((2.0505700000000003e-05 * fVec11[((IOTA - 122) & 127)]) + ((7.2276399999999995e-06 * fVec11[((IOTA - 121) & 127)]) + ((1.3220600000000002e-05 * fVec11[((IOTA - 120) & 127)]) + ((1.2673900000000001e-05 * fVec11[((IOTA - 118) & 127)]) + ((4.4900900000000009e-05 * fVec11[((IOTA - 117) & 127)]) + ((4.1691399999999999e-05 * fVec11[((IOTA - 110) & 127)]) + ((0.00027668200000000005 * fVec11[((IOTA - 92) & 127)]) + ((0.00280241 * fVec11[((IOTA - 82) & 127)]) + ((0.0020786799999999999 * fVec11[((IOTA - 81) & 127)]) + ((0.00091981900000000011 * fVec11[((IOTA - 80) & 127)]) + ((0.0002275 * fVec11[((IOTA - 79) & 127)]) + ((7.5101900000000007e-05 * fVec11[((IOTA - 76) & 127)]) + ((0.00076943900000000002 * fVec11[((IOTA - 72) & 127)]) + ((0.00063752100000000005 * fVec11[((IOTA - 71) & 127)]) + ((0.0026788099999999998 * fVec11[((IOTA - 70) & 127)]) + ((0.0036546199999999999 * fVec11[((IOTA - 69) & 127)]) + ((7.3547000000000006e-05 * fVec11[((IOTA - 68) & 127)]) + ((0.0010616200000000001 * fVec11[((IOTA - 66) & 127)]) + ((0.00033734999999999999 * fVec11[((IOTA - 64) & 127)]) + ((0.00050336900000000004 * fVec11[((IOTA - 63) & 127)]) + ((0.0013227800000000002 * fVec11[((IOTA - 61) & 127)]) + ((0.0013802 * fVec11[((IOTA - 60) & 127)]) + ((0.00057507100000000002 * fVec11[((IOTA - 59) & 127)]) + ((0.0017853699999999999 * fVec11[((IOTA - 58) & 127)]) + ((0.0034082300000000004 * fVec11[((IOTA - 57) & 127)]) + ((0.00345985 * fVec11[((IOTA - 56) & 127)]) + ((0.00155444 * fVec11[((IOTA - 53) & 127)]) + ((0.0014091100000000001 * fVec11[((IOTA - 52) & 127)]) + ((0.0018625499999999999 * fVec11[((IOTA - 51) & 127)]) + ((0.00053192199999999999 * fVec11[((IOTA - 48) & 127)]) + ((0.000148231 * fVec11[((IOTA - 47) & 127)]) + ((0.0072206400000000004 * fVec11[((IOTA - 44) & 127)]) + ((0.0011467599999999999 * fVec11[((IOTA - 43) & 127)]) + ((0.0050838799999999998 * fVec11[((IOTA - 35) & 127)]) + ((0.013509400000000001 * fVec11[((IOTA - 32) & 127)]) + ((0.015784800000000002 * fVec11[((IOTA - 27) & 127)]) + ((0.0024348900000000003 * fVec11[((IOTA - 25) & 127)]) + ((0.0034120499999999998 * fVec11[((IOTA - 24) & 127)]) + ((0.00206245 * fVec11[((IOTA - 23) & 127)]) + ((0.036800600000000003 * fVec11[((IOTA - 19) & 127)]) + ((0.016894100000000002 * fVec11[((IOTA - 16) & 127)]) + ((0.015907999999999999 * fVec11[((IOTA - 15) & 127)]) + ((0.00139284 * fVec11[((IOTA - 13) & 127)]) + ((0.0111489 * fVec11[((IOTA - 12) & 127)]) + ((0.00048254200000000003 * fVec11[((IOTA - 10) & 127)]) + ((0.00084440200000000006 * fVec11[((IOTA - 7) & 127)]) + (((6.1446399999999998e-06 * fVec12[((IOTA - 123) & 127)]) + ((8.1741199999999999e-06 * fVec12[((IOTA - 122) & 127)]) + ((2.2315100000000003e-05 * fVec12[((IOTA - 118) & 127)]) + ((4.6366800000000005e-05 * fVec12[((IOTA - 109) & 127)]) + ((0.00015426800000000002 * fVec12[((IOTA - 108) & 127)]) + ((2.9996500000000003e-05 * fVec12[((IOTA - 104) & 127)]) + ((0.00022863600000000002 * fVec12[((IOTA - 103) & 127)]) + ((0.00013601100000000001 * fVec12[((IOTA - 101) & 127)]) + ((0.00011103000000000001 * fVec12[((IOTA - 100) & 127)]) + ((0.00055044600000000001 * fVec12[((IOTA - 98) & 127)]) + ((0.00044798200000000004 * fVec12[((IOTA - 97) & 127)]) + ((0.00032662200000000005 * fVec12[((IOTA - 96) & 127)]) + ((0.00065044300000000001 * fVec12[((IOTA - 92) & 127)]) + ((0.0011816700000000001 * fVec12[((IOTA - 91) & 127)]) + ((0.00100478 * fVec12[((IOTA - 90) & 127)]) + ((4.4411900000000001e-05 * fVec12[((IOTA - 89) & 127)]) + ((0.000103707 * fVec12[((IOTA - 85) & 127)]) + ((0.00012959000000000001 * fVec12[((IOTA - 84) & 127)]) + ((0.0012166300000000002 * fVec12[((IOTA - 82) & 127)]) + ((0.00073013500000000003 * fVec12[((IOTA - 81) & 127)]) + ((0.0015448500000000002 * fVec12[((IOTA - 80) & 127)]) + ((0.00109789 * fVec12[((IOTA - 79) & 127)]) + ((0.00041635900000000004 * fVec12[((IOTA - 77) & 127)]) + ((0.00033440100000000001 * fVec12[((IOTA - 74) & 127)]) + ((0.00171149 * fVec12[((IOTA - 71) & 127)]) + ((0.0032145400000000001 * fVec12[((IOTA - 70) & 127)]) + ((0.000234556 * fVec12[((IOTA - 69) & 127)]) + ((0.0009296310000000001 * fVec12[((IOTA - 67) & 127)]) + ((0.00017575500000000001 * fVec12[((IOTA - 66) & 127)]) + ((0.00098071900000000012 * fVec12[((IOTA - 64) & 127)]) + ((0.00040021800000000004 * fVec12[((IOTA - 63) & 127)]) + ((0.0020002800000000001 * fVec12[((IOTA - 61) & 127)]) + ((0.0024495200000000002 * fVec12[((IOTA - 56) & 127)]) + ((0.0022621500000000001 * fVec12[((IOTA - 55) & 127)]) + ((0.0026210900000000004 * fVec12[((IOTA - 54) & 127)]) + ((0.00107755 * fVec12[((IOTA - 53) & 127)]) + ((0.0018869100000000001 * fVec12[((IOTA - 51) & 127)]) + ((0.0016816299999999999 * fVec12[((IOTA - 48) & 127)]) + ((0.00099638500000000002 * fVec12[((IOTA - 47) & 127)]) + ((0.0058219500000000002 * fVec12[((IOTA - 34) & 127)]) + ((0.017078900000000001 * fVec12[((IOTA - 33) & 127)]) + ((0.00087088200000000001 * fVec12[((IOTA - 30) & 127)]) + ((0.027225600000000003 * fVec12[((IOTA - 29) & 127)]) + ((0.00946403 * fVec12[((IOTA - 28) & 127)]) + ((0.0038781100000000002 * fVec12[((IOTA - 25) & 127)]) + ((0.048044200000000002 * fVec12[((IOTA - 24) & 127)]) + ((0.0047823300000000004 * fVec12[((IOTA - 23) & 127)]) + ((0.031593799999999998 * fVec12[((IOTA - 21) & 127)]) + ((0.0092093100000000001 * fVec12[((IOTA - 20) & 127)]) + ((0.0182515 * fVec12[((IOTA - 19) & 127)]) + ((0.0125871 * fVec12[((IOTA - 16) & 127)]) + ((0.0010793700000000001 * fVec12[((IOTA - 14) & 127)]) + ((0.0061485900000000007 * fVec12[((IOTA - 12) & 127)]) + ((0.00108131 * fVec12[((IOTA - 11) & 127)]) + ((0.00149292 * fVec12[((IOTA - 7) & 127)]) + ((0.00062478100000000003 * fVec12[((IOTA - 5) & 127)]) + ((0.00028939400000000006 * fVec12[((IOTA - 3) & 127)]) + ((0.0034718700000000002 * fVec12[((IOTA - 43) & 127)]) + ((1.4188099999999999e-06 * fVec13[((IOTA - 126) & 127)]) + ((3.3187600000000001e-06 * fVec13[((IOTA - 125) & 127)]) + ((5.1482899999999996e-06 * fVec13[((IOTA - 124) & 127)]) + ((8.7600299999999995e-06 * fVec13[((IOTA - 123) & 127)]) + ((5.5660599999999997e-06 * fVec13[((IOTA - 122) & 127)]) + ((5.530960000000001e-05 * fVec13[((IOTA - 121) & 127)]) + ((0.00011918500000000002 * fVec13[((IOTA - 120) & 127)]) + ((9.8678100000000013e-05 * fVec13[((IOTA - 119) & 127)]) + (((4.2207000000000004e-05 * fVec13[((IOTA - 118) & 127)]) + ((3.2490500000000003e-05 * fVec13[((IOTA - 117) & 127)]) + ((0.00011697900000000002 * fVec13[((IOTA - 116) & 127)]) + ((5.6691300000000004e-05 * fVec13[((IOTA - 112) & 127)]) + ((0.000115798 * fVec13[((IOTA - 111) & 127)]) + ((0.0037339199999999999 * fVec13[((IOTA - 96) & 127)]) + ((0.0027517499999999999 * fVec13[((IOTA - 95) & 127)]) + ((0.00065997500000000004 * fVec13[((IOTA - 92) & 127)]) + ((0.0016790700000000002 * fVec13[((IOTA - 91) & 127)]) + ((0.0019817300000000001 * fVec13[((IOTA - 88) & 127)]) + ((0.0040626200000000003 * fVec13[((IOTA - 87) & 127)]) + ((0.0023261500000000004 * fVec13[((IOTA - 86) & 127)]) + ((0.0015044699999999999 * fVec13[((IOTA - 85) & 127)]) + ((0.0010922900000000001 * fVec13[((IOTA - 84) & 127)]) + ((0.0012104800000000001 * fVec13[((IOTA - 64) & 127)]) + ((0.0039074399999999999 * fVec13[((IOTA - 63) & 127)]) + ((0.00102589 * fVec13[((IOTA - 62) & 127)]) + ((0.00070128400000000004 * fVec13[((IOTA - 59) & 127)]) + ((0.00040873600000000004 * fVec13[((IOTA - 58) & 127)]) + ((0.0011136799999999999 * fVec13[((IOTA - 57) & 127)]) + ((0.0035448200000000002 * fVec13[((IOTA - 56) & 127)]) + ((0.0022150400000000002 * fVec13[((IOTA - 55) & 127)]) + ((3.8517100000000004e-05 * fVec13[((IOTA - 54) & 127)]) + ((0.0011952499999999999 * fVec13[((IOTA - 53) & 127)]) + ((0.00017855099999999999 * fVec13[((IOTA - 52) & 127)]) + ((0.0020778099999999998 * fVec13[((IOTA - 51) & 127)]) + ((0.0048699899999999994 * fVec13[((IOTA - 50) & 127)]) + ((0.0020168600000000001 * fVec13[((IOTA - 49) & 127)]) + ((0.0011997199999999998 * fVec13[((IOTA - 47) & 127)]) + ((0.0061526599999999999 * fVec13[((IOTA - 46) & 127)]) + ((0.0028706900000000004 * fVec13[((IOTA - 44) & 127)]) + ((0.011077500000000001 * fVec13[((IOTA - 43) & 127)]) + ((0.0022290399999999998 * fVec13[((IOTA - 42) & 127)]) + ((0.0084232300000000003 * fVec13[((IOTA - 39) & 127)]) + ((0.00127378 * fVec13[((IOTA - 38) & 127)]) + ((0.0125606 * fVec13[((IOTA - 34) & 127)]) + ((0.011542200000000001 * fVec13[((IOTA - 31) & 127)]) + ((0.024367400000000001 * fVec13[((IOTA - 30) & 127)]) + ((0.0537481 * fVec13[((IOTA - 26) & 127)]) + ((0.013059400000000001 * fVec13[((IOTA - 23) & 127)]) + ((0.021176 * fVec13[((IOTA - 22) & 127)]) + ((0.0292041 * fVec13[((IOTA - 18) & 127)]) + ((0.015611 * fVec13[((IOTA - 17) & 127)]) + ((0.0131959 * fVec13[((IOTA - 15) & 127)]) + ((0.0033820999999999999 * fVec13[((IOTA - 13) & 127)]) + ((0.00065433400000000001 * fVec13[((IOTA - 10) & 127)]) + ((0.00012754700000000002 * fVec13[((IOTA - 9) & 127)]) + ((0.00028600400000000002 * fVec13[((IOTA - 8) & 127)]) + ((0.00056964699999999995 * fVec13[((IOTA - 6) & 127)]) + ((0.00055874899999999999 * fVec13[((IOTA - 4) & 127)]) + ((0.000107706 * fVec13[((IOTA - 2) & 127)]) + ((2.3258800000000005e-05 * fVec13[((IOTA - 1) & 127)]) + (((1.8648999999999999e-06 * fVec14[((IOTA - 126) & 127)]) + ((1.0045e-05 * fVec14[((IOTA - 125) & 127)]) + ((1.8977800000000003e-05 * fVec14[((IOTA - 124) & 127)]) + ((1.9873400000000002e-05 * fVec14[((IOTA - 123) & 127)]) + ((3.4954000000000004e-05 * fVec14[((IOTA - 122) & 127)]) + ((9.3229500000000019e-05 * fVec14[((IOTA - 121) & 127)]) + ((0.00012240800000000001 * fVec14[((IOTA - 120) & 127)]) + ((9.5701199999999998e-05 * fVec14[((IOTA - 119) & 127)]) + ((5.5900900000000005e-05 * fVec14[((IOTA - 115) & 127)]) + ((1.9102600000000002e-05 * fVec14[((IOTA - 114) & 127)]) + ((0.000876507 * fVec14[((IOTA - 91) & 127)]) + ((0.0020406000000000001 * fVec14[((IOTA - 90) & 127)]) + ((0.0013280499999999999 * fVec14[((IOTA - 89) & 127)]) + ((0.0017303100000000001 * fVec14[((IOTA - 88) & 127)]) + ((0.0029776099999999999 * fVec14[((IOTA - 87) & 127)]) + ((0.0025195700000000001 * fVec14[((IOTA - 86) & 127)]) + ((0.0011889400000000001 * fVec14[((IOTA - 83) & 127)]) + ((0.000386788 * fVec14[((IOTA - 81) & 127)]) + ((0.00090610300000000012 * fVec14[((IOTA - 80) & 127)]) + ((0.0014508500000000001 * fVec14[((IOTA - 79) & 127)]) + ((0.00082739399999999996 * fVec14[((IOTA - 78) & 127)]) + ((0.00013990400000000002 * fVec14[((IOTA - 76) & 127)]) + ((0.00174807 * fVec14[((IOTA - 75) & 127)]) + ((0.00102092 * fVec14[((IOTA - 74) & 127)]) + ((0.00235812 * fVec14[((IOTA - 63) & 127)]) + ((0.0021519 * fVec14[((IOTA - 62) & 127)]) + ((7.6654099999999985e-06 * fVec14[((IOTA - 61) & 127)]) + ((0.0022741699999999998 * fVec14[((IOTA - 55) & 127)]) + ((0.0042530099999999998 * fVec14[((IOTA - 54) & 127)]) + ((0.0032282000000000001 * fVec14[((IOTA - 53) & 127)]) + ((0.0041592900000000004 * fVec14[((IOTA - 50) & 127)]) + ((0.0075640600000000001 * fVec14[((IOTA - 49) & 127)]) + ((0.0079603199999999999 * fVec14[((IOTA - 48) & 127)]) + ((0.0030812000000000001 * fVec14[((IOTA - 47) & 127)]) + ((0.00018497500000000001 * fVec14[((IOTA - 46) & 127)]) + ((0.00033097600000000002 * fVec14[((IOTA - 43) & 127)]) + ((0.0066906600000000002 * fVec14[((IOTA - 42) & 127)]) + ((0.0081795500000000007 * fVec14[((IOTA - 41) & 127)]) + ((0.00108066 * fVec14[((IOTA - 40) & 127)]) + ((0.0088403900000000001 * fVec14[((IOTA - 36) & 127)]) + ((0.0054466000000000002 * fVec14[((IOTA - 35) & 127)]) + ((0.0040393699999999996 * fVec14[((IOTA - 33) & 127)]) + ((0.033863999999999998 * fVec14[((IOTA - 29) & 127)]) + ((0.0092350000000000002 * fVec14[((IOTA - 28) & 127)]) + ((0.00053732699999999999 * fVec14[((IOTA - 26) & 127)]) + ((0.0092729000000000006 * fVec14[((IOTA - 22) & 127)]) + ((0.022299000000000003 * fVec14[((IOTA - 21) & 127)]) + ((0.0012059800000000002 * fVec14[((IOTA - 20) & 127)]) + ((0.025815500000000002 * fVec14[((IOTA - 17) & 127)]) + ((7.0800700000000008e-05 * fVec14[((IOTA - 9) & 127)]) + ((0.00076956600000000007 * fVec14[((IOTA - 8) & 127)]) + ((0.000231146 * fVec14[((IOTA - 5) & 127)]) + ((0.00014766099999999999 * fVec14[((IOTA - 4) & 127)]) + ((0.0043864799999999999 * fVec14[((IOTA - 24) & 127)]) + ((1.4833700000000001e-06 * fVec0[((IOTA - 126) & 127)]) + ((3.6973199999999999e-06 * fVec0[((IOTA - 125) & 127)]) + ((8.5058500000000009e-06 * fVec0[((IOTA - 124) & 127)]) + ((4.1470500000000002e-05 * fVec0[((IOTA - 123) & 127)]) + ((7.1901800000000003e-05 * fVec0[((IOTA - 122) & 127)]) + ((3.71676e-05 * fVec0[((IOTA - 121) & 127)]) + ((3.3982700000000002e-05 * fVec0[((IOTA - 119) & 127)]) + ((0.00016550199999999999 * fVec0[((IOTA - 118) & 127)]) + ((0.00015893600000000001 * fVec0[((IOTA - 117) & 127)]) + ((0.00011888700000000002 * fVec0[((IOTA - 116) & 127)]) + ((2.6751600000000003e-05 * fVec0[((IOTA - 115) & 127)]) + ((1.5261800000000004e-05 * fVec0[((IOTA - 114) & 127)]) + ((0.00030627700000000004 * fVec0[((IOTA - 113) & 127)]) + ((0.00036830800000000003 * fVec0[((IOTA - 112) & 127)]) + ((0.000251322 * fVec0[((IOTA - 111) & 127)]) + ((0.00022708 * fVec0[((IOTA - 110) & 127)]) + ((0.000239084 * fVec0[((IOTA - 109) & 127)]) + ((9.1238799999999995e-06 * fVec0[((IOTA - 108) & 127)]) + (((0.00015429699999999999 * fVec0[((IOTA - 90) & 127)]) + ((0.00021972700000000002 * fVec0[((IOTA - 89) & 127)]) + ((0.0011643300000000001 * fVec0[((IOTA - 82) & 127)]) + ((0.00065994200000000006 * fVec0[((IOTA - 77) & 127)]) + ((0.0017905 * fVec0[((IOTA - 76) & 127)]) + ((0.00015571700000000001 * fVec0[((IOTA - 71) & 127)]) + ((0.0044549899999999998 * fVec0[((IOTA - 70) & 127)]) + ((0.0039002500000000005 * fVec0[((IOTA - 69) & 127)]) + ((0.00054205300000000002 * fVec0[((IOTA - 68) & 127)]) + ((0.00164631 * fVec0[((IOTA - 67) & 127)]) + ((0.00128867 * fVec0[((IOTA - 66) & 127)]) + ((0.000377902 * fVec0[((IOTA - 65) & 127)]) + ((0.00227327 * fVec0[((IOTA - 64) & 127)]) + ((0.0030369999999999998 * fVec0[((IOTA - 63) & 127)]) + ((0.0023284899999999999 * fVec0[((IOTA - 62) & 127)]) + ((0.0020450900000000003 * fVec0[((IOTA - 61) & 127)]) + ((0.0038926099999999999 * fVec0[((IOTA - 60) & 127)]) + ((0.0051357799999999995 * fVec0[((IOTA - 59) & 127)]) + ((0.0057548899999999995 * fVec0[((IOTA - 58) & 127)]) + ((0.0110121 * fVec0[((IOTA - 57) & 127)]) + ((0.010980500000000001 * fVec0[((IOTA - 56) & 127)]) + ((0.0074067100000000004 * fVec0[((IOTA - 55) & 127)]) + ((0.0058000299999999994 * fVec0[((IOTA - 54) & 127)]) + ((0.0046881500000000003 * fVec0[((IOTA - 53) & 127)]) + ((0.0079024400000000002 * fVec0[((IOTA - 52) & 127)]) + ((0.0143637 * fVec0[((IOTA - 51) & 127)]) + ((0.018899099999999999 * fVec0[((IOTA - 50) & 127)]) + ((0.016699200000000001 * fVec0[((IOTA - 49) & 127)]) + ((0.0121522 * fVec0[((IOTA - 48) & 127)]) + ((0.011685700000000002 * fVec0[((IOTA - 47) & 127)]) + ((0.0122635 * fVec0[((IOTA - 46) & 127)]) + ((0.019806900000000002 * fVec0[((IOTA - 44) & 127)]) + ((0.017303499999999999 * fVec0[((IOTA - 43) & 127)]) + ((0.017569399999999999 * fVec0[((IOTA - 42) & 127)]) + ((0.016072400000000001 * fVec0[((IOTA - 41) & 127)]) + ((0.0227996 * fVec0[((IOTA - 40) & 127)]) + ((0.016965000000000001 * fVec0[((IOTA - 39) & 127)]) + ((0.017693 * fVec0[((IOTA - 38) & 127)]) + ((0.033336499999999998 * fVec0[((IOTA - 37) & 127)]) + ((0.029073099999999998 * fVec0[((IOTA - 36) & 127)]) + ((0.018621700000000001 * fVec0[((IOTA - 35) & 127)]) + ((0.023295900000000001 * fVec0[((IOTA - 34) & 127)]) + ((0.030269900000000002 * fVec0[((IOTA - 33) & 127)]) + ((0.023498000000000002 * fVec0[((IOTA - 32) & 127)]) + ((0.012867100000000001 * fVec0[((IOTA - 31) & 127)]) + ((0.024658199999999998 * fVec0[((IOTA - 30) & 127)]) + ((0.033071500000000004 * fVec0[((IOTA - 29) & 127)]) + ((0.023601800000000003 * fVec0[((IOTA - 28) & 127)]) + ((0.025422899999999998 * fVec0[((IOTA - 27) & 127)]) + ((0.014210199999999999 * fVec0[((IOTA - 26) & 127)]) + ((0.010567 * fVec0[((IOTA - 25) & 127)]) + ((0.056895100000000004 * fVec0[((IOTA - 24) & 127)]) + ((0.00027516900000000002 * fVec0[((IOTA - 2) & 127)]) + ((1.74779e-06 * fVec15[((IOTA - 126) & 127)]) + ((7.1374800000000001e-06 * fVec15[((IOTA - 125) & 127)]) + ((1.69708e-05 * fVec15[((IOTA - 124) & 127)]) + ((7.3980599999999995e-07 * fVec15[((IOTA - 123) & 127)]) + ((5.5996800000000006e-05 * fVec15[((IOTA - 121) & 127)]) + ((0.000117308 * fVec15[((IOTA - 120) & 127)]) + ((0.000195057 * fVec15[((IOTA - 119) & 127)]) + ((0.00020216400000000001 * fVec15[((IOTA - 118) & 127)]) + ((0.00016114100000000001 * fVec15[((IOTA - 117) & 127)]) + ((0.00038352400000000005 * fVec15[((IOTA - 116) & 127)]) + ((0.00050261399999999997 * fVec15[((IOTA - 115) & 127)]) + ((0.00050803600000000001 * fVec15[((IOTA - 114) & 127)]) + ((0.00042659700000000002 * fVec15[((IOTA - 113) & 127)]) + ((0.00050421999999999999 * fVec15[((IOTA - 112) & 127)]) + ((0.0010336799999999999 * fVec15[((IOTA - 111) & 127)]) + ((0.00070924500000000006 * fVec15[((IOTA - 110) & 127)]) + ((0.000779071 * fVec15[((IOTA - 109) & 127)]) + ((0.00095978899999999995 * fVec15[((IOTA - 108) & 127)]) + ((0.0013097499999999999 * fVec15[((IOTA - 107) & 127)]) + ((0.0016259900000000001 * fVec15[((IOTA - 106) & 127)]) + ((0.00100501 * fVec15[((IOTA - 105) & 127)]) + ((0.0012236 * fVec15[((IOTA - 104) & 127)]) + ((0.0018419899999999999 * fVec15[((IOTA - 103) & 127)]) + ((0.00147426 * fVec15[((IOTA - 102) & 127)]) + ((0.0013272 * fVec15[((IOTA - 101) & 127)]) + ((0.00094503799999999993 * fVec15[((IOTA - 100) & 127)]) + ((0.00059498000000000005 * fVec15[((IOTA - 99) & 127)]) + ((0.0025739000000000001 * fVec15[((IOTA - 96) & 127)]) + ((0.0033512100000000003 * fVec15[((IOTA - 95) & 127)]) + ((0.00193882 * fVec15[((IOTA - 88) & 127)]) + ((0.00365475 * fVec15[((IOTA - 87) & 127)]) + ((0.0026745600000000003 * fVec15[((IOTA - 86) & 127)]) + ((0.0017505699999999999 * fVec15[((IOTA - 85) & 127)]) + ((0.0010550900000000001 * fVec15[((IOTA - 84) & 127)]) + ((2.5156400000000002e-05 * fVec15[((IOTA - 83) & 127)]) + (((((((((((((((((((((((((((((((((((((((((((0.0081598500000000015 * fVec15[((IOTA - 37) & 127)]) + ((0.010539099999999999 * fVec15[((IOTA - 36) & 127)]) + ((0.0056564199999999997 * fVec15[((IOTA - 35) & 127)]) + ((0.0272114 * fVec15[((IOTA - 34) & 127)]) + ((0.021074000000000002 * fVec15[((IOTA - 33) & 127)]) + ((0.0080194200000000011 * fVec15[((IOTA - 31) & 127)]) + ((0.017730600000000003 * fVec15[((IOTA - 30) & 127)]) + ((0.0216826 * fVec15[((IOTA - 29) & 127)]) + ((0.0123141 * fVec15[((IOTA - 28) & 127)]) + ((0.0075229400000000005 * fVec15[((IOTA - 27) & 127)]) + ((((((((((((((((((((((((((1.96889e-05 * fTemp15) + (0.00012713900000000002 * fVec15[((IOTA - 1) & 127)])) + (0.00072445800000000009 * fVec15[((IOTA - 3) & 127)])) + (0.00084005400000000002 * fVec15[((IOTA - 4) & 127)])) + (0.00078251799999999995 * fVec15[((IOTA - 5) & 127)])) + (0.00064681700000000005 * fVec15[((IOTA - 6) & 127)])) + (0.00105561 * fVec15[((IOTA - 7) & 127)])) + (0.00107368 * fVec15[((IOTA - 8) & 127)])) + (0.00055752700000000004 * fVec15[((IOTA - 9) & 127)])) + (0.0020685400000000002 * fVec15[((IOTA - 10) & 127)])) + (0.0007391530000000001 * fVec15[((IOTA - 11) & 127)])) + (0.048941400000000003 * fVec15[((IOTA - 12) & 127)])) + (0.053001199999999998 * fVec15[((IOTA - 13) & 127)])) + (0.046698700000000003 * fVec15[((IOTA - 14) & 127)])) + (0.062211200000000001 * fVec15[((IOTA - 15) & 127)])) + (0.043948600000000004 * fVec15[((IOTA - 16) & 127)])) + (0.081345899999999999 * fVec15[((IOTA - 17) & 127)])) + (0.047424299999999996 * fVec15[((IOTA - 18) & 127)])) + (0.041405900000000002 * fVec15[((IOTA - 19) & 127)])) + (0.069732299999999997 * fVec15[((IOTA - 20) & 127)])) + (0.050380599999999998 * fVec15[((IOTA - 21) & 127)])) + (0.111697 * fVec15[((IOTA - 22) & 127)])) + (0.054464300000000007 * fVec15[((IOTA - 23) & 127)])) + (0.036801199999999999 * fVec15[((IOTA - 25) & 127)])) + (0.042423700000000002 * fVec15[((IOTA - 26) & 127)])) + (0.0060665699999999994 * fVec15[((IOTA - 38) & 127)])))))))))))) + (0.0072938400000000002 * fVec15[((IOTA - 39) & 127)])) + (0.0014525200000000001 * fVec15[((IOTA - 40) & 127)])) + (0.0049862900000000009 * fVec15[((IOTA - 41) & 127)])) + (0.014038399999999999 * fVec15[((IOTA - 42) & 127)])) + (0.0105579 * fVec15[((IOTA - 43) & 127)])) + (0.00431154 * fVec15[((IOTA - 45) & 127)])) + (0.0084629900000000001 * fVec15[((IOTA - 46) & 127)])) + (0.0007157 * fVec15[((IOTA - 47) & 127)])) + (0.00067166600000000002 * fVec15[((IOTA - 48) & 127)])) + (0.0046483699999999998 * fVec15[((IOTA - 49) & 127)])) + (0.0014356600000000001 * fVec15[((IOTA - 50) & 127)])) + (0.00020930100000000001 * fVec15[((IOTA - 51) & 127)])) + (0.00139635 * fVec15[((IOTA - 52) & 127)])) + (0.0029229899999999999 * fVec15[((IOTA - 53) & 127)])) + (0.0042595000000000003 * fVec15[((IOTA - 54) & 127)])) + (0.0039973999999999999 * fVec15[((IOTA - 55) & 127)])) + (4.49754e-06 * fVec15[((IOTA - 56) & 127)])) + (0.00278079 * fVec15[((IOTA - 58) & 127)])) + (0.0038020800000000002 * fVec15[((IOTA - 59) & 127)])) + (0.0026122100000000002 * fVec15[((IOTA - 60) & 127)])) + (0.00338193 * fVec15[((IOTA - 61) & 127)])) + (0.0064815100000000002 * fVec15[((IOTA - 62) & 127)])) + (0.0067297599999999996 * fVec15[((IOTA - 63) & 127)])) + (0.0038801100000000004 * fVec15[((IOTA - 64) & 127)])) + (0.0029381799999999999 * fVec15[((IOTA - 65) & 127)])) + (0.0016933400000000002 * fVec15[((IOTA - 66) & 127)])) + (0.0028127900000000003 * fVec15[((IOTA - 67) & 127)])) + (0.0033726400000000001 * fVec15[((IOTA - 68) & 127)])) + (0.00072404000000000001 * fVec15[((IOTA - 70) & 127)])) + (0.0019828699999999999 * fVec15[((IOTA - 71) & 127)])) + (0.0039087499999999999 * fVec15[((IOTA - 72) & 127)])) + (0.0053022700000000004 * fVec15[((IOTA - 73) & 127)])) + (0.0036032700000000004 * fVec15[((IOTA - 74) & 127)])) + (0.0053821700000000004 * fVec15[((IOTA - 75) & 127)])) + (0.0046326400000000004 * fVec15[((IOTA - 76) & 127)])) + (0.0020199900000000002 * fVec15[((IOTA - 77) & 127)])) + (0.0020409600000000001 * fVec15[((IOTA - 78) & 127)])) + (0.00144247 * fVec15[((IOTA - 79) & 127)])) + (0.0026692399999999998 * fVec15[((IOTA - 80) & 127)])) + (0.0015368900000000002 * fVec15[((IOTA - 81) & 127)])) + (0.017027300000000002 * fVec0[((IOTA - 45) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0027836500000000004 * fVec14[((IOTA - 23) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (1.5002200000000002e-05 * fTemp13)))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0068587300000000004 * fVec12[((IOTA - 42) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.00014837300000000001 * fVec11[((IOTA - 3) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))) + (0.00015575000000000002 * fVec10[((IOTA - 2) & 127)]))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.00050830900000000008 * fVec9[((IOTA - 83) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0026334700000000002 * fVec7[((IOTA - 59) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.000594639 * fVec3[((IOTA - 10) & 127)]))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (5.4750600000000001e-07 * fVec2[((IOTA - 1) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) - ((5.3289600000000006e-05 * fVec2[((IOTA - 119) & 127)]) + ((0.000107805 * fVec2[((IOTA - 118) & 127)]) + ((0.00016857100000000001 * fVec2[((IOTA - 117) & 127)]) + ((0.00017462800000000002 * fVec2[((IOTA - 116) & 127)]) + ((0.00020061900000000003 * fVec2[((IOTA - 115) & 127)]) + ((0.00026791400000000001 * fVec2[((IOTA - 114) & 127)]) + ((0.000214133 * fVec2[((IOTA - 113) & 127)]) + ((0.00012941499999999999 * fVec2[((IOTA - 112) & 127)]) + ((0.000341992 * fVec2[((IOTA - 111) & 127)]) + ((0.00073887099999999999 * fVec2[((IOTA - 110) & 127)]) + ((0.000178548 * fVec2[((IOTA - 109) & 127)]) + ((0.00034591100000000002 * fVec2[((IOTA - 107) & 127)]) + ((0.00056230400000000002 * fVec2[((IOTA - 106) & 127)]) + ((0.00034390400000000002 * fVec2[((IOTA - 105) & 127)]) + ((0.00040135500000000005 * fVec2[((IOTA - 102) & 127)]) + ((1.2323300000000001e-05 * fVec2[((IOTA - 99) & 127)]) + ((0.00017320599999999999 * fVec2[((IOTA - 98) & 127)]) + ((0.00137867 * fVec2[((IOTA - 90) & 127)]) + ((0.0011166000000000001 * fVec2[((IOTA - 89) & 127)]) + ((0.00077525200000000004 * fVec2[((IOTA - 88) & 127)]) + ((0.00051248299999999999 * fVec2[((IOTA - 87) & 127)]) + ((0.00034004900000000004 * fVec2[((IOTA - 86) & 127)]) + ((0.000134423 * fVec2[((IOTA - 84) & 127)]) + ((0.0011347400000000002 * fVec2[((IOTA - 83) & 127)]) + ((5.784670000000001e-05 * fVec2[((IOTA - 82) & 127)]) + ((0.0017782899999999999 * fVec2[((IOTA - 77) & 127)]) + ((0.0008637740000000001 * fVec2[((IOTA - 78) & 127)]) + ((0.00095740300000000012 * fVec2[((IOTA - 76) & 127)]) + ((0.00027913800000000001 * fVec2[((IOTA - 75) & 127)]) + ((0.000413929 * fVec2[((IOTA - 74) & 127)]) + ((0.0014493400000000001 * fVec2[((IOTA - 70) & 127)]) + ((0.0024630899999999998 * fVec2[((IOTA - 69) & 127)]) + ((0.00066884400000000004 * fVec2[((IOTA - 68) & 127)]) + ((0.0016410700000000001 * fVec2[((IOTA - 66) & 127)]) + ((0.0028929999999999997 * fVec2[((IOTA - 65) & 127)]) + ((0.00010676699999999999 * fVec2[((IOTA - 60) & 127)]) + ((0.00080710999999999999 * fVec2[((IOTA - 58) & 127)]) + ((0.0015732999999999999 * fVec2[((IOTA - 57) & 127)]) + ((0.00047008900000000003 * fVec2[((IOTA - 56) & 127)]) + ((0.00058858800000000002 * fVec2[((IOTA - 46) & 127)]) + ((0.0089012100000000014 * fVec2[((IOTA - 45) & 127)]) + ((0.0063971999999999996 * fVec2[((IOTA - 44) & 127)]) + ((0.0011654300000000001 * fVec2[((IOTA - 42) & 127)]) + ((0.0065539300000000003 * fVec2[((IOTA - 41) & 127)]) + ((0.00199292 * fVec2[((IOTA - 37) & 127)]) + ((0.0048789100000000002 * fVec2[((IOTA - 36) & 127)]) + ((0.00714622 * fVec2[((IOTA - 35) & 127)]) + ((0.0095548600000000001 * fVec2[((IOTA - 33) & 127)]) + ((0.019952500000000001 * fVec2[((IOTA - 32) & 127)]) + ((0.0029659300000000003 * fVec2[((IOTA - 29) & 127)]) + ((0.029996000000000002 * fVec2[((IOTA - 28) & 127)]) + ((0.0016475999999999999 * fVec2[((IOTA - 27) & 127)]) + ((0.0254705 * fVec2[((IOTA - 25) & 127)]) + ((0.0083854199999999993 * fVec2[((IOTA - 24) & 127)]) + ((0.039960800000000005 * fVec2[((IOTA - 18) & 127)]) + ((0.0071864600000000004 * fVec2[((IOTA - 17) & 127)]) + ((0.032838599999999996 * fVec2[((IOTA - 15) & 127)]) + ((0.00164934 * fVec2[((IOTA - 14) & 127)]) + ((0.0050885600000000007 * fVec2[((IOTA - 13) & 127)]) + ((0.00094482099999999999 * fVec2[((IOTA - 10) & 127)]) + ((7.6144500000000003e-05 * fVec2[((IOTA - 9) & 127)]) + ((0.00057894000000000001 * fVec2[((IOTA - 8) & 127)]) + ((0.00159128 * fVec2[((IOTA - 6) & 127)]) + ((0.00015391 * fVec2[((IOTA - 4) & 127)]) + ((0.00036213800000000002 * fVec2[((IOTA - 2) & 127)]) + (((3.5712499999999999e-07 * fVec1[((IOTA - 126) & 127)]) + ((1.1903999999999999e-06 * fVec1[((IOTA - 125) & 127)]) + ((9.2364300000000006e-06 * fVec1[((IOTA - 124) & 127)]) + ((7.6588799999999991e-06 * fVec1[((IOTA - 123) & 127)]) + ((8.9591899999999999e-06 * fVec1[((IOTA - 120) & 127)]) + ((8.6424300000000005e-05 * fVec1[((IOTA - 119) & 127)]) + ((7.8613700000000009e-05 * fVec1[((IOTA - 118) & 127)]) + ((4.4238600000000007e-05 * fVec1[((IOTA - 117) & 127)]) + ((5.5205099999999998e-06 * fVec1[((IOTA - 115) & 127)]) + ((6.6639900000000005e-05 * fVec1[((IOTA - 114) & 127)]) + ((0.000157369 * fVec1[((IOTA - 111) & 127)]) + ((8.3003200000000005e-05 * fVec1[((IOTA - 110) & 127)]) + ((0.00025192200000000002 * fVec1[((IOTA - 92) & 127)]) + ((0.00091703800000000001 * fVec1[((IOTA - 91) & 127)]) + ((0.0010790400000000001 * fVec1[((IOTA - 88) & 127)]) + ((0.0015949800000000002 * fVec1[((IOTA - 87) & 127)]) + ((0.0014874700000000001 * fVec1[((IOTA - 86) & 127)]) + ((0.00093692899999999997 * fVec1[((IOTA - 85) & 127)]) + ((0.0006546770000000001 * fVec1[((IOTA - 84) & 127)]) + ((0.000134809 * fVec1[((IOTA - 83) & 127)]) + ((0.00019593 * fVec1[((IOTA - 82) & 127)]) + ((0.0010831700000000001 * fVec1[((IOTA - 81) & 127)]) + ((0.00121283 * fVec1[((IOTA - 80) & 127)]) + ((0.0021823700000000003 * fVec1[((IOTA - 79) & 127)]) + ((0.00119166 * fVec1[((IOTA - 78) & 127)]) + ((0.00055397699999999999 * fVec1[((IOTA - 74) & 127)]) + ((0.0020279199999999999 * fVec1[((IOTA - 73) & 127)]) + ((0.00014092800000000002 * fVec1[((IOTA - 72) & 127)]) + ((8.5715000000000015e-05 * fVec1[((IOTA - 70) & 127)]) + ((0.00122613 * fVec1[((IOTA - 69) & 127)]) + ((0.0019218100000000001 * fVec1[((IOTA - 68) & 127)]) + ((0.00059488300000000004 * fVec1[((IOTA - 67) & 127)]) + ((0.000167615 * fVec1[((IOTA - 63) & 127)]) + ((0.00088971800000000006 * fVec1[((IOTA - 61) & 127)]) + ((0.0030282500000000001 * fVec1[((IOTA - 60) & 127)]) + ((0.0047336600000000006 * fVec1[((IOTA - 54) & 127)]) + ((0.0044919299999999999 * fVec1[((IOTA - 53) & 127)]) + ((0.0011470100000000002 * fVec1[((IOTA - 50) & 127)]) + ((0.0012055499999999999 * fVec1[((IOTA - 48) & 127)]) + ((0.0059192300000000002 * fVec1[((IOTA - 47) & 127)]) + ((0.0032900500000000001 * fVec1[((IOTA - 44) & 127)]) + ((0.00224289 * fVec1[((IOTA - 43) & 127)]) + ((0.00017730600000000001 * fVec1[((IOTA - 42) & 127)]) + ((0.0072601800000000006 * fVec1[((IOTA - 41) & 127)]) + ((0.00453353 * fVec1[((IOTA - 38) & 127)]) + ((0.00194991 * fVec1[((IOTA - 37) & 127)]) + ((0.00083337500000000002 * fVec1[((IOTA - 35) & 127)]) + ((0.0247151 * fVec1[((IOTA - 34) & 127)]) + ((0.0069191299999999999 * fVec1[((IOTA - 30) & 127)]) + ((0.0098116000000000002 * fVec1[((IOTA - 29) & 127)]) + ((0.0033760599999999997 * fVec1[((IOTA - 28) & 127)]) + ((0.0036533899999999998 * fVec1[((IOTA - 27) & 127)]) + ((0.0209074 * fVec1[((IOTA - 22) & 127)]) + ((0.0101589 * fVec1[((IOTA - 21) & 127)]) + ((0.016458299999999999 * fVec1[((IOTA - 20) & 127)]) + ((0.021493699999999998 * fVec1[((IOTA - 17) & 127)]) + ((0.0062705200000000008 * fVec1[((IOTA - 16) & 127)]) + ((0.0048823400000000006 * fVec1[((IOTA - 14) & 127)]) + ((0.00084803800000000007 * fVec1[((IOTA - 11) & 127)]) + ((0.0012552099999999999 * fVec1[((IOTA - 8) & 127)]) + ((0.00022105899999999999 * fVec1[((IOTA - 6) & 127)]) + ((0.00024554399999999999 * fVec1[((IOTA - 5) & 127)]) + ((0.00061196600000000003 * fVec1[((IOTA - 4) & 127)]) + ((0.00014579800000000002 * fVec1[((IOTA - 2) & 127)]) + ((0.00061885399999999995 * fVec1[((IOTA - 95) & 127)]) + ((0.00036137300000000001 * fVec1[((IOTA - 96) & 127)]) + ((0.000439844 * fVec1[((IOTA - 107) & 127)]) + ((0.00027481100000000002 * fVec1[((IOTA - 106) & 127)]) + ((3.2093500000000007e-05 * fVec1[((IOTA - 102) & 127)]) + ((0.00066768000000000003 * fVec1[((IOTA - 94) & 127)]) + ((1.2403399999999998e-06 * fVec3[((IOTA - 126) & 127)]) + ((6.5523199999999996e-06 * fVec3[((IOTA - 125) & 127)]) + ((7.103729999999999e-06 * fVec3[((IOTA - 124) & 127)]) + ((4.8057499999999997e-06 * fVec3[((IOTA - 123) & 127)]) + ((0.00017124500000000002 * fVec1[((IOTA - 93) & 127)]) + ((1.4907900000000002e-05 * fVec3[((IOTA - 122) & 127)]) + ((5.6678000000000002e-05 * fVec3[((IOTA - 121) & 127)]) + ((3.2618499999999999e-05 * fVec3[((IOTA - 120) & 127)]) + ((4.2858599999999997e-05 * fVec3[((IOTA - 116) & 127)]) + ((1.2154900000000001e-05 * fVec3[((IOTA - 115) & 127)]) + ((8.1906300000000006e-05 * fVec3[((IOTA - 101) & 127)]) + ((0.00050917300000000003 * fVec3[((IOTA - 100) & 127)]) + ((3.9907800000000004e-05 * fVec3[((IOTA - 92) & 127)]) + ((0.00063396999999999998 * fVec3[((IOTA - 91) & 127)]) + ((0.00126468 * fVec3[((IOTA - 90) & 127)]) + ((0.0016863 * fVec3[((IOTA - 89) & 127)]) + ((0.0009600120000000001 * fVec3[((IOTA - 86) & 127)]) + ((0.00042185499999999995 * fVec3[((IOTA - 85) & 127)]) + ((0.00064300400000000008 * fVec3[((IOTA - 84) & 127)]) + ((0.00199081 * fVec3[((IOTA - 83) & 127)]) + ((0.0032951600000000001 * fVec3[((IOTA - 82) & 127)]) + ((0.00044377400000000002 * fVec3[((IOTA - 81) & 127)]) + ((0.00023944000000000002 * fVec3[((IOTA - 4) & 127)]) + ((4.13336e-05 * fVec3[((IOTA - 2) & 127)]) + ((3.3055699999999998e-06 * fTemp3) + ((0.0062460900000000001 * fVec3[((IOTA - 18) & 127)]) + ((0.012640400000000001 * fVec3[((IOTA - 17) & 127)]) + ((0.00686843 * fVec3[((IOTA - 15) & 127)]) + ((0.0014359399999999999 * fVec3[((IOTA - 13) & 127)]) + ((0.0011996999999999999 * fVec3[((IOTA - 78) & 127)]) + ((0.0027583899999999999 * fVec3[((IOTA - 77) & 127)]) + ((0.00029621600000000002 * fVec3[((IOTA - 76) & 127)]) + ((0.00032057000000000006 * fVec3[((IOTA - 74) & 127)]) + ((0.00011427200000000001 * fVec3[((IOTA - 70) & 127)]) + ((0.0023553899999999997 * fVec3[((IOTA - 69) & 127)]) + ((0.00121634 * fVec3[((IOTA - 61) & 127)]) + ((0.0031460699999999999 * fVec3[((IOTA - 60) & 127)]) + ((0.0028280900000000001 * fVec3[((IOTA - 59) & 127)]) + ((0.0041894900000000006 * fVec3[((IOTA - 58) & 127)]) + ((0.0050431600000000005 * fVec3[((IOTA - 57) & 127)]) + ((0.00070018900000000004 * fVec3[((IOTA - 56) & 127)]) + ((0.00079562299999999997 * fVec3[((IOTA - 55) & 127)]) + ((0.0022291200000000002 * fVec3[((IOTA - 54) & 127)]) + ((0.0010627800000000001 * fVec3[((IOTA - 53) & 127)]) + ((0.0025782299999999999 * fVec3[((IOTA - 52) & 127)]) + ((0.0010116599999999999 * fVec3[((IOTA - 51) & 127)]) + ((0.0030807500000000002 * fVec3[((IOTA - 45) & 127)]) + ((0.0044340900000000008 * fVec3[((IOTA - 44) & 127)]) + ((0.0014659899999999999 * fVec3[((IOTA - 39) & 127)]) + ((0.0075921599999999997 * fVec3[((IOTA - 38) & 127)]) + ((0.0013163199999999999 * fVec3[((IOTA - 36) & 127)]) + ((0.00139712 * fVec3[((IOTA - 35) & 127)]) + ((0.020927999999999999 * fVec3[((IOTA - 31) & 127)]) + ((0.0030580300000000002 * fVec3[((IOTA - 30) & 127)]) + ((0.0031714500000000001 * fVec3[((IOTA - 27) & 127)]) + ((0.0298252 * fVec3[((IOTA - 26) & 127)]) + ((0.0033139300000000001 * fVec3[((IOTA - 23) & 127)]) + ((0.0065595499999999999 * fVec3[((IOTA - 21) & 127)]) + ((0.0090886700000000001 * fVec3[((IOTA - 20) & 127)]) + ((7.7107099999999996e-07 * fVec4[((IOTA - 126) & 127)]) + ((5.8016599999999996e-06 * fVec4[((IOTA - 125) & 127)]) + ((1.0293200000000002e-05 * fVec4[((IOTA - 124) & 127)]) + ((1.26402e-05 * fVec4[((IOTA - 123) & 127)]) + ((0.00021416200000000003 * fVec3[((IOTA - 9) & 127)]) + ((1.2875300000000002e-05 * fVec4[((IOTA - 120) & 127)]) + ((3.0919700000000001e-05 * fVec4[((IOTA - 119) & 127)]) + ((1.3730199999999999e-05 * fVec4[((IOTA - 118) & 127)]) + ((0.00010595300000000001 * fVec4[((IOTA - 117) & 127)]) + ((6.2319599999999999e-05 * fVec4[((IOTA - 116) & 127)]) + ((2.2893700000000001e-05 * fVec4[((IOTA - 114) & 127)]) + ((3.6990800000000005e-05 * fVec4[((IOTA - 113) & 127)]) + (((0.0014893700000000001 * fVec4[((IOTA - 94) & 127)]) + ((0.00075849800000000003 * fVec4[((IOTA - 93) & 127)]) + ((1.7939800000000001e-05 * fVec4[((IOTA - 89) & 127)]) + ((0.00022978000000000001 * fVec4[((IOTA - 88) & 127)]) + ((0.00040871000000000006 * fVec4[((IOTA - 87) & 127)]) + ((0.00042454500000000002 * fVec4[((IOTA - 86) & 127)]) + ((0.00052778200000000008 * fVec4[((IOTA - 85) & 127)]) + ((0.00239894 * fVec4[((IOTA - 84) & 127)]) + ((0.0029262899999999998 * fVec4[((IOTA - 83) & 127)]) + ((0.0016715800000000002 * fVec4[((IOTA - 82) & 127)]) + ((0.0011202900000000001 * fVec4[((IOTA - 81) & 127)]) + ((7.5395799999999992e-06 * fVec4[((IOTA - 80) & 127)]) + ((0.00041440900000000002 * fVec4[((IOTA - 79) & 127)]) + ((3.0152300000000003e-05 * fVec4[((IOTA - 78) & 127)]) + ((0.00071683100000000002 * fVec4[((IOTA - 76) & 127)]) + ((3.8916400000000006e-05 * fVec4[((IOTA - 75) & 127)]) + ((0.0012073799999999999 * fVec4[((IOTA - 74) & 127)]) + ((0.0031728900000000003 * fVec4[((IOTA - 73) & 127)]) + ((0.0018727100000000001 * fVec4[((IOTA - 72) & 127)]) + ((0.0085886299999999999 * fVec4[((IOTA - 37) & 127)]) + ((0.0098696699999999988 * fVec4[((IOTA - 36) & 127)]) + ((0.0081568500000000002 * fVec4[((IOTA - 33) & 127)]) + ((0.0055860800000000002 * fVec4[((IOTA - 32) & 127)]) + ((0.0076456700000000002 * fVec4[((IOTA - 31) & 127)]) + ((0.0047620600000000003 * fVec4[((IOTA - 30) & 127)]) + ((0.00244034 * fVec4[((IOTA - 26) & 127)]) + ((0.0027159100000000002 * fVec4[((IOTA - 25) & 127)]) + ((0.0355257 * fVec4[((IOTA - 24) & 127)]) + ((0.013501399999999998 * fVec4[((IOTA - 21) & 127)]) + ((0.010106800000000001 * fVec4[((IOTA - 20) & 127)]) + ((0.016007400000000001 * fVec4[((IOTA - 19) & 127)]) + ((0.020679400000000001 * fVec4[((IOTA - 18) & 127)]) + ((0.0034218899999999999 * fVec4[((IOTA - 17) & 127)]) + ((0.016782399999999999 * fVec4[((IOTA - 14) & 127)]) + ((2.9919300000000003e-05 * fVec4[((IOTA - 11) & 127)]) + ((3.3796500000000001e-05 * fVec4[((IOTA - 9) & 127)]) + ((0.00054442100000000001 * fVec4[((IOTA - 8) & 127)]) + ((0.00043977500000000005 * fVec4[((IOTA - 6) & 127)]) + ((0.00054478099999999993 * fVec4[((IOTA - 5) & 127)]) + ((0.00022437700000000002 * fVec4[((IOTA - 2) & 127)]) + ((1.9893300000000001e-05 * fVec4[((IOTA - 1) & 127)]) + ((0.0034829700000000002 * fVec4[((IOTA - 41) & 127)]) + ((0.0043833000000000006 * fVec4[((IOTA - 40) & 127)]) + ((0.00091280400000000013 * fVec4[((IOTA - 63) & 127)]) + ((0.0016841900000000001 * fVec4[((IOTA - 57) & 127)]) + ((0.0014167300000000002 * fVec4[((IOTA - 56) & 127)]) + ((8.9493100000000012e-05 * fVec4[((IOTA - 53) & 127)]) + ((0.00032871800000000003 * fVec4[((IOTA - 52) & 127)]) + ((0.00168373 * fVec4[((IOTA - 51) & 127)]) + ((0.0039323600000000002 * fVec4[((IOTA - 50) & 127)]) + ((0.00094270700000000009 * fVec4[((IOTA - 49) & 127)]) + ((0.0014762200000000001 * fVec4[((IOTA - 45) & 127)]) + ((0.0053497099999999997 * fVec4[((IOTA - 44) & 127)]) + ((0.0013171900000000002 * fVec4[((IOTA - 43) & 127)]) + ((7.4901799999999991e-07 * fVec5[((IOTA - 126) & 127)]) + ((5.2121499999999997e-06 * fVec5[((IOTA - 125) & 127)]) + ((1.5791000000000002e-05 * fVec5[((IOTA - 124) & 127)]) + ((2.1421500000000001e-05 * fVec5[((IOTA - 123) & 127)]) + ((2.2224499999999998e-06 * fVec5[((IOTA - 122) & 127)]) + ((2.15894e-05 * fVec5[((IOTA - 121) & 127)]) + ((4.67294e-05 * fVec5[((IOTA - 120) & 127)]) + ((1.9006100000000001e-05 * fVec5[((IOTA - 119) & 127)]) + ((0.00029154800000000003 * fVec5[((IOTA - 104) & 127)]) + ((0.000281437 * fVec5[((IOTA - 102) & 127)]) + ((0.000737136 * fVec5[((IOTA - 101) & 127)]) + ((0.00013028900000000001 * fVec5[((IOTA - 100) & 127)]) + ((0.00065116800000000004 * fVec5[((IOTA - 97) & 127)]) + ((0.00087714900000000003 * fVec5[((IOTA - 96) & 127)]) + ((0.00059851200000000005 * fVec5[((IOTA - 95) & 127)]) + ((0.00058117100000000001 * fVec5[((IOTA - 94) & 127)]) + ((7.4501400000000011e-05 * fVec5[((IOTA - 93) & 127)]) + ((0.00070493400000000005 * fVec5[((IOTA - 92) & 127)]) + ((0.0016990600000000001 * fVec5[((IOTA - 91) & 127)]) + ((0.00105839 * fVec5[((IOTA - 90) & 127)]) + ((0.00064867600000000005 * fVec5[((IOTA - 89) & 127)]) + ((0.00045359999999999997 * fVec5[((IOTA - 84) & 127)]) + ((0.00105405 * fVec5[((IOTA - 83) & 127)]) + ((0.00066430600000000003 * fVec5[((IOTA - 82) & 127)]) + ((1.0579500000000001e-05 * fVec5[((IOTA - 79) & 127)]) + ((9.0909400000000007e-05 * fVec5[((IOTA - 63) & 127)]) + ((7.1183600000000001e-05 * fVec5[((IOTA - 62) & 127)]) + ((0.00144718 * fVec5[((IOTA - 60) & 127)]) + ((0.0011860799999999999 * fVec5[((IOTA - 59) & 127)]) + ((0.0020690300000000003 * fVec5[((IOTA - 57) & 127)]) + ((0.0015602599999999999 * fVec5[((IOTA - 56) & 127)]) + ((0.0016904800000000001 * fVec5[((IOTA - 53) & 127)]) + ((0.00271579 * fVec5[((IOTA - 52) & 127)]) + ((0.0088627000000000011 * fVec5[((IOTA - 51) & 127)]) + ((0.00769069 * fVec5[((IOTA - 50) & 127)]) + ((0.0025811200000000001 * fVec5[((IOTA - 49) & 127)]) + ((0.0020010100000000001 * fVec5[((IOTA - 47) & 127)]) + ((0.0056659300000000004 * fVec5[((IOTA - 46) & 127)]) + ((0.0029520400000000004 * fVec5[((IOTA - 45) & 127)]) + ((0.0094207800000000001 * fVec5[((IOTA - 44) & 127)]) + ((0.0089927300000000009 * fVec5[((IOTA - 43) & 127)]) + ((0.0031344599999999999 * fVec5[((IOTA - 40) & 127)]) + ((0.0020825100000000001 * fVec5[((IOTA - 39) & 127)]) + ((0.010519000000000001 * fVec5[((IOTA - 30) & 127)]) + ((0.0133066 * fVec5[((IOTA - 27) & 127)]) + ((0.024085600000000002 * fVec5[((IOTA - 22) & 127)]) + ((0.0087774500000000009 * fVec5[((IOTA - 21) & 127)]) + ((0.0316397 * fVec5[((IOTA - 20) & 127)]) + ((0.0629913 * fVec5[((IOTA - 19) & 127)]) + ((0.00082095500000000006 * fVec5[((IOTA - 18) & 127)]) + ((0.027291599999999999 * fVec5[((IOTA - 17) & 127)]) + ((0.037210699999999999 * fVec5[((IOTA - 16) & 127)]) + ((0.00091706600000000002 * fVec5[((IOTA - 15) & 127)]) + ((0.00067622200000000009 * fVec5[((IOTA - 10) & 127)]) + ((0.00071360300000000005 * fVec5[((IOTA - 8) & 127)]) + ((0.000243654 * fVec5[((IOTA - 7) & 127)]) + ((0.00032285800000000003 * fVec5[((IOTA - 6) & 127)]) + ((0.000171535 * fVec5[((IOTA - 5) & 127)]) + ((9.1102699999999994e-07 * fVec5[((IOTA - 3) & 127)]) + ((0.000151989 * fVec5[((IOTA - 2) & 127)]) + ((7.3887899999999996e-06 * fVec5[((IOTA - 1) & 127)]) + ((0.00085561100000000005 * fVec5[((IOTA - 78) & 127)]) + ((0.00048425300000000003 * fVec5[((IOTA - 77) & 127)]) + ((0.00017929400000000001 * fVec5[((IOTA - 70) & 127)]) + ((0.0012052899999999999 * fVec5[((IOTA - 69) & 127)]) + ((6.2717999999999998e-07 * fVec6[((IOTA - 126) & 127)]) + ((3.2364399999999999e-06 * fVec6[((IOTA - 125) & 127)]) + ((8.259009999999999e-06 * fVec6[((IOTA - 123) & 127)]) + ((8.9749599999999985e-06 * fVec6[((IOTA - 122) & 127)]) + ((1.8158200000000002e-05 * fVec6[((IOTA - 121) & 127)]) + ((5.1987600000000003e-05 * fVec6[((IOTA - 120) & 127)]) + ((3.15558e-05 * fVec6[((IOTA - 119) & 127)]) + ((5.2186399999999999e-05 * fVec6[((IOTA - 117) & 127)]) + ((0.00018432800000000001 * fVec6[((IOTA - 116) & 127)]) + ((5.6034300000000007e-05 * fVec6[((IOTA - 115) & 127)]) + ((6.3985300000000012e-05 * fVec6[((IOTA - 111) & 127)]) + ((9.3386400000000004e-05 * fVec6[((IOTA - 110) & 127)]) + ((0.000174119 * fVec6[((IOTA - 109) & 127)]) + ((0.00035722100000000001 * fVec6[((IOTA - 106) & 127)]) + ((0.00040952500000000005 * fVec6[((IOTA - 105) & 127)]) + ((0.00017427299999999999 * fVec6[((IOTA - 104) & 127)]) + ((0.00071199800000000004 * fVec6[((IOTA - 97) & 127)]) + ((0.0017188800000000001 * fVec6[((IOTA - 96) & 127)]) + ((0.00054225400000000004 * fVec6[((IOTA - 95) & 127)]) + ((0.000197515 * fVec6[((IOTA - 86) & 127)]) + ((0.0015781700000000001 * fVec6[((IOTA - 85) & 127)]) + ((0.0013863900000000001 * fVec6[((IOTA - 84) & 127)]) + ((1.9453499999999999e-05 * fVec6[((IOTA - 80) & 127)]) + ((0.00084547300000000001 * fVec6[((IOTA - 78) & 127)]) + ((0.0020311800000000001 * fVec6[((IOTA - 65) & 127)]) + ((0.0011967099999999999 * fVec6[((IOTA - 64) & 127)]) + ((6.0100200000000004e-05 * fVec6[((IOTA - 63) & 127)]) + ((0.0025393899999999999 * fVec6[((IOTA - 57) & 127)]) + ((0.0044228100000000001 * fVec6[((IOTA - 56) & 127)]) + ((0.0033445299999999996 * fVec6[((IOTA - 55) & 127)]) + ((0.0038119900000000003 * fVec6[((IOTA - 52) & 127)]) + ((0.00126195 * fVec6[((IOTA - 47) & 127)]) + ((0.00358275 * fVec6[((IOTA - 46) & 127)]) + ((0.0054975199999999997 * fVec6[((IOTA - 45) & 127)]) + ((0.0039510000000000005 * fVec6[((IOTA - 43) & 127)]) + ((0.0080632699999999991 * fVec6[((IOTA - 42) & 127)]) + ((0.0029713500000000002 * fVec6[((IOTA - 39) & 127)]) + ((0.0045339300000000003 * fVec6[((IOTA - 38) & 127)]) + ((0.0034691399999999999 * fVec6[((IOTA - 36) & 127)]) + ((0.011907399999999999 * fVec6[((IOTA - 33) & 127)]) + ((0.0053706500000000003 * fVec6[((IOTA - 31) & 127)]) + ((0.00080878800000000006 * fVec6[((IOTA - 29) & 127)]) + ((0.0049867499999999999 * fVec6[((IOTA - 28) & 127)]) + ((0.0289144 * fVec6[((IOTA - 26) & 127)]) + ((0.0069377800000000002 * fVec6[((IOTA - 23) & 127)]) + ((0.015224500000000002 * fVec6[((IOTA - 20) & 127)]) + ((0.022025899999999998 * fVec6[((IOTA - 19) & 127)]) + ((0.0022348099999999998 * fVec6[((IOTA - 15) & 127)]) + ((0.00084613300000000011 * fVec6[((IOTA - 10) & 127)]) + ((0.00031658400000000002 * fVec6[((IOTA - 8) & 127)]) + ((4.6817000000000003e-05 * fVec6[((IOTA - 7) & 127)]) + ((0.00030049700000000005 * fVec6[((IOTA - 6) & 127)]) + ((0.00030608399999999998 * fVec6[((IOTA - 4) & 127)]) + ((4.2592100000000005e-06 * fVec6[((IOTA - 3) & 127)]) + ((5.0763200000000002e-05 * fVec6[((IOTA - 2) & 127)]) + ((6.9052499999999997e-06 * fTemp6) + ((0.0012562599999999999 * fVec6[((IOTA - 68) & 127)]) + ((0.0010871100000000001 * fVec6[((IOTA - 66) & 127)]) + ((0.00076569700000000008 * fVec6[((IOTA - 76) & 127)]) + ((0.0010572699999999999 * fVec6[((IOTA - 75) & 127)]) + ((7.5785200000000008e-05 * fVec6[((IOTA - 74) & 127)]) + ((0.0013651400000000001 * fVec6[((IOTA - 73) & 127)]) + ((2.74129e-05 * fVec6[((IOTA - 72) & 127)]) + ((0.00022492500000000001 * fVec6[((IOTA - 71) & 127)]) + ((9.1296399999999994e-07 * fVec7[((IOTA - 126) & 127)]) + ((3.51464e-06 * fVec7[((IOTA - 125) & 127)]) + ((2.3408799999999998e-06 * fVec7[((IOTA - 124) & 127)]) + ((1.2859600000000001e-05 * fVec7[((IOTA - 122) & 127)]) + ((6.8135399999999998e-05 * fVec7[((IOTA - 121) & 127)]) + ((7.0622400000000003e-05 * fVec7[((IOTA - 120) & 127)]) + ((4.4616900000000003e-05 * fVec7[((IOTA - 117) & 127)]) + ((0.00015610500000000002 * fVec7[((IOTA - 109) & 127)]) + ((9.1847800000000005e-05 * fVec7[((IOTA - 106) & 127)]) + ((0.00040533400000000003 * fVec7[((IOTA - 105) & 127)]) + ((8.3187099999999995e-05 * fVec7[((IOTA - 100) & 127)]) + ((7.3548200000000008e-05 * fVec7[((IOTA - 99) & 127)]) + ((0.00037095800000000001 * fVec7[((IOTA - 98) & 127)]) + ((0.0010441100000000002 * fVec7[((IOTA - 97) & 127)]) + ((0.00075582300000000009 * fVec7[((IOTA - 96) & 127)]) + ((0.00068145800000000002 * fVec7[((IOTA - 95) & 127)]) + ((0.0017419500000000001 * fVec7[((IOTA - 94) & 127)]) + ((0.0020959099999999999 * fVec7[((IOTA - 93) & 127)]) + ((0.00075253699999999995 * fVec7[((IOTA - 89) & 127)]) + ((0.00076358900000000006 * fVec7[((IOTA - 88) & 127)]) + ((0.00093791200000000006 * fVec7[((IOTA - 87) & 127)]) + ((0.00066410799999999995 * fVec7[((IOTA - 86) & 127)]) + ((2.1531600000000002e-05 * fVec7[((IOTA - 83) & 127)]) + ((0.00025595899999999999 * fVec7[((IOTA - 78) & 127)]) + ((0.0026293000000000002 * fVec7[((IOTA - 77) & 127)]) + ((0.00269454 * fVec7[((IOTA - 76) & 127)]) + ((0.00143391 * fVec7[((IOTA - 75) & 127)]) + ((0.00067648000000000003 * fVec7[((IOTA - 74) & 127)]) + ((0.000121165 * fVec7[((IOTA - 73) & 127)]) + ((0.00082825600000000009 * fVec7[((IOTA - 66) & 127)]) + ((0.00018963900000000002 * fVec7[((IOTA - 65) & 127)]) + ((0.0047187499999999999 * fVec7[((IOTA - 52) & 127)]) + ((0.0066370199999999996 * fVec7[((IOTA - 51) & 127)]) + ((0.0045751999999999998 * fVec7[((IOTA - 50) & 127)]) + ((0.0044307699999999997 * fVec7[((IOTA - 49) & 127)]) + ((0.0081759099999999998 * fVec7[((IOTA - 48) & 127)]) + ((0.0042540899999999994 * fVec7[((IOTA - 47) & 127)]) + ((0.0031707900000000002 * fVec7[((IOTA - 46) & 127)]) + ((0.0049894200000000005 * fVec7[((IOTA - 45) & 127)]) + ((0.0097648300000000004 * fVec7[((IOTA - 44) & 127)]) + ((0.0162506 * fVec7[((IOTA - 43) & 127)]) + ((0.0036288100000000001 * fVec7[((IOTA - 42) & 127)]) + ((0.0078956300000000007 * fVec7[((IOTA - 40) & 127)]) + ((0.0017256800000000001 * fVec7[((IOTA - 31) & 127)]) + ((0.027913199999999999 * fVec7[((IOTA - 24) & 127)]) + ((0.025743499999999999 * fVec7[((IOTA - 23) & 127)]) + ((0.0068529799999999998 * fVec7[((IOTA - 21) & 127)]) + ((0.063915300000000008 * fVec7[((IOTA - 20) & 127)]) + ((0.0661857 * fVec7[((IOTA - 19) & 127)]) + ((0.022665999999999999 * fVec7[((IOTA - 18) & 127)]) + ((0.00195357 * fVec7[((IOTA - 17) & 127)]) + ((0.0555369 * fVec7[((IOTA - 16) & 127)]) + ((0.027306 * fVec7[((IOTA - 15) & 127)]) + ((0.00054477599999999996 * fVec7[((IOTA - 10) & 127)]) + ((0.00092023100000000009 * fVec7[((IOTA - 7) & 127)]) + ((0.00095649800000000007 * fVec7[((IOTA - 6) & 127)]) + ((0.00031540600000000001 * fVec7[((IOTA - 2) & 127)]) + ((0.0035877300000000003 * fVec7[((IOTA - 57) & 127)]) + ((0.0028681000000000002 * fVec7[((IOTA - 56) & 127)]) + ((0.00039613000000000002 * fVec7[((IOTA - 62) & 127)]) + ((0.0029183799999999999 * fVec7[((IOTA - 61) & 127)]) + (((9.8664600000000001e-07 * fVec8[((IOTA - 125) & 127)]) + ((6.1935700000000003e-07 * fVec8[((IOTA - 122) & 127)]) + ((4.9322600000000008e-05 * fVec8[((IOTA - 119) & 127)]) + ((6.2934400000000005e-05 * fVec8[((IOTA - 118) & 127)]) + ((4.2778300000000003e-05 * fVec8[((IOTA - 117) & 127)]) + ((0.000106835 * fVec8[((IOTA - 116) & 127)]) + ((0.00018018099999999999 * fVec8[((IOTA - 115) & 127)]) + ((0.00022798800000000001 * fVec8[((IOTA - 114) & 127)]) + ((0.000195522 * fVec8[((IOTA - 113) & 127)]) + ((0.00011808300000000001 * fVec8[((IOTA - 112) & 127)]) + ((3.4608100000000005e-05 * fVec8[((IOTA - 111) & 127)]) + ((5.723480000000001e-05 * fVec8[((IOTA - 110) & 127)]) + ((0.00097602600000000006 * fVec8[((IOTA - 85) & 127)]) + ((0.0015255399999999999 * fVec8[((IOTA - 84) & 127)]) + ((0.00237411 * fVec8[((IOTA - 83) & 127)]) + ((0.00010142800000000001 * fVec8[((IOTA - 109) & 127)]) + ((9.4569700000000004e-05 * fVec8[((IOTA - 108) & 127)]) + ((0.000158107 * fVec8[((IOTA - 107) & 127)]) + ((0.00070563799999999997 * fVec8[((IOTA - 106) & 127)]) + ((0.00048340500000000007 * fVec8[((IOTA - 105) & 127)]) + ((0.00023814000000000002 * fVec8[((IOTA - 102) & 127)]) + ((0.00015213499999999999 * fVec8[((IOTA - 100) & 127)]) + ((0.0010091500000000001 * fVec8[((IOTA - 99) & 127)]) + ((0.0015510600000000002 * fVec8[((IOTA - 98) & 127)]) + ((0.00028692799999999999 * fVec8[((IOTA - 97) & 127)]) + ((0.00133293 * fVec8[((IOTA - 94) & 127)]) + ((0.0028975800000000003 * fVec8[((IOTA - 93) & 127)]) + ((0.00088707100000000013 * fVec8[((IOTA - 92) & 127)]) + ((0.00187875 * fVec8[((IOTA - 90) & 127)]) + ((0.0031641300000000002 * fVec8[((IOTA - 89) & 127)]) + ((0.0018413699999999999 * fVec8[((IOTA - 88) & 127)]) + ((0.00031068799999999999 * fVec8[((IOTA - 86) & 127)]) + ((0.00112458 * fVec8[((IOTA - 82) & 127)]) + ((0.000380057 * fVec8[((IOTA - 81) & 127)]) + ((0.00020890300000000003 * fVec8[((IOTA - 78) & 127)]) + ((0.000712925 * fVec8[((IOTA - 75) & 127)]) + ((0.00090846500000000001 * fVec8[((IOTA - 74) & 127)]) + ((0.000274135 * fVec8[((IOTA - 73) & 127)]) + ((0.00090694700000000018 * fVec8[((IOTA - 64) & 127)]) + ((0.0024532300000000003 * fVec8[((IOTA - 63) & 127)]) + ((0.00327984 * fVec8[((IOTA - 62) & 127)]) + ((0.0016773199999999999 * fVec8[((IOTA - 61) & 127)]) + ((0.00044967800000000007 * fVec8[((IOTA - 60) & 127)]) + ((0.00165106 * fVec8[((IOTA - 59) & 127)]) + ((0.0048987900000000001 * fVec8[((IOTA - 58) & 127)]) + ((0.0034805299999999999 * fVec8[((IOTA - 57) & 127)]) + ((0.0023563100000000003 * fVec8[((IOTA - 56) & 127)]) + ((0.0045976600000000008 * fVec8[((IOTA - 55) & 127)]) + ((0.00626758 * fVec8[((IOTA - 54) & 127)]) + ((0.0026557099999999999 * fVec8[((IOTA - 53) & 127)]) + ((0.0043297500000000003 * fVec8[((IOTA - 52) & 127)]) + ((0.009209740000000001 * fVec8[((IOTA - 51) & 127)]) + ((0.0029224400000000001 * fVec8[((IOTA - 50) & 127)]) + ((0.0033351100000000001 * fVec8[((IOTA - 49) & 127)]) + ((0.010395300000000001 * fVec8[((IOTA - 48) & 127)]) + ((0.0093701299999999991 * fVec8[((IOTA - 47) & 127)]) + ((0.0063169100000000002 * fVec8[((IOTA - 46) & 127)]) + ((0.013098200000000001 * fVec8[((IOTA - 45) & 127)]) + ((0.014119 * fVec8[((IOTA - 44) & 127)]) + ((0.0021459299999999999 * fVec8[((IOTA - 43) & 127)]) + ((0.0105223 * fVec8[((IOTA - 42) & 127)]) + ((0.022465099999999998 * fVec8[((IOTA - 41) & 127)]) + ((0.0080828600000000007 * fVec8[((IOTA - 40) & 127)]) + ((0.0026383800000000001 * fVec8[((IOTA - 39) & 127)]) + ((0.0090530699999999999 * fVec8[((IOTA - 38) & 127)]) + ((0.0048908699999999994 * fVec8[((IOTA - 37) & 127)]) + ((0.0060908100000000003 * fVec8[((IOTA - 35) & 127)]) + ((0.0095365099999999998 * fVec8[((IOTA - 32) & 127)]) + ((0.00093046600000000002 * fVec8[((IOTA - 22) & 127)]) + ((0.015390200000000001 * fVec8[((IOTA - 18) & 127)]) + ((0.045964499999999998 * fVec8[((IOTA - 17) & 127)]) + ((0.028787799999999999 * fVec8[((IOTA - 15) & 127)]) + ((0.048319200000000007 * fVec8[((IOTA - 14) & 127)]) + ((0.076845299999999991 * fVec8[((IOTA - 13) & 127)]) + ((0.074437400000000001 * fVec8[((IOTA - 12) & 127)]) + ((0.0013813599999999999 * fVec8[((IOTA - 11) & 127)]) + ((9.4869000000000016e-05 * fVec8[((IOTA - 10) & 127)]) + ((0.00097653599999999996 * fVec8[((IOTA - 9) & 127)]) + ((0.00011001600000000001 * fVec8[((IOTA - 8) & 127)]) + ((0.00063438600000000002 * fVec8[((IOTA - 5) & 127)]) + ((0.00012589200000000001 * fVec8[((IOTA - 4) & 127)]) + ((0.00040925999999999999 * fVec8[((IOTA - 3) & 127)]) + ((6.4495100000000005e-05 * fVec8[((IOTA - 1) & 127)]) + ((1.3287400000000001e-05 * fTemp8) + ((7.7027e-06 * fVec9[((IOTA - 124) & 127)]) + ((1.6838600000000002e-05 * fVec9[((IOTA - 123) & 127)]) + ((4.9636399999999996e-06 * fVec9[((IOTA - 122) & 127)]) + ((1.9403399999999997e-06 * fVec9[((IOTA - 121) & 127)]) + ((5.28142e-05 * fVec9[((IOTA - 120) & 127)]) + ((0.00011763 * fVec9[((IOTA - 119) & 127)]) + ((0.00016574 * fVec9[((IOTA - 118) & 127)]) + ((0.00013637100000000001 * fVec9[((IOTA - 117) & 127)]) + ((9.7487500000000005e-05 * fVec9[((IOTA - 116) & 127)]) + ((2.2899500000000003e-05 * fVec9[((IOTA - 115) & 127)]) + ((1.3387100000000001e-05 * fVec9[((IOTA - 114) & 127)]) + ((8.3318600000000019e-05 * fVec9[((IOTA - 113) & 127)]) + ((0.00025527000000000004 * fVec9[((IOTA - 112) & 127)]) + ((0.00034909600000000002 * fVec9[((IOTA - 111) & 127)]) + ((0.00019374600000000001 * fVec9[((IOTA - 107) & 127)]) + ((0.00019838400000000002 * fVec9[((IOTA - 103) & 127)]) + ((0.000168486 * fVec9[((IOTA - 96) & 127)]) + ((0.0012272400000000001 * fVec9[((IOTA - 95) & 127)]) + ((0.00020233 * fVec9[((IOTA - 94) & 127)]) + ((0.00017177199999999999 * fVec9[((IOTA - 90) & 127)]) + ((0.00020847000000000004 * fVec9[((IOTA - 88) & 127)]) + ((0.0011732700000000001 * fVec9[((IOTA - 87) & 127)]) + ((0.00090841200000000004 * fVec9[((IOTA - 86) & 127)]) + ((0.0010732299999999999 * fVec9[((IOTA - 81) & 127)]) + ((0.00257691 * fVec9[((IOTA - 80) & 127)]) + ((0.0017717799999999999 * fVec9[((IOTA - 79) & 127)]) + ((0.000561862 * fVec9[((IOTA - 78) & 127)]) + ((0.000172719 * fVec9[((IOTA - 77) & 127)]) + ((0.0028482099999999999 * fVec9[((IOTA - 75) & 127)]) + ((0.00218125 * fVec9[((IOTA - 74) & 127)]) + ((0.00024288000000000001 * fVec9[((IOTA - 71) & 127)]) + ((0.00186575 * fVec9[((IOTA - 70) & 127)]) + ((0.000588187 * fVec9[((IOTA - 66) & 127)]) + ((0.00080033999999999997 * fVec9[((IOTA - 58) & 127)]) + ((2.5690700000000001e-05 * fVec9[((IOTA - 55) & 127)]) + ((0.00113243 * fVec9[((IOTA - 54) & 127)]) + ((0.0016462200000000001 * fVec9[((IOTA - 53) & 127)]) + ((0.00086436100000000012 * fVec9[((IOTA - 51) & 127)]) + ((0.0055766799999999997 * fVec9[((IOTA - 50) & 127)]) + ((0.00392757 * fVec9[((IOTA - 49) & 127)]) + ((0.0028865100000000001 * fVec9[((IOTA - 48) & 127)]) + ((0.0056144999999999997 * fVec9[((IOTA - 47) & 127)]) + ((0.0026402700000000001 * fVec9[((IOTA - 46) & 127)]) + ((0.0042838500000000005 * fVec9[((IOTA - 44) & 127)]) + ((0.0034927700000000001 * fVec9[((IOTA - 43) & 127)]) + ((0.00038527200000000003 * fVec9[((IOTA - 42) & 127)]) + ((0.0065811400000000009 * fVec9[((IOTA - 41) & 127)]) + ((0.0015727 * fVec9[((IOTA - 40) & 127)]) + ((0.0158301 * fVec9[((IOTA - 37) & 127)]) + ((0.027060000000000001 * fVec9[((IOTA - 34) & 127)]) + ((0.0063968899999999997 * fVec9[((IOTA - 31) & 127)]) + ((0.011650000000000001 * fVec9[((IOTA - 30) & 127)]) + ((0.0092220099999999992 * fVec9[((IOTA - 28) & 127)]) + ((0.0078633899999999996 * fVec9[((IOTA - 27) & 127)]) + ((0.029293300000000001 * fVec9[((IOTA - 25) & 127)]) + ((0.0096560400000000012 * fVec9[((IOTA - 24) & 127)]) + ((0.0144406 * fVec9[((IOTA - 22) & 127)]) + ((0.013951100000000001 * fVec9[((IOTA - 21) & 127)]) + ((0.0084374500000000009 * fVec9[((IOTA - 18) & 127)]) + ((0.0039648399999999999 * fVec9[((IOTA - 16) & 127)]) + ((0.0014542699999999999 * fVec9[((IOTA - 13) & 127)]) + ((0.00036303800000000004 * fVec9[((IOTA - 12) & 127)]) + ((0.00064008700000000001 * fVec9[((IOTA - 9) & 127)]) + ((6.501340000000001e-05 * fVec9[((IOTA - 6) & 127)]) + ((0.00011503900000000001 * fVec9[((IOTA - 5) & 127)]) + ((7.4982900000000003e-05 * fVec9[((IOTA - 3) & 127)]) + ((0.0016132899999999999 * fVec9[((IOTA - 85) & 127)]) + ((0.00035920700000000001 * fVec9[((IOTA - 84) & 127)]) + ((5.3978799999999999e-06 * fVec10[((IOTA - 125) & 127)]) + ((7.9125199999999989e-06 * fVec10[((IOTA - 124) & 127)]) + ((8.4929499999999997e-06 * fVec10[((IOTA - 123) & 127)]) + ((1.1654000000000001e-05 * fVec10[((IOTA - 122) & 127)]) + ((5.1085100000000003e-05 * fVec10[((IOTA - 121) & 127)]) + ((6.8685100000000004e-05 * fVec10[((IOTA - 120) & 127)]) + ((3.2896299999999998e-06 * fVec10[((IOTA - 119) & 127)]) + ((1.41509e-05 * fVec10[((IOTA - 118) & 127)]) + ((1.6980200000000004e-05 * fVec10[((IOTA - 117) & 127)]) + ((3.2223700000000001e-05 * fVec10[((IOTA - 116) & 127)]) + ((5.8471100000000003e-05 * fVec10[((IOTA - 115) & 127)]) + ((0.00031580000000000003 * fVec9[((IOTA - 82) & 127)]) + ((0.00010523700000000001 * fVec10[((IOTA - 114) & 127)]) + ((0.00013697000000000001 * fVec10[((IOTA - 113) & 127)]) + ((0.00010184400000000001 * fVec10[((IOTA - 112) & 127)]) + ((3.2479200000000006e-05 * fVec10[((IOTA - 104) & 127)]) + ((7.9775300000000009e-05 * fVec10[((IOTA - 103) & 127)]) + ((0.000178013 * fVec10[((IOTA - 97) & 127)]) + ((0.00096584900000000007 * fVec10[((IOTA - 96) & 127)]) + ((0.00013887600000000002 * fVec10[((IOTA - 95) & 127)]) + ((0.00019982600000000001 * fVec10[((IOTA - 92) & 127)]) + ((0.00031649800000000002 * fVec10[((IOTA - 89) & 127)]) + ((0.00037059799999999998 * fVec10[((IOTA - 88) & 127)]) + ((0.0012454300000000001 * fVec10[((IOTA - 87) & 127)]) + ((0.00084652999999999996 * fVec10[((IOTA - 86) & 127)]) + ((0.00049063800000000005 * fVec10[((IOTA - 82) & 127)]) + ((0.0013326899999999999 * fVec10[((IOTA - 79) & 127)]) + ((0.00140716 * fVec10[((IOTA - 78) & 127)]) + ((0.00036433400000000001 * fVec10[((IOTA - 77) & 127)]) + ((0.001299 * fVec10[((IOTA - 75) & 127)]) + ((0.0033552399999999998 * fVec10[((IOTA - 74) & 127)]) + ((0.00067232300000000006 * fVec10[((IOTA - 73) & 127)]) + ((0.000145224 * fVec10[((IOTA - 72) & 127)]) + ((0.00118198 * fVec10[((IOTA - 69) & 127)]) + ((0.0029169900000000004 * fVec10[((IOTA - 68) & 127)]) + ((0.000641773 * fVec10[((IOTA - 63) & 127)]) + ((0.0019041500000000001 * fVec10[((IOTA - 62) & 127)]) + ((0.0020271300000000003 * fVec10[((IOTA - 58) & 127)]) + ((0.0037510500000000001 * fVec10[((IOTA - 57) & 127)]) + ((0.0046006600000000003 * fVec10[((IOTA - 56) & 127)]) + ((0.0050914999999999997 * fVec10[((IOTA - 55) & 127)]) + ((0.0044053800000000004 * fVec10[((IOTA - 54) & 127)]) + ((0.0011063700000000002 * fVec10[((IOTA - 53) & 127)]) + ((0.0019731900000000001 * fVec10[((IOTA - 52) & 127)]) + ((0.0069039700000000006 * fVec10[((IOTA - 51) & 127)]) + ((0.0049702700000000006 * fVec10[((IOTA - 43) & 127)]) + ((0.0075730000000000007 * fVec10[((IOTA - 42) & 127)]) + ((0.0060422599999999998 * fVec10[((IOTA - 41) & 127)]) + ((0.0027149600000000002 * fVec10[((IOTA - 39) & 127)]) + ((0.00081970899999999995 * fVec10[((IOTA - 38) & 127)]) + ((5.1858400000000005e-05 * fVec10[((IOTA - 36) & 127)]) + ((0.0045739400000000003 * fVec10[((IOTA - 35) & 127)]) + ((0.00060354400000000002 * fVec10[((IOTA - 31) & 127)]) + ((0.0126207 * fVec10[((IOTA - 18) & 127)]) + ((0.015590100000000001 * fVec10[((IOTA - 17) & 127)]) + ((0.027497799999999999 * fVec10[((IOTA - 15) & 127)]) + ((0.019302299999999998 * fVec10[((IOTA - 14) & 127)]) + ((0.044481599999999996 * fVec10[((IOTA - 13) & 127)]) + ((0.042276800000000003 * fVec10[((IOTA - 12) & 127)]) + ((0.0021177600000000002 * fVec10[((IOTA - 11) & 127)]) + ((0.00027291300000000001 * fVec10[((IOTA - 9) & 127)]) + ((6.1550000000000005e-05 * fVec10[((IOTA - 8) & 127)]) + ((0.000175224 * fVec10[((IOTA - 5) & 127)]) + ((0.00022650199999999998 * fVec10[((IOTA - 4) & 127)]) + ((5.7022700000000005e-05 * fVec10[((IOTA - 3) & 127)]) + ((2.0383200000000002e-05 * fVec10[((IOTA - 1) & 127)]) + ((4.9413399999999999e-06 * fTemp10) + ((0.0058685200000000003 * fVec10[((IOTA - 50) & 127)]) + ((0.0061875000000000003 * fVec10[((IOTA - 49) & 127)]) + ((0.0086182900000000007 * fVec10[((IOTA - 48) & 127)]) + ((0.0054012200000000008 * fVec10[((IOTA - 45) & 127)]) + ((0.0051318099999999997 * fVec10[((IOTA - 47) & 127)]) + ((0.00066500800000000003 * fVec10[((IOTA - 46) & 127)]) + ((5.7104899999999999e-06 * fVec11[((IOTA - 125) & 127)]) + ((1.32724e-05 * fVec11[((IOTA - 124) & 127)]) + (((2.9137400000000003e-05 * fVec11[((IOTA - 119) & 127)]) + ((0.00015924 * fVec11[((IOTA - 116) & 127)]) + ((7.6537400000000003e-05 * fVec11[((IOTA - 115) & 127)]) + ((6.5009300000000008e-05 * fVec11[((IOTA - 114) & 127)]) + ((3.6292700000000006e-05 * fVec11[((IOTA - 113) & 127)]) + ((2.5843400000000004e-05 * fVec11[((IOTA - 112) & 127)]) + ((0.00024267500000000003 * fVec11[((IOTA - 111) & 127)]) + ((0.00031903100000000006 * fVec11[((IOTA - 109) & 127)]) + ((0.00039582199999999999 * fVec11[((IOTA - 108) & 127)]) + ((0.00027277099999999999 * fVec11[((IOTA - 107) & 127)]) + ((0.00092367899999999995 * fVec11[((IOTA - 106) & 127)]) + ((0.0007145500000000001 * fVec11[((IOTA - 105) & 127)]) + ((0.000964846 * fVec11[((IOTA - 104) & 127)]) + ((0.0012146699999999999 * fVec11[((IOTA - 103) & 127)]) + ((0.00057145600000000003 * fVec11[((IOTA - 102) & 127)]) + ((0.00054242299999999999 * fVec11[((IOTA - 101) & 127)]) + ((0.00061493000000000008 * fVec11[((IOTA - 100) & 127)]) + ((0.0012203400000000001 * fVec11[((IOTA - 99) & 127)]) + ((0.0011941500000000002 * fVec11[((IOTA - 98) & 127)]) + ((0.00106711 * fVec11[((IOTA - 97) & 127)]) + ((0.00308618 * fVec11[((IOTA - 96) & 127)]) + ((0.0032894199999999999 * fVec11[((IOTA - 95) & 127)]) + ((0.00149796 * fVec11[((IOTA - 94) & 127)]) + ((0.00068249900000000004 * fVec11[((IOTA - 93) & 127)]) + ((0.00078510099999999996 * fVec11[((IOTA - 91) & 127)]) + ((0.00096550100000000012 * fVec11[((IOTA - 90) & 127)]) + ((0.00058387599999999999 * fVec11[((IOTA - 89) & 127)]) + ((0.0013118799999999999 * fVec11[((IOTA - 88) & 127)]) + ((0.0027314400000000003 * fVec11[((IOTA - 87) & 127)]) + ((0.0023903100000000001 * fVec11[((IOTA - 85) & 127)]) + ((0.0025833200000000001 * fVec11[((IOTA - 84) & 127)]) + ((0.00058356200000000003 * fVec11[((IOTA - 83) & 127)]) + ((0.0013939500000000001 * fVec11[((IOTA - 78) & 127)]) + ((1.7606500000000002e-05 * fVec11[((IOTA - 77) & 127)]) + ((0.0017979000000000001 * fVec11[((IOTA - 75) & 127)]) + ((0.00191797 * fVec11[((IOTA - 74) & 127)]) + ((0.00034747700000000001 * fVec11[((IOTA - 73) & 127)]) + ((0.000544179 * fVec11[((IOTA - 67) & 127)]) + ((5.63033e-05 * fVec11[((IOTA - 65) & 127)]) + ((0.0011524899999999999 * fVec11[((IOTA - 62) & 127)]) + ((0.00156606 * fVec11[((IOTA - 55) & 127)]) + ((0.0015830800000000002 * fVec11[((IOTA - 54) & 127)]) + ((0.0017704200000000002 * fVec11[((IOTA - 50) & 127)]) + ((0.0019330199999999999 * fVec11[((IOTA - 49) & 127)]) + ((0.00266146 * fVec11[((IOTA - 46) & 127)]) + ((0.0011897800000000001 * fVec11[((IOTA - 45) & 127)]) + ((0.00307881 * fVec11[((IOTA - 42) & 127)]) + ((0.0024114100000000001 * fVec11[((IOTA - 41) & 127)]) + ((0.0026392099999999999 * fVec11[((IOTA - 40) & 127)]) + ((0.00380796 * fVec11[((IOTA - 39) & 127)]) + ((0.000320157 * fVec11[((IOTA - 38) & 127)]) + ((0.00022045699999999999 * fVec11[((IOTA - 37) & 127)]) + ((0.0020659800000000002 * fVec11[((IOTA - 36) & 127)]) + ((0.0049689600000000006 * fVec11[((IOTA - 34) & 127)]) + ((0.0075314300000000004 * fVec11[((IOTA - 33) & 127)]) + ((0.0022648999999999998 * fVec11[((IOTA - 31) & 127)]) + ((0.00801492 * fVec11[((IOTA - 30) & 127)]) + ((0.0110397 * fVec11[((IOTA - 29) & 127)]) + ((0.0010152000000000002 * fVec11[((IOTA - 28) & 127)]) + ((0.00287375 * fVec11[((IOTA - 26) & 127)]) + ((0.0161706 * fVec11[((IOTA - 22) & 127)]) + ((0.026332300000000003 * fVec11[((IOTA - 21) & 127)]) + ((0.0079993000000000009 * fVec11[((IOTA - 20) & 127)]) + ((0.00082163500000000003 * fVec11[((IOTA - 18) & 127)]) + ((0.035125400000000001 * fVec11[((IOTA - 17) & 127)]) + ((0.0032833199999999997 * fVec11[((IOTA - 14) & 127)]) + ((0.00033950300000000006 * fVec11[((IOTA - 11) & 127)]) + ((0.00046365600000000006 * fVec11[((IOTA - 9) & 127)]) + ((0.0019217200000000002 * fVec11[((IOTA - 8) & 127)]) + ((0.00055710700000000004 * fVec11[((IOTA - 6) & 127)]) + ((0.00078591400000000008 * fVec11[((IOTA - 5) & 127)]) + ((0.00067488200000000002 * fVec11[((IOTA - 4) & 127)]) + ((0.00020588500000000003 * fVec11[((IOTA - 2) & 127)]) + ((3.4700500000000004e-05 * fVec11[((IOTA - 1) & 127)]) + ((1.2413700000000001e-05 * fTemp11) + ((4.8744899999999996e-07 * fVec12[((IOTA - 126) & 127)]) + ((3.5255099999999998e-06 * fVec12[((IOTA - 125) & 127)]) + ((3.2621699999999995e-06 * fVec12[((IOTA - 124) & 127)]) + ((4.06072e-05 * fVec12[((IOTA - 121) & 127)]) + ((6.3317800000000006e-05 * fVec12[((IOTA - 120) & 127)]) + ((3.5918100000000004e-05 * fVec12[((IOTA - 119) & 127)]) + ((2.5941400000000002e-05 * fVec12[((IOTA - 117) & 127)]) + ((8.6548500000000007e-05 * fVec12[((IOTA - 116) & 127)]) + ((5.5551700000000006e-05 * fVec12[((IOTA - 115) & 127)]) + ((3.6866300000000003e-05 * fVec12[((IOTA - 114) & 127)]) + ((4.2405400000000006e-05 * fVec12[((IOTA - 113) & 127)]) + ((8.1096800000000013e-05 * fVec12[((IOTA - 112) & 127)]) + ((4.85157e-05 * fVec12[((IOTA - 111) & 127)]) + ((7.4248300000000005e-05 * fVec12[((IOTA - 110) & 127)]) + ((6.1852800000000005e-05 * fVec12[((IOTA - 107) & 127)]) + ((0.00033669 * fVec12[((IOTA - 106) & 127)]) + ((0.00035192300000000003 * fVec12[((IOTA - 105) & 127)]) + ((0.000181368 * fVec12[((IOTA - 102) & 127)]) + ((5.5010100000000004e-05 * fVec12[((IOTA - 99) & 127)]) + ((0.000181872 * fVec12[((IOTA - 95) & 127)]) + ((0.0012472700000000002 * fVec12[((IOTA - 94) & 127)]) + ((0.0012369000000000002 * fVec12[((IOTA - 93) & 127)]) + ((0.0031734000000000003 * fVec11[((IOTA - 86) & 127)]) + ((0.00084541900000000014 * fVec12[((IOTA - 88) & 127)]) + ((0.00030825999999999998 * fVec12[((IOTA - 87) & 127)]) + ((0.000991956 * fVec12[((IOTA - 86) & 127)]) + ((0.00039449500000000002 * fVec12[((IOTA - 83) & 127)]) + ((0.00042647700000000008 * fVec12[((IOTA - 78) & 127)]) + ((0.00098188600000000017 * fVec12[((IOTA - 76) & 127)]) + ((0.00076287399999999997 * fVec12[((IOTA - 75) & 127)]) + ((0.00019696800000000003 * fVec12[((IOTA - 73) & 127)]) + ((0.00026003500000000004 * fVec12[((IOTA - 72) & 127)]) + ((0.000162845 * fVec12[((IOTA - 68) & 127)]) + ((0.000175864 * fVec12[((IOTA - 65) & 127)]) + ((0.00122741 * fVec12[((IOTA - 62) & 127)]) + ((0.00051085000000000008 * fVec12[((IOTA - 60) & 127)]) + ((0.0042857099999999999 * fVec12[((IOTA - 59) & 127)]) + ((0.0037509800000000001 * fVec12[((IOTA - 58) & 127)]) + ((0.0013037700000000001 * fVec12[((IOTA - 57) & 127)]) + ((0.000469171 * fVec12[((IOTA - 52) & 127)]) + ((0.0048162999999999999 * fVec12[((IOTA - 50) & 127)]) + ((0.0040944599999999994 * fVec12[((IOTA - 49) & 127)]) + ((0.00164357 * fVec12[((IOTA - 41) & 127)]) + ((0.0066234300000000005 * fVec12[((IOTA - 40) & 127)]) + ((0.0107364 * fVec12[((IOTA - 39) & 127)]) + ((0.0021625699999999999 * fVec12[((IOTA - 38) & 127)]) + ((0.00078494800000000005 * fVec12[((IOTA - 37) & 127)]) + ((0.008554550000000001 * fVec12[((IOTA - 36) & 127)]) + ((0.0087573399999999989 * fVec12[((IOTA - 35) & 127)]) + ((0.0022028300000000002 * fVec12[((IOTA - 32) & 127)]) + ((0.026865800000000002 * fVec12[((IOTA - 31) & 127)]) + ((0.0066563000000000004 * fVec12[((IOTA - 27) & 127)]) + ((0.0173968 * fVec12[((IOTA - 26) & 127)]) + ((0.0065755600000000003 * fVec12[((IOTA - 22) & 127)]) + ((0.044476699999999994 * fVec12[((IOTA - 18) & 127)]) + ((0.019425600000000001 * fVec12[((IOTA - 17) & 127)]) + ((0.0258357 * fVec12[((IOTA - 15) & 127)]) + ((0.00505303 * fVec12[((IOTA - 13) & 127)]) + ((0.00065851199999999999 * fVec12[((IOTA - 10) & 127)]) + ((6.4017400000000008e-05 * fVec12[((IOTA - 9) & 127)]) + ((0.00033249800000000003 * fVec12[((IOTA - 8) & 127)]) + ((0.00114682 * fVec12[((IOTA - 6) & 127)]) + ((0.00043610699999999998 * fVec12[((IOTA - 4) & 127)]) + ((0.000204336 * fVec12[((IOTA - 2) & 127)]) + ((2.2571e-06 * fVec12[((IOTA - 1) & 127)]) + ((1.2883700000000002e-05 * fTemp12) + ((0.0041798500000000006 * fVec12[((IOTA - 46) & 127)]) + ((0.0073487099999999996 * fVec12[((IOTA - 45) & 127)]) + (((7.4329200000000013e-05 * fVec13[((IOTA - 115) & 127)]) + ((0.00029377599999999999 * fVec13[((IOTA - 114) & 127)]) + ((0.000280439 * fVec13[((IOTA - 113) & 127)]) + ((0.00052814500000000005 * fVec13[((IOTA - 110) & 127)]) + ((0.00070229999999999999 * fVec13[((IOTA - 109) & 127)]) + ((0.00080632699999999991 * fVec13[((IOTA - 108) & 127)]) + ((0.000643591 * fVec13[((IOTA - 107) & 127)]) + ((0.00081294000000000006 * fVec13[((IOTA - 106) & 127)]) + ((0.00082950500000000002 * fVec13[((IOTA - 105) & 127)]) + ((0.00023107600000000004 * fVec13[((IOTA - 104) & 127)]) + ((0.00064061300000000005 * fVec13[((IOTA - 103) & 127)]) + ((0.00091664000000000003 * fVec13[((IOTA - 102) & 127)]) + ((0.00073955600000000005 * fVec13[((IOTA - 101) & 127)]) + ((0.00059890600000000007 * fVec13[((IOTA - 100) & 127)]) + ((0.0013073099999999999 * fVec13[((IOTA - 99) & 127)]) + ((0.0027768799999999998 * fVec13[((IOTA - 98) & 127)]) + ((0.00040949500000000001 * fVec13[((IOTA - 97) & 127)]) + ((0.00145302 * fVec13[((IOTA - 94) & 127)]) + ((0.0022861500000000002 * fVec13[((IOTA - 93) & 127)]) + ((0.001377 * fVec13[((IOTA - 90) & 127)]) + ((0.0017080400000000001 * fVec13[((IOTA - 89) & 127)]) + ((0.00016274 * fVec13[((IOTA - 83) & 127)]) + ((0.0027307400000000002 * fVec13[((IOTA - 82) & 127)]) + ((0.0028667500000000004 * fVec13[((IOTA - 81) & 127)]) + ((0.0011399600000000002 * fVec13[((IOTA - 80) & 127)]) + ((0.00110995 * fVec13[((IOTA - 79) & 127)]) + ((0.00124602 * fVec13[((IOTA - 78) & 127)]) + ((0.00129805 * fVec13[((IOTA - 77) & 127)]) + ((0.00100318 * fVec13[((IOTA - 76) & 127)]) + ((0.0022987899999999998 * fVec13[((IOTA - 75) & 127)]) + ((0.0019049100000000001 * fVec13[((IOTA - 74) & 127)]) + ((0.00059075000000000002 * fVec13[((IOTA - 73) & 127)]) + ((0.00103864 * fVec13[((IOTA - 72) & 127)]) + ((0.0013787300000000001 * fVec13[((IOTA - 71) & 127)]) + ((0.00293582 * fVec13[((IOTA - 69) & 127)]) + ((0.0023355800000000003 * fVec13[((IOTA - 68) & 127)]) + ((0.0011070699999999999 * fVec13[((IOTA - 67) & 127)]) + ((0.00050953999999999995 * fVec13[((IOTA - 66) & 127)]) + ((0.00044511000000000003 * fVec13[((IOTA - 65) & 127)]) + ((0.0038312400000000001 * fVec13[((IOTA - 61) & 127)]) + ((0.0016638 * fVec13[((IOTA - 60) & 127)]) + ((0.0043739400000000006 * fVec13[((IOTA - 48) & 127)]) + ((0.0013932899999999999 * fVec13[((IOTA - 45) & 127)]) + ((0.0106382 * fVec13[((IOTA - 41) & 127)]) + ((0.0043469900000000002 * fVec13[((IOTA - 40) & 127)]) + ((0.0015759000000000001 * fVec13[((IOTA - 37) & 127)]) + ((0.0073185999999999998 * fVec13[((IOTA - 36) & 127)]) + ((0.0045973800000000007 * fVec13[((IOTA - 35) & 127)]) + ((0.013183400000000001 * fVec13[((IOTA - 33) & 127)]) + ((0.028645500000000001 * fVec13[((IOTA - 32) & 127)]) + ((0.0092487699999999999 * fVec13[((IOTA - 29) & 127)]) + ((0.039121700000000002 * fVec13[((IOTA - 28) & 127)]) + ((0.00144425 * fVec13[((IOTA - 27) & 127)]) + ((0.0060285 * fVec13[((IOTA - 25) & 127)]) + ((0.057524800000000001 * fVec13[((IOTA - 24) & 127)]) + ((0.021570599999999999 * fVec13[((IOTA - 21) & 127)]) + ((0.0014793400000000002 * fVec13[((IOTA - 20) & 127)]) + ((0.0043988899999999999 * fVec13[((IOTA - 19) & 127)]) + ((0.0073680199999999994 * fVec13[((IOTA - 16) & 127)]) + ((0.00058021900000000009 * fVec13[((IOTA - 14) & 127)]) + ((0.0037306700000000002 * fVec13[((IOTA - 12) & 127)]) + ((0.000216524 * fVec13[((IOTA - 11) & 127)]) + ((0.00050577300000000005 * fVec13[((IOTA - 7) & 127)]) + ((0.00019702400000000002 * fVec13[((IOTA - 5) & 127)]) + ((6.4290300000000003e-05 * fVec13[((IOTA - 3) & 127)]) + (((7.8904099999999995e-07 * fVec14[((IOTA - 118) & 127)]) + ((5.1851200000000009e-05 * fVec14[((IOTA - 117) & 127)]) + ((3.48346e-05 * fVec14[((IOTA - 116) & 127)]) + ((0.000263489 * fVec14[((IOTA - 113) & 127)]) + ((0.00048678400000000003 * fVec14[((IOTA - 112) & 127)]) + ((0.00066577900000000002 * fVec14[((IOTA - 111) & 127)]) + ((0.00072508600000000011 * fVec14[((IOTA - 110) & 127)]) + ((0.00094771800000000006 * fVec14[((IOTA - 109) & 127)]) + ((0.00085589900000000003 * fVec14[((IOTA - 108) & 127)]) + ((0.00097958799999999999 * fVec14[((IOTA - 107) & 127)]) + ((0.00110553 * fVec14[((IOTA - 106) & 127)]) + ((0.00082826600000000003 * fVec14[((IOTA - 105) & 127)]) + ((0.0013655500000000001 * fVec14[((IOTA - 104) & 127)]) + ((0.0018638400000000001 * fVec14[((IOTA - 103) & 127)]) + ((0.0016748900000000001 * fVec14[((IOTA - 102) & 127)]) + ((0.0013615999999999999 * fVec14[((IOTA - 101) & 127)]) + ((0.00120272 * fVec14[((IOTA - 100) & 127)]) + ((0.00103053 * fVec14[((IOTA - 99) & 127)]) + ((0.00040302800000000005 * fVec14[((IOTA - 98) & 127)]) + ((0.0012814900000000001 * fVec14[((IOTA - 97) & 127)]) + ((0.0015273399999999999 * fVec14[((IOTA - 96) & 127)]) + ((0.00038473700000000001 * fVec14[((IOTA - 94) & 127)]) + ((0.0010250300000000001 * fVec14[((IOTA - 93) & 127)]) + ((0.0011346700000000002 * fVec14[((IOTA - 92) & 127)]) + ((0.00043984100000000006 * fVec14[((IOTA - 85) & 127)]) + ((0.00021673800000000001 * fVec14[((IOTA - 84) & 127)]) + ((2.74177e-05 * fVec14[((IOTA - 82) & 127)]) + ((0.0010960700000000002 * fVec14[((IOTA - 77) & 127)]) + ((0.0013700399999999999 * fVec14[((IOTA - 73) & 127)]) + ((0.0030635300000000001 * fVec14[((IOTA - 72) & 127)]) + ((0.0019637600000000002 * fVec14[((IOTA - 71) & 127)]) + ((0.0018260300000000002 * fVec14[((IOTA - 70) & 127)]) + ((0.00238017 * fVec14[((IOTA - 69) & 127)]) + ((0.0031654200000000004 * fVec14[((IOTA - 68) & 127)]) + ((0.00275033 * fVec14[((IOTA - 67) & 127)]) + ((0.00046550599999999997 * fVec14[((IOTA - 95) & 127)]) + ((0.0016457800000000001 * fVec14[((IOTA - 66) & 127)]) + ((0.0025982900000000001 * fVec14[((IOTA - 65) & 127)]) + ((0.0013408299999999999 * fVec14[((IOTA - 64) & 127)]) + ((0.00068048499999999999 * fVec14[((IOTA - 60) & 127)]) + ((0.00122218 * fVec14[((IOTA - 59) & 127)]) + ((0.00152893 * fVec14[((IOTA - 58) & 127)]) + ((0.0022063899999999999 * fVec14[((IOTA - 57) & 127)]) + ((0.00078379700000000003 * fVec14[((IOTA - 56) & 127)]) + ((0.00022987799999999998 * fVec14[((IOTA - 52) & 127)]) + ((0.00078511300000000004 * fVec14[((IOTA - 51) & 127)]) + ((0.0012260999999999999 * fVec14[((IOTA - 45) & 127)]) + ((0.0026344099999999998 * fVec14[((IOTA - 44) & 127)]) + ((0.0038961999999999998 * fVec14[((IOTA - 39) & 127)]) + ((0.011467400000000001 * fVec14[((IOTA - 38) & 127)]) + ((0.0078678299999999993 * fVec14[((IOTA - 37) & 127)]) + ((0.0013363100000000001 * fVec14[((IOTA - 34) & 127)]) + ((0.0089890100000000004 * fVec14[((IOTA - 32) & 127)]) + ((0.0143689 * fVec14[((IOTA - 31) & 127)]) + ((0.0049334100000000001 * fVec14[((IOTA - 30) & 127)]) + ((0.019686699999999998 * fVec14[((IOTA - 27) & 127)]) + ((0.043355600000000001 * fVec14[((IOTA - 19) & 127)]) + ((0.00249322 * fVec14[((IOTA - 18) & 127)]) + ((0.0141169 * fVec14[((IOTA - 16) & 127)]) + ((0.0111431 * fVec14[((IOTA - 15) & 127)]) + ((0.0018486100000000001 * fVec14[((IOTA - 14) & 127)]) + ((0.000976993 * fVec14[((IOTA - 13) & 127)]) + ((0.0056459800000000001 * fVec14[((IOTA - 12) & 127)]) + ((0.00021246900000000004 * fVec14[((IOTA - 11) & 127)]) + ((0.00095016999999999996 * fVec14[((IOTA - 10) & 127)]) + ((0.0010622000000000001 * fVec14[((IOTA - 7) & 127)]) + ((0.00024919200000000002 * fVec14[((IOTA - 6) & 127)]) + ((0.00027049600000000001 * fVec14[((IOTA - 3) & 127)]) + ((5.1123399999999996e-06 * fVec14[((IOTA - 2) & 127)]) + ((3.7580000000000003e-05 * fVec14[((IOTA - 1) & 127)]) + ((1.0426200000000002e-05 * fTemp14) + (((2.5019199999999998e-06 * fVec0[((IOTA - 120) & 127)]) + ((0.00015476800000000001 * fVec0[((IOTA - 107) & 127)]) + ((2.3801800000000003e-05 * fVec0[((IOTA - 106) & 127)]) + ((0.00041578400000000004 * fVec0[((IOTA - 105) & 127)]) + ((0.00040693400000000007 * fVec0[((IOTA - 104) & 127)]) + ((0.00042180200000000004 * fVec0[((IOTA - 103) & 127)]) + ((0.00064933700000000005 * fVec0[((IOTA - 102) & 127)]) + ((0.0010572399999999999 * fVec0[((IOTA - 101) & 127)]) + ((0.00096599000000000012 * fVec0[((IOTA - 100) & 127)]) + ((0.00077855999999999997 * fVec0[((IOTA - 99) & 127)]) + ((0.00073777800000000002 * fVec0[((IOTA - 98) & 127)]) + ((0.00065036 * fVec0[((IOTA - 97) & 127)]) + ((0.00069698000000000004 * fVec0[((IOTA - 96) & 127)]) + ((0.0013956400000000001 * fVec0[((IOTA - 95) & 127)]) + ((0.0020596199999999999 * fVec0[((IOTA - 94) & 127)]) + ((0.0017169000000000002 * fVec0[((IOTA - 93) & 127)]) + ((0.0012716300000000002 * fVec0[((IOTA - 92) & 127)]) + ((0.00130908 * fVec0[((IOTA - 91) & 127)]) + ((0.00161012 * fVec0[((IOTA - 88) & 127)]) + ((0.0016711600000000001 * fVec0[((IOTA - 87) & 127)]) + ((0.00075290600000000002 * fVec0[((IOTA - 86) & 127)]) + ((0.00144013 * fVec0[((IOTA - 85) & 127)]) + ((0.0021273399999999997 * fVec0[((IOTA - 84) & 127)]) + ((0.00061597300000000002 * fVec0[((IOTA - 83) & 127)]) + ((0.00011313 * fVec0[((IOTA - 81) & 127)]) + ((0.0017438199999999999 * fVec0[((IOTA - 80) & 127)]) + ((0.0024799100000000001 * fVec0[((IOTA - 79) & 127)]) + ((0.00256507 * fVec0[((IOTA - 78) & 127)]) + ((0.0011552999999999999 * fVec0[((IOTA - 75) & 127)]) + ((0.00098456699999999999 * fVec0[((IOTA - 74) & 127)]) + ((0.000597807 * fVec0[((IOTA - 73) & 127)]) + ((0.0018893200000000001 * fVec0[((IOTA - 72) & 127)]) + ((0.00092920600000000002 * fVec0[((IOTA - 23) & 127)]) + ((0.080136100000000002 * fVec0[((IOTA - 22) & 127)]) + ((0.041393800000000001 * fVec0[((IOTA - 21) & 127)]) + ((0.042661300000000006 * fVec0[((IOTA - 20) & 127)]) + ((0.0088175100000000006 * fVec0[((IOTA - 19) & 127)]) + ((0.053196100000000003 * fVec0[((IOTA - 18) & 127)]) + ((0.097787600000000002 * fVec0[((IOTA - 17) & 127)]) + ((0.040092800000000005 * fVec0[((IOTA - 16) & 127)]) + ((0.077296500000000004 * fVec0[((IOTA - 15) & 127)]) + ((0.064767000000000005 * fVec0[((IOTA - 14) & 127)]) + ((0.082935599999999998 * fVec0[((IOTA - 13) & 127)]) + ((0.078489900000000001 * fVec0[((IOTA - 12) & 127)]) + ((0.00067357800000000009 * fVec0[((IOTA - 11) & 127)]) + ((0.00149621 * fVec0[((IOTA - 10) & 127)]) + ((0.00072128000000000003 * fVec0[((IOTA - 9) & 127)]) + ((0.00101019 * fVec0[((IOTA - 8) & 127)]) + ((0.000453602 * fVec0[((IOTA - 7) & 127)]) + ((0.00042969900000000002 * fVec0[((IOTA - 6) & 127)]) + ((0.0010723199999999999 * fVec0[((IOTA - 5) & 127)]) + ((0.00084950300000000004 * fVec0[((IOTA - 4) & 127)]) + ((0.00085153200000000001 * fVec0[((IOTA - 3) & 127)]) + ((0.00015027000000000001 * fVec0[((IOTA - 1) & 127)]) + (((((((((((((((((0.00016075400000000001 * fVec15[((IOTA - 2) & 127)]) + (0.021295600000000001 * fVec15[((IOTA - 24) & 127)])) + (0.0029395300000000001 * fVec15[((IOTA - 32) & 127)])) + (0.00092078700000000006 * fVec15[((IOTA - 44) & 127)])) + (0.00063317100000000008 * fVec15[((IOTA - 57) & 127)])) + (0.00036222200000000004 * fVec15[((IOTA - 69) & 127)])) + (0.000687957 * fVec15[((IOTA - 82) & 127)])) + (0.00087519700000000008 * fVec15[((IOTA - 89) & 127)])) + (0.000611186 * fVec15[((IOTA - 90) & 127)])) + (1.7742000000000001e-05 * fVec15[((IOTA - 91) & 127)])) + (0.00196204 * fVec15[((IOTA - 92) & 127)])) + (0.0023706000000000001 * fVec15[((IOTA - 93) & 127)])) + (9.940880000000001e-05 * fVec15[((IOTA - 94) & 127)])) + (0.00047365100000000001 * fVec15[((IOTA - 97) & 127)])) + (0.00048385000000000002 * fVec15[((IOTA - 98) & 127)])) + (1.47262e-06 * fVec15[((IOTA - 122) & 127)])) + (2.4709800000000002e-05 * fTemp0)))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0086390499999999988 * fVec14[((IOTA - 25) & 127)]))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.00181733 * fVec13[((IOTA - 70) & 127)]))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0047594300000000003 * fVec12[((IOTA - 44) & 127)]))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0077257100000000002 * fVec10[((IOTA - 44) & 127)]))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0020411800000000001 * fVec7[((IOTA - 60) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.00044645700000000004 * fVec3[((IOTA - 8) & 127)]))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (1.8821400000000002e-05 * fTemp2)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
			fRec19[0] = max((fRec19[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fHbargraph1 = FAUSTFLOAT(fRec19[0]);
			output1[i] = FAUSTFLOAT(fTemp17);
			fRec1[1] = fRec1[0];
			fRec3[1] = fRec3[0];
			fRec2[1] = fRec2[0];
			IOTA = (IOTA + 1);
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			fRec8[1] = fRec8[0];
			fRec9[1] = fRec9[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			fRec12[1] = fRec12[0];
			fRec13[1] = fRec13[0];
			fRec14[1] = fRec14[0];
			fRec15[1] = fRec15[0];
			fRec16[1] = fRec16[0];
			fRec17[1] = fRec17[0];
			fRec18[1] = fRec18[0];
			fRec0[1] = fRec0[0];
			fRec19[1] = fRec19[0];
			
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
