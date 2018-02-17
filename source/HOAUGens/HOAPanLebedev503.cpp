/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAPanLebedev503"
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
	
	int fSamplingFreq;
	double fConst0;
	double fConst1;
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fCheckbox0;
	double fConst2;
	double fConst3;
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fEntry0;
	FAUSTFLOAT fCheckbox1;
	FAUSTFLOAT fHslider2;
	double fRec3[2];
	double fRec4[2];
	double fRec2[2];
	double fRec1[2];
	double fRec6[2];
	double fRec5[2];
	FAUSTFLOAT fCheckbox2;
	double fConst4;
	double fConst5;
	double fConst6;
	double fConst7;
	double fRec10[2];
	double fRec11[2];
	double fRec9[2];
	double fRec12[2];
	double fVec0[2];
	double fRec13[2];
	double fRec14[2];
	double fRec8[2];
	double fRec7[2];
	double fRec17[2];
	double fRec18[2];
	double fRec16[2];
	double fRec19[2];
	double fRec15[2];
	FAUSTFLOAT fCheckbox3;
	FAUSTFLOAT fCheckbox4;
	double fConst8;
	double fConst9;
	double fRec22[2];
	double fRec23[2];
	double fRec21[2];
	double fRec20[2];
	double fVec1[2];
	double fRec24[2];
	double fRec26[2];
	double fRec27[2];
	double fRec25[2];
	double fRec0[2];
	FAUSTFLOAT fVbargraph0;
	FAUSTFLOAT fHslider3;
	double fRec28[2];
	FAUSTFLOAT fVbargraph1;
	double fRec29[2];
	FAUSTFLOAT fVbargraph2;
	double fRec30[2];
	FAUSTFLOAT fVbargraph3;
	double fRec31[2];
	FAUSTFLOAT fVbargraph4;
	double fRec32[2];
	FAUSTFLOAT fVbargraph5;
	double fRec33[2];
	FAUSTFLOAT fVbargraph6;
	double fRec34[2];
	FAUSTFLOAT fVbargraph7;
	double fRec35[2];
	FAUSTFLOAT fVbargraph8;
	double fRec36[2];
	FAUSTFLOAT fVbargraph9;
	double fRec37[2];
	FAUSTFLOAT fVbargraph10;
	double fRec38[2];
	FAUSTFLOAT fVbargraph11;
	double fRec39[2];
	FAUSTFLOAT fVbargraph12;
	double fRec40[2];
	FAUSTFLOAT fVbargraph13;
	double fRec41[2];
	FAUSTFLOAT fVbargraph14;
	double fRec42[2];
	FAUSTFLOAT fVbargraph15;
	double fRec43[2];
	FAUSTFLOAT fVbargraph16;
	double fRec44[2];
	FAUSTFLOAT fVbargraph17;
	double fRec45[2];
	FAUSTFLOAT fVbargraph18;
	double fRec46[2];
	FAUSTFLOAT fVbargraph19;
	double fRec47[2];
	FAUSTFLOAT fVbargraph20;
	double fRec48[2];
	FAUSTFLOAT fVbargraph21;
	double fRec49[2];
	FAUSTFLOAT fVbargraph22;
	double fRec50[2];
	FAUSTFLOAT fVbargraph23;
	double fRec51[2];
	FAUSTFLOAT fVbargraph24;
	double fRec52[2];
	FAUSTFLOAT fVbargraph25;
	double fRec53[2];
	FAUSTFLOAT fVbargraph26;
	double fRec54[2];
	FAUSTFLOAT fVbargraph27;
	double fRec55[2];
	FAUSTFLOAT fVbargraph28;
	double fRec56[2];
	FAUSTFLOAT fVbargraph29;
	double fRec57[2];
	FAUSTFLOAT fVbargraph30;
	double fRec58[2];
	FAUSTFLOAT fVbargraph31;
	double fRec59[2];
	FAUSTFLOAT fVbargraph32;
	double fRec60[2];
	FAUSTFLOAT fVbargraph33;
	double fRec61[2];
	FAUSTFLOAT fVbargraph34;
	double fRec62[2];
	FAUSTFLOAT fVbargraph35;
	double fRec63[2];
	FAUSTFLOAT fVbargraph36;
	double fRec64[2];
	FAUSTFLOAT fVbargraph37;
	double fRec65[2];
	FAUSTFLOAT fVbargraph38;
	double fRec66[2];
	FAUSTFLOAT fVbargraph39;
	double fRec67[2];
	FAUSTFLOAT fVbargraph40;
	double fRec68[2];
	FAUSTFLOAT fVbargraph41;
	double fRec69[2];
	FAUSTFLOAT fVbargraph42;
	double fRec70[2];
	FAUSTFLOAT fVbargraph43;
	double fRec71[2];
	FAUSTFLOAT fVbargraph44;
	double fRec72[2];
	FAUSTFLOAT fVbargraph45;
	double fRec73[2];
	FAUSTFLOAT fVbargraph46;
	double fRec74[2];
	FAUSTFLOAT fVbargraph47;
	double fRec75[2];
	FAUSTFLOAT fVbargraph48;
	double fRec76[2];
	FAUSTFLOAT fVbargraph49;
	
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
		m->declare("name", "HOAPanLebedev503");
		m->declare("nfc.lib/author", "Pierre Lecomte");
		m->declare("nfc.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("nfc.lib/license", "GPL");
		m->declare("nfc.lib/name", "NF And NFC Filters Library");
		m->declare("nfc.lib/version", "1.0");
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
		return 1;
		
	}
	virtual int getNumOutputs() {
		return 50;
		
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
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
		fConst0 = min(192000.0, max(1.0, double(fSamplingFreq)));
		fConst1 = (80.0 / fConst0);
		fConst2 = (340.0 / fConst0);
		fConst3 = (170.0 / fConst0);
		fConst4 = (789.54302057287055 / fConst0);
		fConst5 = (394.77151028643527 / fConst0);
		fConst6 = (625.22848971356643 / fConst0);
		fConst7 = (1250.4569794271329 / fConst0);
		fConst8 = (510.0 / fConst0);
		fConst9 = (1020.0 / fConst0);
		
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.0);
		fCheckbox0 = FAUSTFLOAT(0.0);
		fHslider1 = FAUSTFLOAT(2.0);
		fEntry0 = FAUSTFLOAT(1.0700000000000001);
		fCheckbox1 = FAUSTFLOAT(0.0);
		fHslider2 = FAUSTFLOAT(0.0);
		fCheckbox2 = FAUSTFLOAT(0.0);
		fCheckbox3 = FAUSTFLOAT(0.0);
		fCheckbox4 = FAUSTFLOAT(0.0);
		fHslider3 = FAUSTFLOAT(0.0);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec3[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec4[l1] = 0.0;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec2[l2] = 0.0;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec1[l3] = 0.0;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec6[l4] = 0.0;
			
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec5[l5] = 0.0;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec10[l6] = 0.0;
			
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec11[l7] = 0.0;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec9[l8] = 0.0;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fRec12[l9] = 0.0;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fVec0[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec13[l11] = 0.0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec14[l12] = 0.0;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec8[l13] = 0.0;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec7[l14] = 0.0;
			
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec17[l15] = 0.0;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fRec18[l16] = 0.0;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec16[l17] = 0.0;
			
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec19[l18] = 0.0;
			
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec15[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec22[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec23[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec21[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec20[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fVec1[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec24[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec26[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec27[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec25[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec0[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec28[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec29[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec30[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec31[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec32[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec33[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec34[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec35[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec36[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec37[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec38[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec39[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec40[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec41[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec42[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec43[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec44[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec45[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec46[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec47[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec48[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec49[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec50[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec51[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec52[l54] = 0.0;
			
		}
		for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
			fRec53[l55] = 0.0;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fRec54[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec55[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec56[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec57[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec58[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec59[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fRec60[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec61[l63] = 0.0;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec62[l64] = 0.0;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec63[l65] = 0.0;
			
		}
		for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
			fRec64[l66] = 0.0;
			
		}
		for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
			fRec65[l67] = 0.0;
			
		}
		for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
			fRec66[l68] = 0.0;
			
		}
		for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
			fRec67[l69] = 0.0;
			
		}
		for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
			fRec68[l70] = 0.0;
			
		}
		for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
			fRec69[l71] = 0.0;
			
		}
		for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
			fRec70[l72] = 0.0;
			
		}
		for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
			fRec71[l73] = 0.0;
			
		}
		for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
			fRec72[l74] = 0.0;
			
		}
		for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
			fRec73[l75] = 0.0;
			
		}
		for (int l76 = 0; (l76 < 2); l76 = (l76 + 1)) {
			fRec74[l76] = 0.0;
			
		}
		for (int l77 = 0; (l77 < 2); l77 = (l77 + 1)) {
			fRec75[l77] = 0.0;
			
		}
		for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
			fRec76[l78] = 0.0;
			
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
		ui_interface->openVerticalBox("HOAPanLebedev503");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fHslider2, "0+1", "");
		ui_interface->declare(&fHslider2, "osc", "/gain_0 -20 20");
		ui_interface->declare(&fHslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  0", &fHslider2, 0.0, -30.0, 20.0, 0.10000000000000001);
		ui_interface->declare(&fHslider1, "0+2", "");
		ui_interface->declare(&fHslider1, "osc", "/radius_0 0.5 50");
		ui_interface->declare(&fHslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  0", &fHslider1, 2.0, 0.5, 50.0, 0.01);
		ui_interface->declare(&fHslider3, "0+3", "");
		ui_interface->declare(&fHslider3, "osc", "/azimuth_0 0 360");
		ui_interface->declare(&fHslider3, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth  0", &fHslider3, 0.0, -3.1415926535897931, 3.1415926535897931, 0.10000000000000001);
		ui_interface->declare(&fHslider0, "0+4", "");
		ui_interface->declare(&fHslider0, "osc", "/elevation_0 -90 90");
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation  0", &fHslider0, 0.0, -1.5707963267948966, 1.5707963267948966, 0.10000000000000001);
		ui_interface->declare(0, "0+5", "");
		ui_interface->openHorizontalBox("Spherical Wave");
		ui_interface->addCheckButton("Yes", &fCheckbox1);
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openVerticalBox("Mute Order");
		ui_interface->addCheckButton("0", &fCheckbox3);
		ui_interface->addCheckButton("1", &fCheckbox0);
		ui_interface->addCheckButton("2", &fCheckbox4);
		ui_interface->addCheckButton("3", &fCheckbox2);
		ui_interface->closeBox();
		ui_interface->declare(&fEntry0, "~", "");
		ui_interface->addNumEntry("Speaker Radius", &fEntry0, 1.0700000000000001, 0.5, 10.0, 0.01);
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openVerticalBox("Outputs");
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("1-25");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph0, "osc", "/output1 -70 6");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2392eb0", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "osc", "/output2 -70 6");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x239d600", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph2, "osc", "/output3 -70 6");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23a5f90", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph3, "osc", "/output4 -70 6");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23aea00", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph4, "osc", "/output5 -70 6");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23b7550", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph5, "osc", "/output6 -70 6");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23bf940", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph6, "osc", "/output7 -70 6");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23c87b0", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph7, "osc", "/output8 -70 6");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23d0da0", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph8, "osc", "/output9 -70 6");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23d9470", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph9, "osc", "/output10 -70 6");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23e1c20", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph10, "osc", "/output11 -70 6");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23eacb0", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph11, "osc", "/output12 -70 6");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23f3e20", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph12, "osc", "/output13 -70 6");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23fd070", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph13, "osc", "/output14 -70 6");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24063a0", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph14, "osc", "/output15 -70 6");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x240ebc0", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph15, "osc", "/output16 -70 6");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24174c0", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph16, "osc", "/output17 -70 6");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x241fea0", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph17, "osc", "/output18 -70 6");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2428960", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph18, "osc", "/output19 -70 6");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2432250", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph19, "osc", "/output20 -70 6");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x243b2c0", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph20, "osc", "/output21 -70 6");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2444410", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph21, "osc", "/output22 -70 6");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x244d640", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph22, "osc", "/output23 -70 6");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2456560", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph23, "osc", "/output24 -70 6");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x245f560", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph24, "osc", "/output25 -70 6");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2468640", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph25, "osc", "/output26 -70 6");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2471800", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph26, "osc", "/output27 -70 6");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x247ada0", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph27, "osc", "/output28 -70 6");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2483010", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph28, "osc", "/output29 -70 6");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x248b360", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph29, "osc", "/output30 -70 6");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2493790", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph30, "osc", "/output31 -70 6");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x249ce00", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph31, "osc", "/output32 -70 6");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24a5dc0", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph32, "osc", "/output33 -70 6");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24aee60", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph33, "osc", "/output34 -70 6");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24b7fe0", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph34, "osc", "/output35 -70 6");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24c1240", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph35, "osc", "/output36 -70 6");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24ca580", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph36, "osc", "/output37 -70 6");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24d39a0", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph37, "osc", "/output38 -70 6");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24dcea0", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph38, "osc", "/output39 -70 6");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24e56c0", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph39, "osc", "/output40 -70 6");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24edfc0", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph40, "osc", "/output41 -70 6");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24f69a0", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph41, "osc", "/output42 -70 6");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24ff460", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph42, "osc", "/output43 -70 6");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2508000", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph43, "osc", "/output44 -70 6");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2510c80", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph44, "osc", "/output45 -70 6");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x25199e0", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph45, "osc", "/output46 -70 6");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2522820", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph46, "osc", "/output47 -70 6");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x252b740", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph47, "osc", "/output48 -70 6");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2534740", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph48, "osc", "/output49 -70 6");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x253d820", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph49, "osc", "/output50 -70 6");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x25469e0", &fVbargraph49, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
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
		double fSlow0 = double(fHslider0);
		double fSlow1 = sin(fSlow0);
		double fSlow2 = (3.0 * (1.0 - double(fCheckbox0)));
		double fSlow3 = double(fHslider1);
		double fSlow4 = (fConst3 / fSlow3);
		double fSlow5 = (fSlow4 + 1.0);
		double fSlow6 = (fConst2 / (fSlow5 * fSlow3));
		double fSlow7 = double(fEntry0);
		double fSlow8 = (fConst3 / fSlow7);
		double fSlow9 = (1.0 / (fSlow8 + 1.0));
		double fSlow10 = double(fCheckbox1);
		double fSlow11 = (((fSlow5 * fSlow10) * fSlow7) / fSlow3);
		double fSlow12 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fHslider2))));
		double fSlow13 = (fConst2 / fSlow7);
		double fSlow14 = (1.0 - fSlow10);
		double fSlow15 = (fSlow14 * fSlow7);
		double fSlow16 = ((3.0 * mydsp_faustpower2_f(fSlow1)) + -1.0);
		double fSlow17 = (1.0 - double(fCheckbox2));
		double fSlow18 = (2.333333333333333 * (((2.5 * fSlow16) + -2.0) * fSlow17));
		double fSlow19 = ((fConst5 / fSlow3) + 1.0);
		double fSlow20 = (fConst4 / (fSlow19 * fSlow3));
		double fSlow21 = (1.0 / ((fConst5 / fSlow7) + 1.0));
		double fSlow22 = mydsp_faustpower2_f(fSlow8);
		double fSlow23 = (1.0 / (((fConst6 / fSlow7) + (6.4594326934833797 * fSlow22)) + 1.0));
		double fSlow24 = mydsp_faustpower2_f(fSlow4);
		double fSlow25 = (((fConst6 / fSlow3) + (6.4594326934833797 * fSlow24)) + 1.0);
		double fSlow26 = (((fSlow25 * fSlow10) * fSlow7) / fSlow3);
		double fSlow27 = (25.837730773933519 * fSlow22);
		double fSlow28 = (fSlow27 + (fConst7 / fSlow7));
		double fSlow29 = (1.0 / fSlow25);
		double fSlow30 = (25.837730773933519 * fSlow24);
		double fSlow31 = ((fConst7 / fSlow3) + fSlow30);
		double fSlow32 = (fConst4 / fSlow7);
		double fSlow33 = ((((fSlow10 / fSlow3) + fSlow14) * fSlow7) * (1.0 - double(fCheckbox3)));
		double fSlow34 = (1.0 - double(fCheckbox4));
		double fSlow35 = (2.5 * (fSlow16 * fSlow34));
		double fSlow36 = (((fConst8 / fSlow3) + (3.0 * fSlow24)) + 1.0);
		double fSlow37 = (1.0 / fSlow36);
		double fSlow38 = (12.0 * fSlow24);
		double fSlow39 = ((fConst9 / fSlow3) + fSlow38);
		double fSlow40 = (1.0 / (((fConst8 / fSlow7) + (3.0 * fSlow22)) + 1.0));
		double fSlow41 = (((fSlow36 * fSlow10) * fSlow7) / fSlow3);
		double fSlow42 = (12.0 * fSlow22);
		double fSlow43 = (fSlow42 + (fConst9 / fSlow7));
		double fSlow44 = double(fHslider3);
		double fSlow45 = cos(fSlow44);
		double fSlow46 = cos(fSlow0);
		double fSlow47 = (fSlow45 * fSlow46);
		double fSlow48 = mydsp_faustpower2_f(fSlow46);
		double fSlow49 = ((3.0 * (mydsp_faustpower2_f(fSlow45) * fSlow48)) + -1.0);
		double fSlow50 = (2.333333333333333 * (((2.5 * fSlow49) + -2.0) * fSlow17));
		double fSlow51 = (2.5 * (fSlow49 * fSlow34));
		double fSlow52 = cos((fSlow44 + -1.5707963267948966));
		double fSlow53 = (fSlow52 * fSlow46);
		double fSlow54 = ((3.0 * (mydsp_faustpower2_f(fSlow52) * fSlow48)) + -1.0);
		double fSlow55 = (2.333333333333333 * (((2.5 * fSlow54) + -2.0) * fSlow17));
		double fSlow56 = (2.5 * (fSlow54 * fSlow34));
		double fSlow57 = cos((fSlow44 + -3.1415926535897931));
		double fSlow58 = (fSlow57 * fSlow46);
		double fSlow59 = ((3.0 * (mydsp_faustpower2_f(fSlow57) * fSlow48)) + -1.0);
		double fSlow60 = (2.333333333333333 * (((2.5 * fSlow59) + -2.0) * fSlow17));
		double fSlow61 = (2.5 * (fSlow59 * fSlow34));
		double fSlow62 = cos((fSlow44 + -4.7123889803846897));
		double fSlow63 = (fSlow62 * fSlow46);
		double fSlow64 = ((3.0 * (mydsp_faustpower2_f(fSlow62) * fSlow48)) + -1.0);
		double fSlow65 = (2.333333333333333 * (((2.5 * fSlow64) + -2.0) * fSlow17));
		double fSlow66 = (2.5 * (fSlow64 * fSlow34));
		double fSlow67 = ((6.123233995736766e-17 * fSlow47) - fSlow1);
		double fSlow68 = ((3.0 * mydsp_faustpower2_f(fSlow67)) + -1.0);
		double fSlow69 = (2.333333333333333 * (((2.5 * fSlow68) + -2.0) * fSlow17));
		double fSlow70 = (2.5 * (fSlow68 * fSlow34));
		double fSlow71 = (0.70710678118654757 * fSlow47);
		double fSlow72 = (0.70710678118654746 * fSlow1);
		double fSlow73 = (fSlow71 + fSlow72);
		double fSlow74 = ((3.0 * mydsp_faustpower2_f(fSlow73)) + -1.0);
		double fSlow75 = (2.333333333333333 * (((2.5 * fSlow74) + -2.0) * fSlow17));
		double fSlow76 = (2.5 * (fSlow74 * fSlow34));
		double fSlow77 = (0.70710678118654757 * fSlow53);
		double fSlow78 = (fSlow72 + fSlow77);
		double fSlow79 = ((3.0 * mydsp_faustpower2_f(fSlow78)) + -1.0);
		double fSlow80 = (2.333333333333333 * (((2.5 * fSlow79) + -2.0) * fSlow17));
		double fSlow81 = (2.5 * (fSlow79 * fSlow34));
		double fSlow82 = (0.70710678118654757 * fSlow58);
		double fSlow83 = (fSlow72 + fSlow82);
		double fSlow84 = ((3.0 * mydsp_faustpower2_f(fSlow83)) + -1.0);
		double fSlow85 = (2.5 * (fSlow84 * fSlow34));
		double fSlow86 = (2.333333333333333 * (((2.5 * fSlow84) + -2.0) * fSlow17));
		double fSlow87 = (0.70710678118654757 * fSlow63);
		double fSlow88 = (fSlow72 + fSlow87);
		double fSlow89 = ((3.0 * mydsp_faustpower2_f(fSlow88)) + -1.0);
		double fSlow90 = (2.333333333333333 * (((2.5 * fSlow89) + -2.0) * fSlow17));
		double fSlow91 = (2.5 * (fSlow89 * fSlow34));
		double fSlow92 = cos((fSlow44 + -0.78539816339744828));
		double fSlow93 = (fSlow92 * fSlow46);
		double fSlow94 = ((3.0 * (mydsp_faustpower2_f(fSlow92) * fSlow48)) + -1.0);
		double fSlow95 = (2.333333333333333 * (((2.5 * fSlow94) + -2.0) * fSlow17));
		double fSlow96 = (2.5 * (fSlow94 * fSlow34));
		double fSlow97 = cos((fSlow44 + -2.3561944901923448));
		double fSlow98 = (fSlow97 * fSlow46);
		double fSlow99 = ((3.0 * (mydsp_faustpower2_f(fSlow97) * fSlow48)) + -1.0);
		double fSlow100 = (2.333333333333333 * (((2.5 * fSlow99) + -2.0) * fSlow17));
		double fSlow101 = (2.5 * (fSlow99 * fSlow34));
		double fSlow102 = cos((fSlow44 + -3.9269908169872414));
		double fSlow103 = (fSlow102 * fSlow46);
		double fSlow104 = ((3.0 * (mydsp_faustpower2_f(fSlow102) * fSlow48)) + -1.0);
		double fSlow105 = (2.333333333333333 * (((2.5 * fSlow104) + -2.0) * fSlow17));
		double fSlow106 = (2.5 * (fSlow104 * fSlow34));
		double fSlow107 = cos((fSlow44 + -5.497787143782138));
		double fSlow108 = (fSlow107 * fSlow46);
		double fSlow109 = ((3.0 * (mydsp_faustpower2_f(fSlow107) * fSlow48)) + -1.0);
		double fSlow110 = (2.333333333333333 * (((2.5 * fSlow109) + -2.0) * fSlow17));
		double fSlow111 = (2.5 * (fSlow109 * fSlow34));
		double fSlow112 = (fSlow71 - fSlow72);
		double fSlow113 = ((3.0 * mydsp_faustpower2_f(fSlow112)) + -1.0);
		double fSlow114 = (2.333333333333333 * (((2.5 * fSlow113) + -2.0) * fSlow17));
		double fSlow115 = (2.5 * (fSlow113 * fSlow34));
		double fSlow116 = (fSlow77 - fSlow72);
		double fSlow117 = ((3.0 * mydsp_faustpower2_f(fSlow116)) + -1.0);
		double fSlow118 = (2.333333333333333 * (((2.5 * fSlow117) + -2.0) * fSlow17));
		double fSlow119 = (2.5 * (fSlow117 * fSlow34));
		double fSlow120 = (fSlow82 - fSlow72);
		double fSlow121 = ((3.0 * mydsp_faustpower2_f(fSlow120)) + -1.0);
		double fSlow122 = (2.333333333333333 * (((2.5 * fSlow121) + -2.0) * fSlow17));
		double fSlow123 = (2.5 * (fSlow121 * fSlow34));
		double fSlow124 = (fSlow87 - fSlow72);
		double fSlow125 = ((3.0 * mydsp_faustpower2_f(fSlow124)) + -1.0);
		double fSlow126 = (2.333333333333333 * (((2.5 * fSlow125) + -2.0) * fSlow17));
		double fSlow127 = (2.5 * (fSlow125 * fSlow34));
		double fSlow128 = (0.81649658092772592 * fSlow93);
		double fSlow129 = (0.57735026918962584 * fSlow1);
		double fSlow130 = (fSlow128 + fSlow129);
		double fSlow131 = ((3.0 * mydsp_faustpower2_f(fSlow130)) + -1.0);
		double fSlow132 = (2.333333333333333 * (((2.5 * fSlow131) + -2.0) * fSlow17));
		double fSlow133 = (2.5 * (fSlow131 * fSlow34));
		double fSlow134 = (0.81649658092772592 * fSlow98);
		double fSlow135 = (fSlow129 + fSlow134);
		double fSlow136 = ((3.0 * mydsp_faustpower2_f(fSlow135)) + -1.0);
		double fSlow137 = (2.333333333333333 * (((2.5 * fSlow136) + -2.0) * fSlow17));
		double fSlow138 = (2.5 * (fSlow136 * fSlow34));
		double fSlow139 = (0.81649658092772592 * fSlow103);
		double fSlow140 = (fSlow129 + fSlow139);
		double fSlow141 = ((3.0 * mydsp_faustpower2_f(fSlow140)) + -1.0);
		double fSlow142 = (2.333333333333333 * (((2.5 * fSlow141) + -2.0) * fSlow17));
		double fSlow143 = (2.5 * (fSlow141 * fSlow34));
		double fSlow144 = (0.81649658092772592 * fSlow108);
		double fSlow145 = (fSlow129 + fSlow144);
		double fSlow146 = ((3.0 * mydsp_faustpower2_f(fSlow145)) + -1.0);
		double fSlow147 = (2.333333333333333 * (((2.5 * fSlow146) + -2.0) * fSlow17));
		double fSlow148 = (2.5 * (fSlow146 * fSlow34));
		double fSlow149 = (fSlow128 - fSlow129);
		double fSlow150 = ((3.0 * mydsp_faustpower2_f(fSlow149)) + -1.0);
		double fSlow151 = (2.333333333333333 * (((2.5 * fSlow150) + -2.0) * fSlow17));
		double fSlow152 = (2.5 * (fSlow150 * fSlow34));
		double fSlow153 = (fSlow134 - fSlow129);
		double fSlow154 = ((3.0 * mydsp_faustpower2_f(fSlow153)) + -1.0);
		double fSlow155 = (2.333333333333333 * (((2.5 * fSlow154) + -2.0) * fSlow17));
		double fSlow156 = (2.5 * (fSlow154 * fSlow34));
		double fSlow157 = (fSlow139 - fSlow129);
		double fSlow158 = ((3.0 * mydsp_faustpower2_f(fSlow157)) + -1.0);
		double fSlow159 = (2.333333333333333 * (((2.5 * fSlow158) + -2.0) * fSlow17));
		double fSlow160 = (2.5 * (fSlow158 * fSlow34));
		double fSlow161 = (fSlow144 - fSlow129);
		double fSlow162 = ((3.0 * mydsp_faustpower2_f(fSlow161)) + -1.0);
		double fSlow163 = (2.333333333333333 * (((2.5 * fSlow162) + -2.0) * fSlow17));
		double fSlow164 = (2.5 * (fSlow162 * fSlow34));
		double fSlow165 = (0.42640143271122027 * (cos((fSlow44 + -0.78539816339744839)) * fSlow46));
		double fSlow166 = (0.90453403373329111 * fSlow1);
		double fSlow167 = (fSlow165 + fSlow166);
		double fSlow168 = ((3.0 * mydsp_faustpower2_f(fSlow167)) + -1.0);
		double fSlow169 = (2.333333333333333 * (((2.5 * fSlow168) + -2.0) * fSlow17));
		double fSlow170 = (2.5 * (fSlow168 * fSlow34));
		double fSlow171 = (0.42640143271122027 * fSlow98);
		double fSlow172 = (fSlow166 + fSlow171);
		double fSlow173 = ((3.0 * mydsp_faustpower2_f(fSlow172)) + -1.0);
		double fSlow174 = (2.333333333333333 * (((2.5 * fSlow173) + -2.0) * fSlow17));
		double fSlow175 = (2.5 * (fSlow173 * fSlow34));
		double fSlow176 = (0.42640143271122027 * fSlow103);
		double fSlow177 = (fSlow166 + fSlow176);
		double fSlow178 = ((3.0 * mydsp_faustpower2_f(fSlow177)) + -1.0);
		double fSlow179 = (2.333333333333333 * (((2.5 * fSlow178) + -2.0) * fSlow17));
		double fSlow180 = (2.5 * (fSlow178 * fSlow34));
		double fSlow181 = (0.42640143271122027 * fSlow108);
		double fSlow182 = (fSlow166 + fSlow181);
		double fSlow183 = ((3.0 * mydsp_faustpower2_f(fSlow182)) + -1.0);
		double fSlow184 = (2.333333333333333 * (((2.5 * fSlow183) + -2.0) * fSlow17));
		double fSlow185 = (2.5 * (fSlow183 * fSlow34));
		double fSlow186 = (0.95346258924559235 * (cos((fSlow44 + -0.32175055439664263)) * fSlow46));
		double fSlow187 = (0.30151134457776352 * fSlow1);
		double fSlow188 = (fSlow186 + fSlow187);
		double fSlow189 = ((3.0 * mydsp_faustpower2_f(fSlow188)) + -1.0);
		double fSlow190 = (2.333333333333333 * (((2.5 * fSlow189) + -2.0) * fSlow17));
		double fSlow191 = (2.5 * (fSlow189 * fSlow34));
		double fSlow192 = (0.95346258924559235 * (cos((fSlow44 + -1.2490457723982544)) * fSlow46));
		double fSlow193 = (fSlow187 + fSlow192);
		double fSlow194 = ((3.0 * mydsp_faustpower2_f(fSlow193)) + -1.0);
		double fSlow195 = (2.333333333333333 * (((2.5 * fSlow194) + -2.0) * fSlow17));
		double fSlow196 = (2.5 * (fSlow194 * fSlow34));
		double fSlow197 = (0.95346258924559235 * (cos((fSlow44 + -1.8925468811915387)) * fSlow46));
		double fSlow198 = (fSlow187 + fSlow197);
		double fSlow199 = ((3.0 * mydsp_faustpower2_f(fSlow198)) + -1.0);
		double fSlow200 = (2.333333333333333 * (((2.5 * fSlow199) + -2.0) * fSlow17));
		double fSlow201 = (2.5 * (fSlow199 * fSlow34));
		double fSlow202 = (0.95346258924559235 * (cos((fSlow44 + -2.8198420991931505)) * fSlow46));
		double fSlow203 = (fSlow187 + fSlow202);
		double fSlow204 = ((3.0 * mydsp_faustpower2_f(fSlow203)) + -1.0);
		double fSlow205 = (2.333333333333333 * (((2.5 * fSlow204) + -2.0) * fSlow17));
		double fSlow206 = (2.5 * (fSlow204 * fSlow34));
		double fSlow207 = (0.95346258924559235 * (cos((fSlow44 + -3.4633432079864357)) * fSlow46));
		double fSlow208 = (fSlow187 + fSlow207);
		double fSlow209 = ((3.0 * mydsp_faustpower2_f(fSlow208)) + -1.0);
		double fSlow210 = (2.333333333333333 * (((2.5 * fSlow209) + -2.0) * fSlow17));
		double fSlow211 = (2.5 * (fSlow209 * fSlow34));
		double fSlow212 = (0.95346258924559235 * (cos((fSlow44 + -4.3906384259880475)) * fSlow46));
		double fSlow213 = (fSlow187 + fSlow212);
		double fSlow214 = ((3.0 * mydsp_faustpower2_f(fSlow213)) + -1.0);
		double fSlow215 = (2.333333333333333 * (((2.5 * fSlow214) + -2.0) * fSlow17));
		double fSlow216 = (2.5 * (fSlow214 * fSlow34));
		double fSlow217 = (0.95346258924559235 * (cos((fSlow44 + -5.0341395347813318)) * fSlow46));
		double fSlow218 = (fSlow187 + fSlow217);
		double fSlow219 = ((3.0 * mydsp_faustpower2_f(fSlow218)) + -1.0);
		double fSlow220 = (2.333333333333333 * (((2.5 * fSlow219) + -2.0) * fSlow17));
		double fSlow221 = (2.5 * (fSlow219 * fSlow34));
		double fSlow222 = (0.95346258924559235 * (cos((fSlow44 + -5.9614347527829432)) * fSlow46));
		double fSlow223 = (fSlow187 + fSlow222);
		double fSlow224 = ((3.0 * mydsp_faustpower2_f(fSlow223)) + -1.0);
		double fSlow225 = (2.333333333333333 * (((2.5 * fSlow224) + -2.0) * fSlow17));
		double fSlow226 = (2.5 * (fSlow224 * fSlow34));
		double fSlow227 = (fSlow186 - fSlow187);
		double fSlow228 = ((3.0 * mydsp_faustpower2_f(fSlow227)) + -1.0);
		double fSlow229 = (2.333333333333333 * (((2.5 * fSlow228) + -2.0) * fSlow17));
		double fSlow230 = (2.5 * (fSlow228 * fSlow34));
		double fSlow231 = (fSlow192 - fSlow187);
		double fSlow232 = ((3.0 * mydsp_faustpower2_f(fSlow231)) + -1.0);
		double fSlow233 = (2.333333333333333 * (((2.5 * fSlow232) + -2.0) * fSlow17));
		double fSlow234 = (2.5 * (fSlow232 * fSlow34));
		double fSlow235 = (fSlow197 - fSlow187);
		double fSlow236 = ((3.0 * mydsp_faustpower2_f(fSlow235)) + -1.0);
		double fSlow237 = (2.333333333333333 * (((2.5 * fSlow236) + -2.0) * fSlow17));
		double fSlow238 = (2.5 * (fSlow236 * fSlow34));
		double fSlow239 = (fSlow202 - fSlow187);
		double fSlow240 = ((3.0 * mydsp_faustpower2_f(fSlow239)) + -1.0);
		double fSlow241 = (2.333333333333333 * (((2.5 * fSlow240) + -2.0) * fSlow17));
		double fSlow242 = (2.5 * (fSlow240 * fSlow34));
		double fSlow243 = (fSlow207 - fSlow187);
		double fSlow244 = ((3.0 * mydsp_faustpower2_f(fSlow243)) + -1.0);
		double fSlow245 = (2.333333333333333 * (((2.5 * fSlow244) + -2.0) * fSlow17));
		double fSlow246 = (2.5 * (fSlow244 * fSlow34));
		double fSlow247 = (fSlow212 - fSlow187);
		double fSlow248 = ((3.0 * mydsp_faustpower2_f(fSlow247)) + -1.0);
		double fSlow249 = (2.333333333333333 * (((2.5 * fSlow248) + -2.0) * fSlow17));
		double fSlow250 = (2.5 * (fSlow248 * fSlow34));
		double fSlow251 = (fSlow217 - fSlow187);
		double fSlow252 = ((3.0 * mydsp_faustpower2_f(fSlow251)) + -1.0);
		double fSlow253 = (2.333333333333333 * (((2.5 * fSlow252) + -2.0) * fSlow17));
		double fSlow254 = (2.5 * (fSlow252 * fSlow34));
		double fSlow255 = (fSlow222 - fSlow187);
		double fSlow256 = ((3.0 * mydsp_faustpower2_f(fSlow255)) + -1.0);
		double fSlow257 = (2.333333333333333 * (((2.5 * fSlow256) + -2.0) * fSlow17));
		double fSlow258 = (2.5 * (fSlow256 * fSlow34));
		double fSlow259 = (fSlow165 - fSlow166);
		double fSlow260 = ((3.0 * mydsp_faustpower2_f(fSlow259)) + -1.0);
		double fSlow261 = (2.333333333333333 * (((2.5 * fSlow260) + -2.0) * fSlow17));
		double fSlow262 = (2.5 * (fSlow260 * fSlow34));
		double fSlow263 = (fSlow171 - fSlow166);
		double fSlow264 = ((3.0 * mydsp_faustpower2_f(fSlow263)) + -1.0);
		double fSlow265 = (2.333333333333333 * (((2.5 * fSlow264) + -2.0) * fSlow17));
		double fSlow266 = (2.5 * (fSlow264 * fSlow34));
		double fSlow267 = (fSlow176 - fSlow166);
		double fSlow268 = ((3.0 * mydsp_faustpower2_f(fSlow267)) + -1.0);
		double fSlow269 = (2.333333333333333 * (((2.5 * fSlow268) + -2.0) * fSlow17));
		double fSlow270 = (2.5 * (fSlow268 * fSlow34));
		double fSlow271 = (fSlow181 - fSlow166);
		double fSlow272 = ((3.0 * mydsp_faustpower2_f(fSlow271)) + -1.0);
		double fSlow273 = (2.333333333333333 * (((2.5 * fSlow272) + -2.0) * fSlow17));
		double fSlow274 = (2.5 * (fSlow272 * fSlow34));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec3[0] = (fSlow12 + (0.999 * fRec3[1]));
			double fTemp0 = (fRec3[0] * double(input0[i]));
			fRec4[0] = (fRec4[1] + fRec2[1]);
			fRec2[0] = (fSlow9 * ((fSlow11 * fTemp0) - (fSlow13 * fRec4[0])));
			fRec1[0] = (fRec2[0] + fRec1[1]);
			double fTemp1 = (fSlow15 * fTemp0);
			fRec6[0] = (fRec6[1] + fRec5[1]);
			fRec5[0] = (fSlow9 * (fTemp1 - (fSlow13 * fRec6[0])));
			double fTemp2 = (fSlow2 * ((fSlow6 * (fRec1[0] - fRec2[0])) + (fRec2[0] + fRec5[0])));
			fRec10[0] = (fRec10[1] + fRec9[1]);
			fRec11[0] = (fRec11[1] + fRec10[1]);
			fRec9[0] = (fSlow23 * ((fSlow26 * fTemp0) - ((fSlow28 * fRec10[0]) + (fSlow27 * fRec11[0]))));
			fRec12[0] = (fRec9[0] + fRec12[1]);
			double fTemp3 = (fRec12[0] - fRec9[0]);
			fVec0[0] = fTemp3;
			fRec13[0] = (fRec13[1] + fVec0[1]);
			fRec14[0] = (fRec14[1] + fRec8[1]);
			fRec8[0] = (fSlow21 * ((fSlow19 * (fRec9[0] + (fSlow29 * ((fSlow31 * fTemp3) + (fSlow30 * fRec13[0]))))) - (fSlow32 * fRec14[0])));
			fRec7[0] = (fRec8[0] + fRec7[1]);
			fRec17[0] = (fRec17[1] + fRec16[1]);
			fRec18[0] = (fRec18[1] + fRec17[1]);
			fRec16[0] = (fSlow23 * (fTemp1 - ((fSlow28 * fRec17[0]) + (fSlow27 * fRec18[0]))));
			fRec19[0] = (fRec19[1] + fRec15[1]);
			fRec15[0] = (fSlow21 * (fRec16[0] - (fSlow32 * fRec19[0])));
			double fTemp4 = ((fSlow20 * (fRec7[0] - fRec8[0])) + (fRec8[0] + fRec15[0]));
			double fTemp5 = (fSlow33 * fTemp0);
			fRec22[0] = (fRec22[1] + fRec21[1]);
			fRec23[0] = (fRec23[1] + fRec22[1]);
			fRec21[0] = (fSlow40 * ((fSlow41 * fTemp0) - ((fSlow43 * fRec22[0]) + (fSlow42 * fRec23[0]))));
			fRec20[0] = (fRec21[0] + fRec20[1]);
			double fTemp6 = (fRec20[0] - fRec21[0]);
			fVec1[0] = fTemp6;
			fRec24[0] = (fRec24[1] + fVec1[1]);
			fRec26[0] = (fRec26[1] + fRec25[1]);
			fRec27[0] = (fRec27[1] + fRec26[1]);
			fRec25[0] = (fSlow40 * (fTemp1 - ((fSlow43 * fRec26[0]) + (fSlow42 * fRec27[0]))));
			double fTemp7 = ((fSlow37 * ((fSlow39 * fTemp6) + (fSlow38 * fRec24[0]))) + (fRec21[0] + fRec25[0]));
			double fTemp8 = (0.012698412698412698 * ((fSlow1 * (fTemp2 + (fSlow18 * fTemp4))) + (fTemp5 + (fSlow35 * fTemp7))));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fVbargraph0 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp8);
			double fTemp9 = (0.012698412698412698 * ((fSlow47 * (fTemp2 + (fSlow50 * fTemp4))) + (fTemp5 + (fSlow51 * fTemp7))));
			fRec28[0] = max((fRec28[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph1 = FAUSTFLOAT(fRec28[0]);
			output1[i] = FAUSTFLOAT(fTemp9);
			double fTemp10 = (0.012698412698412698 * ((fSlow53 * (fTemp2 + (fSlow55 * fTemp4))) + (fTemp5 + (fSlow56 * fTemp7))));
			fRec29[0] = max((fRec29[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fVbargraph2 = FAUSTFLOAT(fRec29[0]);
			output2[i] = FAUSTFLOAT(fTemp10);
			double fTemp11 = (0.012698412698412698 * ((fSlow58 * (fTemp2 + (fSlow60 * fTemp4))) + (fTemp5 + (fSlow61 * fTemp7))));
			fRec30[0] = max((fRec30[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fVbargraph3 = FAUSTFLOAT(fRec30[0]);
			output3[i] = FAUSTFLOAT(fTemp11);
			double fTemp12 = (0.012698412698412698 * ((fSlow63 * (fTemp2 + (fSlow65 * fTemp4))) + (fTemp5 + (fSlow66 * fTemp7))));
			fRec31[0] = max((fRec31[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph4 = FAUSTFLOAT(fRec31[0]);
			output4[i] = FAUSTFLOAT(fTemp12);
			double fTemp13 = (0.012698412698412698 * ((fSlow67 * (fTemp2 + (fSlow69 * fTemp4))) + (fTemp5 + (fSlow70 * fTemp7))));
			fRec32[0] = max((fRec32[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fVbargraph5 = FAUSTFLOAT(fRec32[0]);
			output5[i] = FAUSTFLOAT(fTemp13);
			double fTemp14 = (0.022574955908289243 * ((fSlow73 * (fTemp2 + (fSlow75 * fTemp4))) + (fTemp5 + (fSlow76 * fTemp7))));
			fRec33[0] = max((fRec33[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fVbargraph6 = FAUSTFLOAT(fRec33[0]);
			output6[i] = FAUSTFLOAT(fTemp14);
			double fTemp15 = (0.022574955908289243 * ((fSlow78 * (fTemp2 + (fSlow80 * fTemp4))) + (fTemp5 + (fSlow81 * fTemp7))));
			fRec34[0] = max((fRec34[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph7 = FAUSTFLOAT(fRec34[0]);
			output7[i] = FAUSTFLOAT(fTemp15);
			double fTemp16 = (0.022574955908289243 * ((fTemp5 + (fSlow85 * fTemp7)) + (fSlow83 * (fTemp2 + (fSlow86 * fTemp4)))));
			fRec35[0] = max((fRec35[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph8 = FAUSTFLOAT(fRec35[0]);
			output8[i] = FAUSTFLOAT(fTemp16);
			double fTemp17 = (0.022574955908289243 * ((fSlow88 * (fTemp2 + (fSlow90 * fTemp4))) + (fTemp5 + (fSlow91 * fTemp7))));
			fRec36[0] = max((fRec36[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph9 = FAUSTFLOAT(fRec36[0]);
			output9[i] = FAUSTFLOAT(fTemp17);
			double fTemp18 = (0.022574955908289243 * ((fSlow93 * (fTemp2 + (fSlow95 * fTemp4))) + (fTemp5 + (fSlow96 * fTemp7))));
			fRec37[0] = max((fRec37[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph10 = FAUSTFLOAT(fRec37[0]);
			output10[i] = FAUSTFLOAT(fTemp18);
			double fTemp19 = (0.022574955908289243 * ((fSlow98 * (fTemp2 + (fSlow100 * fTemp4))) + (fTemp5 + (fSlow101 * fTemp7))));
			fRec38[0] = max((fRec38[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph11 = FAUSTFLOAT(fRec38[0]);
			output11[i] = FAUSTFLOAT(fTemp19);
			double fTemp20 = (0.022574955908289243 * ((fSlow103 * (fTemp2 + (fSlow105 * fTemp4))) + (fTemp5 + (fSlow106 * fTemp7))));
			fRec39[0] = max((fRec39[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph12 = FAUSTFLOAT(fRec39[0]);
			output12[i] = FAUSTFLOAT(fTemp20);
			double fTemp21 = (0.022574955908289243 * ((fSlow108 * (fTemp2 + (fSlow110 * fTemp4))) + (fTemp5 + (fSlow111 * fTemp7))));
			fRec40[0] = max((fRec40[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph13 = FAUSTFLOAT(fRec40[0]);
			output13[i] = FAUSTFLOAT(fTemp21);
			double fTemp22 = (0.022574955908289243 * ((fSlow112 * (fTemp2 + (fSlow114 * fTemp4))) + (fTemp5 + (fSlow115 * fTemp7))));
			fRec41[0] = max((fRec41[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph14 = FAUSTFLOAT(fRec41[0]);
			output14[i] = FAUSTFLOAT(fTemp22);
			double fTemp23 = (0.022574955908289243 * ((fSlow116 * (fTemp2 + (fSlow118 * fTemp4))) + (fTemp5 + (fSlow119 * fTemp7))));
			fRec42[0] = max((fRec42[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fVbargraph15 = FAUSTFLOAT(fRec42[0]);
			output15[i] = FAUSTFLOAT(fTemp23);
			double fTemp24 = (0.022574955908289243 * ((fSlow120 * (fTemp2 + (fSlow122 * fTemp4))) + (fTemp5 + (fSlow123 * fTemp7))));
			fRec43[0] = max((fRec43[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph16 = FAUSTFLOAT(fRec43[0]);
			output16[i] = FAUSTFLOAT(fTemp24);
			double fTemp25 = (0.022574955908289243 * ((fSlow124 * (fTemp2 + (fSlow126 * fTemp4))) + (fTemp5 + (fSlow127 * fTemp7))));
			fRec44[0] = max((fRec44[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph17 = FAUSTFLOAT(fRec44[0]);
			output17[i] = FAUSTFLOAT(fTemp25);
			double fTemp26 = (0.021093750000000001 * ((fSlow130 * (fTemp2 + (fSlow132 * fTemp4))) + (fTemp5 + (fSlow133 * fTemp7))));
			fRec45[0] = max((fRec45[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fVbargraph18 = FAUSTFLOAT(fRec45[0]);
			output18[i] = FAUSTFLOAT(fTemp26);
			double fTemp27 = (0.021093750000000001 * ((fSlow135 * (fTemp2 + (fSlow137 * fTemp4))) + (fTemp5 + (fSlow138 * fTemp7))));
			fRec46[0] = max((fRec46[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph19 = FAUSTFLOAT(fRec46[0]);
			output19[i] = FAUSTFLOAT(fTemp27);
			double fTemp28 = (0.021093750000000001 * ((fSlow140 * (fTemp2 + (fSlow142 * fTemp4))) + (fTemp5 + (fSlow143 * fTemp7))));
			fRec47[0] = max((fRec47[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fVbargraph20 = FAUSTFLOAT(fRec47[0]);
			output20[i] = FAUSTFLOAT(fTemp28);
			double fTemp29 = (0.021093750000000001 * ((fSlow145 * (fTemp2 + (fSlow147 * fTemp4))) + (fTemp5 + (fSlow148 * fTemp7))));
			fRec48[0] = max((fRec48[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph21 = FAUSTFLOAT(fRec48[0]);
			output21[i] = FAUSTFLOAT(fTemp29);
			double fTemp30 = (0.021093750000000001 * ((fSlow149 * (fTemp2 + (fSlow151 * fTemp4))) + (fTemp5 + (fSlow152 * fTemp7))));
			fRec49[0] = max((fRec49[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph22 = FAUSTFLOAT(fRec49[0]);
			output22[i] = FAUSTFLOAT(fTemp30);
			double fTemp31 = (0.021093750000000001 * ((fSlow153 * (fTemp2 + (fSlow155 * fTemp4))) + (fTemp5 + (fSlow156 * fTemp7))));
			fRec50[0] = max((fRec50[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph23 = FAUSTFLOAT(fRec50[0]);
			output23[i] = FAUSTFLOAT(fTemp31);
			double fTemp32 = (0.021093750000000001 * ((fSlow157 * (fTemp2 + (fSlow159 * fTemp4))) + (fTemp5 + (fSlow160 * fTemp7))));
			fRec51[0] = max((fRec51[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph24 = FAUSTFLOAT(fRec51[0]);
			output24[i] = FAUSTFLOAT(fTemp32);
			double fTemp33 = (0.021093750000000001 * ((fSlow161 * (fTemp2 + (fSlow163 * fTemp4))) + (fTemp5 + (fSlow164 * fTemp7))));
			fRec52[0] = max((fRec52[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph25 = FAUSTFLOAT(fRec52[0]);
			output25[i] = FAUSTFLOAT(fTemp33);
			double fTemp34 = (0.02017333553791887 * ((fSlow167 * (fTemp2 + (fSlow169 * fTemp4))) + (fTemp5 + (fSlow170 * fTemp7))));
			fRec53[0] = max((fRec53[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fVbargraph26 = FAUSTFLOAT(fRec53[0]);
			output26[i] = FAUSTFLOAT(fTemp34);
			double fTemp35 = (0.02017333553791887 * ((fSlow172 * (fTemp2 + (fSlow174 * fTemp4))) + (fTemp5 + (fSlow175 * fTemp7))));
			fRec54[0] = max((fRec54[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph27 = FAUSTFLOAT(fRec54[0]);
			output27[i] = FAUSTFLOAT(fTemp35);
			double fTemp36 = (0.02017333553791887 * ((fSlow177 * (fTemp2 + (fSlow179 * fTemp4))) + (fTemp5 + (fSlow180 * fTemp7))));
			fRec55[0] = max((fRec55[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fVbargraph28 = FAUSTFLOAT(fRec55[0]);
			output28[i] = FAUSTFLOAT(fTemp36);
			double fTemp37 = (0.02017333553791887 * ((fSlow182 * (fTemp2 + (fSlow184 * fTemp4))) + (fTemp5 + (fSlow185 * fTemp7))));
			fRec56[0] = max((fRec56[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fVbargraph29 = FAUSTFLOAT(fRec56[0]);
			output29[i] = FAUSTFLOAT(fTemp37);
			double fTemp38 = (0.02017333553791887 * ((fSlow188 * (fTemp2 + (fSlow190 * fTemp4))) + (fTemp5 + (fSlow191 * fTemp7))));
			fRec57[0] = max((fRec57[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fVbargraph30 = FAUSTFLOAT(fRec57[0]);
			output30[i] = FAUSTFLOAT(fTemp38);
			double fTemp39 = (0.02017333553791887 * ((fSlow193 * (fTemp2 + (fSlow195 * fTemp4))) + (fTemp5 + (fSlow196 * fTemp7))));
			fRec58[0] = max((fRec58[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph31 = FAUSTFLOAT(fRec58[0]);
			output31[i] = FAUSTFLOAT(fTemp39);
			double fTemp40 = (0.02017333553791887 * ((fSlow198 * (fTemp2 + (fSlow200 * fTemp4))) + (fTemp5 + (fSlow201 * fTemp7))));
			fRec59[0] = max((fRec59[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fVbargraph32 = FAUSTFLOAT(fRec59[0]);
			output32[i] = FAUSTFLOAT(fTemp40);
			double fTemp41 = (0.02017333553791887 * ((fSlow203 * (fTemp2 + (fSlow205 * fTemp4))) + (fTemp5 + (fSlow206 * fTemp7))));
			fRec60[0] = max((fRec60[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph33 = FAUSTFLOAT(fRec60[0]);
			output33[i] = FAUSTFLOAT(fTemp41);
			double fTemp42 = (0.02017333553791887 * ((fSlow208 * (fTemp2 + (fSlow210 * fTemp4))) + (fTemp5 + (fSlow211 * fTemp7))));
			fRec61[0] = max((fRec61[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fVbargraph34 = FAUSTFLOAT(fRec61[0]);
			output34[i] = FAUSTFLOAT(fTemp42);
			double fTemp43 = (0.02017333553791887 * ((fSlow213 * (fTemp2 + (fSlow215 * fTemp4))) + (fTemp5 + (fSlow216 * fTemp7))));
			fRec62[0] = max((fRec62[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph35 = FAUSTFLOAT(fRec62[0]);
			output35[i] = FAUSTFLOAT(fTemp43);
			double fTemp44 = (0.02017333553791887 * ((fSlow218 * (fTemp2 + (fSlow220 * fTemp4))) + (fTemp5 + (fSlow221 * fTemp7))));
			fRec63[0] = max((fRec63[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fVbargraph36 = FAUSTFLOAT(fRec63[0]);
			output36[i] = FAUSTFLOAT(fTemp44);
			double fTemp45 = (0.02017333553791887 * ((fSlow223 * (fTemp2 + (fSlow225 * fTemp4))) + (fTemp5 + (fSlow226 * fTemp7))));
			fRec64[0] = max((fRec64[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fVbargraph37 = FAUSTFLOAT(fRec64[0]);
			output37[i] = FAUSTFLOAT(fTemp45);
			double fTemp46 = (0.02017333553791887 * ((fSlow227 * (fTemp2 + (fSlow229 * fTemp4))) + (fTemp5 + (fSlow230 * fTemp7))));
			fRec65[0] = max((fRec65[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph38 = FAUSTFLOAT(fRec65[0]);
			output38[i] = FAUSTFLOAT(fTemp46);
			double fTemp47 = (0.02017333553791887 * ((fSlow231 * (fTemp2 + (fSlow233 * fTemp4))) + (fTemp5 + (fSlow234 * fTemp7))));
			fRec66[0] = max((fRec66[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fVbargraph39 = FAUSTFLOAT(fRec66[0]);
			output39[i] = FAUSTFLOAT(fTemp47);
			double fTemp48 = (0.02017333553791887 * ((fSlow235 * (fTemp2 + (fSlow237 * fTemp4))) + (fTemp5 + (fSlow238 * fTemp7))));
			fRec67[0] = max((fRec67[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph40 = FAUSTFLOAT(fRec67[0]);
			output40[i] = FAUSTFLOAT(fTemp48);
			double fTemp49 = (0.02017333553791887 * ((fSlow239 * (fTemp2 + (fSlow241 * fTemp4))) + (fTemp5 + (fSlow242 * fTemp7))));
			fRec68[0] = max((fRec68[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fVbargraph41 = FAUSTFLOAT(fRec68[0]);
			output41[i] = FAUSTFLOAT(fTemp49);
			double fTemp50 = (0.02017333553791887 * ((fSlow243 * (fTemp2 + (fSlow245 * fTemp4))) + (fTemp5 + (fSlow246 * fTemp7))));
			fRec69[0] = max((fRec69[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph42 = FAUSTFLOAT(fRec69[0]);
			output42[i] = FAUSTFLOAT(fTemp50);
			double fTemp51 = (0.02017333553791887 * ((fSlow247 * (fTemp2 + (fSlow249 * fTemp4))) + (fTemp5 + (fSlow250 * fTemp7))));
			fRec70[0] = max((fRec70[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph43 = FAUSTFLOAT(fRec70[0]);
			output43[i] = FAUSTFLOAT(fTemp51);
			double fTemp52 = (0.02017333553791887 * ((fSlow251 * (fTemp2 + (fSlow253 * fTemp4))) + (fTemp5 + (fSlow254 * fTemp7))));
			fRec71[0] = max((fRec71[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fVbargraph44 = FAUSTFLOAT(fRec71[0]);
			output44[i] = FAUSTFLOAT(fTemp52);
			double fTemp53 = (0.02017333553791887 * ((fSlow255 * (fTemp2 + (fSlow257 * fTemp4))) + (fTemp5 + (fSlow258 * fTemp7))));
			fRec72[0] = max((fRec72[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fVbargraph45 = FAUSTFLOAT(fRec72[0]);
			output45[i] = FAUSTFLOAT(fTemp53);
			double fTemp54 = (0.02017333553791887 * ((fSlow259 * (fTemp2 + (fSlow261 * fTemp4))) + (fTemp5 + (fSlow262 * fTemp7))));
			fRec73[0] = max((fRec73[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fVbargraph46 = FAUSTFLOAT(fRec73[0]);
			output46[i] = FAUSTFLOAT(fTemp54);
			double fTemp55 = (0.02017333553791887 * ((fSlow263 * (fTemp2 + (fSlow265 * fTemp4))) + (fTemp5 + (fSlow266 * fTemp7))));
			fRec74[0] = max((fRec74[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph47 = FAUSTFLOAT(fRec74[0]);
			output47[i] = FAUSTFLOAT(fTemp55);
			double fTemp56 = (0.02017333553791887 * ((fSlow267 * (fTemp2 + (fSlow269 * fTemp4))) + (fTemp5 + (fSlow270 * fTemp7))));
			fRec75[0] = max((fRec75[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fVbargraph48 = FAUSTFLOAT(fRec75[0]);
			output48[i] = FAUSTFLOAT(fTemp56);
			double fTemp57 = (0.02017333553791887 * ((fSlow271 * (fTemp2 + (fSlow273 * fTemp4))) + (fTemp5 + (fSlow274 * fTemp7))));
			fRec76[0] = max((fRec76[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fVbargraph49 = FAUSTFLOAT(fRec76[0]);
			output49[i] = FAUSTFLOAT(fTemp57);
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec2[1] = fRec2[0];
			fRec1[1] = fRec1[0];
			fRec6[1] = fRec6[0];
			fRec5[1] = fRec5[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			fRec9[1] = fRec9[0];
			fRec12[1] = fRec12[0];
			fVec0[1] = fVec0[0];
			fRec13[1] = fRec13[0];
			fRec14[1] = fRec14[0];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
			fRec17[1] = fRec17[0];
			fRec18[1] = fRec18[0];
			fRec16[1] = fRec16[0];
			fRec19[1] = fRec19[0];
			fRec15[1] = fRec15[0];
			fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			fRec21[1] = fRec21[0];
			fRec20[1] = fRec20[0];
			fVec1[1] = fVec1[0];
			fRec24[1] = fRec24[0];
			fRec26[1] = fRec26[0];
			fRec27[1] = fRec27[0];
			fRec25[1] = fRec25[0];
			fRec0[1] = fRec0[0];
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec30[1] = fRec30[0];
			fRec31[1] = fRec31[0];
			fRec32[1] = fRec32[0];
			fRec33[1] = fRec33[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec38[1] = fRec38[0];
			fRec39[1] = fRec39[0];
			fRec40[1] = fRec40[0];
			fRec41[1] = fRec41[0];
			fRec42[1] = fRec42[0];
			fRec43[1] = fRec43[0];
			fRec44[1] = fRec44[0];
			fRec45[1] = fRec45[0];
			fRec46[1] = fRec46[0];
			fRec47[1] = fRec47[0];
			fRec48[1] = fRec48[0];
			fRec49[1] = fRec49[0];
			fRec50[1] = fRec50[0];
			fRec51[1] = fRec51[0];
			fRec52[1] = fRec52[0];
			fRec53[1] = fRec53[0];
			fRec54[1] = fRec54[0];
			fRec55[1] = fRec55[0];
			fRec56[1] = fRec56[0];
			fRec57[1] = fRec57[0];
			fRec58[1] = fRec58[0];
			fRec59[1] = fRec59[0];
			fRec60[1] = fRec60[0];
			fRec61[1] = fRec61[0];
			fRec62[1] = fRec62[0];
			fRec63[1] = fRec63[0];
			fRec64[1] = fRec64[0];
			fRec65[1] = fRec65[0];
			fRec66[1] = fRec66[0];
			fRec67[1] = fRec67[0];
			fRec68[1] = fRec68[0];
			fRec69[1] = fRec69[0];
			fRec70[1] = fRec70[0];
			fRec71[1] = fRec71[0];
			fRec72[1] = fRec72[0];
			fRec73[1] = fRec73[0];
			fRec74[1] = fRec74[0];
			fRec75[1] = fRec75[0];
			fRec76[1] = fRec76[0];
			
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
