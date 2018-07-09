/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAEncoder5"
version: "1.0"
Code generated with Faust 2.5.23 (https://faust.grame.fr)
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
	FAUSTFLOAT fCheckbox0;
	FAUSTFLOAT fEntry0;
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fHslider1;
	double fRec1[2];
	double fRec0[2];
	FAUSTFLOAT fVbargraph0;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fHslider3;
	double fConst6;
	double fConst7;
	double fRec5[2];
	double fRec4[2];
	double fRec3[2];
	double fRec2[2];
	FAUSTFLOAT fVbargraph1;
	double fRec6[2];
	FAUSTFLOAT fVbargraph2;
	double fRec7[2];
	FAUSTFLOAT fVbargraph3;
	double fConst8;
	double fConst9;
	double fConst10;
	double fConst11;
	double fConst12;
	double fConst13;
	double fRec11[2];
	double fRec12[2];
	double fRec10[2];
	double fRec9[2];
	double fVec0[2];
	double fRec13[2];
	double fRec8[2];
	FAUSTFLOAT fVbargraph4;
	double fConst14;
	double fConst15;
	double fConst16;
	double fRec14[2];
	FAUSTFLOAT fVbargraph5;
	double fRec15[2];
	FAUSTFLOAT fVbargraph6;
	double fRec16[2];
	FAUSTFLOAT fVbargraph7;
	double fConst17;
	double fRec17[2];
	FAUSTFLOAT fVbargraph8;
	double fConst18;
	double fConst19;
	double fConst20;
	double fConst21;
	double fConst22;
	double fConst23;
	double fConst24;
	double fRec22[2];
	double fRec23[2];
	double fRec21[2];
	double fRec24[2];
	double fVec1[2];
	double fRec25[2];
	double fRec26[2];
	double fRec20[2];
	double fRec19[2];
	double fRec18[2];
	FAUSTFLOAT fVbargraph9;
	double fConst25;
	double fConst26;
	double fConst27;
	double fRec27[2];
	FAUSTFLOAT fVbargraph10;
	double fConst28;
	double fRec28[2];
	FAUSTFLOAT fVbargraph11;
	double fRec29[2];
	FAUSTFLOAT fVbargraph12;
	double fRec30[2];
	FAUSTFLOAT fVbargraph13;
	double fConst29;
	double fRec31[2];
	FAUSTFLOAT fVbargraph14;
	double fRec32[2];
	FAUSTFLOAT fVbargraph15;
	double fConst30;
	double fConst31;
	double fConst32;
	double fConst33;
	double fConst34;
	double fConst35;
	double fConst36;
	double fConst37;
	double fRec37[2];
	double fRec38[2];
	double fRec36[2];
	double fRec39[2];
	double fVec2[2];
	double fRec40[2];
	double fRec41[2];
	double fRec42[2];
	double fRec35[2];
	double fRec34[2];
	double fVec3[2];
	double fRec43[2];
	double fRec33[2];
	FAUSTFLOAT fVbargraph16;
	double fConst38;
	double fConst39;
	double fRec44[2];
	FAUSTFLOAT fVbargraph17;
	double fConst40;
	double fConst41;
	double fRec45[2];
	FAUSTFLOAT fVbargraph18;
	double fConst42;
	double fRec46[2];
	FAUSTFLOAT fVbargraph19;
	double fRec47[2];
	FAUSTFLOAT fVbargraph20;
	double fRec48[2];
	FAUSTFLOAT fVbargraph21;
	double fConst43;
	double fRec49[2];
	FAUSTFLOAT fVbargraph22;
	double fRec50[2];
	FAUSTFLOAT fVbargraph23;
	double fConst44;
	double fRec51[2];
	FAUSTFLOAT fVbargraph24;
	double fConst45;
	double fConst46;
	double fConst47;
	double fConst48;
	double fConst49;
	double fConst50;
	double fRec57[2];
	double fRec58[2];
	double fRec56[2];
	double fRec59[2];
	double fVec4[2];
	double fRec60[2];
	double fConst51;
	double fRec61[2];
	double fRec62[2];
	double fRec55[2];
	double fRec63[2];
	double fVec5[2];
	double fRec64[2];
	double fRec65[2];
	double fRec54[2];
	double fRec53[2];
	double fRec52[2];
	FAUSTFLOAT fVbargraph25;
	double fConst52;
	double fConst53;
	double fRec66[2];
	FAUSTFLOAT fVbargraph26;
	double fConst54;
	double fRec67[2];
	FAUSTFLOAT fVbargraph27;
	double fConst55;
	double fConst56;
	double fRec68[2];
	FAUSTFLOAT fVbargraph28;
	double fConst57;
	double fRec69[2];
	FAUSTFLOAT fVbargraph29;
	double fRec70[2];
	FAUSTFLOAT fVbargraph30;
	double fRec71[2];
	FAUSTFLOAT fVbargraph31;
	double fConst58;
	double fRec72[2];
	FAUSTFLOAT fVbargraph32;
	double fRec73[2];
	FAUSTFLOAT fVbargraph33;
	double fConst59;
	double fRec74[2];
	FAUSTFLOAT fVbargraph34;
	double fRec75[2];
	FAUSTFLOAT fVbargraph35;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("filename", "HOAEncoder5");
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
		m->declare("name", "HOAEncoder5");
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
		return 36;
		
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
		fConst2 = double(tgamma(1.0));
		fConst3 = double(tgamma(3.0));
		fConst4 = double(tgamma(1.5));
		fConst5 = (1.5957691216057308 * (sqrt((3.0 * (fConst2 / fConst3))) * fConst4));
		fConst6 = (340.0 / fConst0);
		fConst7 = (170.0 / fConst0);
		fConst8 = double(tgamma(5.0));
		fConst9 = double(tgamma(2.5));
		fConst10 = (sqrt((5.0 * (fConst2 / fConst8))) * fConst9);
		fConst11 = (6.3830764864229232 * fConst10);
		fConst12 = (510.0 / fConst0);
		fConst13 = (1020.0 / fConst0);
		fConst14 = double(tgamma(2.0));
		fConst15 = double(tgamma(4.0));
		fConst16 = (4.7873073648171927 * (sqrt((5.0 * (fConst14 / fConst15))) * fConst4));
		fConst17 = (3.1915382432114616 * fConst10);
		fConst18 = double(tgamma(7.0));
		fConst19 = double(tgamma(3.5));
		fConst20 = (6.3830764864229232 * (sqrt((7.0 * (fConst2 / fConst18))) * fConst19));
		fConst21 = (789.54302057287055 / fConst0);
		fConst22 = (394.77151028643527 / fConst0);
		fConst23 = (625.22848971356643 / fConst0);
		fConst24 = (1250.4569794271329 / fConst0);
		fConst25 = double(tgamma(6.0));
		fConst26 = (sqrt((7.0 * (fConst14 / fConst25))) * fConst9);
		fConst27 = (31.915382432114615 * fConst26);
		fConst28 = (0.70710678118654757 * (sqrt((7.0 * (fConst3 / fConst8))) * fConst4));
		fConst29 = (15.957691216057308 * fConst26);
		fConst30 = double(tgamma(9.0));
		fConst31 = double(tgamma(4.5));
		fConst32 = (sqrt((9.0 * (fConst2 / fConst30))) * fConst31);
		fConst33 = (25.532305945691693 * fConst32);
		fConst34 = (984.71160495892411 / fConst0);
		fConst35 = (1969.4232099178482 / fConst0);
		fConst36 = (715.28839504107589 / fConst0);
		fConst37 = (1430.5767900821518 / fConst0);
		fConst38 = double(tgamma(8.0));
		fConst39 = (44.681535404960464 * (sqrt((9.0 * (fConst14 / fConst38))) * fConst19));
		fConst40 = (sqrt((9.0 * (fConst3 / fConst18))) * fConst9);
		fConst41 = (1.4142135623730951 * fConst40);
		fConst42 = (0.47140452079103168 * (sqrt((9.0 * (fConst15 / fConst25))) * fConst4));
		fConst43 = (0.70710678118654757 * fConst40);
		fConst44 = (12.766152972845846 * fConst32);
		fConst45 = (25.532305945691693 * (sqrt((11.0 * (fConst2 / double(tgamma(11.0))))) * double(tgamma(5.5))));
		fConst46 = (1239.8911224120607 / fConst0);
		fConst47 = (619.94556120603033 / fConst0);
		fConst48 = (1139.6651757122002 / fConst0);
		fConst49 = (790.3892630817644 / fConst0);
		fConst50 = (1580.7785261635288 / fConst0);
		fConst51 = (2279.3303514244003 / fConst0);
		fConst52 = (sqrt((11.0 * (fConst14 / double(tgamma(10.0))))) * fConst31);
		fConst53 = (229.79075351122526 * fConst52);
		fConst54 = (0.70710678118654757 * (sqrt((11.0 * (fConst3 / fConst30))) * fConst19));
		fConst55 = (sqrt((11.0 * (fConst15 / fConst38))) * fConst9);
		fConst56 = (0.94280904158206336 * fConst55);
		fConst57 = (0.35355339059327379 * (sqrt((11.0 * (fConst8 / fConst18))) * fConst4));
		fConst58 = (0.47140452079103168 * fConst55);
		fConst59 = (114.89537675561263 * fConst52);
		
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = FAUSTFLOAT(0.0);
		fEntry0 = FAUSTFLOAT(1.0700000000000001);
		fHslider0 = FAUSTFLOAT(2.0);
		fHslider1 = FAUSTFLOAT(0.0);
		fHslider2 = FAUSTFLOAT(0.0);
		fHslider3 = FAUSTFLOAT(0.0);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec1[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec0[l1] = 0.0;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec5[l2] = 0.0;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec4[l3] = 0.0;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec3[l4] = 0.0;
			
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec2[l5] = 0.0;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec6[l6] = 0.0;
			
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec7[l7] = 0.0;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec11[l8] = 0.0;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fRec12[l9] = 0.0;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec10[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec9[l11] = 0.0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fVec0[l12] = 0.0;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec13[l13] = 0.0;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec8[l14] = 0.0;
			
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec14[l15] = 0.0;
			
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
			fRec22[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec23[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec21[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec24[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fVec1[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec25[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec26[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec20[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec19[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec18[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec27[l29] = 0.0;
			
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
			fRec37[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec38[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec36[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec39[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fVec2[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec40[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec41[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec42[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec35[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec34[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fVec3[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec43[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec33[l47] = 0.0;
			
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
			fRec57[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec58[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec56[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec59[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fVec4[l60] = 0.0;
			
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
			fRec55[l64] = 0.0;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec63[l65] = 0.0;
			
		}
		for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
			fVec5[l66] = 0.0;
			
		}
		for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
			fRec64[l67] = 0.0;
			
		}
		for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
			fRec65[l68] = 0.0;
			
		}
		for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
			fRec54[l69] = 0.0;
			
		}
		for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
			fRec53[l70] = 0.0;
			
		}
		for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
			fRec52[l71] = 0.0;
			
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
		ui_interface->openHorizontalBox("HOAEncoder5");
		ui_interface->openVerticalBox("Parameters");
		ui_interface->openHorizontalBox("Source  0");
		ui_interface->declare(&fHslider1, "0+1", "");
		ui_interface->declare(&fHslider1, "osc", "/gain_0 -20 20");
		ui_interface->declare(&fHslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  0", &fHslider1, 0.0, -20.0, 20.0, 0.10000000000000001);
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
		ui_interface->declare(&fEntry0, "~", "");
		ui_interface->addNumEntry("Speaker Radius  0", &fEntry0, 1.0700000000000001, 0.5, 10.0, 0.01);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openVerticalBox("Outputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244c2e950", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244c44ee0", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244c4a9f0", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244c50bb0", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244c68f10", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244c70fd0", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244c77420", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244c7d4f0", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244c84900", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244ca3f30", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244cab9f0", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244cb3310", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244cb9aa0", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244cbfb20", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244cc6330", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244ccce10", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244cf3650", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244cfb2a0", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d03180", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d0ac70", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d11740", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d17b60", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d1e360", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d24760", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d2bbc0", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5");
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d593b0", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d60f10", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d68a00", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d70ab0", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d78940", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d7f590", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d85990", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d8c530", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d92cd0", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244d99a20", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x559244da05c0", &fVbargraph35, -70.0, 6.0);
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
		double fSlow0 = double(fCheckbox0);
		double fSlow1 = double(fEntry0);
		double fSlow2 = double(fHslider0);
		double fSlow3 = (1.0 - fSlow0);
		double fSlow4 = (((fSlow0 * fSlow1) / fSlow2) + fSlow3);
		double fSlow5 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fHslider1))));
		double fSlow6 = sin(double(fHslider2));
		double fSlow7 = mydsp_faustpower2_f(fSlow6);
		double fSlow8 = (1.0 - fSlow7);
		double fSlow9 = pow(fSlow8, 0.5);
		double fSlow10 = double(fHslider3);
		double fSlow11 = sin(fSlow10);
		double fSlow12 = (fConst5 * (fSlow9 * fSlow11));
		double fSlow13 = (fConst7 / fSlow2);
		double fSlow14 = (fSlow13 + 1.0);
		double fSlow15 = (fConst6 / (fSlow14 * fSlow2));
		double fSlow16 = (fConst7 / fSlow1);
		double fSlow17 = (1.0 / (fSlow16 + 1.0));
		double fSlow18 = (((fSlow14 * fSlow0) * fSlow1) / fSlow2);
		double fSlow19 = (fConst6 / fSlow1);
		double fSlow20 = (1.7320508075688772 * fSlow6);
		double fSlow21 = cos(fSlow10);
		double fSlow22 = (fConst5 * (fSlow21 * fSlow9));
		double fSlow23 = (fConst11 * ((fSlow21 * fSlow8) * fSlow11));
		double fSlow24 = mydsp_faustpower2_f(fSlow13);
		double fSlow25 = (((fConst12 / fSlow2) + (3.0 * fSlow24)) + 1.0);
		double fSlow26 = (1.0 / fSlow25);
		double fSlow27 = (12.0 * fSlow24);
		double fSlow28 = ((fConst13 / fSlow2) + fSlow27);
		double fSlow29 = mydsp_faustpower2_f(fSlow16);
		double fSlow30 = (1.0 / (((fConst12 / fSlow1) + (3.0 * fSlow29)) + 1.0));
		double fSlow31 = (((fSlow25 * fSlow0) * fSlow1) / fSlow2);
		double fSlow32 = (12.0 * fSlow29);
		double fSlow33 = (fSlow32 + (fConst13 / fSlow1));
		double fSlow34 = (fConst16 * ((fSlow6 * fSlow9) * fSlow11));
		double fSlow35 = ((3.0 * fSlow7) + -1.0);
		double fSlow36 = (1.1180339887498949 * fSlow35);
		double fSlow37 = (fSlow21 * fSlow6);
		double fSlow38 = (fConst16 * (fSlow37 * fSlow9));
		double fSlow39 = mydsp_faustpower2_f(fSlow21);
		double fSlow40 = ((2.0 * fSlow39) + -1.0);
		double fSlow41 = (fConst17 * (fSlow40 * fSlow8));
		double fSlow42 = ((4.0 * fSlow39) + -1.0);
		double fSlow43 = pow(fSlow8, 1.5);
		double fSlow44 = (fConst20 * ((fSlow42 * fSlow43) * fSlow11));
		double fSlow45 = ((fConst22 / fSlow2) + 1.0);
		double fSlow46 = (fConst21 / (fSlow45 * fSlow2));
		double fSlow47 = (1.0 / ((fConst22 / fSlow1) + 1.0));
		double fSlow48 = (1.0 / (((fConst23 / fSlow1) + (6.4594326934833797 * fSlow29)) + 1.0));
		double fSlow49 = (((fConst23 / fSlow2) + (6.4594326934833797 * fSlow24)) + 1.0);
		double fSlow50 = (((fSlow49 * fSlow0) * fSlow1) / fSlow2);
		double fSlow51 = (25.837730773933519 * fSlow29);
		double fSlow52 = (fSlow51 + (fConst24 / fSlow1));
		double fSlow53 = (1.0 / fSlow49);
		double fSlow54 = (25.837730773933519 * fSlow24);
		double fSlow55 = ((fConst24 / fSlow2) + fSlow54);
		double fSlow56 = (fConst21 / fSlow1);
		double fSlow57 = (fConst27 * ((fSlow37 * fSlow8) * fSlow11));
		double fSlow58 = ((16.925687506432688 * fSlow7) + -3.3851375012865379);
		double fSlow59 = (fConst28 * ((fSlow58 * fSlow9) * fSlow11));
		double fSlow60 = ((2.5 * fSlow35) + -2.0);
		double fSlow61 = (0.88191710368819687 * (fSlow60 * fSlow6));
		double fSlow62 = (fConst28 * ((fSlow58 * fSlow21) * fSlow9));
		double fSlow63 = (fConst29 * ((fSlow40 * fSlow6) * fSlow8));
		double fSlow64 = ((2.0 * fSlow40) + -1.0);
		double fSlow65 = (fSlow64 * fSlow21);
		double fSlow66 = (fConst20 * (fSlow65 * fSlow43));
		double fSlow67 = (fSlow42 + -1.0);
		double fSlow68 = (fSlow67 * fSlow21);
		double fSlow69 = mydsp_faustpower2_f(fSlow8);
		double fSlow70 = (fConst33 * ((fSlow68 * fSlow69) * fSlow11));
		double fSlow71 = (((fConst34 / fSlow2) + (9.1401308902779004 * fSlow24)) + 1.0);
		double fSlow72 = (1.0 / fSlow71);
		double fSlow73 = (36.560523561111602 * fSlow24);
		double fSlow74 = ((fConst35 / fSlow2) + fSlow73);
		double fSlow75 = (1.0 / (((fConst34 / fSlow1) + (9.1401308902779004 * fSlow29)) + 1.0));
		double fSlow76 = (1.0 / (((fConst36 / fSlow1) + (11.4878004768713 * fSlow29)) + 1.0));
		double fSlow77 = (((fConst36 / fSlow2) + (11.4878004768713 * fSlow24)) + 1.0);
		double fSlow78 = (((fSlow77 * fSlow0) * fSlow1) / fSlow2);
		double fSlow79 = (45.951201907485199 * fSlow29);
		double fSlow80 = (fSlow79 + (fConst37 / fSlow1));
		double fSlow81 = (1.0 / fSlow77);
		double fSlow82 = (45.951201907485199 * fSlow24);
		double fSlow83 = ((fConst37 / fSlow2) + fSlow82);
		double fSlow84 = (36.560523561111602 * fSlow29);
		double fSlow85 = (fSlow84 + (fConst35 / fSlow1));
		double fSlow86 = (fConst39 * (((fSlow42 * fSlow6) * fSlow43) * fSlow11));
		double fSlow87 = ((78.986541696685876 * fSlow7) + -11.283791670955125);
		double fSlow88 = (fConst41 * (((fSlow87 * fSlow21) * fSlow8) * fSlow11));
		double fSlow89 = ((3.5 * fSlow58) + -13.540550005146152);
		double fSlow90 = (fConst42 * (((fSlow89 * fSlow6) * fSlow9) * fSlow11));
		double fSlow91 = ((2.333333333333333 * (fSlow60 * fSlow7)) - (1.5 * fSlow35));
		double fSlow92 = (0.75 * fSlow91);
		double fSlow93 = (fConst42 * (((fSlow89 * fSlow21) * fSlow6) * fSlow9));
		double fSlow94 = (fConst43 * ((fSlow40 * fSlow87) * fSlow8));
		double fSlow95 = (fConst39 * ((fSlow65 * fSlow6) * fSlow43));
		double fSlow96 = (1.0 - (2.0 * ((1.0 - fSlow64) * fSlow39)));
		double fSlow97 = (fConst44 * (fSlow96 * fSlow69));
		double fSlow98 = pow(fSlow8, 2.5);
		double fSlow99 = (fConst45 * (((1.0 - (4.0 * ((1.0 - fSlow67) * fSlow39))) * fSlow98) * fSlow11));
		double fSlow100 = ((fConst47 / fSlow2) + 1.0);
		double fSlow101 = (fConst46 / (fSlow100 * fSlow2));
		double fSlow102 = (1.0 / ((fConst47 / fSlow1) + 1.0));
		double fSlow103 = (1.0 / (((fConst48 / fSlow1) + (14.272480513280099 * fSlow29)) + 1.0));
		double fSlow104 = (((fConst48 / fSlow2) + (14.272480513280099 * fSlow24)) + 1.0);
		double fSlow105 = (1.0 / (((fConst49 / fSlow1) + (18.1563153134523 * fSlow29)) + 1.0));
		double fSlow106 = (((fConst49 / fSlow2) + (18.1563153134523 * fSlow24)) + 1.0);
		double fSlow107 = (((fSlow106 * fSlow0) * fSlow1) / fSlow2);
		double fSlow108 = (72.6252612538092 * fSlow29);
		double fSlow109 = (fSlow108 + (fConst50 / fSlow1));
		double fSlow110 = (1.0 / fSlow106);
		double fSlow111 = (72.6252612538092 * fSlow24);
		double fSlow112 = ((fConst50 / fSlow2) + fSlow111);
		double fSlow113 = (57.089922053120397 * fSlow29);
		double fSlow114 = (fSlow113 + (fConst51 / fSlow1));
		double fSlow115 = (1.0 / fSlow104);
		double fSlow116 = (57.089922053120397 * fSlow24);
		double fSlow117 = ((fConst51 / fSlow2) + fSlow116);
		double fSlow118 = (fConst46 / fSlow1);
		double fSlow119 = (fConst53 * (((fSlow68 * fSlow6) * fSlow69) * fSlow11));
		double fSlow120 = ((284.35155010806915 * fSlow7) + -31.594616678674353);
		double fSlow121 = (fConst54 * (((fSlow42 * fSlow120) * fSlow43) * fSlow11));
		double fSlow122 = ((4.5 * fSlow87) + -67.702750025730751);
		double fSlow123 = (fConst56 * ((((fSlow122 * fSlow21) * fSlow6) * fSlow8) * fSlow11));
		double fSlow124 = ((3.0 * (fSlow89 * fSlow7)) - (2.5 * fSlow58));
		double fSlow125 = (fConst57 * ((fSlow124 * fSlow9) * fSlow11));
		double fSlow126 = (0.66332495807108005 * (((2.25 * fSlow91) - (1.3333333333333333 * fSlow60)) * fSlow6));
		double fSlow127 = (fConst57 * ((fSlow124 * fSlow21) * fSlow9));
		double fSlow128 = (fConst58 * (((fSlow40 * fSlow122) * fSlow6) * fSlow8));
		double fSlow129 = (fConst54 * (((fSlow64 * fSlow120) * fSlow21) * fSlow43));
		double fSlow130 = (fConst59 * ((fSlow96 * fSlow6) * fSlow69));
		double fSlow131 = (fConst45 * ((((2.0 * fSlow96) - fSlow64) * fSlow21) * fSlow98));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fSlow5 + (0.999 * fRec1[1]));
			double fTemp0 = (fRec1[0] * double(input0[i]));
			double fTemp1 = (fSlow4 * fTemp0);
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp1))))));
			fVbargraph0 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp1);
			fRec5[0] = (fRec5[1] + fRec4[1]);
			fRec4[0] = (fSlow17 * ((fSlow18 * fTemp0) - (fSlow19 * fRec5[0])));
			fRec3[0] = (fRec4[0] + fRec3[1]);
			double fTemp2 = (fSlow3 * fTemp0);
			double fTemp3 = ((fSlow15 * (fRec3[0] - fRec4[0])) + (fRec4[0] + fTemp2));
			double fTemp4 = (fSlow12 * fTemp3);
			fRec2[0] = max((fRec2[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fVbargraph1 = FAUSTFLOAT(fRec2[0]);
			output1[i] = FAUSTFLOAT(fTemp4);
			double fTemp5 = (fSlow20 * fTemp3);
			fRec6[0] = max((fRec6[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fVbargraph2 = FAUSTFLOAT(fRec6[0]);
			output2[i] = FAUSTFLOAT(fTemp5);
			double fTemp6 = (fSlow22 * fTemp3);
			fRec7[0] = max((fRec7[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fVbargraph3 = FAUSTFLOAT(fRec7[0]);
			output3[i] = FAUSTFLOAT(fTemp6);
			fRec11[0] = (fRec11[1] + fRec10[1]);
			fRec12[0] = (fRec12[1] + fRec11[1]);
			fRec10[0] = (fSlow30 * ((fSlow31 * fTemp0) - ((fSlow33 * fRec11[0]) + (fSlow32 * fRec12[0]))));
			fRec9[0] = (fRec10[0] + fRec9[1]);
			double fTemp7 = (fRec9[0] - fRec10[0]);
			fVec0[0] = fTemp7;
			fRec13[0] = (fRec13[1] + fVec0[1]);
			double fTemp8 = ((fSlow26 * ((fSlow28 * fTemp7) + (fSlow27 * fRec13[0]))) + (fRec10[0] + fTemp2));
			double fTemp9 = (fSlow23 * fTemp8);
			fRec8[0] = max((fRec8[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph4 = FAUSTFLOAT(fRec8[0]);
			output4[i] = FAUSTFLOAT(fTemp9);
			double fTemp10 = (fSlow34 * fTemp8);
			fRec14[0] = max((fRec14[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fVbargraph5 = FAUSTFLOAT(fRec14[0]);
			output5[i] = FAUSTFLOAT(fTemp10);
			double fTemp11 = (fSlow36 * fTemp8);
			fRec15[0] = max((fRec15[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fVbargraph6 = FAUSTFLOAT(fRec15[0]);
			output6[i] = FAUSTFLOAT(fTemp11);
			double fTemp12 = (fSlow38 * fTemp8);
			fRec16[0] = max((fRec16[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph7 = FAUSTFLOAT(fRec16[0]);
			output7[i] = FAUSTFLOAT(fTemp12);
			double fTemp13 = (fSlow41 * fTemp8);
			fRec17[0] = max((fRec17[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fVbargraph8 = FAUSTFLOAT(fRec17[0]);
			output8[i] = FAUSTFLOAT(fTemp13);
			fRec22[0] = (fRec22[1] + fRec21[1]);
			fRec23[0] = (fRec23[1] + fRec22[1]);
			fRec21[0] = (fSlow48 * ((fSlow50 * fTemp0) - ((fSlow52 * fRec22[0]) + (fSlow51 * fRec23[0]))));
			fRec24[0] = (fRec21[0] + fRec24[1]);
			double fTemp14 = (fRec24[0] - fRec21[0]);
			fVec1[0] = fTemp14;
			fRec25[0] = (fRec25[1] + fVec1[1]);
			fRec26[0] = (fRec26[1] + fRec20[1]);
			fRec20[0] = (fSlow47 * ((fSlow45 * (fRec21[0] + (fSlow53 * ((fSlow55 * fTemp14) + (fSlow54 * fRec25[0]))))) - (fSlow56 * fRec26[0])));
			fRec19[0] = (fRec20[0] + fRec19[1]);
			double fTemp15 = ((fSlow46 * (fRec19[0] - fRec20[0])) + (fRec20[0] + fTemp2));
			double fTemp16 = (fSlow44 * fTemp15);
			fRec18[0] = max((fRec18[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph9 = FAUSTFLOAT(fRec18[0]);
			output9[i] = FAUSTFLOAT(fTemp16);
			double fTemp17 = (fSlow57 * fTemp15);
			fRec27[0] = max((fRec27[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph10 = FAUSTFLOAT(fRec27[0]);
			output10[i] = FAUSTFLOAT(fTemp17);
			double fTemp18 = (fSlow59 * fTemp15);
			fRec28[0] = max((fRec28[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph11 = FAUSTFLOAT(fRec28[0]);
			output11[i] = FAUSTFLOAT(fTemp18);
			double fTemp19 = (fSlow61 * fTemp15);
			fRec29[0] = max((fRec29[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph12 = FAUSTFLOAT(fRec29[0]);
			output12[i] = FAUSTFLOAT(fTemp19);
			double fTemp20 = (fSlow62 * fTemp15);
			fRec30[0] = max((fRec30[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph13 = FAUSTFLOAT(fRec30[0]);
			output13[i] = FAUSTFLOAT(fTemp20);
			double fTemp21 = (fSlow63 * fTemp15);
			fRec31[0] = max((fRec31[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph14 = FAUSTFLOAT(fRec31[0]);
			output14[i] = FAUSTFLOAT(fTemp21);
			double fTemp22 = (fSlow66 * fTemp15);
			fRec32[0] = max((fRec32[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph15 = FAUSTFLOAT(fRec32[0]);
			output15[i] = FAUSTFLOAT(fTemp22);
			fRec37[0] = (fRec37[1] + fRec36[1]);
			fRec38[0] = (fRec38[1] + fRec37[1]);
			fRec36[0] = (fSlow76 * ((fSlow78 * fTemp0) - ((fSlow80 * fRec37[0]) + (fSlow79 * fRec38[0]))));
			fRec39[0] = (fRec36[0] + fRec39[1]);
			double fTemp23 = (fRec39[0] - fRec36[0]);
			fVec2[0] = fTemp23;
			fRec40[0] = (fRec40[1] + fVec2[1]);
			fRec41[0] = (fRec41[1] + fRec35[1]);
			fRec42[0] = (fRec42[1] + fRec41[1]);
			fRec35[0] = (fSlow75 * ((fSlow71 * (fRec36[0] + (fSlow81 * ((fSlow83 * fTemp23) + (fSlow82 * fRec40[0]))))) - ((fSlow85 * fRec41[0]) + (fSlow84 * fRec42[0]))));
			fRec34[0] = (fRec35[0] + fRec34[1]);
			double fTemp24 = (fRec34[0] - fRec35[0]);
			fVec3[0] = fTemp24;
			fRec43[0] = (fRec43[1] + fVec3[1]);
			double fTemp25 = ((fSlow72 * ((fSlow74 * fTemp24) + (fSlow73 * fRec43[0]))) + (fRec35[0] + fTemp2));
			double fTemp26 = (fSlow70 * fTemp25);
			fRec33[0] = max((fRec33[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fVbargraph16 = FAUSTFLOAT(fRec33[0]);
			output16[i] = FAUSTFLOAT(fTemp26);
			double fTemp27 = (fSlow86 * fTemp25);
			fRec44[0] = max((fRec44[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph17 = FAUSTFLOAT(fRec44[0]);
			output17[i] = FAUSTFLOAT(fTemp27);
			double fTemp28 = (fSlow88 * fTemp25);
			fRec45[0] = max((fRec45[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fVbargraph18 = FAUSTFLOAT(fRec45[0]);
			output18[i] = FAUSTFLOAT(fTemp28);
			double fTemp29 = (fSlow90 * fTemp25);
			fRec46[0] = max((fRec46[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph19 = FAUSTFLOAT(fRec46[0]);
			output19[i] = FAUSTFLOAT(fTemp29);
			double fTemp30 = (fSlow92 * fTemp25);
			fRec47[0] = max((fRec47[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph20 = FAUSTFLOAT(fRec47[0]);
			output20[i] = FAUSTFLOAT(fTemp30);
			double fTemp31 = (fSlow93 * fTemp25);
			fRec48[0] = max((fRec48[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph21 = FAUSTFLOAT(fRec48[0]);
			output21[i] = FAUSTFLOAT(fTemp31);
			double fTemp32 = (fSlow94 * fTemp25);
			fRec49[0] = max((fRec49[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph22 = FAUSTFLOAT(fRec49[0]);
			output22[i] = FAUSTFLOAT(fTemp32);
			double fTemp33 = (fSlow95 * fTemp25);
			fRec50[0] = max((fRec50[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph23 = FAUSTFLOAT(fRec50[0]);
			output23[i] = FAUSTFLOAT(fTemp33);
			double fTemp34 = (fSlow97 * fTemp25);
			fRec51[0] = max((fRec51[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fVbargraph24 = FAUSTFLOAT(fRec51[0]);
			output24[i] = FAUSTFLOAT(fTemp34);
			fRec57[0] = (fRec57[1] + fRec56[1]);
			fRec58[0] = (fRec58[1] + fRec57[1]);
			fRec56[0] = (fSlow105 * ((fSlow107 * fTemp0) - ((fSlow109 * fRec57[0]) + (fSlow108 * fRec58[0]))));
			fRec59[0] = (fRec56[0] + fRec59[1]);
			double fTemp35 = (fRec59[0] - fRec56[0]);
			fVec4[0] = fTemp35;
			fRec60[0] = (fRec60[1] + fVec4[1]);
			fRec61[0] = (fRec61[1] + fRec55[1]);
			fRec62[0] = (fRec62[1] + fRec61[1]);
			fRec55[0] = (fSlow103 * ((fSlow104 * (fRec56[0] + (fSlow110 * ((fSlow112 * fTemp35) + (fSlow111 * fRec60[0]))))) - ((fSlow114 * fRec61[0]) + (fSlow113 * fRec62[0]))));
			fRec63[0] = (fRec55[0] + fRec63[1]);
			double fTemp36 = (fRec63[0] - fRec55[0]);
			fVec5[0] = fTemp36;
			fRec64[0] = (fRec64[1] + fVec5[1]);
			fRec65[0] = (fRec65[1] + fRec54[1]);
			fRec54[0] = (fSlow102 * ((fSlow100 * (fRec55[0] + (fSlow115 * ((fSlow117 * fTemp36) + (fSlow116 * fRec64[0]))))) - (fSlow118 * fRec65[0])));
			fRec53[0] = (fRec54[0] + fRec53[1]);
			double fTemp37 = ((fSlow101 * (fRec53[0] - fRec54[0])) + (fRec54[0] + fTemp2));
			double fTemp38 = (fSlow99 * fTemp37);
			fRec52[0] = max((fRec52[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fVbargraph25 = FAUSTFLOAT(fRec52[0]);
			output25[i] = FAUSTFLOAT(fTemp38);
			double fTemp39 = (fSlow119 * fTemp37);
			fRec66[0] = max((fRec66[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph26 = FAUSTFLOAT(fRec66[0]);
			output26[i] = FAUSTFLOAT(fTemp39);
			double fTemp40 = (fSlow121 * fTemp37);
			fRec67[0] = max((fRec67[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fVbargraph27 = FAUSTFLOAT(fRec67[0]);
			output27[i] = FAUSTFLOAT(fTemp40);
			double fTemp41 = (fSlow123 * fTemp37);
			fRec68[0] = max((fRec68[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph28 = FAUSTFLOAT(fRec68[0]);
			output28[i] = FAUSTFLOAT(fTemp41);
			double fTemp42 = (fSlow125 * fTemp37);
			fRec69[0] = max((fRec69[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fVbargraph29 = FAUSTFLOAT(fRec69[0]);
			output29[i] = FAUSTFLOAT(fTemp42);
			double fTemp43 = (fSlow126 * fTemp37);
			fRec70[0] = max((fRec70[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph30 = FAUSTFLOAT(fRec70[0]);
			output30[i] = FAUSTFLOAT(fTemp43);
			double fTemp44 = (fSlow127 * fTemp37);
			fRec71[0] = max((fRec71[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fVbargraph31 = FAUSTFLOAT(fRec71[0]);
			output31[i] = FAUSTFLOAT(fTemp44);
			double fTemp45 = (fSlow128 * fTemp37);
			fRec72[0] = max((fRec72[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fVbargraph32 = FAUSTFLOAT(fRec72[0]);
			output32[i] = FAUSTFLOAT(fTemp45);
			double fTemp46 = (fSlow129 * fTemp37);
			fRec73[0] = max((fRec73[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph33 = FAUSTFLOAT(fRec73[0]);
			output33[i] = FAUSTFLOAT(fTemp46);
			double fTemp47 = (fSlow130 * fTemp37);
			fRec74[0] = max((fRec74[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fVbargraph34 = FAUSTFLOAT(fRec74[0]);
			output34[i] = FAUSTFLOAT(fTemp47);
			double fTemp48 = (fSlow131 * fTemp37);
			fRec75[0] = max((fRec75[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph35 = FAUSTFLOAT(fRec75[0]);
			output35[i] = FAUSTFLOAT(fTemp48);
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec5[1] = fRec5[0];
			fRec4[1] = fRec4[0];
			fRec3[1] = fRec3[0];
			fRec2[1] = fRec2[0];
			fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			fRec11[1] = fRec11[0];
			fRec12[1] = fRec12[0];
			fRec10[1] = fRec10[0];
			fRec9[1] = fRec9[0];
			fVec0[1] = fVec0[0];
			fRec13[1] = fRec13[0];
			fRec8[1] = fRec8[0];
			fRec14[1] = fRec14[0];
			fRec15[1] = fRec15[0];
			fRec16[1] = fRec16[0];
			fRec17[1] = fRec17[0];
			fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			fRec21[1] = fRec21[0];
			fRec24[1] = fRec24[0];
			fVec1[1] = fVec1[0];
			fRec25[1] = fRec25[0];
			fRec26[1] = fRec26[0];
			fRec20[1] = fRec20[0];
			fRec19[1] = fRec19[0];
			fRec18[1] = fRec18[0];
			fRec27[1] = fRec27[0];
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec30[1] = fRec30[0];
			fRec31[1] = fRec31[0];
			fRec32[1] = fRec32[0];
			fRec37[1] = fRec37[0];
			fRec38[1] = fRec38[0];
			fRec36[1] = fRec36[0];
			fRec39[1] = fRec39[0];
			fVec2[1] = fVec2[0];
			fRec40[1] = fRec40[0];
			fRec41[1] = fRec41[0];
			fRec42[1] = fRec42[0];
			fRec35[1] = fRec35[0];
			fRec34[1] = fRec34[0];
			fVec3[1] = fVec3[0];
			fRec43[1] = fRec43[0];
			fRec33[1] = fRec33[0];
			fRec44[1] = fRec44[0];
			fRec45[1] = fRec45[0];
			fRec46[1] = fRec46[0];
			fRec47[1] = fRec47[0];
			fRec48[1] = fRec48[0];
			fRec49[1] = fRec49[0];
			fRec50[1] = fRec50[0];
			fRec51[1] = fRec51[0];
			fRec57[1] = fRec57[0];
			fRec58[1] = fRec58[0];
			fRec56[1] = fRec56[0];
			fRec59[1] = fRec59[0];
			fVec4[1] = fVec4[0];
			fRec60[1] = fRec60[0];
			fRec61[1] = fRec61[0];
			fRec62[1] = fRec62[0];
			fRec55[1] = fRec55[0];
			fRec63[1] = fRec63[0];
			fVec5[1] = fVec5[0];
			fRec64[1] = fRec64[0];
			fRec65[1] = fRec65[0];
			fRec54[1] = fRec54[0];
			fRec53[1] = fRec53[0];
			fRec52[1] = fRec52[0];
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
