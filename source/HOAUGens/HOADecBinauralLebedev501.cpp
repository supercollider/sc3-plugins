/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2015"
license: "GPL)"
name: "HOADecBinauralLebedev501"
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

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
	
 private:
	
	int fSamplingFreq;
	double fConst0;
	FAUSTFLOAT fVslider0;
	double fRec1[2];
	FAUSTFLOAT fVslider1;
	double fRec3[2];
	double fRec2[2];
	FAUSTFLOAT fVbargraph0;
	int IOTA;
	double fVec0[128];
	double fRec4[2];
	FAUSTFLOAT fVbargraph1;
	double fVec1[128];
	double fRec5[2];
	FAUSTFLOAT fVbargraph2;
	double fVec2[128];
	double fRec6[2];
	FAUSTFLOAT fVbargraph3;
	double fVec3[128];
	double fRec0[2];
	FAUSTFLOAT fHbargraph0;
	double fRec7[2];
	FAUSTFLOAT fHbargraph1;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2015");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
		m->declare("license", "GPL)");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("name", "HOADecBinauralLebedev501");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 4;
		
	}
	virtual int getNumOutputs() {
		return 2;
		
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
		fVslider0 = FAUSTFLOAT(0.0);
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
		IOTA = 0;
		for (int l3 = 0; (l3 < 128); l3 = (l3 + 1)) {
			fVec0[l3] = 0.0;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec4[l4] = 0.0;
			
		}
		for (int l5 = 0; (l5 < 128); l5 = (l5 + 1)) {
			fVec1[l5] = 0.0;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec5[l6] = 0.0;
			
		}
		for (int l7 = 0; (l7 < 128); l7 = (l7 + 1)) {
			fVec2[l7] = 0.0;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec6[l8] = 0.0;
			
		}
		for (int l9 = 0; (l9 < 128); l9 = (l9 + 1)) {
			fVec3[l9] = 0.0;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec0[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec7[l11] = 0.0;
			
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
		ui_interface->openVerticalBox("HOADecBinauralLebedev501");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x442b120", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x4408a30", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x43dcb20", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x43b1140", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(&fVslider1, "1", "");
		ui_interface->declare(&fVslider1, "osc", "/levelin -10 10");
		ui_interface->declare(&fVslider1, "unit", "dB");
		ui_interface->addVerticalSlider("Inputs Gain", &fVslider1, 0.0, -10.0, 10.0, 0.10000000000000001);
		ui_interface->declare(&fVslider0, "2", "");
		ui_interface->declare(&fVslider0, "osc", "/levelout -10 10");
		ui_interface->declare(&fVslider0, "unit", "dB");
		ui_interface->addVerticalSlider("Outputs Gain", &fVslider0, 0.0, -10.0, 10.0, 0.10000000000000001);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("Outputs");
		ui_interface->openHorizontalBox("Left");
		ui_interface->declare(&fHbargraph0, "unit", "dB");
		ui_interface->addHorizontalBargraph("0x451a1b0", &fHbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Right");
		ui_interface->declare(&fHbargraph1, "unit", "dB");
		ui_interface->addHorizontalBargraph("0x45cf730", &fHbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* input2 = inputs[2];
		FAUSTFLOAT* input3 = inputs[3];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		double fSlow0 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider0))));
		double fSlow1 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider1))));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			fRec3[0] = (fSlow1 + (0.999 * fRec3[1]));
			double fTemp0 = (fRec3[0] * double(input3[i]));
			fRec2[0] = max((fRec2[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fVbargraph0 = FAUSTFLOAT(fRec2[0]);
			fVec0[(IOTA & 127)] = fTemp0;
			double fTemp1 = (fRec3[0] * double(input2[i]));
			fRec4[0] = max((fRec4[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp1))))));
			fVbargraph1 = FAUSTFLOAT(fRec4[0]);
			fVec1[(IOTA & 127)] = fTemp1;
			double fTemp2 = (fRec3[0] * double(input1[i]));
			fRec5[0] = max((fRec5[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp2))))));
			fVbargraph2 = FAUSTFLOAT(fRec5[0]);
			fVec2[(IOTA & 127)] = fTemp2;
			double fTemp3 = (fRec3[0] * double(input0[i]));
			fRec6[0] = max((fRec6[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp3))))));
			fVbargraph3 = FAUSTFLOAT(fRec6[0]);
			fVec3[(IOTA & 127)] = fTemp3;
			double fTemp4 = (fRec1[0] * (((2.4533100000000001e-06 * fVec0[((IOTA - 126) & 127)]) + ((1.70273e-06 * fVec0[((IOTA - 125) & 127)]) + ((8.0341699999999997e-06 * fVec0[((IOTA - 123) & 127)]) + ((3.8827600000000008e-05 * fVec0[((IOTA - 122) & 127)]) + ((0.000101833 * fVec0[((IOTA - 121) & 127)]) + ((0.000105639 * fVec0[((IOTA - 120) & 127)]) + ((3.9296600000000006e-05 * fVec0[((IOTA - 119) & 127)]) + ((3.85491e-05 * fVec0[((IOTA - 117) & 127)]) + ((0.00018456300000000003 * fVec0[((IOTA - 116) & 127)]) + ((0.000184012 * fVec0[((IOTA - 115) & 127)]) + ((0.0019589099999999999 * fVec0[((IOTA - 96) & 127)]) + ((0.0031713800000000001 * fVec0[((IOTA - 95) & 127)]) + ((0.00068253400000000005 * fVec0[((IOTA - 94) & 127)]) + ((0.00056285600000000003 * fVec0[((IOTA - 91) & 127)]) + ((0.0014036900000000002 * fVec0[((IOTA - 90) & 127)]) + ((0.00337025 * fVec0[((IOTA - 87) & 127)]) + ((0.0052786200000000004 * fVec0[((IOTA - 86) & 127)]) + ((0.0015441700000000001 * fVec0[((IOTA - 85) & 127)]) + ((0.00036768800000000002 * fVec0[((IOTA - 84) & 127)]) + ((0.00069545300000000007 * fVec0[((IOTA - 83) & 127)]) + ((0.0010311999999999999 * fVec0[((IOTA - 65) & 127)]) + ((0.00113331 * fVec0[((IOTA - 64) & 127)]) + ((0.0014698599999999999 * fVec0[((IOTA - 63) & 127)]) + ((0.0020663499999999998 * fVec0[((IOTA - 57) & 127)]) + ((0.00288332 * fVec0[((IOTA - 56) & 127)]) + ((0.0008817050000000001 * fVec0[((IOTA - 55) & 127)]) + ((0.0035919900000000002 * fVec0[((IOTA - 54) & 127)]) + ((0.0016634199999999999 * fVec0[((IOTA - 53) & 127)]) + ((0.0012247 * fVec0[((IOTA - 52) & 127)]) + ((0.00117636 * fVec0[((IOTA - 51) & 127)]) + ((0.0053477200000000003 * fVec0[((IOTA - 49) & 127)]) + ((0.00571347 * fVec0[((IOTA - 48) & 127)]) + ((0.0068109299999999998 * fVec0[((IOTA - 45) & 127)]) + ((0.00503769 * fVec0[((IOTA - 44) & 127)]) + ((0.0033690899999999999 * fVec0[((IOTA - 43) & 127)]) + ((0.0081425600000000001 * fVec0[((IOTA - 42) & 127)]) + ((0.0048383499999999999 * fVec0[((IOTA - 39) & 127)]) + ((1.73416e-05 * fVec0[((IOTA - 38) & 127)]) + ((0.0011593500000000002 * fVec0[((IOTA - 37) & 127)]) + ((0.00341282 * fVec0[((IOTA - 36) & 127)]) + ((0.0053672899999999994 * fVec0[((IOTA - 34) & 127)]) + ((0.0060068300000000003 * fVec0[((IOTA - 33) & 127)]) + ((0.036282299999999996 * fVec0[((IOTA - 30) & 127)]) + ((0.0096774400000000007 * fVec0[((IOTA - 29) & 127)]) + ((0.03066 * fVec0[((IOTA - 26) & 127)]) + ((0.0019489700000000002 * fVec0[((IOTA - 23) & 127)]) + ((9.4065400000000002e-05 * fVec0[((IOTA - 22) & 127)]) + ((0.000702438 * fVec0[((IOTA - 20) & 127)]) + ((0.0174746 * fVec0[((IOTA - 18) & 127)]) + ((0.021190899999999999 * fVec0[((IOTA - 17) & 127)]) + ((0.0025345699999999999 * fVec0[((IOTA - 16) & 127)]) + ((0.0025992000000000003 * fVec0[((IOTA - 15) & 127)]) + ((0.0061892400000000004 * fVec0[((IOTA - 13) & 127)]) + ((0.0020100199999999999 * fVec0[((IOTA - 10) & 127)]) + ((0.00148817 * fVec0[((IOTA - 8) & 127)]) + ((0.00053443399999999997 * fVec0[((IOTA - 6) & 127)]) + ((0.00069470999999999997 * fVec0[((IOTA - 4) & 127)]) + ((0.00013874000000000002 * fVec0[((IOTA - 2) & 127)]) + ((2.49699e-05 * fVec0[((IOTA - 1) & 127)]) + ((9.9362800000000004e-06 * fTemp0) + ((2.4082099999999999e-06 * fVec1[((IOTA - 126) & 127)]) + ((8.464169999999999e-06 * fVec1[((IOTA - 125) & 127)]) + ((8.8836499999999992e-06 * fVec1[((IOTA - 124) & 127)]) + ((8.7967399999999994e-06 * fVec1[((IOTA - 123) & 127)]) + ((2.4375700000000001e-05 * fVec1[((IOTA - 122) & 127)]) + ((8.0066800000000004e-05 * fVec1[((IOTA - 121) & 127)]) + ((9.9426100000000014e-05 * fVec1[((IOTA - 120) & 127)]) + ((9.0816700000000014e-05 * fVec1[((IOTA - 119) & 127)]) + ((5.80835e-05 * fVec1[((IOTA - 118) & 127)]) + ((7.7452699999999998e-05 * fVec1[((IOTA - 117) & 127)]) + ((9.5024799999999997e-07 * fVec1[((IOTA - 116) & 127)]) + ((5.1149400000000002e-05 * fVec1[((IOTA - 98) & 127)]) + ((0.00032476299999999999 * fVec1[((IOTA - 97) & 127)]) + ((0.0012872900000000002 * fVec1[((IOTA - 94) & 127)]) + ((0.0022912900000000001 * fVec1[((IOTA - 93) & 127)]) + ((0.00119739 * fVec1[((IOTA - 92) & 127)]) + ((0.00070883900000000006 * fVec0[((IOTA - 62) & 127)]) + ((0.0019518999999999999 * fVec1[((IOTA - 90) & 127)]) + ((0.0043689900000000005 * fVec1[((IOTA - 89) & 127)]) + ((0.00375801 * fVec1[((IOTA - 88) & 127)]) + ((0.0016994499999999999 * fVec1[((IOTA - 87) & 127)]) + ((0.0032713800000000004 * fVec1[((IOTA - 86) & 127)]) + ((0.0036832600000000003 * fVec1[((IOTA - 85) & 127)]) + ((0.00089328000000000009 * fVec1[((IOTA - 84) & 127)]) + ((0.00072158200000000002 * fVec1[((IOTA - 83) & 127)]) + ((0.00038337700000000001 * fVec1[((IOTA - 80) & 127)]) + ((0.0016079900000000001 * fVec1[((IOTA - 79) & 127)]) + ((0.0020680200000000003 * fVec1[((IOTA - 78) & 127)]) + ((0.00028545500000000005 * fVec1[((IOTA - 77) & 127)]) + ((0.00025900400000000002 * fVec1[((IOTA - 75) & 127)]) + ((3.3718100000000005e-05 * fVec1[((IOTA - 63) & 127)]) + ((0.0016691100000000001 * fVec1[((IOTA - 62) & 127)]) + ((0.00089519800000000015 * fVec1[((IOTA - 60) & 127)]) + ((0.00079125400000000002 * fVec1[((IOTA - 59) & 127)]) + ((0.00099548200000000001 * fVec1[((IOTA - 58) & 127)]) + ((0.0024358800000000001 * fVec1[((IOTA - 54) & 127)]) + ((0.0056765999999999995 * fVec1[((IOTA - 53) & 127)]) + ((0.0027909200000000001 * fVec1[((IOTA - 52) & 127)]) + ((0.00077908600000000012 * fVec1[((IOTA - 51) & 127)]) + ((0.0016214599999999999 * fVec1[((IOTA - 50) & 127)]) + ((0.0042173100000000002 * fVec1[((IOTA - 48) & 127)]) + ((0.0091871399999999999 * fVec1[((IOTA - 47) & 127)]) + ((0.0013077200000000001 * fVec1[((IOTA - 46) & 127)]) + ((0.0021427500000000001 * fVec1[((IOTA - 45) & 127)]) + ((0.0022329500000000005 * fVec1[((IOTA - 44) & 127)]) + ((0.0028589500000000003 * fVec1[((IOTA - 42) & 127)]) + ((0.0070971000000000003 * fVec1[((IOTA - 41) & 127)]) + ((0.0053105900000000004 * fVec1[((IOTA - 40) & 127)]) + ((0.0030150300000000001 * fVec1[((IOTA - 39) & 127)]) + ((0.00053429399999999994 * fVec1[((IOTA - 36) & 127)]) + ((0.0164669 * fVec1[((IOTA - 35) & 127)]) + ((0.0086483100000000011 * fVec1[((IOTA - 32) & 127)]) + ((0.021576300000000003 * fVec1[((IOTA - 29) & 127)]) + ((0.021685099999999999 * fVec1[((IOTA - 28) & 127)]) + ((0.0057969700000000002 * fVec1[((IOTA - 26) & 127)]) + ((0.0057622799999999998 * fVec1[((IOTA - 24) & 127)]) + ((0.0197594 * fVec1[((IOTA - 21) & 127)]) + ((0.0121098 * fVec1[((IOTA - 20) & 127)]) + ((0.032535399999999999 * fVec1[((IOTA - 17) & 127)]) + ((0.00050339700000000005 * fVec1[((IOTA - 14) & 127)]) + ((0.00086802899999999998 * fVec1[((IOTA - 13) & 127)]) + ((0.0016583500000000001 * fVec1[((IOTA - 8) & 127)]) + ((0.00031507000000000003 * fVec1[((IOTA - 5) & 127)]) + ((0.00049461800000000005 * fVec1[((IOTA - 4) & 127)]) + ((0.00015840400000000001 * fVec1[((IOTA - 2) & 127)]) + ((2.8519800000000004e-05 * fVec1[((IOTA - 1) & 127)]) + ((4.4578800000000004e-06 * fTemp1) + ((1.1796500000000001e-05 * fVec2[((IOTA - 114) & 127)]) + ((0.00041981599999999999 * fVec2[((IOTA - 107) & 127)]) + ((5.3605900000000008e-05 * fVec2[((IOTA - 105) & 127)]) + ((0.00046969700000000004 * fVec2[((IOTA - 104) & 127)]) + ((0.00047704000000000008 * fVec2[((IOTA - 103) & 127)]) + ((0.00043236800000000009 * fVec2[((IOTA - 102) & 127)]) + ((0.00070973900000000003 * fVec2[((IOTA - 101) & 127)]) + ((0.000705385 * fVec2[((IOTA - 100) & 127)]) + ((0.00045614100000000008 * fVec2[((IOTA - 99) & 127)]) + ((0.00092227100000000001 * fVec2[((IOTA - 98) & 127)]) + ((0.00081300600000000002 * fVec2[((IOTA - 97) & 127)]) + ((0.000623519 * fVec2[((IOTA - 96) & 127)]) + ((0.0013659500000000001 * fVec2[((IOTA - 95) & 127)]) + ((0.00214314 * fVec2[((IOTA - 94) & 127)]) + ((0.0016777999999999999 * fVec2[((IOTA - 93) & 127)]) + ((0.00108099 * fVec2[((IOTA - 92) & 127)]) + ((6.7984800000000007e-05 * fVec2[((IOTA - 90) & 127)]) + ((0.000264467 * fVec2[((IOTA - 89) & 127)]) + ((0.00045975200000000007 * fVec2[((IOTA - 88) & 127)]) + ((0.0013345100000000001 * fVec2[((IOTA - 87) & 127)]) + ((0.0013671599999999999 * fVec2[((IOTA - 86) & 127)]) + ((0.00088433700000000001 * fVec2[((IOTA - 85) & 127)]) + ((0.0010498400000000002 * fVec2[((IOTA - 84) & 127)]) + ((0.00300031 * fVec2[((IOTA - 83) & 127)]) + ((0.00033132700000000002 * fVec2[((IOTA - 82) & 127)]) + ((0.0037660800000000002 * fVec1[((IOTA - 33) & 127)]) + ((2.3648600000000003e-05 * fVec2[((IOTA - 80) & 127)]) + ((0.00058793100000000009 * fVec2[((IOTA - 79) & 127)]) + ((0.0027816299999999998 * fVec2[((IOTA - 78) & 127)]) + ((0.0017765299999999999 * fVec2[((IOTA - 77) & 127)]) + ((0.00046795000000000002 * fVec2[((IOTA - 73) & 127)]) + ((0.0016152100000000002 * fVec2[((IOTA - 72) & 127)]) + ((0.00010376700000000001 * fVec2[((IOTA - 71) & 127)]) + ((0.058508199999999996 * fVec2[((IOTA - 22) & 127)]) + ((0.050635700000000006 * fVec2[((IOTA - 21) & 127)]) + ((0.041341200000000002 * fVec2[((IOTA - 20) & 127)]) + ((0.0214629 * fVec2[((IOTA - 19) & 127)]) + ((0.029345699999999999 * fVec2[((IOTA - 18) & 127)]) + ((0.088954799999999987 * fVec2[((IOTA - 17) & 127)]) + ((0.078143400000000002 * fVec2[((IOTA - 15) & 127)]) + ((0.060886200000000001 * fVec2[((IOTA - 14) & 127)]) + ((0.077131900000000003 * fVec2[((IOTA - 13) & 127)]) + ((0.090551499999999993 * fVec2[((IOTA - 12) & 127)]) + ((0.0054581200000000003 * fVec2[((IOTA - 11) & 127)]) + ((0.0018110400000000001 * fVec2[((IOTA - 9) & 127)]) + ((0.00194177 * fVec2[((IOTA - 7) & 127)]) + ((0.00114689 * fVec2[((IOTA - 5) & 127)]) + ((0.00058690300000000006 * fVec2[((IOTA - 4) & 127)]) + ((0.0012299100000000001 * fVec2[((IOTA - 3) & 127)]) + ((0.00015469 * fVec2[((IOTA - 1) & 127)]) + ((3.3955100000000003e-05 * fTemp2) + ((2.0625300000000001e-06 * fVec3[((IOTA - 126) & 127)]) + ((5.0813899999999995e-06 * fVec3[((IOTA - 125) & 127)]) + ((5.7126099999999992e-06 * fVec3[((IOTA - 124) & 127)]) + ((2.09787e-05 * fVec3[((IOTA - 123) & 127)]) + ((1.8717900000000003e-05 * fVec3[((IOTA - 122) & 127)]) + ((4.2541300000000006e-05 * fVec3[((IOTA - 121) & 127)]) + ((0.00010654600000000001 * fVec3[((IOTA - 120) & 127)]) + ((0.00016404399999999999 * fVec3[((IOTA - 119) & 127)]) + ((0.00016872499999999999 * fVec3[((IOTA - 118) & 127)]) + ((0.00025855300000000002 * fVec3[((IOTA - 117) & 127)]) + ((0.00034160000000000001 * fVec3[((IOTA - 116) & 127)]) + ((0.00045382400000000002 * fVec3[((IOTA - 115) & 127)]) + ((0.00054736599999999998 * fVec3[((IOTA - 114) & 127)]) + ((0.00053358400000000003 * fVec3[((IOTA - 113) & 127)]) + ((0.00050102000000000002 * fVec3[((IOTA - 112) & 127)]) + ((0.00067625599999999997 * fVec3[((IOTA - 111) & 127)]) + ((0.00098166300000000002 * fVec3[((IOTA - 110) & 127)]) + ((0.00083451500000000008 * fVec3[((IOTA - 109) & 127)]) + ((0.0009436510000000001 * fVec3[((IOTA - 108) & 127)]) + ((0.0012923000000000001 * fVec3[((IOTA - 107) & 127)]) + ((0.00136699 * fVec3[((IOTA - 106) & 127)]) + ((0.0014059200000000002 * fVec3[((IOTA - 105) & 127)]) + ((0.0012065399999999999 * fVec3[((IOTA - 104) & 127)]) + ((0.00185348 * fVec3[((IOTA - 103) & 127)]) + ((0.00154024 * fVec3[((IOTA - 102) & 127)]) + ((0.00087727600000000008 * fVec3[((IOTA - 101) & 127)]) + ((0.00050683400000000006 * fVec3[((IOTA - 100) & 127)]) + ((0.00061282500000000011 * fVec3[((IOTA - 99) & 127)]) + ((0.00058470199999999999 * fVec3[((IOTA - 98) & 127)]) + ((0.00054874000000000004 * fVec3[((IOTA - 96) & 127)]) + ((0.00272945 * fVec3[((IOTA - 95) & 127)]) + ((0.0021461499999999999 * fVec3[((IOTA - 94) & 127)]) + ((2.3955000000000004e-05 * fVec3[((IOTA - 93) & 127)]) + ((0.00041329900000000006 * fVec3[((IOTA - 90) & 127)]) + ((0.0037083100000000003 * fVec3[((IOTA - 87) & 127)]) + ((0.0051757600000000006 * fVec3[((IOTA - 86) & 127)]) + ((0.0016799500000000001 * fVec3[((IOTA - 85) & 127)]) + ((0.0015701700000000001 * fVec3[((IOTA - 83) & 127)]) + ((0.00056369199999999997 * fVec3[((IOTA - 82) & 127)]) + ((0.0024365300000000001 * fVec3[((IOTA - 80) & 127)]) + ((0.0041905200000000005 * fVec3[((IOTA - 79) & 127)]) + ((0.0024405 * fVec3[((IOTA - 78) & 127)]) + ((0.00082847699999999999 * fVec3[((IOTA - 77) & 127)]) + ((0.0041945800000000007 * fVec3[((IOTA - 76) & 127)]) + ((0.0063743100000000002 * fVec3[((IOTA - 75) & 127)]) + ((0.0029129100000000003 * fVec3[((IOTA - 74) & 127)]) + ((0.0030868900000000001 * fVec3[((IOTA - 73) & 127)]) + ((0.0052356299999999998 * fVec3[((IOTA - 72) & 127)]) + ((0.0032107500000000001 * fVec3[((IOTA - 71) & 127)]) + ((0.00185049 * fVec3[((IOTA - 70) & 127)]) + ((0.0014647 * fVec3[((IOTA - 69) & 127)]) + ((0.0022559300000000002 * fVec3[((IOTA - 68) & 127)]) + ((0.0034284599999999999 * fVec3[((IOTA - 67) & 127)]) + ((0.0023089200000000003 * fVec3[((IOTA - 66) & 127)]) + ((0.0027403499999999999 * fVec3[((IOTA - 65) & 127)]) + ((0.0042152999999999999 * fVec3[((IOTA - 64) & 127)]) + ((0.0062385100000000001 * fVec3[((IOTA - 63) & 127)]) + ((0.0068040700000000006 * fVec3[((IOTA - 62) & 127)]) + ((0.0035775299999999998 * fVec3[((IOTA - 61) & 127)]) + ((0.0042250600000000001 * fVec3[((IOTA - 60) & 127)]) + ((0.0031211899999999998 * fVec3[((IOTA - 59) & 127)]) + ((0.0036129299999999999 * fVec3[((IOTA - 58) & 127)]) + ((0.0050028199999999998 * fVec3[((IOTA - 57) & 127)]) + ((0.0012118500000000002 * fVec3[((IOTA - 56) & 127)]) + ((0.0012461600000000001 * fVec3[((IOTA - 55) & 127)]) + ((0.00305273 * fVec3[((IOTA - 54) & 127)]) + ((0.00325793 * fVec3[((IOTA - 53) & 127)]) + ((0.0028833999999999999 * fVec3[((IOTA - 52) & 127)]) + ((0.0036652500000000001 * fVec3[((IOTA - 51) & 127)]) + ((0.00086377000000000014 * fVec3[((IOTA - 50) & 127)]) + ((0.00175234 * fVec3[((IOTA - 49) & 127)]) + ((0.00306785 * fVec3[((IOTA - 48) & 127)]) + ((0.00250454 * fVec3[((IOTA - 46) & 127)]) + ((0.015005800000000001 * fVec3[((IOTA - 45) & 127)]) + ((0.0079248200000000008 * fVec3[((IOTA - 44) & 127)]) + ((0.0028857800000000001 * fVec3[((IOTA - 43) & 127)]) + ((0.0074329900000000004 * fVec3[((IOTA - 42) & 127)]) + ((0.0056945699999999995 * fVec3[((IOTA - 41) & 127)]) + ((0.0064973000000000001 * fVec3[((IOTA - 40) & 127)]) + ((0.011589100000000001 * fVec3[((IOTA - 39) & 127)]) + ((0.0069301900000000001 * fVec3[((IOTA - 38) & 127)]) + ((0.0079812000000000008 * fVec3[((IOTA - 37) & 127)]) + ((0.0141227 * fVec3[((IOTA - 36) & 127)]) + ((0.0044680700000000002 * fVec3[((IOTA - 35) & 127)]) + ((0.020023599999999999 * fVec3[((IOTA - 34) & 127)]) + ((0.023001999999999998 * fVec3[((IOTA - 33) & 127)]) + ((0.0057082499999999998 * fVec3[((IOTA - 32) & 127)]) + ((0.011366300000000001 * fVec3[((IOTA - 31) & 127)]) + ((0.0266261 * fVec3[((IOTA - 30) & 127)]) + ((0.0087064099999999995 * fVec3[((IOTA - 29) & 127)]) + ((0.030624600000000002 * fVec3[((IOTA - 27) & 127)]) + ((0.0528391 * fVec3[((IOTA - 26) & 127)]) + ((0.031746299999999998 * fVec3[((IOTA - 25) & 127)]) + ((((((((((((((((((((((((2.5118200000000003e-05 * fTemp3) + (0.00012370799999999999 * fVec3[((IOTA - 1) & 127)])) + (0.00096838400000000008 * fVec3[((IOTA - 3) & 127)])) + (0.00059949000000000005 * fVec3[((IOTA - 4) & 127)])) + (0.00076772799999999997 * fVec3[((IOTA - 5) & 127)])) + (0.00014117900000000001 * fVec3[((IOTA - 6) & 127)])) + (0.0018634200000000002 * fVec3[((IOTA - 7) & 127)])) + (0.00024957099999999997 * fVec3[((IOTA - 8) & 127)])) + (0.0011589199999999999 * fVec3[((IOTA - 9) & 127)])) + (0.00050879500000000004 * fVec3[((IOTA - 10) & 127)])) + (0.0036302599999999997 * fVec3[((IOTA - 11) & 127)])) + (0.056510400000000002 * fVec3[((IOTA - 12) & 127)])) + (0.049342300000000006 * fVec3[((IOTA - 13) & 127)])) + (0.0449074 * fVec3[((IOTA - 14) & 127)])) + (0.064749100000000004 * fVec3[((IOTA - 15) & 127)])) + (0.059606500000000007 * fVec3[((IOTA - 16) & 127)])) + (0.073495100000000008 * fVec3[((IOTA - 17) & 127)])) + (0.034514800000000005 * fVec3[((IOTA - 18) & 127)])) + (0.055772500000000003 * fVec3[((IOTA - 19) & 127)])) + (0.06596840000000001 * fVec3[((IOTA - 20) & 127)])) + (0.057809200000000005 * fVec3[((IOTA - 21) & 127)])) + (0.096895100000000012 * fVec3[((IOTA - 22) & 127)])) + (0.021260899999999999 * fVec3[((IOTA - 23) & 127)])) + (0.063658400000000004 * fVec2[((IOTA - 16) & 127)]))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) - ((6.9716599999999994e-06 * fVec0[((IOTA - 124) & 127)]) + ((9.8485200000000004e-06 * fVec0[((IOTA - 118) & 127)]) + ((0.00010357900000000001 * fVec0[((IOTA - 114) & 127)]) + ((0.00042897299999999997 * fVec0[((IOTA - 113) & 127)]) + ((0.00029024100000000006 * fVec0[((IOTA - 112) & 127)]) + ((9.8297200000000012e-05 * fVec0[((IOTA - 111) & 127)]) + ((0.00020804999999999999 * fVec0[((IOTA - 110) & 127)]) + ((0.00054109500000000001 * fVec0[((IOTA - 109) & 127)]) + ((0.00083962700000000002 * fVec0[((IOTA - 108) & 127)]) + ((0.00056389100000000007 * fVec0[((IOTA - 107) & 127)]) + ((0.00051816800000000006 * fVec0[((IOTA - 106) & 127)]) + ((0.00065456900000000003 * fVec0[((IOTA - 105) & 127)]) + ((0.00088825400000000011 * fVec0[((IOTA - 104) & 127)]) + ((0.000446193 * fVec0[((IOTA - 103) & 127)]) + ((0.00071199100000000003 * fVec0[((IOTA - 102) & 127)]) + ((0.0012468500000000001 * fVec0[((IOTA - 101) & 127)]) + ((0.0012435300000000001 * fVec0[((IOTA - 100) & 127)]) + ((0.0010388900000000002 * fVec0[((IOTA - 99) & 127)]) + ((0.00170268 * fVec0[((IOTA - 98) & 127)]) + ((0.00126004 * fVec0[((IOTA - 97) & 127)]) + ((0.0013548899999999999 * fVec0[((IOTA - 93) & 127)]) + ((0.0020163799999999999 * fVec0[((IOTA - 92) & 127)]) + ((0.0010302600000000001 * fVec0[((IOTA - 89) & 127)]) + ((0.00142103 * fVec0[((IOTA - 88) & 127)]) + ((0.0020040399999999999 * fVec0[((IOTA - 82) & 127)]) + ((0.0030592000000000002 * fVec0[((IOTA - 81) & 127)]) + ((0.00082976100000000015 * fVec0[((IOTA - 80) & 127)]) + ((0.00083032800000000008 * fVec0[((IOTA - 79) & 127)]) + ((0.00146794 * fVec0[((IOTA - 78) & 127)]) + ((0.0029912300000000001 * fVec0[((IOTA - 77) & 127)]) + ((0.0019008400000000002 * fVec0[((IOTA - 76) & 127)]) + ((0.00032308700000000001 * fVec0[((IOTA - 75) & 127)]) + ((0.00121188 * fVec0[((IOTA - 74) & 127)]) + ((0.0015286800000000001 * fVec0[((IOTA - 73) & 127)]) + ((0.0016313200000000001 * fVec0[((IOTA - 72) & 127)]) + ((0.0019843199999999999 * fVec0[((IOTA - 71) & 127)]) + ((0.0031219500000000001 * fVec0[((IOTA - 70) & 127)]) + ((0.0017147500000000001 * fVec0[((IOTA - 69) & 127)]) + ((4.5241600000000004e-05 * fVec0[((IOTA - 68) & 127)]) + ((0.00193403 * fVec0[((IOTA - 67) & 127)]) + ((0.00103439 * fVec0[((IOTA - 66) & 127)]) + ((0.00025587599999999999 * fVec0[((IOTA - 61) & 127)]) + ((0.0014905299999999999 * fVec0[((IOTA - 60) & 127)]) + ((0.000502175 * fVec0[((IOTA - 59) & 127)]) + ((0.00055048599999999999 * fVec0[((IOTA - 58) & 127)]) + ((0.00049892100000000004 * fVec0[((IOTA - 50) & 127)]) + ((0.0029968600000000001 * fVec0[((IOTA - 47) & 127)]) + ((0.0024887899999999998 * fVec0[((IOTA - 46) & 127)]) + ((0.0030320900000000003 * fVec0[((IOTA - 41) & 127)]) + ((0.0076587399999999998 * fVec0[((IOTA - 40) & 127)]) + ((0.010064699999999999 * fVec0[((IOTA - 35) & 127)]) + ((0.030136699999999999 * fVec0[((IOTA - 32) & 127)]) + ((0.017341200000000001 * fVec0[((IOTA - 31) & 127)]) + ((0.039170099999999999 * fVec0[((IOTA - 28) & 127)]) + ((0.00031563800000000003 * fVec0[((IOTA - 27) & 127)]) + ((0.00069887100000000011 * fVec0[((IOTA - 25) & 127)]) + ((0.025458500000000002 * fVec0[((IOTA - 24) & 127)]) + ((0.0120184 * fVec0[((IOTA - 21) & 127)]) + ((0.011691100000000001 * fVec0[((IOTA - 19) & 127)]) + ((0.0053095800000000004 * fVec0[((IOTA - 14) & 127)]) + ((0.0072171800000000001 * fVec0[((IOTA - 12) & 127)]) + ((0.0030320200000000003 * fVec0[((IOTA - 11) & 127)]) + ((0.00131485 * fVec0[((IOTA - 9) & 127)]) + ((0.0011081700000000001 * fVec0[((IOTA - 7) & 127)]) + ((0.00036208399999999998 * fVec0[((IOTA - 5) & 127)]) + (((6.574880000000001e-05 * fVec1[((IOTA - 115) & 127)]) + ((7.5893800000000015e-05 * fVec1[((IOTA - 114) & 127)]) + ((0.00055035400000000008 * fVec1[((IOTA - 113) & 127)]) + ((0.00054230400000000007 * fVec1[((IOTA - 112) & 127)]) + ((0.00038862600000000004 * fVec1[((IOTA - 111) & 127)]) + ((0.00038355300000000002 * fVec1[((IOTA - 110) & 127)]) + ((0.0010789999999999999 * fVec1[((IOTA - 109) & 127)]) + ((0.00127535 * fVec1[((IOTA - 108) & 127)]) + ((0.00026040700000000004 * fVec1[((IOTA - 107) & 127)]) + ((0.00064190699999999994 * fVec1[((IOTA - 106) & 127)]) + ((0.0012643700000000001 * fVec1[((IOTA - 105) & 127)]) + ((0.0014951399999999998 * fVec1[((IOTA - 104) & 127)]) + ((0.00096466700000000011 * fVec1[((IOTA - 103) & 127)]) + ((0.00062200899999999997 * fVec1[((IOTA - 102) & 127)]) + ((0.0013151 * fVec1[((IOTA - 101) & 127)]) + ((0.00155276 * fVec1[((IOTA - 100) & 127)]) + ((0.00147963 * fVec1[((IOTA - 99) & 127)]) + ((0.0012132899999999999 * fVec1[((IOTA - 96) & 127)]) + ((0.0010497099999999999 * fVec1[((IOTA - 95) & 127)]) + ((5.4447799999999994e-06 * fVec1[((IOTA - 91) & 127)]) + ((0.00060830200000000002 * fVec1[((IOTA - 82) & 127)]) + ((0.00073753600000000001 * fVec1[((IOTA - 81) & 127)]) + ((4.1098800000000007e-05 * fVec1[((IOTA - 76) & 127)]) + ((0.00031511100000000003 * fVec1[((IOTA - 74) & 127)]) + ((0.0011385200000000001 * fVec1[((IOTA - 73) & 127)]) + ((0.0023925600000000002 * fVec1[((IOTA - 72) & 127)]) + ((0.00191902 * fVec1[((IOTA - 71) & 127)]) + ((0.00073584900000000001 * fVec1[((IOTA - 70) & 127)]) + ((0.0033698500000000002 * fVec1[((IOTA - 69) & 127)]) + ((0.0046562499999999998 * fVec1[((IOTA - 68) & 127)]) + ((0.0026088700000000001 * fVec1[((IOTA - 67) & 127)]) + ((0.0015381499999999998 * fVec1[((IOTA - 66) & 127)]) + ((0.00177024 * fVec1[((IOTA - 65) & 127)]) + ((0.0027169400000000002 * fVec1[((IOTA - 64) & 127)]) + ((0.0011334100000000001 * fVec1[((IOTA - 61) & 127)]) + ((0.0012374899999999999 * fVec1[((IOTA - 57) & 127)]) + ((0.00350462 * fVec1[((IOTA - 56) & 127)]) + ((0.00029472900000000003 * fVec1[((IOTA - 55) & 127)]) + ((0.00161133 * fVec1[((IOTA - 49) & 127)]) + ((0.0043256300000000004 * fVec1[((IOTA - 43) & 127)]) + ((0.0039263700000000002 * fVec1[((IOTA - 38) & 127)]) + ((0.0193136 * fVec1[((IOTA - 37) & 127)]) + ((0.0137585 * fVec1[((IOTA - 34) & 127)]) + ((0.016165499999999999 * fVec1[((IOTA - 31) & 127)]) + ((0.0200181 * fVec1[((IOTA - 30) & 127)]) + ((0.00436148 * fVec1[((IOTA - 27) & 127)]) + ((0.0115485 * fVec1[((IOTA - 25) & 127)]) + ((0.0032127699999999998 * fVec1[((IOTA - 23) & 127)]) + ((0.0057804000000000006 * fVec1[((IOTA - 22) & 127)]) + ((0.037111700000000004 * fVec1[((IOTA - 19) & 127)]) + ((0.014907300000000002 * fVec1[((IOTA - 18) & 127)]) + ((0.0092683000000000001 * fVec1[((IOTA - 16) & 127)]) + ((0.0158698 * fVec1[((IOTA - 15) & 127)]) + ((0.00489909 * fVec1[((IOTA - 12) & 127)]) + ((0.00036272099999999999 * fVec1[((IOTA - 11) & 127)]) + ((0.00043871700000000003 * fVec1[((IOTA - 10) & 127)]) + ((0.00018132700000000001 * fVec1[((IOTA - 9) & 127)]) + ((0.00097058900000000011 * fVec1[((IOTA - 7) & 127)]) + ((0.00016036100000000001 * fVec1[((IOTA - 6) & 127)]) + (((1.8656099999999998e-06 * fVec2[((IOTA - 126) & 127)]) + ((2.5639300000000001e-06 * fVec2[((IOTA - 125) & 127)]) + ((8.3185099999999991e-06 * fVec2[((IOTA - 124) & 127)]) + ((3.8801999999999999e-05 * fVec2[((IOTA - 123) & 127)]) + ((6.5230500000000005e-05 * fVec2[((IOTA - 122) & 127)]) + ((4.81323e-05 * fVec2[((IOTA - 121) & 127)]) + ((3.1175000000000006e-05 * fVec2[((IOTA - 120) & 127)]) + ((1.1129000000000001e-05 * fVec2[((IOTA - 119) & 127)]) + ((0.00016385400000000001 * fVec2[((IOTA - 118) & 127)]) + ((0.000137362 * fVec2[((IOTA - 117) & 127)]) + ((9.139900000000001e-05 * fVec2[((IOTA - 116) & 127)]) + ((0.00012562199999999998 * fVec2[((IOTA - 115) & 127)]) + ((0.00025063300000000005 * fVec2[((IOTA - 113) & 127)]) + ((0.000272806 * fVec2[((IOTA - 112) & 127)]) + ((0.000314307 * fVec2[((IOTA - 111) & 127)]) + ((0.00026935700000000002 * fVec2[((IOTA - 110) & 127)]) + ((0.00027303400000000002 * fVec2[((IOTA - 109) & 127)]) + ((8.74662e-06 * fVec2[((IOTA - 108) & 127)]) + ((0.00035653000000000003 * fVec2[((IOTA - 106) & 127)]) + ((0.000138672 * fVec2[((IOTA - 91) & 127)]) + ((0.0011749499999999999 * fVec2[((IOTA - 81) & 127)]) + ((0.00092974400000000003 * fVec2[((IOTA - 76) & 127)]) + ((0.00189912 * fVec2[((IOTA - 75) & 127)]) + ((0.0018498099999999999 * fVec2[((IOTA - 74) & 127)]) + ((0.00102903 * fVec2[((IOTA - 70) & 127)]) + ((0.0023879000000000001 * fVec2[((IOTA - 69) & 127)]) + ((0.0030104600000000004 * fVec2[((IOTA - 68) & 127)]) + ((0.00157916 * fVec2[((IOTA - 67) & 127)]) + ((0.0021747000000000003 * fVec2[((IOTA - 66) & 127)]) + ((0.0024374800000000001 * fVec2[((IOTA - 65) & 127)]) + ((0.0017056100000000002 * fVec2[((IOTA - 64) & 127)]) + ((0.00246014 * fVec2[((IOTA - 63) & 127)]) + ((0.0044470300000000003 * fVec2[((IOTA - 62) & 127)]) + ((0.0047484400000000005 * fVec2[((IOTA - 61) & 127)]) + ((0.0027951100000000004 * fVec2[((IOTA - 60) & 127)]) + ((0.0047547600000000002 * fVec2[((IOTA - 59) & 127)]) + ((0.0037710400000000002 * fVec2[((IOTA - 58) & 127)]) + ((0.0045419900000000001 * fVec2[((IOTA - 57) & 127)]) + ((0.010441600000000001 * fVec2[((IOTA - 56) & 127)]) + ((0.0100582 * fVec2[((IOTA - 55) & 127)]) + ((0.0076609499999999997 * fVec2[((IOTA - 54) & 127)]) + ((0.0068764300000000002 * fVec2[((IOTA - 53) & 127)]) + ((0.0095819500000000005 * fVec2[((IOTA - 52) & 127)]) + ((0.015913200000000002 * fVec2[((IOTA - 51) & 127)]) + ((0.015119400000000002 * fVec2[((IOTA - 50) & 127)]) + ((0.0128559 * fVec2[((IOTA - 49) & 127)]) + ((0.016606200000000002 * fVec2[((IOTA - 48) & 127)]) + ((0.016194200000000002 * fVec2[((IOTA - 47) & 127)]) + ((0.0116756 * fVec2[((IOTA - 46) & 127)]) + ((0.008875280000000001 * fVec2[((IOTA - 45) & 127)]) + ((0.016079200000000002 * fVec2[((IOTA - 44) & 127)]) + ((0.0188738 * fVec2[((IOTA - 43) & 127)]) + ((0.022184499999999999 * fVec2[((IOTA - 42) & 127)]) + ((0.020373700000000002 * fVec2[((IOTA - 41) & 127)]) + ((0.0086227300000000003 * fVec2[((IOTA - 40) & 127)]) + ((0.015368999999999999 * fVec2[((IOTA - 39) & 127)]) + ((0.025263300000000002 * fVec2[((IOTA - 38) & 127)]) + ((0.026966700000000003 * fVec2[((IOTA - 37) & 127)]) + ((0.023967600000000002 * fVec2[((IOTA - 36) & 127)]) + ((0.0180915 * fVec2[((IOTA - 35) & 127)]) + ((0.026436999999999999 * fVec2[((IOTA - 34) & 127)]) + ((0.035762599999999999 * fVec2[((IOTA - 33) & 127)]) + ((0.019380700000000001 * fVec2[((IOTA - 32) & 127)]) + ((0.0052166499999999998 * fVec2[((IOTA - 31) & 127)]) + ((0.012525700000000001 * fVec2[((IOTA - 30) & 127)]) + ((0.039007399999999998 * fVec2[((IOTA - 29) & 127)]) + ((0.039406500000000004 * fVec2[((IOTA - 28) & 127)]) + ((0.0160175 * fVec2[((IOTA - 27) & 127)]) + ((0.0095142000000000004 * fVec2[((IOTA - 26) & 127)]) + ((0.00060328500000000006 * fVec2[((IOTA - 25) & 127)]) + ((0.042794800000000008 * fVec2[((IOTA - 24) & 127)]) + ((0.034687500000000003 * fVec2[((IOTA - 23) & 127)]) + ((0.0011779100000000001 * fVec2[((IOTA - 10) & 127)]) + ((0.000167329 * fVec2[((IOTA - 8) & 127)]) + ((0.00041459900000000003 * fVec2[((IOTA - 6) & 127)]) + ((((((((((((0.00017378000000000001 * fVec3[((IOTA - 2) & 127)]) + (0.015415000000000002 * fVec3[((IOTA - 24) & 127)])) + (0.0114164 * fVec3[((IOTA - 28) & 127)])) + (0.0029673899999999999 * fVec3[((IOTA - 47) & 127)])) + (0.0010694400000000001 * fVec3[((IOTA - 81) & 127)])) + (0.000311911 * fVec3[((IOTA - 84) & 127)])) + (0.00141242 * fVec3[((IOTA - 88) & 127)])) + (0.00134669 * fVec3[((IOTA - 89) & 127)])) + (0.0014838199999999998 * fVec3[((IOTA - 91) & 127)])) + (0.0024969700000000003 * fVec3[((IOTA - 92) & 127)])) + (0.00054486499999999995 * fVec3[((IOTA - 97) & 127)])) + (0.00031506200000000001 * fVec2[((IOTA - 2) & 127)]))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.00019245000000000002 * fVec1[((IOTA - 3) & 127)]))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.00017773600000000001 * fVec0[((IOTA - 3) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
			fRec0[0] = max((fRec0[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fHbargraph0 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp4);
			double fTemp5 = (fRec1[0] * (((1.4188099999999999e-06 * fVec0[((IOTA - 126) & 127)]) + ((3.3187600000000001e-06 * fVec0[((IOTA - 125) & 127)]) + ((5.1482899999999996e-06 * fVec0[((IOTA - 124) & 127)]) + ((8.7600299999999995e-06 * fVec0[((IOTA - 123) & 127)]) + ((5.5660599999999997e-06 * fVec0[((IOTA - 122) & 127)]) + ((5.530960000000001e-05 * fVec0[((IOTA - 121) & 127)]) + ((0.00011918500000000002 * fVec0[((IOTA - 120) & 127)]) + ((9.8678100000000013e-05 * fVec0[((IOTA - 119) & 127)]) + ((4.2207000000000004e-05 * fVec0[((IOTA - 118) & 127)]) + ((3.2490500000000003e-05 * fVec0[((IOTA - 117) & 127)]) + ((0.00011697900000000002 * fVec0[((IOTA - 116) & 127)]) + ((5.6691300000000004e-05 * fVec0[((IOTA - 112) & 127)]) + ((0.000115798 * fVec0[((IOTA - 111) & 127)]) + ((0.0037339199999999999 * fVec0[((IOTA - 96) & 127)]) + ((0.0027517499999999999 * fVec0[((IOTA - 95) & 127)]) + ((0.00065997500000000004 * fVec0[((IOTA - 92) & 127)]) + ((0.0016790700000000002 * fVec0[((IOTA - 91) & 127)]) + ((0.0019817300000000001 * fVec0[((IOTA - 88) & 127)]) + ((0.0040626200000000003 * fVec0[((IOTA - 87) & 127)]) + ((0.0023261500000000004 * fVec0[((IOTA - 86) & 127)]) + ((0.0015044699999999999 * fVec0[((IOTA - 85) & 127)]) + ((0.0010922900000000001 * fVec0[((IOTA - 84) & 127)]) + ((0.0012104800000000001 * fVec0[((IOTA - 64) & 127)]) + ((0.0039074399999999999 * fVec0[((IOTA - 63) & 127)]) + ((0.00070128400000000004 * fVec0[((IOTA - 59) & 127)]) + ((0.00040873600000000004 * fVec0[((IOTA - 58) & 127)]) + ((0.0011136799999999999 * fVec0[((IOTA - 57) & 127)]) + ((0.0035448200000000002 * fVec0[((IOTA - 56) & 127)]) + ((0.0022150400000000002 * fVec0[((IOTA - 55) & 127)]) + ((3.8517100000000004e-05 * fVec0[((IOTA - 54) & 127)]) + ((0.0011952499999999999 * fVec0[((IOTA - 53) & 127)]) + ((0.00017855099999999999 * fVec0[((IOTA - 52) & 127)]) + ((0.0020778099999999998 * fVec0[((IOTA - 51) & 127)]) + ((0.0048699899999999994 * fVec0[((IOTA - 50) & 127)]) + ((0.0020168600000000001 * fVec0[((IOTA - 49) & 127)]) + ((0.0011997199999999998 * fVec0[((IOTA - 47) & 127)]) + ((0.0061526599999999999 * fVec0[((IOTA - 46) & 127)]) + ((0.0028706900000000004 * fVec0[((IOTA - 44) & 127)]) + ((0.011077500000000001 * fVec0[((IOTA - 43) & 127)]) + ((0.0022290399999999998 * fVec0[((IOTA - 42) & 127)]) + ((0.0084232300000000003 * fVec0[((IOTA - 39) & 127)]) + ((0.00127378 * fVec0[((IOTA - 38) & 127)]) + ((0.0125606 * fVec0[((IOTA - 34) & 127)]) + ((0.011542200000000001 * fVec0[((IOTA - 31) & 127)]) + ((0.024367400000000001 * fVec0[((IOTA - 30) & 127)]) + ((0.0537481 * fVec0[((IOTA - 26) & 127)]) + ((0.013059400000000001 * fVec0[((IOTA - 23) & 127)]) + ((0.021176 * fVec0[((IOTA - 22) & 127)]) + ((0.0292041 * fVec0[((IOTA - 18) & 127)]) + ((0.015611 * fVec0[((IOTA - 17) & 127)]) + ((0.0131959 * fVec0[((IOTA - 15) & 127)]) + ((0.0033820999999999999 * fVec0[((IOTA - 13) & 127)]) + ((0.00065433400000000001 * fVec0[((IOTA - 10) & 127)]) + ((0.00012754700000000002 * fVec0[((IOTA - 9) & 127)]) + ((0.00028600400000000002 * fVec0[((IOTA - 8) & 127)]) + ((0.00056964699999999995 * fVec0[((IOTA - 6) & 127)]) + ((0.00055874899999999999 * fVec0[((IOTA - 4) & 127)]) + ((0.000107706 * fVec0[((IOTA - 2) & 127)]) + ((2.3258800000000005e-05 * fVec0[((IOTA - 1) & 127)]) + ((1.5002200000000002e-05 * fTemp0) + ((1.8648999999999999e-06 * fVec1[((IOTA - 126) & 127)]) + ((1.0045e-05 * fVec1[((IOTA - 125) & 127)]) + ((1.8977800000000003e-05 * fVec1[((IOTA - 124) & 127)]) + ((1.9873400000000002e-05 * fVec1[((IOTA - 123) & 127)]) + ((3.4954000000000004e-05 * fVec1[((IOTA - 122) & 127)]) + ((9.3229500000000019e-05 * fVec1[((IOTA - 121) & 127)]) + ((0.00012240800000000001 * fVec1[((IOTA - 120) & 127)]) + ((9.5701199999999998e-05 * fVec1[((IOTA - 119) & 127)]) + ((5.5900900000000005e-05 * fVec1[((IOTA - 115) & 127)]) + ((1.9102600000000002e-05 * fVec1[((IOTA - 114) & 127)]) + (((0.000876507 * fVec1[((IOTA - 91) & 127)]) + ((0.0020406000000000001 * fVec1[((IOTA - 90) & 127)]) + ((0.0013280499999999999 * fVec1[((IOTA - 89) & 127)]) + ((0.0017303100000000001 * fVec1[((IOTA - 88) & 127)]) + ((0.0029776099999999999 * fVec1[((IOTA - 87) & 127)]) + ((0.0025195700000000001 * fVec1[((IOTA - 86) & 127)]) + ((0.0011889400000000001 * fVec1[((IOTA - 83) & 127)]) + ((0.000386788 * fVec1[((IOTA - 81) & 127)]) + ((0.00090610300000000012 * fVec1[((IOTA - 80) & 127)]) + ((0.0014508500000000001 * fVec1[((IOTA - 79) & 127)]) + ((0.00082739399999999996 * fVec1[((IOTA - 78) & 127)]) + ((0.00013990400000000002 * fVec1[((IOTA - 76) & 127)]) + ((0.00174807 * fVec1[((IOTA - 75) & 127)]) + ((0.00102092 * fVec1[((IOTA - 74) & 127)]) + ((0.00235812 * fVec1[((IOTA - 63) & 127)]) + ((0.0021519 * fVec1[((IOTA - 62) & 127)]) + ((7.6654099999999985e-06 * fVec1[((IOTA - 61) & 127)]) + ((0.0022741699999999998 * fVec1[((IOTA - 55) & 127)]) + ((0.0042530099999999998 * fVec1[((IOTA - 54) & 127)]) + ((0.0032282000000000001 * fVec1[((IOTA - 53) & 127)]) + ((0.0041592900000000004 * fVec1[((IOTA - 50) & 127)]) + ((0.0075640600000000001 * fVec1[((IOTA - 49) & 127)]) + ((0.0079603199999999999 * fVec1[((IOTA - 48) & 127)]) + ((0.0030812000000000001 * fVec1[((IOTA - 47) & 127)]) + ((0.00018497500000000001 * fVec1[((IOTA - 46) & 127)]) + ((0.00033097600000000002 * fVec1[((IOTA - 43) & 127)]) + ((0.0066906600000000002 * fVec1[((IOTA - 42) & 127)]) + ((0.0081795500000000007 * fVec1[((IOTA - 41) & 127)]) + ((0.00108066 * fVec1[((IOTA - 40) & 127)]) + ((0.0088403900000000001 * fVec1[((IOTA - 36) & 127)]) + ((0.0054466000000000002 * fVec1[((IOTA - 35) & 127)]) + ((0.033863999999999998 * fVec1[((IOTA - 29) & 127)]) + ((0.0092350000000000002 * fVec1[((IOTA - 28) & 127)]) + ((0.00053732699999999999 * fVec1[((IOTA - 26) & 127)]) + ((0.0043864799999999999 * fVec1[((IOTA - 24) & 127)]) + ((0.0027836500000000004 * fVec1[((IOTA - 23) & 127)]) + ((0.0092729000000000006 * fVec1[((IOTA - 22) & 127)]) + ((0.022299000000000003 * fVec1[((IOTA - 21) & 127)]) + ((0.0012059800000000002 * fVec1[((IOTA - 20) & 127)]) + ((0.025815500000000002 * fVec1[((IOTA - 17) & 127)]) + ((7.0800700000000008e-05 * fVec1[((IOTA - 9) & 127)]) + ((0.00076956600000000007 * fVec1[((IOTA - 8) & 127)]) + ((0.000231146 * fVec1[((IOTA - 5) & 127)]) + ((0.00014766099999999999 * fVec1[((IOTA - 4) & 127)]) + ((1.4833700000000001e-06 * fVec2[((IOTA - 126) & 127)]) + ((3.6973199999999999e-06 * fVec2[((IOTA - 125) & 127)]) + ((8.5058500000000009e-06 * fVec2[((IOTA - 124) & 127)]) + ((4.1470500000000002e-05 * fVec2[((IOTA - 123) & 127)]) + ((7.1901800000000003e-05 * fVec2[((IOTA - 122) & 127)]) + ((3.71676e-05 * fVec2[((IOTA - 121) & 127)]) + ((3.3982700000000002e-05 * fVec2[((IOTA - 119) & 127)]) + ((0.00016550199999999999 * fVec2[((IOTA - 118) & 127)]) + ((0.00015893600000000001 * fVec2[((IOTA - 117) & 127)]) + ((0.00011888700000000002 * fVec2[((IOTA - 116) & 127)]) + ((2.6751600000000003e-05 * fVec2[((IOTA - 115) & 127)]) + ((1.5261800000000004e-05 * fVec2[((IOTA - 114) & 127)]) + ((0.00030627700000000004 * fVec2[((IOTA - 113) & 127)]) + ((0.00036830800000000003 * fVec2[((IOTA - 112) & 127)]) + ((0.000251322 * fVec2[((IOTA - 111) & 127)]) + ((0.00022708 * fVec2[((IOTA - 110) & 127)]) + ((0.000239084 * fVec2[((IOTA - 109) & 127)]) + ((9.1238799999999995e-06 * fVec2[((IOTA - 108) & 127)]) + ((0.00015429699999999999 * fVec2[((IOTA - 90) & 127)]) + ((0.00021972700000000002 * fVec2[((IOTA - 89) & 127)]) + ((0.0011643300000000001 * fVec2[((IOTA - 82) & 127)]) + (((0.00065994200000000006 * fVec2[((IOTA - 77) & 127)]) + ((0.0017905 * fVec2[((IOTA - 76) & 127)]) + ((0.00015571700000000001 * fVec2[((IOTA - 71) & 127)]) + ((0.0044549899999999998 * fVec2[((IOTA - 70) & 127)]) + ((0.0039002500000000005 * fVec2[((IOTA - 69) & 127)]) + ((0.00054205300000000002 * fVec2[((IOTA - 68) & 127)]) + ((0.00164631 * fVec2[((IOTA - 67) & 127)]) + ((0.00128867 * fVec2[((IOTA - 66) & 127)]) + ((0.000377902 * fVec2[((IOTA - 65) & 127)]) + ((0.00227327 * fVec2[((IOTA - 64) & 127)]) + ((0.0030369999999999998 * fVec2[((IOTA - 63) & 127)]) + ((0.0023284899999999999 * fVec2[((IOTA - 62) & 127)]) + ((0.0020450900000000003 * fVec2[((IOTA - 61) & 127)]) + ((0.0038926099999999999 * fVec2[((IOTA - 60) & 127)]) + ((0.0051357799999999995 * fVec2[((IOTA - 59) & 127)]) + ((0.0057548899999999995 * fVec2[((IOTA - 58) & 127)]) + ((0.0110121 * fVec2[((IOTA - 57) & 127)]) + ((0.010980500000000001 * fVec2[((IOTA - 56) & 127)]) + ((0.0074067100000000004 * fVec2[((IOTA - 55) & 127)]) + ((0.0058000299999999994 * fVec2[((IOTA - 54) & 127)]) + ((0.0046881500000000003 * fVec2[((IOTA - 53) & 127)]) + ((0.0079024400000000002 * fVec2[((IOTA - 52) & 127)]) + ((0.0143637 * fVec2[((IOTA - 51) & 127)]) + ((0.018899099999999999 * fVec2[((IOTA - 50) & 127)]) + ((0.016699200000000001 * fVec2[((IOTA - 49) & 127)]) + ((0.0121522 * fVec2[((IOTA - 48) & 127)]) + ((0.011685700000000002 * fVec2[((IOTA - 47) & 127)]) + ((0.0122635 * fVec2[((IOTA - 46) & 127)]) + ((0.017027300000000002 * fVec2[((IOTA - 45) & 127)]) + ((0.019806900000000002 * fVec2[((IOTA - 44) & 127)]) + ((0.017303499999999999 * fVec2[((IOTA - 43) & 127)]) + ((0.017569399999999999 * fVec2[((IOTA - 42) & 127)]) + ((0.016072400000000001 * fVec2[((IOTA - 41) & 127)]) + ((0.0227996 * fVec2[((IOTA - 40) & 127)]) + ((0.016965000000000001 * fVec2[((IOTA - 39) & 127)]) + ((0.017693 * fVec2[((IOTA - 38) & 127)]) + ((0.033336499999999998 * fVec2[((IOTA - 37) & 127)]) + ((0.029073099999999998 * fVec2[((IOTA - 36) & 127)]) + ((0.018621700000000001 * fVec2[((IOTA - 35) & 127)]) + ((0.023295900000000001 * fVec2[((IOTA - 34) & 127)]) + ((0.030269900000000002 * fVec2[((IOTA - 33) & 127)]) + ((0.023498000000000002 * fVec2[((IOTA - 32) & 127)]) + ((0.012867100000000001 * fVec2[((IOTA - 31) & 127)]) + ((0.024658199999999998 * fVec2[((IOTA - 30) & 127)]) + ((0.033071500000000004 * fVec2[((IOTA - 29) & 127)]) + ((0.023601800000000003 * fVec2[((IOTA - 28) & 127)]) + ((0.025422899999999998 * fVec2[((IOTA - 27) & 127)]) + ((0.014210199999999999 * fVec2[((IOTA - 26) & 127)]) + ((0.010567 * fVec2[((IOTA - 25) & 127)]) + ((0.056895100000000004 * fVec2[((IOTA - 24) & 127)]) + (((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((0.00105561 * fVec3[((IOTA - 7) & 127)]) + ((((((1.96889e-05 * fTemp3) + (0.00012713900000000002 * fVec3[((IOTA - 1) & 127)])) + (0.00072445800000000009 * fVec3[((IOTA - 3) & 127)])) + (0.00084005400000000002 * fVec3[((IOTA - 4) & 127)])) + (0.00078251799999999995 * fVec3[((IOTA - 5) & 127)])) + (0.00064681700000000005 * fVec3[((IOTA - 6) & 127)]))) + (0.00107368 * fVec3[((IOTA - 8) & 127)])) + (0.00055752700000000004 * fVec3[((IOTA - 9) & 127)])) + (0.0020685400000000002 * fVec3[((IOTA - 10) & 127)])) + (0.0007391530000000001 * fVec3[((IOTA - 11) & 127)])) + (0.048941400000000003 * fVec3[((IOTA - 12) & 127)])) + (0.053001199999999998 * fVec3[((IOTA - 13) & 127)])) + (0.046698700000000003 * fVec3[((IOTA - 14) & 127)])) + (0.062211200000000001 * fVec3[((IOTA - 15) & 127)])) + (0.043948600000000004 * fVec3[((IOTA - 16) & 127)])) + (0.081345899999999999 * fVec3[((IOTA - 17) & 127)])) + (0.047424299999999996 * fVec3[((IOTA - 18) & 127)])) + (0.041405900000000002 * fVec3[((IOTA - 19) & 127)])) + (0.069732299999999997 * fVec3[((IOTA - 20) & 127)])) + (0.050380599999999998 * fVec3[((IOTA - 21) & 127)])) + (0.111697 * fVec3[((IOTA - 22) & 127)])) + (0.054464300000000007 * fVec3[((IOTA - 23) & 127)])) + (0.036801199999999999 * fVec3[((IOTA - 25) & 127)])) + (0.042423700000000002 * fVec3[((IOTA - 26) & 127)])) + (0.0075229400000000005 * fVec3[((IOTA - 27) & 127)])) + (0.0123141 * fVec3[((IOTA - 28) & 127)])) + (0.0216826 * fVec3[((IOTA - 29) & 127)])) + (0.017730600000000003 * fVec3[((IOTA - 30) & 127)])) + (0.0080194200000000011 * fVec3[((IOTA - 31) & 127)])) + (0.021074000000000002 * fVec3[((IOTA - 33) & 127)])) + (0.0272114 * fVec3[((IOTA - 34) & 127)])) + (0.0056564199999999997 * fVec3[((IOTA - 35) & 127)])) + (0.010539099999999999 * fVec3[((IOTA - 36) & 127)])) + (0.0081598500000000015 * fVec3[((IOTA - 37) & 127)])) + (0.0060665699999999994 * fVec3[((IOTA - 38) & 127)])) + (0.0072938400000000002 * fVec3[((IOTA - 39) & 127)])) + (0.0014525200000000001 * fVec3[((IOTA - 40) & 127)])) + (0.0049862900000000009 * fVec3[((IOTA - 41) & 127)])) + (0.014038399999999999 * fVec3[((IOTA - 42) & 127)])) + (0.0105579 * fVec3[((IOTA - 43) & 127)])) + (0.00431154 * fVec3[((IOTA - 45) & 127)])) + (0.0084629900000000001 * fVec3[((IOTA - 46) & 127)])) + (0.0007157 * fVec3[((IOTA - 47) & 127)])) + (0.00067166600000000002 * fVec3[((IOTA - 48) & 127)])) + (0.0046483699999999998 * fVec3[((IOTA - 49) & 127)])) + (0.0014356600000000001 * fVec3[((IOTA - 50) & 127)])) + (0.00020930100000000001 * fVec3[((IOTA - 51) & 127)])) + (0.00139635 * fVec3[((IOTA - 52) & 127)])) + (0.0029229899999999999 * fVec3[((IOTA - 53) & 127)])) + (0.0042595000000000003 * fVec3[((IOTA - 54) & 127)])) + (0.0039973999999999999 * fVec3[((IOTA - 55) & 127)])) + (4.49754e-06 * fVec3[((IOTA - 56) & 127)])) + (0.00278079 * fVec3[((IOTA - 58) & 127)])) + (0.0038020800000000002 * fVec3[((IOTA - 59) & 127)])) + (0.0026122100000000002 * fVec3[((IOTA - 60) & 127)])) + (0.00338193 * fVec3[((IOTA - 61) & 127)])) + (0.0064815100000000002 * fVec3[((IOTA - 62) & 127)])) + (0.0067297599999999996 * fVec3[((IOTA - 63) & 127)])) + (0.0038801100000000004 * fVec3[((IOTA - 64) & 127)])) + (0.0029381799999999999 * fVec3[((IOTA - 65) & 127)])) + (0.0016933400000000002 * fVec3[((IOTA - 66) & 127)])) + (0.0028127900000000003 * fVec3[((IOTA - 67) & 127)])) + (0.0033726400000000001 * fVec3[((IOTA - 68) & 127)])) + (0.00072404000000000001 * fVec3[((IOTA - 70) & 127)])) + (0.0019828699999999999 * fVec3[((IOTA - 71) & 127)])) + (0.0039087499999999999 * fVec3[((IOTA - 72) & 127)])) + (0.0053022700000000004 * fVec3[((IOTA - 73) & 127)])) + (0.0036032700000000004 * fVec3[((IOTA - 74) & 127)])) + (0.0053821700000000004 * fVec3[((IOTA - 75) & 127)])) + (0.0046326400000000004 * fVec3[((IOTA - 76) & 127)])) + (0.0020199900000000002 * fVec3[((IOTA - 77) & 127)])) + (0.0020409600000000001 * fVec3[((IOTA - 78) & 127)])) + (0.00144247 * fVec3[((IOTA - 79) & 127)])) + (0.0026692399999999998 * fVec3[((IOTA - 80) & 127)])) + (0.0015368900000000002 * fVec3[((IOTA - 81) & 127)])) + (2.5156400000000002e-05 * fVec3[((IOTA - 83) & 127)])) + (0.0010550900000000001 * fVec3[((IOTA - 84) & 127)])) + (0.0017505699999999999 * fVec3[((IOTA - 85) & 127)])) + (0.0026745600000000003 * fVec3[((IOTA - 86) & 127)])) + (0.00365475 * fVec3[((IOTA - 87) & 127)])) + (0.00193882 * fVec3[((IOTA - 88) & 127)])) + (0.0033512100000000003 * fVec3[((IOTA - 95) & 127)])) + (0.0025739000000000001 * fVec3[((IOTA - 96) & 127)])) + (0.00059498000000000005 * fVec3[((IOTA - 99) & 127)])) + (0.00094503799999999993 * fVec3[((IOTA - 100) & 127)])) + (0.0013272 * fVec3[((IOTA - 101) & 127)])) + (0.00147426 * fVec3[((IOTA - 102) & 127)])) + (0.0018419899999999999 * fVec3[((IOTA - 103) & 127)])) + (0.0012236 * fVec3[((IOTA - 104) & 127)])) + (0.00100501 * fVec3[((IOTA - 105) & 127)])) + (0.0016259900000000001 * fVec3[((IOTA - 106) & 127)])) + (0.0013097499999999999 * fVec3[((IOTA - 107) & 127)])) + (0.00095978899999999995 * fVec3[((IOTA - 108) & 127)])) + (0.000779071 * fVec3[((IOTA - 109) & 127)])) + (0.00070924500000000006 * fVec3[((IOTA - 110) & 127)])) + (0.0010336799999999999 * fVec3[((IOTA - 111) & 127)])) + (0.00050421999999999999 * fVec3[((IOTA - 112) & 127)])) + (0.00042659700000000002 * fVec3[((IOTA - 113) & 127)])) + (0.00050803600000000001 * fVec3[((IOTA - 114) & 127)])) + (0.00050261399999999997 * fVec3[((IOTA - 115) & 127)])) + (0.00038352400000000005 * fVec3[((IOTA - 116) & 127)])) + (0.00016114100000000001 * fVec3[((IOTA - 117) & 127)])) + (0.00020216400000000001 * fVec3[((IOTA - 118) & 127)])) + (0.000195057 * fVec3[((IOTA - 119) & 127)])) + (0.000117308 * fVec3[((IOTA - 120) & 127)])) + (5.5996800000000006e-05 * fVec3[((IOTA - 121) & 127)])) + (7.3980599999999995e-07 * fVec3[((IOTA - 123) & 127)])) + (1.69708e-05 * fVec3[((IOTA - 124) & 127)])) + (7.1374800000000001e-06 * fVec3[((IOTA - 125) & 127)])) + (1.74779e-06 * fVec3[((IOTA - 126) & 127)])) + (0.00027516900000000002 * fVec2[((IOTA - 2) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0040393699999999996 * fVec1[((IOTA - 33) & 127)]))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.00102589 * fVec0[((IOTA - 62) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) - ((7.4329200000000013e-05 * fVec0[((IOTA - 115) & 127)]) + ((0.00029377599999999999 * fVec0[((IOTA - 114) & 127)]) + ((0.000280439 * fVec0[((IOTA - 113) & 127)]) + ((0.00052814500000000005 * fVec0[((IOTA - 110) & 127)]) + ((0.00070229999999999999 * fVec0[((IOTA - 109) & 127)]) + ((0.00080632699999999991 * fVec0[((IOTA - 108) & 127)]) + ((0.000643591 * fVec0[((IOTA - 107) & 127)]) + ((0.00081294000000000006 * fVec0[((IOTA - 106) & 127)]) + ((0.00082950500000000002 * fVec0[((IOTA - 105) & 127)]) + ((0.00023107600000000004 * fVec0[((IOTA - 104) & 127)]) + ((0.00064061300000000005 * fVec0[((IOTA - 103) & 127)]) + ((0.00091664000000000003 * fVec0[((IOTA - 102) & 127)]) + ((0.00073955600000000005 * fVec0[((IOTA - 101) & 127)]) + ((0.00059890600000000007 * fVec0[((IOTA - 100) & 127)]) + ((0.0013073099999999999 * fVec0[((IOTA - 99) & 127)]) + ((0.0027768799999999998 * fVec0[((IOTA - 98) & 127)]) + ((0.00040949500000000001 * fVec0[((IOTA - 97) & 127)]) + ((0.00145302 * fVec0[((IOTA - 94) & 127)]) + ((0.0022861500000000002 * fVec0[((IOTA - 93) & 127)]) + ((0.001377 * fVec0[((IOTA - 90) & 127)]) + ((0.0017080400000000001 * fVec0[((IOTA - 89) & 127)]) + ((0.00016274 * fVec0[((IOTA - 83) & 127)]) + ((0.0027307400000000002 * fVec0[((IOTA - 82) & 127)]) + ((0.0028667500000000004 * fVec0[((IOTA - 81) & 127)]) + ((0.0011399600000000002 * fVec0[((IOTA - 80) & 127)]) + ((0.00110995 * fVec0[((IOTA - 79) & 127)]) + ((0.00124602 * fVec0[((IOTA - 78) & 127)]) + ((0.00129805 * fVec0[((IOTA - 77) & 127)]) + ((0.00100318 * fVec0[((IOTA - 76) & 127)]) + ((0.0022987899999999998 * fVec0[((IOTA - 75) & 127)]) + ((0.0019049100000000001 * fVec0[((IOTA - 74) & 127)]) + ((0.00059075000000000002 * fVec0[((IOTA - 73) & 127)]) + ((0.00103864 * fVec0[((IOTA - 72) & 127)]) + ((0.0013787300000000001 * fVec0[((IOTA - 71) & 127)]) + ((0.00181733 * fVec0[((IOTA - 70) & 127)]) + ((0.00293582 * fVec0[((IOTA - 69) & 127)]) + ((0.0023355800000000003 * fVec0[((IOTA - 68) & 127)]) + ((0.0011070699999999999 * fVec0[((IOTA - 67) & 127)]) + ((0.00050953999999999995 * fVec0[((IOTA - 66) & 127)]) + ((0.00044511000000000003 * fVec0[((IOTA - 65) & 127)]) + ((0.0038312400000000001 * fVec0[((IOTA - 61) & 127)]) + ((0.0016638 * fVec0[((IOTA - 60) & 127)]) + ((0.0043739400000000006 * fVec0[((IOTA - 48) & 127)]) + ((0.0013932899999999999 * fVec0[((IOTA - 45) & 127)]) + ((0.0106382 * fVec0[((IOTA - 41) & 127)]) + ((0.0043469900000000002 * fVec0[((IOTA - 40) & 127)]) + ((0.0015759000000000001 * fVec0[((IOTA - 37) & 127)]) + ((0.0073185999999999998 * fVec0[((IOTA - 36) & 127)]) + ((0.0045973800000000007 * fVec0[((IOTA - 35) & 127)]) + ((0.013183400000000001 * fVec0[((IOTA - 33) & 127)]) + ((0.028645500000000001 * fVec0[((IOTA - 32) & 127)]) + ((0.0092487699999999999 * fVec0[((IOTA - 29) & 127)]) + ((0.039121700000000002 * fVec0[((IOTA - 28) & 127)]) + ((0.00144425 * fVec0[((IOTA - 27) & 127)]) + ((0.0060285 * fVec0[((IOTA - 25) & 127)]) + ((0.057524800000000001 * fVec0[((IOTA - 24) & 127)]) + ((0.021570599999999999 * fVec0[((IOTA - 21) & 127)]) + ((0.0014793400000000002 * fVec0[((IOTA - 20) & 127)]) + ((0.0043988899999999999 * fVec0[((IOTA - 19) & 127)]) + ((0.0073680199999999994 * fVec0[((IOTA - 16) & 127)]) + ((0.00058021900000000009 * fVec0[((IOTA - 14) & 127)]) + ((0.0037306700000000002 * fVec0[((IOTA - 12) & 127)]) + ((0.000216524 * fVec0[((IOTA - 11) & 127)]) + ((0.00050577300000000005 * fVec0[((IOTA - 7) & 127)]) + ((0.00019702400000000002 * fVec0[((IOTA - 5) & 127)]) + (((7.8904099999999995e-07 * fVec1[((IOTA - 118) & 127)]) + ((5.1851200000000009e-05 * fVec1[((IOTA - 117) & 127)]) + ((3.48346e-05 * fVec1[((IOTA - 116) & 127)]) + ((0.000263489 * fVec1[((IOTA - 113) & 127)]) + ((0.00048678400000000003 * fVec1[((IOTA - 112) & 127)]) + ((0.00066577900000000002 * fVec1[((IOTA - 111) & 127)]) + ((0.00072508600000000011 * fVec1[((IOTA - 110) & 127)]) + ((0.00094771800000000006 * fVec1[((IOTA - 109) & 127)]) + ((0.00085589900000000003 * fVec1[((IOTA - 108) & 127)]) + ((0.00097958799999999999 * fVec1[((IOTA - 107) & 127)]) + ((0.00110553 * fVec1[((IOTA - 106) & 127)]) + ((0.00082826600000000003 * fVec1[((IOTA - 105) & 127)]) + ((0.0013655500000000001 * fVec1[((IOTA - 104) & 127)]) + ((0.0018638400000000001 * fVec1[((IOTA - 103) & 127)]) + ((0.0016748900000000001 * fVec1[((IOTA - 102) & 127)]) + ((0.0013615999999999999 * fVec1[((IOTA - 101) & 127)]) + ((0.00120272 * fVec1[((IOTA - 100) & 127)]) + ((0.00103053 * fVec1[((IOTA - 99) & 127)]) + ((0.00040302800000000005 * fVec1[((IOTA - 98) & 127)]) + ((0.0012814900000000001 * fVec1[((IOTA - 97) & 127)]) + ((0.0015273399999999999 * fVec1[((IOTA - 96) & 127)]) + ((0.00046550599999999997 * fVec1[((IOTA - 95) & 127)]) + ((0.00038473700000000001 * fVec1[((IOTA - 94) & 127)]) + ((0.0010250300000000001 * fVec1[((IOTA - 93) & 127)]) + ((0.0011346700000000002 * fVec1[((IOTA - 92) & 127)]) + ((0.00043984100000000006 * fVec1[((IOTA - 85) & 127)]) + ((0.00021673800000000001 * fVec1[((IOTA - 84) & 127)]) + ((2.74177e-05 * fVec1[((IOTA - 82) & 127)]) + ((0.0010960700000000002 * fVec1[((IOTA - 77) & 127)]) + ((0.0013700399999999999 * fVec1[((IOTA - 73) & 127)]) + ((0.0030635300000000001 * fVec1[((IOTA - 72) & 127)]) + ((0.0019637600000000002 * fVec1[((IOTA - 71) & 127)]) + ((0.0018260300000000002 * fVec1[((IOTA - 70) & 127)]) + ((0.00238017 * fVec1[((IOTA - 69) & 127)]) + ((0.0031654200000000004 * fVec1[((IOTA - 68) & 127)]) + ((0.00275033 * fVec1[((IOTA - 67) & 127)]) + ((0.0016457800000000001 * fVec1[((IOTA - 66) & 127)]) + ((0.0025982900000000001 * fVec1[((IOTA - 65) & 127)]) + ((0.0013408299999999999 * fVec1[((IOTA - 64) & 127)]) + ((0.00068048499999999999 * fVec1[((IOTA - 60) & 127)]) + ((0.00122218 * fVec1[((IOTA - 59) & 127)]) + ((0.00152893 * fVec1[((IOTA - 58) & 127)]) + ((0.0022063899999999999 * fVec1[((IOTA - 57) & 127)]) + ((0.00078379700000000003 * fVec1[((IOTA - 56) & 127)]) + ((0.00022987799999999998 * fVec1[((IOTA - 52) & 127)]) + ((0.00078511300000000004 * fVec1[((IOTA - 51) & 127)]) + ((0.0012260999999999999 * fVec1[((IOTA - 45) & 127)]) + ((0.0026344099999999998 * fVec1[((IOTA - 44) & 127)]) + ((0.0038961999999999998 * fVec1[((IOTA - 39) & 127)]) + ((0.011467400000000001 * fVec1[((IOTA - 38) & 127)]) + ((0.0078678299999999993 * fVec1[((IOTA - 37) & 127)]) + ((0.0013363100000000001 * fVec1[((IOTA - 34) & 127)]) + ((0.0089890100000000004 * fVec1[((IOTA - 32) & 127)]) + ((0.0143689 * fVec1[((IOTA - 31) & 127)]) + ((0.0049334100000000001 * fVec1[((IOTA - 30) & 127)]) + ((0.019686699999999998 * fVec1[((IOTA - 27) & 127)]) + ((0.0086390499999999988 * fVec1[((IOTA - 25) & 127)]) + ((0.043355600000000001 * fVec1[((IOTA - 19) & 127)]) + ((0.00249322 * fVec1[((IOTA - 18) & 127)]) + ((0.0141169 * fVec1[((IOTA - 16) & 127)]) + ((0.0111431 * fVec1[((IOTA - 15) & 127)]) + ((0.0018486100000000001 * fVec1[((IOTA - 14) & 127)]) + ((0.000976993 * fVec1[((IOTA - 13) & 127)]) + ((0.0056459800000000001 * fVec1[((IOTA - 12) & 127)]) + ((0.00021246900000000004 * fVec1[((IOTA - 11) & 127)]) + ((0.00095016999999999996 * fVec1[((IOTA - 10) & 127)]) + ((0.0010622000000000001 * fVec1[((IOTA - 7) & 127)]) + ((0.00024919200000000002 * fVec1[((IOTA - 6) & 127)]) + ((0.00027049600000000001 * fVec1[((IOTA - 3) & 127)]) + ((5.1123399999999996e-06 * fVec1[((IOTA - 2) & 127)]) + ((3.7580000000000003e-05 * fVec1[((IOTA - 1) & 127)]) + (((2.5019199999999998e-06 * fVec2[((IOTA - 120) & 127)]) + ((0.00015476800000000001 * fVec2[((IOTA - 107) & 127)]) + ((2.3801800000000003e-05 * fVec2[((IOTA - 106) & 127)]) + ((0.00041578400000000004 * fVec2[((IOTA - 105) & 127)]) + ((0.00040693400000000007 * fVec2[((IOTA - 104) & 127)]) + ((0.00042180200000000004 * fVec2[((IOTA - 103) & 127)]) + ((0.00064933700000000005 * fVec2[((IOTA - 102) & 127)]) + ((0.0010572399999999999 * fVec2[((IOTA - 101) & 127)]) + ((0.00096599000000000012 * fVec2[((IOTA - 100) & 127)]) + ((0.00077855999999999997 * fVec2[((IOTA - 99) & 127)]) + ((0.00073777800000000002 * fVec2[((IOTA - 98) & 127)]) + ((0.00065036 * fVec2[((IOTA - 97) & 127)]) + ((0.00069698000000000004 * fVec2[((IOTA - 96) & 127)]) + ((0.0013956400000000001 * fVec2[((IOTA - 95) & 127)]) + ((0.0020596199999999999 * fVec2[((IOTA - 94) & 127)]) + ((0.0017169000000000002 * fVec2[((IOTA - 93) & 127)]) + ((0.0012716300000000002 * fVec2[((IOTA - 92) & 127)]) + ((0.00130908 * fVec2[((IOTA - 91) & 127)]) + ((0.00161012 * fVec2[((IOTA - 88) & 127)]) + ((0.0016711600000000001 * fVec2[((IOTA - 87) & 127)]) + ((0.00075290600000000002 * fVec2[((IOTA - 86) & 127)]) + ((0.00144013 * fVec2[((IOTA - 85) & 127)]) + ((0.0021273399999999997 * fVec2[((IOTA - 84) & 127)]) + ((0.00061597300000000002 * fVec2[((IOTA - 83) & 127)]) + ((0.00011313 * fVec2[((IOTA - 81) & 127)]) + ((0.0017438199999999999 * fVec2[((IOTA - 80) & 127)]) + ((0.0024799100000000001 * fVec2[((IOTA - 79) & 127)]) + ((0.00256507 * fVec2[((IOTA - 78) & 127)]) + ((0.0011552999999999999 * fVec2[((IOTA - 75) & 127)]) + ((0.00098456699999999999 * fVec2[((IOTA - 74) & 127)]) + ((0.000597807 * fVec2[((IOTA - 73) & 127)]) + ((0.0018893200000000001 * fVec2[((IOTA - 72) & 127)]) + ((0.00092920600000000002 * fVec2[((IOTA - 23) & 127)]) + ((0.080136100000000002 * fVec2[((IOTA - 22) & 127)]) + ((0.041393800000000001 * fVec2[((IOTA - 21) & 127)]) + ((0.042661300000000006 * fVec2[((IOTA - 20) & 127)]) + ((0.0088175100000000006 * fVec2[((IOTA - 19) & 127)]) + ((0.053196100000000003 * fVec2[((IOTA - 18) & 127)]) + ((0.097787600000000002 * fVec2[((IOTA - 17) & 127)]) + ((0.077296500000000004 * fVec2[((IOTA - 15) & 127)]) + ((0.064767000000000005 * fVec2[((IOTA - 14) & 127)]) + ((0.082935599999999998 * fVec2[((IOTA - 13) & 127)]) + ((0.078489900000000001 * fVec2[((IOTA - 12) & 127)]) + ((0.00067357800000000009 * fVec2[((IOTA - 11) & 127)]) + ((0.00149621 * fVec2[((IOTA - 10) & 127)]) + ((0.00072128000000000003 * fVec2[((IOTA - 9) & 127)]) + ((0.00101019 * fVec2[((IOTA - 8) & 127)]) + ((0.000453602 * fVec2[((IOTA - 7) & 127)]) + ((0.00042969900000000002 * fVec2[((IOTA - 6) & 127)]) + ((0.0010723199999999999 * fVec2[((IOTA - 5) & 127)]) + ((0.00084950300000000004 * fVec2[((IOTA - 4) & 127)]) + ((0.00085153200000000001 * fVec2[((IOTA - 3) & 127)]) + ((0.00015027000000000001 * fVec2[((IOTA - 1) & 127)]) + ((2.4709800000000002e-05 * fTemp2) + ((1.47262e-06 * fVec3[((IOTA - 122) & 127)]) + ((0.00048385000000000002 * fVec3[((IOTA - 98) & 127)]) + ((0.00047365100000000001 * fVec3[((IOTA - 97) & 127)]) + ((9.940880000000001e-05 * fVec3[((IOTA - 94) & 127)]) + ((0.0023706000000000001 * fVec3[((IOTA - 93) & 127)]) + ((0.00196204 * fVec3[((IOTA - 92) & 127)]) + ((1.7742000000000001e-05 * fVec3[((IOTA - 91) & 127)]) + ((0.000611186 * fVec3[((IOTA - 90) & 127)]) + ((0.00087519700000000008 * fVec3[((IOTA - 89) & 127)]) + ((0.000687957 * fVec3[((IOTA - 82) & 127)]) + ((0.00036222200000000004 * fVec3[((IOTA - 69) & 127)]) + ((0.00063317100000000008 * fVec3[((IOTA - 57) & 127)]) + ((0.00092078700000000006 * fVec3[((IOTA - 44) & 127)]) + ((0.0029395300000000001 * fVec3[((IOTA - 32) & 127)]) + (((0.00016075400000000001 * fVec3[((IOTA - 2) & 127)]) + (0.021295600000000001 * fVec3[((IOTA - 24) & 127)])) + (0.040092800000000005 * fVec2[((IOTA - 16) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (1.0426200000000002e-05 * fTemp1))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (6.4290300000000003e-05 * fVec0[((IOTA - 3) & 127)])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
			fRec7[0] = max((fRec7[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fHbargraph1 = FAUSTFLOAT(fRec7[0]);
			output1[i] = FAUSTFLOAT(fTemp5);
			fRec1[1] = fRec1[0];
			fRec3[1] = fRec3[0];
			fRec2[1] = fRec2[0];
			IOTA = (IOTA + 1);
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec6[1] = fRec6[0];
			fRec0[1] = fRec0[0];
			fRec7[1] = fRec7[0];
			
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
