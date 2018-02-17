/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAPanLebedev502"
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
	FAUSTFLOAT fCheckbox0;
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fEntry0;
	FAUSTFLOAT fCheckbox1;
	FAUSTFLOAT fHslider1;
	double fRec1[2];
	FAUSTFLOAT fCheckbox2;
	FAUSTFLOAT fHslider2;
	double fConst2;
	double fConst3;
	double fRec4[2];
	double fRec3[2];
	double fRec2[2];
	double fRec6[2];
	double fRec5[2];
	FAUSTFLOAT fCheckbox3;
	double fConst4;
	double fConst5;
	double fRec9[2];
	double fRec10[2];
	double fRec8[2];
	double fRec7[2];
	double fVec0[2];
	double fRec11[2];
	double fRec13[2];
	double fRec14[2];
	double fRec12[2];
	double fRec0[2];
	FAUSTFLOAT fVbargraph0;
	FAUSTFLOAT fHslider3;
	double fRec15[2];
	FAUSTFLOAT fVbargraph1;
	double fRec16[2];
	FAUSTFLOAT fVbargraph2;
	double fRec17[2];
	FAUSTFLOAT fVbargraph3;
	double fRec18[2];
	FAUSTFLOAT fVbargraph4;
	double fRec19[2];
	FAUSTFLOAT fVbargraph5;
	double fRec20[2];
	FAUSTFLOAT fVbargraph6;
	double fRec21[2];
	FAUSTFLOAT fVbargraph7;
	double fRec22[2];
	FAUSTFLOAT fVbargraph8;
	double fRec23[2];
	FAUSTFLOAT fVbargraph9;
	double fRec24[2];
	FAUSTFLOAT fVbargraph10;
	double fRec25[2];
	FAUSTFLOAT fVbargraph11;
	double fRec26[2];
	FAUSTFLOAT fVbargraph12;
	double fRec27[2];
	FAUSTFLOAT fVbargraph13;
	double fRec28[2];
	FAUSTFLOAT fVbargraph14;
	double fRec29[2];
	FAUSTFLOAT fVbargraph15;
	double fRec30[2];
	FAUSTFLOAT fVbargraph16;
	double fRec31[2];
	FAUSTFLOAT fVbargraph17;
	double fRec32[2];
	FAUSTFLOAT fVbargraph18;
	double fRec33[2];
	FAUSTFLOAT fVbargraph19;
	double fRec34[2];
	FAUSTFLOAT fVbargraph20;
	double fRec35[2];
	FAUSTFLOAT fVbargraph21;
	double fRec36[2];
	FAUSTFLOAT fVbargraph22;
	double fRec37[2];
	FAUSTFLOAT fVbargraph23;
	double fRec38[2];
	FAUSTFLOAT fVbargraph24;
	double fRec39[2];
	FAUSTFLOAT fVbargraph25;
	double fRec40[2];
	FAUSTFLOAT fVbargraph26;
	double fRec41[2];
	FAUSTFLOAT fVbargraph27;
	double fRec42[2];
	FAUSTFLOAT fVbargraph28;
	double fRec43[2];
	FAUSTFLOAT fVbargraph29;
	double fRec44[2];
	FAUSTFLOAT fVbargraph30;
	double fRec45[2];
	FAUSTFLOAT fVbargraph31;
	double fRec46[2];
	FAUSTFLOAT fVbargraph32;
	double fRec47[2];
	FAUSTFLOAT fVbargraph33;
	double fRec48[2];
	FAUSTFLOAT fVbargraph34;
	double fRec49[2];
	FAUSTFLOAT fVbargraph35;
	double fRec50[2];
	FAUSTFLOAT fVbargraph36;
	double fRec51[2];
	FAUSTFLOAT fVbargraph37;
	double fRec52[2];
	FAUSTFLOAT fVbargraph38;
	double fRec53[2];
	FAUSTFLOAT fVbargraph39;
	double fRec54[2];
	FAUSTFLOAT fVbargraph40;
	double fRec55[2];
	FAUSTFLOAT fVbargraph41;
	double fRec56[2];
	FAUSTFLOAT fVbargraph42;
	double fRec57[2];
	FAUSTFLOAT fVbargraph43;
	double fRec58[2];
	FAUSTFLOAT fVbargraph44;
	double fRec59[2];
	FAUSTFLOAT fVbargraph45;
	double fRec60[2];
	FAUSTFLOAT fVbargraph46;
	double fRec61[2];
	FAUSTFLOAT fVbargraph47;
	double fRec62[2];
	FAUSTFLOAT fVbargraph48;
	double fRec63[2];
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
		m->declare("name", "HOAPanLebedev502");
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
		fConst4 = (510.0 / fConst0);
		fConst5 = (1020.0 / fConst0);
		
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = FAUSTFLOAT(0.0);
		fHslider0 = FAUSTFLOAT(2.0);
		fEntry0 = FAUSTFLOAT(1.0700000000000001);
		fCheckbox1 = FAUSTFLOAT(0.0);
		fHslider1 = FAUSTFLOAT(0.0);
		fCheckbox2 = FAUSTFLOAT(0.0);
		fHslider2 = FAUSTFLOAT(0.0);
		fCheckbox3 = FAUSTFLOAT(0.0);
		fHslider3 = FAUSTFLOAT(0.0);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec1[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec4[l1] = 0.0;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec3[l2] = 0.0;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec2[l3] = 0.0;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec6[l4] = 0.0;
			
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec5[l5] = 0.0;
			
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
			fVec0[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec11[l11] = 0.0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec13[l12] = 0.0;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec14[l13] = 0.0;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec12[l14] = 0.0;
			
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec0[l15] = 0.0;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fRec15[l16] = 0.0;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec16[l17] = 0.0;
			
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec17[l18] = 0.0;
			
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec18[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec19[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec20[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec21[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec22[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec23[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec24[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec25[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec26[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec27[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec28[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec29[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec30[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec31[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec32[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec33[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec34[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec35[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec36[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec37[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec38[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec39[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec40[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec41[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec42[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec43[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec44[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec45[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec46[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec47[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec48[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec49[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec50[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec51[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec52[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec53[l54] = 0.0;
			
		}
		for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
			fRec54[l55] = 0.0;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fRec55[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec56[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec57[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec58[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec59[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec60[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fRec61[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec62[l63] = 0.0;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec63[l64] = 0.0;
			
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
		ui_interface->openVerticalBox("HOAPanLebedev502");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fHslider1, "0+1", "");
		ui_interface->declare(&fHslider1, "osc", "/gain_0 -20 20");
		ui_interface->declare(&fHslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  0", &fHslider1, 0.0, -30.0, 20.0, 0.10000000000000001);
		ui_interface->declare(&fHslider0, "0+2", "");
		ui_interface->declare(&fHslider0, "osc", "/radius_0 0.5 50");
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  0", &fHslider0, 2.0, 0.5, 50.0, 0.01);
		ui_interface->declare(&fHslider3, "0+3", "");
		ui_interface->declare(&fHslider3, "osc", "/azimuth_0 0 360");
		ui_interface->declare(&fHslider3, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth  0", &fHslider3, 0.0, -3.1415926535897931, 3.1415926535897931, 0.10000000000000001);
		ui_interface->declare(&fHslider2, "0+4", "");
		ui_interface->declare(&fHslider2, "osc", "/elevation_0 -90 90");
		ui_interface->declare(&fHslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation  0", &fHslider2, 0.0, -1.5707963267948966, 1.5707963267948966, 0.10000000000000001);
		ui_interface->declare(0, "0+5", "");
		ui_interface->openHorizontalBox("Spherical Wave");
		ui_interface->addCheckButton("Yes", &fCheckbox0);
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openVerticalBox("Mute Order");
		ui_interface->addCheckButton("0", &fCheckbox1);
		ui_interface->addCheckButton("1", &fCheckbox2);
		ui_interface->addCheckButton("2", &fCheckbox3);
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
		ui_interface->addVerticalBargraph("0x2a8dfc0", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "osc", "/output2 -70 6");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a97b70", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph2, "osc", "/output3 -70 6");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2a9f960", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph3, "osc", "/output4 -70 6");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2aa7830", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph4, "osc", "/output5 -70 6");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2aaf7e0", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph5, "osc", "/output6 -70 6");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ab7630", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph6, "osc", "/output7 -70 6");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2abfb10", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph7, "osc", "/output8 -70 6");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ac7950", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph8, "osc", "/output9 -70 6");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2acf870", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph9, "osc", "/output10 -70 6");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ad7870", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph10, "osc", "/output11 -70 6");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2adfd60", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph11, "osc", "/output12 -70 6");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ae8330", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph12, "osc", "/output13 -70 6");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2af09e0", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph13, "osc", "/output14 -70 6");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2af9170", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph14, "osc", "/output15 -70 6");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b01000", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph15, "osc", "/output16 -70 6");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b08f70", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph16, "osc", "/output17 -70 6");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b10fc0", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph17, "osc", "/output18 -70 6");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b190f0", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph18, "osc", "/output19 -70 6");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b22230", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph19, "osc", "/output20 -70 6");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b2ad00", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph20, "osc", "/output21 -70 6");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b338b0", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph21, "osc", "/output22 -70 6");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b3c540", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph22, "osc", "/output23 -70 6");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b44ad0", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph23, "osc", "/output24 -70 6");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b4d140", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph24, "osc", "/output25 -70 6");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b55890", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph25, "osc", "/output26 -70 6");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b5e0c0", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph26, "osc", "/output27 -70 6");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b66cd0", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph27, "osc", "/output28 -70 6");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b6e5b0", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph28, "osc", "/output29 -70 6");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b75f70", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph29, "osc", "/output30 -70 6");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b7da10", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph30, "osc", "/output31 -70 6");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b866f0", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph31, "osc", "/output32 -70 6");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b8ed20", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph32, "osc", "/output33 -70 6");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b97430", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph33, "osc", "/output34 -70 6");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2b9fc20", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph34, "osc", "/output35 -70 6");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ba84f0", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph35, "osc", "/output36 -70 6");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bb0ea0", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph36, "osc", "/output37 -70 6");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bb9930", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph37, "osc", "/output38 -70 6");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bc24a0", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph38, "osc", "/output39 -70 6");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bca330", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph39, "osc", "/output40 -70 6");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bd22a0", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph40, "osc", "/output41 -70 6");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bda2f0", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph41, "osc", "/output42 -70 6");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2be2420", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph42, "osc", "/output43 -70 6");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bea630", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph43, "osc", "/output44 -70 6");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bf2920", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph44, "osc", "/output45 -70 6");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2bfacf0", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph45, "osc", "/output46 -70 6");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c031a0", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph46, "osc", "/output47 -70 6");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c0b730", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph47, "osc", "/output48 -70 6");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c13da0", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph48, "osc", "/output49 -70 6");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c1c4f0", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph49, "osc", "/output50 -70 6");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2c24d20", &fVbargraph49, -70.0, 6.0);
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
		double fSlow0 = double(fCheckbox0);
		double fSlow1 = double(fHslider0);
		double fSlow2 = (1.0 - fSlow0);
		double fSlow3 = double(fEntry0);
		double fSlow4 = ((((fSlow0 / fSlow1) + fSlow2) * fSlow3) * (1.0 - double(fCheckbox1)));
		double fSlow5 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fHslider1))));
		double fSlow6 = (1.0 - double(fCheckbox2));
		double fSlow7 = double(fHslider2);
		double fSlow8 = sin(fSlow7);
		double fSlow9 = (3.0 * (fSlow6 * fSlow8));
		double fSlow10 = (fConst3 / fSlow1);
		double fSlow11 = (fSlow10 + 1.0);
		double fSlow12 = (fConst2 / (fSlow11 * fSlow1));
		double fSlow13 = (fConst3 / fSlow3);
		double fSlow14 = (1.0 / (fSlow13 + 1.0));
		double fSlow15 = (((fSlow11 * fSlow0) * fSlow3) / fSlow1);
		double fSlow16 = (fConst2 / fSlow3);
		double fSlow17 = (fSlow2 * fSlow3);
		double fSlow18 = (1.0 - double(fCheckbox3));
		double fSlow19 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow8)) + -1.0) * fSlow18));
		double fSlow20 = mydsp_faustpower2_f(fSlow10);
		double fSlow21 = (((fConst4 / fSlow1) + (3.0 * fSlow20)) + 1.0);
		double fSlow22 = (1.0 / fSlow21);
		double fSlow23 = (12.0 * fSlow20);
		double fSlow24 = ((fConst5 / fSlow1) + fSlow23);
		double fSlow25 = mydsp_faustpower2_f(fSlow13);
		double fSlow26 = (1.0 / (((fConst4 / fSlow3) + (3.0 * fSlow25)) + 1.0));
		double fSlow27 = (((fSlow21 * fSlow0) * fSlow3) / fSlow1);
		double fSlow28 = (12.0 * fSlow25);
		double fSlow29 = (fSlow28 + (fConst5 / fSlow3));
		double fSlow30 = double(fHslider3);
		double fSlow31 = cos(fSlow30);
		double fSlow32 = cos(fSlow7);
		double fSlow33 = (3.0 * ((fSlow31 * fSlow6) * fSlow32));
		double fSlow34 = mydsp_faustpower2_f(fSlow32);
		double fSlow35 = (2.5 * (((3.0 * (mydsp_faustpower2_f(fSlow31) * fSlow34)) + -1.0) * fSlow18));
		double fSlow36 = cos((fSlow30 + -1.5707963267948966));
		double fSlow37 = (3.0 * ((fSlow36 * fSlow6) * fSlow32));
		double fSlow38 = (2.5 * (((3.0 * (mydsp_faustpower2_f(fSlow36) * fSlow34)) + -1.0) * fSlow18));
		double fSlow39 = cos((fSlow30 + -3.1415926535897931));
		double fSlow40 = (3.0 * ((fSlow39 * fSlow6) * fSlow32));
		double fSlow41 = (2.5 * (((3.0 * (mydsp_faustpower2_f(fSlow39) * fSlow34)) + -1.0) * fSlow18));
		double fSlow42 = cos((fSlow30 + -4.7123889803846897));
		double fSlow43 = (3.0 * ((fSlow42 * fSlow6) * fSlow32));
		double fSlow44 = (2.5 * (((3.0 * (mydsp_faustpower2_f(fSlow42) * fSlow34)) + -1.0) * fSlow18));
		double fSlow45 = (fSlow31 * fSlow32);
		double fSlow46 = ((6.123233995736766e-17 * fSlow45) - fSlow8);
		double fSlow47 = (3.0 * (fSlow46 * fSlow6));
		double fSlow48 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow46)) + -1.0) * fSlow18));
		double fSlow49 = (0.70710678118654757 * fSlow45);
		double fSlow50 = (0.70710678118654746 * fSlow8);
		double fSlow51 = (fSlow49 + fSlow50);
		double fSlow52 = (3.0 * (fSlow51 * fSlow6));
		double fSlow53 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow51)) + -1.0) * fSlow18));
		double fSlow54 = (0.70710678118654757 * (fSlow36 * fSlow32));
		double fSlow55 = (fSlow50 + fSlow54);
		double fSlow56 = (3.0 * (fSlow55 * fSlow6));
		double fSlow57 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow55)) + -1.0) * fSlow18));
		double fSlow58 = (0.70710678118654757 * (fSlow39 * fSlow32));
		double fSlow59 = (fSlow50 + fSlow58);
		double fSlow60 = (3.0 * (fSlow59 * fSlow6));
		double fSlow61 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow59)) + -1.0) * fSlow18));
		double fSlow62 = (0.70710678118654757 * (fSlow42 * fSlow32));
		double fSlow63 = (fSlow50 + fSlow62);
		double fSlow64 = (3.0 * (fSlow63 * fSlow6));
		double fSlow65 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow63)) + -1.0) * fSlow18));
		double fSlow66 = cos((fSlow30 + -0.78539816339744828));
		double fSlow67 = (3.0 * ((fSlow66 * fSlow6) * fSlow32));
		double fSlow68 = (2.5 * (((3.0 * (mydsp_faustpower2_f(fSlow66) * fSlow34)) + -1.0) * fSlow18));
		double fSlow69 = cos((fSlow30 + -2.3561944901923448));
		double fSlow70 = (3.0 * ((fSlow69 * fSlow6) * fSlow32));
		double fSlow71 = (2.5 * (((3.0 * (mydsp_faustpower2_f(fSlow69) * fSlow34)) + -1.0) * fSlow18));
		double fSlow72 = cos((fSlow30 + -3.9269908169872414));
		double fSlow73 = (3.0 * ((fSlow72 * fSlow6) * fSlow32));
		double fSlow74 = (2.5 * (((3.0 * (mydsp_faustpower2_f(fSlow72) * fSlow34)) + -1.0) * fSlow18));
		double fSlow75 = cos((fSlow30 + -5.497787143782138));
		double fSlow76 = (3.0 * ((fSlow75 * fSlow6) * fSlow32));
		double fSlow77 = (2.5 * (((3.0 * (mydsp_faustpower2_f(fSlow75) * fSlow34)) + -1.0) * fSlow18));
		double fSlow78 = (fSlow49 - fSlow50);
		double fSlow79 = (3.0 * (fSlow78 * fSlow6));
		double fSlow80 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow78)) + -1.0) * fSlow18));
		double fSlow81 = (fSlow54 - fSlow50);
		double fSlow82 = (3.0 * (fSlow81 * fSlow6));
		double fSlow83 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow81)) + -1.0) * fSlow18));
		double fSlow84 = (fSlow58 - fSlow50);
		double fSlow85 = (3.0 * (fSlow84 * fSlow6));
		double fSlow86 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow84)) + -1.0) * fSlow18));
		double fSlow87 = (fSlow62 - fSlow50);
		double fSlow88 = (3.0 * (fSlow87 * fSlow6));
		double fSlow89 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow87)) + -1.0) * fSlow18));
		double fSlow90 = (0.81649658092772592 * (fSlow66 * fSlow32));
		double fSlow91 = (0.57735026918962584 * fSlow8);
		double fSlow92 = (fSlow90 + fSlow91);
		double fSlow93 = (3.0 * (fSlow92 * fSlow6));
		double fSlow94 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow92)) + -1.0) * fSlow18));
		double fSlow95 = (fSlow69 * fSlow32);
		double fSlow96 = (0.81649658092772592 * fSlow95);
		double fSlow97 = (fSlow91 + fSlow96);
		double fSlow98 = (3.0 * (fSlow97 * fSlow6));
		double fSlow99 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow97)) + -1.0) * fSlow18));
		double fSlow100 = (fSlow72 * fSlow32);
		double fSlow101 = (0.81649658092772592 * fSlow100);
		double fSlow102 = (fSlow91 + fSlow101);
		double fSlow103 = (3.0 * (fSlow102 * fSlow6));
		double fSlow104 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow102)) + -1.0) * fSlow18));
		double fSlow105 = (fSlow75 * fSlow32);
		double fSlow106 = (0.81649658092772592 * fSlow105);
		double fSlow107 = (fSlow91 + fSlow106);
		double fSlow108 = (3.0 * (fSlow107 * fSlow6));
		double fSlow109 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow107)) + -1.0) * fSlow18));
		double fSlow110 = (fSlow90 - fSlow91);
		double fSlow111 = (3.0 * (fSlow110 * fSlow6));
		double fSlow112 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow110)) + -1.0) * fSlow18));
		double fSlow113 = (fSlow96 - fSlow91);
		double fSlow114 = (3.0 * (fSlow113 * fSlow6));
		double fSlow115 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow113)) + -1.0) * fSlow18));
		double fSlow116 = (fSlow101 - fSlow91);
		double fSlow117 = (3.0 * (fSlow116 * fSlow6));
		double fSlow118 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow116)) + -1.0) * fSlow18));
		double fSlow119 = (fSlow106 - fSlow91);
		double fSlow120 = (3.0 * (fSlow119 * fSlow6));
		double fSlow121 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow119)) + -1.0) * fSlow18));
		double fSlow122 = (0.42640143271122027 * (cos((fSlow30 + -0.78539816339744839)) * fSlow32));
		double fSlow123 = (0.90453403373329111 * fSlow8);
		double fSlow124 = (fSlow122 + fSlow123);
		double fSlow125 = (3.0 * (fSlow124 * fSlow6));
		double fSlow126 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow124)) + -1.0) * fSlow18));
		double fSlow127 = (0.42640143271122027 * fSlow95);
		double fSlow128 = (fSlow123 + fSlow127);
		double fSlow129 = (3.0 * (fSlow128 * fSlow6));
		double fSlow130 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow128)) + -1.0) * fSlow18));
		double fSlow131 = (0.42640143271122027 * fSlow100);
		double fSlow132 = (fSlow123 + fSlow131);
		double fSlow133 = (3.0 * (fSlow132 * fSlow6));
		double fSlow134 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow132)) + -1.0) * fSlow18));
		double fSlow135 = (0.42640143271122027 * fSlow105);
		double fSlow136 = (fSlow123 + fSlow135);
		double fSlow137 = (3.0 * (fSlow136 * fSlow6));
		double fSlow138 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow136)) + -1.0) * fSlow18));
		double fSlow139 = (0.95346258924559235 * (cos((fSlow30 + -0.32175055439664263)) * fSlow32));
		double fSlow140 = (0.30151134457776352 * fSlow8);
		double fSlow141 = (fSlow139 + fSlow140);
		double fSlow142 = (3.0 * (fSlow141 * fSlow6));
		double fSlow143 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow141)) + -1.0) * fSlow18));
		double fSlow144 = (0.95346258924559235 * (cos((fSlow30 + -1.2490457723982544)) * fSlow32));
		double fSlow145 = (fSlow140 + fSlow144);
		double fSlow146 = (3.0 * (fSlow145 * fSlow6));
		double fSlow147 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow145)) + -1.0) * fSlow18));
		double fSlow148 = (0.95346258924559235 * (cos((fSlow30 + -1.8925468811915387)) * fSlow32));
		double fSlow149 = (fSlow140 + fSlow148);
		double fSlow150 = (3.0 * (fSlow149 * fSlow6));
		double fSlow151 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow149)) + -1.0) * fSlow18));
		double fSlow152 = (0.95346258924559235 * (cos((fSlow30 + -2.8198420991931505)) * fSlow32));
		double fSlow153 = (fSlow140 + fSlow152);
		double fSlow154 = (3.0 * (fSlow153 * fSlow6));
		double fSlow155 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow153)) + -1.0) * fSlow18));
		double fSlow156 = (0.95346258924559235 * (cos((fSlow30 + -3.4633432079864357)) * fSlow32));
		double fSlow157 = (fSlow140 + fSlow156);
		double fSlow158 = (3.0 * (fSlow157 * fSlow6));
		double fSlow159 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow157)) + -1.0) * fSlow18));
		double fSlow160 = (0.95346258924559235 * (cos((fSlow30 + -4.3906384259880475)) * fSlow32));
		double fSlow161 = (fSlow140 + fSlow160);
		double fSlow162 = (3.0 * (fSlow161 * fSlow6));
		double fSlow163 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow161)) + -1.0) * fSlow18));
		double fSlow164 = (0.95346258924559235 * (cos((fSlow30 + -5.0341395347813318)) * fSlow32));
		double fSlow165 = (fSlow140 + fSlow164);
		double fSlow166 = (3.0 * (fSlow165 * fSlow6));
		double fSlow167 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow165)) + -1.0) * fSlow18));
		double fSlow168 = (0.95346258924559235 * (cos((fSlow30 + -5.9614347527829432)) * fSlow32));
		double fSlow169 = (fSlow140 + fSlow168);
		double fSlow170 = (3.0 * (fSlow169 * fSlow6));
		double fSlow171 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow169)) + -1.0) * fSlow18));
		double fSlow172 = (fSlow139 - fSlow140);
		double fSlow173 = (3.0 * (fSlow172 * fSlow6));
		double fSlow174 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow172)) + -1.0) * fSlow18));
		double fSlow175 = (fSlow144 - fSlow140);
		double fSlow176 = (3.0 * (fSlow175 * fSlow6));
		double fSlow177 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow175)) + -1.0) * fSlow18));
		double fSlow178 = (fSlow148 - fSlow140);
		double fSlow179 = (3.0 * (fSlow178 * fSlow6));
		double fSlow180 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow178)) + -1.0) * fSlow18));
		double fSlow181 = (fSlow152 - fSlow140);
		double fSlow182 = (3.0 * (fSlow181 * fSlow6));
		double fSlow183 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow181)) + -1.0) * fSlow18));
		double fSlow184 = (fSlow156 - fSlow140);
		double fSlow185 = (3.0 * (fSlow184 * fSlow6));
		double fSlow186 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow184)) + -1.0) * fSlow18));
		double fSlow187 = (fSlow160 - fSlow140);
		double fSlow188 = (3.0 * (fSlow187 * fSlow6));
		double fSlow189 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow187)) + -1.0) * fSlow18));
		double fSlow190 = (fSlow164 - fSlow140);
		double fSlow191 = (3.0 * (fSlow190 * fSlow6));
		double fSlow192 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow190)) + -1.0) * fSlow18));
		double fSlow193 = (fSlow168 - fSlow140);
		double fSlow194 = (3.0 * (fSlow193 * fSlow6));
		double fSlow195 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow193)) + -1.0) * fSlow18));
		double fSlow196 = (fSlow122 - fSlow123);
		double fSlow197 = (3.0 * (fSlow196 * fSlow6));
		double fSlow198 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow196)) + -1.0) * fSlow18));
		double fSlow199 = (fSlow127 - fSlow123);
		double fSlow200 = (3.0 * (fSlow199 * fSlow6));
		double fSlow201 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow199)) + -1.0) * fSlow18));
		double fSlow202 = (fSlow131 - fSlow123);
		double fSlow203 = (3.0 * (fSlow202 * fSlow6));
		double fSlow204 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow202)) + -1.0) * fSlow18));
		double fSlow205 = (fSlow135 - fSlow123);
		double fSlow206 = (3.0 * (fSlow205 * fSlow6));
		double fSlow207 = (2.5 * (((3.0 * mydsp_faustpower2_f(fSlow205)) + -1.0) * fSlow18));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fSlow5 + (0.999 * fRec1[1]));
			double fTemp0 = (fRec1[0] * double(input0[i]));
			double fTemp1 = (fSlow4 * fTemp0);
			fRec4[0] = (fRec4[1] + fRec3[1]);
			fRec3[0] = (fSlow14 * ((fSlow15 * fTemp0) - (fSlow16 * fRec4[0])));
			fRec2[0] = (fRec3[0] + fRec2[1]);
			double fTemp2 = (fSlow17 * fTemp0);
			fRec6[0] = (fRec6[1] + fRec5[1]);
			fRec5[0] = (fSlow14 * (fTemp2 - (fSlow16 * fRec6[0])));
			double fTemp3 = ((fSlow12 * (fRec2[0] - fRec3[0])) + (fRec3[0] + fRec5[0]));
			fRec9[0] = (fRec9[1] + fRec8[1]);
			fRec10[0] = (fRec10[1] + fRec9[1]);
			fRec8[0] = (fSlow26 * ((fSlow27 * fTemp0) - ((fSlow29 * fRec9[0]) + (fSlow28 * fRec10[0]))));
			fRec7[0] = (fRec8[0] + fRec7[1]);
			double fTemp4 = (fRec7[0] - fRec8[0]);
			fVec0[0] = fTemp4;
			fRec11[0] = (fRec11[1] + fVec0[1]);
			fRec13[0] = (fRec13[1] + fRec12[1]);
			fRec14[0] = (fRec14[1] + fRec13[1]);
			fRec12[0] = (fSlow26 * (fTemp2 - ((fSlow29 * fRec13[0]) + (fSlow28 * fRec14[0]))));
			double fTemp5 = ((fSlow22 * ((fSlow24 * fTemp4) + (fSlow23 * fRec11[0]))) + (fRec8[0] + fRec12[0]));
			double fTemp6 = (0.012698412698412698 * ((fTemp1 + (fSlow9 * fTemp3)) + (fSlow19 * fTemp5)));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fVbargraph0 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp6);
			double fTemp7 = (0.012698412698412698 * ((fTemp1 + (fSlow33 * fTemp3)) + (fSlow35 * fTemp5)));
			fRec15[0] = max((fRec15[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp7))))));
			fVbargraph1 = FAUSTFLOAT(fRec15[0]);
			output1[i] = FAUSTFLOAT(fTemp7);
			double fTemp8 = (0.012698412698412698 * ((fTemp1 + (fSlow37 * fTemp3)) + (fSlow38 * fTemp5)));
			fRec16[0] = max((fRec16[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fVbargraph2 = FAUSTFLOAT(fRec16[0]);
			output2[i] = FAUSTFLOAT(fTemp8);
			double fTemp9 = (0.012698412698412698 * ((fTemp1 + (fSlow40 * fTemp3)) + (fSlow41 * fTemp5)));
			fRec17[0] = max((fRec17[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph3 = FAUSTFLOAT(fRec17[0]);
			output3[i] = FAUSTFLOAT(fTemp9);
			double fTemp10 = (0.012698412698412698 * ((fTemp1 + (fSlow43 * fTemp3)) + (fSlow44 * fTemp5)));
			fRec18[0] = max((fRec18[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fVbargraph4 = FAUSTFLOAT(fRec18[0]);
			output4[i] = FAUSTFLOAT(fTemp10);
			double fTemp11 = (0.012698412698412698 * ((fTemp1 + (fSlow47 * fTemp3)) + (fSlow48 * fTemp5)));
			fRec19[0] = max((fRec19[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fVbargraph5 = FAUSTFLOAT(fRec19[0]);
			output5[i] = FAUSTFLOAT(fTemp11);
			double fTemp12 = (0.022574955908289243 * ((fTemp1 + (fSlow52 * fTemp3)) + (fSlow53 * fTemp5)));
			fRec20[0] = max((fRec20[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph6 = FAUSTFLOAT(fRec20[0]);
			output6[i] = FAUSTFLOAT(fTemp12);
			double fTemp13 = (0.022574955908289243 * ((fTemp1 + (fSlow56 * fTemp3)) + (fSlow57 * fTemp5)));
			fRec21[0] = max((fRec21[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fVbargraph7 = FAUSTFLOAT(fRec21[0]);
			output7[i] = FAUSTFLOAT(fTemp13);
			double fTemp14 = (0.022574955908289243 * ((fTemp1 + (fSlow60 * fTemp3)) + (fSlow61 * fTemp5)));
			fRec22[0] = max((fRec22[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fVbargraph8 = FAUSTFLOAT(fRec22[0]);
			output8[i] = FAUSTFLOAT(fTemp14);
			double fTemp15 = (0.022574955908289243 * ((fTemp1 + (fSlow64 * fTemp3)) + (fSlow65 * fTemp5)));
			fRec23[0] = max((fRec23[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph9 = FAUSTFLOAT(fRec23[0]);
			output9[i] = FAUSTFLOAT(fTemp15);
			double fTemp16 = (0.022574955908289243 * ((fTemp1 + (fSlow67 * fTemp3)) + (fSlow68 * fTemp5)));
			fRec24[0] = max((fRec24[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph10 = FAUSTFLOAT(fRec24[0]);
			output10[i] = FAUSTFLOAT(fTemp16);
			double fTemp17 = (0.022574955908289243 * ((fTemp1 + (fSlow70 * fTemp3)) + (fSlow71 * fTemp5)));
			fRec25[0] = max((fRec25[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph11 = FAUSTFLOAT(fRec25[0]);
			output11[i] = FAUSTFLOAT(fTemp17);
			double fTemp18 = (0.022574955908289243 * ((fTemp1 + (fSlow73 * fTemp3)) + (fSlow74 * fTemp5)));
			fRec26[0] = max((fRec26[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph12 = FAUSTFLOAT(fRec26[0]);
			output12[i] = FAUSTFLOAT(fTemp18);
			double fTemp19 = (0.022574955908289243 * ((fTemp1 + (fSlow76 * fTemp3)) + (fSlow77 * fTemp5)));
			fRec27[0] = max((fRec27[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph13 = FAUSTFLOAT(fRec27[0]);
			output13[i] = FAUSTFLOAT(fTemp19);
			double fTemp20 = (0.022574955908289243 * ((fTemp1 + (fSlow79 * fTemp3)) + (fSlow80 * fTemp5)));
			fRec28[0] = max((fRec28[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph14 = FAUSTFLOAT(fRec28[0]);
			output14[i] = FAUSTFLOAT(fTemp20);
			double fTemp21 = (0.022574955908289243 * ((fTemp1 + (fSlow82 * fTemp3)) + (fSlow83 * fTemp5)));
			fRec29[0] = max((fRec29[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph15 = FAUSTFLOAT(fRec29[0]);
			output15[i] = FAUSTFLOAT(fTemp21);
			double fTemp22 = (0.022574955908289243 * ((fTemp1 + (fSlow85 * fTemp3)) + (fSlow86 * fTemp5)));
			fRec30[0] = max((fRec30[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph16 = FAUSTFLOAT(fRec30[0]);
			output16[i] = FAUSTFLOAT(fTemp22);
			double fTemp23 = (0.022574955908289243 * ((fTemp1 + (fSlow88 * fTemp3)) + (fSlow89 * fTemp5)));
			fRec31[0] = max((fRec31[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fVbargraph17 = FAUSTFLOAT(fRec31[0]);
			output17[i] = FAUSTFLOAT(fTemp23);
			double fTemp24 = (0.021093750000000001 * ((fTemp1 + (fSlow93 * fTemp3)) + (fSlow94 * fTemp5)));
			fRec32[0] = max((fRec32[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph18 = FAUSTFLOAT(fRec32[0]);
			output18[i] = FAUSTFLOAT(fTemp24);
			double fTemp25 = (0.021093750000000001 * ((fTemp1 + (fSlow98 * fTemp3)) + (fSlow99 * fTemp5)));
			fRec33[0] = max((fRec33[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph19 = FAUSTFLOAT(fRec33[0]);
			output19[i] = FAUSTFLOAT(fTemp25);
			double fTemp26 = (0.021093750000000001 * ((fTemp1 + (fSlow103 * fTemp3)) + (fSlow104 * fTemp5)));
			fRec34[0] = max((fRec34[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fVbargraph20 = FAUSTFLOAT(fRec34[0]);
			output20[i] = FAUSTFLOAT(fTemp26);
			double fTemp27 = (0.021093750000000001 * ((fTemp1 + (fSlow108 * fTemp3)) + (fSlow109 * fTemp5)));
			fRec35[0] = max((fRec35[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph21 = FAUSTFLOAT(fRec35[0]);
			output21[i] = FAUSTFLOAT(fTemp27);
			double fTemp28 = (0.021093750000000001 * ((fTemp1 + (fSlow111 * fTemp3)) + (fSlow112 * fTemp5)));
			fRec36[0] = max((fRec36[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fVbargraph22 = FAUSTFLOAT(fRec36[0]);
			output22[i] = FAUSTFLOAT(fTemp28);
			double fTemp29 = (0.021093750000000001 * ((fTemp1 + (fSlow114 * fTemp3)) + (fSlow115 * fTemp5)));
			fRec37[0] = max((fRec37[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph23 = FAUSTFLOAT(fRec37[0]);
			output23[i] = FAUSTFLOAT(fTemp29);
			double fTemp30 = (0.021093750000000001 * ((fTemp1 + (fSlow117 * fTemp3)) + (fSlow118 * fTemp5)));
			fRec38[0] = max((fRec38[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph24 = FAUSTFLOAT(fRec38[0]);
			output24[i] = FAUSTFLOAT(fTemp30);
			double fTemp31 = (0.021093750000000001 * ((fTemp1 + (fSlow120 * fTemp3)) + (fSlow121 * fTemp5)));
			fRec39[0] = max((fRec39[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph25 = FAUSTFLOAT(fRec39[0]);
			output25[i] = FAUSTFLOAT(fTemp31);
			double fTemp32 = (0.02017333553791887 * ((fTemp1 + (fSlow125 * fTemp3)) + (fSlow126 * fTemp5)));
			fRec40[0] = max((fRec40[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph26 = FAUSTFLOAT(fRec40[0]);
			output26[i] = FAUSTFLOAT(fTemp32);
			double fTemp33 = (0.02017333553791887 * ((fTemp1 + (fSlow129 * fTemp3)) + (fSlow130 * fTemp5)));
			fRec41[0] = max((fRec41[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph27 = FAUSTFLOAT(fRec41[0]);
			output27[i] = FAUSTFLOAT(fTemp33);
			double fTemp34 = (0.02017333553791887 * ((fTemp1 + (fSlow133 * fTemp3)) + (fSlow134 * fTemp5)));
			fRec42[0] = max((fRec42[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fVbargraph28 = FAUSTFLOAT(fRec42[0]);
			output28[i] = FAUSTFLOAT(fTemp34);
			double fTemp35 = (0.02017333553791887 * ((fTemp1 + (fSlow137 * fTemp3)) + (fSlow138 * fTemp5)));
			fRec43[0] = max((fRec43[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph29 = FAUSTFLOAT(fRec43[0]);
			output29[i] = FAUSTFLOAT(fTemp35);
			double fTemp36 = (0.02017333553791887 * ((fTemp1 + (fSlow142 * fTemp3)) + (fSlow143 * fTemp5)));
			fRec44[0] = max((fRec44[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fVbargraph30 = FAUSTFLOAT(fRec44[0]);
			output30[i] = FAUSTFLOAT(fTemp36);
			double fTemp37 = (0.02017333553791887 * ((fTemp1 + (fSlow146 * fTemp3)) + (fSlow147 * fTemp5)));
			fRec45[0] = max((fRec45[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fVbargraph31 = FAUSTFLOAT(fRec45[0]);
			output31[i] = FAUSTFLOAT(fTemp37);
			double fTemp38 = (0.02017333553791887 * ((fTemp1 + (fSlow150 * fTemp3)) + (fSlow151 * fTemp5)));
			fRec46[0] = max((fRec46[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fVbargraph32 = FAUSTFLOAT(fRec46[0]);
			output32[i] = FAUSTFLOAT(fTemp38);
			double fTemp39 = (0.02017333553791887 * ((fTemp1 + (fSlow154 * fTemp3)) + (fSlow155 * fTemp5)));
			fRec47[0] = max((fRec47[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph33 = FAUSTFLOAT(fRec47[0]);
			output33[i] = FAUSTFLOAT(fTemp39);
			double fTemp40 = (0.02017333553791887 * ((fTemp1 + (fSlow158 * fTemp3)) + (fSlow159 * fTemp5)));
			fRec48[0] = max((fRec48[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fVbargraph34 = FAUSTFLOAT(fRec48[0]);
			output34[i] = FAUSTFLOAT(fTemp40);
			double fTemp41 = (0.02017333553791887 * ((fTemp1 + (fSlow162 * fTemp3)) + (fSlow163 * fTemp5)));
			fRec49[0] = max((fRec49[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph35 = FAUSTFLOAT(fRec49[0]);
			output35[i] = FAUSTFLOAT(fTemp41);
			double fTemp42 = (0.02017333553791887 * ((fTemp1 + (fSlow166 * fTemp3)) + (fSlow167 * fTemp5)));
			fRec50[0] = max((fRec50[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fVbargraph36 = FAUSTFLOAT(fRec50[0]);
			output36[i] = FAUSTFLOAT(fTemp42);
			double fTemp43 = (0.02017333553791887 * ((fTemp1 + (fSlow170 * fTemp3)) + (fSlow171 * fTemp5)));
			fRec51[0] = max((fRec51[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph37 = FAUSTFLOAT(fRec51[0]);
			output37[i] = FAUSTFLOAT(fTemp43);
			double fTemp44 = (0.02017333553791887 * ((fTemp1 + (fSlow173 * fTemp3)) + (fSlow174 * fTemp5)));
			fRec52[0] = max((fRec52[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fVbargraph38 = FAUSTFLOAT(fRec52[0]);
			output38[i] = FAUSTFLOAT(fTemp44);
			double fTemp45 = (0.02017333553791887 * ((fTemp1 + (fSlow176 * fTemp3)) + (fSlow177 * fTemp5)));
			fRec53[0] = max((fRec53[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fVbargraph39 = FAUSTFLOAT(fRec53[0]);
			output39[i] = FAUSTFLOAT(fTemp45);
			double fTemp46 = (0.02017333553791887 * ((fTemp1 + (fSlow179 * fTemp3)) + (fSlow180 * fTemp5)));
			fRec54[0] = max((fRec54[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph40 = FAUSTFLOAT(fRec54[0]);
			output40[i] = FAUSTFLOAT(fTemp46);
			double fTemp47 = (0.02017333553791887 * ((fTemp1 + (fSlow182 * fTemp3)) + (fSlow183 * fTemp5)));
			fRec55[0] = max((fRec55[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fVbargraph41 = FAUSTFLOAT(fRec55[0]);
			output41[i] = FAUSTFLOAT(fTemp47);
			double fTemp48 = (0.02017333553791887 * ((fTemp1 + (fSlow185 * fTemp3)) + (fSlow186 * fTemp5)));
			fRec56[0] = max((fRec56[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph42 = FAUSTFLOAT(fRec56[0]);
			output42[i] = FAUSTFLOAT(fTemp48);
			double fTemp49 = (0.02017333553791887 * ((fTemp1 + (fSlow188 * fTemp3)) + (fSlow189 * fTemp5)));
			fRec57[0] = max((fRec57[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fVbargraph43 = FAUSTFLOAT(fRec57[0]);
			output43[i] = FAUSTFLOAT(fTemp49);
			double fTemp50 = (0.02017333553791887 * ((fTemp1 + (fSlow191 * fTemp3)) + (fSlow192 * fTemp5)));
			fRec58[0] = max((fRec58[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph44 = FAUSTFLOAT(fRec58[0]);
			output44[i] = FAUSTFLOAT(fTemp50);
			double fTemp51 = (0.02017333553791887 * ((fTemp1 + (fSlow194 * fTemp3)) + (fSlow195 * fTemp5)));
			fRec59[0] = max((fRec59[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph45 = FAUSTFLOAT(fRec59[0]);
			output45[i] = FAUSTFLOAT(fTemp51);
			double fTemp52 = (0.02017333553791887 * ((fTemp1 + (fSlow197 * fTemp3)) + (fSlow198 * fTemp5)));
			fRec60[0] = max((fRec60[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fVbargraph46 = FAUSTFLOAT(fRec60[0]);
			output46[i] = FAUSTFLOAT(fTemp52);
			double fTemp53 = (0.02017333553791887 * ((fTemp1 + (fSlow200 * fTemp3)) + (fSlow201 * fTemp5)));
			fRec61[0] = max((fRec61[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fVbargraph47 = FAUSTFLOAT(fRec61[0]);
			output47[i] = FAUSTFLOAT(fTemp53);
			double fTemp54 = (0.02017333553791887 * ((fTemp1 + (fSlow203 * fTemp3)) + (fSlow204 * fTemp5)));
			fRec62[0] = max((fRec62[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fVbargraph48 = FAUSTFLOAT(fRec62[0]);
			output48[i] = FAUSTFLOAT(fTemp54);
			double fTemp55 = (0.02017333553791887 * ((fTemp1 + (fSlow206 * fTemp3)) + (fSlow207 * fTemp5)));
			fRec63[0] = max((fRec63[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph49 = FAUSTFLOAT(fRec63[0]);
			output49[i] = FAUSTFLOAT(fTemp55);
			fRec1[1] = fRec1[0];
			fRec4[1] = fRec4[0];
			fRec3[1] = fRec3[0];
			fRec2[1] = fRec2[0];
			fRec6[1] = fRec6[0];
			fRec5[1] = fRec5[0];
			fRec9[1] = fRec9[0];
			fRec10[1] = fRec10[0];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
			fVec0[1] = fVec0[0];
			fRec11[1] = fRec11[0];
			fRec13[1] = fRec13[0];
			fRec14[1] = fRec14[0];
			fRec12[1] = fRec12[0];
			fRec0[1] = fRec0[0];
			fRec15[1] = fRec15[0];
			fRec16[1] = fRec16[0];
			fRec17[1] = fRec17[0];
			fRec18[1] = fRec18[0];
			fRec19[1] = fRec19[0];
			fRec20[1] = fRec20[0];
			fRec21[1] = fRec21[0];
			fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			fRec24[1] = fRec24[0];
			fRec25[1] = fRec25[0];
			fRec26[1] = fRec26[0];
			fRec27[1] = fRec27[0];
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
