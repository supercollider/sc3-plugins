/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOADecLebedev504"
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
	double fConst10;
	double fConst11;
	double fConst12;
	double fRec20[2];
	double fRec21[2];
	double fRec19[2];
	double fConst13;
	double fRec22[2];
	double fRec23[2];
	double fRec18[2];
	double fRec17[2];
	FAUSTFLOAT fVbargraph4;
	double fRec0[2];
	FAUSTFLOAT fVbargraph5;
	double fConst14;
	double fConst15;
	double fConst16;
	double fConst17;
	double fConst18;
	double fRec28[2];
	double fRec29[2];
	double fRec27[2];
	double fRec30[2];
	double fRec26[2];
	double fRec25[2];
	FAUSTFLOAT fVbargraph6;
	double fConst19;
	double fConst20;
	double fConst21;
	double fConst22;
	double fRec34[2];
	double fRec35[2];
	double fRec33[2];
	double fRec36[2];
	double fRec37[2];
	double fRec32[2];
	double fRec31[2];
	FAUSTFLOAT fVbargraph7;
	double fConst23;
	double fConst24;
	double fConst25;
	double fConst26;
	double fConst27;
	double fRec41[2];
	double fRec42[2];
	double fRec40[2];
	double fRec43[2];
	double fRec39[2];
	double fRec38[2];
	FAUSTFLOAT fVbargraph8;
	double fConst28;
	double fConst29;
	double fConst30;
	double fRec46[2];
	double fRec47[2];
	double fRec45[2];
	double fRec44[2];
	FAUSTFLOAT fVbargraph9;
	double fConst31;
	double fConst32;
	double fConst33;
	double fRec50[2];
	double fRec49[2];
	double fRec48[2];
	FAUSTFLOAT fVbargraph10;
	double fConst34;
	double fConst35;
	double fRec54[2];
	double fRec55[2];
	double fRec53[2];
	double fRec56[2];
	double fRec57[2];
	double fRec52[2];
	double fRec51[2];
	FAUSTFLOAT fVbargraph11;
	double fRec24[2];
	FAUSTFLOAT fVbargraph12;
	double fConst36;
	double fConst37;
	double fRec62[2];
	double fRec63[2];
	double fRec61[2];
	double fRec64[2];
	double fRec60[2];
	double fRec59[2];
	FAUSTFLOAT fVbargraph13;
	double fConst38;
	double fConst39;
	double fRec68[2];
	double fRec69[2];
	double fRec67[2];
	double fRec70[2];
	double fRec66[2];
	double fRec65[2];
	FAUSTFLOAT fVbargraph14;
	double fRec73[2];
	double fRec72[2];
	double fRec71[2];
	FAUSTFLOAT fVbargraph15;
	double fRec58[2];
	FAUSTFLOAT fVbargraph16;
	double fConst40;
	double fConst41;
	double fConst42;
	double fRec74[2];
	FAUSTFLOAT fVbargraph17;
	double fRec75[2];
	FAUSTFLOAT fVbargraph18;
	double fRec76[2];
	FAUSTFLOAT fVbargraph19;
	double fConst43;
	double fConst44;
	double fConst45;
	double fConst46;
	double fRec81[2];
	double fRec82[2];
	double fRec80[2];
	double fRec83[2];
	double fRec84[2];
	double fRec79[2];
	double fRec78[2];
	FAUSTFLOAT fVbargraph20;
	double fConst47;
	double fConst48;
	double fConst49;
	double fConst50;
	double fRec88[2];
	double fRec89[2];
	double fRec87[2];
	double fRec90[2];
	double fRec86[2];
	double fRec85[2];
	FAUSTFLOAT fVbargraph21;
	double fConst51;
	double fConst52;
	double fConst53;
	double fConst54;
	double fConst55;
	double fRec93[2];
	double fRec94[2];
	double fRec92[2];
	double fRec91[2];
	FAUSTFLOAT fVbargraph22;
	double fConst56;
	double fConst57;
	double fConst58;
	double fRec98[2];
	double fRec99[2];
	double fRec97[2];
	double fRec100[2];
	double fRec101[2];
	double fRec96[2];
	double fRec95[2];
	FAUSTFLOAT fVbargraph23;
	double fConst59;
	double fRec77[2];
	FAUSTFLOAT fVbargraph24;
	double fConst60;
	double fConst61;
	double fConst62;
	double fConst63;
	double fConst64;
	double fConst65;
	double fRec106[2];
	double fRec107[2];
	double fRec105[2];
	double fRec108[2];
	double fRec109[2];
	double fRec104[2];
	double fRec103[2];
	FAUSTFLOAT fVbargraph25;
	double fConst66;
	double fConst67;
	double fRec112[2];
	double fRec113[2];
	double fRec111[2];
	double fRec110[2];
	FAUSTFLOAT fVbargraph26;
	double fRec117[2];
	double fRec118[2];
	double fRec116[2];
	double fRec119[2];
	double fRec120[2];
	double fRec115[2];
	double fRec114[2];
	FAUSTFLOAT fVbargraph27;
	double fRec102[2];
	FAUSTFLOAT fVbargraph28;
	double fConst68;
	double fConst69;
	double fConst70;
	double fConst71;
	double fConst72;
	double fConst73;
	double fConst74;
	double fRec121[2];
	FAUSTFLOAT fVbargraph29;
	double fRec122[2];
	FAUSTFLOAT fVbargraph30;
	double fConst75;
	double fConst76;
	double fRec127[2];
	double fRec128[2];
	double fRec126[2];
	double fRec129[2];
	double fRec130[2];
	double fRec125[2];
	double fRec124[2];
	FAUSTFLOAT fVbargraph31;
	double fConst77;
	double fRec134[2];
	double fRec135[2];
	double fRec133[2];
	double fRec136[2];
	double fRec137[2];
	double fRec132[2];
	double fRec131[2];
	FAUSTFLOAT fVbargraph32;
	double fRec140[2];
	double fRec141[2];
	double fRec139[2];
	double fRec138[2];
	FAUSTFLOAT fVbargraph33;
	double fConst78;
	double fRec123[2];
	FAUSTFLOAT fVbargraph34;
	double fConst79;
	double fConst80;
	double fConst81;
	double fConst82;
	double fConst83;
	double fConst84;
	double fConst85;
	double fConst86;
	double fRec142[2];
	FAUSTFLOAT fVbargraph35;
	double fConst87;
	double fConst88;
	double fConst89;
	double fConst90;
	double fConst91;
	double fRec143[2];
	FAUSTFLOAT fVbargraph36;
	double fConst92;
	double fConst93;
	double fConst94;
	double fConst95;
	double fConst96;
	double fConst97;
	double fConst98;
	double fRec144[2];
	FAUSTFLOAT fVbargraph37;
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
	double fRec145[2];
	FAUSTFLOAT fVbargraph38;
	double fConst111;
	double fConst112;
	double fConst113;
	double fConst114;
	double fConst115;
	double fRec146[2];
	FAUSTFLOAT fVbargraph39;
	double fConst116;
	double fConst117;
	double fConst118;
	double fConst119;
	double fConst120;
	double fRec147[2];
	FAUSTFLOAT fVbargraph40;
	double fConst121;
	double fRec148[2];
	FAUSTFLOAT fVbargraph41;
	double fConst122;
	double fRec153[2];
	double fRec154[2];
	double fRec152[2];
	double fRec155[2];
	double fRec151[2];
	double fRec150[2];
	FAUSTFLOAT fVbargraph42;
	double fConst123;
	double fConst124;
	double fConst125;
	double fConst126;
	double fConst127;
	double fConst128;
	double fConst129;
	double fConst130;
	double fConst131;
	double fConst132;
	double fConst133;
	double fConst134;
	double fConst135;
	double fRec149[2];
	FAUSTFLOAT fVbargraph43;
	double fConst136;
	double fConst137;
	double fConst138;
	double fConst139;
	double fConst140;
	double fConst141;
	double fConst142;
	double fConst143;
	double fConst144;
	double fConst145;
	double fConst146;
	double fConst147;
	double fConst148;
	double fConst149;
	double fConst150;
	double fRec156[2];
	FAUSTFLOAT fVbargraph44;
	double fConst151;
	double fConst152;
	double fConst153;
	double fConst154;
	double fConst155;
	double fConst156;
	double fConst157;
	double fConst158;
	double fConst159;
	double fConst160;
	double fConst161;
	double fConst162;
	double fConst163;
	double fRec157[2];
	FAUSTFLOAT fVbargraph45;
	double fConst164;
	double fConst165;
	double fConst166;
	double fConst167;
	double fConst168;
	double fConst169;
	double fConst170;
	double fConst171;
	double fConst172;
	double fConst173;
	double fConst174;
	double fConst175;
	double fRec158[2];
	FAUSTFLOAT fVbargraph46;
	double fConst176;
	double fConst177;
	double fConst178;
	double fConst179;
	double fConst180;
	double fConst181;
	double fConst182;
	double fConst183;
	double fConst184;
	double fConst185;
	double fConst186;
	double fConst187;
	double fConst188;
	double fRec159[2];
	FAUSTFLOAT fVbargraph47;
	double fConst189;
	double fConst190;
	double fConst191;
	double fConst192;
	double fConst193;
	double fConst194;
	double fConst195;
	double fConst196;
	double fConst197;
	double fConst198;
	double fRec160[2];
	FAUSTFLOAT fVbargraph48;
	double fConst199;
	double fConst200;
	double fConst201;
	double fConst202;
	double fConst203;
	double fConst204;
	double fRec161[2];
	FAUSTFLOAT fVbargraph49;
	double fConst205;
	double fConst206;
	double fConst207;
	double fConst208;
	double fConst209;
	double fConst210;
	double fConst211;
	double fConst212;
	double fRec162[2];
	FAUSTFLOAT fVbargraph50;
	double fConst213;
	double fConst214;
	double fConst215;
	double fConst216;
	double fConst217;
	double fConst218;
	double fConst219;
	double fConst220;
	double fConst221;
	double fConst222;
	double fConst223;
	double fConst224;
	double fConst225;
	double fConst226;
	double fConst227;
	double fConst228;
	double fConst229;
	double fConst230;
	double fRec163[2];
	FAUSTFLOAT fVbargraph51;
	double fConst231;
	double fConst232;
	double fConst233;
	double fConst234;
	double fConst235;
	double fConst236;
	double fConst237;
	double fConst238;
	double fConst239;
	double fConst240;
	double fRec164[2];
	FAUSTFLOAT fVbargraph52;
	double fConst241;
	double fConst242;
	double fConst243;
	double fConst244;
	double fConst245;
	double fConst246;
	double fConst247;
	double fConst248;
	double fConst249;
	double fConst250;
	double fConst251;
	double fConst252;
	double fConst253;
	double fRec165[2];
	FAUSTFLOAT fVbargraph53;
	double fConst254;
	double fConst255;
	double fConst256;
	double fConst257;
	double fConst258;
	double fConst259;
	double fConst260;
	double fConst261;
	double fConst262;
	double fConst263;
	double fConst264;
	double fConst265;
	double fRec166[2];
	FAUSTFLOAT fVbargraph54;
	double fConst266;
	double fConst267;
	double fConst268;
	double fConst269;
	double fConst270;
	double fConst271;
	double fConst272;
	double fConst273;
	double fConst274;
	double fConst275;
	double fRec167[2];
	FAUSTFLOAT fVbargraph55;
	double fConst276;
	double fConst277;
	double fConst278;
	double fConst279;
	double fConst280;
	double fConst281;
	double fConst282;
	double fConst283;
	double fConst284;
	double fConst285;
	double fConst286;
	double fConst287;
	double fConst288;
	double fConst289;
	double fRec168[2];
	FAUSTFLOAT fVbargraph56;
	double fConst290;
	double fConst291;
	double fConst292;
	double fConst293;
	double fConst294;
	double fConst295;
	double fConst296;
	double fConst297;
	double fConst298;
	double fConst299;
	double fConst300;
	double fConst301;
	double fConst302;
	double fConst303;
	double fConst304;
	double fConst305;
	double fConst306;
	double fRec169[2];
	FAUSTFLOAT fVbargraph57;
	double fConst307;
	double fConst308;
	double fConst309;
	double fConst310;
	double fConst311;
	double fConst312;
	double fConst313;
	double fConst314;
	double fConst315;
	double fConst316;
	double fConst317;
	double fConst318;
	double fConst319;
	double fConst320;
	double fConst321;
	double fConst322;
	double fConst323;
	double fConst324;
	double fConst325;
	double fRec170[2];
	FAUSTFLOAT fVbargraph58;
	double fConst326;
	double fConst327;
	double fConst328;
	double fConst329;
	double fConst330;
	double fConst331;
	double fConst332;
	double fConst333;
	double fConst334;
	double fConst335;
	double fConst336;
	double fConst337;
	double fConst338;
	double fConst339;
	double fRec171[2];
	FAUSTFLOAT fVbargraph59;
	double fConst340;
	double fConst341;
	double fConst342;
	double fConst343;
	double fConst344;
	double fConst345;
	double fConst346;
	double fConst347;
	double fConst348;
	double fConst349;
	double fConst350;
	double fConst351;
	double fConst352;
	double fConst353;
	double fRec172[2];
	FAUSTFLOAT fVbargraph60;
	double fConst354;
	double fConst355;
	double fConst356;
	double fConst357;
	double fConst358;
	double fConst359;
	double fConst360;
	double fConst361;
	double fConst362;
	double fConst363;
	double fConst364;
	double fConst365;
	double fConst366;
	double fConst367;
	double fConst368;
	double fConst369;
	double fConst370;
	double fConst371;
	double fRec173[2];
	FAUSTFLOAT fVbargraph61;
	double fConst372;
	double fConst373;
	double fConst374;
	double fConst375;
	double fConst376;
	double fConst377;
	double fConst378;
	double fConst379;
	double fConst380;
	double fConst381;
	double fConst382;
	double fConst383;
	double fConst384;
	double fConst385;
	double fConst386;
	double fConst387;
	double fConst388;
	double fConst389;
	double fConst390;
	double fConst391;
	double fRec174[2];
	FAUSTFLOAT fVbargraph62;
	double fConst392;
	double fConst393;
	double fConst394;
	double fConst395;
	double fConst396;
	double fConst397;
	double fConst398;
	double fConst399;
	double fConst400;
	double fRec175[2];
	FAUSTFLOAT fVbargraph63;
	double fConst401;
	double fConst402;
	double fConst403;
	double fConst404;
	double fConst405;
	double fConst406;
	double fConst407;
	double fRec176[2];
	FAUSTFLOAT fVbargraph64;
	double fConst408;
	double fConst409;
	double fConst410;
	double fConst411;
	double fConst412;
	double fConst413;
	double fConst414;
	double fConst415;
	double fConst416;
	double fConst417;
	double fConst418;
	double fConst419;
	double fRec177[2];
	FAUSTFLOAT fVbargraph65;
	double fConst420;
	double fConst421;
	double fConst422;
	double fConst423;
	double fConst424;
	double fConst425;
	double fConst426;
	double fConst427;
	double fConst428;
	double fConst429;
	double fConst430;
	double fConst431;
	double fConst432;
	double fConst433;
	double fConst434;
	double fRec178[2];
	FAUSTFLOAT fVbargraph66;
	double fConst435;
	double fConst436;
	double fConst437;
	double fConst438;
	double fConst439;
	double fConst440;
	double fConst441;
	double fConst442;
	double fRec179[2];
	FAUSTFLOAT fVbargraph67;
	double fConst443;
	double fConst444;
	double fConst445;
	double fConst446;
	double fConst447;
	double fConst448;
	double fConst449;
	double fConst450;
	double fRec180[2];
	FAUSTFLOAT fVbargraph68;
	double fConst451;
	double fConst452;
	double fConst453;
	double fConst454;
	double fConst455;
	double fConst456;
	double fConst457;
	double fConst458;
	double fConst459;
	double fConst460;
	double fConst461;
	double fConst462;
	double fConst463;
	double fConst464;
	double fRec181[2];
	FAUSTFLOAT fVbargraph69;
	double fConst465;
	double fConst466;
	double fConst467;
	double fConst468;
	double fConst469;
	double fConst470;
	double fConst471;
	double fConst472;
	double fConst473;
	double fConst474;
	double fConst475;
	double fConst476;
	double fConst477;
	double fConst478;
	double fConst479;
	double fRec182[2];
	FAUSTFLOAT fVbargraph70;
	double fConst480;
	double fConst481;
	double fConst482;
	double fConst483;
	double fConst484;
	double fConst485;
	double fConst486;
	double fConst487;
	double fConst488;
	double fConst489;
	double fConst490;
	double fConst491;
	double fConst492;
	double fConst493;
	double fConst494;
	double fConst495;
	double fConst496;
	double fRec183[2];
	FAUSTFLOAT fVbargraph71;
	double fConst497;
	double fConst498;
	double fConst499;
	double fConst500;
	double fConst501;
	double fConst502;
	double fConst503;
	double fConst504;
	double fConst505;
	double fRec184[2];
	FAUSTFLOAT fVbargraph72;
	double fConst506;
	double fConst507;
	double fConst508;
	double fConst509;
	double fConst510;
	double fConst511;
	double fRec185[2];
	FAUSTFLOAT fVbargraph73;
	double fConst512;
	double fConst513;
	double fConst514;
	double fConst515;
	double fConst516;
	double fConst517;
	double fConst518;
	double fConst519;
	double fRec186[2];
	FAUSTFLOAT fVbargraph74;
	
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
		m->declare("name", "HOADecLebedev504");
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
		return 25;
		
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
		fConst2 = (170.0 / fConst0);
		fConst3 = (340.0 / fConst0);
		fConst4 = (510.0 / fConst0);
		fConst5 = (1020.0 / fConst0);
		fConst6 = (394.77151028643527 / fConst0);
		fConst7 = (625.22848971356643 / fConst0);
		fConst8 = (1250.4569794271329 / fConst0);
		fConst9 = (789.54302057287055 / fConst0);
		fConst10 = (984.71160495892411 / fConst0);
		fConst11 = (715.28839504107589 / fConst0);
		fConst12 = (1430.5767900821518 / fConst0);
		fConst13 = (1969.4232099178482 / fConst0);
		fConst14 = double(tgamma(1.5));
		fConst15 = double(tgamma(3.0));
		fConst16 = double(tgamma(5.0));
		fConst17 = sqrt((7.0 * (fConst15 / fConst16)));
		fConst18 = ((0.0 - (3.3851375012865379 * fConst14)) * fConst17);
		fConst19 = double(tgamma(2.5));
		fConst20 = double(tgamma(7.0));
		fConst21 = sqrt((9.0 * (fConst15 / fConst20)));
		fConst22 = ((0.0 - (11.283791670955125 * fConst19)) * fConst21);
		fConst23 = double(tgamma(3.5));
		fConst24 = double(tgamma(1.0));
		fConst25 = sqrt((7.0 * (fConst24 / fConst20)));
		fConst26 = (fConst23 * fConst25);
		fConst27 = (0.081054939510132359 * fConst26);
		fConst28 = sqrt((5.0 * (fConst24 / fConst16)));
		fConst29 = (fConst19 * fConst28);
		fConst30 = (0.04052746975506618 * fConst29);
		fConst31 = sqrt((3.0 * (fConst24 / fConst15)));
		fConst32 = (fConst14 * fConst31);
		fConst33 = (0.02026373487753309 * fConst32);
		fConst34 = (double(tgamma(4.5)) * sqrt((9.0 * (fConst24 / double(tgamma(9.0))))));
		fConst35 = (0.16210987902026472 * fConst34);
		fConst36 = (0.0 - (3.1915382432114616 * fConst29));
		fConst37 = (0.0 - (6.3830764864229232 * fConst26));
		fConst38 = (0.0 - (0.70710678118654757 * fConst22));
		fConst39 = (0.0089791337293529845 * fConst18);
		fConst40 = (0.0 - (1.5957691216057308 * fConst32));
		fConst41 = (0.0 - (0.70710678118654757 * fConst18));
		fConst42 = (0.0089791337293529845 * fConst22);
		fConst43 = (0.050946219890026692 * fConst25);
		fConst44 = double(tgamma(2.0));
		fConst45 = sqrt((9.0 * (fConst44 / double(tgamma(8.0)))));
		fConst46 = (0.25217092292041182 * fConst45);
		fConst47 = (0.036024417560058836 * fConst28);
		fConst48 = double(tgamma(6.0));
		fConst49 = sqrt((7.0 * (fConst44 / fConst48)));
		fConst50 = (0.12736554972506672 * fConst49);
		fConst51 = (0.22515260975036763 * fConst21);
		fConst52 = (0.025473109945013343 * fConst31);
		fConst53 = double(tgamma(4.0));
		fConst54 = sqrt((5.0 * (fConst44 / fConst53)));
		fConst55 = (0.054036626340088233 * fConst54);
		fConst56 = (0.057314497376279992 * fConst17);
		fConst57 = sqrt((9.0 * (fConst53 / fConst48)));
		fConst58 = (0.02251526097503671 * fConst57);
		fConst59 = (0.072048835120117685 * fConst34);
		fConst60 = (0.0 - (1.595769121605731 * fConst29));
		fConst61 = (0.0 - (2.256758334191026 * fConst26));
		fConst62 = (fConst19 * fConst49);
		fConst63 = (0.0 - (5.6418958354775643 * fConst62));
		fConst64 = (fConst23 * fConst45);
		fConst65 = (0.0 - (11.170383851240118 * fConst64));
		fConst66 = (fConst19 * fConst21);
		fConst67 = (0.0 - (9.9735570100358153 * fConst66));
		fConst68 = (0.0 - (1.1283791670955128 * fConst32));
		fConst69 = (fConst14 * fConst54);
		fConst70 = (0.0 - (2.3936536824085959 * fConst69));
		fConst71 = (fConst14 * fConst17);
		fConst72 = (0.0 - (2.5388531259649025 * fConst71));
		fConst73 = (fConst14 * fConst57);
		fConst74 = (0.0 - (0.99735570100357918 * fConst73));
		fConst75 = (0.0 - (4.5135166683820485 * fConst26));
		fConst76 = (0.0 - (12.766152972845845 * fConst34));
		fConst77 = (1.3574799921485692e-16 * fConst34);
		fConst78 = (0.10189243978005338 * fConst26);
		fConst79 = (1.3574799921485695e-16 * fConst34);
		fConst80 = (0.011287477954144623 * fConst18);
		fConst81 = (0.025473109945013343 * fConst32);
		fConst82 = (0.0 - (1.1283791670955126 * fConst32));
		fConst83 = (0.0 - (7.0866384831701748e-16 * fConst29));
		fConst84 = (0.0 - (0.5 * fConst18));
		fConst85 = (0.0 - (1.5700924586837752e-16 * fConst22));
		fConst86 = (0.0 - (12.766152972845846 * fConst34));
		fConst87 = (0.0 - (1.128379167095513 * fConst32));
		fConst88 = (0.0 - (4.5135166683820547 * fConst26));
		fConst89 = (0.0 - (0.50000000000000011 * fConst18));
		fConst90 = (2.7149599842971385e-16 * fConst34);
		fConst91 = (0.10189243978005329 * fConst26);
		fConst92 = (2.714959984297139e-16 * fConst34);
		fConst93 = (0.01128747795414462 * fConst18);
		fConst94 = (0.025473109945013332 * fConst32);
		fConst95 = (0.0 - (1.417327696634035e-15 * fConst29));
		fConst96 = (0.0 - (4.5135166683820476 * fConst26));
		fConst97 = (0.0 - (4.5135166683820538 * fConst26));
		fConst98 = (0.0 - (3.1401849173675503e-16 * fConst22));
		fConst99 = (0.0 - (1.6925687506432687 * fConst14));
		fConst100 = (fConst99 * fConst54);
		fConst101 = (0.0 - (3.9894228040143274 * fConst19));
		fConst102 = (fConst101 * fConst49);
		fConst103 = ((0.0 - (7.898654169668589 * fConst23)) * fConst45);
		fConst104 = (0.0 - ((3.5355339059327373 * fConst99) + (2.3936536824085963 * fConst14)));
		fConst105 = (fConst104 * fConst17);
		fConst106 = ((0.0 - ((4.9497474683058318 * fConst101) + (5.6418958354775643 * fConst19))) * fConst21);
		fConst107 = ((0.0 - ((2.4748737341529159 * fConst104) + (4.0 * fConst99))) * fConst57);
		fConst108 = (0.010641936271825759 * fConst107);
		fConst109 = (0.050946219890026692 * fConst26);
		fConst110 = (0.036024417560058836 * fConst29);
		fConst111 = (0.0 - (1.4142135623730951 * fConst102));
		fConst112 = (0.0 - (1.4142135623730951 * fConst103));
		fConst113 = (0.0 - (0.70710678118654757 * fConst106));
		fConst114 = (0.015962904407738641 * fConst105);
		fConst115 = (0.031925808815477283 * fConst100);
		fConst116 = (0.0 - (1.4142135623730951 * fConst100));
		fConst117 = (0.0 - (0.70710678118654757 * fConst105));
		fConst118 = (0.0 - (0.47140452079103168 * fConst107));
		fConst119 = (0.015962904407738641 * fConst106);
		fConst120 = (0.031925808815477283 * fConst102);
		fConst121 = (0.031925808815477283 * fConst103);
		fConst122 = (0.12956030605175081 * fConst49);
		fConst123 = (2.8768166971227152e-17 * fConst49);
		fConst124 = (0.14960335515053727 * fConst21);
		fConst125 = (3.3218617889860196e-17 * fConst21);
		fConst126 = ((0.0 - (2.6596152026762163 * fConst14)) * fConst57);
		fConst127 = (0.051824122420700336 * fConst25);
		fConst128 = (0.20944469721075218 * fConst45);
		fConst129 = (0.033660754908870889 * fConst54);
		fConst130 = (0.019434045907762629 * fConst17);
		fConst131 = (0.019434045907762633 * fConst17);
		fConst132 = (5.6373863931755746e-17 * fConst34);
		fConst133 = (0.0 - (2.4568472851294949 * fConst26));
		fConst134 = (0.0 - (9.929230089991206 * fConst64));
		fConst135 = (0.0 - (5.6738457657092614 * fConst34));
		fConst136 = (0.0 - (0.92131773192356115 * fConst32));
		fConst137 = (0.0 - (2.1276921621409737 * fConst29));
		fConst138 = (0.0 - (1.5957691216057308 * fConst69));
		fConst139 = (0.0 - (4.7244256554467816e-16 * fConst29));
		fConst140 = (0.0 - (6.1421182128237417 * fConst62));
		fConst141 = (0.0 - (0.92131773192356159 * fConst71));
		fConst142 = (0.0 - (1.3638242119692871e-15 * fConst62));
		fConst143 = (0.0 - (7.0923072071365816 * fConst66));
		fConst144 = (0.0 - (0.33333333333333331 * fConst126));
		fConst145 = (0.0 - (1.5748085518155944e-15 * fConst66));
		fConst146 = (0.0 - (5.6738457657092622 * fConst34));
		fConst147 = (0.019434045907762623 * fConst31);
		fConst148 = (5.6373863931755758e-17 * fConst34);
		fConst149 = (0.007031250000000001 * fConst126);
		fConst150 = (0.20944469721075201 * fConst45);
		fConst151 = (0.0 - (0.92131773192356148 * fConst32));
		fConst152 = (0.0 - (1.5957691216057313 * fConst69));
		fConst153 = (0.0 - (2.456847285129498 * fConst26));
		fConst154 = (0.0 - (0.92131773192356192 * fConst71));
		fConst155 = (0.0 - (9.9292300899912185 * fConst64));
		fConst156 = (0.0 - (0.33333333333333343 * fConst126));
		fConst157 = (0.051824122420700267 * fConst25);
		fConst158 = (0.20944469721075193 * fConst45);
		fConst159 = (0.12956030605175084 * fConst49);
		fConst160 = (5.7536333942454304e-17 * fConst49);
		fConst161 = (0.14960335515053733 * fConst21);
		fConst162 = (6.6437235779720392e-17 * fConst21);
		fConst163 = (1.1274772786351149e-16 * fConst34);
		fConst164 = (0.019434045907762616 * fConst31);
		fConst165 = (0.033660754908870882 * fConst54);
		fConst166 = (0.019434045907762626 * fConst17);
		fConst167 = (1.1274772786351152e-16 * fConst34);
		fConst168 = (0.0070312499999999993 * fConst126);
		fConst169 = (0.0 - (9.4488513108935631e-16 * fConst29));
		fConst170 = (0.0 - (2.456847285129494 * fConst26));
		fConst171 = (0.0 - (2.7276484239385742e-15 * fConst62));
		fConst172 = (0.0 - (2.4568472851294976 * fConst26));
		fConst173 = (0.0 - (9.9292300899912025 * fConst64));
		fConst174 = (0.0 - (3.1496171036311889e-15 * fConst66));
		fConst175 = (0.0 - (9.9292300899912167 * fConst64));
		fConst176 = (0.0 - (4.3431334391370653 * fConst19));
		fConst177 = ((0.0 - ((4.0414518843273806 * fConst176) + (7.5225277806367483 * fConst19))) * fConst21);
		fConst178 = (0.0 - (1.5957691216057308 * fConst14));
		fConst179 = (0.0 - ((2.8867513459481291 * fConst178) + (2.7639531957706835 * fConst14)));
		fConst180 = ((0.0 - ((2.0207259421636903 * fConst179) + (4.0 * fConst178))) * fConst57);
		fConst181 = (fConst176 * fConst49);
		fConst182 = (fConst179 * fConst17);
		fConst183 = (fConst178 * fConst54);
		fConst184 = (0.021093750000000005 * fConst183);
		fConst185 = (0.051824122420700336 * fConst26);
		fConst186 = ((0.0 - (9.9292300899912096 * fConst23)) * fConst45);
		fConst187 = (0.021093750000000012 * fConst186);
		fConst188 = (0.0 - (0.99999999999999967 * fConst186));
		fConst189 = (0.0 - fConst183);
		fConst190 = (0.0 - (1.4142135623730951 * fConst181));
		fConst191 = (0.0 - (0.5 * fConst182));
		fConst192 = (0.0 - (3.1401849173675503e-16 * fConst181));
		fConst193 = (0.0 - (0.70710678118654757 * fConst177));
		fConst194 = (0.0 - (0.33333333333333331 * fConst180));
		fConst195 = (0.0 - (1.5700924586837752e-16 * fConst177));
		fConst196 = (0.010546875000000002 * fConst182);
		fConst197 = (0.019434045907762623 * fConst32);
		fConst198 = (0.007031250000000001 * fConst180);
		fConst199 = (0.0 - (1.0000000000000002 * fConst183));
		fConst200 = (0.0 - (0.50000000000000011 * fConst182));
		fConst201 = (0.0 - (1.0000000000000009 * fConst186));
		fConst202 = (0.0 - (0.33333333333333343 * fConst180));
		fConst203 = (0.051824122420700267 * fConst26);
		fConst204 = (0.021093749999999988 * fConst186);
		fConst205 = (0.010546874999999999 * fConst182);
		fConst206 = (0.021093749999999998 * fConst183);
		fConst207 = (0.019434045907762616 * fConst32);
		fConst208 = (0.0070312499999999993 * fConst180);
		fConst209 = (0.0 - (6.2803698347351007e-16 * fConst181));
		fConst210 = (0.0 - (0.99999999999999933 * fConst186));
		fConst211 = (0.0 - (3.1401849173675503e-16 * fConst177));
		fConst212 = (0.0 - (1.0000000000000007 * fConst186));
		fConst213 = (0.026338897580074728 * fConst54);
		fConst214 = (0.026338897580074724 * fConst54);
		fConst215 = (0.045001699736364593 * fConst17);
		fConst216 = (0.045001699736364587 * fConst17);
		fConst217 = (0.059861130863806261 * fConst57);
		fConst218 = (0.059861130863806254 * fConst57);
		fConst219 = (0.052943176160428845 * fConst49);
		fConst220 = (0.011706176702255417 * fConst28);
		fConst221 = (0.13834572466301867 * fConst21);
		fConst222 = (0.0070590901547238332 * fConst25);
		fConst223 = (0.044696311044975172 * fConst45);
		fConst224 = (0.0 - (1.2884797242127563e-16 * fConst29));
		fConst225 = (0.0 - (5.8273688116286149e-16 * fConst62));
		fConst226 = (0.0 - (0.34992181344801404 * fConst26));
		fConst227 = (0.0 - (1.9878387281720961e-16 * fConst34));
		fConst228 = (0.0 - (1.522748764978713e-15 * fConst66));
		fConst229 = (0.0 - (2.2156133258658022 * fConst64));
		fConst230 = (0.0 - (0.42202158587919975 * fConst34));
		fConst231 = (0.0 - (0.48114249349102017 * fConst32));
		fConst232 = (0.0 - (0.58027968058390089 * fConst29));
		fConst233 = (0.0 - (1.3056292813137786 * fConst69));
		fConst234 = (0.0 - (2.624413600860108 * fConst62));
		fConst235 = (0.0 - (2.230751560731095 * fConst71));
		fConst236 = (0.0 - (6.8578507705370146 * fConst66));
		fConst237 = (0.0 - (2.9673392757131363 * fConst73));
		fConst238 = (0.044696311044975207 * fConst45);
		fConst239 = (0.0097062489627452973 * fConst31);
		fConst240 = (4.0101337658685596e-18 * fConst34);
		fConst241 = (0.0 - (0.48114249349102034 * fConst32));
		fConst242 = (0.0 - (1.3056292813137791 * fConst69));
		fConst243 = (0.0 - (0.34992181344801421 * fConst26));
		fConst244 = (0.0 - (2.2307515607310959 * fConst71));
		fConst245 = (0.0 - (2.2156133258658035 * fConst64));
		fConst246 = (0.0 - (2.9673392757131376 * fConst73));
		fConst247 = (0.0070590901547238306 * fConst25);
		fConst248 = (0.044696311044975165 * fConst45);
		fConst249 = (0.052943176160428852 * fConst49);
		fConst250 = (2.3511493268037518e-17 * fConst49);
		fConst251 = (0.1383457246630187 * fConst21);
		fConst252 = (6.1437843551734282e-17 * fConst21);
		fConst253 = (8.0202675317371177e-18 * fConst34);
		fConst254 = (8.0202675317371192e-18 * fConst34);
		fConst255 = (0.0097062489627452939 * fConst31);
		fConst256 = (0.026338897580074721 * fConst54);
		fConst257 = (0.045001699736364573 * fConst17);
		fConst258 = (0.05986113086380624 * fConst57);
		fConst259 = (0.0 - (2.5769594484255127e-16 * fConst29));
		fConst260 = (0.0 - (0.34992181344801365 * fConst26));
		fConst261 = (0.0 - (1.165473762325723e-15 * fConst62));
		fConst262 = (0.0 - (0.34992181344801415 * fConst26));
		fConst263 = (0.0 - (2.2156133258658 * fConst64));
		fConst264 = (0.0 - (3.0454975299574259e-15 * fConst66));
		fConst265 = (0.0 - (2.2156133258658031 * fConst64));
		fConst266 = (0.19368401452822653 * fConst45);
		fConst267 = (0.13408893313492565 * fConst45);
		fConst268 = ((0.0 - (3.7301790647785582 * fConst19)) * fConst21);
		fConst269 = (0.0 - (1.7605101637794649 * fConst14));
		fConst270 = (((1.0552897060221724 * fConst269) - (3.8926344352424089 * fConst14)) * fConst57);
		fConst271 = (0.052943176160428991 * fConst49);
		fConst272 = (0.070590901547238516 * fConst49);
		fConst273 = (fConst269 * fConst17);
		fConst274 = (0.0087796325266915921 * fConst54);
		fConst275 = (0.026338897580074738 * fConst54);
		fConst276 = (0.0 - (2.3211187223356089 * fConst29));
		fConst277 = (0.0 - (3.1492963210321352 * fConst26));
		fConst278 = (0.0 - (3.4992181344801496 * fConst62));
		fConst279 = (0.0 - (4.548983574824196 * fConst26));
		fConst280 = (0.0 - (10.128518061100836 * fConst34));
		fConst281 = (0.0 - (6.6468399775974225 * fConst64));
		fConst282 = (0.0 - (0.56568542494923812 * fConst268));
		fConst283 = (0.0 - (9.600991078751834 * fConst64));
		fConst284 = (0.052943176160428929 * fConst49);
		fConst285 = (0.035118530106766334 * fConst28);
		fConst286 = (0.026338897580074742 * fConst54);
		fConst287 = (0.0087796325266915817 * fConst54);
		fConst288 = (0.0085588214148084006 * fConst268);
		fConst289 = (0.059595081393300449 * fConst34);
		fConst290 = (0.09176817201141016 * fConst25);
		fConst291 = (0.19368401452822634 * fConst45);
		fConst292 = (0.0 - (0.48114249349102062 * fConst32));
		fConst293 = (0.0 - (1.7408390417517061 * fConst29));
		fConst294 = (0.0 - (0.43520976043792636 * fConst69));
		fConst295 = (0.0 - (2.3211187223356093 * fConst29));
		fConst296 = (0.0 - (3.1492963210321379 * fConst26));
		fConst297 = (0.0 - (2.6244136008601116 * fConst62));
		fConst298 = (0.0 - (0.22360679774997891 * fConst273));
		fConst299 = (0.0 - (3.4992181344801501 * fConst62));
		fConst300 = (0.0 - (6.6468399775974278 * fConst64));
		fConst301 = (0.0 - (0.42426406871192845 * fConst268));
		fConst302 = (0.0 - (0.14907119849998593 * fConst270));
		fConst303 = (0.029118746888235927 * fConst31);
		fConst304 = (0.026338897580074745 * fConst54);
		fConst305 = (0.009021789919139777 * fConst270);
		fConst306 = (0.013532684878709665 * fConst273);
		fConst307 = (0.0 - (6.6468399775974083 * fConst64));
		fConst308 = (0.0 - (1.443427480473062 * fConst32));
		fConst309 = (0.0 - (1.7408390417517088 * fConst29));
		fConst310 = (0.0 - (1.3056292813137793 * fConst69));
		fConst311 = (0.0 - (2.6244136008601155 * fConst62));
		fConst312 = (0.0 - (0.67082039324993681 * fConst273));
		fConst313 = (0.0 - (3.1492963210321285 * fConst26));
		fConst314 = (0.0 - (10.128518061100845 * fConst34));
		fConst315 = (0.0 - (0.42426406871192907 * fConst268));
		fConst316 = (0.0 - (0.44721359549995787 * fConst270));
		fConst317 = (0.091768172011410271 * fConst25);
		fConst318 = (0.19368401452822659 * fConst45);
		fConst319 = (0.046824706809021746 * fConst28);
		fConst320 = (0.0097062489627453216 * fConst31);
		fConst321 = (0.0087796325266915939 * fConst54);
		fConst322 = (0.011411761886411192 * fConst268);
		fConst323 = (0.0030072633063799293 * fConst270);
		fConst324 = (0.0045108949595698946 * fConst273);
		fConst325 = (0.059595081393300019 * fConst34);
		fConst326 = (0.0 - (0.48114249349102112 * fConst32));
		fConst327 = (0.0 - (1.4434274804730622 * fConst32));
		fConst328 = (0.0 - (0.43520976043792686 * fConst69));
		fConst329 = (0.0 - (1.3056292813137795 * fConst69));
		fConst330 = (0.0 - (4.5489835748241987 * fConst26));
		fConst331 = (0.0 - (0.22360679774997916 * fConst273));
		fConst332 = (0.0 - (0.67082039324993692 * fConst273));
		fConst333 = (0.0 - (3.1492963210321339 * fConst26));
		fConst334 = (0.0 - (9.6009910787518393 * fConst64));
		fConst335 = (0.0 - (0.1490711984999861 * fConst270));
		fConst336 = (0.0 - (0.44721359549995793 * fConst270));
		fConst337 = (0.0 - (6.6468399775974198 * fConst64));
		fConst338 = (0.052943176160428963 * fConst49);
		fConst339 = (0.070590901547238558 * fConst49);
		fConst340 = (0.13408893313492587 * fConst45);
		fConst341 = (0.19368401452822642 * fConst45);
		fConst342 = (0.0 - (0.48114249349102095 * fConst32));
		fConst343 = (0.0 - (0.4352097604379267 * fConst69));
		fConst344 = (0.0 - (2.3211187223356085 * fConst29));
		fConst345 = (0.0 - (0.22360679774997907 * fConst273));
		fConst346 = (0.0 - (3.4992181344801487 * fConst62));
		fConst347 = (0.0 - (10.12851806110084 * fConst34));
		fConst348 = (0.0 - (0.14907119849998604 * fConst270));
		fConst349 = (0.0 - (0.5656854249492379 * fConst268));
		fConst350 = (0.052943176160428942 * fConst49);
		fConst351 = (0.035118530106766348 * fConst28);
		fConst352 = (0.0085588214148084041 * fConst268);
		fConst353 = (0.059595081393300338 * fConst34);
		fConst354 = (0.0 - (3.4992181344801505 * fConst62));
		fConst355 = (0.0 - (1.4434274804730625 * fConst32));
		fConst356 = (0.0 - (1.7408390417517057 * fConst29));
		fConst357 = (0.0 - (1.3056292813137798 * fConst69));
		fConst358 = (0.0 - (2.3211187223356098 * fConst29));
		fConst359 = (0.0 - (2.6244136008601107 * fConst62));
		fConst360 = (0.0 - (0.67082039324993703 * fConst273));
		fConst361 = (0.0 - (4.5489835748241934 * fConst26));
		fConst362 = (0.0 - (0.42426406871192829 * fConst268));
		fConst363 = (0.0 - (0.44721359549995798 * fConst270));
		fConst364 = (0.0 - (0.56568542494923824 * fConst268));
		fConst365 = (0.0 - (9.6009910787518287 * fConst64));
		fConst366 = (0.063531811392514792 * fConst25);
		fConst367 = (0.13408893313492612 * fConst45);
		fConst368 = (0.0097062489627453008 * fConst31);
		fConst369 = (0.0087796325266915748 * fConst54);
		fConst370 = (0.0030072633063799232 * fConst270);
		fConst371 = (0.004510894959569885 * fConst273);
		fConst372 = (0.063531811392514445 * fConst25);
		fConst373 = (0.13408893313492534 * fConst45);
		fConst374 = (0.0 - (0.48114249349102228 * fConst32));
		fConst375 = (0.0 - (1.7408390417517114 * fConst29));
		fConst376 = (0.0 - (0.43520976043792792 * fConst69));
		fConst377 = (0.0 - (4.5489835748242058 * fConst26));
		fConst378 = (0.0 - (2.6244136008601191 * fConst62));
		fConst379 = (0.0 - (0.22360679774997969 * fConst273));
		fConst380 = (0.0 - (10.12851806110085 * fConst34));
		fConst381 = (0.0 - (9.6009910787518553 * fConst64));
		fConst382 = (0.0 - (0.42426406871192973 * fConst268));
		fConst383 = (0.0 - (0.14907119849998646 * fConst270));
		fConst384 = (0.046824706809021698 * fConst28);
		fConst385 = (0.07059090154723846 * fConst49);
		fConst386 = (0.029118746888235913 * fConst31);
		fConst387 = (0.026338897580074731 * fConst54);
		fConst388 = (0.011411761886411183 * fConst268);
		fConst389 = (0.0090217899191397718 * fConst270);
		fConst390 = (0.01353268487870966 * fConst273);
		fConst391 = (0.059595081393299554 * fConst34);
		fConst392 = (fConst45 * (0.0 - (8.2571033474838984 * fConst23)));
		fConst393 = (0.0 - (3.0929010896773175 * fConst19));
		fConst394 = (fConst21 * (0.0 - ((2.1105794120443448 * fConst393) + (10.257992428141025 * fConst19))));
		fConst395 = (0.0 - (0.97315860881060223 * fConst14));
		fConst396 = (0.0 - ((1.5075567228888176 * fConst395) + (3.2276019669290172 * fConst14)));
		fConst397 = (fConst57 * (0.0 - ((1.0552897060221724 * fConst396) + (4.0 * fConst395))));
		fConst398 = (fConst49 * fConst393);
		fConst399 = (fConst17 * fConst396);
		fConst400 = (fConst54 * fConst395);
		fConst401 = (0.0 - (1.1313708498984762 * fConst398));
		fConst402 = (0.0 - (0.80498447189992428 * fConst392));
		fConst403 = (0.0 - (0.56568542494923812 * fConst394));
		fConst404 = (0.0 - (1.1627553482998907 * fConst392));
		fConst405 = (0.0085588214148084006 * fConst394);
		fConst406 = (0.017117642829616801 * fConst398);
		fConst407 = (0.035118530106766334 * fConst29);
		fConst408 = (0.0 - (0.44721359549995782 * fConst400));
		fConst409 = (0.0 - (0.84852813742385691 * fConst398));
		fConst410 = (0.0 - (0.22360679774997891 * fConst399));
		fConst411 = (0.0 - (0.80498447189992484 * fConst392));
		fConst412 = (0.0 - (0.42426406871192845 * fConst394));
		fConst413 = (0.0 - (0.14907119849998593 * fConst397));
		fConst414 = (0.023456653789763415 * fConst392);
		fConst415 = (0.009021789919139777 * fConst397);
		fConst416 = (0.013532684878709665 * fConst399);
		fConst417 = (0.027065369757419331 * fConst400);
		fConst418 = (0.029118746888235927 * fConst32);
		fConst419 = (0.09176817201141016 * fConst26);
		fConst420 = (0.0 - (1.3416407864998736 * fConst400));
		fConst421 = (0.0 - (0.84852813742385813 * fConst398));
		fConst422 = (0.0 - (0.67082039324993681 * fConst399));
		fConst423 = (0.0 - (0.42426406871192907 * fConst394));
		fConst424 = (0.0 - (0.44721359549995787 * fConst397));
		fConst425 = (0.0 - (0.8049844718999225 * fConst392));
		fConst426 = (0.011411761886411192 * fConst394);
		fConst427 = (0.0030072633063799293 * fConst397);
		fConst428 = (0.02345665378976344 * fConst392);
		fConst429 = (0.022823523772822384 * fConst398);
		fConst430 = (0.0045108949595698946 * fConst399);
		fConst431 = (0.0090217899191397891 * fConst400);
		fConst432 = (0.091768172011410271 * fConst26);
		fConst433 = (0.0097062489627453216 * fConst32);
		fConst434 = (0.046824706809021746 * fConst29);
		fConst435 = (0.0 - (0.22360679774997916 * fConst399));
		fConst436 = (0.0 - (0.44721359549995832 * fConst400));
		fConst437 = (0.0 - (1.3416407864998738 * fConst400));
		fConst438 = (0.0 - (0.67082039324993692 * fConst399));
		fConst439 = (0.0 - (1.1627553482998914 * fConst392));
		fConst440 = (0.0 - (0.1490711984999861 * fConst397));
		fConst441 = (0.0 - (0.44721359549995793 * fConst397));
		fConst442 = (0.0 - (0.80498447189992395 * fConst392));
		fConst443 = (0.0 - (0.44721359549995815 * fConst400));
		fConst444 = (0.0 - (0.22360679774997907 * fConst399));
		fConst445 = (0.0 - (1.1313708498984758 * fConst398));
		fConst446 = (0.0 - (0.14907119849998604 * fConst397));
		fConst447 = (0.0 - (0.5656854249492379 * fConst394));
		fConst448 = (0.0085588214148084041 * fConst394);
		fConst449 = (0.017117642829616808 * fConst398);
		fConst450 = (0.035118530106766348 * fConst29);
		fConst451 = (0.0 - (1.3416407864998741 * fConst400));
		fConst452 = (0.0 - (0.84852813742385658 * fConst398));
		fConst453 = (0.0 - (0.67082039324993703 * fConst399));
		fConst454 = (0.0 - (1.1313708498984765 * fConst398));
		fConst455 = (0.0 - (0.42426406871192829 * fConst394));
		fConst456 = (0.0 - (0.44721359549995798 * fConst397));
		fConst457 = (0.0 - (0.56568542494923824 * fConst394));
		fConst458 = (0.0 - (1.1627553482998902 * fConst392));
		fConst459 = (0.0030072633063799232 * fConst397);
		fConst460 = (0.016239221854451615 * fConst392);
		fConst461 = (0.004510894959569885 * fConst399);
		fConst462 = (0.00902178991913977 * fConst400);
		fConst463 = (0.063531811392514792 * fConst26);
		fConst464 = (0.0097062489627453008 * fConst32);
		fConst465 = (0.0 - (0.44721359549995937 * fConst400));
		fConst466 = (0.0 - (0.84852813742385946 * fConst398));
		fConst467 = (0.0 - (0.22360679774997969 * fConst399));
		fConst468 = (0.0 - (1.1627553482998934 * fConst392));
		fConst469 = (0.0 - (0.42426406871192973 * fConst394));
		fConst470 = (0.0 - (0.14907119849998646 * fConst397));
		fConst471 = (0.016239221854451522 * fConst392);
		fConst472 = (0.011411761886411183 * fConst394);
		fConst473 = (0.0090217899191397718 * fConst397);
		fConst474 = (0.022823523772822367 * fConst398);
		fConst475 = (0.01353268487870966 * fConst399);
		fConst476 = (0.027065369757419321 * fConst400);
		fConst477 = (0.063531811392514445 * fConst26);
		fConst478 = (0.046824706809021698 * fConst29);
		fConst479 = (0.029118746888235913 * fConst32);
		fConst480 = (0.0 - (1.3056292813137786 * fConst14));
		fConst481 = (0.0 - ((4.5226701686664557 * fConst480) + (1.4434274804730605 * fConst14)));
		fConst482 = (fConst57 * (0.0 - ((3.1658691180665191 * fConst481) + (4.0 * fConst480))));
		fConst483 = (0.0 - (1.8557406538063874 * fConst19));
		fConst484 = (fConst21 * (0.0 - ((6.3317382361330381 * fConst483) + (2.0515984856282001 * fConst19))));
		fConst485 = (0.014264702358014003 * fConst484);
		fConst486 = (fConst45 * (0.0 - (2.2156133258658013 * fConst23)));
		fConst487 = (0.020173335537918863 * fConst486);
		fConst488 = (fConst481 * fConst17);
		fConst489 = (fConst483 * fConst49);
		fConst490 = (0.028529404716028006 * fConst489);
		fConst491 = (fConst480 * fConst54);
		fConst492 = (0.011706176702255417 * fConst29);
		fConst493 = (0.0070590901547238332 * fConst26);
		fConst494 = (0.0 - (3.1401849173675503e-16 * fConst489));
		fConst495 = (0.0 - (1.5700924586837752e-16 * fConst484));
		fConst496 = (0.0 - (1.0000000000000004 * fConst486));
		fConst497 = (0.0 - fConst491);
		fConst498 = (0.0 - (1.4142135623730951 * fConst489));
		fConst499 = (0.0 - (0.5 * fConst488));
		fConst500 = (0.0 - (0.70710678118654757 * fConst484));
		fConst501 = (0.0 - (0.33333333333333331 * fConst482));
		fConst502 = (0.0067244451793062911 * fConst482);
		fConst503 = (0.010086667768959437 * fConst488);
		fConst504 = (0.020173335537918873 * fConst491);
		fConst505 = (0.0097062489627452973 * fConst32);
		fConst506 = (0.0 - (1.0000000000000002 * fConst491));
		fConst507 = (0.0 - (0.50000000000000011 * fConst488));
		fConst508 = (0.0 - (1.0000000000000009 * fConst486));
		fConst509 = (0.0 - (0.33333333333333343 * fConst482));
		fConst510 = (0.0070590901547238306 * fConst26);
		fConst511 = (0.020173335537918856 * fConst486);
		fConst512 = (0.010086667768959433 * fConst488);
		fConst513 = (0.020173335537918866 * fConst491);
		fConst514 = (0.0097062489627452939 * fConst32);
		fConst515 = (0.0067244451793062885 * fConst482);
		fConst516 = (0.0 - (6.2803698347351007e-16 * fConst489));
		fConst517 = (0.0 - (0.99999999999999933 * fConst486));
		fConst518 = (0.0 - (3.1401849173675503e-16 * fConst484));
		fConst519 = (0.0 - (1.0000000000000007 * fConst486));
		
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
			fRec20[l16] = 0.0;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec21[l17] = 0.0;
			
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec19[l18] = 0.0;
			
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec22[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec23[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec18[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec17[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec0[l23] = 0.0;
			
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
			fRec37[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec32[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec31[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec41[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec42[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec40[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec43[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec39[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec38[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec46[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec47[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec45[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec44[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec50[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec49[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec48[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec54[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec55[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec53[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec56[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec57[l54] = 0.0;
			
		}
		for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
			fRec52[l55] = 0.0;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fRec51[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec24[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec62[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec63[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec61[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec64[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fRec60[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec59[l63] = 0.0;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec68[l64] = 0.0;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec69[l65] = 0.0;
			
		}
		for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
			fRec67[l66] = 0.0;
			
		}
		for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
			fRec70[l67] = 0.0;
			
		}
		for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
			fRec66[l68] = 0.0;
			
		}
		for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
			fRec65[l69] = 0.0;
			
		}
		for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
			fRec73[l70] = 0.0;
			
		}
		for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
			fRec72[l71] = 0.0;
			
		}
		for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
			fRec71[l72] = 0.0;
			
		}
		for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
			fRec58[l73] = 0.0;
			
		}
		for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
			fRec74[l74] = 0.0;
			
		}
		for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
			fRec75[l75] = 0.0;
			
		}
		for (int l76 = 0; (l76 < 2); l76 = (l76 + 1)) {
			fRec76[l76] = 0.0;
			
		}
		for (int l77 = 0; (l77 < 2); l77 = (l77 + 1)) {
			fRec81[l77] = 0.0;
			
		}
		for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
			fRec82[l78] = 0.0;
			
		}
		for (int l79 = 0; (l79 < 2); l79 = (l79 + 1)) {
			fRec80[l79] = 0.0;
			
		}
		for (int l80 = 0; (l80 < 2); l80 = (l80 + 1)) {
			fRec83[l80] = 0.0;
			
		}
		for (int l81 = 0; (l81 < 2); l81 = (l81 + 1)) {
			fRec84[l81] = 0.0;
			
		}
		for (int l82 = 0; (l82 < 2); l82 = (l82 + 1)) {
			fRec79[l82] = 0.0;
			
		}
		for (int l83 = 0; (l83 < 2); l83 = (l83 + 1)) {
			fRec78[l83] = 0.0;
			
		}
		for (int l84 = 0; (l84 < 2); l84 = (l84 + 1)) {
			fRec88[l84] = 0.0;
			
		}
		for (int l85 = 0; (l85 < 2); l85 = (l85 + 1)) {
			fRec89[l85] = 0.0;
			
		}
		for (int l86 = 0; (l86 < 2); l86 = (l86 + 1)) {
			fRec87[l86] = 0.0;
			
		}
		for (int l87 = 0; (l87 < 2); l87 = (l87 + 1)) {
			fRec90[l87] = 0.0;
			
		}
		for (int l88 = 0; (l88 < 2); l88 = (l88 + 1)) {
			fRec86[l88] = 0.0;
			
		}
		for (int l89 = 0; (l89 < 2); l89 = (l89 + 1)) {
			fRec85[l89] = 0.0;
			
		}
		for (int l90 = 0; (l90 < 2); l90 = (l90 + 1)) {
			fRec93[l90] = 0.0;
			
		}
		for (int l91 = 0; (l91 < 2); l91 = (l91 + 1)) {
			fRec94[l91] = 0.0;
			
		}
		for (int l92 = 0; (l92 < 2); l92 = (l92 + 1)) {
			fRec92[l92] = 0.0;
			
		}
		for (int l93 = 0; (l93 < 2); l93 = (l93 + 1)) {
			fRec91[l93] = 0.0;
			
		}
		for (int l94 = 0; (l94 < 2); l94 = (l94 + 1)) {
			fRec98[l94] = 0.0;
			
		}
		for (int l95 = 0; (l95 < 2); l95 = (l95 + 1)) {
			fRec99[l95] = 0.0;
			
		}
		for (int l96 = 0; (l96 < 2); l96 = (l96 + 1)) {
			fRec97[l96] = 0.0;
			
		}
		for (int l97 = 0; (l97 < 2); l97 = (l97 + 1)) {
			fRec100[l97] = 0.0;
			
		}
		for (int l98 = 0; (l98 < 2); l98 = (l98 + 1)) {
			fRec101[l98] = 0.0;
			
		}
		for (int l99 = 0; (l99 < 2); l99 = (l99 + 1)) {
			fRec96[l99] = 0.0;
			
		}
		for (int l100 = 0; (l100 < 2); l100 = (l100 + 1)) {
			fRec95[l100] = 0.0;
			
		}
		for (int l101 = 0; (l101 < 2); l101 = (l101 + 1)) {
			fRec77[l101] = 0.0;
			
		}
		for (int l102 = 0; (l102 < 2); l102 = (l102 + 1)) {
			fRec106[l102] = 0.0;
			
		}
		for (int l103 = 0; (l103 < 2); l103 = (l103 + 1)) {
			fRec107[l103] = 0.0;
			
		}
		for (int l104 = 0; (l104 < 2); l104 = (l104 + 1)) {
			fRec105[l104] = 0.0;
			
		}
		for (int l105 = 0; (l105 < 2); l105 = (l105 + 1)) {
			fRec108[l105] = 0.0;
			
		}
		for (int l106 = 0; (l106 < 2); l106 = (l106 + 1)) {
			fRec109[l106] = 0.0;
			
		}
		for (int l107 = 0; (l107 < 2); l107 = (l107 + 1)) {
			fRec104[l107] = 0.0;
			
		}
		for (int l108 = 0; (l108 < 2); l108 = (l108 + 1)) {
			fRec103[l108] = 0.0;
			
		}
		for (int l109 = 0; (l109 < 2); l109 = (l109 + 1)) {
			fRec112[l109] = 0.0;
			
		}
		for (int l110 = 0; (l110 < 2); l110 = (l110 + 1)) {
			fRec113[l110] = 0.0;
			
		}
		for (int l111 = 0; (l111 < 2); l111 = (l111 + 1)) {
			fRec111[l111] = 0.0;
			
		}
		for (int l112 = 0; (l112 < 2); l112 = (l112 + 1)) {
			fRec110[l112] = 0.0;
			
		}
		for (int l113 = 0; (l113 < 2); l113 = (l113 + 1)) {
			fRec117[l113] = 0.0;
			
		}
		for (int l114 = 0; (l114 < 2); l114 = (l114 + 1)) {
			fRec118[l114] = 0.0;
			
		}
		for (int l115 = 0; (l115 < 2); l115 = (l115 + 1)) {
			fRec116[l115] = 0.0;
			
		}
		for (int l116 = 0; (l116 < 2); l116 = (l116 + 1)) {
			fRec119[l116] = 0.0;
			
		}
		for (int l117 = 0; (l117 < 2); l117 = (l117 + 1)) {
			fRec120[l117] = 0.0;
			
		}
		for (int l118 = 0; (l118 < 2); l118 = (l118 + 1)) {
			fRec115[l118] = 0.0;
			
		}
		for (int l119 = 0; (l119 < 2); l119 = (l119 + 1)) {
			fRec114[l119] = 0.0;
			
		}
		for (int l120 = 0; (l120 < 2); l120 = (l120 + 1)) {
			fRec102[l120] = 0.0;
			
		}
		for (int l121 = 0; (l121 < 2); l121 = (l121 + 1)) {
			fRec121[l121] = 0.0;
			
		}
		for (int l122 = 0; (l122 < 2); l122 = (l122 + 1)) {
			fRec122[l122] = 0.0;
			
		}
		for (int l123 = 0; (l123 < 2); l123 = (l123 + 1)) {
			fRec127[l123] = 0.0;
			
		}
		for (int l124 = 0; (l124 < 2); l124 = (l124 + 1)) {
			fRec128[l124] = 0.0;
			
		}
		for (int l125 = 0; (l125 < 2); l125 = (l125 + 1)) {
			fRec126[l125] = 0.0;
			
		}
		for (int l126 = 0; (l126 < 2); l126 = (l126 + 1)) {
			fRec129[l126] = 0.0;
			
		}
		for (int l127 = 0; (l127 < 2); l127 = (l127 + 1)) {
			fRec130[l127] = 0.0;
			
		}
		for (int l128 = 0; (l128 < 2); l128 = (l128 + 1)) {
			fRec125[l128] = 0.0;
			
		}
		for (int l129 = 0; (l129 < 2); l129 = (l129 + 1)) {
			fRec124[l129] = 0.0;
			
		}
		for (int l130 = 0; (l130 < 2); l130 = (l130 + 1)) {
			fRec134[l130] = 0.0;
			
		}
		for (int l131 = 0; (l131 < 2); l131 = (l131 + 1)) {
			fRec135[l131] = 0.0;
			
		}
		for (int l132 = 0; (l132 < 2); l132 = (l132 + 1)) {
			fRec133[l132] = 0.0;
			
		}
		for (int l133 = 0; (l133 < 2); l133 = (l133 + 1)) {
			fRec136[l133] = 0.0;
			
		}
		for (int l134 = 0; (l134 < 2); l134 = (l134 + 1)) {
			fRec137[l134] = 0.0;
			
		}
		for (int l135 = 0; (l135 < 2); l135 = (l135 + 1)) {
			fRec132[l135] = 0.0;
			
		}
		for (int l136 = 0; (l136 < 2); l136 = (l136 + 1)) {
			fRec131[l136] = 0.0;
			
		}
		for (int l137 = 0; (l137 < 2); l137 = (l137 + 1)) {
			fRec140[l137] = 0.0;
			
		}
		for (int l138 = 0; (l138 < 2); l138 = (l138 + 1)) {
			fRec141[l138] = 0.0;
			
		}
		for (int l139 = 0; (l139 < 2); l139 = (l139 + 1)) {
			fRec139[l139] = 0.0;
			
		}
		for (int l140 = 0; (l140 < 2); l140 = (l140 + 1)) {
			fRec138[l140] = 0.0;
			
		}
		for (int l141 = 0; (l141 < 2); l141 = (l141 + 1)) {
			fRec123[l141] = 0.0;
			
		}
		for (int l142 = 0; (l142 < 2); l142 = (l142 + 1)) {
			fRec142[l142] = 0.0;
			
		}
		for (int l143 = 0; (l143 < 2); l143 = (l143 + 1)) {
			fRec143[l143] = 0.0;
			
		}
		for (int l144 = 0; (l144 < 2); l144 = (l144 + 1)) {
			fRec144[l144] = 0.0;
			
		}
		for (int l145 = 0; (l145 < 2); l145 = (l145 + 1)) {
			fRec145[l145] = 0.0;
			
		}
		for (int l146 = 0; (l146 < 2); l146 = (l146 + 1)) {
			fRec146[l146] = 0.0;
			
		}
		for (int l147 = 0; (l147 < 2); l147 = (l147 + 1)) {
			fRec147[l147] = 0.0;
			
		}
		for (int l148 = 0; (l148 < 2); l148 = (l148 + 1)) {
			fRec148[l148] = 0.0;
			
		}
		for (int l149 = 0; (l149 < 2); l149 = (l149 + 1)) {
			fRec153[l149] = 0.0;
			
		}
		for (int l150 = 0; (l150 < 2); l150 = (l150 + 1)) {
			fRec154[l150] = 0.0;
			
		}
		for (int l151 = 0; (l151 < 2); l151 = (l151 + 1)) {
			fRec152[l151] = 0.0;
			
		}
		for (int l152 = 0; (l152 < 2); l152 = (l152 + 1)) {
			fRec155[l152] = 0.0;
			
		}
		for (int l153 = 0; (l153 < 2); l153 = (l153 + 1)) {
			fRec151[l153] = 0.0;
			
		}
		for (int l154 = 0; (l154 < 2); l154 = (l154 + 1)) {
			fRec150[l154] = 0.0;
			
		}
		for (int l155 = 0; (l155 < 2); l155 = (l155 + 1)) {
			fRec149[l155] = 0.0;
			
		}
		for (int l156 = 0; (l156 < 2); l156 = (l156 + 1)) {
			fRec156[l156] = 0.0;
			
		}
		for (int l157 = 0; (l157 < 2); l157 = (l157 + 1)) {
			fRec157[l157] = 0.0;
			
		}
		for (int l158 = 0; (l158 < 2); l158 = (l158 + 1)) {
			fRec158[l158] = 0.0;
			
		}
		for (int l159 = 0; (l159 < 2); l159 = (l159 + 1)) {
			fRec159[l159] = 0.0;
			
		}
		for (int l160 = 0; (l160 < 2); l160 = (l160 + 1)) {
			fRec160[l160] = 0.0;
			
		}
		for (int l161 = 0; (l161 < 2); l161 = (l161 + 1)) {
			fRec161[l161] = 0.0;
			
		}
		for (int l162 = 0; (l162 < 2); l162 = (l162 + 1)) {
			fRec162[l162] = 0.0;
			
		}
		for (int l163 = 0; (l163 < 2); l163 = (l163 + 1)) {
			fRec163[l163] = 0.0;
			
		}
		for (int l164 = 0; (l164 < 2); l164 = (l164 + 1)) {
			fRec164[l164] = 0.0;
			
		}
		for (int l165 = 0; (l165 < 2); l165 = (l165 + 1)) {
			fRec165[l165] = 0.0;
			
		}
		for (int l166 = 0; (l166 < 2); l166 = (l166 + 1)) {
			fRec166[l166] = 0.0;
			
		}
		for (int l167 = 0; (l167 < 2); l167 = (l167 + 1)) {
			fRec167[l167] = 0.0;
			
		}
		for (int l168 = 0; (l168 < 2); l168 = (l168 + 1)) {
			fRec168[l168] = 0.0;
			
		}
		for (int l169 = 0; (l169 < 2); l169 = (l169 + 1)) {
			fRec169[l169] = 0.0;
			
		}
		for (int l170 = 0; (l170 < 2); l170 = (l170 + 1)) {
			fRec170[l170] = 0.0;
			
		}
		for (int l171 = 0; (l171 < 2); l171 = (l171 + 1)) {
			fRec171[l171] = 0.0;
			
		}
		for (int l172 = 0; (l172 < 2); l172 = (l172 + 1)) {
			fRec172[l172] = 0.0;
			
		}
		for (int l173 = 0; (l173 < 2); l173 = (l173 + 1)) {
			fRec173[l173] = 0.0;
			
		}
		for (int l174 = 0; (l174 < 2); l174 = (l174 + 1)) {
			fRec174[l174] = 0.0;
			
		}
		for (int l175 = 0; (l175 < 2); l175 = (l175 + 1)) {
			fRec175[l175] = 0.0;
			
		}
		for (int l176 = 0; (l176 < 2); l176 = (l176 + 1)) {
			fRec176[l176] = 0.0;
			
		}
		for (int l177 = 0; (l177 < 2); l177 = (l177 + 1)) {
			fRec177[l177] = 0.0;
			
		}
		for (int l178 = 0; (l178 < 2); l178 = (l178 + 1)) {
			fRec178[l178] = 0.0;
			
		}
		for (int l179 = 0; (l179 < 2); l179 = (l179 + 1)) {
			fRec179[l179] = 0.0;
			
		}
		for (int l180 = 0; (l180 < 2); l180 = (l180 + 1)) {
			fRec180[l180] = 0.0;
			
		}
		for (int l181 = 0; (l181 < 2); l181 = (l181 + 1)) {
			fRec181[l181] = 0.0;
			
		}
		for (int l182 = 0; (l182 < 2); l182 = (l182 + 1)) {
			fRec182[l182] = 0.0;
			
		}
		for (int l183 = 0; (l183 < 2); l183 = (l183 + 1)) {
			fRec183[l183] = 0.0;
			
		}
		for (int l184 = 0; (l184 < 2); l184 = (l184 + 1)) {
			fRec184[l184] = 0.0;
			
		}
		for (int l185 = 0; (l185 < 2); l185 = (l185 + 1)) {
			fRec185[l185] = 0.0;
			
		}
		for (int l186 = 0; (l186 < 2); l186 = (l186 + 1)) {
			fRec186[l186] = 0.0;
			
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
		ui_interface->openVerticalBox("HOADecLebedev504");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("B-Format");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3770610", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3845f80", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x377b940", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3804730", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3924110", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38cc830", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x378a1e0", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3890f30", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37f9960", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x382cd50", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x398afa0", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x383c9e0", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x379ccc0", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37c9090", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3882600", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37ecfd0", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3917ec0", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38bfd40", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3907150", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38dcc70", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37b21c0", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38a2ee0", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37dbc70", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38717a0", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3817660", &fVbargraph11, -70.0, 6.0);
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
		ui_interface->openHorizontalBox("Outputs 1-25");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph5, "osc", "/output1 -70 6");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37b7320", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph12, "osc", "/output2 -70 6");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x381d040", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph16, "osc", "/output3 -70 6");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x384b320", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph17, "osc", "/output4 -70 6");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3852d30", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph18, "osc", "/output5 -70 6");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3859130", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph19, "osc", "/output6 -70 6");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x385e8f0", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph24, "osc", "/output7 -70 6");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38ab880", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph28, "osc", "/output8 -70 6");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38e2910", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph29, "osc", "/output9 -70 6");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38eceb0", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph30, "osc", "/output10 -70 6");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38f4660", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph34, "osc", "/output11 -70 6");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x392ba40", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph35, "osc", "/output12 -70 6");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39389b0", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph36, "osc", "/output13 -70 6");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3944240", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph37, "osc", "/output14 -70 6");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x394fc90", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph38, "osc", "/output15 -70 6");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x395ef80", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph39, "osc", "/output16 -70 6");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39691b0", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph40, "osc", "/output17 -70 6");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3972c80", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph41, "osc", "/output18 -70 6");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x397a860", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph43, "osc", "/output19 -70 6");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x399dd50", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph44, "osc", "/output20 -70 6");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39b1660", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph45, "osc", "/output21 -70 6");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39c3aa0", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph46, "osc", "/output22 -70 6");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39d4e50", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph47, "osc", "/output23 -70 6");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39e7650", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph48, "osc", "/output24 -70 6");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39f5e80", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph49, "osc", "/output25 -70 6");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a03600", &fVbargraph49, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph50, "osc", "/output26 -70 6");
		ui_interface->declare(&fVbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a11090", &fVbargraph50, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph51, "osc", "/output27 -70 6");
		ui_interface->declare(&fVbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a27a50", &fVbargraph51, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph52, "osc", "/output28 -70 6");
		ui_interface->declare(&fVbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a36d30", &fVbargraph52, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph53, "osc", "/output29 -70 6");
		ui_interface->declare(&fVbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a47f60", &fVbargraph53, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph54, "osc", "/output30 -70 6");
		ui_interface->declare(&fVbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a58310", &fVbargraph54, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph55, "osc", "/output31 -70 6");
		ui_interface->declare(&fVbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a6d140", &fVbargraph55, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph56, "osc", "/output32 -70 6");
		ui_interface->declare(&fVbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a80a60", &fVbargraph56, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph57, "osc", "/output33 -70 6");
		ui_interface->declare(&fVbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a947f0", &fVbargraph57, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph58, "osc", "/output34 -70 6");
		ui_interface->declare(&fVbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3aa8870", &fVbargraph58, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph59, "osc", "/output35 -70 6");
		ui_interface->declare(&fVbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3abc160", &fVbargraph59, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph60, "osc", "/output36 -70 6");
		ui_interface->declare(&fVbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3aceab0", &fVbargraph60, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph61, "osc", "/output37 -70 6");
		ui_interface->declare(&fVbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3ae3690", &fVbargraph61, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph62, "osc", "/output38 -70 6");
		ui_interface->declare(&fVbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3af8890", &fVbargraph62, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph63, "osc", "/output39 -70 6");
		ui_interface->declare(&fVbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b080f0", &fVbargraph63, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph64, "osc", "/output40 -70 6");
		ui_interface->declare(&fVbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b15050", &fVbargraph64, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph65, "osc", "/output41 -70 6");
		ui_interface->declare(&fVbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b23740", &fVbargraph65, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph66, "osc", "/output42 -70 6");
		ui_interface->declare(&fVbargraph66, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b33530", &fVbargraph66, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph67, "osc", "/output43 -70 6");
		ui_interface->declare(&fVbargraph67, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b40ee0", &fVbargraph67, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph68, "osc", "/output44 -70 6");
		ui_interface->declare(&fVbargraph68, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b4e850", &fVbargraph68, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph69, "osc", "/output45 -70 6");
		ui_interface->declare(&fVbargraph69, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b5e690", &fVbargraph69, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph70, "osc", "/output46 -70 6");
		ui_interface->declare(&fVbargraph70, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b6e9d0", &fVbargraph70, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph71, "osc", "/output47 -70 6");
		ui_interface->declare(&fVbargraph71, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b83380", &fVbargraph71, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph72, "osc", "/output48 -70 6");
		ui_interface->declare(&fVbargraph72, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b90c90", &fVbargraph72, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph73, "osc", "/output49 -70 6");
		ui_interface->declare(&fVbargraph73, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b9e040", &fVbargraph73, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph74, "osc", "/output50 -70 6");
		ui_interface->declare(&fVbargraph74, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3bab370", &fVbargraph74, -70.0, 6.0);
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
		double fSlow19 = (1.0 / (((fConst10 / fSlow2) + (9.1401308902779004 * fSlow10)) + 1.0));
		double fSlow20 = (1.0 / (((fConst11 / fSlow2) + (11.4878004768713 * fSlow10)) + 1.0));
		double fSlow21 = (45.951201907485199 * fSlow10);
		double fSlow22 = (fSlow21 + (fConst12 / fSlow2));
		double fSlow23 = (36.560523561111602 * fSlow10);
		double fSlow24 = (fSlow23 + (fConst13 / fSlow2));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			fRec3[0] = (fSlow6 + (0.999 * fRec3[1]));
			double fTemp0 = (fSlow5 * (fRec3[0] * double(input0[i])));
			fRec2[0] = max((fRec2[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fVbargraph0 = FAUSTFLOAT(fRec2[0]);
			double fTemp1 = (0.012698412698412698 * fTemp0);
			double fTemp2 = (fRec3[0] * double(input2[i]));
			fRec6[0] = (fRec6[1] + fRec5[1]);
			fRec5[0] = (fSlow8 * ((fSlow3 * fTemp2) - (fSlow9 * fRec6[0])));
			double fTemp3 = (fRec5[0] + (fSlow4 * fTemp2));
			fRec4[0] = max((fRec4[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp3))))));
			fVbargraph1 = FAUSTFLOAT(fRec4[0]);
			double fTemp4 = (0.021994295969128601 * fTemp3);
			double fTemp5 = (fRec3[0] * double(input6[i]));
			fRec9[0] = (fRec9[1] + fRec8[1]);
			fRec10[0] = (fRec10[1] + fRec9[1]);
			fRec8[0] = (fSlow11 * ((fSlow3 * fTemp5) - ((fSlow13 * fRec9[0]) + (fSlow12 * fRec10[0]))));
			double fTemp6 = (fRec8[0] + (fSlow4 * fTemp5));
			fRec7[0] = max((fRec7[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fVbargraph2 = FAUSTFLOAT(fRec7[0]);
			double fTemp7 = (0.028394513999997331 * fTemp6);
			double fTemp8 = (fRec3[0] * double(input12[i]));
			fRec14[0] = (fRec14[1] + fRec13[1]);
			fRec15[0] = (fRec15[1] + fRec14[1]);
			fRec13[0] = (fSlow15 * ((fSlow3 * fTemp8) - ((fSlow17 * fRec14[0]) + (fSlow16 * fRec15[0]))));
			fRec16[0] = (fRec16[1] + fRec12[1]);
			fRec12[0] = (fSlow14 * (fRec13[0] - (fSlow18 * fRec16[0])));
			double fTemp9 = (fRec12[0] + (fSlow4 * fTemp8));
			fRec11[0] = max((fRec11[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph3 = FAUSTFLOAT(fRec11[0]);
			double fTemp10 = (0.033596842045264641 * fTemp9);
			double fTemp11 = (fRec3[0] * double(input20[i]));
			fRec20[0] = (fRec20[1] + fRec19[1]);
			fRec21[0] = (fRec21[1] + fRec20[1]);
			fRec19[0] = (fSlow20 * ((fSlow3 * fTemp11) - ((fSlow22 * fRec20[0]) + (fSlow21 * fRec21[0]))));
			fRec22[0] = (fRec22[1] + fRec18[1]);
			fRec23[0] = (fRec23[1] + fRec22[1]);
			fRec18[0] = (fSlow19 * (fRec19[0] - ((fSlow24 * fRec22[0]) + (fSlow23 * fRec23[0]))));
			double fTemp12 = (fRec18[0] + (fSlow4 * fTemp11));
			fRec17[0] = max((fRec17[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph4 = FAUSTFLOAT(fRec17[0]);
			double fTemp13 = (0.038095238095238099 * fTemp12);
			double fTemp14 = (fRec1[0] * ((((fTemp1 + fTemp4) + fTemp7) + fTemp10) + fTemp13));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fVbargraph5 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp14);
			double fTemp15 = (fRec3[0] * double(input13[i]));
			fRec28[0] = (fRec28[1] + fRec27[1]);
			fRec29[0] = (fRec29[1] + fRec28[1]);
			fRec27[0] = (fSlow15 * ((fSlow3 * fTemp15) - ((fSlow17 * fRec28[0]) + (fSlow16 * fRec29[0]))));
			fRec30[0] = (fRec30[1] + fRec26[1]);
			fRec26[0] = (fSlow14 * (fRec27[0] - (fSlow18 * fRec30[0])));
			double fTemp16 = (fRec26[0] + (fSlow4 * fTemp15));
			fRec25[0] = max((fRec25[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph6 = FAUSTFLOAT(fRec25[0]);
			double fTemp17 = (fRec3[0] * double(input22[i]));
			fRec34[0] = (fRec34[1] + fRec33[1]);
			fRec35[0] = (fRec35[1] + fRec34[1]);
			fRec33[0] = (fSlow20 * ((fSlow3 * fTemp17) - ((fSlow22 * fRec34[0]) + (fSlow21 * fRec35[0]))));
			fRec36[0] = (fRec32[1] + fRec36[1]);
			fRec37[0] = (fRec37[1] + fRec36[1]);
			fRec32[0] = (fSlow19 * (fRec33[0] - ((fSlow24 * fRec36[0]) + (fSlow23 * fRec37[0]))));
			double fTemp18 = (fRec32[0] + (fSlow4 * fTemp17));
			fRec31[0] = max((fRec31[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph7 = FAUSTFLOAT(fRec31[0]);
			double fTemp19 = (fRec3[0] * double(input15[i]));
			fRec41[0] = (fRec41[1] + fRec40[1]);
			fRec42[0] = (fRec42[1] + fRec41[1]);
			fRec40[0] = (fSlow15 * ((fSlow3 * fTemp19) - ((fSlow17 * fRec41[0]) + (fSlow16 * fRec42[0]))));
			fRec43[0] = (fRec43[1] + fRec39[1]);
			fRec39[0] = (fSlow14 * (fRec40[0] - (fSlow18 * fRec43[0])));
			double fTemp20 = (fRec39[0] + (fSlow4 * fTemp19));
			fRec38[0] = max((fRec38[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph8 = FAUSTFLOAT(fRec38[0]);
			double fTemp21 = (fRec3[0] * double(input8[i]));
			fRec46[0] = (fRec46[1] + fRec45[1]);
			fRec47[0] = (fRec47[1] + fRec46[1]);
			fRec45[0] = (fSlow11 * ((fSlow3 * fTemp21) - ((fSlow13 * fRec46[0]) + (fSlow12 * fRec47[0]))));
			double fTemp22 = (fRec45[0] + (fSlow4 * fTemp21));
			fRec44[0] = max((fRec44[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph9 = FAUSTFLOAT(fRec44[0]);
			double fTemp23 = (fConst30 * fTemp22);
			double fTemp24 = (fRec3[0] * double(input3[i]));
			fRec50[0] = (fRec50[1] + fRec49[1]);
			fRec49[0] = (fSlow8 * ((fSlow3 * fTemp24) - (fSlow9 * fRec50[0])));
			double fTemp25 = (fRec49[0] + (fSlow4 * fTemp24));
			fRec48[0] = max((fRec48[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph10 = FAUSTFLOAT(fRec48[0]);
			double fTemp26 = (0.014285714285714285 * fTemp12);
			double fTemp27 = (fRec3[0] * double(input24[i]));
			fRec54[0] = (fRec54[1] + fRec53[1]);
			fRec55[0] = (fRec55[1] + fRec54[1]);
			fRec53[0] = (fSlow20 * ((fSlow3 * fTemp27) - ((fSlow22 * fRec54[0]) + (fSlow21 * fRec55[0]))));
			fRec56[0] = (fRec56[1] + fRec52[1]);
			fRec57[0] = (fRec57[1] + fRec56[1]);
			fRec52[0] = (fSlow19 * (fRec53[0] - ((fSlow24 * fRec56[0]) + (fSlow23 * fRec57[0]))));
			double fTemp28 = (fRec52[0] + (fSlow4 * fTemp27));
			fRec51[0] = max((fRec51[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fVbargraph11 = FAUSTFLOAT(fRec51[0]);
			double fTemp29 = (fConst35 * fTemp28);
			double fTemp30 = (0.014197256999998666 * fTemp6);
			double fTemp31 = (fRec1[0] * ((((0.0089791337293529845 * ((fConst18 * fTemp16) + (fConst22 * fTemp18))) + ((fConst27 * fTemp20) + (fTemp23 + ((fConst33 * fTemp25) + (fTemp1 + fTemp26))))) + fTemp29) - fTemp30));
			fRec24[0] = max((fRec24[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph12 = FAUSTFLOAT(fRec24[0]);
			output1[i] = FAUSTFLOAT(fTemp31);
			double fTemp32 = (fConst36 * fTemp22);
			double fTemp33 = (fRec3[0] * double(input9[i]));
			fRec62[0] = (fRec62[1] + fRec61[1]);
			fRec63[0] = (fRec63[1] + fRec62[1]);
			fRec61[0] = (fSlow15 * ((fSlow3 * fTemp33) - ((fSlow17 * fRec62[0]) + (fSlow16 * fRec63[0]))));
			fRec64[0] = (fRec64[1] + fRec60[1]);
			fRec60[0] = (fSlow14 * (fRec61[0] - (fSlow18 * fRec64[0])));
			double fTemp34 = (fRec60[0] + (fSlow4 * fTemp33));
			fRec59[0] = max((fRec59[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fVbargraph13 = FAUSTFLOAT(fRec59[0]);
			double fTemp35 = (fConst38 * fTemp18);
			double fTemp36 = (fRec3[0] * double(input11[i]));
			fRec68[0] = (fRec68[1] + fRec67[1]);
			fRec69[0] = (fRec69[1] + fRec68[1]);
			fRec67[0] = (fSlow15 * ((fSlow3 * fTemp36) - ((fSlow17 * fRec68[0]) + (fSlow16 * fRec69[0]))));
			fRec70[0] = (fRec70[1] + fRec66[1]);
			fRec66[0] = (fSlow14 * (fRec67[0] - (fSlow18 * fRec70[0])));
			double fTemp37 = (fRec66[0] + (fSlow4 * fTemp36));
			fRec65[0] = max((fRec65[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fVbargraph14 = FAUSTFLOAT(fRec65[0]);
			double fTemp38 = (fRec3[0] * double(input1[i]));
			fRec73[0] = (fRec73[1] + fRec72[1]);
			fRec72[0] = (fSlow8 * ((fSlow3 * fTemp38) - (fSlow9 * fRec73[0])));
			double fTemp39 = (fRec72[0] + (fSlow4 * fTemp38));
			fRec71[0] = max((fRec71[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph15 = FAUSTFLOAT(fRec71[0]);
			double fTemp40 = (fTemp26 + fTemp29);
			double fTemp41 = (fRec1[0] * (((0.012698412698412698 * (((fTemp0 + fTemp32) + (fConst37 * fTemp34)) + fTemp35)) + ((fConst39 * fTemp37) + ((fConst33 * fTemp39) + fTemp40))) - fTemp30));
			fRec58[0] = max((fRec58[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph16 = FAUSTFLOAT(fRec58[0]);
			output2[i] = FAUSTFLOAT(fTemp41);
			double fTemp42 = (fRec1[0] * (((0.012698412698412698 * (((fTemp0 + (fConst40 * fTemp25)) + (fConst41 * fTemp16)) + (fConst37 * fTemp20))) + (fTemp29 + ((fConst42 * fTemp18) + (fTemp23 + fTemp26)))) - fTemp30));
			fRec74[0] = max((fRec74[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fVbargraph17 = FAUSTFLOAT(fRec74[0]);
			output3[i] = FAUSTFLOAT(fTemp42);
			double fTemp43 = (fRec1[0] * (((0.012698412698412698 * (fTemp35 + ((fTemp32 + (fTemp0 + (fConst40 * fTemp39))) + (fConst41 * fTemp37)))) + (fTemp40 + (fConst27 * fTemp34))) - fTemp30));
			fRec75[0] = max((fRec75[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph18 = FAUSTFLOAT(fRec75[0]);
			output4[i] = FAUSTFLOAT(fTemp43);
			double fTemp44 = (fRec1[0] * ((fTemp13 + (fTemp1 + fTemp7)) - (fTemp4 + fTemp10)));
			fRec76[0] = max((fRec76[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fVbargraph19 = FAUSTFLOAT(fRec76[0]);
			output5[i] = FAUSTFLOAT(fTemp44);
			double fTemp45 = (fRec3[0] * double(input23[i]));
			fRec81[0] = (fRec81[1] + fRec80[1]);
			fRec82[0] = (fRec82[1] + fRec81[1]);
			fRec80[0] = (fSlow20 * ((fSlow3 * fTemp45) - ((fSlow22 * fRec81[0]) + (fSlow21 * fRec82[0]))));
			fRec83[0] = (fRec83[1] + fRec79[1]);
			fRec84[0] = (fRec84[1] + fRec83[1]);
			fRec79[0] = (fSlow19 * (fRec80[0] - ((fSlow24 * fRec83[0]) + (fSlow23 * fRec84[0]))));
			double fTemp46 = (fRec79[0] + (fSlow4 * fTemp45));
			fRec78[0] = max((fRec78[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph20 = FAUSTFLOAT(fRec78[0]);
			double fTemp47 = (fRec3[0] * double(input14[i]));
			fRec88[0] = (fRec88[1] + fRec87[1]);
			fRec89[0] = (fRec89[1] + fRec88[1]);
			fRec87[0] = (fSlow15 * ((fSlow3 * fTemp47) - ((fSlow17 * fRec88[0]) + (fSlow16 * fRec89[0]))));
			fRec90[0] = (fRec90[1] + fRec86[1]);
			fRec86[0] = (fSlow14 * (fRec87[0] - (fSlow18 * fRec90[0])));
			double fTemp48 = (fRec86[0] + (fSlow4 * fTemp47));
			fRec85[0] = max((fRec85[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph21 = FAUSTFLOAT(fRec85[0]);
			double fTemp49 = (fConst19 * (((fConst47 * fTemp22) + (fConst50 * fTemp48)) + (fConst51 * fTemp18)));
			double fTemp50 = (fRec3[0] * double(input7[i]));
			fRec93[0] = (fRec93[1] + fRec92[1]);
			fRec94[0] = (fRec94[1] + fRec93[1]);
			fRec92[0] = (fSlow11 * ((fSlow3 * fTemp50) - ((fSlow13 * fRec93[0]) + (fSlow12 * fRec94[0]))));
			double fTemp51 = (fRec92[0] + (fSlow4 * fTemp50));
			fRec91[0] = max((fRec91[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph22 = FAUSTFLOAT(fRec91[0]);
			double fTemp52 = (fRec3[0] * double(input21[i]));
			fRec98[0] = (fRec98[1] + fRec97[1]);
			fRec99[0] = (fRec99[1] + fRec98[1]);
			fRec97[0] = (fSlow20 * ((fSlow3 * fTemp52) - ((fSlow22 * fRec98[0]) + (fSlow21 * fRec99[0]))));
			fRec100[0] = (fRec100[1] + fRec96[1]);
			fRec101[0] = (fRec101[1] + fRec100[1]);
			fRec96[0] = (fSlow19 * (fRec97[0] - ((fSlow24 * fRec100[0]) + (fSlow23 * fRec101[0]))));
			double fTemp53 = (fRec96[0] + (fSlow4 * fTemp52));
			fRec95[0] = max((fRec95[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fVbargraph23 = FAUSTFLOAT(fRec95[0]);
			double fTemp54 = (0.022574955908289243 * fTemp0);
			double fTemp55 = (0.027648561470568499 * fTemp3);
			double fTemp56 = (0.01261978399999881 * fTemp6);
			double fTemp57 = (fConst59 * fTemp28);
			double fTemp58 = (0.010558468816293317 * fTemp9);
			double fTemp59 = (0.027513227513227514 * fTemp12);
			double fTemp60 = (fTemp58 + fTemp59);
			double fTemp61 = (fRec1[0] * (((fConst23 * ((fConst43 * fTemp20) + (fConst46 * fTemp46))) + (fTemp49 + ((fConst14 * ((((fConst52 * fTemp25) + (fConst55 * fTemp51)) + (fConst56 * fTemp16)) + (fConst58 * fTemp53))) + (((fTemp54 + fTemp55) + fTemp56) + fTemp57)))) - fTemp60));
			fRec77[0] = max((fRec77[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fVbargraph24 = FAUSTFLOAT(fRec77[0]);
			output6[i] = FAUSTFLOAT(fTemp61);
			double fTemp62 = (fConst60 * fTemp22);
			double fTemp63 = ((fTemp0 + fTemp62) + (fConst61 * fTemp34));
			double fTemp64 = (fConst63 * fTemp48);
			double fTemp65 = (fRec3[0] * double(input17[i]));
			fRec106[0] = (fRec106[1] + fRec105[1]);
			fRec107[0] = (fRec107[1] + fRec106[1]);
			fRec105[0] = (fSlow20 * ((fSlow3 * fTemp65) - ((fSlow22 * fRec106[0]) + (fSlow21 * fRec107[0]))));
			fRec108[0] = (fRec108[1] + fRec104[1]);
			fRec109[0] = (fRec109[1] + fRec108[1]);
			fRec104[0] = (fSlow19 * (fRec105[0] - ((fSlow24 * fRec108[0]) + (fSlow23 * fRec109[0]))));
			double fTemp66 = (fRec104[0] + (fSlow4 * fTemp65));
			fRec103[0] = max((fRec103[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fVbargraph25 = FAUSTFLOAT(fRec103[0]);
			double fTemp67 = (fConst67 * fTemp18);
			double fTemp68 = (fRec3[0] * double(input5[i]));
			fRec112[0] = (fRec112[1] + fRec111[1]);
			fRec113[0] = (fRec113[1] + fRec112[1]);
			fRec111[0] = (fSlow11 * ((fSlow3 * fTemp68) - ((fSlow13 * fRec112[0]) + (fSlow12 * fRec113[0]))));
			double fTemp69 = (fRec111[0] + (fSlow4 * fTemp68));
			fRec110[0] = max((fRec110[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp69))))));
			fVbargraph26 = FAUSTFLOAT(fRec110[0]);
			double fTemp70 = (fRec3[0] * double(input19[i]));
			fRec117[0] = (fRec117[1] + fRec116[1]);
			fRec118[0] = (fRec118[1] + fRec117[1]);
			fRec116[0] = (fSlow20 * ((fSlow3 * fTemp70) - ((fSlow22 * fRec117[0]) + (fSlow21 * fRec118[0]))));
			fRec119[0] = (fRec119[1] + fRec115[1]);
			fRec120[0] = (fRec120[1] + fRec119[1]);
			fRec115[0] = (fSlow19 * (fRec116[0] - ((fSlow24 * fRec119[0]) + (fSlow23 * fRec120[0]))));
			double fTemp71 = (fRec115[0] + (fSlow4 * fTemp70));
			fRec114[0] = max((fRec114[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fVbargraph27 = FAUSTFLOAT(fRec114[0]);
			double fTemp72 = (fTemp57 + (fTemp55 + fTemp56));
			double fTemp73 = (fRec1[0] * (((0.022574955908289243 * (((fTemp63 + fTemp64) + (fConst65 * fTemp66)) + fTemp67)) + ((fConst14 * ((((fConst52 * fTemp39) + (fConst55 * fTemp69)) + (fConst56 * fTemp37)) + (fConst58 * fTemp71))) + fTemp72)) - fTemp60));
			fRec102[0] = max((fRec102[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp73))))));
			fVbargraph28 = FAUSTFLOAT(fRec102[0]);
			output7[i] = FAUSTFLOAT(fTemp73);
			double fTemp74 = (fTemp0 + (fConst68 * fTemp25));
			double fTemp75 = (fConst61 * fTemp20);
			double fTemp76 = (fRec1[0] * (((0.022574955908289243 * (((((fTemp74 + (fConst70 * fTemp51)) + (fConst72 * fTemp16)) + fTemp75) + (fConst74 * fTemp53)) + (fConst65 * fTemp46))) + (fTemp49 + fTemp72)) - fTemp60));
			fRec121[0] = max((fRec121[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp76))))));
			fVbargraph29 = FAUSTFLOAT(fRec121[0]);
			output8[i] = FAUSTFLOAT(fTemp76);
			double fTemp77 = (fTemp0 + (fConst68 * fTemp39));
			double fTemp78 = (fRec1[0] * (((0.022574955908289243 * (fTemp67 + ((fTemp64 + ((fTemp62 + (fTemp77 + (fConst70 * fTemp69))) + (fConst72 * fTemp37))) + (fConst74 * fTemp71)))) + (fTemp72 + (fConst23 * ((fConst43 * fTemp34) + (fConst46 * fTemp66))))) - fTemp60));
			fRec122[0] = max((fRec122[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fVbargraph30 = FAUSTFLOAT(fRec122[0]);
			output9[i] = FAUSTFLOAT(fTemp78);
			double fTemp79 = (fRec3[0] * double(input18[i]));
			fRec127[0] = (fRec127[1] + fRec126[1]);
			fRec128[0] = (fRec128[1] + fRec127[1]);
			fRec126[0] = (fSlow20 * ((fSlow3 * fTemp79) - ((fSlow22 * fRec127[0]) + (fSlow21 * fRec128[0]))));
			fRec129[0] = (fRec129[1] + fRec125[1]);
			fRec130[0] = (fRec130[1] + fRec129[1]);
			fRec125[0] = (fSlow19 * (fRec126[0] - ((fSlow24 * fRec129[0]) + (fSlow23 * fRec130[0]))));
			double fTemp80 = (fRec125[0] + (fSlow4 * fTemp79));
			fRec124[0] = max((fRec124[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp80))))));
			fVbargraph31 = FAUSTFLOAT(fRec124[0]);
			double fTemp81 = (fRec3[0] * double(input16[i]));
			fRec134[0] = (fRec134[1] + fRec133[1]);
			fRec135[0] = (fRec135[1] + fRec134[1]);
			fRec133[0] = (fSlow20 * ((fSlow3 * fTemp81) - ((fSlow22 * fRec134[0]) + (fSlow21 * fRec135[0]))));
			fRec136[0] = (fRec136[1] + fRec132[1]);
			fRec137[0] = (fRec137[1] + fRec136[1]);
			fRec132[0] = (fSlow19 * (fRec133[0] - ((fSlow24 * fRec136[0]) + (fSlow23 * fRec137[0]))));
			double fTemp82 = (fRec132[0] + (fSlow4 * fTemp81));
			fRec131[0] = max((fRec131[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp82))))));
			fVbargraph32 = FAUSTFLOAT(fRec131[0]);
			double fTemp83 = (fRec3[0] * double(input4[i]));
			fRec140[0] = (fRec140[1] + fRec139[1]);
			fRec141[0] = (fRec141[1] + fRec140[1]);
			fRec139[0] = (fSlow11 * ((fSlow3 * fTemp83) - ((fSlow13 * fRec140[0]) + (fSlow12 * fRec141[0]))));
			double fTemp84 = (fRec139[0] + (fSlow4 * fTemp83));
			fRec138[0] = max((fRec138[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp84))))));
			fVbargraph33 = FAUSTFLOAT(fRec138[0]);
			double fTemp85 = (0.025396825396825397 * fTemp12);
			double fTemp86 = (0.025239567999997631 * fTemp6);
			double fTemp87 = (fRec1[0] * (((0.022574955908289243 * ((fTemp0 + (fConst75 * fTemp20)) + (fConst76 * fTemp28))) + ((fConst22 * ((3.5444768026723675e-18 * fTemp18) + (0.015962904407738641 * fTemp80))) + ((fConst77 * fTemp82) + ((fConst18 * ((0.011287477954144623 * fTemp16) + (0.011287477954144622 * fTemp37))) + ((fConst29 * ((1.5998055129555246e-17 * fTemp22) + (0.072048835120117657 * fTemp84))) + ((fConst32 * ((0.025473109945013343 * fTemp25) + (0.025473109945013336 * fTemp39))) + ((fConst78 * fTemp34) + fTemp85))))))) - fTemp86));
			fRec123[0] = max((fRec123[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp87))))));
			fVbargraph34 = FAUSTFLOAT(fRec123[0]);
			output10[i] = FAUSTFLOAT(fTemp87);
			double fTemp88 = (fConst81 * fTemp39);
			double fTemp89 = (fConst36 * fTemp84);
			double fTemp90 = (fConst38 * fTemp80);
			double fTemp91 = (fConst86 * fTemp28);
			double fTemp92 = (fRec1[0] * ((((fConst26 * ((0.10189243978005338 * fTemp20) + (0.1018924397800533 * fTemp34))) + ((fConst79 * fTemp82) + ((fConst80 * fTemp37) + (fTemp88 + fTemp85)))) + (0.022574955908289243 * (((((((fTemp0 + (fConst82 * fTemp25)) + fTemp89) + (fConst83 * fTemp22)) + (fConst84 * fTemp16)) + fTemp90) + (fConst85 * fTemp18)) + fTemp91))) - fTemp86));
			fRec142[0] = max((fRec142[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp92))))));
			fVbargraph35 = FAUSTFLOAT(fRec142[0]);
			output11[i] = FAUSTFLOAT(fTemp92);
			double fTemp93 = (fRec1[0] * (((0.022574955908289243 * (fTemp91 + (((((fTemp0 + (fConst82 * fTemp39)) + (fConst87 * fTemp25)) + (fConst88 * fTemp34)) + (fConst84 * fTemp37)) + (fConst89 * fTemp16)))) + ((fConst22 * ((7.088953605344735e-18 * fTemp18) + (0.015962904407738645 * fTemp80))) + ((fConst29 * ((3.1996110259110492e-17 * fTemp22) + (0.072048835120117671 * fTemp84))) + ((fConst90 * fTemp82) + (fTemp85 + (fConst91 * fTemp20)))))) - fTemp86));
			fRec143[0] = max((fRec143[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fVbargraph36 = FAUSTFLOAT(fRec143[0]);
			output12[i] = FAUSTFLOAT(fTemp93);
			double fTemp94 = (fRec1[0] * ((((fConst92 * fTemp82) + ((fConst93 * fTemp16) + (fTemp85 + (fConst94 * fTemp25)))) + (0.022574955908289243 * (fTemp91 + ((fTemp90 + (((((fTemp89 + (fTemp0 + (fConst87 * fTemp39))) + (fConst95 * fTemp22)) + (fConst96 * fTemp34)) + (fConst89 * fTemp37)) + (fConst97 * fTemp20))) + (fConst98 * fTemp18))))) - fTemp86));
			fRec144[0] = max((fRec144[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp94))))));
			fVbargraph37 = FAUSTFLOAT(fRec144[0]);
			output13[i] = FAUSTFLOAT(fTemp94);
			double fTemp95 = (fConst110 * fTemp22);
			double fTemp96 = (fTemp55 + fTemp59);
			double fTemp97 = (fRec1[0] * (((0.031925808815477283 * (((fConst100 * fTemp51) + (fConst102 * fTemp48)) + (fConst103 * fTemp46))) + ((0.015962904407738641 * ((fConst105 * fTemp16) + (fConst106 * fTemp18))) + ((fConst108 * fTemp53) + (fTemp57 + ((fConst109 * fTemp20) + (fTemp95 + ((fConst81 * fTemp25) + (fTemp58 + (fTemp54 + fTemp56))))))))) - fTemp96));
			fRec145[0] = max((fRec145[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp97))))));
			fVbargraph38 = FAUSTFLOAT(fRec145[0]);
			output14[i] = FAUSTFLOAT(fTemp97);
			double fTemp98 = (fConst111 * fTemp48);
			double fTemp99 = (fConst113 * fTemp18);
			double fTemp100 = (fTemp56 + fTemp58);
			double fTemp101 = (fTemp57 + fTemp100);
			double fTemp102 = (fRec1[0] * (((0.022574955908289243 * (((fTemp63 + fTemp98) + (fConst112 * fTemp66)) + fTemp99)) + ((fConst108 * fTemp71) + ((fConst114 * fTemp37) + ((fConst115 * fTemp69) + (fTemp88 + fTemp101))))) - fTemp96));
			fRec146[0] = max((fRec146[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp102))))));
			fVbargraph39 = FAUSTFLOAT(fRec146[0]);
			output15[i] = FAUSTFLOAT(fTemp102);
			double fTemp103 = (fRec1[0] * (((0.022574955908289243 * (((fTemp75 + ((fTemp74 + (fConst116 * fTemp51)) + (fConst117 * fTemp16))) + (fConst118 * fTemp53)) + (fConst112 * fTemp46))) + ((fConst119 * fTemp18) + ((fConst120 * fTemp48) + (fTemp57 + (fTemp95 + fTemp100))))) - fTemp96));
			fRec147[0] = max((fRec147[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp103))))));
			fVbargraph40 = FAUSTFLOAT(fRec147[0]);
			output16[i] = FAUSTFLOAT(fTemp103);
			double fTemp104 = (fRec1[0] * (((0.022574955908289243 * (fTemp99 + ((fTemp98 + ((fTemp62 + (fTemp77 + (fConst116 * fTemp69))) + (fConst117 * fTemp37))) + (fConst118 * fTemp71)))) + ((fConst121 * fTemp66) + ((fConst109 * fTemp34) + fTemp101))) - fTemp96));
			fRec148[0] = max((fRec148[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp104))))));
			fVbargraph41 = FAUSTFLOAT(fRec148[0]);
			output17[i] = FAUSTFLOAT(fTemp104);
			double fTemp105 = ((9.9655853669580557e-18 * fTemp22) + (0.044881006545161167 * fTemp84));
			double fTemp106 = (fRec3[0] * double(input10[i]));
			fRec153[0] = (fRec153[1] + fRec152[1]);
			fRec154[0] = (fRec154[1] + fRec153[1]);
			fRec152[0] = (fSlow15 * ((fSlow3 * fTemp106) - ((fSlow17 * fRec153[0]) + (fSlow16 * fRec154[0]))));
			fRec155[0] = (fRec155[1] + fRec151[1]);
			fRec151[0] = (fSlow14 * (fRec152[0] - (fSlow18 * fRec155[0])));
			double fTemp107 = (fRec151[0] + (fSlow4 * fTemp106));
			fRec150[0] = max((fRec150[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp107))))));
			fVbargraph42 = FAUSTFLOAT(fRec150[0]);
			double fTemp108 = ((0.007031250000000001 * fTemp53) + (0.0070312500000000002 * fTemp71));
			double fTemp109 = ((0.019434045907762623 * fTemp25) + (0.019434045907762619 * fTemp39));
			double fTemp110 = (fConst129 * fTemp69);
			double fTemp111 = (5.236595479505967e-18 * fTemp6);
			double fTemp112 = (fConst132 * fTemp82);
			double fTemp113 = (fTemp0 + fTemp3);
			double fTemp114 = (fConst133 * fTemp20);
			double fTemp115 = (fConst135 * fTemp28);
			double fTemp116 = (0.021480823570105497 * fTemp9);
			double fTemp117 = (0.024609375000000006 * fTemp12);
			double fTemp118 = (fTemp116 + fTemp117);
			double fTemp119 = (fRec1[0] * ((((fConst19 * (((((fConst28 * fTemp105) + (fConst122 * fTemp107)) + (fConst123 * fTemp48)) + (fConst124 * fTemp80)) + (fConst125 * fTemp18))) + ((fConst126 * fTemp108) + ((fConst23 * ((fConst127 * fTemp34) + (fConst128 * fTemp66))) + ((fConst14 * (((((fConst31 * fTemp109) + fTemp110) + (fConst129 * fTemp51)) + (fConst130 * fTemp37)) + (fConst131 * fTemp16))) + (fTemp111 + fTemp112))))) + (0.021093750000000001 * (((fTemp113 + fTemp114) + (fConst134 * fTemp46)) + fTemp115))) - fTemp118));
			fRec149[0] = max((fRec149[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp119))))));
			fVbargraph43 = FAUSTFLOAT(fRec149[0]);
			output18[i] = FAUSTFLOAT(fTemp119);
			double fTemp120 = (fConst136 * fTemp25);
			double fTemp121 = (fConst137 * fTemp84);
			double fTemp122 = (fConst139 * fTemp22);
			double fTemp123 = (fConst140 * fTemp107);
			double fTemp124 = (fConst143 * fTemp80);
			double fTemp125 = (fConst146 * fTemp28);
			double fTemp126 = (fConst148 * fTemp82);
			double fTemp127 = ((0.051824122420700336 * fTemp20) + (0.051824122420700287 * fTemp34));
			double fTemp128 = (fRec1[0] * (((0.021093750000000001 * (((((((((((fTemp113 + fTemp120) + fTemp121) + (fConst138 * fTemp51)) + fTemp122) + fTemp123) + (fConst141 * fTemp16)) + (fConst142 * fTemp48)) + fTemp124) + (fConst144 * fTemp53)) + (fConst145 * fTemp18)) + fTemp125)) + (((fConst14 * ((fTemp110 + (fConst147 * fTemp39)) + (fConst131 * fTemp37))) + ((fTemp111 + fTemp126) + (fConst149 * fTemp71))) + (fConst23 * (((fConst25 * fTemp127) + (fConst150 * fTemp66)) + (fConst128 * fTemp46))))) - fTemp118));
			fRec156[0] = max((fRec156[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp128))))));
			fVbargraph44 = FAUSTFLOAT(fRec156[0]);
			output19[i] = FAUSTFLOAT(fTemp128);
			double fTemp129 = (fTemp0 + (fConst136 * fTemp39));
			double fTemp130 = (fConst151 * fTemp25);
			double fTemp131 = (fConst153 * fTemp34);
			double fTemp132 = ((1.9931170733916111e-17 * fTemp22) + (0.044881006545161174 * fTemp84));
			double fTemp133 = (fConst163 * fTemp82);
			double fTemp134 = (fRec1[0] * (((0.021093750000000001 * (fTemp125 + ((((((((((fTemp3 + fTemp129) + fTemp130) + (fConst138 * fTemp69)) + (fConst152 * fTemp51)) + fTemp131) + (fConst141 * fTemp37)) + (fConst154 * fTemp16)) + (fConst155 * fTemp66)) + (fConst144 * fTemp71)) + (fConst156 * fTemp53)))) + ((fConst23 * ((fConst157 * fTemp20) + (fConst158 * fTemp46))) + ((fConst19 * (((((fConst28 * fTemp132) + (fConst159 * fTemp107)) + (fConst160 * fTemp48)) + (fConst161 * fTemp80)) + (fConst162 * fTemp18))) + (fTemp111 + fTemp133)))) - fTemp118));
			fRec157[0] = max((fRec157[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp134))))));
			fVbargraph45 = FAUSTFLOAT(fRec157[0]);
			output20[i] = FAUSTFLOAT(fTemp134);
			double fTemp135 = (fConst167 * fTemp82);
			double fTemp136 = (fTemp0 + (fConst151 * fTemp39));
			double fTemp137 = (fConst169 * fTemp22);
			double fTemp138 = (fConst170 * fTemp34);
			double fTemp139 = (fConst172 * fTemp20);
			double fTemp140 = (fRec1[0] * ((((fConst14 * (((fConst164 * fTemp25) + (fConst165 * fTemp51)) + (fConst166 * fTemp16))) + ((fTemp111 + fTemp135) + (fConst168 * fTemp53))) + (0.021093750000000001 * (fTemp125 + ((((fTemp124 + (((((fTemp123 + ((((fTemp121 + (fTemp3 + fTemp136)) + (fConst152 * fTemp69)) + fTemp137) + fTemp138)) + (fConst154 * fTemp37)) + (fConst171 * fTemp48)) + fTemp139) + (fConst173 * fTemp66))) + (fConst156 * fTemp71)) + (fConst174 * fTemp18)) + (fConst175 * fTemp46))))) - fTemp118));
			fRec158[0] = max((fRec158[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp140))))));
			fVbargraph46 = FAUSTFLOAT(fRec158[0]);
			output21[i] = FAUSTFLOAT(fTemp140);
			double fTemp141 = (fTemp111 + fTemp116);
			double fTemp142 = (fTemp0 - fTemp3);
			double fTemp143 = (fRec1[0] * ((((fConst177 * ((3.3119137800360884e-18 * fTemp18) + (0.014915533665653739 * fTemp80))) + ((fConst180 * fTemp108) + (((fConst181 * ((6.6238275600721768e-18 * fTemp48) + (0.029831067331307477 * fTemp107))) + ((fConst182 * ((0.010546875000000002 * fTemp16) + (0.010546875000000001 * fTemp37))) + ((fConst184 * fTemp51) + ((fConst29 * fTemp105) + ((fConst32 * fTemp109) + (fTemp112 + ((fConst185 * fTemp34) + fTemp141))))))) + (fConst187 * fTemp66)))) + (0.021093750000000001 * (fTemp115 + ((fTemp114 + (fTemp142 + (fConst183 * fTemp69))) + (fConst188 * fTemp46))))) - fTemp117));
			fRec159[0] = max((fRec159[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp143))))));
			fVbargraph47 = FAUSTFLOAT(fRec159[0]);
			output22[i] = FAUSTFLOAT(fTemp143);
			double fTemp144 = (fConst190 * fTemp107);
			double fTemp145 = (fConst193 * fTemp80);
			double fTemp146 = (fRec1[0] * (((0.021093750000000001 * (fTemp125 + (((((((fTemp122 + ((fTemp121 + (fTemp120 + fTemp142)) + (fConst189 * fTemp51))) + fTemp144) + (fConst191 * fTemp16)) + (fConst192 * fTemp48)) + fTemp145) + (fConst194 * fTemp53)) + (fConst195 * fTemp18)))) + ((fConst186 * ((0.021093749999999994 * fTemp66) + (0.021093750000000012 * fTemp46))) + (((fConst196 * fTemp37) + ((fConst184 * fTemp69) + ((fConst26 * fTemp127) + (fTemp126 + ((fConst197 * fTemp39) + fTemp141))))) + (fConst198 * fTemp71)))) - fTemp117));
			fRec160[0] = max((fRec160[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp146))))));
			fVbargraph48 = FAUSTFLOAT(fRec160[0]);
			output23[i] = FAUSTFLOAT(fTemp146);
			double fTemp147 = (fRec1[0] * (((0.021093750000000001 * (fTemp125 + ((((((fTemp131 + (((fTemp130 - (fTemp3 - fTemp129)) + (fConst189 * fTemp69)) + (fConst199 * fTemp51))) + (fConst191 * fTemp37)) + (fConst200 * fTemp16)) + (fConst201 * fTemp66)) + (fConst194 * fTemp71)) + (fConst202 * fTemp53)))) + ((fConst177 * ((6.6238275600721768e-18 * fTemp18) + (0.01491553366565374 * fTemp80))) + ((fConst181 * ((1.3247655120144354e-17 * fTemp48) + (0.029831067331307481 * fTemp107))) + (((fConst29 * fTemp132) + (fTemp133 + ((fConst203 * fTemp20) + fTemp141))) + (fConst204 * fTemp46))))) - fTemp117));
			fRec161[0] = max((fRec161[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp147))))));
			fVbargraph49 = FAUSTFLOAT(fRec161[0]);
			output24[i] = FAUSTFLOAT(fTemp147);
			double fTemp148 = (fRec1[0] * (((((fConst205 * fTemp16) + ((fConst206 * fTemp51) + (fTemp135 + ((fConst207 * fTemp25) + fTemp141)))) + (fConst208 * fTemp53)) + (0.021093750000000001 * (fTemp125 + ((((fTemp145 + ((fTemp139 + (((fTemp144 + (fTemp138 + (fTemp137 + ((fTemp121 - (fTemp3 - fTemp136)) + (fConst199 * fTemp69))))) + (fConst200 * fTemp37)) + (fConst209 * fTemp48))) + (fConst210 * fTemp66))) + (fConst202 * fTemp71)) + (fConst211 * fTemp18)) + (fConst212 * fTemp46))))) - fTemp117));
			fRec162[0] = max((fRec162[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp148))))));
			fVbargraph50 = FAUSTFLOAT(fRec162[0]);
			output25[i] = FAUSTFLOAT(fTemp148);
			double fTemp149 = ((0.0097062489627452956 * fTemp25) + (0.0097062489627452973 * fTemp39));
			double fTemp150 = (fConst213 * fTemp69);
			double fTemp151 = (fConst215 * fTemp37);
			double fTemp152 = (fConst217 * fTemp71);
			double fTemp153 = (fConst223 * fTemp66);
			double fTemp154 = (0.031605542669238249 * fTemp3);
			double fTemp155 = (0.032806508796871948 * fTemp6);
			double fTemp156 = (0.026333598483989317 * fTemp9);
			double fTemp157 = (0.0142547123015874 * fTemp12);
			double fTemp158 = (((fTemp154 + fTemp155) + fTemp156) + fTemp157);
			double fTemp159 = (fConst224 * fTemp22);
			double fTemp160 = (fTemp0 + fTemp159);
			double fTemp161 = (fConst225 * fTemp48);
			double fTemp162 = (fConst226 * fTemp20);
			double fTemp163 = (fConst227 * fTemp82);
			double fTemp164 = (fConst228 * fTemp18);
			double fTemp165 = (fConst230 * fTemp28);
			double fTemp166 = (fRec1[0] * (((fConst14 * (((((((fConst31 * fTemp149) + fTemp150) + (fConst214 * fTemp51)) + fTemp151) + (fConst216 * fTemp16)) + fTemp152) + (fConst218 * fTemp53))) + ((fConst19 * (((fConst219 * fTemp107) + (fConst220 * fTemp84)) + (fConst221 * fTemp80))) + ((fConst23 * ((fConst222 * fTemp34) + fTemp153)) + fTemp158))) + (0.02017333553791887 * ((((((fTemp160 + fTemp161) + fTemp162) + fTemp163) + fTemp164) + (fConst229 * fTemp46)) + fTemp165))));
			fRec163[0] = max((fRec163[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp166))))));
			fVbargraph51 = FAUSTFLOAT(fRec163[0]);
			output26[i] = FAUSTFLOAT(fTemp166);
			double fTemp167 = (fConst232 * fTemp84);
			double fTemp168 = ((fTemp0 + (fConst231 * fTemp25)) + fTemp167);
			double fTemp169 = (fConst234 * fTemp107);
			double fTemp170 = (fConst236 * fTemp80);
			double fTemp171 = ((0.0070590901547238384 * fTemp20) + (0.0070590901547238332 * fTemp34));
			double fTemp172 = (fConst240 * fTemp82);
			double fTemp173 = (fRec1[0] * ((0.02017333553791887 * (fTemp165 + (fTemp164 + (((fTemp161 + (((fTemp159 + (fTemp168 + (fConst233 * fTemp51))) + fTemp169) + (fConst235 * fTemp16))) + fTemp170) + (fConst237 * fTemp53))))) + ((fConst23 * ((fTemp153 + (fConst25 * fTemp171)) + (fConst238 * fTemp46))) + ((fConst14 * (fTemp152 + (fTemp151 + (fTemp150 + (fConst239 * fTemp39))))) + (fTemp158 + fTemp172)))));
			fRec164[0] = max((fRec164[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp173))))));
			fVbargraph52 = FAUSTFLOAT(fRec164[0]);
			output27[i] = FAUSTFLOAT(fTemp173);
			double fTemp174 = ((fTemp0 + (fConst231 * fTemp39)) + (fConst241 * fTemp25));
			double fTemp175 = (fConst243 * fTemp34);
			double fTemp176 = ((5.19858676206982e-18 * fTemp22) + (0.011706176702255421 * fTemp84));
			double fTemp177 = (fConst253 * fTemp82);
			double fTemp178 = (fRec1[0] * ((0.02017333553791887 * (fTemp165 + ((((((((fTemp174 + (fConst233 * fTemp69)) + (fConst242 * fTemp51)) + fTemp175) + (fConst235 * fTemp37)) + (fConst244 * fTemp16)) + (fConst245 * fTemp66)) + (fConst237 * fTemp71)) + (fConst246 * fTemp53)))) + ((fConst23 * ((fConst247 * fTemp20) + (fConst248 * fTemp46))) + ((fConst19 * (((((fConst28 * fTemp176) + (fConst249 * fTemp107)) + (fConst250 * fTemp48)) + (fConst251 * fTemp80)) + (fConst252 * fTemp18))) + (fTemp158 + fTemp177)))));
			fRec165[0] = max((fRec165[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp178))))));
			fVbargraph53 = FAUSTFLOAT(fRec165[0]);
			output28[i] = FAUSTFLOAT(fTemp178);
			double fTemp179 = (fConst254 * fTemp82);
			double fTemp180 = (fTemp167 + (fTemp0 + (fConst241 * fTemp39)));
			double fTemp181 = (fConst259 * fTemp22);
			double fTemp182 = (fConst260 * fTemp34);
			double fTemp183 = (fConst262 * fTemp20);
			double fTemp184 = (fRec1[0] * (((fTemp158 + fTemp179) + (fConst14 * ((((fConst255 * fTemp25) + (fConst256 * fTemp51)) + (fConst257 * fTemp16)) + (fConst258 * fTemp53)))) + (0.02017333553791887 * (fTemp165 + ((((fTemp170 + (((((fTemp169 + (((fTemp180 + (fConst242 * fTemp69)) + fTemp181) + fTemp182)) + (fConst244 * fTemp37)) + (fConst261 * fTemp48)) + fTemp183) + (fConst263 * fTemp66))) + (fConst246 * fTemp71)) + (fConst264 * fTemp18)) + (fConst265 * fTemp46))))));
			fRec166[0] = max((fRec166[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp184))))));
			fVbargraph54 = FAUSTFLOAT(fRec166[0]);
			output29[i] = FAUSTFLOAT(fTemp184);
			double fTemp185 = (0.20432599334845877 * fTemp82);
			double fTemp186 = (fConst34 * ((0.059595081393300019 * fTemp28) + fTemp185));
			double fTemp187 = ((0.063531811392514598 * fTemp20) + (0.091768172011410243 * fTemp34));
			double fTemp188 = ((0.011411761886411192 * fTemp18) + (0.008558821414808411 * fTemp80));
			double fTemp189 = ((0.0090217899191397753 * fTemp53) + (0.0030072633063799288 * fTemp71));
			double fTemp190 = ((0.046824706809021746 * fTemp22) + (0.035118530106766375 * fTemp84));
			double fTemp191 = (fConst272 * fTemp48);
			double fTemp192 = ((0.013532684878709662 * fTemp16) + (0.0045108949595698937 * fTemp37));
			double fTemp193 = ((0.029118746888235916 * fTemp25) + (0.0097062489627453199 * fTemp39));
			double fTemp194 = (0.02017333553791887 * fTemp0);
			double fTemp195 = (0.010535180889746075 * fTemp3);
			double fTemp196 = (0.0042514054232804339 * fTemp12);
			double fTemp197 = (0.01640325439843596 * fTemp6);
			double fTemp198 = (0.020481687709769415 * fTemp9);
			double fTemp199 = (fTemp197 + fTemp198);
			double fTemp200 = (fRec1[0] * ((fTemp186 + ((fConst23 * (((fConst25 * fTemp187) + (fConst266 * fTemp66)) + (fConst267 * fTemp46))) + ((fConst268 * fTemp188) + ((fConst270 * fTemp189) + ((fConst19 * (((fConst28 * fTemp190) + (fConst271 * fTemp107)) + fTemp191)) + ((fConst273 * fTemp192) + ((fConst14 * (((fConst31 * fTemp193) + (fConst274 * fTemp69)) + (fConst275 * fTemp51))) + ((fTemp194 + fTemp195) + fTemp196)))))))) - fTemp199));
			fRec167[0] = max((fRec167[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp200))))));
			fVbargraph55 = FAUSTFLOAT(fRec167[0]);
			output30[i] = FAUSTFLOAT(fTemp200);
			double fTemp201 = ((fTemp0 + (fConst276 * fTemp22)) + (fConst277 * fTemp34));
			double fTemp202 = (fConst279 * fTemp20);
			double fTemp203 = (fConst280 * fTemp82);
			double fTemp204 = (fConst282 * fTemp18);
			double fTemp205 = ((0.0030072633063799254 * fTemp53) + (0.0090217899191397753 * fTemp71));
			double fTemp206 = ((0.0045108949595698885 * fTemp16) + (0.013532684878709664 * fTemp37));
			double fTemp207 = ((0.0097062489627453077 * fTemp25) + (0.029118746888235923 * fTemp39));
			double fTemp208 = (fTemp195 + fTemp196);
			double fTemp209 = (fConst289 * fTemp28);
			double fTemp210 = (fRec1[0] * (((0.02017333553791887 * ((((((fTemp201 + (fConst278 * fTemp48)) + fTemp202) + fTemp203) + (fConst281 * fTemp66)) + fTemp204) + (fConst283 * fTemp46))) + ((fConst270 * fTemp205) + ((fConst273 * fTemp206) + ((fConst19 * ((fConst284 * fTemp107) + (fConst285 * fTemp84))) + ((fConst14 * (((fConst31 * fTemp207) + (fConst286 * fTemp69)) + (fConst287 * fTemp51))) + ((fConst288 * fTemp80) + (fTemp208 + fTemp209))))))) - fTemp199));
			fRec168[0] = max((fRec168[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp210))))));
			fVbargraph56 = FAUSTFLOAT(fRec168[0]);
			output31[i] = FAUSTFLOAT(fTemp210);
			double fTemp211 = (fConst34 * ((0.059595081393300539 * fTemp28) + (0.20432599334845869 * fTemp82)));
			double fTemp212 = ((fTemp0 + (fConst292 * fTemp25)) + (fConst293 * fTemp84));
			double fTemp213 = (fConst295 * fTemp22);
			double fTemp214 = (fConst296 * fTemp34);
			double fTemp215 = (fRec1[0] * ((fTemp211 + ((fConst23 * ((fConst290 * fTemp20) + (fConst291 * fTemp46))) + ((0.02017333553791887 * (fTemp204 + (((((((((fTemp212 + (fConst294 * fTemp51)) + fTemp213) + fTemp214) + (fConst297 * fTemp107)) + (fConst298 * fTemp16)) + (fConst299 * fTemp48)) + (fConst300 * fTemp66)) + (fConst301 * fTemp80)) + (fConst302 * fTemp53)))) + ((fConst14 * ((fConst303 * fTemp39) + (fConst304 * fTemp69))) + ((fConst305 * fTemp71) + (fTemp208 + (fConst306 * fTemp37))))))) - fTemp199));
			fRec169[0] = max((fRec169[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp215))))));
			fVbargraph57 = FAUSTFLOAT(fRec169[0]);
			output32[i] = FAUSTFLOAT(fTemp215);
			double fTemp216 = ((fTemp0 + (fConst308 * fTemp25)) + (fConst309 * fTemp84));
			double fTemp217 = (fConst313 * fTemp20);
			double fTemp218 = (fConst314 * fTemp82);
			double fTemp219 = (fConst325 * fTemp28);
			double fTemp220 = (fRec1[0] * (((0.02017333553791887 * ((fConst307 * fTemp46) + (((((((fTemp216 + (fConst310 * fTemp51)) + (fConst311 * fTemp107)) + (fConst312 * fTemp16)) + fTemp217) + fTemp218) + (fConst315 * fTemp80)) + (fConst316 * fTemp53)))) + ((fConst23 * ((fConst317 * fTemp34) + (fConst318 * fTemp66))) + ((fConst19 * (fTemp191 + (fConst319 * fTemp22))) + ((fConst14 * ((fConst320 * fTemp39) + (fConst321 * fTemp69))) + ((fConst322 * fTemp18) + ((fConst323 * fTemp71) + ((fConst324 * fTemp37) + (fTemp219 + fTemp208)))))))) - fTemp199));
			fRec170[0] = max((fRec170[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp220))))));
			fVbargraph58 = FAUSTFLOAT(fRec170[0]);
			output33[i] = FAUSTFLOAT(fTemp220);
			double fTemp221 = (fConst34 * (fTemp185 + (0.059595081393300352 * fTemp28)));
			double fTemp222 = ((fTemp0 + (fConst326 * fTemp39)) + (fConst327 * fTemp25));
			double fTemp223 = (fConst330 * fTemp34);
			double fTemp224 = (fConst333 * fTemp20);
			double fTemp225 = ((0.011411761886411199 * fTemp18) + (0.0085588214148084076 * fTemp80));
			double fTemp226 = ((0.046824706809021774 * fTemp22) + (0.035118530106766362 * fTemp84));
			double fTemp227 = (fRec1[0] * ((fTemp221 + ((0.02017333553791887 * ((((((((((fTemp222 + (fConst328 * fTemp69)) + (fConst329 * fTemp51)) + fTemp223) + (fConst331 * fTemp37)) + (fConst332 * fTemp16)) + fTemp224) + (fConst334 * fTemp66)) + (fConst335 * fTemp71)) + (fConst336 * fTemp53)) + (fConst337 * fTemp46))) + ((fConst268 * fTemp225) + (fTemp208 + (fConst19 * (((fConst28 * fTemp226) + (fConst338 * fTemp107)) + (fConst339 * fTemp48))))))) - fTemp199));
			fRec171[0] = max((fRec171[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp227))))));
			fVbargraph59 = FAUSTFLOAT(fRec171[0]);
			output34[i] = FAUSTFLOAT(fTemp227);
			double fTemp228 = ((0.091768172011410187 * fTemp20) + (0.063531811392514695 * fTemp34));
			double fTemp229 = ((fTemp0 + (fConst327 * fTemp39)) + (fConst342 * fTemp25));
			double fTemp230 = (fConst344 * fTemp22);
			double fTemp231 = (fConst347 * fTemp82);
			double fTemp232 = (fConst353 * fTemp28);
			double fTemp233 = (fRec1[0] * (((fConst23 * (((fConst25 * fTemp228) + (fConst340 * fTemp66)) + (fConst341 * fTemp46))) + ((0.02017333553791887 * ((((((((((fTemp229 + (fConst329 * fTemp69)) + (fConst343 * fTemp51)) + fTemp230) + (fConst332 * fTemp37)) + (fConst345 * fTemp16)) + (fConst346 * fTemp48)) + fTemp231) + (fConst336 * fTemp71)) + (fConst348 * fTemp53)) + (fConst349 * fTemp18))) + ((fConst19 * ((fConst350 * fTemp107) + (fConst351 * fTemp84))) + ((fConst352 * fTemp80) + (fTemp208 + fTemp232))))) - fTemp199));
			fRec172[0] = max((fRec172[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp233))))));
			fVbargraph60 = FAUSTFLOAT(fRec172[0]);
			output35[i] = FAUSTFLOAT(fTemp233);
			double fTemp234 = (fConst34 * ((0.059595081393300636 * fTemp28) + (0.20432599334845863 * fTemp82)));
			double fTemp235 = ((fTemp0 + (fConst355 * fTemp39)) + (fConst356 * fTemp84));
			double fTemp236 = (fConst358 * fTemp22);
			double fTemp237 = (fConst361 * fTemp20);
			double fTemp238 = (fRec1[0] * ((fTemp234 + ((0.02017333553791887 * (((((((fConst354 * fTemp48) + ((((fTemp235 + (fConst357 * fTemp69)) + fTemp236) + (fConst359 * fTemp107)) + (fConst360 * fTemp37))) + fTemp237) + (fConst362 * fTemp80)) + (fConst363 * fTemp71)) + (fConst364 * fTemp18)) + (fConst365 * fTemp46))) + ((fConst23 * ((fConst366 * fTemp34) + (fConst367 * fTemp66))) + ((fConst14 * ((fConst368 * fTemp25) + (fConst369 * fTemp51))) + ((fConst370 * fTemp53) + (fTemp208 + (fConst371 * fTemp16))))))) - fTemp199));
			fRec173[0] = max((fRec173[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp238))))));
			fVbargraph61 = FAUSTFLOAT(fRec173[0]);
			output36[i] = FAUSTFLOAT(fTemp238);
			double fTemp239 = ((fTemp0 + (fConst374 * fTemp39)) + (fConst375 * fTemp84));
			double fTemp240 = (fConst377 * fTemp34);
			double fTemp241 = (fConst380 * fTemp82);
			double fTemp242 = (fConst391 * fTemp28);
			double fTemp243 = (fRec1[0] * (((fConst23 * ((fConst372 * fTemp20) + (fConst373 * fTemp46))) + ((0.02017333553791887 * ((((((((fTemp239 + (fConst376 * fTemp69)) + fTemp240) + (fConst378 * fTemp107)) + (fConst379 * fTemp37)) + fTemp241) + (fConst381 * fTemp66)) + (fConst382 * fTemp80)) + (fConst383 * fTemp71))) + ((fConst19 * ((fConst384 * fTemp22) + (fConst385 * fTemp48))) + ((fConst14 * ((fConst386 * fTemp25) + (fConst387 * fTemp51))) + ((fConst388 * fTemp18) + ((fConst389 * fTemp53) + ((fConst390 * fTemp16) + (fTemp208 + fTemp242)))))))) - fTemp199));
			fRec174[0] = max((fRec174[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp243))))));
			fVbargraph62 = FAUSTFLOAT(fRec174[0]);
			output37[i] = FAUSTFLOAT(fTemp243);
			double fTemp244 = (fTemp195 + fTemp197);
			double fTemp245 = (fRec1[0] * (((fConst392 * ((0.023456653789763436 * fTemp66) + (0.01623922185445156 * fTemp46))) + ((fConst394 * fTemp188) + ((fConst397 * fTemp189) + ((fConst398 * ((0.022823523772822384 * fTemp48) + (0.017117642829616822 * fTemp107))) + ((fConst399 * fTemp192) + ((fConst400 * ((0.027065369757419324 * fTemp51) + (0.0090217899191397874 * fTemp69))) + (fTemp186 + ((fConst26 * fTemp187) + ((fConst29 * fTemp190) + ((fConst32 * fTemp193) + (fTemp196 + (fTemp194 + fTemp198)))))))))))) - fTemp244));
			fRec175[0] = max((fRec175[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp245))))));
			fVbargraph63 = FAUSTFLOAT(fRec175[0]);
			output38[i] = FAUSTFLOAT(fTemp245);
			double fTemp246 = (fConst401 * fTemp48);
			double fTemp247 = (fConst403 * fTemp18);
			double fTemp248 = (fTemp198 + fTemp196);
			double fTemp249 = (fRec1[0] * (((0.02017333553791887 * ((((fTemp203 + (fTemp202 + (fTemp201 + fTemp246))) + (fConst402 * fTemp66)) + fTemp247) + (fConst404 * fTemp46))) + ((fConst397 * fTemp205) + ((fConst399 * fTemp206) + ((fConst400 * ((0.009021789919139777 * fTemp51) + (0.027065369757419327 * fTemp69))) + ((fConst405 * fTemp80) + ((fConst406 * fTemp107) + ((fConst32 * fTemp207) + ((fConst407 * fTemp84) + (fTemp209 + fTemp248))))))))) - fTemp244));
			fRec176[0] = max((fRec176[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp249))))));
			fVbargraph64 = FAUSTFLOAT(fRec176[0]);
			output39[i] = FAUSTFLOAT(fTemp249);
			double fTemp250 = (fRec1[0] * (((0.02017333553791887 * (fTemp247 + ((((fTemp246 + (((fTemp214 + (fTemp213 + (fTemp212 + (fConst408 * fTemp51)))) + (fConst409 * fTemp107)) + (fConst410 * fTemp16))) + (fConst411 * fTemp66)) + (fConst412 * fTemp80)) + (fConst413 * fTemp53)))) + ((fConst414 * fTemp46) + ((fConst415 * fTemp71) + ((fConst416 * fTemp37) + ((fConst417 * fTemp69) + (fTemp211 + ((fConst418 * fTemp39) + ((fConst419 * fTemp20) + fTemp248)))))))) - fTemp244));
			fRec177[0] = max((fRec177[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp250))))));
			fVbargraph65 = FAUSTFLOAT(fRec177[0]);
			output40[i] = FAUSTFLOAT(fTemp250);
			double fTemp251 = (fRec1[0] * (((0.02017333553791887 * ((((fTemp218 + (fTemp217 + (((fTemp216 + (fConst420 * fTemp51)) + (fConst421 * fTemp107)) + (fConst422 * fTemp16)))) + (fConst423 * fTemp80)) + (fConst424 * fTemp53)) + (fConst425 * fTemp46))) + ((fConst426 * fTemp18) + ((fConst427 * fTemp71) + ((fConst428 * fTemp66) + ((fConst429 * fTemp48) + ((fConst430 * fTemp37) + ((fConst431 * fTemp69) + ((fConst432 * fTemp34) + ((fConst433 * fTemp39) + (fTemp219 + ((fConst434 * fTemp22) + fTemp248))))))))))) - fTemp244));
			fRec178[0] = max((fRec178[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp251))))));
			fVbargraph66 = FAUSTFLOAT(fRec178[0]);
			output41[i] = FAUSTFLOAT(fTemp251);
			double fTemp252 = (fRec1[0] * (((0.02017333553791887 * (((((fTemp224 + (((fConst435 * fTemp37) + (fTemp223 + ((fTemp222 + (fConst436 * fTemp69)) + (fConst437 * fTemp51)))) + (fConst438 * fTemp16))) + (fConst439 * fTemp66)) + (fConst440 * fTemp71)) + (fConst441 * fTemp53)) + (fConst442 * fTemp46))) + ((fConst394 * fTemp225) + ((fConst398 * ((0.022823523772822398 * fTemp48) + (0.017117642829616815 * fTemp107))) + (fTemp221 + ((fConst29 * fTemp226) + fTemp248))))) - fTemp244));
			fRec179[0] = max((fRec179[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp252))))));
			fVbargraph67 = FAUSTFLOAT(fRec179[0]);
			output42[i] = FAUSTFLOAT(fTemp252);
			double fTemp253 = (fRec1[0] * (((fConst392 * ((0.016239221854451587 * fTemp66) + (0.023456653789763426 * fTemp46))) + ((0.02017333553791887 * ((((fTemp231 + ((((fTemp230 + ((fTemp229 + (fConst437 * fTemp69)) + (fConst443 * fTemp51))) + (fConst438 * fTemp37)) + (fConst444 * fTemp16)) + (fConst445 * fTemp48))) + (fConst441 * fTemp71)) + (fConst446 * fTemp53)) + (fConst447 * fTemp18))) + ((fConst448 * fTemp80) + ((fConst449 * fTemp107) + ((fConst26 * fTemp228) + ((fConst450 * fTemp84) + (fTemp232 + fTemp248))))))) - fTemp244));
			fRec180[0] = max((fRec180[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp253))))));
			fVbargraph68 = FAUSTFLOAT(fRec180[0]);
			output43[i] = FAUSTFLOAT(fTemp253);
			double fTemp254 = (fRec1[0] * (((0.02017333553791887 * (((((fTemp237 + ((((fTemp236 + (fTemp235 + (fConst451 * fTemp69))) + (fConst452 * fTemp107)) + (fConst453 * fTemp37)) + (fConst454 * fTemp48))) + (fConst455 * fTemp80)) + (fConst456 * fTemp71)) + (fConst457 * fTemp18)) + (fConst458 * fTemp46))) + ((fConst459 * fTemp53) + ((fConst460 * fTemp66) + ((fConst461 * fTemp16) + ((fConst462 * fTemp51) + (fTemp234 + ((fConst463 * fTemp34) + ((fConst464 * fTemp25) + fTemp248)))))))) - fTemp244));
			fRec181[0] = max((fRec181[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp254))))));
			fVbargraph69 = FAUSTFLOAT(fRec181[0]);
			output44[i] = FAUSTFLOAT(fTemp254);
			double fTemp255 = (fRec1[0] * (((0.02017333553791887 * ((((fTemp241 + (((fTemp240 + (fTemp239 + (fConst465 * fTemp69))) + (fConst466 * fTemp107)) + (fConst467 * fTemp37))) + (fConst468 * fTemp66)) + (fConst469 * fTemp80)) + (fConst470 * fTemp71))) + ((fConst471 * fTemp46) + ((fConst472 * fTemp18) + ((fConst473 * fTemp53) + ((fConst474 * fTemp48) + ((fConst475 * fTemp16) + ((fConst476 * fTemp51) + (fTemp242 + ((fConst477 * fTemp20) + ((fConst478 * fTemp22) + ((fConst479 * fTemp25) + fTemp248))))))))))) - fTemp244));
			fRec182[0] = max((fRec182[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp255))))));
			fVbargraph70 = FAUSTFLOAT(fRec182[0]);
			output45[i] = FAUSTFLOAT(fTemp255);
			double fTemp256 = (fTemp155 + fTemp157);
			double fTemp257 = (fConst494 * fTemp48);
			double fTemp258 = (fConst495 * fTemp18);
			double fTemp259 = (fTemp154 + fTemp156);
			double fTemp260 = (fRec1[0] * ((((fConst482 * ((0.0067244451793062893 * fTemp53) + (0.0067244451793062911 * fTemp71))) + ((fConst485 * fTemp80) + ((fConst487 * fTemp66) + ((fConst488 * ((0.010086667768959435 * fTemp16) + (0.010086667768959437 * fTemp37))) + ((fConst490 * fTemp107) + ((fConst491 * ((0.02017333553791887 * fTemp51) + (0.020173335537918873 * fTemp69))) + ((fConst32 * fTemp149) + ((fConst492 * fTemp84) + ((fConst493 * fTemp34) + fTemp256))))))))) + (0.02017333553791887 * (fTemp165 + (((fTemp163 + (fTemp162 + (fTemp160 + fTemp257))) + fTemp258) + (fConst496 * fTemp46))))) - fTemp259));
			fRec183[0] = max((fRec183[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp260))))));
			fVbargraph71 = FAUSTFLOAT(fRec183[0]);
			output46[i] = FAUSTFLOAT(fTemp260);
			double fTemp261 = (fConst498 * fTemp107);
			double fTemp262 = (fConst500 * fTemp80);
			double fTemp263 = (fRec1[0] * (((0.02017333553791887 * (fTemp165 + (fTemp258 + (((fTemp257 + (((fTemp159 + (fTemp168 + (fConst497 * fTemp51))) + fTemp261) + (fConst499 * fTemp16))) + fTemp262) + (fConst501 * fTemp53))))) + ((fConst486 * ((0.020173335537918863 * fTemp66) + (0.02017333553791888 * fTemp46))) + ((fConst502 * fTemp71) + ((fConst503 * fTemp37) + ((fConst504 * fTemp69) + ((fConst26 * fTemp171) + (fTemp172 + ((fConst505 * fTemp39) + fTemp256)))))))) - fTemp259));
			fRec184[0] = max((fRec184[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp263))))));
			fVbargraph72 = FAUSTFLOAT(fRec184[0]);
			output47[i] = FAUSTFLOAT(fTemp263);
			double fTemp264 = (fRec1[0] * (((0.02017333553791887 * (fTemp165 + ((((((fTemp175 + ((fTemp174 + (fConst497 * fTemp69)) + (fConst506 * fTemp51))) + (fConst499 * fTemp37)) + (fConst507 * fTemp16)) + (fConst508 * fTemp66)) + (fConst501 * fTemp71)) + (fConst509 * fTemp53)))) + ((fConst484 * ((6.3348003989167635e-18 * fTemp18) + (0.014264702358014004 * fTemp80))) + ((fConst489 * ((1.2669600797833527e-17 * fTemp48) + (0.028529404716028009 * fTemp107))) + (((fConst29 * fTemp176) + (fTemp177 + ((fConst510 * fTemp20) + fTemp256))) + (fConst511 * fTemp46))))) - fTemp259));
			fRec185[0] = max((fRec185[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp264))))));
			fVbargraph73 = FAUSTFLOAT(fRec185[0]);
			output48[i] = FAUSTFLOAT(fTemp264);
			double fTemp265 = (fRec1[0] * (((((fConst512 * fTemp16) + ((fConst513 * fTemp51) + (fTemp179 + ((fConst514 * fTemp25) + fTemp256)))) + (fConst515 * fTemp53)) + (0.02017333553791887 * (fTemp165 + ((((fTemp262 + ((fTemp183 + (((fTemp261 + (fTemp182 + (fTemp181 + (fTemp180 + (fConst506 * fTemp69))))) + (fConst507 * fTemp37)) + (fConst516 * fTemp48))) + (fConst517 * fTemp66))) + (fConst509 * fTemp71)) + (fConst518 * fTemp18)) + (fConst519 * fTemp46))))) - fTemp259));
			fRec186[0] = max((fRec186[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp265))))));
			fVbargraph74 = FAUSTFLOAT(fRec186[0]);
			output49[i] = FAUSTFLOAT(fTemp265);
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
			fRec20[1] = fRec20[0];
			fRec21[1] = fRec21[0];
			fRec19[1] = fRec19[0];
			fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			fRec18[1] = fRec18[0];
			fRec17[1] = fRec17[0];
			fRec0[1] = fRec0[0];
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
			fRec37[1] = fRec37[0];
			fRec32[1] = fRec32[0];
			fRec31[1] = fRec31[0];
			fRec41[1] = fRec41[0];
			fRec42[1] = fRec42[0];
			fRec40[1] = fRec40[0];
			fRec43[1] = fRec43[0];
			fRec39[1] = fRec39[0];
			fRec38[1] = fRec38[0];
			fRec46[1] = fRec46[0];
			fRec47[1] = fRec47[0];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec50[1] = fRec50[0];
			fRec49[1] = fRec49[0];
			fRec48[1] = fRec48[0];
			fRec54[1] = fRec54[0];
			fRec55[1] = fRec55[0];
			fRec53[1] = fRec53[0];
			fRec56[1] = fRec56[0];
			fRec57[1] = fRec57[0];
			fRec52[1] = fRec52[0];
			fRec51[1] = fRec51[0];
			fRec24[1] = fRec24[0];
			fRec62[1] = fRec62[0];
			fRec63[1] = fRec63[0];
			fRec61[1] = fRec61[0];
			fRec64[1] = fRec64[0];
			fRec60[1] = fRec60[0];
			fRec59[1] = fRec59[0];
			fRec68[1] = fRec68[0];
			fRec69[1] = fRec69[0];
			fRec67[1] = fRec67[0];
			fRec70[1] = fRec70[0];
			fRec66[1] = fRec66[0];
			fRec65[1] = fRec65[0];
			fRec73[1] = fRec73[0];
			fRec72[1] = fRec72[0];
			fRec71[1] = fRec71[0];
			fRec58[1] = fRec58[0];
			fRec74[1] = fRec74[0];
			fRec75[1] = fRec75[0];
			fRec76[1] = fRec76[0];
			fRec81[1] = fRec81[0];
			fRec82[1] = fRec82[0];
			fRec80[1] = fRec80[0];
			fRec83[1] = fRec83[0];
			fRec84[1] = fRec84[0];
			fRec79[1] = fRec79[0];
			fRec78[1] = fRec78[0];
			fRec88[1] = fRec88[0];
			fRec89[1] = fRec89[0];
			fRec87[1] = fRec87[0];
			fRec90[1] = fRec90[0];
			fRec86[1] = fRec86[0];
			fRec85[1] = fRec85[0];
			fRec93[1] = fRec93[0];
			fRec94[1] = fRec94[0];
			fRec92[1] = fRec92[0];
			fRec91[1] = fRec91[0];
			fRec98[1] = fRec98[0];
			fRec99[1] = fRec99[0];
			fRec97[1] = fRec97[0];
			fRec100[1] = fRec100[0];
			fRec101[1] = fRec101[0];
			fRec96[1] = fRec96[0];
			fRec95[1] = fRec95[0];
			fRec77[1] = fRec77[0];
			fRec106[1] = fRec106[0];
			fRec107[1] = fRec107[0];
			fRec105[1] = fRec105[0];
			fRec108[1] = fRec108[0];
			fRec109[1] = fRec109[0];
			fRec104[1] = fRec104[0];
			fRec103[1] = fRec103[0];
			fRec112[1] = fRec112[0];
			fRec113[1] = fRec113[0];
			fRec111[1] = fRec111[0];
			fRec110[1] = fRec110[0];
			fRec117[1] = fRec117[0];
			fRec118[1] = fRec118[0];
			fRec116[1] = fRec116[0];
			fRec119[1] = fRec119[0];
			fRec120[1] = fRec120[0];
			fRec115[1] = fRec115[0];
			fRec114[1] = fRec114[0];
			fRec102[1] = fRec102[0];
			fRec121[1] = fRec121[0];
			fRec122[1] = fRec122[0];
			fRec127[1] = fRec127[0];
			fRec128[1] = fRec128[0];
			fRec126[1] = fRec126[0];
			fRec129[1] = fRec129[0];
			fRec130[1] = fRec130[0];
			fRec125[1] = fRec125[0];
			fRec124[1] = fRec124[0];
			fRec134[1] = fRec134[0];
			fRec135[1] = fRec135[0];
			fRec133[1] = fRec133[0];
			fRec136[1] = fRec136[0];
			fRec137[1] = fRec137[0];
			fRec132[1] = fRec132[0];
			fRec131[1] = fRec131[0];
			fRec140[1] = fRec140[0];
			fRec141[1] = fRec141[0];
			fRec139[1] = fRec139[0];
			fRec138[1] = fRec138[0];
			fRec123[1] = fRec123[0];
			fRec142[1] = fRec142[0];
			fRec143[1] = fRec143[0];
			fRec144[1] = fRec144[0];
			fRec145[1] = fRec145[0];
			fRec146[1] = fRec146[0];
			fRec147[1] = fRec147[0];
			fRec148[1] = fRec148[0];
			fRec153[1] = fRec153[0];
			fRec154[1] = fRec154[0];
			fRec152[1] = fRec152[0];
			fRec155[1] = fRec155[0];
			fRec151[1] = fRec151[0];
			fRec150[1] = fRec150[0];
			fRec149[1] = fRec149[0];
			fRec156[1] = fRec156[0];
			fRec157[1] = fRec157[0];
			fRec158[1] = fRec158[0];
			fRec159[1] = fRec159[0];
			fRec160[1] = fRec160[0];
			fRec161[1] = fRec161[0];
			fRec162[1] = fRec162[0];
			fRec163[1] = fRec163[0];
			fRec164[1] = fRec164[0];
			fRec165[1] = fRec165[0];
			fRec166[1] = fRec166[0];
			fRec167[1] = fRec167[0];
			fRec168[1] = fRec168[0];
			fRec169[1] = fRec169[0];
			fRec170[1] = fRec170[0];
			fRec171[1] = fRec171[0];
			fRec172[1] = fRec172[0];
			fRec173[1] = fRec173[0];
			fRec174[1] = fRec174[0];
			fRec175[1] = fRec175[0];
			fRec176[1] = fRec176[0];
			fRec177[1] = fRec177[0];
			fRec178[1] = fRec178[0];
			fRec179[1] = fRec179[0];
			fRec180[1] = fRec180[0];
			fRec181[1] = fRec181[0];
			fRec182[1] = fRec182[0];
			fRec183[1] = fRec183[0];
			fRec184[1] = fRec184[0];
			fRec185[1] = fRec185[0];
			fRec186[1] = fRec186[0];
			
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
