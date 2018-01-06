/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOADecLebedev504"
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
	double fRec0[2];
	FAUSTFLOAT fVbargraph15;
	double fRec81[2];
	FAUSTFLOAT fVbargraph16;
	double fRec85[2];
	double fRec86[2];
	double fRec84[2];
	double fRec83[2];
	FAUSTFLOAT fVbargraph17;
	double fRec89[2];
	double fRec88[2];
	double fRec87[2];
	FAUSTFLOAT fVbargraph18;
	double fRec93[2];
	double fRec94[2];
	double fRec92[2];
	double fRec95[2];
	double fRec96[2];
	double fRec91[2];
	double fRec90[2];
	FAUSTFLOAT fVbargraph19;
	double fRec100[2];
	double fRec101[2];
	double fRec99[2];
	double fRec102[2];
	double fRec98[2];
	double fRec97[2];
	FAUSTFLOAT fVbargraph20;
	double fRec106[2];
	double fRec107[2];
	double fRec105[2];
	double fRec108[2];
	double fRec104[2];
	double fRec103[2];
	FAUSTFLOAT fVbargraph21;
	double fRec112[2];
	double fRec113[2];
	double fRec111[2];
	double fRec114[2];
	double fRec115[2];
	double fRec110[2];
	double fRec109[2];
	FAUSTFLOAT fVbargraph22;
	double fRec82[2];
	FAUSTFLOAT fVbargraph23;
	double fRec116[2];
	FAUSTFLOAT fVbargraph24;
	double fRec117[2];
	FAUSTFLOAT fVbargraph25;
	double fRec118[2];
	FAUSTFLOAT fVbargraph26;
	double fRec119[2];
	FAUSTFLOAT fVbargraph27;
	double fRec123[2];
	double fRec124[2];
	double fRec122[2];
	double fRec121[2];
	FAUSTFLOAT fVbargraph28;
	double fRec128[2];
	double fRec129[2];
	double fRec127[2];
	double fRec130[2];
	double fRec126[2];
	double fRec125[2];
	FAUSTFLOAT fVbargraph29;
	double fRec134[2];
	double fRec135[2];
	double fRec133[2];
	double fRec136[2];
	double fRec137[2];
	double fRec132[2];
	double fRec131[2];
	FAUSTFLOAT fVbargraph30;
	double fRec141[2];
	double fRec142[2];
	double fRec140[2];
	double fRec143[2];
	double fRec144[2];
	double fRec139[2];
	double fRec138[2];
	FAUSTFLOAT fVbargraph31;
	double fRec120[2];
	FAUSTFLOAT fVbargraph32;
	double fRec145[2];
	FAUSTFLOAT fVbargraph33;
	double fRec146[2];
	FAUSTFLOAT fVbargraph34;
	double fRec147[2];
	FAUSTFLOAT fVbargraph35;
	double fRec148[2];
	FAUSTFLOAT fVbargraph36;
	double fRec149[2];
	FAUSTFLOAT fVbargraph37;
	double fRec150[2];
	FAUSTFLOAT fVbargraph38;
	double fRec151[2];
	FAUSTFLOAT fVbargraph39;
	double fRec152[2];
	FAUSTFLOAT fVbargraph40;
	double fRec153[2];
	FAUSTFLOAT fVbargraph41;
	double fRec154[2];
	FAUSTFLOAT fVbargraph42;
	double fRec155[2];
	FAUSTFLOAT fVbargraph43;
	double fRec156[2];
	FAUSTFLOAT fVbargraph44;
	double fRec157[2];
	FAUSTFLOAT fVbargraph45;
	double fRec158[2];
	FAUSTFLOAT fVbargraph46;
	double fRec159[2];
	FAUSTFLOAT fVbargraph47;
	double fRec160[2];
	FAUSTFLOAT fVbargraph48;
	double fRec161[2];
	FAUSTFLOAT fVbargraph49;
	double fRec162[2];
	FAUSTFLOAT fVbargraph50;
	double fRec163[2];
	FAUSTFLOAT fVbargraph51;
	double fRec164[2];
	FAUSTFLOAT fVbargraph52;
	double fRec165[2];
	FAUSTFLOAT fVbargraph53;
	double fRec166[2];
	FAUSTFLOAT fVbargraph54;
	double fRec167[2];
	FAUSTFLOAT fVbargraph55;
	double fRec168[2];
	FAUSTFLOAT fVbargraph56;
	double fRec169[2];
	FAUSTFLOAT fVbargraph57;
	double fRec170[2];
	FAUSTFLOAT fVbargraph58;
	double fRec171[2];
	FAUSTFLOAT fVbargraph59;
	double fRec172[2];
	FAUSTFLOAT fVbargraph60;
	double fRec173[2];
	FAUSTFLOAT fVbargraph61;
	double fRec174[2];
	FAUSTFLOAT fVbargraph62;
	double fRec175[2];
	FAUSTFLOAT fVbargraph63;
	double fRec176[2];
	FAUSTFLOAT fVbargraph64;
	double fRec177[2];
	FAUSTFLOAT fVbargraph65;
	double fRec178[2];
	FAUSTFLOAT fVbargraph66;
	double fRec179[2];
	FAUSTFLOAT fVbargraph67;
	double fRec180[2];
	FAUSTFLOAT fVbargraph68;
	double fRec181[2];
	FAUSTFLOAT fVbargraph69;
	double fRec182[2];
	FAUSTFLOAT fVbargraph70;
	double fRec183[2];
	FAUSTFLOAT fVbargraph71;
	double fRec184[2];
	FAUSTFLOAT fVbargraph72;
	double fRec185[2];
	FAUSTFLOAT fVbargraph73;
	double fRec186[2];
	FAUSTFLOAT fVbargraph74;
	
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
		m->declare("name", "HOADecLebedev504");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
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
			fRec0[l80] = 0.0;
			
		}
		for (int l81 = 0; (l81 < 2); l81 = (l81 + 1)) {
			fRec81[l81] = 0.0;
			
		}
		for (int l82 = 0; (l82 < 2); l82 = (l82 + 1)) {
			fRec85[l82] = 0.0;
			
		}
		for (int l83 = 0; (l83 < 2); l83 = (l83 + 1)) {
			fRec86[l83] = 0.0;
			
		}
		for (int l84 = 0; (l84 < 2); l84 = (l84 + 1)) {
			fRec84[l84] = 0.0;
			
		}
		for (int l85 = 0; (l85 < 2); l85 = (l85 + 1)) {
			fRec83[l85] = 0.0;
			
		}
		for (int l86 = 0; (l86 < 2); l86 = (l86 + 1)) {
			fRec89[l86] = 0.0;
			
		}
		for (int l87 = 0; (l87 < 2); l87 = (l87 + 1)) {
			fRec88[l87] = 0.0;
			
		}
		for (int l88 = 0; (l88 < 2); l88 = (l88 + 1)) {
			fRec87[l88] = 0.0;
			
		}
		for (int l89 = 0; (l89 < 2); l89 = (l89 + 1)) {
			fRec93[l89] = 0.0;
			
		}
		for (int l90 = 0; (l90 < 2); l90 = (l90 + 1)) {
			fRec94[l90] = 0.0;
			
		}
		for (int l91 = 0; (l91 < 2); l91 = (l91 + 1)) {
			fRec92[l91] = 0.0;
			
		}
		for (int l92 = 0; (l92 < 2); l92 = (l92 + 1)) {
			fRec95[l92] = 0.0;
			
		}
		for (int l93 = 0; (l93 < 2); l93 = (l93 + 1)) {
			fRec96[l93] = 0.0;
			
		}
		for (int l94 = 0; (l94 < 2); l94 = (l94 + 1)) {
			fRec91[l94] = 0.0;
			
		}
		for (int l95 = 0; (l95 < 2); l95 = (l95 + 1)) {
			fRec90[l95] = 0.0;
			
		}
		for (int l96 = 0; (l96 < 2); l96 = (l96 + 1)) {
			fRec100[l96] = 0.0;
			
		}
		for (int l97 = 0; (l97 < 2); l97 = (l97 + 1)) {
			fRec101[l97] = 0.0;
			
		}
		for (int l98 = 0; (l98 < 2); l98 = (l98 + 1)) {
			fRec99[l98] = 0.0;
			
		}
		for (int l99 = 0; (l99 < 2); l99 = (l99 + 1)) {
			fRec102[l99] = 0.0;
			
		}
		for (int l100 = 0; (l100 < 2); l100 = (l100 + 1)) {
			fRec98[l100] = 0.0;
			
		}
		for (int l101 = 0; (l101 < 2); l101 = (l101 + 1)) {
			fRec97[l101] = 0.0;
			
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
			fRec104[l106] = 0.0;
			
		}
		for (int l107 = 0; (l107 < 2); l107 = (l107 + 1)) {
			fRec103[l107] = 0.0;
			
		}
		for (int l108 = 0; (l108 < 2); l108 = (l108 + 1)) {
			fRec112[l108] = 0.0;
			
		}
		for (int l109 = 0; (l109 < 2); l109 = (l109 + 1)) {
			fRec113[l109] = 0.0;
			
		}
		for (int l110 = 0; (l110 < 2); l110 = (l110 + 1)) {
			fRec111[l110] = 0.0;
			
		}
		for (int l111 = 0; (l111 < 2); l111 = (l111 + 1)) {
			fRec114[l111] = 0.0;
			
		}
		for (int l112 = 0; (l112 < 2); l112 = (l112 + 1)) {
			fRec115[l112] = 0.0;
			
		}
		for (int l113 = 0; (l113 < 2); l113 = (l113 + 1)) {
			fRec110[l113] = 0.0;
			
		}
		for (int l114 = 0; (l114 < 2); l114 = (l114 + 1)) {
			fRec109[l114] = 0.0;
			
		}
		for (int l115 = 0; (l115 < 2); l115 = (l115 + 1)) {
			fRec82[l115] = 0.0;
			
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
			fRec123[l120] = 0.0;
			
		}
		for (int l121 = 0; (l121 < 2); l121 = (l121 + 1)) {
			fRec124[l121] = 0.0;
			
		}
		for (int l122 = 0; (l122 < 2); l122 = (l122 + 1)) {
			fRec122[l122] = 0.0;
			
		}
		for (int l123 = 0; (l123 < 2); l123 = (l123 + 1)) {
			fRec121[l123] = 0.0;
			
		}
		for (int l124 = 0; (l124 < 2); l124 = (l124 + 1)) {
			fRec128[l124] = 0.0;
			
		}
		for (int l125 = 0; (l125 < 2); l125 = (l125 + 1)) {
			fRec129[l125] = 0.0;
			
		}
		for (int l126 = 0; (l126 < 2); l126 = (l126 + 1)) {
			fRec127[l126] = 0.0;
			
		}
		for (int l127 = 0; (l127 < 2); l127 = (l127 + 1)) {
			fRec130[l127] = 0.0;
			
		}
		for (int l128 = 0; (l128 < 2); l128 = (l128 + 1)) {
			fRec126[l128] = 0.0;
			
		}
		for (int l129 = 0; (l129 < 2); l129 = (l129 + 1)) {
			fRec125[l129] = 0.0;
			
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
			fRec141[l137] = 0.0;
			
		}
		for (int l138 = 0; (l138 < 2); l138 = (l138 + 1)) {
			fRec142[l138] = 0.0;
			
		}
		for (int l139 = 0; (l139 < 2); l139 = (l139 + 1)) {
			fRec140[l139] = 0.0;
			
		}
		for (int l140 = 0; (l140 < 2); l140 = (l140 + 1)) {
			fRec143[l140] = 0.0;
			
		}
		for (int l141 = 0; (l141 < 2); l141 = (l141 + 1)) {
			fRec144[l141] = 0.0;
			
		}
		for (int l142 = 0; (l142 < 2); l142 = (l142 + 1)) {
			fRec139[l142] = 0.0;
			
		}
		for (int l143 = 0; (l143 < 2); l143 = (l143 + 1)) {
			fRec138[l143] = 0.0;
			
		}
		for (int l144 = 0; (l144 < 2); l144 = (l144 + 1)) {
			fRec120[l144] = 0.0;
			
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
			fRec149[l149] = 0.0;
			
		}
		for (int l150 = 0; (l150 < 2); l150 = (l150 + 1)) {
			fRec150[l150] = 0.0;
			
		}
		for (int l151 = 0; (l151 < 2); l151 = (l151 + 1)) {
			fRec151[l151] = 0.0;
			
		}
		for (int l152 = 0; (l152 < 2); l152 = (l152 + 1)) {
			fRec152[l152] = 0.0;
			
		}
		for (int l153 = 0; (l153 < 2); l153 = (l153 + 1)) {
			fRec153[l153] = 0.0;
			
		}
		for (int l154 = 0; (l154 < 2); l154 = (l154 + 1)) {
			fRec154[l154] = 0.0;
			
		}
		for (int l155 = 0; (l155 < 2); l155 = (l155 + 1)) {
			fRec155[l155] = 0.0;
			
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
		ui_interface->addVerticalBargraph("0x354ece0", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3641f60", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x355a1e0", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3563b80", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3638b80", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36b4090", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3572420", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x357dab0", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3589140", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3661460", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36c27e0", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x366fba0", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x359bd00", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35aa8d0", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35b94a0", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35c8150", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3652e50", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36e7c60", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3681080", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36d35c0", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35dd810", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35ee0d0", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x35fe990", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x360f330", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x361fdb0", &fVbargraph14, -70.0, 6.0);
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
		ui_interface->declare(&fVbargraph15, "osc", "/output1");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36251b0", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph16, "osc", "/output2");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x362da80", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph23, "osc", "/output3");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3686570", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph24, "osc", "/output4");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x368e2a0", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph25, "osc", "/output5");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36960a0", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph26, "osc", "/output6");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x369c940", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph27, "osc", "/output7");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36a8b30", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph32, "osc", "/output8");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36ee670", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph33, "osc", "/output9");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36f9880", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph34, "osc", "/output10");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3704d40", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph35, "osc", "/output11");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x370f3c0", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph36, "osc", "/output12");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3717a10", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph37, "osc", "/output13");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x371fb60", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph38, "osc", "/output14");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3728150", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph39, "osc", "/output15");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x372f010", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph40, "osc", "/output16");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3737270", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph41, "osc", "/output17");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x373f7c0", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph42, "osc", "/output18");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3747be0", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph43, "osc", "/output19");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3758910", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph44, "osc", "/output20");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37656b0", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph45, "osc", "/output21");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3771cc0", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph46, "osc", "/output22");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x377e620", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph47, "osc", "/output23");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37872c0", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph48, "osc", "/output24");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x378fa40", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph49, "osc", "/output25");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3798660", &fVbargraph49, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph50, "osc", "/output26");
		ui_interface->declare(&fVbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37a1340", &fVbargraph50, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph51, "osc", "/output27");
		ui_interface->declare(&fVbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37b1930", &fVbargraph51, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph52, "osc", "/output28");
		ui_interface->declare(&fVbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37bbf30", &fVbargraph52, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph53, "osc", "/output29");
		ui_interface->declare(&fVbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37c8ac0", &fVbargraph53, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph54, "osc", "/output30");
		ui_interface->declare(&fVbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37d4fb0", &fVbargraph54, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph55, "osc", "/output31");
		ui_interface->declare(&fVbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37e5bf0", &fVbargraph55, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph56, "osc", "/output32");
		ui_interface->declare(&fVbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37f4870", &fVbargraph56, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph57, "osc", "/output33");
		ui_interface->declare(&fVbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3802660", &fVbargraph57, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph58, "osc", "/output34");
		ui_interface->declare(&fVbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x380f210", &fVbargraph58, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph59, "osc", "/output35");
		ui_interface->declare(&fVbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x381ce10", &fVbargraph59, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fVbargraph60, "osc", "/output36");
		ui_interface->declare(&fVbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3829f30", &fVbargraph60, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fVbargraph61, "osc", "/output37");
		ui_interface->declare(&fVbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3837ae0", &fVbargraph61, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fVbargraph62, "osc", "/output38");
		ui_interface->declare(&fVbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3846e60", &fVbargraph62, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fVbargraph63, "osc", "/output39");
		ui_interface->declare(&fVbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x384f590", &fVbargraph63, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fVbargraph64, "osc", "/output40");
		ui_interface->declare(&fVbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38579b0", &fVbargraph64, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fVbargraph65, "osc", "/output41");
		ui_interface->declare(&fVbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3860090", &fVbargraph65, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fVbargraph66, "osc", "/output42");
		ui_interface->declare(&fVbargraph66, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3868a60", &fVbargraph66, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fVbargraph67, "osc", "/output43");
		ui_interface->declare(&fVbargraph67, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3870f40", &fVbargraph67, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fVbargraph68, "osc", "/output44");
		ui_interface->declare(&fVbargraph68, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38798c0", &fVbargraph68, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fVbargraph69, "osc", "/output45");
		ui_interface->declare(&fVbargraph69, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3882140", &fVbargraph69, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fVbargraph70, "osc", "/output46");
		ui_interface->declare(&fVbargraph70, "unit", "dB");
		ui_interface->addVerticalBargraph("0x388aaa0", &fVbargraph70, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fVbargraph71, "osc", "/output47");
		ui_interface->declare(&fVbargraph71, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3893510", &fVbargraph71, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fVbargraph72, "osc", "/output48");
		ui_interface->declare(&fVbargraph72, "unit", "dB");
		ui_interface->addVerticalBargraph("0x389bc70", &fVbargraph72, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fVbargraph73, "osc", "/output49");
		ui_interface->declare(&fVbargraph73, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38a4870", &fVbargraph73, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fVbargraph74, "osc", "/output50");
		ui_interface->declare(&fVbargraph74, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38ad730", &fVbargraph74, -70.0, 6.0);
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
			double fTemp44 = (fRec1[0] * ((((((((((((((fTemp1 + fTemp4) + fTemp7) + fTemp10) + fTemp13) + fTemp16) + fTemp19) + fTemp22) + fTemp25) + fTemp28) + fTemp31) + fTemp34) + fTemp37) + fTemp40) + fTemp43));
			fRec0[0] = max((fRec0[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fVbargraph15 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp44);
			double fTemp45 = (0.021994295969128601 * fTemp6);
			double fTemp46 = (0.024590370452110585 * fTemp15);
			double fTemp47 = (0.026560635762986527 * fTemp27);
			double fTemp48 = (0.014285714285714285 * fTemp30);
			double fTemp49 = (0.028171808490950554 * fTemp42);
			double fTemp50 = (0.014197256999998666 * fTemp9);
			double fTemp51 = (0.020573779994945588 * fTemp21);
			double fTemp52 = (0.021295885499997998 * fTemp36);
			double fTemp53 = (fRec1[0] * ((((((fTemp1 + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) - ((fTemp50 + fTemp51) + fTemp52)));
			fRec81[0] = max((fRec81[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fVbargraph16 = FAUSTFLOAT(fRec81[0]);
			output1[i] = FAUSTFLOAT(fTemp53);
			double fTemp54 = (fRec3[0] * double(input4[i]));
			fRec85[0] = (fRec85[1] + fRec84[1]);
			fRec86[0] = (fRec86[1] + fRec85[1]);
			fRec84[0] = (fSlow11 * ((fSlow3 * fTemp54) - ((fSlow13 * fRec85[0]) + (fSlow12 * fRec86[0]))));
			double fTemp55 = (fRec84[0] + (fSlow4 * fTemp54));
			fRec83[0] = max((fRec83[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph17 = FAUSTFLOAT(fRec83[0]);
			double fTemp56 = (fRec3[0] * double(input1[i]));
			fRec89[0] = (fRec89[1] + fRec88[1]);
			fRec88[0] = (fSlow8 * ((fSlow3 * fTemp56) - (fSlow9 * fRec89[0])));
			double fTemp57 = (fRec88[0] + (fSlow4 * fTemp56));
			fRec87[0] = max((fRec87[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fVbargraph18 = FAUSTFLOAT(fRec87[0]);
			double fTemp58 = (0.021994295969128601 * fTemp57);
			double fTemp59 = (fTemp1 + fTemp7);
			double fTemp60 = (fRec3[0] * double(input16[i]));
			fRec93[0] = (fRec93[1] + fRec92[1]);
			fRec94[0] = (fRec94[1] + fRec93[1]);
			fRec92[0] = (fSlow20 * ((fSlow3 * fTemp60) - ((fSlow22 * fRec93[0]) + (fSlow21 * fRec94[0]))));
			fRec95[0] = (fRec95[1] + fRec91[1]);
			fRec96[0] = (fRec96[1] + fRec95[1]);
			fRec91[0] = (fSlow19 * (fRec92[0] - ((fSlow24 * fRec95[0]) + (fSlow23 * fRec96[0]))));
			double fTemp61 = (fRec91[0] + (fSlow4 * fTemp60));
			fRec90[0] = max((fRec90[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fVbargraph19 = FAUSTFLOAT(fRec90[0]);
			double fTemp62 = (fRec3[0] * double(input9[i]));
			fRec100[0] = (fRec100[1] + fRec99[1]);
			fRec101[0] = (fRec101[1] + fRec100[1]);
			fRec99[0] = (fSlow15 * ((fSlow3 * fTemp62) - ((fSlow17 * fRec100[0]) + (fSlow16 * fRec101[0]))));
			fRec102[0] = (fRec102[1] + fRec98[1]);
			fRec98[0] = (fSlow14 * (fRec99[0] - (fSlow18 * fRec102[0])));
			double fTemp63 = (fRec98[0] + (fSlow4 * fTemp62));
			fRec97[0] = max((fRec97[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fVbargraph20 = FAUSTFLOAT(fRec97[0]);
			double fTemp64 = (0.026560635762986527 * fTemp63);
			double fTemp65 = (fRec3[0] * double(input11[i]));
			fRec106[0] = (fRec106[1] + fRec105[1]);
			fRec107[0] = (fRec107[1] + fRec106[1]);
			fRec105[0] = (fSlow15 * ((fSlow3 * fTemp65) - ((fSlow17 * fRec106[0]) + (fSlow16 * fRec107[0]))));
			fRec108[0] = (fRec108[1] + fRec104[1]);
			fRec104[0] = (fSlow14 * (fRec105[0] - (fSlow18 * fRec108[0])));
			double fTemp66 = (fRec104[0] + (fSlow4 * fTemp65));
			fRec103[0] = max((fRec103[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fVbargraph21 = FAUSTFLOAT(fRec103[0]);
			double fTemp67 = (0.020573779994945588 * fTemp66);
			double fTemp68 = (fRec3[0] * double(input18[i]));
			fRec112[0] = (fRec112[1] + fRec111[1]);
			fRec113[0] = (fRec113[1] + fRec112[1]);
			fRec111[0] = (fSlow20 * ((fSlow3 * fTemp68) - ((fSlow22 * fRec112[0]) + (fSlow21 * fRec113[0]))));
			fRec114[0] = (fRec114[1] + fRec110[1]);
			fRec115[0] = (fRec115[1] + fRec114[1]);
			fRec110[0] = (fSlow19 * (fRec111[0] - ((fSlow24 * fRec114[0]) + (fSlow23 * fRec115[0]))));
			double fTemp69 = (fRec110[0] + (fSlow4 * fTemp68));
			fRec109[0] = max((fRec109[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp69))))));
			fVbargraph22 = FAUSTFLOAT(fRec109[0]);
			double fTemp70 = (fRec1[0] * (((3.0114518464024883e-18 * fTemp55) + (fTemp58 + (fTemp49 + (fTemp52 + (fTemp48 + fTemp59))))) - ((6.9001030189269641e-18 * fTemp61) + (fTemp64 + ((fTemp67 + (((fTemp50 + fTemp46) + (1.2597806908585981e-18 * fTemp21)) + (4.879109635569025e-18 * fTemp27))) + (2.607993801258108e-18 * fTemp69))))));
			fRec82[0] = max((fRec82[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp70))))));
			fVbargraph23 = FAUSTFLOAT(fRec82[0]);
			output2[i] = FAUSTFLOAT(fTemp70);
			double fTemp71 = (fRec1[0] * (((9.7582192711380499e-18 * fTemp63) + ((2.6935244158092871e-18 * fTemp57) + ((5.2159876025162161e-18 * fTemp69) + (fTemp49 + (fTemp48 + (fTemp51 + (fTemp1 + fTemp46))))))) - ((1.3800206037853928e-17 * fTemp61) + ((6.0229036928049765e-18 * fTemp55) + ((2.5195613817171963e-18 * fTemp66) + (fTemp52 + (fTemp47 + (fTemp45 + fTemp50))))))));
			fRec116[0] = max((fRec116[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fVbargraph24 = FAUSTFLOAT(fRec116[0]);
			output3[i] = FAUSTFLOAT(fTemp71);
			double fTemp72 = (fRec1[0] * ((fTemp64 + ((9.0343555392074636e-18 * fTemp55) + (fTemp67 + (fTemp49 + (fTemp52 + (fTemp48 + ((fTemp1 + (3.7793420725757941e-18 * fTemp21)) + (1.4637328906707076e-17 * fTemp27)))))))) - ((2.0700309056780893e-17 * fTemp61) + (fTemp58 + ((fTemp46 + (fTemp50 + (4.0402866237139308e-18 * fTemp6))) + (7.823981403774323e-18 * fTemp69))))));
			fRec117[0] = max((fRec117[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp72))))));
			fVbargraph25 = FAUSTFLOAT(fRec117[0]);
			output4[i] = FAUSTFLOAT(fTemp72);
			double fTemp73 = (fRec1[0] * ((fTemp43 + (fTemp37 + (fTemp31 + (fTemp28 + (fTemp22 + (fTemp16 + (fTemp10 + fTemp59))))))) - (fTemp40 + (fTemp34 + (fTemp25 + (fTemp19 + (fTemp4 + fTemp13)))))));
			fRec118[0] = max((fRec118[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp73))))));
			fVbargraph26 = FAUSTFLOAT(fRec118[0]);
			output5[i] = FAUSTFLOAT(fTemp73);
			double fTemp74 = (0.022574955908289243 * fTemp0);
			double fTemp75 = (0.027648561470568499 * fTemp3);
			double fTemp76 = (fTemp74 + fTemp75);
			double fTemp77 = (0.027648561470568506 * fTemp6);
			double fTemp78 = (0.012619783999998805 * fTemp9);
			double fTemp79 = (0.043716214137085485 * fTemp12);
			double fTemp80 = (0.021858107068542749 * fTemp15);
			double fTemp81 = (0.038794291597509752 * fTemp21);
			double fTemp82 = (0.04089277388695433 * fTemp24);
			double fTemp83 = (0.016694405031674409 * fTemp27);
			double fTemp84 = (0.013385302265262959 * fTemp33);
			double fTemp85 = (0.047324189999995547 * fTemp36);
			double fTemp86 = (0.035414181017315374 * fTemp39);
			double fTemp87 = (0.012520803773755805 * fTemp42);
			double fTemp88 = (0.010558468816293319 * fTemp18);
			double fTemp89 = (0.027513227513227514 * fTemp30);
			double fTemp90 = (fRec1[0] * ((((((((((((fTemp76 + fTemp77) + fTemp78) + fTemp79) + fTemp80) + fTemp81) + fTemp82) + fTemp83) + fTemp84) + fTemp85) + fTemp86) + fTemp87) - (fTemp88 + fTemp89)));
			fRec119[0] = max((fRec119[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp90))))));
			fVbargraph27 = FAUSTFLOAT(fRec119[0]);
			output6[i] = FAUSTFLOAT(fTemp90);
			double fTemp91 = (fRec3[0] * double(input5[i]));
			fRec123[0] = (fRec123[1] + fRec122[1]);
			fRec124[0] = (fRec124[1] + fRec123[1]);
			fRec122[0] = (fSlow11 * ((fSlow3 * fTemp91) - ((fSlow13 * fRec123[0]) + (fSlow12 * fRec124[0]))));
			double fTemp92 = (fRec122[0] + (fSlow4 * fTemp91));
			fRec121[0] = max((fRec121[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp92))))));
			fVbargraph28 = FAUSTFLOAT(fRec121[0]);
			double fTemp93 = (0.043716214137085485 * fTemp92);
			double fTemp94 = (fRec3[0] * double(input10[i]));
			fRec128[0] = (fRec128[1] + fRec127[1]);
			fRec129[0] = (fRec129[1] + fRec128[1]);
			fRec127[0] = (fSlow15 * ((fSlow3 * fTemp94) - ((fSlow17 * fRec128[0]) + (fSlow16 * fRec129[0]))));
			fRec130[0] = (fRec130[1] + fRec126[1]);
			fRec126[0] = (fSlow14 * (fRec127[0] - (fSlow18 * fRec130[0])));
			double fTemp95 = (fRec126[0] + (fSlow4 * fTemp94));
			fRec125[0] = max((fRec125[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp95))))));
			fVbargraph29 = FAUSTFLOAT(fRec125[0]);
			double fTemp96 = (5.0079204648915091e-18 * fTemp95);
			double fTemp97 = (2.6768460856911014e-18 * fTemp55);
			double fTemp98 = (0.027648561470568506 * fTemp57);
			double fTemp99 = (fRec3[0] * double(input19[i]));
			fRec134[0] = (fRec134[1] + fRec133[1]);
			fRec135[0] = (fRec135[1] + fRec134[1]);
			fRec133[0] = (fSlow20 * ((fSlow3 * fTemp99) - ((fSlow22 * fRec134[0]) + (fSlow21 * fRec135[0]))));
			fRec136[0] = (fRec136[1] + fRec132[1]);
			fRec137[0] = (fRec137[1] + fRec136[1]);
			fRec132[0] = (fSlow19 * (fRec133[0] - ((fSlow24 * fRec136[0]) + (fSlow23 * fRec137[0]))));
			double fTemp100 = (fRec132[0] + (fSlow4 * fTemp99));
			fRec131[0] = max((fRec131[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp100))))));
			fVbargraph30 = FAUSTFLOAT(fRec131[0]);
			double fTemp101 = (0.013385302265262959 * fTemp100);
			double fTemp102 = (5.7955417805735729e-18 * fTemp69);
			double fTemp103 = (0.038794291597509752 * fTemp66);
			double fTemp104 = (1.6929861152980278e-18 * fTemp6);
			double fTemp105 = (2.6768460856911007e-18 * fTemp12);
			double fTemp106 = (2.3754652515039685e-18 * fTemp21);
			double fTemp107 = (8.1961337873870493e-19 * fTemp33);
			double fTemp108 = (3.0667124528564294e-18 * (fTemp27 + fTemp61));
			double fTemp109 = (fRec3[0] * double(input17[i]));
			fRec141[0] = (fRec141[1] + fRec140[1]);
			fRec142[0] = (fRec142[1] + fRec141[1]);
			fRec140[0] = (fSlow20 * ((fSlow3 * fTemp109) - ((fSlow22 * fRec141[0]) + (fSlow21 * fRec142[0]))));
			fRec143[0] = (fRec143[1] + fRec139[1]);
			fRec144[0] = (fRec144[1] + fRec143[1]);
			fRec139[0] = (fSlow19 * (fRec140[0] - ((fSlow24 * fRec143[0]) + (fSlow23 * fRec144[0]))));
			double fTemp110 = (fRec139[0] + (fSlow4 * fTemp109));
			fRec138[0] = max((fRec138[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp110))))));
			fVbargraph31 = FAUSTFLOAT(fRec138[0]);
			double fTemp111 = (0.035414181017315374 * fTemp110);
			double fTemp112 = (0.016694405031674409 * fTemp63);
			double fTemp113 = (6.5054795140920346e-18 * fTemp39);
			double fTemp114 = (fRec1[0] * ((fTemp93 + (fTemp96 + (fTemp97 + (fTemp98 + (fTemp101 + (fTemp102 + (fTemp103 + (fTemp87 + ((((fTemp78 + (fTemp76 + fTemp104)) + fTemp105) + fTemp106) + fTemp107))))))))) - (fTemp108 + (fTemp111 + (fTemp112 + ((fTemp85 + (fTemp89 + (fTemp82 + (fTemp80 + fTemp88)))) + fTemp113))))));
			fRec120[0] = max((fRec120[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp114))))));
			fVbargraph32 = FAUSTFLOAT(fRec120[0]);
			output7[i] = FAUSTFLOAT(fTemp114);
			double fTemp115 = (6.1334249057128588e-18 * (fTemp63 - fTemp61));
			double fTemp116 = (5.3536921713822013e-18 * fTemp92);
			double fTemp117 = (1.3010959028184069e-17 * fTemp110);
			double fTemp118 = (3.3859722305960555e-18 * fTemp57);
			double fTemp119 = (1.6392267574774099e-18 * fTemp100);
			double fTemp120 = (4.7509305030079371e-18 * fTemp66);
			double fTemp121 = (fTemp76 + fTemp78);
			double fTemp122 = (1.0015840929783018e-17 * fTemp95);
			double fTemp123 = (5.3536921713822029e-18 * fTemp55);
			double fTemp124 = (1.1591083561147146e-17 * fTemp69);
			double fTemp125 = (fRec1[0] * ((fTemp115 + (fTemp116 + (fTemp117 + (fTemp118 + (fTemp119 + (fTemp120 + (fTemp87 + (fTemp85 + (fTemp82 + (fTemp80 + fTemp121)))))))))) - (fTemp122 + (fTemp123 + (fTemp124 + (fTemp86 + (fTemp84 + (fTemp89 + (fTemp83 + (fTemp81 + (fTemp88 + (fTemp77 + fTemp79))))))))))));
			fRec145[0] = max((fRec145[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp125))))));
			fVbargraph33 = FAUSTFLOAT(fRec145[0]);
			output8[i] = FAUSTFLOAT(fTemp125);
			double fTemp126 = (1.5023761394674526e-17 * fTemp95);
			double fTemp127 = (8.0305382570733035e-18 * fTemp55);
			double fTemp128 = (1.7386625341720719e-17 * fTemp69);
			double fTemp129 = (9.2001373585692901e-18 * fTemp27);
			double fTemp130 = (1.9516438542276106e-17 * fTemp39);
			double fTemp131 = (9.200137358569287e-18 * fTemp61);
			double fTemp132 = (5.0789583458940831e-18 * fTemp6);
			double fTemp133 = (8.0305382570733005e-18 * fTemp12);
			double fTemp134 = (7.1263957545119048e-18 * fTemp21);
			double fTemp135 = (2.4588401362161147e-18 * fTemp33);
			double fTemp136 = (fRec1[0] * ((fTemp126 + (fTemp111 + (fTemp112 + (fTemp127 + (fTemp128 + (fTemp87 + ((fTemp121 + fTemp129) + fTemp130))))))) - (fTemp93 + (fTemp131 + (fTemp98 + (fTemp101 + (fTemp103 + (fTemp85 + ((fTemp89 + (fTemp82 + ((fTemp88 + (fTemp80 + (fTemp132 + fTemp133))) + fTemp134))) + fTemp135)))))))));
			fRec146[0] = max((fRec146[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp136))))));
			fVbargraph34 = FAUSTFLOAT(fRec146[0]);
			output9[i] = FAUSTFLOAT(fTemp136);
			double fTemp137 = (0.043716214137085485 * fTemp55);
			double fTemp138 = (0.027648561470568499 * fTemp57);
			double fTemp139 = (fTemp74 + fTemp77);
			double fTemp140 = (0.025396825396825397 * fTemp30);
			double fTemp141 = (0.037859351999996446 * fTemp69);
			double fTemp142 = (0.025862861065006505 * fTemp66);
			double fTemp143 = (0.025239567999997631 * fTemp9);
			double fTemp144 = (0.025862861065006511 * fTemp21);
			double fTemp145 = (0.050083215095023212 * fTemp42);
			double fTemp146 = (fRec1[0] * (((6.133424905712858e-18 * fTemp61) + (fTemp137 + (fTemp138 + ((fTemp139 + (2.6768460856911007e-18 * fTemp15)) + fTemp140)))) - ((0.033388810063348803 * (fTemp27 - fTemp63)) + (fTemp141 + (fTemp142 + (((fTemp143 + fTemp144) + (2.3182167122294292e-18 * fTemp36)) + fTemp145))))));
			fRec147[0] = max((fRec147[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp146))))));
			fVbargraph35 = FAUSTFLOAT(fRec147[0]);
			output10[i] = FAUSTFLOAT(fTemp146);
			double fTemp147 = (0.025862861065006511 * fTemp66);
			double fTemp148 = (fRec1[0] * (((1.8400274717138574e-17 * fTemp61) + ((0.033388810063348796 * fTemp63) + (fTemp98 + (fTemp141 + ((fTemp140 + ((fTemp74 + (0.025862861065006505 * fTemp21)) + (0.033388810063348817 * fTemp27))) + (6.9546501366882881e-18 * fTemp36)))))) - (fTemp137 + (fTemp147 + (fTemp145 + ((fTemp143 + (0.027648561470568499 * fTemp6)) + (8.0305382570733005e-18 * fTemp15)))))));
			fRec148[0] = max((fRec148[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp148))))));
			fVbargraph36 = FAUSTFLOAT(fRec148[0]);
			output11[i] = FAUSTFLOAT(fTemp148);
			double fTemp149 = (fRec1[0] * (((3.0667124528564293e-17 * fTemp61) + (fTemp137 + (fTemp142 + (fTemp140 + ((fTemp144 + (fTemp74 + (1.3384230428455503e-17 * fTemp15))) + (0.033388810063348762 * fTemp27)))))) - ((0.033388810063348852 * fTemp63) + (fTemp138 + (fTemp141 + (fTemp145 + ((fTemp77 + fTemp143) + (1.1591083561147147e-17 * fTemp36))))))));
			fRec149[0] = max((fRec149[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp149))))));
			fVbargraph37 = FAUSTFLOAT(fRec149[0]);
			output12[i] = FAUSTFLOAT(fTemp149);
			double fTemp150 = (fRec1[0] * (((4.2933974339990009e-17 * fTemp61) + (fTemp141 + (fTemp147 + ((fTemp140 + (fTemp74 + (0.027648561470568496 * fTemp6))) + (1.6227516985606004e-17 * fTemp36))))) - ((0.033388810063348817 * fTemp63) + (fTemp137 + (fTemp98 + (fTemp145 + (((fTemp143 + (1.8737922599837705e-17 * fTemp15)) + (0.025862861065006501 * fTemp21)) + (0.033388810063348796 * fTemp27))))))));
			fRec150[0] = max((fRec150[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp150))))));
			fVbargraph38 = FAUSTFLOAT(fRec150[0]);
			output13[i] = FAUSTFLOAT(fTemp150);
			double fTemp151 = (fRec1[0] * ((fTemp87 + (fTemp85 + (fTemp83 + (fTemp81 + (fTemp88 + (fTemp80 + (fTemp78 + fTemp139))))))) - (fTemp86 + (fTemp84 + (fTemp89 + (fTemp82 + (fTemp75 + fTemp79)))))));
			fRec151[0] = max((fRec151[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp151))))));
			fVbargraph39 = FAUSTFLOAT(fRec151[0]);
			output14[i] = FAUSTFLOAT(fTemp151);
			double fTemp152 = (fRec1[0] * ((fTemp111 + (fTemp97 + (fTemp98 + (fTemp102 + (fTemp103 + (fTemp87 + (fTemp113 + (fTemp82 + (fTemp106 + (fTemp88 + (fTemp78 + (fTemp74 + fTemp104)))))))))))) - (fTemp108 + (fTemp93 + (fTemp96 + (fTemp112 + (fTemp101 + (fTemp85 + (fTemp107 + (fTemp89 + (fTemp80 + (fTemp75 + fTemp105))))))))))));
			fRec152[0] = max((fRec152[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp152))))));
			fVbargraph40 = FAUSTFLOAT(fRec152[0]);
			output15[i] = FAUSTFLOAT(fTemp152);
			double fTemp153 = (fTemp74 + fTemp78);
			double fTemp154 = (fRec1[0] * ((fTemp115 + (fTemp122 + (fTemp118 + (fTemp120 + (fTemp87 + (fTemp86 + (fTemp85 + (fTemp84 + (fTemp88 + (fTemp80 + (fTemp79 + fTemp153))))))))))) - (fTemp116 + (fTemp117 + (fTemp123 + (fTemp119 + (fTemp124 + (fTemp89 + (fTemp83 + (fTemp82 + (fTemp81 + (fTemp75 + fTemp77))))))))))));
			fRec153[0] = max((fRec153[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp154))))));
			fVbargraph41 = FAUSTFLOAT(fRec153[0]);
			output16[i] = FAUSTFLOAT(fTemp154);
			double fTemp155 = (fRec1[0] * ((fTemp93 + (fTemp112 + (fTemp127 + (fTemp101 + (fTemp128 + (fTemp87 + (fTemp135 + (fTemp129 + (fTemp82 + (fTemp88 + (fTemp133 + fTemp153))))))))))) - (fTemp126 + (fTemp111 + (fTemp131 + (fTemp98 + (fTemp103 + (fTemp130 + (fTemp85 + (fTemp89 + (fTemp134 + (fTemp80 + (fTemp75 + fTemp132)))))))))))));
			fRec154[0] = max((fRec154[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp155))))));
			fVbargraph42 = FAUSTFLOAT(fRec154[0]);
			output17[i] = FAUSTFLOAT(fTemp155);
			double fTemp156 = (fTemp0 + fTemp3);
			double fTemp157 = (0.021093750000000001 * fTemp156);
			double fTemp158 = (0.027231914153020904 * fTemp92);
			double fTemp159 = (0.041597435974919175 * fTemp95);
			double fTemp160 = (0.029413829057838591 * fTemp110);
			double fTemp161 = (2.5471083409710855e-18 * fTemp61);
			double fTemp162 = (0.016982082124440745 * fTemp63);
			double fTemp163 = (0.027231914153020897 * fTemp55);
			double fTemp164 = (0.021093749999999994 * fTemp57);
			double fTemp165 = (0.031444705933590805 * fTemp69);
			double fTemp166 = (0.013154264250377145 * fTemp66);
			double fTemp167 = (5.236595479505967e-18 * fTemp9);
			double fTemp168 = ((0.021093749999999998 * fTemp6) + fTemp167);
			double fTemp169 = (0.027231914153020911 * fTemp12);
			double fTemp170 = (1.6674738251076273e-18 * fTemp15);
			double fTemp171 = (0.013154264250377147 * fTemp21);
			double fTemp172 = (2.5471083409710867e-18 * fTemp24);
			double fTemp173 = (1.9254329235850881e-18 * fTemp36);
			double fTemp174 = (0.01111738239902945 * (fTemp33 + fTemp100));
			double fTemp175 = (0.021480823570105497 * fTemp18);
			double fTemp176 = (0.016982082124440741 * fTemp27);
			double fTemp177 = (0.024609375000000013 * fTemp30);
			double fTemp178 = (0.029413829057838584 * fTemp39);
			double fTemp179 = (0.020798717987459581 * fTemp42);
			double fTemp180 = (fRec1[0] * ((fTemp157 + (fTemp158 + (fTemp159 + (fTemp160 + (fTemp161 + (fTemp162 + (fTemp163 + (fTemp164 + (fTemp165 + (fTemp166 + (((((fTemp168 + fTemp169) + fTemp170) + fTemp171) + fTemp172) + fTemp173))))))))))) - (fTemp174 + ((((fTemp175 + fTemp176) + fTemp177) + fTemp178) + fTemp179))));
			fRec155[0] = max((fRec155[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp180))))));
			fVbargraph43 = FAUSTFLOAT(fRec155[0]);
			output18[i] = FAUSTFLOAT(fTemp180);
			double fTemp181 = (0.01111738239902945 * (fTemp33 - fTemp100));
			double fTemp182 = (0.027231914153020911 * fTemp92);
			double fTemp183 = (0.02941382905783858 * fTemp110);
			double fTemp184 = (7.6413250229132566e-18 * fTemp61);
			double fTemp185 = (0.016982082124440741 * fTemp63);
			double fTemp186 = (0.021093749999999998 * fTemp57);
			double fTemp187 = (0.013154264250377147 * fTemp66);
			double fTemp188 = (0.016982082124440748 * fTemp27);
			double fTemp189 = (0.029413829057838594 * fTemp39);
			double fTemp190 = (0.021093749999999994 * fTemp6);
			double fTemp191 = (0.027231914153020904 * fTemp12);
			double fTemp192 = (5.0024214753228816e-18 * fTemp15);
			double fTemp193 = (0.013154264250377145 * fTemp21);
			double fTemp194 = (7.6413250229132581e-18 * fTemp24);
			double fTemp195 = (5.7762987707552641e-18 * fTemp36);
			double fTemp196 = (fRec1[0] * ((fTemp181 + (fTemp157 + (fTemp182 + (fTemp183 + (fTemp184 + (fTemp185 + (fTemp186 + (fTemp187 + ((fTemp167 + fTemp188) + fTemp189))))))))) - (fTemp159 + (fTemp163 + (fTemp165 + (fTemp179 + ((fTemp177 + (((fTemp175 + ((fTemp190 + fTemp191) + fTemp192)) + fTemp193) + fTemp194)) + fTemp195)))))));
			fRec156[0] = max((fRec156[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp196))))));
			fVbargraph44 = FAUSTFLOAT(fRec156[0]);
			output19[i] = FAUSTFLOAT(fTemp196);
			double fTemp197 = (1.2735541704855428e-17 * fTemp61);
			double fTemp198 = (0.01111738239902945 * fTemp100);
			double fTemp199 = (8.3373691255381378e-18 * fTemp15);
			double fTemp200 = (1.2735541704855431e-17 * fTemp24);
			double fTemp201 = (0.016982082124440721 * fTemp27);
			double fTemp202 = (0.011117382399029452 * fTemp33);
			double fTemp203 = (9.6271646179254396e-18 * fTemp36);
			double fTemp204 = (0.029413829057838549 * fTemp39);
			double fTemp205 = (0.029413829057838629 * fTemp110);
			double fTemp206 = (0.016982082124440766 * fTemp63);
			double fTemp207 = (0.013154264250377148 * fTemp21);
			double fTemp208 = (fRec1[0] * ((fTemp159 + (fTemp197 + (fTemp163 + (fTemp198 + (fTemp165 + ((((((fTemp167 + fTemp199) + fTemp200) + fTemp201) + fTemp202) + fTemp203) + fTemp204)))))) - ((0.021093750000000001 * (fTemp6 - fTemp156)) + (fTemp158 + (fTemp205 + (fTemp206 + (fTemp164 + (fTemp166 + (fTemp179 + (fTemp177 + ((fTemp169 + fTemp175) + fTemp207)))))))))));
			fRec157[0] = max((fRec157[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp208))))));
			fVbargraph45 = FAUSTFLOAT(fRec157[0]);
			output20[i] = FAUSTFLOAT(fTemp208);
			double fTemp209 = (fTemp0 - fTemp57);
			double fTemp210 = (1.7829758386797597e-17 * fTemp61);
			double fTemp211 = (fTemp167 + (0.021093749999999991 * fTemp6));
			double fTemp212 = (0.027231914153020901 * fTemp12);
			double fTemp213 = (0.011117382399029452 * fTemp100);
			double fTemp214 = (0.029413829057838594 * fTemp110);
			double fTemp215 = (0.016982082124440748 * fTemp63);
			double fTemp216 = (0.013154264250377148 * fTemp66);
			double fTemp217 = (1.167231677575339e-17 * fTemp15);
			double fTemp218 = (1.7829758386797603e-17 * fTemp24);
			double fTemp219 = (0.011117382399029449 * fTemp33);
			double fTemp220 = (1.3478030465095617e-17 * fTemp36);
			double fTemp221 = (0.02941382905783858 * fTemp39);
			double fTemp222 = (fRec1[0] * (((0.021093750000000001 * (fTemp3 + fTemp209)) + (fTemp210 + ((fTemp193 + (fTemp211 + fTemp212)) + fTemp213))) - (fTemp182 + (fTemp159 + (fTemp214 + (fTemp215 + (fTemp163 + (fTemp165 + (fTemp216 + (fTemp179 + ((((fTemp177 + (fTemp176 + ((fTemp175 + fTemp217) + fTemp218))) + fTemp219) + fTemp220) + fTemp221)))))))))));
			fRec158[0] = max((fRec158[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp222))))));
			fVbargraph46 = FAUSTFLOAT(fRec158[0]);
			output21[i] = FAUSTFLOAT(fTemp222);
			double fTemp223 = (fTemp0 - fTemp3);
			double fTemp224 = (0.021093750000000001 * fTemp223);
			double fTemp225 = (fRec1[0] * ((fTemp224 + (fTemp174 + (fTemp161 + (fTemp162 + (fTemp163 + (fTemp164 + (fTemp165 + (fTemp166 + (fTemp178 + (fTemp173 + (fTemp171 + (fTemp175 + (fTemp168 + fTemp170))))))))))))) - (fTemp158 + (fTemp159 + (fTemp160 + (fTemp179 + (fTemp177 + (fTemp176 + (fTemp169 + fTemp172)))))))));
			fRec159[0] = max((fRec159[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp225))))));
			fVbargraph47 = FAUSTFLOAT(fRec159[0]);
			output22[i] = FAUSTFLOAT(fTemp225);
			double fTemp226 = (fRec1[0] * ((fTemp224 + (fTemp159 + (fTemp184 + (fTemp185 + (fTemp186 + (fTemp187 + (fTemp188 + (fTemp194 + (fTemp175 + (fTemp167 + fTemp191)))))))))) - (fTemp181 + (fTemp182 + (fTemp183 + (fTemp163 + (fTemp165 + (fTemp179 + (fTemp189 + (fTemp195 + (fTemp177 + (fTemp193 + (fTemp190 + fTemp192)))))))))))));
			fRec160[0] = max((fRec160[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp226))))));
			fVbargraph48 = FAUSTFLOAT(fRec160[0]);
			output23[i] = FAUSTFLOAT(fTemp226);
			double fTemp227 = (fRec1[0] * ((fTemp158 + (fTemp205 + (fTemp197 + (fTemp163 + (fTemp165 + (fTemp203 + (fTemp201 + (fTemp175 + (fTemp199 + (fTemp167 + fTemp169)))))))))) - ((0.021093750000000001 * (fTemp6 - fTemp223)) + (fTemp159 + (fTemp206 + (fTemp164 + (fTemp198 + (fTemp166 + (fTemp179 + (fTemp204 + (fTemp202 + (fTemp177 + (fTemp207 + fTemp200)))))))))))));
			fRec161[0] = max((fRec161[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp227))))));
			fVbargraph49 = FAUSTFLOAT(fRec161[0]);
			output24[i] = FAUSTFLOAT(fTemp227);
			double fTemp228 = (fRec1[0] * ((fTemp182 + (fTemp159 + (fTemp214 + (fTemp210 + (fTemp221 + (fTemp219 + (fTemp218 + (fTemp193 + (fTemp175 + fTemp211))))))))) - ((0.021093750000000001 * (fTemp3 - fTemp209)) + (fTemp215 + (fTemp163 + (fTemp213 + (fTemp165 + (fTemp216 + (fTemp179 + (fTemp220 + (fTemp177 + (fTemp176 + (fTemp212 + fTemp217)))))))))))));
			fRec162[0] = max((fRec162[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp228))))));
			fVbargraph50 = FAUSTFLOAT(fRec162[0]);
			output25[i] = FAUSTFLOAT(fTemp228);
			double fTemp229 = (0.021308452520676528 * (fTemp12 + fTemp92));
			double fTemp230 = (0.010535180889746065 * (fTemp6 + fTemp57));
			double fTemp231 = (0.016998264729033836 * fTemp95);
			double fTemp232 = (0.0062770252486745223 * fTemp110);
			double fTemp233 = (0.0023131708388267431 * fTemp63);
			double fTemp234 = (0.0071028175068921654 * fTemp55);
			double fTemp235 = (0.035587388102713349 * fTemp100);
			double fTemp236 = (0.02907849643359095 * fTemp69);
			double fTemp237 = (0.030460165261409405 * fTemp66);
			double fTemp238 = (0.02017333553791887 * fTemp0);
			double fTemp239 = (0.031605542669238249 * fTemp3);
			double fTemp240 = (fTemp238 + fTemp239);
			double fTemp241 = (0.032806508796871948 * fTemp9);
			double fTemp242 = (fTemp240 + fTemp241);
			double fTemp243 = (0.0263335984839893 * fTemp18);
			double fTemp244 = (fTemp242 + fTemp243);
			double fTemp245 = (0.030460165261409402 * fTemp21);
			double fTemp246 = (0.014254712301587407 * fTemp30);
			double fTemp247 = (0.035587388102713342 * fTemp33);
			double fTemp248 = (4.7584639941967359e-19 * fTemp61);
			double fTemp249 = (1.1422201709353029e-18 * fTemp15);
			double fTemp250 = (2.7335294515958808e-18 * fTemp24);
			double fTemp251 = (0.0023131708388267449 * fTemp27);
			double fTemp252 = (4.6761788733398907e-18 * fTemp36);
			double fTemp253 = (0.0062770252486745275 * fTemp39);
			double fTemp254 = (0.0014795090396723079 * fTemp42);
			double fTemp255 = (fRec1[0] * ((fTemp229 + (fTemp230 + (fTemp231 + (fTemp232 + (fTemp233 + (fTemp234 + (fTemp235 + (fTemp236 + (fTemp237 + (((fTemp244 + fTemp245) + fTemp246) + fTemp247)))))))))) - (fTemp248 + (((((fTemp249 + fTemp250) + fTemp251) + fTemp252) + fTemp253) + fTemp254))));
			fRec163[0] = max((fRec163[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp255))))));
			fVbargraph51 = FAUSTFLOAT(fRec163[0]);
			output26[i] = FAUSTFLOAT(fTemp255);
			double fTemp256 = (0.0062770252486745231 * fTemp110);
			double fTemp257 = (5.4356280292327993e-19 * fTemp61);
			double fTemp258 = (0.0023131708388267444 * fTemp27);
			double fTemp259 = (0.0062770252486745266 * fTemp39);
			double fTemp260 = (0.021308452520676528 * (fTemp12 - fTemp92));
			double fTemp261 = (0.010535180889746065 * (fTemp6 - fTemp57));
			double fTemp262 = (1.3047664087114909e-18 * fTemp15);
			double fTemp263 = (3.1225305737205953e-18 * fTemp24);
			double fTemp264 = (5.3416331372122328e-18 * fTemp36);
			double fTemp265 = (fRec1[0] * ((fTemp256 + (fTemp257 + (fTemp233 + (fTemp235 + (fTemp237 + ((fTemp246 + (fTemp244 + fTemp258)) + fTemp259)))))) - (fTemp260 + (fTemp261 + (fTemp231 + (fTemp234 + (fTemp236 + (fTemp254 + ((fTemp247 + ((fTemp245 + fTemp262) + fTemp263)) + fTemp264)))))))));
			fRec164[0] = max((fRec164[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp265))))));
			fVbargraph52 = FAUSTFLOAT(fRec164[0]);
			output27[i] = FAUSTFLOAT(fTemp265);
			double fTemp266 = (9.0593800487213328e-19 * fTemp61);
			double fTemp267 = (2.1746106811858183e-18 * fTemp15);
			double fTemp268 = (5.2042176228676594e-18 * fTemp24);
			double fTemp269 = (0.0023131708388267409 * fTemp27);
			double fTemp270 = (8.902721895353721e-18 * fTemp36);
			double fTemp271 = (0.0062770252486745171 * fTemp39);
			double fTemp272 = (0.021308452520676528 * fTemp92);
			double fTemp273 = (0.0062770252486745335 * fTemp110);
			double fTemp274 = (0.002313170838826747 * fTemp63);
			double fTemp275 = (0.010535180889746065 * fTemp57);
			double fTemp276 = (0.035587388102713342 * fTemp100);
			double fTemp277 = (0.030460165261409402 * fTemp66);
			double fTemp278 = (0.010535180889746067 * fTemp6);
			double fTemp279 = (0.021308452520676532 * fTemp12);
			double fTemp280 = (0.030460165261409412 * fTemp21);
			double fTemp281 = (0.035587388102713349 * fTemp33);
			double fTemp282 = (fRec1[0] * ((fTemp231 + (fTemp266 + (fTemp234 + (fTemp236 + (((fTemp246 + (((fTemp243 + (fTemp242 + fTemp267)) + fTemp268) + fTemp269)) + fTemp270) + fTemp271))))) - (fTemp272 + (fTemp273 + (fTemp274 + (fTemp275 + (fTemp276 + (fTemp277 + (fTemp254 + (((fTemp278 + fTemp279) + fTemp280) + fTemp281))))))))));
			fRec165[0] = max((fRec165[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp282))))));
			fVbargraph53 = FAUSTFLOAT(fRec165[0]);
			output28[i] = FAUSTFLOAT(fTemp282);
			double fTemp283 = (1.2683132068209864e-18 * fTemp61);
			double fTemp284 = (0.010535180889746063 * fTemp6);
			double fTemp285 = (0.021308452520676525 * fTemp12);
			double fTemp286 = (0.030460165261409398 * fTemp21);
			double fTemp287 = (0.035587388102713335 * fTemp33);
			double fTemp288 = (0.021308452520676532 * fTemp92);
			double fTemp289 = (0.0062770252486745266 * fTemp110);
			double fTemp290 = (0.0023131708388267444 * fTemp63);
			double fTemp291 = (0.010535180889746067 * fTemp57);
			double fTemp292 = (0.030460165261409412 * fTemp66);
			double fTemp293 = (3.0444549536601452e-18 * fTemp15);
			double fTemp294 = (7.2859046720147234e-18 * fTemp24);
			double fTemp295 = (0.0023131708388267431 * fTemp27);
			double fTemp296 = (1.246381065349521e-17 * fTemp36);
			double fTemp297 = (0.0062770252486745231 * fTemp39);
			double fTemp298 = (fRec1[0] * ((fTemp283 + ((fTemp246 + ((fTemp243 + ((fTemp241 + (fTemp240 + fTemp284)) + fTemp285)) + fTemp286)) + fTemp287)) - (fTemp288 + (fTemp231 + (fTemp289 + (fTemp290 + (fTemp234 + (fTemp291 + (fTemp235 + (fTemp236 + (fTemp292 + (fTemp254 + ((((fTemp293 + fTemp294) + fTemp295) + fTemp296) + fTemp297)))))))))))));
			fRec166[0] = max((fRec166[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp298))))));
			fVbargraph54 = FAUSTFLOAT(fRec166[0]);
			output29[i] = FAUSTFLOAT(fTemp298);
			double fTemp299 = (0.0071028175068921914 * fTemp92);
			double fTemp300 = (0.016998264729033888 * fTemp95);
			double fTemp301 = (0.027200442744256398 * fTemp110);
			double fTemp302 = (0.035508216952135616 * fTemp61);
			double fTemp303 = (0.030071220904747824 * fTemp63);
			double fTemp304 = (0.021308452520676577 * fTemp55);
			double fTemp305 = (0.010535180889746094 * fTemp57);
			double fTemp306 = (0.010535180889746075 * fTemp3);
			double fTemp307 = (fTemp238 + fTemp306);
			double fTemp308 = (0.031605542669238242 * fTemp6);
			double fTemp309 = (0.021308452520676542 * fTemp12);
			double fTemp310 = (0.028411270027568724 * fTemp15);
			double fTemp311 = (0.022664352972045144 * fTemp24);
			double fTemp312 = (0.020818537549440738 * fTemp27);
			double fTemp313 = (0.0042514054232804356 * fTemp30);
			double fTemp314 = (0.018831075746023603 * fTemp39);
			double fTemp315 = (0.010356563277706152 * fTemp42);
			double fTemp316 = (0.010280801007450534 * fTemp100);
			double fTemp317 = (0.006710422253905635 * fTemp69);
			double fTemp318 = (0.0053753232814252047 * fTemp66);
			double fTemp319 = (0.01640325439843596 * fTemp9);
			double fTemp320 = (0.020481687709769415 * fTemp18);
			double fTemp321 = (fTemp319 + fTemp320);
			double fTemp322 = (0.016125969844275591 * fTemp21);
			double fTemp323 = (0.030842403022351559 * fTemp33);
			double fTemp324 = (0.0089472296718741632 * fTemp36);
			double fTemp325 = (fRec1[0] * ((fTemp299 + (fTemp300 + (fTemp301 + (fTemp302 + (fTemp303 + (fTemp304 + (fTemp305 + ((((((((fTemp307 + fTemp308) + fTemp309) + fTemp310) + fTemp311) + fTemp312) + fTemp313) + fTemp314) + fTemp315)))))))) - (fTemp316 + (fTemp317 + (fTemp318 + (((fTemp321 + fTemp322) + fTemp323) + fTemp324))))));
			fRec167[0] = max((fRec167[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp325))))));
			fVbargraph55 = FAUSTFLOAT(fRec167[0]);
			output30[i] = FAUSTFLOAT(fTemp325);
			double fTemp326 = (0.021308452520676546 * fTemp92);
			double fTemp327 = (0.016998264729033867 * fTemp95);
			double fTemp328 = (0.021308452520676556 * fTemp55);
			double fTemp329 = (0.031605542669238242 * fTemp57);
			double fTemp330 = (0.010535180889746081 * fTemp6);
			double fTemp331 = (0.0071028175068921827 * fTemp12);
			double fTemp332 = (0.0089472296718741701 * fTemp36);
			double fTemp333 = (0.010356563277706217 * fTemp42);
			double fTemp334 = (0.018831075746023641 * fTemp110);
			double fTemp335 = (0.035508216952135595 * fTemp61);
			double fTemp336 = (0.020818537549440783 * fTemp63);
			double fTemp337 = (0.030842403022351562 * fTemp100);
			double fTemp338 = (0.0067104222539056272 * fTemp69);
			double fTemp339 = (0.016125969844275591 * fTemp66);
			double fTemp340 = (0.028411270027568741 * fTemp15);
			double fTemp341 = (0.0053753232814251978 * fTemp21);
			double fTemp342 = (0.022664352972045162 * fTemp24);
			double fTemp343 = (0.030071220904747797 * fTemp27);
			double fTemp344 = (0.010280801007450522 * fTemp33);
			double fTemp345 = (0.027200442744256374 * fTemp39);
			double fTemp346 = (fRec1[0] * ((fTemp326 + (fTemp327 + (fTemp328 + (fTemp329 + (((fTemp313 + ((fTemp307 + fTemp330) + fTemp331)) + fTemp332) + fTemp333))))) - (fTemp334 + (fTemp335 + (fTemp336 + (fTemp337 + (fTemp338 + (fTemp339 + ((((((fTemp320 + (fTemp319 + fTemp340)) + fTemp341) + fTemp342) + fTemp343) + fTemp344) + fTemp345)))))))));
			fRec168[0] = max((fRec168[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp346))))));
			fVbargraph56 = FAUSTFLOAT(fRec168[0]);
			output31[i] = FAUSTFLOAT(fTemp346);
			double fTemp347 = (0.021308452520676553 * fTemp92);
			double fTemp348 = (0.0067104222539056254 * fTemp69);
			double fTemp349 = (0.0053753232814251969 * fTemp21);
			double fTemp350 = (0.03007122090474779 * fTemp27);
			double fTemp351 = (0.010280801007450517 * fTemp33);
			double fTemp352 = (0.0089472296718741719 * fTemp36);
			double fTemp353 = (0.027200442744256363 * fTemp39);
			double fTemp354 = (0.010356563277706232 * fTemp42);
			double fTemp355 = (0.016998264729033864 * fTemp95);
			double fTemp356 = (0.018831075746023648 * fTemp110);
			double fTemp357 = (0.02081853754944079 * fTemp63);
			double fTemp358 = (0.021308452520676546 * fTemp55);
			double fTemp359 = (0.016125969844275594 * fTemp66);
			double fTemp360 = (0.010535180889746079 * fTemp6);
			double fTemp361 = (0.007102817506892181 * fTemp12);
			double fTemp362 = (0.028411270027568745 * fTemp15);
			double fTemp363 = (0.030842403022351569 * fTemp100);
			double fTemp364 = (fRec1[0] * ((fTemp347 + (fTemp335 + (fTemp329 + (fTemp348 + (((((fTemp313 + ((fTemp307 + fTemp349) + fTemp350)) + fTemp351) + fTemp352) + fTemp353) + fTemp354))))) - (fTemp355 + (fTemp356 + (fTemp357 + (fTemp358 + ((fTemp359 + (fTemp342 + (fTemp320 + (((fTemp319 + fTemp360) + fTemp361) + fTemp362)))) + fTemp363)))))));
			fRec169[0] = max((fRec169[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp364))))));
			fVbargraph57 = FAUSTFLOAT(fRec169[0]);
			output32[i] = FAUSTFLOAT(fTemp364);
			double fTemp365 = (0.0071028175068921931 * fTemp92);
			double fTemp366 = (0.027200442744256408 * fTemp110);
			double fTemp367 = (0.030071220904747838 * fTemp63);
			double fTemp368 = (0.010535180889746096 * fTemp57);
			double fTemp369 = (0.0067104222539056367 * fTemp69);
			double fTemp370 = (0.028411270027568717 * fTemp15);
			double fTemp371 = (0.022664352972045138 * fTemp24);
			double fTemp372 = (0.010356563277706145 * fTemp42);
			double fTemp373 = (0.021308452520676584 * fTemp55);
			double fTemp374 = (0.010280801007450538 * fTemp100);
			double fTemp375 = (0.0053753232814252056 * fTemp66);
			double fTemp376 = (fTemp308 + fTemp319);
			double fTemp377 = (0.020818537549440724 * fTemp27);
			double fTemp378 = (0.018831075746023586 * fTemp39);
			double fTemp379 = (fRec1[0] * ((fTemp365 + (fTemp366 + (fTemp367 + (fTemp368 + (fTemp369 + ((fTemp323 + (fTemp313 + ((fTemp322 + (fTemp307 + fTemp370)) + fTemp371))) + fTemp372)))))) - (fTemp300 + (fTemp302 + (fTemp373 + (fTemp374 + (fTemp375 + ((fTemp324 + ((fTemp320 + (fTemp309 + fTemp376)) + fTemp377)) + fTemp378))))))));
			fRec170[0] = max((fRec170[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp379))))));
			fVbargraph58 = FAUSTFLOAT(fRec170[0]);
			output33[i] = FAUSTFLOAT(fTemp379);
			double fTemp380 = (0.016998264729033881 * fTemp95);
			double fTemp381 = (0.035508216952135609 * fTemp61);
			double fTemp382 = (0.02130845252067657 * fTemp55);
			double fTemp383 = (0.010280801007450529 * fTemp100);
			double fTemp384 = (0.0053753232814252021 * fTemp66);
			double fTemp385 = (0.028411270027568731 * fTemp15);
			double fTemp386 = (0.022664352972045151 * fTemp24);
			double fTemp387 = (0.030842403022351562 * fTemp33);
			double fTemp388 = (0.010356563277706178 * fTemp42);
			double fTemp389 = (0.0071028175068921871 * fTemp92);
			double fTemp390 = (0.030071220904747821 * fTemp63);
			double fTemp391 = (0.010535180889746089 * fTemp57);
			double fTemp392 = (0.0067104222539056315 * fTemp69);
			double fTemp393 = (0.021308452520676546 * fTemp12);
			double fTemp394 = (0.020818537549440744 * fTemp27);
			double fTemp395 = (0.0089472296718741667 * fTemp36);
			double fTemp396 = (0.01883107574602361 * fTemp39);
			double fTemp397 = (fRec1[0] * ((fTemp380 + (fTemp381 + (fTemp382 + (fTemp383 + (fTemp384 + (((fTemp313 + ((fTemp322 + (fTemp307 + fTemp385)) + fTemp386)) + fTemp387) + fTemp388)))))) - (fTemp389 + (fTemp301 + (fTemp390 + (fTemp391 + (fTemp392 + ((((fTemp320 + (fTemp376 + fTemp393)) + fTemp394) + fTemp395) + fTemp396))))))));
			fRec171[0] = max((fRec171[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp397))))));
			fVbargraph59 = FAUSTFLOAT(fRec171[0]);
			output34[i] = FAUSTFLOAT(fTemp397);
			double fTemp398 = (0.016998264729033874 * fTemp95);
			double fTemp399 = (0.018831075746023631 * fTemp110);
			double fTemp400 = (0.020818537549440772 * fTemp63);
			double fTemp401 = (0.021308452520676563 * fTemp55);
			double fTemp402 = (0.0053753232814252004 * fTemp21);
			double fTemp403 = (0.0300712209047478 * fTemp27);
			double fTemp404 = (0.010280801007450525 * fTemp33);
			double fTemp405 = (0.0089472296718741684 * fTemp36);
			double fTemp406 = (0.027200442744256377 * fTemp39);
			double fTemp407 = (0.010356563277706199 * fTemp42);
			double fTemp408 = (0.010535180889746086 * fTemp6);
			double fTemp409 = (0.0071028175068921853 * fTemp12);
			double fTemp410 = (0.028411270027568734 * fTemp15);
			double fTemp411 = (0.0067104222539056298 * fTemp69);
			double fTemp412 = (fRec1[0] * ((fTemp398 + (fTemp399 + (fTemp400 + (fTemp401 + (fTemp337 + (fTemp339 + (((((fTemp313 + ((fTemp307 + fTemp402) + fTemp403)) + fTemp404) + fTemp405) + fTemp406) + fTemp407))))))) - (fTemp326 + (fTemp381 + (fTemp329 + ((fTemp386 + (fTemp320 + (((fTemp319 + fTemp408) + fTemp409) + fTemp410))) + fTemp411))))));
			fRec172[0] = max((fRec172[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp412))))));
			fVbargraph60 = FAUSTFLOAT(fRec172[0]);
			output35[i] = FAUSTFLOAT(fTemp412);
			double fTemp413 = (0.018831075746023662 * fTemp110);
			double fTemp414 = (0.035508216952135588 * fTemp61);
			double fTemp415 = (0.020818537549440803 * fTemp63);
			double fTemp416 = (0.0067104222539056237 * fTemp69);
			double fTemp417 = (0.010535180889746074 * fTemp6);
			double fTemp418 = (0.0071028175068921775 * fTemp12);
			double fTemp419 = (0.0089472296718741736 * fTemp36);
			double fTemp420 = (0.010356563277706251 * fTemp42);
			double fTemp421 = (0.016998264729033857 * fTemp95);
			double fTemp422 = (0.021308452520676542 * fTemp55);
			double fTemp423 = (0.028411270027568748 * fTemp15);
			double fTemp424 = (0.0053753232814251943 * fTemp21);
			double fTemp425 = (0.022664352972045165 * fTemp24);
			double fTemp426 = (0.030071220904747779 * fTemp27);
			double fTemp427 = (0.010280801007450515 * fTemp33);
			double fTemp428 = (0.027200442744256356 * fTemp39);
			double fTemp429 = (fRec1[0] * ((fTemp413 + (fTemp414 + (fTemp415 + (fTemp363 + (fTemp416 + (fTemp359 + (((fTemp313 + ((fTemp307 + fTemp417) + fTemp418)) + fTemp419) + fTemp420))))))) - (fTemp347 + (fTemp421 + (fTemp422 + (fTemp329 + ((((((fTemp320 + (fTemp319 + fTemp423)) + fTemp424) + fTemp425) + fTemp426) + fTemp427) + fTemp428)))))));
			fRec173[0] = max((fRec173[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp429))))));
			fVbargraph61 = FAUSTFLOAT(fRec173[0]);
			output36[i] = FAUSTFLOAT(fTemp429);
			double fTemp430 = (0.010280801007450555 * fTemp100);
			double fTemp431 = (0.0067104222539056471 * fTemp69);
			double fTemp432 = (0.0053753232814252143 * fTemp66);
			double fTemp433 = (0.031605542669238228 * fTemp6);
			double fTemp434 = (0.021308452520676539 * fTemp12);
			double fTemp435 = (0.028411270027568696 * fTemp15);
			double fTemp436 = (0.02266435297204512 * fTemp24);
			double fTemp437 = (0.02081853754944071 * fTemp27);
			double fTemp438 = (0.018831075746023575 * fTemp39);
			double fTemp439 = (0.010356563277706064 * fTemp42);
			double fTemp440 = (0.0071028175068922053 * fTemp92);
			double fTemp441 = (0.016998264729033919 * fTemp95);
			double fTemp442 = (0.027200442744256415 * fTemp110);
			double fTemp443 = (0.035508216952135643 * fTemp61);
			double fTemp444 = (0.030071220904747842 * fTemp63);
			double fTemp445 = (0.021308452520676618 * fTemp55);
			double fTemp446 = (0.010535180889746113 * fTemp57);
			double fTemp447 = (0.016125969844275587 * fTemp21);
			double fTemp448 = (0.030842403022351552 * fTemp33);
			double fTemp449 = (0.0089472296718741563 * fTemp36);
			double fTemp450 = (fRec1[0] * ((fTemp430 + (fTemp431 + (fTemp432 + (((fTemp313 + (((((fTemp307 + fTemp433) + fTemp434) + fTemp435) + fTemp436) + fTemp437)) + fTemp438) + fTemp439)))) - (fTemp440 + (fTemp441 + (fTemp442 + (fTemp443 + (fTemp444 + (fTemp445 + (fTemp446 + (((fTemp321 + fTemp447) + fTemp448) + fTemp449))))))))));
			fRec174[0] = max((fRec174[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp450))))));
			fVbargraph62 = FAUSTFLOAT(fRec174[0]);
			output37[i] = FAUSTFLOAT(fTemp450);
			double fTemp451 = (fTemp306 + fTemp319);
			double fTemp452 = (fRec1[0] * ((fTemp302 + (fTemp303 + (fTemp304 + (fTemp305 + (fTemp316 + (fTemp315 + (fTemp323 + (fTemp313 + (fTemp312 + (fTemp320 + (fTemp310 + (fTemp238 + fTemp308)))))))))))) - (fTemp299 + (fTemp300 + (fTemp301 + (fTemp317 + (fTemp318 + (fTemp314 + (fTemp324 + (fTemp311 + (fTemp322 + (fTemp309 + fTemp451))))))))))));
			fRec175[0] = max((fRec175[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp452))))));
			fVbargraph63 = FAUSTFLOAT(fRec175[0]);
			output38[i] = FAUSTFLOAT(fTemp452);
			double fTemp453 = (fRec1[0] * ((fTemp334 + (fTemp328 + (fTemp329 + (fTemp337 + (fTemp333 + (fTemp345 + (fTemp332 + (fTemp344 + (fTemp313 + (fTemp342 + (fTemp320 + (fTemp238 + fTemp330)))))))))))) - (fTemp326 + (fTemp327 + (fTemp335 + (fTemp336 + (fTemp338 + (fTemp339 + (fTemp343 + (fTemp341 + (fTemp340 + (fTemp331 + fTemp451))))))))))));
			fRec176[0] = max((fRec176[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp453))))));
			fVbargraph64 = FAUSTFLOAT(fRec176[0]);
			output39[i] = FAUSTFLOAT(fTemp453);
			double fTemp454 = (fRec1[0] * ((fTemp355 + (fTemp356 + (fTemp335 + (fTemp329 + (fTemp363 + (fTemp348 + (fTemp354 + (fTemp352 + (fTemp313 + (fTemp350 + (fTemp342 + (fTemp349 + (fTemp320 + (fTemp238 + fTemp361)))))))))))))) - (fTemp347 + (fTemp357 + (fTemp358 + (fTemp359 + (fTemp353 + (fTemp351 + (fTemp362 + (fTemp319 + (fTemp306 + fTemp360)))))))))));
			fRec177[0] = max((fRec177[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp454))))));
			fVbargraph65 = FAUSTFLOAT(fRec177[0]);
			output40[i] = FAUSTFLOAT(fTemp454);
			double fTemp455 = (fTemp319 + (fTemp306 + fTemp308));
			double fTemp456 = (fRec1[0] * ((fTemp300 + (fTemp367 + (fTemp368 + (fTemp374 + (fTemp369 + (fTemp372 + (fTemp378 + (fTemp313 + (fTemp322 + (fTemp320 + (fTemp370 + (fTemp238 + fTemp309)))))))))))) - (fTemp365 + (fTemp366 + (fTemp302 + (fTemp373 + (fTemp375 + (fTemp324 + (fTemp323 + (fTemp377 + (fTemp371 + fTemp455)))))))))));
			fRec178[0] = max((fRec178[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp456))))));
			fVbargraph66 = FAUSTFLOAT(fRec178[0]);
			output41[i] = FAUSTFLOAT(fTemp456);
			double fTemp457 = (fRec1[0] * ((fTemp389 + (fTemp301 + (fTemp381 + (fTemp382 + (fTemp384 + (fTemp388 + (fTemp396 + (fTemp313 + (fTemp322 + (fTemp320 + (fTemp385 + (fTemp238 + fTemp393)))))))))))) - (fTemp380 + (fTemp390 + (fTemp391 + (fTemp383 + (fTemp392 + (fTemp395 + (fTemp387 + (fTemp394 + (fTemp386 + fTemp455)))))))))));
			fRec179[0] = max((fRec179[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp457))))));
			fVbargraph67 = FAUSTFLOAT(fRec179[0]);
			output42[i] = FAUSTFLOAT(fTemp457);
			double fTemp458 = (fRec1[0] * ((fTemp326 + (fTemp400 + (fTemp401 + (fTemp339 + (fTemp407 + (fTemp405 + (fTemp313 + (fTemp403 + (fTemp386 + (fTemp402 + (fTemp320 + (fTemp238 + fTemp409)))))))))))) - (fTemp398 + (fTemp399 + (fTemp381 + (fTemp329 + (fTemp337 + (fTemp411 + (fTemp406 + (fTemp404 + (fTemp410 + (fTemp319 + (fTemp306 + fTemp408)))))))))))));
			fRec180[0] = max((fRec180[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp458))))));
			fVbargraph68 = FAUSTFLOAT(fRec180[0]);
			output43[i] = FAUSTFLOAT(fTemp458);
			double fTemp459 = (fRec1[0] * ((fTemp347 + (fTemp421 + (fTemp414 + (fTemp415 + (fTemp416 + (fTemp359 + (fTemp420 + (fTemp428 + (fTemp419 + (fTemp427 + (fTemp313 + (fTemp425 + (fTemp320 + (fTemp238 + fTemp417)))))))))))))) - (fTemp413 + (fTemp422 + (fTemp329 + (fTemp363 + (fTemp426 + (fTemp424 + (fTemp423 + (fTemp418 + fTemp451))))))))));
			fRec181[0] = max((fRec181[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp459))))));
			fVbargraph69 = FAUSTFLOAT(fRec181[0]);
			output44[i] = FAUSTFLOAT(fTemp459);
			double fTemp460 = (fRec1[0] * ((fTemp440 + (fTemp441 + (fTemp442 + (fTemp431 + (fTemp432 + (fTemp439 + (fTemp448 + (fTemp313 + (fTemp437 + (fTemp320 + (fTemp435 + (fTemp238 + fTemp433)))))))))))) - (fTemp443 + (fTemp444 + (fTemp445 + (fTemp446 + (fTemp430 + (fTemp438 + (fTemp449 + (fTemp436 + (fTemp447 + (fTemp434 + fTemp451))))))))))));
			fRec182[0] = max((fRec182[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp460))))));
			fVbargraph70 = FAUSTFLOAT(fRec182[0]);
			output45[i] = FAUSTFLOAT(fTemp460);
			double fTemp461 = (fTemp238 + fTemp241);
			double fTemp462 = (fRec1[0] * ((fTemp230 + (fTemp233 + (fTemp234 + (fTemp236 + (fTemp237 + (fTemp253 + (fTemp246 + (fTemp250 + (fTemp245 + fTemp461))))))))) - (fTemp229 + (fTemp231 + (fTemp232 + (fTemp248 + (fTemp235 + (fTemp254 + (fTemp252 + (fTemp247 + (fTemp251 + (fTemp243 + (fTemp239 + fTemp249)))))))))))));
			fRec183[0] = max((fRec183[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp462))))));
			fVbargraph71 = FAUSTFLOAT(fRec183[0]);
			output46[i] = FAUSTFLOAT(fTemp462);
			double fTemp463 = (fRec1[0] * ((fTemp260 + (fTemp231 + (fTemp257 + (fTemp233 + (fTemp237 + (fTemp247 + (fTemp246 + (fTemp258 + (fTemp263 + fTemp461))))))))) - (fTemp261 + (fTemp256 + (fTemp234 + (fTemp235 + (fTemp236 + (fTemp254 + (fTemp259 + (fTemp264 + (fTemp245 + (fTemp243 + (fTemp239 + fTemp262)))))))))))));
			fRec184[0] = max((fRec184[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp463))))));
			fVbargraph72 = FAUSTFLOAT(fRec184[0]);
			output47[i] = FAUSTFLOAT(fTemp463);
			double fTemp464 = (fRec1[0] * ((fTemp272 + (fTemp273 + (fTemp266 + (fTemp234 + (fTemp276 + (fTemp236 + (fTemp270 + (fTemp281 + (fTemp246 + (fTemp269 + (fTemp267 + (fTemp279 + fTemp461)))))))))))) - (fTemp231 + (fTemp274 + (fTemp275 + (fTemp277 + (fTemp254 + (fTemp271 + (fTemp268 + (fTemp280 + (fTemp243 + (fTemp239 + fTemp278))))))))))));
			fRec185[0] = max((fRec185[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp464))))));
			fVbargraph73 = FAUSTFLOAT(fRec185[0]);
			output48[i] = FAUSTFLOAT(fTemp464);
			double fTemp465 = (fRec1[0] * ((fTemp288 + (fTemp231 + (fTemp289 + (fTemp283 + (fTemp235 + (fTemp297 + (fTemp246 + (fTemp294 + (fTemp286 + (fTemp241 + (fTemp238 + fTemp284))))))))))) - (fTemp290 + (fTemp234 + (fTemp291 + (fTemp236 + (fTemp292 + (fTemp254 + (fTemp296 + (fTemp287 + (fTemp295 + (fTemp243 + (fTemp293 + (fTemp239 + fTemp285))))))))))))));
			fRec186[0] = max((fRec186[1] - fConst1), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp465))))));
			fVbargraph74 = FAUSTFLOAT(fRec186[0]);
			output49[i] = FAUSTFLOAT(fTemp465);
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
			fRec0[1] = fRec0[0];
			fRec81[1] = fRec81[0];
			fRec85[1] = fRec85[0];
			fRec86[1] = fRec86[0];
			fRec84[1] = fRec84[0];
			fRec83[1] = fRec83[0];
			fRec89[1] = fRec89[0];
			fRec88[1] = fRec88[0];
			fRec87[1] = fRec87[0];
			fRec93[1] = fRec93[0];
			fRec94[1] = fRec94[0];
			fRec92[1] = fRec92[0];
			fRec95[1] = fRec95[0];
			fRec96[1] = fRec96[0];
			fRec91[1] = fRec91[0];
			fRec90[1] = fRec90[0];
			fRec100[1] = fRec100[0];
			fRec101[1] = fRec101[0];
			fRec99[1] = fRec99[0];
			fRec102[1] = fRec102[0];
			fRec98[1] = fRec98[0];
			fRec97[1] = fRec97[0];
			fRec106[1] = fRec106[0];
			fRec107[1] = fRec107[0];
			fRec105[1] = fRec105[0];
			fRec108[1] = fRec108[0];
			fRec104[1] = fRec104[0];
			fRec103[1] = fRec103[0];
			fRec112[1] = fRec112[0];
			fRec113[1] = fRec113[0];
			fRec111[1] = fRec111[0];
			fRec114[1] = fRec114[0];
			fRec115[1] = fRec115[0];
			fRec110[1] = fRec110[0];
			fRec109[1] = fRec109[0];
			fRec82[1] = fRec82[0];
			fRec116[1] = fRec116[0];
			fRec117[1] = fRec117[0];
			fRec118[1] = fRec118[0];
			fRec119[1] = fRec119[0];
			fRec123[1] = fRec123[0];
			fRec124[1] = fRec124[0];
			fRec122[1] = fRec122[0];
			fRec121[1] = fRec121[0];
			fRec128[1] = fRec128[0];
			fRec129[1] = fRec129[0];
			fRec127[1] = fRec127[0];
			fRec130[1] = fRec130[0];
			fRec126[1] = fRec126[0];
			fRec125[1] = fRec125[0];
			fRec134[1] = fRec134[0];
			fRec135[1] = fRec135[0];
			fRec133[1] = fRec133[0];
			fRec136[1] = fRec136[0];
			fRec137[1] = fRec137[0];
			fRec132[1] = fRec132[0];
			fRec131[1] = fRec131[0];
			fRec141[1] = fRec141[0];
			fRec142[1] = fRec142[0];
			fRec140[1] = fRec140[0];
			fRec143[1] = fRec143[0];
			fRec144[1] = fRec144[0];
			fRec139[1] = fRec139[0];
			fRec138[1] = fRec138[0];
			fRec120[1] = fRec120[0];
			fRec145[1] = fRec145[0];
			fRec146[1] = fRec146[0];
			fRec147[1] = fRec147[0];
			fRec148[1] = fRec148[0];
			fRec149[1] = fRec149[0];
			fRec150[1] = fRec150[0];
			fRec151[1] = fRec151[0];
			fRec152[1] = fRec152[0];
			fRec153[1] = fRec153[0];
			fRec154[1] = fRec154[0];
			fRec155[1] = fRec155[0];
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
