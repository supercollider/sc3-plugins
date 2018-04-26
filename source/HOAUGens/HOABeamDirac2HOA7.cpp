/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2016"
license: "GPL"
name: "HOABeamDirac2HOA7"
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
double mydsp_faustpower3_f(double value) {
	return ((value * value) * value);
	
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
	
	FAUSTFLOAT fCheckbox0;
	FAUSTFLOAT fCheckbox1;
	int fSamplingFreq;
	double fConst0;
	double fConst1;
	FAUSTFLOAT fEntry0;
	double fRec0[2];
	double fRec1[2];
	FAUSTFLOAT fVslider0;
	FAUSTFLOAT fVslider1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	FAUSTFLOAT fVslider2;
	FAUSTFLOAT fVslider3;
	double fRec2[2];
	double fRec3[2];
	double fRec4[2];
	double fConst6;
	double fConst7;
	double fConst8;
	double fConst9;
	double fConst10;
	double fConst11;
	double fConst12;
	double fConst13;
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
	double fConst71;
	double fConst72;
	double fConst73;
	double fConst74;
	double fConst75;
	double fConst76;
	double fConst77;
	double fConst78;
	double fConst79;
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
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
		m->declare("filename", "HOABeamDirac2HOA7");
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
		m->declare("name", "HOABeamDirac2HOA7");
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
		return 64;
		
	}
	virtual int getNumOutputs() {
		return 64;
		
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
			case 50: {
				rate = 1;
				break;
			}
			case 51: {
				rate = 1;
				break;
			}
			case 52: {
				rate = 1;
				break;
			}
			case 53: {
				rate = 1;
				break;
			}
			case 54: {
				rate = 1;
				break;
			}
			case 55: {
				rate = 1;
				break;
			}
			case 56: {
				rate = 1;
				break;
			}
			case 57: {
				rate = 1;
				break;
			}
			case 58: {
				rate = 1;
				break;
			}
			case 59: {
				rate = 1;
				break;
			}
			case 60: {
				rate = 1;
				break;
			}
			case 61: {
				rate = 1;
				break;
			}
			case 62: {
				rate = 1;
				break;
			}
			case 63: {
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
			case 50: {
				rate = 1;
				break;
			}
			case 51: {
				rate = 1;
				break;
			}
			case 52: {
				rate = 1;
				break;
			}
			case 53: {
				rate = 1;
				break;
			}
			case 54: {
				rate = 1;
				break;
			}
			case 55: {
				rate = 1;
				break;
			}
			case 56: {
				rate = 1;
				break;
			}
			case 57: {
				rate = 1;
				break;
			}
			case 58: {
				rate = 1;
				break;
			}
			case 59: {
				rate = 1;
				break;
			}
			case 60: {
				rate = 1;
				break;
			}
			case 61: {
				rate = 1;
				break;
			}
			case 62: {
				rate = 1;
				break;
			}
			case 63: {
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
		fConst1 = (1.0 / fConst0);
		fConst2 = double(tgamma(2.5));
		fConst3 = double(tgamma(1.0));
		fConst4 = double(tgamma(5.0));
		fConst5 = sqrt((5.0 * (fConst3 / fConst4)));
		fConst6 = double(tgamma(2.0));
		fConst7 = double(tgamma(6.0));
		fConst8 = sqrt((7.0 * (fConst6 / fConst7)));
		fConst9 = (31.915382432114615 * fConst8);
		fConst10 = (15.957691216057308 * fConst8);
		fConst11 = double(tgamma(9.0));
		fConst12 = double(tgamma(4.5));
		fConst13 = (sqrt((9.0 * (fConst3 / fConst11))) * fConst12);
		fConst14 = (25.532305945691693 * fConst13);
		fConst15 = double(tgamma(3.0));
		fConst16 = double(tgamma(7.0));
		fConst17 = (sqrt((9.0 * (fConst15 / fConst16))) * fConst2);
		fConst18 = (1.4142135623730951 * fConst17);
		fConst19 = (0.70710678118654757 * fConst17);
		fConst20 = (12.766152972845846 * fConst13);
		fConst21 = double(tgamma(4.0));
		fConst22 = double(tgamma(8.0));
		fConst23 = (sqrt((11.0 * (fConst21 / fConst22))) * fConst2);
		fConst24 = (0.94280904158206336 * fConst23);
		fConst25 = (0.47140452079103168 * fConst23);
		fConst26 = double(tgamma(13.0));
		fConst27 = double(tgamma(6.5));
		fConst28 = (sqrt((13.0 * (fConst3 / fConst26))) * fConst27);
		fConst29 = (51.064611891383386 * fConst28);
		fConst30 = double(tgamma(11.0));
		fConst31 = (sqrt((13.0 * (fConst15 / fConst30))) * fConst12);
		fConst32 = (1.4142135623730951 * fConst31);
		fConst33 = (sqrt((13.0 * (fConst4 / fConst11))) * fConst2);
		fConst34 = (0.70710678118654757 * fConst33);
		fConst35 = (0.35355339059327379 * fConst33);
		fConst36 = (0.70710678118654757 * fConst31);
		fConst37 = double(tgamma(12.0));
		fConst38 = (sqrt((15.0 * (fConst21 / fConst37))) * fConst12);
		fConst39 = (0.94280904158206336 * fConst38);
		fConst40 = double(tgamma(10.0));
		fConst41 = (sqrt((15.0 * (fConst7 / fConst40))) * fConst2);
		fConst42 = (0.28284271247461906 * fConst41);
		fConst43 = (sqrt((15.0 * (fConst6 / double(tgamma(14.0))))) * fConst27);
		fConst44 = (663.83995458798404 * fConst43);
		fConst45 = double(tgamma(5.5));
		fConst46 = sqrt((11.0 * (fConst3 / fConst30)));
		fConst47 = (25.532305945691693 * fConst46);
		fConst48 = sqrt((13.0 * (fConst6 / fConst37)));
		fConst49 = (280.85536540260864 * fConst48);
		fConst50 = sqrt((15.0 * (fConst15 / fConst26)));
		fConst51 = (0.70710678118654757 * fConst50);
		fConst52 = (sqrt((11.0 * (fConst6 / fConst40))) * fConst12);
		fConst53 = (229.79075351122526 * fConst52);
		fConst54 = (114.89537675561263 * fConst52);
		fConst55 = (0.47140452079103168 * fConst38);
		fConst56 = double(tgamma(3.5));
		fConst57 = sqrt((7.0 * (fConst3 / fConst16)));
		fConst58 = (6.3830764864229232 * fConst57);
		fConst59 = sqrt((9.0 * (fConst6 / fConst22)));
		fConst60 = (44.681535404960464 * fConst59);
		fConst61 = sqrt((11.0 * (fConst15 / fConst11)));
		fConst62 = (0.70710678118654757 * fConst61);
		fConst63 = sqrt((13.0 * (fConst21 / fConst40)));
		fConst64 = (0.47140452079103168 * fConst63);
		fConst65 = sqrt((15.0 * (fConst4 / fConst30)));
		fConst66 = (0.35355339059327379 * fConst65);
		fConst67 = (sqrt((15.0 * (fConst3 / double(tgamma(15.0))))) * double(tgamma(7.5)));
		fConst68 = (102.12922378276677 * fConst67);
		fConst69 = (0.56568542494923812 * fConst41);
		fConst70 = double(tgamma(1.5));
		fConst71 = sqrt((3.0 * (fConst3 / fConst15)));
		fConst72 = (1.5957691216057308 * fConst71);
		fConst73 = sqrt((5.0 * (fConst6 / fConst21)));
		fConst74 = (4.7873073648171927 * fConst73);
		fConst75 = sqrt((7.0 * (fConst15 / fConst4)));
		fConst76 = (0.70710678118654757 * fConst75);
		fConst77 = sqrt((9.0 * (fConst21 / fConst7)));
		fConst78 = (0.47140452079103168 * fConst77);
		fConst79 = sqrt((11.0 * (fConst4 / fConst16)));
		fConst80 = (0.35355339059327379 * fConst79);
		fConst81 = sqrt((13.0 * (fConst7 / fConst22)));
		fConst82 = (0.28284271247461906 * fConst81);
		fConst83 = sqrt((15.0 * (fConst16 / fConst11)));
		fConst84 = (0.23570226039551584 * fConst83);
		fConst85 = (0.12698727186848197 * (fConst71 * fConst70));
		fConst86 = (fConst5 * fConst2);
		fConst87 = (0.50794908747392786 * fConst86);
		fConst88 = (0.3809618156054459 * (fConst73 * fConst70));
		fConst89 = (0.25397454373696393 * fConst86);
		fConst90 = (0.50794908747392786 * (fConst57 * fConst56));
		fConst91 = (fConst8 * fConst2);
		fConst92 = (2.5397454373696391 * fConst91);
		fConst93 = (0.056269769759819135 * (fConst75 * fConst70));
		fConst94 = (1.2698727186848195 * fConst91);
		fConst95 = (2.0317963498957115 * fConst13);
		fConst96 = (3.5556436123174948 * (fConst59 * fConst56));
		fConst97 = (0.11253953951963827 * fConst17);
		fConst98 = (0.037513179839879424 * (fConst77 * fConst70));
		fConst99 = (0.056269769759819135 * fConst17);
		fConst100 = (1.0158981749478557 * fConst13);
		fConst101 = (2.0317963498957115 * (fConst46 * fConst45));
		fConst102 = (18.286167149061402 * fConst52);
		fConst103 = (0.056269769759819135 * (fConst61 * fConst56));
		fConst104 = (0.075026359679758847 * fConst23);
		fConst105 = (0.028134884879909568 * (fConst79 * fConst70));
		fConst106 = (0.037513179839879424 * fConst23);
		fConst107 = (9.1430835745307011 * fConst52);
		fConst108 = (4.0635926997914229 * fConst28);
		fConst109 = (22.349759848852827 * (fConst48 * fConst45));
		fConst110 = (0.11253953951963827 * fConst31);
		fConst111 = (0.037513179839879424 * (fConst63 * fConst56));
		fConst112 = (0.056269769759819135 * fConst33);
		fConst113 = (0.022507907903927659 * (fConst81 * fConst70));
		fConst114 = (0.028134884879909568 * fConst33);
		fConst115 = (0.056269769759819135 * fConst31);
		fConst116 = (8.1271853995828458 * fConst67);
		fConst117 = (52.826705097288496 * fConst43);
		fConst118 = (0.056269769759819135 * (fConst50 * fConst45));
		fConst119 = (0.075026359679758847 * fConst38);
		fConst120 = (0.028134884879909568 * (fConst65 * fConst56));
		fConst121 = (0.045015815807855318 * fConst41);
		fConst122 = (0.018756589919939712 * (fConst83 * fConst70));
		fConst123 = (0.022507907903927659 * fConst41);
		fConst124 = (0.037513179839879424 * fConst38);
		
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = FAUSTFLOAT(0.0);
		fCheckbox1 = FAUSTFLOAT(0.0);
		fEntry0 = FAUSTFLOAT(1.0);
		fVslider0 = FAUSTFLOAT(0.0);
		fVslider1 = FAUSTFLOAT(0.0);
		fVslider2 = FAUSTFLOAT(0.0);
		fVslider3 = FAUSTFLOAT(0.0);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec0[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec1[l1] = 0.0;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec2[l2] = 0.0;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec3[l3] = 0.0;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec4[l4] = 0.0;
			
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
		ui_interface->openHorizontalBox("Parameters");
		ui_interface->declare(0, "2", "");
		ui_interface->openVerticalBox("On/Off");
		ui_interface->addCheckButton("On", &fCheckbox1);
		ui_interface->closeBox();
		ui_interface->declare(&fEntry0, "3", "");
		ui_interface->declare(&fEntry0, "unit", "s");
		ui_interface->addNumEntry("Crossfade", &fEntry0, 1.0, 0.10000000000000001, 10.0, 0.10000000000000001);
		ui_interface->declare(&fVslider3, "4", "");
		ui_interface->declare(&fVslider3, "unit", "dB");
		ui_interface->addVerticalSlider("Gain", &fVslider3, 0.0, -20.0, 20.0, 0.10000000000000001);
		ui_interface->declare(&fVslider2, "5", "");
		ui_interface->addVerticalSlider("Azimuth", &fVslider2, 0.0, -3.1415926535897931, 3.1415926535897931, 0.10000000000000001);
		ui_interface->declare(&fVslider1, "6", "");
		ui_interface->addVerticalSlider("Elevation", &fVslider1, 0.0, -1.5707963267948966, 1.5707963267948966, 0.10000000000000001);
		ui_interface->declare(&fCheckbox0, "7", "");
		ui_interface->addCheckButton("Timer/Manual", &fCheckbox0);
		ui_interface->declare(&fVslider0, "8", "");
		ui_interface->addVerticalSlider("Focus", &fVslider0, 0.0, 0.0, 1.0, 0.0001);
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
		FAUSTFLOAT* input36 = inputs[36];
		FAUSTFLOAT* input37 = inputs[37];
		FAUSTFLOAT* input38 = inputs[38];
		FAUSTFLOAT* input39 = inputs[39];
		FAUSTFLOAT* input40 = inputs[40];
		FAUSTFLOAT* input41 = inputs[41];
		FAUSTFLOAT* input42 = inputs[42];
		FAUSTFLOAT* input43 = inputs[43];
		FAUSTFLOAT* input44 = inputs[44];
		FAUSTFLOAT* input45 = inputs[45];
		FAUSTFLOAT* input46 = inputs[46];
		FAUSTFLOAT* input47 = inputs[47];
		FAUSTFLOAT* input48 = inputs[48];
		FAUSTFLOAT* input49 = inputs[49];
		FAUSTFLOAT* input50 = inputs[50];
		FAUSTFLOAT* input51 = inputs[51];
		FAUSTFLOAT* input52 = inputs[52];
		FAUSTFLOAT* input53 = inputs[53];
		FAUSTFLOAT* input54 = inputs[54];
		FAUSTFLOAT* input55 = inputs[55];
		FAUSTFLOAT* input56 = inputs[56];
		FAUSTFLOAT* input57 = inputs[57];
		FAUSTFLOAT* input58 = inputs[58];
		FAUSTFLOAT* input59 = inputs[59];
		FAUSTFLOAT* input60 = inputs[60];
		FAUSTFLOAT* input61 = inputs[61];
		FAUSTFLOAT* input62 = inputs[62];
		FAUSTFLOAT* input63 = inputs[63];
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
		FAUSTFLOAT* output50 = outputs[50];
		FAUSTFLOAT* output51 = outputs[51];
		FAUSTFLOAT* output52 = outputs[52];
		FAUSTFLOAT* output53 = outputs[53];
		FAUSTFLOAT* output54 = outputs[54];
		FAUSTFLOAT* output55 = outputs[55];
		FAUSTFLOAT* output56 = outputs[56];
		FAUSTFLOAT* output57 = outputs[57];
		FAUSTFLOAT* output58 = outputs[58];
		FAUSTFLOAT* output59 = outputs[59];
		FAUSTFLOAT* output60 = outputs[60];
		FAUSTFLOAT* output61 = outputs[61];
		FAUSTFLOAT* output62 = outputs[62];
		FAUSTFLOAT* output63 = outputs[63];
		int iSlow0 = int(double(fCheckbox0));
		double fSlow1 = double(fCheckbox1);
		int iSlow2 = int(fSlow1);
		double fSlow3 = double(fEntry0);
		double fSlow4 = (fConst1 / fSlow3);
		int iSlow5 = (fSlow1 > 0.0);
		double fSlow6 = (fConst0 * fSlow3);
		int iSlow7 = ((1.0 - fSlow1) > 0.0);
		double fSlow8 = double(fVslider0);
		double fSlow9 = (1.0 - fSlow8);
		double fSlow10 = sin(double(fVslider1));
		double fSlow11 = mydsp_faustpower2_f(fSlow10);
		double fSlow12 = (1.0 - fSlow11);
		double fSlow13 = double(fVslider2);
		double fSlow14 = cos(fSlow13);
		double fSlow15 = sin(fSlow13);
		double fSlow16 = (6.3830764864229232 * (fSlow14 * fSlow15));
		double fSlow17 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider3))));
		double fSlow18 = mydsp_faustpower2_f(fSlow14);
		double fSlow19 = ((2.0 * fSlow18) + -1.0);
		double fSlow20 = (3.1915382432114616 * fSlow19);
		double fSlow21 = (fSlow14 * fSlow10);
		double fSlow22 = (fConst9 * (fSlow21 * fSlow15));
		double fSlow23 = (fSlow19 * fSlow10);
		double fSlow24 = (fConst10 * fSlow23);
		double fSlow25 = ((4.0 * fSlow18) + -1.0);
		double fSlow26 = (fSlow25 + -1.0);
		double fSlow27 = (fSlow26 * fSlow14);
		double fSlow28 = (fConst14 * ((fSlow27 * fSlow12) * fSlow15));
		double fSlow29 = ((78.986541696685876 * fSlow11) + -11.283791670955125);
		double fSlow30 = (fSlow29 * fSlow14);
		double fSlow31 = (fConst18 * (fSlow30 * fSlow15));
		double fSlow32 = (fSlow19 * fSlow29);
		double fSlow33 = (fConst19 * fSlow32);
		double fSlow34 = ((2.0 * fSlow19) + -1.0);
		double fSlow35 = (1.0 - fSlow34);
		double fSlow36 = (1.0 - (2.0 * (fSlow35 * fSlow18)));
		double fSlow37 = (fConst20 * (fSlow36 * fSlow12));
		double fSlow38 = ((4.5 * fSlow29) + -67.702750025730751);
		double fSlow39 = ((fSlow38 * fSlow14) * fSlow10);
		double fSlow40 = (fConst24 * (fSlow39 * fSlow15));
		double fSlow41 = ((fSlow19 * fSlow38) * fSlow10);
		double fSlow42 = (fConst25 * fSlow41);
		double fSlow43 = (1.0 - fSlow26);
		double fSlow44 = (1.0 - (4.0 * (fSlow43 * fSlow18)));
		double fSlow45 = (0.0 - (2.0 * ((fSlow26 - fSlow44) * fSlow14)));
		double fSlow46 = mydsp_faustpower2_f(fSlow12);
		double fSlow47 = (fConst29 * ((fSlow45 * fSlow46) * fSlow15));
		double fSlow48 = ((893.67630033964599 * fSlow11) + -81.24330003087691);
		double fSlow49 = ((fSlow26 * fSlow48) * fSlow14);
		double fSlow50 = (fConst32 * ((fSlow49 * fSlow12) * fSlow15));
		double fSlow51 = ((3.6666666666666665 * (fSlow38 * fSlow11)) - (3.5 * fSlow29));
		double fSlow52 = (fSlow51 * fSlow14);
		double fSlow53 = (fConst34 * (fSlow52 * fSlow15));
		double fSlow54 = (fSlow19 * fSlow51);
		double fSlow55 = (fConst35 * fSlow54);
		double fSlow56 = (fSlow36 * fSlow48);
		double fSlow57 = (fConst36 * (fSlow56 * fSlow12));
		double fSlow58 = ((2.0 * fSlow36) - fSlow34);
		double fSlow59 = ((2.0 * ((fSlow35 + fSlow58) * fSlow18)) + -1.0);
		double fSlow60 = (fConst29 * (fSlow59 * fSlow46));
		double fSlow61 = ((6.5 * fSlow48) + -812.43300030876912);
		double fSlow62 = (((fSlow26 * fSlow61) * fSlow14) * fSlow10);
		double fSlow63 = (fConst39 * ((fSlow62 * fSlow12) * fSlow15));
		double fSlow64 = ((3.25 * fSlow51) - (2.6666666666666665 * fSlow38));
		double fSlow65 = ((fSlow19 * fSlow64) * fSlow10);
		double fSlow66 = (fConst42 * fSlow65);
		double fSlow67 = (fSlow59 * fSlow10);
		double fSlow68 = (fConst44 * (fSlow67 * fSlow46));
		double fSlow69 = pow(fSlow12, 2.5);
		double fSlow70 = (fConst45 * fSlow69);
		double fSlow71 = (fSlow44 * fSlow15);
		double fSlow72 = (fSlow58 * fSlow14);
		double fSlow73 = (fSlow44 * fSlow10);
		double fSlow74 = (fConst49 * (fSlow73 * fSlow15));
		double fSlow75 = (fSlow72 * fSlow10);
		double fSlow76 = (fConst49 * fSlow75);
		double fSlow77 = ((2581.7315343145328 * fSlow11) + -198.59473340881021);
		double fSlow78 = (fSlow44 * fSlow77);
		double fSlow79 = (fConst51 * (fSlow78 * fSlow15));
		double fSlow80 = ((fSlow58 * fSlow77) * fSlow14);
		double fSlow81 = (fConst51 * fSlow80);
		double fSlow82 = ((3.0 * fSlow11) + -1.0);
		double fSlow83 = ((2.5 * fSlow82) + -2.0);
		double fSlow84 = (0.88191710368819687 * fSlow83);
		double fSlow85 = ((fSlow27 * fSlow46) * fSlow15);
		double fSlow86 = (fConst53 * fSlow85);
		double fSlow87 = ((2.333333333333333 * (fSlow83 * fSlow11)) - (1.5 * fSlow82));
		double fSlow88 = ((2.25 * fSlow87) - (1.3333333333333333 * fSlow83));
		double fSlow89 = (0.66332495807108005 * fSlow88);
		double fSlow90 = (fSlow36 * fSlow46);
		double fSlow91 = (fConst54 * fSlow90);
		double fSlow92 = mydsp_faustpower3_f(fSlow12);
		double fSlow93 = ((fSlow45 * fSlow92) * fSlow15);
		double fSlow94 = (fConst44 * fSlow93);
		double fSlow95 = ((2.2000000000000002 * (fSlow88 * fSlow11)) - (1.25 * fSlow87));
		double fSlow96 = ((2.1666666666666665 * fSlow95) - (1.2000000000000002 * fSlow88));
		double fSlow97 = (0.55328333517248807 * fSlow96);
		double fSlow98 = (fSlow36 * fSlow61);
		double fSlow99 = (fConst55 * (fSlow98 * fSlow46));
		double fSlow100 = pow(fSlow12, 1.5);
		double fSlow101 = (fConst56 * fSlow100);
		double fSlow102 = (fSlow25 * fSlow15);
		double fSlow103 = (fSlow34 * fSlow14);
		double fSlow104 = (fSlow25 * fSlow10);
		double fSlow105 = (fConst60 * (fSlow104 * fSlow15));
		double fSlow106 = (fSlow103 * fSlow10);
		double fSlow107 = (fConst60 * fSlow106);
		double fSlow108 = ((284.35155010806915 * fSlow11) + -31.594616678674353);
		double fSlow109 = (fSlow25 * fSlow108);
		double fSlow110 = (fConst62 * (fSlow109 * fSlow15));
		double fSlow111 = ((fSlow34 * fSlow108) * fSlow14);
		double fSlow112 = (fConst62 * fSlow111);
		double fSlow113 = ((5.5 * fSlow108) + -252.75693342939482);
		double fSlow114 = ((fSlow25 * fSlow113) * fSlow10);
		double fSlow115 = (fConst64 * (fSlow114 * fSlow15));
		double fSlow116 = (((fSlow34 * fSlow113) * fSlow14) * fSlow10);
		double fSlow117 = (fConst64 * fSlow116);
		double fSlow118 = ((4.333333333333333 * (fSlow113 * fSlow11)) - (4.5 * fSlow108));
		double fSlow119 = (fSlow25 * fSlow118);
		double fSlow120 = (fConst66 * (fSlow119 * fSlow15));
		double fSlow121 = ((fSlow34 * fSlow118) * fSlow14);
		double fSlow122 = (fConst66 * fSlow121);
		double fSlow123 = (1.1180339887498949 * fSlow82);
		double fSlow124 = (0.75 * fSlow87);
		double fSlow125 = (0.60092521257733145 * fSlow95);
		double fSlow126 = pow(fSlow12, 3.5);
		double fSlow127 = (((((2.0 * fSlow45) + (4.0 * (fSlow43 * fSlow14))) * fSlow14) + -1.0) * fSlow126);
		double fSlow128 = (fConst68 * fSlow127);
		double fSlow129 = (((fSlow64 * fSlow14) * fSlow10) * fSlow12);
		double fSlow130 = (fConst69 * fSlow129);
		double fSlow131 = pow(fSlow12, 0.5);
		double fSlow132 = (fConst70 * fSlow131);
		double fSlow133 = (fConst74 * (fSlow10 * fSlow15));
		double fSlow134 = (fConst74 * fSlow21);
		double fSlow135 = ((16.925687506432688 * fSlow11) + -3.3851375012865379);
		double fSlow136 = (fConst76 * (fSlow135 * fSlow15));
		double fSlow137 = (fSlow135 * fSlow14);
		double fSlow138 = (fConst76 * fSlow137);
		double fSlow139 = ((3.5 * fSlow135) + -13.540550005146152);
		double fSlow140 = (fSlow139 * fSlow10);
		double fSlow141 = (fConst78 * (fSlow140 * fSlow15));
		double fSlow142 = ((fSlow139 * fSlow14) * fSlow10);
		double fSlow143 = (fConst78 * fSlow142);
		double fSlow144 = ((3.0 * (fSlow139 * fSlow11)) - (2.5 * fSlow135));
		double fSlow145 = (fConst80 * (fSlow144 * fSlow15));
		double fSlow146 = (fSlow144 * fSlow14);
		double fSlow147 = (fConst80 * fSlow146);
		double fSlow148 = ((2.75 * fSlow144) - (2.0 * fSlow139));
		double fSlow149 = (fSlow148 * fSlow10);
		double fSlow150 = (fConst82 * (fSlow149 * fSlow15));
		double fSlow151 = ((fSlow148 * fSlow14) * fSlow10);
		double fSlow152 = (fConst82 * fSlow151);
		double fSlow153 = ((2.6000000000000001 * (fSlow148 * fSlow11)) - (1.75 * fSlow144));
		double fSlow154 = (fConst84 * (fSlow153 * fSlow15));
		double fSlow155 = (fSlow153 * fSlow14);
		double fSlow156 = (fConst84 * fSlow155);
		double fSlow157 = ((((2.0 * fSlow59) - fSlow58) * fSlow14) * fSlow126);
		double fSlow158 = (fConst68 * fSlow157);
		double fSlow159 = (fConst85 * (fSlow131 * fSlow15));
		double fSlow160 = (0.13783222385544802 * fSlow10);
		double fSlow161 = (fConst85 * (fSlow14 * fSlow131));
		double fSlow162 = (fConst87 * ((fSlow14 * fSlow12) * fSlow15));
		double fSlow163 = (fConst88 * ((fSlow10 * fSlow131) * fSlow15));
		double fSlow164 = (0.088970317927147144 * fSlow82);
		double fSlow165 = (fConst88 * (fSlow21 * fSlow131));
		double fSlow166 = (fConst89 * (fSlow19 * fSlow12));
		double fSlow167 = (fConst90 * ((fSlow25 * fSlow100) * fSlow15));
		double fSlow168 = (fConst92 * ((fSlow21 * fSlow12) * fSlow15));
		double fSlow169 = (fConst93 * ((fSlow135 * fSlow131) * fSlow15));
		double fSlow170 = (0.070180733224632072 * (fSlow83 * fSlow10));
		double fSlow171 = (fConst93 * (fSlow137 * fSlow131));
		double fSlow172 = (fConst94 * (fSlow23 * fSlow12));
		double fSlow173 = (fConst90 * (fSlow103 * fSlow100));
		double fSlow174 = (fConst95 * fSlow85);
		double fSlow175 = (fConst96 * ((fSlow104 * fSlow100) * fSlow15));
		double fSlow176 = (fConst97 * ((fSlow30 * fSlow12) * fSlow15));
		double fSlow177 = (fConst98 * ((fSlow140 * fSlow131) * fSlow15));
		double fSlow178 = (0.059683103659460751 * fSlow87);
		double fSlow179 = (fConst98 * (fSlow142 * fSlow131));
		double fSlow180 = (fConst99 * (fSlow32 * fSlow12));
		double fSlow181 = (fConst96 * (fSlow106 * fSlow100));
		double fSlow182 = (fConst100 * fSlow90);
		double fSlow183 = (fConst101 * ((fSlow44 * fSlow69) * fSlow15));
		double fSlow184 = (fConst102 * (((fSlow27 * fSlow10) * fSlow46) * fSlow15));
		double fSlow185 = (fConst103 * ((fSlow109 * fSlow100) * fSlow15));
		double fSlow186 = (fConst104 * ((fSlow39 * fSlow12) * fSlow15));
		double fSlow187 = (fConst105 * ((fSlow144 * fSlow131) * fSlow15));
		double fSlow188 = (0.052785722976618307 * (fSlow88 * fSlow10));
		double fSlow189 = (fConst105 * (fSlow146 * fSlow131));
		double fSlow190 = (fConst106 * (fSlow41 * fSlow12));
		double fSlow191 = (fConst103 * (fSlow111 * fSlow100));
		double fSlow192 = (fConst107 * ((fSlow36 * fSlow10) * fSlow46));
		double fSlow193 = (fConst101 * (fSlow72 * fSlow69));
		double fSlow194 = (fConst108 * fSlow93);
		double fSlow195 = (fConst109 * ((fSlow73 * fSlow69) * fSlow15));
		double fSlow196 = (fConst110 * ((fSlow49 * fSlow46) * fSlow15));
		double fSlow197 = (fConst111 * ((fSlow114 * fSlow100) * fSlow15));
		double fSlow198 = (fConst112 * ((fSlow52 * fSlow12) * fSlow15));
		double fSlow199 = (fConst113 * ((fSlow149 * fSlow131) * fSlow15));
		double fSlow200 = (0.047820109005115151 * fSlow95);
		double fSlow201 = (fConst113 * (fSlow151 * fSlow131));
		double fSlow202 = (fConst114 * (fSlow54 * fSlow12));
		double fSlow203 = (fConst111 * (fSlow116 * fSlow100));
		double fSlow204 = (fConst115 * (fSlow56 * fSlow46));
		double fSlow205 = (fConst109 * (fSlow75 * fSlow69));
		double fSlow206 = (fConst108 * (fSlow59 * fSlow92));
		double fSlow207 = (fConst116 * (fSlow127 * fSlow15));
		double fSlow208 = (fConst117 * (((fSlow45 * fSlow10) * fSlow92) * fSlow15));
		double fSlow209 = (fConst118 * ((fSlow78 * fSlow69) * fSlow15));
		double fSlow210 = (fConst119 * ((fSlow62 * fSlow46) * fSlow15));
		double fSlow211 = (fConst120 * ((fSlow119 * fSlow100) * fSlow15));
		double fSlow212 = (fConst121 * (fSlow129 * fSlow15));
		double fSlow213 = (fConst122 * ((fSlow153 * fSlow131) * fSlow15));
		double fSlow214 = (0.044028888861535699 * (fSlow96 * fSlow10));
		double fSlow215 = (fConst122 * (fSlow155 * fSlow131));
		double fSlow216 = (fConst123 * (fSlow65 * fSlow12));
		double fSlow217 = (fConst120 * (fSlow121 * fSlow100));
		double fSlow218 = (fConst124 * ((fSlow98 * fSlow10) * fSlow46));
		double fSlow219 = (fConst118 * (fSlow80 * fSlow69));
		double fSlow220 = (fConst117 * (fSlow67 * fSlow92));
		double fSlow221 = (fConst116 * fSlow157);
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec0[0] = (iSlow5?0.0:min(fSlow6, (fRec0[1] + 1.0)));
			double fTemp0 = double(input0[i]);
			fRec1[0] = (iSlow7?fSlow6:max(0.0, (fRec1[1] + -1.0)));
			fRec2[0] = (fSlow17 + (0.999 * fRec2[1]));
			fRec3[0] = (iSlow5?fSlow6:max(0.0, (fRec3[1] + -1.0)));
			double fTemp1 = (fRec2[0] * fRec3[0]);
			double fTemp2 = double(input4[i]);
			fRec4[0] = (iSlow7?0.0:min(fSlow6, (fRec4[1] + 1.0)));
			double fTemp3 = (fRec2[0] * fRec4[0]);
			double fTemp4 = double(input8[i]);
			double fTemp5 = double(input10[i]);
			double fTemp6 = double(input14[i]);
			double fTemp7 = double(input16[i]);
			double fTemp8 = double(input18[i]);
			double fTemp9 = double(input22[i]);
			double fTemp10 = double(input24[i]);
			double fTemp11 = double(input28[i]);
			double fTemp12 = double(input32[i]);
			double fTemp13 = double(input36[i]);
			double fTemp14 = double(input38[i]);
			double fTemp15 = double(input40[i]);
			double fTemp16 = double(input44[i]);
			double fTemp17 = double(input46[i]);
			double fTemp18 = double(input48[i]);
			double fTemp19 = double(input52[i]);
			double fTemp20 = double(input58[i]);
			double fTemp21 = double(input62[i]);
			double fTemp22 = double(input25[i]);
			double fTemp23 = double(input35[i]);
			double fTemp24 = double(input37[i]);
			double fTemp25 = double(input47[i]);
			double fTemp26 = double(input51[i]);
			double fTemp27 = double(input61[i]);
			double fTemp28 = double(input2[i]);
			double fTemp29 = double(input12[i]);
			double fTemp30 = double(input26[i]);
			double fTemp31 = double(input30[i]);
			double fTemp32 = double(input34[i]);
			double fTemp33 = double(input50[i]);
			double fTemp34 = double(input56[i]);
			double fTemp35 = double(input60[i]);
			double fTemp36 = double(input9[i]);
			double fTemp37 = double(input15[i]);
			double fTemp38 = double(input17[i]);
			double fTemp39 = double(input23[i]);
			double fTemp40 = double(input27[i]);
			double fTemp41 = double(input33[i]);
			double fTemp42 = double(input39[i]);
			double fTemp43 = double(input45[i]);
			double fTemp44 = double(input53[i]);
			double fTemp45 = double(input59[i]);
			double fTemp46 = double(input6[i]);
			double fTemp47 = double(input20[i]);
			double fTemp48 = double(input42[i]);
			double fTemp49 = double(input49[i]);
			double fTemp50 = double(input54[i]);
			double fTemp51 = double(input1[i]);
			double fTemp52 = double(input3[i]);
			double fTemp53 = double(input5[i]);
			double fTemp54 = double(input7[i]);
			double fTemp55 = double(input11[i]);
			double fTemp56 = double(input13[i]);
			double fTemp57 = double(input19[i]);
			double fTemp58 = double(input21[i]);
			double fTemp59 = double(input29[i]);
			double fTemp60 = double(input31[i]);
			double fTemp61 = double(input41[i]);
			double fTemp62 = double(input43[i]);
			double fTemp63 = double(input55[i]);
			double fTemp64 = double(input57[i]);
			double fTemp65 = double(input63[i]);
			double fTemp66 = (((fSlow12 * ((((((((((((((((fConst2 * (((fConst5 * ((fSlow16 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp2)):(iSlow2?(fSlow4 * (fTemp3 * fTemp2)):(fSlow4 * (fTemp1 * fTemp2))))) + (fSlow20 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp4)):(iSlow2?(fSlow4 * (fTemp3 * fTemp4)):(fSlow4 * (fTemp1 * fTemp4))))))) + (fSlow22 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp5)):(iSlow2?(fSlow4 * (fTemp3 * fTemp5)):(fSlow4 * (fTemp1 * fTemp5)))))) + (fSlow24 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp6)):(iSlow2?(fSlow4 * (fTemp3 * fTemp6)):(fSlow4 * (fTemp1 * fTemp6))))))) + (fSlow28 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp7)):(iSlow2?(fSlow4 * (fTemp3 * fTemp7)):(fSlow4 * (fTemp1 * fTemp7)))))) + (fSlow31 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp8)):(iSlow2?(fSlow4 * (fTemp3 * fTemp8)):(fSlow4 * (fTemp1 * fTemp8)))))) + (fSlow33 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp9)):(iSlow2?(fSlow4 * (fTemp3 * fTemp9)):(fSlow4 * (fTemp1 * fTemp9)))))) + (fSlow37 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp10)):(iSlow2?(fSlow4 * (fTemp3 * fTemp10)):(fSlow4 * (fTemp1 * fTemp10)))))) + (fSlow40 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp11)):(iSlow2?(fSlow4 * (fTemp3 * fTemp11)):(fSlow4 * (fTemp1 * fTemp11)))))) + (fSlow42 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp12)):(iSlow2?(fSlow4 * (fTemp3 * fTemp12)):(fSlow4 * (fTemp1 * fTemp12)))))) + (fSlow47 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp13)):(iSlow2?(fSlow4 * (fTemp3 * fTemp13)):(fSlow4 * (fTemp1 * fTemp13)))))) + (fSlow50 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp14)):(iSlow2?(fSlow4 * (fTemp3 * fTemp14)):(fSlow4 * (fTemp1 * fTemp14)))))) + (fSlow53 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp15)):(iSlow2?(fSlow4 * (fTemp3 * fTemp15)):(fSlow4 * (fTemp1 * fTemp15)))))) + (fSlow55 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp16)):(iSlow2?(fSlow4 * (fTemp3 * fTemp16)):(fSlow4 * (fTemp1 * fTemp16)))))) + (fSlow57 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp17)):(iSlow2?(fSlow4 * (fTemp3 * fTemp17)):(fSlow4 * (fTemp1 * fTemp17)))))) + (fSlow60 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp18)):(iSlow2?(fSlow4 * (fTemp3 * fTemp18)):(fSlow4 * (fTemp1 * fTemp18)))))) + (fSlow63 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp19)):(iSlow2?(fSlow4 * (fTemp3 * fTemp19)):(fSlow4 * (fTemp1 * fTemp19)))))) + (fSlow66 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp20)):(iSlow2?(fSlow4 * (fTemp3 * fTemp20)):(fSlow4 * (fTemp1 * fTemp20)))))) + (fSlow68 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp21)):(iSlow2?(fSlow4 * (fTemp3 * fTemp21)):(fSlow4 * (fTemp1 * fTemp21))))))) + ((fSlow70 * (((((fConst47 * ((fSlow71 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp22)):(iSlow2?(fSlow4 * (fTemp3 * fTemp22)):(fSlow4 * (fTemp1 * fTemp22))))) + (fSlow72 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp23)):(iSlow2?(fSlow4 * (fTemp3 * fTemp23)):(fSlow4 * (fTemp1 * fTemp23))))))) + (fSlow74 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp24)):(iSlow2?(fSlow4 * (fTemp3 * fTemp24)):(fSlow4 * (fTemp1 * fTemp24)))))) + (fSlow76 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp25)):(iSlow2?(fSlow4 * (fTemp3 * fTemp25)):(fSlow4 * (fTemp1 * fTemp25)))))) + (fSlow79 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp26)):(iSlow2?(fSlow4 * (fTemp3 * fTemp26)):(fSlow4 * (fTemp1 * fTemp26)))))) + (fSlow81 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp27)):(iSlow2?(fSlow4 * (fTemp3 * fTemp27)):(fSlow4 * (fTemp1 * fTemp27))))))) + ((fSlow10 * ((((((((1.7320508075688772 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp28)):(iSlow2?(fSlow4 * (fTemp3 * fTemp28)):(fSlow4 * (fTemp1 * fTemp28))))) + (fSlow84 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp29)):(iSlow2?(fSlow4 * (fTemp3 * fTemp29)):(fSlow4 * (fTemp1 * fTemp29)))))) + (fSlow86 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp30)):(iSlow2?(fSlow4 * (fTemp3 * fTemp30)):(fSlow4 * (fTemp1 * fTemp30)))))) + (fSlow89 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp31)):(iSlow2?(fSlow4 * (fTemp3 * fTemp31)):(fSlow4 * (fTemp1 * fTemp31)))))) + (fSlow91 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp32)):(iSlow2?(fSlow4 * (fTemp3 * fTemp32)):(fSlow4 * (fTemp1 * fTemp32)))))) + (fSlow94 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp33)):(iSlow2?(fSlow4 * (fTemp3 * fTemp33)):(fSlow4 * (fTemp1 * fTemp33)))))) + (fSlow97 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp34)):(iSlow2?(fSlow4 * (fTemp3 * fTemp34)):(fSlow4 * (fTemp1 * fTemp34)))))) + (fSlow99 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp35)):(iSlow2?(fSlow4 * (fTemp3 * fTemp35)):(fSlow4 * (fTemp1 * fTemp35))))))) + ((fSlow101 * (((((((((fConst58 * ((fSlow102 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp36)):(iSlow2?(fSlow4 * (fTemp3 * fTemp36)):(fSlow4 * (fTemp1 * fTemp36))))) + (fSlow103 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp37)):(iSlow2?(fSlow4 * (fTemp3 * fTemp37)):(fSlow4 * (fTemp1 * fTemp37))))))) + (fSlow105 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp38)):(iSlow2?(fSlow4 * (fTemp3 * fTemp38)):(fSlow4 * (fTemp1 * fTemp38)))))) + (fSlow107 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp39)):(iSlow2?(fSlow4 * (fTemp3 * fTemp39)):(fSlow4 * (fTemp1 * fTemp39)))))) + (fSlow110 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp40)):(iSlow2?(fSlow4 * (fTemp3 * fTemp40)):(fSlow4 * (fTemp1 * fTemp40)))))) + (fSlow112 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp41)):(iSlow2?(fSlow4 * (fTemp3 * fTemp41)):(fSlow4 * (fTemp1 * fTemp41)))))) + (fSlow115 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp42)):(iSlow2?(fSlow4 * (fTemp3 * fTemp42)):(fSlow4 * (fTemp1 * fTemp42)))))) + (fSlow117 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp43)):(iSlow2?(fSlow4 * (fTemp3 * fTemp43)):(fSlow4 * (fTemp1 * fTemp43)))))) + (fSlow120 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp44)):(iSlow2?(fSlow4 * (fTemp3 * fTemp44)):(fSlow4 * (fTemp1 * fTemp44)))))) + (fSlow122 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp45)):(iSlow2?(fSlow4 * (fTemp3 * fTemp45)):(fSlow4 * (fTemp1 * fTemp45))))))) + ((((((iSlow0?(fSlow8 * (fRec2[0] * fTemp0)):(iSlow2?(fSlow4 * (fTemp3 * fTemp0)):(fSlow4 * (fTemp1 * fTemp0)))) + (fSlow123 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp46)):(iSlow2?(fSlow4 * (fTemp3 * fTemp46)):(fSlow4 * (fTemp1 * fTemp46)))))) + (fSlow124 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp47)):(iSlow2?(fSlow4 * (fTemp3 * fTemp47)):(fSlow4 * (fTemp1 * fTemp47)))))) + (fSlow125 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp48)):(iSlow2?(fSlow4 * (fTemp3 * fTemp48)):(fSlow4 * (fTemp1 * fTemp48)))))) + (fSlow15 * ((fSlow128 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp49)):(iSlow2?(fSlow4 * (fTemp3 * fTemp49)):(fSlow4 * (fTemp1 * fTemp49))))) + (fSlow130 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp50)):(iSlow2?(fSlow4 * (fTemp3 * fTemp50)):(fSlow4 * (fTemp1 * fTemp50)))))))) + (fSlow132 * (((((((((((((fConst72 * ((fSlow15 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp51)):(iSlow2?(fSlow4 * (fTemp3 * fTemp51)):(fSlow4 * (fTemp1 * fTemp51))))) + (fSlow14 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp52)):(iSlow2?(fSlow4 * (fTemp3 * fTemp52)):(fSlow4 * (fTemp1 * fTemp52))))))) + (fSlow133 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp53)):(iSlow2?(fSlow4 * (fTemp3 * fTemp53)):(fSlow4 * (fTemp1 * fTemp53)))))) + (fSlow134 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp54)):(iSlow2?(fSlow4 * (fTemp3 * fTemp54)):(fSlow4 * (fTemp1 * fTemp54)))))) + (fSlow136 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp55)):(iSlow2?(fSlow4 * (fTemp3 * fTemp55)):(fSlow4 * (fTemp1 * fTemp55)))))) + (fSlow138 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp56)):(iSlow2?(fSlow4 * (fTemp3 * fTemp56)):(fSlow4 * (fTemp1 * fTemp56)))))) + (fSlow141 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp57)):(iSlow2?(fSlow4 * (fTemp3 * fTemp57)):(fSlow4 * (fTemp1 * fTemp57)))))) + (fSlow143 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp58)):(iSlow2?(fSlow4 * (fTemp3 * fTemp58)):(fSlow4 * (fTemp1 * fTemp58)))))) + (fSlow145 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp59)):(iSlow2?(fSlow4 * (fTemp3 * fTemp59)):(fSlow4 * (fTemp1 * fTemp59)))))) + (fSlow147 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp60)):(iSlow2?(fSlow4 * (fTemp3 * fTemp60)):(fSlow4 * (fTemp1 * fTemp60)))))) + (fSlow150 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp61)):(iSlow2?(fSlow4 * (fTemp3 * fTemp61)):(fSlow4 * (fTemp1 * fTemp61)))))) + (fSlow152 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp62)):(iSlow2?(fSlow4 * (fTemp3 * fTemp62)):(fSlow4 * (fTemp1 * fTemp62)))))) + (fSlow154 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp63)):(iSlow2?(fSlow4 * (fTemp3 * fTemp63)):(fSlow4 * (fTemp1 * fTemp63)))))) + (fSlow156 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp64)):(iSlow2?(fSlow4 * (fTemp3 * fTemp64)):(fSlow4 * (fTemp1 * fTemp64)))))))))))) + (fSlow158 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp65)):(iSlow2?(fSlow4 * (fTemp3 * fTemp65)):(fSlow4 * (fTemp1 * fTemp65))))));
			output0[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp0):(iSlow2?(fSlow4 * (fRec1[0] * fTemp0)):(fSlow4 * (fRec0[0] * fTemp0)))) + (0.079577471545947673 * fTemp66)));
			output1[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp51):(iSlow2?(fSlow4 * (fRec1[0] * fTemp51)):(fSlow4 * (fRec0[0] * fTemp51)))) + (fSlow159 * fTemp66)));
			output2[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp28):(iSlow2?(fSlow4 * (fRec1[0] * fTemp28)):(fSlow4 * (fRec0[0] * fTemp28)))) + (fSlow160 * fTemp66)));
			output3[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp52):(iSlow2?(fSlow4 * (fRec1[0] * fTemp52)):(fSlow4 * (fRec0[0] * fTemp52)))) + (fSlow161 * fTemp66)));
			output4[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp2):(iSlow2?(fSlow4 * (fRec1[0] * fTemp2)):(fSlow4 * (fRec0[0] * fTemp2)))) + (fSlow162 * fTemp66)));
			output5[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp53):(iSlow2?(fSlow4 * (fRec1[0] * fTemp53)):(fSlow4 * (fRec0[0] * fTemp53)))) + (fSlow163 * fTemp66)));
			output6[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp46):(iSlow2?(fSlow4 * (fRec1[0] * fTemp46)):(fSlow4 * (fRec0[0] * fTemp46)))) + (fSlow164 * fTemp66)));
			output7[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp54):(iSlow2?(fSlow4 * (fRec1[0] * fTemp54)):(fSlow4 * (fRec0[0] * fTemp54)))) + (fSlow165 * fTemp66)));
			output8[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp4):(iSlow2?(fSlow4 * (fRec1[0] * fTemp4)):(fSlow4 * (fRec0[0] * fTemp4)))) + (fSlow166 * fTemp66)));
			output9[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp36):(iSlow2?(fSlow4 * (fRec1[0] * fTemp36)):(fSlow4 * (fRec0[0] * fTemp36)))) + (fSlow167 * fTemp66)));
			output10[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp5):(iSlow2?(fSlow4 * (fRec1[0] * fTemp5)):(fSlow4 * (fRec0[0] * fTemp5)))) + (fSlow168 * fTemp66)));
			output11[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp55):(iSlow2?(fSlow4 * (fRec1[0] * fTemp55)):(fSlow4 * (fRec0[0] * fTemp55)))) + (fSlow169 * fTemp66)));
			output12[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp29):(iSlow2?(fSlow4 * (fRec1[0] * fTemp29)):(fSlow4 * (fRec0[0] * fTemp29)))) + (fSlow170 * fTemp66)));
			output13[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp56):(iSlow2?(fSlow4 * (fRec1[0] * fTemp56)):(fSlow4 * (fRec0[0] * fTemp56)))) + (fSlow171 * fTemp66)));
			output14[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp6):(iSlow2?(fSlow4 * (fRec1[0] * fTemp6)):(fSlow4 * (fRec0[0] * fTemp6)))) + (fSlow172 * fTemp66)));
			output15[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp37):(iSlow2?(fSlow4 * (fRec1[0] * fTemp37)):(fSlow4 * (fRec0[0] * fTemp37)))) + (fSlow173 * fTemp66)));
			output16[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp7):(iSlow2?(fSlow4 * (fRec1[0] * fTemp7)):(fSlow4 * (fRec0[0] * fTemp7)))) + (fSlow174 * fTemp66)));
			output17[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp38):(iSlow2?(fSlow4 * (fRec1[0] * fTemp38)):(fSlow4 * (fRec0[0] * fTemp38)))) + (fSlow175 * fTemp66)));
			output18[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp8):(iSlow2?(fSlow4 * (fRec1[0] * fTemp8)):(fSlow4 * (fRec0[0] * fTemp8)))) + (fSlow176 * fTemp66)));
			output19[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp57):(iSlow2?(fSlow4 * (fRec1[0] * fTemp57)):(fSlow4 * (fRec0[0] * fTemp57)))) + (fSlow177 * fTemp66)));
			output20[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp47):(iSlow2?(fSlow4 * (fRec1[0] * fTemp47)):(fSlow4 * (fRec0[0] * fTemp47)))) + (fSlow178 * fTemp66)));
			output21[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp58):(iSlow2?(fSlow4 * (fRec1[0] * fTemp58)):(fSlow4 * (fRec0[0] * fTemp58)))) + (fSlow179 * fTemp66)));
			output22[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp9):(iSlow2?(fSlow4 * (fRec1[0] * fTemp9)):(fSlow4 * (fRec0[0] * fTemp9)))) + (fSlow180 * fTemp66)));
			output23[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp39):(iSlow2?(fSlow4 * (fRec1[0] * fTemp39)):(fSlow4 * (fRec0[0] * fTemp39)))) + (fSlow181 * fTemp66)));
			output24[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp10):(iSlow2?(fSlow4 * (fRec1[0] * fTemp10)):(fSlow4 * (fRec0[0] * fTemp10)))) + (fSlow182 * fTemp66)));
			output25[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp22):(iSlow2?(fSlow4 * (fRec1[0] * fTemp22)):(fSlow4 * (fRec0[0] * fTemp22)))) + (fSlow183 * fTemp66)));
			output26[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp30):(iSlow2?(fSlow4 * (fRec1[0] * fTemp30)):(fSlow4 * (fRec0[0] * fTemp30)))) + (fSlow184 * fTemp66)));
			output27[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp40):(iSlow2?(fSlow4 * (fRec1[0] * fTemp40)):(fSlow4 * (fRec0[0] * fTemp40)))) + (fSlow185 * fTemp66)));
			output28[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp11):(iSlow2?(fSlow4 * (fRec1[0] * fTemp11)):(fSlow4 * (fRec0[0] * fTemp11)))) + (fSlow186 * fTemp66)));
			output29[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp59):(iSlow2?(fSlow4 * (fRec1[0] * fTemp59)):(fSlow4 * (fRec0[0] * fTemp59)))) + (fSlow187 * fTemp66)));
			output30[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp31):(iSlow2?(fSlow4 * (fRec1[0] * fTemp31)):(fSlow4 * (fRec0[0] * fTemp31)))) + (fSlow188 * fTemp66)));
			output31[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp60):(iSlow2?(fSlow4 * (fRec1[0] * fTemp60)):(fSlow4 * (fRec0[0] * fTemp60)))) + (fSlow189 * fTemp66)));
			output32[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp12):(iSlow2?(fSlow4 * (fRec1[0] * fTemp12)):(fSlow4 * (fRec0[0] * fTemp12)))) + (fSlow190 * fTemp66)));
			output33[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp41):(iSlow2?(fSlow4 * (fRec1[0] * fTemp41)):(fSlow4 * (fRec0[0] * fTemp41)))) + (fSlow191 * fTemp66)));
			output34[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp32):(iSlow2?(fSlow4 * (fRec1[0] * fTemp32)):(fSlow4 * (fRec0[0] * fTemp32)))) + (fSlow192 * fTemp66)));
			output35[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp23):(iSlow2?(fSlow4 * (fRec1[0] * fTemp23)):(fSlow4 * (fRec0[0] * fTemp23)))) + (fSlow193 * fTemp66)));
			output36[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp13):(iSlow2?(fSlow4 * (fRec1[0] * fTemp13)):(fSlow4 * (fRec0[0] * fTemp13)))) + (fSlow194 * fTemp66)));
			output37[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp24):(iSlow2?(fSlow4 * (fRec1[0] * fTemp24)):(fSlow4 * (fRec0[0] * fTemp24)))) + (fSlow195 * fTemp66)));
			output38[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp14):(iSlow2?(fSlow4 * (fRec1[0] * fTemp14)):(fSlow4 * (fRec0[0] * fTemp14)))) + (fSlow196 * fTemp66)));
			output39[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp42):(iSlow2?(fSlow4 * (fRec1[0] * fTemp42)):(fSlow4 * (fRec0[0] * fTemp42)))) + (fSlow197 * fTemp66)));
			output40[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp15):(iSlow2?(fSlow4 * (fRec1[0] * fTemp15)):(fSlow4 * (fRec0[0] * fTemp15)))) + (fSlow198 * fTemp66)));
			output41[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp61):(iSlow2?(fSlow4 * (fRec1[0] * fTemp61)):(fSlow4 * (fRec0[0] * fTemp61)))) + (fSlow199 * fTemp66)));
			output42[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp48):(iSlow2?(fSlow4 * (fRec1[0] * fTemp48)):(fSlow4 * (fRec0[0] * fTemp48)))) + (fSlow200 * fTemp66)));
			output43[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp62):(iSlow2?(fSlow4 * (fRec1[0] * fTemp62)):(fSlow4 * (fRec0[0] * fTemp62)))) + (fSlow201 * fTemp66)));
			output44[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp16):(iSlow2?(fSlow4 * (fRec1[0] * fTemp16)):(fSlow4 * (fRec0[0] * fTemp16)))) + (fSlow202 * fTemp66)));
			output45[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp43):(iSlow2?(fSlow4 * (fRec1[0] * fTemp43)):(fSlow4 * (fRec0[0] * fTemp43)))) + (fSlow203 * fTemp66)));
			output46[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp17):(iSlow2?(fSlow4 * (fRec1[0] * fTemp17)):(fSlow4 * (fRec0[0] * fTemp17)))) + (fSlow204 * fTemp66)));
			output47[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp25):(iSlow2?(fSlow4 * (fRec1[0] * fTemp25)):(fSlow4 * (fRec0[0] * fTemp25)))) + (fSlow205 * fTemp66)));
			output48[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp18):(iSlow2?(fSlow4 * (fRec1[0] * fTemp18)):(fSlow4 * (fRec0[0] * fTemp18)))) + (fSlow206 * fTemp66)));
			output49[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp49):(iSlow2?(fSlow4 * (fRec1[0] * fTemp49)):(fSlow4 * (fRec0[0] * fTemp49)))) + (fSlow207 * fTemp66)));
			output50[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp33):(iSlow2?(fSlow4 * (fRec1[0] * fTemp33)):(fSlow4 * (fRec0[0] * fTemp33)))) + (fSlow208 * fTemp66)));
			output51[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp26):(iSlow2?(fSlow4 * (fRec1[0] * fTemp26)):(fSlow4 * (fRec0[0] * fTemp26)))) + (fSlow209 * fTemp66)));
			output52[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp19):(iSlow2?(fSlow4 * (fRec1[0] * fTemp19)):(fSlow4 * (fRec0[0] * fTemp19)))) + (fSlow210 * fTemp66)));
			output53[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp44):(iSlow2?(fSlow4 * (fRec1[0] * fTemp44)):(fSlow4 * (fRec0[0] * fTemp44)))) + (fSlow211 * fTemp66)));
			output54[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp50):(iSlow2?(fSlow4 * (fRec1[0] * fTemp50)):(fSlow4 * (fRec0[0] * fTemp50)))) + (fSlow212 * fTemp66)));
			output55[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp63):(iSlow2?(fSlow4 * (fRec1[0] * fTemp63)):(fSlow4 * (fRec0[0] * fTemp63)))) + (fSlow213 * fTemp66)));
			output56[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp34):(iSlow2?(fSlow4 * (fRec1[0] * fTemp34)):(fSlow4 * (fRec0[0] * fTemp34)))) + (fSlow214 * fTemp66)));
			output57[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp64):(iSlow2?(fSlow4 * (fRec1[0] * fTemp64)):(fSlow4 * (fRec0[0] * fTemp64)))) + (fSlow215 * fTemp66)));
			output58[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp20):(iSlow2?(fSlow4 * (fRec1[0] * fTemp20)):(fSlow4 * (fRec0[0] * fTemp20)))) + (fSlow216 * fTemp66)));
			output59[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp45):(iSlow2?(fSlow4 * (fRec1[0] * fTemp45)):(fSlow4 * (fRec0[0] * fTemp45)))) + (fSlow217 * fTemp66)));
			output60[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp35):(iSlow2?(fSlow4 * (fRec1[0] * fTemp35)):(fSlow4 * (fRec0[0] * fTemp35)))) + (fSlow218 * fTemp66)));
			output61[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp27):(iSlow2?(fSlow4 * (fRec1[0] * fTemp27)):(fSlow4 * (fRec0[0] * fTemp27)))) + (fSlow219 * fTemp66)));
			output62[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp21):(iSlow2?(fSlow4 * (fRec1[0] * fTemp21)):(fSlow4 * (fRec0[0] * fTemp21)))) + (fSlow220 * fTemp66)));
			output63[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp65):(iSlow2?(fSlow4 * (fRec1[0] * fTemp65)):(fSlow4 * (fRec0[0] * fTemp65)))) + (fSlow221 * fTemp66)));
			fRec0[1] = fRec0[0];
			fRec1[1] = fRec1[0];
			fRec2[1] = fRec2[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			
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
