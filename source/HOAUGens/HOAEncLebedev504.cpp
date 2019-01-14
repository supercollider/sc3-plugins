/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAEncLebedev504"
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
	double fRec0[2];
	FAUSTFLOAT fVbargraph0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
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
	double fRec2[2];
	FAUSTFLOAT fVbargraph1;
	double fRec3[2];
	FAUSTFLOAT fVbargraph2;
	double fConst17;
	double fConst18;
	double fConst19;
	double fRec4[2];
	FAUSTFLOAT fVbargraph3;
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
	double fRec5[2];
	FAUSTFLOAT fVbargraph4;
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
	double fRec6[2];
	FAUSTFLOAT fVbargraph5;
	double fRec7[2];
	FAUSTFLOAT fVbargraph6;
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
	double fRec8[2];
	FAUSTFLOAT fVbargraph7;
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
	double fRec9[2];
	FAUSTFLOAT fVbargraph8;
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
	double fRec10[2];
	FAUSTFLOAT fVbargraph9;
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
	double fRec11[2];
	FAUSTFLOAT fVbargraph10;
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
	double fRec12[2];
	FAUSTFLOAT fVbargraph11;
	double fRec13[2];
	FAUSTFLOAT fVbargraph12;
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
	double fRec14[2];
	FAUSTFLOAT fVbargraph13;
	double fConst231;
	double fConst232;
	double fConst233;
	double fConst234;
	double fConst235;
	double fConst236;
	double fConst237;
	double fConst238;
	double fConst239;
	double fConst240;
	double fConst241;
	double fConst242;
	double fConst243;
	double fConst244;
	double fConst245;
	double fConst246;
	double fConst247;
	double fConst248;
	double fConst249;
	double fConst250;
	double fConst251;
	double fConst252;
	double fConst253;
	double fConst254;
	double fConst255;
	double fConst256;
	double fConst257;
	double fConst258;
	double fConst259;
	double fConst260;
	double fConst261;
	double fConst262;
	double fRec15[2];
	FAUSTFLOAT fVbargraph14;
	double fConst263;
	double fConst264;
	double fConst265;
	double fConst266;
	double fConst267;
	double fConst268;
	double fConst269;
	double fConst270;
	double fConst271;
	double fConst272;
	double fRec16[2];
	FAUSTFLOAT fVbargraph15;
	double fConst273;
	double fConst274;
	double fConst275;
	double fConst276;
	double fConst277;
	double fConst278;
	double fRec17[2];
	FAUSTFLOAT fVbargraph16;
	double fConst279;
	double fConst280;
	double fConst281;
	double fConst282;
	double fConst283;
	double fConst284;
	double fConst285;
	double fConst286;
	double fConst287;
	double fConst288;
	double fConst289;
	double fConst290;
	double fConst291;
	double fConst292;
	double fConst293;
	double fConst294;
	double fConst295;
	double fConst296;
	double fConst297;
	double fConst298;
	double fConst299;
	double fConst300;
	double fConst301;
	double fConst302;
	double fConst303;
	double fConst304;
	double fConst305;
	double fConst306;
	double fConst307;
	double fConst308;
	double fConst309;
	double fConst310;
	double fConst311;
	double fConst312;
	double fConst313;
	double fConst314;
	double fConst315;
	double fConst316;
	double fConst317;
	double fConst318;
	double fConst319;
	double fConst320;
	double fConst321;
	double fRec18[2];
	FAUSTFLOAT fVbargraph17;
	double fConst322;
	double fConst323;
	double fConst324;
	double fConst325;
	double fConst326;
	double fConst327;
	double fConst328;
	double fConst329;
	double fConst330;
	double fConst331;
	double fConst332;
	double fConst333;
	double fConst334;
	double fConst335;
	double fConst336;
	double fConst337;
	double fConst338;
	double fConst339;
	double fConst340;
	double fConst341;
	double fConst342;
	double fConst343;
	double fConst344;
	double fConst345;
	double fConst346;
	double fConst347;
	double fConst348;
	double fConst349;
	double fConst350;
	double fConst351;
	double fConst352;
	double fConst353;
	double fConst354;
	double fConst355;
	double fConst356;
	double fConst357;
	double fConst358;
	double fConst359;
	double fConst360;
	double fConst361;
	double fConst362;
	double fConst363;
	double fRec19[2];
	FAUSTFLOAT fVbargraph18;
	double fConst364;
	double fConst365;
	double fConst366;
	double fConst367;
	double fConst368;
	double fConst369;
	double fConst370;
	double fConst371;
	double fConst372;
	double fConst373;
	double fConst374;
	double fConst375;
	double fConst376;
	double fConst377;
	double fConst378;
	double fConst379;
	double fConst380;
	double fConst381;
	double fConst382;
	double fConst383;
	double fConst384;
	double fConst385;
	double fConst386;
	double fConst387;
	double fConst388;
	double fConst389;
	double fConst390;
	double fConst391;
	double fConst392;
	double fConst393;
	double fConst394;
	double fConst395;
	double fConst396;
	double fConst397;
	double fConst398;
	double fConst399;
	double fConst400;
	double fConst401;
	double fConst402;
	double fConst403;
	double fConst404;
	double fConst405;
	double fConst406;
	double fConst407;
	double fConst408;
	double fConst409;
	double fConst410;
	double fRec20[2];
	FAUSTFLOAT fVbargraph19;
	double fRec21[2];
	FAUSTFLOAT fVbargraph20;
	double fConst411;
	double fConst412;
	double fConst413;
	double fConst414;
	double fConst415;
	double fConst416;
	double fConst417;
	double fConst418;
	double fConst419;
	double fConst420;
	double fConst421;
	double fConst422;
	double fConst423;
	double fConst424;
	double fConst425;
	double fConst426;
	double fConst427;
	double fConst428;
	double fRec22[2];
	FAUSTFLOAT fVbargraph21;
	double fConst429;
	double fConst430;
	double fConst431;
	double fConst432;
	double fConst433;
	double fConst434;
	double fConst435;
	double fConst436;
	double fConst437;
	double fConst438;
	double fConst439;
	double fConst440;
	double fConst441;
	double fConst442;
	double fConst443;
	double fConst444;
	double fConst445;
	double fConst446;
	double fConst447;
	double fConst448;
	double fConst449;
	double fConst450;
	double fConst451;
	double fConst452;
	double fConst453;
	double fConst454;
	double fConst455;
	double fConst456;
	double fConst457;
	double fConst458;
	double fConst459;
	double fConst460;
	double fConst461;
	double fConst462;
	double fConst463;
	double fConst464;
	double fConst465;
	double fRec23[2];
	FAUSTFLOAT fVbargraph22;
	double fConst466;
	double fConst467;
	double fConst468;
	double fConst469;
	double fConst470;
	double fConst471;
	double fConst472;
	double fConst473;
	double fConst474;
	double fConst475;
	double fConst476;
	double fConst477;
	double fConst478;
	double fConst479;
	double fConst480;
	double fConst481;
	double fConst482;
	double fConst483;
	double fConst484;
	double fConst485;
	double fConst486;
	double fConst487;
	double fConst488;
	double fConst489;
	double fConst490;
	double fConst491;
	double fConst492;
	double fConst493;
	double fConst494;
	double fConst495;
	double fRec24[2];
	FAUSTFLOAT fVbargraph23;
	double fConst496;
	double fConst497;
	double fConst498;
	double fConst499;
	double fConst500;
	double fRec25[2];
	FAUSTFLOAT fVbargraph24;
	
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
		m->declare("name", "HOAEncLebedev504");
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
		return 50;
		
	}
	virtual int getNumOutputs() {
		return 25;
		
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
		fConst1 = double(tgamma(1.0));
		fConst2 = double(tgamma(3.0));
		fConst3 = double(tgamma(1.5));
		fConst4 = (sqrt((3.0 * (fConst1 / fConst2))) * fConst3);
		fConst5 = (0.012698412698412698 * (0.0 - (1.5957691216057308 * fConst4)));
		fConst6 = (0.0 - (1.1283791670955128 * fConst4));
		fConst7 = (0.0 - (1.1283791670955126 * fConst4));
		fConst8 = (0.0 - (1.128379167095513 * fConst4));
		fConst9 = (0.0 - (0.92131773192356115 * fConst4));
		fConst10 = (0.0 - (0.92131773192356148 * fConst4));
		fConst11 = (0.0 - (0.48114249349102017 * fConst4));
		fConst12 = (0.0 - (0.48114249349102034 * fConst4));
		fConst13 = (0.0 - (0.48114249349102112 * fConst4));
		fConst14 = (0.0 - (1.4434274804730622 * fConst4));
		fConst15 = (0.0 - (1.4434274804730625 * fConst4));
		fConst16 = (0.0 - (0.48114249349102228 * fConst4));
		fConst17 = (0.0 - (0.48114249349102062 * fConst4));
		fConst18 = (0.0 - (1.443427480473062 * fConst4));
		fConst19 = (0.0 - (0.48114249349102095 * fConst4));
		fConst20 = double(tgamma(5.0));
		fConst21 = double(tgamma(2.5));
		fConst22 = (sqrt((5.0 * (fConst1 / fConst20))) * fConst21);
		fConst23 = (0.0 - (0.58027968058390089 * fConst22));
		fConst24 = (0.0 - (1.7408390417517061 * fConst22));
		fConst25 = (0.0 - (1.7408390417517088 * fConst22));
		fConst26 = (0.0 - (1.7408390417517057 * fConst22));
		fConst27 = (0.0 - (1.7408390417517114 * fConst22));
		fConst28 = (0.0 - (3.1915382432114616 * fConst22));
		fConst29 = (0.022574955908289243 * fConst28);
		fConst30 = (0.021093750000000001 * (0.0 - (2.1276921621409737 * fConst22)));
		fConst31 = double(tgamma(2.0));
		fConst32 = double(tgamma(4.0));
		fConst33 = sqrt((5.0 * (fConst31 / fConst32)));
		fConst34 = (fConst33 * fConst3);
		fConst35 = (0.0 - (2.3936536824085959 * fConst34));
		fConst36 = (0.0 - (1.6925687506432687 * fConst3));
		fConst37 = (0.0 - (1.4142135623730951 * (fConst33 * fConst36)));
		fConst38 = (0.0 - (1.5957691216057308 * fConst34));
		fConst39 = (0.0 - (1.5957691216057313 * fConst34));
		fConst40 = (0.0 - (1.5957691216057308 * fConst3));
		fConst41 = (fConst33 * fConst40);
		fConst42 = (0.0 - fConst41);
		fConst43 = (0.0 - (1.0000000000000002 * fConst41));
		fConst44 = (0.054036626340088233 * fConst3);
		fConst45 = (0.031925808815477283 * fConst36);
		fConst46 = (0.033660754908870889 * fConst3);
		fConst47 = (0.021093750000000001 * fConst40);
		fConst48 = (0.021093750000000005 * fConst40);
		fConst49 = (0.026338897580074728 * fConst3);
		fConst50 = (0.0087796325266915921 * fConst3);
		fConst51 = (0.026338897580074742 * fConst3);
		fConst52 = (0.026338897580074745 * fConst3);
		fConst53 = (0.0087796325266915939 * fConst3);
		fConst54 = (0.0 - (0.97315860881060223 * fConst3));
		fConst55 = (0.0090217899191397874 * fConst54);
		fConst56 = (0.027065369757419327 * fConst54);
		fConst57 = (0.027065369757419331 * fConst54);
		fConst58 = (0.0090217899191397891 * fConst54);
		fConst59 = (0.0 - (1.3056292813137786 * fConst3));
		fConst60 = (0.020173335537918873 * fConst59);
		fConst61 = (0.0 - (1.3056292813137786 * fConst34));
		fConst62 = (0.0 - (1.3056292813137791 * fConst34));
		fConst63 = (0.0 - (0.43520976043792686 * fConst34));
		fConst64 = (0.0 - (1.3056292813137795 * fConst34));
		fConst65 = (0.0 - (1.3056292813137798 * fConst34));
		fConst66 = (0.0 - (0.43520976043792792 * fConst34));
		fConst67 = (fConst33 * fConst54);
		fConst68 = (0.0 - (0.44721359549995832 * fConst67));
		fConst69 = (0.0 - (1.3416407864998738 * fConst67));
		fConst70 = (0.0 - (1.3416407864998741 * fConst67));
		fConst71 = (0.0 - (0.44721359549995937 * fConst67));
		fConst72 = (fConst33 * fConst59);
		fConst73 = (0.0 - fConst72);
		fConst74 = (0.0 - (1.0000000000000002 * fConst72));
		fConst75 = (0.033660754908870882 * fConst3);
		fConst76 = (0.021093749999999998 * fConst40);
		fConst77 = (0.026338897580074724 * fConst3);
		fConst78 = (0.026338897580074721 * fConst3);
		fConst79 = (0.026338897580074738 * fConst3);
		fConst80 = (0.0087796325266915817 * fConst3);
		fConst81 = (0.0087796325266915748 * fConst3);
		fConst82 = (0.026338897580074731 * fConst3);
		fConst83 = (0.027065369757419324 * fConst54);
		fConst84 = (0.009021789919139777 * fConst54);
		fConst85 = (0.00902178991913977 * fConst54);
		fConst86 = (0.027065369757419321 * fConst54);
		fConst87 = (0.02017333553791887 * fConst59);
		fConst88 = (0.020173335537918866 * fConst59);
		fConst89 = (0.0 - (0.43520976043792636 * fConst34));
		fConst90 = (0.0 - (1.3056292813137793 * fConst34));
		fConst91 = (0.0 - (0.4352097604379267 * fConst34));
		fConst92 = (0.0 - (0.44721359549995782 * fConst67));
		fConst93 = (0.0 - (1.3416407864998736 * fConst67));
		fConst94 = (0.0 - (0.44721359549995815 * fConst67));
		fConst95 = (0.0 - (1.2884797242127563e-16 * fConst22));
		fConst96 = (0.0 - (2.5769594484255127e-16 * fConst22));
		fConst97 = (0.0 - (2.3211187223356089 * fConst22));
		fConst98 = (0.0 - (2.3211187223356093 * fConst22));
		fConst99 = (0.0 - (2.3211187223356085 * fConst22));
		fConst100 = (0.0 - (2.3211187223356098 * fConst22));
		fConst101 = (0.012698412698412698 * fConst28);
		fConst102 = (0.0 - (1.595769121605731 * fConst22));
		fConst103 = (0.0 - (7.0866384831701748e-16 * fConst22));
		fConst104 = (0.0 - (1.417327696634035e-15 * fConst22));
		fConst105 = (0.0 - (4.7244256554467816e-16 * fConst22));
		fConst106 = (0.0 - (9.4488513108935631e-16 * fConst22));
		fConst107 = double(tgamma(7.0));
		fConst108 = double(tgamma(3.5));
		fConst109 = (sqrt((7.0 * (fConst1 / fConst107))) * fConst108);
		fConst110 = (0.012698412698412698 * (0.0 - (6.3830764864229232 * fConst109)));
		fConst111 = (0.0 - (2.256758334191026 * fConst109));
		fConst112 = (0.0 - (4.5135166683820547 * fConst109));
		fConst113 = (0.0 - (4.5135166683820476 * fConst109));
		fConst114 = (0.0 - (2.456847285129498 * fConst109));
		fConst115 = (0.0 - (2.456847285129494 * fConst109));
		fConst116 = (0.0 - (0.34992181344801421 * fConst109));
		fConst117 = (0.0 - (0.34992181344801365 * fConst109));
		fConst118 = (0.0 - (3.1492963210321352 * fConst109));
		fConst119 = (0.0 - (3.1492963210321379 * fConst109));
		fConst120 = (0.0 - (4.5489835748241987 * fConst109));
		fConst121 = (0.0 - (4.5489835748242058 * fConst109));
		fConst122 = double(tgamma(6.0));
		fConst123 = sqrt((7.0 * (fConst31 / fConst122)));
		fConst124 = (fConst123 * fConst21);
		fConst125 = (0.0 - (2.624413600860108 * fConst124));
		fConst126 = (0.0 - (2.6244136008601116 * fConst124));
		fConst127 = (0.0 - (2.6244136008601155 * fConst124));
		fConst128 = (0.0 - (2.6244136008601107 * fConst124));
		fConst129 = (0.0 - (2.6244136008601191 * fConst124));
		fConst130 = (0.0 - (3.0929010896773175 * fConst21));
		fConst131 = (fConst123 * fConst130);
		fConst132 = (0.0 - (0.84852813742385691 * fConst131));
		fConst133 = (0.0 - (0.84852813742385813 * fConst131));
		fConst134 = (0.0 - (0.84852813742385658 * fConst131));
		fConst135 = (0.0 - (0.84852813742385946 * fConst131));
		fConst136 = (0.0 - (1.8557406538063874 * fConst21));
		fConst137 = (fConst123 * fConst136);
		fConst138 = (0.0 - (1.4142135623730951 * fConst137));
		fConst139 = (0.0 - (6.1421182128237417 * fConst124));
		fConst140 = (0.0 - (4.3431334391370653 * fConst21));
		fConst141 = (fConst123 * fConst140);
		fConst142 = (0.0 - (1.4142135623730951 * fConst141));
		fConst143 = (0.029831067331307477 * fConst140);
		fConst144 = (0.029831067331307481 * fConst140);
		fConst145 = (0.052943176160428845 * fConst21);
		fConst146 = (0.052943176160428852 * fConst21);
		fConst147 = (0.052943176160428991 * fConst21);
		fConst148 = (0.052943176160428929 * fConst21);
		fConst149 = (0.052943176160428963 * fConst21);
		fConst150 = (0.052943176160428942 * fConst21);
		fConst151 = (0.017117642829616822 * fConst130);
		fConst152 = (0.017117642829616801 * fConst130);
		fConst153 = (0.017117642829616815 * fConst130);
		fConst154 = (0.017117642829616808 * fConst130);
		fConst155 = (0.028529404716028006 * fConst136);
		fConst156 = (0.028529404716028009 * fConst136);
		fConst157 = sqrt((7.0 * (fConst2 / fConst20)));
		fConst158 = (0.0 - (3.3851375012865379 * fConst3));
		fConst159 = (fConst157 * fConst158);
		fConst160 = (0.012698412698412698 * (0.0 - (0.70710678118654757 * fConst159)));
		fConst161 = (fConst157 * fConst3);
		fConst162 = (0.0 - (2.5388531259649025 * fConst161));
		fConst163 = (0.0 - (0.5 * fConst159));
		fConst164 = (0.0 - (0.50000000000000011 * fConst159));
		fConst165 = (0.0 - ((3.5355339059327373 * fConst36) + (2.3936536824085963 * fConst3)));
		fConst166 = (0.0 - (0.70710678118654757 * (fConst157 * fConst165)));
		fConst167 = (0.0 - (0.92131773192356159 * fConst161));
		fConst168 = (0.0 - (0.92131773192356192 * fConst161));
		fConst169 = (0.0 - ((2.8867513459481291 * fConst40) + (2.7639531957706835 * fConst3)));
		fConst170 = (fConst157 * fConst169);
		fConst171 = (0.0 - (0.5 * fConst170));
		fConst172 = (0.0 - (0.50000000000000011 * fConst170));
		fConst173 = (0.057314497376279992 * fConst3);
		fConst174 = (0.0089791337293529845 * fConst158);
		fConst175 = (0.011287477954144622 * fConst158);
		fConst176 = (0.011287477954144623 * fConst158);
		fConst177 = (0.015962904407738641 * fConst165);
		fConst178 = (0.019434045907762629 * fConst3);
		fConst179 = (0.019434045907762633 * fConst3);
		fConst180 = (0.010546875000000001 * fConst169);
		fConst181 = (0.010546875000000002 * fConst169);
		fConst182 = (0.045001699736364593 * fConst3);
		fConst183 = (0.0 - (1.7605101637794649 * fConst3));
		fConst184 = (0.0045108949595698937 * fConst183);
		fConst185 = (0.013532684878709664 * fConst183);
		fConst186 = (0.013532684878709665 * fConst183);
		fConst187 = (0.0045108949595698946 * fConst183);
		fConst188 = (0.0 - ((1.5075567228888176 * fConst54) + (3.2276019669290172 * fConst3)));
		fConst189 = (0.0045108949595698937 * fConst188);
		fConst190 = (0.013532684878709664 * fConst188);
		fConst191 = (0.013532684878709665 * fConst188);
		fConst192 = (0.0045108949595698946 * fConst188);
		fConst193 = (0.0 - ((4.5226701686664557 * fConst59) + (1.4434274804730605 * fConst3)));
		fConst194 = (0.010086667768959437 * fConst193);
		fConst195 = (0.0 - (2.230751560731095 * fConst161));
		fConst196 = (0.0 - (2.2307515607310959 * fConst161));
		fConst197 = (fConst157 * fConst183);
		fConst198 = (0.0 - (0.22360679774997916 * fConst197));
		fConst199 = (0.0 - (0.67082039324993692 * fConst197));
		fConst200 = (0.0 - (0.67082039324993703 * fConst197));
		fConst201 = (0.0 - (0.22360679774997969 * fConst197));
		fConst202 = (fConst157 * fConst188);
		fConst203 = (0.0 - (0.22360679774997916 * fConst202));
		fConst204 = (0.0 - (0.67082039324993692 * fConst202));
		fConst205 = (0.0 - (0.67082039324993703 * fConst202));
		fConst206 = (0.0 - (0.22360679774997969 * fConst202));
		fConst207 = (fConst157 * fConst193);
		fConst208 = (0.0 - (0.5 * fConst207));
		fConst209 = (0.0 - (0.50000000000000011 * fConst207));
		fConst210 = (0.01128747795414462 * fConst158);
		fConst211 = (0.019434045907762626 * fConst3);
		fConst212 = (0.010546874999999999 * fConst169);
		fConst213 = (0.045001699736364587 * fConst3);
		fConst214 = (0.045001699736364573 * fConst3);
		fConst215 = (0.013532684878709662 * fConst183);
		fConst216 = (0.0045108949595698885 * fConst183);
		fConst217 = (0.004510894959569885 * fConst183);
		fConst218 = (0.01353268487870966 * fConst183);
		fConst219 = (0.013532684878709662 * fConst188);
		fConst220 = (0.0045108949595698885 * fConst188);
		fConst221 = (0.004510894959569885 * fConst188);
		fConst222 = (0.01353268487870966 * fConst188);
		fConst223 = (0.010086667768959435 * fConst193);
		fConst224 = (0.010086667768959433 * fConst193);
		fConst225 = (0.0 - (0.22360679774997891 * fConst197));
		fConst226 = (0.0 - (0.67082039324993681 * fConst197));
		fConst227 = (0.0 - (0.22360679774997907 * fConst197));
		fConst228 = (0.0 - (0.22360679774997891 * fConst202));
		fConst229 = (0.0 - (0.67082039324993681 * fConst202));
		fConst230 = (0.0 - (0.22360679774997907 * fConst202));
		fConst231 = (0.0 - (5.8273688116286149e-16 * fConst124));
		fConst232 = (0.0 - (1.165473762325723e-15 * fConst124));
		fConst233 = (0.0 - (3.4992181344801496 * fConst124));
		fConst234 = (0.0 - (3.4992181344801501 * fConst124));
		fConst235 = (0.0 - (3.4992181344801487 * fConst124));
		fConst236 = (0.0 - (3.4992181344801505 * fConst124));
		fConst237 = (0.0 - (1.1313708498984762 * fConst131));
		fConst238 = (0.0 - (1.1313708498984758 * fConst131));
		fConst239 = (0.0 - (1.1313708498984765 * fConst131));
		fConst240 = (0.0 - (3.1401849173675503e-16 * fConst137));
		fConst241 = (0.0 - (6.2803698347351007e-16 * fConst137));
		fConst242 = (0.0 - (5.6418958354775643 * fConst124));
		fConst243 = (0.0 - (3.9894228040143274 * fConst21));
		fConst244 = (0.0 - (1.4142135623730951 * (fConst123 * fConst243)));
		fConst245 = (0.0 - (1.3638242119692871e-15 * fConst124));
		fConst246 = (0.0 - (2.7276484239385742e-15 * fConst124));
		fConst247 = (0.0 - (3.1401849173675503e-16 * fConst141));
		fConst248 = (0.0 - (6.2803698347351007e-16 * fConst141));
		fConst249 = (0.12736554972506672 * fConst21);
		fConst250 = (0.031925808815477283 * fConst243);
		fConst251 = (2.8768166971227152e-17 * fConst21);
		fConst252 = (5.7536333942454304e-17 * fConst21);
		fConst253 = (6.6238275600721768e-18 * fConst140);
		fConst254 = (1.3247655120144354e-17 * fConst140);
		fConst255 = (2.3511493268037518e-17 * fConst21);
		fConst256 = (0.070590901547238516 * fConst21);
		fConst257 = (0.070590901547238558 * fConst21);
		fConst258 = (0.07059090154723846 * fConst21);
		fConst259 = (0.022823523772822384 * fConst130);
		fConst260 = (0.022823523772822398 * fConst130);
		fConst261 = (0.022823523772822367 * fConst130);
		fConst262 = (1.2669600797833527e-17 * fConst136);
		fConst263 = (0.0 - (0.34992181344801404 * fConst109));
		fConst264 = (0.0 - (0.34992181344801415 * fConst109));
		fConst265 = (0.0 - (4.548983574824196 * fConst109));
		fConst266 = (0.0 - (3.1492963210321285 * fConst109));
		fConst267 = (0.0 - (3.1492963210321339 * fConst109));
		fConst268 = (0.0 - (4.5489835748241934 * fConst109));
		fConst269 = (0.0 - (4.5135166683820485 * fConst109));
		fConst270 = (0.0 - (4.5135166683820538 * fConst109));
		fConst271 = (0.0 - (2.4568472851294949 * fConst109));
		fConst272 = (0.0 - (2.4568472851294976 * fConst109));
		fConst273 = (double(tgamma(4.5)) * sqrt((9.0 * (fConst1 / double(tgamma(9.0))))));
		fConst274 = (0.0 - (1.9878387281720961e-16 * fConst273));
		fConst275 = (0.0 - (10.128518061100836 * fConst273));
		fConst276 = (0.0 - (10.128518061100845 * fConst273));
		fConst277 = (0.0 - (10.12851806110084 * fConst273));
		fConst278 = (0.0 - (10.12851806110085 * fConst273));
		fConst279 = sqrt((9.0 * (fConst31 / double(tgamma(8.0)))));
		fConst280 = (fConst108 * fConst279);
		fConst281 = (0.0 - (11.170383851240118 * fConst280));
		fConst282 = (0.0 - (7.898654169668589 * fConst108));
		fConst283 = (0.0 - (1.4142135623730951 * (fConst279 * fConst282)));
		fConst284 = (0.0 - (9.9292300899912185 * fConst280));
		fConst285 = (0.0 - (9.9292300899912025 * fConst280));
		fConst286 = (0.0 - (9.9292300899912096 * fConst108));
		fConst287 = (fConst279 * fConst286);
		fConst288 = (0.0 - (1.0000000000000009 * fConst287));
		fConst289 = (0.0 - (0.99999999999999933 * fConst287));
		fConst290 = (0.25217092292041182 * fConst108);
		fConst291 = (0.031925808815477283 * fConst282);
		fConst292 = (0.20944469721075218 * fConst108);
		fConst293 = (0.20944469721075201 * fConst108);
		fConst294 = (0.021093750000000012 * fConst286);
		fConst295 = (0.021093749999999994 * fConst286);
		fConst296 = (0.044696311044975172 * fConst108);
		fConst297 = (0.19368401452822653 * fConst108);
		fConst298 = (0.19368401452822659 * fConst108);
		fConst299 = (0.13408893313492587 * fConst108);
		fConst300 = (0.13408893313492612 * fConst108);
		fConst301 = (0.0 - (8.2571033474838984 * fConst108));
		fConst302 = (0.023456653789763436 * fConst301);
		fConst303 = (0.02345665378976344 * fConst301);
		fConst304 = (0.016239221854451587 * fConst301);
		fConst305 = (0.016239221854451615 * fConst301);
		fConst306 = (0.0 - (2.2156133258658013 * fConst108));
		fConst307 = (0.020173335537918863 * fConst306);
		fConst308 = (0.0 - (2.2156133258658035 * fConst280));
		fConst309 = (0.0 - (2.2156133258658 * fConst280));
		fConst310 = (0.0 - (6.6468399775974225 * fConst280));
		fConst311 = (0.0 - (6.6468399775974278 * fConst280));
		fConst312 = (0.0 - (9.6009910787518393 * fConst280));
		fConst313 = (0.0 - (9.6009910787518553 * fConst280));
		fConst314 = (fConst279 * fConst301);
		fConst315 = (0.0 - (0.80498447189992428 * fConst314));
		fConst316 = (0.0 - (0.80498447189992484 * fConst314));
		fConst317 = (0.0 - (1.1627553482998914 * fConst314));
		fConst318 = (0.0 - (1.1627553482998934 * fConst314));
		fConst319 = (fConst279 * fConst306);
		fConst320 = (0.0 - (1.0000000000000009 * fConst319));
		fConst321 = (0.0 - (0.99999999999999933 * fConst319));
		fConst322 = sqrt((9.0 * (fConst2 / fConst107)));
		fConst323 = (fConst21 * fConst322);
		fConst324 = (0.0 - (6.8578507705370146 * fConst323));
		fConst325 = (0.0 - (3.7301790647785582 * fConst21));
		fConst326 = (fConst322 * fConst325);
		fConst327 = (0.0 - (0.42426406871192845 * fConst326));
		fConst328 = (0.0 - (0.42426406871192907 * fConst326));
		fConst329 = (0.0 - (0.42426406871192829 * fConst326));
		fConst330 = (0.0 - (0.42426406871192973 * fConst326));
		fConst331 = (0.0 - ((2.1105794120443448 * fConst130) + (10.257992428141025 * fConst21)));
		fConst332 = (fConst322 * fConst331);
		fConst333 = (0.0 - (0.42426406871192845 * fConst332));
		fConst334 = (0.0 - (0.42426406871192907 * fConst332));
		fConst335 = (0.0 - (0.42426406871192829 * fConst332));
		fConst336 = (0.0 - (0.42426406871192973 * fConst332));
		fConst337 = (0.0 - ((6.3317382361330381 * fConst136) + (2.0515984856282001 * fConst21)));
		fConst338 = (fConst322 * fConst337);
		fConst339 = (0.0 - (0.70710678118654757 * fConst338));
		fConst340 = (0.0 - (11.283791670955125 * fConst21));
		fConst341 = (fConst322 * fConst340);
		fConst342 = (0.0 - (0.70710678118654757 * fConst341));
		fConst343 = (0.022574955908289243 * fConst342);
		fConst344 = (0.0 - (7.0923072071365816 * fConst323));
		fConst345 = (0.0 - ((4.0414518843273806 * fConst140) + (7.5225277806367483 * fConst21)));
		fConst346 = (fConst322 * fConst345);
		fConst347 = (0.0 - (0.70710678118654757 * fConst346));
		fConst348 = (0.14960335515053727 * fConst21);
		fConst349 = (0.14960335515053733 * fConst21);
		fConst350 = (0.014915533665653739 * fConst345);
		fConst351 = (0.01491553366565374 * fConst345);
		fConst352 = (0.13834572466301867 * fConst21);
		fConst353 = (0.1383457246630187 * fConst21);
		fConst354 = (0.008558821414808411 * fConst325);
		fConst355 = (0.0085588214148084006 * fConst325);
		fConst356 = (0.0085588214148084076 * fConst325);
		fConst357 = (0.0085588214148084041 * fConst325);
		fConst358 = (0.008558821414808411 * fConst331);
		fConst359 = (0.0085588214148084006 * fConst331);
		fConst360 = (0.0085588214148084076 * fConst331);
		fConst361 = (0.0085588214148084041 * fConst331);
		fConst362 = (0.014264702358014003 * fConst337);
		fConst363 = (0.014264702358014004 * fConst337);
		fConst364 = sqrt((9.0 * (fConst32 / fConst122)));
		fConst365 = (fConst3 * fConst364);
		fConst366 = (0.0 - (0.99735570100357918 * fConst365));
		fConst367 = (0.0 - ((2.4748737341529159 * fConst165) + (4.0 * fConst36)));
		fConst368 = (0.0 - (0.47140452079103168 * (fConst364 * fConst367)));
		fConst369 = (0.0 - (2.6596152026762163 * fConst3));
		fConst370 = (fConst364 * fConst369);
		fConst371 = (0.0 - (0.33333333333333331 * fConst370));
		fConst372 = (0.0 - (0.33333333333333343 * fConst370));
		fConst373 = (0.0 - ((2.0207259421636903 * fConst169) + (4.0 * fConst40)));
		fConst374 = (fConst364 * fConst373);
		fConst375 = (0.0 - (0.33333333333333331 * fConst374));
		fConst376 = (0.0 - (0.33333333333333343 * fConst374));
		fConst377 = (0.02251526097503671 * fConst3);
		fConst378 = (0.010641936271825759 * fConst367);
		fConst379 = (0.0070312500000000002 * fConst369);
		fConst380 = (0.007031250000000001 * fConst369);
		fConst381 = (0.0070312500000000002 * fConst373);
		fConst382 = (0.007031250000000001 * fConst373);
		fConst383 = (0.059861130863806261 * fConst3);
		fConst384 = ((1.0552897060221724 * fConst183) - (3.8926344352424089 * fConst3));
		fConst385 = (0.0030072633063799288 * fConst384);
		fConst386 = (0.0090217899191397753 * fConst384);
		fConst387 = (0.009021789919139777 * fConst384);
		fConst388 = (0.0030072633063799293 * fConst384);
		fConst389 = (0.0 - ((1.0552897060221724 * fConst188) + (4.0 * fConst54)));
		fConst390 = (0.0030072633063799288 * fConst389);
		fConst391 = (0.0090217899191397753 * fConst389);
		fConst392 = (0.009021789919139777 * fConst389);
		fConst393 = (0.0030072633063799293 * fConst389);
		fConst394 = (0.0 - ((3.1658691180665191 * fConst193) + (4.0 * fConst59)));
		fConst395 = (0.0067244451793062911 * fConst394);
		fConst396 = (0.0 - (2.9673392757131363 * fConst365));
		fConst397 = (0.0 - (2.9673392757131376 * fConst365));
		fConst398 = (fConst364 * fConst384);
		fConst399 = (0.0 - (0.1490711984999861 * fConst398));
		fConst400 = (0.0 - (0.44721359549995793 * fConst398));
		fConst401 = (0.0 - (0.44721359549995798 * fConst398));
		fConst402 = (0.0 - (0.14907119849998646 * fConst398));
		fConst403 = (fConst364 * fConst389);
		fConst404 = (0.0 - (0.1490711984999861 * fConst403));
		fConst405 = (0.0 - (0.44721359549995793 * fConst403));
		fConst406 = (0.0 - (0.44721359549995798 * fConst403));
		fConst407 = (0.0 - (0.14907119849998646 * fConst403));
		fConst408 = (fConst364 * fConst394);
		fConst409 = (0.0 - (0.33333333333333331 * fConst408));
		fConst410 = (0.0 - (0.33333333333333343 * fConst408));
		fConst411 = (0.0070312499999999993 * fConst369);
		fConst412 = (0.0070312499999999993 * fConst373);
		fConst413 = (0.059861130863806254 * fConst3);
		fConst414 = (0.05986113086380624 * fConst3);
		fConst415 = (0.0030072633063799254 * fConst384);
		fConst416 = (0.0030072633063799232 * fConst384);
		fConst417 = (0.0090217899191397718 * fConst384);
		fConst418 = (0.0030072633063799254 * fConst389);
		fConst419 = (0.0030072633063799232 * fConst389);
		fConst420 = (0.0090217899191397718 * fConst389);
		fConst421 = (0.0067244451793062893 * fConst394);
		fConst422 = (0.0067244451793062885 * fConst394);
		fConst423 = (0.0 - (0.14907119849998593 * fConst398));
		fConst424 = (0.0 - (0.44721359549995787 * fConst398));
		fConst425 = (0.0 - (0.14907119849998604 * fConst398));
		fConst426 = (0.0 - (0.14907119849998593 * fConst403));
		fConst427 = (0.0 - (0.44721359549995787 * fConst403));
		fConst428 = (0.0 - (0.14907119849998604 * fConst403));
		fConst429 = (0.0 - (1.522748764978713e-15 * fConst323));
		fConst430 = (0.0 - (3.0454975299574259e-15 * fConst323));
		fConst431 = (0.0 - (0.56568542494923812 * fConst326));
		fConst432 = (0.0 - (0.5656854249492379 * fConst326));
		fConst433 = (0.0 - (0.56568542494923824 * fConst326));
		fConst434 = (0.0 - (0.56568542494923812 * fConst332));
		fConst435 = (0.0 - (0.5656854249492379 * fConst332));
		fConst436 = (0.0 - (0.56568542494923824 * fConst332));
		fConst437 = (0.0 - (1.5700924586837752e-16 * fConst338));
		fConst438 = (0.0 - (3.1401849173675503e-16 * fConst338));
		fConst439 = (0.012698412698412698 * fConst342);
		fConst440 = (0.0 - (9.9735570100358153 * fConst323));
		fConst441 = (0.0 - (1.5700924586837752e-16 * fConst341));
		fConst442 = (0.0 - (3.1401849173675503e-16 * fConst341));
		fConst443 = (0.0 - ((4.9497474683058318 * fConst243) + (5.6418958354775643 * fConst21)));
		fConst444 = (0.0 - (0.70710678118654757 * (fConst322 * fConst443)));
		fConst445 = (0.0 - (1.5748085518155944e-15 * fConst323));
		fConst446 = (0.0 - (3.1496171036311889e-15 * fConst323));
		fConst447 = (0.0 - (1.5700924586837752e-16 * fConst346));
		fConst448 = (0.0 - (3.1401849173675503e-16 * fConst346));
		fConst449 = (0.0089791337293529845 * fConst340);
		fConst450 = (0.22515260975036763 * fConst21);
		fConst451 = (3.5444768026723675e-18 * fConst340);
		fConst452 = (7.088953605344735e-18 * fConst340);
		fConst453 = (0.015962904407738641 * fConst443);
		fConst454 = (3.3218617889860196e-17 * fConst21);
		fConst455 = (6.6437235779720392e-17 * fConst21);
		fConst456 = (3.3119137800360884e-18 * fConst345);
		fConst457 = (6.6238275600721768e-18 * fConst345);
		fConst458 = (6.1437843551734282e-17 * fConst21);
		fConst459 = (0.011411761886411192 * fConst325);
		fConst460 = (0.011411761886411199 * fConst325);
		fConst461 = (0.011411761886411183 * fConst325);
		fConst462 = (0.011411761886411192 * fConst331);
		fConst463 = (0.011411761886411199 * fConst331);
		fConst464 = (0.011411761886411183 * fConst331);
		fConst465 = (6.3348003989167635e-18 * fConst337);
		fConst466 = (0.0 - (2.2156133258658022 * fConst280));
		fConst467 = (0.0 - (2.2156133258658031 * fConst280));
		fConst468 = (0.0 - (9.600991078751834 * fConst280));
		fConst469 = (0.0 - (6.6468399775974083 * fConst280));
		fConst470 = (0.0 - (6.6468399775974198 * fConst280));
		fConst471 = (0.0 - (9.6009910787518287 * fConst280));
		fConst472 = (0.0 - (1.1627553482998907 * fConst314));
		fConst473 = (0.0 - (0.8049844718999225 * fConst314));
		fConst474 = (0.0 - (0.80498447189992395 * fConst314));
		fConst475 = (0.0 - (1.1627553482998902 * fConst314));
		fConst476 = (0.0 - (1.0000000000000004 * fConst319));
		fConst477 = (0.0 - (1.0000000000000007 * fConst319));
		fConst478 = (0.0 - (9.929230089991206 * fConst280));
		fConst479 = (0.0 - (9.9292300899912167 * fConst280));
		fConst480 = (0.0 - (0.99999999999999967 * fConst287));
		fConst481 = (0.0 - (1.0000000000000007 * fConst287));
		fConst482 = (0.20944469721075193 * fConst108);
		fConst483 = (0.021093749999999988 * fConst286);
		fConst484 = (0.044696311044975207 * fConst108);
		fConst485 = (0.044696311044975165 * fConst108);
		fConst486 = (0.13408893313492565 * fConst108);
		fConst487 = (0.19368401452822634 * fConst108);
		fConst488 = (0.19368401452822642 * fConst108);
		fConst489 = (0.13408893313492534 * fConst108);
		fConst490 = (0.01623922185445156 * fConst301);
		fConst491 = (0.023456653789763415 * fConst301);
		fConst492 = (0.023456653789763426 * fConst301);
		fConst493 = (0.016239221854451522 * fConst301);
		fConst494 = (0.02017333553791888 * fConst306);
		fConst495 = (0.020173335537918856 * fConst306);
		fConst496 = (0.0 - (12.766152972845845 * fConst273));
		fConst497 = (0.0 - (12.766152972845846 * fConst273));
		fConst498 = (0.0 - (5.6738457657092614 * fConst273));
		fConst499 = (0.0 - (5.6738457657092622 * fConst273));
		fConst500 = (0.02017333553791887 * (0.0 - (0.42202158587919975 * fConst273)));
		
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.0);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec1[l0] = 0.0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec0[l1] = 0.0;
			
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
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec5[l5] = 0.0;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec6[l6] = 0.0;
			
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec7[l7] = 0.0;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec8[l8] = 0.0;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fRec9[l9] = 0.0;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec10[l10] = 0.0;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec11[l11] = 0.0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec12[l12] = 0.0;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec13[l13] = 0.0;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec14[l14] = 0.0;
			
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
			fRec18[l18] = 0.0;
			
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec19[l19] = 0.0;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec20[l20] = 0.0;
			
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec21[l21] = 0.0;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec22[l22] = 0.0;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec23[l23] = 0.0;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec24[l24] = 0.0;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec25[l25] = 0.0;
			
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
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("Outputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fVbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36e3900", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x36fd560", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3707f80", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3719d10", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x372ed00", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3751ae0", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x375c970", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3773880", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x378bc00", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37a55a0", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37c2960", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37ebae0", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x37f2890", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x380ad90", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3828e70", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x383efd0", &fVbargraph15, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fVbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38530d0", &fVbargraph16, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fVbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3876150", &fVbargraph17, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fVbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3896e70", &fVbargraph18, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fVbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38bb290", &fVbargraph19, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fVbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38c2d20", &fVbargraph20, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fVbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38d8950", &fVbargraph21, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fVbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x38f9600", &fVbargraph22, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fVbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3916590", &fVbargraph23, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fVbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3928fb0", &fVbargraph24, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(&fHslider0, "1", "");
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->declare(&fHslider0, "unit", "dB");
		ui_interface->addHorizontalSlider("Gain", &fHslider0, 0.0, -10.0, 50.0, 0.10000000000000001);
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
		double fSlow0 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fHslider0))));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			double fTemp0 = double(input0[i]);
			double fTemp1 = double(input1[i]);
			double fTemp2 = double(input2[i]);
			double fTemp3 = double(input3[i]);
			double fTemp4 = double(input4[i]);
			double fTemp5 = double(input5[i]);
			double fTemp6 = double(input6[i]);
			double fTemp7 = double(input7[i]);
			double fTemp8 = double(input8[i]);
			double fTemp9 = double(input9[i]);
			double fTemp10 = (((fTemp6 + fTemp7) + fTemp8) + fTemp9);
			double fTemp11 = double(input10[i]);
			double fTemp12 = double(input11[i]);
			double fTemp13 = double(input12[i]);
			double fTemp14 = double(input13[i]);
			double fTemp15 = double(input14[i]);
			double fTemp16 = double(input15[i]);
			double fTemp17 = double(input16[i]);
			double fTemp18 = double(input17[i]);
			double fTemp19 = double(input18[i]);
			double fTemp20 = double(input19[i]);
			double fTemp21 = double(input20[i]);
			double fTemp22 = double(input21[i]);
			double fTemp23 = (((fTemp19 + fTemp20) + fTemp21) + fTemp22);
			double fTemp24 = double(input22[i]);
			double fTemp25 = double(input23[i]);
			double fTemp26 = double(input24[i]);
			double fTemp27 = double(input25[i]);
			double fTemp28 = ((((fTemp23 + fTemp24) + fTemp25) + fTemp26) + fTemp27);
			double fTemp29 = double(input26[i]);
			double fTemp30 = double(input27[i]);
			double fTemp31 = (fTemp29 + fTemp30);
			double fTemp32 = double(input28[i]);
			double fTemp33 = double(input29[i]);
			double fTemp34 = ((fTemp31 + fTemp32) + fTemp33);
			double fTemp35 = double(input30[i]);
			double fTemp36 = double(input31[i]);
			double fTemp37 = double(input32[i]);
			double fTemp38 = double(input33[i]);
			double fTemp39 = double(input34[i]);
			double fTemp40 = double(input35[i]);
			double fTemp41 = double(input36[i]);
			double fTemp42 = double(input37[i]);
			double fTemp43 = double(input38[i]);
			double fTemp44 = double(input39[i]);
			double fTemp45 = double(input40[i]);
			double fTemp46 = double(input41[i]);
			double fTemp47 = double(input42[i]);
			double fTemp48 = double(input43[i]);
			double fTemp49 = double(input44[i]);
			double fTemp50 = double(input45[i]);
			double fTemp51 = double(input46[i]);
			double fTemp52 = double(input47[i]);
			double fTemp53 = double(input48[i]);
			double fTemp54 = double(input49[i]);
			double fTemp55 = (fRec1[0] * ((((0.012698412698412698 * (((((fTemp0 + fTemp1) + fTemp2) + fTemp3) + fTemp4) + fTemp5)) + (0.022574955908289243 * ((((((((fTemp10 + fTemp11) + fTemp12) + fTemp13) + fTemp14) + fTemp15) + fTemp16) + fTemp17) + fTemp18))) + (0.021093750000000001 * fTemp28)) + (0.02017333553791887 * ((((((((((((((((((((fTemp34 + fTemp35) + fTemp36) + fTemp37) + fTemp38) + fTemp39) + fTemp40) + fTemp41) + fTemp42) + fTemp43) + fTemp44) + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) + fTemp50) + fTemp51) + fTemp52) + fTemp53) + fTemp54))));
			fRec0[0] = max((fRec0[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp55))))));
			fVbargraph0 = FAUSTFLOAT(fRec0[0]);
			output0[i] = FAUSTFLOAT(fTemp55);
			double fTemp56 = (fRec1[0] * (((((fConst5 * fTemp4) + (0.022574955908289243 * ((((fConst6 * fTemp9) + (fConst7 * fTemp13)) + (fConst8 * fTemp14)) + (fConst6 * fTemp18)))) + (0.021093750000000001 * ((((fConst9 * fTemp21) + (fConst10 * fTemp22)) + (fConst9 * fTemp26)) + (fConst10 * fTemp27)))) + (fConst4 * (((((((((((((((((((((0.02026373487753309 * fTemp2) + (0.025473109945013343 * fTemp7)) + (0.025473109945013336 * fTemp11)) + (0.025473109945013343 * fTemp12)) + (0.025473109945013343 * fTemp16)) + (0.019434045907762619 * fTemp19)) + (0.019434045907762623 * fTemp20)) + (0.019434045907762619 * fTemp24)) + (0.019434045907762623 * fTemp25)) + (0.0097062489627452973 * fTemp29)) + (0.0097062489627452973 * fTemp30)) + (0.0097062489627453199 * fTemp35)) + (0.029118746888235923 * fTemp36)) + (0.029118746888235927 * fTemp37)) + (0.0097062489627453216 * fTemp38)) + (0.0097062489627453199 * fTemp43)) + (0.029118746888235923 * fTemp44)) + (0.029118746888235927 * fTemp45)) + (0.0097062489627453216 * fTemp46)) + (0.0097062489627452973 * fTemp51)) + (0.0097062489627452973 * fTemp52)))) + (0.02017333553791887 * ((((((((((((fConst11 * fTemp32) + (fConst12 * fTemp33)) + (fConst13 * fTemp39)) + (fConst14 * fTemp40)) + (fConst15 * fTemp41)) + (fConst16 * fTemp42)) + (fConst13 * fTemp47)) + (fConst14 * fTemp48)) + (fConst15 * fTemp49)) + (fConst16 * fTemp50)) + (fConst11 * fTemp53)) + (fConst12 * fTemp54)))));
			fRec2[0] = max((fRec2[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fVbargraph1 = FAUSTFLOAT(fRec2[0]);
			output1[i] = FAUSTFLOAT(fTemp56);
			double fTemp57 = (fTemp0 - fTemp5);
			double fTemp58 = ((((fTemp10 - fTemp15) - fTemp16) - fTemp17) - fTemp18);
			double fTemp59 = ((((fTemp23 - fTemp24) - fTemp25) - fTemp26) - fTemp27);
			double fTemp60 = (((((((fTemp35 + fTemp36) + fTemp37) + fTemp38) + fTemp39) + fTemp40) + fTemp41) + fTemp42);
			double fTemp61 = ((((((((fTemp60 - fTemp43) - fTemp44) - fTemp45) - fTemp46) - fTemp47) - fTemp48) - fTemp49) - fTemp50);
			double fTemp62 = ((((fTemp34 - fTemp51) - fTemp52) - fTemp53) - fTemp54);
			double fTemp63 = (fRec1[0] * (((((0.021994295969128601 * fTemp57) + (0.027648561470568499 * fTemp58)) + (0.021093750000000001 * fTemp59)) + (0.010535180889746075 * fTemp61)) + (0.031605542669238249 * fTemp62)));
			fRec3[0] = max((fRec3[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fVbargraph2 = FAUSTFLOAT(fRec3[0]);
			output2[i] = FAUSTFLOAT(fTemp63);
			double fTemp64 = (fRec1[0] * ((fConst4 * (((((((((((((((((((((0.02026373487753309 * fTemp1) + (0.025473109945013343 * fTemp6)) + (0.025473109945013343 * fTemp11)) + (0.025473109945013332 * fTemp14)) + (0.025473109945013343 * fTemp15)) + (0.019434045907762623 * fTemp19)) + (0.019434045907762616 * fTemp22)) + (0.019434045907762623 * fTemp24)) + (0.019434045907762616 * fTemp27)) + (0.0097062489627452956 * fTemp29)) + (0.0097062489627452939 * fTemp33)) + (0.029118746888235916 * fTemp35)) + (0.0097062489627453077 * fTemp36)) + (0.0097062489627453008 * fTemp41)) + (0.029118746888235913 * fTemp42)) + (0.029118746888235916 * fTemp43)) + (0.0097062489627453077 * fTemp44)) + (0.0097062489627453008 * fTemp49)) + (0.029118746888235913 * fTemp50)) + (0.0097062489627452956 * fTemp51)) + (0.0097062489627452939 * fTemp54))) + ((((fConst5 * fTemp3) + (0.022574955908289243 * ((((fConst6 * fTemp8) + (fConst7 * fTemp12)) + (fConst8 * fTemp13)) + (fConst6 * fTemp17)))) + (0.021093750000000001 * ((((fConst9 * fTemp20) + (fConst10 * fTemp21)) + (fConst9 * fTemp25)) + (fConst10 * fTemp26)))) + (0.02017333553791887 * ((((((((((((fConst11 * fTemp30) + (fConst12 * fTemp32)) + (fConst17 * fTemp37)) + (fConst18 * fTemp38)) + (fConst14 * fTemp39)) + (fConst19 * fTemp40)) + (fConst17 * fTemp45)) + (fConst18 * fTemp46)) + (fConst14 * fTemp47)) + (fConst19 * fTemp48)) + (fConst11 * fTemp52)) + (fConst12 * fTemp53))))));
			fRec4[0] = max((fRec4[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fVbargraph3 = FAUSTFLOAT(fRec4[0]);
			output3[i] = FAUSTFLOAT(fTemp64);
			double fTemp65 = (fTemp30 + fTemp33);
			double fTemp66 = (fTemp12 + fTemp14);
			double fTemp67 = (fTemp20 + fTemp22);
			double fTemp68 = (fRec1[0] * ((0.02017333553791887 * (((((((((((fConst23 * fTemp65) + (fConst24 * fTemp37)) + (fConst25 * fTemp38)) + (fConst26 * fTemp41)) + (fConst27 * fTemp42)) + (fConst24 * fTemp45)) + (fConst25 * fTemp46)) + (fConst26 * fTemp49)) + (fConst27 * fTemp50)) + (fConst23 * fTemp52)) + (fConst23 * fTemp54))) + (((fConst29 * fTemp66) + (fConst30 * ((fTemp67 + fTemp25) + fTemp27))) + (fConst22 * ((((((((((((((((((0.072048835120117657 * fTemp11) + (0.072048835120117671 * fTemp13)) + (0.044881006545161167 * fTemp19)) + (0.044881006545161174 * fTemp21)) + (0.044881006545161167 * fTemp24)) + (0.044881006545161174 * fTemp26)) + (0.011706176702255417 * fTemp29)) + (0.011706176702255421 * fTemp32)) + (0.035118530106766375 * fTemp35)) + (0.035118530106766334 * fTemp36)) + (0.035118530106766362 * fTemp39)) + (0.035118530106766348 * fTemp40)) + (0.035118530106766375 * fTemp43)) + (0.035118530106766334 * fTemp44)) + (0.035118530106766362 * fTemp47)) + (0.035118530106766348 * fTemp48)) + (0.011706176702255417 * fTemp51)) + (0.011706176702255421 * fTemp53))))));
			fRec5[0] = max((fRec5[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fVbargraph4 = FAUSTFLOAT(fRec5[0]);
			output4[i] = FAUSTFLOAT(fTemp68);
			double fTemp69 = (fConst46 * fTemp19);
			double fTemp70 = (fRec1[0] * ((((0.022574955908289243 * ((fConst35 * fTemp9) + (fConst37 * fTemp18))) + (0.021093750000000001 * ((((fConst38 * fTemp21) + (fConst39 * fTemp22)) + (fConst42 * fTemp26)) + (fConst43 * fTemp27)))) + (fConst33 * ((((((((((((((((((fConst44 * fTemp7) + (fConst45 * fTemp16)) + fTemp69) + (fConst46 * fTemp20)) + (fConst47 * fTemp24)) + (fConst48 * fTemp25)) + (fConst49 * fTemp29)) + (fConst49 * fTemp30)) + (fConst50 * fTemp35)) + (fConst51 * fTemp36)) + (fConst52 * fTemp37)) + (fConst53 * fTemp38)) + (fConst55 * fTemp43)) + (fConst56 * fTemp44)) + (fConst57 * fTemp45)) + (fConst58 * fTemp46)) + (fConst60 * fTemp51)) + (fConst60 * fTemp52)))) + (0.02017333553791887 * ((((((((((((fConst61 * fTemp32) + (fConst62 * fTemp33)) + (fConst63 * fTemp39)) + (fConst64 * fTemp40)) + (fConst65 * fTemp41)) + (fConst66 * fTemp42)) + (fConst68 * fTemp47)) + (fConst69 * fTemp48)) + (fConst70 * fTemp49)) + (fConst71 * fTemp50)) + (fConst73 * fTemp53)) + (fConst74 * fTemp54)))));
			fRec6[0] = max((fRec6[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp70))))));
			fVbargraph5 = FAUSTFLOAT(fRec6[0]);
			output5[i] = FAUSTFLOAT(fTemp70);
			double fTemp71 = ((((fTemp10 + fTemp15) + fTemp16) + fTemp17) + fTemp18);
			double fTemp72 = (fTemp0 + fTemp5);
			double fTemp73 = ((((fTemp34 + fTemp51) + fTemp52) + fTemp53) + fTemp54);
			double fTemp74 = (((fTemp1 + fTemp2) + fTemp3) + fTemp4);
			double fTemp75 = (((fTemp11 + fTemp12) + fTemp13) + fTemp14);
			double fTemp76 = ((((((((fTemp60 + fTemp43) + fTemp44) + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) + fTemp50);
			double fTemp77 = (fRec1[0] * ((((0.01261978399999881 * fTemp71) + ((0.028394513999997331 * fTemp72) + (5.236595479505967e-18 * fTemp28))) + (0.032806508796871948 * fTemp73)) - (((0.014197256999998666 * fTemp74) + (0.025239567999997631 * fTemp75)) + (0.01640325439843596 * fTemp76))));
			fRec7[0] = max((fRec7[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp77))))));
			fVbargraph6 = FAUSTFLOAT(fRec7[0]);
			output6[i] = FAUSTFLOAT(fTemp77);
			double fTemp78 = (fRec1[0] * ((fConst33 * ((((((((((((((((fTemp69 + ((fConst44 * fTemp6) + (fConst45 * fTemp15))) + (fConst75 * fTemp22)) + (fConst48 * fTemp24)) + (fConst76 * fTemp27)) + (fConst77 * fTemp29)) + (fConst78 * fTemp33)) + (fConst79 * fTemp35)) + (fConst80 * fTemp36)) + (fConst81 * fTemp41)) + (fConst82 * fTemp42)) + (fConst83 * fTemp43)) + (fConst84 * fTemp44)) + (fConst85 * fTemp49)) + (fConst86 * fTemp50)) + (fConst87 * fTemp51)) + (fConst88 * fTemp54))) + (((0.022574955908289243 * ((fConst35 * fTemp8) + (fConst37 * fTemp17))) + (0.021093750000000001 * ((((fConst38 * fTemp20) + (fConst39 * fTemp21)) + (fConst42 * fTemp25)) + (fConst43 * fTemp26)))) + (0.02017333553791887 * ((((((((((((fConst61 * fTemp30) + (fConst62 * fTemp32)) + (fConst89 * fTemp37)) + (fConst90 * fTemp38)) + (fConst64 * fTemp39)) + (fConst91 * fTemp40)) + (fConst92 * fTemp45)) + (fConst93 * fTemp46)) + (fConst69 * fTemp47)) + (fConst94 * fTemp48)) + (fConst73 * fTemp52)) + (fConst74 * fTemp53))))));
			fRec8[0] = max((fRec8[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fVbargraph7 = FAUSTFLOAT(fRec8[0]);
			output7[i] = FAUSTFLOAT(fTemp78);
			double fTemp79 = (fTemp2 + fTemp4);
			double fTemp80 = (fTemp7 + fTemp9);
			double fTemp81 = (fTemp1 + fTemp3);
			double fTemp82 = (fRec1[0] * ((0.02017333553791887 * (((((((((((((fConst95 * fTemp31) + (fConst96 * fTemp33)) + (fConst97 * fTemp36)) + (fConst98 * fTemp37)) + (fConst99 * fTemp40)) + (fConst100 * fTemp41)) + (fConst97 * fTemp44)) + (fConst98 * fTemp45)) + (fConst99 * fTemp48)) + (fConst100 * fTemp49)) + (fConst95 * fTemp51)) + (fConst95 * fTemp52)) + (fConst96 * fTemp54))) + ((((fConst101 * fTemp79) + (0.022574955908289243 * ((fConst102 * fTemp18) + ((((fConst102 * fTemp80) + (fConst103 * fTemp12)) + (fConst104 * fTemp14)) + (fConst102 * fTemp16))))) + (0.021093750000000001 * ((((fConst105 * fTemp20) + (fConst106 * fTemp22)) + (fConst105 * fTemp25)) + (fConst106 * fTemp27)))) + (fConst22 * (((((((((((((((((((((0.04052746975506618 * fTemp81) + (0.036024417560058836 * fTemp6)) + (0.036024417560058836 * fTemp8)) + (1.5998055129555246e-17 * fTemp11)) + (3.1996110259110492e-17 * fTemp13)) + (0.036024417560058836 * fTemp15)) + (0.036024417560058836 * fTemp17)) + (9.9655853669580557e-18 * fTemp19)) + (1.9931170733916111e-17 * fTemp21)) + (9.9655853669580557e-18 * fTemp24)) + (1.9931170733916111e-17 * fTemp26)) + (5.19858676206982e-18 * fTemp32)) + (0.046824706809021746 * fTemp35)) + (0.046824706809021746 * fTemp38)) + (0.046824706809021774 * fTemp39)) + (0.046824706809021698 * fTemp42)) + (0.046824706809021746 * fTemp43)) + (0.046824706809021746 * fTemp46)) + (0.046824706809021774 * fTemp47)) + (0.046824706809021698 * fTemp50)) + (5.19858676206982e-18 * fTemp53))))));
			fRec9[0] = max((fRec9[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp82))))));
			fVbargraph8 = FAUSTFLOAT(fRec9[0]);
			output8[i] = FAUSTFLOAT(fTemp82);
			double fTemp83 = (fRec1[0] * (((((fConst110 * fTemp2) + (0.022574955908289243 * ((((fConst111 * fTemp7) + (fConst112 * fTemp13)) + (fConst113 * fTemp14)) + (fConst111 * fTemp16)))) + (0.021093750000000001 * ((((fConst114 * fTemp21) + (fConst115 * fTemp22)) + (fConst114 * fTemp26)) + (fConst115 * fTemp27)))) + (fConst109 * (((((((((((((((((((((0.081054939510132359 * fTemp4) + (0.050946219890026692 * fTemp9)) + (0.10189243978005338 * fTemp11)) + (0.1018924397800533 * fTemp12)) + (0.050946219890026692 * fTemp18)) + (0.051824122420700336 * fTemp19)) + (0.051824122420700287 * fTemp20)) + (0.051824122420700336 * fTemp24)) + (0.051824122420700287 * fTemp25)) + (0.0070590901547238332 * fTemp29)) + (0.0070590901547238332 * fTemp30)) + (0.091768172011410243 * fTemp35)) + (0.091768172011410271 * fTemp38)) + (0.063531811392514695 * fTemp40)) + (0.063531811392514792 * fTemp41)) + (0.091768172011410243 * fTemp43)) + (0.091768172011410271 * fTemp46)) + (0.063531811392514695 * fTemp48)) + (0.063531811392514792 * fTemp49)) + (0.0070590901547238332 * fTemp51)) + (0.0070590901547238332 * fTemp52)))) + (0.02017333553791887 * ((((((((((((fConst116 * fTemp32) + (fConst117 * fTemp33)) + (fConst118 * fTemp36)) + (fConst119 * fTemp37)) + (fConst120 * fTemp39)) + (fConst121 * fTemp42)) + (fConst118 * fTemp44)) + (fConst119 * fTemp45)) + (fConst120 * fTemp47)) + (fConst121 * fTemp50)) + (fConst116 * fTemp53)) + (fConst117 * fTemp54)))));
			fRec10[0] = max((fRec10[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp83))))));
			fVbargraph9 = FAUSTFLOAT(fRec10[0]);
			output9[i] = FAUSTFLOAT(fTemp83);
			double fTemp84 = (fRec1[0] * ((0.02017333553791887 * (((((((((((fConst125 * fTemp65) + (fConst126 * fTemp37)) + (fConst127 * fTemp38)) + (fConst128 * fTemp41)) + (fConst129 * fTemp42)) + (fConst132 * fTemp45)) + (fConst133 * fTemp46)) + (fConst134 * fTemp49)) + (fConst135 * fTemp50)) + (fConst138 * fTemp52)) + (fConst138 * fTemp54))) + ((0.021093750000000001 * (((fConst139 * fTemp67) + (fConst142 * fTemp25)) + (fConst142 * fTemp27))) + (fConst123 * (((((((((((((((fConst21 * ((0.12956030605175081 * fTemp19) + (0.12956030605175084 * fTemp21))) + (fConst143 * fTemp24)) + (fConst144 * fTemp26)) + (fConst145 * fTemp29)) + (fConst146 * fTemp32)) + (fConst147 * fTemp35)) + (fConst148 * fTemp36)) + (fConst149 * fTemp39)) + (fConst150 * fTemp40)) + (fConst151 * fTemp43)) + (fConst152 * fTemp44)) + (fConst153 * fTemp47)) + (fConst154 * fTemp48)) + (fConst155 * fTemp51)) + (fConst156 * fTemp53))))));
			fRec11[0] = max((fRec11[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp84))))));
			fVbargraph10 = FAUSTFLOAT(fRec11[0]);
			output10[i] = FAUSTFLOAT(fTemp84);
			double fTemp85 = (fRec1[0] * (((((fConst160 * fTemp4) + (0.022574955908289243 * ((((fConst162 * fTemp9) + (fConst163 * fTemp13)) + (fConst164 * fTemp14)) + (fConst166 * fTemp18)))) + (0.021093750000000001 * ((((fConst167 * fTemp21) + (fConst168 * fTemp22)) + (fConst171 * fTemp26)) + (fConst172 * fTemp27)))) + (fConst157 * (((((((((((((((((((((fConst173 * fTemp7) + (fConst174 * fTemp2)) + (fConst175 * fTemp11)) + (fConst176 * fTemp12)) + (fConst177 * fTemp16)) + (fConst178 * fTemp19)) + (fConst179 * fTemp20)) + (fConst180 * fTemp24)) + (fConst181 * fTemp25)) + (fConst182 * fTemp29)) + (fConst182 * fTemp30)) + (fConst184 * fTemp35)) + (fConst185 * fTemp36)) + (fConst186 * fTemp37)) + (fConst187 * fTemp38)) + (fConst189 * fTemp43)) + (fConst190 * fTemp44)) + (fConst191 * fTemp45)) + (fConst192 * fTemp46)) + (fConst194 * fTemp51)) + (fConst194 * fTemp52)))) + (0.02017333553791887 * ((((((((((((fConst195 * fTemp32) + (fConst196 * fTemp33)) + (fConst198 * fTemp39)) + (fConst199 * fTemp40)) + (fConst200 * fTemp41)) + (fConst201 * fTemp42)) + (fConst203 * fTemp47)) + (fConst204 * fTemp48)) + (fConst205 * fTemp49)) + (fConst206 * fTemp50)) + (fConst208 * fTemp53)) + (fConst209 * fTemp54)))));
			fRec12[0] = max((fRec12[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp85))))));
			fVbargraph11 = FAUSTFLOAT(fRec12[0]);
			output11[i] = FAUSTFLOAT(fTemp85);
			double fTemp86 = (fRec1[0] * (((0.033596842045264641 * fTemp57) + (0.026333598483989317 * fTemp62)) - (((0.010558468816293317 * fTemp58) + (0.021480823570105497 * fTemp59)) + (0.020481687709769415 * fTemp61))));
			fRec13[0] = max((fRec13[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp86))))));
			fVbargraph12 = FAUSTFLOAT(fRec13[0]);
			output12[i] = FAUSTFLOAT(fTemp86);
			double fTemp87 = (fRec1[0] * ((fConst157 * (((((((((((((((((((((fConst173 * fTemp6) + (fConst174 * fTemp1)) + (fConst176 * fTemp11)) + (fConst210 * fTemp14)) + (fConst177 * fTemp15)) + (fConst179 * fTemp19)) + (fConst211 * fTemp22)) + (fConst181 * fTemp24)) + (fConst212 * fTemp27)) + (fConst213 * fTemp29)) + (fConst214 * fTemp33)) + (fConst215 * fTemp35)) + (fConst216 * fTemp36)) + (fConst217 * fTemp41)) + (fConst218 * fTemp42)) + (fConst219 * fTemp43)) + (fConst220 * fTemp44)) + (fConst221 * fTemp49)) + (fConst222 * fTemp50)) + (fConst223 * fTemp51)) + (fConst224 * fTemp54))) + ((((fConst160 * fTemp3) + (0.022574955908289243 * ((((fConst162 * fTemp8) + (fConst163 * fTemp12)) + (fConst164 * fTemp13)) + (fConst166 * fTemp17)))) + (0.021093750000000001 * ((((fConst167 * fTemp20) + (fConst168 * fTemp21)) + (fConst171 * fTemp25)) + (fConst172 * fTemp26)))) + (0.02017333553791887 * ((((((((((((fConst195 * fTemp30) + (fConst196 * fTemp32)) + (fConst225 * fTemp37)) + (fConst226 * fTemp38)) + (fConst199 * fTemp39)) + (fConst227 * fTemp40)) + (fConst228 * fTemp45)) + (fConst229 * fTemp46)) + (fConst204 * fTemp47)) + (fConst230 * fTemp48)) + (fConst208 * fTemp52)) + (fConst209 * fTemp53))))));
			fRec14[0] = max((fRec14[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp87))))));
			fVbargraph13 = FAUSTFLOAT(fRec14[0]);
			output13[i] = FAUSTFLOAT(fTemp87);
			double fTemp88 = (fRec1[0] * ((0.02017333553791887 * (((((((((((((fConst231 * fTemp31) + (fConst232 * fTemp33)) + (fConst233 * fTemp36)) + (fConst234 * fTemp37)) + (fConst235 * fTemp40)) + (fConst236 * fTemp41)) + (fConst237 * fTemp44)) + (fConst237 * fTemp45)) + (fConst238 * fTemp48)) + (fConst239 * fTemp49)) + (fConst240 * fTemp51)) + (fConst240 * fTemp52)) + (fConst241 * fTemp54))) + (((0.022574955908289243 * (((fConst242 * fTemp80) + (fConst244 * fTemp16)) + (fConst244 * fTemp18))) + (0.021093750000000001 * ((((fConst245 * fTemp20) + (fConst246 * fTemp22)) + (fConst247 * fTemp25)) + (fConst248 * fTemp27)))) + (fConst123 * (((((((((((((((((fConst249 * (fTemp6 + fTemp8)) + (fConst250 * fTemp15)) + (fConst250 * fTemp17)) + (fConst251 * fTemp19)) + (fConst252 * fTemp21)) + (fConst253 * fTemp24)) + (fConst254 * fTemp26)) + (fConst255 * fTemp32)) + (fConst256 * fTemp35)) + (fConst256 * fTemp38)) + (fConst257 * fTemp39)) + (fConst258 * fTemp42)) + (fConst259 * fTemp43)) + (fConst259 * fTemp46)) + (fConst260 * fTemp47)) + (fConst261 * fTemp50)) + (fConst262 * fTemp53))))));
			fRec15[0] = max((fRec15[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp88))))));
			fVbargraph14 = FAUSTFLOAT(fRec15[0]);
			output14[i] = FAUSTFLOAT(fTemp88);
			double fTemp89 = (fRec1[0] * ((0.02017333553791887 * ((((((((((((fConst263 * fTemp29) + (fConst264 * fTemp33)) + (fConst265 * fTemp36)) + (fConst266 * fTemp38)) + (fConst267 * fTemp39)) + (fConst268 * fTemp41)) + (fConst265 * fTemp44)) + (fConst266 * fTemp46)) + (fConst267 * fTemp47)) + (fConst268 * fTemp49)) + (fConst263 * fTemp51)) + (fConst264 * fTemp54))) + ((((fConst110 * fTemp3) + (0.022574955908289243 * ((((fConst111 * fTemp8) + (fConst269 * fTemp11)) + (fConst270 * fTemp14)) + (fConst111 * fTemp17)))) + (0.021093750000000001 * ((((fConst271 * fTemp19) + (fConst272 * fTemp22)) + (fConst271 * fTemp24)) + (fConst272 * fTemp27)))) + (fConst109 * (((((((((((((((((((((0.081054939510132359 * fTemp1) + (0.050946219890026692 * fTemp6)) + (0.10189243978005338 * fTemp12)) + (0.10189243978005329 * fTemp13)) + (0.050946219890026692 * fTemp15)) + (0.051824122420700336 * fTemp20)) + (0.051824122420700267 * fTemp21)) + (0.051824122420700336 * fTemp25)) + (0.051824122420700267 * fTemp26)) + (0.0070590901547238384 * fTemp30)) + (0.0070590901547238306 * fTemp32)) + (0.063531811392514598 * fTemp35)) + (0.09176817201141016 * fTemp37)) + (0.091768172011410187 * fTemp40)) + (0.063531811392514445 * fTemp42)) + (0.063531811392514598 * fTemp43)) + (0.09176817201141016 * fTemp45)) + (0.091768172011410187 * fTemp48)) + (0.063531811392514445 * fTemp50)) + (0.0070590901547238384 * fTemp52)) + (0.0070590901547238306 * fTemp53))))));
			fRec16[0] = max((fRec16[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp89))))));
			fVbargraph15 = FAUSTFLOAT(fRec16[0]);
			output15[i] = FAUSTFLOAT(fTemp89);
			double fTemp90 = (fRec1[0] * ((0.02017333553791887 * ((((((((((fConst274 * fTemp29) + (fConst275 * fTemp36)) + (fConst276 * fTemp38)) + (fConst277 * fTemp40)) + (fConst278 * fTemp42)) + (fConst275 * fTemp44)) + (fConst276 * fTemp46)) + (fConst277 * fTemp48)) + (fConst278 * fTemp50)) + (fConst274 * fTemp51))) + (fConst273 * ((((((((((((((((((((((((((1.3574799921485692e-16 * fTemp11) + (1.3574799921485695e-16 * fTemp12)) + (2.7149599842971385e-16 * fTemp13)) + (2.714959984297139e-16 * fTemp14)) + (5.6373863931755746e-17 * fTemp19)) + (5.6373863931755758e-17 * fTemp20)) + (1.1274772786351149e-16 * fTemp21)) + (1.1274772786351152e-16 * fTemp22)) + (5.6373863931755746e-17 * fTemp24)) + (5.6373863931755758e-17 * fTemp25)) + (1.1274772786351149e-16 * fTemp26)) + (1.1274772786351152e-16 * fTemp27)) + (4.0101337658685596e-18 * fTemp30)) + (8.0202675317371177e-18 * fTemp32)) + (8.0202675317371192e-18 * fTemp33)) + (0.20432599334845877 * fTemp35)) + (0.20432599334845869 * fTemp37)) + (0.20432599334845877 * fTemp39)) + (0.20432599334845863 * fTemp41)) + (0.20432599334845877 * fTemp43)) + (0.20432599334845869 * fTemp45)) + (0.20432599334845877 * fTemp47)) + (0.20432599334845863 * fTemp49)) + (4.0101337658685596e-18 * fTemp52)) + (8.0202675317371177e-18 * fTemp53)) + (8.0202675317371192e-18 * fTemp54)))));
			fRec17[0] = max((fRec17[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp90))))));
			fVbargraph16 = FAUSTFLOAT(fRec17[0]);
			output16[i] = FAUSTFLOAT(fTemp90);
			double fTemp91 = (fRec1[0] * ((((0.022574955908289243 * ((fConst281 * fTemp7) + (fConst283 * fTemp16))) + (0.021093750000000001 * ((((fConst284 * fTemp21) + (fConst285 * fTemp22)) + (fConst288 * fTemp26)) + (fConst289 * fTemp27)))) + (fConst279 * ((((((((((((((((((fConst290 * fTemp9) + (fConst291 * fTemp18)) + (fConst292 * fTemp19)) + (fConst293 * fTemp20)) + (fConst294 * fTemp24)) + (fConst295 * fTemp25)) + (fConst296 * fTemp29)) + (fConst296 * fTemp30)) + (fConst297 * fTemp35)) + (fConst298 * fTemp38)) + (fConst299 * fTemp40)) + (fConst300 * fTemp41)) + (fConst302 * fTemp43)) + (fConst303 * fTemp46)) + (fConst304 * fTemp48)) + (fConst305 * fTemp49)) + (fConst307 * fTemp51)) + (fConst307 * fTemp52)))) + (0.02017333553791887 * ((((((((((((fConst308 * fTemp32) + (fConst309 * fTemp33)) + (fConst310 * fTemp36)) + (fConst311 * fTemp37)) + (fConst312 * fTemp39)) + (fConst313 * fTemp42)) + (fConst315 * fTemp44)) + (fConst316 * fTemp45)) + (fConst317 * fTemp47)) + (fConst318 * fTemp50)) + (fConst320 * fTemp53)) + (fConst321 * fTemp54)))));
			fRec18[0] = max((fRec18[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp91))))));
			fVbargraph17 = FAUSTFLOAT(fRec18[0]);
			output17[i] = FAUSTFLOAT(fTemp91);
			double fTemp92 = (fRec1[0] * ((0.02017333553791887 * (((((((((((fConst324 * fTemp65) + (fConst327 * fTemp37)) + (fConst328 * fTemp38)) + (fConst329 * fTemp41)) + (fConst330 * fTemp42)) + (fConst333 * fTemp45)) + (fConst334 * fTemp46)) + (fConst335 * fTemp49)) + (fConst336 * fTemp50)) + (fConst339 * fTemp52)) + (fConst339 * fTemp54))) + (((fConst343 * fTemp66) + (0.021093750000000001 * (((fConst344 * fTemp67) + (fConst347 * fTemp25)) + (fConst347 * fTemp27)))) + (fConst322 * (((((((((((((((((fConst340 * ((0.015962904407738641 * fTemp11) + (0.015962904407738645 * fTemp13))) + (fConst348 * fTemp19)) + (fConst349 * fTemp21)) + (fConst350 * fTemp24)) + (fConst351 * fTemp26)) + (fConst352 * fTemp29)) + (fConst353 * fTemp32)) + (fConst354 * fTemp35)) + (fConst355 * fTemp36)) + (fConst356 * fTemp39)) + (fConst357 * fTemp40)) + (fConst358 * fTemp43)) + (fConst359 * fTemp44)) + (fConst360 * fTemp47)) + (fConst361 * fTemp48)) + (fConst362 * fTemp51)) + (fConst363 * fTemp53))))));
			fRec19[0] = max((fRec19[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp92))))));
			fVbargraph18 = FAUSTFLOAT(fRec19[0]);
			output18[i] = FAUSTFLOAT(fTemp92);
			double fTemp93 = (fRec1[0] * ((((0.022574955908289243 * ((fConst366 * fTemp9) + (fConst368 * fTemp18))) + (0.021093750000000001 * ((((fConst371 * fTemp21) + (fConst372 * fTemp22)) + (fConst375 * fTemp26)) + (fConst376 * fTemp27)))) + (fConst364 * ((((((((((((((((((fConst377 * fTemp7) + (fConst378 * fTemp16)) + (fConst379 * fTemp19)) + (fConst380 * fTemp20)) + (fConst381 * fTemp24)) + (fConst382 * fTemp25)) + (fConst383 * fTemp29)) + (fConst383 * fTemp30)) + (fConst385 * fTemp35)) + (fConst386 * fTemp36)) + (fConst387 * fTemp37)) + (fConst388 * fTemp38)) + (fConst390 * fTemp43)) + (fConst391 * fTemp44)) + (fConst392 * fTemp45)) + (fConst393 * fTemp46)) + (fConst395 * fTemp51)) + (fConst395 * fTemp52)))) + (0.02017333553791887 * ((((((((((((fConst396 * fTemp32) + (fConst397 * fTemp33)) + (fConst399 * fTemp39)) + (fConst400 * fTemp40)) + (fConst401 * fTemp41)) + (fConst402 * fTemp42)) + (fConst404 * fTemp47)) + (fConst405 * fTemp48)) + (fConst406 * fTemp49)) + (fConst407 * fTemp50)) + (fConst409 * fTemp53)) + (fConst410 * fTemp54)))));
			fRec20[0] = max((fRec20[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fVbargraph19 = FAUSTFLOAT(fRec20[0]);
			output19[i] = FAUSTFLOAT(fTemp93);
			double fTemp94 = (fRec1[0] * ((((((0.014285714285714285 * fTemp74) + (0.038095238095238099 * fTemp72)) + (0.025396825396825397 * fTemp75)) + (0.0042514054232804339 * fTemp76)) + (0.0142547123015874 * fTemp73)) - ((0.027513227513227514 * fTemp71) + (0.024609375000000006 * fTemp28))));
			fRec21[0] = max((fRec21[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp94))))));
			fVbargraph20 = FAUSTFLOAT(fRec21[0]);
			output20[i] = FAUSTFLOAT(fTemp94);
			double fTemp95 = (fRec1[0] * ((fConst364 * ((((((((((((((((((fConst377 * fTemp6) + (fConst378 * fTemp15)) + (fConst380 * fTemp19)) + (fConst411 * fTemp22)) + (fConst382 * fTemp24)) + (fConst412 * fTemp27)) + (fConst413 * fTemp29)) + (fConst414 * fTemp33)) + (fConst386 * fTemp35)) + (fConst415 * fTemp36)) + (fConst416 * fTemp41)) + (fConst417 * fTemp42)) + (fConst391 * fTemp43)) + (fConst418 * fTemp44)) + (fConst419 * fTemp49)) + (fConst420 * fTemp50)) + (fConst421 * fTemp51)) + (fConst422 * fTemp54))) + (((0.022574955908289243 * ((fConst366 * fTemp8) + (fConst368 * fTemp17))) + (0.021093750000000001 * ((((fConst371 * fTemp20) + (fConst372 * fTemp21)) + (fConst375 * fTemp25)) + (fConst376 * fTemp26)))) + (0.02017333553791887 * ((((((((((((fConst396 * fTemp30) + (fConst397 * fTemp32)) + (fConst423 * fTemp37)) + (fConst424 * fTemp38)) + (fConst400 * fTemp39)) + (fConst425 * fTemp40)) + (fConst426 * fTemp45)) + (fConst427 * fTemp46)) + (fConst405 * fTemp47)) + (fConst428 * fTemp48)) + (fConst409 * fTemp52)) + (fConst410 * fTemp53))))));
			fRec22[0] = max((fRec22[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp95))))));
			fVbargraph21 = FAUSTFLOAT(fRec22[0]);
			output21[i] = FAUSTFLOAT(fTemp95);
			double fTemp96 = (fRec1[0] * ((0.02017333553791887 * (((((((((((((fConst429 * fTemp31) + (fConst430 * fTemp33)) + (fConst431 * fTemp36)) + (fConst431 * fTemp37)) + (fConst432 * fTemp40)) + (fConst433 * fTemp41)) + (fConst434 * fTemp44)) + (fConst434 * fTemp45)) + (fConst435 * fTemp48)) + (fConst436 * fTemp49)) + (fConst437 * fTemp51)) + (fConst437 * fTemp52)) + (fConst438 * fTemp54))) + ((((fConst439 * fTemp79) + (0.022574955908289243 * (((((fConst440 * fTemp80) + (fConst441 * fTemp12)) + (fConst442 * fTemp14)) + (fConst444 * fTemp16)) + (fConst444 * fTemp18)))) + (0.021093750000000001 * ((((fConst445 * fTemp20) + (fConst446 * fTemp22)) + (fConst447 * fTemp25)) + (fConst448 * fTemp27)))) + (fConst322 * (((((((((((((((((((((fConst449 * fTemp81) + (fConst450 * fTemp6)) + (fConst450 * fTemp8)) + (fConst451 * fTemp11)) + (fConst452 * fTemp13)) + (fConst453 * fTemp15)) + (fConst453 * fTemp17)) + (fConst454 * fTemp19)) + (fConst455 * fTemp21)) + (fConst456 * fTemp24)) + (fConst457 * fTemp26)) + (fConst458 * fTemp32)) + (fConst459 * fTemp35)) + (fConst459 * fTemp38)) + (fConst460 * fTemp39)) + (fConst461 * fTemp42)) + (fConst462 * fTemp43)) + (fConst462 * fTemp46)) + (fConst463 * fTemp47)) + (fConst464 * fTemp50)) + (fConst465 * fTemp53))))));
			fRec23[0] = max((fRec23[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp96))))));
			fVbargraph22 = FAUSTFLOAT(fRec23[0]);
			output22[i] = FAUSTFLOAT(fTemp96);
			double fTemp97 = (fRec1[0] * ((0.02017333553791887 * ((((((((((((fConst466 * fTemp29) + (fConst467 * fTemp33)) + (fConst468 * fTemp36)) + (fConst469 * fTemp38)) + (fConst470 * fTemp39)) + (fConst471 * fTemp41)) + (fConst472 * fTemp44)) + (fConst473 * fTemp46)) + (fConst474 * fTemp47)) + (fConst475 * fTemp49)) + (fConst476 * fTemp51)) + (fConst477 * fTemp54))) + (((0.022574955908289243 * ((fConst281 * fTemp8) + (fConst283 * fTemp17))) + (0.021093750000000001 * ((((fConst478 * fTemp19) + (fConst479 * fTemp22)) + (fConst480 * fTemp24)) + (fConst481 * fTemp27)))) + (fConst279 * ((((((((((((((((((fConst290 * fTemp6) + (fConst291 * fTemp15)) + (fConst292 * fTemp20)) + (fConst482 * fTemp21)) + (fConst294 * fTemp25)) + (fConst483 * fTemp26)) + (fConst484 * fTemp30)) + (fConst485 * fTemp32)) + (fConst486 * fTemp35)) + (fConst487 * fTemp37)) + (fConst488 * fTemp40)) + (fConst489 * fTemp42)) + (fConst490 * fTemp43)) + (fConst491 * fTemp45)) + (fConst492 * fTemp48)) + (fConst493 * fTemp50)) + (fConst494 * fTemp52)) + (fConst495 * fTemp53))))));
			fRec24[0] = max((fRec24[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp97))))));
			fVbargraph23 = FAUSTFLOAT(fRec24[0]);
			output23[i] = FAUSTFLOAT(fTemp97);
			double fTemp98 = (fRec1[0] * ((((0.022574955908289243 * ((((fConst496 * fTemp11) + (fConst497 * fTemp12)) + (fConst497 * fTemp13)) + (fConst497 * fTemp14))) + (0.021093750000000001 * ((((((((fConst498 * fTemp19) + (fConst499 * fTemp20)) + (fConst499 * fTemp21)) + (fConst499 * fTemp22)) + (fConst498 * fTemp24)) + (fConst499 * fTemp25)) + (fConst499 * fTemp26)) + (fConst499 * fTemp27)))) + (fConst273 * (((((((((((((((((((((((((0.16210987902026472 * fTemp74) + (0.072048835120117685 * fTemp6)) + (0.072048835120117685 * fTemp7)) + (0.072048835120117685 * fTemp8)) + (0.072048835120117685 * fTemp9)) + (0.072048835120117685 * fTemp15)) + (0.072048835120117685 * fTemp16)) + (0.072048835120117685 * fTemp17)) + (0.072048835120117685 * fTemp18)) + (0.059595081393300019 * fTemp35)) + (0.059595081393300449 * fTemp36)) + (0.059595081393300539 * fTemp37)) + (0.059595081393300019 * fTemp38)) + (0.059595081393300352 * fTemp39)) + (0.059595081393300338 * fTemp40)) + (0.059595081393300636 * fTemp41)) + (0.059595081393299554 * fTemp42)) + (0.059595081393300019 * fTemp43)) + (0.059595081393300449 * fTemp44)) + (0.059595081393300539 * fTemp45)) + (0.059595081393300019 * fTemp46)) + (0.059595081393300352 * fTemp47)) + (0.059595081393300338 * fTemp48)) + (0.059595081393300636 * fTemp49)) + (0.059595081393299554 * fTemp50)))) + (fConst500 * fTemp73)));
			fRec25[0] = max((fRec25[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp98))))));
			fVbargraph24 = FAUSTFLOAT(fRec25[0]);
			output24[i] = FAUSTFLOAT(fTemp98);
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
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
