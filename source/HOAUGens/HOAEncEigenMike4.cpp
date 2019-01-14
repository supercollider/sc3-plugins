/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2015"
license: "GPL"
name: "HOAEncEigenMike4"
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
	FAUSTFLOAT fHslider0;
	double fRec1[2];
	double fRec2[2];
	FAUSTFLOAT fVbargraph0;
	double fRec3[2];
	FAUSTFLOAT fVbargraph1;
	double fRec4[2];
	FAUSTFLOAT fVbargraph2;
	double fRec5[2];
	FAUSTFLOAT fVbargraph3;
	double fRec6[2];
	FAUSTFLOAT fVbargraph4;
	double fRec7[2];
	FAUSTFLOAT fVbargraph5;
	double fRec8[2];
	FAUSTFLOAT fVbargraph6;
	double fRec9[2];
	FAUSTFLOAT fVbargraph7;
	double fRec10[2];
	FAUSTFLOAT fVbargraph8;
	double fRec11[2];
	FAUSTFLOAT fVbargraph9;
	double fRec12[2];
	FAUSTFLOAT fVbargraph10;
	double fRec13[2];
	FAUSTFLOAT fVbargraph11;
	double fRec14[2];
	FAUSTFLOAT fVbargraph12;
	double fRec15[2];
	FAUSTFLOAT fVbargraph13;
	double fRec16[2];
	FAUSTFLOAT fVbargraph14;
	double fRec17[2];
	FAUSTFLOAT fVbargraph15;
	double fRec18[2];
	FAUSTFLOAT fVbargraph16;
	double fRec19[2];
	FAUSTFLOAT fVbargraph17;
	double fRec20[2];
	FAUSTFLOAT fVbargraph18;
	double fRec21[2];
	FAUSTFLOAT fVbargraph19;
	double fRec22[2];
	FAUSTFLOAT fVbargraph20;
	double fRec23[2];
	FAUSTFLOAT fVbargraph21;
	double fRec24[2];
	FAUSTFLOAT fVbargraph22;
	double fRec25[2];
	FAUSTFLOAT fVbargraph23;
	double fRec26[2];
	FAUSTFLOAT fVbargraph24;
	double fRec27[2];
	FAUSTFLOAT fVbargraph25;
	double fRec28[2];
	FAUSTFLOAT fVbargraph26;
	double fRec29[2];
	FAUSTFLOAT fVbargraph27;
	double fRec30[2];
	FAUSTFLOAT fVbargraph28;
	double fRec31[2];
	FAUSTFLOAT fVbargraph29;
	double fRec32[2];
	FAUSTFLOAT fVbargraph30;
	double fRec33[2];
	FAUSTFLOAT fVbargraph31;
	double fRec0[2];
	FAUSTFLOAT fVbargraph32;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	double fConst6;
	double fConst7;
	double fConst8;
	double fConst9;
	double fConst10;
	double fConst11;
	double fConst12;
	double fRec34[2];
	FAUSTFLOAT fVbargraph33;
	double fRec35[2];
	FAUSTFLOAT fVbargraph34;
	double fConst13;
	double fConst14;
	double fConst15;
	double fConst16;
	double fConst17;
	double fConst18;
	double fConst19;
	double fConst20;
	double fRec36[2];
	FAUSTFLOAT fVbargraph35;
	double fConst21;
	double fConst22;
	double fConst23;
	double fConst24;
	double fConst25;
	double fConst26;
	double fConst27;
	double fConst28;
	double fRec37[2];
	FAUSTFLOAT fVbargraph36;
	double fConst29;
	double fConst30;
	double fConst31;
	double fConst32;
	double fConst33;
	double fConst34;
	double fConst35;
	double fConst36;
	double fConst37;
	double fConst38;
	double fConst39;
	double fConst40;
	double fConst41;
	double fConst42;
	double fConst43;
	double fConst44;
	double fConst45;
	double fConst46;
	double fConst47;
	double fConst48;
	double fConst49;
	double fConst50;
	double fConst51;
	double fConst52;
	double fRec38[2];
	FAUSTFLOAT fVbargraph37;
	double fRec39[2];
	FAUSTFLOAT fVbargraph38;
	double fConst53;
	double fConst54;
	double fConst55;
	double fConst56;
	double fConst57;
	double fConst58;
	double fConst59;
	double fConst60;
	double fConst61;
	double fConst62;
	double fConst63;
	double fConst64;
	double fConst65;
	double fConst66;
	double fConst67;
	double fConst68;
	double fConst69;
	double fConst70;
	double fRec40[2];
	FAUSTFLOAT fVbargraph39;
	double fConst71;
	double fConst72;
	double fConst73;
	double fConst74;
	double fConst75;
	double fConst76;
	double fConst77;
	double fConst78;
	double fConst79;
	double fRec41[2];
	FAUSTFLOAT fVbargraph40;
	double fConst80;
	double fConst81;
	double fConst82;
	double fConst83;
	double fConst84;
	double fConst85;
	double fConst86;
	double fConst87;
	double fConst88;
	double fConst89;
	double fConst90;
	double fRec42[2];
	FAUSTFLOAT fVbargraph41;
	double fConst91;
	double fConst92;
	double fConst93;
	double fConst94;
	double fConst95;
	double fConst96;
	double fConst97;
	double fConst98;
	double fConst99;
	double fConst100;
	double fConst101;
	double fRec43[2];
	FAUSTFLOAT fVbargraph42;
	double fConst102;
	double fConst103;
	double fConst104;
	double fConst105;
	double fConst106;
	double fConst107;
	double fConst108;
	double fConst109;
	double fConst110;
	double fConst111;
	double fConst112;
	double fConst113;
	double fConst114;
	double fConst115;
	double fConst116;
	double fConst117;
	double fConst118;
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
	double fConst131;
	double fConst132;
	double fConst133;
	double fRec44[2];
	FAUSTFLOAT fVbargraph43;
	double fRec45[2];
	FAUSTFLOAT fVbargraph44;
	double fConst134;
	double fConst135;
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
	double fRec46[2];
	FAUSTFLOAT fVbargraph45;
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
	double fConst171;
	double fConst172;
	double fConst173;
	double fConst174;
	double fConst175;
	double fConst176;
	double fConst177;
	double fConst178;
	double fConst179;
	double fConst180;
	double fRec47[2];
	FAUSTFLOAT fVbargraph46;
	double fConst181;
	double fConst182;
	double fConst183;
	double fConst184;
	double fConst185;
	double fConst186;
	double fConst187;
	double fConst188;
	double fRec48[2];
	FAUSTFLOAT fVbargraph47;
	double fConst189;
	double fConst190;
	double fConst191;
	double fConst192;
	double fConst193;
	double fConst194;
	double fConst195;
	double fConst196;
	double fRec49[2];
	FAUSTFLOAT fVbargraph48;
	double fConst197;
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
	double fConst210;
	double fConst211;
	double fConst212;
	double fConst213;
	double fConst214;
	double fConst215;
	double fConst216;
	double fConst217;
	double fConst218;
	double fRec50[2];
	FAUSTFLOAT fVbargraph49;
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
	double fConst234;
	double fConst235;
	double fConst236;
	double fConst237;
	double fConst238;
	double fRec51[2];
	FAUSTFLOAT fVbargraph50;
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
	double fConst253;
	double fConst254;
	double fConst255;
	double fConst256;
	double fConst257;
	double fConst258;
	double fConst259;
	double fConst260;
	double fConst261;
	double fRec52[2];
	FAUSTFLOAT fVbargraph51;
	double fRec53[2];
	FAUSTFLOAT fVbargraph52;
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
	double fConst272;
	double fConst273;
	double fConst274;
	double fConst275;
	double fConst276;
	double fConst277;
	double fConst278;
	double fRec54[2];
	FAUSTFLOAT fVbargraph53;
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
	double fConst297;
	double fConst298;
	double fConst299;
	double fConst300;
	double fConst301;
	double fConst302;
	double fRec55[2];
	FAUSTFLOAT fVbargraph54;
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
	double fConst313;
	double fConst314;
	double fConst315;
	double fConst316;
	double fConst317;
	double fConst318;
	double fConst319;
	double fConst320;
	double fRec56[2];
	FAUSTFLOAT fVbargraph55;
	double fConst321;
	double fConst322;
	double fConst323;
	double fConst324;
	double fConst325;
	double fConst326;
	double fRec57[2];
	FAUSTFLOAT fVbargraph56;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2015");
		m->declare("eigenmike32.lib/author", "Pierre Lecomte");
		m->declare("eigenmike32.lib/copyright", "(c) Pierre Lecomte 2015");
		m->declare("eigenmike32.lib/license", "GPL");
		m->declare("eigenmike32.lib/name", "Pentakis dodecahedron grid");
		m->declare("eigenmike32.lib/version", "1.0");
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
		m->declare("name", "HOAEncEigenMike4");
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
		return 32;
		
	}
	virtual int getNumOutputs() {
		return 25;
		
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
		fConst1 = double(tgamma(1.0));
		fConst2 = double(tgamma(3.0));
		fConst3 = double(tgamma(1.5));
		fConst4 = (sqrt((3.0 * (fConst1 / fConst2))) * fConst3);
		fConst5 = (0.0 - (0.83894547537068676 * fConst4));
		fConst6 = (0.0 - (0.92131773170354936 * fConst4));
		fConst7 = (0.0 - (1.4907234049739491 * fConst4));
		fConst8 = (0.0 - (0.83894547432629885 * fConst4));
		fConst9 = (0.0 - (0.92131773191602406 * fConst4));
		fConst10 = (0.0 - (1.4907234045358599 * fConst4));
		fConst11 = (0.0 - (0.5694056722918005 * fConst4));
		fConst12 = (0.0 - (1.3574422932346608 * fConst4));
		fConst13 = (0.0 - (1.4907234046979638 * fConst4));
		fConst14 = (0.0 - (1.3574422936455703 * fConst4));
		fConst15 = (0.0 - (1.3574422934949071 * fConst4));
		fConst16 = (0.0 - (0.83894547484849291 * fConst4));
		fConst17 = (0.0 - (0.92131773189251487 * fConst4));
		fConst18 = (0.0 - (0.56940567317101876 * fConst4));
		fConst19 = (0.0 - (0.92131773172705878 * fConst4));
		fConst20 = (0.0 - (0.56940567290330546 * fConst4));
		fConst21 = double(tgamma(5.0));
		fConst22 = double(tgamma(2.5));
		fConst23 = (sqrt((5.0 * (fConst1 / fConst21))) * fConst22);
		fConst24 = (0.0 - (2.8545985866509747 * fConst23));
		fConst25 = (0.0 - (2.1276921620518694 * fConst23));
		fConst26 = (0.0 - (2.127692159770834 * fConst23));
		fConst27 = (0.0 - (2.8545985849673534 * fConst23));
		fConst28 = (0.0 - (2.127692162576873 * fConst23));
		fConst29 = double(tgamma(2.0));
		fConst30 = double(tgamma(4.0));
		fConst31 = sqrt((5.0 * (fConst29 / fConst30)));
		fConst32 = (0.049867785051350533 * fConst3);
		fConst33 = (0.0 - (1.595769121639145 * fConst3));
		fConst34 = (0.031250000000079749 * fConst33);
		fConst35 = (0.049867785014654206 * fConst3);
		fConst36 = (0.066904654355926602 * fConst3);
		fConst37 = (0.0 - (1.513879513216569 * fConst3));
		fConst38 = (0.044194173824159223 * fConst37);
		fConst39 = (0.0 - (1.1283791662916762 * fConst3));
		fConst40 = (0.044194173824159223 * fConst39);
		fConst41 = (0.031250000006010782 * fConst33);
		fConst42 = (0.049867785060815094 * fConst3);
		fConst43 = (fConst31 * fConst33);
		fConst44 = (0.0 - (0.99999999978213805 * fConst43));
		fConst45 = (fConst31 * fConst3);
		fConst46 = (0.0 - (1.5957691212914875 * fConst45));
		fConst47 = (0.0 - (1.5957691216595047 * fConst45));
		fConst48 = (0.0 - (1.0000000000127585 * fConst43));
		fConst49 = (0.0 - (1.5957691204689346 * fConst45));
		fConst50 = (0.0 - (2.1409489393896513 * fConst45));
		fConst51 = (0.0 - (1.4142135623730951 * (fConst31 * fConst37)));
		fConst52 = (0.0 - (1.4142135623730951 * (fConst31 * fConst39)));
		fConst53 = (0.049867785048675617 * fConst3);
		fConst54 = (0.0 - (1.1283791670614929 * fConst3));
		fConst55 = (0.044194173824159223 * fConst54);
		fConst56 = (0.066904654348894713 * fConst3);
		fConst57 = (0.049867785051096042 * fConst3);
		fConst58 = (0.031249999999920262 * fConst33);
		fConst59 = (0.0 - (1.5138795130574554 * fConst3));
		fConst60 = (0.044194173824159223 * fConst59);
		fConst61 = (0.031250000006808193 * fConst33);
		fConst62 = (0.049867785062087583 * fConst3);
		fConst63 = (0.0 - (1.5957691215576197 * fConst45));
		fConst64 = (0.0 - (1.4142135623730951 * (fConst31 * fConst54)));
		fConst65 = (0.0 - (2.1409489391646308 * fConst45));
		fConst66 = (0.0 - (1.5957691216187855 * fConst45));
		fConst67 = (0.0 - (0.99999999998724154 * fConst43));
		fConst68 = (0.0 - (1.4142135623730951 * (fConst31 * fConst59)));
		fConst69 = (0.0 - (0.99999999980765519 * fConst43));
		fConst70 = (0.0 - (1.5957691213322069 * fConst45));
		fConst71 = (0.0 - (1.0858147483458616e-11 * fConst23));
		fConst72 = (0.0 - (2.3788321562260699 * fConst23));
		fConst73 = (0.0 - (2.3788321569902795 * fConst23));
		fConst74 = (0.0 - (0.4063530434926958 * fConst23));
		fConst75 = (0.0 - (2.3094187680584044 * fConst23));
		fConst76 = (0.0 - (5.4292390966272419e-11 * fConst23));
		fConst77 = (0.0 - (2.378832153716274 * fConst23));
		fConst78 = (0.0 - (8.1850154790365642e-10 * fConst23));
		fConst79 = (0.0 - (2.3788321544804822 * fConst23));
		fConst80 = double(tgamma(7.0));
		fConst81 = double(tgamma(3.5));
		fConst82 = (sqrt((7.0 * (fConst1 / fConst80))) * fConst81);
		fConst83 = (0.0 - (6.3572844778421267 * fConst82));
		fConst84 = (0.0 - (2.9260633694272102 * fConst82));
		fConst85 = (0.0 - (2.4568472865809237 * fConst82));
		fConst86 = (0.0 - (0.28999148408756475 * fConst82));
		fConst87 = (0.0 - (3.9290178837373331 * fConst82));
		fConst88 = (0.0 - (6.3572844791652345 * fConst82));
		fConst89 = (0.0 - (2.4568472848811247 * fConst82));
		fConst90 = (0.0 - (2.9260633624460199 * fConst82));
		fConst91 = double(tgamma(6.0));
		fConst92 = sqrt((7.0 * (fConst29 / fConst91)));
		fConst93 = (0.19194119415074193 * fConst22);
		fConst94 = (0.0 - (4.3431334391370644 * fConst22));
		fConst95 = (0.04419417382415923 * fConst94);
		fConst96 = (0.19194119415074187 * fConst22);
		fConst97 = (0.044194173824159223 * fConst94);
		fConst98 = (fConst92 * fConst94);
		fConst99 = (0.0 - (1.4142135623730951 * fConst98));
		fConst100 = (fConst92 * fConst22);
		fConst101 = (0.0 - (6.1421182128237399 * fConst100));
		fConst102 = sqrt((7.0 * (fConst2 / fConst21)));
		fConst103 = (0.028791179128110564 * fConst3);
		fConst104 = (0.0 - (3.3851375012865379 * fConst3));
		fConst105 = (0.011617126076936305 * fConst104);
		fConst106 = (0.020642487809373531 * fConst104);
		fConst107 = (0.0 - ((2.7639531957128085 * fConst3) + (2.8867513460690217 * fConst33)));
		fConst108 = (0.015625000000039874 * fConst107);
		fConst109 = (0.089771875033956811 * fConst3);
		fConst110 = (0.024304538356264491 * fConst3);
		fConst111 = (0.0 - ((2.6286555606082351 * fConst37) + (2.8795699518469395 * fConst3)));
		fConst112 = (0.022097086912079612 * fConst111);
		fConst113 = (0.0 - ((4.6708617953819722 * fConst39) + (1.2078918363708513 * fConst3)));
		fConst114 = (0.022097086912079612 * fConst113);
		fConst115 = (0.011617126071105382 * fConst104);
		fConst116 = (0.015625000003005391 * fConst107);
		fConst117 = (0.020642487806139163 * fConst104);
		fConst118 = (0.028791179133574932 * fConst3);
		fConst119 = (fConst102 * fConst104);
		fConst120 = (0.0 - (0.37174803463013312 * fConst119));
		fConst121 = (fConst102 * fConst107);
		fConst122 = (0.0 - (0.49999999989106902 * fConst121));
		fConst123 = (0.0 - (0.66055960994526475 * fConst119));
		fConst124 = (fConst102 * fConst3);
		fConst125 = (0.0 - (0.92131773189646693 * fConst124));
		fConst126 = (0.0 - (0.3717480341673508 * fConst119));
		fConst127 = (0.0 - (0.92131773210894174 * fConst124));
		fConst128 = (0.0 - (0.66055960975114159 * fConst119));
		fConst129 = (0.0 - (0.50000000000637923 * fConst121));
		fConst130 = (0.0 - (2.8727000010866179 * fConst124));
		fConst131 = (0.0 - (0.7777452274004637 * fConst124));
		fConst132 = (0.0 - (0.70710678118654757 * (fConst102 * fConst111)));
		fConst133 = (0.0 - (0.70710678118654757 * (fConst102 * fConst113)));
		fConst134 = (0.018796904843951478 * fConst104);
		fConst135 = (0.0 - (1.1491488228408451 * fConst3));
		fConst136 = (0.022097086912079612 * fConst135);
		fConst137 = (0.0 - ((1.7841104488024717 * fConst54) + (3.1623018850062845 * fConst3)));
		fConst138 = (0.022097086912079612 * fConst137);
		fConst139 = (0.018796904840703489 * fConst104);
		fConst140 = (0.10295567663646205 * fConst3);
		fConst141 = (0.028791179127963633 * fConst3);
		fConst142 = (0.007884728723289422 * fConst104);
		fConst143 = (0.015624999999960131 * fConst107);
		fConst144 = (0.0 - ((4.253254042028713 * fConst59) + (1.7796721029334124 * fConst3)));
		fConst145 = (0.022097086912079612 * fConst144);
		fConst146 = (0.015625000003404096 * fConst107);
		fConst147 = (0.0078847287195823092 * fConst104);
		fConst148 = (0.028791179134309602 * fConst3);
		fConst149 = (0.0 - (0.70710678118654757 * (fConst102 * fConst135)));
		fConst150 = (0.0 - (0.60150095518852686 * fConst119));
		fConst151 = (0.0 - (0.70710678118654757 * (fConst102 * fConst137)));
		fConst152 = (0.0 - (0.60150095512176593 * fConst119));
		fConst153 = (0.0 - (3.2945816523667855 * fConst124));
		fConst154 = (0.0 - (0.92131773208543244 * fConst124));
		fConst155 = (0.0 - (0.25231131953485497 * fConst119));
		fConst156 = (0.0 - (0.49999999999362077 * fConst121));
		fConst157 = (0.0 - (0.70710678118654757 * (fConst102 * fConst144)));
		fConst158 = (0.0 - (0.4999999999038276 * fConst121));
		fConst159 = (0.0 - (0.25231131941622759 * fConst119));
		fConst160 = (0.0 - (0.92131773191997635 * fConst124));
		fConst161 = (0.1552836879846895 * fConst22);
		fConst162 = (0.0 - (3.5136687610121586 * fConst22));
		fConst163 = (0.044194173824159223 * fConst162);
		fConst164 = (0.11724619443659033 * fConst22);
		fConst165 = (0.0 - (2.6529785329417432 * fConst22));
		fConst166 = (0.044194173824159223 * fConst165);
		fConst167 = (1.9256467651072228e-11 * fConst94);
		fConst168 = (8.3633408575032958e-11 * fConst22);
		fConst169 = (0.0 - (3.1344771863690116e-11 * fConst100));
		fConst170 = (0.0 - (7.2170869955858409e-12 * fConst98));
		fConst171 = (0.0 - (1.8980189062872217 * fConst100));
		fConst172 = (0.0 - (6.0706664864297446 * fConst100));
		fConst173 = (0.0 - (4.2926094388763847 * fConst22));
		fConst174 = (0.0 - (1.4142135623730951 * (fConst92 * fConst173)));
		fConst175 = (0.0 - (1.3421020394559686 * fConst22));
		fConst176 = (0.0 - (1.4142135623730951 * (fConst92 * fConst175)));
		fConst177 = (0.0 - (1.5672863270319247e-10 * fConst100));
		fConst178 = (0.0 - (3.6086534042650283e-11 * fConst98));
		fConst179 = (0.0 - (5.4403358273051902e-10 * fConst98));
		fConst180 = (0.0 - (2.3628104451704574e-09 * fConst100));
		fConst181 = (0.0 - (0.57323598386659269 * fConst82));
		fConst182 = (0.0 - (0.57323598919882257 * fConst82));
		fConst183 = (0.0 - (2.4568472849939691 * fConst82));
		fConst184 = (0.0 - (5.6729021311513463 * fConst82));
		fConst185 = (0.0 - (2.4568472833694006 * fConst82));
		fConst186 = (0.0 - (5.6729021295749007 * fConst82));
		fConst187 = (0.0 - (5.203686055152577 * fConst82));
		fConst188 = (0.0 - (0.92751530491955814 * fConst82));
		fConst189 = (double(tgamma(4.5)) * sqrt((9.0 * (fConst1 / double(tgamma(9.0))))));
		fConst190 = (0.0 - (10.212922369619969 * fConst189));
		fConst191 = (0.0 - (5.7910350987853847e-11 * fConst189));
		fConst192 = (0.0 - (12.687104824167649 * fConst189));
		fConst193 = (0.0 - (4.9444630204394806e-09 * fConst189));
		fConst194 = (0.0 - (10.212922387690494 * fConst189));
		fConst195 = (0.0 - (2.8955977255842764e-10 * fConst189));
		fConst196 = (0.0 - (12.687104827514046 * fConst189));
		fConst197 = sqrt((9.0 * (fConst29 / double(tgamma(8.0)))));
		fConst198 = (0.0 - (9.9292300897832995 * fConst81));
		fConst199 = (fConst197 * fConst198);
		fConst200 = (0.0 - (1.0000000006535863 * fConst199));
		fConst201 = (fConst81 * fConst197);
		fConst202 = (0.0 - (9.9292300962729083 * fConst201));
		fConst203 = (0.0 - (1.8963142016150152 * fConst201));
		fConst204 = (0.0 - (14.459248590941478 * fConst201));
		fConst205 = (0.0 - (10.22423272951675 * fConst81));
		fConst206 = (0.0 - (1.4142135623730951 * (fConst197 * fConst205)));
		fConst207 = (0.0 - (1.3408966312223309 * fConst81));
		fConst208 = (0.0 - (1.4142135623730951 * (fConst197 * fConst207)));
		fConst209 = (0.0 - (9.9292300894032515 * fConst201));
		fConst210 = (0.0 - (0.9999999999617244 * fConst199));
		fConst211 = (0.31028844030335295 * fConst81);
		fConst212 = (0.031249999999760792 * fConst198);
		fConst213 = (0.031249999981967664 * fConst198);
		fConst214 = (0.31028844012668089 * fConst81);
		fConst215 = (0.059259818800469224 * fConst81);
		fConst216 = (0.45185151846692118 * fConst81);
		fConst217 = (0.044194173824159223 * fConst205);
		fConst218 = (0.044194173824159223 * fConst207);
		fConst219 = sqrt((9.0 * (fConst2 / fConst80)));
		fConst220 = (0.2216346002462225 * fConst22);
		fConst221 = (0.0 - (11.283791670955125 * fConst22));
		fConst222 = (0.019764235376110769 * fConst221);
		fConst223 = (0.014731391264225374 * fConst221);
		fConst224 = (0.0 - ((7.5225277803217159 * fConst22) + (4.041451884496631 * fConst94)));
		fConst225 = (0.022097086912079615 * fConst224);
		fConst226 = (0.019764235366430384 * fConst221);
		fConst227 = (0.22163460024622245 * fConst22);
		fConst228 = (0.014731391283653389 * fConst221);
		fConst229 = (0.022097086912079612 * fConst224);
		fConst230 = (fConst219 * fConst221);
		fConst231 = (0.0 - (0.63245553221237039 * fConst230));
		fConst232 = (fConst219 * fConst224);
		fConst233 = (0.0 - (0.70710678118654757 * fConst232));
		fConst234 = (0.0 - (0.47140452026591112 * fConst230));
		fConst235 = (fConst22 * fConst219);
		fConst236 = (0.0 - (7.0923072078791183 * fConst235));
		fConst237 = (0.0 - (0.63245553183935277 * fConst230));
		fConst238 = (0.0 - (0.47140452088760798 * fConst230));
		fConst239 = sqrt((9.0 * (fConst30 / fConst91)));
		fConst240 = (0.12918856329855402 * fConst3);
		fConst241 = (0.0 - (2.6596152019522412 * fConst3));
		fConst242 = (0.0 - ((2.0207259422483155 * fConst107) + (4.0 * fConst33)));
		fConst243 = (0.0 - (4.0316412356782187 * fConst3));
		fConst244 = (0.01473139127471974 * fConst243);
		fConst245 = (0.0 - ((1.8400588924257646 * fConst111) + (4.0 * fConst37)));
		fConst246 = (0.01473139127471974 * fConst245);
		fConst247 = (0.0 - ((3.2696032567673803 * fConst113) + (4.0 * fConst39)));
		fConst248 = (0.01473139127471974 * fConst247);
		fConst249 = (0.01041666666867026 * fConst242);
		fConst250 = (0.01041666666867026 * fConst241);
		fConst251 = (fConst239 * fConst242);
		fConst252 = (0.0 - (0.33333333326071263 * fConst251));
		fConst253 = (fConst239 * fConst241);
		fConst254 = (0.0 - (0.33333333326071263 * fConst253));
		fConst255 = (0.0 - (0.33333333333758614 * fConst253));
		fConst256 = (0.0 - (0.33333333333758614 * fConst251));
		fConst257 = (fConst3 * fConst239);
		fConst258 = (0.0 - (4.1340340255537287 * fConst257));
		fConst259 = (0.0 - (0.47140452079103168 * (fConst239 * fConst243)));
		fConst260 = (0.0 - (0.47140452079103168 * (fConst239 * fConst245)));
		fConst261 = (0.0 - (0.47140452079103168 * (fConst239 * fConst247)));
		fConst262 = (0.11514556317659597 * fConst3);
		fConst263 = ((1.2488773141617302 * fConst135) - (4.5135166682459715 * fConst3));
		fConst264 = (0.0 - ((1.2488773141617302 * fConst137) + (4.0 * fConst54)));
		fConst265 = (0.010416666666640087 * fConst241);
		fConst266 = (0.010416666666640087 * fConst242);
		fConst267 = (0.0 - ((2.9772778294200997 * fConst144) + (4.0 * fConst59)));
		fConst268 = (0.01473139127471974 * fConst267);
		fConst269 = (0.010416666668936063 * fConst242);
		fConst270 = (0.010416666668936063 * fConst241);
		fConst271 = (0.0 - (0.47140452079103168 * (fConst239 * fConst263)));
		fConst272 = (0.0 - (0.47140452079103168 * (fConst239 * fConst264)));
		fConst273 = (0.0 - (3.6846580216510709 * fConst257));
		fConst274 = (0.0 - (0.33333333332908049 * fConst253));
		fConst275 = (0.0 - (0.33333333332908049 * fConst251));
		fConst276 = (0.0 - (0.47140452079103168 * (fConst239 * fConst267)));
		fConst277 = (0.0 - (0.33333333326921838 * fConst251));
		fConst278 = (0.0 - (0.33333333326921838 * fConst253));
		fConst279 = (0.0098821176879093937 * fConst221);
		fConst280 = (0.022097086912079612 * (0.0 - (1.0708342953989121 * fConst22)));
		fConst281 = (0.022097086912079612 * (0.0 - ((2.4977546283234604 * fConst162) + (9.8471167056125903 * fConst22))));
		fConst282 = (0.009882117676857774 * fConst221);
		fConst283 = (0.28015891130704573 * fConst22);
		fConst284 = (0.022097086912079612 * (0.0 - ((5.9545556588401993 * fConst165) + (3.1187633124264642 * fConst22))));
		fConst285 = (9.6282338255361139e-12 * fConst224);
		fConst286 = (9.6571541918193163e-11 * fConst22);
		fConst287 = (0.0098821177072701549 * fConst221);
		fConst288 = (0.0098821177001713802 * fConst221);
		fConst289 = (0.0 - (3.6193824950167161e-11 * fConst235));
		fConst290 = (0.0 - (0.52704627699509621 * fConst230));
		fConst291 = (0.0 - (3.6085434977929205e-12 * fConst232));
		fConst292 = (0.0 - (0.52704627716441199 * fConst230));
		fConst293 = (0.0 - (5.1898861886710215 * fConst235));
		fConst294 = (0.0 - (5.3968369311601121 * fConst235));
		fConst295 = (0.0 - (0.70710678118654757 * (fConst219 * (0.0 - ((3.6801177848515292 * fConst173) + (8.1650283574679019 * fConst22))))));
		fConst296 = (0.0 - (0.70710678118654757 * (fConst219 * (0.0 - ((6.5392065135347606 * fConst175) + (1.4366749630473863 * fConst22))))));
		fConst297 = (0.0 - (1.8097463658076773e-10 * fConst235));
		fConst298 = (0.0 - (0.52704627643903401 * fConst230));
		fConst299 = (0.0 - (1.8043267021325141e-11 * fConst232));
		fConst300 = (0.0 - (2.7201679136525951e-10 * fConst232));
		fConst301 = (0.0 - (0.52704627660834946 * fConst230));
		fConst302 = (0.0 - (2.7283384934120938e-09 * fConst235));
		fConst303 = (0.0 - (9.9292300898593062 * fConst201));
		fConst304 = (0.0 - (1.000000000007655 * fConst199));
		fConst305 = (0.0 - (0.99999999934641381 * fConst199));
		fConst306 = (0.0 - (9.9292300832936924 * fConst201));
		fConst307 = (0.0 - (12.997530928599598 * fConst201));
		fConst308 = (0.0 - (9.190642258294659 * fConst81));
		fConst309 = (0.0 - (1.4142135623730951 * (fConst197 * fConst308)));
		fConst310 = (0.0 - (5.5229415075696471 * fConst201));
		fConst311 = (0.0 - (3.9053093920991513 * fConst81));
		fConst312 = (0.0 - (1.4142135623730951 * (fConst197 * fConst311)));
		fConst313 = (0.40617284151873745 * fConst81);
		fConst314 = (0.044194173824159223 * fConst308);
		fConst315 = (0.17259192211155147 * fConst81);
		fConst316 = (0.044194173824159223 * fConst311);
		fConst317 = (0.31028844031760461 * fConst81);
		fConst318 = (0.031250000001196113 * fConst198);
		fConst319 = (0.03125000001803234 * fConst198);
		fConst320 = (0.31028844048477533 * fConst81);
		fConst321 = (0.0 - (7.659691783828209 * fConst189));
		fConst322 = (0.0 - (7.6596917952497838 * fConst189));
		fConst323 = (0.0 - (5.6738457652340379 * fConst189));
		fConst324 = (0.0 - (5.6738457652340371 * fConst189));
		fConst325 = (0.0 - (7.6596917638193442 * fConst189));
		fConst326 = (0.0 - (7.6596917711557539 * fConst189));
		
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.0);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec1[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec2[l1] = 0.0;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec3[l2] = 0.0;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec4[l3] = 0.0;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec5[l4] = 0.0;
			
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec6[l5] = 0.0;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec7[l6] = 0.0;
			
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec8[l7] = 0.0;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec9[l8] = 0.0;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fRec10[l9] = 0.0;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec11[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec12[l11] = 0.0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec13[l12] = 0.0;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec14[l13] = 0.0;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec15[l14] = 0.0;
			
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec16[l15] = 0.0;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fRec17[l16] = 0.0;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec18[l17] = 0.0;
			
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec19[l18] = 0.0;
			
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec20[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec21[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec22[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec23[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec24[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec25[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec26[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec27[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec28[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec29[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec30[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec31[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec32[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec33[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec0[l33] = 0.0;
			
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
			fRec37[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec38[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec39[l39] = 0.0;
			
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
		ui_interface->openVerticalBox("HOAEncEigenMike4");
		ui_interface->declare(0, "0", "");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31a29c0", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31a7510", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x319de70", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3199320", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31947d0", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x318fc80", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x318b130", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31865e0", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3181a90", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x317cf40", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31783f0", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31738a0", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x316ed50", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x316a200", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31656b0", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3160b60", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x315c010", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31574c0", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3152970", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x314de20", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31492d0", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3144780", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x313fc30", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x313b0e0", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3136590", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3131a40", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x312cef0", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31283a0", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3123850", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x311ed00", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x311a1b0", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3115570", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("Outputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31af220", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31c1ee0", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31cb700", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31db580", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31e9ce0", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31fece0", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3209cb0", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x321b7f0", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x322e1d0", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3240850", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x324c970", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3268170", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x326ee40", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3287a10", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x329cc50", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32acff0", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32bc6a0", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32d07d0", &fVbargraph49, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32e3190", &fVbargraph50, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32f8290", &fVbargraph51, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32ff6b0", &fVbargraph52, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3311740", &fVbargraph53, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x332d620", &fVbargraph54, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x333f6a0", &fVbargraph55, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3350900", &fVbargraph56, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(&fHslider0, "1", "");
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->declare(&fHslider0, "unit", "dB");
		ui_interface->addHorizontalSlider("Gain", &fHslider0, 0.0, -10.0, 50.0, 0.10000000000000001);
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
		double fSlow0 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fHslider0))));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			double fTemp0 = double(input31[i]);
			fRec2[0] = max((fRec2[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fVbargraph0 = FAUSTFLOAT(fRec2[0]);
			double fTemp1 = double(input30[i]);
			fRec3[0] = max((fRec3[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp1))))));
			fVbargraph1 = FAUSTFLOAT(fRec3[0]);
			double fTemp2 = double(input29[i]);
			fRec4[0] = max((fRec4[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp2))))));
			fVbargraph2 = FAUSTFLOAT(fRec4[0]);
			double fTemp3 = double(input28[i]);
			fRec5[0] = max((fRec5[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp3))))));
			fVbargraph3 = FAUSTFLOAT(fRec5[0]);
			double fTemp4 = double(input27[i]);
			fRec6[0] = max((fRec6[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fVbargraph4 = FAUSTFLOAT(fRec6[0]);
			double fTemp5 = double(input26[i]);
			fRec7[0] = max((fRec7[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fVbargraph5 = FAUSTFLOAT(fRec7[0]);
			double fTemp6 = double(input25[i]);
			fRec8[0] = max((fRec8[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fVbargraph6 = FAUSTFLOAT(fRec8[0]);
			double fTemp7 = double(input24[i]);
			fRec9[0] = max((fRec9[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp7))))));
			fVbargraph7 = FAUSTFLOAT(fRec9[0]);
			double fTemp8 = double(input23[i]);
			fRec10[0] = max((fRec10[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fVbargraph8 = FAUSTFLOAT(fRec10[0]);
			double fTemp9 = double(input22[i]);
			fRec11[0] = max((fRec11[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph9 = FAUSTFLOAT(fRec11[0]);
			double fTemp10 = double(input21[i]);
			fRec12[0] = max((fRec12[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fVbargraph10 = FAUSTFLOAT(fRec12[0]);
			double fTemp11 = double(input20[i]);
			fRec13[0] = max((fRec13[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fVbargraph11 = FAUSTFLOAT(fRec13[0]);
			double fTemp12 = double(input19[i]);
			fRec14[0] = max((fRec14[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph12 = FAUSTFLOAT(fRec14[0]);
			double fTemp13 = double(input18[i]);
			fRec15[0] = max((fRec15[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fVbargraph13 = FAUSTFLOAT(fRec15[0]);
			double fTemp14 = double(input17[i]);
			fRec16[0] = max((fRec16[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fVbargraph14 = FAUSTFLOAT(fRec16[0]);
			double fTemp15 = double(input16[i]);
			fRec17[0] = max((fRec17[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph15 = FAUSTFLOAT(fRec17[0]);
			double fTemp16 = double(input15[i]);
			fRec18[0] = max((fRec18[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph16 = FAUSTFLOAT(fRec18[0]);
			double fTemp17 = double(input14[i]);
			fRec19[0] = max((fRec19[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph17 = FAUSTFLOAT(fRec19[0]);
			double fTemp18 = double(input13[i]);
			fRec20[0] = max((fRec20[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph18 = FAUSTFLOAT(fRec20[0]);
			double fTemp19 = double(input12[i]);
			fRec21[0] = max((fRec21[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph19 = FAUSTFLOAT(fRec21[0]);
			double fTemp20 = double(input11[i]);
			fRec22[0] = max((fRec22[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph20 = FAUSTFLOAT(fRec22[0]);
			double fTemp21 = double(input10[i]);
			fRec23[0] = max((fRec23[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph21 = FAUSTFLOAT(fRec23[0]);
			double fTemp22 = double(input9[i]);
			fRec24[0] = max((fRec24[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph22 = FAUSTFLOAT(fRec24[0]);
			double fTemp23 = double(input8[i]);
			fRec25[0] = max((fRec25[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fVbargraph23 = FAUSTFLOAT(fRec25[0]);
			double fTemp24 = double(input7[i]);
			fRec26[0] = max((fRec26[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph24 = FAUSTFLOAT(fRec26[0]);
			double fTemp25 = double(input6[i]);
			fRec27[0] = max((fRec27[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph25 = FAUSTFLOAT(fRec27[0]);
			double fTemp26 = double(input5[i]);
			fRec28[0] = max((fRec28[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fVbargraph26 = FAUSTFLOAT(fRec28[0]);
			double fTemp27 = double(input4[i]);
			fRec29[0] = max((fRec29[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph27 = FAUSTFLOAT(fRec29[0]);
			double fTemp28 = double(input3[i]);
			fRec30[0] = max((fRec30[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fVbargraph28 = FAUSTFLOAT(fRec30[0]);
			double fTemp29 = double(input2[i]);
			fRec31[0] = max((fRec31[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph29 = FAUSTFLOAT(fRec31[0]);
			double fTemp30 = double(input0[i]);
			fRec32[0] = max((fRec32[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph30 = FAUSTFLOAT(fRec32[0]);
			double fTemp31 = double(input1[i]);
			fRec33[0] = max((fRec33[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph31 = FAUSTFLOAT(fRec33[0]);
			double fTemp32 = (0.03125 * (fRec1[0] * (fTemp0 + (fTemp1 + (fTemp2 + (fTemp3 + (fTemp4 + (fTemp5 + (fTemp6 + (fTemp7 + (fTemp8 + (fTemp9 + (fTemp10 + (fTemp11 + (fTemp12 + (fTemp13 + (fTemp14 + (fTemp15 + (fTemp16 + (fTemp17 + (fTemp18 + (fTemp19 + (fTemp20 + (fTemp21 + (fTemp22 + (fTemp23 + (fTemp24 + (fTemp25 + (fTemp26 + (fTemp27 + (fTemp28 + (fTemp29 + (fTemp30 + fTemp31)))))))))))))))))))))))))))))))));
			fRec0[0] = max((fRec0[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph32 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp32);
			double fTemp33 = (fRec1[0] * ((fConst4 * ((((((((((((0.026217046093473896 * fTemp31) + (0.02879117912208189 * fTemp26)) + (0.046585106402240353 * fTemp25)) + (0.02879117912208189 * fTemp24)) + (0.017793927259118766 * fTemp19)) + (0.04242007166358315 * fTemp18)) + (0.04242007166358315 * fTemp17)) + (0.017793927259118766 * fTemp16)) + (0.026217046080314912 * fTemp12)) + (0.028791179127546258 * fTemp6)) + (0.046585106394941164 * fTemp5)) + (0.028791179127546258 * fTemp4))) + (0.03125 * ((((((((((((fConst5 * fTemp28) + (fConst6 * fTemp22)) + (fConst7 * fTemp21)) + (fConst6 * fTemp20)) + (fConst8 * fTemp14)) + (fConst9 * fTemp10)) + (fConst10 * fTemp9)) + (fConst9 * fTemp8)) + (fConst11 * fTemp3)) + (fConst12 * fTemp2)) + (fConst12 * fTemp1)) + (fConst11 * fTemp0)))));
			fRec34[0] = max((fRec34[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph33 = FAUSTFLOAT(fRec34[0]);
			output1[i] = FAUSTFLOAT(fTemp33);
			double fTemp34 = (fTemp4 - (fTemp6 + (fTemp8 - (fTemp10 + (fTemp20 - (fTemp22 - (fTemp26 - fTemp24)))))));
			double fTemp35 = (fTemp13 - (fTemp15 + (fTemp30 - fTemp29)));
			double fTemp36 = (fTemp7 - (fTemp11 + (fTemp27 - fTemp23)));
			double fTemp37 = (fTemp1 - (fTemp2 + (fTemp18 - fTemp17)));
			double fTemp38 = (fTemp0 - (fTemp3 + (fTemp19 - fTemp16)));
			double fTemp39 = (fRec1[0] * ((0.031250000001308703 * fTemp34) - ((((0.019313562147752456 * fTemp35) + (0.046042825614321402 * fTemp36)) + (0.028456031166074459 * fTemp37)) + (0.050563562154587252 * fTemp38))));
			fRec35[0] = max((fRec35[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph34 = FAUSTFLOAT(fRec35[0]);
			output2[i] = FAUSTFLOAT(fTemp39);
			double fTemp40 = (fRec1[0] * ((fConst4 * ((((((((((((0.04658510639681137 * fTemp30) + (0.04242007166358315 * fTemp31)) + (0.04658510639681137 * fTemp29)) + (0.042420071656253222 * fTemp28)) + (0.026217046089015403 * fTemp27)) + (0.028791179121934959 * fTemp26)) + (0.017793927259118766 * fTemp25)) + (0.028791179121934959 * fTemp24)) + (0.026217046089015403 * fTemp23)) + (0.028791179128280928 * fTemp22)) + (0.017793927250752708 * fTemp21)) + (0.028791179128280928 * fTemp20))) + (0.03125 * ((((((((((((fConst13 * fTemp15) + (fConst14 * fTemp14)) + (fConst13 * fTemp13)) + (fConst15 * fTemp12)) + (fConst16 * fTemp11)) + (fConst17 * fTemp10)) + (fConst18 * fTemp9)) + (fConst17 * fTemp8)) + (fConst16 * fTemp7)) + (fConst19 * fTemp6)) + (fConst20 * fTemp5)) + (fConst19 * fTemp4)))));
			fRec36[0] = max((fRec36[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fVbargraph35 = FAUSTFLOAT(fRec36[0]);
			output3[i] = FAUSTFLOAT(fTemp40);
			double fTemp41 = (fRec1[0] * ((fConst23 * ((((((((0.089206205807902131 * fTemp31) + (0.066490380064120932 * fTemp26)) + (0.066490380019538955 * fTemp25)) + (0.066490380064120932 * fTemp24)) + (0.089206205764209553 * fTemp14)) + (0.066490380064120919 * fTemp10)) + (0.066490380107227617 * fTemp9)) + (0.066490380064120919 * fTemp8))) + (0.03125 * ((((((((fConst24 * fTemp28) + (fConst25 * fTemp22)) + (fConst26 * fTemp21)) + (fConst25 * fTemp20)) + (fConst27 * fTemp12)) + (fConst25 * fTemp6)) + (fConst28 * fTemp5)) + (fConst25 * fTemp4)))));
			fRec37[0] = max((fRec37[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph36 = FAUSTFLOAT(fRec37[0]);
			output4[i] = FAUSTFLOAT(fTemp41);
			double fTemp42 = (fRec1[0] * ((fConst31 * ((((((((fConst32 * fTemp26) + (fConst34 * fTemp24)) + (fConst35 * fTemp19)) + (fConst36 * fTemp18)) + (fConst38 * fTemp17)) + (fConst40 * fTemp16)) + (fConst41 * fTemp6)) + (fConst42 * fTemp4))) + (0.03125 * ((((((((fConst44 * fTemp22) + (fConst46 * fTemp20)) + (fConst47 * fTemp10)) + (fConst48 * fTemp8)) + (fConst49 * fTemp3)) + (fConst50 * fTemp2)) + (fConst51 * fTemp1)) + (fConst52 * fTemp0)))));
			fRec38[0] = max((fRec38[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fVbargraph37 = FAUSTFLOAT(fRec38[0]);
			output5[i] = FAUSTFLOAT(fTemp42);
			double fTemp43 = (fTemp7 + (fTemp11 + (fTemp27 + fTemp23)));
			double fTemp44 = (fTemp4 + (fTemp6 + (fTemp8 + (fTemp10 + (fTemp20 + (fTemp22 + (fTemp26 + fTemp24)))))));
			double fTemp45 = (fTemp0 + (fTemp3 + (fTemp19 + fTemp16)));
			double fTemp46 = (fTemp30 + fTemp29);
			double fTemp47 = (fTemp13 + (fTemp15 + fTemp46));
			double fTemp48 = (fTemp5 + (fTemp9 + (fTemp12 + (fTemp14 + (fTemp21 + (fTemp25 + (fTemp31 + fTemp28)))))));
			double fTemp49 = (fTemp1 + (fTemp2 + (fTemp18 + fTemp17)));
			double fTemp50 = (fRec1[0] * ((((0.040906781083793552 * fTemp43) + (2.9263569544132831e-12 * fTemp44)) + (0.056531781096479002 * fTemp45)) - (((0.021593218926725055 * fTemp47) + (0.034938562148434216 * fTemp48)) + (0.0059682189255058898 * fTemp49))));
			fRec39[0] = max((fRec39[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph38 = FAUSTFLOAT(fRec39[0]);
			output6[i] = FAUSTFLOAT(fTemp50);
			double fTemp51 = (fRec1[0] * ((fConst31 * ((((((((fConst53 * fTemp30) + (fConst55 * fTemp29)) + (fConst56 * fTemp27)) + (fConst57 * fTemp26)) + (fConst58 * fTemp24)) + (fConst60 * fTemp23)) + (fConst61 * fTemp22)) + (fConst62 * fTemp20))) + (0.03125 * ((((((((fConst63 * fTemp15) + (fConst64 * fTemp13)) + (fConst65 * fTemp11)) + (fConst66 * fTemp10)) + (fConst67 * fTemp8)) + (fConst68 * fTemp7)) + (fConst69 * fTemp6)) + (fConst70 * fTemp4)))));
			fRec40[0] = max((fRec40[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph39 = FAUSTFLOAT(fRec40[0]);
			output7[i] = FAUSTFLOAT(fTemp51);
			double fTemp52 = (fRec1[0] * ((fConst23 * ((((((((((((((0.087037037470924977 * fTemp30) + (0.044603102903292141 * fTemp31)) + (0.087037037470924977 * fTemp29)) + (0.044603102853410466 * fTemp28)) + (0.027566233589157149 * fTemp27)) + (0.027566233589157149 * fTemp23)) + (2.8971462571211461e-11 * fTemp22)) + (2.8971462571211461e-11 * fTemp20)) + (0.087037037470924977 * fTemp15)) + (0.044603102990677254 * fTemp14)) + (0.087037037470924977 * fTemp13)) + (0.044603102958636814 * fTemp12)) + (0.027566233589157149 * fTemp11)) + (0.027566233589157149 * fTemp7))) + (0.03125 * ((((((((((((((((((fConst71 * fTemp26) + (fConst72 * fTemp25)) + (fConst71 * fTemp24)) + (fConst73 * fTemp21)) + (fConst74 * fTemp19)) + (fConst75 * fTemp18)) + (fConst75 * fTemp17)) + (fConst74 * fTemp16)) + (fConst76 * fTemp10)) + (fConst77 * fTemp9)) + (fConst76 * fTemp8)) + (fConst78 * fTemp6)) + (fConst79 * fTemp5)) + (fConst78 * fTemp4)) + (fConst74 * fTemp3)) + (fConst75 * fTemp2)) + (fConst75 * fTemp1)) + (fConst74 * fTemp0)))));
			fRec41[0] = max((fRec41[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fVbargraph40 = FAUSTFLOAT(fRec41[0]);
			output8[i] = FAUSTFLOAT(fTemp52);
			double fTemp53 = (fRec1[0] * ((0.03125 * ((((((((((((fConst83 * fTemp28) + (fConst84 * fTemp25)) + (fConst85 * fTemp22)) + (fConst85 * fTemp20)) + (fConst86 * fTemp19)) + (fConst87 * fTemp18)) + (fConst87 * fTemp17)) + (fConst86 * fTemp16)) + (fConst88 * fTemp14)) + (fConst89 * fTemp10)) + (fConst89 * fTemp8)) + (fConst90 * fTemp5))) + (fConst82 * ((((((((((((0.19866513994759175 * fTemp31) + (0.076776477654886113 * fTemp26)) + (0.076776477654886113 * fTemp24)) + (0.09143948039011067 * fTemp21)) + (0.19866513996426247 * fTemp12)) + (0.09143947998092801 * fTemp9)) + (0.076776477611171123 * fTemp6)) + (0.076776477611171123 * fTemp4)) + (0.0090622338777363986 * fTemp3)) + (0.12278180886679166 * fTemp2)) + (0.12278180886679166 * fTemp1)) + (0.0090622338777363986 * fTemp0)))));
			fRec42[0] = max((fRec42[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fVbargraph41 = FAUSTFLOAT(fRec42[0]);
			output9[i] = FAUSTFLOAT(fTemp53);
			double fTemp54 = (fRec1[0] * ((fConst92 * ((((fConst93 * fTemp26) + (fConst95 * fTemp24)) + (fConst96 * fTemp10)) + (fConst97 * fTemp8))) + (0.03125 * ((((fConst99 * fTemp22) + (fConst101 * fTemp20)) + (fConst99 * fTemp6)) + (fConst101 * fTemp4)))));
			fRec43[0] = max((fRec43[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fVbargraph42 = FAUSTFLOAT(fRec43[0]);
			output10[i] = FAUSTFLOAT(fTemp54);
			double fTemp55 = (fRec1[0] * ((fConst102 * ((((((((((((fConst103 * fTemp26) + (fConst105 * fTemp31)) + (fConst106 * fTemp25)) + (fConst108 * fTemp24)) + (fConst109 * fTemp19)) + (fConst110 * fTemp18)) + (fConst112 * fTemp17)) + (fConst114 * fTemp16)) + (fConst115 * fTemp12)) + (fConst116 * fTemp6)) + (fConst117 * fTemp5)) + (fConst118 * fTemp4))) + (0.03125 * ((((((((((((fConst120 * fTemp28) + (fConst122 * fTemp22)) + (fConst123 * fTemp21)) + (fConst125 * fTemp20)) + (fConst126 * fTemp14)) + (fConst127 * fTemp10)) + (fConst128 * fTemp9)) + (fConst129 * fTemp8)) + (fConst130 * fTemp3)) + (fConst131 * fTemp2)) + (fConst132 * fTemp1)) + (fConst133 * fTemp0)))));
			fRec44[0] = max((fRec44[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph43 = FAUSTFLOAT(fRec44[0]);
			output11[i] = FAUSTFLOAT(fTemp55);
			double fTemp56 = (fRec1[0] * (((0.034862310657899916 * fTemp35) + (0.035165788928874113 * fTemp37)) - (((0.021733652816759153 * fTemp36) + (0.031823442324083143 * fTemp34)) + (0.052652155785676891 * fTemp38))));
			fRec45[0] = max((fRec45[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fVbargraph44 = FAUSTFLOAT(fRec45[0]);
			output12[i] = FAUSTFLOAT(fTemp56);
			double fTemp57 = (fRec1[0] * ((fConst102 * ((((((((((((fConst134 * fTemp31) + (fConst136 * fTemp30)) + (fConst138 * fTemp29)) + (fConst139 * fTemp28)) + (fConst140 * fTemp27)) + (fConst141 * fTemp26)) + (fConst142 * fTemp25)) + (fConst143 * fTemp24)) + (fConst145 * fTemp23)) + (fConst146 * fTemp22)) + (fConst147 * fTemp21)) + (fConst148 * fTemp20))) + (0.03125 * ((((((((((((fConst149 * fTemp15) + (fConst150 * fTemp14)) + (fConst151 * fTemp13)) + (fConst152 * fTemp12)) + (fConst153 * fTemp11)) + (fConst154 * fTemp10)) + (fConst155 * fTemp9)) + (fConst156 * fTemp8)) + (fConst157 * fTemp7)) + (fConst158 * fTemp6)) + (fConst159 * fTemp5)) + (fConst160 * fTemp4)))));
			fRec46[0] = max((fRec46[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fVbargraph45 = FAUSTFLOAT(fRec46[0]);
			output13[i] = FAUSTFLOAT(fTemp57);
			double fTemp58 = (fRec1[0] * ((fConst92 * ((((((((((fConst161 * fTemp30) + (fConst163 * fTemp29)) + (fConst164 * fTemp27)) + (fConst166 * fTemp23)) + (fConst167 * fTemp22)) + (fConst168 * fTemp20)) + (fConst161 * fTemp15)) + (fConst163 * fTemp13)) + (fConst164 * fTemp11)) + (fConst166 * fTemp7))) + (0.03125 * ((((((((((((((fConst169 * fTemp26) + (fConst170 * fTemp24)) + (fConst171 * fTemp19)) + (fConst172 * fTemp18)) + (fConst174 * fTemp17)) + (fConst176 * fTemp16)) + (fConst177 * fTemp10)) + (fConst178 * fTemp8)) + (fConst179 * fTemp6)) + (fConst180 * fTemp4)) + (fConst171 * fTemp3)) + (fConst172 * fTemp2)) + (fConst174 * fTemp1)) + (fConst176 * fTemp0)))));
			fRec47[0] = max((fRec47[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fVbargraph46 = FAUSTFLOAT(fRec47[0]);
			output14[i] = FAUSTFLOAT(fTemp58);
			double fTemp59 = (fRec1[0] * ((0.03125 * ((((((((((((fConst181 * fTemp31) + (fConst182 * fTemp28)) + (fConst183 * fTemp26)) + (fConst184 * fTemp25)) + (fConst183 * fTemp24)) + (fConst185 * fTemp22)) + (fConst186 * fTemp21)) + (fConst185 * fTemp20)) + (fConst187 * fTemp15)) + (fConst187 * fTemp13)) + (fConst188 * fTemp11)) + (fConst188 * fTemp7))) + (fConst82 * (((((((((((0.028984853278736192 * fTemp27) + (0.16261518922351803 * fTemp46)) + (0.028984853278736192 * fTemp23)) + (0.017913624203916748 * fTemp14)) + (0.017913624310949439 * fTemp12)) + (0.076776477658412487 * fTemp10)) + (0.17727819176027068 * fTemp9)) + (0.076776477658412487 * fTemp8)) + (0.07677647769977651 * fTemp6)) + (0.17727819171100689 * fTemp5)) + (0.07677647769977651 * fTemp4)))));
			fRec48[0] = max((fRec48[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fVbargraph47 = FAUSTFLOAT(fRec48[0]);
			output15[i] = FAUSTFLOAT(fTemp59);
			double fTemp60 = (fRec1[0] * ((0.03125 * ((((((((((fConst190 * fTemp28) + (fConst191 * fTemp26)) + (fConst192 * fTemp25)) + (fConst191 * fTemp24)) + (fConst193 * fTemp22)) + (fConst193 * fTemp20)) + (fConst194 * fTemp12)) + (fConst195 * fTemp10)) + (fConst196 * fTemp9)) + (fConst195 * fTemp8))) + (fConst189 * ((((((0.31915382431831724 * fTemp31) + (0.39647202572339707 * fTemp21)) + (0.31915382478727489 * fTemp14)) + (1.3641692798607456e-10 * fTemp6)) + (0.39647202582797197 * fTemp5)) + (1.3641692798607456e-10 * fTemp4)))));
			fRec49[0] = max((fRec49[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fVbargraph48 = FAUSTFLOAT(fRec49[0]);
			output16[i] = FAUSTFLOAT(fTemp60);
			double fTemp61 = (fRec1[0] * ((0.03125 * ((((((((fConst200 * fTemp22) + (fConst202 * fTemp20)) + (fConst203 * fTemp19)) + (fConst204 * fTemp18)) + (fConst206 * fTemp17)) + (fConst208 * fTemp16)) + (fConst209 * fTemp10)) + (fConst210 * fTemp8))) + (fConst197 * ((((((((fConst211 * fTemp26) + (fConst212 * fTemp24)) + (fConst213 * fTemp6)) + (fConst214 * fTemp4)) + (fConst215 * fTemp3)) + (fConst216 * fTemp2)) + (fConst217 * fTemp1)) + (fConst218 * fTemp0)))));
			fRec50[0] = max((fRec50[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fVbargraph49 = FAUSTFLOAT(fRec50[0]);
			output17[i] = FAUSTFLOAT(fTemp61);
			double fTemp62 = (fRec1[0] * ((fConst219 * ((((((((fConst220 * fTemp26) + (fConst222 * fTemp31)) + (fConst223 * fTemp25)) + (fConst225 * fTemp24)) + (fConst226 * fTemp14)) + (fConst227 * fTemp10)) + (fConst228 * fTemp9)) + (fConst229 * fTemp8))) + (0.03125 * ((((((((fConst231 * fTemp28) + (fConst233 * fTemp22)) + (fConst234 * fTemp21)) + (fConst236 * fTemp20)) + (fConst237 * fTemp12)) + (fConst233 * fTemp6)) + (fConst238 * fTemp5)) + (fConst236 * fTemp4)))));
			fRec51[0] = max((fRec51[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fVbargraph50 = FAUSTFLOAT(fRec51[0]);
			output18[i] = FAUSTFLOAT(fTemp62);
			double fTemp63 = (fRec1[0] * ((fConst239 * (((((((fConst240 * fTemp19) + (0.010416666666693247 * ((fConst241 * fTemp26) + (fConst242 * fTemp24)))) + (fConst244 * fTemp18)) + (fConst246 * fTemp17)) + (fConst248 * fTemp16)) + (fConst249 * fTemp6)) + (fConst250 * fTemp4))) + (0.03125 * ((((((((fConst252 * fTemp22) + (fConst254 * fTemp20)) + (fConst255 * fTemp10)) + (fConst256 * fTemp8)) + (fConst258 * fTemp3)) + (fConst259 * fTemp2)) + (fConst260 * fTemp1)) + (fConst261 * fTemp0)))));
			fRec52[0] = max((fRec52[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fVbargraph51 = FAUSTFLOAT(fRec52[0]);
			output19[i] = FAUSTFLOAT(fTemp63);
			double fTemp64 = (fRec1[0] * (((0.03515625 * fTemp48) + (0.040716809753480042 * fTemp45)) - (((0.036458333335514514 * fTemp44) + ((0.0029563930072167557 * fTemp47) + (0.0044761641722250133 * fTemp43))) + (0.030680085805992734 * fTemp49))));
			fRec53[0] = max((fRec53[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fVbargraph52 = FAUSTFLOAT(fRec53[0]);
			output20[i] = FAUSTFLOAT(fTemp64);
			double fTemp65 = (fRec1[0] * ((fConst239 * (((((((fConst262 * fTemp27) + (0.01473139127471974 * ((fConst263 * fTemp30) + (fConst264 * fTemp29)))) + (fConst265 * fTemp26)) + (fConst266 * fTemp24)) + (fConst268 * fTemp23)) + (fConst269 * fTemp22)) + (fConst270 * fTemp20))) + (0.03125 * ((((((((fConst271 * fTemp15) + (fConst272 * fTemp13)) + (fConst273 * fTemp11)) + (fConst274 * fTemp10)) + (fConst275 * fTemp8)) + (fConst276 * fTemp7)) + (fConst277 * fTemp6)) + (fConst278 * fTemp4)))));
			fRec54[0] = max((fRec54[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fVbargraph53 = FAUSTFLOAT(fRec54[0]);
			output21[i] = FAUSTFLOAT(fTemp65);
			double fTemp66 = (fRec1[0] * ((fConst219 * ((((((((((((((fConst279 * fTemp31) + (fConst280 * fTemp30)) + (fConst281 * fTemp29)) + (fConst282 * fTemp28)) + (fConst283 * fTemp27)) + (fConst284 * fTemp23)) + (fConst285 * fTemp22)) + (fConst286 * fTemp20)) + (fConst280 * fTemp15)) + (fConst287 * fTemp14)) + (fConst281 * fTemp13)) + (fConst288 * fTemp12)) + (fConst283 * fTemp11)) + (fConst284 * fTemp7))) + (0.03125 * ((((((((((((((((((fConst289 * fTemp26) + (fConst290 * fTemp25)) + (fConst291 * fTemp24)) + (fConst292 * fTemp21)) + (fConst293 * fTemp19)) + (fConst294 * fTemp18)) + (fConst295 * fTemp17)) + (fConst296 * fTemp16)) + (fConst297 * fTemp10)) + (fConst298 * fTemp9)) + (fConst299 * fTemp8)) + (fConst300 * fTemp6)) + (fConst301 * fTemp5)) + (fConst302 * fTemp4)) + (fConst293 * fTemp3)) + (fConst294 * fTemp2)) + (fConst295 * fTemp1)) + (fConst296 * fTemp0)))));
			fRec55[0] = max((fRec55[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fVbargraph54 = FAUSTFLOAT(fRec55[0]);
			output22[i] = FAUSTFLOAT(fTemp66);
			double fTemp67 = (fRec1[0] * ((0.03125 * ((((((((fConst303 * fTemp26) + (fConst304 * fTemp24)) + (fConst305 * fTemp22)) + (fConst306 * fTemp20)) + (fConst307 * fTemp15)) + (fConst309 * fTemp13)) + (fConst310 * fTemp11)) + (fConst312 * fTemp7))) + (fConst197 * ((((((((fConst313 * fTemp30) + (fConst314 * fTemp29)) + (fConst315 * fTemp27)) + (fConst316 * fTemp23)) + (fConst317 * fTemp10)) + (fConst318 * fTemp8)) + (fConst319 * fTemp6)) + (fConst320 * fTemp4)))));
			fRec56[0] = max((fRec56[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp67))))));
			fVbargraph55 = FAUSTFLOAT(fRec56[0]);
			output23[i] = FAUSTFLOAT(fTemp67);
			double fTemp68 = (fRec1[0] * ((0.03125 * ((((((((((((fConst321 * fTemp31) + (fConst322 * fTemp28)) + (fConst323 * fTemp26)) + (fConst323 * fTemp24)) + (fConst324 * fTemp22)) + (fConst324 * fTemp20)) + (fConst325 * fTemp14)) + (fConst326 * fTemp12)) + (fConst323 * fTemp10)) + (fConst323 * fTemp8)) + (fConst323 * fTemp6)) + (fConst323 * fTemp4))) + (fConst189 * (((((((((((((((((((0.030476478292642641 * fTemp27) + (0.30382122984176912 * fTemp46)) + (0.044326920549846299 * fTemp25)) + (0.030476478292642641 * fTemp23)) + (0.0443269208346505 * fTemp21)) + (0.0064672104551386778 * fTemp19)) + (0.20888888992654256 * fTemp18)) + (0.20888888992654256 * fTemp17)) + (0.0064672104551386778 * fTemp16)) + (0.30382122984176912 * fTemp15)) + (0.30382122984176912 * fTemp13)) + (0.030476478292642641 * fTemp11)) + (0.044326919614500547 * fTemp9)) + (0.030476478292642641 * fTemp7)) + (0.044326919899304089 * fTemp5)) + (0.0064672104551386778 * fTemp3)) + (0.20888888992654256 * fTemp2)) + (0.20888888992654256 * fTemp1)) + (0.0064672104551386778 * fTemp0)))));
			fRec57[0] = max((fRec57[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fVbargraph56 = FAUSTFLOAT(fRec57[0]);
			output24[i] = FAUSTFLOAT(fTemp68);
			fRec1[1] = fRec1[0];
			fRec2[1] = fRec2[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			fRec8[1] = fRec8[0];
			fRec9[1] = fRec9[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			fRec12[1] = fRec12[0];
			fRec13[1] = fRec13[0];
			fRec14[1] = fRec14[0];
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
			fRec0[1] = fRec0[0];
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
