/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2016"
license: "GPL"
name: "HOAConverter5"
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

double rint(double dummy0);
double fabs(double dummy0);
double log10(double dummy0);

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
	
 private:
	
	int fSamplingFreq;
	double fConst0;
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fHslider1;
	double fRec1[2];
	FAUSTFLOAT fVbargraph0;
	double fRec0[2];
	FAUSTFLOAT fVbargraph1;
	double fRec3[2];
	FAUSTFLOAT fVbargraph2;
	double fRec4[2];
	FAUSTFLOAT fVbargraph3;
	double fRec5[2];
	FAUSTFLOAT fVbargraph4;
	double fRec2[2];
	FAUSTFLOAT fVbargraph5;
	double fRec6[2];
	FAUSTFLOAT fVbargraph6;
	double fRec7[2];
	FAUSTFLOAT fVbargraph7;
	double fRec9[2];
	FAUSTFLOAT fVbargraph8;
	double fRec10[2];
	FAUSTFLOAT fVbargraph9;
	double fRec11[2];
	FAUSTFLOAT fVbargraph10;
	double fRec8[2];
	FAUSTFLOAT fVbargraph11;
	double fRec13[2];
	FAUSTFLOAT fVbargraph12;
	double fRec14[2];
	FAUSTFLOAT fVbargraph13;
	double fRec12[2];
	FAUSTFLOAT fVbargraph14;
	double fRec15[2];
	FAUSTFLOAT fVbargraph15;
	double fRec16[2];
	FAUSTFLOAT fVbargraph16;
	double fRec17[2];
	FAUSTFLOAT fVbargraph17;
	double fRec19[2];
	FAUSTFLOAT fVbargraph18;
	double fRec20[2];
	FAUSTFLOAT fVbargraph19;
	double fRec21[2];
	FAUSTFLOAT fVbargraph20;
	double fRec18[2];
	FAUSTFLOAT fVbargraph21;
	double fRec23[2];
	FAUSTFLOAT fVbargraph22;
	double fRec24[2];
	FAUSTFLOAT fVbargraph23;
	double fRec22[2];
	FAUSTFLOAT fVbargraph24;
	double fRec26[2];
	FAUSTFLOAT fVbargraph25;
	double fRec25[2];
	FAUSTFLOAT fVbargraph26;
	double fRec28[2];
	FAUSTFLOAT fVbargraph27;
	double fRec27[2];
	FAUSTFLOAT fVbargraph28;
	double fRec29[2];
	FAUSTFLOAT fVbargraph29;
	double fRec30[2];
	FAUSTFLOAT fVbargraph30;
	double fRec31[2];
	FAUSTFLOAT fVbargraph31;
	double fRec33[2];
	FAUSTFLOAT fVbargraph32;
	double fRec32[2];
	FAUSTFLOAT fVbargraph33;
	double fRec35[2];
	FAUSTFLOAT fVbargraph34;
	double fRec34[2];
	FAUSTFLOAT fVbargraph35;
	double fRec37[2];
	FAUSTFLOAT fVbargraph36;
	double fRec36[2];
	FAUSTFLOAT fVbargraph37;
	double fRec39[2];
	FAUSTFLOAT fVbargraph38;
	double fRec38[2];
	FAUSTFLOAT fVbargraph39;
	double fRec41[2];
	FAUSTFLOAT fVbargraph40;
	double fRec40[2];
	FAUSTFLOAT fVbargraph41;
	double fRec43[2];
	FAUSTFLOAT fVbargraph42;
	double fRec42[2];
	FAUSTFLOAT fVbargraph43;
	double fRec45[2];
	FAUSTFLOAT fVbargraph44;
	double fRec44[2];
	FAUSTFLOAT fVbargraph45;
	double fRec47[2];
	FAUSTFLOAT fVbargraph46;
	double fRec46[2];
	FAUSTFLOAT fVbargraph47;
	double fRec49[2];
	FAUSTFLOAT fVbargraph48;
	double fRec48[2];
	FAUSTFLOAT fVbargraph49;
	double fRec51[2];
	FAUSTFLOAT fVbargraph50;
	double fRec50[2];
	FAUSTFLOAT fVbargraph51;
	double fRec53[2];
	FAUSTFLOAT fVbargraph52;
	double fRec52[2];
	FAUSTFLOAT fVbargraph53;
	double fRec55[2];
	FAUSTFLOAT fVbargraph54;
	double fRec54[2];
	FAUSTFLOAT fVbargraph55;
	double fRec57[2];
	FAUSTFLOAT fVbargraph56;
	double fRec56[2];
	FAUSTFLOAT fVbargraph57;
	double fRec59[2];
	FAUSTFLOAT fVbargraph58;
	double fRec58[2];
	FAUSTFLOAT fVbargraph59;
	double fRec61[2];
	FAUSTFLOAT fVbargraph60;
	double fRec60[2];
	FAUSTFLOAT fVbargraph61;
	double fRec63[2];
	FAUSTFLOAT fVbargraph62;
	double fRec62[2];
	FAUSTFLOAT fVbargraph63;
	double fRec65[2];
	FAUSTFLOAT fVbargraph64;
	double fRec64[2];
	FAUSTFLOAT fVbargraph65;
	double fRec67[2];
	FAUSTFLOAT fVbargraph66;
	double fRec66[2];
	FAUSTFLOAT fVbargraph67;
	double fRec69[2];
	FAUSTFLOAT fVbargraph68;
	double fRec68[2];
	FAUSTFLOAT fVbargraph69;
	double fRec71[2];
	FAUSTFLOAT fVbargraph70;
	double fRec70[2];
	FAUSTFLOAT fVbargraph71;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
		m->declare("license", "GPL");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("name", "HOAConverter5");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "0.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 36;
		
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
		fConst0 = (80.0 / min(192000.0, max(1000.0, double(fSamplingFreq))));
		
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(1.0);
		fHslider1 = FAUSTFLOAT(1.0);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec1[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec0[l1] = 0.0;
			
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
			fRec2[l5] = 0.0;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec6[l6] = 0.0;
			
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec7[l7] = 0.0;
			
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
			fRec8[l11] = 0.0;
			
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
			fRec15[l15] = 0.0;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fRec16[l16] = 0.0;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec17[l17] = 0.0;
			
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
			fRec18[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec23[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec24[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec22[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec26[l25] = 0.0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec25[l26] = 0.0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec28[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec27[l28] = 0.0;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec29[l29] = 0.0;
			
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec30[l30] = 0.0;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec31[l31] = 0.0;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec33[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec32[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec35[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec34[l35] = 0.0;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec37[l36] = 0.0;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec36[l37] = 0.0;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec39[l38] = 0.0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec38[l39] = 0.0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec41[l40] = 0.0;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec40[l41] = 0.0;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec43[l42] = 0.0;
			
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec42[l43] = 0.0;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fRec45[l44] = 0.0;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec44[l45] = 0.0;
			
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec47[l46] = 0.0;
			
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			fRec46[l47] = 0.0;
			
		}
		for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
			fRec49[l48] = 0.0;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec48[l49] = 0.0;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			fRec51[l50] = 0.0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec50[l51] = 0.0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec53[l52] = 0.0;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec52[l53] = 0.0;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec55[l54] = 0.0;
			
		}
		for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
			fRec54[l55] = 0.0;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fRec57[l56] = 0.0;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec56[l57] = 0.0;
			
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec59[l58] = 0.0;
			
		}
		for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
			fRec58[l59] = 0.0;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec61[l60] = 0.0;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			fRec60[l61] = 0.0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			fRec63[l62] = 0.0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec62[l63] = 0.0;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec65[l64] = 0.0;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec64[l65] = 0.0;
			
		}
		for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
			fRec67[l66] = 0.0;
			
		}
		for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
			fRec66[l67] = 0.0;
			
		}
		for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
			fRec69[l68] = 0.0;
			
		}
		for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
			fRec68[l69] = 0.0;
			
		}
		for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
			fRec71[l70] = 0.0;
			
		}
		for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
			fRec70[l71] = 0.0;
			
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
		ui_interface->openVerticalBox("HOAConverter5");
		ui_interface->declare(&fHslider1, "0", "");
		ui_interface->declare(&fHslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Input (1-ACN_N3D,2-ACN_SN3D, 3-FuMa)", &fHslider1, 1.0, 1.0, 3.0, 1.0);
		ui_interface->declare(&fHslider0, "0", "");
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Output (1-ACN_N3D,2-ACN_SN3D, 3-FuMa)", &fHslider0, 1.0, 1.0, 3.0, 1.0);
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d36450", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d434e0", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d4f7a0", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d48ed0", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d66900", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d7d670", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d6c1c0", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d82f30", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d72610", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1da2b50", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1db9c40", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1dcb8a0", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1da84f0", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1dbf5e0", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1dd8100", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1daea20", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1df8fc0", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e04cc0", &fVbargraph34, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e10530", &fVbargraph36, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e1bf60", &fVbargraph38, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e27b50", &fVbargraph40, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e33900", &fVbargraph42, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e3f870", &fVbargraph44, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e4b9a0", &fVbargraph46, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e57c90", &fVbargraph48, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5");
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e64140", &fVbargraph50, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e6fe30", &fVbargraph52, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e7b860", &fVbargraph54, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e87450", &fVbargraph56, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e93200", &fVbargraph58, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e9f170", &fVbargraph60, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1eab2a0", &fVbargraph62, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1eb7590", &fVbargraph64, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph66, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1ec3a40", &fVbargraph66, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph68, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1ed00b0", &fVbargraph68, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph70, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1edc8e0", &fVbargraph70, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openHorizontalBox("Outputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d3d420", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d54860", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d5ada0", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d60be0", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d77a30", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d88f70", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d8f430", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d95f70", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1d9cb90", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1db3f20", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1dc5aa0", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1dd1e70", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1dde020", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1de4d20", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1debea0", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1df2d60", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1dfeec0", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e0a650", &fVbargraph35, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e15fa0", &fVbargraph37, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e21ab0", &fVbargraph39, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e2d780", &fVbargraph41, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e39610", &fVbargraph43, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e45660", &fVbargraph45, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e51870", &fVbargraph47, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e5dc40", &fVbargraph49, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5");
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e69f50", &fVbargraph51, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e758a0", &fVbargraph53, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fVbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e813b0", &fVbargraph55, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fVbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e8d080", &fVbargraph57, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fVbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1e98f10", &fVbargraph59, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fVbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1ea4f60", &fVbargraph61, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fVbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1eb1170", &fVbargraph63, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fVbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1ebd540", &fVbargraph65, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fVbargraph67, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1ec9ad0", &fVbargraph67, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fVbargraph69, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1ed6220", &fVbargraph69, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fVbargraph71, "unit", "dB");
		ui_interface->addVerticalBargraph("0x1ee2b30", &fVbargraph71, -70.0, 6.0);
		ui_interface->closeBox();
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
		int iSlow0 = int(((rint(double(fHslider0)) + (3.0 * rint(double(fHslider1)))) + -4.0));
		int iSlow1 = (iSlow0 >= 5);
		int iSlow2 = (iSlow0 >= 3);
		int iSlow3 = (iSlow0 >= 2);
		int iSlow4 = (iSlow0 >= 7);
		int iSlow5 = (iSlow0 >= 6);
		int iSlow6 = (iSlow0 >= 8);
		int iSlow7 = (iSlow0 >= 1);
		int iSlow8 = (iSlow0 >= 4);
		for (int i = 0; (i < count); i = (i + 1)) {
			double fTemp0 = double(input0[i]);
			fRec1[0] = max((fRec1[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fVbargraph0 = FAUSTFLOAT(fRec1[0]);
			double fTemp1 = (0.70710678118654746 * fTemp0);
			double fTemp2 = (1.4142135623730951 * fTemp0);
			double fTemp3 = (iSlow1?(iSlow4?(iSlow6?fTemp0:fTemp2):(iSlow5?fTemp2:fTemp1)):(iSlow2?fTemp0:(iSlow3?fTemp1:fTemp0)));
			fRec0[0] = max((fRec0[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp3))))));
			fVbargraph1 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp3);
			double fTemp4 = double(input1[i]);
			fRec3[0] = max((fRec3[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fVbargraph2 = FAUSTFLOAT(fRec3[0]);
			double fTemp5 = (0.57735026918962584 * fTemp4);
			double fTemp6 = double(input3[i]);
			fRec4[0] = max((fRec4[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fVbargraph3 = FAUSTFLOAT(fRec4[0]);
			double fTemp7 = (0.57735026918962584 * fTemp6);
			double fTemp8 = (1.7320508075688772 * fTemp4);
			double fTemp9 = double(input2[i]);
			fRec5[0] = max((fRec5[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph4 = FAUSTFLOAT(fRec5[0]);
			double fTemp10 = (1.7320508075688772 * fTemp9);
			double fTemp11 = (iSlow1?(iSlow4?(iSlow6?fTemp4:fTemp9):(iSlow5?fTemp10:fTemp6)):(iSlow2?(iSlow8?fTemp4:fTemp8):(iSlow3?fTemp7:(iSlow7?fTemp5:fTemp4))));
			fRec2[0] = max((fRec2[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fVbargraph5 = FAUSTFLOAT(fRec2[0]);
			output1[i] = FAUSTFLOAT(fTemp11);
			double fTemp12 = (0.57735026918962584 * fTemp9);
			double fTemp13 = (1.7320508075688772 * fTemp6);
			double fTemp14 = (iSlow1?(iSlow4?(iSlow6?fTemp9:fTemp6):(iSlow5?fTemp13:fTemp4)):(iSlow2?(iSlow8?fTemp9:fTemp10):(iSlow3?fTemp5:(iSlow7?fTemp12:fTemp9))));
			fRec6[0] = max((fRec6[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fVbargraph6 = FAUSTFLOAT(fRec6[0]);
			output2[i] = FAUSTFLOAT(fTemp14);
			double fTemp15 = (iSlow1?(iSlow4?(iSlow6?fTemp6:fTemp4):(iSlow5?fTemp8:fTemp9)):(iSlow2?(iSlow8?fTemp6:fTemp13):(iSlow3?fTemp12:(iSlow7?fTemp7:fTemp6))));
			fRec7[0] = max((fRec7[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph7 = FAUSTFLOAT(fRec7[0]);
			output3[i] = FAUSTFLOAT(fTemp15);
			double fTemp16 = double(input4[i]);
			fRec9[0] = max((fRec9[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph8 = FAUSTFLOAT(fRec9[0]);
			double fTemp17 = double(input6[i]);
			fRec10[0] = max((fRec10[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph9 = FAUSTFLOAT(fRec10[0]);
			double fTemp18 = (0.44721359549995793 * fTemp17);
			double fTemp19 = (2.2360679774997898 * fTemp16);
			double fTemp20 = double(input8[i]);
			fRec11[0] = max((fRec11[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph10 = FAUSTFLOAT(fRec11[0]);
			double fTemp21 = (iSlow1?(iSlow4?(iSlow6?fTemp16:(0.86602540378443871 * fTemp20)):(iSlow5?(1.9364916731037085 * fTemp20):fTemp17)):(iSlow2?(iSlow8?fTemp16:fTemp19):(iSlow3?fTemp18:(iSlow7?(0.44721359549995793 * fTemp16):fTemp16))));
			fRec8[0] = max((fRec8[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph11 = FAUSTFLOAT(fRec8[0]);
			output4[i] = FAUSTFLOAT(fTemp21);
			double fTemp22 = double(input5[i]);
			fRec13[0] = max((fRec13[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph12 = FAUSTFLOAT(fRec13[0]);
			double fTemp23 = double(input7[i]);
			fRec14[0] = max((fRec14[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fVbargraph13 = FAUSTFLOAT(fRec14[0]);
			double fTemp24 = (iSlow1?(iSlow4?(iSlow6?fTemp22:(0.86602540378443871 * fTemp17)):(iSlow5?(1.9364916731037085 * fTemp17):(1.1547005383792515 * fTemp23))):(iSlow2?(iSlow8?fTemp22:(2.2360679774997898 * fTemp22)):(iSlow3?(0.5163977794943222 * fTemp23):(iSlow7?(0.44721359549995793 * fTemp22):fTemp22))));
			fRec12[0] = max((fRec12[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph14 = FAUSTFLOAT(fRec12[0]);
			output5[i] = FAUSTFLOAT(fTemp24);
			double fTemp25 = (iSlow1?(iSlow4?(iSlow6?fTemp17:fTemp16):(iSlow5?fTemp19:(1.1547005383792515 * fTemp22))):(iSlow2?(iSlow8?fTemp17:(2.2360679774997898 * fTemp17)):(iSlow3?(0.5163977794943222 * fTemp22):(iSlow7?fTemp18:fTemp17))));
			fRec15[0] = max((fRec15[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph15 = FAUSTFLOAT(fRec15[0]);
			output6[i] = FAUSTFLOAT(fTemp25);
			double fTemp26 = (iSlow1?(iSlow4?(iSlow6?fTemp23:(0.86602540378443871 * fTemp22)):(iSlow5?(1.9364916731037085 * fTemp22):(1.1547005383792515 * fTemp20))):(iSlow2?(iSlow8?fTemp23:(2.2360679774997898 * fTemp23)):(iSlow3?(0.5163977794943222 * fTemp20):(iSlow7?(0.44721359549995793 * fTemp23):fTemp23))));
			fRec16[0] = max((fRec16[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fVbargraph16 = FAUSTFLOAT(fRec16[0]);
			output7[i] = FAUSTFLOAT(fTemp26);
			double fTemp27 = (iSlow1?(iSlow4?(iSlow6?fTemp20:(0.86602540378443871 * fTemp23)):(iSlow5?(1.9364916731037085 * fTemp23):(1.1547005383792515 * fTemp16))):(iSlow2?(iSlow8?fTemp20:(2.2360679774997898 * fTemp20)):(iSlow3?(0.5163977794943222 * fTemp16):(iSlow7?(0.44721359549995793 * fTemp20):fTemp20))));
			fRec17[0] = max((fRec17[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fVbargraph17 = FAUSTFLOAT(fRec17[0]);
			output8[i] = FAUSTFLOAT(fTemp27);
			double fTemp28 = double(input9[i]);
			fRec19[0] = max((fRec19[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fVbargraph18 = FAUSTFLOAT(fRec19[0]);
			double fTemp29 = double(input12[i]);
			fRec20[0] = max((fRec20[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph19 = FAUSTFLOAT(fRec20[0]);
			double fTemp30 = (0.3779644730092272 * fTemp29);
			double fTemp31 = (2.6457513110645907 * fTemp28);
			double fTemp32 = double(input15[i]);
			fRec21[0] = max((fRec21[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph20 = FAUSTFLOAT(fRec21[0]);
			double fTemp33 = (iSlow1?(iSlow4?(iSlow6?fTemp28:(0.79056941504209477 * fTemp32)):(iSlow5?(2.0916500663351889 * fTemp32):fTemp29)):(iSlow2?(iSlow8?fTemp28:fTemp31):(iSlow3?fTemp30:(iSlow7?(0.3779644730092272 * fTemp28):fTemp28))));
			fRec18[0] = max((fRec18[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph21 = FAUSTFLOAT(fRec18[0]);
			output9[i] = FAUSTFLOAT(fTemp33);
			double fTemp34 = double(input10[i]);
			fRec23[0] = max((fRec23[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fVbargraph22 = FAUSTFLOAT(fRec23[0]);
			double fTemp35 = double(input13[i]);
			fRec24[0] = max((fRec24[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph23 = FAUSTFLOAT(fRec24[0]);
			double fTemp36 = (iSlow1?(iSlow4?(iSlow6?fTemp34:(0.7453559924999299 * fTemp35)):(iSlow5?(1.9720265943665387 * fTemp35):(1.1858541225631423 * fTemp35))):(iSlow2?(iSlow8?fTemp34:(2.6457513110645907 * fTemp34)):(iSlow3?(0.44821072850039761 * fTemp35):(iSlow7?(0.3779644730092272 * fTemp34):fTemp34))));
			fRec22[0] = max((fRec22[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fVbargraph24 = FAUSTFLOAT(fRec22[0]);
			output10[i] = FAUSTFLOAT(fTemp36);
			double fTemp37 = double(input11[i]);
			fRec26[0] = max((fRec26[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fVbargraph25 = FAUSTFLOAT(fRec26[0]);
			double fTemp38 = (iSlow1?(iSlow4?(iSlow6?fTemp37:(0.84327404271156781 * fTemp37)):(iSlow5?(2.2310934040908683 * fTemp37):(1.1858541225631423 * fTemp37))):(iSlow2?(iSlow8?fTemp37:(2.6457513110645907 * fTemp37)):(iSlow3?(0.44821072850039761 * fTemp37):(iSlow7?(0.3779644730092272 * fTemp37):fTemp37))));
			fRec25[0] = max((fRec25[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fVbargraph26 = FAUSTFLOAT(fRec25[0]);
			output11[i] = FAUSTFLOAT(fTemp38);
			double fTemp39 = double(input14[i]);
			fRec28[0] = max((fRec28[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fVbargraph27 = FAUSTFLOAT(fRec28[0]);
			double fTemp40 = (iSlow1?(iSlow4?(iSlow6?fTemp29:fTemp28):(iSlow5?fTemp31:(1.3416407864998738 * fTemp39))):(iSlow2?(iSlow8?fTemp29:(2.6457513110645907 * fTemp29)):(iSlow3?(0.50709255283710997 * fTemp39):(iSlow7?fTemp30:fTemp29))));
			fRec27[0] = max((fRec27[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fVbargraph28 = FAUSTFLOAT(fRec27[0]);
			output12[i] = FAUSTFLOAT(fTemp40);
			double fTemp41 = (iSlow1?(iSlow4?(iSlow6?fTemp35:(0.84327404271156781 * fTemp34)):(iSlow5?(2.2310934040908683 * fTemp34):(1.3416407864998738 * fTemp34))):(iSlow2?(iSlow8?fTemp35:(2.6457513110645907 * fTemp35)):(iSlow3?(0.50709255283710997 * fTemp34):(iSlow7?(0.3779644730092272 * fTemp35):fTemp35))));
			fRec29[0] = max((fRec29[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fVbargraph29 = FAUSTFLOAT(fRec29[0]);
			output13[i] = FAUSTFLOAT(fTemp41);
			double fTemp42 = (iSlow1?(iSlow4?(iSlow6?fTemp39:(0.7453559924999299 * fTemp29)):(iSlow5?(1.9720265943665387 * fTemp29):(1.2649110640673518 * fTemp32))):(iSlow2?(iSlow8?fTemp39:(2.6457513110645907 * fTemp39)):(iSlow3?(0.47809144373375745 * fTemp32):(iSlow7?(0.3779644730092272 * fTemp39):fTemp39))));
			fRec30[0] = max((fRec30[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fVbargraph30 = FAUSTFLOAT(fRec30[0]);
			output14[i] = FAUSTFLOAT(fTemp42);
			double fTemp43 = (iSlow1?(iSlow4?(iSlow6?fTemp32:(0.79056941504209477 * fTemp39)):(iSlow5?(2.0916500663351889 * fTemp39):(1.2649110640673518 * fTemp28))):(iSlow2?(iSlow8?fTemp32:(2.6457513110645907 * fTemp32)):(iSlow3?(0.47809144373375745 * fTemp28):(iSlow7?(0.3779644730092272 * fTemp32):fTemp32))));
			fRec31[0] = max((fRec31[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fVbargraph31 = FAUSTFLOAT(fRec31[0]);
			output15[i] = FAUSTFLOAT(fTemp43);
			double fTemp44 = double(input16[i]);
			fRec33[0] = max((fRec33[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fVbargraph32 = FAUSTFLOAT(fRec33[0]);
			double fTemp45 = (iSlow1?(iSlow4?(iSlow6?fTemp44:0.0):0.0):(iSlow2?(iSlow8?fTemp44:(3.0 * fTemp44)):(iSlow3?0.0:(iSlow7?(0.33333333333333331 * fTemp44):fTemp44))));
			fRec32[0] = max((fRec32[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fVbargraph33 = FAUSTFLOAT(fRec32[0]);
			output16[i] = FAUSTFLOAT(fTemp45);
			double fTemp46 = double(input17[i]);
			fRec35[0] = max((fRec35[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fVbargraph34 = FAUSTFLOAT(fRec35[0]);
			double fTemp47 = (iSlow1?(iSlow4?(iSlow6?fTemp46:0.0):0.0):(iSlow2?(iSlow8?fTemp46:(3.0 * fTemp46)):(iSlow3?0.0:(iSlow7?(0.33333333333333331 * fTemp46):fTemp46))));
			fRec34[0] = max((fRec34[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fVbargraph35 = FAUSTFLOAT(fRec34[0]);
			output17[i] = FAUSTFLOAT(fTemp47);
			double fTemp48 = double(input18[i]);
			fRec37[0] = max((fRec37[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fVbargraph36 = FAUSTFLOAT(fRec37[0]);
			double fTemp49 = (iSlow1?(iSlow4?(iSlow6?fTemp48:0.0):0.0):(iSlow2?(iSlow8?fTemp48:(3.0 * fTemp48)):(iSlow3?0.0:(iSlow7?(0.33333333333333331 * fTemp48):fTemp48))));
			fRec36[0] = max((fRec36[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp49))))));
			fVbargraph37 = FAUSTFLOAT(fRec36[0]);
			output18[i] = FAUSTFLOAT(fTemp49);
			double fTemp50 = double(input19[i]);
			fRec39[0] = max((fRec39[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fVbargraph38 = FAUSTFLOAT(fRec39[0]);
			double fTemp51 = (iSlow1?(iSlow4?(iSlow6?fTemp50:0.0):0.0):(iSlow2?(iSlow8?fTemp50:(3.0 * fTemp50)):(iSlow3?0.0:(iSlow7?(0.33333333333333331 * fTemp50):fTemp50))));
			fRec38[0] = max((fRec38[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fVbargraph39 = FAUSTFLOAT(fRec38[0]);
			output19[i] = FAUSTFLOAT(fTemp51);
			double fTemp52 = double(input20[i]);
			fRec41[0] = max((fRec41[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fVbargraph40 = FAUSTFLOAT(fRec41[0]);
			double fTemp53 = (iSlow1?(iSlow4?(iSlow6?fTemp52:0.0):0.0):(iSlow2?(iSlow8?fTemp52:(3.0 * fTemp52)):(iSlow3?0.0:(iSlow7?(0.33333333333333331 * fTemp52):fTemp52))));
			fRec40[0] = max((fRec40[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fVbargraph41 = FAUSTFLOAT(fRec40[0]);
			output20[i] = FAUSTFLOAT(fTemp53);
			double fTemp54 = double(input21[i]);
			fRec43[0] = max((fRec43[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fVbargraph42 = FAUSTFLOAT(fRec43[0]);
			double fTemp55 = (iSlow1?(iSlow4?(iSlow6?fTemp54:0.0):0.0):(iSlow2?(iSlow8?fTemp54:(3.0 * fTemp54)):(iSlow3?0.0:(iSlow7?(0.33333333333333331 * fTemp54):fTemp54))));
			fRec42[0] = max((fRec42[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph43 = FAUSTFLOAT(fRec42[0]);
			output21[i] = FAUSTFLOAT(fTemp55);
			double fTemp56 = double(input22[i]);
			fRec45[0] = max((fRec45[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fVbargraph44 = FAUSTFLOAT(fRec45[0]);
			double fTemp57 = (iSlow1?(iSlow4?(iSlow6?fTemp56:0.0):0.0):(iSlow2?(iSlow8?fTemp56:(3.0 * fTemp56)):(iSlow3?0.0:(iSlow7?(0.33333333333333331 * fTemp56):fTemp56))));
			fRec44[0] = max((fRec44[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fVbargraph45 = FAUSTFLOAT(fRec44[0]);
			output22[i] = FAUSTFLOAT(fTemp57);
			double fTemp58 = double(input23[i]);
			fRec47[0] = max((fRec47[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fVbargraph46 = FAUSTFLOAT(fRec47[0]);
			double fTemp59 = (iSlow1?(iSlow4?(iSlow6?fTemp58:0.0):0.0):(iSlow2?(iSlow8?fTemp58:(3.0 * fTemp58)):(iSlow3?0.0:(iSlow7?(0.33333333333333331 * fTemp58):fTemp58))));
			fRec46[0] = max((fRec46[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fVbargraph47 = FAUSTFLOAT(fRec46[0]);
			output23[i] = FAUSTFLOAT(fTemp59);
			double fTemp60 = double(input24[i]);
			fRec49[0] = max((fRec49[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fVbargraph48 = FAUSTFLOAT(fRec49[0]);
			double fTemp61 = (iSlow1?(iSlow4?(iSlow6?fTemp60:0.0):0.0):(iSlow2?(iSlow8?fTemp60:(3.0 * fTemp60)):(iSlow3?0.0:(iSlow7?(0.33333333333333331 * fTemp60):fTemp60))));
			fRec48[0] = max((fRec48[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fVbargraph49 = FAUSTFLOAT(fRec48[0]);
			output24[i] = FAUSTFLOAT(fTemp61);
			double fTemp62 = double(input25[i]);
			fRec51[0] = max((fRec51[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fVbargraph50 = FAUSTFLOAT(fRec51[0]);
			double fTemp63 = (iSlow1?(iSlow4?(iSlow6?fTemp62:0.0):0.0):(iSlow2?(iSlow8?fTemp62:(3.3166247903553998 * fTemp62)):(iSlow3?0.0:(iSlow7?(0.30151134457776363 * fTemp62):fTemp62))));
			fRec50[0] = max((fRec50[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fVbargraph51 = FAUSTFLOAT(fRec50[0]);
			output25[i] = FAUSTFLOAT(fTemp63);
			double fTemp64 = double(input26[i]);
			fRec53[0] = max((fRec53[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fVbargraph52 = FAUSTFLOAT(fRec53[0]);
			double fTemp65 = (iSlow1?(iSlow4?(iSlow6?fTemp64:0.0):0.0):(iSlow2?(iSlow8?fTemp64:(3.3166247903553998 * fTemp64)):(iSlow3?0.0:(iSlow7?(0.30151134457776363 * fTemp64):fTemp64))));
			fRec52[0] = max((fRec52[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fVbargraph53 = FAUSTFLOAT(fRec52[0]);
			output26[i] = FAUSTFLOAT(fTemp65);
			double fTemp66 = double(input27[i]);
			fRec55[0] = max((fRec55[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fVbargraph54 = FAUSTFLOAT(fRec55[0]);
			double fTemp67 = (iSlow1?(iSlow4?(iSlow6?fTemp66:0.0):0.0):(iSlow2?(iSlow8?fTemp66:(3.3166247903553998 * fTemp66)):(iSlow3?0.0:(iSlow7?(0.30151134457776363 * fTemp66):fTemp66))));
			fRec54[0] = max((fRec54[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp67))))));
			fVbargraph55 = FAUSTFLOAT(fRec54[0]);
			output27[i] = FAUSTFLOAT(fTemp67);
			double fTemp68 = double(input28[i]);
			fRec57[0] = max((fRec57[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fVbargraph56 = FAUSTFLOAT(fRec57[0]);
			double fTemp69 = (iSlow1?(iSlow4?(iSlow6?fTemp68:0.0):0.0):(iSlow2?(iSlow8?fTemp68:(3.3166247903553998 * fTemp68)):(iSlow3?0.0:(iSlow7?(0.30151134457776363 * fTemp68):fTemp68))));
			fRec56[0] = max((fRec56[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp69))))));
			fVbargraph57 = FAUSTFLOAT(fRec56[0]);
			output28[i] = FAUSTFLOAT(fTemp69);
			double fTemp70 = double(input29[i]);
			fRec59[0] = max((fRec59[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp70))))));
			fVbargraph58 = FAUSTFLOAT(fRec59[0]);
			double fTemp71 = (iSlow1?(iSlow4?(iSlow6?fTemp70:0.0):0.0):(iSlow2?(iSlow8?fTemp70:(3.3166247903553998 * fTemp70)):(iSlow3?0.0:(iSlow7?(0.30151134457776363 * fTemp70):fTemp70))));
			fRec58[0] = max((fRec58[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fVbargraph59 = FAUSTFLOAT(fRec58[0]);
			output29[i] = FAUSTFLOAT(fTemp71);
			double fTemp72 = double(input30[i]);
			fRec61[0] = max((fRec61[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp72))))));
			fVbargraph60 = FAUSTFLOAT(fRec61[0]);
			double fTemp73 = (iSlow1?(iSlow4?(iSlow6?fTemp72:0.0):0.0):(iSlow2?(iSlow8?fTemp72:(3.3166247903553998 * fTemp72)):(iSlow3?0.0:(iSlow7?(0.30151134457776363 * fTemp72):fTemp72))));
			fRec60[0] = max((fRec60[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp73))))));
			fVbargraph61 = FAUSTFLOAT(fRec60[0]);
			output30[i] = FAUSTFLOAT(fTemp73);
			double fTemp74 = double(input31[i]);
			fRec63[0] = max((fRec63[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp74))))));
			fVbargraph62 = FAUSTFLOAT(fRec63[0]);
			double fTemp75 = (iSlow1?(iSlow4?(iSlow6?fTemp74:0.0):0.0):(iSlow2?(iSlow8?fTemp74:(3.3166247903553998 * fTemp74)):(iSlow3?0.0:(iSlow7?(0.30151134457776363 * fTemp74):fTemp74))));
			fRec62[0] = max((fRec62[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp75))))));
			fVbargraph63 = FAUSTFLOAT(fRec62[0]);
			output31[i] = FAUSTFLOAT(fTemp75);
			double fTemp76 = double(input32[i]);
			fRec65[0] = max((fRec65[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp76))))));
			fVbargraph64 = FAUSTFLOAT(fRec65[0]);
			double fTemp77 = (iSlow1?(iSlow4?(iSlow6?fTemp76:0.0):0.0):(iSlow2?(iSlow8?fTemp76:(3.3166247903553998 * fTemp76)):(iSlow3?0.0:(iSlow7?(0.30151134457776363 * fTemp76):fTemp76))));
			fRec64[0] = max((fRec64[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp77))))));
			fVbargraph65 = FAUSTFLOAT(fRec64[0]);
			output32[i] = FAUSTFLOAT(fTemp77);
			double fTemp78 = double(input33[i]);
			fRec67[0] = max((fRec67[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fVbargraph66 = FAUSTFLOAT(fRec67[0]);
			double fTemp79 = (iSlow1?(iSlow4?(iSlow6?fTemp78:0.0):0.0):(iSlow2?(iSlow8?fTemp78:(3.3166247903553998 * fTemp78)):(iSlow3?0.0:(iSlow7?(0.30151134457776363 * fTemp78):fTemp78))));
			fRec66[0] = max((fRec66[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp79))))));
			fVbargraph67 = FAUSTFLOAT(fRec66[0]);
			output33[i] = FAUSTFLOAT(fTemp79);
			double fTemp80 = double(input34[i]);
			fRec69[0] = max((fRec69[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp80))))));
			fVbargraph68 = FAUSTFLOAT(fRec69[0]);
			double fTemp81 = (iSlow1?(iSlow4?(iSlow6?fTemp80:0.0):0.0):(iSlow2?(iSlow8?fTemp80:(3.3166247903553998 * fTemp80)):(iSlow3?0.0:(iSlow7?(0.30151134457776363 * fTemp80):fTemp80))));
			fRec68[0] = max((fRec68[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp81))))));
			fVbargraph69 = FAUSTFLOAT(fRec68[0]);
			output34[i] = FAUSTFLOAT(fTemp81);
			double fTemp82 = double(input35[i]);
			fRec71[0] = max((fRec71[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp82))))));
			fVbargraph70 = FAUSTFLOAT(fRec71[0]);
			double fTemp83 = (iSlow1?(iSlow4?(iSlow6?fTemp82:0.0):0.0):(iSlow2?(iSlow8?fTemp82:(3.3166247903553998 * fTemp82)):(iSlow3?0.0:(iSlow7?(0.30151134457776363 * fTemp82):fTemp82))));
			fRec70[0] = max((fRec70[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp83))))));
			fVbargraph71 = FAUSTFLOAT(fRec70[0]);
			output35[i] = FAUSTFLOAT(fTemp83);
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec2[1] = fRec2[0];
			fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			fRec9[1] = fRec9[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			fRec8[1] = fRec8[0];
			fRec13[1] = fRec13[0];
			fRec14[1] = fRec14[0];
			fRec12[1] = fRec12[0];
			fRec15[1] = fRec15[0];
			fRec16[1] = fRec16[0];
			fRec17[1] = fRec17[0];
			fRec19[1] = fRec19[0];
			fRec20[1] = fRec20[0];
			fRec21[1] = fRec21[0];
			fRec18[1] = fRec18[0];
			fRec23[1] = fRec23[0];
			fRec24[1] = fRec24[0];
			fRec22[1] = fRec22[0];
			fRec26[1] = fRec26[0];
			fRec25[1] = fRec25[0];
			fRec28[1] = fRec28[0];
			fRec27[1] = fRec27[0];
			fRec29[1] = fRec29[0];
			fRec30[1] = fRec30[0];
			fRec31[1] = fRec31[0];
			fRec33[1] = fRec33[0];
			fRec32[1] = fRec32[0];
			fRec35[1] = fRec35[0];
			fRec34[1] = fRec34[0];
			fRec37[1] = fRec37[0];
			fRec36[1] = fRec36[0];
			fRec39[1] = fRec39[0];
			fRec38[1] = fRec38[0];
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fRec43[1] = fRec43[0];
			fRec42[1] = fRec42[0];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec47[1] = fRec47[0];
			fRec46[1] = fRec46[0];
			fRec49[1] = fRec49[0];
			fRec48[1] = fRec48[0];
			fRec51[1] = fRec51[0];
			fRec50[1] = fRec50[0];
			fRec53[1] = fRec53[0];
			fRec52[1] = fRec52[0];
			fRec55[1] = fRec55[0];
			fRec54[1] = fRec54[0];
			fRec57[1] = fRec57[0];
			fRec56[1] = fRec56[0];
			fRec59[1] = fRec59[0];
			fRec58[1] = fRec58[0];
			fRec61[1] = fRec61[0];
			fRec60[1] = fRec60[0];
			fRec63[1] = fRec63[0];
			fRec62[1] = fRec62[0];
			fRec65[1] = fRec65[0];
			fRec64[1] = fRec64[0];
			fRec67[1] = fRec67[0];
			fRec66[1] = fRec66[0];
			fRec69[1] = fRec69[0];
			fRec68[1] = fRec68[0];
			fRec71[1] = fRec71[0];
			fRec70[1] = fRec70[0];
			
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
