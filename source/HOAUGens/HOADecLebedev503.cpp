/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOADecLebedev503"
version: "1.0"
Code generated with Faust 2.2.0 (http://faust.grame.fr)
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

// If other than 'faust2sc --prefix Faust' is used, sed this as well:
#if !defined(SC_FAUST_PREFIX)
#define SC_FAUST_PREFIX "Faust"
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
    
        /** Global init, calls the following methods :
         * - static class 'classInit' : static table initialisation
         * - 'instanceInit' : constants and instance table initialisation
         *
         * @param samplingRate - the sampling rate in Herz
         */
        virtual void init(int samplingRate) = 0;

        /** Init instance state
         *
         * @param samplingRate - the sampling rate in Hertz
         */
        virtual void instanceInit(int samplingRate) = 0;

        /** Init instance constant state
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
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value metadata).
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with sucessive in/out audio buffers.
         *
         * @param count - the nomber of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the nomber of frames to compute
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
        virtual void metadata(Meta* m) { return fDSP->metadata(m); }
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

#ifndef FAUST_UI_H
#define FAUST_UI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

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
    { }

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
    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) { }
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) { }

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

#include <math.h>

double pow(double dummy0, double dummy1);
double fabs(double dummy0);
double log10(double dummy0);
double mydsp_faustpower2_f(double value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
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
	double fRec9[2];
	double fRec8[2];
	double fRec7[2];
	FAUSTFLOAT fVbargraph2;
	double fConst4;
	double fConst5;
	double fRec12[2];
	double fRec13[2];
	double fRec11[2];
	double fRec10[2];
	FAUSTFLOAT fVbargraph3;
	double fRec16[2];
	double fRec17[2];
	double fRec15[2];
	double fRec14[2];
	FAUSTFLOAT fVbargraph4;
	double fRec20[2];
	double fRec21[2];
	double fRec19[2];
	double fRec18[2];
	FAUSTFLOAT fVbargraph5;
	double fConst6;
	double fConst7;
	double fConst8;
	double fRec25[2];
	double fRec26[2];
	double fRec24[2];
	double fConst9;
	double fRec27[2];
	double fRec23[2];
	double fRec22[2];
	FAUSTFLOAT fVbargraph6;
	double fRec31[2];
	double fRec32[2];
	double fRec30[2];
	double fRec33[2];
	double fRec29[2];
	double fRec28[2];
	FAUSTFLOAT fVbargraph7;
	double fRec37[2];
	double fRec38[2];
	double fRec36[2];
	double fRec39[2];
	double fRec35[2];
	double fRec34[2];
	FAUSTFLOAT fVbargraph8;
	double fRec43[2];
	double fRec44[2];
	double fRec42[2];
	double fRec45[2];
	double fRec41[2];
	double fRec40[2];
	FAUSTFLOAT fVbargraph9;
	double fRec0[2];
	FAUSTFLOAT fVbargraph10;
	double fRec46[2];
	FAUSTFLOAT fVbargraph11;
	double fRec50[2];
	double fRec49[2];
	double fRec48[2];
	FAUSTFLOAT fVbargraph12;
	double fRec53[2];
	double fRec54[2];
	double fRec52[2];
	double fRec51[2];
	FAUSTFLOAT fVbargraph13;
	double fRec58[2];
	double fRec59[2];
	double fRec57[2];
	double fRec60[2];
	double fRec56[2];
	double fRec55[2];
	FAUSTFLOAT fVbargraph14;
	double fRec64[2];
	double fRec65[2];
	double fRec63[2];
	double fRec66[2];
	double fRec62[2];
	double fRec61[2];
	FAUSTFLOAT fVbargraph15;
	double fRec47[2];
	FAUSTFLOAT fVbargraph16;
	double fRec67[2];
	FAUSTFLOAT fVbargraph17;
	double fRec68[2];
	FAUSTFLOAT fVbargraph18;
	double fRec69[2];
	FAUSTFLOAT fVbargraph19;
	double fRec70[2];
	FAUSTFLOAT fVbargraph20;
	double fRec74[2];
	double fRec75[2];
	double fRec73[2];
	double fRec72[2];
	FAUSTFLOAT fVbargraph21;
	double fRec79[2];
	double fRec80[2];
	double fRec78[2];
	double fRec81[2];
	double fRec77[2];
	double fRec76[2];
	FAUSTFLOAT fVbargraph22;
	double fRec71[2];
	FAUSTFLOAT fVbargraph23;
	double fRec82[2];
	FAUSTFLOAT fVbargraph24;
	double fRec83[2];
	FAUSTFLOAT fVbargraph25;
	double fRec84[2];
	FAUSTFLOAT fVbargraph26;
	double fRec85[2];
	FAUSTFLOAT fVbargraph27;
	double fRec86[2];
	FAUSTFLOAT fVbargraph28;
	double fRec87[2];
	FAUSTFLOAT fVbargraph29;
	double fRec88[2];
	FAUSTFLOAT fVbargraph30;
	double fRec89[2];
	FAUSTFLOAT fVbargraph31;
	double fRec90[2];
	FAUSTFLOAT fVbargraph32;
	double fRec91[2];
	FAUSTFLOAT fVbargraph33;
	double fRec92[2];
	FAUSTFLOAT fVbargraph34;
	double fRec93[2];
	FAUSTFLOAT fVbargraph35;
	double fRec94[2];
	FAUSTFLOAT fVbargraph36;
	double fRec95[2];
	FAUSTFLOAT fVbargraph37;
	double fRec96[2];
	FAUSTFLOAT fVbargraph38;
	double fRec97[2];
	FAUSTFLOAT fVbargraph39;
	double fRec98[2];
	FAUSTFLOAT fVbargraph40;
	double fRec99[2];
	FAUSTFLOAT fVbargraph41;
	double fRec100[2];
	FAUSTFLOAT fVbargraph42;
	double fRec101[2];
	FAUSTFLOAT fVbargraph43;
	double fRec102[2];
	FAUSTFLOAT fVbargraph44;
	double fRec103[2];
	FAUSTFLOAT fVbargraph45;
	double fRec104[2];
	FAUSTFLOAT fVbargraph46;
	double fRec105[2];
	FAUSTFLOAT fVbargraph47;
	double fRec106[2];
	FAUSTFLOAT fVbargraph48;
	double fRec107[2];
	FAUSTFLOAT fVbargraph49;
	double fRec108[2];
	FAUSTFLOAT fVbargraph50;
	double fRec109[2];
	FAUSTFLOAT fVbargraph51;
	double fRec110[2];
	FAUSTFLOAT fVbargraph52;
	double fRec111[2];
	FAUSTFLOAT fVbargraph53;
	double fRec112[2];
	FAUSTFLOAT fVbargraph54;
	double fRec113[2];
	FAUSTFLOAT fVbargraph55;
	double fRec114[2];
	FAUSTFLOAT fVbargraph56;
	double fRec115[2];
	FAUSTFLOAT fVbargraph57;
	double fRec116[2];
	FAUSTFLOAT fVbargraph58;
	double fRec117[2];
	FAUSTFLOAT fVbargraph59;
	double fRec118[2];
	FAUSTFLOAT fVbargraph60;
	double fRec119[2];
	FAUSTFLOAT fVbargraph61;
	double fRec120[2];
	FAUSTFLOAT fVbargraph62;
	double fRec121[2];
	FAUSTFLOAT fVbargraph63;
	double fRec122[2];
	FAUSTFLOAT fVbargraph64;
	double fRec123[2];
	FAUSTFLOAT fVbargraph65;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
		m->declare("lib/lebedev.lib/author", "Pierre Lecomte");
		m->declare("lib/lebedev.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/lebedev.lib/license", "GPL");
		m->declare("lib/lebedev.lib/name", "Lebdev grids and weights");
		m->declare("lib/lebedev.lib/version", "1.0");
		m->declare("lib/nfc.lib/author", "Pierre Lecomte");
		m->declare("lib/nfc.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/nfc.lib/license", "GPL");
		m->declare("lib/nfc.lib/name", "NF And NFC Filters Library");
		m->declare("lib/nfc.lib/version", "1.0");
		m->declare("lib/ymn.lib/author", "Pierre Lecomte");
		m->declare("lib/ymn.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/ymn.lib/license", "GPL");
		m->declare("lib/ymn.lib/name", "Spherical Harmonics library");
		m->declare("lib/ymn.lib/version", "1.0");
		m->declare("license", "GPL");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("name", "HOADecLebedev503");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 16;
		
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
		fConst0 = min(192000.0, max(1000.0, double(fSamplingFreq)));
		fConst1 = (80.0 / fConst0);
		fConst2 = (170.0 / fConst0);
		fConst3 = (340.0 / fConst0);
		fConst4 = (510.0 / fConst0);
		fConst5 = (1020.0 / fConst0);
		fConst6 = (394.77151028643527 / fConst0);
		fConst7 = (625.22848971356643 / fConst0);
		fConst8 = (1250.4569794271329 / fConst0);
		fConst9 = (789.54302057287055 / fConst0);
		
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
			fRec8[l7] = 0.0;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec7[l8] = 0.0;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fRec12[l9] = 0.0;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec13[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec11[l11] = 0.0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec10[l12] = 0.0;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec16[l13] = 0.0;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec17[l14] = 0.0;
			
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec15[l15] = 0.0;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fRec14[l16] = 0.0;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec20[l17] = 0.0;
			
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec21[l18] = 0.0;
			
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec19[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec18[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec25[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec26[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec24[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec27[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec23[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec22[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec31[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec32[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec30[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec33[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec29[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec28[l32] = 0.0;
			
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
			fRec41[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec40[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec0[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec46[l46] = 0.0;
			
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
			fRec53[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec54[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec52[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec51[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec58[l54] = 0.0;
			
		}
		for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
			fRec59[l55] = 0.0;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fRec57[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec60[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec56[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec55[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec64[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec65[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fRec63[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec66[l63] = 0.0;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec62[l64] = 0.0;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec61[l65] = 0.0;
			
		}
		for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
			fRec47[l66] = 0.0;
			
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
			fRec74[l71] = 0.0;
			
		}
		for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
			fRec75[l72] = 0.0;
			
		}
		for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
			fRec73[l73] = 0.0;
			
		}
		for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
			fRec72[l74] = 0.0;
			
		}
		for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
			fRec79[l75] = 0.0;
			
		}
		for (int l76 = 0; (l76 < 2); l76 = (l76 + 1)) {
			fRec80[l76] = 0.0;
			
		}
		for (int l77 = 0; (l77 < 2); l77 = (l77 + 1)) {
			fRec78[l77] = 0.0;
			
		}
		for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
			fRec81[l78] = 0.0;
			
		}
		for (int l79 = 0; (l79 < 2); l79 = (l79 + 1)) {
			fRec77[l79] = 0.0;
			
		}
		for (int l80 = 0; (l80 < 2); l80 = (l80 + 1)) {
			fRec76[l80] = 0.0;
			
		}
		for (int l81 = 0; (l81 < 2); l81 = (l81 + 1)) {
			fRec71[l81] = 0.0;
			
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
			fRec86[l86] = 0.0;
			
		}
		for (int l87 = 0; (l87 < 2); l87 = (l87 + 1)) {
			fRec87[l87] = 0.0;
			
		}
		for (int l88 = 0; (l88 < 2); l88 = (l88 + 1)) {
			fRec88[l88] = 0.0;
			
		}
		for (int l89 = 0; (l89 < 2); l89 = (l89 + 1)) {
			fRec89[l89] = 0.0;
			
		}
		for (int l90 = 0; (l90 < 2); l90 = (l90 + 1)) {
			fRec90[l90] = 0.0;
			
		}
		for (int l91 = 0; (l91 < 2); l91 = (l91 + 1)) {
			fRec91[l91] = 0.0;
			
		}
		for (int l92 = 0; (l92 < 2); l92 = (l92 + 1)) {
			fRec92[l92] = 0.0;
			
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
		for (int l100 = 0; (l100 < 2); l100 = (l100 + 1)) {
			fRec100[l100] = 0.0;
			
		}
		for (int l101 = 0; (l101 < 2); l101 = (l101 + 1)) {
			fRec101[l101] = 0.0;
			
		}
		for (int l102 = 0; (l102 < 2); l102 = (l102 + 1)) {
			fRec102[l102] = 0.0;
			
		}
		for (int l103 = 0; (l103 < 2); l103 = (l103 + 1)) {
			fRec103[l103] = 0.0;
			
		}
		for (int l104 = 0; (l104 < 2); l104 = (l104 + 1)) {
			fRec104[l104] = 0.0;
			
		}
		for (int l105 = 0; (l105 < 2); l105 = (l105 + 1)) {
			fRec105[l105] = 0.0;
			
		}
		for (int l106 = 0; (l106 < 2); l106 = (l106 + 1)) {
			fRec106[l106] = 0.0;
			
		}
		for (int l107 = 0; (l107 < 2); l107 = (l107 + 1)) {
			fRec107[l107] = 0.0;
			
		}
		for (int l108 = 0; (l108 < 2); l108 = (l108 + 1)) {
			fRec108[l108] = 0.0;
			
		}
		for (int l109 = 0; (l109 < 2); l109 = (l109 + 1)) {
			fRec109[l109] = 0.0;
			
		}
		for (int l110 = 0; (l110 < 2); l110 = (l110 + 1)) {
			fRec110[l110] = 0.0;
			
		}
		for (int l111 = 0; (l111 < 2); l111 = (l111 + 1)) {
			fRec111[l111] = 0.0;
			
		}
		for (int l112 = 0; (l112 < 2); l112 = (l112 + 1)) {
			fRec112[l112] = 0.0;
			
		}
		for (int l113 = 0; (l113 < 2); l113 = (l113 + 1)) {
			fRec113[l113] = 0.0;
			
		}
		for (int l114 = 0; (l114 < 2); l114 = (l114 + 1)) {
			fRec114[l114] = 0.0;
			
		}
		for (int l115 = 0; (l115 < 2); l115 = (l115 + 1)) {
			fRec115[l115] = 0.0;
			
		}
		for (int l116 = 0; (l116 < 2); l116 = (l116 + 1)) {
			fRec116[l116] = 0.0;
			
		}
		for (int l117 = 0; (l117 < 2); l117 = (l117 + 1)) {
			fRec117[l117] = 0.0;
			
		}
		for (int l118 = 0; (l118 < 2); l118 = (l118 + 1)) {
			fRec118[l118] = 0.0;
			
		}
		for (int l119 = 0; (l119 < 2); l119 = (l119 + 1)) {
			fRec119[l119] = 0.0;
			
		}
		for (int l120 = 0; (l120 < 2); l120 = (l120 + 1)) {
			fRec120[l120] = 0.0;
			
		}
		for (int l121 = 0; (l121 < 2); l121 = (l121 + 1)) {
			fRec121[l121] = 0.0;
			
		}
		for (int l122 = 0; (l122 < 2); l122 = (l122 + 1)) {
			fRec122[l122] = 0.0;
			
		}
		for (int l123 = 0; (l123 < 2); l123 = (l123 + 1)) {
			fRec123[l123] = 0.0;
			
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
		ui_interface->openVerticalBox("HOADecLebedev503");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("B-Format");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39b2f00", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a41ca0", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39be400", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39c7da0", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a4d1d0", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a98a90", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39d6640", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39e1cd0", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39ed360", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a5b800", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3aa71e0", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a69f40", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x39fff30", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a0eb00", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a1d6d0", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a2c380", &fVbargraph9, -70.0, 6.0);
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
		ui_interface->declare(&fVbargraph10, "osc", "/output1");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a315c0", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph11, "osc", "/output2");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a387b0", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph16, "osc", "/output3");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a70080", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph17, "osc", "/output4");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a76ce0", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph18, "osc", "/output5");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a7da20", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph19, "osc", "/output6");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a83960", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph20, "osc", "/output7");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3a8d530", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph23, "osc", "/output8");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3ab0350", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph24, "osc", "/output9");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3ab9340", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph25, "osc", "/output10");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3ac2020", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph26, "osc", "/output11");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3aca4a0", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph27, "osc", "/output12");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3ad1a30", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph28, "osc", "/output13");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3ad8ac0", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph29, "osc", "/output14");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3adfff0", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph30, "osc", "/output15");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3ae6550", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph31, "osc", "/output16");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3aed8b0", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph32, "osc", "/output17");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3af50c0", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph33, "osc", "/output18");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3afc5c0", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph34, "osc", "/output19");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b095d0", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph35, "osc", "/output20");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b13960", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph36, "osc", "/output21");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b1cfa0", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph37, "osc", "/output22");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b26cd0", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph38, "osc", "/output23");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b2ec30", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph39, "osc", "/output24");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b36670", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph40, "osc", "/output25");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b3e370", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph41, "osc", "/output26");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b45f70", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph42, "osc", "/output27");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b520c0", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph43, "osc", "/output28");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b59ee0", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph44, "osc", "/output29");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b63ab0", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph45, "osc", "/output30");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b6d5a0", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph46, "osc", "/output31");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b79d40", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph47, "osc", "/output32");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b84ae0", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph48, "osc", "/output33");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b8efb0", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph49, "osc", "/output34");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3b98dc0", &fVbargraph49, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph50, "osc", "/output35");
		ui_interface->declare(&fVbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3ba3270", &fVbargraph50, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph51, "osc", "/output36");
		ui_interface->declare(&fVbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3bad030", &fVbargraph51, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph52, "osc", "/output37");
		ui_interface->declare(&fVbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3bb72c0", &fVbargraph52, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph53, "osc", "/output38");
		ui_interface->declare(&fVbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3bc2760", &fVbargraph53, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph54, "osc", "/output39");
		ui_interface->declare(&fVbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3bc9db0", &fVbargraph54, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph55, "osc", "/output40");
		ui_interface->declare(&fVbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3bd10f0", &fVbargraph55, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph56, "osc", "/output41");
		ui_interface->declare(&fVbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3bd86f0", &fVbargraph56, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph57, "osc", "/output42");
		ui_interface->declare(&fVbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3bdffe0", &fVbargraph57, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph58, "osc", "/output43");
		ui_interface->declare(&fVbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3be73e0", &fVbargraph58, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph59, "osc", "/output44");
		ui_interface->declare(&fVbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3beec80", &fVbargraph59, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph60, "osc", "/output45");
		ui_interface->declare(&fVbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3bf6420", &fVbargraph60, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph61, "osc", "/output46");
		ui_interface->declare(&fVbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3bfdca0", &fVbargraph61, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph62, "osc", "/output47");
		ui_interface->declare(&fVbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3c05630", &fVbargraph62, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph63, "osc", "/output48");
		ui_interface->declare(&fVbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3c0ccb0", &fVbargraph63, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph64, "osc", "/output49");
		ui_interface->declare(&fVbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3c147d0", &fVbargraph64, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph65, "osc", "/output50");
		ui_interface->declare(&fVbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3c1c5b0", &fVbargraph65, -70.0, 6.0);
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
			double fTemp5 = (fRec3[0] * double(input3[i]));
			fRec9[0] = (fRec9[1] + fRec8[1]);
			fRec8[0] = (fSlow8 * ((fSlow3 * fTemp5) - (fSlow9 * fRec9[0])));
			double fTemp6 = (fRec8[0] + (fSlow4 * fTemp5));
			fRec7[0] = max((fRec7[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fVbargraph2 = FAUSTFLOAT(fRec7[0]);
			double fTemp7 = (1.3467622079046435e-18 * fTemp6);
			double fTemp8 = (fRec3[0] * double(input6[i]));
			fRec12[0] = (fRec12[1] + fRec11[1]);
			fRec13[0] = (fRec13[1] + fRec12[1]);
			fRec11[0] = (fSlow11 * ((fSlow3 * fTemp8) - ((fSlow13 * fRec12[0]) + (fSlow12 * fRec13[0]))));
			double fTemp9 = (fRec11[0] + (fSlow4 * fTemp8));
			fRec10[0] = max((fRec10[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph3 = FAUSTFLOAT(fRec10[0]);
			double fTemp10 = (0.028394513999997331 * fTemp9);
			double fTemp11 = (fRec3[0] * double(input7[i]));
			fRec16[0] = (fRec16[1] + fRec15[1]);
			fRec17[0] = (fRec17[1] + fRec16[1]);
			fRec15[0] = (fSlow11 * ((fSlow3 * fTemp11) - ((fSlow13 * fRec16[0]) + (fSlow12 * fRec17[0]))));
			double fTemp12 = (fRec15[0] + (fSlow4 * fTemp11));
			fRec14[0] = max((fRec14[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph4 = FAUSTFLOAT(fRec14[0]);
			double fTemp13 = (3.0114518464024883e-18 * fTemp12);
			double fTemp14 = (fRec3[0] * double(input8[i]));
			fRec20[0] = (fRec20[1] + fRec19[1]);
			fRec21[0] = (fRec21[1] + fRec20[1]);
			fRec19[0] = (fSlow11 * ((fSlow3 * fTemp14) - ((fSlow13 * fRec20[0]) + (fSlow12 * fRec21[0]))));
			double fTemp15 = (fRec19[0] + (fSlow4 * fTemp14));
			fRec18[0] = max((fRec18[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph5 = FAUSTFLOAT(fRec18[0]);
			double fTemp16 = (9.2199121612079837e-35 * fTemp15);
			double fTemp17 = (fRec3[0] * double(input12[i]));
			fRec25[0] = (fRec25[1] + fRec24[1]);
			fRec26[0] = (fRec26[1] + fRec25[1]);
			fRec24[0] = (fSlow15 * ((fSlow3 * fTemp17) - ((fSlow17 * fRec25[0]) + (fSlow16 * fRec26[0]))));
			fRec27[0] = (fRec27[1] + fRec23[1]);
			fRec23[0] = (fSlow14 * (fRec24[0] - (fSlow18 * fRec27[0])));
			double fTemp18 = (fRec23[0] + (fSlow4 * fTemp17));
			fRec22[0] = max((fRec22[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph6 = FAUSTFLOAT(fRec22[0]);
			double fTemp19 = (0.033596842045264641 * fTemp18);
			double fTemp20 = (fRec3[0] * double(input13[i]));
			fRec31[0] = (fRec31[1] + fRec30[1]);
			fRec32[0] = (fRec32[1] + fRec31[1]);
			fRec30[0] = (fSlow15 * ((fSlow3 * fTemp20) - ((fSlow17 * fRec31[0]) + (fSlow16 * fRec32[0]))));
			fRec33[0] = (fRec33[1] + fRec29[1]);
			fRec29[0] = (fSlow14 * (fRec30[0] - (fSlow18 * fRec33[0])));
			double fTemp21 = (fRec29[0] + (fSlow4 * fTemp20));
			fRec28[0] = max((fRec28[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph7 = FAUSTFLOAT(fRec28[0]);
			double fTemp22 = (5.0391227634343926e-18 * fTemp21);
			double fTemp23 = (fRec3[0] * double(input14[i]));
			fRec37[0] = (fRec37[1] + fRec36[1]);
			fRec38[0] = (fRec38[1] + fRec37[1]);
			fRec36[0] = (fSlow15 * ((fSlow3 * fTemp23) - ((fSlow17 * fRec37[0]) + (fSlow16 * fRec38[0]))));
			fRec39[0] = (fRec39[1] + fRec35[1]);
			fRec35[0] = (fSlow14 * (fRec36[0] - (fSlow18 * fRec39[0])));
			double fTemp24 = (fRec35[0] + (fSlow4 * fTemp23));
			fRec34[0] = max((fRec34[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph8 = FAUSTFLOAT(fRec34[0]);
			double fTemp25 = (2.4393594688416384e-34 * fTemp24);
			double fTemp26 = (fRec3[0] * double(input15[i]));
			fRec43[0] = (fRec43[1] + fRec42[1]);
			fRec44[0] = (fRec44[1] + fRec43[1]);
			fRec42[0] = (fSlow15 * ((fSlow3 * fTemp26) - ((fSlow17 * fRec43[0]) + (fSlow16 * fRec44[0]))));
			fRec45[0] = (fRec45[1] + fRec41[1]);
			fRec41[0] = (fSlow14 * (fRec42[0] - (fSlow18 * fRec45[0])));
			double fTemp27 = (fRec41[0] + (fSlow4 * fTemp26));
			fRec40[0] = max((fRec40[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph9 = FAUSTFLOAT(fRec40[0]);
			double fTemp28 = (6.0979103388536467e-51 * fTemp27);
			double fTemp29 = (fRec1[0] * (((((((((fTemp1 + fTemp4) + fTemp7) + fTemp10) + fTemp13) + fTemp16) + fTemp19) + fTemp22) + fTemp25) + fTemp28));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph10 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp29);
			double fTemp30 = (0.021994295969128601 * fTemp6);
			double fTemp31 = (0.024590370452110585 * fTemp15);
			double fTemp32 = (0.026560635762986527 * fTemp27);
			double fTemp33 = (0.014197256999998666 * fTemp9);
			double fTemp34 = (0.020573779994945588 * fTemp21);
			double fTemp35 = (fRec1[0] * ((((fTemp1 + fTemp30) + fTemp31) + fTemp32) - (fTemp33 + fTemp34)));
			fRec46[0] = max((fRec46[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph11 = FAUSTFLOAT(fRec46[0]);
			output1[i] = FAUSTFLOAT(fTemp35);
			double fTemp36 = (fRec3[0] * double(input1[i]));
			fRec50[0] = (fRec50[1] + fRec49[1]);
			fRec49[0] = (fSlow8 * ((fSlow3 * fTemp36) - (fSlow9 * fRec50[0])));
			double fTemp37 = (fRec49[0] + (fSlow4 * fTemp36));
			fRec48[0] = max((fRec48[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fVbargraph12 = FAUSTFLOAT(fRec48[0]);
			double fTemp38 = (0.021994295969128601 * fTemp37);
			double fTemp39 = (fTemp1 + fTemp7);
			double fTemp40 = (fRec3[0] * double(input4[i]));
			fRec53[0] = (fRec53[1] + fRec52[1]);
			fRec54[0] = (fRec54[1] + fRec53[1]);
			fRec52[0] = (fSlow11 * ((fSlow3 * fTemp40) - ((fSlow13 * fRec53[0]) + (fSlow12 * fRec54[0]))));
			double fTemp41 = (fRec52[0] + (fSlow4 * fTemp40));
			fRec51[0] = max((fRec51[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph13 = FAUSTFLOAT(fRec51[0]);
			double fTemp42 = (fRec3[0] * double(input9[i]));
			fRec58[0] = (fRec58[1] + fRec57[1]);
			fRec59[0] = (fRec59[1] + fRec58[1]);
			fRec57[0] = (fSlow15 * ((fSlow3 * fTemp42) - ((fSlow17 * fRec58[0]) + (fSlow16 * fRec59[0]))));
			fRec60[0] = (fRec60[1] + fRec56[1]);
			fRec56[0] = (fSlow14 * (fRec57[0] - (fSlow18 * fRec60[0])));
			double fTemp43 = (fRec56[0] + (fSlow4 * fTemp42));
			fRec55[0] = max((fRec55[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph14 = FAUSTFLOAT(fRec55[0]);
			double fTemp44 = (0.026560635762986527 * fTemp43);
			double fTemp45 = (fRec3[0] * double(input11[i]));
			fRec64[0] = (fRec64[1] + fRec63[1]);
			fRec65[0] = (fRec65[1] + fRec64[1]);
			fRec63[0] = (fSlow15 * ((fSlow3 * fTemp45) - ((fSlow17 * fRec64[0]) + (fSlow16 * fRec65[0]))));
			fRec66[0] = (fRec66[1] + fRec62[1]);
			fRec62[0] = (fSlow14 * (fRec63[0] - (fSlow18 * fRec66[0])));
			double fTemp46 = (fRec62[0] + (fSlow4 * fTemp45));
			fRec61[0] = max((fRec61[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph15 = FAUSTFLOAT(fRec61[0]);
			double fTemp47 = (0.020573779994945588 * fTemp46);
			double fTemp48 = (fRec1[0] * (((fTemp38 + fTemp39) + (3.0114518464024883e-18 * fTemp41)) - (fTemp44 + (fTemp47 + (((fTemp33 + fTemp31) + (1.2597806908585981e-18 * fTemp21)) + (4.879109635569025e-18 * fTemp27))))));
			fRec47[0] = max((fRec47[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph16 = FAUSTFLOAT(fRec47[0]);
			output2[i] = FAUSTFLOAT(fTemp48);
			double fTemp49 = (fRec1[0] * (((9.7582192711380499e-18 * fTemp43) + ((2.6935244158092871e-18 * fTemp37) + (fTemp34 + (fTemp1 + fTemp31)))) - ((6.0229036928049765e-18 * fTemp41) + ((2.5195613817171963e-18 * fTemp46) + (fTemp32 + (fTemp30 + fTemp33))))));
			fRec67[0] = max((fRec67[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fVbargraph17 = FAUSTFLOAT(fRec67[0]);
			output3[i] = FAUSTFLOAT(fTemp49);
			double fTemp50 = (fRec1[0] * ((fTemp44 + ((9.0343555392074636e-18 * fTemp41) + (fTemp47 + ((fTemp1 + (3.7793420725757941e-18 * fTemp21)) + (1.4637328906707076e-17 * fTemp27))))) - (fTemp38 + (fTemp31 + (fTemp33 + (4.0402866237139308e-18 * fTemp6))))));
			fRec68[0] = max((fRec68[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph18 = FAUSTFLOAT(fRec68[0]);
			output4[i] = FAUSTFLOAT(fTemp50);
			double fTemp51 = (fRec1[0] * ((fTemp28 + (fTemp22 + (fTemp16 + (fTemp10 + fTemp39)))) - (fTemp25 + (fTemp19 + (fTemp4 + fTemp13)))));
			fRec69[0] = max((fRec69[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph19 = FAUSTFLOAT(fRec69[0]);
			output5[i] = FAUSTFLOAT(fTemp51);
			double fTemp52 = (0.022574955908289243 * fTemp0);
			double fTemp53 = (0.027648561470568499 * fTemp3);
			double fTemp54 = (fTemp52 + fTemp53);
			double fTemp55 = (0.027648561470568506 * fTemp6);
			double fTemp56 = (0.012619783999998805 * fTemp9);
			double fTemp57 = (0.043716214137085485 * fTemp12);
			double fTemp58 = (0.021858107068542749 * fTemp15);
			double fTemp59 = (0.038794291597509752 * fTemp21);
			double fTemp60 = (0.04089277388695433 * fTemp24);
			double fTemp61 = (0.016694405031674409 * fTemp27);
			double fTemp62 = (0.010558468816293319 * fTemp18);
			double fTemp63 = (fRec1[0] * ((((((((fTemp54 + fTemp55) + fTemp56) + fTemp57) + fTemp58) + fTemp59) + fTemp60) + fTemp61) - fTemp62));
			fRec70[0] = max((fRec70[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fVbargraph20 = FAUSTFLOAT(fRec70[0]);
			output6[i] = FAUSTFLOAT(fTemp63);
			double fTemp64 = (fRec3[0] * double(input5[i]));
			fRec74[0] = (fRec74[1] + fRec73[1]);
			fRec75[0] = (fRec75[1] + fRec74[1]);
			fRec73[0] = (fSlow11 * ((fSlow3 * fTemp64) - ((fSlow13 * fRec74[0]) + (fSlow12 * fRec75[0]))));
			double fTemp65 = (fRec73[0] + (fSlow4 * fTemp64));
			fRec72[0] = max((fRec72[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fVbargraph21 = FAUSTFLOAT(fRec72[0]);
			double fTemp66 = (0.043716214137085485 * fTemp65);
			double fTemp67 = (fRec3[0] * double(input10[i]));
			fRec79[0] = (fRec79[1] + fRec78[1]);
			fRec80[0] = (fRec80[1] + fRec79[1]);
			fRec78[0] = (fSlow15 * ((fSlow3 * fTemp67) - ((fSlow17 * fRec79[0]) + (fSlow16 * fRec80[0]))));
			fRec81[0] = (fRec81[1] + fRec77[1]);
			fRec77[0] = (fSlow14 * (fRec78[0] - (fSlow18 * fRec81[0])));
			double fTemp68 = (fRec77[0] + (fSlow4 * fTemp67));
			fRec76[0] = max((fRec76[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fVbargraph22 = FAUSTFLOAT(fRec76[0]);
			double fTemp69 = (5.0079204648915091e-18 * fTemp68);
			double fTemp70 = (2.6768460856911014e-18 * fTemp41);
			double fTemp71 = (0.027648561470568506 * fTemp37);
			double fTemp72 = (0.038794291597509752 * fTemp46);
			double fTemp73 = (1.6929861152980278e-18 * fTemp6);
			double fTemp74 = (2.6768460856911007e-18 * fTemp12);
			double fTemp75 = (2.3754652515039685e-18 * fTemp21);
			double fTemp76 = (0.016694405031674409 * fTemp43);
			double fTemp77 = (3.0667124528564294e-18 * fTemp27);
			double fTemp78 = (fRec1[0] * ((fTemp66 + (fTemp69 + (fTemp70 + (fTemp71 + (fTemp72 + (((fTemp56 + (fTemp54 + fTemp73)) + fTemp74) + fTemp75)))))) - (fTemp76 + ((fTemp60 + (fTemp58 + fTemp62)) + fTemp77))));
			fRec71[0] = max((fRec71[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fVbargraph23 = FAUSTFLOAT(fRec71[0]);
			output7[i] = FAUSTFLOAT(fTemp78);
			double fTemp79 = (5.3536921713822013e-18 * fTemp65);
			double fTemp80 = (6.1334249057128588e-18 * fTemp43);
			double fTemp81 = (3.3859722305960555e-18 * fTemp37);
			double fTemp82 = (4.7509305030079371e-18 * fTemp46);
			double fTemp83 = (fTemp54 + fTemp56);
			double fTemp84 = (1.0015840929783018e-17 * fTemp68);
			double fTemp85 = (5.3536921713822029e-18 * fTemp41);
			double fTemp86 = (fRec1[0] * ((fTemp79 + (fTemp80 + (fTemp81 + (fTemp82 + (fTemp60 + (fTemp58 + fTemp83)))))) - (fTemp84 + (fTemp85 + (fTemp61 + (fTemp59 + (fTemp62 + (fTemp55 + fTemp57))))))));
			fRec82[0] = max((fRec82[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp86))))));
			fVbargraph24 = FAUSTFLOAT(fRec82[0]);
			output8[i] = FAUSTFLOAT(fTemp86);
			double fTemp87 = (1.5023761394674526e-17 * fTemp68);
			double fTemp88 = (8.0305382570733035e-18 * fTemp41);
			double fTemp89 = (9.2001373585692901e-18 * fTemp27);
			double fTemp90 = (5.0789583458940831e-18 * fTemp6);
			double fTemp91 = (8.0305382570733005e-18 * fTemp12);
			double fTemp92 = (7.1263957545119048e-18 * fTemp21);
			double fTemp93 = (fRec1[0] * ((fTemp87 + (fTemp76 + (fTemp88 + (fTemp83 + fTemp89)))) - (fTemp66 + (fTemp71 + (fTemp72 + (fTemp60 + ((fTemp62 + (fTemp58 + (fTemp90 + fTemp91))) + fTemp92)))))));
			fRec83[0] = max((fRec83[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fVbargraph25 = FAUSTFLOAT(fRec83[0]);
			output9[i] = FAUSTFLOAT(fTemp93);
			double fTemp94 = (0.043716214137085485 * fTemp41);
			double fTemp95 = (0.027648561470568499 * fTemp37);
			double fTemp96 = (fTemp52 + fTemp55);
			double fTemp97 = (0.025862861065006505 * fTemp46);
			double fTemp98 = (0.025239567999997631 * fTemp9);
			double fTemp99 = (0.025862861065006511 * fTemp21);
			double fTemp100 = (fRec1[0] * ((fTemp94 + (fTemp95 + (fTemp96 + (2.6768460856911007e-18 * fTemp15)))) - ((fTemp97 + (fTemp98 + fTemp99)) + (0.033388810063348803 * (fTemp27 - fTemp43)))));
			fRec84[0] = max((fRec84[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp100))))));
			fVbargraph26 = FAUSTFLOAT(fRec84[0]);
			output10[i] = FAUSTFLOAT(fTemp100);
			double fTemp101 = (0.025862861065006511 * fTemp46);
			double fTemp102 = (fRec1[0] * (((0.033388810063348796 * fTemp43) + (fTemp71 + ((fTemp52 + (0.025862861065006505 * fTemp21)) + (0.033388810063348817 * fTemp27)))) - (fTemp94 + (((fTemp98 + (0.027648561470568499 * fTemp6)) + (8.0305382570733005e-18 * fTemp15)) + fTemp101))));
			fRec85[0] = max((fRec85[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp102))))));
			fVbargraph27 = FAUSTFLOAT(fRec85[0]);
			output11[i] = FAUSTFLOAT(fTemp102);
			double fTemp103 = (fRec1[0] * ((fTemp94 + (fTemp97 + ((fTemp99 + (fTemp52 + (1.3384230428455503e-17 * fTemp15))) + (0.033388810063348762 * fTemp27)))) - ((fTemp95 + (fTemp55 + fTemp98)) + (0.033388810063348852 * fTemp43))));
			fRec86[0] = max((fRec86[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp103))))));
			fVbargraph28 = FAUSTFLOAT(fRec86[0]);
			output12[i] = FAUSTFLOAT(fTemp103);
			double fTemp104 = (fRec1[0] * ((fTemp101 + (fTemp52 + (0.027648561470568496 * fTemp6))) - ((0.033388810063348817 * fTemp43) + (fTemp94 + (fTemp71 + (((fTemp98 + (1.8737922599837705e-17 * fTemp15)) + (0.025862861065006501 * fTemp21)) + (0.033388810063348796 * fTemp27)))))));
			fRec87[0] = max((fRec87[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp104))))));
			fVbargraph29 = FAUSTFLOAT(fRec87[0]);
			output13[i] = FAUSTFLOAT(fTemp104);
			double fTemp105 = (fRec1[0] * ((fTemp61 + (fTemp59 + (fTemp62 + (fTemp58 + (fTemp56 + fTemp96))))) - (fTemp60 + (fTemp53 + fTemp57))));
			fRec88[0] = max((fRec88[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp105))))));
			fVbargraph30 = FAUSTFLOAT(fRec88[0]);
			output14[i] = FAUSTFLOAT(fTemp105);
			double fTemp106 = (fRec1[0] * ((fTemp70 + (fTemp71 + (fTemp72 + (fTemp60 + (fTemp75 + (fTemp62 + (fTemp56 + (fTemp52 + fTemp73)))))))) - (fTemp66 + (fTemp69 + (fTemp76 + (fTemp77 + (fTemp58 + (fTemp53 + fTemp74))))))));
			fRec89[0] = max((fRec89[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp106))))));
			fVbargraph31 = FAUSTFLOAT(fRec89[0]);
			output15[i] = FAUSTFLOAT(fTemp106);
			double fTemp107 = (fTemp52 + fTemp56);
			double fTemp108 = (fRec1[0] * ((fTemp84 + (fTemp80 + (fTemp81 + (fTemp82 + (fTemp62 + (fTemp58 + (fTemp57 + fTemp107))))))) - (fTemp79 + (fTemp85 + (fTemp61 + (fTemp60 + (fTemp59 + (fTemp53 + fTemp55))))))));
			fRec90[0] = max((fRec90[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp108))))));
			fVbargraph32 = FAUSTFLOAT(fRec90[0]);
			output16[i] = FAUSTFLOAT(fTemp108);
			double fTemp109 = (fRec1[0] * ((fTemp66 + (fTemp76 + (fTemp88 + (fTemp89 + (fTemp60 + (fTemp62 + (fTemp91 + fTemp107))))))) - (fTemp87 + (fTemp71 + (fTemp72 + (fTemp92 + (fTemp58 + (fTemp53 + fTemp90))))))));
			fRec91[0] = max((fRec91[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp109))))));
			fVbargraph33 = FAUSTFLOAT(fRec91[0]);
			output17[i] = FAUSTFLOAT(fTemp109);
			double fTemp110 = (fTemp0 + fTemp3);
			double fTemp111 = (0.021093750000000001 * fTemp110);
			double fTemp112 = (0.027231914153020904 * fTemp65);
			double fTemp113 = (0.041597435974919175 * fTemp68);
			double fTemp114 = (0.016982082124440745 * fTemp43);
			double fTemp115 = (0.027231914153020897 * fTemp41);
			double fTemp116 = (0.021093749999999994 * fTemp37);
			double fTemp117 = (0.013154264250377145 * fTemp46);
			double fTemp118 = (5.236595479505967e-18 * fTemp9);
			double fTemp119 = ((0.021093749999999998 * fTemp6) + fTemp118);
			double fTemp120 = (0.027231914153020911 * fTemp12);
			double fTemp121 = (1.6674738251076273e-18 * fTemp15);
			double fTemp122 = (0.013154264250377147 * fTemp21);
			double fTemp123 = (2.5471083409710867e-18 * fTemp24);
			double fTemp124 = (0.021480823570105497 * fTemp18);
			double fTemp125 = (0.016982082124440741 * fTemp27);
			double fTemp126 = (fRec1[0] * ((fTemp111 + (fTemp112 + (fTemp113 + (fTemp114 + (fTemp115 + (fTemp116 + (fTemp117 + ((((fTemp119 + fTemp120) + fTemp121) + fTemp122) + fTemp123)))))))) - (fTemp124 + fTemp125)));
			fRec92[0] = max((fRec92[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp126))))));
			fVbargraph34 = FAUSTFLOAT(fRec92[0]);
			output18[i] = FAUSTFLOAT(fTemp126);
			double fTemp127 = (0.027231914153020911 * fTemp65);
			double fTemp128 = (0.016982082124440741 * fTemp43);
			double fTemp129 = (0.021093749999999998 * fTemp37);
			double fTemp130 = (0.013154264250377147 * fTemp46);
			double fTemp131 = (0.016982082124440748 * fTemp27);
			double fTemp132 = (0.021093749999999994 * fTemp6);
			double fTemp133 = (0.027231914153020904 * fTemp12);
			double fTemp134 = (5.0024214753228816e-18 * fTemp15);
			double fTemp135 = (0.013154264250377145 * fTemp21);
			double fTemp136 = (7.6413250229132581e-18 * fTemp24);
			double fTemp137 = (fRec1[0] * ((fTemp111 + (fTemp127 + (fTemp128 + (fTemp129 + (fTemp130 + (fTemp118 + fTemp131)))))) - (fTemp113 + (fTemp115 + (((fTemp124 + ((fTemp132 + fTemp133) + fTemp134)) + fTemp135) + fTemp136)))));
			fRec93[0] = max((fRec93[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp137))))));
			fVbargraph35 = FAUSTFLOAT(fRec93[0]);
			output19[i] = FAUSTFLOAT(fTemp137);
			double fTemp138 = (8.3373691255381378e-18 * fTemp15);
			double fTemp139 = (1.2735541704855431e-17 * fTemp24);
			double fTemp140 = (0.016982082124440721 * fTemp27);
			double fTemp141 = (0.016982082124440766 * fTemp43);
			double fTemp142 = (0.013154264250377148 * fTemp21);
			double fTemp143 = (fRec1[0] * ((fTemp113 + (fTemp115 + (((fTemp118 + fTemp138) + fTemp139) + fTemp140))) - ((0.021093750000000001 * (fTemp6 - fTemp110)) + (fTemp112 + (fTemp141 + (fTemp116 + (fTemp117 + ((fTemp120 + fTemp124) + fTemp142))))))));
			fRec94[0] = max((fRec94[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp143))))));
			fVbargraph36 = FAUSTFLOAT(fRec94[0]);
			output20[i] = FAUSTFLOAT(fTemp143);
			double fTemp144 = (fTemp0 - fTemp37);
			double fTemp145 = (fTemp118 + (0.021093749999999991 * fTemp6));
			double fTemp146 = (0.027231914153020901 * fTemp12);
			double fTemp147 = (0.016982082124440748 * fTemp43);
			double fTemp148 = (0.013154264250377148 * fTemp46);
			double fTemp149 = (1.167231677575339e-17 * fTemp15);
			double fTemp150 = (1.7829758386797603e-17 * fTemp24);
			double fTemp151 = (fRec1[0] * (((0.021093750000000001 * (fTemp3 + fTemp144)) + (fTemp135 + (fTemp145 + fTemp146))) - (fTemp127 + (fTemp113 + (fTemp147 + (fTemp115 + (fTemp148 + (fTemp125 + ((fTemp124 + fTemp149) + fTemp150)))))))));
			fRec95[0] = max((fRec95[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp151))))));
			fVbargraph37 = FAUSTFLOAT(fRec95[0]);
			output21[i] = FAUSTFLOAT(fTemp151);
			double fTemp152 = (fTemp0 - fTemp3);
			double fTemp153 = (0.021093750000000001 * fTemp152);
			double fTemp154 = (fRec1[0] * ((fTemp153 + (fTemp114 + (fTemp115 + (fTemp116 + (fTemp117 + (fTemp122 + (fTemp124 + (fTemp119 + fTemp121)))))))) - (fTemp112 + (fTemp113 + (fTemp125 + (fTemp120 + fTemp123))))));
			fRec96[0] = max((fRec96[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp154))))));
			fVbargraph38 = FAUSTFLOAT(fRec96[0]);
			output22[i] = FAUSTFLOAT(fTemp154);
			double fTemp155 = (fRec1[0] * ((fTemp153 + (fTemp113 + (fTemp128 + (fTemp129 + (fTemp130 + (fTemp131 + (fTemp136 + (fTemp124 + (fTemp118 + fTemp133))))))))) - (fTemp127 + (fTemp115 + (fTemp135 + (fTemp132 + fTemp134))))));
			fRec97[0] = max((fRec97[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp155))))));
			fVbargraph39 = FAUSTFLOAT(fRec97[0]);
			output23[i] = FAUSTFLOAT(fTemp155);
			double fTemp156 = (fRec1[0] * ((fTemp112 + (fTemp115 + (fTemp140 + (fTemp124 + (fTemp138 + (fTemp118 + fTemp120)))))) - ((0.021093750000000001 * (fTemp6 - fTemp152)) + (fTemp113 + (fTemp141 + (fTemp116 + (fTemp117 + (fTemp142 + fTemp139))))))));
			fRec98[0] = max((fRec98[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp156))))));
			fVbargraph40 = FAUSTFLOAT(fRec98[0]);
			output24[i] = FAUSTFLOAT(fTemp156);
			double fTemp157 = (fRec1[0] * ((fTemp127 + (fTemp113 + (fTemp150 + (fTemp135 + (fTemp124 + fTemp145))))) - ((0.021093750000000001 * (fTemp3 - fTemp144)) + (fTemp147 + (fTemp115 + (fTemp148 + (fTemp125 + (fTemp146 + fTemp149))))))));
			fRec99[0] = max((fRec99[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp157))))));
			fVbargraph41 = FAUSTFLOAT(fRec99[0]);
			output25[i] = FAUSTFLOAT(fTemp157);
			double fTemp158 = (0.021308452520676528 * (fTemp12 + fTemp65));
			double fTemp159 = (0.010535180889746065 * (fTemp6 + fTemp37));
			double fTemp160 = (0.016998264729033836 * fTemp68);
			double fTemp161 = (0.0023131708388267431 * fTemp43);
			double fTemp162 = (0.0071028175068921654 * fTemp41);
			double fTemp163 = (0.030460165261409405 * fTemp46);
			double fTemp164 = (0.02017333553791887 * fTemp0);
			double fTemp165 = (0.031605542669238249 * fTemp3);
			double fTemp166 = (fTemp164 + fTemp165);
			double fTemp167 = (0.032806508796871948 * fTemp9);
			double fTemp168 = (fTemp166 + fTemp167);
			double fTemp169 = (0.0263335984839893 * fTemp18);
			double fTemp170 = (fTemp168 + fTemp169);
			double fTemp171 = (0.030460165261409402 * fTemp21);
			double fTemp172 = (1.1422201709353029e-18 * fTemp15);
			double fTemp173 = (2.7335294515958808e-18 * fTemp24);
			double fTemp174 = (0.0023131708388267449 * fTemp27);
			double fTemp175 = (fRec1[0] * ((fTemp158 + (fTemp159 + (fTemp160 + (fTemp161 + (fTemp162 + (fTemp163 + (fTemp170 + fTemp171))))))) - ((fTemp172 + fTemp173) + fTemp174)));
			fRec100[0] = max((fRec100[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp175))))));
			fVbargraph42 = FAUSTFLOAT(fRec100[0]);
			output26[i] = FAUSTFLOAT(fTemp175);
			double fTemp176 = (0.0023131708388267444 * fTemp27);
			double fTemp177 = (0.021308452520676528 * (fTemp12 - fTemp65));
			double fTemp178 = (0.010535180889746065 * (fTemp6 - fTemp37));
			double fTemp179 = (1.3047664087114909e-18 * fTemp15);
			double fTemp180 = (3.1225305737205953e-18 * fTemp24);
			double fTemp181 = (fRec1[0] * ((fTemp161 + (fTemp163 + (fTemp170 + fTemp176))) - (fTemp177 + (fTemp178 + (fTemp160 + (fTemp162 + ((fTemp171 + fTemp179) + fTemp180)))))));
			fRec101[0] = max((fRec101[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp181))))));
			fVbargraph43 = FAUSTFLOAT(fRec101[0]);
			output27[i] = FAUSTFLOAT(fTemp181);
			double fTemp182 = (2.1746106811858183e-18 * fTemp15);
			double fTemp183 = (5.2042176228676594e-18 * fTemp24);
			double fTemp184 = (0.0023131708388267409 * fTemp27);
			double fTemp185 = (0.021308452520676528 * fTemp65);
			double fTemp186 = (0.002313170838826747 * fTemp43);
			double fTemp187 = (0.010535180889746065 * fTemp37);
			double fTemp188 = (0.030460165261409402 * fTemp46);
			double fTemp189 = (0.010535180889746067 * fTemp6);
			double fTemp190 = (0.021308452520676532 * fTemp12);
			double fTemp191 = (0.030460165261409412 * fTemp21);
			double fTemp192 = (fRec1[0] * ((fTemp160 + (fTemp162 + (((fTemp169 + (fTemp168 + fTemp182)) + fTemp183) + fTemp184))) - (fTemp185 + (fTemp186 + (fTemp187 + (fTemp188 + ((fTemp189 + fTemp190) + fTemp191)))))));
			fRec102[0] = max((fRec102[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp192))))));
			fVbargraph44 = FAUSTFLOAT(fRec102[0]);
			output28[i] = FAUSTFLOAT(fTemp192);
			double fTemp193 = (0.010535180889746063 * fTemp6);
			double fTemp194 = (0.021308452520676525 * fTemp12);
			double fTemp195 = (0.030460165261409398 * fTemp21);
			double fTemp196 = (0.021308452520676532 * fTemp65);
			double fTemp197 = (0.0023131708388267444 * fTemp43);
			double fTemp198 = (0.010535180889746067 * fTemp37);
			double fTemp199 = (0.030460165261409412 * fTemp46);
			double fTemp200 = (3.0444549536601452e-18 * fTemp15);
			double fTemp201 = (7.2859046720147234e-18 * fTemp24);
			double fTemp202 = (0.0023131708388267431 * fTemp27);
			double fTemp203 = (fRec1[0] * (((fTemp169 + ((fTemp167 + (fTemp166 + fTemp193)) + fTemp194)) + fTemp195) - (fTemp196 + (fTemp160 + (fTemp197 + (fTemp162 + (fTemp198 + (fTemp199 + ((fTemp200 + fTemp201) + fTemp202)))))))));
			fRec103[0] = max((fRec103[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp203))))));
			fVbargraph45 = FAUSTFLOAT(fRec103[0]);
			output29[i] = FAUSTFLOAT(fTemp203);
			double fTemp204 = (0.0071028175068921914 * fTemp65);
			double fTemp205 = (0.016998264729033888 * fTemp68);
			double fTemp206 = (0.030071220904747824 * fTemp43);
			double fTemp207 = (0.021308452520676577 * fTemp41);
			double fTemp208 = (0.010535180889746094 * fTemp37);
			double fTemp209 = (0.010535180889746075 * fTemp3);
			double fTemp210 = (fTemp164 + fTemp209);
			double fTemp211 = (0.031605542669238242 * fTemp6);
			double fTemp212 = (0.021308452520676542 * fTemp12);
			double fTemp213 = (0.028411270027568724 * fTemp15);
			double fTemp214 = (0.022664352972045144 * fTemp24);
			double fTemp215 = (0.020818537549440738 * fTemp27);
			double fTemp216 = (0.0053753232814252047 * fTemp46);
			double fTemp217 = (0.01640325439843596 * fTemp9);
			double fTemp218 = (0.020481687709769415 * fTemp18);
			double fTemp219 = (fTemp217 + fTemp218);
			double fTemp220 = (0.016125969844275591 * fTemp21);
			double fTemp221 = (fRec1[0] * ((fTemp204 + (fTemp205 + (fTemp206 + (fTemp207 + (fTemp208 + (((((fTemp210 + fTemp211) + fTemp212) + fTemp213) + fTemp214) + fTemp215)))))) - (fTemp216 + (fTemp219 + fTemp220))));
			fRec104[0] = max((fRec104[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp221))))));
			fVbargraph46 = FAUSTFLOAT(fRec104[0]);
			output30[i] = FAUSTFLOAT(fTemp221);
			double fTemp222 = (0.021308452520676546 * fTemp65);
			double fTemp223 = (0.021308452520676556 * fTemp41);
			double fTemp224 = (0.031605542669238242 * fTemp37);
			double fTemp225 = (0.010535180889746081 * fTemp6);
			double fTemp226 = (0.0071028175068921827 * fTemp12);
			double fTemp227 = (0.016998264729033867 * fTemp68);
			double fTemp228 = (0.020818537549440783 * fTemp43);
			double fTemp229 = (0.016125969844275591 * fTemp46);
			double fTemp230 = (0.028411270027568741 * fTemp15);
			double fTemp231 = (0.0053753232814251978 * fTemp21);
			double fTemp232 = (0.022664352972045162 * fTemp24);
			double fTemp233 = (0.030071220904747797 * fTemp27);
			double fTemp234 = (fRec1[0] * ((fTemp222 + ((fTemp223 + (fTemp224 + ((fTemp210 + fTemp225) + fTemp226))) + fTemp227)) - (fTemp228 + (fTemp229 + ((((fTemp218 + (fTemp217 + fTemp230)) + fTemp231) + fTemp232) + fTemp233)))));
			fRec105[0] = max((fRec105[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp234))))));
			fVbargraph47 = FAUSTFLOAT(fRec105[0]);
			output31[i] = FAUSTFLOAT(fTemp234);
			double fTemp235 = (0.021308452520676553 * fTemp65);
			double fTemp236 = (0.0053753232814251969 * fTemp21);
			double fTemp237 = (0.03007122090474779 * fTemp27);
			double fTemp238 = (0.016998264729033864 * fTemp68);
			double fTemp239 = (0.02081853754944079 * fTemp43);
			double fTemp240 = (0.021308452520676546 * fTemp41);
			double fTemp241 = (0.016125969844275594 * fTemp46);
			double fTemp242 = (0.010535180889746079 * fTemp6);
			double fTemp243 = (0.007102817506892181 * fTemp12);
			double fTemp244 = (0.028411270027568745 * fTemp15);
			double fTemp245 = (fRec1[0] * ((fTemp235 + (fTemp224 + ((fTemp210 + fTemp236) + fTemp237))) - (fTemp238 + (fTemp239 + (fTemp240 + (fTemp241 + (fTemp232 + (fTemp218 + (((fTemp217 + fTemp242) + fTemp243) + fTemp244)))))))));
			fRec106[0] = max((fRec106[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp245))))));
			fVbargraph48 = FAUSTFLOAT(fRec106[0]);
			output32[i] = FAUSTFLOAT(fTemp245);
			double fTemp246 = (0.0071028175068921931 * fTemp65);
			double fTemp247 = (0.030071220904747838 * fTemp43);
			double fTemp248 = (0.010535180889746096 * fTemp37);
			double fTemp249 = (0.028411270027568717 * fTemp15);
			double fTemp250 = (0.022664352972045138 * fTemp24);
			double fTemp251 = (0.021308452520676584 * fTemp41);
			double fTemp252 = (0.0053753232814252056 * fTemp46);
			double fTemp253 = (fTemp211 + fTemp217);
			double fTemp254 = (0.020818537549440724 * fTemp27);
			double fTemp255 = (fRec1[0] * ((fTemp246 + (fTemp247 + (fTemp248 + ((fTemp220 + (fTemp210 + fTemp249)) + fTemp250)))) - (fTemp205 + (fTemp251 + (fTemp252 + ((fTemp218 + (fTemp212 + fTemp253)) + fTemp254))))));
			fRec107[0] = max((fRec107[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp255))))));
			fVbargraph49 = FAUSTFLOAT(fRec107[0]);
			output33[i] = FAUSTFLOAT(fTemp255);
			double fTemp256 = (0.016998264729033881 * fTemp68);
			double fTemp257 = (0.02130845252067657 * fTemp41);
			double fTemp258 = (0.0053753232814252021 * fTemp46);
			double fTemp259 = (0.028411270027568731 * fTemp15);
			double fTemp260 = (0.022664352972045151 * fTemp24);
			double fTemp261 = (0.0071028175068921871 * fTemp65);
			double fTemp262 = (0.030071220904747821 * fTemp43);
			double fTemp263 = (0.010535180889746089 * fTemp37);
			double fTemp264 = (0.021308452520676546 * fTemp12);
			double fTemp265 = (0.020818537549440744 * fTemp27);
			double fTemp266 = (fRec1[0] * ((fTemp256 + (fTemp257 + (fTemp258 + ((fTemp220 + (fTemp210 + fTemp259)) + fTemp260)))) - (fTemp261 + (fTemp262 + (fTemp263 + ((fTemp218 + (fTemp253 + fTemp264)) + fTemp265))))));
			fRec108[0] = max((fRec108[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp266))))));
			fVbargraph50 = FAUSTFLOAT(fRec108[0]);
			output34[i] = FAUSTFLOAT(fTemp266);
			double fTemp267 = (0.016998264729033874 * fTemp68);
			double fTemp268 = (0.020818537549440772 * fTemp43);
			double fTemp269 = (0.021308452520676563 * fTemp41);
			double fTemp270 = (0.0053753232814252004 * fTemp21);
			double fTemp271 = (0.0300712209047478 * fTemp27);
			double fTemp272 = (0.010535180889746086 * fTemp6);
			double fTemp273 = (0.0071028175068921853 * fTemp12);
			double fTemp274 = (0.028411270027568734 * fTemp15);
			double fTemp275 = (fRec1[0] * ((fTemp267 + (fTemp268 + (fTemp269 + (fTemp229 + ((fTemp210 + fTemp270) + fTemp271))))) - (fTemp222 + (fTemp224 + (fTemp260 + (fTemp218 + (((fTemp217 + fTemp272) + fTemp273) + fTemp274)))))));
			fRec109[0] = max((fRec109[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp275))))));
			fVbargraph51 = FAUSTFLOAT(fRec109[0]);
			output35[i] = FAUSTFLOAT(fTemp275);
			double fTemp276 = (0.020818537549440803 * fTemp43);
			double fTemp277 = (0.010535180889746074 * fTemp6);
			double fTemp278 = (0.0071028175068921775 * fTemp12);
			double fTemp279 = (0.016998264729033857 * fTemp68);
			double fTemp280 = (0.021308452520676542 * fTemp41);
			double fTemp281 = (0.030071220904747779 * fTemp27);
			double fTemp282 = (0.028411270027568748 * fTemp15);
			double fTemp283 = (0.0053753232814251943 * fTemp21);
			double fTemp284 = (0.022664352972045165 * fTemp24);
			double fTemp285 = (fRec1[0] * ((fTemp276 + (fTemp241 + ((fTemp210 + fTemp277) + fTemp278))) - (fTemp235 + (fTemp279 + (fTemp280 + (fTemp224 + (fTemp281 + (((fTemp218 + (fTemp217 + fTemp282)) + fTemp283) + fTemp284))))))));
			fRec110[0] = max((fRec110[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp285))))));
			fVbargraph52 = FAUSTFLOAT(fRec110[0]);
			output36[i] = FAUSTFLOAT(fTemp285);
			double fTemp286 = (0.0053753232814252143 * fTemp46);
			double fTemp287 = (0.031605542669238228 * fTemp6);
			double fTemp288 = (0.021308452520676539 * fTemp12);
			double fTemp289 = (0.028411270027568696 * fTemp15);
			double fTemp290 = (0.02266435297204512 * fTemp24);
			double fTemp291 = (0.02081853754944071 * fTemp27);
			double fTemp292 = (0.0071028175068922053 * fTemp65);
			double fTemp293 = (0.016998264729033919 * fTemp68);
			double fTemp294 = (0.030071220904747842 * fTemp43);
			double fTemp295 = (0.021308452520676618 * fTemp41);
			double fTemp296 = (0.010535180889746113 * fTemp37);
			double fTemp297 = (0.016125969844275587 * fTemp21);
			double fTemp298 = (fRec1[0] * ((fTemp286 + (((((fTemp210 + fTemp287) + fTemp288) + fTemp289) + fTemp290) + fTemp291)) - (fTemp292 + (fTemp293 + (fTemp294 + (fTemp295 + (fTemp296 + (fTemp219 + fTemp297))))))));
			fRec111[0] = max((fRec111[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp298))))));
			fVbargraph53 = FAUSTFLOAT(fRec111[0]);
			output37[i] = FAUSTFLOAT(fTemp298);
			double fTemp299 = (fTemp209 + fTemp217);
			double fTemp300 = (fRec1[0] * ((fTemp206 + (fTemp207 + (fTemp208 + (fTemp215 + (fTemp218 + (fTemp213 + (fTemp164 + fTemp211))))))) - (fTemp204 + (fTemp205 + (fTemp216 + (fTemp214 + (fTemp220 + (fTemp212 + fTemp299))))))));
			fRec112[0] = max((fRec112[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp300))))));
			fVbargraph54 = FAUSTFLOAT(fRec112[0]);
			output38[i] = FAUSTFLOAT(fTemp300);
			double fTemp301 = (fRec1[0] * ((fTemp223 + (fTemp224 + (fTemp232 + (fTemp218 + (fTemp164 + fTemp225))))) - (fTemp222 + (fTemp227 + (fTemp228 + (fTemp229 + (fTemp233 + (fTemp231 + (fTemp230 + (fTemp226 + fTemp299))))))))));
			fRec113[0] = max((fRec113[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp301))))));
			fVbargraph55 = FAUSTFLOAT(fRec113[0]);
			output39[i] = FAUSTFLOAT(fTemp301);
			double fTemp302 = (fRec1[0] * ((fTemp238 + (fTemp224 + (fTemp237 + (fTemp232 + (fTemp236 + (fTemp218 + (fTemp164 + fTemp243))))))) - (fTemp235 + (fTemp239 + (fTemp240 + (fTemp241 + (fTemp244 + (fTemp217 + (fTemp209 + fTemp242)))))))));
			fRec114[0] = max((fRec114[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp302))))));
			fVbargraph56 = FAUSTFLOAT(fRec114[0]);
			output40[i] = FAUSTFLOAT(fTemp302);
			double fTemp303 = (fTemp217 + (fTemp209 + fTemp211));
			double fTemp304 = (fRec1[0] * ((fTemp205 + (fTemp247 + (fTemp248 + (fTemp220 + (fTemp218 + (fTemp249 + (fTemp164 + fTemp212))))))) - (fTemp246 + (fTemp251 + (fTemp252 + (fTemp254 + (fTemp250 + fTemp303)))))));
			fRec115[0] = max((fRec115[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp304))))));
			fVbargraph57 = FAUSTFLOAT(fRec115[0]);
			output41[i] = FAUSTFLOAT(fTemp304);
			double fTemp305 = (fRec1[0] * ((fTemp261 + (fTemp257 + (fTemp258 + (fTemp220 + (fTemp218 + (fTemp259 + (fTemp164 + fTemp264))))))) - (fTemp256 + (fTemp262 + (fTemp263 + (fTemp265 + (fTemp260 + fTemp303)))))));
			fRec116[0] = max((fRec116[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp305))))));
			fVbargraph58 = FAUSTFLOAT(fRec116[0]);
			output42[i] = FAUSTFLOAT(fTemp305);
			double fTemp306 = (fRec1[0] * ((fTemp222 + (fTemp268 + (fTemp269 + (fTemp229 + (fTemp271 + (fTemp260 + (fTemp270 + (fTemp218 + (fTemp164 + fTemp273))))))))) - (fTemp267 + (fTemp224 + (fTemp274 + (fTemp217 + (fTemp209 + fTemp272)))))));
			fRec117[0] = max((fRec117[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp306))))));
			fVbargraph59 = FAUSTFLOAT(fRec117[0]);
			output43[i] = FAUSTFLOAT(fTemp306);
			double fTemp307 = (fRec1[0] * ((fTemp235 + (fTemp279 + (fTemp276 + (fTemp241 + (fTemp284 + (fTemp218 + (fTemp164 + fTemp277))))))) - (fTemp280 + (fTemp224 + (fTemp281 + (fTemp283 + (fTemp282 + (fTemp278 + fTemp299))))))));
			fRec118[0] = max((fRec118[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp307))))));
			fVbargraph60 = FAUSTFLOAT(fRec118[0]);
			output44[i] = FAUSTFLOAT(fTemp307);
			double fTemp308 = (fRec1[0] * ((fTemp292 + (fTemp293 + (fTemp286 + (fTemp291 + (fTemp218 + (fTemp289 + (fTemp164 + fTemp287))))))) - (fTemp294 + (fTemp295 + (fTemp296 + (fTemp290 + (fTemp297 + (fTemp288 + fTemp299))))))));
			fRec119[0] = max((fRec119[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp308))))));
			fVbargraph61 = FAUSTFLOAT(fRec119[0]);
			output45[i] = FAUSTFLOAT(fTemp308);
			double fTemp309 = (fTemp164 + fTemp167);
			double fTemp310 = (fRec1[0] * ((fTemp159 + (fTemp161 + (fTemp162 + (fTemp163 + (fTemp173 + (fTemp171 + fTemp309)))))) - (fTemp158 + (fTemp160 + (fTemp174 + (fTemp169 + (fTemp165 + fTemp172)))))));
			fRec120[0] = max((fRec120[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp310))))));
			fVbargraph62 = FAUSTFLOAT(fRec120[0]);
			output46[i] = FAUSTFLOAT(fTemp310);
			double fTemp311 = (fRec1[0] * ((fTemp177 + (fTemp160 + (fTemp161 + (fTemp163 + (fTemp176 + (fTemp180 + fTemp309)))))) - (fTemp178 + (fTemp162 + (fTemp171 + (fTemp169 + (fTemp165 + fTemp179)))))));
			fRec121[0] = max((fRec121[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp311))))));
			fVbargraph63 = FAUSTFLOAT(fRec121[0]);
			output47[i] = FAUSTFLOAT(fTemp311);
			double fTemp312 = (fRec1[0] * ((fTemp185 + (fTemp162 + (fTemp184 + (fTemp182 + (fTemp190 + fTemp309))))) - (fTemp160 + (fTemp186 + (fTemp187 + (fTemp188 + (fTemp183 + (fTemp191 + (fTemp169 + (fTemp165 + fTemp189))))))))));
			fRec122[0] = max((fRec122[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp312))))));
			fVbargraph64 = FAUSTFLOAT(fRec122[0]);
			output48[i] = FAUSTFLOAT(fTemp312);
			double fTemp313 = (fRec1[0] * ((fTemp196 + (fTemp160 + (fTemp201 + (fTemp195 + (fTemp167 + (fTemp164 + fTemp193)))))) - (fTemp197 + (fTemp162 + (fTemp198 + (fTemp199 + (fTemp202 + (fTemp169 + (fTemp200 + (fTemp165 + fTemp194))))))))));
			fRec123[0] = max((fRec123[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp313))))));
			fVbargraph65 = FAUSTFLOAT(fRec123[0]);
			output49[i] = FAUSTFLOAT(fTemp313);
			fRec1[1] = fRec1[0];
			fRec3[1] = fRec3[0];
			fRec2[1] = fRec2[0];
			fRec6[1] = fRec6[0];
			fRec5[1] = fRec5[0];
			fRec4[1] = fRec4[0];
			fRec9[1] = fRec9[0];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
			fRec12[1] = fRec12[0];
			fRec13[1] = fRec13[0];
			fRec11[1] = fRec11[0];
			fRec10[1] = fRec10[0];
			fRec16[1] = fRec16[0];
			fRec17[1] = fRec17[0];
			fRec15[1] = fRec15[0];
			fRec14[1] = fRec14[0];
			fRec20[1] = fRec20[0];
			fRec21[1] = fRec21[0];
			fRec19[1] = fRec19[0];
			fRec18[1] = fRec18[0];
			fRec25[1] = fRec25[0];
			fRec26[1] = fRec26[0];
			fRec24[1] = fRec24[0];
			fRec27[1] = fRec27[0];
			fRec23[1] = fRec23[0];
			fRec22[1] = fRec22[0];
			fRec31[1] = fRec31[0];
			fRec32[1] = fRec32[0];
			fRec30[1] = fRec30[0];
			fRec33[1] = fRec33[0];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
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
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fRec0[1] = fRec0[0];
			fRec46[1] = fRec46[0];
			fRec50[1] = fRec50[0];
			fRec49[1] = fRec49[0];
			fRec48[1] = fRec48[0];
			fRec53[1] = fRec53[0];
			fRec54[1] = fRec54[0];
			fRec52[1] = fRec52[0];
			fRec51[1] = fRec51[0];
			fRec58[1] = fRec58[0];
			fRec59[1] = fRec59[0];
			fRec57[1] = fRec57[0];
			fRec60[1] = fRec60[0];
			fRec56[1] = fRec56[0];
			fRec55[1] = fRec55[0];
			fRec64[1] = fRec64[0];
			fRec65[1] = fRec65[0];
			fRec63[1] = fRec63[0];
			fRec66[1] = fRec66[0];
			fRec62[1] = fRec62[0];
			fRec61[1] = fRec61[0];
			fRec47[1] = fRec47[0];
			fRec67[1] = fRec67[0];
			fRec68[1] = fRec68[0];
			fRec69[1] = fRec69[0];
			fRec70[1] = fRec70[0];
			fRec74[1] = fRec74[0];
			fRec75[1] = fRec75[0];
			fRec73[1] = fRec73[0];
			fRec72[1] = fRec72[0];
			fRec79[1] = fRec79[0];
			fRec80[1] = fRec80[0];
			fRec78[1] = fRec78[0];
			fRec81[1] = fRec81[0];
			fRec77[1] = fRec77[0];
			fRec76[1] = fRec76[0];
			fRec71[1] = fRec71[0];
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
			fRec114[1] = fRec114[0];
			fRec115[1] = fRec115[0];
			fRec116[1] = fRec116[0];
			fRec117[1] = fRec117[0];
			fRec118[1] = fRec118[0];
			fRec119[1] = fRec119[0];
			fRec120[1] = fRec120[0];
			fRec121[1] = fRec121[0];
			fRec122[1] = fRec122[0];
			fRec123[1] = fRec123[0];
			
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
    #if !defined(NDEBUG)
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

#if !defined(NDEBUG) & defined(SC_API_EXPORT)
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

#if !defined(NDEBUG)
    Print("Faust: %s numControls=%d\n", name.c_str(), g_numControls);
#endif // NDEBUG
}

// EOF

#endif
