/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2016"
license: "GPL"
name: "HOABeamHCardio2HOA2"
version: "1.0"
Code generated with Faust 2.5.23 (https://faust.grame.fr)
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
	
	FAUSTFLOAT fCheckbox0;
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
		m->declare("filename", "HOABeamHCardio2HOA2");
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
		fCheckbox0 = FAUSTFLOAT(0.0);
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
		ui_interface->addCheckButton("Int/Float", &fCheckbox0);
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Order", &fHslider0, 0.0, 0.0, 2.0, 0.0001);
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
		double fSlow0 = double(fHslider0);
		double fSlow1 = (int(double(fCheckbox0))?fSlow0:double(int(fSlow0)));
		double fSlow2 = max(0.0, (1.0 - fabs(fSlow1)));
		double fSlow3 = sin(double(fHslider1));
		double fSlow4 = (0.43301700000000004 * fSlow3);
		double fSlow5 = mydsp_faustpower2_f(fSlow3);
		double fSlow6 = (1.0 - fSlow5);
		double fSlow7 = pow(fSlow6, 0.5);
		double fSlow8 = (fConst4 * fSlow7);
		double fSlow9 = double(fHslider2);
		double fSlow10 = sin(fSlow9);
		double fSlow11 = cos(fSlow9);
		double fSlow12 = max(0.0, (1.0 - fabs((fSlow1 + -1.0))));
		double fSlow13 = (fConst5 * fSlow6);
		double fSlow14 = (fSlow11 * fSlow10);
		double fSlow15 = (0.70921930945587786 * fSlow14);
		double fSlow16 = ((2.0 * mydsp_faustpower2_f(fSlow11)) + -1.0);
		double fSlow17 = (0.35460965472793893 * fSlow16);
		double fSlow18 = (fConst0 * fSlow7);
		double fSlow19 = (fSlow3 * fSlow10);
		double fSlow20 = (fConst8 * fSlow19);
		double fSlow21 = (fSlow11 * fSlow3);
		double fSlow22 = (fConst8 * fSlow21);
		double fSlow23 = (0.19245000000000004 * fSlow3);
		double fSlow24 = ((3.0 * fSlow5) + -1.0);
		double fSlow25 = (0.124224 * fSlow24);
		double fSlow26 = max(0.0, (1.0 - fabs((fSlow1 + -2.0))));
		double fSlow27 = (fConst3 * fSlow7);
		double fSlow28 = (0.39894624032434478 * fSlow10);
		double fSlow29 = (0.30902256091651653 * fSlow11);
		double fSlow30 = (0.335413669149 * fSlow3);
		double fSlow31 = (fSlow7 * fSlow10);
		double fSlow32 = (0.0 - (fConst9 * fSlow31));
		double fSlow33 = (fConst10 * fSlow31);
		double fSlow34 = (0.0 - fSlow33);
		double fSlow35 = (fConst11 * fSlow3);
		double fSlow36 = (fSlow11 * fSlow6);
		double fSlow37 = (fConst12 * fSlow36);
		double fSlow38 = (0.055554711935999999 * fSlow24);
		double fSlow39 = (fSlow16 * fSlow6);
		double fSlow40 = (fConst13 * fSlow39);
		double fSlow41 = (0.11112 - (fSlow38 + fSlow40));
		double fSlow42 = (fSlow11 * fSlow7);
		double fSlow43 = (fConst14 * fSlow42);
		double fSlow44 = (0.14907119265000002 * fSlow3);
		double fSlow45 = (0.0 - (fConst15 * fSlow31));
		double fSlow46 = (0.0 - (fConst14 * fSlow31));
		double fSlow47 = (fConst10 * fSlow7);
		double fSlow48 = (fConst17 * fSlow31);
		double fSlow49 = (fConst17 * fSlow42);
		double fSlow50 = ((0.111109299648 * fSlow24) + 0.11112);
		double fSlow51 = (fConst14 * fSlow7);
		double fSlow52 = (0.39894624032434478 * fSlow11);
		double fSlow53 = (0.30902256091651653 * fSlow10);
		double fSlow54 = (0.0 - (fConst9 * fSlow42));
		double fSlow55 = (0.0 - (fConst15 * fSlow42));
		double fSlow56 = (fSlow40 + (0.11112 - fSlow38));
		double fSlow57 = (fConst18 * fSlow7);
		double fSlow58 = (fSlow6 * fSlow10);
		double fSlow59 = (fConst12 * fSlow58);
		double fSlow60 = (fSlow3 * fSlow7);
		double fSlow61 = (fConst17 * fSlow60);
		double fSlow62 = (fConst19 * fSlow10);
		double fSlow63 = (fConst20 * fSlow21);
		double fSlow64 = (fConst20 * fSlow19);
		double fSlow65 = (fConst21 * fSlow58);
		double fSlow66 = (0.079363856447999995 * fSlow24);
		double fSlow67 = (0.11112 - fSlow66);
		double fSlow68 = (fSlow36 * fSlow10);
		double fSlow69 = (0.0 - (fConst22 * fSlow68));
		double fSlow70 = (fConst23 * fSlow19);
		double fSlow71 = (fConst24 * fSlow31);
		double fSlow72 = (0.039681866111999996 * fSlow24);
		double fSlow73 = (fConst25 * fSlow39);
		double fSlow74 = (fSlow72 + (0.11112 - fSlow73));
		double fSlow75 = (fConst26 * fSlow68);
		double fSlow76 = (fSlow60 * fSlow10);
		double fSlow77 = (0.0 - (fConst27 * fSlow76));
		double fSlow78 = (0.38730209625899997 * fSlow3);
		double fSlow79 = (fConst28 * fSlow31);
		double fSlow80 = (fConst28 * fSlow42);
		double fSlow81 = (fSlow66 + 0.11112);
		double fSlow82 = (0.0 - (fConst29 * fSlow39));
		double fSlow83 = (fConst10 * fSlow42);
		double fSlow84 = (fConst8 * fSlow3);
		double fSlow85 = (fConst23 * fSlow21);
		double fSlow86 = ((fSlow72 + fSlow73) + 0.11112);
		double fSlow87 = (fConst0 * fSlow42);
		double fSlow88 = (fConst20 * fSlow3);
		double fSlow89 = (fConst30 * fSlow39);
		double fSlow90 = (fConst31 * fSlow7);
		double fSlow91 = (0.56864069481277157 * fSlow14);
		double fSlow92 = (0.28432034740638579 * fSlow16);
		double fSlow93 = (fConst32 * fSlow7);
		double fSlow94 = (0.28347631111800004 * fSlow3);
		double fSlow95 = (fConst33 * fSlow7);
		double fSlow96 = (fConst34 * fSlow60);
		double fSlow97 = (fConst35 * fSlow58);
		double fSlow98 = (fConst36 * fSlow7);
		double fSlow99 = (fConst34 * fSlow31);
		double fSlow100 = (fConst36 * fSlow31);
		double fSlow101 = (0.0 - (fConst37 * fSlow42));
		double fSlow102 = (0.35857228434300004 * fSlow3);
		double fSlow103 = (fConst3 * fSlow31);
		double fSlow104 = (fConst38 * fSlow31);
		double fSlow105 = (0.0 - (fConst39 * fSlow42));
		double fSlow106 = (0.089085626687999991 * fSlow24);
		double fSlow107 = (fConst40 * fSlow39);
		double fSlow108 = (fSlow106 + fSlow107);
		double fSlow109 = (0.0 - (fConst41 * fSlow68));
		double fSlow110 = (0.0 - (fConst42 * fSlow31));
		double fSlow111 = (0.38032316126999999 * fSlow3);
		double fSlow112 = (0.0 - (fConst42 * fSlow42));
		double fSlow113 = (0.0 - (fConst43 * fSlow31));
		double fSlow114 = (fSlow21 * fSlow7);
		double fSlow115 = (0.0 - (fConst44 * fSlow114));
		double fSlow116 = (0.10910718143999999 * fSlow24);
		double fSlow117 = (0.0 - (fConst44 * fSlow76));
		double fSlow118 = (0.16903075950000004 * fSlow3);
		double fSlow119 = (0.0 - (fConst43 * fSlow42));
		double fSlow120 = (fConst45 * fSlow42);
		double fSlow121 = (0.0 - (fConst37 * fSlow31));
		double fSlow122 = (fConst46 * fSlow3);
		double fSlow123 = (0.0 - (fConst39 * fSlow31));
		double fSlow124 = (fSlow106 - fSlow107);
		double fSlow125 = (0.15936380355000002 * fSlow3);
		double fSlow126 = (0.0 - (fConst33 * fSlow31));
		double fSlow127 = (fConst33 * fSlow42);
		double fSlow128 = (fConst49 * fSlow3);
		double fSlow129 = (0.0 - fSlow100);
		double fSlow130 = (fConst50 * fSlow39);
		double fSlow131 = (0.0 - (fConst34 * fSlow76));
		double fSlow132 = (0.12598816230000001 * fSlow3);
		double fSlow133 = (0.0 - (fConst31 * fSlow31));
		double fSlow134 = (fConst31 * fSlow42);
		double fSlow135 = (0.0 - (fConst32 * fSlow31));
		double fSlow136 = (fConst51 * fSlow39);
		double fSlow137 = (0.0 - (fConst52 * fSlow68));
		double fSlow138 = (fConst32 * fSlow42);
		double fSlow139 = (0.29969976813193649 * fSlow16);
		double fSlow140 = (0.59939953626387299 * fSlow14);
		double fSlow141 = (0.42383938840116497 * fSlow14);
		double fSlow142 = (0.21191969420058249 * fSlow16);
		double fSlow143 = (fConst53 * fSlow60);
		double fSlow144 = (fConst54 * fSlow60);
		double fSlow145 = (fConst26 * fSlow58);
		double fSlow146 = (0.068731027392000002 * fSlow24);
		double fSlow147 = (fConst54 * fSlow76);
		double fSlow148 = (fConst16 * fSlow76);
		double fSlow149 = (0.0 - (fConst55 * fSlow114));
		double fSlow150 = (0.097200435264000004 * fSlow24);
		double fSlow151 = (fConst56 * fSlow39);
		double fSlow152 = (fSlow150 + fSlow151);
		double fSlow153 = (0.0 - (fConst57 * fSlow68));
		double fSlow154 = (0.10131694289093836 * fSlow14);
		double fSlow155 = (0.050658471445469179 * fSlow16);
		double fSlow156 = (0.0 - (fConst58 * fSlow114));
		double fSlow157 = (0.0 - (fConst58 * fSlow76));
		double fSlow158 = (0.10647773203199999 * fSlow24);
		double fSlow159 = (0.0 - (fConst55 * fSlow76));
		double fSlow160 = (fConst59 * fSlow114);
		double fSlow161 = (fSlow150 - fSlow151);
		double fSlow162 = (0.0 - fSlow147);
		double fSlow163 = (fConst54 * fSlow114);
		double fSlow164 = (fConst60 * fSlow39);
		double fSlow165 = (0.0 - (fConst61 * fSlow68));
		double fSlow166 = (0.0 - (fConst53 * fSlow76));
		double fSlow167 = (fConst53 * fSlow114);
		double fSlow168 = (0.0 - (fConst62 * fSlow68));
		double fSlow169 = (fConst63 * fSlow39);
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
			output0[i] = FAUSTFLOAT((((fSlow2 * fTemp0) + (((0.24993000000000001 * fTemp0) + ((fSlow4 * fTemp1) + (fSlow8 * fTemp4))) * fSlow12)) + (((0.11112 * fTemp0) + ((fSlow13 * ((fSlow15 * fTemp5) + (fSlow17 * fTemp6))) + ((fSlow18 * (((fConst6 * fTemp4) + (fSlow20 * fTemp7)) + (fSlow22 * fTemp8))) + ((fSlow23 * fTemp1) + (fSlow25 * fTemp9))))) * fSlow26)));
			output1[i] = FAUSTFLOAT((((fSlow2 * fTemp2) + (fSlow12 * ((0.24993000000000001 * fTemp2) + ((((fSlow27 * ((fSlow28 * fTemp0) + (fSlow29 * fTemp5))) + (fSlow30 * fTemp7)) + (fSlow32 * fTemp9)) + (fSlow34 * fTemp6))))) + (fSlow26 * ((((fSlow10 * ((fSlow18 * ((fConst6 * fTemp0) + (fSlow35 * fTemp1))) + (fSlow37 * fTemp3))) + ((fSlow41 * fTemp2) + ((fSlow43 * fTemp5) + (fSlow44 * fTemp7)))) + (fSlow45 * fTemp9)) + (fSlow46 * fTemp6)))));
			double fTemp10 = ((fSlow10 * fTemp7) + (fSlow11 * fTemp8));
			output2[i] = FAUSTFLOAT((((fSlow2 * fTemp1) + (fSlow12 * ((0.24993000000000001 * fTemp1) + ((fSlow3 * ((0.43301700000000004 * fTemp0) + (0.38730209625899997 * fTemp9))) + (fSlow47 * fTemp10))))) + (fSlow26 * ((fSlow3 * ((((fSlow48 * fTemp2) + (0.19245000000000004 * fTemp0)) + (fSlow49 * fTemp3)) + (0.17213247615000002 * fTemp9))) + ((fSlow50 * fTemp1) + (fSlow51 * fTemp10))))));
			output3[i] = FAUSTFLOAT((((fSlow2 * fTemp3) + (fSlow12 * ((0.24993000000000001 * fTemp3) + ((fSlow27 * (((fSlow52 * fTemp0) + (fSlow53 * fTemp5)) + (fSlow29 * fTemp6))) + ((fSlow54 * fTemp9) + (fSlow30 * fTemp8)))))) + (fSlow26 * ((fSlow51 * ((fSlow10 * fTemp5) + (fSlow11 * fTemp6))) + ((fSlow55 * fTemp9) + ((fSlow56 * fTemp3) + ((fSlow11 * (((fSlow57 * fTemp0) + (fSlow59 * fTemp2)) + (fSlow61 * fTemp1))) + (fSlow44 * fTemp8))))))));
			output4[i] = FAUSTFLOAT((((fSlow2 * fTemp5) + (fSlow12 * ((fSlow47 * ((fSlow11 * fTemp2) + (fSlow10 * fTemp3))) + (0.24993000000000001 * fTemp5)))) + (fSlow26 * ((fSlow18 * (((fSlow62 * fTemp3) + (fSlow63 * fTemp7)) + (fSlow64 * fTemp8))) + (((fSlow11 * ((fSlow51 * fTemp2) + (fSlow65 * fTemp0))) + (fSlow67 * fTemp5)) + (fSlow69 * fTemp9))))));
			output5[i] = FAUSTFLOAT((((fSlow2 * fTemp7) + (fSlow12 * (((fSlow30 * fTemp2) + (fSlow33 * fTemp1)) + (0.24993000000000001 * fTemp7)))) + (fSlow26 * ((((fSlow18 * (((fSlow62 * fTemp1) + (fSlow63 * fTemp5)) + (fSlow70 * fTemp9))) + ((fSlow3 * ((fSlow71 * fTemp0) + (0.14907119265000002 * fTemp2))) + (fSlow74 * fTemp7))) + (fSlow75 * fTemp8)) + (fSlow77 * fTemp6)))));
			output6[i] = FAUSTFLOAT((((fSlow2 * fTemp9) + (fSlow12 * ((((fSlow32 * fTemp2) + (fSlow78 * fTemp1)) + (fSlow54 * fTemp3)) + (0.24993000000000001 * fTemp9)))) + (fSlow26 * (((fSlow3 * (((fSlow79 * fTemp7) + (0.17213247615000002 * fTemp1)) + (fSlow80 * fTemp8))) + (((fSlow69 * fTemp5) + (((fSlow25 * fTemp0) + (fSlow45 * fTemp2)) + (fSlow55 * fTemp3))) + (fSlow81 * fTemp9))) + (fSlow82 * fTemp6)))));
			output7[i] = FAUSTFLOAT((((fSlow2 * fTemp8) + (fSlow12 * (((fSlow83 * fTemp1) + (fSlow30 * fTemp3)) + (0.24993000000000001 * fTemp8)))) + (fSlow26 * ((fSlow18 * ((((fSlow11 * ((fConst19 * fTemp1) + (fSlow84 * fTemp0))) + (fSlow64 * fTemp5)) + (fSlow85 * fTemp9)) + (fSlow63 * fTemp6))) + (((fSlow44 * fTemp3) + (fSlow75 * fTemp7)) + (fSlow86 * fTemp8))))));
			output8[i] = FAUSTFLOAT((((fSlow2 * fTemp6) + (fSlow12 * (((fSlow34 * fTemp2) + (fSlow83 * fTemp3)) + (0.24993000000000001 * fTemp6)))) + (fSlow26 * (((fSlow87 * ((fConst19 * fTemp3) + (fSlow88 * fTemp8))) + ((fSlow82 * fTemp9) + (((fSlow89 * fTemp0) + (fSlow46 * fTemp2)) + (fSlow77 * fTemp7)))) + (fSlow67 * fTemp6)))));
			double fTemp11 = ((fSlow11 * fTemp5) + (fSlow10 * fTemp6));
			output9[i] = FAUSTFLOAT(((fSlow90 * (fSlow12 * fTemp11)) + (fSlow26 * ((fSlow13 * ((fSlow91 * fTemp3) + (fSlow92 * fTemp2))) + (fSlow93 * fTemp11)))));
			output10[i] = FAUSTFLOAT(((fSlow12 * ((fSlow94 * fTemp5) + (fSlow95 * ((fSlow11 * fTemp7) + (fSlow10 * fTemp8))))) + (fSlow26 * ((fSlow11 * (((fSlow96 * fTemp2) + (fSlow97 * fTemp1)) + (fSlow98 * fTemp7))) + ((fSlow3 * ((fSlow99 * fTemp3) + (0.12598816230000001 * fTemp5))) + (fSlow100 * fTemp8))))));
			output11[i] = FAUSTFLOAT(((fSlow12 * (((fSlow101 * fTemp5) + (fSlow102 * fTemp7)) + (fSlow103 * ((0.082589850671945866 * fTemp6) + (0.28609910994747967 * fTemp9))))) + (fSlow26 * (((fSlow3 * ((fSlow104 * fTemp1) + (0.15936380355000002 * fTemp7))) + ((fSlow105 * fTemp5) + ((fSlow108 * fTemp2) + (fSlow109 * fTemp3)))) + (fSlow103 * ((0.036706218836248881 * fTemp6) + (0.127153838554589 * fTemp9)))))));
			output12[i] = FAUSTFLOAT(((fSlow12 * (((fSlow110 * fTemp7) + (fSlow111 * fTemp9)) + (fSlow112 * fTemp8))) + (fSlow26 * (((fSlow113 * fTemp7) + ((fSlow115 * fTemp3) + ((fSlow116 * fTemp1) + ((fSlow117 * fTemp2) + (fSlow118 * fTemp9))))) + (fSlow119 * fTemp8)))));
			output13[i] = FAUSTFLOAT(((fSlow12 * (((fSlow120 * fTemp9) + ((fSlow121 * fTemp5) + (fSlow102 * fTemp8))) + (fSlow101 * fTemp6))) + (fSlow26 * (((fSlow87 * ((fSlow122 * fTemp1) + (fConst47 * fTemp9))) + ((fSlow123 * fTemp5) + ((fSlow124 * fTemp3) + ((fSlow109 * fTemp2) + (fSlow125 * fTemp8))))) + (fSlow105 * fTemp6)))));
			output14[i] = FAUSTFLOAT(((fSlow12 * (((fSlow126 * fTemp7) + (fSlow127 * fTemp8)) + (fSlow94 * fTemp6))) + (fSlow26 * ((fSlow87 * ((fConst48 * fTemp8) + (fSlow128 * fTemp3))) + ((fSlow129 * fTemp7) + ((fSlow130 * fTemp1) + ((fSlow131 * fTemp2) + (fSlow132 * fTemp6))))))));
			output15[i] = FAUSTFLOAT(((fSlow12 * ((fSlow133 * fTemp5) + (fSlow134 * fTemp6))) + (fSlow26 * ((fSlow135 * fTemp5) + ((fSlow136 * fTemp3) + ((fSlow137 * fTemp2) + (fSlow138 * fTemp6)))))));
			output16[i] = FAUSTFLOAT((fSlow13 * (fSlow26 * ((fSlow139 * fTemp5) + (fSlow140 * fTemp6)))));
			output17[i] = FAUSTFLOAT((fSlow26 * ((fSlow13 * ((fSlow141 * fTemp8) + (fSlow142 * fTemp7))) + (fSlow143 * fTemp11))));
			output18[i] = FAUSTFLOAT((fSlow26 * ((fSlow11 * ((fSlow144 * fTemp7) + (fSlow145 * fTemp9))) + ((fSlow146 * fTemp5) + (fSlow147 * fTemp8)))));
			output19[i] = FAUSTFLOAT((fSlow26 * ((fSlow148 * ((0.12014724747147398 * fTemp6) + (0.41620233757211678 * fTemp9))) + (((fSlow149 * fTemp5) + (fSlow152 * fTemp7)) + (fSlow153 * fTemp8)))));
			output20[i] = FAUSTFLOAT((fSlow26 * ((fSlow13 * ((fSlow154 * fTemp5) + (fSlow155 * fTemp6))) + ((fSlow156 * fTemp8) + ((fSlow157 * fTemp7) + (fSlow158 * fTemp9))))));
			output21[i] = FAUSTFLOAT((fSlow26 * ((((fSlow153 * fTemp7) + ((fSlow159 * fTemp5) + (fSlow160 * fTemp9))) + (fSlow161 * fTemp8)) + (fSlow149 * fTemp6))));
			output22[i] = FAUSTFLOAT((fSlow26 * (((fSlow73 * fTemp9) + ((fSlow162 * fTemp7) + (fSlow163 * fTemp8))) + (fSlow146 * fTemp6))));
			output23[i] = FAUSTFLOAT((fSlow26 * ((fSlow164 * fTemp8) + ((fSlow165 * fTemp7) + ((fSlow166 * fTemp5) + (fSlow167 * fTemp6))))));
			output24[i] = FAUSTFLOAT((fSlow26 * ((fSlow168 * fTemp5) + (fSlow169 * fTemp6))));
			
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
