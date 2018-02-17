/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOADecLebedev263"
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
	
	int fSamplingFreq;
	double fConst0;
	double fConst1;
	FAUSTFLOAT fVslider0;
	double fRec1[2];
	FAUSTFLOAT fCheckbox0;
	FAUSTFLOAT fEntry0;
	FAUSTFLOAT fVslider1;
	double fRec3[2];
	double fRec2[2];
	FAUSTFLOAT fVbargraph0;
	double fConst2;
	double fConst3;
	double fRec6[2];
	double fRec5[2];
	double fRec4[2];
	FAUSTFLOAT fVbargraph1;
	double fConst4;
	double fConst5;
	double fRec9[2];
	double fRec10[2];
	double fRec8[2];
	double fRec7[2];
	FAUSTFLOAT fVbargraph2;
	double fConst6;
	double fConst7;
	double fConst8;
	double fRec14[2];
	double fRec15[2];
	double fRec13[2];
	double fConst9;
	double fRec16[2];
	double fRec12[2];
	double fRec11[2];
	FAUSTFLOAT fVbargraph3;
	double fRec0[2];
	FAUSTFLOAT fVbargraph4;
	double fConst10;
	double fConst11;
	double fConst12;
	double fConst13;
	double fConst14;
	double fConst15;
	double fRec20[2];
	double fRec19[2];
	double fRec18[2];
	FAUSTFLOAT fVbargraph5;
	double fConst16;
	double fConst17;
	double fConst18;
	double fConst19;
	double fConst20;
	double fRec23[2];
	double fRec24[2];
	double fRec22[2];
	double fRec21[2];
	FAUSTFLOAT fVbargraph6;
	double fConst21;
	double fConst22;
	double fConst23;
	double fRec28[2];
	double fRec29[2];
	double fRec27[2];
	double fRec30[2];
	double fRec26[2];
	double fRec25[2];
	FAUSTFLOAT fVbargraph7;
	double fConst24;
	double fConst25;
	double fRec34[2];
	double fRec35[2];
	double fRec33[2];
	double fRec36[2];
	double fRec32[2];
	double fRec31[2];
	FAUSTFLOAT fVbargraph8;
	double fRec17[2];
	FAUSTFLOAT fVbargraph9;
	double fRec40[2];
	double fRec39[2];
	double fRec38[2];
	FAUSTFLOAT fVbargraph10;
	double fConst26;
	double fConst27;
	double fRec44[2];
	double fRec45[2];
	double fRec43[2];
	double fRec46[2];
	double fRec42[2];
	double fRec41[2];
	FAUSTFLOAT fVbargraph11;
	double fRec50[2];
	double fRec51[2];
	double fRec49[2];
	double fRec52[2];
	double fRec48[2];
	double fRec47[2];
	FAUSTFLOAT fVbargraph12;
	double fRec37[2];
	FAUSTFLOAT fVbargraph13;
	double fConst28;
	double fConst29;
	double fRec53[2];
	FAUSTFLOAT fVbargraph14;
	double fRec54[2];
	FAUSTFLOAT fVbargraph15;
	double fRec55[2];
	FAUSTFLOAT fVbargraph16;
	double fConst30;
	double fConst31;
	double fConst32;
	double fConst33;
	double fRec60[2];
	double fRec61[2];
	double fRec59[2];
	double fRec62[2];
	double fRec58[2];
	double fRec57[2];
	FAUSTFLOAT fVbargraph17;
	double fConst34;
	double fConst35;
	double fConst36;
	double fRec65[2];
	double fRec66[2];
	double fRec64[2];
	double fRec63[2];
	FAUSTFLOAT fVbargraph18;
	double fConst37;
	double fConst38;
	double fRec56[2];
	FAUSTFLOAT fVbargraph19;
	double fConst39;
	double fConst40;
	double fConst41;
	double fConst42;
	double fRec70[2];
	double fRec71[2];
	double fRec69[2];
	double fRec68[2];
	FAUSTFLOAT fVbargraph20;
	double fRec67[2];
	FAUSTFLOAT fVbargraph21;
	double fConst43;
	double fConst44;
	double fConst45;
	double fConst46;
	double fConst47;
	double fRec72[2];
	FAUSTFLOAT fVbargraph22;
	double fRec73[2];
	FAUSTFLOAT fVbargraph23;
	double fConst48;
	double fRec77[2];
	double fRec78[2];
	double fRec76[2];
	double fRec75[2];
	FAUSTFLOAT fVbargraph24;
	double fConst49;
	double fRec74[2];
	FAUSTFLOAT fVbargraph25;
	double fConst50;
	double fConst51;
	double fConst52;
	double fConst53;
	double fConst54;
	double fRec79[2];
	FAUSTFLOAT fVbargraph26;
	double fConst55;
	double fConst56;
	double fConst57;
	double fConst58;
	double fRec80[2];
	FAUSTFLOAT fVbargraph27;
	double fConst59;
	double fConst60;
	double fConst61;
	double fConst62;
	double fConst63;
	double fRec81[2];
	FAUSTFLOAT fVbargraph28;
	double fConst64;
	double fConst65;
	double fConst66;
	double fConst67;
	double fConst68;
	double fConst69;
	double fRec82[2];
	FAUSTFLOAT fVbargraph29;
	double fConst70;
	double fConst71;
	double fRec83[2];
	FAUSTFLOAT fVbargraph30;
	double fConst72;
	double fConst73;
	double fConst74;
	double fRec84[2];
	FAUSTFLOAT fVbargraph31;
	double fRec85[2];
	FAUSTFLOAT fVbargraph32;
	double fConst75;
	double fConst76;
	double fRec90[2];
	double fRec91[2];
	double fRec89[2];
	double fRec92[2];
	double fRec88[2];
	double fRec87[2];
	FAUSTFLOAT fVbargraph33;
	double fConst77;
	double fConst78;
	double fConst79;
	double fConst80;
	double fConst81;
	double fConst82;
	double fRec86[2];
	FAUSTFLOAT fVbargraph34;
	double fConst83;
	double fConst84;
	double fConst85;
	double fConst86;
	double fConst87;
	double fConst88;
	double fConst89;
	double fConst90;
	double fRec93[2];
	FAUSTFLOAT fVbargraph35;
	double fConst91;
	double fConst92;
	double fConst93;
	double fConst94;
	double fConst95;
	double fConst96;
	double fConst97;
	double fRec94[2];
	FAUSTFLOAT fVbargraph36;
	double fConst98;
	double fConst99;
	double fConst100;
	double fConst101;
	double fConst102;
	double fConst103;
	double fConst104;
	double fRec95[2];
	FAUSTFLOAT fVbargraph37;
	double fConst105;
	double fConst106;
	double fConst107;
	double fConst108;
	double fConst109;
	double fRec96[2];
	FAUSTFLOAT fVbargraph38;
	double fConst110;
	double fConst111;
	double fConst112;
	double fConst113;
	double fConst114;
	double fConst115;
	double fRec97[2];
	FAUSTFLOAT fVbargraph39;
	double fConst116;
	double fConst117;
	double fRec98[2];
	FAUSTFLOAT fVbargraph40;
	double fConst118;
	double fConst119;
	double fConst120;
	double fConst121;
	double fRec99[2];
	FAUSTFLOAT fVbargraph41;
	
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
		m->declare("name", "HOADecLebedev263");
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
		return 16;
		
	}
	virtual int getNumOutputs() {
		return 26;
		
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
		fConst2 = (170.0 / fConst0);
		fConst3 = (340.0 / fConst0);
		fConst4 = (510.0 / fConst0);
		fConst5 = (1020.0 / fConst0);
		fConst6 = (394.77151028643527 / fConst0);
		fConst7 = (625.22848971356643 / fConst0);
		fConst8 = (1250.4569794271329 / fConst0);
		fConst9 = (789.54302057287055 / fConst0);
		fConst10 = double(tgamma(1.5));
		fConst11 = double(tgamma(1.0));
		fConst12 = double(tgamma(3.0));
		fConst13 = sqrt((3.0 * (fConst11 / fConst12)));
		fConst14 = (fConst10 * fConst13);
		fConst15 = (0.07598900579074909 * fConst14);
		fConst16 = double(tgamma(2.5));
		fConst17 = double(tgamma(5.0));
		fConst18 = sqrt((5.0 * (fConst11 / fConst17)));
		fConst19 = (fConst16 * fConst18);
		fConst20 = (0.15197801158149818 * fConst19);
		fConst21 = sqrt((7.0 * (fConst12 / fConst17)));
		fConst22 = (fConst21 * (0.0 - (3.3851375012865379 * fConst10)));
		fConst23 = (0.033671751485073696 * fConst22);
		fConst24 = (double(tgamma(3.5)) * sqrt((7.0 * (fConst11 / double(tgamma(7.0))))));
		fConst25 = (0.30395602316299636 * fConst24);
		fConst26 = (0.0 - (3.1915382432114616 * fConst19));
		fConst27 = (0.0 - (6.3830764864229232 * fConst24));
		fConst28 = (0.0 - (1.5957691216057308 * fConst14));
		fConst29 = (0.0 - (0.70710678118654757 * fConst22));
		fConst30 = (0.06079120463259928 * fConst18);
		fConst31 = double(tgamma(2.0));
		fConst32 = sqrt((7.0 * (fConst31 / double(tgamma(6.0)))));
		fConst33 = (0.2149293651610501 * fConst32);
		fConst34 = (0.042985873032210013 * fConst13);
		fConst35 = sqrt((5.0 * (fConst31 / double(tgamma(4.0)))));
		fConst36 = (0.091186806948898899 * fConst35);
		fConst37 = (0.09671821432247249 * fConst21);
		fConst38 = (0.08597174606442004 * fConst24);
		fConst39 = (0.0 - (1.595769121605731 * fConst19));
		fConst40 = (0.0 - (2.256758334191026 * fConst24));
		fConst41 = (fConst16 * fConst32);
		fConst42 = (0.0 - (5.6418958354775643 * fConst41));
		fConst43 = (0.0 - (1.1283791670955128 * fConst14));
		fConst44 = (fConst10 * fConst35);
		fConst45 = (0.0 - (2.3936536824085959 * fConst44));
		fConst46 = (fConst10 * fConst21);
		fConst47 = (0.0 - (2.5388531259649025 * fConst46));
		fConst48 = (0.0 - (4.5135166683820485 * fConst24));
		fConst49 = (0.17194349212884008 * fConst24);
		fConst50 = (0.042985873032210013 * fConst14);
		fConst51 = (0.019047619047619053 * fConst22);
		fConst52 = (0.0 - (1.1283791670955126 * fConst14));
		fConst53 = (0.0 - (7.0866384831701748e-16 * fConst19));
		fConst54 = (0.0 - (0.5 * fConst22));
		fConst55 = (0.0 - (1.128379167095513 * fConst14));
		fConst56 = (0.0 - (4.5135166683820547 * fConst24));
		fConst57 = (0.0 - (0.50000000000000011 * fConst22));
		fConst58 = (0.17194349212883991 * fConst24);
		fConst59 = (0.042985873032209999 * fConst14);
		fConst60 = (0.019047619047619046 * fConst22);
		fConst61 = (0.0 - (1.417327696634035e-15 * fConst19));
		fConst62 = (0.0 - (4.5135166683820476 * fConst24));
		fConst63 = (0.0 - (4.5135166683820538 * fConst24));
		fConst64 = (0.0 - (1.6925687506432687 * fConst10));
		fConst65 = (fConst35 * fConst64);
		fConst66 = (fConst32 * (0.0 - (3.9894228040143274 * fConst16)));
		fConst67 = (fConst21 * (0.0 - ((3.5355339059327373 * fConst64) + (2.3936536824085963 * fConst10))));
		fConst68 = (0.026937401188058957 * fConst67);
		fConst69 = (0.06079120463259928 * fConst19);
		fConst70 = (0.0 - (1.4142135623730951 * fConst66));
		fConst71 = (0.053874802376117914 * fConst65);
		fConst72 = (0.053874802376117914 * fConst66);
		fConst73 = (0.0 - (1.4142135623730951 * fConst65));
		fConst74 = (0.0 - (0.70710678118654757 * fConst67));
		fConst75 = (0.0 - (2.4568472851294949 * fConst24));
		fConst76 = (0.19742522826933454 * fConst32);
		fConst77 = (4.3837206813298509e-17 * fConst32);
		fConst78 = (0.078970091307733831 * fConst24);
		fConst79 = (0.051292578908755625 * fConst35);
		fConst80 = (0.051292578908755639 * fConst35);
		fConst81 = (0.02961378424040019 * fConst21);
		fConst82 = (0.029613784240400197 * fConst21);
		fConst83 = (0.0 - (0.92131773192356115 * fConst14));
		fConst84 = (0.0 - (2.1276921621409737 * fConst19));
		fConst85 = (0.0 - (1.5957691216057308 * fConst44));
		fConst86 = (0.0 - (4.7244256554467816e-16 * fConst19));
		fConst87 = (0.0 - (6.1421182128237417 * fConst41));
		fConst88 = (0.0 - (0.92131773192356159 * fConst46));
		fConst89 = (0.0 - (1.3638242119692871e-15 * fConst41));
		fConst90 = (0.02961378424040018 * fConst13);
		fConst91 = (0.19742522826933459 * fConst32);
		fConst92 = (8.7674413626597019e-17 * fConst32);
		fConst93 = (0.0 - (0.92131773192356148 * fConst14));
		fConst94 = (0.0 - (1.5957691216057313 * fConst44));
		fConst95 = (0.0 - (2.456847285129498 * fConst24));
		fConst96 = (0.0 - (0.92131773192356192 * fConst46));
		fConst97 = (0.078970091307733734 * fConst24);
		fConst98 = (0.029613784240400173 * fConst13);
		fConst99 = (0.051292578908755618 * fConst35);
		fConst100 = (0.029613784240400187 * fConst21);
		fConst101 = (0.0 - (9.4488513108935631e-16 * fConst19));
		fConst102 = (0.0 - (2.456847285129494 * fConst24));
		fConst103 = (0.0 - (2.7276484239385742e-15 * fConst41));
		fConst104 = (0.0 - (2.4568472851294976 * fConst24));
		fConst105 = (0.0 - (1.5957691216057308 * fConst10));
		fConst106 = (fConst105 * fConst35);
		fConst107 = (fConst32 * (0.0 - (4.3431334391370653 * fConst16)));
		fConst108 = (fConst21 * (0.0 - ((2.8867513459481291 * fConst105) + (2.7639531957706835 * fConst10))));
		fConst109 = (0.032142857142857147 * fConst106);
		fConst110 = (0.0 - fConst106);
		fConst111 = (0.0 - (1.4142135623730951 * fConst107));
		fConst112 = (0.0 - (0.5 * fConst108));
		fConst113 = (0.0 - (3.1401849173675503e-16 * fConst107));
		fConst114 = (0.016071428571428573 * fConst108);
		fConst115 = (0.02961378424040018 * fConst14);
		fConst116 = (0.0 - (1.0000000000000002 * fConst106));
		fConst117 = (0.0 - (0.50000000000000011 * fConst108));
		fConst118 = (0.032142857142857133 * fConst106);
		fConst119 = (0.029613784240400173 * fConst14);
		fConst120 = (0.016071428571428566 * fConst108);
		fConst121 = (0.0 - (6.2803698347351007e-16 * fConst107));
		
	}
	
	virtual void instanceResetUserInterface() {
		fVslider0 = FAUSTFLOAT(0.0);
		fCheckbox0 = FAUSTFLOAT(0.0);
		fEntry0 = FAUSTFLOAT(1.0700000000000001);
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
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec6[l3] = 0.0;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec5[l4] = 0.0;
			
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec4[l5] = 0.0;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec9[l6] = 0.0;
			
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec10[l7] = 0.0;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec8[l8] = 0.0;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fRec7[l9] = 0.0;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec14[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec15[l11] = 0.0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec13[l12] = 0.0;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec16[l13] = 0.0;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec12[l14] = 0.0;
			
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec11[l15] = 0.0;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fRec0[l16] = 0.0;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec20[l17] = 0.0;
			
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec19[l18] = 0.0;
			
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec18[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec23[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec24[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec22[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec21[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec28[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec29[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec27[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec30[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec26[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec25[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec34[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec35[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec33[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec36[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec32[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec31[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec17[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec40[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec39[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec38[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec44[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec45[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec43[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec46[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec42[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec41[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec50[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec51[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec49[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec52[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec48[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec47[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec37[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec53[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec54[l54] = 0.0;
			
		}
		for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
			fRec55[l55] = 0.0;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fRec60[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec61[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec59[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec62[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec58[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec57[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fRec65[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec66[l63] = 0.0;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec64[l64] = 0.0;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec63[l65] = 0.0;
			
		}
		for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
			fRec56[l66] = 0.0;
			
		}
		for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
			fRec70[l67] = 0.0;
			
		}
		for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
			fRec71[l68] = 0.0;
			
		}
		for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
			fRec69[l69] = 0.0;
			
		}
		for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
			fRec68[l70] = 0.0;
			
		}
		for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
			fRec67[l71] = 0.0;
			
		}
		for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
			fRec72[l72] = 0.0;
			
		}
		for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
			fRec73[l73] = 0.0;
			
		}
		for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
			fRec77[l74] = 0.0;
			
		}
		for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
			fRec78[l75] = 0.0;
			
		}
		for (int l76 = 0; (l76 < 2); l76 = (l76 + 1)) {
			fRec76[l76] = 0.0;
			
		}
		for (int l77 = 0; (l77 < 2); l77 = (l77 + 1)) {
			fRec75[l77] = 0.0;
			
		}
		for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
			fRec74[l78] = 0.0;
			
		}
		for (int l79 = 0; (l79 < 2); l79 = (l79 + 1)) {
			fRec79[l79] = 0.0;
			
		}
		for (int l80 = 0; (l80 < 2); l80 = (l80 + 1)) {
			fRec80[l80] = 0.0;
			
		}
		for (int l81 = 0; (l81 < 2); l81 = (l81 + 1)) {
			fRec81[l81] = 0.0;
			
		}
		for (int l82 = 0; (l82 < 2); l82 = (l82 + 1)) {
			fRec82[l82] = 0.0;
			
		}
		for (int l83 = 0; (l83 < 2); l83 = (l83 + 1)) {
			fRec83[l83] = 0.0;
			
		}
		for (int l84 = 0; (l84 < 2); l84 = (l84 + 1)) {
			fRec84[l84] = 0.0;
			
		}
		for (int l85 = 0; (l85 < 2); l85 = (l85 + 1)) {
			fRec85[l85] = 0.0;
			
		}
		for (int l86 = 0; (l86 < 2); l86 = (l86 + 1)) {
			fRec90[l86] = 0.0;
			
		}
		for (int l87 = 0; (l87 < 2); l87 = (l87 + 1)) {
			fRec91[l87] = 0.0;
			
		}
		for (int l88 = 0; (l88 < 2); l88 = (l88 + 1)) {
			fRec89[l88] = 0.0;
			
		}
		for (int l89 = 0; (l89 < 2); l89 = (l89 + 1)) {
			fRec92[l89] = 0.0;
			
		}
		for (int l90 = 0; (l90 < 2); l90 = (l90 + 1)) {
			fRec88[l90] = 0.0;
			
		}
		for (int l91 = 0; (l91 < 2); l91 = (l91 + 1)) {
			fRec87[l91] = 0.0;
			
		}
		for (int l92 = 0; (l92 < 2); l92 = (l92 + 1)) {
			fRec86[l92] = 0.0;
			
		}
		for (int l93 = 0; (l93 < 2); l93 = (l93 + 1)) {
			fRec93[l93] = 0.0;
			
		}
		for (int l94 = 0; (l94 < 2); l94 = (l94 + 1)) {
			fRec94[l94] = 0.0;
			
		}
		for (int l95 = 0; (l95 < 2); l95 = (l95 + 1)) {
			fRec95[l95] = 0.0;
			
		}
		for (int l96 = 0; (l96 < 2); l96 = (l96 + 1)) {
			fRec96[l96] = 0.0;
			
		}
		for (int l97 = 0; (l97 < 2); l97 = (l97 + 1)) {
			fRec97[l97] = 0.0;
			
		}
		for (int l98 = 0; (l98 < 2); l98 = (l98 + 1)) {
			fRec98[l98] = 0.0;
			
		}
		for (int l99 = 0; (l99 < 2); l99 = (l99 + 1)) {
			fRec99[l99] = 0.0;
			
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
		ui_interface->openVerticalBox("HOADecLebedev263");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("B-Format");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a7fb90", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2afb2c0", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a8aec0", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2abdb10", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b89c50", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b66e60", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a99760", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b4f970", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2acb760", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b0ac20", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bdea90", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b196f0", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2aac240", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2adbf40", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b41db0", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2aec6e0", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(&fVslider1, "1", "");
		ui_interface->declare(&fVslider1, "osc", "/levelin -10 60");
		ui_interface->declare(&fVslider1, "unit", "dB");
		ui_interface->addVerticalSlider("Inputs Gain", &fVslider1, 0.0, -10.0, 60.0, 0.10000000000000001);
		ui_interface->declare(&fVslider0, "2", "");
		ui_interface->declare(&fVslider0, "osc", "/levelout -10 60");
		ui_interface->declare(&fVslider0, "unit", "dB");
		ui_interface->addVerticalSlider("Outputs Gain", &fVslider0, 0.0, -10.0, 60.0, 0.10000000000000001);
		ui_interface->declare(0, "3", "");
		ui_interface->openVerticalBox("NFC");
		ui_interface->addCheckButton("Yes", &fCheckbox0);
		ui_interface->closeBox();
		ui_interface->declare(&fEntry0, "4", "");
		ui_interface->addNumEntry("Speakers Radius", &fEntry0, 1.0700000000000001, 0.5, 10.0, 0.01);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 1-outs/2");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph4, "osc", "/output1 -70 6");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ab12c0", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph9, "osc", "/output2 -70 6");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2af1dd0", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph13, "osc", "/output3 -70 6");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b1e740", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph14, "osc", "/output4 -70 6");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b256c0", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph15, "osc", "/output5 -70 6");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b2b700", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph16, "osc", "/output6 -70 6");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b30ce0", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph19, "osc", "/output7 -70 6");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b58030", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph21, "osc", "/output8 -70 6");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b6c490", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph22, "osc", "/output9 -70 6");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b754e0", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph23, "osc", "/output10 -70 6");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b7c360", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph25, "osc", "/output11 -70 6");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b90a20", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph26, "osc", "/output12 -70 6");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b9ac00", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph27, "osc", "/output13 -70 6");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ba4960", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 14-26");
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph28, "osc", "/output14 -70 6");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bae6f0", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph29, "osc", "/output15 -70 6");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bb8840", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph30, "osc", "/output16 -70 6");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bbfb90", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph31, "osc", "/output17 -70 6");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bc72b0", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph32, "osc", "/output18 -70 6");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bcd5b0", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph34, "osc", "/output19 -70 6");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2be9be0", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph35, "osc", "/output20 -70 6");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bf7200", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph36, "osc", "/output21 -70 6");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c042e0", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph37, "osc", "/output22 -70 6");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c10950", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph38, "osc", "/output23 -70 6");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c1c430", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph39, "osc", "/output24 -70 6");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c26770", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph40, "osc", "/output25 -70 6");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c2f8e0", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph41, "osc", "/output26 -70 6");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c390d0", &fVbargraph41, -70.0, 6.0);
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
		double fSlow0 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider0))));
		double fSlow1 = double(fCheckbox0);
		double fSlow2 = double(fEntry0);
		double fSlow3 = (fSlow1 * fSlow2);
		double fSlow4 = (1.0 - fSlow1);
		double fSlow5 = (fSlow3 + fSlow4);
		double fSlow6 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider1))));
		double fSlow7 = (fConst2 / fSlow2);
		double fSlow8 = (1.0 / (fSlow7 + 1.0));
		double fSlow9 = (fConst3 / fSlow2);
		double fSlow10 = mydsp_faustpower2_f(fSlow7);
		double fSlow11 = (1.0 / (((fConst4 / fSlow2) + (3.0 * fSlow10)) + 1.0));
		double fSlow12 = (12.0 * fSlow10);
		double fSlow13 = (fSlow12 + (fConst5 / fSlow2));
		double fSlow14 = (1.0 / ((fConst6 / fSlow2) + 1.0));
		double fSlow15 = (1.0 / (((fConst7 / fSlow2) + (6.4594326934833797 * fSlow10)) + 1.0));
		double fSlow16 = (25.837730773933519 * fSlow10);
		double fSlow17 = (fSlow16 + (fConst8 / fSlow2));
		double fSlow18 = (fConst9 / fSlow2);
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			fRec3[0] = (fSlow6 + (0.999 * fRec3[1]));
			double fTemp0 = (fSlow5 * (fRec3[0] * double(input0[i])));
			fRec2[0] = max((fRec2[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fVbargraph0 = FAUSTFLOAT(fRec2[0]);
			double fTemp1 = (0.047619047619047623 * fTemp0);
			double fTemp2 = (fRec3[0] * double(input2[i]));
			fRec6[0] = (fRec6[1] + fRec5[1]);
			fRec5[0] = (fSlow8 * ((fSlow3 * fTemp2) - (fSlow9 * fRec6[0])));
			double fTemp3 = (fRec5[0] + (fSlow4 * fTemp2));
			fRec4[0] = max((fRec4[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp3))))));
			fVbargraph1 = FAUSTFLOAT(fRec4[0]);
			double fTemp4 = (0.082478609884232251 * fTemp3);
			double fTemp5 = (fRec3[0] * double(input6[i]));
			fRec9[0] = (fRec9[1] + fRec8[1]);
			fRec10[0] = (fRec10[1] + fRec9[1]);
			fRec8[0] = (fSlow11 * ((fSlow3 * fTemp5) - ((fSlow13 * fRec9[0]) + (fSlow12 * fRec10[0]))));
			double fTemp6 = (fRec8[0] + (fSlow4 * fTemp5));
			fRec7[0] = max((fRec7[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fVbargraph2 = FAUSTFLOAT(fRec7[0]);
			double fTemp7 = (0.10647942749999 * fTemp6);
			double fTemp8 = (fRec3[0] * double(input12[i]));
			fRec14[0] = (fRec14[1] + fRec13[1]);
			fRec15[0] = (fRec15[1] + fRec14[1]);
			fRec13[0] = (fSlow15 * ((fSlow3 * fTemp8) - ((fSlow17 * fRec14[0]) + (fSlow16 * fRec15[0]))));
			fRec16[0] = (fRec16[1] + fRec12[1]);
			fRec12[0] = (fSlow14 * (fRec13[0] - (fSlow18 * fRec16[0])));
			double fTemp9 = (fRec12[0] + (fSlow4 * fTemp8));
			fRec11[0] = max((fRec11[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph3 = FAUSTFLOAT(fRec11[0]);
			double fTemp10 = (0.12598815766974242 * fTemp9);
			double fTemp11 = (fRec1[0] * (((fTemp1 + fTemp4) + fTemp7) + fTemp10));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fVbargraph4 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp11);
			double fTemp12 = (fRec3[0] * double(input3[i]));
			fRec20[0] = (fRec20[1] + fRec19[1]);
			fRec19[0] = (fSlow8 * ((fSlow3 * fTemp12) - (fSlow9 * fRec20[0])));
			double fTemp13 = (fRec19[0] + (fSlow4 * fTemp12));
			fRec18[0] = max((fRec18[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fVbargraph5 = FAUSTFLOAT(fRec18[0]);
			double fTemp14 = (fRec3[0] * double(input8[i]));
			fRec23[0] = (fRec23[1] + fRec22[1]);
			fRec24[0] = (fRec24[1] + fRec23[1]);
			fRec22[0] = (fSlow11 * ((fSlow3 * fTemp14) - ((fSlow13 * fRec23[0]) + (fSlow12 * fRec24[0]))));
			double fTemp15 = (fRec22[0] + (fSlow4 * fTemp14));
			fRec21[0] = max((fRec21[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph6 = FAUSTFLOAT(fRec21[0]);
			double fTemp16 = (fConst20 * fTemp15);
			double fTemp17 = (fRec3[0] * double(input13[i]));
			fRec28[0] = (fRec28[1] + fRec27[1]);
			fRec29[0] = (fRec29[1] + fRec28[1]);
			fRec27[0] = (fSlow15 * ((fSlow3 * fTemp17) - ((fSlow17 * fRec28[0]) + (fSlow16 * fRec29[0]))));
			fRec30[0] = (fRec30[1] + fRec26[1]);
			fRec26[0] = (fSlow14 * (fRec27[0] - (fSlow18 * fRec30[0])));
			double fTemp18 = (fRec26[0] + (fSlow4 * fTemp17));
			fRec25[0] = max((fRec25[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph7 = FAUSTFLOAT(fRec25[0]);
			double fTemp19 = (fRec3[0] * double(input15[i]));
			fRec34[0] = (fRec34[1] + fRec33[1]);
			fRec35[0] = (fRec35[1] + fRec34[1]);
			fRec33[0] = (fSlow15 * ((fSlow3 * fTemp19) - ((fSlow17 * fRec34[0]) + (fSlow16 * fRec35[0]))));
			fRec36[0] = (fRec36[1] + fRec32[1]);
			fRec32[0] = (fSlow14 * (fRec33[0] - (fSlow18 * fRec36[0])));
			double fTemp20 = (fRec32[0] + (fSlow4 * fTemp19));
			fRec31[0] = max((fRec31[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph8 = FAUSTFLOAT(fRec31[0]);
			double fTemp21 = (0.053239713749994998 * fTemp6);
			double fTemp22 = (fRec1[0] * (((((fTemp1 + (fConst15 * fTemp13)) + fTemp16) + (fConst23 * fTemp18)) + (fConst25 * fTemp20)) - fTemp21));
			fRec17[0] = max((fRec17[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph9 = FAUSTFLOAT(fRec17[0]);
			output1[i] = FAUSTFLOAT(fTemp22);
			double fTemp23 = (fRec3[0] * double(input1[i]));
			fRec40[0] = (fRec40[1] + fRec39[1]);
			fRec39[0] = (fSlow8 * ((fSlow3 * fTemp23) - (fSlow9 * fRec40[0])));
			double fTemp24 = (fRec39[0] + (fSlow4 * fTemp23));
			fRec38[0] = max((fRec38[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph10 = FAUSTFLOAT(fRec38[0]);
			double fTemp25 = (fConst26 * fTemp15);
			double fTemp26 = (fRec3[0] * double(input9[i]));
			fRec44[0] = (fRec44[1] + fRec43[1]);
			fRec45[0] = (fRec45[1] + fRec44[1]);
			fRec43[0] = (fSlow15 * ((fSlow3 * fTemp26) - ((fSlow17 * fRec44[0]) + (fSlow16 * fRec45[0]))));
			fRec46[0] = (fRec46[1] + fRec42[1]);
			fRec42[0] = (fSlow14 * (fRec43[0] - (fSlow18 * fRec46[0])));
			double fTemp27 = (fRec42[0] + (fSlow4 * fTemp26));
			fRec41[0] = max((fRec41[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph11 = FAUSTFLOAT(fRec41[0]);
			double fTemp28 = (fRec3[0] * double(input11[i]));
			fRec50[0] = (fRec50[1] + fRec49[1]);
			fRec51[0] = (fRec51[1] + fRec50[1]);
			fRec49[0] = (fSlow15 * ((fSlow3 * fTemp28) - ((fSlow17 * fRec50[0]) + (fSlow16 * fRec51[0]))));
			fRec52[0] = (fRec52[1] + fRec48[1]);
			fRec48[0] = (fSlow14 * (fRec49[0] - (fSlow18 * fRec52[0])));
			double fTemp29 = (fRec48[0] + (fSlow4 * fTemp28));
			fRec47[0] = max((fRec47[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph12 = FAUSTFLOAT(fRec47[0]);
			double fTemp30 = (fRec1[0] * ((((fConst15 * fTemp24) + (0.047619047619047623 * ((fTemp0 + fTemp25) + (fConst27 * fTemp27)))) + (fConst23 * fTemp29)) - fTemp21));
			fRec37[0] = max((fRec37[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph13 = FAUSTFLOAT(fRec37[0]);
			output2[i] = FAUSTFLOAT(fTemp30);
			double fTemp31 = (fRec1[0] * ((fTemp16 + (0.047619047619047623 * (((fTemp0 + (fConst28 * fTemp13)) + (fConst29 * fTemp18)) + (fConst27 * fTemp20)))) - fTemp21));
			fRec53[0] = max((fRec53[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph14 = FAUSTFLOAT(fRec53[0]);
			output3[i] = FAUSTFLOAT(fTemp31);
			double fTemp32 = (fRec1[0] * (((fConst25 * fTemp27) + (0.047619047619047623 * ((fTemp25 + (fTemp0 + (fConst28 * fTemp24))) + (fConst29 * fTemp29)))) - fTemp21));
			fRec54[0] = max((fRec54[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph15 = FAUSTFLOAT(fRec54[0]);
			output4[i] = FAUSTFLOAT(fTemp32);
			double fTemp33 = (fRec1[0] * ((fTemp1 + fTemp7) - (fTemp4 + fTemp10)));
			fRec55[0] = max((fRec55[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph16 = FAUSTFLOAT(fRec55[0]);
			output5[i] = FAUSTFLOAT(fTemp33);
			double fTemp34 = (fRec3[0] * double(input14[i]));
			fRec60[0] = (fRec60[1] + fRec59[1]);
			fRec61[0] = (fRec61[1] + fRec60[1]);
			fRec59[0] = (fSlow15 * ((fSlow3 * fTemp34) - ((fSlow17 * fRec60[0]) + (fSlow16 * fRec61[0]))));
			fRec62[0] = (fRec62[1] + fRec58[1]);
			fRec58[0] = (fSlow14 * (fRec59[0] - (fSlow18 * fRec62[0])));
			double fTemp35 = (fRec58[0] + (fSlow4 * fTemp34));
			fRec57[0] = max((fRec57[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph17 = FAUSTFLOAT(fRec57[0]);
			double fTemp36 = (fConst16 * ((fConst30 * fTemp15) + (fConst33 * fTemp35)));
			double fTemp37 = (fRec3[0] * double(input7[i]));
			fRec65[0] = (fRec65[1] + fRec64[1]);
			fRec66[0] = (fRec66[1] + fRec65[1]);
			fRec64[0] = (fSlow11 * ((fSlow3 * fTemp37) - ((fSlow13 * fRec65[0]) + (fSlow12 * fRec66[0]))));
			double fTemp38 = (fRec64[0] + (fSlow4 * fTemp37));
			fRec63[0] = max((fRec63[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fVbargraph18 = FAUSTFLOAT(fRec63[0]);
			double fTemp39 = (0.038095238095238099 * fTemp0);
			double fTemp40 = (0.046656947481584346 * fTemp3);
			double fTemp41 = (0.021295885499997991 * fTemp6);
			double fTemp42 = (fConst38 * fTemp20);
			double fTemp43 = (0.017817416127494972 * fTemp9);
			double fTemp44 = (fRec1[0] * ((fTemp36 + ((fConst10 * (((fConst34 * fTemp13) + (fConst36 * fTemp38)) + (fConst37 * fTemp18))) + (((fTemp39 + fTemp40) + fTemp41) + fTemp42))) - fTemp43));
			fRec56[0] = max((fRec56[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fVbargraph19 = FAUSTFLOAT(fRec56[0]);
			output6[i] = FAUSTFLOAT(fTemp44);
			double fTemp45 = (fConst39 * fTemp15);
			double fTemp46 = ((fTemp0 + fTemp45) + (fConst40 * fTemp27));
			double fTemp47 = (fConst42 * fTemp35);
			double fTemp48 = (fTemp40 + fTemp41);
			double fTemp49 = (fRec3[0] * double(input5[i]));
			fRec70[0] = (fRec70[1] + fRec69[1]);
			fRec71[0] = (fRec71[1] + fRec70[1]);
			fRec69[0] = (fSlow11 * ((fSlow3 * fTemp49) - ((fSlow13 * fRec70[0]) + (fSlow12 * fRec71[0]))));
			double fTemp50 = (fRec69[0] + (fSlow4 * fTemp49));
			fRec68[0] = max((fRec68[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph20 = FAUSTFLOAT(fRec68[0]);
			double fTemp51 = (fRec1[0] * (((0.038095238095238099 * (fTemp46 + fTemp47)) + (fTemp48 + (fConst10 * (((fConst34 * fTemp24) + (fConst36 * fTemp50)) + (fConst37 * fTemp29))))) - fTemp43));
			fRec67[0] = max((fRec67[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph21 = FAUSTFLOAT(fRec67[0]);
			output7[i] = FAUSTFLOAT(fTemp51);
			double fTemp52 = (fTemp0 + (fConst43 * fTemp13));
			double fTemp53 = (fConst40 * fTemp20);
			double fTemp54 = (fRec1[0] * (((fTemp36 + fTemp48) + (0.038095238095238099 * (((fTemp52 + (fConst45 * fTemp38)) + (fConst47 * fTemp18)) + fTemp53))) - fTemp43));
			fRec72[0] = max((fRec72[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fVbargraph22 = FAUSTFLOAT(fRec72[0]);
			output8[i] = FAUSTFLOAT(fTemp54);
			double fTemp55 = (fConst38 * fTemp27);
			double fTemp56 = (fTemp0 + (fConst43 * fTemp24));
			double fTemp57 = (fRec1[0] * (((fTemp48 + fTemp55) + (0.038095238095238099 * (fTemp47 + ((fTemp45 + (fTemp56 + (fConst45 * fTemp50))) + (fConst47 * fTemp29))))) - fTemp43));
			fRec73[0] = max((fRec73[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fVbargraph23 = FAUSTFLOAT(fRec73[0]);
			output9[i] = FAUSTFLOAT(fTemp57);
			double fTemp58 = (fRec3[0] * double(input4[i]));
			fRec77[0] = (fRec77[1] + fRec76[1]);
			fRec78[0] = (fRec78[1] + fRec77[1]);
			fRec76[0] = (fSlow11 * ((fSlow3 * fTemp58) - ((fSlow13 * fRec77[0]) + (fSlow12 * fRec78[0]))));
			double fTemp59 = (fRec76[0] + (fSlow4 * fTemp58));
			fRec75[0] = max((fRec75[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fVbargraph24 = FAUSTFLOAT(fRec75[0]);
			double fTemp60 = (0.042591770999995997 * fTemp6);
			double fTemp61 = (fRec1[0] * (((0.038095238095238099 * (fTemp0 + (fConst48 * fTemp20))) + ((fConst22 * ((0.019047619047619053 * fTemp18) + (0.019047619047619049 * fTemp29))) + ((fConst19 * ((2.6996718031124477e-17 * fTemp15) + (0.12158240926519855 * fTemp59))) + ((fConst14 * ((0.042985873032210013 * fTemp13) + (0.042985873032210006 * fTemp24))) + (fConst49 * fTemp27))))) - fTemp60));
			fRec74[0] = max((fRec74[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fVbargraph25 = FAUSTFLOAT(fRec74[0]);
			output10[i] = FAUSTFLOAT(fTemp61);
			double fTemp62 = (fConst50 * fTemp24);
			double fTemp63 = (fConst26 * fTemp59);
			double fTemp64 = (fRec1[0] * (((fConst24 * ((0.17194349212884008 * fTemp20) + (0.17194349212883997 * fTemp27))) + ((fTemp62 + (fConst51 * fTemp29)) + (0.038095238095238099 * ((((fTemp0 + (fConst52 * fTemp13)) + fTemp63) + (fConst53 * fTemp15)) + (fConst54 * fTemp18))))) - fTemp60));
			fRec79[0] = max((fRec79[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fVbargraph26 = FAUSTFLOAT(fRec79[0]);
			output11[i] = FAUSTFLOAT(fTemp64);
			double fTemp65 = (fRec1[0] * (((0.038095238095238099 * (((((fTemp0 + (fConst52 * fTemp24)) + (fConst55 * fTemp13)) + (fConst56 * fTemp27)) + (fConst54 * fTemp29)) + (fConst57 * fTemp18))) + ((fConst19 * ((5.3993436062248954e-17 * fTemp15) + (0.12158240926519859 * fTemp59))) + (fConst58 * fTemp20))) - fTemp60));
			fRec80[0] = max((fRec80[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fVbargraph27 = FAUSTFLOAT(fRec80[0]);
			output12[i] = FAUSTFLOAT(fTemp65);
			double fTemp66 = (fRec1[0] * ((((fConst59 * fTemp13) + (fConst60 * fTemp18)) + (0.038095238095238099 * (((((fTemp63 + (fTemp0 + (fConst55 * fTemp24))) + (fConst61 * fTemp15)) + (fConst62 * fTemp27)) + (fConst57 * fTemp29)) + (fConst63 * fTemp20)))) - fTemp60));
			fRec81[0] = max((fRec81[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fVbargraph28 = FAUSTFLOAT(fRec81[0]);
			output13[i] = FAUSTFLOAT(fTemp66);
			double fTemp67 = (fConst69 * fTemp15);
			double fTemp68 = (fRec1[0] * (((0.053874802376117914 * ((fConst65 * fTemp38) + (fConst66 * fTemp35))) + ((fConst68 * fTemp18) + (fTemp42 + (fTemp67 + ((fConst50 * fTemp13) + (fTemp43 + (fTemp39 + fTemp41))))))) - fTemp40));
			fRec82[0] = max((fRec82[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fVbargraph29 = FAUSTFLOAT(fRec82[0]);
			output14[i] = FAUSTFLOAT(fTemp68);
			double fTemp69 = (fConst70 * fTemp35);
			double fTemp70 = (fTemp41 + fTemp43);
			double fTemp71 = (fRec1[0] * (((0.038095238095238099 * (fTemp46 + fTemp69)) + ((fConst68 * fTemp29) + ((fConst71 * fTemp50) + (fTemp62 + fTemp70)))) - fTemp40));
			fRec83[0] = max((fRec83[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fVbargraph30 = FAUSTFLOAT(fRec83[0]);
			output15[i] = FAUSTFLOAT(fTemp71);
			double fTemp72 = (fRec1[0] * ((((fConst72 * fTemp35) + (fTemp67 + fTemp70)) + (0.038095238095238099 * (fTemp53 + ((fTemp52 + (fConst73 * fTemp38)) + (fConst74 * fTemp18))))) - fTemp40));
			fRec84[0] = max((fRec84[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp72))))));
			fVbargraph31 = FAUSTFLOAT(fRec84[0]);
			output16[i] = FAUSTFLOAT(fTemp72);
			double fTemp73 = (fRec1[0] * (((fTemp55 + fTemp70) + (0.038095238095238099 * (fTemp69 + ((fTemp45 + (fTemp56 + (fConst73 * fTemp50))) + (fConst74 * fTemp29))))) - fTemp40));
			fRec85[0] = max((fRec85[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp73))))));
			fVbargraph32 = FAUSTFLOAT(fRec85[0]);
			output17[i] = FAUSTFLOAT(fTemp73);
			double fTemp74 = (fTemp0 + fTemp3);
			double fTemp75 = (fConst75 * fTemp20);
			double fTemp76 = ((1.518565389250751e-17 * fTemp15) + (0.068390105211674143 * fTemp59));
			double fTemp77 = (fRec3[0] * double(input10[i]));
			fRec90[0] = (fRec90[1] + fRec89[1]);
			fRec91[0] = (fRec91[1] + fRec90[1]);
			fRec89[0] = (fSlow15 * ((fSlow3 * fTemp77) - ((fSlow17 * fRec90[0]) + (fSlow16 * fRec91[0]))));
			fRec92[0] = (fRec92[1] + fRec88[1]);
			fRec88[0] = (fSlow14 * (fRec89[0] - (fSlow18 * fRec92[0])));
			double fTemp78 = (fRec88[0] + (fSlow4 * fTemp77));
			fRec87[0] = max((fRec87[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fVbargraph33 = FAUSTFLOAT(fRec87[0]);
			double fTemp79 = (7.9795740640090912e-18 * fTemp6);
			double fTemp80 = (fConst78 * fTemp27);
			double fTemp81 = ((0.02961378424040018 * fTemp13) + (0.029613784240400176 * fTemp24));
			double fTemp82 = (0.032732683535398849 * fTemp9);
			double fTemp83 = (fRec1[0] * (((0.03214285714285714 * (fTemp74 + fTemp75)) + ((fConst16 * (((fConst18 * fTemp76) + (fConst76 * fTemp78)) + (fConst77 * fTemp35))) + ((fTemp79 + fTemp80) + (fConst10 * (((((fConst13 * fTemp81) + (fConst79 * fTemp50)) + (fConst80 * fTemp38)) + (fConst81 * fTemp29)) + (fConst82 * fTemp18)))))) - fTemp82));
			fRec86[0] = max((fRec86[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp83))))));
			fVbargraph34 = FAUSTFLOAT(fRec86[0]);
			output18[i] = FAUSTFLOAT(fTemp83);
			double fTemp84 = (fConst24 * ((0.078970091307733831 * fTemp20) + (0.078970091307733761 * fTemp27)));
			double fTemp85 = (fConst83 * fTemp13);
			double fTemp86 = (fConst84 * fTemp59);
			double fTemp87 = (fConst86 * fTemp15);
			double fTemp88 = (fConst87 * fTemp78);
			double fTemp89 = (fRec1[0] * ((fTemp84 + ((0.03214285714285714 * (((((((fTemp74 + fTemp85) + fTemp86) + (fConst85 * fTemp38)) + fTemp87) + fTemp88) + (fConst88 * fTemp18)) + (fConst89 * fTemp35))) + (fTemp79 + (fConst10 * (((fConst90 * fTemp24) + (fConst80 * fTemp50)) + (fConst82 * fTemp29)))))) - fTemp82));
			fRec93[0] = max((fRec93[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp89))))));
			fVbargraph35 = FAUSTFLOAT(fRec93[0]);
			output19[i] = FAUSTFLOAT(fTemp89);
			double fTemp90 = ((3.037130778501502e-17 * fTemp15) + (0.068390105211674157 * fTemp59));
			double fTemp91 = (fTemp0 + (fConst83 * fTemp24));
			double fTemp92 = (fConst93 * fTemp13);
			double fTemp93 = (fConst95 * fTemp27);
			double fTemp94 = (fConst97 * fTemp20);
			double fTemp95 = (fRec1[0] * (((fConst16 * (((fConst18 * fTemp90) + (fConst91 * fTemp78)) + (fConst92 * fTemp35))) + ((0.03214285714285714 * (((((((fTemp3 + fTemp91) + fTemp92) + (fConst85 * fTemp50)) + (fConst94 * fTemp38)) + fTemp93) + (fConst88 * fTemp29)) + (fConst96 * fTemp18))) + (fTemp79 + fTemp94))) - fTemp82));
			fRec94[0] = max((fRec94[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp95))))));
			fVbargraph36 = FAUSTFLOAT(fRec94[0]);
			output20[i] = FAUSTFLOAT(fTemp95);
			double fTemp96 = (fTemp0 + (fConst93 * fTemp24));
			double fTemp97 = (fConst101 * fTemp15);
			double fTemp98 = (fConst102 * fTemp27);
			double fTemp99 = (fConst104 * fTemp20);
			double fTemp100 = (fRec1[0] * (((fTemp79 + (fConst10 * (((fConst98 * fTemp13) + (fConst99 * fTemp38)) + (fConst100 * fTemp18)))) + (0.03214285714285714 * ((((fTemp88 + ((((fTemp86 + (fTemp3 + fTemp96)) + (fConst94 * fTemp50)) + fTemp97) + fTemp98)) + (fConst96 * fTemp29)) + (fConst103 * fTemp35)) + fTemp99))) - fTemp82));
			fRec95[0] = max((fRec95[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp100))))));
			fVbargraph37 = FAUSTFLOAT(fRec95[0]);
			output21[i] = FAUSTFLOAT(fTemp100);
			double fTemp101 = (fTemp0 - fTemp3);
			double fTemp102 = (fTemp79 + fTemp82);
			double fTemp103 = (fRec1[0] * ((0.03214285714285714 * (fTemp75 + (fTemp101 + (fConst106 * fTemp50)))) + ((fConst107 * ((1.0093451520109982e-17 * fTemp35) + (0.045456864504849481 * fTemp78))) + ((fConst108 * ((0.016071428571428573 * fTemp18) + (0.01607142857142857 * fTemp29))) + ((fConst109 * fTemp38) + ((fConst19 * fTemp76) + ((fConst14 * fTemp81) + (fTemp80 + fTemp102))))))));
			fRec96[0] = max((fRec96[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp103))))));
			fVbargraph38 = FAUSTFLOAT(fRec96[0]);
			output22[i] = FAUSTFLOAT(fTemp103);
			double fTemp104 = (fConst111 * fTemp78);
			double fTemp105 = (fRec1[0] * ((0.03214285714285714 * ((((fTemp87 + ((fTemp86 + (fTemp85 + fTemp101)) + (fConst110 * fTemp38))) + fTemp104) + (fConst112 * fTemp18)) + (fConst113 * fTemp35))) + ((fConst114 * fTemp29) + ((fConst109 * fTemp50) + (fTemp84 + ((fConst115 * fTemp24) + fTemp102))))));
			fRec97[0] = max((fRec97[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp105))))));
			fVbargraph39 = FAUSTFLOAT(fRec97[0]);
			output23[i] = FAUSTFLOAT(fTemp105);
			double fTemp106 = (fRec1[0] * ((fConst107 * ((2.0186903040219964e-17 * fTemp35) + (0.045456864504849488 * fTemp78))) + ((0.03214285714285714 * (((fTemp93 + (((fTemp92 - (fTemp3 - fTemp91)) + (fConst110 * fTemp50)) + (fConst116 * fTemp38))) + (fConst112 * fTemp29)) + (fConst117 * fTemp18))) + ((fConst19 * fTemp90) + (fTemp94 + fTemp102)))));
			fRec98[0] = max((fRec98[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp106))))));
			fVbargraph40 = FAUSTFLOAT(fRec98[0]);
			output24[i] = FAUSTFLOAT(fTemp106);
			double fTemp107 = (fRec1[0] * ((((fConst118 * fTemp38) + ((fConst119 * fTemp13) + fTemp102)) + (fConst120 * fTemp18)) + (0.03214285714285714 * (fTemp99 + (((fTemp104 + (fTemp98 + (fTemp97 + ((fTemp86 - (fTemp3 - fTemp96)) + (fConst116 * fTemp50))))) + (fConst117 * fTemp29)) + (fConst121 * fTemp35))))));
			fRec99[0] = max((fRec99[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp107))))));
			fVbargraph41 = FAUSTFLOAT(fRec99[0]);
			output25[i] = FAUSTFLOAT(fTemp107);
			fRec1[1] = fRec1[0];
			fRec3[1] = fRec3[0];
			fRec2[1] = fRec2[0];
			fRec6[1] = fRec6[0];
			fRec5[1] = fRec5[0];
			fRec4[1] = fRec4[0];
			fRec9[1] = fRec9[0];
			fRec10[1] = fRec10[0];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
			fRec14[1] = fRec14[0];
			fRec15[1] = fRec15[0];
			fRec13[1] = fRec13[0];
			fRec16[1] = fRec16[0];
			fRec12[1] = fRec12[0];
			fRec11[1] = fRec11[0];
			fRec0[1] = fRec0[0];
			fRec20[1] = fRec20[0];
			fRec19[1] = fRec19[0];
			fRec18[1] = fRec18[0];
			fRec23[1] = fRec23[0];
			fRec24[1] = fRec24[0];
			fRec22[1] = fRec22[0];
			fRec21[1] = fRec21[0];
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec27[1] = fRec27[0];
			fRec30[1] = fRec30[0];
			fRec26[1] = fRec26[0];
			fRec25[1] = fRec25[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			fRec33[1] = fRec33[0];
			fRec36[1] = fRec36[0];
			fRec32[1] = fRec32[0];
			fRec31[1] = fRec31[0];
			fRec17[1] = fRec17[0];
			fRec40[1] = fRec40[0];
			fRec39[1] = fRec39[0];
			fRec38[1] = fRec38[0];
			fRec44[1] = fRec44[0];
			fRec45[1] = fRec45[0];
			fRec43[1] = fRec43[0];
			fRec46[1] = fRec46[0];
			fRec42[1] = fRec42[0];
			fRec41[1] = fRec41[0];
			fRec50[1] = fRec50[0];
			fRec51[1] = fRec51[0];
			fRec49[1] = fRec49[0];
			fRec52[1] = fRec52[0];
			fRec48[1] = fRec48[0];
			fRec47[1] = fRec47[0];
			fRec37[1] = fRec37[0];
			fRec53[1] = fRec53[0];
			fRec54[1] = fRec54[0];
			fRec55[1] = fRec55[0];
			fRec60[1] = fRec60[0];
			fRec61[1] = fRec61[0];
			fRec59[1] = fRec59[0];
			fRec62[1] = fRec62[0];
			fRec58[1] = fRec58[0];
			fRec57[1] = fRec57[0];
			fRec65[1] = fRec65[0];
			fRec66[1] = fRec66[0];
			fRec64[1] = fRec64[0];
			fRec63[1] = fRec63[0];
			fRec56[1] = fRec56[0];
			fRec70[1] = fRec70[0];
			fRec71[1] = fRec71[0];
			fRec69[1] = fRec69[0];
			fRec68[1] = fRec68[0];
			fRec67[1] = fRec67[0];
			fRec72[1] = fRec72[0];
			fRec73[1] = fRec73[0];
			fRec77[1] = fRec77[0];
			fRec78[1] = fRec78[0];
			fRec76[1] = fRec76[0];
			fRec75[1] = fRec75[0];
			fRec74[1] = fRec74[0];
			fRec79[1] = fRec79[0];
			fRec80[1] = fRec80[0];
			fRec81[1] = fRec81[0];
			fRec82[1] = fRec82[0];
			fRec83[1] = fRec83[0];
			fRec84[1] = fRec84[0];
			fRec85[1] = fRec85[0];
			fRec90[1] = fRec90[0];
			fRec91[1] = fRec91[0];
			fRec89[1] = fRec89[0];
			fRec92[1] = fRec92[0];
			fRec88[1] = fRec88[0];
			fRec87[1] = fRec87[0];
			fRec86[1] = fRec86[0];
			fRec93[1] = fRec93[0];
			fRec94[1] = fRec94[0];
			fRec95[1] = fRec95[0];
			fRec96[1] = fRec96[0];
			fRec97[1] = fRec97[0];
			fRec98[1] = fRec98[0];
			fRec99[1] = fRec99[0];
			
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
