/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAEncEigenMike2"
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
	double fRec0[2];
	FAUSTFLOAT fVbargraph26;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	double fConst6;
	double fConst7;
	double fConst8;
	double fConst9;
	double fRec28[2];
	FAUSTFLOAT fVbargraph27;
	double fRec29[2];
	FAUSTFLOAT fVbargraph28;
	double fRec30[2];
	FAUSTFLOAT fVbargraph29;
	double fConst10;
	double fConst11;
	double fConst12;
	double fConst13;
	double fRec31[2];
	FAUSTFLOAT fVbargraph30;
	double fConst14;
	double fConst15;
	double fConst16;
	double fConst17;
	double fConst18;
	double fConst19;
	double fConst20;
	double fConst21;
	double fConst22;
	double fConst23;
	double fConst24;
	double fConst25;
	double fConst26;
	double fConst27;
	double fConst28;
	double fConst29;
	double fConst30;
	double fRec32[2];
	FAUSTFLOAT fVbargraph31;
	double fRec33[2];
	FAUSTFLOAT fVbargraph32;
	double fConst31;
	double fConst32;
	double fRec34[2];
	FAUSTFLOAT fVbargraph33;
	double fConst33;
	double fConst34;
	double fConst35;
	double fConst36;
	double fConst37;
	double fConst38;
	double fRec35[2];
	FAUSTFLOAT fVbargraph34;
	
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
		m->declare("name", "HOAEncEigenMike2");
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
		return 26;
		
	}
	virtual int getNumOutputs() {
		return 9;
		
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
		fConst1 = double(tgamma(1.5));
		fConst2 = double(tgamma(1.0));
		fConst3 = (fConst1 * sqrt((3.0 * (fConst2 / double(tgamma(3.0))))));
		fConst4 = (0.047619047619047623 * (0.0 - (1.5957691216057308 * fConst3)));
		fConst5 = (0.0 - (1.1283791670955128 * fConst3));
		fConst6 = (0.0 - (1.1283791670955126 * fConst3));
		fConst7 = (0.0 - (1.128379167095513 * fConst3));
		fConst8 = (0.0 - (0.92131773192356115 * fConst3));
		fConst9 = (0.0 - (0.92131773192356148 * fConst3));
		fConst10 = (double(tgamma(2.5)) * sqrt((5.0 * (fConst2 / double(tgamma(5.0))))));
		fConst11 = (0.03214285714285714 * (0.0 - (2.1276921621409737 * fConst10)));
		fConst12 = (0.0 - (3.1915382432114616 * fConst10));
		fConst13 = (0.038095238095238099 * fConst12);
		fConst14 = sqrt((5.0 * (double(tgamma(2.0)) / double(tgamma(4.0)))));
		fConst15 = (fConst1 * fConst14);
		fConst16 = (0.0 - (2.3936536824085959 * fConst15));
		fConst17 = (0.0 - (1.6925687506432687 * fConst1));
		fConst18 = (0.0 - (1.4142135623730951 * (fConst14 * fConst17)));
		fConst19 = (0.091186806948898899 * fConst1);
		fConst20 = (0.053874802376117914 * fConst17);
		fConst21 = (0.051292578908755625 * fConst1);
		fConst22 = (0.051292578908755639 * fConst1);
		fConst23 = (0.0 - (1.5957691216057308 * fConst1));
		fConst24 = (0.03214285714285714 * fConst23);
		fConst25 = (0.032142857142857147 * fConst23);
		fConst26 = (0.0 - (1.5957691216057308 * fConst15));
		fConst27 = (0.0 - (1.5957691216057313 * fConst15));
		fConst28 = (fConst14 * fConst23);
		fConst29 = (0.0 - fConst28);
		fConst30 = (0.0 - (1.0000000000000002 * fConst28));
		fConst31 = (0.051292578908755618 * fConst1);
		fConst32 = (0.032142857142857133 * fConst23);
		fConst33 = (0.0 - (4.7244256554467816e-16 * fConst10));
		fConst34 = (0.0 - (9.4488513108935631e-16 * fConst10));
		fConst35 = (0.047619047619047623 * fConst12);
		fConst36 = (0.0 - (1.595769121605731 * fConst10));
		fConst37 = (0.0 - (7.0866384831701748e-16 * fConst10));
		fConst38 = (0.0 - (1.417327696634035e-15 * fConst10));
		
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
			fRec0[l27] = 0.0;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec28[l28] = 0.0;
			
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
			fRec32[l32] = 0.0;
			
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec33[l33] = 0.0;
			
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec34[l34] = 0.0;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fRec35[l35] = 0.0;
			
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
		ui_interface->openVerticalBox("HOAEncEigenMike2");
		ui_interface->declare(0, "0", "");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d583c0", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d5cf10", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d53870", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d4ed20", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d4a1d0", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d455a0", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d94df0", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d99e80", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d8fd60", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d8acd0", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d85c40", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d80bb0", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d7bb20", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d76a90", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d71a00", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d6c970", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d678e0", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2d62850", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2dc2ea0", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2dc89b0", &fVbargraph25, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2dbd380", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2db7870", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2db1d60", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2dac250", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2da6740", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2da0c30", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("Outputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2dcef80", &fVbargraph26, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2ddef30", &fVbargraph27, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2de5d50", &fVbargraph28, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2def870", &fVbargraph29, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2dfa9c0", &fVbargraph30, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2e0b710", &fVbargraph31, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2e136c0", &fVbargraph32, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2e1c6e0", &fVbargraph33, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x2e2ae50", &fVbargraph34, -70.0, 6.0);
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
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		FAUSTFLOAT* output2 = outputs[2];
		FAUSTFLOAT* output3 = outputs[3];
		FAUSTFLOAT* output4 = outputs[4];
		FAUSTFLOAT* output5 = outputs[5];
		FAUSTFLOAT* output6 = outputs[6];
		FAUSTFLOAT* output7 = outputs[7];
		FAUSTFLOAT* output8 = outputs[8];
		double fSlow0 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fHslider0))));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			double fTemp0 = double(input5[i]);
			fRec2[0] = max((fRec2[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fVbargraph0 = FAUSTFLOAT(fRec2[0]);
			double fTemp1 = double(input4[i]);
			fRec3[0] = max((fRec3[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp1))))));
			fVbargraph1 = FAUSTFLOAT(fRec3[0]);
			double fTemp2 = double(input3[i]);
			fRec4[0] = max((fRec4[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp2))))));
			fVbargraph2 = FAUSTFLOAT(fRec4[0]);
			double fTemp3 = double(input2[i]);
			fRec5[0] = max((fRec5[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp3))))));
			fVbargraph3 = FAUSTFLOAT(fRec5[0]);
			double fTemp4 = double(input0[i]);
			fRec6[0] = max((fRec6[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fVbargraph4 = FAUSTFLOAT(fRec6[0]);
			double fTemp5 = double(input1[i]);
			fRec7[0] = max((fRec7[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fVbargraph5 = FAUSTFLOAT(fRec7[0]);
			double fTemp6 = double(input17[i]);
			fRec8[0] = max((fRec8[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fVbargraph6 = FAUSTFLOAT(fRec8[0]);
			double fTemp7 = double(input16[i]);
			fRec9[0] = max((fRec9[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp7))))));
			fVbargraph7 = FAUSTFLOAT(fRec9[0]);
			double fTemp8 = double(input15[i]);
			fRec10[0] = max((fRec10[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fVbargraph8 = FAUSTFLOAT(fRec10[0]);
			double fTemp9 = double(input14[i]);
			fRec11[0] = max((fRec11[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fVbargraph9 = FAUSTFLOAT(fRec11[0]);
			double fTemp10 = double(input13[i]);
			fRec12[0] = max((fRec12[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fVbargraph10 = FAUSTFLOAT(fRec12[0]);
			double fTemp11 = double(input12[i]);
			fRec13[0] = max((fRec13[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fVbargraph11 = FAUSTFLOAT(fRec13[0]);
			double fTemp12 = double(input11[i]);
			fRec14[0] = max((fRec14[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fVbargraph12 = FAUSTFLOAT(fRec14[0]);
			double fTemp13 = double(input10[i]);
			fRec15[0] = max((fRec15[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fVbargraph13 = FAUSTFLOAT(fRec15[0]);
			double fTemp14 = double(input9[i]);
			fRec16[0] = max((fRec16[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fVbargraph14 = FAUSTFLOAT(fRec16[0]);
			double fTemp15 = double(input8[i]);
			fRec17[0] = max((fRec17[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fVbargraph15 = FAUSTFLOAT(fRec17[0]);
			double fTemp16 = double(input6[i]);
			fRec18[0] = max((fRec18[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fVbargraph16 = FAUSTFLOAT(fRec18[0]);
			double fTemp17 = double(input7[i]);
			fRec19[0] = max((fRec19[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fVbargraph17 = FAUSTFLOAT(fRec19[0]);
			double fTemp18 = (fTemp14 + (fTemp15 + (fTemp16 + fTemp17)));
			double fTemp19 = double(input25[i]);
			fRec20[0] = max((fRec20[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fVbargraph18 = FAUSTFLOAT(fRec20[0]);
			double fTemp20 = double(input24[i]);
			fRec21[0] = max((fRec21[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fVbargraph19 = FAUSTFLOAT(fRec21[0]);
			double fTemp21 = double(input23[i]);
			fRec22[0] = max((fRec22[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fVbargraph20 = FAUSTFLOAT(fRec22[0]);
			double fTemp22 = double(input22[i]);
			fRec23[0] = max((fRec23[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fVbargraph21 = FAUSTFLOAT(fRec23[0]);
			double fTemp23 = double(input21[i]);
			fRec24[0] = max((fRec24[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fVbargraph22 = FAUSTFLOAT(fRec24[0]);
			double fTemp24 = double(input20[i]);
			fRec25[0] = max((fRec25[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fVbargraph23 = FAUSTFLOAT(fRec25[0]);
			double fTemp25 = double(input18[i]);
			fRec26[0] = max((fRec26[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp25))))));
			fVbargraph24 = FAUSTFLOAT(fRec26[0]);
			double fTemp26 = double(input19[i]);
			fRec27[0] = max((fRec27[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fVbargraph25 = FAUSTFLOAT(fRec27[0]);
			double fTemp27 = (fTemp23 + (fTemp24 + (fTemp25 + fTemp26)));
			double fTemp28 = (fTemp19 + (fTemp20 + (fTemp21 + (fTemp22 + fTemp27))));
			double fTemp29 = (fRec1[0] * (((0.047619047619047623 * (fTemp0 + (fTemp1 + (fTemp2 + (fTemp3 + (fTemp4 + fTemp5)))))) + (0.038095238095238099 * (fTemp6 + (fTemp7 + (fTemp8 + (fTemp9 + (fTemp10 + (fTemp11 + (fTemp12 + (fTemp13 + fTemp18)))))))))) + (0.03214285714285714 * fTemp28)));
			fRec0[0] = max((fRec0[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fVbargraph26 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp29);
			double fTemp30 = (fRec1[0] * ((((fConst4 * fTemp1) + (0.038095238095238099 * ((((fConst5 * fTemp14) + (fConst6 * fTemp11)) + (fConst7 * fTemp10)) + (fConst5 * fTemp6)))) + (fConst3 * (((((((((0.07598900579074909 * fTemp3) + (0.042985873032210013 * fTemp17)) + (0.042985873032210006 * fTemp13)) + (0.042985873032210013 * fTemp12)) + (0.042985873032210013 * fTemp8)) + (0.029613784240400176 * fTemp25)) + (0.02961378424040018 * fTemp26)) + (0.029613784240400176 * fTemp22)) + (0.02961378424040018 * fTemp21)))) + (0.03214285714285714 * ((((fConst8 * fTemp24) + (fConst9 * fTemp23)) + (fConst8 * fTemp20)) + (fConst9 * fTemp19)))));
			fRec28[0] = max((fRec28[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fVbargraph27 = FAUSTFLOAT(fRec28[0]);
			output1[i] = FAUSTFLOAT(fTemp30);
			double fTemp31 = (fRec1[0] * ((0.082478609884232251 * (fTemp4 - fTemp0)) - ((0.046656947481584346 * (fTemp6 + (fTemp7 + (fTemp8 + (fTemp9 - fTemp18))))) + (0.03214285714285714 * (fTemp19 + (fTemp20 + (fTemp21 + (fTemp22 - fTemp27))))))));
			fRec29[0] = max((fRec29[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fVbargraph28 = FAUSTFLOAT(fRec29[0]);
			output2[i] = FAUSTFLOAT(fTemp31);
			double fTemp32 = (fRec1[0] * ((fConst3 * (((((((((0.07598900579074909 * fTemp5) + (0.042985873032210013 * fTemp16)) + (0.042985873032210013 * fTemp13)) + (0.042985873032209999 * fTemp10)) + (0.042985873032210013 * fTemp9)) + (0.02961378424040018 * fTemp25)) + (0.029613784240400173 * fTemp23)) + (0.02961378424040018 * fTemp22)) + (0.029613784240400173 * fTemp19))) + (((fConst4 * fTemp2) + (0.038095238095238099 * ((((fConst5 * fTemp15) + (fConst6 * fTemp12)) + (fConst7 * fTemp11)) + (fConst5 * fTemp7)))) + (0.03214285714285714 * ((((fConst8 * fTemp26) + (fConst9 * fTemp24)) + (fConst8 * fTemp21)) + (fConst9 * fTemp20))))));
			fRec30[0] = max((fRec30[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fVbargraph29 = FAUSTFLOAT(fRec30[0]);
			output3[i] = FAUSTFLOAT(fTemp32);
			double fTemp33 = (fRec1[0] * ((fConst11 * (fTemp19 + (fTemp21 + (fTemp26 + fTemp23)))) + ((fConst13 * (fTemp12 + fTemp10)) + (fConst10 * ((((((0.12158240926519855 * fTemp13) + (0.12158240926519859 * fTemp11)) + (0.068390105211674143 * fTemp25)) + (0.068390105211674157 * fTemp24)) + (0.068390105211674143 * fTemp22)) + (0.068390105211674157 * fTemp20))))));
			fRec31[0] = max((fRec31[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fVbargraph30 = FAUSTFLOAT(fRec31[0]);
			output4[i] = FAUSTFLOAT(fTemp33);
			double fTemp34 = (fRec1[0] * (((0.038095238095238099 * ((fConst16 * fTemp14) + (fConst18 * fTemp6))) + (fConst14 * ((((((fConst19 * fTemp17) + (fConst20 * fTemp8)) + (fConst21 * fTemp25)) + (fConst22 * fTemp26)) + (fConst24 * fTemp22)) + (fConst25 * fTemp21)))) + (0.03214285714285714 * ((((fConst26 * fTemp24) + (fConst27 * fTemp23)) + (fConst29 * fTemp20)) + (fConst30 * fTemp19)))));
			fRec32[0] = max((fRec32[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fVbargraph31 = FAUSTFLOAT(fRec32[0]);
			output5[i] = FAUSTFLOAT(fTemp34);
			double fTemp35 = (fRec1[0] * (((0.021295885499997991 * (fTemp6 + (fTemp7 + (fTemp8 + (fTemp9 + fTemp18))))) + ((0.10647942749999 * (fTemp4 + fTemp0)) + (7.9795740640090912e-18 * fTemp28))) - ((0.053239713749994998 * (fTemp1 + (fTemp2 + (fTemp5 + fTemp3)))) + (0.042591770999995997 * (fTemp10 + (fTemp11 + (fTemp13 + fTemp12)))))));
			fRec33[0] = max((fRec33[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fVbargraph32 = FAUSTFLOAT(fRec33[0]);
			output6[i] = FAUSTFLOAT(fTemp35);
			double fTemp36 = (fRec1[0] * ((fConst14 * ((((((fConst19 * fTemp16) + (fConst20 * fTemp9)) + (fConst22 * fTemp25)) + (fConst31 * fTemp23)) + (fConst25 * fTemp22)) + (fConst32 * fTemp19))) + ((0.038095238095238099 * ((fConst16 * fTemp15) + (fConst18 * fTemp7))) + (0.03214285714285714 * ((((fConst26 * fTemp26) + (fConst27 * fTemp24)) + (fConst29 * fTemp21)) + (fConst30 * fTemp20))))));
			fRec34[0] = max((fRec34[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fVbargraph33 = FAUSTFLOAT(fRec34[0]);
			output7[i] = FAUSTFLOAT(fTemp36);
			double fTemp37 = (fRec1[0] * ((0.03214285714285714 * ((((fConst33 * fTemp26) + (fConst34 * fTemp23)) + (fConst33 * fTemp21)) + (fConst34 * fTemp19))) + (((fConst35 * (fTemp3 + fTemp1)) + (0.038095238095238099 * (((((fConst36 * (fTemp17 + fTemp14)) + (fConst37 * fTemp12)) + (fConst38 * fTemp10)) + (fConst36 * fTemp8)) + (fConst36 * fTemp6)))) + (fConst10 * (((((((((((0.06079120463259928 * fTemp16) + (0.15197801158149818 * (fTemp5 + fTemp2))) + (0.06079120463259928 * fTemp15)) + (2.6996718031124477e-17 * fTemp13)) + (5.3993436062248954e-17 * fTemp11)) + (0.06079120463259928 * fTemp9)) + (0.06079120463259928 * fTemp7)) + (1.518565389250751e-17 * fTemp25)) + (3.037130778501502e-17 * fTemp24)) + (1.518565389250751e-17 * fTemp22)) + (3.037130778501502e-17 * fTemp20))))));
			fRec35[0] = max((fRec35[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fVbargraph34 = FAUSTFLOAT(fRec35[0]);
			output8[i] = FAUSTFLOAT(fTemp37);
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
			fRec0[1] = fRec0[0];
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec30[1] = fRec30[0];
			fRec31[1] = fRec31[0];
			fRec32[1] = fRec32[0];
			fRec33[1] = fRec33[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			
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
