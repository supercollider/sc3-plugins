/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2016"
license: "GPL"
name: "HOABeamHCardio2HOA3"
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
	double fConst64;
	double fConst65;
	double fConst66;
	double fConst67;
	double fConst68;
	double fConst69;
	double fConst70;
	double fConst71;
	double fConst72;
	double fConst73;
	double fConst74;
	double fConst75;
	double fConst76;
	double fConst77;
	double fConst78;
	double fConst79;
	double fConst80;
	double fConst81;
	double fConst82;
	double fConst83;
	double fConst84;
	double fConst85;
	double fConst86;
	double fConst87;
	double fConst88;
	double fConst89;
	double fConst90;
	double fConst91;
	double fConst92;
	double fConst93;
	double fConst94;
	double fConst95;
	double fConst96;
	double fConst97;
	double fConst98;
	double fConst99;
	double fConst100;
	double fConst101;
	double fConst102;
	double fConst103;
	double fConst104;
	double fConst105;
	double fConst106;
	double fConst107;
	double fConst108;
	double fConst109;
	double fConst110;
	double fConst111;
	double fConst112;
	double fConst113;
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
		m->declare("name", "HOABeamHCardio2HOA3");
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
		return 16;
		
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
		fConst11 = (0.13734193310743828 * fConst2);
		fConst12 = (0.34821994335939616 * fConst7);
		fConst13 = (0.28432034740638579 * fConst5);
		fConst14 = (0.13734193310743828 * fConst3);
		fConst15 = (0.079294439912251033 * fConst3);
		fConst16 = (0.41201936208494599 * fConst7);
		fConst17 = (0.54935914944659459 * fConst5);
		fConst18 = (0.27467957472329729 * fConst5);
		fConst19 = (0.073411290721777919 * fConst5);
		fConst20 = (fConst0 * fConst7);
		fConst21 = (0.26973011046743212 * fConst20);
		fConst22 = (0.14682258144355584 * fConst5);
		fConst23 = (0.34821994335939616 * fConst20);
		fConst24 = (0.56864069481277157 * fConst5);
		fConst25 = (0.44046721241618542 * fConst7);
		fConst26 = (0.41201936208494599 * fConst20);
		fConst27 = (0.17730759750868944 * fConst3);
		fConst28 = (0.082589850671945866 * fConst3);
		fConst29 = (0.31986871235221448 * fConst3);
		fConst30 = (0.29429924039525734 * fConst7);
		fConst31 = (0.14216239476090706 * fConst2);
		fConst32 = (0.45310390476724288 * fConst5);
		fConst33 = (0.70921930945587786 * fConst5);
		fConst34 = (0.036706218836248881 * fConst3);
		fConst35 = (0.14216239476090706 * fConst3);
		fConst36 = (0.20230284584479297 * fConst3);
		fConst37 = (0.26117175201329362 * fConst3);
		fConst38 = (0.53191448209190839 * fConst20);
		fConst39 = (0.16991369833047504 * fConst7);
		fConst40 = (0.11607512793945357 * fConst2);
		fConst41 = (0.29429924039525734 * fConst20);
		fConst42 = (0.19619949359683822 * fConst5);
		fConst43 = (0.39239898719367644 * fConst5);
		fConst44 = (0.089911441543473844 * fConst3);
		fConst45 = (0.11607512793945357 * fConst3);
		fConst46 = (0.28609910994747967 * fConst3);
		fConst47 = (0.16991369833047504 * fConst20);
		fConst48 = (0.22655195238362144 * fConst5);
		fConst49 = (0.127153838554589 * fConst3);
		fConst50 = (0.036706218836248881 * fConst2);
		fConst51 = (0.35460965472793893 * fConst5);
		fConst52 = (0.28034340012861197 * fConst7);
		fConst53 = (0.23640620007885615 * fConst5);
		fConst54 = (0.11820310003942808 * fConst5);
		fConst55 = (0.21715302348505744 * fConst7);
		fConst56 = (0.46429325781252823 * fConst5);
		fConst57 = (0.47281310937702176 * fConst5);
		fConst58 = (0.28034340012861197 * fConst20);
		fConst59 = (0.11213767920013405 * fConst7);
		fConst60 = (0.44046721241618542 * fConst20);
		fConst61 = (0.18311971648219821 * fConst5);
		fConst62 = (0.21715302348505744 * fConst20);
		fConst63 = (0.11213767920013405 * fConst20);
		fConst64 = (0.23640655468851088 * fConst5);
		fConst65 = (0.127153838554589 * fConst2);
		fConst66 = (0.36623943296439643 * fConst5);
		fConst67 = (0.23214662890626411 * fConst5);
		fConst68 = (0.32573840838610657 * fConst3);
		fConst69 = (0.1447711214430524 * fConst3);
		fConst70 = (0.28209767915702649 * fConst3);
		fConst71 = (0.31787954130087376 * fConst20);
		fConst72 = (0.12537544335157685 * fConst3);
		fConst73 = (0.061558601720767374 * fConst3);
		fConst74 = (0.3398279285754322 * fConst20);
		fConst75 = (0.10596150257755792 * fConst3);
		fConst76 = (0.027359094218383301 * fConst3);
		fConst77 = (0.10662277113532474 * fConst3);
		fConst78 = (0.12014724747147398 * fConst20);
		fConst79 = (0.080098164980982656 * fConst5);
		fConst80 = (0.16019632996196531 * fConst5);
		fConst81 = (0.047387405817769852 * fConst3);
		fConst82 = (0.21324554227064949 * fConst3);
		fConst83 = (0.094774811635539705 * fConst3);
		fConst84 = (0.30395136058729716 * fConst20);
		fConst85 = (0.27529923627565928 * fConst3);
		fConst86 = (0.41620233757211678 * fConst7);
		fConst87 = (0.12235394458243126 * fConst2);
		fConst88 = (0.23841585846519298 * fConst3);
		fConst89 = (0.3398279285754322 * fConst7);
		fConst90 = (0.10596150257755792 * fConst2);
		fConst91 = (0.12537544335157685 * fConst2);
		fConst92 = (0.31787954130087376 * fConst7);
		fConst93 = (0.21191969420058249 * fConst5);
		fConst94 = (0.42383938840116497 * fConst5);
		fConst95 = (0.59939953626387299 * fConst5);
		fConst96 = (0.29969976813193649 * fConst5);
		fConst97 = (0.29280934793091795 * fConst20);
		fConst98 = (0.33810718522586491 * fConst20);
		fConst99 = (0.084526530349225165 * fConst20);
		fConst100 = (0.3273704914048397 * fConst20);
		fConst101 = (0.35639617816845831 * fConst5);
		fConst102 = (0.021298742387096829 * fConst5);
		fConst103 = (0.15651317678313359 * fConst20);
		fConst104 = (0.082489652492467883 * fConst5);
		fConst105 = (0.16497930498493577 * fConst5);
		fConst106 = (0.042597484774193659 * fConst5);
		fConst107 = (0.31948113580645249 * fConst20);
		fConst108 = (0.40411511288106022 * fConst20);
		fConst109 = (0.17819808908422916 * fConst5);
		fConst110 = (0.4781556584351529 * fConst5);
		fConst111 = (0.23907782921757645 * fConst5);
		fConst112 = (0.6172959763186826 * fConst5);
		fConst113 = (0.3086479881593413 * fConst5);
		
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
		FAUSTFLOAT* input9 = inputs[9];
		FAUSTFLOAT* input10 = inputs[10];
		FAUSTFLOAT* input11 = inputs[11];
		FAUSTFLOAT* input12 = inputs[12];
		FAUSTFLOAT* input13 = inputs[13];
		FAUSTFLOAT* input14 = inputs[14];
		FAUSTFLOAT* input15 = inputs[15];
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
		double fSlow31 = (0.0 - (fConst10 * fSlow29));
		double fSlow32 = (fSlow11 * fSlow7);
		double fSlow33 = (fConst0 * fSlow32);
		double fSlow34 = (fConst12 * fSlow3);
		double fSlow35 = (fSlow15 * fSlow6);
		double fSlow36 = (fConst13 * fSlow35);
		double fSlow37 = (0.0 - (fConst14 * fSlow29));
		double fSlow38 = (0.0 - (fConst15 * fSlow29));
		double fSlow39 = (fConst16 * fSlow3);
		double fSlow40 = (fSlow11 * fSlow6);
		double fSlow41 = (fConst17 * fSlow40);
		double fSlow42 = (0.055554711935999999 * fSlow23);
		double fSlow43 = (fConst18 * fSlow35);
		double fSlow44 = (0.11112 - (fSlow42 + fSlow43));
		double fSlow45 = (0.14907119265000002 * fSlow3);
		double fSlow46 = (0.089085626687999991 * fSlow23);
		double fSlow47 = (fConst19 * fSlow35);
		double fSlow48 = (fSlow46 + fSlow47);
		double fSlow49 = (fSlow3 * fSlow7);
		double fSlow50 = (fSlow49 * fSlow10);
		double fSlow51 = (0.0 - (fConst21 * fSlow50));
		double fSlow52 = (fSlow40 * fSlow10);
		double fSlow53 = (0.0 - (fConst22 * fSlow52));
		double fSlow54 = (0.0 - (fConst23 * fSlow50));
		double fSlow55 = (0.0 - (fConst24 * fSlow52));
		double fSlow56 = (fConst10 * fSlow7);
		double fSlow57 = (0.46429325781252823 * fSlow13);
		double fSlow58 = (0.23214662890626411 * fSlow15);
		double fSlow59 = (fConst25 * fSlow18);
		double fSlow60 = (fConst25 * fSlow20);
		double fSlow61 = ((0.111109299648 * fSlow23) + 0.11112);
		double fSlow62 = (fConst26 * fSlow29);
		double fSlow63 = (fConst26 * fSlow32);
		double fSlow64 = (0.10910718143999999 * fSlow23);
		double fSlow65 = (0.39894624032434478 * fSlow11);
		double fSlow66 = (0.30902256091651653 * fSlow10);
		double fSlow67 = (0.0 - (fConst9 * fSlow32));
		double fSlow68 = (fConst12 * fSlow18);
		double fSlow69 = (fConst12 * fSlow20);
		double fSlow70 = (fSlow46 - fSlow47);
		double fSlow71 = (fSlow20 * fSlow7);
		double fSlow72 = (0.0 - (fConst21 * fSlow71));
		double fSlow73 = (fConst27 * fSlow7);
		double fSlow74 = (fSlow6 * fSlow10);
		double fSlow75 = (fConst17 * fSlow74);
		double fSlow76 = (fConst26 * fSlow49);
		double fSlow77 = (fConst24 * fSlow74);
		double fSlow78 = (0.0 - (fConst15 * fSlow32));
		double fSlow79 = (fSlow43 + (0.11112 - fSlow42));
		double fSlow80 = (0.31986871235221448 * fSlow11);
		double fSlow81 = (0.28347631111800004 * fSlow3);
		double fSlow82 = (0.0 - (fConst28 * fSlow32));
		double fSlow83 = (0.0 - (fConst28 * fSlow29));
		double fSlow84 = (0.0 - (fConst29 * fSlow29));
		double fSlow85 = (fConst11 * fSlow10);
		double fSlow86 = (fConst30 * fSlow20);
		double fSlow87 = (fConst30 * fSlow18);
		double fSlow88 = (fConst31 * fSlow11);
		double fSlow89 = (0.0 - (fConst32 * fSlow52));
		double fSlow90 = (0.079363856447999995 * fSlow23);
		double fSlow91 = (0.11112 - fSlow90);
		double fSlow92 = (fConst14 * fSlow7);
		double fSlow93 = (fConst33 * fSlow74);
		double fSlow94 = (0.12598816230000001 * fSlow3);
		double fSlow95 = (0.0 - (fConst34 * fSlow32));
		double fSlow96 = (0.0 - (fConst34 * fSlow29));
		double fSlow97 = (0.0 - (fConst35 * fSlow29));
		double fSlow98 = (0.26117175201329362 * fSlow11);
		double fSlow99 = (0.0 - (fConst36 * fSlow29));
		double fSlow100 = (0.0 - (fConst37 * fSlow29));
		double fSlow101 = (fConst38 * fSlow29);
		double fSlow102 = (fConst39 * fSlow18);
		double fSlow103 = (fConst40 * fSlow11);
		double fSlow104 = (0.0 - (fConst41 * fSlow50));
		double fSlow105 = (0.039681866111999996 * fSlow23);
		double fSlow106 = (fConst42 * fSlow35);
		double fSlow107 = (fSlow105 + (0.11112 - fSlow106));
		double fSlow108 = (fConst43 * fSlow52);
		double fSlow109 = (0.0 - (fConst44 * fSlow29));
		double fSlow110 = (0.0 - (fConst45 * fSlow29));
		double fSlow111 = (fConst46 * fSlow7);
		double fSlow112 = (fConst47 * fSlow29);
		double fSlow113 = (fConst47 * fSlow32);
		double fSlow114 = (0.0 - (fConst48 * fSlow35));
		double fSlow115 = (fSlow90 + 0.11112);
		double fSlow116 = (fConst49 * fSlow7);
		double fSlow117 = (0.26117175201329362 * fSlow10);
		double fSlow118 = (0.0 - (fConst36 * fSlow32));
		double fSlow119 = (fConst8 * fSlow3);
		double fSlow120 = (fConst39 * fSlow20);
		double fSlow121 = (fConst40 * fSlow10);
		double fSlow122 = (0.0 - (fConst44 * fSlow32));
		double fSlow123 = ((fSlow105 + fSlow106) + 0.11112);
		double fSlow124 = (0.31986871235221448 * fSlow10);
		double fSlow125 = (0.082589850671945866 * fSlow10);
		double fSlow126 = (fConst30 * fSlow3);
		double fSlow127 = (fConst31 * fSlow10);
		double fSlow128 = (fConst50 * fSlow10);
		double fSlow129 = (fConst51 * fSlow35);
		double fSlow130 = (fConst29 * fSlow7);
		double fSlow131 = (fConst52 * fSlow20);
		double fSlow132 = (fConst52 * fSlow18);
		double fSlow133 = (0.0 - (fConst53 * fSlow52));
		double fSlow134 = (0.56864069481277157 * fSlow13);
		double fSlow135 = (0.28432034740638579 * fSlow15);
		double fSlow136 = (0.11112 - (0.092591103744000008 * fSlow23));
		double fSlow137 = (0.0 - (fConst54 * fSlow35));
		double fSlow138 = (fConst37 * fSlow7);
		double fSlow139 = (fConst55 * fSlow20);
		double fSlow140 = (fConst55 * fSlow18);
		double fSlow141 = (fConst23 * fSlow29);
		double fSlow142 = (fConst23 * fSlow49);
		double fSlow143 = (fConst56 * fSlow74);
		double fSlow144 = (fConst45 * fSlow7);
		double fSlow145 = (0.0 - (fConst57 * fSlow52));
		double fSlow146 = (0.0 - (fConst58 * fSlow50));
		double fSlow147 = (0.35857228434300004 * fSlow3);
		double fSlow148 = (fConst3 * fSlow29);
		double fSlow149 = (fConst5 * fSlow52);
		double fSlow150 = (fConst2 * fSlow10);
		double fSlow151 = (fConst59 * fSlow18);
		double fSlow152 = (fConst60 * fSlow29);
		double fSlow153 = (fConst61 * fSlow35);
		double fSlow154 = (fSlow42 + (0.11112 - fSlow153));
		double fSlow155 = (0.0 - (fConst62 * fSlow50));
		double fSlow156 = (0.38032316126999999 * fSlow3);
		double fSlow157 = (fConst63 * fSlow29);
		double fSlow158 = (fConst63 * fSlow32);
		double fSlow159 = ((0.074072907839999994 * fSlow23) + 0.11112);
		double fSlow160 = (0.0 - (fConst64 * fSlow35));
		double fSlow161 = (fConst46 * fSlow32);
		double fSlow162 = (fConst25 * fSlow3);
		double fSlow163 = (fConst59 * fSlow20);
		double fSlow164 = (0.15936380355000002 * fSlow3);
		double fSlow165 = (fConst66 * fSlow52);
		double fSlow166 = ((fSlow42 + fSlow153) + 0.11112);
		double fSlow167 = (fConst37 * fSlow32);
		double fSlow168 = (fConst67 * fSlow35);
		double fSlow169 = (fConst29 * fSlow32);
		double fSlow170 = (fConst52 * fSlow3);
		double fSlow171 = (0.23640620007885615 * fSlow13);
		double fSlow172 = (fConst68 * fSlow7);
		double fSlow173 = (0.29969976813193649 * fSlow15);
		double fSlow174 = (0.59939953626387299 * fSlow13);
		double fSlow175 = (fConst69 * fSlow7);
		double fSlow176 = (0.25000236495 * fSlow3);
		double fSlow177 = (fConst70 * fSlow7);
		double fSlow178 = (fConst71 * fSlow7);
		double fSlow179 = (0.42383938840116497 * fSlow13);
		double fSlow180 = (0.21191969420058249 * fSlow15);
		double fSlow181 = (fConst72 * fSlow7);
		double fSlow182 = (0.0 - (fConst73 * fSlow32));
		double fSlow183 = (0.32733010779299998 * fSlow3);
		double fSlow184 = (0.061558601720767374 * fSlow10);
		double fSlow185 = (fConst74 * fSlow49);
		double fSlow186 = (fConst43 * fSlow74);
		double fSlow187 = (fConst75 * fSlow7);
		double fSlow188 = (fConst74 * fSlow29);
		double fSlow189 = (0.068731027392000002 * fSlow23);
		double fSlow190 = (0.0 - (fConst76 * fSlow32));
		double fSlow191 = (0.0 - (fConst77 * fSlow32));
		double fSlow192 = (0.365966049618 * fSlow3);
		double fSlow193 = (fConst20 * fSlow29);
		double fSlow194 = (0.0 - (fConst78 * fSlow71));
		double fSlow195 = (0.097200435264000004 * fSlow23);
		double fSlow196 = (fConst79 * fSlow35);
		double fSlow197 = (fSlow195 + fSlow196);
		double fSlow198 = (0.0 - (fConst80 * fSlow52));
		double fSlow199 = (0.0 - (fConst81 * fSlow32));
		double fSlow200 = (0.0 - (fConst82 * fSlow29));
		double fSlow201 = (0.37796841482400001 * fSlow3);
		double fSlow202 = (0.0 - (fConst82 * fSlow32));
		double fSlow203 = (0.0 - (fConst83 * fSlow29));
		double fSlow204 = (0.10131694289093836 * fSlow13);
		double fSlow205 = (0.050658471445469179 * fSlow15);
		double fSlow206 = (0.0 - (fConst84 * fSlow71));
		double fSlow207 = (0.0 - (fConst84 * fSlow50));
		double fSlow208 = (0.10647773203199999 * fSlow23);
		double fSlow209 = (0.16798421640000002 * fSlow3);
		double fSlow210 = (0.0 - (fConst83 * fSlow32));
		double fSlow211 = (fConst85 * fSlow32);
		double fSlow212 = (0.0 - (fConst77 * fSlow29));
		double fSlow213 = (fConst86 * fSlow3);
		double fSlow214 = (0.0 - (fConst81 * fSlow29));
		double fSlow215 = (fSlow195 - fSlow196);
		double fSlow216 = (0.0 - (fConst78 * fSlow50));
		double fSlow217 = (0.16264988730000002 * fSlow3);
		double fSlow218 = (0.0 - (fConst88 * fSlow29));
		double fSlow219 = (0.0 - (fConst73 * fSlow29));
		double fSlow220 = (fConst88 * fSlow32);
		double fSlow221 = (fConst89 * fSlow3);
		double fSlow222 = (0.0 - (fConst75 * fSlow29));
		double fSlow223 = (0.0 - (fConst76 * fSlow29));
		double fSlow224 = (0.0 - (fConst74 * fSlow50));
		double fSlow225 = (0.14547853605000002 * fSlow3);
		double fSlow226 = (0.0 - (fConst70 * fSlow29));
		double fSlow227 = (fConst70 * fSlow32);
		double fSlow228 = (fConst92 * fSlow3);
		double fSlow229 = (0.0 - (fConst72 * fSlow29));
		double fSlow230 = (fConst93 * fSlow35);
		double fSlow231 = (0.0 - (fConst94 * fSlow52));
		double fSlow232 = (0.0 - (fConst71 * fSlow50));
		double fSlow233 = (0.11111100750000003 * fSlow3);
		double fSlow234 = (0.0 - (fConst68 * fSlow29));
		double fSlow235 = (fConst68 * fSlow32);
		double fSlow236 = (0.0 - (fConst69 * fSlow29));
		double fSlow237 = (0.0 - (fConst95 * fSlow52));
		double fSlow238 = (fConst96 * fSlow35);
		double fSlow239 = (fConst69 * fSlow32);
		double fSlow240 = (0.3086479881593413 * fSlow15);
		double fSlow241 = (0.6172959763186826 * fSlow13);
		double fSlow242 = (0.23907782921757645 * fSlow15);
		double fSlow243 = (0.4781556584351529 * fSlow13);
		double fSlow244 = (fConst97 * fSlow49);
		double fSlow245 = (fConst98 * fSlow49);
		double fSlow246 = (0.055834588607999995 * fSlow23);
		double fSlow247 = (0.35639617816845831 * fSlow13);
		double fSlow248 = (0.17819808908422916 * fSlow15);
		double fSlow249 = (0.0 - (fConst99 * fSlow71));
		double fSlow250 = (0.083751820800000001 * fSlow23);
		double fSlow251 = (fConst100 * fSlow49);
		double fSlow252 = (fConst101 * fSlow74);
		double fSlow253 = (fConst20 * fSlow50);
		double fSlow254 = (fConst102 * fSlow35);
		double fSlow255 = (0.0 - (fConst103 * fSlow71));
		double fSlow256 = (0.10010255635199999 * fSlow23);
		double fSlow257 = (fConst104 * fSlow35);
		double fSlow258 = (fSlow256 + fSlow257);
		double fSlow259 = (0.0 - (fConst105 * fSlow52));
		double fSlow260 = (0.0 - (fConst106 * fSlow52));
		double fSlow261 = (0.1347048603221927 * fSlow13);
		double fSlow262 = (0.067352430161096349 * fSlow15);
		double fSlow263 = (0.0 - (fConst107 * fSlow71));
		double fSlow264 = (0.0 - (fConst107 * fSlow50));
		double fSlow265 = (0.10551735628799999 * fSlow23);
		double fSlow266 = (fSlow256 - fSlow257);
		double fSlow267 = (fConst106 * fSlow74);
		double fSlow268 = (fConst108 * fSlow49);
		double fSlow269 = (0.0 - (fConst103 * fSlow50));
		double fSlow270 = (fConst109 * fSlow35);
		double fSlow271 = (0.0 - (fConst100 * fSlow50));
		double fSlow272 = (0.0 - (fConst99 * fSlow50));
		double fSlow273 = (fConst100 * fSlow71);
		double fSlow274 = (0.0 - (fConst101 * fSlow52));
		double fSlow275 = (0.0 - (fConst98 * fSlow50));
		double fSlow276 = (fConst98 * fSlow71);
		double fSlow277 = (0.0 - (fConst110 * fSlow52));
		double fSlow278 = (0.0 - (fConst97 * fSlow50));
		double fSlow279 = (fConst111 * fSlow35);
		double fSlow280 = (fConst97 * fSlow71);
		double fSlow281 = (0.0 - (fConst112 * fSlow52));
		double fSlow282 = (fConst113 * fSlow35);
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
			double fTemp10 = double(input10[i]);
			double fTemp11 = double(input9[i]);
			double fTemp12 = double(input11[i]);
			double fTemp13 = double(input12[i]);
			double fTemp14 = double(input13[i]);
			double fTemp15 = double(input14[i]);
			double fTemp16 = double(input15[i]);
			output1[i] = FAUSTFLOAT((iSlow1?(((((((fSlow33 * ((fConst11 * fTemp5) + (fSlow34 * fTemp10))) + ((fSlow36 * fTemp11) + ((fSlow37 * fTemp6) + ((fSlow38 * fTemp9) + ((fSlow10 * ((fSlow17 * ((fConst6 * fTemp0) + (fSlow39 * fTemp1))) + (fSlow41 * fTemp3))) + ((fSlow44 * fTemp2) + (fSlow45 * fTemp7))))))) + (fSlow48 * fTemp12)) + (fSlow51 * fTemp13)) + (fSlow53 * fTemp14)) + (fSlow54 * fTemp15)) + (fSlow55 * fTemp16)):(iSlow2?((0.24993000000000001 * fTemp2) + ((((fSlow25 * ((fSlow26 * fTemp0) + (fSlow27 * fTemp5))) + (fSlow28 * fTemp7)) + (fSlow30 * fTemp9)) + (fSlow31 * fTemp6))):fTemp2)));
			double fTemp17 = ((fSlow10 * fTemp7) + (fSlow11 * fTemp8));
			output2[i] = FAUSTFLOAT((iSlow1?((fSlow12 * ((fSlow57 * fTemp10) + (fSlow58 * fTemp15))) + ((fSlow17 * (((fConst11 * fTemp17) + (fSlow59 * fTemp12)) + (fSlow60 * fTemp14))) + (((fSlow61 * fTemp1) + (fSlow3 * ((((fSlow62 * fTemp2) + (0.19245000000000004 * fTemp0)) + (fSlow63 * fTemp3)) + (0.17213247615000002 * fTemp9)))) + (fSlow64 * fTemp13)))):(iSlow2?((0.24993000000000001 * fTemp1) + ((fSlow3 * ((0.43301700000000004 * fTemp0) + (0.38730209625899997 * fTemp9))) + (fSlow56 * fTemp17))):fTemp1)));
			output3[i] = FAUSTFLOAT((iSlow1?(((fSlow17 * (((fConst11 * ((fSlow10 * fTemp5) + (fSlow11 * fTemp6))) + (fSlow68 * fTemp10)) + (fSlow69 * fTemp15))) + ((fSlow70 * fTemp14) + ((fSlow72 * fTemp13) + (((fSlow11 * ((((fSlow73 * fTemp0) + (fSlow75 * fTemp2)) + (fSlow76 * fTemp1)) + (fSlow77 * fTemp11))) + ((fSlow78 * fTemp9) + ((fSlow79 * fTemp3) + (fSlow45 * fTemp8)))) + (fSlow53 * fTemp12))))) + (fSlow36 * fTemp16)):(iSlow2?((0.24993000000000001 * fTemp3) + ((fSlow25 * (((fSlow65 * fTemp0) + (fSlow66 * fTemp5)) + (fSlow27 * fTemp6))) + ((fSlow67 * fTemp9) + (fSlow28 * fTemp8)))):fTemp3)));
			output4[i] = FAUSTFLOAT((iSlow1?(((((fSlow17 * ((((fSlow85 * fTemp3) + (fSlow86 * fTemp7)) + (fSlow87 * fTemp8)) + (fSlow88 * fTemp11))) + ((fSlow89 * fTemp9) + ((fSlow91 * fTemp5) + ((fSlow11 * ((fSlow92 * fTemp2) + (fSlow93 * fTemp0))) + (fSlow94 * fTemp10))))) + (fSlow95 * fTemp12)) + (fSlow96 * fTemp14)) + (fSlow97 * fTemp16)):(iSlow2?((0.24993000000000001 * fTemp5) + (((((fSlow25 * ((0.30902256091651653 * ((fSlow11 * fTemp2) + (fSlow10 * fTemp3))) + (fSlow80 * fTemp11))) + (fSlow81 * fTemp10)) + (fSlow82 * fTemp12)) + (fSlow83 * fTemp14)) + (fSlow84 * fTemp16))):fTemp5)));
			output5[i] = FAUSTFLOAT((iSlow1?((((fSlow3 * (((fSlow101 * fTemp0) + (0.14907119265000002 * fTemp2)) + (0.15936380355000002 * fTemp12))) + ((fSlow17 * ((((fSlow85 * fTemp1) + (fSlow86 * fTemp5)) + (fSlow102 * fTemp9)) + (fSlow103 * fTemp10))) + ((fSlow104 * fTemp6) + ((fSlow107 * fTemp7) + (fSlow108 * fTemp8))))) + (fSlow109 * fTemp13)) + (fSlow110 * fTemp15)):(iSlow2?((0.24993000000000001 * fTemp7) + ((((fSlow25 * ((fSlow66 * fTemp1) + (fSlow98 * fTemp10))) + (fSlow3 * ((0.335413669149 * fTemp2) + (0.35857228434300004 * fTemp12)))) + (fSlow99 * fTemp13)) + (fSlow100 * fTemp15))):fTemp7)));
			double fTemp18 = ((fSlow10 * fTemp12) + (fSlow11 * fTemp14));
			output6[i] = FAUSTFLOAT((iSlow1?((fSlow3 * ((((fSlow112 * fTemp7) + (0.17213247615000002 * fTemp1)) + (fSlow113 * fTemp8)) + (0.16903075950000004 * fTemp13))) + ((fSlow114 * fTemp6) + ((fSlow115 * fTemp9) + ((fSlow89 * fTemp5) + ((fSlow78 * fTemp3) + ((fSlow38 * fTemp2) + ((fSlow24 * fTemp0) + (fSlow116 * fTemp18)))))))):(iSlow2?((0.24993000000000001 * fTemp9) + ((fSlow111 * fTemp18) + ((fSlow3 * ((0.38730209625899997 * fTemp1) + (0.38032316126999999 * fTemp13))) + ((fSlow30 * fTemp2) + (fSlow67 * fTemp3))))):fTemp9)));
			output7[i] = FAUSTFLOAT((iSlow1?((fSlow17 * ((((((fSlow11 * ((fConst11 * fTemp1) + (fSlow119 * fTemp0))) + (fSlow87 * fTemp5)) + (fSlow120 * fTemp9)) + (fSlow86 * fTemp6)) + (fSlow121 * fTemp10)) + (fSlow103 * fTemp15))) + ((fSlow3 * ((0.14907119265000002 * fTemp3) + (0.15936380355000002 * fTemp14))) + ((fSlow122 * fTemp13) + ((fSlow108 * fTemp7) + (fSlow123 * fTemp8))))):(iSlow2?((0.24993000000000001 * fTemp8) + ((fSlow25 * (((fSlow27 * fTemp1) + (fSlow117 * fTemp10)) + (fSlow98 * fTemp15))) + ((fSlow118 * fTemp13) + (fSlow3 * ((0.335413669149 * fTemp3) + (0.35857228434300004 * fTemp14)))))):fTemp8)));
			output8[i] = FAUSTFLOAT((iSlow1?((fSlow17 * ((((fSlow11 * ((fConst11 * fTemp3) + (fSlow126 * fTemp8))) + (fSlow127 * fTemp11)) + (fSlow128 * fTemp12)) + (fSlow88 * fTemp16))) + ((fSlow95 * fTemp14) + ((fSlow91 * fTemp6) + ((fSlow114 * fTemp9) + ((fSlow104 * fTemp7) + ((fSlow37 * fTemp2) + ((fSlow129 * fTemp0) + (fSlow94 * fTemp15)))))))):(iSlow2?((0.24993000000000001 * fTemp6) + ((fSlow25 * ((((fSlow27 * fTemp3) + (fSlow124 * fTemp11)) + (fSlow125 * fTemp12)) + (fSlow80 * fTemp16))) + ((fSlow81 * fTemp15) + ((fSlow31 * fTemp2) + (fSlow82 * fTemp14))))):fTemp6)));
			double fTemp19 = ((fSlow11 * fTemp5) + (fSlow10 * fTemp6));
			output9[i] = FAUSTFLOAT((iSlow1?((fSlow17 * (((fSlow131 * fTemp10) + (fConst31 * fTemp19)) + (fSlow132 * fTemp15))) + ((fSlow133 * fTemp14) + (((fSlow12 * ((fSlow134 * fTemp3) + (fSlow135 * fTemp2))) + (fSlow136 * fTemp11)) + (fSlow137 * fTemp12)))):(iSlow2?((fSlow130 * fTemp19) + (0.24993000000000001 * fTemp11)):fTemp11)));
			output10[i] = FAUSTFLOAT((iSlow1?((0.11112 * fTemp10) + (((fSlow17 * ((((fSlow121 * fTemp8) + (fSlow131 * fTemp11)) + (fSlow139 * fTemp12)) + (fSlow140 * fTemp14))) + (((fSlow3 * ((fSlow141 * fTemp3) + (0.12598816230000001 * fTemp5))) + (fSlow11 * (((fSlow142 * fTemp2) + (fSlow143 * fTemp1)) + (fSlow144 * fTemp7)))) + (fSlow145 * fTemp13))) + (fSlow146 * fTemp16))):(iSlow2?(((fSlow81 * fTemp5) + (fSlow138 * ((fSlow11 * fTemp7) + (fSlow10 * fTemp8)))) + (0.24993000000000001 * fTemp10)):fTemp10)));
			output11[i] = FAUSTFLOAT((iSlow1?((fSlow149 * ((0.36623943296439643 * fTemp14) + (0.23640620007885615 * fTemp16))) + (((fSlow17 * (((fSlow150 * ((0.127153838554589 * fTemp9) + (0.036706218836248881 * fTemp6))) + (fSlow139 * fTemp10)) + (fSlow151 * fTemp13))) + ((((fSlow3 * ((fSlow152 * fTemp1) + (0.15936380355000002 * fTemp7))) + ((fSlow95 * fTemp5) + ((fSlow48 * fTemp2) + (fSlow53 * fTemp3)))) + (fSlow137 * fTemp11)) + (fSlow154 * fTemp12))) + (fSlow155 * fTemp15))):(iSlow2?((((fSlow82 * fTemp5) + (fSlow147 * fTemp7)) + (fSlow148 * ((0.28609910994747967 * fTemp9) + (0.082589850671945866 * fTemp6)))) + (0.24993000000000001 * fTemp12)):fTemp12)));
			output12[i] = FAUSTFLOAT((iSlow1?(((fSlow3 * (((fSlow157 * fTemp12) + (0.16903075950000004 * fTemp9)) + (fSlow158 * fTemp14))) + (((fSlow145 * fTemp10) + (((((fSlow51 * fTemp2) + (fSlow64 * fTemp1)) + (fSlow72 * fTemp3)) + (fSlow109 * fTemp7)) + (fSlow122 * fTemp8))) + (fSlow159 * fTemp13))) + (fSlow160 * fTemp15)):(iSlow2?((((fSlow99 * fTemp7) + (fSlow156 * fTemp9)) + (fSlow118 * fTemp8)) + (0.24993000000000001 * fTemp13)):fTemp13)));
			output13[i] = FAUSTFLOAT((iSlow1?(((fSlow17 * ((((fSlow11 * ((fSlow162 * fTemp1) + (fConst65 * fTemp9))) + (fSlow140 * fTemp10)) + (fSlow163 * fTemp13)) + (fSlow139 * fTemp15))) + ((((fSlow133 * fTemp11) + (((fSlow96 * fTemp5) + ((fSlow70 * fTemp3) + ((fSlow53 * fTemp2) + (fSlow164 * fTemp8)))) + (fSlow95 * fTemp6))) + (fSlow165 * fTemp12)) + (fSlow166 * fTemp14))) + (fSlow137 * fTemp16)):(iSlow2?((((fSlow161 * fTemp9) + ((fSlow83 * fTemp5) + (fSlow147 * fTemp8))) + (fSlow82 * fTemp6)) + (0.24993000000000001 * fTemp14)):fTemp14)));
			output14[i] = FAUSTFLOAT((iSlow1?((0.11112 * fTemp15) + (((fSlow160 * fTemp13) + (((fSlow110 * fTemp7) + ((fSlow168 * fTemp1) + ((fSlow54 * fTemp2) + (fSlow94 * fTemp6)))) + (fSlow155 * fTemp12))) + (fSlow17 * ((((fSlow11 * ((fConst40 * fTemp8) + (fSlow34 * fTemp3))) + (fSlow132 * fTemp11)) + (fSlow139 * fTemp14)) + (fSlow131 * fTemp16))))):(iSlow2?((((fSlow100 * fTemp7) + (fSlow167 * fTemp8)) + (fSlow81 * fTemp6)) + (0.24993000000000001 * fTemp15)):fTemp15)));
			output15[i] = FAUSTFLOAT((iSlow1?(((fSlow33 * ((fConst31 * fTemp6) + (fSlow170 * fTemp15))) + ((fSlow137 * fTemp14) + ((fSlow12 * ((fSlow171 * fTemp12) + (fSlow135 * fTemp3))) + ((fSlow146 * fTemp10) + ((fSlow55 * fTemp2) + (fSlow97 * fTemp5)))))) + (fSlow136 * fTemp16)):(iSlow2?(((fSlow84 * fTemp5) + (fSlow169 * fTemp6)) + (0.24993000000000001 * fTemp16)):fTemp16)));
			double fTemp20 = ((fSlow11 * fTemp11) + (fSlow10 * fTemp16));
			output16[i] = FAUSTFLOAT((iSlow1?((fSlow12 * ((fSlow173 * fTemp5) + (fSlow174 * fTemp6))) + (fSlow175 * fTemp20)):(iSlow2?(fSlow172 * fTemp20):0.0)));
			double fTemp21 = ((fSlow11 * fTemp10) + (fSlow10 * fTemp15));
			output17[i] = FAUSTFLOAT((iSlow1?((fSlow3 * ((fSlow178 * fTemp19) + (0.11111100750000003 * fTemp11))) + ((fSlow12 * ((fSlow179 * fTemp8) + (fSlow180 * fTemp7))) + (fSlow181 * fTemp21))):(iSlow2?((fSlow176 * fTemp11) + (fSlow177 * fTemp21)):0.0)));
			output18[i] = FAUSTFLOAT((iSlow1?(((fSlow11 * (((fSlow185 * fTemp7) + (fSlow186 * fTemp9)) + (fSlow187 * fTemp12))) + ((fSlow3 * ((fSlow188 * fTemp8) + (0.14547853605000002 * fTemp10))) + ((fSlow189 * fTemp5) + (fSlow190 * fTemp11)))) + (fSlow148 * ((0.10596150257755792 * fTemp14) + (0.027359094218383301 * fTemp16)))):(iSlow2?(((fSlow182 * fTemp11) + (fSlow183 * fTemp10)) + (fSlow25 * ((fSlow184 * fTemp16) + (0.23841585846519298 * ((fSlow11 * fTemp12) + (fSlow10 * fTemp14)))))):0.0)));
			output19[i] = FAUSTFLOAT((iSlow1?(((fSlow3 * ((fSlow193 * ((0.41620233757211678 * fTemp9) + (0.12014724747147398 * fTemp6))) + (0.16264988730000002 * fTemp12))) + ((((fSlow194 * fTemp5) + (fSlow197 * fTemp7)) + (fSlow198 * fTemp8)) + (fSlow199 * fTemp10))) + (fSlow148 * ((0.12235394458243126 * fTemp13) + (0.047387405817769852 * fTemp15)))):(iSlow2?(((fSlow191 * fTemp10) + (fSlow192 * fTemp12)) + (fSlow148 * ((0.27529923627565928 * fTemp13) + (0.10662277113532474 * fTemp15)))):0.0)));
			output20[i] = FAUSTFLOAT((iSlow1?(((fSlow203 * fTemp12) + ((fSlow12 * ((fSlow204 * fTemp5) + (fSlow205 * fTemp6))) + ((fSlow206 * fTemp8) + ((fSlow207 * fTemp7) + ((fSlow208 * fTemp9) + (fSlow209 * fTemp13)))))) + (fSlow210 * fTemp14)):(iSlow2?(((fSlow200 * fTemp12) + (fSlow201 * fTemp13)) + (fSlow202 * fTemp14)):0.0)));
			output21[i] = FAUSTFLOAT((iSlow1?(((fSlow33 * ((fSlow213 * fTemp9) + (fConst87 * fTemp13))) + ((fSlow214 * fTemp10) + ((fSlow194 * fTemp6) + ((fSlow215 * fTemp8) + ((fSlow198 * fTemp7) + ((fSlow216 * fTemp5) + (fSlow217 * fTemp14))))))) + (fSlow199 * fTemp15)):(iSlow2?(((fSlow211 * fTemp13) + ((fSlow212 * fTemp10) + (fSlow192 * fTemp14))) + (fSlow191 * fTemp15)):0.0)));
			output22[i] = FAUSTFLOAT((iSlow1?(((fSlow33 * ((fSlow221 * fTemp8) + (fConst90 * fTemp14))) + ((fSlow222 * fTemp12) + ((fSlow223 * fTemp11) + ((fSlow189 * fTemp6) + ((fSlow106 * fTemp9) + ((fSlow224 * fTemp7) + (fSlow225 * fTemp15))))))) + (fSlow190 * fTemp16)):(iSlow2?(((fSlow218 * fTemp12) + ((fSlow219 * fTemp11) + ((fSlow220 * fTemp14) + (fSlow183 * fTemp15)))) + (fSlow182 * fTemp16)):0.0)));
			output23[i] = FAUSTFLOAT((iSlow1?((fSlow33 * ((fConst91 * fTemp15) + (fSlow228 * fTemp6))) + ((fSlow229 * fTemp10) + ((fSlow230 * fTemp8) + ((fSlow231 * fTemp7) + ((fSlow232 * fTemp5) + (fSlow233 * fTemp16)))))):(iSlow2?(((fSlow226 * fTemp10) + (fSlow227 * fTemp15)) + (fSlow176 * fTemp16)):0.0)));
			output24[i] = FAUSTFLOAT((iSlow1?((fSlow236 * fTemp11) + ((fSlow237 * fTemp5) + ((fSlow238 * fTemp6) + (fSlow239 * fTemp16)))):(iSlow2?((fSlow234 * fTemp11) + (fSlow235 * fTemp16)):0.0)));
			output25[i] = FAUSTFLOAT((iSlow1?(fSlow12 * ((fSlow240 * fTemp11) + (fSlow241 * fTemp16))):0.0));
			output26[i] = FAUSTFLOAT((iSlow1?((fSlow12 * ((fSlow242 * fTemp10) + (fSlow243 * fTemp15))) + (fSlow244 * fTemp20)):0.0));
			output27[i] = FAUSTFLOAT((iSlow1?((fSlow245 * fTemp21) + ((fSlow246 * fTemp11) + (fSlow12 * ((fSlow247 * fTemp14) + (fSlow248 * fTemp12))))):0.0));
			output28[i] = FAUSTFLOAT((iSlow1?((((fSlow249 * fTemp11) + (fSlow250 * fTemp10)) + (fSlow11 * ((fSlow251 * fTemp12) + (fSlow252 * fTemp13)))) + (fSlow253 * ((0.3273704914048397 * fTemp14) + (0.084526530349225165 * fTemp16)))):0.0));
			output29[i] = FAUSTFLOAT((iSlow1?(((fSlow253 * ((0.40411511288106022 * fTemp13) + (0.15651317678313359 * fTemp15))) + ((((fSlow254 * fTemp11) + (fSlow255 * fTemp10)) + (fSlow258 * fTemp12)) + (fSlow259 * fTemp14))) + (fSlow260 * fTemp16)):0.0));
			output30[i] = FAUSTFLOAT((iSlow1?((fSlow12 * ((fSlow261 * fTemp10) + (fSlow262 * fTemp15))) + ((fSlow263 * fTemp14) + ((fSlow264 * fTemp12) + (fSlow265 * fTemp13)))):0.0));
			output31[i] = FAUSTFLOAT((iSlow1?((fSlow255 * fTemp15) + ((fSlow266 * fTemp14) + ((fSlow11 * ((fSlow267 * fTemp11) + (fSlow268 * fTemp13))) + ((fSlow259 * fTemp12) + ((fSlow269 * fTemp10) + (fSlow254 * fTemp16)))))):0.0));
			output32[i] = FAUSTFLOAT((iSlow1?((((fSlow270 * fTemp13) + ((fSlow271 * fTemp12) + ((fSlow272 * fTemp11) + (fSlow273 * fTemp14)))) + (fSlow250 * fTemp15)) + (fSlow249 * fTemp16)):0.0));
			output33[i] = FAUSTFLOAT((iSlow1?(((fSlow270 * fTemp14) + ((fSlow274 * fTemp12) + ((fSlow275 * fTemp10) + (fSlow276 * fTemp15)))) + (fSlow246 * fTemp16)):0.0));
			output34[i] = FAUSTFLOAT((iSlow1?((fSlow277 * fTemp10) + ((fSlow278 * fTemp11) + ((fSlow279 * fTemp15) + (fSlow280 * fTemp16)))):0.0));
			output35[i] = FAUSTFLOAT((iSlow1?((fSlow281 * fTemp11) + (fSlow282 * fTemp16)):0.0));
			
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
