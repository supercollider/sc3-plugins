/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAPanLebedev505"
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

double cos(double dummy0);
double sin(double dummy0);
double pow(double dummy0, double dummy1);
double mydsp_faustpower2_f(double value) {
	return (value * value);
	
}
double mydsp_faustpower4_f(double value) {
	return (((value * value) * value) * value);
	
}
double fabs(double dummy0);
double log10(double dummy0);
double mydsp_faustpower3_f(double value) {
	return ((value * value) * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
	
 private:
	
	int fSamplingFreq;
	double fConst0;
	double fConst1;
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fCheckbox0;
	double fConst2;
	double fConst3;
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fEntry0;
	FAUSTFLOAT fCheckbox1;
	FAUSTFLOAT fHslider3;
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
		m->declare("name", "HOAPanLebedev505");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
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
		fConst0 = min(192000.0, max(1000.0, double(fSamplingFreq)));
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
		fHslider1 = FAUSTFLOAT(0.0);
		fCheckbox0 = FAUSTFLOAT(0.0);
		fHslider2 = FAUSTFLOAT(2.0);
		fEntry0 = FAUSTFLOAT(1.0700000000000001);
		fCheckbox1 = FAUSTFLOAT(0.0);
		fHslider3 = FAUSTFLOAT(0.0);
		fCheckbox2 = FAUSTFLOAT(0.0);
		fCheckbox3 = FAUSTFLOAT(0.0);
		fCheckbox4 = FAUSTFLOAT(0.0);
		fCheckbox5 = FAUSTFLOAT(0.0);
		fCheckbox6 = FAUSTFLOAT(0.0);
		
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
		ui_interface->declare(&fHslider3, "0+1", "");
		ui_interface->declare(&fHslider3, "osc", "/gain_0 -20 20");
		ui_interface->declare(&fHslider3, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  0", &fHslider3, 0.0, -30.0, 20.0, 0.10000000000000001);
		ui_interface->declare(&fHslider2, "0+2", "");
		ui_interface->declare(&fHslider2, "osc", "/radius_0 0.5 50");
		ui_interface->declare(&fHslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  0", &fHslider2, 2.0, 0.5, 50.0, 0.01);
		ui_interface->declare(&fHslider0, "0+3", "");
		ui_interface->declare(&fHslider0, "osc", "/azimuth_0 0 360");
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth  0", &fHslider0, 0.0, -3.1415926535897931, 3.1415926535897931, 0.10000000000000001);
		ui_interface->declare(&fHslider1, "0+4", "");
		ui_interface->declare(&fHslider1, "osc", "/elevation_0 -90 90");
		ui_interface->declare(&fHslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation  0", &fHslider1, 0.0, -1.5707963267948966, 1.5707963267948966, 0.10000000000000001);
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
		ui_interface->declare(&fVbargraph0, "osc", "/output1");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2281280", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "osc", "/output2");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x228ca30", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph2, "osc", "/output3");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2298cc0", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph3, "osc", "/output4");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x22a5030", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph4, "osc", "/output5");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x22b1480", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph5, "osc", "/output6");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x22bc300", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph6, "osc", "/output7");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x22c7fb0", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph7, "osc", "/output8");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x22d33e0", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph8, "osc", "/output9");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x22de8f0", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph9, "osc", "/output10");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x22e9ee0", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph10, "osc", "/output11");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x22f6870", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph11, "osc", "/output12");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23032e0", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph12, "osc", "/output13");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x230fe30", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph13, "osc", "/output14");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x231ca60", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph14, "osc", "/output15");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23280c0", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph15, "osc", "/output16");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2333800", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph16, "osc", "/output17");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x233f020", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph17, "osc", "/output18");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x234a920", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph18, "osc", "/output19");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2357050", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph19, "osc", "/output20");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2362f00", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph20, "osc", "/output21");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x236ee90", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph21, "osc", "/output22");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x237af00", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph22, "osc", "/output23");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2386c60", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph23, "osc", "/output24");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2392aa0", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph24, "osc", "/output25");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x239e9c0", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openHorizontalBox("26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph25, "osc", "/output26");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23aa9c0", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph26, "osc", "/output27");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23b6da0", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph27, "osc", "/output28");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23c1e50", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph28, "osc", "/output29");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23ccfe0", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph29, "osc", "/output30");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23d8250", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph30, "osc", "/output31");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23e4700", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph31, "osc", "/output32");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23f0500", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph32, "osc", "/output33");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x23fc3e0", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph33, "osc", "/output34");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24083a0", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph34, "osc", "/output35");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2414440", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph35, "osc", "/output36");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24205c0", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph36, "osc", "/output37");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x242c820", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph37, "osc", "/output38");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2438b60", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph38, "osc", "/output39");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24441c0", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph39, "osc", "/output40");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x244f900", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph40, "osc", "/output41");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x245b120", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph41, "osc", "/output42");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2466a20", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph42, "osc", "/output43");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2472400", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph43, "osc", "/output44");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x247dec0", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph44, "osc", "/output45");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2489a60", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph45, "osc", "/output46");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24956e0", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph46, "osc", "/output47");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24a1440", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph47, "osc", "/output48");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24ad280", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph48, "osc", "/output49");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24b91a0", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph49, "osc", "/output50");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x24c51a0", &fVbargraph49, -70.0, 6.0);
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
		double fSlow1 = cos(fSlow0);
		double fSlow2 = double(fHslider1);
		double fSlow3 = cos(fSlow2);
		double fSlow4 = (fSlow1 * fSlow3);
		double fSlow5 = (6.123233995736766e-17 * fSlow4);
		double fSlow6 = sin(fSlow2);
		double fSlow7 = (fSlow5 + fSlow6);
		double fSlow8 = (1.0 - double(fCheckbox0));
		double fSlow9 = (3.0 * fSlow8);
		double fSlow10 = double(fHslider2);
		double fSlow11 = (fConst3 / fSlow10);
		double fSlow12 = (fSlow11 + 1.0);
		double fSlow13 = (fConst2 / (fSlow12 * fSlow10));
		double fSlow14 = double(fEntry0);
		double fSlow15 = (fConst3 / fSlow14);
		double fSlow16 = (1.0 / (fSlow15 + 1.0));
		double fSlow17 = double(fCheckbox1);
		double fSlow18 = (((fSlow12 * fSlow17) * fSlow14) / fSlow10);
		double fSlow19 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fHslider3))));
		double fSlow20 = (fConst2 / fSlow14);
		double fSlow21 = (1.0 - fSlow17);
		double fSlow22 = (fSlow21 * fSlow14);
		double fSlow23 = mydsp_faustpower2_f(fSlow7);
		double fSlow24 = (1.0 - double(fCheckbox2));
		double fSlow25 = (3.5 * (((5.0 * fSlow23) + -3.0) * fSlow24));
		double fSlow26 = ((fConst5 / fSlow10) + 1.0);
		double fSlow27 = (fConst4 / (fSlow26 * fSlow10));
		double fSlow28 = (1.0 / ((fConst5 / fSlow14) + 1.0));
		double fSlow29 = mydsp_faustpower2_f(fSlow15);
		double fSlow30 = (1.0 / (((fConst6 / fSlow14) + (6.4594326934833797 * fSlow29)) + 1.0));
		double fSlow31 = mydsp_faustpower2_f(fSlow11);
		double fSlow32 = (((fConst6 / fSlow10) + (6.4594326934833797 * fSlow31)) + 1.0);
		double fSlow33 = (((fSlow32 * fSlow17) * fSlow14) / fSlow10);
		double fSlow34 = (25.837730773933519 * fSlow29);
		double fSlow35 = (fSlow34 + (fConst7 / fSlow14));
		double fSlow36 = (1.0 / fSlow32);
		double fSlow37 = (25.837730773933519 * fSlow31);
		double fSlow38 = ((fConst7 / fSlow10) + fSlow37);
		double fSlow39 = (fConst4 / fSlow14);
		double fSlow40 = (1.0 - double(fCheckbox3));
		double fSlow41 = (1.375 * (((15.0 - (70.0 * fSlow23)) + (63.0 * mydsp_faustpower4_f(fSlow7))) * fSlow40));
		double fSlow42 = ((fConst9 / fSlow10) + 1.0);
		double fSlow43 = (fConst8 / (fSlow42 * fSlow10));
		double fSlow44 = (1.0 / ((fConst9 / fSlow14) + 1.0));
		double fSlow45 = (1.0 / (((fConst10 / fSlow14) + (14.272480513280099 * fSlow29)) + 1.0));
		double fSlow46 = (((fConst10 / fSlow10) + (14.272480513280099 * fSlow31)) + 1.0);
		double fSlow47 = (1.0 / (((fConst11 / fSlow14) + (18.1563153134523 * fSlow29)) + 1.0));
		double fSlow48 = (((fConst11 / fSlow10) + (18.1563153134523 * fSlow31)) + 1.0);
		double fSlow49 = (((fSlow48 * fSlow17) * fSlow14) / fSlow10);
		double fSlow50 = (72.6252612538092 * fSlow29);
		double fSlow51 = (fSlow50 + (fConst12 / fSlow14));
		double fSlow52 = (1.0 / fSlow48);
		double fSlow53 = (72.6252612538092 * fSlow31);
		double fSlow54 = ((fConst12 / fSlow10) + fSlow53);
		double fSlow55 = (57.089922053120397 * fSlow29);
		double fSlow56 = (fSlow55 + (fConst13 / fSlow14));
		double fSlow57 = (1.0 / fSlow46);
		double fSlow58 = (57.089922053120397 * fSlow31);
		double fSlow59 = ((fConst13 / fSlow10) + fSlow58);
		double fSlow60 = (fConst8 / fSlow14);
		double fSlow61 = ((((fSlow17 / fSlow10) + fSlow21) * fSlow14) * (1.0 - double(fCheckbox4)));
		double fSlow62 = (1.0 - double(fCheckbox5));
		double fSlow63 = (2.5 * (((3.0 * fSlow23) + -1.0) * fSlow62));
		double fSlow64 = (((fConst14 / fSlow10) + (3.0 * fSlow31)) + 1.0);
		double fSlow65 = (1.0 / fSlow64);
		double fSlow66 = (12.0 * fSlow31);
		double fSlow67 = ((fConst15 / fSlow10) + fSlow66);
		double fSlow68 = (1.0 / (((fConst14 / fSlow14) + (3.0 * fSlow29)) + 1.0));
		double fSlow69 = (((fSlow64 * fSlow17) * fSlow14) / fSlow10);
		double fSlow70 = (12.0 * fSlow29);
		double fSlow71 = (fSlow70 + (fConst15 / fSlow14));
		double fSlow72 = (1.0 - double(fCheckbox6));
		double fSlow73 = (1.125 * (((fSlow23 * ((35.0 * fSlow23) + -30.0)) + 3.0) * fSlow72));
		double fSlow74 = (((fConst16 / fSlow10) + (9.1401308902779004 * fSlow31)) + 1.0);
		double fSlow75 = (1.0 / fSlow74);
		double fSlow76 = (36.560523561111602 * fSlow31);
		double fSlow77 = ((fConst17 / fSlow10) + fSlow76);
		double fSlow78 = (1.0 / (((fConst16 / fSlow14) + (9.1401308902779004 * fSlow29)) + 1.0));
		double fSlow79 = (1.0 / (((fConst18 / fSlow14) + (11.4878004768713 * fSlow29)) + 1.0));
		double fSlow80 = (((fConst18 / fSlow10) + (11.4878004768713 * fSlow31)) + 1.0);
		double fSlow81 = (((fSlow80 * fSlow17) * fSlow14) / fSlow10);
		double fSlow82 = (45.951201907485199 * fSlow29);
		double fSlow83 = (fSlow82 + (fConst19 / fSlow14));
		double fSlow84 = (1.0 / fSlow80);
		double fSlow85 = (45.951201907485199 * fSlow31);
		double fSlow86 = ((fConst19 / fSlow10) + fSlow85);
		double fSlow87 = (36.560523561111602 * fSlow29);
		double fSlow88 = (fSlow87 + (fConst17 / fSlow14));
		double fSlow89 = (3.0 * ((fSlow1 * fSlow8) * fSlow3));
		double fSlow90 = mydsp_faustpower2_f(fSlow4);
		double fSlow91 = (2.5 * (((3.0 * fSlow90) + -1.0) * fSlow62));
		double fSlow92 = mydsp_faustpower3_f(fSlow4);
		double fSlow93 = (3.5 * (((5.0 * fSlow92) - (3.0 * fSlow4)) * fSlow24));
		double fSlow94 = (1.125 * (((fSlow90 * ((35.0 * fSlow90) + -30.0)) + 3.0) * fSlow72));
		double fSlow95 = (1.375 * (((15.0 * fSlow4) + (fSlow92 * ((63.0 * fSlow90) + -70.0))) * fSlow40));
		double fSlow96 = cos((fSlow0 + -1.5707963267948966));
		double fSlow97 = (3.0 * ((fSlow96 * fSlow8) * fSlow3));
		double fSlow98 = (fSlow96 * fSlow3);
		double fSlow99 = mydsp_faustpower2_f(fSlow98);
		double fSlow100 = (2.5 * (((3.0 * fSlow99) + -1.0) * fSlow62));
		double fSlow101 = mydsp_faustpower3_f(fSlow98);
		double fSlow102 = (3.5 * (((5.0 * fSlow101) - (3.0 * fSlow98)) * fSlow24));
		double fSlow103 = (1.125 * (((fSlow99 * ((35.0 * fSlow99) + -30.0)) + 3.0) * fSlow72));
		double fSlow104 = (1.375 * (((15.0 * fSlow98) + (fSlow101 * ((63.0 * fSlow99) + -70.0))) * fSlow40));
		double fSlow105 = cos((fSlow0 + -3.1415926535897931));
		double fSlow106 = (3.0 * ((fSlow105 * fSlow8) * fSlow3));
		double fSlow107 = (fSlow105 * fSlow3);
		double fSlow108 = mydsp_faustpower2_f(fSlow107);
		double fSlow109 = (2.5 * (((3.0 * fSlow108) + -1.0) * fSlow62));
		double fSlow110 = mydsp_faustpower3_f(fSlow107);
		double fSlow111 = (3.5 * (((5.0 * fSlow110) - (3.0 * fSlow107)) * fSlow24));
		double fSlow112 = (1.125 * (((fSlow108 * ((35.0 * fSlow108) + -30.0)) + 3.0) * fSlow72));
		double fSlow113 = (1.375 * (((15.0 * fSlow107) + (fSlow110 * ((63.0 * fSlow108) + -70.0))) * fSlow40));
		double fSlow114 = cos((fSlow0 + -4.7123889803846897));
		double fSlow115 = (3.0 * ((fSlow114 * fSlow8) * fSlow3));
		double fSlow116 = (fSlow114 * fSlow3);
		double fSlow117 = mydsp_faustpower2_f(fSlow116);
		double fSlow118 = (2.5 * (((3.0 * fSlow117) + -1.0) * fSlow62));
		double fSlow119 = mydsp_faustpower3_f(fSlow116);
		double fSlow120 = (3.5 * (((5.0 * fSlow119) - (3.0 * fSlow116)) * fSlow24));
		double fSlow121 = (1.125 * (((fSlow117 * ((35.0 * fSlow117) + -30.0)) + 3.0) * fSlow72));
		double fSlow122 = (1.375 * (((15.0 * fSlow116) + (fSlow119 * ((63.0 * fSlow117) + -70.0))) * fSlow40));
		double fSlow123 = (fSlow5 - fSlow6);
		double fSlow124 = mydsp_faustpower2_f(fSlow123);
		double fSlow125 = (3.5 * (((5.0 * fSlow124) + -3.0) * fSlow24));
		double fSlow126 = (1.375 * (((15.0 - (70.0 * fSlow124)) + (63.0 * mydsp_faustpower4_f(fSlow123))) * fSlow40));
		double fSlow127 = (2.5 * (((3.0 * fSlow124) + -1.0) * fSlow62));
		double fSlow128 = (1.125 * (((fSlow124 * ((35.0 * fSlow124) + -30.0)) + 3.0) * fSlow72));
		double fSlow129 = (0.70710678118654757 * fSlow4);
		double fSlow130 = (0.70710678118654746 * fSlow6);
		double fSlow131 = (fSlow129 + fSlow130);
		double fSlow132 = mydsp_faustpower2_f(fSlow131);
		double fSlow133 = (3.5 * (((5.0 * fSlow132) + -3.0) * fSlow24));
		double fSlow134 = (1.375 * (((15.0 - (70.0 * fSlow132)) + (63.0 * mydsp_faustpower4_f(fSlow131))) * fSlow40));
		double fSlow135 = (2.5 * (((3.0 * fSlow132) + -1.0) * fSlow62));
		double fSlow136 = (1.125 * (((fSlow132 * ((35.0 * fSlow132) + -30.0)) + 3.0) * fSlow72));
		double fSlow137 = (0.70710678118654757 * fSlow98);
		double fSlow138 = (fSlow137 + fSlow130);
		double fSlow139 = mydsp_faustpower2_f(fSlow138);
		double fSlow140 = (3.5 * (((5.0 * fSlow139) + -3.0) * fSlow24));
		double fSlow141 = (1.375 * (((15.0 - (70.0 * fSlow139)) + (63.0 * mydsp_faustpower4_f(fSlow138))) * fSlow40));
		double fSlow142 = (2.5 * (((3.0 * fSlow139) + -1.0) * fSlow62));
		double fSlow143 = (1.125 * (((fSlow139 * ((35.0 * fSlow139) + -30.0)) + 3.0) * fSlow72));
		double fSlow144 = (0.70710678118654757 * fSlow107);
		double fSlow145 = (fSlow130 + fSlow144);
		double fSlow146 = mydsp_faustpower2_f(fSlow145);
		double fSlow147 = (3.5 * (((5.0 * fSlow146) + -3.0) * fSlow24));
		double fSlow148 = (1.375 * (((15.0 - (70.0 * fSlow146)) + (63.0 * mydsp_faustpower4_f(fSlow145))) * fSlow40));
		double fSlow149 = (2.5 * (((3.0 * fSlow146) + -1.0) * fSlow62));
		double fSlow150 = (1.125 * (((fSlow146 * ((35.0 * fSlow146) + -30.0)) + 3.0) * fSlow72));
		double fSlow151 = (0.70710678118654757 * fSlow116);
		double fSlow152 = (fSlow130 + fSlow151);
		double fSlow153 = mydsp_faustpower2_f(fSlow152);
		double fSlow154 = (3.5 * (((5.0 * fSlow153) + -3.0) * fSlow24));
		double fSlow155 = (1.375 * (((15.0 - (70.0 * fSlow153)) + (63.0 * mydsp_faustpower4_f(fSlow152))) * fSlow40));
		double fSlow156 = (2.5 * (((3.0 * fSlow153) + -1.0) * fSlow62));
		double fSlow157 = (1.125 * (((fSlow153 * ((35.0 * fSlow153) + -30.0)) + 3.0) * fSlow72));
		double fSlow158 = cos((fSlow0 + -0.78539816339744828));
		double fSlow159 = (3.0 * ((fSlow158 * fSlow8) * fSlow3));
		double fSlow160 = (fSlow158 * fSlow3);
		double fSlow161 = mydsp_faustpower2_f(fSlow160);
		double fSlow162 = (2.5 * (((3.0 * fSlow161) + -1.0) * fSlow62));
		double fSlow163 = mydsp_faustpower3_f(fSlow160);
		double fSlow164 = (3.5 * (((5.0 * fSlow163) - (3.0 * fSlow160)) * fSlow24));
		double fSlow165 = (1.125 * (((fSlow161 * ((35.0 * fSlow161) + -30.0)) + 3.0) * fSlow72));
		double fSlow166 = (1.375 * (((15.0 * fSlow160) + (fSlow163 * ((63.0 * fSlow161) + -70.0))) * fSlow40));
		double fSlow167 = cos((fSlow0 + -2.3561944901923448));
		double fSlow168 = (3.0 * ((fSlow167 * fSlow8) * fSlow3));
		double fSlow169 = (fSlow167 * fSlow3);
		double fSlow170 = mydsp_faustpower2_f(fSlow169);
		double fSlow171 = (2.5 * (((3.0 * fSlow170) + -1.0) * fSlow62));
		double fSlow172 = mydsp_faustpower3_f(fSlow169);
		double fSlow173 = (3.5 * (((5.0 * fSlow172) - (3.0 * fSlow169)) * fSlow24));
		double fSlow174 = (1.125 * (((fSlow170 * ((35.0 * fSlow170) + -30.0)) + 3.0) * fSlow72));
		double fSlow175 = (1.375 * (((15.0 * fSlow169) + (fSlow172 * ((63.0 * fSlow170) + -70.0))) * fSlow40));
		double fSlow176 = cos((fSlow0 + -3.9269908169872414));
		double fSlow177 = (3.0 * ((fSlow176 * fSlow8) * fSlow3));
		double fSlow178 = (fSlow176 * fSlow3);
		double fSlow179 = mydsp_faustpower2_f(fSlow178);
		double fSlow180 = (2.5 * (((3.0 * fSlow179) + -1.0) * fSlow62));
		double fSlow181 = mydsp_faustpower3_f(fSlow178);
		double fSlow182 = (3.5 * (((5.0 * fSlow181) - (3.0 * fSlow178)) * fSlow24));
		double fSlow183 = (1.125 * (((fSlow179 * ((35.0 * fSlow179) + -30.0)) + 3.0) * fSlow72));
		double fSlow184 = (1.375 * (((15.0 * fSlow178) + (fSlow181 * ((63.0 * fSlow179) + -70.0))) * fSlow40));
		double fSlow185 = cos((fSlow0 + -5.497787143782138));
		double fSlow186 = (3.0 * ((fSlow185 * fSlow8) * fSlow3));
		double fSlow187 = (fSlow185 * fSlow3);
		double fSlow188 = mydsp_faustpower2_f(fSlow187);
		double fSlow189 = (2.5 * (((3.0 * fSlow188) + -1.0) * fSlow62));
		double fSlow190 = mydsp_faustpower3_f(fSlow187);
		double fSlow191 = (3.5 * (((5.0 * fSlow190) - (3.0 * fSlow187)) * fSlow24));
		double fSlow192 = (1.125 * (((fSlow188 * ((35.0 * fSlow188) + -30.0)) + 3.0) * fSlow72));
		double fSlow193 = (1.375 * (((15.0 * fSlow187) + (fSlow190 * ((63.0 * fSlow188) + -70.0))) * fSlow40));
		double fSlow194 = (fSlow129 - fSlow130);
		double fSlow195 = mydsp_faustpower2_f(fSlow194);
		double fSlow196 = (3.5 * (((5.0 * fSlow195) + -3.0) * fSlow24));
		double fSlow197 = (1.375 * (((15.0 - (70.0 * fSlow195)) + (63.0 * mydsp_faustpower4_f(fSlow194))) * fSlow40));
		double fSlow198 = (2.5 * (((3.0 * fSlow195) + -1.0) * fSlow62));
		double fSlow199 = (1.125 * (((fSlow195 * ((35.0 * fSlow195) + -30.0)) + 3.0) * fSlow72));
		double fSlow200 = (fSlow137 - fSlow130);
		double fSlow201 = mydsp_faustpower2_f(fSlow200);
		double fSlow202 = (3.5 * (((5.0 * fSlow201) + -3.0) * fSlow24));
		double fSlow203 = (1.375 * (((15.0 - (70.0 * fSlow201)) + (63.0 * mydsp_faustpower4_f(fSlow200))) * fSlow40));
		double fSlow204 = (2.5 * (((3.0 * fSlow201) + -1.0) * fSlow62));
		double fSlow205 = (1.125 * (((fSlow201 * ((35.0 * fSlow201) + -30.0)) + 3.0) * fSlow72));
		double fSlow206 = (fSlow144 - fSlow130);
		double fSlow207 = mydsp_faustpower2_f(fSlow206);
		double fSlow208 = (3.5 * (((5.0 * fSlow207) + -3.0) * fSlow24));
		double fSlow209 = (1.375 * (((15.0 - (70.0 * fSlow207)) + (63.0 * mydsp_faustpower4_f(fSlow206))) * fSlow40));
		double fSlow210 = (1.125 * (((fSlow207 * ((35.0 * fSlow207) + -30.0)) + 3.0) * fSlow72));
		double fSlow211 = (2.5 * (((3.0 * fSlow207) + -1.0) * fSlow62));
		double fSlow212 = (fSlow151 - fSlow130);
		double fSlow213 = mydsp_faustpower2_f(fSlow212);
		double fSlow214 = (3.5 * (((5.0 * fSlow213) + -3.0) * fSlow24));
		double fSlow215 = (1.375 * (((15.0 - (70.0 * fSlow213)) + (63.0 * mydsp_faustpower4_f(fSlow212))) * fSlow40));
		double fSlow216 = (2.5 * (((3.0 * fSlow213) + -1.0) * fSlow62));
		double fSlow217 = (1.125 * (((fSlow213 * ((35.0 * fSlow213) + -30.0)) + 3.0) * fSlow72));
		double fSlow218 = (0.81649658092772592 * fSlow160);
		double fSlow219 = (0.57735026918962584 * fSlow6);
		double fSlow220 = (fSlow218 + fSlow219);
		double fSlow221 = mydsp_faustpower2_f(fSlow220);
		double fSlow222 = (3.5 * (((5.0 * fSlow221) + -3.0) * fSlow24));
		double fSlow223 = (1.375 * (((15.0 - (70.0 * fSlow221)) + (63.0 * mydsp_faustpower4_f(fSlow220))) * fSlow40));
		double fSlow224 = (2.5 * (((3.0 * fSlow221) + -1.0) * fSlow62));
		double fSlow225 = (1.125 * (((fSlow221 * ((35.0 * fSlow221) + -30.0)) + 3.0) * fSlow72));
		double fSlow226 = (0.81649658092772592 * fSlow169);
		double fSlow227 = (fSlow219 + fSlow226);
		double fSlow228 = mydsp_faustpower2_f(fSlow227);
		double fSlow229 = (3.5 * (((5.0 * fSlow228) + -3.0) * fSlow24));
		double fSlow230 = (1.375 * (((15.0 - (70.0 * fSlow228)) + (63.0 * mydsp_faustpower4_f(fSlow227))) * fSlow40));
		double fSlow231 = (2.5 * (((3.0 * fSlow228) + -1.0) * fSlow62));
		double fSlow232 = (1.125 * (((fSlow228 * ((35.0 * fSlow228) + -30.0)) + 3.0) * fSlow72));
		double fSlow233 = (0.81649658092772592 * fSlow178);
		double fSlow234 = (fSlow219 + fSlow233);
		double fSlow235 = mydsp_faustpower2_f(fSlow234);
		double fSlow236 = (3.5 * (((5.0 * fSlow235) + -3.0) * fSlow24));
		double fSlow237 = (1.375 * (((15.0 - (70.0 * fSlow235)) + (63.0 * mydsp_faustpower4_f(fSlow234))) * fSlow40));
		double fSlow238 = (2.5 * (((3.0 * fSlow235) + -1.0) * fSlow62));
		double fSlow239 = (1.125 * (((fSlow235 * ((35.0 * fSlow235) + -30.0)) + 3.0) * fSlow72));
		double fSlow240 = (0.81649658092772592 * fSlow187);
		double fSlow241 = (fSlow219 + fSlow240);
		double fSlow242 = mydsp_faustpower2_f(fSlow241);
		double fSlow243 = (3.5 * (((5.0 * fSlow242) + -3.0) * fSlow24));
		double fSlow244 = (1.375 * (((15.0 - (70.0 * fSlow242)) + (63.0 * mydsp_faustpower4_f(fSlow241))) * fSlow40));
		double fSlow245 = (2.5 * (((3.0 * fSlow242) + -1.0) * fSlow62));
		double fSlow246 = (1.125 * (((fSlow242 * ((35.0 * fSlow242) + -30.0)) + 3.0) * fSlow72));
		double fSlow247 = (fSlow218 - fSlow219);
		double fSlow248 = mydsp_faustpower2_f(fSlow247);
		double fSlow249 = (3.5 * (((5.0 * fSlow248) + -3.0) * fSlow24));
		double fSlow250 = (1.375 * (((15.0 - (70.0 * fSlow248)) + (63.0 * mydsp_faustpower4_f(fSlow247))) * fSlow40));
		double fSlow251 = (2.5 * (((3.0 * fSlow248) + -1.0) * fSlow62));
		double fSlow252 = (1.125 * (((fSlow248 * ((35.0 * fSlow248) + -30.0)) + 3.0) * fSlow72));
		double fSlow253 = (fSlow226 - fSlow219);
		double fSlow254 = mydsp_faustpower2_f(fSlow253);
		double fSlow255 = (3.5 * (((5.0 * fSlow254) + -3.0) * fSlow24));
		double fSlow256 = (1.375 * (((15.0 - (70.0 * fSlow254)) + (63.0 * mydsp_faustpower4_f(fSlow253))) * fSlow40));
		double fSlow257 = (2.5 * (((3.0 * fSlow254) + -1.0) * fSlow62));
		double fSlow258 = (1.125 * (((fSlow254 * ((35.0 * fSlow254) + -30.0)) + 3.0) * fSlow72));
		double fSlow259 = (fSlow233 - fSlow219);
		double fSlow260 = mydsp_faustpower2_f(fSlow259);
		double fSlow261 = (3.5 * (((5.0 * fSlow260) + -3.0) * fSlow24));
		double fSlow262 = (1.375 * (((15.0 - (70.0 * fSlow260)) + (63.0 * mydsp_faustpower4_f(fSlow259))) * fSlow40));
		double fSlow263 = (2.5 * (((3.0 * fSlow260) + -1.0) * fSlow62));
		double fSlow264 = (1.125 * (((fSlow260 * ((35.0 * fSlow260) + -30.0)) + 3.0) * fSlow72));
		double fSlow265 = (fSlow240 - fSlow219);
		double fSlow266 = mydsp_faustpower2_f(fSlow265);
		double fSlow267 = (3.5 * (((5.0 * fSlow266) + -3.0) * fSlow24));
		double fSlow268 = (1.375 * (((15.0 - (70.0 * fSlow266)) + (63.0 * mydsp_faustpower4_f(fSlow265))) * fSlow40));
		double fSlow269 = (2.5 * (((3.0 * fSlow266) + -1.0) * fSlow62));
		double fSlow270 = (1.125 * (((fSlow266 * ((35.0 * fSlow266) + -30.0)) + 3.0) * fSlow72));
		double fSlow271 = (0.42640143271122027 * (cos((fSlow0 + -0.78539816339744839)) * fSlow3));
		double fSlow272 = (0.90453403373329111 * fSlow6);
		double fSlow273 = (fSlow271 + fSlow272);
		double fSlow274 = mydsp_faustpower2_f(fSlow273);
		double fSlow275 = (3.5 * (((5.0 * fSlow274) + -3.0) * fSlow24));
		double fSlow276 = (1.375 * (((15.0 - (70.0 * fSlow274)) + (63.0 * mydsp_faustpower4_f(fSlow273))) * fSlow40));
		double fSlow277 = (2.5 * (((3.0 * fSlow274) + -1.0) * fSlow62));
		double fSlow278 = (1.125 * (((fSlow274 * ((35.0 * fSlow274) + -30.0)) + 3.0) * fSlow72));
		double fSlow279 = (0.42640143271122027 * fSlow169);
		double fSlow280 = (fSlow272 + fSlow279);
		double fSlow281 = mydsp_faustpower2_f(fSlow280);
		double fSlow282 = (3.5 * (((5.0 * fSlow281) + -3.0) * fSlow24));
		double fSlow283 = (1.375 * (((15.0 - (70.0 * fSlow281)) + (63.0 * mydsp_faustpower4_f(fSlow280))) * fSlow40));
		double fSlow284 = (2.5 * (((3.0 * fSlow281) + -1.0) * fSlow62));
		double fSlow285 = (1.125 * (((fSlow281 * ((35.0 * fSlow281) + -30.0)) + 3.0) * fSlow72));
		double fSlow286 = (0.42640143271122027 * fSlow178);
		double fSlow287 = (fSlow272 + fSlow286);
		double fSlow288 = mydsp_faustpower2_f(fSlow287);
		double fSlow289 = (3.5 * (((5.0 * fSlow288) + -3.0) * fSlow24));
		double fSlow290 = (1.375 * (((15.0 - (70.0 * fSlow288)) + (63.0 * mydsp_faustpower4_f(fSlow287))) * fSlow40));
		double fSlow291 = (2.5 * (((3.0 * fSlow288) + -1.0) * fSlow62));
		double fSlow292 = (1.125 * (((fSlow288 * ((35.0 * fSlow288) + -30.0)) + 3.0) * fSlow72));
		double fSlow293 = (0.42640143271122027 * fSlow187);
		double fSlow294 = (fSlow272 + fSlow293);
		double fSlow295 = mydsp_faustpower2_f(fSlow294);
		double fSlow296 = (3.5 * (((5.0 * fSlow295) + -3.0) * fSlow24));
		double fSlow297 = (1.375 * (((15.0 - (70.0 * fSlow295)) + (63.0 * mydsp_faustpower4_f(fSlow294))) * fSlow40));
		double fSlow298 = (2.5 * (((3.0 * fSlow295) + -1.0) * fSlow62));
		double fSlow299 = (1.125 * (((fSlow295 * ((35.0 * fSlow295) + -30.0)) + 3.0) * fSlow72));
		double fSlow300 = (0.95346258924559235 * (cos((fSlow0 + -0.32175055439664263)) * fSlow3));
		double fSlow301 = (0.30151134457776352 * fSlow6);
		double fSlow302 = (fSlow300 + fSlow301);
		double fSlow303 = mydsp_faustpower2_f(fSlow302);
		double fSlow304 = (3.5 * (((5.0 * fSlow303) + -3.0) * fSlow24));
		double fSlow305 = (1.375 * (((15.0 - (70.0 * fSlow303)) + (63.0 * mydsp_faustpower4_f(fSlow302))) * fSlow40));
		double fSlow306 = (2.5 * (((3.0 * fSlow303) + -1.0) * fSlow62));
		double fSlow307 = (1.125 * (((fSlow303 * ((35.0 * fSlow303) + -30.0)) + 3.0) * fSlow72));
		double fSlow308 = (0.95346258924559235 * (cos((fSlow0 + -1.2490457723982544)) * fSlow3));
		double fSlow309 = (fSlow301 + fSlow308);
		double fSlow310 = mydsp_faustpower2_f(fSlow309);
		double fSlow311 = (3.5 * (((5.0 * fSlow310) + -3.0) * fSlow24));
		double fSlow312 = (1.375 * (((15.0 - (70.0 * fSlow310)) + (63.0 * mydsp_faustpower4_f(fSlow309))) * fSlow40));
		double fSlow313 = (2.5 * (((3.0 * fSlow310) + -1.0) * fSlow62));
		double fSlow314 = (1.125 * (((fSlow310 * ((35.0 * fSlow310) + -30.0)) + 3.0) * fSlow72));
		double fSlow315 = (0.95346258924559235 * (cos((fSlow0 + -1.8925468811915387)) * fSlow3));
		double fSlow316 = (fSlow301 + fSlow315);
		double fSlow317 = mydsp_faustpower2_f(fSlow316);
		double fSlow318 = (3.5 * (((5.0 * fSlow317) + -3.0) * fSlow24));
		double fSlow319 = (1.375 * (((15.0 - (70.0 * fSlow317)) + (63.0 * mydsp_faustpower4_f(fSlow316))) * fSlow40));
		double fSlow320 = (2.5 * (((3.0 * fSlow317) + -1.0) * fSlow62));
		double fSlow321 = (1.125 * (((fSlow317 * ((35.0 * fSlow317) + -30.0)) + 3.0) * fSlow72));
		double fSlow322 = (0.95346258924559235 * (cos((fSlow0 + -2.8198420991931505)) * fSlow3));
		double fSlow323 = (fSlow301 + fSlow322);
		double fSlow324 = mydsp_faustpower2_f(fSlow323);
		double fSlow325 = (3.5 * (((5.0 * fSlow324) + -3.0) * fSlow24));
		double fSlow326 = (1.375 * (((15.0 - (70.0 * fSlow324)) + (63.0 * mydsp_faustpower4_f(fSlow323))) * fSlow40));
		double fSlow327 = (2.5 * (((3.0 * fSlow324) + -1.0) * fSlow62));
		double fSlow328 = (1.125 * (((fSlow324 * ((35.0 * fSlow324) + -30.0)) + 3.0) * fSlow72));
		double fSlow329 = (0.95346258924559235 * (cos((fSlow0 + -3.4633432079864357)) * fSlow3));
		double fSlow330 = (fSlow301 + fSlow329);
		double fSlow331 = mydsp_faustpower2_f(fSlow330);
		double fSlow332 = (3.5 * (((5.0 * fSlow331) + -3.0) * fSlow24));
		double fSlow333 = (1.375 * (((15.0 - (70.0 * fSlow331)) + (63.0 * mydsp_faustpower4_f(fSlow330))) * fSlow40));
		double fSlow334 = (2.5 * (((3.0 * fSlow331) + -1.0) * fSlow62));
		double fSlow335 = (1.125 * (((fSlow331 * ((35.0 * fSlow331) + -30.0)) + 3.0) * fSlow72));
		double fSlow336 = (0.95346258924559235 * (cos((fSlow0 + -4.3906384259880475)) * fSlow3));
		double fSlow337 = (fSlow301 + fSlow336);
		double fSlow338 = mydsp_faustpower2_f(fSlow337);
		double fSlow339 = (3.5 * (((5.0 * fSlow338) + -3.0) * fSlow24));
		double fSlow340 = (1.375 * (((15.0 - (70.0 * fSlow338)) + (63.0 * mydsp_faustpower4_f(fSlow337))) * fSlow40));
		double fSlow341 = (2.5 * (((3.0 * fSlow338) + -1.0) * fSlow62));
		double fSlow342 = (1.125 * (((fSlow338 * ((35.0 * fSlow338) + -30.0)) + 3.0) * fSlow72));
		double fSlow343 = (0.95346258924559235 * (cos((fSlow0 + -5.0341395347813318)) * fSlow3));
		double fSlow344 = (fSlow301 + fSlow343);
		double fSlow345 = mydsp_faustpower2_f(fSlow344);
		double fSlow346 = (3.5 * (((5.0 * fSlow345) + -3.0) * fSlow24));
		double fSlow347 = (1.375 * (((15.0 - (70.0 * fSlow345)) + (63.0 * mydsp_faustpower4_f(fSlow344))) * fSlow40));
		double fSlow348 = (2.5 * (((3.0 * fSlow345) + -1.0) * fSlow62));
		double fSlow349 = (1.125 * (((fSlow345 * ((35.0 * fSlow345) + -30.0)) + 3.0) * fSlow72));
		double fSlow350 = (0.95346258924559235 * (cos((fSlow0 + -5.9614347527829432)) * fSlow3));
		double fSlow351 = (fSlow301 + fSlow350);
		double fSlow352 = mydsp_faustpower2_f(fSlow351);
		double fSlow353 = (3.5 * (((5.0 * fSlow352) + -3.0) * fSlow24));
		double fSlow354 = (1.375 * (((15.0 - (70.0 * fSlow352)) + (63.0 * mydsp_faustpower4_f(fSlow351))) * fSlow40));
		double fSlow355 = (2.5 * (((3.0 * fSlow352) + -1.0) * fSlow62));
		double fSlow356 = (1.125 * (((fSlow352 * ((35.0 * fSlow352) + -30.0)) + 3.0) * fSlow72));
		double fSlow357 = (fSlow300 - fSlow301);
		double fSlow358 = mydsp_faustpower2_f(fSlow357);
		double fSlow359 = (3.5 * (((5.0 * fSlow358) + -3.0) * fSlow24));
		double fSlow360 = (1.375 * (((15.0 - (70.0 * fSlow358)) + (63.0 * mydsp_faustpower4_f(fSlow357))) * fSlow40));
		double fSlow361 = (2.5 * (((3.0 * fSlow358) + -1.0) * fSlow62));
		double fSlow362 = (1.125 * (((fSlow358 * ((35.0 * fSlow358) + -30.0)) + 3.0) * fSlow72));
		double fSlow363 = (fSlow308 - fSlow301);
		double fSlow364 = mydsp_faustpower2_f(fSlow363);
		double fSlow365 = (3.5 * (((5.0 * fSlow364) + -3.0) * fSlow24));
		double fSlow366 = (1.375 * (((15.0 - (70.0 * fSlow364)) + (63.0 * mydsp_faustpower4_f(fSlow363))) * fSlow40));
		double fSlow367 = (2.5 * (((3.0 * fSlow364) + -1.0) * fSlow62));
		double fSlow368 = (1.125 * (((fSlow364 * ((35.0 * fSlow364) + -30.0)) + 3.0) * fSlow72));
		double fSlow369 = (fSlow315 - fSlow301);
		double fSlow370 = mydsp_faustpower2_f(fSlow369);
		double fSlow371 = (3.5 * (((5.0 * fSlow370) + -3.0) * fSlow24));
		double fSlow372 = (1.375 * (((15.0 - (70.0 * fSlow370)) + (63.0 * mydsp_faustpower4_f(fSlow369))) * fSlow40));
		double fSlow373 = (2.5 * (((3.0 * fSlow370) + -1.0) * fSlow62));
		double fSlow374 = (1.125 * (((fSlow370 * ((35.0 * fSlow370) + -30.0)) + 3.0) * fSlow72));
		double fSlow375 = (fSlow322 - fSlow301);
		double fSlow376 = mydsp_faustpower2_f(fSlow375);
		double fSlow377 = (3.5 * (((5.0 * fSlow376) + -3.0) * fSlow24));
		double fSlow378 = (1.375 * (((15.0 - (70.0 * fSlow376)) + (63.0 * mydsp_faustpower4_f(fSlow375))) * fSlow40));
		double fSlow379 = (2.5 * (((3.0 * fSlow376) + -1.0) * fSlow62));
		double fSlow380 = (1.125 * (((fSlow376 * ((35.0 * fSlow376) + -30.0)) + 3.0) * fSlow72));
		double fSlow381 = (fSlow329 - fSlow301);
		double fSlow382 = mydsp_faustpower2_f(fSlow381);
		double fSlow383 = (3.5 * (((5.0 * fSlow382) + -3.0) * fSlow24));
		double fSlow384 = (1.375 * (((15.0 - (70.0 * fSlow382)) + (63.0 * mydsp_faustpower4_f(fSlow381))) * fSlow40));
		double fSlow385 = (2.5 * (((3.0 * fSlow382) + -1.0) * fSlow62));
		double fSlow386 = (1.125 * (((fSlow382 * ((35.0 * fSlow382) + -30.0)) + 3.0) * fSlow72));
		double fSlow387 = (fSlow336 - fSlow301);
		double fSlow388 = mydsp_faustpower2_f(fSlow387);
		double fSlow389 = (3.5 * (((5.0 * fSlow388) + -3.0) * fSlow24));
		double fSlow390 = (1.375 * (((15.0 - (70.0 * fSlow388)) + (63.0 * mydsp_faustpower4_f(fSlow387))) * fSlow40));
		double fSlow391 = (2.5 * (((3.0 * fSlow388) + -1.0) * fSlow62));
		double fSlow392 = (1.125 * (((fSlow388 * ((35.0 * fSlow388) + -30.0)) + 3.0) * fSlow72));
		double fSlow393 = (fSlow343 - fSlow301);
		double fSlow394 = mydsp_faustpower2_f(fSlow393);
		double fSlow395 = (3.5 * (((5.0 * fSlow394) + -3.0) * fSlow24));
		double fSlow396 = (1.375 * (((15.0 - (70.0 * fSlow394)) + (63.0 * mydsp_faustpower4_f(fSlow393))) * fSlow40));
		double fSlow397 = (2.5 * (((3.0 * fSlow394) + -1.0) * fSlow62));
		double fSlow398 = (1.125 * (((fSlow394 * ((35.0 * fSlow394) + -30.0)) + 3.0) * fSlow72));
		double fSlow399 = (fSlow350 - fSlow301);
		double fSlow400 = mydsp_faustpower2_f(fSlow399);
		double fSlow401 = (3.5 * (((5.0 * fSlow400) + -3.0) * fSlow24));
		double fSlow402 = (1.375 * (((15.0 - (70.0 * fSlow400)) + (63.0 * mydsp_faustpower4_f(fSlow399))) * fSlow40));
		double fSlow403 = (2.5 * (((3.0 * fSlow400) + -1.0) * fSlow62));
		double fSlow404 = (1.125 * (((fSlow400 * ((35.0 * fSlow400) + -30.0)) + 3.0) * fSlow72));
		double fSlow405 = (fSlow271 - fSlow272);
		double fSlow406 = mydsp_faustpower2_f(fSlow405);
		double fSlow407 = (3.5 * (((5.0 * fSlow406) + -3.0) * fSlow24));
		double fSlow408 = (1.375 * (((15.0 - (70.0 * fSlow406)) + (63.0 * mydsp_faustpower4_f(fSlow405))) * fSlow40));
		double fSlow409 = (2.5 * (((3.0 * fSlow406) + -1.0) * fSlow62));
		double fSlow410 = (1.125 * (((fSlow406 * ((35.0 * fSlow406) + -30.0)) + 3.0) * fSlow72));
		double fSlow411 = (fSlow279 - fSlow272);
		double fSlow412 = mydsp_faustpower2_f(fSlow411);
		double fSlow413 = (3.5 * (((5.0 * fSlow412) + -3.0) * fSlow24));
		double fSlow414 = (1.375 * (((15.0 - (70.0 * fSlow412)) + (63.0 * mydsp_faustpower4_f(fSlow411))) * fSlow40));
		double fSlow415 = (2.5 * (((3.0 * fSlow412) + -1.0) * fSlow62));
		double fSlow416 = (1.125 * (((fSlow412 * ((35.0 * fSlow412) + -30.0)) + 3.0) * fSlow72));
		double fSlow417 = (fSlow286 - fSlow272);
		double fSlow418 = mydsp_faustpower2_f(fSlow417);
		double fSlow419 = (3.5 * (((5.0 * fSlow418) + -3.0) * fSlow24));
		double fSlow420 = (1.375 * (((15.0 - (70.0 * fSlow418)) + (63.0 * mydsp_faustpower4_f(fSlow417))) * fSlow40));
		double fSlow421 = (2.5 * (((3.0 * fSlow418) + -1.0) * fSlow62));
		double fSlow422 = (1.125 * (((fSlow418 * ((35.0 * fSlow418) + -30.0)) + 3.0) * fSlow72));
		double fSlow423 = (fSlow293 - fSlow272);
		double fSlow424 = mydsp_faustpower2_f(fSlow423);
		double fSlow425 = (3.5 * (((5.0 * fSlow424) + -3.0) * fSlow24));
		double fSlow426 = (1.375 * (((15.0 - (70.0 * fSlow424)) + (63.0 * mydsp_faustpower4_f(fSlow423))) * fSlow40));
		double fSlow427 = (2.5 * (((3.0 * fSlow424) + -1.0) * fSlow62));
		double fSlow428 = (1.125 * (((fSlow424 * ((35.0 * fSlow424) + -30.0)) + 3.0) * fSlow72));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec3[0] = (fSlow19 + (0.999 * fRec3[1]));
			double fTemp0 = (fRec3[0] * double(input0[i]));
			fRec4[0] = (fRec4[1] + fRec2[1]);
			fRec2[0] = (fSlow16 * ((fSlow18 * fTemp0) - (fSlow20 * fRec4[0])));
			fRec1[0] = (fRec2[0] + fRec1[1]);
			double fTemp1 = (fSlow22 * fTemp0);
			fRec6[0] = (fRec6[1] + fRec5[1]);
			fRec5[0] = (fSlow16 * (fTemp1 - (fSlow20 * fRec6[0])));
			double fTemp2 = ((fSlow13 * (fRec1[0] - fRec2[0])) + (fRec2[0] + fRec5[0]));
			double fTemp3 = (fSlow9 * fTemp2);
			fRec10[0] = (fRec10[1] + fRec9[1]);
			fRec11[0] = (fRec11[1] + fRec10[1]);
			fRec9[0] = (fSlow30 * ((fSlow33 * fTemp0) - ((fSlow35 * fRec10[0]) + (fSlow34 * fRec11[0]))));
			fRec12[0] = (fRec9[0] + fRec12[1]);
			double fTemp4 = (fRec12[0] - fRec9[0]);
			fVec0[0] = fTemp4;
			fRec13[0] = (fRec13[1] + fVec0[1]);
			fRec14[0] = (fRec14[1] + fRec8[1]);
			fRec8[0] = (fSlow28 * ((fSlow26 * (fRec9[0] + (fSlow36 * ((fSlow38 * fTemp4) + (fSlow37 * fRec13[0]))))) - (fSlow39 * fRec14[0])));
			fRec7[0] = (fRec8[0] + fRec7[1]);
			fRec17[0] = (fRec17[1] + fRec16[1]);
			fRec18[0] = (fRec18[1] + fRec17[1]);
			fRec16[0] = (fSlow30 * (fTemp1 - ((fSlow35 * fRec17[0]) + (fSlow34 * fRec18[0]))));
			fRec19[0] = (fRec19[1] + fRec15[1]);
			fRec15[0] = (fSlow28 * (fRec16[0] - (fSlow39 * fRec19[0])));
			double fTemp5 = ((fSlow27 * (fRec7[0] - fRec8[0])) + (fRec8[0] + fRec15[0]));
			fRec24[0] = (fRec24[1] + fRec23[1]);
			fRec25[0] = (fRec25[1] + fRec24[1]);
			fRec23[0] = (fSlow47 * ((fSlow49 * fTemp0) - ((fSlow51 * fRec24[0]) + (fSlow50 * fRec25[0]))));
			fRec26[0] = (fRec23[0] + fRec26[1]);
			double fTemp6 = (fRec26[0] - fRec23[0]);
			fVec1[0] = fTemp6;
			fRec27[0] = (fRec27[1] + fVec1[1]);
			fRec28[0] = (fRec28[1] + fRec22[1]);
			fRec29[0] = (fRec29[1] + fRec28[1]);
			fRec22[0] = (fSlow45 * ((fSlow46 * (fRec23[0] + (fSlow52 * ((fSlow54 * fTemp6) + (fSlow53 * fRec27[0]))))) - ((fSlow56 * fRec28[0]) + (fSlow55 * fRec29[0]))));
			fRec30[0] = (fRec22[0] + fRec30[1]);
			double fTemp7 = (fRec30[0] - fRec22[0]);
			fVec2[0] = fTemp7;
			fRec31[0] = (fRec31[1] + fVec2[1]);
			fRec32[0] = (fRec32[1] + fRec21[1]);
			fRec21[0] = (fSlow44 * ((fSlow42 * (fRec22[0] + (fSlow57 * ((fSlow59 * fTemp7) + (fSlow58 * fRec31[0]))))) - (fSlow60 * fRec32[0])));
			fRec20[0] = (fRec21[0] + fRec20[1]);
			fRec36[0] = (fRec36[1] + fRec35[1]);
			fRec37[0] = (fRec37[1] + fRec36[1]);
			fRec35[0] = (fSlow47 * (fTemp1 - ((fSlow51 * fRec36[0]) + (fSlow50 * fRec37[0]))));
			fRec38[0] = (fRec38[1] + fRec34[1]);
			fRec39[0] = (fRec39[1] + fRec38[1]);
			fRec34[0] = (fSlow45 * (fRec35[0] - ((fSlow56 * fRec38[0]) + (fSlow55 * fRec39[0]))));
			fRec40[0] = (fRec40[1] + fRec33[1]);
			fRec33[0] = (fSlow44 * (fRec34[0] - (fSlow60 * fRec40[0])));
			double fTemp8 = ((fSlow43 * (fRec20[0] - fRec21[0])) + (fRec21[0] + fRec33[0]));
			double fTemp9 = (fSlow61 * fTemp0);
			fRec43[0] = (fRec43[1] + fRec42[1]);
			fRec44[0] = (fRec44[1] + fRec43[1]);
			fRec42[0] = (fSlow68 * ((fSlow69 * fTemp0) - ((fSlow71 * fRec43[0]) + (fSlow70 * fRec44[0]))));
			fRec41[0] = (fRec42[0] + fRec41[1]);
			double fTemp10 = (fRec41[0] - fRec42[0]);
			fVec3[0] = fTemp10;
			fRec45[0] = (fRec45[1] + fVec3[1]);
			fRec47[0] = (fRec47[1] + fRec46[1]);
			fRec48[0] = (fRec48[1] + fRec47[1]);
			fRec46[0] = (fSlow68 * (fTemp1 - ((fSlow71 * fRec47[0]) + (fSlow70 * fRec48[0]))));
			double fTemp11 = ((fSlow65 * ((fSlow67 * fTemp10) + (fSlow66 * fRec45[0]))) + (fRec42[0] + fRec46[0]));
			fRec52[0] = (fRec52[1] + fRec51[1]);
			fRec53[0] = (fRec53[1] + fRec52[1]);
			fRec51[0] = (fSlow79 * ((fSlow81 * fTemp0) - ((fSlow83 * fRec52[0]) + (fSlow82 * fRec53[0]))));
			fRec54[0] = (fRec51[0] + fRec54[1]);
			double fTemp12 = (fRec54[0] - fRec51[0]);
			fVec4[0] = fTemp12;
			fRec55[0] = (fRec55[1] + fVec4[1]);
			fRec56[0] = (fRec56[1] + fRec50[1]);
			fRec57[0] = (fRec57[1] + fRec56[1]);
			fRec50[0] = (fSlow78 * ((fSlow74 * (fRec51[0] + (fSlow84 * ((fSlow86 * fTemp12) + (fSlow85 * fRec55[0]))))) - ((fSlow88 * fRec56[0]) + (fSlow87 * fRec57[0]))));
			fRec49[0] = (fRec50[0] + fRec49[1]);
			double fTemp13 = (fRec49[0] - fRec50[0]);
			fVec5[0] = fTemp13;
			fRec58[0] = (fRec58[1] + fVec5[1]);
			fRec61[0] = (fRec61[1] + fRec60[1]);
			fRec62[0] = (fRec62[1] + fRec61[1]);
			fRec60[0] = (fSlow79 * (fTemp1 - ((fSlow83 * fRec61[0]) + (fSlow82 * fRec62[0]))));
			fRec63[0] = (fRec63[1] + fRec59[1]);
			fRec64[0] = (fRec64[1] + fRec63[1]);
			fRec59[0] = (fSlow78 * (fRec60[0] - ((fSlow88 * fRec63[0]) + (fSlow87 * fRec64[0]))));
			double fTemp14 = ((fSlow75 * ((fSlow77 * fTemp13) + (fSlow76 * fRec58[0]))) + (fRec50[0] + fRec59[0]));
			double fTemp15 = (0.012698412698412698 * ((fSlow7 * ((fTemp3 + (fSlow25 * fTemp5)) + (fSlow41 * fTemp8))) + ((fTemp9 + (fSlow63 * fTemp11)) + (fSlow73 * fTemp14))));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph0 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp15);
			double fTemp16 = (0.012698412698412698 * (((((fTemp9 + (fSlow89 * fTemp2)) + (fSlow91 * fTemp11)) + (fSlow93 * fTemp5)) + (fSlow94 * fTemp14)) + (fSlow95 * fTemp8)));
			fRec65[0] = max((fRec65[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph1 = FAUSTFLOAT(fRec65[0]);
			output1[i] = FAUSTFLOAT(fTemp16);
			double fTemp17 = (0.012698412698412698 * (((((fTemp9 + (fSlow97 * fTemp2)) + (fSlow100 * fTemp11)) + (fSlow102 * fTemp5)) + (fSlow103 * fTemp14)) + (fSlow104 * fTemp8)));
			fRec66[0] = max((fRec66[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph2 = FAUSTFLOAT(fRec66[0]);
			output2[i] = FAUSTFLOAT(fTemp17);
			double fTemp18 = (0.012698412698412698 * (((((fTemp9 + (fSlow106 * fTemp2)) + (fSlow109 * fTemp11)) + (fSlow111 * fTemp5)) + (fSlow112 * fTemp14)) + (fSlow113 * fTemp8)));
			fRec67[0] = max((fRec67[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fVbargraph3 = FAUSTFLOAT(fRec67[0]);
			output3[i] = FAUSTFLOAT(fTemp18);
			double fTemp19 = (0.012698412698412698 * (((((fTemp9 + (fSlow115 * fTemp2)) + (fSlow118 * fTemp11)) + (fSlow120 * fTemp5)) + (fSlow121 * fTemp14)) + (fSlow122 * fTemp8)));
			fRec68[0] = max((fRec68[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph4 = FAUSTFLOAT(fRec68[0]);
			output4[i] = FAUSTFLOAT(fTemp19);
			double fTemp20 = (0.012698412698412698 * ((fSlow123 * ((fTemp3 + (fSlow125 * fTemp5)) + (fSlow126 * fTemp8))) + ((fTemp9 + (fSlow127 * fTemp11)) + (fSlow128 * fTemp14))));
			fRec69[0] = max((fRec69[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph5 = FAUSTFLOAT(fRec69[0]);
			output5[i] = FAUSTFLOAT(fTemp20);
			double fTemp21 = (0.022574955908289243 * ((fSlow131 * ((fTemp3 + (fSlow133 * fTemp5)) + (fSlow134 * fTemp8))) + ((fTemp9 + (fSlow135 * fTemp11)) + (fSlow136 * fTemp14))));
			fRec70[0] = max((fRec70[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph6 = FAUSTFLOAT(fRec70[0]);
			output6[i] = FAUSTFLOAT(fTemp21);
			double fTemp22 = (0.022574955908289243 * ((fSlow138 * ((fTemp3 + (fSlow140 * fTemp5)) + (fSlow141 * fTemp8))) + ((fTemp9 + (fSlow142 * fTemp11)) + (fSlow143 * fTemp14))));
			fRec71[0] = max((fRec71[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph7 = FAUSTFLOAT(fRec71[0]);
			output7[i] = FAUSTFLOAT(fTemp22);
			double fTemp23 = (0.022574955908289243 * ((fSlow145 * ((fTemp3 + (fSlow147 * fTemp5)) + (fSlow148 * fTemp8))) + ((fTemp9 + (fSlow149 * fTemp11)) + (fSlow150 * fTemp14))));
			fRec72[0] = max((fRec72[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fVbargraph8 = FAUSTFLOAT(fRec72[0]);
			output8[i] = FAUSTFLOAT(fTemp23);
			double fTemp24 = (0.022574955908289243 * ((fSlow152 * ((fTemp3 + (fSlow154 * fTemp5)) + (fSlow155 * fTemp8))) + ((fTemp9 + (fSlow156 * fTemp11)) + (fSlow157 * fTemp14))));
			fRec73[0] = max((fRec73[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph9 = FAUSTFLOAT(fRec73[0]);
			output9[i] = FAUSTFLOAT(fTemp24);
			double fTemp25 = (0.022574955908289243 * (((((fTemp9 + (fSlow159 * fTemp2)) + (fSlow162 * fTemp11)) + (fSlow164 * fTemp5)) + (fSlow165 * fTemp14)) + (fSlow166 * fTemp8)));
			fRec74[0] = max((fRec74[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph10 = FAUSTFLOAT(fRec74[0]);
			output10[i] = FAUSTFLOAT(fTemp25);
			double fTemp26 = (0.022574955908289243 * (((((fTemp9 + (fSlow168 * fTemp2)) + (fSlow171 * fTemp11)) + (fSlow173 * fTemp5)) + (fSlow174 * fTemp14)) + (fSlow175 * fTemp8)));
			fRec75[0] = max((fRec75[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fVbargraph11 = FAUSTFLOAT(fRec75[0]);
			output11[i] = FAUSTFLOAT(fTemp26);
			double fTemp27 = (0.022574955908289243 * (((((fTemp9 + (fSlow177 * fTemp2)) + (fSlow180 * fTemp11)) + (fSlow182 * fTemp5)) + (fSlow183 * fTemp14)) + (fSlow184 * fTemp8)));
			fRec76[0] = max((fRec76[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph12 = FAUSTFLOAT(fRec76[0]);
			output12[i] = FAUSTFLOAT(fTemp27);
			double fTemp28 = (0.022574955908289243 * (((((fTemp9 + (fSlow186 * fTemp2)) + (fSlow189 * fTemp11)) + (fSlow191 * fTemp5)) + (fSlow192 * fTemp14)) + (fSlow193 * fTemp8)));
			fRec77[0] = max((fRec77[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fVbargraph13 = FAUSTFLOAT(fRec77[0]);
			output13[i] = FAUSTFLOAT(fTemp28);
			double fTemp29 = (0.022574955908289243 * ((fSlow194 * ((fTemp3 + (fSlow196 * fTemp5)) + (fSlow197 * fTemp8))) + ((fTemp9 + (fSlow198 * fTemp11)) + (fSlow199 * fTemp14))));
			fRec78[0] = max((fRec78[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph14 = FAUSTFLOAT(fRec78[0]);
			output14[i] = FAUSTFLOAT(fTemp29);
			double fTemp30 = (0.022574955908289243 * ((fSlow200 * ((fTemp3 + (fSlow202 * fTemp5)) + (fSlow203 * fTemp8))) + ((fTemp9 + (fSlow204 * fTemp11)) + (fSlow205 * fTemp14))));
			fRec79[0] = max((fRec79[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph15 = FAUSTFLOAT(fRec79[0]);
			output15[i] = FAUSTFLOAT(fTemp30);
			double fTemp31 = (0.022574955908289243 * ((fSlow206 * ((fTemp3 + (fSlow208 * fTemp5)) + (fSlow209 * fTemp8))) + ((fSlow210 * fTemp14) + (fTemp9 + (fSlow211 * fTemp11)))));
			fRec80[0] = max((fRec80[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph16 = FAUSTFLOAT(fRec80[0]);
			output16[i] = FAUSTFLOAT(fTemp31);
			double fTemp32 = (0.022574955908289243 * ((fSlow212 * ((fTemp3 + (fSlow214 * fTemp5)) + (fSlow215 * fTemp8))) + ((fTemp9 + (fSlow216 * fTemp11)) + (fSlow217 * fTemp14))));
			fRec81[0] = max((fRec81[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph17 = FAUSTFLOAT(fRec81[0]);
			output17[i] = FAUSTFLOAT(fTemp32);
			double fTemp33 = (0.021093750000000001 * ((fSlow220 * ((fTemp3 + (fSlow222 * fTemp5)) + (fSlow223 * fTemp8))) + ((fTemp9 + (fSlow224 * fTemp11)) + (fSlow225 * fTemp14))));
			fRec82[0] = max((fRec82[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph18 = FAUSTFLOAT(fRec82[0]);
			output18[i] = FAUSTFLOAT(fTemp33);
			double fTemp34 = (0.021093750000000001 * ((fSlow227 * ((fTemp3 + (fSlow229 * fTemp5)) + (fSlow230 * fTemp8))) + ((fTemp9 + (fSlow231 * fTemp11)) + (fSlow232 * fTemp14))));
			fRec83[0] = max((fRec83[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fVbargraph19 = FAUSTFLOAT(fRec83[0]);
			output19[i] = FAUSTFLOAT(fTemp34);
			double fTemp35 = (0.021093750000000001 * ((fSlow234 * ((fTemp3 + (fSlow236 * fTemp5)) + (fSlow237 * fTemp8))) + ((fTemp9 + (fSlow238 * fTemp11)) + (fSlow239 * fTemp14))));
			fRec84[0] = max((fRec84[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph20 = FAUSTFLOAT(fRec84[0]);
			output20[i] = FAUSTFLOAT(fTemp35);
			double fTemp36 = (0.021093750000000001 * ((fSlow241 * ((fTemp3 + (fSlow243 * fTemp5)) + (fSlow244 * fTemp8))) + ((fTemp9 + (fSlow245 * fTemp11)) + (fSlow246 * fTemp14))));
			fRec85[0] = max((fRec85[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fVbargraph21 = FAUSTFLOAT(fRec85[0]);
			output21[i] = FAUSTFLOAT(fTemp36);
			double fTemp37 = (0.021093750000000001 * ((fSlow247 * ((fTemp3 + (fSlow249 * fTemp5)) + (fSlow250 * fTemp8))) + ((fTemp9 + (fSlow251 * fTemp11)) + (fSlow252 * fTemp14))));
			fRec86[0] = max((fRec86[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fVbargraph22 = FAUSTFLOAT(fRec86[0]);
			output22[i] = FAUSTFLOAT(fTemp37);
			double fTemp38 = (0.021093750000000001 * ((fSlow253 * ((fTemp3 + (fSlow255 * fTemp5)) + (fSlow256 * fTemp8))) + ((fTemp9 + (fSlow257 * fTemp11)) + (fSlow258 * fTemp14))));
			fRec87[0] = max((fRec87[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fVbargraph23 = FAUSTFLOAT(fRec87[0]);
			output23[i] = FAUSTFLOAT(fTemp38);
			double fTemp39 = (0.021093750000000001 * ((fSlow259 * ((fTemp3 + (fSlow261 * fTemp5)) + (fSlow262 * fTemp8))) + ((fTemp9 + (fSlow263 * fTemp11)) + (fSlow264 * fTemp14))));
			fRec88[0] = max((fRec88[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph24 = FAUSTFLOAT(fRec88[0]);
			output24[i] = FAUSTFLOAT(fTemp39);
			double fTemp40 = (0.021093750000000001 * ((fSlow265 * ((fTemp3 + (fSlow267 * fTemp5)) + (fSlow268 * fTemp8))) + ((fTemp9 + (fSlow269 * fTemp11)) + (fSlow270 * fTemp14))));
			fRec89[0] = max((fRec89[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fVbargraph25 = FAUSTFLOAT(fRec89[0]);
			output25[i] = FAUSTFLOAT(fTemp40);
			double fTemp41 = (0.02017333553791887 * ((fSlow273 * ((fTemp3 + (fSlow275 * fTemp5)) + (fSlow276 * fTemp8))) + ((fTemp9 + (fSlow277 * fTemp11)) + (fSlow278 * fTemp14))));
			fRec90[0] = max((fRec90[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph26 = FAUSTFLOAT(fRec90[0]);
			output26[i] = FAUSTFLOAT(fTemp41);
			double fTemp42 = (0.02017333553791887 * ((fSlow280 * ((fTemp3 + (fSlow282 * fTemp5)) + (fSlow283 * fTemp8))) + ((fTemp9 + (fSlow284 * fTemp11)) + (fSlow285 * fTemp14))));
			fRec91[0] = max((fRec91[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fVbargraph27 = FAUSTFLOAT(fRec91[0]);
			output27[i] = FAUSTFLOAT(fTemp42);
			double fTemp43 = (0.02017333553791887 * ((fSlow287 * ((fTemp3 + (fSlow289 * fTemp5)) + (fSlow290 * fTemp8))) + ((fTemp9 + (fSlow291 * fTemp11)) + (fSlow292 * fTemp14))));
			fRec92[0] = max((fRec92[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph28 = FAUSTFLOAT(fRec92[0]);
			output28[i] = FAUSTFLOAT(fTemp43);
			double fTemp44 = (0.02017333553791887 * ((fSlow294 * ((fTemp3 + (fSlow296 * fTemp5)) + (fSlow297 * fTemp8))) + ((fTemp9 + (fSlow298 * fTemp11)) + (fSlow299 * fTemp14))));
			fRec93[0] = max((fRec93[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fVbargraph29 = FAUSTFLOAT(fRec93[0]);
			output29[i] = FAUSTFLOAT(fTemp44);
			double fTemp45 = (0.02017333553791887 * ((fSlow302 * ((fTemp3 + (fSlow304 * fTemp5)) + (fSlow305 * fTemp8))) + ((fTemp9 + (fSlow306 * fTemp11)) + (fSlow307 * fTemp14))));
			fRec94[0] = max((fRec94[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fVbargraph30 = FAUSTFLOAT(fRec94[0]);
			output30[i] = FAUSTFLOAT(fTemp45);
			double fTemp46 = (0.02017333553791887 * ((fSlow309 * ((fTemp3 + (fSlow311 * fTemp5)) + (fSlow312 * fTemp8))) + ((fTemp9 + (fSlow313 * fTemp11)) + (fSlow314 * fTemp14))));
			fRec95[0] = max((fRec95[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph31 = FAUSTFLOAT(fRec95[0]);
			output31[i] = FAUSTFLOAT(fTemp46);
			double fTemp47 = (0.02017333553791887 * ((fSlow316 * ((fTemp3 + (fSlow318 * fTemp5)) + (fSlow319 * fTemp8))) + ((fTemp9 + (fSlow320 * fTemp11)) + (fSlow321 * fTemp14))));
			fRec96[0] = max((fRec96[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fVbargraph32 = FAUSTFLOAT(fRec96[0]);
			output32[i] = FAUSTFLOAT(fTemp47);
			double fTemp48 = (0.02017333553791887 * ((fSlow323 * ((fTemp3 + (fSlow325 * fTemp5)) + (fSlow326 * fTemp8))) + ((fTemp9 + (fSlow327 * fTemp11)) + (fSlow328 * fTemp14))));
			fRec97[0] = max((fRec97[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph33 = FAUSTFLOAT(fRec97[0]);
			output33[i] = FAUSTFLOAT(fTemp48);
			double fTemp49 = (0.02017333553791887 * ((fSlow330 * ((fTemp3 + (fSlow332 * fTemp5)) + (fSlow333 * fTemp8))) + ((fTemp9 + (fSlow334 * fTemp11)) + (fSlow335 * fTemp14))));
			fRec98[0] = max((fRec98[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fVbargraph34 = FAUSTFLOAT(fRec98[0]);
			output34[i] = FAUSTFLOAT(fTemp49);
			double fTemp50 = (0.02017333553791887 * ((fSlow337 * ((fTemp3 + (fSlow339 * fTemp5)) + (fSlow340 * fTemp8))) + ((fTemp9 + (fSlow341 * fTemp11)) + (fSlow342 * fTemp14))));
			fRec99[0] = max((fRec99[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph35 = FAUSTFLOAT(fRec99[0]);
			output35[i] = FAUSTFLOAT(fTemp50);
			double fTemp51 = (0.02017333553791887 * ((fSlow344 * ((fTemp3 + (fSlow346 * fTemp5)) + (fSlow347 * fTemp8))) + ((fTemp9 + (fSlow348 * fTemp11)) + (fSlow349 * fTemp14))));
			fRec100[0] = max((fRec100[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph36 = FAUSTFLOAT(fRec100[0]);
			output36[i] = FAUSTFLOAT(fTemp51);
			double fTemp52 = (0.02017333553791887 * ((fSlow351 * ((fTemp3 + (fSlow353 * fTemp5)) + (fSlow354 * fTemp8))) + ((fTemp9 + (fSlow355 * fTemp11)) + (fSlow356 * fTemp14))));
			fRec101[0] = max((fRec101[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fVbargraph37 = FAUSTFLOAT(fRec101[0]);
			output37[i] = FAUSTFLOAT(fTemp52);
			double fTemp53 = (0.02017333553791887 * ((fSlow357 * ((fTemp3 + (fSlow359 * fTemp5)) + (fSlow360 * fTemp8))) + ((fTemp9 + (fSlow361 * fTemp11)) + (fSlow362 * fTemp14))));
			fRec102[0] = max((fRec102[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fVbargraph38 = FAUSTFLOAT(fRec102[0]);
			output38[i] = FAUSTFLOAT(fTemp53);
			double fTemp54 = (0.02017333553791887 * ((fSlow363 * ((fTemp3 + (fSlow365 * fTemp5)) + (fSlow366 * fTemp8))) + ((fTemp9 + (fSlow367 * fTemp11)) + (fSlow368 * fTemp14))));
			fRec103[0] = max((fRec103[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fVbargraph39 = FAUSTFLOAT(fRec103[0]);
			output39[i] = FAUSTFLOAT(fTemp54);
			double fTemp55 = (0.02017333553791887 * ((fSlow369 * ((fTemp3 + (fSlow371 * fTemp5)) + (fSlow372 * fTemp8))) + ((fTemp9 + (fSlow373 * fTemp11)) + (fSlow374 * fTemp14))));
			fRec104[0] = max((fRec104[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph40 = FAUSTFLOAT(fRec104[0]);
			output40[i] = FAUSTFLOAT(fTemp55);
			double fTemp56 = (0.02017333553791887 * ((fSlow375 * ((fTemp3 + (fSlow377 * fTemp5)) + (fSlow378 * fTemp8))) + ((fTemp9 + (fSlow379 * fTemp11)) + (fSlow380 * fTemp14))));
			fRec105[0] = max((fRec105[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fVbargraph41 = FAUSTFLOAT(fRec105[0]);
			output41[i] = FAUSTFLOAT(fTemp56);
			double fTemp57 = (0.02017333553791887 * ((fSlow381 * ((fTemp3 + (fSlow383 * fTemp5)) + (fSlow384 * fTemp8))) + ((fTemp9 + (fSlow385 * fTemp11)) + (fSlow386 * fTemp14))));
			fRec106[0] = max((fRec106[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fVbargraph42 = FAUSTFLOAT(fRec106[0]);
			output42[i] = FAUSTFLOAT(fTemp57);
			double fTemp58 = (0.02017333553791887 * ((fSlow387 * ((fTemp3 + (fSlow389 * fTemp5)) + (fSlow390 * fTemp8))) + ((fTemp9 + (fSlow391 * fTemp11)) + (fSlow392 * fTemp14))));
			fRec107[0] = max((fRec107[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fVbargraph43 = FAUSTFLOAT(fRec107[0]);
			output43[i] = FAUSTFLOAT(fTemp58);
			double fTemp59 = (0.02017333553791887 * ((fSlow393 * ((fTemp3 + (fSlow395 * fTemp5)) + (fSlow396 * fTemp8))) + ((fTemp9 + (fSlow397 * fTemp11)) + (fSlow398 * fTemp14))));
			fRec108[0] = max((fRec108[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fVbargraph44 = FAUSTFLOAT(fRec108[0]);
			output44[i] = FAUSTFLOAT(fTemp59);
			double fTemp60 = (0.02017333553791887 * ((fSlow399 * ((fTemp3 + (fSlow401 * fTemp5)) + (fSlow402 * fTemp8))) + ((fTemp9 + (fSlow403 * fTemp11)) + (fSlow404 * fTemp14))));
			fRec109[0] = max((fRec109[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fVbargraph45 = FAUSTFLOAT(fRec109[0]);
			output45[i] = FAUSTFLOAT(fTemp60);
			double fTemp61 = (0.02017333553791887 * ((fSlow405 * ((fTemp3 + (fSlow407 * fTemp5)) + (fSlow408 * fTemp8))) + ((fTemp9 + (fSlow409 * fTemp11)) + (fSlow410 * fTemp14))));
			fRec110[0] = max((fRec110[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fVbargraph46 = FAUSTFLOAT(fRec110[0]);
			output46[i] = FAUSTFLOAT(fTemp61);
			double fTemp62 = (0.02017333553791887 * ((fSlow411 * ((fTemp3 + (fSlow413 * fTemp5)) + (fSlow414 * fTemp8))) + ((fTemp9 + (fSlow415 * fTemp11)) + (fSlow416 * fTemp14))));
			fRec111[0] = max((fRec111[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fVbargraph47 = FAUSTFLOAT(fRec111[0]);
			output47[i] = FAUSTFLOAT(fTemp62);
			double fTemp63 = (0.02017333553791887 * ((fSlow417 * ((fTemp3 + (fSlow419 * fTemp5)) + (fSlow420 * fTemp8))) + ((fTemp9 + (fSlow421 * fTemp11)) + (fSlow422 * fTemp14))));
			fRec112[0] = max((fRec112[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fVbargraph48 = FAUSTFLOAT(fRec112[0]);
			output48[i] = FAUSTFLOAT(fTemp63);
			double fTemp64 = (0.02017333553791887 * ((fSlow423 * ((fTemp3 + (fSlow425 * fTemp5)) + (fSlow426 * fTemp8))) + ((fTemp9 + (fSlow427 * fTemp11)) + (fSlow428 * fTemp14))));
			fRec113[0] = max((fRec113[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fVbargraph49 = FAUSTFLOAT(fRec113[0]);
			output49[i] = FAUSTFLOAT(fTemp64);
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
