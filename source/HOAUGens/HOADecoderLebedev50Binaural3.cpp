//----------------------------------------------------------
// name: "HOADecoderLebedev50Binaural3"
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
	double 	fRec4[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fTempPerm15;
	double 	fVec1[128];
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
	double 	fTempPerm34;
	double 	fRec5[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fTempPerm35;
	double 	fVec2[128];
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
	double 	fRec6[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fTempPerm51;
	double 	fVec3[128];
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
	double 	fTempPerm137;
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
	double 	fRec7[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fTempPerm213;
	double 	fVec4[128];
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
	double 	fRec8[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fTempPerm227;
	double 	fVec5[128];
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
	double 	fRec9[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fTempPerm301;
	double 	fVec6[128];
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
	double 	fRec10[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fTempPerm314;
	double 	fVec7[128];
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
	double 	fRec11[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fTempPerm329;
	double 	fVec8[128];
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
	double 	fRec12[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fTempPerm351;
	double 	fVec9[128];
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
	double 	fRec13[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fTempPerm375;
	double 	fVec10[128];
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
	double 	fRec14[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fTempPerm491;
	double 	fVec11[128];
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
	double 	fRec15[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fTempPerm506;
	double 	fVec12[128];
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
	double 	fRec16[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fTempPerm523;
	double 	fVec13[128];
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
	double 	fRec17[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fTempPerm665;
	double 	fVec14[128];
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
	double 	fRec18[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fTempPerm817;
	double 	fVec15[128];
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
	double 	fTempPerm1153;
	double 	fTempPerm1154;
	double 	fTempPerm1155;
	double 	fTempPerm1156;
	double 	fTempPerm1157;
	double 	fTempPerm1158;
	double 	fTempPerm1159;
	double 	fTempPerm1160;
	double 	fTempPerm1161;
	double 	fTempPerm1162;
	double 	fTempPerm1163;
	double 	fTempPerm1164;
	double 	fTempPerm1165;
	double 	fTempPerm1166;
	double 	fTempPerm1167;
	double 	fTempPerm1168;
	double 	fTempPerm1169;
	double 	fTempPerm1170;
	double 	fTempPerm1171;
	double 	fTempPerm1172;
	double 	fTempPerm1173;
	double 	fTempPerm1174;
	double 	fTempPerm1175;
	double 	fTempPerm1176;
	double 	fTempPerm1177;
	double 	fTempPerm1178;
	double 	fTempPerm1179;
	double 	fTempPerm1180;
	double 	fTempPerm1181;
	double 	fTempPerm1182;
	double 	fTempPerm1183;
	double 	fTempPerm1184;
	double 	fTempPerm1185;
	double 	fTempPerm1186;
	double 	fTempPerm1187;
	double 	fTempPerm1188;
	double 	fTempPerm1189;
	double 	fTempPerm1190;
	double 	fTempPerm1191;
	double 	fTempPerm1192;
	double 	fTempPerm1193;
	double 	fTempPerm1194;
	double 	fTempPerm1195;
	double 	fTempPerm1196;
	double 	fTempPerm1197;
	double 	fTempPerm1198;
	double 	fTempPerm1199;
	double 	fTempPerm1200;
	double 	fTempPerm1201;
	double 	fTempPerm1202;
	double 	fTempPerm1203;
	double 	fTempPerm1204;
	double 	fTempPerm1205;
	double 	fTempPerm1206;
	double 	fTempPerm1207;
	double 	fTempPerm1208;
	double 	fTempPerm1209;
	double 	fTempPerm1210;
	double 	fTempPerm1211;
	double 	fTempPerm1212;
	double 	fTempPerm1213;
	double 	fTempPerm1214;
	double 	fTempPerm1215;
	double 	fTempPerm1216;
	double 	fTempPerm1217;
	double 	fTempPerm1218;
	double 	fTempPerm1219;
	double 	fTempPerm1220;
	double 	fTempPerm1221;
	double 	fTempPerm1222;
	double 	fTempPerm1223;
	double 	fTempPerm1224;
	double 	fTempPerm1225;
	double 	fTempPerm1226;
	double 	fTempPerm1227;
	double 	fTempPerm1228;
	double 	fTempPerm1229;
	double 	fTempPerm1230;
	double 	fTempPerm1231;
	double 	fTempPerm1232;
	double 	fTempPerm1233;
	double 	fTempPerm1234;
	double 	fTempPerm1235;
	double 	fTempPerm1236;
	double 	fTempPerm1237;
	double 	fTempPerm1238;
	double 	fTempPerm1239;
	double 	fTempPerm1240;
	double 	fTempPerm1241;
	double 	fTempPerm1242;
	double 	fTempPerm1243;
	double 	fTempPerm1244;
	double 	fTempPerm1245;
	double 	fTempPerm1246;
	double 	fTempPerm1247;
	double 	fTempPerm1248;
	double 	fTempPerm1249;
	double 	fTempPerm1250;
	double 	fTempPerm1251;
	double 	fTempPerm1252;
	double 	fTempPerm1253;
	double 	fTempPerm1254;
	double 	fTempPerm1255;
	double 	fTempPerm1256;
	double 	fTempPerm1257;
	double 	fTempPerm1258;
	double 	fTempPerm1259;
	double 	fTempPerm1260;
	double 	fTempPerm1261;
	double 	fTempPerm1262;
	double 	fTempPerm1263;
	double 	fTempPerm1264;
	double 	fTempPerm1265;
	double 	fTempPerm1266;
	double 	fTempPerm1267;
	double 	fTempPerm1268;
	double 	fTempPerm1269;
	double 	fTempPerm1270;
	double 	fTempPerm1271;
	double 	fTempPerm1272;
	double 	fTempPerm1273;
	double 	fTempPerm1274;
	double 	fTempPerm1275;
	double 	fTempPerm1276;
	double 	fTempPerm1277;
	double 	fTempPerm1278;
	double 	fTempPerm1279;
	double 	fTempPerm1280;
	double 	fTempPerm1281;
	double 	fTempPerm1282;
	double 	fTempPerm1283;
	double 	fTempPerm1284;
	double 	fTempPerm1285;
	double 	fTempPerm1286;
	double 	fTempPerm1287;
	double 	fTempPerm1288;
	double 	fTempPerm1289;
	double 	fTempPerm1290;
	double 	fTempPerm1291;
	double 	fTempPerm1292;
	double 	fTempPerm1293;
	double 	fTempPerm1294;
	double 	fTempPerm1295;
	double 	fTempPerm1296;
	double 	fTempPerm1297;
	double 	fTempPerm1298;
	double 	fTempPerm1299;
	double 	fTempPerm1300;
	double 	fTempPerm1301;
	double 	fTempPerm1302;
	double 	fTempPerm1303;
	double 	fTempPerm1304;
	double 	fTempPerm1305;
	double 	fTempPerm1306;
	double 	fTempPerm1307;
	double 	fTempPerm1308;
	double 	fTempPerm1309;
	double 	fTempPerm1310;
	double 	fTempPerm1311;
	double 	fTempPerm1312;
	double 	fTempPerm1313;
	double 	fTempPerm1314;
	double 	fTempPerm1315;
	double 	fTempPerm1316;
	double 	fTempPerm1317;
	double 	fTempPerm1318;
	double 	fTempPerm1319;
	double 	fTempPerm1320;
	double 	fTempPerm1321;
	double 	fTempPerm1322;
	double 	fTempPerm1323;
	double 	fTempPerm1324;
	double 	fTempPerm1325;
	double 	fTempPerm1326;
	double 	fTempPerm1327;
	double 	fTempPerm1328;
	double 	fTempPerm1329;
	double 	fTempPerm1330;
	double 	fTempPerm1331;
	double 	fTempPerm1332;
	double 	fTempPerm1333;
	double 	fTempPerm1334;
	double 	fTempPerm1335;
	double 	fTempPerm1336;
	double 	fTempPerm1337;
	double 	fTempPerm1338;
	double 	fTempPerm1339;
	double 	fTempPerm1340;
	double 	fTempPerm1341;
	double 	fTempPerm1342;
	double 	fTempPerm1343;
	double 	fTempPerm1344;
	double 	fTempPerm1345;
	double 	fTempPerm1346;
	double 	fTempPerm1347;
	double 	fTempPerm1348;
	double 	fTempPerm1349;
	double 	fTempPerm1350;
	double 	fTempPerm1351;
	double 	fTempPerm1352;
	double 	fTempPerm1353;
	double 	fTempPerm1354;
	double 	fTempPerm1355;
	double 	fTempPerm1356;
	double 	fTempPerm1357;
	double 	fTempPerm1358;
	double 	fTempPerm1359;
	double 	fTempPerm1360;
	double 	fTempPerm1361;
	double 	fTempPerm1362;
	double 	fTempPerm1363;
	double 	fTempPerm1364;
	double 	fTempPerm1365;
	double 	fTempPerm1366;
	double 	fTempPerm1367;
	double 	fTempPerm1368;
	double 	fTempPerm1369;
	double 	fTempPerm1370;
	double 	fTempPerm1371;
	double 	fTempPerm1372;
	double 	fTempPerm1373;
	double 	fTempPerm1374;
	double 	fTempPerm1375;
	double 	fTempPerm1376;
	double 	fTempPerm1377;
	double 	fTempPerm1378;
	double 	fTempPerm1379;
	double 	fTempPerm1380;
	double 	fTempPerm1381;
	double 	fTempPerm1382;
	double 	fTempPerm1383;
	double 	fTempPerm1384;
	double 	fTempPerm1385;
	double 	fTempPerm1386;
	double 	fTempPerm1387;
	double 	fTempPerm1388;
	double 	fTempPerm1389;
	double 	fTempPerm1390;
	double 	fTempPerm1391;
	double 	fTempPerm1392;
	double 	fTempPerm1393;
	double 	fTempPerm1394;
	double 	fTempPerm1395;
	double 	fTempPerm1396;
	double 	fTempPerm1397;
	double 	fTempPerm1398;
	double 	fTempPerm1399;
	double 	fTempPerm1400;
	double 	fTempPerm1401;
	double 	fTempPerm1402;
	double 	fTempPerm1403;
	double 	fTempPerm1404;
	double 	fTempPerm1405;
	double 	fTempPerm1406;
	double 	fTempPerm1407;
	double 	fTempPerm1408;
	double 	fTempPerm1409;
	double 	fTempPerm1410;
	double 	fTempPerm1411;
	double 	fTempPerm1412;
	double 	fTempPerm1413;
	double 	fTempPerm1414;
	double 	fTempPerm1415;
	double 	fTempPerm1416;
	double 	fTempPerm1417;
	double 	fTempPerm1418;
	double 	fTempPerm1419;
	double 	fTempPerm1420;
	double 	fTempPerm1421;
	double 	fTempPerm1422;
	double 	fTempPerm1423;
	double 	fTempPerm1424;
	double 	fTempPerm1425;
	double 	fTempPerm1426;
	double 	fTempPerm1427;
	double 	fTempPerm1428;
	double 	fTempPerm1429;
	double 	fTempPerm1430;
	double 	fTempPerm1431;
	double 	fTempPerm1432;
	double 	fTempPerm1433;
	double 	fTempPerm1434;
	double 	fTempPerm1435;
	double 	fTempPerm1436;
	double 	fTempPerm1437;
	double 	fTempPerm1438;
	double 	fTempPerm1439;
	double 	fTempPerm1440;
	double 	fTempPerm1441;
	double 	fTempPerm1442;
	double 	fTempPerm1443;
	double 	fTempPerm1444;
	double 	fTempPerm1445;
	double 	fTempPerm1446;
	double 	fTempPerm1447;
	double 	fTempPerm1448;
	double 	fTempPerm1449;
	double 	fTempPerm1450;
	double 	fTempPerm1451;
	double 	fTempPerm1452;
	double 	fTempPerm1453;
	double 	fTempPerm1454;
	double 	fTempPerm1455;
	double 	fTempPerm1456;
	double 	fTempPerm1457;
	double 	fTempPerm1458;
	double 	fTempPerm1459;
	double 	fTempPerm1460;
	double 	fTempPerm1461;
	double 	fTempPerm1462;
	double 	fTempPerm1463;
	double 	fTempPerm1464;
	double 	fTempPerm1465;
	double 	fTempPerm1466;
	double 	fTempPerm1467;
	double 	fTempPerm1468;
	double 	fTempPerm1469;
	double 	fTempPerm1470;
	double 	fTempPerm1471;
	double 	fTempPerm1472;
	double 	fTempPerm1473;
	double 	fTempPerm1474;
	double 	fTempPerm1475;
	double 	fTempPerm1476;
	double 	fTempPerm1477;
	double 	fTempPerm1478;
	double 	fTempPerm1479;
	double 	fTempPerm1480;
	double 	fTempPerm1481;
	double 	fTempPerm1482;
	double 	fTempPerm1483;
	double 	fTempPerm1484;
	double 	fTempPerm1485;
	double 	fTempPerm1486;
	double 	fTempPerm1487;
	double 	fTempPerm1488;
	double 	fTempPerm1489;
	double 	fTempPerm1490;
	double 	fTempPerm1491;
	double 	fTempPerm1492;
	double 	fTempPerm1493;
	double 	fTempPerm1494;
	double 	fTempPerm1495;
	double 	fTempPerm1496;
	double 	fTempPerm1497;
	double 	fTempPerm1498;
	double 	fTempPerm1499;
	double 	fTempPerm1500;
	double 	fTempPerm1501;
	double 	fTempPerm1502;
	double 	fTempPerm1503;
	double 	fTempPerm1504;
	double 	fTempPerm1505;
	double 	fTempPerm1506;
	double 	fTempPerm1507;
	double 	fTempPerm1508;
	double 	fTempPerm1509;
	double 	fTempPerm1510;
	double 	fTempPerm1511;
	double 	fTempPerm1512;
	double 	fTempPerm1513;
	double 	fTempPerm1514;
	double 	fTempPerm1515;
	double 	fTempPerm1516;
	double 	fTempPerm1517;
	double 	fTempPerm1518;
	double 	fTempPerm1519;
	double 	fTempPerm1520;
	double 	fTempPerm1521;
	double 	fTempPerm1522;
	double 	fTempPerm1523;
	double 	fTempPerm1524;
	double 	fTempPerm1525;
	double 	fTempPerm1526;
	double 	fTempPerm1527;
	double 	fTempPerm1528;
	double 	fTempPerm1529;
	double 	fTempPerm1530;
	double 	fTempPerm1531;
	double 	fTempPerm1532;
	double 	fTempPerm1533;
	double 	fTempPerm1534;
	double 	fTempPerm1535;
	double 	fTempPerm1536;
	double 	fTempPerm1537;
	double 	fTempPerm1538;
	double 	fTempPerm1539;
	double 	fTempPerm1540;
	double 	fTempPerm1541;
	double 	fTempPerm1542;
	double 	fTempPerm1543;
	double 	fTempPerm1544;
	double 	fTempPerm1545;
	double 	fTempPerm1546;
	double 	fTempPerm1547;
	double 	fTempPerm1548;
	double 	fTempPerm1549;
	double 	fTempPerm1550;
	double 	fTempPerm1551;
	double 	fTempPerm1552;
	double 	fTempPerm1553;
	double 	fTempPerm1554;
	double 	fTempPerm1555;
	double 	fTempPerm1556;
	double 	fTempPerm1557;
	double 	fTempPerm1558;
	double 	fTempPerm1559;
	double 	fTempPerm1560;
	double 	fTempPerm1561;
	double 	fTempPerm1562;
	double 	fTempPerm1563;
	double 	fTempPerm1564;
	double 	fTempPerm1565;
	double 	fTempPerm1566;
	double 	fTempPerm1567;
	double 	fTempPerm1568;
	double 	fTempPerm1569;
	double 	fTempPerm1570;
	double 	fTempPerm1571;
	double 	fTempPerm1572;
	double 	fTempPerm1573;
	double 	fTempPerm1574;
	double 	fTempPerm1575;
	double 	fTempPerm1576;
	double 	fTempPerm1577;
	double 	fTempPerm1578;
	double 	fTempPerm1579;
	double 	fTempPerm1580;
	double 	fTempPerm1581;
	double 	fTempPerm1582;
	double 	fTempPerm1583;
	double 	fTempPerm1584;
	double 	fTempPerm1585;
	double 	fTempPerm1586;
	double 	fTempPerm1587;
	double 	fTempPerm1588;
	double 	fTempPerm1589;
	double 	fTempPerm1590;
	double 	fTempPerm1591;
	double 	fTempPerm1592;
	double 	fTempPerm1593;
	double 	fTempPerm1594;
	double 	fTempPerm1595;
	double 	fTempPerm1596;
	double 	fTempPerm1597;
	double 	fTempPerm1598;
	double 	fTempPerm1599;
	double 	fTempPerm1600;
	double 	fTempPerm1601;
	double 	fTempPerm1602;
	double 	fTempPerm1603;
	double 	fTempPerm1604;
	double 	fTempPerm1605;
	double 	fTempPerm1606;
	double 	fTempPerm1607;
	double 	fTempPerm1608;
	double 	fTempPerm1609;
	double 	fTempPerm1610;
	double 	fTempPerm1611;
	double 	fTempPerm1612;
	double 	fTempPerm1613;
	double 	fTempPerm1614;
	double 	fTempPerm1615;
	double 	fTempPerm1616;
	double 	fTempPerm1617;
	double 	fTempPerm1618;
	double 	fTempPerm1619;
	double 	fTempPerm1620;
	double 	fTempPerm1621;
	double 	fTempPerm1622;
	double 	fTempPerm1623;
	double 	fTempPerm1624;
	double 	fTempPerm1625;
	double 	fTempPerm1626;
	double 	fTempPerm1627;
	double 	fTempPerm1628;
	double 	fTempPerm1629;
	double 	fTempPerm1630;
	double 	fTempPerm1631;
	double 	fTempPerm1632;
	double 	fTempPerm1633;
	double 	fTempPerm1634;
	double 	fTempPerm1635;
	double 	fTempPerm1636;
	double 	fTempPerm1637;
	double 	fTempPerm1638;
	double 	fTempPerm1639;
	double 	fTempPerm1640;
	double 	fTempPerm1641;
	double 	fTempPerm1642;
	double 	fTempPerm1643;
	double 	fTempPerm1644;
	double 	fTempPerm1645;
	double 	fTempPerm1646;
	double 	fTempPerm1647;
	double 	fTempPerm1648;
	double 	fTempPerm1649;
	double 	fTempPerm1650;
	double 	fTempPerm1651;
	double 	fTempPerm1652;
	double 	fTempPerm1653;
	double 	fTempPerm1654;
	double 	fTempPerm1655;
	double 	fTempPerm1656;
	double 	fTempPerm1657;
	double 	fTempPerm1658;
	double 	fTempPerm1659;
	double 	fTempPerm1660;
	double 	fTempPerm1661;
	double 	fTempPerm1662;
	double 	fTempPerm1663;
	double 	fTempPerm1664;
	double 	fTempPerm1665;
	double 	fTempPerm1666;
	double 	fTempPerm1667;
	double 	fTempPerm1668;
	double 	fTempPerm1669;
	double 	fTempPerm1670;
	double 	fTempPerm1671;
	double 	fTempPerm1672;
	double 	fTempPerm1673;
	double 	fTempPerm1674;
	double 	fTempPerm1675;
	double 	fTempPerm1676;
	double 	fTempPerm1677;
	double 	fTempPerm1678;
	double 	fTempPerm1679;
	double 	fTempPerm1680;
	double 	fTempPerm1681;
	double 	fTempPerm1682;
	double 	fTempPerm1683;
	double 	fTempPerm1684;
	double 	fTempPerm1685;
	double 	fTempPerm1686;
	double 	fTempPerm1687;
	double 	fTempPerm1688;
	double 	fTempPerm1689;
	double 	fTempPerm1690;
	double 	fTempPerm1691;
	double 	fTempPerm1692;
	double 	fTempPerm1693;
	double 	fTempPerm1694;
	double 	fTempPerm1695;
	double 	fTempPerm1696;
	double 	fTempPerm1697;
	double 	fTempPerm1698;
	double 	fTempPerm1699;
	double 	fTempPerm1700;
	double 	fTempPerm1701;
	double 	fTempPerm1702;
	double 	fTempPerm1703;
	double 	fTempPerm1704;
	double 	fTempPerm1705;
	double 	fTempPerm1706;
	double 	fTempPerm1707;
	double 	fTempPerm1708;
	double 	fTempPerm1709;
	double 	fTempPerm1710;
	double 	fTempPerm1711;
	double 	fTempPerm1712;
	double 	fTempPerm1713;
	double 	fTempPerm1714;
	double 	fTempPerm1715;
	double 	fTempPerm1716;
	double 	fTempPerm1717;
	double 	fTempPerm1718;
	double 	fTempPerm1719;
	double 	fTempPerm1720;
	double 	fTempPerm1721;
	double 	fTempPerm1722;
	double 	fTempPerm1723;
	double 	fTempPerm1724;
	double 	fTempPerm1725;
	double 	fTempPerm1726;
	double 	fTempPerm1727;
	double 	fTempPerm1728;
	double 	fTempPerm1729;
	double 	fTempPerm1730;
	double 	fTempPerm1731;
	double 	fTempPerm1732;
	double 	fTempPerm1733;
	double 	fTempPerm1734;
	double 	fTempPerm1735;
	double 	fTempPerm1736;
	double 	fTempPerm1737;
	double 	fTempPerm1738;
	double 	fTempPerm1739;
	double 	fTempPerm1740;
	double 	fTempPerm1741;
	double 	fTempPerm1742;
	double 	fTempPerm1743;
	double 	fTempPerm1744;
	double 	fTempPerm1745;
	double 	fTempPerm1746;
	double 	fTempPerm1747;
	double 	fTempPerm1748;
	double 	fTempPerm1749;
	double 	fTempPerm1750;
	double 	fTempPerm1751;
	double 	fTempPerm1752;
	double 	fTempPerm1753;
	double 	fTempPerm1754;
	double 	fTempPerm1755;
	double 	fTempPerm1756;
	double 	fTempPerm1757;
	double 	fTempPerm1758;
	double 	fTempPerm1759;
	double 	fTempPerm1760;
	double 	fTempPerm1761;
	double 	fTempPerm1762;
	double 	fTempPerm1763;
	double 	fTempPerm1764;
	double 	fTempPerm1765;
	double 	fTempPerm1766;
	double 	fTempPerm1767;
	double 	fTempPerm1768;
	double 	fTempPerm1769;
	double 	fTempPerm1770;
	double 	fTempPerm1771;
	double 	fTempPerm1772;
	double 	fTempPerm1773;
	double 	fTempPerm1774;
	double 	fTempPerm1775;
	double 	fTempPerm1776;
	double 	fTempPerm1777;
	double 	fTempPerm1778;
	double 	fTempPerm1779;
	double 	fTempPerm1780;
	double 	fTempPerm1781;
	double 	fTempPerm1782;
	double 	fTempPerm1783;
	double 	fTempPerm1784;
	double 	fTempPerm1785;
	double 	fTempPerm1786;
	double 	fTempPerm1787;
	double 	fTempPerm1788;
	double 	fTempPerm1789;
	double 	fTempPerm1790;
	double 	fTempPerm1791;
	double 	fTempPerm1792;
	double 	fTempPerm1793;
	double 	fTempPerm1794;
	double 	fTempPerm1795;
	double 	fTempPerm1796;
	double 	fTempPerm1797;
	double 	fTempPerm1798;
	double 	fTempPerm1799;
	double 	fTempPerm1800;
	double 	fTempPerm1801;
	double 	fTempPerm1802;
	double 	fTempPerm1803;
	double 	fTempPerm1804;
	double 	fTempPerm1805;
	double 	fTempPerm1806;
	double 	fTempPerm1807;
	double 	fTempPerm1808;
	double 	fTempPerm1809;
	double 	fTempPerm1810;
	double 	fTempPerm1811;
	double 	fTempPerm1812;
	double 	fTempPerm1813;
	double 	fTempPerm1814;
	double 	fTempPerm1815;
	double 	fTempPerm1816;
	double 	fTempPerm1817;
	double 	fTempPerm1818;
	double 	fTempPerm1819;
	double 	fTempPerm1820;
	double 	fTempPerm1821;
	double 	fTempPerm1822;
	double 	fTempPerm1823;
	double 	fTempPerm1824;
	double 	fTempPerm1825;
	double 	fTempPerm1826;
	double 	fTempPerm1827;
	double 	fTempPerm1828;
	double 	fTempPerm1829;
	double 	fTempPerm1830;
	double 	fTempPerm1831;
	double 	fTempPerm1832;
	double 	fTempPerm1833;
	double 	fTempPerm1834;
	double 	fTempPerm1835;
	double 	fTempPerm1836;
	double 	fTempPerm1837;
	double 	fTempPerm1838;
	double 	fTempPerm1839;
	double 	fTempPerm1840;
	double 	fTempPerm1841;
	double 	fTempPerm1842;
	double 	fTempPerm1843;
	double 	fTempPerm1844;
	double 	fTempPerm1845;
	double 	fTempPerm1846;
	double 	fTempPerm1847;
	double 	fTempPerm1848;
	double 	fTempPerm1849;
	double 	fTempPerm1850;
	double 	fTempPerm1851;
	double 	fTempPerm1852;
	double 	fTempPerm1853;
	double 	fTempPerm1854;
	double 	fTempPerm1855;
	double 	fTempPerm1856;
	double 	fTempPerm1857;
	double 	fTempPerm1858;
	double 	fTempPerm1859;
	double 	fTempPerm1860;
	double 	fTempPerm1861;
	double 	fTempPerm1862;
	double 	fTempPerm1863;
	double 	fTempPerm1864;
	double 	fTempPerm1865;
	double 	fTempPerm1866;
	double 	fTempPerm1867;
	double 	fTempPerm1868;
	double 	fTempPerm1869;
	double 	fTempPerm1870;
	double 	fTempPerm1871;
	double 	fTempPerm1872;
	double 	fTempPerm1873;
	double 	fTempPerm1874;
	double 	fTempPerm1875;
	double 	fTempPerm1876;
	double 	fTempPerm1877;
	double 	fTempPerm1878;
	double 	fTempPerm1879;
	double 	fTempPerm1880;
	double 	fTempPerm1881;
	double 	fTempPerm1882;
	double 	fTempPerm1883;
	double 	fTempPerm1884;
	double 	fTempPerm1885;
	double 	fTempPerm1886;
	double 	fTempPerm1887;
	double 	fTempPerm1888;
	double 	fTempPerm1889;
	double 	fTempPerm1890;
	double 	fTempPerm1891;
	double 	fTempPerm1892;
	double 	fTempPerm1893;
	double 	fTempPerm1894;
	double 	fTempPerm1895;
	double 	fTempPerm1896;
	double 	fTempPerm1897;
	double 	fTempPerm1898;
	double 	fTempPerm1899;
	double 	fTempPerm1900;
	double 	fTempPerm1901;
	double 	fTempPerm1902;
	double 	fTempPerm1903;
	double 	fTempPerm1904;
	double 	fTempPerm1905;
	double 	fTempPerm1906;
	double 	fTempPerm1907;
	double 	fTempPerm1908;
	double 	fTempPerm1909;
	double 	fTempPerm1910;
	double 	fTempPerm1911;
	double 	fTempPerm1912;
	double 	fTempPerm1913;
	double 	fTempPerm1914;
	double 	fTempPerm1915;
	double 	fTempPerm1916;
	double 	fTempPerm1917;
	double 	fTempPerm1918;
	double 	fTempPerm1919;
	double 	fTempPerm1920;
	double 	fTempPerm1921;
	double 	fTempPerm1922;
	double 	fTempPerm1923;
	double 	fTempPerm1924;
	double 	fTempPerm1925;
	double 	fTempPerm1926;
	double 	fTempPerm1927;
	double 	fTempPerm1928;
	double 	fTempPerm1929;
	double 	fTempPerm1930;
	double 	fTempPerm1931;
	double 	fTempPerm1932;
	double 	fTempPerm1933;
	double 	fTempPerm1934;
	double 	fTempPerm1935;
	double 	fTempPerm1936;
	double 	fTempPerm1937;
	double 	fTempPerm1938;
	double 	fTempPerm1939;
	double 	fTempPerm1940;
	double 	fTempPerm1941;
	double 	fTempPerm1942;
	double 	fTempPerm1943;
	double 	fTempPerm1944;
	double 	fTempPerm1945;
	double 	fTempPerm1946;
	double 	fTempPerm1947;
	double 	fTempPerm1948;
	double 	fTempPerm1949;
	double 	fTempPerm1950;
	double 	fTempPerm1951;
	double 	fTempPerm1952;
	double 	fTempPerm1953;
	double 	fTempPerm1954;
	double 	fTempPerm1955;
	double 	fTempPerm1956;
	double 	fTempPerm1957;
	double 	fTempPerm1958;
	double 	fTempPerm1959;
	double 	fTempPerm1960;
	double 	fTempPerm1961;
	double 	fTempPerm1962;
	double 	fTempPerm1963;
	double 	fTempPerm1964;
	double 	fTempPerm1965;
	double 	fTempPerm1966;
	double 	fTempPerm1967;
	double 	fTempPerm1968;
	double 	fTempPerm1969;
	double 	fTempPerm1970;
	double 	fTempPerm1971;
	double 	fTempPerm1972;
	double 	fTempPerm1973;
	double 	fTempPerm1974;
	double 	fTempPerm1975;
	double 	fTempPerm1976;
	double 	fTempPerm1977;
	double 	fTempPerm1978;
	double 	fTempPerm1979;
	double 	fTempPerm1980;
	double 	fTempPerm1981;
	double 	fTempPerm1982;
	double 	fTempPerm1983;
	double 	fTempPerm1984;
	double 	fTempPerm1985;
	double 	fTempPerm1986;
	double 	fTempPerm1987;
	double 	fTempPerm1988;
	double 	fTempPerm1989;
	double 	fTempPerm1990;
	double 	fTempPerm1991;
	double 	fTempPerm1992;
	double 	fTempPerm1993;
	double 	fTempPerm1994;
	double 	fTempPerm1995;
	double 	fTempPerm1996;
	double 	fTempPerm1997;
	double 	fTempPerm1998;
	double 	fTempPerm1999;
	double 	fTempPerm2000;
	double 	fTempPerm2001;
	double 	fTempPerm2002;
	double 	fTempPerm2003;
	double 	fTempPerm2004;
	double 	fTempPerm2005;
	double 	fTempPerm2006;
	double 	fTempPerm2007;
	double 	fTempPerm2008;
	double 	fTempPerm2009;
	double 	fTempPerm2010;
	double 	fTempPerm2011;
	double 	fTempPerm2012;
	double 	fTempPerm2013;
	double 	fTempPerm2014;
	double 	fTempPerm2015;
	double 	fTempPerm2016;
	double 	fTempPerm2017;
	double 	fTempPerm2018;
	double 	fTempPerm2019;
	double 	fTempPerm2020;
	double 	fTempPerm2021;
	double 	fTempPerm2022;
	double 	fTempPerm2023;
	double 	fTempPerm2024;
	double 	fTempPerm2025;
	double 	fTempPerm2026;
	double 	fTempPerm2027;
	double 	fTempPerm2028;
	double 	fTempPerm2029;
	double 	fTempPerm2030;
	double 	fTempPerm2031;
	double 	fTempPerm2032;
	double 	fTempPerm2033;
	double 	fTempPerm2034;
	double 	fTempPerm2035;
	double 	fTempPerm2036;
	double 	fTempPerm2037;
	double 	fTempPerm2038;
	double 	fTempPerm2039;
	double 	fTempPerm2040;
	double 	fTempPerm2041;
	double 	fTempPerm2042;
	double 	fTempPerm2043;
	double 	fTempPerm2044;
	double 	fTempPerm2045;
	double 	fTempPerm2046;
	double 	fTempPerm2047;
	double 	fTempPerm2048;
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fTempPerm2049;
	double 	fRec19[2];
	FAUSTFLOAT 	fbargraph17;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOADecoderLebedev50Binaural3");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL)");
		m->declare("copyright", "(c) Pierre Lecomte 2015");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("gui.lib/name", "GUI Library");
		m->declare("gui.lib/version", "1.0");
		m->declare("gui.lib/author", "Pierre Lecomte");
		m->declare("gui.lib/license", "GPL");
		m->declare("gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
	}

	virtual int getNumInputs() { return 16; }
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
		fTempPerm1154 = 0;
		fTempPerm1155 = 0;
		fTempPerm1156 = 0;
		fTempPerm1157 = 0;
		fTempPerm1158 = 0;
		fTempPerm1159 = 0;
		fTempPerm1160 = 0;
		fTempPerm1161 = 0;
		fTempPerm1162 = 0;
		fTempPerm1163 = 0;
		fTempPerm1164 = 0;
		fTempPerm1165 = 0;
		fTempPerm1166 = 0;
		fTempPerm1167 = 0;
		fTempPerm1168 = 0;
		fTempPerm1169 = 0;
		fTempPerm1170 = 0;
		fTempPerm1171 = 0;
		fTempPerm1172 = 0;
		fTempPerm1173 = 0;
		fTempPerm1174 = 0;
		fTempPerm1175 = 0;
		fTempPerm1176 = 0;
		fTempPerm1177 = 0;
		fTempPerm1178 = 0;
		fTempPerm1179 = 0;
		fTempPerm1180 = 0;
		fTempPerm1181 = 0;
		fTempPerm1182 = 0;
		fTempPerm1183 = 0;
		fTempPerm1184 = 0;
		fTempPerm1185 = 0;
		fTempPerm1186 = 0;
		fTempPerm1187 = 0;
		fTempPerm1188 = 0;
		fTempPerm1189 = 0;
		fTempPerm1190 = 0;
		fTempPerm1191 = 0;
		fTempPerm1192 = 0;
		fTempPerm1193 = 0;
		fTempPerm1194 = 0;
		fTempPerm1195 = 0;
		fTempPerm1196 = 0;
		fTempPerm1197 = 0;
		fTempPerm1198 = 0;
		fTempPerm1199 = 0;
		fTempPerm1200 = 0;
		fTempPerm1201 = 0;
		fTempPerm1202 = 0;
		fTempPerm1203 = 0;
		fTempPerm1204 = 0;
		fTempPerm1205 = 0;
		fTempPerm1206 = 0;
		fTempPerm1207 = 0;
		fTempPerm1208 = 0;
		fTempPerm1209 = 0;
		fTempPerm1210 = 0;
		fTempPerm1211 = 0;
		fTempPerm1212 = 0;
		fTempPerm1213 = 0;
		fTempPerm1214 = 0;
		fTempPerm1215 = 0;
		fTempPerm1216 = 0;
		fTempPerm1217 = 0;
		fTempPerm1218 = 0;
		fTempPerm1219 = 0;
		fTempPerm1220 = 0;
		fTempPerm1221 = 0;
		fTempPerm1222 = 0;
		fTempPerm1223 = 0;
		fTempPerm1224 = 0;
		fTempPerm1225 = 0;
		fTempPerm1226 = 0;
		fTempPerm1227 = 0;
		fTempPerm1228 = 0;
		fTempPerm1229 = 0;
		fTempPerm1230 = 0;
		fTempPerm1231 = 0;
		fTempPerm1232 = 0;
		fTempPerm1233 = 0;
		fTempPerm1234 = 0;
		fTempPerm1235 = 0;
		fTempPerm1236 = 0;
		fTempPerm1237 = 0;
		fTempPerm1238 = 0;
		fTempPerm1239 = 0;
		fTempPerm1240 = 0;
		fTempPerm1241 = 0;
		fTempPerm1242 = 0;
		fTempPerm1243 = 0;
		fTempPerm1244 = 0;
		fTempPerm1245 = 0;
		fTempPerm1246 = 0;
		fTempPerm1247 = 0;
		fTempPerm1248 = 0;
		fTempPerm1249 = 0;
		fTempPerm1250 = 0;
		fTempPerm1251 = 0;
		fTempPerm1252 = 0;
		fTempPerm1253 = 0;
		fTempPerm1254 = 0;
		fTempPerm1255 = 0;
		fTempPerm1256 = 0;
		fTempPerm1257 = 0;
		fTempPerm1258 = 0;
		fTempPerm1259 = 0;
		fTempPerm1260 = 0;
		fTempPerm1261 = 0;
		fTempPerm1262 = 0;
		fTempPerm1263 = 0;
		fTempPerm1264 = 0;
		fTempPerm1265 = 0;
		fTempPerm1266 = 0;
		fTempPerm1267 = 0;
		fTempPerm1268 = 0;
		fTempPerm1269 = 0;
		fTempPerm1270 = 0;
		fTempPerm1271 = 0;
		fTempPerm1272 = 0;
		fTempPerm1273 = 0;
		fTempPerm1274 = 0;
		fTempPerm1275 = 0;
		fTempPerm1276 = 0;
		fTempPerm1277 = 0;
		fTempPerm1278 = 0;
		fTempPerm1279 = 0;
		fTempPerm1280 = 0;
		fTempPerm1281 = 0;
		fTempPerm1282 = 0;
		fTempPerm1283 = 0;
		fTempPerm1284 = 0;
		fTempPerm1285 = 0;
		fTempPerm1286 = 0;
		fTempPerm1287 = 0;
		fTempPerm1288 = 0;
		fTempPerm1289 = 0;
		fTempPerm1290 = 0;
		fTempPerm1291 = 0;
		fTempPerm1292 = 0;
		fTempPerm1293 = 0;
		fTempPerm1294 = 0;
		fTempPerm1295 = 0;
		fTempPerm1296 = 0;
		fTempPerm1297 = 0;
		fTempPerm1298 = 0;
		fTempPerm1299 = 0;
		fTempPerm1300 = 0;
		fTempPerm1301 = 0;
		fTempPerm1302 = 0;
		fTempPerm1303 = 0;
		fTempPerm1304 = 0;
		fTempPerm1305 = 0;
		fTempPerm1306 = 0;
		fTempPerm1307 = 0;
		fTempPerm1308 = 0;
		fTempPerm1309 = 0;
		fTempPerm1310 = 0;
		fTempPerm1311 = 0;
		fTempPerm1312 = 0;
		fTempPerm1313 = 0;
		fTempPerm1314 = 0;
		fTempPerm1315 = 0;
		fTempPerm1316 = 0;
		fTempPerm1317 = 0;
		fTempPerm1318 = 0;
		fTempPerm1319 = 0;
		fTempPerm1320 = 0;
		fTempPerm1321 = 0;
		fTempPerm1322 = 0;
		fTempPerm1323 = 0;
		fTempPerm1324 = 0;
		fTempPerm1325 = 0;
		fTempPerm1326 = 0;
		fTempPerm1327 = 0;
		fTempPerm1328 = 0;
		fTempPerm1329 = 0;
		fTempPerm1330 = 0;
		fTempPerm1331 = 0;
		fTempPerm1332 = 0;
		fTempPerm1333 = 0;
		fTempPerm1334 = 0;
		fTempPerm1335 = 0;
		fTempPerm1336 = 0;
		fTempPerm1337 = 0;
		fTempPerm1338 = 0;
		fTempPerm1339 = 0;
		fTempPerm1340 = 0;
		fTempPerm1341 = 0;
		fTempPerm1342 = 0;
		fTempPerm1343 = 0;
		fTempPerm1344 = 0;
		fTempPerm1345 = 0;
		fTempPerm1346 = 0;
		fTempPerm1347 = 0;
		fTempPerm1348 = 0;
		fTempPerm1349 = 0;
		fTempPerm1350 = 0;
		fTempPerm1351 = 0;
		fTempPerm1352 = 0;
		fTempPerm1353 = 0;
		fTempPerm1354 = 0;
		fTempPerm1355 = 0;
		fTempPerm1356 = 0;
		fTempPerm1357 = 0;
		fTempPerm1358 = 0;
		fTempPerm1359 = 0;
		fTempPerm1360 = 0;
		fTempPerm1361 = 0;
		fTempPerm1362 = 0;
		fTempPerm1363 = 0;
		fTempPerm1364 = 0;
		fTempPerm1365 = 0;
		fTempPerm1366 = 0;
		fTempPerm1367 = 0;
		fTempPerm1368 = 0;
		fTempPerm1369 = 0;
		fTempPerm1370 = 0;
		fTempPerm1371 = 0;
		fTempPerm1372 = 0;
		fTempPerm1373 = 0;
		fTempPerm1374 = 0;
		fTempPerm1375 = 0;
		fTempPerm1376 = 0;
		fTempPerm1377 = 0;
		fTempPerm1378 = 0;
		fTempPerm1379 = 0;
		fTempPerm1380 = 0;
		fTempPerm1381 = 0;
		fTempPerm1382 = 0;
		fTempPerm1383 = 0;
		fTempPerm1384 = 0;
		fTempPerm1385 = 0;
		fTempPerm1386 = 0;
		fTempPerm1387 = 0;
		fTempPerm1388 = 0;
		fTempPerm1389 = 0;
		fTempPerm1390 = 0;
		fTempPerm1391 = 0;
		fTempPerm1392 = 0;
		fTempPerm1393 = 0;
		fTempPerm1394 = 0;
		fTempPerm1395 = 0;
		fTempPerm1396 = 0;
		fTempPerm1397 = 0;
		fTempPerm1398 = 0;
		fTempPerm1399 = 0;
		fTempPerm1400 = 0;
		fTempPerm1401 = 0;
		fTempPerm1402 = 0;
		fTempPerm1403 = 0;
		fTempPerm1404 = 0;
		fTempPerm1405 = 0;
		fTempPerm1406 = 0;
		fTempPerm1407 = 0;
		fTempPerm1408 = 0;
		fTempPerm1409 = 0;
		fTempPerm1410 = 0;
		fTempPerm1411 = 0;
		fTempPerm1412 = 0;
		fTempPerm1413 = 0;
		fTempPerm1414 = 0;
		fTempPerm1415 = 0;
		fTempPerm1416 = 0;
		fTempPerm1417 = 0;
		fTempPerm1418 = 0;
		fTempPerm1419 = 0;
		fTempPerm1420 = 0;
		fTempPerm1421 = 0;
		fTempPerm1422 = 0;
		fTempPerm1423 = 0;
		fTempPerm1424 = 0;
		fTempPerm1425 = 0;
		fTempPerm1426 = 0;
		fTempPerm1427 = 0;
		fTempPerm1428 = 0;
		fTempPerm1429 = 0;
		fTempPerm1430 = 0;
		fTempPerm1431 = 0;
		fTempPerm1432 = 0;
		fTempPerm1433 = 0;
		fTempPerm1434 = 0;
		fTempPerm1435 = 0;
		fTempPerm1436 = 0;
		fTempPerm1437 = 0;
		fTempPerm1438 = 0;
		fTempPerm1439 = 0;
		fTempPerm1440 = 0;
		fTempPerm1441 = 0;
		fTempPerm1442 = 0;
		fTempPerm1443 = 0;
		fTempPerm1444 = 0;
		fTempPerm1445 = 0;
		fTempPerm1446 = 0;
		fTempPerm1447 = 0;
		fTempPerm1448 = 0;
		fTempPerm1449 = 0;
		fTempPerm1450 = 0;
		fTempPerm1451 = 0;
		fTempPerm1452 = 0;
		fTempPerm1453 = 0;
		fTempPerm1454 = 0;
		fTempPerm1455 = 0;
		fTempPerm1456 = 0;
		fTempPerm1457 = 0;
		fTempPerm1458 = 0;
		fTempPerm1459 = 0;
		fTempPerm1460 = 0;
		fTempPerm1461 = 0;
		fTempPerm1462 = 0;
		fTempPerm1463 = 0;
		fTempPerm1464 = 0;
		fTempPerm1465 = 0;
		fTempPerm1466 = 0;
		fTempPerm1467 = 0;
		fTempPerm1468 = 0;
		fTempPerm1469 = 0;
		fTempPerm1470 = 0;
		fTempPerm1471 = 0;
		fTempPerm1472 = 0;
		fTempPerm1473 = 0;
		fTempPerm1474 = 0;
		fTempPerm1475 = 0;
		fTempPerm1476 = 0;
		fTempPerm1477 = 0;
		fTempPerm1478 = 0;
		fTempPerm1479 = 0;
		fTempPerm1480 = 0;
		fTempPerm1481 = 0;
		fTempPerm1482 = 0;
		fTempPerm1483 = 0;
		fTempPerm1484 = 0;
		fTempPerm1485 = 0;
		fTempPerm1486 = 0;
		fTempPerm1487 = 0;
		fTempPerm1488 = 0;
		fTempPerm1489 = 0;
		fTempPerm1490 = 0;
		fTempPerm1491 = 0;
		fTempPerm1492 = 0;
		fTempPerm1493 = 0;
		fTempPerm1494 = 0;
		fTempPerm1495 = 0;
		fTempPerm1496 = 0;
		fTempPerm1497 = 0;
		fTempPerm1498 = 0;
		fTempPerm1499 = 0;
		fTempPerm1500 = 0;
		fTempPerm1501 = 0;
		fTempPerm1502 = 0;
		fTempPerm1503 = 0;
		fTempPerm1504 = 0;
		fTempPerm1505 = 0;
		fTempPerm1506 = 0;
		fTempPerm1507 = 0;
		fTempPerm1508 = 0;
		fTempPerm1509 = 0;
		fTempPerm1510 = 0;
		fTempPerm1511 = 0;
		fTempPerm1512 = 0;
		fTempPerm1513 = 0;
		fTempPerm1514 = 0;
		fTempPerm1515 = 0;
		fTempPerm1516 = 0;
		fTempPerm1517 = 0;
		fTempPerm1518 = 0;
		fTempPerm1519 = 0;
		fTempPerm1520 = 0;
		fTempPerm1521 = 0;
		fTempPerm1522 = 0;
		fTempPerm1523 = 0;
		fTempPerm1524 = 0;
		fTempPerm1525 = 0;
		fTempPerm1526 = 0;
		fTempPerm1527 = 0;
		fTempPerm1528 = 0;
		fTempPerm1529 = 0;
		fTempPerm1530 = 0;
		fTempPerm1531 = 0;
		fTempPerm1532 = 0;
		fTempPerm1533 = 0;
		fTempPerm1534 = 0;
		fTempPerm1535 = 0;
		fTempPerm1536 = 0;
		fTempPerm1537 = 0;
		fTempPerm1538 = 0;
		fTempPerm1539 = 0;
		fTempPerm1540 = 0;
		fTempPerm1541 = 0;
		fTempPerm1542 = 0;
		fTempPerm1543 = 0;
		fTempPerm1544 = 0;
		fTempPerm1545 = 0;
		fTempPerm1546 = 0;
		fTempPerm1547 = 0;
		fTempPerm1548 = 0;
		fTempPerm1549 = 0;
		fTempPerm1550 = 0;
		fTempPerm1551 = 0;
		fTempPerm1552 = 0;
		fTempPerm1553 = 0;
		fTempPerm1554 = 0;
		fTempPerm1555 = 0;
		fTempPerm1556 = 0;
		fTempPerm1557 = 0;
		fTempPerm1558 = 0;
		fTempPerm1559 = 0;
		fTempPerm1560 = 0;
		fTempPerm1561 = 0;
		fTempPerm1562 = 0;
		fTempPerm1563 = 0;
		fTempPerm1564 = 0;
		fTempPerm1565 = 0;
		fTempPerm1566 = 0;
		fTempPerm1567 = 0;
		fTempPerm1568 = 0;
		fTempPerm1569 = 0;
		fTempPerm1570 = 0;
		fTempPerm1571 = 0;
		fTempPerm1572 = 0;
		fTempPerm1573 = 0;
		fTempPerm1574 = 0;
		fTempPerm1575 = 0;
		fTempPerm1576 = 0;
		fTempPerm1577 = 0;
		fTempPerm1578 = 0;
		fTempPerm1579 = 0;
		fTempPerm1580 = 0;
		fTempPerm1581 = 0;
		fTempPerm1582 = 0;
		fTempPerm1583 = 0;
		fTempPerm1584 = 0;
		fTempPerm1585 = 0;
		fTempPerm1586 = 0;
		fTempPerm1587 = 0;
		fTempPerm1588 = 0;
		fTempPerm1589 = 0;
		fTempPerm1590 = 0;
		fTempPerm1591 = 0;
		fTempPerm1592 = 0;
		fTempPerm1593 = 0;
		fTempPerm1594 = 0;
		fTempPerm1595 = 0;
		fTempPerm1596 = 0;
		fTempPerm1597 = 0;
		fTempPerm1598 = 0;
		fTempPerm1599 = 0;
		fTempPerm1600 = 0;
		fTempPerm1601 = 0;
		fTempPerm1602 = 0;
		fTempPerm1603 = 0;
		fTempPerm1604 = 0;
		fTempPerm1605 = 0;
		fTempPerm1606 = 0;
		fTempPerm1607 = 0;
		fTempPerm1608 = 0;
		fTempPerm1609 = 0;
		fTempPerm1610 = 0;
		fTempPerm1611 = 0;
		fTempPerm1612 = 0;
		fTempPerm1613 = 0;
		fTempPerm1614 = 0;
		fTempPerm1615 = 0;
		fTempPerm1616 = 0;
		fTempPerm1617 = 0;
		fTempPerm1618 = 0;
		fTempPerm1619 = 0;
		fTempPerm1620 = 0;
		fTempPerm1621 = 0;
		fTempPerm1622 = 0;
		fTempPerm1623 = 0;
		fTempPerm1624 = 0;
		fTempPerm1625 = 0;
		fTempPerm1626 = 0;
		fTempPerm1627 = 0;
		fTempPerm1628 = 0;
		fTempPerm1629 = 0;
		fTempPerm1630 = 0;
		fTempPerm1631 = 0;
		fTempPerm1632 = 0;
		fTempPerm1633 = 0;
		fTempPerm1634 = 0;
		fTempPerm1635 = 0;
		fTempPerm1636 = 0;
		fTempPerm1637 = 0;
		fTempPerm1638 = 0;
		fTempPerm1639 = 0;
		fTempPerm1640 = 0;
		fTempPerm1641 = 0;
		fTempPerm1642 = 0;
		fTempPerm1643 = 0;
		fTempPerm1644 = 0;
		fTempPerm1645 = 0;
		fTempPerm1646 = 0;
		fTempPerm1647 = 0;
		fTempPerm1648 = 0;
		fTempPerm1649 = 0;
		fTempPerm1650 = 0;
		fTempPerm1651 = 0;
		fTempPerm1652 = 0;
		fTempPerm1653 = 0;
		fTempPerm1654 = 0;
		fTempPerm1655 = 0;
		fTempPerm1656 = 0;
		fTempPerm1657 = 0;
		fTempPerm1658 = 0;
		fTempPerm1659 = 0;
		fTempPerm1660 = 0;
		fTempPerm1661 = 0;
		fTempPerm1662 = 0;
		fTempPerm1663 = 0;
		fTempPerm1664 = 0;
		fTempPerm1665 = 0;
		fTempPerm1666 = 0;
		fTempPerm1667 = 0;
		fTempPerm1668 = 0;
		fTempPerm1669 = 0;
		fTempPerm1670 = 0;
		fTempPerm1671 = 0;
		fTempPerm1672 = 0;
		fTempPerm1673 = 0;
		fTempPerm1674 = 0;
		fTempPerm1675 = 0;
		fTempPerm1676 = 0;
		fTempPerm1677 = 0;
		fTempPerm1678 = 0;
		fTempPerm1679 = 0;
		fTempPerm1680 = 0;
		fTempPerm1681 = 0;
		fTempPerm1682 = 0;
		fTempPerm1683 = 0;
		fTempPerm1684 = 0;
		fTempPerm1685 = 0;
		fTempPerm1686 = 0;
		fTempPerm1687 = 0;
		fTempPerm1688 = 0;
		fTempPerm1689 = 0;
		fTempPerm1690 = 0;
		fTempPerm1691 = 0;
		fTempPerm1692 = 0;
		fTempPerm1693 = 0;
		fTempPerm1694 = 0;
		fTempPerm1695 = 0;
		fTempPerm1696 = 0;
		fTempPerm1697 = 0;
		fTempPerm1698 = 0;
		fTempPerm1699 = 0;
		fTempPerm1700 = 0;
		fTempPerm1701 = 0;
		fTempPerm1702 = 0;
		fTempPerm1703 = 0;
		fTempPerm1704 = 0;
		fTempPerm1705 = 0;
		fTempPerm1706 = 0;
		fTempPerm1707 = 0;
		fTempPerm1708 = 0;
		fTempPerm1709 = 0;
		fTempPerm1710 = 0;
		fTempPerm1711 = 0;
		fTempPerm1712 = 0;
		fTempPerm1713 = 0;
		fTempPerm1714 = 0;
		fTempPerm1715 = 0;
		fTempPerm1716 = 0;
		fTempPerm1717 = 0;
		fTempPerm1718 = 0;
		fTempPerm1719 = 0;
		fTempPerm1720 = 0;
		fTempPerm1721 = 0;
		fTempPerm1722 = 0;
		fTempPerm1723 = 0;
		fTempPerm1724 = 0;
		fTempPerm1725 = 0;
		fTempPerm1726 = 0;
		fTempPerm1727 = 0;
		fTempPerm1728 = 0;
		fTempPerm1729 = 0;
		fTempPerm1730 = 0;
		fTempPerm1731 = 0;
		fTempPerm1732 = 0;
		fTempPerm1733 = 0;
		fTempPerm1734 = 0;
		fTempPerm1735 = 0;
		fTempPerm1736 = 0;
		fTempPerm1737 = 0;
		fTempPerm1738 = 0;
		fTempPerm1739 = 0;
		fTempPerm1740 = 0;
		fTempPerm1741 = 0;
		fTempPerm1742 = 0;
		fTempPerm1743 = 0;
		fTempPerm1744 = 0;
		fTempPerm1745 = 0;
		fTempPerm1746 = 0;
		fTempPerm1747 = 0;
		fTempPerm1748 = 0;
		fTempPerm1749 = 0;
		fTempPerm1750 = 0;
		fTempPerm1751 = 0;
		fTempPerm1752 = 0;
		fTempPerm1753 = 0;
		fTempPerm1754 = 0;
		fTempPerm1755 = 0;
		fTempPerm1756 = 0;
		fTempPerm1757 = 0;
		fTempPerm1758 = 0;
		fTempPerm1759 = 0;
		fTempPerm1760 = 0;
		fTempPerm1761 = 0;
		fTempPerm1762 = 0;
		fTempPerm1763 = 0;
		fTempPerm1764 = 0;
		fTempPerm1765 = 0;
		fTempPerm1766 = 0;
		fTempPerm1767 = 0;
		fTempPerm1768 = 0;
		fTempPerm1769 = 0;
		fTempPerm1770 = 0;
		fTempPerm1771 = 0;
		fTempPerm1772 = 0;
		fTempPerm1773 = 0;
		fTempPerm1774 = 0;
		fTempPerm1775 = 0;
		fTempPerm1776 = 0;
		fTempPerm1777 = 0;
		fTempPerm1778 = 0;
		fTempPerm1779 = 0;
		fTempPerm1780 = 0;
		fTempPerm1781 = 0;
		fTempPerm1782 = 0;
		fTempPerm1783 = 0;
		fTempPerm1784 = 0;
		fTempPerm1785 = 0;
		fTempPerm1786 = 0;
		fTempPerm1787 = 0;
		fTempPerm1788 = 0;
		fTempPerm1789 = 0;
		fTempPerm1790 = 0;
		fTempPerm1791 = 0;
		fTempPerm1792 = 0;
		fTempPerm1793 = 0;
		fTempPerm1794 = 0;
		fTempPerm1795 = 0;
		fTempPerm1796 = 0;
		fTempPerm1797 = 0;
		fTempPerm1798 = 0;
		fTempPerm1799 = 0;
		fTempPerm1800 = 0;
		fTempPerm1801 = 0;
		fTempPerm1802 = 0;
		fTempPerm1803 = 0;
		fTempPerm1804 = 0;
		fTempPerm1805 = 0;
		fTempPerm1806 = 0;
		fTempPerm1807 = 0;
		fTempPerm1808 = 0;
		fTempPerm1809 = 0;
		fTempPerm1810 = 0;
		fTempPerm1811 = 0;
		fTempPerm1812 = 0;
		fTempPerm1813 = 0;
		fTempPerm1814 = 0;
		fTempPerm1815 = 0;
		fTempPerm1816 = 0;
		fTempPerm1817 = 0;
		fTempPerm1818 = 0;
		fTempPerm1819 = 0;
		fTempPerm1820 = 0;
		fTempPerm1821 = 0;
		fTempPerm1822 = 0;
		fTempPerm1823 = 0;
		fTempPerm1824 = 0;
		fTempPerm1825 = 0;
		fTempPerm1826 = 0;
		fTempPerm1827 = 0;
		fTempPerm1828 = 0;
		fTempPerm1829 = 0;
		fTempPerm1830 = 0;
		fTempPerm1831 = 0;
		fTempPerm1832 = 0;
		fTempPerm1833 = 0;
		fTempPerm1834 = 0;
		fTempPerm1835 = 0;
		fTempPerm1836 = 0;
		fTempPerm1837 = 0;
		fTempPerm1838 = 0;
		fTempPerm1839 = 0;
		fTempPerm1840 = 0;
		fTempPerm1841 = 0;
		fTempPerm1842 = 0;
		fTempPerm1843 = 0;
		fTempPerm1844 = 0;
		fTempPerm1845 = 0;
		fTempPerm1846 = 0;
		fTempPerm1847 = 0;
		fTempPerm1848 = 0;
		fTempPerm1849 = 0;
		fTempPerm1850 = 0;
		fTempPerm1851 = 0;
		fTempPerm1852 = 0;
		fTempPerm1853 = 0;
		fTempPerm1854 = 0;
		fTempPerm1855 = 0;
		fTempPerm1856 = 0;
		fTempPerm1857 = 0;
		fTempPerm1858 = 0;
		fTempPerm1859 = 0;
		fTempPerm1860 = 0;
		fTempPerm1861 = 0;
		fTempPerm1862 = 0;
		fTempPerm1863 = 0;
		fTempPerm1864 = 0;
		fTempPerm1865 = 0;
		fTempPerm1866 = 0;
		fTempPerm1867 = 0;
		fTempPerm1868 = 0;
		fTempPerm1869 = 0;
		fTempPerm1870 = 0;
		fTempPerm1871 = 0;
		fTempPerm1872 = 0;
		fTempPerm1873 = 0;
		fTempPerm1874 = 0;
		fTempPerm1875 = 0;
		fTempPerm1876 = 0;
		fTempPerm1877 = 0;
		fTempPerm1878 = 0;
		fTempPerm1879 = 0;
		fTempPerm1880 = 0;
		fTempPerm1881 = 0;
		fTempPerm1882 = 0;
		fTempPerm1883 = 0;
		fTempPerm1884 = 0;
		fTempPerm1885 = 0;
		fTempPerm1886 = 0;
		fTempPerm1887 = 0;
		fTempPerm1888 = 0;
		fTempPerm1889 = 0;
		fTempPerm1890 = 0;
		fTempPerm1891 = 0;
		fTempPerm1892 = 0;
		fTempPerm1893 = 0;
		fTempPerm1894 = 0;
		fTempPerm1895 = 0;
		fTempPerm1896 = 0;
		fTempPerm1897 = 0;
		fTempPerm1898 = 0;
		fTempPerm1899 = 0;
		fTempPerm1900 = 0;
		fTempPerm1901 = 0;
		fTempPerm1902 = 0;
		fTempPerm1903 = 0;
		fTempPerm1904 = 0;
		fTempPerm1905 = 0;
		fTempPerm1906 = 0;
		fTempPerm1907 = 0;
		fTempPerm1908 = 0;
		fTempPerm1909 = 0;
		fTempPerm1910 = 0;
		fTempPerm1911 = 0;
		fTempPerm1912 = 0;
		fTempPerm1913 = 0;
		fTempPerm1914 = 0;
		fTempPerm1915 = 0;
		fTempPerm1916 = 0;
		fTempPerm1917 = 0;
		fTempPerm1918 = 0;
		fTempPerm1919 = 0;
		fTempPerm1920 = 0;
		fTempPerm1921 = 0;
		fTempPerm1922 = 0;
		fTempPerm1923 = 0;
		fTempPerm1924 = 0;
		fTempPerm1925 = 0;
		fTempPerm1926 = 0;
		fTempPerm1927 = 0;
		fTempPerm1928 = 0;
		fTempPerm1929 = 0;
		fTempPerm1930 = 0;
		fTempPerm1931 = 0;
		fTempPerm1932 = 0;
		fTempPerm1933 = 0;
		fTempPerm1934 = 0;
		fTempPerm1935 = 0;
		fTempPerm1936 = 0;
		fTempPerm1937 = 0;
		fTempPerm1938 = 0;
		fTempPerm1939 = 0;
		fTempPerm1940 = 0;
		fTempPerm1941 = 0;
		fTempPerm1942 = 0;
		fTempPerm1943 = 0;
		fTempPerm1944 = 0;
		fTempPerm1945 = 0;
		fTempPerm1946 = 0;
		fTempPerm1947 = 0;
		fTempPerm1948 = 0;
		fTempPerm1949 = 0;
		fTempPerm1950 = 0;
		fTempPerm1951 = 0;
		fTempPerm1952 = 0;
		fTempPerm1953 = 0;
		fTempPerm1954 = 0;
		fTempPerm1955 = 0;
		fTempPerm1956 = 0;
		fTempPerm1957 = 0;
		fTempPerm1958 = 0;
		fTempPerm1959 = 0;
		fTempPerm1960 = 0;
		fTempPerm1961 = 0;
		fTempPerm1962 = 0;
		fTempPerm1963 = 0;
		fTempPerm1964 = 0;
		fTempPerm1965 = 0;
		fTempPerm1966 = 0;
		fTempPerm1967 = 0;
		fTempPerm1968 = 0;
		fTempPerm1969 = 0;
		fTempPerm1970 = 0;
		fTempPerm1971 = 0;
		fTempPerm1972 = 0;
		fTempPerm1973 = 0;
		fTempPerm1974 = 0;
		fTempPerm1975 = 0;
		fTempPerm1976 = 0;
		fTempPerm1977 = 0;
		fTempPerm1978 = 0;
		fTempPerm1979 = 0;
		fTempPerm1980 = 0;
		fTempPerm1981 = 0;
		fTempPerm1982 = 0;
		fTempPerm1983 = 0;
		fTempPerm1984 = 0;
		fTempPerm1985 = 0;
		fTempPerm1986 = 0;
		fTempPerm1987 = 0;
		fTempPerm1988 = 0;
		fTempPerm1989 = 0;
		fTempPerm1990 = 0;
		fTempPerm1991 = 0;
		fTempPerm1992 = 0;
		fTempPerm1993 = 0;
		fTempPerm1994 = 0;
		fTempPerm1995 = 0;
		fTempPerm1996 = 0;
		fTempPerm1997 = 0;
		fTempPerm1998 = 0;
		fTempPerm1999 = 0;
		fTempPerm2000 = 0;
		fTempPerm2001 = 0;
		fTempPerm2002 = 0;
		fTempPerm2003 = 0;
		fTempPerm2004 = 0;
		fTempPerm2005 = 0;
		fTempPerm2006 = 0;
		fTempPerm2007 = 0;
		fTempPerm2008 = 0;
		fTempPerm2009 = 0;
		fTempPerm2010 = 0;
		fTempPerm2011 = 0;
		fTempPerm2012 = 0;
		fTempPerm2013 = 0;
		fTempPerm2014 = 0;
		fTempPerm2015 = 0;
		fTempPerm2016 = 0;
		fTempPerm2017 = 0;
		fTempPerm2018 = 0;
		fTempPerm2019 = 0;
		fTempPerm2020 = 0;
		fTempPerm2021 = 0;
		fTempPerm2022 = 0;
		fTempPerm2023 = 0;
		fTempPerm2024 = 0;
		fTempPerm2025 = 0;
		fTempPerm2026 = 0;
		fTempPerm2027 = 0;
		fTempPerm2028 = 0;
		fTempPerm2029 = 0;
		fTempPerm2030 = 0;
		fTempPerm2031 = 0;
		fTempPerm2032 = 0;
		fTempPerm2033 = 0;
		fTempPerm2034 = 0;
		fTempPerm2035 = 0;
		fTempPerm2036 = 0;
		fTempPerm2037 = 0;
		fTempPerm2038 = 0;
		fTempPerm2039 = 0;
		fTempPerm2040 = 0;
		fTempPerm2041 = 0;
		fTempPerm2042 = 0;
		fTempPerm2043 = 0;
		fTempPerm2044 = 0;
		fTempPerm2045 = 0;
		fTempPerm2046 = 0;
		fTempPerm2047 = 0;
		fTempPerm2048 = 0;
		fTempPerm2049 = 0;
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
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<128; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<128; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<128; i++) fVec11[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<128; i++) fVec12[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<128; i++) fVec13[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<128; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<128; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
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
		ui_interface->openVerticalBox("HOADecoderLebedev50Binaural3");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad111186b0", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad11107370", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad110a8080", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad110a3410", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad1109ee20", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad11085370", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad1107f120", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad11079510", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad11074f80", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad110710d0", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad10cf2310", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad10cee050", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad10cd5e60", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad10cd0e20", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad10ccae30", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fad10cc6300", &fbargraph0, -7e+01, 6.0);
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
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addHorizontalBargraph("0x7fad10eaba10", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Right");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addHorizontalBargraph("0x7fad10f28cb0", &fbargraph17, -7e+01, 6.0);
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
		FAUSTFLOAT* input9 = input[9];
		FAUSTFLOAT* input10 = input[10];
		FAUSTFLOAT* input11 = input[11];
		FAUSTFLOAT* input12 = input[12];
		FAUSTFLOAT* input13 = input[13];
		FAUSTFLOAT* input14 = input[14];
		FAUSTFLOAT* input15 = input[15];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		//LoopGraphScalar
		for (int i=0; i<count; i++) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			fRec3[0] = (fSlow1 + (0.999 * fRec3[1]));
			fTempPerm0 = ((double)input15[i] * fRec3[0]);
			fRec2[0] = max((fRec2[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm0))))));
			fbargraph0 = fRec2[0];
			fTempPerm1 = fTempPerm0;
			fVec0[IOTA&127] = fTempPerm1;
			fTempPerm2 = fVec0[(IOTA-125)&127];
			fTempPerm3 = fVec0[(IOTA-123)&127];
			fTempPerm4 = fVec0[(IOTA-100)&127];
			fTempPerm5 = fVec0[(IOTA-93)&127];
			fTempPerm6 = fVec0[(IOTA-71)&127];
			fTempPerm7 = fVec0[(IOTA-63)&127];
			fTempPerm8 = fVec0[(IOTA-54)&127];
			fTempPerm9 = fVec0[(IOTA-53)&127];
			fTempPerm10 = fVec0[(IOTA-62)&127];
			fTempPerm11 = fVec0[(IOTA-52)&127];
			fTempPerm12 = fVec0[(IOTA-51)&127];
			fTempPerm13 = fVec0[(IOTA-39)&127];
			fTempPerm14 = ((double)input14[i] * fRec3[0]);
			fRec4[0] = max((fRec4[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm14))))));
			fbargraph1 = fRec4[0];
			fTempPerm15 = fTempPerm14;
			fVec1[IOTA&127] = fTempPerm15;
			fTempPerm16 = fVec1[(IOTA-106)&127];
			fTempPerm17 = fVec1[(IOTA-103)&127];
			fTempPerm18 = fVec1[(IOTA-101)&127];
			fTempPerm19 = fVec1[(IOTA-93)&127];
			fTempPerm20 = fVec1[(IOTA-76)&127];
			fTempPerm21 = fVec1[(IOTA-68)&127];
			fTempPerm22 = fVec1[(IOTA-67)&127];
			fTempPerm23 = fVec1[(IOTA-80)&127];
			fTempPerm24 = fVec1[(IOTA-65)&127];
			fTempPerm25 = fVec1[(IOTA-54)&127];
			fTempPerm26 = fVec1[(IOTA-42)&127];
			fTempPerm27 = fVec1[(IOTA-39)&127];
			fTempPerm28 = fVec1[(IOTA-36)&127];
			fTempPerm29 = fVec1[(IOTA-35)&127];
			fTempPerm30 = fVec1[(IOTA-12)&127];
			fTempPerm31 = fVec1[(IOTA-9)&127];
			fTempPerm32 = fVec1[(IOTA-7)&127];
			fTempPerm33 = fVec1[(IOTA-5)&127];
			fTempPerm34 = ((double)input13[i] * fRec3[0]);
			fRec5[0] = max((fRec5[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm34))))));
			fbargraph2 = fRec5[0];
			fTempPerm35 = fTempPerm34;
			fVec2[IOTA&127] = fTempPerm35;
			fTempPerm36 = fVec2[(IOTA-115)&127];
			fTempPerm37 = fVec2[(IOTA-114)&127];
			fTempPerm38 = fVec2[(IOTA-113)&127];
			fTempPerm39 = fVec2[(IOTA-112)&127];
			fTempPerm40 = fVec2[(IOTA-106)&127];
			fTempPerm41 = fVec2[(IOTA-105)&127];
			fTempPerm42 = fVec2[(IOTA-104)&127];
			fTempPerm43 = fVec2[(IOTA-108)&127];
			fTempPerm44 = fVec2[(IOTA-98)&127];
			fTempPerm45 = fVec2[(IOTA-97)&127];
			fTempPerm46 = fVec2[(IOTA-83)&127];
			fTempPerm47 = fVec2[(IOTA-64)&127];
			fTempPerm48 = fVec2[(IOTA-34)&127];
			fTempPerm49 = fVec2[(IOTA-39)&127];
			fTempPerm50 = ((double)input12[i] * fRec3[0]);
			fRec6[0] = max((fRec6[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm50))))));
			fbargraph3 = fRec6[0];
			fTempPerm51 = fTempPerm50;
			fVec3[IOTA&127] = fTempPerm51;
			fTempPerm52 = fVec3[(IOTA-104)&127];
			fTempPerm53 = fVec3[(IOTA-103)&127];
			fTempPerm54 = fVec3[(IOTA-92)&127];
			fTempPerm55 = fVec0[(IOTA-126)&127];
			fTempPerm56 = fVec0[(IOTA-120)&127];
			fTempPerm57 = fVec0[(IOTA-95)&127];
			fTempPerm58 = fVec0[(IOTA-96)&127];
			fTempPerm59 = fVec0[(IOTA-94)&127];
			fTempPerm60 = fVec0[(IOTA-80)&127];
			fTempPerm61 = fVec0[(IOTA-66)&127];
			fTempPerm62 = fVec0[(IOTA-47)&127];
			fTempPerm63 = fVec0[(IOTA-42)&127];
			fTempPerm64 = fVec0[(IOTA-41)&127];
			fTempPerm65 = fVec1[(IOTA-113)&127];
			fTempPerm66 = fVec1[(IOTA-95)&127];
			fTempPerm67 = fVec1[(IOTA-91)&127];
			fTempPerm68 = fVec1[(IOTA-89)&127];
			fTempPerm69 = fVec1[(IOTA-79)&127];
			fTempPerm70 = fVec1[(IOTA-60)&127];
			fTempPerm71 = fVec1[(IOTA-59)&127];
			fTempPerm72 = fVec1[(IOTA-56)&127];
			fTempPerm73 = fVec1[(IOTA-55)&127];
			fTempPerm74 = fVec1[(IOTA-32)&127];
			fTempPerm75 = fVec2[(IOTA-28)&127];
			fTempPerm76 = fVec2[(IOTA-27)&127];
			fTempPerm77 = fVec2[(IOTA-24)&127];
			fTempPerm78 = fVec2[(IOTA-103)&127];
			fTempPerm79 = fVec2[(IOTA-95)&127];
			fTempPerm80 = fVec2[(IOTA-92)&127];
			fTempPerm81 = fVec2[(IOTA-109)&127];
			fTempPerm82 = fVec2[(IOTA-71)&127];
			fTempPerm83 = fVec2[(IOTA-48)&127];
			fTempPerm84 = fVec2[(IOTA-70)&127];
			fTempPerm85 = fVec2[(IOTA-69)&127];
			fTempPerm86 = fVec2[(IOTA-47)&127];
			fTempPerm87 = fVec2[(IOTA-46)&127];
			fTempPerm88 = fVec2[(IOTA-42)&127];
			fTempPerm89 = fVec2[(IOTA-36)&127];
			fTempPerm90 = fVec0[(IOTA-122)&127];
			fTempPerm91 = fVec0[(IOTA-73)&127];
			fTempPerm92 = fVec0[(IOTA-72)&127];
			fTempPerm93 = fVec0[(IOTA-91)&127];
			fTempPerm94 = fVec0[(IOTA-84)&127];
			fTempPerm95 = fVec0[(IOTA-61)&127];
			fTempPerm96 = fVec0[(IOTA-49)&127];
			fTempPerm97 = fVec0[(IOTA-60)&127];
			fTempPerm98 = fVec0[(IOTA-30)&127];
			fTempPerm99 = fVec0[(IOTA-29)&127];
			fTempPerm100 = fVec0[(IOTA-23)&127];
			fTempPerm101 = fVec1[(IOTA-122)&127];
			fTempPerm102 = fVec1[(IOTA-121)&127];
			fTempPerm103 = fVec1[(IOTA-94)&127];
			fTempPerm104 = fVec1[(IOTA-100)&127];
			fTempPerm105 = fVec1[(IOTA-88)&127];
			fTempPerm106 = fVec2[(IOTA-111)&127];
			fTempPerm107 = fVec2[(IOTA-5)&127];
			fTempPerm108 = fVec2[(IOTA-3)&127];
			fTempPerm109 = fVec1[(IOTA-3)&127];
			fTempPerm110 = fVec2[(IOTA-102)&127];
			fTempPerm111 = fVec2[(IOTA-65)&127];
			fTempPerm112 = fVec2[(IOTA-41)&127];
			fTempPerm113 = fVec2[(IOTA-33)&127];
			fTempPerm114 = fVec2[(IOTA-32)&127];
			fTempPerm115 = fVec2[(IOTA-19)&127];
			fTempPerm116 = fVec2[(IOTA-14)&127];
			fTempPerm117 = fVec2[(IOTA-12)&127];
			fTempPerm118 = fVec2[(IOTA-11)&127];
			fTempPerm119 = fVec2[(IOTA-10)&127];
			fTempPerm120 = fVec3[(IOTA-109)&127];
			fTempPerm121 = fVec3[(IOTA-108)&127];
			fTempPerm122 = fVec2[(IOTA-9)&127];
			fTempPerm123 = fVec2[(IOTA-7)&127];
			fTempPerm124 = fVec3[(IOTA-107)&127];
			fTempPerm125 = fVec3[(IOTA-106)&127];
			fTempPerm126 = fVec3[(IOTA-56)&127];
			fTempPerm127 = fVec3[(IOTA-51)&127];
			fTempPerm128 = fVec3[(IOTA-52)&127];
			fTempPerm129 = fVec3[(IOTA-46)&127];
			fTempPerm130 = fVec3[(IOTA-40)&127];
			fTempPerm131 = fVec3[(IOTA-38)&127];
			fTempPerm132 = fVec3[(IOTA-27)&127];
			fTempPerm133 = fVec3[(IOTA-26)&127];
			fTempPerm134 = fVec3[(IOTA-22)&127];
			fTempPerm135 = fVec3[(IOTA-21)&127];
			fTempPerm136 = fVec3[(IOTA-34)&127];
			fTempPerm137 = fVec0[(IOTA-16)&127];
			fTempPerm138 = fVec0[(IOTA-12)&127];
			fTempPerm139 = fVec0[(IOTA-11)&127];
			fTempPerm140 = fVec0[(IOTA-9)&127];
			fTempPerm141 = fVec0[(IOTA-7)&127];
			fTempPerm142 = fVec0[(IOTA-5)&127];
			fTempPerm143 = fVec0[(IOTA-3)&127];
			fTempPerm144 = fVec0[(IOTA-121)&127];
			fTempPerm145 = fVec0[(IOTA-124)&127];
			fTempPerm146 = fVec0[(IOTA-92)&127];
			fTempPerm147 = fVec0[(IOTA-99)&127];
			fTempPerm148 = fVec0[(IOTA-79)&127];
			fTempPerm149 = fVec0[(IOTA-104)&127];
			fTempPerm150 = fVec0[(IOTA-86)&127];
			fTempPerm151 = fVec0[(IOTA-57)&127];
			fTempPerm152 = fVec0[(IOTA-50)&127];
			fTempPerm153 = fVec0[(IOTA-48)&127];
			fTempPerm154 = fVec0[(IOTA-26)&127];
			fTempPerm155 = fVec0[(IOTA-22)&127];
			fTempPerm156 = fVec0[(IOTA-21)&127];
			fTempPerm157 = fVec0[(IOTA-19)&127];
			fTempPerm158 = fVec0[(IOTA-20)&127];
			fTempPerm159 = fVec1[(IOTA-118)&127];
			fTempPerm160 = fVec1[(IOTA-126)&127];
			fTempPerm161 = fVec1[(IOTA-105)&127];
			fTempPerm162 = fVec1[(IOTA-92)&127];
			fTempPerm163 = fVec1[(IOTA-98)&127];
			fTempPerm164 = fVec1[(IOTA-64)&127];
			fTempPerm165 = fVec1[(IOTA-63)&127];
			fTempPerm166 = fVec1[(IOTA-58)&127];
			fTempPerm167 = fVec1[(IOTA-44)&127];
			fTempPerm168 = fVec1[(IOTA-45)&127];
			fTempPerm169 = fVec1[(IOTA-30)&127];
			fTempPerm170 = fVec1[(IOTA-31)&127];
			fTempPerm171 = fVec1[(IOTA-27)&127];
			fTempPerm172 = fVec1[(IOTA-26)&127];
			fTempPerm173 = fVec1[(IOTA-25)&127];
			fTempPerm174 = fVec1[(IOTA-23)&127];
			fTempPerm175 = fVec1[(IOTA-24)&127];
			fTempPerm176 = fVec1[(IOTA-21)&127];
			fTempPerm177 = fVec1[(IOTA-18)&127];
			fTempPerm178 = fVec1[(IOTA-15)&127];
			fTempPerm179 = fVec1[(IOTA-6)&127];
			fTempPerm180 = fVec1[(IOTA-102)&127];
			fTempPerm181 = fVec2[(IOTA-118)&127];
			fTempPerm182 = fVec2[(IOTA-110)&127];
			fTempPerm183 = fVec2[(IOTA-101)&127];
			fTempPerm184 = fVec1[(IOTA-111)&127];
			fTempPerm185 = fVec1[(IOTA-110)&127];
			fTempPerm186 = fVec1[(IOTA-85)&127];
			fTempPerm187 = fVec2[(IOTA-100)&127];
			fTempPerm188 = fVec2[(IOTA-99)&127];
			fTempPerm189 = fVec2[(IOTA-96)&127];
			fTempPerm190 = fVec2[(IOTA-72)&127];
			fTempPerm191 = fVec2[(IOTA-107)&127];
			fTempPerm192 = fVec2[(IOTA-93)&127];
			fTempPerm193 = fVec2[(IOTA-74)&127];
			fTempPerm194 = fVec2[(IOTA-73)&127];
			fTempPerm195 = fVec2[(IOTA-67)&127];
			fTempPerm196 = fVec2[(IOTA-66)&127];
			fTempPerm197 = fVec2[(IOTA-63)&127];
			fTempPerm198 = fVec2[(IOTA-45)&127];
			fTempPerm199 = fVec3[(IOTA-110)&127];
			fTempPerm200 = fVec3[(IOTA-100)&127];
			fTempPerm201 = fVec3[(IOTA-81)&127];
			fTempPerm202 = fVec3[(IOTA-101)&127];
			fTempPerm203 = fVec3[(IOTA-80)&127];
			fTempPerm204 = fVec3[(IOTA-79)&127];
			fTempPerm205 = fVec3[(IOTA-68)&127];
			fTempPerm206 = fVec3[(IOTA-67)&127];
			fTempPerm207 = fVec3[(IOTA-60)&127];
			fTempPerm208 = fVec3[(IOTA-63)&127];
			fTempPerm209 = fVec3[(IOTA-59)&127];
			fTempPerm210 = fVec3[(IOTA-58)&127];
			fTempPerm211 = fVec3[(IOTA-54)&127];
			fTempPerm212 = ((double)input11[i] * fRec3[0]);
			fRec7[0] = max((fRec7[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm212))))));
			fbargraph4 = fRec7[0];
			fTempPerm213 = fTempPerm212;
			fVec4[IOTA&127] = fTempPerm213;
			fTempPerm214 = fVec4[(IOTA-107)&127];
			fTempPerm215 = fVec4[(IOTA-103)&127];
			fTempPerm216 = fVec4[(IOTA-102)&127];
			fTempPerm217 = fVec4[(IOTA-96)&127];
			fTempPerm218 = fVec4[(IOTA-94)&127];
			fTempPerm219 = fVec4[(IOTA-85)&127];
			fTempPerm220 = fVec4[(IOTA-81)&127];
			fTempPerm221 = fVec4[(IOTA-4)&127];
			fTempPerm222 = fVec4[(IOTA-2)&127];
			fTempPerm223 = fVec4[(IOTA-8)&127];
			fTempPerm224 = fVec4[(IOTA-6)&127];
			fTempPerm225 = fVec4[(IOTA-39)&127];
			fTempPerm226 = ((double)input10[i] * fRec3[0]);
			fRec8[0] = max((fRec8[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm226))))));
			fbargraph5 = fRec8[0];
			fTempPerm227 = fTempPerm226;
			fVec5[IOTA&127] = fTempPerm227;
			fTempPerm228 = fVec5[(IOTA-119)&127];
			fTempPerm229 = fVec4[(IOTA-53)&127];
			fTempPerm230 = fVec4[(IOTA-54)&127];
			fTempPerm231 = fVec4[(IOTA-52)&127];
			fTempPerm232 = fVec5[(IOTA-115)&127];
			fTempPerm233 = fVec5[(IOTA-109)&127];
			fTempPerm234 = fVec5[(IOTA-106)&127];
			fTempPerm235 = fVec5[(IOTA-102)&127];
			fTempPerm236 = fVec5[(IOTA-104)&127];
			fTempPerm237 = fVec5[(IOTA-86)&127];
			fTempPerm238 = fVec5[(IOTA-84)&127];
			fTempPerm239 = fVec5[(IOTA-70)&127];
			fTempPerm240 = fVec5[(IOTA-71)&127];
			fTempPerm241 = fVec5[(IOTA-43)&127];
			fTempPerm242 = fVec4[(IOTA-124)&127];
			fTempPerm243 = fVec4[(IOTA-123)&127];
			fTempPerm244 = fVec4[(IOTA-119)&127];
			fTempPerm245 = fVec4[(IOTA-101)&127];
			fTempPerm246 = fVec4[(IOTA-92)&127];
			fTempPerm247 = fVec4[(IOTA-91)&127];
			fTempPerm248 = fVec4[(IOTA-84)&127];
			fTempPerm249 = fVec4[(IOTA-80)&127];
			fTempPerm250 = fVec4[(IOTA-76)&127];
			fTempPerm251 = fVec4[(IOTA-62)&127];
			fTempPerm252 = fVec4[(IOTA-61)&127];
			fTempPerm253 = fVec4[(IOTA-55)&127];
			fTempPerm254 = fVec4[(IOTA-50)&127];
			fTempPerm255 = fVec4[(IOTA-49)&127];
			fTempPerm256 = fVec4[(IOTA-46)&127];
			fTempPerm257 = fVec4[(IOTA-45)&127];
			fTempPerm258 = fVec4[(IOTA-57)&127];
			fTempPerm259 = fVec4[(IOTA-51)&127];
			fTempPerm260 = fVec4[(IOTA-44)&127];
			fTempPerm261 = fVec4[(IOTA-43)&127];
			fTempPerm262 = fVec4[(IOTA-16)&127];
			fTempPerm263 = fVec4[(IOTA-15)&127];
			fTempPerm264 = fVec4[(IOTA-10)&127];
			fTempPerm265 = fVec5[(IOTA-126)&127];
			fTempPerm266 = fVec5[(IOTA-122)&127];
			fTempPerm267 = fVec5[(IOTA-121)&127];
			fTempPerm268 = fVec5[(IOTA-120)&127];
			fTempPerm269 = fVec5[(IOTA-116)&127];
			fTempPerm270 = fVec5[(IOTA-103)&127];
			fTempPerm271 = fVec5[(IOTA-101)&127];
			fTempPerm272 = fVec5[(IOTA-96)&127];
			fTempPerm273 = fVec5[(IOTA-90)&127];
			fTempPerm274 = fVec5[(IOTA-79)&127];
			fTempPerm275 = fVec5[(IOTA-68)&127];
			fTempPerm276 = fVec5[(IOTA-67)&127];
			fTempPerm277 = fVec5[(IOTA-55)&127];
			fTempPerm278 = fVec5[(IOTA-54)&127];
			fTempPerm279 = fVec5[(IOTA-42)&127];
			fTempPerm280 = fVec5[(IOTA-44)&127];
			fTempPerm281 = fVec5[(IOTA-38)&127];
			fTempPerm282 = fVec5[(IOTA-37)&127];
			fTempPerm283 = fVec5[(IOTA-33)&127];
			fTempPerm284 = fVec5[(IOTA-32)&127];
			fTempPerm285 = fVec5[(IOTA-31)&127];
			fTempPerm286 = fVec5[(IOTA-26)&127];
			fTempPerm287 = fVec5[(IOTA-25)&127];
			fTempPerm288 = fVec5[(IOTA-21)&127];
			fTempPerm289 = fVec5[(IOTA-18)&127];
			fTempPerm290 = fVec5[(IOTA-19)&127];
			fTempPerm291 = fVec5[(IOTA-15)&127];
			fTempPerm292 = fVec5[(IOTA-12)&127];
			fTempPerm293 = fVec5[(IOTA-11)&127];
			fTempPerm294 = fVec5[(IOTA-10)&127];
			fTempPerm295 = fVec5[(IOTA-9)&127];
			fTempPerm296 = fVec5[(IOTA-7)&127];
			fTempPerm297 = fVec5[(IOTA-6)&127];
			fTempPerm298 = fVec5[(IOTA-3)&127];
			fTempPerm299 = fVec5[(IOTA-2)&127];
			fTempPerm300 = ((double)input9[i] * fRec3[0]);
			fRec9[0] = max((fRec9[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm300))))));
			fbargraph6 = fRec9[0];
			fTempPerm301 = fTempPerm300;
			fVec6[IOTA&127] = fTempPerm301;
			fTempPerm302 = fVec6[(IOTA-117)&127];
			fTempPerm303 = fVec6[(IOTA-105)&127];
			fTempPerm304 = fVec6[(IOTA-104)&127];
			fTempPerm305 = fVec6[(IOTA-116)&127];
			fTempPerm306 = fVec6[(IOTA-94)&127];
			fTempPerm307 = fVec6[(IOTA-98)&127];
			fTempPerm308 = fVec6[(IOTA-83)&127];
			fTempPerm309 = fVec6[(IOTA-77)&127];
			fTempPerm310 = fVec6[(IOTA-64)&127];
			fTempPerm311 = fVec6[(IOTA-41)&127];
			fTempPerm312 = fVec6[(IOTA-52)&127];
			fTempPerm313 = ((double)input8[i] * fRec3[0]);
			fRec10[0] = max((fRec10[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm313))))));
			fbargraph7 = fRec10[0];
			fTempPerm314 = fTempPerm313;
			fVec7[IOTA&127] = fTempPerm314;
			fTempPerm315 = fVec7[(IOTA-126)&127];
			fTempPerm316 = fVec7[(IOTA-86)&127];
			fTempPerm317 = fVec7[(IOTA-77)&127];
			fTempPerm318 = fVec7[(IOTA-76)&127];
			fTempPerm319 = fVec7[(IOTA-75)&127];
			fTempPerm320 = fVec7[(IOTA-64)&127];
			fTempPerm321 = fVec7[(IOTA-70)&127];
			fTempPerm322 = fVec7[(IOTA-69)&127];
			fTempPerm323 = fVec7[(IOTA-71)&127];
			fTempPerm324 = fVec7[(IOTA-42)&127];
			fTempPerm325 = fVec7[(IOTA-96)&127];
			fTempPerm326 = fVec7[(IOTA-110)&127];
			fTempPerm327 = fVec7[(IOTA-103)&127];
			fTempPerm328 = ((double)input7[i] * fRec3[0]);
			fRec11[0] = max((fRec11[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm328))))));
			fbargraph8 = fRec11[0];
			fTempPerm329 = fTempPerm328;
			fVec8[IOTA&127] = fTempPerm329;
			fTempPerm330 = fVec8[(IOTA-125)&127];
			fTempPerm331 = fVec6[(IOTA-42)&127];
			fTempPerm332 = fVec6[(IOTA-40)&127];
			fTempPerm333 = fVec8[(IOTA-110)&127];
			fTempPerm334 = fVec8[(IOTA-104)&127];
			fTempPerm335 = fVec8[(IOTA-101)&127];
			fTempPerm336 = fVec8[(IOTA-86)&127];
			fTempPerm337 = fVec8[(IOTA-77)&127];
			fTempPerm338 = fVec8[(IOTA-70)&127];
			fTempPerm339 = fVec8[(IOTA-58)&127];
			fTempPerm340 = fVec8[(IOTA-41)&127];
			fTempPerm341 = fVec8[(IOTA-44)&127];
			fTempPerm342 = fVec8[(IOTA-36)&127];
			fTempPerm343 = fVec8[(IOTA-15)&127];
			fTempPerm344 = fVec8[(IOTA-12)&127];
			fTempPerm345 = fVec8[(IOTA-11)&127];
			fTempPerm346 = fVec8[(IOTA-10)&127];
			fTempPerm347 = fVec8[(IOTA-9)&127];
			fTempPerm348 = fVec8[(IOTA-7)&127];
			fTempPerm349 = fVec8[(IOTA-2)&127];
			fTempPerm350 = ((double)input6[i] * fRec3[0]);
			fRec12[0] = max((fRec12[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm350))))));
			fbargraph9 = fRec12[0];
			fTempPerm351 = fTempPerm350;
			fVec9[IOTA&127] = fTempPerm351;
			fTempPerm352 = fVec9[(IOTA-118)&127];
			fTempPerm353 = fVec9[(IOTA-100)&127];
			fTempPerm354 = fVec9[(IOTA-94)&127];
			fTempPerm355 = fVec9[(IOTA-98)&127];
			fTempPerm356 = fVec9[(IOTA-83)&127];
			fTempPerm357 = fVec9[(IOTA-88)&127];
			fTempPerm358 = fVec9[(IOTA-79)&127];
			fTempPerm359 = fVec9[(IOTA-66)&127];
			fTempPerm360 = fVec9[(IOTA-71)&127];
			fTempPerm361 = fVec9[(IOTA-65)&127];
			fTempPerm362 = fVec9[(IOTA-64)&127];
			fTempPerm363 = fVec9[(IOTA-63)&127];
			fTempPerm364 = fVec9[(IOTA-33)&127];
			fTempPerm365 = fVec9[(IOTA-26)&127];
			fTempPerm366 = fVec9[(IOTA-22)&127];
			fTempPerm367 = fVec9[(IOTA-21)&127];
			fTempPerm368 = fVec9[(IOTA-20)&127];
			fTempPerm369 = fVec9[(IOTA-19)&127];
			fTempPerm370 = fVec9[(IOTA-18)&127];
			fTempPerm371 = fVec9[(IOTA-8)&127];
			fTempPerm372 = fVec9[(IOTA-6)&127];
			fTempPerm373 = fVec9[(IOTA-2)&127];
			fTempPerm374 = ((double)input5[i] * fRec3[0]);
			fRec13[0] = max((fRec13[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm374))))));
			fbargraph10 = fRec13[0];
			fTempPerm375 = fTempPerm374;
			fVec10[IOTA&127] = fTempPerm375;
			fTempPerm376 = fVec10[(IOTA-125)&127];
			fTempPerm377 = fVec3[(IOTA-45)&127];
			fTempPerm378 = fVec3[(IOTA-57)&127];
			fTempPerm379 = fVec3[(IOTA-48)&127];
			fTempPerm380 = fVec3[(IOTA-25)&127];
			fTempPerm381 = fVec3[(IOTA-16)&127];
			fTempPerm382 = fVec3[(IOTA-15)&127];
			fTempPerm383 = fVec3[(IOTA-12)&127];
			fTempPerm384 = fVec3[(IOTA-11)&127];
			fTempPerm385 = fVec3[(IOTA-10)&127];
			fTempPerm386 = fVec3[(IOTA-9)&127];
			fTempPerm387 = fVec3[(IOTA-7)&127];
			fTempPerm388 = fVec3[(IOTA-4)&127];
			fTempPerm389 = fVec3[(IOTA-3)&127];
			fTempPerm390 = fVec2[(IOTA-119)&127];
			fTempPerm391 = fVec3[(IOTA-115)&127];
			fTempPerm392 = fVec3[(IOTA-111)&127];
			fTempPerm393 = fVec3[(IOTA-105)&127];
			fTempPerm394 = fVec3[(IOTA-99)&127];
			fTempPerm395 = fVec3[(IOTA-98)&127];
			fTempPerm396 = fVec3[(IOTA-90)&127];
			fTempPerm397 = fVec3[(IOTA-102)&127];
			fTempPerm398 = fVec3[(IOTA-97)&127];
			fTempPerm399 = fVec3[(IOTA-89)&127];
			fTempPerm400 = fVec3[(IOTA-69)&127];
			fTempPerm401 = fVec1[(IOTA-104)&127];
			fTempPerm402 = fVec1[(IOTA-99)&127];
			fTempPerm403 = fVec4[(IOTA-110)&127];
			fTempPerm404 = fVec4[(IOTA-90)&127];
			fTempPerm405 = fVec4[(IOTA-89)&127];
			fTempPerm406 = fVec4[(IOTA-30)&127];
			fTempPerm407 = fVec4[(IOTA-25)&127];
			fTempPerm408 = fVec4[(IOTA-26)&127];
			fTempPerm409 = fVec4[(IOTA-22)&127];
			fTempPerm410 = fVec4[(IOTA-19)&127];
			fTempPerm411 = fVec4[(IOTA-18)&127];
			fTempPerm412 = fVec5[(IOTA-123)&127];
			fTempPerm413 = fVec4[(IOTA-21)&127];
			fTempPerm414 = fVec4[(IOTA-20)&127];
			fTempPerm415 = fVec5[(IOTA-114)&127];
			fTempPerm416 = fVec5[(IOTA-78)&127];
			fTempPerm417 = fVec5[(IOTA-97)&127];
			fTempPerm418 = fVec5[(IOTA-65)&127];
			fTempPerm419 = fVec5[(IOTA-66)&127];
			fTempPerm420 = fVec5[(IOTA-64)&127];
			fTempPerm421 = fVec5[(IOTA-57)&127];
			fTempPerm422 = fVec6[(IOTA-118)&127];
			fTempPerm423 = fVec5[(IOTA-52)&127];
			fTempPerm424 = fVec5[(IOTA-51)&127];
			fTempPerm425 = fVec6[(IOTA-123)&127];
			fTempPerm426 = fVec6[(IOTA-75)&127];
			fTempPerm427 = fVec6[(IOTA-45)&127];
			fTempPerm428 = fVec6[(IOTA-46)&127];
			fTempPerm429 = fVec6[(IOTA-44)&127];
			fTempPerm430 = fVec6[(IOTA-43)&127];
			fTempPerm431 = fVec7[(IOTA-121)&127];
			fTempPerm432 = fVec7[(IOTA-117)&127];
			fTempPerm433 = fVec7[(IOTA-94)&127];
			fTempPerm434 = fVec7[(IOTA-74)&127];
			fTempPerm435 = fVec7[(IOTA-72)&127];
			fTempPerm436 = fVec7[(IOTA-66)&127];
			fTempPerm437 = fVec7[(IOTA-53)&127];
			fTempPerm438 = fVec7[(IOTA-65)&127];
			fTempPerm439 = fVec7[(IOTA-79)&127];
			fTempPerm440 = fVec7[(IOTA-28)&127];
			fTempPerm441 = fVec7[(IOTA-29)&127];
			fTempPerm442 = fVec7[(IOTA-27)&127];
			fTempPerm443 = fVec7[(IOTA-26)&127];
			fTempPerm444 = fVec7[(IOTA-24)&127];
			fTempPerm445 = fVec7[(IOTA-23)&127];
			fTempPerm446 = fVec7[(IOTA-22)&127];
			fTempPerm447 = fVec7[(IOTA-20)&127];
			fTempPerm448 = fVec7[(IOTA-19)&127];
			fTempPerm449 = fVec7[(IOTA-18)&127];
			fTempPerm450 = fVec7[(IOTA-10)&127];
			fTempPerm451 = fVec7[(IOTA-8)&127];
			fTempPerm452 = fVec7[(IOTA-6)&127];
			fTempPerm453 = fVec7[(IOTA-4)&127];
			fTempPerm454 = fVec7[(IOTA-2)&127];
			fTempPerm455 = fVec8[(IOTA-117)&127];
			fTempPerm456 = fVec8[(IOTA-112)&127];
			fTempPerm457 = fVec8[(IOTA-106)&127];
			fTempPerm458 = fVec8[(IOTA-103)&127];
			fTempPerm459 = fVec8[(IOTA-94)&127];
			fTempPerm460 = fVec8[(IOTA-85)&127];
			fTempPerm461 = fVec8[(IOTA-61)&127];
			fTempPerm462 = fVec8[(IOTA-35)&127];
			fTempPerm463 = fVec8[(IOTA-32)&127];
			fTempPerm464 = fVec8[(IOTA-31)&127];
			fTempPerm465 = fVec8[(IOTA-29)&127];
			fTempPerm466 = fVec8[(IOTA-25)&127];
			fTempPerm467 = fVec8[(IOTA-26)&127];
			fTempPerm468 = fVec8[(IOTA-19)&127];
			fTempPerm469 = fVec8[(IOTA-18)&127];
			fTempPerm470 = fVec9[(IOTA-126)&127];
			fTempPerm471 = fVec9[(IOTA-102)&127];
			fTempPerm472 = fVec9[(IOTA-84)&127];
			fTempPerm473 = fVec9[(IOTA-70)&127];
			fTempPerm474 = fVec9[(IOTA-52)&127];
			fTempPerm475 = fVec10[(IOTA-126)&127];
			fTempPerm476 = fVec10[(IOTA-109)&127];
			fTempPerm477 = fVec10[(IOTA-106)&127];
			fTempPerm478 = fVec10[(IOTA-102)&127];
			fTempPerm479 = fVec10[(IOTA-99)&127];
			fTempPerm480 = fVec10[(IOTA-65)&127];
			fTempPerm481 = fVec10[(IOTA-67)&127];
			fTempPerm482 = fVec10[(IOTA-52)&127];
			fTempPerm483 = fVec10[(IOTA-51)&127];
			fTempPerm484 = fVec10[(IOTA-49)&127];
			fTempPerm485 = fVec10[(IOTA-32)&127];
			fTempPerm486 = fVec10[(IOTA-29)&127];
			fTempPerm487 = fVec10[(IOTA-28)&127];
			fTempPerm488 = fVec10[(IOTA-96)&127];
			fTempPerm489 = fVec10[(IOTA-95)&127];
			fTempPerm490 = ((double)input4[i] * fRec3[0]);
			fRec14[0] = max((fRec14[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm490))))));
			fbargraph11 = fRec14[0];
			fTempPerm491 = fTempPerm490;
			fVec11[IOTA&127] = fTempPerm491;
			fTempPerm492 = fVec11[(IOTA-125)&127];
			fTempPerm493 = fVec11[(IOTA-124)&127];
			fTempPerm494 = fVec11[(IOTA-105)&127];
			fTempPerm495 = fVec11[(IOTA-104)&127];
			fTempPerm496 = fVec11[(IOTA-111)&127];
			fTempPerm497 = fVec11[(IOTA-98)&127];
			fTempPerm498 = fVec11[(IOTA-92)&127];
			fTempPerm499 = fVec11[(IOTA-87)&127];
			fTempPerm500 = fVec11[(IOTA-114)&127];
			fTempPerm501 = fVec11[(IOTA-75)&127];
			fTempPerm502 = fVec11[(IOTA-68)&127];
			fTempPerm503 = fVec11[(IOTA-66)&127];
			fTempPerm504 = fVec11[(IOTA-61)&127];
			fTempPerm505 = ((double)input3[i] * fRec3[0]);
			fRec15[0] = max((fRec15[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm505))))));
			fbargraph12 = fRec15[0];
			fTempPerm506 = fTempPerm505;
			fVec12[IOTA&127] = fTempPerm506;
			fTempPerm507 = fVec12[(IOTA-119)&127];
			fTempPerm508 = fVec12[(IOTA-126)&127];
			fTempPerm509 = fVec11[(IOTA-57)&127];
			fTempPerm510 = fVec12[(IOTA-125)&127];
			fTempPerm511 = fVec12[(IOTA-117)&127];
			fTempPerm512 = fVec12[(IOTA-90)&127];
			fTempPerm513 = fVec12[(IOTA-83)&127];
			fTempPerm514 = fVec12[(IOTA-63)&127];
			fTempPerm515 = fVec12[(IOTA-45)&127];
			fTempPerm516 = fVec12[(IOTA-42)&127];
			fTempPerm517 = fVec12[(IOTA-34)&127];
			fTempPerm518 = fVec12[(IOTA-2)&127];
			fTempPerm519 = fVec12[(IOTA-38)&127];
			fTempPerm520 = fVec12[(IOTA-39)&127];
			fTempPerm521 = fVec12[(IOTA-37)&127];
			fTempPerm522 = ((double)input2[i] * fRec3[0]);
			fRec16[0] = max((fRec16[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm522))))));
			fbargraph13 = fRec16[0];
			fTempPerm523 = fTempPerm522;
			fVec13[IOTA&127] = fTempPerm523;
			fTempPerm524 = fVec13[(IOTA-124)&127];
			fTempPerm525 = fVec13[(IOTA-119)&127];
			fTempPerm526 = fVec13[(IOTA-117)&127];
			fTempPerm527 = fVec13[(IOTA-118)&127];
			fTempPerm528 = fVec13[(IOTA-97)&127];
			fTempPerm529 = fVec13[(IOTA-86)&127];
			fTempPerm530 = fVec13[(IOTA-84)&127];
			fTempPerm531 = fVec13[(IOTA-83)&127];
			fTempPerm532 = fVec13[(IOTA-79)&127];
			fTempPerm533 = fVec13[(IOTA-78)&127];
			fTempPerm534 = fVec6[(IOTA-97)&127];
			fTempPerm535 = fVec6[(IOTA-87)&127];
			fTempPerm536 = fVec6[(IOTA-74)&127];
			fTempPerm537 = fVec6[(IOTA-78)&127];
			fTempPerm538 = fVec6[(IOTA-76)&127];
			fTempPerm539 = fVec6[(IOTA-61)&127];
			fTempPerm540 = fVec6[(IOTA-60)&127];
			fTempPerm541 = fVec6[(IOTA-35)&127];
			fTempPerm542 = fVec6[(IOTA-65)&127];
			fTempPerm543 = fVec6[(IOTA-23)&127];
			fTempPerm544 = fVec6[(IOTA-22)&127];
			fTempPerm545 = fVec6[(IOTA-20)&127];
			fTempPerm546 = fVec6[(IOTA-18)&127];
			fTempPerm547 = fVec6[(IOTA-19)&127];
			fTempPerm548 = fVec6[(IOTA-16)&127];
			fTempPerm549 = fVec6[(IOTA-15)&127];
			fTempPerm550 = fVec6[(IOTA-10)&127];
			fTempPerm551 = fVec6[(IOTA-8)&127];
			fTempPerm552 = fVec6[(IOTA-6)&127];
			fTempPerm553 = fVec6[(IOTA-4)&127];
			fTempPerm554 = fVec6[(IOTA-2)&127];
			fTempPerm555 = fVec6[(IOTA-59)&127];
			fTempPerm556 = fVec7[(IOTA-122)&127];
			fTempPerm557 = fVec7[(IOTA-95)&127];
			fTempPerm558 = fVec7[(IOTA-91)&127];
			fTempPerm559 = fVec7[(IOTA-38)&127];
			fTempPerm560 = fVec7[(IOTA-34)&127];
			fTempPerm561 = fVec7[(IOTA-33)&127];
			fTempPerm562 = fVec8[(IOTA-116)&127];
			fTempPerm563 = fVec8[(IOTA-102)&127];
			fTempPerm564 = fVec8[(IOTA-97)&127];
			fTempPerm565 = fVec8[(IOTA-105)&127];
			fTempPerm566 = fVec8[(IOTA-93)&127];
			fTempPerm567 = fVec8[(IOTA-76)&127];
			fTempPerm568 = fVec8[(IOTA-66)&127];
			fTempPerm569 = fVec8[(IOTA-63)&127];
			fTempPerm570 = fVec8[(IOTA-62)&127];
			fTempPerm571 = fVec8[(IOTA-59)&127];
			fTempPerm572 = fVec8[(IOTA-57)&127];
			fTempPerm573 = fVec8[(IOTA-38)&127];
			fTempPerm574 = fVec8[(IOTA-54)&127];
			fTempPerm575 = fVec9[(IOTA-119)&127];
			fTempPerm576 = fVec9[(IOTA-106)&127];
			fTempPerm577 = fVec9[(IOTA-105)&127];
			fTempPerm578 = fVec9[(IOTA-101)&127];
			fTempPerm579 = fVec9[(IOTA-99)&127];
			fTempPerm580 = fVec9[(IOTA-108)&127];
			fTempPerm581 = fVec9[(IOTA-107)&127];
			fTempPerm582 = fVec9[(IOTA-91)&127];
			fTempPerm583 = fVec9[(IOTA-82)&127];
			fTempPerm584 = fVec9[(IOTA-90)&127];
			fTempPerm585 = fVec9[(IOTA-72)&127];
			fTempPerm586 = fVec9[(IOTA-60)&127];
			fTempPerm587 = fVec10[(IOTA-124)&127];
			fTempPerm588 = fVec10[(IOTA-123)&127];
			fTempPerm589 = fVec10[(IOTA-94)&127];
			fTempPerm590 = fVec10[(IOTA-119)&127];
			fTempPerm591 = fVec10[(IOTA-93)&127];
			fTempPerm592 = fVec10[(IOTA-92)&127];
			fTempPerm593 = fVec9[(IOTA-36)&127];
			fTempPerm594 = fVec10[(IOTA-91)&127];
			fTempPerm595 = fVec10[(IOTA-85)&127];
			fTempPerm596 = fVec10[(IOTA-84)&127];
			fTempPerm597 = fVec10[(IOTA-77)&127];
			fTempPerm598 = fVec10[(IOTA-64)&127];
			fTempPerm599 = fVec10[(IOTA-71)&127];
			fTempPerm600 = fVec10[(IOTA-45)&127];
			fTempPerm601 = fVec10[(IOTA-42)&127];
			fTempPerm602 = fVec10[(IOTA-40)&127];
			fTempPerm603 = fVec10[(IOTA-39)&127];
			fTempPerm604 = fVec11[(IOTA-126)&127];
			fTempPerm605 = fVec10[(IOTA-75)&127];
			fTempPerm606 = fVec11[(IOTA-106)&127];
			fTempPerm607 = fVec11[(IOTA-101)&127];
			fTempPerm608 = fVec11[(IOTA-95)&127];
			fTempPerm609 = fVec11[(IOTA-83)&127];
			fTempPerm610 = fVec11[(IOTA-78)&127];
			fTempPerm611 = fVec11[(IOTA-97)&127];
			fTempPerm612 = fVec11[(IOTA-30)&127];
			fTempPerm613 = fVec11[(IOTA-26)&127];
			fTempPerm614 = fVec11[(IOTA-25)&127];
			fTempPerm615 = fVec11[(IOTA-18)&127];
			fTempPerm616 = fVec11[(IOTA-17)&127];
			fTempPerm617 = fVec11[(IOTA-16)&127];
			fTempPerm618 = fVec11[(IOTA-15)&127];
			fTempPerm619 = fVec11[(IOTA-13)&127];
			fTempPerm620 = fVec11[(IOTA-62)&127];
			fTempPerm621 = fVec11[(IOTA-50)&127];
			fTempPerm622 = fVec12[(IOTA-116)&127];
			fTempPerm623 = fVec12[(IOTA-91)&127];
			fTempPerm624 = fVec12[(IOTA-87)&127];
			fTempPerm625 = fVec12[(IOTA-86)&127];
			fTempPerm626 = fVec12[(IOTA-84)&127];
			fTempPerm627 = fVec12[(IOTA-121)&127];
			fTempPerm628 = fVec12[(IOTA-120)&127];
			fTempPerm629 = fVec12[(IOTA-62)&127];
			fTempPerm630 = fVec12[(IOTA-55)&127];
			fTempPerm631 = fVec12[(IOTA-54)&127];
			fTempPerm632 = fVec12[(IOTA-52)&127];
			fTempPerm633 = fVec12[(IOTA-48)&127];
			fTempPerm634 = fVec12[(IOTA-43)&127];
			fTempPerm635 = fVec12[(IOTA-20)&127];
			fTempPerm636 = fVec12[(IOTA-18)&127];
			fTempPerm637 = fVec12[(IOTA-17)&127];
			fTempPerm638 = fVec12[(IOTA-16)&127];
			fTempPerm639 = fVec12[(IOTA-4)&127];
			fTempPerm640 = fVec12[(IOTA-15)&127];
			fTempPerm641 = fVec12[(IOTA-13)&127];
			fTempPerm642 = fVec12[(IOTA-10)&127];
			fTempPerm643 = fVec12[(IOTA-8)&127];
			fTempPerm644 = fVec12[(IOTA-6)&127];
			fTempPerm645 = fVec13[(IOTA-122)&127];
			fTempPerm646 = fVec13[(IOTA-121)&127];
			fTempPerm647 = fVec13[(IOTA-92)&127];
			fTempPerm648 = fVec13[(IOTA-89)&127];
			fTempPerm649 = fVec13[(IOTA-88)&127];
			fTempPerm650 = fVec13[(IOTA-80)&127];
			fTempPerm651 = fVec13[(IOTA-90)&127];
			fTempPerm652 = fVec13[(IOTA-75)&127];
			fTempPerm653 = fVec13[(IOTA-62)&127];
			fTempPerm654 = fVec13[(IOTA-46)&127];
			fTempPerm655 = fVec13[(IOTA-45)&127];
			fTempPerm656 = fVec13[(IOTA-44)&127];
			fTempPerm657 = fVec13[(IOTA-42)&127];
			fTempPerm658 = fVec13[(IOTA-21)&127];
			fTempPerm659 = fVec13[(IOTA-20)&127];
			fTempPerm660 = fVec13[(IOTA-8)&127];
			fTempPerm661 = fVec13[(IOTA-5)&127];
			fTempPerm662 = fVec13[(IOTA-4)&127];
			fTempPerm663 = fVec13[(IOTA-2)&127];
			fTempPerm664 = ((double)input1[i] * fRec3[0]);
			fRec17[0] = max((fRec17[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm664))))));
			fbargraph14 = fRec17[0];
			fTempPerm665 = fTempPerm664;
			fVec14[IOTA&127] = fTempPerm665;
			fTempPerm666 = fVec14[(IOTA-114)&127];
			fTempPerm667 = fVec14[(IOTA-107)&127];
			fTempPerm668 = fVec14[(IOTA-101)&127];
			fTempPerm669 = fVec14[(IOTA-73)&127];
			fTempPerm670 = fVec14[(IOTA-85)&127];
			fTempPerm671 = fVec14[(IOTA-22)&127];
			fTempPerm672 = fVec2[(IOTA-40)&127];
			fTempPerm673 = fVec3[(IOTA-91)&127];
			fTempPerm674 = fVec3[(IOTA-70)&127];
			fTempPerm675 = fVec3[(IOTA-66)&127];
			fTempPerm676 = fVec3[(IOTA-64)&127];
			fTempPerm677 = fVec3[(IOTA-28)&127];
			fTempPerm678 = fVec3[(IOTA-31)&127];
			fTempPerm679 = fVec4[(IOTA-121)&127];
			fTempPerm680 = fVec4[(IOTA-126)&127];
			fTempPerm681 = fVec4[(IOTA-125)&127];
			fTempPerm682 = fVec4[(IOTA-120)&127];
			fTempPerm683 = fVec4[(IOTA-97)&127];
			fTempPerm684 = fVec4[(IOTA-93)&127];
			fTempPerm685 = fVec4[(IOTA-82)&127];
			fTempPerm686 = fVec4[(IOTA-77)&127];
			fTempPerm687 = fVec4[(IOTA-72)&127];
			fTempPerm688 = fVec4[(IOTA-68)&127];
			fTempPerm689 = fVec4[(IOTA-58)&127];
			fTempPerm690 = fVec4[(IOTA-17)&127];
			fTempPerm691 = fVec5[(IOTA-108)&127];
			fTempPerm692 = fVec5[(IOTA-95)&127];
			fTempPerm693 = fVec5[(IOTA-83)&127];
			fTempPerm694 = fVec5[(IOTA-85)&127];
			fTempPerm695 = fVec5[(IOTA-75)&127];
			fTempPerm696 = fVec5[(IOTA-29)&127];
			fTempPerm697 = fVec5[(IOTA-28)&127];
			fTempPerm698 = fVec5[(IOTA-125)&127];
			fTempPerm699 = fVec5[(IOTA-74)&127];
			fTempPerm700 = fVec5[(IOTA-63)&127];
			fTempPerm701 = fVec5[(IOTA-30)&127];
			fTempPerm702 = fVec6[(IOTA-120)&127];
			fTempPerm703 = fVec6[(IOTA-122)&127];
			fTempPerm704 = fVec6[(IOTA-121)&127];
			fTempPerm705 = fVec6[(IOTA-95)&127];
			fTempPerm706 = fVec6[(IOTA-96)&127];
			fTempPerm707 = fVec6[(IOTA-93)&127];
			fTempPerm708 = fVec6[(IOTA-92)&127];
			fTempPerm709 = fVec6[(IOTA-108)&127];
			fTempPerm710 = fVec6[(IOTA-72)&127];
			fTempPerm711 = fVec6[(IOTA-54)&127];
			fTempPerm712 = fVec6[(IOTA-53)&127];
			fTempPerm713 = fVec6[(IOTA-50)&127];
			fTempPerm714 = fVec6[(IOTA-49)&127];
			fTempPerm715 = fVec6[(IOTA-48)&127];
			fTempPerm716 = fVec6[(IOTA-47)&127];
			fTempPerm717 = fVec7[(IOTA-120)&127];
			fTempPerm718 = fVec7[(IOTA-104)&127];
			fTempPerm719 = fVec7[(IOTA-68)&127];
			fTempPerm720 = fVec7[(IOTA-67)&127];
			fTempPerm721 = fVec8[(IOTA-126)&127];
			fTempPerm722 = fVec8[(IOTA-121)&127];
			fTempPerm723 = fVec8[(IOTA-100)&127];
			fTempPerm724 = fVec8[(IOTA-111)&127];
			fTempPerm725 = fVec8[(IOTA-108)&127];
			fTempPerm726 = fVec8[(IOTA-107)&127];
			fTempPerm727 = fVec8[(IOTA-99)&127];
			fTempPerm728 = fVec8[(IOTA-98)&127];
			fTempPerm729 = fVec8[(IOTA-72)&127];
			fTempPerm730 = fVec8[(IOTA-71)&127];
			fTempPerm731 = fVec8[(IOTA-68)&127];
			fTempPerm732 = fVec8[(IOTA-67)&127];
			fTempPerm733 = fVec8[(IOTA-64)&127];
			fTempPerm734 = fVec8[(IOTA-56)&127];
			fTempPerm735 = fVec8[(IOTA-55)&127];
			fTempPerm736 = fVec8[(IOTA-51)&127];
			fTempPerm737 = fVec8[(IOTA-50)&127];
			fTempPerm738 = fVec9[(IOTA-110)&127];
			fTempPerm739 = fVec9[(IOTA-87)&127];
			fTempPerm740 = fVec9[(IOTA-92)&127];
			fTempPerm741 = fVec9[(IOTA-75)&127];
			fTempPerm742 = fVec9[(IOTA-97)&127];
			fTempPerm743 = fVec9[(IOTA-39)&127];
			fTempPerm744 = fVec9[(IOTA-32)&127];
			fTempPerm745 = fVec9[(IOTA-30)&127];
			fTempPerm746 = fVec9[(IOTA-37)&127];
			fTempPerm747 = fVec9[(IOTA-29)&127];
			fTempPerm748 = fVec9[(IOTA-28)&127];
			fTempPerm749 = fVec9[(IOTA-25)&127];
			fTempPerm750 = fVec10[(IOTA-116)&127];
			fTempPerm751 = fVec9[(IOTA-24)&127];
			fTempPerm752 = fVec7[(IOTA-30)&127];
			fTempPerm753 = fVec10[(IOTA-115)&127];
			fTempPerm754 = fVec10[(IOTA-114)&127];
			fTempPerm755 = fVec10[(IOTA-108)&127];
			fTempPerm756 = fVec10[(IOTA-107)&127];
			fTempPerm757 = fVec10[(IOTA-100)&127];
			fTempPerm758 = fVec10[(IOTA-87)&127];
			fTempPerm759 = fVec10[(IOTA-86)&127];
			fTempPerm760 = fVec10[(IOTA-103)&127];
			fTempPerm761 = fVec10[(IOTA-38)&127];
			fTempPerm762 = fVec10[(IOTA-35)&127];
			fTempPerm763 = fVec10[(IOTA-76)&127];
			fTempPerm764 = fVec10[(IOTA-36)&127];
			fTempPerm765 = fVec10[(IOTA-33)&127];
			fTempPerm766 = fVec10[(IOTA-24)&127];
			fTempPerm767 = fVec10[(IOTA-23)&127];
			fTempPerm768 = fVec10[(IOTA-22)&127];
			fTempPerm769 = fVec10[(IOTA-21)&127];
			fTempPerm770 = fVec10[(IOTA-20)&127];
			fTempPerm771 = fVec10[(IOTA-17)&127];
			fTempPerm772 = fVec10[(IOTA-14)&127];
			fTempPerm773 = fVec10[(IOTA-13)&127];
			fTempPerm774 = fVec10[(IOTA-10)&127];
			fTempPerm775 = fVec10[(IOTA-8)&127];
			fTempPerm776 = fVec10[(IOTA-6)&127];
			fTempPerm777 = fVec10[(IOTA-5)&127];
			fTempPerm778 = fVec10[(IOTA-4)&127];
			fTempPerm779 = fVec10[(IOTA-2)&127];
			fTempPerm780 = fVec11[(IOTA-119)&127];
			fTempPerm781 = fVec11[(IOTA-117)&127];
			fTempPerm782 = fVec11[(IOTA-116)&127];
			fTempPerm783 = fVec11[(IOTA-107)&127];
			fTempPerm784 = fVec11[(IOTA-86)&127];
			fTempPerm785 = fVec11[(IOTA-67)&127];
			fTempPerm786 = fVec11[(IOTA-56)&127];
			fTempPerm787 = fVec11[(IOTA-59)&127];
			fTempPerm788 = fVec11[(IOTA-52)&127];
			fTempPerm789 = fVec11[(IOTA-10)&127];
			fTempPerm790 = fVec11[(IOTA-8)&127];
			fTempPerm791 = fVec11[(IOTA-6)&127];
			fTempPerm792 = fVec11[(IOTA-4)&127];
			fTempPerm793 = fVec11[(IOTA-2)&127];
			fTempPerm794 = fVec12[(IOTA-122)&127];
			fTempPerm795 = fVec12[(IOTA-85)&127];
			fTempPerm796 = fVec12[(IOTA-57)&127];
			fTempPerm797 = fVec12[(IOTA-23)&127];
			fTempPerm798 = fVec12[(IOTA-22)&127];
			fTempPerm799 = fVec12[(IOTA-36)&127];
			fTempPerm800 = fVec13[(IOTA-120)&127];
			fTempPerm801 = fVec12[(IOTA-56)&127];
			fTempPerm802 = fVec12[(IOTA-51)&127];
			fTempPerm803 = fVec13[(IOTA-93)&127];
			fTempPerm804 = fVec13[(IOTA-63)&127];
			fTempPerm805 = fVec13[(IOTA-54)&127];
			fTempPerm806 = fVec14[(IOTA-104)&127];
			fTempPerm807 = fVec14[(IOTA-98)&127];
			fTempPerm808 = fVec14[(IOTA-96)&127];
			fTempPerm809 = fVec14[(IOTA-93)&127];
			fTempPerm810 = fVec14[(IOTA-90)&127];
			fTempPerm811 = fVec14[(IOTA-88)&127];
			fTempPerm812 = fVec14[(IOTA-83)&127];
			fTempPerm813 = fVec14[(IOTA-82)&127];
			fTempPerm814 = fVec14[(IOTA-80)&127];
			fTempPerm815 = fVec14[(IOTA-79)&127];
			fTempPerm816 = ((double)input0[i] * fRec3[0]);
			fRec18[0] = max((fRec18[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm816))))));
			fbargraph15 = fRec18[0];
			fTempPerm817 = fTempPerm816;
			fVec15[IOTA&127] = fTempPerm817;
			fTempPerm818 = fVec15[(IOTA-126)&127];
			fTempPerm819 = fVec15[(IOTA-121)&127];
			fTempPerm820 = fVec15[(IOTA-117)&127];
			fTempPerm821 = fVec15[(IOTA-95)&127];
			fTempPerm822 = fVec15[(IOTA-108)&127];
			fTempPerm823 = fVec15[(IOTA-87)&127];
			fTempPerm824 = fVec15[(IOTA-86)&127];
			fTempPerm825 = fVec15[(IOTA-74)&127];
			fTempPerm826 = fVec15[(IOTA-73)&127];
			fTempPerm827 = fVec15[(IOTA-68)&127];
			fTempPerm828 = fVec15[(IOTA-63)&127];
			fTempPerm829 = fVec15[(IOTA-62)&127];
			fTempPerm830 = fVec15[(IOTA-56)&127];
			fTempPerm831 = fVec15[(IOTA-49)&127];
			fTempPerm832 = fVec15[(IOTA-48)&127];
			fTempPerm833 = fVec15[(IOTA-42)&127];
			fTempPerm834 = fVec15[(IOTA-41)&127];
			fTempPerm835 = fVec15[(IOTA-40)&127];
			fTempPerm836 = fVec15[(IOTA-38)&127];
			fTempPerm837 = fVec15[(IOTA-35)&127];
			fTempPerm838 = fVec15[(IOTA-34)&127];
			fTempPerm839 = fVec15[(IOTA-22)&127];
			fTempPerm840 = fVec15[(IOTA-21)&127];
			fTempPerm841 = fVec15[(IOTA-20)&127];
			fTempPerm842 = fVec15[(IOTA-19)&127];
			fTempPerm843 = fVec15[(IOTA-119)&127];
			fTempPerm844 = fVec15[(IOTA-114)&127];
			fTempPerm845 = fVec15[(IOTA-107)&127];
			fTempPerm846 = fVec15[(IOTA-106)&127];
			fTempPerm847 = fVec15[(IOTA-104)&127];
			fTempPerm848 = fVec15[(IOTA-102)&127];
			fTempPerm849 = fVec15[(IOTA-98)&127];
			fTempPerm850 = fVec15[(IOTA-85)&127];
			fTempPerm851 = fVec15[(IOTA-94)&127];
			fTempPerm852 = fVec15[(IOTA-80)&127];
			fTempPerm853 = fVec15[(IOTA-70)&127];
			fTempPerm854 = fVec15[(IOTA-69)&127];
			fTempPerm855 = fVec15[(IOTA-60)&127];
			fTempPerm856 = fVec15[(IOTA-76)&127];
			fTempPerm857 = fVec15[(IOTA-75)&127];
			fTempPerm858 = fVec15[(IOTA-57)&127];
			fTempPerm859 = fVec15[(IOTA-52)&127];
			fTempPerm860 = fVec15[(IOTA-51)&127];
			fTempPerm861 = fVec15[(IOTA-43)&127];
			fTempPerm862 = fVec15[(IOTA-39)&127];
			fTempPerm863 = fVec15[(IOTA-37)&127];
			fTempPerm864 = fVec15[(IOTA-32)&127];
			fTempPerm865 = fVec15[(IOTA-31)&127];
			fTempPerm866 = fVec15[(IOTA-36)&127];
			fTempPerm867 = fVec15[(IOTA-29)&127];
			fTempPerm868 = fVec15[(IOTA-30)&127];
			fTempPerm869 = fVec15[(IOTA-27)&127];
			fTempPerm870 = fVec15[(IOTA-26)&127];
			fTempPerm871 = fVec15[(IOTA-25)&127];
			fTempPerm872 = fVec15[(IOTA-23)&127];
			fTempPerm873 = fVec13[(IOTA-1)&127];
			fTempPerm874 = fVec10[(IOTA-110)&127];
			fTempPerm875 = fVec10[(IOTA-101)&127];
			fTempPerm876 = fVec10[(IOTA-98)&127];
			fTempPerm877 = fVec10[(IOTA-88)&127];
			fTempPerm878 = fVec10[(IOTA-90)&127];
			fTempPerm879 = fVec10[(IOTA-89)&127];
			fTempPerm880 = fVec10[(IOTA-113)&127];
			fTempPerm881 = fVec10[(IOTA-78)&127];
			fTempPerm882 = fVec10[(IOTA-74)&127];
			fTempPerm883 = fVec10[(IOTA-97)&127];
			fTempPerm884 = fVec10[(IOTA-73)&127];
			fTempPerm885 = fVec10[(IOTA-48)&127];
			fTempPerm886 = fVec10[(IOTA-47)&127];
			fTempPerm887 = fVec10[(IOTA-60)&127];
			fTempPerm888 = fVec11[(IOTA-120)&127];
			fTempPerm889 = fVec11[(IOTA-113)&127];
			fTempPerm890 = fVec11[(IOTA-123)&127];
			fTempPerm891 = fVec11[(IOTA-121)&127];
			fTempPerm892 = fVec11[(IOTA-112)&127];
			fTempPerm893 = fVec11[(IOTA-93)&127];
			fTempPerm894 = fVec11[(IOTA-108)&127];
			fTempPerm895 = fVec11[(IOTA-94)&127];
			fTempPerm896 = fVec11[(IOTA-74)&127];
			fTempPerm897 = fVec11[(IOTA-48)&127];
			fTempPerm898 = fVec11[(IOTA-49)&127];
			fTempPerm899 = fVec11[(IOTA-44)&127];
			fTempPerm900 = fVec11[(IOTA-45)&127];
			fTempPerm901 = fVec11[(IOTA-42)&127];
			fTempPerm902 = fVec11[(IOTA-40)&127];
			fTempPerm903 = fVec11[(IOTA-39)&127];
			fTempPerm904 = fVec11[(IOTA-38)&127];
			fTempPerm905 = fVec11[(IOTA-37)&127];
			fTempPerm906 = fVec11[(IOTA-35)&127];
			fTempPerm907 = fVec11[(IOTA-36)&127];
			fTempPerm908 = fVec12[(IOTA-123)&127];
			fTempPerm909 = fVec12[(IOTA-115)&127];
			fTempPerm910 = fVec11[(IOTA-31)&127];
			fTempPerm911 = fVec12[(IOTA-95)&127];
			fTempPerm912 = fVec12[(IOTA-94)&127];
			fTempPerm913 = fVec12[(IOTA-65)&127];
			fTempPerm914 = fVec12[(IOTA-64)&127];
			fTempPerm915 = fVec12[(IOTA-53)&127];
			fTempPerm916 = fVec12[(IOTA-29)&127];
			fTempPerm917 = fVec12[(IOTA-30)&127];
			fTempPerm918 = fVec12[(IOTA-26)&127];
			fTempPerm919 = fVec13[(IOTA-126)&127];
			fTempPerm920 = fVec13[(IOTA-123)&127];
			fTempPerm921 = fVec13[(IOTA-116)&127];
			fTempPerm922 = fVec13[(IOTA-94)&127];
			fTempPerm923 = fVec13[(IOTA-85)&127];
			fTempPerm924 = fVec13[(IOTA-77)&127];
			fTempPerm925 = fVec13[(IOTA-87)&127];
			fTempPerm926 = fVec13[(IOTA-60)&127];
			fTempPerm927 = fVec13[(IOTA-59)&127];
			fTempPerm928 = fVec13[(IOTA-58)&127];
			fTempPerm929 = fVec13[(IOTA-35)&127];
			fTempPerm930 = fVec13[(IOTA-32)&127];
			fTempPerm931 = fVec13[(IOTA-33)&127];
			fTempPerm932 = fVec13[(IOTA-14)&127];
			fTempPerm933 = fVec13[(IOTA-13)&127];
			fTempPerm934 = fVec13[(IOTA-28)&127];
			fTempPerm935 = fVec14[(IOTA-103)&127];
			fTempPerm936 = fVec14[(IOTA-95)&127];
			fTempPerm937 = fVec14[(IOTA-89)&127];
			fTempPerm938 = fVec14[(IOTA-86)&127];
			fTempPerm939 = fVec13[(IOTA-29)&127];
			fTempPerm940 = fVec14[(IOTA-72)&127];
			fTempPerm941 = fVec14[(IOTA-102)&127];
			fTempPerm942 = fVec15[(IOTA-125)&127];
			fTempPerm943 = fVec15[(IOTA-124)&127];
			fTempPerm944 = fVec15[(IOTA-116)&127];
			fTempPerm945 = fVec15[(IOTA-115)&127];
			fTempPerm946 = fVec15[(IOTA-112)&127];
			fTempPerm947 = fVec15[(IOTA-105)&127];
			fTempPerm948 = fVec15[(IOTA-101)&127];
			fTempPerm949 = fVec15[(IOTA-99)&127];
			fTempPerm950 = fVec15[(IOTA-96)&127];
			fTempPerm951 = fVec15[(IOTA-93)&127];
			fTempPerm952 = fVec15[(IOTA-83)&127];
			fTempPerm953 = fVec15[(IOTA-82)&127];
			fTempPerm954 = fVec15[(IOTA-79)&127];
			fTempPerm955 = fVec15[(IOTA-78)&127];
			fTempPerm956 = fVec15[(IOTA-77)&127];
			fTempPerm957 = fVec15[(IOTA-72)&127];
			fTempPerm958 = fVec15[(IOTA-67)&127];
			fTempPerm959 = fVec15[(IOTA-66)&127];
			fTempPerm960 = fVec15[(IOTA-61)&127];
			fTempPerm961 = fVec15[(IOTA-54)&127];
			fTempPerm962 = fVec15[(IOTA-53)&127];
			fTempPerm963 = fVec15[(IOTA-50)&127];
			fTempPerm964 = fVec15[(IOTA-46)&127];
			fTempPerm965 = fVec15[(IOTA-45)&127];
			fTempPerm966 = fVec15[(IOTA-44)&127];
			fTempPerm967 = fVec15[(IOTA-1)&127];
			fTempPerm968 = fVec10[(IOTA-1)&127];
			fTempPerm969 = fVec11[(IOTA-1)&127];
			fTempPerm970 = fVec12[(IOTA-1)&127];
			fTempPerm971 = fVec13[(IOTA-51)&127];
			fTempPerm972 = fVec13[(IOTA-47)&127];
			fTempPerm973 = fVec13[(IOTA-48)&127];
			fTempPerm974 = fVec13[(IOTA-36)&127];
			fTempPerm975 = fVec13[(IOTA-50)&127];
			fTempPerm976 = fVec13[(IOTA-39)&127];
			fTempPerm977 = fVec13[(IOTA-40)&127];
			fTempPerm978 = fVec13[(IOTA-41)&127];
			fTempPerm979 = fVec14[(IOTA-100)&127];
			fTempPerm980 = fVec14[(IOTA-97)&127];
			fTempPerm981 = fVec14[(IOTA-92)&127];
			fTempPerm982 = fVec14[(IOTA-94)&127];
			fTempPerm983 = fVec14[(IOTA-87)&127];
			fTempPerm984 = fVec14[(IOTA-77)&127];
			fTempPerm985 = fVec14[(IOTA-105)&127];
			fTempPerm986 = fVec14[(IOTA-78)&127];
			fTempPerm987 = fVec14[(IOTA-21)&127];
			fTempPerm988 = fVec14[(IOTA-20)&127];
			fTempPerm989 = fVec14[(IOTA-19)&127];
			fTempPerm990 = fVec14[(IOTA-18)&127];
			fTempPerm991 = fVec14[(IOTA-16)&127];
			fTempPerm992 = fVec14[(IOTA-17)&127];
			fTempPerm993 = fVec14[(IOTA-14)&127];
			fTempPerm994 = fVec14[(IOTA-13)&127];
			fTempPerm995 = fVec14[(IOTA-12)&127];
			fTempPerm996 = fVec14[(IOTA-11)&127];
			fTempPerm997 = fVec14[(IOTA-9)&127];
			fTempPerm998 = fVec14[(IOTA-5)&127];
			fTempPerm999 = fVec14[(IOTA-4)&127];
			fTempPerm1000 = fVec14[(IOTA-3)&127];
			fTempPerm1001 = fVec15[(IOTA-118)&127];
			fTempPerm1002 = fVec15[(IOTA-113)&127];
			fTempPerm1003 = fVec14[(IOTA-7)&127];
			fTempPerm1004 = fVec15[(IOTA-111)&127];
			fTempPerm1005 = fVec15[(IOTA-110)&127];
			fTempPerm1006 = fVec15[(IOTA-109)&127];
			fTempPerm1007 = fVec15[(IOTA-100)&127];
			fTempPerm1008 = fVec15[(IOTA-90)&127];
			fTempPerm1009 = fVec15[(IOTA-65)&127];
			fTempPerm1010 = fVec15[(IOTA-71)&127];
			fTempPerm1011 = fVec15[(IOTA-64)&127];
			fTempPerm1012 = fVec15[(IOTA-33)&127];
			fTempPerm1013 = fVec15[(IOTA-59)&127];
			fTempPerm1014 = fVec15[(IOTA-58)&127];
			fTempPerm1015 = fVec2[(IOTA-51)&127];
			fTempPerm1016 = fVec6[(IOTA-119)&127];
			fTempPerm1017 = fVec9[(IOTA-104)&127];
			fTempPerm1018 = fVec11[(IOTA-58)&127];
			fTempPerm1019 = fVec15[(IOTA-123)&127];
			fTempPerm1020 = fVec14[(IOTA-1)&127];
			fTempPerm1021 = fVec2[(IOTA-75)&127];
			fTempPerm1022 = fVec3[(IOTA-96)&127];
			fTempPerm1023 = fVec4[(IOTA-67)&127];
			fTempPerm1024 = fVec5[(IOTA-61)&127];
			fTempPerm1025 = fVec6[(IOTA-82)&127];
			fTempPerm1026 = fVec6[(IOTA-66)&127];
			fTempPerm1027 = fVec6[(IOTA-51)&127];
			fTempPerm1028 = fVec9[(IOTA-10)&127];
			fTempPerm1029 = fVec10[(IOTA-105)&127];
			fTempPerm1030 = fVec10[(IOTA-54)&127];
			fTempPerm1031 = fVec12[(IOTA-49)&127];
			fTempPerm1032 = fVec12[(IOTA-44)&127];
			fTempPerm1033 = fVec14[(IOTA-84)&127];
			fTempPerm1034 = fVec0[(IOTA-38)&127];
			fTempPerm1035 = fVec4[(IOTA-48)&127];
			fTempPerm1036 = fVec5[(IOTA-56)&127];
			fTempPerm1037 = fVec6[(IOTA-126)&127];
			fTempPerm1038 = fVec6[(IOTA-73)&127];
			fTempPerm1039 = fVec9[(IOTA-93)&127];
			fTempPerm1040 = fVec10[(IOTA-83)&127];
			fTempPerm1041 = fVec10[(IOTA-72)&127];
			fTempPerm1042 = fVec12[(IOTA-96)&127];
			fTempPerm1043 = fVec12[(IOTA-33)&127];
			fTempPerm1044 = fVec13[(IOTA-98)&127];
			fTempPerm1045 = fVec13[(IOTA-26)&127];
			fTempPerm1046 = fVec13[(IOTA-24)&127];
			fTempPerm1047 = fVec13[(IOTA-17)&127];
			fTempPerm1048 = fVec14[(IOTA-99)&127];
			fTempPerm1049 = fVec15[(IOTA-122)&127];
			fTempPerm1050 = fVec15[(IOTA-120)&127];
			fTempPerm1051 = fVec15[(IOTA-18)&127];
			fTempPerm1052 = fVec15[(IOTA-17)&127];
			fTempPerm1053 = fVec15[(IOTA-16)&127];
			fTempPerm1054 = fVec15[(IOTA-15)&127];
			fTempPerm1055 = fVec15[(IOTA-14)&127];
			fTempPerm1056 = fVec15[(IOTA-13)&127];
			fTempPerm1057 = fVec15[(IOTA-12)&127];
			fTempPerm1058 = fVec15[(IOTA-11)&127];
			fTempPerm1059 = fVec15[(IOTA-10)&127];
			fTempPerm1060 = fVec15[(IOTA-9)&127];
			fTempPerm1061 = fVec15[(IOTA-8)&127];
			fTempPerm1062 = fVec15[(IOTA-7)&127];
			fTempPerm1063 = fVec15[(IOTA-6)&127];
			fTempPerm1064 = fVec15[(IOTA-5)&127];
			fTempPerm1065 = fVec15[(IOTA-4)&127];
			fTempPerm1066 = fVec15[(IOTA-3)&127];
			fTempPerm1067 = fVec14[(IOTA-15)&127];
			fTempPerm1068 = fVec4[(IOTA-40)&127];
			fTempPerm1069 = fVec5[(IOTA-105)&127];
			fTempPerm1070 = fVec6[(IOTA-86)&127];
			fTempPerm1071 = fVec9[(IOTA-109)&127];
			fTempPerm1072 = fVec10[(IOTA-104)&127];
			fTempPerm1073 = fVec11[(IOTA-110)&127];
			fTempPerm1074 = fVec15[(IOTA-103)&127];
			fTempPerm1075 = fVec15[(IOTA-55)&127];
			fTempPerm1076 = fVec2[(IOTA-22)&127];
			fTempPerm1077 = fVec2[(IOTA-1)&127];
			fTempPerm1078 = fVec1[(IOTA-1)&127];
			fTempPerm1079 = fVec13[(IOTA-52)&127];
			fTempPerm1080 = fVec13[(IOTA-53)&127];
			fTempPerm1081 = fVec13[(IOTA-125)&127];
			fTempPerm1082 = fVec8[(IOTA-3)&127];
			fTempPerm1083 = fVec8[(IOTA-6)&127];
			fTempPerm1084 = fVec14[(IOTA-71)&127];
			fTempPerm1085 = fVec1[(IOTA-119)&127];
			fTempPerm1086 = fVec0[(IOTA-112)&127];
			fTempPerm1087 = fVec0[(IOTA-103)&127];
			fTempPerm1088 = fVec0[(IOTA-98)&127];
			fTempPerm1089 = fVec0[(IOTA-82)&127];
			fTempPerm1090 = fVec0[(IOTA-28)&127];
			fTempPerm1091 = fVec0[(IOTA-27)&127];
			fTempPerm1092 = fVec1[(IOTA-108)&127];
			fTempPerm1093 = fVec1[(IOTA-107)&127];
			fTempPerm1094 = fVec1[(IOTA-90)&127];
			fTempPerm1095 = fVec1[(IOTA-87)&127];
			fTempPerm1096 = fVec1[(IOTA-75)&127];
			fTempPerm1097 = fVec1[(IOTA-71)&127];
			fTempPerm1098 = fVec1[(IOTA-66)&127];
			fTempPerm1099 = fVec1[(IOTA-69)&127];
			fTempPerm1100 = fVec1[(IOTA-53)&127];
			fTempPerm1101 = fVec1[(IOTA-41)&127];
			fTempPerm1102 = fVec1[(IOTA-40)&127];
			fTempPerm1103 = fVec1[(IOTA-11)&127];
			fTempPerm1104 = fVec1[(IOTA-10)&127];
			fTempPerm1105 = fVec1[(IOTA-8)&127];
			fTempPerm1106 = fVec2[(IOTA-124)&127];
			fTempPerm1107 = fVec1[(IOTA-4)&127];
			fTempPerm1108 = fVec1[(IOTA-2)&127];
			fTempPerm1109 = fVec2[(IOTA-87)&127];
			fTempPerm1110 = fVec2[(IOTA-94)&127];
			fTempPerm1111 = fVec2[(IOTA-88)&127];
			fTempPerm1112 = fVec2[(IOTA-85)&127];
			fTempPerm1113 = fVec2[(IOTA-81)&127];
			fTempPerm1114 = fVec2[(IOTA-82)&127];
			fTempPerm1115 = fVec2[(IOTA-76)&127];
			fTempPerm1116 = fVec2[(IOTA-60)&127];
			fTempPerm1117 = fVec2[(IOTA-57)&127];
			fTempPerm1118 = fVec2[(IOTA-37)&127];
			fTempPerm1119 = fVec2[(IOTA-54)&127];
			fTempPerm1120 = fVec3[(IOTA-117)&127];
			fTempPerm1121 = fVec3[(IOTA-116)&127];
			fTempPerm1122 = fVec0[(IOTA-116)&127];
			fTempPerm1123 = fVec0[(IOTA-111)&127];
			fTempPerm1124 = fVec0[(IOTA-110)&127];
			fTempPerm1125 = fVec0[(IOTA-102)&127];
			fTempPerm1126 = fVec0[(IOTA-115)&127];
			fTempPerm1127 = fVec0[(IOTA-109)&127];
			fTempPerm1128 = fVec0[(IOTA-108)&127];
			fTempPerm1129 = fVec0[(IOTA-88)&127];
			fTempPerm1130 = fVec0[(IOTA-97)&127];
			fTempPerm1131 = fVec0[(IOTA-89)&127];
			fTempPerm1132 = fVec0[(IOTA-81)&127];
			fTempPerm1133 = fVec0[(IOTA-74)&127];
			fTempPerm1134 = fVec0[(IOTA-65)&127];
			fTempPerm1135 = fVec0[(IOTA-59)&127];
			fTempPerm1136 = fVec0[(IOTA-46)&127];
			fTempPerm1137 = fVec0[(IOTA-43)&127];
			fTempPerm1138 = fVec0[(IOTA-45)&127];
			fTempPerm1139 = fVec1[(IOTA-125)&127];
			fTempPerm1140 = fVec1[(IOTA-97)&127];
			fTempPerm1141 = fVec1[(IOTA-83)&127];
			fTempPerm1142 = fVec1[(IOTA-77)&127];
			fTempPerm1143 = fVec1[(IOTA-62)&127];
			fTempPerm1144 = fVec1[(IOTA-48)&127];
			fTempPerm1145 = fVec1[(IOTA-50)&127];
			fTempPerm1146 = fVec1[(IOTA-47)&127];
			fTempPerm1147 = fVec1[(IOTA-46)&127];
			fTempPerm1148 = fVec1[(IOTA-34)&127];
			fTempPerm1149 = fVec2[(IOTA-30)&127];
			fTempPerm1150 = fVec2[(IOTA-29)&127];
			fTempPerm1151 = fVec2[(IOTA-25)&127];
			fTempPerm1152 = fVec2[(IOTA-117)&127];
			fTempPerm1153 = fVec2[(IOTA-91)&127];
			fTempPerm1154 = fVec2[(IOTA-78)&127];
			fTempPerm1155 = fVec2[(IOTA-58)&127];
			fTempPerm1156 = fVec2[(IOTA-49)&127];
			fTempPerm1157 = fVec2[(IOTA-50)&127];
			fTempPerm1158 = fVec2[(IOTA-44)&127];
			fTempPerm1159 = fVec2[(IOTA-43)&127];
			fTempPerm1160 = fVec2[(IOTA-31)&127];
			fTempPerm1161 = fVec0[(IOTA-119)&127];
			fTempPerm1162 = fVec0[(IOTA-107)&127];
			fTempPerm1163 = fVec0[(IOTA-106)&127];
			fTempPerm1164 = fVec0[(IOTA-105)&127];
			fTempPerm1165 = fVec0[(IOTA-90)&127];
			fTempPerm1166 = fVec0[(IOTA-85)&127];
			fTempPerm1167 = fVec0[(IOTA-70)&127];
			fTempPerm1168 = fVec0[(IOTA-67)&127];
			fTempPerm1169 = fVec0[(IOTA-75)&127];
			fTempPerm1170 = fVec0[(IOTA-69)&127];
			fTempPerm1171 = fVec0[(IOTA-64)&127];
			fTempPerm1172 = fVec0[(IOTA-37)&127];
			fTempPerm1173 = fVec0[(IOTA-44)&127];
			fTempPerm1174 = fVec0[(IOTA-35)&127];
			fTempPerm1175 = fVec0[(IOTA-36)&127];
			fTempPerm1176 = fVec0[(IOTA-34)&127];
			fTempPerm1177 = fVec0[(IOTA-33)&127];
			fTempPerm1178 = fVec0[(IOTA-32)&127];
			fTempPerm1179 = fVec0[(IOTA-31)&127];
			fTempPerm1180 = fVec0[(IOTA-25)&127];
			fTempPerm1181 = fVec0[(IOTA-24)&127];
			fTempPerm1182 = fVec1[(IOTA-124)&127];
			fTempPerm1183 = fVec1[(IOTA-123)&127];
			fTempPerm1184 = fVec1[(IOTA-115)&127];
			fTempPerm1185 = fVec1[(IOTA-114)&127];
			fTempPerm1186 = fVec1[(IOTA-112)&127];
			fTempPerm1187 = fVec1[(IOTA-117)&127];
			fTempPerm1188 = fVec1[(IOTA-109)&127];
			fTempPerm1189 = fVec1[(IOTA-86)&127];
			fTempPerm1190 = fVec1[(IOTA-82)&127];
			fTempPerm1191 = fVec1[(IOTA-74)&127];
			fTempPerm1192 = fVec1[(IOTA-70)&127];
			fTempPerm1193 = fVec1[(IOTA-72)&127];
			fTempPerm1194 = fVec1[(IOTA-52)&127];
			fTempPerm1195 = fVec1[(IOTA-51)&127];
			fTempPerm1196 = fVec1[(IOTA-38)&127];
			fTempPerm1197 = fVec2[(IOTA-122)&127];
			fTempPerm1198 = fVec2[(IOTA-120)&127];
			fTempPerm1199 = fVec2[(IOTA-116)&127];
			fTempPerm1200 = fVec2[(IOTA-2)&127];
			fTempPerm1201 = fVec2[(IOTA-4)&127];
			fTempPerm1202 = fVec2[(IOTA-80)&127];
			fTempPerm1203 = fVec2[(IOTA-79)&127];
			fTempPerm1204 = fVec2[(IOTA-77)&127];
			fTempPerm1205 = fVec2[(IOTA-68)&127];
			fTempPerm1206 = fVec2[(IOTA-59)&127];
			fTempPerm1207 = fVec2[(IOTA-56)&127];
			fTempPerm1208 = fVec2[(IOTA-23)&127];
			fTempPerm1209 = fVec2[(IOTA-21)&127];
			fTempPerm1210 = fVec2[(IOTA-20)&127];
			fTempPerm1211 = fVec2[(IOTA-18)&127];
			fTempPerm1212 = fVec2[(IOTA-17)&127];
			fTempPerm1213 = fVec2[(IOTA-16)&127];
			fTempPerm1214 = fVec2[(IOTA-15)&127];
			fTempPerm1215 = fVec2[(IOTA-13)&127];
			fTempPerm1216 = fVec3[(IOTA-119)&127];
			fTempPerm1217 = fVec3[(IOTA-118)&127];
			fTempPerm1218 = fVec2[(IOTA-8)&127];
			fTempPerm1219 = fVec3[(IOTA-124)&127];
			fTempPerm1220 = fVec2[(IOTA-6)&127];
			fTempPerm1221 = fVec3[(IOTA-112)&127];
			fTempPerm1222 = fVec3[(IOTA-87)&127];
			fTempPerm1223 = fVec3[(IOTA-84)&127];
			fTempPerm1224 = fVec3[(IOTA-73)&127];
			fTempPerm1225 = fVec3[(IOTA-50)&127];
			fTempPerm1226 = fVec3[(IOTA-47)&127];
			fTempPerm1227 = fVec3[(IOTA-43)&127];
			fTempPerm1228 = fVec3[(IOTA-42)&127];
			fTempPerm1229 = fVec3[(IOTA-41)&127];
			fTempPerm1230 = fVec3[(IOTA-39)&127];
			fTempPerm1231 = fVec3[(IOTA-20)&127];
			fTempPerm1232 = fVec3[(IOTA-19)&127];
			fTempPerm1233 = fVec3[(IOTA-18)&127];
			fTempPerm1234 = fVec3[(IOTA-35)&127];
			fTempPerm1235 = fVec0[(IOTA-15)&127];
			fTempPerm1236 = fVec0[(IOTA-17)&127];
			fTempPerm1237 = fVec0[(IOTA-14)&127];
			fTempPerm1238 = fVec0[(IOTA-13)&127];
			fTempPerm1239 = fVec0[(IOTA-10)&127];
			fTempPerm1240 = fVec0[(IOTA-8)&127];
			fTempPerm1241 = fVec0[(IOTA-6)&127];
			fTempPerm1242 = fVec0[(IOTA-4)&127];
			fTempPerm1243 = fVec0[(IOTA-2)&127];
			fTempPerm1244 = fVec0[(IOTA-114)&127];
			fTempPerm1245 = fVec0[(IOTA-113)&127];
			fTempPerm1246 = fVec0[(IOTA-101)&127];
			fTempPerm1247 = fVec0[(IOTA-87)&127];
			fTempPerm1248 = fVec0[(IOTA-78)&127];
			fTempPerm1249 = fVec0[(IOTA-118)&127];
			fTempPerm1250 = fVec0[(IOTA-117)&127];
			fTempPerm1251 = fVec0[(IOTA-68)&127];
			fTempPerm1252 = fVec0[(IOTA-58)&127];
			fTempPerm1253 = fVec0[(IOTA-77)&127];
			fTempPerm1254 = fVec0[(IOTA-76)&127];
			fTempPerm1255 = fVec0[(IOTA-56)&127];
			fTempPerm1256 = fVec0[(IOTA-55)&127];
			fTempPerm1257 = fVec0[(IOTA-40)&127];
			fTempPerm1258 = fVec0[(IOTA-18)&127];
			fTempPerm1259 = fVec1[(IOTA-120)&127];
			fTempPerm1260 = fVec1[(IOTA-116)&127];
			fTempPerm1261 = fVec1[(IOTA-96)&127];
			fTempPerm1262 = fVec1[(IOTA-84)&127];
			fTempPerm1263 = fVec1[(IOTA-81)&127];
			fTempPerm1264 = fVec1[(IOTA-78)&127];
			fTempPerm1265 = fVec1[(IOTA-73)&127];
			fTempPerm1266 = fVec1[(IOTA-57)&127];
			fTempPerm1267 = fVec1[(IOTA-37)&127];
			fTempPerm1268 = fVec1[(IOTA-33)&127];
			fTempPerm1269 = fVec1[(IOTA-29)&127];
			fTempPerm1270 = fVec1[(IOTA-28)&127];
			fTempPerm1271 = fVec1[(IOTA-22)&127];
			fTempPerm1272 = fVec1[(IOTA-20)&127];
			fTempPerm1273 = fVec1[(IOTA-19)&127];
			fTempPerm1274 = fVec1[(IOTA-17)&127];
			fTempPerm1275 = fVec1[(IOTA-16)&127];
			fTempPerm1276 = fVec1[(IOTA-14)&127];
			fTempPerm1277 = fVec1[(IOTA-13)&127];
			fTempPerm1278 = fVec2[(IOTA-125)&127];
			fTempPerm1279 = fVec2[(IOTA-126)&127];
			fTempPerm1280 = fVec2[(IOTA-89)&127];
			fTempPerm1281 = fVec2[(IOTA-84)&127];
			fTempPerm1282 = fVec2[(IOTA-61)&127];
			fTempPerm1283 = fVec2[(IOTA-55)&127];
			fTempPerm1284 = fVec2[(IOTA-53)&127];
			fTempPerm1285 = fVec2[(IOTA-52)&127];
			fTempPerm1286 = fVec2[(IOTA-35)&127];
			fTempPerm1287 = fVec2[(IOTA-26)&127];
			fTempPerm1288 = fVec3[(IOTA-125)&127];
			fTempPerm1289 = fVec3[(IOTA-114)&127];
			fTempPerm1290 = fVec3[(IOTA-88)&127];
			fTempPerm1291 = fVec3[(IOTA-78)&127];
			fTempPerm1292 = fVec3[(IOTA-76)&127];
			fTempPerm1293 = fVec3[(IOTA-62)&127];
			fTempPerm1294 = fVec3[(IOTA-53)&127];
			fTempPerm1295 = fVec3[(IOTA-49)&127];
			fTempPerm1296 = fVec3[(IOTA-55)&127];
			fTempPerm1297 = fVec4[(IOTA-122)&127];
			fTempPerm1298 = fVec4[(IOTA-115)&127];
			fTempPerm1299 = fVec4[(IOTA-104)&127];
			fTempPerm1300 = fVec4[(IOTA-87)&127];
			fTempPerm1301 = fVec4[(IOTA-83)&127];
			fTempPerm1302 = fVec4[(IOTA-79)&127];
			fTempPerm1303 = fVec4[(IOTA-70)&127];
			fTempPerm1304 = fVec4[(IOTA-63)&127];
			fTempPerm1305 = fVec4[(IOTA-5)&127];
			fTempPerm1306 = fVec4[(IOTA-3)&127];
			fTempPerm1307 = fVec4[(IOTA-60)&127];
			fTempPerm1308 = fVec4[(IOTA-59)&127];
			fTempPerm1309 = fVec4[(IOTA-36)&127];
			fTempPerm1310 = fVec4[(IOTA-7)&127];
			fTempPerm1311 = fVec5[(IOTA-118)&127];
			fTempPerm1312 = fVec5[(IOTA-117)&127];
			fTempPerm1313 = fVec5[(IOTA-88)&127];
			fTempPerm1314 = fVec5[(IOTA-87)&127];
			fTempPerm1315 = fVec5[(IOTA-76)&127];
			fTempPerm1316 = fVec5[(IOTA-60)&127];
			fTempPerm1317 = fVec5[(IOTA-59)&127];
			fTempPerm1318 = fVec5[(IOTA-72)&127];
			fTempPerm1319 = fVec5[(IOTA-58)&127];
			fTempPerm1320 = fVec4[(IOTA-116)&127];
			fTempPerm1321 = fVec4[(IOTA-112)&127];
			fTempPerm1322 = fVec4[(IOTA-106)&127];
			fTempPerm1323 = fVec4[(IOTA-105)&127];
			fTempPerm1324 = fVec4[(IOTA-111)&127];
			fTempPerm1325 = fVec4[(IOTA-100)&127];
			fTempPerm1326 = fVec4[(IOTA-74)&127];
			fTempPerm1327 = fVec4[(IOTA-69)&127];
			fTempPerm1328 = fVec4[(IOTA-65)&127];
			fTempPerm1329 = fVec4[(IOTA-47)&127];
			fTempPerm1330 = fVec4[(IOTA-14)&127];
			fTempPerm1331 = fVec4[(IOTA-13)&127];
			fTempPerm1332 = fVec4[(IOTA-12)&127];
			fTempPerm1333 = fVec4[(IOTA-11)&127];
			fTempPerm1334 = fVec4[(IOTA-9)&127];
			fTempPerm1335 = fVec5[(IOTA-113)&127];
			fTempPerm1336 = fVec5[(IOTA-111)&127];
			fTempPerm1337 = fVec5[(IOTA-94)&127];
			fTempPerm1338 = fVec5[(IOTA-92)&127];
			fTempPerm1339 = fVec5[(IOTA-91)&127];
			fTempPerm1340 = fVec5[(IOTA-80)&127];
			fTempPerm1341 = fVec5[(IOTA-98)&127];
			fTempPerm1342 = fVec5[(IOTA-50)&127];
			fTempPerm1343 = fVec5[(IOTA-47)&127];
			fTempPerm1344 = fVec5[(IOTA-49)&127];
			fTempPerm1345 = fVec5[(IOTA-46)&127];
			fTempPerm1346 = fVec5[(IOTA-41)&127];
			fTempPerm1347 = fVec5[(IOTA-39)&127];
			fTempPerm1348 = fVec5[(IOTA-40)&127];
			fTempPerm1349 = fVec5[(IOTA-48)&127];
			fTempPerm1350 = fVec5[(IOTA-45)&127];
			fTempPerm1351 = fVec5[(IOTA-34)&127];
			fTempPerm1352 = fVec5[(IOTA-35)&127];
			fTempPerm1353 = fVec5[(IOTA-27)&127];
			fTempPerm1354 = fVec5[(IOTA-24)&127];
			fTempPerm1355 = fVec5[(IOTA-23)&127];
			fTempPerm1356 = fVec5[(IOTA-22)&127];
			fTempPerm1357 = fVec5[(IOTA-20)&127];
			fTempPerm1358 = fVec5[(IOTA-17)&127];
			fTempPerm1359 = fVec5[(IOTA-16)&127];
			fTempPerm1360 = fVec5[(IOTA-14)&127];
			fTempPerm1361 = fVec5[(IOTA-13)&127];
			fTempPerm1362 = fVec5[(IOTA-8)&127];
			fTempPerm1363 = fVec5[(IOTA-5)&127];
			fTempPerm1364 = fVec5[(IOTA-4)&127];
			fTempPerm1365 = fVec6[(IOTA-124)&127];
			fTempPerm1366 = fVec6[(IOTA-125)&127];
			fTempPerm1367 = fVec6[(IOTA-103)&127];
			fTempPerm1368 = fVec6[(IOTA-102)&127];
			fTempPerm1369 = fVec6[(IOTA-99)&127];
			fTempPerm1370 = fVec6[(IOTA-90)&127];
			fTempPerm1371 = fVec6[(IOTA-89)&127];
			fTempPerm1372 = fVec6[(IOTA-63)&127];
			fTempPerm1373 = fVec7[(IOTA-87)&127];
			fTempPerm1374 = fVec7[(IOTA-93)&127];
			fTempPerm1375 = fVec7[(IOTA-81)&127];
			fTempPerm1376 = fVec7[(IOTA-61)&127];
			fTempPerm1377 = fVec7[(IOTA-62)&127];
			fTempPerm1378 = fVec7[(IOTA-63)&127];
			fTempPerm1379 = fVec7[(IOTA-58)&127];
			fTempPerm1380 = fVec7[(IOTA-59)&127];
			fTempPerm1381 = fVec7[(IOTA-57)&127];
			fTempPerm1382 = fVec7[(IOTA-56)&127];
			fTempPerm1383 = fVec7[(IOTA-105)&127];
			fTempPerm1384 = fVec7[(IOTA-109)&127];
			fTempPerm1385 = fVec7[(IOTA-108)&127];
			fTempPerm1386 = fVec7[(IOTA-102)&127];
			fTempPerm1387 = fVec7[(IOTA-101)&127];
			fTempPerm1388 = fVec6[(IOTA-39)&127];
			fTempPerm1389 = fVec7[(IOTA-118)&127];
			fTempPerm1390 = fVec7[(IOTA-107)&127];
			fTempPerm1391 = fVec6[(IOTA-36)&127];
			fTempPerm1392 = fVec8[(IOTA-123)&127];
			fTempPerm1393 = fVec8[(IOTA-118)&127];
			fTempPerm1394 = fVec7[(IOTA-123)&127];
			fTempPerm1395 = fVec8[(IOTA-122)&127];
			fTempPerm1396 = fVec8[(IOTA-109)&127];
			fTempPerm1397 = fVec8[(IOTA-95)&127];
			fTempPerm1398 = fVec8[(IOTA-91)&127];
			fTempPerm1399 = fVec8[(IOTA-88)&127];
			fTempPerm1400 = fVec8[(IOTA-81)&127];
			fTempPerm1401 = fVec8[(IOTA-74)&127];
			fTempPerm1402 = fVec8[(IOTA-53)&127];
			fTempPerm1403 = fVec8[(IOTA-49)&127];
			fTempPerm1404 = fVec8[(IOTA-43)&127];
			fTempPerm1405 = fVec8[(IOTA-42)&127];
			fTempPerm1406 = fVec8[(IOTA-37)&127];
			fTempPerm1407 = fVec8[(IOTA-16)&127];
			fTempPerm1408 = fVec8[(IOTA-14)&127];
			fTempPerm1409 = fVec8[(IOTA-13)&127];
			fTempPerm1410 = fVec8[(IOTA-8)&127];
			fTempPerm1411 = fVec9[(IOTA-122)&127];
			fTempPerm1412 = fVec9[(IOTA-115)&127];
			fTempPerm1413 = fVec9[(IOTA-96)&127];
			fTempPerm1414 = fVec9[(IOTA-116)&127];
			fTempPerm1415 = fVec9[(IOTA-95)&127];
			fTempPerm1416 = fVec9[(IOTA-89)&127];
			fTempPerm1417 = fVec9[(IOTA-86)&127];
			fTempPerm1418 = fVec9[(IOTA-78)&127];
			fTempPerm1419 = fVec9[(IOTA-68)&127];
			fTempPerm1420 = fVec9[(IOTA-67)&127];
			fTempPerm1421 = fVec9[(IOTA-77)&127];
			fTempPerm1422 = fVec9[(IOTA-57)&127];
			fTempPerm1423 = fVec9[(IOTA-51)&127];
			fTempPerm1424 = fVec9[(IOTA-50)&127];
			fTempPerm1425 = fVec9[(IOTA-44)&127];
			fTempPerm1426 = fVec9[(IOTA-42)&127];
			fTempPerm1427 = fVec9[(IOTA-41)&127];
			fTempPerm1428 = fVec9[(IOTA-40)&127];
			fTempPerm1429 = fVec9[(IOTA-35)&127];
			fTempPerm1430 = fVec9[(IOTA-34)&127];
			fTempPerm1431 = fVec9[(IOTA-23)&127];
			fTempPerm1432 = fVec9[(IOTA-17)&127];
			fTempPerm1433 = fVec9[(IOTA-16)&127];
			fTempPerm1434 = fVec9[(IOTA-14)&127];
			fTempPerm1435 = fVec9[(IOTA-13)&127];
			fTempPerm1436 = fVec9[(IOTA-12)&127];
			fTempPerm1437 = fVec9[(IOTA-9)&127];
			fTempPerm1438 = fVec9[(IOTA-7)&127];
			fTempPerm1439 = fVec9[(IOTA-5)&127];
			fTempPerm1440 = fVec9[(IOTA-4)&127];
			fTempPerm1441 = fVec9[(IOTA-3)&127];
			fTempPerm1442 = fVec10[(IOTA-121)&127];
			fTempPerm1443 = fVec10[(IOTA-120)&127];
			fTempPerm1444 = fVec3[(IOTA-61)&127];
			fTempPerm1445 = fVec3[(IOTA-44)&127];
			fTempPerm1446 = fVec3[(IOTA-36)&127];
			fTempPerm1447 = fVec3[(IOTA-24)&127];
			fTempPerm1448 = fVec3[(IOTA-17)&127];
			fTempPerm1449 = fVec3[(IOTA-14)&127];
			fTempPerm1450 = fVec3[(IOTA-13)&127];
			fTempPerm1451 = fVec3[(IOTA-8)&127];
			fTempPerm1452 = fVec3[(IOTA-6)&127];
			fTempPerm1453 = fVec3[(IOTA-5)&127];
			fTempPerm1454 = fVec3[(IOTA-2)&127];
			fTempPerm1455 = fVec2[(IOTA-123)&127];
			fTempPerm1456 = fVec3[(IOTA-126)&127];
			fTempPerm1457 = fVec3[(IOTA-113)&127];
			fTempPerm1458 = fVec3[(IOTA-93)&127];
			fTempPerm1459 = fVec3[(IOTA-82)&127];
			fTempPerm1460 = fVec3[(IOTA-77)&127];
			fTempPerm1461 = fVec3[(IOTA-83)&127];
			fTempPerm1462 = fVec3[(IOTA-75)&127];
			fTempPerm1463 = fVec3[(IOTA-74)&127];
			fTempPerm1464 = fVec3[(IOTA-65)&127];
			fTempPerm1465 = fVec2[(IOTA-121)&127];
			fTempPerm1466 = fVec3[(IOTA-122)&127];
			fTempPerm1467 = fVec4[(IOTA-118)&127];
			fTempPerm1468 = fVec4[(IOTA-99)&127];
			fTempPerm1469 = fVec4[(IOTA-117)&127];
			fTempPerm1470 = fVec4[(IOTA-108)&127];
			fTempPerm1471 = fVec4[(IOTA-88)&127];
			fTempPerm1472 = fVec4[(IOTA-86)&127];
			fTempPerm1473 = fVec4[(IOTA-75)&127];
			fTempPerm1474 = fVec4[(IOTA-64)&127];
			fTempPerm1475 = fVec4[(IOTA-42)&127];
			fTempPerm1476 = fVec4[(IOTA-38)&127];
			fTempPerm1477 = fVec4[(IOTA-35)&127];
			fTempPerm1478 = fVec4[(IOTA-33)&127];
			fTempPerm1479 = fVec4[(IOTA-34)&127];
			fTempPerm1480 = fVec4[(IOTA-37)&127];
			fTempPerm1481 = fVec4[(IOTA-31)&127];
			fTempPerm1482 = fVec4[(IOTA-29)&127];
			fTempPerm1483 = fVec4[(IOTA-32)&127];
			fTempPerm1484 = fVec4[(IOTA-28)&127];
			fTempPerm1485 = fVec4[(IOTA-27)&127];
			fTempPerm1486 = fVec4[(IOTA-24)&127];
			fTempPerm1487 = fVec4[(IOTA-23)&127];
			fTempPerm1488 = fVec5[(IOTA-112)&127];
			fTempPerm1489 = fVec5[(IOTA-107)&127];
			fTempPerm1490 = fVec5[(IOTA-99)&127];
			fTempPerm1491 = fVec5[(IOTA-100)&127];
			fTempPerm1492 = fVec5[(IOTA-89)&127];
			fTempPerm1493 = fVec5[(IOTA-82)&127];
			fTempPerm1494 = fVec5[(IOTA-73)&127];
			fTempPerm1495 = fVec5[(IOTA-62)&127];
			fTempPerm1496 = fVec5[(IOTA-53)&127];
			fTempPerm1497 = fVec6[(IOTA-106)&127];
			fTempPerm1498 = fVec6[(IOTA-88)&127];
			fTempPerm1499 = fVec6[(IOTA-79)&127];
			fTempPerm1500 = fVec6[(IOTA-85)&127];
			fTempPerm1501 = fVec6[(IOTA-84)&127];
			fTempPerm1502 = fVec6[(IOTA-81)&127];
			fTempPerm1503 = fVec6[(IOTA-71)&127];
			fTempPerm1504 = fVec6[(IOTA-70)&127];
			fTempPerm1505 = fVec6[(IOTA-58)&127];
			fTempPerm1506 = fVec6[(IOTA-26)&127];
			fTempPerm1507 = fVec6[(IOTA-25)&127];
			fTempPerm1508 = fVec7[(IOTA-114)&127];
			fTempPerm1509 = fVec7[(IOTA-106)&127];
			fTempPerm1510 = fVec7[(IOTA-97)&127];
			fTempPerm1511 = fVec7[(IOTA-84)&127];
			fTempPerm1512 = fVec7[(IOTA-73)&127];
			fTempPerm1513 = fVec7[(IOTA-60)&127];
			fTempPerm1514 = fVec7[(IOTA-50)&127];
			fTempPerm1515 = fVec7[(IOTA-48)&127];
			fTempPerm1516 = fVec7[(IOTA-51)&127];
			fTempPerm1517 = fVec7[(IOTA-52)&127];
			fTempPerm1518 = fVec7[(IOTA-45)&127];
			fTempPerm1519 = fVec7[(IOTA-46)&127];
			fTempPerm1520 = fVec7[(IOTA-47)&127];
			fTempPerm1521 = fVec7[(IOTA-41)&127];
			fTempPerm1522 = fVec7[(IOTA-39)&127];
			fTempPerm1523 = fVec7[(IOTA-40)&127];
			fTempPerm1524 = fVec7[(IOTA-25)&127];
			fTempPerm1525 = fVec7[(IOTA-21)&127];
			fTempPerm1526 = fVec7[(IOTA-17)&127];
			fTempPerm1527 = fVec7[(IOTA-16)&127];
			fTempPerm1528 = fVec7[(IOTA-15)&127];
			fTempPerm1529 = fVec7[(IOTA-14)&127];
			fTempPerm1530 = fVec7[(IOTA-13)&127];
			fTempPerm1531 = fVec7[(IOTA-12)&127];
			fTempPerm1532 = fVec7[(IOTA-9)&127];
			fTempPerm1533 = fVec7[(IOTA-7)&127];
			fTempPerm1534 = fVec7[(IOTA-5)&127];
			fTempPerm1535 = fVec7[(IOTA-3)&127];
			fTempPerm1536 = fVec8[(IOTA-120)&127];
			fTempPerm1537 = fVec8[(IOTA-114)&127];
			fTempPerm1538 = fVec8[(IOTA-113)&127];
			fTempPerm1539 = fVec8[(IOTA-87)&127];
			fTempPerm1540 = fVec8[(IOTA-80)&127];
			fTempPerm1541 = fVec8[(IOTA-83)&127];
			fTempPerm1542 = fVec8[(IOTA-75)&127];
			fTempPerm1543 = fVec8[(IOTA-60)&127];
			fTempPerm1544 = fVec8[(IOTA-40)&127];
			fTempPerm1545 = fVec8[(IOTA-33)&127];
			fTempPerm1546 = fVec8[(IOTA-34)&127];
			fTempPerm1547 = fVec8[(IOTA-30)&127];
			fTempPerm1548 = fVec8[(IOTA-28)&127];
			fTempPerm1549 = fVec8[(IOTA-27)&127];
			fTempPerm1550 = fVec8[(IOTA-24)&127];
			fTempPerm1551 = fVec8[(IOTA-23)&127];
			fTempPerm1552 = fVec8[(IOTA-21)&127];
			fTempPerm1553 = fVec8[(IOTA-22)&127];
			fTempPerm1554 = fVec8[(IOTA-20)&127];
			fTempPerm1555 = fVec9[(IOTA-124)&127];
			fTempPerm1556 = fVec9[(IOTA-120)&127];
			fTempPerm1557 = fVec9[(IOTA-113)&127];
			fTempPerm1558 = fVec9[(IOTA-62)&127];
			fTempPerm1559 = fVec9[(IOTA-55)&127];
			fTempPerm1560 = fVec9[(IOTA-54)&127];
			fTempPerm1561 = fVec9[(IOTA-53)&127];
			fTempPerm1562 = fVec10[(IOTA-112)&127];
			fTempPerm1563 = fVec10[(IOTA-66)&127];
			fTempPerm1564 = fVec10[(IOTA-53)&127];
			fTempPerm1565 = fVec10[(IOTA-30)&127];
			fTempPerm1566 = fVec10[(IOTA-31)&127];
			fTempPerm1567 = fVec10[(IOTA-63)&127];
			fTempPerm1568 = fVec10[(IOTA-62)&127];
			fTempPerm1569 = fVec10[(IOTA-55)&127];
			fTempPerm1570 = fVec11[(IOTA-122)&127];
			fTempPerm1571 = fVec11[(IOTA-103)&127];
			fTempPerm1572 = fVec11[(IOTA-102)&127];
			fTempPerm1573 = fVec11[(IOTA-96)&127];
			fTempPerm1574 = fVec11[(IOTA-84)&127];
			fTempPerm1575 = fVec10[(IOTA-27)&127];
			fTempPerm1576 = fVec11[(IOTA-85)&127];
			fTempPerm1577 = fVec11[(IOTA-65)&127];
			fTempPerm1578 = fVec12[(IOTA-114)&127];
			fTempPerm1579 = fVec12[(IOTA-113)&127];
			fTempPerm1580 = fVec11[(IOTA-55)&127];
			fTempPerm1581 = fVec12[(IOTA-107)&127];
			fTempPerm1582 = fVec12[(IOTA-110)&127];
			fTempPerm1583 = fVec12[(IOTA-75)&127];
			fTempPerm1584 = fVec12[(IOTA-72)&127];
			fTempPerm1585 = fVec12[(IOTA-60)&127];
			fTempPerm1586 = fVec12[(IOTA-61)&127];
			fTempPerm1587 = fVec12[(IOTA-47)&127];
			fTempPerm1588 = fVec12[(IOTA-46)&127];
			fTempPerm1589 = fVec12[(IOTA-41)&127];
			fTempPerm1590 = fVec12[(IOTA-35)&127];
			fTempPerm1591 = fVec12[(IOTA-3)&127];
			fTempPerm1592 = fVec13[(IOTA-114)&127];
			fTempPerm1593 = fVec13[(IOTA-115)&127];
			fTempPerm1594 = fVec13[(IOTA-113)&127];
			fTempPerm1595 = fVec13[(IOTA-105)&127];
			fTempPerm1596 = fVec13[(IOTA-107)&127];
			fTempPerm1597 = fVec13[(IOTA-106)&127];
			fTempPerm1598 = fVec12[(IOTA-106)&127];
			fTempPerm1599 = fVec13[(IOTA-102)&127];
			fTempPerm1600 = fVec13[(IOTA-103)&127];
			fTempPerm1601 = fVec13[(IOTA-96)&127];
			fTempPerm1602 = fVec13[(IOTA-95)&127];
			fTempPerm1603 = fVec13[(IOTA-100)&127];
			fTempPerm1604 = fVec13[(IOTA-70)&127];
			fTempPerm1605 = fVec13[(IOTA-55)&127];
			fTempPerm1606 = fVec13[(IOTA-66)&127];
			fTempPerm1607 = fVec6[(IOTA-115)&127];
			fTempPerm1608 = fVec6[(IOTA-113)&127];
			fTempPerm1609 = fVec6[(IOTA-101)&127];
			fTempPerm1610 = fVec6[(IOTA-100)&127];
			fTempPerm1611 = fVec6[(IOTA-91)&127];
			fTempPerm1612 = fVec6[(IOTA-69)&127];
			fTempPerm1613 = fVec6[(IOTA-68)&127];
			fTempPerm1614 = fVec6[(IOTA-67)&127];
			fTempPerm1615 = fVec6[(IOTA-33)&127];
			fTempPerm1616 = fVec6[(IOTA-31)&127];
			fTempPerm1617 = fVec6[(IOTA-34)&127];
			fTempPerm1618 = fVec6[(IOTA-32)&127];
			fTempPerm1619 = fVec6[(IOTA-30)&127];
			fTempPerm1620 = fVec6[(IOTA-29)&127];
			fTempPerm1621 = fVec6[(IOTA-28)&127];
			fTempPerm1622 = fVec6[(IOTA-27)&127];
			fTempPerm1623 = fVec6[(IOTA-24)&127];
			fTempPerm1624 = fVec6[(IOTA-21)&127];
			fTempPerm1625 = fVec6[(IOTA-17)&127];
			fTempPerm1626 = fVec6[(IOTA-14)&127];
			fTempPerm1627 = fVec6[(IOTA-13)&127];
			fTempPerm1628 = fVec6[(IOTA-12)&127];
			fTempPerm1629 = fVec6[(IOTA-11)&127];
			fTempPerm1630 = fVec6[(IOTA-9)&127];
			fTempPerm1631 = fVec6[(IOTA-7)&127];
			fTempPerm1632 = fVec6[(IOTA-5)&127];
			fTempPerm1633 = fVec6[(IOTA-3)&127];
			fTempPerm1634 = fVec7[(IOTA-125)&127];
			fTempPerm1635 = fVec7[(IOTA-112)&127];
			fTempPerm1636 = fVec7[(IOTA-99)&127];
			fTempPerm1637 = fVec7[(IOTA-98)&127];
			fTempPerm1638 = fVec7[(IOTA-89)&127];
			fTempPerm1639 = fVec7[(IOTA-78)&127];
			fTempPerm1640 = fVec7[(IOTA-54)&127];
			fTempPerm1641 = fVec7[(IOTA-37)&127];
			fTempPerm1642 = fVec7[(IOTA-35)&127];
			fTempPerm1643 = fVec7[(IOTA-36)&127];
			fTempPerm1644 = fVec8[(IOTA-96)&127];
			fTempPerm1645 = fVec8[(IOTA-90)&127];
			fTempPerm1646 = fVec8[(IOTA-84)&127];
			fTempPerm1647 = fVec8[(IOTA-79)&127];
			fTempPerm1648 = fVec8[(IOTA-73)&127];
			fTempPerm1649 = fVec8[(IOTA-47)&127];
			fTempPerm1650 = fVec8[(IOTA-45)&127];
			fTempPerm1651 = fVec8[(IOTA-39)&127];
			fTempPerm1652 = fVec8[(IOTA-17)&127];
			fTempPerm1653 = fVec9[(IOTA-125)&127];
			fTempPerm1654 = fVec9[(IOTA-123)&127];
			fTempPerm1655 = fVec9[(IOTA-121)&127];
			fTempPerm1656 = fVec8[(IOTA-52)&127];
			fTempPerm1657 = fVec9[(IOTA-117)&127];
			fTempPerm1658 = fVec9[(IOTA-111)&127];
			fTempPerm1659 = fVec9[(IOTA-85)&127];
			fTempPerm1660 = fVec9[(IOTA-112)&127];
			fTempPerm1661 = fVec9[(IOTA-76)&127];
			fTempPerm1662 = fVec9[(IOTA-81)&127];
			fTempPerm1663 = fVec9[(IOTA-61)&127];
			fTempPerm1664 = fVec9[(IOTA-59)&127];
			fTempPerm1665 = fVec9[(IOTA-56)&127];
			fTempPerm1666 = fVec9[(IOTA-49)&127];
			fTempPerm1667 = fVec9[(IOTA-48)&127];
			fTempPerm1668 = fVec9[(IOTA-58)&127];
			fTempPerm1669 = fVec9[(IOTA-46)&127];
			fTempPerm1670 = fVec9[(IOTA-47)&127];
			fTempPerm1671 = fVec9[(IOTA-45)&127];
			fTempPerm1672 = fVec10[(IOTA-70)&127];
			fTempPerm1673 = fVec10[(IOTA-59)&127];
			fTempPerm1674 = fVec10[(IOTA-61)&127];
			fTempPerm1675 = fVec10[(IOTA-43)&127];
			fTempPerm1676 = fVec10[(IOTA-44)&127];
			fTempPerm1677 = fVec10[(IOTA-41)&127];
			fTempPerm1678 = fVec10[(IOTA-58)&127];
			fTempPerm1679 = fVec10[(IOTA-57)&127];
			fTempPerm1680 = fVec11[(IOTA-100)&127];
			fTempPerm1681 = fVec11[(IOTA-89)&127];
			fTempPerm1682 = fVec11[(IOTA-82)&127];
			fTempPerm1683 = fVec11[(IOTA-79)&127];
			fTempPerm1684 = fVec11[(IOTA-71)&127];
			fTempPerm1685 = fVec11[(IOTA-81)&127];
			fTempPerm1686 = fVec11[(IOTA-43)&127];
			fTempPerm1687 = fVec11[(IOTA-29)&127];
			fTempPerm1688 = fVec11[(IOTA-28)&127];
			fTempPerm1689 = fVec11[(IOTA-27)&127];
			fTempPerm1690 = fVec11[(IOTA-24)&127];
			fTempPerm1691 = fVec11[(IOTA-23)&127];
			fTempPerm1692 = fVec11[(IOTA-22)&127];
			fTempPerm1693 = fVec11[(IOTA-20)&127];
			fTempPerm1694 = fVec11[(IOTA-21)&127];
			fTempPerm1695 = fVec11[(IOTA-19)&127];
			fTempPerm1696 = fVec11[(IOTA-14)&127];
			fTempPerm1697 = fVec11[(IOTA-80)&127];
			fTempPerm1698 = fVec11[(IOTA-69)&127];
			fTempPerm1699 = fVec11[(IOTA-70)&127];
			fTempPerm1700 = fVec11[(IOTA-76)&127];
			fTempPerm1701 = fVec11[(IOTA-63)&127];
			fTempPerm1702 = fVec11[(IOTA-51)&127];
			fTempPerm1703 = fVec12[(IOTA-108)&127];
			fTempPerm1704 = fVec12[(IOTA-101)&127];
			fTempPerm1705 = fVec12[(IOTA-100)&127];
			fTempPerm1706 = fVec12[(IOTA-97)&127];
			fTempPerm1707 = fVec12[(IOTA-89)&127];
			fTempPerm1708 = fVec12[(IOTA-79)&127];
			fTempPerm1709 = fVec12[(IOTA-67)&127];
			fTempPerm1710 = fVec12[(IOTA-66)&127];
			fTempPerm1711 = fVec12[(IOTA-68)&127];
			fTempPerm1712 = fVec12[(IOTA-21)&127];
			fTempPerm1713 = fVec12[(IOTA-19)&127];
			fTempPerm1714 = fVec12[(IOTA-14)&127];
			fTempPerm1715 = fVec12[(IOTA-12)&127];
			fTempPerm1716 = fVec12[(IOTA-11)&127];
			fTempPerm1717 = fVec12[(IOTA-9)&127];
			fTempPerm1718 = fVec12[(IOTA-7)&127];
			fTempPerm1719 = fVec12[(IOTA-5)&127];
			fTempPerm1720 = fVec13[(IOTA-111)&127];
			fTempPerm1721 = fVec13[(IOTA-99)&127];
			fTempPerm1722 = fVec13[(IOTA-72)&127];
			fTempPerm1723 = fVec13[(IOTA-69)&127];
			fTempPerm1724 = fVec13[(IOTA-73)&127];
			fTempPerm1725 = fVec13[(IOTA-74)&127];
			fTempPerm1726 = fVec13[(IOTA-43)&127];
			fTempPerm1727 = fVec13[(IOTA-22)&127];
			fTempPerm1728 = fVec13[(IOTA-19)&127];
			fTempPerm1729 = fVec13[(IOTA-9)&127];
			fTempPerm1730 = fVec13[(IOTA-7)&127];
			fTempPerm1731 = fVec13[(IOTA-6)&127];
			fTempPerm1732 = fVec13[(IOTA-3)&127];
			fTempPerm1733 = fVec13[(IOTA-61)&127];
			fTempPerm1734 = fVec13[(IOTA-10)&127];
			fTempPerm1735 = fVec14[(IOTA-125)&127];
			fTempPerm1736 = fVec13[(IOTA-12)&127];
			fTempPerm1737 = fVec13[(IOTA-11)&127];
			fTempPerm1738 = fVec14[(IOTA-118)&127];
			fTempPerm1739 = fVec14[(IOTA-117)&127];
			fTempPerm1740 = fVec14[(IOTA-113)&127];
			fTempPerm1741 = fVec14[(IOTA-112)&127];
			fTempPerm1742 = fVec14[(IOTA-109)&127];
			fTempPerm1743 = fVec14[(IOTA-67)&127];
			fTempPerm1744 = fVec14[(IOTA-66)&127];
			fTempPerm1745 = fVec14[(IOTA-65)&127];
			fTempPerm1746 = fVec14[(IOTA-54)&127];
			fTempPerm1747 = fVec14[(IOTA-24)&127];
			fTempPerm1748 = fVec14[(IOTA-25)&127];
			fTempPerm1749 = fVec14[(IOTA-31)&127];
			fTempPerm1750 = fVec14[(IOTA-23)&127];
			fTempPerm1751 = fVec3[(IOTA-95)&127];
			fTempPerm1752 = fVec3[(IOTA-72)&127];
			fTempPerm1753 = fVec3[(IOTA-71)&127];
			fTempPerm1754 = fVec3[(IOTA-37)&127];
			fTempPerm1755 = fVec3[(IOTA-30)&127];
			fTempPerm1756 = fVec3[(IOTA-33)&127];
			fTempPerm1757 = fVec3[(IOTA-23)&127];
			fTempPerm1758 = fVec4[(IOTA-114)&127];
			fTempPerm1759 = fVec4[(IOTA-113)&127];
			fTempPerm1760 = fVec4[(IOTA-109)&127];
			fTempPerm1761 = fVec4[(IOTA-98)&127];
			fTempPerm1762 = fVec4[(IOTA-95)&127];
			fTempPerm1763 = fVec4[(IOTA-78)&127];
			fTempPerm1764 = fVec4[(IOTA-73)&127];
			fTempPerm1765 = fVec4[(IOTA-56)&127];
			fTempPerm1766 = fVec4[(IOTA-41)&127];
			fTempPerm1767 = fVec5[(IOTA-124)&127];
			fTempPerm1768 = fVec5[(IOTA-110)&127];
			fTempPerm1769 = fVec5[(IOTA-81)&127];
			fTempPerm1770 = fVec5[(IOTA-93)&127];
			fTempPerm1771 = fVec5[(IOTA-77)&127];
			fTempPerm1772 = fVec3[(IOTA-29)&127];
			fTempPerm1773 = fVec5[(IOTA-36)&127];
			fTempPerm1774 = fVec6[(IOTA-112)&127];
			fTempPerm1775 = fVec6[(IOTA-114)&127];
			fTempPerm1776 = fVec6[(IOTA-110)&127];
			fTempPerm1777 = fVec6[(IOTA-107)&127];
			fTempPerm1778 = fVec6[(IOTA-109)&127];
			fTempPerm1779 = fVec6[(IOTA-55)&127];
			fTempPerm1780 = fVec6[(IOTA-56)&127];
			fTempPerm1781 = fVec6[(IOTA-38)&127];
			fTempPerm1782 = fVec6[(IOTA-37)&127];
			fTempPerm1783 = fVec7[(IOTA-115)&127];
			fTempPerm1784 = fVec7[(IOTA-111)&127];
			fTempPerm1785 = fVec7[(IOTA-90)&127];
			fTempPerm1786 = fVec7[(IOTA-92)&127];
			fTempPerm1787 = fVec7[(IOTA-83)&127];
			fTempPerm1788 = fVec7[(IOTA-82)&127];
			fTempPerm1789 = fVec7[(IOTA-44)&127];
			fTempPerm1790 = fVec7[(IOTA-80)&127];
			fTempPerm1791 = fVec7[(IOTA-43)&127];
			fTempPerm1792 = fVec8[(IOTA-124)&127];
			fTempPerm1793 = fVec8[(IOTA-119)&127];
			fTempPerm1794 = fVec8[(IOTA-115)&127];
			fTempPerm1795 = fVec8[(IOTA-69)&127];
			fTempPerm1796 = fVec8[(IOTA-92)&127];
			fTempPerm1797 = fVec8[(IOTA-89)&127];
			fTempPerm1798 = fVec8[(IOTA-65)&127];
			fTempPerm1799 = fVec8[(IOTA-48)&127];
			fTempPerm1800 = fVec8[(IOTA-46)&127];
			fTempPerm1801 = fVec9[(IOTA-114)&127];
			fTempPerm1802 = fVec9[(IOTA-103)&127];
			fTempPerm1803 = fVec9[(IOTA-80)&127];
			fTempPerm1804 = fVec9[(IOTA-74)&127];
			fTempPerm1805 = fVec9[(IOTA-73)&127];
			fTempPerm1806 = fVec9[(IOTA-69)&127];
			fTempPerm1807 = fVec9[(IOTA-43)&127];
			fTempPerm1808 = fVec9[(IOTA-38)&127];
			fTempPerm1809 = fVec9[(IOTA-31)&127];
			fTempPerm1810 = fVec9[(IOTA-27)&127];
			fTempPerm1811 = fVec10[(IOTA-117)&127];
			fTempPerm1812 = fVec9[(IOTA-11)&127];
			fTempPerm1813 = fVec10[(IOTA-111)&127];
			fTempPerm1814 = fVec10[(IOTA-37)&127];
			fTempPerm1815 = fVec10[(IOTA-46)&127];
			fTempPerm1816 = fVec10[(IOTA-34)&127];
			fTempPerm1817 = fVec10[(IOTA-26)&127];
			fTempPerm1818 = fVec10[(IOTA-25)&127];
			fTempPerm1819 = fVec10[(IOTA-19)&127];
			fTempPerm1820 = fVec10[(IOTA-18)&127];
			fTempPerm1821 = fVec10[(IOTA-16)&127];
			fTempPerm1822 = fVec10[(IOTA-15)&127];
			fTempPerm1823 = fVec10[(IOTA-12)&127];
			fTempPerm1824 = fVec10[(IOTA-11)&127];
			fTempPerm1825 = fVec10[(IOTA-9)&127];
			fTempPerm1826 = fVec10[(IOTA-7)&127];
			fTempPerm1827 = fVec10[(IOTA-3)&127];
			fTempPerm1828 = fVec11[(IOTA-64)&127];
			fTempPerm1829 = fVec11[(IOTA-60)&127];
			fTempPerm1830 = fVec11[(IOTA-54)&127];
			fTempPerm1831 = fVec11[(IOTA-53)&127];
			fTempPerm1832 = fVec11[(IOTA-12)&127];
			fTempPerm1833 = fVec11[(IOTA-11)&127];
			fTempPerm1834 = fVec11[(IOTA-9)&127];
			fTempPerm1835 = fVec11[(IOTA-7)&127];
			fTempPerm1836 = fVec11[(IOTA-5)&127];
			fTempPerm1837 = fVec11[(IOTA-3)&127];
			fTempPerm1838 = fVec12[(IOTA-118)&127];
			fTempPerm1839 = fVec12[(IOTA-112)&127];
			fTempPerm1840 = fVec12[(IOTA-88)&127];
			fTempPerm1841 = fVec12[(IOTA-109)&127];
			fTempPerm1842 = fVec12[(IOTA-80)&127];
			fTempPerm1843 = fVec12[(IOTA-78)&127];
			fTempPerm1844 = fVec12[(IOTA-77)&127];
			fTempPerm1845 = fVec12[(IOTA-71)&127];
			fTempPerm1846 = fVec12[(IOTA-81)&127];
			fTempPerm1847 = fVec12[(IOTA-59)&127];
			fTempPerm1848 = fVec12[(IOTA-58)&127];
			fTempPerm1849 = fVec12[(IOTA-24)&127];
			fTempPerm1850 = fVec12[(IOTA-25)&127];
			fTempPerm1851 = fVec13[(IOTA-109)&127];
			fTempPerm1852 = fVec13[(IOTA-104)&127];
			fTempPerm1853 = fVec13[(IOTA-101)&127];
			fTempPerm1854 = fVec13[(IOTA-82)&127];
			fTempPerm1855 = fVec14[(IOTA-119)&127];
			fTempPerm1856 = fVec14[(IOTA-116)&127];
			fTempPerm1857 = fVec14[(IOTA-115)&127];
			fTempPerm1858 = fVec14[(IOTA-75)&127];
			fTempPerm1859 = fVec14[(IOTA-70)&127];
			fTempPerm1860 = fVec14[(IOTA-62)&127];
			fTempPerm1861 = fVec14[(IOTA-51)&127];
			fTempPerm1862 = fVec14[(IOTA-46)&127];
			fTempPerm1863 = fVec14[(IOTA-68)&127];
			fTempPerm1864 = fVec14[(IOTA-63)&127];
			fTempPerm1865 = fVec14[(IOTA-45)&127];
			fTempPerm1866 = fVec15[(IOTA-92)&127];
			fTempPerm1867 = fVec15[(IOTA-91)&127];
			fTempPerm1868 = fVec3[(IOTA-1)&127];
			fTempPerm1869 = fVec4[(IOTA-1)&127];
			fTempPerm1870 = fVec5[(IOTA-1)&127];
			fTempPerm1871 = fVec6[(IOTA-1)&127];
			fTempPerm1872 = fVec14[(IOTA-27)&127];
			fTempPerm1873 = fVec14[(IOTA-26)&127];
			fTempPerm1874 = fVec15[(IOTA-97)&127];
			fTempPerm1875 = fVec15[(IOTA-84)&127];
			fTempPerm1876 = fVec15[(IOTA-47)&127];
			fTempPerm1877 = fVec15[(IOTA-28)&127];
			fTempPerm1878 = fVec15[(IOTA-24)&127];
			fTempPerm1879 = fVec10[(IOTA-82)&127];
			fTempPerm1880 = fVec10[(IOTA-81)&127];
			fTempPerm1881 = fVec10[(IOTA-80)&127];
			fTempPerm1882 = fVec10[(IOTA-79)&127];
			fTempPerm1883 = fVec10[(IOTA-68)&127];
			fTempPerm1884 = fVec10[(IOTA-56)&127];
			fTempPerm1885 = fVec10[(IOTA-50)&127];
			fTempPerm1886 = fVec11[(IOTA-118)&127];
			fTempPerm1887 = fVec11[(IOTA-115)&127];
			fTempPerm1888 = fVec10[(IOTA-69)&127];
			fTempPerm1889 = fVec11[(IOTA-91)&127];
			fTempPerm1890 = fVec11[(IOTA-88)&127];
			fTempPerm1891 = fVec11[(IOTA-90)&127];
			fTempPerm1892 = fVec11[(IOTA-77)&127];
			fTempPerm1893 = fVec11[(IOTA-72)&127];
			fTempPerm1894 = fVec11[(IOTA-73)&127];
			fTempPerm1895 = fVec11[(IOTA-47)&127];
			fTempPerm1896 = fVec11[(IOTA-46)&127];
			fTempPerm1897 = fVec11[(IOTA-41)&127];
			fTempPerm1898 = fVec11[(IOTA-33)&127];
			fTempPerm1899 = fVec12[(IOTA-105)&127];
			fTempPerm1900 = fVec11[(IOTA-34)&127];
			fTempPerm1901 = fVec12[(IOTA-124)&127];
			fTempPerm1902 = fVec12[(IOTA-104)&127];
			fTempPerm1903 = fVec12[(IOTA-103)&127];
			fTempPerm1904 = fVec12[(IOTA-102)&127];
			fTempPerm1905 = fVec12[(IOTA-111)&127];
			fTempPerm1906 = fVec11[(IOTA-32)&127];
			fTempPerm1907 = fVec12[(IOTA-99)&127];
			fTempPerm1908 = fVec12[(IOTA-98)&127];
			fTempPerm1909 = fVec12[(IOTA-93)&127];
			fTempPerm1910 = fVec12[(IOTA-82)&127];
			fTempPerm1911 = fVec12[(IOTA-73)&127];
			fTempPerm1912 = fVec12[(IOTA-69)&127];
			fTempPerm1913 = fVec12[(IOTA-32)&127];
			fTempPerm1914 = fVec12[(IOTA-31)&127];
			fTempPerm1915 = fVec12[(IOTA-28)&127];
			fTempPerm1916 = fVec12[(IOTA-27)&127];
			fTempPerm1917 = fVec12[(IOTA-92)&127];
			fTempPerm1918 = fVec13[(IOTA-112)&127];
			fTempPerm1919 = fVec13[(IOTA-108)&127];
			fTempPerm1920 = fVec12[(IOTA-70)&127];
			fTempPerm1921 = fVec12[(IOTA-76)&127];
			fTempPerm1922 = fVec12[(IOTA-74)&127];
			fTempPerm1923 = fVec13[(IOTA-91)&127];
			fTempPerm1924 = fVec13[(IOTA-71)&127];
			fTempPerm1925 = fVec13[(IOTA-67)&127];
			fTempPerm1926 = fVec13[(IOTA-68)&127];
			fTempPerm1927 = fVec13[(IOTA-34)&127];
			fTempPerm1928 = fVec13[(IOTA-56)&127];
			fTempPerm1929 = fVec13[(IOTA-57)&127];
			fTempPerm1930 = fVec13[(IOTA-16)&127];
			fTempPerm1931 = fVec14[(IOTA-122)&127];
			fTempPerm1932 = fVec14[(IOTA-110)&127];
			fTempPerm1933 = fVec13[(IOTA-15)&127];
			fTempPerm1934 = fVec14[(IOTA-111)&127];
			fTempPerm1935 = fVec13[(IOTA-30)&127];
			fTempPerm1936 = fVec14[(IOTA-81)&127];
			fTempPerm1937 = fVec14[(IOTA-61)&127];
			fTempPerm1938 = fVec14[(IOTA-91)&127];
			fTempPerm1939 = fVec14[(IOTA-64)&127];
			fTempPerm1940 = fVec14[(IOTA-60)&127];
			fTempPerm1941 = fVec14[(IOTA-58)&127];
			fTempPerm1942 = fVec14[(IOTA-56)&127];
			fTempPerm1943 = fVec14[(IOTA-57)&127];
			fTempPerm1944 = fVec14[(IOTA-47)&127];
			fTempPerm1945 = fVec14[(IOTA-44)&127];
			fTempPerm1946 = fVec14[(IOTA-49)&127];
			fTempPerm1947 = fVec14[(IOTA-48)&127];
			fTempPerm1948 = fVec14[(IOTA-42)&127];
			fTempPerm1949 = fVec14[(IOTA-43)&127];
			fTempPerm1950 = fVec14[(IOTA-41)&127];
			fTempPerm1951 = fVec14[(IOTA-39)&127];
			fTempPerm1952 = fVec14[(IOTA-35)&127];
			fTempPerm1953 = fVec14[(IOTA-38)&127];
			fTempPerm1954 = fVec14[(IOTA-34)&127];
			fTempPerm1955 = fVec14[(IOTA-33)&127];
			fTempPerm1956 = fVec14[(IOTA-37)&127];
			fTempPerm1957 = fVec14[(IOTA-36)&127];
			fTempPerm1958 = fVec15[(IOTA-89)&127];
			fTempPerm1959 = fVec7[(IOTA-1)&127];
			fTempPerm1960 = fVec8[(IOTA-1)&127];
			fTempPerm1961 = fVec9[(IOTA-1)&127];
			fTempPerm1962 = fVec13[(IOTA-65)&127];
			fTempPerm1963 = fVec13[(IOTA-49)&127];
			fTempPerm1964 = fVec13[(IOTA-37)&127];
			fTempPerm1965 = fVec13[(IOTA-31)&127];
			fTempPerm1966 = fVec13[(IOTA-18)&127];
			fTempPerm1967 = fVec14[(IOTA-123)&127];
			fTempPerm1968 = fVec14[(IOTA-108)&127];
			fTempPerm1969 = fVec13[(IOTA-38)&127];
			fTempPerm1970 = fVec14[(IOTA-126)&127];
			fTempPerm1971 = fVec14[(IOTA-120)&127];
			fTempPerm1972 = fVec14[(IOTA-106)&127];
			fTempPerm1973 = fVec14[(IOTA-76)&127];
			fTempPerm1974 = fVec14[(IOTA-74)&127];
			fTempPerm1975 = fVec14[(IOTA-69)&127];
			fTempPerm1976 = fVec14[(IOTA-55)&127];
			fTempPerm1977 = fVec14[(IOTA-53)&127];
			fTempPerm1978 = fVec14[(IOTA-52)&127];
			fTempPerm1979 = fVec14[(IOTA-50)&127];
			fTempPerm1980 = fVec14[(IOTA-32)&127];
			fTempPerm1981 = fVec14[(IOTA-30)&127];
			fTempPerm1982 = fVec14[(IOTA-28)&127];
			fTempPerm1983 = fVec14[(IOTA-29)&127];
			fTempPerm1984 = fVec14[(IOTA-10)&127];
			fTempPerm1985 = fVec14[(IOTA-2)&127];
			fTempPerm1986 = fVec14[(IOTA-8)&127];
			fTempPerm1987 = fVec14[(IOTA-6)&127];
			fTempPerm1988 = fVec15[(IOTA-81)&127];
			fTempPerm1989 = fVec15[(IOTA-88)&127];
			fTempPerm1990 = fVec2[(IOTA-62)&127];
			fTempPerm1991 = fVec3[(IOTA-121)&127];
			fTempPerm1992 = fVec1[(IOTA-43)&127];
			fTempPerm1993 = fVec2[(IOTA-86)&127];
			fTempPerm1994 = fVec3[(IOTA-120)&127];
			fTempPerm1995 = fVec3[(IOTA-32)&127];
			fTempPerm1996 = fVec4[(IOTA-71)&127];
			fTempPerm1997 = fVec6[(IOTA-80)&127];
			fTempPerm1998 = fVec7[(IOTA-124)&127];
			fTempPerm1999 = fVec7[(IOTA-119)&127];
			fTempPerm2000 = fVec7[(IOTA-85)&127];
			fTempPerm2001 = fVec7[(IOTA-32)&127];
			fTempPerm2002 = fVec8[(IOTA-82)&127];
			fTempPerm2003 = fVec1[(IOTA-61)&127];
			fTempPerm2004 = fVec1[(IOTA-49)&127];
			fTempPerm2005 = fVec2[(IOTA-90)&127];
			fTempPerm2006 = fVec3[(IOTA-94)&127];
			fTempPerm2007 = fVec3[(IOTA-86)&127];
			fTempPerm2008 = fVec3[(IOTA-85)&127];
			fTempPerm2009 = fVec6[(IOTA-62)&127];
			fTempPerm2010 = fVec6[(IOTA-57)&127];
			fTempPerm2011 = fVec7[(IOTA-113)&127];
			fTempPerm2012 = fVec7[(IOTA-88)&127];
			fTempPerm2013 = fVec7[(IOTA-55)&127];
			fTempPerm2014 = fVec7[(IOTA-11)&127];
			fTempPerm2015 = fVec10[(IOTA-118)&127];
			fTempPerm2016 = fVec11[(IOTA-99)&127];
			fTempPerm2017 = fVec12[(IOTA-50)&127];
			fTempPerm2018 = fVec14[(IOTA-59)&127];
			fTempPerm2019 = fVec14[(IOTA-40)&127];
			fTempPerm2020 = fVec3[(IOTA-123)&127];
			fTempPerm2021 = fVec5[(IOTA-69)&127];
			fTempPerm2022 = fVec7[(IOTA-116)&127];
			fTempPerm2023 = fVec7[(IOTA-100)&127];
			fTempPerm2024 = fVec7[(IOTA-49)&127];
			fTempPerm2025 = fVec7[(IOTA-31)&127];
			fTempPerm2026 = fVec12[(IOTA-40)&127];
			fTempPerm2027 = fVec13[(IOTA-27)&127];
			fTempPerm2028 = fVec13[(IOTA-25)&127];
			fTempPerm2029 = fVec13[(IOTA-23)&127];
			fTempPerm2030 = fVec15[(IOTA-2)&127];
			fTempPerm2031 = fVec0[(IOTA-83)&127];
			fTempPerm2032 = fVec4[(IOTA-66)&127];
			fTempPerm2033 = fVec6[(IOTA-111)&127];
			fTempPerm2034 = fVec10[(IOTA-122)&127];
			fTempPerm2035 = fVec11[(IOTA-109)&127];
			fTempPerm2036 = fVec8[(IOTA-78)&127];
			fTempPerm2037 = fVec0[(IOTA-1)&127];
			fTempPerm2038 = fVec14[(IOTA-121)&127];
			fTempPerm2039 = fVec14[(IOTA-124)&127];
			fTempPerm2040 = fVec13[(IOTA-64)&127];
			fTempPerm2041 = fVec13[(IOTA-76)&127];
			fTempPerm2042 = fVec13[(IOTA-81)&127];
			fTempPerm2043 = fVec13[(IOTA-110)&127];
			fTempPerm2044 = fVec2[(IOTA-38)&127];
			fTempPerm2045 = fVec9[(IOTA-15)&127];
			fTempPerm2046 = fVec8[(IOTA-4)&127];
			fTempPerm2047 = fVec8[(IOTA-5)&127];
			fTempPerm2048 = (fRec1[0] * ((((1.22111e-06 * fTempPerm2) + ((2.6002200000000003e-05 * fTempPerm3) + ((0.00031062500000000003 * fTempPerm4) + ((8.30153e-05 * fTempPerm5) + ((0.00113353 * fTempPerm6) + ((0.000438634 * fTempPerm7) + ((0.0018594100000000001 * fTempPerm8) + ((0.0034922200000000003 * fTempPerm9) + ((0.00314139 * fTempPerm10) + ((0.00127944 * fTempPerm11) + ((0.00164227 * fTempPerm12) + ((0.00688927 * fTempPerm13) + ((0.000576861 * fTempPerm16) + ((0.000614408 * fTempPerm17) + ((0.000728026 * fTempPerm18) + ((0.0021457000000000004 * fTempPerm19) + ((0.0008025430000000001 * fTempPerm20) + ((0.0006540630000000001 * fTempPerm21) + ((0.000140689 * fTempPerm22) + ((0.000623596 * fTempPerm23) + ((0.00192579 * fTempPerm24) + ((0.00102788 * fTempPerm25) + ((0.00394411 * fTempPerm26) + ((0.00976594 * fTempPerm27) + ((0.007637 * fTempPerm28) + ((0.015424400000000001 * fTempPerm29) + ((0.014135400000000001 * fTempPerm30) + ((0.00118495 * fTempPerm31) + ((0.0007035520000000001 * fTempPerm32) + ((0.000133299 * fTempPerm33) + ((1.38519e-05 * fTempPerm36) + ((0.00014199 * fTempPerm37) + ((9.84972e-05 * fTempPerm38) + ((0.000185267 * fTempPerm39) + ((0.000354246 * fTempPerm40) + ((0.000627893 * fTempPerm41) + ((0.000344298 * fTempPerm42) + ((0.0005856910000000001 * fTempPerm43) + ((0.000507438 * fTempPerm44) + ((0.0005648830000000001 * fTempPerm45) + ((0.000105676 * fTempPerm46) + ((0.00300284 * fTempPerm47) + ((7.685440000000001e-05 * fTempPerm48) + ((0.008349130000000001 * fTempPerm49) + ((3.25173e-05 * fTempPerm52) + ((7.679690000000001e-05 * fTempPerm53) + ((0.00173953 * fTempPerm54) + ((1.40342e-06 * fTempPerm55) + ((1.26129e-05 * fTempPerm56) + ((0.0027205 * fTempPerm57) + ((0.00050057 * fTempPerm58) + ((0.00204468 * fTempPerm59) + ((0.000423161 * fTempPerm60) + ((0.000557951 * fTempPerm61) + ((0.00110392 * fTempPerm62) + ((0.0039352 * fTempPerm63) + ((0.00132128 * fTempPerm64) + ((1.2691900000000001e-05 * fTempPerm65) + ((0.00117306 * fTempPerm66) + ((0.00015330900000000002 * fTempPerm67) + ((0.00154597 * fTempPerm68) + ((0.000178293 * fTempPerm69) + ((0.0025198 * fTempPerm70) + ((0.00117113 * fTempPerm71) + ((0.000455995 * fTempPerm72) + ((0.00278239 * fTempPerm73) + ((0.00180637 * fTempPerm74) + ((0.00736302 * fTempPerm75) + ((0.0168631 * fTempPerm76) + ((0.00872424 * fTempPerm77) + ((2.9823200000000003e-05 * fTempPerm78) + ((0.000377329 * fTempPerm79) + ((0.000856978 * fTempPerm80) + ((0.0006452280000000001 * fTempPerm81) + ((0.00192226 * fTempPerm82) + ((0.00225201 * fTempPerm83) + ((0.0028435500000000002 * fTempPerm84) + ((4.2787900000000004e-05 * fTempPerm85) + ((0.0035765 * fTempPerm86) + ((0.0050062100000000005 * fTempPerm87) + ((0.00604743 * fTempPerm88) + ((0.011238600000000001 * fTempPerm89) + ((2.64155e-05 * fTempPerm90) + ((0.000286919 * fTempPerm91) + ((0.00219127 * fTempPerm92) + ((0.0024016700000000003 * fTempPerm93) + ((0.000586933 * fTempPerm94) + ((0.0052284 * fTempPerm95) + ((0.00531055 * fTempPerm96) + ((0.0025010699999999998 * fTempPerm97) + ((0.031043400000000002 * fTempPerm98) + ((0.00121804 * fTempPerm99) + ((0.026438899999999998 * fTempPerm100) + ((2.9040900000000002e-05 * fTempPerm101) + ((3.33905e-05 * fTempPerm102) + ((0.0016346700000000002 * fTempPerm103) + ((0.00038062800000000004 * fTempPerm104) + ((0.00067549 * fTempPerm105) + ((9.1395e-05 * fTempPerm106) + ((0.0005823470000000001 * fTempPerm107) + ((0.000279699 * fTempPerm108) + ((0.000394012 * fTempPerm109) + ((0.000615073 * fTempPerm110) + ((0.00214308 * fTempPerm111) + ((0.004987910000000001 * fTempPerm112) + ((0.0100301 * fTempPerm113) + ((0.00539354 * fTempPerm114) + ((0.0366919 * fTempPerm115) + ((0.0113579 * fTempPerm116) + ((0.00373188 * fTempPerm117) + ((0.0010972500000000001 * fTempPerm118) + ((6.821750000000001e-05 * fTempPerm119) + ((0.000512512 * fTempPerm120) + ((0.000462645 * fTempPerm121) + ((0.0006112590000000001 * fTempPerm122) + ((0.000566156 * fTempPerm123) + ((0.000146092 * fTempPerm124) + ((0.000122237 * fTempPerm125) + ((0.000710064 * fTempPerm126) + ((0.0022978399999999998 * fTempPerm127) + ((0.0019625 * fTempPerm128) + ((0.00594723 * fTempPerm129) + ((0.000258005 * fTempPerm130) + ((0.00645056 * fTempPerm131) + ((0.0152522 * fTempPerm132) + ((0.0102524 * fTempPerm133) + ((0.0441556 * fTempPerm134) + ((0.00819548 * fTempPerm135) + ((0.014772 * fTempPerm136) + ((0.00406018 * fTempPerm137) + ((0.0196517 * fTempPerm138) + ((0.009802130000000001 * fTempPerm139) + ((0.00308077 * fTempPerm140) + ((0.00323953 * fTempPerm141) + ((0.00121235 * fTempPerm142) + ((0.000682276 * fTempPerm143) + ((2.8177200000000002e-05 * fTempPerm144) + ((1.2893400000000001e-05 * fTempPerm145) + ((0.0022225799999999996 * fTempPerm146) + ((7.3607e-05 * fTempPerm147) + ((0.00101481 * fTempPerm148) + ((0.00041671500000000006 * fTempPerm149) + ((0.0006386580000000001 * fTempPerm150) + ((0.00123933 * fTempPerm151) + ((0.00289419 * fTempPerm152) + ((0.006529200000000001 * fTempPerm153) + ((0.0264857 * fTempPerm154) + ((0.009431110000000001 * fTempPerm155) + ((0.0253549 * fTempPerm156) + ((0.0140976 * fTempPerm157) + ((0.0346439 * fTempPerm158) + ((2.78184e-05 * fTempPerm159) + ((4.46206e-07 * fTempPerm160) + ((0.000591619 * fTempPerm161) + ((0.00259752 * fTempPerm162) + ((0.000755146 * fTempPerm163) + ((0.00162886 * fTempPerm164) + ((0.000507685 * fTempPerm165) + ((0.000150399 * fTempPerm166) + ((0.00436414 * fTempPerm167) + ((0.011156999999999999 * fTempPerm168) + ((0.0126468 * fTempPerm169) + ((0.00736515 * fTempPerm170) + ((0.0011752499999999999 * fTempPerm171) + ((0.022163 * fTempPerm172) + ((0.00711267 * fTempPerm173) + ((0.00130168 * fTempPerm174) + ((0.018755900000000002 * fTempPerm175) + ((0.0137094 * fTempPerm176) + ((0.0240516 * fTempPerm177) + ((0.0157246 * fTempPerm178) + ((5.9634000000000005e-05 * fTempPerm179) + ((0.000617358 * fTempPerm180) + ((7.05634e-05 * fTempPerm181) + ((8.04918e-05 * fTempPerm182) + ((0.000377714 * fTempPerm183) + ((0.00037104300000000005 * fTempPerm184) + ((0.00041838800000000006 * fTempPerm185) + ((0.00107274 * fTempPerm186) + ((0.000159857 * fTempPerm187) + ((0.000190946 * fTempPerm188) + ((8.150900000000001e-05 * fTempPerm189) + ((0.0008022950000000001 * fTempPerm190) + ((0.000335637 * fTempPerm191) + ((0.000410223 * fTempPerm192) + ((0.0022562899999999998 * fTempPerm193) + ((0.000321723 * fTempPerm194) + ((0.0024381200000000002 * fTempPerm195) + ((0.00239619 * fTempPerm196) + ((0.000580731 * fTempPerm197) + ((0.0009067840000000001 * fTempPerm198) + ((0.00035913 * fTempPerm199) + ((0.000339349 * fTempPerm200) + ((2.5817700000000002e-05 * fTempPerm201) + ((0.0010270300000000002 * fTempPerm202) + ((0.0008419320000000001 * fTempPerm203) + ((0.0008762980000000001 * fTempPerm204) + ((0.00230851 * fTempPerm205) + ((0.002405 * fTempPerm206) + ((0.000715895 * fTempPerm207) + ((0.0028319 * fTempPerm208) + ((0.0012767 * fTempPerm209) + ((0.00165568 * fTempPerm210) + ((0.00232054 * fTempPerm211) + ((0.000134045 * fTempPerm214) + ((0.000144139 * fTempPerm215) + ((0.000745986 * fTempPerm216) + ((0.000727429 * fTempPerm217) + ((0.0009564700000000001 * fTempPerm218) + ((0.000624043 * fTempPerm219) + ((0.00111031 * fTempPerm220) + ((0.000167915 * fTempPerm221) + ((0.00029155500000000004 * fTempPerm222) + ((0.0019467900000000001 * fTempPerm223) + ((0.00139101 * fTempPerm224) + ((0.00281839 * fTempPerm225) + ((3.0540600000000004e-05 * fTempPerm228) + ((0.00011347700000000001 * fTempPerm229) + ((0.0005154550000000001 * fTempPerm230) + ((0.000654081 * fTempPerm231) + ((7.54347e-05 * fTempPerm232) + ((0.000329795 * fTempPerm233) + ((2.9296e-06 * fTempPerm234) + ((9.942030000000001e-05 * fTempPerm235) + ((0.000164362 * fTempPerm236) + ((0.00105312 * fTempPerm237) + ((0.000436437 * fTempPerm238) + ((0.000284002 * fTempPerm239) + ((0.00033464100000000005 * fTempPerm240) + ((0.00594912 * fTempPerm241) + ((1.1716500000000002e-05 * fTempPerm242) + ((1.6095900000000003e-05 * fTempPerm243) + ((6.90663e-05 * fTempPerm244) + ((0.000418061 * fTempPerm245) + ((0.00037591 * fTempPerm246) + ((0.0009170540000000002 * fTempPerm247) + ((0.0010226500000000002 * fTempPerm248) + ((0.00105746 * fTempPerm249) + ((0.0008985989999999999 * fTempPerm250) + ((0.00226954 * fTempPerm251) + ((0.0017599800000000002 * fTempPerm252) + ((0.000143156 * fTempPerm253) + ((0.008659739999999999 * fTempPerm254) + ((0.00901863 * fTempPerm255) + ((0.00031685400000000004 * fTempPerm256) + ((0.00695498 * fTempPerm257) + ((0.0012819700000000001 * fTempPerm258) + ((0.004685590000000001 * fTempPerm259) + ((0.0112361 * fTempPerm260) + ((0.00594332 * fTempPerm261) + ((0.029706100000000003 * fTempPerm262) + ((0.011985200000000001 * fTempPerm263) + ((0.00355895 * fTempPerm264) + ((1.3822700000000001e-06 * fTempPerm265) + ((8.18749e-07 * fTempPerm266) + ((4.995890000000001e-05 * fTempPerm267) + ((4.887430000000001e-05 * fTempPerm268) + ((8.945570000000001e-05 * fTempPerm269) + ((1.0304700000000001e-05 * fTempPerm270) + ((0.000143957 * fTempPerm271) + ((0.00224032 * fTempPerm272) + ((0.0013291 * fTempPerm273) + ((0.000472243 * fTempPerm274) + ((0.000346059 * fTempPerm275) + ((0.00166238 * fTempPerm276) + ((0.00496274 * fTempPerm277) + ((0.00034213 * fTempPerm278) + ((0.006992460000000001 * fTempPerm279) + ((0.00172897 * fTempPerm280) + ((0.004667119999999999 * fTempPerm281) + ((0.00742784 * fTempPerm282) + ((0.00384314 * fTempPerm283) + ((0.001851 * fTempPerm284) + ((0.00023100900000000004 * fTempPerm285) + ((0.008648379999999999 * fTempPerm286) + ((0.0347789 * fTempPerm287) + ((0.00336605 * fTempPerm288) + ((0.0132495 * fTempPerm289) + ((0.0166362 * fTempPerm290) + ((0.0149275 * fTempPerm291) + ((0.00153611 * fTempPerm292) + ((0.000223801 * fTempPerm293) + ((0.0002062 * fTempPerm294) + ((0.00029508100000000004 * fTempPerm295) + ((9.31576e-05 * fTempPerm296) + ((0.000831508 * fTempPerm297) + ((0.000139085 * fTempPerm298) + ((0.000100796 * fTempPerm299) + ((6.81638e-05 * fTempPerm302) + ((0.000373165 * fTempPerm303) + ((0.000392463 * fTempPerm304) + ((2.3925e-06 * fTempPerm305) + ((0.00246217 * fTempPerm306) + ((0.000690762 * fTempPerm307) + ((0.000270084 * fTempPerm308) + ((0.00311041 * fTempPerm309) + ((0.00148653 * fTempPerm310) + ((0.008452 * fTempPerm311) + ((0.00210043 * fTempPerm312) + ((1.06705e-06 * fTempPerm315) + ((0.0022532999999999997 * fTempPerm316) + ((0.000535477 * fTempPerm317) + ((0.00089492 * fTempPerm318) + ((0.000985821 * fTempPerm319) + ((0.00124406 * fTempPerm320) + ((0.00185354 * fTempPerm321) + ((0.00241144 * fTempPerm322) + ((0.00287405 * fTempPerm323) + ((0.000214717 * fTempPerm324) + ((0.0011606799999999999 * fTempPerm325) + ((0.000144938 * fTempPerm326) + ((1.0279e-06 * fTempPerm327) + ((2.773e-06 * fTempPerm330) + ((0.0101911 * fTempPerm331) + ((0.00272871 * fTempPerm332) + ((0.000142212 * fTempPerm333) + ((0.000253833 * fTempPerm334) + ((0.0005151 * fTempPerm335) + ((0.00127749 * fTempPerm336) + ((6.29365e-05 * fTempPerm337) + ((0.000662261 * fTempPerm338) + ((0.0027539400000000003 * fTempPerm339) + ((0.0013761300000000001 * fTempPerm340) + ((0.0021549399999999997 * fTempPerm341) + ((0.0030387 * fTempPerm342) + ((0.0071841 * fTempPerm343) + ((0.000278137 * fTempPerm344) + ((0.00027819 * fTempPerm345) + ((0.000339552 * fTempPerm346) + ((0.00022609 * fTempPerm347) + ((0.000160215 * fTempPerm348) + ((((5.2352700000000004e-05 * fTempPerm349) + ((3.0215200000000005e-05 * fTempPerm352) + ((0.000261474 * fTempPerm353) + ((0.00058267 * fTempPerm354) + ((0.00121078 * fTempPerm355) + ((0.00143222 * fTempPerm356) + ((0.000561379 * fTempPerm357) + ((0.000646904 * fTempPerm358) + ((0.000551147 * fTempPerm359) + ((0.00117567 * fTempPerm360) + ((0.00110439 * fTempPerm361) + ((0.0016155 * fTempPerm362) + ((0.00215115 * fTempPerm363) + ((0.0158006 * fTempPerm364) + ((0.0234635 * fTempPerm365) + ((0.0359968 * fTempPerm366) + ((0.013752100000000001 * fTempPerm367) + ((0.0018844 * fTempPerm368) + ((0.0448946 * fTempPerm369) + ((0.000331593 * fTempPerm370) + ((0.000946613 * fTempPerm371) + ((0.00101332 * fTempPerm372) + ((0.000257463 * fTempPerm373) + ((3.35889e-06 * fTempPerm376) + ((0.0056101400000000004 * fTempPerm377) + ((0.00238549 * fTempPerm378) + ((0.0005175220000000001 * fTempPerm379) + ((0.0185312 * fTempPerm380) + ((0.022452899999999998 * fTempPerm381) + ((0.0121954 * fTempPerm382) + ((0.0109563 * fTempPerm383) + ((0.00016957000000000002 * fTempPerm384) + ((0.00010783300000000001 * fTempPerm385) + ((0.00024862500000000004 * fTempPerm386) + ((0.000840711 * fTempPerm387) + ((9.0163e-05 * fTempPerm388) + ((0.000364899 * fTempPerm389) + ((3.07658e-05 * fTempPerm390) + ((7.12921e-05 * fTempPerm391) + ((0.00018952700000000002 * fTempPerm392) + ((2.03142e-05 * fTempPerm393) + ((0.000671795 * fTempPerm394) + ((0.00154983 * fTempPerm395) + ((0.00132314 * fTempPerm396) + ((0.00081182 * fTempPerm397) + ((0.00193557 * fTempPerm398) + ((0.000502031 * fTempPerm399) + ((0.00126208 * fTempPerm400) + ((0.000494544 * fTempPerm401) + ((0.000621383 * fTempPerm402) + ((7.126120000000001e-05 * fTempPerm403) + ((0.00199792 * fTempPerm404) + ((0.000841901 * fTempPerm405) + ((0.0196498 * fTempPerm406) + ((0.00351383 * fTempPerm407) + ((0.00532911 * fTempPerm408) + ((0.0244384 * fTempPerm409) + ((0.0667629 * fTempPerm410) + ((0.0107382 * fTempPerm411) + ((7.05084e-07 * fTempPerm412) + ((0.0101949 * fTempPerm413) + ((0.0262009 * fTempPerm414) + ((0.000145918 * fTempPerm415) + ((0.00094611 * fTempPerm416) + ((0.0006859300000000001 * fTempPerm417) + ((0.00147564 * fTempPerm418) + ((0.000611671 * fTempPerm419) + ((0.0014824 * fTempPerm420) + ((0.00147745 * fTempPerm421) + ((3.5723000000000003e-05 * fTempPerm422) + ((0.00171871 * fTempPerm423) + ((0.00204601 * fTempPerm424) + ((1.44848e-05 * fTempPerm425) + ((0.00173413 * fTempPerm426) + ((0.0034409700000000002 * fTempPerm427) + ((0.00598216 * fTempPerm428) + ((0.0067079800000000005 * fTempPerm429) + ((0.00793981 * fTempPerm430) + ((5.611290000000001e-05 * fTempPerm431) + ((4.87482e-05 * fTempPerm432) + ((0.0007838960000000001 * fTempPerm433) + ((0.0010638000000000002 * fTempPerm434) + ((0.00207133 * fTempPerm435) + ((0.0036506200000000003 * fTempPerm436) + ((0.0006661270000000001 * fTempPerm437) + ((0.0039108500000000004 * fTempPerm438) + ((0.000782042 * fTempPerm439) + ((0.00481083 * fTempPerm440) + ((0.0273854 * fTempPerm441) + ((0.0409783 * fTempPerm442) + ((0.049985600000000005 * fTempPerm443) + ((0.0387707 * fTempPerm444) + ((0.10758200000000001 * fTempPerm445) + ((0.021392099999999997 * fTempPerm446) + ((0.04177190000000001 * fTempPerm447) + ((0.0551774 * fTempPerm448) + ((0.0108086 * fTempPerm449) + ((0.0026607500000000004 * fTempPerm450) + ((0.000669039 * fTempPerm451) + ((0.00135568 * fTempPerm452) + ((0.000128073 * fTempPerm453) + ((0.000378556 * fTempPerm454) + ((2.84575e-06 * fTempPerm455) + ((5.2591200000000006e-05 * fTempPerm456) + ((0.00035608600000000003 * fTempPerm457) + ((0.000285357 * fTempPerm458) + ((0.00124614 * fTempPerm459) + ((0.000700973 * fTempPerm460) + ((0.000400805 * fTempPerm461) + ((0.021556799999999997 * fTempPerm462) + ((0.0235272 * fTempPerm463) + ((0.0021948500000000004 * fTempPerm464) + ((0.0005180720000000001 * fTempPerm465) + ((0.0035849099999999997 * fTempPerm466) + ((0.024944 * fTempPerm467) + ((0.011300600000000001 * fTempPerm468) + ((0.009433120000000001 * fTempPerm469) + ((8.963050000000002e-08 * fTempPerm470) + ((0.000208667 * fTempPerm471) + ((0.0007687070000000001 * fTempPerm472) + ((0.0012521400000000001 * fTempPerm473) + ((0.00122438 * fTempPerm474) + ((3.43292e-08 * fTempPerm475) + ((0.00020187200000000002 * fTempPerm476) + ((0.000506744 * fTempPerm477) + ((0.000692291 * fTempPerm478) + ((0.000380719 * fTempPerm479) + ((8.84199e-05 * fTempPerm480) + ((0.0005591420000000001 * fTempPerm481) + ((0.000219724 * fTempPerm482) + ((0.0017431900000000001 * fTempPerm483) + ((0.000292148 * fTempPerm484) + ((0.00914287 * fTempPerm485) + ((0.00998562 * fTempPerm486) + ((0.0109229 * fTempPerm487) + ((0.0034697200000000004 * fTempPerm488) + ((0.0030116200000000004 * fTempPerm489) + ((6.18201e-06 * fTempPerm492) + ((3.82797e-06 * fTempPerm493) + ((0.00036701 * fTempPerm494) + ((0.0005077079999999999 * fTempPerm495) + ((8.2866e-05 * fTempPerm496) + ((0.00018379600000000001 * fTempPerm497) + ((0.00014839199999999998 * fTempPerm498) + ((0.0008625190000000001 * fTempPerm499) + ((8.7317e-05 * fTempPerm500) + ((0.0013118799999999999 * fTempPerm501) + ((0.0014712700000000002 * fTempPerm502) + ((8.71167e-05 * fTempPerm503) + ((0.00160801 * fTempPerm504) + ((3.9296600000000006e-05 * fTempPerm507) + ((2.45331e-06 * fTempPerm508) + ((0.0005051960000000001 * fTempPerm509) + ((1.70273e-06 * fTempPerm510) + ((3.85491e-05 * fTempPerm511) + ((0.0014036900000000002 * fTempPerm512) + ((0.0006954530000000001 * fTempPerm513) + ((0.00146986 * fTempPerm514) + ((0.00681093 * fTempPerm515) + ((0.00814256 * fTempPerm516) + ((0.005367289999999999 * fTempPerm517) + ((0.00013874000000000002 * fTempPerm518) + ((1.73416e-05 * fTempPerm519) + ((0.00483835 * fTempPerm520) + ((0.0011593500000000002 * fTempPerm521) + ((8.88365e-06 * fTempPerm524) + ((9.081670000000001e-05 * fTempPerm525) + ((7.74527e-05 * fTempPerm526) + ((5.80835e-05 * fTempPerm527) + ((0.000324763 * fTempPerm528) + ((0.0032713800000000004 * fTempPerm529) + ((0.0008932800000000001 * fTempPerm530) + ((0.000721582 * fTempPerm531) + ((0.0016079900000000001 * fTempPerm532) + ((0.0020680200000000003 * fTempPerm533) + ((0.00059752 * fTempPerm534) + ((0.000582052 * fTempPerm535) + ((0.000465135 * fTempPerm536) + ((0.00231907 * fTempPerm537) + ((0.00187164 * fTempPerm538) + ((0.0004972959999999999 * fTempPerm539) + ((0.001716 * fTempPerm540) + ((0.00169851 * fTempPerm541) + ((0.00252236 * fTempPerm542) + ((0.0576913 * fTempPerm543) + ((0.016169199999999998 * fTempPerm544) + ((0.0446943 * fTempPerm545) + ((0.046224100000000004 * fTempPerm546) + ((0.0735245 * fTempPerm547) + ((0.0398189 * fTempPerm548) + ((0.030653800000000002 * fTempPerm549) + ((0.00283126 * fTempPerm550) + ((0.00023471600000000002 * fTempPerm551) + ((0.00166465 * fTempPerm552) + ((0.000189441 * fTempPerm553) + ((0.00041035500000000005 * fTempPerm554) + ((0.000571158 * fTempPerm555) + ((3.8319600000000005e-05 * fTempPerm556) + ((0.00316299 * fTempPerm557) + ((3.61728e-05 * fTempPerm558) + ((0.00833056 * fTempPerm559) + ((0.00160415 * fTempPerm560) + ((0.018119700000000002 * fTempPerm561) + ((2.4558300000000005e-05 * fTempPerm562) + ((0.00070562 * fTempPerm563) + ((0.00143667 * fTempPerm564) + ((0.000344038 * fTempPerm565) + ((0.0022713 * fTempPerm566) + ((2.0687900000000002e-05 * fTempPerm567) + ((3.8066800000000006e-05 * fTempPerm568) + ((0.0014662800000000001 * fTempPerm569) + ((0.00257166 * fTempPerm570) + ((0.00221665 * fTempPerm571) + ((0.000547636 * fTempPerm572) + ((0.002183 * fTempPerm573) + ((0.000204676 * fTempPerm574) + ((4.86084e-05 * fTempPerm575) + ((0.00045785700000000004 * fTempPerm576) + ((0.000135904 * fTempPerm577) + ((0.000307859 * fTempPerm578) + ((0.00048677700000000003 * fTempPerm579) + ((0.000204718 * fTempPerm580) + ((0.000337973 * fTempPerm581) + ((3.56328e-05 * fTempPerm582) + ((0.00046067700000000005 * fTempPerm583) + ((3.64663e-05 * fTempPerm584) + ((6.25862e-05 * fTempPerm585) + ((0.0019439 * fTempPerm586) + ((3.98048e-06 * fTempPerm587) + ((2.0105900000000003e-05 * fTempPerm588) + ((0.000754655 * fTempPerm589) + ((7.760860000000001e-05 * fTempPerm590) + ((0.000681816 * fTempPerm591) + ((0.0007193410000000001 * fTempPerm592) + ((0.00957015 * fTempPerm593) + ((0.00040184499999999996 * fTempPerm594) + ((0.0034494400000000002 * fTempPerm595) + ((0.00255904 * fTempPerm596) + ((0.0011545000000000001 * fTempPerm597) + ((0.00150508 * fTempPerm598) + ((0.0005897750000000001 * fTempPerm599) + ((0.00184624 * fTempPerm600) + ((0.0038023500000000003 * fTempPerm601) + ((0.00290687 * fTempPerm602) + ((0.0047945900000000005 * fTempPerm603) + ((2.51378e-07 * fTempPerm604) + ((0.000206941 * fTempPerm605) + ((5.25869e-05 * fTempPerm606) + ((0.000254261 * fTempPerm607) + ((7.614930000000001e-05 * fTempPerm608) + ((0.00129144 * fTempPerm609) + ((0.00047882 * fTempPerm610) + ((0.000351382 * fTempPerm611) + ((0.0105234 * fTempPerm612) + ((0.0034125200000000005 * fTempPerm613) + ((0.00691979 * fTempPerm614) + ((0.0316732 * fTempPerm615) + ((0.0274497 * fTempPerm616) + ((0.00309089 * fTempPerm617) + ((0.0103427 * fTempPerm618) + ((0.0120915 * fTempPerm619) + ((0.00205137 * fTempPerm620) + ((0.00160511 * fTempPerm621) + ((0.00018456300000000003 * fTempPerm622) + ((0.000562856 * fTempPerm623) + ((0.00337025 * fTempPerm624) + ((0.00527862 * fTempPerm625) + ((0.000367688 * fTempPerm626) + ((0.000101833 * fTempPerm627) + ((0.000105639 * fTempPerm628) + ((0.0007088390000000001 * fTempPerm629) + ((0.0008817050000000001 * fTempPerm630) + ((0.00359199 * fTempPerm631) + ((0.0012247 * fTempPerm632) + ((0.00571347 * fTempPerm633) + ((0.00336909 * fTempPerm634) + ((0.000702438 * fTempPerm635) + ((0.0174746 * fTempPerm636) + ((0.0211909 * fTempPerm637) + ((0.00253457 * fTempPerm638) + ((0.00069471 * fTempPerm639) + ((0.0025992000000000003 * fTempPerm640) + ((0.00618924 * fTempPerm641) + ((0.00201002 * fTempPerm642) + ((0.00148817 * fTempPerm643) + ((0.000534434 * fTempPerm644) + ((2.43757e-05 * fTempPerm645) + ((8.00668e-05 * fTempPerm646) + ((0.00119739 * fTempPerm647) + ((0.0043689900000000005 * fTempPerm648) + ((0.00375801 * fTempPerm649) + ((0.000383377 * fTempPerm650) + ((0.0019519 * fTempPerm651) + ((0.000259004 * fTempPerm652) + ((0.0016691100000000001 * fTempPerm653) + ((0.00130772 * fTempPerm654) + ((0.00214275 * fTempPerm655) + ((0.0022329500000000005 * fTempPerm656) + ((0.0028589500000000003 * fTempPerm657) + ((0.0197594 * fTempPerm658) + ((0.0121098 * fTempPerm659) + ((0.00165835 * fTempPerm660) + ((0.00031507000000000003 * fTempPerm661) + ((0.000494618 * fTempPerm662) + ((0.000158404 * fTempPerm663) + ((1.1796500000000001e-05 * fTempPerm666) + ((0.000419816 * fTempPerm667) + ((0.000709739 * fTempPerm668) + ((0.00046795 * fTempPerm669) + ((0.000884337 * fTempPerm670) + ((0.058508199999999996 * fTempPerm671) + ((0.00405293 * fTempPerm672) + ((0.0020737100000000003 * fTempPerm673) + ((0.000245352 * fTempPerm674) + ((0.0014007000000000002 * fTempPerm675) + ((0.000938082 * fTempPerm676) + ((0.00572338 * fTempPerm677) + ((0.005861290000000001 * fTempPerm678) + ((1.2353500000000001e-05 * fTempPerm679) + ((1.16687e-06 * fTempPerm680) + ((8.047479999999999e-06 * fTempPerm681) + ((5.0538800000000005e-05 * fTempPerm682) + ((0.000997294 * fTempPerm683) + ((0.0019079 * fTempPerm684) + ((0.00016474200000000003 * fTempPerm685) + ((0.0012639700000000001 * fTempPerm686) + ((0.0009538050000000001 * fTempPerm687) + ((0.000305641 * fTempPerm688) + ((0.000718359 * fTempPerm689) + ((0.0291912 * fTempPerm690) + ((0.000273018 * fTempPerm691) + ((0.000548452 * fTempPerm692) + ((0.0008838 * fTempPerm693) + ((0.00043581100000000003 * fTempPerm694) + ((0.0029536099999999997 * fTempPerm695) + ((0.00683453 * fTempPerm696) + ((0.00659946 * fTempPerm697) + ((4.32078e-06 * fTempPerm698) + ((0.0014493400000000001 * fTempPerm699) + ((0.000313016 * fTempPerm700) + ((0.00185503 * fTempPerm701) + ((5.59214e-05 * fTempPerm702) + ((2.1146600000000005e-05 * fTempPerm703) + ((3.75609e-05 * fTempPerm704) + ((0.00104006 * fTempPerm705) + ((3.8900100000000004e-05 * fTempPerm706) + ((0.00110409 * fTempPerm707) + ((0.000252183 * fTempPerm708) + ((0.000148299 * fTempPerm709) + ((0.00136597 * fTempPerm710) + ((0.00311967 * fTempPerm711) + ((0.00149633 * fTempPerm712) + ((0.00445085 * fTempPerm713) + ((0.00211746 * fTempPerm714) + ((0.0040522299999999995 * fTempPerm715) + ((0.00646392 * fTempPerm716) + ((4.34695e-05 * fTempPerm717) + ((7.264470000000001e-05 * fTempPerm718) + ((0.0054627 * fTempPerm719) + ((0.00534785 * fTempPerm720) + ((4.94959e-07 * fTempPerm721) + ((5.3135e-06 * fTempPerm722) + ((0.000320771 * fTempPerm723) + ((0.000247962 * fTempPerm724) + ((0.00010309 * fTempPerm725) + ((0.00018337600000000001 * fTempPerm726) + ((9.065480000000002e-05 * fTempPerm727) + ((0.0007114370000000001 * fTempPerm728) + ((0.000749201 * fTempPerm729) + ((0.00108661 * fTempPerm730) + ((0.00221192 * fTempPerm731) + ((0.00361819 * fTempPerm732) + ((2.7286800000000004e-05 * fTempPerm733) + ((0.00343243 * fTempPerm734) + ((0.00288148 * fTempPerm735) + ((0.0025276200000000004 * fTempPerm736) + ((0.00459555 * fTempPerm737) + ((0.000100847 * fTempPerm738) + ((0.00015598 * fTempPerm739) + ((0.0011506799999999998 * fTempPerm740) + ((0.00271718 * fTempPerm741) + ((0.0007649050000000001 * fTempPerm742) + ((0.00703939 * fTempPerm743) + ((0.0118865 * fTempPerm744) + ((0.005873929999999999 * fTempPerm745) + ((0.00489365 * fTempPerm746) + ((0.024751 * fTempPerm747) + ((0.00867306 * fTempPerm748) + ((0.0527583 * fTempPerm749) + ((5.07898e-05 * fTempPerm750) + ((0.0124978 * fTempPerm751) + ((0.036438700000000004 * fTempPerm752) + ((0.00020361699999999999 * fTempPerm753) + ((0.000300808 * fTempPerm754) + ((0.00040572 * fTempPerm755) + ((0.000530096 * fTempPerm756) + ((0.0006938150000000001 * fTempPerm757) + ((0.00133959 * fTempPerm758) + ((0.0026214800000000003 * fTempPerm759) + ((0.00162719 * fTempPerm760) + ((0.0038672100000000003 * fTempPerm761) + ((0.00601814 * fTempPerm762) + ((0.0006117130000000001 * fTempPerm763) + ((4.1971900000000004e-05 * fTempPerm764) + ((0.004904550000000001 * fTempPerm765) + ((0.0013329 * fTempPerm766) + ((0.0016006100000000001 * fTempPerm767) + ((0.00991536 * fTempPerm768) + ((0.013603 * fTempPerm769) + ((0.0167235 * fTempPerm770) + ((0.045127499999999994 * fTempPerm771) + ((0.00596596 * fTempPerm772) + ((0.0027371 * fTempPerm773) + ((4.61844e-05 * fTempPerm774) + ((0.0025209700000000004 * fTempPerm775) + ((0.000368557 * fTempPerm776) + ((0.000702765 * fTempPerm777) + ((0.000903587 * fTempPerm778) + ((0.000231406 * fTempPerm779) + ((8.214079999999998e-06 * fTempPerm780) + ((3.8470500000000004e-05 * fTempPerm781) + ((9.096330000000001e-05 * fTempPerm782) + ((0.000204282 * fTempPerm783) + ((0.00190242 * fTempPerm784) + ((0.00262382 * fTempPerm785) + ((0.0016339300000000002 * fTempPerm786) + ((0.00147938 * fTempPerm787) + ((0.00110736 * fTempPerm788) + ((0.00389043 * fTempPerm789) + ((0.00247242 * fTempPerm790) + ((0.0012886100000000001 * fTempPerm791) + ((0.0008485659999999999 * fTempPerm792) + ((0.000202228 * fTempPerm793) + ((3.882760000000001e-05 * fTempPerm794) + ((0.00154417 * fTempPerm795) + ((0.00206635 * fTempPerm796) + ((0.0019489700000000002 * fTempPerm797) + ((9.40654e-05 * fTempPerm798) + ((0.00341282 * fTempPerm799) + ((9.942610000000001e-05 * fTempPerm800) + ((0.00288332 * fTempPerm801) + ((0.00117636 * fTempPerm802) + ((0.00229129 * fTempPerm803) + ((3.3718100000000005e-05 * fTempPerm804) + (((((0.00243588 * fTempPerm805) + ((0.00046969700000000004 * fTempPerm806) + ((0.000922271 * fTempPerm807) + ((0.000623519 * fTempPerm808) + ((0.0016778 * fTempPerm809) + ((6.798480000000001e-05 * fTempPerm810) + ((0.00045975200000000007 * fTempPerm811) + ((0.00300031 * fTempPerm812) + ((0.000331327 * fTempPerm813) + ((2.3648600000000003e-05 * fTempPerm814) + ((0.0005879310000000001 * fTempPerm815) + ((2.06253e-06 * fTempPerm818) + ((4.2541300000000006e-05 * fTempPerm819) + ((0.000258553 * fTempPerm820) + ((0.00272945 * fTempPerm821) + ((0.0009436510000000001 * fTempPerm822) + ((0.0037083100000000003 * fTempPerm823) + ((0.005175760000000001 * fTempPerm824) + ((0.0029129100000000003 * fTempPerm825) + ((0.00308689 * fTempPerm826) + ((0.00225593 * fTempPerm827) + ((0.00623851 * fTempPerm828) + ((0.006804070000000001 * fTempPerm829) + ((0.0012118500000000002 * fTempPerm830) + ((0.00175234 * fTempPerm831) + ((0.00306785 * fTempPerm832) + ((0.00743299 * fTempPerm833) + ((0.0056945699999999995 * fTempPerm834) + ((0.0064973 * fTempPerm835) + ((0.00693019 * fTempPerm836) + ((0.00446807 * fTempPerm837) + ((0.0200236 * fTempPerm838) + ((0.09689510000000001 * fTempPerm839) + ((0.057809200000000005 * fTempPerm840) + ((0.06596840000000001 * fTempPerm841) + ((0.0557725 * fTempPerm842) + (((((4.0528500000000005e-06 * fVec3[IOTA&127]) + ((5.4869299999999995e-06 * fVec5[IOTA&127]) + ((0.000164044 * fTempPerm843) + ((0.000547366 * fTempPerm844) + ((0.0012923000000000001 * fTempPerm845) + ((0.00136699 * fTempPerm846) + ((0.0012065399999999999 * fTempPerm847) + ((0.00154024 * fTempPerm848) + ((0.000584702 * fTempPerm849) + ((0.0016799500000000001 * fTempPerm850) + ((0.00214615 * fTempPerm851) + ((0.00243653 * fTempPerm852) + ((0.00185049 * fTempPerm853) + ((0.0014647 * fTempPerm854) + ((0.00422506 * fTempPerm855) + ((0.004194580000000001 * fTempPerm856) + ((0.00637431 * fTempPerm857) + ((0.00500282 * fTempPerm858) + ((0.0028834 * fTempPerm859) + ((0.00366525 * fTempPerm860) + ((0.00288578 * fTempPerm861) + ((0.011589100000000001 * fTempPerm862) + ((0.0079812 * fTempPerm863) + ((0.00570825 * fTempPerm864) + ((0.011366300000000001 * fTempPerm865) + ((0.0141227 * fTempPerm866) + ((0.00870641 * fTempPerm867) + ((0.0266261 * fTempPerm868) + ((0.030624600000000002 * fTempPerm869) + ((0.0528391 * fTempPerm870) + ((0.0317463 * fTempPerm871) + ((0.0212609 * fTempPerm872) + ((2.8519800000000004e-05 * fTempPerm873) + ((4.45788e-06 * fVec13[IOTA&127]) + ((0.000260275 * fTempPerm874) + ((0.00046662400000000006 * fTempPerm875) + ((0.0008425430000000001 * fTempPerm876) + ((0.0017063 * fTempPerm877) + ((0.0005514300000000001 * fTempPerm878) + ((0.00167233 * fTempPerm879) + ((3.26233e-05 * fTempPerm880) + ((0.00127111 * fTempPerm881) + ((0.000647084 * fTempPerm882) + ((0.00156339 * fTempPerm883) + ((0.0010632999999999999 * fTempPerm884) + ((0.00356967 * fTempPerm885) + ((0.000554582 * fTempPerm886) + ((0.0014054500000000001 * fTempPerm887) + ((3.872240000000001e-05 * fTempPerm888) + ((0.000227166 * fTempPerm889) + ((2.11859e-06 * fTempPerm890) + ((2.72483e-05 * fTempPerm891) + ((8.327320000000001e-05 * fTempPerm892) + ((0.000632642 * fTempPerm893) + ((0.00010655300000000001 * fTempPerm894) + ((0.00155754 * fTempPerm895) + ((0.00116141 * fTempPerm896) + ((0.00196889 * fTempPerm897) + ((0.00798082 * fTempPerm898) + ((0.000208879 * fTempPerm899) + ((0.00444323 * fTempPerm900) + ((0.00131455 * fTempPerm901) + ((9.226730000000001e-05 * fTempPerm902) + ((0.0053560100000000005 * fTempPerm903) + ((0.0053044 * fTempPerm904) + ((0.00291413 * fTempPerm905) + ((0.0073240200000000005 * fTempPerm906) + ((0.010230900000000001 * fTempPerm907) + ((8.03417e-06 * fTempPerm908) + ((0.000184012 * fTempPerm909) + ((0.00732389 * fTempPerm910) + ((0.00317138 * fTempPerm911) + ((0.000682534 * fTempPerm912) + ((0.0010312 * fTempPerm913) + ((0.00113331 * fTempPerm914) + ((0.0016634199999999999 * fTempPerm915) + ((0.00967744 * fTempPerm916) + ((0.036282299999999996 * fTempPerm917) + ((0.03066 * fTempPerm918) + ((2.40821e-06 * fTempPerm919) + ((8.79674e-06 * fTempPerm920) + ((9.50248e-07 * fTempPerm921) + ((0.0012872900000000002 * fTempPerm922) + ((0.0036832600000000003 * fTempPerm923) + ((0.00028545500000000005 * fTempPerm924) + ((0.00169945 * fTempPerm925) + ((0.0008951980000000002 * fTempPerm926) + ((0.000791254 * fTempPerm927) + ((0.000995482 * fTempPerm928) + ((0.0164669 * fTempPerm929) + ((0.008648310000000001 * fTempPerm930) + ((0.00376608 * fTempPerm931) + ((0.000503397 * fTempPerm932) + ((0.000868029 * fTempPerm933) + ((0.0216851 * fTempPerm934) + ((0.0004770400000000001 * fTempPerm935) + ((0.00136595 * fTempPerm936) + ((0.000264467 * fTempPerm937) + ((0.0013671599999999999 * fTempPerm938) + ((0.021576300000000003 * fTempPerm939) + ((0.0016152100000000002 * fTempPerm940) + ((0.0004323680000000001 * fTempPerm941) + ((5.0813899999999995e-06 * fTempPerm942) + ((5.712609999999999e-06 * fTempPerm943) + ((0.0003416 * fTempPerm944) + ((0.000453824 * fTempPerm945) + ((0.00050102 * fTempPerm946) + ((0.0014059200000000002 * fTempPerm947) + ((0.0008772760000000001 * fTempPerm948) + ((0.0006128250000000001 * fTempPerm949) + ((0.00054874 * fTempPerm950) + ((2.3955000000000004e-05 * fTempPerm951) + ((0.00157017 * fTempPerm952) + ((0.000563692 * fTempPerm953) + ((0.0041905200000000005 * fTempPerm954) + ((0.0024405 * fTempPerm955) + ((0.000828477 * fTempPerm956) + ((0.00523563 * fTempPerm957) + ((0.00342846 * fTempPerm958) + ((0.0023089200000000003 * fTempPerm959) + ((0.0035775299999999998 * fTempPerm960) + ((0.00305273 * fTempPerm961) + ((0.00325793 * fTempPerm962) + ((0.0008637700000000001 * fTempPerm963) + ((0.00250454 * fTempPerm964) + ((0.015005800000000001 * fTempPerm965) + ((0.00792482 * fTempPerm966) + ((0.000123708 * fTempPerm967) + ((2.5118200000000003e-05 * fVec15[IOTA&127]) + ((4.2462999999999995e-06 * fVec8[IOTA&127]) + ((4.41782e-05 * fTempPerm968) + ((1.57811e-05 * fVec10[IOTA&127]) + ((2.00167e-05 * fTempPerm969) + ((1.4622100000000002e-05 * fVec11[IOTA&127]) + ((2.49699e-05 * fTempPerm970) + ((9.93628e-06 * fVec12[IOTA&127]) + ((0.0007790860000000001 * fTempPerm971) + ((0.00918714 * fTempPerm972) + ((0.00421731 * fTempPerm973) + ((0.0005342939999999999 * fTempPerm974) + ((0.00162146 * fTempPerm975) + ((0.00301503 * fTempPerm976) + ((0.0053105900000000004 * fTempPerm977) + ((0.0070971 * fTempPerm978) + ((0.000705385 * fTempPerm979) + ((0.000813006 * fTempPerm980) + ((0.00108099 * fTempPerm981) + ((0.00214314 * fTempPerm982) + ((0.0013345100000000001 * fTempPerm983) + ((0.00177653 * fTempPerm984) + ((5.360590000000001e-05 * fTempPerm985) + ((0.0027816299999999998 * fTempPerm986) + ((0.050635700000000006 * fTempPerm987) + ((0.0413412 * fTempPerm988) + ((0.0214629 * fTempPerm989) + ((0.0293457 * fTempPerm990) + ((0.0636584 * fTempPerm991) + ((0.08895479999999999 * fTempPerm992) + ((0.0608862 * fTempPerm993) + ((0.0771319 * fTempPerm994) + ((0.0905515 * fTempPerm995) + ((0.00545812 * fTempPerm996) + ((0.00181104 * fTempPerm997) + ((0.00114689 * fTempPerm998) + ((0.0005869030000000001 * fTempPerm999) + ((0.00122991 * fTempPerm1000) + ((0.000168725 * fTempPerm1001) + ((0.000533584 * fTempPerm1002) + ((0.00194177 * fTempPerm1003) + ((0.000676256 * fTempPerm1004) + ((0.000981663 * fTempPerm1005) + ((0.0008345150000000001 * fTempPerm1006) + ((0.0005068340000000001 * fTempPerm1007) + ((0.00041329900000000006 * fTempPerm1008) + ((0.00274035 * fTempPerm1009) + ((0.00321075 * fTempPerm1010) + ((0.0042153 * fTempPerm1011) + ((0.023001999999999998 * fTempPerm1012) + ((0.00312119 * fTempPerm1013) + ((0.00361293 * fTempPerm1014) + ((0.00177941 * fTempPerm1015) + ((2.8070300000000005e-05 * fTempPerm1016) + ((6.092990000000001e-05 * fTempPerm1017) + ((0.000131157 * fTempPerm1018) + ((2.09787e-05 * fTempPerm1019) + ((0.00015469 * fTempPerm1020) + ((3.39551e-05 * fVec14[IOTA&127]) + ((0.000524925 * fTempPerm1021) + ((0.00129779 * fTempPerm1022) + ((0.000773294 * fTempPerm1023) + ((0.00046532900000000006 * fTempPerm1024) + ((3.92849e-05 * fTempPerm1025) + ((0.0005198920000000001 * fTempPerm1026) + ((0.0077832 * fTempPerm1027) + ((0.00255288 * fTempPerm1028) + ((0.0004971870000000001 * fTempPerm1029) + ((0.00215198 * fTempPerm1030) + ((0.00534772 * fTempPerm1031) + ((0.00503769 * fTempPerm1032) + ((0.0010498400000000002 * fTempPerm1033) + ((0.0013770800000000001 * fTempPerm1034) + (((0.00194699 * fTempPerm1035) + ((0.00522673 * fTempPerm1036) + ((1.2067399999999999e-06 * fTempPerm1037) + ((0.000709958 * fTempPerm1038) + ((0.000824869 * fTempPerm1039) + ((0.00145329 * fTempPerm1040) + ((0.000609699 * fTempPerm1041) + ((0.00195891 * fTempPerm1042) + ((0.00600683 * fTempPerm1043) + ((5.11494e-05 * fTempPerm1044) + ((0.00579697 * fTempPerm1045) + ((0.00576228 * fTempPerm1046) + ((0.0325354 * fTempPerm1047) + ((0.0004561410000000001 * fTempPerm1048) + ((1.8717900000000003e-05 * fTempPerm1049) + ((0.000106546 * fTempPerm1050) + ((0.034514800000000005 * fTempPerm1051) + ((0.07349510000000001 * fTempPerm1052) + ((0.05960650000000001 * fTempPerm1053) + ((0.0647491 * fTempPerm1054) + ((0.0449074 * fTempPerm1055) + ((0.049342300000000006 * fTempPerm1056) + ((0.0565104 * fTempPerm1057) + ((0.0036302599999999997 * fTempPerm1058) + ((0.000508795 * fTempPerm1059) + ((0.00115892 * fTempPerm1060) + ((0.00024957099999999997 * fTempPerm1061) + ((0.0018634200000000002 * fTempPerm1062) + ((0.000141179 * fTempPerm1063) + ((0.000767728 * fTempPerm1064) + ((0.00059949 * fTempPerm1065) + ((0.0009683840000000001 * fTempPerm1066) + ((0.0781434 * fTempPerm1067) + ((0.00041302300000000005 * fTempPerm1068) + ((0.000418213 * fTempPerm1069) + ((0.00181963 * fTempPerm1070) + ((0.000341662 * fTempPerm1071) + ((0.0011330000000000001 * fTempPerm1072) + ((0.000145149 * fTempPerm1073) + ((0.00185348 * fTempPerm1074) + (0.00124616 * fTempPerm1075))))))))))))))))))))))))))))))))))))))))) + (0.0277414 * fTempPerm1076))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (4.34537e-05 * fTempPerm1077)) + (8.37095e-06 * fVec1[IOTA&127])) + (3.0721e-05 * fTempPerm1078)))))))))))))))))))))))))))))))))))))) + (0.00279092 * fTempPerm1079)) + (0.0056765999999999995 * fTempPerm1080)) + (8.464169999999999e-06 * fTempPerm1081)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (2.49726e-05 * fTempPerm1082)) + (0.000240886 * fTempPerm1083))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.00010376700000000001 * (fTempPerm1084 - fTempPerm1085))) - ((0.000252511 * fTempPerm1086) + ((1.6587200000000003e-05 * fTempPerm1087) + ((1.0584400000000002e-05 * fTempPerm1088) + ((0.000822186 * fTempPerm1089) + ((0.0337348 * fTempPerm1090) + ((0.00030688 * fTempPerm1091) + ((0.00038113700000000003 * fTempPerm1092) + ((0.000253606 * fTempPerm1093) + ((0.000386924 * fTempPerm1094) + ((0.0011128800000000001 * fTempPerm1095) + ((0.000493225 * fTempPerm1096) + ((0.0007384500000000001 * fTempPerm1097) + ((0.000785903 * fTempPerm1098) + ((7.25117e-05 * fTempPerm1099) + ((0.00218833 * fTempPerm1100) + ((0.00318131 * fTempPerm1101) + ((0.00317124 * fTempPerm1102) + ((0.000172325 * fTempPerm1103) + ((4.8090900000000004e-05 * fTempPerm1104) + ((0.00122706 * fTempPerm1105) + ((1.63933e-05 * fTempPerm1106) + ((0.0005400820000000001 * fTempPerm1107) + ((2.44107e-05 * fTempPerm1108) + ((0.000220327 * fTempPerm1109) + ((0.00013262100000000003 * fTempPerm1110) + ((0.000167573 * fTempPerm1111) + ((0.00191763 * fTempPerm1112) + ((0.0018585000000000001 * fTempPerm1113) + ((0.0011357 * fTempPerm1114) + ((0.00212052 * fTempPerm1115) + ((0.0034899700000000002 * fTempPerm1116) + ((0.00315957 * fTempPerm1117) + ((0.00855274 * fTempPerm1118) + ((0.000246231 * fTempPerm1119) + ((0.00014156200000000002 * fTempPerm1120) + ((0.000131389 * fTempPerm1121) + ((0.00012319500000000002 * fTempPerm1122) + ((0.000318859 * fTempPerm1123) + ((0.00039344000000000005 * fTempPerm1124) + ((0.0009186190000000001 * fTempPerm1125) + ((0.000211704 * fTempPerm1126) + ((0.00033167 * fTempPerm1127) + ((0.000465384 * fTempPerm1128) + ((0.0011811600000000001 * fTempPerm1129) + ((0.00040115600000000006 * fTempPerm1130) + ((0.0010665400000000002 * fTempPerm1131) + ((0.000382178 * fTempPerm1132) + ((0.0008083080000000001 * fTempPerm1133) + ((0.0007735880000000001 * fTempPerm1134) + ((0.0018273299999999998 * fTempPerm1135) + ((0.00137963 * fTempPerm1136) + ((0.00871503 * fTempPerm1137) + ((0.00228113 * fTempPerm1138) + ((7.35414e-06 * fTempPerm1139) + ((8.54434e-05 * fTempPerm1140) + ((0.0005563160000000001 * fTempPerm1141) + ((0.0007600840000000001 * fTempPerm1142) + ((0.0009593400000000001 * fTempPerm1143) + ((0.0005454310000000001 * fTempPerm1144) + ((0.0008344860000000001 * fTempPerm1145) + ((0.00350576 * fTempPerm1146) + ((0.00428256 * fTempPerm1147) + ((0.0160001 * fTempPerm1148) + ((0.026311300000000003 * fTempPerm1149) + ((0.00458477 * fTempPerm1150) + ((0.0147427 * fTempPerm1151) + ((2.22436e-05 * fTempPerm1152) + ((0.000612352 * fTempPerm1153) + ((0.00035679200000000004 * fTempPerm1154) + ((0.00277183 * fTempPerm1155) + ((0.000581225 * fTempPerm1156) + ((0.000500102 * fTempPerm1157) + ((0.00721087 * fTempPerm1158) + ((0.00183014 * fTempPerm1159) + ((0.00309207 * fTempPerm1160) + ((6.42769e-05 * fTempPerm1161) + ((0.000393613 * fTempPerm1162) + ((0.000420754 * fTempPerm1163) + ((0.000370808 * fTempPerm1164) + ((0.00146779 * fTempPerm1165) + ((0.000256723 * fTempPerm1166) + ((0.00187645 * fTempPerm1167) + ((0.0014275899999999998 * fTempPerm1168) + ((0.0017828800000000001 * fTempPerm1169) + ((0.00131779 * fTempPerm1170) + ((0.00030791900000000003 * fTempPerm1171) + ((0.009139089999999999 * fTempPerm1172) + ((0.00775237 * fTempPerm1173) + ((0.00043536700000000003 * fTempPerm1174) + ((0.00220238 * fTempPerm1175) + ((0.0065159300000000005 * fTempPerm1176) + ((0.00236909 * fTempPerm1177) + ((0.0177826 * fTempPerm1178) + ((0.00588073 * fTempPerm1179) + ((0.0271505 * fTempPerm1180) + ((0.0184273 * fTempPerm1181) + ((1.98714e-05 * fTempPerm1182) + ((1.68007e-05 * fTempPerm1183) + ((4.004800000000001e-05 * fTempPerm1184) + ((0.000135082 * fTempPerm1185) + ((1.4516400000000002e-05 * fTempPerm1186) + ((2.00899e-06 * fTempPerm1187) + ((4.095139999999999e-06 * fTempPerm1188) + ((0.000184015 * fTempPerm1189) + ((0.00136395 * fTempPerm1190) + ((7.25093e-06 * fTempPerm1191) + ((0.00068371 * fTempPerm1192) + ((0.0008484410000000001 * fTempPerm1193) + ((0.0010072700000000002 * fTempPerm1194) + ((0.00137375 * fTempPerm1195) + ((0.0019387000000000002 * fTempPerm1196) + ((2.3497200000000003e-05 * fTempPerm1197) + ((2.6824200000000003e-05 * fTempPerm1198) + ((4.78325e-05 * fTempPerm1199) + ((0.000105713 * fTempPerm1200) + ((0.000504787 * fTempPerm1201) + ((0.0012131000000000002 * fTempPerm1202) + ((0.000254 * fTempPerm1203) + ((0.000979097 * fTempPerm1204) + ((0.0008138310000000001 * fTempPerm1205) + ((0.0025903700000000003 * fTempPerm1206) + ((0.00416767 * fTempPerm1207) + ((0.0040668100000000006 * fTempPerm1208) + ((0.0102882 * fTempPerm1209) + ((0.0166733 * fTempPerm1210) + ((0.00668055 * fTempPerm1211) + ((0.00678927 * fTempPerm1212) + ((0.0091074 * fTempPerm1213) + ((0.0130569 * fTempPerm1214) + ((0.00421391 * fTempPerm1215) + ((8.465059999999999e-06 * fTempPerm1216) + ((4.13157e-05 * fTempPerm1217) + ((0.000857682 * fTempPerm1218) + ((7.80589e-06 * fTempPerm1219) + ((0.000329953 * fTempPerm1220) + ((4.59823e-05 * fTempPerm1221) + ((0.000504936 * fTempPerm1222) + ((0.000250352 * fTempPerm1223) + ((0.0041234900000000005 * fTempPerm1224) + ((0.0028892500000000003 * fTempPerm1225) + ((6.334310000000001e-05 * fTempPerm1226) + ((0.0026067200000000003 * fTempPerm1227) + ((0.00364145 * fTempPerm1228) + ((0.00245199 * fTempPerm1229) + ((0.000935414 * fTempPerm1230) + ((0.00686806 * fTempPerm1231) + ((0.0158055 * fTempPerm1232) + ((0.0196975 * fTempPerm1233) + ((0.00821411 * fTempPerm1234) + ((0.0197247 * fTempPerm1235) + ((0.0142523 * fTempPerm1236) + ((0.00039235 * fTempPerm1237) + ((0.016522600000000002 * fTempPerm1238) + ((0.00577376 * fTempPerm1239) + ((0.00298131 * fTempPerm1240) + ((0.0021578599999999997 * fTempPerm1241) + ((0.0006632360000000001 * fTempPerm1242) + ((0.00032355000000000003 * fTempPerm1243) + ((0.00034920800000000005 * fTempPerm1244) + ((0.000332442 * fTempPerm1245) + ((0.000257437 * fTempPerm1246) + ((0.000930012 * fTempPerm1247) + ((0.000563399 * fTempPerm1248) + ((9.164930000000001e-05 * fTempPerm1249) + ((5.71822e-05 * fTempPerm1250) + ((0.00267889 * fTempPerm1251) + ((0.000741378 * fTempPerm1252) + ((0.000300667 * fTempPerm1253) + ((0.00075356 * fTempPerm1254) + ((0.0021144 * fTempPerm1255) + ((0.00236502 * fTempPerm1256) + ((0.00355896 * fTempPerm1257) + ((0.037316 * fTempPerm1258) + ((4.04727e-05 * fTempPerm1259) + ((9.438850000000001e-05 * fTempPerm1260) + ((0.0005743240000000001 * fTempPerm1261) + ((8.31551e-05 * fTempPerm1262) + ((0.0011469 * fTempPerm1263) + ((0.0022757299999999997 * fTempPerm1264) + ((0.00102811 * fTempPerm1265) + ((0.000230976 * fTempPerm1266) + ((0.010274799999999999 * fTempPerm1267) + ((0.015218700000000002 * fTempPerm1268) + ((0.00514333 * fTempPerm1269) + ((0.019054 * fTempPerm1270) + ((0.029220799999999998 * fTempPerm1271) + ((0.0191082 * fTempPerm1272) + ((0.0101244 * fTempPerm1273) + ((0.0243991 * fTempPerm1274) + ((0.00719438 * fTempPerm1275) + ((0.0065491 * fTempPerm1276) + ((0.00345408 * fTempPerm1277) + ((7.5122799999999995e-06 * fTempPerm1278) + ((5.28915e-07 * fTempPerm1279) + ((0.0009494360000000001 * fTempPerm1280) + ((0.00189814 * fTempPerm1281) + ((0.000477056 * fTempPerm1282) + ((0.00401066 * fTempPerm1283) + ((0.00327415 * fTempPerm1284) + ((0.0026408399999999998 * fTempPerm1285) + ((0.006443870000000001 * fTempPerm1286) + ((0.014478500000000002 * fTempPerm1287) + ((7.41927e-06 * fTempPerm1288) + ((2.35945e-05 * fTempPerm1289) + ((0.00021774900000000002 * fTempPerm1290) + ((0.00148109 * fTempPerm1291) + ((0.0015205800000000001 * fTempPerm1292) + ((0.00338625 * fTempPerm1293) + ((0.00112007 * fTempPerm1294) + ((0.000955261 * fTempPerm1295) + ((0.000321321 * fTempPerm1296) + ((5.1650600000000005e-06 * fTempPerm1297) + ((1.20038e-05 * fTempPerm1298) + ((0.00041449600000000004 * fTempPerm1299) + ((0.00256128 * fTempPerm1300) + ((0.000145004 * fTempPerm1301) + ((0.00114574 * fTempPerm1302) + ((0.0024714 * fTempPerm1303) + ((0.00173247 * fTempPerm1304) + ((0.00048772500000000005 * fTempPerm1305) + ((0.000196712 * fTempPerm1306) + ((0.000832929 * fTempPerm1307) + ((4.4803300000000005e-05 * fTempPerm1308) + ((0.00229341 * fTempPerm1309) + ((0.00101342 * fTempPerm1310) + ((7.5002e-06 * fTempPerm1311) + ((3.85597e-05 * fTempPerm1312) + ((0.00319579 * fTempPerm1313) + ((0.00118062 * fTempPerm1314) + ((0.00021222400000000003 * fTempPerm1315) + ((0.00152274 * fTempPerm1316) + ((0.00373734 * fTempPerm1317) + ((0.000300188 * fTempPerm1318) + ((0.0014280500000000002 * fTempPerm1319) + ((3.1495500000000004e-05 * fTempPerm1320) + ((0.00042444499999999997 * fTempPerm1321) + ((0.00011064600000000001 * fTempPerm1322) + ((0.000302481 * fTempPerm1323) + ((0.000222638 * fTempPerm1324) + ((0.00012015400000000002 * fTempPerm1325) + ((0.00406353 * fTempPerm1326) + ((0.00148032 * fTempPerm1327) + ((0.00200254 * fTempPerm1328) + ((0.00176094 * fTempPerm1329) + ((0.00441737 * fTempPerm1330) + ((0.047074 * fTempPerm1331) + ((0.050409800000000005 * fTempPerm1332) + ((0.00882548 * fTempPerm1333) + ((0.0028401100000000003 * fTempPerm1334) + ((0.000126889 * fTempPerm1335) + ((8.367780000000001e-05 * fTempPerm1336) + ((0.000766797 * fTempPerm1337) + ((0.00193305 * fTempPerm1338) + ((0.0006355460000000001 * fTempPerm1339) + ((0.0023513099999999997 * fTempPerm1340) + ((0.000880495 * fTempPerm1341) + ((0.00244285 * fTempPerm1342) + ((0.0008249700000000001 * fTempPerm1343) + ((0.0025408400000000004 * fTempPerm1344) + ((0.00107731 * fTempPerm1345) + ((0.00438658 * fTempPerm1346) + ((0.0029096 * fTempPerm1347) + ((0.00466173 * fTempPerm1348) + ((0.00223008 * fTempPerm1349) + ((6.941790000000001e-05 * fTempPerm1350) + ((0.00855288 * fTempPerm1351) + ((0.0060419900000000006 * fTempPerm1352) + ((0.030350199999999997 * fTempPerm1353) + ((0.00316892 * fTempPerm1354) + ((0.025193300000000002 * fTempPerm1355) + ((0.00210234 * fTempPerm1356) + ((0.007391600000000001 * fTempPerm1357) + ((0.017867400000000002 * fTempPerm1358) + ((0.010231500000000001 * fTempPerm1359) + ((0.00146583 * fTempPerm1360) + ((0.006423460000000001 * fTempPerm1361) + ((0.0007851070000000001 * fTempPerm1362) + ((0.000350258 * fTempPerm1363) + ((0.000137982 * fTempPerm1364) + ((1.7421599999999998e-07 * fTempPerm1365) + ((1.6178099999999998e-07 * fTempPerm1366) + ((0.000384054 * fTempPerm1367) + ((0.00108188 * fTempPerm1368) + ((0.00040792200000000007 * fTempPerm1369) + ((0.0017854000000000001 * fTempPerm1370) + ((0.000146465 * fTempPerm1371) + ((2.4437900000000002e-05 * fTempPerm1372) + ((0.00134825 * fTempPerm1373) + ((0.00267142 * fTempPerm1374) + ((0.00237535 * fTempPerm1375) + ((0.0010079800000000001 * fTempPerm1376) + ((0.00320891 * fTempPerm1377) + ((0.00200773 * fTempPerm1378) + ((0.00476789 * fTempPerm1379) + ((0.0044288800000000005 * fTempPerm1380) + ((0.0024688500000000003 * fTempPerm1381) + ((0.00277838 * fTempPerm1382) + ((0.000233207 * fTempPerm1383) + ((3.0118399999999995e-06 * fTempPerm1384) + ((0.000539206 * fTempPerm1385) + ((0.00041388 * fTempPerm1386) + ((0.000138245 * fTempPerm1387) + ((0.00329724 * fTempPerm1388) + ((0.00011532 * fTempPerm1389) + ((0.000342912 * fTempPerm1390) + ((0.0108342 * fTempPerm1391) + ((9.6016e-06 * fTempPerm1392) + ((9.82888e-05 * fTempPerm1393) + ((3.11789e-06 * fTempPerm1394) + ((2.6161300000000003e-05 * fTempPerm1395) + ((9.07505e-05 * fTempPerm1396) + ((0.0015964400000000002 * fTempPerm1397) + ((0.00200683 * fTempPerm1398) + ((0.0011488700000000002 * fTempPerm1399) + ((0.00186912 * fTempPerm1400) + ((0.00042257 * fTempPerm1401) + ((0.0018522 * fTempPerm1402) + ((0.00164123 * fTempPerm1403) + ((0.00985606 * fTempPerm1404) + ((0.00552124 * fTempPerm1405) + ((0.0008197110000000001 * fTempPerm1406) + ((0.0074312 * fTempPerm1407) + ((0.000372384 * fTempPerm1408) + ((0.00252045 * fTempPerm1409) + ((0.000528537 * fTempPerm1410) + ((((2.8368800000000002e-05 * fTempPerm1411) + ((4.5684500000000006e-05 * fTempPerm1412) + ((0.00117138 * fTempPerm1413) + ((9.0214e-05 * fTempPerm1414) + ((0.000483767 * fTempPerm1415) + ((0.00040982500000000005 * fTempPerm1416) + ((0.00192194 * fTempPerm1417) + ((0.00108387 * fTempPerm1418) + ((0.00269537 * fTempPerm1419) + ((0.000692677 * fTempPerm1420) + ((0.00355741 * fTempPerm1421) + ((0.00128336 * fTempPerm1422) + ((0.00328774 * fTempPerm1423) + ((0.00909628 * fTempPerm1424) + ((0.0052343400000000005 * fTempPerm1425) + ((0.00361629 * fTempPerm1426) + ((0.010498400000000001 * fTempPerm1427) + ((0.00212608 * fTempPerm1428) + ((0.0114966 * fTempPerm1429) + ((0.00104271 * fTempPerm1430) + ((0.0128167 * fTempPerm1431) + ((0.011590800000000002 * fTempPerm1432) + ((0.00388006 * fTempPerm1433) + (((0.020153300000000002 * fTempPerm1434) + ((0.042437800000000005 * fTempPerm1435) + ((0.0471336 * fTempPerm1436) + ((0.00143681 * fTempPerm1437) + ((0.000861014 * fTempPerm1438) + ((0.000584922 * fTempPerm1439) + ((6.78291e-06 * fTempPerm1440) + ((0.000255296 * fTempPerm1441) + ((3.06573e-05 * fTempPerm1442) + ((5.334069999999999e-07 * fTempPerm1443) + ((0.0009507300000000001 * fTempPerm1444) + ((0.00694716 * fTempPerm1445) + ((0.007290700000000001 * fTempPerm1446) + ((0.045109500000000004 * fTempPerm1447) + ((0.012670500000000001 * fTempPerm1448) + ((0.0171496 * fTempPerm1449) + ((0.000392742 * fTempPerm1450) + ((0.0006347410000000001 * fTempPerm1451) + ((0.00022428400000000002 * fTempPerm1452) + ((0.000612552 * fTempPerm1453) + ((0.000177443 * fTempPerm1454) + ((2.3049300000000003e-05 * fTempPerm1455) + ((7.51808e-07 * fTempPerm1456) + ((5.5362e-05 * fTempPerm1457) + ((7.51968e-05 * fTempPerm1458) + ((0.000794133 * fTempPerm1459) + ((0.00187816 * fTempPerm1460) + ((0.00168139 * fTempPerm1461) + ((0.000212276 * fTempPerm1462) + ((0.00359524 * fTempPerm1463) + ((0.0018163100000000002 * fTempPerm1464) + ((3.1160000000000007e-05 * fTempPerm1465) + ((4.3886799999999996e-07 * fTempPerm1466) + ((6.228880000000001e-05 * fTempPerm1467) + ((0.000518811 * fTempPerm1468) + ((0.000107544 * fTempPerm1469) + ((0.000344395 * fTempPerm1470) + ((0.00044989 * fTempPerm1471) + ((0.00232233 * fTempPerm1472) + ((0.00214124 * fTempPerm1473) + ((0.00244249 * fTempPerm1474) + ((0.000275069 * fTempPerm1475) + ((0.00894526 * fTempPerm1476) + ((0.00781816 * fTempPerm1477) + ((0.0201016 * fTempPerm1478) + ((0.006073520000000001 * fTempPerm1479) + ((0.004008149999999999 * fTempPerm1480) + ((0.00482022 * fTempPerm1481) + ((0.015268600000000002 * fTempPerm1482) + ((0.0113631 * fTempPerm1483) + ((0.054586100000000005 * fTempPerm1484) + ((0.00763561 * fTempPerm1485) + ((0.00641723 * fTempPerm1486) + ((0.0251683 * fTempPerm1487) + ((7.143640000000001e-05 * fTempPerm1488) + ((0.00011731000000000001 * fTempPerm1489) + ((0.000736785 * fTempPerm1490) + ((0.000291027 * fTempPerm1491) + ((0.000266689 * fTempPerm1492) + ((0.0002994 * fTempPerm1493) + ((0.000415989 * fTempPerm1494) + ((0.00128761 * fTempPerm1495) + ((0.0026737700000000002 * fTempPerm1496) + ((6.68194e-05 * fTempPerm1497) + ((0.00112463 * fTempPerm1498) + ((0.00133571 * fTempPerm1499) + ((0.000121433 * fTempPerm1500) + ((0.0015572399999999999 * fTempPerm1501) + ((0.00286542 * fTempPerm1502) + ((0.0028925200000000004 * fTempPerm1503) + ((0.00488274 * fTempPerm1504) + ((0.0004585340000000001 * fTempPerm1505) + ((0.00410253 * fTempPerm1506) + ((0.0350465 * fTempPerm1507) + ((0.00030905200000000004 * fTempPerm1508) + ((0.000262141 * fTempPerm1509) + ((0.00113754 * fTempPerm1510) + ((0.0017591500000000001 * fTempPerm1511) + ((0.00026133000000000005 * fTempPerm1512) + ((0.000630871 * fTempPerm1513) + ((0.005016619999999999 * fTempPerm1514) + ((0.0040625900000000005 * fTempPerm1515) + ((0.00823842 * fTempPerm1516) + ((0.0018776900000000002 * fTempPerm1517) + ((0.00613615 * fTempPerm1518) + ((0.00919539 * fTempPerm1519) + ((0.011857 * fTempPerm1520) + ((0.0102805 * fTempPerm1521) + ((0.0028042699999999998 * fTempPerm1522) + ((0.0249734 * fTempPerm1523) + ((0.00554456 * fTempPerm1524) + ((0.014456700000000001 * fTempPerm1525) + ((0.043969699999999994 * fTempPerm1526) + ((0.010103500000000001 * fTempPerm1527) + ((0.026084999999999997 * fTempPerm1528) + ((0.0405775 * fTempPerm1529) + ((0.0703236 * fTempPerm1530) + ((0.0864246 * fTempPerm1531) + ((0.0021171000000000002 * fTempPerm1532) + ((0.000272118 * fTempPerm1533) + ((0.0007430550000000001 * fTempPerm1534) + ((0.000701652 * fTempPerm1535) + ((7.66841e-06 * fTempPerm1536) + ((0.000131304 * fTempPerm1537) + ((7.442140000000001e-05 * fTempPerm1538) + ((0.00112713 * fTempPerm1539) + ((0.0023870700000000003 * fTempPerm1540) + ((0.00119961 * fTempPerm1541) + ((0.00020293100000000003 * fTempPerm1542) + ((0.0012919000000000001 * fTempPerm1543) + ((0.00391025 * fTempPerm1544) + ((0.00390009 * fTempPerm1545) + ((0.015326500000000002 * fTempPerm1546) + ((0.0107983 * fTempPerm1547) + ((0.0016720200000000002 * fTempPerm1548) + ((0.0115642 * fTempPerm1549) + ((0.008342460000000001 * fTempPerm1550) + ((0.00160587 * fTempPerm1551) + ((0.00482279 * fTempPerm1552) + ((0.00574575 * fTempPerm1553) + ((0.005366360000000001 * fTempPerm1554) + ((4.115529999999999e-06 * fTempPerm1555) + ((4.5063100000000006e-05 * fTempPerm1556) + ((0.000127879 * fTempPerm1557) + ((0.00141222 * fTempPerm1558) + ((0.00507132 * fTempPerm1559) + ((0.00504437 * fTempPerm1560) + ((0.00419002 * fTempPerm1561) + ((9.185100000000001e-05 * fTempPerm1562) + ((0.00041918 * fTempPerm1563) + ((0.00143258 * fTempPerm1564) + ((0.009995440000000001 * fTempPerm1565) + ((0.00354222 * fTempPerm1566) + ((0.000475105 * fTempPerm1567) + ((0.0017495700000000002 * fTempPerm1568) + ((0.000359638 * fTempPerm1569) + ((9.02966e-06 * fTempPerm1570) + ((0.00027007400000000003 * fTempPerm1571) + ((0.00044415800000000005 * fTempPerm1572) + ((0.000492681 * fTempPerm1573) + ((0.0011273899999999998 * fTempPerm1574) + ((0.00581921 * fTempPerm1575) + ((0.00033490500000000003 * fTempPerm1576) + ((0.00110453 * fTempPerm1577) + ((0.000103579 * fTempPerm1578) + ((0.000428973 * fTempPerm1579) + ((0.0015042599999999999 * fTempPerm1580) + ((0.0005638910000000001 * fTempPerm1581) + ((0.00020805 * fTempPerm1582) + ((0.000323087 * fTempPerm1583) + ((0.0016313200000000001 * fTempPerm1584) + ((0.00149053 * fTempPerm1585) + ((0.000255876 * fTempPerm1586) + ((0.00299686 * fTempPerm1587) + ((0.00248879 * fTempPerm1588) + ((0.0030320900000000003 * fTempPerm1589) + ((0.0100647 * fTempPerm1590) + ((0.000177736 * fTempPerm1591) + ((7.589380000000002e-05 * fTempPerm1592) + ((6.574880000000001e-05 * fTempPerm1593) + ((0.0005503540000000001 * fTempPerm1594) + ((0.0012643700000000001 * fTempPerm1595) + ((0.00026040700000000004 * fTempPerm1596) + ((0.0006419069999999999 * fTempPerm1597) + ((0.0005181680000000001 * fTempPerm1598) + ((0.000622009 * fTempPerm1599) + ((0.0009646670000000001 * fTempPerm1600) + ((0.00121329 * fTempPerm1601) + ((0.00104971 * fTempPerm1602) + ((0.00155276 * fTempPerm1603) + ((0.000735849 * fTempPerm1604) + ((0.00029472900000000003 * fTempPerm1605) + ((0.0015381499999999998 * fTempPerm1606) + ((0.000216745 * fTempPerm1607) + ((5.25158e-05 * fTempPerm1608) + ((0.000419827 * fTempPerm1609) + ((0.000472497 * fTempPerm1610) + ((0.00170461 * fTempPerm1611) + ((0.00163475 * fTempPerm1612) + ((0.00146534 * fTempPerm1613) + ((0.0016001099999999999 * fTempPerm1614) + ((0.0204983 * fTempPerm1615) + ((0.00863187 * fTempPerm1616) + ((0.021263200000000003 * fTempPerm1617) + ((0.0187685 * fTempPerm1618) + ((0.0119454 * fTempPerm1619) + ((0.0241119 * fTempPerm1620) + ((0.0154331 * fTempPerm1621) + ((0.011326600000000001 * fTempPerm1622) + ((0.0109835 * fTempPerm1623) + ((0.008624660000000001 * fTempPerm1624) + ((0.0030574400000000002 * fTempPerm1625) + ((0.0162979 * fTempPerm1626) + ((0.053418799999999995 * fTempPerm1627) + ((0.07132780000000001 * fTempPerm1628) + ((0.00745693 * fTempPerm1629) + ((0.00196452 * fTempPerm1630) + ((1.4284499999999999e-06 * fTempPerm1631) + ((0.000656648 * fTempPerm1632) + ((0.00033454700000000003 * fTempPerm1633) + ((2.11033e-06 * fTempPerm1634) + ((0.00016311100000000002 * fTempPerm1635) + ((0.000918475 * fTempPerm1636) + ((0.0013005599999999999 * fTempPerm1637) + ((0.00235077 * fTempPerm1638) + ((0.000603783 * fTempPerm1639) + ((0.00761051 * fTempPerm1640) + ((0.0112762 * fTempPerm1641) + ((0.0037140800000000002 * fTempPerm1642) + ((0.00700634 * fTempPerm1643) + ((0.0008188190000000001 * fTempPerm1644) + ((0.0005041500000000001 * fTempPerm1645) + ((0.00286108 * fTempPerm1646) + ((0.0014690600000000001 * fTempPerm1647) + ((4.81969e-05 * fTempPerm1648) + ((0.0019447000000000002 * fTempPerm1649) + ((0.00320578 * fTempPerm1650) + ((0.0014029000000000001 * fTempPerm1651) + ((0.0110708 * fTempPerm1652) + ((3.5305499999999996e-06 * fTempPerm1653) + ((8.4151e-06 * fTempPerm1654) + ((6.11718e-05 * fTempPerm1655) + ((0.00402653 * fTempPerm1656) + ((0.00013188 * fTempPerm1657) + ((8.460210000000001e-05 * fTempPerm1658) + ((0.00102436 * fTempPerm1659) + ((0.00010810400000000001 * fTempPerm1660) + ((0.00203951 * fTempPerm1661) + ((0.00100778 * fTempPerm1662) + ((0.000592974 * fTempPerm1663) + ((0.000534437 * fTempPerm1664) + ((0.00311109 * fTempPerm1665) + ((0.0036924600000000003 * fTempPerm1666) + ((0.00460171 * fTempPerm1667) + ((0.0013940900000000002 * fTempPerm1668) + ((0.00701442 * fTempPerm1669) + ((0.0073180300000000005 * fTempPerm1670) + ((0.0020962000000000003 * fTempPerm1671) + ((0.0009612730000000001 * fTempPerm1672) + ((0.00142376 * fTempPerm1673) + ((0.00138811 * fTempPerm1674) + ((0.00337746 * fTempPerm1675) + ((0.00313597 * fTempPerm1676) + ((0.00043131800000000003 * fTempPerm1677) + ((0.00132344 * fTempPerm1678) + ((0.0023363800000000003 * fTempPerm1679) + ((0.00031515600000000003 * fTempPerm1680) + ((0.00127253 * fTempPerm1681) + ((6.21192e-05 * fTempPerm1682) + ((0.0007156350000000001 * fTempPerm1683) + ((0.0007697240000000001 * fTempPerm1684) + ((0.0017261400000000001 * fTempPerm1685) + ((0.0021753000000000002 * fTempPerm1686) + ((0.00508919 * fTempPerm1687) + ((0.010143899999999999 * fTempPerm1688) + ((0.00396492 * fTempPerm1689) + ((0.014927200000000002 * fTempPerm1690) + ((0.010792999999999999 * fTempPerm1691) + ((0.008510160000000001 * fTempPerm1692) + ((0.0123166 * fTempPerm1693) + ((0.0223719 * fTempPerm1694) + ((0.0179474 * fTempPerm1695) + ((0.007369410000000001 * fTempPerm1696) + ((0.00048162000000000004 * fTempPerm1697) + ((0.0027559100000000003 * fTempPerm1698) + ((0.0011577500000000001 * fTempPerm1699) + ((0.0014551499999999999 * fTempPerm1700) + ((5.8659e-05 * fTempPerm1701) + ((0.00191281 * fTempPerm1702) + ((0.000839627 * fTempPerm1703) + ((0.00124685 * fTempPerm1704) + ((0.00124353 * fTempPerm1705) + ((0.00126004 * fTempPerm1706) + ((0.00103026 * fTempPerm1707) + ((0.0008303280000000001 * fTempPerm1708) + ((0.00193403 * fTempPerm1709) + ((0.00103439 * fTempPerm1710) + ((4.5241600000000004e-05 * fTempPerm1711) + ((0.0120184 * fTempPerm1712) + ((0.011691100000000001 * fTempPerm1713) + ((0.00530958 * fTempPerm1714) + ((0.00721718 * fTempPerm1715) + ((0.0030320200000000003 * fTempPerm1716) + ((0.00131485 * fTempPerm1717) + ((0.0011081700000000001 * fTempPerm1718) + ((0.000362084 * fTempPerm1719) + ((0.00038862600000000004 * fTempPerm1720) + ((0.00147963 * fTempPerm1721) + ((0.00239256 * fTempPerm1722) + ((0.00336985 * fTempPerm1723) + ((0.00113852 * fTempPerm1724) + ((0.000315111 * fTempPerm1725) + ((0.0043256300000000004 * fTempPerm1726) + ((0.005780400000000001 * fTempPerm1727) + ((0.037111700000000004 * fTempPerm1728) + ((0.000181327 * fTempPerm1729) + ((0.0009705890000000001 * fTempPerm1730) + ((0.000160361 * fTempPerm1731) + ((0.00019245000000000002 * fTempPerm1732) + ((0.00113341 * fTempPerm1733) + ((0.00043871700000000003 * fTempPerm1734) + ((2.56393e-06 * fTempPerm1735) + ((0.00489909 * fTempPerm1736) + ((0.000362721 * fTempPerm1737) + ((0.000163854 * fTempPerm1738) + ((0.000137362 * fTempPerm1739) + ((0.00025063300000000005 * fTempPerm1740) + ((0.000272806 * fTempPerm1741) + ((0.000273034 * fTempPerm1742) + ((0.00157916 * fTempPerm1743) + ((0.0021747000000000003 * fTempPerm1744) + ((0.00243748 * fTempPerm1745) + ((0.00766095 * fTempPerm1746) + ((0.04279480000000001 * fTempPerm1747) + ((0.0006032850000000001 * fTempPerm1748) + ((0.00521665 * fTempPerm1749) + ((0.0346875 * fTempPerm1750) + (((0.000208778 * fTempPerm1751) + ((0.000898868 * fTempPerm1752) + ((0.0012871 * fTempPerm1753) + ((0.00182432 * fTempPerm1754) + ((0.0105468 * fTempPerm1755) + ((0.00151485 * fTempPerm1756) + ((0.0130615 * fTempPerm1757) + ((2.5178300000000003e-05 * fTempPerm1758) + ((0.000470283 * fTempPerm1759) + ((4.14666e-05 * fTempPerm1760) + ((6.79702e-05 * fTempPerm1761) + ((0.00034321600000000003 * fTempPerm1762) + ((0.000951349 * fTempPerm1763) + ((0.00039275000000000003 * fTempPerm1764) + ((0.000591623 * fTempPerm1765) + ((0.0020045600000000003 * fTempPerm1766) + ((1.2052900000000001e-05 * fTempPerm1767) + ((0.000142659 * fTempPerm1768) + ((0.00286437 * fTempPerm1769) + ((0.0012207399999999999 * fTempPerm1770) + ((0.0017465 * fTempPerm1771) + ((0.00839197 * fTempPerm1772) + ((0.00817616 * fTempPerm1773) + ((0.000133249 * fTempPerm1774) + ((0.000282792 * fTempPerm1775) + ((0.00043214699999999997 * fTempPerm1776) + ((3.3736500000000005e-05 * fTempPerm1777) + ((0.00027134800000000003 * fTempPerm1778) + ((0.0012102200000000001 * fTempPerm1779) + ((0.00150013 * fTempPerm1780) + ((0.010154000000000002 * fTempPerm1781) + ((0.019400900000000002 * fTempPerm1782) + ((0.00021442500000000002 * fTempPerm1783) + ((0.000169249 * fTempPerm1784) + ((0.000249663 * fTempPerm1785) + ((0.00213426 * fTempPerm1786) + ((0.00040582000000000004 * fTempPerm1787) + ((0.00225807 * fTempPerm1788) + ((0.0156673 * fTempPerm1789) + ((0.000120611 * fTempPerm1790) + ((0.0121152 * fTempPerm1791) + ((6.8184199999999995e-06 * fTempPerm1792) + ((9.995620000000002e-05 * fTempPerm1793) + ((0.000165695 * fTempPerm1794) + ((0.000818766 * fTempPerm1795) + ((3.58722e-05 * fTempPerm1796) + ((0.00041316100000000005 * fTempPerm1797) + ((0.00116132 * fTempPerm1798) + ((0.0035543000000000003 * fTempPerm1799) + ((0.00454218 * fTempPerm1800) + ((0.00010922300000000001 * fTempPerm1801) + ((0.000141295 * fTempPerm1802) + ((0.00035581800000000004 * fTempPerm1803) + ((0.000997929 * fTempPerm1804) + ((0.00449214 * fTempPerm1805) + ((0.0008606820000000001 * fTempPerm1806) + ((0.00717239 * fTempPerm1807) + ((0.00655248 * fTempPerm1808) + ((0.009254680000000001 * fTempPerm1809) + ((0.00263881 * fTempPerm1810) + ((8.331490000000002e-05 * fTempPerm1811) + ((0.00567696 * fTempPerm1812) + ((2.7205700000000003e-05 * fTempPerm1813) + ((0.0017748 * fTempPerm1814) + ((0.000134822 * fTempPerm1815) + ((0.009537660000000002 * fTempPerm1816) + ((0.000643151 * fTempPerm1817) + ((0.0165022 * fTempPerm1818) + ((0.026153100000000002 * fTempPerm1819) + ((0.0155188 * fTempPerm1820) + ((0.0117481 * fTempPerm1821) + ((0.0240448 * fTempPerm1822) + ((0.011073200000000002 * fTempPerm1823) + ((0.00016208700000000002 * fTempPerm1824) + ((0.00023927800000000002 * fTempPerm1825) + ((0.000998409 * fTempPerm1826) + ((0.00026075800000000004 * fTempPerm1827) + ((0.00190039 * fTempPerm1828) + ((0.000343024 * fTempPerm1829) + ((0.00385064 * fTempPerm1830) + ((0.00280679 * fTempPerm1831) + ((0.0142655 * fTempPerm1832) + ((0.006754800000000001 * fTempPerm1833) + ((0.0025108100000000005 * fTempPerm1834) + ((0.00235191 * fTempPerm1835) + ((0.0009197939999999999 * fTempPerm1836) + ((0.000506998 * fTempPerm1837) + ((9.84852e-06 * fTempPerm1838) + ((0.00029024100000000006 * fTempPerm1839) + ((0.00142103 * fTempPerm1840) + ((0.000541095 * fTempPerm1841) + ((0.0008297610000000001 * fTempPerm1842) + ((0.00146794 * fTempPerm1843) + ((0.00299123 * fTempPerm1844) + ((0.00198432 * fTempPerm1845) + ((0.0030592 * fTempPerm1846) + ((0.000502175 * fTempPerm1847) + ((0.000550486 * fTempPerm1848) + ((0.025458500000000002 * fTempPerm1849) + ((0.0006988710000000001 * fTempPerm1850) + ((0.0010789999999999999 * fTempPerm1851) + ((0.0014951399999999998 * fTempPerm1852) + ((0.0013151 * fTempPerm1853) + ((0.000608302 * fTempPerm1854) + ((((((((1.1129000000000001e-05 * fTempPerm1855) + ((9.139900000000001e-05 * fTempPerm1856) + ((0.00012562199999999998 * fTempPerm1857) + ((0.00189912 * fTempPerm1858) + ((0.00102903 * fTempPerm1859) + ((0.00444703 * fTempPerm1860) + ((0.015913200000000002 * fTempPerm1861) + ((0.0116756 * fTempPerm1862) + ((0.0030104600000000004 * fTempPerm1863) + ((0.00246014 * fTempPerm1864) + ((0.008875280000000001 * fTempPerm1865) + ((0.0024969700000000003 * fTempPerm1866) + ((0.0014838199999999998 * fTempPerm1867) + (((((2.38744e-05 * fTempPerm1868) + ((1.9656400000000004e-05 * fTempPerm1869) + ((1.12306e-05 * fVec4[IOTA&127]) + ((3.8530500000000007e-05 * fTempPerm1870) + ((4.19701e-05 * fTempPerm1871) + ((1.60757e-05 * fVec6[IOTA&127]) + ((0.0160175 * fTempPerm1872) + ((0.0095142 * fTempPerm1873) + ((0.000544865 * fTempPerm1874) + ((0.000311911 * fTempPerm1875) + ((0.00296739 * fTempPerm1876) + ((0.0114164 * fTempPerm1877) + ((0.015415000000000002 * fTempPerm1878) + ((0.00152143 * fTempPerm1879) + ((0.0032921200000000004 * fTempPerm1880) + ((0.00193753 * fTempPerm1881) + ((0.000625519 * fTempPerm1882) + ((0.0037726500000000002 * fTempPerm1883) + ((0.00476244 * fTempPerm1884) + ((0.00351467 * fTempPerm1885) + ((1.9893500000000002e-05 * fTempPerm1886) + ((3.7877500000000005e-05 * fTempPerm1887) + ((0.0042904 * fTempPerm1888) + ((0.0015305800000000001 * fTempPerm1889) + ((0.00166752 * fTempPerm1890) + ((0.00172844 * fTempPerm1891) + ((1.14088e-05 * fTempPerm1892) + ((0.00136039 * fTempPerm1893) + ((0.000782908 * fTempPerm1894) + ((0.00304542 * fTempPerm1895) + ((0.0010344 * fTempPerm1896) + ((0.00241033 * fTempPerm1897) + ((0.00274518 * fTempPerm1898) + ((0.000654569 * fTempPerm1899) + ((0.00544477 * fTempPerm1900) + ((6.971659999999999e-06 * fTempPerm1901) + ((0.0008882540000000001 * fTempPerm1902) + ((0.000446193 * fTempPerm1903) + ((0.000711991 * fTempPerm1904) + ((9.829720000000001e-05 * fTempPerm1905) + ((0.00192986 * fTempPerm1906) + ((0.0010388900000000002 * fTempPerm1907) + ((0.00170268 * fTempPerm1908) + ((0.0013548899999999999 * fTempPerm1909) + ((0.00200404 * fTempPerm1910) + ((0.0015286800000000001 * fTempPerm1911) + ((0.0017147500000000001 * fTempPerm1912) + ((0.0301367 * fTempPerm1913) + ((0.0173412 * fTempPerm1914) + ((0.0391701 * fTempPerm1915) + ((0.00031563800000000003 * fTempPerm1916) + ((0.00201638 * fTempPerm1917) + ((0.0005423040000000001 * fTempPerm1918) + ((0.00127535 * fTempPerm1919) + ((0.00312195 * fTempPerm1920) + ((0.0019008400000000002 * fTempPerm1921) + ((0.00121188 * fTempPerm1922) + ((5.444779999999999e-06 * fTempPerm1923) + ((0.00191902 * fTempPerm1924) + ((0.00260887 * fTempPerm1925) + ((0.00465625 * fTempPerm1926) + ((0.0137585 * fTempPerm1927) + ((0.00350462 * fTempPerm1928) + ((0.00123749 * fTempPerm1929) + ((0.0092683 * fTempPerm1930) + ((6.52305e-05 * fTempPerm1931) + ((0.000269357 * fTempPerm1932) + ((0.0158698 * fTempPerm1933) + ((0.000314307 * fTempPerm1934) + ((0.0200181 * fTempPerm1935) + ((0.00117495 * fTempPerm1936) + ((0.0047484400000000005 * fTempPerm1937) + ((0.000138672 * fTempPerm1938) + ((0.0017056100000000002 * fTempPerm1939) + ((0.0027951100000000004 * fTempPerm1940) + ((0.0037710400000000002 * fTempPerm1941) + ((0.0104416 * fTempPerm1942) + ((0.00454199 * fTempPerm1943) + ((0.016194200000000002 * fTempPerm1944) + ((0.016079200000000002 * fTempPerm1945) + ((0.0128559 * fTempPerm1946) + ((0.0166062 * fTempPerm1947) + ((0.0221845 * fTempPerm1948) + ((0.0188738 * fTempPerm1949) + ((0.0203737 * fTempPerm1950) + ((0.015368999999999999 * fTempPerm1951) + ((0.0180915 * fTempPerm1952) + ((0.025263300000000002 * fTempPerm1953) + ((0.026437 * fTempPerm1954) + ((0.0357626 * fTempPerm1955) + ((0.026966700000000003 * fTempPerm1956) + ((0.023967600000000002 * fTempPerm1957) + ((0.00134669 * fTempPerm1958) + ((7.21153e-05 * fTempPerm1959) + ((1.68655e-05 * fVec7[IOTA&127]) + ((2.8269799999999997e-06 * fTempPerm1960) + ((2.9285600000000003e-05 * fTempPerm1961) + ((3.866159999999999e-06 * fVec9[IOTA&127]) + ((0.00177024 * fTempPerm1962) + ((0.00161133 * fTempPerm1963) + ((0.0193136 * fTempPerm1964) + ((0.0161655 * fTempPerm1965) + ((0.014907300000000002 * fTempPerm1966) + ((3.8802e-05 * fTempPerm1967) + ((8.74662e-06 * fTempPerm1968) + ((0.00392637 * fTempPerm1969) + ((1.8656099999999998e-06 * fTempPerm1970) + ((3.1175000000000006e-05 * fTempPerm1971) + ((0.00035653000000000003 * fTempPerm1972) + ((0.000929744 * fTempPerm1973) + ((0.00184981 * fTempPerm1974) + ((0.0023879 * fTempPerm1975) + ((0.0100582 * fTempPerm1976) + ((0.00687643 * fTempPerm1977) + ((0.00958195 * fTempPerm1978) + ((0.015119400000000002 * fTempPerm1979) + ((0.0193807 * fTempPerm1980) + ((0.0125257 * fTempPerm1981) + ((0.039406500000000004 * fTempPerm1982) + ((0.0390074 * fTempPerm1983) + ((0.0011779100000000001 * fTempPerm1984) + ((0.000315062 * fTempPerm1985) + ((0.000167329 * fTempPerm1986) + ((0.00041459900000000003 * fTempPerm1987) + ((0.00106944 * fTempPerm1988) + ((0.00141242 * fTempPerm1989) + ((0.0007414510000000001 * fTempPerm1990) + ((2.57285e-05 * fTempPerm1991) + ((0.007374660000000001 * fTempPerm1992) + ((0.000161277 * fTempPerm1993) + ((3.85191e-06 * fTempPerm1994) + ((0.00392588 * fTempPerm1995) + ((0.00217995 * fTempPerm1996) + ((0.00350304 * fTempPerm1997) + ((5.52174e-06 * fTempPerm1998) + ((9.036830000000001e-05 * fTempPerm1999) + ((0.0010940099999999999 * fTempPerm2000) + ((0.0077227400000000005 * fTempPerm2001) + ((0.00111238 * fTempPerm2002) + ((0.000404712 * fTempPerm2003) + ((0.000815364 * fTempPerm2004) + ((0.0008307620000000001 * fTempPerm2005) + ((0.0014440100000000001 * fTempPerm2006) + ((0.00129094 * fTempPerm2007) + ((0.00131875 * fTempPerm2008) + ((0.000127246 * fTempPerm2009) + ((0.0015582 * fTempPerm2010) + ((4.7581900000000006e-05 * fTempPerm2011) + ((0.00461702 * fTempPerm2012) + ((0.00756122 * fTempPerm2013) + ((0.00526349 * fTempPerm2014) + ((1.03751e-05 * fTempPerm2015) + ((0.000684209 * fTempPerm2016) + ((0.000498921 * fTempPerm2017) + ((0.00475476 * fTempPerm2018) + ((0.00862273 * fTempPerm2019) + ((7.683169999999999e-06 * fTempPerm2020) + ((0.00152436 * fTempPerm2021) + ((1.965e-06 * fTempPerm2022) + ((0.000478309 * fTempPerm2023) + ((0.00323696 * fTempPerm2024) + ((0.00317886 * fTempPerm2025) + ((0.00765874 * fTempPerm2026) + ((0.00436148 * fTempPerm2027) + ((0.0115485 * fTempPerm2028) + ((0.0032127699999999998 * fTempPerm2029) + ((0.00017378 * fTempPerm2030) + ((3.69031e-05 * fTempPerm2031) + ((0.00048183400000000005 * fTempPerm2032) + ((0.00038273500000000004 * fTempPerm2033) + ((1.70851e-05 * fTempPerm2034) + (0.000141728 * (fTempPerm2035 - fTempPerm2036))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (2.5231199999999997e-06 * fVec2[IOTA&127])) + (2.0610900000000003e-05 * fVec0[IOTA&127])) + (4.3569800000000004e-05 * fTempPerm2037))))))))))))))) + (4.81323e-05 * fTempPerm2038)) + (8.318509999999999e-06 * fTempPerm2039)) + (0.00271694 * fTempPerm2040)) + (4.109880000000001e-05 * fTempPerm2041)) + (0.000737536 * fTempPerm2042)) + (0.000383553 * fTempPerm2043)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.000582475 * fTempPerm2044))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0222634 * fTempPerm2045))))))))))))))))))))))))) + (0.00017067000000000002 * fTempPerm2046)) + (0.00017706000000000002 * fTempPerm2047)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
			fRec0[0] = max((fRec0[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm2048))))));
			fbargraph16 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTempPerm2048;
			fTempPerm2049 = (fRec1[0] * (((6.12983e-06 * fTempPerm2) + ((1.3215300000000002e-05 * fTempPerm3) + ((1.7659600000000003e-05 * fTempPerm1087) + ((0.000444418 * fTempPerm4) + ((0.0011280300000000001 * fTempPerm5) + ((0.000174066 * fTempPerm6) + ((0.00499281 * fTempPerm7) + ((0.0014746 * fTempPerm8) + ((0.0030302600000000003 * fTempPerm9) + ((0.0037463600000000002 * fTempPerm10) + ((0.00252014 * fTempPerm11) + ((0.0023052000000000003 * fTempPerm12) + ((0.00297933 * fTempPerm13) + ((0.000292392 * fTempPerm1092) + ((0.000143232 * fTempPerm17) + ((9.21074e-05 * fTempPerm18) + ((0.000983816 * fTempPerm1094) + ((0.0005833430000000001 * fTempPerm20) + ((0.00130066 * fTempPerm1096) + ((0.00041085900000000007 * fTempPerm1097) + ((0.0012948999999999999 * fTempPerm1098) + ((0.0018707700000000001 * fTempPerm24) + ((0.0006798460000000001 * fTempPerm1102) + ((0.00402003 * fTempPerm27) + ((0.017721 * fTempPerm28) + ((0.0133545 * fTempPerm30) + ((0.000312022 * fTempPerm31) + ((0.000425134 * fTempPerm1104) + ((0.000594499 * fTempPerm32) + ((0.00012805500000000002 * fTempPerm37) + ((0.000103477 * fTempPerm38) + ((0.00025872700000000005 * fTempPerm39) + ((0.00044084399999999997 * fTempPerm40) + ((0.000582041 * fTempPerm41) + ((0.000426232 * fTempPerm42) + ((0.000136777 * fTempPerm43) + ((0.0012123 * fTempPerm44) + ((0.00045970700000000006 * fTempPerm45) + ((0.000276212 * fTempPerm1109) + ((4.9281200000000005e-05 * fTempPerm1110) + ((0.000572913 * fTempPerm1111) + ((0.000133325 * fTempPerm47) + ((0.00716025 * fTempPerm1118) + ((0.0059806600000000005 * fTempPerm48) + ((9.32166e-05 * fTempPerm52) + ((0.00011625700000000002 * fTempPerm53) + ((0.0010682 * fTempPerm54) + (((7.93834e-06 * fTempPerm56) + (((((((0.000127816 * fTempPerm1132) + (((0.000484007 * fTempPerm62) + ((1.55653e-05 * fTempPerm1135) + ((0.0053646300000000004 * fTempPerm1137) + ((0.000202603 * fTempPerm65) + ((0.000276893 * fTempPerm1140) + ((0.00119197 * fTempPerm68) + ((0.0008614940000000001 * fTempPerm1142) + ((0.0005644370000000001 * fTempPerm1143) + ((0.00017768299999999999 * fTempPerm71) + (((0.0022670200000000002 * fTempPerm73) + ((0.004592430000000001 * fTempPerm1147) + ((0.0126165 * fTempPerm74) + ((0.00479751 * fTempPerm75) + ((0.0106036 * fTempPerm1150) + ((0.00037619 * fTempPerm1151) + ((0.025127100000000003 * fTempPerm77) + ((0.000253793 * fTempPerm78) + ((0.0006181 * fTempPerm79) + ((3.3221e-05 * fTempPerm1152) + ((9.34823e-05 * fTempPerm81) + ((0.00338497 * fTempPerm82) + ((0.0047209 * fTempPerm1156) + ((0.004004570000000001 * fTempPerm83) + ((0.000829111 * fTempPerm1157) + ((0.00282354 * fTempPerm86) + ((0.00240414 * fTempPerm87) + ((0.00123226 * fTempPerm1159) + ((0.00555038 * fTempPerm88) + ((((5.37174e-05 * fTempPerm91) + ((0.00100658 * fTempPerm92) + (((0.000719126 * fTempPerm1168) + ((((0.00621762 * fTempPerm96) + ((0.0021503299999999998 * fTempPerm1176) + ((0.00406638 * fTempPerm1179) + ((0.022960699999999997 * fTempPerm98) + ((0.0262763 * fTempPerm100) + ((2.1476600000000004e-05 * fTempPerm101) + ((2.6092200000000004e-05 * fTempPerm102) + ((0.000147762 * fTempPerm1186) + ((0.000458341 * fTempPerm1188) + (((0.00218685 * fTempPerm1194) + ((9.30791e-05 * fTempPerm1195) + ((0.000297869 * fTempPerm106) + ((0.00012342900000000001 * fTempPerm107) + ((0.000134953 * fTempPerm108) + ((9.075200000000001e-05 * fTempPerm109) + ((0.0015819200000000001 * fTempPerm1202) + ((0.0004933950000000001 * fTempPerm110) + ((0.000346228 * fTempPerm1203) + ((0.00106408 * fTempPerm1205) + ((0.0010799700000000002 * fTempPerm111) + ((0.005516409999999999 * fTempPerm112) + ((0.00334124 * fTempPerm113) + ((0.00130206 * fTempPerm114) + ((0.0323072 * fTempPerm115) + ((0.0006845830000000001 * fTempPerm1213) + ((0.0022406600000000002 * fTempPerm116) + ((0.00121477 * fTempPerm117) + ((0.000150709 * fTempPerm118) + ((0.000594639 * fTempPerm119) + ((0.000392538 * fTempPerm120) + ((0.000212529 * fTempPerm121) + ((0.000438524 * fTempPerm123) + ((3.5476400000000005e-05 * fTempPerm1220) + ((0.000132193 * fTempPerm1221) + ((0.000223411 * fTempPerm124) + ((0.000455971 * fTempPerm125) + ((0.0044656900000000005 * fTempPerm1226) + ((0.00553479 * fTempPerm129) + ((0.00304111 * fTempPerm1228) + ((0.00615977 * fTempPerm1230) + ((0.00325646 * fTempPerm131) + ((0.012678 * fTempPerm132) + ((0.0419631 * fTempPerm134) + ((0.014713700000000001 * fTempPerm136) + ((0.007966260000000001 * fTempPerm1234) + ((0.0156604 * fTempPerm137) + ((0.00693131 * fTempPerm138) + ((0.0015239000000000001 * fTempPerm139) + ((0.0018694900000000001 * fTempPerm141) + ((0.0009135770000000002 * fTempPerm142) + ((0.00034025 * fTempPerm143) + ((1.4631700000000003e-05 * fTempPerm144) + ((((0.00177585 * fTempPerm148) + (((0.00026651699999999997 * fTempPerm1246) + ((0.0010646199999999998 * fTempPerm1256) + ((0.006408909999999999 * fTempPerm152) + ((0.00136571 * fTempPerm153) + ((0.00049839 * fTempPerm1257) + ((0.016656999999999998 * fTempPerm154) + ((0.0204958 * fTempPerm155) + ((0.023659500000000003 * fTempPerm156) + ((0.0278627 * fTempPerm157) + ((0.0210797 * fTempPerm158) + ((4.1404e-05 * fTempPerm1260) + ((0.0005187740000000001 * fTempPerm161) + ((0.000654997 * fTempPerm163) + ((0.0006025220000000001 * fTempPerm164) + ((((0.008413589999999999 * fTempPerm168) + ((0.0035177100000000003 * fTempPerm1268) + ((0.00870097 * fTempPerm170) + ((0.00866514 * fTempPerm172) + ((0.0044089 * fTempPerm173) + ((0.0116758 * fTempPerm174) + ((0.023826100000000003 * fTempPerm175) + ((0.00803106 * fTempPerm1273) + ((0.0123386 * fTempPerm177) + ((0.00919814 * fTempPerm178) + ((0.00114058 * fTempPerm1277) + ((2.9367600000000003e-05 * fTempPerm181) + ((0.00033988700000000003 * fTempPerm182) + ((0.000520144 * fTempPerm188) + ((0.00024474399999999997 * fTempPerm189) + ((0.00328831 * fTempPerm190) + ((0.000571553 * fTempPerm191) + ((0.00026790500000000003 * fTempPerm192) + ((0.000171971 * fTempPerm194) + ((0.00265451 * fTempPerm195) + ((0.00231148 * fTempPerm196) + ((0.00104922 * fTempPerm197) + ((0.000209402 * fTempPerm199) + ((0.000494459 * fTempPerm200) + ((0.000639692 * fTempPerm202) + ((0.00019294500000000003 * fTempPerm205) + ((0.0022136499999999997 * fTempPerm206) + ((0.000977515 * fTempPerm1293) + ((0.000369514 * fTempPerm207) + ((0.00206966 * fTempPerm209) + ((0.00039934300000000003 * fTempPerm210) + ((0.00168114 * fTempPerm1296) + ((0.00194062 * fTempPerm211) + ((8.81382e-05 * fTempPerm214) + ((2.3044100000000002e-05 * fTempPerm1298) + ((6.961430000000001e-05 * fTempPerm215) + ((0.0021085400000000003 * fTempPerm1300) + ((0.00014791 * fTempPerm219) + ((0.000325346 * fTempPerm220) + ((0.00019390000000000002 * fTempPerm221) + ((0.00534952 * fTempPerm1309) + ((1.3482700000000002e-05 * fTempPerm1311) + ((0.0012717499999999999 * fTempPerm230) + ((2.56366e-05 * fTempPerm235) + ((0.0012231700000000002 * fTempPerm1313) + ((7.657620000000001e-05 * fTempPerm1314) + ((0.00270927 * fTempPerm1316) + ((0.00043061600000000004 * fTempPerm239) + ((0.00101213 * fTempPerm1317) + ((9.565340000000001e-05 * fTempPerm1319) + ((4.6020600000000005e-05 * fTempPerm1320) + ((0.000248997 * fTempPerm1321) + ((0.000301242 * fTempPerm1322) + ((4.2708300000000006e-05 * fTempPerm1323) + ((8.069520000000001e-05 * fTempPerm1324) + ((0.000996855 * fTempPerm249) + ((0.000584763 * fTempPerm250) + ((0.00139515 * fTempPerm1326) + ((0.00169188 * fTempPerm1328) + ((0.000525487 * fTempPerm252) + ((0.0021074099999999997 * fTempPerm253) + ((0.00276563 * fTempPerm1330) + ((0.0483216 * fTempPerm1331) + ((0.0460809 * fTempPerm1332) + ((0.00517239 * fTempPerm1333) + ((0.0011151400000000001 * fTempPerm1334) + ((0.00013360000000000002 * fTempPerm1335) + ((0.00031798600000000003 * fTempPerm270) + ((0.000135894 * fTempPerm271) + ((0.00132148 * fTempPerm1337) + ((0.00043513300000000004 * fTempPerm1338) + ((0.00047923700000000003 * fTempPerm1339) + ((0.00045356600000000003 * fTempPerm273) + ((0.00072084 * fTempPerm1341) + ((0.000630502 * fTempPerm274) + ((0.000166844 * fTempPerm276) + ((0.00156251 * fTempPerm278) + ((0.0030938500000000004 * fTempPerm1342) + ((0.00313502 * fTempPerm1344) + ((0.00498626 * fTempPerm280) + ((0.00584137 * fTempPerm1346) + ((0.004541280000000001 * fTempPerm1348) + ((0.00402261 * fTempPerm1349) + ((0.00651347 * fTempPerm282) + ((0.0107473 * fTempPerm1351) + ((0.008688010000000001 * fTempPerm1352) + ((0.0015977300000000001 * fTempPerm284) + ((0.0134849 * fTempPerm1353) + ((0.00202873 * fTempPerm287) + ((0.0184808 * fTempPerm1354) + ((0.00561989 * fTempPerm1356) + ((0.017673 * fTempPerm288) + ((0.0109137 * fTempPerm289) + ((0.00203881 * fTempPerm1358) + ((0.00335082 * fTempPerm1359) + ((0.00123416 * fTempPerm1360) + ((0.00218183 * fTempPerm1361) + ((0.000125948 * fTempPerm292) + ((0.00011811100000000002 * fTempPerm293) + ((0.000980811 * fTempPerm295) + ((0.000447309 * fTempPerm1363) + ((0.00046841100000000007 * fTempPerm304) + ((5.73293e-05 * fTempPerm305) + ((0.000399315 * fTempPerm1367) + ((3.42585e-06 * fTempPerm1368) + ((0.00027312 * fTempPerm1370) + ((0.0010393 * fTempPerm310) + ((0.0012512 * fTempPerm1372) + ((0.00289646 * fTempPerm311) + ((1.0336899999999999e-06 * fTempPerm315) + ((9.91772e-05 * fTempPerm1373) + ((0.0018692700000000001 * fTempPerm317) + ((0.0010500000000000002 * fTempPerm318) + ((0.00446647 * fTempPerm321) + ((0.00472803 * fTempPerm322) + ((0.0025056699999999998 * fTempPerm323) + ((3.3373e-05 * fTempPerm1387) + ((0.0022768000000000003 * fTempPerm325) + ((0.0028975399999999997 * fTempPerm1388) + ((0.00311321 * fTempPerm1391) + ((1.2511200000000001e-05 * fTempPerm327) + ((3.30668e-06 * fTempPerm330) + ((1.0896600000000002e-05 * fTempPerm1394) + ((9.644430000000001e-05 * fTempPerm333) + ((0.000340968 * fTempPerm1396) + ((0.000362151 * fTempPerm334) + ((0.0011957500000000002 * fTempPerm335) + ((0.000192954 * fTempPerm1398) + ((0.0177702 * fTempPerm342) + ((0.000251299 * fTempPerm343) + ((0.00047379 * fTempPerm1408) + ((0.000359551 * fTempPerm345) + ((0.00050859 * fTempPerm346) + ((0.00025865 * fTempPerm1410) + ((0.00030013 * fTempPerm348) + ((0.000129621 * fTempPerm2046) + ((5.1984399999999995e-06 * fTempPerm349) + ((0.000102185 * fTempPerm353) + ((0.000159738 * fTempPerm354) + ((0.0015065 * fTempPerm355) + ((0.0007640640000000001 * fTempPerm356) + ((1.1146700000000001e-05 * fTempPerm1420) + ((0.0016114299999999999 * fTempPerm359) + ((8.02869e-05 * fTempPerm360) + ((0.00306934 * fTempPerm361) + ((0.00293369 * fTempPerm362) + ((0.00157178 * fTempPerm1428) + ((0.00411948 * fTempPerm1430) + ((0.0107584 * fTempPerm364) + ((0.00992074 * fTempPerm365) + ((0.000320497 * fTempPerm1431) + ((0.034926 * fTempPerm366) + ((0.00697526 * fTempPerm367) + ((0.0037277 * fTempPerm368) + ((0.045202099999999995 * fTempPerm369) + ((0.00526948 * fTempPerm1433) + ((0.000231249 * fTempPerm1438) + ((0.00020135900000000002 * fTempPerm372) + ((0.00015575000000000002 * fTempPerm373) + ((7.2276399999999995e-06 * fTempPerm1442) + ((1.3220600000000002e-05 * fTempPerm1443) + ((0.000253042 * fTempPerm1444) + ((0.00213182 * fTempPerm379) + ((0.0230173 * fTempPerm381) + ((0.00596581 * fTempPerm382) + ((0.0035753 * fTempPerm1450) + ((0.010657300000000001 * fTempPerm383) + ((0.000237477 * fTempPerm385) + ((0.0007996400000000001 * fTempPerm387) + ((0.000231547 * fTempPerm388) + ((0.000236034 * fTempPerm389) + ((5.6064900000000006e-05 * fTempPerm390) + ((5.669620000000001e-05 * fTempPerm391) + ((0.000310328 * fTempPerm392) + ((0.00023670100000000002 * fTempPerm393) + ((0.00113736 * fTempPerm394) + ((0.0011671499999999998 * fTempPerm395) + ((0.000239355 * fTempPerm396) + ((0.000538982 * fTempPerm397) + ((0.0008074730000000001 * fTempPerm398) + ((6.07349e-05 * fTempPerm1460) + ((0.00138286 * fTempPerm400) + ((0.00226848 * fTempPerm1464) + ((((2.7837800000000002e-06 * fTempPerm1466) + ((8.15635e-05 * fTempPerm1467) + ((4.50719e-05 * fTempPerm403) + ((0.000321589 * fTempPerm1468) + ((7.06061e-05 * fTempPerm1469) + ((0.00029415900000000005 * fTempPerm1470) + ((0.00127291 * fTempPerm1471) + ((0.0005086 * fTempPerm1472) + ((0.00186454 * fTempPerm1473) + ((0.0010098400000000001 * fTempPerm1474) + ((0.00409361 * fTempPerm1475) + ((0.0030470000000000002 * fTempPerm1476) + ((0.010443599999999999 * fTempPerm1477) + ((0.007845340000000001 * fTempPerm1478) + ((0.014278200000000001 * fTempPerm1479) + ((0.00961553 * fTempPerm1480) + ((0.00394804 * fTempPerm1481) + ((0.0309151 * fTempPerm1482) + ((0.006265520000000001 * fTempPerm1483) + ((0.0386248 * fTempPerm1484) + ((0.00563275 * fTempPerm407) + ((0.0106274 * fTempPerm408) + ((0.000933732 * fTempPerm1486) + ((0.0144447 * fTempPerm1487) + ((2.48682e-07 * fTempPerm1488) + ((0.00013044 * fTempPerm1489) + ((6.72119e-05 * fTempPerm415) + ((0.000283247 * fTempPerm1490) + ((0.000417124 * fTempPerm1491) + ((0.00136599 * fTempPerm1492) + ((0.00334238 * fTempPerm1493) + ((0.000616297 * fTempPerm1495) + ((0.00015190600000000002 * fTempPerm1496) + ((3.27635e-05 * fTempPerm422) + ((0.00260939 * fTempPerm424) + ((9.30078e-06 * fTempPerm425) + ((0.0008548220000000001 * fTempPerm1499) + ((0.0008165480000000001 * fTempPerm1500) + ((7.82466e-05 * fTempPerm1501) + ((0.00270886 * fTempPerm1502) + ((0.0022211500000000003 * fTempPerm1503) + ((0.0016530499999999999 * fTempPerm1504) + ((0.0019319900000000002 * fTempPerm1505) + ((0.024289200000000004 * fTempPerm1506) + ((0.00734391 * fTempPerm1507) + ((5.7825300000000005e-06 * fTempPerm431) + ((0.0013042499999999999 * fTempPerm435) + ((0.00656153 * fTempPerm436) + ((0.00222449 * fTempPerm438) + ((0.000129173 * fTempPerm439) + ((0.0056186100000000004 * fTempPerm440) + ((0.032932100000000006 * fTempPerm441) + ((0.024621200000000003 * fTempPerm442) + ((0.042141200000000004 * fTempPerm443) + ((0.028794800000000002 * fTempPerm1524) + ((0.06345640000000001 * fTempPerm444) + ((0.07541260000000001 * fTempPerm445) + ((0.00046772600000000007 * fTempPerm1525) + ((0.0490934 * fTempPerm447) + ((0.0540471 * fTempPerm448) + ((0.0108471 * fTempPerm1527) + ((0.0010471900000000001 * fTempPerm1533) + ((0.00047446700000000006 * fTempPerm452) + ((0.00031437800000000003 * fTempPerm454) + ((0.000233961 * fTempPerm457) + ((0.0005083090000000001 * fTempPerm1541) + ((0.000454632 * fTempPerm461) + ((0.00292619 * fTempPerm1543) + ((0.019155500000000002 * fTempPerm1545) + ((0.00963596 * fTempPerm462) + ((0.0192933 * fTempPerm463) + ((0.00688345 * fTempPerm465) + ((0.036199 * fTempPerm467) + ((0.0284124 * fTempPerm1551) + ((0.0169305 * fTempPerm1554) + ((0.0106017 * fTempPerm468) + ((6.59979e-08 * fTempPerm470) + ((4.2136500000000006e-05 * fTempPerm471) + ((0.0005151690000000001 * fTempPerm472) + ((0.00159566 * fTempPerm473) + ((7.21156e-07 * fTempPerm475) + ((0.00106162 * fTempPerm1563) + ((0.00140911 * fTempPerm482) + ((0.00155444 * fTempPerm1564) + ((0.00186255 * fTempPerm483) + ((0.013509400000000001 * fTempPerm485) + ((0.000503369 * fTempPerm1567) + ((8.17412e-06 * fTempPerm1570) + ((2.9996500000000003e-05 * fTempPerm495) + ((0.00022863600000000002 * fTempPerm1571) + ((0.000550446 * fTempPerm497) + ((0.00032662200000000005 * fTempPerm1573) + ((0.000650443 * fTempPerm498) + ((0.00012959 * fTempPerm1574) + ((0.0157848 * fTempPerm1575) + ((0.00017575500000000001 * fTempPerm503) + ((0.000103707 * fTempPerm1576) + ((0.00200028 * fTempPerm504) + ((9.867810000000001e-05 * fTempPerm507) + ((1.41881e-06 * fTempPerm508) + ((3.31876e-06 * fTempPerm510) + ((3.2490500000000003e-05 * fTempPerm511) + ((0.00226215 * fTempPerm1580) + ((0.00390744 * fTempPerm514) + ((0.0011997199999999998 * fTempPerm1587) + ((0.00615266 * fTempPerm1588) + ((0.00222904 * fTempPerm516) + ((0.0125606 * fTempPerm517) + ((0.000107706 * fTempPerm518) + ((0.00127378 * fTempPerm519) + ((0.00842323 * fTempPerm520) + ((1.8977800000000003e-05 * fTempPerm524) + ((1.9102600000000002e-05 * fTempPerm1592) + ((9.57012e-05 * fTempPerm525) + ((5.5900900000000005e-05 * fTempPerm1593) + ((0.00251957 * fTempPerm529) + ((0.00118894 * fTempPerm531) + ((0.00145085 * fTempPerm532) + ((0.000827394 * fTempPerm533) + ((0.00227417 * fTempPerm1605) + ((0.00023654000000000004 * fTempPerm1607) + ((5.6889900000000006e-05 * fTempPerm1608) + ((0.000283063 * fTempPerm1609) + ((0.0028355700000000004 * fTempPerm1611) + ((0.0011901600000000002 * fTempPerm1612) + ((0.00159933 * fTempPerm1613) + ((0.0008315060000000001 * fTempPerm1614) + ((0.00638856 * fTempPerm541) + ((0.027778900000000002 * fTempPerm1615) + ((0.0197598 * fTempPerm1617) + ((0.025745200000000003 * fTempPerm1618) + ((0.0028674200000000003 * fTempPerm1619) + ((0.0237538 * fTempPerm1620) + ((0.0209338 * fTempPerm1621) + ((0.0134953 * fTempPerm1622) + ((0.00912445 * fTempPerm544) + ((0.0275179 * fTempPerm1626) + ((0.060376700000000005 * fTempPerm1627) + ((0.06088860000000001 * fTempPerm1628) + ((0.00512142 * fTempPerm1629) + ((0.00126033 * fTempPerm1630) + ((2.08207e-05 * fTempPerm551) + ((0.000532984 * fTempPerm1632) + ((6.278260000000001e-05 * fTempPerm553) + ((7.65605e-05 * fTempPerm1633) + ((0.00263347 * fTempPerm555) + ((0.0020205099999999997 * fTempPerm557) + ((0.000275005 * fTempPerm558) + ((0.0014331 * fTempPerm1643) + ((0.00436247 * fTempPerm560) + ((0.007149600000000001 * fTempPerm561) + ((0.000638058 * fTempPerm563) + ((0.00122649 * fTempPerm564) + ((0.0005237470000000001 * fTempPerm565) + ((0.0011231 * fTempPerm566) + ((0.000103257 * fTempPerm567) + ((0.00171093 * fTempPerm1648) + ((0.0015055300000000001 * fTempPerm569) + ((0.00129704 * fTempPerm570) + ((0.00394994 * fTempPerm571) + ((0.00042873700000000005 * fTempPerm572) + ((0.00645499 * fTempPerm1650) + ((0.009424450000000001 * fTempPerm1651) + ((0.0011423000000000002 * fTempPerm573) + ((0.000263627 * fTempPerm1652) + ((0.005743590000000001 * fTempPerm1656) + ((0.000618073 * fTempPerm576) + ((2.1505e-05 * fTempPerm1658) + ((0.00041380000000000003 * fTempPerm577) + ((1.1025e-06 * fTempPerm578) + ((0.0010346200000000002 * fTempPerm579) + ((0.000129941 * fTempPerm580) + ((0.000501459 * fTempPerm581) + ((0.000493628 * fTempPerm582) + ((7.1971e-05 * fTempPerm1659) + ((0.00101923 * fTempPerm584) + ((0.000348142 * fTempPerm1661) + ((9.494400000000002e-05 * fTempPerm1662) + ((0.00021672300000000002 * fTempPerm1663) + ((0.00152519 * fTempPerm586) + ((0.0006341350000000001 * fTempPerm1664) + ((3.47604e-08 * fTempPerm588) + ((0.00027668200000000005 * fTempPerm592) + ((0.00267881 * fTempPerm1672) + ((0.00033735 * fTempPerm598) + ((0.000575071 * fTempPerm1673) + ((0.000637521 * fTempPerm599) + ((0.0013227800000000002 * fTempPerm1674) + ((0.00114676 * fTempPerm1675) + ((0.00722064 * fTempPerm1676) + ((0.0017853699999999999 * fTempPerm1678) + ((0.0034082300000000004 * fTempPerm1679) + ((0.000136011 * fTempPerm607) + ((0.00011103000000000001 * fTempPerm1680) + ((4.44119e-05 * fTempPerm1681) + ((0.0012166300000000002 * fTempPerm1682) + ((0.00109789 * fTempPerm1683) + ((0.00171149 * fTempPerm1684) + ((0.000730135 * fTempPerm1685) + ((0.00044798200000000004 * fTempPerm611) + ((0.00347187 * fTempPerm1686) + ((0.027225600000000003 * fTempPerm1687) + ((0.000870882 * fTempPerm612) + ((0.00946403 * fTempPerm1688) + ((0.00387811 * fTempPerm614) + ((0.0480442 * fTempPerm1690) + ((0.0047823300000000004 * fTempPerm1691) + ((0.00920931 * fTempPerm1693) + ((0.0315938 * fTempPerm1694) + ((0.0182515 * fTempPerm1695) + ((0.0125871 * fTempPerm617) + ((0.00107937 * fTempPerm1696) + ((0.0015448500000000002 * fTempPerm1697) + ((0.000234556 * fTempPerm1698) + ((0.00321454 * fTempPerm1699) + ((0.00040021800000000004 * fTempPerm1701) + ((0.00188691 * fTempPerm1702) + ((0.00011697900000000002 * fTempPerm622) + ((0.0016790700000000002 * fTempPerm623) + ((0.00406262 * fTempPerm624) + ((0.0023261500000000004 * fTempPerm625) + ((0.00109229 * fTempPerm626) + ((5.530960000000001e-05 * fTempPerm627) + ((0.00011918500000000002 * fTempPerm628) + ((0.00102589 * fTempPerm629) + ((0.00221504 * fTempPerm630) + ((3.8517100000000004e-05 * fTempPerm631) + ((0.000178551 * fTempPerm632) + ((0.0110775 * fTempPerm634) + ((0.0292041 * fTempPerm636) + ((0.015611 * fTempPerm637) + ((0.000558749 * fTempPerm639) + ((0.0131959 * fTempPerm640) + ((0.0033821 * fTempPerm641) + ((0.000654334 * fTempPerm642) + ((0.00012754700000000002 * fTempPerm1717) + ((0.000286004 * fTempPerm643) + ((0.000569647 * fTempPerm644) + ((3.4954000000000004e-05 * fTempPerm645) + ((9.322950000000002e-05 * fTempPerm646) + ((0.0013280499999999999 * fTempPerm648) + ((0.00173031 * fTempPerm649) + ((0.0009061030000000001 * fTempPerm650) + ((0.0020406 * fTempPerm651) + ((0.00174807 * fTempPerm652) + ((0.00102092 * fTempPerm1725) + ((0.0021519 * fTempPerm653) + ((0.000184975 * fTempPerm654) + ((0.000330976 * fTempPerm1726) + ((0.00669066 * fTempPerm657) + ((0.0092729 * fTempPerm1727) + ((0.022299000000000003 * fTempPerm658) + ((0.0012059800000000002 * fTempPerm659) + ((0.0007695660000000001 * fTempPerm660) + ((7.080070000000001e-05 * fTempPerm1729) + ((0.000231146 * fTempPerm661) + ((0.000147661 * fTempPerm662) + ((7.665409999999998e-06 * fTempPerm1733) + ((3.69732e-06 * fTempPerm1735) + ((0.000165502 * fTempPerm1738) + ((0.000158936 * fTempPerm1739) + ((1.5261800000000004e-05 * fTempPerm666) + ((0.00030627700000000004 * fTempPerm1740) + ((0.000368308 * fTempPerm1741) + ((0.000239084 * fTempPerm1742) + ((0.00164631 * fTempPerm1743) + ((0.00128867 * fTempPerm1744) + ((0.000377902 * fTempPerm1745) + ((0.005800029999999999 * fTempPerm1746) + ((0.056895100000000004 * fTempPerm1747) + ((0.010567 * fTempPerm1748) + ((0.012867100000000001 * fTempPerm1749) + ((0.009551539999999999 * fTempPerm672) + ((0.000116086 * fTempPerm1751) + ((0.00105812 * fTempPerm673) + ((0.000244418 * fTempPerm1753) + ((0.0022682500000000003 * fTempPerm674) + ((0.00231892 * fTempPerm675) + ((0.000274873 * fTempPerm676) + ((0.014908999999999999 * fTempPerm677) + ((0.0180855 * fTempPerm1757) + ((3.8369700000000006e-05 * fTempPerm1758) + ((0.000296876 * fTempPerm1759) + ((0.000312487 * fTempPerm1760) + ((0.00016036600000000003 * fTempPerm1761) + ((0.00129178 * fTempPerm1764) + ((0.0016883999999999998 * fTempPerm687) + ((0.00183846 * fTempPerm688) + ((0.000339129 * fTempPerm689) + ((0.0020292500000000002 * fTempPerm1766) + ((2.4813999999999997e-06 * fTempPerm1767) + ((0.00021603300000000002 * fTempPerm691) + ((0.00029935000000000004 * fTempPerm693) + ((0.00229648 * fTempPerm1769) + ((0.00147025 * fTempPerm1770) + ((0.00022606700000000002 * fTempPerm1771) + ((0.00708753 * fTempPerm1772) + ((0.00634478 * fTempPerm701) + ((0.00024025900000000002 * fTempPerm1774) + ((0.000195567 * fTempPerm1775) + ((0.00037747700000000003 * fTempPerm1776) + ((0.000319806 * fTempPerm1777) + ((0.0012796 * fTempPerm708) + ((3.63852e-06 * fTempPerm709) + ((0.00154955 * fTempPerm710) + ((0.0005140090000000001 * fTempPerm1779) + ((0.00199607 * fTempPerm711) + ((0.000784338 * fTempPerm712) + ((0.018965700000000002 * fTempPerm1781) + ((0.0134784 * fTempPerm1782) + ((2.95878e-05 * fTempPerm717) + ((0.00010999399999999999 * fTempPerm718) + ((0.00379148 * fTempPerm719) + ((0.00637634 * fTempPerm720) + ((0.0006416280000000001 * fTempPerm1790) + ((1.57539e-07 * fTempPerm721) + ((0.0010047600000000002 * fTempPerm723) + ((1.0191e-05 * fTempPerm725) + ((0.0009547269999999999 * fTempPerm727) + ((0.0009277500000000001 * fTempPerm728) + ((0.0013637999999999999 * fTempPerm1795) + ((0.00203837 * fTempPerm729) + ((0.0024844 * fTempPerm731) + ((0.00125953 * fTempPerm1796) + ((0.000559696 * fTempPerm1797) + ((0.000743556 * fTempPerm732) + ((0.000897913 * fTempPerm1798) + ((0.0028104300000000005 * fTempPerm733) + ((0.000775016 * fTempPerm734) + ((0.000258758 * fTempPerm738) + ((0.000954181 * fTempPerm1803) + ((0.00295888 * fTempPerm744) + ((0.0139764 * fTempPerm745) + ((0.00867053 * fTempPerm746) + ((0.00939652 * fTempPerm747) + ((0.0163858 * fTempPerm748) + ((0.0152545 * fTempPerm1810) + ((0.041638400000000006 * fTempPerm749) + ((4.490090000000001e-05 * fTempPerm1811) + ((0.020153599999999997 * fTempPerm751) + ((0.04143 * fTempPerm752) + ((0.00508388 * fTempPerm762) + ((7.510190000000001e-05 * fTempPerm763) + ((0.0024348900000000003 * fTempPerm1818) + ((0.00341205 * fTempPerm766) + ((0.00206245 * fTempPerm767) + ((0.0368006 * fTempPerm1819) + ((0.016894100000000002 * fTempPerm1821) + ((0.015908 * fTempPerm1822) + ((0.00139284 * fTempPerm773) + ((0.0111489 * fTempPerm1823) + ((0.00048254200000000003 * fTempPerm774) + ((0.0008444020000000001 * fTempPerm1826) + ((0.000148373 * fTempPerm1827) + ((0.0009807190000000001 * fTempPerm1828) + ((0.0009296310000000001 * fTempPerm785) + ((0.00244952 * fTempPerm786) + ((0.0026210900000000004 * fTempPerm1830) + ((0.00107755 * fTempPerm1831) + ((0.006148590000000001 * fTempPerm1832) + ((0.00108131 * fTempPerm1833) + ((0.00149292 * fTempPerm1835) + ((0.000624781 * fTempPerm1836) + ((0.00028939400000000006 * fTempPerm1837) + ((5.56606e-06 * fTempPerm794) + ((4.2207000000000004e-05 * fTempPerm1838) + ((5.6691300000000004e-05 * fTempPerm1839) + ((0.00198173 * fTempPerm1840) + ((0.00150447 * fTempPerm795) + ((0.00111368 * fTempPerm796) + ((0.000701284 * fTempPerm1847) + ((0.00040873600000000004 * fTempPerm1848) + ((0.0130594 * fTempPerm797) + ((0.021176 * fTempPerm798) + ((0.000122408 * fTempPerm800) + ((0.00354482 * fTempPerm801) + ((0.00207781 * fTempPerm802) + ((0.00235812 * fTempPerm804) + ((1.0045e-05 * fTempPerm1081) + ((0.000386788 * fTempPerm2042) + ((0.00013990400000000002 * fTempPerm2041) + ((0.0032282 * fTempPerm1080) + ((8.505850000000001e-06 * fTempPerm2039) + ((0.00425301 * fTempPerm805) + ((3.71676e-05 * fTempPerm2038) + ((3.39827e-05 * fTempPerm1855) + ((0.00011888700000000002 * fTempPerm1856) + ((2.6751600000000003e-05 * fTempPerm1857) + ((0.000154297 * fTempPerm810) + ((0.00116433 * fTempPerm813) + ((0.00445499 * fTempPerm1859) + ((0.00232849 * fTempPerm1860) + ((0.0143637 * fTempPerm1861) + ((0.0122635 * fTempPerm1862) + ((0.000542053 * fTempPerm1863) + ((0.0030369999999999998 * fTempPerm1864) + ((0.017027300000000002 * fTempPerm1865) + ((1.74779e-06 * fTempPerm818) + ((5.5996800000000006e-05 * fTempPerm819) + ((0.000161141 * fTempPerm820) + ((0.0033512100000000003 * fTempPerm821) + ((0.000959789 * fTempPerm822) + ((0.00365475 * fTempPerm823) + ((0.0026745600000000003 * fTempPerm824) + ((0.0036032700000000004 * fTempPerm825) + ((0.00530227 * fTempPerm826) + ((0.00337264 * fTempPerm827) + ((0.0067297599999999996 * fTempPerm828) + ((0.00648151 * fTempPerm829) + ((4.49754e-06 * fTempPerm830) + ((0.00464837 * fTempPerm831) + ((0.000671666 * fTempPerm832) + ((0.0140384 * fTempPerm833) + ((0.004986290000000001 * fTempPerm834) + ((0.0014525200000000001 * fTempPerm835) + ((0.006066569999999999 * fTempPerm836) + ((0.00565642 * fTempPerm837) + ((0.0272114 * fTempPerm838) + ((0.111697 * fTempPerm839) + ((0.0503806 * fTempPerm840) + ((0.0697323 * fTempPerm841) + ((0.0414059 * fTempPerm842) + (((2.87613e-06 * fTempPerm1078) + ((3.51439e-07 * fVec1[IOTA&127]) + ((6.58295e-06 * fTempPerm1077) + ((3.2773799999999996e-06 * fVec3[IOTA&127]) + ((1.31127e-05 * fVec4[IOTA&127]) + ((1.2689500000000001e-05 * fTempPerm1870) + ((2.9599300000000003e-05 * fTempPerm1871) + ((1.8915500000000002e-05 * fVec6[IOTA&127]) + ((0.0254229 * fTempPerm1872) + ((0.000195057 * fTempPerm843) + ((0.0142102 * fTempPerm1873) + ((0.000508036 * fTempPerm844) + ((0.00130975 * fTempPerm845) + ((0.0016259900000000001 * fTempPerm846) + ((0.0012236 * fTempPerm847) + ((0.00147426 * fTempPerm848) + ((0.00175057 * fTempPerm850) + ((0.00105509 * fTempPerm1875) + ((0.00266924 * fTempPerm852) + ((0.00072404 * fTempPerm853) + ((0.0026122100000000002 * fTempPerm855) + ((0.00463264 * fTempPerm856) + ((0.00538217 * fTempPerm857) + ((0.00139635 * fTempPerm859) + ((0.000209301 * fTempPerm860) + ((0.0105579 * fTempPerm861) + ((0.00729384 * fTempPerm862) + ((0.0007157 * fTempPerm1876) + ((0.008159850000000001 * fTempPerm863) + ((0.008019420000000001 * fTempPerm865) + ((0.0105391 * fTempPerm866) + ((0.0216826 * fTempPerm867) + ((0.017730600000000003 * fTempPerm868) + ((0.0075229400000000005 * fTempPerm869) + ((0.0123141 * fTempPerm1877) + ((0.0424237 * fTempPerm870) + ((0.0368012 * fTempPerm871) + ((0.05446430000000001 * fTempPerm872) + ((4.16914e-05 * fTempPerm874) + ((0.00280241 * fTempPerm1879) + ((0.00207868 * fTempPerm1880) + ((0.0009198190000000001 * fTempPerm1881) + ((0.0002275 * fTempPerm1882) + ((0.000531922 * fTempPerm885) + ((0.000148231 * fTempPerm886) + ((7.3547e-05 * fTempPerm1883) + ((0.0013802 * fTempPerm887) + ((0.00345985 * fTempPerm1884) + ((2.2315100000000003e-05 * fTempPerm1886) + ((0.00365462 * fTempPerm1888) + ((6.14464e-06 * fTempPerm890) + ((4.6366800000000005e-05 * fTempPerm2035) + ((0.00015426800000000002 * fTempPerm894) + ((0.0011816700000000001 * fTempPerm1889) + ((0.00100478 * fTempPerm1891) + ((0.00041635900000000004 * fTempPerm1892) + ((0.000334401 * fTempPerm896) + ((0.00168163 * fTempPerm897) + ((0.000996385 * fTempPerm1895) + ((0.00685873 * fTempPerm901) + ((0.0170789 * fTempPerm1898) + ((8.76003e-06 * fTempPerm908) + ((0.00582195 * fTempPerm1900) + ((5.14829e-06 * fTempPerm1901) + ((0.000115798 * fTempPerm1905) + ((0.00275175 * fTempPerm911) + ((0.00121048 * fTempPerm914) + ((0.00119525 * fTempPerm915) + ((0.0115422 * fTempPerm1914) + ((0.0243674 * fTempPerm917) + ((0.0537481 * fTempPerm918) + ((0.000659975 * fTempPerm1917) + ((1.8649e-06 * fTempPerm919) + ((1.9873400000000002e-05 * fTempPerm920) + ((0.000876507 * fTempPerm1923) + ((0.00297761 * fTempPerm925) + ((0.0054466 * fTempPerm929) + ((0.00403937 * fTempPerm931) + ((7.19018e-05 * fTempPerm1931) + ((0.00022708 * fTempPerm1932) + ((0.009235 * fTempPerm934) + ((0.000251322 * fTempPerm1934) + ((0.00021972700000000002 * fTempPerm937) + ((0.033864 * fTempPerm939) + ((0.0020450900000000003 * fTempPerm1937) + ((0.00227327 * fTempPerm1939) + ((0.00389261 * fTempPerm1940) + ((0.0057548899999999995 * fTempPerm1941) + ((0.0109805 * fTempPerm1942) + ((0.0110121 * fTempPerm1943) + ((0.011685700000000002 * fTempPerm1944) + ((0.019806900000000002 * fTempPerm1945) + ((0.0166992 * fTempPerm1946) + ((0.0121522 * fTempPerm1947) + ((0.0175694 * fTempPerm1948) + ((0.0173035 * fTempPerm1949) + ((0.0160724 * fTempPerm1950) + ((0.016965 * fTempPerm1951) + ((0.0186217 * fTempPerm1952) + ((0.017693 * fTempPerm1953) + ((0.0232959 * fTempPerm1954) + ((0.030269900000000002 * fTempPerm1955) + ((0.0333365 * fTempPerm1956) + ((0.029073099999999998 * fTempPerm1957) + ((7.13748e-06 * fTempPerm942) + ((1.69708e-05 * fTempPerm943) + ((0.00038352400000000005 * fTempPerm944) + ((0.000502614 * fTempPerm945) + ((0.00050422 * fTempPerm946) + ((0.00100501 * fTempPerm947) + ((0.0013272 * fTempPerm948) + ((0.00059498 * fTempPerm949) + ((0.0025739 * fTempPerm950) + ((2.5156400000000002e-05 * fTempPerm952) + ((0.00144247 * fTempPerm954) + ((0.00204096 * fTempPerm955) + ((0.00201999 * fTempPerm956) + ((0.00390875 * fTempPerm957) + ((0.0028127900000000003 * fTempPerm958) + ((0.0016933400000000002 * fTempPerm959) + ((0.00338193 * fTempPerm960) + ((0.0042595 * fTempPerm961) + ((0.00292299 * fTempPerm962) + ((0.00143566 * fTempPerm963) + ((0.00846299 * fTempPerm964) + ((0.00431154 * fTempPerm965) + ((0.00012713900000000002 * fTempPerm967) + ((1.96889e-05 * fVec15[IOTA&127]) + ((2.00819e-05 * fTempPerm1960) + ((5.35095e-06 * fVec8[IOTA&127]) + ((2.3258800000000005e-05 * fTempPerm970) + ((1.5002200000000002e-05 * fVec12[IOTA&127]) + ((0.00756406 * fTempPerm1963) + ((0.0030812 * fTempPerm972) + ((0.00796032 * fTempPerm973) + ((0.00884039 * fTempPerm974) + ((4.14705e-05 * fTempPerm1967) + ((9.12388e-06 * fTempPerm1968) + ((0.00415929 * fTempPerm975) + ((0.00108066 * fTempPerm977) + ((0.00817955 * fTempPerm978) + ((1.48337e-06 * fTempPerm1970) + ((0.0006599420000000001 * fTempPerm984) + ((0.0017905 * fTempPerm1973) + ((0.00015571700000000001 * fTempPerm1084) + ((0.0039002500000000005 * fTempPerm1975) + ((0.00740671 * fTempPerm1976) + ((0.00468815 * fTempPerm1977) + ((0.00790244 * fTempPerm1978) + ((0.0188991 * fTempPerm1979) + ((0.023498 * fTempPerm1980) + ((0.024658199999999998 * fTempPerm1981) + ((0.023601800000000003 * fTempPerm1982) + ((0.033071500000000004 * fTempPerm1983) + ((0.000275169 * fTempPerm1985) + ((0.000202164 * fTempPerm1001) + ((0.000426597 * fTempPerm1002) + ((0.00103368 * fTempPerm1004) + ((0.0007092450000000001 * fTempPerm1005) + ((0.000779071 * fTempPerm1006) + ((0.0009450379999999999 * fTempPerm1007) + ((0.0015368900000000002 * fTempPerm1988) + ((0.00193882 * fTempPerm1989) + ((0.00293818 * fTempPerm1009) + ((0.00198287 * fTempPerm1010) + ((0.0038801100000000004 * fTempPerm1011) + ((0.021074000000000002 * fTempPerm1012) + ((0.00380208 * fTempPerm1013) + ((0.00278079 * fTempPerm1014) + ((0.0006431350000000001 * fTempPerm1990) + ((8.304339999999999e-06 * fTempPerm1991) + ((0.0018129 * fTempPerm1996) + ((8.174099999999999e-06 * fTempPerm1016) + ((0.0012425 * fTempPerm1997) + ((1.07578e-06 * fTempPerm1998) + ((7.39806e-07 * fTempPerm1019) + ((0.00219487 * fTempPerm2004) + ((0.00210505 * fTempPerm1021) + ((0.000987828 * fTempPerm1022) + ((0.00195065 * fTempPerm1023) + ((0.00206534 * fTempPerm1024) + ((0.00273936 * fTempPerm1025) + ((0.000358363 * fTempPerm1028) + ((1.26739e-05 * fTempPerm2015) + ((0.004869989999999999 * fTempPerm2017) + ((0.00201686 * fTempPerm1031) + ((0.0028706900000000004 * fTempPerm1032) + ((0.0051357799999999995 * fTempPerm2018) + ((0.0227996 * fTempPerm2019) + ((0.00011673199999999999 * fTempPerm1034) + ((((0.00033741 * fTempPerm2021) + (((((0.00373392 * fTempPerm1042) + ((0.000537327 * fTempPerm1045) + ((0.00438648 * fTempPerm1046) + ((0.0027836500000000004 * fTempPerm2029) + ((0.0258155 * fTempPerm1047) + ((0.000117308 * fTempPerm1050) + ((0.047424299999999996 * fTempPerm1051) + ((0.0813459 * fTempPerm1052) + ((0.043948600000000004 * fTempPerm1053) + ((0.0622112 * fTempPerm1054) + ((0.0466987 * fTempPerm1055) + ((0.0530012 * fTempPerm1056) + ((0.0489414 * fTempPerm1057) + ((0.0007391530000000001 * fTempPerm1058) + ((0.0020685400000000002 * fTempPerm1059) + ((0.000557527 * fTempPerm1060) + ((0.00107368 * fTempPerm1061) + ((0.00105561 * fTempPerm1062) + ((0.000646817 * fTempPerm1063) + ((0.000782518 * fTempPerm1064) + ((0.000840054 * fTempPerm1065) + ((0.0007244580000000001 * fTempPerm1066) + (((0.000644241 * fTempPerm2033) + ((0.00016541 * fTempPerm1071) + (((0.00184199 * fTempPerm1074) + (0.0039974 * fTempPerm1075)) + (2.0505700000000003e-05 * fTempPerm2034)))) + (0.0012429300000000002 * fTempPerm2032)))))))))))))))))))))))) + (0.000769439 * fTempPerm1041)) + (0.000654404 * fTempPerm1039)) + (0.009442299999999999 * fTempPerm2025))) + (0.000604909 * fTempPerm1035)) + (0.00742411 * fTempPerm1076)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (5.47506e-07 * fTempPerm2037)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.0007135570000000001 * fTempPerm402)) + (0.000622619 * fTempPerm401))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.00421747 * fTempPerm1266)) + (0.00396111 * fTempPerm166)))))))))))))))) + (5.2133700000000005e-05 * fTempPerm149))) + (0.0019904 * fTempPerm146)) + (1.75291e-05 * fTempPerm145))))))))))))))))))))))))))))))))))))))))))))) + (0.000784532 * fTempPerm104))))))))))) + (0.00110327 * fTempPerm95)) + (0.000722126 * fTempPerm1171))) + (0.000397475 * fTempPerm93)))) + (0.00035911300000000004 * fTempPerm1166)) + (3.73611e-06 * fTempPerm90))))))))))))))))))))) + (0.00449539 * fTempPerm72))))))))))) + (0.0014990399999999999 * fTempPerm60))) + (0.000140679 * fTempPerm59)) + (0.0021076800000000002 * fTempPerm58)) + (0.000204947 * fTempPerm1130)) + (0.00199225 * fTempPerm57)) + (0.000163275 * fTempPerm1128))) + (9.58706e-07 * fTempPerm55))))))))))))))))))))))))))))))))))))))))))))))))) - ((0.000129415 * fTempPerm1086) + (((5.784670000000001e-05 * fTempPerm1089) + ((0.029996000000000002 * fTempPerm1090) + ((0.0016476 * fTempPerm1091) + ((0.000439844 * fTempPerm1093) + ((0.000274811 * fTempPerm16) + ((0.00017124500000000002 * fTempPerm19) + ((0.0015949800000000002 * fTempPerm1095) + ((0.0019218100000000001 * fTempPerm21) + ((0.000594883 * fTempPerm22) + ((0.00121283 * fTempPerm23) + ((0.00122613 * fTempPerm1099) + ((0.004733660000000001 * fTempPerm25) + ((0.00449193 * fTempPerm1100) + ((0.007260180000000001 * fTempPerm1101) + ((0.000177306 * fTempPerm26) + ((0.000833375 * fTempPerm29) + ((0.0008480380000000001 * fTempPerm1103) + ((0.0012552099999999999 * fTempPerm1105) + ((0.000245544 * fTempPerm33) + ((1.2154900000000001e-05 * fTempPerm36) + ((7.103729999999999e-06 * fTempPerm1106) + ((0.000611966 * fTempPerm1107) + ((0.00014579800000000002 * fTempPerm1108) + ((0.00042185499999999995 * fTempPerm1112) + ((0.000443774 * fTempPerm1113) + ((0.00199081 * fTempPerm46) + ((0.00329516 * fTempPerm1114) + ((0.000296216 * fTempPerm1115) + ((0.00314607 * fTempPerm1116) + ((0.0050431600000000005 * fTempPerm1117) + ((0.0022291200000000002 * fTempPerm1119) + ((0.00146599 * fTempPerm49) + ((0.00010595300000000001 * fTempPerm1120) + ((6.23196e-05 * fTempPerm1121) + (((((0.00040135500000000005 * fTempPerm1125) + (((0.000178548 * fTempPerm1127) + ((0.000775252 * fTempPerm1129) + (((0.000413929 * fTempPerm1133) + ((0.0016410700000000001 * fTempPerm61) + ((0.0028929999999999997 * fTempPerm1134) + ((0.000588588 * fTempPerm1136) + ((0.008901210000000001 * fTempPerm1138) + ((0.00116543 * fTempPerm63) + ((0.00655393 * fTempPerm64) + ((1.1903999999999999e-06 * fTempPerm1139) + ((0.000618854 * fTempPerm66) + ((0.000917038 * fTempPerm67) + ((0.000134809 * fTempPerm1141) + ((0.0021823700000000003 * fTempPerm69) + ((0.00302825 * fTempPerm70) + ((0.00120555 * fTempPerm1144) + ((0.0011470100000000002 * fTempPerm1145) + ((0.00591923 * fTempPerm1146) + ((0.0247151 * fTempPerm1148) + ((0.00305803 * fTempPerm1149) + ((0.00317145 * fTempPerm76) + ((3.9907800000000004e-05 * fTempPerm80) + ((0.00063397 * fTempPerm1153) + ((0.0011997 * fTempPerm1154) + ((0.004189490000000001 * fTempPerm1155) + ((0.000114272 * fTempPerm84) + ((0.0023553899999999997 * fTempPerm85) + ((0.004434090000000001 * fTempPerm1158) + ((0.00131632 * fTempPerm89) + ((0.020928 * fTempPerm1160) + ((5.3289600000000006e-05 * fTempPerm1161) + ((0.000345911 * fTempPerm1162) + ((0.000562304 * fTempPerm1163) + ((((0.000134423 * fTempPerm94) + ((0.0014493400000000001 * fTempPerm1167) + ((0.000279138 * fTempPerm1169) + ((0.00246309 * fTempPerm1170) + ((0.00199292 * fTempPerm1172) + ((0.0063972 * fTempPerm1173) + (((0.00714622 * fTempPerm1174) + ((0.00487891 * fTempPerm1175) + ((0.00955486 * fTempPerm1177) + ((0.0199525 * fTempPerm1178) + ((0.0254705 * fTempPerm1180) + ((0.0029659300000000003 * fTempPerm99) + ((0.00838542 * fTempPerm1181) + (((7.658879999999999e-06 * fTempPerm1183) + ((5.52051e-06 * fTempPerm1184) + ((6.66399e-05 * fTempPerm1185) + ((0.00066768 * fTempPerm103) + ((4.423860000000001e-05 * fTempPerm1187) + (((0.00148747 * fTempPerm1189) + (((0.000553977 * fTempPerm1191) + (((0.00014092800000000002 * fTempPerm1193) + ((0.00453353 * fTempPerm1196) + ((1.4907900000000002e-05 * fTempPerm1197) + ((3.26185e-05 * fTempPerm1198) + ((4.2858599999999997e-05 * fTempPerm1199) + ((4.13336e-05 * fTempPerm1200) + ((0.00023944000000000002 * fTempPerm1201) + ((0.00275839 * fTempPerm1204) + ((0.00282809 * fTempPerm1206) + ((0.000700189 * fTempPerm1207) + ((0.00331393 * fTempPerm1208) + ((0.00655955 * fTempPerm1209) + ((0.00908867 * fTempPerm1210) + ((0.00624609 * fTempPerm1211) + ((0.012640400000000001 * fTempPerm1212) + ((0.00686843 * fTempPerm1214) + ((0.00143594 * fTempPerm1215) + ((3.09197e-05 * fTempPerm1216) + ((1.37302e-05 * fTempPerm1217) + ((0.00044645700000000004 * fTempPerm1218) + ((0.00021416200000000003 * fTempPerm122) + ((1.0293200000000002e-05 * fTempPerm1219) + ((0.00040871000000000006 * fTempPerm1222) + ((0.00239894 * fTempPerm1223) + ((0.0031728900000000003 * fTempPerm1224) + ((0.0014167300000000002 * fTempPerm126) + ((0.00393236 * fTempPerm1225) + ((0.00168373 * fTempPerm127) + ((0.00032871800000000003 * fTempPerm128) + ((0.0013171900000000002 * fTempPerm1227) + ((0.0043833000000000006 * fTempPerm130) + ((0.00348297 * fTempPerm1229) + ((0.00244034 * fTempPerm133) + ((0.0101068 * fTempPerm1231) + ((0.013501399999999998 * fTempPerm135) + ((0.0160074 * fTempPerm1232) + ((0.0206794 * fTempPerm1233) + ((0.032838599999999996 * fTempPerm1235) + ((0.00718646 * fTempPerm1236) + ((0.00164934 * fTempPerm1237) + ((0.005088560000000001 * fTempPerm1238) + ((0.000944821 * fTempPerm1239) + ((7.61445e-05 * fTempPerm140) + ((0.00057894 * fTempPerm1240) + ((0.00159128 * fTempPerm1241) + ((0.00015391 * fTempPerm1242) + ((0.000362138 * fTempPerm1243) + ((((1.2323300000000001e-05 * fTempPerm147) + ((0.000512483 * fTempPerm1247) + ((0.0008637740000000001 * fTempPerm1248) + ((0.000107805 * fTempPerm1249) + ((((0.000668844 * fTempPerm1251) + ((0.00080711 * fTempPerm1252) + ((0.0015733 * fTempPerm151) + ((0.0017782899999999999 * fTempPerm1253) + ((0.0009574030000000001 * fTempPerm1254) + ((0.00047008900000000003 * fTempPerm1255) + ((0.039960800000000005 * fTempPerm1258) + ((((8.95919e-06 * fTempPerm1259) + ((0.000361373 * fTempPerm1261) + (((0.0006546770000000001 * fTempPerm1262) + ((0.00108317 * fTempPerm1263) + ((0.00119166 * fTempPerm1264) + ((0.000167615 * fTempPerm165) + (((0.00329005 * fTempPerm167) + ((0.00194991 * fTempPerm1267) + ((0.0098116 * fTempPerm1269) + ((0.00691913 * fTempPerm169) + ((0.0033760599999999997 * fTempPerm1270) + ((0.00365339 * fTempPerm171) + ((0.0209074 * fTempPerm1271) + ((0.0101589 * fTempPerm176) + ((0.0164583 * fTempPerm1272) + ((0.021493699999999998 * fTempPerm1274) + ((0.006270520000000001 * fTempPerm1275) + ((0.004882340000000001 * fTempPerm1276) + (((3.209350000000001e-05 * fTempPerm180) + ((6.55232e-06 * fTempPerm1278) + ((8.19063e-05 * fTempPerm183) + (((8.30032e-05 * fTempPerm185) + ((0.000936929 * fTempPerm186) + ((1.2403399999999998e-06 * fTempPerm1279) + ((0.0016863 * fTempPerm1280) + ((0.0006430040000000001 * fTempPerm1281) + ((0.000509173 * fTempPerm187) + ((0.00032057000000000006 * fTempPerm193) + ((0.00121634 * fTempPerm1282) + ((0.000795623 * fTempPerm1283) + ((0.0010627800000000001 * fTempPerm1284) + ((0.00257823 * fTempPerm1285) + ((0.00139712 * fTempPerm1286) + ((0.00308075 * fTempPerm198) + ((0.0298252 * fTempPerm1287) + ((5.80166e-06 * fTempPerm1288) + ((0.0011202900000000001 * fTempPerm201) + ((2.28937e-05 * fTempPerm1289) + ((7.539579999999999e-06 * fTempPerm203) + ((0.00022978 * fTempPerm1290) + ((0.000414409 * fTempPerm204) + ((3.0152300000000003e-05 * fTempPerm1291) + ((0.000716831 * fTempPerm1292) + ((8.949310000000001e-05 * fTempPerm1294) + ((0.0009427070000000001 * fTempPerm1295) + ((0.0009128040000000001 * fTempPerm208) + ((2.2224499999999998e-06 * fTempPerm1297) + ((0.00029154800000000003 * fTempPerm1299) + ((0.000281437 * fTempPerm216) + ((0.000877149 * fTempPerm217) + ((0.000581171 * fTempPerm218) + ((0.00105405 * fTempPerm1301) + ((1.0579500000000001e-05 * fTempPerm1302) + ((0.000179294 * fTempPerm1303) + ((9.090940000000001e-05 * fTempPerm1304) + ((0.000171535 * fTempPerm1305) + ((9.110269999999999e-07 * fTempPerm1306) + ((0.000151989 * fTempPerm222) + ((0.00144718 * fTempPerm1307) + ((0.00118608 * fTempPerm1308) + ((0.000713603 * fTempPerm223) + ((0.000243654 * fTempPerm1310) + ((0.00032285800000000003 * fTempPerm224) + ((0.00208251 * fTempPerm225) + ((3.15558e-05 * fTempPerm228) + ((0.00169048 * fTempPerm229) + ((0.00271579 * fTempPerm231) + ((5.21864e-05 * fTempPerm1312) + ((5.6034300000000007e-05 * fTempPerm232) + ((0.000174119 * fTempPerm233) + ((0.000357221 * fTempPerm234) + ((0.000174273 * fTempPerm236) + ((0.000197515 * fTempPerm237) + ((0.0013863900000000001 * fTempPerm238) + ((0.0007656970000000001 * fTempPerm1315) + ((0.000224925 * fTempPerm240) + ((2.74129e-05 * fTempPerm1318) + ((0.0039510000000000005 * fTempPerm241) + ((1.5791000000000002e-05 * fTempPerm242) + ((2.14215e-05 * fTempPerm243) + ((1.90061e-05 * fTempPerm244) + ((0.000737136 * fTempPerm245) + ((0.000130289 * fTempPerm1325) + ((0.000704934 * fTempPerm246) + ((0.00169906 * fTempPerm247) + ((0.00045359999999999997 * fTempPerm248) + ((0.00120529 * fTempPerm1327) + ((7.11836e-05 * fTempPerm251) + ((0.00769069 * fTempPerm254) + ((0.00258112 * fTempPerm255) + ((0.00200101 * fTempPerm1329) + ((0.0056659300000000004 * fTempPerm256) + ((0.0029520400000000004 * fTempPerm257) + ((0.0020690300000000003 * fTempPerm258) + ((0.008862700000000001 * fTempPerm259) + ((0.00942078 * fTempPerm260) + ((0.00899273 * fTempPerm261) + ((0.0372107 * fTempPerm262) + ((0.000917066 * fTempPerm263) + ((0.0006762220000000001 * fTempPerm264) + ((6.2718e-07 * fTempPerm265) + ((8.974959999999999e-06 * fTempPerm266) + ((1.8158200000000002e-05 * fTempPerm267) + ((5.1987600000000003e-05 * fTempPerm268) + ((0.00018432800000000001 * fTempPerm269) + ((6.398530000000001e-05 * fTempPerm1336) + ((0.00171888 * fTempPerm272) + ((1.94535e-05 * fTempPerm1340) + ((0.00125626 * fTempPerm275) + ((0.0033445299999999996 * fTempPerm277) + ((0.00126195 * fTempPerm1343) + ((0.00358275 * fTempPerm1345) + ((0.008063269999999999 * fTempPerm279) + ((0.00297135 * fTempPerm1347) + ((0.00549752 * fTempPerm1350) + ((0.00453393 * fTempPerm281) + ((0.011907399999999999 * fTempPerm283) + ((0.00537065 * fTempPerm285) + ((0.0289144 * fTempPerm286) + ((0.00693778 * fTempPerm1355) + ((0.015224500000000002 * fTempPerm1357) + ((0.022025899999999998 * fTempPerm290) + ((0.00223481 * fTempPerm291) + ((0.0008461330000000001 * fTempPerm294) + ((0.000316584 * fTempPerm1362) + ((4.6817e-05 * fTempPerm296) + ((0.00030049700000000005 * fTempPerm297) + ((0.000306084 * fTempPerm1364) + ((4.2592100000000005e-06 * fTempPerm298) + ((5.07632e-05 * fTempPerm299) + ((2.3408799999999998e-06 * fTempPerm1365) + ((4.46169e-05 * fTempPerm302) + ((0.00040533400000000003 * fTempPerm303) + ((3.51464e-06 * fTempPerm1366) + ((0.0017419500000000001 * fTempPerm306) + ((7.354820000000001e-05 * fTempPerm1369) + ((0.000370958 * fTempPerm307) + ((0.000752537 * fTempPerm1371) + ((2.1531600000000002e-05 * fTempPerm308) + ((0.0026293 * fTempPerm309) + ((0.00471875 * fTempPerm312) + ((0.000310688 * fTempPerm316) + ((0.0028975800000000003 * fTempPerm1374) + ((0.000712925 * fTempPerm319) + ((0.000380057 * fTempPerm1375) + ((0.0009069470000000002 * fTempPerm320) + ((0.00167732 * fTempPerm1376) + ((0.00327984 * fTempPerm1377) + ((0.0024532300000000003 * fTempPerm1378) + ((0.00489879 * fTempPerm1379) + ((0.00165106 * fTempPerm1380) + ((0.00348053 * fTempPerm1381) + ((0.0023563100000000003 * fTempPerm1382) + ((0.0105223 * fTempPerm324) + ((0.00048340500000000007 * fTempPerm1383) + ((0.000101428 * fTempPerm1384) + ((9.45697e-05 * fTempPerm1385) + ((0.00023814000000000002 * fTempPerm1386) + ((6.29344e-05 * fTempPerm1389) + ((0.000158107 * fTempPerm1390) + ((5.723480000000001e-05 * fTempPerm326) + ((1.6838600000000002e-05 * fTempPerm1392) + ((0.00016574 * fTempPerm1393) + ((0.00362881 * fTempPerm331) + ((0.00789563 * fTempPerm332) + ((4.96364e-06 * fTempPerm1395) + ((0.00122724 * fTempPerm1397) + ((0.00020847000000000004 * fTempPerm1399) + ((0.000908412 * fTempPerm336) + ((0.0010732299999999999 * fTempPerm1400) + ((0.000172719 * fTempPerm337) + ((0.00218125 * fTempPerm1401) + ((0.00186575 * fTempPerm338) + ((0.00080034 * fTempPerm339) + ((0.00164622 * fTempPerm1402) + ((0.00392757 * fTempPerm1403) + ((0.006581140000000001 * fTempPerm340) + ((0.00349277 * fTempPerm1404) + ((0.0042838500000000005 * fTempPerm341) + ((0.00038527200000000003 * fTempPerm1405) + ((0.0158301 * fTempPerm1406) + ((0.00396484 * fTempPerm1407) + ((0.00145427 * fTempPerm1409) + ((0.00036303800000000004 * fTempPerm344) + ((0.000640087 * fTempPerm347) + ((6.501340000000001e-05 * fTempPerm1083) + ((0.00011503900000000001 * fTempPerm2047) + ((7.49829e-05 * fTempPerm1082) + ((1.1654e-05 * fTempPerm1411) + ((1.41509e-05 * fTempPerm352) + ((5.84711e-05 * fTempPerm1412) + ((0.0009658490000000001 * fTempPerm1413) + ((3.22237e-05 * fTempPerm1414) + ((0.00013887600000000002 * fTempPerm1415) + ((0.000316498 * fTempPerm1416) + ((0.00084653 * fTempPerm1417) + ((0.000370598 * fTempPerm357) + ((0.0013326899999999999 * fTempPerm358) + ((0.00140716 * fTempPerm1418) + ((0.0029169900000000004 * fTempPerm1419) + ((0.000364334 * fTempPerm1421) + ((0.000641773 * fTempPerm363) + ((0.00375105 * fTempPerm1422) + ((0.006903970000000001 * fTempPerm1423) + ((0.00586852 * fTempPerm1424) + ((0.00772571 * fTempPerm1425) + ((0.007573000000000001 * fTempPerm1426) + ((0.00604226 * fTempPerm1427) + ((0.00457394 * fTempPerm1429) + ((0.0126207 * fTempPerm370) + ((0.0155901 * fTempPerm1432) + ((0.0274978 * fTempPerm2045) + ((0.019302299999999998 * fTempPerm1434) + ((0.044481599999999996 * fTempPerm1435) + ((0.0422768 * fTempPerm1436) + ((0.000272913 * fTempPerm1437) + ((6.155e-05 * fTempPerm371) + ((0.000175224 * fTempPerm1439) + ((0.00022650199999999998 * fTempPerm1440) + ((5.7022700000000005e-05 * fTempPerm1441) + ((5.71049e-06 * fTempPerm376) + ((0.00534971 * fTempPerm1445) + ((0.00147622 * fTempPerm377) + ((0.0016841900000000001 * fTempPerm378) + ((0.009869669999999999 * fTempPerm1446) + ((0.00271591 * fTempPerm380) + ((0.0355257 * fTempPerm1447) + ((0.00342189 * fTempPerm1448) + ((0.0167824 * fTempPerm1449) + ((2.9919300000000003e-05 * fTempPerm384) + ((3.37965e-05 * fTempPerm386) + ((0.000544421 * fTempPerm1451) + ((0.00043977500000000005 * fTempPerm1452) + ((0.0005447809999999999 * fTempPerm1453) + ((0.00022437700000000002 * fTempPerm1454) + ((4.80575e-06 * fTempPerm1455) + ((7.71071e-07 * fTempPerm1456) + ((3.6990800000000005e-05 * fTempPerm1457) + ((0.000758498 * fTempPerm1458) + ((1.79398e-05 * fTempPerm399) + ((0.0016715800000000002 * fTempPerm1459) + ((0.00292629 * fTempPerm1461) + ((3.8916400000000006e-05 * fTempPerm1462) + ((0.0012073799999999999 * fTempPerm1463) + (((5.6678e-05 * fTempPerm1465) + ((0.00105839 * fTempPerm404) + ((0.000648676 * fTempPerm405) + ((0.010519 * fTempPerm406) + ((0.0133066 * fTempPerm1485) + ((0.024085600000000002 * fTempPerm409) + ((0.0629913 * fTempPerm410) + ((0.0008209550000000001 * fTempPerm411) + ((8.259009999999999e-06 * fTempPerm412) + ((0.008777450000000001 * fTempPerm413) + ((0.0316397 * fTempPerm414) + ((0.000845473 * fTempPerm416) + ((0.000711998 * fTempPerm417) + ((0.0013651400000000001 * fTempPerm1494) + ((0.00203118 * fTempPerm418) + ((0.00108711 * fTempPerm419) + ((0.00119671 * fTempPerm420) + ((0.00253939 * fTempPerm421) + ((9.18478e-05 * fTempPerm1497) + ((0.0007635890000000001 * fTempPerm1498) + ((0.0038119900000000003 * fTempPerm423) + ((0.00143391 * fTempPerm426) + ((0.0049894200000000005 * fTempPerm427) + ((0.00317079 * fTempPerm428) + ((0.00976483 * fTempPerm429) + ((0.0162506 * fTempPerm430) + ((4.2778300000000003e-05 * fTempPerm432) + ((0.000227988 * fTempPerm1508) + ((0.000705638 * fTempPerm1509) + ((0.000286928 * fTempPerm1510) + ((0.00152554 * fTempPerm1511) + ((0.00133293 * fTempPerm433) + ((0.000908465 * fTempPerm434) + ((0.000274135 * fTempPerm1512) + ((0.00265571 * fTempPerm437) + ((0.00044967800000000007 * fTempPerm1513) + ((0.00292244 * fTempPerm1514) + ((0.010395300000000001 * fTempPerm1515) + ((0.009209740000000001 * fTempPerm1516) + ((0.00432975 * fTempPerm1517) + ((0.0130982 * fTempPerm1518) + ((0.00631691 * fTempPerm1519) + ((0.009370129999999999 * fTempPerm1520) + ((0.022465099999999998 * fTempPerm1521) + ((0.00263838 * fTempPerm1522) + ((0.00808286 * fTempPerm1523) + ((0.000930466 * fTempPerm446) + ((0.015390200000000001 * fTempPerm449) + ((0.0459645 * fTempPerm1526) + ((0.0287878 * fTempPerm1528) + ((0.04831920000000001 * fTempPerm1529) + ((0.07684529999999999 * fTempPerm1530) + ((0.0744374 * fTempPerm1531) + ((9.486900000000002e-05 * fTempPerm450) + ((0.000976536 * fTempPerm1532) + ((0.00011001600000000001 * fTempPerm451) + ((0.000634386 * fTempPerm1534) + ((0.000125892 * fTempPerm453) + ((0.00040926 * fTempPerm1535) + ((5.28142e-05 * fTempPerm1536) + ((0.000136371 * fTempPerm455) + ((1.3387100000000001e-05 * fTempPerm1537) + ((8.331860000000002e-05 * fTempPerm1538) + ((0.00025527000000000004 * fTempPerm456) + ((0.00019838400000000002 * fTempPerm458) + ((0.00020233 * fTempPerm459) + ((0.0011732700000000001 * fTempPerm1539) + ((0.0016132899999999999 * fTempPerm460) + ((0.00257691 * fTempPerm1540) + ((0.00284821 * fTempPerm1542) + ((0.0015727 * fTempPerm1544) + ((0.02706 * fTempPerm1546) + ((0.00639689 * fTempPerm464) + ((0.01165 * fTempPerm1547) + ((0.00922201 * fTempPerm1548) + ((0.00786339 * fTempPerm1549) + ((0.0292933 * fTempPerm466) + ((0.009656040000000001 * fTempPerm1550) + ((0.013951100000000001 * fTempPerm1552) + ((0.0144406 * fTempPerm1553) + ((0.00843745 * fTempPerm469) + ((7.912519999999999e-06 * fTempPerm1555) + ((6.86851e-05 * fTempPerm1556) + ((0.00013697 * fTempPerm1557) + ((0.00190415 * fTempPerm1558) + ((0.0050915 * fTempPerm1559) + ((0.00440538 * fTempPerm1560) + ((0.0011063700000000002 * fTempPerm1561) + ((0.00197319 * fTempPerm474) + ((2.5843400000000004e-05 * fTempPerm1562) + ((0.00031903100000000006 * fTempPerm476) + ((0.000923679 * fTempPerm477) + ((0.000571456 * fTempPerm478) + ((0.00122034 * fTempPerm479) + ((5.63033e-05 * fTempPerm480) + ((0.000544179 * fTempPerm481) + ((0.00193302 * fTempPerm484) + ((0.00801492 * fTempPerm1565) + ((0.0022649 * fTempPerm1566) + ((0.0110397 * fTempPerm486) + ((0.0010152000000000002 * fTempPerm487) + ((0.00308618 * fTempPerm488) + ((0.00328942 * fTempPerm489) + ((0.00115249 * fTempPerm1568) + ((0.00156606 * fTempPerm1569) + ((3.52551e-06 * fTempPerm492) + ((3.2621699999999995e-06 * fTempPerm493) + ((0.000351923 * fTempPerm494) + ((4.85157e-05 * fTempPerm496) + ((0.000181368 * fTempPerm1572) + ((0.00030826 * fTempPerm499) + ((3.68663e-05 * fTempPerm500) + ((0.000762874 * fTempPerm501) + ((0.000162845 * fTempPerm502) + ((0.000175864 * fTempPerm1577) + ((0.00130377 * fTempPerm509) + ((0.000293776 * fTempPerm1578) + ((0.000280439 * fTempPerm1579) + ((0.000643591 * fTempPerm1581) + ((0.000528145 * fTempPerm1582) + ((0.001377 * fTempPerm512) + ((0.00016274 * fTempPerm513) + ((0.0022987899999999998 * fTempPerm1583) + ((0.00103864 * fTempPerm1584) + ((0.0016638 * fTempPerm1585) + ((0.00383124 * fTempPerm1586) + ((0.00139329 * fTempPerm515) + ((0.0106382 * fTempPerm1589) + ((0.004597380000000001 * fTempPerm1590) + ((6.42903e-05 * fTempPerm1591) + ((0.0015759 * fTempPerm521) + ((5.185120000000001e-05 * fTempPerm526) + ((7.89041e-07 * fTempPerm527) + ((0.000263489 * fTempPerm1594) + ((0.000828266 * fTempPerm1595) + ((0.000979588 * fTempPerm1596) + ((0.00110553 * fTempPerm1597) + ((0.0008129400000000001 * fTempPerm1598) + ((0.00167489 * fTempPerm1599) + ((0.0012814900000000001 * fTempPerm528) + ((0.00186384 * fTempPerm1600) + ((0.0015273399999999999 * fTempPerm1601) + ((0.00046550599999999997 * fTempPerm1602) + ((0.00120272 * fTempPerm1603) + ((0.000216738 * fTempPerm530) + ((0.0018260300000000002 * fTempPerm1604) + ((0.0016457800000000001 * fTempPerm1606) + ((8.31871e-05 * fTempPerm1610) + ((0.0010441100000000002 * fTempPerm534) + ((0.0009379120000000001 * fTempPerm535) + ((0.00067648 * fTempPerm536) + ((0.000255959 * fTempPerm537) + ((0.00269454 * fTempPerm538) + ((0.00291838 * fTempPerm539) + ((0.00204118 * fTempPerm540) + ((0.00018963900000000002 * fTempPerm542) + ((0.00172568 * fTempPerm1616) + ((0.0279132 * fTempPerm1623) + ((0.0257435 * fTempPerm543) + ((0.00685298 * fTempPerm1624) + ((0.06391530000000001 * fTempPerm545) + ((0.022666 * fTempPerm546) + ((0.0661857 * fTempPerm547) + ((0.00195357 * fTempPerm1625) + ((0.0555369 * fTempPerm548) + ((0.027306 * fTempPerm549) + ((0.000544776 * fTempPerm550) + ((0.0009202310000000001 * fTempPerm1631) + ((0.0009564980000000001 * fTempPerm552) + ((0.000315406 * fTempPerm554) + ((9.86646e-07 * fTempPerm1634) + ((6.19357e-07 * fTempPerm556) + ((0.00011808300000000001 * fTempPerm1635) + ((0.00100915 * fTempPerm1636) + ((0.0015510600000000002 * fTempPerm1637) + ((0.0031641300000000002 * fTempPerm1638) + ((0.00905307 * fTempPerm559) + ((0.00020890300000000003 * fTempPerm1639) + ((0.00626758 * fTempPerm1640) + ((0.004890869999999999 * fTempPerm1641) + ((0.00609081 * fTempPerm1642) + ((9.74875e-05 * fTempPerm562) + ((0.000168486 * fTempPerm1644) + ((0.000171772 * fTempPerm1645) + ((0.000359207 * fTempPerm1646) + ((0.00177178 * fTempPerm1647) + ((0.000588187 * fTempPerm568) + ((0.0056145 * fTempPerm1649) + ((5.39788e-06 * fTempPerm1653) + ((8.49295e-06 * fTempPerm1654) + ((5.10851e-05 * fTempPerm1655) + ((0.00113243 * fTempPerm574) + ((3.28963e-06 * fTempPerm575) + ((1.6980200000000004e-05 * fTempPerm1657) + ((0.0004906380000000001 * fTempPerm583) + ((0.00010184400000000001 * fTempPerm1660) + ((0.000145224 * fTempPerm585) + ((0.00460066 * fTempPerm1665) + ((0.0061875 * fTempPerm1666) + ((0.00861829 * fTempPerm1667) + ((0.0020271300000000003 * fTempPerm1668) + ((0.000665008 * fTempPerm1669) + ((1.32724e-05 * fTempPerm587) + ((0.00149796 * fTempPerm589) + ((2.9137400000000003e-05 * fTempPerm590) + ((0.000682499 * fTempPerm591) + ((0.00513181 * fTempPerm1670) + ((0.005401220000000001 * fTempPerm1671) + ((5.1858400000000005e-05 * fTempPerm593) + ((0.000785101 * fTempPerm594) + ((0.00239031 * fTempPerm595) + ((0.00258332 * fTempPerm596) + ((1.76065e-05 * fTempPerm597) + ((0.00118978 * fTempPerm600) + ((0.00241141 * fTempPerm1677) + ((0.00307881 * fTempPerm601) + ((0.00263921 * fTempPerm602) + ((0.00380796 * fTempPerm603) + ((4.87449e-07 * fTempPerm604) + ((0.0017979 * fTempPerm605) + ((0.00033669 * fTempPerm606) + ((0.000181872 * fTempPerm608) + ((0.000394495 * fTempPerm609) + ((0.0004264770000000001 * fTempPerm610) + ((0.0066563 * fTempPerm1689) + ((0.0173968 * fTempPerm613) + ((0.00657556 * fTempPerm1692) + ((0.044476699999999994 * fTempPerm615) + ((0.0194256 * fTempPerm616) + ((0.0258357 * fTempPerm618) + ((0.00505303 * fTempPerm619) + ((0.0009818860000000002 * fTempPerm1700) + ((0.00122741 * fTempPerm620) + ((0.0048163 * fTempPerm621) + ((0.0008063269999999999 * fTempPerm1703) + ((0.000739556 * fTempPerm1704) + ((0.0005989060000000001 * fTempPerm1705) + ((0.000409495 * fTempPerm1706) + ((0.00170804 * fTempPerm1707) + ((0.00110995 * fTempPerm1708) + ((0.00110707 * fTempPerm1709) + ((0.00050954 * fTempPerm1710) + ((0.0023355800000000003 * fTempPerm1711) + ((0.004373940000000001 * fTempPerm633) + ((0.0014793400000000002 * fTempPerm635) + ((0.0215706 * fTempPerm1712) + ((0.00439889 * fTempPerm1713) + ((0.007368019999999999 * fTempPerm638) + ((0.0005802190000000001 * fTempPerm1714) + ((0.00373067 * fTempPerm1715) + ((0.000216524 * fTempPerm1716) + ((0.000505773 * fTempPerm1718) + ((0.00019702400000000002 * fTempPerm1719) + ((0.000665779 * fTempPerm1720) + ((0.00103053 * fTempPerm1721) + ((0.0011346700000000002 * fTempPerm647) + ((0.00306353 * fTempPerm1722) + ((0.00238017 * fTempPerm1723) + ((0.0013700399999999999 * fTempPerm1724) + ((0.0012261 * fTempPerm655) + ((0.00263441 * fTempPerm656) + ((0.0433556 * fTempPerm1728) + ((0.0010622000000000001 * fTempPerm1730) + ((0.000249192 * fTempPerm1731) + ((0.000270496 * fTempPerm1732) + ((5.1123399999999996e-06 * fTempPerm663) + ((0.00095017 * fTempPerm1734) + ((0.00564598 * fTempPerm1736) + ((0.00021246900000000004 * fTempPerm1737) + ((0.000154768 * fTempPerm667) + ((0.0010572399999999999 * fTempPerm668) + ((0.000597807 * fTempPerm669) + ((0.00144013 * fTempPerm670) + ((0.000929206 * fTempPerm1750) + ((0.0801361 * fTempPerm671) + ((0.00759216 * fTempPerm2044) + ((0.00187271 * fTempPerm1752) + ((0.00858863 * fTempPerm1754) + ((0.00764567 * fTempPerm678) + ((0.00476206 * fTempPerm1755) + ((2.15894e-05 * fTempPerm679) + ((7.490179999999999e-07 * fTempPerm680) + ((0.00815685 * fTempPerm1756) + ((5.21215e-06 * fTempPerm681) + ((4.67294e-05 * fTempPerm682) + ((0.000651168 * fTempPerm683) + ((0.000598512 * fTempPerm1762) + ((7.450140000000001e-05 * fTempPerm684) + ((0.000855611 * fTempPerm1763) + ((0.000664306 * fTempPerm685) + ((0.00048425300000000003 * fTempPerm686) + ((0.00156026 * fTempPerm1765) + ((0.0272916 * fTempPerm690) + ((9.33864e-05 * fTempPerm1768) + ((0.000542254 * fTempPerm692) + ((0.00157817 * fTempPerm694) + ((0.00105727 * fTempPerm695) + ((0.0008087880000000001 * fTempPerm696) + ((0.00498675 * fTempPerm697) + ((3.23644e-06 * fTempPerm698) + ((7.578520000000001e-05 * fTempPerm699) + ((6.0100200000000004e-05 * fTempPerm700) + ((0.00346914 * fTempPerm1773) + ((7.06224e-05 * fTempPerm702) + ((1.2859600000000001e-05 * fTempPerm703) + ((6.81354e-05 * fTempPerm704) + ((0.000681458 * fTempPerm705) + ((0.0007558230000000001 * fTempPerm706) + ((0.00209591 * fTempPerm707) + ((0.00015610500000000002 * fTempPerm1778) + ((0.0028681 * fTempPerm1780) + ((0.0045752 * fTempPerm713) + ((0.00443077 * fTempPerm714) + ((0.00817591 * fTempPerm715) + ((0.0042540899999999994 * fTempPerm716) + ((0.000180181 * fTempPerm1783) + ((3.4608100000000005e-05 * fTempPerm1784) + ((0.00187875 * fTempPerm1785) + ((0.0008870710000000001 * fTempPerm1786) + ((0.00237411 * fTempPerm1787) + ((0.00112458 * fTempPerm1788) + ((0.014119 * fTempPerm1789) + ((0.00214593 * fTempPerm1791) + ((7.7027e-06 * fTempPerm1792) + ((1.9403399999999997e-06 * fTempPerm722) + ((0.00011763 * fTempPerm1793) + ((2.2899500000000003e-05 * fTempPerm1794) + ((0.000349096 * fTempPerm724) + ((0.000193746 * fTempPerm726) + ((0.000561862 * fTempPerm2036) + ((0.00024288 * fTempPerm730) + ((2.56907e-05 * fTempPerm735) + ((0.0008643610000000001 * fTempPerm736) + ((0.00557668 * fTempPerm737) + ((0.00288651 * fTempPerm1799) + ((0.00264027 * fTempPerm1800) + ((0.00010523700000000001 * fTempPerm1801) + ((7.977530000000001e-05 * fTempPerm1802) + ((0.00124543 * fTempPerm739) + ((0.000199826 * fTempPerm740) + ((0.001299 * fTempPerm741) + ((0.000178013 * fTempPerm742) + ((0.00335524 * fTempPerm1804) + ((0.0006723230000000001 * fTempPerm1805) + ((0.00118198 * fTempPerm1806) + ((0.00271496 * fTempPerm743) + ((0.004970270000000001 * fTempPerm1807) + ((0.000819709 * fTempPerm1808) + ((0.000603544 * fTempPerm1809) + ((0.00015924 * fTempPerm750) + ((0.00211776 * fTempPerm1812) + ((7.65374e-05 * fTempPerm753) + ((6.500930000000001e-05 * fTempPerm754) + ((0.000395822 * fTempPerm755) + ((0.000272771 * fTempPerm756) + ((0.0006149300000000001 * fTempPerm757) + ((0.00024267500000000003 * fTempPerm1813) + ((0.0027314400000000003 * fTempPerm758) + ((0.0031734000000000003 * fTempPerm759) + ((0.00121467 * fTempPerm760) + ((0.000320157 * fTempPerm761) + ((0.000220457 * fTempPerm1814) + ((0.00266146 * fTempPerm1815) + ((0.0049689600000000006 * fTempPerm1816) + ((0.0020659800000000002 * fTempPerm764) + ((0.00753143 * fTempPerm765) + ((0.00287375 * fTempPerm1817) + ((0.0161706 * fTempPerm768) + ((0.026332300000000003 * fTempPerm769) + ((0.0079993 * fTempPerm770) + ((0.0351254 * fTempPerm771) + ((0.000821635 * fTempPerm1820) + ((0.0032833199999999997 * fTempPerm772) + ((0.00033950300000000006 * fTempPerm1824) + ((0.00046365600000000006 * fTempPerm1825) + ((0.0019217200000000002 * fTempPerm775) + ((0.000557107 * fTempPerm776) + ((0.0007859140000000001 * fTempPerm777) + ((0.000674882 * fTempPerm778) + ((0.00020588500000000003 * fTempPerm779) + ((3.5918100000000004e-05 * fTempPerm780) + ((2.5941400000000002e-05 * fTempPerm781) + ((8.65485e-05 * fTempPerm782) + ((6.18528e-05 * fTempPerm783) + ((0.000991956 * fTempPerm784) + ((0.00428571 * fTempPerm787) + ((0.0005108500000000001 * fTempPerm1829) + ((0.000469171 * fTempPerm788) + ((0.000658512 * fTempPerm789) + ((0.00033249800000000003 * fTempPerm790) + ((6.401740000000001e-05 * fTempPerm1834) + ((0.00114682 * fTempPerm791) + ((0.000436107 * fTempPerm792) + ((0.000204336 * fTempPerm793) + ((0.0007023 * fTempPerm1841) + ((0.0011399600000000002 * fTempPerm1842) + ((0.00124602 * fTempPerm1843) + ((0.00129805 * fTempPerm1844) + ((0.00137873 * fTempPerm1845) + ((0.0028667500000000004 * fTempPerm1846) + ((0.0575248 * fTempPerm1849) + ((0.0060285 * fTempPerm1850) + ((0.0073186 * fTempPerm799) + ((0.0009477180000000001 * fTempPerm1851) + ((0.00136555 * fTempPerm1852) + ((0.0013616 * fTempPerm1853) + ((2.74177e-05 * fTempPerm1854) + ((0.0010250300000000001 * fTempPerm803) + ((0.0007250860000000001 * fTempPerm2043) + ((0.0013408299999999999 * fTempPerm2040) + ((0.00022987799999999998 * fTempPerm1079) + ((0.00040693400000000007 * fTempPerm806) + ((0.000737778 * fTempPerm807) + ((0.00069698 * fTempPerm808) + ((0.0017169000000000002 * fTempPerm809) + ((0.00161012 * fTempPerm811) + ((0.000615973 * fTempPerm812) + ((0.0017438199999999999 * fTempPerm814) + ((0.00247991 * fTempPerm815) + ((0.0011553 * fTempPerm1858) + ((0.00196204 * fTempPerm1866) + ((1.7742e-05 * fTempPerm1867) + (((3.3055699999999998e-06 * fVec2[IOTA&127]) + ((1.98933e-05 * fTempPerm1868) + ((7.38879e-06 * fTempPerm1869) + ((6.90525e-06 * fVec5[IOTA&127]) + ((0.00048385 * fTempPerm849) + ((0.000473651 * fTempPerm1874) + ((9.940880000000001e-05 * fTempPerm851) + ((0.00036222200000000004 * fTempPerm854) + ((0.0006331710000000001 * fTempPerm858) + ((0.00293953 * fTempPerm864) + ((0.0212956 * fTempPerm1878) + ((3.758e-05 * fTempPerm873) + ((1.0426200000000002e-05 * fVec13[IOTA&127]) + ((0.000542423 * fTempPerm875) + ((0.0011941500000000002 * fTempPerm876) + ((0.0013118799999999999 * fTempPerm877) + ((0.0009655010000000001 * fTempPerm878) + ((0.000583876 * fTempPerm879) + ((3.6292700000000006e-05 * fTempPerm880) + ((0.0013939500000000001 * fTempPerm881) + ((0.00191797 * fTempPerm882) + ((0.00106711 * fTempPerm883) + ((0.000347477 * fTempPerm884) + ((0.0017704200000000002 * fTempPerm1885) + ((6.33178e-05 * fTempPerm888) + ((5.5551700000000006e-05 * fTempPerm1887) + ((4.2405400000000006e-05 * fTempPerm889) + ((4.06072e-05 * fTempPerm891) + ((8.109680000000001e-05 * fTempPerm892) + ((0.0012369000000000002 * fTempPerm893) + ((0.0012472700000000002 * fTempPerm895) + ((0.0008454190000000001 * fTempPerm1890) + ((0.00026003500000000004 * fTempPerm1893) + ((0.004094459999999999 * fTempPerm898) + ((0.00019696800000000003 * fTempPerm1894) + ((0.00475943 * fTempPerm899) + ((0.004179850000000001 * fTempPerm1896) + ((0.00734871 * fTempPerm900) + ((0.0066234300000000005 * fTempPerm902) + ((0.00164357 * fTempPerm1897) + ((0.0107364 * fTempPerm903) + ((0.00216257 * fTempPerm904) + ((0.000784948 * fTempPerm905) + ((0.008757339999999999 * fTempPerm906) + ((0.008554550000000001 * fTempPerm907) + ((0.000829505 * fTempPerm1899) + ((0.00023107600000000004 * fTempPerm1902) + ((0.0006406130000000001 * fTempPerm1903) + ((0.00091664 * fTempPerm1904) + ((7.432920000000001e-05 * fTempPerm909) + ((0.0022028300000000002 * fTempPerm1906) + ((0.026865800000000002 * fTempPerm910) + ((0.00145302 * fTempPerm912) + ((0.00130731 * fTempPerm1907) + ((0.00277688 * fTempPerm1908) + ((0.0022861500000000002 * fTempPerm1909) + ((0.00273074 * fTempPerm1910) + ((0.00059075 * fTempPerm1911) + ((0.00293582 * fTempPerm1912) + ((0.00044511 * fTempPerm913) + ((0.0286455 * fTempPerm1913) + ((0.00924877 * fTempPerm916) + ((0.0391217 * fTempPerm1915) + ((0.00144425 * fTempPerm1916) + ((3.48346e-05 * fTempPerm921) + ((0.00048678400000000003 * fTempPerm1918) + ((0.000855899 * fTempPerm1919) + ((0.00181733 * fTempPerm1920) + ((0.00100318 * fTempPerm1921) + ((0.0019049100000000001 * fTempPerm1922) + ((0.000384737 * fTempPerm922) + ((0.00043984100000000006 * fTempPerm923) + ((0.0010960700000000002 * fTempPerm924) + ((0.00196376 * fTempPerm1924) + ((0.00275033 * fTempPerm1925) + ((0.0031654200000000004 * fTempPerm1926) + ((0.000680485 * fTempPerm926) + ((0.00122218 * fTempPerm927) + ((0.00152893 * fTempPerm928) + ((0.00133631 * fTempPerm1927) + ((0.000783797 * fTempPerm1928) + ((0.00220639 * fTempPerm1929) + ((0.00898901 * fTempPerm930) + ((0.00184861 * fTempPerm932) + ((0.0141169 * fTempPerm1930) + ((0.000976993 * fTempPerm933) + ((0.0111431 * fTempPerm1933) + ((0.00042180200000000004 * fTempPerm935) + ((0.00139564 * fTempPerm936) + ((0.000752906 * fTempPerm938) + ((0.00493341 * fTempPerm1935) + ((0.00011313 * fTempPerm1936) + ((0.00188932 * fTempPerm940) + ((0.00130908 * fTempPerm1938) + ((0.000649337 * fTempPerm941) + ((0.0023706 * fTempPerm951) + ((0.0008751970000000001 * fTempPerm1958) + ((0.000687957 * fTempPerm953) + ((0.0009207870000000001 * fTempPerm966) + ((6.44951e-05 * fTempPerm1959) + ((1.3287400000000001e-05 * fVec7[IOTA&127]) + ((2.03832e-05 * fTempPerm1961) + ((4.94134e-06 * fVec9[IOTA&127]) + ((3.4700500000000004e-05 * fTempPerm968) + ((1.2413700000000001e-05 * fVec10[IOTA&127]) + ((2.2571e-06 * fTempPerm969) + ((1.2883700000000002e-05 * fVec11[IOTA&127]) + ((0.000785113 * fTempPerm971) + ((0.00259829 * fTempPerm1962) + ((0.00786783 * fTempPerm1964) + ((0.0143689 * fTempPerm1965) + ((0.00249322 * fTempPerm1966) + ((0.0038962 * fTempPerm976) + ((0.0114674 * fTempPerm1969) + ((2.50192e-06 * fTempPerm1971) + ((2.3801800000000003e-05 * fTempPerm1972) + ((0.0009659900000000001 * fTempPerm979) + ((0.00065036 * fTempPerm980) + ((0.0012716300000000002 * fTempPerm981) + ((0.00205962 * fTempPerm982) + ((0.00167116 * fTempPerm983) + ((0.00041578400000000004 * fTempPerm985) + ((0.00256507 * fTempPerm986) + ((0.000984567 * fTempPerm1974) + ((0.0413938 * fTempPerm987) + ((0.042661300000000006 * fTempPerm988) + ((0.00881751 * fTempPerm989) + ((0.0531961 * fTempPerm990) + ((0.040092800000000005 * fTempPerm991) + ((0.0977876 * fTempPerm992) + ((0.064767 * fTempPerm993) + ((0.0829356 * fTempPerm994) + ((0.0784899 * fTempPerm995) + ((0.0006735780000000001 * fTempPerm996) + ((0.00072128 * fTempPerm997) + ((0.00149621 * fTempPerm1984) + ((0.0010723199999999999 * fTempPerm998) + ((0.000849503 * fTempPerm999) + ((0.000851532 * fTempPerm1000) + ((0.00101019 * fTempPerm1986) + ((0.000453602 * fTempPerm1003) + ((0.000429699 * fTempPerm1987) + ((0.000611186 * fTempPerm1008) + ((0.00101166 * fTempPerm1015) + ((0.00224289 * fTempPerm1992) + ((0.0009600120000000001 * fTempPerm1993) + ((1.2875300000000002e-05 * fTempPerm1994) + ((0.00558608 * fTempPerm1995) + ((4.932260000000001e-05 * fTempPerm1999) + ((0.0009760260000000001 * fTempPerm2000) + ((0.00953651 * fTempPerm2001) + ((0.00031580000000000003 * fTempPerm2002) + ((3.2479200000000006e-05 * fTempPerm1017) + ((0.00375098 * fTempPerm1018) + ((0.00015027 * fTempPerm1020) + ((2.4709800000000002e-05 * fVec14[IOTA&127]) + ((0.0008897180000000001 * fTempPerm2003) + ((0.00126468 * fTempPerm2005) + ((0.00148937 * fTempPerm2006) + ((0.000424545 * fTempPerm2007) + ((0.0005277820000000001 * fTempPerm2008) + ((0.0008282560000000001 * fTempPerm1026) + ((0.00039613 * fTempPerm2009) + ((0.0035877300000000003 * fTempPerm2010) + ((0.0066370199999999996 * fTempPerm1027) + ((0.000195522 * fTempPerm2011) + ((0.00184137 * fTempPerm2012) + ((0.004597660000000001 * fTempPerm2013) + ((0.0013813599999999999 * fTempPerm2014) + ((0.0007145500000000001 * fTempPerm1029) + ((0.0015830800000000002 * fTempPerm1030) + ((5.5010100000000004e-05 * fTempPerm2016) + ((0.0021273399999999997 * fTempPerm1033) + ((1.26402e-05 * fTempPerm2020) + ((0.00442281 * fTempPerm1036) + ((9.129639999999999e-07 * fTempPerm1037) + ((0.000121165 * fTempPerm1038) + ((0.000106835 * fTempPerm2022) + ((0.000152135 * fTempPerm2023) + ((0.00333511 * fTempPerm2024) + ((0.000583562 * fTempPerm1040) + ((0.00434699 * fTempPerm2026) + ((0.013183400000000001 * fTempPerm1043) + ((0.00040302800000000005 * fTempPerm1044) + ((0.019686699999999998 * fTempPerm2027) + ((0.008639049999999999 * fTempPerm2028) + ((0.00077856 * fTempPerm1048) + ((1.47262e-06 * fTempPerm1049) + ((0.000160754 * fTempPerm2030) + ((0.0772965 * fTempPerm1067) + (((0.00313446 * fTempPerm1068) + ((0.00040952500000000005 * fTempPerm1069) + (((7.42483e-05 * fTempPerm1073) + (0.000964846 * fTempPerm1072)) + (0.000664108 * fTempPerm1070)))) + (0.0011347400000000002 * fTempPerm2031)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (1.8821400000000002e-05 * fVec0[IOTA&127])))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (8.64243e-05 * fTempPerm1085))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))) + (0.000157369 * fTempPerm184))))) + (0.00022105899999999999 * fTempPerm179)))))))))))))) + (0.00202792 * fTempPerm1265)))))) + (0.000251922 * fTempPerm162)))) + (3.57125e-07 * fTempPerm160)) + (7.861370000000001e-05 * fTempPerm159))))))))) + (0.00034004900000000004 * fTempPerm150)) + (0.000168571 * fTempPerm1250)))))) + (0.000214133 * fTempPerm1245)) + (0.000267914 * fTempPerm1244))))))))))))))))))))))))))))))))))))))))))))))))) + (8.571500000000002e-05 * fTempPerm1192))) + (0.00019593 * fTempPerm1190))) + (0.00107904 * fTempPerm105))))))) + (9.23643e-06 * fTempPerm1182))))))))) + (0.00010676699999999999 * fTempPerm97)))))))) + (0.00137867 * fTempPerm1165)) + (0.000343904 * fTempPerm1164))))))))))))))))))))))))))))))))) + (0.0011166000000000001 * fTempPerm1131)))) + (0.00020061900000000003 * fTempPerm1126))) + (0.000738871 * fTempPerm1124)) + (0.000341992 * fTempPerm1123)) + (0.00017462800000000002 * fTempPerm1122)))))))))))))))))))))))))))))))))))) + (0.000173206 * fTempPerm1088)))));
			fRec19[0] = max((fRec19[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTempPerm2049))))));
			fbargraph17 = fRec19[0];
			output1[i] = (FAUSTFLOAT)fTempPerm2049;
			// post processing
			fRec19[1] = fRec19[0];
			fRec0[1] = fRec0[0];
			fRec18[1] = fRec18[0];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			fRec15[1] = fRec15[0];
			fRec14[1] = fRec14[0];
			fRec13[1] = fRec13[0];
			fRec12[1] = fRec12[0];
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
