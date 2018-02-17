/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOADecLebedev502"
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
	double fRec0[2];
	FAUSTFLOAT fVbargraph3;
	double fConst6;
	double fConst7;
	double fConst8;
	double fConst9;
	double fConst10;
	double fRec14[2];
	double fRec13[2];
	double fRec12[2];
	FAUSTFLOAT fVbargraph4;
	double fConst11;
	double fConst12;
	double fRec17[2];
	double fRec18[2];
	double fRec16[2];
	double fRec15[2];
	FAUSTFLOAT fVbargraph5;
	double fRec11[2];
	FAUSTFLOAT fVbargraph6;
	double fRec22[2];
	double fRec21[2];
	double fRec20[2];
	FAUSTFLOAT fVbargraph7;
	double fConst13;
	double fRec19[2];
	FAUSTFLOAT fVbargraph8;
	double fConst14;
	double fRec23[2];
	FAUSTFLOAT fVbargraph9;
	double fRec24[2];
	FAUSTFLOAT fVbargraph10;
	double fRec25[2];
	FAUSTFLOAT fVbargraph11;
	double fConst15;
	double fConst16;
	double fConst17;
	double fRec29[2];
	double fRec30[2];
	double fRec28[2];
	double fRec27[2];
	FAUSTFLOAT fVbargraph12;
	double fConst18;
	double fRec26[2];
	FAUSTFLOAT fVbargraph13;
	double fConst19;
	double fRec34[2];
	double fRec35[2];
	double fRec33[2];
	double fRec32[2];
	FAUSTFLOAT fVbargraph14;
	double fRec31[2];
	FAUSTFLOAT fVbargraph15;
	double fConst20;
	double fConst21;
	double fConst22;
	double fRec36[2];
	FAUSTFLOAT fVbargraph16;
	double fRec37[2];
	FAUSTFLOAT fVbargraph17;
	double fRec41[2];
	double fRec42[2];
	double fRec40[2];
	double fRec39[2];
	FAUSTFLOAT fVbargraph18;
	double fRec38[2];
	FAUSTFLOAT fVbargraph19;
	double fConst23;
	double fConst24;
	double fConst25;
	double fRec43[2];
	FAUSTFLOAT fVbargraph20;
	double fConst26;
	double fRec44[2];
	FAUSTFLOAT fVbargraph21;
	double fConst27;
	double fConst28;
	double fRec45[2];
	FAUSTFLOAT fVbargraph22;
	double fConst29;
	double fConst30;
	double fRec46[2];
	FAUSTFLOAT fVbargraph23;
	double fRec47[2];
	FAUSTFLOAT fVbargraph24;
	double fConst31;
	double fRec48[2];
	FAUSTFLOAT fVbargraph25;
	double fRec49[2];
	FAUSTFLOAT fVbargraph26;
	double fConst32;
	double fRec50[2];
	FAUSTFLOAT fVbargraph27;
	double fConst33;
	double fConst34;
	double fConst35;
	double fConst36;
	double fConst37;
	double fRec51[2];
	FAUSTFLOAT fVbargraph28;
	double fConst38;
	double fConst39;
	double fRec52[2];
	FAUSTFLOAT fVbargraph29;
	double fConst40;
	double fConst41;
	double fConst42;
	double fRec53[2];
	FAUSTFLOAT fVbargraph30;
	double fConst43;
	double fConst44;
	double fRec54[2];
	FAUSTFLOAT fVbargraph31;
	double fConst45;
	double fConst46;
	double fRec55[2];
	FAUSTFLOAT fVbargraph32;
	double fConst47;
	double fRec56[2];
	FAUSTFLOAT fVbargraph33;
	double fConst48;
	double fConst49;
	double fRec57[2];
	FAUSTFLOAT fVbargraph34;
	double fConst50;
	double fConst51;
	double fConst52;
	double fConst53;
	double fRec58[2];
	FAUSTFLOAT fVbargraph35;
	double fConst54;
	double fConst55;
	double fConst56;
	double fConst57;
	double fRec59[2];
	FAUSTFLOAT fVbargraph36;
	double fConst58;
	double fConst59;
	double fRec60[2];
	FAUSTFLOAT fVbargraph37;
	double fConst60;
	double fConst61;
	double fConst62;
	double fRec61[2];
	FAUSTFLOAT fVbargraph38;
	double fConst63;
	double fConst64;
	double fRec62[2];
	FAUSTFLOAT fVbargraph39;
	double fConst65;
	double fConst66;
	double fConst67;
	double fConst68;
	double fRec63[2];
	FAUSTFLOAT fVbargraph40;
	double fConst69;
	double fConst70;
	double fConst71;
	double fConst72;
	double fConst73;
	double fConst74;
	double fRec64[2];
	FAUSTFLOAT fVbargraph41;
	double fConst75;
	double fConst76;
	double fConst77;
	double fConst78;
	double fConst79;
	double fConst80;
	double fRec65[2];
	FAUSTFLOAT fVbargraph42;
	double fConst81;
	double fConst82;
	double fConst83;
	double fConst84;
	double fRec66[2];
	FAUSTFLOAT fVbargraph43;
	double fConst85;
	double fConst86;
	double fConst87;
	double fConst88;
	double fRec67[2];
	FAUSTFLOAT fVbargraph44;
	double fConst89;
	double fConst90;
	double fConst91;
	double fConst92;
	double fConst93;
	double fConst94;
	double fRec68[2];
	FAUSTFLOAT fVbargraph45;
	double fConst95;
	double fConst96;
	double fConst97;
	double fConst98;
	double fConst99;
	double fConst100;
	double fRec69[2];
	FAUSTFLOAT fVbargraph46;
	double fConst101;
	double fRec70[2];
	FAUSTFLOAT fVbargraph47;
	double fRec71[2];
	FAUSTFLOAT fVbargraph48;
	double fConst102;
	double fConst103;
	double fConst104;
	double fRec72[2];
	FAUSTFLOAT fVbargraph49;
	double fConst105;
	double fConst106;
	double fConst107;
	double fRec73[2];
	FAUSTFLOAT fVbargraph50;
	double fConst108;
	double fConst109;
	double fRec74[2];
	FAUSTFLOAT fVbargraph51;
	double fConst110;
	double fRec75[2];
	FAUSTFLOAT fVbargraph52;
	double fConst111;
	double fConst112;
	double fConst113;
	double fRec76[2];
	FAUSTFLOAT fVbargraph53;
	double fConst114;
	double fConst115;
	double fConst116;
	double fRec77[2];
	FAUSTFLOAT fVbargraph54;
	double fConst117;
	double fRec78[2];
	FAUSTFLOAT fVbargraph55;
	double fConst118;
	double fConst119;
	double fConst120;
	double fRec79[2];
	FAUSTFLOAT fVbargraph56;
	double fConst121;
	double fRec80[2];
	FAUSTFLOAT fVbargraph57;
	double fConst122;
	double fConst123;
	double fRec81[2];
	FAUSTFLOAT fVbargraph58;
	
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
		m->declare("name", "HOADecLebedev502");
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
		return 9;
		
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
		fConst6 = double(tgamma(1.5));
		fConst7 = double(tgamma(1.0));
		fConst8 = sqrt((3.0 * (fConst7 / double(tgamma(3.0)))));
		fConst9 = (fConst6 * fConst8);
		fConst10 = (0.02026373487753309 * fConst9);
		fConst11 = (double(tgamma(2.5)) * sqrt((5.0 * (fConst7 / double(tgamma(5.0))))));
		fConst12 = (0.04052746975506618 * fConst11);
		fConst13 = (0.0 - (3.1915382432114616 * fConst11));
		fConst14 = (0.0 - (1.5957691216057308 * fConst9));
		fConst15 = (0.025473109945013343 * fConst8);
		fConst16 = sqrt((5.0 * (double(tgamma(2.0)) / double(tgamma(4.0)))));
		fConst17 = (0.054036626340088233 * fConst16);
		fConst18 = (0.036024417560058836 * fConst11);
		fConst19 = (0.0 - (1.595769121605731 * fConst11));
		fConst20 = (0.0 - (1.1283791670955128 * fConst9));
		fConst21 = (fConst6 * fConst16);
		fConst22 = (0.0 - (2.3936536824085959 * fConst21));
		fConst23 = (0.025473109945013343 * fConst9);
		fConst24 = (0.0 - (1.1283791670955126 * fConst9));
		fConst25 = (0.0 - (7.0866384831701748e-16 * fConst11));
		fConst26 = (0.0 - (1.128379167095513 * fConst9));
		fConst27 = (0.025473109945013332 * fConst9);
		fConst28 = (0.0 - (1.417327696634035e-15 * fConst11));
		fConst29 = (fConst16 * (0.0 - (1.6925687506432687 * fConst6)));
		fConst30 = (0.031925808815477283 * fConst29);
		fConst31 = (0.0 - (1.4142135623730951 * fConst29));
		fConst32 = (0.033660754908870889 * fConst16);
		fConst33 = (0.019434045907762623 * fConst8);
		fConst34 = (0.0 - (0.92131773192356115 * fConst9));
		fConst35 = (0.0 - (2.1276921621409737 * fConst11));
		fConst36 = (0.0 - (1.5957691216057308 * fConst21));
		fConst37 = (0.0 - (4.7244256554467816e-16 * fConst11));
		fConst38 = (0.0 - (0.92131773192356148 * fConst9));
		fConst39 = (0.0 - (1.5957691216057313 * fConst21));
		fConst40 = (0.0 - (9.4488513108935631e-16 * fConst11));
		fConst41 = (0.033660754908870882 * fConst16);
		fConst42 = (0.019434045907762616 * fConst8);
		fConst43 = (fConst16 * (0.0 - (1.5957691216057308 * fConst6)));
		fConst44 = (0.021093750000000005 * fConst43);
		fConst45 = (0.019434045907762623 * fConst9);
		fConst46 = (0.0 - fConst43);
		fConst47 = (0.0 - (1.0000000000000002 * fConst43));
		fConst48 = (0.019434045907762616 * fConst9);
		fConst49 = (0.021093749999999998 * fConst43);
		fConst50 = (0.0 - (1.2884797242127563e-16 * fConst11));
		fConst51 = (0.026338897580074728 * fConst16);
		fConst52 = (0.026338897580074724 * fConst16);
		fConst53 = (0.011706176702255417 * fConst11);
		fConst54 = (0.0097062489627452973 * fConst8);
		fConst55 = (0.0 - (0.48114249349102017 * fConst9));
		fConst56 = (0.0 - (0.58027968058390089 * fConst11));
		fConst57 = (0.0 - (1.3056292813137786 * fConst21));
		fConst58 = (0.0 - (0.48114249349102034 * fConst9));
		fConst59 = (0.0 - (1.3056292813137791 * fConst21));
		fConst60 = (0.0 - (2.5769594484255127e-16 * fConst11));
		fConst61 = (0.0097062489627452939 * fConst8);
		fConst62 = (0.026338897580074721 * fConst16);
		fConst63 = (0.0087796325266915921 * fConst16);
		fConst64 = (0.026338897580074738 * fConst16);
		fConst65 = (0.0 - (2.3211187223356089 * fConst11));
		fConst66 = (0.026338897580074742 * fConst16);
		fConst67 = (0.0087796325266915817 * fConst16);
		fConst68 = (0.035118530106766334 * fConst11);
		fConst69 = (0.029118746888235927 * fConst8);
		fConst70 = (0.026338897580074745 * fConst16);
		fConst71 = (0.0 - (0.48114249349102062 * fConst9));
		fConst72 = (0.0 - (1.7408390417517061 * fConst11));
		fConst73 = (0.0 - (0.43520976043792636 * fConst21));
		fConst74 = (0.0 - (2.3211187223356093 * fConst11));
		fConst75 = (0.0 - (1.443427480473062 * fConst9));
		fConst76 = (0.0 - (1.7408390417517088 * fConst11));
		fConst77 = (0.0 - (1.3056292813137793 * fConst21));
		fConst78 = (0.0097062489627453216 * fConst8);
		fConst79 = (0.0087796325266915939 * fConst16);
		fConst80 = (0.046824706809021746 * fConst11);
		fConst81 = (0.0 - (0.48114249349102112 * fConst9));
		fConst82 = (0.0 - (1.4434274804730622 * fConst9));
		fConst83 = (0.0 - (0.43520976043792686 * fConst21));
		fConst84 = (0.0 - (1.3056292813137795 * fConst21));
		fConst85 = (0.035118530106766348 * fConst11);
		fConst86 = (0.0 - (0.48114249349102095 * fConst9));
		fConst87 = (0.0 - (0.4352097604379267 * fConst21));
		fConst88 = (0.0 - (2.3211187223356085 * fConst11));
		fConst89 = (0.0 - (1.4434274804730625 * fConst9));
		fConst90 = (0.0 - (1.7408390417517057 * fConst11));
		fConst91 = (0.0 - (1.3056292813137798 * fConst21));
		fConst92 = (0.0 - (2.3211187223356098 * fConst11));
		fConst93 = (0.0097062489627453008 * fConst8);
		fConst94 = (0.0087796325266915748 * fConst16);
		fConst95 = (0.029118746888235913 * fConst8);
		fConst96 = (0.026338897580074731 * fConst16);
		fConst97 = (0.0 - (0.48114249349102228 * fConst9));
		fConst98 = (0.0 - (1.7408390417517114 * fConst11));
		fConst99 = (0.0 - (0.43520976043792792 * fConst21));
		fConst100 = (0.046824706809021698 * fConst11);
		fConst101 = (fConst16 * (0.0 - (0.97315860881060223 * fConst6)));
		fConst102 = (0.029118746888235927 * fConst9);
		fConst103 = (0.027065369757419331 * fConst101);
		fConst104 = (0.0 - (0.44721359549995782 * fConst101));
		fConst105 = (0.0 - (1.3416407864998736 * fConst101));
		fConst106 = (0.0090217899191397891 * fConst101);
		fConst107 = (0.0097062489627453216 * fConst9);
		fConst108 = (0.0 - (0.44721359549995832 * fConst101));
		fConst109 = (0.0 - (1.3416407864998738 * fConst101));
		fConst110 = (0.0 - (0.44721359549995815 * fConst101));
		fConst111 = (0.0097062489627453008 * fConst9);
		fConst112 = (0.00902178991913977 * fConst101);
		fConst113 = (0.0 - (1.3416407864998741 * fConst101));
		fConst114 = (0.0 - (0.44721359549995937 * fConst101));
		fConst115 = (0.027065369757419321 * fConst101);
		fConst116 = (0.029118746888235913 * fConst9);
		fConst117 = (fConst16 * (0.0 - (1.3056292813137786 * fConst6)));
		fConst118 = (0.020173335537918873 * fConst117);
		fConst119 = (0.0097062489627452973 * fConst9);
		fConst120 = (0.0 - fConst117);
		fConst121 = (0.0 - (1.0000000000000002 * fConst117));
		fConst122 = (0.0097062489627452939 * fConst9);
		fConst123 = (0.020173335537918866 * fConst117);
		
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
			fRec0[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec14[l11] = 0.0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec13[l12] = 0.0;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec12[l13] = 0.0;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec17[l14] = 0.0;
			
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec18[l15] = 0.0;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fRec16[l16] = 0.0;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec15[l17] = 0.0;
			
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec11[l18] = 0.0;
			
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec22[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec21[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec20[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec19[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec23[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec24[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec25[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec29[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec30[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec28[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec27[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec26[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec34[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec35[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec33[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec32[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec31[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec36[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec37[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec41[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec42[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec40[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec39[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec38[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec43[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec44[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec45[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec46[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec47[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec48[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec49[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec50[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec51[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec52[l52] = 0.0;
			
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
			fRec56[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec57[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec58[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec59[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec60[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec61[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fRec62[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec63[l63] = 0.0;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec64[l64] = 0.0;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec65[l65] = 0.0;
			
		}
		for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
			fRec66[l66] = 0.0;
			
		}
		for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
			fRec67[l67] = 0.0;
			
		}
		for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
			fRec68[l68] = 0.0;
			
		}
		for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
			fRec69[l69] = 0.0;
			
		}
		for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
			fRec70[l70] = 0.0;
			
		}
		for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
			fRec71[l71] = 0.0;
			
		}
		for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
			fRec72[l72] = 0.0;
			
		}
		for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
			fRec73[l73] = 0.0;
			
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
			fRec77[l77] = 0.0;
			
		}
		for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
			fRec78[l78] = 0.0;
			
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
		ui_interface->openVerticalBox("HOADecLebedev502");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("B-Format");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f3d870", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f848b0", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f48ba0", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f68930", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1fdc210", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1fbc910", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f57440", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1fa8c50", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f75c80", &fVbargraph5, -70.0, 6.0);
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
		ui_interface->declare(&fVbargraph3, "osc", "/output1 -70 6");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f5c3e0", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph6, "osc", "/output2 -70 6");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f7b3c0", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph8, "osc", "/output3 -70 6");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f8a540", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph9, "osc", "/output4 -70 6");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f90450", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph10, "osc", "/output5 -70 6");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f95d10", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph11, "osc", "/output6 -70 6");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1f9b110", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph13, "osc", "/output7 -70 6");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1faff20", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph15, "osc", "/output8 -70 6");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1fc1d90", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph16, "osc", "/output9 -70 6");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1fc94a0", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph17, "osc", "/output10 -70 6");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1fcf7a0", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph19, "osc", "/output11 -70 6");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1fe1860", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph20, "osc", "/output12 -70 6");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1fe9b80", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph21, "osc", "/output13 -70 6");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1ff1310", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph22, "osc", "/output14 -70 6");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1ff8ad0", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph23, "osc", "/output15 -70 6");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2000190", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph24, "osc", "/output16 -70 6");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20063f0", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph25, "osc", "/output17 -70 6");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x200d200", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph26, "osc", "/output18 -70 6");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2013800", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph27, "osc", "/output19 -70 6");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x201d320", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph28, "osc", "/output20 -70 6");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20276e0", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph29, "osc", "/output21 -70 6");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2030df0", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph30, "osc", "/output22 -70 6");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x203a2c0", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph31, "osc", "/output23 -70 6");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2042660", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph32, "osc", "/output24 -70 6");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x204a520", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph33, "osc", "/output25 -70 6");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2051fd0", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph34, "osc", "/output26 -70 6");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x205a1c0", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph35, "osc", "/output27 -70 6");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2064a70", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph36, "osc", "/output28 -70 6");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x206d520", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph37, "osc", "/output29 -70 6");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2075a40", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph38, "osc", "/output30 -70 6");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x207df30", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph39, "osc", "/output31 -70 6");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2087860", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph40, "osc", "/output32 -70 6");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2091110", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph41, "osc", "/output33 -70 6");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x209b730", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph42, "osc", "/output34 -70 6");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20a5a20", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph43, "osc", "/output35 -70 6");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20af840", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph44, "osc", "/output36 -70 6");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20b9190", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph45, "osc", "/output37 -70 6");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20c3b30", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph46, "osc", "/output38 -70 6");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20ce370", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph47, "osc", "/output39 -70 6");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20d5ee0", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph48, "osc", "/output40 -70 6");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20dcc60", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph49, "osc", "/output41 -70 6");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20e4ba0", &fVbargraph49, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph50, "osc", "/output42 -70 6");
		ui_interface->declare(&fVbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20ecbc0", &fVbargraph50, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph51, "osc", "/output43 -70 6");
		ui_interface->declare(&fVbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20f4660", &fVbargraph51, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph52, "osc", "/output44 -70 6");
		ui_interface->declare(&fVbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x20fbad0", &fVbargraph52, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph53, "osc", "/output45 -70 6");
		ui_interface->declare(&fVbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2103d90", &fVbargraph53, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph54, "osc", "/output46 -70 6");
		ui_interface->declare(&fVbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x210c130", &fVbargraph54, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph55, "osc", "/output47 -70 6");
		ui_interface->declare(&fVbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2113de0", &fVbargraph55, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph56, "osc", "/output48 -70 6");
		ui_interface->declare(&fVbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x211bfa0", &fVbargraph56, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph57, "osc", "/output49 -70 6");
		ui_interface->declare(&fVbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x21236a0", &fVbargraph57, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph58, "osc", "/output50 -70 6");
		ui_interface->declare(&fVbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x212b6b0", &fVbargraph58, -70.0, 6.0);
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
			double fTemp8 = (fRec1[0] * ((fTemp1 + fTemp4) + fTemp7));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fVbargraph3 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp8);
			double fTemp9 = (fRec3[0] * double(input3[i]));
			fRec14[0] = (fRec14[1] + fRec13[1]);
			fRec13[0] = (fSlow8 * ((fSlow3 * fTemp9) - (fSlow9 * fRec14[0])));
			double fTemp10 = (fRec13[0] + (fSlow4 * fTemp9));
			fRec12[0] = max((fRec12[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fVbargraph4 = FAUSTFLOAT(fRec12[0]);
			double fTemp11 = (fRec3[0] * double(input8[i]));
			fRec17[0] = (fRec17[1] + fRec16[1]);
			fRec18[0] = (fRec18[1] + fRec17[1]);
			fRec16[0] = (fSlow11 * ((fSlow3 * fTemp11) - ((fSlow13 * fRec17[0]) + (fSlow12 * fRec18[0]))));
			double fTemp12 = (fRec16[0] + (fSlow4 * fTemp11));
			fRec15[0] = max((fRec15[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph5 = FAUSTFLOAT(fRec15[0]);
			double fTemp13 = (fConst12 * fTemp12);
			double fTemp14 = (0.014197256999998666 * fTemp6);
			double fTemp15 = (fRec1[0] * (((fTemp1 + (fConst10 * fTemp10)) + fTemp13) - fTemp14));
			fRec11[0] = max((fRec11[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph6 = FAUSTFLOAT(fRec11[0]);
			output1[i] = FAUSTFLOAT(fTemp15);
			double fTemp16 = (fRec3[0] * double(input1[i]));
			fRec22[0] = (fRec22[1] + fRec21[1]);
			fRec21[0] = (fSlow8 * ((fSlow3 * fTemp16) - (fSlow9 * fRec22[0])));
			double fTemp17 = (fRec21[0] + (fSlow4 * fTemp16));
			fRec20[0] = max((fRec20[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph7 = FAUSTFLOAT(fRec20[0]);
			double fTemp18 = (fConst13 * fTemp12);
			double fTemp19 = (fRec1[0] * (((fConst10 * fTemp17) + (0.012698412698412698 * (fTemp0 + fTemp18))) - fTemp14));
			fRec19[0] = max((fRec19[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph8 = FAUSTFLOAT(fRec19[0]);
			output2[i] = FAUSTFLOAT(fTemp19);
			double fTemp20 = (fRec1[0] * ((fTemp13 + (0.012698412698412698 * (fTemp0 + (fConst14 * fTemp10)))) - fTemp14));
			fRec23[0] = max((fRec23[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph9 = FAUSTFLOAT(fRec23[0]);
			output3[i] = FAUSTFLOAT(fTemp20);
			double fTemp21 = (fRec1[0] * ((0.012698412698412698 * (fTemp18 + (fTemp0 + (fConst14 * fTemp17)))) - fTemp14));
			fRec24[0] = max((fRec24[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph10 = FAUSTFLOAT(fRec24[0]);
			output4[i] = FAUSTFLOAT(fTemp21);
			double fTemp22 = (fRec1[0] * ((fTemp1 + fTemp7) - fTemp4));
			fRec25[0] = max((fRec25[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph11 = FAUSTFLOAT(fRec25[0]);
			output5[i] = FAUSTFLOAT(fTemp22);
			double fTemp23 = (fRec3[0] * double(input7[i]));
			fRec29[0] = (fRec29[1] + fRec28[1]);
			fRec30[0] = (fRec30[1] + fRec29[1]);
			fRec28[0] = (fSlow11 * ((fSlow3 * fTemp23) - ((fSlow13 * fRec29[0]) + (fSlow12 * fRec30[0]))));
			double fTemp24 = (fRec28[0] + (fSlow4 * fTemp23));
			fRec27[0] = max((fRec27[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph12 = FAUSTFLOAT(fRec27[0]);
			double fTemp25 = (0.022574955908289243 * fTemp0);
			double fTemp26 = (0.027648561470568499 * fTemp3);
			double fTemp27 = (0.01261978399999881 * fTemp6);
			double fTemp28 = (fConst18 * fTemp12);
			double fTemp29 = (fRec1[0] * ((fConst6 * ((fConst15 * fTemp10) + (fConst17 * fTemp24))) + (((fTemp25 + fTemp26) + fTemp27) + fTemp28)));
			fRec26[0] = max((fRec26[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph13 = FAUSTFLOAT(fRec26[0]);
			output6[i] = FAUSTFLOAT(fTemp29);
			double fTemp30 = (fConst19 * fTemp12);
			double fTemp31 = (0.022574955908289243 * (fTemp0 + fTemp30));
			double fTemp32 = (fRec3[0] * double(input5[i]));
			fRec34[0] = (fRec34[1] + fRec33[1]);
			fRec35[0] = (fRec35[1] + fRec34[1]);
			fRec33[0] = (fSlow11 * ((fSlow3 * fTemp32) - ((fSlow13 * fRec34[0]) + (fSlow12 * fRec35[0]))));
			double fTemp33 = (fRec33[0] + (fSlow4 * fTemp32));
			fRec32[0] = max((fRec32[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph14 = FAUSTFLOAT(fRec32[0]);
			double fTemp34 = (fTemp26 + fTemp27);
			double fTemp35 = (fRec1[0] * (fTemp31 + ((fConst6 * ((fConst15 * fTemp17) + (fConst17 * fTemp33))) + fTemp34)));
			fRec31[0] = max((fRec31[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph15 = FAUSTFLOAT(fRec31[0]);
			output7[i] = FAUSTFLOAT(fTemp35);
			double fTemp36 = (fTemp0 + (fConst20 * fTemp10));
			double fTemp37 = (fRec1[0] * ((0.022574955908289243 * (fTemp36 + (fConst22 * fTemp24))) + (fTemp28 + fTemp34)));
			fRec36[0] = max((fRec36[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fVbargraph16 = FAUSTFLOAT(fRec36[0]);
			output8[i] = FAUSTFLOAT(fTemp37);
			double fTemp38 = (fTemp0 + (fConst20 * fTemp17));
			double fTemp39 = (fRec1[0] * (fTemp34 + (0.022574955908289243 * (fTemp30 + (fTemp38 + (fConst22 * fTemp33))))));
			fRec37[0] = max((fRec37[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph17 = FAUSTFLOAT(fRec37[0]);
			output9[i] = FAUSTFLOAT(fTemp39);
			double fTemp40 = (fRec3[0] * double(input4[i]));
			fRec41[0] = (fRec41[1] + fRec40[1]);
			fRec42[0] = (fRec42[1] + fRec41[1]);
			fRec40[0] = (fSlow11 * ((fSlow3 * fTemp40) - ((fSlow13 * fRec41[0]) + (fSlow12 * fRec42[0]))));
			double fTemp41 = (fRec40[0] + (fSlow4 * fTemp40));
			fRec39[0] = max((fRec39[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph18 = FAUSTFLOAT(fRec39[0]);
			double fTemp42 = (0.025239567999997631 * fTemp6);
			double fTemp43 = (fRec1[0] * (((fTemp25 + (fConst9 * ((0.025473109945013343 * fTemp10) + (0.025473109945013336 * fTemp17)))) + (fConst11 * ((1.5998055129555246e-17 * fTemp12) + (0.072048835120117657 * fTemp41)))) - fTemp42));
			fRec38[0] = max((fRec38[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph19 = FAUSTFLOAT(fRec38[0]);
			output10[i] = FAUSTFLOAT(fTemp43);
			double fTemp44 = (fConst23 * fTemp17);
			double fTemp45 = (fConst13 * fTemp41);
			double fTemp46 = (fRec1[0] * ((fTemp44 + (0.022574955908289243 * (((fTemp0 + (fConst24 * fTemp10)) + fTemp45) + (fConst25 * fTemp12)))) - fTemp42));
			fRec43[0] = max((fRec43[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph20 = FAUSTFLOAT(fRec43[0]);
			output11[i] = FAUSTFLOAT(fTemp46);
			double fTemp47 = (fRec1[0] * (((0.022574955908289243 * ((fTemp0 + (fConst24 * fTemp17)) + (fConst26 * fTemp10))) + (fConst11 * ((3.1996110259110492e-17 * fTemp12) + (0.072048835120117671 * fTemp41)))) - fTemp42));
			fRec44[0] = max((fRec44[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fVbargraph21 = FAUSTFLOAT(fRec44[0]);
			output12[i] = FAUSTFLOAT(fTemp47);
			double fTemp48 = (fRec1[0] * (((fConst27 * fTemp10) + (0.022574955908289243 * ((fTemp45 + (fTemp0 + (fConst26 * fTemp17))) + (fConst28 * fTemp12)))) - fTemp42));
			fRec45[0] = max((fRec45[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph22 = FAUSTFLOAT(fRec45[0]);
			output13[i] = FAUSTFLOAT(fTemp48);
			double fTemp49 = (fRec1[0] * (((fConst30 * fTemp24) + (fTemp28 + ((fConst23 * fTemp10) + (fTemp25 + fTemp27)))) - fTemp26));
			fRec46[0] = max((fRec46[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fVbargraph23 = FAUSTFLOAT(fRec46[0]);
			output14[i] = FAUSTFLOAT(fTemp49);
			double fTemp50 = (fRec1[0] * (((fConst30 * fTemp33) + (fTemp31 + (fTemp27 + fTemp44))) - fTemp26));
			fRec47[0] = max((fRec47[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph24 = FAUSTFLOAT(fRec47[0]);
			output15[i] = FAUSTFLOAT(fTemp50);
			double fTemp51 = (fRec1[0] * (((0.022574955908289243 * (fTemp36 + (fConst31 * fTemp24))) + (fTemp27 + fTemp28)) - fTemp26));
			fRec48[0] = max((fRec48[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph25 = FAUSTFLOAT(fRec48[0]);
			output16[i] = FAUSTFLOAT(fTemp51);
			double fTemp52 = (fRec1[0] * ((fTemp27 + (0.022574955908289243 * (fTemp30 + (fTemp38 + (fConst31 * fTemp33))))) - fTemp26));
			fRec49[0] = max((fRec49[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fVbargraph26 = FAUSTFLOAT(fRec49[0]);
			output17[i] = FAUSTFLOAT(fTemp52);
			double fTemp53 = (fConst11 * ((9.9655853669580557e-18 * fTemp12) + (0.044881006545161167 * fTemp41)));
			double fTemp54 = ((0.019434045907762623 * fTemp10) + (0.019434045907762619 * fTemp17));
			double fTemp55 = (fConst32 * fTemp33);
			double fTemp56 = (fTemp0 + fTemp3);
			double fTemp57 = (5.236595479505967e-18 * fTemp6);
			double fTemp58 = (fRec1[0] * (fTemp53 + ((fConst6 * (((fConst8 * fTemp54) + fTemp55) + (fConst32 * fTemp24))) + ((0.021093750000000001 * fTemp56) + fTemp57))));
			fRec50[0] = max((fRec50[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fVbargraph27 = FAUSTFLOAT(fRec50[0]);
			output18[i] = FAUSTFLOAT(fTemp58);
			double fTemp59 = (fConst34 * fTemp10);
			double fTemp60 = (fConst35 * fTemp41);
			double fTemp61 = (fConst37 * fTemp12);
			double fTemp62 = (fRec1[0] * ((fTemp57 + (fConst6 * (fTemp55 + (fConst33 * fTemp17)))) + (0.021093750000000001 * ((((fTemp56 + fTemp59) + fTemp60) + (fConst36 * fTemp24)) + fTemp61))));
			fRec51[0] = max((fRec51[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fVbargraph28 = FAUSTFLOAT(fRec51[0]);
			output19[i] = FAUSTFLOAT(fTemp62);
			double fTemp63 = (fConst11 * ((1.9931170733916111e-17 * fTemp12) + (0.044881006545161174 * fTemp41)));
			double fTemp64 = (fTemp0 + (fConst34 * fTemp17));
			double fTemp65 = (fConst38 * fTemp10);
			double fTemp66 = (fRec1[0] * (fTemp63 + (fTemp57 + (0.021093750000000001 * ((((fTemp3 + fTemp64) + fTemp65) + (fConst36 * fTemp33)) + (fConst39 * fTemp24))))));
			fRec52[0] = max((fRec52[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fVbargraph29 = FAUSTFLOAT(fRec52[0]);
			output20[i] = FAUSTFLOAT(fTemp66);
			double fTemp67 = (fTemp0 + (fConst38 * fTemp17));
			double fTemp68 = (fConst40 * fTemp12);
			double fTemp69 = (fRec1[0] * ((0.021093750000000001 * (((fTemp60 + (fTemp3 + fTemp67)) + (fConst39 * fTemp33)) + fTemp68)) + (fTemp57 + (fConst6 * ((fConst41 * fTemp24) + (fConst42 * fTemp10))))));
			fRec53[0] = max((fRec53[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp69))))));
			fVbargraph30 = FAUSTFLOAT(fRec53[0]);
			output21[i] = FAUSTFLOAT(fTemp69);
			double fTemp70 = (fTemp0 - fTemp3);
			double fTemp71 = (fRec1[0] * ((fConst44 * fTemp24) + ((0.021093750000000001 * (fTemp70 + (fConst43 * fTemp33))) + (fTemp53 + ((fConst9 * fTemp54) + fTemp57)))));
			fRec54[0] = max((fRec54[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fVbargraph31 = FAUSTFLOAT(fRec54[0]);
			output22[i] = FAUSTFLOAT(fTemp71);
			double fTemp72 = (fRec1[0] * (((fConst44 * fTemp33) + (fTemp57 + (fConst45 * fTemp17))) + (0.021093750000000001 * (fTemp61 + ((fTemp60 + (fTemp59 + fTemp70)) + (fConst46 * fTemp24))))));
			fRec55[0] = max((fRec55[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp72))))));
			fVbargraph32 = FAUSTFLOAT(fRec55[0]);
			output23[i] = FAUSTFLOAT(fTemp72);
			double fTemp73 = (fRec1[0] * ((0.021093750000000001 * (((fTemp65 - (fTemp3 - fTemp64)) + (fConst46 * fTemp33)) + (fConst47 * fTemp24))) + (fTemp57 + fTemp63)));
			fRec56[0] = max((fRec56[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp73))))));
			fVbargraph33 = FAUSTFLOAT(fRec56[0]);
			output24[i] = FAUSTFLOAT(fTemp73);
			double fTemp74 = (fRec1[0] * (((fTemp57 + (fConst48 * fTemp10)) + (fConst49 * fTemp24)) + (0.021093750000000001 * (fTemp68 + ((fTemp60 - (fTemp3 - fTemp67)) + (fConst47 * fTemp33))))));
			fRec57[0] = max((fRec57[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp74))))));
			fVbargraph34 = FAUSTFLOAT(fRec57[0]);
			output25[i] = FAUSTFLOAT(fTemp74);
			double fTemp75 = (fConst50 * fTemp12);
			double fTemp76 = (0.02017333553791887 * (fTemp0 + fTemp75));
			double fTemp77 = ((0.0097062489627452956 * fTemp10) + (0.0097062489627452973 * fTemp17));
			double fTemp78 = (fConst51 * fTemp33);
			double fTemp79 = (fConst53 * fTemp41);
			double fTemp80 = (0.031605542669238249 * fTemp3);
			double fTemp81 = (0.032806508796871948 * fTemp6);
			double fTemp82 = (fTemp80 + fTemp81);
			double fTemp83 = (fRec1[0] * (fTemp76 + ((fConst6 * (((fConst8 * fTemp77) + fTemp78) + (fConst52 * fTemp24))) + (fTemp79 + fTemp82))));
			fRec58[0] = max((fRec58[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp83))))));
			fVbargraph35 = FAUSTFLOAT(fRec58[0]);
			output26[i] = FAUSTFLOAT(fTemp83);
			double fTemp84 = (fConst56 * fTemp41);
			double fTemp85 = ((fTemp0 + (fConst55 * fTemp10)) + fTemp84);
			double fTemp86 = (fRec1[0] * ((fTemp82 + (fConst6 * (fTemp78 + (fConst54 * fTemp17)))) + (0.02017333553791887 * (fTemp75 + (fTemp85 + (fConst57 * fTemp24))))));
			fRec59[0] = max((fRec59[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp86))))));
			fVbargraph36 = FAUSTFLOAT(fRec59[0]);
			output27[i] = FAUSTFLOAT(fTemp86);
			double fTemp87 = (fConst11 * ((5.19858676206982e-18 * fTemp12) + (0.011706176702255421 * fTemp41)));
			double fTemp88 = ((fTemp0 + (fConst55 * fTemp17)) + (fConst58 * fTemp10));
			double fTemp89 = (fRec1[0] * (fTemp87 + (fTemp82 + (0.02017333553791887 * ((fTemp88 + (fConst57 * fTemp33)) + (fConst59 * fTemp24))))));
			fRec60[0] = max((fRec60[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp89))))));
			fVbargraph37 = FAUSTFLOAT(fRec60[0]);
			output28[i] = FAUSTFLOAT(fTemp89);
			double fTemp90 = (fTemp84 + (fTemp0 + (fConst58 * fTemp17)));
			double fTemp91 = (fConst60 * fTemp12);
			double fTemp92 = (fRec1[0] * ((0.02017333553791887 * ((fTemp90 + (fConst59 * fTemp33)) + fTemp91)) + (fTemp82 + (fConst6 * ((fConst61 * fTemp10) + (fConst62 * fTemp24))))));
			fRec61[0] = max((fRec61[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp92))))));
			fVbargraph38 = FAUSTFLOAT(fRec61[0]);
			output29[i] = FAUSTFLOAT(fTemp92);
			double fTemp93 = (fConst11 * ((0.046824706809021746 * fTemp12) + (0.035118530106766375 * fTemp41)));
			double fTemp94 = (0.02017333553791887 * fTemp0);
			double fTemp95 = (0.010535180889746075 * fTemp3);
			double fTemp96 = ((0.029118746888235916 * fTemp10) + (0.0097062489627453199 * fTemp17));
			double fTemp97 = (0.01640325439843596 * fTemp6);
			double fTemp98 = (fRec1[0] * ((fTemp93 + ((fTemp94 + fTemp95) + (fConst6 * (((fConst8 * fTemp96) + (fConst63 * fTemp33)) + (fConst64 * fTemp24))))) - fTemp97));
			fRec62[0] = max((fRec62[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp98))))));
			fVbargraph39 = FAUSTFLOAT(fRec62[0]);
			output30[i] = FAUSTFLOAT(fTemp98);
			double fTemp99 = (0.02017333553791887 * (fTemp0 + (fConst65 * fTemp12)));
			double fTemp100 = ((0.0097062489627453077 * fTemp10) + (0.029118746888235923 * fTemp17));
			double fTemp101 = (fConst68 * fTemp41);
			double fTemp102 = (fRec1[0] * ((fTemp99 + ((fConst6 * (((fConst8 * fTemp100) + (fConst66 * fTemp33)) + (fConst67 * fTemp24))) + (fTemp95 + fTemp101))) - fTemp97));
			fRec63[0] = max((fRec63[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp102))))));
			fVbargraph40 = FAUSTFLOAT(fRec63[0]);
			output31[i] = FAUSTFLOAT(fTemp102);
			double fTemp103 = ((fTemp0 + (fConst71 * fTemp10)) + (fConst72 * fTemp41));
			double fTemp104 = (fConst74 * fTemp12);
			double fTemp105 = (fRec1[0] * (((fTemp95 + (fConst6 * ((fConst69 * fTemp17) + (fConst70 * fTemp33)))) + (0.02017333553791887 * ((fTemp103 + (fConst73 * fTemp24)) + fTemp104))) - fTemp97));
			fRec64[0] = max((fRec64[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp105))))));
			fVbargraph41 = FAUSTFLOAT(fRec64[0]);
			output32[i] = FAUSTFLOAT(fTemp105);
			double fTemp106 = ((fTemp0 + (fConst75 * fTemp10)) + (fConst76 * fTemp41));
			double fTemp107 = (fConst80 * fTemp12);
			double fTemp108 = (fRec1[0] * (((0.02017333553791887 * (fTemp106 + (fConst77 * fTemp24))) + ((fConst6 * ((fConst78 * fTemp17) + (fConst79 * fTemp33))) + (fTemp95 + fTemp107))) - fTemp97));
			fRec65[0] = max((fRec65[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp108))))));
			fVbargraph42 = FAUSTFLOAT(fRec65[0]);
			output33[i] = FAUSTFLOAT(fTemp108);
			double fTemp109 = (fConst11 * ((0.046824706809021774 * fTemp12) + (0.035118530106766362 * fTemp41)));
			double fTemp110 = ((fTemp0 + (fConst81 * fTemp17)) + (fConst82 * fTemp10));
			double fTemp111 = (fRec1[0] * ((fTemp109 + (fTemp95 + (0.02017333553791887 * ((fTemp110 + (fConst83 * fTemp33)) + (fConst84 * fTemp24))))) - fTemp97));
			fRec66[0] = max((fRec66[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp111))))));
			fVbargraph43 = FAUSTFLOAT(fRec66[0]);
			output34[i] = FAUSTFLOAT(fTemp111);
			double fTemp112 = (fConst85 * fTemp41);
			double fTemp113 = ((fTemp0 + (fConst82 * fTemp17)) + (fConst86 * fTemp10));
			double fTemp114 = (fConst88 * fTemp12);
			double fTemp115 = (fRec1[0] * (((fTemp95 + fTemp112) + (0.02017333553791887 * (((fTemp113 + (fConst84 * fTemp33)) + (fConst87 * fTemp24)) + fTemp114))) - fTemp97));
			fRec67[0] = max((fRec67[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp115))))));
			fVbargraph44 = FAUSTFLOAT(fRec67[0]);
			output35[i] = FAUSTFLOAT(fTemp115);
			double fTemp116 = ((fTemp0 + (fConst89 * fTemp17)) + (fConst90 * fTemp41));
			double fTemp117 = (fConst92 * fTemp12);
			double fTemp118 = (fRec1[0] * (((0.02017333553791887 * ((fTemp116 + (fConst91 * fTemp33)) + fTemp117)) + (fTemp95 + (fConst6 * ((fConst93 * fTemp10) + (fConst94 * fTemp24))))) - fTemp97));
			fRec68[0] = max((fRec68[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp118))))));
			fVbargraph45 = FAUSTFLOAT(fRec68[0]);
			output36[i] = FAUSTFLOAT(fTemp118);
			double fTemp119 = ((fTemp0 + (fConst97 * fTemp17)) + (fConst98 * fTemp41));
			double fTemp120 = (fConst100 * fTemp12);
			double fTemp121 = (fRec1[0] * (((fConst6 * ((fConst95 * fTemp10) + (fConst96 * fTemp24))) + ((0.02017333553791887 * (fTemp119 + (fConst99 * fTemp33))) + (fTemp95 + fTemp120))) - fTemp97));
			fRec69[0] = max((fRec69[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp121))))));
			fVbargraph46 = FAUSTFLOAT(fRec69[0]);
			output37[i] = FAUSTFLOAT(fTemp121);
			double fTemp122 = (fTemp95 + fTemp97);
			double fTemp123 = (fRec1[0] * (((fConst101 * ((0.027065369757419324 * fTemp24) + (0.0090217899191397874 * fTemp33))) + (fTemp93 + (fTemp94 + (fConst9 * fTemp96)))) - fTemp122));
			fRec70[0] = max((fRec70[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp123))))));
			fVbargraph47 = FAUSTFLOAT(fRec70[0]);
			output38[i] = FAUSTFLOAT(fTemp123);
			double fTemp124 = (fRec1[0] * (((fConst101 * ((0.009021789919139777 * fTemp24) + (0.027065369757419327 * fTemp33))) + (fTemp99 + ((fConst9 * fTemp100) + fTemp101))) - fTemp122));
			fRec71[0] = max((fRec71[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp124))))));
			fVbargraph48 = FAUSTFLOAT(fRec71[0]);
			output39[i] = FAUSTFLOAT(fTemp124);
			double fTemp125 = (fRec1[0] * ((((fConst102 * fTemp17) + (fConst103 * fTemp33)) + (0.02017333553791887 * (fTemp104 + (fTemp103 + (fConst104 * fTemp24))))) - fTemp122));
			fRec72[0] = max((fRec72[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp125))))));
			fVbargraph49 = FAUSTFLOAT(fRec72[0]);
			output40[i] = FAUSTFLOAT(fTemp125);
			double fTemp126 = (fRec1[0] * (((0.02017333553791887 * (fTemp106 + (fConst105 * fTemp24))) + ((fConst106 * fTemp33) + (fTemp107 + (fConst107 * fTemp17)))) - fTemp122));
			fRec73[0] = max((fRec73[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp126))))));
			fVbargraph50 = FAUSTFLOAT(fRec73[0]);
			output41[i] = FAUSTFLOAT(fTemp126);
			double fTemp127 = (fRec1[0] * ((fTemp109 + (0.02017333553791887 * ((fTemp110 + (fConst108 * fTemp33)) + (fConst109 * fTemp24)))) - fTemp122));
			fRec74[0] = max((fRec74[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp127))))));
			fVbargraph51 = FAUSTFLOAT(fRec74[0]);
			output42[i] = FAUSTFLOAT(fTemp127);
			double fTemp128 = (fRec1[0] * ((fTemp112 + (0.02017333553791887 * (fTemp114 + ((fTemp113 + (fConst109 * fTemp33)) + (fConst110 * fTemp24))))) - fTemp122));
			fRec75[0] = max((fRec75[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp128))))));
			fVbargraph52 = FAUSTFLOAT(fRec75[0]);
			output43[i] = FAUSTFLOAT(fTemp128);
			double fTemp129 = (fRec1[0] * ((((fConst111 * fTemp10) + (fConst112 * fTemp24)) + (0.02017333553791887 * (fTemp117 + (fTemp116 + (fConst113 * fTemp33))))) - fTemp122));
			fRec76[0] = max((fRec76[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp129))))));
			fVbargraph53 = FAUSTFLOAT(fRec76[0]);
			output44[i] = FAUSTFLOAT(fTemp129);
			double fTemp130 = (fRec1[0] * (((0.02017333553791887 * (fTemp119 + (fConst114 * fTemp33))) + ((fConst115 * fTemp24) + ((fConst116 * fTemp10) + fTemp120))) - fTemp122));
			fRec77[0] = max((fRec77[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp130))))));
			fVbargraph54 = FAUSTFLOAT(fRec77[0]);
			output45[i] = FAUSTFLOAT(fTemp130);
			double fTemp131 = (fRec1[0] * (((fConst117 * ((0.02017333553791887 * fTemp24) + (0.020173335537918873 * fTemp33))) + (fTemp76 + ((fConst9 * fTemp77) + (fTemp79 + fTemp81)))) - fTemp80));
			fRec78[0] = max((fRec78[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp131))))));
			fVbargraph55 = FAUSTFLOAT(fRec78[0]);
			output46[i] = FAUSTFLOAT(fTemp131);
			double fTemp132 = (fRec1[0] * ((((fConst118 * fTemp33) + ((fConst119 * fTemp17) + fTemp81)) + (0.02017333553791887 * (fTemp75 + (fTemp85 + (fConst120 * fTemp24))))) - fTemp80));
			fRec79[0] = max((fRec79[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp132))))));
			fVbargraph56 = FAUSTFLOAT(fRec79[0]);
			output47[i] = FAUSTFLOAT(fTemp132);
			double fTemp133 = (fRec1[0] * (((0.02017333553791887 * ((fTemp88 + (fConst120 * fTemp33)) + (fConst121 * fTemp24))) + (fTemp81 + fTemp87)) - fTemp80));
			fRec80[0] = max((fRec80[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp133))))));
			fVbargraph57 = FAUSTFLOAT(fRec80[0]);
			output48[i] = FAUSTFLOAT(fTemp133);
			double fTemp134 = (fRec1[0] * ((((fTemp81 + (fConst122 * fTemp10)) + (fConst123 * fTemp24)) + (0.02017333553791887 * (fTemp91 + (fTemp90 + (fConst121 * fTemp33))))) - fTemp80));
			fRec81[0] = max((fRec81[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp134))))));
			fVbargraph58 = FAUSTFLOAT(fRec81[0]);
			output49[i] = FAUSTFLOAT(fTemp134);
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
			fRec0[1] = fRec0[0];
			fRec14[1] = fRec14[0];
			fRec13[1] = fRec13[0];
			fRec12[1] = fRec12[0];
			fRec17[1] = fRec17[0];
			fRec18[1] = fRec18[0];
			fRec16[1] = fRec16[0];
			fRec15[1] = fRec15[0];
			fRec11[1] = fRec11[0];
			fRec22[1] = fRec22[0];
			fRec21[1] = fRec21[0];
			fRec20[1] = fRec20[0];
			fRec19[1] = fRec19[0];
			fRec23[1] = fRec23[0];
			fRec24[1] = fRec24[0];
			fRec25[1] = fRec25[0];
			fRec29[1] = fRec29[0];
			fRec30[1] = fRec30[0];
			fRec28[1] = fRec28[0];
			fRec27[1] = fRec27[0];
			fRec26[1] = fRec26[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			fRec33[1] = fRec33[0];
			fRec32[1] = fRec32[0];
			fRec31[1] = fRec31[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec41[1] = fRec41[0];
			fRec42[1] = fRec42[0];
			fRec40[1] = fRec40[0];
			fRec39[1] = fRec39[0];
			fRec38[1] = fRec38[0];
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
			fRec77[1] = fRec77[0];
			fRec78[1] = fRec78[0];
			fRec79[1] = fRec79[0];
			fRec80[1] = fRec80[0];
			fRec81[1] = fRec81[0];
			
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
