/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2016"
license: "GPL"
name: "HOABeamHCardio2HOA2"
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

/* link with : "" */
#include <cmath>
#include <math.h>

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
	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	FAUSTFLOAT fHslider2;
	double fConst5;
	double fConst6;
	double fConst7;
	double fConst8;
	double fConst9;
	double fConst10;
	double fConst11;
	double fConst12;
	double fConst13;
	double fConst14;
	double fConst15;
	double fConst16;
	double fConst17;
	double fConst18;
	double fConst19;
	double fConst20;
	double fConst21;
	double fConst22;
	double fConst23;
	double fConst24;
	double fConst25;
	double fConst26;
	double fConst27;
	double fConst28;
	double fConst29;
	double fConst30;
	double fConst31;
	double fConst32;
	double fConst33;
	double fConst34;
	double fConst35;
	double fConst36;
	double fConst37;
	double fConst38;
	double fConst39;
	double fConst40;
	double fConst41;
	double fConst42;
	double fConst43;
	double fConst44;
	double fConst45;
	double fConst46;
	double fConst47;
	double fConst48;
	double fConst49;
	double fConst50;
	double fConst51;
	double fConst52;
	double fConst53;
	double fConst54;
	double fConst55;
	double fConst56;
	double fConst57;
	double fConst58;
	double fConst59;
	double fConst60;
	double fConst61;
	double fConst62;
	double fConst63;
	int fSamplingFreq;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("cijk.lib/author", "Pierre Lecomte");
		m->declare("cijk.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("cijk.lib/license", "GPL");
		m->declare("cijk.lib/name", "Cijk matrix");
		m->declare("cijk.lib/version", "10");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
		m->declare("gui.lib/author", "Pierre Lecomte");
		m->declare("gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("gui.lib/license", "GPL");
		m->declare("gui.lib/name", "GUI Library");
		m->declare("gui.lib/version", "1.0");
		m->declare("license", "GPL");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("name", "HOABeamHCardio2HOA2");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "0.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
		m->declare("ymn.lib/author", "Pierre Lecomte");
		m->declare("ymn.lib/copyright", "(c) Pierre Lecomte 2017");
		m->declare("ymn.lib/license", "GPL");
		m->declare("ymn.lib/name", "Spherical Harmonics library");
		m->declare("ymn.lib/version", "2.0");
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
		fConst0 = double(tgamma(1.5));
		fConst1 = double(tgamma(1.0));
		fConst2 = sqrt((3.0 * (fConst1 / double(tgamma(3.0)))));
		fConst3 = (fConst0 * fConst2);
		fConst4 = (0.39894624032434478 * fConst3);
		fConst5 = (double(tgamma(2.5)) * sqrt((5.0 * (fConst1 / double(tgamma(5.0))))));
		fConst6 = (0.17730759750868944 * fConst2);
		fConst7 = sqrt((5.0 * (double(tgamma(2.0)) / double(tgamma(4.0)))));
		fConst8 = (0.53191448209190839 * fConst7);
		fConst9 = (0.17841434392041153 * fConst3);
		fConst10 = (0.30902256091651653 * fConst3);
		fConst11 = (0.41201936208494599 * fConst7);
		fConst12 = (0.54935914944659459 * fConst5);
		fConst13 = (0.27467957472329729 * fConst5);
		fConst14 = (0.13734193310743828 * fConst3);
		fConst15 = (0.079294439912251033 * fConst3);
		fConst16 = (fConst0 * fConst7);
		fConst17 = (0.41201936208494599 * fConst16);
		fConst18 = (0.17730759750868944 * fConst3);
		fConst19 = (0.13734193310743828 * fConst2);
		fConst20 = (0.29429924039525734 * fConst7);
		fConst21 = (0.70921930945587786 * fConst5);
		fConst22 = (0.45310390476724288 * fConst5);
		fConst23 = (0.16991369833047504 * fConst7);
		fConst24 = (0.53191448209190839 * fConst16);
		fConst25 = (0.19619949359683822 * fConst5);
		fConst26 = (0.39239898719367644 * fConst5);
		fConst27 = (0.29429924039525734 * fConst16);
		fConst28 = (0.16991369833047504 * fConst16);
		fConst29 = (0.22655195238362144 * fConst5);
		fConst30 = (0.35460965472793893 * fConst5);
		fConst31 = (0.31986871235221448 * fConst3);
		fConst32 = (0.14216239476090706 * fConst3);
		fConst33 = (0.26117175201329362 * fConst3);
		fConst34 = (0.34821994335939616 * fConst16);
		fConst35 = (0.46429325781252823 * fConst5);
		fConst36 = (0.11607512793945357 * fConst3);
		fConst37 = (0.082589850671945866 * fConst3);
		fConst38 = (0.44046721241618542 * fConst16);
		fConst39 = (0.036706218836248881 * fConst3);
		fConst40 = (0.073411290721777919 * fConst5);
		fConst41 = (0.14682258144355584 * fConst5);
		fConst42 = (0.20230284584479297 * fConst3);
		fConst43 = (0.089911441543473844 * fConst3);
		fConst44 = (0.26973011046743212 * fConst16);
		fConst45 = (0.28609910994747967 * fConst3);
		fConst46 = (0.44046721241618542 * fConst7);
		fConst47 = (0.127153838554589 * fConst2);
		fConst48 = (0.11607512793945357 * fConst2);
		fConst49 = (0.34821994335939616 * fConst7);
		fConst50 = (0.23214662890626411 * fConst5);
		fConst51 = (0.28432034740638579 * fConst5);
		fConst52 = (0.56864069481277157 * fConst5);
		fConst53 = (0.31787954130087376 * fConst16);
		fConst54 = (0.3398279285754322 * fConst16);
		fConst55 = (0.12014724747147398 * fConst16);
		fConst56 = (0.080098164980982656 * fConst5);
		fConst57 = (0.16019632996196531 * fConst5);
		fConst58 = (0.30395136058729716 * fConst16);
		fConst59 = (0.41620233757211678 * fConst16);
		fConst60 = (0.21191969420058249 * fConst5);
		fConst61 = (0.42383938840116497 * fConst5);
		fConst62 = (0.59939953626387299 * fConst5);
		fConst63 = (0.29969976813193649 * fConst5);
		
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
		ui_interface->declare(&fHslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth", &fHslider2, 0.0, -3.1415926535897931, 3.1415926535897931, 0.10000000000000001);
		ui_interface->declare(&fHslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation", &fHslider1, 0.0, -1.5707963267948966, 1.5707963267948966, 0.10000000000000001);
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
		double fSlow3 = sin(double(fHslider1));
		double fSlow4 = (0.43301700000000004 * fSlow3);
		double fSlow5 = mydsp_faustpower2_f(fSlow3);
		double fSlow6 = (1.0 - fSlow5);
		double fSlow7 = pow(fSlow6, 0.5);
		double fSlow8 = (fConst4 * fSlow7);
		double fSlow9 = double(fHslider2);
		double fSlow10 = sin(fSlow9);
		double fSlow11 = cos(fSlow9);
		double fSlow12 = (fConst5 * fSlow6);
		double fSlow13 = (fSlow11 * fSlow10);
		double fSlow14 = (0.70921930945587786 * fSlow13);
		double fSlow15 = ((2.0 * mydsp_faustpower2_f(fSlow11)) + -1.0);
		double fSlow16 = (0.35460965472793893 * fSlow15);
		double fSlow17 = (fConst0 * fSlow7);
		double fSlow18 = (fSlow3 * fSlow10);
		double fSlow19 = (fConst8 * fSlow18);
		double fSlow20 = (fSlow11 * fSlow3);
		double fSlow21 = (fConst8 * fSlow20);
		double fSlow22 = (0.19245000000000004 * fSlow3);
		double fSlow23 = ((3.0 * fSlow5) + -1.0);
		double fSlow24 = (0.124224 * fSlow23);
		double fSlow25 = (fConst3 * fSlow7);
		double fSlow26 = (0.39894624032434478 * fSlow10);
		double fSlow27 = (0.30902256091651653 * fSlow11);
		double fSlow28 = (0.335413669149 * fSlow3);
		double fSlow29 = (fSlow7 * fSlow10);
		double fSlow30 = (0.0 - (fConst9 * fSlow29));
		double fSlow31 = (fConst10 * fSlow29);
		double fSlow32 = (0.0 - fSlow31);
		double fSlow33 = (fConst11 * fSlow3);
		double fSlow34 = (fSlow11 * fSlow6);
		double fSlow35 = (fConst12 * fSlow34);
		double fSlow36 = (0.055554711935999999 * fSlow23);
		double fSlow37 = (fSlow15 * fSlow6);
		double fSlow38 = (fConst13 * fSlow37);
		double fSlow39 = (0.11112 - (fSlow36 + fSlow38));
		double fSlow40 = (fSlow11 * fSlow7);
		double fSlow41 = (fConst14 * fSlow40);
		double fSlow42 = (0.14907119265000002 * fSlow3);
		double fSlow43 = (0.0 - (fConst15 * fSlow29));
		double fSlow44 = (0.0 - (fConst14 * fSlow29));
		double fSlow45 = (fConst10 * fSlow7);
		double fSlow46 = (fConst17 * fSlow29);
		double fSlow47 = (fConst17 * fSlow40);
		double fSlow48 = ((0.111109299648 * fSlow23) + 0.11112);
		double fSlow49 = (fConst14 * fSlow7);
		double fSlow50 = (0.39894624032434478 * fSlow11);
		double fSlow51 = (0.30902256091651653 * fSlow10);
		double fSlow52 = (0.0 - (fConst9 * fSlow40));
		double fSlow53 = (0.0 - (fConst15 * fSlow40));
		double fSlow54 = (fSlow38 + (0.11112 - fSlow36));
		double fSlow55 = (fConst18 * fSlow7);
		double fSlow56 = (fSlow6 * fSlow10);
		double fSlow57 = (fConst12 * fSlow56);
		double fSlow58 = (fSlow3 * fSlow7);
		double fSlow59 = (fConst17 * fSlow58);
		double fSlow60 = (fConst19 * fSlow10);
		double fSlow61 = (fConst20 * fSlow20);
		double fSlow62 = (fConst20 * fSlow18);
		double fSlow63 = (fConst21 * fSlow56);
		double fSlow64 = (0.079363856447999995 * fSlow23);
		double fSlow65 = (0.11112 - fSlow64);
		double fSlow66 = (fSlow34 * fSlow10);
		double fSlow67 = (0.0 - (fConst22 * fSlow66));
		double fSlow68 = (fConst23 * fSlow18);
		double fSlow69 = (fConst24 * fSlow29);
		double fSlow70 = (0.039681866111999996 * fSlow23);
		double fSlow71 = (fConst25 * fSlow37);
		double fSlow72 = (fSlow70 + (0.11112 - fSlow71));
		double fSlow73 = (fConst26 * fSlow66);
		double fSlow74 = (fSlow58 * fSlow10);
		double fSlow75 = (0.0 - (fConst27 * fSlow74));
		double fSlow76 = (0.38730209625899997 * fSlow3);
		double fSlow77 = (fConst28 * fSlow29);
		double fSlow78 = (fConst28 * fSlow40);
		double fSlow79 = (fSlow64 + 0.11112);
		double fSlow80 = (0.0 - (fConst29 * fSlow37));
		double fSlow81 = (fConst10 * fSlow40);
		double fSlow82 = (fConst8 * fSlow3);
		double fSlow83 = (fConst23 * fSlow20);
		double fSlow84 = ((fSlow70 + fSlow71) + 0.11112);
		double fSlow85 = (fConst0 * fSlow40);
		double fSlow86 = (fConst20 * fSlow3);
		double fSlow87 = (fConst30 * fSlow37);
		double fSlow88 = (fConst31 * fSlow7);
		double fSlow89 = (0.56864069481277157 * fSlow13);
		double fSlow90 = (0.28432034740638579 * fSlow15);
		double fSlow91 = (fConst32 * fSlow7);
		double fSlow92 = (0.28347631111800004 * fSlow3);
		double fSlow93 = (fConst33 * fSlow7);
		double fSlow94 = (fConst34 * fSlow58);
		double fSlow95 = (fConst35 * fSlow56);
		double fSlow96 = (fConst36 * fSlow7);
		double fSlow97 = (fConst34 * fSlow29);
		double fSlow98 = (fConst36 * fSlow29);
		double fSlow99 = (0.0 - (fConst37 * fSlow40));
		double fSlow100 = (0.35857228434300004 * fSlow3);
		double fSlow101 = (fConst3 * fSlow29);
		double fSlow102 = (fConst38 * fSlow29);
		double fSlow103 = (0.0 - (fConst39 * fSlow40));
		double fSlow104 = (0.089085626687999991 * fSlow23);
		double fSlow105 = (fConst40 * fSlow37);
		double fSlow106 = (fSlow104 + fSlow105);
		double fSlow107 = (0.0 - (fConst41 * fSlow66));
		double fSlow108 = (0.0 - (fConst42 * fSlow29));
		double fSlow109 = (0.38032316126999999 * fSlow3);
		double fSlow110 = (0.0 - (fConst42 * fSlow40));
		double fSlow111 = (0.0 - (fConst43 * fSlow29));
		double fSlow112 = (fSlow20 * fSlow7);
		double fSlow113 = (0.0 - (fConst44 * fSlow112));
		double fSlow114 = (0.10910718143999999 * fSlow23);
		double fSlow115 = (0.0 - (fConst44 * fSlow74));
		double fSlow116 = (0.16903075950000004 * fSlow3);
		double fSlow117 = (0.0 - (fConst43 * fSlow40));
		double fSlow118 = (fConst45 * fSlow40);
		double fSlow119 = (0.0 - (fConst37 * fSlow29));
		double fSlow120 = (fConst46 * fSlow3);
		double fSlow121 = (0.0 - (fConst39 * fSlow29));
		double fSlow122 = (fSlow104 - fSlow105);
		double fSlow123 = (0.15936380355000002 * fSlow3);
		double fSlow124 = (0.0 - (fConst33 * fSlow29));
		double fSlow125 = (fConst33 * fSlow40);
		double fSlow126 = (fConst49 * fSlow3);
		double fSlow127 = (0.0 - fSlow98);
		double fSlow128 = (fConst50 * fSlow37);
		double fSlow129 = (0.0 - (fConst34 * fSlow74));
		double fSlow130 = (0.12598816230000001 * fSlow3);
		double fSlow131 = (0.0 - (fConst31 * fSlow29));
		double fSlow132 = (fConst31 * fSlow40);
		double fSlow133 = (0.0 - (fConst32 * fSlow29));
		double fSlow134 = (fConst51 * fSlow37);
		double fSlow135 = (0.0 - (fConst52 * fSlow66));
		double fSlow136 = (fConst32 * fSlow40);
		double fSlow137 = (0.29969976813193649 * fSlow15);
		double fSlow138 = (0.59939953626387299 * fSlow13);
		double fSlow139 = (0.42383938840116497 * fSlow13);
		double fSlow140 = (0.21191969420058249 * fSlow15);
		double fSlow141 = (fConst53 * fSlow58);
		double fSlow142 = (fConst54 * fSlow58);
		double fSlow143 = (fConst26 * fSlow56);
		double fSlow144 = (0.068731027392000002 * fSlow23);
		double fSlow145 = (fConst54 * fSlow74);
		double fSlow146 = (fConst16 * fSlow74);
		double fSlow147 = (0.0 - (fConst55 * fSlow112));
		double fSlow148 = (0.097200435264000004 * fSlow23);
		double fSlow149 = (fConst56 * fSlow37);
		double fSlow150 = (fSlow148 + fSlow149);
		double fSlow151 = (0.0 - (fConst57 * fSlow66));
		double fSlow152 = (0.10131694289093836 * fSlow13);
		double fSlow153 = (0.050658471445469179 * fSlow15);
		double fSlow154 = (0.0 - (fConst58 * fSlow112));
		double fSlow155 = (0.0 - (fConst58 * fSlow74));
		double fSlow156 = (0.10647773203199999 * fSlow23);
		double fSlow157 = (0.0 - (fConst55 * fSlow74));
		double fSlow158 = (fConst59 * fSlow112);
		double fSlow159 = (fSlow148 - fSlow149);
		double fSlow160 = (0.0 - fSlow145);
		double fSlow161 = (fConst54 * fSlow112);
		double fSlow162 = (fConst60 * fSlow37);
		double fSlow163 = (0.0 - (fConst61 * fSlow66));
		double fSlow164 = (0.0 - (fConst53 * fSlow74));
		double fSlow165 = (fConst53 * fSlow112);
		double fSlow166 = (0.0 - (fConst62 * fSlow66));
		double fSlow167 = (fConst63 * fSlow37);
		for (int i = 0; (i < count); i = (i + 1)) {
			double fTemp0 = double(input0[i]);
			double fTemp1 = double(input2[i]);
			double fTemp2 = double(input1[i]);
			double fTemp3 = double(input3[i]);
			double fTemp4 = ((fSlow10 * fTemp2) + (fSlow11 * fTemp3));
			double fTemp5 = double(input4[i]);
			double fTemp6 = double(input8[i]);
			double fTemp7 = double(input5[i]);
			double fTemp8 = double(input7[i]);
			double fTemp9 = double(input6[i]);
			output0[i] = FAUSTFLOAT((iSlow1?((0.11112 * fTemp0) + ((fSlow12 * ((fSlow14 * fTemp5) + (fSlow16 * fTemp6))) + ((fSlow17 * (((fConst6 * fTemp4) + (fSlow19 * fTemp7)) + (fSlow21 * fTemp8))) + ((fSlow22 * fTemp1) + (fSlow24 * fTemp9))))):(iSlow2?((0.24993000000000001 * fTemp0) + ((fSlow4 * fTemp1) + (fSlow8 * fTemp4))):fTemp0)));
			output1[i] = FAUSTFLOAT((iSlow1?((((fSlow10 * ((fSlow17 * ((fConst6 * fTemp0) + (fSlow33 * fTemp1))) + (fSlow35 * fTemp3))) + ((fSlow39 * fTemp2) + ((fSlow41 * fTemp5) + (fSlow42 * fTemp7)))) + (fSlow43 * fTemp9)) + (fSlow44 * fTemp6)):(iSlow2?((0.24993000000000001 * fTemp2) + ((((fSlow25 * ((fSlow26 * fTemp0) + (fSlow27 * fTemp5))) + (fSlow28 * fTemp7)) + (fSlow30 * fTemp9)) + (fSlow32 * fTemp6))):fTemp2)));
			double fTemp10 = ((fSlow10 * fTemp7) + (fSlow11 * fTemp8));
			output2[i] = FAUSTFLOAT((iSlow1?((fSlow3 * ((((fSlow46 * fTemp2) + (0.19245000000000004 * fTemp0)) + (fSlow47 * fTemp3)) + (0.17213247615000002 * fTemp9))) + ((fSlow48 * fTemp1) + (fSlow49 * fTemp10))):(iSlow2?((0.24993000000000001 * fTemp1) + ((fSlow3 * ((0.43301700000000004 * fTemp0) + (0.38730209625899997 * fTemp9))) + (fSlow45 * fTemp10))):fTemp1)));
			output3[i] = FAUSTFLOAT((iSlow1?((fSlow49 * ((fSlow10 * fTemp5) + (fSlow11 * fTemp6))) + ((fSlow53 * fTemp9) + ((fSlow54 * fTemp3) + ((fSlow11 * (((fSlow55 * fTemp0) + (fSlow57 * fTemp2)) + (fSlow59 * fTemp1))) + (fSlow42 * fTemp8))))):(iSlow2?((0.24993000000000001 * fTemp3) + ((fSlow25 * (((fSlow50 * fTemp0) + (fSlow51 * fTemp5)) + (fSlow27 * fTemp6))) + ((fSlow52 * fTemp9) + (fSlow28 * fTemp8)))):fTemp3)));
			output4[i] = FAUSTFLOAT((iSlow1?((fSlow17 * (((fSlow60 * fTemp3) + (fSlow61 * fTemp7)) + (fSlow62 * fTemp8))) + (((fSlow11 * ((fSlow49 * fTemp2) + (fSlow63 * fTemp0))) + (fSlow65 * fTemp5)) + (fSlow67 * fTemp9))):(iSlow2?((fSlow45 * ((fSlow11 * fTemp2) + (fSlow10 * fTemp3))) + (0.24993000000000001 * fTemp5)):fTemp5)));
			output5[i] = FAUSTFLOAT((iSlow1?((((fSlow17 * (((fSlow60 * fTemp1) + (fSlow61 * fTemp5)) + (fSlow68 * fTemp9))) + ((fSlow3 * ((fSlow69 * fTemp0) + (0.14907119265000002 * fTemp2))) + (fSlow72 * fTemp7))) + (fSlow73 * fTemp8)) + (fSlow75 * fTemp6)):(iSlow2?(((fSlow28 * fTemp2) + (fSlow31 * fTemp1)) + (0.24993000000000001 * fTemp7)):fTemp7)));
			output6[i] = FAUSTFLOAT((iSlow1?(((fSlow3 * (((fSlow77 * fTemp7) + (0.17213247615000002 * fTemp1)) + (fSlow78 * fTemp8))) + (((fSlow67 * fTemp5) + (((fSlow24 * fTemp0) + (fSlow43 * fTemp2)) + (fSlow53 * fTemp3))) + (fSlow79 * fTemp9))) + (fSlow80 * fTemp6)):(iSlow2?((((fSlow30 * fTemp2) + (fSlow76 * fTemp1)) + (fSlow52 * fTemp3)) + (0.24993000000000001 * fTemp9)):fTemp9)));
			output7[i] = FAUSTFLOAT((iSlow1?((fSlow17 * ((((fSlow11 * ((fConst19 * fTemp1) + (fSlow82 * fTemp0))) + (fSlow62 * fTemp5)) + (fSlow83 * fTemp9)) + (fSlow61 * fTemp6))) + (((fSlow42 * fTemp3) + (fSlow73 * fTemp7)) + (fSlow84 * fTemp8))):(iSlow2?(((fSlow81 * fTemp1) + (fSlow28 * fTemp3)) + (0.24993000000000001 * fTemp8)):fTemp8)));
			output8[i] = FAUSTFLOAT((iSlow1?(((fSlow85 * ((fConst19 * fTemp3) + (fSlow86 * fTemp8))) + ((fSlow80 * fTemp9) + (((fSlow87 * fTemp0) + (fSlow44 * fTemp2)) + (fSlow75 * fTemp7)))) + (fSlow65 * fTemp6)):(iSlow2?(((fSlow32 * fTemp2) + (fSlow81 * fTemp3)) + (0.24993000000000001 * fTemp6)):fTemp6)));
			double fTemp11 = ((fSlow11 * fTemp5) + (fSlow10 * fTemp6));
			output9[i] = FAUSTFLOAT((iSlow1?((fSlow12 * ((fSlow89 * fTemp3) + (fSlow90 * fTemp2))) + (fSlow91 * fTemp11)):(iSlow2?(fSlow88 * fTemp11):0.0)));
			output10[i] = FAUSTFLOAT((iSlow1?((fSlow11 * (((fSlow94 * fTemp2) + (fSlow95 * fTemp1)) + (fSlow96 * fTemp7))) + ((fSlow3 * ((fSlow97 * fTemp3) + (0.12598816230000001 * fTemp5))) + (fSlow98 * fTemp8))):(iSlow2?((fSlow92 * fTemp5) + (fSlow93 * ((fSlow11 * fTemp7) + (fSlow10 * fTemp8)))):0.0)));
			output11[i] = FAUSTFLOAT((iSlow1?(((fSlow3 * ((fSlow102 * fTemp1) + (0.15936380355000002 * fTemp7))) + ((fSlow103 * fTemp5) + ((fSlow106 * fTemp2) + (fSlow107 * fTemp3)))) + (fSlow101 * ((0.127153838554589 * fTemp9) + (0.036706218836248881 * fTemp6)))):(iSlow2?(((fSlow99 * fTemp5) + (fSlow100 * fTemp7)) + (fSlow101 * ((0.28609910994747967 * fTemp9) + (0.082589850671945866 * fTemp6)))):0.0)));
			output12[i] = FAUSTFLOAT((iSlow1?(((fSlow111 * fTemp7) + ((fSlow113 * fTemp3) + ((fSlow114 * fTemp1) + ((fSlow115 * fTemp2) + (fSlow116 * fTemp9))))) + (fSlow117 * fTemp8)):(iSlow2?(((fSlow108 * fTemp7) + (fSlow109 * fTemp9)) + (fSlow110 * fTemp8)):0.0)));
			output13[i] = FAUSTFLOAT((iSlow1?(((fSlow85 * ((fSlow120 * fTemp1) + (fConst47 * fTemp9))) + ((fSlow121 * fTemp5) + ((fSlow122 * fTemp3) + ((fSlow107 * fTemp2) + (fSlow123 * fTemp8))))) + (fSlow103 * fTemp6)):(iSlow2?(((fSlow118 * fTemp9) + ((fSlow119 * fTemp5) + (fSlow100 * fTemp8))) + (fSlow99 * fTemp6)):0.0)));
			output14[i] = FAUSTFLOAT((iSlow1?((fSlow85 * ((fConst48 * fTemp8) + (fSlow126 * fTemp3))) + ((fSlow127 * fTemp7) + ((fSlow128 * fTemp1) + ((fSlow129 * fTemp2) + (fSlow130 * fTemp6))))):(iSlow2?(((fSlow124 * fTemp7) + (fSlow125 * fTemp8)) + (fSlow92 * fTemp6)):0.0)));
			output15[i] = FAUSTFLOAT((iSlow1?((fSlow133 * fTemp5) + ((fSlow134 * fTemp3) + ((fSlow135 * fTemp2) + (fSlow136 * fTemp6)))):(iSlow2?((fSlow131 * fTemp5) + (fSlow132 * fTemp6)):0.0)));
			output16[i] = FAUSTFLOAT((iSlow1?(fSlow12 * ((fSlow137 * fTemp5) + (fSlow138 * fTemp6))):0.0));
			output17[i] = FAUSTFLOAT((iSlow1?((fSlow12 * ((fSlow139 * fTemp8) + (fSlow140 * fTemp7))) + (fSlow141 * fTemp11)):0.0));
			output18[i] = FAUSTFLOAT((iSlow1?((fSlow11 * ((fSlow142 * fTemp7) + (fSlow143 * fTemp9))) + ((fSlow144 * fTemp5) + (fSlow145 * fTemp8))):0.0));
			output19[i] = FAUSTFLOAT((iSlow1?((fSlow146 * ((0.41620233757211678 * fTemp9) + (0.12014724747147398 * fTemp6))) + (((fSlow147 * fTemp5) + (fSlow150 * fTemp7)) + (fSlow151 * fTemp8))):0.0));
			output20[i] = FAUSTFLOAT((iSlow1?((fSlow12 * ((fSlow152 * fTemp5) + (fSlow153 * fTemp6))) + ((fSlow154 * fTemp8) + ((fSlow155 * fTemp7) + (fSlow156 * fTemp9)))):0.0));
			output21[i] = FAUSTFLOAT((iSlow1?((((fSlow151 * fTemp7) + ((fSlow157 * fTemp5) + (fSlow158 * fTemp9))) + (fSlow159 * fTemp8)) + (fSlow147 * fTemp6)):0.0));
			output22[i] = FAUSTFLOAT((iSlow1?(((fSlow71 * fTemp9) + ((fSlow160 * fTemp7) + (fSlow161 * fTemp8))) + (fSlow144 * fTemp6)):0.0));
			output23[i] = FAUSTFLOAT((iSlow1?((fSlow162 * fTemp8) + ((fSlow163 * fTemp7) + ((fSlow164 * fTemp5) + (fSlow165 * fTemp6)))):0.0));
			output24[i] = FAUSTFLOAT((iSlow1?((fSlow166 * fTemp5) + (fSlow167 * fTemp6)):0.0));
			
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
