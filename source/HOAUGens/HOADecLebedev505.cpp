/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOADecLebedev505"
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
	double fConst14;
	double fConst15;
	double fConst16;
	double fConst17;
	double fRec28[2];
	double fRec29[2];
	double fRec27[2];
	double fConst18;
	double fRec30[2];
	double fRec31[2];
	double fRec26[2];
	double fConst19;
	double fRec32[2];
	double fRec25[2];
	double fRec24[2];
	FAUSTFLOAT fVbargraph5;
	double fRec0[2];
	FAUSTFLOAT fVbargraph6;
	double fConst20;
	double fConst21;
	double fConst22;
	double fConst23;
	double fConst24;
	double fConst25;
	double fRec37[2];
	double fRec38[2];
	double fRec36[2];
	double fRec39[2];
	double fRec35[2];
	double fRec34[2];
	FAUSTFLOAT fVbargraph7;
	double fConst26;
	double fConst27;
	double fConst28;
	double fConst29;
	double fRec43[2];
	double fRec44[2];
	double fRec42[2];
	double fRec45[2];
	double fRec46[2];
	double fRec41[2];
	double fRec40[2];
	FAUSTFLOAT fVbargraph8;
	double fConst30;
	double fConst31;
	double fConst32;
	double fConst33;
	double fRec51[2];
	double fRec52[2];
	double fRec50[2];
	double fRec53[2];
	double fRec54[2];
	double fRec49[2];
	double fRec55[2];
	double fRec48[2];
	double fRec47[2];
	FAUSTFLOAT fVbargraph9;
	double fConst34;
	double fConst35;
	double fConst36;
	double fRec60[2];
	double fRec61[2];
	double fRec59[2];
	double fRec62[2];
	double fRec63[2];
	double fRec58[2];
	double fRec64[2];
	double fRec57[2];
	double fRec56[2];
	FAUSTFLOAT fVbargraph10;
	double fConst37;
	double fConst38;
	double fConst39;
	double fConst40;
	double fRec68[2];
	double fRec69[2];
	double fRec67[2];
	double fRec70[2];
	double fRec66[2];
	double fRec65[2];
	FAUSTFLOAT fVbargraph11;
	double fConst41;
	double fConst42;
	double fConst43;
	double fRec73[2];
	double fRec74[2];
	double fRec72[2];
	double fRec71[2];
	FAUSTFLOAT fVbargraph12;
	double fConst44;
	double fConst45;
	double fConst46;
	double fRec77[2];
	double fRec76[2];
	double fRec75[2];
	FAUSTFLOAT fVbargraph13;
	double fConst47;
	double fConst48;
	double fConst49;
	double fConst50;
	double fRec81[2];
	double fRec82[2];
	double fRec80[2];
	double fRec83[2];
	double fRec84[2];
	double fRec79[2];
	double fRec78[2];
	FAUSTFLOAT fVbargraph14;
	double fConst51;
	double fConst52;
	double fRec89[2];
	double fRec90[2];
	double fRec88[2];
	double fRec91[2];
	double fRec92[2];
	double fRec87[2];
	double fRec93[2];
	double fRec86[2];
	double fRec85[2];
	FAUSTFLOAT fVbargraph15;
	double fRec33[2];
	FAUSTFLOAT fVbargraph16;
	double fConst53;
	double fConst54;
	double fRec98[2];
	double fRec99[2];
	double fRec97[2];
	double fRec100[2];
	double fRec96[2];
	double fRec95[2];
	FAUSTFLOAT fVbargraph17;
	double fConst55;
	double fConst56;
	double fRec105[2];
	double fRec106[2];
	double fRec104[2];
	double fRec107[2];
	double fRec108[2];
	double fRec103[2];
	double fRec109[2];
	double fRec102[2];
	double fRec101[2];
	FAUSTFLOAT fVbargraph18;
	double fConst57;
	double fRec113[2];
	double fRec114[2];
	double fRec112[2];
	double fRec115[2];
	double fRec111[2];
	double fRec110[2];
	FAUSTFLOAT fVbargraph19;
	double fRec118[2];
	double fRec117[2];
	double fRec116[2];
	FAUSTFLOAT fVbargraph20;
	double fRec123[2];
	double fRec124[2];
	double fRec122[2];
	double fRec125[2];
	double fRec126[2];
	double fRec121[2];
	double fRec127[2];
	double fRec120[2];
	double fRec119[2];
	FAUSTFLOAT fVbargraph21;
	double fRec132[2];
	double fRec133[2];
	double fRec131[2];
	double fRec134[2];
	double fRec135[2];
	double fRec130[2];
	double fRec136[2];
	double fRec129[2];
	double fRec128[2];
	FAUSTFLOAT fVbargraph22;
	double fRec94[2];
	FAUSTFLOAT fVbargraph23;
	double fConst58;
	double fConst59;
	double fConst60;
	double fConst61;
	double fConst62;
	double fRec137[2];
	FAUSTFLOAT fVbargraph24;
	double fConst63;
	double fRec138[2];
	FAUSTFLOAT fVbargraph25;
	double fRec139[2];
	FAUSTFLOAT fVbargraph26;
	double fConst64;
	double fConst65;
	double fConst66;
	double fConst67;
	double fRec145[2];
	double fRec146[2];
	double fRec144[2];
	double fRec147[2];
	double fRec148[2];
	double fRec143[2];
	double fRec149[2];
	double fRec142[2];
	double fRec141[2];
	FAUSTFLOAT fVbargraph27;
	double fConst68;
	double fConst69;
	double fConst70;
	double fConst71;
	double fRec153[2];
	double fRec154[2];
	double fRec152[2];
	double fRec155[2];
	double fRec156[2];
	double fRec151[2];
	double fRec150[2];
	FAUSTFLOAT fVbargraph28;
	double fConst72;
	double fConst73;
	double fConst74;
	double fConst75;
	double fConst76;
	double fRec160[2];
	double fRec161[2];
	double fRec159[2];
	double fRec162[2];
	double fRec158[2];
	double fRec157[2];
	FAUSTFLOAT fVbargraph29;
	double fConst77;
	double fConst78;
	double fConst79;
	double fConst80;
	double fRec167[2];
	double fRec168[2];
	double fRec166[2];
	double fRec169[2];
	double fRec170[2];
	double fRec165[2];
	double fRec171[2];
	double fRec164[2];
	double fRec163[2];
	FAUSTFLOAT fVbargraph30;
	double fConst81;
	double fConst82;
	double fConst83;
	double fConst84;
	double fConst85;
	double fRec174[2];
	double fRec175[2];
	double fRec173[2];
	double fRec172[2];
	FAUSTFLOAT fVbargraph31;
	double fConst86;
	double fConst87;
	double fConst88;
	double fRec179[2];
	double fRec180[2];
	double fRec178[2];
	double fRec181[2];
	double fRec182[2];
	double fRec177[2];
	double fRec176[2];
	FAUSTFLOAT fVbargraph32;
	double fConst89;
	double fRec140[2];
	FAUSTFLOAT fVbargraph33;
	double fConst90;
	double fConst91;
	double fConst92;
	double fConst93;
	double fConst94;
	double fConst95;
	double fRec187[2];
	double fRec188[2];
	double fRec186[2];
	double fRec189[2];
	double fRec190[2];
	double fRec185[2];
	double fRec184[2];
	FAUSTFLOAT fVbargraph34;
	double fConst96;
	double fConst97;
	double fConst98;
	double fConst99;
	double fConst100;
	double fConst101;
	double fRec193[2];
	double fRec194[2];
	double fRec192[2];
	double fRec191[2];
	FAUSTFLOAT fVbargraph35;
	double fRec198[2];
	double fRec199[2];
	double fRec197[2];
	double fRec200[2];
	double fRec201[2];
	double fRec196[2];
	double fRec195[2];
	FAUSTFLOAT fVbargraph36;
	double fRec183[2];
	FAUSTFLOAT fVbargraph37;
	double fConst102;
	double fConst103;
	double fConst104;
	double fConst105;
	double fConst106;
	double fConst107;
	double fConst108;
	double fConst109;
	double fConst110;
	double fRec202[2];
	FAUSTFLOAT fVbargraph38;
	double fRec203[2];
	FAUSTFLOAT fVbargraph39;
	double fRec208[2];
	double fRec209[2];
	double fRec207[2];
	double fRec210[2];
	double fRec211[2];
	double fRec206[2];
	double fRec205[2];
	FAUSTFLOAT fVbargraph40;
	double fConst111;
	double fRec215[2];
	double fRec216[2];
	double fRec214[2];
	double fRec217[2];
	double fRec218[2];
	double fRec213[2];
	double fRec212[2];
	FAUSTFLOAT fVbargraph41;
	double fRec221[2];
	double fRec222[2];
	double fRec220[2];
	double fRec219[2];
	FAUSTFLOAT fVbargraph42;
	double fConst112;
	double fConst113;
	double fConst114;
	double fConst115;
	double fConst116;
	double fConst117;
	double fConst118;
	double fRec204[2];
	FAUSTFLOAT fVbargraph43;
	double fConst119;
	double fConst120;
	double fConst121;
	double fConst122;
	double fConst123;
	double fConst124;
	double fConst125;
	double fConst126;
	double fConst127;
	double fConst128;
	double fConst129;
	double fConst130;
	double fRec223[2];
	FAUSTFLOAT fVbargraph44;
	double fConst131;
	double fConst132;
	double fConst133;
	double fConst134;
	double fConst135;
	double fConst136;
	double fConst137;
	double fConst138;
	double fRec224[2];
	FAUSTFLOAT fVbargraph45;
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
	double fRec225[2];
	FAUSTFLOAT fVbargraph46;
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
	double fConst164;
	double fConst165;
	double fConst166;
	double fConst167;
	double fConst168;
	double fConst169;
	double fConst170;
	double fRec226[2];
	FAUSTFLOAT fVbargraph47;
	double fConst171;
	double fConst172;
	double fConst173;
	double fConst174;
	double fConst175;
	double fConst176;
	double fConst177;
	double fRec227[2];
	FAUSTFLOAT fVbargraph48;
	double fConst178;
	double fConst179;
	double fConst180;
	double fConst181;
	double fConst182;
	double fConst183;
	double fRec228[2];
	FAUSTFLOAT fVbargraph49;
	double fConst184;
	double fRec229[2];
	FAUSTFLOAT fVbargraph50;
	double fConst185;
	double fRec234[2];
	double fRec235[2];
	double fRec233[2];
	double fRec236[2];
	double fRec232[2];
	double fRec231[2];
	FAUSTFLOAT fVbargraph51;
	double fConst186;
	double fConst187;
	double fConst188;
	double fConst189;
	double fRec241[2];
	double fRec242[2];
	double fRec240[2];
	double fRec243[2];
	double fRec244[2];
	double fRec239[2];
	double fRec245[2];
	double fRec238[2];
	double fRec237[2];
	FAUSTFLOAT fVbargraph52;
	double fConst190;
	double fConst191;
	double fConst192;
	double fConst193;
	double fConst194;
	double fConst195;
	double fConst196;
	double fConst197;
	double fRec250[2];
	double fRec251[2];
	double fRec249[2];
	double fRec252[2];
	double fRec253[2];
	double fRec248[2];
	double fRec254[2];
	double fRec247[2];
	double fRec246[2];
	FAUSTFLOAT fVbargraph53;
	double fConst198;
	double fConst199;
	double fConst200;
	double fConst201;
	double fConst202;
	double fConst203;
	double fConst204;
	double fConst205;
	double fConst206;
	double fConst207;
	double fConst208;
	double fConst209;
	double fRec230[2];
	FAUSTFLOAT fVbargraph54;
	double fConst210;
	double fConst211;
	double fConst212;
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
	double fConst231;
	double fConst232;
	double fConst233;
	double fRec255[2];
	FAUSTFLOAT fVbargraph55;
	double fConst234;
	double fConst235;
	double fConst236;
	double fConst237;
	double fConst238;
	double fConst239;
	double fConst240;
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
	double fRec256[2];
	FAUSTFLOAT fVbargraph56;
	double fConst253;
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
	double fConst266;
	double fConst267;
	double fConst268;
	double fConst269;
	double fConst270;
	double fConst271;
	double fRec257[2];
	FAUSTFLOAT fVbargraph57;
	double fConst272;
	double fConst273;
	double fConst274;
	double fConst275;
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
	double fConst290;
	double fConst291;
	double fConst292;
	double fConst293;
	double fConst294;
	double fConst295;
	double fConst296;
	double fRec258[2];
	FAUSTFLOAT fVbargraph58;
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
	double fConst307;
	double fConst308;
	double fConst309;
	double fConst310;
	double fConst311;
	double fConst312;
	double fRec259[2];
	FAUSTFLOAT fVbargraph59;
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
	double fRec260[2];
	FAUSTFLOAT fVbargraph60;
	double fConst324;
	double fConst325;
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
	double fRec261[2];
	FAUSTFLOAT fVbargraph61;
	double fConst337;
	double fConst338;
	double fConst339;
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
	double fRec262[2];
	FAUSTFLOAT fVbargraph62;
	double fConst365;
	double fConst366;
	double fConst367;
	double fConst368;
	double fConst369;
	double fConst370;
	double fConst371;
	double fConst372;
	double fConst373;
	double fConst374;
	double fConst375;
	double fConst376;
	double fConst377;
	double fConst378;
	double fConst379;
	double fConst380;
	double fRec263[2];
	FAUSTFLOAT fVbargraph63;
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
	double fConst392;
	double fConst393;
	double fConst394;
	double fConst395;
	double fConst396;
	double fConst397;
	double fConst398;
	double fConst399;
	double fRec264[2];
	FAUSTFLOAT fVbargraph64;
	double fConst400;
	double fConst401;
	double fConst402;
	double fConst403;
	double fConst404;
	double fConst405;
	double fConst406;
	double fConst407;
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
	double fRec265[2];
	FAUSTFLOAT fVbargraph65;
	double fConst418;
	double fConst419;
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
	double fConst435;
	double fConst436;
	double fRec266[2];
	FAUSTFLOAT fVbargraph66;
	double fConst437;
	double fConst438;
	double fConst439;
	double fConst440;
	double fConst441;
	double fConst442;
	double fConst443;
	double fConst444;
	double fConst445;
	double fConst446;
	double fConst447;
	double fConst448;
	double fConst449;
	double fConst450;
	double fConst451;
	double fConst452;
	double fConst453;
	double fConst454;
	double fConst455;
	double fConst456;
	double fConst457;
	double fConst458;
	double fRec267[2];
	FAUSTFLOAT fVbargraph67;
	double fConst459;
	double fConst460;
	double fConst461;
	double fConst462;
	double fConst463;
	double fConst464;
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
	double fConst480;
	double fConst481;
	double fConst482;
	double fConst483;
	double fConst484;
	double fConst485;
	double fRec268[2];
	FAUSTFLOAT fVbargraph68;
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
	double fConst497;
	double fConst498;
	double fConst499;
	double fConst500;
	double fConst501;
	double fConst502;
	double fConst503;
	double fConst504;
	double fConst505;
	double fConst506;
	double fConst507;
	double fConst508;
	double fConst509;
	double fConst510;
	double fConst511;
	double fRec269[2];
	FAUSTFLOAT fVbargraph69;
	double fConst512;
	double fConst513;
	double fConst514;
	double fConst515;
	double fConst516;
	double fConst517;
	double fConst518;
	double fConst519;
	double fConst520;
	double fConst521;
	double fConst522;
	double fConst523;
	double fConst524;
	double fConst525;
	double fConst526;
	double fConst527;
	double fConst528;
	double fConst529;
	double fConst530;
	double fConst531;
	double fConst532;
	double fRec270[2];
	FAUSTFLOAT fVbargraph70;
	double fConst533;
	double fConst534;
	double fConst535;
	double fConst536;
	double fConst537;
	double fConst538;
	double fConst539;
	double fConst540;
	double fConst541;
	double fConst542;
	double fConst543;
	double fConst544;
	double fConst545;
	double fConst546;
	double fConst547;
	double fConst548;
	double fConst549;
	double fConst550;
	double fConst551;
	double fConst552;
	double fRec271[2];
	FAUSTFLOAT fVbargraph71;
	double fConst553;
	double fConst554;
	double fConst555;
	double fConst556;
	double fConst557;
	double fConst558;
	double fConst559;
	double fConst560;
	double fConst561;
	double fConst562;
	double fConst563;
	double fConst564;
	double fConst565;
	double fConst566;
	double fConst567;
	double fConst568;
	double fConst569;
	double fConst570;
	double fConst571;
	double fConst572;
	double fConst573;
	double fConst574;
	double fConst575;
	double fConst576;
	double fConst577;
	double fConst578;
	double fRec272[2];
	FAUSTFLOAT fVbargraph72;
	double fConst579;
	double fConst580;
	double fConst581;
	double fConst582;
	double fConst583;
	double fConst584;
	double fConst585;
	double fConst586;
	double fConst587;
	double fConst588;
	double fConst589;
	double fConst590;
	double fConst591;
	double fConst592;
	double fConst593;
	double fConst594;
	double fConst595;
	double fConst596;
	double fConst597;
	double fConst598;
	double fConst599;
	double fConst600;
	double fConst601;
	double fConst602;
	double fConst603;
	double fConst604;
	double fConst605;
	double fConst606;
	double fConst607;
	double fConst608;
	double fRec273[2];
	FAUSTFLOAT fVbargraph73;
	double fConst609;
	double fConst610;
	double fConst611;
	double fConst612;
	double fConst613;
	double fConst614;
	double fConst615;
	double fConst616;
	double fConst617;
	double fConst618;
	double fConst619;
	double fConst620;
	double fConst621;
	double fConst622;
	double fConst623;
	double fConst624;
	double fRec274[2];
	FAUSTFLOAT fVbargraph74;
	double fConst625;
	double fConst626;
	double fConst627;
	double fConst628;
	double fConst629;
	double fConst630;
	double fConst631;
	double fConst632;
	double fConst633;
	double fConst634;
	double fConst635;
	double fConst636;
	double fConst637;
	double fConst638;
	double fRec275[2];
	FAUSTFLOAT fVbargraph75;
	double fConst639;
	double fConst640;
	double fConst641;
	double fConst642;
	double fConst643;
	double fConst644;
	double fConst645;
	double fConst646;
	double fConst647;
	double fConst648;
	double fConst649;
	double fConst650;
	double fConst651;
	double fConst652;
	double fConst653;
	double fConst654;
	double fConst655;
	double fConst656;
	double fRec276[2];
	FAUSTFLOAT fVbargraph76;
	double fConst657;
	double fConst658;
	double fConst659;
	double fConst660;
	double fConst661;
	double fConst662;
	double fConst663;
	double fConst664;
	double fConst665;
	double fConst666;
	double fConst667;
	double fConst668;
	double fConst669;
	double fConst670;
	double fConst671;
	double fConst672;
	double fConst673;
	double fConst674;
	double fConst675;
	double fConst676;
	double fConst677;
	double fConst678;
	double fConst679;
	double fConst680;
	double fRec277[2];
	FAUSTFLOAT fVbargraph77;
	double fConst681;
	double fConst682;
	double fConst683;
	double fConst684;
	double fConst685;
	double fConst686;
	double fConst687;
	double fConst688;
	double fConst689;
	double fConst690;
	double fConst691;
	double fConst692;
	double fRec278[2];
	FAUSTFLOAT fVbargraph78;
	double fConst693;
	double fConst694;
	double fConst695;
	double fConst696;
	double fConst697;
	double fConst698;
	double fConst699;
	double fConst700;
	double fConst701;
	double fConst702;
	double fConst703;
	double fConst704;
	double fConst705;
	double fConst706;
	double fRec279[2];
	FAUSTFLOAT fVbargraph79;
	double fConst707;
	double fConst708;
	double fConst709;
	double fConst710;
	double fConst711;
	double fConst712;
	double fConst713;
	double fConst714;
	double fConst715;
	double fConst716;
	double fConst717;
	double fConst718;
	double fConst719;
	double fConst720;
	double fConst721;
	double fConst722;
	double fConst723;
	double fConst724;
	double fConst725;
	double fConst726;
	double fRec280[2];
	FAUSTFLOAT fVbargraph80;
	double fConst727;
	double fConst728;
	double fConst729;
	double fConst730;
	double fConst731;
	double fConst732;
	double fConst733;
	double fConst734;
	double fConst735;
	double fConst736;
	double fConst737;
	double fConst738;
	double fConst739;
	double fConst740;
	double fConst741;
	double fConst742;
	double fConst743;
	double fConst744;
	double fConst745;
	double fConst746;
	double fConst747;
	double fConst748;
	double fConst749;
	double fConst750;
	double fRec281[2];
	FAUSTFLOAT fVbargraph81;
	double fConst751;
	double fConst752;
	double fConst753;
	double fConst754;
	double fConst755;
	double fConst756;
	double fConst757;
	double fConst758;
	double fConst759;
	double fConst760;
	double fConst761;
	double fConst762;
	double fConst763;
	double fConst764;
	double fConst765;
	double fConst766;
	double fConst767;
	double fConst768;
	double fConst769;
	double fConst770;
	double fConst771;
	double fConst772;
	double fConst773;
	double fConst774;
	double fConst775;
	double fConst776;
	double fConst777;
	double fConst778;
	double fConst779;
	double fConst780;
	double fRec282[2];
	FAUSTFLOAT fVbargraph82;
	double fConst781;
	double fConst782;
	double fConst783;
	double fConst784;
	double fConst785;
	double fConst786;
	double fConst787;
	double fConst788;
	double fConst789;
	double fConst790;
	double fConst791;
	double fConst792;
	double fConst793;
	double fConst794;
	double fRec283[2];
	FAUSTFLOAT fVbargraph83;
	double fConst795;
	double fConst796;
	double fConst797;
	double fConst798;
	double fConst799;
	double fConst800;
	double fConst801;
	double fConst802;
	double fConst803;
	double fConst804;
	double fConst805;
	double fRec284[2];
	FAUSTFLOAT fVbargraph84;
	double fConst806;
	double fConst807;
	double fConst808;
	double fConst809;
	double fConst810;
	double fConst811;
	double fConst812;
	double fConst813;
	double fConst814;
	double fConst815;
	double fConst816;
	double fConst817;
	double fConst818;
	double fConst819;
	double fRec285[2];
	FAUSTFLOAT fVbargraph85;
	
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
		m->declare("name", "HOADecLebedev505");
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
		return 36;
		
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
		fConst14 = (619.94556120603033 / fConst0);
		fConst15 = (1139.6651757122002 / fConst0);
		fConst16 = (790.3892630817644 / fConst0);
		fConst17 = (1580.7785261635288 / fConst0);
		fConst18 = (2279.3303514244003 / fConst0);
		fConst19 = (1239.8911224120607 / fConst0);
		fConst20 = double(tgamma(1.5));
		fConst21 = (0.0 - (3.3851375012865379 * fConst20));
		fConst22 = double(tgamma(3.0));
		fConst23 = double(tgamma(5.0));
		fConst24 = sqrt((7.0 * (fConst22 / fConst23)));
		fConst25 = (fConst21 * fConst24);
		fConst26 = double(tgamma(2.5));
		fConst27 = double(tgamma(7.0));
		fConst28 = sqrt((9.0 * (fConst22 / fConst27)));
		fConst29 = ((0.0 - (11.283791670955125 * fConst26)) * fConst28);
		fConst30 = double(tgamma(3.5));
		fConst31 = double(tgamma(9.0));
		fConst32 = sqrt((11.0 * (fConst22 / fConst31)));
		fConst33 = ((0.0 - (31.594616678674353 * fConst30)) * fConst32);
		fConst34 = sqrt((11.0 * (fConst23 / fConst27)));
		fConst35 = ((0.0 - (2.5 * fConst21)) * fConst34);
		fConst36 = (0.0044895668646764923 * fConst35);
		fConst37 = double(tgamma(1.0));
		fConst38 = sqrt((7.0 * (fConst37 / fConst27)));
		fConst39 = (fConst30 * fConst38);
		fConst40 = (0.081054939510132359 * fConst39);
		fConst41 = sqrt((5.0 * (fConst37 / fConst23)));
		fConst42 = (fConst26 * fConst41);
		fConst43 = (0.04052746975506618 * fConst42);
		fConst44 = sqrt((3.0 * (fConst37 / fConst22)));
		fConst45 = (fConst20 * fConst44);
		fConst46 = (0.02026373487753309 * fConst45);
		fConst47 = double(tgamma(4.5));
		fConst48 = sqrt((9.0 * (fConst37 / fConst31)));
		fConst49 = (fConst47 * fConst48);
		fConst50 = (0.16210987902026472 * fConst49);
		fConst51 = (double(tgamma(5.5)) * sqrt((11.0 * (fConst37 / double(tgamma(11.0))))));
		fConst52 = (0.32421975804052944 * fConst51);
		fConst53 = (0.0 - (3.1915382432114616 * fConst42));
		fConst54 = (0.0 - (6.3830764864229232 * fConst39));
		fConst55 = (0.0 - (0.70710678118654757 * fConst29));
		fConst56 = (0.0 - (0.70710678118654757 * fConst33));
		fConst57 = (0.0089791337293529845 * fConst25);
		fConst58 = (0.0089791337293529845 * fConst29);
		fConst59 = (0.0 - (1.5957691216057308 * fConst45));
		fConst60 = (0.0 - (0.70710678118654757 * fConst25));
		fConst61 = (0.0 - (0.35355339059327379 * fConst35));
		fConst62 = (0.0 - (25.532305945691693 * fConst51));
		fConst63 = (0.0089791337293529845 * fConst33);
		fConst64 = (0.072048835120117685 * fConst48);
		fConst65 = double(tgamma(2.0));
		fConst66 = sqrt((11.0 * (fConst65 / double(tgamma(10.0)))));
		fConst67 = (0.45851597901024027 * fConst66);
		fConst68 = (0.050946219890026692 * fConst38);
		fConst69 = double(tgamma(8.0));
		fConst70 = sqrt((9.0 * (fConst65 / fConst69)));
		fConst71 = (0.25217092292041182 * fConst70);
		fConst72 = (0.62409119365282684 * fConst32);
		fConst73 = (0.036024417560058836 * fConst41);
		fConst74 = double(tgamma(6.0));
		fConst75 = sqrt((7.0 * (fConst65 / fConst74)));
		fConst76 = (0.12736554972506672 * fConst75);
		fConst77 = (0.22515260975036763 * fConst28);
		fConst78 = double(tgamma(4.0));
		fConst79 = sqrt((11.0 * (fConst78 / fConst69)));
		fConst80 = (0.22288971201886659 * fConst79);
		fConst81 = ((0.0 - (4.4881006545161242 * fConst20)) * fConst34);
		fConst82 = (0.0079814522038693207 * fConst81);
		fConst83 = (0.025473109945013343 * fConst44);
		fConst84 = sqrt((5.0 * (fConst65 / fConst78)));
		fConst85 = (0.054036626340088233 * fConst84);
		fConst86 = (0.057314497376279992 * fConst24);
		fConst87 = sqrt((9.0 * (fConst78 / fConst74)));
		fConst88 = (0.02251526097503671 * fConst87);
		fConst89 = (0.10189243978005341 * fConst51);
		fConst90 = (0.0 - (1.595769121605731 * fConst42));
		fConst91 = (0.0 - (2.256758334191026 * fConst39));
		fConst92 = (fConst26 * fConst75);
		fConst93 = (0.0 - (5.6418958354775643 * fConst92));
		fConst94 = (fConst30 * fConst70);
		fConst95 = (0.0 - (11.170383851240118 * fConst94));
		fConst96 = (fConst26 * fConst28);
		fConst97 = (0.0 - (9.9735570100358153 * fConst96));
		fConst98 = (fConst30 * fConst32);
		fConst99 = (0.0 - (27.645289593840062 * fConst98));
		fConst100 = (fConst26 * fConst79);
		fConst101 = (0.0 - (9.8733177120857309 * fConst100));
		fConst102 = (0.0 - (1.1283791670955128 * fConst45));
		fConst103 = (fConst20 * fConst84);
		fConst104 = (0.0 - (2.3936536824085959 * fConst103));
		fConst105 = (fConst20 * fConst24);
		fConst106 = (0.0 - (2.5388531259649025 * fConst105));
		fConst107 = (fConst20 * fConst87);
		fConst108 = (0.0 - (0.99735570100357918 * fConst107));
		fConst109 = (0.0 - (0.35355339059327379 * fConst81));
		fConst110 = (0.0 - (4.5135166683820529 * fConst51));
		fConst111 = (1.3574799921485692e-16 * fConst49);
		fConst112 = (0.10189243978005338 * fConst39);
		fConst113 = (0.011287477954144627 * fConst33);
		fConst114 = (0.0 - (4.5135166683820485 * fConst39));
		fConst115 = (0.0 - (12.766152972845845 * fConst49));
		fConst116 = (0.0 - (18.054066673528194 * fConst51));
		fConst117 = (0.0 - (0.49999999999999983 * fConst33));
		fConst118 = (0.0 - (18.054066673528208 * fConst51));
		fConst119 = (0.0 - (1.1283791670955126 * fConst45));
		fConst120 = (0.0 - (7.0866384831701748e-16 * fConst42));
		fConst121 = (0.0 - (0.5 * fConst25));
		fConst122 = (0.0 - (1.5700924586837752e-16 * fConst29));
		fConst123 = (0.0 - (12.766152972845846 * fConst49));
		fConst124 = (0.0 - (18.054066673528212 * fConst51));
		fConst125 = (0.0 - (0.25 * fConst35));
		fConst126 = (1.3574799921485695e-16 * fConst49);
		fConst127 = (0.0056437389770723117 * fConst35);
		fConst128 = (0.011287477954144623 * fConst25);
		fConst129 = (0.025473109945013343 * fConst45);
		fConst130 = (0.4075697591202132 * fConst51);
		fConst131 = (0.0 - (1.128379167095513 * fConst45));
		fConst132 = (0.0 - (4.5135166683820547 * fConst39));
		fConst133 = (0.0 - (0.50000000000000011 * fConst25));
		fConst134 = (0.0 - (0.50000000000000044 * fConst33));
		fConst135 = (0.0 - (0.25000000000000006 * fConst35));
		fConst136 = (2.7149599842971385e-16 * fConst49);
		fConst137 = (0.10189243978005329 * fConst39);
		fConst138 = (0.011287477954144615 * fConst33);
		fConst139 = (2.714959984297139e-16 * fConst49);
		fConst140 = (0.40756975912021387 * fConst51);
		fConst141 = (0.01128747795414462 * fConst25);
		fConst142 = (0.025473109945013332 * fConst45);
		fConst143 = (0.0056437389770723099 * fConst35);
		fConst144 = (0.0 - (1.417327696634035e-15 * fConst42));
		fConst145 = (0.0 - (4.5135166683820476 * fConst39));
		fConst146 = (0.0 - (4.5135166683820538 * fConst39));
		fConst147 = (0.0 - (3.1401849173675503e-16 * fConst29));
		fConst148 = (0.0 - (0.49999999999999967 * fConst33));
		fConst149 = (0.0 - (0.50000000000000033 * fConst33));
		fConst150 = (0.0 - (18.05406667352818 * fConst51));
		fConst151 = (0.0 - (1.6925687506432687 * fConst20));
		fConst152 = (fConst151 * fConst84);
		fConst153 = (0.0 - (3.9894228040143274 * fConst26));
		fConst154 = (fConst153 * fConst75);
		fConst155 = (0.0 - (7.898654169668589 * fConst30));
		fConst156 = (fConst155 * fConst70);
		fConst157 = ((0.0 - (14.361922094451582 * fConst47)) * fConst66);
		fConst158 = (0.0 - ((3.5355339059327373 * fConst151) + (2.3936536824085963 * fConst20)));
		fConst159 = (fConst158 * fConst24);
		fConst160 = (0.0 - ((4.9497474683058318 * fConst153) + (5.6418958354775643 * fConst26)));
		fConst161 = (fConst160 * fConst28);
		fConst162 = ((0.0 - ((6.3639610306789276 * fConst155) + (11.170383851240119 * fConst30))) * fConst32);
		fConst163 = (0.0 - ((2.4748737341529159 * fConst158) + (4.0 * fConst151)));
		fConst164 = (fConst163 * fConst87);
		fConst165 = ((0.0 - ((3.1819805153394638 * fConst160) + (6.0 * fConst153))) * fConst79);
		fConst166 = ((0.0 - ((2.1213203435596424 * fConst163) + (2.5 * fConst158))) * fConst34);
		fConst167 = (0.0079814522038693207 * fConst166);
		fConst168 = (0.072048835120117685 * fConst49);
		fConst169 = (0.050946219890026692 * fConst39);
		fConst170 = (0.036024417560058836 * fConst42);
		fConst171 = (0.0 - (1.4142135623730951 * fConst154));
		fConst172 = (0.0 - (1.4142135623730951 * fConst156));
		fConst173 = (0.0 - (0.70710678118654757 * fConst161));
		fConst174 = (0.0 - (0.70710678118654757 * fConst162));
		fConst175 = (0.0 - (0.47140452079103168 * fConst165));
		fConst176 = (0.010641936271825759 * fConst164);
		fConst177 = (0.015962904407738641 * fConst159);
		fConst178 = (0.0 - (1.4142135623730951 * fConst152));
		fConst179 = (0.0 - (0.70710678118654757 * fConst159));
		fConst180 = (0.0 - (0.47140452079103168 * fConst164));
		fConst181 = (0.0 - (0.35355339059327379 * fConst166));
		fConst182 = (0.010641936271825759 * fConst165);
		fConst183 = (0.015962904407738641 * fConst161);
		fConst184 = (0.015962904407738641 * fConst162);
		fConst185 = (0.12956030605175081 * fConst75);
		fConst186 = (2.8768166971227152e-17 * fConst75);
		fConst187 = (0.14960335515053727 * fConst28);
		fConst188 = (3.3218617889860196e-17 * fConst28);
		fConst189 = (1.0598124096115483e-16 * fConst79);
		fConst190 = (2.3532562778684169e-32 * fConst79);
		fConst191 = (0.0 - (2.6596152026762163 * fConst20));
		fConst192 = (((1.7320508075688776 * fConst191) - (4.6065886596178087 * fConst20)) * fConst34);
		fConst193 = (0.051824122420700336 * fConst38);
		fConst194 = (0.20944469721075218 * fConst70);
		fConst195 = (0.36276885694490241 * fConst32);
		fConst196 = (5.6373863931755746e-17 * fConst48);
		fConst197 = (2.9292718964632671e-16 * fConst66);
		fConst198 = (fConst191 * fConst87);
		fConst199 = (0.033660754908870889 * fConst84);
		fConst200 = (0.019434045907762629 * fConst24);
		fConst201 = (0.019434045907762633 * fConst24);
		fConst202 = (0.0 - (2.4568472851294949 * fConst39));
		fConst203 = (0.0 - (9.929230089991206 * fConst94));
		fConst204 = (0.0 - (5.6738457657092614 * fConst49));
		fConst205 = (0.0 - (6.5515927603453177 * fConst51));
		fConst206 = (0.0 - (17.197930995906468 * fConst98));
		fConst207 = (fConst47 * fConst66);
		fConst208 = (0.0 - (29.482167421553953 * fConst207));
		fConst209 = (0.0 - (6.551592760345323 * fConst51));
		fConst210 = (0.0 - (0.92131773192356115 * fConst45));
		fConst211 = (0.0 - (2.1276921621409737 * fConst42));
		fConst212 = (0.0 - (1.5957691216057308 * fConst103));
		fConst213 = (0.0 - (4.7244256554467816e-16 * fConst42));
		fConst214 = (0.0 - (6.1421182128237417 * fConst92));
		fConst215 = (0.0 - (0.92131773192356159 * fConst105));
		fConst216 = (0.0 - (1.3638242119692871e-15 * fConst92));
		fConst217 = (0.0 - (7.0923072071365816 * fConst96));
		fConst218 = (0.0 - (0.33333333333333331 * fConst198));
		fConst219 = (0.0 - (1.5748085518155944e-15 * fConst96));
		fConst220 = (0.0 - (5.6738457657092622 * fConst49));
		fConst221 = (0.0 - (6.5515927603453248 * fConst51));
		fConst222 = (0.0 - (5.0242958677880805e-15 * fConst100));
		fConst223 = (0.0 - (0.25 * fConst192));
		fConst224 = (0.0 - (1.1156177909894717e-30 * fConst100));
		fConst225 = (0.0 - (29.482167421553957 * fConst207));
		fConst226 = (0.20944469721075201 * fConst70);
		fConst227 = (0.36276885694490207 * fConst32);
		fConst228 = (5.6373863931755758e-17 * fConst48);
		fConst229 = (2.9292718964632676e-16 * fConst66);
		fConst230 = (0.019434045907762623 * fConst44);
		fConst231 = (0.0052734375000000012 * fConst192);
		fConst232 = (0.007031250000000001 * fConst198);
		fConst233 = (0.13819765978853404 * fConst51);
		fConst234 = (0.0 - (0.92131773192356148 * fConst45));
		fConst235 = (0.0 - (1.5957691216057313 * fConst103));
		fConst236 = (0.0 - (2.456847285129498 * fConst39));
		fConst237 = (0.0 - (0.92131773192356192 * fConst105));
		fConst238 = (0.0 - (9.9292300899912185 * fConst94));
		fConst239 = (0.0 - (0.33333333333333343 * fConst198));
		fConst240 = (0.0 - (17.197930995906489 * fConst98));
		fConst241 = (0.0 - (0.25000000000000006 * fConst192));
		fConst242 = (0.051824122420700267 * fConst38);
		fConst243 = (0.20944469721075193 * fConst70);
		fConst244 = (0.36276885694490202 * fConst32);
		fConst245 = (0.12956030605175084 * fConst75);
		fConst246 = (5.7536333942454304e-17 * fConst75);
		fConst247 = (0.14960335515053733 * fConst28);
		fConst248 = (6.6437235779720392e-17 * fConst28);
		fConst249 = (1.0598124096115484e-16 * fConst79);
		fConst250 = (4.7065125557368339e-32 * fConst79);
		fConst251 = (1.1274772786351149e-16 * fConst48);
		fConst252 = (5.8585437929265342e-16 * fConst66);
		fConst253 = (1.1274772786351152e-16 * fConst48);
		fConst254 = (5.8585437929265352e-16 * fConst66);
		fConst255 = (0.019434045907762616 * fConst44);
		fConst256 = (0.033660754908870882 * fConst84);
		fConst257 = (0.019434045907762626 * fConst24);
		fConst258 = (0.0070312499999999993 * fConst198);
		fConst259 = (0.13819765978853429 * fConst51);
		fConst260 = (0.0052734374999999995 * fConst192);
		fConst261 = (0.0 - (9.4488513108935631e-16 * fConst42));
		fConst262 = (0.0 - (2.456847285129494 * fConst39));
		fConst263 = (0.0 - (2.7276484239385742e-15 * fConst92));
		fConst264 = (0.0 - (2.4568472851294976 * fConst39));
		fConst265 = (0.0 - (9.9292300899912025 * fConst94));
		fConst266 = (0.0 - (3.1496171036311889e-15 * fConst96));
		fConst267 = (0.0 - (9.9292300899912167 * fConst94));
		fConst268 = (0.0 - (17.197930995906464 * fConst98));
		fConst269 = (0.0 - (2.2312355819789434e-30 * fConst100));
		fConst270 = (0.0 - (17.197930995906486 * fConst98));
		fConst271 = (0.0 - (6.5515927603453132 * fConst51));
		fConst272 = (0.0 - (4.3431334391370653 * fConst26));
		fConst273 = (0.0 - ((4.0414518843273806 * fConst272) + (7.5225277806367483 * fConst26)));
		fConst274 = ((0.0 - ((2.5980762113533165 * fConst273) + (6.0 * fConst272))) * fConst79);
		fConst275 = (0.0 - (1.5957691216057308 * fConst20));
		fConst276 = (0.0 - ((2.8867513459481291 * fConst275) + (2.7639531957706835 * fConst20)));
		fConst277 = (0.0 - ((2.0207259421636903 * fConst276) + (4.0 * fConst275)));
		fConst278 = ((0.0 - ((1.7320508075688776 * fConst277) + (2.5 * fConst276))) * fConst34);
		fConst279 = (0.0 - (9.9292300899912096 * fConst30));
		fConst280 = ((0.0 - ((5.1961524227066329 * fConst279) + (17.197930995906471 * fConst30))) * fConst32);
		fConst281 = (0.010546875000000006 * fConst280);
		fConst282 = ((0.0 - (20.847040507857912 * fConst47)) * fConst66);
		fConst283 = (1.405126015541214e-17 * fConst282);
		fConst284 = (fConst273 * fConst28);
		fConst285 = (fConst277 * fConst87);
		fConst286 = (fConst279 * fConst70);
		fConst287 = (0.021093750000000012 * fConst286);
		fConst288 = (fConst272 * fConst75);
		fConst289 = (fConst276 * fConst24);
		fConst290 = (fConst275 * fConst84);
		fConst291 = (0.021093750000000005 * fConst290);
		fConst292 = (5.6373863931755746e-17 * fConst49);
		fConst293 = (0.051824122420700336 * fConst39);
		fConst294 = (0.0 - (0.99999999999999967 * fConst286));
		fConst295 = (0.0 - (0.49999999999999983 * fConst280));
		fConst296 = (0.0 - (1.4142135623730949 * fConst282));
		fConst297 = (0.0 - fConst290);
		fConst298 = (0.0 - (1.4142135623730951 * fConst288));
		fConst299 = (0.0 - (0.5 * fConst289));
		fConst300 = (0.0 - (3.1401849173675503e-16 * fConst288));
		fConst301 = (0.0 - (0.70710678118654757 * fConst284));
		fConst302 = (0.0 - (0.33333333333333331 * fConst285));
		fConst303 = (0.0 - (1.5700924586837752e-16 * fConst284));
		fConst304 = (0.0 - (0.47140452079103168 * fConst274));
		fConst305 = (0.0 - (0.25 * fConst278));
		fConst306 = (0.0 - (1.0467283057891834e-16 * fConst274));
		fConst307 = (0.0 - (1.4142135623730951 * fConst282));
		fConst308 = (0.0052734375000000012 * fConst278);
		fConst309 = (0.007031250000000001 * fConst285);
		fConst310 = (0.010546875000000002 * fConst289);
		fConst311 = (5.6373863931755758e-17 * fConst49);
		fConst312 = (0.019434045907762623 * fConst45);
		fConst313 = (0.0 - (1.0000000000000002 * fConst290));
		fConst314 = (0.0 - (0.50000000000000011 * fConst289));
		fConst315 = (0.0 - (1.0000000000000009 * fConst286));
		fConst316 = (0.0 - (0.33333333333333343 * fConst285));
		fConst317 = (0.0 - (0.50000000000000044 * fConst280));
		fConst318 = (0.0 - (0.25000000000000006 * fConst278));
		fConst319 = (0.010546874999999994 * fConst280);
		fConst320 = (0.021093749999999988 * fConst286);
		fConst321 = (2.810252031082428e-17 * fConst282);
		fConst322 = (1.1274772786351149e-16 * fConst49);
		fConst323 = (0.051824122420700267 * fConst39);
		fConst324 = (0.0070312499999999993 * fConst285);
		fConst325 = (0.010546874999999999 * fConst289);
		fConst326 = (0.021093749999999998 * fConst290);
		fConst327 = (1.1274772786351152e-16 * fConst49);
		fConst328 = (0.019434045907762616 * fConst45);
		fConst329 = (0.0052734374999999995 * fConst278);
		fConst330 = (0.0 - (6.2803698347351007e-16 * fConst288));
		fConst331 = (0.0 - (0.99999999999999933 * fConst286));
		fConst332 = (0.0 - (3.1401849173675503e-16 * fConst284));
		fConst333 = (0.0 - (1.0000000000000007 * fConst286));
		fConst334 = (0.0 - (0.49999999999999967 * fConst280));
		fConst335 = (0.0 - (2.0934566115783667e-16 * fConst274));
		fConst336 = (0.0 - (0.50000000000000033 * fConst280));
		fConst337 = (0.026338897580074728 * fConst84);
		fConst338 = (0.026338897580074724 * fConst84);
		fConst339 = (0.045001699736364593 * fConst24);
		fConst340 = (0.045001699736364587 * fConst24);
		fConst341 = (0.059861130863806261 * fConst87);
		fConst342 = (0.059861130863806254 * fConst87);
		fConst343 = (0.065577343198713223 * fConst34);
		fConst344 = (0.065577343198713209 * fConst34);
		fConst345 = (0.052943176160428845 * fConst75);
		fConst346 = (0.011706176702255417 * fConst41);
		fConst347 = (0.13834572466301867 * fConst28);
		fConst348 = (0.26952889681672887 * fConst79);
		fConst349 = (0.0070590901547238332 * fConst38);
		fConst350 = (0.044696311044975172 * fConst70);
		fConst351 = (0.15722518980975816 * fConst32);
		fConst352 = (0.0 - (1.2884797242127563e-16 * fConst42));
		fConst353 = (0.0 - (5.8273688116286149e-16 * fConst92));
		fConst354 = (0.0 - (0.34992181344801404 * fConst39));
		fConst355 = (0.0 - (1.9878387281720961e-16 * fConst49));
		fConst356 = (0.0 - (1.522748764978713e-15 * fConst96));
		fConst357 = (0.0 - (2.2156133258658022 * fConst94));
		fConst358 = (0.0 - (0.42202158587919975 * fConst49));
		fConst359 = (0.0 - (0.25448859159855519 * fConst51));
		fConst360 = (0.0 - (1.6182610048842852e-15 * fConst207));
		fConst361 = (0.0 - (2.9666604859200243e-15 * fConst100));
		fConst362 = (0.0 - (7.7937131177057708 * fConst98));
		fConst363 = (0.0 - (3.4355959865804975 * fConst207));
		fConst364 = (0.0 - (0.25448859159855491 * fConst51));
		fConst365 = (0.0 - (0.48114249349102017 * fConst45));
		fConst366 = (0.0 - (0.58027968058390089 * fConst42));
		fConst367 = (0.0 - (1.3056292813137786 * fConst103));
		fConst368 = (0.0 - (2.624413600860108 * fConst92));
		fConst369 = (0.0 - (2.230751560731095 * fConst105));
		fConst370 = (0.0 - (6.8578507705370146 * fConst96));
		fConst371 = (0.0 - (2.9673392757131363 * fConst107));
		fConst372 = (0.0 - (13.360651058924196 * fConst100));
		fConst373 = (fConst20 * fConst34);
		fConst374 = (0.0 - (3.2506941192471897 * fConst373));
		fConst375 = (0.044696311044975207 * fConst70);
		fConst376 = (0.1572251898097583 * fConst32);
		fConst377 = (0.0097062489627452973 * fConst44);
		fConst378 = (4.0101337658685596e-18 * fConst48);
		fConst379 = (3.2645722239460454e-17 * fConst66);
		fConst380 = (0.0051338837488900495 * fConst51);
		fConst381 = (0.0 - (0.48114249349102034 * fConst45));
		fConst382 = (0.0 - (1.3056292813137791 * fConst103));
		fConst383 = (0.0 - (0.34992181344801421 * fConst39));
		fConst384 = (0.0 - (2.2307515607310959 * fConst105));
		fConst385 = (0.0 - (2.2156133258658035 * fConst94));
		fConst386 = (0.0 - (2.9673392757131376 * fConst107));
		fConst387 = (0.0 - (7.7937131177057752 * fConst98));
		fConst388 = (0.0 - (3.2506941192471905 * fConst373));
		fConst389 = (0.0070590901547238306 * fConst38);
		fConst390 = (0.044696311044975165 * fConst70);
		fConst391 = (0.15722518980975814 * fConst32);
		fConst392 = (0.052943176160428852 * fConst75);
		fConst393 = (2.3511493268037518e-17 * fConst75);
		fConst394 = (0.1383457246630187 * fConst28);
		fConst395 = (6.1437843551734282e-17 * fConst28);
		fConst396 = (0.26952889681672892 * fConst79);
		fConst397 = (1.1969487481910018e-16 * fConst79);
		fConst398 = (8.0202675317371177e-18 * fConst48);
		fConst399 = (6.5291444478920883e-17 * fConst66);
		fConst400 = (0.0097062489627452939 * fConst44);
		fConst401 = (0.026338897580074721 * fConst84);
		fConst402 = (0.045001699736364573 * fConst24);
		fConst403 = (0.05986113086380624 * fConst87);
		fConst404 = (0.0051338837488900581 * fConst51);
		fConst405 = (8.0202675317371192e-18 * fConst48);
		fConst406 = (6.5291444478920908e-17 * fConst66);
		fConst407 = (0.0 - (2.5769594484255127e-16 * fConst42));
		fConst408 = (0.0 - (0.34992181344801365 * fConst39));
		fConst409 = (0.0 - (1.165473762325723e-15 * fConst92));
		fConst410 = (0.0 - (0.34992181344801415 * fConst39));
		fConst411 = (0.0 - (2.2156133258658 * fConst94));
		fConst412 = (0.0 - (3.0454975299574259e-15 * fConst96));
		fConst413 = (0.0 - (2.2156133258658031 * fConst94));
		fConst414 = (0.0 - (7.7937131177057628 * fConst98));
		fConst415 = (0.0 - (5.9333209718400486e-15 * fConst100));
		fConst416 = (0.0 - (7.7937131177057735 * fConst98));
		fConst417 = (0.0 - (0.25448859159855475 * fConst51));
		fConst418 = (0.0 - (0.76346577479571942 * fConst51));
		fConst419 = (0.55445944488012866 * fConst66);
		fConst420 = (0.1617173380900363 * fConst66);
		fConst421 = ((0.0 - (4.979220665234859 * fConst30)) * fConst32);
		fConst422 = (0.0 - (3.7301790647785582 * fConst26));
		fConst423 = (((1.3568010505999357 * fConst422) - (18.557406538063905 * fConst26)) * fConst79);
		fConst424 = (0.0 - (1.7605101637794649 * fConst20));
		fConst425 = ((1.0552897060221724 * fConst424) - (3.8926344352424089 * fConst20));
		fConst426 = (((0.90453403373329044 * fConst425) - (2.5 * fConst424)) * fConst34);
		fConst427 = (0.19368401452822653 * fConst70);
		fConst428 = (0.13408893313492565 * fConst70);
		fConst429 = (fConst422 * fConst28);
		fConst430 = (fConst425 * fConst87);
		fConst431 = (0.052943176160428991 * fConst75);
		fConst432 = (0.070590901547238516 * fConst75);
		fConst433 = (fConst424 * fConst24);
		fConst434 = (0.0087796325266915921 * fConst84);
		fConst435 = (0.026338897580074738 * fConst84);
		fConst436 = (0.40557681616231633 * fConst51);
		fConst437 = (0.059595081393300449 * fConst48);
		fConst438 = (0.16171733809003749 * fConst66);
		fConst439 = (0.0 - (2.3211187223356089 * fConst42));
		fConst440 = (0.0 - (3.1492963210321352 * fConst39));
		fConst441 = (0.0 - (3.4992181344801496 * fConst92));
		fConst442 = (0.0 - (4.548983574824196 * fConst39));
		fConst443 = (0.0 - (10.128518061100836 * fConst49));
		fConst444 = (0.0 - (6.6468399775974225 * fConst94));
		fConst445 = (0.0 - (0.56568542494923812 * fConst429));
		fConst446 = (0.0 - (9.600991078751834 * fConst94));
		fConst447 = (0.0 - (0.76346577479567002 * fConst51));
		fConst448 = (0.0 - (27.484767892644079 * fConst207));
		fConst449 = (0.0 - (0.40249223594996214 * fConst421));
		fConst450 = (0.0 - (0.37712361663282534 * fConst423));
		fConst451 = (0.0 - (0.58137767414994534 * fConst421));
		fConst452 = (0.052943176160428929 * fConst75);
		fConst453 = (0.035118530106766334 * fConst41);
		fConst454 = (0.026338897580074742 * fConst84);
		fConst455 = (0.0087796325266915817 * fConst84);
		fConst456 = (0.0057058809432056004 * fConst423);
		fConst457 = (0.0085588214148084006 * fConst429);
		fConst458 = (0.40557681616231639 * fConst51);
		fConst459 = (0.0 - (0.48114249349102062 * fConst45));
		fConst460 = (0.0 - (1.7408390417517061 * fConst42));
		fConst461 = (0.0 - (0.43520976043792636 * fConst103));
		fConst462 = (0.0 - (2.3211187223356093 * fConst42));
		fConst463 = (0.0 - (3.1492963210321379 * fConst39));
		fConst464 = (0.0 - (2.6244136008601116 * fConst92));
		fConst465 = (0.0 - (0.22360679774997891 * fConst433));
		fConst466 = (0.0 - (3.4992181344801501 * fConst92));
		fConst467 = (0.0 - (6.6468399775974278 * fConst94));
		fConst468 = (0.0 - (0.42426406871192845 * fConst429));
		fConst469 = (0.0 - (0.14907119849998593 * fConst430));
		fConst470 = (0.0 - (0.76346577479565958 * fConst51));
		fConst471 = (0.0 - (0.40249223594996242 * fConst421));
		fConst472 = (0.0 - (0.28284271247461895 * fConst423));
		fConst473 = (0.0 - (0.11180339887498945 * fConst426));
		fConst474 = (0.0 - (0.3771236166328254 * fConst423));
		fConst475 = (0.0 - (20.104598736285961 * fConst51));
		fConst476 = (0.09176817201141016 * fConst38);
		fConst477 = (0.19368401452822634 * fConst70);
		fConst478 = (0.029118746888235927 * fConst44);
		fConst479 = (0.026338897580074745 * fConst84);
		fConst480 = (0.0067663424393548327 * fConst426);
		fConst481 = (0.009021789919139777 * fConst430);
		fConst482 = (0.013532684878709665 * fConst433);
		fConst483 = (0.011728326894881708 * fConst421);
		fConst484 = (0.55445944488012844 * fConst66);
		fConst485 = (0.16171733809003774 * fConst66);
		fConst486 = (0.059595081393300019 * fConst48);
		fConst487 = (0.0 - (1.443427480473062 * fConst45));
		fConst488 = (0.0 - (1.7408390417517088 * fConst42));
		fConst489 = (0.0 - (1.3056292813137793 * fConst103));
		fConst490 = (0.0 - (2.6244136008601155 * fConst92));
		fConst491 = (0.0 - (0.67082039324993681 * fConst433));
		fConst492 = (0.0 - (3.1492963210321285 * fConst39));
		fConst493 = (0.0 - (10.128518061100845 * fConst49));
		fConst494 = (0.0 - (0.42426406871192907 * fConst429));
		fConst495 = (0.0 - (0.44721359549995787 * fConst430));
		fConst496 = (0.0 - (6.6468399775974083 * fConst94));
		fConst497 = (0.0 - (27.484767892644101 * fConst207));
		fConst498 = (0.0 - (0.28284271247461934 * fConst423));
		fConst499 = (0.0 - (0.3354101966249684 * fConst426));
		fConst500 = (0.0 - (0.40249223594996125 * fConst421));
		fConst501 = (0.091768172011410271 * fConst38);
		fConst502 = (0.19368401452822659 * fConst70);
		fConst503 = (0.046824706809021746 * fConst41);
		fConst504 = (0.0097062489627453216 * fConst44);
		fConst505 = (0.0087796325266915939 * fConst84);
		fConst506 = (0.007607841257607462 * fConst423);
		fConst507 = (0.0022554474797849473 * fConst426);
		fConst508 = (0.01172832689488172 * fConst421);
		fConst509 = (0.011411761886411192 * fConst429);
		fConst510 = (0.0030072633063799293 * fConst430);
		fConst511 = (0.0045108949595698946 * fConst433);
		fConst512 = (0.16171733809003719 * fConst66);
		fConst513 = (0.0 - (0.48114249349102112 * fConst45));
		fConst514 = (0.0 - (1.4434274804730622 * fConst45));
		fConst515 = (0.0 - (0.43520976043792686 * fConst103));
		fConst516 = (0.0 - (1.3056292813137795 * fConst103));
		fConst517 = (0.0 - (4.5489835748241987 * fConst39));
		fConst518 = (0.0 - (0.22360679774997916 * fConst433));
		fConst519 = (0.0 - (0.67082039324993692 * fConst433));
		fConst520 = (0.0 - (3.1492963210321339 * fConst39));
		fConst521 = (0.0 - (9.6009910787518393 * fConst94));
		fConst522 = (0.0 - (0.1490711984999861 * fConst430));
		fConst523 = (0.0 - (0.44721359549995793 * fConst430));
		fConst524 = (0.0 - (6.6468399775974198 * fConst94));
		fConst525 = (0.0 - (20.104598736285965 * fConst51));
		fConst526 = (0.0 - (0.58137767414994568 * fConst421));
		fConst527 = (0.0 - (0.11180339887498958 * fConst426));
		fConst528 = (0.0 - (0.33541019662496846 * fConst426));
		fConst529 = (0.0 - (0.40249223594996197 * fConst421));
		fConst530 = (0.052943176160428963 * fConst75);
		fConst531 = (0.070590901547238558 * fConst75);
		fConst532 = (0.01540165124667044 * fConst51);
		fConst533 = (0.0 - (0.48114249349102095 * fConst45));
		fConst534 = (0.0 - (0.4352097604379267 * fConst103));
		fConst535 = (0.0 - (2.3211187223356085 * fConst42));
		fConst536 = (0.0 - (0.22360679774997907 * fConst433));
		fConst537 = (0.0 - (3.4992181344801487 * fConst92));
		fConst538 = (0.0 - (10.12851806110084 * fConst49));
		fConst539 = (0.0 - (0.14907119849998604 * fConst430));
		fConst540 = (0.0 - (0.5656854249492379 * fConst429));
		fConst541 = (0.0 - (27.484767892644086 * fConst207));
		fConst542 = (0.0 - (0.11180339887498954 * fConst426));
		fConst543 = (0.0 - (0.37712361663282523 * fConst423));
		fConst544 = (0.059595081393300338 * fConst48);
		fConst545 = (0.16171733809003716 * fConst66);
		fConst546 = (0.052943176160428942 * fConst75);
		fConst547 = (0.035118530106766348 * fConst41);
		fConst548 = (0.19368401452822642 * fConst70);
		fConst549 = (0.13408893313492587 * fConst70);
		fConst550 = (0.005705880943205603 * fConst423);
		fConst551 = (0.0085588214148084041 * fConst429);
		fConst552 = (0.015401651246670513 * fConst51);
		fConst553 = (0.55445944488012833 * fConst66);
		fConst554 = (0.16171733809003799 * fConst66);
		fConst555 = (0.0 - (1.4434274804730625 * fConst45));
		fConst556 = (0.0 - (1.7408390417517057 * fConst42));
		fConst557 = (0.0 - (1.3056292813137798 * fConst103));
		fConst558 = (0.0 - (2.3211187223356098 * fConst42));
		fConst559 = (0.0 - (2.6244136008601107 * fConst92));
		fConst560 = (0.0 - (0.67082039324993703 * fConst433));
		fConst561 = (0.0 - (3.4992181344801505 * fConst92));
		fConst562 = (0.0 - (4.5489835748241934 * fConst39));
		fConst563 = (0.0 - (0.42426406871192829 * fConst429));
		fConst564 = (0.0 - (0.44721359549995798 * fConst430));
		fConst565 = (0.0 - (0.56568542494923824 * fConst429));
		fConst566 = (0.0 - (9.6009910787518287 * fConst94));
		fConst567 = (0.0 - (0.28284271247461884 * fConst423));
		fConst568 = (0.0 - (0.33541019662496852 * fConst426));
		fConst569 = (0.0 - (0.37712361663282545 * fConst423));
		fConst570 = (0.0 - (0.58137767414994512 * fConst421));
		fConst571 = (0.063531811392514792 * fConst38);
		fConst572 = (0.13408893313492612 * fConst70);
		fConst573 = (0.0097062489627453008 * fConst44);
		fConst574 = (0.0087796325266915748 * fConst84);
		fConst575 = (0.0022554474797849425 * fConst426);
		fConst576 = (0.0081196109272258076 * fConst421);
		fConst577 = (0.0030072633063799232 * fConst430);
		fConst578 = (0.004510894959569885 * fConst433);
		fConst579 = (0.0 - (0.48114249349102228 * fConst45));
		fConst580 = (0.0 - (1.7408390417517114 * fConst42));
		fConst581 = (0.0 - (0.43520976043792792 * fConst103));
		fConst582 = (0.0 - (4.5489835748242058 * fConst39));
		fConst583 = (0.0 - (2.6244136008601191 * fConst92));
		fConst584 = (0.0 - (0.22360679774997969 * fConst433));
		fConst585 = (0.0 - (10.12851806110085 * fConst49));
		fConst586 = (0.0 - (9.6009910787518553 * fConst94));
		fConst587 = (0.0 - (0.42426406871192973 * fConst429));
		fConst588 = (0.0 - (0.14907119849998646 * fConst430));
		fConst589 = (0.0 - (20.104598736285954 * fConst51));
		fConst590 = (0.0 - (27.484767892644118 * fConst207));
		fConst591 = (0.0 - (0.58137767414994668 * fConst421));
		fConst592 = (0.0 - (0.28284271247461978 * fConst423));
		fConst593 = (0.0 - (0.11180339887498984 * fConst426));
		fConst594 = (0.0 - (0.76346577479577982 * fConst51));
		fConst595 = (0.059595081393299554 * fConst48);
		fConst596 = (0.16171733809003505 * fConst66);
		fConst597 = (0.063531811392514445 * fConst38);
		fConst598 = (0.13408893313492534 * fConst70);
		fConst599 = (0.046824706809021698 * fConst41);
		fConst600 = (0.07059090154723846 * fConst75);
		fConst601 = (0.029118746888235913 * fConst44);
		fConst602 = (0.026338897580074731 * fConst84);
		fConst603 = (0.0081196109272257608 * fConst421);
		fConst604 = (0.0076078412576074551 * fConst423);
		fConst605 = (0.0067663424393548301 * fConst426);
		fConst606 = (0.011411761886411183 * fConst429);
		fConst607 = (0.0090217899191397718 * fConst430);
		fConst608 = (0.01353268487870966 * fConst433);
		fConst609 = (0.0 - (8.2571033474838984 * fConst30));
		fConst610 = (fConst32 * (0.0 - ((2.7136021011998714 * fConst609) + (27.385713658791659 * fConst30))));
		fConst611 = (0.0 - (3.0929010896773175 * fConst26));
		fConst612 = (0.0 - ((2.1105794120443448 * fConst611) + (10.257992428141025 * fConst26)));
		fConst613 = (fConst79 * (0.0 - ((1.3568010505999357 * fConst612) + (6.0 * fConst611))));
		fConst614 = (0.0 - (0.97315860881060223 * fConst20));
		fConst615 = (0.0 - ((1.5075567228888176 * fConst614) + (3.2276019669290172 * fConst20)));
		fConst616 = (0.0 - ((1.0552897060221724 * fConst615) + (4.0 * fConst614)));
		fConst617 = (fConst34 * (0.0 - ((0.90453403373329044 * fConst616) + (2.5 * fConst615))));
		fConst618 = (fConst70 * fConst609);
		fConst619 = (fConst28 * fConst612);
		fConst620 = (fConst87 * fConst616);
		fConst621 = (fConst75 * fConst611);
		fConst622 = (fConst24 * fConst615);
		fConst623 = (fConst84 * fConst614);
		fConst624 = (fConst66 * (0.0 - (20.244443496069714 * fConst47)));
		fConst625 = (0.0 - (1.1313708498984762 * fConst621));
		fConst626 = (0.0 - (0.80498447189992428 * fConst618));
		fConst627 = (0.0 - (0.56568542494923812 * fConst619));
		fConst628 = (0.0 - (1.1627553482998907 * fConst618));
		fConst629 = (0.0 - (1.3576450198781711 * fConst624));
		fConst630 = (0.0 - (0.40249223594996214 * fConst610));
		fConst631 = (0.0 - (0.37712361663282534 * fConst613));
		fConst632 = (0.0 - (0.58137767414994534 * fConst610));
		fConst633 = (0.007988233320487842 * fConst624);
		fConst634 = (0.0057058809432056004 * fConst613);
		fConst635 = (0.0085588214148084006 * fConst619);
		fConst636 = (0.017117642829616801 * fConst621);
		fConst637 = (0.035118530106766334 * fConst42);
		fConst638 = (0.059595081393300449 * fConst49);
		fConst639 = (0.0 - (0.44721359549995782 * fConst623));
		fConst640 = (0.0 - (0.84852813742385691 * fConst621));
		fConst641 = (0.0 - (0.22360679774997891 * fConst622));
		fConst642 = (0.0 - (0.80498447189992484 * fConst618));
		fConst643 = (0.0 - (0.42426406871192845 * fConst619));
		fConst644 = (0.0 - (0.14907119849998593 * fConst620));
		fConst645 = (0.0 - (0.40249223594996242 * fConst610));
		fConst646 = (0.0 - (0.28284271247461895 * fConst613));
		fConst647 = (0.0 - (0.11180339887498945 * fConst617));
		fConst648 = (0.0 - (0.3771236166328254 * fConst613));
		fConst649 = (0.011728326894881708 * fConst610);
		fConst650 = (0.023456653789763415 * fConst618);
		fConst651 = (0.009021789919139777 * fConst620);
		fConst652 = (0.013532684878709665 * fConst622);
		fConst653 = (0.027065369757419331 * fConst623);
		fConst654 = (0.029118746888235927 * fConst45);
		fConst655 = (0.09176817201141016 * fConst39);
		fConst656 = (0.0067663424393548327 * fConst617);
		fConst657 = (0.0 - (1.3416407864998736 * fConst623));
		fConst658 = (0.0 - (0.84852813742385813 * fConst621));
		fConst659 = (0.0 - (0.67082039324993681 * fConst622));
		fConst660 = (0.0 - (0.42426406871192907 * fConst619));
		fConst661 = (0.0 - (0.44721359549995787 * fConst620));
		fConst662 = (0.0 - (0.8049844718999225 * fConst618));
		fConst663 = (0.0 - (1.3576450198781722 * fConst624));
		fConst664 = (0.0 - (0.28284271247461934 * fConst613));
		fConst665 = (0.0 - (0.3354101966249684 * fConst617));
		fConst666 = (0.0 - (0.40249223594996125 * fConst610));
		fConst667 = (0.0079882333204877847 * fConst624);
		fConst668 = (0.007607841257607462 * fConst613);
		fConst669 = (0.0022554474797849473 * fConst617);
		fConst670 = (0.01172832689488172 * fConst610);
		fConst671 = (0.011411761886411192 * fConst619);
		fConst672 = (0.0030072633063799293 * fConst620);
		fConst673 = (0.02345665378976344 * fConst618);
		fConst674 = (0.022823523772822384 * fConst621);
		fConst675 = (0.0045108949595698946 * fConst622);
		fConst676 = (0.0090217899191397891 * fConst623);
		fConst677 = (0.091768172011410271 * fConst39);
		fConst678 = (0.0097062489627453216 * fConst45);
		fConst679 = (0.059595081393300019 * fConst49);
		fConst680 = (0.046824706809021746 * fConst42);
		fConst681 = (0.0 - (0.44721359549995832 * fConst623));
		fConst682 = (0.0 - (1.3416407864998738 * fConst623));
		fConst683 = (0.0 - (0.22360679774997916 * fConst622));
		fConst684 = (0.0 - (0.67082039324993692 * fConst622));
		fConst685 = (0.0 - (1.1627553482998914 * fConst618));
		fConst686 = (0.0 - (0.1490711984999861 * fConst620));
		fConst687 = (0.0 - (0.44721359549995793 * fConst620));
		fConst688 = (0.0 - (0.80498447189992395 * fConst618));
		fConst689 = (0.0 - (0.58137767414994568 * fConst610));
		fConst690 = (0.0 - (0.11180339887498958 * fConst617));
		fConst691 = (0.0 - (0.33541019662496846 * fConst617));
		fConst692 = (0.0 - (0.40249223594996197 * fConst610));
		fConst693 = (0.0 - (0.44721359549995815 * fConst623));
		fConst694 = (0.0 - (0.22360679774997907 * fConst622));
		fConst695 = (0.0 - (1.1313708498984758 * fConst621));
		fConst696 = (0.0 - (0.14907119849998604 * fConst620));
		fConst697 = (0.0 - (0.5656854249492379 * fConst619));
		fConst698 = (0.0 - (1.3576450198781715 * fConst624));
		fConst699 = (0.0 - (0.11180339887498954 * fConst617));
		fConst700 = (0.0 - (0.37712361663282523 * fConst613));
		fConst701 = (0.0085588214148084041 * fConst619);
		fConst702 = (0.017117642829616808 * fConst621);
		fConst703 = (0.035118530106766348 * fConst42);
		fConst704 = (0.059595081393300338 * fConst49);
		fConst705 = (0.005705880943205603 * fConst613);
		fConst706 = (0.0079882333204878263 * fConst624);
		fConst707 = (0.0 - (1.3416407864998741 * fConst623));
		fConst708 = (0.0 - (0.84852813742385658 * fConst621));
		fConst709 = (0.0 - (0.67082039324993703 * fConst622));
		fConst710 = (0.0 - (1.1313708498984765 * fConst621));
		fConst711 = (0.0 - (0.42426406871192829 * fConst619));
		fConst712 = (0.0 - (0.44721359549995798 * fConst620));
		fConst713 = (0.0 - (0.56568542494923824 * fConst619));
		fConst714 = (0.0 - (1.1627553482998902 * fConst618));
		fConst715 = (0.0 - (0.28284271247461884 * fConst613));
		fConst716 = (0.0 - (0.33541019662496852 * fConst617));
		fConst717 = (0.0 - (0.37712361663282545 * fConst613));
		fConst718 = (0.0 - (0.58137767414994512 * fConst610));
		fConst719 = (0.0022554474797849425 * fConst617);
		fConst720 = (0.0081196109272258076 * fConst610);
		fConst721 = (0.0030072633063799232 * fConst620);
		fConst722 = (0.016239221854451615 * fConst618);
		fConst723 = (0.004510894959569885 * fConst622);
		fConst724 = (0.00902178991913977 * fConst623);
		fConst725 = (0.063531811392514792 * fConst39);
		fConst726 = (0.0097062489627453008 * fConst45);
		fConst727 = (0.016239221854451522 * fConst618);
		fConst728 = (0.011411761886411183 * fConst619);
		fConst729 = (0.0090217899191397718 * fConst620);
		fConst730 = (0.022823523772822367 * fConst621);
		fConst731 = (0.01353268487870966 * fConst622);
		fConst732 = (0.027065369757419321 * fConst623);
		fConst733 = (0.059595081393299554 * fConst49);
		fConst734 = (0.063531811392514445 * fConst39);
		fConst735 = (0.046824706809021698 * fConst42);
		fConst736 = (0.029118746888235913 * fConst45);
		fConst737 = (0.0067663424393548301 * fConst617);
		fConst738 = (0.0076078412576074551 * fConst613);
		fConst739 = (0.0081196109272257608 * fConst610);
		fConst740 = (0.0079882333204877223 * fConst624);
		fConst741 = (0.0 - (0.44721359549995937 * fConst623));
		fConst742 = (0.0 - (0.84852813742385946 * fConst621));
		fConst743 = (0.0 - (0.22360679774997969 * fConst622));
		fConst744 = (0.0 - (1.1627553482998934 * fConst618));
		fConst745 = (0.0 - (0.42426406871192973 * fConst619));
		fConst746 = (0.0 - (0.14907119849998646 * fConst620));
		fConst747 = (0.0 - (1.3576450198781731 * fConst624));
		fConst748 = (0.0 - (0.58137767414994668 * fConst610));
		fConst749 = (0.0 - (0.28284271247461978 * fConst613));
		fConst750 = (0.0 - (0.11180339887498984 * fConst617));
		fConst751 = (0.0 - (1.3056292813137786 * fConst20));
		fConst752 = (0.0 - ((4.5226701686664557 * fConst751) + (1.4434274804730605 * fConst20)));
		fConst753 = (0.0 - ((3.1658691180665191 * fConst752) + (4.0 * fConst751)));
		fConst754 = (fConst34 * (0.0 - ((2.7136021011998732 * fConst753) + (2.5 * fConst752))));
		fConst755 = (0.0 - (1.8557406538063874 * fConst26));
		fConst756 = (0.0 - ((6.3317382361330381 * fConst755) + (2.0515984856282001 * fConst26)));
		fConst757 = (fConst79 * (0.0 - ((4.0704031517998098 * fConst756) + (6.0 * fConst755))));
		fConst758 = (0.009509801572009334 * fConst757);
		fConst759 = (0.0 - (2.2156133258658013 * fConst30));
		fConst760 = (fConst32 * (0.0 - ((8.1408063035996197 * fConst759) + (2.449452694136097 * fConst30))));
		fConst761 = (0.010086667768959431 * fConst760);
		fConst762 = (fConst753 * fConst87);
		fConst763 = (fConst756 * fConst28);
		fConst764 = (0.014264702358014003 * fConst763);
		fConst765 = (fConst759 * fConst70);
		fConst766 = (0.020173335537918863 * fConst765);
		fConst767 = (fConst752 * fConst24);
		fConst768 = (fConst755 * fConst75);
		fConst769 = (0.028529404716028006 * fConst768);
		fConst770 = (fConst751 * fConst84);
		fConst771 = (0.011706176702255417 * fConst42);
		fConst772 = (0.0070590901547238332 * fConst39);
		fConst773 = (0.0 - (3.1401849173675503e-16 * fConst768));
		fConst774 = (0.0 - (1.5700924586837752e-16 * fConst763));
		fConst775 = (0.0 - (1.0000000000000004 * fConst765));
		fConst776 = (fConst66 * (0.0 - (2.4293332195283566 * fConst47)));
		fConst777 = (0.0 - (6.6613381477509402e-16 * fConst776));
		fConst778 = (0.0 - (1.0467283057891834e-16 * fConst757));
		fConst779 = (0.0 - (0.50000000000000022 * fConst760));
		fConst780 = (0.0 - (1.4142135623730951 * fConst776));
		fConst781 = (0.0 - fConst770);
		fConst782 = (0.0 - (1.4142135623730951 * fConst768));
		fConst783 = (0.0 - (0.5 * fConst767));
		fConst784 = (0.0 - (0.70710678118654757 * fConst763));
		fConst785 = (0.0 - (0.33333333333333331 * fConst762));
		fConst786 = (0.0 - (0.47140452079103168 * fConst757));
		fConst787 = (0.0 - (0.25 * fConst754));
		fConst788 = (1.343814095861187e-17 * fConst776);
		fConst789 = (0.0050433338844797183 * fConst754);
		fConst790 = (0.0067244451793062911 * fConst762);
		fConst791 = (0.010086667768959437 * fConst767);
		fConst792 = (0.020173335537918873 * fConst770);
		fConst793 = (4.0101337658685596e-18 * fConst49);
		fConst794 = (0.0097062489627452973 * fConst45);
		fConst795 = (0.0 - (1.0000000000000002 * fConst770));
		fConst796 = (0.0 - (0.50000000000000011 * fConst767));
		fConst797 = (0.0 - (1.0000000000000009 * fConst765));
		fConst798 = (0.0 - (0.33333333333333343 * fConst762));
		fConst799 = (0.0 - (0.50000000000000044 * fConst760));
		fConst800 = (0.0 - (0.25000000000000006 * fConst754));
		fConst801 = (0.010086667768959428 * fConst760);
		fConst802 = (2.6876281917223736e-17 * fConst776);
		fConst803 = (0.020173335537918856 * fConst765);
		fConst804 = (8.0202675317371177e-18 * fConst49);
		fConst805 = (0.0070590901547238306 * fConst39);
		fConst806 = (2.6876281917223739e-17 * fConst776);
		fConst807 = (0.0067244451793062885 * fConst762);
		fConst808 = (0.010086667768959433 * fConst767);
		fConst809 = (0.020173335537918866 * fConst770);
		fConst810 = (8.0202675317371192e-18 * fConst49);
		fConst811 = (0.0097062489627452939 * fConst45);
		fConst812 = (0.0050433338844797166 * fConst754);
		fConst813 = (0.0 - (6.2803698347351007e-16 * fConst768));
		fConst814 = (0.0 - (0.99999999999999933 * fConst765));
		fConst815 = (0.0 - (3.1401849173675503e-16 * fConst763));
		fConst816 = (0.0 - (1.0000000000000007 * fConst765));
		fConst817 = (0.0 - (0.49999999999999967 * fConst760));
		fConst818 = (0.0 - (2.0934566115783667e-16 * fConst757));
		fConst819 = (0.0 - (0.50000000000000033 * fConst760));
		
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
			fRec28[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec29[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec27[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec30[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec31[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec26[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec32[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec25[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec24[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec0[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec37[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec38[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec36[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec39[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec35[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec34[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec43[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec44[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec42[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec45[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec46[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec41[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec40[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec51[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec52[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec50[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec53[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec54[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec49[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec55[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec48[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec47[l54] = 0.0;
			
		}
		for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
			fRec60[l55] = 0.0;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fRec61[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec59[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec62[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec63[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec58[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec64[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fRec57[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec56[l63] = 0.0;
			
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
			fRec74[l71] = 0.0;
			
		}
		for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
			fRec72[l72] = 0.0;
			
		}
		for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
			fRec71[l73] = 0.0;
			
		}
		for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
			fRec77[l74] = 0.0;
			
		}
		for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
			fRec76[l75] = 0.0;
			
		}
		for (int l76 = 0; (l76 < 2); l76 = (l76 + 1)) {
			fRec75[l76] = 0.0;
			
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
			fRec89[l84] = 0.0;
			
		}
		for (int l85 = 0; (l85 < 2); l85 = (l85 + 1)) {
			fRec90[l85] = 0.0;
			
		}
		for (int l86 = 0; (l86 < 2); l86 = (l86 + 1)) {
			fRec88[l86] = 0.0;
			
		}
		for (int l87 = 0; (l87 < 2); l87 = (l87 + 1)) {
			fRec91[l87] = 0.0;
			
		}
		for (int l88 = 0; (l88 < 2); l88 = (l88 + 1)) {
			fRec92[l88] = 0.0;
			
		}
		for (int l89 = 0; (l89 < 2); l89 = (l89 + 1)) {
			fRec87[l89] = 0.0;
			
		}
		for (int l90 = 0; (l90 < 2); l90 = (l90 + 1)) {
			fRec93[l90] = 0.0;
			
		}
		for (int l91 = 0; (l91 < 2); l91 = (l91 + 1)) {
			fRec86[l91] = 0.0;
			
		}
		for (int l92 = 0; (l92 < 2); l92 = (l92 + 1)) {
			fRec85[l92] = 0.0;
			
		}
		for (int l93 = 0; (l93 < 2); l93 = (l93 + 1)) {
			fRec33[l93] = 0.0;
			
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
			fRec96[l98] = 0.0;
			
		}
		for (int l99 = 0; (l99 < 2); l99 = (l99 + 1)) {
			fRec95[l99] = 0.0;
			
		}
		for (int l100 = 0; (l100 < 2); l100 = (l100 + 1)) {
			fRec105[l100] = 0.0;
			
		}
		for (int l101 = 0; (l101 < 2); l101 = (l101 + 1)) {
			fRec106[l101] = 0.0;
			
		}
		for (int l102 = 0; (l102 < 2); l102 = (l102 + 1)) {
			fRec104[l102] = 0.0;
			
		}
		for (int l103 = 0; (l103 < 2); l103 = (l103 + 1)) {
			fRec107[l103] = 0.0;
			
		}
		for (int l104 = 0; (l104 < 2); l104 = (l104 + 1)) {
			fRec108[l104] = 0.0;
			
		}
		for (int l105 = 0; (l105 < 2); l105 = (l105 + 1)) {
			fRec103[l105] = 0.0;
			
		}
		for (int l106 = 0; (l106 < 2); l106 = (l106 + 1)) {
			fRec109[l106] = 0.0;
			
		}
		for (int l107 = 0; (l107 < 2); l107 = (l107 + 1)) {
			fRec102[l107] = 0.0;
			
		}
		for (int l108 = 0; (l108 < 2); l108 = (l108 + 1)) {
			fRec101[l108] = 0.0;
			
		}
		for (int l109 = 0; (l109 < 2); l109 = (l109 + 1)) {
			fRec113[l109] = 0.0;
			
		}
		for (int l110 = 0; (l110 < 2); l110 = (l110 + 1)) {
			fRec114[l110] = 0.0;
			
		}
		for (int l111 = 0; (l111 < 2); l111 = (l111 + 1)) {
			fRec112[l111] = 0.0;
			
		}
		for (int l112 = 0; (l112 < 2); l112 = (l112 + 1)) {
			fRec115[l112] = 0.0;
			
		}
		for (int l113 = 0; (l113 < 2); l113 = (l113 + 1)) {
			fRec111[l113] = 0.0;
			
		}
		for (int l114 = 0; (l114 < 2); l114 = (l114 + 1)) {
			fRec110[l114] = 0.0;
			
		}
		for (int l115 = 0; (l115 < 2); l115 = (l115 + 1)) {
			fRec118[l115] = 0.0;
			
		}
		for (int l116 = 0; (l116 < 2); l116 = (l116 + 1)) {
			fRec117[l116] = 0.0;
			
		}
		for (int l117 = 0; (l117 < 2); l117 = (l117 + 1)) {
			fRec116[l117] = 0.0;
			
		}
		for (int l118 = 0; (l118 < 2); l118 = (l118 + 1)) {
			fRec123[l118] = 0.0;
			
		}
		for (int l119 = 0; (l119 < 2); l119 = (l119 + 1)) {
			fRec124[l119] = 0.0;
			
		}
		for (int l120 = 0; (l120 < 2); l120 = (l120 + 1)) {
			fRec122[l120] = 0.0;
			
		}
		for (int l121 = 0; (l121 < 2); l121 = (l121 + 1)) {
			fRec125[l121] = 0.0;
			
		}
		for (int l122 = 0; (l122 < 2); l122 = (l122 + 1)) {
			fRec126[l122] = 0.0;
			
		}
		for (int l123 = 0; (l123 < 2); l123 = (l123 + 1)) {
			fRec121[l123] = 0.0;
			
		}
		for (int l124 = 0; (l124 < 2); l124 = (l124 + 1)) {
			fRec127[l124] = 0.0;
			
		}
		for (int l125 = 0; (l125 < 2); l125 = (l125 + 1)) {
			fRec120[l125] = 0.0;
			
		}
		for (int l126 = 0; (l126 < 2); l126 = (l126 + 1)) {
			fRec119[l126] = 0.0;
			
		}
		for (int l127 = 0; (l127 < 2); l127 = (l127 + 1)) {
			fRec132[l127] = 0.0;
			
		}
		for (int l128 = 0; (l128 < 2); l128 = (l128 + 1)) {
			fRec133[l128] = 0.0;
			
		}
		for (int l129 = 0; (l129 < 2); l129 = (l129 + 1)) {
			fRec131[l129] = 0.0;
			
		}
		for (int l130 = 0; (l130 < 2); l130 = (l130 + 1)) {
			fRec134[l130] = 0.0;
			
		}
		for (int l131 = 0; (l131 < 2); l131 = (l131 + 1)) {
			fRec135[l131] = 0.0;
			
		}
		for (int l132 = 0; (l132 < 2); l132 = (l132 + 1)) {
			fRec130[l132] = 0.0;
			
		}
		for (int l133 = 0; (l133 < 2); l133 = (l133 + 1)) {
			fRec136[l133] = 0.0;
			
		}
		for (int l134 = 0; (l134 < 2); l134 = (l134 + 1)) {
			fRec129[l134] = 0.0;
			
		}
		for (int l135 = 0; (l135 < 2); l135 = (l135 + 1)) {
			fRec128[l135] = 0.0;
			
		}
		for (int l136 = 0; (l136 < 2); l136 = (l136 + 1)) {
			fRec94[l136] = 0.0;
			
		}
		for (int l137 = 0; (l137 < 2); l137 = (l137 + 1)) {
			fRec137[l137] = 0.0;
			
		}
		for (int l138 = 0; (l138 < 2); l138 = (l138 + 1)) {
			fRec138[l138] = 0.0;
			
		}
		for (int l139 = 0; (l139 < 2); l139 = (l139 + 1)) {
			fRec139[l139] = 0.0;
			
		}
		for (int l140 = 0; (l140 < 2); l140 = (l140 + 1)) {
			fRec145[l140] = 0.0;
			
		}
		for (int l141 = 0; (l141 < 2); l141 = (l141 + 1)) {
			fRec146[l141] = 0.0;
			
		}
		for (int l142 = 0; (l142 < 2); l142 = (l142 + 1)) {
			fRec144[l142] = 0.0;
			
		}
		for (int l143 = 0; (l143 < 2); l143 = (l143 + 1)) {
			fRec147[l143] = 0.0;
			
		}
		for (int l144 = 0; (l144 < 2); l144 = (l144 + 1)) {
			fRec148[l144] = 0.0;
			
		}
		for (int l145 = 0; (l145 < 2); l145 = (l145 + 1)) {
			fRec143[l145] = 0.0;
			
		}
		for (int l146 = 0; (l146 < 2); l146 = (l146 + 1)) {
			fRec149[l146] = 0.0;
			
		}
		for (int l147 = 0; (l147 < 2); l147 = (l147 + 1)) {
			fRec142[l147] = 0.0;
			
		}
		for (int l148 = 0; (l148 < 2); l148 = (l148 + 1)) {
			fRec141[l148] = 0.0;
			
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
			fRec156[l153] = 0.0;
			
		}
		for (int l154 = 0; (l154 < 2); l154 = (l154 + 1)) {
			fRec151[l154] = 0.0;
			
		}
		for (int l155 = 0; (l155 < 2); l155 = (l155 + 1)) {
			fRec150[l155] = 0.0;
			
		}
		for (int l156 = 0; (l156 < 2); l156 = (l156 + 1)) {
			fRec160[l156] = 0.0;
			
		}
		for (int l157 = 0; (l157 < 2); l157 = (l157 + 1)) {
			fRec161[l157] = 0.0;
			
		}
		for (int l158 = 0; (l158 < 2); l158 = (l158 + 1)) {
			fRec159[l158] = 0.0;
			
		}
		for (int l159 = 0; (l159 < 2); l159 = (l159 + 1)) {
			fRec162[l159] = 0.0;
			
		}
		for (int l160 = 0; (l160 < 2); l160 = (l160 + 1)) {
			fRec158[l160] = 0.0;
			
		}
		for (int l161 = 0; (l161 < 2); l161 = (l161 + 1)) {
			fRec157[l161] = 0.0;
			
		}
		for (int l162 = 0; (l162 < 2); l162 = (l162 + 1)) {
			fRec167[l162] = 0.0;
			
		}
		for (int l163 = 0; (l163 < 2); l163 = (l163 + 1)) {
			fRec168[l163] = 0.0;
			
		}
		for (int l164 = 0; (l164 < 2); l164 = (l164 + 1)) {
			fRec166[l164] = 0.0;
			
		}
		for (int l165 = 0; (l165 < 2); l165 = (l165 + 1)) {
			fRec169[l165] = 0.0;
			
		}
		for (int l166 = 0; (l166 < 2); l166 = (l166 + 1)) {
			fRec170[l166] = 0.0;
			
		}
		for (int l167 = 0; (l167 < 2); l167 = (l167 + 1)) {
			fRec165[l167] = 0.0;
			
		}
		for (int l168 = 0; (l168 < 2); l168 = (l168 + 1)) {
			fRec171[l168] = 0.0;
			
		}
		for (int l169 = 0; (l169 < 2); l169 = (l169 + 1)) {
			fRec164[l169] = 0.0;
			
		}
		for (int l170 = 0; (l170 < 2); l170 = (l170 + 1)) {
			fRec163[l170] = 0.0;
			
		}
		for (int l171 = 0; (l171 < 2); l171 = (l171 + 1)) {
			fRec174[l171] = 0.0;
			
		}
		for (int l172 = 0; (l172 < 2); l172 = (l172 + 1)) {
			fRec175[l172] = 0.0;
			
		}
		for (int l173 = 0; (l173 < 2); l173 = (l173 + 1)) {
			fRec173[l173] = 0.0;
			
		}
		for (int l174 = 0; (l174 < 2); l174 = (l174 + 1)) {
			fRec172[l174] = 0.0;
			
		}
		for (int l175 = 0; (l175 < 2); l175 = (l175 + 1)) {
			fRec179[l175] = 0.0;
			
		}
		for (int l176 = 0; (l176 < 2); l176 = (l176 + 1)) {
			fRec180[l176] = 0.0;
			
		}
		for (int l177 = 0; (l177 < 2); l177 = (l177 + 1)) {
			fRec178[l177] = 0.0;
			
		}
		for (int l178 = 0; (l178 < 2); l178 = (l178 + 1)) {
			fRec181[l178] = 0.0;
			
		}
		for (int l179 = 0; (l179 < 2); l179 = (l179 + 1)) {
			fRec182[l179] = 0.0;
			
		}
		for (int l180 = 0; (l180 < 2); l180 = (l180 + 1)) {
			fRec177[l180] = 0.0;
			
		}
		for (int l181 = 0; (l181 < 2); l181 = (l181 + 1)) {
			fRec176[l181] = 0.0;
			
		}
		for (int l182 = 0; (l182 < 2); l182 = (l182 + 1)) {
			fRec140[l182] = 0.0;
			
		}
		for (int l183 = 0; (l183 < 2); l183 = (l183 + 1)) {
			fRec187[l183] = 0.0;
			
		}
		for (int l184 = 0; (l184 < 2); l184 = (l184 + 1)) {
			fRec188[l184] = 0.0;
			
		}
		for (int l185 = 0; (l185 < 2); l185 = (l185 + 1)) {
			fRec186[l185] = 0.0;
			
		}
		for (int l186 = 0; (l186 < 2); l186 = (l186 + 1)) {
			fRec189[l186] = 0.0;
			
		}
		for (int l187 = 0; (l187 < 2); l187 = (l187 + 1)) {
			fRec190[l187] = 0.0;
			
		}
		for (int l188 = 0; (l188 < 2); l188 = (l188 + 1)) {
			fRec185[l188] = 0.0;
			
		}
		for (int l189 = 0; (l189 < 2); l189 = (l189 + 1)) {
			fRec184[l189] = 0.0;
			
		}
		for (int l190 = 0; (l190 < 2); l190 = (l190 + 1)) {
			fRec193[l190] = 0.0;
			
		}
		for (int l191 = 0; (l191 < 2); l191 = (l191 + 1)) {
			fRec194[l191] = 0.0;
			
		}
		for (int l192 = 0; (l192 < 2); l192 = (l192 + 1)) {
			fRec192[l192] = 0.0;
			
		}
		for (int l193 = 0; (l193 < 2); l193 = (l193 + 1)) {
			fRec191[l193] = 0.0;
			
		}
		for (int l194 = 0; (l194 < 2); l194 = (l194 + 1)) {
			fRec198[l194] = 0.0;
			
		}
		for (int l195 = 0; (l195 < 2); l195 = (l195 + 1)) {
			fRec199[l195] = 0.0;
			
		}
		for (int l196 = 0; (l196 < 2); l196 = (l196 + 1)) {
			fRec197[l196] = 0.0;
			
		}
		for (int l197 = 0; (l197 < 2); l197 = (l197 + 1)) {
			fRec200[l197] = 0.0;
			
		}
		for (int l198 = 0; (l198 < 2); l198 = (l198 + 1)) {
			fRec201[l198] = 0.0;
			
		}
		for (int l199 = 0; (l199 < 2); l199 = (l199 + 1)) {
			fRec196[l199] = 0.0;
			
		}
		for (int l200 = 0; (l200 < 2); l200 = (l200 + 1)) {
			fRec195[l200] = 0.0;
			
		}
		for (int l201 = 0; (l201 < 2); l201 = (l201 + 1)) {
			fRec183[l201] = 0.0;
			
		}
		for (int l202 = 0; (l202 < 2); l202 = (l202 + 1)) {
			fRec202[l202] = 0.0;
			
		}
		for (int l203 = 0; (l203 < 2); l203 = (l203 + 1)) {
			fRec203[l203] = 0.0;
			
		}
		for (int l204 = 0; (l204 < 2); l204 = (l204 + 1)) {
			fRec208[l204] = 0.0;
			
		}
		for (int l205 = 0; (l205 < 2); l205 = (l205 + 1)) {
			fRec209[l205] = 0.0;
			
		}
		for (int l206 = 0; (l206 < 2); l206 = (l206 + 1)) {
			fRec207[l206] = 0.0;
			
		}
		for (int l207 = 0; (l207 < 2); l207 = (l207 + 1)) {
			fRec210[l207] = 0.0;
			
		}
		for (int l208 = 0; (l208 < 2); l208 = (l208 + 1)) {
			fRec211[l208] = 0.0;
			
		}
		for (int l209 = 0; (l209 < 2); l209 = (l209 + 1)) {
			fRec206[l209] = 0.0;
			
		}
		for (int l210 = 0; (l210 < 2); l210 = (l210 + 1)) {
			fRec205[l210] = 0.0;
			
		}
		for (int l211 = 0; (l211 < 2); l211 = (l211 + 1)) {
			fRec215[l211] = 0.0;
			
		}
		for (int l212 = 0; (l212 < 2); l212 = (l212 + 1)) {
			fRec216[l212] = 0.0;
			
		}
		for (int l213 = 0; (l213 < 2); l213 = (l213 + 1)) {
			fRec214[l213] = 0.0;
			
		}
		for (int l214 = 0; (l214 < 2); l214 = (l214 + 1)) {
			fRec217[l214] = 0.0;
			
		}
		for (int l215 = 0; (l215 < 2); l215 = (l215 + 1)) {
			fRec218[l215] = 0.0;
			
		}
		for (int l216 = 0; (l216 < 2); l216 = (l216 + 1)) {
			fRec213[l216] = 0.0;
			
		}
		for (int l217 = 0; (l217 < 2); l217 = (l217 + 1)) {
			fRec212[l217] = 0.0;
			
		}
		for (int l218 = 0; (l218 < 2); l218 = (l218 + 1)) {
			fRec221[l218] = 0.0;
			
		}
		for (int l219 = 0; (l219 < 2); l219 = (l219 + 1)) {
			fRec222[l219] = 0.0;
			
		}
		for (int l220 = 0; (l220 < 2); l220 = (l220 + 1)) {
			fRec220[l220] = 0.0;
			
		}
		for (int l221 = 0; (l221 < 2); l221 = (l221 + 1)) {
			fRec219[l221] = 0.0;
			
		}
		for (int l222 = 0; (l222 < 2); l222 = (l222 + 1)) {
			fRec204[l222] = 0.0;
			
		}
		for (int l223 = 0; (l223 < 2); l223 = (l223 + 1)) {
			fRec223[l223] = 0.0;
			
		}
		for (int l224 = 0; (l224 < 2); l224 = (l224 + 1)) {
			fRec224[l224] = 0.0;
			
		}
		for (int l225 = 0; (l225 < 2); l225 = (l225 + 1)) {
			fRec225[l225] = 0.0;
			
		}
		for (int l226 = 0; (l226 < 2); l226 = (l226 + 1)) {
			fRec226[l226] = 0.0;
			
		}
		for (int l227 = 0; (l227 < 2); l227 = (l227 + 1)) {
			fRec227[l227] = 0.0;
			
		}
		for (int l228 = 0; (l228 < 2); l228 = (l228 + 1)) {
			fRec228[l228] = 0.0;
			
		}
		for (int l229 = 0; (l229 < 2); l229 = (l229 + 1)) {
			fRec229[l229] = 0.0;
			
		}
		for (int l230 = 0; (l230 < 2); l230 = (l230 + 1)) {
			fRec234[l230] = 0.0;
			
		}
		for (int l231 = 0; (l231 < 2); l231 = (l231 + 1)) {
			fRec235[l231] = 0.0;
			
		}
		for (int l232 = 0; (l232 < 2); l232 = (l232 + 1)) {
			fRec233[l232] = 0.0;
			
		}
		for (int l233 = 0; (l233 < 2); l233 = (l233 + 1)) {
			fRec236[l233] = 0.0;
			
		}
		for (int l234 = 0; (l234 < 2); l234 = (l234 + 1)) {
			fRec232[l234] = 0.0;
			
		}
		for (int l235 = 0; (l235 < 2); l235 = (l235 + 1)) {
			fRec231[l235] = 0.0;
			
		}
		for (int l236 = 0; (l236 < 2); l236 = (l236 + 1)) {
			fRec241[l236] = 0.0;
			
		}
		for (int l237 = 0; (l237 < 2); l237 = (l237 + 1)) {
			fRec242[l237] = 0.0;
			
		}
		for (int l238 = 0; (l238 < 2); l238 = (l238 + 1)) {
			fRec240[l238] = 0.0;
			
		}
		for (int l239 = 0; (l239 < 2); l239 = (l239 + 1)) {
			fRec243[l239] = 0.0;
			
		}
		for (int l240 = 0; (l240 < 2); l240 = (l240 + 1)) {
			fRec244[l240] = 0.0;
			
		}
		for (int l241 = 0; (l241 < 2); l241 = (l241 + 1)) {
			fRec239[l241] = 0.0;
			
		}
		for (int l242 = 0; (l242 < 2); l242 = (l242 + 1)) {
			fRec245[l242] = 0.0;
			
		}
		for (int l243 = 0; (l243 < 2); l243 = (l243 + 1)) {
			fRec238[l243] = 0.0;
			
		}
		for (int l244 = 0; (l244 < 2); l244 = (l244 + 1)) {
			fRec237[l244] = 0.0;
			
		}
		for (int l245 = 0; (l245 < 2); l245 = (l245 + 1)) {
			fRec250[l245] = 0.0;
			
		}
		for (int l246 = 0; (l246 < 2); l246 = (l246 + 1)) {
			fRec251[l246] = 0.0;
			
		}
		for (int l247 = 0; (l247 < 2); l247 = (l247 + 1)) {
			fRec249[l247] = 0.0;
			
		}
		for (int l248 = 0; (l248 < 2); l248 = (l248 + 1)) {
			fRec252[l248] = 0.0;
			
		}
		for (int l249 = 0; (l249 < 2); l249 = (l249 + 1)) {
			fRec253[l249] = 0.0;
			
		}
		for (int l250 = 0; (l250 < 2); l250 = (l250 + 1)) {
			fRec248[l250] = 0.0;
			
		}
		for (int l251 = 0; (l251 < 2); l251 = (l251 + 1)) {
			fRec254[l251] = 0.0;
			
		}
		for (int l252 = 0; (l252 < 2); l252 = (l252 + 1)) {
			fRec247[l252] = 0.0;
			
		}
		for (int l253 = 0; (l253 < 2); l253 = (l253 + 1)) {
			fRec246[l253] = 0.0;
			
		}
		for (int l254 = 0; (l254 < 2); l254 = (l254 + 1)) {
			fRec230[l254] = 0.0;
			
		}
		for (int l255 = 0; (l255 < 2); l255 = (l255 + 1)) {
			fRec255[l255] = 0.0;
			
		}
		for (int l256 = 0; (l256 < 2); l256 = (l256 + 1)) {
			fRec256[l256] = 0.0;
			
		}
		for (int l257 = 0; (l257 < 2); l257 = (l257 + 1)) {
			fRec257[l257] = 0.0;
			
		}
		for (int l258 = 0; (l258 < 2); l258 = (l258 + 1)) {
			fRec258[l258] = 0.0;
			
		}
		for (int l259 = 0; (l259 < 2); l259 = (l259 + 1)) {
			fRec259[l259] = 0.0;
			
		}
		for (int l260 = 0; (l260 < 2); l260 = (l260 + 1)) {
			fRec260[l260] = 0.0;
			
		}
		for (int l261 = 0; (l261 < 2); l261 = (l261 + 1)) {
			fRec261[l261] = 0.0;
			
		}
		for (int l262 = 0; (l262 < 2); l262 = (l262 + 1)) {
			fRec262[l262] = 0.0;
			
		}
		for (int l263 = 0; (l263 < 2); l263 = (l263 + 1)) {
			fRec263[l263] = 0.0;
			
		}
		for (int l264 = 0; (l264 < 2); l264 = (l264 + 1)) {
			fRec264[l264] = 0.0;
			
		}
		for (int l265 = 0; (l265 < 2); l265 = (l265 + 1)) {
			fRec265[l265] = 0.0;
			
		}
		for (int l266 = 0; (l266 < 2); l266 = (l266 + 1)) {
			fRec266[l266] = 0.0;
			
		}
		for (int l267 = 0; (l267 < 2); l267 = (l267 + 1)) {
			fRec267[l267] = 0.0;
			
		}
		for (int l268 = 0; (l268 < 2); l268 = (l268 + 1)) {
			fRec268[l268] = 0.0;
			
		}
		for (int l269 = 0; (l269 < 2); l269 = (l269 + 1)) {
			fRec269[l269] = 0.0;
			
		}
		for (int l270 = 0; (l270 < 2); l270 = (l270 + 1)) {
			fRec270[l270] = 0.0;
			
		}
		for (int l271 = 0; (l271 < 2); l271 = (l271 + 1)) {
			fRec271[l271] = 0.0;
			
		}
		for (int l272 = 0; (l272 < 2); l272 = (l272 + 1)) {
			fRec272[l272] = 0.0;
			
		}
		for (int l273 = 0; (l273 < 2); l273 = (l273 + 1)) {
			fRec273[l273] = 0.0;
			
		}
		for (int l274 = 0; (l274 < 2); l274 = (l274 + 1)) {
			fRec274[l274] = 0.0;
			
		}
		for (int l275 = 0; (l275 < 2); l275 = (l275 + 1)) {
			fRec275[l275] = 0.0;
			
		}
		for (int l276 = 0; (l276 < 2); l276 = (l276 + 1)) {
			fRec276[l276] = 0.0;
			
		}
		for (int l277 = 0; (l277 < 2); l277 = (l277 + 1)) {
			fRec277[l277] = 0.0;
			
		}
		for (int l278 = 0; (l278 < 2); l278 = (l278 + 1)) {
			fRec278[l278] = 0.0;
			
		}
		for (int l279 = 0; (l279 < 2); l279 = (l279 + 1)) {
			fRec279[l279] = 0.0;
			
		}
		for (int l280 = 0; (l280 < 2); l280 = (l280 + 1)) {
			fRec280[l280] = 0.0;
			
		}
		for (int l281 = 0; (l281 < 2); l281 = (l281 + 1)) {
			fRec281[l281] = 0.0;
			
		}
		for (int l282 = 0; (l282 < 2); l282 = (l282 + 1)) {
			fRec282[l282] = 0.0;
			
		}
		for (int l283 = 0; (l283 < 2); l283 = (l283 + 1)) {
			fRec283[l283] = 0.0;
			
		}
		for (int l284 = 0; (l284 < 2); l284 = (l284 + 1)) {
			fRec284[l284] = 0.0;
			
		}
		for (int l285 = 0; (l285 < 2); l285 = (l285 + 1)) {
			fRec285[l285] = 0.0;
			
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
		ui_interface->openVerticalBox("HOADecLebedev505");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("B-Format");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x28d0380", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a150e0", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x28db6b0", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x29a99e0", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b4f7d0", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2af6230", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x28e9f50", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ab76c0", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x299ec10", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x29e7e50", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bd13a0", &fVbargraph51, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a0bb40", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x28fca30", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2943580", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a92a70", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2992280", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b43580", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ae7030", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b32810", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b06670", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2911f30", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ac9670", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2956160", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a811a0", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x29bc810", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5");
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a28ab0", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c00a60", &fVbargraph53, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x29fcbb0", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2be7040", &fVbargraph52, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a3c5d0", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x292c2d0", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2981b80", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2aa8bf0", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x296c460", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a6def0", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x29d27b0", &fVbargraph15, -70.0, 6.0);
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
		ui_interface->declare(&fVbargraph6, "osc", "/output1 -70 6");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2931510", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph16, "osc", "/output2 -70 6");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x29d8140", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph23, "osc", "/output3 -70 6");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a418c0", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph24, "osc", "/output4 -70 6");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a4aff0", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph25, "osc", "/output5 -70 6");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a52410", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph26, "osc", "/output6 -70 6");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a57db0", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph33, "osc", "/output7 -70 6");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ad2b70", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph37, "osc", "/output8 -70 6");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b0cac0", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph38, "osc", "/output9 -70 6");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b18fa0", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph39, "osc", "/output10 -70 6");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b21860", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph43, "osc", "/output11 -70 6");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b5c9c0", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph44, "osc", "/output12 -70 6");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b6d0d0", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph45, "osc", "/output13 -70 6");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b7bdf0", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph46, "osc", "/output14 -70 6");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b8b520", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph47, "osc", "/output15 -70 6");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ba0b50", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph48, "osc", "/output16 -70 6");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bad260", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph49, "osc", "/output17 -70 6");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bb8180", &fVbargraph49, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph50, "osc", "/output18 -70 6");
		ui_interface->declare(&fVbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bc0c60", &fVbargraph50, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph54, "osc", "/output19 -70 6");
		ui_interface->declare(&fVbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c13c10", &fVbargraph54, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph55, "osc", "/output20 -70 6");
		ui_interface->declare(&fVbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c2e680", &fVbargraph55, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph56, "osc", "/output21 -70 6");
		ui_interface->declare(&fVbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c46090", &fVbargraph56, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph57, "osc", "/output22 -70 6");
		ui_interface->declare(&fVbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c5cff0", &fVbargraph57, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph58, "osc", "/output23 -70 6");
		ui_interface->declare(&fVbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c78370", &fVbargraph58, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph59, "osc", "/output24 -70 6");
		ui_interface->declare(&fVbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c8bd40", &fVbargraph59, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph60, "osc", "/output25 -70 6");
		ui_interface->declare(&fVbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c9dcb0", &fVbargraph60, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph61, "osc", "/output26 -70 6");
		ui_interface->declare(&fVbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2caf470", &fVbargraph61, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph62, "osc", "/output27 -70 6");
		ui_interface->declare(&fVbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2cce860", &fVbargraph62, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph63, "osc", "/output28 -70 6");
		ui_interface->declare(&fVbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ce2640", &fVbargraph63, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph64, "osc", "/output29 -70 6");
		ui_interface->declare(&fVbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2cf9030", &fVbargraph64, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph65, "osc", "/output30 -70 6");
		ui_interface->declare(&fVbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d0e720", &fVbargraph65, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph66, "osc", "/output31 -70 6");
		ui_interface->declare(&fVbargraph66, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d2c9c0", &fVbargraph66, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph67, "osc", "/output32 -70 6");
		ui_interface->declare(&fVbargraph67, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d46ef0", &fVbargraph67, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph68, "osc", "/output33 -70 6");
		ui_interface->declare(&fVbargraph68, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d626a0", &fVbargraph68, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph69, "osc", "/output34 -70 6");
		ui_interface->declare(&fVbargraph69, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d7c490", &fVbargraph69, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph70, "osc", "/output35 -70 6");
		ui_interface->declare(&fVbargraph70, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d96700", &fVbargraph70, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph71, "osc", "/output36 -70 6");
		ui_interface->declare(&fVbargraph71, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2daea80", &fVbargraph71, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph72, "osc", "/output37 -70 6");
		ui_interface->declare(&fVbargraph72, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2dca200", &fVbargraph72, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph73, "osc", "/output38 -70 6");
		ui_interface->declare(&fVbargraph73, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2de6dd0", &fVbargraph73, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph74, "osc", "/output39 -70 6");
		ui_interface->declare(&fVbargraph74, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2dfc640", &fVbargraph74, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph75, "osc", "/output40 -70 6");
		ui_interface->declare(&fVbargraph75, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2e0e050", &fVbargraph75, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph76, "osc", "/output41 -70 6");
		ui_interface->declare(&fVbargraph76, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2e216a0", &fVbargraph76, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph77, "osc", "/output42 -70 6");
		ui_interface->declare(&fVbargraph77, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2e36d00", &fVbargraph77, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph78, "osc", "/output43 -70 6");
		ui_interface->declare(&fVbargraph78, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2e484d0", &fVbargraph78, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph79, "osc", "/output44 -70 6");
		ui_interface->declare(&fVbargraph79, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2e5a5c0", &fVbargraph79, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph80, "osc", "/output45 -70 6");
		ui_interface->declare(&fVbargraph80, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2e6f180", &fVbargraph80, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph81, "osc", "/output46 -70 6");
		ui_interface->declare(&fVbargraph81, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2e850e0", &fVbargraph81, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph82, "osc", "/output47 -70 6");
		ui_interface->declare(&fVbargraph82, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ea36f0", &fVbargraph82, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph83, "osc", "/output48 -70 6");
		ui_interface->declare(&fVbargraph83, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2eb5450", &fVbargraph83, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph84, "osc", "/output49 -70 6");
		ui_interface->declare(&fVbargraph84, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ec6de0", &fVbargraph84, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph85, "osc", "/output50 -70 6");
		ui_interface->declare(&fVbargraph85, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ed8890", &fVbargraph85, -70.0, 6.0);
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
		double fSlow25 = (1.0 / ((fConst14 / fSlow2) + 1.0));
		double fSlow26 = (1.0 / (((fConst15 / fSlow2) + (14.272480513280099 * fSlow10)) + 1.0));
		double fSlow27 = (1.0 / (((fConst16 / fSlow2) + (18.1563153134523 * fSlow10)) + 1.0));
		double fSlow28 = (72.6252612538092 * fSlow10);
		double fSlow29 = (fSlow28 + (fConst17 / fSlow2));
		double fSlow30 = (57.089922053120397 * fSlow10);
		double fSlow31 = (fSlow30 + (fConst18 / fSlow2));
		double fSlow32 = (fConst19 / fSlow2);
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
			double fTemp14 = (fRec3[0] * double(input30[i]));
			fRec28[0] = (fRec28[1] + fRec27[1]);
			fRec29[0] = (fRec29[1] + fRec28[1]);
			fRec27[0] = (fSlow27 * ((fSlow3 * fTemp14) - ((fSlow29 * fRec28[0]) + (fSlow28 * fRec29[0]))));
			fRec30[0] = (fRec30[1] + fRec26[1]);
			fRec31[0] = (fRec31[1] + fRec30[1]);
			fRec26[0] = (fSlow26 * (fRec27[0] - ((fSlow31 * fRec30[0]) + (fSlow30 * fRec31[0]))));
			fRec32[0] = (fRec32[1] + fRec25[1]);
			fRec25[0] = (fSlow25 * (fRec26[0] - (fSlow32 * fRec32[0])));
			double fTemp15 = (fRec25[0] + (fSlow4 * fTemp14));
			fRec24[0] = max((fRec24[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph5 = FAUSTFLOAT(fRec24[0]);
			double fTemp16 = (0.042115870353719366 * fTemp15);
			double fTemp17 = (fRec1[0] * (((((fTemp1 + fTemp4) + fTemp7) + fTemp10) + fTemp13) + fTemp16));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph6 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp17);
			double fTemp18 = (fRec3[0] * double(input13[i]));
			fRec37[0] = (fRec37[1] + fRec36[1]);
			fRec38[0] = (fRec38[1] + fRec37[1]);
			fRec36[0] = (fSlow15 * ((fSlow3 * fTemp18) - ((fSlow17 * fRec37[0]) + (fSlow16 * fRec38[0]))));
			fRec39[0] = (fRec39[1] + fRec35[1]);
			fRec35[0] = (fSlow14 * (fRec36[0] - (fSlow18 * fRec39[0])));
			double fTemp19 = (fRec35[0] + (fSlow4 * fTemp18));
			fRec34[0] = max((fRec34[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph7 = FAUSTFLOAT(fRec34[0]);
			double fTemp20 = (fRec3[0] * double(input22[i]));
			fRec43[0] = (fRec43[1] + fRec42[1]);
			fRec44[0] = (fRec44[1] + fRec43[1]);
			fRec42[0] = (fSlow20 * ((fSlow3 * fTemp20) - ((fSlow22 * fRec43[0]) + (fSlow21 * fRec44[0]))));
			fRec45[0] = (fRec45[1] + fRec41[1]);
			fRec46[0] = (fRec46[1] + fRec45[1]);
			fRec41[0] = (fSlow19 * (fRec42[0] - ((fSlow24 * fRec45[0]) + (fSlow23 * fRec46[0]))));
			double fTemp21 = (fRec41[0] + (fSlow4 * fTemp20));
			fRec40[0] = max((fRec40[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph8 = FAUSTFLOAT(fRec40[0]);
			double fTemp22 = (fRec3[0] * double(input33[i]));
			fRec51[0] = (fRec51[1] + fRec50[1]);
			fRec52[0] = (fRec52[1] + fRec51[1]);
			fRec50[0] = (fSlow27 * ((fSlow3 * fTemp22) - ((fSlow29 * fRec51[0]) + (fSlow28 * fRec52[0]))));
			fRec53[0] = (fRec53[1] + fRec49[1]);
			fRec54[0] = (fRec54[1] + fRec53[1]);
			fRec49[0] = (fSlow26 * (fRec50[0] - ((fSlow31 * fRec53[0]) + (fSlow30 * fRec54[0]))));
			fRec55[0] = (fRec55[1] + fRec48[1]);
			fRec48[0] = (fSlow25 * (fRec49[0] - (fSlow32 * fRec55[0])));
			double fTemp23 = (fRec48[0] + (fSlow4 * fTemp22));
			fRec47[0] = max((fRec47[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fVbargraph9 = FAUSTFLOAT(fRec47[0]);
			double fTemp24 = (fRec3[0] * double(input31[i]));
			fRec60[0] = (fRec60[1] + fRec59[1]);
			fRec61[0] = (fRec61[1] + fRec60[1]);
			fRec59[0] = (fSlow27 * ((fSlow3 * fTemp24) - ((fSlow29 * fRec60[0]) + (fSlow28 * fRec61[0]))));
			fRec62[0] = (fRec62[1] + fRec58[1]);
			fRec63[0] = (fRec63[1] + fRec62[1]);
			fRec58[0] = (fSlow26 * (fRec59[0] - ((fSlow31 * fRec62[0]) + (fSlow30 * fRec63[0]))));
			fRec64[0] = (fRec64[1] + fRec57[1]);
			fRec57[0] = (fSlow25 * (fRec58[0] - (fSlow32 * fRec64[0])));
			double fTemp25 = (fRec57[0] + (fSlow4 * fTemp24));
			fRec56[0] = max((fRec56[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph10 = FAUSTFLOAT(fRec56[0]);
			double fTemp26 = (fRec3[0] * double(input15[i]));
			fRec68[0] = (fRec68[1] + fRec67[1]);
			fRec69[0] = (fRec69[1] + fRec68[1]);
			fRec67[0] = (fSlow15 * ((fSlow3 * fTemp26) - ((fSlow17 * fRec68[0]) + (fSlow16 * fRec69[0]))));
			fRec70[0] = (fRec70[1] + fRec66[1]);
			fRec66[0] = (fSlow14 * (fRec67[0] - (fSlow18 * fRec70[0])));
			double fTemp27 = (fRec66[0] + (fSlow4 * fTemp26));
			fRec65[0] = max((fRec65[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph11 = FAUSTFLOAT(fRec65[0]);
			double fTemp28 = (fRec3[0] * double(input8[i]));
			fRec73[0] = (fRec73[1] + fRec72[1]);
			fRec74[0] = (fRec74[1] + fRec73[1]);
			fRec72[0] = (fSlow11 * ((fSlow3 * fTemp28) - ((fSlow13 * fRec73[0]) + (fSlow12 * fRec74[0]))));
			double fTemp29 = (fRec72[0] + (fSlow4 * fTemp28));
			fRec71[0] = max((fRec71[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph12 = FAUSTFLOAT(fRec71[0]);
			double fTemp30 = (fConst43 * fTemp29);
			double fTemp31 = (fRec3[0] * double(input3[i]));
			fRec77[0] = (fRec77[1] + fRec76[1]);
			fRec76[0] = (fSlow8 * ((fSlow3 * fTemp31) - (fSlow9 * fRec77[0])));
			double fTemp32 = (fRec76[0] + (fSlow4 * fTemp31));
			fRec75[0] = max((fRec75[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph13 = FAUSTFLOAT(fRec75[0]);
			double fTemp33 = (0.014285714285714285 * fTemp12);
			double fTemp34 = (fRec3[0] * double(input24[i]));
			fRec81[0] = (fRec81[1] + fRec80[1]);
			fRec82[0] = (fRec82[1] + fRec81[1]);
			fRec80[0] = (fSlow20 * ((fSlow3 * fTemp34) - ((fSlow22 * fRec81[0]) + (fSlow21 * fRec82[0]))));
			fRec83[0] = (fRec83[1] + fRec79[1]);
			fRec84[0] = (fRec84[1] + fRec83[1]);
			fRec79[0] = (fSlow19 * (fRec80[0] - ((fSlow24 * fRec83[0]) + (fSlow23 * fRec84[0]))));
			double fTemp35 = (fRec79[0] + (fSlow4 * fTemp34));
			fRec78[0] = max((fRec78[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph14 = FAUSTFLOAT(fRec78[0]);
			double fTemp36 = (fConst50 * fTemp35);
			double fTemp37 = (fRec3[0] * double(input35[i]));
			fRec89[0] = (fRec89[1] + fRec88[1]);
			fRec90[0] = (fRec90[1] + fRec89[1]);
			fRec88[0] = (fSlow27 * ((fSlow3 * fTemp37) - ((fSlow29 * fRec89[0]) + (fSlow28 * fRec90[0]))));
			fRec91[0] = (fRec91[1] + fRec87[1]);
			fRec92[0] = (fRec92[1] + fRec91[1]);
			fRec87[0] = (fSlow26 * (fRec88[0] - ((fSlow31 * fRec91[0]) + (fSlow30 * fRec92[0]))));
			fRec93[0] = (fRec93[1] + fRec86[1]);
			fRec86[0] = (fSlow25 * (fRec87[0] - (fSlow32 * fRec93[0])));
			double fTemp38 = (fRec86[0] + (fSlow4 * fTemp37));
			fRec85[0] = max((fRec85[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fVbargraph15 = FAUSTFLOAT(fRec85[0]);
			double fTemp39 = (0.014197256999998666 * fTemp6);
			double fTemp40 = (fRec1[0] * ((((0.0089791337293529845 * (((fConst25 * fTemp19) + (fConst29 * fTemp21)) + (fConst33 * fTemp23))) + ((fConst36 * fTemp25) + (((fConst40 * fTemp27) + (fTemp30 + ((fConst46 * fTemp32) + (fTemp1 + fTemp33)))) + fTemp36))) + (fConst52 * fTemp38)) - fTemp39));
			fRec33[0] = max((fRec33[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fVbargraph16 = FAUSTFLOAT(fRec33[0]);
			output1[i] = FAUSTFLOAT(fTemp40);
			double fTemp41 = (fConst53 * fTemp29);
			double fTemp42 = (fRec3[0] * double(input9[i]));
			fRec98[0] = (fRec98[1] + fRec97[1]);
			fRec99[0] = (fRec99[1] + fRec98[1]);
			fRec97[0] = (fSlow15 * ((fSlow3 * fTemp42) - ((fSlow17 * fRec98[0]) + (fSlow16 * fRec99[0]))));
			fRec100[0] = (fRec100[1] + fRec96[1]);
			fRec96[0] = (fSlow14 * (fRec97[0] - (fSlow18 * fRec100[0])));
			double fTemp43 = (fRec96[0] + (fSlow4 * fTemp42));
			fRec95[0] = max((fRec95[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph17 = FAUSTFLOAT(fRec95[0]);
			double fTemp44 = (fConst55 * fTemp21);
			double fTemp45 = (fRec3[0] * double(input27[i]));
			fRec105[0] = (fRec105[1] + fRec104[1]);
			fRec106[0] = (fRec106[1] + fRec105[1]);
			fRec104[0] = (fSlow27 * ((fSlow3 * fTemp45) - ((fSlow29 * fRec105[0]) + (fSlow28 * fRec106[0]))));
			fRec107[0] = (fRec107[1] + fRec103[1]);
			fRec108[0] = (fRec108[1] + fRec107[1]);
			fRec103[0] = (fSlow26 * (fRec104[0] - ((fSlow31 * fRec107[0]) + (fSlow30 * fRec108[0]))));
			fRec109[0] = (fRec109[1] + fRec102[1]);
			fRec102[0] = (fSlow25 * (fRec103[0] - (fSlow32 * fRec109[0])));
			double fTemp46 = (fRec102[0] + (fSlow4 * fTemp45));
			fRec101[0] = max((fRec101[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph18 = FAUSTFLOAT(fRec101[0]);
			double fTemp47 = (fRec3[0] * double(input11[i]));
			fRec113[0] = (fRec113[1] + fRec112[1]);
			fRec114[0] = (fRec114[1] + fRec113[1]);
			fRec112[0] = (fSlow15 * ((fSlow3 * fTemp47) - ((fSlow17 * fRec113[0]) + (fSlow16 * fRec114[0]))));
			fRec115[0] = (fRec115[1] + fRec111[1]);
			fRec111[0] = (fSlow14 * (fRec112[0] - (fSlow18 * fRec115[0])));
			double fTemp48 = (fRec111[0] + (fSlow4 * fTemp47));
			fRec110[0] = max((fRec110[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph19 = FAUSTFLOAT(fRec110[0]);
			double fTemp49 = (fRec3[0] * double(input1[i]));
			fRec118[0] = (fRec118[1] + fRec117[1]);
			fRec117[0] = (fSlow8 * ((fSlow3 * fTemp49) - (fSlow9 * fRec118[0])));
			double fTemp50 = (fRec117[0] + (fSlow4 * fTemp49));
			fRec116[0] = max((fRec116[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph20 = FAUSTFLOAT(fRec116[0]);
			double fTemp51 = (fTemp33 + fTemp36);
			double fTemp52 = (fRec3[0] * double(input25[i]));
			fRec123[0] = (fRec123[1] + fRec122[1]);
			fRec124[0] = (fRec124[1] + fRec123[1]);
			fRec122[0] = (fSlow27 * ((fSlow3 * fTemp52) - ((fSlow29 * fRec123[0]) + (fSlow28 * fRec124[0]))));
			fRec125[0] = (fRec125[1] + fRec121[1]);
			fRec126[0] = (fRec126[1] + fRec125[1]);
			fRec121[0] = (fSlow26 * (fRec122[0] - ((fSlow31 * fRec125[0]) + (fSlow30 * fRec126[0]))));
			fRec127[0] = (fRec127[1] + fRec120[1]);
			fRec120[0] = (fSlow25 * (fRec121[0] - (fSlow32 * fRec127[0])));
			double fTemp53 = (fRec120[0] + (fSlow4 * fTemp52));
			fRec119[0] = max((fRec119[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fVbargraph21 = FAUSTFLOAT(fRec119[0]);
			double fTemp54 = (fRec3[0] * double(input29[i]));
			fRec132[0] = (fRec132[1] + fRec131[1]);
			fRec133[0] = (fRec133[1] + fRec132[1]);
			fRec131[0] = (fSlow27 * ((fSlow3 * fTemp54) - ((fSlow29 * fRec132[0]) + (fSlow28 * fRec133[0]))));
			fRec134[0] = (fRec134[1] + fRec130[1]);
			fRec135[0] = (fRec135[1] + fRec134[1]);
			fRec130[0] = (fSlow26 * (fRec131[0] - ((fSlow31 * fRec134[0]) + (fSlow30 * fRec135[0]))));
			fRec136[0] = (fRec136[1] + fRec129[1]);
			fRec129[0] = (fSlow25 * (fRec130[0] - (fSlow32 * fRec136[0])));
			double fTemp55 = (fRec129[0] + (fSlow4 * fTemp54));
			fRec128[0] = max((fRec128[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph22 = FAUSTFLOAT(fRec128[0]);
			double fTemp56 = (fRec1[0] * ((((0.012698412698412698 * ((((fTemp0 + fTemp41) + (fConst54 * fTemp43)) + fTemp44) + (fConst56 * fTemp46))) + (((fConst57 * fTemp48) + ((fConst46 * fTemp50) + fTemp51)) + (fConst52 * fTemp53))) + (fConst36 * fTemp55)) - fTemp39));
			fRec94[0] = max((fRec94[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fVbargraph23 = FAUSTFLOAT(fRec94[0]);
			output2[i] = FAUSTFLOAT(fTemp56);
			double fTemp57 = (fRec1[0] * (((fTemp36 + ((fConst58 * fTemp21) + (fTemp30 + fTemp33))) + (0.012698412698412698 * ((((((fTemp0 + (fConst59 * fTemp32)) + (fConst60 * fTemp19)) + (fConst54 * fTemp27)) + (fConst61 * fTemp25)) + (fConst56 * fTemp23)) + (fConst62 * fTemp38)))) - fTemp39));
			fRec137[0] = max((fRec137[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fVbargraph24 = FAUSTFLOAT(fRec137[0]);
			output3[i] = FAUSTFLOAT(fTemp57);
			double fTemp58 = (fRec1[0] * (((0.012698412698412698 * (((fTemp44 + ((fTemp41 + (fTemp0 + (fConst59 * fTemp50))) + (fConst60 * fTemp48))) + (fConst62 * fTemp53)) + (fConst61 * fTemp55))) + ((fTemp51 + (fConst40 * fTemp43)) + (fConst63 * fTemp46))) - fTemp39));
			fRec138[0] = max((fRec138[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fVbargraph25 = FAUSTFLOAT(fRec138[0]);
			output4[i] = FAUSTFLOAT(fTemp58);
			double fTemp59 = (fRec1[0] * ((fTemp13 + (fTemp1 + fTemp7)) - (fTemp16 + (fTemp4 + fTemp10))));
			fRec139[0] = max((fRec139[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fVbargraph26 = FAUSTFLOAT(fRec139[0]);
			output5[i] = FAUSTFLOAT(fTemp59);
			double fTemp60 = (fRec3[0] * double(input34[i]));
			fRec145[0] = (fRec145[1] + fRec144[1]);
			fRec146[0] = (fRec146[1] + fRec145[1]);
			fRec144[0] = (fSlow27 * ((fSlow3 * fTemp60) - ((fSlow29 * fRec145[0]) + (fSlow28 * fRec146[0]))));
			fRec147[0] = (fRec147[1] + fRec143[1]);
			fRec148[0] = (fRec148[1] + fRec147[1]);
			fRec143[0] = (fSlow26 * (fRec144[0] - ((fSlow31 * fRec147[0]) + (fSlow30 * fRec148[0]))));
			fRec149[0] = (fRec149[1] + fRec142[1]);
			fRec142[0] = (fSlow25 * (fRec143[0] - (fSlow32 * fRec149[0])));
			double fTemp61 = (fRec142[0] + (fSlow4 * fTemp60));
			fRec141[0] = max((fRec141[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fVbargraph27 = FAUSTFLOAT(fRec141[0]);
			double fTemp62 = (fConst47 * ((fConst64 * fTemp35) + (fConst67 * fTemp61)));
			double fTemp63 = (fRec3[0] * double(input23[i]));
			fRec153[0] = (fRec153[1] + fRec152[1]);
			fRec154[0] = (fRec154[1] + fRec153[1]);
			fRec152[0] = (fSlow20 * ((fSlow3 * fTemp63) - ((fSlow22 * fRec153[0]) + (fSlow21 * fRec154[0]))));
			fRec155[0] = (fRec155[1] + fRec151[1]);
			fRec156[0] = (fRec156[1] + fRec155[1]);
			fRec151[0] = (fSlow19 * (fRec152[0] - ((fSlow24 * fRec155[0]) + (fSlow23 * fRec156[0]))));
			double fTemp64 = (fRec151[0] + (fSlow4 * fTemp63));
			fRec150[0] = max((fRec150[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fVbargraph28 = FAUSTFLOAT(fRec150[0]);
			double fTemp65 = (fRec3[0] * double(input14[i]));
			fRec160[0] = (fRec160[1] + fRec159[1]);
			fRec161[0] = (fRec161[1] + fRec160[1]);
			fRec159[0] = (fSlow15 * ((fSlow3 * fTemp65) - ((fSlow17 * fRec160[0]) + (fSlow16 * fRec161[0]))));
			fRec162[0] = (fRec162[1] + fRec158[1]);
			fRec158[0] = (fSlow14 * (fRec159[0] - (fSlow18 * fRec162[0])));
			double fTemp66 = (fRec158[0] + (fSlow4 * fTemp65));
			fRec157[0] = max((fRec157[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fVbargraph29 = FAUSTFLOAT(fRec157[0]);
			double fTemp67 = (fRec3[0] * double(input32[i]));
			fRec167[0] = (fRec167[1] + fRec166[1]);
			fRec168[0] = (fRec168[1] + fRec167[1]);
			fRec166[0] = (fSlow27 * ((fSlow3 * fTemp67) - ((fSlow29 * fRec167[0]) + (fSlow28 * fRec168[0]))));
			fRec169[0] = (fRec169[1] + fRec165[1]);
			fRec170[0] = (fRec170[1] + fRec169[1]);
			fRec165[0] = (fSlow26 * (fRec166[0] - ((fSlow31 * fRec169[0]) + (fSlow30 * fRec170[0]))));
			fRec171[0] = (fRec171[1] + fRec164[1]);
			fRec164[0] = (fSlow25 * (fRec165[0] - (fSlow32 * fRec171[0])));
			double fTemp68 = (fRec164[0] + (fSlow4 * fTemp67));
			fRec163[0] = max((fRec163[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fVbargraph30 = FAUSTFLOAT(fRec163[0]);
			double fTemp69 = (fConst26 * ((((fConst73 * fTemp29) + (fConst76 * fTemp66)) + (fConst77 * fTemp21)) + (fConst80 * fTemp68)));
			double fTemp70 = (fRec3[0] * double(input7[i]));
			fRec174[0] = (fRec174[1] + fRec173[1]);
			fRec175[0] = (fRec175[1] + fRec174[1]);
			fRec173[0] = (fSlow11 * ((fSlow3 * fTemp70) - ((fSlow13 * fRec174[0]) + (fSlow12 * fRec175[0]))));
			double fTemp71 = (fRec173[0] + (fSlow4 * fTemp70));
			fRec172[0] = max((fRec172[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fVbargraph31 = FAUSTFLOAT(fRec172[0]);
			double fTemp72 = (fRec3[0] * double(input21[i]));
			fRec179[0] = (fRec179[1] + fRec178[1]);
			fRec180[0] = (fRec180[1] + fRec179[1]);
			fRec178[0] = (fSlow20 * ((fSlow3 * fTemp72) - ((fSlow22 * fRec179[0]) + (fSlow21 * fRec180[0]))));
			fRec181[0] = (fRec181[1] + fRec177[1]);
			fRec182[0] = (fRec182[1] + fRec181[1]);
			fRec177[0] = (fSlow19 * (fRec178[0] - ((fSlow24 * fRec181[0]) + (fSlow23 * fRec182[0]))));
			double fTemp73 = (fRec177[0] + (fSlow4 * fTemp72));
			fRec176[0] = max((fRec176[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp73))))));
			fVbargraph32 = FAUSTFLOAT(fRec176[0]);
			double fTemp74 = (0.022574955908289243 * fTemp0);
			double fTemp75 = (0.027648561470568499 * fTemp3);
			double fTemp76 = (0.01261978399999881 * fTemp6);
			double fTemp77 = (fConst89 * fTemp38);
			double fTemp78 = (0.010558468816293317 * fTemp9);
			double fTemp79 = (0.027513227513227514 * fTemp12);
			double fTemp80 = (0.028125949882539081 * fTemp15);
			double fTemp81 = ((fTemp78 + fTemp79) + fTemp80);
			double fTemp82 = (fRec1[0] * ((fTemp62 + ((fConst30 * (((fConst68 * fTemp27) + (fConst71 * fTemp64)) + (fConst72 * fTemp23))) + (fTemp69 + ((fConst82 * fTemp25) + ((fConst20 * ((((fConst83 * fTemp32) + (fConst85 * fTemp71)) + (fConst86 * fTemp19)) + (fConst88 * fTemp73))) + (((fTemp74 + fTemp75) + fTemp76) + fTemp77)))))) - fTemp81));
			fRec140[0] = max((fRec140[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp82))))));
			fVbargraph33 = FAUSTFLOAT(fRec140[0]);
			output6[i] = FAUSTFLOAT(fTemp82);
			double fTemp83 = (fConst90 * fTemp29);
			double fTemp84 = ((fTemp0 + fTemp83) + (fConst91 * fTemp43));
			double fTemp85 = (fConst93 * fTemp66);
			double fTemp86 = (fRec3[0] * double(input17[i]));
			fRec187[0] = (fRec187[1] + fRec186[1]);
			fRec188[0] = (fRec188[1] + fRec187[1]);
			fRec186[0] = (fSlow20 * ((fSlow3 * fTemp86) - ((fSlow22 * fRec187[0]) + (fSlow21 * fRec188[0]))));
			fRec189[0] = (fRec189[1] + fRec185[1]);
			fRec190[0] = (fRec190[1] + fRec189[1]);
			fRec185[0] = (fSlow19 * (fRec186[0] - ((fSlow24 * fRec189[0]) + (fSlow23 * fRec190[0]))));
			double fTemp87 = (fRec185[0] + (fSlow4 * fTemp86));
			fRec184[0] = max((fRec184[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp87))))));
			fVbargraph34 = FAUSTFLOAT(fRec184[0]);
			double fTemp88 = (fConst97 * fTemp21);
			double fTemp89 = (fConst101 * fTemp68);
			double fTemp90 = (fRec3[0] * double(input5[i]));
			fRec193[0] = (fRec193[1] + fRec192[1]);
			fRec194[0] = (fRec194[1] + fRec193[1]);
			fRec192[0] = (fSlow11 * ((fSlow3 * fTemp90) - ((fSlow13 * fRec193[0]) + (fSlow12 * fRec194[0]))));
			double fTemp91 = (fRec192[0] + (fSlow4 * fTemp90));
			fRec191[0] = max((fRec191[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp91))))));
			fVbargraph35 = FAUSTFLOAT(fRec191[0]);
			double fTemp92 = (fRec3[0] * double(input19[i]));
			fRec198[0] = (fRec198[1] + fRec197[1]);
			fRec199[0] = (fRec199[1] + fRec198[1]);
			fRec197[0] = (fSlow20 * ((fSlow3 * fTemp92) - ((fSlow22 * fRec198[0]) + (fSlow21 * fRec199[0]))));
			fRec200[0] = (fRec200[1] + fRec196[1]);
			fRec201[0] = (fRec201[1] + fRec200[1]);
			fRec196[0] = (fSlow19 * (fRec197[0] - ((fSlow24 * fRec200[0]) + (fSlow23 * fRec201[0]))));
			double fTemp93 = (fRec196[0] + (fSlow4 * fTemp92));
			fRec195[0] = max((fRec195[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fVbargraph36 = FAUSTFLOAT(fRec195[0]);
			double fTemp94 = (fTemp75 + fTemp76);
			double fTemp95 = (fConst89 * fTemp53);
			double fTemp96 = (fRec1[0] * (((0.022574955908289243 * (((((fTemp84 + fTemp85) + (fConst95 * fTemp87)) + fTemp88) + (fConst99 * fTemp46)) + fTemp89)) + ((fConst82 * fTemp55) + ((fConst20 * ((((fConst83 * fTemp50) + (fConst85 * fTemp91)) + (fConst86 * fTemp48)) + (fConst88 * fTemp93))) + (fTemp62 + (fTemp94 + fTemp95))))) - fTemp81));
			fRec183[0] = max((fRec183[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp96))))));
			fVbargraph37 = FAUSTFLOAT(fRec183[0]);
			output7[i] = FAUSTFLOAT(fTemp96);
			double fTemp97 = (fTemp0 + (fConst102 * fTemp32));
			double fTemp98 = (fConst91 * fTemp27);
			double fTemp99 = (fConst110 * fTemp38);
			double fTemp100 = (fRec1[0] * (((fTemp62 + (fTemp69 + fTemp94)) + (0.022574955908289243 * ((((((((fTemp97 + (fConst104 * fTemp71)) + (fConst106 * fTemp19)) + fTemp98) + (fConst108 * fTemp73)) + (fConst95 * fTemp64)) + (fConst109 * fTemp25)) + (fConst99 * fTemp23)) + fTemp99))) - fTemp81));
			fRec202[0] = max((fRec202[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp100))))));
			fVbargraph38 = FAUSTFLOAT(fRec202[0]);
			output8[i] = FAUSTFLOAT(fTemp100);
			double fTemp101 = (fTemp0 + (fConst102 * fTemp50));
			double fTemp102 = (fConst110 * fTemp53);
			double fTemp103 = (fRec1[0] * (((0.022574955908289243 * (fTemp89 + (((fTemp88 + ((fTemp85 + ((fTemp83 + (fTemp101 + (fConst104 * fTemp91))) + (fConst106 * fTemp48))) + (fConst108 * fTemp93))) + fTemp102) + (fConst109 * fTemp55)))) + ((fConst30 * (((fConst68 * fTemp43) + (fConst71 * fTemp87)) + (fConst72 * fTemp46))) + (fTemp62 + fTemp94))) - fTemp81));
			fRec203[0] = max((fRec203[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp103))))));
			fVbargraph39 = FAUSTFLOAT(fRec203[0]);
			output9[i] = FAUSTFLOAT(fTemp103);
			double fTemp104 = (fRec3[0] * double(input18[i]));
			fRec208[0] = (fRec208[1] + fRec207[1]);
			fRec209[0] = (fRec209[1] + fRec208[1]);
			fRec207[0] = (fSlow20 * ((fSlow3 * fTemp104) - ((fSlow22 * fRec208[0]) + (fSlow21 * fRec209[0]))));
			fRec210[0] = (fRec210[1] + fRec206[1]);
			fRec211[0] = (fRec211[1] + fRec210[1]);
			fRec206[0] = (fSlow19 * (fRec207[0] - ((fSlow24 * fRec210[0]) + (fSlow23 * fRec211[0]))));
			double fTemp105 = (fRec206[0] + (fSlow4 * fTemp104));
			fRec205[0] = max((fRec205[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp105))))));
			fVbargraph40 = FAUSTFLOAT(fRec205[0]);
			double fTemp106 = (fRec3[0] * double(input16[i]));
			fRec215[0] = (fRec215[1] + fRec214[1]);
			fRec216[0] = (fRec216[1] + fRec215[1]);
			fRec214[0] = (fSlow20 * ((fSlow3 * fTemp106) - ((fSlow22 * fRec215[0]) + (fSlow21 * fRec216[0]))));
			fRec217[0] = (fRec217[1] + fRec213[1]);
			fRec218[0] = (fRec218[1] + fRec217[1]);
			fRec213[0] = (fSlow19 * (fRec214[0] - ((fSlow24 * fRec217[0]) + (fSlow23 * fRec218[0]))));
			double fTemp107 = (fRec213[0] + (fSlow4 * fTemp106));
			fRec212[0] = max((fRec212[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp107))))));
			fVbargraph41 = FAUSTFLOAT(fRec212[0]);
			double fTemp108 = (fRec3[0] * double(input4[i]));
			fRec221[0] = (fRec221[1] + fRec220[1]);
			fRec222[0] = (fRec222[1] + fRec221[1]);
			fRec220[0] = (fSlow11 * ((fSlow3 * fTemp108) - ((fSlow13 * fRec221[0]) + (fSlow12 * fRec222[0]))));
			double fTemp109 = (fRec220[0] + (fSlow4 * fTemp108));
			fRec219[0] = max((fRec219[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp109))))));
			fVbargraph42 = FAUSTFLOAT(fRec219[0]);
			double fTemp110 = (0.025396825396825397 * fTemp12);
			double fTemp111 = (0.025239567999997631 * fTemp6);
			double fTemp112 = (fRec1[0] * (((((fConst29 * ((3.5444768026723675e-18 * fTemp21) + (0.015962904407738641 * fTemp105))) + ((fConst111 * fTemp107) + ((fConst25 * ((0.011287477954144623 * fTemp19) + (0.011287477954144622 * fTemp48))) + ((fConst42 * ((1.5998055129555246e-17 * fTemp29) + (0.072048835120117657 * fTemp109))) + ((fConst45 * ((0.025473109945013343 * fTemp32) + (0.025473109945013336 * fTemp50))) + (((fConst112 * fTemp43) + fTemp110) + (fConst113 * fTemp46))))))) + (fConst35 * ((0.0056437389770723117 * fTemp25) + (0.0056437389770723108 * fTemp55)))) + (0.022574955908289243 * (((((fTemp0 + (fConst114 * fTemp27)) + (fConst115 * fTemp35)) + (fConst116 * fTemp53)) + (fConst117 * fTemp23)) + (fConst118 * fTemp38)))) - fTemp111));
			fRec204[0] = max((fRec204[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp112))))));
			fVbargraph43 = FAUSTFLOAT(fRec204[0]);
			output10[i] = FAUSTFLOAT(fTemp112);
			double fTemp113 = (fConst53 * fTemp109);
			double fTemp114 = (fConst55 * fTemp105);
			double fTemp115 = (fConst123 * fTemp35);
			double fTemp116 = (fConst129 * fTemp50);
			double fTemp117 = (fRec1[0] * (((fConst33 * ((0.011287477954144627 * fTemp23) + (0.011287477954144618 * fTemp46))) + ((0.022574955908289243 * (((((((((fTemp0 + (fConst119 * fTemp32)) + fTemp113) + (fConst120 * fTemp29)) + (fConst121 * fTemp19)) + fTemp114) + (fConst122 * fTemp21)) + fTemp115) + (fConst124 * fTemp53)) + (fConst125 * fTemp25))) + ((fConst39 * ((0.10189243978005338 * fTemp27) + (0.1018924397800533 * fTemp43))) + ((fConst126 * fTemp107) + ((fConst127 * fTemp55) + ((fConst128 * fTemp48) + (fTemp116 + (fTemp110 + (fConst130 * fTemp38))))))))) - fTemp111));
			fRec223[0] = max((fRec223[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp117))))));
			fVbargraph44 = FAUSTFLOAT(fRec223[0]);
			output11[i] = FAUSTFLOAT(fTemp117);
			double fTemp118 = (fRec1[0] * (((fConst51 * ((0.40756975912021387 * fTemp38) + (0.40756975912021298 * fTemp53))) + ((0.022574955908289243 * ((((fTemp115 + (((((fTemp0 + (fConst119 * fTemp50)) + (fConst131 * fTemp32)) + (fConst132 * fTemp43)) + (fConst121 * fTemp48)) + (fConst133 * fTemp19))) + (fConst134 * fTemp46)) + (fConst125 * fTemp55)) + (fConst135 * fTemp25))) + ((fConst29 * ((7.088953605344735e-18 * fTemp21) + (0.015962904407738645 * fTemp105))) + ((fConst42 * ((3.1996110259110492e-17 * fTemp29) + (0.072048835120117671 * fTemp109))) + ((fConst136 * fTemp107) + ((fTemp110 + (fConst137 * fTemp27)) + (fConst138 * fTemp23))))))) - fTemp111));
			fRec224[0] = max((fRec224[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp118))))));
			fVbargraph45 = FAUSTFLOAT(fRec224[0]);
			output12[i] = FAUSTFLOAT(fTemp118);
			double fTemp119 = (fRec1[0] * ((((fConst139 * fTemp107) + ((fConst140 * fTemp53) + (((fConst141 * fTemp19) + (fTemp110 + (fConst142 * fTemp32))) + (fConst143 * fTemp25)))) + (0.022574955908289243 * (((((fTemp115 + ((fTemp114 + (((((fTemp113 + (fTemp0 + (fConst131 * fTemp50))) + (fConst144 * fTemp29)) + (fConst145 * fTemp43)) + (fConst133 * fTemp48)) + (fConst146 * fTemp27))) + (fConst147 * fTemp21))) + (fConst148 * fTemp46)) + (fConst135 * fTemp55)) + (fConst149 * fTemp23)) + (fConst150 * fTemp38)))) - fTemp111));
			fRec225[0] = max((fRec225[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp119))))));
			fVbargraph46 = FAUSTFLOAT(fRec225[0]);
			output13[i] = FAUSTFLOAT(fTemp119);
			double fTemp120 = (fConst154 * fTemp66);
			double fTemp121 = (fConst157 * fTemp61);
			double fTemp122 = (fConst168 * fTemp35);
			double fTemp123 = (fConst170 * fTemp29);
			double fTemp124 = (fTemp75 + fTemp79);
			double fTemp125 = (fRec1[0] * (((0.031925808815477283 * ((((fConst152 * fTemp71) + fTemp120) + (fConst156 * fTemp64)) + fTemp121)) + ((0.015962904407738641 * (((fConst159 * fTemp19) + (fConst161 * fTemp21)) + (fConst162 * fTemp23))) + ((0.010641936271825759 * ((fConst164 * fTemp73) + (fConst165 * fTemp68))) + ((fConst167 * fTemp25) + (fTemp77 + (fTemp122 + ((fConst169 * fTemp27) + (fTemp123 + ((fConst129 * fTemp32) + (fTemp80 + (fTemp78 + (fTemp74 + fTemp76)))))))))))) - fTemp124));
			fRec226[0] = max((fRec226[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp125))))));
			fVbargraph47 = FAUSTFLOAT(fRec226[0]);
			output14[i] = FAUSTFLOAT(fTemp125);
			double fTemp126 = (fConst171 * fTemp66);
			double fTemp127 = (fConst173 * fTemp21);
			double fTemp128 = (fConst175 * fTemp68);
			double fTemp129 = (fTemp80 + (fTemp76 + fTemp78));
			double fTemp130 = (fTemp122 + fTemp129);
			double fTemp131 = (fRec1[0] * (((0.031925808815477283 * (fTemp121 + (fConst152 * fTemp91))) + ((0.022574955908289243 * (((((fTemp84 + fTemp126) + (fConst172 * fTemp87)) + fTemp127) + (fConst174 * fTemp46)) + fTemp128)) + ((fConst167 * fTemp55) + ((fConst176 * fTemp93) + ((fConst177 * fTemp48) + (fTemp95 + (fTemp116 + fTemp130))))))) - fTemp124));
			fRec227[0] = max((fRec227[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp131))))));
			fVbargraph48 = FAUSTFLOAT(fRec227[0]);
			output15[i] = FAUSTFLOAT(fTemp131);
			double fTemp132 = (fRec1[0] * (((0.022574955908289243 * (fTemp99 + (((((fTemp98 + ((fTemp97 + (fConst178 * fTemp71)) + (fConst179 * fTemp19))) + (fConst180 * fTemp73)) + (fConst172 * fTemp64)) + (fConst181 * fTemp25)) + (fConst174 * fTemp23)))) + ((0.031925808815477283 * (fTemp120 + fTemp121)) + ((fConst182 * fTemp68) + ((fConst183 * fTemp21) + (fTemp122 + (fTemp123 + fTemp129)))))) - fTemp124));
			fRec228[0] = max((fRec228[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp132))))));
			fVbargraph49 = FAUSTFLOAT(fRec228[0]);
			output16[i] = FAUSTFLOAT(fTemp132);
			double fTemp133 = (fRec1[0] * (((0.031925808815477283 * (fTemp121 + (fConst156 * fTemp87))) + ((0.022574955908289243 * (fTemp128 + ((fTemp102 + (fTemp127 + ((fTemp126 + ((fTemp83 + (fTemp101 + (fConst178 * fTemp91))) + (fConst179 * fTemp48))) + (fConst180 * fTemp93)))) + (fConst181 * fTemp55)))) + ((fConst184 * fTemp46) + ((fConst169 * fTemp43) + fTemp130)))) - fTemp124));
			fRec229[0] = max((fRec229[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp133))))));
			fVbargraph50 = FAUSTFLOAT(fRec229[0]);
			output17[i] = FAUSTFLOAT(fTemp133);
			double fTemp134 = ((9.9655853669580557e-18 * fTemp29) + (0.044881006545161167 * fTemp109));
			double fTemp135 = (fRec3[0] * double(input10[i]));
			fRec234[0] = (fRec234[1] + fRec233[1]);
			fRec235[0] = (fRec235[1] + fRec234[1]);
			fRec233[0] = (fSlow15 * ((fSlow3 * fTemp135) - ((fSlow17 * fRec234[0]) + (fSlow16 * fRec235[0]))));
			fRec236[0] = (fRec236[1] + fRec232[1]);
			fRec232[0] = (fSlow14 * (fRec233[0] - (fSlow18 * fRec236[0])));
			double fTemp136 = (fRec232[0] + (fSlow4 * fTemp135));
			fRec231[0] = max((fRec231[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp136))))));
			fVbargraph51 = FAUSTFLOAT(fRec231[0]);
			double fTemp137 = (fRec3[0] * double(input28[i]));
			fRec241[0] = (fRec241[1] + fRec240[1]);
			fRec242[0] = (fRec242[1] + fRec241[1]);
			fRec240[0] = (fSlow27 * ((fSlow3 * fTemp137) - ((fSlow29 * fRec241[0]) + (fSlow28 * fRec242[0]))));
			fRec243[0] = (fRec243[1] + fRec239[1]);
			fRec244[0] = (fRec244[1] + fRec243[1]);
			fRec239[0] = (fSlow26 * (fRec240[0] - ((fSlow31 * fRec243[0]) + (fSlow30 * fRec244[0]))));
			fRec245[0] = (fRec245[1] + fRec238[1]);
			fRec238[0] = (fSlow25 * (fRec239[0] - (fSlow32 * fRec245[0])));
			double fTemp138 = (fRec238[0] + (fSlow4 * fTemp137));
			fRec237[0] = max((fRec237[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp138))))));
			fVbargraph52 = FAUSTFLOAT(fRec237[0]);
			double fTemp139 = ((0.0052734375000000012 * fTemp25) + (0.0052734375000000003 * fTemp55));
			double fTemp140 = (fRec3[0] * double(input26[i]));
			fRec250[0] = (fRec250[1] + fRec249[1]);
			fRec251[0] = (fRec251[1] + fRec250[1]);
			fRec249[0] = (fSlow27 * ((fSlow3 * fTemp140) - ((fSlow29 * fRec250[0]) + (fSlow28 * fRec251[0]))));
			fRec252[0] = (fRec252[1] + fRec248[1]);
			fRec253[0] = (fRec253[1] + fRec252[1]);
			fRec248[0] = (fSlow26 * (fRec249[0] - ((fSlow31 * fRec252[0]) + (fSlow30 * fRec253[0]))));
			fRec254[0] = (fRec254[1] + fRec247[1]);
			fRec247[0] = (fSlow25 * (fRec248[0] - (fSlow32 * fRec254[0])));
			double fTemp141 = (fRec247[0] + (fSlow4 * fTemp140));
			fRec246[0] = max((fRec246[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp141))))));
			fVbargraph53 = FAUSTFLOAT(fRec246[0]);
			double fTemp142 = ((0.007031250000000001 * fTemp73) + (0.0070312500000000002 * fTemp93));
			double fTemp143 = (5.236595479505967e-18 * fTemp6);
			double fTemp144 = ((0.019434045907762623 * fTemp32) + (0.019434045907762619 * fTemp50));
			double fTemp145 = (fConst199 * fTemp91);
			double fTemp146 = (fTemp0 + fTemp3);
			double fTemp147 = (fConst202 * fTemp27);
			double fTemp148 = (fConst204 * fTemp35);
			double fTemp149 = (fConst205 * fTemp53);
			double fTemp150 = (fConst209 * fTemp38);
			double fTemp151 = (0.021480823570105497 * fTemp9);
			double fTemp152 = (0.024609375000000006 * fTemp12);
			double fTemp153 = (0.006731909351411762 * fTemp15);
			double fTemp154 = ((fTemp151 + fTemp152) + fTemp153);
			double fTemp155 = (fRec1[0] * ((((fConst26 * (((((((fConst41 * fTemp134) + (fConst185 * fTemp136)) + (fConst186 * fTemp66)) + (fConst187 * fTemp105)) + (fConst188 * fTemp21)) + (fConst189 * fTemp138)) + (fConst190 * fTemp68))) + ((fConst192 * fTemp139) + ((fConst30 * (((fConst193 * fTemp43) + (fConst194 * fTemp87)) + (fConst195 * fTemp46))) + ((fConst47 * ((fConst196 * fTemp107) + (fConst197 * fTemp141))) + ((fConst198 * fTemp142) + (fTemp143 + (fConst20 * (((((fConst44 * fTemp144) + fTemp145) + (fConst199 * fTemp71)) + (fConst200 * fTemp48)) + (fConst201 * fTemp19))))))))) + (0.021093750000000001 * (((((((fTemp146 + fTemp147) + (fConst203 * fTemp64)) + fTemp148) + fTemp149) + (fConst206 * fTemp23)) + (fConst208 * fTemp61)) + fTemp150))) - fTemp154));
			fRec230[0] = max((fRec230[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp155))))));
			fVbargraph54 = FAUSTFLOAT(fRec230[0]);
			output18[i] = FAUSTFLOAT(fTemp155);
			double fTemp156 = (fConst210 * fTemp32);
			double fTemp157 = (fConst211 * fTemp109);
			double fTemp158 = (fConst213 * fTemp29);
			double fTemp159 = (fConst214 * fTemp136);
			double fTemp160 = (fConst217 * fTemp105);
			double fTemp161 = (fConst220 * fTemp35);
			double fTemp162 = (fConst221 * fTemp53);
			double fTemp163 = (fConst222 * fTemp138);
			double fTemp164 = (fConst225 * fTemp61);
			double fTemp165 = ((0.051824122420700336 * fTemp27) + (0.051824122420700287 * fTemp43));
			double fTemp166 = (fConst233 * fTemp38);
			double fTemp167 = (fRec1[0] * (((0.021093750000000001 * ((((((((((((((((fTemp146 + fTemp156) + fTemp157) + (fConst212 * fTemp71)) + fTemp158) + fTemp159) + (fConst215 * fTemp19)) + (fConst216 * fTemp66)) + fTemp160) + (fConst218 * fTemp73)) + (fConst219 * fTemp21)) + fTemp161) + fTemp162) + fTemp163) + (fConst223 * fTemp25)) + (fConst224 * fTemp68)) + fTemp164)) + ((fConst30 * (((((fConst38 * fTemp165) + (fConst226 * fTemp87)) + (fConst194 * fTemp64)) + (fConst227 * fTemp46)) + (fConst195 * fTemp23))) + ((fConst47 * ((fConst228 * fTemp107) + (fConst229 * fTemp141))) + ((fConst20 * ((fTemp145 + (fConst230 * fTemp50)) + (fConst201 * fTemp48))) + ((fConst231 * fTemp55) + ((fConst232 * fTemp93) + (fTemp143 + fTemp166))))))) - fTemp154));
			fRec255[0] = max((fRec255[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp167))))));
			fVbargraph55 = FAUSTFLOAT(fRec255[0]);
			output19[i] = FAUSTFLOAT(fTemp167);
			double fTemp168 = (fConst51 * ((0.13819765978853429 * fTemp38) + (0.13819765978853399 * fTemp53)));
			double fTemp169 = (fTemp0 + (fConst210 * fTemp50));
			double fTemp170 = (fConst234 * fTemp32);
			double fTemp171 = (fConst236 * fTemp43);
			double fTemp172 = ((1.9931170733916111e-17 * fTemp29) + (0.044881006545161174 * fTemp109));
			double fTemp173 = (fRec1[0] * ((fTemp168 + ((0.021093750000000001 * (fTemp164 + ((((fTemp161 + ((((((((((fTemp3 + fTemp169) + fTemp170) + (fConst212 * fTemp91)) + (fConst235 * fTemp71)) + fTemp171) + (fConst215 * fTemp48)) + (fConst237 * fTemp19)) + (fConst238 * fTemp87)) + (fConst218 * fTemp93)) + (fConst239 * fTemp73))) + (fConst240 * fTemp46)) + (fConst223 * fTemp55)) + (fConst241 * fTemp25)))) + ((fConst30 * (((fConst242 * fTemp27) + (fConst243 * fTemp64)) + (fConst244 * fTemp23))) + ((fConst26 * (((((((fConst41 * fTemp172) + (fConst245 * fTemp136)) + (fConst246 * fTemp66)) + (fConst247 * fTemp105)) + (fConst248 * fTemp21)) + (fConst249 * fTemp138)) + (fConst250 * fTemp68))) + (fTemp143 + (fConst47 * ((fConst251 * fTemp107) + (fConst252 * fTemp141)))))))) - fTemp154));
			fRec256[0] = max((fRec256[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp173))))));
			fVbargraph56 = FAUSTFLOAT(fRec256[0]);
			output20[i] = FAUSTFLOAT(fTemp173);
			double fTemp174 = (fConst259 * fTemp53);
			double fTemp175 = (fTemp0 + (fConst234 * fTemp50));
			double fTemp176 = (fConst261 * fTemp29);
			double fTemp177 = (fConst262 * fTemp43);
			double fTemp178 = (fConst264 * fTemp27);
			double fTemp179 = (fConst271 * fTemp38);
			double fTemp180 = (fRec1[0] * ((((fConst47 * ((fConst253 * fTemp107) + (fConst254 * fTemp141))) + ((fConst20 * (((fConst255 * fTemp32) + (fConst256 * fTemp71)) + (fConst257 * fTemp19))) + (((fConst258 * fTemp73) + (fTemp143 + fTemp174)) + (fConst260 * fTemp25)))) + (0.021093750000000001 * ((fTemp164 + ((((fTemp163 + ((fTemp161 + ((((fTemp160 + (((((fTemp159 + ((((fTemp157 + (fTemp3 + fTemp175)) + (fConst235 * fTemp91)) + fTemp176) + fTemp177)) + (fConst237 * fTemp48)) + (fConst263 * fTemp66)) + fTemp178) + (fConst265 * fTemp87))) + (fConst239 * fTemp93)) + (fConst266 * fTemp21)) + (fConst267 * fTemp64))) + (fConst268 * fTemp46))) + (fConst241 * fTemp55)) + (fConst269 * fTemp68)) + (fConst270 * fTemp23))) + fTemp179))) - fTemp154));
			fRec257[0] = max((fRec257[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp180))))));
			fVbargraph57 = FAUSTFLOAT(fRec257[0]);
			output21[i] = FAUSTFLOAT(fTemp180);
			double fTemp181 = (fConst283 * fTemp141);
			double fTemp182 = (fTemp153 + (fTemp143 + fTemp151));
			double fTemp183 = (fTemp0 - fTemp3);
			double fTemp184 = (fRec1[0] * ((((fConst274 * ((2.2079425200240589e-18 * fTemp68) + (0.0099436891104358258 * fTemp138))) + ((fConst278 * fTemp139) + ((fConst281 * fTemp46) + (fTemp181 + ((fConst284 * ((3.3119137800360884e-18 * fTemp21) + (0.014915533665653739 * fTemp105))) + ((fConst285 * fTemp142) + ((fConst287 * fTemp87) + ((fConst288 * ((6.6238275600721768e-18 * fTemp66) + (0.029831067331307477 * fTemp136))) + ((fConst289 * ((0.010546875000000002 * fTemp19) + (0.010546875000000001 * fTemp48))) + ((fConst291 * fTemp71) + ((fConst42 * fTemp134) + ((fConst45 * fTemp144) + ((fConst292 * fTemp107) + ((fConst293 * fTemp43) + fTemp182)))))))))))))) + (0.021093750000000001 * (fTemp150 + (((fTemp149 + (fTemp148 + ((fTemp147 + (fTemp183 + (fConst290 * fTemp91))) + (fConst294 * fTemp64)))) + (fConst295 * fTemp23)) + (fConst296 * fTemp61))))) - fTemp152));
			fRec258[0] = max((fRec258[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp184))))));
			fVbargraph58 = FAUSTFLOAT(fRec258[0]);
			output22[i] = FAUSTFLOAT(fTemp184);
			double fTemp185 = (fConst298 * fTemp136);
			double fTemp186 = (fConst301 * fTemp105);
			double fTemp187 = (fConst304 * fTemp138);
			double fTemp188 = (fConst307 * fTemp61);
			double fTemp189 = (fRec1[0] * (((0.021093750000000001 * (((((fTemp162 + (fTemp161 + (((((((fTemp158 + ((fTemp157 + (fTemp156 + fTemp183)) + (fConst297 * fTemp71))) + fTemp185) + (fConst299 * fTemp19)) + (fConst300 * fTemp66)) + fTemp186) + (fConst302 * fTemp73)) + (fConst303 * fTemp21)))) + fTemp187) + (fConst305 * fTemp25)) + (fConst306 * fTemp68)) + fTemp188)) + ((fConst280 * ((0.010546875000000006 * fTemp23) + (0.010546874999999997 * fTemp46))) + ((fConst286 * ((0.021093749999999994 * fTemp87) + (0.021093750000000012 * fTemp64))) + ((fConst308 * fTemp55) + (fTemp181 + ((fConst309 * fTemp93) + ((fConst310 * fTemp48) + ((fConst291 * fTemp91) + ((fConst39 * fTemp165) + ((fConst311 * fTemp107) + ((fConst312 * fTemp50) + (fTemp166 + fTemp182)))))))))))) - fTemp152));
			fRec259[0] = max((fRec259[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp189))))));
			fVbargraph59 = FAUSTFLOAT(fRec259[0]);
			output23[i] = FAUSTFLOAT(fTemp189);
			double fTemp190 = (fConst321 * fTemp141);
			double fTemp191 = (fRec1[0] * (((0.021093750000000001 * (fTemp188 + ((((fTemp161 + ((((((fTemp171 + (((fTemp170 - (fTemp3 - fTemp169)) + (fConst297 * fTemp91)) + (fConst313 * fTemp71))) + (fConst299 * fTemp48)) + (fConst314 * fTemp19)) + (fConst315 * fTemp87)) + (fConst302 * fTemp93)) + (fConst316 * fTemp73))) + (fConst317 * fTemp46)) + (fConst305 * fTemp55)) + (fConst318 * fTemp25)))) + ((fConst274 * ((4.4158850400481179e-18 * fTemp68) + (0.0099436891104358275 * fTemp138))) + ((fConst284 * ((6.6238275600721768e-18 * fTemp21) + (0.01491553366565374 * fTemp105))) + ((fConst288 * ((1.3247655120144354e-17 * fTemp66) + (0.029831067331307481 * fTemp136))) + ((fConst319 * fTemp23) + ((fConst320 * fTemp64) + (fTemp190 + (fTemp168 + ((fConst42 * fTemp172) + ((fConst322 * fTemp107) + ((fConst323 * fTemp27) + fTemp182))))))))))) - fTemp152));
			fRec260[0] = max((fRec260[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp191))))));
			fVbargraph60 = FAUSTFLOAT(fRec260[0]);
			output24[i] = FAUSTFLOAT(fTemp191);
			double fTemp192 = (fRec1[0] * ((((fTemp190 + ((fConst324 * fTemp73) + ((fConst325 * fTemp19) + ((fConst326 * fTemp71) + ((fConst327 * fTemp107) + (fTemp174 + ((fConst328 * fTemp32) + fTemp182))))))) + (fConst329 * fTemp25)) + (0.021093750000000001 * (fTemp179 + (fTemp188 + ((((fTemp187 + ((fTemp161 + ((((fTemp186 + ((fTemp178 + (((fTemp185 + (fTemp177 + (fTemp176 + ((fTemp157 - (fTemp3 - fTemp175)) + (fConst313 * fTemp91))))) + (fConst314 * fTemp48)) + (fConst330 * fTemp66))) + (fConst331 * fTemp87))) + (fConst316 * fTemp93)) + (fConst332 * fTemp21)) + (fConst333 * fTemp64))) + (fConst334 * fTemp46))) + (fConst318 * fTemp55)) + (fConst335 * fTemp68)) + (fConst336 * fTemp23)))))) - fTemp152));
			fRec261[0] = max((fRec261[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp192))))));
			fVbargraph61 = FAUSTFLOAT(fRec261[0]);
			output25[i] = FAUSTFLOAT(fTemp192);
			double fTemp193 = ((0.0097062489627452956 * fTemp32) + (0.0097062489627452973 * fTemp50));
			double fTemp194 = (fConst337 * fTemp91);
			double fTemp195 = (fConst339 * fTemp48);
			double fTemp196 = (fConst341 * fTemp93);
			double fTemp197 = (fConst343 * fTemp55);
			double fTemp198 = (fConst344 * fTemp25);
			double fTemp199 = (0.031605542669238249 * fTemp3);
			double fTemp200 = (0.032806508796871948 * fTemp6);
			double fTemp201 = (0.026333598483989317 * fTemp9);
			double fTemp202 = (0.0142547123015874 * fTemp12);
			double fTemp203 = (((fTemp199 + fTemp200) + fTemp201) + fTemp202);
			double fTemp204 = (fConst350 * fTemp87);
			double fTemp205 = (fConst351 * fTemp46);
			double fTemp206 = (fConst352 * fTemp29);
			double fTemp207 = (fTemp0 + fTemp206);
			double fTemp208 = (fConst353 * fTemp66);
			double fTemp209 = (fConst354 * fTemp27);
			double fTemp210 = (fConst355 * fTemp107);
			double fTemp211 = (fConst356 * fTemp21);
			double fTemp212 = (fConst358 * fTemp35);
			double fTemp213 = (fConst359 * fTemp53);
			double fTemp214 = (fConst361 * fTemp68);
			double fTemp215 = (fConst363 * fTemp61);
			double fTemp216 = (fConst364 * fTemp38);
			double fTemp217 = (0.0007502480158728946 * fTemp15);
			double fTemp218 = (fRec1[0] * ((((fConst20 * (((((((((fConst44 * fTemp193) + fTemp194) + (fConst338 * fTemp71)) + fTemp195) + (fConst340 * fTemp19)) + fTemp196) + (fConst342 * fTemp73)) + fTemp197) + fTemp198)) + ((fConst26 * ((((fConst345 * fTemp136) + (fConst346 * fTemp109)) + (fConst347 * fTemp105)) + (fConst348 * fTemp138))) + (fTemp203 + (fConst30 * (((fConst349 * fTemp43) + fTemp204) + fTemp205))))) + (0.02017333553791887 * ((((((((((((fTemp207 + fTemp208) + fTemp209) + fTemp210) + fTemp211) + (fConst357 * fTemp64)) + fTemp212) + fTemp213) + (fConst360 * fTemp141)) + fTemp214) + (fConst362 * fTemp23)) + fTemp215) + fTemp216))) - fTemp217));
			fRec262[0] = max((fRec262[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp218))))));
			fVbargraph62 = FAUSTFLOAT(fRec262[0]);
			output26[i] = FAUSTFLOAT(fTemp218);
			double fTemp219 = (fConst366 * fTemp109);
			double fTemp220 = ((fTemp0 + (fConst365 * fTemp32)) + fTemp219);
			double fTemp221 = (fConst368 * fTemp136);
			double fTemp222 = (fConst370 * fTemp105);
			double fTemp223 = (fConst372 * fTemp138);
			double fTemp224 = ((0.0070590901547238384 * fTemp27) + (0.0070590901547238332 * fTemp43));
			double fTemp225 = (fConst380 * fTemp38);
			double fTemp226 = (fRec1[0] * (((0.02017333553791887 * (fTemp215 + (fTemp214 + (((fTemp213 + (fTemp212 + (fTemp211 + (((fTemp208 + (((fTemp206 + (fTemp220 + (fConst367 * fTemp71))) + fTemp221) + (fConst369 * fTemp19))) + fTemp222) + (fConst371 * fTemp73))))) + fTemp223) + (fConst374 * fTemp25))))) + ((fConst30 * ((fTemp205 + ((fTemp204 + (fConst38 * fTemp224)) + (fConst375 * fTemp64))) + (fConst376 * fTemp23))) + ((fConst20 * (fTemp197 + (fTemp196 + (fTemp195 + (fTemp194 + (fConst377 * fTemp50)))))) + ((fConst47 * ((fConst378 * fTemp107) + (fConst379 * fTemp141))) + (fTemp203 + fTemp225))))) - fTemp217));
			fRec263[0] = max((fRec263[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp226))))));
			fVbargraph63 = FAUSTFLOAT(fRec263[0]);
			output27[i] = FAUSTFLOAT(fTemp226);
			double fTemp227 = (fConst51 * ((0.0051338837488900581 * fTemp38) + (0.0051338837488900469 * fTemp53)));
			double fTemp228 = ((fTemp0 + (fConst365 * fTemp50)) + (fConst381 * fTemp32));
			double fTemp229 = (fConst383 * fTemp43);
			double fTemp230 = ((5.19858676206982e-18 * fTemp29) + (0.011706176702255421 * fTemp109));
			double fTemp231 = (fRec1[0] * ((fTemp227 + ((0.02017333553791887 * (fTemp215 + ((((fTemp212 + ((((((((fTemp228 + (fConst367 * fTemp91)) + (fConst382 * fTemp71)) + fTemp229) + (fConst369 * fTemp48)) + (fConst384 * fTemp19)) + (fConst385 * fTemp87)) + (fConst371 * fTemp93)) + (fConst386 * fTemp73))) + (fConst387 * fTemp46)) + (fConst374 * fTemp55)) + (fConst388 * fTemp25)))) + ((fConst30 * (((fConst389 * fTemp27) + (fConst390 * fTemp64)) + (fConst391 * fTemp23))) + ((fConst26 * (((((((fConst41 * fTemp230) + (fConst392 * fTemp136)) + (fConst393 * fTemp66)) + (fConst394 * fTemp105)) + (fConst395 * fTemp21)) + (fConst396 * fTemp138)) + (fConst397 * fTemp68))) + (fTemp203 + (fConst47 * ((fConst398 * fTemp107) + (fConst399 * fTemp141)))))))) - fTemp217));
			fRec264[0] = max((fRec264[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp231))))));
			fVbargraph64 = FAUSTFLOAT(fRec264[0]);
			output28[i] = FAUSTFLOAT(fTemp231);
			double fTemp232 = (fConst404 * fTemp53);
			double fTemp233 = (fTemp219 + (fTemp0 + (fConst381 * fTemp50)));
			double fTemp234 = (fConst407 * fTemp29);
			double fTemp235 = (fConst408 * fTemp43);
			double fTemp236 = (fConst410 * fTemp27);
			double fTemp237 = (fConst417 * fTemp38);
			double fTemp238 = (fRec1[0] * ((((fConst20 * (fTemp198 + ((((fConst400 * fTemp32) + (fConst401 * fTemp71)) + (fConst402 * fTemp19)) + (fConst403 * fTemp73)))) + ((fTemp203 + fTemp232) + (fConst47 * ((fConst405 * fTemp107) + (fConst406 * fTemp141))))) + (0.02017333553791887 * ((fTemp215 + ((((fTemp223 + ((fTemp212 + ((((fTemp222 + (((((fTemp221 + (((fTemp233 + (fConst382 * fTemp91)) + fTemp234) + fTemp235)) + (fConst384 * fTemp48)) + (fConst409 * fTemp66)) + fTemp236) + (fConst411 * fTemp87))) + (fConst386 * fTemp93)) + (fConst412 * fTemp21)) + (fConst413 * fTemp64))) + (fConst414 * fTemp46))) + (fConst388 * fTemp55)) + (fConst415 * fTemp68)) + (fConst416 * fTemp23))) + fTemp237))) - fTemp217));
			fRec265[0] = max((fRec265[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp238))))));
			fVbargraph65 = FAUSTFLOAT(fRec265[0]);
			output29[i] = FAUSTFLOAT(fTemp238);
			double fTemp239 = (0.02017333553791887 * (fTemp0 + (fConst418 * fTemp38)));
			double fTemp240 = (0.20432599334845877 * fTemp107);
			double fTemp241 = ((0.059595081393300019 * fTemp35) + fTemp240);
			double fTemp242 = (fConst419 * fTemp141);
			double fTemp243 = (fConst420 * fTemp61);
			double fTemp244 = ((0.0081196109272257799 * fTemp23) + (0.011728326894881718 * fTemp46));
			double fTemp245 = ((0.007607841257607462 * fTemp68) + (0.0057058809432056056 * fTemp138));
			double fTemp246 = ((0.006766342439354831 * fTemp25) + (0.0022554474797849468 * fTemp55));
			double fTemp247 = ((0.063531811392514598 * fTemp27) + (0.091768172011410243 * fTemp43));
			double fTemp248 = ((0.011411761886411192 * fTemp21) + (0.008558821414808411 * fTemp105));
			double fTemp249 = ((0.0090217899191397753 * fTemp73) + (0.0030072633063799288 * fTemp93));
			double fTemp250 = ((0.046824706809021746 * fTemp29) + (0.035118530106766375 * fTemp109));
			double fTemp251 = (fConst432 * fTemp66);
			double fTemp252 = ((0.013532684878709662 * fTemp19) + (0.0045108949595698937 * fTemp48));
			double fTemp253 = ((0.029118746888235916 * fTemp32) + (0.0097062489627453199 * fTemp50));
			double fTemp254 = (fConst436 * fTemp53);
			double fTemp255 = (0.02309096671075838 * fTemp15);
			double fTemp256 = (0.010535180889746075 * fTemp3);
			double fTemp257 = (0.0042514054232804339 * fTemp12);
			double fTemp258 = (fTemp255 + (fTemp256 + fTemp257));
			double fTemp259 = (0.01640325439843596 * fTemp6);
			double fTemp260 = (0.020481687709769415 * fTemp9);
			double fTemp261 = (fTemp259 + fTemp260);
			double fTemp262 = (fRec1[0] * ((fTemp239 + ((fConst47 * (((fConst48 * fTemp241) + fTemp242) + fTemp243)) + ((fConst421 * fTemp244) + ((fConst423 * fTemp245) + ((fConst426 * fTemp246) + ((fConst30 * (((fConst38 * fTemp247) + (fConst427 * fTemp87)) + (fConst428 * fTemp64))) + ((fConst429 * fTemp248) + ((fConst430 * fTemp249) + ((fConst26 * (((fConst41 * fTemp250) + (fConst431 * fTemp136)) + fTemp251)) + ((fConst433 * fTemp252) + ((fConst20 * (((fConst44 * fTemp253) + (fConst434 * fTemp91)) + (fConst435 * fTemp71))) + (fTemp254 + fTemp258)))))))))))) - fTemp261));
			fRec266[0] = max((fRec266[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp262))))));
			fVbargraph66 = FAUSTFLOAT(fRec266[0]);
			output30[i] = FAUSTFLOAT(fTemp262);
			double fTemp263 = ((fTemp0 + (fConst439 * fTemp29)) + (fConst440 * fTemp43));
			double fTemp264 = (fConst442 * fTemp27);
			double fTemp265 = (fConst443 * fTemp107);
			double fTemp266 = (fConst445 * fTemp21);
			double fTemp267 = (fConst447 * fTemp53);
			double fTemp268 = ((0.0022554474797849442 * fTemp25) + (0.0067663424393548319 * fTemp55));
			double fTemp269 = ((0.0030072633063799254 * fTemp73) + (0.0090217899191397753 * fTemp93));
			double fTemp270 = ((0.0045108949595698885 * fTemp19) + (0.013532684878709664 * fTemp48));
			double fTemp271 = ((0.0097062489627453077 * fTemp32) + (0.029118746888235923 * fTemp50));
			double fTemp272 = (fConst458 * fTemp38);
			double fTemp273 = (fRec1[0] * (((fConst47 * ((fConst437 * fTemp35) + (fConst438 * fTemp61))) + ((0.02017333553791887 * (((((((((((fTemp263 + (fConst441 * fTemp66)) + fTemp264) + fTemp265) + (fConst444 * fTemp87)) + fTemp266) + (fConst446 * fTemp64)) + fTemp267) + (fConst448 * fTemp141)) + (fConst449 * fTemp46)) + (fConst450 * fTemp68)) + (fConst451 * fTemp23))) + ((fConst426 * fTemp268) + ((fConst430 * fTemp269) + ((fConst433 * fTemp270) + ((fConst26 * ((fConst452 * fTemp136) + (fConst453 * fTemp109))) + ((fConst20 * (((fConst44 * fTemp271) + (fConst454 * fTemp91)) + (fConst455 * fTemp71))) + ((fConst456 * fTemp138) + ((fConst457 * fTemp105) + (fTemp258 + fTemp272)))))))))) - fTemp261));
			fRec267[0] = max((fRec267[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp273))))));
			fVbargraph67 = FAUSTFLOAT(fRec267[0]);
			output31[i] = FAUSTFLOAT(fTemp273);
			double fTemp274 = ((fTemp0 + (fConst459 * fTemp32)) + (fConst460 * fTemp109));
			double fTemp275 = (fConst462 * fTemp29);
			double fTemp276 = (fConst463 * fTemp43);
			double fTemp277 = (fConst470 * fTemp53);
			double fTemp278 = (fConst475 * fTemp38);
			double fTemp279 = ((0.059595081393300539 * fTemp35) + (0.20432599334845869 * fTemp107));
			double fTemp280 = (fRec1[0] * (((0.02017333553791887 * (((((((fTemp266 + (((((((((fTemp274 + (fConst461 * fTemp71)) + fTemp275) + fTemp276) + (fConst464 * fTemp136)) + (fConst465 * fTemp19)) + (fConst466 * fTemp66)) + (fConst467 * fTemp87)) + (fConst468 * fTemp105)) + (fConst469 * fTemp73))) + fTemp277) + (fConst471 * fTemp46)) + (fConst472 * fTemp138)) + (fConst473 * fTemp25)) + (fConst474 * fTemp68)) + fTemp278)) + (((fConst30 * ((fConst476 * fTemp27) + (fConst477 * fTemp64))) + ((fConst20 * ((fConst478 * fTemp50) + (fConst479 * fTemp91))) + (((fConst480 * fTemp55) + ((fConst481 * fTemp93) + (fTemp258 + (fConst482 * fTemp48)))) + (fConst483 * fTemp23)))) + (fConst47 * (((fConst48 * fTemp279) + (fConst484 * fTemp141)) + (fConst485 * fTemp61))))) - fTemp261));
			fRec268[0] = max((fRec268[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp280))))));
			fVbargraph68 = FAUSTFLOAT(fRec268[0]);
			output32[i] = FAUSTFLOAT(fTemp280);
			double fTemp281 = (fConst51 * ((0.015401651246671252 * fTemp38) + (0.40557681616231639 * fTemp53)));
			double fTemp282 = ((fTemp0 + (fConst487 * fTemp32)) + (fConst488 * fTemp109));
			double fTemp283 = (fConst492 * fTemp27);
			double fTemp284 = (fConst493 * fTemp107);
			double fTemp285 = (fRec1[0] * ((fTemp281 + ((fConst47 * (fTemp243 + (fConst486 * fTemp35))) + ((0.02017333553791887 * ((((((((((((fTemp282 + (fConst489 * fTemp71)) + (fConst490 * fTemp136)) + (fConst491 * fTemp19)) + fTemp283) + fTemp284) + (fConst494 * fTemp105)) + (fConst495 * fTemp73)) + (fConst496 * fTemp64)) + (fConst497 * fTemp141)) + (fConst498 * fTemp138)) + (fConst499 * fTemp25)) + (fConst500 * fTemp23))) + ((fConst30 * ((fConst501 * fTemp43) + (fConst502 * fTemp87))) + ((fConst26 * (fTemp251 + (fConst503 * fTemp29))) + ((fConst20 * ((fConst504 * fTemp50) + (fConst505 * fTemp91))) + ((fConst506 * fTemp68) + ((fConst507 * fTemp55) + ((fConst508 * fTemp46) + ((fConst509 * fTemp21) + ((fConst510 * fTemp93) + (fTemp258 + (fConst511 * fTemp48))))))))))))) - fTemp261));
			fRec269[0] = max((fRec269[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp285))))));
			fVbargraph69 = FAUSTFLOAT(fRec269[0]);
			output33[i] = FAUSTFLOAT(fTemp285);
			double fTemp286 = (fTemp240 + (0.059595081393300352 * fTemp35));
			double fTemp287 = ((fTemp0 + (fConst513 * fTemp50)) + (fConst514 * fTemp32));
			double fTemp288 = (fConst517 * fTemp43);
			double fTemp289 = (fConst520 * fTemp27);
			double fTemp290 = (fConst525 * fTemp53);
			double fTemp291 = ((0.0076078412576074646 * fTemp68) + (0.0057058809432056047 * fTemp138));
			double fTemp292 = ((0.011411761886411199 * fTemp21) + (0.0085588214148084076 * fTemp105));
			double fTemp293 = ((0.046824706809021774 * fTemp29) + (0.035118530106766362 * fTemp109));
			double fTemp294 = (fConst532 * fTemp38);
			double fTemp295 = (fRec1[0] * (((fConst47 * ((fTemp242 + (fConst48 * fTemp286)) + (fConst512 * fTemp61))) + ((0.02017333553791887 * (((((((((((((((fTemp287 + (fConst515 * fTemp91)) + (fConst516 * fTemp71)) + fTemp288) + (fConst518 * fTemp48)) + (fConst519 * fTemp19)) + fTemp289) + (fConst521 * fTemp87)) + (fConst522 * fTemp93)) + (fConst523 * fTemp73)) + (fConst524 * fTemp64)) + fTemp290) + (fConst526 * fTemp46)) + (fConst527 * fTemp55)) + (fConst528 * fTemp25)) + (fConst529 * fTemp23))) + ((fConst423 * fTemp291) + ((fConst429 * fTemp292) + ((fConst26 * (((fConst41 * fTemp293) + (fConst530 * fTemp136)) + (fConst531 * fTemp66))) + (fTemp258 + fTemp294)))))) - fTemp261));
			fRec270[0] = max((fRec270[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp295))))));
			fVbargraph70 = FAUSTFLOAT(fRec270[0]);
			output34[i] = FAUSTFLOAT(fTemp295);
			double fTemp296 = ((fTemp0 + (fConst514 * fTemp50)) + (fConst533 * fTemp32));
			double fTemp297 = (fConst535 * fTemp29);
			double fTemp298 = (fConst538 * fTemp107);
			double fTemp299 = ((0.011728326894881713 * fTemp23) + (0.0081196109272257937 * fTemp46));
			double fTemp300 = ((0.091768172011410187 * fTemp27) + (0.063531811392514695 * fTemp43));
			double fTemp301 = (fConst552 * fTemp53);
			double fTemp302 = (fRec1[0] * (((0.02017333553791887 * (fTemp278 + ((((((((((((((fTemp296 + (fConst516 * fTemp91)) + (fConst534 * fTemp71)) + fTemp297) + (fConst519 * fTemp48)) + (fConst536 * fTemp19)) + (fConst537 * fTemp66)) + fTemp298) + (fConst523 * fTemp93)) + (fConst539 * fTemp73)) + (fConst540 * fTemp21)) + (fConst541 * fTemp141)) + (fConst528 * fTemp55)) + (fConst542 * fTemp25)) + (fConst543 * fTemp68)))) + ((fConst47 * ((fConst544 * fTemp35) + (fConst545 * fTemp61))) + ((fConst421 * fTemp299) + ((fConst26 * ((fConst546 * fTemp136) + (fConst547 * fTemp109))) + ((fConst30 * ((fConst548 * fTemp64) + ((fConst38 * fTemp300) + (fConst549 * fTemp87)))) + ((fConst550 * fTemp138) + ((fConst551 * fTemp105) + (fTemp258 + fTemp301)))))))) - fTemp261));
			fRec271[0] = max((fRec271[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp302))))));
			fVbargraph71 = FAUSTFLOAT(fRec271[0]);
			output35[i] = FAUSTFLOAT(fTemp302);
			double fTemp303 = (fConst51 * ((0.40557681616231639 * fTemp38) + (0.015401651246669784 * fTemp53)));
			double fTemp304 = ((0.059595081393300636 * fTemp35) + (0.20432599334845863 * fTemp107));
			double fTemp305 = ((fTemp0 + (fConst555 * fTemp50)) + (fConst556 * fTemp109));
			double fTemp306 = (fConst558 * fTemp29);
			double fTemp307 = (fConst562 * fTemp27);
			double fTemp308 = (fRec1[0] * ((fTemp303 + ((fConst47 * (((fConst48 * fTemp304) + (fConst553 * fTemp141)) + (fConst554 * fTemp61))) + ((0.02017333553791887 * ((((((((((((((fTemp305 + (fConst557 * fTemp91)) + fTemp306) + (fConst559 * fTemp136)) + (fConst560 * fTemp48)) + (fConst561 * fTemp66)) + fTemp307) + (fConst563 * fTemp105)) + (fConst564 * fTemp93)) + (fConst565 * fTemp21)) + (fConst566 * fTemp64)) + (fConst567 * fTemp138)) + (fConst568 * fTemp55)) + (fConst569 * fTemp68)) + (fConst570 * fTemp23))) + ((fConst30 * ((fConst571 * fTemp43) + (fConst572 * fTemp87))) + ((fConst20 * ((fConst573 * fTemp32) + (fConst574 * fTemp71))) + ((fConst575 * fTemp25) + ((fConst576 * fTemp46) + ((fConst577 * fTemp73) + (fTemp258 + (fConst578 * fTemp19)))))))))) - fTemp261));
			fRec272[0] = max((fRec272[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp308))))));
			fVbargraph72 = FAUSTFLOAT(fRec272[0]);
			output36[i] = FAUSTFLOAT(fTemp308);
			double fTemp309 = ((fTemp0 + (fConst579 * fTemp50)) + (fConst580 * fTemp109));
			double fTemp310 = (fConst582 * fTemp43);
			double fTemp311 = (fConst585 * fTemp107);
			double fTemp312 = (fConst589 * fTemp53);
			double fTemp313 = (fConst594 * fTemp38);
			double fTemp314 = (fRec1[0] * (((0.02017333553791887 * ((((((((((((((fTemp309 + (fConst581 * fTemp91)) + fTemp310) + (fConst583 * fTemp136)) + (fConst584 * fTemp48)) + fTemp311) + (fConst586 * fTemp87)) + (fConst587 * fTemp105)) + (fConst588 * fTemp93)) + fTemp312) + (fConst590 * fTemp141)) + (fConst591 * fTemp46)) + (fConst592 * fTemp138)) + (fConst593 * fTemp55)) + fTemp313)) + ((fConst47 * ((fConst595 * fTemp35) + (fConst596 * fTemp61))) + ((fConst30 * ((fConst597 * fTemp27) + (fConst598 * fTemp64))) + ((fConst26 * ((fConst599 * fTemp29) + (fConst600 * fTemp66))) + ((fConst20 * ((fConst601 * fTemp32) + (fConst602 * fTemp71))) + ((fConst603 * fTemp23) + ((fConst604 * fTemp68) + ((fConst605 * fTemp25) + ((fConst606 * fTemp21) + ((fConst607 * fTemp73) + (fTemp258 + (fConst608 * fTemp19)))))))))))) - fTemp261));
			fRec273[0] = max((fRec273[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp314))))));
			fVbargraph73 = FAUSTFLOAT(fRec273[0]);
			output37[i] = FAUSTFLOAT(fTemp314);
			double fTemp315 = (fTemp260 + fTemp257);
			double fTemp316 = (0.027388228527386899 * fTemp141);
			double fTemp317 = (fTemp255 + (fTemp256 + fTemp259));
			double fTemp318 = (fRec1[0] * ((((fConst610 * fTemp244) + ((fConst613 * fTemp245) + ((fConst617 * fTemp246) + ((fConst618 * ((0.023456653789763436 * fTemp87) + (0.01623922185445156 * fTemp64))) + ((fConst619 * fTemp248) + ((fConst620 * fTemp249) + ((fConst621 * ((0.022823523772822384 * fTemp66) + (0.017117642829616822 * fTemp136))) + ((fConst622 * fTemp252) + ((fConst623 * ((0.027065369757419324 * fTemp71) + (0.0090217899191397874 * fTemp91))) + (fTemp239 + ((fConst49 * fTemp241) + ((fConst39 * fTemp247) + ((fConst42 * fTemp250) + ((fConst45 * fTemp253) + (fTemp254 + fTemp315))))))))))))))) + (fConst624 * (fTemp316 + (0.0079882333204877847 * fTemp61)))) - fTemp317));
			fRec274[0] = max((fRec274[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp318))))));
			fVbargraph74 = FAUSTFLOAT(fRec274[0]);
			output38[i] = FAUSTFLOAT(fTemp318);
			double fTemp319 = (fConst625 * fTemp66);
			double fTemp320 = (fConst627 * fTemp21);
			double fTemp321 = (fRec1[0] * (((0.02017333553791887 * (((((fTemp267 + ((((fTemp265 + (fTemp264 + (fTemp263 + fTemp319))) + (fConst626 * fTemp87)) + fTemp320) + (fConst628 * fTemp64))) + (fConst629 * fTemp141)) + (fConst630 * fTemp46)) + (fConst631 * fTemp68)) + (fConst632 * fTemp23))) + ((fConst617 * fTemp268) + ((fConst620 * fTemp269) + ((fConst622 * fTemp270) + ((fConst623 * ((0.009021789919139777 * fTemp71) + (0.027065369757419327 * fTemp91))) + ((fConst633 * fTemp61) + ((fConst634 * fTemp138) + ((fConst635 * fTemp105) + ((fConst636 * fTemp136) + ((fConst45 * fTemp271) + ((fConst637 * fTemp109) + (fTemp272 + ((fConst638 * fTemp35) + fTemp315))))))))))))) - fTemp317));
			fRec275[0] = max((fRec275[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp321))))));
			fVbargraph75 = FAUSTFLOAT(fRec275[0]);
			output39[i] = FAUSTFLOAT(fTemp321);
			double fTemp322 = (fRec1[0] * (((0.02017333553791887 * (fTemp278 + (((((fTemp277 + (fTemp320 + ((((fTemp319 + (((fTemp276 + (fTemp275 + (fTemp274 + (fConst639 * fTemp71)))) + (fConst640 * fTemp136)) + (fConst641 * fTemp19))) + (fConst642 * fTemp87)) + (fConst643 * fTemp105)) + (fConst644 * fTemp73)))) + (fConst645 * fTemp46)) + (fConst646 * fTemp138)) + (fConst647 * fTemp25)) + (fConst648 * fTemp68)))) + ((fConst624 * ((0.027388228527386888 * fTemp141) + (0.0079882333204878541 * fTemp61))) + ((fConst649 * fTemp23) + (((fConst650 * fTemp64) + ((fConst651 * fTemp93) + ((fConst652 * fTemp48) + ((fConst653 * fTemp91) + ((fConst49 * fTemp279) + ((fConst654 * fTemp50) + ((fConst655 * fTemp27) + fTemp315))))))) + (fConst656 * fTemp55))))) - fTemp317));
			fRec276[0] = max((fRec276[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp322))))));
			fVbargraph76 = FAUSTFLOAT(fRec276[0]);
			output40[i] = FAUSTFLOAT(fTemp322);
			double fTemp323 = (fRec1[0] * (((0.02017333553791887 * ((((((((fTemp284 + (fTemp283 + (((fTemp282 + (fConst657 * fTemp71)) + (fConst658 * fTemp136)) + (fConst659 * fTemp19)))) + (fConst660 * fTemp105)) + (fConst661 * fTemp73)) + (fConst662 * fTemp64)) + (fConst663 * fTemp141)) + (fConst664 * fTemp138)) + (fConst665 * fTemp25)) + (fConst666 * fTemp23))) + ((fConst667 * fTemp61) + ((fConst668 * fTemp68) + ((fConst669 * fTemp55) + ((fConst670 * fTemp46) + ((fConst671 * fTemp21) + ((fConst672 * fTemp93) + ((fConst673 * fTemp87) + ((fConst674 * fTemp66) + ((fConst675 * fTemp48) + ((fConst676 * fTemp91) + (fTemp281 + ((fConst677 * fTemp43) + ((fConst678 * fTemp50) + ((fConst679 * fTemp35) + ((fConst680 * fTemp29) + fTemp315)))))))))))))))) - fTemp317));
			fRec277[0] = max((fRec277[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp323))))));
			fVbargraph77 = FAUSTFLOAT(fRec277[0]);
			output41[i] = FAUSTFLOAT(fTemp323);
			double fTemp324 = (fRec1[0] * (((fConst624 * (fTemp316 + (0.0079882333204878298 * fTemp61))) + ((0.02017333553791887 * (((((fTemp290 + (((((fTemp289 + (((fTemp288 + ((fTemp287 + (fConst681 * fTemp91)) + (fConst682 * fTemp71))) + (fConst683 * fTemp48)) + (fConst684 * fTemp19))) + (fConst685 * fTemp87)) + (fConst686 * fTemp93)) + (fConst687 * fTemp73)) + (fConst688 * fTemp64))) + (fConst689 * fTemp46)) + (fConst690 * fTemp55)) + (fConst691 * fTemp25)) + (fConst692 * fTemp23))) + ((fConst613 * fTemp291) + ((fConst619 * fTemp292) + ((fConst621 * ((0.022823523772822398 * fTemp66) + (0.017117642829616815 * fTemp136))) + ((fConst49 * fTemp286) + ((fConst42 * fTemp293) + (fTemp294 + fTemp315)))))))) - fTemp317));
			fRec278[0] = max((fRec278[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp324))))));
			fVbargraph78 = FAUSTFLOAT(fRec278[0]);
			output42[i] = FAUSTFLOAT(fTemp324);
			double fTemp325 = (fRec1[0] * (((0.02017333553791887 * (fTemp278 + ((((((((fTemp298 + ((((fTemp297 + ((fTemp296 + (fConst682 * fTemp91)) + (fConst693 * fTemp71))) + (fConst684 * fTemp48)) + (fConst694 * fTemp19)) + (fConst695 * fTemp66))) + (fConst687 * fTemp93)) + (fConst696 * fTemp73)) + (fConst697 * fTemp21)) + (fConst698 * fTemp141)) + (fConst691 * fTemp55)) + (fConst699 * fTemp25)) + (fConst700 * fTemp68)))) + ((fConst610 * fTemp299) + ((fConst618 * ((0.016239221854451587 * fTemp87) + (0.023456653789763426 * fTemp64))) + ((((fConst701 * fTemp105) + ((fConst702 * fTemp136) + ((fConst39 * fTemp300) + ((fConst703 * fTemp109) + (fTemp301 + ((fConst704 * fTemp35) + fTemp315)))))) + (fConst705 * fTemp138)) + (fConst706 * fTemp61))))) - fTemp317));
			fRec279[0] = max((fRec279[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp325))))));
			fVbargraph79 = FAUSTFLOAT(fRec279[0]);
			output43[i] = FAUSTFLOAT(fTemp325);
			double fTemp326 = (fRec1[0] * (((fConst624 * ((0.027388228527386878 * fTemp141) + (0.007988233320487868 * fTemp61))) + ((0.02017333553791887 * (((((((((fTemp307 + ((((fTemp306 + (fTemp305 + (fConst707 * fTemp91))) + (fConst708 * fTemp136)) + (fConst709 * fTemp48)) + (fConst710 * fTemp66))) + (fConst711 * fTemp105)) + (fConst712 * fTemp93)) + (fConst713 * fTemp21)) + (fConst714 * fTemp64)) + (fConst715 * fTemp138)) + (fConst716 * fTemp55)) + (fConst717 * fTemp68)) + (fConst718 * fTemp23))) + ((fConst719 * fTemp25) + ((fConst720 * fTemp46) + ((fConst721 * fTemp73) + ((fConst722 * fTemp87) + ((fConst723 * fTemp19) + ((fConst724 * fTemp71) + (fTemp303 + ((fConst49 * fTemp304) + ((fConst725 * fTemp43) + ((fConst726 * fTemp32) + fTemp315)))))))))))) - fTemp317));
			fRec280[0] = max((fRec280[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp326))))));
			fVbargraph80 = FAUSTFLOAT(fRec280[0]);
			output44[i] = FAUSTFLOAT(fTemp326);
			double fTemp327 = (fRec1[0] * ((((((((fConst727 * fTemp64) + ((fConst728 * fTemp21) + ((fConst729 * fTemp73) + ((fConst730 * fTemp66) + ((fConst731 * fTemp19) + ((fConst732 * fTemp71) + ((fConst733 * fTemp35) + ((fConst734 * fTemp27) + ((fConst735 * fTemp29) + ((fConst736 * fTemp32) + fTemp315)))))))))) + (fConst737 * fTemp25)) + (fConst738 * fTemp68)) + (fConst739 * fTemp23)) + (fConst740 * fTemp61)) + (0.02017333553791887 * (fTemp313 + (((((fTemp312 + ((((fTemp311 + (((fTemp310 + (fTemp309 + (fConst741 * fTemp91))) + (fConst742 * fTemp136)) + (fConst743 * fTemp48))) + (fConst744 * fTemp87)) + (fConst745 * fTemp105)) + (fConst746 * fTemp93))) + (fConst747 * fTemp141)) + (fConst748 * fTemp46)) + (fConst749 * fTemp138)) + (fConst750 * fTemp55))))) - fTemp317));
			fRec281[0] = max((fRec281[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp327))))));
			fVbargraph81 = FAUSTFLOAT(fRec281[0]);
			output45[i] = FAUSTFLOAT(fTemp327);
			double fTemp328 = (fTemp217 + (fTemp200 + fTemp202));
			double fTemp329 = (fConst773 * fTemp66);
			double fTemp330 = (fConst774 * fTemp21);
			double fTemp331 = (fConst778 * fTemp68);
			double fTemp332 = (fConst780 * fTemp61);
			double fTemp333 = (fTemp199 + fTemp201);
			double fTemp334 = (fRec1[0] * ((((fConst754 * ((0.0050433338844797174 * fTemp25) + (0.0050433338844797183 * fTemp55))) + ((fConst758 * fTemp138) + ((fConst761 * fTemp46) + ((fConst762 * ((0.0067244451793062893 * fTemp73) + (0.0067244451793062911 * fTemp93))) + ((fConst764 * fTemp105) + ((fConst766 * fTemp87) + ((fConst767 * ((0.010086667768959435 * fTemp19) + (0.010086667768959437 * fTemp48))) + ((fConst769 * fTemp136) + ((fConst770 * ((0.02017333553791887 * fTemp71) + (0.020173335537918873 * fTemp91))) + ((fConst45 * fTemp193) + ((fConst771 * fTemp109) + ((fConst772 * fTemp43) + fTemp328)))))))))))) + (0.02017333553791887 * (fTemp216 + (((((fTemp213 + (fTemp212 + (((fTemp210 + (fTemp209 + (fTemp207 + fTemp329))) + fTemp330) + (fConst775 * fTemp64)))) + (fConst777 * fTemp141)) + fTemp331) + (fConst779 * fTemp23)) + fTemp332)))) - fTemp333));
			fRec282[0] = max((fRec282[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp334))))));
			fVbargraph82 = FAUSTFLOAT(fRec282[0]);
			output46[i] = FAUSTFLOAT(fTemp334);
			double fTemp335 = (fConst782 * fTemp136);
			double fTemp336 = (fConst784 * fTemp105);
			double fTemp337 = (fConst786 * fTemp138);
			double fTemp338 = (fRec1[0] * (((0.02017333553791887 * (fTemp332 + (fTemp331 + (((fTemp213 + (fTemp212 + (fTemp330 + (((fTemp329 + (((fTemp206 + (fTemp220 + (fConst781 * fTemp71))) + fTemp335) + (fConst783 * fTemp19))) + fTemp336) + (fConst785 * fTemp73))))) + fTemp337) + (fConst787 * fTemp25))))) + ((fConst760 * ((0.01008666776895944 * fTemp23) + (0.010086667768959431 * fTemp46))) + ((fConst788 * fTemp141) + ((fConst765 * ((0.020173335537918863 * fTemp87) + (0.02017333553791888 * fTemp64))) + ((fConst789 * fTemp55) + ((fConst790 * fTemp93) + ((fConst791 * fTemp48) + ((fConst792 * fTemp91) + ((fConst39 * fTemp224) + ((fConst793 * fTemp107) + ((fConst794 * fTemp50) + (fTemp225 + fTemp328)))))))))))) - fTemp333));
			fRec283[0] = max((fRec283[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp338))))));
			fVbargraph83 = FAUSTFLOAT(fRec283[0]);
			output47[i] = FAUSTFLOAT(fTemp338);
			double fTemp339 = (fRec1[0] * (((0.02017333553791887 * (fTemp332 + ((((fTemp212 + ((((((fTemp229 + ((fTemp228 + (fConst781 * fTemp91)) + (fConst795 * fTemp71))) + (fConst783 * fTemp48)) + (fConst796 * fTemp19)) + (fConst797 * fTemp87)) + (fConst785 * fTemp93)) + (fConst798 * fTemp73))) + (fConst799 * fTemp46)) + (fConst787 * fTemp55)) + (fConst800 * fTemp25)))) + ((fConst757 * ((4.2232002659445085e-18 * fTemp68) + (0.0095098015720093357 * fTemp138))) + ((fConst763 * ((6.3348003989167635e-18 * fTemp21) + (0.014264702358014004 * fTemp105))) + ((fConst768 * ((1.2669600797833527e-17 * fTemp66) + (0.028529404716028009 * fTemp136))) + ((fConst801 * fTemp23) + ((fConst802 * fTemp141) + ((fConst803 * fTemp64) + (fTemp227 + ((fConst42 * fTemp230) + ((fConst804 * fTemp107) + ((fConst805 * fTemp27) + fTemp328))))))))))) - fTemp333));
			fRec284[0] = max((fRec284[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp339))))));
			fVbargraph84 = FAUSTFLOAT(fRec284[0]);
			output48[i] = FAUSTFLOAT(fTemp339);
			double fTemp340 = (fRec1[0] * ((((fConst806 * fTemp141) + (((fConst807 * fTemp73) + ((fConst808 * fTemp19) + ((fConst809 * fTemp71) + ((fConst810 * fTemp107) + (fTemp232 + ((fConst811 * fTemp32) + fTemp328)))))) + (fConst812 * fTemp25))) + (0.02017333553791887 * (fTemp237 + (fTemp332 + ((((fTemp337 + ((fTemp212 + ((((fTemp336 + ((fTemp236 + (((fTemp335 + (fTemp235 + (fTemp234 + (fTemp233 + (fConst795 * fTemp91))))) + (fConst796 * fTemp48)) + (fConst813 * fTemp66))) + (fConst814 * fTemp87))) + (fConst798 * fTemp93)) + (fConst815 * fTemp21)) + (fConst816 * fTemp64))) + (fConst817 * fTemp46))) + (fConst800 * fTemp55)) + (fConst818 * fTemp68)) + (fConst819 * fTemp23)))))) - fTemp333));
			fRec285[0] = max((fRec285[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp340))))));
			fVbargraph85 = FAUSTFLOAT(fRec285[0]);
			output49[i] = FAUSTFLOAT(fTemp340);
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
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec27[1] = fRec27[0];
			fRec30[1] = fRec30[0];
			fRec31[1] = fRec31[0];
			fRec26[1] = fRec26[0];
			fRec32[1] = fRec32[0];
			fRec25[1] = fRec25[0];
			fRec24[1] = fRec24[0];
			fRec0[1] = fRec0[0];
			fRec37[1] = fRec37[0];
			fRec38[1] = fRec38[0];
			fRec36[1] = fRec36[0];
			fRec39[1] = fRec39[0];
			fRec35[1] = fRec35[0];
			fRec34[1] = fRec34[0];
			fRec43[1] = fRec43[0];
			fRec44[1] = fRec44[0];
			fRec42[1] = fRec42[0];
			fRec45[1] = fRec45[0];
			fRec46[1] = fRec46[0];
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fRec51[1] = fRec51[0];
			fRec52[1] = fRec52[0];
			fRec50[1] = fRec50[0];
			fRec53[1] = fRec53[0];
			fRec54[1] = fRec54[0];
			fRec49[1] = fRec49[0];
			fRec55[1] = fRec55[0];
			fRec48[1] = fRec48[0];
			fRec47[1] = fRec47[0];
			fRec60[1] = fRec60[0];
			fRec61[1] = fRec61[0];
			fRec59[1] = fRec59[0];
			fRec62[1] = fRec62[0];
			fRec63[1] = fRec63[0];
			fRec58[1] = fRec58[0];
			fRec64[1] = fRec64[0];
			fRec57[1] = fRec57[0];
			fRec56[1] = fRec56[0];
			fRec68[1] = fRec68[0];
			fRec69[1] = fRec69[0];
			fRec67[1] = fRec67[0];
			fRec70[1] = fRec70[0];
			fRec66[1] = fRec66[0];
			fRec65[1] = fRec65[0];
			fRec73[1] = fRec73[0];
			fRec74[1] = fRec74[0];
			fRec72[1] = fRec72[0];
			fRec71[1] = fRec71[0];
			fRec77[1] = fRec77[0];
			fRec76[1] = fRec76[0];
			fRec75[1] = fRec75[0];
			fRec81[1] = fRec81[0];
			fRec82[1] = fRec82[0];
			fRec80[1] = fRec80[0];
			fRec83[1] = fRec83[0];
			fRec84[1] = fRec84[0];
			fRec79[1] = fRec79[0];
			fRec78[1] = fRec78[0];
			fRec89[1] = fRec89[0];
			fRec90[1] = fRec90[0];
			fRec88[1] = fRec88[0];
			fRec91[1] = fRec91[0];
			fRec92[1] = fRec92[0];
			fRec87[1] = fRec87[0];
			fRec93[1] = fRec93[0];
			fRec86[1] = fRec86[0];
			fRec85[1] = fRec85[0];
			fRec33[1] = fRec33[0];
			fRec98[1] = fRec98[0];
			fRec99[1] = fRec99[0];
			fRec97[1] = fRec97[0];
			fRec100[1] = fRec100[0];
			fRec96[1] = fRec96[0];
			fRec95[1] = fRec95[0];
			fRec105[1] = fRec105[0];
			fRec106[1] = fRec106[0];
			fRec104[1] = fRec104[0];
			fRec107[1] = fRec107[0];
			fRec108[1] = fRec108[0];
			fRec103[1] = fRec103[0];
			fRec109[1] = fRec109[0];
			fRec102[1] = fRec102[0];
			fRec101[1] = fRec101[0];
			fRec113[1] = fRec113[0];
			fRec114[1] = fRec114[0];
			fRec112[1] = fRec112[0];
			fRec115[1] = fRec115[0];
			fRec111[1] = fRec111[0];
			fRec110[1] = fRec110[0];
			fRec118[1] = fRec118[0];
			fRec117[1] = fRec117[0];
			fRec116[1] = fRec116[0];
			fRec123[1] = fRec123[0];
			fRec124[1] = fRec124[0];
			fRec122[1] = fRec122[0];
			fRec125[1] = fRec125[0];
			fRec126[1] = fRec126[0];
			fRec121[1] = fRec121[0];
			fRec127[1] = fRec127[0];
			fRec120[1] = fRec120[0];
			fRec119[1] = fRec119[0];
			fRec132[1] = fRec132[0];
			fRec133[1] = fRec133[0];
			fRec131[1] = fRec131[0];
			fRec134[1] = fRec134[0];
			fRec135[1] = fRec135[0];
			fRec130[1] = fRec130[0];
			fRec136[1] = fRec136[0];
			fRec129[1] = fRec129[0];
			fRec128[1] = fRec128[0];
			fRec94[1] = fRec94[0];
			fRec137[1] = fRec137[0];
			fRec138[1] = fRec138[0];
			fRec139[1] = fRec139[0];
			fRec145[1] = fRec145[0];
			fRec146[1] = fRec146[0];
			fRec144[1] = fRec144[0];
			fRec147[1] = fRec147[0];
			fRec148[1] = fRec148[0];
			fRec143[1] = fRec143[0];
			fRec149[1] = fRec149[0];
			fRec142[1] = fRec142[0];
			fRec141[1] = fRec141[0];
			fRec153[1] = fRec153[0];
			fRec154[1] = fRec154[0];
			fRec152[1] = fRec152[0];
			fRec155[1] = fRec155[0];
			fRec156[1] = fRec156[0];
			fRec151[1] = fRec151[0];
			fRec150[1] = fRec150[0];
			fRec160[1] = fRec160[0];
			fRec161[1] = fRec161[0];
			fRec159[1] = fRec159[0];
			fRec162[1] = fRec162[0];
			fRec158[1] = fRec158[0];
			fRec157[1] = fRec157[0];
			fRec167[1] = fRec167[0];
			fRec168[1] = fRec168[0];
			fRec166[1] = fRec166[0];
			fRec169[1] = fRec169[0];
			fRec170[1] = fRec170[0];
			fRec165[1] = fRec165[0];
			fRec171[1] = fRec171[0];
			fRec164[1] = fRec164[0];
			fRec163[1] = fRec163[0];
			fRec174[1] = fRec174[0];
			fRec175[1] = fRec175[0];
			fRec173[1] = fRec173[0];
			fRec172[1] = fRec172[0];
			fRec179[1] = fRec179[0];
			fRec180[1] = fRec180[0];
			fRec178[1] = fRec178[0];
			fRec181[1] = fRec181[0];
			fRec182[1] = fRec182[0];
			fRec177[1] = fRec177[0];
			fRec176[1] = fRec176[0];
			fRec140[1] = fRec140[0];
			fRec187[1] = fRec187[0];
			fRec188[1] = fRec188[0];
			fRec186[1] = fRec186[0];
			fRec189[1] = fRec189[0];
			fRec190[1] = fRec190[0];
			fRec185[1] = fRec185[0];
			fRec184[1] = fRec184[0];
			fRec193[1] = fRec193[0];
			fRec194[1] = fRec194[0];
			fRec192[1] = fRec192[0];
			fRec191[1] = fRec191[0];
			fRec198[1] = fRec198[0];
			fRec199[1] = fRec199[0];
			fRec197[1] = fRec197[0];
			fRec200[1] = fRec200[0];
			fRec201[1] = fRec201[0];
			fRec196[1] = fRec196[0];
			fRec195[1] = fRec195[0];
			fRec183[1] = fRec183[0];
			fRec202[1] = fRec202[0];
			fRec203[1] = fRec203[0];
			fRec208[1] = fRec208[0];
			fRec209[1] = fRec209[0];
			fRec207[1] = fRec207[0];
			fRec210[1] = fRec210[0];
			fRec211[1] = fRec211[0];
			fRec206[1] = fRec206[0];
			fRec205[1] = fRec205[0];
			fRec215[1] = fRec215[0];
			fRec216[1] = fRec216[0];
			fRec214[1] = fRec214[0];
			fRec217[1] = fRec217[0];
			fRec218[1] = fRec218[0];
			fRec213[1] = fRec213[0];
			fRec212[1] = fRec212[0];
			fRec221[1] = fRec221[0];
			fRec222[1] = fRec222[0];
			fRec220[1] = fRec220[0];
			fRec219[1] = fRec219[0];
			fRec204[1] = fRec204[0];
			fRec223[1] = fRec223[0];
			fRec224[1] = fRec224[0];
			fRec225[1] = fRec225[0];
			fRec226[1] = fRec226[0];
			fRec227[1] = fRec227[0];
			fRec228[1] = fRec228[0];
			fRec229[1] = fRec229[0];
			fRec234[1] = fRec234[0];
			fRec235[1] = fRec235[0];
			fRec233[1] = fRec233[0];
			fRec236[1] = fRec236[0];
			fRec232[1] = fRec232[0];
			fRec231[1] = fRec231[0];
			fRec241[1] = fRec241[0];
			fRec242[1] = fRec242[0];
			fRec240[1] = fRec240[0];
			fRec243[1] = fRec243[0];
			fRec244[1] = fRec244[0];
			fRec239[1] = fRec239[0];
			fRec245[1] = fRec245[0];
			fRec238[1] = fRec238[0];
			fRec237[1] = fRec237[0];
			fRec250[1] = fRec250[0];
			fRec251[1] = fRec251[0];
			fRec249[1] = fRec249[0];
			fRec252[1] = fRec252[0];
			fRec253[1] = fRec253[0];
			fRec248[1] = fRec248[0];
			fRec254[1] = fRec254[0];
			fRec247[1] = fRec247[0];
			fRec246[1] = fRec246[0];
			fRec230[1] = fRec230[0];
			fRec255[1] = fRec255[0];
			fRec256[1] = fRec256[0];
			fRec257[1] = fRec257[0];
			fRec258[1] = fRec258[0];
			fRec259[1] = fRec259[0];
			fRec260[1] = fRec260[0];
			fRec261[1] = fRec261[0];
			fRec262[1] = fRec262[0];
			fRec263[1] = fRec263[0];
			fRec264[1] = fRec264[0];
			fRec265[1] = fRec265[0];
			fRec266[1] = fRec266[0];
			fRec267[1] = fRec267[0];
			fRec268[1] = fRec268[0];
			fRec269[1] = fRec269[0];
			fRec270[1] = fRec270[0];
			fRec271[1] = fRec271[0];
			fRec272[1] = fRec272[0];
			fRec273[1] = fRec273[0];
			fRec274[1] = fRec274[0];
			fRec275[1] = fRec275[0];
			fRec276[1] = fRec276[0];
			fRec277[1] = fRec277[0];
			fRec278[1] = fRec278[0];
			fRec279[1] = fRec279[0];
			fRec280[1] = fRec280[0];
			fRec281[1] = fRec281[0];
			fRec282[1] = fRec282[0];
			fRec283[1] = fRec283[0];
			fRec284[1] = fRec284[0];
			fRec285[1] = fRec285[0];
			
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
