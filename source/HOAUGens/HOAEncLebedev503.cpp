/* ------------------------------------------------------------
author: "Pierre Lecomte"
copyright: "(c) Pierre Lecomte 2014"
license: "GPL"
name: "HOAEncLebedev503"
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
	double fRec6[2];
	FAUSTFLOAT fVbargraph5;
	double fRec7[2];
	FAUSTFLOAT fVbargraph6;
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
	double fRec8[2];
	FAUSTFLOAT fVbargraph7;
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
	double fRec9[2];
	FAUSTFLOAT fVbargraph8;
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
	double fRec10[2];
	FAUSTFLOAT fVbargraph9;
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
	double fRec11[2];
	FAUSTFLOAT fVbargraph10;
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
	double fRec12[2];
	FAUSTFLOAT fVbargraph11;
	double fRec13[2];
	FAUSTFLOAT fVbargraph12;
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
	double fRec14[2];
	FAUSTFLOAT fVbargraph13;
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
	double fRec15[2];
	FAUSTFLOAT fVbargraph14;
	double fConst259;
	double fConst260;
	double fConst261;
	double fConst262;
	double fConst263;
	double fConst264;
	double fConst265;
	double fConst266;
	double fConst267;
	double fConst268;
	double fRec16[2];
	FAUSTFLOAT fVbargraph15;
	
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
		m->declare("name", "HOAEncLebedev503");
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
		return 16;
		
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
		fConst32 = sqrt((5.0 * (fConst31 / double(tgamma(4.0)))));
		fConst33 = (fConst32 * fConst3);
		fConst34 = (0.0 - (2.3936536824085959 * fConst33));
		fConst35 = (0.0 - (1.6925687506432687 * fConst3));
		fConst36 = (0.0 - (1.4142135623730951 * (fConst32 * fConst35)));
		fConst37 = (0.0 - (1.5957691216057308 * fConst33));
		fConst38 = (0.0 - (1.5957691216057313 * fConst33));
		fConst39 = (0.0 - (1.5957691216057308 * fConst3));
		fConst40 = (fConst32 * fConst39);
		fConst41 = (0.0 - fConst40);
		fConst42 = (0.0 - (1.0000000000000002 * fConst40));
		fConst43 = (0.054036626340088233 * fConst3);
		fConst44 = (0.031925808815477283 * fConst35);
		fConst45 = (0.033660754908870889 * fConst3);
		fConst46 = (0.021093750000000001 * fConst39);
		fConst47 = (0.021093750000000005 * fConst39);
		fConst48 = (0.026338897580074728 * fConst3);
		fConst49 = (0.0087796325266915921 * fConst3);
		fConst50 = (0.026338897580074742 * fConst3);
		fConst51 = (0.026338897580074745 * fConst3);
		fConst52 = (0.0087796325266915939 * fConst3);
		fConst53 = (0.0 - (0.97315860881060223 * fConst3));
		fConst54 = (0.0090217899191397874 * fConst53);
		fConst55 = (0.027065369757419327 * fConst53);
		fConst56 = (0.027065369757419331 * fConst53);
		fConst57 = (0.0090217899191397891 * fConst53);
		fConst58 = (0.0 - (1.3056292813137786 * fConst3));
		fConst59 = (0.020173335537918873 * fConst58);
		fConst60 = (0.0 - (1.3056292813137786 * fConst33));
		fConst61 = (0.0 - (1.3056292813137791 * fConst33));
		fConst62 = (0.0 - (0.43520976043792686 * fConst33));
		fConst63 = (0.0 - (1.3056292813137795 * fConst33));
		fConst64 = (0.0 - (1.3056292813137798 * fConst33));
		fConst65 = (0.0 - (0.43520976043792792 * fConst33));
		fConst66 = (fConst32 * fConst53);
		fConst67 = (0.0 - (0.44721359549995832 * fConst66));
		fConst68 = (0.0 - (1.3416407864998738 * fConst66));
		fConst69 = (0.0 - (1.3416407864998741 * fConst66));
		fConst70 = (0.0 - (0.44721359549995937 * fConst66));
		fConst71 = (fConst32 * fConst58);
		fConst72 = (0.0 - fConst71);
		fConst73 = (0.0 - (1.0000000000000002 * fConst71));
		fConst74 = (0.033660754908870882 * fConst3);
		fConst75 = (0.021093749999999998 * fConst39);
		fConst76 = (0.026338897580074724 * fConst3);
		fConst77 = (0.026338897580074721 * fConst3);
		fConst78 = (0.026338897580074738 * fConst3);
		fConst79 = (0.0087796325266915817 * fConst3);
		fConst80 = (0.0087796325266915748 * fConst3);
		fConst81 = (0.026338897580074731 * fConst3);
		fConst82 = (0.027065369757419324 * fConst53);
		fConst83 = (0.009021789919139777 * fConst53);
		fConst84 = (0.00902178991913977 * fConst53);
		fConst85 = (0.027065369757419321 * fConst53);
		fConst86 = (0.02017333553791887 * fConst58);
		fConst87 = (0.020173335537918866 * fConst58);
		fConst88 = (0.0 - (0.43520976043792636 * fConst33));
		fConst89 = (0.0 - (1.3056292813137793 * fConst33));
		fConst90 = (0.0 - (0.4352097604379267 * fConst33));
		fConst91 = (0.0 - (0.44721359549995782 * fConst66));
		fConst92 = (0.0 - (1.3416407864998736 * fConst66));
		fConst93 = (0.0 - (0.44721359549995815 * fConst66));
		fConst94 = (0.0 - (2.5769594484255127e-16 * fConst22));
		fConst95 = (0.0 - (1.2884797242127563e-16 * fConst22));
		fConst96 = (0.0 - (2.3211187223356089 * fConst22));
		fConst97 = (0.0 - (2.3211187223356093 * fConst22));
		fConst98 = (0.0 - (2.3211187223356085 * fConst22));
		fConst99 = (0.0 - (2.3211187223356098 * fConst22));
		fConst100 = (0.012698412698412698 * fConst28);
		fConst101 = (0.0 - (1.595769121605731 * fConst22));
		fConst102 = (0.0 - (7.0866384831701748e-16 * fConst22));
		fConst103 = (0.0 - (1.417327696634035e-15 * fConst22));
		fConst104 = (0.0 - (4.7244256554467816e-16 * fConst22));
		fConst105 = (0.0 - (9.4488513108935631e-16 * fConst22));
		fConst106 = (double(tgamma(3.5)) * sqrt((7.0 * (fConst1 / double(tgamma(7.0))))));
		fConst107 = (0.012698412698412698 * (0.0 - (6.3830764864229232 * fConst106)));
		fConst108 = (0.0 - (2.256758334191026 * fConst106));
		fConst109 = (0.0 - (4.5135166683820547 * fConst106));
		fConst110 = (0.0 - (4.5135166683820476 * fConst106));
		fConst111 = (0.0 - (2.456847285129498 * fConst106));
		fConst112 = (0.0 - (2.456847285129494 * fConst106));
		fConst113 = (0.0 - (0.34992181344801421 * fConst106));
		fConst114 = (0.0 - (0.34992181344801365 * fConst106));
		fConst115 = (0.0 - (3.1492963210321352 * fConst106));
		fConst116 = (0.0 - (3.1492963210321379 * fConst106));
		fConst117 = (0.0 - (4.5489835748241987 * fConst106));
		fConst118 = (0.0 - (4.5489835748242058 * fConst106));
		fConst119 = sqrt((7.0 * (fConst31 / double(tgamma(6.0)))));
		fConst120 = (fConst21 * fConst119);
		fConst121 = (0.0 - (2.624413600860108 * fConst120));
		fConst122 = (0.0 - (2.6244136008601116 * fConst120));
		fConst123 = (0.0 - (2.6244136008601155 * fConst120));
		fConst124 = (0.0 - (2.6244136008601107 * fConst120));
		fConst125 = (0.0 - (2.6244136008601191 * fConst120));
		fConst126 = (0.0 - (3.0929010896773175 * fConst21));
		fConst127 = (fConst119 * fConst126);
		fConst128 = (0.0 - (0.84852813742385691 * fConst127));
		fConst129 = (0.0 - (0.84852813742385813 * fConst127));
		fConst130 = (0.0 - (0.84852813742385658 * fConst127));
		fConst131 = (0.0 - (0.84852813742385946 * fConst127));
		fConst132 = (0.0 - (1.8557406538063874 * fConst21));
		fConst133 = (fConst119 * fConst132);
		fConst134 = (0.0 - (1.4142135623730951 * fConst133));
		fConst135 = (0.0 - (6.1421182128237417 * fConst120));
		fConst136 = (0.0 - (4.3431334391370653 * fConst21));
		fConst137 = (fConst119 * fConst136);
		fConst138 = (0.0 - (1.4142135623730951 * fConst137));
		fConst139 = (0.029831067331307477 * fConst136);
		fConst140 = (0.029831067331307481 * fConst136);
		fConst141 = (0.052943176160428845 * fConst21);
		fConst142 = (0.052943176160428852 * fConst21);
		fConst143 = (0.052943176160428991 * fConst21);
		fConst144 = (0.052943176160428929 * fConst21);
		fConst145 = (0.052943176160428963 * fConst21);
		fConst146 = (0.052943176160428942 * fConst21);
		fConst147 = (0.017117642829616822 * fConst126);
		fConst148 = (0.017117642829616801 * fConst126);
		fConst149 = (0.017117642829616815 * fConst126);
		fConst150 = (0.017117642829616808 * fConst126);
		fConst151 = (0.028529404716028006 * fConst132);
		fConst152 = (0.028529404716028009 * fConst132);
		fConst153 = sqrt((7.0 * (fConst2 / fConst20)));
		fConst154 = (0.0 - (3.3851375012865379 * fConst3));
		fConst155 = (fConst153 * fConst154);
		fConst156 = (0.012698412698412698 * (0.0 - (0.70710678118654757 * fConst155)));
		fConst157 = (fConst3 * fConst153);
		fConst158 = (0.0 - (2.5388531259649025 * fConst157));
		fConst159 = (0.0 - (0.5 * fConst155));
		fConst160 = (0.0 - (0.50000000000000011 * fConst155));
		fConst161 = (0.0 - ((3.5355339059327373 * fConst35) + (2.3936536824085963 * fConst3)));
		fConst162 = (0.0 - (0.70710678118654757 * (fConst153 * fConst161)));
		fConst163 = (0.0 - (0.92131773192356159 * fConst157));
		fConst164 = (0.0 - (0.92131773192356192 * fConst157));
		fConst165 = (0.0 - ((2.8867513459481291 * fConst39) + (2.7639531957706835 * fConst3)));
		fConst166 = (fConst153 * fConst165);
		fConst167 = (0.0 - (0.5 * fConst166));
		fConst168 = (0.0 - (0.50000000000000011 * fConst166));
		fConst169 = (0.057314497376279992 * fConst3);
		fConst170 = (0.0089791337293529845 * fConst154);
		fConst171 = (0.011287477954144622 * fConst154);
		fConst172 = (0.011287477954144623 * fConst154);
		fConst173 = (0.015962904407738641 * fConst161);
		fConst174 = (0.019434045907762629 * fConst3);
		fConst175 = (0.019434045907762633 * fConst3);
		fConst176 = (0.010546875000000001 * fConst165);
		fConst177 = (0.010546875000000002 * fConst165);
		fConst178 = (0.045001699736364593 * fConst3);
		fConst179 = (0.0 - (1.7605101637794649 * fConst3));
		fConst180 = (0.0045108949595698937 * fConst179);
		fConst181 = (0.013532684878709664 * fConst179);
		fConst182 = (0.013532684878709665 * fConst179);
		fConst183 = (0.0045108949595698946 * fConst179);
		fConst184 = (0.0 - ((1.5075567228888176 * fConst53) + (3.2276019669290172 * fConst3)));
		fConst185 = (0.0045108949595698937 * fConst184);
		fConst186 = (0.013532684878709664 * fConst184);
		fConst187 = (0.013532684878709665 * fConst184);
		fConst188 = (0.0045108949595698946 * fConst184);
		fConst189 = (0.0 - ((4.5226701686664557 * fConst58) + (1.4434274804730605 * fConst3)));
		fConst190 = (0.010086667768959437 * fConst189);
		fConst191 = (0.0 - (2.230751560731095 * fConst157));
		fConst192 = (0.0 - (2.2307515607310959 * fConst157));
		fConst193 = (fConst153 * fConst179);
		fConst194 = (0.0 - (0.22360679774997916 * fConst193));
		fConst195 = (0.0 - (0.67082039324993692 * fConst193));
		fConst196 = (0.0 - (0.67082039324993703 * fConst193));
		fConst197 = (0.0 - (0.22360679774997969 * fConst193));
		fConst198 = (fConst153 * fConst184);
		fConst199 = (0.0 - (0.22360679774997916 * fConst198));
		fConst200 = (0.0 - (0.67082039324993692 * fConst198));
		fConst201 = (0.0 - (0.67082039324993703 * fConst198));
		fConst202 = (0.0 - (0.22360679774997969 * fConst198));
		fConst203 = (fConst153 * fConst189);
		fConst204 = (0.0 - (0.5 * fConst203));
		fConst205 = (0.0 - (0.50000000000000011 * fConst203));
		fConst206 = (0.01128747795414462 * fConst154);
		fConst207 = (0.019434045907762626 * fConst3);
		fConst208 = (0.010546874999999999 * fConst165);
		fConst209 = (0.045001699736364587 * fConst3);
		fConst210 = (0.045001699736364573 * fConst3);
		fConst211 = (0.013532684878709662 * fConst179);
		fConst212 = (0.0045108949595698885 * fConst179);
		fConst213 = (0.004510894959569885 * fConst179);
		fConst214 = (0.01353268487870966 * fConst179);
		fConst215 = (0.013532684878709662 * fConst184);
		fConst216 = (0.0045108949595698885 * fConst184);
		fConst217 = (0.004510894959569885 * fConst184);
		fConst218 = (0.01353268487870966 * fConst184);
		fConst219 = (0.010086667768959435 * fConst189);
		fConst220 = (0.010086667768959433 * fConst189);
		fConst221 = (0.0 - (0.22360679774997891 * fConst193));
		fConst222 = (0.0 - (0.67082039324993681 * fConst193));
		fConst223 = (0.0 - (0.22360679774997907 * fConst193));
		fConst224 = (0.0 - (0.22360679774997891 * fConst198));
		fConst225 = (0.0 - (0.67082039324993681 * fConst198));
		fConst226 = (0.0 - (0.22360679774997907 * fConst198));
		fConst227 = (0.0 - (5.8273688116286149e-16 * fConst120));
		fConst228 = (0.0 - (1.165473762325723e-15 * fConst120));
		fConst229 = (0.0 - (3.4992181344801496 * fConst120));
		fConst230 = (0.0 - (3.4992181344801501 * fConst120));
		fConst231 = (0.0 - (3.4992181344801487 * fConst120));
		fConst232 = (0.0 - (3.4992181344801505 * fConst120));
		fConst233 = (0.0 - (1.1313708498984762 * fConst127));
		fConst234 = (0.0 - (1.1313708498984758 * fConst127));
		fConst235 = (0.0 - (1.1313708498984765 * fConst127));
		fConst236 = (0.0 - (3.1401849173675503e-16 * fConst133));
		fConst237 = (0.0 - (6.2803698347351007e-16 * fConst133));
		fConst238 = (0.0 - (5.6418958354775643 * fConst120));
		fConst239 = (0.0 - (3.9894228040143274 * fConst21));
		fConst240 = (0.0 - (1.4142135623730951 * (fConst119 * fConst239)));
		fConst241 = (0.0 - (1.3638242119692871e-15 * fConst120));
		fConst242 = (0.0 - (2.7276484239385742e-15 * fConst120));
		fConst243 = (0.0 - (3.1401849173675503e-16 * fConst137));
		fConst244 = (0.0 - (6.2803698347351007e-16 * fConst137));
		fConst245 = (0.12736554972506672 * fConst21);
		fConst246 = (0.031925808815477283 * fConst239);
		fConst247 = (2.8768166971227152e-17 * fConst21);
		fConst248 = (5.7536333942454304e-17 * fConst21);
		fConst249 = (6.6238275600721768e-18 * fConst136);
		fConst250 = (1.3247655120144354e-17 * fConst136);
		fConst251 = (2.3511493268037518e-17 * fConst21);
		fConst252 = (0.070590901547238516 * fConst21);
		fConst253 = (0.070590901547238558 * fConst21);
		fConst254 = (0.07059090154723846 * fConst21);
		fConst255 = (0.022823523772822384 * fConst126);
		fConst256 = (0.022823523772822398 * fConst126);
		fConst257 = (0.022823523772822367 * fConst126);
		fConst258 = (1.2669600797833527e-17 * fConst132);
		fConst259 = (0.0 - (0.34992181344801404 * fConst106));
		fConst260 = (0.0 - (0.34992181344801415 * fConst106));
		fConst261 = (0.0 - (4.548983574824196 * fConst106));
		fConst262 = (0.0 - (3.1492963210321285 * fConst106));
		fConst263 = (0.0 - (3.1492963210321339 * fConst106));
		fConst264 = (0.0 - (4.5489835748241934 * fConst106));
		fConst265 = (0.0 - (4.5135166683820485 * fConst106));
		fConst266 = (0.0 - (4.5135166683820538 * fConst106));
		fConst267 = (0.0 - (2.4568472851294949 * fConst106));
		fConst268 = (0.0 - (2.4568472851294976 * fConst106));
		
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
		ui_interface->addVerticalBargraph("0x3120680", &fVbargraph0, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fVbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x313a2e0", &fVbargraph1, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fVbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3144d00", &fVbargraph2, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fVbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3156a90", &fVbargraph3, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fVbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x316b870", &fVbargraph4, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fVbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x318e350", &fVbargraph5, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fVbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3198580", &fVbargraph6, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fVbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31af490", &fVbargraph7, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fVbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31c73f0", &fVbargraph8, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fVbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31e0790", &fVbargraph9, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fVbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x31fd850", &fVbargraph10, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fVbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x32269d0", &fVbargraph11, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fVbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x322d780", &fVbargraph12, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fVbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3245c80", &fVbargraph13, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fVbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3263d60", &fVbargraph14, -70.0, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fVbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x3279ec0", &fVbargraph15, -70.0, 6.0);
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
			double fTemp66 = (fTemp20 + fTemp22);
			double fTemp67 = (fRec1[0] * ((0.02017333553791887 * (((((((((((fConst23 * fTemp65) + (fConst24 * fTemp37)) + (fConst25 * fTemp38)) + (fConst26 * fTemp41)) + (fConst27 * fTemp42)) + (fConst24 * fTemp45)) + (fConst25 * fTemp46)) + (fConst26 * fTemp49)) + (fConst27 * fTemp50)) + (fConst23 * fTemp52)) + (fConst23 * fTemp54))) + (((fConst29 * (fTemp12 + fTemp14)) + (fConst30 * ((fTemp66 + fTemp25) + fTemp27))) + (fConst22 * ((((((((((((((((((0.072048835120117657 * fTemp11) + (0.072048835120117671 * fTemp13)) + (0.044881006545161167 * fTemp19)) + (0.044881006545161174 * fTemp21)) + (0.044881006545161167 * fTemp24)) + (0.044881006545161174 * fTemp26)) + (0.011706176702255417 * fTemp29)) + (0.011706176702255421 * fTemp32)) + (0.035118530106766375 * fTemp35)) + (0.035118530106766334 * fTemp36)) + (0.035118530106766362 * fTemp39)) + (0.035118530106766348 * fTemp40)) + (0.035118530106766375 * fTemp43)) + (0.035118530106766334 * fTemp44)) + (0.035118530106766362 * fTemp47)) + (0.035118530106766348 * fTemp48)) + (0.011706176702255417 * fTemp51)) + (0.011706176702255421 * fTemp53))))));
			fRec5[0] = max((fRec5[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp67))))));
			fVbargraph4 = FAUSTFLOAT(fRec5[0]);
			output4[i] = FAUSTFLOAT(fTemp67);
			double fTemp68 = (fConst45 * fTemp19);
			double fTemp69 = (fRec1[0] * ((((0.022574955908289243 * ((fConst34 * fTemp9) + (fConst36 * fTemp18))) + (0.021093750000000001 * ((((fConst37 * fTemp21) + (fConst38 * fTemp22)) + (fConst41 * fTemp26)) + (fConst42 * fTemp27)))) + (fConst32 * ((((((((((((((((((fConst43 * fTemp7) + (fConst44 * fTemp16)) + fTemp68) + (fConst45 * fTemp20)) + (fConst46 * fTemp24)) + (fConst47 * fTemp25)) + (fConst48 * fTemp29)) + (fConst48 * fTemp30)) + (fConst49 * fTemp35)) + (fConst50 * fTemp36)) + (fConst51 * fTemp37)) + (fConst52 * fTemp38)) + (fConst54 * fTemp43)) + (fConst55 * fTemp44)) + (fConst56 * fTemp45)) + (fConst57 * fTemp46)) + (fConst59 * fTemp51)) + (fConst59 * fTemp52)))) + (0.02017333553791887 * ((((((((((((fConst60 * fTemp32) + (fConst61 * fTemp33)) + (fConst62 * fTemp39)) + (fConst63 * fTemp40)) + (fConst64 * fTemp41)) + (fConst65 * fTemp42)) + (fConst67 * fTemp47)) + (fConst68 * fTemp48)) + (fConst69 * fTemp49)) + (fConst70 * fTemp50)) + (fConst72 * fTemp53)) + (fConst73 * fTemp54)))));
			fRec6[0] = max((fRec6[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp69))))));
			fVbargraph5 = FAUSTFLOAT(fRec6[0]);
			output5[i] = FAUSTFLOAT(fTemp69);
			double fTemp70 = (fRec1[0] * ((((0.01261978399999881 * ((((fTemp10 + fTemp15) + fTemp16) + fTemp17) + fTemp18)) + ((0.028394513999997331 * (fTemp0 + fTemp5)) + (5.236595479505967e-18 * fTemp28))) + (0.032806508796871948 * ((((fTemp34 + fTemp51) + fTemp52) + fTemp53) + fTemp54))) - (((0.014197256999998666 * (((fTemp1 + fTemp2) + fTemp3) + fTemp4)) + (0.025239567999997631 * (((fTemp11 + fTemp12) + fTemp13) + fTemp14))) + (0.01640325439843596 * ((((((((fTemp60 + fTemp43) + fTemp44) + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) + fTemp50)))));
			fRec7[0] = max((fRec7[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp70))))));
			fVbargraph6 = FAUSTFLOAT(fRec7[0]);
			output6[i] = FAUSTFLOAT(fTemp70);
			double fTemp71 = (fRec1[0] * ((fConst32 * ((((((((((((((((fTemp68 + ((fConst43 * fTemp6) + (fConst44 * fTemp15))) + (fConst74 * fTemp22)) + (fConst47 * fTemp24)) + (fConst75 * fTemp27)) + (fConst76 * fTemp29)) + (fConst77 * fTemp33)) + (fConst78 * fTemp35)) + (fConst79 * fTemp36)) + (fConst80 * fTemp41)) + (fConst81 * fTemp42)) + (fConst82 * fTemp43)) + (fConst83 * fTemp44)) + (fConst84 * fTemp49)) + (fConst85 * fTemp50)) + (fConst86 * fTemp51)) + (fConst87 * fTemp54))) + (((0.022574955908289243 * ((fConst34 * fTemp8) + (fConst36 * fTemp17))) + (0.021093750000000001 * ((((fConst37 * fTemp20) + (fConst38 * fTemp21)) + (fConst41 * fTemp25)) + (fConst42 * fTemp26)))) + (0.02017333553791887 * ((((((((((((fConst60 * fTemp30) + (fConst61 * fTemp32)) + (fConst88 * fTemp37)) + (fConst89 * fTemp38)) + (fConst63 * fTemp39)) + (fConst90 * fTemp40)) + (fConst91 * fTemp45)) + (fConst92 * fTemp46)) + (fConst68 * fTemp47)) + (fConst93 * fTemp48)) + (fConst72 * fTemp52)) + (fConst73 * fTemp53))))));
			fRec8[0] = max((fRec8[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fVbargraph7 = FAUSTFLOAT(fRec8[0]);
			output7[i] = FAUSTFLOAT(fTemp71);
			double fTemp72 = (fTemp7 + fTemp9);
			double fTemp73 = (fRec1[0] * ((0.02017333553791887 * (((((((((((((fConst94 * fTemp33) + (fConst95 * fTemp31)) + (fConst96 * fTemp36)) + (fConst97 * fTemp37)) + (fConst98 * fTemp40)) + (fConst99 * fTemp41)) + (fConst96 * fTemp44)) + (fConst97 * fTemp45)) + (fConst98 * fTemp48)) + (fConst99 * fTemp49)) + (fConst95 * fTemp51)) + (fConst95 * fTemp52)) + (fConst94 * fTemp54))) + ((((fConst100 * (fTemp2 + fTemp4)) + (0.022574955908289243 * (((((fConst101 * fTemp72) + (fConst102 * fTemp12)) + (fConst103 * fTemp14)) + (fConst101 * fTemp16)) + (fConst101 * fTemp18)))) + (0.021093750000000001 * ((((fConst104 * fTemp20) + (fConst105 * fTemp22)) + (fConst104 * fTemp25)) + (fConst105 * fTemp27)))) + (fConst22 * (((((((((((((((((((((0.04052746975506618 * (fTemp1 + fTemp3)) + (0.036024417560058836 * fTemp6)) + (0.036024417560058836 * fTemp8)) + (1.5998055129555246e-17 * fTemp11)) + (3.1996110259110492e-17 * fTemp13)) + (0.036024417560058836 * fTemp15)) + (0.036024417560058836 * fTemp17)) + (9.9655853669580557e-18 * fTemp19)) + (1.9931170733916111e-17 * fTemp21)) + (9.9655853669580557e-18 * fTemp24)) + (1.9931170733916111e-17 * fTemp26)) + (5.19858676206982e-18 * fTemp32)) + (0.046824706809021746 * fTemp35)) + (0.046824706809021746 * fTemp38)) + (0.046824706809021774 * fTemp39)) + (0.046824706809021698 * fTemp42)) + (0.046824706809021746 * fTemp43)) + (0.046824706809021746 * fTemp46)) + (0.046824706809021774 * fTemp47)) + (0.046824706809021698 * fTemp50)) + (5.19858676206982e-18 * fTemp53))))));
			fRec9[0] = max((fRec9[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp73))))));
			fVbargraph8 = FAUSTFLOAT(fRec9[0]);
			output8[i] = FAUSTFLOAT(fTemp73);
			double fTemp74 = (fRec1[0] * (((((fConst107 * fTemp2) + (0.022574955908289243 * ((((fConst108 * fTemp7) + (fConst109 * fTemp13)) + (fConst110 * fTemp14)) + (fConst108 * fTemp16)))) + (0.021093750000000001 * ((((fConst111 * fTemp21) + (fConst112 * fTemp22)) + (fConst111 * fTemp26)) + (fConst112 * fTemp27)))) + (fConst106 * (((((((((((((((((((((0.081054939510132359 * fTemp4) + (0.050946219890026692 * fTemp9)) + (0.10189243978005338 * fTemp11)) + (0.1018924397800533 * fTemp12)) + (0.050946219890026692 * fTemp18)) + (0.051824122420700336 * fTemp19)) + (0.051824122420700287 * fTemp20)) + (0.051824122420700336 * fTemp24)) + (0.051824122420700287 * fTemp25)) + (0.0070590901547238332 * fTemp29)) + (0.0070590901547238332 * fTemp30)) + (0.091768172011410243 * fTemp35)) + (0.091768172011410271 * fTemp38)) + (0.063531811392514695 * fTemp40)) + (0.063531811392514792 * fTemp41)) + (0.091768172011410243 * fTemp43)) + (0.091768172011410271 * fTemp46)) + (0.063531811392514695 * fTemp48)) + (0.063531811392514792 * fTemp49)) + (0.0070590901547238332 * fTemp51)) + (0.0070590901547238332 * fTemp52)))) + (0.02017333553791887 * ((((((((((((fConst113 * fTemp32) + (fConst114 * fTemp33)) + (fConst115 * fTemp36)) + (fConst116 * fTemp37)) + (fConst117 * fTemp39)) + (fConst118 * fTemp42)) + (fConst115 * fTemp44)) + (fConst116 * fTemp45)) + (fConst117 * fTemp47)) + (fConst118 * fTemp50)) + (fConst113 * fTemp53)) + (fConst114 * fTemp54)))));
			fRec10[0] = max((fRec10[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp74))))));
			fVbargraph9 = FAUSTFLOAT(fRec10[0]);
			output9[i] = FAUSTFLOAT(fTemp74);
			double fTemp75 = (fRec1[0] * ((0.02017333553791887 * (((((((((((fConst121 * fTemp65) + (fConst122 * fTemp37)) + (fConst123 * fTemp38)) + (fConst124 * fTemp41)) + (fConst125 * fTemp42)) + (fConst128 * fTemp45)) + (fConst129 * fTemp46)) + (fConst130 * fTemp49)) + (fConst131 * fTemp50)) + (fConst134 * fTemp52)) + (fConst134 * fTemp54))) + ((0.021093750000000001 * (((fConst135 * fTemp66) + (fConst138 * fTemp25)) + (fConst138 * fTemp27))) + (fConst119 * (((((((((((((((fConst21 * ((0.12956030605175081 * fTemp19) + (0.12956030605175084 * fTemp21))) + (fConst139 * fTemp24)) + (fConst140 * fTemp26)) + (fConst141 * fTemp29)) + (fConst142 * fTemp32)) + (fConst143 * fTemp35)) + (fConst144 * fTemp36)) + (fConst145 * fTemp39)) + (fConst146 * fTemp40)) + (fConst147 * fTemp43)) + (fConst148 * fTemp44)) + (fConst149 * fTemp47)) + (fConst150 * fTemp48)) + (fConst151 * fTemp51)) + (fConst152 * fTemp53))))));
			fRec11[0] = max((fRec11[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp75))))));
			fVbargraph10 = FAUSTFLOAT(fRec11[0]);
			output10[i] = FAUSTFLOAT(fTemp75);
			double fTemp76 = (fRec1[0] * (((((fConst156 * fTemp4) + (0.022574955908289243 * ((((fConst158 * fTemp9) + (fConst159 * fTemp13)) + (fConst160 * fTemp14)) + (fConst162 * fTemp18)))) + (0.021093750000000001 * ((((fConst163 * fTemp21) + (fConst164 * fTemp22)) + (fConst167 * fTemp26)) + (fConst168 * fTemp27)))) + (fConst153 * (((((((((((((((((((((fConst169 * fTemp7) + (fConst170 * fTemp2)) + (fConst171 * fTemp11)) + (fConst172 * fTemp12)) + (fConst173 * fTemp16)) + (fConst174 * fTemp19)) + (fConst175 * fTemp20)) + (fConst176 * fTemp24)) + (fConst177 * fTemp25)) + (fConst178 * fTemp29)) + (fConst178 * fTemp30)) + (fConst180 * fTemp35)) + (fConst181 * fTemp36)) + (fConst182 * fTemp37)) + (fConst183 * fTemp38)) + (fConst185 * fTemp43)) + (fConst186 * fTemp44)) + (fConst187 * fTemp45)) + (fConst188 * fTemp46)) + (fConst190 * fTemp51)) + (fConst190 * fTemp52)))) + (0.02017333553791887 * ((((((((((((fConst191 * fTemp32) + (fConst192 * fTemp33)) + (fConst194 * fTemp39)) + (fConst195 * fTemp40)) + (fConst196 * fTemp41)) + (fConst197 * fTemp42)) + (fConst199 * fTemp47)) + (fConst200 * fTemp48)) + (fConst201 * fTemp49)) + (fConst202 * fTemp50)) + (fConst204 * fTemp53)) + (fConst205 * fTemp54)))));
			fRec12[0] = max((fRec12[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp76))))));
			fVbargraph11 = FAUSTFLOAT(fRec12[0]);
			output11[i] = FAUSTFLOAT(fTemp76);
			double fTemp77 = (fRec1[0] * (((0.033596842045264641 * fTemp57) + (0.026333598483989317 * fTemp62)) - (((0.010558468816293317 * fTemp58) + (0.021480823570105497 * fTemp59)) + (0.020481687709769415 * fTemp61))));
			fRec13[0] = max((fRec13[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp77))))));
			fVbargraph12 = FAUSTFLOAT(fRec13[0]);
			output12[i] = FAUSTFLOAT(fTemp77);
			double fTemp78 = (fRec1[0] * ((fConst153 * (((((((((((((((((((((fConst169 * fTemp6) + (fConst170 * fTemp1)) + (fConst172 * fTemp11)) + (fConst206 * fTemp14)) + (fConst173 * fTemp15)) + (fConst175 * fTemp19)) + (fConst207 * fTemp22)) + (fConst177 * fTemp24)) + (fConst208 * fTemp27)) + (fConst209 * fTemp29)) + (fConst210 * fTemp33)) + (fConst211 * fTemp35)) + (fConst212 * fTemp36)) + (fConst213 * fTemp41)) + (fConst214 * fTemp42)) + (fConst215 * fTemp43)) + (fConst216 * fTemp44)) + (fConst217 * fTemp49)) + (fConst218 * fTemp50)) + (fConst219 * fTemp51)) + (fConst220 * fTemp54))) + ((((fConst156 * fTemp3) + (0.022574955908289243 * ((((fConst158 * fTemp8) + (fConst159 * fTemp12)) + (fConst160 * fTemp13)) + (fConst162 * fTemp17)))) + (0.021093750000000001 * ((((fConst163 * fTemp20) + (fConst164 * fTemp21)) + (fConst167 * fTemp25)) + (fConst168 * fTemp26)))) + (0.02017333553791887 * ((((((((((((fConst191 * fTemp30) + (fConst192 * fTemp32)) + (fConst221 * fTemp37)) + (fConst222 * fTemp38)) + (fConst195 * fTemp39)) + (fConst223 * fTemp40)) + (fConst224 * fTemp45)) + (fConst225 * fTemp46)) + (fConst200 * fTemp47)) + (fConst226 * fTemp48)) + (fConst204 * fTemp52)) + (fConst205 * fTemp53))))));
			fRec14[0] = max((fRec14[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fVbargraph13 = FAUSTFLOAT(fRec14[0]);
			output13[i] = FAUSTFLOAT(fTemp78);
			double fTemp79 = (fRec1[0] * ((0.02017333553791887 * (((((((((((((fConst227 * fTemp31) + (fConst228 * fTemp33)) + (fConst229 * fTemp36)) + (fConst230 * fTemp37)) + (fConst231 * fTemp40)) + (fConst232 * fTemp41)) + (fConst233 * fTemp44)) + (fConst233 * fTemp45)) + (fConst234 * fTemp48)) + (fConst235 * fTemp49)) + (fConst236 * fTemp51)) + (fConst236 * fTemp52)) + (fConst237 * fTemp54))) + (((0.022574955908289243 * (((fConst238 * fTemp72) + (fConst240 * fTemp16)) + (fConst240 * fTemp18))) + (0.021093750000000001 * ((((fConst241 * fTemp20) + (fConst242 * fTemp22)) + (fConst243 * fTemp25)) + (fConst244 * fTemp27)))) + (fConst119 * (((((((((((((((((fConst245 * (fTemp6 + fTemp8)) + (fConst246 * fTemp15)) + (fConst246 * fTemp17)) + (fConst247 * fTemp19)) + (fConst248 * fTemp21)) + (fConst249 * fTemp24)) + (fConst250 * fTemp26)) + (fConst251 * fTemp32)) + (fConst252 * fTemp35)) + (fConst252 * fTemp38)) + (fConst253 * fTemp39)) + (fConst254 * fTemp42)) + (fConst255 * fTemp43)) + (fConst255 * fTemp46)) + (fConst256 * fTemp47)) + (fConst257 * fTemp50)) + (fConst258 * fTemp53))))));
			fRec15[0] = max((fRec15[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp79))))));
			fVbargraph14 = FAUSTFLOAT(fRec15[0]);
			output14[i] = FAUSTFLOAT(fTemp79);
			double fTemp80 = (fRec1[0] * ((0.02017333553791887 * ((((((((((((fConst259 * fTemp29) + (fConst260 * fTemp33)) + (fConst261 * fTemp36)) + (fConst262 * fTemp38)) + (fConst263 * fTemp39)) + (fConst264 * fTemp41)) + (fConst261 * fTemp44)) + (fConst262 * fTemp46)) + (fConst263 * fTemp47)) + (fConst264 * fTemp49)) + (fConst259 * fTemp51)) + (fConst260 * fTemp54))) + ((((fConst107 * fTemp3) + (0.022574955908289243 * ((((fConst108 * fTemp8) + (fConst265 * fTemp11)) + (fConst266 * fTemp14)) + (fConst108 * fTemp17)))) + (0.021093750000000001 * ((((fConst267 * fTemp19) + (fConst268 * fTemp22)) + (fConst267 * fTemp24)) + (fConst268 * fTemp27)))) + (fConst106 * (((((((((((((((((((((0.081054939510132359 * fTemp1) + (0.050946219890026692 * fTemp6)) + (0.10189243978005338 * fTemp12)) + (0.10189243978005329 * fTemp13)) + (0.050946219890026692 * fTemp15)) + (0.051824122420700336 * fTemp20)) + (0.051824122420700267 * fTemp21)) + (0.051824122420700336 * fTemp25)) + (0.051824122420700267 * fTemp26)) + (0.0070590901547238384 * fTemp30)) + (0.0070590901547238306 * fTemp32)) + (0.063531811392514598 * fTemp35)) + (0.09176817201141016 * fTemp37)) + (0.091768172011410187 * fTemp40)) + (0.063531811392514445 * fTemp42)) + (0.063531811392514598 * fTemp43)) + (0.09176817201141016 * fTemp45)) + (0.091768172011410187 * fTemp48)) + (0.063531811392514445 * fTemp50)) + (0.0070590901547238384 * fTemp52)) + (0.0070590901547238306 * fTemp53))))));
			fRec16[0] = max((fRec16[1] - fConst0), min(6.0, (20.0 * log10(max(0.00031622776601683794, fabs(fTemp80))))));
			fVbargraph15 = FAUSTFLOAT(fRec16[0]);
			output15[i] = FAUSTFLOAT(fTemp80);
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
