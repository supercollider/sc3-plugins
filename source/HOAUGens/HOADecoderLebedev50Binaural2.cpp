//----------------------------------------------------------
// name: "HOADecoderLebedev50Binaural2"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL)"
// copyright: "(c) Pierre Lecomte 2015"
//
// Code generated with Faust 0.12.0 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with  */
#include <math.h>
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
         * @param samplingRate - the sampling rate in Herz
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
    
        virtual void addSoundfile(const char* label, Soundfile** sf_zone) {}

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


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
  private:
	double 	fConst0;
	FAUSTFLOAT 	fslider0;
	double 	fRec1[2];
	FAUSTFLOAT 	fslider1;
	double 	fRec3[2];
	double 	fTempPerm0;
	double 	fRec2[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fTempPerm1;
	int 	IOTA;
	double 	fVec0[128];
	double 	fTempPerm2;
	double 	fTempPerm3;
	double 	fTempPerm4;
	double 	fTempPerm5;
	double 	fTempPerm6;
	double 	fTempPerm7;
	double 	fTempPerm8;
	double 	fTempPerm9;
	double 	fTempPerm10;
	double 	fTempPerm11;
	double 	fTempPerm12;
	double 	fRec4[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fTempPerm13;
	double 	fVec1[128];
	double 	fTempPerm14;
	double 	fTempPerm15;
	double 	fTempPerm16;
	double 	fTempPerm17;
	double 	fTempPerm18;
	double 	fTempPerm19;
	double 	fTempPerm20;
	double 	fTempPerm21;
	double 	fTempPerm22;
	double 	fTempPerm23;
	double 	fTempPerm24;
	double 	fTempPerm25;
	double 	fTempPerm26;
	double 	fTempPerm27;
	double 	fTempPerm28;
	double 	fTempPerm29;
	double 	fTempPerm30;
	double 	fTempPerm31;
	double 	fRec5[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fTempPerm32;
	double 	fVec2[128];
	double 	fTempPerm33;
	double 	fTempPerm34;
	double 	fTempPerm35;
	double 	fTempPerm36;
	double 	fTempPerm37;
	double 	fTempPerm38;
	double 	fTempPerm39;
	double 	fTempPerm40;
	double 	fTempPerm41;
	double 	fTempPerm42;
	double 	fTempPerm43;
	double 	fRec6[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fTempPerm44;
	double 	fVec3[128];
	double 	fTempPerm45;
	double 	fTempPerm46;
	double 	fTempPerm47;
	double 	fTempPerm48;
	double 	fTempPerm49;
	double 	fTempPerm50;
	double 	fTempPerm51;
	double 	fTempPerm52;
	double 	fTempPerm53;
	double 	fTempPerm54;
	double 	fTempPerm55;
	double 	fTempPerm56;
	double 	fTempPerm57;
	double 	fTempPerm58;
	double 	fTempPerm59;
	double 	fTempPerm60;
	double 	fTempPerm61;
	double 	fTempPerm62;
	double 	fTempPerm63;
	double 	fTempPerm64;
	double 	fTempPerm65;
	double 	fTempPerm66;
	double 	fTempPerm67;
	double 	fTempPerm68;
	double 	fTempPerm69;
	double 	fTempPerm70;
	double 	fTempPerm71;
	double 	fTempPerm72;
	double 	fTempPerm73;
	double 	fTempPerm74;
	double 	fTempPerm75;
	double 	fTempPerm76;
	double 	fTempPerm77;
	double 	fTempPerm78;
	double 	fTempPerm79;
	double 	fTempPerm80;
	double 	fTempPerm81;
	double 	fTempPerm82;
	double 	fTempPerm83;
	double 	fTempPerm84;
	double 	fTempPerm85;
	double 	fTempPerm86;
	double 	fTempPerm87;
	double 	fTempPerm88;
	double 	fTempPerm89;
	double 	fTempPerm90;
	double 	fTempPerm91;
	double 	fTempPerm92;
	double 	fTempPerm93;
	double 	fTempPerm94;
	double 	fTempPerm95;
	double 	fTempPerm96;
	double 	fTempPerm97;
	double 	fTempPerm98;
	double 	fTempPerm99;
	double 	fTempPerm100;
	double 	fTempPerm101;
	double 	fTempPerm102;
	double 	fTempPerm103;
	double 	fTempPerm104;
	double 	fTempPerm105;
	double 	fTempPerm106;
	double 	fTempPerm107;
	double 	fTempPerm108;
	double 	fTempPerm109;
	double 	fTempPerm110;
	double 	fTempPerm111;
	double 	fTempPerm112;
	double 	fTempPerm113;
	double 	fTempPerm114;
	double 	fTempPerm115;
	double 	fRec7[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fTempPerm116;
	double 	fVec4[128];
	double 	fTempPerm117;
	double 	fTempPerm118;
	double 	fTempPerm119;
	double 	fTempPerm120;
	double 	fTempPerm121;
	double 	fTempPerm122;
	double 	fTempPerm123;
	double 	fTempPerm124;
	double 	fTempPerm125;
	double 	fRec8[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fTempPerm126;
	double 	fVec5[128];
	double 	fTempPerm127;
	double 	fTempPerm128;
	double 	fTempPerm129;
	double 	fTempPerm130;
	double 	fTempPerm131;
	double 	fTempPerm132;
	double 	fTempPerm133;
	double 	fTempPerm134;
	double 	fTempPerm135;
	double 	fTempPerm136;
	double 	fTempPerm137;
	double 	fTempPerm138;
	double 	fTempPerm139;
	double 	fTempPerm140;
	double 	fTempPerm141;
	double 	fTempPerm142;
	double 	fRec9[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fTempPerm143;
	double 	fVec6[128];
	double 	fTempPerm144;
	double 	fTempPerm145;
	double 	fTempPerm146;
	double 	fTempPerm147;
	double 	fTempPerm148;
	double 	fTempPerm149;
	double 	fTempPerm150;
	double 	fTempPerm151;
	double 	fTempPerm152;
	double 	fTempPerm153;
	double 	fTempPerm154;
	double 	fTempPerm155;
	double 	fTempPerm156;
	double 	fTempPerm157;
	double 	fTempPerm158;
	double 	fTempPerm159;
	double 	fTempPerm160;
	double 	fRec10[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fTempPerm161;
	double 	fVec7[128];
	double 	fTempPerm162;
	double 	fTempPerm163;
	double 	fTempPerm164;
	double 	fTempPerm165;
	double 	fTempPerm166;
	double 	fTempPerm167;
	double 	fTempPerm168;
	double 	fTempPerm169;
	double 	fTempPerm170;
	double 	fTempPerm171;
	double 	fTempPerm172;
	double 	fTempPerm173;
	double 	fTempPerm174;
	double 	fTempPerm175;
	double 	fTempPerm176;
	double 	fTempPerm177;
	double 	fTempPerm178;
	double 	fTempPerm179;
	double 	fTempPerm180;
	double 	fTempPerm181;
	double 	fTempPerm182;
	double 	fTempPerm183;
	double 	fTempPerm184;
	double 	fTempPerm185;
	double 	fTempPerm186;
	double 	fTempPerm187;
	double 	fTempPerm188;
	double 	fTempPerm189;
	double 	fTempPerm190;
	double 	fTempPerm191;
	double 	fTempPerm192;
	double 	fTempPerm193;
	double 	fTempPerm194;
	double 	fTempPerm195;
	double 	fTempPerm196;
	double 	fTempPerm197;
	double 	fTempPerm198;
	double 	fTempPerm199;
	double 	fTempPerm200;
	double 	fTempPerm201;
	double 	fTempPerm202;
	double 	fTempPerm203;
	double 	fTempPerm204;
	double 	fTempPerm205;
	double 	fTempPerm206;
	double 	fTempPerm207;
	double 	fTempPerm208;
	double 	fTempPerm209;
	double 	fTempPerm210;
	double 	fTempPerm211;
	double 	fTempPerm212;
	double 	fTempPerm213;
	double 	fTempPerm214;
	double 	fTempPerm215;
	double 	fTempPerm216;
	double 	fTempPerm217;
	double 	fTempPerm218;
	double 	fTempPerm219;
	double 	fTempPerm220;
	double 	fTempPerm221;
	double 	fTempPerm222;
	double 	fTempPerm223;
	double 	fTempPerm224;
	double 	fTempPerm225;
	double 	fTempPerm226;
	double 	fTempPerm227;
	double 	fTempPerm228;
	double 	fTempPerm229;
	double 	fTempPerm230;
	double 	fTempPerm231;
	double 	fTempPerm232;
	double 	fTempPerm233;
	double 	fTempPerm234;
	double 	fTempPerm235;
	double 	fTempPerm236;
	double 	fTempPerm237;
	double 	fTempPerm238;
	double 	fTempPerm239;
	double 	fTempPerm240;
	double 	fTempPerm241;
	double 	fTempPerm242;
	double 	fTempPerm243;
	double 	fTempPerm244;
	double 	fTempPerm245;
	double 	fTempPerm246;
	double 	fTempPerm247;
	double 	fTempPerm248;
	double 	fTempPerm249;
	double 	fTempPerm250;
	double 	fTempPerm251;
	double 	fTempPerm252;
	double 	fTempPerm253;
	double 	fTempPerm254;
	double 	fTempPerm255;
	double 	fTempPerm256;
	double 	fTempPerm257;
	double 	fTempPerm258;
	double 	fTempPerm259;
	double 	fTempPerm260;
	double 	fTempPerm261;
	double 	fTempPerm262;
	double 	fTempPerm263;
	double 	fTempPerm264;
	double 	fTempPerm265;
	double 	fTempPerm266;
	double 	fTempPerm267;
	double 	fTempPerm268;
	double 	fTempPerm269;
	double 	fTempPerm270;
	double 	fTempPerm271;
	double 	fTempPerm272;
	double 	fTempPerm273;
	double 	fTempPerm274;
	double 	fTempPerm275;
	double 	fTempPerm276;
	double 	fTempPerm277;
	double 	fTempPerm278;
	double 	fTempPerm279;
	double 	fTempPerm280;
	double 	fTempPerm281;
	double 	fTempPerm282;
	double 	fTempPerm283;
	double 	fTempPerm284;
	double 	fTempPerm285;
	double 	fTempPerm286;
	double 	fTempPerm287;
	double 	fTempPerm288;
	double 	fTempPerm289;
	double 	fTempPerm290;
	double 	fTempPerm291;
	double 	fTempPerm292;
	double 	fTempPerm293;
	double 	fTempPerm294;
	double 	fTempPerm295;
	double 	fTempPerm296;
	double 	fTempPerm297;
	double 	fTempPerm298;
	double 	fTempPerm299;
	double 	fTempPerm300;
	double 	fTempPerm301;
	double 	fTempPerm302;
	double 	fTempPerm303;
	double 	fTempPerm304;
	double 	fTempPerm305;
	double 	fTempPerm306;
	double 	fTempPerm307;
	double 	fRec11[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fTempPerm308;
	double 	fVec8[128];
	double 	fTempPerm309;
	double 	fTempPerm310;
	double 	fTempPerm311;
	double 	fTempPerm312;
	double 	fTempPerm313;
	double 	fTempPerm314;
	double 	fTempPerm315;
	double 	fTempPerm316;
	double 	fTempPerm317;
	double 	fTempPerm318;
	double 	fTempPerm319;
	double 	fTempPerm320;
	double 	fTempPerm321;
	double 	fTempPerm322;
	double 	fTempPerm323;
	double 	fTempPerm324;
	double 	fTempPerm325;
	double 	fTempPerm326;
	double 	fTempPerm327;
	double 	fTempPerm328;
	double 	fTempPerm329;
	double 	fTempPerm330;
	double 	fTempPerm331;
	double 	fTempPerm332;
	double 	fTempPerm333;
	double 	fTempPerm334;
	double 	fTempPerm335;
	double 	fTempPerm336;
	double 	fTempPerm337;
	double 	fTempPerm338;
	double 	fTempPerm339;
	double 	fTempPerm340;
	double 	fTempPerm341;
	double 	fTempPerm342;
	double 	fTempPerm343;
	double 	fTempPerm344;
	double 	fTempPerm345;
	double 	fTempPerm346;
	double 	fTempPerm347;
	double 	fTempPerm348;
	double 	fTempPerm349;
	double 	fTempPerm350;
	double 	fTempPerm351;
	double 	fTempPerm352;
	double 	fTempPerm353;
	double 	fTempPerm354;
	double 	fTempPerm355;
	double 	fTempPerm356;
	double 	fTempPerm357;
	double 	fTempPerm358;
	double 	fTempPerm359;
	double 	fTempPerm360;
	double 	fTempPerm361;
	double 	fTempPerm362;
	double 	fTempPerm363;
	double 	fTempPerm364;
	double 	fTempPerm365;
	double 	fTempPerm366;
	double 	fTempPerm367;
	double 	fTempPerm368;
	double 	fTempPerm369;
	double 	fTempPerm370;
	double 	fTempPerm371;
	double 	fTempPerm372;
	double 	fTempPerm373;
	double 	fTempPerm374;
	double 	fTempPerm375;
	double 	fTempPerm376;
	double 	fTempPerm377;
	double 	fTempPerm378;
	double 	fTempPerm379;
	double 	fTempPerm380;
	double 	fTempPerm381;
	double 	fTempPerm382;
	double 	fTempPerm383;
	double 	fTempPerm384;
	double 	fTempPerm385;
	double 	fTempPerm386;
	double 	fTempPerm387;
	double 	fTempPerm388;
	double 	fTempPerm389;
	double 	fTempPerm390;
	double 	fTempPerm391;
	double 	fTempPerm392;
	double 	fTempPerm393;
	double 	fTempPerm394;
	double 	fTempPerm395;
	double 	fTempPerm396;
	double 	fTempPerm397;
	double 	fTempPerm398;
	double 	fTempPerm399;
	double 	fTempPerm400;
	double 	fTempPerm401;
	double 	fTempPerm402;
	double 	fTempPerm403;
	double 	fTempPerm404;
	double 	fTempPerm405;
	double 	fTempPerm406;
	double 	fTempPerm407;
	double 	fTempPerm408;
	double 	fTempPerm409;
	double 	fTempPerm410;
	double 	fTempPerm411;
	double 	fTempPerm412;
	double 	fTempPerm413;
	double 	fTempPerm414;
	double 	fTempPerm415;
	double 	fTempPerm416;
	double 	fTempPerm417;
	double 	fTempPerm418;
	double 	fTempPerm419;
	double 	fTempPerm420;
	double 	fTempPerm421;
	double 	fTempPerm422;
	double 	fTempPerm423;
	double 	fTempPerm424;
	double 	fTempPerm425;
	double 	fTempPerm426;
	double 	fTempPerm427;
	double 	fTempPerm428;
	double 	fTempPerm429;
	double 	fTempPerm430;
	double 	fTempPerm431;
	double 	fTempPerm432;
	double 	fTempPerm433;
	double 	fTempPerm434;
	double 	fTempPerm435;
	double 	fTempPerm436;
	double 	fTempPerm437;
	double 	fTempPerm438;
	double 	fTempPerm439;
	double 	fTempPerm440;
	double 	fTempPerm441;
	double 	fTempPerm442;
	double 	fTempPerm443;
	double 	fTempPerm444;
	double 	fTempPerm445;
	double 	fTempPerm446;
	double 	fTempPerm447;
	double 	fTempPerm448;
	double 	fTempPerm449;
	double 	fTempPerm450;
	double 	fTempPerm451;
	double 	fTempPerm452;
	double 	fTempPerm453;
	double 	fTempPerm454;
	double 	fTempPerm455;
	double 	fTempPerm456;
	double 	fTempPerm457;
	double 	fTempPerm458;
	double 	fTempPerm459;
	double 	fTempPerm460;
	double 	fTempPerm461;
	double 	fTempPerm462;
	double 	fTempPerm463;
	double 	fTempPerm464;
	double 	fTempPerm465;
	double 	fTempPerm466;
	double 	fTempPerm467;
	double 	fTempPerm468;
	double 	fTempPerm469;
	double 	fTempPerm470;
	double 	fTempPerm471;
	double 	fTempPerm472;
	double 	fTempPerm473;
	double 	fTempPerm474;
	double 	fTempPerm475;
	double 	fTempPerm476;
	double 	fTempPerm477;
	double 	fTempPerm478;
	double 	fTempPerm479;
	double 	fTempPerm480;
	double 	fTempPerm481;
	double 	fTempPerm482;
	double 	fTempPerm483;
	double 	fTempPerm484;
	double 	fTempPerm485;
	double 	fTempPerm486;
	double 	fTempPerm487;
	double 	fTempPerm488;
	double 	fTempPerm489;
	double 	fTempPerm490;
	double 	fTempPerm491;
	double 	fTempPerm492;
	double 	fTempPerm493;
	double 	fTempPerm494;
	double 	fTempPerm495;
	double 	fTempPerm496;
	double 	fTempPerm497;
	double 	fTempPerm498;
	double 	fTempPerm499;
	double 	fTempPerm500;
	double 	fTempPerm501;
	double 	fTempPerm502;
	double 	fTempPerm503;
	double 	fTempPerm504;
	double 	fTempPerm505;
	double 	fTempPerm506;
	double 	fTempPerm507;
	double 	fTempPerm508;
	double 	fTempPerm509;
	double 	fTempPerm510;
	double 	fTempPerm511;
	double 	fTempPerm512;
	double 	fTempPerm513;
	double 	fTempPerm514;
	double 	fTempPerm515;
	double 	fTempPerm516;
	double 	fTempPerm517;
	double 	fTempPerm518;
	double 	fTempPerm519;
	double 	fTempPerm520;
	double 	fTempPerm521;
	double 	fTempPerm522;
	double 	fTempPerm523;
	double 	fTempPerm524;
	double 	fTempPerm525;
	double 	fTempPerm526;
	double 	fTempPerm527;
	double 	fTempPerm528;
	double 	fTempPerm529;
	double 	fTempPerm530;
	double 	fTempPerm531;
	double 	fTempPerm532;
	double 	fTempPerm533;
	double 	fTempPerm534;
	double 	fTempPerm535;
	double 	fTempPerm536;
	double 	fTempPerm537;
	double 	fTempPerm538;
	double 	fTempPerm539;
	double 	fTempPerm540;
	double 	fTempPerm541;
	double 	fTempPerm542;
	double 	fTempPerm543;
	double 	fTempPerm544;
	double 	fTempPerm545;
	double 	fTempPerm546;
	double 	fTempPerm547;
	double 	fTempPerm548;
	double 	fTempPerm549;
	double 	fTempPerm550;
	double 	fTempPerm551;
	double 	fTempPerm552;
	double 	fTempPerm553;
	double 	fTempPerm554;
	double 	fTempPerm555;
	double 	fTempPerm556;
	double 	fTempPerm557;
	double 	fTempPerm558;
	double 	fTempPerm559;
	double 	fTempPerm560;
	double 	fTempPerm561;
	double 	fTempPerm562;
	double 	fTempPerm563;
	double 	fTempPerm564;
	double 	fTempPerm565;
	double 	fTempPerm566;
	double 	fTempPerm567;
	double 	fTempPerm568;
	double 	fTempPerm569;
	double 	fTempPerm570;
	double 	fTempPerm571;
	double 	fTempPerm572;
	double 	fTempPerm573;
	double 	fTempPerm574;
	double 	fTempPerm575;
	double 	fTempPerm576;
	double 	fTempPerm577;
	double 	fTempPerm578;
	double 	fTempPerm579;
	double 	fTempPerm580;
	double 	fTempPerm581;
	double 	fTempPerm582;
	double 	fTempPerm583;
	double 	fTempPerm584;
	double 	fTempPerm585;
	double 	fTempPerm586;
	double 	fTempPerm587;
	double 	fTempPerm588;
	double 	fTempPerm589;
	double 	fTempPerm590;
	double 	fTempPerm591;
	double 	fTempPerm592;
	double 	fTempPerm593;
	double 	fTempPerm594;
	double 	fTempPerm595;
	double 	fTempPerm596;
	double 	fTempPerm597;
	double 	fTempPerm598;
	double 	fTempPerm599;
	double 	fTempPerm600;
	double 	fTempPerm601;
	double 	fTempPerm602;
	double 	fTempPerm603;
	double 	fTempPerm604;
	double 	fTempPerm605;
	double 	fTempPerm606;
	double 	fTempPerm607;
	double 	fTempPerm608;
	double 	fTempPerm609;
	double 	fTempPerm610;
	double 	fTempPerm611;
	double 	fTempPerm612;
	double 	fTempPerm613;
	double 	fTempPerm614;
	double 	fTempPerm615;
	double 	fTempPerm616;
	double 	fTempPerm617;
	double 	fTempPerm618;
	double 	fTempPerm619;
	double 	fTempPerm620;
	double 	fTempPerm621;
	double 	fTempPerm622;
	double 	fTempPerm623;
	double 	fTempPerm624;
	double 	fTempPerm625;
	double 	fTempPerm626;
	double 	fTempPerm627;
	double 	fTempPerm628;
	double 	fTempPerm629;
	double 	fTempPerm630;
	double 	fTempPerm631;
	double 	fTempPerm632;
	double 	fTempPerm633;
	double 	fTempPerm634;
	double 	fTempPerm635;
	double 	fTempPerm636;
	double 	fTempPerm637;
	double 	fTempPerm638;
	double 	fTempPerm639;
	double 	fTempPerm640;
	double 	fTempPerm641;
	double 	fTempPerm642;
	double 	fTempPerm643;
	double 	fTempPerm644;
	double 	fTempPerm645;
	double 	fTempPerm646;
	double 	fTempPerm647;
	double 	fTempPerm648;
	double 	fTempPerm649;
	double 	fTempPerm650;
	double 	fTempPerm651;
	double 	fTempPerm652;
	double 	fTempPerm653;
	double 	fTempPerm654;
	double 	fTempPerm655;
	double 	fTempPerm656;
	double 	fTempPerm657;
	double 	fTempPerm658;
	double 	fTempPerm659;
	double 	fTempPerm660;
	double 	fTempPerm661;
	double 	fTempPerm662;
	double 	fTempPerm663;
	double 	fTempPerm664;
	double 	fTempPerm665;
	double 	fTempPerm666;
	double 	fTempPerm667;
	double 	fTempPerm668;
	double 	fTempPerm669;
	double 	fTempPerm670;
	double 	fTempPerm671;
	double 	fTempPerm672;
	double 	fTempPerm673;
	double 	fTempPerm674;
	double 	fTempPerm675;
	double 	fTempPerm676;
	double 	fTempPerm677;
	double 	fTempPerm678;
	double 	fTempPerm679;
	double 	fTempPerm680;
	double 	fTempPerm681;
	double 	fTempPerm682;
	double 	fTempPerm683;
	double 	fTempPerm684;
	double 	fTempPerm685;
	double 	fTempPerm686;
	double 	fTempPerm687;
	double 	fTempPerm688;
	double 	fTempPerm689;
	double 	fTempPerm690;
	double 	fTempPerm691;
	double 	fTempPerm692;
	double 	fTempPerm693;
	double 	fTempPerm694;
	double 	fTempPerm695;
	double 	fTempPerm696;
	double 	fTempPerm697;
	double 	fTempPerm698;
	double 	fTempPerm699;
	double 	fTempPerm700;
	double 	fTempPerm701;
	double 	fTempPerm702;
	double 	fTempPerm703;
	double 	fTempPerm704;
	double 	fTempPerm705;
	double 	fTempPerm706;
	double 	fTempPerm707;
	double 	fTempPerm708;
	double 	fTempPerm709;
	double 	fTempPerm710;
	double 	fTempPerm711;
	double 	fTempPerm712;
	double 	fTempPerm713;
	double 	fTempPerm714;
	double 	fTempPerm715;
	double 	fTempPerm716;
	double 	fTempPerm717;
	double 	fTempPerm718;
	double 	fTempPerm719;
	double 	fTempPerm720;
	double 	fTempPerm721;
	double 	fTempPerm722;
	double 	fTempPerm723;
	double 	fTempPerm724;
	double 	fTempPerm725;
	double 	fTempPerm726;
	double 	fTempPerm727;
	double 	fTempPerm728;
	double 	fTempPerm729;
	double 	fTempPerm730;
	double 	fTempPerm731;
	double 	fTempPerm732;
	double 	fTempPerm733;
	double 	fTempPerm734;
	double 	fTempPerm735;
	double 	fTempPerm736;
	double 	fTempPerm737;
	double 	fTempPerm738;
	double 	fTempPerm739;
	double 	fTempPerm740;
	double 	fTempPerm741;
	double 	fTempPerm742;
	double 	fTempPerm743;
	double 	fTempPerm744;
	double 	fTempPerm745;
	double 	fTempPerm746;
	double 	fTempPerm747;
	double 	fTempPerm748;
	double 	fTempPerm749;
	double 	fTempPerm750;
	double 	fTempPerm751;
	double 	fTempPerm752;
	double 	fTempPerm753;
	double 	fTempPerm754;
	double 	fTempPerm755;
	double 	fTempPerm756;
	double 	fTempPerm757;
	double 	fTempPerm758;
	double 	fTempPerm759;
	double 	fTempPerm760;
	double 	fTempPerm761;
	double 	fTempPerm762;
	double 	fTempPerm763;
	double 	fTempPerm764;
	double 	fTempPerm765;
	double 	fTempPerm766;
	double 	fTempPerm767;
	double 	fTempPerm768;
	double 	fTempPerm769;
	double 	fTempPerm770;
	double 	fTempPerm771;
	double 	fTempPerm772;
	double 	fTempPerm773;
	double 	fTempPerm774;
	double 	fTempPerm775;
	double 	fTempPerm776;
	double 	fTempPerm777;
	double 	fTempPerm778;
	double 	fTempPerm779;
	double 	fTempPerm780;
	double 	fTempPerm781;
	double 	fTempPerm782;
	double 	fTempPerm783;
	double 	fTempPerm784;
	double 	fTempPerm785;
	double 	fTempPerm786;
	double 	fTempPerm787;
	double 	fTempPerm788;
	double 	fTempPerm789;
	double 	fTempPerm790;
	double 	fTempPerm791;
	double 	fTempPerm792;
	double 	fTempPerm793;
	double 	fTempPerm794;
	double 	fTempPerm795;
	double 	fTempPerm796;
	double 	fTempPerm797;
	double 	fTempPerm798;
	double 	fTempPerm799;
	double 	fTempPerm800;
	double 	fTempPerm801;
	double 	fTempPerm802;
	double 	fTempPerm803;
	double 	fTempPerm804;
	double 	fTempPerm805;
	double 	fTempPerm806;
	double 	fTempPerm807;
	double 	fTempPerm808;
	double 	fTempPerm809;
	double 	fTempPerm810;
	double 	fTempPerm811;
	double 	fTempPerm812;
	double 	fTempPerm813;
	double 	fTempPerm814;
	double 	fTempPerm815;
	double 	fTempPerm816;
	double 	fTempPerm817;
	double 	fTempPerm818;
	double 	fTempPerm819;
	double 	fTempPerm820;
	double 	fTempPerm821;
	double 	fTempPerm822;
	double 	fTempPerm823;
	double 	fTempPerm824;
	double 	fTempPerm825;
	double 	fTempPerm826;
	double 	fTempPerm827;
	double 	fTempPerm828;
	double 	fTempPerm829;
	double 	fTempPerm830;
	double 	fTempPerm831;
	double 	fTempPerm832;
	double 	fTempPerm833;
	double 	fTempPerm834;
	double 	fTempPerm835;
	double 	fTempPerm836;
	double 	fTempPerm837;
	double 	fTempPerm838;
	double 	fTempPerm839;
	double 	fTempPerm840;
	double 	fTempPerm841;
	double 	fTempPerm842;
	double 	fTempPerm843;
	double 	fTempPerm844;
	double 	fTempPerm845;
	double 	fTempPerm846;
	double 	fTempPerm847;
	double 	fTempPerm848;
	double 	fTempPerm849;
	double 	fTempPerm850;
	double 	fTempPerm851;
	double 	fTempPerm852;
	double 	fTempPerm853;
	double 	fTempPerm854;
	double 	fTempPerm855;
	double 	fTempPerm856;
	double 	fTempPerm857;
	double 	fTempPerm858;
	double 	fTempPerm859;
	double 	fTempPerm860;
	double 	fTempPerm861;
	double 	fTempPerm862;
	double 	fTempPerm863;
	double 	fTempPerm864;
	double 	fTempPerm865;
	double 	fTempPerm866;
	double 	fTempPerm867;
	double 	fTempPerm868;
	double 	fTempPerm869;
	double 	fTempPerm870;
	double 	fTempPerm871;
	double 	fTempPerm872;
	double 	fTempPerm873;
	double 	fTempPerm874;
	double 	fTempPerm875;
	double 	fTempPerm876;
	double 	fTempPerm877;
	double 	fTempPerm878;
	double 	fTempPerm879;
	double 	fTempPerm880;
	double 	fTempPerm881;
	double 	fTempPerm882;
	double 	fTempPerm883;
	double 	fTempPerm884;
	double 	fTempPerm885;
	double 	fTempPerm886;
	double 	fTempPerm887;
	double 	fTempPerm888;
	double 	fTempPerm889;
	double 	fTempPerm890;
	double 	fTempPerm891;
	double 	fTempPerm892;
	double 	fTempPerm893;
	double 	fTempPerm894;
	double 	fTempPerm895;
	double 	fTempPerm896;
	double 	fTempPerm897;
	double 	fTempPerm898;
	double 	fTempPerm899;
	double 	fTempPerm900;
	double 	fTempPerm901;
	double 	fTempPerm902;
	double 	fTempPerm903;
	double 	fTempPerm904;
	double 	fTempPerm905;
	double 	fTempPerm906;
	double 	fTempPerm907;
	double 	fTempPerm908;
	double 	fTempPerm909;
	double 	fTempPerm910;
	double 	fTempPerm911;
	double 	fTempPerm912;
	double 	fTempPerm913;
	double 	fTempPerm914;
	double 	fTempPerm915;
	double 	fTempPerm916;
	double 	fTempPerm917;
	double 	fTempPerm918;
	double 	fTempPerm919;
	double 	fTempPerm920;
	double 	fTempPerm921;
	double 	fTempPerm922;
	double 	fTempPerm923;
	double 	fTempPerm924;
	double 	fTempPerm925;
	double 	fTempPerm926;
	double 	fTempPerm927;
	double 	fTempPerm928;
	double 	fTempPerm929;
	double 	fTempPerm930;
	double 	fTempPerm931;
	double 	fTempPerm932;
	double 	fTempPerm933;
	double 	fTempPerm934;
	double 	fTempPerm935;
	double 	fTempPerm936;
	double 	fTempPerm937;
	double 	fTempPerm938;
	double 	fTempPerm939;
	double 	fTempPerm940;
	double 	fTempPerm941;
	double 	fTempPerm942;
	double 	fTempPerm943;
	double 	fTempPerm944;
	double 	fTempPerm945;
	double 	fTempPerm946;
	double 	fTempPerm947;
	double 	fTempPerm948;
	double 	fTempPerm949;
	double 	fTempPerm950;
	double 	fTempPerm951;
	double 	fTempPerm952;
	double 	fTempPerm953;
	double 	fTempPerm954;
	double 	fTempPerm955;
	double 	fTempPerm956;
	double 	fTempPerm957;
	double 	fTempPerm958;
	double 	fTempPerm959;
	double 	fTempPerm960;
	double 	fTempPerm961;
	double 	fTempPerm962;
	double 	fTempPerm963;
	double 	fTempPerm964;
	double 	fTempPerm965;
	double 	fTempPerm966;
	double 	fTempPerm967;
	double 	fTempPerm968;
	double 	fTempPerm969;
	double 	fTempPerm970;
	double 	fTempPerm971;
	double 	fTempPerm972;
	double 	fTempPerm973;
	double 	fTempPerm974;
	double 	fTempPerm975;
	double 	fTempPerm976;
	double 	fTempPerm977;
	double 	fTempPerm978;
	double 	fTempPerm979;
	double 	fTempPerm980;
	double 	fTempPerm981;
	double 	fTempPerm982;
	double 	fTempPerm983;
	double 	fTempPerm984;
	double 	fTempPerm985;
	double 	fTempPerm986;
	double 	fTempPerm987;
	double 	fTempPerm988;
	double 	fTempPerm989;
	double 	fTempPerm990;
	double 	fTempPerm991;
	double 	fTempPerm992;
	double 	fTempPerm993;
	double 	fTempPerm994;
	double 	fTempPerm995;
	double 	fTempPerm996;
	double 	fTempPerm997;
	double 	fTempPerm998;
	double 	fTempPerm999;
	double 	fTempPerm1000;
	double 	fTempPerm1001;
	double 	fTempPerm1002;
	double 	fTempPerm1003;
	double 	fTempPerm1004;
	double 	fTempPerm1005;
	double 	fTempPerm1006;
	double 	fTempPerm1007;
	double 	fTempPerm1008;
	double 	fTempPerm1009;
	double 	fTempPerm1010;
	double 	fTempPerm1011;
	double 	fTempPerm1012;
	double 	fTempPerm1013;
	double 	fTempPerm1014;
	double 	fTempPerm1015;
	double 	fTempPerm1016;
	double 	fTempPerm1017;
	double 	fTempPerm1018;
	double 	fTempPerm1019;
	double 	fTempPerm1020;
	double 	fTempPerm1021;
	double 	fTempPerm1022;
	double 	fTempPerm1023;
	double 	fTempPerm1024;
	double 	fTempPerm1025;
	double 	fTempPerm1026;
	double 	fTempPerm1027;
	double 	fTempPerm1028;
	double 	fTempPerm1029;
	double 	fTempPerm1030;
	double 	fTempPerm1031;
	double 	fTempPerm1032;
	double 	fTempPerm1033;
	double 	fTempPerm1034;
	double 	fTempPerm1035;
	double 	fTempPerm1036;
	double 	fTempPerm1037;
	double 	fTempPerm1038;
	double 	fTempPerm1039;
	double 	fTempPerm1040;
	double 	fTempPerm1041;
	double 	fTempPerm1042;
	double 	fTempPerm1043;
	double 	fTempPerm1044;
	double 	fTempPerm1045;
	double 	fTempPerm1046;
	double 	fTempPerm1047;
	double 	fTempPerm1048;
	double 	fTempPerm1049;
	double 	fTempPerm1050;
	double 	fTempPerm1051;
	double 	fTempPerm1052;
	double 	fTempPerm1053;
	double 	fTempPerm1054;
	double 	fTempPerm1055;
	double 	fTempPerm1056;
	double 	fTempPerm1057;
	double 	fTempPerm1058;
	double 	fTempPerm1059;
	double 	fTempPerm1060;
	double 	fTempPerm1061;
	double 	fTempPerm1062;
	double 	fTempPerm1063;
	double 	fTempPerm1064;
	double 	fTempPerm1065;
	double 	fTempPerm1066;
	double 	fTempPerm1067;
	double 	fTempPerm1068;
	double 	fTempPerm1069;
	double 	fTempPerm1070;
	double 	fTempPerm1071;
	double 	fTempPerm1072;
	double 	fTempPerm1073;
	double 	fTempPerm1074;
	double 	fTempPerm1075;
	double 	fTempPerm1076;
	double 	fTempPerm1077;
	double 	fTempPerm1078;
	double 	fTempPerm1079;
	double 	fTempPerm1080;
	double 	fTempPerm1081;
	double 	fTempPerm1082;
	double 	fTempPerm1083;
	double 	fTempPerm1084;
	double 	fTempPerm1085;
	double 	fTempPerm1086;
	double 	fTempPerm1087;
	double 	fTempPerm1088;
	double 	fTempPerm1089;
	double 	fTempPerm1090;
	double 	fTempPerm1091;
	double 	fTempPerm1092;
	double 	fTempPerm1093;
	double 	fTempPerm1094;
	double 	fTempPerm1095;
	double 	fTempPerm1096;
	double 	fTempPerm1097;
	double 	fTempPerm1098;
	double 	fTempPerm1099;
	double 	fTempPerm1100;
	double 	fTempPerm1101;
	double 	fTempPerm1102;
	double 	fTempPerm1103;
	double 	fTempPerm1104;
	double 	fTempPerm1105;
	double 	fTempPerm1106;
	double 	fTempPerm1107;
	double 	fTempPerm1108;
	double 	fTempPerm1109;
	double 	fTempPerm1110;
	double 	fTempPerm1111;
	double 	fTempPerm1112;
	double 	fTempPerm1113;
	double 	fTempPerm1114;
	double 	fTempPerm1115;
	double 	fTempPerm1116;
	double 	fTempPerm1117;
	double 	fTempPerm1118;
	double 	fTempPerm1119;
	double 	fTempPerm1120;
	double 	fTempPerm1121;
	double 	fTempPerm1122;
	double 	fTempPerm1123;
	double 	fTempPerm1124;
	double 	fTempPerm1125;
	double 	fTempPerm1126;
	double 	fTempPerm1127;
	double 	fTempPerm1128;
	double 	fTempPerm1129;
	double 	fTempPerm1130;
	double 	fTempPerm1131;
	double 	fTempPerm1132;
	double 	fTempPerm1133;
	double 	fTempPerm1134;
	double 	fTempPerm1135;
	double 	fTempPerm1136;
	double 	fTempPerm1137;
	double 	fTempPerm1138;
	double 	fTempPerm1139;
	double 	fTempPerm1140;
	double 	fTempPerm1141;
	double 	fTempPerm1142;
	double 	fTempPerm1143;
	double 	fTempPerm1144;
	double 	fTempPerm1145;
	double 	fTempPerm1146;
	double 	fTempPerm1147;
	double 	fTempPerm1148;
	double 	fTempPerm1149;
	double 	fTempPerm1150;
	double 	fTempPerm1151;
	double 	fTempPerm1152;
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fTempPerm1153;
	double 	fRec12[2];
	FAUSTFLOAT 	fbargraph10;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("gui.lib/name", "GUI Library");
		m->declare("gui.lib/version", "1.0");
		m->declare("gui.lib/author", "Pierre Lecomte");
		m->declare("gui.lib/license", "GPL");
		m->declare("gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("name", "HOADecoderLebedev50Binaural2");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL)");
		m->declare("copyright", "(c) Pierre Lecomte 2015");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
	}

	virtual int getNumInputs() { return 9; }
	virtual int getNumOutputs() { return 2; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = (8e+01 / min(1.92e+05, max(1.0, (double)fSamplingFreq)));
		fTempPerm0 = 0;
		fTempPerm1 = 0;
		fTempPerm2 = 0;
		fTempPerm3 = 0;
		fTempPerm4 = 0;
		fTempPerm5 = 0;
		fTempPerm6 = 0;
		fTempPerm7 = 0;
		fTempPerm8 = 0;
		fTempPerm9 = 0;
		fTempPerm10 = 0;
		fTempPerm11 = 0;
		fTempPerm12 = 0;
		fTempPerm13 = 0;
		fTempPerm14 = 0;
		fTempPerm15 = 0;
		fTempPerm16 = 0;
		fTempPerm17 = 0;
		fTempPerm18 = 0;
		fTempPerm19 = 0;
		fTempPerm20 = 0;
		fTempPerm21 = 0;
		fTempPerm22 = 0;
		fTempPerm23 = 0;
		fTempPerm24 = 0;
		fTempPerm25 = 0;
		fTempPerm26 = 0;
		fTempPerm27 = 0;
		fTempPerm28 = 0;
		fTempPerm29 = 0;
		fTempPerm30 = 0;
		fTempPerm31 = 0;
		fTempPerm32 = 0;
		fTempPerm33 = 0;
		fTempPerm34 = 0;
		fTempPerm35 = 0;
		fTempPerm36 = 0;
		fTempPerm37 = 0;
		fTempPerm38 = 0;
		fTempPerm39 = 0;
		fTempPerm40 = 0;
		fTempPerm41 = 0;
		fTempPerm42 = 0;
		fTempPerm43 = 0;
		fTempPerm44 = 0;
		fTempPerm45 = 0;
		fTempPerm46 = 0;
		fTempPerm47 = 0;
		fTempPerm48 = 0;
		fTempPerm49 = 0;
		fTempPerm50 = 0;
		fTempPerm51 = 0;
		fTempPerm52 = 0;
		fTempPerm53 = 0;
		fTempPerm54 = 0;
		fTempPerm55 = 0;
		fTempPerm56 = 0;
		fTempPerm57 = 0;
		fTempPerm58 = 0;
		fTempPerm59 = 0;
		fTempPerm60 = 0;
		fTempPerm61 = 0;
		fTempPerm62 = 0;
		fTempPerm63 = 0;
		fTempPerm64 = 0;
		fTempPerm65 = 0;
		fTempPerm66 = 0;
		fTempPerm67 = 0;
		fTempPerm68 = 0;
		fTempPerm69 = 0;
		fTempPerm70 = 0;
		fTempPerm71 = 0;
		fTempPerm72 = 0;
		fTempPerm73 = 0;
		fTempPerm74 = 0;
		fTempPerm75 = 0;
		fTempPerm76 = 0;
		fTempPerm77 = 0;
		fTempPerm78 = 0;
		fTempPerm79 = 0;
		fTempPerm80 = 0;
		fTempPerm81 = 0;
		fTempPerm82 = 0;
		fTempPerm83 = 0;
		fTempPerm84 = 0;
		fTempPerm85 = 0;
		fTempPerm86 = 0;
		fTempPerm87 = 0;
		fTempPerm88 = 0;
		fTempPerm89 = 0;
		fTempPerm90 = 0;
		fTempPerm91 = 0;
		fTempPerm92 = 0;
		fTempPerm93 = 0;
		fTempPerm94 = 0;
		fTempPerm95 = 0;
		fTempPerm96 = 0;
		fTempPerm97 = 0;
		fTempPerm98 = 0;
		fTempPerm99 = 0;
		fTempPerm100 = 0;
		fTempPerm101 = 0;
		fTempPerm102 = 0;
		fTempPerm103 = 0;
		fTempPerm104 = 0;
		fTempPerm105 = 0;
		fTempPerm106 = 0;
		fTempPerm107 = 0;
		fTempPerm108 = 0;
		fTempPerm109 = 0;
		fTempPerm110 = 0;
		fTempPerm111 = 0;
		fTempPerm112 = 0;
		fTempPerm113 = 0;
		fTempPerm114 = 0;
		fTempPerm115 = 0;
		fTempPerm116 = 0;
		fTempPerm117 = 0;
		fTempPerm118 = 0;
		fTempPerm119 = 0;
		fTempPerm120 = 0;
		fTempPerm121 = 0;
		fTempPerm122 = 0;
		fTempPerm123 = 0;
		fTempPerm124 = 0;
		fTempPerm125 = 0;
		fTempPerm126 = 0;
		fTempPerm127 = 0;
		fTempPerm128 = 0;
		fTempPerm129 = 0;
		fTempPerm130 = 0;
		fTempPerm131 = 0;
		fTempPerm132 = 0;
		fTempPerm133 = 0;
		fTempPerm134 = 0;
		fTempPerm135 = 0;
		fTempPerm136 = 0;
		fTempPerm137 = 0;
		fTempPerm138 = 0;
		fTempPerm139 = 0;
		fTempPerm140 = 0;
		fTempPerm141 = 0;
		fTempPerm142 = 0;
		fTempPerm143 = 0;
		fTempPerm144 = 0;
		fTempPerm145 = 0;
		fTempPerm146 = 0;
		fTempPerm147 = 0;
		fTempPerm148 = 0;
		fTempPerm149 = 0;
		fTempPerm150 = 0;
		fTempPerm151 = 0;
		fTempPerm152 = 0;
		fTempPerm153 = 0;
		fTempPerm154 = 0;
		fTempPerm155 = 0;
		fTempPerm156 = 0;
		fTempPerm157 = 0;
		fTempPerm158 = 0;
		fTempPerm159 = 0;
		fTempPerm160 = 0;
		fTempPerm161 = 0;
		fTempPerm162 = 0;
		fTempPerm163 = 0;
		fTempPerm164 = 0;
		fTempPerm165 = 0;
		fTempPerm166 = 0;
		fTempPerm167 = 0;
		fTempPerm168 = 0;
		fTempPerm169 = 0;
		fTempPerm170 = 0;
		fTempPerm171 = 0;
		fTempPerm172 = 0;
		fTempPerm173 = 0;
		fTempPerm174 = 0;
		fTempPerm175 = 0;
		fTempPerm176 = 0;
		fTempPerm177 = 0;
		fTempPerm178 = 0;
		fTempPerm179 = 0;
		fTempPerm180 = 0;
		fTempPerm181 = 0;
		fTempPerm182 = 0;
		fTempPerm183 = 0;
		fTempPerm184 = 0;
		fTempPerm185 = 0;
		fTempPerm186 = 0;
		fTempPerm187 = 0;
		fTempPerm188 = 0;
		fTempPerm189 = 0;
		fTempPerm190 = 0;
		fTempPerm191 = 0;
		fTempPerm192 = 0;
		fTempPerm193 = 0;
		fTempPerm194 = 0;
		fTempPerm195 = 0;
		fTempPerm196 = 0;
		fTempPerm197 = 0;
		fTempPerm198 = 0;
		fTempPerm199 = 0;
		fTempPerm200 = 0;
		fTempPerm201 = 0;
		fTempPerm202 = 0;
		fTempPerm203 = 0;
		fTempPerm204 = 0;
		fTempPerm205 = 0;
		fTempPerm206 = 0;
		fTempPerm207 = 0;
		fTempPerm208 = 0;
		fTempPerm209 = 0;
		fTempPerm210 = 0;
		fTempPerm211 = 0;
		fTempPerm212 = 0;
		fTempPerm213 = 0;
		fTempPerm214 = 0;
		fTempPerm215 = 0;
		fTempPerm216 = 0;
		fTempPerm217 = 0;
		fTempPerm218 = 0;
		fTempPerm219 = 0;
		fTempPerm220 = 0;
		fTempPerm221 = 0;
		fTempPerm222 = 0;
		fTempPerm223 = 0;
		fTempPerm224 = 0;
		fTempPerm225 = 0;
		fTempPerm226 = 0;
		fTempPerm227 = 0;
		fTempPerm228 = 0;
		fTempPerm229 = 0;
		fTempPerm230 = 0;
		fTempPerm231 = 0;
		fTempPerm232 = 0;
		fTempPerm233 = 0;
		fTempPerm234 = 0;
		fTempPerm235 = 0;
		fTempPerm236 = 0;
		fTempPerm237 = 0;
		fTempPerm238 = 0;
		fTempPerm239 = 0;
		fTempPerm240 = 0;
		fTempPerm241 = 0;
		fTempPerm242 = 0;
		fTempPerm243 = 0;
		fTempPerm244 = 0;
		fTempPerm245 = 0;
		fTempPerm246 = 0;
		fTempPerm247 = 0;
		fTempPerm248 = 0;
		fTempPerm249 = 0;
		fTempPerm250 = 0;
		fTempPerm251 = 0;
		fTempPerm252 = 0;
		fTempPerm253 = 0;
		fTempPerm254 = 0;
		fTempPerm255 = 0;
		fTempPerm256 = 0;
		fTempPerm257 = 0;
		fTempPerm258 = 0;
		fTempPerm259 = 0;
		fTempPerm260 = 0;
		fTempPerm261 = 0;
		fTempPerm262 = 0;
		fTempPerm263 = 0;
		fTempPerm264 = 0;
		fTempPerm265 = 0;
		fTempPerm266 = 0;
		fTempPerm267 = 0;
		fTempPerm268 = 0;
		fTempPerm269 = 0;
		fTempPerm270 = 0;
		fTempPerm271 = 0;
		fTempPerm272 = 0;
		fTempPerm273 = 0;
		fTempPerm274 = 0;
		fTempPerm275 = 0;
		fTempPerm276 = 0;
		fTempPerm277 = 0;
		fTempPerm278 = 0;
		fTempPerm279 = 0;
		fTempPerm280 = 0;
		fTempPerm281 = 0;
		fTempPerm282 = 0;
		fTempPerm283 = 0;
		fTempPerm284 = 0;
		fTempPerm285 = 0;
		fTempPerm286 = 0;
		fTempPerm287 = 0;
		fTempPerm288 = 0;
		fTempPerm289 = 0;
		fTempPerm290 = 0;
		fTempPerm291 = 0;
		fTempPerm292 = 0;
		fTempPerm293 = 0;
		fTempPerm294 = 0;
		fTempPerm295 = 0;
		fTempPerm296 = 0;
		fTempPerm297 = 0;
		fTempPerm298 = 0;
		fTempPerm299 = 0;
		fTempPerm300 = 0;
		fTempPerm301 = 0;
		fTempPerm302 = 0;
		fTempPerm303 = 0;
		fTempPerm304 = 0;
		fTempPerm305 = 0;
		fTempPerm306 = 0;
		fTempPerm307 = 0;
		fTempPerm308 = 0;
		fTempPerm309 = 0;
		fTempPerm310 = 0;
		fTempPerm311 = 0;
		fTempPerm312 = 0;
		fTempPerm313 = 0;
		fTempPerm314 = 0;
		fTempPerm315 = 0;
		fTempPerm316 = 0;
		fTempPerm317 = 0;
		fTempPerm318 = 0;
		fTempPerm319 = 0;
		fTempPerm320 = 0;
		fTempPerm321 = 0;
		fTempPerm322 = 0;
		fTempPerm323 = 0;
		fTempPerm324 = 0;
		fTempPerm325 = 0;
		fTempPerm326 = 0;
		fTempPerm327 = 0;
		fTempPerm328 = 0;
		fTempPerm329 = 0;
		fTempPerm330 = 0;
		fTempPerm331 = 0;
		fTempPerm332 = 0;
		fTempPerm333 = 0;
		fTempPerm334 = 0;
		fTempPerm335 = 0;
		fTempPerm336 = 0;
		fTempPerm337 = 0;
		fTempPerm338 = 0;
		fTempPerm339 = 0;
		fTempPerm340 = 0;
		fTempPerm341 = 0;
		fTempPerm342 = 0;
		fTempPerm343 = 0;
		fTempPerm344 = 0;
		fTempPerm345 = 0;
		fTempPerm346 = 0;
		fTempPerm347 = 0;
		fTempPerm348 = 0;
		fTempPerm349 = 0;
		fTempPerm350 = 0;
		fTempPerm351 = 0;
		fTempPerm352 = 0;
		fTempPerm353 = 0;
		fTempPerm354 = 0;
		fTempPerm355 = 0;
		fTempPerm356 = 0;
		fTempPerm357 = 0;
		fTempPerm358 = 0;
		fTempPerm359 = 0;
		fTempPerm360 = 0;
		fTempPerm361 = 0;
		fTempPerm362 = 0;
		fTempPerm363 = 0;
		fTempPerm364 = 0;
		fTempPerm365 = 0;
		fTempPerm366 = 0;
		fTempPerm367 = 0;
		fTempPerm368 = 0;
		fTempPerm369 = 0;
		fTempPerm370 = 0;
		fTempPerm371 = 0;
		fTempPerm372 = 0;
		fTempPerm373 = 0;
		fTempPerm374 = 0;
		fTempPerm375 = 0;
		fTempPerm376 = 0;
		fTempPerm377 = 0;
		fTempPerm378 = 0;
		fTempPerm379 = 0;
		fTempPerm380 = 0;
		fTempPerm381 = 0;
		fTempPerm382 = 0;
		fTempPerm383 = 0;
		fTempPerm384 = 0;
		fTempPerm385 = 0;
		fTempPerm386 = 0;
		fTempPerm387 = 0;
		fTempPerm388 = 0;
		fTempPerm389 = 0;
		fTempPerm390 = 0;
		fTempPerm391 = 0;
		fTempPerm392 = 0;
		fTempPerm393 = 0;
		fTempPerm394 = 0;
		fTempPerm395 = 0;
		fTempPerm396 = 0;
		fTempPerm397 = 0;
		fTempPerm398 = 0;
		fTempPerm399 = 0;
		fTempPerm400 = 0;
		fTempPerm401 = 0;
		fTempPerm402 = 0;
		fTempPerm403 = 0;
		fTempPerm404 = 0;
		fTempPerm405 = 0;
		fTempPerm406 = 0;
		fTempPerm407 = 0;
		fTempPerm408 = 0;
		fTempPerm409 = 0;
		fTempPerm410 = 0;
		fTempPerm411 = 0;
		fTempPerm412 = 0;
		fTempPerm413 = 0;
		fTempPerm414 = 0;
		fTempPerm415 = 0;
		fTempPerm416 = 0;
		fTempPerm417 = 0;
		fTempPerm418 = 0;
		fTempPerm419 = 0;
		fTempPerm420 = 0;
		fTempPerm421 = 0;
		fTempPerm422 = 0;
		fTempPerm423 = 0;
		fTempPerm424 = 0;
		fTempPerm425 = 0;
		fTempPerm426 = 0;
		fTempPerm427 = 0;
		fTempPerm428 = 0;
		fTempPerm429 = 0;
		fTempPerm430 = 0;
		fTempPerm431 = 0;
		fTempPerm432 = 0;
		fTempPerm433 = 0;
		fTempPerm434 = 0;
		fTempPerm435 = 0;
		fTempPerm436 = 0;
		fTempPerm437 = 0;
		fTempPerm438 = 0;
		fTempPerm439 = 0;
		fTempPerm440 = 0;
		fTempPerm441 = 0;
		fTempPerm442 = 0;
		fTempPerm443 = 0;
		fTempPerm444 = 0;
		fTempPerm445 = 0;
		fTempPerm446 = 0;
		fTempPerm447 = 0;
		fTempPerm448 = 0;
		fTempPerm449 = 0;
		fTempPerm450 = 0;
		fTempPerm451 = 0;
		fTempPerm452 = 0;
		fTempPerm453 = 0;
		fTempPerm454 = 0;
		fTempPerm455 = 0;
		fTempPerm456 = 0;
		fTempPerm457 = 0;
		fTempPerm458 = 0;
		fTempPerm459 = 0;
		fTempPerm460 = 0;
		fTempPerm461 = 0;
		fTempPerm462 = 0;
		fTempPerm463 = 0;
		fTempPerm464 = 0;
		fTempPerm465 = 0;
		fTempPerm466 = 0;
		fTempPerm467 = 0;
		fTempPerm468 = 0;
		fTempPerm469 = 0;
		fTempPerm470 = 0;
		fTempPerm471 = 0;
		fTempPerm472 = 0;
		fTempPerm473 = 0;
		fTempPerm474 = 0;
		fTempPerm475 = 0;
		fTempPerm476 = 0;
		fTempPerm477 = 0;
		fTempPerm478 = 0;
		fTempPerm479 = 0;
		fTempPerm480 = 0;
		fTempPerm481 = 0;
		fTempPerm482 = 0;
		fTempPerm483 = 0;
		fTempPerm484 = 0;
		fTempPerm485 = 0;
		fTempPerm486 = 0;
		fTempPerm487 = 0;
		fTempPerm488 = 0;
		fTempPerm489 = 0;
		fTempPerm490 = 0;
		fTempPerm491 = 0;
		fTempPerm492 = 0;
		fTempPerm493 = 0;
		fTempPerm494 = 0;
		fTempPerm495 = 0;
		fTempPerm496 = 0;
		fTempPerm497 = 0;
		fTempPerm498 = 0;
		fTempPerm499 = 0;
		fTempPerm500 = 0;
		fTempPerm501 = 0;
		fTempPerm502 = 0;
		fTempPerm503 = 0;
		fTempPerm504 = 0;
		fTempPerm505 = 0;
		fTempPerm506 = 0;
		fTempPerm507 = 0;
		fTempPerm508 = 0;
		fTempPerm509 = 0;
		fTempPerm510 = 0;
		fTempPerm511 = 0;
		fTempPerm512 = 0;
		fTempPerm513 = 0;
		fTempPerm514 = 0;
		fTempPerm515 = 0;
		fTempPerm516 = 0;
		fTempPerm517 = 0;
		fTempPerm518 = 0;
		fTempPerm519 = 0;
		fTempPerm520 = 0;
		fTempPerm521 = 0;
		fTempPerm522 = 0;
		fTempPerm523 = 0;
		fTempPerm524 = 0;
		fTempPerm525 = 0;
		fTempPerm526 = 0;
		fTempPerm527 = 0;
		fTempPerm528 = 0;
		fTempPerm529 = 0;
		fTempPerm530 = 0;
		fTempPerm531 = 0;
		fTempPerm532 = 0;
		fTempPerm533 = 0;
		fTempPerm534 = 0;
		fTempPerm535 = 0;
		fTempPerm536 = 0;
		fTempPerm537 = 0;
		fTempPerm538 = 0;
		fTempPerm539 = 0;
		fTempPerm540 = 0;
		fTempPerm541 = 0;
		fTempPerm542 = 0;
		fTempPerm543 = 0;
		fTempPerm544 = 0;
		fTempPerm545 = 0;
		fTempPerm546 = 0;
		fTempPerm547 = 0;
		fTempPerm548 = 0;
		fTempPerm549 = 0;
		fTempPerm550 = 0;
		fTempPerm551 = 0;
		fTempPerm552 = 0;
		fTempPerm553 = 0;
		fTempPerm554 = 0;
		fTempPerm555 = 0;
		fTempPerm556 = 0;
		fTempPerm557 = 0;
		fTempPerm558 = 0;
		fTempPerm559 = 0;
		fTempPerm560 = 0;
		fTempPerm561 = 0;
		fTempPerm562 = 0;
		fTempPerm563 = 0;
		fTempPerm564 = 0;
		fTempPerm565 = 0;
		fTempPerm566 = 0;
		fTempPerm567 = 0;
		fTempPerm568 = 0;
		fTempPerm569 = 0;
		fTempPerm570 = 0;
		fTempPerm571 = 0;
		fTempPerm572 = 0;
		fTempPerm573 = 0;
		fTempPerm574 = 0;
		fTempPerm575 = 0;
		fTempPerm576 = 0;
		fTempPerm577 = 0;
		fTempPerm578 = 0;
		fTempPerm579 = 0;
		fTempPerm580 = 0;
		fTempPerm581 = 0;
		fTempPerm582 = 0;
		fTempPerm583 = 0;
		fTempPerm584 = 0;
		fTempPerm585 = 0;
		fTempPerm586 = 0;
		fTempPerm587 = 0;
		fTempPerm588 = 0;
		fTempPerm589 = 0;
		fTempPerm590 = 0;
		fTempPerm591 = 0;
		fTempPerm592 = 0;
		fTempPerm593 = 0;
		fTempPerm594 = 0;
		fTempPerm595 = 0;
		fTempPerm596 = 0;
		fTempPerm597 = 0;
		fTempPerm598 = 0;
		fTempPerm599 = 0;
		fTempPerm600 = 0;
		fTempPerm601 = 0;
		fTempPerm602 = 0;
		fTempPerm603 = 0;
		fTempPerm604 = 0;
		fTempPerm605 = 0;
		fTempPerm606 = 0;
		fTempPerm607 = 0;
		fTempPerm608 = 0;
		fTempPerm609 = 0;
		fTempPerm610 = 0;
		fTempPerm611 = 0;
		fTempPerm612 = 0;
		fTempPerm613 = 0;
		fTempPerm614 = 0;
		fTempPerm615 = 0;
		fTempPerm616 = 0;
		fTempPerm617 = 0;
		fTempPerm618 = 0;
		fTempPerm619 = 0;
		fTempPerm620 = 0;
		fTempPerm621 = 0;
		fTempPerm622 = 0;
		fTempPerm623 = 0;
		fTempPerm624 = 0;
		fTempPerm625 = 0;
		fTempPerm626 = 0;
		fTempPerm627 = 0;
		fTempPerm628 = 0;
		fTempPerm629 = 0;
		fTempPerm630 = 0;
		fTempPerm631 = 0;
		fTempPerm632 = 0;
		fTempPerm633 = 0;
		fTempPerm634 = 0;
		fTempPerm635 = 0;
		fTempPerm636 = 0;
		fTempPerm637 = 0;
		fTempPerm638 = 0;
		fTempPerm639 = 0;
		fTempPerm640 = 0;
		fTempPerm641 = 0;
		fTempPerm642 = 0;
		fTempPerm643 = 0;
		fTempPerm644 = 0;
		fTempPerm645 = 0;
		fTempPerm646 = 0;
		fTempPerm647 = 0;
		fTempPerm648 = 0;
		fTempPerm649 = 0;
		fTempPerm650 = 0;
		fTempPerm651 = 0;
		fTempPerm652 = 0;
		fTempPerm653 = 0;
		fTempPerm654 = 0;
		fTempPerm655 = 0;
		fTempPerm656 = 0;
		fTempPerm657 = 0;
		fTempPerm658 = 0;
		fTempPerm659 = 0;
		fTempPerm660 = 0;
		fTempPerm661 = 0;
		fTempPerm662 = 0;
		fTempPerm663 = 0;
		fTempPerm664 = 0;
		fTempPerm665 = 0;
		fTempPerm666 = 0;
		fTempPerm667 = 0;
		fTempPerm668 = 0;
		fTempPerm669 = 0;
		fTempPerm670 = 0;
		fTempPerm671 = 0;
		fTempPerm672 = 0;
		fTempPerm673 = 0;
		fTempPerm674 = 0;
		fTempPerm675 = 0;
		fTempPerm676 = 0;
		fTempPerm677 = 0;
		fTempPerm678 = 0;
		fTempPerm679 = 0;
		fTempPerm680 = 0;
		fTempPerm681 = 0;
		fTempPerm682 = 0;
		fTempPerm683 = 0;
		fTempPerm684 = 0;
		fTempPerm685 = 0;
		fTempPerm686 = 0;
		fTempPerm687 = 0;
		fTempPerm688 = 0;
		fTempPerm689 = 0;
		fTempPerm690 = 0;
		fTempPerm691 = 0;
		fTempPerm692 = 0;
		fTempPerm693 = 0;
		fTempPerm694 = 0;
		fTempPerm695 = 0;
		fTempPerm696 = 0;
		fTempPerm697 = 0;
		fTempPerm698 = 0;
		fTempPerm699 = 0;
		fTempPerm700 = 0;
		fTempPerm701 = 0;
		fTempPerm702 = 0;
		fTempPerm703 = 0;
		fTempPerm704 = 0;
		fTempPerm705 = 0;
		fTempPerm706 = 0;
		fTempPerm707 = 0;
		fTempPerm708 = 0;
		fTempPerm709 = 0;
		fTempPerm710 = 0;
		fTempPerm711 = 0;
		fTempPerm712 = 0;
		fTempPerm713 = 0;
		fTempPerm714 = 0;
		fTempPerm715 = 0;
		fTempPerm716 = 0;
		fTempPerm717 = 0;
		fTempPerm718 = 0;
		fTempPerm719 = 0;
		fTempPerm720 = 0;
		fTempPerm721 = 0;
		fTempPerm722 = 0;
		fTempPerm723 = 0;
		fTempPerm724 = 0;
		fTempPerm725 = 0;
		fTempPerm726 = 0;
		fTempPerm727 = 0;
		fTempPerm728 = 0;
		fTempPerm729 = 0;
		fTempPerm730 = 0;
		fTempPerm731 = 0;
		fTempPerm732 = 0;
		fTempPerm733 = 0;
		fTempPerm734 = 0;
		fTempPerm735 = 0;
		fTempPerm736 = 0;
		fTempPerm737 = 0;
		fTempPerm738 = 0;
		fTempPerm739 = 0;
		fTempPerm740 = 0;
		fTempPerm741 = 0;
		fTempPerm742 = 0;
		fTempPerm743 = 0;
		fTempPerm744 = 0;
		fTempPerm745 = 0;
		fTempPerm746 = 0;
		fTempPerm747 = 0;
		fTempPerm748 = 0;
		fTempPerm749 = 0;
		fTempPerm750 = 0;
		fTempPerm751 = 0;
		fTempPerm752 = 0;
		fTempPerm753 = 0;
		fTempPerm754 = 0;
		fTempPerm755 = 0;
		fTempPerm756 = 0;
		fTempPerm757 = 0;
		fTempPerm758 = 0;
		fTempPerm759 = 0;
		fTempPerm760 = 0;
		fTempPerm761 = 0;
		fTempPerm762 = 0;
		fTempPerm763 = 0;
		fTempPerm764 = 0;
		fTempPerm765 = 0;
		fTempPerm766 = 0;
		fTempPerm767 = 0;
		fTempPerm768 = 0;
		fTempPerm769 = 0;
		fTempPerm770 = 0;
		fTempPerm771 = 0;
		fTempPerm772 = 0;
		fTempPerm773 = 0;
		fTempPerm774 = 0;
		fTempPerm775 = 0;
		fTempPerm776 = 0;
		fTempPerm777 = 0;
		fTempPerm778 = 0;
		fTempPerm779 = 0;
		fTempPerm780 = 0;
		fTempPerm781 = 0;
		fTempPerm782 = 0;
		fTempPerm783 = 0;
		fTempPerm784 = 0;
		fTempPerm785 = 0;
		fTempPerm786 = 0;
		fTempPerm787 = 0;
		fTempPerm788 = 0;
		fTempPerm789 = 0;
		fTempPerm790 = 0;
		fTempPerm791 = 0;
		fTempPerm792 = 0;
		fTempPerm793 = 0;
		fTempPerm794 = 0;
		fTempPerm795 = 0;
		fTempPerm796 = 0;
		fTempPerm797 = 0;
		fTempPerm798 = 0;
		fTempPerm799 = 0;
		fTempPerm800 = 0;
		fTempPerm801 = 0;
		fTempPerm802 = 0;
		fTempPerm803 = 0;
		fTempPerm804 = 0;
		fTempPerm805 = 0;
		fTempPerm806 = 0;
		fTempPerm807 = 0;
		fTempPerm808 = 0;
		fTempPerm809 = 0;
		fTempPerm810 = 0;
		fTempPerm811 = 0;
		fTempPerm812 = 0;
		fTempPerm813 = 0;
		fTempPerm814 = 0;
		fTempPerm815 = 0;
		fTempPerm816 = 0;
		fTempPerm817 = 0;
		fTempPerm818 = 0;
		fTempPerm819 = 0;
		fTempPerm820 = 0;
		fTempPerm821 = 0;
		fTempPerm822 = 0;
		fTempPerm823 = 0;
		fTempPerm824 = 0;
		fTempPerm825 = 0;
		fTempPerm826 = 0;
		fTempPerm827 = 0;
		fTempPerm828 = 0;
		fTempPerm829 = 0;
		fTempPerm830 = 0;
		fTempPerm831 = 0;
		fTempPerm832 = 0;
		fTempPerm833 = 0;
		fTempPerm834 = 0;
		fTempPerm835 = 0;
		fTempPerm836 = 0;
		fTempPerm837 = 0;
		fTempPerm838 = 0;
		fTempPerm839 = 0;
		fTempPerm840 = 0;
		fTempPerm841 = 0;
		fTempPerm842 = 0;
		fTempPerm843 = 0;
		fTempPerm844 = 0;
		fTempPerm845 = 0;
		fTempPerm846 = 0;
		fTempPerm847 = 0;
		fTempPerm848 = 0;
		fTempPerm849 = 0;
		fTempPerm850 = 0;
		fTempPerm851 = 0;
		fTempPerm852 = 0;
		fTempPerm853 = 0;
		fTempPerm854 = 0;
		fTempPerm855 = 0;
		fTempPerm856 = 0;
		fTempPerm857 = 0;
		fTempPerm858 = 0;
		fTempPerm859 = 0;
		fTempPerm860 = 0;
		fTempPerm861 = 0;
		fTempPerm862 = 0;
		fTempPerm863 = 0;
		fTempPerm864 = 0;
		fTempPerm865 = 0;
		fTempPerm866 = 0;
		fTempPerm867 = 0;
		fTempPerm868 = 0;
		fTempPerm869 = 0;
		fTempPerm870 = 0;
		fTempPerm871 = 0;
		fTempPerm872 = 0;
		fTempPerm873 = 0;
		fTempPerm874 = 0;
		fTempPerm875 = 0;
		fTempPerm876 = 0;
		fTempPerm877 = 0;
		fTempPerm878 = 0;
		fTempPerm879 = 0;
		fTempPerm880 = 0;
		fTempPerm881 = 0;
		fTempPerm882 = 0;
		fTempPerm883 = 0;
		fTempPerm884 = 0;
		fTempPerm885 = 0;
		fTempPerm886 = 0;
		fTempPerm887 = 0;
		fTempPerm888 = 0;
		fTempPerm889 = 0;
		fTempPerm890 = 0;
		fTempPerm891 = 0;
		fTempPerm892 = 0;
		fTempPerm893 = 0;
		fTempPerm894 = 0;
		fTempPerm895 = 0;
		fTempPerm896 = 0;
		fTempPerm897 = 0;
		fTempPerm898 = 0;
		fTempPerm899 = 0;
		fTempPerm900 = 0;
		fTempPerm901 = 0;
		fTempPerm902 = 0;
		fTempPerm903 = 0;
		fTempPerm904 = 0;
		fTempPerm905 = 0;
		fTempPerm906 = 0;
		fTempPerm907 = 0;
		fTempPerm908 = 0;
		fTempPerm909 = 0;
		fTempPerm910 = 0;
		fTempPerm911 = 0;
		fTempPerm912 = 0;
		fTempPerm913 = 0;
		fTempPerm914 = 0;
		fTempPerm915 = 0;
		fTempPerm916 = 0;
		fTempPerm917 = 0;
		fTempPerm918 = 0;
		fTempPerm919 = 0;
		fTempPerm920 = 0;
		fTempPerm921 = 0;
		fTempPerm922 = 0;
		fTempPerm923 = 0;
		fTempPerm924 = 0;
		fTempPerm925 = 0;
		fTempPerm926 = 0;
		fTempPerm927 = 0;
		fTempPerm928 = 0;
		fTempPerm929 = 0;
		fTempPerm930 = 0;
		fTempPerm931 = 0;
		fTempPerm932 = 0;
		fTempPerm933 = 0;
		fTempPerm934 = 0;
		fTempPerm935 = 0;
		fTempPerm936 = 0;
		fTempPerm937 = 0;
		fTempPerm938 = 0;
		fTempPerm939 = 0;
		fTempPerm940 = 0;
		fTempPerm941 = 0;
		fTempPerm942 = 0;
		fTempPerm943 = 0;
		fTempPerm944 = 0;
		fTempPerm945 = 0;
		fTempPerm946 = 0;
		fTempPerm947 = 0;
		fTempPerm948 = 0;
		fTempPerm949 = 0;
		fTempPerm950 = 0;
		fTempPerm951 = 0;
		fTempPerm952 = 0;
		fTempPerm953 = 0;
		fTempPerm954 = 0;
		fTempPerm955 = 0;
		fTempPerm956 = 0;
		fTempPerm957 = 0;
		fTempPerm958 = 0;
		fTempPerm959 = 0;
		fTempPerm960 = 0;
		fTempPerm961 = 0;
		fTempPerm962 = 0;
		fTempPerm963 = 0;
		fTempPerm964 = 0;
		fTempPerm965 = 0;
		fTempPerm966 = 0;
		fTempPerm967 = 0;
		fTempPerm968 = 0;
		fTempPerm969 = 0;
		fTempPerm970 = 0;
		fTempPerm971 = 0;
		fTempPerm972 = 0;
		fTempPerm973 = 0;
		fTempPerm974 = 0;
		fTempPerm975 = 0;
		fTempPerm976 = 0;
		fTempPerm977 = 0;
		fTempPerm978 = 0;
		fTempPerm979 = 0;
		fTempPerm980 = 0;
		fTempPerm981 = 0;
		fTempPerm982 = 0;
		fTempPerm983 = 0;
		fTempPerm984 = 0;
		fTempPerm985 = 0;
		fTempPerm986 = 0;
		fTempPerm987 = 0;
		fTempPerm988 = 0;
		fTempPerm989 = 0;
		fTempPerm990 = 0;
		fTempPerm991 = 0;
		fTempPerm992 = 0;
		fTempPerm993 = 0;
		fTempPerm994 = 0;
		fTempPerm995 = 0;
		fTempPerm996 = 0;
		fTempPerm997 = 0;
		fTempPerm998 = 0;
		fTempPerm999 = 0;
		fTempPerm1000 = 0;
		fTempPerm1001 = 0;
		fTempPerm1002 = 0;
		fTempPerm1003 = 0;
		fTempPerm1004 = 0;
		fTempPerm1005 = 0;
		fTempPerm1006 = 0;
		fTempPerm1007 = 0;
		fTempPerm1008 = 0;
		fTempPerm1009 = 0;
		fTempPerm1010 = 0;
		fTempPerm1011 = 0;
		fTempPerm1012 = 0;
		fTempPerm1013 = 0;
		fTempPerm1014 = 0;
		fTempPerm1015 = 0;
		fTempPerm1016 = 0;
		fTempPerm1017 = 0;
		fTempPerm1018 = 0;
		fTempPerm1019 = 0;
		fTempPerm1020 = 0;
		fTempPerm1021 = 0;
		fTempPerm1022 = 0;
		fTempPerm1023 = 0;
		fTempPerm1024 = 0;
		fTempPerm1025 = 0;
		fTempPerm1026 = 0;
		fTempPerm1027 = 0;
		fTempPerm1028 = 0;
		fTempPerm1029 = 0;
		fTempPerm1030 = 0;
		fTempPerm1031 = 0;
		fTempPerm1032 = 0;
		fTempPerm1033 = 0;
		fTempPerm1034 = 0;
		fTempPerm1035 = 0;
		fTempPerm1036 = 0;
		fTempPerm1037 = 0;
		fTempPerm1038 = 0;
		fTempPerm1039 = 0;
		fTempPerm1040 = 0;
		fTempPerm1041 = 0;
		fTempPerm1042 = 0;
		fTempPerm1043 = 0;
		fTempPerm1044 = 0;
		fTempPerm1045 = 0;
		fTempPerm1046 = 0;
		fTempPerm1047 = 0;
		fTempPerm1048 = 0;
		fTempPerm1049 = 0;
		fTempPerm1050 = 0;
		fTempPerm1051 = 0;
		fTempPerm1052 = 0;
		fTempPerm1053 = 0;
		fTempPerm1054 = 0;
		fTempPerm1055 = 0;
		fTempPerm1056 = 0;
		fTempPerm1057 = 0;
		fTempPerm1058 = 0;
		fTempPerm1059 = 0;
		fTempPerm1060 = 0;
		fTempPerm1061 = 0;
		fTempPerm1062 = 0;
		fTempPerm1063 = 0;
		fTempPerm1064 = 0;
		fTempPerm1065 = 0;
		fTempPerm1066 = 0;
		fTempPerm1067 = 0;
		fTempPerm1068 = 0;
		fTempPerm1069 = 0;
		fTempPerm1070 = 0;
		fTempPerm1071 = 0;
		fTempPerm1072 = 0;
		fTempPerm1073 = 0;
		fTempPerm1074 = 0;
		fTempPerm1075 = 0;
		fTempPerm1076 = 0;
		fTempPerm1077 = 0;
		fTempPerm1078 = 0;
		fTempPerm1079 = 0;
		fTempPerm1080 = 0;
		fTempPerm1081 = 0;
		fTempPerm1082 = 0;
		fTempPerm1083 = 0;
		fTempPerm1084 = 0;
		fTempPerm1085 = 0;
		fTempPerm1086 = 0;
		fTempPerm1087 = 0;
		fTempPerm1088 = 0;
		fTempPerm1089 = 0;
		fTempPerm1090 = 0;
		fTempPerm1091 = 0;
		fTempPerm1092 = 0;
		fTempPerm1093 = 0;
		fTempPerm1094 = 0;
		fTempPerm1095 = 0;
		fTempPerm1096 = 0;
		fTempPerm1097 = 0;
		fTempPerm1098 = 0;
		fTempPerm1099 = 0;
		fTempPerm1100 = 0;
		fTempPerm1101 = 0;
		fTempPerm1102 = 0;
		fTempPerm1103 = 0;
		fTempPerm1104 = 0;
		fTempPerm1105 = 0;
		fTempPerm1106 = 0;
		fTempPerm1107 = 0;
		fTempPerm1108 = 0;
		fTempPerm1109 = 0;
		fTempPerm1110 = 0;
		fTempPerm1111 = 0;
		fTempPerm1112 = 0;
		fTempPerm1113 = 0;
		fTempPerm1114 = 0;
		fTempPerm1115 = 0;
		fTempPerm1116 = 0;
		fTempPerm1117 = 0;
		fTempPerm1118 = 0;
		fTempPerm1119 = 0;
		fTempPerm1120 = 0;
		fTempPerm1121 = 0;
		fTempPerm1122 = 0;
		fTempPerm1123 = 0;
		fTempPerm1124 = 0;
		fTempPerm1125 = 0;
		fTempPerm1126 = 0;
		fTempPerm1127 = 0;
		fTempPerm1128 = 0;
		fTempPerm1129 = 0;
		fTempPerm1130 = 0;
		fTempPerm1131 = 0;
		fTempPerm1132 = 0;
		fTempPerm1133 = 0;
		fTempPerm1134 = 0;
		fTempPerm1135 = 0;
		fTempPerm1136 = 0;
		fTempPerm1137 = 0;
		fTempPerm1138 = 0;
		fTempPerm1139 = 0;
		fTempPerm1140 = 0;
		fTempPerm1141 = 0;
		fTempPerm1142 = 0;
		fTempPerm1143 = 0;
		fTempPerm1144 = 0;
		fTempPerm1145 = 0;
		fTempPerm1146 = 0;
		fTempPerm1147 = 0;
		fTempPerm1148 = 0;
		fTempPerm1149 = 0;
		fTempPerm1150 = 0;
		fTempPerm1151 = 0;
		fTempPerm1152 = 0;
		fTempPerm1153 = 0;
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.0;
		fslider1 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		IOTA = 0;
		for (int i=0; i<128; i++) fVec0[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<128; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<128; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<128; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<128; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<128; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<128; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<128; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<128; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
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
		ui_interface->openVerticalBox("HOADecoderLebedev50Binaural2");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa3d5f11610", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa3d6145be0", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa3d6140b30", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa3d613bd70", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa3d6138770", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa3d5e1e0e0", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa3d5e1a1b0", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa3d5e14490", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fa3d5f63c50", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(&fslider1, "1", "");
		ui_interface->declare(&fslider1, "osc", "/levelin -10 10");
		ui_interface->declare(&fslider1, "unit", "dB");
		ui_interface->addVerticalSlider("Inputs Gain", &fslider1, 0.0, -1e+01, 1e+01, 0.1);
		ui_interface->declare(&fslider0, "2", "");
		ui_interface->declare(&fslider0, "osc", "/levelout -10 10");
		ui_interface->declare(&fslider0, "unit", "dB");
		ui_interface->addVerticalSlider("Outputs Gain", &fslider0, 0.0, -1e+01, 1e+01, 0.1);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("Outputs");
		ui_interface->openHorizontalBox("Left");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addHorizontalBargraph("0x7fa3d5e46dc0", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Right");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addHorizontalBargraph("0x7fa3d6190190", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		//zone1
		//zone2
		double 	fSlow0 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider0))));
		double 	fSlow1 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider1))));
		//zone2b
		//zone3
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* input2 = input[2];
		FAUSTFLOAT* input3 = input[3];
		FAUSTFLOAT* input4 = input[4];
		FAUSTFLOAT* input5 = input[5];
		FAUSTFLOAT* input6 = input[6];
		FAUSTFLOAT* input7 = input[7];
		FAUSTFLOAT* input8 = input[8];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		//LoopGraphScalar
		for (int i=0; i<count; i++) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			fRec3[0] = (fSlow1 + (0.999 * fRec3[1]));
			fTempPerm0 = ((double)input8[i] * fRec3[0]);
			fRec2[0] = max((fRec2[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm0))))));
			fbargraph0 = fRec2[0];
			fTempPerm1 = fTempPerm0;
			fVec0[IOTA&127] = fTempPerm1;
			fTempPerm2 = fVec0[(IOTA-103)&127];
			fTempPerm3 = fVec0[(IOTA-120)&127];
			fTempPerm4 = fVec0[(IOTA-121)&127];
			fTempPerm5 = fVec0[(IOTA-96)&127];
			fTempPerm6 = fVec0[(IOTA-91)&127];
			fTempPerm7 = fVec0[(IOTA-95)&127];
			fTempPerm8 = fVec0[(IOTA-67)&127];
			fTempPerm9 = fVec0[(IOTA-66)&127];
			fTempPerm10 = fVec0[(IOTA-34)&127];
			fTempPerm11 = fVec0[(IOTA-33)&127];
			fTempPerm12 = ((double)input7[i] * fRec3[0]);
			fRec4[0] = max((fRec4[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm12))))));
			fbargraph1 = fRec4[0];
			fTempPerm13 = fTempPerm12;
			fVec1[IOTA&127] = fTempPerm13;
			fTempPerm14 = fVec1[(IOTA-125)&127];
			fTempPerm15 = fVec1[(IOTA-111)&127];
			fTempPerm16 = fVec1[(IOTA-110)&127];
			fTempPerm17 = fVec1[(IOTA-102)&127];
			fTempPerm18 = fVec1[(IOTA-97)&127];
			fTempPerm19 = fVec1[(IOTA-94)&127];
			fTempPerm20 = fVec1[(IOTA-85)&127];
			fTempPerm21 = fVec1[(IOTA-77)&127];
			fTempPerm22 = fVec1[(IOTA-63)&127];
			fTempPerm23 = fVec1[(IOTA-57)&127];
			fTempPerm24 = fVec1[(IOTA-32)&127];
			fTempPerm25 = fVec1[(IOTA-29)&127];
			fTempPerm26 = fVec1[(IOTA-31)&127];
			fTempPerm27 = fVec1[(IOTA-18)&127];
			fTempPerm28 = fVec1[(IOTA-15)&127];
			fTempPerm29 = fVec1[(IOTA-103)&127];
			fTempPerm30 = fVec1[(IOTA-98)&127];
			fTempPerm31 = ((double)input6[i] * fRec3[0]);
			fRec5[0] = max((fRec5[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm31))))));
			fbargraph2 = fRec5[0];
			fTempPerm32 = fTempPerm31;
			fVec2[IOTA&127] = fTempPerm32;
			fTempPerm33 = fVec2[(IOTA-118)&127];
			fTempPerm34 = fVec2[(IOTA-108)&127];
			fTempPerm35 = fVec2[(IOTA-94)&127];
			fTempPerm36 = fVec2[(IOTA-91)&127];
			fTempPerm37 = fVec2[(IOTA-90)&127];
			fTempPerm38 = fVec2[(IOTA-87)&127];
			fTempPerm39 = fVec2[(IOTA-100)&127];
			fTempPerm40 = fVec2[(IOTA-98)&127];
			fTempPerm41 = fVec2[(IOTA-84)&127];
			fTempPerm42 = fVec2[(IOTA-75)&127];
			fTempPerm43 = ((double)input5[i] * fRec3[0]);
			fRec6[0] = max((fRec6[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm43))))));
			fbargraph3 = fRec6[0];
			fTempPerm44 = fTempPerm43;
			fVec3[IOTA&127] = fTempPerm44;
			fTempPerm45 = fVec3[(IOTA-125)&127];
			fTempPerm46 = fVec3[(IOTA-124)&127];
			fTempPerm47 = fVec3[(IOTA-108)&127];
			fTempPerm48 = fVec3[(IOTA-107)&127];
			fTempPerm49 = fVec2[(IOTA-52)&127];
			fTempPerm50 = fVec3[(IOTA-104)&127];
			fTempPerm51 = fVec3[(IOTA-101)&127];
			fTempPerm52 = fVec3[(IOTA-92)&127];
			fTempPerm53 = fVec3[(IOTA-91)&127];
			fTempPerm54 = fVec3[(IOTA-86)&127];
			fTempPerm55 = fVec3[(IOTA-71)&127];
			fTempPerm56 = fVec3[(IOTA-60)&127];
			fTempPerm57 = fVec3[(IOTA-52)&127];
			fTempPerm58 = fVec3[(IOTA-51)&127];
			fTempPerm59 = fVec3[(IOTA-45)&127];
			fTempPerm60 = fVec3[(IOTA-54)&127];
			fTempPerm61 = fVec3[(IOTA-39)&127];
			fTempPerm62 = fVec3[(IOTA-42)&127];
			fTempPerm63 = fVec3[(IOTA-38)&127];
			fTempPerm64 = fVec0[(IOTA-122)&127];
			fTempPerm65 = fVec0[(IOTA-110)&127];
			fTempPerm66 = fVec0[(IOTA-126)&127];
			fTempPerm67 = fVec0[(IOTA-104)&127];
			fTempPerm68 = fVec0[(IOTA-30)&127];
			fTempPerm69 = fVec0[(IOTA-29)&127];
			fTempPerm70 = fVec0[(IOTA-38)&127];
			fTempPerm71 = fVec0[(IOTA-28)&127];
			fTempPerm72 = fVec0[(IOTA-26)&127];
			fTempPerm73 = fVec0[(IOTA-27)&127];
			fTempPerm74 = fVec0[(IOTA-24)&127];
			fTempPerm75 = fVec0[(IOTA-20)&127];
			fTempPerm76 = fVec0[(IOTA-19)&127];
			fTempPerm77 = fVec0[(IOTA-18)&127];
			fTempPerm78 = fVec0[(IOTA-10)&127];
			fTempPerm79 = fVec0[(IOTA-8)&127];
			fTempPerm80 = fVec0[(IOTA-6)&127];
			fTempPerm81 = fVec0[(IOTA-4)&127];
			fTempPerm82 = fVec0[(IOTA-2)&127];
			fTempPerm83 = fVec1[(IOTA-121)&127];
			fTempPerm84 = fVec1[(IOTA-112)&127];
			fTempPerm85 = fVec1[(IOTA-100)&127];
			fTempPerm86 = fVec1[(IOTA-106)&127];
			fTempPerm87 = fVec1[(IOTA-105)&127];
			fTempPerm88 = fVec1[(IOTA-101)&127];
			fTempPerm89 = fVec1[(IOTA-93)&127];
			fTempPerm90 = fVec1[(IOTA-67)&127];
			fTempPerm91 = fVec1[(IOTA-71)&127];
			fTempPerm92 = fVec1[(IOTA-56)&127];
			fTempPerm93 = fVec1[(IOTA-55)&127];
			fTempPerm94 = fVec1[(IOTA-38)&127];
			fTempPerm95 = fVec1[(IOTA-36)&127];
			fTempPerm96 = fVec1[(IOTA-35)&127];
			fTempPerm97 = fVec2[(IOTA-126)&127];
			fTempPerm98 = fVec2[(IOTA-104)&127];
			fTempPerm99 = fVec2[(IOTA-99)&127];
			fTempPerm100 = fVec2[(IOTA-79)&127];
			fTempPerm101 = fVec2[(IOTA-63)&127];
			fTempPerm102 = fVec2[(IOTA-66)&127];
			fTempPerm103 = fVec2[(IOTA-39)&127];
			fTempPerm104 = fVec2[(IOTA-37)&127];
			fTempPerm105 = fVec2[(IOTA-26)&127];
			fTempPerm106 = fVec2[(IOTA-22)&127];
			fTempPerm107 = fVec3[(IOTA-123)&127];
			fTempPerm108 = fVec3[(IOTA-110)&127];
			fTempPerm109 = fVec3[(IOTA-102)&127];
			fTempPerm110 = fVec3[(IOTA-100)&127];
			fTempPerm111 = fVec3[(IOTA-98)&127];
			fTempPerm112 = fVec3[(IOTA-95)&127];
			fTempPerm113 = fVec3[(IOTA-4)&127];
			fTempPerm114 = fVec3[(IOTA-2)&127];
			fTempPerm115 = ((double)input4[i] * fRec3[0]);
			fRec7[0] = max((fRec7[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm115))))));
			fbargraph4 = fRec7[0];
			fTempPerm116 = fTempPerm115;
			fVec4[IOTA&127] = fTempPerm116;
			fTempPerm117 = fVec4[(IOTA-125)&127];
			fTempPerm118 = fVec4[(IOTA-117)&127];
			fTempPerm119 = fVec4[(IOTA-86)&127];
			fTempPerm120 = fVec4[(IOTA-74)&127];
			fTempPerm121 = fVec4[(IOTA-68)&127];
			fTempPerm122 = fVec4[(IOTA-42)&127];
			fTempPerm123 = fVec4[(IOTA-67)&127];
			fTempPerm124 = fVec4[(IOTA-56)&127];
			fTempPerm125 = ((double)input3[i] * fRec3[0]);
			fRec8[0] = max((fRec8[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm125))))));
			fbargraph5 = fRec8[0];
			fTempPerm126 = fTempPerm125;
			fVec5[IOTA&127] = fTempPerm126;
			fTempPerm127 = fVec5[(IOTA-126)&127];
			fTempPerm128 = fVec5[(IOTA-125)&127];
			fTempPerm129 = fVec5[(IOTA-123)&127];
			fTempPerm130 = fVec5[(IOTA-122)&127];
			fTempPerm131 = fVec5[(IOTA-120)&127];
			fTempPerm132 = fVec5[(IOTA-94)&127];
			fTempPerm133 = fVec5[(IOTA-86)&127];
			fTempPerm134 = fVec5[(IOTA-85)&127];
			fTempPerm135 = fVec5[(IOTA-90)&127];
			fTempPerm136 = fVec5[(IOTA-65)&127];
			fTempPerm137 = fVec5[(IOTA-64)&127];
			fTempPerm138 = fVec5[(IOTA-63)&127];
			fTempPerm139 = fVec5[(IOTA-62)&127];
			fTempPerm140 = fVec5[(IOTA-20)&127];
			fTempPerm141 = fVec5[(IOTA-34)&127];
			fTempPerm142 = ((double)input2[i] * fRec3[0]);
			fRec9[0] = max((fRec9[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm142))))));
			fbargraph6 = fRec9[0];
			fTempPerm143 = fTempPerm142;
			fVec6[IOTA&127] = fTempPerm143;
			fTempPerm144 = fVec6[(IOTA-124)&127];
			fTempPerm145 = fVec6[(IOTA-121)&127];
			fTempPerm146 = fVec6[(IOTA-120)&127];
			fTempPerm147 = fVec6[(IOTA-116)&127];
			fTempPerm148 = fVec6[(IOTA-122)&127];
			fTempPerm149 = fVec6[(IOTA-98)&127];
			fTempPerm150 = fVec5[(IOTA-36)&127];
			fTempPerm151 = fVec5[(IOTA-33)&127];
			fTempPerm152 = fVec5[(IOTA-37)&127];
			fTempPerm153 = fVec6[(IOTA-62)&127];
			fTempPerm154 = fVec6[(IOTA-53)&127];
			fTempPerm155 = fVec6[(IOTA-75)&127];
			fTempPerm156 = fVec6[(IOTA-52)&127];
			fTempPerm157 = fVec6[(IOTA-46)&127];
			fTempPerm158 = fVec6[(IOTA-45)&127];
			fTempPerm159 = fVec6[(IOTA-54)&127];
			fTempPerm160 = ((double)input1[i] * fRec3[0]);
			fRec10[0] = max((fRec10[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm160))))));
			fbargraph7 = fRec10[0];
			fTempPerm161 = fTempPerm160;
			fVec7[IOTA&127] = fTempPerm161;
			fTempPerm162 = fVec7[(IOTA-114)&127];
			fTempPerm163 = fVec7[(IOTA-101)&127];
			fTempPerm164 = fVec7[(IOTA-99)&127];
			fTempPerm165 = fVec7[(IOTA-98)&127];
			fTempPerm166 = fVec7[(IOTA-97)&127];
			fTempPerm167 = fVec0[(IOTA-86)&127];
			fTempPerm168 = fVec0[(IOTA-79)&127];
			fTempPerm169 = fVec0[(IOTA-76)&127];
			fTempPerm170 = fVec0[(IOTA-72)&127];
			fTempPerm171 = fVec0[(IOTA-71)&127];
			fTempPerm172 = fVec0[(IOTA-65)&127];
			fTempPerm173 = fVec0[(IOTA-64)&127];
			fTempPerm174 = fVec0[(IOTA-69)&127];
			fTempPerm175 = fVec0[(IOTA-68)&127];
			fTempPerm176 = fVec0[(IOTA-23)&127];
			fTempPerm177 = fVec1[(IOTA-126)&127];
			fTempPerm178 = fVec1[(IOTA-116)&127];
			fTempPerm179 = fVec1[(IOTA-104)&127];
			fTempPerm180 = fVec1[(IOTA-99)&127];
			fTempPerm181 = fVec1[(IOTA-107)&127];
			fTempPerm182 = fVec1[(IOTA-68)&127];
			fTempPerm183 = fVec1[(IOTA-70)&127];
			fTempPerm184 = fVec1[(IOTA-59)&127];
			fTempPerm185 = fVec1[(IOTA-58)&127];
			fTempPerm186 = fVec1[(IOTA-54)&127];
			fTempPerm187 = fVec1[(IOTA-25)&127];
			fTempPerm188 = fVec1[(IOTA-44)&127];
			fTempPerm189 = fVec1[(IOTA-19)&127];
			fTempPerm190 = fVec1[(IOTA-12)&127];
			fTempPerm191 = fVec1[(IOTA-26)&127];
			fTempPerm192 = fVec2[(IOTA-107)&127];
			fTempPerm193 = fVec2[(IOTA-106)&127];
			fTempPerm194 = fVec2[(IOTA-88)&127];
			fTempPerm195 = fVec2[(IOTA-72)&127];
			fTempPerm196 = fVec2[(IOTA-71)&127];
			fTempPerm197 = fVec2[(IOTA-65)&127];
			fTempPerm198 = fVec2[(IOTA-64)&127];
			fTempPerm199 = fVec2[(IOTA-33)&127];
			fTempPerm200 = fVec2[(IOTA-32)&127];
			fTempPerm201 = fVec2[(IOTA-30)&127];
			fTempPerm202 = fVec2[(IOTA-29)&127];
			fTempPerm203 = fVec2[(IOTA-28)&127];
			fTempPerm204 = fVec3[(IOTA-119)&127];
			fTempPerm205 = fVec2[(IOTA-36)&127];
			fTempPerm206 = fVec3[(IOTA-115)&127];
			fTempPerm207 = fVec3[(IOTA-106)&127];
			fTempPerm208 = fVec3[(IOTA-105)&127];
			fTempPerm209 = fVec3[(IOTA-113)&127];
			fTempPerm210 = fVec3[(IOTA-99)&127];
			fTempPerm211 = fVec3[(IOTA-97)&127];
			fTempPerm212 = fVec3[(IOTA-96)&127];
			fTempPerm213 = fVec3[(IOTA-90)&127];
			fTempPerm214 = fVec3[(IOTA-89)&127];
			fTempPerm215 = fVec3[(IOTA-85)&127];
			fTempPerm216 = fVec3[(IOTA-94)&127];
			fTempPerm217 = fVec3[(IOTA-83)&127];
			fTempPerm218 = fVec3[(IOTA-77)&127];
			fTempPerm219 = fVec3[(IOTA-74)&127];
			fTempPerm220 = fVec3[(IOTA-73)&127];
			fTempPerm221 = fVec3[(IOTA-64)&127];
			fTempPerm222 = fVec3[(IOTA-40)&127];
			fTempPerm223 = fVec3[(IOTA-14)&127];
			fTempPerm224 = fVec3[(IOTA-13)&127];
			fTempPerm225 = fVec3[(IOTA-10)&127];
			fTempPerm226 = fVec3[(IOTA-8)&127];
			fTempPerm227 = fVec3[(IOTA-6)&127];
			fTempPerm228 = fVec4[(IOTA-113)&127];
			fTempPerm229 = fVec4[(IOTA-119)&127];
			fTempPerm230 = fVec3[(IOTA-5)&127];
			fTempPerm231 = fVec4[(IOTA-126)&127];
			fTempPerm232 = fVec4[(IOTA-111)&127];
			fTempPerm233 = fVec4[(IOTA-110)&127];
			fTempPerm234 = fVec4[(IOTA-108)&127];
			fTempPerm235 = fVec4[(IOTA-105)&127];
			fTempPerm236 = fVec4[(IOTA-97)&127];
			fTempPerm237 = fVec4[(IOTA-95)&127];
			fTempPerm238 = fVec4[(IOTA-59)&127];
			fTempPerm239 = fVec4[(IOTA-61)&127];
			fTempPerm240 = fVec4[(IOTA-58)&127];
			fTempPerm241 = fVec4[(IOTA-36)&127];
			fTempPerm242 = fVec4[(IOTA-35)&127];
			fTempPerm243 = fVec4[(IOTA-31)&127];
			fTempPerm244 = fVec4[(IOTA-25)&127];
			fTempPerm245 = fVec4[(IOTA-26)&127];
			fTempPerm246 = fVec4[(IOTA-18)&127];
			fTempPerm247 = fVec4[(IOTA-17)&127];
			fTempPerm248 = fVec4[(IOTA-16)&127];
			fTempPerm249 = fVec4[(IOTA-15)&127];
			fTempPerm250 = fVec4[(IOTA-13)&127];
			fTempPerm251 = fVec4[(IOTA-10)&127];
			fTempPerm252 = fVec4[(IOTA-8)&127];
			fTempPerm253 = fVec4[(IOTA-6)&127];
			fTempPerm254 = fVec4[(IOTA-4)&127];
			fTempPerm255 = fVec4[(IOTA-2)&127];
			fTempPerm256 = fVec5[(IOTA-117)&127];
			fTempPerm257 = fVec5[(IOTA-115)&127];
			fTempPerm258 = fVec5[(IOTA-91)&127];
			fTempPerm259 = fVec5[(IOTA-53)&127];
			fTempPerm260 = fVec5[(IOTA-26)&127];
			fTempPerm261 = fVec5[(IOTA-23)&127];
			fTempPerm262 = fVec5[(IOTA-18)&127];
			fTempPerm263 = fVec5[(IOTA-17)&127];
			fTempPerm264 = fVec5[(IOTA-16)&127];
			fTempPerm265 = fVec5[(IOTA-15)&127];
			fTempPerm266 = fVec5[(IOTA-13)&127];
			fTempPerm267 = fVec5[(IOTA-10)&127];
			fTempPerm268 = fVec5[(IOTA-8)&127];
			fTempPerm269 = fVec5[(IOTA-6)&127];
			fTempPerm270 = fVec5[(IOTA-4)&127];
			fTempPerm271 = fVec5[(IOTA-2)&127];
			fTempPerm272 = fVec6[(IOTA-117)&127];
			fTempPerm273 = fVec5[(IOTA-55)&127];
			fTempPerm274 = fVec5[(IOTA-54)&127];
			fTempPerm275 = fVec6[(IOTA-97)&127];
			fTempPerm276 = fVec6[(IOTA-90)&127];
			fTempPerm277 = fVec6[(IOTA-125)&127];
			fTempPerm278 = fVec6[(IOTA-123)&127];
			fTempPerm279 = fVec6[(IOTA-89)&127];
			fTempPerm280 = fVec6[(IOTA-88)&127];
			fTempPerm281 = fVec6[(IOTA-63)&127];
			fTempPerm282 = fVec6[(IOTA-58)&127];
			fTempPerm283 = fVec6[(IOTA-47)&127];
			fTempPerm284 = fVec6[(IOTA-48)&127];
			fTempPerm285 = fVec6[(IOTA-44)&127];
			fTempPerm286 = fVec6[(IOTA-42)&127];
			fTempPerm287 = fVec6[(IOTA-41)&127];
			fTempPerm288 = fVec6[(IOTA-29)&127];
			fTempPerm289 = fVec6[(IOTA-28)&127];
			fTempPerm290 = fVec7[(IOTA-102)&127];
			fTempPerm291 = fVec7[(IOTA-100)&127];
			fTempPerm292 = fVec7[(IOTA-95)&127];
			fTempPerm293 = fVec7[(IOTA-94)&127];
			fTempPerm294 = fVec7[(IOTA-105)&127];
			fTempPerm295 = fVec7[(IOTA-89)&127];
			fTempPerm296 = fVec7[(IOTA-78)&127];
			fTempPerm297 = fVec7[(IOTA-85)&127];
			fTempPerm298 = fVec7[(IOTA-79)&127];
			fTempPerm299 = fVec7[(IOTA-107)&127];
			fTempPerm300 = fVec7[(IOTA-87)&127];
			fTempPerm301 = fVec7[(IOTA-77)&127];
			fTempPerm302 = fVec7[(IOTA-72)&127];
			fTempPerm303 = fVec7[(IOTA-7)&127];
			fTempPerm304 = fVec7[(IOTA-5)&127];
			fTempPerm305 = fVec7[(IOTA-4)&127];
			fTempPerm306 = fVec7[(IOTA-3)&127];
			fTempPerm307 = ((double)input0[i] * fRec3[0]);
			fRec11[0] = max((fRec11[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm307))))));
			fbargraph8 = fRec11[0];
			fTempPerm308 = fTempPerm307;
			fVec8[IOTA&127] = fTempPerm308;
			fTempPerm309 = fVec8[(IOTA-125)&127];
			fTempPerm310 = fVec8[(IOTA-124)&127];
			fTempPerm311 = fVec8[(IOTA-123)&127];
			fTempPerm312 = fVec8[(IOTA-120)&127];
			fTempPerm313 = fVec8[(IOTA-116)&127];
			fTempPerm314 = fVec8[(IOTA-106)&127];
			fTempPerm315 = fVec8[(IOTA-104)&127];
			fTempPerm316 = fVec8[(IOTA-101)&127];
			fTempPerm317 = fVec8[(IOTA-96)&127];
			fTempPerm318 = fVec8[(IOTA-110)&127];
			fTempPerm319 = fVec8[(IOTA-100)&127];
			fTempPerm320 = fVec8[(IOTA-98)&127];
			fTempPerm321 = fVec8[(IOTA-95)&127];
			fTempPerm322 = fVec8[(IOTA-82)&127];
			fTempPerm323 = fVec8[(IOTA-75)&127];
			fTempPerm324 = fVec8[(IOTA-73)&127];
			fTempPerm325 = fVec8[(IOTA-72)&127];
			fTempPerm326 = fVec8[(IOTA-69)&127];
			fTempPerm327 = fVec8[(IOTA-68)&127];
			fTempPerm328 = fVec8[(IOTA-67)&127];
			fTempPerm329 = fVec8[(IOTA-63)&127];
			fTempPerm330 = fVec8[(IOTA-40)&127];
			fTempPerm331 = fVec8[(IOTA-37)&127];
			fTempPerm332 = fVec8[(IOTA-30)&127];
			fTempPerm333 = fVec3[(IOTA-1)&127];
			fTempPerm334 = fVec7[(IOTA-96)&127];
			fTempPerm335 = fVec7[(IOTA-93)&127];
			fTempPerm336 = fVec7[(IOTA-84)&127];
			fTempPerm337 = fVec7[(IOTA-19)&127];
			fTempPerm338 = fVec7[(IOTA-18)&127];
			fTempPerm339 = fVec7[(IOTA-17)&127];
			fTempPerm340 = fVec7[(IOTA-16)&127];
			fTempPerm341 = fVec7[(IOTA-15)&127];
			fTempPerm342 = fVec7[(IOTA-14)&127];
			fTempPerm343 = fVec7[(IOTA-13)&127];
			fTempPerm344 = fVec7[(IOTA-11)&127];
			fTempPerm345 = fVec7[(IOTA-12)&127];
			fTempPerm346 = fVec7[(IOTA-9)&127];
			fTempPerm347 = fVec8[(IOTA-117)&127];
			fTempPerm348 = fVec8[(IOTA-115)&127];
			fTempPerm349 = fVec8[(IOTA-113)&127];
			fTempPerm350 = fVec8[(IOTA-111)&127];
			fTempPerm351 = fVec8[(IOTA-103)&127];
			fTempPerm352 = fVec8[(IOTA-102)&127];
			fTempPerm353 = fVec8[(IOTA-122)&127];
			fTempPerm354 = fVec8[(IOTA-121)&127];
			fTempPerm355 = fVec8[(IOTA-109)&127];
			fTempPerm356 = fVec8[(IOTA-57)&127];
			fTempPerm357 = fVec8[(IOTA-56)&127];
			fTempPerm358 = fVec8[(IOTA-79)&127];
			fTempPerm359 = fVec8[(IOTA-78)&127];
			fTempPerm360 = fVec8[(IOTA-77)&127];
			fTempPerm361 = fVec8[(IOTA-55)&127];
			fTempPerm362 = fVec8[(IOTA-54)&127];
			fTempPerm363 = fVec8[(IOTA-43)&127];
			fTempPerm364 = fVec8[(IOTA-42)&127];
			fTempPerm365 = fVec8[(IOTA-29)&127];
			fTempPerm366 = fVec8[(IOTA-27)&127];
			fTempPerm367 = fVec8[(IOTA-26)&127];
			fTempPerm368 = fVec8[(IOTA-25)&127];
			fTempPerm369 = fVec8[(IOTA-5)&127];
			fTempPerm370 = fVec8[(IOTA-4)&127];
			fTempPerm371 = fVec8[(IOTA-3)&127];
			fTempPerm372 = fVec0[(IOTA-117)&127];
			fTempPerm373 = fVec0[(IOTA-94)&127];
			fTempPerm374 = fVec0[(IOTA-77)&127];
			fTempPerm375 = fVec0[(IOTA-75)&127];
			fTempPerm376 = fVec0[(IOTA-70)&127];
			fTempPerm377 = fVec0[(IOTA-53)&127];
			fTempPerm378 = fVec0[(IOTA-42)&127];
			fTempPerm379 = fVec1[(IOTA-117)&127];
			fTempPerm380 = fVec1[(IOTA-108)&127];
			fTempPerm381 = fVec1[(IOTA-76)&127];
			fTempPerm382 = fVec1[(IOTA-72)&127];
			fTempPerm383 = fVec1[(IOTA-66)&127];
			fTempPerm384 = fVec1[(IOTA-86)&127];
			fTempPerm385 = fVec1[(IOTA-3)&127];
			fTempPerm386 = fVec1[(IOTA-2)&127];
			fTempPerm387 = fVec1[(IOTA-61)&127];
			fTempPerm388 = fVec1[(IOTA-64)&127];
			fTempPerm389 = fVec1[(IOTA-51)&127];
			fTempPerm390 = fVec1[(IOTA-41)&127];
			fTempPerm391 = fVec1[(IOTA-11)&127];
			fTempPerm392 = fVec1[(IOTA-10)&127];
			fTempPerm393 = fVec1[(IOTA-9)&127];
			fTempPerm394 = fVec1[(IOTA-7)&127];
			fTempPerm395 = fVec1[(IOTA-6)&127];
			fTempPerm396 = fVec1[(IOTA-50)&127];
			fTempPerm397 = fVec2[(IOTA-110)&127];
			fTempPerm398 = fVec2[(IOTA-105)&127];
			fTempPerm399 = fVec2[(IOTA-97)&127];
			fTempPerm400 = fVec2[(IOTA-102)&127];
			fTempPerm401 = fVec1[(IOTA-62)&127];
			fTempPerm402 = fVec2[(IOTA-93)&127];
			fTempPerm403 = fVec2[(IOTA-92)&127];
			fTempPerm404 = fVec2[(IOTA-70)&127];
			fTempPerm405 = fVec2[(IOTA-83)&127];
			fTempPerm406 = fVec2[(IOTA-82)&127];
			fTempPerm407 = fVec2[(IOTA-60)&127];
			fTempPerm408 = fVec2[(IOTA-21)&127];
			fTempPerm409 = fVec2[(IOTA-20)&127];
			fTempPerm410 = fVec2[(IOTA-19)&127];
			fTempPerm411 = fVec3[(IOTA-114)&127];
			fTempPerm412 = fVec3[(IOTA-103)&127];
			fTempPerm413 = fVec3[(IOTA-126)&127];
			fTempPerm414 = fVec3[(IOTA-109)&127];
			fTempPerm415 = fVec3[(IOTA-88)&127];
			fTempPerm416 = fVec3[(IOTA-93)&127];
			fTempPerm417 = fVec3[(IOTA-87)&127];
			fTempPerm418 = fVec3[(IOTA-76)&127];
			fTempPerm419 = fVec3[(IOTA-72)&127];
			fTempPerm420 = fVec3[(IOTA-67)&127];
			fTempPerm421 = fVec3[(IOTA-65)&127];
			fTempPerm422 = fVec3[(IOTA-75)&127];
			fTempPerm423 = fVec3[(IOTA-49)&127];
			fTempPerm424 = fVec3[(IOTA-48)&127];
			fTempPerm425 = fVec3[(IOTA-47)&127];
			fTempPerm426 = fVec3[(IOTA-36)&127];
			fTempPerm427 = fVec3[(IOTA-35)&127];
			fTempPerm428 = fVec4[(IOTA-116)&127];
			fTempPerm429 = fVec4[(IOTA-114)&127];
			fTempPerm430 = fVec4[(IOTA-107)&127];
			fTempPerm431 = fVec4[(IOTA-106)&127];
			fTempPerm432 = fVec4[(IOTA-101)&127];
			fTempPerm433 = fVec4[(IOTA-93)&127];
			fTempPerm434 = fVec4[(IOTA-66)&127];
			fTempPerm435 = fVec4[(IOTA-62)&127];
			fTempPerm436 = fVec4[(IOTA-50)&127];
			fTempPerm437 = fVec4[(IOTA-49)&127];
			fTempPerm438 = fVec5[(IOTA-121)&127];
			fTempPerm439 = fVec4[(IOTA-52)&127];
			fTempPerm440 = fVec5[(IOTA-116)&127];
			fTempPerm441 = fVec4[(IOTA-44)&127];
			fTempPerm442 = fVec4[(IOTA-45)&127];
			fTempPerm443 = fVec5[(IOTA-96)&127];
			fTempPerm444 = fVec5[(IOTA-57)&127];
			fTempPerm445 = fVec5[(IOTA-56)&127];
			fTempPerm446 = fVec5[(IOTA-48)&127];
			fTempPerm447 = fVec5[(IOTA-42)&127];
			fTempPerm448 = fVec6[(IOTA-93)&127];
			fTempPerm449 = fVec6[(IOTA-126)&127];
			fTempPerm450 = fVec6[(IOTA-119)&127];
			fTempPerm451 = fVec6[(IOTA-85)&127];
			fTempPerm452 = fVec6[(IOTA-80)&127];
			fTempPerm453 = fVec6[(IOTA-79)&127];
			fTempPerm454 = fVec6[(IOTA-92)&127];
			fTempPerm455 = fVec6[(IOTA-60)&127];
			fTempPerm456 = fVec6[(IOTA-59)&127];
			fTempPerm457 = fVec7[(IOTA-82)&127];
			fTempPerm458 = fVec7[(IOTA-80)&127];
			fTempPerm459 = fVec7[(IOTA-71)&127];
			fTempPerm460 = fVec7[(IOTA-21)&127];
			fTempPerm461 = fVec7[(IOTA-22)&127];
			fTempPerm462 = fVec7[(IOTA-20)&127];
			fTempPerm463 = fVec8[(IOTA-126)&127];
			fTempPerm464 = fVec8[(IOTA-86)&127];
			fTempPerm465 = fVec8[(IOTA-80)&127];
			fTempPerm466 = fVec8[(IOTA-70)&127];
			fTempPerm467 = fVec8[(IOTA-66)&127];
			fTempPerm468 = fVec8[(IOTA-65)&127];
			fTempPerm469 = fVec8[(IOTA-60)&127];
			fTempPerm470 = fVec8[(IOTA-59)&127];
			fTempPerm471 = fVec8[(IOTA-38)&127];
			fTempPerm472 = fVec3[(IOTA-29)&127];
			fTempPerm473 = fVec3[(IOTA-24)&127];
			fTempPerm474 = fVec3[(IOTA-23)&127];
			fTempPerm475 = fVec3[(IOTA-22)&127];
			fTempPerm476 = fVec3[(IOTA-20)&127];
			fTempPerm477 = fVec3[(IOTA-21)&127];
			fTempPerm478 = fVec4[(IOTA-124)&127];
			fTempPerm479 = fVec4[(IOTA-120)&127];
			fTempPerm480 = fVec4[(IOTA-112)&127];
			fTempPerm481 = fVec4[(IOTA-98)&127];
			fTempPerm482 = fVec4[(IOTA-94)&127];
			fTempPerm483 = fVec4[(IOTA-92)&127];
			fTempPerm484 = fVec4[(IOTA-40)&127];
			fTempPerm485 = fVec4[(IOTA-39)&127];
			fTempPerm486 = fVec4[(IOTA-38)&127];
			fTempPerm487 = fVec4[(IOTA-87)&127];
			fTempPerm488 = fVec5[(IOTA-119)&127];
			fTempPerm489 = fVec4[(IOTA-37)&127];
			fTempPerm490 = fVec5[(IOTA-95)&127];
			fTempPerm491 = fVec5[(IOTA-87)&127];
			fTempPerm492 = fVec5[(IOTA-52)&127];
			fTempPerm493 = fVec5[(IOTA-51)&127];
			fTempPerm494 = fVec5[(IOTA-49)&127];
			fTempPerm495 = fVec5[(IOTA-45)&127];
			fTempPerm496 = fVec5[(IOTA-44)&127];
			fTempPerm497 = fVec5[(IOTA-43)&127];
			fTempPerm498 = fVec5[(IOTA-39)&127];
			fTempPerm499 = fVec5[(IOTA-38)&127];
			fTempPerm500 = fVec6[(IOTA-94)&127];
			fTempPerm501 = fVec6[(IOTA-87)&127];
			fTempPerm502 = fVec6[(IOTA-84)&127];
			fTempPerm503 = fVec6[(IOTA-78)&127];
			fTempPerm504 = fVec6[(IOTA-77)&127];
			fTempPerm505 = fVec6[(IOTA-83)&127];
			fTempPerm506 = fVec6[(IOTA-50)&127];
			fTempPerm507 = fVec6[(IOTA-51)&127];
			fTempPerm508 = fVec6[(IOTA-39)&127];
			fTempPerm509 = fVec6[(IOTA-40)&127];
			fTempPerm510 = fVec6[(IOTA-35)&127];
			fTempPerm511 = fVec6[(IOTA-36)&127];
			fTempPerm512 = fVec6[(IOTA-33)&127];
			fTempPerm513 = fVec6[(IOTA-26)&127];
			fTempPerm514 = fVec6[(IOTA-32)&127];
			fTempPerm515 = fVec6[(IOTA-24)&127];
			fTempPerm516 = fVec6[(IOTA-21)&127];
			fTempPerm517 = fVec6[(IOTA-20)&127];
			fTempPerm518 = fVec6[(IOTA-17)&127];
			fTempPerm519 = fVec6[(IOTA-14)&127];
			fTempPerm520 = fVec6[(IOTA-13)&127];
			fTempPerm521 = fVec6[(IOTA-8)&127];
			fTempPerm522 = fVec6[(IOTA-5)&127];
			fTempPerm523 = fVec6[(IOTA-4)&127];
			fTempPerm524 = fVec6[(IOTA-2)&127];
			fTempPerm525 = fVec7[(IOTA-104)&127];
			fTempPerm526 = fVec7[(IOTA-103)&127];
			fTempPerm527 = fVec7[(IOTA-86)&127];
			fTempPerm528 = fVec7[(IOTA-83)&127];
			fTempPerm529 = fVec8[(IOTA-118)&127];
			fTempPerm530 = fVec8[(IOTA-114)&127];
			fTempPerm531 = fVec8[(IOTA-112)&127];
			fTempPerm532 = fVec8[(IOTA-105)&127];
			fTempPerm533 = fVec8[(IOTA-94)&127];
			fTempPerm534 = fVec8[(IOTA-108)&127];
			fTempPerm535 = fVec8[(IOTA-99)&127];
			fTempPerm536 = fVec8[(IOTA-93)&127];
			fTempPerm537 = fVec8[(IOTA-90)&127];
			fTempPerm538 = fVec8[(IOTA-76)&127];
			fTempPerm539 = fVec8[(IOTA-83)&127];
			fTempPerm540 = fVec8[(IOTA-74)&127];
			fTempPerm541 = fVec8[(IOTA-71)&127];
			fTempPerm542 = fVec8[(IOTA-87)&127];
			fTempPerm543 = fVec8[(IOTA-62)&127];
			fTempPerm544 = fVec8[(IOTA-61)&127];
			fTempPerm545 = fVec8[(IOTA-58)&127];
			fTempPerm546 = fVec8[(IOTA-53)&127];
			fTempPerm547 = fVec8[(IOTA-52)&127];
			fTempPerm548 = fVec8[(IOTA-51)&127];
			fTempPerm549 = fVec8[(IOTA-50)&127];
			fTempPerm550 = fVec8[(IOTA-48)&127];
			fTempPerm551 = fVec8[(IOTA-49)&127];
			fTempPerm552 = fVec8[(IOTA-64)&127];
			fTempPerm553 = fVec8[(IOTA-46)&127];
			fTempPerm554 = fVec8[(IOTA-45)&127];
			fTempPerm555 = fVec8[(IOTA-44)&127];
			fTempPerm556 = fVec8[(IOTA-23)&127];
			fTempPerm557 = fVec8[(IOTA-22)&127];
			fTempPerm558 = fVec8[(IOTA-21)&127];
			fTempPerm559 = fVec8[(IOTA-20)&127];
			fTempPerm560 = fVec8[(IOTA-19)&127];
			fTempPerm561 = fVec8[(IOTA-18)&127];
			fTempPerm562 = fVec8[(IOTA-17)&127];
			fTempPerm563 = fVec8[(IOTA-15)&127];
			fTempPerm564 = fVec8[(IOTA-6)&127];
			fTempPerm565 = fVec8[(IOTA-7)&127];
			fTempPerm566 = fVec8[(IOTA-8)&127];
			fTempPerm567 = fVec8[(IOTA-9)&127];
			fTempPerm568 = fVec8[(IOTA-10)&127];
			fTempPerm569 = fVec8[(IOTA-11)&127];
			fTempPerm570 = fVec8[(IOTA-12)&127];
			fTempPerm571 = fVec8[(IOTA-16)&127];
			fTempPerm572 = fVec8[(IOTA-13)&127];
			fTempPerm573 = fVec8[(IOTA-14)&127];
			fTempPerm574 = fVec4[(IOTA-1)&127];
			fTempPerm575 = fVec5[(IOTA-1)&127];
			fTempPerm576 = fVec6[(IOTA-1)&127];
			fTempPerm577 = fVec7[(IOTA-1)&127];
			fTempPerm578 = fVec8[(IOTA-1)&127];
			fTempPerm579 = fVec0[(IOTA-22)&127];
			fTempPerm580 = fVec3[(IOTA-78)&127];
			fTempPerm581 = fVec4[(IOTA-48)&127];
			fTempPerm582 = fVec4[(IOTA-30)&127];
			fTempPerm583 = fVec5[(IOTA-22)&127];
			fTempPerm584 = fVec6[(IOTA-86)&127];
			fTempPerm585 = fVec7[(IOTA-92)&127];
			fTempPerm586 = fVec8[(IOTA-119)&127];
			fTempPerm587 = fVec8[(IOTA-107)&127];
			fTempPerm588 = fVec8[(IOTA-41)&127];
			fTempPerm589 = fVec8[(IOTA-39)&127];
			fTempPerm590 = fVec2[(IOTA-25)&127];
			fTempPerm591 = fVec2[(IOTA-24)&127];
			fTempPerm592 = fVec3[(IOTA-28)&127];
			fTempPerm593 = fVec3[(IOTA-17)&127];
			fTempPerm594 = fVec4[(IOTA-104)&127];
			fTempPerm595 = fVec4[(IOTA-57)&127];
			fTempPerm596 = fVec5[(IOTA-30)&127];
			fTempPerm597 = fVec5[(IOTA-29)&127];
			fTempPerm598 = fVec6[(IOTA-118)&127];
			fTempPerm599 = fVec7[(IOTA-90)&127];
			fTempPerm600 = fVec7[(IOTA-73)&127];
			fTempPerm601 = fVec8[(IOTA-36)&127];
			fTempPerm602 = fVec8[(IOTA-35)&127];
			fTempPerm603 = fVec8[(IOTA-34)&127];
			fTempPerm604 = fVec8[(IOTA-33)&127];
			fTempPerm605 = fVec8[(IOTA-32)&127];
			fTempPerm606 = fVec8[(IOTA-31)&127];
			fTempPerm607 = fVec2[(IOTA-119)&127];
			fTempPerm608 = fVec3[(IOTA-84)&127];
			fTempPerm609 = fVec3[(IOTA-33)&127];
			fTempPerm610 = fVec3[(IOTA-32)&127];
			fTempPerm611 = fVec4[(IOTA-123)&127];
			fTempPerm612 = fVec4[(IOTA-75)&127];
			fTempPerm613 = fVec5[(IOTA-84)&127];
			fTempPerm614 = fVec7[(IOTA-88)&127];
			fTempPerm615 = fVec5[(IOTA-83)&127];
			fTempPerm616 = fVec8[(IOTA-85)&127];
			fTempPerm617 = fVec4[(IOTA-78)&127];
			fTempPerm618 = fVec4[(IOTA-83)&127];
			fTempPerm619 = fVec4[(IOTA-121)&127];
			fTempPerm620 = fVec3[(IOTA-116)&127];
			fTempPerm621 = fVec2[(IOTA-2)&127];
			fTempPerm622 = fVec2[(IOTA-6)&127];
			fTempPerm623 = fVec2[(IOTA-8)&127];
			fTempPerm624 = fVec2[(IOTA-10)&127];
			fTempPerm625 = fVec2[(IOTA-18)&127];
			fTempPerm626 = fVec2[(IOTA-101)&127];
			fTempPerm627 = fVec2[(IOTA-109)&127];
			fTempPerm628 = fVec0[(IOTA-74)&127];
			fTempPerm629 = fVec0[(IOTA-124)&127];
			fTempPerm630 = fVec0[(IOTA-119)&127];
			fTempPerm631 = fVec0[(IOTA-113)&127];
			fTempPerm632 = fVec0[(IOTA-109)&127];
			fTempPerm633 = fVec0[(IOTA-99)&127];
			fTempPerm634 = fVec0[(IOTA-125)&127];
			fTempPerm635 = fVec0[(IOTA-123)&127];
			fTempPerm636 = fVec0[(IOTA-116)&127];
			fTempPerm637 = fVec0[(IOTA-115)&127];
			fTempPerm638 = fVec4[(IOTA-109)&127];
			fTempPerm639 = fVec1[(IOTA-78)&127];
			fTempPerm640 = fVec0[(IOTA-92)&127];
			fTempPerm641 = fVec0[(IOTA-111)&127];
			fTempPerm642 = fVec0[(IOTA-107)&127];
			fTempPerm643 = fVec0[(IOTA-90)&127];
			fTempPerm644 = fVec0[(IOTA-82)&127];
			fTempPerm645 = fVec0[(IOTA-61)&127];
			fTempPerm646 = fVec0[(IOTA-57)&127];
			fTempPerm647 = fVec0[(IOTA-62)&127];
			fTempPerm648 = fVec0[(IOTA-56)&127];
			fTempPerm649 = fVec0[(IOTA-54)&127];
			fTempPerm650 = fVec0[(IOTA-36)&127];
			fTempPerm651 = fVec0[(IOTA-37)&127];
			fTempPerm652 = fVec0[(IOTA-45)&127];
			fTempPerm653 = fVec0[(IOTA-32)&127];
			fTempPerm654 = fVec1[(IOTA-123)&127];
			fTempPerm655 = fVec1[(IOTA-113)&127];
			fTempPerm656 = fVec1[(IOTA-118)&127];
			fTempPerm657 = fVec1[(IOTA-109)&127];
			fTempPerm658 = fVec1[(IOTA-91)&127];
			fTempPerm659 = fVec1[(IOTA-75)&127];
			fTempPerm660 = fVec1[(IOTA-74)&127];
			fTempPerm661 = fVec1[(IOTA-28)&127];
			fTempPerm662 = fVec1[(IOTA-30)&127];
			fTempPerm663 = fVec1[(IOTA-27)&127];
			fTempPerm664 = fVec1[(IOTA-17)&127];
			fTempPerm665 = fVec1[(IOTA-16)&127];
			fTempPerm666 = fVec1[(IOTA-14)&127];
			fTempPerm667 = fVec1[(IOTA-13)&127];
			fTempPerm668 = fVec1[(IOTA-119)&127];
			fTempPerm669 = fVec2[(IOTA-123)&127];
			fTempPerm670 = fVec2[(IOTA-122)&127];
			fTempPerm671 = fVec2[(IOTA-121)&127];
			fTempPerm672 = fVec2[(IOTA-114)&127];
			fTempPerm673 = fVec2[(IOTA-111)&127];
			fTempPerm674 = fVec2[(IOTA-86)&127];
			fTempPerm675 = fVec2[(IOTA-58)&127];
			fTempPerm676 = fVec2[(IOTA-81)&127];
			fTempPerm677 = fVec2[(IOTA-80)&127];
			fTempPerm678 = fVec2[(IOTA-56)&127];
			fTempPerm679 = fVec2[(IOTA-53)&127];
			fTempPerm680 = fVec2[(IOTA-47)&127];
			fTempPerm681 = fVec2[(IOTA-43)&127];
			fTempPerm682 = fVec2[(IOTA-46)&127];
			fTempPerm683 = fVec3[(IOTA-121)&127];
			fTempPerm684 = fVec3[(IOTA-57)&127];
			fTempPerm685 = fVec3[(IOTA-56)&127];
			fTempPerm686 = fVec3[(IOTA-66)&127];
			fTempPerm687 = fVec3[(IOTA-80)&127];
			fTempPerm688 = fVec3[(IOTA-55)&127];
			fTempPerm689 = fVec3[(IOTA-50)&127];
			fTempPerm690 = fVec3[(IOTA-44)&127];
			fTempPerm691 = fVec3[(IOTA-43)&127];
			fTempPerm692 = fVec3[(IOTA-37)&127];
			fTempPerm693 = fVec0[(IOTA-108)&127];
			fTempPerm694 = fVec0[(IOTA-97)&127];
			fTempPerm695 = fVec0[(IOTA-98)&127];
			fTempPerm696 = fVec0[(IOTA-84)&127];
			fTempPerm697 = fVec0[(IOTA-63)&127];
			fTempPerm698 = fVec0[(IOTA-85)&127];
			fTempPerm699 = fVec0[(IOTA-59)&127];
			fTempPerm700 = fVec0[(IOTA-41)&127];
			fTempPerm701 = fVec0[(IOTA-60)&127];
			fTempPerm702 = fVec0[(IOTA-35)&127];
			fTempPerm703 = fVec0[(IOTA-40)&127];
			fTempPerm704 = fVec0[(IOTA-39)&127];
			fTempPerm705 = fVec0[(IOTA-25)&127];
			fTempPerm706 = fVec0[(IOTA-17)&127];
			fTempPerm707 = fVec0[(IOTA-16)&127];
			fTempPerm708 = fVec0[(IOTA-15)&127];
			fTempPerm709 = fVec0[(IOTA-14)&127];
			fTempPerm710 = fVec0[(IOTA-13)&127];
			fTempPerm711 = fVec0[(IOTA-12)&127];
			fTempPerm712 = fVec0[(IOTA-11)&127];
			fTempPerm713 = fVec0[(IOTA-9)&127];
			fTempPerm714 = fVec0[(IOTA-7)&127];
			fTempPerm715 = fVec0[(IOTA-5)&127];
			fTempPerm716 = fVec0[(IOTA-3)&127];
			fTempPerm717 = fVec1[(IOTA-120)&127];
			fTempPerm718 = fVec1[(IOTA-115)&127];
			fTempPerm719 = fVec1[(IOTA-90)&127];
			fTempPerm720 = fVec1[(IOTA-89)&127];
			fTempPerm721 = fVec1[(IOTA-83)&127];
			fTempPerm722 = fVec1[(IOTA-81)&127];
			fTempPerm723 = fVec1[(IOTA-65)&127];
			fTempPerm724 = fVec1[(IOTA-49)&127];
			fTempPerm725 = fVec1[(IOTA-47)&127];
			fTempPerm726 = fVec1[(IOTA-48)&127];
			fTempPerm727 = fVec1[(IOTA-46)&127];
			fTempPerm728 = fVec1[(IOTA-42)&127];
			fTempPerm729 = fVec1[(IOTA-45)&127];
			fTempPerm730 = fVec1[(IOTA-37)&127];
			fTempPerm731 = fVec1[(IOTA-34)&127];
			fTempPerm732 = fVec1[(IOTA-33)&127];
			fTempPerm733 = fVec2[(IOTA-74)&127];
			fTempPerm734 = fVec2[(IOTA-57)&127];
			fTempPerm735 = fVec2[(IOTA-38)&127];
			fTempPerm736 = fVec2[(IOTA-23)&127];
			fTempPerm737 = fVec3[(IOTA-118)&127];
			fTempPerm738 = fVec3[(IOTA-111)&127];
			fTempPerm739 = fVec3[(IOTA-63)&127];
			fTempPerm740 = fVec3[(IOTA-27)&127];
			fTempPerm741 = fVec3[(IOTA-3)&127];
			fTempPerm742 = fVec4[(IOTA-122)&127];
			fTempPerm743 = fVec4[(IOTA-103)&127];
			fTempPerm744 = fVec4[(IOTA-96)&127];
			fTempPerm745 = fVec4[(IOTA-65)&127];
			fTempPerm746 = fVec4[(IOTA-71)&127];
			fTempPerm747 = fVec4[(IOTA-63)&127];
			fTempPerm748 = fVec4[(IOTA-41)&127];
			fTempPerm749 = fVec4[(IOTA-64)&127];
			fTempPerm750 = fVec4[(IOTA-72)&127];
			fTempPerm751 = fVec4[(IOTA-60)&127];
			fTempPerm752 = fVec4[(IOTA-27)&127];
			fTempPerm753 = fVec5[(IOTA-108)&127];
			fTempPerm754 = fVec5[(IOTA-102)&127];
			fTempPerm755 = fVec5[(IOTA-114)&127];
			fTempPerm756 = fVec5[(IOTA-101)&127];
			fTempPerm757 = fVec5[(IOTA-100)&127];
			fTempPerm758 = fVec5[(IOTA-99)&127];
			fTempPerm759 = fVec5[(IOTA-98)&127];
			fTempPerm760 = fVec5[(IOTA-80)&127];
			fTempPerm761 = fVec5[(IOTA-81)&127];
			fTempPerm762 = fVec5[(IOTA-79)&127];
			fTempPerm763 = fVec5[(IOTA-73)&127];
			fTempPerm764 = fVec5[(IOTA-78)&127];
			fTempPerm765 = fVec5[(IOTA-77)&127];
			fTempPerm766 = fVec5[(IOTA-61)&127];
			fTempPerm767 = fVec5[(IOTA-60)&127];
			fTempPerm768 = fVec5[(IOTA-47)&127];
			fTempPerm769 = fVec5[(IOTA-21)&127];
			fTempPerm770 = fVec5[(IOTA-19)&127];
			fTempPerm771 = fVec5[(IOTA-32)&127];
			fTempPerm772 = fVec5[(IOTA-31)&127];
			fTempPerm773 = fVec6[(IOTA-110)&127];
			fTempPerm774 = fVec6[(IOTA-100)&127];
			fTempPerm775 = fVec6[(IOTA-111)&127];
			fTempPerm776 = fVec5[(IOTA-35)&127];
			fTempPerm777 = fVec6[(IOTA-96)&127];
			fTempPerm778 = fVec6[(IOTA-95)&127];
			fTempPerm779 = fVec6[(IOTA-91)&127];
			fTempPerm780 = fVec6[(IOTA-76)&127];
			fTempPerm781 = fVec6[(IOTA-61)&127];
			fTempPerm782 = fVec7[(IOTA-122)&127];
			fTempPerm783 = fVec7[(IOTA-118)&127];
			fTempPerm784 = fVec7[(IOTA-121)&127];
			fTempPerm785 = fVec7[(IOTA-110)&127];
			fTempPerm786 = fVec0[(IOTA-89)&127];
			fTempPerm787 = fVec0[(IOTA-93)&127];
			fTempPerm788 = fVec0[(IOTA-80)&127];
			fTempPerm789 = fVec0[(IOTA-51)&127];
			fTempPerm790 = fVec0[(IOTA-50)&127];
			fTempPerm791 = fVec0[(IOTA-31)&127];
			fTempPerm792 = fVec0[(IOTA-49)&127];
			fTempPerm793 = fVec1[(IOTA-122)&127];
			fTempPerm794 = fVec1[(IOTA-114)&127];
			fTempPerm795 = fVec1[(IOTA-96)&127];
			fTempPerm796 = fVec1[(IOTA-92)&127];
			fTempPerm797 = fVec1[(IOTA-87)&127];
			fTempPerm798 = fVec1[(IOTA-69)&127];
			fTempPerm799 = fVec1[(IOTA-53)&127];
			fTempPerm800 = fVec1[(IOTA-43)&127];
			fTempPerm801 = fVec1[(IOTA-20)&127];
			fTempPerm802 = fVec2[(IOTA-125)&127];
			fTempPerm803 = fVec2[(IOTA-124)&127];
			fTempPerm804 = fVec2[(IOTA-112)&127];
			fTempPerm805 = fVec2[(IOTA-117)&127];
			fTempPerm806 = fVec1[(IOTA-24)&127];
			fTempPerm807 = fVec1[(IOTA-23)&127];
			fTempPerm808 = fVec1[(IOTA-21)&127];
			fTempPerm809 = fVec1[(IOTA-22)&127];
			fTempPerm810 = fVec2[(IOTA-116)&127];
			fTempPerm811 = fVec2[(IOTA-78)&127];
			fTempPerm812 = fVec2[(IOTA-73)&127];
			fTempPerm813 = fVec2[(IOTA-59)&127];
			fTempPerm814 = fVec2[(IOTA-61)&127];
			fTempPerm815 = fVec2[(IOTA-85)&127];
			fTempPerm816 = fVec2[(IOTA-54)&127];
			fTempPerm817 = fVec2[(IOTA-31)&127];
			fTempPerm818 = fVec2[(IOTA-34)&127];
			fTempPerm819 = fVec3[(IOTA-122)&127];
			fTempPerm820 = fVec2[(IOTA-55)&127];
			fTempPerm821 = fVec2[(IOTA-35)&127];
			fTempPerm822 = fVec3[(IOTA-117)&127];
			fTempPerm823 = fVec3[(IOTA-82)&127];
			fTempPerm824 = fVec3[(IOTA-79)&127];
			fTempPerm825 = fVec3[(IOTA-69)&127];
			fTempPerm826 = fVec3[(IOTA-68)&127];
			fTempPerm827 = fVec3[(IOTA-62)&127];
			fTempPerm828 = fVec3[(IOTA-61)&127];
			fTempPerm829 = fVec3[(IOTA-59)&127];
			fTempPerm830 = fVec3[(IOTA-41)&127];
			fTempPerm831 = fVec3[(IOTA-15)&127];
			fTempPerm832 = fVec3[(IOTA-12)&127];
			fTempPerm833 = fVec3[(IOTA-11)&127];
			fTempPerm834 = fVec3[(IOTA-9)&127];
			fTempPerm835 = fVec3[(IOTA-7)&127];
			fTempPerm836 = fVec4[(IOTA-70)&127];
			fTempPerm837 = fVec4[(IOTA-46)&127];
			fTempPerm838 = fVec4[(IOTA-34)&127];
			fTempPerm839 = fVec4[(IOTA-33)&127];
			fTempPerm840 = fVec4[(IOTA-47)&127];
			fTempPerm841 = fVec4[(IOTA-32)&127];
			fTempPerm842 = fVec4[(IOTA-24)&127];
			fTempPerm843 = fVec4[(IOTA-23)&127];
			fTempPerm844 = fVec4[(IOTA-21)&127];
			fTempPerm845 = fVec4[(IOTA-22)&127];
			fTempPerm846 = fVec4[(IOTA-20)&127];
			fTempPerm847 = fVec4[(IOTA-19)&127];
			fTempPerm848 = fVec4[(IOTA-14)&127];
			fTempPerm849 = fVec4[(IOTA-11)&127];
			fTempPerm850 = fVec4[(IOTA-9)&127];
			fTempPerm851 = fVec4[(IOTA-7)&127];
			fTempPerm852 = fVec4[(IOTA-5)&127];
			fTempPerm853 = fVec4[(IOTA-3)&127];
			fTempPerm854 = fVec4[(IOTA-12)&127];
			fTempPerm855 = fVec5[(IOTA-111)&127];
			fTempPerm856 = fVec5[(IOTA-124)&127];
			fTempPerm857 = fVec5[(IOTA-113)&127];
			fTempPerm858 = fVec5[(IOTA-97)&127];
			fTempPerm859 = fVec5[(IOTA-92)&127];
			fTempPerm860 = fVec5[(IOTA-69)&127];
			fTempPerm861 = fVec5[(IOTA-27)&127];
			fTempPerm862 = fVec5[(IOTA-24)&127];
			fTempPerm863 = fVec5[(IOTA-14)&127];
			fTempPerm864 = fVec5[(IOTA-12)&127];
			fTempPerm865 = fVec5[(IOTA-11)&127];
			fTempPerm866 = fVec5[(IOTA-9)&127];
			fTempPerm867 = fVec5[(IOTA-7)&127];
			fTempPerm868 = fVec5[(IOTA-5)&127];
			fTempPerm869 = fVec5[(IOTA-3)&127];
			fTempPerm870 = fVec6[(IOTA-114)&127];
			fTempPerm871 = fVec6[(IOTA-113)&127];
			fTempPerm872 = fVec6[(IOTA-112)&127];
			fTempPerm873 = fVec6[(IOTA-107)&127];
			fTempPerm874 = fVec5[(IOTA-28)&127];
			fTempPerm875 = fVec6[(IOTA-102)&127];
			fTempPerm876 = fVec6[(IOTA-74)&127];
			fTempPerm877 = fVec6[(IOTA-64)&127];
			fTempPerm878 = fVec6[(IOTA-82)&127];
			fTempPerm879 = fVec6[(IOTA-55)&127];
			fTempPerm880 = fVec6[(IOTA-30)&127];
			fTempPerm881 = fVec6[(IOTA-43)&127];
			fTempPerm882 = fVec7[(IOTA-125)&127];
			fTempPerm883 = fVec6[(IOTA-57)&127];
			fTempPerm884 = fVec6[(IOTA-31)&127];
			fTempPerm885 = fVec7[(IOTA-124)&127];
			fTempPerm886 = fVec7[(IOTA-120)&127];
			fTempPerm887 = fVec7[(IOTA-119)&127];
			fTempPerm888 = fVec7[(IOTA-116)&127];
			fTempPerm889 = fVec7[(IOTA-112)&127];
			fTempPerm890 = fVec7[(IOTA-74)&127];
			fTempPerm891 = fVec7[(IOTA-68)&127];
			fTempPerm892 = fVec7[(IOTA-63)&127];
			fTempPerm893 = fVec7[(IOTA-56)&127];
			fTempPerm894 = fVec7[(IOTA-48)&127];
			fTempPerm895 = fVec7[(IOTA-46)&127];
			fTempPerm896 = fVec7[(IOTA-45)&127];
			fTempPerm897 = fVec7[(IOTA-41)&127];
			fTempPerm898 = fVec7[(IOTA-47)&127];
			fTempPerm899 = fVec7[(IOTA-39)&127];
			fTempPerm900 = fVec7[(IOTA-40)&127];
			fTempPerm901 = fVec7[(IOTA-38)&127];
			fTempPerm902 = fVec7[(IOTA-37)&127];
			fTempPerm903 = fVec7[(IOTA-36)&127];
			fTempPerm904 = fVec7[(IOTA-35)&127];
			fTempPerm905 = fVec7[(IOTA-6)&127];
			fTempPerm906 = fVec7[(IOTA-2)&127];
			fTempPerm907 = fVec8[(IOTA-97)&127];
			fTempPerm908 = fVec1[(IOTA-1)&127];
			fTempPerm909 = fVec7[(IOTA-109)&127];
			fTempPerm910 = fVec7[(IOTA-81)&127];
			fTempPerm911 = fVec7[(IOTA-75)&127];
			fTempPerm912 = fVec7[(IOTA-70)&127];
			fTempPerm913 = fVec7[(IOTA-66)&127];
			fTempPerm914 = fVec7[(IOTA-69)&127];
			fTempPerm915 = fVec7[(IOTA-65)&127];
			fTempPerm916 = fVec7[(IOTA-61)&127];
			fTempPerm917 = fVec7[(IOTA-53)&127];
			fTempPerm918 = fVec7[(IOTA-60)&127];
			fTempPerm919 = fVec7[(IOTA-55)&127];
			fTempPerm920 = fVec7[(IOTA-52)&127];
			fTempPerm921 = fVec7[(IOTA-54)&127];
			fTempPerm922 = fVec7[(IOTA-49)&127];
			fTempPerm923 = fVec7[(IOTA-44)&127];
			fTempPerm924 = fVec7[(IOTA-51)&127];
			fTempPerm925 = fVec7[(IOTA-27)&127];
			fTempPerm926 = fVec7[(IOTA-43)&127];
			fTempPerm927 = fVec7[(IOTA-26)&127];
			fTempPerm928 = fVec7[(IOTA-25)&127];
			fTempPerm929 = fVec7[(IOTA-10)&127];
			fTempPerm930 = fVec7[(IOTA-8)&127];
			fTempPerm931 = fVec8[(IOTA-92)&127];
			fTempPerm932 = fVec8[(IOTA-91)&127];
			fTempPerm933 = fVec8[(IOTA-88)&127];
			fTempPerm934 = fVec8[(IOTA-28)&127];
			fTempPerm935 = fVec8[(IOTA-2)&127];
			fTempPerm936 = fVec0[(IOTA-1)&127];
			fTempPerm937 = fVec2[(IOTA-1)&127];
			fTempPerm938 = fVec0[(IOTA-118)&127];
			fTempPerm939 = fVec0[(IOTA-114)&127];
			fTempPerm940 = fVec0[(IOTA-112)&127];
			fTempPerm941 = fVec0[(IOTA-106)&127];
			fTempPerm942 = fVec0[(IOTA-102)&127];
			fTempPerm943 = fVec0[(IOTA-105)&127];
			fTempPerm944 = fVec0[(IOTA-101)&127];
			fTempPerm945 = fVec0[(IOTA-88)&127];
			fTempPerm946 = fVec0[(IOTA-83)&127];
			fTempPerm947 = fVec0[(IOTA-81)&127];
			fTempPerm948 = fVec0[(IOTA-87)&127];
			fTempPerm949 = fVec0[(IOTA-78)&127];
			fTempPerm950 = fVec0[(IOTA-58)&127];
			fTempPerm951 = fVec0[(IOTA-52)&127];
			fTempPerm952 = fVec0[(IOTA-55)&127];
			fTempPerm953 = fVec0[(IOTA-48)&127];
			fTempPerm954 = fVec0[(IOTA-47)&127];
			fTempPerm955 = fVec0[(IOTA-46)&127];
			fTempPerm956 = fVec0[(IOTA-44)&127];
			fTempPerm957 = fVec0[(IOTA-43)&127];
			fTempPerm958 = fVec1[(IOTA-95)&127];
			fTempPerm959 = fVec1[(IOTA-88)&127];
			fTempPerm960 = fVec1[(IOTA-80)&127];
			fTempPerm961 = fVec1[(IOTA-82)&127];
			fTempPerm962 = fVec1[(IOTA-73)&127];
			fTempPerm963 = fVec1[(IOTA-84)&127];
			fTempPerm964 = fVec1[(IOTA-5)&127];
			fTempPerm965 = fVec1[(IOTA-4)&127];
			fTempPerm966 = fVec1[(IOTA-52)&127];
			fTempPerm967 = fVec1[(IOTA-60)&127];
			fTempPerm968 = fVec1[(IOTA-8)&127];
			fTempPerm969 = fVec2[(IOTA-120)&127];
			fTempPerm970 = fVec2[(IOTA-115)&127];
			fTempPerm971 = fVec2[(IOTA-113)&127];
			fTempPerm972 = fVec2[(IOTA-103)&127];
			fTempPerm973 = fVec2[(IOTA-95)&127];
			fTempPerm974 = fVec2[(IOTA-89)&127];
			fTempPerm975 = fVec2[(IOTA-77)&127];
			fTempPerm976 = fVec2[(IOTA-76)&127];
			fTempPerm977 = fVec2[(IOTA-96)&127];
			fTempPerm978 = fVec2[(IOTA-69)&127];
			fTempPerm979 = fVec2[(IOTA-68)&127];
			fTempPerm980 = fVec2[(IOTA-27)&127];
			fTempPerm981 = fVec3[(IOTA-120)&127];
			fTempPerm982 = fVec3[(IOTA-81)&127];
			fTempPerm983 = fVec3[(IOTA-58)&127];
			fTempPerm984 = fVec3[(IOTA-53)&127];
			fTempPerm985 = fVec3[(IOTA-46)&127];
			fTempPerm986 = fVec4[(IOTA-84)&127];
			fTempPerm987 = fVec4[(IOTA-82)&127];
			fTempPerm988 = fVec4[(IOTA-79)&127];
			fTempPerm989 = fVec4[(IOTA-77)&127];
			fTempPerm990 = fVec4[(IOTA-73)&127];
			fTempPerm991 = fVec4[(IOTA-53)&127];
			fTempPerm992 = fVec4[(IOTA-55)&127];
			fTempPerm993 = fVec4[(IOTA-54)&127];
			fTempPerm994 = fVec4[(IOTA-51)&127];
			fTempPerm995 = fVec5[(IOTA-118)&127];
			fTempPerm996 = fVec5[(IOTA-106)&127];
			fTempPerm997 = fVec4[(IOTA-43)&127];
			fTempPerm998 = fVec5[(IOTA-107)&127];
			fTempPerm999 = fVec5[(IOTA-104)&127];
			fTempPerm1000 = fVec5[(IOTA-103)&127];
			fTempPerm1001 = fVec5[(IOTA-93)&127];
			fTempPerm1002 = fVec5[(IOTA-88)&127];
			fTempPerm1003 = fVec5[(IOTA-66)&127];
			fTempPerm1004 = fVec5[(IOTA-72)&127];
			fTempPerm1005 = fVec5[(IOTA-58)&127];
			fTempPerm1006 = fVec6[(IOTA-109)&127];
			fTempPerm1007 = fVec6[(IOTA-106)&127];
			fTempPerm1008 = fVec6[(IOTA-104)&127];
			fTempPerm1009 = fVec6[(IOTA-99)&127];
			fTempPerm1010 = fVec6[(IOTA-72)&127];
			fTempPerm1011 = fVec6[(IOTA-67)&127];
			fTempPerm1012 = fVec6[(IOTA-101)&127];
			fTempPerm1013 = fVec6[(IOTA-71)&127];
			fTempPerm1014 = fVec6[(IOTA-66)&127];
			fTempPerm1015 = fVec7[(IOTA-126)&127];
			fTempPerm1016 = fVec7[(IOTA-76)&127];
			fTempPerm1017 = fVec7[(IOTA-59)&127];
			fTempPerm1018 = fVec7[(IOTA-24)&127];
			fTempPerm1019 = fVec7[(IOTA-57)&127];
			fTempPerm1020 = fVec7[(IOTA-58)&127];
			fTempPerm1021 = fVec7[(IOTA-23)&127];
			fTempPerm1022 = fVec8[(IOTA-84)&127];
			fTempPerm1023 = fVec8[(IOTA-24)&127];
			fTempPerm1024 = fVec3[(IOTA-31)&127];
			fTempPerm1025 = fVec3[(IOTA-30)&127];
			fTempPerm1026 = fVec3[(IOTA-25)&127];
			fTempPerm1027 = fVec3[(IOTA-19)&127];
			fTempPerm1028 = fVec3[(IOTA-18)&127];
			fTempPerm1029 = fVec4[(IOTA-100)&127];
			fTempPerm1030 = fVec4[(IOTA-99)&127];
			fTempPerm1031 = fVec4[(IOTA-90)&127];
			fTempPerm1032 = fVec4[(IOTA-85)&127];
			fTempPerm1033 = fVec4[(IOTA-81)&127];
			fTempPerm1034 = fVec4[(IOTA-88)&127];
			fTempPerm1035 = fVec5[(IOTA-82)&127];
			fTempPerm1036 = fVec5[(IOTA-109)&127];
			fTempPerm1037 = fVec5[(IOTA-89)&127];
			fTempPerm1038 = fVec5[(IOTA-76)&127];
			fTempPerm1039 = fVec5[(IOTA-75)&127];
			fTempPerm1040 = fVec5[(IOTA-110)&127];
			fTempPerm1041 = fVec5[(IOTA-74)&127];
			fTempPerm1042 = fVec5[(IOTA-68)&127];
			fTempPerm1043 = fVec5[(IOTA-67)&127];
			fTempPerm1044 = fVec5[(IOTA-59)&127];
			fTempPerm1045 = fVec5[(IOTA-46)&127];
			fTempPerm1046 = fVec5[(IOTA-50)&127];
			fTempPerm1047 = fVec5[(IOTA-41)&127];
			fTempPerm1048 = fVec5[(IOTA-25)&127];
			fTempPerm1049 = fVec6[(IOTA-115)&127];
			fTempPerm1050 = fVec6[(IOTA-108)&127];
			fTempPerm1051 = fVec6[(IOTA-105)&127];
			fTempPerm1052 = fVec6[(IOTA-103)&127];
			fTempPerm1053 = fVec6[(IOTA-73)&127];
			fTempPerm1054 = fVec6[(IOTA-81)&127];
			fTempPerm1055 = fVec6[(IOTA-69)&127];
			fTempPerm1056 = fVec5[(IOTA-40)&127];
			fTempPerm1057 = fVec6[(IOTA-70)&127];
			fTempPerm1058 = fVec6[(IOTA-68)&127];
			fTempPerm1059 = fVec6[(IOTA-65)&127];
			fTempPerm1060 = fVec6[(IOTA-37)&127];
			fTempPerm1061 = fVec6[(IOTA-38)&127];
			fTempPerm1062 = fVec6[(IOTA-34)&127];
			fTempPerm1063 = fVec6[(IOTA-27)&127];
			fTempPerm1064 = fVec6[(IOTA-25)&127];
			fTempPerm1065 = fVec6[(IOTA-23)&127];
			fTempPerm1066 = fVec6[(IOTA-22)&127];
			fTempPerm1067 = fVec6[(IOTA-19)&127];
			fTempPerm1068 = fVec6[(IOTA-18)&127];
			fTempPerm1069 = fVec6[(IOTA-16)&127];
			fTempPerm1070 = fVec6[(IOTA-15)&127];
			fTempPerm1071 = fVec6[(IOTA-12)&127];
			fTempPerm1072 = fVec6[(IOTA-11)&127];
			fTempPerm1073 = fVec6[(IOTA-10)&127];
			fTempPerm1074 = fVec6[(IOTA-9)&127];
			fTempPerm1075 = fVec6[(IOTA-7)&127];
			fTempPerm1076 = fVec6[(IOTA-6)&127];
			fTempPerm1077 = fVec7[(IOTA-117)&127];
			fTempPerm1078 = fVec7[(IOTA-123)&127];
			fTempPerm1079 = fVec7[(IOTA-111)&127];
			fTempPerm1080 = fVec6[(IOTA-3)&127];
			fTempPerm1081 = fVec7[(IOTA-113)&127];
			fTempPerm1082 = fVec7[(IOTA-106)&127];
			fTempPerm1083 = fVec7[(IOTA-67)&127];
			fTempPerm1084 = fVec7[(IOTA-64)&127];
			fTempPerm1085 = fVec7[(IOTA-62)&127];
			fTempPerm1086 = fVec7[(IOTA-50)&127];
			fTempPerm1087 = fVec7[(IOTA-33)&127];
			fTempPerm1088 = fVec7[(IOTA-32)&127];
			fTempPerm1089 = fVec7[(IOTA-34)&127];
			fTempPerm1090 = fVec7[(IOTA-42)&127];
			fTempPerm1091 = fVec7[(IOTA-30)&127];
			fTempPerm1092 = fVec7[(IOTA-29)&127];
			fTempPerm1093 = fVec7[(IOTA-31)&127];
			fTempPerm1094 = fVec7[(IOTA-28)&127];
			fTempPerm1095 = fVec8[(IOTA-89)&127];
			fTempPerm1096 = fVec8[(IOTA-81)&127];
			fTempPerm1097 = fVec8[(IOTA-47)&127];
			fTempPerm1098 = fVec1[(IOTA-124)&127];
			fTempPerm1099 = fVec1[(IOTA-79)&127];
			fTempPerm1100 = fVec2[(IOTA-42)&127];
			fTempPerm1101 = fVec2[(IOTA-44)&127];
			fTempPerm1102 = fVec1[(IOTA-39)&127];
			fTempPerm1103 = fVec1[(IOTA-40)&127];
			fTempPerm1104 = fVec2[(IOTA-41)&127];
			fTempPerm1105 = fVec2[(IOTA-40)&127];
			fTempPerm1106 = fVec3[(IOTA-70)&127];
			fTempPerm1107 = fVec4[(IOTA-118)&127];
			fTempPerm1108 = fVec4[(IOTA-102)&127];
			fTempPerm1109 = fVec4[(IOTA-91)&127];
			fTempPerm1110 = fVec4[(IOTA-76)&127];
			fTempPerm1111 = fVec4[(IOTA-89)&127];
			fTempPerm1112 = fVec4[(IOTA-29)&127];
			fTempPerm1113 = fVec4[(IOTA-28)&127];
			fTempPerm1114 = fVec2[(IOTA-51)&127];
			fTempPerm1115 = fVec2[(IOTA-45)&127];
			fTempPerm1116 = fVec2[(IOTA-17)&127];
			fTempPerm1117 = fVec2[(IOTA-16)&127];
			fTempPerm1118 = fVec2[(IOTA-15)&127];
			fTempPerm1119 = fVec2[(IOTA-14)&127];
			fTempPerm1120 = fVec2[(IOTA-13)&127];
			fTempPerm1121 = fVec2[(IOTA-12)&127];
			fTempPerm1122 = fVec2[(IOTA-11)&127];
			fTempPerm1123 = fVec2[(IOTA-9)&127];
			fTempPerm1124 = fVec2[(IOTA-7)&127];
			fTempPerm1125 = fVec2[(IOTA-5)&127];
			fTempPerm1126 = fVec2[(IOTA-4)&127];
			fTempPerm1127 = fVec2[(IOTA-3)&127];
			fTempPerm1128 = fVec3[(IOTA-112)&127];
			fTempPerm1129 = fVec3[(IOTA-26)&127];
			fTempPerm1130 = fVec4[(IOTA-80)&127];
			fTempPerm1131 = fVec5[(IOTA-112)&127];
			fTempPerm1132 = fVec5[(IOTA-70)&127];
			fTempPerm1133 = fVec0[(IOTA-100)&127];
			fTempPerm1134 = fVec0[(IOTA-21)&127];
			fTempPerm1135 = fVec2[(IOTA-50)&127];
			fTempPerm1136 = fVec2[(IOTA-67)&127];
			fTempPerm1137 = fVec2[(IOTA-62)&127];
			fTempPerm1138 = fVec2[(IOTA-49)&127];
			fTempPerm1139 = fVec2[(IOTA-48)&127];
			fTempPerm1140 = fVec3[(IOTA-16)&127];
			fTempPerm1141 = fVec4[(IOTA-115)&127];
			fTempPerm1142 = fVec5[(IOTA-71)&127];
			fTempPerm1143 = fVec6[(IOTA-56)&127];
			fTempPerm1144 = fVec7[(IOTA-91)&127];
			fTempPerm1145 = fVec0[(IOTA-73)&127];
			fTempPerm1146 = fVec3[(IOTA-34)&127];
			fTempPerm1147 = fVec4[(IOTA-69)&127];
			fTempPerm1148 = fVec5[(IOTA-105)&127];
			fTempPerm1149 = fVec6[(IOTA-49)&127];
			fTempPerm1150 = fVec7[(IOTA-108)&127];
			fTempPerm1151 = fVec7[(IOTA-115)&127];
			fTempPerm1152 = (fRec1[0] * (((1.0279e-06 * fTempPerm2) + ((4.34695e-05 * fTempPerm3) + ((5.611290000000001e-05 * fTempPerm4) + ((0.0011606799999999999 * fTempPerm5) + ((3.61728e-05 * fTempPerm6) + ((0.00316299 * fTempPerm7) + ((0.00534785 * fTempPerm8) + ((0.0036506200000000003 * fTempPerm9) + ((0.00160415 * fTempPerm10) + ((0.018119700000000002 * fTempPerm11) + ((2.773e-06 * fTempPerm14) + ((0.000247962 * fTempPerm15) + ((0.000142212 * fTempPerm16) + ((0.00070562 * fTempPerm17) + ((0.00143667 * fTempPerm18) + ((0.00124614 * fTempPerm19) + ((0.000700973 * fTempPerm20) + ((6.29365e-05 * fTempPerm21) + ((0.0014662800000000001 * fTempPerm22) + ((0.000547636 * fTempPerm23) + ((0.0235272 * fTempPerm24) + ((0.0005180720000000001 * fTempPerm25) + ((0.0021948500000000004 * fTempPerm26) + ((0.009433120000000001 * fTempPerm27) + ((0.0071841 * fTempPerm28) + ((0.000285357 * fTempPerm29) + ((0.0007114370000000001 * fTempPerm30) + ((3.0215200000000005e-05 * fTempPerm33) + ((0.000204718 * fTempPerm34) + ((0.00058267 * fTempPerm35) + ((3.56328e-05 * fTempPerm36) + ((3.64663e-05 * fTempPerm37) + ((0.00015598 * fTempPerm38) + ((0.000261474 * fTempPerm39) + ((0.00121078 * fTempPerm40) + ((0.0007687070000000001 * fTempPerm41) + ((0.00271718 * fTempPerm42) + ((3.35889e-06 * fTempPerm45) + ((3.98048e-06 * fTempPerm46) + ((0.00040572 * fTempPerm47) + ((0.000530096 * fTempPerm48) + ((0.00122438 * fTempPerm49) + ((0.0011330000000000001 * fTempPerm50) + ((0.00046662400000000006 * fTempPerm51) + ((0.0007193410000000001 * fTempPerm52) + ((0.00040184499999999996 * fTempPerm53) + ((0.0026214800000000003 * fTempPerm54) + ((0.0005897750000000001 * fTempPerm55) + ((0.0014054500000000001 * fTempPerm56) + ((0.000219724 * fTempPerm57) + ((0.0017431900000000001 * fTempPerm58) + ((0.00184624 * fTempPerm59) + ((0.00215198 * fTempPerm60) + ((0.0047945900000000005 * fTempPerm61) + ((0.0038023500000000003 * fTempPerm62) + ((0.0038672100000000003 * fTempPerm63) + ((3.8319600000000005e-05 * fTempPerm64) + ((0.000144938 * fTempPerm65) + ((1.06705e-06 * fTempPerm66) + ((7.264470000000001e-05 * fTempPerm67) + ((0.036438700000000004 * fTempPerm68) + ((0.0273854 * fTempPerm69) + ((0.00833056 * fTempPerm70) + ((0.00481083 * fTempPerm71) + ((0.049985600000000005 * fTempPerm72) + ((0.0409783 * fTempPerm73) + ((0.0387707 * fTempPerm74) + ((0.04177190000000001 * fTempPerm75) + ((0.0551774 * fTempPerm76) + ((0.0108086 * fTempPerm77) + ((0.0026607500000000004 * fTempPerm78) + ((0.000669039 * fTempPerm79) + ((0.00135568 * fTempPerm80) + ((0.000128073 * fTempPerm81) + ((0.000378556 * fTempPerm82) + ((5.3135e-06 * fTempPerm83) + ((5.2591200000000006e-05 * fTempPerm84) + ((0.000320771 * fTempPerm85) + ((0.00035608600000000003 * fTempPerm86) + ((0.000344038 * fTempPerm87) + ((0.0005151 * fTempPerm88) + ((0.0022713 * fTempPerm89) + ((0.00361819 * fTempPerm90) + ((0.00108661 * fTempPerm91) + ((0.00343243 * fTempPerm92) + ((0.00288148 * fTempPerm93) + ((0.002183 * fTempPerm94) + ((0.0030387 * fTempPerm95) + ((0.021556799999999997 * fTempPerm96) + ((8.963050000000002e-08 * fTempPerm97) + ((6.092990000000001e-05 * fTempPerm98) + ((0.00048677700000000003 * fTempPerm99) + ((0.000646904 * fTempPerm100) + ((0.00215115 * fTempPerm101) + ((0.000551147 * fTempPerm102) + ((0.00703939 * fTempPerm103) + ((0.00489365 * fTempPerm104) + ((0.0234635 * fTempPerm105) + ((0.0359968 * fTempPerm106) + ((2.0105900000000003e-05 * fTempPerm107) + ((0.000260275 * fTempPerm108) + ((0.000692291 * fTempPerm109) + ((0.0006938150000000001 * fTempPerm110) + ((0.0008425430000000001 * fTempPerm111) + ((0.0030116200000000004 * fTempPerm112) + ((0.000903587 * fTempPerm113) + ((0.000231406 * fTempPerm114) + ((6.18201e-06 * fTempPerm117) + ((3.8470500000000004e-05 * fTempPerm118) + ((0.00190242 * fTempPerm119) + ((0.00116141 * fTempPerm120) + ((0.0014712700000000002 * fTempPerm121) + ((0.00131455 * fTempPerm122) + ((0.00262382 * fTempPerm123) + ((0.0016339300000000002 * fTempPerm124) + ((2.45331e-06 * fTempPerm127) + ((1.70273e-06 * fTempPerm128) + ((8.03417e-06 * fTempPerm129) + ((3.882760000000001e-05 * fTempPerm130) + ((0.000105639 * fTempPerm131) + ((0.000682534 * fTempPerm132) + ((0.00527862 * fTempPerm133) + ((0.00154417 * fTempPerm134) + ((0.0014036900000000002 * fTempPerm135) + ((0.0010312 * fTempPerm136) + ((0.00113331 * fTempPerm137) + ((0.00146986 * fTempPerm138) + ((0.0007088390000000001 * fTempPerm139) + ((0.000702438 * fTempPerm140) + ((0.005367289999999999 * fTempPerm141) + ((8.88365e-06 * fTempPerm144) + ((8.00668e-05 * fTempPerm145) + ((9.942610000000001e-05 * fTempPerm146) + ((9.50248e-07 * fTempPerm147) + ((2.43757e-05 * fTempPerm148) + ((5.11494e-05 * fTempPerm149) + ((0.00341282 * fTempPerm150) + ((0.00600683 * fTempPerm151) + ((0.0011593500000000002 * fTempPerm152) + ((0.0016691100000000001 * fTempPerm153) + ((0.0056765999999999995 * fTempPerm154) + ((0.000259004 * fTempPerm155) + ((0.00279092 * fTempPerm156) + ((0.00130772 * fTempPerm157) + ((0.00214275 * fTempPerm158) + ((0.00243588 * fTempPerm159) + ((1.1796500000000001e-05 * fTempPerm162) + ((0.000709739 * fTempPerm163) + ((0.0004561410000000001 * fTempPerm164) + ((0.000922271 * fTempPerm165) + ((0.000813006 * fTempPerm166) + ((0.0022532999999999997 * fTempPerm167) + ((0.000782042 * fTempPerm168) + ((0.00089492 * fTempPerm169) + ((0.00207133 * fTempPerm170) + ((0.00287405 * fTempPerm171) + ((0.0039108500000000004 * fTempPerm172) + ((0.00124406 * fTempPerm173) + ((0.00241144 * fTempPerm174) + ((0.0054627 * fTempPerm175) + ((0.10758200000000001 * fTempPerm176) + ((4.94959e-07 * fTempPerm177) + ((2.4558300000000005e-05 * fTempPerm178) + ((0.000253833 * fTempPerm179) + ((9.065480000000002e-05 * fTempPerm180) + ((0.00018337600000000001 * fTempPerm181) + ((0.00221192 * fTempPerm182) + ((0.000662261 * fTempPerm183) + ((0.00221665 * fTempPerm184) + ((0.0027539400000000003 * fTempPerm185) + ((0.000204676 * fTempPerm186) + ((0.0035849099999999997 * fTempPerm187) + ((0.0021549399999999997 * fTempPerm188) + ((0.011300600000000001 * fTempPerm189) + ((0.000278137 * fTempPerm190) + ((0.024944 * fTempPerm191) + ((0.000337973 * fTempPerm192) + ((0.00045785700000000004 * fTempPerm193) + ((0.000561379 * fTempPerm194) + ((6.25862e-05 * fTempPerm195) + ((0.00117567 * fTempPerm196) + ((0.00110439 * fTempPerm197) + ((0.0016155 * fTempPerm198) + ((0.0158006 * fTempPerm199) + ((0.0118865 * fTempPerm200) + ((0.005873929999999999 * fTempPerm201) + ((0.024751 * fTempPerm202) + ((0.00867306 * fTempPerm203) + ((7.760860000000001e-05 * fTempPerm204) + ((0.00957015 * fTempPerm205) + ((0.00020361699999999999 * fTempPerm206) + ((0.000506744 * fTempPerm207) + ((0.0004971870000000001 * fTempPerm208) + ((3.26233e-05 * fTempPerm209) + ((0.000380719 * fTempPerm210) + ((0.00156339 * fTempPerm211) + ((0.0034697200000000004 * fTempPerm212) + ((0.0005514300000000001 * fTempPerm213) + ((0.00167233 * fTempPerm214) + ((0.0034494400000000002 * fTempPerm215) + ((0.000754655 * fTempPerm216) + ((0.00145329 * fTempPerm217) + ((0.0011545000000000001 * fTempPerm218) + ((0.000647084 * fTempPerm219) + ((0.0010632999999999999 * fTempPerm220) + ((0.00150508 * fTempPerm221) + ((0.00290687 * fTempPerm222) + ((0.00596596 * fTempPerm223) + ((0.0027371 * fTempPerm224) + ((4.61844e-05 * fTempPerm225) + ((0.0025209700000000004 * fTempPerm226) + ((0.000368557 * fTempPerm227) + ((0.000227166 * fTempPerm228) + ((8.214079999999998e-06 * fTempPerm229) + ((0.000702765 * fTempPerm230) + ((2.51378e-07 * fTempPerm231) + ((8.2866e-05 * fTempPerm232) + ((0.000145149 * fTempPerm233) + ((0.00010655300000000001 * fTempPerm234) + ((0.00036701 * fTempPerm235) + ((0.000351382 * fTempPerm236) + ((7.614930000000001e-05 * fTempPerm237) + ((0.00147938 * fTempPerm238) + ((0.00160801 * fTempPerm239) + ((0.000131157 * fTempPerm240) + ((0.010230900000000001 * fTempPerm241) + ((0.0073240200000000005 * fTempPerm242) + ((0.00732389 * fTempPerm243) + ((0.00691979 * fTempPerm244) + ((0.0034125200000000005 * fTempPerm245) + ((0.0316732 * fTempPerm246) + ((0.0274497 * fTempPerm247) + ((0.00309089 * fTempPerm248) + ((0.0103427 * fTempPerm249) + ((0.0120915 * fTempPerm250) + ((0.00389043 * fTempPerm251) + ((0.00247242 * fTempPerm252) + ((0.0012886100000000001 * fTempPerm253) + ((0.0008485659999999999 * fTempPerm254) + ((0.000202228 * fTempPerm255) + ((3.85491e-05 * fTempPerm256) + ((0.000184012 * fTempPerm257) + ((0.000562856 * fTempPerm258) + ((0.0016634199999999999 * fTempPerm259) + ((0.03066 * fTempPerm260) + ((0.0019489700000000002 * fTempPerm261) + ((0.0174746 * fTempPerm262) + ((0.0211909 * fTempPerm263) + ((0.00253457 * fTempPerm264) + ((0.0025992000000000003 * fTempPerm265) + ((0.00618924 * fTempPerm266) + ((0.00201002 * fTempPerm267) + ((0.00148817 * fTempPerm268) + ((0.000534434 * fTempPerm269) + ((0.00069471 * fTempPerm270) + ((0.00013874000000000002 * fTempPerm271) + ((7.74527e-05 * fTempPerm272) + ((0.0008817050000000001 * fTempPerm273) + ((0.00359199 * fTempPerm274) + ((0.000324763 * fTempPerm275) + ((0.0019519 * fTempPerm276) + ((8.464169999999999e-06 * fTempPerm277) + ((8.79674e-06 * fTempPerm278) + ((0.0043689900000000005 * fTempPerm279) + ((0.00375801 * fTempPerm280) + ((3.3718100000000005e-05 * fTempPerm281) + ((0.000995482 * fTempPerm282) + ((0.00918714 * fTempPerm283) + ((0.00421731 * fTempPerm284) + ((0.0022329500000000005 * fTempPerm285) + ((0.0028589500000000003 * fTempPerm286) + ((0.0070971 * fTempPerm287) + ((0.021576300000000003 * fTempPerm288) + ((0.0216851 * fTempPerm289) + ((0.0004323680000000001 * fTempPerm290) + ((0.000705385 * fTempPerm291) + ((0.00136595 * fTempPerm292) + ((0.00214314 * fTempPerm293) + ((5.360590000000001e-05 * fTempPerm294) + ((0.000264467 * fTempPerm295) + ((0.0027816299999999998 * fTempPerm296) + ((0.000884337 * fTempPerm297) + ((0.0005879310000000001 * fTempPerm298) + ((0.000419816 * fTempPerm299) + ((0.0013345100000000001 * fTempPerm300) + ((0.00177653 * fTempPerm301) + ((0.0016152100000000002 * fTempPerm302) + ((0.00194177 * fTempPerm303) + ((0.00114689 * fTempPerm304) + ((0.0005869030000000001 * fTempPerm305) + ((0.00122991 * fTempPerm306) + ((5.0813899999999995e-06 * fTempPerm309) + ((5.712609999999999e-06 * fTempPerm310) + ((2.09787e-05 * fTempPerm311) + ((0.000106546 * fTempPerm312) + ((0.0003416 * fTempPerm313) + ((0.00136699 * fTempPerm314) + ((0.0012065399999999999 * fTempPerm315) + ((0.0008772760000000001 * fTempPerm316) + ((0.00054874 * fTempPerm317) + ((0.000981663 * fTempPerm318) + ((0.0005068340000000001 * fTempPerm319) + ((0.000584702 * fTempPerm320) + ((0.00272945 * fTempPerm321) + ((0.000563692 * fTempPerm322) + ((0.00637431 * fTempPerm323) + ((0.00308689 * fTempPerm324) + ((0.00523563 * fTempPerm325) + ((0.0014647 * fTempPerm326) + ((0.00225593 * fTempPerm327) + ((0.00342846 * fTempPerm328) + ((0.00623851 * fTempPerm329) + ((0.0064973 * fTempPerm330) + ((0.0079812 * fTempPerm331) + ((4.2462999999999995e-06 * fVec1[IOTA&127]) + ((0.0266261 * fTempPerm332) + ((4.41782e-05 * fTempPerm333) + ((1.57811e-05 * fVec3[IOTA&127]) + ((0.000623519 * fTempPerm334) + ((0.0016778 * fTempPerm335) + ((0.0010498400000000002 * fTempPerm336) + ((0.0214629 * fTempPerm337) + ((0.0293457 * fTempPerm338) + ((0.08895479999999999 * fTempPerm339) + ((0.0636584 * fTempPerm340) + ((0.0781434 * fTempPerm341) + ((0.0608862 * fTempPerm342) + ((0.0771319 * fTempPerm343) + ((0.00545812 * fTempPerm344) + ((0.0905515 * fTempPerm345) + ((0.00181104 * fTempPerm346) + ((0.000258553 * fTempPerm347) + ((0.000453824 * fTempPerm348) + ((0.000533584 * fTempPerm349) + ((0.000676256 * fTempPerm350) + ((0.00185348 * fTempPerm351) + ((0.00154024 * fTempPerm352) + ((1.8717900000000003e-05 * fTempPerm353) + ((4.2541300000000006e-05 * fTempPerm354) + ((0.0008345150000000001 * fTempPerm355) + ((0.00500282 * fTempPerm356) + ((0.0012118500000000002 * fTempPerm357) + ((0.0041905200000000005 * fTempPerm358) + ((0.0024405 * fTempPerm359) + ((0.000828477 * fTempPerm360) + ((0.00124616 * fTempPerm361) + ((0.00305273 * fTempPerm362) + ((0.00288578 * fTempPerm363) + ((0.00743299 * fTempPerm364) + ((0.00870641 * fTempPerm365) + ((0.030624600000000002 * fTempPerm366) + ((0.0528391 * fTempPerm367) + ((0.0317463 * fTempPerm368) + ((0.000767728 * fTempPerm369) + ((0.00059949 * fTempPerm370) + ((0.0009683840000000001 * fTempPerm371) + ((4.87482e-05 * fTempPerm372) + ((0.0007838960000000001 * fTempPerm373) + ((0.000535477 * fTempPerm374) + ((0.000985821 * fTempPerm375) + ((0.00185354 * fTempPerm376) + ((0.0006661270000000001 * fTempPerm377) + ((0.000214717 * fTempPerm378) + ((2.84575e-06 * fTempPerm379) + ((0.00010309 * fTempPerm380) + ((2.0687900000000002e-05 * fTempPerm381) + ((0.000749201 * fTempPerm382) + ((3.8066800000000006e-05 * fTempPerm383) + ((0.00127749 * fTempPerm384) + ((2.49726e-05 * fTempPerm385) + ((5.2352700000000004e-05 * fTempPerm386) + ((0.000400805 * fTempPerm387) + ((2.7286800000000004e-05 * fTempPerm388) + ((0.0025276200000000004 * fTempPerm389) + ((0.0013761300000000001 * fTempPerm390) + ((0.00027819 * fTempPerm391) + ((0.000339552 * fTempPerm392) + ((0.00022609 * fTempPerm393) + ((0.000160215 * fTempPerm394) + ((0.000240886 * fTempPerm395) + ((0.00459555 * fTempPerm396) + ((0.000100847 * fTempPerm397) + ((0.000135904 * fTempPerm398) + ((0.0007649050000000001 * fTempPerm399) + ((0.000208667 * fTempPerm400) + ((0.00257166 * fTempPerm401) + ((0.000824869 * fTempPerm402) + ((0.0011506799999999998 * fTempPerm403) + ((0.0012521400000000001 * fTempPerm404) + ((0.00143222 * fTempPerm405) + ((0.00046067700000000005 * fTempPerm406) + ((0.0019439 * fTempPerm407) + ((0.013752100000000001 * fTempPerm408) + ((0.0018844 * fTempPerm409) + ((0.0448946 * fTempPerm410) + ((0.000300808 * fTempPerm411) + ((0.00162719 * fTempPerm412) + ((3.43292e-08 * fTempPerm413) + ((0.00020187200000000002 * fTempPerm414) + ((0.0017063 * fTempPerm415) + ((0.000681816 * fTempPerm416) + ((0.00133959 * fTempPerm417) + ((0.0006117130000000001 * fTempPerm418) + ((0.000609699 * fTempPerm419) + ((0.0005591420000000001 * fTempPerm420) + ((8.84199e-05 * fTempPerm421) + ((0.000206941 * fTempPerm422) + ((0.000292148 * fTempPerm423) + ((0.00356967 * fTempPerm424) + ((0.000554582 * fTempPerm425) + ((4.1971900000000004e-05 * fTempPerm426) + ((0.00601814 * fTempPerm427) + ((9.096330000000001e-05 * fTempPerm428) + ((8.7317e-05 * fTempPerm429) + ((0.000204282 * fTempPerm430) + ((5.25869e-05 * fTempPerm431) + ((0.000254261 * fTempPerm432) + ((0.000632642 * fTempPerm433) + ((8.71167e-05 * fTempPerm434) + ((0.00205137 * fTempPerm435) + ((0.00160511 * fTempPerm436) + ((0.00798082 * fTempPerm437) + ((0.000101833 * fTempPerm438) + ((0.00110736 * fTempPerm439) + ((0.00018456300000000003 * fTempPerm440) + ((0.000208879 * fTempPerm441) + ((0.00444323 * fTempPerm442) + ((0.00195891 * fTempPerm443) + ((0.00206635 * fTempPerm444) + ((0.00288332 * fTempPerm445) + ((0.00571347 * fTempPerm446) + ((0.00814256 * fTempPerm447) + ((0.00229129 * fTempPerm448) + ((2.40821e-06 * fTempPerm449) + ((9.081670000000001e-05 * fTempPerm450) + ((0.0036832600000000003 * fTempPerm451) + ((0.000383377 * fTempPerm452) + ((0.0016079900000000001 * fTempPerm453) + ((0.00119739 * fTempPerm454) + ((0.0008951980000000002 * fTempPerm455) + ((0.000791254 * fTempPerm456) + ((0.000331327 * fTempPerm457) + ((2.3648600000000003e-05 * fTempPerm458) + ((0.00010376700000000001 * fTempPerm459) + ((0.050635700000000006 * fTempPerm460) + ((0.058508199999999996 * fTempPerm461) + ((0.0413412 * fTempPerm462) + ((2.06253e-06 * fTempPerm463) + ((0.005175760000000001 * fTempPerm464) + ((0.00243653 * fTempPerm465) + ((0.00185049 * fTempPerm466) + ((0.0023089200000000003 * fTempPerm467) + ((0.00274035 * fTempPerm468) + ((0.00422506 * fTempPerm469) + ((0.00312119 * fTempPerm470) + ((0.00693019 * fTempPerm471) + ((0.00998562 * fTempPerm472) + ((0.0013329 * fTempPerm473) + ((0.0016006100000000001 * fTempPerm474) + ((0.00991536 * fTempPerm475) + ((0.0167235 * fTempPerm476) + ((0.013603 * fTempPerm477) + ((3.82797e-06 * fTempPerm478) + ((3.872240000000001e-05 * fTempPerm479) + ((8.327320000000001e-05 * fTempPerm480) + ((0.00018379600000000001 * fTempPerm481) + ((0.00155754 * fTempPerm482) + ((0.00014839199999999998 * fTempPerm483) + ((9.226730000000001e-05 * fTempPerm484) + ((0.0053560100000000005 * fTempPerm485) + ((0.0053044 * fTempPerm486) + ((0.0008625190000000001 * fTempPerm487) + ((3.9296600000000006e-05 * fTempPerm488) + ((0.00291413 * fTempPerm489) + ((0.00317138 * fTempPerm490) + ((0.00337025 * fTempPerm491) + ((0.0012247 * fTempPerm492) + ((0.00117636 * fTempPerm493) + ((0.00534772 * fTempPerm494) + ((0.00681093 * fTempPerm495) + ((0.00503769 * fTempPerm496) + ((0.00336909 * fTempPerm497) + ((0.00483835 * fTempPerm498) + ((1.73416e-05 * fTempPerm499) + ((0.0012872900000000002 * fTempPerm500) + ((0.00169945 * fTempPerm501) + ((0.0008932800000000001 * fTempPerm502) + ((0.0020680200000000003 * fTempPerm503) + ((0.00028545500000000005 * fTempPerm504) + ((0.000721582 * fTempPerm505) + ((0.00162146 * fTempPerm506) + ((0.0007790860000000001 * fTempPerm507) + ((0.00301503 * fTempPerm508) + ((0.0053105900000000004 * fTempPerm509) + ((0.0164669 * fTempPerm510) + ((0.0005342939999999999 * fTempPerm511) + ((0.00376608 * fTempPerm512) + ((0.00579697 * fTempPerm513) + ((0.008648310000000001 * fTempPerm514) + ((0.00576228 * fTempPerm515) + ((0.0197594 * fTempPerm516) + ((0.0121098 * fTempPerm517) + ((0.0325354 * fTempPerm518) + ((0.000503397 * fTempPerm519) + ((0.000868029 * fTempPerm520) + ((0.00165835 * fTempPerm521) + ((0.00031507000000000003 * fTempPerm522) + ((0.000494618 * fTempPerm523) + ((0.000158404 * fTempPerm524) + ((0.00046969700000000004 * fTempPerm525) + ((0.0004770400000000001 * fTempPerm526) + ((0.0013671599999999999 * fTempPerm527) + ((0.00300031 * fTempPerm528) + ((0.000168725 * fTempPerm529) + ((0.000547366 * fTempPerm530) + ((0.00050102 * fTempPerm531) + ((0.0014059200000000002 * fTempPerm532) + ((0.00214615 * fTempPerm533) + ((0.0009436510000000001 * fTempPerm534) + ((0.0006128250000000001 * fTempPerm535) + ((2.3955000000000004e-05 * fTempPerm536) + ((0.00041329900000000006 * fTempPerm537) + ((0.004194580000000001 * fTempPerm538) + ((0.00157017 * fTempPerm539) + ((0.0029129100000000003 * fTempPerm540) + ((0.00321075 * fTempPerm541) + ((0.0037083100000000003 * fTempPerm542) + ((0.006804070000000001 * fTempPerm543) + ((0.0035775299999999998 * fTempPerm544) + ((0.00361293 * fTempPerm545) + ((0.00325793 * fTempPerm546) + ((0.0028834 * fTempPerm547) + ((0.00366525 * fTempPerm548) + ((0.0008637700000000001 * fTempPerm549) + ((0.00306785 * fTempPerm550) + ((0.00175234 * fTempPerm551) + ((0.0042153 * fTempPerm552) + ((0.00250454 * fTempPerm553) + ((0.015005800000000001 * fTempPerm554) + ((0.00792482 * fTempPerm555) + ((0.0212609 * fTempPerm556) + ((0.09689510000000001 * fTempPerm557) + ((0.057809200000000005 * fTempPerm558) + ((0.06596840000000001 * fTempPerm559) + ((0.0557725 * fTempPerm560) + ((0.034514800000000005 * fTempPerm561) + ((0.07349510000000001 * fTempPerm562) + ((0.0647491 * fTempPerm563) + ((0.000141179 * fTempPerm564) + ((0.0018634200000000002 * fTempPerm565) + ((0.00024957099999999997 * fTempPerm566) + ((0.00115892 * fTempPerm567) + ((0.000508795 * fTempPerm568) + ((0.0036302599999999997 * fTempPerm569) + ((0.0565104 * fTempPerm570) + ((0.05960650000000001 * fTempPerm571) + ((0.049342300000000006 * fTempPerm572) + ((0.0449074 * fTempPerm573) + ((2.00167e-05 * fTempPerm574) + ((1.4622100000000002e-05 * fVec4[IOTA&127]) + ((2.49699e-05 * fTempPerm575) + ((9.93628e-06 * fVec5[IOTA&127]) + ((2.8519800000000004e-05 * fTempPerm576) + ((4.45788e-06 * fVec6[IOTA&127]) + ((0.00015469 * fTempPerm577) + ((3.39551e-05 * fVec7[IOTA&127]) + ((0.000123708 * fTempPerm578) + ((2.5118200000000003e-05 * fVec8[IOTA&127]) + ((0.021392099999999997 * fTempPerm579) + ((0.00127111 * fTempPerm580) + ((0.00196889 * fTempPerm581) + ((((((((((((((0.0105234 * fTempPerm582) + ((9.40654e-05 * fTempPerm583) + ((0.0032713800000000004 * fTempPerm584) + ((0.00108099 * fTempPerm585) + ((0.000164044 * fTempPerm586) + ((0.0012923000000000001 * fTempPerm587) + (((0.0056945699999999995 * fTempPerm588) + ((0.011589100000000001 * fTempPerm589) + ((0.0527583 * fTempPerm590) + ((0.0124978 * fTempPerm591) + ((0.0109229 * fTempPerm592) + ((0.045127499999999994 * fTempPerm593) + ((0.0005077079999999999 * fTempPerm594) + ((0.0005051960000000001 * fTempPerm595) + ((0.036282299999999996 * fTempPerm596) + ((0.00967744 * fTempPerm597) + ((5.80835e-05 * fTempPerm598) + ((6.798480000000001e-05 * fTempPerm599) + ((0.00046795 * fTempPerm600) + ((0.0141227 * fTempPerm601) + ((0.00446807 * fTempPerm602) + ((0.0200236 * fTempPerm603) + ((0.023001999999999998 * fTempPerm604) + ((0.00570825 * fTempPerm605) + ((0.011366300000000001 * fTempPerm606) + ((4.86084e-05 * fTempPerm607) + ((0.00255904 * fTempPerm608) + ((0.004904550000000001 * fTempPerm609) + ((0.00914287 * fTempPerm610) + ((2.11859e-06 * fTempPerm611) + ((0.0013118799999999999 * fTempPerm612) + ((0.000367688 * fTempPerm613) + ((0.00045975200000000007 * fTempPerm614) + (0.0006954530000000001 * fTempPerm615)))))))))))))))))))))))))))) + (0.0016799500000000001 * fTempPerm616)))))))) + (0.00047882 * fTempPerm617)) + (0.00129144 * fTempPerm618)) + (2.72483e-05 * fTempPerm619)) + (5.07898e-05 * fTempPerm620)) + (0.000257463 * fTempPerm621)) + (0.00101332 * fTempPerm622)) + (0.000946613 * fTempPerm623)) + (0.00255288 * fTempPerm624)) + (0.000331593 * fTempPerm625)) + (0.000307859 * fTempPerm626)) + (0.000341662 * fTempPerm627)) + (0.0010638000000000002 * fTempPerm628))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) - ((5.52174e-06 * fTempPerm629) + ((9.036830000000001e-05 * fTempPerm630) + ((4.7581900000000006e-05 * fTempPerm631) + ((3.0118399999999995e-06 * fTempPerm632) + ((0.000918475 * fTempPerm633) + ((2.11033e-06 * fTempPerm634) + ((3.11789e-06 * fTempPerm635) + ((1.965e-06 * fTempPerm636) + ((0.00021442500000000002 * fTempPerm637) + ((0.000141728 * (fTempPerm638 - fTempPerm639)) + ((0.00213426 * fTempPerm640) + ((0.000169249 * fTempPerm641) + ((0.000342912 * fTempPerm642) + ((0.000249663 * fTempPerm643) + ((0.00225807 * fTempPerm644) + ((0.0010079800000000001 * fTempPerm645) + ((0.0024688500000000003 * fTempPerm646) + ((0.00320891 * fTempPerm647) + ((0.00277838 * fTempPerm648) + ((0.00761051 * fTempPerm649) + ((0.00700634 * fTempPerm650) + ((0.0112762 * fTempPerm651) + ((0.00613615 * fTempPerm652) + ((0.0077227400000000005 * fTempPerm653) + ((9.6016e-06 * fTempPerm654) + ((7.442140000000001e-05 * fTempPerm655) + ((9.82888e-05 * fTempPerm656) + ((9.07505e-05 * fTempPerm657) + ((0.00200683 * fTempPerm658) + ((0.00020293100000000003 * fTempPerm659) + ((0.00042257 * fTempPerm660) + ((0.0016720200000000002 * fTempPerm661) + ((0.0107983 * fTempPerm662) + ((0.0115642 * fTempPerm663) + ((0.0110708 * fTempPerm664) + ((0.0074312 * fTempPerm665) + ((0.000372384 * fTempPerm666) + ((0.00252045 * fTempPerm667) + ((9.995620000000002e-05 * fTempPerm668) + ((8.4151e-06 * fTempPerm669) + ((2.8368800000000002e-05 * fTempPerm670) + ((6.11718e-05 * fTempPerm671) + ((0.00010922300000000001 * fTempPerm672) + ((8.460210000000001e-05 * fTempPerm673) + ((0.00192194 * fTempPerm674) + ((0.0013940900000000002 * fTempPerm675) + ((0.00100778 * fTempPerm676) + ((0.00035581800000000004 * fTempPerm677) + ((0.00311109 * fTempPerm678) + ((0.00419002 * fTempPerm679) + ((0.0073180300000000005 * fTempPerm680) + ((0.00717239 * fTempPerm681) + ((0.00701442 * fTempPerm682) + ((3.06573e-05 * fTempPerm683) + ((0.0023363800000000003 * fTempPerm684) + ((0.00476244 * fTempPerm685) + ((0.00041918 * fTempPerm686) + ((0.00193753 * fTempPerm687) + ((0.000359638 * fTempPerm688) + ((0.00351467 * fTempPerm689) + ((0.00313597 * fTempPerm690) + ((0.00337746 * fTempPerm691) + ((0.0017748 * fTempPerm692) + ((0.000539206 * fTempPerm693) + ((0.00113754 * fTempPerm694) + ((0.0013005599999999999 * fTempPerm695) + ((0.0017591500000000001 * fTempPerm696) + ((0.00200773 * fTempPerm697) + ((0.0010940099999999999 * fTempPerm698) + ((0.0044288800000000005 * fTempPerm699) + ((0.0102805 * fTempPerm700) + ((0.000630871 * fTempPerm701) + ((0.0037140800000000002 * fTempPerm702) + ((0.0249734 * fTempPerm703) + ((0.0028042699999999998 * fTempPerm704) + ((0.00554456 * fTempPerm705) + ((0.043969699999999994 * fTempPerm706) + ((0.010103500000000001 * fTempPerm707) + ((0.026084999999999997 * fTempPerm708) + ((0.0405775 * fTempPerm709) + ((0.0703236 * fTempPerm710) + ((0.0864246 * fTempPerm711) + ((0.00526349 * fTempPerm712) + ((0.0021171000000000002 * fTempPerm713) + ((0.000272118 * fTempPerm714) + ((0.0007430550000000001 * fTempPerm715) + ((0.000701652 * fTempPerm716) + ((7.66841e-06 * fTempPerm717) + ((0.000165695 * fTempPerm718) + ((0.0005041500000000001 * fTempPerm719) + ((0.00041316100000000005 * fTempPerm720) + ((0.00119961 * fTempPerm721) + ((0.00186912 * fTempPerm722) + ((0.00116132 * fTempPerm723) + ((0.00164123 * fTempPerm724) + ((0.0019447000000000002 * fTempPerm725) + ((0.0035543000000000003 * fTempPerm726) + ((0.00454218 * fTempPerm727) + ((0.00552124 * fTempPerm728) + ((0.00320578 * fTempPerm729) + ((0.0008197110000000001 * fTempPerm730) + ((0.015326500000000002 * fTempPerm731) + ((0.00390009 * fTempPerm732) + ((0.000997929 * fTempPerm733) + ((0.00128336 * fTempPerm734) + ((0.00655248 * fTempPerm735) + ((0.0128167 * fTempPerm736) + ((1.03751e-05 * fTempPerm737) + ((2.7205700000000003e-05 * fTempPerm738) + ((0.000475105 * fTempPerm739) + ((0.00581921 * fTempPerm740) + ((0.00026075800000000004 * fTempPerm741) + ((9.02966e-06 * fTempPerm742) + ((0.00027007400000000003 * fTempPerm743) + ((0.000492681 * fTempPerm744) + ((0.00110453 * fTempPerm745) + ((0.0007697240000000001 * fTempPerm746) + ((5.8659e-05 * fTempPerm747) + ((0.00241033 * fTempPerm748) + ((0.00190039 * fTempPerm749) + ((0.00136039 * fTempPerm750) + ((0.000343024 * fTempPerm751) + ((0.00396492 * fTempPerm752) + ((0.000839627 * fTempPerm753) + ((0.000711991 * fTempPerm754) + ((0.000103579 * fTempPerm755) + ((0.00124685 * fTempPerm756) + ((0.00124353 * fTempPerm757) + ((0.0010388900000000002 * fTempPerm758) + ((0.00170268 * fTempPerm759) + ((0.0008297610000000001 * fTempPerm760) + ((0.0030592 * fTempPerm761) + ((0.0008303280000000001 * fTempPerm762) + ((0.0015286800000000001 * fTempPerm763) + ((0.00146794 * fTempPerm764) + ((0.00299123 * fTempPerm765) + ((0.000255876 * fTempPerm766) + ((0.00149053 * fTempPerm767) + ((0.00299686 * fTempPerm768) + ((0.0120184 * fTempPerm769) + ((0.011691100000000001 * fTempPerm770) + ((0.0301367 * fTempPerm771) + ((0.0173412 * fTempPerm772) + ((0.000383553 * fTempPerm773) + ((0.00155276 * fTempPerm774) + ((0.00038862600000000004 * fTempPerm775) + ((0.0100647 * fTempPerm776) + ((0.00121329 * fTempPerm777) + ((0.00104971 * fTempPerm778) + ((5.444779999999999e-06 * fTempPerm779) + ((4.109880000000001e-05 * fTempPerm780) + ((0.00113341 * fTempPerm781) + ((6.52305e-05 * fTempPerm782) + ((0.000163854 * fTempPerm783) + ((4.81323e-05 * fTempPerm784) + ((0.000269357 * fTempPerm785) + ((0.00235077 * fTempPerm786) + ((0.00267142 * fTempPerm787) + ((0.000120611 * fTempPerm788) + ((0.00823842 * fTempPerm789) + ((0.005016619999999999 * fTempPerm790) + ((0.00317886 * fTempPerm791) + ((0.00323696 * fTempPerm792) + ((2.6161300000000003e-05 * fTempPerm793) + ((0.000131304 * fTempPerm794) + ((0.0008188190000000001 * fTempPerm795) + ((3.58722e-05 * fTempPerm796) + ((0.00112713 * fTempPerm797) + ((0.000818766 * fTempPerm798) + ((0.0018522 * fTempPerm799) + ((0.00985606 * fTempPerm800) + ((0.005366360000000001 * fTempPerm801) + ((3.5305499999999996e-06 * fTempPerm802) + ((4.115529999999999e-06 * fTempPerm803) + ((0.00010810400000000001 * fTempPerm804) + ((0.00013188 * fTempPerm805) + ((0.008342460000000001 * fTempPerm806) + ((0.00160587 * fTempPerm807) + ((0.00482279 * fTempPerm808) + ((0.00574575 * fTempPerm809) + ((9.0214e-05 * fTempPerm810) + ((0.00108387 * fTempPerm811) + ((0.00449214 * fTempPerm812) + ((0.000534437 * fTempPerm813) + ((0.000592974 * fTempPerm814) + ((0.00102436 * fTempPerm815) + ((0.00504437 * fTempPerm816) + ((0.009254680000000001 * fTempPerm817) + ((0.00104271 * fTempPerm818) + ((1.70851e-05 * fTempPerm819) + ((0.00507132 * fTempPerm820) + ((0.0114966 * fTempPerm821) + ((8.331490000000002e-05 * fTempPerm822) + ((0.00152143 * fTempPerm823) + ((0.000625519 * fTempPerm824) + ((0.0042904 * fTempPerm825) + ((0.0037726500000000002 * fTempPerm826) + ((0.0017495700000000002 * fTempPerm827) + ((0.00138811 * fTempPerm828) + ((0.00142376 * fTempPerm829) + ((0.00043131800000000003 * fTempPerm830) + ((0.0240448 * fTempPerm831) + ((0.011073200000000002 * fTempPerm832) + ((0.00016208700000000002 * fTempPerm833) + ((0.00023927800000000002 * fTempPerm834) + ((0.000998409 * fTempPerm835) + ((0.0011577500000000001 * fTempPerm836) + ((0.0010344 * fTempPerm837) + ((0.00544477 * fTempPerm838) + ((0.00274518 * fTempPerm839) + ((0.00304542 * fTempPerm840) + ((0.00192986 * fTempPerm841) + ((0.014927200000000002 * fTempPerm842) + ((0.010792999999999999 * fTempPerm843) + ((0.0223719 * fTempPerm844) + ((0.008510160000000001 * fTempPerm845) + ((0.0123166 * fTempPerm846) + ((0.0179474 * fTempPerm847) + ((0.007369410000000001 * fTempPerm848) + ((0.006754800000000001 * fTempPerm849) + ((0.0025108100000000005 * fTempPerm850) + ((0.00235191 * fTempPerm851) + ((0.0009197939999999999 * fTempPerm852) + ((0.000506998 * fTempPerm853) + ((0.0142655 * fTempPerm854) + ((9.829720000000001e-05 * fTempPerm855) + ((6.971659999999999e-06 * fTempPerm856) + ((0.000428973 * fTempPerm857) + ((0.00126004 * fTempPerm858) + ((0.00201638 * fTempPerm859) + ((0.0017147500000000001 * fTempPerm860) + ((0.00031563800000000003 * fTempPerm861) + ((0.025458500000000002 * fTempPerm862) + ((0.00530958 * fTempPerm863) + ((0.00721718 * fTempPerm864) + ((0.0030320200000000003 * fTempPerm865) + ((0.00131485 * fTempPerm866) + ((0.0011081700000000001 * fTempPerm867) + ((0.000362084 * fTempPerm868) + ((0.000177736 * fTempPerm869) + ((7.589380000000002e-05 * fTempPerm870) + ((0.0005503540000000001 * fTempPerm871) + ((0.0005423040000000001 * fTempPerm872) + ((0.00026040700000000004 * fTempPerm873) + ((0.0391701 * fTempPerm874) + ((0.000622009 * fTempPerm875) + ((0.000315111 * fTempPerm876) + ((0.00271694 * fTempPerm877) + ((0.000608302 * fTempPerm878) + ((0.00029472900000000003 * fTempPerm879) + ((0.0200181 * fTempPerm880) + ((0.0043256300000000004 * fTempPerm881) + ((2.56393e-06 * fTempPerm882) + ((0.00123749 * fTempPerm883) + ((0.0161655 * fTempPerm884) + ((8.318509999999999e-06 * fTempPerm885) + ((3.1175000000000006e-05 * fTempPerm886) + ((1.1129000000000001e-05 * fTempPerm887) + ((9.139900000000001e-05 * fTempPerm888) + ((0.000272806 * fTempPerm889) + ((0.00184981 * fTempPerm890) + ((0.0030104600000000004 * fTempPerm891) + ((0.00246014 * fTempPerm892) + ((0.0104416 * fTempPerm893) + ((0.0166062 * fTempPerm894) + ((0.0116756 * fTempPerm895) + ((0.008875280000000001 * fTempPerm896) + ((0.0203737 * fTempPerm897) + ((0.016194200000000002 * fTempPerm898) + ((0.015368999999999999 * fTempPerm899) + ((0.00862273 * fTempPerm900) + ((0.025263300000000002 * fTempPerm901) + ((0.026966700000000003 * fTempPerm902) + ((0.023967600000000002 * fTempPerm903) + ((0.0180915 * fTempPerm904) + ((0.00041459900000000003 * fTempPerm905) + ((0.000315062 * fTempPerm906) + ((0.000544865 * fTempPerm907) + ((2.8269799999999997e-06 * fTempPerm908) + ((0.000273034 * fTempPerm909) + ((0.00117495 * fTempPerm910) + ((0.00189912 * fTempPerm911) + ((0.00102903 * fTempPerm912) + ((0.0021747000000000003 * fTempPerm913) + ((0.0023879 * fTempPerm914) + ((0.00243748 * fTempPerm915) + ((0.0047484400000000005 * fTempPerm916) + ((0.00687643 * fTempPerm917) + ((0.0027951100000000004 * fTempPerm918) + ((0.0100582 * fTempPerm919) + ((0.00958195 * fTempPerm920) + ((0.00766095 * fTempPerm921) + ((0.0128559 * fTempPerm922) + ((0.016079200000000002 * fTempPerm923) + ((0.015913200000000002 * fTempPerm924) + ((0.0160175 * fTempPerm925) + ((0.0188738 * fTempPerm926) + ((0.0095142 * fTempPerm927) + ((0.0006032850000000001 * fTempPerm928) + ((0.0011779100000000001 * fTempPerm929) + ((0.000167329 * fTempPerm930) + ((0.0024969700000000003 * fTempPerm931) + ((0.0014838199999999998 * fTempPerm932) + ((0.00141242 * fTempPerm933) + ((0.0114164 * fTempPerm934) + ((0.00017378 * fTempPerm935) + ((7.21153e-05 * fTempPerm936) + ((1.68655e-05 * fVec0[IOTA&127]) + ((2.9285600000000003e-05 * fTempPerm937) + ((0.00011532 * fTempPerm938) + ((0.00030905200000000004 * fTempPerm939) + ((0.00016311100000000002 * fTempPerm940) + ((0.000262141 * fTempPerm941) + ((0.00041388 * fTempPerm942) + ((0.000233207 * fTempPerm943) + ((0.000138245 * fTempPerm944) + ((0.00461702 * fTempPerm945) + ((0.00040582000000000004 * fTempPerm946) + ((0.00237535 * fTempPerm947) + ((0.00134825 * fTempPerm948) + ((0.000603783 * fTempPerm949) + ((0.00476789 * fTempPerm950) + ((0.0018776900000000002 * fTempPerm951) + ((0.00756122 * fTempPerm952) + ((0.0040625900000000005 * fTempPerm953) + ((0.011857 * fTempPerm954) + ((0.00919539 * fTempPerm955) + ((0.0156673 * fTempPerm956) + ((0.0121152 * fTempPerm957) + ((0.0015964400000000002 * fTempPerm958) + ((0.0011488700000000002 * fTempPerm959) + ((0.0023870700000000003 * fTempPerm960) + ((0.00111238 * fTempPerm961) + ((4.81969e-05 * fTempPerm962) + ((0.00286108 * fTempPerm963) + ((0.00017706000000000002 * fTempPerm964) + ((0.00017067000000000002 * fTempPerm965) + ((0.00402653 * fTempPerm966) + ((0.0012919000000000001 * fTempPerm967) + ((0.000528537 * fTempPerm968) + ((4.5063100000000006e-05 * fTempPerm969) + ((4.5684500000000006e-05 * fTempPerm970) + ((0.000127879 * fTempPerm971) + ((0.000141295 * fTempPerm972) + ((0.000483767 * fTempPerm973) + ((0.00040982500000000005 * fTempPerm974) + ((0.00355741 * fTempPerm975) + ((0.00203951 * fTempPerm976) + ((0.00117138 * fTempPerm977) + ((0.0008606820000000001 * fTempPerm978) + ((0.00269537 * fTempPerm979) + ((0.00263881 * fTempPerm980) + ((5.334069999999999e-07 * fTempPerm981) + ((0.0032921200000000004 * fTempPerm982) + ((0.00132344 * fTempPerm983) + ((0.00143258 * fTempPerm984) + ((0.000134822 * fTempPerm985) + ((0.0011273899999999998 * fTempPerm986) + ((6.21192e-05 * fTempPerm987) + ((0.0007156350000000001 * fTempPerm988) + ((1.14088e-05 * fTempPerm989) + ((0.000782908 * fTempPerm990) + ((0.00280679 * fTempPerm991) + ((0.0015042599999999999 * fTempPerm992) + ((0.00385064 * fTempPerm993) + ((0.00191281 * fTempPerm994) + ((9.84852e-06 * fTempPerm995) + ((0.0005181680000000001 * fTempPerm996) + ((0.0021753000000000002 * fTempPerm997) + ((0.0005638910000000001 * fTempPerm998) + ((0.0008882540000000001 * fTempPerm999) + ((0.000446193 * fTempPerm1000) + ((0.0013548899999999999 * fTempPerm1001) + ((0.00142103 * fTempPerm1002) + ((0.00103439 * fTempPerm1003) + ((0.0016313200000000001 * fTempPerm1004) + ((0.000550486 * fTempPerm1005) + ((0.0010789999999999999 * fTempPerm1006) + ((0.0006419069999999999 * fTempPerm1007) + ((0.0014951399999999998 * fTempPerm1008) + ((0.00147963 * fTempPerm1009) + ((0.00239256 * fTempPerm1010) + ((0.00260887 * fTempPerm1011) + ((0.0013151 * fTempPerm1012) + ((0.00191902 * fTempPerm1013) + ((0.0015381499999999998 * fTempPerm1014) + ((1.8656099999999998e-06 * fTempPerm1015) + ((0.000929744 * fTempPerm1016) + ((0.00475476 * fTempPerm1017) + ((0.04279480000000001 * fTempPerm1018) + ((0.00454199 * fTempPerm1019) + ((0.0037710400000000002 * fTempPerm1020) + ((0.0346875 * fTempPerm1021) + ((0.000311911 * fTempPerm1022) + ((0.015415000000000002 * fTempPerm1023) + ((0.00354222 * fTempPerm1024) + ((0.009995440000000001 * fTempPerm1025) + ((0.0165022 * fTempPerm1026) + ((0.026153100000000002 * fTempPerm1027) + ((0.0155188 * fTempPerm1028) + ((0.00031515600000000003 * fTempPerm1029) + ((0.000684209 * fTempPerm1030) + ((0.00172844 * fTempPerm1031) + ((0.00033490500000000003 * fTempPerm1032) + ((0.0017261400000000001 * fTempPerm1033) + ((0.00166752 * fTempPerm1034) + ((0.00200404 * fTempPerm1035) + ((0.000541095 * fTempPerm1036) + ((0.00103026 * fTempPerm1037) + ((0.0019008400000000002 * fTempPerm1038) + ((0.000323087 * fTempPerm1039) + ((0.00020805 * fTempPerm1040) + ((0.00121188 * fTempPerm1041) + ((4.5241600000000004e-05 * fTempPerm1042) + ((0.00193403 * fTempPerm1043) + ((0.000502175 * fTempPerm1044) + ((0.00248879 * fTempPerm1045) + ((0.000498921 * fTempPerm1046) + ((0.0030320900000000003 * fTempPerm1047) + ((0.0006988710000000001 * fTempPerm1048) + ((6.574880000000001e-05 * fTempPerm1049) + ((0.00127535 * fTempPerm1050) + ((0.0012643700000000001 * fTempPerm1051) + ((0.0009646670000000001 * fTempPerm1052) + ((0.00113852 * fTempPerm1053) + ((0.000737536 * fTempPerm1054) + ((0.00336985 * fTempPerm1055) + ((0.00765874 * fTempPerm1056) + ((0.000735849 * fTempPerm1057) + ((0.00465625 * fTempPerm1058) + ((0.00177024 * fTempPerm1059) + ((0.0193136 * fTempPerm1060) + ((0.00392637 * fTempPerm1061) + ((0.0137585 * fTempPerm1062) + ((0.00436148 * fTempPerm1063) + ((0.0115485 * fTempPerm1064) + ((0.0032127699999999998 * fTempPerm1065) + ((0.005780400000000001 * fTempPerm1066) + ((0.037111700000000004 * fTempPerm1067) + ((0.014907300000000002 * fTempPerm1068) + ((0.0092683 * fTempPerm1069) + ((0.0158698 * fTempPerm1070) + ((0.00489909 * fTempPerm1071) + ((0.000362721 * fTempPerm1072) + ((0.00043871700000000003 * fTempPerm1073) + ((0.000181327 * fTempPerm1074) + ((0.0009705890000000001 * fTempPerm1075) + ((0.000160361 * fTempPerm1076) + ((0.000137362 * fTempPerm1077) + ((3.8802e-05 * fTempPerm1078) + ((0.000314307 * fTempPerm1079) + ((0.00019245000000000002 * fTempPerm1080) + ((0.00025063300000000005 * fTempPerm1081) + ((0.00035653000000000003 * fTempPerm1082) + ((0.00157916 * fTempPerm1083) + ((0.0017056100000000002 * fTempPerm1084) + ((0.00444703 * fTempPerm1085) + ((0.015119400000000002 * fTempPerm1086) + ((0.0357626 * fTempPerm1087) + ((0.0193807 * fTempPerm1088) + ((0.026437 * fTempPerm1089) + ((0.0221845 * fTempPerm1090) + ((0.0125257 * fTempPerm1091) + ((0.0390074 * fTempPerm1092) + ((0.00521665 * fTempPerm1093) + ((0.039406500000000004 * fTempPerm1094) + ((0.00134669 * fTempPerm1095) + ((0.00106944 * fTempPerm1096) + ((0.00296739 * fTempPerm1097) + ((3.866159999999999e-06 * fVec2[IOTA&127]) + ((6.8184199999999995e-06 * fTempPerm1098) + ((0.0014690600000000001 * fTempPerm1099) + ((0.00361629 * fTempPerm1100) + ((0.0052343400000000005 * fTempPerm1101) + ((0.0014029000000000001 * fTempPerm1102) + ((0.00391025 * fTempPerm1103) + ((0.010498400000000001 * fTempPerm1104) + ((0.00212608 * fTempPerm1105) + ((0.0009612730000000001 * fTempPerm1106) + ((1.9893500000000002e-05 * fTempPerm1107) + ((0.00044415800000000005 * fTempPerm1108) + ((0.0015305800000000001 * fTempPerm1109) + ((0.0014551499999999999 * fTempPerm1110) + ((0.00127253 * fTempPerm1111) + ((0.00508919 * fTempPerm1112) + ((0.010143899999999999 * fTempPerm1113) + ((0.00328774 * fTempPerm1114) + ((0.0020962000000000003 * fTempPerm1115) + ((0.011590800000000002 * fTempPerm1116) + ((0.00388006 * fTempPerm1117) + ((0.0222634 * fTempPerm1118) + ((0.020153300000000002 * fTempPerm1119) + ((0.042437800000000005 * fTempPerm1120) + ((0.0471336 * fTempPerm1121) + ((0.00567696 * fTempPerm1122) + ((0.00143681 * fTempPerm1123) + ((0.000861014 * fTempPerm1124) + ((0.000584922 * fTempPerm1125) + ((6.78291e-06 * fTempPerm1126) + ((0.000255296 * fTempPerm1127) + ((9.185100000000001e-05 * fTempPerm1128) + ((0.000643151 * fTempPerm1129) + ((0.00048162000000000004 * fTempPerm1130) + ((0.00029024100000000006 * fTempPerm1131) + ((0.00312195 * fTempPerm1132) + ((0.000478309 * fTempPerm1133) + ((0.014456700000000001 * fTempPerm1134) + ((0.00909628 * fTempPerm1135) + ((0.000692677 * fTempPerm1136) + ((0.00141222 * fTempPerm1137) + ((0.0036924600000000003 * fTempPerm1138) + ((0.00460171 * fTempPerm1139) + ((0.0117481 * fTempPerm1140) + ((3.7877500000000005e-05 * fTempPerm1141) + ((0.00198432 * fTempPerm1142) + ((0.00350462 * fTempPerm1143) + ((0.000138672 * fTempPerm1144) + ((0.00026133000000000005 * fTempPerm1145) + ((0.009537660000000002 * fTempPerm1146) + ((0.0027559100000000003 * fTempPerm1147) + ((0.000654569 * fTempPerm1148) + ((0.00161133 * fTempPerm1149) + ((8.74662e-06 * fTempPerm1150) + (0.00012562199999999998 * fTempPerm1151))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
			fRec0[0] = max((fRec0[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm1152))))));
			fbargraph9 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTempPerm1152;
			fTempPerm1153 = (fRec1[0] * (((1.07578e-06 * fTempPerm629) + ((1.2511200000000001e-05 * fTempPerm2) + ((2.95878e-05 * fTempPerm3) + ((1.0896600000000002e-05 * fTempPerm635) + ((5.7825300000000005e-06 * fTempPerm4) + ((0.0022768000000000003 * fTempPerm5) + ((0.000275005 * fTempPerm6) + ((0.0020205099999999997 * fTempPerm7) + ((0.00637634 * fTempPerm8) + ((0.00656153 * fTempPerm9) + ((0.0014331 * fTempPerm650) + ((0.00436247 * fTempPerm10) + ((0.007149600000000001 * fTempPerm11) + ((3.30668e-06 * fTempPerm14) + ((9.644430000000001e-05 * fTempPerm16) + ((0.000340968 * fTempPerm657) + ((0.000638058 * fTempPerm17) + ((0.00122649 * fTempPerm18) + ((0.000192954 * fTempPerm658) + ((0.0015055300000000001 * fTempPerm22) + ((0.00042873700000000005 * fTempPerm23) + ((0.0192933 * fTempPerm24) + ((0.00688345 * fTempPerm25) + ((0.000263627 * fTempPerm664) + ((0.00047379 * fTempPerm666) + ((0.000251299 * fTempPerm28) + ((0.0009277500000000001 * fTempPerm30) + ((2.1505e-05 * fTempPerm673) + ((0.000129941 * fTempPerm34) + ((0.000159738 * fTempPerm35) + ((0.000493628 * fTempPerm36) + ((0.00101923 * fTempPerm37) + ((0.000102185 * fTempPerm39) + ((0.0015065 * fTempPerm40) + ((0.0005151690000000001 * fTempPerm41) + ((9.494400000000002e-05 * fTempPerm676) + ((0.000954181 * fTempPerm677) + ((7.2276399999999995e-06 * fTempPerm683) + ((0.00027668200000000005 * fTempPerm52) + ((0.000637521 * fTempPerm55) + ((0.0013802 * fTempPerm56) + ((0.0034082300000000004 * fTempPerm684) + ((0.00345985 * fTempPerm685) + ((0.00106162 * fTempPerm686) + ((0.0009198190000000001 * fTempPerm687) + ((0.00140911 * fTempPerm57) + ((0.00186255 * fTempPerm58) + ((0.00722064 * fTempPerm690) + ((0.00114676 * fTempPerm691) + ((1.0336899999999999e-06 * fTempPerm66) + ((0.00010999399999999999 * fTempPerm67) + ((0.04143 * fTempPerm68) + ((0.032932100000000006 * fTempPerm69) + ((0.0056186100000000004 * fTempPerm71) + ((0.042141200000000004 * fTempPerm72) + ((0.024621200000000003 * fTempPerm73) + ((0.028794800000000002 * fTempPerm705) + ((0.06345640000000001 * fTempPerm74) + ((0.0490934 * fTempPerm75) + ((0.0540471 * fTempPerm76) + ((0.0108471 * fTempPerm707) + ((0.0010471900000000001 * fTempPerm714) + ((0.00047446700000000006 * fTempPerm80) + ((0.00031437800000000003 * fTempPerm82) + ((0.0010047600000000002 * fTempPerm85) + ((0.000233961 * fTempPerm86) + ((0.0005237470000000001 * fTempPerm87) + ((0.0011957500000000002 * fTempPerm88) + ((0.0011231 * fTempPerm89) + ((0.000559696 * fTempPerm720) + ((0.0005083090000000001 * fTempPerm721) + ((0.000743556 * fTempPerm90) + ((0.000897913 * fTempPerm723) + ((0.000775016 * fTempPerm92) + ((0.00645499 * fTempPerm729) + ((0.0011423000000000002 * fTempPerm94) + ((0.0177702 * fTempPerm95) + ((0.00963596 * fTempPerm96) + ((0.019155500000000002 * fTempPerm732) + ((6.59979e-08 * fTempPerm97) + ((0.0010346200000000002 * fTempPerm99) + ((0.0016114299999999999 * fTempPerm102) + ((0.00867053 * fTempPerm104) + ((0.00992074 * fTempPerm105) + ((0.000320497 * fTempPerm736) + ((0.034926 * fTempPerm106) + ((3.47604e-08 * fTempPerm107) + ((1.26739e-05 * fTempPerm737) + ((4.16914e-05 * fTempPerm108) + ((0.000503369 * fTempPerm739) + ((0.0157848 * fTempPerm740) + ((0.000148373 * fTempPerm741) + ((8.17412e-06 * fTempPerm742) + ((0.00022863600000000002 * fTempPerm743) + ((0.00032662200000000005 * fTempPerm744) + ((0.000334401 * fTempPerm120) + ((0.00171149 * fTempPerm746) + ((0.00685873 * fTempPerm122) + ((0.00040021800000000004 * fTempPerm747) + ((0.0009807190000000001 * fTempPerm749) + ((0.0009296310000000001 * fTempPerm123) + ((0.00244952 * fTempPerm124) + ((1.41881e-06 * fTempPerm127) + ((3.31876e-06 * fTempPerm128) + ((8.76003e-06 * fTempPerm129) + ((5.56606e-06 * fTempPerm130) + ((0.00011918500000000002 * fTempPerm131) + ((0.0023261500000000004 * fTempPerm133) + ((0.00150447 * fTempPerm134) + ((0.00121048 * fTempPerm137) + ((0.00390744 * fTempPerm138) + ((0.00102589 * fTempPerm139) + ((0.0011997199999999998 * fTempPerm768) + ((0.0125606 * fTempPerm141) + ((0.0115422 * fTempPerm772) + ((1.8977800000000003e-05 * fTempPerm144) + ((9.322950000000002e-05 * fTempPerm145) + ((0.000122408 * fTempPerm146) + ((3.4954000000000004e-05 * fTempPerm148) + ((0.000876507 * fTempPerm779) + ((0.00013990400000000002 * fTempPerm780) + ((0.0021519 * fTempPerm153) + ((0.0032282 * fTempPerm154) + ((0.00174807 * fTempPerm155) + ((7.665409999999998e-06 * fTempPerm781) + ((0.000184975 * fTempPerm157) + ((7.19018e-05 * fTempPerm782) + ((0.000165502 * fTempPerm783) + ((0.00425301 * fTempPerm159) + ((3.71676e-05 * fTempPerm784) + ((1.5261800000000004e-05 * fTempPerm162) + ((0.00022708 * fTempPerm785) + ((0.0006416280000000001 * fTempPerm788) + ((0.000129173 * fTempPerm168) + ((0.0010500000000000002 * fTempPerm169) + ((0.0013042499999999999 * fTempPerm170) + ((0.0025056699999999998 * fTempPerm171) + ((0.00222449 * fTempPerm172) + ((0.00472803 * fTempPerm174) + ((0.00379148 * fTempPerm175) + ((0.009442299999999999 * fTempPerm791) + ((((((0.00125953 * fTempPerm796) + ((((((((((((((((((((((((4.490090000000001e-05 * fTempPerm822) + ((0.00280241 * fTempPerm823) + ((0.0002275 * fTempPerm824) + ((0.00365462 * fTempPerm825) + ((7.3547e-05 * fTempPerm826) + ((0.00033735 * fTempPerm221) + ((0.0013227800000000002 * fTempPerm828) + ((0.000575071 * fTempPerm829) + ((0.015908 * fTempPerm831) + ((0.00139284 * fTempPerm224) + ((0.0111489 * fTempPerm832) + ((0.00048254200000000003 * fTempPerm225) + ((0.0008444020000000001 * fTempPerm835) + ((0.00015426800000000002 * fTempPerm234) + ((0.00044798200000000004 * fTempPerm236) + ((0.00321454 * fTempPerm836) + ((0.00200028 * fTempPerm239) + ((0.00582195 * fTempPerm838) + ((0.0170789 * fTempPerm839) + ((0.000996385 * fTempPerm840) + ((0.00387811 * fTempPerm244) + ((0.0480442 * fTempPerm842) + ((0.0047823300000000004 * fTempPerm843) + ((0.0315938 * fTempPerm844) + ((0.00920931 * fTempPerm846) + ((0.0182515 * fTempPerm847) + ((0.0125871 * fTempPerm248) + ((0.00107937 * fTempPerm848) + ((0.00108131 * fTempPerm849) + ((0.00149292 * fTempPerm851) + ((0.000624781 * fTempPerm852) + ((0.00028939400000000006 * fTempPerm853) + ((3.2490500000000003e-05 * fTempPerm256) + ((0.006148590000000001 * fTempPerm854) + ((0.000115798 * fTempPerm855) + ((5.14829e-06 * fTempPerm856) + ((0.000659975 * fTempPerm859) + ((0.0016790700000000002 * fTempPerm258) + ((0.00119525 * fTempPerm259) + ((0.0537481 * fTempPerm260) + ((0.0130594 * fTempPerm261) + ((0.0292041 * fTempPerm262) + ((0.015611 * fTempPerm263) + ((0.0131959 * fTempPerm265) + ((0.0033821 * fTempPerm266) + ((0.000654334 * fTempPerm267) + ((0.000286004 * fTempPerm268) + ((0.00012754700000000002 * fTempPerm866) + ((0.000569647 * fTempPerm269) + ((0.000558749 * fTempPerm270) + ((0.000107706 * fTempPerm271) + ((1.9102600000000002e-05 * fTempPerm870) + ((0.00221504 * fTempPerm273) + ((3.8517100000000004e-05 * fTempPerm274) + ((0.0020406 * fTempPerm276) + ((1.0045e-05 * fTempPerm277) + ((1.9873400000000002e-05 * fTempPerm278) + ((0.0013280499999999999 * fTempPerm279) + ((0.00173031 * fTempPerm280) + ((0.00102092 * fTempPerm876) + ((0.00235812 * fTempPerm281) + ((0.00227417 * fTempPerm879) + ((0.0030812 * fTempPerm283) + ((0.00796032 * fTempPerm284) + ((0.000330976 * fTempPerm881) + ((3.69732e-06 * fTempPerm882) + ((0.00669066 * fTempPerm286) + ((0.00817955 * fTempPerm287) + ((0.033864 * fTempPerm288) + ((0.009235 * fTempPerm289) + ((8.505850000000001e-06 * fTempPerm885) + ((3.39827e-05 * fTempPerm887) + ((0.00011888700000000002 * fTempPerm888) + ((0.000368308 * fTempPerm889) + ((0.00021972700000000002 * fTempPerm295) + ((0.0006599420000000001 * fTempPerm301) + (((((((((((((((((((((((((((((((((((((2.00819e-05 * fTempPerm908) + ((((0.000239084 * fTempPerm909) + ((0.00445499 * fTempPerm912) + ((0.00128867 * fTempPerm913) + ((((((((((((((((((((((((((((((((((((((((((3.3373e-05 * fTempPerm944) + ((((((((((((((((((((((0.00129704 * fTempPerm401) + ((((0.00159566 * fTempPerm404) + ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((0.000550446 * fTempPerm481) + (((((((((((((((((((((((((((((((((0.022299000000000003 * fTempPerm516) + ((0.0012059800000000002 * fTempPerm517) + ((((((((4.14705e-05 * fTempPerm1078) + ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((1.1146700000000001e-05 * fTempPerm1136) + (((0.020153599999999997 * fTempPerm591) + ((0.016894100000000002 * fTempPerm1140) + ((2.9996500000000003e-05 * fTempPerm594) + ((0.0243674 * fTempPerm596) + ((0.000154297 * fTempPerm599) + ((0.0105391 * fTempPerm601) + ((0.00565642 * fTempPerm602) + ((0.0272114 * fTempPerm603) + ((0.021074000000000002 * fTempPerm604) + ((0.008019420000000001 * fTempPerm606) + ((0.013509400000000001 * fTempPerm610) + ((6.14464e-06 * fTempPerm611) + ((0.000234556 * fTempPerm1147) + ((((9.12388e-06 * fTempPerm1150) + (2.6751600000000003e-05 * fTempPerm1151)) + (0.00756406 * fTempPerm1149)) + (0.00109229 * fTempPerm613))))))))))))))) + (0.041638400000000006 * fTempPerm590))) + (0.00046772600000000007 * fTempPerm1134)) + (0.00729384 * fTempPerm589)) + (0.004986290000000001 * fTempPerm588)) + (0.00175057 * fTempPerm616)) + (0.00130975 * fTempPerm587)) + (0.000195057 * fTempPerm586)) + (0.00251957 * fTempPerm584)) + (0.021176 * fTempPerm583)) + (5.6691300000000004e-05 * fTempPerm1131)) + (0.000870882 * fTempPerm582)) + (0.0015448500000000002 * fTempPerm1130)) + (0.00015575000000000002 * fTempPerm621)) + (0.00020135900000000002 * fTempPerm622)) + (0.000231249 * fTempPerm1124)) + (0.000358363 * fTempPerm624)) + (0.00526948 * fTempPerm1117)) + (1.1025e-06 * fTempPerm626)) + (0.00016541 * fTempPerm627)) + (0.00946403 * fTempPerm1113)) + (0.027225600000000003 * fTempPerm1112)) + (4.44119e-05 * fTempPerm1111)) + (0.00168163 * fTempPerm581)) + (0.0011816700000000001 * fTempPerm1109)) + (2.2315100000000003e-05 * fTempPerm1107)) + (0.00267881 * fTempPerm1106)) + (0.00157178 * fTempPerm1105)) + (0.009424450000000001 * fTempPerm1102)) + (1.96889e-05 * fVec8[IOTA&127])) + (0.00012713900000000002 * fTempPerm578)) + (1.5002200000000002e-05 * fVec5[IOTA&127])) + (2.3258800000000005e-05 * fTempPerm575)) + (0.0466987 * fTempPerm573)) + (0.0530012 * fTempPerm572)) + (0.043948600000000004 * fTempPerm571)) + (0.0489414 * fTempPerm570)) + (0.0007391530000000001 * fTempPerm569)) + (0.0020685400000000002 * fTempPerm568)) + (0.000557527 * fTempPerm567)) + (0.00107368 * fTempPerm566)) + (0.00105561 * fTempPerm565)) + (0.000646817 * fTempPerm564)) + (0.0622112 * fTempPerm563)) + (0.0813459 * fTempPerm562)) + (0.047424299999999996 * fTempPerm561)) + (0.0414059 * fTempPerm560)) + (0.0697323 * fTempPerm559)) + (0.0503806 * fTempPerm558)) + (0.111697 * fTempPerm557)) + (0.05446430000000001 * fTempPerm556)) + (0.00431154 * fTempPerm554)) + (0.00846299 * fTempPerm553)) + (0.0038801100000000004 * fTempPerm552)) + (0.0007157 * fTempPerm1097)) + (0.00464837 * fTempPerm551)) + (0.000671666 * fTempPerm550)) + (0.00143566 * fTempPerm549)) + (0.000209301 * fTempPerm548)) + (0.00139635 * fTempPerm547)) + (0.00292299 * fTempPerm546)) + (0.00278079 * fTempPerm545)) + (0.00338193 * fTempPerm544)) + (0.00648151 * fTempPerm543)) + (0.00365475 * fTempPerm542)) + (0.00198287 * fTempPerm541)) + (0.0036032700000000004 * fTempPerm540)) + (0.0015368900000000002 * fTempPerm1096)) + (2.5156400000000002e-05 * fTempPerm539)) + (0.00463264 * fTempPerm538)) + (0.00059498 * fTempPerm535)) + (0.000959789 * fTempPerm534)) + (0.023601800000000003 * fTempPerm1094)) + (0.00100501 * fTempPerm532)) + (0.00050422 * fTempPerm531)) + (0.000508036 * fTempPerm530)) + (0.000202164 * fTempPerm529)) + (0.012867100000000001 * fTempPerm1093)) + (0.033071500000000004 * fTempPerm1092)) + (0.024658199999999998 * fTempPerm1091)) + (0.0175694 * fTempPerm1090)) + (0.0232959 * fTempPerm1089)) + (0.023498 * fTempPerm1088)) + (0.030269900000000002 * fTempPerm1087)) + (0.0188991 * fTempPerm1086)) + (0.00232849 * fTempPerm1085)) + (0.00227327 * fTempPerm1084)) + (0.00164631 * fTempPerm1083)) + (0.00030627700000000004 * fTempPerm1081)) + (0.000251322 * fTempPerm1079))) + (0.000158936 * fTempPerm1077)) + (0.000147661 * fTempPerm523)) + (0.000231146 * fTempPerm522)) + (0.0007695660000000001 * fTempPerm521)) + (7.080070000000001e-05 * fTempPerm1074)) + (0.0258155 * fTempPerm518)))) + (0.0092729 * fTempPerm1066)) + (0.00438648 * fTempPerm515)) + (0.0027836500000000004 * fTempPerm1065)) + (0.000537327 * fTempPerm513)) + (0.00403937 * fTempPerm512)) + (0.00884039 * fTempPerm511)) + (0.0054466 * fTempPerm510)) + (0.00108066 * fTempPerm509)) + (0.00415929 * fTempPerm506)) + (0.000386788 * fTempPerm1054)) + (0.00118894 * fTempPerm505)) + (0.000827394 * fTempPerm503)) + (0.00297761 * fTempPerm501)) + (5.5900900000000005e-05 * fTempPerm1049)) + (0.00127378 * fTempPerm499)) + (0.00842323 * fTempPerm498)) + (0.0110775 * fTempPerm497)) + (0.0028706900000000004 * fTempPerm496)) + (0.00201686 * fTempPerm494)) + (0.004869989999999999 * fTempPerm1046)) + (0.00207781 * fTempPerm493)) + (0.00615266 * fTempPerm1045)) + (0.000701284 * fTempPerm1044)) + (0.000178551 * fTempPerm492)) + (0.00406262 * fTempPerm491)) + (0.00275175 * fTempPerm490)) + (9.867810000000001e-05 * fTempPerm488)) + (0.000730135 * fTempPerm1033)) + (0.000103707 * fTempPerm1032)) + (0.00100478 * fTempPerm1031)) + (0.000650443 * fTempPerm483))) + (0.00011103000000000001 * fTempPerm1029)) + (4.6366800000000005e-05 * fTempPerm638)) + (0.0368006 * fTempPerm1027)) + (0.00206245 * fTempPerm474)) + (0.00341205 * fTempPerm473)) + (0.0024348900000000003 * fTempPerm1026)) + (0.006066569999999999 * fTempPerm471)) + (0.00380208 * fTempPerm470)) + (0.0026122100000000002 * fTempPerm469)) + (0.00293818 * fTempPerm468)) + (0.0016933400000000002 * fTempPerm467)) + (0.00072404 * fTempPerm466)) + (0.00105509 * fTempPerm1022)) + (0.00266924 * fTempPerm465)) + (0.0026745600000000003 * fTempPerm464)) + (1.74779e-06 * fTempPerm463)) + (0.0057548899999999995 * fTempPerm1020)) + (0.0110121 * fTempPerm1019)) + (0.056895100000000004 * fTempPerm1018)) + (0.0051357799999999995 * fTempPerm1017)) + (0.00015571700000000001 * fTempPerm459)) + (0.0017905 * fTempPerm1016)) + (0.00116433 * fTempPerm457)) + (1.48337e-06 * fTempPerm1015)) + (0.00145085 * fTempPerm453)) + (0.0009061030000000001 * fTempPerm452)) + (9.57012e-05 * fTempPerm450)) + (1.8649e-06 * fTempPerm449)) + (0.00222904 * fTempPerm447)) + (0.00354482 * fTempPerm445)) + (0.00111368 * fTempPerm444)) + (0.00040873600000000004 * fTempPerm1005)) + (0.00198173 * fTempPerm1002)) + (0.00373392 * fTempPerm443)) + (0.00347187 * fTempPerm997)) + (0.00011697900000000002 * fTempPerm440)) + (4.2207000000000004e-05 * fTempPerm995)) + (0.00188691 * fTempPerm994)) + (5.530960000000001e-05 * fTempPerm438)) + (0.0026210900000000004 * fTempPerm993)) + (0.00226215 * fTempPerm992)) + (0.00107755 * fTempPerm991)) + (0.00017575500000000001 * fTempPerm434)) + (0.00041635900000000004 * fTempPerm989)) + (0.00109789 * fTempPerm988)) + (0.0012166300000000002 * fTempPerm987)) + (0.00012959 * fTempPerm986)) + (0.000136011 * fTempPerm432)) + (0.00508388 * fTempPerm427)) + (0.000148231 * fTempPerm425)) + (0.000531922 * fTempPerm424)) + (0.00155444 * fTempPerm984)) + (0.0017853699999999999 * fTempPerm983)) + (0.000769439 * fTempPerm419)) + (7.510190000000001e-05 * fTempPerm418)) + (0.00207868 * fTempPerm982)) + (1.3220600000000002e-05 * fTempPerm981)) + (7.21156e-07 * fTempPerm413)) + (0.045202099999999995 * fTempPerm410)) + (0.0037277 * fTempPerm409)) + (0.00697526 * fTempPerm408)) + (0.0152545 * fTempPerm980)) + (0.00152519 * fTempPerm407)) + (0.0007640640000000001 * fTempPerm405))) + (0.000654404 * fTempPerm402)) + (0.000348142 * fTempPerm976))) + (4.2136500000000006e-05 * fTempPerm400)) + (0.00041380000000000003 * fTempPerm398)) + (0.000258758 * fTempPerm397)) + (0.00030013 * fTempPerm394)) + (0.00025865 * fTempPerm968)) + (0.00050859 * fTempPerm392)) + (0.000359551 * fTempPerm391)) + (0.00292619 * fTempPerm967)) + (0.0028104300000000005 * fTempPerm388)) + (0.000454632 * fTempPerm387)) + (0.005743590000000001 * fTempPerm966)) + (5.1984399999999995e-06 * fTempPerm386)) + (0.000129621 * fTempPerm965)) + (0.00171093 * fTempPerm962)) + (0.00203837 * fTempPerm382)) + (0.000103257 * fTempPerm381)) + (1.0191e-05 * fTempPerm380)) + (0.00446647 * fTempPerm376)) + (0.0018692700000000001 * fTempPerm374)) + (9.91772e-05 * fTempPerm948))) + (0.0007244580000000001 * fTempPerm371)) + (0.000840054 * fTempPerm370)) + (0.000782518 * fTempPerm369)) + (0.0368012 * fTempPerm368)) + (0.0424237 * fTempPerm367)) + (0.0075229400000000005 * fTempPerm366)) + (0.0123141 * fTempPerm934)) + (0.0216826 * fTempPerm365)) + (0.0140384 * fTempPerm364)) + (0.0105579 * fTempPerm363)) + (0.0042595 * fTempPerm362)) + (0.0039974 * fTempPerm361)) + (0.00201999 * fTempPerm360)) + (0.00204096 * fTempPerm359)) + (0.00144247 * fTempPerm358)) + (4.49754e-06 * fTempPerm357)) + (0.00193882 * fTempPerm933)) + (0.000779071 * fTempPerm355)) + (5.5996800000000006e-05 * fTempPerm354)) + (0.00147426 * fTempPerm352)) + (0.00184199 * fTempPerm351)) + (0.00103368 * fTempPerm350)) + (0.000426597 * fTempPerm349)) + (0.000502614 * fTempPerm348)) + (0.000161141 * fTempPerm347)) + (0.010567 * fTempPerm928)) + (0.0142102 * fTempPerm927)) + (0.0173035 * fTempPerm926)) + (0.0254229 * fTempPerm925)) + (0.0143637 * fTempPerm924)) + (0.019806900000000002 * fTempPerm923)) + (0.0166992 * fTempPerm922)) + (0.005800029999999999 * fTempPerm921)) + (0.00790244 * fTempPerm920)) + (0.00740671 * fTempPerm919)) + (0.00389261 * fTempPerm918)) + (0.00468815 * fTempPerm917)) + (0.0020450900000000003 * fTempPerm916)) + (0.000377902 * fTempPerm915)) + (0.0039002500000000005 * fTempPerm914))))) + (0.017730600000000003 * fTempPerm332)) + (5.35095e-06 * fVec1[IOTA&127]))) + (0.008159850000000001 * fTempPerm331)) + (0.0014525200000000001 * fTempPerm330)) + (0.0067297599999999996 * fTempPerm329)) + (0.0028127900000000003 * fTempPerm328)) + (0.00337264 * fTempPerm327)) + (0.00390875 * fTempPerm325)) + (0.00530227 * fTempPerm324)) + (0.00538217 * fTempPerm323)) + (0.0033512100000000003 * fTempPerm321)) + (0.0009450379999999999 * fTempPerm319)) + (0.0007092450000000001 * fTempPerm318)) + (0.0025739 * fTempPerm317)) + (0.0013272 * fTempPerm316)) + (0.0012236 * fTempPerm315)) + (0.0016259900000000001 * fTempPerm314)) + (0.00038352400000000005 * fTempPerm313)) + (0.000117308 * fTempPerm312)) + (7.39806e-07 * fTempPerm311)) + (1.69708e-05 * fTempPerm310)) + (7.13748e-06 * fTempPerm309)) + (0.000275169 * fTempPerm906)) + (0.0186217 * fTempPerm904)) + (0.029073099999999998 * fTempPerm903)) + (0.0333365 * fTempPerm902)) + (0.017693 * fTempPerm901)) + (0.0227996 * fTempPerm900)) + (0.016965 * fTempPerm899)) + (0.011685700000000002 * fTempPerm898)) + (0.0160724 * fTempPerm897)) + (0.017027300000000002 * fTempPerm896)) + (0.0122635 * fTempPerm895)) + (0.0121522 * fTempPerm894)) + (0.0109805 * fTempPerm893)) + (0.0030369999999999998 * fTempPerm892)) + (0.000542053 * fTempPerm891)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (2.0505700000000003e-05 * fTempPerm819)) + (0.0163858 * fTempPerm203)) + (0.00939652 * fTempPerm202)) + (0.0139764 * fTempPerm201)) + (0.00411948 * fTempPerm818)) + (0.00295888 * fTempPerm200)) + (0.0107584 * fTempPerm199)) + (7.1971e-05 * fTempPerm815)) + (0.00021672300000000002 * fTempPerm814)) + (0.00293369 * fTempPerm198)) + (0.00306934 * fTempPerm197)) + (0.0006341350000000001 * fTempPerm813)) + (8.02869e-05 * fTempPerm196)) + (0.000618073 * fTempPerm193)) + (0.000501459 * fTempPerm192)) + (0.036199 * fTempPerm191)) + (0.0284124 * fTempPerm807)) + (0.0106017 * fTempPerm189)) + (0.0169305 * fTempPerm801)) + (0.00394994 * fTempPerm184)) + (0.0024844 * fTempPerm182)) + (0.0013637999999999999 * fTempPerm798))) + (0.0009547269999999999 * fTempPerm180)) + (0.000362151 * fTempPerm179)) + (1.57539e-07 * fTempPerm177)) + (0.07541260000000001 * fTempPerm176))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) - ((4.932260000000001e-05 * fTempPerm630) + ((0.000195522 * fTempPerm631) + ((0.000101428 * fTempPerm632) + ((0.00100915 * fTempPerm633) + ((9.86646e-07 * fTempPerm634) + ((0.000106835 * fTempPerm636) + ((0.000180181 * fTempPerm637) + ((0.0008870710000000001 * fTempPerm640) + ((3.4608100000000005e-05 * fTempPerm641) + ((0.000158107 * fTempPerm642) + ((0.00187875 * fTempPerm643) + ((0.00112458 * fTempPerm644) + ((0.00167732 * fTempPerm645) + ((0.00348053 * fTempPerm646) + ((0.00327984 * fTempPerm647) + ((0.0023563100000000003 * fTempPerm648) + ((0.00626758 * fTempPerm649) + ((0.004890869999999999 * fTempPerm651) + ((0.0130982 * fTempPerm652) + ((0.00953651 * fTempPerm653) + ((1.6838600000000002e-05 * fTempPerm654) + ((8.331860000000002e-05 * fTempPerm655) + ((0.000349096 * fTempPerm15) + ((0.00016574 * fTempPerm656) + ((0.00020233 * fTempPerm19) + ((0.000561862 * fTempPerm639) + ((0.0016132899999999999 * fTempPerm20) + ((0.000172719 * fTempPerm21) + ((0.00284821 * fTempPerm659) + ((0.00218125 * fTempPerm660) + ((0.00922201 * fTempPerm661) + ((0.01165 * fTempPerm662) + ((0.00639689 * fTempPerm26) + ((0.00786339 * fTempPerm663) + ((0.00843745 * fTempPerm27) + ((0.00396484 * fTempPerm665) + ((0.00145427 * fTempPerm667) + ((0.00011763 * fTempPerm668) + ((0.00019838400000000002 * fTempPerm29) + ((8.49295e-06 * fTempPerm669) + ((1.1654e-05 * fTempPerm670) + ((5.10851e-05 * fTempPerm671) + ((1.41509e-05 * fTempPerm33) + ((0.00010523700000000001 * fTempPerm672) + ((0.00124543 * fTempPerm38) + ((0.00084653 * fTempPerm674) + ((0.0020271300000000003 * fTempPerm675) + ((0.001299 * fTempPerm42) + ((0.00460066 * fTempPerm678) + ((0.0011063700000000002 * fTempPerm679) + ((0.00513181 * fTempPerm680) + ((0.004970270000000001 * fTempPerm681) + ((0.000665008 * fTempPerm682) + ((5.71049e-06 * fTempPerm45) + ((1.32724e-05 * fTempPerm46) + ((0.000395822 * fTempPerm47) + ((0.000272771 * fTempPerm48) + ((0.00197319 * fTempPerm49) + ((0.000964846 * fTempPerm50) + ((0.000542423 * fTempPerm51) + ((0.000785101 * fTempPerm53) + ((0.0031734000000000003 * fTempPerm54) + ((0.00156606 * fTempPerm688) + ((0.0017704200000000002 * fTempPerm689) + ((0.00118978 * fTempPerm59) + ((0.0015830800000000002 * fTempPerm60) + ((0.00380796 * fTempPerm61) + ((0.00307881 * fTempPerm62) + ((0.000320157 * fTempPerm63) + ((0.000220457 * fTempPerm692) + ((9.45697e-05 * fTempPerm693) + ((6.19357e-07 * fTempPerm64) + ((5.723480000000001e-05 * fTempPerm65) + ((0.000286928 * fTempPerm694) + ((0.0015510600000000002 * fTempPerm695) + ((0.00152554 * fTempPerm696) + ((0.0024532300000000003 * fTempPerm697) + ((0.0009760260000000001 * fTempPerm698) + ((0.00165106 * fTempPerm699) + ((0.022465099999999998 * fTempPerm700) + ((0.00044967800000000007 * fTempPerm701) + ((0.00609081 * fTempPerm702) + ((0.00905307 * fTempPerm70) + ((0.00808286 * fTempPerm703) + ((0.00263838 * fTempPerm704) + ((0.015390200000000001 * fTempPerm77) + ((0.0459645 * fTempPerm706) + ((0.0287878 * fTempPerm708) + ((0.04831920000000001 * fTempPerm709) + ((0.07684529999999999 * fTempPerm710) + ((0.0744374 * fTempPerm711) + ((0.0013813599999999999 * fTempPerm712) + ((9.486900000000002e-05 * fTempPerm78) + ((0.000976536 * fTempPerm713) + ((0.00011001600000000001 * fTempPerm79) + ((0.000634386 * fTempPerm715) + ((0.000125892 * fTempPerm81) + ((0.00040926 * fTempPerm716) + ((1.9403399999999997e-06 * fTempPerm83) + ((5.28142e-05 * fTempPerm717) + ((2.2899500000000003e-05 * fTempPerm718) + ((0.00025527000000000004 * fTempPerm84) + ((0.000171772 * fTempPerm719) + ((0.0010732299999999999 * fTempPerm722) + ((0.00024288 * fTempPerm91) + ((0.00392757 * fTempPerm724) + ((2.56907e-05 * fTempPerm93) + ((0.0056145 * fTempPerm725) + ((0.00288651 * fTempPerm726) + ((0.00264027 * fTempPerm727) + ((0.00038527200000000003 * fTempPerm728) + ((0.0158301 * fTempPerm730) + ((0.02706 * fTempPerm731) + ((3.2479200000000006e-05 * fTempPerm98) + ((0.0013326899999999999 * fTempPerm100) + ((0.00335524 * fTempPerm733) + ((0.000641773 * fTempPerm101) + ((0.00375105 * fTempPerm734) + ((0.00271496 * fTempPerm103) + ((0.000819709 * fTempPerm735) + ((0.00024267500000000003 * fTempPerm738) + ((0.000571456 * fTempPerm109) + ((0.0006149300000000001 * fTempPerm110) + ((0.0011941500000000002 * fTempPerm111) + ((0.00328942 * fTempPerm112) + ((0.000674882 * fTempPerm113) + ((0.00020588500000000003 * fTempPerm114) + ((3.52551e-06 * fTempPerm117) + ((2.5941400000000002e-05 * fTempPerm118) + ((0.000991956 * fTempPerm119) + ((0.000162845 * fTempPerm121) + ((0.000175864 * fTempPerm745) + ((0.00164357 * fTempPerm748) + ((0.00026003500000000004 * fTempPerm750) + ((0.0005108500000000001 * fTempPerm751) + ((0.0066563 * fTempPerm752) + ((0.0008063269999999999 * fTempPerm753) + ((0.00091664 * fTempPerm754) + ((0.000293776 * fTempPerm755) + ((0.000739556 * fTempPerm756) + ((0.0005989060000000001 * fTempPerm757) + ((0.00130731 * fTempPerm758) + ((0.00277688 * fTempPerm759) + ((0.00145302 * fTempPerm132) + ((0.0011399600000000002 * fTempPerm760) + ((0.001377 * fTempPerm135) + ((0.0028667500000000004 * fTempPerm761) + ((0.00110995 * fTempPerm762) + ((0.00059075 * fTempPerm763) + ((0.00124602 * fTempPerm764) + ((0.00129805 * fTempPerm765) + ((0.00044511 * fTempPerm136) + ((0.00383124 * fTempPerm766) + ((0.0016638 * fTempPerm767) + ((0.0014793400000000002 * fTempPerm140) + ((0.0215706 * fTempPerm769) + ((0.00439889 * fTempPerm770) + ((0.0286455 * fTempPerm771) + ((3.48346e-05 * fTempPerm147) + ((0.0007250860000000001 * fTempPerm773) + ((0.00120272 * fTempPerm774) + ((0.00040302800000000005 * fTempPerm149) + ((0.000665779 * fTempPerm775) + ((0.004597380000000001 * fTempPerm776) + ((0.0073186 * fTempPerm150) + ((0.013183400000000001 * fTempPerm151) + ((0.0015759 * fTempPerm152) + ((0.0015273399999999999 * fTempPerm777) + ((0.00046550599999999997 * fTempPerm778) + ((0.00022987799999999998 * fTempPerm156) + ((0.0012261 * fTempPerm158) + ((0.0010572399999999999 * fTempPerm163) + ((0.00077856 * fTempPerm164) + ((0.000737778 * fTempPerm165) + ((0.00065036 * fTempPerm166) + ((0.0031641300000000002 * fTempPerm786) + ((0.000310688 * fTempPerm167) + ((0.0028975800000000003 * fTempPerm787) + ((0.0009069470000000002 * fTempPerm173) + ((0.009209740000000001 * fTempPerm789) + (((((((((((((((((0.00036303800000000004 * fTempPerm190) + (((7.912519999999999e-06 * fTempPerm803) + (((1.6980200000000004e-05 * fTempPerm805) + (((((((((((((((((((3.6292700000000006e-05 * fTempPerm209) + ((0.00122034 * fTempPerm210) + ((0.00106711 * fTempPerm211) + ((0.00308618 * fTempPerm212) + ((0.0009655010000000001 * fTempPerm213) + ((0.000583876 * fTempPerm214) + ((0.00239031 * fTempPerm215) + ((0.00149796 * fTempPerm216) + ((0.000583562 * fTempPerm217) + ((1.76065e-05 * fTempPerm218) + ((0.00191797 * fTempPerm219) + ((0.000347477 * fTempPerm220) + ((0.00115249 * fTempPerm827) + ((0.00241141 * fTempPerm830) + ((0.00263921 * fTempPerm222) + ((0.0032833199999999997 * fTempPerm223) + ((0.00033950300000000006 * fTempPerm833) + ((0.0019217200000000002 * fTempPerm226) + ((0.00046365600000000006 * fTempPerm834) + ((0.000557107 * fTempPerm227) + ((4.2405400000000006e-05 * fTempPerm228) + ((3.5918100000000004e-05 * fTempPerm229) + ((0.0007859140000000001 * fTempPerm230) + ((4.87449e-07 * fTempPerm231) + ((4.85157e-05 * fTempPerm232) + ((7.42483e-05 * fTempPerm233) + ((0.000351923 * fTempPerm235) + ((0.000181872 * fTempPerm237) + ((0.00428571 * fTempPerm238) + ((0.00375098 * fTempPerm240) + ((0.004179850000000001 * fTempPerm837) + ((0.008554550000000001 * fTempPerm241) + ((0.008757339999999999 * fTempPerm242) + ((0.0022028300000000002 * fTempPerm841) + ((0.026865800000000002 * fTempPerm243) + ((0.0173968 * fTempPerm245) + ((0.00657556 * fTempPerm845) + ((0.044476699999999994 * fTempPerm246) + ((0.0194256 * fTempPerm247) + ((0.0258357 * fTempPerm249) + ((0.00505303 * fTempPerm250) + ((0.000658512 * fTempPerm251) + ((0.00033249800000000003 * fTempPerm252) + ((6.401740000000001e-05 * fTempPerm850) + ((0.00114682 * fTempPerm253) + ((0.000436107 * fTempPerm254) + ((0.000204336 * fTempPerm255) + ((7.432920000000001e-05 * fTempPerm257) + ((0.000280439 * fTempPerm857) + ((0.000409495 * fTempPerm858) + ((0.00293582 * fTempPerm860) + ((0.00144425 * fTempPerm861) + ((0.0575248 * fTempPerm862) + ((0.007368019999999999 * fTempPerm264) + ((0.0005802190000000001 * fTempPerm863) + ((0.00373067 * fTempPerm864) + ((0.000216524 * fTempPerm865) + ((0.000505773 * fTempPerm867) + ((0.00019702400000000002 * fTempPerm868) + ((6.42903e-05 * fTempPerm869) + ((5.185120000000001e-05 * fTempPerm272) + ((0.000263489 * fTempPerm871) + ((0.00048678400000000003 * fTempPerm872) + ((0.000979588 * fTempPerm873) + ((0.0012814900000000001 * fTempPerm275) + ((0.0391217 * fTempPerm874) + ((0.00167489 * fTempPerm875) + ((0.0013408299999999999 * fTempPerm877) + ((2.74177e-05 * fTempPerm878) + ((0.00152893 * fTempPerm282) + ((0.00493341 * fTempPerm880) + ((0.00263441 * fTempPerm285) + ((0.00220639 * fTempPerm883) + ((0.0143689 * fTempPerm884) + ((2.50192e-06 * fTempPerm886) + ((0.000649337 * fTempPerm290) + ((0.0009659900000000001 * fTempPerm291) + ((0.00139564 * fTempPerm292) + ((0.00205962 * fTempPerm293) + ((0.00041578400000000004 * fTempPerm294) + ((0.00256507 * fTempPerm296) + ((0.00144013 * fTempPerm297) + ((0.00247991 * fTempPerm298) + ((0.000154768 * fTempPerm299) + ((0.000984567 * fTempPerm890) + ((0.00167116 * fTempPerm300) + ((((((((((((((((0.0021273399999999997 * fTempPerm336) + ((0.00011313 * fTempPerm910) + ((((((((((((((((((((((((((((((((((((((((((((((0.000136371 * fTempPerm379) + (((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((0.0019049100000000001 * fTempPerm1041) + (((((((((((((((((((((((((((0.0433556 * fTempPerm1067) + (((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((0.0061875 * fTempPerm1138) + (((((((((((((((3.28963e-06 * fTempPerm607) + ((0.00258332 * fTempPerm608) + ((0.0049689600000000006 * fTempPerm1146) + ((0.00753143 * fTempPerm609) + ((0.000762874 * fTempPerm612) + ((0.000829505 * fTempPerm1148) + ((0.00161012 * fTempPerm614) + (0.00016274 * fTempPerm615)))))))) + (0.000274135 * fTempPerm1145)) + (0.00293953 * fTempPerm605)) + (0.000597807 * fTempPerm600)) + (0.00130908 * fTempPerm1144)) + (0.000783797 * fTempPerm1143)) + (7.89041e-07 * fTempPerm598)) + (0.00924877 * fTempPerm597)) + (0.00137873 * fTempPerm1142)) + (0.00130377 * fTempPerm595)) + (5.5551700000000006e-05 * fTempPerm1141)) + (0.0351254 * fTempPerm593)) + (0.0010152000000000002 * fTempPerm592)) + (0.00861829 * fTempPerm1139))) + (0.00190415 * fTempPerm1137)) + (0.00586852 * fTempPerm1135)) + (0.000152135 * fTempPerm1133)) + (0.0012716300000000002 * fTempPerm585)) + (0.00181733 * fTempPerm1132)) + (0.0004264770000000001 * fTempPerm617)) + (0.000394495 * fTempPerm618)) + (4.06072e-05 * fTempPerm619)) + (0.00287375 * fTempPerm1129)) + (2.5843400000000004e-05 * fTempPerm1128)) + (0.00015924 * fTempPerm620)) + (5.7022700000000005e-05 * fTempPerm1127)) + (0.00022650199999999998 * fTempPerm1126)) + (0.000175224 * fTempPerm1125)) + (6.155e-05 * fTempPerm623)) + (0.000272913 * fTempPerm1123)) + (0.00211776 * fTempPerm1122)) + (0.0422768 * fTempPerm1121)) + (0.044481599999999996 * fTempPerm1120)) + (0.019302299999999998 * fTempPerm1119)) + (0.0274978 * fTempPerm1118)) + (0.0155901 * fTempPerm1116)) + (0.0126207 * fTempPerm625)) + (0.005401220000000001 * fTempPerm1115)) + (0.006903970000000001 * fTempPerm1114)) + (0.000908465 * fTempPerm628)) + (0.0009818860000000002 * fTempPerm1110)) + (0.000181368 * fTempPerm1108)) + (0.0013939500000000001 * fTempPerm580)) + (0.00604226 * fTempPerm1104)) + (0.0015727 * fTempPerm1103)) + (0.00772571 * fTempPerm1101)) + (0.007573000000000001 * fTempPerm1100)) + (0.00177178 * fTempPerm1099)) + (0.000930466 * fTempPerm579)) + (7.7027e-06 * fTempPerm1098)) + (4.94134e-06 * fVec2[IOTA&127])) + (2.4709800000000002e-05 * fVec7[IOTA&127])) + (0.00015027 * fTempPerm577)) + (1.0426200000000002e-05 * fVec6[IOTA&127])) + (3.758e-05 * fTempPerm576)) + (1.2883700000000002e-05 * fVec4[IOTA&127])) + (2.2571e-06 * fTempPerm574)) + (0.0009207870000000001 * fTempPerm555)) + (0.0008751970000000001 * fTempPerm1095)) + (0.000611186 * fTempPerm537)) + (0.0023706 * fTempPerm536)) + (9.940880000000001e-05 * fTempPerm533)) + (0.000615973 * fTempPerm528)) + (0.000752906 * fTempPerm527)) + (0.00042180200000000004 * fTempPerm526)) + (0.00040693400000000007 * fTempPerm525)) + (2.3801800000000003e-05 * fTempPerm1082)) + (5.1123399999999996e-06 * fTempPerm524)) + (0.000270496 * fTempPerm1080)) + (0.000249192 * fTempPerm1076)) + (0.0010622000000000001 * fTempPerm1075)) + (0.00095017 * fTempPerm1073)) + (0.00021246900000000004 * fTempPerm1072)) + (0.00564598 * fTempPerm1071)) + (0.000976993 * fTempPerm520)) + (0.00184861 * fTempPerm519)) + (0.0111431 * fTempPerm1070)) + (0.0141169 * fTempPerm1069)) + (0.00249322 * fTempPerm1068))) + (0.008639049999999999 * fTempPerm1064)) + (0.00898901 * fTempPerm514)) + (0.019686699999999998 * fTempPerm1063)) + (0.00133631 * fTempPerm1062)) + (0.0114674 * fTempPerm1061)) + (0.00786783 * fTempPerm1060)) + (0.0038962 * fTempPerm508)) + (0.00259829 * fTempPerm1059)) + (0.000785113 * fTempPerm507)) + (0.0031654200000000004 * fTempPerm1058)) + (0.0018260300000000002 * fTempPerm1057)) + (0.00434699 * fTempPerm1056)) + (0.00238017 * fTempPerm1055)) + (0.0013700399999999999 * fTempPerm1053)) + (0.0010960700000000002 * fTempPerm504)) + (0.000216738 * fTempPerm502)) + (0.000384737 * fTempPerm500)) + (0.00186384 * fTempPerm1052)) + (0.000828266 * fTempPerm1051)) + (0.000855899 * fTempPerm1050)) + (0.0060285 * fTempPerm1048)) + (0.0106382 * fTempPerm1047)) + (0.00139329 * fTempPerm495)) + (0.00110707 * fTempPerm1043)) + (0.0023355800000000003 * fTempPerm1042))) + (0.000528145 * fTempPerm1040)) + (0.0022987899999999998 * fTempPerm1039)) + (0.00100318 * fTempPerm1038)) + (0.00170804 * fTempPerm1037)) + (0.0007023 * fTempPerm1036)) + (0.00273074 * fTempPerm1035)) + (0.000784948 * fTempPerm489)) + (0.00030826 * fTempPerm487)) + (0.0008454190000000001 * fTempPerm1034)) + (0.00216257 * fTempPerm486)) + (0.0107364 * fTempPerm485)) + (0.0066234300000000005 * fTempPerm484)) + (0.0012472700000000002 * fTempPerm482)) + (5.5010100000000004e-05 * fTempPerm1030)) + (8.109680000000001e-05 * fTempPerm480)) + (6.33178e-05 * fTempPerm479)) + (3.2621699999999995e-06 * fTempPerm478)) + (0.000821635 * fTempPerm1028)) + (0.026332300000000003 * fTempPerm477)) + (0.0079993 * fTempPerm476)) + (0.0161706 * fTempPerm475)) + (0.0110397 * fTempPerm472)) + (0.00801492 * fTempPerm1025)) + (0.0022649 * fTempPerm1024)) + (0.0212956 * fTempPerm1023)) + (0.000929206 * fTempPerm1021)) + (0.042661300000000006 * fTempPerm462)) + (0.0801361 * fTempPerm461)) + (0.0413938 * fTempPerm460)) + (0.0017438199999999999 * fTempPerm458)) + (0.00122218 * fTempPerm456)) + (0.000680485 * fTempPerm455)) + (0.0016457800000000001 * fTempPerm1014)) + (0.00196376 * fTempPerm1013)) + (0.0013616 * fTempPerm1012)) + (0.00275033 * fTempPerm1011)) + (0.00306353 * fTempPerm1010)) + (0.0011346700000000002 * fTempPerm454)) + (0.00043984100000000006 * fTempPerm451)) + (0.0010250300000000001 * fTempPerm448)) + (0.00103053 * fTempPerm1009)) + (0.00136555 * fTempPerm1008)) + (0.00110553 * fTempPerm1007)) + (0.0009477180000000001 * fTempPerm1006)) + (0.004373940000000001 * fTempPerm446)) + (0.00103864 * fTempPerm1004)) + (0.00050954 * fTempPerm1003)) + (0.0022861500000000002 * fTempPerm1001)) + (0.0006406130000000001 * fTempPerm1000)) + (0.00023107600000000004 * fTempPerm999)) + (0.000643591 * fTempPerm998)) + (0.00734871 * fTempPerm442)) + (0.00475943 * fTempPerm441)) + (0.0008129400000000001 * fTempPerm996)) + (0.000469171 * fTempPerm439)) + (0.004094459999999999 * fTempPerm437)) + (0.0048163 * fTempPerm436)) + (0.00122741 * fTempPerm435)) + (0.00019696800000000003 * fTempPerm990)) + (0.0012369000000000002 * fTempPerm433)) + (0.00033669 * fTempPerm431)) + (6.18528e-05 * fTempPerm430)) + (3.68663e-05 * fTempPerm429)) + (8.65485e-05 * fTempPerm428)) + (0.0020659800000000002 * fTempPerm426)) + (0.00266146 * fTempPerm985)) + (0.00193302 * fTempPerm423)) + (0.0017979 * fTempPerm422)) + (5.63033e-05 * fTempPerm421)) + (0.000544179 * fTempPerm420)) + (0.0027314400000000003 * fTempPerm417)) + (0.000682499 * fTempPerm416)) + (0.0013118799999999999 * fTempPerm415)) + (0.00031903100000000006 * fTempPerm414)) + (0.00121467 * fTempPerm412)) + (6.500930000000001e-05 * fTempPerm411)) + (0.0029169900000000004 * fTempPerm979)) + (0.00118198 * fTempPerm978)) + (0.0004906380000000001 * fTempPerm406)) + (0.000199826 * fTempPerm403)) + (0.0009658490000000001 * fTempPerm977)) + (0.000364334 * fTempPerm975)) + (0.000316498 * fTempPerm974)) + (0.00013887600000000002 * fTempPerm973)) + (0.000178013 * fTempPerm399)) + (7.977530000000001e-05 * fTempPerm972)) + (0.00013697 * fTempPerm971)) + (5.84711e-05 * fTempPerm970)) + (6.86851e-05 * fTempPerm969)) + (0.00557668 * fTempPerm396)) + (6.501340000000001e-05 * fTempPerm395)) + (0.000640087 * fTempPerm393)) + (0.006581140000000001 * fTempPerm390)) + (0.0008643610000000001 * fTempPerm389)) + (7.49829e-05 * fTempPerm385)) + (0.00011503900000000001 * fTempPerm964)) + (0.000359207 * fTempPerm963)) + (0.000908412 * fTempPerm384)) + (0.00031580000000000003 * fTempPerm961)) + (0.000588187 * fTempPerm383)) + (0.00257691 * fTempPerm960)) + (0.00020847000000000004 * fTempPerm959)) + (0.00122724 * fTempPerm958))) + (0.0105223 * fTempPerm378)) + (0.00214593 * fTempPerm957)) + (0.014119 * fTempPerm956)) + (0.00631691 * fTempPerm955)) + (0.009370129999999999 * fTempPerm954)) + (0.010395300000000001 * fTempPerm953)) + (0.004597660000000001 * fTempPerm952)) + (0.00432975 * fTempPerm951)) + (0.00265571 * fTempPerm377)) + (0.00489879 * fTempPerm950)) + (0.000712925 * fTempPerm375)) + (0.00020890300000000003 * fTempPerm949)) + (0.000380057 * fTempPerm947)) + (0.00237411 * fTempPerm946)) + (0.00184137 * fTempPerm945)) + (0.00133293 * fTempPerm373)) + (0.00048340500000000007 * fTempPerm943)) + (0.00023814000000000002 * fTempPerm942)) + (0.000705638 * fTempPerm941)) + (0.00011808300000000001 * fTempPerm940)) + (0.000227988 * fTempPerm939)) + (4.2778300000000003e-05 * fTempPerm372)) + (6.29344e-05 * fTempPerm938)) + (2.03832e-05 * fTempPerm937)) + (1.3287400000000001e-05 * fVec0[IOTA&127])) + (6.44951e-05 * fTempPerm936)) + (0.000160754 * fTempPerm935)) + (0.0006331710000000001 * fTempPerm356)) + (1.7742e-05 * fTempPerm932)) + (0.00196204 * fTempPerm931)) + (1.47262e-06 * fTempPerm353)) + (0.00101019 * fTempPerm930)) + (0.00072128 * fTempPerm346)) + (0.00149621 * fTempPerm929)) + (0.0784899 * fTempPerm345)) + (0.0006735780000000001 * fTempPerm344)) + (0.0829356 * fTempPerm343)) + (0.064767 * fTempPerm342)) + (0.0772965 * fTempPerm341)) + (0.040092800000000005 * fTempPerm340)) + (0.0977876 * fTempPerm339)) + (0.0531961 * fTempPerm338)) + (0.00881751 * fTempPerm337)) + (0.0011553 * fTempPerm911)))) + (0.0017169000000000002 * fTempPerm335)) + (0.00069698 * fTempPerm334)) + (1.2413700000000001e-05 * fVec3[IOTA&127])) + (3.4700500000000004e-05 * fTempPerm333)) + (0.00036222200000000004 * fTempPerm326)) + (0.000687957 * fTempPerm322)) + (0.00048385 * fTempPerm320)) + (0.000473651 * fTempPerm907)) + (0.000851532 * fTempPerm306)) + (0.000849503 * fTempPerm305)) + (0.0010723199999999999 * fTempPerm304)) + (0.000429699 * fTempPerm905)) + (0.000453602 * fTempPerm303)) + (0.00188932 * fTempPerm302)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0007145500000000001 * fTempPerm208)) + (0.000923679 * fTempPerm207)) + (7.65374e-05 * fTempPerm206)) + (0.00457394 * fTempPerm821)) + (5.1858400000000005e-05 * fTempPerm205)) + (0.0050915 * fTempPerm820)) + (2.9137400000000003e-05 * fTempPerm204)) + (0.000603544 * fTempPerm817)) + (0.00440538 * fTempPerm816)) + (0.000145224 * fTempPerm195)) + (0.0006723230000000001 * fTempPerm812)) + (0.00140716 * fTempPerm811)) + (0.000370598 * fTempPerm194)) + (3.22237e-05 * fTempPerm810)) + (0.0144406 * fTempPerm809)) + (0.013951100000000001 * fTempPerm808)) + (0.009656040000000001 * fTempPerm806))) + (0.00010184400000000001 * fTempPerm804))) + (5.39788e-06 * fTempPerm802))) + (0.0042838500000000005 * fTempPerm188)) + (0.0292933 * fTempPerm187)) + (0.00349277 * fTempPerm800)) + (0.00113243 * fTempPerm186)) + (0.00164622 * fTempPerm799)) + (0.00080034 * fTempPerm185)) + (0.00186575 * fTempPerm183)) + (0.0011732700000000001 * fTempPerm797)) + (0.000193746 * fTempPerm181)) + (0.000168486 * fTempPerm795)) + (1.3387100000000001e-05 * fTempPerm794)) + (9.74875e-05 * fTempPerm178)) + (4.96364e-06 * fTempPerm793)) + (0.00333511 * fTempPerm792)) + (0.00292244 * fTempPerm790))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
			fRec12[0] = max((fRec12[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm1153))))));
			fbargraph10 = fRec12[0];
			output1[i] = (FAUSTFLOAT)fTempPerm1153;
			// post processing
			fRec12[1] = fRec12[0];
			fRec0[1] = fRec0[0];
			fRec11[1] = fRec11[0];
			fRec10[1] = fRec10[0];
			fRec9[1] = fRec9[0];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
			fRec6[1] = fRec6[0];
			fRec5[1] = fRec5[0];
			fRec4[1] = fRec4[0];
			IOTA = IOTA+1;
			fRec2[1] = fRec2[0];
			fRec3[1] = fRec3[0];
			fRec1[1] = fRec1[0];
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

#ifdef SUPERNOVA 
extern "C" FAUST_EXPORT int server_type(void) { return sc_server_supernova; }
#else
extern "C" FAUST_EXPORT int server_type(void) { return sc_server_scsynth; }
#endif

// EOF
