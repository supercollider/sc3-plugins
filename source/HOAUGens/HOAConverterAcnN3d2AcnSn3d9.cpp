/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2016"
license: "GPL"
name: "HOAConverterAcnN3d2AcnSn3d9"
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
	double fRec1[2];
	FAUSTFLOAT fVbargraph0;
	double fRec0[2];
	FAUSTFLOAT fVbargraph1;
	double fRec3[2];
	FAUSTFLOAT fVbargraph2;
	double fRec2[2];
	FAUSTFLOAT fVbargraph3;
	double fRec5[2];
	FAUSTFLOAT fVbargraph4;
	double fRec4[2];
	FAUSTFLOAT fVbargraph5;
	double fRec7[2];
	FAUSTFLOAT fVbargraph6;
	double fRec6[2];
	FAUSTFLOAT fVbargraph7;
	double fRec9[2];
	FAUSTFLOAT fVbargraph8;
	double fRec8[2];
	FAUSTFLOAT fVbargraph9;
	double fRec11[2];
	FAUSTFLOAT fVbargraph10;
	double fRec10[2];
	FAUSTFLOAT fVbargraph11;
	double fRec13[2];
	FAUSTFLOAT fVbargraph12;
	double fRec12[2];
	FAUSTFLOAT fVbargraph13;
	double fRec15[2];
	FAUSTFLOAT fVbargraph14;
	double fRec14[2];
	FAUSTFLOAT fVbargraph15;
	double fRec17[2];
	FAUSTFLOAT fVbargraph16;
	double fRec16[2];
	FAUSTFLOAT fVbargraph17;
	double fRec19[2];
	FAUSTFLOAT fVbargraph18;
	double fRec18[2];
	FAUSTFLOAT fVbargraph19;
	double fRec21[2];
	FAUSTFLOAT fVbargraph20;
	double fRec20[2];
	FAUSTFLOAT fVbargraph21;
	double fRec23[2];
	FAUSTFLOAT fVbargraph22;
	double fRec22[2];
	FAUSTFLOAT fVbargraph23;
	double fRec25[2];
	FAUSTFLOAT fVbargraph24;
	double fRec24[2];
	FAUSTFLOAT fVbargraph25;
	double fRec27[2];
	FAUSTFLOAT fVbargraph26;
	double fRec26[2];
	FAUSTFLOAT fVbargraph27;
	double fRec29[2];
	FAUSTFLOAT fVbargraph28;
	double fRec28[2];
	FAUSTFLOAT fVbargraph29;
	double fRec31[2];
	FAUSTFLOAT fVbargraph30;
	double fRec30[2];
	FAUSTFLOAT fVbargraph31;
	double fRec33[2];
	FAUSTFLOAT fVbargraph32;
	double fRec32[2];
	FAUSTFLOAT fVbargraph33;
	double fRec35[2];
	FAUSTFLOAT fVbargraph34;
	double fRec34[2];
	FAUSTFLOAT fVbargraph35;
	double fRec37[2];
	FAUSTFLOAT fVbargraph36;
	double fRec36[2];
	FAUSTFLOAT fVbargraph37;
	double fRec39[2];
	FAUSTFLOAT fVbargraph38;
	double fRec38[2];
	FAUSTFLOAT fVbargraph39;
	double fRec41[2];
	FAUSTFLOAT fVbargraph40;
	double fRec40[2];
	FAUSTFLOAT fVbargraph41;
	double fRec43[2];
	FAUSTFLOAT fVbargraph42;
	double fRec42[2];
	FAUSTFLOAT fVbargraph43;
	double fRec45[2];
	FAUSTFLOAT fVbargraph44;
	double fRec44[2];
	FAUSTFLOAT fVbargraph45;
	double fRec47[2];
	FAUSTFLOAT fVbargraph46;
	double fRec46[2];
	FAUSTFLOAT fVbargraph47;
	double fRec49[2];
	FAUSTFLOAT fVbargraph48;
	double fRec48[2];
	FAUSTFLOAT fVbargraph49;
	double fRec51[2];
	FAUSTFLOAT fVbargraph50;
	double fRec50[2];
	FAUSTFLOAT fVbargraph51;
	double fRec53[2];
	FAUSTFLOAT fVbargraph52;
	double fRec52[2];
	FAUSTFLOAT fVbargraph53;
	double fRec55[2];
	FAUSTFLOAT fVbargraph54;
	double fRec54[2];
	FAUSTFLOAT fVbargraph55;
	double fRec57[2];
	FAUSTFLOAT fVbargraph56;
	double fRec56[2];
	FAUSTFLOAT fVbargraph57;
	double fRec59[2];
	FAUSTFLOAT fVbargraph58;
	double fRec58[2];
	FAUSTFLOAT fVbargraph59;
	double fRec61[2];
	FAUSTFLOAT fVbargraph60;
	double fRec60[2];
	FAUSTFLOAT fVbargraph61;
	double fRec63[2];
	FAUSTFLOAT fVbargraph62;
	double fRec62[2];
	FAUSTFLOAT fVbargraph63;
	double fRec65[2];
	FAUSTFLOAT fVbargraph64;
	double fRec64[2];
	FAUSTFLOAT fVbargraph65;
	double fRec67[2];
	FAUSTFLOAT fVbargraph66;
	double fRec66[2];
	FAUSTFLOAT fVbargraph67;
	double fRec69[2];
	FAUSTFLOAT fVbargraph68;
	double fRec68[2];
	FAUSTFLOAT fVbargraph69;
	double fRec71[2];
	FAUSTFLOAT fVbargraph70;
	double fRec70[2];
	FAUSTFLOAT fVbargraph71;
	double fRec73[2];
	FAUSTFLOAT fVbargraph72;
	double fRec72[2];
	FAUSTFLOAT fVbargraph73;
	double fRec75[2];
	FAUSTFLOAT fVbargraph74;
	double fRec74[2];
	FAUSTFLOAT fVbargraph75;
	double fRec77[2];
	FAUSTFLOAT fVbargraph76;
	double fRec76[2];
	FAUSTFLOAT fVbargraph77;
	double fRec79[2];
	FAUSTFLOAT fVbargraph78;
	double fRec78[2];
	FAUSTFLOAT fVbargraph79;
	double fRec81[2];
	FAUSTFLOAT fVbargraph80;
	double fRec80[2];
	FAUSTFLOAT fVbargraph81;
	double fRec83[2];
	FAUSTFLOAT fVbargraph82;
	double fRec82[2];
	FAUSTFLOAT fVbargraph83;
	double fRec85[2];
	FAUSTFLOAT fVbargraph84;
	double fRec84[2];
	FAUSTFLOAT fVbargraph85;
	double fRec87[2];
	FAUSTFLOAT fVbargraph86;
	double fRec86[2];
	FAUSTFLOAT fVbargraph87;
	double fRec89[2];
	FAUSTFLOAT fVbargraph88;
	double fRec88[2];
	FAUSTFLOAT fVbargraph89;
	double fRec91[2];
	FAUSTFLOAT fVbargraph90;
	double fRec90[2];
	FAUSTFLOAT fVbargraph91;
	double fRec93[2];
	FAUSTFLOAT fVbargraph92;
	double fRec92[2];
	FAUSTFLOAT fVbargraph93;
	double fRec95[2];
	FAUSTFLOAT fVbargraph94;
	double fRec94[2];
	FAUSTFLOAT fVbargraph95;
	double fRec97[2];
	FAUSTFLOAT fVbargraph96;
	double fRec96[2];
	FAUSTFLOAT fVbargraph97;
	double fRec99[2];
	FAUSTFLOAT fVbargraph98;
	double fRec98[2];
	FAUSTFLOAT fVbargraph99;
	double fRec101[2];
	FAUSTFLOAT fVbargraph100;
	double fRec100[2];
	FAUSTFLOAT fVbargraph101;
	double fRec103[2];
	FAUSTFLOAT fVbargraph102;
	double fRec102[2];
	FAUSTFLOAT fVbargraph103;
	double fRec105[2];
	FAUSTFLOAT fVbargraph104;
	double fRec104[2];
	FAUSTFLOAT fVbargraph105;
	double fRec107[2];
	FAUSTFLOAT fVbargraph106;
	double fRec106[2];
	FAUSTFLOAT fVbargraph107;
	double fRec109[2];
	FAUSTFLOAT fVbargraph108;
	double fRec108[2];
	FAUSTFLOAT fVbargraph109;
	double fRec111[2];
	FAUSTFLOAT fVbargraph110;
	double fRec110[2];
	FAUSTFLOAT fVbargraph111;
	double fRec113[2];
	FAUSTFLOAT fVbargraph112;
	double fRec112[2];
	FAUSTFLOAT fVbargraph113;
	double fRec115[2];
	FAUSTFLOAT fVbargraph114;
	double fRec114[2];
	FAUSTFLOAT fVbargraph115;
	double fRec117[2];
	FAUSTFLOAT fVbargraph116;
	double fRec116[2];
	FAUSTFLOAT fVbargraph117;
	double fRec119[2];
	FAUSTFLOAT fVbargraph118;
	double fRec118[2];
	FAUSTFLOAT fVbargraph119;
	double fRec121[2];
	FAUSTFLOAT fVbargraph120;
	double fRec120[2];
	FAUSTFLOAT fVbargraph121;
	double fRec123[2];
	FAUSTFLOAT fVbargraph122;
	double fRec122[2];
	FAUSTFLOAT fVbargraph123;
	double fRec125[2];
	FAUSTFLOAT fVbargraph124;
	double fRec124[2];
	FAUSTFLOAT fVbargraph125;
	double fRec127[2];
	FAUSTFLOAT fVbargraph126;
	double fRec126[2];
	FAUSTFLOAT fVbargraph127;
	double fRec129[2];
	FAUSTFLOAT fVbargraph128;
	double fRec128[2];
	FAUSTFLOAT fVbargraph129;
	double fRec131[2];
	FAUSTFLOAT fVbargraph130;
	double fRec130[2];
	FAUSTFLOAT fVbargraph131;
	double fRec133[2];
	FAUSTFLOAT fVbargraph132;
	double fRec132[2];
	FAUSTFLOAT fVbargraph133;
	double fRec135[2];
	FAUSTFLOAT fVbargraph134;
	double fRec134[2];
	FAUSTFLOAT fVbargraph135;
	double fRec137[2];
	FAUSTFLOAT fVbargraph136;
	double fRec136[2];
	FAUSTFLOAT fVbargraph137;
	double fRec139[2];
	FAUSTFLOAT fVbargraph138;
	double fRec138[2];
	FAUSTFLOAT fVbargraph139;
	double fRec141[2];
	FAUSTFLOAT fVbargraph140;
	double fRec140[2];
	FAUSTFLOAT fVbargraph141;
	double fRec143[2];
	FAUSTFLOAT fVbargraph142;
	double fRec142[2];
	FAUSTFLOAT fVbargraph143;
	double fRec145[2];
	FAUSTFLOAT fVbargraph144;
	double fRec144[2];
	FAUSTFLOAT fVbargraph145;
	double fRec147[2];
	FAUSTFLOAT fVbargraph146;
	double fRec146[2];
	FAUSTFLOAT fVbargraph147;
	double fRec149[2];
	FAUSTFLOAT fVbargraph148;
	double fRec148[2];
	FAUSTFLOAT fVbargraph149;
	double fRec151[2];
	FAUSTFLOAT fVbargraph150;
	double fRec150[2];
	FAUSTFLOAT fVbargraph151;
	double fRec153[2];
	FAUSTFLOAT fVbargraph152;
	double fRec152[2];
	FAUSTFLOAT fVbargraph153;
	double fRec155[2];
	FAUSTFLOAT fVbargraph154;
	double fRec154[2];
	FAUSTFLOAT fVbargraph155;
	double fRec157[2];
	FAUSTFLOAT fVbargraph156;
	double fRec156[2];
	FAUSTFLOAT fVbargraph157;
	double fRec159[2];
	FAUSTFLOAT fVbargraph158;
	double fRec158[2];
	FAUSTFLOAT fVbargraph159;
	double fRec161[2];
	FAUSTFLOAT fVbargraph160;
	double fRec160[2];
	FAUSTFLOAT fVbargraph161;
	double fRec163[2];
	FAUSTFLOAT fVbargraph162;
	double fRec162[2];
	FAUSTFLOAT fVbargraph163;
	double fRec165[2];
	FAUSTFLOAT fVbargraph164;
	double fRec164[2];
	FAUSTFLOAT fVbargraph165;
	double fRec167[2];
	FAUSTFLOAT fVbargraph166;
	double fRec166[2];
	FAUSTFLOAT fVbargraph167;
	double fRec169[2];
	FAUSTFLOAT fVbargraph168;
	double fRec168[2];
	FAUSTFLOAT fVbargraph169;
	double fRec171[2];
	FAUSTFLOAT fVbargraph170;
	double fRec170[2];
	FAUSTFLOAT fVbargraph171;
	double fRec173[2];
	FAUSTFLOAT fVbargraph172;
	double fRec172[2];
	FAUSTFLOAT fVbargraph173;
	double fRec175[2];
	FAUSTFLOAT fVbargraph174;
	double fRec174[2];
	FAUSTFLOAT fVbargraph175;
	double fRec177[2];
	FAUSTFLOAT fVbargraph176;
	double fRec176[2];
	FAUSTFLOAT fVbargraph177;
	double fRec179[2];
	FAUSTFLOAT fVbargraph178;
	double fRec178[2];
	FAUSTFLOAT fVbargraph179;
	double fRec181[2];
	FAUSTFLOAT fVbargraph180;
	double fRec180[2];
	FAUSTFLOAT fVbargraph181;
	double fRec183[2];
	FAUSTFLOAT fVbargraph182;
	double fRec182[2];
	FAUSTFLOAT fVbargraph183;
	double fRec185[2];
	FAUSTFLOAT fVbargraph184;
	double fRec184[2];
	FAUSTFLOAT fVbargraph185;
	double fRec187[2];
	FAUSTFLOAT fVbargraph186;
	double fRec186[2];
	FAUSTFLOAT fVbargraph187;
	double fRec189[2];
	FAUSTFLOAT fVbargraph188;
	double fRec188[2];
	FAUSTFLOAT fVbargraph189;
	double fRec191[2];
	FAUSTFLOAT fVbargraph190;
	double fRec190[2];
	FAUSTFLOAT fVbargraph191;
	double fRec193[2];
	FAUSTFLOAT fVbargraph192;
	double fRec192[2];
	FAUSTFLOAT fVbargraph193;
	double fRec195[2];
	FAUSTFLOAT fVbargraph194;
	double fRec194[2];
	FAUSTFLOAT fVbargraph195;
	double fRec197[2];
	FAUSTFLOAT fVbargraph196;
	double fRec196[2];
	FAUSTFLOAT fVbargraph197;
	double fRec199[2];
	FAUSTFLOAT fVbargraph198;
	double fRec198[2];
	FAUSTFLOAT fVbargraph199;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
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
		m->declare("name", "HOAConverterAcnN3d2AcnSn3d9");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 100;
		
	}
	virtual int getNumOutputs() {
		return 100;
		
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
			case 50: {
				rate = 1;
				break;
			}
			case 51: {
				rate = 1;
				break;
			}
			case 52: {
				rate = 1;
				break;
			}
			case 53: {
				rate = 1;
				break;
			}
			case 54: {
				rate = 1;
				break;
			}
			case 55: {
				rate = 1;
				break;
			}
			case 56: {
				rate = 1;
				break;
			}
			case 57: {
				rate = 1;
				break;
			}
			case 58: {
				rate = 1;
				break;
			}
			case 59: {
				rate = 1;
				break;
			}
			case 60: {
				rate = 1;
				break;
			}
			case 61: {
				rate = 1;
				break;
			}
			case 62: {
				rate = 1;
				break;
			}
			case 63: {
				rate = 1;
				break;
			}
			case 64: {
				rate = 1;
				break;
			}
			case 65: {
				rate = 1;
				break;
			}
			case 66: {
				rate = 1;
				break;
			}
			case 67: {
				rate = 1;
				break;
			}
			case 68: {
				rate = 1;
				break;
			}
			case 69: {
				rate = 1;
				break;
			}
			case 70: {
				rate = 1;
				break;
			}
			case 71: {
				rate = 1;
				break;
			}
			case 72: {
				rate = 1;
				break;
			}
			case 73: {
				rate = 1;
				break;
			}
			case 74: {
				rate = 1;
				break;
			}
			case 75: {
				rate = 1;
				break;
			}
			case 76: {
				rate = 1;
				break;
			}
			case 77: {
				rate = 1;
				break;
			}
			case 78: {
				rate = 1;
				break;
			}
			case 79: {
				rate = 1;
				break;
			}
			case 80: {
				rate = 1;
				break;
			}
			case 81: {
				rate = 1;
				break;
			}
			case 82: {
				rate = 1;
				break;
			}
			case 83: {
				rate = 1;
				break;
			}
			case 84: {
				rate = 1;
				break;
			}
			case 85: {
				rate = 1;
				break;
			}
			case 86: {
				rate = 1;
				break;
			}
			case 87: {
				rate = 1;
				break;
			}
			case 88: {
				rate = 1;
				break;
			}
			case 89: {
				rate = 1;
				break;
			}
			case 90: {
				rate = 1;
				break;
			}
			case 91: {
				rate = 1;
				break;
			}
			case 92: {
				rate = 1;
				break;
			}
			case 93: {
				rate = 1;
				break;
			}
			case 94: {
				rate = 1;
				break;
			}
			case 95: {
				rate = 1;
				break;
			}
			case 96: {
				rate = 1;
				break;
			}
			case 97: {
				rate = 1;
				break;
			}
			case 98: {
				rate = 1;
				break;
			}
			case 99: {
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
			case 50: {
				rate = 1;
				break;
			}
			case 51: {
				rate = 1;
				break;
			}
			case 52: {
				rate = 1;
				break;
			}
			case 53: {
				rate = 1;
				break;
			}
			case 54: {
				rate = 1;
				break;
			}
			case 55: {
				rate = 1;
				break;
			}
			case 56: {
				rate = 1;
				break;
			}
			case 57: {
				rate = 1;
				break;
			}
			case 58: {
				rate = 1;
				break;
			}
			case 59: {
				rate = 1;
				break;
			}
			case 60: {
				rate = 1;
				break;
			}
			case 61: {
				rate = 1;
				break;
			}
			case 62: {
				rate = 1;
				break;
			}
			case 63: {
				rate = 1;
				break;
			}
			case 64: {
				rate = 1;
				break;
			}
			case 65: {
				rate = 1;
				break;
			}
			case 66: {
				rate = 1;
				break;
			}
			case 67: {
				rate = 1;
				break;
			}
			case 68: {
				rate = 1;
				break;
			}
			case 69: {
				rate = 1;
				break;
			}
			case 70: {
				rate = 1;
				break;
			}
			case 71: {
				rate = 1;
				break;
			}
			case 72: {
				rate = 1;
				break;
			}
			case 73: {
				rate = 1;
				break;
			}
			case 74: {
				rate = 1;
				break;
			}
			case 75: {
				rate = 1;
				break;
			}
			case 76: {
				rate = 1;
				break;
			}
			case 77: {
				rate = 1;
				break;
			}
			case 78: {
				rate = 1;
				break;
			}
			case 79: {
				rate = 1;
				break;
			}
			case 80: {
				rate = 1;
				break;
			}
			case 81: {
				rate = 1;
				break;
			}
			case 82: {
				rate = 1;
				break;
			}
			case 83: {
				rate = 1;
				break;
			}
			case 84: {
				rate = 1;
				break;
			}
			case 85: {
				rate = 1;
				break;
			}
			case 86: {
				rate = 1;
				break;
			}
			case 87: {
				rate = 1;
				break;
			}
			case 88: {
				rate = 1;
				break;
			}
			case 89: {
				rate = 1;
				break;
			}
			case 90: {
				rate = 1;
				break;
			}
			case 91: {
				rate = 1;
				break;
			}
			case 92: {
				rate = 1;
				break;
			}
			case 93: {
				rate = 1;
				break;
			}
			case 94: {
				rate = 1;
				break;
			}
			case 95: {
				rate = 1;
				break;
			}
			case 96: {
				rate = 1;
				break;
			}
			case 97: {
				rate = 1;
				break;
			}
			case 98: {
				rate = 1;
				break;
			}
			case 99: {
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
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec1[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec0[l1] = 0.0;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec3[l2] = 0.0;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec2[l3] = 0.0;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec5[l4] = 0.0;
			
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec4[l5] = 0.0;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec7[l6] = 0.0;
			
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec6[l7] = 0.0;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec9[l8] = 0.0;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fRec8[l9] = 0.0;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec11[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec10[l11] = 0.0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec13[l12] = 0.0;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec12[l13] = 0.0;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec15[l14] = 0.0;
			
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec14[l15] = 0.0;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fRec17[l16] = 0.0;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec16[l17] = 0.0;
			
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec19[l18] = 0.0;
			
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec18[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec21[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec20[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec23[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec22[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec25[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec24[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec27[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec26[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec29[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec28[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec31[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec30[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec33[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec32[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec35[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec34[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec37[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec36[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec39[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec38[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec41[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec40[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec43[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec42[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec45[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec44[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec47[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec46[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec49[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec48[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec51[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec50[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec53[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec52[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec55[l54] = 0.0;
			
		}
		for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
			fRec54[l55] = 0.0;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fRec57[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec56[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec59[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec58[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec61[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec60[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fRec63[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec62[l63] = 0.0;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec65[l64] = 0.0;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec64[l65] = 0.0;
			
		}
		for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
			fRec67[l66] = 0.0;
			
		}
		for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
			fRec66[l67] = 0.0;
			
		}
		for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
			fRec69[l68] = 0.0;
			
		}
		for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
			fRec68[l69] = 0.0;
			
		}
		for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
			fRec71[l70] = 0.0;
			
		}
		for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
			fRec70[l71] = 0.0;
			
		}
		for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
			fRec73[l72] = 0.0;
			
		}
		for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
			fRec72[l73] = 0.0;
			
		}
		for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
			fRec75[l74] = 0.0;
			
		}
		for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
			fRec74[l75] = 0.0;
			
		}
		for (int l76 = 0; (l76 < 2); l76 = (l76 + 1)) {
			fRec77[l76] = 0.0;
			
		}
		for (int l77 = 0; (l77 < 2); l77 = (l77 + 1)) {
			fRec76[l77] = 0.0;
			
		}
		for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
			fRec79[l78] = 0.0;
			
		}
		for (int l79 = 0; (l79 < 2); l79 = (l79 + 1)) {
			fRec78[l79] = 0.0;
			
		}
		for (int l80 = 0; (l80 < 2); l80 = (l80 + 1)) {
			fRec81[l80] = 0.0;
			
		}
		for (int l81 = 0; (l81 < 2); l81 = (l81 + 1)) {
			fRec80[l81] = 0.0;
			
		}
		for (int l82 = 0; (l82 < 2); l82 = (l82 + 1)) {
			fRec83[l82] = 0.0;
			
		}
		for (int l83 = 0; (l83 < 2); l83 = (l83 + 1)) {
			fRec82[l83] = 0.0;
			
		}
		for (int l84 = 0; (l84 < 2); l84 = (l84 + 1)) {
			fRec85[l84] = 0.0;
			
		}
		for (int l85 = 0; (l85 < 2); l85 = (l85 + 1)) {
			fRec84[l85] = 0.0;
			
		}
		for (int l86 = 0; (l86 < 2); l86 = (l86 + 1)) {
			fRec87[l86] = 0.0;
			
		}
		for (int l87 = 0; (l87 < 2); l87 = (l87 + 1)) {
			fRec86[l87] = 0.0;
			
		}
		for (int l88 = 0; (l88 < 2); l88 = (l88 + 1)) {
			fRec89[l88] = 0.0;
			
		}
		for (int l89 = 0; (l89 < 2); l89 = (l89 + 1)) {
			fRec88[l89] = 0.0;
			
		}
		for (int l90 = 0; (l90 < 2); l90 = (l90 + 1)) {
			fRec91[l90] = 0.0;
			
		}
		for (int l91 = 0; (l91 < 2); l91 = (l91 + 1)) {
			fRec90[l91] = 0.0;
			
		}
		for (int l92 = 0; (l92 < 2); l92 = (l92 + 1)) {
			fRec93[l92] = 0.0;
			
		}
		for (int l93 = 0; (l93 < 2); l93 = (l93 + 1)) {
			fRec92[l93] = 0.0;
			
		}
		for (int l94 = 0; (l94 < 2); l94 = (l94 + 1)) {
			fRec95[l94] = 0.0;
			
		}
		for (int l95 = 0; (l95 < 2); l95 = (l95 + 1)) {
			fRec94[l95] = 0.0;
			
		}
		for (int l96 = 0; (l96 < 2); l96 = (l96 + 1)) {
			fRec97[l96] = 0.0;
			
		}
		for (int l97 = 0; (l97 < 2); l97 = (l97 + 1)) {
			fRec96[l97] = 0.0;
			
		}
		for (int l98 = 0; (l98 < 2); l98 = (l98 + 1)) {
			fRec99[l98] = 0.0;
			
		}
		for (int l99 = 0; (l99 < 2); l99 = (l99 + 1)) {
			fRec98[l99] = 0.0;
			
		}
		for (int l100 = 0; (l100 < 2); l100 = (l100 + 1)) {
			fRec101[l100] = 0.0;
			
		}
		for (int l101 = 0; (l101 < 2); l101 = (l101 + 1)) {
			fRec100[l101] = 0.0;
			
		}
		for (int l102 = 0; (l102 < 2); l102 = (l102 + 1)) {
			fRec103[l102] = 0.0;
			
		}
		for (int l103 = 0; (l103 < 2); l103 = (l103 + 1)) {
			fRec102[l103] = 0.0;
			
		}
		for (int l104 = 0; (l104 < 2); l104 = (l104 + 1)) {
			fRec105[l104] = 0.0;
			
		}
		for (int l105 = 0; (l105 < 2); l105 = (l105 + 1)) {
			fRec104[l105] = 0.0;
			
		}
		for (int l106 = 0; (l106 < 2); l106 = (l106 + 1)) {
			fRec107[l106] = 0.0;
			
		}
		for (int l107 = 0; (l107 < 2); l107 = (l107 + 1)) {
			fRec106[l107] = 0.0;
			
		}
		for (int l108 = 0; (l108 < 2); l108 = (l108 + 1)) {
			fRec109[l108] = 0.0;
			
		}
		for (int l109 = 0; (l109 < 2); l109 = (l109 + 1)) {
			fRec108[l109] = 0.0;
			
		}
		for (int l110 = 0; (l110 < 2); l110 = (l110 + 1)) {
			fRec111[l110] = 0.0;
			
		}
		for (int l111 = 0; (l111 < 2); l111 = (l111 + 1)) {
			fRec110[l111] = 0.0;
			
		}
		for (int l112 = 0; (l112 < 2); l112 = (l112 + 1)) {
			fRec113[l112] = 0.0;
			
		}
		for (int l113 = 0; (l113 < 2); l113 = (l113 + 1)) {
			fRec112[l113] = 0.0;
			
		}
		for (int l114 = 0; (l114 < 2); l114 = (l114 + 1)) {
			fRec115[l114] = 0.0;
			
		}
		for (int l115 = 0; (l115 < 2); l115 = (l115 + 1)) {
			fRec114[l115] = 0.0;
			
		}
		for (int l116 = 0; (l116 < 2); l116 = (l116 + 1)) {
			fRec117[l116] = 0.0;
			
		}
		for (int l117 = 0; (l117 < 2); l117 = (l117 + 1)) {
			fRec116[l117] = 0.0;
			
		}
		for (int l118 = 0; (l118 < 2); l118 = (l118 + 1)) {
			fRec119[l118] = 0.0;
			
		}
		for (int l119 = 0; (l119 < 2); l119 = (l119 + 1)) {
			fRec118[l119] = 0.0;
			
		}
		for (int l120 = 0; (l120 < 2); l120 = (l120 + 1)) {
			fRec121[l120] = 0.0;
			
		}
		for (int l121 = 0; (l121 < 2); l121 = (l121 + 1)) {
			fRec120[l121] = 0.0;
			
		}
		for (int l122 = 0; (l122 < 2); l122 = (l122 + 1)) {
			fRec123[l122] = 0.0;
			
		}
		for (int l123 = 0; (l123 < 2); l123 = (l123 + 1)) {
			fRec122[l123] = 0.0;
			
		}
		for (int l124 = 0; (l124 < 2); l124 = (l124 + 1)) {
			fRec125[l124] = 0.0;
			
		}
		for (int l125 = 0; (l125 < 2); l125 = (l125 + 1)) {
			fRec124[l125] = 0.0;
			
		}
		for (int l126 = 0; (l126 < 2); l126 = (l126 + 1)) {
			fRec127[l126] = 0.0;
			
		}
		for (int l127 = 0; (l127 < 2); l127 = (l127 + 1)) {
			fRec126[l127] = 0.0;
			
		}
		for (int l128 = 0; (l128 < 2); l128 = (l128 + 1)) {
			fRec129[l128] = 0.0;
			
		}
		for (int l129 = 0; (l129 < 2); l129 = (l129 + 1)) {
			fRec128[l129] = 0.0;
			
		}
		for (int l130 = 0; (l130 < 2); l130 = (l130 + 1)) {
			fRec131[l130] = 0.0;
			
		}
		for (int l131 = 0; (l131 < 2); l131 = (l131 + 1)) {
			fRec130[l131] = 0.0;
			
		}
		for (int l132 = 0; (l132 < 2); l132 = (l132 + 1)) {
			fRec133[l132] = 0.0;
			
		}
		for (int l133 = 0; (l133 < 2); l133 = (l133 + 1)) {
			fRec132[l133] = 0.0;
			
		}
		for (int l134 = 0; (l134 < 2); l134 = (l134 + 1)) {
			fRec135[l134] = 0.0;
			
		}
		for (int l135 = 0; (l135 < 2); l135 = (l135 + 1)) {
			fRec134[l135] = 0.0;
			
		}
		for (int l136 = 0; (l136 < 2); l136 = (l136 + 1)) {
			fRec137[l136] = 0.0;
			
		}
		for (int l137 = 0; (l137 < 2); l137 = (l137 + 1)) {
			fRec136[l137] = 0.0;
			
		}
		for (int l138 = 0; (l138 < 2); l138 = (l138 + 1)) {
			fRec139[l138] = 0.0;
			
		}
		for (int l139 = 0; (l139 < 2); l139 = (l139 + 1)) {
			fRec138[l139] = 0.0;
			
		}
		for (int l140 = 0; (l140 < 2); l140 = (l140 + 1)) {
			fRec141[l140] = 0.0;
			
		}
		for (int l141 = 0; (l141 < 2); l141 = (l141 + 1)) {
			fRec140[l141] = 0.0;
			
		}
		for (int l142 = 0; (l142 < 2); l142 = (l142 + 1)) {
			fRec143[l142] = 0.0;
			
		}
		for (int l143 = 0; (l143 < 2); l143 = (l143 + 1)) {
			fRec142[l143] = 0.0;
			
		}
		for (int l144 = 0; (l144 < 2); l144 = (l144 + 1)) {
			fRec145[l144] = 0.0;
			
		}
		for (int l145 = 0; (l145 < 2); l145 = (l145 + 1)) {
			fRec144[l145] = 0.0;
			
		}
		for (int l146 = 0; (l146 < 2); l146 = (l146 + 1)) {
			fRec147[l146] = 0.0;
			
		}
		for (int l147 = 0; (l147 < 2); l147 = (l147 + 1)) {
			fRec146[l147] = 0.0;
			
		}
		for (int l148 = 0; (l148 < 2); l148 = (l148 + 1)) {
			fRec149[l148] = 0.0;
			
		}
		for (int l149 = 0; (l149 < 2); l149 = (l149 + 1)) {
			fRec148[l149] = 0.0;
			
		}
		for (int l150 = 0; (l150 < 2); l150 = (l150 + 1)) {
			fRec151[l150] = 0.0;
			
		}
		for (int l151 = 0; (l151 < 2); l151 = (l151 + 1)) {
			fRec150[l151] = 0.0;
			
		}
		for (int l152 = 0; (l152 < 2); l152 = (l152 + 1)) {
			fRec153[l152] = 0.0;
			
		}
		for (int l153 = 0; (l153 < 2); l153 = (l153 + 1)) {
			fRec152[l153] = 0.0;
			
		}
		for (int l154 = 0; (l154 < 2); l154 = (l154 + 1)) {
			fRec155[l154] = 0.0;
			
		}
		for (int l155 = 0; (l155 < 2); l155 = (l155 + 1)) {
			fRec154[l155] = 0.0;
			
		}
		for (int l156 = 0; (l156 < 2); l156 = (l156 + 1)) {
			fRec157[l156] = 0.0;
			
		}
		for (int l157 = 0; (l157 < 2); l157 = (l157 + 1)) {
			fRec156[l157] = 0.0;
			
		}
		for (int l158 = 0; (l158 < 2); l158 = (l158 + 1)) {
			fRec159[l158] = 0.0;
			
		}
		for (int l159 = 0; (l159 < 2); l159 = (l159 + 1)) {
			fRec158[l159] = 0.0;
			
		}
		for (int l160 = 0; (l160 < 2); l160 = (l160 + 1)) {
			fRec161[l160] = 0.0;
			
		}
		for (int l161 = 0; (l161 < 2); l161 = (l161 + 1)) {
			fRec160[l161] = 0.0;
			
		}
		for (int l162 = 0; (l162 < 2); l162 = (l162 + 1)) {
			fRec163[l162] = 0.0;
			
		}
		for (int l163 = 0; (l163 < 2); l163 = (l163 + 1)) {
			fRec162[l163] = 0.0;
			
		}
		for (int l164 = 0; (l164 < 2); l164 = (l164 + 1)) {
			fRec165[l164] = 0.0;
			
		}
		for (int l165 = 0; (l165 < 2); l165 = (l165 + 1)) {
			fRec164[l165] = 0.0;
			
		}
		for (int l166 = 0; (l166 < 2); l166 = (l166 + 1)) {
			fRec167[l166] = 0.0;
			
		}
		for (int l167 = 0; (l167 < 2); l167 = (l167 + 1)) {
			fRec166[l167] = 0.0;
			
		}
		for (int l168 = 0; (l168 < 2); l168 = (l168 + 1)) {
			fRec169[l168] = 0.0;
			
		}
		for (int l169 = 0; (l169 < 2); l169 = (l169 + 1)) {
			fRec168[l169] = 0.0;
			
		}
		for (int l170 = 0; (l170 < 2); l170 = (l170 + 1)) {
			fRec171[l170] = 0.0;
			
		}
		for (int l171 = 0; (l171 < 2); l171 = (l171 + 1)) {
			fRec170[l171] = 0.0;
			
		}
		for (int l172 = 0; (l172 < 2); l172 = (l172 + 1)) {
			fRec173[l172] = 0.0;
			
		}
		for (int l173 = 0; (l173 < 2); l173 = (l173 + 1)) {
			fRec172[l173] = 0.0;
			
		}
		for (int l174 = 0; (l174 < 2); l174 = (l174 + 1)) {
			fRec175[l174] = 0.0;
			
		}
		for (int l175 = 0; (l175 < 2); l175 = (l175 + 1)) {
			fRec174[l175] = 0.0;
			
		}
		for (int l176 = 0; (l176 < 2); l176 = (l176 + 1)) {
			fRec177[l176] = 0.0;
			
		}
		for (int l177 = 0; (l177 < 2); l177 = (l177 + 1)) {
			fRec176[l177] = 0.0;
			
		}
		for (int l178 = 0; (l178 < 2); l178 = (l178 + 1)) {
			fRec179[l178] = 0.0;
			
		}
		for (int l179 = 0; (l179 < 2); l179 = (l179 + 1)) {
			fRec178[l179] = 0.0;
			
		}
		for (int l180 = 0; (l180 < 2); l180 = (l180 + 1)) {
			fRec181[l180] = 0.0;
			
		}
		for (int l181 = 0; (l181 < 2); l181 = (l181 + 1)) {
			fRec180[l181] = 0.0;
			
		}
		for (int l182 = 0; (l182 < 2); l182 = (l182 + 1)) {
			fRec183[l182] = 0.0;
			
		}
		for (int l183 = 0; (l183 < 2); l183 = (l183 + 1)) {
			fRec182[l183] = 0.0;
			
		}
		for (int l184 = 0; (l184 < 2); l184 = (l184 + 1)) {
			fRec185[l184] = 0.0;
			
		}
		for (int l185 = 0; (l185 < 2); l185 = (l185 + 1)) {
			fRec184[l185] = 0.0;
			
		}
		for (int l186 = 0; (l186 < 2); l186 = (l186 + 1)) {
			fRec187[l186] = 0.0;
			
		}
		for (int l187 = 0; (l187 < 2); l187 = (l187 + 1)) {
			fRec186[l187] = 0.0;
			
		}
		for (int l188 = 0; (l188 < 2); l188 = (l188 + 1)) {
			fRec189[l188] = 0.0;
			
		}
		for (int l189 = 0; (l189 < 2); l189 = (l189 + 1)) {
			fRec188[l189] = 0.0;
			
		}
		for (int l190 = 0; (l190 < 2); l190 = (l190 + 1)) {
			fRec191[l190] = 0.0;
			
		}
		for (int l191 = 0; (l191 < 2); l191 = (l191 + 1)) {
			fRec190[l191] = 0.0;
			
		}
		for (int l192 = 0; (l192 < 2); l192 = (l192 + 1)) {
			fRec193[l192] = 0.0;
			
		}
		for (int l193 = 0; (l193 < 2); l193 = (l193 + 1)) {
			fRec192[l193] = 0.0;
			
		}
		for (int l194 = 0; (l194 < 2); l194 = (l194 + 1)) {
			fRec195[l194] = 0.0;
			
		}
		for (int l195 = 0; (l195 < 2); l195 = (l195 + 1)) {
			fRec194[l195] = 0.0;
			
		}
		for (int l196 = 0; (l196 < 2); l196 = (l196 + 1)) {
			fRec197[l196] = 0.0;
			
		}
		for (int l197 = 0; (l197 < 2); l197 = (l197 + 1)) {
			fRec196[l197] = 0.0;
			
		}
		for (int l198 = 0; (l198 < 2); l198 = (l198 + 1)) {
			fRec199[l198] = 0.0;
			
		}
		for (int l199 = 0; (l199 < 2); l199 = (l199 + 1)) {
			fRec198[l199] = 0.0;
			
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
		ui_interface->openVerticalBox("HOAConverterAcnN3d2AcnSn3d9");
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("ACN N3D");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f37000", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f40cf0", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f4ac00", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f54bf0", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f5ef70", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f69040", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f731f0", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f7d560", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f87a90", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f92350", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f9c5e0", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fa6950", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fb0e80", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fbb570", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fc5e20", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fd0890", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fdb690", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fe5af0", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ff0020", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ffa710", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3004fc0", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x300fa30", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x301a660", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3025450", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3030400", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5");
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x303b740", &fVbargraph50, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3045d50", &fVbargraph52, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3050440", &fVbargraph54, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x305acf0", &fVbargraph56, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3065760", &fVbargraph58, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3070390", &fVbargraph60, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x307b180", &fVbargraph62, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3086130", &fVbargraph64, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph66, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30912a0", &fVbargraph66, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph68, "unit", "dB");
		ui_interface->addVerticalBargraph("0x309c5d0", &fVbargraph68, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph70, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30a7ac0", &fVbargraph70, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("6");
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph72, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30b3340", &fVbargraph72, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph74, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30bdb10", &fVbargraph74, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph76, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30c83c0", &fVbargraph76, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph78, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30d2e30", &fVbargraph78, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph80, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30dda60", &fVbargraph80, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph82, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30e8850", &fVbargraph82, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph84, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30f3800", &fVbargraph84, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph86, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30fe970", &fVbargraph86, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph88, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3109ca0", &fVbargraph88, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph90, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3115190", &fVbargraph90, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph92, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3120840", &fVbargraph92, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph94, "unit", "dB");
		ui_interface->addVerticalBargraph("0x312c0b0", &fVbargraph94, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph96, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3137ae0", &fVbargraph96, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("7");
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph98, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31438a0", &fVbargraph98, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph100, "unit", "dB");
		ui_interface->addVerticalBargraph("0x314e230", &fVbargraph100, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("51");
		ui_interface->declare(&fVbargraph102, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3158ca0", &fVbargraph102, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("52");
		ui_interface->declare(&fVbargraph104, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31638d0", &fVbargraph104, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("53");
		ui_interface->declare(&fVbargraph106, "unit", "dB");
		ui_interface->addVerticalBargraph("0x316e6c0", &fVbargraph106, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("54");
		ui_interface->declare(&fVbargraph108, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3179670", &fVbargraph108, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("55");
		ui_interface->declare(&fVbargraph110, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31847e0", &fVbargraph110, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("56");
		ui_interface->declare(&fVbargraph112, "unit", "dB");
		ui_interface->addVerticalBargraph("0x318fb10", &fVbargraph112, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("57");
		ui_interface->declare(&fVbargraph114, "unit", "dB");
		ui_interface->addVerticalBargraph("0x319b000", &fVbargraph114, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("58");
		ui_interface->declare(&fVbargraph116, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31a66b0", &fVbargraph116, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("59");
		ui_interface->declare(&fVbargraph118, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31b1f30", &fVbargraph118, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("60");
		ui_interface->declare(&fVbargraph120, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31bd960", &fVbargraph120, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("61");
		ui_interface->declare(&fVbargraph122, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31c9550", &fVbargraph122, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("62");
		ui_interface->declare(&fVbargraph124, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31d5300", &fVbargraph124, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("63");
		ui_interface->declare(&fVbargraph126, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31e1270", &fVbargraph126, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("8");
		ui_interface->openVerticalBox("64");
		ui_interface->declare(&fVbargraph128, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31ed570", &fVbargraph128, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("65");
		ui_interface->declare(&fVbargraph130, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31f80c0", &fVbargraph130, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("66");
		ui_interface->declare(&fVbargraph132, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3202cf0", &fVbargraph132, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("67");
		ui_interface->declare(&fVbargraph134, "unit", "dB");
		ui_interface->addVerticalBargraph("0x320dae0", &fVbargraph134, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("68");
		ui_interface->declare(&fVbargraph136, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3218a90", &fVbargraph136, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("69");
		ui_interface->declare(&fVbargraph138, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3223c00", &fVbargraph138, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("70");
		ui_interface->declare(&fVbargraph140, "unit", "dB");
		ui_interface->addVerticalBargraph("0x322ef40", &fVbargraph140, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("71");
		ui_interface->declare(&fVbargraph142, "unit", "dB");
		ui_interface->addVerticalBargraph("0x323a430", &fVbargraph142, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("72");
		ui_interface->declare(&fVbargraph144, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3245ae0", &fVbargraph144, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("73");
		ui_interface->declare(&fVbargraph146, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3251350", &fVbargraph146, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("74");
		ui_interface->declare(&fVbargraph148, "unit", "dB");
		ui_interface->addVerticalBargraph("0x325cd80", &fVbargraph148, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("75");
		ui_interface->declare(&fVbargraph150, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3268970", &fVbargraph150, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("76");
		ui_interface->declare(&fVbargraph152, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3274720", &fVbargraph152, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("77");
		ui_interface->declare(&fVbargraph154, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3280690", &fVbargraph154, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("78");
		ui_interface->declare(&fVbargraph156, "unit", "dB");
		ui_interface->addVerticalBargraph("0x328c7c0", &fVbargraph156, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("79");
		ui_interface->declare(&fVbargraph158, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3298ac0", &fVbargraph158, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("80");
		ui_interface->declare(&fVbargraph160, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32a4f70", &fVbargraph160, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("9");
		ui_interface->openVerticalBox("81");
		ui_interface->declare(&fVbargraph162, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32b17b0", &fVbargraph162, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("82");
		ui_interface->declare(&fVbargraph164, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32bc4c0", &fVbargraph164, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("83");
		ui_interface->declare(&fVbargraph166, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32c72b0", &fVbargraph166, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("84");
		ui_interface->declare(&fVbargraph168, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32d2260", &fVbargraph168, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("85");
		ui_interface->declare(&fVbargraph170, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32dd3d0", &fVbargraph170, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("86");
		ui_interface->declare(&fVbargraph172, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32e8700", &fVbargraph172, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("87");
		ui_interface->declare(&fVbargraph174, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32f3bf0", &fVbargraph174, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("88");
		ui_interface->declare(&fVbargraph176, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32ff2a0", &fVbargraph176, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("89");
		ui_interface->declare(&fVbargraph178, "unit", "dB");
		ui_interface->addVerticalBargraph("0x330ab10", &fVbargraph178, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("90");
		ui_interface->declare(&fVbargraph180, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3316540", &fVbargraph180, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("91");
		ui_interface->declare(&fVbargraph182, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3322130", &fVbargraph182, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("92");
		ui_interface->declare(&fVbargraph184, "unit", "dB");
		ui_interface->addVerticalBargraph("0x332dee0", &fVbargraph184, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("93");
		ui_interface->declare(&fVbargraph186, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3339e50", &fVbargraph186, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("94");
		ui_interface->declare(&fVbargraph188, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3345f80", &fVbargraph188, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("95");
		ui_interface->declare(&fVbargraph190, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3352270", &fVbargraph190, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("96");
		ui_interface->declare(&fVbargraph192, "unit", "dB");
		ui_interface->addVerticalBargraph("0x335e720", &fVbargraph192, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("97");
		ui_interface->declare(&fVbargraph194, "unit", "dB");
		ui_interface->addVerticalBargraph("0x336ad90", &fVbargraph194, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("98");
		ui_interface->declare(&fVbargraph196, "unit", "dB");
		ui_interface->addVerticalBargraph("0x33775c0", &fVbargraph196, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("99");
		ui_interface->declare(&fVbargraph198, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3383fb0", &fVbargraph198, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openHorizontalBox("ACN SN3D");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f3b180", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f45260", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f4f170", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f59240", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f635c0", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f6d690", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f77920", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f81d70", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f8c380", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2f96a80", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fa0d10", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fab160", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fb5770", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fbff40", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fca8d0", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fd5420", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fdfeb0", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fea300", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ff4910", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2fff0e0", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3009a70", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30145c0", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x301f2d0", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x302a1a0", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3035230", &fVbargraph49, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5");
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3040030", &fVbargraph51, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x304a640", &fVbargraph53, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3054e10", &fVbargraph55, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x305f7a0", &fVbargraph57, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x306a2f0", &fVbargraph59, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3075000", &fVbargraph61, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x307fed0", &fVbargraph63, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x308af60", &fVbargraph65, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph67, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30961b0", &fVbargraph67, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph69, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30a15c0", &fVbargraph69, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph71, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30acb90", &fVbargraph71, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("6");
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph73, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30b7d10", &fVbargraph73, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph75, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30c24e0", &fVbargraph75, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph77, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30cce70", &fVbargraph77, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph79, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30d79c0", &fVbargraph79, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph81, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30e26d0", &fVbargraph81, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph83, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30ed5a0", &fVbargraph83, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph85, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30f8630", &fVbargraph85, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph87, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3103880", &fVbargraph87, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph89, "unit", "dB");
		ui_interface->addVerticalBargraph("0x310ec90", &fVbargraph89, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph91, "unit", "dB");
		ui_interface->addVerticalBargraph("0x311a260", &fVbargraph91, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph93, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31259f0", &fVbargraph93, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph95, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3131340", &fVbargraph95, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph97, "unit", "dB");
		ui_interface->addVerticalBargraph("0x313ce50", &fVbargraph97, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("7");
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph99, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3148350", &fVbargraph99, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph101, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3152ce0", &fVbargraph101, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("51");
		ui_interface->declare(&fVbargraph103, "unit", "dB");
		ui_interface->addVerticalBargraph("0x315d830", &fVbargraph103, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("52");
		ui_interface->declare(&fVbargraph105, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3168540", &fVbargraph105, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("53");
		ui_interface->declare(&fVbargraph107, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3173410", &fVbargraph107, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("54");
		ui_interface->declare(&fVbargraph109, "unit", "dB");
		ui_interface->addVerticalBargraph("0x317e4a0", &fVbargraph109, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("55");
		ui_interface->declare(&fVbargraph111, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31896f0", &fVbargraph111, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("56");
		ui_interface->declare(&fVbargraph113, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3194b00", &fVbargraph113, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("57");
		ui_interface->declare(&fVbargraph115, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31a00d0", &fVbargraph115, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("58");
		ui_interface->declare(&fVbargraph117, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31ab870", &fVbargraph117, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("59");
		ui_interface->declare(&fVbargraph119, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31b71c0", &fVbargraph119, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("60");
		ui_interface->declare(&fVbargraph121, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31c2cd0", &fVbargraph121, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("61");
		ui_interface->declare(&fVbargraph123, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31ce9a0", &fVbargraph123, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("62");
		ui_interface->declare(&fVbargraph125, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31da830", &fVbargraph125, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("63");
		ui_interface->declare(&fVbargraph127, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31e6880", &fVbargraph127, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("8");
		ui_interface->openVerticalBox("64");
		ui_interface->declare(&fVbargraph129, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31f2100", &fVbargraph129, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("65");
		ui_interface->declare(&fVbargraph131, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31fcc50", &fVbargraph131, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("66");
		ui_interface->declare(&fVbargraph133, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3207960", &fVbargraph133, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("67");
		ui_interface->declare(&fVbargraph135, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3212830", &fVbargraph135, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("68");
		ui_interface->declare(&fVbargraph137, "unit", "dB");
		ui_interface->addVerticalBargraph("0x321d8c0", &fVbargraph137, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("69");
		ui_interface->declare(&fVbargraph139, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3228b10", &fVbargraph139, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("70");
		ui_interface->declare(&fVbargraph141, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3233f30", &fVbargraph141, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("71");
		ui_interface->declare(&fVbargraph143, "unit", "dB");
		ui_interface->addVerticalBargraph("0x323f500", &fVbargraph143, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("72");
		ui_interface->declare(&fVbargraph145, "unit", "dB");
		ui_interface->addVerticalBargraph("0x324ac90", &fVbargraph145, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("73");
		ui_interface->declare(&fVbargraph147, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32565e0", &fVbargraph147, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("74");
		ui_interface->declare(&fVbargraph149, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32620f0", &fVbargraph149, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("75");
		ui_interface->declare(&fVbargraph151, "unit", "dB");
		ui_interface->addVerticalBargraph("0x326ddc0", &fVbargraph151, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("76");
		ui_interface->declare(&fVbargraph153, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3279c50", &fVbargraph153, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("77");
		ui_interface->declare(&fVbargraph155, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3285ca0", &fVbargraph155, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("78");
		ui_interface->declare(&fVbargraph157, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3291ec0", &fVbargraph157, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("79");
		ui_interface->declare(&fVbargraph159, "unit", "dB");
		ui_interface->addVerticalBargraph("0x329e290", &fVbargraph159, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("80");
		ui_interface->declare(&fVbargraph161, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32aa820", &fVbargraph161, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("9");
		ui_interface->openVerticalBox("81");
		ui_interface->declare(&fVbargraph163, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32b6420", &fVbargraph163, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("82");
		ui_interface->declare(&fVbargraph165, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32c1130", &fVbargraph165, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("83");
		ui_interface->declare(&fVbargraph167, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32cc000", &fVbargraph167, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("84");
		ui_interface->declare(&fVbargraph169, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32d7090", &fVbargraph169, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("85");
		ui_interface->declare(&fVbargraph171, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32e22e0", &fVbargraph171, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("86");
		ui_interface->declare(&fVbargraph173, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32ed6f0", &fVbargraph173, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("87");
		ui_interface->declare(&fVbargraph175, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32f8cc0", &fVbargraph175, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("88");
		ui_interface->declare(&fVbargraph177, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3304450", &fVbargraph177, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("89");
		ui_interface->declare(&fVbargraph179, "unit", "dB");
		ui_interface->addVerticalBargraph("0x330fda0", &fVbargraph179, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("90");
		ui_interface->declare(&fVbargraph181, "unit", "dB");
		ui_interface->addVerticalBargraph("0x331b8b0", &fVbargraph181, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("91");
		ui_interface->declare(&fVbargraph183, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3327580", &fVbargraph183, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("92");
		ui_interface->declare(&fVbargraph185, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3333410", &fVbargraph185, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("93");
		ui_interface->declare(&fVbargraph187, "unit", "dB");
		ui_interface->addVerticalBargraph("0x333f460", &fVbargraph187, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("94");
		ui_interface->declare(&fVbargraph189, "unit", "dB");
		ui_interface->addVerticalBargraph("0x334b670", &fVbargraph189, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("95");
		ui_interface->declare(&fVbargraph191, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3357a40", &fVbargraph191, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("96");
		ui_interface->declare(&fVbargraph193, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3363fd0", &fVbargraph193, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("97");
		ui_interface->declare(&fVbargraph195, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3370720", &fVbargraph195, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("98");
		ui_interface->declare(&fVbargraph197, "unit", "dB");
		ui_interface->addVerticalBargraph("0x337d030", &fVbargraph197, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("99");
		ui_interface->declare(&fVbargraph199, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3389b00", &fVbargraph199, -70.0, 6.0);
		ui_interface->closeBox();
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
		FAUSTFLOAT* input50 = inputs[50];
		FAUSTFLOAT* input51 = inputs[51];
		FAUSTFLOAT* input52 = inputs[52];
		FAUSTFLOAT* input53 = inputs[53];
		FAUSTFLOAT* input54 = inputs[54];
		FAUSTFLOAT* input55 = inputs[55];
		FAUSTFLOAT* input56 = inputs[56];
		FAUSTFLOAT* input57 = inputs[57];
		FAUSTFLOAT* input58 = inputs[58];
		FAUSTFLOAT* input59 = inputs[59];
		FAUSTFLOAT* input60 = inputs[60];
		FAUSTFLOAT* input61 = inputs[61];
		FAUSTFLOAT* input62 = inputs[62];
		FAUSTFLOAT* input63 = inputs[63];
		FAUSTFLOAT* input64 = inputs[64];
		FAUSTFLOAT* input65 = inputs[65];
		FAUSTFLOAT* input66 = inputs[66];
		FAUSTFLOAT* input67 = inputs[67];
		FAUSTFLOAT* input68 = inputs[68];
		FAUSTFLOAT* input69 = inputs[69];
		FAUSTFLOAT* input70 = inputs[70];
		FAUSTFLOAT* input71 = inputs[71];
		FAUSTFLOAT* input72 = inputs[72];
		FAUSTFLOAT* input73 = inputs[73];
		FAUSTFLOAT* input74 = inputs[74];
		FAUSTFLOAT* input75 = inputs[75];
		FAUSTFLOAT* input76 = inputs[76];
		FAUSTFLOAT* input77 = inputs[77];
		FAUSTFLOAT* input78 = inputs[78];
		FAUSTFLOAT* input79 = inputs[79];
		FAUSTFLOAT* input80 = inputs[80];
		FAUSTFLOAT* input81 = inputs[81];
		FAUSTFLOAT* input82 = inputs[82];
		FAUSTFLOAT* input83 = inputs[83];
		FAUSTFLOAT* input84 = inputs[84];
		FAUSTFLOAT* input85 = inputs[85];
		FAUSTFLOAT* input86 = inputs[86];
		FAUSTFLOAT* input87 = inputs[87];
		FAUSTFLOAT* input88 = inputs[88];
		FAUSTFLOAT* input89 = inputs[89];
		FAUSTFLOAT* input90 = inputs[90];
		FAUSTFLOAT* input91 = inputs[91];
		FAUSTFLOAT* input92 = inputs[92];
		FAUSTFLOAT* input93 = inputs[93];
		FAUSTFLOAT* input94 = inputs[94];
		FAUSTFLOAT* input95 = inputs[95];
		FAUSTFLOAT* input96 = inputs[96];
		FAUSTFLOAT* input97 = inputs[97];
		FAUSTFLOAT* input98 = inputs[98];
		FAUSTFLOAT* input99 = inputs[99];
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
		FAUSTFLOAT* output50 = outputs[50];
		FAUSTFLOAT* output51 = outputs[51];
		FAUSTFLOAT* output52 = outputs[52];
		FAUSTFLOAT* output53 = outputs[53];
		FAUSTFLOAT* output54 = outputs[54];
		FAUSTFLOAT* output55 = outputs[55];
		FAUSTFLOAT* output56 = outputs[56];
		FAUSTFLOAT* output57 = outputs[57];
		FAUSTFLOAT* output58 = outputs[58];
		FAUSTFLOAT* output59 = outputs[59];
		FAUSTFLOAT* output60 = outputs[60];
		FAUSTFLOAT* output61 = outputs[61];
		FAUSTFLOAT* output62 = outputs[62];
		FAUSTFLOAT* output63 = outputs[63];
		FAUSTFLOAT* output64 = outputs[64];
		FAUSTFLOAT* output65 = outputs[65];
		FAUSTFLOAT* output66 = outputs[66];
		FAUSTFLOAT* output67 = outputs[67];
		FAUSTFLOAT* output68 = outputs[68];
		FAUSTFLOAT* output69 = outputs[69];
		FAUSTFLOAT* output70 = outputs[70];
		FAUSTFLOAT* output71 = outputs[71];
		FAUSTFLOAT* output72 = outputs[72];
		FAUSTFLOAT* output73 = outputs[73];
		FAUSTFLOAT* output74 = outputs[74];
		FAUSTFLOAT* output75 = outputs[75];
		FAUSTFLOAT* output76 = outputs[76];
		FAUSTFLOAT* output77 = outputs[77];
		FAUSTFLOAT* output78 = outputs[78];
		FAUSTFLOAT* output79 = outputs[79];
		FAUSTFLOAT* output80 = outputs[80];
		FAUSTFLOAT* output81 = outputs[81];
		FAUSTFLOAT* output82 = outputs[82];
		FAUSTFLOAT* output83 = outputs[83];
		FAUSTFLOAT* output84 = outputs[84];
		FAUSTFLOAT* output85 = outputs[85];
		FAUSTFLOAT* output86 = outputs[86];
		FAUSTFLOAT* output87 = outputs[87];
		FAUSTFLOAT* output88 = outputs[88];
		FAUSTFLOAT* output89 = outputs[89];
		FAUSTFLOAT* output90 = outputs[90];
		FAUSTFLOAT* output91 = outputs[91];
		FAUSTFLOAT* output92 = outputs[92];
		FAUSTFLOAT* output93 = outputs[93];
		FAUSTFLOAT* output94 = outputs[94];
		FAUSTFLOAT* output95 = outputs[95];
		FAUSTFLOAT* output96 = outputs[96];
		FAUSTFLOAT* output97 = outputs[97];
		FAUSTFLOAT* output98 = outputs[98];
		FAUSTFLOAT* output99 = outputs[99];
		for (int i = 0; (i < count); i = (i + 1)) {
			double fTemp0 = double(input0[i]);
			fRec1[0] = max((fRec1[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fVbargraph0 = FAUSTFLOAT(fRec1[0]);
			fRec0[0] = max((fRec0[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fVbargraph1 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp0);
			double fTemp1 = double(input1[i]);
			fRec3[0] = max((fRec3[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp1))))));
			fVbargraph2 = FAUSTFLOAT(fRec3[0]);
			double fTemp2 = (0.57735026918962584 * fTemp1);
			fRec2[0] = max((fRec2[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp2))))));
			fVbargraph3 = FAUSTFLOAT(fRec2[0]);
			output1[i] = FAUSTFLOAT(fTemp2);
			double fTemp3 = double(input2[i]);
			fRec5[0] = max((fRec5[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp3))))));
			fVbargraph4 = FAUSTFLOAT(fRec5[0]);
			double fTemp4 = (0.57735026918962584 * fTemp3);
			fRec4[0] = max((fRec4[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fVbargraph5 = FAUSTFLOAT(fRec4[0]);
			output2[i] = FAUSTFLOAT(fTemp4);
			double fTemp5 = double(input3[i]);
			fRec7[0] = max((fRec7[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fVbargraph6 = FAUSTFLOAT(fRec7[0]);
			double fTemp6 = (0.57735026918962584 * fTemp5);
			fRec6[0] = max((fRec6[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fVbargraph7 = FAUSTFLOAT(fRec6[0]);
			output3[i] = FAUSTFLOAT(fTemp6);
			double fTemp7 = double(input4[i]);
			fRec9[0] = max((fRec9[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp7))))));
			fVbargraph8 = FAUSTFLOAT(fRec9[0]);
			double fTemp8 = (0.44721359549995793 * fTemp7);
			fRec8[0] = max((fRec8[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fVbargraph9 = FAUSTFLOAT(fRec8[0]);
			output4[i] = FAUSTFLOAT(fTemp8);
			double fTemp9 = double(input5[i]);
			fRec11[0] = max((fRec11[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph10 = FAUSTFLOAT(fRec11[0]);
			double fTemp10 = (0.44721359549995793 * fTemp9);
			fRec10[0] = max((fRec10[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fVbargraph11 = FAUSTFLOAT(fRec10[0]);
			output5[i] = FAUSTFLOAT(fTemp10);
			double fTemp11 = double(input6[i]);
			fRec13[0] = max((fRec13[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fVbargraph12 = FAUSTFLOAT(fRec13[0]);
			double fTemp12 = (0.44721359549995793 * fTemp11);
			fRec12[0] = max((fRec12[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph13 = FAUSTFLOAT(fRec12[0]);
			output6[i] = FAUSTFLOAT(fTemp12);
			double fTemp13 = double(input7[i]);
			fRec15[0] = max((fRec15[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fVbargraph14 = FAUSTFLOAT(fRec15[0]);
			double fTemp14 = (0.44721359549995793 * fTemp13);
			fRec14[0] = max((fRec14[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fVbargraph15 = FAUSTFLOAT(fRec14[0]);
			output7[i] = FAUSTFLOAT(fTemp14);
			double fTemp15 = double(input8[i]);
			fRec17[0] = max((fRec17[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph16 = FAUSTFLOAT(fRec17[0]);
			double fTemp16 = (0.44721359549995793 * fTemp15);
			fRec16[0] = max((fRec16[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph17 = FAUSTFLOAT(fRec16[0]);
			output8[i] = FAUSTFLOAT(fTemp16);
			double fTemp17 = double(input9[i]);
			fRec19[0] = max((fRec19[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph18 = FAUSTFLOAT(fRec19[0]);
			double fTemp18 = (0.3779644730092272 * fTemp17);
			fRec18[0] = max((fRec18[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph19 = FAUSTFLOAT(fRec18[0]);
			output9[i] = FAUSTFLOAT(fTemp18);
			double fTemp19 = double(input10[i]);
			fRec21[0] = max((fRec21[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph20 = FAUSTFLOAT(fRec21[0]);
			double fTemp20 = (0.3779644730092272 * fTemp19);
			fRec20[0] = max((fRec20[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph21 = FAUSTFLOAT(fRec20[0]);
			output10[i] = FAUSTFLOAT(fTemp20);
			double fTemp21 = double(input11[i]);
			fRec23[0] = max((fRec23[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph22 = FAUSTFLOAT(fRec23[0]);
			double fTemp22 = (0.3779644730092272 * fTemp21);
			fRec22[0] = max((fRec22[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph23 = FAUSTFLOAT(fRec22[0]);
			output11[i] = FAUSTFLOAT(fTemp22);
			double fTemp23 = double(input12[i]);
			fRec25[0] = max((fRec25[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fVbargraph24 = FAUSTFLOAT(fRec25[0]);
			double fTemp24 = (0.3779644730092272 * fTemp23);
			fRec24[0] = max((fRec24[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph25 = FAUSTFLOAT(fRec24[0]);
			output12[i] = FAUSTFLOAT(fTemp24);
			double fTemp25 = double(input13[i]);
			fRec27[0] = max((fRec27[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph26 = FAUSTFLOAT(fRec27[0]);
			double fTemp26 = (0.3779644730092272 * fTemp25);
			fRec26[0] = max((fRec26[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fVbargraph27 = FAUSTFLOAT(fRec26[0]);
			output13[i] = FAUSTFLOAT(fTemp26);
			double fTemp27 = double(input14[i]);
			fRec29[0] = max((fRec29[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph28 = FAUSTFLOAT(fRec29[0]);
			double fTemp28 = (0.3779644730092272 * fTemp27);
			fRec28[0] = max((fRec28[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fVbargraph29 = FAUSTFLOAT(fRec28[0]);
			output14[i] = FAUSTFLOAT(fTemp28);
			double fTemp29 = double(input15[i]);
			fRec31[0] = max((fRec31[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph30 = FAUSTFLOAT(fRec31[0]);
			double fTemp30 = (0.3779644730092272 * fTemp29);
			fRec30[0] = max((fRec30[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph31 = FAUSTFLOAT(fRec30[0]);
			output15[i] = FAUSTFLOAT(fTemp30);
			double fTemp31 = double(input16[i]);
			fRec33[0] = max((fRec33[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph32 = FAUSTFLOAT(fRec33[0]);
			double fTemp32 = (0.33333333333333331 * fTemp31);
			fRec32[0] = max((fRec32[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph33 = FAUSTFLOAT(fRec32[0]);
			output16[i] = FAUSTFLOAT(fTemp32);
			double fTemp33 = double(input17[i]);
			fRec35[0] = max((fRec35[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph34 = FAUSTFLOAT(fRec35[0]);
			double fTemp34 = (0.33333333333333331 * fTemp33);
			fRec34[0] = max((fRec34[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fVbargraph35 = FAUSTFLOAT(fRec34[0]);
			output17[i] = FAUSTFLOAT(fTemp34);
			double fTemp35 = double(input18[i]);
			fRec37[0] = max((fRec37[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph36 = FAUSTFLOAT(fRec37[0]);
			double fTemp36 = (0.33333333333333331 * fTemp35);
			fRec36[0] = max((fRec36[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fVbargraph37 = FAUSTFLOAT(fRec36[0]);
			output18[i] = FAUSTFLOAT(fTemp36);
			double fTemp37 = double(input19[i]);
			fRec39[0] = max((fRec39[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fVbargraph38 = FAUSTFLOAT(fRec39[0]);
			double fTemp38 = (0.33333333333333331 * fTemp37);
			fRec38[0] = max((fRec38[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fVbargraph39 = FAUSTFLOAT(fRec38[0]);
			output19[i] = FAUSTFLOAT(fTemp38);
			double fTemp39 = double(input20[i]);
			fRec41[0] = max((fRec41[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph40 = FAUSTFLOAT(fRec41[0]);
			double fTemp40 = (0.33333333333333331 * fTemp39);
			fRec40[0] = max((fRec40[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fVbargraph41 = FAUSTFLOAT(fRec40[0]);
			output20[i] = FAUSTFLOAT(fTemp40);
			double fTemp41 = double(input21[i]);
			fRec43[0] = max((fRec43[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph42 = FAUSTFLOAT(fRec43[0]);
			double fTemp42 = (0.33333333333333331 * fTemp41);
			fRec42[0] = max((fRec42[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fVbargraph43 = FAUSTFLOAT(fRec42[0]);
			output21[i] = FAUSTFLOAT(fTemp42);
			double fTemp43 = double(input22[i]);
			fRec45[0] = max((fRec45[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph44 = FAUSTFLOAT(fRec45[0]);
			double fTemp44 = (0.33333333333333331 * fTemp43);
			fRec44[0] = max((fRec44[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fVbargraph45 = FAUSTFLOAT(fRec44[0]);
			output22[i] = FAUSTFLOAT(fTemp44);
			double fTemp45 = double(input23[i]);
			fRec47[0] = max((fRec47[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fVbargraph46 = FAUSTFLOAT(fRec47[0]);
			double fTemp46 = (0.33333333333333331 * fTemp45);
			fRec46[0] = max((fRec46[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph47 = FAUSTFLOAT(fRec46[0]);
			output23[i] = FAUSTFLOAT(fTemp46);
			double fTemp47 = double(input24[i]);
			fRec49[0] = max((fRec49[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fVbargraph48 = FAUSTFLOAT(fRec49[0]);
			double fTemp48 = (0.33333333333333331 * fTemp47);
			fRec48[0] = max((fRec48[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph49 = FAUSTFLOAT(fRec48[0]);
			output24[i] = FAUSTFLOAT(fTemp48);
			double fTemp49 = double(input25[i]);
			fRec51[0] = max((fRec51[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fVbargraph50 = FAUSTFLOAT(fRec51[0]);
			double fTemp50 = (0.30151134457776363 * fTemp49);
			fRec50[0] = max((fRec50[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph51 = FAUSTFLOAT(fRec50[0]);
			output25[i] = FAUSTFLOAT(fTemp50);
			double fTemp51 = double(input26[i]);
			fRec53[0] = max((fRec53[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph52 = FAUSTFLOAT(fRec53[0]);
			double fTemp52 = (0.30151134457776363 * fTemp51);
			fRec52[0] = max((fRec52[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fVbargraph53 = FAUSTFLOAT(fRec52[0]);
			output26[i] = FAUSTFLOAT(fTemp52);
			double fTemp53 = double(input27[i]);
			fRec55[0] = max((fRec55[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fVbargraph54 = FAUSTFLOAT(fRec55[0]);
			double fTemp54 = (0.30151134457776363 * fTemp53);
			fRec54[0] = max((fRec54[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fVbargraph55 = FAUSTFLOAT(fRec54[0]);
			output27[i] = FAUSTFLOAT(fTemp54);
			double fTemp55 = double(input28[i]);
			fRec57[0] = max((fRec57[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph56 = FAUSTFLOAT(fRec57[0]);
			double fTemp56 = (0.30151134457776363 * fTemp55);
			fRec56[0] = max((fRec56[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fVbargraph57 = FAUSTFLOAT(fRec56[0]);
			output28[i] = FAUSTFLOAT(fTemp56);
			double fTemp57 = double(input29[i]);
			fRec59[0] = max((fRec59[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fVbargraph58 = FAUSTFLOAT(fRec59[0]);
			double fTemp58 = (0.30151134457776363 * fTemp57);
			fRec58[0] = max((fRec58[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fVbargraph59 = FAUSTFLOAT(fRec58[0]);
			output29[i] = FAUSTFLOAT(fTemp58);
			double fTemp59 = double(input30[i]);
			fRec61[0] = max((fRec61[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fVbargraph60 = FAUSTFLOAT(fRec61[0]);
			double fTemp60 = (0.30151134457776363 * fTemp59);
			fRec60[0] = max((fRec60[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fVbargraph61 = FAUSTFLOAT(fRec60[0]);
			output30[i] = FAUSTFLOAT(fTemp60);
			double fTemp61 = double(input31[i]);
			fRec63[0] = max((fRec63[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fVbargraph62 = FAUSTFLOAT(fRec63[0]);
			double fTemp62 = (0.30151134457776363 * fTemp61);
			fRec62[0] = max((fRec62[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fVbargraph63 = FAUSTFLOAT(fRec62[0]);
			output31[i] = FAUSTFLOAT(fTemp62);
			double fTemp63 = double(input32[i]);
			fRec65[0] = max((fRec65[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fVbargraph64 = FAUSTFLOAT(fRec65[0]);
			double fTemp64 = (0.30151134457776363 * fTemp63);
			fRec64[0] = max((fRec64[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fVbargraph65 = FAUSTFLOAT(fRec64[0]);
			output32[i] = FAUSTFLOAT(fTemp64);
			double fTemp65 = double(input33[i]);
			fRec67[0] = max((fRec67[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fVbargraph66 = FAUSTFLOAT(fRec67[0]);
			double fTemp66 = (0.30151134457776363 * fTemp65);
			fRec66[0] = max((fRec66[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fVbargraph67 = FAUSTFLOAT(fRec66[0]);
			output33[i] = FAUSTFLOAT(fTemp66);
			double fTemp67 = double(input34[i]);
			fRec69[0] = max((fRec69[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp67))))));
			fVbargraph68 = FAUSTFLOAT(fRec69[0]);
			double fTemp68 = (0.30151134457776363 * fTemp67);
			fRec68[0] = max((fRec68[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fVbargraph69 = FAUSTFLOAT(fRec68[0]);
			output34[i] = FAUSTFLOAT(fTemp68);
			double fTemp69 = double(input35[i]);
			fRec71[0] = max((fRec71[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp69))))));
			fVbargraph70 = FAUSTFLOAT(fRec71[0]);
			double fTemp70 = (0.30151134457776363 * fTemp69);
			fRec70[0] = max((fRec70[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp70))))));
			fVbargraph71 = FAUSTFLOAT(fRec70[0]);
			output35[i] = FAUSTFLOAT(fTemp70);
			double fTemp71 = double(input36[i]);
			fRec73[0] = max((fRec73[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fVbargraph72 = FAUSTFLOAT(fRec73[0]);
			double fTemp72 = (0.27735009811261457 * fTemp71);
			fRec72[0] = max((fRec72[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp72))))));
			fVbargraph73 = FAUSTFLOAT(fRec72[0]);
			output36[i] = FAUSTFLOAT(fTemp72);
			double fTemp73 = double(input37[i]);
			fRec75[0] = max((fRec75[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp73))))));
			fVbargraph74 = FAUSTFLOAT(fRec75[0]);
			double fTemp74 = (0.27735009811261457 * fTemp73);
			fRec74[0] = max((fRec74[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp74))))));
			fVbargraph75 = FAUSTFLOAT(fRec74[0]);
			output37[i] = FAUSTFLOAT(fTemp74);
			double fTemp75 = double(input38[i]);
			fRec77[0] = max((fRec77[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp75))))));
			fVbargraph76 = FAUSTFLOAT(fRec77[0]);
			double fTemp76 = (0.27735009811261457 * fTemp75);
			fRec76[0] = max((fRec76[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp76))))));
			fVbargraph77 = FAUSTFLOAT(fRec76[0]);
			output38[i] = FAUSTFLOAT(fTemp76);
			double fTemp77 = double(input39[i]);
			fRec79[0] = max((fRec79[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp77))))));
			fVbargraph78 = FAUSTFLOAT(fRec79[0]);
			double fTemp78 = (0.27735009811261457 * fTemp77);
			fRec78[0] = max((fRec78[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fVbargraph79 = FAUSTFLOAT(fRec78[0]);
			output39[i] = FAUSTFLOAT(fTemp78);
			double fTemp79 = double(input40[i]);
			fRec81[0] = max((fRec81[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp79))))));
			fVbargraph80 = FAUSTFLOAT(fRec81[0]);
			double fTemp80 = (0.27735009811261457 * fTemp79);
			fRec80[0] = max((fRec80[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp80))))));
			fVbargraph81 = FAUSTFLOAT(fRec80[0]);
			output40[i] = FAUSTFLOAT(fTemp80);
			double fTemp81 = double(input41[i]);
			fRec83[0] = max((fRec83[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp81))))));
			fVbargraph82 = FAUSTFLOAT(fRec83[0]);
			double fTemp82 = (0.27735009811261457 * fTemp81);
			fRec82[0] = max((fRec82[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp82))))));
			fVbargraph83 = FAUSTFLOAT(fRec82[0]);
			output41[i] = FAUSTFLOAT(fTemp82);
			double fTemp83 = double(input42[i]);
			fRec85[0] = max((fRec85[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp83))))));
			fVbargraph84 = FAUSTFLOAT(fRec85[0]);
			double fTemp84 = (0.27735009811261457 * fTemp83);
			fRec84[0] = max((fRec84[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp84))))));
			fVbargraph85 = FAUSTFLOAT(fRec84[0]);
			output42[i] = FAUSTFLOAT(fTemp84);
			double fTemp85 = double(input43[i]);
			fRec87[0] = max((fRec87[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp85))))));
			fVbargraph86 = FAUSTFLOAT(fRec87[0]);
			double fTemp86 = (0.27735009811261457 * fTemp85);
			fRec86[0] = max((fRec86[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp86))))));
			fVbargraph87 = FAUSTFLOAT(fRec86[0]);
			output43[i] = FAUSTFLOAT(fTemp86);
			double fTemp87 = double(input44[i]);
			fRec89[0] = max((fRec89[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp87))))));
			fVbargraph88 = FAUSTFLOAT(fRec89[0]);
			double fTemp88 = (0.27735009811261457 * fTemp87);
			fRec88[0] = max((fRec88[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp88))))));
			fVbargraph89 = FAUSTFLOAT(fRec88[0]);
			output44[i] = FAUSTFLOAT(fTemp88);
			double fTemp89 = double(input45[i]);
			fRec91[0] = max((fRec91[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp89))))));
			fVbargraph90 = FAUSTFLOAT(fRec91[0]);
			double fTemp90 = (0.27735009811261457 * fTemp89);
			fRec90[0] = max((fRec90[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp90))))));
			fVbargraph91 = FAUSTFLOAT(fRec90[0]);
			output45[i] = FAUSTFLOAT(fTemp90);
			double fTemp91 = double(input46[i]);
			fRec93[0] = max((fRec93[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp91))))));
			fVbargraph92 = FAUSTFLOAT(fRec93[0]);
			double fTemp92 = (0.27735009811261457 * fTemp91);
			fRec92[0] = max((fRec92[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp92))))));
			fVbargraph93 = FAUSTFLOAT(fRec92[0]);
			output46[i] = FAUSTFLOAT(fTemp92);
			double fTemp93 = double(input47[i]);
			fRec95[0] = max((fRec95[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fVbargraph94 = FAUSTFLOAT(fRec95[0]);
			double fTemp94 = (0.27735009811261457 * fTemp93);
			fRec94[0] = max((fRec94[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp94))))));
			fVbargraph95 = FAUSTFLOAT(fRec94[0]);
			output47[i] = FAUSTFLOAT(fTemp94);
			double fTemp95 = double(input48[i]);
			fRec97[0] = max((fRec97[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp95))))));
			fVbargraph96 = FAUSTFLOAT(fRec97[0]);
			double fTemp96 = (0.27735009811261457 * fTemp95);
			fRec96[0] = max((fRec96[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp96))))));
			fVbargraph97 = FAUSTFLOAT(fRec96[0]);
			output48[i] = FAUSTFLOAT(fTemp96);
			double fTemp97 = double(input49[i]);
			fRec99[0] = max((fRec99[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp97))))));
			fVbargraph98 = FAUSTFLOAT(fRec99[0]);
			double fTemp98 = (0.2581988897471611 * fTemp97);
			fRec98[0] = max((fRec98[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp98))))));
			fVbargraph99 = FAUSTFLOAT(fRec98[0]);
			output49[i] = FAUSTFLOAT(fTemp98);
			double fTemp99 = double(input50[i]);
			fRec101[0] = max((fRec101[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp99))))));
			fVbargraph100 = FAUSTFLOAT(fRec101[0]);
			double fTemp100 = (0.2581988897471611 * fTemp99);
			fRec100[0] = max((fRec100[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp100))))));
			fVbargraph101 = FAUSTFLOAT(fRec100[0]);
			output50[i] = FAUSTFLOAT(fTemp100);
			double fTemp101 = double(input51[i]);
			fRec103[0] = max((fRec103[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp101))))));
			fVbargraph102 = FAUSTFLOAT(fRec103[0]);
			double fTemp102 = (0.2581988897471611 * fTemp101);
			fRec102[0] = max((fRec102[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp102))))));
			fVbargraph103 = FAUSTFLOAT(fRec102[0]);
			output51[i] = FAUSTFLOAT(fTemp102);
			double fTemp103 = double(input52[i]);
			fRec105[0] = max((fRec105[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp103))))));
			fVbargraph104 = FAUSTFLOAT(fRec105[0]);
			double fTemp104 = (0.2581988897471611 * fTemp103);
			fRec104[0] = max((fRec104[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp104))))));
			fVbargraph105 = FAUSTFLOAT(fRec104[0]);
			output52[i] = FAUSTFLOAT(fTemp104);
			double fTemp105 = double(input53[i]);
			fRec107[0] = max((fRec107[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp105))))));
			fVbargraph106 = FAUSTFLOAT(fRec107[0]);
			double fTemp106 = (0.2581988897471611 * fTemp105);
			fRec106[0] = max((fRec106[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp106))))));
			fVbargraph107 = FAUSTFLOAT(fRec106[0]);
			output53[i] = FAUSTFLOAT(fTemp106);
			double fTemp107 = double(input54[i]);
			fRec109[0] = max((fRec109[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp107))))));
			fVbargraph108 = FAUSTFLOAT(fRec109[0]);
			double fTemp108 = (0.2581988897471611 * fTemp107);
			fRec108[0] = max((fRec108[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp108))))));
			fVbargraph109 = FAUSTFLOAT(fRec108[0]);
			output54[i] = FAUSTFLOAT(fTemp108);
			double fTemp109 = double(input55[i]);
			fRec111[0] = max((fRec111[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp109))))));
			fVbargraph110 = FAUSTFLOAT(fRec111[0]);
			double fTemp110 = (0.2581988897471611 * fTemp109);
			fRec110[0] = max((fRec110[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp110))))));
			fVbargraph111 = FAUSTFLOAT(fRec110[0]);
			output55[i] = FAUSTFLOAT(fTemp110);
			double fTemp111 = double(input56[i]);
			fRec113[0] = max((fRec113[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp111))))));
			fVbargraph112 = FAUSTFLOAT(fRec113[0]);
			double fTemp112 = (0.2581988897471611 * fTemp111);
			fRec112[0] = max((fRec112[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp112))))));
			fVbargraph113 = FAUSTFLOAT(fRec112[0]);
			output56[i] = FAUSTFLOAT(fTemp112);
			double fTemp113 = double(input57[i]);
			fRec115[0] = max((fRec115[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp113))))));
			fVbargraph114 = FAUSTFLOAT(fRec115[0]);
			double fTemp114 = (0.2581988897471611 * fTemp113);
			fRec114[0] = max((fRec114[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp114))))));
			fVbargraph115 = FAUSTFLOAT(fRec114[0]);
			output57[i] = FAUSTFLOAT(fTemp114);
			double fTemp115 = double(input58[i]);
			fRec117[0] = max((fRec117[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp115))))));
			fVbargraph116 = FAUSTFLOAT(fRec117[0]);
			double fTemp116 = (0.2581988897471611 * fTemp115);
			fRec116[0] = max((fRec116[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp116))))));
			fVbargraph117 = FAUSTFLOAT(fRec116[0]);
			output58[i] = FAUSTFLOAT(fTemp116);
			double fTemp117 = double(input59[i]);
			fRec119[0] = max((fRec119[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp117))))));
			fVbargraph118 = FAUSTFLOAT(fRec119[0]);
			double fTemp118 = (0.2581988897471611 * fTemp117);
			fRec118[0] = max((fRec118[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp118))))));
			fVbargraph119 = FAUSTFLOAT(fRec118[0]);
			output59[i] = FAUSTFLOAT(fTemp118);
			double fTemp119 = double(input60[i]);
			fRec121[0] = max((fRec121[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp119))))));
			fVbargraph120 = FAUSTFLOAT(fRec121[0]);
			double fTemp120 = (0.2581988897471611 * fTemp119);
			fRec120[0] = max((fRec120[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp120))))));
			fVbargraph121 = FAUSTFLOAT(fRec120[0]);
			output60[i] = FAUSTFLOAT(fTemp120);
			double fTemp121 = double(input61[i]);
			fRec123[0] = max((fRec123[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp121))))));
			fVbargraph122 = FAUSTFLOAT(fRec123[0]);
			double fTemp122 = (0.2581988897471611 * fTemp121);
			fRec122[0] = max((fRec122[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp122))))));
			fVbargraph123 = FAUSTFLOAT(fRec122[0]);
			output61[i] = FAUSTFLOAT(fTemp122);
			double fTemp123 = double(input62[i]);
			fRec125[0] = max((fRec125[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp123))))));
			fVbargraph124 = FAUSTFLOAT(fRec125[0]);
			double fTemp124 = (0.2581988897471611 * fTemp123);
			fRec124[0] = max((fRec124[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp124))))));
			fVbargraph125 = FAUSTFLOAT(fRec124[0]);
			output62[i] = FAUSTFLOAT(fTemp124);
			double fTemp125 = double(input63[i]);
			fRec127[0] = max((fRec127[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp125))))));
			fVbargraph126 = FAUSTFLOAT(fRec127[0]);
			double fTemp126 = (0.2581988897471611 * fTemp125);
			fRec126[0] = max((fRec126[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp126))))));
			fVbargraph127 = FAUSTFLOAT(fRec126[0]);
			output63[i] = FAUSTFLOAT(fTemp126);
			double fTemp127 = double(input64[i]);
			fRec129[0] = max((fRec129[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp127))))));
			fVbargraph128 = FAUSTFLOAT(fRec129[0]);
			double fTemp128 = (0.24253562503633297 * fTemp127);
			fRec128[0] = max((fRec128[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp128))))));
			fVbargraph129 = FAUSTFLOAT(fRec128[0]);
			output64[i] = FAUSTFLOAT(fTemp128);
			double fTemp129 = double(input65[i]);
			fRec131[0] = max((fRec131[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp129))))));
			fVbargraph130 = FAUSTFLOAT(fRec131[0]);
			double fTemp130 = (0.24253562503633297 * fTemp129);
			fRec130[0] = max((fRec130[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp130))))));
			fVbargraph131 = FAUSTFLOAT(fRec130[0]);
			output65[i] = FAUSTFLOAT(fTemp130);
			double fTemp131 = double(input66[i]);
			fRec133[0] = max((fRec133[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp131))))));
			fVbargraph132 = FAUSTFLOAT(fRec133[0]);
			double fTemp132 = (0.24253562503633297 * fTemp131);
			fRec132[0] = max((fRec132[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp132))))));
			fVbargraph133 = FAUSTFLOAT(fRec132[0]);
			output66[i] = FAUSTFLOAT(fTemp132);
			double fTemp133 = double(input67[i]);
			fRec135[0] = max((fRec135[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp133))))));
			fVbargraph134 = FAUSTFLOAT(fRec135[0]);
			double fTemp134 = (0.24253562503633297 * fTemp133);
			fRec134[0] = max((fRec134[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp134))))));
			fVbargraph135 = FAUSTFLOAT(fRec134[0]);
			output67[i] = FAUSTFLOAT(fTemp134);
			double fTemp135 = double(input68[i]);
			fRec137[0] = max((fRec137[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp135))))));
			fVbargraph136 = FAUSTFLOAT(fRec137[0]);
			double fTemp136 = (0.24253562503633297 * fTemp135);
			fRec136[0] = max((fRec136[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp136))))));
			fVbargraph137 = FAUSTFLOAT(fRec136[0]);
			output68[i] = FAUSTFLOAT(fTemp136);
			double fTemp137 = double(input69[i]);
			fRec139[0] = max((fRec139[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp137))))));
			fVbargraph138 = FAUSTFLOAT(fRec139[0]);
			double fTemp138 = (0.24253562503633297 * fTemp137);
			fRec138[0] = max((fRec138[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp138))))));
			fVbargraph139 = FAUSTFLOAT(fRec138[0]);
			output69[i] = FAUSTFLOAT(fTemp138);
			double fTemp139 = double(input70[i]);
			fRec141[0] = max((fRec141[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp139))))));
			fVbargraph140 = FAUSTFLOAT(fRec141[0]);
			double fTemp140 = (0.24253562503633297 * fTemp139);
			fRec140[0] = max((fRec140[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp140))))));
			fVbargraph141 = FAUSTFLOAT(fRec140[0]);
			output70[i] = FAUSTFLOAT(fTemp140);
			double fTemp141 = double(input71[i]);
			fRec143[0] = max((fRec143[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp141))))));
			fVbargraph142 = FAUSTFLOAT(fRec143[0]);
			double fTemp142 = (0.24253562503633297 * fTemp141);
			fRec142[0] = max((fRec142[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp142))))));
			fVbargraph143 = FAUSTFLOAT(fRec142[0]);
			output71[i] = FAUSTFLOAT(fTemp142);
			double fTemp143 = double(input72[i]);
			fRec145[0] = max((fRec145[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp143))))));
			fVbargraph144 = FAUSTFLOAT(fRec145[0]);
			double fTemp144 = (0.24253562503633297 * fTemp143);
			fRec144[0] = max((fRec144[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp144))))));
			fVbargraph145 = FAUSTFLOAT(fRec144[0]);
			output72[i] = FAUSTFLOAT(fTemp144);
			double fTemp145 = double(input73[i]);
			fRec147[0] = max((fRec147[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp145))))));
			fVbargraph146 = FAUSTFLOAT(fRec147[0]);
			double fTemp146 = (0.24253562503633297 * fTemp145);
			fRec146[0] = max((fRec146[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp146))))));
			fVbargraph147 = FAUSTFLOAT(fRec146[0]);
			output73[i] = FAUSTFLOAT(fTemp146);
			double fTemp147 = double(input74[i]);
			fRec149[0] = max((fRec149[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp147))))));
			fVbargraph148 = FAUSTFLOAT(fRec149[0]);
			double fTemp148 = (0.24253562503633297 * fTemp147);
			fRec148[0] = max((fRec148[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp148))))));
			fVbargraph149 = FAUSTFLOAT(fRec148[0]);
			output74[i] = FAUSTFLOAT(fTemp148);
			double fTemp149 = double(input75[i]);
			fRec151[0] = max((fRec151[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp149))))));
			fVbargraph150 = FAUSTFLOAT(fRec151[0]);
			double fTemp150 = (0.24253562503633297 * fTemp149);
			fRec150[0] = max((fRec150[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp150))))));
			fVbargraph151 = FAUSTFLOAT(fRec150[0]);
			output75[i] = FAUSTFLOAT(fTemp150);
			double fTemp151 = double(input76[i]);
			fRec153[0] = max((fRec153[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp151))))));
			fVbargraph152 = FAUSTFLOAT(fRec153[0]);
			double fTemp152 = (0.24253562503633297 * fTemp151);
			fRec152[0] = max((fRec152[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp152))))));
			fVbargraph153 = FAUSTFLOAT(fRec152[0]);
			output76[i] = FAUSTFLOAT(fTemp152);
			double fTemp153 = double(input77[i]);
			fRec155[0] = max((fRec155[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp153))))));
			fVbargraph154 = FAUSTFLOAT(fRec155[0]);
			double fTemp154 = (0.24253562503633297 * fTemp153);
			fRec154[0] = max((fRec154[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp154))))));
			fVbargraph155 = FAUSTFLOAT(fRec154[0]);
			output77[i] = FAUSTFLOAT(fTemp154);
			double fTemp155 = double(input78[i]);
			fRec157[0] = max((fRec157[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp155))))));
			fVbargraph156 = FAUSTFLOAT(fRec157[0]);
			double fTemp156 = (0.24253562503633297 * fTemp155);
			fRec156[0] = max((fRec156[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp156))))));
			fVbargraph157 = FAUSTFLOAT(fRec156[0]);
			output78[i] = FAUSTFLOAT(fTemp156);
			double fTemp157 = double(input79[i]);
			fRec159[0] = max((fRec159[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp157))))));
			fVbargraph158 = FAUSTFLOAT(fRec159[0]);
			double fTemp158 = (0.24253562503633297 * fTemp157);
			fRec158[0] = max((fRec158[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp158))))));
			fVbargraph159 = FAUSTFLOAT(fRec158[0]);
			output79[i] = FAUSTFLOAT(fTemp158);
			double fTemp159 = double(input80[i]);
			fRec161[0] = max((fRec161[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp159))))));
			fVbargraph160 = FAUSTFLOAT(fRec161[0]);
			double fTemp160 = (0.24253562503633297 * fTemp159);
			fRec160[0] = max((fRec160[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp160))))));
			fVbargraph161 = FAUSTFLOAT(fRec160[0]);
			output80[i] = FAUSTFLOAT(fTemp160);
			double fTemp161 = double(input81[i]);
			fRec163[0] = max((fRec163[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp161))))));
			fVbargraph162 = FAUSTFLOAT(fRec163[0]);
			double fTemp162 = (0.22941573387056174 * fTemp161);
			fRec162[0] = max((fRec162[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp162))))));
			fVbargraph163 = FAUSTFLOAT(fRec162[0]);
			output81[i] = FAUSTFLOAT(fTemp162);
			double fTemp163 = double(input82[i]);
			fRec165[0] = max((fRec165[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp163))))));
			fVbargraph164 = FAUSTFLOAT(fRec165[0]);
			double fTemp164 = (0.22941573387056174 * fTemp163);
			fRec164[0] = max((fRec164[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp164))))));
			fVbargraph165 = FAUSTFLOAT(fRec164[0]);
			output82[i] = FAUSTFLOAT(fTemp164);
			double fTemp165 = double(input83[i]);
			fRec167[0] = max((fRec167[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp165))))));
			fVbargraph166 = FAUSTFLOAT(fRec167[0]);
			double fTemp166 = (0.22941573387056174 * fTemp165);
			fRec166[0] = max((fRec166[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp166))))));
			fVbargraph167 = FAUSTFLOAT(fRec166[0]);
			output83[i] = FAUSTFLOAT(fTemp166);
			double fTemp167 = double(input84[i]);
			fRec169[0] = max((fRec169[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp167))))));
			fVbargraph168 = FAUSTFLOAT(fRec169[0]);
			double fTemp168 = (0.22941573387056174 * fTemp167);
			fRec168[0] = max((fRec168[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp168))))));
			fVbargraph169 = FAUSTFLOAT(fRec168[0]);
			output84[i] = FAUSTFLOAT(fTemp168);
			double fTemp169 = double(input85[i]);
			fRec171[0] = max((fRec171[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp169))))));
			fVbargraph170 = FAUSTFLOAT(fRec171[0]);
			double fTemp170 = (0.22941573387056174 * fTemp169);
			fRec170[0] = max((fRec170[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp170))))));
			fVbargraph171 = FAUSTFLOAT(fRec170[0]);
			output85[i] = FAUSTFLOAT(fTemp170);
			double fTemp171 = double(input86[i]);
			fRec173[0] = max((fRec173[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp171))))));
			fVbargraph172 = FAUSTFLOAT(fRec173[0]);
			double fTemp172 = (0.22941573387056174 * fTemp171);
			fRec172[0] = max((fRec172[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp172))))));
			fVbargraph173 = FAUSTFLOAT(fRec172[0]);
			output86[i] = FAUSTFLOAT(fTemp172);
			double fTemp173 = double(input87[i]);
			fRec175[0] = max((fRec175[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp173))))));
			fVbargraph174 = FAUSTFLOAT(fRec175[0]);
			double fTemp174 = (0.22941573387056174 * fTemp173);
			fRec174[0] = max((fRec174[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp174))))));
			fVbargraph175 = FAUSTFLOAT(fRec174[0]);
			output87[i] = FAUSTFLOAT(fTemp174);
			double fTemp175 = double(input88[i]);
			fRec177[0] = max((fRec177[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp175))))));
			fVbargraph176 = FAUSTFLOAT(fRec177[0]);
			double fTemp176 = (0.22941573387056174 * fTemp175);
			fRec176[0] = max((fRec176[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp176))))));
			fVbargraph177 = FAUSTFLOAT(fRec176[0]);
			output88[i] = FAUSTFLOAT(fTemp176);
			double fTemp177 = double(input89[i]);
			fRec179[0] = max((fRec179[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp177))))));
			fVbargraph178 = FAUSTFLOAT(fRec179[0]);
			double fTemp178 = (0.22941573387056174 * fTemp177);
			fRec178[0] = max((fRec178[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp178))))));
			fVbargraph179 = FAUSTFLOAT(fRec178[0]);
			output89[i] = FAUSTFLOAT(fTemp178);
			double fTemp179 = double(input90[i]);
			fRec181[0] = max((fRec181[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp179))))));
			fVbargraph180 = FAUSTFLOAT(fRec181[0]);
			double fTemp180 = (0.22941573387056174 * fTemp179);
			fRec180[0] = max((fRec180[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp180))))));
			fVbargraph181 = FAUSTFLOAT(fRec180[0]);
			output90[i] = FAUSTFLOAT(fTemp180);
			double fTemp181 = double(input91[i]);
			fRec183[0] = max((fRec183[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp181))))));
			fVbargraph182 = FAUSTFLOAT(fRec183[0]);
			double fTemp182 = (0.22941573387056174 * fTemp181);
			fRec182[0] = max((fRec182[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp182))))));
			fVbargraph183 = FAUSTFLOAT(fRec182[0]);
			output91[i] = FAUSTFLOAT(fTemp182);
			double fTemp183 = double(input92[i]);
			fRec185[0] = max((fRec185[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp183))))));
			fVbargraph184 = FAUSTFLOAT(fRec185[0]);
			double fTemp184 = (0.22941573387056174 * fTemp183);
			fRec184[0] = max((fRec184[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp184))))));
			fVbargraph185 = FAUSTFLOAT(fRec184[0]);
			output92[i] = FAUSTFLOAT(fTemp184);
			double fTemp185 = double(input93[i]);
			fRec187[0] = max((fRec187[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp185))))));
			fVbargraph186 = FAUSTFLOAT(fRec187[0]);
			double fTemp186 = (0.22941573387056174 * fTemp185);
			fRec186[0] = max((fRec186[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp186))))));
			fVbargraph187 = FAUSTFLOAT(fRec186[0]);
			output93[i] = FAUSTFLOAT(fTemp186);
			double fTemp187 = double(input94[i]);
			fRec189[0] = max((fRec189[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp187))))));
			fVbargraph188 = FAUSTFLOAT(fRec189[0]);
			double fTemp188 = (0.22941573387056174 * fTemp187);
			fRec188[0] = max((fRec188[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp188))))));
			fVbargraph189 = FAUSTFLOAT(fRec188[0]);
			output94[i] = FAUSTFLOAT(fTemp188);
			double fTemp189 = double(input95[i]);
			fRec191[0] = max((fRec191[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp189))))));
			fVbargraph190 = FAUSTFLOAT(fRec191[0]);
			double fTemp190 = (0.22941573387056174 * fTemp189);
			fRec190[0] = max((fRec190[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp190))))));
			fVbargraph191 = FAUSTFLOAT(fRec190[0]);
			output95[i] = FAUSTFLOAT(fTemp190);
			double fTemp191 = double(input96[i]);
			fRec193[0] = max((fRec193[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp191))))));
			fVbargraph192 = FAUSTFLOAT(fRec193[0]);
			double fTemp192 = (0.22941573387056174 * fTemp191);
			fRec192[0] = max((fRec192[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp192))))));
			fVbargraph193 = FAUSTFLOAT(fRec192[0]);
			output96[i] = FAUSTFLOAT(fTemp192);
			double fTemp193 = double(input97[i]);
			fRec195[0] = max((fRec195[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp193))))));
			fVbargraph194 = FAUSTFLOAT(fRec195[0]);
			double fTemp194 = (0.22941573387056174 * fTemp193);
			fRec194[0] = max((fRec194[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp194))))));
			fVbargraph195 = FAUSTFLOAT(fRec194[0]);
			output97[i] = FAUSTFLOAT(fTemp194);
			double fTemp195 = double(input98[i]);
			fRec197[0] = max((fRec197[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp195))))));
			fVbargraph196 = FAUSTFLOAT(fRec197[0]);
			double fTemp196 = (0.22941573387056174 * fTemp195);
			fRec196[0] = max((fRec196[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp196))))));
			fVbargraph197 = FAUSTFLOAT(fRec196[0]);
			output98[i] = FAUSTFLOAT(fTemp196);
			double fTemp197 = double(input99[i]);
			fRec199[0] = max((fRec199[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp197))))));
			fVbargraph198 = FAUSTFLOAT(fRec199[0]);
			double fTemp198 = (0.22941573387056174 * fTemp197);
			fRec198[0] = max((fRec198[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp198))))));
			fVbargraph199 = FAUSTFLOAT(fRec198[0]);
			output99[i] = FAUSTFLOAT(fTemp198);
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec3[1] = fRec3[0];
			fRec2[1] = fRec2[0];
			fRec5[1] = fRec5[0];
			fRec4[1] = fRec4[0];
			fRec7[1] = fRec7[0];
			fRec6[1] = fRec6[0];
			fRec9[1] = fRec9[0];
			fRec8[1] = fRec8[0];
			fRec11[1] = fRec11[0];
			fRec10[1] = fRec10[0];
			fRec13[1] = fRec13[0];
			fRec12[1] = fRec12[0];
			fRec15[1] = fRec15[0];
			fRec14[1] = fRec14[0];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			fRec19[1] = fRec19[0];
			fRec18[1] = fRec18[0];
			fRec21[1] = fRec21[0];
			fRec20[1] = fRec20[0];
			fRec23[1] = fRec23[0];
			fRec22[1] = fRec22[0];
			fRec25[1] = fRec25[0];
			fRec24[1] = fRec24[0];
			fRec27[1] = fRec27[0];
			fRec26[1] = fRec26[0];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec33[1] = fRec33[0];
			fRec32[1] = fRec32[0];
			fRec35[1] = fRec35[0];
			fRec34[1] = fRec34[0];
			fRec37[1] = fRec37[0];
			fRec36[1] = fRec36[0];
			fRec39[1] = fRec39[0];
			fRec38[1] = fRec38[0];
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fRec43[1] = fRec43[0];
			fRec42[1] = fRec42[0];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec47[1] = fRec47[0];
			fRec46[1] = fRec46[0];
			fRec49[1] = fRec49[0];
			fRec48[1] = fRec48[0];
			fRec51[1] = fRec51[0];
			fRec50[1] = fRec50[0];
			fRec53[1] = fRec53[0];
			fRec52[1] = fRec52[0];
			fRec55[1] = fRec55[0];
			fRec54[1] = fRec54[0];
			fRec57[1] = fRec57[0];
			fRec56[1] = fRec56[0];
			fRec59[1] = fRec59[0];
			fRec58[1] = fRec58[0];
			fRec61[1] = fRec61[0];
			fRec60[1] = fRec60[0];
			fRec63[1] = fRec63[0];
			fRec62[1] = fRec62[0];
			fRec65[1] = fRec65[0];
			fRec64[1] = fRec64[0];
			fRec67[1] = fRec67[0];
			fRec66[1] = fRec66[0];
			fRec69[1] = fRec69[0];
			fRec68[1] = fRec68[0];
			fRec71[1] = fRec71[0];
			fRec70[1] = fRec70[0];
			fRec73[1] = fRec73[0];
			fRec72[1] = fRec72[0];
			fRec75[1] = fRec75[0];
			fRec74[1] = fRec74[0];
			fRec77[1] = fRec77[0];
			fRec76[1] = fRec76[0];
			fRec79[1] = fRec79[0];
			fRec78[1] = fRec78[0];
			fRec81[1] = fRec81[0];
			fRec80[1] = fRec80[0];
			fRec83[1] = fRec83[0];
			fRec82[1] = fRec82[0];
			fRec85[1] = fRec85[0];
			fRec84[1] = fRec84[0];
			fRec87[1] = fRec87[0];
			fRec86[1] = fRec86[0];
			fRec89[1] = fRec89[0];
			fRec88[1] = fRec88[0];
			fRec91[1] = fRec91[0];
			fRec90[1] = fRec90[0];
			fRec93[1] = fRec93[0];
			fRec92[1] = fRec92[0];
			fRec95[1] = fRec95[0];
			fRec94[1] = fRec94[0];
			fRec97[1] = fRec97[0];
			fRec96[1] = fRec96[0];
			fRec99[1] = fRec99[0];
			fRec98[1] = fRec98[0];
			fRec101[1] = fRec101[0];
			fRec100[1] = fRec100[0];
			fRec103[1] = fRec103[0];
			fRec102[1] = fRec102[0];
			fRec105[1] = fRec105[0];
			fRec104[1] = fRec104[0];
			fRec107[1] = fRec107[0];
			fRec106[1] = fRec106[0];
			fRec109[1] = fRec109[0];
			fRec108[1] = fRec108[0];
			fRec111[1] = fRec111[0];
			fRec110[1] = fRec110[0];
			fRec113[1] = fRec113[0];
			fRec112[1] = fRec112[0];
			fRec115[1] = fRec115[0];
			fRec114[1] = fRec114[0];
			fRec117[1] = fRec117[0];
			fRec116[1] = fRec116[0];
			fRec119[1] = fRec119[0];
			fRec118[1] = fRec118[0];
			fRec121[1] = fRec121[0];
			fRec120[1] = fRec120[0];
			fRec123[1] = fRec123[0];
			fRec122[1] = fRec122[0];
			fRec125[1] = fRec125[0];
			fRec124[1] = fRec124[0];
			fRec127[1] = fRec127[0];
			fRec126[1] = fRec126[0];
			fRec129[1] = fRec129[0];
			fRec128[1] = fRec128[0];
			fRec131[1] = fRec131[0];
			fRec130[1] = fRec130[0];
			fRec133[1] = fRec133[0];
			fRec132[1] = fRec132[0];
			fRec135[1] = fRec135[0];
			fRec134[1] = fRec134[0];
			fRec137[1] = fRec137[0];
			fRec136[1] = fRec136[0];
			fRec139[1] = fRec139[0];
			fRec138[1] = fRec138[0];
			fRec141[1] = fRec141[0];
			fRec140[1] = fRec140[0];
			fRec143[1] = fRec143[0];
			fRec142[1] = fRec142[0];
			fRec145[1] = fRec145[0];
			fRec144[1] = fRec144[0];
			fRec147[1] = fRec147[0];
			fRec146[1] = fRec146[0];
			fRec149[1] = fRec149[0];
			fRec148[1] = fRec148[0];
			fRec151[1] = fRec151[0];
			fRec150[1] = fRec150[0];
			fRec153[1] = fRec153[0];
			fRec152[1] = fRec152[0];
			fRec155[1] = fRec155[0];
			fRec154[1] = fRec154[0];
			fRec157[1] = fRec157[0];
			fRec156[1] = fRec156[0];
			fRec159[1] = fRec159[0];
			fRec158[1] = fRec158[0];
			fRec161[1] = fRec161[0];
			fRec160[1] = fRec160[0];
			fRec163[1] = fRec163[0];
			fRec162[1] = fRec162[0];
			fRec165[1] = fRec165[0];
			fRec164[1] = fRec164[0];
			fRec167[1] = fRec167[0];
			fRec166[1] = fRec166[0];
			fRec169[1] = fRec169[0];
			fRec168[1] = fRec168[0];
			fRec171[1] = fRec171[0];
			fRec170[1] = fRec170[0];
			fRec173[1] = fRec173[0];
			fRec172[1] = fRec172[0];
			fRec175[1] = fRec175[0];
			fRec174[1] = fRec174[0];
			fRec177[1] = fRec177[0];
			fRec176[1] = fRec176[0];
			fRec179[1] = fRec179[0];
			fRec178[1] = fRec178[0];
			fRec181[1] = fRec181[0];
			fRec180[1] = fRec180[0];
			fRec183[1] = fRec183[0];
			fRec182[1] = fRec182[0];
			fRec185[1] = fRec185[0];
			fRec184[1] = fRec184[0];
			fRec187[1] = fRec187[0];
			fRec186[1] = fRec186[0];
			fRec189[1] = fRec189[0];
			fRec188[1] = fRec188[0];
			fRec191[1] = fRec191[0];
			fRec190[1] = fRec190[0];
			fRec193[1] = fRec193[0];
			fRec192[1] = fRec192[0];
			fRec195[1] = fRec195[0];
			fRec194[1] = fRec194[0];
			fRec197[1] = fRec197[0];
			fRec196[1] = fRec196[0];
			fRec199[1] = fRec199[0];
			fRec198[1] = fRec198[0];
			
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
