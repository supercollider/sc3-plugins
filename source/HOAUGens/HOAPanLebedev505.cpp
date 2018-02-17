/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAPanLebedev505"
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
	double fConst8;
	double fConst9;
	double fConst10;
	double fConst11;
	double fConst12;
	double fRec24[2];
	double fRec25[2];
	double fRec23[2];
	double fRec26[2];
	double fVec1[2];
	double fRec27[2];
	double fConst13;
	double fRec28[2];
	double fRec29[2];
	double fRec22[2];
	double fRec30[2];
	double fVec2[2];
	double fRec31[2];
	double fRec32[2];
	double fRec21[2];
	double fRec20[2];
	double fRec36[2];
	double fRec37[2];
	double fRec35[2];
	double fRec38[2];
	double fRec39[2];
	double fRec34[2];
	double fRec40[2];
	double fRec33[2];
	FAUSTFLOAT fCheckbox4;
	FAUSTFLOAT fCheckbox5;
	double fConst14;
	double fConst15;
	double fRec43[2];
	double fRec44[2];
	double fRec42[2];
	double fRec41[2];
	double fVec3[2];
	double fRec45[2];
	double fRec47[2];
	double fRec48[2];
	double fRec46[2];
	FAUSTFLOAT fCheckbox6;
	double fConst16;
	double fConst17;
	double fConst18;
	double fConst19;
	double fRec52[2];
	double fRec53[2];
	double fRec51[2];
	double fRec54[2];
	double fVec4[2];
	double fRec55[2];
	double fRec56[2];
	double fRec57[2];
	double fRec50[2];
	double fRec49[2];
	double fVec5[2];
	double fRec58[2];
	double fRec61[2];
	double fRec62[2];
	double fRec60[2];
	double fRec63[2];
	double fRec64[2];
	double fRec59[2];
	double fRec0[2];
	FAUSTFLOAT fVbargraph0;
	FAUSTFLOAT fHslider3;
	double fRec65[2];
	FAUSTFLOAT fVbargraph1;
	double fRec66[2];
	FAUSTFLOAT fVbargraph2;
	double fRec67[2];
	FAUSTFLOAT fVbargraph3;
	double fRec68[2];
	FAUSTFLOAT fVbargraph4;
	double fRec69[2];
	FAUSTFLOAT fVbargraph5;
	double fRec70[2];
	FAUSTFLOAT fVbargraph6;
	double fRec71[2];
	FAUSTFLOAT fVbargraph7;
	double fRec72[2];
	FAUSTFLOAT fVbargraph8;
	double fRec73[2];
	FAUSTFLOAT fVbargraph9;
	double fRec74[2];
	FAUSTFLOAT fVbargraph10;
	double fRec75[2];
	FAUSTFLOAT fVbargraph11;
	double fRec76[2];
	FAUSTFLOAT fVbargraph12;
	double fRec77[2];
	FAUSTFLOAT fVbargraph13;
	double fRec78[2];
	FAUSTFLOAT fVbargraph14;
	double fRec79[2];
	FAUSTFLOAT fVbargraph15;
	double fRec80[2];
	FAUSTFLOAT fVbargraph16;
	double fRec81[2];
	FAUSTFLOAT fVbargraph17;
	double fRec82[2];
	FAUSTFLOAT fVbargraph18;
	double fRec83[2];
	FAUSTFLOAT fVbargraph19;
	double fRec84[2];
	FAUSTFLOAT fVbargraph20;
	double fRec85[2];
	FAUSTFLOAT fVbargraph21;
	double fRec86[2];
	FAUSTFLOAT fVbargraph22;
	double fRec87[2];
	FAUSTFLOAT fVbargraph23;
	double fRec88[2];
	FAUSTFLOAT fVbargraph24;
	double fRec89[2];
	FAUSTFLOAT fVbargraph25;
	double fRec90[2];
	FAUSTFLOAT fVbargraph26;
	double fRec91[2];
	FAUSTFLOAT fVbargraph27;
	double fRec92[2];
	FAUSTFLOAT fVbargraph28;
	double fRec93[2];
	FAUSTFLOAT fVbargraph29;
	double fRec94[2];
	FAUSTFLOAT fVbargraph30;
	double fRec95[2];
	FAUSTFLOAT fVbargraph31;
	double fRec96[2];
	FAUSTFLOAT fVbargraph32;
	double fRec97[2];
	FAUSTFLOAT fVbargraph33;
	double fRec98[2];
	FAUSTFLOAT fVbargraph34;
	double fRec99[2];
	FAUSTFLOAT fVbargraph35;
	double fRec100[2];
	FAUSTFLOAT fVbargraph36;
	double fRec101[2];
	FAUSTFLOAT fVbargraph37;
	double fRec102[2];
	FAUSTFLOAT fVbargraph38;
	double fRec103[2];
	FAUSTFLOAT fVbargraph39;
	double fRec104[2];
	FAUSTFLOAT fVbargraph40;
	double fRec105[2];
	FAUSTFLOAT fVbargraph41;
	double fRec106[2];
	FAUSTFLOAT fVbargraph42;
	double fRec107[2];
	FAUSTFLOAT fVbargraph43;
	double fRec108[2];
	FAUSTFLOAT fVbargraph44;
	double fRec109[2];
	FAUSTFLOAT fVbargraph45;
	double fRec110[2];
	FAUSTFLOAT fVbargraph46;
	double fRec111[2];
	FAUSTFLOAT fVbargraph47;
	double fRec112[2];
	FAUSTFLOAT fVbargraph48;
	double fRec113[2];
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
		m->declare("name", "HOAPanLebedev505");
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
		fConst8 = (1239.8911224120607 / fConst0);
		fConst9 = (619.94556120603033 / fConst0);
		fConst10 = (1139.6651757122002 / fConst0);
		fConst11 = (790.3892630817644 / fConst0);
		fConst12 = (1580.7785261635288 / fConst0);
		fConst13 = (2279.3303514244003 / fConst0);
		fConst14 = (510.0 / fConst0);
		fConst15 = (1020.0 / fConst0);
		fConst16 = (984.71160495892411 / fConst0);
		fConst17 = (1969.4232099178482 / fConst0);
		fConst18 = (715.28839504107589 / fConst0);
		fConst19 = (1430.5767900821518 / fConst0);
		
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
		fCheckbox6 = FAUSTFLOAT(0.0);
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
			fRec24[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec25[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec23[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec26[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fVec1[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec27[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec28[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec29[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec22[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec30[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fVec2[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec31[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec32[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec21[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec20[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec36[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec37[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec35[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec38[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec39[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec34[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec40[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec33[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec43[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec44[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec42[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec41[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fVec3[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec45[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec47[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec48[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec46[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec52[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec53[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec51[l54] = 0.0;
			
		}
		for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
			fRec54[l55] = 0.0;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fVec4[l56] = 0.0;
			
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
			fRec50[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec49[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fVec5[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec58[l63] = 0.0;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec61[l64] = 0.0;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec62[l65] = 0.0;
			
		}
		for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
			fRec60[l66] = 0.0;
			
		}
		for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
			fRec63[l67] = 0.0;
			
		}
		for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
			fRec64[l68] = 0.0;
			
		}
		for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
			fRec59[l69] = 0.0;
			
		}
		for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
			fRec0[l70] = 0.0;
			
		}
		for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
			fRec65[l71] = 0.0;
			
		}
		for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
			fRec66[l72] = 0.0;
			
		}
		for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
			fRec67[l73] = 0.0;
			
		}
		for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
			fRec68[l74] = 0.0;
			
		}
		for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
			fRec69[l75] = 0.0;
			
		}
		for (int l76 = 0; (l76 < 2); l76 = (l76 + 1)) {
			fRec70[l76] = 0.0;
			
		}
		for (int l77 = 0; (l77 < 2); l77 = (l77 + 1)) {
			fRec71[l77] = 0.0;
			
		}
		for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
			fRec72[l78] = 0.0;
			
		}
		for (int l79 = 0; (l79 < 2); l79 = (l79 + 1)) {
			fRec73[l79] = 0.0;
			
		}
		for (int l80 = 0; (l80 < 2); l80 = (l80 + 1)) {
			fRec74[l80] = 0.0;
			
		}
		for (int l81 = 0; (l81 < 2); l81 = (l81 + 1)) {
			fRec75[l81] = 0.0;
			
		}
		for (int l82 = 0; (l82 < 2); l82 = (l82 + 1)) {
			fRec76[l82] = 0.0;
			
		}
		for (int l83 = 0; (l83 < 2); l83 = (l83 + 1)) {
			fRec77[l83] = 0.0;
			
		}
		for (int l84 = 0; (l84 < 2); l84 = (l84 + 1)) {
			fRec78[l84] = 0.0;
			
		}
		for (int l85 = 0; (l85 < 2); l85 = (l85 + 1)) {
			fRec79[l85] = 0.0;
			
		}
		for (int l86 = 0; (l86 < 2); l86 = (l86 + 1)) {
			fRec80[l86] = 0.0;
			
		}
		for (int l87 = 0; (l87 < 2); l87 = (l87 + 1)) {
			fRec81[l87] = 0.0;
			
		}
		for (int l88 = 0; (l88 < 2); l88 = (l88 + 1)) {
			fRec82[l88] = 0.0;
			
		}
		for (int l89 = 0; (l89 < 2); l89 = (l89 + 1)) {
			fRec83[l89] = 0.0;
			
		}
		for (int l90 = 0; (l90 < 2); l90 = (l90 + 1)) {
			fRec84[l90] = 0.0;
			
		}
		for (int l91 = 0; (l91 < 2); l91 = (l91 + 1)) {
			fRec85[l91] = 0.0;
			
		}
		for (int l92 = 0; (l92 < 2); l92 = (l92 + 1)) {
			fRec86[l92] = 0.0;
			
		}
		for (int l93 = 0; (l93 < 2); l93 = (l93 + 1)) {
			fRec87[l93] = 0.0;
			
		}
		for (int l94 = 0; (l94 < 2); l94 = (l94 + 1)) {
			fRec88[l94] = 0.0;
			
		}
		for (int l95 = 0; (l95 < 2); l95 = (l95 + 1)) {
			fRec89[l95] = 0.0;
			
		}
		for (int l96 = 0; (l96 < 2); l96 = (l96 + 1)) {
			fRec90[l96] = 0.0;
			
		}
		for (int l97 = 0; (l97 < 2); l97 = (l97 + 1)) {
			fRec91[l97] = 0.0;
			
		}
		for (int l98 = 0; (l98 < 2); l98 = (l98 + 1)) {
			fRec92[l98] = 0.0;
			
		}
		for (int l99 = 0; (l99 < 2); l99 = (l99 + 1)) {
			fRec93[l99] = 0.0;
			
		}
		for (int l100 = 0; (l100 < 2); l100 = (l100 + 1)) {
			fRec94[l100] = 0.0;
			
		}
		for (int l101 = 0; (l101 < 2); l101 = (l101 + 1)) {
			fRec95[l101] = 0.0;
			
		}
		for (int l102 = 0; (l102 < 2); l102 = (l102 + 1)) {
			fRec96[l102] = 0.0;
			
		}
		for (int l103 = 0; (l103 < 2); l103 = (l103 + 1)) {
			fRec97[l103] = 0.0;
			
		}
		for (int l104 = 0; (l104 < 2); l104 = (l104 + 1)) {
			fRec98[l104] = 0.0;
			
		}
		for (int l105 = 0; (l105 < 2); l105 = (l105 + 1)) {
			fRec99[l105] = 0.0;
			
		}
		for (int l106 = 0; (l106 < 2); l106 = (l106 + 1)) {
			fRec100[l106] = 0.0;
			
		}
		for (int l107 = 0; (l107 < 2); l107 = (l107 + 1)) {
			fRec101[l107] = 0.0;
			
		}
		for (int l108 = 0; (l108 < 2); l108 = (l108 + 1)) {
			fRec102[l108] = 0.0;
			
		}
		for (int l109 = 0; (l109 < 2); l109 = (l109 + 1)) {
			fRec103[l109] = 0.0;
			
		}
		for (int l110 = 0; (l110 < 2); l110 = (l110 + 1)) {
			fRec104[l110] = 0.0;
			
		}
		for (int l111 = 0; (l111 < 2); l111 = (l111 + 1)) {
			fRec105[l111] = 0.0;
			
		}
		for (int l112 = 0; (l112 < 2); l112 = (l112 + 1)) {
			fRec106[l112] = 0.0;
			
		}
		for (int l113 = 0; (l113 < 2); l113 = (l113 + 1)) {
			fRec107[l113] = 0.0;
			
		}
		for (int l114 = 0; (l114 < 2); l114 = (l114 + 1)) {
			fRec108[l114] = 0.0;
			
		}
		for (int l115 = 0; (l115 < 2); l115 = (l115 + 1)) {
			fRec109[l115] = 0.0;
			
		}
		for (int l116 = 0; (l116 < 2); l116 = (l116 + 1)) {
			fRec110[l116] = 0.0;
			
		}
		for (int l117 = 0; (l117 < 2); l117 = (l117 + 1)) {
			fRec111[l117] = 0.0;
			
		}
		for (int l118 = 0; (l118 < 2); l118 = (l118 + 1)) {
			fRec112[l118] = 0.0;
			
		}
		for (int l119 = 0; (l119 < 2); l119 = (l119 + 1)) {
			fRec113[l119] = 0.0;
			
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
		ui_interface->openVerticalBox("HOAPanLebedev505");
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
		ui_interface->addCheckButton("0", &fCheckbox4);
		ui_interface->addCheckButton("1", &fCheckbox0);
		ui_interface->addCheckButton("2", &fCheckbox5);
		ui_interface->addCheckButton("3", &fCheckbox2);
		ui_interface->addCheckButton("4", &fCheckbox6);
		ui_interface->addCheckButton("5", &fCheckbox3);
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
		ui_interface->addVerticalBargraph("0x3053950", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "osc", "/output2 -70 6");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30608f0", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph2, "osc", "/output3 -70 6");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x306bad0", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph3, "osc", "/output4 -70 6");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3076d90", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph4, "osc", "/output5 -70 6");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3082130", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph5, "osc", "/output6 -70 6");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x308cb90", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph6, "osc", "/output7 -70 6");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3098070", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph7, "osc", "/output8 -70 6");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30a2cd0", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph8, "osc", "/output9 -70 6");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30ada10", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph9, "osc", "/output10 -70 6");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30b8830", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph10, "osc", "/output11 -70 6");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30c4110", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph11, "osc", "/output12 -70 6");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30cfad0", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph12, "osc", "/output13 -70 6");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30db570", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph13, "osc", "/output14 -70 6");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30e70f0", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph14, "osc", "/output15 -70 6");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30f1f80", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph15, "osc", "/output16 -70 6");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30fcef0", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph16, "osc", "/output17 -70 6");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3107f40", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph17, "osc", "/output18 -70 6");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3113070", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph18, "osc", "/output19 -70 6");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x311efd0", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph19, "osc", "/output20 -70 6");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x312a6b0", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph20, "osc", "/output21 -70 6");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3135e70", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph21, "osc", "/output22 -70 6");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3141710", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph22, "osc", "/output23 -70 6");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x314cca0", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph23, "osc", "/output24 -70 6");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3158310", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph24, "osc", "/output25 -70 6");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3163a60", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph25, "osc", "/output26 -70 6");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x316f290", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph26, "osc", "/output27 -70 6");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x317aea0", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph27, "osc", "/output28 -70 6");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3185780", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph28, "osc", "/output29 -70 6");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3190140", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph29, "osc", "/output30 -70 6");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x319abe0", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph30, "osc", "/output31 -70 6");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31a68c0", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph31, "osc", "/output32 -70 6");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31b1ef0", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph32, "osc", "/output33 -70 6");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31bd600", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph33, "osc", "/output34 -70 6");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31c8df0", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph34, "osc", "/output35 -70 6");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31d46c0", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph35, "osc", "/output36 -70 6");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31e0070", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph36, "osc", "/output37 -70 6");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31ebb00", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph37, "osc", "/output38 -70 6");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31f7670", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph38, "osc", "/output39 -70 6");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3202500", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph39, "osc", "/output40 -70 6");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x320d470", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph40, "osc", "/output41 -70 6");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32184c0", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph41, "osc", "/output42 -70 6");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32235f0", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph42, "osc", "/output43 -70 6");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x322e800", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph43, "osc", "/output44 -70 6");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3239af0", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph44, "osc", "/output45 -70 6");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3244ec0", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph45, "osc", "/output46 -70 6");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3250370", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph46, "osc", "/output47 -70 6");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x325b900", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph47, "osc", "/output48 -70 6");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3266f70", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph48, "osc", "/output49 -70 6");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32726c0", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph49, "osc", "/output50 -70 6");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x327def0", &fVbargraph49, -70.0, 6.0);
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
		double fSlow35 = ((2.333333333333333 * (fSlow18 * fSlow16)) - (1.5 * fSlow17));
		double fSlow36 = (1.0 - double(fCheckbox3));
		double fSlow37 = (2.2000000000000002 * (((2.25 * fSlow35) - (1.3333333333333333 * fSlow18)) * fSlow36));
		double fSlow38 = ((fConst9 / fSlow3) + 1.0);
		double fSlow39 = (fConst8 / (fSlow38 * fSlow3));
		double fSlow40 = (1.0 / ((fConst9 / fSlow7) + 1.0));
		double fSlow41 = (1.0 / (((fConst10 / fSlow7) + (14.272480513280099 * fSlow24)) + 1.0));
		double fSlow42 = (((fConst10 / fSlow3) + (14.272480513280099 * fSlow26)) + 1.0);
		double fSlow43 = (1.0 / (((fConst11 / fSlow7) + (18.1563153134523 * fSlow24)) + 1.0));
		double fSlow44 = (((fConst11 / fSlow3) + (18.1563153134523 * fSlow26)) + 1.0);
		double fSlow45 = (((fSlow44 * fSlow10) * fSlow7) / fSlow3);
		double fSlow46 = (72.6252612538092 * fSlow24);
		double fSlow47 = (fSlow46 + (fConst12 / fSlow7));
		double fSlow48 = (1.0 / fSlow44);
		double fSlow49 = (72.6252612538092 * fSlow26);
		double fSlow50 = ((fConst12 / fSlow3) + fSlow49);
		double fSlow51 = (57.089922053120397 * fSlow24);
		double fSlow52 = (fSlow51 + (fConst13 / fSlow7));
		double fSlow53 = (1.0 / fSlow42);
		double fSlow54 = (57.089922053120397 * fSlow26);
		double fSlow55 = ((fConst13 / fSlow3) + fSlow54);
		double fSlow56 = (fConst8 / fSlow7);
		double fSlow57 = ((((fSlow10 / fSlow3) + fSlow14) * fSlow7) * (1.0 - double(fCheckbox4)));
		double fSlow58 = (1.0 - double(fCheckbox5));
		double fSlow59 = (2.5 * (fSlow17 * fSlow58));
		double fSlow60 = (((fConst14 / fSlow3) + (3.0 * fSlow26)) + 1.0);
		double fSlow61 = (1.0 / fSlow60);
		double fSlow62 = (12.0 * fSlow26);
		double fSlow63 = ((fConst15 / fSlow3) + fSlow62);
		double fSlow64 = (1.0 / (((fConst14 / fSlow7) + (3.0 * fSlow24)) + 1.0));
		double fSlow65 = (((fSlow60 * fSlow10) * fSlow7) / fSlow3);
		double fSlow66 = (12.0 * fSlow24);
		double fSlow67 = (fSlow66 + (fConst15 / fSlow7));
		double fSlow68 = (1.0 - double(fCheckbox6));
		double fSlow69 = (2.25 * (fSlow35 * fSlow68));
		double fSlow70 = (((fConst16 / fSlow3) + (9.1401308902779004 * fSlow26)) + 1.0);
		double fSlow71 = (1.0 / fSlow70);
		double fSlow72 = (36.560523561111602 * fSlow26);
		double fSlow73 = ((fConst17 / fSlow3) + fSlow72);
		double fSlow74 = (1.0 / (((fConst16 / fSlow7) + (9.1401308902779004 * fSlow24)) + 1.0));
		double fSlow75 = (1.0 / (((fConst18 / fSlow7) + (11.4878004768713 * fSlow24)) + 1.0));
		double fSlow76 = (((fConst18 / fSlow3) + (11.4878004768713 * fSlow26)) + 1.0);
		double fSlow77 = (((fSlow76 * fSlow10) * fSlow7) / fSlow3);
		double fSlow78 = (45.951201907485199 * fSlow24);
		double fSlow79 = (fSlow78 + (fConst19 / fSlow7));
		double fSlow80 = (1.0 / fSlow76);
		double fSlow81 = (45.951201907485199 * fSlow26);
		double fSlow82 = ((fConst19 / fSlow3) + fSlow81);
		double fSlow83 = (36.560523561111602 * fSlow24);
		double fSlow84 = (fSlow83 + (fConst17 / fSlow7));
		double fSlow85 = double(fHslider3);
		double fSlow86 = cos(fSlow85);
		double fSlow87 = cos(fSlow0);
		double fSlow88 = (fSlow86 * fSlow87);
		double fSlow89 = mydsp_faustpower2_f(fSlow86);
		double fSlow90 = mydsp_faustpower2_f(fSlow87);
		double fSlow91 = ((3.0 * (fSlow89 * fSlow90)) + -1.0);
		double fSlow92 = ((2.5 * fSlow91) + -2.0);
		double fSlow93 = (2.333333333333333 * (fSlow92 * fSlow19));
		double fSlow94 = ((2.333333333333333 * ((fSlow89 * fSlow92) * fSlow90)) - (1.5 * fSlow91));
		double fSlow95 = (2.2000000000000002 * (((2.25 * fSlow94) - (1.3333333333333333 * fSlow92)) * fSlow36));
		double fSlow96 = (2.5 * (fSlow91 * fSlow58));
		double fSlow97 = (2.25 * (fSlow94 * fSlow68));
		double fSlow98 = cos((fSlow85 + -1.5707963267948966));
		double fSlow99 = (fSlow98 * fSlow87);
		double fSlow100 = mydsp_faustpower2_f(fSlow98);
		double fSlow101 = ((3.0 * (fSlow100 * fSlow90)) + -1.0);
		double fSlow102 = ((2.5 * fSlow101) + -2.0);
		double fSlow103 = (2.333333333333333 * (fSlow102 * fSlow19));
		double fSlow104 = ((2.333333333333333 * ((fSlow100 * fSlow102) * fSlow90)) - (1.5 * fSlow101));
		double fSlow105 = (2.2000000000000002 * (((2.25 * fSlow104) - (1.3333333333333333 * fSlow102)) * fSlow36));
		double fSlow106 = (2.5 * (fSlow101 * fSlow58));
		double fSlow107 = (2.25 * (fSlow104 * fSlow68));
		double fSlow108 = cos((fSlow85 + -3.1415926535897931));
		double fSlow109 = (fSlow108 * fSlow87);
		double fSlow110 = mydsp_faustpower2_f(fSlow108);
		double fSlow111 = ((3.0 * (fSlow110 * fSlow90)) + -1.0);
		double fSlow112 = ((2.5 * fSlow111) + -2.0);
		double fSlow113 = (2.333333333333333 * (fSlow112 * fSlow19));
		double fSlow114 = ((2.333333333333333 * ((fSlow110 * fSlow112) * fSlow90)) - (1.5 * fSlow111));
		double fSlow115 = (2.2000000000000002 * (((2.25 * fSlow114) - (1.3333333333333333 * fSlow112)) * fSlow36));
		double fSlow116 = (2.5 * (fSlow111 * fSlow58));
		double fSlow117 = (2.25 * (fSlow114 * fSlow68));
		double fSlow118 = cos((fSlow85 + -4.7123889803846897));
		double fSlow119 = (fSlow118 * fSlow87);
		double fSlow120 = mydsp_faustpower2_f(fSlow118);
		double fSlow121 = ((3.0 * (fSlow120 * fSlow90)) + -1.0);
		double fSlow122 = ((2.5 * fSlow121) + -2.0);
		double fSlow123 = (2.333333333333333 * (fSlow122 * fSlow19));
		double fSlow124 = ((2.333333333333333 * ((fSlow120 * fSlow122) * fSlow90)) - (1.5 * fSlow121));
		double fSlow125 = (2.2000000000000002 * (((2.25 * fSlow124) - (1.3333333333333333 * fSlow122)) * fSlow36));
		double fSlow126 = (2.5 * (fSlow121 * fSlow58));
		double fSlow127 = (2.25 * (fSlow124 * fSlow68));
		double fSlow128 = ((6.123233995736766e-17 * fSlow88) - fSlow1);
		double fSlow129 = mydsp_faustpower2_f(fSlow128);
		double fSlow130 = ((3.0 * fSlow129) + -1.0);
		double fSlow131 = ((2.5 * fSlow130) + -2.0);
		double fSlow132 = (2.333333333333333 * (fSlow131 * fSlow19));
		double fSlow133 = ((2.333333333333333 * (fSlow129 * fSlow131)) - (1.5 * fSlow130));
		double fSlow134 = (2.2000000000000002 * (((2.25 * fSlow133) - (1.3333333333333333 * fSlow131)) * fSlow36));
		double fSlow135 = (2.5 * (fSlow130 * fSlow58));
		double fSlow136 = (2.25 * (fSlow133 * fSlow68));
		double fSlow137 = (0.70710678118654757 * fSlow88);
		double fSlow138 = (0.70710678118654746 * fSlow1);
		double fSlow139 = (fSlow137 + fSlow138);
		double fSlow140 = mydsp_faustpower2_f(fSlow139);
		double fSlow141 = ((3.0 * fSlow140) + -1.0);
		double fSlow142 = ((2.5 * fSlow141) + -2.0);
		double fSlow143 = (2.333333333333333 * (fSlow142 * fSlow19));
		double fSlow144 = ((2.333333333333333 * (fSlow140 * fSlow142)) - (1.5 * fSlow141));
		double fSlow145 = (2.2000000000000002 * (((2.25 * fSlow144) - (1.3333333333333333 * fSlow142)) * fSlow36));
		double fSlow146 = (2.5 * (fSlow141 * fSlow58));
		double fSlow147 = (2.25 * (fSlow144 * fSlow68));
		double fSlow148 = (0.70710678118654757 * fSlow99);
		double fSlow149 = (fSlow138 + fSlow148);
		double fSlow150 = mydsp_faustpower2_f(fSlow149);
		double fSlow151 = ((3.0 * fSlow150) + -1.0);
		double fSlow152 = ((2.5 * fSlow151) + -2.0);
		double fSlow153 = (2.333333333333333 * (fSlow152 * fSlow19));
		double fSlow154 = ((2.333333333333333 * (fSlow150 * fSlow152)) - (1.5 * fSlow151));
		double fSlow155 = (2.2000000000000002 * (((2.25 * fSlow154) - (1.3333333333333333 * fSlow152)) * fSlow36));
		double fSlow156 = (2.5 * (fSlow151 * fSlow58));
		double fSlow157 = (2.25 * (fSlow154 * fSlow68));
		double fSlow158 = (0.70710678118654757 * fSlow109);
		double fSlow159 = (fSlow138 + fSlow158);
		double fSlow160 = mydsp_faustpower2_f(fSlow159);
		double fSlow161 = ((3.0 * fSlow160) + -1.0);
		double fSlow162 = ((2.5 * fSlow161) + -2.0);
		double fSlow163 = (2.333333333333333 * (fSlow162 * fSlow19));
		double fSlow164 = ((2.333333333333333 * (fSlow160 * fSlow162)) - (1.5 * fSlow161));
		double fSlow165 = (2.2000000000000002 * (((2.25 * fSlow164) - (1.3333333333333333 * fSlow162)) * fSlow36));
		double fSlow166 = (2.5 * (fSlow161 * fSlow58));
		double fSlow167 = (2.25 * (fSlow164 * fSlow68));
		double fSlow168 = (0.70710678118654757 * fSlow119);
		double fSlow169 = (fSlow138 + fSlow168);
		double fSlow170 = mydsp_faustpower2_f(fSlow169);
		double fSlow171 = ((3.0 * fSlow170) + -1.0);
		double fSlow172 = ((2.5 * fSlow171) + -2.0);
		double fSlow173 = (2.333333333333333 * (fSlow172 * fSlow19));
		double fSlow174 = ((2.333333333333333 * (fSlow170 * fSlow172)) - (1.5 * fSlow171));
		double fSlow175 = (2.2000000000000002 * (((2.25 * fSlow174) - (1.3333333333333333 * fSlow172)) * fSlow36));
		double fSlow176 = (2.5 * (fSlow171 * fSlow58));
		double fSlow177 = (2.25 * (fSlow174 * fSlow68));
		double fSlow178 = cos((fSlow85 + -0.78539816339744828));
		double fSlow179 = (fSlow178 * fSlow87);
		double fSlow180 = mydsp_faustpower2_f(fSlow178);
		double fSlow181 = ((3.0 * (fSlow180 * fSlow90)) + -1.0);
		double fSlow182 = ((2.5 * fSlow181) + -2.0);
		double fSlow183 = (2.333333333333333 * (fSlow182 * fSlow19));
		double fSlow184 = ((2.333333333333333 * ((fSlow180 * fSlow182) * fSlow90)) - (1.5 * fSlow181));
		double fSlow185 = (2.2000000000000002 * (((2.25 * fSlow184) - (1.3333333333333333 * fSlow182)) * fSlow36));
		double fSlow186 = (2.5 * (fSlow181 * fSlow58));
		double fSlow187 = (2.25 * (fSlow184 * fSlow68));
		double fSlow188 = cos((fSlow85 + -2.3561944901923448));
		double fSlow189 = (fSlow188 * fSlow87);
		double fSlow190 = mydsp_faustpower2_f(fSlow188);
		double fSlow191 = ((3.0 * (fSlow190 * fSlow90)) + -1.0);
		double fSlow192 = ((2.5 * fSlow191) + -2.0);
		double fSlow193 = (2.333333333333333 * (fSlow192 * fSlow19));
		double fSlow194 = ((2.333333333333333 * ((fSlow190 * fSlow192) * fSlow90)) - (1.5 * fSlow191));
		double fSlow195 = (2.2000000000000002 * (((2.25 * fSlow194) - (1.3333333333333333 * fSlow192)) * fSlow36));
		double fSlow196 = (2.5 * (fSlow191 * fSlow58));
		double fSlow197 = (2.25 * (fSlow194 * fSlow68));
		double fSlow198 = cos((fSlow85 + -3.9269908169872414));
		double fSlow199 = (fSlow198 * fSlow87);
		double fSlow200 = mydsp_faustpower2_f(fSlow198);
		double fSlow201 = ((3.0 * (fSlow200 * fSlow90)) + -1.0);
		double fSlow202 = ((2.5 * fSlow201) + -2.0);
		double fSlow203 = (2.333333333333333 * (fSlow202 * fSlow19));
		double fSlow204 = ((2.333333333333333 * ((fSlow200 * fSlow202) * fSlow90)) - (1.5 * fSlow201));
		double fSlow205 = (2.2000000000000002 * (((2.25 * fSlow204) - (1.3333333333333333 * fSlow202)) * fSlow36));
		double fSlow206 = (2.5 * (fSlow201 * fSlow58));
		double fSlow207 = (2.25 * (fSlow204 * fSlow68));
		double fSlow208 = cos((fSlow85 + -5.497787143782138));
		double fSlow209 = (fSlow208 * fSlow87);
		double fSlow210 = mydsp_faustpower2_f(fSlow208);
		double fSlow211 = ((3.0 * (fSlow210 * fSlow90)) + -1.0);
		double fSlow212 = ((2.5 * fSlow211) + -2.0);
		double fSlow213 = (2.333333333333333 * (fSlow212 * fSlow19));
		double fSlow214 = ((2.333333333333333 * ((fSlow210 * fSlow212) * fSlow90)) - (1.5 * fSlow211));
		double fSlow215 = (2.2000000000000002 * (((2.25 * fSlow214) - (1.3333333333333333 * fSlow212)) * fSlow36));
		double fSlow216 = (2.5 * (fSlow211 * fSlow58));
		double fSlow217 = (2.25 * (fSlow214 * fSlow68));
		double fSlow218 = (fSlow137 - fSlow138);
		double fSlow219 = mydsp_faustpower2_f(fSlow218);
		double fSlow220 = ((3.0 * fSlow219) + -1.0);
		double fSlow221 = ((2.5 * fSlow220) + -2.0);
		double fSlow222 = (2.333333333333333 * (fSlow221 * fSlow19));
		double fSlow223 = ((2.333333333333333 * (fSlow219 * fSlow221)) - (1.5 * fSlow220));
		double fSlow224 = (2.2000000000000002 * (((2.25 * fSlow223) - (1.3333333333333333 * fSlow221)) * fSlow36));
		double fSlow225 = (2.5 * (fSlow220 * fSlow58));
		double fSlow226 = (2.25 * (fSlow223 * fSlow68));
		double fSlow227 = (fSlow148 - fSlow138);
		double fSlow228 = mydsp_faustpower2_f(fSlow227);
		double fSlow229 = ((3.0 * fSlow228) + -1.0);
		double fSlow230 = ((2.5 * fSlow229) + -2.0);
		double fSlow231 = (2.333333333333333 * (fSlow230 * fSlow19));
		double fSlow232 = ((2.333333333333333 * (fSlow228 * fSlow230)) - (1.5 * fSlow229));
		double fSlow233 = (2.2000000000000002 * (((2.25 * fSlow232) - (1.3333333333333333 * fSlow230)) * fSlow36));
		double fSlow234 = (2.5 * (fSlow229 * fSlow58));
		double fSlow235 = (2.25 * (fSlow232 * fSlow68));
		double fSlow236 = (fSlow158 - fSlow138);
		double fSlow237 = mydsp_faustpower2_f(fSlow236);
		double fSlow238 = ((3.0 * fSlow237) + -1.0);
		double fSlow239 = ((2.5 * fSlow238) + -2.0);
		double fSlow240 = (2.333333333333333 * (fSlow239 * fSlow19));
		double fSlow241 = ((2.333333333333333 * (fSlow237 * fSlow239)) - (1.5 * fSlow238));
		double fSlow242 = (2.2000000000000002 * (((2.25 * fSlow241) - (1.3333333333333333 * fSlow239)) * fSlow36));
		double fSlow243 = (2.5 * (fSlow238 * fSlow58));
		double fSlow244 = (2.25 * (fSlow241 * fSlow68));
		double fSlow245 = (fSlow168 - fSlow138);
		double fSlow246 = mydsp_faustpower2_f(fSlow245);
		double fSlow247 = ((3.0 * fSlow246) + -1.0);
		double fSlow248 = ((2.5 * fSlow247) + -2.0);
		double fSlow249 = (2.333333333333333 * (fSlow248 * fSlow19));
		double fSlow250 = ((2.333333333333333 * (fSlow246 * fSlow248)) - (1.5 * fSlow247));
		double fSlow251 = (2.2000000000000002 * (((2.25 * fSlow250) - (1.3333333333333333 * fSlow248)) * fSlow36));
		double fSlow252 = (2.5 * (fSlow247 * fSlow58));
		double fSlow253 = (2.25 * (fSlow250 * fSlow68));
		double fSlow254 = (0.81649658092772592 * fSlow179);
		double fSlow255 = (0.57735026918962584 * fSlow1);
		double fSlow256 = (fSlow254 + fSlow255);
		double fSlow257 = mydsp_faustpower2_f(fSlow256);
		double fSlow258 = ((3.0 * fSlow257) + -1.0);
		double fSlow259 = ((2.5 * fSlow258) + -2.0);
		double fSlow260 = (2.333333333333333 * (fSlow259 * fSlow19));
		double fSlow261 = ((2.333333333333333 * (fSlow257 * fSlow259)) - (1.5 * fSlow258));
		double fSlow262 = (2.2000000000000002 * (((2.25 * fSlow261) - (1.3333333333333333 * fSlow259)) * fSlow36));
		double fSlow263 = (2.5 * (fSlow258 * fSlow58));
		double fSlow264 = (2.25 * (fSlow261 * fSlow68));
		double fSlow265 = (0.81649658092772592 * fSlow189);
		double fSlow266 = (fSlow255 + fSlow265);
		double fSlow267 = mydsp_faustpower2_f(fSlow266);
		double fSlow268 = ((3.0 * fSlow267) + -1.0);
		double fSlow269 = ((2.5 * fSlow268) + -2.0);
		double fSlow270 = (2.333333333333333 * (fSlow269 * fSlow19));
		double fSlow271 = ((2.333333333333333 * (fSlow267 * fSlow269)) - (1.5 * fSlow268));
		double fSlow272 = (2.2000000000000002 * (((2.25 * fSlow271) - (1.3333333333333333 * fSlow269)) * fSlow36));
		double fSlow273 = (2.5 * (fSlow268 * fSlow58));
		double fSlow274 = (2.25 * (fSlow271 * fSlow68));
		double fSlow275 = (0.81649658092772592 * fSlow199);
		double fSlow276 = (fSlow255 + fSlow275);
		double fSlow277 = mydsp_faustpower2_f(fSlow276);
		double fSlow278 = ((3.0 * fSlow277) + -1.0);
		double fSlow279 = ((2.5 * fSlow278) + -2.0);
		double fSlow280 = (2.333333333333333 * (fSlow279 * fSlow19));
		double fSlow281 = ((2.333333333333333 * (fSlow277 * fSlow279)) - (1.5 * fSlow278));
		double fSlow282 = (2.2000000000000002 * (((2.25 * fSlow281) - (1.3333333333333333 * fSlow279)) * fSlow36));
		double fSlow283 = (2.5 * (fSlow278 * fSlow58));
		double fSlow284 = (2.25 * (fSlow281 * fSlow68));
		double fSlow285 = (0.81649658092772592 * fSlow209);
		double fSlow286 = (fSlow255 + fSlow285);
		double fSlow287 = mydsp_faustpower2_f(fSlow286);
		double fSlow288 = ((3.0 * fSlow287) + -1.0);
		double fSlow289 = ((2.5 * fSlow288) + -2.0);
		double fSlow290 = (2.333333333333333 * (fSlow289 * fSlow19));
		double fSlow291 = ((2.333333333333333 * (fSlow287 * fSlow289)) - (1.5 * fSlow288));
		double fSlow292 = (2.2000000000000002 * (((2.25 * fSlow291) - (1.3333333333333333 * fSlow289)) * fSlow36));
		double fSlow293 = (2.5 * (fSlow288 * fSlow58));
		double fSlow294 = (2.25 * (fSlow291 * fSlow68));
		double fSlow295 = (fSlow254 - fSlow255);
		double fSlow296 = mydsp_faustpower2_f(fSlow295);
		double fSlow297 = ((3.0 * fSlow296) + -1.0);
		double fSlow298 = ((2.5 * fSlow297) + -2.0);
		double fSlow299 = (2.333333333333333 * (fSlow298 * fSlow19));
		double fSlow300 = ((2.333333333333333 * (fSlow296 * fSlow298)) - (1.5 * fSlow297));
		double fSlow301 = (2.2000000000000002 * (((2.25 * fSlow300) - (1.3333333333333333 * fSlow298)) * fSlow36));
		double fSlow302 = (2.5 * (fSlow297 * fSlow58));
		double fSlow303 = (2.25 * (fSlow300 * fSlow68));
		double fSlow304 = (fSlow265 - fSlow255);
		double fSlow305 = mydsp_faustpower2_f(fSlow304);
		double fSlow306 = ((3.0 * fSlow305) + -1.0);
		double fSlow307 = ((2.5 * fSlow306) + -2.0);
		double fSlow308 = (2.333333333333333 * (fSlow307 * fSlow19));
		double fSlow309 = ((2.333333333333333 * (fSlow305 * fSlow307)) - (1.5 * fSlow306));
		double fSlow310 = (2.2000000000000002 * (((2.25 * fSlow309) - (1.3333333333333333 * fSlow307)) * fSlow36));
		double fSlow311 = (2.5 * (fSlow306 * fSlow58));
		double fSlow312 = (2.25 * (fSlow309 * fSlow68));
		double fSlow313 = (fSlow275 - fSlow255);
		double fSlow314 = mydsp_faustpower2_f(fSlow313);
		double fSlow315 = ((3.0 * fSlow314) + -1.0);
		double fSlow316 = ((2.5 * fSlow315) + -2.0);
		double fSlow317 = (2.333333333333333 * (fSlow316 * fSlow19));
		double fSlow318 = ((2.333333333333333 * (fSlow314 * fSlow316)) - (1.5 * fSlow315));
		double fSlow319 = (2.2000000000000002 * (((2.25 * fSlow318) - (1.3333333333333333 * fSlow316)) * fSlow36));
		double fSlow320 = (2.5 * (fSlow315 * fSlow58));
		double fSlow321 = (2.25 * (fSlow318 * fSlow68));
		double fSlow322 = (fSlow285 - fSlow255);
		double fSlow323 = mydsp_faustpower2_f(fSlow322);
		double fSlow324 = ((3.0 * fSlow323) + -1.0);
		double fSlow325 = ((2.5 * fSlow324) + -2.0);
		double fSlow326 = (2.333333333333333 * (fSlow325 * fSlow19));
		double fSlow327 = ((2.333333333333333 * (fSlow323 * fSlow325)) - (1.5 * fSlow324));
		double fSlow328 = (2.2000000000000002 * (((2.25 * fSlow327) - (1.3333333333333333 * fSlow325)) * fSlow36));
		double fSlow329 = (2.5 * (fSlow324 * fSlow58));
		double fSlow330 = (2.25 * (fSlow327 * fSlow68));
		double fSlow331 = (0.42640143271122027 * (cos((fSlow85 + -0.78539816339744839)) * fSlow87));
		double fSlow332 = (0.90453403373329111 * fSlow1);
		double fSlow333 = (fSlow331 + fSlow332);
		double fSlow334 = mydsp_faustpower2_f(fSlow333);
		double fSlow335 = ((3.0 * fSlow334) + -1.0);
		double fSlow336 = ((2.5 * fSlow335) + -2.0);
		double fSlow337 = (2.333333333333333 * (fSlow336 * fSlow19));
		double fSlow338 = ((2.333333333333333 * (fSlow334 * fSlow336)) - (1.5 * fSlow335));
		double fSlow339 = (2.2000000000000002 * (((2.25 * fSlow338) - (1.3333333333333333 * fSlow336)) * fSlow36));
		double fSlow340 = (2.5 * (fSlow335 * fSlow58));
		double fSlow341 = (2.25 * (fSlow338 * fSlow68));
		double fSlow342 = (0.42640143271122027 * fSlow189);
		double fSlow343 = (fSlow332 + fSlow342);
		double fSlow344 = mydsp_faustpower2_f(fSlow343);
		double fSlow345 = ((3.0 * fSlow344) + -1.0);
		double fSlow346 = ((2.5 * fSlow345) + -2.0);
		double fSlow347 = (2.333333333333333 * (fSlow346 * fSlow19));
		double fSlow348 = ((2.333333333333333 * (fSlow344 * fSlow346)) - (1.5 * fSlow345));
		double fSlow349 = (2.2000000000000002 * (((2.25 * fSlow348) - (1.3333333333333333 * fSlow346)) * fSlow36));
		double fSlow350 = (2.5 * (fSlow345 * fSlow58));
		double fSlow351 = (2.25 * (fSlow348 * fSlow68));
		double fSlow352 = (0.42640143271122027 * fSlow199);
		double fSlow353 = (fSlow332 + fSlow352);
		double fSlow354 = mydsp_faustpower2_f(fSlow353);
		double fSlow355 = ((3.0 * fSlow354) + -1.0);
		double fSlow356 = ((2.5 * fSlow355) + -2.0);
		double fSlow357 = (2.333333333333333 * (fSlow356 * fSlow19));
		double fSlow358 = ((2.333333333333333 * (fSlow354 * fSlow356)) - (1.5 * fSlow355));
		double fSlow359 = (2.2000000000000002 * (((2.25 * fSlow358) - (1.3333333333333333 * fSlow356)) * fSlow36));
		double fSlow360 = (2.5 * (fSlow355 * fSlow58));
		double fSlow361 = (2.25 * (fSlow358 * fSlow68));
		double fSlow362 = (0.42640143271122027 * fSlow209);
		double fSlow363 = (fSlow332 + fSlow362);
		double fSlow364 = mydsp_faustpower2_f(fSlow363);
		double fSlow365 = ((3.0 * fSlow364) + -1.0);
		double fSlow366 = ((2.5 * fSlow365) + -2.0);
		double fSlow367 = (2.333333333333333 * (fSlow366 * fSlow19));
		double fSlow368 = ((2.333333333333333 * (fSlow364 * fSlow366)) - (1.5 * fSlow365));
		double fSlow369 = (2.2000000000000002 * (((2.25 * fSlow368) - (1.3333333333333333 * fSlow366)) * fSlow36));
		double fSlow370 = (2.5 * (fSlow365 * fSlow58));
		double fSlow371 = (2.25 * (fSlow368 * fSlow68));
		double fSlow372 = (0.95346258924559235 * (cos((fSlow85 + -0.32175055439664263)) * fSlow87));
		double fSlow373 = (0.30151134457776352 * fSlow1);
		double fSlow374 = (fSlow372 + fSlow373);
		double fSlow375 = mydsp_faustpower2_f(fSlow374);
		double fSlow376 = ((3.0 * fSlow375) + -1.0);
		double fSlow377 = ((2.5 * fSlow376) + -2.0);
		double fSlow378 = (2.333333333333333 * (fSlow377 * fSlow19));
		double fSlow379 = ((2.333333333333333 * (fSlow375 * fSlow377)) - (1.5 * fSlow376));
		double fSlow380 = (2.2000000000000002 * (((2.25 * fSlow379) - (1.3333333333333333 * fSlow377)) * fSlow36));
		double fSlow381 = (2.5 * (fSlow376 * fSlow58));
		double fSlow382 = (2.25 * (fSlow379 * fSlow68));
		double fSlow383 = (0.95346258924559235 * (cos((fSlow85 + -1.2490457723982544)) * fSlow87));
		double fSlow384 = (fSlow373 + fSlow383);
		double fSlow385 = mydsp_faustpower2_f(fSlow384);
		double fSlow386 = ((3.0 * fSlow385) + -1.0);
		double fSlow387 = ((2.5 * fSlow386) + -2.0);
		double fSlow388 = (2.333333333333333 * (fSlow387 * fSlow19));
		double fSlow389 = ((2.333333333333333 * (fSlow385 * fSlow387)) - (1.5 * fSlow386));
		double fSlow390 = (2.2000000000000002 * (((2.25 * fSlow389) - (1.3333333333333333 * fSlow387)) * fSlow36));
		double fSlow391 = (2.5 * (fSlow386 * fSlow58));
		double fSlow392 = (2.25 * (fSlow389 * fSlow68));
		double fSlow393 = (0.95346258924559235 * (cos((fSlow85 + -1.8925468811915387)) * fSlow87));
		double fSlow394 = (fSlow373 + fSlow393);
		double fSlow395 = mydsp_faustpower2_f(fSlow394);
		double fSlow396 = ((3.0 * fSlow395) + -1.0);
		double fSlow397 = ((2.5 * fSlow396) + -2.0);
		double fSlow398 = (2.333333333333333 * (fSlow397 * fSlow19));
		double fSlow399 = ((2.333333333333333 * (fSlow395 * fSlow397)) - (1.5 * fSlow396));
		double fSlow400 = (2.2000000000000002 * (((2.25 * fSlow399) - (1.3333333333333333 * fSlow397)) * fSlow36));
		double fSlow401 = (2.5 * (fSlow396 * fSlow58));
		double fSlow402 = (2.25 * (fSlow399 * fSlow68));
		double fSlow403 = (0.95346258924559235 * (cos((fSlow85 + -2.8198420991931505)) * fSlow87));
		double fSlow404 = (fSlow373 + fSlow403);
		double fSlow405 = mydsp_faustpower2_f(fSlow404);
		double fSlow406 = ((3.0 * fSlow405) + -1.0);
		double fSlow407 = ((2.5 * fSlow406) + -2.0);
		double fSlow408 = (2.333333333333333 * (fSlow407 * fSlow19));
		double fSlow409 = ((2.333333333333333 * (fSlow405 * fSlow407)) - (1.5 * fSlow406));
		double fSlow410 = (2.2000000000000002 * (((2.25 * fSlow409) - (1.3333333333333333 * fSlow407)) * fSlow36));
		double fSlow411 = (2.5 * (fSlow406 * fSlow58));
		double fSlow412 = (2.25 * (fSlow409 * fSlow68));
		double fSlow413 = (0.95346258924559235 * (cos((fSlow85 + -3.4633432079864357)) * fSlow87));
		double fSlow414 = (fSlow373 + fSlow413);
		double fSlow415 = mydsp_faustpower2_f(fSlow414);
		double fSlow416 = ((3.0 * fSlow415) + -1.0);
		double fSlow417 = ((2.5 * fSlow416) + -2.0);
		double fSlow418 = (2.333333333333333 * (fSlow417 * fSlow19));
		double fSlow419 = ((2.333333333333333 * (fSlow415 * fSlow417)) - (1.5 * fSlow416));
		double fSlow420 = (2.2000000000000002 * (((2.25 * fSlow419) - (1.3333333333333333 * fSlow417)) * fSlow36));
		double fSlow421 = (2.5 * (fSlow416 * fSlow58));
		double fSlow422 = (2.25 * (fSlow419 * fSlow68));
		double fSlow423 = (0.95346258924559235 * (cos((fSlow85 + -4.3906384259880475)) * fSlow87));
		double fSlow424 = (fSlow373 + fSlow423);
		double fSlow425 = mydsp_faustpower2_f(fSlow424);
		double fSlow426 = ((3.0 * fSlow425) + -1.0);
		double fSlow427 = ((2.5 * fSlow426) + -2.0);
		double fSlow428 = (2.333333333333333 * (fSlow427 * fSlow19));
		double fSlow429 = ((2.333333333333333 * (fSlow425 * fSlow427)) - (1.5 * fSlow426));
		double fSlow430 = (2.2000000000000002 * (((2.25 * fSlow429) - (1.3333333333333333 * fSlow427)) * fSlow36));
		double fSlow431 = (2.5 * (fSlow426 * fSlow58));
		double fSlow432 = (2.25 * (fSlow429 * fSlow68));
		double fSlow433 = (0.95346258924559235 * (cos((fSlow85 + -5.0341395347813318)) * fSlow87));
		double fSlow434 = (fSlow373 + fSlow433);
		double fSlow435 = mydsp_faustpower2_f(fSlow434);
		double fSlow436 = ((3.0 * fSlow435) + -1.0);
		double fSlow437 = ((2.5 * fSlow436) + -2.0);
		double fSlow438 = (2.333333333333333 * (fSlow437 * fSlow19));
		double fSlow439 = ((2.333333333333333 * (fSlow435 * fSlow437)) - (1.5 * fSlow436));
		double fSlow440 = (2.2000000000000002 * (((2.25 * fSlow439) - (1.3333333333333333 * fSlow437)) * fSlow36));
		double fSlow441 = (2.5 * (fSlow436 * fSlow58));
		double fSlow442 = (2.25 * (fSlow439 * fSlow68));
		double fSlow443 = (0.95346258924559235 * (cos((fSlow85 + -5.9614347527829432)) * fSlow87));
		double fSlow444 = (fSlow373 + fSlow443);
		double fSlow445 = mydsp_faustpower2_f(fSlow444);
		double fSlow446 = ((3.0 * fSlow445) + -1.0);
		double fSlow447 = ((2.5 * fSlow446) + -2.0);
		double fSlow448 = (2.333333333333333 * (fSlow447 * fSlow19));
		double fSlow449 = ((2.333333333333333 * (fSlow445 * fSlow447)) - (1.5 * fSlow446));
		double fSlow450 = (2.2000000000000002 * (((2.25 * fSlow449) - (1.3333333333333333 * fSlow447)) * fSlow36));
		double fSlow451 = (2.5 * (fSlow446 * fSlow58));
		double fSlow452 = (2.25 * (fSlow449 * fSlow68));
		double fSlow453 = (fSlow372 - fSlow373);
		double fSlow454 = mydsp_faustpower2_f(fSlow453);
		double fSlow455 = ((3.0 * fSlow454) + -1.0);
		double fSlow456 = ((2.5 * fSlow455) + -2.0);
		double fSlow457 = (2.333333333333333 * (fSlow456 * fSlow19));
		double fSlow458 = ((2.333333333333333 * (fSlow454 * fSlow456)) - (1.5 * fSlow455));
		double fSlow459 = (2.2000000000000002 * (((2.25 * fSlow458) - (1.3333333333333333 * fSlow456)) * fSlow36));
		double fSlow460 = (2.5 * (fSlow455 * fSlow58));
		double fSlow461 = (2.25 * (fSlow458 * fSlow68));
		double fSlow462 = (fSlow383 - fSlow373);
		double fSlow463 = mydsp_faustpower2_f(fSlow462);
		double fSlow464 = ((3.0 * fSlow463) + -1.0);
		double fSlow465 = ((2.5 * fSlow464) + -2.0);
		double fSlow466 = (2.333333333333333 * (fSlow465 * fSlow19));
		double fSlow467 = ((2.333333333333333 * (fSlow463 * fSlow465)) - (1.5 * fSlow464));
		double fSlow468 = (2.2000000000000002 * (((2.25 * fSlow467) - (1.3333333333333333 * fSlow465)) * fSlow36));
		double fSlow469 = (2.5 * (fSlow464 * fSlow58));
		double fSlow470 = (2.25 * (fSlow467 * fSlow68));
		double fSlow471 = (fSlow393 - fSlow373);
		double fSlow472 = mydsp_faustpower2_f(fSlow471);
		double fSlow473 = ((3.0 * fSlow472) + -1.0);
		double fSlow474 = ((2.5 * fSlow473) + -2.0);
		double fSlow475 = (2.333333333333333 * (fSlow474 * fSlow19));
		double fSlow476 = ((2.333333333333333 * (fSlow472 * fSlow474)) - (1.5 * fSlow473));
		double fSlow477 = (2.2000000000000002 * (((2.25 * fSlow476) - (1.3333333333333333 * fSlow474)) * fSlow36));
		double fSlow478 = (2.5 * (fSlow473 * fSlow58));
		double fSlow479 = (2.25 * (fSlow476 * fSlow68));
		double fSlow480 = (fSlow403 - fSlow373);
		double fSlow481 = mydsp_faustpower2_f(fSlow480);
		double fSlow482 = ((3.0 * fSlow481) + -1.0);
		double fSlow483 = ((2.5 * fSlow482) + -2.0);
		double fSlow484 = (2.333333333333333 * (fSlow483 * fSlow19));
		double fSlow485 = ((2.333333333333333 * (fSlow481 * fSlow483)) - (1.5 * fSlow482));
		double fSlow486 = (2.2000000000000002 * (((2.25 * fSlow485) - (1.3333333333333333 * fSlow483)) * fSlow36));
		double fSlow487 = (2.5 * (fSlow482 * fSlow58));
		double fSlow488 = (2.25 * (fSlow485 * fSlow68));
		double fSlow489 = (fSlow413 - fSlow373);
		double fSlow490 = mydsp_faustpower2_f(fSlow489);
		double fSlow491 = ((3.0 * fSlow490) + -1.0);
		double fSlow492 = ((2.5 * fSlow491) + -2.0);
		double fSlow493 = (2.333333333333333 * (fSlow492 * fSlow19));
		double fSlow494 = ((2.333333333333333 * (fSlow490 * fSlow492)) - (1.5 * fSlow491));
		double fSlow495 = (2.2000000000000002 * (((2.25 * fSlow494) - (1.3333333333333333 * fSlow492)) * fSlow36));
		double fSlow496 = (2.5 * (fSlow491 * fSlow58));
		double fSlow497 = (2.25 * (fSlow494 * fSlow68));
		double fSlow498 = (fSlow423 - fSlow373);
		double fSlow499 = mydsp_faustpower2_f(fSlow498);
		double fSlow500 = ((3.0 * fSlow499) + -1.0);
		double fSlow501 = ((2.5 * fSlow500) + -2.0);
		double fSlow502 = (2.333333333333333 * (fSlow501 * fSlow19));
		double fSlow503 = ((2.333333333333333 * (fSlow499 * fSlow501)) - (1.5 * fSlow500));
		double fSlow504 = (2.2000000000000002 * (((2.25 * fSlow503) - (1.3333333333333333 * fSlow501)) * fSlow36));
		double fSlow505 = (2.5 * (fSlow500 * fSlow58));
		double fSlow506 = (2.25 * (fSlow503 * fSlow68));
		double fSlow507 = (fSlow433 - fSlow373);
		double fSlow508 = mydsp_faustpower2_f(fSlow507);
		double fSlow509 = ((3.0 * fSlow508) + -1.0);
		double fSlow510 = ((2.5 * fSlow509) + -2.0);
		double fSlow511 = (2.333333333333333 * (fSlow510 * fSlow19));
		double fSlow512 = ((2.333333333333333 * (fSlow508 * fSlow510)) - (1.5 * fSlow509));
		double fSlow513 = (2.2000000000000002 * (((2.25 * fSlow512) - (1.3333333333333333 * fSlow510)) * fSlow36));
		double fSlow514 = (2.5 * (fSlow509 * fSlow58));
		double fSlow515 = (2.25 * (fSlow512 * fSlow68));
		double fSlow516 = (fSlow443 - fSlow373);
		double fSlow517 = mydsp_faustpower2_f(fSlow516);
		double fSlow518 = ((3.0 * fSlow517) + -1.0);
		double fSlow519 = ((2.5 * fSlow518) + -2.0);
		double fSlow520 = (2.333333333333333 * (fSlow519 * fSlow19));
		double fSlow521 = ((2.333333333333333 * (fSlow517 * fSlow519)) - (1.5 * fSlow518));
		double fSlow522 = (2.2000000000000002 * (((2.25 * fSlow521) - (1.3333333333333333 * fSlow519)) * fSlow36));
		double fSlow523 = (2.5 * (fSlow518 * fSlow58));
		double fSlow524 = (2.25 * (fSlow521 * fSlow68));
		double fSlow525 = (fSlow331 - fSlow332);
		double fSlow526 = mydsp_faustpower2_f(fSlow525);
		double fSlow527 = ((3.0 * fSlow526) + -1.0);
		double fSlow528 = ((2.5 * fSlow527) + -2.0);
		double fSlow529 = (2.333333333333333 * (fSlow528 * fSlow19));
		double fSlow530 = ((2.333333333333333 * (fSlow526 * fSlow528)) - (1.5 * fSlow527));
		double fSlow531 = (2.2000000000000002 * (((2.25 * fSlow530) - (1.3333333333333333 * fSlow528)) * fSlow36));
		double fSlow532 = (2.5 * (fSlow527 * fSlow58));
		double fSlow533 = (2.25 * (fSlow530 * fSlow68));
		double fSlow534 = (fSlow342 - fSlow332);
		double fSlow535 = mydsp_faustpower2_f(fSlow534);
		double fSlow536 = ((3.0 * fSlow535) + -1.0);
		double fSlow537 = ((2.5 * fSlow536) + -2.0);
		double fSlow538 = (2.333333333333333 * (fSlow537 * fSlow19));
		double fSlow539 = ((2.333333333333333 * (fSlow535 * fSlow537)) - (1.5 * fSlow536));
		double fSlow540 = (2.2000000000000002 * (((2.25 * fSlow539) - (1.3333333333333333 * fSlow537)) * fSlow36));
		double fSlow541 = (2.5 * (fSlow536 * fSlow58));
		double fSlow542 = (2.25 * (fSlow539 * fSlow68));
		double fSlow543 = (fSlow352 - fSlow332);
		double fSlow544 = mydsp_faustpower2_f(fSlow543);
		double fSlow545 = ((3.0 * fSlow544) + -1.0);
		double fSlow546 = ((2.5 * fSlow545) + -2.0);
		double fSlow547 = (2.333333333333333 * (fSlow546 * fSlow19));
		double fSlow548 = ((2.333333333333333 * (fSlow544 * fSlow546)) - (1.5 * fSlow545));
		double fSlow549 = (2.2000000000000002 * (((2.25 * fSlow548) - (1.3333333333333333 * fSlow546)) * fSlow36));
		double fSlow550 = (2.5 * (fSlow545 * fSlow58));
		double fSlow551 = (2.25 * (fSlow548 * fSlow68));
		double fSlow552 = (fSlow362 - fSlow332);
		double fSlow553 = mydsp_faustpower2_f(fSlow552);
		double fSlow554 = ((3.0 * fSlow553) + -1.0);
		double fSlow555 = ((2.5 * fSlow554) + -2.0);
		double fSlow556 = (2.333333333333333 * (fSlow555 * fSlow19));
		double fSlow557 = ((2.333333333333333 * (fSlow553 * fSlow555)) - (1.5 * fSlow554));
		double fSlow558 = (2.2000000000000002 * (((2.25 * fSlow557) - (1.3333333333333333 * fSlow555)) * fSlow36));
		double fSlow559 = (2.5 * (fSlow554 * fSlow58));
		double fSlow560 = (2.25 * (fSlow557 * fSlow68));
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
			fRec24[0] = (fRec24[1] + fRec23[1]);
			fRec25[0] = (fRec25[1] + fRec24[1]);
			fRec23[0] = (fSlow43 * ((fSlow45 * fTemp0) - ((fSlow47 * fRec24[0]) + (fSlow46 * fRec25[0]))));
			fRec26[0] = (fRec23[0] + fRec26[1]);
			double fTemp5 = (fRec26[0] - fRec23[0]);
			fVec1[0] = fTemp5;
			fRec27[0] = (fRec27[1] + fVec1[1]);
			fRec28[0] = (fRec28[1] + fRec22[1]);
			fRec29[0] = (fRec29[1] + fRec28[1]);
			fRec22[0] = (fSlow41 * ((fSlow42 * (fRec23[0] + (fSlow48 * ((fSlow50 * fTemp5) + (fSlow49 * fRec27[0]))))) - ((fSlow52 * fRec28[0]) + (fSlow51 * fRec29[0]))));
			fRec30[0] = (fRec22[0] + fRec30[1]);
			double fTemp6 = (fRec30[0] - fRec22[0]);
			fVec2[0] = fTemp6;
			fRec31[0] = (fRec31[1] + fVec2[1]);
			fRec32[0] = (fRec32[1] + fRec21[1]);
			fRec21[0] = (fSlow40 * ((fSlow38 * (fRec22[0] + (fSlow53 * ((fSlow55 * fTemp6) + (fSlow54 * fRec31[0]))))) - (fSlow56 * fRec32[0])));
			fRec20[0] = (fRec21[0] + fRec20[1]);
			fRec36[0] = (fRec36[1] + fRec35[1]);
			fRec37[0] = (fRec37[1] + fRec36[1]);
			fRec35[0] = (fSlow43 * (fTemp1 - ((fSlow47 * fRec36[0]) + (fSlow46 * fRec37[0]))));
			fRec38[0] = (fRec38[1] + fRec34[1]);
			fRec39[0] = (fRec39[1] + fRec38[1]);
			fRec34[0] = (fSlow41 * (fRec35[0] - ((fSlow52 * fRec38[0]) + (fSlow51 * fRec39[0]))));
			fRec40[0] = (fRec40[1] + fRec33[1]);
			fRec33[0] = (fSlow40 * (fRec34[0] - (fSlow56 * fRec40[0])));
			double fTemp7 = ((fSlow39 * (fRec20[0] - fRec21[0])) + (fRec21[0] + fRec33[0]));
			double fTemp8 = (fSlow57 * fTemp0);
			fRec43[0] = (fRec43[1] + fRec42[1]);
			fRec44[0] = (fRec44[1] + fRec43[1]);
			fRec42[0] = (fSlow64 * ((fSlow65 * fTemp0) - ((fSlow67 * fRec43[0]) + (fSlow66 * fRec44[0]))));
			fRec41[0] = (fRec42[0] + fRec41[1]);
			double fTemp9 = (fRec41[0] - fRec42[0]);
			fVec3[0] = fTemp9;
			fRec45[0] = (fRec45[1] + fVec3[1]);
			fRec47[0] = (fRec47[1] + fRec46[1]);
			fRec48[0] = (fRec48[1] + fRec47[1]);
			fRec46[0] = (fSlow64 * (fTemp1 - ((fSlow67 * fRec47[0]) + (fSlow66 * fRec48[0]))));
			double fTemp10 = ((fSlow61 * ((fSlow63 * fTemp9) + (fSlow62 * fRec45[0]))) + (fRec42[0] + fRec46[0]));
			fRec52[0] = (fRec52[1] + fRec51[1]);
			fRec53[0] = (fRec53[1] + fRec52[1]);
			fRec51[0] = (fSlow75 * ((fSlow77 * fTemp0) - ((fSlow79 * fRec52[0]) + (fSlow78 * fRec53[0]))));
			fRec54[0] = (fRec51[0] + fRec54[1]);
			double fTemp11 = (fRec54[0] - fRec51[0]);
			fVec4[0] = fTemp11;
			fRec55[0] = (fRec55[1] + fVec4[1]);
			fRec56[0] = (fRec56[1] + fRec50[1]);
			fRec57[0] = (fRec57[1] + fRec56[1]);
			fRec50[0] = (fSlow74 * ((fSlow70 * (fRec51[0] + (fSlow80 * ((fSlow82 * fTemp11) + (fSlow81 * fRec55[0]))))) - ((fSlow84 * fRec56[0]) + (fSlow83 * fRec57[0]))));
			fRec49[0] = (fRec50[0] + fRec49[1]);
			double fTemp12 = (fRec49[0] - fRec50[0]);
			fVec5[0] = fTemp12;
			fRec58[0] = (fRec58[1] + fVec5[1]);
			fRec61[0] = (fRec61[1] + fRec60[1]);
			fRec62[0] = (fRec62[1] + fRec61[1]);
			fRec60[0] = (fSlow75 * (fTemp1 - ((fSlow79 * fRec61[0]) + (fSlow78 * fRec62[0]))));
			fRec63[0] = (fRec63[1] + fRec59[1]);
			fRec64[0] = (fRec64[1] + fRec63[1]);
			fRec59[0] = (fSlow74 * (fRec60[0] - ((fSlow84 * fRec63[0]) + (fSlow83 * fRec64[0]))));
			double fTemp13 = ((fSlow71 * ((fSlow73 * fTemp12) + (fSlow72 * fRec58[0]))) + (fRec50[0] + fRec59[0]));
			double fTemp14 = (0.012698412698412698 * ((fSlow1 * ((fTemp2 + (fSlow20 * fTemp4)) + (fSlow37 * fTemp7))) + ((fTemp8 + (fSlow59 * fTemp10)) + (fSlow69 * fTemp13))));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fVbargraph0 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp14);
			double fTemp15 = (0.012698412698412698 * ((fSlow88 * ((fTemp2 + (fSlow93 * fTemp4)) + (fSlow95 * fTemp7))) + ((fTemp8 + (fSlow96 * fTemp10)) + (fSlow97 * fTemp13))));
			fRec65[0] = max((fRec65[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph1 = FAUSTFLOAT(fRec65[0]);
			output1[i] = FAUSTFLOAT(fTemp15);
			double fTemp16 = (0.012698412698412698 * ((fSlow99 * ((fTemp2 + (fSlow103 * fTemp4)) + (fSlow105 * fTemp7))) + ((fTemp8 + (fSlow106 * fTemp10)) + (fSlow107 * fTemp13))));
			fRec66[0] = max((fRec66[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph2 = FAUSTFLOAT(fRec66[0]);
			output2[i] = FAUSTFLOAT(fTemp16);
			double fTemp17 = (0.012698412698412698 * ((fSlow109 * ((fTemp2 + (fSlow113 * fTemp4)) + (fSlow115 * fTemp7))) + ((fTemp8 + (fSlow116 * fTemp10)) + (fSlow117 * fTemp13))));
			fRec67[0] = max((fRec67[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph3 = FAUSTFLOAT(fRec67[0]);
			output3[i] = FAUSTFLOAT(fTemp17);
			double fTemp18 = (0.012698412698412698 * ((fSlow119 * ((fTemp2 + (fSlow123 * fTemp4)) + (fSlow125 * fTemp7))) + ((fTemp8 + (fSlow126 * fTemp10)) + (fSlow127 * fTemp13))));
			fRec68[0] = max((fRec68[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph4 = FAUSTFLOAT(fRec68[0]);
			output4[i] = FAUSTFLOAT(fTemp18);
			double fTemp19 = (0.012698412698412698 * ((fSlow128 * ((fTemp2 + (fSlow132 * fTemp4)) + (fSlow134 * fTemp7))) + ((fTemp8 + (fSlow135 * fTemp10)) + (fSlow136 * fTemp13))));
			fRec69[0] = max((fRec69[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph5 = FAUSTFLOAT(fRec69[0]);
			output5[i] = FAUSTFLOAT(fTemp19);
			double fTemp20 = (0.022574955908289243 * ((fSlow139 * ((fTemp2 + (fSlow143 * fTemp4)) + (fSlow145 * fTemp7))) + ((fTemp8 + (fSlow146 * fTemp10)) + (fSlow147 * fTemp13))));
			fRec70[0] = max((fRec70[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph6 = FAUSTFLOAT(fRec70[0]);
			output6[i] = FAUSTFLOAT(fTemp20);
			double fTemp21 = (0.022574955908289243 * ((fSlow149 * ((fTemp2 + (fSlow153 * fTemp4)) + (fSlow155 * fTemp7))) + ((fTemp8 + (fSlow156 * fTemp10)) + (fSlow157 * fTemp13))));
			fRec71[0] = max((fRec71[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph7 = FAUSTFLOAT(fRec71[0]);
			output7[i] = FAUSTFLOAT(fTemp21);
			double fTemp22 = (0.022574955908289243 * ((fSlow159 * ((fTemp2 + (fSlow163 * fTemp4)) + (fSlow165 * fTemp7))) + ((fTemp8 + (fSlow166 * fTemp10)) + (fSlow167 * fTemp13))));
			fRec72[0] = max((fRec72[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph8 = FAUSTFLOAT(fRec72[0]);
			output8[i] = FAUSTFLOAT(fTemp22);
			double fTemp23 = (0.022574955908289243 * ((fSlow169 * ((fTemp2 + (fSlow173 * fTemp4)) + (fSlow175 * fTemp7))) + ((fTemp8 + (fSlow176 * fTemp10)) + (fSlow177 * fTemp13))));
			fRec73[0] = max((fRec73[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fVbargraph9 = FAUSTFLOAT(fRec73[0]);
			output9[i] = FAUSTFLOAT(fTemp23);
			double fTemp24 = (0.022574955908289243 * ((fSlow179 * ((fTemp2 + (fSlow183 * fTemp4)) + (fSlow185 * fTemp7))) + ((fTemp8 + (fSlow186 * fTemp10)) + (fSlow187 * fTemp13))));
			fRec74[0] = max((fRec74[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph10 = FAUSTFLOAT(fRec74[0]);
			output10[i] = FAUSTFLOAT(fTemp24);
			double fTemp25 = (0.022574955908289243 * ((fSlow189 * ((fTemp2 + (fSlow193 * fTemp4)) + (fSlow195 * fTemp7))) + ((fTemp8 + (fSlow196 * fTemp10)) + (fSlow197 * fTemp13))));
			fRec75[0] = max((fRec75[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph11 = FAUSTFLOAT(fRec75[0]);
			output11[i] = FAUSTFLOAT(fTemp25);
			double fTemp26 = (0.022574955908289243 * ((fSlow199 * ((fTemp2 + (fSlow203 * fTemp4)) + (fSlow205 * fTemp7))) + ((fTemp8 + (fSlow206 * fTemp10)) + (fSlow207 * fTemp13))));
			fRec76[0] = max((fRec76[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fVbargraph12 = FAUSTFLOAT(fRec76[0]);
			output12[i] = FAUSTFLOAT(fTemp26);
			double fTemp27 = (0.022574955908289243 * ((fSlow209 * ((fTemp2 + (fSlow213 * fTemp4)) + (fSlow215 * fTemp7))) + ((fTemp8 + (fSlow216 * fTemp10)) + (fSlow217 * fTemp13))));
			fRec77[0] = max((fRec77[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph13 = FAUSTFLOAT(fRec77[0]);
			output13[i] = FAUSTFLOAT(fTemp27);
			double fTemp28 = (0.022574955908289243 * ((fSlow218 * ((fTemp2 + (fSlow222 * fTemp4)) + (fSlow224 * fTemp7))) + ((fTemp8 + (fSlow225 * fTemp10)) + (fSlow226 * fTemp13))));
			fRec78[0] = max((fRec78[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fVbargraph14 = FAUSTFLOAT(fRec78[0]);
			output14[i] = FAUSTFLOAT(fTemp28);
			double fTemp29 = (0.022574955908289243 * ((fSlow227 * ((fTemp2 + (fSlow231 * fTemp4)) + (fSlow233 * fTemp7))) + ((fTemp8 + (fSlow234 * fTemp10)) + (fSlow235 * fTemp13))));
			fRec79[0] = max((fRec79[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph15 = FAUSTFLOAT(fRec79[0]);
			output15[i] = FAUSTFLOAT(fTemp29);
			double fTemp30 = (0.022574955908289243 * ((fSlow236 * ((fTemp2 + (fSlow240 * fTemp4)) + (fSlow242 * fTemp7))) + ((fTemp8 + (fSlow243 * fTemp10)) + (fSlow244 * fTemp13))));
			fRec80[0] = max((fRec80[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph16 = FAUSTFLOAT(fRec80[0]);
			output16[i] = FAUSTFLOAT(fTemp30);
			double fTemp31 = (0.022574955908289243 * ((fSlow245 * ((fTemp2 + (fSlow249 * fTemp4)) + (fSlow251 * fTemp7))) + ((fTemp8 + (fSlow252 * fTemp10)) + (fSlow253 * fTemp13))));
			fRec81[0] = max((fRec81[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph17 = FAUSTFLOAT(fRec81[0]);
			output17[i] = FAUSTFLOAT(fTemp31);
			double fTemp32 = (0.021093750000000001 * ((fSlow256 * ((fTemp2 + (fSlow260 * fTemp4)) + (fSlow262 * fTemp7))) + ((fTemp8 + (fSlow263 * fTemp10)) + (fSlow264 * fTemp13))));
			fRec82[0] = max((fRec82[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph18 = FAUSTFLOAT(fRec82[0]);
			output18[i] = FAUSTFLOAT(fTemp32);
			double fTemp33 = (0.021093750000000001 * ((fSlow266 * ((fTemp2 + (fSlow270 * fTemp4)) + (fSlow272 * fTemp7))) + ((fTemp8 + (fSlow273 * fTemp10)) + (fSlow274 * fTemp13))));
			fRec83[0] = max((fRec83[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph19 = FAUSTFLOAT(fRec83[0]);
			output19[i] = FAUSTFLOAT(fTemp33);
			double fTemp34 = (0.021093750000000001 * ((fSlow276 * ((fTemp2 + (fSlow280 * fTemp4)) + (fSlow282 * fTemp7))) + ((fTemp8 + (fSlow283 * fTemp10)) + (fSlow284 * fTemp13))));
			fRec84[0] = max((fRec84[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fVbargraph20 = FAUSTFLOAT(fRec84[0]);
			output20[i] = FAUSTFLOAT(fTemp34);
			double fTemp35 = (0.021093750000000001 * ((fSlow286 * ((fTemp2 + (fSlow290 * fTemp4)) + (fSlow292 * fTemp7))) + ((fTemp8 + (fSlow293 * fTemp10)) + (fSlow294 * fTemp13))));
			fRec85[0] = max((fRec85[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph21 = FAUSTFLOAT(fRec85[0]);
			output21[i] = FAUSTFLOAT(fTemp35);
			double fTemp36 = (0.021093750000000001 * ((fSlow295 * ((fTemp2 + (fSlow299 * fTemp4)) + (fSlow301 * fTemp7))) + ((fTemp8 + (fSlow302 * fTemp10)) + (fSlow303 * fTemp13))));
			fRec86[0] = max((fRec86[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fVbargraph22 = FAUSTFLOAT(fRec86[0]);
			output22[i] = FAUSTFLOAT(fTemp36);
			double fTemp37 = (0.021093750000000001 * ((fSlow304 * ((fTemp2 + (fSlow308 * fTemp4)) + (fSlow310 * fTemp7))) + ((fTemp8 + (fSlow311 * fTemp10)) + (fSlow312 * fTemp13))));
			fRec87[0] = max((fRec87[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fVbargraph23 = FAUSTFLOAT(fRec87[0]);
			output23[i] = FAUSTFLOAT(fTemp37);
			double fTemp38 = (0.021093750000000001 * ((fSlow313 * ((fTemp2 + (fSlow317 * fTemp4)) + (fSlow319 * fTemp7))) + ((fTemp8 + (fSlow320 * fTemp10)) + (fSlow321 * fTemp13))));
			fRec88[0] = max((fRec88[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fVbargraph24 = FAUSTFLOAT(fRec88[0]);
			output24[i] = FAUSTFLOAT(fTemp38);
			double fTemp39 = (0.021093750000000001 * ((fSlow322 * ((fTemp2 + (fSlow326 * fTemp4)) + (fSlow328 * fTemp7))) + ((fTemp8 + (fSlow329 * fTemp10)) + (fSlow330 * fTemp13))));
			fRec89[0] = max((fRec89[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph25 = FAUSTFLOAT(fRec89[0]);
			output25[i] = FAUSTFLOAT(fTemp39);
			double fTemp40 = (0.02017333553791887 * ((fSlow333 * ((fTemp2 + (fSlow337 * fTemp4)) + (fSlow339 * fTemp7))) + ((fTemp8 + (fSlow340 * fTemp10)) + (fSlow341 * fTemp13))));
			fRec90[0] = max((fRec90[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fVbargraph26 = FAUSTFLOAT(fRec90[0]);
			output26[i] = FAUSTFLOAT(fTemp40);
			double fTemp41 = (0.02017333553791887 * ((fSlow343 * ((fTemp2 + (fSlow347 * fTemp4)) + (fSlow349 * fTemp7))) + ((fTemp8 + (fSlow350 * fTemp10)) + (fSlow351 * fTemp13))));
			fRec91[0] = max((fRec91[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph27 = FAUSTFLOAT(fRec91[0]);
			output27[i] = FAUSTFLOAT(fTemp41);
			double fTemp42 = (0.02017333553791887 * ((fSlow353 * ((fTemp2 + (fSlow357 * fTemp4)) + (fSlow359 * fTemp7))) + ((fTemp8 + (fSlow360 * fTemp10)) + (fSlow361 * fTemp13))));
			fRec92[0] = max((fRec92[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fVbargraph28 = FAUSTFLOAT(fRec92[0]);
			output28[i] = FAUSTFLOAT(fTemp42);
			double fTemp43 = (0.02017333553791887 * ((fSlow363 * ((fTemp2 + (fSlow367 * fTemp4)) + (fSlow369 * fTemp7))) + ((fTemp8 + (fSlow370 * fTemp10)) + (fSlow371 * fTemp13))));
			fRec93[0] = max((fRec93[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph29 = FAUSTFLOAT(fRec93[0]);
			output29[i] = FAUSTFLOAT(fTemp43);
			double fTemp44 = (0.02017333553791887 * ((fSlow374 * ((fTemp2 + (fSlow378 * fTemp4)) + (fSlow380 * fTemp7))) + ((fTemp8 + (fSlow381 * fTemp10)) + (fSlow382 * fTemp13))));
			fRec94[0] = max((fRec94[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fVbargraph30 = FAUSTFLOAT(fRec94[0]);
			output30[i] = FAUSTFLOAT(fTemp44);
			double fTemp45 = (0.02017333553791887 * ((fSlow384 * ((fTemp2 + (fSlow388 * fTemp4)) + (fSlow390 * fTemp7))) + ((fTemp8 + (fSlow391 * fTemp10)) + (fSlow392 * fTemp13))));
			fRec95[0] = max((fRec95[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fVbargraph31 = FAUSTFLOAT(fRec95[0]);
			output31[i] = FAUSTFLOAT(fTemp45);
			double fTemp46 = (0.02017333553791887 * ((fSlow394 * ((fTemp2 + (fSlow398 * fTemp4)) + (fSlow400 * fTemp7))) + ((fTemp8 + (fSlow401 * fTemp10)) + (fSlow402 * fTemp13))));
			fRec96[0] = max((fRec96[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph32 = FAUSTFLOAT(fRec96[0]);
			output32[i] = FAUSTFLOAT(fTemp46);
			double fTemp47 = (0.02017333553791887 * ((fSlow404 * ((fTemp2 + (fSlow408 * fTemp4)) + (fSlow410 * fTemp7))) + ((fTemp8 + (fSlow411 * fTemp10)) + (fSlow412 * fTemp13))));
			fRec97[0] = max((fRec97[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fVbargraph33 = FAUSTFLOAT(fRec97[0]);
			output33[i] = FAUSTFLOAT(fTemp47);
			double fTemp48 = (0.02017333553791887 * ((fSlow414 * ((fTemp2 + (fSlow418 * fTemp4)) + (fSlow420 * fTemp7))) + ((fTemp8 + (fSlow421 * fTemp10)) + (fSlow422 * fTemp13))));
			fRec98[0] = max((fRec98[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph34 = FAUSTFLOAT(fRec98[0]);
			output34[i] = FAUSTFLOAT(fTemp48);
			double fTemp49 = (0.02017333553791887 * ((fSlow424 * ((fTemp2 + (fSlow428 * fTemp4)) + (fSlow430 * fTemp7))) + ((fTemp8 + (fSlow431 * fTemp10)) + (fSlow432 * fTemp13))));
			fRec99[0] = max((fRec99[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fVbargraph35 = FAUSTFLOAT(fRec99[0]);
			output35[i] = FAUSTFLOAT(fTemp49);
			double fTemp50 = (0.02017333553791887 * ((fSlow434 * ((fTemp2 + (fSlow438 * fTemp4)) + (fSlow440 * fTemp7))) + ((fTemp8 + (fSlow441 * fTemp10)) + (fSlow442 * fTemp13))));
			fRec100[0] = max((fRec100[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph36 = FAUSTFLOAT(fRec100[0]);
			output36[i] = FAUSTFLOAT(fTemp50);
			double fTemp51 = (0.02017333553791887 * ((fSlow444 * ((fTemp2 + (fSlow448 * fTemp4)) + (fSlow450 * fTemp7))) + ((fTemp8 + (fSlow451 * fTemp10)) + (fSlow452 * fTemp13))));
			fRec101[0] = max((fRec101[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph37 = FAUSTFLOAT(fRec101[0]);
			output37[i] = FAUSTFLOAT(fTemp51);
			double fTemp52 = (0.02017333553791887 * ((fSlow453 * ((fTemp2 + (fSlow457 * fTemp4)) + (fSlow459 * fTemp7))) + ((fTemp8 + (fSlow460 * fTemp10)) + (fSlow461 * fTemp13))));
			fRec102[0] = max((fRec102[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fVbargraph38 = FAUSTFLOAT(fRec102[0]);
			output38[i] = FAUSTFLOAT(fTemp52);
			double fTemp53 = (0.02017333553791887 * ((fSlow462 * ((fTemp2 + (fSlow466 * fTemp4)) + (fSlow468 * fTemp7))) + ((fTemp8 + (fSlow469 * fTemp10)) + (fSlow470 * fTemp13))));
			fRec103[0] = max((fRec103[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fVbargraph39 = FAUSTFLOAT(fRec103[0]);
			output39[i] = FAUSTFLOAT(fTemp53);
			double fTemp54 = (0.02017333553791887 * ((fSlow471 * ((fTemp2 + (fSlow475 * fTemp4)) + (fSlow477 * fTemp7))) + ((fTemp8 + (fSlow478 * fTemp10)) + (fSlow479 * fTemp13))));
			fRec104[0] = max((fRec104[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fVbargraph40 = FAUSTFLOAT(fRec104[0]);
			output40[i] = FAUSTFLOAT(fTemp54);
			double fTemp55 = (0.02017333553791887 * ((fSlow480 * ((fTemp2 + (fSlow484 * fTemp4)) + (fSlow486 * fTemp7))) + ((fTemp8 + (fSlow487 * fTemp10)) + (fSlow488 * fTemp13))));
			fRec105[0] = max((fRec105[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph41 = FAUSTFLOAT(fRec105[0]);
			output41[i] = FAUSTFLOAT(fTemp55);
			double fTemp56 = (0.02017333553791887 * ((fSlow489 * ((fTemp2 + (fSlow493 * fTemp4)) + (fSlow495 * fTemp7))) + ((fTemp8 + (fSlow496 * fTemp10)) + (fSlow497 * fTemp13))));
			fRec106[0] = max((fRec106[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fVbargraph42 = FAUSTFLOAT(fRec106[0]);
			output42[i] = FAUSTFLOAT(fTemp56);
			double fTemp57 = (0.02017333553791887 * ((fSlow498 * ((fTemp2 + (fSlow502 * fTemp4)) + (fSlow504 * fTemp7))) + ((fTemp8 + (fSlow505 * fTemp10)) + (fSlow506 * fTemp13))));
			fRec107[0] = max((fRec107[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fVbargraph43 = FAUSTFLOAT(fRec107[0]);
			output43[i] = FAUSTFLOAT(fTemp57);
			double fTemp58 = (0.02017333553791887 * ((fSlow507 * ((fTemp2 + (fSlow511 * fTemp4)) + (fSlow513 * fTemp7))) + ((fTemp8 + (fSlow514 * fTemp10)) + (fSlow515 * fTemp13))));
			fRec108[0] = max((fRec108[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fVbargraph44 = FAUSTFLOAT(fRec108[0]);
			output44[i] = FAUSTFLOAT(fTemp58);
			double fTemp59 = (0.02017333553791887 * ((fSlow516 * ((fTemp2 + (fSlow520 * fTemp4)) + (fSlow522 * fTemp7))) + ((fTemp8 + (fSlow523 * fTemp10)) + (fSlow524 * fTemp13))));
			fRec109[0] = max((fRec109[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fVbargraph45 = FAUSTFLOAT(fRec109[0]);
			output45[i] = FAUSTFLOAT(fTemp59);
			double fTemp60 = (0.02017333553791887 * ((fSlow525 * ((fTemp2 + (fSlow529 * fTemp4)) + (fSlow531 * fTemp7))) + ((fTemp8 + (fSlow532 * fTemp10)) + (fSlow533 * fTemp13))));
			fRec110[0] = max((fRec110[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fVbargraph46 = FAUSTFLOAT(fRec110[0]);
			output46[i] = FAUSTFLOAT(fTemp60);
			double fTemp61 = (0.02017333553791887 * ((fSlow534 * ((fTemp2 + (fSlow538 * fTemp4)) + (fSlow540 * fTemp7))) + ((fTemp8 + (fSlow541 * fTemp10)) + (fSlow542 * fTemp13))));
			fRec111[0] = max((fRec111[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fVbargraph47 = FAUSTFLOAT(fRec111[0]);
			output47[i] = FAUSTFLOAT(fTemp61);
			double fTemp62 = (0.02017333553791887 * ((fSlow543 * ((fTemp2 + (fSlow547 * fTemp4)) + (fSlow549 * fTemp7))) + ((fTemp8 + (fSlow550 * fTemp10)) + (fSlow551 * fTemp13))));
			fRec112[0] = max((fRec112[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fVbargraph48 = FAUSTFLOAT(fRec112[0]);
			output48[i] = FAUSTFLOAT(fTemp62);
			double fTemp63 = (0.02017333553791887 * ((fSlow552 * ((fTemp2 + (fSlow556 * fTemp4)) + (fSlow558 * fTemp7))) + ((fTemp8 + (fSlow559 * fTemp10)) + (fSlow560 * fTemp13))));
			fRec113[0] = max((fRec113[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fVbargraph49 = FAUSTFLOAT(fRec113[0]);
			output49[i] = FAUSTFLOAT(fTemp63);
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
			fRec24[1] = fRec24[0];
			fRec25[1] = fRec25[0];
			fRec23[1] = fRec23[0];
			fRec26[1] = fRec26[0];
			fVec1[1] = fVec1[0];
			fRec27[1] = fRec27[0];
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec22[1] = fRec22[0];
			fRec30[1] = fRec30[0];
			fVec2[1] = fVec2[0];
			fRec31[1] = fRec31[0];
			fRec32[1] = fRec32[0];
			fRec21[1] = fRec21[0];
			fRec20[1] = fRec20[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec35[1] = fRec35[0];
			fRec38[1] = fRec38[0];
			fRec39[1] = fRec39[0];
			fRec34[1] = fRec34[0];
			fRec40[1] = fRec40[0];
			fRec33[1] = fRec33[0];
			fRec43[1] = fRec43[0];
			fRec44[1] = fRec44[0];
			fRec42[1] = fRec42[0];
			fRec41[1] = fRec41[0];
			fVec3[1] = fVec3[0];
			fRec45[1] = fRec45[0];
			fRec47[1] = fRec47[0];
			fRec48[1] = fRec48[0];
			fRec46[1] = fRec46[0];
			fRec52[1] = fRec52[0];
			fRec53[1] = fRec53[0];
			fRec51[1] = fRec51[0];
			fRec54[1] = fRec54[0];
			fVec4[1] = fVec4[0];
			fRec55[1] = fRec55[0];
			fRec56[1] = fRec56[0];
			fRec57[1] = fRec57[0];
			fRec50[1] = fRec50[0];
			fRec49[1] = fRec49[0];
			fVec5[1] = fVec5[0];
			fRec58[1] = fRec58[0];
			fRec61[1] = fRec61[0];
			fRec62[1] = fRec62[0];
			fRec60[1] = fRec60[0];
			fRec63[1] = fRec63[0];
			fRec64[1] = fRec64[0];
			fRec59[1] = fRec59[0];
			fRec0[1] = fRec0[0];
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
			fRec93[1] = fRec93[0];
			fRec94[1] = fRec94[0];
			fRec95[1] = fRec95[0];
			fRec96[1] = fRec96[0];
			fRec97[1] = fRec97[0];
			fRec98[1] = fRec98[0];
			fRec99[1] = fRec99[0];
			fRec100[1] = fRec100[0];
			fRec101[1] = fRec101[0];
			fRec102[1] = fRec102[0];
			fRec103[1] = fRec103[0];
			fRec104[1] = fRec104[0];
			fRec105[1] = fRec105[0];
			fRec106[1] = fRec106[0];
			fRec107[1] = fRec107[0];
			fRec108[1] = fRec108[0];
			fRec109[1] = fRec109[0];
			fRec110[1] = fRec110[0];
			fRec111[1] = fRec111[0];
			fRec112[1] = fRec112[0];
			fRec113[1] = fRec113[0];
			
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
