/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAEncLebedev502"
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
	FAUSTFLOAT fHslider0;
	double fRec1[2];
	double fRec0[2];
	FAUSTFLOAT fVbargraph0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
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
	double fRec2[2];
	FAUSTFLOAT fVbargraph1;
	double fRec3[2];
	FAUSTFLOAT fVbargraph2;
	double fConst16;
	double fConst17;
	double fConst18;
	double fRec4[2];
	FAUSTFLOAT fVbargraph3;
	double fConst19;
	double fConst20;
	double fConst21;
	double fConst22;
	double fConst23;
	double fConst24;
	double fConst25;
	double fConst26;
	double fConst27;
	double fRec5[2];
	FAUSTFLOAT fVbargraph4;
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
	double fRec6[2];
	FAUSTFLOAT fVbargraph5;
	double fRec7[2];
	FAUSTFLOAT fVbargraph6;
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
	double fRec8[2];
	FAUSTFLOAT fVbargraph7;
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
	double fRec9[2];
	FAUSTFLOAT fVbargraph8;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("gui.lib/author", "Pierre Lecomte");
		m->declare("gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("gui.lib/license", "GPL");
		m->declare("gui.lib/name", "GUI Library");
		m->declare("gui.lib/version", "1.0");
		m->declare("lebedev.lib/author", "Pierre Lecomte");
		m->declare("lebedev.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("lebedev.lib/license", "GPL");
		m->declare("lebedev.lib/name", "Lebdev grids and weights");
		m->declare("lebedev.lib/version", "1.0");
		m->declare("license", "GPL");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("name", "HOAEncLebedev502");
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
		return 50;
		
	}
	virtual int getNumOutputs() {
		return 9;
		
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
		fConst1 = double(tgamma(1.5));
		fConst2 = double(tgamma(1.0));
		fConst3 = (fConst1 * sqrt((3.0 * (fConst2 / double(tgamma(3.0))))));
		fConst4 = (0.012698412698412698 * (0.0 - (1.5957691216057308 * fConst3)));
		fConst5 = (0.0 - (1.1283791670955128 * fConst3));
		fConst6 = (0.0 - (1.1283791670955126 * fConst3));
		fConst7 = (0.0 - (1.128379167095513 * fConst3));
		fConst8 = (0.0 - (0.92131773192356115 * fConst3));
		fConst9 = (0.0 - (0.92131773192356148 * fConst3));
		fConst10 = (0.0 - (0.48114249349102017 * fConst3));
		fConst11 = (0.0 - (0.48114249349102034 * fConst3));
		fConst12 = (0.0 - (0.48114249349102112 * fConst3));
		fConst13 = (0.0 - (1.4434274804730622 * fConst3));
		fConst14 = (0.0 - (1.4434274804730625 * fConst3));
		fConst15 = (0.0 - (0.48114249349102228 * fConst3));
		fConst16 = (0.0 - (0.48114249349102062 * fConst3));
		fConst17 = (0.0 - (1.443427480473062 * fConst3));
		fConst18 = (0.0 - (0.48114249349102095 * fConst3));
		fConst19 = (double(tgamma(2.5)) * sqrt((5.0 * (fConst2 / double(tgamma(5.0))))));
		fConst20 = (0.0 - (0.58027968058390089 * fConst19));
		fConst21 = (0.0 - (1.7408390417517061 * fConst19));
		fConst22 = (0.0 - (1.7408390417517088 * fConst19));
		fConst23 = (0.0 - (1.7408390417517057 * fConst19));
		fConst24 = (0.0 - (1.7408390417517114 * fConst19));
		fConst25 = (0.0 - (3.1915382432114616 * fConst19));
		fConst26 = (0.022574955908289243 * fConst25);
		fConst27 = (0.021093750000000001 * (0.0 - (2.1276921621409737 * fConst19)));
		fConst28 = sqrt((5.0 * (double(tgamma(2.0)) / double(tgamma(4.0)))));
		fConst29 = (fConst1 * fConst28);
		fConst30 = (0.0 - (2.3936536824085959 * fConst29));
		fConst31 = (0.0 - (1.6925687506432687 * fConst1));
		fConst32 = (0.0 - (1.4142135623730951 * (fConst28 * fConst31)));
		fConst33 = (0.0 - (1.5957691216057308 * fConst29));
		fConst34 = (0.0 - (1.5957691216057313 * fConst29));
		fConst35 = (0.0 - (1.5957691216057308 * fConst1));
		fConst36 = (fConst28 * fConst35);
		fConst37 = (0.0 - fConst36);
		fConst38 = (0.0 - (1.0000000000000002 * fConst36));
		fConst39 = (0.054036626340088233 * fConst1);
		fConst40 = (0.031925808815477283 * fConst31);
		fConst41 = (0.033660754908870889 * fConst1);
		fConst42 = (0.021093750000000001 * fConst35);
		fConst43 = (0.021093750000000005 * fConst35);
		fConst44 = (0.026338897580074728 * fConst1);
		fConst45 = (0.0087796325266915921 * fConst1);
		fConst46 = (0.026338897580074742 * fConst1);
		fConst47 = (0.026338897580074745 * fConst1);
		fConst48 = (0.0087796325266915939 * fConst1);
		fConst49 = (0.0 - (0.97315860881060223 * fConst1));
		fConst50 = (0.0090217899191397874 * fConst49);
		fConst51 = (0.027065369757419327 * fConst49);
		fConst52 = (0.027065369757419331 * fConst49);
		fConst53 = (0.0090217899191397891 * fConst49);
		fConst54 = (0.0 - (1.3056292813137786 * fConst1));
		fConst55 = (0.020173335537918873 * fConst54);
		fConst56 = (0.0 - (1.3056292813137786 * fConst29));
		fConst57 = (0.0 - (1.3056292813137791 * fConst29));
		fConst58 = (0.0 - (0.43520976043792686 * fConst29));
		fConst59 = (0.0 - (1.3056292813137795 * fConst29));
		fConst60 = (0.0 - (1.3056292813137798 * fConst29));
		fConst61 = (0.0 - (0.43520976043792792 * fConst29));
		fConst62 = (fConst28 * fConst49);
		fConst63 = (0.0 - (0.44721359549995832 * fConst62));
		fConst64 = (0.0 - (1.3416407864998738 * fConst62));
		fConst65 = (0.0 - (1.3416407864998741 * fConst62));
		fConst66 = (0.0 - (0.44721359549995937 * fConst62));
		fConst67 = (fConst28 * fConst54);
		fConst68 = (0.0 - fConst67);
		fConst69 = (0.0 - (1.0000000000000002 * fConst67));
		fConst70 = (0.033660754908870882 * fConst1);
		fConst71 = (0.021093749999999998 * fConst35);
		fConst72 = (0.026338897580074724 * fConst1);
		fConst73 = (0.026338897580074721 * fConst1);
		fConst74 = (0.026338897580074738 * fConst1);
		fConst75 = (0.0087796325266915817 * fConst1);
		fConst76 = (0.0087796325266915748 * fConst1);
		fConst77 = (0.026338897580074731 * fConst1);
		fConst78 = (0.027065369757419324 * fConst49);
		fConst79 = (0.009021789919139777 * fConst49);
		fConst80 = (0.00902178991913977 * fConst49);
		fConst81 = (0.027065369757419321 * fConst49);
		fConst82 = (0.02017333553791887 * fConst54);
		fConst83 = (0.020173335537918866 * fConst54);
		fConst84 = (0.0 - (0.43520976043792636 * fConst29));
		fConst85 = (0.0 - (1.3056292813137793 * fConst29));
		fConst86 = (0.0 - (0.4352097604379267 * fConst29));
		fConst87 = (0.0 - (0.44721359549995782 * fConst62));
		fConst88 = (0.0 - (1.3416407864998736 * fConst62));
		fConst89 = (0.0 - (0.44721359549995815 * fConst62));
		fConst90 = (0.0 - (1.2884797242127563e-16 * fConst19));
		fConst91 = (0.0 - (2.5769594484255127e-16 * fConst19));
		fConst92 = (0.0 - (2.3211187223356089 * fConst19));
		fConst93 = (0.0 - (2.3211187223356093 * fConst19));
		fConst94 = (0.0 - (2.3211187223356085 * fConst19));
		fConst95 = (0.0 - (2.3211187223356098 * fConst19));
		fConst96 = (0.012698412698412698 * fConst25);
		fConst97 = (0.0 - (1.595769121605731 * fConst19));
		fConst98 = (0.0 - (7.0866384831701748e-16 * fConst19));
		fConst99 = (0.0 - (1.417327696634035e-15 * fConst19));
		fConst100 = (0.0 - (4.7244256554467816e-16 * fConst19));
		fConst101 = (0.0 - (9.4488513108935631e-16 * fConst19));
		
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.0);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec1[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec0[l1] = 0.0;
			
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
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec5[l5] = 0.0;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec6[l6] = 0.0;
			
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec7[l7] = 0.0;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec8[l8] = 0.0;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fRec9[l9] = 0.0;
			
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
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bbcaa0", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bd6400", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2be03d0", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bf2160", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c06520", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c28d00", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c32f30", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c49e40", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c61b90", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
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
		double fSlow0 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fHslider0))));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
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
			double fTemp55 = (fRec1[0] * ((((0.012698412698412698 * (((((fTemp0 + fTemp1) + fTemp2) + fTemp3) + fTemp4) + fTemp5)) + (0.022574955908289243 * ((((((((fTemp10 + fTemp11) + fTemp12) + fTemp13) + fTemp14) + fTemp15) + fTemp16) + fTemp17) + fTemp18))) + (0.021093750000000001 * fTemp28)) + (0.02017333553791887 * ((((((((((((((((((((fTemp34 + fTemp35) + fTemp36) + fTemp37) + fTemp38) + fTemp39) + fTemp40) + fTemp41) + fTemp42) + fTemp43) + fTemp44) + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) + fTemp50) + fTemp51) + fTemp52) + fTemp53) + fTemp54))));
			fRec0[0] = max((fRec0[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph0 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp55);
			double fTemp56 = (fRec1[0] * (((((fConst4 * fTemp4) + (0.022574955908289243 * ((((fConst5 * fTemp9) + (fConst6 * fTemp13)) + (fConst7 * fTemp14)) + (fConst5 * fTemp18)))) + (0.021093750000000001 * ((((fConst8 * fTemp21) + (fConst9 * fTemp22)) + (fConst8 * fTemp26)) + (fConst9 * fTemp27)))) + (fConst3 * (((((((((((((((((((((0.02026373487753309 * fTemp2) + (0.025473109945013343 * fTemp7)) + (0.025473109945013336 * fTemp11)) + (0.025473109945013343 * fTemp12)) + (0.025473109945013343 * fTemp16)) + (0.019434045907762619 * fTemp19)) + (0.019434045907762623 * fTemp20)) + (0.019434045907762619 * fTemp24)) + (0.019434045907762623 * fTemp25)) + (0.0097062489627452973 * fTemp29)) + (0.0097062489627452973 * fTemp30)) + (0.0097062489627453199 * fTemp35)) + (0.029118746888235923 * fTemp36)) + (0.029118746888235927 * fTemp37)) + (0.0097062489627453216 * fTemp38)) + (0.0097062489627453199 * fTemp43)) + (0.029118746888235923 * fTemp44)) + (0.029118746888235927 * fTemp45)) + (0.0097062489627453216 * fTemp46)) + (0.0097062489627452973 * fTemp51)) + (0.0097062489627452973 * fTemp52)))) + (0.02017333553791887 * ((((((((((((fConst10 * fTemp32) + (fConst11 * fTemp33)) + (fConst12 * fTemp39)) + (fConst13 * fTemp40)) + (fConst14 * fTemp41)) + (fConst15 * fTemp42)) + (fConst12 * fTemp47)) + (fConst13 * fTemp48)) + (fConst14 * fTemp49)) + (fConst15 * fTemp50)) + (fConst10 * fTemp53)) + (fConst11 * fTemp54)))));
			fRec2[0] = max((fRec2[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fVbargraph1 = FAUSTFLOAT(fRec2[0]);
			output1[i] = FAUSTFLOAT(fTemp56);
			double fTemp57 = (((((((fTemp35 + fTemp36) + fTemp37) + fTemp38) + fTemp39) + fTemp40) + fTemp41) + fTemp42);
			double fTemp58 = (fRec1[0] * (((((0.021994295969128601 * (fTemp0 - fTemp5)) + (0.027648561470568499 * ((((fTemp10 - fTemp15) - fTemp16) - fTemp17) - fTemp18))) + (0.021093750000000001 * ((((fTemp23 - fTemp24) - fTemp25) - fTemp26) - fTemp27))) + (0.010535180889746075 * ((((((((fTemp57 - fTemp43) - fTemp44) - fTemp45) - fTemp46) - fTemp47) - fTemp48) - fTemp49) - fTemp50))) + (0.031605542669238249 * ((((fTemp34 - fTemp51) - fTemp52) - fTemp53) - fTemp54))));
			fRec3[0] = max((fRec3[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fVbargraph2 = FAUSTFLOAT(fRec3[0]);
			output2[i] = FAUSTFLOAT(fTemp58);
			double fTemp59 = (fRec1[0] * ((fConst3 * (((((((((((((((((((((0.02026373487753309 * fTemp1) + (0.025473109945013343 * fTemp6)) + (0.025473109945013343 * fTemp11)) + (0.025473109945013332 * fTemp14)) + (0.025473109945013343 * fTemp15)) + (0.019434045907762623 * fTemp19)) + (0.019434045907762616 * fTemp22)) + (0.019434045907762623 * fTemp24)) + (0.019434045907762616 * fTemp27)) + (0.0097062489627452956 * fTemp29)) + (0.0097062489627452939 * fTemp33)) + (0.029118746888235916 * fTemp35)) + (0.0097062489627453077 * fTemp36)) + (0.0097062489627453008 * fTemp41)) + (0.029118746888235913 * fTemp42)) + (0.029118746888235916 * fTemp43)) + (0.0097062489627453077 * fTemp44)) + (0.0097062489627453008 * fTemp49)) + (0.029118746888235913 * fTemp50)) + (0.0097062489627452956 * fTemp51)) + (0.0097062489627452939 * fTemp54))) + ((((fConst4 * fTemp3) + (0.022574955908289243 * ((((fConst5 * fTemp8) + (fConst6 * fTemp12)) + (fConst7 * fTemp13)) + (fConst5 * fTemp17)))) + (0.021093750000000001 * ((((fConst8 * fTemp20) + (fConst9 * fTemp21)) + (fConst8 * fTemp25)) + (fConst9 * fTemp26)))) + (0.02017333553791887 * ((((((((((((fConst10 * fTemp30) + (fConst11 * fTemp32)) + (fConst16 * fTemp37)) + (fConst17 * fTemp38)) + (fConst13 * fTemp39)) + (fConst18 * fTemp40)) + (fConst16 * fTemp45)) + (fConst17 * fTemp46)) + (fConst13 * fTemp47)) + (fConst18 * fTemp48)) + (fConst10 * fTemp52)) + (fConst11 * fTemp53))))));
			fRec4[0] = max((fRec4[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fVbargraph3 = FAUSTFLOAT(fRec4[0]);
			output3[i] = FAUSTFLOAT(fTemp59);
			double fTemp60 = (fRec1[0] * ((0.02017333553791887 * (((((((((((fConst20 * (fTemp30 + fTemp33)) + (fConst21 * fTemp37)) + (fConst22 * fTemp38)) + (fConst23 * fTemp41)) + (fConst24 * fTemp42)) + (fConst21 * fTemp45)) + (fConst22 * fTemp46)) + (fConst23 * fTemp49)) + (fConst24 * fTemp50)) + (fConst20 * fTemp52)) + (fConst20 * fTemp54))) + (((fConst26 * (fTemp12 + fTemp14)) + (fConst27 * (((fTemp20 + fTemp22) + fTemp25) + fTemp27))) + (fConst19 * ((((((((((((((((((0.072048835120117657 * fTemp11) + (0.072048835120117671 * fTemp13)) + (0.044881006545161167 * fTemp19)) + (0.044881006545161174 * fTemp21)) + (0.044881006545161167 * fTemp24)) + (0.044881006545161174 * fTemp26)) + (0.011706176702255417 * fTemp29)) + (0.011706176702255421 * fTemp32)) + (0.035118530106766375 * fTemp35)) + (0.035118530106766334 * fTemp36)) + (0.035118530106766362 * fTemp39)) + (0.035118530106766348 * fTemp40)) + (0.035118530106766375 * fTemp43)) + (0.035118530106766334 * fTemp44)) + (0.035118530106766362 * fTemp47)) + (0.035118530106766348 * fTemp48)) + (0.011706176702255417 * fTemp51)) + (0.011706176702255421 * fTemp53))))));
			fRec5[0] = max((fRec5[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fVbargraph4 = FAUSTFLOAT(fRec5[0]);
			output4[i] = FAUSTFLOAT(fTemp60);
			double fTemp61 = (fConst41 * fTemp19);
			double fTemp62 = (fRec1[0] * ((((0.022574955908289243 * ((fConst30 * fTemp9) + (fConst32 * fTemp18))) + (0.021093750000000001 * ((((fConst33 * fTemp21) + (fConst34 * fTemp22)) + (fConst37 * fTemp26)) + (fConst38 * fTemp27)))) + (fConst28 * ((((((((((((((((((fConst39 * fTemp7) + (fConst40 * fTemp16)) + fTemp61) + (fConst41 * fTemp20)) + (fConst42 * fTemp24)) + (fConst43 * fTemp25)) + (fConst44 * fTemp29)) + (fConst44 * fTemp30)) + (fConst45 * fTemp35)) + (fConst46 * fTemp36)) + (fConst47 * fTemp37)) + (fConst48 * fTemp38)) + (fConst50 * fTemp43)) + (fConst51 * fTemp44)) + (fConst52 * fTemp45)) + (fConst53 * fTemp46)) + (fConst55 * fTemp51)) + (fConst55 * fTemp52)))) + (0.02017333553791887 * ((((((((((((fConst56 * fTemp32) + (fConst57 * fTemp33)) + (fConst58 * fTemp39)) + (fConst59 * fTemp40)) + (fConst60 * fTemp41)) + (fConst61 * fTemp42)) + (fConst63 * fTemp47)) + (fConst64 * fTemp48)) + (fConst65 * fTemp49)) + (fConst66 * fTemp50)) + (fConst68 * fTemp53)) + (fConst69 * fTemp54)))));
			fRec6[0] = max((fRec6[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fVbargraph5 = FAUSTFLOAT(fRec6[0]);
			output5[i] = FAUSTFLOAT(fTemp62);
			double fTemp63 = (fRec1[0] * ((((0.01261978399999881 * ((((fTemp10 + fTemp15) + fTemp16) + fTemp17) + fTemp18)) + ((0.028394513999997331 * (fTemp0 + fTemp5)) + (5.236595479505967e-18 * fTemp28))) + (0.032806508796871948 * ((((fTemp34 + fTemp51) + fTemp52) + fTemp53) + fTemp54))) - (((0.014197256999998666 * (((fTemp1 + fTemp2) + fTemp3) + fTemp4)) + (0.025239567999997631 * (((fTemp11 + fTemp12) + fTemp13) + fTemp14))) + (0.01640325439843596 * ((((((((fTemp57 + fTemp43) + fTemp44) + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) + fTemp50)))));
			fRec7[0] = max((fRec7[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fVbargraph6 = FAUSTFLOAT(fRec7[0]);
			output6[i] = FAUSTFLOAT(fTemp63);
			double fTemp64 = (fRec1[0] * ((fConst28 * ((((((((((((((((fTemp61 + ((fConst39 * fTemp6) + (fConst40 * fTemp15))) + (fConst70 * fTemp22)) + (fConst43 * fTemp24)) + (fConst71 * fTemp27)) + (fConst72 * fTemp29)) + (fConst73 * fTemp33)) + (fConst74 * fTemp35)) + (fConst75 * fTemp36)) + (fConst76 * fTemp41)) + (fConst77 * fTemp42)) + (fConst78 * fTemp43)) + (fConst79 * fTemp44)) + (fConst80 * fTemp49)) + (fConst81 * fTemp50)) + (fConst82 * fTemp51)) + (fConst83 * fTemp54))) + (((0.022574955908289243 * ((fConst30 * fTemp8) + (fConst32 * fTemp17))) + (0.021093750000000001 * ((((fConst33 * fTemp20) + (fConst34 * fTemp21)) + (fConst37 * fTemp25)) + (fConst38 * fTemp26)))) + (0.02017333553791887 * ((((((((((((fConst56 * fTemp30) + (fConst57 * fTemp32)) + (fConst84 * fTemp37)) + (fConst85 * fTemp38)) + (fConst59 * fTemp39)) + (fConst86 * fTemp40)) + (fConst87 * fTemp45)) + (fConst88 * fTemp46)) + (fConst64 * fTemp47)) + (fConst89 * fTemp48)) + (fConst68 * fTemp52)) + (fConst69 * fTemp53))))));
			fRec8[0] = max((fRec8[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fVbargraph7 = FAUSTFLOAT(fRec8[0]);
			output7[i] = FAUSTFLOAT(fTemp64);
			double fTemp65 = (fRec1[0] * ((0.02017333553791887 * (((((((((((((fConst90 * fTemp31) + (fConst91 * fTemp33)) + (fConst92 * fTemp36)) + (fConst93 * fTemp37)) + (fConst94 * fTemp40)) + (fConst95 * fTemp41)) + (fConst92 * fTemp44)) + (fConst93 * fTemp45)) + (fConst94 * fTemp48)) + (fConst95 * fTemp49)) + (fConst90 * fTemp51)) + (fConst90 * fTemp52)) + (fConst91 * fTemp54))) + ((((fConst96 * (fTemp2 + fTemp4)) + (0.022574955908289243 * (((((fConst97 * (fTemp7 + fTemp9)) + (fConst98 * fTemp12)) + (fConst99 * fTemp14)) + (fConst97 * fTemp16)) + (fConst97 * fTemp18)))) + (0.021093750000000001 * ((((fConst100 * fTemp20) + (fConst101 * fTemp22)) + (fConst100 * fTemp25)) + (fConst101 * fTemp27)))) + (fConst19 * (((((((((((((((((((((0.04052746975506618 * (fTemp1 + fTemp3)) + (0.036024417560058836 * fTemp6)) + (0.036024417560058836 * fTemp8)) + (1.5998055129555246e-17 * fTemp11)) + (3.1996110259110492e-17 * fTemp13)) + (0.036024417560058836 * fTemp15)) + (0.036024417560058836 * fTemp17)) + (9.9655853669580557e-18 * fTemp19)) + (1.9931170733916111e-17 * fTemp21)) + (9.9655853669580557e-18 * fTemp24)) + (1.9931170733916111e-17 * fTemp26)) + (5.19858676206982e-18 * fTemp32)) + (0.046824706809021746 * fTemp35)) + (0.046824706809021746 * fTemp38)) + (0.046824706809021774 * fTemp39)) + (0.046824706809021698 * fTemp42)) + (0.046824706809021746 * fTemp43)) + (0.046824706809021746 * fTemp46)) + (0.046824706809021774 * fTemp47)) + (0.046824706809021698 * fTemp50)) + (5.19858676206982e-18 * fTemp53))))));
			fRec9[0] = max((fRec9[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fVbargraph8 = FAUSTFLOAT(fRec9[0]);
			output8[i] = FAUSTFLOAT(fTemp65);
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec2[1] = fRec2[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			fRec8[1] = fRec8[0];
			fRec9[1] = fRec9[0];
			
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
