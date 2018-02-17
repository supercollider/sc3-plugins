/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2016"
license: "GPL"
name: "HOABeamDirac2HOA10"
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

double mydsp_faustpower2_f(double value) {
	return (value * value);
	
}
double mydsp_faustpower3_f(double value) {
	return ((value * value) * value);
	
}
double mydsp_faustpower4_f(double value) {
	return (((value * value) * value) * value);
	
}
double mydsp_faustpower5_f(double value) {
	return ((((value * value) * value) * value) * value);
	
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
	int fSamplingFreq;
	double fConst0;
	double fConst1;
	FAUSTFLOAT fEntry0;
	double fRec0[2];
	double fRec1[2];
	FAUSTFLOAT fVslider0;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	FAUSTFLOAT fVslider1;
	FAUSTFLOAT fVslider2;
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
	double fConst196;
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
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Pierre Lecomte");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
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
		m->declare("name", "HOABeamDirac2HOA10");
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
		return 121;
		
	}
	virtual int getNumOutputs() {
		return 121;
		
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
			case 100: {
				rate = 1;
				break;
			}
			case 101: {
				rate = 1;
				break;
			}
			case 102: {
				rate = 1;
				break;
			}
			case 103: {
				rate = 1;
				break;
			}
			case 104: {
				rate = 1;
				break;
			}
			case 105: {
				rate = 1;
				break;
			}
			case 106: {
				rate = 1;
				break;
			}
			case 107: {
				rate = 1;
				break;
			}
			case 108: {
				rate = 1;
				break;
			}
			case 109: {
				rate = 1;
				break;
			}
			case 110: {
				rate = 1;
				break;
			}
			case 111: {
				rate = 1;
				break;
			}
			case 112: {
				rate = 1;
				break;
			}
			case 113: {
				rate = 1;
				break;
			}
			case 114: {
				rate = 1;
				break;
			}
			case 115: {
				rate = 1;
				break;
			}
			case 116: {
				rate = 1;
				break;
			}
			case 117: {
				rate = 1;
				break;
			}
			case 118: {
				rate = 1;
				break;
			}
			case 119: {
				rate = 1;
				break;
			}
			case 120: {
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
			case 100: {
				rate = 1;
				break;
			}
			case 101: {
				rate = 1;
				break;
			}
			case 102: {
				rate = 1;
				break;
			}
			case 103: {
				rate = 1;
				break;
			}
			case 104: {
				rate = 1;
				break;
			}
			case 105: {
				rate = 1;
				break;
			}
			case 106: {
				rate = 1;
				break;
			}
			case 107: {
				rate = 1;
				break;
			}
			case 108: {
				rate = 1;
				break;
			}
			case 109: {
				rate = 1;
				break;
			}
			case 110: {
				rate = 1;
				break;
			}
			case 111: {
				rate = 1;
				break;
			}
			case 112: {
				rate = 1;
				break;
			}
			case 113: {
				rate = 1;
				break;
			}
			case 114: {
				rate = 1;
				break;
			}
			case 115: {
				rate = 1;
				break;
			}
			case 116: {
				rate = 1;
				break;
			}
			case 117: {
				rate = 1;
				break;
			}
			case 118: {
				rate = 1;
				break;
			}
			case 119: {
				rate = 1;
				break;
			}
			case 120: {
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
		fConst43 = double(tgamma(14.0));
		fConst44 = (sqrt((15.0 * (fConst6 / fConst43))) * fConst27);
		fConst45 = (663.83995458798404 * fConst44);
		fConst46 = double(tgamma(15.0));
		fConst47 = (sqrt((17.0 * (fConst15 / fConst46))) * fConst27);
		fConst48 = (0.70710678118654757 * fConst47);
		fConst49 = (sqrt((17.0 * (fConst16 / fConst30))) * fConst2);
		fConst50 = (0.47140452079103168 * fConst49);
		fConst51 = (0.23570226039551584 * fConst49);
		fConst52 = (sqrt((17.0 * (fConst4 / fConst26))) * fConst12);
		fConst53 = (0.35355339059327379 * fConst52);
		fConst54 = double(tgamma(17.0));
		fConst55 = double(tgamma(8.5));
		fConst56 = (sqrt((17.0 * (fConst3 / fConst54))) * fConst55);
		fConst57 = (204.25844756553354 * fConst56);
		fConst58 = double(tgamma(16.0));
		fConst59 = (sqrt((19.0 * (fConst21 / fConst58))) * fConst27);
		fConst60 = (0.47140452079103168 * fConst59);
		fConst61 = (sqrt((19.0 * (fConst22 / fConst37))) * fConst2);
		fConst62 = (0.20203050891044216 * fConst61);
		fConst63 = double(tgamma(19.0));
		fConst64 = (sqrt((21.0 * (fConst15 / fConst63))) * fConst55);
		fConst65 = (1.4142135623730951 * fConst64);
		fConst66 = (sqrt((21.0 * (fConst16 / fConst46))) * fConst12);
		fConst67 = (0.47140452079103168 * fConst66);
		fConst68 = (sqrt((21.0 * (fConst11 / fConst26))) * fConst2);
		fConst69 = (0.17677669529663689 * fConst68);
		fConst70 = (0.23570226039551584 * fConst66);
		fConst71 = (sqrt((21.0 * (fConst4 / fConst54))) * fConst27);
		fConst72 = (0.35355339059327379 * fConst71);
		fConst73 = (0.70710678118654757 * fConst64);
		fConst74 = (sqrt((21.0 * (fConst3 / double(tgamma(21.0))))) * double(tgamma(10.5)));
		fConst75 = (817.03379026213418 * fConst74);
		fConst76 = double(tgamma(5.5));
		fConst77 = sqrt((11.0 * (fConst3 / fConst30)));
		fConst78 = (25.532305945691693 * fConst77);
		fConst79 = sqrt((13.0 * (fConst6 / fConst37)));
		fConst80 = (280.85536540260864 * fConst79);
		fConst81 = sqrt((15.0 * (fConst15 / fConst26)));
		fConst82 = (0.70710678118654757 * fConst81);
		fConst83 = sqrt((17.0 * (fConst21 / fConst43)));
		fConst84 = (0.47140452079103168 * fConst83);
		fConst85 = sqrt((19.0 * (fConst4 / fConst46)));
		fConst86 = (0.35355339059327379 * fConst85);
		fConst87 = sqrt((21.0 * (fConst7 / fConst58)));
		fConst88 = (0.28284271247461906 * fConst87);
		fConst89 = double(tgamma(3.5));
		fConst90 = sqrt((7.0 * (fConst3 / fConst16)));
		fConst91 = (6.3830764864229232 * fConst90);
		fConst92 = sqrt((9.0 * (fConst6 / fConst22)));
		fConst93 = (44.681535404960464 * fConst92);
		fConst94 = sqrt((11.0 * (fConst15 / fConst11)));
		fConst95 = (0.70710678118654757 * fConst94);
		fConst96 = sqrt((13.0 * (fConst21 / fConst40)));
		fConst97 = (0.47140452079103168 * fConst96);
		fConst98 = sqrt((15.0 * (fConst4 / fConst30)));
		fConst99 = (0.35355339059327379 * fConst98);
		fConst100 = sqrt((17.0 * (fConst7 / fConst37)));
		fConst101 = (0.28284271247461906 * fConst100);
		fConst102 = sqrt((19.0 * (fConst16 / fConst26)));
		fConst103 = (0.23570226039551584 * fConst102);
		fConst104 = sqrt((21.0 * (fConst22 / fConst43)));
		fConst105 = (0.20203050891044216 * fConst104);
		fConst106 = double(tgamma(1.5));
		fConst107 = sqrt((3.0 * (fConst3 / fConst15)));
		fConst108 = (1.5957691216057308 * fConst107);
		fConst109 = sqrt((5.0 * (fConst6 / fConst21)));
		fConst110 = (4.7873073648171927 * fConst109);
		fConst111 = sqrt((7.0 * (fConst15 / fConst4)));
		fConst112 = (0.70710678118654757 * fConst111);
		fConst113 = sqrt((9.0 * (fConst21 / fConst7)));
		fConst114 = (0.47140452079103168 * fConst113);
		fConst115 = sqrt((11.0 * (fConst4 / fConst16)));
		fConst116 = (0.35355339059327379 * fConst115);
		fConst117 = sqrt((13.0 * (fConst7 / fConst22)));
		fConst118 = (0.28284271247461906 * fConst117);
		fConst119 = sqrt((15.0 * (fConst16 / fConst11)));
		fConst120 = (0.23570226039551584 * fConst119);
		fConst121 = sqrt((17.0 * (fConst22 / fConst40)));
		fConst122 = (0.20203050891044216 * fConst121);
		fConst123 = sqrt((19.0 * (fConst11 / fConst30)));
		fConst124 = (0.17677669529663689 * fConst123);
		fConst125 = sqrt((21.0 * (fConst40 / fConst37)));
		fConst126 = (0.15713484026367722 * fConst125);
		fConst127 = sqrt((15.0 * (fConst3 / fConst46)));
		fConst128 = double(tgamma(7.5));
		fConst129 = (fConst127 * fConst128);
		fConst130 = (102.12922378276677 * fConst129);
		fConst131 = (0.56568542494923812 * fConst41);
		fConst132 = (408.51689513106709 * fConst56);
		fConst133 = (0.70710678118654757 * fConst52);
		fConst134 = sqrt((19.0 * (fConst3 / fConst63)));
		fConst135 = double(tgamma(9.5));
		fConst136 = (fConst134 * fConst135);
		fConst137 = (408.51689513106709 * fConst136);
		fConst138 = (sqrt((19.0 * (fConst7 / fConst43))) * fConst12);
		fConst139 = (0.56568542494923812 * fConst138);
		fConst140 = (0.35355339059327379 * fConst68);
		fConst141 = (sqrt((11.0 * (fConst6 / fConst40))) * fConst12);
		fConst142 = (229.79075351122526 * fConst141);
		fConst143 = (114.89537675561263 * fConst141);
		fConst144 = (0.47140452079103168 * fConst38);
		fConst145 = double(tgamma(18.0));
		fConst146 = (sqrt((19.0 * (fConst6 / fConst145))) * fConst55);
		fConst147 = (6944.7872172281404 * fConst146);
		fConst148 = (0.40406101782088433 * fConst61);
		fConst149 = (0.28284271247461906 * fConst138);
		fConst150 = (3472.3936086140702 * fConst146);
		fConst151 = (102.12922378276677 * fConst127);
		fConst152 = sqrt((17.0 * (fConst6 / fConst58)));
		fConst153 = (1531.9383567415016 * fConst152);
		fConst154 = sqrt((19.0 * (fConst15 / fConst54)));
		fConst155 = (0.70710678118654757 * fConst154);
		fConst156 = sqrt((21.0 * (fConst21 / fConst145)));
		fConst157 = (0.47140452079103168 * fConst156);
		fConst158 = (408.51689513106709 * fConst134);
		fConst159 = sqrt((21.0 * (fConst6 / double(tgamma(20.0)))));
		fConst160 = (7761.8210074902745 * fConst159);
		fConst161 = (0.12698727186848197 * (fConst107 * fConst106));
		fConst162 = (fConst5 * fConst2);
		fConst163 = (0.50794908747392786 * fConst162);
		fConst164 = (0.3809618156054459 * (fConst109 * fConst106));
		fConst165 = (0.25397454373696393 * fConst162);
		fConst166 = (0.50794908747392786 * (fConst90 * fConst89));
		fConst167 = (fConst8 * fConst2);
		fConst168 = (2.5397454373696391 * fConst167);
		fConst169 = (0.056269769759819135 * (fConst111 * fConst106));
		fConst170 = (1.2698727186848195 * fConst167);
		fConst171 = (2.0317963498957115 * fConst13);
		fConst172 = (3.5556436123174948 * (fConst92 * fConst89));
		fConst173 = (0.11253953951963827 * fConst17);
		fConst174 = (0.037513179839879424 * (fConst113 * fConst106));
		fConst175 = (0.056269769759819135 * fConst17);
		fConst176 = (1.0158981749478557 * fConst13);
		fConst177 = (2.0317963498957115 * (fConst77 * fConst76));
		fConst178 = (18.286167149061402 * fConst141);
		fConst179 = (0.056269769759819135 * (fConst94 * fConst89));
		fConst180 = (0.075026359679758847 * fConst23);
		fConst181 = (0.028134884879909568 * (fConst115 * fConst106));
		fConst182 = (0.037513179839879424 * fConst23);
		fConst183 = (9.1430835745307011 * fConst141);
		fConst184 = (4.0635926997914229 * fConst28);
		fConst185 = (22.349759848852827 * (fConst79 * fConst76));
		fConst186 = (0.11253953951963827 * fConst31);
		fConst187 = (0.037513179839879424 * (fConst96 * fConst89));
		fConst188 = (0.056269769759819135 * fConst33);
		fConst189 = (0.022507907903927659 * (fConst117 * fConst106));
		fConst190 = (0.028134884879909568 * fConst33);
		fConst191 = (0.056269769759819135 * fConst31);
		fConst192 = (8.1271853995828458 * fConst129);
		fConst193 = (52.826705097288496 * fConst44);
		fConst194 = (0.056269769759819135 * (fConst81 * fConst76));
		fConst195 = (0.075026359679758847 * fConst38);
		fConst196 = (0.028134884879909568 * (fConst98 * fConst89));
		fConst197 = (0.045015815807855318 * fConst41);
		fConst198 = (0.018756589919939712 * (fConst119 * fConst106));
		fConst199 = (0.022507907903927659 * fConst41);
		fConst200 = (0.037513179839879424 * fConst38);
		fConst201 = (32.508741598331383 * fConst56);
		fConst202 = (121.90778099374268 * (fConst152 * fConst128));
		fConst203 = (0.056269769759819135 * fConst47);
		fConst204 = (0.037513179839879424 * (fConst83 * fConst76));
		fConst205 = (0.056269769759819135 * fConst52);
		fConst206 = (0.022507907903927659 * (fConst100 * fConst89));
		fConst207 = (0.037513179839879424 * fConst49);
		fConst208 = (0.016077077074234038 * (fConst121 * fConst106));
		fConst209 = (0.018756589919939712 * fConst49);
		fConst210 = (0.028134884879909568 * fConst52);
		fConst211 = (16.254370799165692 * fConst56);
		fConst212 = (32.508741598331383 * fConst136);
		fConst213 = (552.64860717163344 * fConst146);
		fConst214 = (0.056269769759819135 * (fConst154 * fConst128));
		fConst215 = (0.037513179839879424 * fConst59);
		fConst216 = (0.028134884879909568 * (fConst85 * fConst76));
		fConst217 = (0.045015815807855318 * fConst138);
		fConst218 = (0.018756589919939712 * (fConst102 * fConst89));
		fConst219 = (0.032154154148468075 * fConst61);
		fConst220 = (0.014067442439954784 * (fConst123 * fConst106));
		fConst221 = (0.016077077074234038 * fConst61);
		fConst222 = (0.022507907903927659 * fConst138);
		fConst223 = (276.32430358581672 * fConst146);
		fConst224 = (65.017483196662766 * fConst74);
		fConst225 = (617.66609036829618 * (fConst159 * fConst135));
		fConst226 = (0.11253953951963827 * fConst64);
		fConst227 = (0.037513179839879424 * (fConst156 * fConst128));
		fConst228 = (0.028134884879909568 * fConst71);
		fConst229 = (0.022507907903927659 * (fConst87 * fConst76));
		fConst230 = (0.037513179839879424 * fConst66);
		fConst231 = (0.016077077074234038 * (fConst104 * fConst89));
		fConst232 = (0.028134884879909568 * fConst68);
		fConst233 = (0.012504393279959806 * (fConst125 * fConst106));
		fConst234 = (0.014067442439954784 * fConst68);
		fConst235 = (0.018756589919939712 * fConst66);
		fConst236 = (0.056269769759819135 * fConst64);
		
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = FAUSTFLOAT(0.0);
		fEntry0 = FAUSTFLOAT(1.0);
		fVslider0 = FAUSTFLOAT(0.0);
		fVslider1 = FAUSTFLOAT(0.0);
		fVslider2 = FAUSTFLOAT(0.0);
		
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
		ui_interface->addCheckButton("On", &fCheckbox0);
		ui_interface->closeBox();
		ui_interface->declare(&fEntry0, "3", "");
		ui_interface->declare(&fEntry0, "unit", "s");
		ui_interface->addNumEntry("Crossfade", &fEntry0, 1.0, 0.10000000000000001, 10.0, 0.10000000000000001);
		ui_interface->declare(&fVslider2, "4", "");
		ui_interface->declare(&fVslider2, "unit", "dB");
		ui_interface->addVerticalSlider("Gain", &fVslider2, 0.0, -20.0, 20.0, 0.10000000000000001);
		ui_interface->declare(&fVslider1, "5", "");
		ui_interface->addVerticalSlider("Azimuth", &fVslider1, 0.0, -3.1415926535897931, 3.1415926535897931, 0.10000000000000001);
		ui_interface->declare(&fVslider0, "6", "");
		ui_interface->addVerticalSlider("Elevation", &fVslider0, 0.0, -1.5707963267948966, 1.5707963267948966, 0.10000000000000001);
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
		FAUSTFLOAT* input100 = inputs[100];
		FAUSTFLOAT* input101 = inputs[101];
		FAUSTFLOAT* input102 = inputs[102];
		FAUSTFLOAT* input103 = inputs[103];
		FAUSTFLOAT* input104 = inputs[104];
		FAUSTFLOAT* input105 = inputs[105];
		FAUSTFLOAT* input106 = inputs[106];
		FAUSTFLOAT* input107 = inputs[107];
		FAUSTFLOAT* input108 = inputs[108];
		FAUSTFLOAT* input109 = inputs[109];
		FAUSTFLOAT* input110 = inputs[110];
		FAUSTFLOAT* input111 = inputs[111];
		FAUSTFLOAT* input112 = inputs[112];
		FAUSTFLOAT* input113 = inputs[113];
		FAUSTFLOAT* input114 = inputs[114];
		FAUSTFLOAT* input115 = inputs[115];
		FAUSTFLOAT* input116 = inputs[116];
		FAUSTFLOAT* input117 = inputs[117];
		FAUSTFLOAT* input118 = inputs[118];
		FAUSTFLOAT* input119 = inputs[119];
		FAUSTFLOAT* input120 = inputs[120];
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
		FAUSTFLOAT* output100 = outputs[100];
		FAUSTFLOAT* output101 = outputs[101];
		FAUSTFLOAT* output102 = outputs[102];
		FAUSTFLOAT* output103 = outputs[103];
		FAUSTFLOAT* output104 = outputs[104];
		FAUSTFLOAT* output105 = outputs[105];
		FAUSTFLOAT* output106 = outputs[106];
		FAUSTFLOAT* output107 = outputs[107];
		FAUSTFLOAT* output108 = outputs[108];
		FAUSTFLOAT* output109 = outputs[109];
		FAUSTFLOAT* output110 = outputs[110];
		FAUSTFLOAT* output111 = outputs[111];
		FAUSTFLOAT* output112 = outputs[112];
		FAUSTFLOAT* output113 = outputs[113];
		FAUSTFLOAT* output114 = outputs[114];
		FAUSTFLOAT* output115 = outputs[115];
		FAUSTFLOAT* output116 = outputs[116];
		FAUSTFLOAT* output117 = outputs[117];
		FAUSTFLOAT* output118 = outputs[118];
		FAUSTFLOAT* output119 = outputs[119];
		FAUSTFLOAT* output120 = outputs[120];
		double fSlow0 = double(fCheckbox0);
		int iSlow1 = int(fSlow0);
		double fSlow2 = double(fEntry0);
		double fSlow3 = (fConst1 / fSlow2);
		int iSlow4 = (fSlow0 > 0.0);
		double fSlow5 = (fConst0 * fSlow2);
		int iSlow6 = ((1.0 - fSlow0) > 0.0);
		double fSlow7 = sin(double(fVslider0));
		double fSlow8 = mydsp_faustpower2_f(fSlow7);
		double fSlow9 = (1.0 - fSlow8);
		double fSlow10 = double(fVslider1);
		double fSlow11 = cos(fSlow10);
		double fSlow12 = sin(fSlow10);
		double fSlow13 = (6.3830764864229232 * (fSlow11 * fSlow12));
		double fSlow14 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider2))));
		double fSlow15 = mydsp_faustpower2_f(fSlow11);
		double fSlow16 = ((2.0 * fSlow15) + -1.0);
		double fSlow17 = (3.1915382432114616 * fSlow16);
		double fSlow18 = (fSlow11 * fSlow7);
		double fSlow19 = (fConst9 * (fSlow18 * fSlow12));
		double fSlow20 = (fSlow16 * fSlow7);
		double fSlow21 = (fConst10 * fSlow20);
		double fSlow22 = ((4.0 * fSlow15) + -1.0);
		double fSlow23 = (fSlow22 + -1.0);
		double fSlow24 = (fSlow23 * fSlow11);
		double fSlow25 = (fConst14 * ((fSlow24 * fSlow9) * fSlow12));
		double fSlow26 = ((78.986541696685876 * fSlow8) + -11.283791670955125);
		double fSlow27 = (fSlow26 * fSlow11);
		double fSlow28 = (fConst18 * (fSlow27 * fSlow12));
		double fSlow29 = (fSlow16 * fSlow26);
		double fSlow30 = (fConst19 * fSlow29);
		double fSlow31 = ((2.0 * fSlow16) + -1.0);
		double fSlow32 = (1.0 - fSlow31);
		double fSlow33 = (1.0 - (2.0 * (fSlow32 * fSlow15)));
		double fSlow34 = (fConst20 * (fSlow33 * fSlow9));
		double fSlow35 = ((4.5 * fSlow26) + -67.702750025730751);
		double fSlow36 = ((fSlow35 * fSlow11) * fSlow7);
		double fSlow37 = (fConst24 * (fSlow36 * fSlow12));
		double fSlow38 = ((fSlow16 * fSlow35) * fSlow7);
		double fSlow39 = (fConst25 * fSlow38);
		double fSlow40 = (1.0 - fSlow23);
		double fSlow41 = (1.0 - (4.0 * (fSlow40 * fSlow15)));
		double fSlow42 = (fSlow23 - fSlow41);
		double fSlow43 = (0.0 - (2.0 * (fSlow42 * fSlow11)));
		double fSlow44 = mydsp_faustpower2_f(fSlow9);
		double fSlow45 = (fConst29 * ((fSlow43 * fSlow44) * fSlow12));
		double fSlow46 = ((893.67630033964599 * fSlow8) + -81.24330003087691);
		double fSlow47 = ((fSlow23 * fSlow46) * fSlow11);
		double fSlow48 = (fConst32 * ((fSlow47 * fSlow9) * fSlow12));
		double fSlow49 = ((3.6666666666666665 * (fSlow35 * fSlow8)) - (3.5 * fSlow26));
		double fSlow50 = (fSlow49 * fSlow11);
		double fSlow51 = (fConst34 * (fSlow50 * fSlow12));
		double fSlow52 = (fSlow16 * fSlow49);
		double fSlow53 = (fConst35 * fSlow52);
		double fSlow54 = (fSlow33 * fSlow46);
		double fSlow55 = (fConst36 * (fSlow54 * fSlow9));
		double fSlow56 = ((2.0 * fSlow33) - fSlow31);
		double fSlow57 = (fSlow32 + fSlow56);
		double fSlow58 = ((2.0 * (fSlow57 * fSlow15)) + -1.0);
		double fSlow59 = (fConst29 * (fSlow58 * fSlow44));
		double fSlow60 = ((6.5 * fSlow46) + -812.43300030876912);
		double fSlow61 = (((fSlow23 * fSlow60) * fSlow11) * fSlow7);
		double fSlow62 = (fConst39 * ((fSlow61 * fSlow9) * fSlow12));
		double fSlow63 = ((3.25 * fSlow49) - (2.6666666666666665 * fSlow35));
		double fSlow64 = ((fSlow16 * fSlow63) * fSlow7);
		double fSlow65 = (fConst42 * fSlow64);
		double fSlow66 = (fSlow58 * fSlow7);
		double fSlow67 = (fConst45 * (fSlow66 * fSlow44));
		double fSlow68 = ((7041.0860026759983 * fSlow8) + -469.40573351173322);
		double fSlow69 = (fSlow43 * fSlow68);
		double fSlow70 = (fConst48 * ((fSlow69 * fSlow44) * fSlow12));
		double fSlow71 = ((3.0 * (fSlow63 * fSlow8)) - (2.25 * fSlow49));
		double fSlow72 = (fSlow71 * fSlow11);
		double fSlow73 = (fConst50 * (fSlow72 * fSlow12));
		double fSlow74 = (fSlow16 * fSlow71);
		double fSlow75 = (fConst51 * fSlow74);
		double fSlow76 = ((5.0 * (fSlow60 * fSlow8)) - (5.5 * fSlow46));
		double fSlow77 = (fSlow33 * fSlow76);
		double fSlow78 = (fConst53 * (fSlow77 * fSlow9));
		double fSlow79 = (fSlow58 * fSlow68);
		double fSlow80 = (fConst48 * (fSlow79 * fSlow44));
		double fSlow81 = ((2.0 * fSlow58) - fSlow56);
		double fSlow82 = (fSlow57 - fSlow81);
		double fSlow83 = (1.0 - (2.0 * (fSlow82 * fSlow15)));
		double fSlow84 = mydsp_faustpower3_f(fSlow9);
		double fSlow85 = (fConst57 * (fSlow83 * fSlow84));
		double fSlow86 = ((8.5 * fSlow68) + -6571.6802691642652);
		double fSlow87 = ((fSlow43 * fSlow86) * fSlow7);
		double fSlow88 = (fConst60 * ((fSlow87 * fSlow44) * fSlow12));
		double fSlow89 = ((2.833333333333333 * fSlow71) - (2.0 * fSlow63));
		double fSlow90 = ((fSlow16 * fSlow89) * fSlow7);
		double fSlow91 = (fConst62 * fSlow90);
		double fSlow92 = ((fSlow58 * fSlow86) * fSlow7);
		double fSlow93 = (fConst60 * (fSlow92 * fSlow44));
		double fSlow94 = ((2.0 * fSlow43) + (4.0 * (fSlow40 * fSlow11)));
		double fSlow95 = ((fSlow94 * fSlow11) + -1.0);
		double fSlow96 = (fSlow42 + fSlow95);
		double fSlow97 = ((46651.708284396875 * fSlow8) + -2455.3530675998354);
		double fSlow98 = ((fSlow96 * fSlow97) * fSlow11);
		double fSlow99 = (fConst65 * ((fSlow98 * fSlow84) * fSlow12));
		double fSlow100 = ((4.25 * fSlow76) - (4.0 * fSlow60));
		double fSlow101 = ((3.8000000000000003 * (fSlow100 * fSlow8)) - (3.25 * fSlow76));
		double fSlow102 = ((fSlow23 * fSlow101) * fSlow11);
		double fSlow103 = (fConst67 * ((fSlow102 * fSlow9) * fSlow12));
		double fSlow104 = ((2.714285714285714 * (fSlow89 * fSlow8)) - (1.8333333333333333 * fSlow71));
		double fSlow105 = (fSlow16 * fSlow104);
		double fSlow106 = (fConst69 * fSlow105);
		double fSlow107 = (fSlow33 * fSlow101);
		double fSlow108 = (fConst70 * (fSlow107 * fSlow9));
		double fSlow109 = ((6.333333333333333 * (fSlow86 * fSlow8)) - (7.5 * fSlow68));
		double fSlow110 = (fSlow58 * fSlow109);
		double fSlow111 = (fConst72 * (fSlow110 * fSlow44));
		double fSlow112 = (fSlow83 * fSlow97);
		double fSlow113 = (fConst73 * (fSlow112 * fSlow84));
		double fSlow114 = ((2.0 * fSlow83) - fSlow81);
		double fSlow115 = ((2.0 * ((fSlow82 + fSlow114) * fSlow15)) + -1.0);
		double fSlow116 = mydsp_faustpower4_f(fSlow9);
		double fSlow117 = (fConst75 * (fSlow115 * fSlow116));
		double fSlow118 = pow(fSlow9, 2.5);
		double fSlow119 = (fConst76 * fSlow118);
		double fSlow120 = (fSlow41 * fSlow12);
		double fSlow121 = (fSlow56 * fSlow11);
		double fSlow122 = (fSlow41 * fSlow7);
		double fSlow123 = (fConst80 * (fSlow122 * fSlow12));
		double fSlow124 = (fSlow121 * fSlow7);
		double fSlow125 = (fConst80 * fSlow124);
		double fSlow126 = ((2581.7315343145328 * fSlow8) + -198.59473340881021);
		double fSlow127 = (fSlow41 * fSlow126);
		double fSlow128 = (fConst82 * (fSlow127 * fSlow12));
		double fSlow129 = ((fSlow56 * fSlow126) * fSlow11);
		double fSlow130 = (fConst82 * fSlow129);
		double fSlow131 = ((7.5 * fSlow126) + -2383.1368009057223);
		double fSlow132 = ((fSlow41 * fSlow131) * fSlow7);
		double fSlow133 = (fConst84 * (fSlow132 * fSlow12));
		double fSlow134 = (((fSlow56 * fSlow131) * fSlow11) * fSlow7);
		double fSlow135 = (fConst84 * fSlow134);
		double fSlow136 = ((5.6666666666666661 * (fSlow131 * fSlow8)) - (6.5 * fSlow126));
		double fSlow137 = (fSlow41 * fSlow136);
		double fSlow138 = (fConst86 * (fSlow137 * fSlow12));
		double fSlow139 = ((fSlow56 * fSlow136) * fSlow11);
		double fSlow140 = (fConst86 * fSlow139);
		double fSlow141 = ((4.75 * fSlow136) - (4.6666666666666661 * fSlow131));
		double fSlow142 = ((fSlow41 * fSlow141) * fSlow7);
		double fSlow143 = (fConst88 * (fSlow142 * fSlow12));
		double fSlow144 = (((fSlow56 * fSlow141) * fSlow11) * fSlow7);
		double fSlow145 = (fConst88 * fSlow144);
		double fSlow146 = pow(fSlow9, 1.5);
		double fSlow147 = (fConst89 * fSlow146);
		double fSlow148 = (fSlow22 * fSlow12);
		double fSlow149 = (fSlow31 * fSlow11);
		double fSlow150 = (fSlow22 * fSlow7);
		double fSlow151 = (fConst93 * (fSlow150 * fSlow12));
		double fSlow152 = (fSlow149 * fSlow7);
		double fSlow153 = (fConst93 * fSlow152);
		double fSlow154 = ((284.35155010806915 * fSlow8) + -31.594616678674353);
		double fSlow155 = (fSlow22 * fSlow154);
		double fSlow156 = (fConst95 * (fSlow155 * fSlow12));
		double fSlow157 = ((fSlow31 * fSlow154) * fSlow11);
		double fSlow158 = (fConst95 * fSlow157);
		double fSlow159 = ((5.5 * fSlow154) + -252.75693342939482);
		double fSlow160 = ((fSlow22 * fSlow159) * fSlow7);
		double fSlow161 = (fConst97 * (fSlow160 * fSlow12));
		double fSlow162 = (((fSlow31 * fSlow159) * fSlow11) * fSlow7);
		double fSlow163 = (fConst97 * fSlow162);
		double fSlow164 = ((4.333333333333333 * (fSlow159 * fSlow8)) - (4.5 * fSlow154));
		double fSlow165 = (fSlow22 * fSlow164);
		double fSlow166 = (fConst99 * (fSlow165 * fSlow12));
		double fSlow167 = ((fSlow31 * fSlow164) * fSlow11);
		double fSlow168 = (fConst99 * fSlow167);
		double fSlow169 = ((3.75 * fSlow164) - (3.333333333333333 * fSlow159));
		double fSlow170 = ((fSlow22 * fSlow169) * fSlow7);
		double fSlow171 = (fConst101 * (fSlow170 * fSlow12));
		double fSlow172 = (((fSlow31 * fSlow169) * fSlow11) * fSlow7);
		double fSlow173 = (fConst101 * fSlow172);
		double fSlow174 = ((3.4000000000000004 * (fSlow169 * fSlow8)) - (2.75 * fSlow164));
		double fSlow175 = (fSlow22 * fSlow174);
		double fSlow176 = (fConst103 * (fSlow175 * fSlow12));
		double fSlow177 = ((fSlow31 * fSlow174) * fSlow11);
		double fSlow178 = (fConst103 * fSlow177);
		double fSlow179 = ((3.1666666666666665 * fSlow174) - (2.4000000000000004 * fSlow169));
		double fSlow180 = ((fSlow22 * fSlow179) * fSlow7);
		double fSlow181 = (fConst105 * (fSlow180 * fSlow12));
		double fSlow182 = (((fSlow31 * fSlow179) * fSlow11) * fSlow7);
		double fSlow183 = (fConst105 * fSlow182);
		double fSlow184 = pow(fSlow9, 0.5);
		double fSlow185 = (fConst106 * fSlow184);
		double fSlow186 = (fConst110 * (fSlow7 * fSlow12));
		double fSlow187 = (fConst110 * fSlow18);
		double fSlow188 = ((16.925687506432688 * fSlow8) + -3.3851375012865379);
		double fSlow189 = (fConst112 * (fSlow188 * fSlow12));
		double fSlow190 = (fSlow188 * fSlow11);
		double fSlow191 = (fConst112 * fSlow190);
		double fSlow192 = ((3.5 * fSlow188) + -13.540550005146152);
		double fSlow193 = (fSlow192 * fSlow7);
		double fSlow194 = (fConst114 * (fSlow193 * fSlow12));
		double fSlow195 = ((fSlow192 * fSlow11) * fSlow7);
		double fSlow196 = (fConst114 * fSlow195);
		double fSlow197 = ((3.0 * (fSlow192 * fSlow8)) - (2.5 * fSlow188));
		double fSlow198 = (fConst116 * (fSlow197 * fSlow12));
		double fSlow199 = (fSlow197 * fSlow11);
		double fSlow200 = (fConst116 * fSlow199);
		double fSlow201 = ((2.75 * fSlow197) - (2.0 * fSlow192));
		double fSlow202 = (fSlow201 * fSlow7);
		double fSlow203 = (fConst118 * (fSlow202 * fSlow12));
		double fSlow204 = ((fSlow201 * fSlow11) * fSlow7);
		double fSlow205 = (fConst118 * fSlow204);
		double fSlow206 = ((2.6000000000000001 * (fSlow201 * fSlow8)) - (1.75 * fSlow197));
		double fSlow207 = (fConst120 * (fSlow206 * fSlow12));
		double fSlow208 = (fSlow206 * fSlow11);
		double fSlow209 = (fConst120 * fSlow208);
		double fSlow210 = ((2.5 * fSlow206) - (1.6000000000000001 * fSlow201));
		double fSlow211 = (fSlow210 * fSlow7);
		double fSlow212 = (fConst122 * (fSlow211 * fSlow12));
		double fSlow213 = ((fSlow210 * fSlow11) * fSlow7);
		double fSlow214 = (fConst122 * fSlow213);
		double fSlow215 = ((2.4285714285714284 * (fSlow210 * fSlow8)) - (1.5 * fSlow206));
		double fSlow216 = (fConst124 * (fSlow215 * fSlow12));
		double fSlow217 = (fSlow215 * fSlow11);
		double fSlow218 = (fConst124 * fSlow217);
		double fSlow219 = ((2.375 * fSlow215) - (1.4285714285714284 * fSlow210));
		double fSlow220 = (fSlow219 * fSlow7);
		double fSlow221 = (fConst126 * (fSlow220 * fSlow12));
		double fSlow222 = ((fSlow219 * fSlow11) * fSlow7);
		double fSlow223 = (fConst126 * fSlow222);
		double fSlow224 = ((3.0 * fSlow8) + -1.0);
		double fSlow225 = ((2.5 * fSlow224) + -2.0);
		double fSlow226 = ((2.333333333333333 * (fSlow225 * fSlow8)) - (1.5 * fSlow224));
		double fSlow227 = ((2.25 * fSlow226) - (1.3333333333333333 * fSlow225));
		double fSlow228 = ((2.2000000000000002 * (fSlow227 * fSlow8)) - (1.25 * fSlow226));
		double fSlow229 = ((2.1666666666666665 * fSlow228) - (1.2000000000000002 * fSlow227));
		double fSlow230 = ((2.1428571428571428 * (fSlow229 * fSlow8)) - (1.1666666666666665 * fSlow228));
		double fSlow231 = ((2.125 * fSlow230) - (1.1428571428571428 * fSlow229));
		double fSlow232 = ((2.1111111111111112 * (fSlow231 * fSlow8)) - (1.125 * fSlow230));
		double fSlow233 = (0.45825756949558399 * fSlow232);
		double fSlow234 = pow(fSlow9, 3.5);
		double fSlow235 = (fSlow95 * fSlow234);
		double fSlow236 = (fConst130 * fSlow235);
		double fSlow237 = (((fSlow63 * fSlow11) * fSlow7) * fSlow9);
		double fSlow238 = (fConst131 * fSlow237);
		double fSlow239 = (fSlow96 * fSlow11);
		double fSlow240 = (fSlow239 * fSlow116);
		double fSlow241 = (fConst132 * fSlow240);
		double fSlow242 = (((fSlow23 * fSlow76) * fSlow11) * fSlow44);
		double fSlow243 = (fConst133 * fSlow242);
		double fSlow244 = ((((4.0 * fSlow239) - fSlow94) * fSlow11) + 1.0);
		double fSlow245 = pow(fSlow9, 4.5);
		double fSlow246 = (fSlow244 * fSlow245);
		double fSlow247 = (fConst137 * fSlow246);
		double fSlow248 = ((((fSlow23 * fSlow100) * fSlow11) * fSlow7) * fSlow44);
		double fSlow249 = (fConst139 * fSlow248);
		double fSlow250 = mydsp_faustpower5_f(fSlow9);
		double fSlow251 = ((0.0 - (2.0 * ((fSlow96 - fSlow244) * fSlow11))) * fSlow250);
		double fSlow252 = (fConst75 * fSlow251);
		double fSlow253 = ((fSlow43 * fSlow109) * fSlow84);
		double fSlow254 = (fConst72 * fSlow253);
		double fSlow255 = ((fSlow104 * fSlow11) * fSlow9);
		double fSlow256 = (fConst140 * fSlow255);
		double fSlow257 = (1.1180339887498949 * fSlow224);
		double fSlow258 = (0.75 * fSlow226);
		double fSlow259 = (0.60092521257733145 * fSlow228);
		double fSlow260 = (0.51538820320220757 * fSlow230);
		double fSlow261 = (0.88191710368819687 * fSlow225);
		double fSlow262 = ((fSlow24 * fSlow44) * fSlow12);
		double fSlow263 = (fConst142 * fSlow262);
		double fSlow264 = (0.66332495807108005 * fSlow227);
		double fSlow265 = (fSlow33 * fSlow44);
		double fSlow266 = (fConst143 * fSlow265);
		double fSlow267 = ((fSlow43 * fSlow84) * fSlow12);
		double fSlow268 = (fConst45 * fSlow267);
		double fSlow269 = (0.55328333517248807 * fSlow229);
		double fSlow270 = (fSlow33 * fSlow60);
		double fSlow271 = (fConst144 * (fSlow270 * fSlow44));
		double fSlow272 = (fSlow240 * fSlow12);
		double fSlow273 = (fConst147 * fSlow272);
		double fSlow274 = (fSlow89 * fSlow11);
		double fSlow275 = (fConst148 * ((fSlow274 * fSlow9) * fSlow12));
		double fSlow276 = (0.48432210483785265 * fSlow231);
		double fSlow277 = (fSlow33 * fSlow100);
		double fSlow278 = (fConst149 * (fSlow277 * fSlow44));
		double fSlow279 = (fSlow83 * fSlow116);
		double fSlow280 = (fConst150 * fSlow279);
		double fSlow281 = (fConst128 * fSlow234);
		double fSlow282 = (fSlow81 * fSlow11);
		double fSlow283 = (fConst151 * fSlow282);
		double fSlow284 = (fSlow95 * fSlow7);
		double fSlow285 = (fConst153 * (fSlow284 * fSlow12));
		double fSlow286 = (fSlow282 * fSlow7);
		double fSlow287 = (fConst153 * fSlow286);
		double fSlow288 = ((18415.148006998763 * fSlow8) + -1083.244000411692);
		double fSlow289 = (fSlow95 * fSlow288);
		double fSlow290 = (fConst155 * (fSlow289 * fSlow12));
		double fSlow291 = ((fSlow81 * fSlow288) * fSlow11);
		double fSlow292 = (fConst155 * fSlow291);
		double fSlow293 = ((9.5 * fSlow288) + -17331.904006587072);
		double fSlow294 = ((fSlow95 * fSlow293) * fSlow7);
		double fSlow295 = (fConst157 * (fSlow294 * fSlow12));
		double fSlow296 = (((fSlow81 * fSlow293) * fSlow11) * fSlow7);
		double fSlow297 = (fConst157 * fSlow296);
		double fSlow298 = (fConst135 * fSlow245);
		double fSlow299 = (fSlow114 * fSlow11);
		double fSlow300 = (fConst158 * fSlow299);
		double fSlow301 = (fSlow244 * fSlow7);
		double fSlow302 = (fConst160 * (fSlow301 * fSlow12));
		double fSlow303 = (fSlow299 * fSlow7);
		double fSlow304 = (fConst160 * fSlow303);
		double fSlow305 = (fConst161 * (fSlow184 * fSlow12));
		double fSlow306 = (0.13783222385544802 * fSlow7);
		double fSlow307 = (fConst161 * (fSlow11 * fSlow184));
		double fSlow308 = (fConst163 * ((fSlow11 * fSlow9) * fSlow12));
		double fSlow309 = (fConst164 * ((fSlow7 * fSlow184) * fSlow12));
		double fSlow310 = (0.088970317927147144 * fSlow224);
		double fSlow311 = (fConst164 * (fSlow18 * fSlow184));
		double fSlow312 = (fConst165 * (fSlow16 * fSlow9));
		double fSlow313 = (fConst166 * ((fSlow22 * fSlow146) * fSlow12));
		double fSlow314 = (fConst168 * ((fSlow18 * fSlow9) * fSlow12));
		double fSlow315 = (fConst169 * ((fSlow188 * fSlow184) * fSlow12));
		double fSlow316 = (0.070180733224632072 * (fSlow225 * fSlow7));
		double fSlow317 = (fConst169 * (fSlow190 * fSlow184));
		double fSlow318 = (fConst170 * (fSlow20 * fSlow9));
		double fSlow319 = (fConst166 * (fSlow149 * fSlow146));
		double fSlow320 = (fConst171 * fSlow262);
		double fSlow321 = (fConst172 * ((fSlow150 * fSlow146) * fSlow12));
		double fSlow322 = (fConst173 * ((fSlow27 * fSlow9) * fSlow12));
		double fSlow323 = (fConst174 * ((fSlow193 * fSlow184) * fSlow12));
		double fSlow324 = (0.059683103659460751 * fSlow226);
		double fSlow325 = (fConst174 * (fSlow195 * fSlow184));
		double fSlow326 = (fConst175 * (fSlow29 * fSlow9));
		double fSlow327 = (fConst172 * (fSlow152 * fSlow146));
		double fSlow328 = (fConst176 * fSlow265);
		double fSlow329 = (fConst177 * ((fSlow41 * fSlow118) * fSlow12));
		double fSlow330 = (fConst178 * (((fSlow24 * fSlow7) * fSlow44) * fSlow12));
		double fSlow331 = (fConst179 * ((fSlow155 * fSlow146) * fSlow12));
		double fSlow332 = (fConst180 * ((fSlow36 * fSlow9) * fSlow12));
		double fSlow333 = (fConst181 * ((fSlow197 * fSlow184) * fSlow12));
		double fSlow334 = (0.052785722976618307 * (fSlow227 * fSlow7));
		double fSlow335 = (fConst181 * (fSlow199 * fSlow184));
		double fSlow336 = (fConst182 * (fSlow38 * fSlow9));
		double fSlow337 = (fConst179 * (fSlow157 * fSlow146));
		double fSlow338 = (fConst183 * ((fSlow33 * fSlow7) * fSlow44));
		double fSlow339 = (fConst177 * (fSlow121 * fSlow118));
		double fSlow340 = (fConst184 * fSlow267);
		double fSlow341 = (fConst185 * ((fSlow122 * fSlow118) * fSlow12));
		double fSlow342 = (fConst186 * ((fSlow47 * fSlow44) * fSlow12));
		double fSlow343 = (fConst187 * ((fSlow160 * fSlow146) * fSlow12));
		double fSlow344 = (fConst188 * ((fSlow50 * fSlow9) * fSlow12));
		double fSlow345 = (fConst189 * ((fSlow202 * fSlow184) * fSlow12));
		double fSlow346 = (0.047820109005115151 * fSlow228);
		double fSlow347 = (fConst189 * (fSlow204 * fSlow184));
		double fSlow348 = (fConst190 * (fSlow52 * fSlow9));
		double fSlow349 = (fConst187 * (fSlow162 * fSlow146));
		double fSlow350 = (fConst191 * (fSlow54 * fSlow44));
		double fSlow351 = (fConst185 * (fSlow124 * fSlow118));
		double fSlow352 = (fConst184 * (fSlow58 * fSlow84));
		double fSlow353 = (fConst192 * (fSlow235 * fSlow12));
		double fSlow354 = (fConst193 * (((fSlow43 * fSlow7) * fSlow84) * fSlow12));
		double fSlow355 = (fConst194 * ((fSlow127 * fSlow118) * fSlow12));
		double fSlow356 = (fConst195 * ((fSlow61 * fSlow44) * fSlow12));
		double fSlow357 = (fConst196 * ((fSlow165 * fSlow146) * fSlow12));
		double fSlow358 = (fConst197 * (fSlow237 * fSlow12));
		double fSlow359 = (fConst198 * ((fSlow206 * fSlow184) * fSlow12));
		double fSlow360 = (0.044028888861535699 * (fSlow229 * fSlow7));
		double fSlow361 = (fConst198 * (fSlow208 * fSlow184));
		double fSlow362 = (fConst199 * (fSlow64 * fSlow9));
		double fSlow363 = (fConst196 * (fSlow167 * fSlow146));
		double fSlow364 = (fConst200 * ((fSlow270 * fSlow7) * fSlow44));
		double fSlow365 = (fConst194 * (fSlow129 * fSlow118));
		double fSlow366 = (fConst193 * (fSlow66 * fSlow84));
		double fSlow367 = (fConst192 * (fSlow282 * fSlow234));
		double fSlow368 = (fConst201 * fSlow272);
		double fSlow369 = (fConst202 * ((fSlow284 * fSlow234) * fSlow12));
		double fSlow370 = (fConst203 * ((fSlow69 * fSlow84) * fSlow12));
		double fSlow371 = (fConst204 * ((fSlow132 * fSlow118) * fSlow12));
		double fSlow372 = (fConst205 * (fSlow242 * fSlow12));
		double fSlow373 = (fConst206 * ((fSlow170 * fSlow146) * fSlow12));
		double fSlow374 = (fConst207 * ((fSlow72 * fSlow9) * fSlow12));
		double fSlow375 = (fConst208 * ((fSlow211 * fSlow184) * fSlow12));
		double fSlow376 = (0.041013290075440773 * fSlow230);
		double fSlow377 = (fConst208 * (fSlow213 * fSlow184));
		double fSlow378 = (fConst209 * (fSlow74 * fSlow9));
		double fSlow379 = (fConst206 * (fSlow172 * fSlow146));
		double fSlow380 = (fConst210 * (fSlow77 * fSlow44));
		double fSlow381 = (fConst204 * (fSlow134 * fSlow118));
		double fSlow382 = (fConst203 * (fSlow79 * fSlow84));
		double fSlow383 = (fConst202 * (fSlow286 * fSlow234));
		double fSlow384 = (fConst211 * fSlow279);
		double fSlow385 = (fConst212 * (fSlow246 * fSlow12));
		double fSlow386 = (fConst213 * (((fSlow239 * fSlow7) * fSlow116) * fSlow12));
		double fSlow387 = (fConst214 * ((fSlow289 * fSlow234) * fSlow12));
		double fSlow388 = (fConst215 * ((fSlow87 * fSlow84) * fSlow12));
		double fSlow389 = (fConst216 * ((fSlow137 * fSlow118) * fSlow12));
		double fSlow390 = (fConst217 * (fSlow248 * fSlow12));
		double fSlow391 = (fConst218 * ((fSlow175 * fSlow146) * fSlow12));
		double fSlow392 = (fConst219 * (((fSlow274 * fSlow7) * fSlow9) * fSlow12));
		double fSlow393 = (fConst220 * ((fSlow215 * fSlow184) * fSlow12));
		double fSlow394 = (0.038541128516807702 * (fSlow231 * fSlow7));
		double fSlow395 = (fConst220 * (fSlow217 * fSlow184));
		double fSlow396 = (fConst221 * (fSlow90 * fSlow9));
		double fSlow397 = (fConst218 * (fSlow177 * fSlow146));
		double fSlow398 = (fConst222 * ((fSlow277 * fSlow7) * fSlow44));
		double fSlow399 = (fConst216 * (fSlow139 * fSlow118));
		double fSlow400 = (fConst215 * (fSlow92 * fSlow84));
		double fSlow401 = (fConst214 * (fSlow291 * fSlow234));
		double fSlow402 = (fConst223 * ((fSlow83 * fSlow7) * fSlow116));
		double fSlow403 = (fConst212 * (fSlow299 * fSlow245));
		double fSlow404 = (fConst224 * (fSlow251 * fSlow12));
		double fSlow405 = (fConst225 * ((fSlow301 * fSlow245) * fSlow12));
		double fSlow406 = (fConst226 * ((fSlow98 * fSlow116) * fSlow12));
		double fSlow407 = (fConst227 * ((fSlow294 * fSlow234) * fSlow12));
		double fSlow408 = (fConst228 * (fSlow253 * fSlow12));
		double fSlow409 = (fConst229 * ((fSlow142 * fSlow118) * fSlow12));
		double fSlow410 = (fConst230 * ((fSlow102 * fSlow44) * fSlow12));
		double fSlow411 = (fConst231 * ((fSlow180 * fSlow146) * fSlow12));
		double fSlow412 = (fConst232 * (fSlow255 * fSlow12));
		double fSlow413 = (fConst233 * ((fSlow220 * fSlow184) * fSlow12));
		double fSlow414 = (0.03646697869724997 * fSlow232);
		double fSlow415 = (fConst233 * (fSlow222 * fSlow184));
		double fSlow416 = (fConst234 * (fSlow105 * fSlow9));
		double fSlow417 = (fConst231 * (fSlow182 * fSlow146));
		double fSlow418 = (fConst235 * (fSlow107 * fSlow44));
		double fSlow419 = (fConst229 * (fSlow144 * fSlow118));
		double fSlow420 = (fConst228 * (fSlow110 * fSlow84));
		double fSlow421 = (fConst227 * (fSlow296 * fSlow234));
		double fSlow422 = (fConst236 * (fSlow112 * fSlow116));
		double fSlow423 = (fConst225 * (fSlow303 * fSlow245));
		double fSlow424 = (fConst224 * (fSlow115 * fSlow250));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec0[0] = (iSlow4?0.0:min(fSlow5, (fRec0[1] + 1.0)));
			double fTemp0 = double(input0[i]);
			fRec1[0] = (iSlow6?fSlow5:max(0.0, (fRec1[1] + -1.0)));
			fRec2[0] = (fSlow14 + (0.999 * fRec2[1]));
			fRec3[0] = (iSlow4?fSlow5:max(0.0, (fRec3[1] + -1.0)));
			double fTemp1 = (fRec2[0] * fRec3[0]);
			double fTemp2 = double(input4[i]);
			fRec4[0] = (iSlow6?0.0:min(fSlow5, (fRec4[1] + 1.0)));
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
			double fTemp22 = double(input66[i]);
			double fTemp23 = double(input70[i]);
			double fTemp24 = double(input74[i]);
			double fTemp25 = double(input76[i]);
			double fTemp26 = double(input78[i]);
			double fTemp27 = double(input80[i]);
			double fTemp28 = double(input84[i]);
			double fTemp29 = double(input92[i]);
			double fTemp30 = double(input96[i]);
			double fTemp31 = double(input102[i]);
			double fTemp32 = double(input106[i]);
			double fTemp33 = double(input112[i]);
			double fTemp34 = double(input114[i]);
			double fTemp35 = double(input116[i]);
			double fTemp36 = double(input118[i]);
			double fTemp37 = double(input120[i]);
			double fTemp38 = double(input25[i]);
			double fTemp39 = double(input35[i]);
			double fTemp40 = double(input37[i]);
			double fTemp41 = double(input47[i]);
			double fTemp42 = double(input51[i]);
			double fTemp43 = double(input61[i]);
			double fTemp44 = double(input67[i]);
			double fTemp45 = double(input77[i]);
			double fTemp46 = double(input85[i]);
			double fTemp47 = double(input95[i]);
			double fTemp48 = double(input105[i]);
			double fTemp49 = double(input115[i]);
			double fTemp50 = double(input9[i]);
			double fTemp51 = double(input15[i]);
			double fTemp52 = double(input17[i]);
			double fTemp53 = double(input23[i]);
			double fTemp54 = double(input27[i]);
			double fTemp55 = double(input33[i]);
			double fTemp56 = double(input39[i]);
			double fTemp57 = double(input45[i]);
			double fTemp58 = double(input53[i]);
			double fTemp59 = double(input59[i]);
			double fTemp60 = double(input69[i]);
			double fTemp61 = double(input75[i]);
			double fTemp62 = double(input87[i]);
			double fTemp63 = double(input93[i]);
			double fTemp64 = double(input107[i]);
			double fTemp65 = double(input113[i]);
			double fTemp66 = double(input1[i]);
			double fTemp67 = double(input3[i]);
			double fTemp68 = double(input5[i]);
			double fTemp69 = double(input7[i]);
			double fTemp70 = double(input11[i]);
			double fTemp71 = double(input13[i]);
			double fTemp72 = double(input19[i]);
			double fTemp73 = double(input21[i]);
			double fTemp74 = double(input29[i]);
			double fTemp75 = double(input31[i]);
			double fTemp76 = double(input41[i]);
			double fTemp77 = double(input43[i]);
			double fTemp78 = double(input55[i]);
			double fTemp79 = double(input57[i]);
			double fTemp80 = double(input71[i]);
			double fTemp81 = double(input73[i]);
			double fTemp82 = double(input89[i]);
			double fTemp83 = double(input91[i]);
			double fTemp84 = double(input109[i]);
			double fTemp85 = double(input111[i]);
			double fTemp86 = double(input110[i]);
			double fTemp87 = double(input49[i]);
			double fTemp88 = double(input54[i]);
			double fTemp89 = double(input64[i]);
			double fTemp90 = double(input68[i]);
			double fTemp91 = double(input81[i]);
			double fTemp92 = double(input86[i]);
			double fTemp93 = double(input100[i]);
			double fTemp94 = double(input104[i]);
			double fTemp95 = double(input108[i]);
			double fTemp96 = double(input6[i]);
			double fTemp97 = double(input20[i]);
			double fTemp98 = double(input42[i]);
			double fTemp99 = double(input72[i]);
			double fTemp100 = double(input2[i]);
			double fTemp101 = double(input12[i]);
			double fTemp102 = double(input26[i]);
			double fTemp103 = double(input30[i]);
			double fTemp104 = double(input34[i]);
			double fTemp105 = double(input50[i]);
			double fTemp106 = double(input56[i]);
			double fTemp107 = double(input60[i]);
			double fTemp108 = double(input82[i]);
			double fTemp109 = double(input88[i]);
			double fTemp110 = double(input90[i]);
			double fTemp111 = double(input94[i]);
			double fTemp112 = double(input98[i]);
			double fTemp113 = double(input63[i]);
			double fTemp114 = double(input65[i]);
			double fTemp115 = double(input79[i]);
			double fTemp116 = double(input83[i]);
			double fTemp117 = double(input97[i]);
			double fTemp118 = double(input103[i]);
			double fTemp119 = double(input117[i]);
			double fTemp120 = double(input99[i]);
			double fTemp121 = double(input101[i]);
			double fTemp122 = double(input119[i]);
			double fTemp123 = ((fSlow9 * ((((((((((((((((((((((((((((((((fConst2 * (((fConst5 * ((fSlow13 * (iSlow1?(fSlow3 * (fTemp3 * fTemp2)):(fSlow3 * (fTemp1 * fTemp2)))) + (fSlow17 * (iSlow1?(fSlow3 * (fTemp3 * fTemp4)):(fSlow3 * (fTemp1 * fTemp4)))))) + (fSlow19 * (iSlow1?(fSlow3 * (fTemp3 * fTemp5)):(fSlow3 * (fTemp1 * fTemp5))))) + (fSlow21 * (iSlow1?(fSlow3 * (fTemp3 * fTemp6)):(fSlow3 * (fTemp1 * fTemp6)))))) + (fSlow25 * (iSlow1?(fSlow3 * (fTemp3 * fTemp7)):(fSlow3 * (fTemp1 * fTemp7))))) + (fSlow28 * (iSlow1?(fSlow3 * (fTemp3 * fTemp8)):(fSlow3 * (fTemp1 * fTemp8))))) + (fSlow30 * (iSlow1?(fSlow3 * (fTemp3 * fTemp9)):(fSlow3 * (fTemp1 * fTemp9))))) + (fSlow34 * (iSlow1?(fSlow3 * (fTemp3 * fTemp10)):(fSlow3 * (fTemp1 * fTemp10))))) + (fSlow37 * (iSlow1?(fSlow3 * (fTemp3 * fTemp11)):(fSlow3 * (fTemp1 * fTemp11))))) + (fSlow39 * (iSlow1?(fSlow3 * (fTemp3 * fTemp12)):(fSlow3 * (fTemp1 * fTemp12))))) + (fSlow45 * (iSlow1?(fSlow3 * (fTemp3 * fTemp13)):(fSlow3 * (fTemp1 * fTemp13))))) + (fSlow48 * (iSlow1?(fSlow3 * (fTemp3 * fTemp14)):(fSlow3 * (fTemp1 * fTemp14))))) + (fSlow51 * (iSlow1?(fSlow3 * (fTemp3 * fTemp15)):(fSlow3 * (fTemp1 * fTemp15))))) + (fSlow53 * (iSlow1?(fSlow3 * (fTemp3 * fTemp16)):(fSlow3 * (fTemp1 * fTemp16))))) + (fSlow55 * (iSlow1?(fSlow3 * (fTemp3 * fTemp17)):(fSlow3 * (fTemp1 * fTemp17))))) + (fSlow59 * (iSlow1?(fSlow3 * (fTemp3 * fTemp18)):(fSlow3 * (fTemp1 * fTemp18))))) + (fSlow62 * (iSlow1?(fSlow3 * (fTemp3 * fTemp19)):(fSlow3 * (fTemp1 * fTemp19))))) + (fSlow65 * (iSlow1?(fSlow3 * (fTemp3 * fTemp20)):(fSlow3 * (fTemp1 * fTemp20))))) + (fSlow67 * (iSlow1?(fSlow3 * (fTemp3 * fTemp21)):(fSlow3 * (fTemp1 * fTemp21))))) + (fSlow70 * (iSlow1?(fSlow3 * (fTemp3 * fTemp22)):(fSlow3 * (fTemp1 * fTemp22))))) + (fSlow73 * (iSlow1?(fSlow3 * (fTemp3 * fTemp23)):(fSlow3 * (fTemp1 * fTemp23))))) + (fSlow75 * (iSlow1?(fSlow3 * (fTemp3 * fTemp24)):(fSlow3 * (fTemp1 * fTemp24))))) + (fSlow78 * (iSlow1?(fSlow3 * (fTemp3 * fTemp25)):(fSlow3 * (fTemp1 * fTemp25))))) + (fSlow80 * (iSlow1?(fSlow3 * (fTemp3 * fTemp26)):(fSlow3 * (fTemp1 * fTemp26))))) + (fSlow85 * (iSlow1?(fSlow3 * (fTemp3 * fTemp27)):(fSlow3 * (fTemp1 * fTemp27))))) + (fSlow88 * (iSlow1?(fSlow3 * (fTemp3 * fTemp28)):(fSlow3 * (fTemp1 * fTemp28))))) + (fSlow91 * (iSlow1?(fSlow3 * (fTemp3 * fTemp29)):(fSlow3 * (fTemp1 * fTemp29))))) + (fSlow93 * (iSlow1?(fSlow3 * (fTemp3 * fTemp30)):(fSlow3 * (fTemp1 * fTemp30))))) + (fSlow99 * (iSlow1?(fSlow3 * (fTemp3 * fTemp31)):(fSlow3 * (fTemp1 * fTemp31))))) + (fSlow103 * (iSlow1?(fSlow3 * (fTemp3 * fTemp32)):(fSlow3 * (fTemp1 * fTemp32))))) + (fSlow106 * (iSlow1?(fSlow3 * (fTemp3 * fTemp33)):(fSlow3 * (fTemp1 * fTemp33))))) + (fSlow108 * (iSlow1?(fSlow3 * (fTemp3 * fTemp34)):(fSlow3 * (fTemp1 * fTemp34))))) + (fSlow111 * (iSlow1?(fSlow3 * (fTemp3 * fTemp35)):(fSlow3 * (fTemp1 * fTemp35))))) + (fSlow113 * (iSlow1?(fSlow3 * (fTemp3 * fTemp36)):(fSlow3 * (fTemp1 * fTemp36))))) + (fSlow117 * (iSlow1?(fSlow3 * (fTemp3 * fTemp37)):(fSlow3 * (fTemp1 * fTemp37)))))) + ((((fSlow119 * (((((((((((fConst78 * ((fSlow120 * (iSlow1?(fSlow3 * (fTemp3 * fTemp38)):(fSlow3 * (fTemp1 * fTemp38)))) + (fSlow121 * (iSlow1?(fSlow3 * (fTemp3 * fTemp39)):(fSlow3 * (fTemp1 * fTemp39)))))) + (fSlow123 * (iSlow1?(fSlow3 * (fTemp3 * fTemp40)):(fSlow3 * (fTemp1 * fTemp40))))) + (fSlow125 * (iSlow1?(fSlow3 * (fTemp3 * fTemp41)):(fSlow3 * (fTemp1 * fTemp41))))) + (fSlow128 * (iSlow1?(fSlow3 * (fTemp3 * fTemp42)):(fSlow3 * (fTemp1 * fTemp42))))) + (fSlow130 * (iSlow1?(fSlow3 * (fTemp3 * fTemp43)):(fSlow3 * (fTemp1 * fTemp43))))) + (fSlow133 * (iSlow1?(fSlow3 * (fTemp3 * fTemp44)):(fSlow3 * (fTemp1 * fTemp44))))) + (fSlow135 * (iSlow1?(fSlow3 * (fTemp3 * fTemp45)):(fSlow3 * (fTemp1 * fTemp45))))) + (fSlow138 * (iSlow1?(fSlow3 * (fTemp3 * fTemp46)):(fSlow3 * (fTemp1 * fTemp46))))) + (fSlow140 * (iSlow1?(fSlow3 * (fTemp3 * fTemp47)):(fSlow3 * (fTemp1 * fTemp47))))) + (fSlow143 * (iSlow1?(fSlow3 * (fTemp3 * fTemp48)):(fSlow3 * (fTemp1 * fTemp48))))) + (fSlow145 * (iSlow1?(fSlow3 * (fTemp3 * fTemp49)):(fSlow3 * (fTemp1 * fTemp49)))))) + ((fSlow147 * (((((((((((((((fConst91 * ((fSlow148 * (iSlow1?(fSlow3 * (fTemp3 * fTemp50)):(fSlow3 * (fTemp1 * fTemp50)))) + (fSlow149 * (iSlow1?(fSlow3 * (fTemp3 * fTemp51)):(fSlow3 * (fTemp1 * fTemp51)))))) + (fSlow151 * (iSlow1?(fSlow3 * (fTemp3 * fTemp52)):(fSlow3 * (fTemp1 * fTemp52))))) + (fSlow153 * (iSlow1?(fSlow3 * (fTemp3 * fTemp53)):(fSlow3 * (fTemp1 * fTemp53))))) + (fSlow156 * (iSlow1?(fSlow3 * (fTemp3 * fTemp54)):(fSlow3 * (fTemp1 * fTemp54))))) + (fSlow158 * (iSlow1?(fSlow3 * (fTemp3 * fTemp55)):(fSlow3 * (fTemp1 * fTemp55))))) + (fSlow161 * (iSlow1?(fSlow3 * (fTemp3 * fTemp56)):(fSlow3 * (fTemp1 * fTemp56))))) + (fSlow163 * (iSlow1?(fSlow3 * (fTemp3 * fTemp57)):(fSlow3 * (fTemp1 * fTemp57))))) + (fSlow166 * (iSlow1?(fSlow3 * (fTemp3 * fTemp58)):(fSlow3 * (fTemp1 * fTemp58))))) + (fSlow168 * (iSlow1?(fSlow3 * (fTemp3 * fTemp59)):(fSlow3 * (fTemp1 * fTemp59))))) + (fSlow171 * (iSlow1?(fSlow3 * (fTemp3 * fTemp60)):(fSlow3 * (fTemp1 * fTemp60))))) + (fSlow173 * (iSlow1?(fSlow3 * (fTemp3 * fTemp61)):(fSlow3 * (fTemp1 * fTemp61))))) + (fSlow176 * (iSlow1?(fSlow3 * (fTemp3 * fTemp62)):(fSlow3 * (fTemp1 * fTemp62))))) + (fSlow178 * (iSlow1?(fSlow3 * (fTemp3 * fTemp63)):(fSlow3 * (fTemp1 * fTemp63))))) + (fSlow181 * (iSlow1?(fSlow3 * (fTemp3 * fTemp64)):(fSlow3 * (fTemp1 * fTemp64))))) + (fSlow183 * (iSlow1?(fSlow3 * (fTemp3 * fTemp65)):(fSlow3 * (fTemp1 * fTemp65)))))) + ((fSlow185 * (((((((((((((((((((fConst108 * ((fSlow12 * (iSlow1?(fSlow3 * (fTemp3 * fTemp66)):(fSlow3 * (fTemp1 * fTemp66)))) + (fSlow11 * (iSlow1?(fSlow3 * (fTemp3 * fTemp67)):(fSlow3 * (fTemp1 * fTemp67)))))) + (fSlow186 * (iSlow1?(fSlow3 * (fTemp3 * fTemp68)):(fSlow3 * (fTemp1 * fTemp68))))) + (fSlow187 * (iSlow1?(fSlow3 * (fTemp3 * fTemp69)):(fSlow3 * (fTemp1 * fTemp69))))) + (fSlow189 * (iSlow1?(fSlow3 * (fTemp3 * fTemp70)):(fSlow3 * (fTemp1 * fTemp70))))) + (fSlow191 * (iSlow1?(fSlow3 * (fTemp3 * fTemp71)):(fSlow3 * (fTemp1 * fTemp71))))) + (fSlow194 * (iSlow1?(fSlow3 * (fTemp3 * fTemp72)):(fSlow3 * (fTemp1 * fTemp72))))) + (fSlow196 * (iSlow1?(fSlow3 * (fTemp3 * fTemp73)):(fSlow3 * (fTemp1 * fTemp73))))) + (fSlow198 * (iSlow1?(fSlow3 * (fTemp3 * fTemp74)):(fSlow3 * (fTemp1 * fTemp74))))) + (fSlow200 * (iSlow1?(fSlow3 * (fTemp3 * fTemp75)):(fSlow3 * (fTemp1 * fTemp75))))) + (fSlow203 * (iSlow1?(fSlow3 * (fTemp3 * fTemp76)):(fSlow3 * (fTemp1 * fTemp76))))) + (fSlow205 * (iSlow1?(fSlow3 * (fTemp3 * fTemp77)):(fSlow3 * (fTemp1 * fTemp77))))) + (fSlow207 * (iSlow1?(fSlow3 * (fTemp3 * fTemp78)):(fSlow3 * (fTemp1 * fTemp78))))) + (fSlow209 * (iSlow1?(fSlow3 * (fTemp3 * fTemp79)):(fSlow3 * (fTemp1 * fTemp79))))) + (fSlow212 * (iSlow1?(fSlow3 * (fTemp3 * fTemp80)):(fSlow3 * (fTemp1 * fTemp80))))) + (fSlow214 * (iSlow1?(fSlow3 * (fTemp3 * fTemp81)):(fSlow3 * (fTemp1 * fTemp81))))) + (fSlow216 * (iSlow1?(fSlow3 * (fTemp3 * fTemp82)):(fSlow3 * (fTemp1 * fTemp82))))) + (fSlow218 * (iSlow1?(fSlow3 * (fTemp3 * fTemp83)):(fSlow3 * (fTemp1 * fTemp83))))) + (fSlow221 * (iSlow1?(fSlow3 * (fTemp3 * fTemp84)):(fSlow3 * (fTemp1 * fTemp84))))) + (fSlow223 * (iSlow1?(fSlow3 * (fTemp3 * fTemp85)):(fSlow3 * (fTemp1 * fTemp85)))))) + ((fSlow233 * (iSlow1?(fSlow3 * (fTemp3 * fTemp86)):(fSlow3 * (fTemp1 * fTemp86)))) + ((fSlow12 * (((((((((fSlow236 * (iSlow1?(fSlow3 * (fTemp3 * fTemp87)):(fSlow3 * (fTemp1 * fTemp87)))) + (fSlow238 * (iSlow1?(fSlow3 * (fTemp3 * fTemp88)):(fSlow3 * (fTemp1 * fTemp88))))) + (fSlow241 * (iSlow1?(fSlow3 * (fTemp3 * fTemp89)):(fSlow3 * (fTemp1 * fTemp89))))) + (fSlow243 * (iSlow1?(fSlow3 * (fTemp3 * fTemp90)):(fSlow3 * (fTemp1 * fTemp90))))) + (fSlow247 * (iSlow1?(fSlow3 * (fTemp3 * fTemp91)):(fSlow3 * (fTemp1 * fTemp91))))) + (fSlow249 * (iSlow1?(fSlow3 * (fTemp3 * fTemp92)):(fSlow3 * (fTemp1 * fTemp92))))) + (fSlow252 * (iSlow1?(fSlow3 * (fTemp3 * fTemp93)):(fSlow3 * (fTemp1 * fTemp93))))) + (fSlow254 * (iSlow1?(fSlow3 * (fTemp3 * fTemp94)):(fSlow3 * (fTemp1 * fTemp94))))) + (fSlow256 * (iSlow1?(fSlow3 * (fTemp3 * fTemp95)):(fSlow3 * (fTemp1 * fTemp95)))))) + ((((((iSlow1?(fSlow3 * (fTemp3 * fTemp0)):(fSlow3 * (fTemp1 * fTemp0))) + (fSlow257 * (iSlow1?(fSlow3 * (fTemp3 * fTemp96)):(fSlow3 * (fTemp1 * fTemp96))))) + (fSlow258 * (iSlow1?(fSlow3 * (fTemp3 * fTemp97)):(fSlow3 * (fTemp1 * fTemp97))))) + (fSlow259 * (iSlow1?(fSlow3 * (fTemp3 * fTemp98)):(fSlow3 * (fTemp1 * fTemp98))))) + (fSlow260 * (iSlow1?(fSlow3 * (fTemp3 * fTemp99)):(fSlow3 * (fTemp1 * fTemp99))))) + (fSlow7 * (((((((((((((1.7320508075688772 * (iSlow1?(fSlow3 * (fTemp3 * fTemp100)):(fSlow3 * (fTemp1 * fTemp100)))) + (fSlow261 * (iSlow1?(fSlow3 * (fTemp3 * fTemp101)):(fSlow3 * (fTemp1 * fTemp101))))) + (fSlow263 * (iSlow1?(fSlow3 * (fTemp3 * fTemp102)):(fSlow3 * (fTemp1 * fTemp102))))) + (fSlow264 * (iSlow1?(fSlow3 * (fTemp3 * fTemp103)):(fSlow3 * (fTemp1 * fTemp103))))) + (fSlow266 * (iSlow1?(fSlow3 * (fTemp3 * fTemp104)):(fSlow3 * (fTemp1 * fTemp104))))) + (fSlow268 * (iSlow1?(fSlow3 * (fTemp3 * fTemp105)):(fSlow3 * (fTemp1 * fTemp105))))) + (fSlow269 * (iSlow1?(fSlow3 * (fTemp3 * fTemp106)):(fSlow3 * (fTemp1 * fTemp106))))) + (fSlow271 * (iSlow1?(fSlow3 * (fTemp3 * fTemp107)):(fSlow3 * (fTemp1 * fTemp107))))) + (fSlow273 * (iSlow1?(fSlow3 * (fTemp3 * fTemp108)):(fSlow3 * (fTemp1 * fTemp108))))) + (fSlow275 * (iSlow1?(fSlow3 * (fTemp3 * fTemp109)):(fSlow3 * (fTemp1 * fTemp109))))) + (fSlow276 * (iSlow1?(fSlow3 * (fTemp3 * fTemp110)):(fSlow3 * (fTemp1 * fTemp110))))) + (fSlow278 * (iSlow1?(fSlow3 * (fTemp3 * fTemp111)):(fSlow3 * (fTemp1 * fTemp111))))) + (fSlow280 * (iSlow1?(fSlow3 * (fTemp3 * fTemp112)):(fSlow3 * (fTemp1 * fTemp112)))))))))))) + (fSlow281 * (((((((fSlow283 * (iSlow1?(fSlow3 * (fTemp3 * fTemp113)):(fSlow3 * (fTemp1 * fTemp113)))) + (fSlow285 * (iSlow1?(fSlow3 * (fTemp3 * fTemp114)):(fSlow3 * (fTemp1 * fTemp114))))) + (fSlow287 * (iSlow1?(fSlow3 * (fTemp3 * fTemp115)):(fSlow3 * (fTemp1 * fTemp115))))) + (fSlow290 * (iSlow1?(fSlow3 * (fTemp3 * fTemp116)):(fSlow3 * (fTemp1 * fTemp116))))) + (fSlow292 * (iSlow1?(fSlow3 * (fTemp3 * fTemp117)):(fSlow3 * (fTemp1 * fTemp117))))) + (fSlow295 * (iSlow1?(fSlow3 * (fTemp3 * fTemp118)):(fSlow3 * (fTemp1 * fTemp118))))) + (fSlow297 * (iSlow1?(fSlow3 * (fTemp3 * fTemp119)):(fSlow3 * (fTemp1 * fTemp119))))))) + (fSlow298 * (((fSlow300 * (iSlow1?(fSlow3 * (fTemp3 * fTemp120)):(fSlow3 * (fTemp1 * fTemp120)))) + (fSlow302 * (iSlow1?(fSlow3 * (fTemp3 * fTemp121)):(fSlow3 * (fTemp1 * fTemp121))))) + (fSlow304 * (iSlow1?(fSlow3 * (fTemp3 * fTemp122)):(fSlow3 * (fTemp1 * fTemp122))))))));
			output0[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp0)):(fSlow3 * (fRec0[0] * fTemp0))) + (0.079577471545947673 * fTemp123)));
			output1[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp66)):(fSlow3 * (fRec0[0] * fTemp66))) + (fSlow305 * fTemp123)));
			output2[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp100)):(fSlow3 * (fRec0[0] * fTemp100))) + (fSlow306 * fTemp123)));
			output3[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp67)):(fSlow3 * (fRec0[0] * fTemp67))) + (fSlow307 * fTemp123)));
			output4[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp2)):(fSlow3 * (fRec0[0] * fTemp2))) + (fSlow308 * fTemp123)));
			output5[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp68)):(fSlow3 * (fRec0[0] * fTemp68))) + (fSlow309 * fTemp123)));
			output6[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp96)):(fSlow3 * (fRec0[0] * fTemp96))) + (fSlow310 * fTemp123)));
			output7[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp69)):(fSlow3 * (fRec0[0] * fTemp69))) + (fSlow311 * fTemp123)));
			output8[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp4)):(fSlow3 * (fRec0[0] * fTemp4))) + (fSlow312 * fTemp123)));
			output9[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp50)):(fSlow3 * (fRec0[0] * fTemp50))) + (fSlow313 * fTemp123)));
			output10[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp5)):(fSlow3 * (fRec0[0] * fTemp5))) + (fSlow314 * fTemp123)));
			output11[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp70)):(fSlow3 * (fRec0[0] * fTemp70))) + (fSlow315 * fTemp123)));
			output12[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp101)):(fSlow3 * (fRec0[0] * fTemp101))) + (fSlow316 * fTemp123)));
			output13[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp71)):(fSlow3 * (fRec0[0] * fTemp71))) + (fSlow317 * fTemp123)));
			output14[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp6)):(fSlow3 * (fRec0[0] * fTemp6))) + (fSlow318 * fTemp123)));
			output15[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp51)):(fSlow3 * (fRec0[0] * fTemp51))) + (fSlow319 * fTemp123)));
			output16[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp7)):(fSlow3 * (fRec0[0] * fTemp7))) + (fSlow320 * fTemp123)));
			output17[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp52)):(fSlow3 * (fRec0[0] * fTemp52))) + (fSlow321 * fTemp123)));
			output18[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp8)):(fSlow3 * (fRec0[0] * fTemp8))) + (fSlow322 * fTemp123)));
			output19[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp72)):(fSlow3 * (fRec0[0] * fTemp72))) + (fSlow323 * fTemp123)));
			output20[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp97)):(fSlow3 * (fRec0[0] * fTemp97))) + (fSlow324 * fTemp123)));
			output21[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp73)):(fSlow3 * (fRec0[0] * fTemp73))) + (fSlow325 * fTemp123)));
			output22[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp9)):(fSlow3 * (fRec0[0] * fTemp9))) + (fSlow326 * fTemp123)));
			output23[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp53)):(fSlow3 * (fRec0[0] * fTemp53))) + (fSlow327 * fTemp123)));
			output24[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp10)):(fSlow3 * (fRec0[0] * fTemp10))) + (fSlow328 * fTemp123)));
			output25[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp38)):(fSlow3 * (fRec0[0] * fTemp38))) + (fSlow329 * fTemp123)));
			output26[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp102)):(fSlow3 * (fRec0[0] * fTemp102))) + (fSlow330 * fTemp123)));
			output27[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp54)):(fSlow3 * (fRec0[0] * fTemp54))) + (fSlow331 * fTemp123)));
			output28[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp11)):(fSlow3 * (fRec0[0] * fTemp11))) + (fSlow332 * fTemp123)));
			output29[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp74)):(fSlow3 * (fRec0[0] * fTemp74))) + (fSlow333 * fTemp123)));
			output30[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp103)):(fSlow3 * (fRec0[0] * fTemp103))) + (fSlow334 * fTemp123)));
			output31[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp75)):(fSlow3 * (fRec0[0] * fTemp75))) + (fSlow335 * fTemp123)));
			output32[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp12)):(fSlow3 * (fRec0[0] * fTemp12))) + (fSlow336 * fTemp123)));
			output33[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp55)):(fSlow3 * (fRec0[0] * fTemp55))) + (fSlow337 * fTemp123)));
			output34[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp104)):(fSlow3 * (fRec0[0] * fTemp104))) + (fSlow338 * fTemp123)));
			output35[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp39)):(fSlow3 * (fRec0[0] * fTemp39))) + (fSlow339 * fTemp123)));
			output36[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp13)):(fSlow3 * (fRec0[0] * fTemp13))) + (fSlow340 * fTemp123)));
			output37[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp40)):(fSlow3 * (fRec0[0] * fTemp40))) + (fSlow341 * fTemp123)));
			output38[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp14)):(fSlow3 * (fRec0[0] * fTemp14))) + (fSlow342 * fTemp123)));
			output39[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp56)):(fSlow3 * (fRec0[0] * fTemp56))) + (fSlow343 * fTemp123)));
			output40[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp15)):(fSlow3 * (fRec0[0] * fTemp15))) + (fSlow344 * fTemp123)));
			output41[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp76)):(fSlow3 * (fRec0[0] * fTemp76))) + (fSlow345 * fTemp123)));
			output42[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp98)):(fSlow3 * (fRec0[0] * fTemp98))) + (fSlow346 * fTemp123)));
			output43[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp77)):(fSlow3 * (fRec0[0] * fTemp77))) + (fSlow347 * fTemp123)));
			output44[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp16)):(fSlow3 * (fRec0[0] * fTemp16))) + (fSlow348 * fTemp123)));
			output45[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp57)):(fSlow3 * (fRec0[0] * fTemp57))) + (fSlow349 * fTemp123)));
			output46[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp17)):(fSlow3 * (fRec0[0] * fTemp17))) + (fSlow350 * fTemp123)));
			output47[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp41)):(fSlow3 * (fRec0[0] * fTemp41))) + (fSlow351 * fTemp123)));
			output48[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp18)):(fSlow3 * (fRec0[0] * fTemp18))) + (fSlow352 * fTemp123)));
			output49[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp87)):(fSlow3 * (fRec0[0] * fTemp87))) + (fSlow353 * fTemp123)));
			output50[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp105)):(fSlow3 * (fRec0[0] * fTemp105))) + (fSlow354 * fTemp123)));
			output51[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp42)):(fSlow3 * (fRec0[0] * fTemp42))) + (fSlow355 * fTemp123)));
			output52[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp19)):(fSlow3 * (fRec0[0] * fTemp19))) + (fSlow356 * fTemp123)));
			output53[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp58)):(fSlow3 * (fRec0[0] * fTemp58))) + (fSlow357 * fTemp123)));
			output54[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp88)):(fSlow3 * (fRec0[0] * fTemp88))) + (fSlow358 * fTemp123)));
			output55[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp78)):(fSlow3 * (fRec0[0] * fTemp78))) + (fSlow359 * fTemp123)));
			output56[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp106)):(fSlow3 * (fRec0[0] * fTemp106))) + (fSlow360 * fTemp123)));
			output57[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp79)):(fSlow3 * (fRec0[0] * fTemp79))) + (fSlow361 * fTemp123)));
			output58[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp20)):(fSlow3 * (fRec0[0] * fTemp20))) + (fSlow362 * fTemp123)));
			output59[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp59)):(fSlow3 * (fRec0[0] * fTemp59))) + (fSlow363 * fTemp123)));
			output60[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp107)):(fSlow3 * (fRec0[0] * fTemp107))) + (fSlow364 * fTemp123)));
			output61[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp43)):(fSlow3 * (fRec0[0] * fTemp43))) + (fSlow365 * fTemp123)));
			output62[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp21)):(fSlow3 * (fRec0[0] * fTemp21))) + (fSlow366 * fTemp123)));
			output63[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp113)):(fSlow3 * (fRec0[0] * fTemp113))) + (fSlow367 * fTemp123)));
			output64[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp89)):(fSlow3 * (fRec0[0] * fTemp89))) + (fSlow368 * fTemp123)));
			output65[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp114)):(fSlow3 * (fRec0[0] * fTemp114))) + (fSlow369 * fTemp123)));
			output66[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp22)):(fSlow3 * (fRec0[0] * fTemp22))) + (fSlow370 * fTemp123)));
			output67[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp44)):(fSlow3 * (fRec0[0] * fTemp44))) + (fSlow371 * fTemp123)));
			output68[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp90)):(fSlow3 * (fRec0[0] * fTemp90))) + (fSlow372 * fTemp123)));
			output69[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp60)):(fSlow3 * (fRec0[0] * fTemp60))) + (fSlow373 * fTemp123)));
			output70[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp23)):(fSlow3 * (fRec0[0] * fTemp23))) + (fSlow374 * fTemp123)));
			output71[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp80)):(fSlow3 * (fRec0[0] * fTemp80))) + (fSlow375 * fTemp123)));
			output72[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp99)):(fSlow3 * (fRec0[0] * fTemp99))) + (fSlow376 * fTemp123)));
			output73[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp81)):(fSlow3 * (fRec0[0] * fTemp81))) + (fSlow377 * fTemp123)));
			output74[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp24)):(fSlow3 * (fRec0[0] * fTemp24))) + (fSlow378 * fTemp123)));
			output75[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp61)):(fSlow3 * (fRec0[0] * fTemp61))) + (fSlow379 * fTemp123)));
			output76[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp25)):(fSlow3 * (fRec0[0] * fTemp25))) + (fSlow380 * fTemp123)));
			output77[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp45)):(fSlow3 * (fRec0[0] * fTemp45))) + (fSlow381 * fTemp123)));
			output78[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp26)):(fSlow3 * (fRec0[0] * fTemp26))) + (fSlow382 * fTemp123)));
			output79[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp115)):(fSlow3 * (fRec0[0] * fTemp115))) + (fSlow383 * fTemp123)));
			output80[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp27)):(fSlow3 * (fRec0[0] * fTemp27))) + (fSlow384 * fTemp123)));
			output81[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp91)):(fSlow3 * (fRec0[0] * fTemp91))) + (fSlow385 * fTemp123)));
			output82[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp108)):(fSlow3 * (fRec0[0] * fTemp108))) + (fSlow386 * fTemp123)));
			output83[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp116)):(fSlow3 * (fRec0[0] * fTemp116))) + (fSlow387 * fTemp123)));
			output84[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp28)):(fSlow3 * (fRec0[0] * fTemp28))) + (fSlow388 * fTemp123)));
			output85[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp46)):(fSlow3 * (fRec0[0] * fTemp46))) + (fSlow389 * fTemp123)));
			output86[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp92)):(fSlow3 * (fRec0[0] * fTemp92))) + (fSlow390 * fTemp123)));
			output87[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp62)):(fSlow3 * (fRec0[0] * fTemp62))) + (fSlow391 * fTemp123)));
			output88[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp109)):(fSlow3 * (fRec0[0] * fTemp109))) + (fSlow392 * fTemp123)));
			output89[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp82)):(fSlow3 * (fRec0[0] * fTemp82))) + (fSlow393 * fTemp123)));
			output90[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp110)):(fSlow3 * (fRec0[0] * fTemp110))) + (fSlow394 * fTemp123)));
			output91[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp83)):(fSlow3 * (fRec0[0] * fTemp83))) + (fSlow395 * fTemp123)));
			output92[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp29)):(fSlow3 * (fRec0[0] * fTemp29))) + (fSlow396 * fTemp123)));
			output93[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp63)):(fSlow3 * (fRec0[0] * fTemp63))) + (fSlow397 * fTemp123)));
			output94[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp111)):(fSlow3 * (fRec0[0] * fTemp111))) + (fSlow398 * fTemp123)));
			output95[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp47)):(fSlow3 * (fRec0[0] * fTemp47))) + (fSlow399 * fTemp123)));
			output96[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp30)):(fSlow3 * (fRec0[0] * fTemp30))) + (fSlow400 * fTemp123)));
			output97[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp117)):(fSlow3 * (fRec0[0] * fTemp117))) + (fSlow401 * fTemp123)));
			output98[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp112)):(fSlow3 * (fRec0[0] * fTemp112))) + (fSlow402 * fTemp123)));
			output99[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp120)):(fSlow3 * (fRec0[0] * fTemp120))) + (fSlow403 * fTemp123)));
			output100[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp93)):(fSlow3 * (fRec0[0] * fTemp93))) + (fSlow404 * fTemp123)));
			output101[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp121)):(fSlow3 * (fRec0[0] * fTemp121))) + (fSlow405 * fTemp123)));
			output102[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp31)):(fSlow3 * (fRec0[0] * fTemp31))) + (fSlow406 * fTemp123)));
			output103[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp118)):(fSlow3 * (fRec0[0] * fTemp118))) + (fSlow407 * fTemp123)));
			output104[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp94)):(fSlow3 * (fRec0[0] * fTemp94))) + (fSlow408 * fTemp123)));
			output105[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp48)):(fSlow3 * (fRec0[0] * fTemp48))) + (fSlow409 * fTemp123)));
			output106[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp32)):(fSlow3 * (fRec0[0] * fTemp32))) + (fSlow410 * fTemp123)));
			output107[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp64)):(fSlow3 * (fRec0[0] * fTemp64))) + (fSlow411 * fTemp123)));
			output108[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp95)):(fSlow3 * (fRec0[0] * fTemp95))) + (fSlow412 * fTemp123)));
			output109[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp84)):(fSlow3 * (fRec0[0] * fTemp84))) + (fSlow413 * fTemp123)));
			output110[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp86)):(fSlow3 * (fRec0[0] * fTemp86))) + (fSlow414 * fTemp123)));
			output111[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp85)):(fSlow3 * (fRec0[0] * fTemp85))) + (fSlow415 * fTemp123)));
			output112[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp33)):(fSlow3 * (fRec0[0] * fTemp33))) + (fSlow416 * fTemp123)));
			output113[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp65)):(fSlow3 * (fRec0[0] * fTemp65))) + (fSlow417 * fTemp123)));
			output114[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp34)):(fSlow3 * (fRec0[0] * fTemp34))) + (fSlow418 * fTemp123)));
			output115[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp49)):(fSlow3 * (fRec0[0] * fTemp49))) + (fSlow419 * fTemp123)));
			output116[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp35)):(fSlow3 * (fRec0[0] * fTemp35))) + (fSlow420 * fTemp123)));
			output117[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp119)):(fSlow3 * (fRec0[0] * fTemp119))) + (fSlow421 * fTemp123)));
			output118[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp36)):(fSlow3 * (fRec0[0] * fTemp36))) + (fSlow422 * fTemp123)));
			output119[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp122)):(fSlow3 * (fRec0[0] * fTemp122))) + (fSlow423 * fTemp123)));
			output120[i] = FAUSTFLOAT(((iSlow1?(fSlow3 * (fRec1[0] * fTemp37)):(fSlow3 * (fRec0[0] * fTemp37))) + (fSlow424 * fTemp123)));
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
