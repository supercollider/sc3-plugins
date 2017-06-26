/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOADecLebedev505"
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
	double fConst10;
	double fConst11;
	double fConst12;
	double fRec49[2];
	double fRec50[2];
	double fRec48[2];
	double fConst13;
	double fRec51[2];
	double fRec52[2];
	double fRec47[2];
	double fRec46[2];
	FAUSTFLOAT fVbargraph10;
	double fRec56[2];
	double fRec57[2];
	double fRec55[2];
	double fRec58[2];
	double fRec59[2];
	double fRec54[2];
	double fRec53[2];
	FAUSTFLOAT fVbargraph11;
	double fRec63[2];
	double fRec64[2];
	double fRec62[2];
	double fRec65[2];
	double fRec66[2];
	double fRec61[2];
	double fRec60[2];
	FAUSTFLOAT fVbargraph12;
	double fRec70[2];
	double fRec71[2];
	double fRec69[2];
	double fRec72[2];
	double fRec73[2];
	double fRec68[2];
	double fRec67[2];
	FAUSTFLOAT fVbargraph13;
	double fRec77[2];
	double fRec78[2];
	double fRec76[2];
	double fRec79[2];
	double fRec80[2];
	double fRec75[2];
	double fRec74[2];
	FAUSTFLOAT fVbargraph14;
	double fConst14;
	double fConst15;
	double fConst16;
	double fConst17;
	double fRec85[2];
	double fRec86[2];
	double fRec84[2];
	double fConst18;
	double fRec87[2];
	double fRec88[2];
	double fRec83[2];
	double fConst19;
	double fRec89[2];
	double fRec82[2];
	double fRec81[2];
	FAUSTFLOAT fVbargraph15;
	double fRec94[2];
	double fRec95[2];
	double fRec93[2];
	double fRec96[2];
	double fRec97[2];
	double fRec92[2];
	double fRec98[2];
	double fRec91[2];
	double fRec90[2];
	FAUSTFLOAT fVbargraph16;
	double fRec103[2];
	double fRec104[2];
	double fRec102[2];
	double fRec105[2];
	double fRec106[2];
	double fRec101[2];
	double fRec107[2];
	double fRec100[2];
	double fRec99[2];
	FAUSTFLOAT fVbargraph17;
	double fRec112[2];
	double fRec113[2];
	double fRec111[2];
	double fRec114[2];
	double fRec115[2];
	double fRec110[2];
	double fRec116[2];
	double fRec109[2];
	double fRec108[2];
	FAUSTFLOAT fVbargraph18;
	double fRec121[2];
	double fRec122[2];
	double fRec120[2];
	double fRec123[2];
	double fRec124[2];
	double fRec119[2];
	double fRec125[2];
	double fRec118[2];
	double fRec117[2];
	FAUSTFLOAT fVbargraph19;
	double fRec130[2];
	double fRec131[2];
	double fRec129[2];
	double fRec132[2];
	double fRec133[2];
	double fRec128[2];
	double fRec134[2];
	double fRec127[2];
	double fRec126[2];
	FAUSTFLOAT fVbargraph20;
	double fRec0[2];
	FAUSTFLOAT fVbargraph21;
	double fRec135[2];
	FAUSTFLOAT fVbargraph22;
	double fRec141[2];
	double fRec142[2];
	double fRec140[2];
	double fRec143[2];
	double fRec144[2];
	double fRec139[2];
	double fRec145[2];
	double fRec138[2];
	double fRec137[2];
	FAUSTFLOAT fVbargraph23;
	double fRec150[2];
	double fRec151[2];
	double fRec149[2];
	double fRec152[2];
	double fRec153[2];
	double fRec148[2];
	double fRec154[2];
	double fRec147[2];
	double fRec146[2];
	FAUSTFLOAT fVbargraph24;
	double fRec157[2];
	double fRec158[2];
	double fRec156[2];
	double fRec155[2];
	FAUSTFLOAT fVbargraph25;
	double fRec161[2];
	double fRec160[2];
	double fRec159[2];
	FAUSTFLOAT fVbargraph26;
	double fRec166[2];
	double fRec167[2];
	double fRec165[2];
	double fRec168[2];
	double fRec169[2];
	double fRec164[2];
	double fRec170[2];
	double fRec163[2];
	double fRec162[2];
	FAUSTFLOAT fVbargraph27;
	double fRec174[2];
	double fRec175[2];
	double fRec173[2];
	double fRec176[2];
	double fRec177[2];
	double fRec172[2];
	double fRec171[2];
	FAUSTFLOAT fVbargraph28;
	double fRec181[2];
	double fRec182[2];
	double fRec180[2];
	double fRec183[2];
	double fRec179[2];
	double fRec178[2];
	FAUSTFLOAT fVbargraph29;
	double fRec187[2];
	double fRec188[2];
	double fRec186[2];
	double fRec189[2];
	double fRec185[2];
	double fRec184[2];
	FAUSTFLOAT fVbargraph30;
	double fRec193[2];
	double fRec194[2];
	double fRec192[2];
	double fRec195[2];
	double fRec196[2];
	double fRec191[2];
	double fRec190[2];
	FAUSTFLOAT fVbargraph31;
	double fRec136[2];
	FAUSTFLOAT fVbargraph32;
	double fRec197[2];
	FAUSTFLOAT fVbargraph33;
	double fRec198[2];
	FAUSTFLOAT fVbargraph34;
	double fRec199[2];
	FAUSTFLOAT fVbargraph35;
	double fRec200[2];
	FAUSTFLOAT fVbargraph36;
	double fRec204[2];
	double fRec205[2];
	double fRec203[2];
	double fRec202[2];
	FAUSTFLOAT fVbargraph37;
	double fRec209[2];
	double fRec210[2];
	double fRec208[2];
	double fRec211[2];
	double fRec207[2];
	double fRec206[2];
	FAUSTFLOAT fVbargraph38;
	double fRec216[2];
	double fRec217[2];
	double fRec215[2];
	double fRec218[2];
	double fRec219[2];
	double fRec214[2];
	double fRec220[2];
	double fRec213[2];
	double fRec212[2];
	FAUSTFLOAT fVbargraph39;
	double fRec224[2];
	double fRec225[2];
	double fRec223[2];
	double fRec226[2];
	double fRec227[2];
	double fRec222[2];
	double fRec221[2];
	FAUSTFLOAT fVbargraph40;
	double fRec232[2];
	double fRec233[2];
	double fRec231[2];
	double fRec234[2];
	double fRec235[2];
	double fRec230[2];
	double fRec236[2];
	double fRec229[2];
	double fRec228[2];
	FAUSTFLOAT fVbargraph41;
	double fRec240[2];
	double fRec241[2];
	double fRec239[2];
	double fRec242[2];
	double fRec243[2];
	double fRec238[2];
	double fRec237[2];
	FAUSTFLOAT fVbargraph42;
	double fRec201[2];
	FAUSTFLOAT fVbargraph43;
	double fRec244[2];
	FAUSTFLOAT fVbargraph44;
	double fRec245[2];
	FAUSTFLOAT fVbargraph45;
	double fRec246[2];
	FAUSTFLOAT fVbargraph46;
	double fRec247[2];
	FAUSTFLOAT fVbargraph47;
	double fRec248[2];
	FAUSTFLOAT fVbargraph48;
	double fRec249[2];
	FAUSTFLOAT fVbargraph49;
	double fRec250[2];
	FAUSTFLOAT fVbargraph50;
	double fRec251[2];
	FAUSTFLOAT fVbargraph51;
	double fRec252[2];
	FAUSTFLOAT fVbargraph52;
	double fRec253[2];
	FAUSTFLOAT fVbargraph53;
	double fRec254[2];
	FAUSTFLOAT fVbargraph54;
	double fRec255[2];
	FAUSTFLOAT fVbargraph55;
	double fRec256[2];
	FAUSTFLOAT fVbargraph56;
	double fRec257[2];
	FAUSTFLOAT fVbargraph57;
	double fRec258[2];
	FAUSTFLOAT fVbargraph58;
	double fRec259[2];
	FAUSTFLOAT fVbargraph59;
	double fRec260[2];
	FAUSTFLOAT fVbargraph60;
	double fRec261[2];
	FAUSTFLOAT fVbargraph61;
	double fRec262[2];
	FAUSTFLOAT fVbargraph62;
	double fRec263[2];
	FAUSTFLOAT fVbargraph63;
	double fRec264[2];
	FAUSTFLOAT fVbargraph64;
	double fRec265[2];
	FAUSTFLOAT fVbargraph65;
	double fRec266[2];
	FAUSTFLOAT fVbargraph66;
	double fRec267[2];
	FAUSTFLOAT fVbargraph67;
	double fRec268[2];
	FAUSTFLOAT fVbargraph68;
	double fRec269[2];
	FAUSTFLOAT fVbargraph69;
	double fRec270[2];
	FAUSTFLOAT fVbargraph70;
	double fRec271[2];
	FAUSTFLOAT fVbargraph71;
	double fRec272[2];
	FAUSTFLOAT fVbargraph72;
	double fRec273[2];
	FAUSTFLOAT fVbargraph73;
	double fRec274[2];
	FAUSTFLOAT fVbargraph74;
	double fRec275[2];
	FAUSTFLOAT fVbargraph75;
	double fRec276[2];
	FAUSTFLOAT fVbargraph76;
	double fRec277[2];
	FAUSTFLOAT fVbargraph77;
	double fRec278[2];
	FAUSTFLOAT fVbargraph78;
	double fRec279[2];
	FAUSTFLOAT fVbargraph79;
	double fRec280[2];
	FAUSTFLOAT fVbargraph80;
	double fRec281[2];
	FAUSTFLOAT fVbargraph81;
	double fRec282[2];
	FAUSTFLOAT fVbargraph82;
	double fRec283[2];
	FAUSTFLOAT fVbargraph83;
	double fRec284[2];
	FAUSTFLOAT fVbargraph84;
	double fRec285[2];
	FAUSTFLOAT fVbargraph85;
	
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
		m->declare("name", "HOADecLebedev505");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
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
			fRec49[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec50[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec48[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec51[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec52[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec47[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec46[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec56[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec57[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec55[l54] = 0.0;
			
		}
		for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
			fRec58[l55] = 0.0;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fRec59[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec54[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec53[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec63[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec64[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec62[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fRec65[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec66[l63] = 0.0;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec61[l64] = 0.0;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec60[l65] = 0.0;
			
		}
		for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
			fRec70[l66] = 0.0;
			
		}
		for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
			fRec71[l67] = 0.0;
			
		}
		for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
			fRec69[l68] = 0.0;
			
		}
		for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
			fRec72[l69] = 0.0;
			
		}
		for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
			fRec73[l70] = 0.0;
			
		}
		for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
			fRec68[l71] = 0.0;
			
		}
		for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
			fRec67[l72] = 0.0;
			
		}
		for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
			fRec77[l73] = 0.0;
			
		}
		for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
			fRec78[l74] = 0.0;
			
		}
		for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
			fRec76[l75] = 0.0;
			
		}
		for (int l76 = 0; (l76 < 2); l76 = (l76 + 1)) {
			fRec79[l76] = 0.0;
			
		}
		for (int l77 = 0; (l77 < 2); l77 = (l77 + 1)) {
			fRec80[l77] = 0.0;
			
		}
		for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
			fRec75[l78] = 0.0;
			
		}
		for (int l79 = 0; (l79 < 2); l79 = (l79 + 1)) {
			fRec74[l79] = 0.0;
			
		}
		for (int l80 = 0; (l80 < 2); l80 = (l80 + 1)) {
			fRec85[l80] = 0.0;
			
		}
		for (int l81 = 0; (l81 < 2); l81 = (l81 + 1)) {
			fRec86[l81] = 0.0;
			
		}
		for (int l82 = 0; (l82 < 2); l82 = (l82 + 1)) {
			fRec84[l82] = 0.0;
			
		}
		for (int l83 = 0; (l83 < 2); l83 = (l83 + 1)) {
			fRec87[l83] = 0.0;
			
		}
		for (int l84 = 0; (l84 < 2); l84 = (l84 + 1)) {
			fRec88[l84] = 0.0;
			
		}
		for (int l85 = 0; (l85 < 2); l85 = (l85 + 1)) {
			fRec83[l85] = 0.0;
			
		}
		for (int l86 = 0; (l86 < 2); l86 = (l86 + 1)) {
			fRec89[l86] = 0.0;
			
		}
		for (int l87 = 0; (l87 < 2); l87 = (l87 + 1)) {
			fRec82[l87] = 0.0;
			
		}
		for (int l88 = 0; (l88 < 2); l88 = (l88 + 1)) {
			fRec81[l88] = 0.0;
			
		}
		for (int l89 = 0; (l89 < 2); l89 = (l89 + 1)) {
			fRec94[l89] = 0.0;
			
		}
		for (int l90 = 0; (l90 < 2); l90 = (l90 + 1)) {
			fRec95[l90] = 0.0;
			
		}
		for (int l91 = 0; (l91 < 2); l91 = (l91 + 1)) {
			fRec93[l91] = 0.0;
			
		}
		for (int l92 = 0; (l92 < 2); l92 = (l92 + 1)) {
			fRec96[l92] = 0.0;
			
		}
		for (int l93 = 0; (l93 < 2); l93 = (l93 + 1)) {
			fRec97[l93] = 0.0;
			
		}
		for (int l94 = 0; (l94 < 2); l94 = (l94 + 1)) {
			fRec92[l94] = 0.0;
			
		}
		for (int l95 = 0; (l95 < 2); l95 = (l95 + 1)) {
			fRec98[l95] = 0.0;
			
		}
		for (int l96 = 0; (l96 < 2); l96 = (l96 + 1)) {
			fRec91[l96] = 0.0;
			
		}
		for (int l97 = 0; (l97 < 2); l97 = (l97 + 1)) {
			fRec90[l97] = 0.0;
			
		}
		for (int l98 = 0; (l98 < 2); l98 = (l98 + 1)) {
			fRec103[l98] = 0.0;
			
		}
		for (int l99 = 0; (l99 < 2); l99 = (l99 + 1)) {
			fRec104[l99] = 0.0;
			
		}
		for (int l100 = 0; (l100 < 2); l100 = (l100 + 1)) {
			fRec102[l100] = 0.0;
			
		}
		for (int l101 = 0; (l101 < 2); l101 = (l101 + 1)) {
			fRec105[l101] = 0.0;
			
		}
		for (int l102 = 0; (l102 < 2); l102 = (l102 + 1)) {
			fRec106[l102] = 0.0;
			
		}
		for (int l103 = 0; (l103 < 2); l103 = (l103 + 1)) {
			fRec101[l103] = 0.0;
			
		}
		for (int l104 = 0; (l104 < 2); l104 = (l104 + 1)) {
			fRec107[l104] = 0.0;
			
		}
		for (int l105 = 0; (l105 < 2); l105 = (l105 + 1)) {
			fRec100[l105] = 0.0;
			
		}
		for (int l106 = 0; (l106 < 2); l106 = (l106 + 1)) {
			fRec99[l106] = 0.0;
			
		}
		for (int l107 = 0; (l107 < 2); l107 = (l107 + 1)) {
			fRec112[l107] = 0.0;
			
		}
		for (int l108 = 0; (l108 < 2); l108 = (l108 + 1)) {
			fRec113[l108] = 0.0;
			
		}
		for (int l109 = 0; (l109 < 2); l109 = (l109 + 1)) {
			fRec111[l109] = 0.0;
			
		}
		for (int l110 = 0; (l110 < 2); l110 = (l110 + 1)) {
			fRec114[l110] = 0.0;
			
		}
		for (int l111 = 0; (l111 < 2); l111 = (l111 + 1)) {
			fRec115[l111] = 0.0;
			
		}
		for (int l112 = 0; (l112 < 2); l112 = (l112 + 1)) {
			fRec110[l112] = 0.0;
			
		}
		for (int l113 = 0; (l113 < 2); l113 = (l113 + 1)) {
			fRec116[l113] = 0.0;
			
		}
		for (int l114 = 0; (l114 < 2); l114 = (l114 + 1)) {
			fRec109[l114] = 0.0;
			
		}
		for (int l115 = 0; (l115 < 2); l115 = (l115 + 1)) {
			fRec108[l115] = 0.0;
			
		}
		for (int l116 = 0; (l116 < 2); l116 = (l116 + 1)) {
			fRec121[l116] = 0.0;
			
		}
		for (int l117 = 0; (l117 < 2); l117 = (l117 + 1)) {
			fRec122[l117] = 0.0;
			
		}
		for (int l118 = 0; (l118 < 2); l118 = (l118 + 1)) {
			fRec120[l118] = 0.0;
			
		}
		for (int l119 = 0; (l119 < 2); l119 = (l119 + 1)) {
			fRec123[l119] = 0.0;
			
		}
		for (int l120 = 0; (l120 < 2); l120 = (l120 + 1)) {
			fRec124[l120] = 0.0;
			
		}
		for (int l121 = 0; (l121 < 2); l121 = (l121 + 1)) {
			fRec119[l121] = 0.0;
			
		}
		for (int l122 = 0; (l122 < 2); l122 = (l122 + 1)) {
			fRec125[l122] = 0.0;
			
		}
		for (int l123 = 0; (l123 < 2); l123 = (l123 + 1)) {
			fRec118[l123] = 0.0;
			
		}
		for (int l124 = 0; (l124 < 2); l124 = (l124 + 1)) {
			fRec117[l124] = 0.0;
			
		}
		for (int l125 = 0; (l125 < 2); l125 = (l125 + 1)) {
			fRec130[l125] = 0.0;
			
		}
		for (int l126 = 0; (l126 < 2); l126 = (l126 + 1)) {
			fRec131[l126] = 0.0;
			
		}
		for (int l127 = 0; (l127 < 2); l127 = (l127 + 1)) {
			fRec129[l127] = 0.0;
			
		}
		for (int l128 = 0; (l128 < 2); l128 = (l128 + 1)) {
			fRec132[l128] = 0.0;
			
		}
		for (int l129 = 0; (l129 < 2); l129 = (l129 + 1)) {
			fRec133[l129] = 0.0;
			
		}
		for (int l130 = 0; (l130 < 2); l130 = (l130 + 1)) {
			fRec128[l130] = 0.0;
			
		}
		for (int l131 = 0; (l131 < 2); l131 = (l131 + 1)) {
			fRec134[l131] = 0.0;
			
		}
		for (int l132 = 0; (l132 < 2); l132 = (l132 + 1)) {
			fRec127[l132] = 0.0;
			
		}
		for (int l133 = 0; (l133 < 2); l133 = (l133 + 1)) {
			fRec126[l133] = 0.0;
			
		}
		for (int l134 = 0; (l134 < 2); l134 = (l134 + 1)) {
			fRec0[l134] = 0.0;
			
		}
		for (int l135 = 0; (l135 < 2); l135 = (l135 + 1)) {
			fRec135[l135] = 0.0;
			
		}
		for (int l136 = 0; (l136 < 2); l136 = (l136 + 1)) {
			fRec141[l136] = 0.0;
			
		}
		for (int l137 = 0; (l137 < 2); l137 = (l137 + 1)) {
			fRec142[l137] = 0.0;
			
		}
		for (int l138 = 0; (l138 < 2); l138 = (l138 + 1)) {
			fRec140[l138] = 0.0;
			
		}
		for (int l139 = 0; (l139 < 2); l139 = (l139 + 1)) {
			fRec143[l139] = 0.0;
			
		}
		for (int l140 = 0; (l140 < 2); l140 = (l140 + 1)) {
			fRec144[l140] = 0.0;
			
		}
		for (int l141 = 0; (l141 < 2); l141 = (l141 + 1)) {
			fRec139[l141] = 0.0;
			
		}
		for (int l142 = 0; (l142 < 2); l142 = (l142 + 1)) {
			fRec145[l142] = 0.0;
			
		}
		for (int l143 = 0; (l143 < 2); l143 = (l143 + 1)) {
			fRec138[l143] = 0.0;
			
		}
		for (int l144 = 0; (l144 < 2); l144 = (l144 + 1)) {
			fRec137[l144] = 0.0;
			
		}
		for (int l145 = 0; (l145 < 2); l145 = (l145 + 1)) {
			fRec150[l145] = 0.0;
			
		}
		for (int l146 = 0; (l146 < 2); l146 = (l146 + 1)) {
			fRec151[l146] = 0.0;
			
		}
		for (int l147 = 0; (l147 < 2); l147 = (l147 + 1)) {
			fRec149[l147] = 0.0;
			
		}
		for (int l148 = 0; (l148 < 2); l148 = (l148 + 1)) {
			fRec152[l148] = 0.0;
			
		}
		for (int l149 = 0; (l149 < 2); l149 = (l149 + 1)) {
			fRec153[l149] = 0.0;
			
		}
		for (int l150 = 0; (l150 < 2); l150 = (l150 + 1)) {
			fRec148[l150] = 0.0;
			
		}
		for (int l151 = 0; (l151 < 2); l151 = (l151 + 1)) {
			fRec154[l151] = 0.0;
			
		}
		for (int l152 = 0; (l152 < 2); l152 = (l152 + 1)) {
			fRec147[l152] = 0.0;
			
		}
		for (int l153 = 0; (l153 < 2); l153 = (l153 + 1)) {
			fRec146[l153] = 0.0;
			
		}
		for (int l154 = 0; (l154 < 2); l154 = (l154 + 1)) {
			fRec157[l154] = 0.0;
			
		}
		for (int l155 = 0; (l155 < 2); l155 = (l155 + 1)) {
			fRec158[l155] = 0.0;
			
		}
		for (int l156 = 0; (l156 < 2); l156 = (l156 + 1)) {
			fRec156[l156] = 0.0;
			
		}
		for (int l157 = 0; (l157 < 2); l157 = (l157 + 1)) {
			fRec155[l157] = 0.0;
			
		}
		for (int l158 = 0; (l158 < 2); l158 = (l158 + 1)) {
			fRec161[l158] = 0.0;
			
		}
		for (int l159 = 0; (l159 < 2); l159 = (l159 + 1)) {
			fRec160[l159] = 0.0;
			
		}
		for (int l160 = 0; (l160 < 2); l160 = (l160 + 1)) {
			fRec159[l160] = 0.0;
			
		}
		for (int l161 = 0; (l161 < 2); l161 = (l161 + 1)) {
			fRec166[l161] = 0.0;
			
		}
		for (int l162 = 0; (l162 < 2); l162 = (l162 + 1)) {
			fRec167[l162] = 0.0;
			
		}
		for (int l163 = 0; (l163 < 2); l163 = (l163 + 1)) {
			fRec165[l163] = 0.0;
			
		}
		for (int l164 = 0; (l164 < 2); l164 = (l164 + 1)) {
			fRec168[l164] = 0.0;
			
		}
		for (int l165 = 0; (l165 < 2); l165 = (l165 + 1)) {
			fRec169[l165] = 0.0;
			
		}
		for (int l166 = 0; (l166 < 2); l166 = (l166 + 1)) {
			fRec164[l166] = 0.0;
			
		}
		for (int l167 = 0; (l167 < 2); l167 = (l167 + 1)) {
			fRec170[l167] = 0.0;
			
		}
		for (int l168 = 0; (l168 < 2); l168 = (l168 + 1)) {
			fRec163[l168] = 0.0;
			
		}
		for (int l169 = 0; (l169 < 2); l169 = (l169 + 1)) {
			fRec162[l169] = 0.0;
			
		}
		for (int l170 = 0; (l170 < 2); l170 = (l170 + 1)) {
			fRec174[l170] = 0.0;
			
		}
		for (int l171 = 0; (l171 < 2); l171 = (l171 + 1)) {
			fRec175[l171] = 0.0;
			
		}
		for (int l172 = 0; (l172 < 2); l172 = (l172 + 1)) {
			fRec173[l172] = 0.0;
			
		}
		for (int l173 = 0; (l173 < 2); l173 = (l173 + 1)) {
			fRec176[l173] = 0.0;
			
		}
		for (int l174 = 0; (l174 < 2); l174 = (l174 + 1)) {
			fRec177[l174] = 0.0;
			
		}
		for (int l175 = 0; (l175 < 2); l175 = (l175 + 1)) {
			fRec172[l175] = 0.0;
			
		}
		for (int l176 = 0; (l176 < 2); l176 = (l176 + 1)) {
			fRec171[l176] = 0.0;
			
		}
		for (int l177 = 0; (l177 < 2); l177 = (l177 + 1)) {
			fRec181[l177] = 0.0;
			
		}
		for (int l178 = 0; (l178 < 2); l178 = (l178 + 1)) {
			fRec182[l178] = 0.0;
			
		}
		for (int l179 = 0; (l179 < 2); l179 = (l179 + 1)) {
			fRec180[l179] = 0.0;
			
		}
		for (int l180 = 0; (l180 < 2); l180 = (l180 + 1)) {
			fRec183[l180] = 0.0;
			
		}
		for (int l181 = 0; (l181 < 2); l181 = (l181 + 1)) {
			fRec179[l181] = 0.0;
			
		}
		for (int l182 = 0; (l182 < 2); l182 = (l182 + 1)) {
			fRec178[l182] = 0.0;
			
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
			fRec185[l187] = 0.0;
			
		}
		for (int l188 = 0; (l188 < 2); l188 = (l188 + 1)) {
			fRec184[l188] = 0.0;
			
		}
		for (int l189 = 0; (l189 < 2); l189 = (l189 + 1)) {
			fRec193[l189] = 0.0;
			
		}
		for (int l190 = 0; (l190 < 2); l190 = (l190 + 1)) {
			fRec194[l190] = 0.0;
			
		}
		for (int l191 = 0; (l191 < 2); l191 = (l191 + 1)) {
			fRec192[l191] = 0.0;
			
		}
		for (int l192 = 0; (l192 < 2); l192 = (l192 + 1)) {
			fRec195[l192] = 0.0;
			
		}
		for (int l193 = 0; (l193 < 2); l193 = (l193 + 1)) {
			fRec196[l193] = 0.0;
			
		}
		for (int l194 = 0; (l194 < 2); l194 = (l194 + 1)) {
			fRec191[l194] = 0.0;
			
		}
		for (int l195 = 0; (l195 < 2); l195 = (l195 + 1)) {
			fRec190[l195] = 0.0;
			
		}
		for (int l196 = 0; (l196 < 2); l196 = (l196 + 1)) {
			fRec136[l196] = 0.0;
			
		}
		for (int l197 = 0; (l197 < 2); l197 = (l197 + 1)) {
			fRec197[l197] = 0.0;
			
		}
		for (int l198 = 0; (l198 < 2); l198 = (l198 + 1)) {
			fRec198[l198] = 0.0;
			
		}
		for (int l199 = 0; (l199 < 2); l199 = (l199 + 1)) {
			fRec199[l199] = 0.0;
			
		}
		for (int l200 = 0; (l200 < 2); l200 = (l200 + 1)) {
			fRec200[l200] = 0.0;
			
		}
		for (int l201 = 0; (l201 < 2); l201 = (l201 + 1)) {
			fRec204[l201] = 0.0;
			
		}
		for (int l202 = 0; (l202 < 2); l202 = (l202 + 1)) {
			fRec205[l202] = 0.0;
			
		}
		for (int l203 = 0; (l203 < 2); l203 = (l203 + 1)) {
			fRec203[l203] = 0.0;
			
		}
		for (int l204 = 0; (l204 < 2); l204 = (l204 + 1)) {
			fRec202[l204] = 0.0;
			
		}
		for (int l205 = 0; (l205 < 2); l205 = (l205 + 1)) {
			fRec209[l205] = 0.0;
			
		}
		for (int l206 = 0; (l206 < 2); l206 = (l206 + 1)) {
			fRec210[l206] = 0.0;
			
		}
		for (int l207 = 0; (l207 < 2); l207 = (l207 + 1)) {
			fRec208[l207] = 0.0;
			
		}
		for (int l208 = 0; (l208 < 2); l208 = (l208 + 1)) {
			fRec211[l208] = 0.0;
			
		}
		for (int l209 = 0; (l209 < 2); l209 = (l209 + 1)) {
			fRec207[l209] = 0.0;
			
		}
		for (int l210 = 0; (l210 < 2); l210 = (l210 + 1)) {
			fRec206[l210] = 0.0;
			
		}
		for (int l211 = 0; (l211 < 2); l211 = (l211 + 1)) {
			fRec216[l211] = 0.0;
			
		}
		for (int l212 = 0; (l212 < 2); l212 = (l212 + 1)) {
			fRec217[l212] = 0.0;
			
		}
		for (int l213 = 0; (l213 < 2); l213 = (l213 + 1)) {
			fRec215[l213] = 0.0;
			
		}
		for (int l214 = 0; (l214 < 2); l214 = (l214 + 1)) {
			fRec218[l214] = 0.0;
			
		}
		for (int l215 = 0; (l215 < 2); l215 = (l215 + 1)) {
			fRec219[l215] = 0.0;
			
		}
		for (int l216 = 0; (l216 < 2); l216 = (l216 + 1)) {
			fRec214[l216] = 0.0;
			
		}
		for (int l217 = 0; (l217 < 2); l217 = (l217 + 1)) {
			fRec220[l217] = 0.0;
			
		}
		for (int l218 = 0; (l218 < 2); l218 = (l218 + 1)) {
			fRec213[l218] = 0.0;
			
		}
		for (int l219 = 0; (l219 < 2); l219 = (l219 + 1)) {
			fRec212[l219] = 0.0;
			
		}
		for (int l220 = 0; (l220 < 2); l220 = (l220 + 1)) {
			fRec224[l220] = 0.0;
			
		}
		for (int l221 = 0; (l221 < 2); l221 = (l221 + 1)) {
			fRec225[l221] = 0.0;
			
		}
		for (int l222 = 0; (l222 < 2); l222 = (l222 + 1)) {
			fRec223[l222] = 0.0;
			
		}
		for (int l223 = 0; (l223 < 2); l223 = (l223 + 1)) {
			fRec226[l223] = 0.0;
			
		}
		for (int l224 = 0; (l224 < 2); l224 = (l224 + 1)) {
			fRec227[l224] = 0.0;
			
		}
		for (int l225 = 0; (l225 < 2); l225 = (l225 + 1)) {
			fRec222[l225] = 0.0;
			
		}
		for (int l226 = 0; (l226 < 2); l226 = (l226 + 1)) {
			fRec221[l226] = 0.0;
			
		}
		for (int l227 = 0; (l227 < 2); l227 = (l227 + 1)) {
			fRec232[l227] = 0.0;
			
		}
		for (int l228 = 0; (l228 < 2); l228 = (l228 + 1)) {
			fRec233[l228] = 0.0;
			
		}
		for (int l229 = 0; (l229 < 2); l229 = (l229 + 1)) {
			fRec231[l229] = 0.0;
			
		}
		for (int l230 = 0; (l230 < 2); l230 = (l230 + 1)) {
			fRec234[l230] = 0.0;
			
		}
		for (int l231 = 0; (l231 < 2); l231 = (l231 + 1)) {
			fRec235[l231] = 0.0;
			
		}
		for (int l232 = 0; (l232 < 2); l232 = (l232 + 1)) {
			fRec230[l232] = 0.0;
			
		}
		for (int l233 = 0; (l233 < 2); l233 = (l233 + 1)) {
			fRec236[l233] = 0.0;
			
		}
		for (int l234 = 0; (l234 < 2); l234 = (l234 + 1)) {
			fRec229[l234] = 0.0;
			
		}
		for (int l235 = 0; (l235 < 2); l235 = (l235 + 1)) {
			fRec228[l235] = 0.0;
			
		}
		for (int l236 = 0; (l236 < 2); l236 = (l236 + 1)) {
			fRec240[l236] = 0.0;
			
		}
		for (int l237 = 0; (l237 < 2); l237 = (l237 + 1)) {
			fRec241[l237] = 0.0;
			
		}
		for (int l238 = 0; (l238 < 2); l238 = (l238 + 1)) {
			fRec239[l238] = 0.0;
			
		}
		for (int l239 = 0; (l239 < 2); l239 = (l239 + 1)) {
			fRec242[l239] = 0.0;
			
		}
		for (int l240 = 0; (l240 < 2); l240 = (l240 + 1)) {
			fRec243[l240] = 0.0;
			
		}
		for (int l241 = 0; (l241 < 2); l241 = (l241 + 1)) {
			fRec238[l241] = 0.0;
			
		}
		for (int l242 = 0; (l242 < 2); l242 = (l242 + 1)) {
			fRec237[l242] = 0.0;
			
		}
		for (int l243 = 0; (l243 < 2); l243 = (l243 + 1)) {
			fRec201[l243] = 0.0;
			
		}
		for (int l244 = 0; (l244 < 2); l244 = (l244 + 1)) {
			fRec244[l244] = 0.0;
			
		}
		for (int l245 = 0; (l245 < 2); l245 = (l245 + 1)) {
			fRec245[l245] = 0.0;
			
		}
		for (int l246 = 0; (l246 < 2); l246 = (l246 + 1)) {
			fRec246[l246] = 0.0;
			
		}
		for (int l247 = 0; (l247 < 2); l247 = (l247 + 1)) {
			fRec247[l247] = 0.0;
			
		}
		for (int l248 = 0; (l248 < 2); l248 = (l248 + 1)) {
			fRec248[l248] = 0.0;
			
		}
		for (int l249 = 0; (l249 < 2); l249 = (l249 + 1)) {
			fRec249[l249] = 0.0;
			
		}
		for (int l250 = 0; (l250 < 2); l250 = (l250 + 1)) {
			fRec250[l250] = 0.0;
			
		}
		for (int l251 = 0; (l251 < 2); l251 = (l251 + 1)) {
			fRec251[l251] = 0.0;
			
		}
		for (int l252 = 0; (l252 < 2); l252 = (l252 + 1)) {
			fRec252[l252] = 0.0;
			
		}
		for (int l253 = 0; (l253 < 2); l253 = (l253 + 1)) {
			fRec253[l253] = 0.0;
			
		}
		for (int l254 = 0; (l254 < 2); l254 = (l254 + 1)) {
			fRec254[l254] = 0.0;
			
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
		ui_interface->addVerticalBargraph("0x30a7140", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32410d0", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30b2640", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30bbfe0", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3237cf0", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32ce850", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30ca880", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30d5f10", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30e15a0", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3275670", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32dcfa0", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3283db0", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x30f4160", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3102d30", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3111900", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31205b0", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3267060", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x332b260", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32952a0", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3301e70", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3135c70", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3146530", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3156df0", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3167790", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3178210", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5");
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x322c850", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x331ac70", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3254500", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32f17a0", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3218ee0", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3192850", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31a6650", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31ba460", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31ce340", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31e2300", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31f63a0", &fVbargraph20, -70.0, 6.0);
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
		ui_interface->declare(&fVbargraph21, "osc", "/output1");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31fb960", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph22, "osc", "/output2");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3205910", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph32, "osc", "/output3");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x329a790", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph33, "osc", "/output4");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32a3b00", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph34, "osc", "/output5");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32acf50", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph35, "osc", "/output6");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32b4340", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph36, "osc", "/output7");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32c32f0", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph43, "osc", "/output8");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3333900", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph44, "osc", "/output9");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3341c70", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph45, "osc", "/output10");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3350290", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph46, "osc", "/output11");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x335ce90", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph47, "osc", "/output12");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x33674b0", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph48, "osc", "/output13");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3370c60", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph49, "osc", "/output14");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x337ac60", &fVbargraph49, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph50, "osc", "/output15");
		ui_interface->declare(&fVbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3382660", &fVbargraph50, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph51, "osc", "/output16");
		ui_interface->declare(&fVbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x338bd60", &fVbargraph51, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph52, "osc", "/output17");
		ui_interface->declare(&fVbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3395750", &fVbargraph52, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph53, "osc", "/output18");
		ui_interface->declare(&fVbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x339f010", &fVbargraph53, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph54, "osc", "/output19");
		ui_interface->declare(&fVbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x33b5120", &fVbargraph54, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph55, "osc", "/output20");
		ui_interface->declare(&fVbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x33c5dc0", &fVbargraph55, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph56, "osc", "/output21");
		ui_interface->declare(&fVbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x33d5ee0", &fVbargraph56, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph57, "osc", "/output22");
		ui_interface->declare(&fVbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x33e6570", &fVbargraph57, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph58, "osc", "/output23");
		ui_interface->declare(&fVbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x33f06b0", &fVbargraph58, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph59, "osc", "/output24");
		ui_interface->declare(&fVbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x33fa2d0", &fVbargraph59, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph60, "osc", "/output25");
		ui_interface->declare(&fVbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3404390", &fVbargraph60, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph61, "osc", "/output26");
		ui_interface->declare(&fVbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x340e350", &fVbargraph61, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph62, "osc", "/output27");
		ui_interface->declare(&fVbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3423b60", &fVbargraph62, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph63, "osc", "/output28");
		ui_interface->declare(&fVbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3431100", &fVbargraph63, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph64, "osc", "/output29");
		ui_interface->declare(&fVbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x34419d0", &fVbargraph64, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph65, "osc", "/output30");
		ui_interface->declare(&fVbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3451640", &fVbargraph65, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph66, "osc", "/output31");
		ui_interface->declare(&fVbargraph66, "unit", "dB");
		ui_interface->addVerticalBargraph("0x34674a0", &fVbargraph66, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph67, "osc", "/output32");
		ui_interface->declare(&fVbargraph67, "unit", "dB");
		ui_interface->addVerticalBargraph("0x347a9e0", &fVbargraph67, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph68, "osc", "/output33");
		ui_interface->declare(&fVbargraph68, "unit", "dB");
		ui_interface->addVerticalBargraph("0x348ce70", &fVbargraph68, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph69, "osc", "/output34");
		ui_interface->declare(&fVbargraph69, "unit", "dB");
		ui_interface->addVerticalBargraph("0x349d540", &fVbargraph69, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph70, "osc", "/output35");
		ui_interface->declare(&fVbargraph70, "unit", "dB");
		ui_interface->addVerticalBargraph("0x34aea90", &fVbargraph70, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph71, "osc", "/output36");
		ui_interface->declare(&fVbargraph71, "unit", "dB");
		ui_interface->addVerticalBargraph("0x34bfc90", &fVbargraph71, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph72, "osc", "/output37");
		ui_interface->declare(&fVbargraph72, "unit", "dB");
		ui_interface->addVerticalBargraph("0x34d1910", &fVbargraph72, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph73, "osc", "/output38");
		ui_interface->declare(&fVbargraph73, "unit", "dB");
		ui_interface->addVerticalBargraph("0x34e5ab0", &fVbargraph73, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph74, "osc", "/output39");
		ui_interface->declare(&fVbargraph74, "unit", "dB");
		ui_interface->addVerticalBargraph("0x34ef680", &fVbargraph74, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph75, "osc", "/output40");
		ui_interface->declare(&fVbargraph75, "unit", "dB");
		ui_interface->addVerticalBargraph("0x34f8f40", &fVbargraph75, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph76, "osc", "/output41");
		ui_interface->declare(&fVbargraph76, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3502ac0", &fVbargraph76, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph77, "osc", "/output42");
		ui_interface->declare(&fVbargraph77, "unit", "dB");
		ui_interface->addVerticalBargraph("0x350c930", &fVbargraph77, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph78, "osc", "/output43");
		ui_interface->declare(&fVbargraph78, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35162b0", &fVbargraph78, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph79, "osc", "/output44");
		ui_interface->declare(&fVbargraph79, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35200d0", &fVbargraph79, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph80, "osc", "/output45");
		ui_interface->declare(&fVbargraph80, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3529df0", &fVbargraph80, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph81, "osc", "/output46");
		ui_interface->declare(&fVbargraph81, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3533bf0", &fVbargraph81, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph82, "osc", "/output47");
		ui_interface->declare(&fVbargraph82, "unit", "dB");
		ui_interface->addVerticalBargraph("0x353db00", &fVbargraph82, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph83, "osc", "/output48");
		ui_interface->declare(&fVbargraph83, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3547710", &fVbargraph83, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph84, "osc", "/output49");
		ui_interface->declare(&fVbargraph84, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35517b0", &fVbargraph84, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph85, "osc", "/output50");
		ui_interface->declare(&fVbargraph85, "unit", "dB");
		ui_interface->addVerticalBargraph("0x355bb10", &fVbargraph85, -70.0, 6.0);
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
			double fTemp29 = (fRec3[0] * double(input20[i]));
			fRec49[0] = (fRec49[1] + fRec48[1]);
			fRec50[0] = (fRec50[1] + fRec49[1]);
			fRec48[0] = (fSlow20 * ((fSlow3 * fTemp29) - ((fSlow22 * fRec49[0]) + (fSlow21 * fRec50[0]))));
			fRec51[0] = (fRec51[1] + fRec47[1]);
			fRec52[0] = (fRec52[1] + fRec51[1]);
			fRec47[0] = (fSlow19 * (fRec48[0] - ((fSlow24 * fRec51[0]) + (fSlow23 * fRec52[0]))));
			double fTemp30 = (fRec47[0] + (fSlow4 * fTemp29));
			fRec46[0] = max((fRec46[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph10 = FAUSTFLOAT(fRec46[0]);
			double fTemp31 = (0.038095238095238099 * fTemp30);
			double fTemp32 = (fRec3[0] * double(input21[i]));
			fRec56[0] = (fRec56[1] + fRec55[1]);
			fRec57[0] = (fRec57[1] + fRec56[1]);
			fRec55[0] = (fSlow20 * ((fSlow3 * fTemp32) - ((fSlow22 * fRec56[0]) + (fSlow21 * fRec57[0]))));
			fRec58[0] = (fRec58[1] + fRec54[1]);
			fRec59[0] = (fRec59[1] + fRec58[1]);
			fRec54[0] = (fSlow19 * (fRec55[0] - ((fSlow24 * fRec58[0]) + (fSlow23 * fRec59[0]))));
			double fTemp33 = (fRec54[0] + (fSlow4 * fTemp32));
			fRec53[0] = max((fRec53[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph11 = FAUSTFLOAT(fRec53[0]);
			double fTemp34 = (7.3765204086483564e-18 * fTemp33);
			double fTemp35 = (fRec3[0] * double(input22[i]));
			fRec63[0] = (fRec63[1] + fRec62[1]);
			fRec64[0] = (fRec64[1] + fRec63[1]);
			fRec62[0] = (fSlow20 * ((fSlow3 * fTemp35) - ((fSlow22 * fRec63[0]) + (fSlow21 * fRec64[0]))));
			fRec65[0] = (fRec65[1] + fRec61[1]);
			fRec66[0] = (fRec66[1] + fRec65[1]);
			fRec61[0] = (fSlow19 * (fRec62[0] - ((fSlow24 * fRec65[0]) + (fSlow23 * fRec66[0]))));
			double fTemp36 = (fRec61[0] + (fSlow4 * fTemp35));
			fRec60[0] = max((fRec60[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fVbargraph12 = FAUSTFLOAT(fRec60[0]);
			double fTemp37 = (4.7908068913603203e-34 * fTemp36);
			double fTemp38 = (fRec3[0] * double(input23[i]));
			fRec70[0] = (fRec70[1] + fRec69[1]);
			fRec71[0] = (fRec71[1] + fRec70[1]);
			fRec69[0] = (fSlow20 * ((fSlow3 * fTemp38) - ((fSlow22 * fRec70[0]) + (fSlow21 * fRec71[0]))));
			fRec72[0] = (fRec72[1] + fRec68[1]);
			fRec73[0] = (fRec73[1] + fRec72[1]);
			fRec68[0] = (fSlow19 * (fRec69[0] - ((fSlow24 * fRec72[0]) + (fSlow23 * fRec73[0]))));
			double fTemp39 = (fRec68[0] + (fSlow4 * fTemp38));
			fRec67[0] = max((fRec67[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph13 = FAUSTFLOAT(fRec67[0]);
			double fTemp40 = (1.8293731016560941e-50 * fTemp39);
			double fTemp41 = (fRec3[0] * double(input24[i]));
			fRec77[0] = (fRec77[1] + fRec76[1]);
			fRec78[0] = (fRec78[1] + fRec77[1]);
			fRec76[0] = (fSlow20 * ((fSlow3 * fTemp41) - ((fSlow22 * fRec77[0]) + (fSlow21 * fRec78[0]))));
			fRec79[0] = (fRec79[1] + fRec75[1]);
			fRec80[0] = (fRec80[1] + fRec79[1]);
			fRec75[0] = (fSlow19 * (fRec76[0] - ((fSlow24 * fRec79[0]) + (fSlow23 * fRec80[0]))));
			double fTemp42 = (fRec75[0] + (fSlow4 * fTemp41));
			fRec74[0] = max((fRec74[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fVbargraph14 = FAUSTFLOAT(fRec74[0]);
			double fTemp43 = (3.9603917912335094e-67 * fTemp42);
			double fTemp44 = (fRec3[0] * double(input30[i]));
			fRec85[0] = (fRec85[1] + fRec84[1]);
			fRec86[0] = (fRec86[1] + fRec85[1]);
			fRec84[0] = (fSlow27 * ((fSlow3 * fTemp44) - ((fSlow29 * fRec85[0]) + (fSlow28 * fRec86[0]))));
			fRec87[0] = (fRec87[1] + fRec83[1]);
			fRec88[0] = (fRec88[1] + fRec87[1]);
			fRec83[0] = (fSlow26 * (fRec84[0] - ((fSlow31 * fRec87[0]) + (fSlow30 * fRec88[0]))));
			fRec89[0] = (fRec89[1] + fRec82[1]);
			fRec82[0] = (fSlow25 * (fRec83[0] - (fSlow32 * fRec89[0])));
			double fTemp45 = (fRec82[0] + (fSlow4 * fTemp44));
			fRec81[0] = max((fRec81[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fVbargraph15 = FAUSTFLOAT(fRec81[0]);
			double fTemp46 = (0.042115870353719366 * fTemp45);
			double fTemp47 = (fRec3[0] * double(input31[i]));
			fRec94[0] = (fRec94[1] + fRec93[1]);
			fRec95[0] = (fRec95[1] + fRec94[1]);
			fRec93[0] = (fSlow27 * ((fSlow3 * fTemp47) - ((fSlow29 * fRec94[0]) + (fSlow28 * fRec95[0]))));
			fRec96[0] = (fRec96[1] + fRec92[1]);
			fRec97[0] = (fRec97[1] + fRec96[1]);
			fRec92[0] = (fSlow26 * (fRec93[0] - ((fSlow31 * fRec96[0]) + (fSlow30 * fRec97[0]))));
			fRec98[0] = (fRec98[1] + fRec91[1]);
			fRec91[0] = (fSlow25 * (fRec92[0] - (fSlow32 * fRec98[0])));
			double fTemp48 = (fRec91[0] + (fSlow4 * fTemp47));
			fRec90[0] = max((fRec90[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph16 = FAUSTFLOAT(fRec90[0]);
			double fTemp49 = (9.9878558487400326e-18 * fTemp48);
			double fTemp50 = (fRec3[0] * double(input32[i]));
			fRec103[0] = (fRec103[1] + fRec102[1]);
			fRec104[0] = (fRec104[1] + fRec103[1]);
			fRec102[0] = (fSlow27 * ((fSlow3 * fTemp50) - ((fSlow29 * fRec103[0]) + (fSlow28 * fRec104[0]))));
			fRec105[0] = (fRec105[1] + fRec101[1]);
			fRec106[0] = (fRec106[1] + fRec105[1]);
			fRec101[0] = (fSlow26 * (fRec102[0] - ((fSlow31 * fRec105[0]) + (fSlow30 * fRec106[0]))));
			fRec107[0] = (fRec107[1] + fRec100[1]);
			fRec100[0] = (fSlow25 * (fRec101[0] - (fSlow32 * fRec107[0])));
			double fTemp51 = (fRec100[0] + (fSlow4 * fTemp50));
			fRec99[0] = max((fRec99[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph17 = FAUSTFLOAT(fRec99[0]);
			double fTemp52 = (8.0904400869483595e-34 * fTemp51);
			double fTemp53 = (fRec3[0] * double(input33[i]));
			fRec112[0] = (fRec112[1] + fRec111[1]);
			fRec113[0] = (fRec113[1] + fRec112[1]);
			fRec111[0] = (fSlow27 * ((fSlow3 * fTemp53) - ((fSlow29 * fRec112[0]) + (fSlow28 * fRec113[0]))));
			fRec114[0] = (fRec114[1] + fRec110[1]);
			fRec115[0] = (fRec115[1] + fRec114[1]);
			fRec110[0] = (fSlow26 * (fRec111[0] - ((fSlow31 * fRec114[0]) + (fSlow30 * fRec115[0]))));
			fRec116[0] = (fRec116[1] + fRec109[1]);
			fRec109[0] = (fSlow25 * (fRec110[0] - (fSlow32 * fRec116[0])));
			double fTemp54 = (fRec109[0] + (fSlow4 * fTemp53));
			fRec108[0] = max((fRec108[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fVbargraph18 = FAUSTFLOAT(fRec108[0]);
			double fTemp55 = (4.0448961198413007e-50 * fTemp54);
			double fTemp56 = (fRec3[0] * double(input34[i]));
			fRec121[0] = (fRec121[1] + fRec120[1]);
			fRec122[0] = (fRec122[1] + fRec121[1]);
			fRec120[0] = (fSlow27 * ((fSlow3 * fTemp56) - ((fSlow29 * fRec121[0]) + (fSlow28 * fRec122[0]))));
			fRec123[0] = (fRec123[1] + fRec119[1]);
			fRec124[0] = (fRec124[1] + fRec123[1]);
			fRec119[0] = (fSlow26 * (fRec120[0] - ((fSlow31 * fRec123[0]) + (fSlow30 * fRec124[0]))));
			fRec125[0] = (fRec125[1] + fRec118[1]);
			fRec118[0] = (fSlow25 * (fRec119[0] - (fSlow32 * fRec125[0])));
			double fTemp57 = (fRec118[0] + (fSlow4 * fTemp56));
			fRec117[0] = max((fRec117[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fVbargraph19 = FAUSTFLOAT(fRec117[0]);
			double fTemp58 = (1.3135133594325083e-66 * fTemp57);
			double fTemp59 = (fRec3[0] * double(input35[i]));
			fRec130[0] = (fRec130[1] + fRec129[1]);
			fRec131[0] = (fRec131[1] + fRec130[1]);
			fRec129[0] = (fSlow27 * ((fSlow3 * fTemp59) - ((fSlow29 * fRec130[0]) + (fSlow28 * fRec131[0]))));
			fRec132[0] = (fRec132[1] + fRec128[1]);
			fRec133[0] = (fRec133[1] + fRec132[1]);
			fRec128[0] = (fSlow26 * (fRec129[0] - ((fSlow31 * fRec132[0]) + (fSlow30 * fRec133[0]))));
			fRec134[0] = (fRec134[1] + fRec127[1]);
			fRec127[0] = (fSlow25 * (fRec128[0] - (fSlow32 * fRec134[0])));
			double fTemp60 = (fRec127[0] + (fSlow4 * fTemp59));
			fRec126[0] = max((fRec126[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fVbargraph20 = FAUSTFLOAT(fRec126[0]);
			double fTemp61 = (2.5434040020076175e-83 * fTemp60);
			double fTemp62 = (fRec1[0] * ((((((((((((((((((((fTemp1 + fTemp4) + fTemp7) + fTemp10) + fTemp13) + fTemp16) + fTemp19) + fTemp22) + fTemp25) + fTemp28) + fTemp31) + fTemp34) + fTemp37) + fTemp40) + fTemp43) + fTemp46) + fTemp49) + fTemp52) + fTemp55) + fTemp58) + fTemp61));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fVbargraph21 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp62);
			double fTemp63 = (0.021994295969128601 * fTemp6);
			double fTemp64 = (0.024590370452110585 * fTemp15);
			double fTemp65 = (0.026560635762986527 * fTemp27);
			double fTemp66 = (0.014285714285714285 * fTemp30);
			double fTemp67 = (0.028171808490950554 * fTemp42);
			double fTemp68 = (0.020389258061373223 * fTemp48);
			double fTemp69 = (0.029546842014263944 * fTemp60);
			double fTemp70 = (0.014197256999998666 * fTemp9);
			double fTemp71 = (0.020573779994945588 * fTemp21);
			double fTemp72 = (0.021295885499997998 * fTemp36);
			double fTemp73 = (0.022022915754780329 * fTemp54);
			double fTemp74 = (fRec1[0] * ((((((((fTemp1 + fTemp63) + fTemp64) + fTemp65) + fTemp66) + fTemp67) + fTemp68) + fTemp69) - (((fTemp70 + fTemp71) + fTemp72) + fTemp73)));
			fRec135[0] = max((fRec135[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp74))))));
			fVbargraph22 = FAUSTFLOAT(fRec135[0]);
			output1[i] = FAUSTFLOAT(fTemp74);
			double fTemp75 = (fRec3[0] * double(input29[i]));
			fRec141[0] = (fRec141[1] + fRec140[1]);
			fRec142[0] = (fRec142[1] + fRec141[1]);
			fRec140[0] = (fSlow27 * ((fSlow3 * fTemp75) - ((fSlow29 * fRec141[0]) + (fSlow28 * fRec142[0]))));
			fRec143[0] = (fRec143[1] + fRec139[1]);
			fRec144[0] = (fRec144[1] + fRec143[1]);
			fRec139[0] = (fSlow26 * (fRec140[0] - ((fSlow31 * fRec143[0]) + (fSlow30 * fRec144[0]))));
			fRec145[0] = (fRec145[1] + fRec138[1]);
			fRec138[0] = (fSlow25 * (fRec139[0] - (fSlow32 * fRec145[0])));
			double fTemp76 = (fRec138[0] + (fSlow4 * fTemp75));
			fRec137[0] = max((fRec137[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp76))))));
			fVbargraph23 = FAUSTFLOAT(fRec137[0]);
			double fTemp77 = (0.020389258061373223 * fTemp76);
			double fTemp78 = (fRec3[0] * double(input25[i]));
			fRec150[0] = (fRec150[1] + fRec149[1]);
			fRec151[0] = (fRec151[1] + fRec150[1]);
			fRec149[0] = (fSlow27 * ((fSlow3 * fTemp78) - ((fSlow29 * fRec150[0]) + (fSlow28 * fRec151[0]))));
			fRec152[0] = (fRec152[1] + fRec148[1]);
			fRec153[0] = (fRec153[1] + fRec152[1]);
			fRec148[0] = (fSlow26 * (fRec149[0] - ((fSlow31 * fRec152[0]) + (fSlow30 * fRec153[0]))));
			fRec154[0] = (fRec154[1] + fRec147[1]);
			fRec147[0] = (fSlow25 * (fRec148[0] - (fSlow32 * fRec154[0])));
			double fTemp79 = (fRec147[0] + (fSlow4 * fTemp78));
			fRec146[0] = max((fRec146[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp79))))));
			fVbargraph24 = FAUSTFLOAT(fRec146[0]);
			double fTemp80 = (0.029546842014263944 * fTemp79);
			double fTemp81 = (fRec3[0] * double(input4[i]));
			fRec157[0] = (fRec157[1] + fRec156[1]);
			fRec158[0] = (fRec158[1] + fRec157[1]);
			fRec156[0] = (fSlow11 * ((fSlow3 * fTemp81) - ((fSlow13 * fRec157[0]) + (fSlow12 * fRec158[0]))));
			double fTemp82 = (fRec156[0] + (fSlow4 * fTemp81));
			fRec155[0] = max((fRec155[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp82))))));
			fVbargraph25 = FAUSTFLOAT(fRec155[0]);
			double fTemp83 = (fRec3[0] * double(input1[i]));
			fRec161[0] = (fRec161[1] + fRec160[1]);
			fRec160[0] = (fSlow8 * ((fSlow3 * fTemp83) - (fSlow9 * fRec161[0])));
			double fTemp84 = (fRec160[0] + (fSlow4 * fTemp83));
			fRec159[0] = max((fRec159[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp84))))));
			fVbargraph26 = FAUSTFLOAT(fRec159[0]);
			double fTemp85 = (0.021994295969128601 * fTemp84);
			double fTemp86 = (fRec3[0] * double(input27[i]));
			fRec166[0] = (fRec166[1] + fRec165[1]);
			fRec167[0] = (fRec167[1] + fRec166[1]);
			fRec165[0] = (fSlow27 * ((fSlow3 * fTemp86) - ((fSlow29 * fRec166[0]) + (fSlow28 * fRec167[0]))));
			fRec168[0] = (fRec168[1] + fRec164[1]);
			fRec169[0] = (fRec169[1] + fRec168[1]);
			fRec164[0] = (fSlow26 * (fRec165[0] - ((fSlow31 * fRec168[0]) + (fSlow30 * fRec169[0]))));
			fRec170[0] = (fRec170[1] + fRec163[1]);
			fRec163[0] = (fSlow25 * (fRec164[0] - (fSlow32 * fRec170[0])));
			double fTemp87 = (fRec163[0] + (fSlow4 * fTemp86));
			fRec162[0] = max((fRec162[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp87))))));
			fVbargraph27 = FAUSTFLOAT(fRec162[0]);
			double fTemp88 = (0.022022915754780329 * fTemp87);
			double fTemp89 = (fTemp1 + fTemp7);
			double fTemp90 = (fRec3[0] * double(input16[i]));
			fRec174[0] = (fRec174[1] + fRec173[1]);
			fRec175[0] = (fRec175[1] + fRec174[1]);
			fRec173[0] = (fSlow20 * ((fSlow3 * fTemp90) - ((fSlow22 * fRec174[0]) + (fSlow21 * fRec175[0]))));
			fRec176[0] = (fRec176[1] + fRec172[1]);
			fRec177[0] = (fRec177[1] + fRec176[1]);
			fRec172[0] = (fSlow19 * (fRec173[0] - ((fSlow24 * fRec176[0]) + (fSlow23 * fRec177[0]))));
			double fTemp91 = (fRec172[0] + (fSlow4 * fTemp90));
			fRec171[0] = max((fRec171[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp91))))));
			fVbargraph28 = FAUSTFLOAT(fRec171[0]);
			double fTemp92 = (fRec3[0] * double(input9[i]));
			fRec181[0] = (fRec181[1] + fRec180[1]);
			fRec182[0] = (fRec182[1] + fRec181[1]);
			fRec180[0] = (fSlow15 * ((fSlow3 * fTemp92) - ((fSlow17 * fRec181[0]) + (fSlow16 * fRec182[0]))));
			fRec183[0] = (fRec183[1] + fRec179[1]);
			fRec179[0] = (fSlow14 * (fRec180[0] - (fSlow18 * fRec183[0])));
			double fTemp93 = (fRec179[0] + (fSlow4 * fTemp92));
			fRec178[0] = max((fRec178[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fVbargraph29 = FAUSTFLOAT(fRec178[0]);
			double fTemp94 = (0.026560635762986527 * fTemp93);
			double fTemp95 = (fRec3[0] * double(input11[i]));
			fRec187[0] = (fRec187[1] + fRec186[1]);
			fRec188[0] = (fRec188[1] + fRec187[1]);
			fRec186[0] = (fSlow15 * ((fSlow3 * fTemp95) - ((fSlow17 * fRec187[0]) + (fSlow16 * fRec188[0]))));
			fRec189[0] = (fRec189[1] + fRec185[1]);
			fRec185[0] = (fSlow14 * (fRec186[0] - (fSlow18 * fRec189[0])));
			double fTemp96 = (fRec185[0] + (fSlow4 * fTemp95));
			fRec184[0] = max((fRec184[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp96))))));
			fVbargraph30 = FAUSTFLOAT(fRec184[0]);
			double fTemp97 = (0.020573779994945588 * fTemp96);
			double fTemp98 = (fRec3[0] * double(input18[i]));
			fRec193[0] = (fRec193[1] + fRec192[1]);
			fRec194[0] = (fRec194[1] + fRec193[1]);
			fRec192[0] = (fSlow20 * ((fSlow3 * fTemp98) - ((fSlow22 * fRec193[0]) + (fSlow21 * fRec194[0]))));
			fRec195[0] = (fRec195[1] + fRec191[1]);
			fRec196[0] = (fRec196[1] + fRec195[1]);
			fRec191[0] = (fSlow19 * (fRec192[0] - ((fSlow24 * fRec195[0]) + (fSlow23 * fRec196[0]))));
			double fTemp99 = (fRec191[0] + (fSlow4 * fTemp98));
			fRec190[0] = max((fRec190[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp99))))));
			fVbargraph31 = FAUSTFLOAT(fRec190[0]);
			double fTemp100 = (fRec1[0] * ((fTemp77 + (fTemp80 + ((3.0114518464024883e-18 * fTemp82) + (fTemp85 + (fTemp88 + ((((fTemp67 + (fTemp72 + (fTemp66 + fTemp89))) + (1.2484819810925041e-18 * fTemp48)) + (4.0455439930475322e-18 * fTemp54)) + (9.0461113744202184e-18 * fTemp60))))))) - ((6.9001030189269641e-18 * fTemp91) + (fTemp94 + ((fTemp97 + (((fTemp70 + fTemp64) + (1.2597806908585981e-18 * fTemp21)) + (4.879109635569025e-18 * fTemp27))) + (2.607993801258108e-18 * fTemp99))))));
			fRec136[0] = max((fRec136[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp100))))));
			fVbargraph32 = FAUSTFLOAT(fRec136[0]);
			output2[i] = FAUSTFLOAT(fTemp100);
			double fTemp101 = (fRec1[0] * (((2.4969639621850082e-18 * fTemp76) + ((1.8092222748840437e-17 * fTemp79) + ((9.7582192711380499e-18 * fTemp93) + ((2.6935244158092871e-18 * fTemp84) + ((5.2159876025162161e-18 * fTemp99) + (fTemp73 + (fTemp67 + (fTemp66 + (fTemp71 + (fTemp1 + fTemp64)))))))))) - ((1.3800206037853928e-17 * fTemp91) + ((6.0229036928049765e-18 * fTemp82) + ((8.0910879860950644e-18 * fTemp87) + ((2.5195613817171963e-18 * fTemp96) + (fTemp69 + (fTemp68 + (fTemp72 + (fTemp65 + (fTemp63 + fTemp70)))))))))));
			fRec197[0] = max((fRec197[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp101))))));
			fVbargraph33 = FAUSTFLOAT(fRec197[0]);
			output3[i] = FAUSTFLOAT(fTemp101);
			double fTemp102 = (fRec1[0] * ((fTemp94 + ((9.0343555392074636e-18 * fTemp82) + (fTemp97 + (fTemp67 + (fTemp72 + (fTemp66 + ((fTemp1 + (3.7793420725757941e-18 * fTemp21)) + (1.4637328906707076e-17 * fTemp27)))))))) - (fTemp77 + (fTemp80 + ((2.0700309056780893e-17 * fTemp91) + (fTemp85 + (fTemp88 + ((7.823981403774323e-18 * fTemp99) + ((((fTemp64 + (fTemp70 + (4.0402866237139308e-18 * fTemp6))) + (3.745445943277512e-18 * fTemp48)) + (1.2136631979142595e-17 * fTemp54)) + (7.962406901797708e-17 * fTemp60))))))))));
			fRec198[0] = max((fRec198[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp102))))));
			fVbargraph34 = FAUSTFLOAT(fRec198[0]);
			output4[i] = FAUSTFLOAT(fTemp102);
			double fTemp103 = (fRec1[0] * ((fTemp61 + (fTemp55 + (fTemp49 + (fTemp43 + (fTemp37 + (fTemp31 + (fTemp28 + (fTemp22 + (fTemp16 + (fTemp10 + fTemp89)))))))))) - (fTemp58 + (fTemp52 + (fTemp46 + (fTemp40 + (fTemp34 + (fTemp25 + (fTemp19 + (fTemp4 + fTemp13))))))))));
			fRec199[0] = max((fRec199[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp103))))));
			fVbargraph35 = FAUSTFLOAT(fRec199[0]);
			output5[i] = FAUSTFLOAT(fTemp103);
			double fTemp104 = (0.022574955908289243 * fTemp0);
			double fTemp105 = (0.027648561470568499 * fTemp3);
			double fTemp106 = (fTemp104 + fTemp105);
			double fTemp107 = (0.027648561470568506 * fTemp6);
			double fTemp108 = (0.012619783999998805 * fTemp9);
			double fTemp109 = (0.043716214137085485 * fTemp12);
			double fTemp110 = (0.021858107068542749 * fTemp15);
			double fTemp111 = (0.038794291597509752 * fTemp21);
			double fTemp112 = (0.04089277388695433 * fTemp24);
			double fTemp113 = (0.016694405031674409 * fTemp27);
			double fTemp114 = (0.013385302265262959 * fTemp33);
			double fTemp115 = (0.047324189999995547 * fTemp36);
			double fTemp116 = (0.035414181017315374 * fTemp39);
			double fTemp117 = (0.012520803773755805 * fTemp42);
			double fTemp118 = (0.033906496904967637 * fTemp51);
			double fTemp119 = (0.048447942889749587 * fTemp54);
			double fTemp120 = (0.029363887673040441 * fTemp57);
			double fTemp121 = (0.0092856766004149483 * fTemp60);
			double fTemp122 = (0.010558468816293319 * fTemp18);
			double fTemp123 = (0.027513227513227514 * fTemp30);
			double fTemp124 = (0.028125949882539081 * fTemp45);
			double fTemp125 = (0.01922317685141265 * fTemp48);
			double fTemp126 = (fRec1[0] * ((((((((((((((((fTemp106 + fTemp107) + fTemp108) + fTemp109) + fTemp110) + fTemp111) + fTemp112) + fTemp113) + fTemp114) + fTemp115) + fTemp116) + fTemp117) + fTemp118) + fTemp119) + fTemp120) + fTemp121) - (((fTemp122 + fTemp123) + fTemp124) + fTemp125)));
			fRec200[0] = max((fRec200[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp126))))));
			fVbargraph36 = FAUSTFLOAT(fRec200[0]);
			output6[i] = FAUSTFLOAT(fTemp126);
			double fTemp127 = (fRec3[0] * double(input5[i]));
			fRec204[0] = (fRec204[1] + fRec203[1]);
			fRec205[0] = (fRec205[1] + fRec204[1]);
			fRec203[0] = (fSlow11 * ((fSlow3 * fTemp127) - ((fSlow13 * fRec204[0]) + (fSlow12 * fRec205[0]))));
			double fTemp128 = (fRec203[0] + (fSlow4 * fTemp127));
			fRec202[0] = max((fRec202[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp128))))));
			fVbargraph37 = FAUSTFLOAT(fRec202[0]);
			double fTemp129 = (0.043716214137085485 * fTemp128);
			double fTemp130 = (fRec3[0] * double(input10[i]));
			fRec209[0] = (fRec209[1] + fRec208[1]);
			fRec210[0] = (fRec210[1] + fRec209[1]);
			fRec208[0] = (fSlow15 * ((fSlow3 * fTemp130) - ((fSlow17 * fRec209[0]) + (fSlow16 * fRec210[0]))));
			fRec211[0] = (fRec211[1] + fRec207[1]);
			fRec207[0] = (fSlow14 * (fRec208[0] - (fSlow18 * fRec211[0])));
			double fTemp131 = (fRec207[0] + (fSlow4 * fTemp130));
			fRec206[0] = max((fRec206[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp131))))));
			fVbargraph38 = FAUSTFLOAT(fRec206[0]);
			double fTemp132 = (5.0079204648915091e-18 * fTemp131);
			double fTemp133 = (0.0092856766004149483 * fTemp79);
			double fTemp134 = (2.6768460856911014e-18 * fTemp82);
			double fTemp135 = (0.027648561470568506 * fTemp84);
			double fTemp136 = (fRec3[0] * double(input28[i]));
			fRec216[0] = (fRec216[1] + fRec215[1]);
			fRec217[0] = (fRec217[1] + fRec216[1]);
			fRec215[0] = (fSlow27 * ((fSlow3 * fTemp136) - ((fSlow29 * fRec216[0]) + (fSlow28 * fRec217[0]))));
			fRec218[0] = (fRec218[1] + fRec214[1]);
			fRec219[0] = (fRec219[1] + fRec218[1]);
			fRec214[0] = (fSlow26 * (fRec215[0] - ((fSlow31 * fRec218[0]) + (fSlow30 * fRec219[0]))));
			fRec220[0] = (fRec220[1] + fRec213[1]);
			fRec213[0] = (fSlow25 * (fRec214[0] - (fSlow32 * fRec220[0])));
			double fTemp137 = (fRec213[0] + (fSlow4 * fTemp136));
			fRec212[0] = max((fRec212[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp137))))));
			fVbargraph39 = FAUSTFLOAT(fRec212[0]);
			double fTemp138 = (4.1523482904968253e-18 * fTemp137);
			double fTemp139 = (fRec3[0] * double(input19[i]));
			fRec224[0] = (fRec224[1] + fRec223[1]);
			fRec225[0] = (fRec225[1] + fRec224[1]);
			fRec223[0] = (fSlow20 * ((fSlow3 * fTemp139) - ((fSlow22 * fRec224[0]) + (fSlow21 * fRec225[0]))));
			fRec226[0] = (fRec226[1] + fRec222[1]);
			fRec227[0] = (fRec227[1] + fRec226[1]);
			fRec222[0] = (fSlow19 * (fRec223[0] - ((fSlow24 * fRec226[0]) + (fSlow23 * fRec227[0]))));
			double fTemp140 = (fRec222[0] + (fSlow4 * fTemp139));
			fRec221[0] = max((fRec221[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp140))))));
			fVbargraph40 = FAUSTFLOAT(fRec221[0]);
			double fTemp141 = (0.013385302265262959 * fTemp140);
			double fTemp142 = (5.7955417805735729e-18 * fTemp99);
			double fTemp143 = (0.038794291597509752 * fTemp96);
			double fTemp144 = (1.6929861152980278e-18 * fTemp6);
			double fTemp145 = (2.6768460856911007e-18 * fTemp12);
			double fTemp146 = (2.3754652515039685e-18 * fTemp21);
			double fTemp147 = (8.1961337873870493e-19 * fTemp33);
			double fTemp148 = (2.8429185316539108e-18 * fTemp60);
			double fTemp149 = (3.0667124528564294e-18 * (fTemp27 + fTemp91));
			double fTemp150 = (0.01922317685141265 * fTemp76);
			double fTemp151 = (fRec3[0] * double(input26[i]));
			fRec232[0] = (fRec232[1] + fRec231[1]);
			fRec233[0] = (fRec233[1] + fRec232[1]);
			fRec231[0] = (fSlow27 * ((fSlow3 * fTemp151) - ((fSlow29 * fRec232[0]) + (fSlow28 * fRec233[0]))));
			fRec234[0] = (fRec234[1] + fRec230[1]);
			fRec235[0] = (fRec235[1] + fRec234[1]);
			fRec230[0] = (fSlow26 * (fRec231[0] - ((fSlow31 * fRec234[0]) + (fSlow30 * fRec235[0]))));
			fRec236[0] = (fRec236[1] + fRec229[1]);
			fRec229[0] = (fSlow25 * (fRec230[0] - (fSlow32 * fRec236[0])));
			double fTemp152 = (fRec229[0] + (fSlow4 * fTemp151));
			fRec228[0] = max((fRec228[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp152))))));
			fVbargraph41 = FAUSTFLOAT(fRec228[0]);
			double fTemp153 = (7.1920782098622802e-18 * fTemp152);
			double fTemp154 = (fRec3[0] * double(input17[i]));
			fRec240[0] = (fRec240[1] + fRec239[1]);
			fRec241[0] = (fRec241[1] + fRec240[1]);
			fRec239[0] = (fSlow20 * ((fSlow3 * fTemp154) - ((fSlow22 * fRec240[0]) + (fSlow21 * fRec241[0]))));
			fRec242[0] = (fRec242[1] + fRec238[1]);
			fRec243[0] = (fRec243[1] + fRec242[1]);
			fRec238[0] = (fSlow19 * (fRec239[0] - ((fSlow24 * fRec242[0]) + (fSlow23 * fRec243[0]))));
			double fTemp155 = (fRec238[0] + (fSlow4 * fTemp154));
			fRec237[0] = max((fRec237[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp155))))));
			fVbargraph42 = FAUSTFLOAT(fRec237[0]);
			double fTemp156 = (0.035414181017315374 * fTemp155);
			double fTemp157 = (0.016694405031674409 * fTemp93);
			double fTemp158 = (0.048447942889749587 * fTemp87);
			double fTemp159 = (6.5054795140920346e-18 * fTemp39);
			double fTemp160 = (1.1770801000262998e-18 * fTemp48);
			double fTemp161 = (8.8997427277808398e-18 * fTemp54);
			double fTemp162 = (fRec1[0] * ((fTemp129 + (fTemp132 + (fTemp133 + (fTemp134 + (fTemp135 + (fTemp138 + (fTemp141 + (fTemp142 + (fTemp143 + ((fTemp120 + (fTemp117 + ((((fTemp108 + (fTemp106 + fTemp144)) + fTemp145) + fTemp146) + fTemp147))) + fTemp148)))))))))) - (fTemp149 + (fTemp150 + (fTemp153 + (fTemp156 + (fTemp157 + (fTemp158 + ((fTemp118 + ((fTemp124 + ((fTemp115 + (fTemp123 + (fTemp112 + (fTemp110 + fTemp122)))) + fTemp159)) + fTemp160)) + fTemp161)))))))));
			fRec201[0] = max((fRec201[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp162))))));
			fVbargraph43 = FAUSTFLOAT(fRec201[0]);
			output7[i] = FAUSTFLOAT(fTemp162);
			double fTemp163 = (6.1334249057128588e-18 * (fTemp93 - fTemp91));
			double fTemp164 = (5.3536921713822013e-18 * fTemp128);
			double fTemp165 = (5.6858370633078216e-18 * fTemp79);
			double fTemp166 = (1.3010959028184069e-17 * fTemp155);
			double fTemp167 = (3.3859722305960555e-18 * fTemp84);
			double fTemp168 = (1.779948545556168e-17 * fTemp87);
			double fTemp169 = (1.6392267574774099e-18 * fTemp140);
			double fTemp170 = (4.7509305030079371e-18 * fTemp96);
			double fTemp171 = (fTemp106 + fTemp108);
			double fTemp172 = (1.0015840929783018e-17 * fTemp131);
			double fTemp173 = (2.3541602000525996e-18 * fTemp76);
			double fTemp174 = (1.438415641972456e-17 * fTemp152);
			double fTemp175 = (5.3536921713822029e-18 * fTemp82);
			double fTemp176 = (8.3046965809936507e-18 * fTemp137);
			double fTemp177 = (1.1591083561147146e-17 * fTemp99);
			double fTemp178 = (fRec1[0] * ((fTemp163 + (fTemp164 + (fTemp165 + (fTemp166 + (fTemp167 + (fTemp168 + (fTemp169 + (fTemp170 + (fTemp120 + (fTemp118 + (fTemp125 + (fTemp117 + (fTemp115 + (fTemp112 + (fTemp110 + fTemp171))))))))))))))) - (fTemp172 + (fTemp173 + (fTemp174 + (fTemp175 + (fTemp176 + (fTemp177 + (fTemp121 + (fTemp119 + (fTemp124 + (fTemp116 + (fTemp114 + (fTemp123 + (fTemp113 + (fTemp111 + (fTemp122 + (fTemp107 + fTemp109))))))))))))))))));
			fRec244[0] = max((fRec244[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp178))))));
			fVbargraph44 = FAUSTFLOAT(fRec244[0]);
			output8[i] = FAUSTFLOAT(fTemp178);
			double fTemp179 = (1.5023761394674526e-17 * fTemp131);
			double fTemp180 = (8.0305382570733035e-18 * fTemp82);
			double fTemp181 = (1.2457044871490476e-17 * fTemp137);
			double fTemp182 = (1.7386625341720719e-17 * fTemp99);
			double fTemp183 = (9.2001373585692901e-18 * fTemp27);
			double fTemp184 = (1.9516438542276106e-17 * fTemp39);
			double fTemp185 = (3.5312403000788995e-18 * fTemp48);
			double fTemp186 = (2.6699228183342524e-17 * fTemp54);
			double fTemp187 = (2.1576234629586837e-17 * fTemp152);
			double fTemp188 = (9.200137358569287e-18 * fTemp91);
			double fTemp189 = (5.0789583458940831e-18 * fTemp6);
			double fTemp190 = (8.0305382570733005e-18 * fTemp12);
			double fTemp191 = (7.1263957545119048e-18 * fTemp21);
			double fTemp192 = (2.4588401362161147e-18 * fTemp33);
			double fTemp193 = (2.5023430732567692e-17 * fTemp60);
			double fTemp194 = (fRec1[0] * ((fTemp179 + (fTemp150 + (fTemp156 + (fTemp157 + (fTemp180 + (fTemp181 + (fTemp158 + (fTemp182 + (fTemp120 + (((fTemp117 + ((fTemp171 + fTemp183) + fTemp184)) + fTemp185) + fTemp186)))))))))) - (fTemp129 + (fTemp187 + (fTemp133 + (fTemp188 + (fTemp135 + (fTemp141 + (fTemp143 + ((fTemp118 + (fTemp124 + (fTemp115 + ((fTemp123 + (fTemp112 + ((fTemp122 + (fTemp110 + (fTemp189 + fTemp190))) + fTemp191))) + fTemp192)))) + fTemp193))))))))));
			fRec245[0] = max((fRec245[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp194))))));
			fVbargraph45 = FAUSTFLOAT(fRec245[0]);
			output9[i] = FAUSTFLOAT(fTemp194);
			double fTemp195 = (0.025630902468550196 * fTemp76);
			double fTemp196 = (0.043716214137085485 * fTemp82);
			double fTemp197 = (0.027648561470568499 * fTemp84);
			double fTemp198 = (fTemp104 + fTemp107);
			double fTemp199 = (0.025396825396825397 * fTemp30);
			double fTemp200 = (0.025630902468550199 * fTemp48);
			double fTemp201 = (0.037859351999996446 * fTemp99);
			double fTemp202 = (0.025862861065006505 * fTemp96);
			double fTemp203 = (0.025239567999997631 * fTemp9);
			double fTemp204 = (0.025862861065006511 * fTemp21);
			double fTemp205 = (0.050083215095023212 * fTemp42);
			double fTemp206 = (fRec1[0] * ((fTemp195 + ((6.133424905712858e-18 * fTemp91) + (fTemp196 + (fTemp197 + ((((fTemp198 + (2.6768460856911007e-18 * fTemp15)) + fTemp199) + fTemp200) + (0.027684538794142621 * fTemp54)))))) - ((0.033388810063348803 * (fTemp27 - fTemp93)) + ((0.037142706401659779 * fTemp79) + ((0.027684538794142628 * fTemp87) + (fTemp201 + (fTemp202 + ((((fTemp203 + fTemp204) + (2.3182167122294292e-18 * fTemp36)) + fTemp205) + (0.037142706401659786 * fTemp60)))))))));
			fRec246[0] = max((fRec246[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp206))))));
			fVbargraph46 = FAUSTFLOAT(fRec246[0]);
			output10[i] = FAUSTFLOAT(fTemp206);
			double fTemp207 = (0.025630902468550199 * fTemp76);
			double fTemp208 = (0.025862861065006511 * fTemp96);
			double fTemp209 = (fRec1[0] * ((fTemp207 + ((1.8400274717138574e-17 * fTemp91) + ((0.033388810063348796 * fTemp93) + (fTemp135 + (fTemp201 + (((fTemp199 + ((fTemp104 + (0.025862861065006505 * fTemp21)) + (0.033388810063348817 * fTemp27))) + (6.9546501366882881e-18 * fTemp36)) + (0.037142706401659731 * fTemp60))))))) - ((0.037142706401659828 * fTemp79) + (fTemp196 + ((0.027684538794142618 * fTemp87) + (fTemp208 + (((fTemp205 + ((fTemp203 + (0.027648561470568499 * fTemp6)) + (8.0305382570733005e-18 * fTemp15))) + (0.025630902468550196 * fTemp48)) + (0.027684538794142632 * fTemp54))))))));
			fRec247[0] = max((fRec247[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp209))))));
			fVbargraph47 = FAUSTFLOAT(fRec247[0]);
			output11[i] = FAUSTFLOAT(fTemp209);
			double fTemp210 = (fRec1[0] * (((0.037142706401659786 * fTemp79) + ((3.0667124528564293e-17 * fTemp91) + (fTemp196 + ((0.027684538794142663 * fTemp87) + (fTemp202 + ((fTemp199 + ((fTemp204 + (fTemp104 + (1.3384230428455503e-17 * fTemp15))) + (0.033388810063348762 * fTemp27))) + (0.037142706401659779 * fTemp60))))))) - (fTemp195 + ((0.033388810063348852 * fTemp93) + (fTemp197 + (fTemp201 + ((fTemp200 + (fTemp205 + ((fTemp107 + fTemp203) + (1.1591083561147147e-17 * fTemp36)))) + (0.027684538794142587 * fTemp54))))))));
			fRec248[0] = max((fRec248[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp210))))));
			fVbargraph48 = FAUSTFLOAT(fRec248[0]);
			output12[i] = FAUSTFLOAT(fTemp210);
			double fTemp211 = (fRec1[0] * (((0.037142706401659856 * fTemp79) + ((4.2933974339990009e-17 * fTemp91) + ((0.027684538794142632 * fTemp87) + (fTemp201 + (fTemp208 + ((((fTemp199 + (fTemp104 + (0.027648561470568496 * fTemp6))) + (1.6227516985606004e-17 * fTemp36)) + (0.025630902468550189 * fTemp48)) + (0.027684538794142618 * fTemp54))))))) - (fTemp207 + ((0.033388810063348817 * fTemp93) + (fTemp196 + (fTemp135 + ((fTemp205 + (((fTemp203 + (1.8737922599837705e-17 * fTemp15)) + (0.025862861065006501 * fTemp21)) + (0.033388810063348796 * fTemp27))) + (0.03714270640165971 * fTemp60))))))));
			fRec249[0] = max((fRec249[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp211))))));
			fVbargraph49 = FAUSTFLOAT(fRec249[0]);
			output13[i] = FAUSTFLOAT(fTemp211);
			double fTemp212 = (fRec1[0] * ((fTemp121 + (fTemp119 + (fTemp124 + (fTemp117 + (fTemp115 + (fTemp113 + (fTemp111 + (fTemp122 + (fTemp110 + (fTemp108 + fTemp198)))))))))) - (fTemp120 + (fTemp118 + (fTemp125 + (fTemp116 + (fTemp114 + (fTemp123 + (fTemp112 + (fTemp105 + fTemp109))))))))));
			fRec250[0] = max((fRec250[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp212))))));
			fVbargraph50 = FAUSTFLOAT(fRec250[0]);
			output14[i] = FAUSTFLOAT(fTemp212);
			double fTemp213 = (fRec1[0] * ((fTemp153 + (fTemp133 + (fTemp156 + (fTemp134 + (fTemp135 + (fTemp142 + (fTemp143 + (fTemp148 + (fTemp118 + (fTemp124 + (fTemp117 + (fTemp159 + (fTemp112 + (fTemp146 + (fTemp122 + (fTemp108 + (fTemp104 + fTemp144))))))))))))))))) - (fTemp149 + (fTemp129 + (fTemp132 + (fTemp150 + (fTemp157 + (fTemp138 + (fTemp158 + (fTemp141 + (fTemp120 + (fTemp161 + (fTemp160 + (fTemp115 + (fTemp147 + (fTemp123 + (fTemp110 + (fTemp105 + fTemp145))))))))))))))))));
			fRec251[0] = max((fRec251[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp213))))));
			fVbargraph51 = FAUSTFLOAT(fRec251[0]);
			output15[i] = FAUSTFLOAT(fTemp213);
			double fTemp214 = (fTemp104 + fTemp108);
			double fTemp215 = (fRec1[0] * ((fTemp163 + (fTemp172 + (fTemp174 + (fTemp165 + (fTemp167 + (fTemp176 + (fTemp168 + (fTemp170 + (fTemp125 + (fTemp124 + (fTemp117 + (fTemp116 + (fTemp115 + (fTemp114 + (fTemp122 + (fTemp110 + (fTemp109 + fTemp214))))))))))))))))) - (fTemp164 + (fTemp173 + (fTemp166 + (fTemp175 + (fTemp169 + (fTemp177 + (fTemp121 + (fTemp120 + (fTemp119 + (fTemp118 + (fTemp123 + (fTemp113 + (fTemp112 + (fTemp111 + (fTemp105 + fTemp107)))))))))))))))));
			fRec252[0] = max((fRec252[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp215))))));
			fVbargraph52 = FAUSTFLOAT(fRec252[0]);
			output16[i] = FAUSTFLOAT(fTemp215);
			double fTemp216 = (fRec1[0] * ((fTemp129 + (fTemp150 + (fTemp187 + (fTemp157 + (fTemp180 + (fTemp158 + (fTemp141 + (fTemp182 + (fTemp186 + (fTemp118 + (fTemp185 + (fTemp124 + (fTemp117 + (fTemp192 + (fTemp183 + (fTemp112 + (fTemp122 + (fTemp190 + fTemp214)))))))))))))))))) - (fTemp179 + (fTemp133 + (fTemp156 + (fTemp188 + (fTemp135 + (fTemp181 + (fTemp143 + (fTemp193 + (fTemp120 + (fTemp184 + (fTemp115 + (fTemp123 + (fTemp191 + (fTemp110 + (fTemp105 + fTemp189)))))))))))))))));
			fRec253[0] = max((fRec253[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp216))))));
			fVbargraph53 = FAUSTFLOAT(fRec253[0]);
			output17[i] = FAUSTFLOAT(fTemp216);
			double fTemp217 = (fTemp0 + fTemp3);
			double fTemp218 = (0.021093750000000001 * fTemp217);
			double fTemp219 = (0.027231914153020904 * fTemp128);
			double fTemp220 = (0.041597435974919175 * fTemp131);
			double fTemp221 = (4.8773411440759829e-18 * fTemp152);
			double fTemp222 = (0.029413829057838591 * fTemp155);
			double fTemp223 = (2.5471083409710855e-18 * fTemp91);
			double fTemp224 = (0.016982082124440745 * fTemp93);
			double fTemp225 = (0.027231914153020897 * fTemp82);
			double fTemp226 = (0.021093749999999994 * fTemp84);
			double fTemp227 = (1.5316979614614479e-17 * fTemp137);
			double fTemp228 = (0.028161597282885743 * fTemp87);
			double fTemp229 = (0.031444705933590805 * fTemp99);
			double fTemp230 = (0.013154264250377145 * fTemp96);
			double fTemp231 = (5.236595479505967e-18 * fTemp9);
			double fTemp232 = ((0.021093749999999998 * fTemp6) + fTemp231);
			double fTemp233 = (0.027231914153020911 * fTemp12);
			double fTemp234 = (1.6674738251076273e-18 * fTemp15);
			double fTemp235 = (0.013154264250377147 * fTemp21);
			double fTemp236 = (2.5471083409710867e-18 * fTemp24);
			double fTemp237 = (1.9254329235850881e-18 * fTemp36);
			double fTemp238 = (9.3789450288214403e-34 * fTemp51);
			double fTemp239 = (0.01111738239902945 * (fTemp33 + fTemp140));
			double fTemp240 = (0.026072572806195678 * fTemp76);
			double fTemp241 = (0.012594249175901169 * fTemp79);
			double fTemp242 = (0.021480823570105497 * fTemp18);
			double fTemp243 = (0.016982082124440741 * fTemp27);
			double fTemp244 = (0.024609375000000013 * fTemp30);
			double fTemp245 = (0.029413829057838584 * fTemp39);
			double fTemp246 = (0.020798717987459581 * fTemp42);
			double fTemp247 = (0.0067319093514117785 * fTemp45);
			double fTemp248 = (0.026072572806195685 * fTemp48);
			double fTemp249 = (0.028161597282885739 * fTemp54);
			double fTemp250 = (0.039826512815546306 * fTemp57);
			double fTemp251 = (0.012594249175901174 * fTemp60);
			double fTemp252 = (fRec1[0] * ((fTemp218 + (fTemp219 + (fTemp220 + (fTemp221 + (fTemp222 + (fTemp223 + (fTemp224 + (fTemp225 + (fTemp226 + (fTemp227 + (fTemp228 + (fTemp229 + (fTemp230 + ((((((fTemp232 + fTemp233) + fTemp234) + fTemp235) + fTemp236) + fTemp237) + fTemp238)))))))))))))) - (fTemp239 + (fTemp240 + (fTemp241 + (((((((((fTemp242 + fTemp243) + fTemp244) + fTemp245) + fTemp246) + fTemp247) + fTemp248) + fTemp249) + fTemp250) + fTemp251))))));
			fRec254[0] = max((fRec254[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp252))))));
			fVbargraph54 = FAUSTFLOAT(fRec254[0]);
			output18[i] = FAUSTFLOAT(fTemp252);
			double fTemp253 = (0.01111738239902945 * (fTemp33 - fTemp140));
			double fTemp254 = (0.027231914153020911 * fTemp128);
			double fTemp255 = (1.4632023432227946e-17 * fTemp152);
			double fTemp256 = (0.02941382905783858 * fTemp155);
			double fTemp257 = (7.6413250229132566e-18 * fTemp91);
			double fTemp258 = (0.016982082124440741 * fTemp93);
			double fTemp259 = (0.021093749999999998 * fTemp84);
			double fTemp260 = (0.028161597282885732 * fTemp87);
			double fTemp261 = (0.013154264250377147 * fTemp96);
			double fTemp262 = (0.016982082124440748 * fTemp27);
			double fTemp263 = (0.029413829057838594 * fTemp39);
			double fTemp264 = (0.026072572806195678 * fTemp48);
			double fTemp265 = (0.028161597282885746 * fTemp54);
			double fTemp266 = (0.012594249175901155 * fTemp60);
			double fTemp267 = (0.026072572806195685 * fTemp76);
			double fTemp268 = (0.012594249175901188 * fTemp79);
			double fTemp269 = (0.021093749999999994 * fTemp6);
			double fTemp270 = (0.027231914153020904 * fTemp12);
			double fTemp271 = (5.0024214753228816e-18 * fTemp15);
			double fTemp272 = (0.013154264250377145 * fTemp21);
			double fTemp273 = (7.6413250229132581e-18 * fTemp24);
			double fTemp274 = (5.7762987707552641e-18 * fTemp36);
			double fTemp275 = (2.8136835086464319e-33 * fTemp51);
			double fTemp276 = (fRec1[0] * ((fTemp253 + (fTemp218 + (fTemp254 + (fTemp255 + (fTemp256 + (fTemp257 + (fTemp258 + (fTemp259 + (fTemp260 + (fTemp261 + (((((fTemp231 + fTemp262) + fTemp263) + fTemp264) + fTemp265) + fTemp266))))))))))) - (fTemp220 + (fTemp267 + (fTemp268 + (fTemp225 + (fTemp227 + (fTemp229 + (fTemp250 + ((fTemp247 + (fTemp246 + ((fTemp244 + (((fTemp242 + ((fTemp269 + fTemp270) + fTemp271)) + fTemp272) + fTemp273)) + fTemp274))) + fTemp275))))))))));
			fRec255[0] = max((fRec255[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp276))))));
			fVbargraph55 = FAUSTFLOAT(fRec255[0]);
			output19[i] = FAUSTFLOAT(fTemp276);
			double fTemp277 = (2.4386705720379917e-17 * fTemp152);
			double fTemp278 = (0.012594249175901172 * fTemp79);
			double fTemp279 = (1.2735541704855428e-17 * fTemp91);
			double fTemp280 = (0.01111738239902945 * fTemp140);
			double fTemp281 = (8.3373691255381378e-18 * fTemp15);
			double fTemp282 = (1.2735541704855431e-17 * fTemp24);
			double fTemp283 = (0.016982082124440721 * fTemp27);
			double fTemp284 = (0.011117382399029452 * fTemp33);
			double fTemp285 = (9.6271646179254396e-18 * fTemp36);
			double fTemp286 = (0.029413829057838549 * fTemp39);
			double fTemp287 = (0.026072572806195689 * fTemp48);
			double fTemp288 = (4.6894725144107197e-33 * fTemp51);
			double fTemp289 = (0.028161597282885704 * fTemp54);
			double fTemp290 = (0.012594249175901169 * fTemp60);
			double fTemp291 = (0.029413829057838629 * fTemp155);
			double fTemp292 = (0.016982082124440766 * fTemp93);
			double fTemp293 = (0.028161597282885781 * fTemp87);
			double fTemp294 = (0.013154264250377148 * fTemp21);
			double fTemp295 = (fRec1[0] * ((fTemp220 + (fTemp240 + (fTemp277 + (fTemp278 + (fTemp279 + (fTemp225 + (fTemp227 + (fTemp280 + (fTemp229 + ((((((((((fTemp231 + fTemp281) + fTemp282) + fTemp283) + fTemp284) + fTemp285) + fTemp286) + fTemp287) + fTemp288) + fTemp289) + fTemp290)))))))))) - ((0.021093750000000001 * (fTemp6 - fTemp217)) + (fTemp219 + (fTemp291 + (fTemp292 + (fTemp226 + (fTemp293 + (fTemp230 + (fTemp250 + (fTemp247 + (fTemp246 + (fTemp244 + ((fTemp233 + fTemp242) + fTemp294))))))))))))));
			fRec256[0] = max((fRec256[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp295))))));
			fVbargraph56 = FAUSTFLOAT(fRec256[0]);
			output20[i] = FAUSTFLOAT(fTemp295);
			double fTemp296 = (fTemp0 - fTemp84);
			double fTemp297 = (1.7829758386797597e-17 * fTemp91);
			double fTemp298 = (fTemp231 + (0.021093749999999991 * fTemp6));
			double fTemp299 = (0.027231914153020901 * fTemp12);
			double fTemp300 = (0.011117382399029452 * fTemp140);
			double fTemp301 = (0.012594249175901195 * fTemp79);
			double fTemp302 = (3.4141388008531875e-17 * fTemp152);
			double fTemp303 = (0.026072572806195689 * fTemp76);
			double fTemp304 = (0.029413829057838594 * fTemp155);
			double fTemp305 = (0.016982082124440748 * fTemp93);
			double fTemp306 = (0.028161597282885746 * fTemp87);
			double fTemp307 = (0.013154264250377148 * fTemp96);
			double fTemp308 = (1.167231677575339e-17 * fTemp15);
			double fTemp309 = (1.7829758386797603e-17 * fTemp24);
			double fTemp310 = (0.011117382399029449 * fTemp33);
			double fTemp311 = (1.3478030465095617e-17 * fTemp36);
			double fTemp312 = (0.02941382905783858 * fTemp39);
			double fTemp313 = (0.026072572806195675 * fTemp48);
			double fTemp314 = (6.5652615201750074e-33 * fTemp51);
			double fTemp315 = (0.028161597282885732 * fTemp54);
			double fTemp316 = (0.012594249175901146 * fTemp60);
			double fTemp317 = (fRec1[0] * (((0.021093750000000001 * (fTemp3 + fTemp296)) + ((((fTemp297 + ((fTemp272 + (fTemp298 + fTemp299)) + fTemp300)) + fTemp301) + fTemp302) + fTemp303)) - (fTemp254 + (fTemp220 + (fTemp304 + (fTemp305 + (fTemp225 + (fTemp227 + (fTemp306 + (fTemp229 + (fTemp307 + ((fTemp250 + ((((fTemp247 + (fTemp246 + ((((fTemp244 + (fTemp243 + ((fTemp242 + fTemp308) + fTemp309))) + fTemp310) + fTemp311) + fTemp312))) + fTemp313) + fTemp314) + fTemp315)) + fTemp316))))))))))));
			fRec257[0] = max((fRec257[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp317))))));
			fVbargraph57 = FAUSTFLOAT(fRec257[0]);
			output21[i] = FAUSTFLOAT(fTemp317);
			double fTemp318 = (fTemp0 - fTemp3);
			double fTemp319 = (0.021093750000000001 * fTemp318);
			double fTemp320 = (fRec1[0] * ((fTemp319 + (fTemp239 + (fTemp223 + (fTemp224 + (fTemp225 + (fTemp226 + (fTemp228 + (fTemp229 + (fTemp230 + (fTemp250 + (fTemp247 + (fTemp245 + (fTemp237 + (fTemp235 + (fTemp242 + (fTemp232 + fTemp234)))))))))))))))) - (fTemp219 + (fTemp220 + (fTemp240 + (fTemp221 + (fTemp241 + (fTemp222 + (fTemp227 + (fTemp251 + (fTemp249 + (fTemp238 + (fTemp248 + (fTemp246 + (fTemp244 + (fTemp243 + (fTemp233 + fTemp236)))))))))))))))));
			fRec258[0] = max((fRec258[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp320))))));
			fVbargraph58 = FAUSTFLOAT(fRec258[0]);
			output22[i] = FAUSTFLOAT(fTemp320);
			double fTemp321 = (fRec1[0] * ((fTemp319 + (fTemp220 + (fTemp257 + (fTemp258 + (fTemp259 + (fTemp227 + (fTemp260 + (fTemp261 + (fTemp266 + (fTemp250 + (fTemp265 + (fTemp275 + (fTemp264 + (fTemp247 + (fTemp262 + (fTemp273 + (fTemp242 + (fTemp231 + fTemp270)))))))))))))))))) - (fTemp253 + (fTemp254 + (fTemp267 + (fTemp255 + (fTemp268 + (fTemp256 + (fTemp225 + (fTemp229 + (fTemp246 + (fTemp263 + (fTemp274 + (fTemp244 + (fTemp272 + (fTemp269 + fTemp271))))))))))))))));
			fRec259[0] = max((fRec259[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp321))))));
			fVbargraph59 = FAUSTFLOAT(fRec259[0]);
			output23[i] = FAUSTFLOAT(fTemp321);
			double fTemp322 = (fRec1[0] * ((fTemp219 + (fTemp240 + (fTemp278 + (fTemp291 + (fTemp279 + (fTemp225 + (fTemp229 + (fTemp290 + (fTemp250 + (fTemp289 + (fTemp287 + (fTemp247 + (fTemp285 + (fTemp283 + (fTemp242 + (fTemp281 + (fTemp231 + fTemp233))))))))))))))))) - ((0.021093750000000001 * (fTemp6 - fTemp318)) + (fTemp220 + (fTemp277 + (fTemp292 + (fTemp226 + (fTemp227 + (fTemp293 + (fTemp280 + (fTemp230 + (fTemp288 + (fTemp246 + (fTemp286 + (fTemp284 + (fTemp244 + (fTemp294 + fTemp282)))))))))))))))));
			fRec260[0] = max((fRec260[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp322))))));
			fVbargraph60 = FAUSTFLOAT(fRec260[0]);
			output24[i] = FAUSTFLOAT(fTemp322);
			double fTemp323 = (fRec1[0] * ((fTemp254 + (fTemp220 + (fTemp303 + (fTemp301 + (fTemp304 + (fTemp297 + (fTemp227 + (fTemp250 + (fTemp314 + (fTemp247 + (fTemp312 + (fTemp310 + (fTemp309 + (fTemp272 + (fTemp242 + fTemp298))))))))))))))) - ((0.021093750000000001 * (fTemp3 - fTemp296)) + (fTemp302 + (fTemp305 + (fTemp225 + (fTemp306 + (fTemp300 + (fTemp229 + (fTemp307 + (fTemp316 + (fTemp315 + (fTemp313 + (fTemp246 + (fTemp311 + (fTemp244 + (fTemp243 + (fTemp299 + fTemp308))))))))))))))))));
			fRec261[0] = max((fRec261[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp323))))));
			fVbargraph61 = FAUSTFLOAT(fRec261[0]);
			output25[i] = FAUSTFLOAT(fTemp323);
			double fTemp324 = (0.021308452520676528 * (fTemp12 + fTemp128));
			double fTemp325 = (0.010535180889746065 * (fTemp6 + fTemp84));
			double fTemp326 = (0.016998264729033836 * fTemp131);
			double fTemp327 = (0.035191232193238609 * fTemp76);
			double fTemp328 = (0.0062770252486745223 * fTemp155);
			double fTemp329 = (0.0023131708388267431 * fTemp93);
			double fTemp330 = (0.0071028175068921654 * fTemp82);
			double fTemp331 = (0.041001357231518168 * fTemp137);
			double fTemp332 = (0.012205326872422687 * fTemp87);
			double fTemp333 = (0.035587388102713349 * fTemp140);
			double fTemp334 = (0.02907849643359095 * fTemp99);
			double fTemp335 = (0.030460165261409405 * fTemp96);
			double fTemp336 = (0.02017333553791887 * fTemp0);
			double fTemp337 = (0.031605542669238249 * fTemp3);
			double fTemp338 = (fTemp336 + fTemp337);
			double fTemp339 = (0.032806508796871948 * fTemp9);
			double fTemp340 = (fTemp338 + fTemp339);
			double fTemp341 = (0.0263335984839893 * fTemp18);
			double fTemp342 = (fTemp340 + fTemp341);
			double fTemp343 = (0.030460165261409402 * fTemp21);
			double fTemp344 = (0.014254712301587407 * fTemp30);
			double fTemp345 = (0.035587388102713342 * fTemp33);
			double fTemp346 = (0.035191232193238602 * fTemp48);
			double fTemp347 = (1.427539198259021e-18 * fTemp152);
			double fTemp348 = (0.00046786183841729075 * fTemp79);
			double fTemp349 = (4.7584639941967359e-19 * fTemp91);
			double fTemp350 = (1.1422201709353029e-18 * fTemp15);
			double fTemp351 = (2.7335294515958808e-18 * fTemp24);
			double fTemp352 = (0.0023131708388267449 * fTemp27);
			double fTemp353 = (4.6761788733398907e-18 * fTemp36);
			double fTemp354 = (0.0062770252486745275 * fTemp39);
			double fTemp355 = (0.0014795090396723079 * fTemp42);
			double fTemp356 = (0.00075024801587290956 * fTemp45);
			double fTemp357 = (6.5935211231487317e-18 * fTemp51);
			double fTemp358 = (0.012205326872422696 * fTemp54);
			double fTemp359 = (0.0044385271190169251 * fTemp57);
			double fTemp360 = (0.00046786183841729048 * fTemp60);
			double fTemp361 = (fRec1[0] * ((fTemp324 + (fTemp325 + (fTemp326 + (fTemp327 + (fTemp328 + (fTemp329 + (fTemp330 + (fTemp331 + (fTemp332 + (fTemp333 + (fTemp334 + (fTemp335 + ((((fTemp342 + fTemp343) + fTemp344) + fTemp345) + fTemp346))))))))))))) - (fTemp347 + (fTemp348 + (fTemp349 + ((((((((((fTemp350 + fTemp351) + fTemp352) + fTemp353) + fTemp354) + fTemp355) + fTemp356) + fTemp357) + fTemp358) + fTemp359) + fTemp360))))));
			fRec262[0] = max((fRec262[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp361))))));
			fVbargraph62 = FAUSTFLOAT(fRec262[0]);
			output26[i] = FAUSTFLOAT(fTemp361);
			double fTemp362 = (1.63068840876984e-18 * fTemp152);
			double fTemp363 = (0.0062770252486745231 * fTemp155);
			double fTemp364 = (5.4356280292327993e-19 * fTemp91);
			double fTemp365 = (0.012205326872422689 * fTemp87);
			double fTemp366 = (0.0023131708388267444 * fTemp27);
			double fTemp367 = (0.0062770252486745266 * fTemp39);
			double fTemp368 = (0.00046786183841728994 * fTemp60);
			double fTemp369 = (0.021308452520676528 * (fTemp12 - fTemp128));
			double fTemp370 = (0.010535180889746065 * (fTemp6 - fTemp84));
			double fTemp371 = (0.00046786183841729124 * fTemp79);
			double fTemp372 = (1.3047664087114909e-18 * fTemp15);
			double fTemp373 = (3.1225305737205953e-18 * fTemp24);
			double fTemp374 = (5.3416331372122328e-18 * fTemp36);
			double fTemp375 = (7.5318271341413852e-18 * fTemp51);
			double fTemp376 = (fRec1[0] * ((fTemp327 + (fTemp362 + (fTemp363 + (fTemp364 + (fTemp329 + (fTemp365 + (fTemp333 + (fTemp335 + ((fTemp358 + ((fTemp344 + (fTemp342 + fTemp366)) + fTemp367)) + fTemp368))))))))) - (fTemp369 + (fTemp370 + (fTemp326 + (fTemp371 + (fTemp330 + (fTemp331 + (fTemp334 + (fTemp359 + ((fTemp346 + (fTemp356 + (fTemp355 + ((fTemp345 + ((fTemp343 + fTemp372) + fTemp373)) + fTemp374)))) + fTemp375)))))))))));
			fRec263[0] = max((fRec263[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp376))))));
			fVbargraph63 = FAUSTFLOAT(fRec263[0]);
			output27[i] = FAUSTFLOAT(fTemp376);
			double fTemp377 = (2.7178140146164e-18 * fTemp152);
			double fTemp378 = (0.00046786183841729064 * fTemp79);
			double fTemp379 = (9.0593800487213328e-19 * fTemp91);
			double fTemp380 = (2.1746106811858183e-18 * fTemp15);
			double fTemp381 = (5.2042176228676594e-18 * fTemp24);
			double fTemp382 = (0.0023131708388267409 * fTemp27);
			double fTemp383 = (8.902721895353721e-18 * fTemp36);
			double fTemp384 = (0.0062770252486745171 * fTemp39);
			double fTemp385 = (1.2553045223568977e-17 * fTemp51);
			double fTemp386 = (0.012205326872422675 * fTemp54);
			double fTemp387 = (0.00046786183841729053 * fTemp60);
			double fTemp388 = (0.021308452520676528 * fTemp128);
			double fTemp389 = (0.035191232193238602 * fTemp76);
			double fTemp390 = (0.0062770252486745335 * fTemp155);
			double fTemp391 = (0.002313170838826747 * fTemp93);
			double fTemp392 = (0.010535180889746065 * fTemp84);
			double fTemp393 = (0.012205326872422709 * fTemp87);
			double fTemp394 = (0.035587388102713342 * fTemp140);
			double fTemp395 = (0.030460165261409402 * fTemp96);
			double fTemp396 = (0.010535180889746067 * fTemp6);
			double fTemp397 = (0.021308452520676532 * fTemp12);
			double fTemp398 = (0.030460165261409412 * fTemp21);
			double fTemp399 = (0.035587388102713349 * fTemp33);
			double fTemp400 = (0.035191232193238609 * fTemp48);
			double fTemp401 = (fRec1[0] * ((fTemp326 + (fTemp377 + (fTemp378 + (fTemp379 + (fTemp330 + (fTemp331 + (fTemp334 + ((((((fTemp344 + (((fTemp341 + (fTemp340 + fTemp380)) + fTemp381) + fTemp382)) + fTemp383) + fTemp384) + fTemp385) + fTemp386) + fTemp387)))))))) - (fTemp388 + (fTemp389 + (fTemp390 + (fTemp391 + (fTemp392 + (fTemp393 + (fTemp394 + (fTemp395 + (fTemp359 + ((fTemp356 + (fTemp355 + (((fTemp396 + fTemp397) + fTemp398) + fTemp399))) + fTemp400))))))))))));
			fRec264[0] = max((fRec264[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp401))))));
			fVbargraph64 = FAUSTFLOAT(fRec264[0]);
			output28[i] = FAUSTFLOAT(fTemp401);
			double fTemp402 = (3.8049396204629603e-18 * fTemp152);
			double fTemp403 = (0.00046786183841729151 * fTemp79);
			double fTemp404 = (1.2683132068209864e-18 * fTemp91);
			double fTemp405 = (0.010535180889746063 * fTemp6);
			double fTemp406 = (0.021308452520676525 * fTemp12);
			double fTemp407 = (0.030460165261409398 * fTemp21);
			double fTemp408 = (0.035587388102713335 * fTemp33);
			double fTemp409 = (0.035191232193238595 * fTemp48);
			double fTemp410 = (0.021308452520676532 * fTemp128);
			double fTemp411 = (0.0062770252486745266 * fTemp155);
			double fTemp412 = (0.0023131708388267444 * fTemp93);
			double fTemp413 = (0.010535180889746067 * fTemp84);
			double fTemp414 = (0.012205326872422696 * fTemp87);
			double fTemp415 = (0.030460165261409412 * fTemp96);
			double fTemp416 = (3.0444549536601452e-18 * fTemp15);
			double fTemp417 = (7.2859046720147234e-18 * fTemp24);
			double fTemp418 = (0.0023131708388267431 * fTemp27);
			double fTemp419 = (1.246381065349521e-17 * fTemp36);
			double fTemp420 = (0.0062770252486745231 * fTemp39);
			double fTemp421 = (1.7574263312996565e-17 * fTemp51);
			double fTemp422 = (0.012205326872422689 * fTemp54);
			double fTemp423 = (0.00046786183841728967 * fTemp60);
			double fTemp424 = (fRec1[0] * ((fTemp402 + (fTemp403 + (fTemp404 + (((fTemp344 + ((fTemp341 + ((fTemp339 + (fTemp338 + fTemp405)) + fTemp406)) + fTemp407)) + fTemp408) + fTemp409)))) - (fTemp410 + (fTemp326 + (fTemp327 + (fTemp411 + (fTemp412 + (fTemp330 + (fTemp413 + (fTemp331 + (fTemp414 + (fTemp333 + (fTemp334 + (fTemp415 + ((fTemp359 + (((fTemp356 + (fTemp355 + ((((fTemp416 + fTemp417) + fTemp418) + fTemp419) + fTemp420))) + fTemp421) + fTemp422)) + fTemp423)))))))))))))));
			fRec265[0] = max((fRec265[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp424))))));
			fVbargraph65 = FAUSTFLOAT(fRec265[0]);
			output29[i] = FAUSTFLOAT(fTemp424);
			double fTemp425 = (0.0071028175068921914 * fTemp128);
			double fTemp426 = (0.016998264729033888 * fTemp131);
			double fTemp427 = (0.035508216952135595 * fTemp152);
			double fTemp428 = (0.036961085234966218 * fTemp79);
			double fTemp429 = (0.027200442744256398 * fTemp155);
			double fTemp430 = (0.035508216952135616 * fTemp91);
			double fTemp431 = (0.030071220904747824 * fTemp93);
			double fTemp432 = (0.021308452520676577 * fTemp82);
			double fTemp433 = (0.010535180889746094 * fTemp84);
			double fTemp434 = (0.010535180889746075 * fTemp3);
			double fTemp435 = (fTemp336 + fTemp434);
			double fTemp436 = (0.031605542669238242 * fTemp6);
			double fTemp437 = (0.021308452520676542 * fTemp12);
			double fTemp438 = (0.028411270027568724 * fTemp15);
			double fTemp439 = (0.022664352972045144 * fTemp24);
			double fTemp440 = (0.020818537549440738 * fTemp27);
			double fTemp441 = (0.0042514054232804356 * fTemp30);
			double fTemp442 = (0.018831075746023603 * fTemp39);
			double fTemp443 = (0.010356563277706152 * fTemp42);
			double fTemp444 = (0.023090966710758377 * fTemp45);
			double fTemp445 = (0.010356563277706147 * fTemp57);
			double fTemp446 = (0.00096856602366710964 * fTemp76);
			double fTemp447 = (0.020500678615759143 * fTemp137);
			double fTemp448 = (0.0045334071240427497 * fTemp87);
			double fTemp449 = (0.010280801007450534 * fTemp140);
			double fTemp450 = (0.006710422253905635 * fTemp99);
			double fTemp451 = (0.0053753232814252047 * fTemp96);
			double fTemp452 = (0.01640325439843596 * fTemp9);
			double fTemp453 = (0.020481687709769415 * fTemp18);
			double fTemp454 = (fTemp452 + fTemp453);
			double fTemp455 = (0.016125969844275591 * fTemp21);
			double fTemp456 = (0.030842403022351559 * fTemp33);
			double fTemp457 = (0.0089472296718741632 * fTemp36);
			double fTemp458 = (0.0029056980710013246 * fTemp48);
			double fTemp459 = (0.027334238154345478 * fTemp51);
			double fTemp460 = (0.0031385126243372789 * fTemp54);
			double fTemp461 = (0.0014035855152519602 * fTemp60);
			double fTemp462 = (fRec1[0] * ((fTemp425 + (fTemp426 + (fTemp427 + (fTemp428 + (fTemp429 + (fTemp430 + (fTemp431 + (fTemp432 + (fTemp433 + ((((((((((fTemp435 + fTemp436) + fTemp437) + fTemp438) + fTemp439) + fTemp440) + fTemp441) + fTemp442) + fTemp443) + fTemp444) + fTemp445)))))))))) - (fTemp446 + (fTemp447 + (fTemp448 + (fTemp449 + (fTemp450 + (fTemp451 + (((((((fTemp454 + fTemp455) + fTemp456) + fTemp457) + fTemp458) + fTemp459) + fTemp460) + fTemp461)))))))));
			fRec266[0] = max((fRec266[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp462))))));
			fVbargraph66 = FAUSTFLOAT(fRec266[0]);
			output30[i] = FAUSTFLOAT(fTemp462);
			double fTemp463 = (0.021308452520676546 * fTemp128);
			double fTemp464 = (0.016998264729033867 * fTemp131);
			double fTemp465 = (0.021308452520676556 * fTemp82);
			double fTemp466 = (0.031605542669238242 * fTemp84);
			double fTemp467 = (0.0031385126243372858 * fTemp87);
			double fTemp468 = (0.010535180889746081 * fTemp6);
			double fTemp469 = (0.0071028175068921827 * fTemp12);
			double fTemp470 = (0.0089472296718741701 * fTemp36);
			double fTemp471 = (0.010356563277706217 * fTemp42);
			double fTemp472 = (0.027334238154345495 * fTemp51);
			double fTemp473 = (0.0045334071240427463 * fTemp54);
			double fTemp474 = (0.010356563277706213 * fTemp57);
			double fTemp475 = (0.036961085234966218 * fTemp60);
			double fTemp476 = (0.0029056980710013254 * fTemp76);
			double fTemp477 = (0.035508216952135581 * fTemp152);
			double fTemp478 = (0.0014035855152518815 * fTemp79);
			double fTemp479 = (0.018831075746023641 * fTemp155);
			double fTemp480 = (0.035508216952135595 * fTemp91);
			double fTemp481 = (0.020818537549440783 * fTemp93);
			double fTemp482 = (0.020500678615759119 * fTemp137);
			double fTemp483 = (0.030842403022351562 * fTemp140);
			double fTemp484 = (0.0067104222539056272 * fTemp99);
			double fTemp485 = (0.016125969844275591 * fTemp96);
			double fTemp486 = (0.028411270027568741 * fTemp15);
			double fTemp487 = (0.0053753232814251978 * fTemp21);
			double fTemp488 = (0.022664352972045162 * fTemp24);
			double fTemp489 = (0.030071220904747797 * fTemp27);
			double fTemp490 = (0.010280801007450522 * fTemp33);
			double fTemp491 = (0.027200442744256374 * fTemp39);
			double fTemp492 = (0.00096856602366710855 * fTemp48);
			double fTemp493 = (fRec1[0] * ((fTemp463 + (fTemp464 + (fTemp465 + (fTemp466 + (fTemp467 + (((((fTemp444 + (((fTemp441 + ((fTemp435 + fTemp468) + fTemp469)) + fTemp470) + fTemp471)) + fTemp472) + fTemp473) + fTemp474) + fTemp475)))))) - (fTemp476 + (fTemp477 + (fTemp478 + (fTemp479 + (fTemp480 + (fTemp481 + (fTemp482 + (fTemp483 + (fTemp484 + (fTemp485 + (((((((fTemp453 + (fTemp452 + fTemp486)) + fTemp487) + fTemp488) + fTemp489) + fTemp490) + fTemp491) + fTemp492)))))))))))));
			fRec267[0] = max((fRec267[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp493))))));
			fVbargraph67 = FAUSTFLOAT(fRec267[0]);
			output31[i] = FAUSTFLOAT(fTemp493);
			double fTemp494 = (0.021308452520676553 * fTemp128);
			double fTemp495 = (0.035508216952135574 * fTemp152);
			double fTemp496 = (0.020500678615759112 * fTemp137);
			double fTemp497 = (0.0031385126243372867 * fTemp87);
			double fTemp498 = (0.0067104222539056254 * fTemp99);
			double fTemp499 = (0.0053753232814251969 * fTemp21);
			double fTemp500 = (0.03007122090474779 * fTemp27);
			double fTemp501 = (0.010280801007450517 * fTemp33);
			double fTemp502 = (0.0089472296718741719 * fTemp36);
			double fTemp503 = (0.027200442744256363 * fTemp39);
			double fTemp504 = (0.010356563277706232 * fTemp42);
			double fTemp505 = (0.00096856602366710812 * fTemp48);
			double fTemp506 = (0.027334238154345498 * fTemp51);
			double fTemp507 = (0.010356563277706229 * fTemp57);
			double fTemp508 = (0.016998264729033864 * fTemp131);
			double fTemp509 = (0.0029056980710013259 * fTemp76);
			double fTemp510 = (0.0014035855152518585 * fTemp79);
			double fTemp511 = (0.018831075746023648 * fTemp155);
			double fTemp512 = (0.02081853754944079 * fTemp93);
			double fTemp513 = (0.021308452520676546 * fTemp82);
			double fTemp514 = (0.030842403022351569 * fTemp140);
			double fTemp515 = (0.016125969844275594 * fTemp96);
			double fTemp516 = (0.010535180889746079 * fTemp6);
			double fTemp517 = (0.007102817506892181 * fTemp12);
			double fTemp518 = (0.028411270027568745 * fTemp15);
			double fTemp519 = (0.0045334071240427445 * fTemp54);
			double fTemp520 = (fRec1[0] * ((fTemp494 + (fTemp495 + (fTemp480 + (fTemp466 + (fTemp496 + (fTemp497 + (fTemp498 + ((((fTemp444 + (((((fTemp441 + ((fTemp435 + fTemp499) + fTemp500)) + fTemp501) + fTemp502) + fTemp503) + fTemp504)) + fTemp505) + fTemp506) + fTemp507)))))))) - (fTemp508 + (fTemp509 + (fTemp510 + (fTemp511 + (fTemp512 + (fTemp513 + (fTemp514 + (fTemp515 + (fTemp475 + ((fTemp488 + (fTemp453 + (((fTemp452 + fTemp516) + fTemp517) + fTemp518))) + fTemp519))))))))))));
			fRec268[0] = max((fRec268[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp520))))));
			fVbargraph68 = FAUSTFLOAT(fRec268[0]);
			output32[i] = FAUSTFLOAT(fTemp520);
			double fTemp521 = (0.0071028175068921931 * fTemp128);
			double fTemp522 = (0.027200442744256408 * fTemp155);
			double fTemp523 = (0.030071220904747838 * fTemp93);
			double fTemp524 = (0.010535180889746096 * fTemp84);
			double fTemp525 = (0.020500678615759146 * fTemp137);
			double fTemp526 = (0.0067104222539056367 * fTemp99);
			double fTemp527 = (0.028411270027568717 * fTemp15);
			double fTemp528 = (0.022664352972045138 * fTemp24);
			double fTemp529 = (0.010356563277706145 * fTemp42);
			double fTemp530 = (0.0031385126243372767 * fTemp54);
			double fTemp531 = (0.01035656327770614 * fTemp57);
			double fTemp532 = (0.0014035855152519582 * fTemp60);
			double fTemp533 = (0.00096856602366710996 * fTemp76);
			double fTemp534 = (0.021308452520676584 * fTemp82);
			double fTemp535 = (0.0045334071240427515 * fTemp87);
			double fTemp536 = (0.010280801007450538 * fTemp140);
			double fTemp537 = (0.0053753232814252056 * fTemp96);
			double fTemp538 = (fTemp436 + fTemp452);
			double fTemp539 = (0.020818537549440724 * fTemp27);
			double fTemp540 = (0.018831075746023586 * fTemp39);
			double fTemp541 = (0.027334238154345474 * fTemp51);
			double fTemp542 = (fRec1[0] * ((fTemp521 + (fTemp428 + (fTemp522 + (fTemp523 + (fTemp524 + (fTemp525 + (fTemp526 + ((((fTemp458 + (fTemp444 + ((fTemp456 + (fTemp441 + ((fTemp455 + (fTemp435 + fTemp527)) + fTemp528))) + fTemp529))) + fTemp530) + fTemp531) + fTemp532)))))))) - (fTemp426 + (fTemp533 + (fTemp427 + (fTemp430 + (fTemp534 + (fTemp535 + (fTemp536 + (fTemp537 + (((fTemp457 + ((fTemp453 + (fTemp437 + fTemp538)) + fTemp539)) + fTemp540) + fTemp541)))))))))));
			fRec269[0] = max((fRec269[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp542))))));
			fVbargraph69 = FAUSTFLOAT(fRec269[0]);
			output33[i] = FAUSTFLOAT(fTemp542);
			double fTemp543 = (0.016998264729033881 * fTemp131);
			double fTemp544 = (0.0009685660236671092 * fTemp76);
			double fTemp545 = (0.035508216952135609 * fTemp91);
			double fTemp546 = (0.02130845252067657 * fTemp82);
			double fTemp547 = (0.010280801007450529 * fTemp140);
			double fTemp548 = (0.0053753232814252021 * fTemp96);
			double fTemp549 = (0.028411270027568731 * fTemp15);
			double fTemp550 = (0.022664352972045151 * fTemp24);
			double fTemp551 = (0.030842403022351562 * fTemp33);
			double fTemp552 = (0.010356563277706178 * fTemp42);
			double fTemp553 = (0.0029056980710013254 * fTemp48);
			double fTemp554 = (0.0031385126243372802 * fTemp54);
			double fTemp555 = (0.010356563277706175 * fTemp57);
			double fTemp556 = (0.0014035855152519131 * fTemp60);
			double fTemp557 = (0.0071028175068921871 * fTemp128);
			double fTemp558 = (0.030071220904747821 * fTemp93);
			double fTemp559 = (0.010535180889746089 * fTemp84);
			double fTemp560 = (0.020500678615759133 * fTemp137);
			double fTemp561 = (0.0067104222539056315 * fTemp99);
			double fTemp562 = (0.021308452520676546 * fTemp12);
			double fTemp563 = (0.020818537549440744 * fTemp27);
			double fTemp564 = (0.0089472296718741667 * fTemp36);
			double fTemp565 = (0.01883107574602361 * fTemp39);
			double fTemp566 = (0.027334238154345481 * fTemp51);
			double fTemp567 = (fRec1[0] * ((fTemp543 + (fTemp544 + (fTemp427 + (fTemp545 + (fTemp546 + (fTemp448 + (fTemp547 + (fTemp548 + (((((fTemp444 + (((fTemp441 + ((fTemp455 + (fTemp435 + fTemp549)) + fTemp550)) + fTemp551) + fTemp552)) + fTemp553) + fTemp554) + fTemp555) + fTemp556))))))))) - (fTemp557 + (fTemp428 + (fTemp429 + (fTemp558 + (fTemp559 + (fTemp560 + (fTemp561 + (((((fTemp453 + (fTemp538 + fTemp562)) + fTemp563) + fTemp564) + fTemp565) + fTemp566))))))))));
			fRec270[0] = max((fRec270[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp567))))));
			fVbargraph70 = FAUSTFLOAT(fRec270[0]);
			output34[i] = FAUSTFLOAT(fTemp567);
			double fTemp568 = (0.016998264729033874 * fTemp131);
			double fTemp569 = (0.001403585515251904 * fTemp79);
			double fTemp570 = (0.018831075746023631 * fTemp155);
			double fTemp571 = (0.020818537549440772 * fTemp93);
			double fTemp572 = (0.021308452520676563 * fTemp82);
			double fTemp573 = (0.0053753232814252004 * fTemp21);
			double fTemp574 = (0.0300712209047478 * fTemp27);
			double fTemp575 = (0.010280801007450525 * fTemp33);
			double fTemp576 = (0.0089472296718741684 * fTemp36);
			double fTemp577 = (0.027200442744256377 * fTemp39);
			double fTemp578 = (0.010356563277706199 * fTemp42);
			double fTemp579 = (0.00096856602366710877 * fTemp48);
			double fTemp580 = (0.027334238154345485 * fTemp51);
			double fTemp581 = (0.010356563277706192 * fTemp57);
			double fTemp582 = (0.035508216952135588 * fTemp152);
			double fTemp583 = (0.020500678615759129 * fTemp137);
			double fTemp584 = (0.0031385126243372841 * fTemp87);
			double fTemp585 = (0.0067104222539056298 * fTemp99);
			double fTemp586 = (0.010535180889746086 * fTemp6);
			double fTemp587 = (0.0071028175068921853 * fTemp12);
			double fTemp588 = (0.028411270027568734 * fTemp15);
			double fTemp589 = (0.0045334071240427471 * fTemp54);
			double fTemp590 = (fRec1[0] * ((fTemp568 + (fTemp476 + (fTemp569 + (fTemp570 + (fTemp571 + (fTemp572 + (fTemp483 + (fTemp485 + ((((fTemp444 + (((((fTemp441 + ((fTemp435 + fTemp573) + fTemp574)) + fTemp575) + fTemp576) + fTemp577) + fTemp578)) + fTemp579) + fTemp580) + fTemp581))))))))) - (fTemp463 + (fTemp582 + (fTemp545 + (fTemp466 + (fTemp583 + (fTemp584 + (fTemp585 + (fTemp475 + ((fTemp550 + (fTemp453 + (((fTemp452 + fTemp586) + fTemp587) + fTemp588))) + fTemp589)))))))))));
			fRec271[0] = max((fRec271[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp590))))));
			fVbargraph71 = FAUSTFLOAT(fRec271[0]);
			output35[i] = FAUSTFLOAT(fTemp590);
			double fTemp591 = (0.035508216952135567 * fTemp152);
			double fTemp592 = (0.0014035855152518362 * fTemp79);
			double fTemp593 = (0.018831075746023662 * fTemp155);
			double fTemp594 = (0.035508216952135588 * fTemp91);
			double fTemp595 = (0.020818537549440803 * fTemp93);
			double fTemp596 = (0.020500678615759105 * fTemp137);
			double fTemp597 = (0.0067104222539056237 * fTemp99);
			double fTemp598 = (0.010535180889746074 * fTemp6);
			double fTemp599 = (0.0071028175068921775 * fTemp12);
			double fTemp600 = (0.0089472296718741736 * fTemp36);
			double fTemp601 = (0.010356563277706251 * fTemp42);
			double fTemp602 = (0.027334238154345505 * fTemp51);
			double fTemp603 = (0.0045334071240427437 * fTemp54);
			double fTemp604 = (0.010356563277706246 * fTemp57);
			double fTemp605 = (0.016998264729033857 * fTemp131);
			double fTemp606 = (0.021308452520676542 * fTemp82);
			double fTemp607 = (0.0031385126243372885 * fTemp87);
			double fTemp608 = (0.028411270027568748 * fTemp15);
			double fTemp609 = (0.0053753232814251943 * fTemp21);
			double fTemp610 = (0.022664352972045165 * fTemp24);
			double fTemp611 = (0.030071220904747779 * fTemp27);
			double fTemp612 = (0.010280801007450515 * fTemp33);
			double fTemp613 = (0.027200442744256356 * fTemp39);
			double fTemp614 = (0.00096856602366710779 * fTemp48);
			double fTemp615 = (fRec1[0] * ((fTemp509 + (fTemp591 + (fTemp592 + (fTemp593 + (fTemp594 + (fTemp595 + (fTemp596 + (fTemp514 + (fTemp597 + (fTemp515 + (fTemp475 + ((((fTemp444 + (((fTemp441 + ((fTemp435 + fTemp598) + fTemp599)) + fTemp600) + fTemp601)) + fTemp602) + fTemp603) + fTemp604)))))))))))) - (fTemp494 + (fTemp605 + (fTemp606 + (fTemp466 + (fTemp607 + (((((((fTemp453 + (fTemp452 + fTemp608)) + fTemp609) + fTemp610) + fTemp611) + fTemp612) + fTemp613) + fTemp614))))))));
			fRec272[0] = max((fRec272[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp615))))));
			fVbargraph72 = FAUSTFLOAT(fRec272[0]);
			output36[i] = FAUSTFLOAT(fTemp615);
			double fTemp616 = (0.00096856602366711159 * fTemp76);
			double fTemp617 = (0.020500678615759181 * fTemp137);
			double fTemp618 = (0.0045334071240427532 * fTemp87);
			double fTemp619 = (0.010280801007450555 * fTemp140);
			double fTemp620 = (0.0067104222539056471 * fTemp99);
			double fTemp621 = (0.0053753232814252143 * fTemp96);
			double fTemp622 = (0.031605542669238228 * fTemp6);
			double fTemp623 = (0.021308452520676539 * fTemp12);
			double fTemp624 = (0.028411270027568696 * fTemp15);
			double fTemp625 = (0.02266435297204512 * fTemp24);
			double fTemp626 = (0.02081853754944071 * fTemp27);
			double fTemp627 = (0.018831075746023575 * fTemp39);
			double fTemp628 = (0.010356563277706064 * fTemp42);
			double fTemp629 = (0.01035656327770606 * fTemp57);
			double fTemp630 = (0.0071028175068922053 * fTemp128);
			double fTemp631 = (0.016998264729033919 * fTemp131);
			double fTemp632 = (0.035508216952135623 * fTemp152);
			double fTemp633 = (0.036961085234966211 * fTemp79);
			double fTemp634 = (0.027200442744256415 * fTemp155);
			double fTemp635 = (0.035508216952135643 * fTemp91);
			double fTemp636 = (0.030071220904747842 * fTemp93);
			double fTemp637 = (0.021308452520676618 * fTemp82);
			double fTemp638 = (0.010535180889746113 * fTemp84);
			double fTemp639 = (0.016125969844275587 * fTemp21);
			double fTemp640 = (0.030842403022351552 * fTemp33);
			double fTemp641 = (0.0089472296718741563 * fTemp36);
			double fTemp642 = (0.0029056980710013241 * fTemp48);
			double fTemp643 = (0.02733423815434545 * fTemp51);
			double fTemp644 = (0.0031385126243372746 * fTemp54);
			double fTemp645 = (0.0014035855152521122 * fTemp60);
			double fTemp646 = (fRec1[0] * ((fTemp616 + (fTemp617 + (fTemp618 + (fTemp619 + (fTemp620 + (fTemp621 + ((fTemp444 + (((fTemp441 + (((((fTemp435 + fTemp622) + fTemp623) + fTemp624) + fTemp625) + fTemp626)) + fTemp627) + fTemp628)) + fTemp629))))))) - (fTemp630 + (fTemp631 + (fTemp632 + (fTemp633 + (fTemp634 + (fTemp635 + (fTemp636 + (fTemp637 + (fTemp638 + (((((((fTemp454 + fTemp639) + fTemp640) + fTemp641) + fTemp642) + fTemp643) + fTemp644) + fTemp645))))))))))));
			fRec273[0] = max((fRec273[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp646))))));
			fVbargraph73 = FAUSTFLOAT(fRec273[0]);
			output37[i] = FAUSTFLOAT(fTemp646);
			double fTemp647 = (fTemp434 + fTemp452);
			double fTemp648 = (fRec1[0] * ((fTemp428 + (fTemp430 + (fTemp431 + (fTemp432 + (fTemp433 + (fTemp447 + (fTemp449 + (fTemp459 + (fTemp443 + (fTemp456 + (fTemp441 + (fTemp440 + (fTemp453 + (fTemp438 + (fTemp336 + fTemp436))))))))))))))) - (fTemp425 + (fTemp426 + (fTemp446 + (fTemp427 + (fTemp429 + (fTemp448 + (fTemp450 + (fTemp451 + (fTemp461 + (fTemp445 + (fTemp460 + (fTemp458 + (fTemp444 + (fTemp442 + (fTemp457 + (fTemp439 + (fTemp455 + (fTemp437 + fTemp647))))))))))))))))))));
			fRec274[0] = max((fRec274[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp648))))));
			fVbargraph74 = FAUSTFLOAT(fRec274[0]);
			output38[i] = FAUSTFLOAT(fTemp648);
			double fTemp649 = (fRec1[0] * ((fTemp477 + (fTemp479 + (fTemp465 + (fTemp466 + (fTemp482 + (fTemp467 + (fTemp483 + (fTemp475 + (fTemp473 + (fTemp471 + (fTemp491 + (fTemp470 + (fTemp490 + (fTemp441 + (fTemp488 + (fTemp453 + (fTemp336 + fTemp468))))))))))))))))) - (fTemp463 + (fTemp464 + (fTemp476 + (fTemp478 + (fTemp480 + (fTemp481 + (fTemp484 + (fTemp485 + (fTemp474 + (fTemp472 + (fTemp492 + (fTemp444 + (fTemp489 + (fTemp487 + (fTemp486 + (fTemp469 + fTemp647))))))))))))))))));
			fRec275[0] = max((fRec275[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp649))))));
			fVbargraph75 = FAUSTFLOAT(fRec275[0]);
			output39[i] = FAUSTFLOAT(fTemp649);
			double fTemp650 = (fRec1[0] * ((fTemp508 + (fTemp511 + (fTemp480 + (fTemp466 + (fTemp497 + (fTemp514 + (fTemp498 + (fTemp505 + (fTemp504 + (fTemp502 + (fTemp441 + (fTemp500 + (fTemp488 + (fTemp499 + (fTemp453 + (fTemp336 + fTemp517)))))))))))))))) - (fTemp494 + (fTemp509 + (fTemp495 + (fTemp510 + (fTemp512 + (fTemp513 + (fTemp496 + (fTemp515 + (fTemp475 + (fTemp507 + (fTemp519 + (fTemp506 + (fTemp444 + (fTemp503 + (fTemp501 + (fTemp518 + (fTemp452 + (fTemp434 + fTemp516))))))))))))))))))));
			fRec276[0] = max((fRec276[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp650))))));
			fVbargraph76 = FAUSTFLOAT(fRec276[0]);
			output40[i] = FAUSTFLOAT(fTemp650);
			double fTemp651 = (fTemp452 + (fTemp434 + fTemp436));
			double fTemp652 = (fRec1[0] * ((fTemp426 + (fTemp427 + (fTemp428 + (fTemp523 + (fTemp524 + (fTemp536 + (fTemp526 + (fTemp532 + (fTemp530 + (fTemp541 + (fTemp458 + (fTemp529 + (fTemp540 + (fTemp441 + (fTemp455 + (fTemp453 + (fTemp527 + (fTemp336 + fTemp437)))))))))))))))))) - (fTemp521 + (fTemp533 + (fTemp522 + (fTemp430 + (fTemp534 + (fTemp525 + (fTemp535 + (fTemp537 + (fTemp531 + (fTemp444 + (fTemp457 + (fTemp456 + (fTemp539 + (fTemp528 + fTemp651))))))))))))))));
			fRec277[0] = max((fRec277[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp652))))));
			fVbargraph77 = FAUSTFLOAT(fRec277[0]);
			output41[i] = FAUSTFLOAT(fTemp652);
			double fTemp653 = (fRec1[0] * ((fTemp557 + (fTemp544 + (fTemp429 + (fTemp545 + (fTemp546 + (fTemp560 + (fTemp448 + (fTemp548 + (fTemp556 + (fTemp554 + (fTemp566 + (fTemp553 + (fTemp552 + (fTemp565 + (fTemp441 + (fTemp455 + (fTemp453 + (fTemp549 + (fTemp336 + fTemp562))))))))))))))))))) - (fTemp543 + (fTemp427 + (fTemp428 + (fTemp558 + (fTemp559 + (fTemp547 + (fTemp561 + (fTemp555 + (fTemp444 + (fTemp564 + (fTemp551 + (fTemp563 + (fTemp550 + fTemp651)))))))))))))));
			fRec278[0] = max((fRec278[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp653))))));
			fVbargraph78 = FAUSTFLOAT(fRec278[0]);
			output42[i] = FAUSTFLOAT(fTemp653);
			double fTemp654 = (fRec1[0] * ((fTemp463 + (fTemp476 + (fTemp582 + (fTemp569 + (fTemp571 + (fTemp572 + (fTemp583 + (fTemp485 + (fTemp579 + (fTemp578 + (fTemp576 + (fTemp441 + (fTemp574 + (fTemp550 + (fTemp573 + (fTemp453 + (fTemp336 + fTemp587))))))))))))))))) - (fTemp568 + (fTemp570 + (fTemp545 + (fTemp466 + (fTemp584 + (fTemp483 + (fTemp585 + (fTemp475 + (fTemp581 + (fTemp589 + (fTemp580 + (fTemp444 + (fTemp577 + (fTemp575 + (fTemp588 + (fTemp452 + (fTemp434 + fTemp586)))))))))))))))))));
			fRec279[0] = max((fRec279[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp654))))));
			fVbargraph79 = FAUSTFLOAT(fRec279[0]);
			output43[i] = FAUSTFLOAT(fTemp654);
			double fTemp655 = (fRec1[0] * ((fTemp494 + (fTemp605 + (fTemp509 + (fTemp592 + (fTemp594 + (fTemp595 + (fTemp597 + (fTemp515 + (fTemp475 + (fTemp603 + (fTemp601 + (fTemp613 + (fTemp600 + (fTemp612 + (fTemp441 + (fTemp610 + (fTemp453 + (fTemp336 + fTemp598)))))))))))))))))) - (fTemp591 + (fTemp593 + (fTemp606 + (fTemp466 + (fTemp596 + (fTemp607 + (fTemp514 + (fTemp604 + (fTemp602 + (fTemp614 + (fTemp444 + (fTemp611 + (fTemp609 + (fTemp608 + (fTemp599 + fTemp647)))))))))))))))));
			fRec280[0] = max((fRec280[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp655))))));
			fVbargraph80 = FAUSTFLOAT(fRec280[0]);
			output44[i] = FAUSTFLOAT(fTemp655);
			double fTemp656 = (fRec1[0] * ((fTemp630 + (fTemp631 + (fTemp616 + (fTemp632 + (fTemp634 + (fTemp618 + (fTemp620 + (fTemp621 + (fTemp643 + (fTemp628 + (fTemp640 + (fTemp441 + (fTemp626 + (fTemp453 + (fTemp624 + (fTemp336 + fTemp622)))))))))))))))) - (fTemp633 + (fTemp635 + (fTemp636 + (fTemp637 + (fTemp638 + (fTemp617 + (fTemp619 + (fTemp645 + (fTemp629 + (fTemp644 + (fTemp642 + (fTemp444 + (fTemp627 + (fTemp641 + (fTemp625 + (fTemp639 + (fTemp623 + fTemp647)))))))))))))))))));
			fRec281[0] = max((fRec281[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp656))))));
			fVbargraph81 = FAUSTFLOAT(fRec281[0]);
			output45[i] = FAUSTFLOAT(fTemp656);
			double fTemp657 = (fTemp336 + fTemp339);
			double fTemp658 = (fRec1[0] * ((fTemp325 + (fTemp327 + (fTemp347 + (fTemp329 + (fTemp330 + (fTemp332 + (fTemp334 + (fTemp335 + (fTemp359 + (fTemp357 + (fTemp346 + (fTemp356 + (fTemp354 + (fTemp344 + (fTemp351 + (fTemp343 + fTemp657)))))))))))))))) - (fTemp324 + (fTemp326 + (fTemp348 + (fTemp328 + (fTemp349 + (fTemp331 + (fTemp333 + (fTemp360 + (fTemp358 + (fTemp355 + (fTemp353 + (fTemp345 + (fTemp352 + (fTemp341 + (fTemp337 + fTemp350)))))))))))))))));
			fRec282[0] = max((fRec282[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp658))))));
			fVbargraph82 = FAUSTFLOAT(fRec282[0]);
			output46[i] = FAUSTFLOAT(fTemp658);
			double fTemp659 = (fRec1[0] * ((fTemp369 + (fTemp326 + (fTemp327 + (fTemp364 + (fTemp329 + (fTemp331 + (fTemp365 + (fTemp335 + (fTemp368 + (fTemp359 + (fTemp358 + (fTemp375 + (fTemp356 + (fTemp345 + (fTemp344 + (fTemp366 + (fTemp373 + fTemp657))))))))))))))))) - (fTemp370 + (fTemp362 + (fTemp371 + (fTemp363 + (fTemp330 + (fTemp333 + (fTemp334 + (fTemp346 + (fTemp355 + (fTemp367 + (fTemp374 + (fTemp343 + (fTemp341 + (fTemp337 + fTemp372))))))))))))))));
			fRec283[0] = max((fRec283[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp659))))));
			fVbargraph83 = FAUSTFLOAT(fRec283[0]);
			output47[i] = FAUSTFLOAT(fTemp659);
			double fTemp660 = (fRec1[0] * ((fTemp388 + (fTemp378 + (fTemp390 + (fTemp379 + (fTemp330 + (fTemp394 + (fTemp334 + (fTemp387 + (fTemp359 + (fTemp386 + (fTemp356 + (fTemp383 + (fTemp399 + (fTemp344 + (fTemp382 + (fTemp380 + (fTemp397 + fTemp657))))))))))))))))) - (fTemp326 + (fTemp389 + (fTemp377 + (fTemp391 + (fTemp392 + (fTemp331 + (fTemp393 + (fTemp395 + (fTemp385 + (fTemp400 + (fTemp355 + (fTemp384 + (fTemp381 + (fTemp398 + (fTemp341 + (fTemp337 + fTemp396))))))))))))))))));
			fRec284[0] = max((fRec284[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp660))))));
			fVbargraph84 = FAUSTFLOAT(fRec284[0]);
			output48[i] = FAUSTFLOAT(fTemp660);
			double fTemp661 = (fRec1[0] * ((fTemp410 + (fTemp326 + (fTemp403 + (fTemp411 + (fTemp404 + (fTemp331 + (fTemp333 + (fTemp359 + (fTemp421 + (fTemp409 + (fTemp356 + (fTemp420 + (fTemp344 + (fTemp417 + (fTemp407 + (fTemp339 + (fTemp336 + fTemp405))))))))))))))))) - (fTemp327 + (fTemp402 + (fTemp412 + (fTemp330 + (fTemp413 + (fTemp414 + (fTemp334 + (fTemp415 + (fTemp423 + (fTemp422 + (fTemp355 + (fTemp419 + (fTemp408 + (fTemp418 + (fTemp341 + (fTemp416 + (fTemp337 + fTemp406)))))))))))))))))));
			fRec285[0] = max((fRec285[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp661))))));
			fVbargraph85 = FAUSTFLOAT(fRec285[0]);
			output49[i] = FAUSTFLOAT(fTemp661);
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
			fRec49[1] = fRec49[0];
			fRec50[1] = fRec50[0];
			fRec48[1] = fRec48[0];
			fRec51[1] = fRec51[0];
			fRec52[1] = fRec52[0];
			fRec47[1] = fRec47[0];
			fRec46[1] = fRec46[0];
			fRec56[1] = fRec56[0];
			fRec57[1] = fRec57[0];
			fRec55[1] = fRec55[0];
			fRec58[1] = fRec58[0];
			fRec59[1] = fRec59[0];
			fRec54[1] = fRec54[0];
			fRec53[1] = fRec53[0];
			fRec63[1] = fRec63[0];
			fRec64[1] = fRec64[0];
			fRec62[1] = fRec62[0];
			fRec65[1] = fRec65[0];
			fRec66[1] = fRec66[0];
			fRec61[1] = fRec61[0];
			fRec60[1] = fRec60[0];
			fRec70[1] = fRec70[0];
			fRec71[1] = fRec71[0];
			fRec69[1] = fRec69[0];
			fRec72[1] = fRec72[0];
			fRec73[1] = fRec73[0];
			fRec68[1] = fRec68[0];
			fRec67[1] = fRec67[0];
			fRec77[1] = fRec77[0];
			fRec78[1] = fRec78[0];
			fRec76[1] = fRec76[0];
			fRec79[1] = fRec79[0];
			fRec80[1] = fRec80[0];
			fRec75[1] = fRec75[0];
			fRec74[1] = fRec74[0];
			fRec85[1] = fRec85[0];
			fRec86[1] = fRec86[0];
			fRec84[1] = fRec84[0];
			fRec87[1] = fRec87[0];
			fRec88[1] = fRec88[0];
			fRec83[1] = fRec83[0];
			fRec89[1] = fRec89[0];
			fRec82[1] = fRec82[0];
			fRec81[1] = fRec81[0];
			fRec94[1] = fRec94[0];
			fRec95[1] = fRec95[0];
			fRec93[1] = fRec93[0];
			fRec96[1] = fRec96[0];
			fRec97[1] = fRec97[0];
			fRec92[1] = fRec92[0];
			fRec98[1] = fRec98[0];
			fRec91[1] = fRec91[0];
			fRec90[1] = fRec90[0];
			fRec103[1] = fRec103[0];
			fRec104[1] = fRec104[0];
			fRec102[1] = fRec102[0];
			fRec105[1] = fRec105[0];
			fRec106[1] = fRec106[0];
			fRec101[1] = fRec101[0];
			fRec107[1] = fRec107[0];
			fRec100[1] = fRec100[0];
			fRec99[1] = fRec99[0];
			fRec112[1] = fRec112[0];
			fRec113[1] = fRec113[0];
			fRec111[1] = fRec111[0];
			fRec114[1] = fRec114[0];
			fRec115[1] = fRec115[0];
			fRec110[1] = fRec110[0];
			fRec116[1] = fRec116[0];
			fRec109[1] = fRec109[0];
			fRec108[1] = fRec108[0];
			fRec121[1] = fRec121[0];
			fRec122[1] = fRec122[0];
			fRec120[1] = fRec120[0];
			fRec123[1] = fRec123[0];
			fRec124[1] = fRec124[0];
			fRec119[1] = fRec119[0];
			fRec125[1] = fRec125[0];
			fRec118[1] = fRec118[0];
			fRec117[1] = fRec117[0];
			fRec130[1] = fRec130[0];
			fRec131[1] = fRec131[0];
			fRec129[1] = fRec129[0];
			fRec132[1] = fRec132[0];
			fRec133[1] = fRec133[0];
			fRec128[1] = fRec128[0];
			fRec134[1] = fRec134[0];
			fRec127[1] = fRec127[0];
			fRec126[1] = fRec126[0];
			fRec0[1] = fRec0[0];
			fRec135[1] = fRec135[0];
			fRec141[1] = fRec141[0];
			fRec142[1] = fRec142[0];
			fRec140[1] = fRec140[0];
			fRec143[1] = fRec143[0];
			fRec144[1] = fRec144[0];
			fRec139[1] = fRec139[0];
			fRec145[1] = fRec145[0];
			fRec138[1] = fRec138[0];
			fRec137[1] = fRec137[0];
			fRec150[1] = fRec150[0];
			fRec151[1] = fRec151[0];
			fRec149[1] = fRec149[0];
			fRec152[1] = fRec152[0];
			fRec153[1] = fRec153[0];
			fRec148[1] = fRec148[0];
			fRec154[1] = fRec154[0];
			fRec147[1] = fRec147[0];
			fRec146[1] = fRec146[0];
			fRec157[1] = fRec157[0];
			fRec158[1] = fRec158[0];
			fRec156[1] = fRec156[0];
			fRec155[1] = fRec155[0];
			fRec161[1] = fRec161[0];
			fRec160[1] = fRec160[0];
			fRec159[1] = fRec159[0];
			fRec166[1] = fRec166[0];
			fRec167[1] = fRec167[0];
			fRec165[1] = fRec165[0];
			fRec168[1] = fRec168[0];
			fRec169[1] = fRec169[0];
			fRec164[1] = fRec164[0];
			fRec170[1] = fRec170[0];
			fRec163[1] = fRec163[0];
			fRec162[1] = fRec162[0];
			fRec174[1] = fRec174[0];
			fRec175[1] = fRec175[0];
			fRec173[1] = fRec173[0];
			fRec176[1] = fRec176[0];
			fRec177[1] = fRec177[0];
			fRec172[1] = fRec172[0];
			fRec171[1] = fRec171[0];
			fRec181[1] = fRec181[0];
			fRec182[1] = fRec182[0];
			fRec180[1] = fRec180[0];
			fRec183[1] = fRec183[0];
			fRec179[1] = fRec179[0];
			fRec178[1] = fRec178[0];
			fRec187[1] = fRec187[0];
			fRec188[1] = fRec188[0];
			fRec186[1] = fRec186[0];
			fRec189[1] = fRec189[0];
			fRec185[1] = fRec185[0];
			fRec184[1] = fRec184[0];
			fRec193[1] = fRec193[0];
			fRec194[1] = fRec194[0];
			fRec192[1] = fRec192[0];
			fRec195[1] = fRec195[0];
			fRec196[1] = fRec196[0];
			fRec191[1] = fRec191[0];
			fRec190[1] = fRec190[0];
			fRec136[1] = fRec136[0];
			fRec197[1] = fRec197[0];
			fRec198[1] = fRec198[0];
			fRec199[1] = fRec199[0];
			fRec200[1] = fRec200[0];
			fRec204[1] = fRec204[0];
			fRec205[1] = fRec205[0];
			fRec203[1] = fRec203[0];
			fRec202[1] = fRec202[0];
			fRec209[1] = fRec209[0];
			fRec210[1] = fRec210[0];
			fRec208[1] = fRec208[0];
			fRec211[1] = fRec211[0];
			fRec207[1] = fRec207[0];
			fRec206[1] = fRec206[0];
			fRec216[1] = fRec216[0];
			fRec217[1] = fRec217[0];
			fRec215[1] = fRec215[0];
			fRec218[1] = fRec218[0];
			fRec219[1] = fRec219[0];
			fRec214[1] = fRec214[0];
			fRec220[1] = fRec220[0];
			fRec213[1] = fRec213[0];
			fRec212[1] = fRec212[0];
			fRec224[1] = fRec224[0];
			fRec225[1] = fRec225[0];
			fRec223[1] = fRec223[0];
			fRec226[1] = fRec226[0];
			fRec227[1] = fRec227[0];
			fRec222[1] = fRec222[0];
			fRec221[1] = fRec221[0];
			fRec232[1] = fRec232[0];
			fRec233[1] = fRec233[0];
			fRec231[1] = fRec231[0];
			fRec234[1] = fRec234[0];
			fRec235[1] = fRec235[0];
			fRec230[1] = fRec230[0];
			fRec236[1] = fRec236[0];
			fRec229[1] = fRec229[0];
			fRec228[1] = fRec228[0];
			fRec240[1] = fRec240[0];
			fRec241[1] = fRec241[0];
			fRec239[1] = fRec239[0];
			fRec242[1] = fRec242[0];
			fRec243[1] = fRec243[0];
			fRec238[1] = fRec238[0];
			fRec237[1] = fRec237[0];
			fRec201[1] = fRec201[0];
			fRec244[1] = fRec244[0];
			fRec245[1] = fRec245[0];
			fRec246[1] = fRec246[0];
			fRec247[1] = fRec247[0];
			fRec248[1] = fRec248[0];
			fRec249[1] = fRec249[0];
			fRec250[1] = fRec250[0];
			fRec251[1] = fRec251[0];
			fRec252[1] = fRec252[0];
			fRec253[1] = fRec253[0];
			fRec254[1] = fRec254[0];
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
