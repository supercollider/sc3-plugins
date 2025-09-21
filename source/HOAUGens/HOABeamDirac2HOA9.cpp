/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2016"
license: "GPL"
name: "HOABeamDirac2HOA9"
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
double mydsp_faustpower4_f(double value) {
	return (((value * value) * value) * value);
	
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
	FAUSTFLOAT fVslider2;
	double fRec2[2];
	double fRec3[2];
	double fRec4[2];
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	double fConst6;
	FAUSTFLOAT fVslider3;
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
	double fConst125;
	double fConst126;
	double fConst127;
	double fConst128;
	double fConst129;
	double fConst130;
	double fConst131;
	double fConst132;
	double fConst133;
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
	double fConst181;
	double fConst182;
	double fConst183;
	double fConst184;
	double fConst185;
	double fConst186;
	double fConst187;
	double fConst188;
	double fConst189;
	double fConst190;
	double fConst191;
	double fConst192;
	double fConst193;
	double fConst194;
	double fConst195;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
		m->declare("filename", "HOABeamDirac2HOA9");
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
		m->declare("name", "HOABeamDirac2HOA9");
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
		return 100;
		
	}
	virtual int getNumOutputs() {
		return 100;
		
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
			case 64: {
				rate = 1;
				break;
			}
			case 65: {
				rate = 1;
				break;
			}
			case 66: {
				rate = 1;
				break;
			}
			case 67: {
				rate = 1;
				break;
			}
			case 68: {
				rate = 1;
				break;
			}
			case 69: {
				rate = 1;
				break;
			}
			case 70: {
				rate = 1;
				break;
			}
			case 71: {
				rate = 1;
				break;
			}
			case 72: {
				rate = 1;
				break;
			}
			case 73: {
				rate = 1;
				break;
			}
			case 74: {
				rate = 1;
				break;
			}
			case 75: {
				rate = 1;
				break;
			}
			case 76: {
				rate = 1;
				break;
			}
			case 77: {
				rate = 1;
				break;
			}
			case 78: {
				rate = 1;
				break;
			}
			case 79: {
				rate = 1;
				break;
			}
			case 80: {
				rate = 1;
				break;
			}
			case 81: {
				rate = 1;
				break;
			}
			case 82: {
				rate = 1;
				break;
			}
			case 83: {
				rate = 1;
				break;
			}
			case 84: {
				rate = 1;
				break;
			}
			case 85: {
				rate = 1;
				break;
			}
			case 86: {
				rate = 1;
				break;
			}
			case 87: {
				rate = 1;
				break;
			}
			case 88: {
				rate = 1;
				break;
			}
			case 89: {
				rate = 1;
				break;
			}
			case 90: {
				rate = 1;
				break;
			}
			case 91: {
				rate = 1;
				break;
			}
			case 92: {
				rate = 1;
				break;
			}
			case 93: {
				rate = 1;
				break;
			}
			case 94: {
				rate = 1;
				break;
			}
			case 95: {
				rate = 1;
				break;
			}
			case 96: {
				rate = 1;
				break;
			}
			case 97: {
				rate = 1;
				break;
			}
			case 98: {
				rate = 1;
				break;
			}
			case 99: {
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
			case 64: {
				rate = 1;
				break;
			}
			case 65: {
				rate = 1;
				break;
			}
			case 66: {
				rate = 1;
				break;
			}
			case 67: {
				rate = 1;
				break;
			}
			case 68: {
				rate = 1;
				break;
			}
			case 69: {
				rate = 1;
				break;
			}
			case 70: {
				rate = 1;
				break;
			}
			case 71: {
				rate = 1;
				break;
			}
			case 72: {
				rate = 1;
				break;
			}
			case 73: {
				rate = 1;
				break;
			}
			case 74: {
				rate = 1;
				break;
			}
			case 75: {
				rate = 1;
				break;
			}
			case 76: {
				rate = 1;
				break;
			}
			case 77: {
				rate = 1;
				break;
			}
			case 78: {
				rate = 1;
				break;
			}
			case 79: {
				rate = 1;
				break;
			}
			case 80: {
				rate = 1;
				break;
			}
			case 81: {
				rate = 1;
				break;
			}
			case 82: {
				rate = 1;
				break;
			}
			case 83: {
				rate = 1;
				break;
			}
			case 84: {
				rate = 1;
				break;
			}
			case 85: {
				rate = 1;
				break;
			}
			case 86: {
				rate = 1;
				break;
			}
			case 87: {
				rate = 1;
				break;
			}
			case 88: {
				rate = 1;
				break;
			}
			case 89: {
				rate = 1;
				break;
			}
			case 90: {
				rate = 1;
				break;
			}
			case 91: {
				rate = 1;
				break;
			}
			case 92: {
				rate = 1;
				break;
			}
			case 93: {
				rate = 1;
				break;
			}
			case 94: {
				rate = 1;
				break;
			}
			case 95: {
				rate = 1;
				break;
			}
			case 96: {
				rate = 1;
				break;
			}
			case 97: {
				rate = 1;
				break;
			}
			case 98: {
				rate = 1;
				break;
			}
			case 99: {
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
		fConst2 = double(tgamma(2.0));
		fConst3 = double(tgamma(10.0));
		fConst4 = double(tgamma(4.5));
		fConst5 = (sqrt((11.0 * (fConst2 / fConst3))) * fConst4);
		fConst6 = (229.79075351122526 * fConst5);
		fConst7 = (114.89537675561263 * fConst5);
		fConst8 = double(tgamma(14.0));
		fConst9 = double(tgamma(6.5));
		fConst10 = (sqrt((15.0 * (fConst2 / fConst8))) * fConst9);
		fConst11 = (663.83995458798404 * fConst10);
		fConst12 = double(tgamma(4.0));
		fConst13 = double(tgamma(12.0));
		fConst14 = (sqrt((15.0 * (fConst12 / fConst13))) * fConst4);
		fConst15 = (0.47140452079103168 * fConst14);
		fConst16 = double(tgamma(8.5));
		fConst17 = (sqrt((19.0 * (fConst2 / double(tgamma(18.0))))) * fConst16);
		fConst18 = (6944.7872172281404 * fConst17);
		fConst19 = double(tgamma(8.0));
		fConst20 = double(tgamma(2.5));
		fConst21 = (sqrt((19.0 * (fConst19 / fConst13))) * fConst20);
		fConst22 = (0.40406101782088433 * fConst21);
		fConst23 = double(tgamma(6.0));
		fConst24 = (sqrt((19.0 * (fConst23 / fConst8))) * fConst4);
		fConst25 = (0.28284271247461906 * fConst24);
		fConst26 = (3472.3936086140702 * fConst17);
		fConst27 = double(tgamma(7.5));
		fConst28 = double(tgamma(1.0));
		fConst29 = double(tgamma(15.0));
		fConst30 = sqrt((15.0 * (fConst28 / fConst29)));
		fConst31 = (102.12922378276677 * fConst30);
		fConst32 = double(tgamma(16.0));
		fConst33 = sqrt((17.0 * (fConst2 / fConst32)));
		fConst34 = (1531.9383567415016 * fConst33);
		fConst35 = double(tgamma(3.0));
		fConst36 = double(tgamma(17.0));
		fConst37 = sqrt((19.0 * (fConst35 / fConst36)));
		fConst38 = (0.70710678118654757 * fConst37);
		fConst39 = double(tgamma(5.0));
		fConst40 = sqrt((5.0 * (fConst28 / fConst39)));
		fConst41 = sqrt((7.0 * (fConst2 / fConst23)));
		fConst42 = (31.915382432114615 * fConst41);
		fConst43 = (15.957691216057308 * fConst41);
		fConst44 = double(tgamma(9.0));
		fConst45 = (sqrt((9.0 * (fConst28 / fConst44))) * fConst4);
		fConst46 = (25.532305945691693 * fConst45);
		fConst47 = double(tgamma(7.0));
		fConst48 = (sqrt((9.0 * (fConst35 / fConst47))) * fConst20);
		fConst49 = (1.4142135623730951 * fConst48);
		fConst50 = (0.70710678118654757 * fConst48);
		fConst51 = (12.766152972845846 * fConst45);
		fConst52 = (sqrt((11.0 * (fConst12 / fConst19))) * fConst20);
		fConst53 = (0.94280904158206336 * fConst52);
		fConst54 = (0.47140452079103168 * fConst52);
		fConst55 = double(tgamma(13.0));
		fConst56 = (sqrt((13.0 * (fConst28 / fConst55))) * fConst9);
		fConst57 = (51.064611891383386 * fConst56);
		fConst58 = double(tgamma(11.0));
		fConst59 = (sqrt((13.0 * (fConst35 / fConst58))) * fConst4);
		fConst60 = (1.4142135623730951 * fConst59);
		fConst61 = (sqrt((13.0 * (fConst39 / fConst44))) * fConst20);
		fConst62 = (0.70710678118654757 * fConst61);
		fConst63 = (0.35355339059327379 * fConst61);
		fConst64 = (0.70710678118654757 * fConst59);
		fConst65 = (0.94280904158206336 * fConst14);
		fConst66 = (sqrt((15.0 * (fConst23 / fConst3))) * fConst20);
		fConst67 = (0.28284271247461906 * fConst66);
		fConst68 = (sqrt((17.0 * (fConst35 / fConst29))) * fConst9);
		fConst69 = (0.70710678118654757 * fConst68);
		fConst70 = (sqrt((17.0 * (fConst47 / fConst58))) * fConst20);
		fConst71 = (0.47140452079103168 * fConst70);
		fConst72 = (0.23570226039551584 * fConst70);
		fConst73 = (sqrt((17.0 * (fConst39 / fConst55))) * fConst4);
		fConst74 = (0.35355339059327379 * fConst73);
		fConst75 = (sqrt((17.0 * (fConst28 / fConst36))) * fConst16);
		fConst76 = (204.25844756553354 * fConst75);
		fConst77 = (sqrt((19.0 * (fConst12 / fConst32))) * fConst9);
		fConst78 = (0.47140452079103168 * fConst77);
		fConst79 = (0.20203050891044216 * fConst21);
		fConst80 = double(tgamma(5.5));
		fConst81 = sqrt((11.0 * (fConst28 / fConst58)));
		fConst82 = (25.532305945691693 * fConst81);
		fConst83 = sqrt((13.0 * (fConst2 / fConst13)));
		fConst84 = (280.85536540260864 * fConst83);
		fConst85 = sqrt((15.0 * (fConst35 / fConst55)));
		fConst86 = (0.70710678118654757 * fConst85);
		fConst87 = sqrt((17.0 * (fConst12 / fConst8)));
		fConst88 = (0.47140452079103168 * fConst87);
		fConst89 = sqrt((19.0 * (fConst39 / fConst29)));
		fConst90 = (0.35355339059327379 * fConst89);
		fConst91 = double(tgamma(3.5));
		fConst92 = sqrt((7.0 * (fConst28 / fConst47)));
		fConst93 = (6.3830764864229232 * fConst92);
		fConst94 = sqrt((9.0 * (fConst2 / fConst19)));
		fConst95 = (44.681535404960464 * fConst94);
		fConst96 = sqrt((11.0 * (fConst35 / fConst44)));
		fConst97 = (0.70710678118654757 * fConst96);
		fConst98 = sqrt((13.0 * (fConst12 / fConst3)));
		fConst99 = (0.47140452079103168 * fConst98);
		fConst100 = sqrt((15.0 * (fConst39 / fConst58)));
		fConst101 = (0.35355339059327379 * fConst100);
		fConst102 = sqrt((17.0 * (fConst23 / fConst13)));
		fConst103 = (0.28284271247461906 * fConst102);
		fConst104 = sqrt((19.0 * (fConst47 / fConst55)));
		fConst105 = (0.23570226039551584 * fConst104);
		fConst106 = double(tgamma(1.5));
		fConst107 = sqrt((3.0 * (fConst28 / fConst35)));
		fConst108 = (1.5957691216057308 * fConst107);
		fConst109 = sqrt((5.0 * (fConst2 / fConst12)));
		fConst110 = (4.7873073648171927 * fConst109);
		fConst111 = sqrt((7.0 * (fConst35 / fConst39)));
		fConst112 = (0.70710678118654757 * fConst111);
		fConst113 = sqrt((9.0 * (fConst12 / fConst23)));
		fConst114 = (0.47140452079103168 * fConst113);
		fConst115 = sqrt((11.0 * (fConst39 / fConst47)));
		fConst116 = (0.35355339059327379 * fConst115);
		fConst117 = sqrt((13.0 * (fConst23 / fConst19)));
		fConst118 = (0.28284271247461906 * fConst117);
		fConst119 = sqrt((15.0 * (fConst47 / fConst44)));
		fConst120 = (0.23570226039551584 * fConst119);
		fConst121 = sqrt((17.0 * (fConst19 / fConst3)));
		fConst122 = (0.20203050891044216 * fConst121);
		fConst123 = sqrt((19.0 * (fConst44 / fConst58)));
		fConst124 = (0.17677669529663689 * fConst123);
		fConst125 = (fConst30 * fConst27);
		fConst126 = (102.12922378276677 * fConst125);
		fConst127 = (0.56568542494923812 * fConst66);
		fConst128 = (408.51689513106709 * fConst75);
		fConst129 = (0.70710678118654757 * fConst73);
		fConst130 = (sqrt((19.0 * (fConst28 / double(tgamma(19.0))))) * double(tgamma(9.5)));
		fConst131 = (408.51689513106709 * fConst130);
		fConst132 = (0.56568542494923812 * fConst24);
		fConst133 = (0.12698727186848197 * (fConst107 * fConst106));
		fConst134 = (fConst40 * fConst20);
		fConst135 = (0.50794908747392786 * fConst134);
		fConst136 = (0.3809618156054459 * (fConst109 * fConst106));
		fConst137 = (0.25397454373696393 * fConst134);
		fConst138 = (0.50794908747392786 * (fConst92 * fConst91));
		fConst139 = (fConst41 * fConst20);
		fConst140 = (2.5397454373696391 * fConst139);
		fConst141 = (0.056269769759819135 * (fConst111 * fConst106));
		fConst142 = (1.2698727186848195 * fConst139);
		fConst143 = (2.0317963498957115 * fConst45);
		fConst144 = (3.5556436123174948 * (fConst94 * fConst91));
		fConst145 = (0.11253953951963827 * fConst48);
		fConst146 = (0.037513179839879424 * (fConst113 * fConst106));
		fConst147 = (0.056269769759819135 * fConst48);
		fConst148 = (1.0158981749478557 * fConst45);
		fConst149 = (2.0317963498957115 * (fConst81 * fConst80));
		fConst150 = (18.286167149061402 * fConst5);
		fConst151 = (0.056269769759819135 * (fConst96 * fConst91));
		fConst152 = (0.075026359679758847 * fConst52);
		fConst153 = (0.028134884879909568 * (fConst115 * fConst106));
		fConst154 = (0.037513179839879424 * fConst52);
		fConst155 = (9.1430835745307011 * fConst5);
		fConst156 = (4.0635926997914229 * fConst56);
		fConst157 = (22.349759848852827 * (fConst83 * fConst80));
		fConst158 = (0.11253953951963827 * fConst59);
		fConst159 = (0.037513179839879424 * (fConst98 * fConst91));
		fConst160 = (0.056269769759819135 * fConst61);
		fConst161 = (0.022507907903927659 * (fConst117 * fConst106));
		fConst162 = (0.028134884879909568 * fConst61);
		fConst163 = (0.056269769759819135 * fConst59);
		fConst164 = (8.1271853995828458 * fConst125);
		fConst165 = (52.826705097288496 * fConst10);
		fConst166 = (0.056269769759819135 * (fConst85 * fConst80));
		fConst167 = (0.075026359679758847 * fConst14);
		fConst168 = (0.028134884879909568 * (fConst100 * fConst91));
		fConst169 = (0.045015815807855318 * fConst66);
		fConst170 = (0.018756589919939712 * (fConst119 * fConst106));
		fConst171 = (0.022507907903927659 * fConst66);
		fConst172 = (0.037513179839879424 * fConst14);
		fConst173 = (32.508741598331383 * fConst75);
		fConst174 = (121.90778099374268 * (fConst33 * fConst27));
		fConst175 = (0.056269769759819135 * fConst68);
		fConst176 = (0.037513179839879424 * (fConst87 * fConst80));
		fConst177 = (0.056269769759819135 * fConst73);
		fConst178 = (0.022507907903927659 * (fConst102 * fConst91));
		fConst179 = (0.037513179839879424 * fConst70);
		fConst180 = (0.016077077074234038 * (fConst121 * fConst106));
		fConst181 = (0.018756589919939712 * fConst70);
		fConst182 = (0.028134884879909568 * fConst73);
		fConst183 = (16.254370799165692 * fConst75);
		fConst184 = (32.508741598331383 * fConst130);
		fConst185 = (552.64860717163344 * fConst17);
		fConst186 = (0.056269769759819135 * (fConst37 * fConst27));
		fConst187 = (0.037513179839879424 * fConst77);
		fConst188 = (0.028134884879909568 * (fConst89 * fConst80));
		fConst189 = (0.045015815807855318 * fConst24);
		fConst190 = (0.018756589919939712 * (fConst104 * fConst91));
		fConst191 = (0.032154154148468075 * fConst21);
		fConst192 = (0.014067442439954784 * (fConst123 * fConst106));
		fConst193 = (0.016077077074234038 * fConst21);
		fConst194 = (0.022507907903927659 * fConst24);
		fConst195 = (276.32430358581672 * fConst17);
		
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
		ui_interface->declare(&fVslider2, "4", "");
		ui_interface->declare(&fVslider2, "unit", "dB");
		ui_interface->addVerticalSlider("Gain", &fVslider2, 0.0, -20.0, 20.0, 0.10000000000000001);
		ui_interface->declare(&fVslider3, "5", "");
		ui_interface->addVerticalSlider("Azimuth", &fVslider3, 0.0, -3.1415926535897931, 3.1415926535897931, 0.10000000000000001);
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
		FAUSTFLOAT* input64 = inputs[64];
		FAUSTFLOAT* input65 = inputs[65];
		FAUSTFLOAT* input66 = inputs[66];
		FAUSTFLOAT* input67 = inputs[67];
		FAUSTFLOAT* input68 = inputs[68];
		FAUSTFLOAT* input69 = inputs[69];
		FAUSTFLOAT* input70 = inputs[70];
		FAUSTFLOAT* input71 = inputs[71];
		FAUSTFLOAT* input72 = inputs[72];
		FAUSTFLOAT* input73 = inputs[73];
		FAUSTFLOAT* input74 = inputs[74];
		FAUSTFLOAT* input75 = inputs[75];
		FAUSTFLOAT* input76 = inputs[76];
		FAUSTFLOAT* input77 = inputs[77];
		FAUSTFLOAT* input78 = inputs[78];
		FAUSTFLOAT* input79 = inputs[79];
		FAUSTFLOAT* input80 = inputs[80];
		FAUSTFLOAT* input81 = inputs[81];
		FAUSTFLOAT* input82 = inputs[82];
		FAUSTFLOAT* input83 = inputs[83];
		FAUSTFLOAT* input84 = inputs[84];
		FAUSTFLOAT* input85 = inputs[85];
		FAUSTFLOAT* input86 = inputs[86];
		FAUSTFLOAT* input87 = inputs[87];
		FAUSTFLOAT* input88 = inputs[88];
		FAUSTFLOAT* input89 = inputs[89];
		FAUSTFLOAT* input90 = inputs[90];
		FAUSTFLOAT* input91 = inputs[91];
		FAUSTFLOAT* input92 = inputs[92];
		FAUSTFLOAT* input93 = inputs[93];
		FAUSTFLOAT* input94 = inputs[94];
		FAUSTFLOAT* input95 = inputs[95];
		FAUSTFLOAT* input96 = inputs[96];
		FAUSTFLOAT* input97 = inputs[97];
		FAUSTFLOAT* input98 = inputs[98];
		FAUSTFLOAT* input99 = inputs[99];
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
		FAUSTFLOAT* output64 = outputs[64];
		FAUSTFLOAT* output65 = outputs[65];
		FAUSTFLOAT* output66 = outputs[66];
		FAUSTFLOAT* output67 = outputs[67];
		FAUSTFLOAT* output68 = outputs[68];
		FAUSTFLOAT* output69 = outputs[69];
		FAUSTFLOAT* output70 = outputs[70];
		FAUSTFLOAT* output71 = outputs[71];
		FAUSTFLOAT* output72 = outputs[72];
		FAUSTFLOAT* output73 = outputs[73];
		FAUSTFLOAT* output74 = outputs[74];
		FAUSTFLOAT* output75 = outputs[75];
		FAUSTFLOAT* output76 = outputs[76];
		FAUSTFLOAT* output77 = outputs[77];
		FAUSTFLOAT* output78 = outputs[78];
		FAUSTFLOAT* output79 = outputs[79];
		FAUSTFLOAT* output80 = outputs[80];
		FAUSTFLOAT* output81 = outputs[81];
		FAUSTFLOAT* output82 = outputs[82];
		FAUSTFLOAT* output83 = outputs[83];
		FAUSTFLOAT* output84 = outputs[84];
		FAUSTFLOAT* output85 = outputs[85];
		FAUSTFLOAT* output86 = outputs[86];
		FAUSTFLOAT* output87 = outputs[87];
		FAUSTFLOAT* output88 = outputs[88];
		FAUSTFLOAT* output89 = outputs[89];
		FAUSTFLOAT* output90 = outputs[90];
		FAUSTFLOAT* output91 = outputs[91];
		FAUSTFLOAT* output92 = outputs[92];
		FAUSTFLOAT* output93 = outputs[93];
		FAUSTFLOAT* output94 = outputs[94];
		FAUSTFLOAT* output95 = outputs[95];
		FAUSTFLOAT* output96 = outputs[96];
		FAUSTFLOAT* output97 = outputs[97];
		FAUSTFLOAT* output98 = outputs[98];
		FAUSTFLOAT* output99 = outputs[99];
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
		double fSlow11 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider2))));
		double fSlow12 = mydsp_faustpower2_f(fSlow10);
		double fSlow13 = ((3.0 * fSlow12) + -1.0);
		double fSlow14 = ((2.5 * fSlow13) + -2.0);
		double fSlow15 = (0.88191710368819687 * fSlow14);
		double fSlow16 = double(fVslider3);
		double fSlow17 = cos(fSlow16);
		double fSlow18 = mydsp_faustpower2_f(fSlow17);
		double fSlow19 = ((4.0 * fSlow18) + -1.0);
		double fSlow20 = (fSlow19 + -1.0);
		double fSlow21 = (fSlow20 * fSlow17);
		double fSlow22 = (1.0 - fSlow12);
		double fSlow23 = mydsp_faustpower2_f(fSlow22);
		double fSlow24 = sin(fSlow16);
		double fSlow25 = ((fSlow21 * fSlow23) * fSlow24);
		double fSlow26 = (fConst6 * fSlow25);
		double fSlow27 = ((2.333333333333333 * (fSlow14 * fSlow12)) - (1.5 * fSlow13));
		double fSlow28 = ((2.25 * fSlow27) - (1.3333333333333333 * fSlow14));
		double fSlow29 = (0.66332495807108005 * fSlow28);
		double fSlow30 = ((2.0 * fSlow18) + -1.0);
		double fSlow31 = ((2.0 * fSlow30) + -1.0);
		double fSlow32 = (1.0 - fSlow31);
		double fSlow33 = (1.0 - (2.0 * (fSlow32 * fSlow18)));
		double fSlow34 = (fSlow33 * fSlow23);
		double fSlow35 = (fConst7 * fSlow34);
		double fSlow36 = (1.0 - fSlow20);
		double fSlow37 = (1.0 - (4.0 * (fSlow36 * fSlow18)));
		double fSlow38 = (fSlow20 - fSlow37);
		double fSlow39 = (0.0 - (2.0 * (fSlow38 * fSlow17)));
		double fSlow40 = mydsp_faustpower3_f(fSlow22);
		double fSlow41 = ((fSlow39 * fSlow40) * fSlow24);
		double fSlow42 = (fConst11 * fSlow41);
		double fSlow43 = ((2.2000000000000002 * (fSlow28 * fSlow12)) - (1.25 * fSlow27));
		double fSlow44 = ((2.1666666666666665 * fSlow43) - (1.2000000000000002 * fSlow28));
		double fSlow45 = (0.55328333517248807 * fSlow44);
		double fSlow46 = ((893.67630033964599 * fSlow12) + -81.24330003087691);
		double fSlow47 = ((6.5 * fSlow46) + -812.43300030876912);
		double fSlow48 = (fSlow33 * fSlow47);
		double fSlow49 = (fConst15 * (fSlow48 * fSlow23));
		double fSlow50 = ((2.0 * fSlow39) + (4.0 * (fSlow36 * fSlow17)));
		double fSlow51 = ((fSlow50 * fSlow17) + -1.0);
		double fSlow52 = ((fSlow38 + fSlow51) * fSlow17);
		double fSlow53 = mydsp_faustpower4_f(fSlow22);
		double fSlow54 = (fSlow52 * fSlow53);
		double fSlow55 = (fSlow54 * fSlow24);
		double fSlow56 = (fConst18 * fSlow55);
		double fSlow57 = ((78.986541696685876 * fSlow12) + -11.283791670955125);
		double fSlow58 = ((4.5 * fSlow57) + -67.702750025730751);
		double fSlow59 = ((3.6666666666666665 * (fSlow58 * fSlow12)) - (3.5 * fSlow57));
		double fSlow60 = ((3.25 * fSlow59) - (2.6666666666666665 * fSlow58));
		double fSlow61 = ((3.0 * (fSlow60 * fSlow12)) - (2.25 * fSlow59));
		double fSlow62 = ((2.833333333333333 * fSlow61) - (2.0 * fSlow60));
		double fSlow63 = (fSlow62 * fSlow17);
		double fSlow64 = (fConst22 * ((fSlow63 * fSlow22) * fSlow24));
		double fSlow65 = ((2.1428571428571428 * (fSlow44 * fSlow12)) - (1.1666666666666665 * fSlow43));
		double fSlow66 = ((2.125 * fSlow65) - (1.1428571428571428 * fSlow44));
		double fSlow67 = (0.48432210483785265 * fSlow66);
		double fSlow68 = ((5.0 * (fSlow47 * fSlow12)) - (5.5 * fSlow46));
		double fSlow69 = ((4.25 * fSlow68) - (4.0 * fSlow47));
		double fSlow70 = (fSlow33 * fSlow69);
		double fSlow71 = (fConst25 * (fSlow70 * fSlow23));
		double fSlow72 = ((2.0 * fSlow33) - fSlow31);
		double fSlow73 = (fSlow32 + fSlow72);
		double fSlow74 = ((2.0 * (fSlow73 * fSlow18)) + -1.0);
		double fSlow75 = ((2.0 * fSlow74) - fSlow72);
		double fSlow76 = (1.0 - (2.0 * ((fSlow73 - fSlow75) * fSlow18)));
		double fSlow77 = (fSlow76 * fSlow53);
		double fSlow78 = (fConst26 * fSlow77);
		double fSlow79 = pow(fSlow22, 3.5);
		double fSlow80 = (fConst27 * fSlow79);
		double fSlow81 = (fSlow75 * fSlow17);
		double fSlow82 = (fConst31 * fSlow81);
		double fSlow83 = (fSlow51 * fSlow10);
		double fSlow84 = (fConst34 * (fSlow83 * fSlow24));
		double fSlow85 = (fSlow81 * fSlow10);
		double fSlow86 = (fConst34 * fSlow85);
		double fSlow87 = ((18415.148006998763 * fSlow12) + -1083.244000411692);
		double fSlow88 = (fSlow51 * fSlow87);
		double fSlow89 = (fConst38 * (fSlow88 * fSlow24));
		double fSlow90 = ((fSlow75 * fSlow87) * fSlow17);
		double fSlow91 = (fConst38 * fSlow90);
		double fSlow92 = (6.3830764864229232 * (fSlow17 * fSlow24));
		double fSlow93 = (3.1915382432114616 * fSlow30);
		double fSlow94 = (fSlow17 * fSlow10);
		double fSlow95 = (fConst42 * (fSlow94 * fSlow24));
		double fSlow96 = (fSlow30 * fSlow10);
		double fSlow97 = (fConst43 * fSlow96);
		double fSlow98 = (fConst46 * ((fSlow21 * fSlow22) * fSlow24));
		double fSlow99 = (fSlow57 * fSlow17);
		double fSlow100 = (fConst49 * (fSlow99 * fSlow24));
		double fSlow101 = (fSlow30 * fSlow57);
		double fSlow102 = (fConst50 * fSlow101);
		double fSlow103 = (fConst51 * (fSlow33 * fSlow22));
		double fSlow104 = ((fSlow58 * fSlow17) * fSlow10);
		double fSlow105 = (fConst53 * (fSlow104 * fSlow24));
		double fSlow106 = ((fSlow30 * fSlow58) * fSlow10);
		double fSlow107 = (fConst54 * fSlow106);
		double fSlow108 = (fConst57 * ((fSlow39 * fSlow23) * fSlow24));
		double fSlow109 = ((fSlow20 * fSlow46) * fSlow17);
		double fSlow110 = (fConst60 * ((fSlow109 * fSlow22) * fSlow24));
		double fSlow111 = (fSlow59 * fSlow17);
		double fSlow112 = (fConst62 * (fSlow111 * fSlow24));
		double fSlow113 = (fSlow30 * fSlow59);
		double fSlow114 = (fConst63 * fSlow113);
		double fSlow115 = (fSlow33 * fSlow46);
		double fSlow116 = (fConst64 * (fSlow115 * fSlow22));
		double fSlow117 = (fConst57 * (fSlow74 * fSlow23));
		double fSlow118 = (((fSlow20 * fSlow47) * fSlow17) * fSlow10);
		double fSlow119 = (fConst65 * ((fSlow118 * fSlow22) * fSlow24));
		double fSlow120 = ((fSlow30 * fSlow60) * fSlow10);
		double fSlow121 = (fConst67 * fSlow120);
		double fSlow122 = (fSlow74 * fSlow10);
		double fSlow123 = (fConst11 * (fSlow122 * fSlow23));
		double fSlow124 = ((7041.0860026759983 * fSlow12) + -469.40573351173322);
		double fSlow125 = (fSlow39 * fSlow124);
		double fSlow126 = (fConst69 * ((fSlow125 * fSlow23) * fSlow24));
		double fSlow127 = (fSlow61 * fSlow17);
		double fSlow128 = (fConst71 * (fSlow127 * fSlow24));
		double fSlow129 = (fSlow30 * fSlow61);
		double fSlow130 = (fConst72 * fSlow129);
		double fSlow131 = (fSlow33 * fSlow68);
		double fSlow132 = (fConst74 * (fSlow131 * fSlow22));
		double fSlow133 = (fSlow74 * fSlow124);
		double fSlow134 = (fConst69 * (fSlow133 * fSlow23));
		double fSlow135 = (fConst76 * (fSlow76 * fSlow40));
		double fSlow136 = ((8.5 * fSlow124) + -6571.6802691642652);
		double fSlow137 = ((fSlow39 * fSlow136) * fSlow10);
		double fSlow138 = (fConst78 * ((fSlow137 * fSlow23) * fSlow24));
		double fSlow139 = ((fSlow30 * fSlow62) * fSlow10);
		double fSlow140 = (fConst79 * fSlow139);
		double fSlow141 = ((fSlow74 * fSlow136) * fSlow10);
		double fSlow142 = (fConst78 * (fSlow141 * fSlow23));
		double fSlow143 = pow(fSlow22, 2.5);
		double fSlow144 = (fConst80 * fSlow143);
		double fSlow145 = (fSlow37 * fSlow24);
		double fSlow146 = (fSlow72 * fSlow17);
		double fSlow147 = (fSlow37 * fSlow10);
		double fSlow148 = (fConst84 * (fSlow147 * fSlow24));
		double fSlow149 = (fSlow146 * fSlow10);
		double fSlow150 = (fConst84 * fSlow149);
		double fSlow151 = ((2581.7315343145328 * fSlow12) + -198.59473340881021);
		double fSlow152 = (fSlow37 * fSlow151);
		double fSlow153 = (fConst86 * (fSlow152 * fSlow24));
		double fSlow154 = ((fSlow72 * fSlow151) * fSlow17);
		double fSlow155 = (fConst86 * fSlow154);
		double fSlow156 = ((7.5 * fSlow151) + -2383.1368009057223);
		double fSlow157 = ((fSlow37 * fSlow156) * fSlow10);
		double fSlow158 = (fConst88 * (fSlow157 * fSlow24));
		double fSlow159 = (((fSlow72 * fSlow156) * fSlow17) * fSlow10);
		double fSlow160 = (fConst88 * fSlow159);
		double fSlow161 = ((5.6666666666666661 * (fSlow156 * fSlow12)) - (6.5 * fSlow151));
		double fSlow162 = (fSlow37 * fSlow161);
		double fSlow163 = (fConst90 * (fSlow162 * fSlow24));
		double fSlow164 = ((fSlow72 * fSlow161) * fSlow17);
		double fSlow165 = (fConst90 * fSlow164);
		double fSlow166 = pow(fSlow22, 1.5);
		double fSlow167 = (fConst91 * fSlow166);
		double fSlow168 = (fSlow19 * fSlow24);
		double fSlow169 = (fSlow31 * fSlow17);
		double fSlow170 = (fSlow19 * fSlow10);
		double fSlow171 = (fConst95 * (fSlow170 * fSlow24));
		double fSlow172 = (fSlow169 * fSlow10);
		double fSlow173 = (fConst95 * fSlow172);
		double fSlow174 = ((284.35155010806915 * fSlow12) + -31.594616678674353);
		double fSlow175 = (fSlow19 * fSlow174);
		double fSlow176 = (fConst97 * (fSlow175 * fSlow24));
		double fSlow177 = ((fSlow31 * fSlow174) * fSlow17);
		double fSlow178 = (fConst97 * fSlow177);
		double fSlow179 = ((5.5 * fSlow174) + -252.75693342939482);
		double fSlow180 = ((fSlow19 * fSlow179) * fSlow10);
		double fSlow181 = (fConst99 * (fSlow180 * fSlow24));
		double fSlow182 = (((fSlow31 * fSlow179) * fSlow17) * fSlow10);
		double fSlow183 = (fConst99 * fSlow182);
		double fSlow184 = ((4.333333333333333 * (fSlow179 * fSlow12)) - (4.5 * fSlow174));
		double fSlow185 = (fSlow19 * fSlow184);
		double fSlow186 = (fConst101 * (fSlow185 * fSlow24));
		double fSlow187 = ((fSlow31 * fSlow184) * fSlow17);
		double fSlow188 = (fConst101 * fSlow187);
		double fSlow189 = ((3.75 * fSlow184) - (3.333333333333333 * fSlow179));
		double fSlow190 = ((fSlow19 * fSlow189) * fSlow10);
		double fSlow191 = (fConst103 * (fSlow190 * fSlow24));
		double fSlow192 = (((fSlow31 * fSlow189) * fSlow17) * fSlow10);
		double fSlow193 = (fConst103 * fSlow192);
		double fSlow194 = ((3.4000000000000004 * (fSlow189 * fSlow12)) - (2.75 * fSlow184));
		double fSlow195 = (fSlow19 * fSlow194);
		double fSlow196 = (fConst105 * (fSlow195 * fSlow24));
		double fSlow197 = ((fSlow31 * fSlow194) * fSlow17);
		double fSlow198 = (fConst105 * fSlow197);
		double fSlow199 = pow(fSlow22, 0.5);
		double fSlow200 = (fConst106 * fSlow199);
		double fSlow201 = (fConst110 * (fSlow10 * fSlow24));
		double fSlow202 = (fConst110 * fSlow94);
		double fSlow203 = ((16.925687506432688 * fSlow12) + -3.3851375012865379);
		double fSlow204 = (fConst112 * (fSlow203 * fSlow24));
		double fSlow205 = (fSlow203 * fSlow17);
		double fSlow206 = (fConst112 * fSlow205);
		double fSlow207 = ((3.5 * fSlow203) + -13.540550005146152);
		double fSlow208 = (fSlow207 * fSlow10);
		double fSlow209 = (fConst114 * (fSlow208 * fSlow24));
		double fSlow210 = ((fSlow207 * fSlow17) * fSlow10);
		double fSlow211 = (fConst114 * fSlow210);
		double fSlow212 = ((3.0 * (fSlow207 * fSlow12)) - (2.5 * fSlow203));
		double fSlow213 = (fConst116 * (fSlow212 * fSlow24));
		double fSlow214 = (fSlow212 * fSlow17);
		double fSlow215 = (fConst116 * fSlow214);
		double fSlow216 = ((2.75 * fSlow212) - (2.0 * fSlow207));
		double fSlow217 = (fSlow216 * fSlow10);
		double fSlow218 = (fConst118 * (fSlow217 * fSlow24));
		double fSlow219 = ((fSlow216 * fSlow17) * fSlow10);
		double fSlow220 = (fConst118 * fSlow219);
		double fSlow221 = ((2.6000000000000001 * (fSlow216 * fSlow12)) - (1.75 * fSlow212));
		double fSlow222 = (fConst120 * (fSlow221 * fSlow24));
		double fSlow223 = (fSlow221 * fSlow17);
		double fSlow224 = (fConst120 * fSlow223);
		double fSlow225 = ((2.5 * fSlow221) - (1.6000000000000001 * fSlow216));
		double fSlow226 = (fSlow225 * fSlow10);
		double fSlow227 = (fConst122 * (fSlow226 * fSlow24));
		double fSlow228 = ((fSlow225 * fSlow17) * fSlow10);
		double fSlow229 = (fConst122 * fSlow228);
		double fSlow230 = ((2.4285714285714284 * (fSlow225 * fSlow12)) - (1.5 * fSlow221));
		double fSlow231 = (fConst124 * (fSlow230 * fSlow24));
		double fSlow232 = (fSlow230 * fSlow17);
		double fSlow233 = (fConst124 * fSlow232);
		double fSlow234 = (1.1180339887498949 * fSlow13);
		double fSlow235 = (0.75 * fSlow27);
		double fSlow236 = (0.60092521257733145 * fSlow43);
		double fSlow237 = (0.51538820320220757 * fSlow65);
		double fSlow238 = (fSlow51 * fSlow79);
		double fSlow239 = (fConst126 * fSlow238);
		double fSlow240 = (((fSlow60 * fSlow17) * fSlow10) * fSlow22);
		double fSlow241 = (fConst127 * fSlow240);
		double fSlow242 = (fConst128 * fSlow54);
		double fSlow243 = (((fSlow20 * fSlow68) * fSlow17) * fSlow23);
		double fSlow244 = (fConst129 * fSlow243);
		double fSlow245 = pow(fSlow22, 4.5);
		double fSlow246 = (((((4.0 * fSlow52) - fSlow50) * fSlow17) + 1.0) * fSlow245);
		double fSlow247 = (fConst131 * fSlow246);
		double fSlow248 = ((((fSlow20 * fSlow69) * fSlow17) * fSlow10) * fSlow23);
		double fSlow249 = (fConst132 * fSlow248);
		double fSlow250 = ((((2.0 * fSlow76) - fSlow75) * fSlow17) * fSlow245);
		double fSlow251 = (fConst131 * fSlow250);
		double fSlow252 = (fConst133 * (fSlow199 * fSlow24));
		double fSlow253 = (0.13783222385544802 * fSlow10);
		double fSlow254 = (fConst133 * (fSlow17 * fSlow199));
		double fSlow255 = (fConst135 * ((fSlow17 * fSlow22) * fSlow24));
		double fSlow256 = (fConst136 * ((fSlow10 * fSlow199) * fSlow24));
		double fSlow257 = (0.088970317927147144 * fSlow13);
		double fSlow258 = (fConst136 * (fSlow94 * fSlow199));
		double fSlow259 = (fConst137 * (fSlow30 * fSlow22));
		double fSlow260 = (fConst138 * ((fSlow19 * fSlow166) * fSlow24));
		double fSlow261 = (fConst140 * ((fSlow94 * fSlow22) * fSlow24));
		double fSlow262 = (fConst141 * ((fSlow203 * fSlow199) * fSlow24));
		double fSlow263 = (0.070180733224632072 * (fSlow14 * fSlow10));
		double fSlow264 = (fConst141 * (fSlow205 * fSlow199));
		double fSlow265 = (fConst142 * (fSlow96 * fSlow22));
		double fSlow266 = (fConst138 * (fSlow169 * fSlow166));
		double fSlow267 = (fConst143 * fSlow25);
		double fSlow268 = (fConst144 * ((fSlow170 * fSlow166) * fSlow24));
		double fSlow269 = (fConst145 * ((fSlow99 * fSlow22) * fSlow24));
		double fSlow270 = (fConst146 * ((fSlow208 * fSlow199) * fSlow24));
		double fSlow271 = (0.059683103659460751 * fSlow27);
		double fSlow272 = (fConst146 * (fSlow210 * fSlow199));
		double fSlow273 = (fConst147 * (fSlow101 * fSlow22));
		double fSlow274 = (fConst144 * (fSlow172 * fSlow166));
		double fSlow275 = (fConst148 * fSlow34);
		double fSlow276 = (fConst149 * ((fSlow37 * fSlow143) * fSlow24));
		double fSlow277 = (fConst150 * (((fSlow21 * fSlow10) * fSlow23) * fSlow24));
		double fSlow278 = (fConst151 * ((fSlow175 * fSlow166) * fSlow24));
		double fSlow279 = (fConst152 * ((fSlow104 * fSlow22) * fSlow24));
		double fSlow280 = (fConst153 * ((fSlow212 * fSlow199) * fSlow24));
		double fSlow281 = (0.052785722976618307 * (fSlow28 * fSlow10));
		double fSlow282 = (fConst153 * (fSlow214 * fSlow199));
		double fSlow283 = (fConst154 * (fSlow106 * fSlow22));
		double fSlow284 = (fConst151 * (fSlow177 * fSlow166));
		double fSlow285 = (fConst155 * ((fSlow33 * fSlow10) * fSlow23));
		double fSlow286 = (fConst149 * (fSlow146 * fSlow143));
		double fSlow287 = (fConst156 * fSlow41);
		double fSlow288 = (fConst157 * ((fSlow147 * fSlow143) * fSlow24));
		double fSlow289 = (fConst158 * ((fSlow109 * fSlow23) * fSlow24));
		double fSlow290 = (fConst159 * ((fSlow180 * fSlow166) * fSlow24));
		double fSlow291 = (fConst160 * ((fSlow111 * fSlow22) * fSlow24));
		double fSlow292 = (fConst161 * ((fSlow217 * fSlow199) * fSlow24));
		double fSlow293 = (0.047820109005115151 * fSlow43);
		double fSlow294 = (fConst161 * (fSlow219 * fSlow199));
		double fSlow295 = (fConst162 * (fSlow113 * fSlow22));
		double fSlow296 = (fConst159 * (fSlow182 * fSlow166));
		double fSlow297 = (fConst163 * (fSlow115 * fSlow23));
		double fSlow298 = (fConst157 * (fSlow149 * fSlow143));
		double fSlow299 = (fConst156 * (fSlow74 * fSlow40));
		double fSlow300 = (fConst164 * (fSlow238 * fSlow24));
		double fSlow301 = (fConst165 * (((fSlow39 * fSlow10) * fSlow40) * fSlow24));
		double fSlow302 = (fConst166 * ((fSlow152 * fSlow143) * fSlow24));
		double fSlow303 = (fConst167 * ((fSlow118 * fSlow23) * fSlow24));
		double fSlow304 = (fConst168 * ((fSlow185 * fSlow166) * fSlow24));
		double fSlow305 = (fConst169 * (fSlow240 * fSlow24));
		double fSlow306 = (fConst170 * ((fSlow221 * fSlow199) * fSlow24));
		double fSlow307 = (0.044028888861535699 * (fSlow44 * fSlow10));
		double fSlow308 = (fConst170 * (fSlow223 * fSlow199));
		double fSlow309 = (fConst171 * (fSlow120 * fSlow22));
		double fSlow310 = (fConst168 * (fSlow187 * fSlow166));
		double fSlow311 = (fConst172 * ((fSlow48 * fSlow10) * fSlow23));
		double fSlow312 = (fConst166 * (fSlow154 * fSlow143));
		double fSlow313 = (fConst165 * (fSlow122 * fSlow40));
		double fSlow314 = (fConst164 * (fSlow81 * fSlow79));
		double fSlow315 = (fConst173 * fSlow55);
		double fSlow316 = (fConst174 * ((fSlow83 * fSlow79) * fSlow24));
		double fSlow317 = (fConst175 * ((fSlow125 * fSlow40) * fSlow24));
		double fSlow318 = (fConst176 * ((fSlow157 * fSlow143) * fSlow24));
		double fSlow319 = (fConst177 * (fSlow243 * fSlow24));
		double fSlow320 = (fConst178 * ((fSlow190 * fSlow166) * fSlow24));
		double fSlow321 = (fConst179 * ((fSlow127 * fSlow22) * fSlow24));
		double fSlow322 = (fConst180 * ((fSlow226 * fSlow199) * fSlow24));
		double fSlow323 = (0.041013290075440773 * fSlow65);
		double fSlow324 = (fConst180 * (fSlow228 * fSlow199));
		double fSlow325 = (fConst181 * (fSlow129 * fSlow22));
		double fSlow326 = (fConst178 * (fSlow192 * fSlow166));
		double fSlow327 = (fConst182 * (fSlow131 * fSlow23));
		double fSlow328 = (fConst176 * (fSlow159 * fSlow143));
		double fSlow329 = (fConst175 * (fSlow133 * fSlow40));
		double fSlow330 = (fConst174 * (fSlow85 * fSlow79));
		double fSlow331 = (fConst183 * fSlow77);
		double fSlow332 = (fConst184 * (fSlow246 * fSlow24));
		double fSlow333 = (fConst185 * (((fSlow52 * fSlow10) * fSlow53) * fSlow24));
		double fSlow334 = (fConst186 * ((fSlow88 * fSlow79) * fSlow24));
		double fSlow335 = (fConst187 * ((fSlow137 * fSlow40) * fSlow24));
		double fSlow336 = (fConst188 * ((fSlow162 * fSlow143) * fSlow24));
		double fSlow337 = (fConst189 * (fSlow248 * fSlow24));
		double fSlow338 = (fConst190 * ((fSlow195 * fSlow166) * fSlow24));
		double fSlow339 = (fConst191 * (((fSlow63 * fSlow10) * fSlow22) * fSlow24));
		double fSlow340 = (fConst192 * ((fSlow230 * fSlow199) * fSlow24));
		double fSlow341 = (0.038541128516807702 * (fSlow66 * fSlow10));
		double fSlow342 = (fConst192 * (fSlow232 * fSlow199));
		double fSlow343 = (fConst193 * (fSlow139 * fSlow22));
		double fSlow344 = (fConst190 * (fSlow197 * fSlow166));
		double fSlow345 = (fConst194 * ((fSlow70 * fSlow10) * fSlow23));
		double fSlow346 = (fConst188 * (fSlow164 * fSlow143));
		double fSlow347 = (fConst187 * (fSlow141 * fSlow40));
		double fSlow348 = (fConst186 * (fSlow90 * fSlow79));
		double fSlow349 = (fConst195 * ((fSlow76 * fSlow10) * fSlow53));
		double fSlow350 = (fConst184 * fSlow250);
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec0[0] = (iSlow5?0.0:min(fSlow6, (fRec0[1] + 1.0)));
			double fTemp0 = double(input0[i]);
			fRec1[0] = (iSlow7?fSlow6:max(0.0, (fRec1[1] + -1.0)));
			fRec2[0] = (fSlow11 + (0.999 * fRec2[1]));
			fRec3[0] = (iSlow5?fSlow6:max(0.0, (fRec3[1] + -1.0)));
			double fTemp1 = (fRec2[0] * fRec3[0]);
			double fTemp2 = double(input2[i]);
			fRec4[0] = (iSlow7?0.0:min(fSlow6, (fRec4[1] + 1.0)));
			double fTemp3 = (fRec2[0] * fRec4[0]);
			double fTemp4 = double(input12[i]);
			double fTemp5 = double(input26[i]);
			double fTemp6 = double(input30[i]);
			double fTemp7 = double(input34[i]);
			double fTemp8 = double(input50[i]);
			double fTemp9 = double(input56[i]);
			double fTemp10 = double(input60[i]);
			double fTemp11 = double(input82[i]);
			double fTemp12 = double(input88[i]);
			double fTemp13 = double(input90[i]);
			double fTemp14 = double(input94[i]);
			double fTemp15 = double(input98[i]);
			double fTemp16 = double(input63[i]);
			double fTemp17 = double(input65[i]);
			double fTemp18 = double(input79[i]);
			double fTemp19 = double(input83[i]);
			double fTemp20 = double(input97[i]);
			double fTemp21 = double(input4[i]);
			double fTemp22 = double(input8[i]);
			double fTemp23 = double(input10[i]);
			double fTemp24 = double(input14[i]);
			double fTemp25 = double(input16[i]);
			double fTemp26 = double(input18[i]);
			double fTemp27 = double(input22[i]);
			double fTemp28 = double(input24[i]);
			double fTemp29 = double(input28[i]);
			double fTemp30 = double(input32[i]);
			double fTemp31 = double(input36[i]);
			double fTemp32 = double(input38[i]);
			double fTemp33 = double(input40[i]);
			double fTemp34 = double(input44[i]);
			double fTemp35 = double(input46[i]);
			double fTemp36 = double(input48[i]);
			double fTemp37 = double(input52[i]);
			double fTemp38 = double(input58[i]);
			double fTemp39 = double(input62[i]);
			double fTemp40 = double(input66[i]);
			double fTemp41 = double(input70[i]);
			double fTemp42 = double(input74[i]);
			double fTemp43 = double(input76[i]);
			double fTemp44 = double(input78[i]);
			double fTemp45 = double(input80[i]);
			double fTemp46 = double(input84[i]);
			double fTemp47 = double(input92[i]);
			double fTemp48 = double(input96[i]);
			double fTemp49 = double(input25[i]);
			double fTemp50 = double(input35[i]);
			double fTemp51 = double(input37[i]);
			double fTemp52 = double(input47[i]);
			double fTemp53 = double(input51[i]);
			double fTemp54 = double(input61[i]);
			double fTemp55 = double(input67[i]);
			double fTemp56 = double(input77[i]);
			double fTemp57 = double(input85[i]);
			double fTemp58 = double(input95[i]);
			double fTemp59 = double(input9[i]);
			double fTemp60 = double(input15[i]);
			double fTemp61 = double(input17[i]);
			double fTemp62 = double(input23[i]);
			double fTemp63 = double(input27[i]);
			double fTemp64 = double(input33[i]);
			double fTemp65 = double(input39[i]);
			double fTemp66 = double(input45[i]);
			double fTemp67 = double(input53[i]);
			double fTemp68 = double(input59[i]);
			double fTemp69 = double(input69[i]);
			double fTemp70 = double(input75[i]);
			double fTemp71 = double(input87[i]);
			double fTemp72 = double(input93[i]);
			double fTemp73 = double(input1[i]);
			double fTemp74 = double(input3[i]);
			double fTemp75 = double(input5[i]);
			double fTemp76 = double(input7[i]);
			double fTemp77 = double(input11[i]);
			double fTemp78 = double(input13[i]);
			double fTemp79 = double(input19[i]);
			double fTemp80 = double(input21[i]);
			double fTemp81 = double(input29[i]);
			double fTemp82 = double(input31[i]);
			double fTemp83 = double(input41[i]);
			double fTemp84 = double(input43[i]);
			double fTemp85 = double(input55[i]);
			double fTemp86 = double(input57[i]);
			double fTemp87 = double(input71[i]);
			double fTemp88 = double(input73[i]);
			double fTemp89 = double(input89[i]);
			double fTemp90 = double(input91[i]);
			double fTemp91 = double(input6[i]);
			double fTemp92 = double(input20[i]);
			double fTemp93 = double(input42[i]);
			double fTemp94 = double(input72[i]);
			double fTemp95 = double(input49[i]);
			double fTemp96 = double(input54[i]);
			double fTemp97 = double(input64[i]);
			double fTemp98 = double(input68[i]);
			double fTemp99 = double(input81[i]);
			double fTemp100 = double(input86[i]);
			double fTemp101 = double(input99[i]);
			double fTemp102 = (((fSlow10 * (((((((((((((1.7320508075688772 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp2)):(iSlow2?(fSlow4 * (fTemp3 * fTemp2)):(fSlow4 * (fTemp1 * fTemp2))))) + (fSlow15 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp4)):(iSlow2?(fSlow4 * (fTemp3 * fTemp4)):(fSlow4 * (fTemp1 * fTemp4)))))) + (fSlow26 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp5)):(iSlow2?(fSlow4 * (fTemp3 * fTemp5)):(fSlow4 * (fTemp1 * fTemp5)))))) + (fSlow29 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp6)):(iSlow2?(fSlow4 * (fTemp3 * fTemp6)):(fSlow4 * (fTemp1 * fTemp6)))))) + (fSlow35 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp7)):(iSlow2?(fSlow4 * (fTemp3 * fTemp7)):(fSlow4 * (fTemp1 * fTemp7)))))) + (fSlow42 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp8)):(iSlow2?(fSlow4 * (fTemp3 * fTemp8)):(fSlow4 * (fTemp1 * fTemp8)))))) + (fSlow45 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp9)):(iSlow2?(fSlow4 * (fTemp3 * fTemp9)):(fSlow4 * (fTemp1 * fTemp9)))))) + (fSlow49 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp10)):(iSlow2?(fSlow4 * (fTemp3 * fTemp10)):(fSlow4 * (fTemp1 * fTemp10)))))) + (fSlow56 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp11)):(iSlow2?(fSlow4 * (fTemp3 * fTemp11)):(fSlow4 * (fTemp1 * fTemp11)))))) + (fSlow64 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp12)):(iSlow2?(fSlow4 * (fTemp3 * fTemp12)):(fSlow4 * (fTemp1 * fTemp12)))))) + (fSlow67 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp13)):(iSlow2?(fSlow4 * (fTemp3 * fTemp13)):(fSlow4 * (fTemp1 * fTemp13)))))) + (fSlow71 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp14)):(iSlow2?(fSlow4 * (fTemp3 * fTemp14)):(fSlow4 * (fTemp1 * fTemp14)))))) + (fSlow78 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp15)):(iSlow2?(fSlow4 * (fTemp3 * fTemp15)):(fSlow4 * (fTemp1 * fTemp15))))))) + ((fSlow80 * (((((fSlow82 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp16)):(iSlow2?(fSlow4 * (fTemp3 * fTemp16)):(fSlow4 * (fTemp1 * fTemp16))))) + (fSlow84 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp17)):(iSlow2?(fSlow4 * (fTemp3 * fTemp17)):(fSlow4 * (fTemp1 * fTemp17)))))) + (fSlow86 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp18)):(iSlow2?(fSlow4 * (fTemp3 * fTemp18)):(fSlow4 * (fTemp1 * fTemp18)))))) + (fSlow89 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp19)):(iSlow2?(fSlow4 * (fTemp3 * fTemp19)):(fSlow4 * (fTemp1 * fTemp19)))))) + (fSlow91 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp20)):(iSlow2?(fSlow4 * (fTemp3 * fTemp20)):(fSlow4 * (fTemp1 * fTemp20))))))) + ((fSlow22 * (((((((((((((((((((((((((fConst20 * (((fConst40 * ((fSlow92 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp21)):(iSlow2?(fSlow4 * (fTemp3 * fTemp21)):(fSlow4 * (fTemp1 * fTemp21))))) + (fSlow93 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp22)):(iSlow2?(fSlow4 * (fTemp3 * fTemp22)):(fSlow4 * (fTemp1 * fTemp22))))))) + (fSlow95 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp23)):(iSlow2?(fSlow4 * (fTemp3 * fTemp23)):(fSlow4 * (fTemp1 * fTemp23)))))) + (fSlow97 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp24)):(iSlow2?(fSlow4 * (fTemp3 * fTemp24)):(fSlow4 * (fTemp1 * fTemp24))))))) + (fSlow98 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp25)):(iSlow2?(fSlow4 * (fTemp3 * fTemp25)):(fSlow4 * (fTemp1 * fTemp25)))))) + (fSlow100 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp26)):(iSlow2?(fSlow4 * (fTemp3 * fTemp26)):(fSlow4 * (fTemp1 * fTemp26)))))) + (fSlow102 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp27)):(iSlow2?(fSlow4 * (fTemp3 * fTemp27)):(fSlow4 * (fTemp1 * fTemp27)))))) + (fSlow103 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp28)):(iSlow2?(fSlow4 * (fTemp3 * fTemp28)):(fSlow4 * (fTemp1 * fTemp28)))))) + (fSlow105 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp29)):(iSlow2?(fSlow4 * (fTemp3 * fTemp29)):(fSlow4 * (fTemp1 * fTemp29)))))) + (fSlow107 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp30)):(iSlow2?(fSlow4 * (fTemp3 * fTemp30)):(fSlow4 * (fTemp1 * fTemp30)))))) + (fSlow108 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp31)):(iSlow2?(fSlow4 * (fTemp3 * fTemp31)):(fSlow4 * (fTemp1 * fTemp31)))))) + (fSlow110 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp32)):(iSlow2?(fSlow4 * (fTemp3 * fTemp32)):(fSlow4 * (fTemp1 * fTemp32)))))) + (fSlow112 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp33)):(iSlow2?(fSlow4 * (fTemp3 * fTemp33)):(fSlow4 * (fTemp1 * fTemp33)))))) + (fSlow114 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp34)):(iSlow2?(fSlow4 * (fTemp3 * fTemp34)):(fSlow4 * (fTemp1 * fTemp34)))))) + (fSlow116 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp35)):(iSlow2?(fSlow4 * (fTemp3 * fTemp35)):(fSlow4 * (fTemp1 * fTemp35)))))) + (fSlow117 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp36)):(iSlow2?(fSlow4 * (fTemp3 * fTemp36)):(fSlow4 * (fTemp1 * fTemp36)))))) + (fSlow119 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp37)):(iSlow2?(fSlow4 * (fTemp3 * fTemp37)):(fSlow4 * (fTemp1 * fTemp37)))))) + (fSlow121 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp38)):(iSlow2?(fSlow4 * (fTemp3 * fTemp38)):(fSlow4 * (fTemp1 * fTemp38)))))) + (fSlow123 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp39)):(iSlow2?(fSlow4 * (fTemp3 * fTemp39)):(fSlow4 * (fTemp1 * fTemp39)))))) + (fSlow126 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp40)):(iSlow2?(fSlow4 * (fTemp3 * fTemp40)):(fSlow4 * (fTemp1 * fTemp40)))))) + (fSlow128 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp41)):(iSlow2?(fSlow4 * (fTemp3 * fTemp41)):(fSlow4 * (fTemp1 * fTemp41)))))) + (fSlow130 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp42)):(iSlow2?(fSlow4 * (fTemp3 * fTemp42)):(fSlow4 * (fTemp1 * fTemp42)))))) + (fSlow132 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp43)):(iSlow2?(fSlow4 * (fTemp3 * fTemp43)):(fSlow4 * (fTemp1 * fTemp43)))))) + (fSlow134 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp44)):(iSlow2?(fSlow4 * (fTemp3 * fTemp44)):(fSlow4 * (fTemp1 * fTemp44)))))) + (fSlow135 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp45)):(iSlow2?(fSlow4 * (fTemp3 * fTemp45)):(fSlow4 * (fTemp1 * fTemp45)))))) + (fSlow138 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp46)):(iSlow2?(fSlow4 * (fTemp3 * fTemp46)):(fSlow4 * (fTemp1 * fTemp46)))))) + (fSlow140 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp47)):(iSlow2?(fSlow4 * (fTemp3 * fTemp47)):(fSlow4 * (fTemp1 * fTemp47)))))) + (fSlow142 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp48)):(iSlow2?(fSlow4 * (fTemp3 * fTemp48)):(fSlow4 * (fTemp1 * fTemp48))))))) + ((fSlow144 * (((((((((fConst82 * ((fSlow145 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp49)):(iSlow2?(fSlow4 * (fTemp3 * fTemp49)):(fSlow4 * (fTemp1 * fTemp49))))) + (fSlow146 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp50)):(iSlow2?(fSlow4 * (fTemp3 * fTemp50)):(fSlow4 * (fTemp1 * fTemp50))))))) + (fSlow148 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp51)):(iSlow2?(fSlow4 * (fTemp3 * fTemp51)):(fSlow4 * (fTemp1 * fTemp51)))))) + (fSlow150 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp52)):(iSlow2?(fSlow4 * (fTemp3 * fTemp52)):(fSlow4 * (fTemp1 * fTemp52)))))) + (fSlow153 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp53)):(iSlow2?(fSlow4 * (fTemp3 * fTemp53)):(fSlow4 * (fTemp1 * fTemp53)))))) + (fSlow155 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp54)):(iSlow2?(fSlow4 * (fTemp3 * fTemp54)):(fSlow4 * (fTemp1 * fTemp54)))))) + (fSlow158 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp55)):(iSlow2?(fSlow4 * (fTemp3 * fTemp55)):(fSlow4 * (fTemp1 * fTemp55)))))) + (fSlow160 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp56)):(iSlow2?(fSlow4 * (fTemp3 * fTemp56)):(fSlow4 * (fTemp1 * fTemp56)))))) + (fSlow163 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp57)):(iSlow2?(fSlow4 * (fTemp3 * fTemp57)):(fSlow4 * (fTemp1 * fTemp57)))))) + (fSlow165 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp58)):(iSlow2?(fSlow4 * (fTemp3 * fTemp58)):(fSlow4 * (fTemp1 * fTemp58))))))) + ((fSlow167 * (((((((((((((fConst93 * ((fSlow168 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp59)):(iSlow2?(fSlow4 * (fTemp3 * fTemp59)):(fSlow4 * (fTemp1 * fTemp59))))) + (fSlow169 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp60)):(iSlow2?(fSlow4 * (fTemp3 * fTemp60)):(fSlow4 * (fTemp1 * fTemp60))))))) + (fSlow171 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp61)):(iSlow2?(fSlow4 * (fTemp3 * fTemp61)):(fSlow4 * (fTemp1 * fTemp61)))))) + (fSlow173 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp62)):(iSlow2?(fSlow4 * (fTemp3 * fTemp62)):(fSlow4 * (fTemp1 * fTemp62)))))) + (fSlow176 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp63)):(iSlow2?(fSlow4 * (fTemp3 * fTemp63)):(fSlow4 * (fTemp1 * fTemp63)))))) + (fSlow178 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp64)):(iSlow2?(fSlow4 * (fTemp3 * fTemp64)):(fSlow4 * (fTemp1 * fTemp64)))))) + (fSlow181 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp65)):(iSlow2?(fSlow4 * (fTemp3 * fTemp65)):(fSlow4 * (fTemp1 * fTemp65)))))) + (fSlow183 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp66)):(iSlow2?(fSlow4 * (fTemp3 * fTemp66)):(fSlow4 * (fTemp1 * fTemp66)))))) + (fSlow186 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp67)):(iSlow2?(fSlow4 * (fTemp3 * fTemp67)):(fSlow4 * (fTemp1 * fTemp67)))))) + (fSlow188 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp68)):(iSlow2?(fSlow4 * (fTemp3 * fTemp68)):(fSlow4 * (fTemp1 * fTemp68)))))) + (fSlow191 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp69)):(iSlow2?(fSlow4 * (fTemp3 * fTemp69)):(fSlow4 * (fTemp1 * fTemp69)))))) + (fSlow193 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp70)):(iSlow2?(fSlow4 * (fTemp3 * fTemp70)):(fSlow4 * (fTemp1 * fTemp70)))))) + (fSlow196 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp71)):(iSlow2?(fSlow4 * (fTemp3 * fTemp71)):(fSlow4 * (fTemp1 * fTemp71)))))) + (fSlow198 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp72)):(iSlow2?(fSlow4 * (fTemp3 * fTemp72)):(fSlow4 * (fTemp1 * fTemp72))))))) + ((fSlow200 * (((((((((((((((((fConst108 * ((fSlow24 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp73)):(iSlow2?(fSlow4 * (fTemp3 * fTemp73)):(fSlow4 * (fTemp1 * fTemp73))))) + (fSlow17 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp74)):(iSlow2?(fSlow4 * (fTemp3 * fTemp74)):(fSlow4 * (fTemp1 * fTemp74))))))) + (fSlow201 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp75)):(iSlow2?(fSlow4 * (fTemp3 * fTemp75)):(fSlow4 * (fTemp1 * fTemp75)))))) + (fSlow202 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp76)):(iSlow2?(fSlow4 * (fTemp3 * fTemp76)):(fSlow4 * (fTemp1 * fTemp76)))))) + (fSlow204 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp77)):(iSlow2?(fSlow4 * (fTemp3 * fTemp77)):(fSlow4 * (fTemp1 * fTemp77)))))) + (fSlow206 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp78)):(iSlow2?(fSlow4 * (fTemp3 * fTemp78)):(fSlow4 * (fTemp1 * fTemp78)))))) + (fSlow209 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp79)):(iSlow2?(fSlow4 * (fTemp3 * fTemp79)):(fSlow4 * (fTemp1 * fTemp79)))))) + (fSlow211 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp80)):(iSlow2?(fSlow4 * (fTemp3 * fTemp80)):(fSlow4 * (fTemp1 * fTemp80)))))) + (fSlow213 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp81)):(iSlow2?(fSlow4 * (fTemp3 * fTemp81)):(fSlow4 * (fTemp1 * fTemp81)))))) + (fSlow215 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp82)):(iSlow2?(fSlow4 * (fTemp3 * fTemp82)):(fSlow4 * (fTemp1 * fTemp82)))))) + (fSlow218 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp83)):(iSlow2?(fSlow4 * (fTemp3 * fTemp83)):(fSlow4 * (fTemp1 * fTemp83)))))) + (fSlow220 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp84)):(iSlow2?(fSlow4 * (fTemp3 * fTemp84)):(fSlow4 * (fTemp1 * fTemp84)))))) + (fSlow222 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp85)):(iSlow2?(fSlow4 * (fTemp3 * fTemp85)):(fSlow4 * (fTemp1 * fTemp85)))))) + (fSlow224 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp86)):(iSlow2?(fSlow4 * (fTemp3 * fTemp86)):(fSlow4 * (fTemp1 * fTemp86)))))) + (fSlow227 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp87)):(iSlow2?(fSlow4 * (fTemp3 * fTemp87)):(fSlow4 * (fTemp1 * fTemp87)))))) + (fSlow229 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp88)):(iSlow2?(fSlow4 * (fTemp3 * fTemp88)):(fSlow4 * (fTemp1 * fTemp88)))))) + (fSlow231 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp89)):(iSlow2?(fSlow4 * (fTemp3 * fTemp89)):(fSlow4 * (fTemp1 * fTemp89)))))) + (fSlow233 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp90)):(iSlow2?(fSlow4 * (fTemp3 * fTemp90)):(fSlow4 * (fTemp1 * fTemp90))))))) + ((((((iSlow0?(fSlow8 * (fRec2[0] * fTemp0)):(iSlow2?(fSlow4 * (fTemp3 * fTemp0)):(fSlow4 * (fTemp1 * fTemp0)))) + (fSlow234 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp91)):(iSlow2?(fSlow4 * (fTemp3 * fTemp91)):(fSlow4 * (fTemp1 * fTemp91)))))) + (fSlow235 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp92)):(iSlow2?(fSlow4 * (fTemp3 * fTemp92)):(fSlow4 * (fTemp1 * fTemp92)))))) + (fSlow236 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp93)):(iSlow2?(fSlow4 * (fTemp3 * fTemp93)):(fSlow4 * (fTemp1 * fTemp93)))))) + (fSlow237 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp94)):(iSlow2?(fSlow4 * (fTemp3 * fTemp94)):(fSlow4 * (fTemp1 * fTemp94)))))) + (fSlow24 * ((((((fSlow239 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp95)):(iSlow2?(fSlow4 * (fTemp3 * fTemp95)):(fSlow4 * (fTemp1 * fTemp95))))) + (fSlow241 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp96)):(iSlow2?(fSlow4 * (fTemp3 * fTemp96)):(fSlow4 * (fTemp1 * fTemp96)))))) + (fSlow242 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp97)):(iSlow2?(fSlow4 * (fTemp3 * fTemp97)):(fSlow4 * (fTemp1 * fTemp97)))))) + (fSlow244 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp98)):(iSlow2?(fSlow4 * (fTemp3 * fTemp98)):(fSlow4 * (fTemp1 * fTemp98)))))) + (fSlow247 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp99)):(iSlow2?(fSlow4 * (fTemp3 * fTemp99)):(fSlow4 * (fTemp1 * fTemp99)))))) + (fSlow249 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp100)):(iSlow2?(fSlow4 * (fTemp3 * fTemp100)):(fSlow4 * (fTemp1 * fTemp100)))))))))))))) + (fSlow251 * (iSlow0?(fSlow8 * (fRec2[0] * fTemp101)):(iSlow2?(fSlow4 * (fTemp3 * fTemp101)):(fSlow4 * (fTemp1 * fTemp101))))));
			output0[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp0):(iSlow2?(fSlow4 * (fRec1[0] * fTemp0)):(fSlow4 * (fRec0[0] * fTemp0)))) + (0.079577471545947673 * fTemp102)));
			output1[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp73):(iSlow2?(fSlow4 * (fRec1[0] * fTemp73)):(fSlow4 * (fRec0[0] * fTemp73)))) + (fSlow252 * fTemp102)));
			output2[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp2):(iSlow2?(fSlow4 * (fRec1[0] * fTemp2)):(fSlow4 * (fRec0[0] * fTemp2)))) + (fSlow253 * fTemp102)));
			output3[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp74):(iSlow2?(fSlow4 * (fRec1[0] * fTemp74)):(fSlow4 * (fRec0[0] * fTemp74)))) + (fSlow254 * fTemp102)));
			output4[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp21):(iSlow2?(fSlow4 * (fRec1[0] * fTemp21)):(fSlow4 * (fRec0[0] * fTemp21)))) + (fSlow255 * fTemp102)));
			output5[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp75):(iSlow2?(fSlow4 * (fRec1[0] * fTemp75)):(fSlow4 * (fRec0[0] * fTemp75)))) + (fSlow256 * fTemp102)));
			output6[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp91):(iSlow2?(fSlow4 * (fRec1[0] * fTemp91)):(fSlow4 * (fRec0[0] * fTemp91)))) + (fSlow257 * fTemp102)));
			output7[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp76):(iSlow2?(fSlow4 * (fRec1[0] * fTemp76)):(fSlow4 * (fRec0[0] * fTemp76)))) + (fSlow258 * fTemp102)));
			output8[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp22):(iSlow2?(fSlow4 * (fRec1[0] * fTemp22)):(fSlow4 * (fRec0[0] * fTemp22)))) + (fSlow259 * fTemp102)));
			output9[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp59):(iSlow2?(fSlow4 * (fRec1[0] * fTemp59)):(fSlow4 * (fRec0[0] * fTemp59)))) + (fSlow260 * fTemp102)));
			output10[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp23):(iSlow2?(fSlow4 * (fRec1[0] * fTemp23)):(fSlow4 * (fRec0[0] * fTemp23)))) + (fSlow261 * fTemp102)));
			output11[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp77):(iSlow2?(fSlow4 * (fRec1[0] * fTemp77)):(fSlow4 * (fRec0[0] * fTemp77)))) + (fSlow262 * fTemp102)));
			output12[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp4):(iSlow2?(fSlow4 * (fRec1[0] * fTemp4)):(fSlow4 * (fRec0[0] * fTemp4)))) + (fSlow263 * fTemp102)));
			output13[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp78):(iSlow2?(fSlow4 * (fRec1[0] * fTemp78)):(fSlow4 * (fRec0[0] * fTemp78)))) + (fSlow264 * fTemp102)));
			output14[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp24):(iSlow2?(fSlow4 * (fRec1[0] * fTemp24)):(fSlow4 * (fRec0[0] * fTemp24)))) + (fSlow265 * fTemp102)));
			output15[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp60):(iSlow2?(fSlow4 * (fRec1[0] * fTemp60)):(fSlow4 * (fRec0[0] * fTemp60)))) + (fSlow266 * fTemp102)));
			output16[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp25):(iSlow2?(fSlow4 * (fRec1[0] * fTemp25)):(fSlow4 * (fRec0[0] * fTemp25)))) + (fSlow267 * fTemp102)));
			output17[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp61):(iSlow2?(fSlow4 * (fRec1[0] * fTemp61)):(fSlow4 * (fRec0[0] * fTemp61)))) + (fSlow268 * fTemp102)));
			output18[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp26):(iSlow2?(fSlow4 * (fRec1[0] * fTemp26)):(fSlow4 * (fRec0[0] * fTemp26)))) + (fSlow269 * fTemp102)));
			output19[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp79):(iSlow2?(fSlow4 * (fRec1[0] * fTemp79)):(fSlow4 * (fRec0[0] * fTemp79)))) + (fSlow270 * fTemp102)));
			output20[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp92):(iSlow2?(fSlow4 * (fRec1[0] * fTemp92)):(fSlow4 * (fRec0[0] * fTemp92)))) + (fSlow271 * fTemp102)));
			output21[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp80):(iSlow2?(fSlow4 * (fRec1[0] * fTemp80)):(fSlow4 * (fRec0[0] * fTemp80)))) + (fSlow272 * fTemp102)));
			output22[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp27):(iSlow2?(fSlow4 * (fRec1[0] * fTemp27)):(fSlow4 * (fRec0[0] * fTemp27)))) + (fSlow273 * fTemp102)));
			output23[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp62):(iSlow2?(fSlow4 * (fRec1[0] * fTemp62)):(fSlow4 * (fRec0[0] * fTemp62)))) + (fSlow274 * fTemp102)));
			output24[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp28):(iSlow2?(fSlow4 * (fRec1[0] * fTemp28)):(fSlow4 * (fRec0[0] * fTemp28)))) + (fSlow275 * fTemp102)));
			output25[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp49):(iSlow2?(fSlow4 * (fRec1[0] * fTemp49)):(fSlow4 * (fRec0[0] * fTemp49)))) + (fSlow276 * fTemp102)));
			output26[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp5):(iSlow2?(fSlow4 * (fRec1[0] * fTemp5)):(fSlow4 * (fRec0[0] * fTemp5)))) + (fSlow277 * fTemp102)));
			output27[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp63):(iSlow2?(fSlow4 * (fRec1[0] * fTemp63)):(fSlow4 * (fRec0[0] * fTemp63)))) + (fSlow278 * fTemp102)));
			output28[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp29):(iSlow2?(fSlow4 * (fRec1[0] * fTemp29)):(fSlow4 * (fRec0[0] * fTemp29)))) + (fSlow279 * fTemp102)));
			output29[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp81):(iSlow2?(fSlow4 * (fRec1[0] * fTemp81)):(fSlow4 * (fRec0[0] * fTemp81)))) + (fSlow280 * fTemp102)));
			output30[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp6):(iSlow2?(fSlow4 * (fRec1[0] * fTemp6)):(fSlow4 * (fRec0[0] * fTemp6)))) + (fSlow281 * fTemp102)));
			output31[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp82):(iSlow2?(fSlow4 * (fRec1[0] * fTemp82)):(fSlow4 * (fRec0[0] * fTemp82)))) + (fSlow282 * fTemp102)));
			output32[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp30):(iSlow2?(fSlow4 * (fRec1[0] * fTemp30)):(fSlow4 * (fRec0[0] * fTemp30)))) + (fSlow283 * fTemp102)));
			output33[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp64):(iSlow2?(fSlow4 * (fRec1[0] * fTemp64)):(fSlow4 * (fRec0[0] * fTemp64)))) + (fSlow284 * fTemp102)));
			output34[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp7):(iSlow2?(fSlow4 * (fRec1[0] * fTemp7)):(fSlow4 * (fRec0[0] * fTemp7)))) + (fSlow285 * fTemp102)));
			output35[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp50):(iSlow2?(fSlow4 * (fRec1[0] * fTemp50)):(fSlow4 * (fRec0[0] * fTemp50)))) + (fSlow286 * fTemp102)));
			output36[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp31):(iSlow2?(fSlow4 * (fRec1[0] * fTemp31)):(fSlow4 * (fRec0[0] * fTemp31)))) + (fSlow287 * fTemp102)));
			output37[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp51):(iSlow2?(fSlow4 * (fRec1[0] * fTemp51)):(fSlow4 * (fRec0[0] * fTemp51)))) + (fSlow288 * fTemp102)));
			output38[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp32):(iSlow2?(fSlow4 * (fRec1[0] * fTemp32)):(fSlow4 * (fRec0[0] * fTemp32)))) + (fSlow289 * fTemp102)));
			output39[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp65):(iSlow2?(fSlow4 * (fRec1[0] * fTemp65)):(fSlow4 * (fRec0[0] * fTemp65)))) + (fSlow290 * fTemp102)));
			output40[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp33):(iSlow2?(fSlow4 * (fRec1[0] * fTemp33)):(fSlow4 * (fRec0[0] * fTemp33)))) + (fSlow291 * fTemp102)));
			output41[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp83):(iSlow2?(fSlow4 * (fRec1[0] * fTemp83)):(fSlow4 * (fRec0[0] * fTemp83)))) + (fSlow292 * fTemp102)));
			output42[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp93):(iSlow2?(fSlow4 * (fRec1[0] * fTemp93)):(fSlow4 * (fRec0[0] * fTemp93)))) + (fSlow293 * fTemp102)));
			output43[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp84):(iSlow2?(fSlow4 * (fRec1[0] * fTemp84)):(fSlow4 * (fRec0[0] * fTemp84)))) + (fSlow294 * fTemp102)));
			output44[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp34):(iSlow2?(fSlow4 * (fRec1[0] * fTemp34)):(fSlow4 * (fRec0[0] * fTemp34)))) + (fSlow295 * fTemp102)));
			output45[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp66):(iSlow2?(fSlow4 * (fRec1[0] * fTemp66)):(fSlow4 * (fRec0[0] * fTemp66)))) + (fSlow296 * fTemp102)));
			output46[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp35):(iSlow2?(fSlow4 * (fRec1[0] * fTemp35)):(fSlow4 * (fRec0[0] * fTemp35)))) + (fSlow297 * fTemp102)));
			output47[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp52):(iSlow2?(fSlow4 * (fRec1[0] * fTemp52)):(fSlow4 * (fRec0[0] * fTemp52)))) + (fSlow298 * fTemp102)));
			output48[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp36):(iSlow2?(fSlow4 * (fRec1[0] * fTemp36)):(fSlow4 * (fRec0[0] * fTemp36)))) + (fSlow299 * fTemp102)));
			output49[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp95):(iSlow2?(fSlow4 * (fRec1[0] * fTemp95)):(fSlow4 * (fRec0[0] * fTemp95)))) + (fSlow300 * fTemp102)));
			output50[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp8):(iSlow2?(fSlow4 * (fRec1[0] * fTemp8)):(fSlow4 * (fRec0[0] * fTemp8)))) + (fSlow301 * fTemp102)));
			output51[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp53):(iSlow2?(fSlow4 * (fRec1[0] * fTemp53)):(fSlow4 * (fRec0[0] * fTemp53)))) + (fSlow302 * fTemp102)));
			output52[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp37):(iSlow2?(fSlow4 * (fRec1[0] * fTemp37)):(fSlow4 * (fRec0[0] * fTemp37)))) + (fSlow303 * fTemp102)));
			output53[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp67):(iSlow2?(fSlow4 * (fRec1[0] * fTemp67)):(fSlow4 * (fRec0[0] * fTemp67)))) + (fSlow304 * fTemp102)));
			output54[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp96):(iSlow2?(fSlow4 * (fRec1[0] * fTemp96)):(fSlow4 * (fRec0[0] * fTemp96)))) + (fSlow305 * fTemp102)));
			output55[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp85):(iSlow2?(fSlow4 * (fRec1[0] * fTemp85)):(fSlow4 * (fRec0[0] * fTemp85)))) + (fSlow306 * fTemp102)));
			output56[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp9):(iSlow2?(fSlow4 * (fRec1[0] * fTemp9)):(fSlow4 * (fRec0[0] * fTemp9)))) + (fSlow307 * fTemp102)));
			output57[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp86):(iSlow2?(fSlow4 * (fRec1[0] * fTemp86)):(fSlow4 * (fRec0[0] * fTemp86)))) + (fSlow308 * fTemp102)));
			output58[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp38):(iSlow2?(fSlow4 * (fRec1[0] * fTemp38)):(fSlow4 * (fRec0[0] * fTemp38)))) + (fSlow309 * fTemp102)));
			output59[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp68):(iSlow2?(fSlow4 * (fRec1[0] * fTemp68)):(fSlow4 * (fRec0[0] * fTemp68)))) + (fSlow310 * fTemp102)));
			output60[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp10):(iSlow2?(fSlow4 * (fRec1[0] * fTemp10)):(fSlow4 * (fRec0[0] * fTemp10)))) + (fSlow311 * fTemp102)));
			output61[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp54):(iSlow2?(fSlow4 * (fRec1[0] * fTemp54)):(fSlow4 * (fRec0[0] * fTemp54)))) + (fSlow312 * fTemp102)));
			output62[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp39):(iSlow2?(fSlow4 * (fRec1[0] * fTemp39)):(fSlow4 * (fRec0[0] * fTemp39)))) + (fSlow313 * fTemp102)));
			output63[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp16):(iSlow2?(fSlow4 * (fRec1[0] * fTemp16)):(fSlow4 * (fRec0[0] * fTemp16)))) + (fSlow314 * fTemp102)));
			output64[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp97):(iSlow2?(fSlow4 * (fRec1[0] * fTemp97)):(fSlow4 * (fRec0[0] * fTemp97)))) + (fSlow315 * fTemp102)));
			output65[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp17):(iSlow2?(fSlow4 * (fRec1[0] * fTemp17)):(fSlow4 * (fRec0[0] * fTemp17)))) + (fSlow316 * fTemp102)));
			output66[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp40):(iSlow2?(fSlow4 * (fRec1[0] * fTemp40)):(fSlow4 * (fRec0[0] * fTemp40)))) + (fSlow317 * fTemp102)));
			output67[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp55):(iSlow2?(fSlow4 * (fRec1[0] * fTemp55)):(fSlow4 * (fRec0[0] * fTemp55)))) + (fSlow318 * fTemp102)));
			output68[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp98):(iSlow2?(fSlow4 * (fRec1[0] * fTemp98)):(fSlow4 * (fRec0[0] * fTemp98)))) + (fSlow319 * fTemp102)));
			output69[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp69):(iSlow2?(fSlow4 * (fRec1[0] * fTemp69)):(fSlow4 * (fRec0[0] * fTemp69)))) + (fSlow320 * fTemp102)));
			output70[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp41):(iSlow2?(fSlow4 * (fRec1[0] * fTemp41)):(fSlow4 * (fRec0[0] * fTemp41)))) + (fSlow321 * fTemp102)));
			output71[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp87):(iSlow2?(fSlow4 * (fRec1[0] * fTemp87)):(fSlow4 * (fRec0[0] * fTemp87)))) + (fSlow322 * fTemp102)));
			output72[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp94):(iSlow2?(fSlow4 * (fRec1[0] * fTemp94)):(fSlow4 * (fRec0[0] * fTemp94)))) + (fSlow323 * fTemp102)));
			output73[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp88):(iSlow2?(fSlow4 * (fRec1[0] * fTemp88)):(fSlow4 * (fRec0[0] * fTemp88)))) + (fSlow324 * fTemp102)));
			output74[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp42):(iSlow2?(fSlow4 * (fRec1[0] * fTemp42)):(fSlow4 * (fRec0[0] * fTemp42)))) + (fSlow325 * fTemp102)));
			output75[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp70):(iSlow2?(fSlow4 * (fRec1[0] * fTemp70)):(fSlow4 * (fRec0[0] * fTemp70)))) + (fSlow326 * fTemp102)));
			output76[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp43):(iSlow2?(fSlow4 * (fRec1[0] * fTemp43)):(fSlow4 * (fRec0[0] * fTemp43)))) + (fSlow327 * fTemp102)));
			output77[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp56):(iSlow2?(fSlow4 * (fRec1[0] * fTemp56)):(fSlow4 * (fRec0[0] * fTemp56)))) + (fSlow328 * fTemp102)));
			output78[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp44):(iSlow2?(fSlow4 * (fRec1[0] * fTemp44)):(fSlow4 * (fRec0[0] * fTemp44)))) + (fSlow329 * fTemp102)));
			output79[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp18):(iSlow2?(fSlow4 * (fRec1[0] * fTemp18)):(fSlow4 * (fRec0[0] * fTemp18)))) + (fSlow330 * fTemp102)));
			output80[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp45):(iSlow2?(fSlow4 * (fRec1[0] * fTemp45)):(fSlow4 * (fRec0[0] * fTemp45)))) + (fSlow331 * fTemp102)));
			output81[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp99):(iSlow2?(fSlow4 * (fRec1[0] * fTemp99)):(fSlow4 * (fRec0[0] * fTemp99)))) + (fSlow332 * fTemp102)));
			output82[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp11):(iSlow2?(fSlow4 * (fRec1[0] * fTemp11)):(fSlow4 * (fRec0[0] * fTemp11)))) + (fSlow333 * fTemp102)));
			output83[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp19):(iSlow2?(fSlow4 * (fRec1[0] * fTemp19)):(fSlow4 * (fRec0[0] * fTemp19)))) + (fSlow334 * fTemp102)));
			output84[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp46):(iSlow2?(fSlow4 * (fRec1[0] * fTemp46)):(fSlow4 * (fRec0[0] * fTemp46)))) + (fSlow335 * fTemp102)));
			output85[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp57):(iSlow2?(fSlow4 * (fRec1[0] * fTemp57)):(fSlow4 * (fRec0[0] * fTemp57)))) + (fSlow336 * fTemp102)));
			output86[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp100):(iSlow2?(fSlow4 * (fRec1[0] * fTemp100)):(fSlow4 * (fRec0[0] * fTemp100)))) + (fSlow337 * fTemp102)));
			output87[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp71):(iSlow2?(fSlow4 * (fRec1[0] * fTemp71)):(fSlow4 * (fRec0[0] * fTemp71)))) + (fSlow338 * fTemp102)));
			output88[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp12):(iSlow2?(fSlow4 * (fRec1[0] * fTemp12)):(fSlow4 * (fRec0[0] * fTemp12)))) + (fSlow339 * fTemp102)));
			output89[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp89):(iSlow2?(fSlow4 * (fRec1[0] * fTemp89)):(fSlow4 * (fRec0[0] * fTemp89)))) + (fSlow340 * fTemp102)));
			output90[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp13):(iSlow2?(fSlow4 * (fRec1[0] * fTemp13)):(fSlow4 * (fRec0[0] * fTemp13)))) + (fSlow341 * fTemp102)));
			output91[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp90):(iSlow2?(fSlow4 * (fRec1[0] * fTemp90)):(fSlow4 * (fRec0[0] * fTemp90)))) + (fSlow342 * fTemp102)));
			output92[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp47):(iSlow2?(fSlow4 * (fRec1[0] * fTemp47)):(fSlow4 * (fRec0[0] * fTemp47)))) + (fSlow343 * fTemp102)));
			output93[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp72):(iSlow2?(fSlow4 * (fRec1[0] * fTemp72)):(fSlow4 * (fRec0[0] * fTemp72)))) + (fSlow344 * fTemp102)));
			output94[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp14):(iSlow2?(fSlow4 * (fRec1[0] * fTemp14)):(fSlow4 * (fRec0[0] * fTemp14)))) + (fSlow345 * fTemp102)));
			output95[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp58):(iSlow2?(fSlow4 * (fRec1[0] * fTemp58)):(fSlow4 * (fRec0[0] * fTemp58)))) + (fSlow346 * fTemp102)));
			output96[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp48):(iSlow2?(fSlow4 * (fRec1[0] * fTemp48)):(fSlow4 * (fRec0[0] * fTemp48)))) + (fSlow347 * fTemp102)));
			output97[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp20):(iSlow2?(fSlow4 * (fRec1[0] * fTemp20)):(fSlow4 * (fRec0[0] * fTemp20)))) + (fSlow348 * fTemp102)));
			output98[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp15):(iSlow2?(fSlow4 * (fRec1[0] * fTemp15)):(fSlow4 * (fRec0[0] * fTemp15)))) + (fSlow349 * fTemp102)));
			output99[i] = FAUSTFLOAT(((iSlow0?(fSlow9 * fTemp101):(iSlow2?(fSlow4 * (fRec1[0] * fTemp101)):(fSlow4 * (fRec0[0] * fTemp101)))) + (fSlow350 * fTemp102)));
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
    for (size_t i = 0; i < numControls; ++i) {
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
