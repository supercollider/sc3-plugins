/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAPanLebedev504"
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
	FAUSTFLOAT fCheckbox5;
	double fConst10;
	double fConst11;
	double fConst12;
	double fConst13;
	double fRec31[2];
	double fRec32[2];
	double fRec30[2];
	double fRec33[2];
	double fVec2[2];
	double fRec34[2];
	double fRec35[2];
	double fRec36[2];
	double fRec29[2];
	double fRec28[2];
	double fVec3[2];
	double fRec37[2];
	double fRec40[2];
	double fRec41[2];
	double fRec39[2];
	double fRec42[2];
	double fRec43[2];
	double fRec38[2];
	double fRec0[2];
	FAUSTFLOAT fVbargraph0;
	FAUSTFLOAT fHslider3;
	double fRec44[2];
	FAUSTFLOAT fVbargraph1;
	double fRec45[2];
	FAUSTFLOAT fVbargraph2;
	double fRec46[2];
	FAUSTFLOAT fVbargraph3;
	double fRec47[2];
	FAUSTFLOAT fVbargraph4;
	double fRec48[2];
	FAUSTFLOAT fVbargraph5;
	double fRec49[2];
	FAUSTFLOAT fVbargraph6;
	double fRec50[2];
	FAUSTFLOAT fVbargraph7;
	double fRec51[2];
	FAUSTFLOAT fVbargraph8;
	double fRec52[2];
	FAUSTFLOAT fVbargraph9;
	double fRec53[2];
	FAUSTFLOAT fVbargraph10;
	double fRec54[2];
	FAUSTFLOAT fVbargraph11;
	double fRec55[2];
	FAUSTFLOAT fVbargraph12;
	double fRec56[2];
	FAUSTFLOAT fVbargraph13;
	double fRec57[2];
	FAUSTFLOAT fVbargraph14;
	double fRec58[2];
	FAUSTFLOAT fVbargraph15;
	double fRec59[2];
	FAUSTFLOAT fVbargraph16;
	double fRec60[2];
	FAUSTFLOAT fVbargraph17;
	double fRec61[2];
	FAUSTFLOAT fVbargraph18;
	double fRec62[2];
	FAUSTFLOAT fVbargraph19;
	double fRec63[2];
	FAUSTFLOAT fVbargraph20;
	double fRec64[2];
	FAUSTFLOAT fVbargraph21;
	double fRec65[2];
	FAUSTFLOAT fVbargraph22;
	double fRec66[2];
	FAUSTFLOAT fVbargraph23;
	double fRec67[2];
	FAUSTFLOAT fVbargraph24;
	double fRec68[2];
	FAUSTFLOAT fVbargraph25;
	double fRec69[2];
	FAUSTFLOAT fVbargraph26;
	double fRec70[2];
	FAUSTFLOAT fVbargraph27;
	double fRec71[2];
	FAUSTFLOAT fVbargraph28;
	double fRec72[2];
	FAUSTFLOAT fVbargraph29;
	double fRec73[2];
	FAUSTFLOAT fVbargraph30;
	double fRec74[2];
	FAUSTFLOAT fVbargraph31;
	double fRec75[2];
	FAUSTFLOAT fVbargraph32;
	double fRec76[2];
	FAUSTFLOAT fVbargraph33;
	double fRec77[2];
	FAUSTFLOAT fVbargraph34;
	double fRec78[2];
	FAUSTFLOAT fVbargraph35;
	double fRec79[2];
	FAUSTFLOAT fVbargraph36;
	double fRec80[2];
	FAUSTFLOAT fVbargraph37;
	double fRec81[2];
	FAUSTFLOAT fVbargraph38;
	double fRec82[2];
	FAUSTFLOAT fVbargraph39;
	double fRec83[2];
	FAUSTFLOAT fVbargraph40;
	double fRec84[2];
	FAUSTFLOAT fVbargraph41;
	double fRec85[2];
	FAUSTFLOAT fVbargraph42;
	double fRec86[2];
	FAUSTFLOAT fVbargraph43;
	double fRec87[2];
	FAUSTFLOAT fVbargraph44;
	double fRec88[2];
	FAUSTFLOAT fVbargraph45;
	double fRec89[2];
	FAUSTFLOAT fVbargraph46;
	double fRec90[2];
	FAUSTFLOAT fVbargraph47;
	double fRec91[2];
	FAUSTFLOAT fVbargraph48;
	double fRec92[2];
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
		m->declare("name", "HOAPanLebedev504");
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
		fConst10 = (984.71160495892411 / fConst0);
		fConst11 = (1969.4232099178482 / fConst0);
		fConst12 = (715.28839504107589 / fConst0);
		fConst13 = (1430.5767900821518 / fConst0);
		
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
		fCheckbox5 = FAUSTFLOAT(0.0);
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
			fRec31[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec32[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec30[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec33[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fVec2[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec34[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec35[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec36[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec29[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec28[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fVec3[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec37[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec40[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec41[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec39[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec42[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec43[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec38[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec0[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec44[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec45[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec46[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec47[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec48[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec49[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec50[l54] = 0.0;
			
		}
		for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
			fRec51[l55] = 0.0;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fRec52[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec53[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec54[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec55[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec56[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec57[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fRec58[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec59[l63] = 0.0;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec60[l64] = 0.0;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec61[l65] = 0.0;
			
		}
		for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
			fRec62[l66] = 0.0;
			
		}
		for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
			fRec63[l67] = 0.0;
			
		}
		for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
			fRec64[l68] = 0.0;
			
		}
		for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
			fRec65[l69] = 0.0;
			
		}
		for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
			fRec66[l70] = 0.0;
			
		}
		for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
			fRec67[l71] = 0.0;
			
		}
		for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
			fRec68[l72] = 0.0;
			
		}
		for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
			fRec69[l73] = 0.0;
			
		}
		for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
			fRec70[l74] = 0.0;
			
		}
		for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
			fRec71[l75] = 0.0;
			
		}
		for (int l76 = 0; (l76 < 2); l76 = (l76 + 1)) {
			fRec72[l76] = 0.0;
			
		}
		for (int l77 = 0; (l77 < 2); l77 = (l77 + 1)) {
			fRec73[l77] = 0.0;
			
		}
		for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
			fRec74[l78] = 0.0;
			
		}
		for (int l79 = 0; (l79 < 2); l79 = (l79 + 1)) {
			fRec75[l79] = 0.0;
			
		}
		for (int l80 = 0; (l80 < 2); l80 = (l80 + 1)) {
			fRec76[l80] = 0.0;
			
		}
		for (int l81 = 0; (l81 < 2); l81 = (l81 + 1)) {
			fRec77[l81] = 0.0;
			
		}
		for (int l82 = 0; (l82 < 2); l82 = (l82 + 1)) {
			fRec78[l82] = 0.0;
			
		}
		for (int l83 = 0; (l83 < 2); l83 = (l83 + 1)) {
			fRec79[l83] = 0.0;
			
		}
		for (int l84 = 0; (l84 < 2); l84 = (l84 + 1)) {
			fRec80[l84] = 0.0;
			
		}
		for (int l85 = 0; (l85 < 2); l85 = (l85 + 1)) {
			fRec81[l85] = 0.0;
			
		}
		for (int l86 = 0; (l86 < 2); l86 = (l86 + 1)) {
			fRec82[l86] = 0.0;
			
		}
		for (int l87 = 0; (l87 < 2); l87 = (l87 + 1)) {
			fRec83[l87] = 0.0;
			
		}
		for (int l88 = 0; (l88 < 2); l88 = (l88 + 1)) {
			fRec84[l88] = 0.0;
			
		}
		for (int l89 = 0; (l89 < 2); l89 = (l89 + 1)) {
			fRec85[l89] = 0.0;
			
		}
		for (int l90 = 0; (l90 < 2); l90 = (l90 + 1)) {
			fRec86[l90] = 0.0;
			
		}
		for (int l91 = 0; (l91 < 2); l91 = (l91 + 1)) {
			fRec87[l91] = 0.0;
			
		}
		for (int l92 = 0; (l92 < 2); l92 = (l92 + 1)) {
			fRec88[l92] = 0.0;
			
		}
		for (int l93 = 0; (l93 < 2); l93 = (l93 + 1)) {
			fRec89[l93] = 0.0;
			
		}
		for (int l94 = 0; (l94 < 2); l94 = (l94 + 1)) {
			fRec90[l94] = 0.0;
			
		}
		for (int l95 = 0; (l95 < 2); l95 = (l95 + 1)) {
			fRec91[l95] = 0.0;
			
		}
		for (int l96 = 0; (l96 < 2); l96 = (l96 + 1)) {
			fRec92[l96] = 0.0;
			
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
		ui_interface->openVerticalBox("HOAPanLebedev504");
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
		ui_interface->addCheckButton("4", &fCheckbox5);
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
		ui_interface->addVerticalBargraph("0x2fbc6c0", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "osc", "/output2 -70 6");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fc8520", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph2, "osc", "/output3 -70 6");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fd25c0", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph3, "osc", "/output4 -70 6");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fdc740", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph4, "osc", "/output5 -70 6");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fe69a0", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph5, "osc", "/output6 -70 6");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ff02c0", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph6, "osc", "/output7 -70 6");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ffa660", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph7, "osc", "/output8 -70 6");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3004180", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph8, "osc", "/output9 -70 6");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x300dd80", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph9, "osc", "/output10 -70 6");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3017a60", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph10, "osc", "/output11 -70 6");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3022200", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph11, "osc", "/output12 -70 6");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x302ca80", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph12, "osc", "/output13 -70 6");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30373e0", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph13, "osc", "/output14 -70 6");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3041e20", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph14, "osc", "/output15 -70 6");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x304bb70", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph15, "osc", "/output16 -70 6");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30559a0", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph16, "osc", "/output17 -70 6");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x305f8b0", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph17, "osc", "/output18 -70 6");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30698a0", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph18, "osc", "/output19 -70 6");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30746c0", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph19, "osc", "/output20 -70 6");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x307ec60", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph20, "osc", "/output21 -70 6");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30892e0", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph21, "osc", "/output22 -70 6");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3093a40", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph22, "osc", "/output23 -70 6");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x309de90", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph23, "osc", "/output24 -70 6");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30a83c0", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph24, "osc", "/output25 -70 6");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30b29d0", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph25, "osc", "/output26 -70 6");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30bd0c0", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph26, "osc", "/output27 -70 6");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30c7b90", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph27, "osc", "/output28 -70 6");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30d1330", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph28, "osc", "/output29 -70 6");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30dabb0", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph29, "osc", "/output30 -70 6");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30e4510", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph30, "osc", "/output31 -70 6");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30ef0b0", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph31, "osc", "/output32 -70 6");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30f95a0", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph32, "osc", "/output33 -70 6");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3103b70", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph33, "osc", "/output34 -70 6");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x310e220", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph34, "osc", "/output35 -70 6");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31189b0", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph35, "osc", "/output36 -70 6");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3123220", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph36, "osc", "/output37 -70 6");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x312db70", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph37, "osc", "/output38 -70 6");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31385a0", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph38, "osc", "/output39 -70 6");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31422f0", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph39, "osc", "/output40 -70 6");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x314c120", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph40, "osc", "/output41 -70 6");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3156030", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph41, "osc", "/output42 -70 6");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3160020", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph42, "osc", "/output43 -70 6");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x316a0f0", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph43, "osc", "/output44 -70 6");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31742a0", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph44, "osc", "/output45 -70 6");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x317e530", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph45, "osc", "/output46 -70 6");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31888a0", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph46, "osc", "/output47 -70 6");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3192cf0", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph47, "osc", "/output48 -70 6");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x319d220", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph48, "osc", "/output49 -70 6");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31a7830", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph49, "osc", "/output50 -70 6");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31b1f20", &fVbargraph49, -70.0, 6.0);
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
		double fSlow16 = mydsp_faustpower2_f(fSlow1);
		double fSlow17 = ((3.0 * fSlow16) + -1.0);
		double fSlow18 = ((2.5 * fSlow17) + -2.0);
		double fSlow19 = (1.0 - double(fCheckbox2));
		double fSlow20 = (2.333333333333333 * (fSlow18 * fSlow19));
		double fSlow21 = ((fConst5 / fSlow3) + 1.0);
		double fSlow22 = (fConst4 / (fSlow21 * fSlow3));
		double fSlow23 = (1.0 / ((fConst5 / fSlow7) + 1.0));
		double fSlow24 = mydsp_faustpower2_f(fSlow8);
		double fSlow25 = (1.0 / (((fConst6 / fSlow7) + (6.4594326934833797 * fSlow24)) + 1.0));
		double fSlow26 = mydsp_faustpower2_f(fSlow4);
		double fSlow27 = (((fConst6 / fSlow3) + (6.4594326934833797 * fSlow26)) + 1.0);
		double fSlow28 = (((fSlow27 * fSlow10) * fSlow7) / fSlow3);
		double fSlow29 = (25.837730773933519 * fSlow24);
		double fSlow30 = (fSlow29 + (fConst7 / fSlow7));
		double fSlow31 = (1.0 / fSlow27);
		double fSlow32 = (25.837730773933519 * fSlow26);
		double fSlow33 = ((fConst7 / fSlow3) + fSlow32);
		double fSlow34 = (fConst4 / fSlow7);
		double fSlow35 = ((((fSlow10 / fSlow3) + fSlow14) * fSlow7) * (1.0 - double(fCheckbox3)));
		double fSlow36 = (1.0 - double(fCheckbox4));
		double fSlow37 = (2.5 * (fSlow17 * fSlow36));
		double fSlow38 = (((fConst8 / fSlow3) + (3.0 * fSlow26)) + 1.0);
		double fSlow39 = (1.0 / fSlow38);
		double fSlow40 = (12.0 * fSlow26);
		double fSlow41 = ((fConst9 / fSlow3) + fSlow40);
		double fSlow42 = (1.0 / (((fConst8 / fSlow7) + (3.0 * fSlow24)) + 1.0));
		double fSlow43 = (((fSlow38 * fSlow10) * fSlow7) / fSlow3);
		double fSlow44 = (12.0 * fSlow24);
		double fSlow45 = (fSlow44 + (fConst9 / fSlow7));
		double fSlow46 = (1.0 - double(fCheckbox5));
		double fSlow47 = (2.25 * (((2.333333333333333 * (fSlow18 * fSlow16)) - (1.5 * fSlow17)) * fSlow46));
		double fSlow48 = (((fConst10 / fSlow3) + (9.1401308902779004 * fSlow26)) + 1.0);
		double fSlow49 = (1.0 / fSlow48);
		double fSlow50 = (36.560523561111602 * fSlow26);
		double fSlow51 = ((fConst11 / fSlow3) + fSlow50);
		double fSlow52 = (1.0 / (((fConst10 / fSlow7) + (9.1401308902779004 * fSlow24)) + 1.0));
		double fSlow53 = (1.0 / (((fConst12 / fSlow7) + (11.4878004768713 * fSlow24)) + 1.0));
		double fSlow54 = (((fConst12 / fSlow3) + (11.4878004768713 * fSlow26)) + 1.0);
		double fSlow55 = (((fSlow54 * fSlow10) * fSlow7) / fSlow3);
		double fSlow56 = (45.951201907485199 * fSlow24);
		double fSlow57 = (fSlow56 + (fConst13 / fSlow7));
		double fSlow58 = (1.0 / fSlow54);
		double fSlow59 = (45.951201907485199 * fSlow26);
		double fSlow60 = ((fConst13 / fSlow3) + fSlow59);
		double fSlow61 = (36.560523561111602 * fSlow24);
		double fSlow62 = (fSlow61 + (fConst11 / fSlow7));
		double fSlow63 = double(fHslider3);
		double fSlow64 = cos(fSlow63);
		double fSlow65 = cos(fSlow0);
		double fSlow66 = (fSlow64 * fSlow65);
		double fSlow67 = mydsp_faustpower2_f(fSlow64);
		double fSlow68 = mydsp_faustpower2_f(fSlow65);
		double fSlow69 = ((3.0 * (fSlow67 * fSlow68)) + -1.0);
		double fSlow70 = ((2.5 * fSlow69) + -2.0);
		double fSlow71 = (2.333333333333333 * (fSlow70 * fSlow19));
		double fSlow72 = (2.5 * (fSlow69 * fSlow36));
		double fSlow73 = (2.25 * (((2.333333333333333 * ((fSlow67 * fSlow70) * fSlow68)) - (1.5 * fSlow69)) * fSlow46));
		double fSlow74 = cos((fSlow63 + -1.5707963267948966));
		double fSlow75 = (fSlow74 * fSlow65);
		double fSlow76 = mydsp_faustpower2_f(fSlow74);
		double fSlow77 = ((3.0 * (fSlow76 * fSlow68)) + -1.0);
		double fSlow78 = ((2.5 * fSlow77) + -2.0);
		double fSlow79 = (2.333333333333333 * (fSlow78 * fSlow19));
		double fSlow80 = (2.5 * (fSlow77 * fSlow36));
		double fSlow81 = (2.25 * (((2.333333333333333 * ((fSlow76 * fSlow78) * fSlow68)) - (1.5 * fSlow77)) * fSlow46));
		double fSlow82 = cos((fSlow63 + -3.1415926535897931));
		double fSlow83 = (fSlow82 * fSlow65);
		double fSlow84 = mydsp_faustpower2_f(fSlow82);
		double fSlow85 = ((3.0 * (fSlow84 * fSlow68)) + -1.0);
		double fSlow86 = ((2.5 * fSlow85) + -2.0);
		double fSlow87 = (2.333333333333333 * (fSlow86 * fSlow19));
		double fSlow88 = (2.5 * (fSlow85 * fSlow36));
		double fSlow89 = (2.25 * (((2.333333333333333 * ((fSlow84 * fSlow86) * fSlow68)) - (1.5 * fSlow85)) * fSlow46));
		double fSlow90 = cos((fSlow63 + -4.7123889803846897));
		double fSlow91 = (fSlow90 * fSlow65);
		double fSlow92 = mydsp_faustpower2_f(fSlow90);
		double fSlow93 = ((3.0 * (fSlow92 * fSlow68)) + -1.0);
		double fSlow94 = ((2.5 * fSlow93) + -2.0);
		double fSlow95 = (2.333333333333333 * (fSlow94 * fSlow19));
		double fSlow96 = (2.5 * (fSlow93 * fSlow36));
		double fSlow97 = (2.25 * (((2.333333333333333 * ((fSlow92 * fSlow94) * fSlow68)) - (1.5 * fSlow93)) * fSlow46));
		double fSlow98 = ((6.123233995736766e-17 * fSlow66) - fSlow1);
		double fSlow99 = mydsp_faustpower2_f(fSlow98);
		double fSlow100 = ((3.0 * fSlow99) + -1.0);
		double fSlow101 = ((2.5 * fSlow100) + -2.0);
		double fSlow102 = (2.333333333333333 * (fSlow101 * fSlow19));
		double fSlow103 = (2.5 * (fSlow100 * fSlow36));
		double fSlow104 = (2.25 * (((2.333333333333333 * (fSlow99 * fSlow101)) - (1.5 * fSlow100)) * fSlow46));
		double fSlow105 = (0.70710678118654757 * fSlow66);
		double fSlow106 = (0.70710678118654746 * fSlow1);
		double fSlow107 = (fSlow105 + fSlow106);
		double fSlow108 = mydsp_faustpower2_f(fSlow107);
		double fSlow109 = ((3.0 * fSlow108) + -1.0);
		double fSlow110 = ((2.5 * fSlow109) + -2.0);
		double fSlow111 = (2.333333333333333 * (fSlow110 * fSlow19));
		double fSlow112 = (2.5 * (fSlow109 * fSlow36));
		double fSlow113 = (2.25 * (((2.333333333333333 * (fSlow108 * fSlow110)) - (1.5 * fSlow109)) * fSlow46));
		double fSlow114 = (0.70710678118654757 * fSlow75);
		double fSlow115 = (fSlow106 + fSlow114);
		double fSlow116 = mydsp_faustpower2_f(fSlow115);
		double fSlow117 = ((3.0 * fSlow116) + -1.0);
		double fSlow118 = ((2.5 * fSlow117) + -2.0);
		double fSlow119 = (2.333333333333333 * (fSlow118 * fSlow19));
		double fSlow120 = (2.5 * (fSlow117 * fSlow36));
		double fSlow121 = (2.25 * (((2.333333333333333 * (fSlow116 * fSlow118)) - (1.5 * fSlow117)) * fSlow46));
		double fSlow122 = (0.70710678118654757 * fSlow83);
		double fSlow123 = (fSlow106 + fSlow122);
		double fSlow124 = mydsp_faustpower2_f(fSlow123);
		double fSlow125 = ((3.0 * fSlow124) + -1.0);
		double fSlow126 = ((2.5 * fSlow125) + -2.0);
		double fSlow127 = (2.333333333333333 * (fSlow126 * fSlow19));
		double fSlow128 = (2.5 * (fSlow125 * fSlow36));
		double fSlow129 = (2.25 * (((2.333333333333333 * (fSlow124 * fSlow126)) - (1.5 * fSlow125)) * fSlow46));
		double fSlow130 = (0.70710678118654757 * fSlow91);
		double fSlow131 = (fSlow106 + fSlow130);
		double fSlow132 = mydsp_faustpower2_f(fSlow131);
		double fSlow133 = ((3.0 * fSlow132) + -1.0);
		double fSlow134 = ((2.5 * fSlow133) + -2.0);
		double fSlow135 = (2.25 * (((2.333333333333333 * (fSlow132 * fSlow134)) - (1.5 * fSlow133)) * fSlow46));
		double fSlow136 = (2.333333333333333 * (fSlow134 * fSlow19));
		double fSlow137 = (2.5 * (fSlow133 * fSlow36));
		double fSlow138 = cos((fSlow63 + -0.78539816339744828));
		double fSlow139 = (fSlow138 * fSlow65);
		double fSlow140 = mydsp_faustpower2_f(fSlow138);
		double fSlow141 = ((3.0 * (fSlow140 * fSlow68)) + -1.0);
		double fSlow142 = ((2.5 * fSlow141) + -2.0);
		double fSlow143 = (2.333333333333333 * (fSlow142 * fSlow19));
		double fSlow144 = (2.5 * (fSlow141 * fSlow36));
		double fSlow145 = (2.25 * (((2.333333333333333 * ((fSlow140 * fSlow142) * fSlow68)) - (1.5 * fSlow141)) * fSlow46));
		double fSlow146 = cos((fSlow63 + -2.3561944901923448));
		double fSlow147 = (fSlow146 * fSlow65);
		double fSlow148 = mydsp_faustpower2_f(fSlow146);
		double fSlow149 = ((3.0 * (fSlow148 * fSlow68)) + -1.0);
		double fSlow150 = ((2.5 * fSlow149) + -2.0);
		double fSlow151 = (2.333333333333333 * (fSlow150 * fSlow19));
		double fSlow152 = (2.5 * (fSlow149 * fSlow36));
		double fSlow153 = (2.25 * (((2.333333333333333 * ((fSlow148 * fSlow150) * fSlow68)) - (1.5 * fSlow149)) * fSlow46));
		double fSlow154 = cos((fSlow63 + -3.9269908169872414));
		double fSlow155 = (fSlow154 * fSlow65);
		double fSlow156 = mydsp_faustpower2_f(fSlow154);
		double fSlow157 = ((3.0 * (fSlow156 * fSlow68)) + -1.0);
		double fSlow158 = ((2.5 * fSlow157) + -2.0);
		double fSlow159 = (2.333333333333333 * (fSlow158 * fSlow19));
		double fSlow160 = (2.5 * (fSlow157 * fSlow36));
		double fSlow161 = (2.25 * (((2.333333333333333 * ((fSlow156 * fSlow158) * fSlow68)) - (1.5 * fSlow157)) * fSlow46));
		double fSlow162 = cos((fSlow63 + -5.497787143782138));
		double fSlow163 = (fSlow162 * fSlow65);
		double fSlow164 = mydsp_faustpower2_f(fSlow162);
		double fSlow165 = ((3.0 * (fSlow164 * fSlow68)) + -1.0);
		double fSlow166 = ((2.5 * fSlow165) + -2.0);
		double fSlow167 = (2.333333333333333 * (fSlow166 * fSlow19));
		double fSlow168 = (2.5 * (fSlow165 * fSlow36));
		double fSlow169 = (2.25 * (((2.333333333333333 * ((fSlow164 * fSlow166) * fSlow68)) - (1.5 * fSlow165)) * fSlow46));
		double fSlow170 = (fSlow105 - fSlow106);
		double fSlow171 = mydsp_faustpower2_f(fSlow170);
		double fSlow172 = ((3.0 * fSlow171) + -1.0);
		double fSlow173 = ((2.5 * fSlow172) + -2.0);
		double fSlow174 = (2.333333333333333 * (fSlow173 * fSlow19));
		double fSlow175 = (2.5 * (fSlow172 * fSlow36));
		double fSlow176 = (2.25 * (((2.333333333333333 * (fSlow171 * fSlow173)) - (1.5 * fSlow172)) * fSlow46));
		double fSlow177 = (fSlow114 - fSlow106);
		double fSlow178 = mydsp_faustpower2_f(fSlow177);
		double fSlow179 = ((3.0 * fSlow178) + -1.0);
		double fSlow180 = ((2.5 * fSlow179) + -2.0);
		double fSlow181 = (2.333333333333333 * (fSlow180 * fSlow19));
		double fSlow182 = (2.5 * (fSlow179 * fSlow36));
		double fSlow183 = (2.25 * (((2.333333333333333 * (fSlow178 * fSlow180)) - (1.5 * fSlow179)) * fSlow46));
		double fSlow184 = (fSlow122 - fSlow106);
		double fSlow185 = mydsp_faustpower2_f(fSlow184);
		double fSlow186 = ((3.0 * fSlow185) + -1.0);
		double fSlow187 = ((2.5 * fSlow186) + -2.0);
		double fSlow188 = (2.333333333333333 * (fSlow187 * fSlow19));
		double fSlow189 = (2.5 * (fSlow186 * fSlow36));
		double fSlow190 = (2.25 * (((2.333333333333333 * (fSlow185 * fSlow187)) - (1.5 * fSlow186)) * fSlow46));
		double fSlow191 = (fSlow130 - fSlow106);
		double fSlow192 = mydsp_faustpower2_f(fSlow191);
		double fSlow193 = ((3.0 * fSlow192) + -1.0);
		double fSlow194 = ((2.5 * fSlow193) + -2.0);
		double fSlow195 = (2.333333333333333 * (fSlow194 * fSlow19));
		double fSlow196 = (2.5 * (fSlow193 * fSlow36));
		double fSlow197 = (2.25 * (((2.333333333333333 * (fSlow192 * fSlow194)) - (1.5 * fSlow193)) * fSlow46));
		double fSlow198 = (0.81649658092772592 * fSlow139);
		double fSlow199 = (0.57735026918962584 * fSlow1);
		double fSlow200 = (fSlow198 + fSlow199);
		double fSlow201 = mydsp_faustpower2_f(fSlow200);
		double fSlow202 = ((3.0 * fSlow201) + -1.0);
		double fSlow203 = ((2.5 * fSlow202) + -2.0);
		double fSlow204 = (2.333333333333333 * (fSlow203 * fSlow19));
		double fSlow205 = (2.5 * (fSlow202 * fSlow36));
		double fSlow206 = (2.25 * (((2.333333333333333 * (fSlow201 * fSlow203)) - (1.5 * fSlow202)) * fSlow46));
		double fSlow207 = (0.81649658092772592 * fSlow147);
		double fSlow208 = (fSlow199 + fSlow207);
		double fSlow209 = mydsp_faustpower2_f(fSlow208);
		double fSlow210 = ((3.0 * fSlow209) + -1.0);
		double fSlow211 = ((2.5 * fSlow210) + -2.0);
		double fSlow212 = (2.333333333333333 * (fSlow211 * fSlow19));
		double fSlow213 = (2.5 * (fSlow210 * fSlow36));
		double fSlow214 = (2.25 * (((2.333333333333333 * (fSlow209 * fSlow211)) - (1.5 * fSlow210)) * fSlow46));
		double fSlow215 = (0.81649658092772592 * fSlow155);
		double fSlow216 = (fSlow199 + fSlow215);
		double fSlow217 = mydsp_faustpower2_f(fSlow216);
		double fSlow218 = ((3.0 * fSlow217) + -1.0);
		double fSlow219 = ((2.5 * fSlow218) + -2.0);
		double fSlow220 = (2.333333333333333 * (fSlow219 * fSlow19));
		double fSlow221 = (2.5 * (fSlow218 * fSlow36));
		double fSlow222 = (2.25 * (((2.333333333333333 * (fSlow217 * fSlow219)) - (1.5 * fSlow218)) * fSlow46));
		double fSlow223 = (0.81649658092772592 * fSlow163);
		double fSlow224 = (fSlow199 + fSlow223);
		double fSlow225 = mydsp_faustpower2_f(fSlow224);
		double fSlow226 = ((3.0 * fSlow225) + -1.0);
		double fSlow227 = ((2.5 * fSlow226) + -2.0);
		double fSlow228 = (2.333333333333333 * (fSlow227 * fSlow19));
		double fSlow229 = (2.5 * (fSlow226 * fSlow36));
		double fSlow230 = (2.25 * (((2.333333333333333 * (fSlow225 * fSlow227)) - (1.5 * fSlow226)) * fSlow46));
		double fSlow231 = (fSlow198 - fSlow199);
		double fSlow232 = mydsp_faustpower2_f(fSlow231);
		double fSlow233 = ((3.0 * fSlow232) + -1.0);
		double fSlow234 = ((2.5 * fSlow233) + -2.0);
		double fSlow235 = (2.333333333333333 * (fSlow234 * fSlow19));
		double fSlow236 = (2.5 * (fSlow233 * fSlow36));
		double fSlow237 = (2.25 * (((2.333333333333333 * (fSlow232 * fSlow234)) - (1.5 * fSlow233)) * fSlow46));
		double fSlow238 = (fSlow207 - fSlow199);
		double fSlow239 = mydsp_faustpower2_f(fSlow238);
		double fSlow240 = ((3.0 * fSlow239) + -1.0);
		double fSlow241 = ((2.5 * fSlow240) + -2.0);
		double fSlow242 = (2.333333333333333 * (fSlow241 * fSlow19));
		double fSlow243 = (2.5 * (fSlow240 * fSlow36));
		double fSlow244 = (2.25 * (((2.333333333333333 * (fSlow239 * fSlow241)) - (1.5 * fSlow240)) * fSlow46));
		double fSlow245 = (fSlow215 - fSlow199);
		double fSlow246 = mydsp_faustpower2_f(fSlow245);
		double fSlow247 = ((3.0 * fSlow246) + -1.0);
		double fSlow248 = ((2.5 * fSlow247) + -2.0);
		double fSlow249 = (2.333333333333333 * (fSlow248 * fSlow19));
		double fSlow250 = (2.5 * (fSlow247 * fSlow36));
		double fSlow251 = (2.25 * (((2.333333333333333 * (fSlow246 * fSlow248)) - (1.5 * fSlow247)) * fSlow46));
		double fSlow252 = (fSlow223 - fSlow199);
		double fSlow253 = mydsp_faustpower2_f(fSlow252);
		double fSlow254 = ((3.0 * fSlow253) + -1.0);
		double fSlow255 = ((2.5 * fSlow254) + -2.0);
		double fSlow256 = (2.333333333333333 * (fSlow255 * fSlow19));
		double fSlow257 = (2.5 * (fSlow254 * fSlow36));
		double fSlow258 = (2.25 * (((2.333333333333333 * (fSlow253 * fSlow255)) - (1.5 * fSlow254)) * fSlow46));
		double fSlow259 = (0.42640143271122027 * (cos((fSlow63 + -0.78539816339744839)) * fSlow65));
		double fSlow260 = (0.90453403373329111 * fSlow1);
		double fSlow261 = (fSlow259 + fSlow260);
		double fSlow262 = mydsp_faustpower2_f(fSlow261);
		double fSlow263 = ((3.0 * fSlow262) + -1.0);
		double fSlow264 = ((2.5 * fSlow263) + -2.0);
		double fSlow265 = (2.333333333333333 * (fSlow264 * fSlow19));
		double fSlow266 = (2.5 * (fSlow263 * fSlow36));
		double fSlow267 = (2.25 * (((2.333333333333333 * (fSlow262 * fSlow264)) - (1.5 * fSlow263)) * fSlow46));
		double fSlow268 = (0.42640143271122027 * fSlow147);
		double fSlow269 = (fSlow260 + fSlow268);
		double fSlow270 = mydsp_faustpower2_f(fSlow269);
		double fSlow271 = ((3.0 * fSlow270) + -1.0);
		double fSlow272 = ((2.5 * fSlow271) + -2.0);
		double fSlow273 = (2.333333333333333 * (fSlow272 * fSlow19));
		double fSlow274 = (2.5 * (fSlow271 * fSlow36));
		double fSlow275 = (2.25 * (((2.333333333333333 * (fSlow270 * fSlow272)) - (1.5 * fSlow271)) * fSlow46));
		double fSlow276 = (0.42640143271122027 * fSlow155);
		double fSlow277 = (fSlow260 + fSlow276);
		double fSlow278 = mydsp_faustpower2_f(fSlow277);
		double fSlow279 = ((3.0 * fSlow278) + -1.0);
		double fSlow280 = ((2.5 * fSlow279) + -2.0);
		double fSlow281 = (2.333333333333333 * (fSlow280 * fSlow19));
		double fSlow282 = (2.5 * (fSlow279 * fSlow36));
		double fSlow283 = (2.25 * (((2.333333333333333 * (fSlow278 * fSlow280)) - (1.5 * fSlow279)) * fSlow46));
		double fSlow284 = (0.42640143271122027 * fSlow163);
		double fSlow285 = (fSlow260 + fSlow284);
		double fSlow286 = mydsp_faustpower2_f(fSlow285);
		double fSlow287 = ((3.0 * fSlow286) + -1.0);
		double fSlow288 = ((2.5 * fSlow287) + -2.0);
		double fSlow289 = (2.333333333333333 * (fSlow288 * fSlow19));
		double fSlow290 = (2.5 * (fSlow287 * fSlow36));
		double fSlow291 = (2.25 * (((2.333333333333333 * (fSlow286 * fSlow288)) - (1.5 * fSlow287)) * fSlow46));
		double fSlow292 = (0.95346258924559235 * (cos((fSlow63 + -0.32175055439664263)) * fSlow65));
		double fSlow293 = (0.30151134457776352 * fSlow1);
		double fSlow294 = (fSlow292 + fSlow293);
		double fSlow295 = mydsp_faustpower2_f(fSlow294);
		double fSlow296 = ((3.0 * fSlow295) + -1.0);
		double fSlow297 = ((2.5 * fSlow296) + -2.0);
		double fSlow298 = (2.333333333333333 * (fSlow297 * fSlow19));
		double fSlow299 = (2.5 * (fSlow296 * fSlow36));
		double fSlow300 = (2.25 * (((2.333333333333333 * (fSlow295 * fSlow297)) - (1.5 * fSlow296)) * fSlow46));
		double fSlow301 = (0.95346258924559235 * (cos((fSlow63 + -1.2490457723982544)) * fSlow65));
		double fSlow302 = (fSlow293 + fSlow301);
		double fSlow303 = mydsp_faustpower2_f(fSlow302);
		double fSlow304 = ((3.0 * fSlow303) + -1.0);
		double fSlow305 = ((2.5 * fSlow304) + -2.0);
		double fSlow306 = (2.333333333333333 * (fSlow305 * fSlow19));
		double fSlow307 = (2.5 * (fSlow304 * fSlow36));
		double fSlow308 = (2.25 * (((2.333333333333333 * (fSlow303 * fSlow305)) - (1.5 * fSlow304)) * fSlow46));
		double fSlow309 = (0.95346258924559235 * (cos((fSlow63 + -1.8925468811915387)) * fSlow65));
		double fSlow310 = (fSlow293 + fSlow309);
		double fSlow311 = mydsp_faustpower2_f(fSlow310);
		double fSlow312 = ((3.0 * fSlow311) + -1.0);
		double fSlow313 = ((2.5 * fSlow312) + -2.0);
		double fSlow314 = (2.333333333333333 * (fSlow313 * fSlow19));
		double fSlow315 = (2.5 * (fSlow312 * fSlow36));
		double fSlow316 = (2.25 * (((2.333333333333333 * (fSlow311 * fSlow313)) - (1.5 * fSlow312)) * fSlow46));
		double fSlow317 = (0.95346258924559235 * (cos((fSlow63 + -2.8198420991931505)) * fSlow65));
		double fSlow318 = (fSlow293 + fSlow317);
		double fSlow319 = mydsp_faustpower2_f(fSlow318);
		double fSlow320 = ((3.0 * fSlow319) + -1.0);
		double fSlow321 = ((2.5 * fSlow320) + -2.0);
		double fSlow322 = (2.333333333333333 * (fSlow321 * fSlow19));
		double fSlow323 = (2.5 * (fSlow320 * fSlow36));
		double fSlow324 = (2.25 * (((2.333333333333333 * (fSlow319 * fSlow321)) - (1.5 * fSlow320)) * fSlow46));
		double fSlow325 = (0.95346258924559235 * (cos((fSlow63 + -3.4633432079864357)) * fSlow65));
		double fSlow326 = (fSlow293 + fSlow325);
		double fSlow327 = mydsp_faustpower2_f(fSlow326);
		double fSlow328 = ((3.0 * fSlow327) + -1.0);
		double fSlow329 = ((2.5 * fSlow328) + -2.0);
		double fSlow330 = (2.333333333333333 * (fSlow329 * fSlow19));
		double fSlow331 = (2.5 * (fSlow328 * fSlow36));
		double fSlow332 = (2.25 * (((2.333333333333333 * (fSlow327 * fSlow329)) - (1.5 * fSlow328)) * fSlow46));
		double fSlow333 = (0.95346258924559235 * (cos((fSlow63 + -4.3906384259880475)) * fSlow65));
		double fSlow334 = (fSlow293 + fSlow333);
		double fSlow335 = mydsp_faustpower2_f(fSlow334);
		double fSlow336 = ((3.0 * fSlow335) + -1.0);
		double fSlow337 = ((2.5 * fSlow336) + -2.0);
		double fSlow338 = (2.333333333333333 * (fSlow337 * fSlow19));
		double fSlow339 = (2.5 * (fSlow336 * fSlow36));
		double fSlow340 = (2.25 * (((2.333333333333333 * (fSlow335 * fSlow337)) - (1.5 * fSlow336)) * fSlow46));
		double fSlow341 = (0.95346258924559235 * (cos((fSlow63 + -5.0341395347813318)) * fSlow65));
		double fSlow342 = (fSlow293 + fSlow341);
		double fSlow343 = mydsp_faustpower2_f(fSlow342);
		double fSlow344 = ((3.0 * fSlow343) + -1.0);
		double fSlow345 = ((2.5 * fSlow344) + -2.0);
		double fSlow346 = (2.333333333333333 * (fSlow345 * fSlow19));
		double fSlow347 = (2.5 * (fSlow344 * fSlow36));
		double fSlow348 = (2.25 * (((2.333333333333333 * (fSlow343 * fSlow345)) - (1.5 * fSlow344)) * fSlow46));
		double fSlow349 = (0.95346258924559235 * (cos((fSlow63 + -5.9614347527829432)) * fSlow65));
		double fSlow350 = (fSlow293 + fSlow349);
		double fSlow351 = mydsp_faustpower2_f(fSlow350);
		double fSlow352 = ((3.0 * fSlow351) + -1.0);
		double fSlow353 = ((2.5 * fSlow352) + -2.0);
		double fSlow354 = (2.333333333333333 * (fSlow353 * fSlow19));
		double fSlow355 = (2.5 * (fSlow352 * fSlow36));
		double fSlow356 = (2.25 * (((2.333333333333333 * (fSlow351 * fSlow353)) - (1.5 * fSlow352)) * fSlow46));
		double fSlow357 = (fSlow292 - fSlow293);
		double fSlow358 = mydsp_faustpower2_f(fSlow357);
		double fSlow359 = ((3.0 * fSlow358) + -1.0);
		double fSlow360 = ((2.5 * fSlow359) + -2.0);
		double fSlow361 = (2.333333333333333 * (fSlow360 * fSlow19));
		double fSlow362 = (2.5 * (fSlow359 * fSlow36));
		double fSlow363 = (2.25 * (((2.333333333333333 * (fSlow358 * fSlow360)) - (1.5 * fSlow359)) * fSlow46));
		double fSlow364 = (fSlow301 - fSlow293);
		double fSlow365 = mydsp_faustpower2_f(fSlow364);
		double fSlow366 = ((3.0 * fSlow365) + -1.0);
		double fSlow367 = ((2.5 * fSlow366) + -2.0);
		double fSlow368 = (2.333333333333333 * (fSlow367 * fSlow19));
		double fSlow369 = (2.5 * (fSlow366 * fSlow36));
		double fSlow370 = (2.25 * (((2.333333333333333 * (fSlow365 * fSlow367)) - (1.5 * fSlow366)) * fSlow46));
		double fSlow371 = (fSlow309 - fSlow293);
		double fSlow372 = mydsp_faustpower2_f(fSlow371);
		double fSlow373 = ((3.0 * fSlow372) + -1.0);
		double fSlow374 = ((2.5 * fSlow373) + -2.0);
		double fSlow375 = (2.333333333333333 * (fSlow374 * fSlow19));
		double fSlow376 = (2.5 * (fSlow373 * fSlow36));
		double fSlow377 = (2.25 * (((2.333333333333333 * (fSlow372 * fSlow374)) - (1.5 * fSlow373)) * fSlow46));
		double fSlow378 = (fSlow317 - fSlow293);
		double fSlow379 = mydsp_faustpower2_f(fSlow378);
		double fSlow380 = ((3.0 * fSlow379) + -1.0);
		double fSlow381 = ((2.5 * fSlow380) + -2.0);
		double fSlow382 = (2.333333333333333 * (fSlow381 * fSlow19));
		double fSlow383 = (2.5 * (fSlow380 * fSlow36));
		double fSlow384 = (2.25 * (((2.333333333333333 * (fSlow379 * fSlow381)) - (1.5 * fSlow380)) * fSlow46));
		double fSlow385 = (fSlow325 - fSlow293);
		double fSlow386 = mydsp_faustpower2_f(fSlow385);
		double fSlow387 = ((3.0 * fSlow386) + -1.0);
		double fSlow388 = ((2.5 * fSlow387) + -2.0);
		double fSlow389 = (2.333333333333333 * (fSlow388 * fSlow19));
		double fSlow390 = (2.5 * (fSlow387 * fSlow36));
		double fSlow391 = (2.25 * (((2.333333333333333 * (fSlow386 * fSlow388)) - (1.5 * fSlow387)) * fSlow46));
		double fSlow392 = (fSlow333 - fSlow293);
		double fSlow393 = mydsp_faustpower2_f(fSlow392);
		double fSlow394 = ((3.0 * fSlow393) + -1.0);
		double fSlow395 = ((2.5 * fSlow394) + -2.0);
		double fSlow396 = (2.333333333333333 * (fSlow395 * fSlow19));
		double fSlow397 = (2.5 * (fSlow394 * fSlow36));
		double fSlow398 = (2.25 * (((2.333333333333333 * (fSlow393 * fSlow395)) - (1.5 * fSlow394)) * fSlow46));
		double fSlow399 = (fSlow341 - fSlow293);
		double fSlow400 = mydsp_faustpower2_f(fSlow399);
		double fSlow401 = ((3.0 * fSlow400) + -1.0);
		double fSlow402 = ((2.5 * fSlow401) + -2.0);
		double fSlow403 = (2.333333333333333 * (fSlow402 * fSlow19));
		double fSlow404 = (2.5 * (fSlow401 * fSlow36));
		double fSlow405 = (2.25 * (((2.333333333333333 * (fSlow400 * fSlow402)) - (1.5 * fSlow401)) * fSlow46));
		double fSlow406 = (fSlow349 - fSlow293);
		double fSlow407 = mydsp_faustpower2_f(fSlow406);
		double fSlow408 = ((3.0 * fSlow407) + -1.0);
		double fSlow409 = ((2.5 * fSlow408) + -2.0);
		double fSlow410 = (2.333333333333333 * (fSlow409 * fSlow19));
		double fSlow411 = (2.5 * (fSlow408 * fSlow36));
		double fSlow412 = (2.25 * (((2.333333333333333 * (fSlow407 * fSlow409)) - (1.5 * fSlow408)) * fSlow46));
		double fSlow413 = (fSlow259 - fSlow260);
		double fSlow414 = mydsp_faustpower2_f(fSlow413);
		double fSlow415 = ((3.0 * fSlow414) + -1.0);
		double fSlow416 = ((2.5 * fSlow415) + -2.0);
		double fSlow417 = (2.333333333333333 * (fSlow416 * fSlow19));
		double fSlow418 = (2.5 * (fSlow415 * fSlow36));
		double fSlow419 = (2.25 * (((2.333333333333333 * (fSlow414 * fSlow416)) - (1.5 * fSlow415)) * fSlow46));
		double fSlow420 = (fSlow268 - fSlow260);
		double fSlow421 = mydsp_faustpower2_f(fSlow420);
		double fSlow422 = ((3.0 * fSlow421) + -1.0);
		double fSlow423 = ((2.5 * fSlow422) + -2.0);
		double fSlow424 = (2.333333333333333 * (fSlow423 * fSlow19));
		double fSlow425 = (2.5 * (fSlow422 * fSlow36));
		double fSlow426 = (2.25 * (((2.333333333333333 * (fSlow421 * fSlow423)) - (1.5 * fSlow422)) * fSlow46));
		double fSlow427 = (fSlow276 - fSlow260);
		double fSlow428 = mydsp_faustpower2_f(fSlow427);
		double fSlow429 = ((3.0 * fSlow428) + -1.0);
		double fSlow430 = ((2.5 * fSlow429) + -2.0);
		double fSlow431 = (2.333333333333333 * (fSlow430 * fSlow19));
		double fSlow432 = (2.5 * (fSlow429 * fSlow36));
		double fSlow433 = (2.25 * (((2.333333333333333 * (fSlow428 * fSlow430)) - (1.5 * fSlow429)) * fSlow46));
		double fSlow434 = (fSlow284 - fSlow260);
		double fSlow435 = mydsp_faustpower2_f(fSlow434);
		double fSlow436 = ((3.0 * fSlow435) + -1.0);
		double fSlow437 = ((2.5 * fSlow436) + -2.0);
		double fSlow438 = (2.333333333333333 * (fSlow437 * fSlow19));
		double fSlow439 = (2.5 * (fSlow436 * fSlow36));
		double fSlow440 = (2.25 * (((2.333333333333333 * (fSlow435 * fSlow437)) - (1.5 * fSlow436)) * fSlow46));
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
			fRec9[0] = (fSlow25 * ((fSlow28 * fTemp0) - ((fSlow30 * fRec10[0]) + (fSlow29 * fRec11[0]))));
			fRec12[0] = (fRec9[0] + fRec12[1]);
			double fTemp3 = (fRec12[0] - fRec9[0]);
			fVec0[0] = fTemp3;
			fRec13[0] = (fRec13[1] + fVec0[1]);
			fRec14[0] = (fRec14[1] + fRec8[1]);
			fRec8[0] = (fSlow23 * ((fSlow21 * (fRec9[0] + (fSlow31 * ((fSlow33 * fTemp3) + (fSlow32 * fRec13[0]))))) - (fSlow34 * fRec14[0])));
			fRec7[0] = (fRec8[0] + fRec7[1]);
			fRec17[0] = (fRec17[1] + fRec16[1]);
			fRec18[0] = (fRec18[1] + fRec17[1]);
			fRec16[0] = (fSlow25 * (fTemp1 - ((fSlow30 * fRec17[0]) + (fSlow29 * fRec18[0]))));
			fRec19[0] = (fRec19[1] + fRec15[1]);
			fRec15[0] = (fSlow23 * (fRec16[0] - (fSlow34 * fRec19[0])));
			double fTemp4 = ((fSlow22 * (fRec7[0] - fRec8[0])) + (fRec8[0] + fRec15[0]));
			double fTemp5 = (fSlow35 * fTemp0);
			fRec22[0] = (fRec22[1] + fRec21[1]);
			fRec23[0] = (fRec23[1] + fRec22[1]);
			fRec21[0] = (fSlow42 * ((fSlow43 * fTemp0) - ((fSlow45 * fRec22[0]) + (fSlow44 * fRec23[0]))));
			fRec20[0] = (fRec21[0] + fRec20[1]);
			double fTemp6 = (fRec20[0] - fRec21[0]);
			fVec1[0] = fTemp6;
			fRec24[0] = (fRec24[1] + fVec1[1]);
			fRec26[0] = (fRec26[1] + fRec25[1]);
			fRec27[0] = (fRec27[1] + fRec26[1]);
			fRec25[0] = (fSlow42 * (fTemp1 - ((fSlow45 * fRec26[0]) + (fSlow44 * fRec27[0]))));
			double fTemp7 = ((fSlow39 * ((fSlow41 * fTemp6) + (fSlow40 * fRec24[0]))) + (fRec21[0] + fRec25[0]));
			fRec31[0] = (fRec31[1] + fRec30[1]);
			fRec32[0] = (fRec32[1] + fRec31[1]);
			fRec30[0] = (fSlow53 * ((fSlow55 * fTemp0) - ((fSlow57 * fRec31[0]) + (fSlow56 * fRec32[0]))));
			fRec33[0] = (fRec30[0] + fRec33[1]);
			double fTemp8 = (fRec33[0] - fRec30[0]);
			fVec2[0] = fTemp8;
			fRec34[0] = (fRec34[1] + fVec2[1]);
			fRec35[0] = (fRec35[1] + fRec29[1]);
			fRec36[0] = (fRec36[1] + fRec35[1]);
			fRec29[0] = (fSlow52 * ((fSlow48 * (fRec30[0] + (fSlow58 * ((fSlow60 * fTemp8) + (fSlow59 * fRec34[0]))))) - ((fSlow62 * fRec35[0]) + (fSlow61 * fRec36[0]))));
			fRec28[0] = (fRec29[0] + fRec28[1]);
			double fTemp9 = (fRec28[0] - fRec29[0]);
			fVec3[0] = fTemp9;
			fRec37[0] = (fRec37[1] + fVec3[1]);
			fRec40[0] = (fRec40[1] + fRec39[1]);
			fRec41[0] = (fRec41[1] + fRec40[1]);
			fRec39[0] = (fSlow53 * (fTemp1 - ((fSlow57 * fRec40[0]) + (fSlow56 * fRec41[0]))));
			fRec42[0] = (fRec42[1] + fRec38[1]);
			fRec43[0] = (fRec43[1] + fRec42[1]);
			fRec38[0] = (fSlow52 * (fRec39[0] - ((fSlow62 * fRec42[0]) + (fSlow61 * fRec43[0]))));
			double fTemp10 = ((fSlow49 * ((fSlow51 * fTemp9) + (fSlow50 * fRec37[0]))) + (fRec29[0] + fRec38[0]));
			double fTemp11 = (0.012698412698412698 * (((fSlow1 * (fTemp2 + (fSlow20 * fTemp4))) + (fTemp5 + (fSlow37 * fTemp7))) + (fSlow47 * fTemp10)));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fVbargraph0 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp11);
			double fTemp12 = (0.012698412698412698 * (((fSlow66 * (fTemp2 + (fSlow71 * fTemp4))) + (fTemp5 + (fSlow72 * fTemp7))) + (fSlow73 * fTemp10)));
			fRec44[0] = max((fRec44[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph1 = FAUSTFLOAT(fRec44[0]);
			output1[i] = FAUSTFLOAT(fTemp12);
			double fTemp13 = (0.012698412698412698 * (((fSlow75 * (fTemp2 + (fSlow79 * fTemp4))) + (fTemp5 + (fSlow80 * fTemp7))) + (fSlow81 * fTemp10)));
			fRec45[0] = max((fRec45[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fVbargraph2 = FAUSTFLOAT(fRec45[0]);
			output2[i] = FAUSTFLOAT(fTemp13);
			double fTemp14 = (0.012698412698412698 * (((fSlow83 * (fTemp2 + (fSlow87 * fTemp4))) + (fTemp5 + (fSlow88 * fTemp7))) + (fSlow89 * fTemp10)));
			fRec46[0] = max((fRec46[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fVbargraph3 = FAUSTFLOAT(fRec46[0]);
			output3[i] = FAUSTFLOAT(fTemp14);
			double fTemp15 = (0.012698412698412698 * (((fSlow91 * (fTemp2 + (fSlow95 * fTemp4))) + (fTemp5 + (fSlow96 * fTemp7))) + (fSlow97 * fTemp10)));
			fRec47[0] = max((fRec47[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph4 = FAUSTFLOAT(fRec47[0]);
			output4[i] = FAUSTFLOAT(fTemp15);
			double fTemp16 = (0.012698412698412698 * (((fSlow98 * (fTemp2 + (fSlow102 * fTemp4))) + (fTemp5 + (fSlow103 * fTemp7))) + (fSlow104 * fTemp10)));
			fRec48[0] = max((fRec48[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph5 = FAUSTFLOAT(fRec48[0]);
			output5[i] = FAUSTFLOAT(fTemp16);
			double fTemp17 = (0.022574955908289243 * (((fSlow107 * (fTemp2 + (fSlow111 * fTemp4))) + (fTemp5 + (fSlow112 * fTemp7))) + (fSlow113 * fTemp10)));
			fRec49[0] = max((fRec49[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph6 = FAUSTFLOAT(fRec49[0]);
			output6[i] = FAUSTFLOAT(fTemp17);
			double fTemp18 = (0.022574955908289243 * (((fSlow115 * (fTemp2 + (fSlow119 * fTemp4))) + (fTemp5 + (fSlow120 * fTemp7))) + (fSlow121 * fTemp10)));
			fRec50[0] = max((fRec50[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph7 = FAUSTFLOAT(fRec50[0]);
			output7[i] = FAUSTFLOAT(fTemp18);
			double fTemp19 = (0.022574955908289243 * (((fSlow123 * (fTemp2 + (fSlow127 * fTemp4))) + (fTemp5 + (fSlow128 * fTemp7))) + (fSlow129 * fTemp10)));
			fRec51[0] = max((fRec51[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph8 = FAUSTFLOAT(fRec51[0]);
			output8[i] = FAUSTFLOAT(fTemp19);
			double fTemp20 = (0.022574955908289243 * ((fSlow135 * fTemp10) + ((fSlow131 * (fTemp2 + (fSlow136 * fTemp4))) + (fTemp5 + (fSlow137 * fTemp7)))));
			fRec52[0] = max((fRec52[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph9 = FAUSTFLOAT(fRec52[0]);
			output9[i] = FAUSTFLOAT(fTemp20);
			double fTemp21 = (0.022574955908289243 * (((fSlow139 * (fTemp2 + (fSlow143 * fTemp4))) + (fTemp5 + (fSlow144 * fTemp7))) + (fSlow145 * fTemp10)));
			fRec53[0] = max((fRec53[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph10 = FAUSTFLOAT(fRec53[0]);
			output10[i] = FAUSTFLOAT(fTemp21);
			double fTemp22 = (0.022574955908289243 * (((fSlow147 * (fTemp2 + (fSlow151 * fTemp4))) + (fTemp5 + (fSlow152 * fTemp7))) + (fSlow153 * fTemp10)));
			fRec54[0] = max((fRec54[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph11 = FAUSTFLOAT(fRec54[0]);
			output11[i] = FAUSTFLOAT(fTemp22);
			double fTemp23 = (0.022574955908289243 * (((fSlow155 * (fTemp2 + (fSlow159 * fTemp4))) + (fTemp5 + (fSlow160 * fTemp7))) + (fSlow161 * fTemp10)));
			fRec55[0] = max((fRec55[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fVbargraph12 = FAUSTFLOAT(fRec55[0]);
			output12[i] = FAUSTFLOAT(fTemp23);
			double fTemp24 = (0.022574955908289243 * (((fSlow163 * (fTemp2 + (fSlow167 * fTemp4))) + (fTemp5 + (fSlow168 * fTemp7))) + (fSlow169 * fTemp10)));
			fRec56[0] = max((fRec56[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph13 = FAUSTFLOAT(fRec56[0]);
			output13[i] = FAUSTFLOAT(fTemp24);
			double fTemp25 = (0.022574955908289243 * (((fSlow170 * (fTemp2 + (fSlow174 * fTemp4))) + (fTemp5 + (fSlow175 * fTemp7))) + (fSlow176 * fTemp10)));
			fRec57[0] = max((fRec57[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph14 = FAUSTFLOAT(fRec57[0]);
			output14[i] = FAUSTFLOAT(fTemp25);
			double fTemp26 = (0.022574955908289243 * (((fSlow177 * (fTemp2 + (fSlow181 * fTemp4))) + (fTemp5 + (fSlow182 * fTemp7))) + (fSlow183 * fTemp10)));
			fRec58[0] = max((fRec58[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fVbargraph15 = FAUSTFLOAT(fRec58[0]);
			output15[i] = FAUSTFLOAT(fTemp26);
			double fTemp27 = (0.022574955908289243 * (((fSlow184 * (fTemp2 + (fSlow188 * fTemp4))) + (fTemp5 + (fSlow189 * fTemp7))) + (fSlow190 * fTemp10)));
			fRec59[0] = max((fRec59[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph16 = FAUSTFLOAT(fRec59[0]);
			output16[i] = FAUSTFLOAT(fTemp27);
			double fTemp28 = (0.022574955908289243 * (((fSlow191 * (fTemp2 + (fSlow195 * fTemp4))) + (fTemp5 + (fSlow196 * fTemp7))) + (fSlow197 * fTemp10)));
			fRec60[0] = max((fRec60[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fVbargraph17 = FAUSTFLOAT(fRec60[0]);
			output17[i] = FAUSTFLOAT(fTemp28);
			double fTemp29 = (0.021093750000000001 * (((fSlow200 * (fTemp2 + (fSlow204 * fTemp4))) + (fTemp5 + (fSlow205 * fTemp7))) + (fSlow206 * fTemp10)));
			fRec61[0] = max((fRec61[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph18 = FAUSTFLOAT(fRec61[0]);
			output18[i] = FAUSTFLOAT(fTemp29);
			double fTemp30 = (0.021093750000000001 * (((fSlow208 * (fTemp2 + (fSlow212 * fTemp4))) + (fTemp5 + (fSlow213 * fTemp7))) + (fSlow214 * fTemp10)));
			fRec62[0] = max((fRec62[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph19 = FAUSTFLOAT(fRec62[0]);
			output19[i] = FAUSTFLOAT(fTemp30);
			double fTemp31 = (0.021093750000000001 * (((fSlow216 * (fTemp2 + (fSlow220 * fTemp4))) + (fTemp5 + (fSlow221 * fTemp7))) + (fSlow222 * fTemp10)));
			fRec63[0] = max((fRec63[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph20 = FAUSTFLOAT(fRec63[0]);
			output20[i] = FAUSTFLOAT(fTemp31);
			double fTemp32 = (0.021093750000000001 * (((fSlow224 * (fTemp2 + (fSlow228 * fTemp4))) + (fTemp5 + (fSlow229 * fTemp7))) + (fSlow230 * fTemp10)));
			fRec64[0] = max((fRec64[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph21 = FAUSTFLOAT(fRec64[0]);
			output21[i] = FAUSTFLOAT(fTemp32);
			double fTemp33 = (0.021093750000000001 * (((fSlow231 * (fTemp2 + (fSlow235 * fTemp4))) + (fTemp5 + (fSlow236 * fTemp7))) + (fSlow237 * fTemp10)));
			fRec65[0] = max((fRec65[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph22 = FAUSTFLOAT(fRec65[0]);
			output22[i] = FAUSTFLOAT(fTemp33);
			double fTemp34 = (0.021093750000000001 * (((fSlow238 * (fTemp2 + (fSlow242 * fTemp4))) + (fTemp5 + (fSlow243 * fTemp7))) + (fSlow244 * fTemp10)));
			fRec66[0] = max((fRec66[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fVbargraph23 = FAUSTFLOAT(fRec66[0]);
			output23[i] = FAUSTFLOAT(fTemp34);
			double fTemp35 = (0.021093750000000001 * (((fSlow245 * (fTemp2 + (fSlow249 * fTemp4))) + (fTemp5 + (fSlow250 * fTemp7))) + (fSlow251 * fTemp10)));
			fRec67[0] = max((fRec67[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph24 = FAUSTFLOAT(fRec67[0]);
			output24[i] = FAUSTFLOAT(fTemp35);
			double fTemp36 = (0.021093750000000001 * (((fSlow252 * (fTemp2 + (fSlow256 * fTemp4))) + (fTemp5 + (fSlow257 * fTemp7))) + (fSlow258 * fTemp10)));
			fRec68[0] = max((fRec68[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fVbargraph25 = FAUSTFLOAT(fRec68[0]);
			output25[i] = FAUSTFLOAT(fTemp36);
			double fTemp37 = (0.02017333553791887 * (((fSlow261 * (fTemp2 + (fSlow265 * fTemp4))) + (fTemp5 + (fSlow266 * fTemp7))) + (fSlow267 * fTemp10)));
			fRec69[0] = max((fRec69[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fVbargraph26 = FAUSTFLOAT(fRec69[0]);
			output26[i] = FAUSTFLOAT(fTemp37);
			double fTemp38 = (0.02017333553791887 * (((fSlow269 * (fTemp2 + (fSlow273 * fTemp4))) + (fTemp5 + (fSlow274 * fTemp7))) + (fSlow275 * fTemp10)));
			fRec70[0] = max((fRec70[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fVbargraph27 = FAUSTFLOAT(fRec70[0]);
			output27[i] = FAUSTFLOAT(fTemp38);
			double fTemp39 = (0.02017333553791887 * (((fSlow277 * (fTemp2 + (fSlow281 * fTemp4))) + (fTemp5 + (fSlow282 * fTemp7))) + (fSlow283 * fTemp10)));
			fRec71[0] = max((fRec71[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph28 = FAUSTFLOAT(fRec71[0]);
			output28[i] = FAUSTFLOAT(fTemp39);
			double fTemp40 = (0.02017333553791887 * (((fSlow285 * (fTemp2 + (fSlow289 * fTemp4))) + (fTemp5 + (fSlow290 * fTemp7))) + (fSlow291 * fTemp10)));
			fRec72[0] = max((fRec72[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fVbargraph29 = FAUSTFLOAT(fRec72[0]);
			output29[i] = FAUSTFLOAT(fTemp40);
			double fTemp41 = (0.02017333553791887 * (((fSlow294 * (fTemp2 + (fSlow298 * fTemp4))) + (fTemp5 + (fSlow299 * fTemp7))) + (fSlow300 * fTemp10)));
			fRec73[0] = max((fRec73[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph30 = FAUSTFLOAT(fRec73[0]);
			output30[i] = FAUSTFLOAT(fTemp41);
			double fTemp42 = (0.02017333553791887 * (((fSlow302 * (fTemp2 + (fSlow306 * fTemp4))) + (fTemp5 + (fSlow307 * fTemp7))) + (fSlow308 * fTemp10)));
			fRec74[0] = max((fRec74[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fVbargraph31 = FAUSTFLOAT(fRec74[0]);
			output31[i] = FAUSTFLOAT(fTemp42);
			double fTemp43 = (0.02017333553791887 * (((fSlow310 * (fTemp2 + (fSlow314 * fTemp4))) + (fTemp5 + (fSlow315 * fTemp7))) + (fSlow316 * fTemp10)));
			fRec75[0] = max((fRec75[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph32 = FAUSTFLOAT(fRec75[0]);
			output32[i] = FAUSTFLOAT(fTemp43);
			double fTemp44 = (0.02017333553791887 * (((fSlow318 * (fTemp2 + (fSlow322 * fTemp4))) + (fTemp5 + (fSlow323 * fTemp7))) + (fSlow324 * fTemp10)));
			fRec76[0] = max((fRec76[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fVbargraph33 = FAUSTFLOAT(fRec76[0]);
			output33[i] = FAUSTFLOAT(fTemp44);
			double fTemp45 = (0.02017333553791887 * (((fSlow326 * (fTemp2 + (fSlow330 * fTemp4))) + (fTemp5 + (fSlow331 * fTemp7))) + (fSlow332 * fTemp10)));
			fRec77[0] = max((fRec77[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fVbargraph34 = FAUSTFLOAT(fRec77[0]);
			output34[i] = FAUSTFLOAT(fTemp45);
			double fTemp46 = (0.02017333553791887 * (((fSlow334 * (fTemp2 + (fSlow338 * fTemp4))) + (fTemp5 + (fSlow339 * fTemp7))) + (fSlow340 * fTemp10)));
			fRec78[0] = max((fRec78[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph35 = FAUSTFLOAT(fRec78[0]);
			output35[i] = FAUSTFLOAT(fTemp46);
			double fTemp47 = (0.02017333553791887 * (((fSlow342 * (fTemp2 + (fSlow346 * fTemp4))) + (fTemp5 + (fSlow347 * fTemp7))) + (fSlow348 * fTemp10)));
			fRec79[0] = max((fRec79[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fVbargraph36 = FAUSTFLOAT(fRec79[0]);
			output36[i] = FAUSTFLOAT(fTemp47);
			double fTemp48 = (0.02017333553791887 * (((fSlow350 * (fTemp2 + (fSlow354 * fTemp4))) + (fTemp5 + (fSlow355 * fTemp7))) + (fSlow356 * fTemp10)));
			fRec80[0] = max((fRec80[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph37 = FAUSTFLOAT(fRec80[0]);
			output37[i] = FAUSTFLOAT(fTemp48);
			double fTemp49 = (0.02017333553791887 * (((fSlow357 * (fTemp2 + (fSlow361 * fTemp4))) + (fTemp5 + (fSlow362 * fTemp7))) + (fSlow363 * fTemp10)));
			fRec81[0] = max((fRec81[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fVbargraph38 = FAUSTFLOAT(fRec81[0]);
			output38[i] = FAUSTFLOAT(fTemp49);
			double fTemp50 = (0.02017333553791887 * (((fSlow364 * (fTemp2 + (fSlow368 * fTemp4))) + (fTemp5 + (fSlow369 * fTemp7))) + (fSlow370 * fTemp10)));
			fRec82[0] = max((fRec82[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph39 = FAUSTFLOAT(fRec82[0]);
			output39[i] = FAUSTFLOAT(fTemp50);
			double fTemp51 = (0.02017333553791887 * (((fSlow371 * (fTemp2 + (fSlow375 * fTemp4))) + (fTemp5 + (fSlow376 * fTemp7))) + (fSlow377 * fTemp10)));
			fRec83[0] = max((fRec83[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph40 = FAUSTFLOAT(fRec83[0]);
			output40[i] = FAUSTFLOAT(fTemp51);
			double fTemp52 = (0.02017333553791887 * (((fSlow378 * (fTemp2 + (fSlow382 * fTemp4))) + (fTemp5 + (fSlow383 * fTemp7))) + (fSlow384 * fTemp10)));
			fRec84[0] = max((fRec84[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fVbargraph41 = FAUSTFLOAT(fRec84[0]);
			output41[i] = FAUSTFLOAT(fTemp52);
			double fTemp53 = (0.02017333553791887 * (((fSlow385 * (fTemp2 + (fSlow389 * fTemp4))) + (fTemp5 + (fSlow390 * fTemp7))) + (fSlow391 * fTemp10)));
			fRec85[0] = max((fRec85[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fVbargraph42 = FAUSTFLOAT(fRec85[0]);
			output42[i] = FAUSTFLOAT(fTemp53);
			double fTemp54 = (0.02017333553791887 * (((fSlow392 * (fTemp2 + (fSlow396 * fTemp4))) + (fTemp5 + (fSlow397 * fTemp7))) + (fSlow398 * fTemp10)));
			fRec86[0] = max((fRec86[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fVbargraph43 = FAUSTFLOAT(fRec86[0]);
			output43[i] = FAUSTFLOAT(fTemp54);
			double fTemp55 = (0.02017333553791887 * (((fSlow399 * (fTemp2 + (fSlow403 * fTemp4))) + (fTemp5 + (fSlow404 * fTemp7))) + (fSlow405 * fTemp10)));
			fRec87[0] = max((fRec87[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph44 = FAUSTFLOAT(fRec87[0]);
			output44[i] = FAUSTFLOAT(fTemp55);
			double fTemp56 = (0.02017333553791887 * (((fSlow406 * (fTemp2 + (fSlow410 * fTemp4))) + (fTemp5 + (fSlow411 * fTemp7))) + (fSlow412 * fTemp10)));
			fRec88[0] = max((fRec88[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fVbargraph45 = FAUSTFLOAT(fRec88[0]);
			output45[i] = FAUSTFLOAT(fTemp56);
			double fTemp57 = (0.02017333553791887 * (((fSlow413 * (fTemp2 + (fSlow417 * fTemp4))) + (fTemp5 + (fSlow418 * fTemp7))) + (fSlow419 * fTemp10)));
			fRec89[0] = max((fRec89[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fVbargraph46 = FAUSTFLOAT(fRec89[0]);
			output46[i] = FAUSTFLOAT(fTemp57);
			double fTemp58 = (0.02017333553791887 * (((fSlow420 * (fTemp2 + (fSlow424 * fTemp4))) + (fTemp5 + (fSlow425 * fTemp7))) + (fSlow426 * fTemp10)));
			fRec90[0] = max((fRec90[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fVbargraph47 = FAUSTFLOAT(fRec90[0]);
			output47[i] = FAUSTFLOAT(fTemp58);
			double fTemp59 = (0.02017333553791887 * (((fSlow427 * (fTemp2 + (fSlow431 * fTemp4))) + (fTemp5 + (fSlow432 * fTemp7))) + (fSlow433 * fTemp10)));
			fRec91[0] = max((fRec91[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fVbargraph48 = FAUSTFLOAT(fRec91[0]);
			output48[i] = FAUSTFLOAT(fTemp59);
			double fTemp60 = (0.02017333553791887 * (((fSlow434 * (fTemp2 + (fSlow438 * fTemp4))) + (fTemp5 + (fSlow439 * fTemp7))) + (fSlow440 * fTemp10)));
			fRec92[0] = max((fRec92[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fVbargraph49 = FAUSTFLOAT(fRec92[0]);
			output49[i] = FAUSTFLOAT(fTemp60);
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
			fRec31[1] = fRec31[0];
			fRec32[1] = fRec32[0];
			fRec30[1] = fRec30[0];
			fRec33[1] = fRec33[0];
			fVec2[1] = fVec2[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			fRec36[1] = fRec36[0];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
			fVec3[1] = fVec3[0];
			fRec37[1] = fRec37[0];
			fRec40[1] = fRec40[0];
			fRec41[1] = fRec41[0];
			fRec39[1] = fRec39[0];
			fRec42[1] = fRec42[0];
			fRec43[1] = fRec43[0];
			fRec38[1] = fRec38[0];
			fRec0[1] = fRec0[0];
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
			fRec77[1] = fRec77[0];
			fRec78[1] = fRec78[0];
			fRec79[1] = fRec79[0];
			fRec80[1] = fRec80[0];
			fRec81[1] = fRec81[0];
			fRec82[1] = fRec82[0];
			fRec83[1] = fRec83[0];
			fRec84[1] = fRec84[0];
			fRec85[1] = fRec85[0];
			fRec86[1] = fRec86[0];
			fRec87[1] = fRec87[0];
			fRec88[1] = fRec88[0];
			fRec89[1] = fRec89[0];
			fRec90[1] = fRec90[0];
			fRec91[1] = fRec91[0];
			fRec92[1] = fRec92[0];
			
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
