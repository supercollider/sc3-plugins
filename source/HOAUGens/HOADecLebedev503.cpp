//----------------------------------------------------------
// name: "HOADecLebedev503"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2014"
//
// Code generated with Faust 0.9.100 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with  */
#include <math.h>
#ifndef FAUSTPOWER
#define FAUSTPOWER
#include <cmath>
template <int N> inline int faustpower(int x)              { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline int faustpower<0>(int x)            { return 1; }
template <> 	 inline int faustpower<1>(int x)            { return x; }
template <> 	 inline int faustpower<2>(int x)            { return x*x; }
template <int N> inline double faustpower(double x)          { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline double faustpower<0>(double x)        { return 1; }
template <> 	 inline double faustpower<1>(double x)        { return x; }
template <> 	 inline double faustpower<2>(double x)        { return x*x; }
#endif
// If other than 'faust2sc --prefix Faust' is used, sed this as well:
#if !defined(SC_FAUST_PREFIX)
# define SC_FAUST_PREFIX "Faust"
#endif

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
         * Trigger the UI* parameter with instance specific calls
         * to 'addBtton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the UI* user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Returns the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /** Global init, calls the following methods:
         * - static class 'classInit': static table initialisation
         * - 'instanceInit': constants and instance table initialisation
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


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
  private:
	double 	fConst0;
	double 	fConst1;
	double 	fConst2;
	FAUSTFLOAT 	fentry0;
	double 	fConst3;
	double 	fConst4;
	FAUSTFLOAT 	fcheckbox0;
	FAUSTFLOAT 	fslider0;
	double 	fRec4[2];
	double 	fRec6[2];
	double 	fRec5[2];
	double 	fConst5;
	double 	fRec3[2];
	double 	fConst6;
	double 	fRec7[2];
	double 	fRec2[2];
	double 	fRec1[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fRec12[2];
	double 	fRec11[2];
	double 	fRec10[2];
	double 	fRec13[2];
	double 	fRec9[2];
	double 	fRec8[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec18[2];
	double 	fRec17[2];
	double 	fRec16[2];
	double 	fRec19[2];
	double 	fRec15[2];
	double 	fRec14[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec20[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fConst7;
	double 	fRec23[2];
	double 	fRec22[2];
	double 	fRec21[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec26[2];
	double 	fRec25[2];
	double 	fRec24[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fConst8;
	double 	fRec30[2];
	double 	fRec29[2];
	double 	fConst9;
	double 	fRec28[2];
	double 	fRec27[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec34[2];
	double 	fRec33[2];
	double 	fRec32[2];
	double 	fRec31[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec38[2];
	double 	fRec37[2];
	double 	fRec36[2];
	double 	fRec35[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec43[2];
	double 	fRec42[2];
	double 	fRec41[2];
	double 	fRec44[2];
	double 	fRec40[2];
	double 	fRec39[2];
	FAUSTFLOAT 	fbargraph9;
	FAUSTFLOAT 	fslider1;
	double 	fRec45[2];
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec50[2];
	double 	fRec49[2];
	double 	fRec48[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec54[2];
	double 	fRec53[2];
	double 	fRec52[2];
	double 	fRec51[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec59[2];
	double 	fRec58[2];
	double 	fRec57[2];
	double 	fRec60[2];
	double 	fRec56[2];
	double 	fRec55[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec65[2];
	double 	fRec64[2];
	double 	fRec63[2];
	double 	fRec66[2];
	double 	fRec62[2];
	double 	fRec61[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec47[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec67[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec68[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec69[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec70[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec76[2];
	double 	fRec75[2];
	double 	fRec74[2];
	double 	fRec77[2];
	double 	fRec73[2];
	double 	fRec72[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec81[2];
	double 	fRec80[2];
	double 	fRec79[2];
	double 	fRec78[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec71[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec82[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec83[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec84[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec85[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec86[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec87[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec88[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec89[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec90[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec91[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec92[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec93[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec94[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec95[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec96[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec97[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec98[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec99[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec100[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec101[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec102[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec103[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec104[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec105[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec106[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec107[2];
	FAUSTFLOAT 	fbargraph49;
	double 	fRec108[2];
	FAUSTFLOAT 	fbargraph50;
	double 	fRec109[2];
	FAUSTFLOAT 	fbargraph51;
	double 	fRec110[2];
	FAUSTFLOAT 	fbargraph52;
	double 	fRec111[2];
	FAUSTFLOAT 	fbargraph53;
	double 	fRec112[2];
	FAUSTFLOAT 	fbargraph54;
	double 	fRec113[2];
	FAUSTFLOAT 	fbargraph55;
	double 	fRec114[2];
	FAUSTFLOAT 	fbargraph56;
	double 	fRec115[2];
	FAUSTFLOAT 	fbargraph57;
	double 	fRec116[2];
	FAUSTFLOAT 	fbargraph58;
	double 	fRec117[2];
	FAUSTFLOAT 	fbargraph59;
	double 	fRec118[2];
	FAUSTFLOAT 	fbargraph60;
	double 	fRec119[2];
	FAUSTFLOAT 	fbargraph61;
	double 	fRec120[2];
	FAUSTFLOAT 	fbargraph62;
	double 	fRec121[2];
	FAUSTFLOAT 	fbargraph63;
	double 	fRec122[2];
	FAUSTFLOAT 	fbargraph64;
	double 	fRec123[2];
	FAUSTFLOAT 	fbargraph65;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOADecLebedev503");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/ymn.lib/name", "Spherical Harmonics library");
		m->declare("lib/ymn.lib/version", "1.0");
		m->declare("lib/ymn.lib/author", "Pierre Lecomte");
		m->declare("lib/ymn.lib/license", "GPL");
		m->declare("lib/ymn.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/nfc.lib/name", "NF And NFC Filters Library");
		m->declare("lib/nfc.lib/version", "1.0");
		m->declare("lib/nfc.lib/author", "Pierre Lecomte");
		m->declare("lib/nfc.lib/license", "GPL");
		m->declare("lib/nfc.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/lebedev.lib/name", "Lebdev grids and weights");
		m->declare("lib/lebedev.lib/version", "1.0");
		m->declare("lib/lebedev.lib/author", "Pierre Lecomte");
		m->declare("lib/lebedev.lib/license", "GPL");
		m->declare("lib/lebedev.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
	}

	virtual int getNumInputs() { return 16; }
	virtual int getNumOutputs() { return 50; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(1.92e+05, max(1e+03, (double)fSamplingFreq));
		fConst1 = (8e+01 / fConst0);
		fConst2 = (394.7715102864353 / fConst0);
		fConst3 = (625.2284897135664 / fConst0);
		fConst4 = (1.7e+02 / fConst0);
		fConst5 = (1250.4569794271329 / fConst0);
		fConst6 = (789.5430205728705 / fConst0);
		fConst7 = (3.4e+02 / fConst0);
		fConst8 = (5.1e+02 / fConst0);
		fConst9 = (1.02e+03 / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fentry0 = 1.07;
		fcheckbox0 = 0.0;
		fslider0 = 0.0;
		fslider1 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<2; i++) fRec88[i] = 0;
		for (int i=0; i<2; i++) fRec89[i] = 0;
		for (int i=0; i<2; i++) fRec90[i] = 0;
		for (int i=0; i<2; i++) fRec91[i] = 0;
		for (int i=0; i<2; i++) fRec92[i] = 0;
		for (int i=0; i<2; i++) fRec93[i] = 0;
		for (int i=0; i<2; i++) fRec94[i] = 0;
		for (int i=0; i<2; i++) fRec95[i] = 0;
		for (int i=0; i<2; i++) fRec96[i] = 0;
		for (int i=0; i<2; i++) fRec97[i] = 0;
		for (int i=0; i<2; i++) fRec98[i] = 0;
		for (int i=0; i<2; i++) fRec99[i] = 0;
		for (int i=0; i<2; i++) fRec100[i] = 0;
		for (int i=0; i<2; i++) fRec101[i] = 0;
		for (int i=0; i<2; i++) fRec102[i] = 0;
		for (int i=0; i<2; i++) fRec103[i] = 0;
		for (int i=0; i<2; i++) fRec104[i] = 0;
		for (int i=0; i<2; i++) fRec105[i] = 0;
		for (int i=0; i<2; i++) fRec106[i] = 0;
		for (int i=0; i<2; i++) fRec107[i] = 0;
		for (int i=0; i<2; i++) fRec108[i] = 0;
		for (int i=0; i<2; i++) fRec109[i] = 0;
		for (int i=0; i<2; i++) fRec110[i] = 0;
		for (int i=0; i<2; i++) fRec111[i] = 0;
		for (int i=0; i<2; i++) fRec112[i] = 0;
		for (int i=0; i<2; i++) fRec113[i] = 0;
		for (int i=0; i<2; i++) fRec114[i] = 0;
		for (int i=0; i<2; i++) fRec115[i] = 0;
		for (int i=0; i<2; i++) fRec116[i] = 0;
		for (int i=0; i<2; i++) fRec117[i] = 0;
		for (int i=0; i<2; i++) fRec118[i] = 0;
		for (int i=0; i<2; i++) fRec119[i] = 0;
		for (int i=0; i<2; i++) fRec120[i] = 0;
		for (int i=0; i<2; i++) fRec121[i] = 0;
		for (int i=0; i<2; i++) fRec122[i] = 0;
		for (int i=0; i<2; i++) fRec123[i] = 0;
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
		ui_interface->openVerticalBox("HOADecLebedev503");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("B-Format");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc5761c20", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc577bcd0", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc5764a80", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc5767450", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc577ea70", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57990b0", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc576b200", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc576e270", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc5771350", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc5782740", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57960f0", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc5786150", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc5774fd0", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc5758a70", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc575c400", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc575fe60", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(&fslider0, "1", "");
		ui_interface->declare(&fslider0, "osc", "/levelin -10 60");
		ui_interface->declare(&fslider0, "unit", "dB");
		ui_interface->addVerticalSlider("Inputs Gain", &fslider0, 0.0, -1e+01, 6e+01, 0.1);
		ui_interface->declare(&fslider1, "2", "");
		ui_interface->declare(&fslider1, "osc", "/levelout -10 60");
		ui_interface->declare(&fslider1, "unit", "dB");
		ui_interface->addVerticalSlider("Outputs Gain", &fslider1, 0.0, -1e+01, 6e+01, 0.1);
		ui_interface->declare(0, "3", "");
		ui_interface->openVerticalBox("NFC");
		ui_interface->addCheckButton("Yes", &fcheckbox0);
		ui_interface->closeBox();
		ui_interface->declare(&fentry0, "4", "");
		ui_interface->addNumEntry("Speakers Radius", &fentry0, 1.07, 0.5, 1e+01, 0.01);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 1-25");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph10, "osc", "/output1");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc5777490", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph11, "osc", "/output2");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57793b0", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph16, "osc", "/output3");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc5787fc0", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph17, "osc", "/output4");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc578a490", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph18, "osc", "/output5");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc578cc00", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph19, "osc", "/output6");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc578ea40", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph20, "osc", "/output7");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc5791b10", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph23, "osc", "/output8");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc579b660", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph24, "osc", "/output9");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc579e750", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph25, "osc", "/output10");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57a18f0", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph26, "osc", "/output11");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57a42b0", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph27, "osc", "/output12");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57a6ce0", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph28, "osc", "/output13");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57a9370", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph29, "osc", "/output14");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57aba50", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph30, "osc", "/output15");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57adaf0", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph31, "osc", "/output16");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57b04d0", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph32, "osc", "/output17");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57b2bd0", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph33, "osc", "/output18");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57b5880", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph34, "osc", "/output19");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57b9e70", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph35, "osc", "/output20");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57bd8a0", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph36, "osc", "/output21");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57c0df0", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph37, "osc", "/output22");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57c42e0", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph38, "osc", "/output23");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57c7110", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph39, "osc", "/output24");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57c9dc0", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph40, "osc", "/output25");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57ccbb0", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph41, "osc", "/output26");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57cfa40", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph42, "osc", "/output27");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57d36c0", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph43, "osc", "/output28");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57d62c0", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph44, "osc", "/output29");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57d9ac0", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph45, "osc", "/output30");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57dca80", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph46, "osc", "/output31");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57e0eb0", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph47, "osc", "/output32");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57e4810", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph48, "osc", "/output33");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57e7fa0", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph49, "osc", "/output34");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57eb3d0", &fbargraph49, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph50, "osc", "/output35");
		ui_interface->declare(&fbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57eeb80", &fbargraph50, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fbargraph51, "osc", "/output36");
		ui_interface->declare(&fbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57f2470", &fbargraph51, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fbargraph52, "osc", "/output37");
		ui_interface->declare(&fbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57f5e60", &fbargraph52, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fbargraph53, "osc", "/output38");
		ui_interface->declare(&fbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc57f9d50", &fbargraph53, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fbargraph54, "osc", "/output39");
		ui_interface->declare(&fbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc60008b0", &fbargraph54, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fbargraph55, "osc", "/output40");
		ui_interface->declare(&fbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc60032c0", &fbargraph55, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fbargraph56, "osc", "/output41");
		ui_interface->declare(&fbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc6005e60", &fbargraph56, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fbargraph57, "osc", "/output42");
		ui_interface->declare(&fbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc6008960", &fbargraph57, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fbargraph58, "osc", "/output43");
		ui_interface->declare(&fbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc600b4f0", &fbargraph58, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fbargraph59, "osc", "/output44");
		ui_interface->declare(&fbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc600e130", &fbargraph59, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fbargraph60, "osc", "/output45");
		ui_interface->declare(&fbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc6010d90", &fbargraph60, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fbargraph61, "osc", "/output46");
		ui_interface->declare(&fbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc60138b0", &fbargraph61, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fbargraph62, "osc", "/output47");
		ui_interface->declare(&fbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc21024e0", &fbargraph62, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fbargraph63, "osc", "/output48");
		ui_interface->declare(&fbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc6017a30", &fbargraph63, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fbargraph64, "osc", "/output49");
		ui_interface->declare(&fbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc601a800", &fbargraph64, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fbargraph65, "osc", "/output50");
		ui_interface->declare(&fbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7f7fc601d6b0", &fbargraph65, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = double(fentry0);
		double 	fSlow1 = (1.0 / ((fConst2 / fSlow0) + 1));
		double 	fSlow2 = (fConst4 / fSlow0);
		double 	fSlow3 = faustpower<2>(fSlow2);
		double 	fSlow4 = (1.0 / (((fConst3 / fSlow0) + (6.45943269348338 * fSlow3)) + 1));
		double 	fSlow5 = double(fcheckbox0);
		double 	fSlow6 = (fSlow5 * fSlow0);
		double 	fSlow7 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider0))));
		double 	fSlow8 = (25.83773077393352 * fSlow3);
		double 	fSlow9 = (fSlow8 + (fConst5 / fSlow0));
		double 	fSlow10 = (fConst6 / fSlow0);
		double 	fSlow11 = (1 - fSlow5);
		double 	fSlow12 = (fSlow11 + fSlow6);
		double 	fSlow13 = (1.0 / (fSlow2 + 1));
		double 	fSlow14 = (fConst7 / fSlow0);
		double 	fSlow15 = (1.0 / (((fConst8 / fSlow0) + (3 * fSlow3)) + 1));
		double 	fSlow16 = (12 * fSlow3);
		double 	fSlow17 = (fSlow16 + (fConst9 / fSlow0));
		double 	fSlow18 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider1))));
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
		FAUSTFLOAT* output2 = output[2];
		FAUSTFLOAT* output3 = output[3];
		FAUSTFLOAT* output4 = output[4];
		FAUSTFLOAT* output5 = output[5];
		FAUSTFLOAT* output6 = output[6];
		FAUSTFLOAT* output7 = output[7];
		FAUSTFLOAT* output8 = output[8];
		FAUSTFLOAT* output9 = output[9];
		FAUSTFLOAT* output10 = output[10];
		FAUSTFLOAT* output11 = output[11];
		FAUSTFLOAT* output12 = output[12];
		FAUSTFLOAT* output13 = output[13];
		FAUSTFLOAT* output14 = output[14];
		FAUSTFLOAT* output15 = output[15];
		FAUSTFLOAT* output16 = output[16];
		FAUSTFLOAT* output17 = output[17];
		FAUSTFLOAT* output18 = output[18];
		FAUSTFLOAT* output19 = output[19];
		FAUSTFLOAT* output20 = output[20];
		FAUSTFLOAT* output21 = output[21];
		FAUSTFLOAT* output22 = output[22];
		FAUSTFLOAT* output23 = output[23];
		FAUSTFLOAT* output24 = output[24];
		FAUSTFLOAT* output25 = output[25];
		FAUSTFLOAT* output26 = output[26];
		FAUSTFLOAT* output27 = output[27];
		FAUSTFLOAT* output28 = output[28];
		FAUSTFLOAT* output29 = output[29];
		FAUSTFLOAT* output30 = output[30];
		FAUSTFLOAT* output31 = output[31];
		FAUSTFLOAT* output32 = output[32];
		FAUSTFLOAT* output33 = output[33];
		FAUSTFLOAT* output34 = output[34];
		FAUSTFLOAT* output35 = output[35];
		FAUSTFLOAT* output36 = output[36];
		FAUSTFLOAT* output37 = output[37];
		FAUSTFLOAT* output38 = output[38];
		FAUSTFLOAT* output39 = output[39];
		FAUSTFLOAT* output40 = output[40];
		FAUSTFLOAT* output41 = output[41];
		FAUSTFLOAT* output42 = output[42];
		FAUSTFLOAT* output43 = output[43];
		FAUSTFLOAT* output44 = output[44];
		FAUSTFLOAT* output45 = output[45];
		FAUSTFLOAT* output46 = output[46];
		FAUSTFLOAT* output47 = output[47];
		FAUSTFLOAT* output48 = output[48];
		FAUSTFLOAT* output49 = output[49];
		for (int i=0; i<count; i++) {
			fRec4[0] = (fSlow7 + (0.999 * fRec4[1]));
			double fTemp0 = (fRec4[0] * (double)input13[i]);
			fRec6[0] = (fRec6[1] + fRec3[1]);
			fRec5[0] = (fRec5[1] + fRec6[1]);
			fRec3[0] = (fSlow4 * ((fSlow6 * fTemp0) - ((fSlow8 * fRec5[0]) + (fSlow9 * fRec6[0]))));
			fRec7[0] = (fRec7[1] + fRec2[1]);
			fRec2[0] = (fSlow1 * (fRec3[0] - (fSlow10 * fRec7[0])));
			double fTemp1 = (fRec2[0] + (fSlow11 * fTemp0));
			fRec1[0] = max((fRec1[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp1))))));
			fbargraph0 = fRec1[0];
			double fTemp2 = fTemp1;
			double fTemp3 = (5.0391227634343926e-18 * fTemp2);
			double fTemp4 = (fRec4[0] * (double)input14[i]);
			fRec12[0] = (fRec12[1] + fRec10[1]);
			fRec11[0] = (fRec11[1] + fRec12[1]);
			fRec10[0] = (fSlow4 * ((fSlow6 * fTemp4) - ((fSlow8 * fRec11[0]) + (fSlow9 * fRec12[0]))));
			fRec13[0] = (fRec13[1] + fRec9[1]);
			fRec9[0] = (fSlow1 * (fRec10[0] - (fSlow10 * fRec13[0])));
			double fTemp5 = ((fSlow11 * fTemp4) + fRec9[0]);
			fRec8[0] = max((fRec8[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fbargraph1 = fRec8[0];
			double fTemp6 = fTemp5;
			double fTemp7 = (2.4393594688416384e-34 * fTemp6);
			double fTemp8 = (fRec4[0] * (double)input15[i]);
			fRec18[0] = (fRec18[1] + fRec16[1]);
			fRec17[0] = (fRec17[1] + fRec18[1]);
			fRec16[0] = (fSlow4 * ((fSlow6 * fTemp8) - ((fSlow8 * fRec17[0]) + (fSlow9 * fRec18[0]))));
			fRec19[0] = (fRec19[1] + fRec15[1]);
			fRec15[0] = (fSlow1 * (fRec16[0] - (fSlow10 * fRec19[0])));
			double fTemp9 = ((fSlow11 * fTemp8) + fRec15[0]);
			fRec14[0] = max((fRec14[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fbargraph2 = fRec14[0];
			double fTemp10 = fTemp9;
			double fTemp11 = (6.097910338853647e-51 * fTemp10);
			double fTemp12 = (fSlow12 * (fRec4[0] * (double)input0[i]));
			fRec20[0] = max((fRec20[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fbargraph3 = fRec20[0];
			double fTemp13 = fTemp12;
			double fTemp14 = (0.012698412698412698 * fTemp13);
			double fTemp15 = (fRec4[0] * (double)input2[i]);
			fRec23[0] = (fRec23[1] + fRec22[1]);
			fRec22[0] = (fSlow13 * ((fSlow6 * fTemp15) - (fSlow14 * fRec23[0])));
			double fTemp16 = (fRec22[0] + (fSlow11 * fTemp15));
			fRec21[0] = max((fRec21[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fbargraph4 = fRec21[0];
			double fTemp17 = fTemp16;
			double fTemp18 = (0.0219942959691286 * fTemp17);
			double fTemp19 = (fRec4[0] * (double)input3[i]);
			fRec26[0] = (fRec26[1] + fRec25[1]);
			fRec25[0] = (fSlow13 * ((fSlow6 * fTemp19) - (fSlow14 * fRec26[0])));
			double fTemp20 = (fRec25[0] + (fSlow11 * fTemp19));
			fRec24[0] = max((fRec24[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph5 = fRec24[0];
			double fTemp21 = fTemp20;
			double fTemp22 = (1.3467622079046435e-18 * fTemp21);
			double fTemp23 = (fRec4[0] * (double)input6[i]);
			fRec30[0] = (fRec30[1] + fRec28[1]);
			fRec29[0] = (fRec29[1] + fRec30[1]);
			fRec28[0] = (fSlow15 * ((fSlow6 * fTemp23) - ((fSlow16 * fRec29[0]) + (fSlow17 * fRec30[0]))));
			double fTemp24 = (fRec28[0] + (fSlow11 * fTemp23));
			fRec27[0] = max((fRec27[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fbargraph6 = fRec27[0];
			double fTemp25 = fTemp24;
			double fTemp26 = (0.02839451399999733 * fTemp25);
			double fTemp27 = (fRec4[0] * (double)input7[i]);
			fRec34[0] = (fRec34[1] + fRec32[1]);
			fRec33[0] = (fRec33[1] + fRec34[1]);
			fRec32[0] = (fSlow15 * ((fSlow6 * fTemp27) - ((fSlow16 * fRec33[0]) + (fSlow17 * fRec34[0]))));
			double fTemp28 = (fRec32[0] + (fSlow11 * fTemp27));
			fRec31[0] = max((fRec31[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph7 = fRec31[0];
			double fTemp29 = fTemp28;
			double fTemp30 = (3.0114518464024883e-18 * fTemp29);
			double fTemp31 = (fRec4[0] * (double)input8[i]);
			fRec38[0] = (fRec38[1] + fRec36[1]);
			fRec37[0] = (fRec37[1] + fRec38[1]);
			fRec36[0] = (fSlow15 * ((fSlow6 * fTemp31) - ((fSlow16 * fRec37[0]) + (fSlow17 * fRec38[0]))));
			double fTemp32 = (fRec36[0] + (fSlow11 * fTemp31));
			fRec35[0] = max((fRec35[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph8 = fRec35[0];
			double fTemp33 = fTemp32;
			double fTemp34 = (9.219912161207984e-35 * fTemp33);
			double fTemp35 = (fRec4[0] * (double)input12[i]);
			fRec43[0] = (fRec43[1] + fRec41[1]);
			fRec42[0] = (fRec42[1] + fRec43[1]);
			fRec41[0] = (fSlow4 * ((fSlow6 * fTemp35) - ((fSlow8 * fRec42[0]) + (fSlow9 * fRec43[0]))));
			fRec44[0] = (fRec44[1] + fRec40[1]);
			fRec40[0] = (fSlow1 * (fRec41[0] - (fSlow10 * fRec44[0])));
			double fTemp36 = (fRec40[0] + (fSlow11 * fTemp35));
			fRec39[0] = max((fRec39[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph9 = fRec39[0];
			double fTemp37 = fTemp36;
			double fTemp38 = (0.03359684204526464 * fTemp37);
			fRec45[0] = (fSlow18 + (0.999 * fRec45[1]));
			double fTemp39 = ((((((((((fTemp3 + fTemp7) + fTemp11) + fTemp14) + fTemp18) + fTemp22) + fTemp26) + fTemp30) + fTemp34) + fTemp38) * fRec45[0]);
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fbargraph10 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp39;
			double fTemp40 = (0.024590370452110585 * fTemp33);
			double fTemp41 = (0.0219942959691286 * fTemp21);
			double fTemp42 = (0.026560635762986527 * fTemp10);
			double fTemp43 = (0.014197256999998666 * fTemp25);
			double fTemp44 = (0.020573779994945588 * fTemp2);
			double fTemp45 = (((fTemp40 + (fTemp41 + (fTemp42 + fTemp14))) - (fTemp43 + fTemp44)) * fRec45[0]);
			fRec46[0] = max((fRec46[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fbargraph11 = fRec46[0];
			output1[i] = (FAUSTFLOAT)fTemp45;
			double fTemp46 = (fRec4[0] * (double)input1[i]);
			fRec50[0] = (fRec50[1] + fRec49[1]);
			fRec49[0] = (fSlow13 * ((fSlow6 * fTemp46) - (fSlow14 * fRec50[0])));
			double fTemp47 = ((fSlow11 * fTemp46) + fRec49[0]);
			fRec48[0] = max((fRec48[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fbargraph12 = fRec48[0];
			double fTemp48 = fTemp47;
			double fTemp49 = (0.0219942959691286 * fTemp48);
			double fTemp50 = (fRec4[0] * (double)input4[i]);
			fRec54[0] = (fRec54[1] + fRec52[1]);
			fRec53[0] = (fRec53[1] + fRec54[1]);
			fRec52[0] = (fSlow15 * ((fSlow6 * fTemp50) - ((fSlow16 * fRec53[0]) + (fSlow17 * fRec54[0]))));
			double fTemp51 = ((fSlow11 * fTemp50) + fRec52[0]);
			fRec51[0] = max((fRec51[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fbargraph13 = fRec51[0];
			double fTemp52 = fTemp51;
			double fTemp53 = (fRec4[0] * (double)input9[i]);
			fRec59[0] = (fRec59[1] + fRec57[1]);
			fRec58[0] = (fRec58[1] + fRec59[1]);
			fRec57[0] = (fSlow4 * ((fSlow6 * fTemp53) - ((fSlow8 * fRec58[0]) + (fSlow9 * fRec59[0]))));
			fRec60[0] = (fRec60[1] + fRec56[1]);
			fRec56[0] = (fSlow1 * (fRec57[0] - (fSlow10 * fRec60[0])));
			double fTemp54 = ((fSlow11 * fTemp53) + fRec56[0]);
			fRec55[0] = max((fRec55[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fbargraph14 = fRec55[0];
			double fTemp55 = fTemp54;
			double fTemp56 = (0.026560635762986527 * fTemp55);
			double fTemp57 = (fRec4[0] * (double)input11[i]);
			fRec65[0] = (fRec65[1] + fRec63[1]);
			fRec64[0] = (fRec64[1] + fRec65[1]);
			fRec63[0] = (fSlow4 * ((fSlow6 * fTemp57) - ((fSlow8 * fRec64[0]) + (fSlow9 * fRec65[0]))));
			fRec66[0] = (fRec66[1] + fRec62[1]);
			fRec62[0] = (fSlow1 * (fRec63[0] - (fSlow10 * fRec66[0])));
			double fTemp58 = ((fSlow11 * fTemp57) + fRec62[0]);
			fRec61[0] = max((fRec61[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fbargraph15 = fRec61[0];
			double fTemp59 = fTemp58;
			double fTemp60 = (0.020573779994945588 * fTemp59);
			double fTemp61 = (((((fTemp49 + (3.0114518464024883e-18 * fTemp52)) + fTemp14) + fTemp22) - (fTemp40 + (fTemp43 + (fTemp56 + (fTemp60 + ((1.2597806908585981e-18 * fTemp2) + (4.879109635569025e-18 * fTemp10))))))) * fRec45[0]);
			fRec47[0] = max((fRec47[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp61))))));
			fbargraph16 = fRec47[0];
			output2[i] = (FAUSTFLOAT)fTemp61;
			double fTemp62 = (((fTemp40 + (((9.75821927113805e-18 * fTemp55) + (fTemp44 + (2.693524415809287e-18 * fTemp48))) + fTemp14)) - (fTemp43 + (fTemp41 + ((6.0229036928049765e-18 * fTemp52) + (fTemp42 + (2.5195613817171963e-18 * fTemp59)))))) * fRec45[0]);
			fRec67[0] = max((fRec67[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fbargraph17 = fRec67[0];
			output3[i] = (FAUSTFLOAT)fTemp62;
			double fTemp63 = (fRec45[0] * ((fTemp14 + (fTemp56 + ((9.034355539207464e-18 * fTemp52) + (fTemp60 + ((3.779342072575794e-18 * fTemp2) + (1.4637328906707076e-17 * fTemp10)))))) - (fTemp40 + (fTemp43 + (fTemp49 + (4.040286623713931e-18 * fTemp21))))));
			fRec68[0] = max((fRec68[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fbargraph18 = fRec68[0];
			output4[i] = (FAUSTFLOAT)fTemp63;
			double fTemp64 = (fRec45[0] * ((fTemp34 + (fTemp26 + (fTemp22 + (fTemp14 + (fTemp3 + fTemp11))))) - (fTemp38 + (fTemp30 + (fTemp7 + fTemp18)))));
			fRec69[0] = max((fRec69[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fbargraph19 = fRec69[0];
			output5[i] = (FAUSTFLOAT)fTemp64;
			double fTemp65 = (0.02185810706854275 * fTemp33);
			double fTemp66 = (0.043716214137085485 * fTemp29);
			double fTemp67 = (0.012619783999998805 * fTemp25);
			double fTemp68 = (0.027648561470568506 * fTemp21);
			double fTemp69 = (0.0276485614705685 * fTemp17);
			double fTemp70 = (0.022574955908289243 * fTemp13);
			double fTemp71 = (0.03879429159750975 * fTemp2);
			double fTemp72 = (0.04089277388695433 * fTemp6);
			double fTemp73 = (0.01669440503167441 * fTemp10);
			double fTemp74 = ((fTemp71 + fTemp72) + fTemp73);
			double fTemp75 = (0.010558468816293319 * fTemp37);
			double fTemp76 = (fRec45[0] * ((fTemp65 + (fTemp66 + (fTemp67 + (fTemp68 + (fTemp69 + (fTemp70 + fTemp74)))))) - fTemp75));
			fRec70[0] = max((fRec70[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp76))))));
			fbargraph20 = fRec70[0];
			output6[i] = (FAUSTFLOAT)fTemp76;
			double fTemp77 = (2.3754652515039685e-18 * fTemp2);
			double fTemp78 = (0.03879429159750975 * fTemp59);
			double fTemp79 = (0.027648561470568506 * fTemp48);
			double fTemp80 = (2.6768460856911014e-18 * fTemp52);
			double fTemp81 = (fRec4[0] * (double)input10[i]);
			fRec76[0] = (fRec76[1] + fRec74[1]);
			fRec75[0] = (fRec75[1] + fRec76[1]);
			fRec74[0] = (fSlow4 * ((fSlow6 * fTemp81) - ((fSlow8 * fRec75[0]) + (fSlow9 * fRec76[0]))));
			fRec77[0] = (fRec77[1] + fRec73[1]);
			fRec73[0] = (fSlow1 * (fRec74[0] - (fSlow10 * fRec77[0])));
			double fTemp82 = ((fSlow11 * fTemp81) + fRec73[0]);
			fRec72[0] = max((fRec72[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp82))))));
			fbargraph21 = fRec72[0];
			double fTemp83 = fTemp82;
			double fTemp84 = (5.007920464891509e-18 * fTemp83);
			double fTemp85 = (fRec4[0] * (double)input5[i]);
			fRec81[0] = (fRec81[1] + fRec79[1]);
			fRec80[0] = (fRec80[1] + fRec81[1]);
			fRec79[0] = (fSlow15 * ((fSlow6 * fTemp85) - ((fSlow16 * fRec80[0]) + (fSlow17 * fRec81[0]))));
			double fTemp86 = ((fSlow11 * fTemp85) + fRec79[0]);
			fRec78[0] = max((fRec78[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp86))))));
			fbargraph22 = fRec78[0];
			double fTemp87 = fTemp86;
			double fTemp88 = (0.043716214137085485 * fTemp87);
			double fTemp89 = (1.6929861152980278e-18 * fTemp21);
			double fTemp90 = (2.6768460856911007e-18 * fTemp29);
			double fTemp91 = (3.0667124528564294e-18 * fTemp10);
			double fTemp92 = (0.01669440503167441 * fTemp55);
			double fTemp93 = ((((((((((((fTemp77 + fTemp78) + fTemp79) + fTemp80) + fTemp84) + fTemp88) + fTemp70) + fTemp69) + fTemp89) + fTemp67) + fTemp90) - ((((fTemp91 + fTemp72) + fTemp92) + fTemp65) + fTemp75)) * fRec45[0]);
			fRec71[0] = max((fRec71[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fbargraph23 = fRec71[0];
			output7[i] = (FAUSTFLOAT)fTemp93;
			double fTemp94 = (5.353692171382201e-18 * fTemp87);
			double fTemp95 = (6.133424905712859e-18 * fTemp55);
			double fTemp96 = (3.3859722305960555e-18 * fTemp48);
			double fTemp97 = (4.750930503007937e-18 * fTemp59);
			double fTemp98 = (1.0015840929783018e-17 * fTemp83);
			double fTemp99 = (5.353692171382203e-18 * fTemp52);
			double fTemp100 = (fTemp71 + fTemp73);
			double fTemp101 = (((((((fTemp94 + (fTemp95 + (fTemp96 + (fTemp97 + fTemp72)))) + fTemp70) + fTemp69) + fTemp67) + fTemp65) - ((((fTemp98 + (fTemp99 + fTemp100)) + fTemp68) + fTemp66) + fTemp75)) * fRec45[0]);
			fRec82[0] = max((fRec82[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp101))))));
			fbargraph24 = fRec82[0];
			output8[i] = (FAUSTFLOAT)fTemp101;
			double fTemp102 = (1.5023761394674526e-17 * fTemp83);
			double fTemp103 = (8.030538257073304e-18 * fTemp52);
			double fTemp104 = (9.20013735856929e-18 * fTemp10);
			double fTemp105 = (8.0305382570733e-18 * fTemp29);
			double fTemp106 = (5.078958345894083e-18 * fTemp21);
			double fTemp107 = (7.126395754511905e-18 * fTemp2);
			double fTemp108 = ((((((fTemp102 + ((fTemp103 + fTemp104) + fTemp92)) + fTemp70) + fTemp69) + fTemp67) - (((fTemp105 + (fTemp106 + ((((fTemp107 + fTemp72) + fTemp78) + fTemp79) + fTemp88))) + fTemp65) + fTemp75)) * fRec45[0]);
			fRec83[0] = max((fRec83[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp108))))));
			fbargraph25 = fRec83[0];
			output9[i] = (FAUSTFLOAT)fTemp108;
			double fTemp109 = (0.0276485614705685 * fTemp48);
			double fTemp110 = (0.043716214137085485 * fTemp52);
			double fTemp111 = (0.02523956799999763 * fTemp25);
			double fTemp112 = (0.025862861065006505 * fTemp59);
			double fTemp113 = (0.02586286106500651 * fTemp2);
			double fTemp114 = ((((((fTemp109 + fTemp110) + fTemp70) + fTemp68) + (2.6768460856911007e-18 * fTemp33)) - (fTemp111 + ((fTemp112 + fTemp113) + (0.0333888100633488 * (fTemp10 - fTemp55))))) * fRec45[0]);
			fRec84[0] = max((fRec84[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp114))))));
			fbargraph26 = fRec84[0];
			output10[i] = (FAUSTFLOAT)fTemp114;
			double fTemp115 = (0.025862861065006505 * fTemp2);
			double fTemp116 = (0.0276485614705685 * fTemp21);
			double fTemp117 = (0.02586286106500651 * fTemp59);
			double fTemp118 = (((((0.033388810063348796 * fTemp55) + ((fTemp115 + (0.03338881006334882 * fTemp10)) + fTemp79)) + fTemp70) - ((8.0305382570733e-18 * fTemp33) + (fTemp111 + (fTemp116 + (fTemp110 + fTemp117))))) * fRec45[0]);
			fRec85[0] = max((fRec85[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp118))))));
			fbargraph27 = fRec85[0];
			output11[i] = (FAUSTFLOAT)fTemp118;
			double fTemp119 = ((((1.3384230428455503e-17 * fTemp33) + ((fTemp110 + (fTemp112 + (fTemp113 + (0.03338881006334876 * fTemp10)))) + fTemp70)) - (fTemp111 + ((fTemp109 + (0.03338881006334885 * fTemp55)) + fTemp68))) * fRec45[0]);
			fRec86[0] = max((fRec86[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp119))))));
			fbargraph28 = fRec86[0];
			output12[i] = (FAUSTFLOAT)fTemp119;
			double fTemp120 = (((fTemp116 + (fTemp117 + fTemp70)) - ((1.8737922599837705e-17 * fTemp33) + (fTemp111 + ((0.03338881006334882 * fTemp55) + (fTemp110 + (((0.0333888100633488 * fTemp10) + fTemp115) + fTemp79)))))) * fRec45[0]);
			fRec87[0] = max((fRec87[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp120))))));
			fbargraph29 = fRec87[0];
			output13[i] = (FAUSTFLOAT)fTemp120;
			double fTemp121 = (((((((fTemp100 + fTemp70) + fTemp68) + fTemp67) + fTemp65) + fTemp75) - ((fTemp69 + fTemp72) + fTemp66)) * fRec45[0]);
			fRec88[0] = max((fRec88[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp121))))));
			fbargraph30 = fRec88[0];
			output14[i] = (FAUSTFLOAT)fTemp121;
			double fTemp122 = ((((((((((fTemp77 + fTemp72) + fTemp78) + fTemp79) + fTemp80) + fTemp70) + fTemp89) + fTemp67) + fTemp75) - ((((((fTemp91 + fTemp92) + fTemp84) + fTemp88) + fTemp69) + fTemp90) + fTemp65)) * fRec45[0]);
			fRec89[0] = max((fRec89[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp122))))));
			fbargraph31 = fRec89[0];
			output15[i] = (FAUSTFLOAT)fTemp122;
			double fTemp123 = ((((((((fTemp98 + (fTemp95 + (fTemp96 + fTemp97))) + fTemp70) + fTemp67) + fTemp66) + fTemp65) + fTemp75) - ((((fTemp99 + fTemp74) + fTemp94) + fTemp69) + fTemp68)) * fRec45[0]);
			fRec90[0] = max((fRec90[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp123))))));
			fbargraph32 = fRec90[0];
			output16[i] = (FAUSTFLOAT)fTemp123;
			double fTemp124 = ((((((((((fTemp104 + fTemp72) + fTemp103) + fTemp92) + fTemp88) + fTemp70) + fTemp67) + fTemp105) + fTemp75) - ((((fTemp102 + ((fTemp107 + fTemp78) + fTemp79)) + fTemp69) + fTemp106) + fTemp65)) * fRec45[0]);
			fRec91[0] = max((fRec91[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp124))))));
			fbargraph33 = fRec91[0];
			output17[i] = (FAUSTFLOAT)fTemp124;
			double fTemp125 = (1.6674738251076273e-18 * fTemp33);
			double fTemp126 = (0.02723191415302091 * fTemp29);
			double fTemp127 = (5.236595479505967e-18 * fTemp25);
			double fTemp128 = (0.021093749999999998 * fTemp21);
			double fTemp129 = (0.027231914153020904 * fTemp87);
			double fTemp130 = (0.041597435974919175 * fTemp83);
			double fTemp131 = (0.016982082124440745 * fTemp55);
			double fTemp132 = (0.027231914153020897 * fTemp52);
			double fTemp133 = (0.021093749999999994 * fTemp48);
			double fTemp134 = (0.013154264250377145 * fTemp59);
			double fTemp135 = (2.5471083409710867e-18 * fTemp6);
			double fTemp136 = (fTemp13 + fTemp17);
			double fTemp137 = (0.02109375 * fTemp136);
			double fTemp138 = (0.013154264250377147 * fTemp2);
			double fTemp139 = (0.021480823570105497 * fTemp37);
			double fTemp140 = (0.01698208212444074 * fTemp10);
			double fTemp141 = (((fTemp125 + (fTemp126 + (fTemp127 + (fTemp128 + (fTemp129 + (fTemp130 + (fTemp131 + (fTemp132 + (fTemp133 + (fTemp134 + (fTemp135 + (fTemp137 + fTemp138)))))))))))) - (fTemp139 + fTemp140)) * fRec45[0]);
			fRec92[0] = max((fRec92[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp141))))));
			fbargraph34 = fRec92[0];
			output18[i] = (FAUSTFLOAT)fTemp141;
			double fTemp142 = (0.02723191415302091 * fTemp87);
			double fTemp143 = (0.01698208212444074 * fTemp55);
			double fTemp144 = (0.021093749999999998 * fTemp48);
			double fTemp145 = (0.013154264250377147 * fTemp59);
			double fTemp146 = (0.01698208212444075 * fTemp10);
			double fTemp147 = (5.002421475322882e-18 * fTemp33);
			double fTemp148 = (0.027231914153020904 * fTemp29);
			double fTemp149 = (0.021093749999999994 * fTemp21);
			double fTemp150 = (0.013154264250377145 * fTemp2);
			double fTemp151 = (7.641325022913258e-18 * fTemp6);
			double fTemp152 = (((fTemp127 + (fTemp142 + (fTemp143 + (fTemp144 + (fTemp145 + (fTemp137 + fTemp146)))))) - (fTemp139 + (fTemp147 + (fTemp148 + (fTemp149 + (fTemp130 + (fTemp132 + (fTemp150 + fTemp151)))))))) * fRec45[0]);
			fRec93[0] = max((fRec93[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp152))))));
			fbargraph35 = fRec93[0];
			output19[i] = (FAUSTFLOAT)fTemp152;
			double fTemp153 = (8.337369125538138e-18 * fTemp33);
			double fTemp154 = (0.01698208212444072 * fTemp10);
			double fTemp155 = (1.273554170485543e-17 * fTemp6);
			double fTemp156 = (0.016982082124440766 * fTemp55);
			double fTemp157 = (0.013154264250377148 * fTemp2);
			double fTemp158 = (((fTemp153 + (fTemp127 + (fTemp130 + (fTemp132 + (fTemp154 + ((0.02109375 * (fTemp136 - fTemp21)) + fTemp155)))))) - (fTemp139 + (fTemp126 + (fTemp129 + (fTemp156 + (fTemp133 + (fTemp134 + fTemp157))))))) * fRec45[0]);
			fRec94[0] = max((fRec94[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp158))))));
			fbargraph36 = fRec94[0];
			output20[i] = (FAUSTFLOAT)fTemp158;
			double fTemp159 = (1.167231677575339e-17 * fTemp33);
			double fTemp160 = (0.01698208212444075 * fTemp55);
			double fTemp161 = (0.013154264250377148 * fTemp59);
			double fTemp162 = (fTemp48 - fTemp13);
			double fTemp163 = (1.7829758386797603e-17 * fTemp6);
			double fTemp164 = (((fTemp148 + (fTemp127 + (fTemp149 + fTemp150))) - (fTemp139 + (fTemp159 + (fTemp142 + (fTemp130 + (fTemp160 + (fTemp132 + (fTemp161 + (fTemp140 + ((0.02109375 * (fTemp162 - fTemp17)) + fTemp163)))))))))) * fRec45[0]);
			fRec95[0] = max((fRec95[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp164))))));
			fbargraph37 = fRec95[0];
			output21[i] = (FAUSTFLOAT)fTemp164;
			double fTemp165 = (fTemp13 - fTemp17);
			double fTemp166 = (0.02109375 * fTemp165);
			double fTemp167 = (((fTemp139 + (fTemp125 + (fTemp127 + (fTemp128 + (fTemp131 + (fTemp132 + (fTemp133 + (fTemp134 + (fTemp138 + fTemp166))))))))) - (fTemp126 + (fTemp129 + (fTemp130 + (fTemp135 + fTemp140))))) * fRec45[0]);
			fRec96[0] = max((fRec96[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp167))))));
			fbargraph38 = fRec96[0];
			output22[i] = (FAUSTFLOAT)fTemp167;
			double fTemp168 = (((fTemp139 + (fTemp148 + (fTemp127 + (fTemp130 + (fTemp143 + (fTemp144 + (fTemp145 + (fTemp146 + (fTemp151 + fTemp166))))))))) - (fTemp147 + (fTemp149 + (fTemp142 + (fTemp132 + fTemp150))))) * fRec45[0]);
			fRec97[0] = max((fRec97[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp168))))));
			fbargraph39 = fRec97[0];
			output23[i] = (FAUSTFLOAT)fTemp168;
			double fTemp169 = (((fTemp139 + (fTemp153 + (fTemp126 + (fTemp127 + (fTemp129 + (fTemp132 + (fTemp154 + (0.02109375 * (fTemp165 - fTemp21))))))))) - (fTemp130 + (fTemp156 + (fTemp133 + (fTemp134 + (fTemp155 + fTemp157)))))) * fRec45[0]);
			fRec98[0] = max((fRec98[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp169))))));
			fbargraph40 = fRec98[0];
			output24[i] = (FAUSTFLOAT)fTemp169;
			double fTemp170 = (fRec45[0] * ((fTemp139 + (fTemp127 + (fTemp149 + (fTemp142 + (fTemp130 + (fTemp150 + fTemp163)))))) - (fTemp159 + (fTemp148 + (fTemp160 + (fTemp132 + (fTemp161 + (fTemp140 + (0.02109375 * (fTemp162 + fTemp17))))))))));
			fRec99[0] = max((fRec99[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp170))))));
			fbargraph41 = fRec99[0];
			output25[i] = (FAUSTFLOAT)fTemp170;
			double fTemp171 = (0.0263335984839893 * fTemp37);
			double fTemp172 = (0.03280650879687195 * fTemp25);
			double fTemp173 = (0.03160554266923825 * fTemp17);
			double fTemp174 = (0.02017333553791887 * fTemp13);
			double fTemp175 = (0.021308452520676528 * (fTemp87 + fTemp29));
			double fTemp176 = (0.010535180889746065 * (fTemp48 + fTemp21));
			double fTemp177 = (0.016998264729033836 * fTemp83);
			double fTemp178 = (0.002313170838826743 * fTemp55);
			double fTemp179 = (0.007102817506892165 * fTemp52);
			double fTemp180 = (0.030460165261409405 * fTemp59);
			double fTemp181 = (0.030460165261409402 * fTemp2);
			double fTemp182 = (1.1422201709353029e-18 * fTemp33);
			double fTemp183 = (2.7335294515958808e-18 * fTemp6);
			double fTemp184 = (0.002313170838826745 * fTemp10);
			double fTemp185 = (fRec45[0] * ((fTemp171 + (fTemp172 + (fTemp173 + (fTemp174 + (fTemp175 + (fTemp176 + (fTemp177 + (fTemp178 + (fTemp179 + (fTemp180 + fTemp181)))))))))) - (fTemp182 + (fTemp183 + fTemp184))));
			fRec100[0] = max((fRec100[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp185))))));
			fbargraph42 = fRec100[0];
			output26[i] = (FAUSTFLOAT)fTemp185;
			double fTemp186 = (0.021308452520676528 * (fTemp87 - fTemp29));
			double fTemp187 = (0.010535180889746065 * (fTemp48 - fTemp21));
			double fTemp188 = (0.0023131708388267444 * fTemp10);
			double fTemp189 = (1.3047664087114909e-18 * fTemp33);
			double fTemp190 = (3.1225305737205953e-18 * fTemp6);
			double fTemp191 = (fRec45[0] * ((fTemp171 + (fTemp172 + (fTemp173 + (fTemp174 + (fTemp186 + (fTemp187 + (fTemp178 + (fTemp180 + fTemp188)))))))) - (fTemp189 + (fTemp177 + (fTemp179 + (fTemp181 + fTemp190))))));
			fRec101[0] = max((fRec101[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp191))))));
			fbargraph43 = fRec101[0];
			output27[i] = (FAUSTFLOAT)fTemp191;
			double fTemp192 = (2.1746106811858183e-18 * fTemp33);
			double fTemp193 = (5.204217622867659e-18 * fTemp6);
			double fTemp194 = (0.002313170838826741 * fTemp10);
			double fTemp195 = (0.02130845252067653 * fTemp29);
			double fTemp196 = (0.010535180889746067 * fTemp21);
			double fTemp197 = (0.021308452520676528 * fTemp87);
			double fTemp198 = (0.002313170838826747 * fTemp55);
			double fTemp199 = (0.010535180889746065 * fTemp48);
			double fTemp200 = (0.030460165261409412 * fTemp2);
			double fTemp201 = (0.030460165261409402 * fTemp59);
			double fTemp202 = (fRec45[0] * ((fTemp171 + (fTemp192 + (fTemp172 + (fTemp173 + (fTemp174 + (fTemp177 + (fTemp179 + (fTemp193 + fTemp194)))))))) - (fTemp195 + (fTemp196 + (fTemp197 + (fTemp198 + (fTemp199 + (fTemp200 + fTemp201))))))));
			fRec102[0] = max((fRec102[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp202))))));
			fbargraph44 = fRec102[0];
			output28[i] = (FAUSTFLOAT)fTemp202;
			double fTemp203 = (0.021308452520676528 * fTemp29);
			double fTemp204 = (0.010535180889746065 * fTemp21);
			double fTemp205 = (3.044454953660145e-18 * fTemp33);
			double fTemp206 = (0.02130845252067653 * fTemp87);
			double fTemp207 = (0.0023131708388267444 * fTemp55);
			double fTemp208 = (0.010535180889746067 * fTemp48);
			double fTemp209 = (0.030460165261409412 * fTemp59);
			double fTemp210 = (7.285904672014723e-18 * fTemp6);
			double fTemp211 = (0.0023131708388267436 * fTemp10);
			double fTemp212 = (fRec45[0] * ((fTemp171 + (fTemp203 + (fTemp172 + (fTemp204 + (fTemp173 + (fTemp174 + fTemp181)))))) - (fTemp205 + (fTemp206 + (fTemp177 + (fTemp207 + (fTemp179 + (fTemp208 + (fTemp209 + (fTemp210 + fTemp211))))))))));
			fRec103[0] = max((fRec103[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp212))))));
			fbargraph45 = fRec103[0];
			output29[i] = (FAUSTFLOAT)fTemp212;
			double fTemp213 = (0.028411270027568724 * fTemp33);
			double fTemp214 = (0.021308452520676542 * fTemp29);
			double fTemp215 = (0.03160554266923824 * fTemp21);
			double fTemp216 = (0.010535180889746075 * fTemp17);
			double fTemp217 = (0.007102817506892191 * fTemp87);
			double fTemp218 = (0.016998264729033888 * fTemp83);
			double fTemp219 = (0.03007122090474782 * fTemp55);
			double fTemp220 = (0.021308452520676584 * fTemp52);
			double fTemp221 = (0.010535180889746094 * fTemp48);
			double fTemp222 = (0.022664352972045144 * fTemp6);
			double fTemp223 = (0.02081853754944074 * fTemp10);
			double fTemp224 = (0.020481687709769415 * fTemp37);
			double fTemp225 = (0.01640325439843596 * fTemp25);
			double fTemp226 = (0.005375323281425205 * fTemp59);
			double fTemp227 = (0.01612596984427559 * fTemp2);
			double fTemp228 = (fRec45[0] * ((fTemp213 + (fTemp214 + (fTemp215 + (fTemp216 + (fTemp174 + (fTemp217 + (fTemp218 + (fTemp219 + (fTemp220 + (fTemp221 + (fTemp222 + fTemp223))))))))))) - (fTemp224 + (fTemp225 + (fTemp226 + fTemp227)))));
			fRec104[0] = max((fRec104[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp228))))));
			fbargraph46 = fRec104[0];
			output30[i] = (FAUSTFLOAT)fTemp228;
			double fTemp229 = (0.007102817506892183 * fTemp29);
			double fTemp230 = (0.01053518088974608 * fTemp21);
			double fTemp231 = (0.021308452520676546 * fTemp87);
			double fTemp232 = (0.03160554266923824 * fTemp48);
			double fTemp233 = (0.021308452520676556 * fTemp52);
			double fTemp234 = (0.016998264729033867 * fTemp83);
			double fTemp235 = (0.02841127002756874 * fTemp33);
			double fTemp236 = (0.020818537549440783 * fTemp55);
			double fTemp237 = (0.01612596984427559 * fTemp59);
			double fTemp238 = (0.030071220904747797 * fTemp10);
			double fTemp239 = (0.005375323281425198 * fTemp2);
			double fTemp240 = (0.022664352972045162 * fTemp6);
			double fTemp241 = (fRec45[0] * ((fTemp229 + (fTemp230 + (fTemp216 + (fTemp174 + (fTemp231 + ((fTemp232 + fTemp233) + fTemp234)))))) - (fTemp224 + (fTemp235 + (fTemp225 + (fTemp236 + (fTemp237 + (fTemp238 + (fTemp239 + fTemp240)))))))));
			fRec105[0] = max((fRec105[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp241))))));
			fbargraph47 = fRec105[0];
			output31[i] = (FAUSTFLOAT)fTemp241;
			double fTemp242 = (0.005375323281425197 * fTemp2);
			double fTemp243 = (0.03007122090474779 * fTemp10);
			double fTemp244 = (0.028411270027568745 * fTemp33);
			double fTemp245 = (0.007102817506892181 * fTemp29);
			double fTemp246 = (0.010535180889746079 * fTemp21);
			double fTemp247 = (0.016998264729033864 * fTemp83);
			double fTemp248 = (0.02081853754944079 * fTemp55);
			double fTemp249 = (0.016125969844275594 * fTemp59);
			double fTemp250 = (fRec45[0] * ((fTemp216 + (fTemp174 + (fTemp232 + (fTemp242 + fTemp243)))) - ((0.021308452520676553 * (fTemp52 - fTemp87)) + (fTemp224 + (fTemp244 + (fTemp245 + (fTemp225 + (fTemp246 + (fTemp247 + (fTemp248 + (fTemp240 + fTemp249)))))))))));
			fRec106[0] = max((fRec106[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp250))))));
			fbargraph48 = fRec106[0];
			output32[i] = (FAUSTFLOAT)fTemp250;
			double fTemp251 = (0.028411270027568717 * fTemp33);
			double fTemp252 = (0.007102817506892193 * fTemp87);
			double fTemp253 = (0.03007122090474784 * fTemp55);
			double fTemp254 = (0.010535180889746096 * fTemp48);
			double fTemp255 = (0.022664352972045138 * fTemp6);
			double fTemp256 = (0.005375323281425206 * fTemp59);
			double fTemp257 = (0.020818537549440724 * fTemp10);
			double fTemp258 = (fRec45[0] * ((fTemp251 + (fTemp216 + (fTemp174 + (fTemp252 + (fTemp253 + (fTemp254 + (fTemp227 + fTemp255))))))) - (fTemp224 + (fTemp214 + (fTemp225 + (fTemp215 + (fTemp218 + (fTemp220 + (fTemp256 + fTemp257)))))))));
			fRec107[0] = max((fRec107[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp258))))));
			fbargraph49 = fRec107[0];
			output33[i] = (FAUSTFLOAT)fTemp258;
			double fTemp259 = (0.02841127002756873 * fTemp33);
			double fTemp260 = (0.01699826472903388 * fTemp83);
			double fTemp261 = (0.02130845252067657 * fTemp52);
			double fTemp262 = (0.02266435297204515 * fTemp6);
			double fTemp263 = (0.005375323281425202 * fTemp59);
			double fTemp264 = (0.021308452520676546 * fTemp29);
			double fTemp265 = (0.007102817506892187 * fTemp87);
			double fTemp266 = (0.01053518088974609 * fTemp48);
			double fTemp267 = (0.020818537549440744 * fTemp10);
			double fTemp268 = (fRec45[0] * ((fTemp259 + (fTemp216 + (fTemp174 + (fTemp260 + (fTemp261 + ((fTemp227 + fTemp262) + fTemp263)))))) - (fTemp224 + (fTemp264 + (fTemp225 + (fTemp215 + (fTemp265 + (fTemp219 + (fTemp266 + fTemp267)))))))));
			fRec108[0] = max((fRec108[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp268))))));
			fbargraph50 = fRec108[0];
			output34[i] = (FAUSTFLOAT)fTemp268;
			double fTemp269 = (0.016998264729033874 * fTemp83);
			double fTemp270 = (0.020818537549440772 * fTemp55);
			double fTemp271 = (0.021308452520676563 * fTemp52);
			double fTemp272 = (0.0053753232814252 * fTemp2);
			double fTemp273 = (0.0300712209047478 * fTemp10);
			double fTemp274 = (0.028411270027568734 * fTemp33);
			double fTemp275 = (0.007102817506892185 * fTemp29);
			double fTemp276 = (0.010535180889746086 * fTemp21);
			double fTemp277 = (0.022664352972045155 * fTemp6);
			double fTemp278 = (fRec45[0] * ((fTemp216 + (fTemp174 + (fTemp269 + (fTemp270 + (fTemp271 + ((fTemp272 + fTemp273) + fTemp237)))))) - (fTemp224 + (fTemp274 + (fTemp275 + (fTemp225 + (fTemp276 + ((fTemp277 + fTemp232) + fTemp231))))))));
			fRec109[0] = max((fRec109[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp278))))));
			fbargraph51 = fRec109[0];
			output35[i] = (FAUSTFLOAT)fTemp278;
			double fTemp279 = (0.0071028175068921775 * fTemp29);
			double fTemp280 = (0.010535180889746074 * fTemp21);
			double fTemp281 = (0.020818537549440803 * fTemp55);
			double fTemp282 = (0.028411270027568748 * fTemp33);
			double fTemp283 = (0.021308452520676553 * fTemp87);
			double fTemp284 = (0.016998264729033857 * fTemp83);
			double fTemp285 = (0.021308452520676542 * fTemp52);
			double fTemp286 = (0.03007122090474778 * fTemp10);
			double fTemp287 = (0.005375323281425194 * fTemp2);
			double fTemp288 = (0.022664352972045165 * fTemp6);
			double fTemp289 = (fRec45[0] * ((fTemp279 + (fTemp280 + (fTemp216 + (fTemp174 + (fTemp281 + fTemp249))))) - (fTemp224 + (fTemp282 + (fTemp225 + (fTemp283 + (fTemp284 + (fTemp285 + (fTemp232 + (fTemp286 + (fTemp287 + fTemp288)))))))))));
			fRec110[0] = max((fRec110[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp289))))));
			fbargraph52 = fRec110[0];
			output36[i] = (FAUSTFLOAT)fTemp289;
			double fTemp290 = (0.028411270027568696 * fTemp33);
			double fTemp291 = (0.02130845252067654 * fTemp29);
			double fTemp292 = (0.03160554266923823 * fTemp21);
			double fTemp293 = (0.005375323281425214 * fTemp59);
			double fTemp294 = (0.02266435297204512 * fTemp6);
			double fTemp295 = (0.02081853754944071 * fTemp10);
			double fTemp296 = (0.007102817506892205 * fTemp87);
			double fTemp297 = (0.016998264729033916 * fTemp83);
			double fTemp298 = (0.030071220904747842 * fTemp55);
			double fTemp299 = (0.021308452520676615 * fTemp52);
			double fTemp300 = (0.010535180889746113 * fTemp48);
			double fTemp301 = (0.016125969844275587 * fTemp2);
			double fTemp302 = (fRec45[0] * ((fTemp290 + (fTemp291 + (fTemp292 + (fTemp216 + (fTemp174 + (fTemp293 + (fTemp294 + fTemp295))))))) - (fTemp224 + (fTemp225 + (fTemp296 + (fTemp297 + (fTemp298 + (fTemp299 + (fTemp300 + fTemp301)))))))));
			fRec111[0] = max((fRec111[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp302))))));
			fbargraph53 = fRec111[0];
			output37[i] = (FAUSTFLOAT)fTemp302;
			double fTemp303 = (fRec45[0] * ((fTemp224 + (fTemp213 + (fTemp215 + (fTemp174 + (fTemp219 + (fTemp220 + (fTemp221 + fTemp223))))))) - (fTemp214 + (fTemp225 + (fTemp216 + (fTemp217 + (fTemp218 + (fTemp226 + (fTemp227 + fTemp222)))))))));
			fRec112[0] = max((fRec112[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp303))))));
			fbargraph54 = fRec112[0];
			output38[i] = (FAUSTFLOAT)fTemp303;
			double fTemp304 = (fRec45[0] * ((fTemp224 + (fTemp230 + (fTemp174 + (fTemp233 + (fTemp232 + fTemp240))))) - (fTemp235 + (fTemp229 + (fTemp225 + (fTemp216 + (fTemp231 + (fTemp234 + (fTemp236 + (fTemp237 + (fTemp239 + fTemp238)))))))))));
			fRec113[0] = max((fRec113[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp304))))));
			fbargraph55 = fRec113[0];
			output39[i] = (FAUSTFLOAT)fTemp304;
			double fTemp305 = (fRec45[0] * ((fTemp224 + (fTemp245 + (fTemp174 + (fTemp247 + (fTemp232 + (fTemp243 + (fTemp240 + fTemp242))))))) - ((0.021308452520676553 * (fTemp52 + fTemp87)) + (fTemp244 + (fTemp225 + (fTemp246 + (fTemp216 + (fTemp248 + fTemp249))))))));
			fRec114[0] = max((fRec114[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp305))))));
			fbargraph56 = fRec114[0];
			output40[i] = (FAUSTFLOAT)fTemp305;
			double fTemp306 = (fRec45[0] * ((fTemp224 + (fTemp251 + (fTemp214 + (fTemp174 + (fTemp218 + (fTemp253 + (fTemp227 + fTemp254))))))) - (fTemp225 + (fTemp215 + (fTemp216 + (fTemp252 + (fTemp220 + (fTemp256 + (fTemp255 + fTemp257)))))))));
			fRec115[0] = max((fRec115[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp306))))));
			fbargraph57 = fRec115[0];
			output41[i] = (FAUSTFLOAT)fTemp306;
			double fTemp307 = (fRec45[0] * ((fTemp224 + (fTemp259 + (fTemp264 + (fTemp174 + (fTemp265 + (fTemp261 + (fTemp227 + fTemp263))))))) - (fTemp225 + (fTemp215 + (fTemp216 + (fTemp260 + (fTemp219 + (fTemp266 + (fTemp262 + fTemp267)))))))));
			fRec116[0] = max((fRec116[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp307))))));
			fbargraph58 = fRec116[0];
			output42[i] = (FAUSTFLOAT)fTemp307;
			double fTemp308 = (fRec45[0] * ((fTemp224 + (fTemp275 + (fTemp174 + (fTemp231 + (fTemp270 + (fTemp271 + (fTemp237 + (fTemp273 + (fTemp272 + fTemp277))))))))) - (fTemp274 + (fTemp225 + (fTemp276 + (fTemp216 + (fTemp269 + fTemp232)))))));
			fRec117[0] = max((fRec117[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp308))))));
			fbargraph59 = fRec117[0];
			output43[i] = (FAUSTFLOAT)fTemp308;
			double fTemp309 = (fRec45[0] * ((fTemp224 + (fTemp280 + (fTemp174 + (fTemp283 + (fTemp284 + (fTemp281 + (fTemp249 + fTemp288))))))) - (fTemp282 + (fTemp279 + (fTemp225 + (fTemp216 + (fTemp285 + (fTemp232 + (fTemp287 + fTemp286)))))))));
			fRec118[0] = max((fRec118[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp309))))));
			fbargraph60 = fRec118[0];
			output44[i] = (FAUSTFLOAT)fTemp309;
			double fTemp310 = (fRec45[0] * ((fTemp224 + (((fTemp174 + (((fTemp293 + fTemp295) + fTemp297) + fTemp296)) + fTemp292) + fTemp290)) - ((fTemp225 + (fTemp216 + ((((fTemp294 + fTemp301) + fTemp300) + fTemp299) + fTemp298))) + fTemp291)));
			fRec119[0] = max((fRec119[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp310))))));
			fbargraph61 = fRec119[0];
			output45[i] = (FAUSTFLOAT)fTemp310;
			double fTemp311 = (fRec45[0] * ((fTemp172 + (fTemp174 + (fTemp176 + (fTemp178 + (fTemp179 + (fTemp180 + (fTemp181 + fTemp183))))))) - (fTemp171 + (fTemp182 + (fTemp173 + (fTemp175 + (fTemp177 + fTemp184)))))));
			fRec120[0] = max((fRec120[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp311))))));
			fbargraph62 = fRec120[0];
			output46[i] = (FAUSTFLOAT)fTemp311;
			double fTemp312 = (fRec45[0] * ((fTemp172 + (fTemp174 + (fTemp187 + (fTemp177 + (fTemp178 + (fTemp180 + (fTemp190 + fTemp188))))))) - (fTemp171 + (fTemp189 + (fTemp173 + (fTemp186 + (fTemp179 + fTemp181)))))));
			fRec121[0] = max((fRec121[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp312))))));
			fbargraph63 = fRec121[0];
			output47[i] = (FAUSTFLOAT)fTemp312;
			double fTemp313 = (fRec45[0] * ((fTemp192 + (fTemp195 + (fTemp172 + (fTemp174 + (fTemp197 + (fTemp179 + fTemp194)))))) - (fTemp171 + (fTemp196 + (fTemp173 + (fTemp177 + (fTemp198 + (fTemp199 + (fTemp201 + (fTemp200 + fTemp193))))))))));
			fRec122[0] = max((fRec122[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp313))))));
			fbargraph64 = fRec122[0];
			output48[i] = (FAUSTFLOAT)fTemp313;
			double fTemp314 = (fRec45[0] * ((fTemp172 + (fTemp204 + (fTemp174 + (fTemp206 + (fTemp177 + (fTemp181 + fTemp210)))))) - (fTemp171 + (fTemp205 + (fTemp203 + (fTemp173 + (fTemp207 + (fTemp179 + (fTemp208 + (fTemp209 + fTemp211))))))))));
			fRec123[0] = max((fRec123[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp314))))));
			fbargraph65 = fRec123[0];
			output49[i] = (FAUSTFLOAT)fTemp314;
			// post processing
			fRec123[1] = fRec123[0];
			fRec122[1] = fRec122[0];
			fRec121[1] = fRec121[0];
			fRec120[1] = fRec120[0];
			fRec119[1] = fRec119[0];
			fRec118[1] = fRec118[0];
			fRec117[1] = fRec117[0];
			fRec116[1] = fRec116[0];
			fRec115[1] = fRec115[0];
			fRec114[1] = fRec114[0];
			fRec113[1] = fRec113[0];
			fRec112[1] = fRec112[0];
			fRec111[1] = fRec111[0];
			fRec110[1] = fRec110[0];
			fRec109[1] = fRec109[0];
			fRec108[1] = fRec108[0];
			fRec107[1] = fRec107[0];
			fRec106[1] = fRec106[0];
			fRec105[1] = fRec105[0];
			fRec104[1] = fRec104[0];
			fRec103[1] = fRec103[0];
			fRec102[1] = fRec102[0];
			fRec101[1] = fRec101[0];
			fRec100[1] = fRec100[0];
			fRec99[1] = fRec99[0];
			fRec98[1] = fRec98[0];
			fRec97[1] = fRec97[0];
			fRec96[1] = fRec96[0];
			fRec95[1] = fRec95[0];
			fRec94[1] = fRec94[0];
			fRec93[1] = fRec93[0];
			fRec92[1] = fRec92[0];
			fRec91[1] = fRec91[0];
			fRec90[1] = fRec90[0];
			fRec89[1] = fRec89[0];
			fRec88[1] = fRec88[0];
			fRec87[1] = fRec87[0];
			fRec86[1] = fRec86[0];
			fRec85[1] = fRec85[0];
			fRec84[1] = fRec84[0];
			fRec83[1] = fRec83[0];
			fRec82[1] = fRec82[0];
			fRec71[1] = fRec71[0];
			fRec78[1] = fRec78[0];
			fRec79[1] = fRec79[0];
			fRec80[1] = fRec80[0];
			fRec81[1] = fRec81[0];
			fRec72[1] = fRec72[0];
			fRec73[1] = fRec73[0];
			fRec77[1] = fRec77[0];
			fRec74[1] = fRec74[0];
			fRec75[1] = fRec75[0];
			fRec76[1] = fRec76[0];
			fRec70[1] = fRec70[0];
			fRec69[1] = fRec69[0];
			fRec68[1] = fRec68[0];
			fRec67[1] = fRec67[0];
			fRec47[1] = fRec47[0];
			fRec61[1] = fRec61[0];
			fRec62[1] = fRec62[0];
			fRec66[1] = fRec66[0];
			fRec63[1] = fRec63[0];
			fRec64[1] = fRec64[0];
			fRec65[1] = fRec65[0];
			fRec55[1] = fRec55[0];
			fRec56[1] = fRec56[0];
			fRec60[1] = fRec60[0];
			fRec57[1] = fRec57[0];
			fRec58[1] = fRec58[0];
			fRec59[1] = fRec59[0];
			fRec51[1] = fRec51[0];
			fRec52[1] = fRec52[0];
			fRec53[1] = fRec53[0];
			fRec54[1] = fRec54[0];
			fRec48[1] = fRec48[0];
			fRec49[1] = fRec49[0];
			fRec50[1] = fRec50[0];
			fRec46[1] = fRec46[0];
			fRec0[1] = fRec0[0];
			fRec45[1] = fRec45[0];
			fRec39[1] = fRec39[0];
			fRec40[1] = fRec40[0];
			fRec44[1] = fRec44[0];
			fRec41[1] = fRec41[0];
			fRec42[1] = fRec42[0];
			fRec43[1] = fRec43[0];
			fRec35[1] = fRec35[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec38[1] = fRec38[0];
			fRec31[1] = fRec31[0];
			fRec32[1] = fRec32[0];
			fRec33[1] = fRec33[0];
			fRec34[1] = fRec34[0];
			fRec27[1] = fRec27[0];
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec30[1] = fRec30[0];
			fRec24[1] = fRec24[0];
			fRec25[1] = fRec25[0];
			fRec26[1] = fRec26[0];
			fRec21[1] = fRec21[0];
			fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			fRec20[1] = fRec20[0];
			fRec14[1] = fRec14[0];
			fRec15[1] = fRec15[0];
			fRec19[1] = fRec19[0];
			fRec16[1] = fRec16[0];
			fRec17[1] = fRec17[0];
			fRec18[1] = fRec18[0];
			fRec8[1] = fRec8[0];
			fRec9[1] = fRec9[0];
			fRec13[1] = fRec13[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			fRec12[1] = fRec12[0];
			fRec1[1] = fRec1[0];
			fRec2[1] = fRec2[0];
			fRec7[1] = fRec7[0];
			fRec3[1] = fRec3[0];
			fRec5[1] = fRec5[0];
			fRec6[1] = fRec6[0];
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
