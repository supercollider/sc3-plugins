//----------------------------------------------------------
// name: "HOADecoderLebedev50Binaural1"
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
	double 	fTempPerm13;
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
	double 	fTempPerm32;
	double 	fTempPerm33;
	double 	fRec4[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fTempPerm34;
	double 	fVec1[128];
	double 	fTempPerm35;
	double 	fTempPerm36;
	double 	fTempPerm37;
	double 	fTempPerm38;
	double 	fTempPerm39;
	double 	fTempPerm40;
	double 	fTempPerm41;
	double 	fTempPerm42;
	double 	fTempPerm43;
	double 	fTempPerm44;
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
	double 	fRec5[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fTempPerm62;
	double 	fVec2[128];
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
	double 	fTempPerm116;
	double 	fTempPerm117;
	double 	fTempPerm118;
	double 	fTempPerm119;
	double 	fTempPerm120;
	double 	fTempPerm121;
	double 	fTempPerm122;
	double 	fTempPerm123;
	double 	fTempPerm124;
	double 	fTempPerm125;
	double 	fTempPerm126;
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
	double 	fRec6[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fTempPerm137;
	double 	fVec3[128];
	double 	fTempPerm138;
	double 	fTempPerm139;
	double 	fTempPerm140;
	double 	fTempPerm141;
	double 	fTempPerm142;
	double 	fTempPerm143;
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
	double 	fTempPerm161;
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
	double 	fTempPerm308;
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
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fTempPerm513;
	double 	fRec7[2];
	FAUSTFLOAT 	fbargraph5;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOADecoderLebedev50Binaural1");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL)");
		m->declare("copyright", "(c) Pierre Lecomte 2015");
		m->declare("gui.lib/name", "GUI Library");
		m->declare("gui.lib/version", "1.0");
		m->declare("gui.lib/author", "Pierre Lecomte");
		m->declare("gui.lib/license", "GPL");
		m->declare("gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("signals.lib/version", "0.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
	}

	virtual int getNumInputs() { return 4; }
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
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
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
		ui_interface->openVerticalBox("HOADecoderLebedev50Binaural1");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fbe77630060", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fbe7753eb50", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fbe77536ed0", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fbe7762b4e0", &fbargraph0, -7e+01, 6.0);
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
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addHorizontalBargraph("0x7fbe77492670", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Right");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addHorizontalBargraph("0x7fbe774a3a50", &fbargraph5, -7e+01, 6.0);
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
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		//LoopGraphScalar
		for (int i=0; i<count; i++) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			fRec3[0] = (fSlow1 + (0.999 * fRec3[1]));
			fTempPerm0 = ((double)input3[i] * fRec3[0]);
			fRec2[0] = max((fRec2[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm0))))));
			fbargraph0 = fRec2[0];
			fTempPerm1 = fTempPerm0;
			fVec0[IOTA&127] = fTempPerm1;
			fTempPerm2 = fVec0[(IOTA-122)&127];
			fTempPerm3 = fVec0[(IOTA-121)&127];
			fTempPerm4 = fVec0[(IOTA-119)&127];
			fTempPerm5 = fVec0[(IOTA-87)&127];
			fTempPerm6 = fVec0[(IOTA-95)&127];
			fTempPerm7 = fVec0[(IOTA-84)&127];
			fTempPerm8 = fVec0[(IOTA-83)&127];
			fTempPerm9 = fVec0[(IOTA-86)&127];
			fTempPerm10 = fVec0[(IOTA-85)&127];
			fTempPerm11 = fVec0[(IOTA-65)&127];
			fTempPerm12 = fVec0[(IOTA-52)&127];
			fTempPerm13 = fVec0[(IOTA-51)&127];
			fTempPerm14 = fVec0[(IOTA-49)&127];
			fTempPerm15 = fVec0[(IOTA-48)&127];
			fTempPerm16 = fVec0[(IOTA-53)&127];
			fTempPerm17 = fVec0[(IOTA-54)&127];
			fTempPerm18 = fVec0[(IOTA-44)&127];
			fTempPerm19 = fVec0[(IOTA-38)&127];
			fTempPerm20 = fVec0[(IOTA-37)&127];
			fTempPerm21 = fVec0[(IOTA-36)&127];
			fTempPerm22 = fVec0[(IOTA-45)&127];
			fTempPerm23 = fVec0[(IOTA-33)&127];
			fTempPerm24 = fVec0[(IOTA-18)&127];
			fTempPerm25 = fVec0[(IOTA-17)&127];
			fTempPerm26 = fVec0[(IOTA-16)&127];
			fTempPerm27 = fVec0[(IOTA-4)&127];
			fTempPerm28 = fVec0[(IOTA-2)&127];
			fTempPerm29 = fVec0[(IOTA-34)&127];
			fTempPerm30 = fVec0[(IOTA-26)&127];
			fTempPerm31 = fVec0[(IOTA-23)&127];
			fTempPerm32 = fVec0[(IOTA-20)&127];
			fTempPerm33 = ((double)input2[i] * fRec3[0]);
			fRec4[0] = max((fRec4[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm33))))));
			fbargraph1 = fRec4[0];
			fTempPerm34 = fTempPerm33;
			fVec1[IOTA&127] = fTempPerm34;
			fTempPerm35 = fVec1[(IOTA-126)&127];
			fTempPerm36 = fVec0[(IOTA-30)&127];
			fTempPerm37 = fVec0[(IOTA-22)&127];
			fTempPerm38 = fVec1[(IOTA-125)&127];
			fTempPerm39 = fVec1[(IOTA-123)&127];
			fTempPerm40 = fVec1[(IOTA-121)&127];
			fTempPerm41 = fVec1[(IOTA-94)&127];
			fTempPerm42 = fVec1[(IOTA-92)&127];
			fTempPerm43 = fVec1[(IOTA-86)&127];
			fTempPerm44 = fVec1[(IOTA-60)&127];
			fTempPerm45 = fVec1[(IOTA-59)&127];
			fTempPerm46 = fVec1[(IOTA-58)&127];
			fTempPerm47 = fVec1[(IOTA-83)&127];
			fTempPerm48 = fVec1[(IOTA-47)&127];
			fTempPerm49 = fVec1[(IOTA-26)&127];
			fTempPerm50 = fVec1[(IOTA-28)&127];
			fTempPerm51 = fVec1[(IOTA-24)&127];
			fTempPerm52 = fVec1[(IOTA-21)&127];
			fTempPerm53 = fVec1[(IOTA-20)&127];
			fTempPerm54 = fVec1[(IOTA-17)&127];
			fTempPerm55 = fVec1[(IOTA-14)&127];
			fTempPerm56 = fVec1[(IOTA-13)&127];
			fTempPerm57 = fVec1[(IOTA-8)&127];
			fTempPerm58 = fVec1[(IOTA-5)&127];
			fTempPerm59 = fVec1[(IOTA-4)&127];
			fTempPerm60 = fVec1[(IOTA-2)&127];
			fTempPerm61 = ((double)input1[i] * fRec3[0]);
			fRec5[0] = max((fRec5[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm61))))));
			fbargraph2 = fRec5[0];
			fTempPerm62 = fTempPerm61;
			fVec2[IOTA&127] = fTempPerm62;
			fTempPerm63 = fVec2[(IOTA-105)&127];
			fTempPerm64 = fVec2[(IOTA-103)&127];
			fTempPerm65 = fVec2[(IOTA-96)&127];
			fTempPerm66 = fVec2[(IOTA-104)&127];
			fTempPerm67 = fVec2[(IOTA-100)&127];
			fTempPerm68 = fVec2[(IOTA-92)&127];
			fTempPerm69 = fVec2[(IOTA-97)&127];
			fTempPerm70 = fVec0[(IOTA-126)&127];
			fTempPerm71 = fVec0[(IOTA-90)&127];
			fTempPerm72 = fVec0[(IOTA-63)&127];
			fTempPerm73 = fVec0[(IOTA-62)&127];
			fTempPerm74 = fVec0[(IOTA-64)&127];
			fTempPerm75 = fVec0[(IOTA-57)&127];
			fTempPerm76 = fVec0[(IOTA-43)&127];
			fTempPerm77 = fVec0[(IOTA-42)&127];
			fTempPerm78 = fVec0[(IOTA-39)&127];
			fTempPerm79 = fVec1[(IOTA-119)&127];
			fTempPerm80 = fVec1[(IOTA-117)&127];
			fTempPerm81 = fVec1[(IOTA-75)&127];
			fTempPerm82 = fVec1[(IOTA-87)&127];
			fTempPerm83 = fVec1[(IOTA-63)&127];
			fTempPerm84 = fVec1[(IOTA-62)&127];
			fTempPerm85 = fVec1[(IOTA-54)&127];
			fTempPerm86 = fVec1[(IOTA-53)&127];
			fTempPerm87 = fVec1[(IOTA-52)&127];
			fTempPerm88 = fVec1[(IOTA-51)&127];
			fTempPerm89 = fVec1[(IOTA-50)&127];
			fTempPerm90 = fVec1[(IOTA-48)&127];
			fTempPerm91 = fVec1[(IOTA-46)&127];
			fTempPerm92 = fVec1[(IOTA-45)&127];
			fTempPerm93 = fVec2[(IOTA-114)&127];
			fTempPerm94 = fVec2[(IOTA-107)&127];
			fTempPerm95 = fVec2[(IOTA-98)&127];
			fTempPerm96 = fVec2[(IOTA-95)&127];
			fTempPerm97 = fVec2[(IOTA-94)&127];
			fTempPerm98 = fVec2[(IOTA-89)&127];
			fTempPerm99 = fVec2[(IOTA-85)&127];
			fTempPerm100 = fVec2[(IOTA-83)&127];
			fTempPerm101 = fVec2[(IOTA-86)&127];
			fTempPerm102 = fVec2[(IOTA-84)&127];
			fTempPerm103 = fVec0[(IOTA-125)&127];
			fTempPerm104 = fVec0[(IOTA-123)&127];
			fTempPerm105 = fVec0[(IOTA-117)&127];
			fTempPerm106 = fVec0[(IOTA-116)&127];
			fTempPerm107 = fVec0[(IOTA-115)&127];
			fTempPerm108 = fVec0[(IOTA-91)&127];
			fTempPerm109 = fVec1[(IOTA-124)&127];
			fTempPerm110 = fVec1[(IOTA-120)&127];
			fTempPerm111 = fVec1[(IOTA-98)&127];
			fTempPerm112 = fVec1[(IOTA-116)&127];
			fTempPerm113 = fVec1[(IOTA-80)&127];
			fTempPerm114 = fVec1[(IOTA-78)&127];
			fTempPerm115 = fVec1[(IOTA-77)&127];
			fTempPerm116 = fVec1[(IOTA-97)&127];
			fTempPerm117 = fVec1[(IOTA-44)&127];
			fTempPerm118 = fVec1[(IOTA-42)&127];
			fTempPerm119 = fVec1[(IOTA-41)&127];
			fTempPerm120 = fVec1[(IOTA-79)&127];
			fTempPerm121 = fVec2[(IOTA-93)&127];
			fTempPerm122 = fVec2[(IOTA-79)&127];
			fTempPerm123 = fVec2[(IOTA-78)&127];
			fTempPerm124 = fVec2[(IOTA-77)&127];
			fTempPerm125 = fVec2[(IOTA-19)&127];
			fTempPerm126 = fVec2[(IOTA-18)&127];
			fTempPerm127 = fVec2[(IOTA-16)&127];
			fTempPerm128 = fVec2[(IOTA-17)&127];
			fTempPerm129 = fVec2[(IOTA-15)&127];
			fTempPerm130 = fVec2[(IOTA-14)&127];
			fTempPerm131 = fVec2[(IOTA-13)&127];
			fTempPerm132 = fVec2[(IOTA-11)&127];
			fTempPerm133 = fVec2[(IOTA-12)&127];
			fTempPerm134 = fVec2[(IOTA-9)&127];
			fTempPerm135 = fVec2[(IOTA-7)&127];
			fTempPerm136 = ((double)input0[i] * fRec3[0]);
			fRec6[0] = max((fRec6[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm136))))));
			fbargraph3 = fRec6[0];
			fTempPerm137 = fTempPerm136;
			fVec3[IOTA&127] = fTempPerm137;
			fTempPerm138 = fVec3[(IOTA-121)&127];
			fTempPerm139 = fVec3[(IOTA-113)&127];
			fTempPerm140 = fVec3[(IOTA-98)&127];
			fTempPerm141 = fVec3[(IOTA-103)&127];
			fTempPerm142 = fVec3[(IOTA-114)&127];
			fTempPerm143 = fVec3[(IOTA-93)&127];
			fTempPerm144 = fVec3[(IOTA-85)&127];
			fTempPerm145 = fVec3[(IOTA-96)&127];
			fTempPerm146 = fVec3[(IOTA-95)&127];
			fTempPerm147 = fVec3[(IOTA-83)&127];
			fTempPerm148 = fVec3[(IOTA-65)&127];
			fTempPerm149 = fVec3[(IOTA-94)&127];
			fTempPerm150 = fVec3[(IOTA-75)&127];
			fTempPerm151 = fVec3[(IOTA-64)&127];
			fTempPerm152 = fVec3[(IOTA-63)&127];
			fTempPerm153 = fVec3[(IOTA-57)&127];
			fTempPerm154 = fVec3[(IOTA-38)&127];
			fTempPerm155 = fVec3[(IOTA-41)&127];
			fTempPerm156 = fVec3[(IOTA-42)&127];
			fTempPerm157 = fVec3[(IOTA-39)&127];
			fTempPerm158 = fVec3[(IOTA-40)&127];
			fTempPerm159 = fVec3[(IOTA-46)&127];
			fTempPerm160 = fVec3[(IOTA-43)&127];
			fTempPerm161 = fVec3[(IOTA-44)&127];
			fTempPerm162 = fVec3[(IOTA-45)&127];
			fTempPerm163 = fVec2[(IOTA-3)&127];
			fTempPerm164 = fVec3[(IOTA-123)&127];
			fTempPerm165 = fVec3[(IOTA-112)&127];
			fTempPerm166 = fVec3[(IOTA-116)&127];
			fTempPerm167 = fVec3[(IOTA-111)&127];
			fTempPerm168 = fVec3[(IOTA-110)&127];
			fTempPerm169 = fVec3[(IOTA-109)&127];
			fTempPerm170 = fVec3[(IOTA-108)&127];
			fTempPerm171 = fVec3[(IOTA-105)&127];
			fTempPerm172 = fVec3[(IOTA-104)&127];
			fTempPerm173 = fVec3[(IOTA-101)&127];
			fTempPerm174 = fVec3[(IOTA-74)&127];
			fTempPerm175 = fVec3[(IOTA-73)&127];
			fTempPerm176 = fVec3[(IOTA-67)&127];
			fTempPerm177 = fVec3[(IOTA-66)&127];
			fTempPerm178 = fVec3[(IOTA-56)&127];
			fTempPerm179 = fVec3[(IOTA-37)&127];
			fTempPerm180 = fVec3[(IOTA-27)&127];
			fTempPerm181 = fVec0[(IOTA-1)&127];
			fTempPerm182 = fVec1[(IOTA-1)&127];
			fTempPerm183 = fVec2[(IOTA-1)&127];
			fTempPerm184 = fVec0[(IOTA-56)&127];
			fTempPerm185 = fVec0[(IOTA-55)&127];
			fTempPerm186 = fVec1[(IOTA-122)&127];
			fTempPerm187 = fVec1[(IOTA-118)&127];
			fTempPerm188 = fVec1[(IOTA-93)&127];
			fTempPerm189 = fVec1[(IOTA-89)&127];
			fTempPerm190 = fVec1[(IOTA-85)&127];
			fTempPerm191 = fVec1[(IOTA-88)&127];
			fTempPerm192 = fVec1[(IOTA-84)&127];
			fTempPerm193 = fVec1[(IOTA-39)&127];
			fTempPerm194 = fVec1[(IOTA-40)&127];
			fTempPerm195 = fVec1[(IOTA-36)&127];
			fTempPerm196 = fVec1[(IOTA-33)&127];
			fTempPerm197 = fVec1[(IOTA-29)&127];
			fTempPerm198 = fVec2[(IOTA-102)&127];
			fTempPerm199 = fVec2[(IOTA-101)&127];
			fTempPerm200 = fVec2[(IOTA-99)&127];
			fTempPerm201 = fVec1[(IOTA-35)&127];
			fTempPerm202 = fVec1[(IOTA-32)&127];
			fTempPerm203 = fVec2[(IOTA-87)&127];
			fTempPerm204 = fVec2[(IOTA-71)&127];
			fTempPerm205 = fVec2[(IOTA-82)&127];
			fTempPerm206 = fVec0[(IOTA-29)&127];
			fTempPerm207 = fVec2[(IOTA-22)&127];
			fTempPerm208 = fVec2[(IOTA-90)&127];
			fTempPerm209 = fVec1[(IOTA-90)&127];
			fTempPerm210 = fVec3[(IOTA-118)&127];
			fTempPerm211 = fVec3[(IOTA-117)&127];
			fTempPerm212 = fVec3[(IOTA-119)&127];
			fTempPerm213 = fVec2[(IOTA-5)&127];
			fTempPerm214 = fVec3[(IOTA-48)&127];
			fTempPerm215 = fVec3[(IOTA-53)&127];
			fTempPerm216 = fVec3[(IOTA-58)&127];
			fTempPerm217 = fVec3[(IOTA-59)&127];
			fTempPerm218 = fVec3[(IOTA-54)&127];
			fTempPerm219 = fVec3[(IOTA-55)&127];
			fTempPerm220 = fVec3[(IOTA-68)&127];
			fTempPerm221 = fVec3[(IOTA-60)&127];
			fTempPerm222 = fVec3[(IOTA-61)&127];
			fTempPerm223 = fVec3[(IOTA-90)&127];
			fTempPerm224 = fVec3[(IOTA-69)&127];
			fTempPerm225 = fVec3[(IOTA-72)&127];
			fTempPerm226 = fVec3[(IOTA-80)&127];
			fTempPerm227 = fVec3[(IOTA-86)&127];
			fTempPerm228 = fVec3[(IOTA-99)&127];
			fTempPerm229 = fVec3[(IOTA-106)&127];
			fTempPerm230 = fVec3[(IOTA-107)&127];
			fTempPerm231 = fVec3[(IOTA-100)&127];
			fTempPerm232 = fVec3[(IOTA-120)&127];
			fTempPerm233 = fVec3[(IOTA-122)&127];
			fTempPerm234 = fVec3[(IOTA-124)&127];
			fTempPerm235 = fVec2[(IOTA-4)&127];
			fTempPerm236 = fVec2[(IOTA-73)&127];
			fTempPerm237 = fVec2[(IOTA-88)&127];
			fTempPerm238 = fVec2[(IOTA-80)&127];
			fTempPerm239 = fVec3[(IOTA-1)&127];
			fTempPerm240 = fVec3[(IOTA-3)&127];
			fTempPerm241 = fVec3[(IOTA-4)&127];
			fTempPerm242 = fVec3[(IOTA-5)&127];
			fTempPerm243 = fVec3[(IOTA-6)&127];
			fTempPerm244 = fVec3[(IOTA-7)&127];
			fTempPerm245 = fVec3[(IOTA-8)&127];
			fTempPerm246 = fVec3[(IOTA-9)&127];
			fTempPerm247 = fVec3[(IOTA-10)&127];
			fTempPerm248 = fVec3[(IOTA-11)&127];
			fTempPerm249 = fVec3[(IOTA-12)&127];
			fTempPerm250 = fVec3[(IOTA-13)&127];
			fTempPerm251 = fVec3[(IOTA-14)&127];
			fTempPerm252 = fVec3[(IOTA-15)&127];
			fTempPerm253 = fVec3[(IOTA-16)&127];
			fTempPerm254 = fVec3[(IOTA-17)&127];
			fTempPerm255 = fVec3[(IOTA-18)&127];
			fTempPerm256 = fVec3[(IOTA-19)&127];
			fTempPerm257 = fVec3[(IOTA-20)&127];
			fTempPerm258 = fVec3[(IOTA-21)&127];
			fTempPerm259 = fVec3[(IOTA-22)&127];
			fTempPerm260 = fVec3[(IOTA-23)&127];
			fTempPerm261 = fVec3[(IOTA-25)&127];
			fTempPerm262 = fVec3[(IOTA-26)&127];
			fTempPerm263 = fVec3[(IOTA-29)&127];
			fTempPerm264 = fVec3[(IOTA-30)&127];
			fTempPerm265 = fVec3[(IOTA-50)&127];
			fTempPerm266 = fVec3[(IOTA-31)&127];
			fTempPerm267 = fVec3[(IOTA-32)&127];
			fTempPerm268 = fVec3[(IOTA-33)&127];
			fTempPerm269 = fVec3[(IOTA-34)&127];
			fTempPerm270 = fVec3[(IOTA-35)&127];
			fTempPerm271 = fVec3[(IOTA-36)&127];
			fTempPerm272 = fVec3[(IOTA-49)&127];
			fTempPerm273 = fVec3[(IOTA-51)&127];
			fTempPerm274 = fVec3[(IOTA-52)&127];
			fTempPerm275 = fVec3[(IOTA-62)&127];
			fTempPerm276 = fVec3[(IOTA-70)&127];
			fTempPerm277 = fVec3[(IOTA-71)&127];
			fTempPerm278 = fVec3[(IOTA-76)&127];
			fTempPerm279 = fVec3[(IOTA-77)&127];
			fTempPerm280 = fVec3[(IOTA-78)&127];
			fTempPerm281 = fVec3[(IOTA-79)&127];
			fTempPerm282 = fVec3[(IOTA-82)&127];
			fTempPerm283 = fVec3[(IOTA-87)&127];
			fTempPerm284 = fVec3[(IOTA-115)&127];
			fTempPerm285 = fVec3[(IOTA-102)&127];
			fTempPerm286 = fVec3[(IOTA-125)&127];
			fTempPerm287 = fVec3[(IOTA-126)&127];
			fTempPerm288 = fVec2[(IOTA-20)&127];
			fTempPerm289 = fVec2[(IOTA-21)&127];
			fTempPerm290 = fVec2[(IOTA-72)&127];
			fTempPerm291 = fVec0[(IOTA-6)&127];
			fTempPerm292 = fVec0[(IOTA-8)&127];
			fTempPerm293 = fVec0[(IOTA-10)&127];
			fTempPerm294 = fVec0[(IOTA-13)&127];
			fTempPerm295 = fVec0[(IOTA-15)&127];
			fTempPerm296 = fVec0[(IOTA-94)&127];
			fTempPerm297 = fVec0[(IOTA-96)&127];
			fTempPerm298 = fVec0[(IOTA-120)&127];
			fTempPerm299 = fVec0[(IOTA-118)&127];
			fTempPerm300 = fVec0[(IOTA-112)&127];
			fTempPerm301 = fVec0[(IOTA-109)&127];
			fTempPerm302 = fVec0[(IOTA-110)&127];
			fTempPerm303 = fVec0[(IOTA-99)&127];
			fTempPerm304 = fVec0[(IOTA-88)&127];
			fTempPerm305 = fVec0[(IOTA-100)&127];
			fTempPerm306 = fVec0[(IOTA-89)&127];
			fTempPerm307 = fVec0[(IOTA-68)&127];
			fTempPerm308 = fVec0[(IOTA-79)&127];
			fTempPerm309 = fVec0[(IOTA-66)&127];
			fTempPerm310 = fVec0[(IOTA-67)&127];
			fTempPerm311 = fVec0[(IOTA-60)&127];
			fTempPerm312 = fVec0[(IOTA-50)&127];
			fTempPerm313 = fVec0[(IOTA-47)&127];
			fTempPerm314 = fVec0[(IOTA-59)&127];
			fTempPerm315 = fVec0[(IOTA-46)&127];
			fTempPerm316 = fVec0[(IOTA-35)&127];
			fTempPerm317 = fVec0[(IOTA-5)&127];
			fTempPerm318 = fVec0[(IOTA-3)&127];
			fTempPerm319 = fVec0[(IOTA-32)&127];
			fTempPerm320 = fVec0[(IOTA-25)&127];
			fTempPerm321 = fVec0[(IOTA-21)&127];
			fTempPerm322 = fVec0[(IOTA-19)&127];
			fTempPerm323 = fVec0[(IOTA-24)&127];
			fTempPerm324 = fVec1[(IOTA-110)&127];
			fTempPerm325 = fVec1[(IOTA-107)&127];
			fTempPerm326 = fVec1[(IOTA-105)&127];
			fTempPerm327 = fVec1[(IOTA-103)&127];
			fTempPerm328 = fVec1[(IOTA-104)&127];
			fTempPerm329 = fVec1[(IOTA-106)&127];
			fTempPerm330 = fVec1[(IOTA-68)&127];
			fTempPerm331 = fVec1[(IOTA-57)&127];
			fTempPerm332 = fVec1[(IOTA-56)&127];
			fTempPerm333 = fVec1[(IOTA-66)&127];
			fTempPerm334 = fVec1[(IOTA-61)&127];
			fTempPerm335 = fVec1[(IOTA-67)&127];
			fTempPerm336 = fVec1[(IOTA-55)&127];
			fTempPerm337 = fVec1[(IOTA-27)&127];
			fTempPerm338 = fVec1[(IOTA-25)&127];
			fTempPerm339 = fVec1[(IOTA-23)&127];
			fTempPerm340 = fVec1[(IOTA-22)&127];
			fTempPerm341 = fVec1[(IOTA-19)&127];
			fTempPerm342 = fVec1[(IOTA-18)&127];
			fTempPerm343 = fVec1[(IOTA-16)&127];
			fTempPerm344 = fVec1[(IOTA-15)&127];
			fTempPerm345 = fVec1[(IOTA-12)&127];
			fTempPerm346 = fVec1[(IOTA-11)&127];
			fTempPerm347 = fVec1[(IOTA-10)&127];
			fTempPerm348 = fVec1[(IOTA-9)&127];
			fTempPerm349 = fVec1[(IOTA-7)&127];
			fTempPerm350 = fVec1[(IOTA-6)&127];
			fTempPerm351 = fVec1[(IOTA-3)&127];
			fTempPerm352 = fVec2[(IOTA-110)&127];
			fTempPerm353 = fVec2[(IOTA-122)&127];
			fTempPerm354 = fVec2[(IOTA-91)&127];
			fTempPerm355 = fVec2[(IOTA-81)&127];
			fTempPerm356 = fVec0[(IOTA-124)&127];
			fTempPerm357 = fVec0[(IOTA-114)&127];
			fTempPerm358 = fVec0[(IOTA-105)&127];
			fTempPerm359 = fVec0[(IOTA-106)&127];
			fTempPerm360 = fVec0[(IOTA-102)&127];
			fTempPerm361 = fVec0[(IOTA-101)&127];
			fTempPerm362 = fVec0[(IOTA-76)&127];
			fTempPerm363 = fVec0[(IOTA-40)&127];
			fTempPerm364 = fVec0[(IOTA-41)&127];
			fTempPerm365 = fVec0[(IOTA-58)&127];
			fTempPerm366 = fVec1[(IOTA-114)&127];
			fTempPerm367 = fVec1[(IOTA-115)&127];
			fTempPerm368 = fVec1[(IOTA-112)&127];
			fTempPerm369 = fVec1[(IOTA-111)&127];
			fTempPerm370 = fVec1[(IOTA-102)&127];
			fTempPerm371 = fVec1[(IOTA-100)&127];
			fTempPerm372 = fVec1[(IOTA-99)&127];
			fTempPerm373 = fVec1[(IOTA-76)&127];
			fTempPerm374 = fVec1[(IOTA-95)&127];
			fTempPerm375 = fVec1[(IOTA-64)&127];
			fTempPerm376 = fVec1[(IOTA-65)&127];
			fTempPerm377 = fVec1[(IOTA-49)&127];
			fTempPerm378 = fVec2[(IOTA-126)&127];
			fTempPerm379 = fVec2[(IOTA-119)&127];
			fTempPerm380 = fVec2[(IOTA-116)&127];
			fTempPerm381 = fVec2[(IOTA-112)&127];
			fTempPerm382 = fVec2[(IOTA-111)&127];
			fTempPerm383 = fVec2[(IOTA-106)&127];
			fTempPerm384 = fVec2[(IOTA-109)&127];
			fTempPerm385 = fVec2[(IOTA-108)&127];
			fTempPerm386 = fVec2[(IOTA-113)&127];
			fTempPerm387 = fVec2[(IOTA-63)&127];
			fTempPerm388 = fVec2[(IOTA-54)&127];
			fTempPerm389 = fVec2[(IOTA-52)&127];
			fTempPerm390 = fVec2[(IOTA-65)&127];
			fTempPerm391 = fVec2[(IOTA-64)&127];
			fTempPerm392 = fVec2[(IOTA-53)&127];
			fTempPerm393 = fVec2[(IOTA-51)&127];
			fTempPerm394 = fVec2[(IOTA-50)&127];
			fTempPerm395 = fVec2[(IOTA-49)&127];
			fTempPerm396 = fVec0[(IOTA-111)&127];
			fTempPerm397 = fVec0[(IOTA-107)&127];
			fTempPerm398 = fVec0[(IOTA-104)&127];
			fTempPerm399 = fVec0[(IOTA-77)&127];
			fTempPerm400 = fVec0[(IOTA-75)&127];
			fTempPerm401 = fVec0[(IOTA-97)&127];
			fTempPerm402 = fVec0[(IOTA-72)&127];
			fTempPerm403 = fVec0[(IOTA-71)&127];
			fTempPerm404 = fVec0[(IOTA-70)&127];
			fTempPerm405 = fVec0[(IOTA-80)&127];
			fTempPerm406 = fVec0[(IOTA-73)&127];
			fTempPerm407 = fVec0[(IOTA-74)&127];
			fTempPerm408 = fVec0[(IOTA-78)&127];
			fTempPerm409 = fVec0[(IOTA-69)&127];
			fTempPerm410 = fVec1[(IOTA-109)&127];
			fTempPerm411 = fVec1[(IOTA-96)&127];
			fTempPerm412 = fVec1[(IOTA-70)&127];
			fTempPerm413 = fVec1[(IOTA-69)&127];
			fTempPerm414 = fVec1[(IOTA-43)&127];
			fTempPerm415 = fVec2[(IOTA-125)&127];
			fTempPerm416 = fVec2[(IOTA-117)&127];
			fTempPerm417 = fVec1[(IOTA-82)&127];
			fTempPerm418 = fVec2[(IOTA-121)&127];
			fTempPerm419 = fVec2[(IOTA-120)&127];
			fTempPerm420 = fVec2[(IOTA-55)&127];
			fTempPerm421 = fVec2[(IOTA-35)&127];
			fTempPerm422 = fVec2[(IOTA-29)&127];
			fTempPerm423 = fVec2[(IOTA-30)&127];
			fTempPerm424 = fVec2[(IOTA-28)&127];
			fTempPerm425 = fVec2[(IOTA-27)&127];
			fTempPerm426 = fVec2[(IOTA-10)&127];
			fTempPerm427 = fVec2[(IOTA-8)&127];
			fTempPerm428 = fVec2[(IOTA-6)&127];
			fTempPerm429 = fVec3[(IOTA-84)&127];
			fTempPerm430 = fVec2[(IOTA-2)&127];
			fTempPerm431 = fVec3[(IOTA-92)&127];
			fTempPerm432 = fVec3[(IOTA-91)&127];
			fTempPerm433 = fVec3[(IOTA-28)&127];
			fTempPerm434 = fVec0[(IOTA-103)&127];
			fTempPerm435 = fVec0[(IOTA-93)&127];
			fTempPerm436 = fVec0[(IOTA-61)&127];
			fTempPerm437 = fVec0[(IOTA-81)&127];
			fTempPerm438 = fVec0[(IOTA-28)&127];
			fTempPerm439 = fVec0[(IOTA-27)&127];
			fTempPerm440 = fVec1[(IOTA-91)&127];
			fTempPerm441 = fVec1[(IOTA-108)&127];
			fTempPerm442 = fVec1[(IOTA-101)&127];
			fTempPerm443 = fVec1[(IOTA-74)&127];
			fTempPerm444 = fVec1[(IOTA-81)&127];
			fTempPerm445 = fVec1[(IOTA-72)&127];
			fTempPerm446 = fVec1[(IOTA-73)&127];
			fTempPerm447 = fVec1[(IOTA-38)&127];
			fTempPerm448 = fVec1[(IOTA-30)&127];
			fTempPerm449 = fVec2[(IOTA-123)&127];
			fTempPerm450 = fVec2[(IOTA-115)&127];
			fTempPerm451 = fVec1[(IOTA-31)&127];
			fTempPerm452 = fVec1[(IOTA-37)&127];
			fTempPerm453 = fVec1[(IOTA-34)&127];
			fTempPerm454 = fVec2[(IOTA-69)&127];
			fTempPerm455 = fVec2[(IOTA-68)&127];
			fTempPerm456 = fVec2[(IOTA-67)&127];
			fTempPerm457 = fVec2[(IOTA-66)&127];
			fTempPerm458 = fVec2[(IOTA-62)&127];
			fTempPerm459 = fVec2[(IOTA-61)&127];
			fTempPerm460 = fVec2[(IOTA-57)&127];
			fTempPerm461 = fVec2[(IOTA-39)&127];
			fTempPerm462 = fVec2[(IOTA-56)&127];
			fTempPerm463 = fVec2[(IOTA-58)&127];
			fTempPerm464 = fVec2[(IOTA-38)&127];
			fTempPerm465 = fVec2[(IOTA-34)&127];
			fTempPerm466 = fVec2[(IOTA-33)&127];
			fTempPerm467 = fVec2[(IOTA-37)&127];
			fTempPerm468 = fVec2[(IOTA-32)&127];
			fTempPerm469 = fVec2[(IOTA-26)&127];
			fTempPerm470 = fVec2[(IOTA-23)&127];
			fTempPerm471 = fVec3[(IOTA-97)&127];
			fTempPerm472 = fVec3[(IOTA-88)&127];
			fTempPerm473 = fVec3[(IOTA-81)&127];
			fTempPerm474 = fVec3[(IOTA-47)&127];
			fTempPerm475 = fVec3[(IOTA-24)&127];
			fTempPerm476 = fVec3[(IOTA-2)&127];
			fTempPerm477 = fVec2[(IOTA-76)&127];
			fTempPerm478 = fVec2[(IOTA-75)&127];
			fTempPerm479 = fVec2[(IOTA-74)&127];
			fTempPerm480 = fVec2[(IOTA-70)&127];
			fTempPerm481 = fVec2[(IOTA-59)&127];
			fTempPerm482 = fVec2[(IOTA-60)&127];
			fTempPerm483 = fVec2[(IOTA-45)&127];
			fTempPerm484 = fVec2[(IOTA-48)&127];
			fTempPerm485 = fVec2[(IOTA-47)&127];
			fTempPerm486 = fVec2[(IOTA-42)&127];
			fTempPerm487 = fVec2[(IOTA-41)&127];
			fTempPerm488 = fVec2[(IOTA-40)&127];
			fTempPerm489 = fVec2[(IOTA-36)&127];
			fTempPerm490 = fVec3[(IOTA-89)&127];
			fTempPerm491 = fVec1[(IOTA-71)&127];
			fTempPerm492 = fVec2[(IOTA-118)&127];
			fTempPerm493 = fVec2[(IOTA-44)&127];
			fTempPerm494 = fVec2[(IOTA-43)&127];
			fTempPerm495 = fVec1[(IOTA-113)&127];
			fTempPerm496 = fVec2[(IOTA-46)&127];
			fTempPerm497 = fVec2[(IOTA-25)&127];
			fTempPerm498 = fVec2[(IOTA-24)&127];
			fTempPerm499 = fVec2[(IOTA-31)&127];
			fTempPerm500 = fVec2[(IOTA-124)&127];
			fTempPerm501 = fVec0[(IOTA-7)&127];
			fTempPerm502 = fVec0[(IOTA-9)&127];
			fTempPerm503 = fVec0[(IOTA-11)&127];
			fTempPerm504 = fVec0[(IOTA-12)&127];
			fTempPerm505 = fVec0[(IOTA-14)&127];
			fTempPerm506 = fVec0[(IOTA-31)&127];
			fTempPerm507 = fVec0[(IOTA-82)&127];
			fTempPerm508 = fVec0[(IOTA-92)&127];
			fTempPerm509 = fVec0[(IOTA-98)&127];
			fTempPerm510 = fVec0[(IOTA-108)&127];
			fTempPerm511 = fVec0[(IOTA-113)&127];
			fTempPerm512 = (fRec1[0] * ((((((3.882760000000001e-05 * fTempPerm2) + ((0.000101833 * fTempPerm3) + ((3.9296600000000006e-05 * fTempPerm4) + ((0.00337025 * fTempPerm5) + ((0.00317138 * fTempPerm6) + ((0.000367688 * fTempPerm7) + ((0.0006954530000000001 * fTempPerm8) + ((0.00527862 * fTempPerm9) + ((0.00154417 * fTempPerm10) + ((0.0010312 * fTempPerm11) + ((0.0012247 * fTempPerm12) + ((0.00117636 * fTempPerm13) + ((0.00534772 * fTempPerm14) + ((0.00571347 * fTempPerm15) + ((0.0016634199999999999 * fTempPerm16) + ((0.00359199 * fTempPerm17) + ((0.00503769 * fTempPerm18) + ((1.73416e-05 * fTempPerm19) + ((0.0011593500000000002 * fTempPerm20) + ((0.00341282 * fTempPerm21) + ((0.00681093 * fTempPerm22) + ((0.00600683 * fTempPerm23) + ((0.0174746 * fTempPerm24) + ((0.0211909 * fTempPerm25) + ((0.00253457 * fTempPerm26) + (((((((0.00069471 * fTempPerm27) + ((0.00013874000000000002 * fTempPerm28) + ((0.005367289999999999 * fTempPerm29) + ((0.03066 * fTempPerm30) + ((0.0019489700000000002 * fTempPerm31) + ((0.000702438 * fTempPerm32) + ((2.40821e-06 * fTempPerm35) + ((0.036282299999999996 * fTempPerm36) + ((9.40654e-05 * fTempPerm37) + ((8.464169999999999e-06 * fTempPerm38) + ((8.79674e-06 * fTempPerm39) + ((8.00668e-05 * fTempPerm40) + ((0.0012872900000000002 * fTempPerm41) + ((0.00119739 * fTempPerm42) + ((0.0032713800000000004 * fTempPerm43) + ((0.0008951980000000002 * fTempPerm44) + ((0.000791254 * fTempPerm45) + ((0.000995482 * fTempPerm46) + ((0.000721582 * fTempPerm47) + ((0.00918714 * fTempPerm48) + ((0.00579697 * fTempPerm49) + ((0.0216851 * fTempPerm50) + ((0.00576228 * fTempPerm51) + ((0.0197594 * fTempPerm52) + ((0.0121098 * fTempPerm53) + ((0.0325354 * fTempPerm54) + ((0.000503397 * fTempPerm55) + ((0.000868029 * fTempPerm56) + ((0.00165835 * fTempPerm57) + ((0.00031507000000000003 * fTempPerm58) + ((0.000494618 * fTempPerm59) + ((0.000158404 * fTempPerm60) + ((5.360590000000001e-05 * fTempPerm63) + ((0.0004770400000000001 * fTempPerm64) + ((0.000623519 * fTempPerm65) + ((0.00046969700000000004 * fTempPerm66) + ((0.000705385 * fTempPerm67) + ((0.00108099 * fTempPerm68) + ((0.000813006 * fTempPerm69) + ((2.45331e-06 * fTempPerm70) + ((0.0014036900000000002 * fTempPerm71) + ((0.00146986 * fTempPerm72) + ((0.0007088390000000001 * fTempPerm73) + ((0.00113331 * fTempPerm74) + ((0.00206635 * fTempPerm75) + ((0.00336909 * fTempPerm76) + ((0.00814256 * fTempPerm77) + ((0.00483835 * fTempPerm78) + ((9.081670000000001e-05 * fTempPerm79) + ((7.74527e-05 * fTempPerm80) + ((0.000259004 * fTempPerm81) + ((0.00169945 * fTempPerm82) + ((3.3718100000000005e-05 * fTempPerm83) + ((0.0016691100000000001 * fTempPerm84) + ((0.00243588 * fTempPerm85) + ((0.0056765999999999995 * fTempPerm86) + ((0.00279092 * fTempPerm87) + ((0.0007790860000000001 * fTempPerm88) + ((0.00162146 * fTempPerm89) + ((0.00421731 * fTempPerm90) + ((0.00130772 * fTempPerm91) + ((0.00214275 * fTempPerm92) + ((1.1796500000000001e-05 * fTempPerm93) + ((0.000419816 * fTempPerm94) + ((0.000922271 * fTempPerm95) + ((0.00136595 * fTempPerm96) + ((0.00214314 * fTempPerm97) + ((0.000264467 * fTempPerm98) + ((0.000884337 * fTempPerm99) + ((0.00300031 * fTempPerm100) + ((0.0013671599999999999 * fTempPerm101) + ((0.0010498400000000002 * fTempPerm102) + ((1.70273e-06 * fTempPerm103) + ((8.03417e-06 * fTempPerm104) + ((3.85491e-05 * fTempPerm105) + ((0.00018456300000000003 * fTempPerm106) + ((0.000184012 * fTempPerm107) + ((0.000562856 * fTempPerm108) + ((8.88365e-06 * fTempPerm109) + ((9.942610000000001e-05 * fTempPerm110) + ((5.11494e-05 * fTempPerm111) + ((9.50248e-07 * fTempPerm112) + ((0.000383377 * fTempPerm113) + ((0.0020680200000000003 * fTempPerm114) + ((0.00028545500000000005 * fTempPerm115) + ((0.000324763 * fTempPerm116) + ((0.0022329500000000005 * fTempPerm117) + ((0.0028589500000000003 * fTempPerm118) + ((0.0070971 * fTempPerm119) + ((0.0016079900000000001 * fTempPerm120) + ((0.0016778 * fTempPerm121) + ((0.0005879310000000001 * fTempPerm122) + ((0.0027816299999999998 * fTempPerm123) + ((0.00177653 * fTempPerm124) + ((0.0214629 * fTempPerm125) + ((0.0293457 * fTempPerm126) + ((0.0636584 * fTempPerm127) + ((0.08895479999999999 * fTempPerm128) + ((0.0781434 * fTempPerm129) + ((0.0608862 * fTempPerm130) + ((0.0771319 * fTempPerm131) + ((0.00545812 * fTempPerm132) + ((0.0905515 * fTempPerm133) + ((0.00181104 * fTempPerm134) + ((0.00194177 * fTempPerm135) + ((4.2541300000000006e-05 * fTempPerm138) + ((0.000533584 * fTempPerm139) + ((0.000584702 * fTempPerm140) + ((0.00185348 * fTempPerm141) + ((0.000547366 * fTempPerm142) + ((2.3955000000000004e-05 * fTempPerm143) + ((0.0016799500000000001 * fTempPerm144) + ((0.00054874 * fTempPerm145) + ((0.00272945 * fTempPerm146) + ((0.00157017 * fTempPerm147) + ((0.00274035 * fTempPerm148) + ((0.00214615 * fTempPerm149) + ((0.00637431 * fTempPerm150) + ((0.0042153 * fTempPerm151) + ((0.00623851 * fTempPerm152) + ((0.00500282 * fTempPerm153) + ((0.00693019 * fTempPerm154) + ((0.0056945699999999995 * fTempPerm155) + ((0.00743299 * fTempPerm156) + ((0.011589100000000001 * fTempPerm157) + ((0.0064973 * fTempPerm158) + ((0.00250454 * fTempPerm159) + ((0.00288578 * fTempPerm160) + ((0.00792482 * fTempPerm161) + ((0.015005800000000001 * fTempPerm162) + ((0.00122991 * fTempPerm163) + ((2.09787e-05 * fTempPerm164) + ((0.00050102 * fTempPerm165) + ((0.0003416 * fTempPerm166) + ((0.000676256 * fTempPerm167) + ((0.000981663 * fTempPerm168) + ((0.0008345150000000001 * fTempPerm169) + ((0.0009436510000000001 * fTempPerm170) + ((0.0014059200000000002 * fTempPerm171) + ((0.0012065399999999999 * fTempPerm172) + ((0.0008772760000000001 * fTempPerm173) + ((0.0029129100000000003 * fTempPerm174) + ((0.00308689 * fTempPerm175) + ((0.00342846 * fTempPerm176) + ((0.0023089200000000003 * fTempPerm177) + ((0.0012118500000000002 * fTempPerm178) + ((0.0079812 * fTempPerm179) + ((0.030624600000000002 * fTempPerm180) + ((2.49699e-05 * fTempPerm181) + ((9.93628e-06 * fVec0[IOTA&127]) + ((2.8519800000000004e-05 * fTempPerm182) + ((4.45788e-06 * fVec1[IOTA&127]) + ((0.00015469 * fTempPerm183) + ((3.39551e-05 * fVec2[IOTA&127]) + ((0.00288332 * fTempPerm184) + ((0.0008817050000000001 * fTempPerm185) + ((2.43757e-05 * fTempPerm186) + ((5.80835e-05 * fTempPerm187) + ((0.00229129 * fTempPerm188) + ((0.0043689900000000005 * fTempPerm189) + ((0.0036832600000000003 * fTempPerm190) + ((0.00375801 * fTempPerm191) + ((0.0008932800000000001 * fTempPerm192) + ((0.00301503 * fTempPerm193) + ((0.0053105900000000004 * fTempPerm194) + ((0.0005342939999999999 * fTempPerm195) + ((0.00376608 * fTempPerm196) + ((0.021576300000000003 * fTempPerm197) + ((0.0004323680000000001 * fTempPerm198) + ((0.000709739 * fTempPerm199) + ((0.0004561410000000001 * fTempPerm200) + ((0.0164669 * fTempPerm201) + ((0.008648310000000001 * fTempPerm202) + ((0.0013345100000000001 * fTempPerm203) + ((0.00010376700000000001 * fTempPerm204) + (((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((0.000331327 * fTempPerm205) + (0.00967744 * fTempPerm206)) + (0.058508199999999996 * fTempPerm207)) + (6.798480000000001e-05 * fTempPerm208)) + (0.0019519 * fTempPerm209)) + (0.000168725 * fTempPerm210)) + (0.000258553 * fTempPerm211)) + (0.000164044 * fTempPerm212)) + (0.00114689 * fTempPerm213)) + (0.00306785 * fTempPerm214)) + (0.00325793 * fTempPerm215)) + (0.00361293 * fTempPerm216)) + (0.00312119 * fTempPerm217)) + (0.00305273 * fTempPerm218)) + (0.00124616 * fTempPerm219)) + (0.00225593 * fTempPerm220)) + (0.00422506 * fTempPerm221)) + (0.0035775299999999998 * fTempPerm222)) + (0.00041329900000000006 * fTempPerm223)) + (0.0014647 * fTempPerm224)) + (0.00523563 * fTempPerm225)) + (0.00243653 * fTempPerm226)) + (0.005175760000000001 * fTempPerm227)) + (0.0006128250000000001 * fTempPerm228)) + (0.00136699 * fTempPerm229)) + (0.0012923000000000001 * fTempPerm230)) + (0.0005068340000000001 * fTempPerm231)) + (0.000106546 * fTempPerm232)) + (1.8717900000000003e-05 * fTempPerm233)) + (5.712609999999999e-06 * fTempPerm234)) + (0.0005869030000000001 * fTempPerm235)) + (0.00046795 * fTempPerm236)) + (0.00045975200000000007 * fTempPerm237)) + (2.3648600000000003e-05 * fTempPerm238)) + (2.5118200000000003e-05 * fVec3[IOTA&127])) + (0.000123708 * fTempPerm239)) + (0.0009683840000000001 * fTempPerm240)) + (0.00059949 * fTempPerm241)) + (0.000767728 * fTempPerm242)) + (0.000141179 * fTempPerm243)) + (0.0018634200000000002 * fTempPerm244)) + (0.00024957099999999997 * fTempPerm245)) + (0.00115892 * fTempPerm246)) + (0.000508795 * fTempPerm247)) + (0.0036302599999999997 * fTempPerm248)) + (0.0565104 * fTempPerm249)) + (0.049342300000000006 * fTempPerm250)) + (0.0449074 * fTempPerm251)) + (0.0647491 * fTempPerm252)) + (0.05960650000000001 * fTempPerm253)) + (0.07349510000000001 * fTempPerm254)) + (0.034514800000000005 * fTempPerm255)) + (0.0557725 * fTempPerm256)) + (0.06596840000000001 * fTempPerm257)) + (0.057809200000000005 * fTempPerm258)) + (0.09689510000000001 * fTempPerm259)) + (0.0212609 * fTempPerm260)) + (0.0317463 * fTempPerm261)) + (0.0528391 * fTempPerm262)) + (0.00870641 * fTempPerm263)) + (0.0266261 * fTempPerm264)) + (0.0008637700000000001 * fTempPerm265)) + (0.011366300000000001 * fTempPerm266)) + (0.00570825 * fTempPerm267)) + (0.023001999999999998 * fTempPerm268)) + (0.0200236 * fTempPerm269)) + (0.00446807 * fTempPerm270)) + (0.0141227 * fTempPerm271)) + (0.00175234 * fTempPerm272)) + (0.00366525 * fTempPerm273)) + (0.0028834 * fTempPerm274)) + (0.006804070000000001 * fTempPerm275)) + (0.00185049 * fTempPerm276)) + (0.00321075 * fTempPerm277)) + (0.004194580000000001 * fTempPerm278)) + (0.000828477 * fTempPerm279)) + (0.0024405 * fTempPerm280)) + (0.0041905200000000005 * fTempPerm281)) + (0.000563692 * fTempPerm282)) + (0.0037083100000000003 * fTempPerm283)) + (0.000453824 * fTempPerm284)) + (0.00154024 * fTempPerm285)) + (5.0813899999999995e-06 * fTempPerm286)) + (2.06253e-06 * fTempPerm287)) + (0.0413412 * fTempPerm288)) + (0.050635700000000006 * fTempPerm289)) + (0.0016152100000000002 * fTempPerm290))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.000534434 * fTempPerm291)) + (0.00148817 * fTempPerm292)) + (0.00201002 * fTempPerm293)) + (0.00618924 * fTempPerm294)) + (0.0025992000000000003 * fTempPerm295))))))))))))))))))))))))))) + (0.000682534 * fTempPerm296)) + (0.00195891 * fTempPerm297)) + (0.000105639 * fTempPerm298)) - (((((((9.84852e-06 * fTempPerm299) + ((0.00029024100000000006 * fTempPerm300) + ((0.000541095 * fTempPerm301) + ((0.00020805 * fTempPerm302) + ((0.0010388900000000002 * fTempPerm303) + ((0.00142103 * fTempPerm304) + ((0.00124353 * fTempPerm305) + ((0.00103026 * fTempPerm306) + ((4.5241600000000004e-05 * fTempPerm307) + ((0.0008303280000000001 * fTempPerm308) + ((0.00103439 * fTempPerm309) + ((0.00193403 * fTempPerm310) + ((0.00149053 * fTempPerm311) + ((0.000498921 * fTempPerm312) + ((0.00299686 * fTempPerm313) + ((0.000502175 * fTempPerm314) + ((0.00248879 * fTempPerm315) + ((0.0100647 * fTempPerm316) + ((((((((0.000362084 * fTempPerm317) + ((0.000177736 * fTempPerm318) + ((0.0301367 * fTempPerm319) + ((0.0006988710000000001 * fTempPerm320) + ((0.0120184 * fTempPerm321) + ((0.011691100000000001 * fTempPerm322) + ((0.025458500000000002 * fTempPerm323) + ((0.000383553 * fTempPerm324) + ((0.00026040700000000004 * fTempPerm325) + ((0.0012643700000000001 * fTempPerm326) + ((0.0009646670000000001 * fTempPerm327) + ((0.0014951399999999998 * fTempPerm328) + ((0.0006419069999999999 * fTempPerm329) + ((0.00465625 * fTempPerm330) + ((0.00123749 * fTempPerm331) + ((0.00350462 * fTempPerm332) + ((0.0015381499999999998 * fTempPerm333) + ((0.00113341 * fTempPerm334) + ((0.00260887 * fTempPerm335) + ((0.00029472900000000003 * fTempPerm336) + ((0.00436148 * fTempPerm337) + ((0.0115485 * fTempPerm338) + ((0.0032127699999999998 * fTempPerm339) + ((0.005780400000000001 * fTempPerm340) + ((0.037111700000000004 * fTempPerm341) + ((0.014907300000000002 * fTempPerm342) + ((0.0092683 * fTempPerm343) + ((0.0158698 * fTempPerm344) + ((0.00489909 * fTempPerm345) + ((0.000362721 * fTempPerm346) + ((0.00043871700000000003 * fTempPerm347) + ((0.000181327 * fTempPerm348) + ((0.0009705890000000001 * fTempPerm349) + ((0.000160361 * fTempPerm350) + ((0.00019245000000000002 * fTempPerm351) + ((0.000269357 * fTempPerm352) + ((6.52305e-05 * fTempPerm353) + ((0.000138672 * fTempPerm354) + ((0.00117495 * fTempPerm355) + ((6.971659999999999e-06 * fTempPerm356) + ((0.000103579 * fTempPerm357) + ((0.000654569 * fTempPerm358) + ((0.0005181680000000001 * fTempPerm359) + ((0.000711991 * fTempPerm360) + ((0.00124685 * fTempPerm361) + ((0.0019008400000000002 * fTempPerm362) + ((0.00765874 * fTempPerm363) + ((0.0030320900000000003 * fTempPerm364) + ((0.000550486 * fTempPerm365) + ((7.589380000000002e-05 * fTempPerm366) + ((6.574880000000001e-05 * fTempPerm367) + ((0.0005423040000000001 * fTempPerm368) + ((0.00038862600000000004 * fTempPerm369) + ((0.000622009 * fTempPerm370) + ((0.00155276 * fTempPerm371) + ((0.00147963 * fTempPerm372) + ((4.109880000000001e-05 * fTempPerm373) + ((0.00104971 * fTempPerm374) + ((0.00271694 * fTempPerm375) + ((0.00177024 * fTempPerm376) + ((0.00161133 * fTempPerm377) + ((1.8656099999999998e-06 * fTempPerm378) + ((1.1129000000000001e-05 * fTempPerm379) + ((9.139900000000001e-05 * fTempPerm380) + ((0.000272806 * fTempPerm381) + ((0.000314307 * fTempPerm382) + ((0.00035653000000000003 * fTempPerm383) + ((0.000273034 * fTempPerm384) + ((8.74662e-06 * fTempPerm385) + ((0.00025063300000000005 * fTempPerm386) + ((0.00246014 * fTempPerm387) + ((0.00766095 * fTempPerm388) + ((0.00958195 * fTempPerm389) + ((0.00243748 * fTempPerm390) + ((0.0017056100000000002 * fTempPerm391) + ((0.00687643 * fTempPerm392) + ((0.015913200000000002 * fTempPerm393) + ((0.015119400000000002 * fTempPerm394) + ((0.0128559 * fTempPerm395) + ((9.829720000000001e-05 * fTempPerm396) + ((0.0005638910000000001 * fTempPerm397) + ((0.0008882540000000001 * fTempPerm398) + ((0.00299123 * fTempPerm399) + ((0.000323087 * fTempPerm400) + ((0.00126004 * fTempPerm401) + ((0.0016313200000000001 * fTempPerm402) + ((0.00198432 * fTempPerm403) + ((0.00312195 * fTempPerm404) + ((0.0008297610000000001 * fTempPerm405) + ((0.0015286800000000001 * fTempPerm406) + ((0.00121188 * fTempPerm407) + ((0.00146794 * fTempPerm408) + ((0.0017147500000000001 * fTempPerm409) + ((0.0010789999999999999 * fTempPerm410) + ((0.00121329 * fTempPerm411) + ((0.000735849 * fTempPerm412) + ((0.00336985 * fTempPerm413) + ((0.0043256300000000004 * fTempPerm414) + ((2.56393e-06 * fTempPerm415) + ((0.000137362 * fTempPerm416) + ((0.000608302 * fTempPerm417) + ((4.81323e-05 * fTempPerm418) + ((3.1175000000000006e-05 * fTempPerm419) + ((0.0100582 * fTempPerm420) + ((0.0180915 * fTempPerm421) + ((0.0390074 * fTempPerm422) + ((0.0125257 * fTempPerm423) + ((0.039406500000000004 * fTempPerm424) + ((0.0160175 * fTempPerm425) + ((0.0011779100000000001 * fTempPerm426) + ((0.000167329 * fTempPerm427) + ((0.00041459900000000003 * fTempPerm428) + ((0.000311911 * fTempPerm429) + ((0.000315062 * fTempPerm430) + ((0.0024969700000000003 * fTempPerm431) + ((0.0014838199999999998 * fTempPerm432) + ((0.0114164 * fTempPerm433) + ((0.000446193 * fTempPerm434) + ((0.0013548899999999999 * fTempPerm435) + ((0.000255876 * fTempPerm436) + ((0.0030592 * fTempPerm437) + ((0.0391701 * fTempPerm438) + ((0.00031563800000000003 * fTempPerm439) + ((5.444779999999999e-06 * fTempPerm440) + ((0.00127535 * fTempPerm441) + ((0.0013151 * fTempPerm442) + ((0.000315111 * fTempPerm443) + ((0.000737536 * fTempPerm444) + ((0.00239256 * fTempPerm445) + ((0.00113852 * fTempPerm446) + ((0.00392637 * fTempPerm447) + ((0.0200181 * fTempPerm448) + ((3.8802e-05 * fTempPerm449) + ((0.00012562199999999998 * fTempPerm450) + ((0.0161655 * fTempPerm451) + ((0.0193136 * fTempPerm452) + ((0.0137585 * fTempPerm453) + ((0.0023879 * fTempPerm454) + ((0.0030104600000000004 * fTempPerm455) + ((0.00157916 * fTempPerm456) + ((0.0021747000000000003 * fTempPerm457) + ((0.00444703 * fTempPerm458) + ((0.0047484400000000005 * fTempPerm459) + ((0.00454199 * fTempPerm460) + ((0.015368999999999999 * fTempPerm461) + ((0.0104416 * fTempPerm462) + ((0.0037710400000000002 * fTempPerm463) + ((0.025263300000000002 * fTempPerm464) + ((0.026437 * fTempPerm465) + ((0.0357626 * fTempPerm466) + ((0.026966700000000003 * fTempPerm467) + ((0.0193807 * fTempPerm468) + ((0.0095142 * fTempPerm469) + ((0.0346875 * fTempPerm470) + ((0.000544865 * fTempPerm471) + ((0.00141242 * fTempPerm472) + ((0.00106944 * fTempPerm473) + ((0.00296739 * fTempPerm474) + ((0.015415000000000002 * fTempPerm475) + ((0.00017378 * fTempPerm476) + ((0.000929744 * fTempPerm477) + ((0.00189912 * fTempPerm478) + ((0.00184981 * fTempPerm479) + ((0.00102903 * fTempPerm480) + ((0.00475476 * fTempPerm481) + ((0.0027951100000000004 * fTempPerm482) + ((0.008875280000000001 * fTempPerm483) + ((0.0166062 * fTempPerm484) + ((0.016194200000000002 * fTempPerm485) + ((0.0221845 * fTempPerm486) + ((0.0203737 * fTempPerm487) + ((0.00862273 * fTempPerm488) + ((0.023967600000000002 * fTempPerm489) + ((0.00134669 * fTempPerm490) + ((0.00191902 * fTempPerm491) + ((0.000163854 * fTempPerm492) + ((0.016079200000000002 * fTempPerm493) + ((0.0188738 * fTempPerm494) + ((0.0005503540000000001 * fTempPerm495) + ((0.0116756 * fTempPerm496) + ((0.0006032850000000001 * fTempPerm497) + ((0.04279480000000001 * fTempPerm498) + ((0.00521665 * fTempPerm499) + (8.318509999999999e-06 * fTempPerm500)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0011081700000000001 * fTempPerm501)) + (0.00131485 * fTempPerm502)) + (0.0030320200000000003 * fTempPerm503)) + (0.00721718 * fTempPerm504)) + (0.00530958 * fTempPerm505)) + (0.0173412 * fTempPerm506)))))))))))))))))))) + (0.00200404 * fTempPerm507)) + (0.00201638 * fTempPerm508)) + (0.00170268 * fTempPerm509)) + (0.000839627 * fTempPerm510)) + (0.000428973 * fTempPerm511))));
			fRec0[0] = max((fRec0[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm512))))));
			fbargraph4 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTempPerm512;
			fTempPerm513 = (fRec1[0] * ((((0.00373392 * fTempPerm297) + ((0.000659975 * fTempPerm508) + (((5.530960000000001e-05 * fTempPerm3) + ((4.2207000000000004e-05 * fTempPerm299) + ((((0.00198173 * fTempPerm304) + ((0.00406262 * fTempPerm5) + (((0.00109229 * fTempPerm7) + ((0.0023261500000000004 * fTempPerm9) + ((0.00150447 * fTempPerm10) + ((0.000178551 * fTempPerm12) + ((0.00207781 * fTempPerm13) + ((0.00201686 * fTempPerm14) + ((0.004869989999999999 * fTempPerm312) + ((0.0011997199999999998 * fTempPerm313) + (((0.00119525 * fTempPerm16) + ((((0.00127378 * fTempPerm19) + ((0.00615266 * fTempPerm315) + ((0.0115422 * fTempPerm506) + ((((((0.000654334 * fTempPerm293) + ((0.000286004 * fTempPerm292) + ((0.00012754700000000002 * fTempPerm502) + ((0.000569647 * fTempPerm291) + ((0.000558749 * fTempPerm27) + ((0.000107706 * fTempPerm28) + ((0.0125606 * fTempPerm29) + ((0.0537481 * fTempPerm30) + ((0.0130594 * fTempPerm31) + (((0.0243674 * fTempPerm36) + ((0.021176 * fTempPerm37) + (((1.9873400000000002e-05 * fTempPerm39) + ((9.322950000000002e-05 * fTempPerm40) + ((0.00251957 * fTempPerm43) + ((7.665409999999998e-06 * fTempPerm334) + ((0.00118894 * fTempPerm47) + (((0.0030812 * fTempPerm48) + ((((((((((7.080070000000001e-05 * fTempPerm348) + (((0.000231146 * fTempPerm58) + ((0.000147661 * fTempPerm59) + ((0.00022708 * fTempPerm352) + ((7.19018e-05 * fTempPerm353) + ((((0.00390744 * fTempPerm72) + (((0.00121048 * fTempPerm74) + ((0.00040873600000000004 * fTempPerm365) + ((0.00111368 * fTempPerm75) + ((((1.9102600000000002e-05 * fTempPerm366) + ((0.00842323 * fTempPerm78) + ((9.57012e-05 * fTempPerm79) + ((5.5900900000000005e-05 * fTempPerm367) + ((0.00174807 * fTempPerm81) + ((0.00297761 * fTempPerm82) + ((0.00013990400000000002 * fTempPerm373) + (((0.0021519 * fTempPerm84) + (((0.0032282 * fTempPerm86) + ((0.00415929 * fTempPerm89) + ((0.00756406 * fTempPerm377) + ((0.00796032 * fTempPerm90) + ((0.000184975 * fTempPerm91) + ((1.48337e-06 * fTempPerm378) + ((3.39827e-05 * fTempPerm379) + ((1.5261800000000004e-05 * fTempPerm93) + (((0.000368308 * fTempPerm381) + ((0.000251322 * fTempPerm382) + ((0.000239084 * fTempPerm384) + ((9.12388e-06 * fTempPerm385) + ((0.00021972700000000002 * fTempPerm98) + ((0.00030627700000000004 * fTempPerm386) + ((0.0030369999999999998 * fTempPerm387) + ((0.005800029999999999 * fTempPerm388) + ((0.00790244 * fTempPerm389) + (((0.00227327 * fTempPerm391) + ((0.00468815 * fTempPerm392) + ((0.0143637 * fTempPerm393) + ((0.0188991 * fTempPerm394) + ((0.0166992 * fTempPerm395) + (((8.76003e-06 * fTempPerm104) + (((0.00011697900000000002 * fTempPerm106) + ((0.000115798 * fTempPerm396) + ((0.0016790700000000002 * fTempPerm108) + ((1.8977800000000003e-05 * fTempPerm109) + ((0.000122408 * fTempPerm110) + (((0.000827394 * fTempPerm114) + (((((3.69732e-06 * fTempPerm415) + ((0.000158936 * fTempPerm416) + ((0.00145085 * fTempPerm120) + ((((0.00740671 * fTempPerm420) + ((0.0186217 * fTempPerm421) + ((0.033071500000000004 * fTempPerm422) + ((0.024658199999999998 * fTempPerm423) + ((0.023601800000000003 * fTempPerm424) + ((((0.000426597 * fTempPerm139) + ((0.00184199 * fTempPerm141) + ((0.000508036 * fTempPerm142) + ((0.00175057 * fTempPerm144) + (((0.0033512100000000003 * fTempPerm146) + (((2.5156400000000002e-05 * fTempPerm147) + ((0.00293818 * fTempPerm148) + ((0.00538217 * fTempPerm150) + ((0.0038801100000000004 * fTempPerm151) + ((0.0067297599999999996 * fTempPerm152) + ((((0.0140384 * fTempPerm156) + ((((0.00846299 * fTempPerm159) + ((0.0105579 * fTempPerm160) + ((((7.39806e-07 * fTempPerm164) + ((0.00050422 * fTempPerm165) + ((0.00038352400000000005 * fTempPerm166) + ((0.00103368 * fTempPerm167) + ((0.0007092450000000001 * fTempPerm168) + ((0.000779071 * fTempPerm169) + ((0.000959789 * fTempPerm170) + ((0.00100501 * fTempPerm171) + ((0.0012236 * fTempPerm172) + (((0.0036032700000000004 * fTempPerm174) + ((0.00530227 * fTempPerm175) + ((0.0028127900000000003 * fTempPerm176) + ((0.0016933400000000002 * fTempPerm177) + ((4.49754e-06 * fTempPerm178) + (((0.0123141 * fTempPerm433) + ((0.0075229400000000005 * fTempPerm180) + ((2.3258800000000005e-05 * fTempPerm181) + ((1.5002200000000002e-05 * fVec0[IOTA&127]) + ((0.00354482 * fTempPerm184) + ((0.00221504 * fTempPerm185) + ((3.4954000000000004e-05 * fTempPerm186) + ((0.000876507 * fTempPerm440) + ((0.0013280499999999999 * fTempPerm189) + ((0.00173031 * fTempPerm191) + ((0.00102092 * fTempPerm443) + ((0.000386788 * fTempPerm444) + ((0.00108066 * fTempPerm194) + ((0.00884039 * fTempPerm195) + ((0.00403937 * fTempPerm196) + ((0.033864 * fTempPerm197) + ((4.14705e-05 * fTempPerm449) + ((2.6751600000000003e-05 * fTempPerm450) + ((0.0054466 * fTempPerm201) + ((0.00015571700000000001 * fTempPerm204) + ((0.0039002500000000005 * fTempPerm454) + ((0.000542053 * fTempPerm455) + ((0.00164631 * fTempPerm456) + ((0.00128867 * fTempPerm457) + ((0.00232849 * fTempPerm458) + ((0.0020450900000000003 * fTempPerm459) + ((0.0110121 * fTempPerm460) + ((0.016965 * fTempPerm461) + ((0.0109805 * fTempPerm462) + ((0.0057548899999999995 * fTempPerm463) + ((0.017693 * fTempPerm464) + ((0.0232959 * fTempPerm465) + ((0.030269900000000002 * fTempPerm466) + ((0.0333365 * fTempPerm467) + ((0.023498 * fTempPerm468) + ((0.0142102 * fTempPerm469) + ((1.74779e-06 * fTempPerm287) + ((7.13748e-06 * fTempPerm286) + ((0.00147426 * fTempPerm285) + ((0.000502614 * fTempPerm284) + ((0.00193882 * fTempPerm472) + ((0.00365475 * fTempPerm283) + ((0.00144247 * fTempPerm281) + ((0.00204096 * fTempPerm280) + ((0.00201999 * fTempPerm279) + ((0.0015368900000000002 * fTempPerm473) + ((0.00463264 * fTempPerm278) + ((0.00198287 * fTempPerm277) + ((0.00072404 * fTempPerm276) + ((0.00648151 * fTempPerm275) + ((0.00139635 * fTempPerm274) + ((0.000209301 * fTempPerm273) + ((0.00464837 * fTempPerm272) + ((0.0007157 * fTempPerm474) + ((0.0105391 * fTempPerm271) + ((0.00565642 * fTempPerm270) + ((0.0272114 * fTempPerm269) + ((0.021074000000000002 * fTempPerm268) + ((0.008019420000000001 * fTempPerm266) + ((0.00143566 * fTempPerm265) + ((0.017730600000000003 * fTempPerm264) + ((0.0216826 * fTempPerm263) + ((0.0424237 * fTempPerm262) + ((0.0368012 * fTempPerm261) + ((0.05446430000000001 * fTempPerm260) + ((0.111697 * fTempPerm259) + ((0.0503806 * fTempPerm258) + ((0.0697323 * fTempPerm257) + ((0.0414059 * fTempPerm256) + ((0.047424299999999996 * fTempPerm255) + ((0.0813459 * fTempPerm254) + ((0.043948600000000004 * fTempPerm253) + ((0.0622112 * fTempPerm252) + ((0.0466987 * fTempPerm251) + ((0.0530012 * fTempPerm250) + ((0.0489414 * fTempPerm249) + ((0.0007391530000000001 * fTempPerm248) + ((0.0020685400000000002 * fTempPerm247) + ((0.000557527 * fTempPerm246) + ((0.00107368 * fTempPerm245) + ((0.00105561 * fTempPerm244) + ((0.000646817 * fTempPerm243) + ((0.000782518 * fTempPerm242) + ((0.000840054 * fTempPerm241) + ((0.0007244580000000001 * fTempPerm240) + ((0.00012713900000000002 * fTempPerm239) + ((1.96889e-05 * fVec3[IOTA&127]) + ((0.0017905 * fTempPerm477) + ((0.00445499 * fTempPerm480) + ((0.0051357799999999995 * fTempPerm481) + ((0.00389261 * fTempPerm482) + ((0.017027300000000002 * fTempPerm483) + ((0.0121522 * fTempPerm484) + ((0.011685700000000002 * fTempPerm485) + ((0.0175694 * fTempPerm486) + ((0.0160724 * fTempPerm487) + ((0.0227996 * fTempPerm488) + ((0.029073099999999998 * fTempPerm489) + ((1.69708e-05 * fTempPerm234) + ((0.000117308 * fTempPerm232) + ((0.0009450379999999999 * fTempPerm231) + ((0.00130975 * fTempPerm230) + ((0.0016259900000000001 * fTempPerm229) + ((0.00059498 * fTempPerm228) + ((0.0026745600000000003 * fTempPerm227) + ((0.00266924 * fTempPerm226) + ((0.00390875 * fTempPerm225) + ((0.00338193 * fTempPerm222) + ((0.0026122100000000002 * fTempPerm221) + ((0.00337264 * fTempPerm220) + ((0.0039974 * fTempPerm219) + ((0.0042595 * fTempPerm218) + ((0.00380208 * fTempPerm217) + ((0.00278079 * fTempPerm216) + ((0.00292299 * fTempPerm215) + ((0.000671666 * fTempPerm214) + ((0.000165502 * fTempPerm492) + ((0.019806900000000002 * fTempPerm493) + ((0.0173035 * fTempPerm494) + ((0.000195057 * fTempPerm212) + ((0.000161141 * fTempPerm211) + ((0.0122635 * fTempPerm496) + ((0.000202164 * fTempPerm210) + ((0.0020406 * fTempPerm209) + ((((((0.012867100000000001 * fTempPerm499) + (0.00116433 * fTempPerm205)) + (8.505850000000001e-06 * fTempPerm500)) + (0.056895100000000004 * fTempPerm498)) + (0.010567 * fTempPerm497)) + (0.000154297 * fTempPerm208)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.008159850000000001 * fTempPerm179))))))) + (0.0013272 * fTempPerm173))))))))))) + (0.000275169 * fTempPerm430)) + (0.00431154 * fTempPerm162)))) + (0.0014525200000000001 * fTempPerm158)) + (0.00729384 * fTempPerm157))) + (0.004986290000000001 * fTempPerm155)) + (0.006066569999999999 * fTempPerm154))))))) + (0.00105509 * fTempPerm429))) + (0.0025739 * fTempPerm145)))))) + (5.5996800000000006e-05 * fTempPerm138)) + (0.0254229 * fTempPerm425))))))) + (0.0006599420000000001 * fTempPerm124)) + (3.71676e-05 * fTempPerm418))))) + (0.00817955 * fTempPerm119)) + (0.00669066 * fTempPerm118)) + (0.000330976 * fTempPerm414))) + (0.0009061030000000001 * fTempPerm113))))))) + (3.2490500000000003e-05 * fTempPerm105))) + (3.31876e-06 * fTempPerm103))))))) + (0.000377902 * fTempPerm390))))))))))) + (0.00011888700000000002 * fTempPerm380)))))))))) + (0.00425301 * fTempPerm85))) + (0.00235812 * fTempPerm83))))))))) + (0.00222904 * fTempPerm77)) + (0.0110775 * fTempPerm76))))) + (0.00102589 * fTempPerm73))) + (1.41881e-06 * fTempPerm70)) + (5.14829e-06 * fTempPerm356)))))) + (0.0007695660000000001 * fTempPerm57))) + (0.0258155 * fTempPerm54)) + (0.0012059800000000002 * fTempPerm53)) + (0.022299000000000003 * fTempPerm52)) + (0.0092729 * fTempPerm340)) + (0.00438648 * fTempPerm51)) + (0.0027836500000000004 * fTempPerm339)) + (0.009235 * fTempPerm50)) + (0.000537327 * fTempPerm49))) + (0.00227417 * fTempPerm336))))))) + (1.0045e-05 * fTempPerm38)))) + (1.8649e-06 * fTempPerm35))))))))))) + (0.0033821 * fTempPerm294)) + (0.0131959 * fTempPerm295)) + (0.015611 * fTempPerm25)) + (0.0292041 * fTempPerm24))))) + (0.0028706900000000004 * fTempPerm18)) + (3.8517100000000004e-05 * fTempPerm17))) + (0.000701284 * fTempPerm314)))))))))) + (0.00275175 * fTempPerm6)))) + (9.867810000000001e-05 * fTempPerm4)) + (5.6691300000000004e-05 * fTempPerm300)))) + (5.56606e-06 * fTempPerm2)))) + (0.00011918500000000002 * fTempPerm298)) - ((0.000280439 * fTempPerm511) + ((0.0008063269999999999 * fTempPerm510) + ((0.00277688 * fTempPerm509) + ((0.00145302 * fTempPerm296) + ((0.00273074 * fTempPerm507) + (((0.000528145 * fTempPerm302) + ((0.00130731 * fTempPerm303) + ((0.0005989060000000001 * fTempPerm305) + ((0.00170804 * fTempPerm306) + ((((0.00110995 * fTempPerm308) + ((0.00044511 * fTempPerm11) + (((0.00110707 * fTempPerm310) + ((0.0016638 * fTempPerm311) + ((0.004373940000000001 * fTempPerm15) + (((0.0073186 * fTempPerm21) + ((0.00139329 * fTempPerm22) + ((0.004597380000000001 * fTempPerm316) + ((0.013183400000000001 * fTempPerm23) + ((((((0.000505773 * fTempPerm501) + ((0.00019702400000000002 * fTempPerm317) + ((6.42903e-05 * fTempPerm318) + ((0.0286455 * fTempPerm319) + ((0.0060285 * fTempPerm320) + ((0.0215706 * fTempPerm321) + ((0.0014793400000000002 * fTempPerm32) + ((0.00439889 * fTempPerm322) + ((0.0575248 * fTempPerm323) + ((0.0007250860000000001 * fTempPerm324) + (((0.000828266 * fTempPerm326) + ((0.00186384 * fTempPerm327) + ((0.000384737 * fTempPerm41) + ((0.00136555 * fTempPerm328) + ((0.00110553 * fTempPerm329) + ((0.0011346700000000002 * fTempPerm42) + ((0.0031654200000000004 * fTempPerm330) + ((0.000680485 * fTempPerm44) + ((0.00122218 * fTempPerm45) + ((0.00220639 * fTempPerm331) + ((0.00152893 * fTempPerm46) + ((0.000783797 * fTempPerm332) + ((0.0016457800000000001 * fTempPerm333) + ((0.00275033 * fTempPerm335) + ((((((((((((0.00095017 * fTempPerm347) + ((0.0010622000000000001 * fTempPerm349) + ((0.000249192 * fTempPerm350) + ((0.000270496 * fTempPerm351) + ((5.1123399999999996e-06 * fTempPerm60) + ((0.00041578400000000004 * fTempPerm63) + ((0.00042180200000000004 * fTempPerm64) + (((0.00040693400000000007 * fTempPerm66) + ((0.0009659900000000001 * fTempPerm67) + ((0.0012716300000000002 * fTempPerm68) + ((0.00130908 * fTempPerm354) + ((0.00065036 * fTempPerm69) + (((((0.0008129400000000001 * fTempPerm359) + ((0.00091664 * fTempPerm360) + (((0.001377 * fTempPerm71) + ((0.00100318 * fTempPerm362) + ((0.00434699 * fTempPerm363) + (((5.185120000000001e-05 * fTempPerm80) + ((0.00048678400000000003 * fTempPerm368) + ((0.000665779 * fTempPerm369) + ((0.00167489 * fTempPerm370) + ((0.00120272 * fTempPerm371) + ((0.00103053 * fTempPerm372) + ((0.00046550599999999997 * fTempPerm374) + ((0.00022987799999999998 * fTempPerm87) + ((0.0013408299999999999 * fTempPerm375) + (((0.00259829 * fTempPerm376) + ((0.0012261 * fTempPerm92) + ((0.000154768 * fTempPerm94) + ((2.3801800000000003e-05 * fTempPerm383) + ((0.000737778 * fTempPerm95) + ((0.00139564 * fTempPerm96) + ((0.00205962 * fTempPerm97) + (((0.000615973 * fTempPerm100) + ((0.000752906 * fTempPerm101) + ((0.0021273399999999997 * fTempPerm102) + ((7.432920000000001e-05 * fTempPerm107) + ((0.000643591 * fTempPerm397) + ((0.00023107600000000004 * fTempPerm398) + ((0.00129805 * fTempPerm399) + ((0.0022987899999999998 * fTempPerm400) + ((0.000409495 * fTempPerm401) + ((0.00103864 * fTempPerm402) + ((0.00137873 * fTempPerm403) + ((0.00181733 * fTempPerm404) + ((0.0011399600000000002 * fTempPerm405) + ((0.00059075 * fTempPerm406) + ((0.0019049100000000001 * fTempPerm407) + ((0.00124602 * fTempPerm408) + ((0.00293582 * fTempPerm409) + ((0.00040302800000000005 * fTempPerm111) + (((0.0009477180000000001 * fTempPerm410) + ((0.0015273399999999999 * fTempPerm411) + ((0.0010960700000000002 * fTempPerm115) + (((0.0018260300000000002 * fTempPerm412) + ((0.00238017 * fTempPerm413) + ((((2.50192e-06 * fTempPerm419) + (((0.00247991 * fTempPerm122) + ((0.00256507 * fTempPerm123) + ((0.00881751 * fTempPerm125) + ((0.0531961 * fTempPerm126) + (((0.0977876 * fTempPerm128) + (((0.064767 * fTempPerm130) + ((0.0829356 * fTempPerm131) + (((0.0784899 * fTempPerm133) + ((0.00072128 * fTempPerm134) + ((0.00149621 * fTempPerm426) + (((((0.00048385 * fTempPerm140) + ((0.0023706 * fTempPerm143) + ((9.940880000000001e-05 * fTempPerm149) + ((0.0006331710000000001 * fTempPerm153) + ((0.0009207870000000001 * fTempPerm161) + (((0.00196204 * fTempPerm431) + ((1.7742e-05 * fTempPerm432) + ((3.758e-05 * fTempPerm182) + ((1.0426200000000002e-05 * fVec1[IOTA&127]) + ((((((((0.0391217 * fTempPerm438) + ((0.00144425 * fTempPerm439) + (((0.0010250300000000001 * fTempPerm188) + ((((0.0013616 * fTempPerm442) + (((0.00306353 * fTempPerm445) + ((0.0013700399999999999 * fTempPerm446) + (((0.0114674 * fTempPerm447) + (((0.000649337 * fTempPerm198) + (((0.00077856 * fTempPerm200) + (((0.00786783 * fTempPerm452) + (((0.00167116 * fTempPerm203) + (((0.00133631 * fTempPerm453) + ((0.000929206 * fTempPerm470) + ((0.0413938 * fTempPerm289) + ((0.042661300000000006 * fTempPerm288) + ((0.000473651 * fTempPerm471) + (((0.00293953 * fTempPerm267) + ((0.0212956 * fTempPerm475) + ((0.000160754 * fTempPerm476) + ((0.0017438199999999999 * fTempPerm238) + ((0.00161012 * fTempPerm237) + ((0.000597807 * fTempPerm236) + ((0.0011553 * fTempPerm478) + ((0.000984567 * fTempPerm479) + (((((0.00036222200000000004 * fTempPerm224) + (((0.00196376 * fTempPerm491) + (((0.000263489 * fTempPerm495) + ((0.0801361 * fTempPerm207) + (0.00924877 * fTempPerm206))) + (0.0010723199999999999 * fTempPerm213))) + (0.000611186 * fTempPerm223))) + (0.0008751970000000001 * fTempPerm490)) + (1.47262e-06 * fTempPerm233)) + (0.000849503 * fTempPerm235)))))))))) + (0.000687957 * fTempPerm282))))))) + (0.00188932 * fTempPerm290))) + (0.00898901 * fTempPerm202))) + (0.0143689 * fTempPerm451))) + (0.0010572399999999999 * fTempPerm199))) + (0.00493341 * fTempPerm448))) + (0.0038962 * fTempPerm193)))) + (0.000216738 * fTempPerm192))) + (0.000855899 * fTempPerm441)) + (0.00043984100000000006 * fTempPerm190))) + (7.89041e-07 * fTempPerm187)))) + (0.0028667500000000004 * fTempPerm437)) + (0.00383124 * fTempPerm436)) + (0.0022861500000000002 * fTempPerm435)) + (0.0006406130000000001 * fTempPerm434)) + (2.4709800000000002e-05 * fVec2[IOTA&127])) + (0.00015027 * fTempPerm183)))))) + (0.000851532 * fTempPerm163))))))) + (0.000429699 * fTempPerm428)) + (0.000453602 * fTempPerm135)) + (0.00101019 * fTempPerm427))))) + (0.0006735780000000001 * fTempPerm132)))) + (0.0772965 * fTempPerm129))) + (0.040092800000000005 * fTempPerm127)))))) + (0.0017169000000000002 * fTempPerm121))) + (2.74177e-05 * fTempPerm417)) + (0.00263441 * fTempPerm117)))) + (0.0012814900000000001 * fTempPerm116))))) + (3.48346e-05 * fTempPerm112)))))))))))))))))))) + (0.00144013 * fTempPerm99))))))))) + (0.000785113 * fTempPerm88))))))))))) + (0.0106382 * fTempPerm364))))) + (0.000739556 * fTempPerm361)))) + (0.000829505 * fTempPerm358)) + (0.000293776 * fTempPerm357)) + (0.00011313 * fTempPerm355))))))) + (0.00069698 * fTempPerm65))))))))) + (0.00021246900000000004 * fTempPerm346)) + (0.00564598 * fTempPerm345)) + (0.000976993 * fTempPerm56)) + (0.00184861 * fTempPerm55)) + (0.0111431 * fTempPerm344)) + (0.0141169 * fTempPerm343)) + (0.00249322 * fTempPerm342)) + (0.0433556 * fTempPerm341)) + (0.008639049999999999 * fTempPerm338)) + (0.019686699999999998 * fTempPerm337)))))))))))))))) + (0.000979588 * fTempPerm325)))))))))))) + (0.000216524 * fTempPerm503)) + (0.00373067 * fTempPerm504)) + (0.0005802190000000001 * fTempPerm505)) + (0.007368019999999999 * fTempPerm26)))))) + (0.0015759 * fTempPerm20))))) + (0.00050954 * fTempPerm309)))) + (0.0023355800000000003 * fTempPerm307)) + (0.00016274 * fTempPerm8)))))) + (0.0007023 * fTempPerm301)))))))));
			fRec7[0] = max((fRec7[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm513))))));
			fbargraph5 = fRec7[0];
			output1[i] = (FAUSTFLOAT)fTempPerm513;
			// post processing
			fRec7[1] = fRec7[0];
			fRec0[1] = fRec0[0];
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
