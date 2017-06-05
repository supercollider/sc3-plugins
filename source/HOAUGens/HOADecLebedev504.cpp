//----------------------------------------------------------
// name: "HOADecLebedev504"
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
	FAUSTFLOAT 	fslider0;
	double 	fRec1[2];
	FAUSTFLOAT 	fcheckbox0;
	FAUSTFLOAT 	fentry0;
	FAUSTFLOAT 	fslider1;
	double 	fRec3[2];
	double 	fRec2[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fConst2;
	double 	fConst3;
	double 	fRec6[2];
	double 	fRec5[2];
	double 	fRec4[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec9[2];
	double 	fRec8[2];
	double 	fRec7[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fConst4;
	double 	fRec13[2];
	double 	fRec12[2];
	double 	fConst5;
	double 	fRec11[2];
	double 	fRec10[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fRec17[2];
	double 	fRec16[2];
	double 	fRec15[2];
	double 	fRec14[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec21[2];
	double 	fRec20[2];
	double 	fRec19[2];
	double 	fRec18[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fConst6;
	double 	fConst7;
	double 	fRec26[2];
	double 	fRec25[2];
	double 	fConst8;
	double 	fRec24[2];
	double 	fConst9;
	double 	fRec27[2];
	double 	fRec23[2];
	double 	fRec22[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec32[2];
	double 	fRec31[2];
	double 	fRec30[2];
	double 	fRec33[2];
	double 	fRec29[2];
	double 	fRec28[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec38[2];
	double 	fRec37[2];
	double 	fRec36[2];
	double 	fRec39[2];
	double 	fRec35[2];
	double 	fRec34[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec44[2];
	double 	fRec43[2];
	double 	fRec42[2];
	double 	fRec45[2];
	double 	fRec41[2];
	double 	fRec40[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fConst10;
	double 	fConst11;
	double 	fRec50[2];
	double 	fRec49[2];
	double 	fConst12;
	double 	fRec48[2];
	double 	fRec52[2];
	double 	fRec51[2];
	double 	fConst13;
	double 	fRec47[2];
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec57[2];
	double 	fRec56[2];
	double 	fRec55[2];
	double 	fRec59[2];
	double 	fRec58[2];
	double 	fRec54[2];
	double 	fRec53[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec64[2];
	double 	fRec63[2];
	double 	fRec62[2];
	double 	fRec66[2];
	double 	fRec65[2];
	double 	fRec61[2];
	double 	fRec60[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec71[2];
	double 	fRec70[2];
	double 	fRec69[2];
	double 	fRec73[2];
	double 	fRec72[2];
	double 	fRec68[2];
	double 	fRec67[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec78[2];
	double 	fRec77[2];
	double 	fRec76[2];
	double 	fRec80[2];
	double 	fRec79[2];
	double 	fRec75[2];
	double 	fRec74[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec81[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec86[2];
	double 	fRec85[2];
	double 	fRec84[2];
	double 	fRec83[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec89[2];
	double 	fRec88[2];
	double 	fRec87[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec94[2];
	double 	fRec93[2];
	double 	fRec92[2];
	double 	fRec96[2];
	double 	fRec95[2];
	double 	fRec91[2];
	double 	fRec90[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec101[2];
	double 	fRec100[2];
	double 	fRec99[2];
	double 	fRec102[2];
	double 	fRec98[2];
	double 	fRec97[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec107[2];
	double 	fRec106[2];
	double 	fRec105[2];
	double 	fRec108[2];
	double 	fRec104[2];
	double 	fRec103[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec113[2];
	double 	fRec112[2];
	double 	fRec111[2];
	double 	fRec115[2];
	double 	fRec114[2];
	double 	fRec110[2];
	double 	fRec109[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec82[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec116[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec117[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec118[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec119[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec124[2];
	double 	fRec123[2];
	double 	fRec122[2];
	double 	fRec121[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec129[2];
	double 	fRec128[2];
	double 	fRec127[2];
	double 	fRec130[2];
	double 	fRec126[2];
	double 	fRec125[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec135[2];
	double 	fRec134[2];
	double 	fRec133[2];
	double 	fRec137[2];
	double 	fRec136[2];
	double 	fRec132[2];
	double 	fRec131[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec142[2];
	double 	fRec141[2];
	double 	fRec140[2];
	double 	fRec144[2];
	double 	fRec143[2];
	double 	fRec139[2];
	double 	fRec138[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec120[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec145[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec146[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec147[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec148[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec149[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec150[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec151[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec152[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec153[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec154[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec155[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec156[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec157[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec158[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec159[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec160[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec161[2];
	FAUSTFLOAT 	fbargraph49;
	double 	fRec162[2];
	FAUSTFLOAT 	fbargraph50;
	double 	fRec163[2];
	FAUSTFLOAT 	fbargraph51;
	double 	fRec164[2];
	FAUSTFLOAT 	fbargraph52;
	double 	fRec165[2];
	FAUSTFLOAT 	fbargraph53;
	double 	fRec166[2];
	FAUSTFLOAT 	fbargraph54;
	double 	fRec167[2];
	FAUSTFLOAT 	fbargraph55;
	double 	fRec168[2];
	FAUSTFLOAT 	fbargraph56;
	double 	fRec169[2];
	FAUSTFLOAT 	fbargraph57;
	double 	fRec170[2];
	FAUSTFLOAT 	fbargraph58;
	double 	fRec171[2];
	FAUSTFLOAT 	fbargraph59;
	double 	fRec172[2];
	FAUSTFLOAT 	fbargraph60;
	double 	fRec173[2];
	FAUSTFLOAT 	fbargraph61;
	double 	fRec174[2];
	FAUSTFLOAT 	fbargraph62;
	double 	fRec175[2];
	FAUSTFLOAT 	fbargraph63;
	double 	fRec176[2];
	FAUSTFLOAT 	fbargraph64;
	double 	fRec177[2];
	FAUSTFLOAT 	fbargraph65;
	double 	fRec178[2];
	FAUSTFLOAT 	fbargraph66;
	double 	fRec179[2];
	FAUSTFLOAT 	fbargraph67;
	double 	fRec180[2];
	FAUSTFLOAT 	fbargraph68;
	double 	fRec181[2];
	FAUSTFLOAT 	fbargraph69;
	double 	fRec182[2];
	FAUSTFLOAT 	fbargraph70;
	double 	fRec183[2];
	FAUSTFLOAT 	fbargraph71;
	double 	fRec184[2];
	FAUSTFLOAT 	fbargraph72;
	double 	fRec185[2];
	FAUSTFLOAT 	fbargraph73;
	double 	fRec186[2];
	FAUSTFLOAT 	fbargraph74;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOADecLebedev504");
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

	virtual int getNumInputs() { return 25; }
	virtual int getNumOutputs() { return 50; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(1.92e+05, max(1e+03, (double)fSamplingFreq));
		fConst1 = (8e+01 / fConst0);
		fConst2 = (1.7e+02 / fConst0);
		fConst3 = (3.4e+02 / fConst0);
		fConst4 = (5.1e+02 / fConst0);
		fConst5 = (1.02e+03 / fConst0);
		fConst6 = (394.7715102864353 / fConst0);
		fConst7 = (625.2284897135664 / fConst0);
		fConst8 = (1250.4569794271329 / fConst0);
		fConst9 = (789.5430205728705 / fConst0);
		fConst10 = (984.7116049589241 / fConst0);
		fConst11 = (715.2883950410759 / fConst0);
		fConst12 = (1430.5767900821518 / fConst0);
		fConst13 = (1969.4232099178482 / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.0;
		fcheckbox0 = 0.0;
		fentry0 = 1.07;
		fslider1 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fRec89[i] = 0;
		for (int i=0; i<2; i++) fRec88[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<2; i++) fRec94[i] = 0;
		for (int i=0; i<2; i++) fRec93[i] = 0;
		for (int i=0; i<2; i++) fRec92[i] = 0;
		for (int i=0; i<2; i++) fRec96[i] = 0;
		for (int i=0; i<2; i++) fRec95[i] = 0;
		for (int i=0; i<2; i++) fRec91[i] = 0;
		for (int i=0; i<2; i++) fRec90[i] = 0;
		for (int i=0; i<2; i++) fRec101[i] = 0;
		for (int i=0; i<2; i++) fRec100[i] = 0;
		for (int i=0; i<2; i++) fRec99[i] = 0;
		for (int i=0; i<2; i++) fRec102[i] = 0;
		for (int i=0; i<2; i++) fRec98[i] = 0;
		for (int i=0; i<2; i++) fRec97[i] = 0;
		for (int i=0; i<2; i++) fRec107[i] = 0;
		for (int i=0; i<2; i++) fRec106[i] = 0;
		for (int i=0; i<2; i++) fRec105[i] = 0;
		for (int i=0; i<2; i++) fRec108[i] = 0;
		for (int i=0; i<2; i++) fRec104[i] = 0;
		for (int i=0; i<2; i++) fRec103[i] = 0;
		for (int i=0; i<2; i++) fRec113[i] = 0;
		for (int i=0; i<2; i++) fRec112[i] = 0;
		for (int i=0; i<2; i++) fRec111[i] = 0;
		for (int i=0; i<2; i++) fRec115[i] = 0;
		for (int i=0; i<2; i++) fRec114[i] = 0;
		for (int i=0; i<2; i++) fRec110[i] = 0;
		for (int i=0; i<2; i++) fRec109[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec116[i] = 0;
		for (int i=0; i<2; i++) fRec117[i] = 0;
		for (int i=0; i<2; i++) fRec118[i] = 0;
		for (int i=0; i<2; i++) fRec119[i] = 0;
		for (int i=0; i<2; i++) fRec124[i] = 0;
		for (int i=0; i<2; i++) fRec123[i] = 0;
		for (int i=0; i<2; i++) fRec122[i] = 0;
		for (int i=0; i<2; i++) fRec121[i] = 0;
		for (int i=0; i<2; i++) fRec129[i] = 0;
		for (int i=0; i<2; i++) fRec128[i] = 0;
		for (int i=0; i<2; i++) fRec127[i] = 0;
		for (int i=0; i<2; i++) fRec130[i] = 0;
		for (int i=0; i<2; i++) fRec126[i] = 0;
		for (int i=0; i<2; i++) fRec125[i] = 0;
		for (int i=0; i<2; i++) fRec135[i] = 0;
		for (int i=0; i<2; i++) fRec134[i] = 0;
		for (int i=0; i<2; i++) fRec133[i] = 0;
		for (int i=0; i<2; i++) fRec137[i] = 0;
		for (int i=0; i<2; i++) fRec136[i] = 0;
		for (int i=0; i<2; i++) fRec132[i] = 0;
		for (int i=0; i<2; i++) fRec131[i] = 0;
		for (int i=0; i<2; i++) fRec142[i] = 0;
		for (int i=0; i<2; i++) fRec141[i] = 0;
		for (int i=0; i<2; i++) fRec140[i] = 0;
		for (int i=0; i<2; i++) fRec144[i] = 0;
		for (int i=0; i<2; i++) fRec143[i] = 0;
		for (int i=0; i<2; i++) fRec139[i] = 0;
		for (int i=0; i<2; i++) fRec138[i] = 0;
		for (int i=0; i<2; i++) fRec120[i] = 0;
		for (int i=0; i<2; i++) fRec145[i] = 0;
		for (int i=0; i<2; i++) fRec146[i] = 0;
		for (int i=0; i<2; i++) fRec147[i] = 0;
		for (int i=0; i<2; i++) fRec148[i] = 0;
		for (int i=0; i<2; i++) fRec149[i] = 0;
		for (int i=0; i<2; i++) fRec150[i] = 0;
		for (int i=0; i<2; i++) fRec151[i] = 0;
		for (int i=0; i<2; i++) fRec152[i] = 0;
		for (int i=0; i<2; i++) fRec153[i] = 0;
		for (int i=0; i<2; i++) fRec154[i] = 0;
		for (int i=0; i<2; i++) fRec155[i] = 0;
		for (int i=0; i<2; i++) fRec156[i] = 0;
		for (int i=0; i<2; i++) fRec157[i] = 0;
		for (int i=0; i<2; i++) fRec158[i] = 0;
		for (int i=0; i<2; i++) fRec159[i] = 0;
		for (int i=0; i<2; i++) fRec160[i] = 0;
		for (int i=0; i<2; i++) fRec161[i] = 0;
		for (int i=0; i<2; i++) fRec162[i] = 0;
		for (int i=0; i<2; i++) fRec163[i] = 0;
		for (int i=0; i<2; i++) fRec164[i] = 0;
		for (int i=0; i<2; i++) fRec165[i] = 0;
		for (int i=0; i<2; i++) fRec166[i] = 0;
		for (int i=0; i<2; i++) fRec167[i] = 0;
		for (int i=0; i<2; i++) fRec168[i] = 0;
		for (int i=0; i<2; i++) fRec169[i] = 0;
		for (int i=0; i<2; i++) fRec170[i] = 0;
		for (int i=0; i<2; i++) fRec171[i] = 0;
		for (int i=0; i<2; i++) fRec172[i] = 0;
		for (int i=0; i<2; i++) fRec173[i] = 0;
		for (int i=0; i<2; i++) fRec174[i] = 0;
		for (int i=0; i<2; i++) fRec175[i] = 0;
		for (int i=0; i<2; i++) fRec176[i] = 0;
		for (int i=0; i<2; i++) fRec177[i] = 0;
		for (int i=0; i<2; i++) fRec178[i] = 0;
		for (int i=0; i<2; i++) fRec179[i] = 0;
		for (int i=0; i<2; i++) fRec180[i] = 0;
		for (int i=0; i<2; i++) fRec181[i] = 0;
		for (int i=0; i<2; i++) fRec182[i] = 0;
		for (int i=0; i<2; i++) fRec183[i] = 0;
		for (int i=0; i<2; i++) fRec184[i] = 0;
		for (int i=0; i<2; i++) fRec185[i] = 0;
		for (int i=0; i<2; i++) fRec186[i] = 0;
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
		ui_interface->openVerticalBox("HOADecLebedev504");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("B-Format");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75ce9b50", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe7588ed40", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75ceccf0", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75cef5a0", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe7588c650", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe7488de20", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75cf34f0", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75cf6540", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75cf9600", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75896d10", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe74891900", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe7589a760", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75d027e0", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75d06370", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75d09fd0", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75d0dcf0", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe758932a0", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe7489bd40", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe7487d100", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe74895e00", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75d13c60", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75d17ec0", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75d1c270", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75d20680", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe74876e50", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(&fslider1, "1", "");
		ui_interface->declare(&fslider1, "osc", "/levelin -10 60");
		ui_interface->declare(&fslider1, "unit", "dB");
		ui_interface->addVerticalSlider("Inputs Gain", &fslider1, 0.0, -1e+01, 6e+01, 0.1);
		ui_interface->declare(&fslider0, "2", "");
		ui_interface->declare(&fslider0, "osc", "/levelout -10 60");
		ui_interface->declare(&fslider0, "unit", "dB");
		ui_interface->addVerticalSlider("Outputs Gain", &fslider0, 0.0, -1e+01, 6e+01, 0.1);
		ui_interface->declare(0, "3", "");
		ui_interface->openVerticalBox("NFC");
		ui_interface->addCheckButton("Yes", &fcheckbox0);
		ui_interface->closeBox();
		ui_interface->declare(&fentry0, "4", "");
		ui_interface->addNumEntry("Speakers Radius", &fentry0, 1.07, 0.5, 1e+01, 0.01);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 1-25");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph15, "osc", "/output1");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe74878d40", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph16, "osc", "/output2");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75889700", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph23, "osc", "/output3");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe7487ec20", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph24, "osc", "/output4");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe74881a10", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph25, "osc", "/output5");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe74884a50", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph26, "osc", "/output6");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe74886d20", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph27, "osc", "/output7");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe7488ad20", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph32, "osc", "/output8");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe7489de80", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph33, "osc", "/output9");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe748a1fc0", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph34, "osc", "/output10");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe748a6120", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph35, "osc", "/output11");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe748a9ae0", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph36, "osc", "/output12");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe748acf40", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph37, "osc", "/output13");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75ad4680", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph38, "osc", "/output14");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75ad75d0", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph39, "osc", "/output15");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75ad9b60", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph40, "osc", "/output16");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75adce20", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph41, "osc", "/output17");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75ae00d0", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph42, "osc", "/output18");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75ae32f0", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph43, "osc", "/output19");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75ae9420", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph44, "osc", "/output20");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75aedd70", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph45, "osc", "/output21");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75af2080", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph46, "osc", "/output22");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75af6d00", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph47, "osc", "/output23");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75afa360", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph48, "osc", "/output24");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e018a0", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph49, "osc", "/output25");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e051c0", &fbargraph49, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph50, "osc", "/output26");
		ui_interface->declare(&fbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e08db0", &fbargraph50, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph51, "osc", "/output27");
		ui_interface->declare(&fbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e0e400", &fbargraph51, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph52, "osc", "/output28");
		ui_interface->declare(&fbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e12630", &fbargraph52, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph53, "osc", "/output29");
		ui_interface->declare(&fbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e16ed0", &fbargraph53, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph54, "osc", "/output30");
		ui_interface->declare(&fbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e1b280", &fbargraph54, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph55, "osc", "/output31");
		ui_interface->declare(&fbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e20cc0", &fbargraph55, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph56, "osc", "/output32");
		ui_interface->declare(&fbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e262a0", &fbargraph56, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph57, "osc", "/output33");
		ui_interface->declare(&fbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e2b220", &fbargraph57, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph58, "osc", "/output34");
		ui_interface->declare(&fbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e2fbe0", &fbargraph58, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph59, "osc", "/output35");
		ui_interface->declare(&fbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e34870", &fbargraph59, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fbargraph60, "osc", "/output36");
		ui_interface->declare(&fbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e39420", &fbargraph60, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fbargraph61, "osc", "/output37");
		ui_interface->declare(&fbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e3e040", &fbargraph61, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fbargraph62, "osc", "/output38");
		ui_interface->declare(&fbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e43ce0", &fbargraph62, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fbargraph63, "osc", "/output39");
		ui_interface->declare(&fbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e46f40", &fbargraph63, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fbargraph64, "osc", "/output40");
		ui_interface->declare(&fbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e4a6f0", &fbargraph64, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fbargraph65, "osc", "/output41");
		ui_interface->declare(&fbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e4de10", &fbargraph65, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fbargraph66, "osc", "/output42");
		ui_interface->declare(&fbargraph66, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e515c0", &fbargraph66, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fbargraph67, "osc", "/output43");
		ui_interface->declare(&fbargraph67, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e54b00", &fbargraph67, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fbargraph68, "osc", "/output44");
		ui_interface->declare(&fbargraph68, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e58040", &fbargraph68, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fbargraph69, "osc", "/output45");
		ui_interface->declare(&fbargraph69, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e5ba00", &fbargraph69, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fbargraph70, "osc", "/output46");
		ui_interface->declare(&fbargraph70, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e5f2b0", &fbargraph70, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fbargraph71, "osc", "/output47");
		ui_interface->declare(&fbargraph71, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e62750", &fbargraph71, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fbargraph72, "osc", "/output48");
		ui_interface->declare(&fbargraph72, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e65e60", &fbargraph72, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fbargraph73, "osc", "/output49");
		ui_interface->declare(&fbargraph73, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e69880", &fbargraph73, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fbargraph74, "osc", "/output50");
		ui_interface->declare(&fbargraph74, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ffe75e6ce70", &fbargraph74, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider0))));
		double 	fSlow1 = double(fcheckbox0);
		double 	fSlow2 = (1 - fSlow1);
		double 	fSlow3 = double(fentry0);
		double 	fSlow4 = (fSlow1 * fSlow3);
		double 	fSlow5 = (fSlow2 + fSlow4);
		double 	fSlow6 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider1))));
		double 	fSlow7 = (fConst2 / fSlow3);
		double 	fSlow8 = (1.0 / (fSlow7 + 1));
		double 	fSlow9 = (fConst3 / fSlow3);
		double 	fSlow10 = faustpower<2>(fSlow7);
		double 	fSlow11 = (1.0 / (((fConst4 / fSlow3) + (3 * fSlow10)) + 1));
		double 	fSlow12 = (12 * fSlow10);
		double 	fSlow13 = (fSlow12 + (fConst5 / fSlow3));
		double 	fSlow14 = (1.0 / ((fConst6 / fSlow3) + 1));
		double 	fSlow15 = (1.0 / (((fConst7 / fSlow3) + (6.45943269348338 * fSlow10)) + 1));
		double 	fSlow16 = (25.83773077393352 * fSlow10);
		double 	fSlow17 = (fSlow16 + (fConst8 / fSlow3));
		double 	fSlow18 = (fConst9 / fSlow3);
		double 	fSlow19 = (1.0 / (((fConst10 / fSlow3) + (9.1401308902779 * fSlow10)) + 1));
		double 	fSlow20 = (1.0 / (((fConst11 / fSlow3) + (11.4878004768713 * fSlow10)) + 1));
		double 	fSlow21 = (45.9512019074852 * fSlow10);
		double 	fSlow22 = (fSlow21 + (fConst12 / fSlow3));
		double 	fSlow23 = (36.5605235611116 * fSlow10);
		double 	fSlow24 = (fSlow23 + (fConst13 / fSlow3));
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
		FAUSTFLOAT* input16 = input[16];
		FAUSTFLOAT* input17 = input[17];
		FAUSTFLOAT* input18 = input[18];
		FAUSTFLOAT* input19 = input[19];
		FAUSTFLOAT* input20 = input[20];
		FAUSTFLOAT* input21 = input[21];
		FAUSTFLOAT* input22 = input[22];
		FAUSTFLOAT* input23 = input[23];
		FAUSTFLOAT* input24 = input[24];
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
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			fRec3[0] = (fSlow6 + (0.999 * fRec3[1]));
			double fTemp0 = (fSlow5 * ((double)input0[i] * fRec3[0]));
			fRec2[0] = max((fRec2[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fbargraph0 = fRec2[0];
			double fTemp1 = fTemp0;
			double fTemp2 = (0.012698412698412698 * fTemp1);
			double fTemp3 = ((double)input2[i] * fRec3[0]);
			fRec6[0] = (fRec6[1] + fRec5[1]);
			fRec5[0] = (fSlow8 * ((fSlow4 * fTemp3) - (fSlow9 * fRec6[0])));
			double fTemp4 = (fRec5[0] + (fSlow2 * fTemp3));
			fRec4[0] = max((fRec4[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fbargraph1 = fRec4[0];
			double fTemp5 = fTemp4;
			double fTemp6 = (0.0219942959691286 * fTemp5);
			double fTemp7 = ((double)input3[i] * fRec3[0]);
			fRec9[0] = (fRec9[1] + fRec8[1]);
			fRec8[0] = (fSlow8 * ((fSlow4 * fTemp7) - (fSlow9 * fRec9[0])));
			double fTemp8 = (fRec8[0] + (fSlow2 * fTemp7));
			fRec7[0] = max((fRec7[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fbargraph2 = fRec7[0];
			double fTemp9 = fTemp8;
			double fTemp10 = (1.3467622079046435e-18 * fTemp9);
			double fTemp11 = ((double)input6[i] * fRec3[0]);
			fRec13[0] = (fRec13[1] + fRec11[1]);
			fRec12[0] = (fRec12[1] + fRec13[1]);
			fRec11[0] = (fSlow11 * ((fSlow4 * fTemp11) - ((fSlow12 * fRec12[0]) + (fSlow13 * fRec13[0]))));
			double fTemp12 = (fRec11[0] + (fSlow2 * fTemp11));
			fRec10[0] = max((fRec10[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fbargraph3 = fRec10[0];
			double fTemp13 = fTemp12;
			double fTemp14 = (0.02839451399999733 * fTemp13);
			double fTemp15 = ((double)input7[i] * fRec3[0]);
			fRec17[0] = (fRec17[1] + fRec15[1]);
			fRec16[0] = (fRec16[1] + fRec17[1]);
			fRec15[0] = (fSlow11 * ((fSlow4 * fTemp15) - ((fSlow12 * fRec16[0]) + (fSlow13 * fRec17[0]))));
			double fTemp16 = (fRec15[0] + (fSlow2 * fTemp15));
			fRec14[0] = max((fRec14[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fbargraph4 = fRec14[0];
			double fTemp17 = fTemp16;
			double fTemp18 = (3.0114518464024883e-18 * fTemp17);
			double fTemp19 = ((double)input8[i] * fRec3[0]);
			fRec21[0] = (fRec21[1] + fRec19[1]);
			fRec20[0] = (fRec20[1] + fRec21[1]);
			fRec19[0] = (fSlow11 * ((fSlow4 * fTemp19) - ((fSlow12 * fRec20[0]) + (fSlow13 * fRec21[0]))));
			double fTemp20 = (fRec19[0] + (fSlow2 * fTemp19));
			fRec18[0] = max((fRec18[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph5 = fRec18[0];
			double fTemp21 = fTemp20;
			double fTemp22 = (9.219912161207984e-35 * fTemp21);
			double fTemp23 = ((double)input12[i] * fRec3[0]);
			fRec26[0] = (fRec26[1] + fRec24[1]);
			fRec25[0] = (fRec25[1] + fRec26[1]);
			fRec24[0] = (fSlow15 * ((fSlow4 * fTemp23) - ((fSlow16 * fRec25[0]) + (fSlow17 * fRec26[0]))));
			fRec27[0] = (fRec27[1] + fRec23[1]);
			fRec23[0] = (fSlow14 * (fRec24[0] - (fSlow18 * fRec27[0])));
			double fTemp24 = (fRec23[0] + (fSlow2 * fTemp23));
			fRec22[0] = max((fRec22[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fbargraph6 = fRec22[0];
			double fTemp25 = fTemp24;
			double fTemp26 = (0.03359684204526464 * fTemp25);
			double fTemp27 = ((double)input13[i] * fRec3[0]);
			fRec32[0] = (fRec32[1] + fRec30[1]);
			fRec31[0] = (fRec31[1] + fRec32[1]);
			fRec30[0] = (fSlow15 * ((fSlow4 * fTemp27) - ((fSlow16 * fRec31[0]) + (fSlow17 * fRec32[0]))));
			fRec33[0] = (fRec33[1] + fRec29[1]);
			fRec29[0] = (fSlow14 * (fRec30[0] - (fSlow18 * fRec33[0])));
			double fTemp28 = (fRec29[0] + (fSlow2 * fTemp27));
			fRec28[0] = max((fRec28[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph7 = fRec28[0];
			double fTemp29 = fTemp28;
			double fTemp30 = (5.0391227634343926e-18 * fTemp29);
			double fTemp31 = ((double)input14[i] * fRec3[0]);
			fRec38[0] = (fRec38[1] + fRec36[1]);
			fRec37[0] = (fRec37[1] + fRec38[1]);
			fRec36[0] = (fSlow15 * ((fSlow4 * fTemp31) - ((fSlow16 * fRec37[0]) + (fSlow17 * fRec38[0]))));
			fRec39[0] = (fRec39[1] + fRec35[1]);
			fRec35[0] = (fSlow14 * (fRec36[0] - (fSlow18 * fRec39[0])));
			double fTemp32 = (fRec35[0] + (fSlow2 * fTemp31));
			fRec34[0] = max((fRec34[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph8 = fRec34[0];
			double fTemp33 = fTemp32;
			double fTemp34 = (2.4393594688416384e-34 * fTemp33);
			double fTemp35 = ((double)input15[i] * fRec3[0]);
			fRec44[0] = (fRec44[1] + fRec42[1]);
			fRec43[0] = (fRec43[1] + fRec44[1]);
			fRec42[0] = (fSlow15 * ((fSlow4 * fTemp35) - ((fSlow16 * fRec43[0]) + (fSlow17 * fRec44[0]))));
			fRec45[0] = (fRec45[1] + fRec41[1]);
			fRec41[0] = (fSlow14 * (fRec42[0] - (fSlow18 * fRec45[0])));
			double fTemp36 = (fRec41[0] + (fSlow2 * fTemp35));
			fRec40[0] = max((fRec40[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph9 = fRec40[0];
			double fTemp37 = fTemp36;
			double fTemp38 = (6.097910338853647e-51 * fTemp37);
			double fTemp39 = ((double)input20[i] * fRec3[0]);
			fRec50[0] = (fRec50[1] + fRec48[1]);
			fRec49[0] = (fRec49[1] + fRec50[1]);
			fRec48[0] = (fSlow20 * ((fSlow4 * fTemp39) - ((fSlow21 * fRec49[0]) + (fSlow22 * fRec50[0]))));
			fRec52[0] = (fRec52[1] + fRec47[1]);
			fRec51[0] = (fRec51[1] + fRec52[1]);
			fRec47[0] = (fSlow19 * (fRec48[0] - ((fSlow23 * fRec51[0]) + (fSlow24 * fRec52[0]))));
			double fTemp40 = (fRec47[0] + (fSlow2 * fTemp39));
			fRec46[0] = max((fRec46[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fbargraph10 = fRec46[0];
			double fTemp41 = fTemp40;
			double fTemp42 = (0.0380952380952381 * fTemp41);
			double fTemp43 = ((double)input21[i] * fRec3[0]);
			fRec57[0] = (fRec57[1] + fRec55[1]);
			fRec56[0] = (fRec56[1] + fRec57[1]);
			fRec55[0] = (fSlow20 * ((fSlow4 * fTemp43) - ((fSlow21 * fRec56[0]) + (fSlow22 * fRec57[0]))));
			fRec59[0] = (fRec59[1] + fRec54[1]);
			fRec58[0] = (fRec58[1] + fRec59[1]);
			fRec54[0] = (fSlow19 * (fRec55[0] - ((fSlow23 * fRec58[0]) + (fSlow24 * fRec59[0]))));
			double fTemp44 = (fRec54[0] + (fSlow2 * fTemp43));
			fRec53[0] = max((fRec53[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fbargraph11 = fRec53[0];
			double fTemp45 = fTemp44;
			double fTemp46 = (7.376520408648356e-18 * fTemp45);
			double fTemp47 = ((double)input22[i] * fRec3[0]);
			fRec64[0] = (fRec64[1] + fRec62[1]);
			fRec63[0] = (fRec63[1] + fRec64[1]);
			fRec62[0] = (fSlow20 * ((fSlow4 * fTemp47) - ((fSlow21 * fRec63[0]) + (fSlow22 * fRec64[0]))));
			fRec66[0] = (fRec66[1] + fRec61[1]);
			fRec65[0] = (fRec65[1] + fRec66[1]);
			fRec61[0] = (fSlow19 * (fRec62[0] - ((fSlow23 * fRec65[0]) + (fSlow24 * fRec66[0]))));
			double fTemp48 = (fRec61[0] + (fSlow2 * fTemp47));
			fRec60[0] = max((fRec60[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fbargraph12 = fRec60[0];
			double fTemp49 = fTemp48;
			double fTemp50 = (4.79080689136032e-34 * fTemp49);
			double fTemp51 = ((double)input23[i] * fRec3[0]);
			fRec71[0] = (fRec71[1] + fRec69[1]);
			fRec70[0] = (fRec70[1] + fRec71[1]);
			fRec69[0] = (fSlow20 * ((fSlow4 * fTemp51) - ((fSlow21 * fRec70[0]) + (fSlow22 * fRec71[0]))));
			fRec73[0] = (fRec73[1] + fRec68[1]);
			fRec72[0] = (fRec72[1] + fRec73[1]);
			fRec68[0] = (fSlow19 * (fRec69[0] - ((fSlow23 * fRec72[0]) + (fSlow24 * fRec73[0]))));
			double fTemp52 = (fRec68[0] + (fSlow2 * fTemp51));
			fRec67[0] = max((fRec67[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fbargraph13 = fRec67[0];
			double fTemp53 = fTemp52;
			double fTemp54 = (1.829373101656094e-50 * fTemp53);
			double fTemp55 = ((double)input24[i] * fRec3[0]);
			fRec78[0] = (fRec78[1] + fRec76[1]);
			fRec77[0] = (fRec77[1] + fRec78[1]);
			fRec76[0] = (fSlow20 * ((fSlow4 * fTemp55) - ((fSlow21 * fRec77[0]) + (fSlow22 * fRec78[0]))));
			fRec80[0] = (fRec80[1] + fRec75[1]);
			fRec79[0] = (fRec79[1] + fRec80[1]);
			fRec75[0] = (fSlow19 * (fRec76[0] - ((fSlow23 * fRec79[0]) + (fSlow24 * fRec80[0]))));
			double fTemp56 = (fRec75[0] + (fSlow2 * fTemp55));
			fRec74[0] = max((fRec74[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fbargraph14 = fRec74[0];
			double fTemp57 = fTemp56;
			double fTemp58 = (3.9603917912335094e-67 * fTemp57);
			double fTemp59 = (fRec1[0] * ((((((((((((((fTemp2 + fTemp6) + fTemp10) + fTemp14) + fTemp18) + fTemp22) + fTemp26) + fTemp30) + fTemp34) + fTemp38) + fTemp42) + fTemp46) + fTemp50) + fTemp54) + fTemp58));
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fbargraph15 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp59;
			double fTemp60 = (0.0219942959691286 * fTemp9);
			double fTemp61 = (0.024590370452110585 * fTemp21);
			double fTemp62 = (0.026560635762986527 * fTemp37);
			double fTemp63 = (0.014285714285714285 * fTemp41);
			double fTemp64 = (0.028171808490950554 * fTemp57);
			double fTemp65 = (0.014197256999998666 * fTemp13);
			double fTemp66 = (0.020573779994945588 * fTemp29);
			double fTemp67 = (0.021295885499998 * fTemp49);
			double fTemp68 = (fRec1[0] * ((((((fTemp2 + fTemp60) + fTemp61) + fTemp62) + fTemp63) + fTemp64) - ((fTemp65 + fTemp66) + fTemp67)));
			fRec81[0] = max((fRec81[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fbargraph16 = fRec81[0];
			output1[i] = (FAUSTFLOAT)fTemp68;
			double fTemp69 = ((double)input4[i] * fRec3[0]);
			fRec86[0] = (fRec86[1] + fRec84[1]);
			fRec85[0] = (fRec85[1] + fRec86[1]);
			fRec84[0] = (fSlow11 * ((fSlow4 * fTemp69) - ((fSlow12 * fRec85[0]) + (fSlow13 * fRec86[0]))));
			double fTemp70 = (fRec84[0] + (fSlow2 * fTemp69));
			fRec83[0] = max((fRec83[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp70))))));
			fbargraph17 = fRec83[0];
			double fTemp71 = fTemp70;
			double fTemp72 = ((double)input1[i] * fRec3[0]);
			fRec89[0] = (fRec89[1] + fRec88[1]);
			fRec88[0] = (fSlow8 * ((fSlow4 * fTemp72) - (fSlow9 * fRec89[0])));
			double fTemp73 = (fRec88[0] + (fSlow2 * fTemp72));
			fRec87[0] = max((fRec87[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp73))))));
			fbargraph18 = fRec87[0];
			double fTemp74 = fTemp73;
			double fTemp75 = (0.0219942959691286 * fTemp74);
			double fTemp76 = (fTemp2 + fTemp10);
			double fTemp77 = ((double)input16[i] * fRec3[0]);
			fRec94[0] = (fRec94[1] + fRec92[1]);
			fRec93[0] = (fRec93[1] + fRec94[1]);
			fRec92[0] = (fSlow20 * ((fSlow4 * fTemp77) - ((fSlow21 * fRec93[0]) + (fSlow22 * fRec94[0]))));
			fRec96[0] = (fRec96[1] + fRec91[1]);
			fRec95[0] = (fRec95[1] + fRec96[1]);
			fRec91[0] = (fSlow19 * (fRec92[0] - ((fSlow23 * fRec95[0]) + (fSlow24 * fRec96[0]))));
			double fTemp78 = (fRec91[0] + (fSlow2 * fTemp77));
			fRec90[0] = max((fRec90[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fbargraph19 = fRec90[0];
			double fTemp79 = fTemp78;
			double fTemp80 = ((double)input9[i] * fRec3[0]);
			fRec101[0] = (fRec101[1] + fRec99[1]);
			fRec100[0] = (fRec100[1] + fRec101[1]);
			fRec99[0] = (fSlow15 * ((fSlow4 * fTemp80) - ((fSlow16 * fRec100[0]) + (fSlow17 * fRec101[0]))));
			fRec102[0] = (fRec102[1] + fRec98[1]);
			fRec98[0] = (fSlow14 * (fRec99[0] - (fSlow18 * fRec102[0])));
			double fTemp81 = (fRec98[0] + (fSlow2 * fTemp80));
			fRec97[0] = max((fRec97[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp81))))));
			fbargraph20 = fRec97[0];
			double fTemp82 = fTemp81;
			double fTemp83 = (0.026560635762986527 * fTemp82);
			double fTemp84 = ((double)input11[i] * fRec3[0]);
			fRec107[0] = (fRec107[1] + fRec105[1]);
			fRec106[0] = (fRec106[1] + fRec107[1]);
			fRec105[0] = (fSlow15 * ((fSlow4 * fTemp84) - ((fSlow16 * fRec106[0]) + (fSlow17 * fRec107[0]))));
			fRec108[0] = (fRec108[1] + fRec104[1]);
			fRec104[0] = (fSlow14 * (fRec105[0] - (fSlow18 * fRec108[0])));
			double fTemp85 = (fRec104[0] + (fSlow2 * fTemp84));
			fRec103[0] = max((fRec103[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp85))))));
			fbargraph21 = fRec103[0];
			double fTemp86 = fTemp85;
			double fTemp87 = (0.020573779994945588 * fTemp86);
			double fTemp88 = ((double)input18[i] * fRec3[0]);
			fRec113[0] = (fRec113[1] + fRec111[1]);
			fRec112[0] = (fRec112[1] + fRec113[1]);
			fRec111[0] = (fSlow20 * ((fSlow4 * fTemp88) - ((fSlow21 * fRec112[0]) + (fSlow22 * fRec113[0]))));
			fRec115[0] = (fRec115[1] + fRec110[1]);
			fRec114[0] = (fRec114[1] + fRec115[1]);
			fRec110[0] = (fSlow19 * (fRec111[0] - ((fSlow23 * fRec114[0]) + (fSlow24 * fRec115[0]))));
			double fTemp89 = (fRec110[0] + (fSlow2 * fTemp88));
			fRec109[0] = max((fRec109[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp89))))));
			fbargraph22 = fRec109[0];
			double fTemp90 = fTemp89;
			double fTemp91 = (fRec1[0] * (((3.0114518464024883e-18 * fTemp71) + (fTemp75 + (fTemp64 + (fTemp67 + (fTemp63 + fTemp76))))) - ((6.900103018926964e-18 * fTemp79) + (fTemp83 + ((fTemp87 + ((4.879109635569025e-18 * fTemp37) + ((fTemp65 + fTemp61) + (1.2597806908585981e-18 * fTemp29)))) + (2.607993801258108e-18 * fTemp90))))));
			fRec82[0] = max((fRec82[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp91))))));
			fbargraph23 = fRec82[0];
			output2[i] = (FAUSTFLOAT)fTemp91;
			double fTemp92 = (fRec1[0] * (((9.75821927113805e-18 * fTemp82) + ((2.693524415809287e-18 * fTemp74) + ((5.215987602516216e-18 * fTemp90) + (fTemp64 + (fTemp63 + (fTemp66 + (fTemp2 + fTemp61))))))) - ((1.3800206037853928e-17 * fTemp79) + ((6.0229036928049765e-18 * fTemp71) + ((2.5195613817171963e-18 * fTemp86) + (fTemp67 + (fTemp62 + (fTemp60 + fTemp65))))))));
			fRec116[0] = max((fRec116[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp92))))));
			fbargraph24 = fRec116[0];
			output3[i] = (FAUSTFLOAT)fTemp92;
			double fTemp93 = (fRec1[0] * ((fTemp83 + ((9.034355539207464e-18 * fTemp71) + (fTemp87 + (fTemp64 + (fTemp67 + (fTemp63 + ((fTemp2 + (3.779342072575794e-18 * fTemp29)) + (1.4637328906707076e-17 * fTemp37)))))))) - ((2.0700309056780893e-17 * fTemp79) + (fTemp75 + ((fTemp61 + (fTemp65 + (4.040286623713931e-18 * fTemp9))) + (7.823981403774323e-18 * fTemp90))))));
			fRec117[0] = max((fRec117[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fbargraph25 = fRec117[0];
			output4[i] = (FAUSTFLOAT)fTemp93;
			double fTemp94 = (fRec1[0] * ((fTemp58 + (fTemp50 + (fTemp42 + (fTemp38 + (fTemp30 + (fTemp22 + (fTemp14 + fTemp76))))))) - (fTemp54 + (fTemp46 + (fTemp34 + (fTemp26 + (fTemp6 + fTemp18)))))));
			fRec118[0] = max((fRec118[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp94))))));
			fbargraph26 = fRec118[0];
			output5[i] = (FAUSTFLOAT)fTemp94;
			double fTemp95 = (0.012520803773755805 * fTemp57);
			double fTemp96 = (0.035414181017315374 * fTemp53);
			double fTemp97 = (0.04089277388695433 * fTemp33);
			double fTemp98 = (0.022574955908289243 * fTemp1);
			double fTemp99 = (0.0276485614705685 * fTemp5);
			double fTemp100 = (fTemp98 + fTemp99);
			double fTemp101 = (0.027648561470568506 * fTemp9);
			double fTemp102 = (0.012619783999998805 * fTemp13);
			double fTemp103 = (0.043716214137085485 * fTemp17);
			double fTemp104 = (0.02185810706854275 * fTemp21);
			double fTemp105 = (0.03879429159750975 * fTemp29);
			double fTemp106 = (0.01669440503167441 * fTemp37);
			double fTemp107 = (0.013385302265262959 * fTemp45);
			double fTemp108 = (0.04732418999999555 * fTemp49);
			double fTemp109 = (0.010558468816293319 * fTemp25);
			double fTemp110 = (0.027513227513227514 * fTemp41);
			double fTemp111 = (fRec1[0] * ((fTemp95 + (fTemp96 + ((((fTemp97 + (((((fTemp100 + fTemp101) + fTemp102) + fTemp103) + fTemp104) + fTemp105)) + fTemp106) + fTemp107) + fTemp108))) - (fTemp109 + fTemp110)));
			fRec119[0] = max((fRec119[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp111))))));
			fbargraph27 = fRec119[0];
			output6[i] = (FAUSTFLOAT)fTemp111;
			double fTemp112 = ((double)input5[i] * fRec3[0]);
			fRec124[0] = (fRec124[1] + fRec122[1]);
			fRec123[0] = (fRec123[1] + fRec124[1]);
			fRec122[0] = (fSlow11 * ((fSlow4 * fTemp112) - ((fSlow12 * fRec123[0]) + (fSlow13 * fRec124[0]))));
			double fTemp113 = (fRec122[0] + (fSlow2 * fTemp112));
			fRec121[0] = max((fRec121[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp113))))));
			fbargraph28 = fRec121[0];
			double fTemp114 = fTemp113;
			double fTemp115 = (0.043716214137085485 * fTemp114);
			double fTemp116 = ((double)input10[i] * fRec3[0]);
			fRec129[0] = (fRec129[1] + fRec127[1]);
			fRec128[0] = (fRec128[1] + fRec129[1]);
			fRec127[0] = (fSlow15 * ((fSlow4 * fTemp116) - ((fSlow16 * fRec128[0]) + (fSlow17 * fRec129[0]))));
			fRec130[0] = (fRec130[1] + fRec126[1]);
			fRec126[0] = (fSlow14 * (fRec127[0] - (fSlow18 * fRec130[0])));
			double fTemp117 = (fRec126[0] + (fSlow2 * fTemp116));
			fRec125[0] = max((fRec125[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp117))))));
			fbargraph29 = fRec125[0];
			double fTemp118 = fTemp117;
			double fTemp119 = (5.007920464891509e-18 * fTemp118);
			double fTemp120 = (2.6768460856911014e-18 * fTemp71);
			double fTemp121 = (0.027648561470568506 * fTemp74);
			double fTemp122 = ((double)input19[i] * fRec3[0]);
			fRec135[0] = (fRec135[1] + fRec133[1]);
			fRec134[0] = (fRec134[1] + fRec135[1]);
			fRec133[0] = (fSlow20 * ((fSlow4 * fTemp122) - ((fSlow21 * fRec134[0]) + (fSlow22 * fRec135[0]))));
			fRec137[0] = (fRec137[1] + fRec132[1]);
			fRec136[0] = (fRec136[1] + fRec137[1]);
			fRec132[0] = (fSlow19 * (fRec133[0] - ((fSlow23 * fRec136[0]) + (fSlow24 * fRec137[0]))));
			double fTemp123 = (fRec132[0] + (fSlow2 * fTemp122));
			fRec131[0] = max((fRec131[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp123))))));
			fbargraph30 = fRec131[0];
			double fTemp124 = fTemp123;
			double fTemp125 = (0.013385302265262959 * fTemp124);
			double fTemp126 = (5.795541780573573e-18 * fTemp90);
			double fTemp127 = (0.03879429159750975 * fTemp86);
			double fTemp128 = (2.6768460856911007e-18 * fTemp17);
			double fTemp129 = (1.6929861152980278e-18 * fTemp9);
			double fTemp130 = (2.3754652515039685e-18 * fTemp29);
			double fTemp131 = (8.196133787387049e-19 * fTemp45);
			double fTemp132 = (3.0667124528564294e-18 * (fTemp37 + fTemp79));
			double fTemp133 = ((double)input17[i] * fRec3[0]);
			fRec142[0] = (fRec142[1] + fRec140[1]);
			fRec141[0] = (fRec142[1] + fRec141[1]);
			fRec140[0] = (fSlow20 * ((fSlow4 * fTemp133) - ((fSlow21 * fRec141[0]) + (fSlow22 * fRec142[0]))));
			fRec144[0] = (fRec144[1] + fRec139[1]);
			fRec143[0] = (fRec143[1] + fRec144[1]);
			fRec139[0] = (fSlow19 * (fRec140[0] - ((fSlow23 * fRec143[0]) + (fSlow24 * fRec144[0]))));
			double fTemp134 = (fRec139[0] + (fSlow2 * fTemp133));
			fRec138[0] = max((fRec138[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp134))))));
			fbargraph31 = fRec138[0];
			double fTemp135 = fTemp134;
			double fTemp136 = (0.035414181017315374 * fTemp135);
			double fTemp137 = (0.01669440503167441 * fTemp82);
			double fTemp138 = (6.5054795140920346e-18 * fTemp53);
			double fTemp139 = (fRec1[0] * ((fTemp115 + (fTemp119 + (fTemp120 + (fTemp121 + (fTemp125 + (fTemp126 + (fTemp127 + (fTemp95 + (((fTemp128 + (fTemp102 + (fTemp100 + fTemp129))) + fTemp130) + fTemp131))))))))) - (fTemp132 + (fTemp136 + (fTemp137 + ((fTemp108 + (fTemp110 + (fTemp97 + (fTemp104 + fTemp109)))) + fTemp138))))));
			fRec120[0] = max((fRec120[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp139))))));
			fbargraph32 = fRec120[0];
			output7[i] = (FAUSTFLOAT)fTemp139;
			double fTemp140 = (6.133424905712859e-18 * (fTemp82 - fTemp79));
			double fTemp141 = (5.353692171382201e-18 * fTemp114);
			double fTemp142 = (1.3010959028184069e-17 * fTemp135);
			double fTemp143 = (3.3859722305960555e-18 * fTemp74);
			double fTemp144 = (1.6392267574774099e-18 * fTemp124);
			double fTemp145 = (4.750930503007937e-18 * fTemp86);
			double fTemp146 = (fTemp100 + fTemp102);
			double fTemp147 = (1.0015840929783018e-17 * fTemp118);
			double fTemp148 = (5.353692171382203e-18 * fTemp71);
			double fTemp149 = (1.1591083561147146e-17 * fTemp90);
			double fTemp150 = (fRec1[0] * ((fTemp140 + (fTemp141 + (fTemp142 + (fTemp143 + (fTemp144 + (fTemp145 + (fTemp95 + (fTemp108 + (fTemp97 + (fTemp104 + fTemp146)))))))))) - (fTemp147 + (fTemp148 + (fTemp149 + (fTemp96 + (fTemp107 + (fTemp110 + (fTemp106 + (fTemp105 + (fTemp109 + (fTemp101 + fTemp103))))))))))));
			fRec145[0] = max((fRec145[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp150))))));
			fbargraph33 = fRec145[0];
			output8[i] = (FAUSTFLOAT)fTemp150;
			double fTemp151 = (1.5023761394674526e-17 * fTemp118);
			double fTemp152 = (8.030538257073304e-18 * fTemp71);
			double fTemp153 = (1.738662534172072e-17 * fTemp90);
			double fTemp154 = (9.20013735856929e-18 * fTemp37);
			double fTemp155 = (1.9516438542276106e-17 * fTemp53);
			double fTemp156 = (9.200137358569287e-18 * fTemp79);
			double fTemp157 = (2.4588401362161147e-18 * fTemp45);
			double fTemp158 = (7.126395754511905e-18 * fTemp29);
			double fTemp159 = (5.078958345894083e-18 * fTemp9);
			double fTemp160 = (8.0305382570733e-18 * fTemp17);
			double fTemp161 = (fRec1[0] * ((fTemp151 + (fTemp136 + (fTemp137 + (fTemp152 + (fTemp153 + (fTemp95 + ((fTemp146 + fTemp154) + fTemp155))))))) - (fTemp115 + (fTemp156 + (fTemp121 + (fTemp125 + (fTemp127 + (fTemp108 + (fTemp157 + (fTemp110 + (fTemp97 + (fTemp158 + (fTemp109 + (fTemp104 + (fTemp159 + fTemp160)))))))))))))));
			fRec146[0] = max((fRec146[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp161))))));
			fbargraph34 = fRec146[0];
			output9[i] = (FAUSTFLOAT)fTemp161;
			double fTemp162 = (0.043716214137085485 * fTemp71);
			double fTemp163 = (0.0276485614705685 * fTemp74);
			double fTemp164 = (fTemp98 + fTemp101);
			double fTemp165 = (0.025396825396825397 * fTemp41);
			double fTemp166 = (0.037859351999996446 * fTemp90);
			double fTemp167 = (0.025862861065006505 * fTemp86);
			double fTemp168 = (0.05008321509502321 * fTemp57);
			double fTemp169 = (0.02523956799999763 * fTemp13);
			double fTemp170 = (0.02586286106500651 * fTemp29);
			double fTemp171 = (fRec1[0] * (((6.133424905712858e-18 * fTemp79) + (fTemp162 + (fTemp163 + ((fTemp164 + (2.6768460856911007e-18 * fTemp21)) + fTemp165)))) - ((0.0333888100633488 * (fTemp37 - fTemp82)) + (fTemp166 + (fTemp167 + (fTemp168 + ((fTemp169 + fTemp170) + (2.3182167122294292e-18 * fTemp49))))))));
			fRec147[0] = max((fRec147[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp171))))));
			fbargraph35 = fRec147[0];
			output10[i] = (FAUSTFLOAT)fTemp171;
			double fTemp172 = (0.025862861065006505 * fTemp29);
			double fTemp173 = (0.02586286106500651 * fTemp86);
			double fTemp174 = (0.0276485614705685 * fTemp9);
			double fTemp175 = (fRec1[0] * (((1.8400274717138574e-17 * fTemp79) + ((0.033388810063348796 * fTemp82) + (fTemp121 + (fTemp166 + ((6.954650136688288e-18 * fTemp49) + (fTemp165 + ((0.03338881006334882 * fTemp37) + (fTemp98 + fTemp172)))))))) - (fTemp162 + (fTemp173 + (fTemp168 + ((8.0305382570733e-18 * fTemp21) + (fTemp169 + fTemp174)))))));
			fRec148[0] = max((fRec148[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp175))))));
			fbargraph36 = fRec148[0];
			output11[i] = (FAUSTFLOAT)fTemp175;
			double fTemp176 = (fRec1[0] * (((3.0667124528564293e-17 * fTemp79) + (fTemp162 + (fTemp167 + (fTemp165 + ((fTemp170 + (fTemp98 + (1.3384230428455503e-17 * fTemp21))) + (0.03338881006334876 * fTemp37)))))) - ((0.03338881006334885 * fTemp82) + (fTemp163 + (fTemp166 + (fTemp168 + ((fTemp101 + fTemp169) + (1.1591083561147147e-17 * fTemp49))))))));
			fRec149[0] = max((fRec149[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp176))))));
			fbargraph37 = fRec149[0];
			output12[i] = (FAUSTFLOAT)fTemp176;
			double fTemp177 = (fRec1[0] * (((4.293397433999001e-17 * fTemp79) + (fTemp166 + (fTemp173 + ((1.6227516985606004e-17 * fTemp49) + (fTemp165 + (fTemp98 + fTemp174)))))) - ((0.03338881006334882 * fTemp82) + (fTemp162 + (fTemp121 + (fTemp168 + ((0.0333888100633488 * fTemp37) + (fTemp172 + (fTemp169 + (1.8737922599837705e-17 * fTemp21))))))))));
			fRec150[0] = max((fRec150[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp177))))));
			fbargraph38 = fRec150[0];
			output13[i] = (FAUSTFLOAT)fTemp177;
			double fTemp178 = (fRec1[0] * ((fTemp95 + (fTemp108 + (fTemp106 + (fTemp105 + (fTemp109 + (fTemp104 + (fTemp102 + fTemp164))))))) - (fTemp96 + (fTemp107 + (fTemp110 + (fTemp97 + (fTemp99 + fTemp103)))))));
			fRec151[0] = max((fRec151[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp178))))));
			fbargraph39 = fRec151[0];
			output14[i] = (FAUSTFLOAT)fTemp178;
			double fTemp179 = (fRec1[0] * ((fTemp136 + (fTemp120 + (fTemp121 + (fTemp126 + (fTemp127 + (fTemp95 + (fTemp138 + (fTemp97 + (fTemp130 + (fTemp109 + (fTemp102 + (fTemp98 + fTemp129)))))))))))) - (fTemp132 + (fTemp115 + (fTemp119 + (fTemp137 + (fTemp125 + (fTemp108 + (fTemp131 + (fTemp110 + (fTemp104 + (fTemp99 + fTemp128))))))))))));
			fRec152[0] = max((fRec152[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp179))))));
			fbargraph40 = fRec152[0];
			output15[i] = (FAUSTFLOAT)fTemp179;
			double fTemp180 = (fTemp98 + fTemp102);
			double fTemp181 = (fRec1[0] * ((fTemp140 + (fTemp147 + (fTemp143 + (fTemp145 + (fTemp95 + (fTemp96 + (fTemp108 + (fTemp107 + (fTemp109 + (fTemp104 + (fTemp103 + fTemp180))))))))))) - (fTemp141 + (fTemp142 + (fTemp148 + (fTemp144 + (fTemp149 + (fTemp110 + (fTemp106 + (fTemp97 + (fTemp105 + (fTemp99 + fTemp101))))))))))));
			fRec153[0] = max((fRec153[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp181))))));
			fbargraph41 = fRec153[0];
			output16[i] = (FAUSTFLOAT)fTemp181;
			double fTemp182 = (fRec1[0] * ((fTemp115 + (fTemp137 + (fTemp152 + (fTemp125 + (fTemp153 + (fTemp95 + (fTemp157 + (fTemp154 + (fTemp97 + (fTemp109 + (fTemp160 + fTemp180))))))))))) - (fTemp151 + (fTemp136 + (fTemp156 + (fTemp121 + (fTemp127 + (fTemp155 + (fTemp108 + (fTemp110 + (fTemp158 + (fTemp104 + (fTemp99 + fTemp159)))))))))))));
			fRec154[0] = max((fRec154[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp182))))));
			fbargraph42 = fRec154[0];
			output17[i] = (FAUSTFLOAT)fTemp182;
			double fTemp183 = (fTemp1 + fTemp5);
			double fTemp184 = (0.02109375 * fTemp183);
			double fTemp185 = (0.027231914153020904 * fTemp114);
			double fTemp186 = (0.041597435974919175 * fTemp118);
			double fTemp187 = (0.02941382905783859 * fTemp135);
			double fTemp188 = (2.5471083409710855e-18 * fTemp79);
			double fTemp189 = (0.016982082124440745 * fTemp82);
			double fTemp190 = (0.027231914153020897 * fTemp71);
			double fTemp191 = (0.021093749999999994 * fTemp74);
			double fTemp192 = (0.031444705933590805 * fTemp90);
			double fTemp193 = (0.013154264250377145 * fTemp86);
			double fTemp194 = (2.5471083409710867e-18 * fTemp33);
			double fTemp195 = (1.6674738251076273e-18 * fTemp21);
			double fTemp196 = (0.02723191415302091 * fTemp17);
			double fTemp197 = (5.236595479505967e-18 * fTemp13);
			double fTemp198 = ((0.021093749999999998 * fTemp9) + fTemp197);
			double fTemp199 = (0.013154264250377147 * fTemp29);
			double fTemp200 = (1.925432923585088e-18 * fTemp49);
			double fTemp201 = (0.01111738239902945 * (fTemp45 + fTemp124));
			double fTemp202 = (0.021480823570105497 * fTemp25);
			double fTemp203 = (0.01698208212444074 * fTemp37);
			double fTemp204 = (0.024609375000000013 * fTemp41);
			double fTemp205 = (0.029413829057838584 * fTemp53);
			double fTemp206 = (0.02079871798745958 * fTemp57);
			double fTemp207 = (fRec1[0] * ((fTemp184 + (fTemp185 + (fTemp186 + (fTemp187 + (fTemp188 + (fTemp189 + (fTemp190 + (fTemp191 + (fTemp192 + (fTemp193 + ((fTemp194 + ((fTemp195 + (fTemp196 + fTemp198)) + fTemp199)) + fTemp200))))))))))) - (fTemp201 + ((((fTemp202 + fTemp203) + fTemp204) + fTemp205) + fTemp206))));
			fRec155[0] = max((fRec155[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp207))))));
			fbargraph43 = fRec155[0];
			output18[i] = (FAUSTFLOAT)fTemp207;
			double fTemp208 = (0.01111738239902945 * (fTemp45 - fTemp124));
			double fTemp209 = (0.02723191415302091 * fTemp114);
			double fTemp210 = (0.02941382905783858 * fTemp135);
			double fTemp211 = (7.641325022913257e-18 * fTemp79);
			double fTemp212 = (0.01698208212444074 * fTemp82);
			double fTemp213 = (0.021093749999999998 * fTemp74);
			double fTemp214 = (0.013154264250377147 * fTemp86);
			double fTemp215 = (0.029413829057838594 * fTemp53);
			double fTemp216 = (0.01698208212444075 * fTemp37);
			double fTemp217 = (5.776298770755264e-18 * fTemp49);
			double fTemp218 = (7.641325022913258e-18 * fTemp33);
			double fTemp219 = (0.013154264250377145 * fTemp29);
			double fTemp220 = (5.002421475322882e-18 * fTemp21);
			double fTemp221 = (0.021093749999999994 * fTemp9);
			double fTemp222 = (0.027231914153020904 * fTemp17);
			double fTemp223 = (fRec1[0] * ((fTemp208 + (fTemp184 + (fTemp209 + (fTemp210 + (fTemp211 + (fTemp212 + (fTemp213 + (fTemp214 + (fTemp215 + (fTemp197 + fTemp216)))))))))) - (fTemp186 + (fTemp190 + (fTemp192 + (fTemp206 + (fTemp217 + (fTemp204 + (fTemp218 + (fTemp219 + (fTemp202 + (fTemp220 + (fTemp221 + fTemp222)))))))))))));
			fRec156[0] = max((fRec156[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp223))))));
			fbargraph44 = fRec156[0];
			output19[i] = (FAUSTFLOAT)fTemp223;
			double fTemp224 = (1.2735541704855428e-17 * fTemp79);
			double fTemp225 = (0.01111738239902945 * fTemp124);
			double fTemp226 = (0.02941382905783855 * fTemp53);
			double fTemp227 = (9.62716461792544e-18 * fTemp49);
			double fTemp228 = (0.011117382399029452 * fTemp45);
			double fTemp229 = (0.01698208212444072 * fTemp37);
			double fTemp230 = (8.337369125538138e-18 * fTemp21);
			double fTemp231 = (1.273554170485543e-17 * fTemp33);
			double fTemp232 = (0.02941382905783863 * fTemp135);
			double fTemp233 = (0.016982082124440766 * fTemp82);
			double fTemp234 = (0.013154264250377148 * fTemp29);
			double fTemp235 = (fRec1[0] * ((fTemp186 + (fTemp224 + (fTemp190 + (fTemp225 + (fTemp192 + (fTemp226 + (fTemp227 + (fTemp228 + (fTemp229 + ((fTemp197 + fTemp230) + fTemp231)))))))))) - ((0.02109375 * (fTemp9 - fTemp183)) + (fTemp185 + (fTemp232 + (fTemp233 + (fTemp191 + (fTemp193 + (fTemp206 + (fTemp204 + (fTemp234 + (fTemp196 + fTemp202))))))))))));
			fRec157[0] = max((fRec157[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp235))))));
			fbargraph45 = fRec157[0];
			output20[i] = (FAUSTFLOAT)fTemp235;
			double fTemp236 = (fTemp1 - fTemp74);
			double fTemp237 = (1.7829758386797597e-17 * fTemp79);
			double fTemp238 = (fTemp197 + fTemp221);
			double fTemp239 = (0.011117382399029452 * fTemp124);
			double fTemp240 = (0.029413829057838594 * fTemp135);
			double fTemp241 = (0.01698208212444075 * fTemp82);
			double fTemp242 = (0.013154264250377148 * fTemp86);
			double fTemp243 = (0.01111738239902945 * fTemp45);
			double fTemp244 = (1.7829758386797603e-17 * fTemp33);
			double fTemp245 = (1.167231677575339e-17 * fTemp21);
			double fTemp246 = (1.3478030465095617e-17 * fTemp49);
			double fTemp247 = (fRec1[0] * (((0.02109375 * (fTemp5 + fTemp236)) + (fTemp237 + ((fTemp219 + (fTemp222 + fTemp238)) + fTemp239))) - (fTemp209 + (fTemp186 + (fTemp240 + (fTemp241 + (fTemp190 + (fTemp192 + (fTemp242 + (fTemp206 + (fTemp205 + ((fTemp243 + (fTemp204 + (fTemp203 + (fTemp244 + (fTemp202 + fTemp245))))) + fTemp246))))))))))));
			fRec158[0] = max((fRec158[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp247))))));
			fbargraph46 = fRec158[0];
			output21[i] = (FAUSTFLOAT)fTemp247;
			double fTemp248 = (fTemp1 - fTemp5);
			double fTemp249 = (0.02109375 * fTemp248);
			double fTemp250 = (fRec1[0] * ((fTemp249 + (fTemp201 + (fTemp188 + (fTemp189 + (fTemp190 + (fTemp191 + (fTemp192 + (fTemp193 + (fTemp205 + (fTemp200 + (fTemp199 + (fTemp202 + (fTemp198 + fTemp195))))))))))))) - (fTemp185 + (fTemp186 + (fTemp187 + (fTemp206 + (fTemp204 + (fTemp203 + (fTemp196 + fTemp194)))))))));
			fRec159[0] = max((fRec159[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp250))))));
			fbargraph47 = fRec159[0];
			output22[i] = (FAUSTFLOAT)fTemp250;
			double fTemp251 = ((((((((((((fTemp197 + fTemp222) + fTemp202) + fTemp218) + fTemp216) + fTemp214) + fTemp213) + fTemp212) + fTemp211) + fTemp186) + fTemp249) - (((((((((((fTemp221 + fTemp220) + fTemp219) + fTemp204) + fTemp217) + fTemp215) + fTemp206) + fTemp192) + fTemp190) + fTemp210) + fTemp209) + fTemp208)) * fRec1[0]);
			fRec160[0] = max((fRec160[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp251))))));
			fbargraph48 = fRec160[0];
			output23[i] = (FAUSTFLOAT)fTemp251;
			double fTemp252 = ((((((((((((fTemp197 + fTemp196) + fTemp230) + fTemp202) + fTemp229) + fTemp227) + fTemp192) + fTemp190) + fTemp224) + fTemp232) + fTemp185) - ((((((((((((0.02109375 * (fTemp9 - fTemp248)) + fTemp234) + fTemp231) + fTemp204) + fTemp228) + fTemp226) + fTemp206) + fTemp193) + fTemp225) + fTemp191) + fTemp233) + fTemp186)) * fRec1[0]);
			fRec161[0] = max((fRec161[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp252))))));
			fbargraph49 = fRec161[0];
			output24[i] = (FAUSTFLOAT)fTemp252;
			double fTemp253 = (((((((((((fTemp202 + fTemp238) + fTemp219) + fTemp244) + fTemp243) + fTemp205) + fTemp237) + fTemp240) + fTemp186) + fTemp209) - ((((((((((((0.02109375 * (fTemp5 - fTemp236)) + fTemp222) + fTemp245) + fTemp203) + fTemp204) + fTemp246) + fTemp206) + fTemp242) + fTemp192) + fTemp239) + fTemp190) + fTemp241)) * fRec1[0]);
			fRec162[0] = max((fRec162[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp253))))));
			fbargraph50 = fRec162[0];
			output25[i] = (FAUSTFLOAT)fTemp253;
			double fTemp254 = (0.016998264729033836 * fTemp118);
			double fTemp255 = (0.006277025248674522 * fTemp135);
			double fTemp256 = (0.002313170838826743 * fTemp82);
			double fTemp257 = (0.007102817506892165 * fTemp71);
			double fTemp258 = (0.03558738810271335 * fTemp124);
			double fTemp259 = (0.02907849643359095 * fTemp90);
			double fTemp260 = (0.030460165261409405 * fTemp86);
			double fTemp261 = (0.014254712301587407 * fTemp41);
			double fTemp262 = (0.030460165261409402 * fTemp29);
			double fTemp263 = (0.03280650879687195 * fTemp13);
			double fTemp264 = (0.03160554266923825 * fTemp5);
			double fTemp265 = (0.02017333553791887 * fTemp1);
			double fTemp266 = (0.010535180889746065 * (fTemp9 + fTemp74));
			double fTemp267 = (0.021308452520676528 * (fTemp17 + fTemp114));
			double fTemp268 = (0.0263335984839893 * fTemp25);
			double fTemp269 = (0.03558738810271334 * fTemp45);
			double fTemp270 = (4.758463994196736e-19 * fTemp79);
			double fTemp271 = (0.001479509039672308 * fTemp57);
			double fTemp272 = (0.0062770252486745275 * fTemp53);
			double fTemp273 = (0.002313170838826745 * fTemp37);
			double fTemp274 = (1.1422201709353029e-18 * fTemp21);
			double fTemp275 = (2.7335294515958808e-18 * fTemp33);
			double fTemp276 = (4.676178873339891e-18 * fTemp49);
			double fTemp277 = (((fTemp254 + (fTemp255 + (fTemp256 + (fTemp257 + (fTemp258 + (fTemp259 + (fTemp260 + ((fTemp261 + (fTemp262 + ((fTemp263 + (fTemp264 + (fTemp265 + (fTemp266 + fTemp267)))) + fTemp268))) + fTemp269)))))))) - (fTemp270 + (fTemp271 + (fTemp272 + ((fTemp273 + (fTemp274 + fTemp275)) + fTemp276))))) * fRec1[0]);
			fRec163[0] = max((fRec163[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp277))))));
			fbargraph51 = fRec163[0];
			output26[i] = (FAUSTFLOAT)fTemp277;
			double fTemp278 = (0.006277025248674523 * fTemp135);
			double fTemp279 = (5.435628029232799e-19 * fTemp79);
			double fTemp280 = (0.006277025248674527 * fTemp53);
			double fTemp281 = (fTemp265 + fTemp264);
			double fTemp282 = (fTemp281 + fTemp263);
			double fTemp283 = (0.0023131708388267444 * fTemp37);
			double fTemp284 = (5.341633137212233e-18 * fTemp49);
			double fTemp285 = (3.1225305737205953e-18 * fTemp33);
			double fTemp286 = (1.3047664087114909e-18 * fTemp21);
			double fTemp287 = (0.010535180889746065 * (fTemp9 - fTemp74));
			double fTemp288 = (0.021308452520676528 * (fTemp17 - fTemp114));
			double fTemp289 = (((fTemp278 + (fTemp279 + (fTemp256 + (fTemp258 + (fTemp260 + (fTemp280 + (fTemp261 + ((fTemp268 + fTemp282) + fTemp283)))))))) - (fTemp254 + (fTemp257 + (fTemp259 + (fTemp271 + (fTemp284 + (fTemp269 + (fTemp285 + (fTemp262 + (fTemp286 + (fTemp287 + fTemp288))))))))))) * fRec1[0]);
			fRec164[0] = max((fRec164[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp289))))));
			fbargraph52 = fRec164[0];
			output27[i] = (FAUSTFLOAT)fTemp289;
			double fTemp290 = (9.059380048721333e-19 * fTemp79);
			double fTemp291 = (8.902721895353721e-18 * fTemp49);
			double fTemp292 = (0.002313170838826741 * fTemp37);
			double fTemp293 = (5.204217622867659e-18 * fTemp33);
			double fTemp294 = (2.1746106811858183e-18 * fTemp21);
			double fTemp295 = (0.006277025248674517 * fTemp53);
			double fTemp296 = (0.021308452520676528 * fTemp114);
			double fTemp297 = (0.0062770252486745335 * fTemp135);
			double fTemp298 = (0.002313170838826747 * fTemp82);
			double fTemp299 = (0.010535180889746065 * fTemp74);
			double fTemp300 = (0.03558738810271334 * fTemp124);
			double fTemp301 = (0.030460165261409402 * fTemp86);
			double fTemp302 = (0.010535180889746067 * fTemp9);
			double fTemp303 = (0.02130845252067653 * fTemp17);
			double fTemp304 = (0.030460165261409412 * fTemp29);
			double fTemp305 = (0.03558738810271335 * fTemp45);
			double fTemp306 = (fRec1[0] * ((fTemp254 + (fTemp290 + (fTemp257 + (fTemp259 + ((fTemp291 + (fTemp261 + (fTemp292 + (fTemp293 + (fTemp268 + (fTemp282 + fTemp294)))))) + fTemp295))))) - (fTemp296 + (fTemp297 + (fTemp298 + (fTemp299 + (fTemp300 + (fTemp301 + (fTemp271 + (((fTemp302 + fTemp303) + fTemp304) + fTemp305))))))))));
			fRec165[0] = max((fRec165[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp306))))));
			fbargraph53 = fRec165[0];
			output28[i] = (FAUSTFLOAT)fTemp306;
			double fTemp307 = (1.2683132068209864e-18 * fTemp79);
			double fTemp308 = (0.021308452520676528 * fTemp17);
			double fTemp309 = (0.010535180889746065 * fTemp9);
			double fTemp310 = (0.02130845252067653 * fTemp114);
			double fTemp311 = (0.006277025248674527 * fTemp135);
			double fTemp312 = (0.0023131708388267444 * fTemp82);
			double fTemp313 = (0.010535180889746067 * fTemp74);
			double fTemp314 = (0.030460165261409412 * fTemp86);
			double fTemp315 = (0.006277025248674525 * fTemp53);
			double fTemp316 = (0.0023131708388267436 * fTemp37);
			double fTemp317 = (3.044454953660145e-18 * fTemp21);
			double fTemp318 = (7.285904672014723e-18 * fTemp33);
			double fTemp319 = (1.246381065349521e-17 * fTemp49);
			double fTemp320 = (fRec1[0] * ((fTemp307 + (fTemp269 + (fTemp261 + (fTemp262 + (fTemp268 + (fTemp308 + (fTemp263 + (fTemp281 + fTemp309)))))))) - (fTemp310 + (fTemp254 + (fTemp311 + (fTemp312 + (fTemp257 + (fTemp313 + (fTemp258 + (fTemp259 + (fTemp314 + (fTemp271 + (fTemp315 + ((fTemp316 + (fTemp317 + fTemp318)) + fTemp319))))))))))))));
			fRec166[0] = max((fRec166[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp320))))));
			fbargraph54 = fRec166[0];
			output29[i] = (FAUSTFLOAT)fTemp320;
			double fTemp321 = (0.010356563277706152 * fTemp57);
			double fTemp322 = (0.018831075746023607 * fTemp53);
			double fTemp323 = (0.02081853754944074 * fTemp37);
			double fTemp324 = (0.028411270027568724 * fTemp21);
			double fTemp325 = (0.010535180889746075 * fTemp5);
			double fTemp326 = (fTemp265 + fTemp325);
			double fTemp327 = (0.03160554266923824 * fTemp9);
			double fTemp328 = (0.021308452520676542 * fTemp17);
			double fTemp329 = (0.022664352972045144 * fTemp33);
			double fTemp330 = (0.004251405423280436 * fTemp41);
			double fTemp331 = (0.010535180889746094 * fTemp74);
			double fTemp332 = (0.021308452520676584 * fTemp71);
			double fTemp333 = (0.03007122090474782 * fTemp82);
			double fTemp334 = (0.035508216952135616 * fTemp79);
			double fTemp335 = (0.027200442744256398 * fTemp135);
			double fTemp336 = (0.016998264729033888 * fTemp118);
			double fTemp337 = (0.007102817506892191 * fTemp114);
			double fTemp338 = (0.01612596984427559 * fTemp29);
			double fTemp339 = (0.01640325439843596 * fTemp13);
			double fTemp340 = (0.020481687709769415 * fTemp25);
			double fTemp341 = (fTemp339 + fTemp340);
			double fTemp342 = (0.03084240302235156 * fTemp45);
			double fTemp343 = (0.008947229671874163 * fTemp49);
			double fTemp344 = (0.005375323281425205 * fTemp86);
			double fTemp345 = (0.006710422253905637 * fTemp90);
			double fTemp346 = (0.010280801007450534 * fTemp124);
			double fTemp347 = ((((((((((fTemp321 + (fTemp322 + ((fTemp323 + ((fTemp324 + ((fTemp326 + fTemp327) + fTemp328)) + fTemp329)) + fTemp330))) + fTemp331) + fTemp332) + fTemp333) + fTemp334) + fTemp335) + fTemp336) + fTemp337) - ((((((fTemp338 + fTemp341) + fTemp342) + fTemp343) + fTemp344) + fTemp345) + fTemp346)) * fRec1[0]);
			fRec167[0] = max((fRec167[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp347))))));
			fbargraph55 = fRec167[0];
			output30[i] = (FAUSTFLOAT)fTemp347;
			double fTemp348 = (0.021308452520676546 * fTemp114);
			double fTemp349 = (0.016998264729033867 * fTemp118);
			double fTemp350 = (0.021308452520676556 * fTemp71);
			double fTemp351 = (0.03160554266923824 * fTemp74);
			double fTemp352 = (0.010356563277706217 * fTemp57);
			double fTemp353 = (0.00894722967187417 * fTemp49);
			double fTemp354 = (0.01053518088974608 * fTemp9);
			double fTemp355 = (0.007102817506892183 * fTemp17);
			double fTemp356 = (0.01883107574602364 * fTemp135);
			double fTemp357 = (0.035508216952135595 * fTemp79);
			double fTemp358 = (0.020818537549440783 * fTemp82);
			double fTemp359 = (0.030842403022351562 * fTemp124);
			double fTemp360 = (0.006710422253905627 * fTemp90);
			double fTemp361 = (0.01612596984427559 * fTemp86);
			double fTemp362 = (0.027200442744256374 * fTemp53);
			double fTemp363 = (0.030071220904747797 * fTemp37);
			double fTemp364 = (0.02841127002756874 * fTemp21);
			double fTemp365 = (0.005375323281425198 * fTemp29);
			double fTemp366 = (0.022664352972045162 * fTemp33);
			double fTemp367 = (0.010280801007450522 * fTemp45);
			double fTemp368 = (((fTemp348 + (fTemp349 + (fTemp350 + (fTemp351 + (fTemp352 + (fTemp353 + (((fTemp354 + fTemp326) + fTemp355) + fTemp330))))))) - (fTemp356 + (fTemp357 + (fTemp358 + (fTemp359 + (fTemp360 + (fTemp361 + (fTemp362 + ((fTemp363 + ((((fTemp364 + fTemp339) + fTemp340) + fTemp365) + fTemp366)) + fTemp367))))))))) * fRec1[0]);
			fRec168[0] = max((fRec168[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp368))))));
			fbargraph56 = fRec168[0];
			output31[i] = (FAUSTFLOAT)fTemp368;
			double fTemp369 = (0.006710422253905626 * fTemp90);
			double fTemp370 = (0.010356563277706236 * fTemp57);
			double fTemp371 = (0.008947229671874172 * fTemp49);
			double fTemp372 = (0.010280801007450517 * fTemp45);
			double fTemp373 = (0.03007122090474779 * fTemp37);
			double fTemp374 = (0.005375323281425197 * fTemp29);
			double fTemp375 = (0.027200442744256363 * fTemp53);
			double fTemp376 = (0.016998264729033864 * fTemp118);
			double fTemp377 = (0.01883107574602365 * fTemp135);
			double fTemp378 = (0.02081853754944079 * fTemp82);
			double fTemp379 = (0.03084240302235157 * fTemp124);
			double fTemp380 = (0.016125969844275594 * fTemp86);
			double fTemp381 = (0.007102817506892181 * fTemp17);
			double fTemp382 = (0.010535180889746079 * fTemp9);
			double fTemp383 = (0.028411270027568745 * fTemp21);
			double fTemp384 = (((fTemp357 + (fTemp351 + (fTemp369 + (fTemp370 + ((fTemp371 + (fTemp372 + ((fTemp373 + (fTemp374 + fTemp326)) + fTemp330))) + fTemp375))))) - (fTemp376 + (fTemp377 + (fTemp378 + (fTemp379 + (fTemp380 + (fTemp366 + (((fTemp381 + ((fTemp382 + (0.021308452520676553 * (fTemp71 - fTemp114))) + fTemp339)) + fTemp383) + fTemp340)))))))) * fRec1[0]);
			fRec169[0] = max((fRec169[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp384))))));
			fbargraph57 = fRec169[0];
			output32[i] = (FAUSTFLOAT)fTemp384;
			double fTemp385 = (0.027200442744256408 * fTemp135);
			double fTemp386 = (0.010356563277706144 * fTemp57);
			double fTemp387 = (0.028411270027568717 * fTemp21);
			double fTemp388 = (0.022664352972045138 * fTemp33);
			double fTemp389 = (0.010535180889746096 * fTemp74);
			double fTemp390 = (0.03007122090474784 * fTemp82);
			double fTemp391 = (0.007102817506892193 * fTemp114);
			double fTemp392 = (0.010280801007450538 * fTemp124);
			double fTemp393 = (fTemp327 + fTemp339);
			double fTemp394 = (0.020818537549440724 * fTemp37);
			double fTemp395 = (0.018831075746023586 * fTemp53);
			double fTemp396 = (0.005375323281425206 * fTemp86);
			double fTemp397 = ((((fTemp385 + ((((fTemp386 + (((((fTemp387 + fTemp326) + fTemp338) + fTemp388) + fTemp330) + fTemp342)) + fTemp345) + fTemp389) + fTemp390)) + fTemp391) - ((((fTemp392 + ((((((fTemp393 + fTemp328) + fTemp340) + fTemp394) + fTemp343) + fTemp395) + fTemp396)) + fTemp332) + fTemp334) + fTemp336)) * fRec1[0]);
			fRec170[0] = max((fRec170[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp397))))));
			fbargraph58 = fRec170[0];
			output33[i] = (FAUSTFLOAT)fTemp397;
			double fTemp398 = (0.01699826472903388 * fTemp118);
			double fTemp399 = (0.03550821695213561 * fTemp79);
			double fTemp400 = (0.02130845252067657 * fTemp71);
			double fTemp401 = (0.010280801007450529 * fTemp124);
			double fTemp402 = (0.005375323281425202 * fTemp86);
			double fTemp403 = (0.030842403022351562 * fTemp45);
			double fTemp404 = (0.02266435297204515 * fTemp33);
			double fTemp405 = (0.02841127002756873 * fTemp21);
			double fTemp406 = (0.010356563277706178 * fTemp57);
			double fTemp407 = (0.007102817506892187 * fTemp114);
			double fTemp408 = (0.01053518088974609 * fTemp74);
			double fTemp409 = (0.0067104222539056315 * fTemp90);
			double fTemp410 = (0.01883107574602361 * fTemp53);
			double fTemp411 = (0.021308452520676546 * fTemp17);
			double fTemp412 = (0.020818537549440744 * fTemp37);
			double fTemp413 = (0.008947229671874167 * fTemp49);
			double fTemp414 = (((fTemp398 + (fTemp399 + (fTemp400 + (fTemp401 + (fTemp402 + ((fTemp403 + ((fTemp404 + ((fTemp405 + fTemp326) + fTemp338)) + fTemp330)) + fTemp406)))))) - (fTemp407 + (((fTemp408 + (fTemp409 + (fTemp410 + ((((fTemp411 + fTemp393) + fTemp340) + fTemp412) + fTemp413)))) + fTemp333) + fTemp335))) * fRec1[0]);
			fRec171[0] = max((fRec171[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp414))))));
			fbargraph59 = fRec171[0];
			output34[i] = (FAUSTFLOAT)fTemp414;
			double fTemp415 = (0.016998264729033874 * fTemp118);
			double fTemp416 = (0.01883107574602363 * fTemp135);
			double fTemp417 = (0.020818537549440772 * fTemp82);
			double fTemp418 = (0.021308452520676563 * fTemp71);
			double fTemp419 = (0.027200442744256377 * fTemp53);
			double fTemp420 = (0.010280801007450525 * fTemp45);
			double fTemp421 = (0.0300712209047478 * fTemp37);
			double fTemp422 = (0.0053753232814252 * fTemp29);
			double fTemp423 = (0.0103565632777062 * fTemp57);
			double fTemp424 = (0.00671042225390563 * fTemp90);
			double fTemp425 = (0.022664352972045155 * fTemp33);
			double fTemp426 = (0.028411270027568734 * fTemp21);
			double fTemp427 = (0.007102817506892185 * fTemp17);
			double fTemp428 = (0.010535180889746086 * fTemp9);
			double fTemp429 = (((fTemp415 + (fTemp416 + (fTemp417 + (fTemp418 + ((((fTemp419 + ((fTemp420 + ((fTemp421 + (fTemp422 + fTemp326)) + fTemp330)) + fTemp353)) + fTemp423) + fTemp361) + fTemp359))))) - ((fTemp399 + ((fTemp424 + (fTemp425 + ((fTemp426 + (fTemp427 + (fTemp428 + fTemp339))) + fTemp340))) + fTemp351)) + fTemp348)) * fRec1[0]);
			fRec172[0] = max((fRec172[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp429))))));
			fbargraph60 = fRec172[0];
			output35[i] = (FAUSTFLOAT)fTemp429;
			double fTemp430 = (0.018831075746023662 * fTemp135);
			double fTemp431 = (0.03550821695213559 * fTemp79);
			double fTemp432 = (0.020818537549440803 * fTemp82);
			double fTemp433 = (0.006710422253905624 * fTemp90);
			double fTemp434 = (0.008947229671874174 * fTemp49);
			double fTemp435 = (0.010535180889746074 * fTemp9);
			double fTemp436 = (0.0071028175068921775 * fTemp17);
			double fTemp437 = (0.010356563277706251 * fTemp57);
			double fTemp438 = (0.016998264729033857 * fTemp118);
			double fTemp439 = (0.021308452520676542 * fTemp71);
			double fTemp440 = (0.027200442744256356 * fTemp53);
			double fTemp441 = (0.03007122090474778 * fTemp37);
			double fTemp442 = (0.022664352972045165 * fTemp33);
			double fTemp443 = (0.028411270027568748 * fTemp21);
			double fTemp444 = (0.005375323281425194 * fTemp29);
			double fTemp445 = (0.010280801007450515 * fTemp45);
			double fTemp446 = (0.021308452520676553 * fTemp114);
			double fTemp447 = (((fTemp430 + (fTemp431 + (fTemp432 + ((fTemp433 + (((fTemp434 + (((fTemp435 + fTemp326) + fTemp436) + fTemp330)) + fTemp437) + fTemp380)) + fTemp379)))) - ((fTemp438 + (fTemp439 + ((fTemp440 + ((fTemp441 + (fTemp442 + (((fTemp443 + fTemp339) + fTemp340) + fTemp444))) + fTemp445)) + fTemp351))) + fTemp446)) * fRec1[0]);
			fRec173[0] = max((fRec173[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp447))))));
			fbargraph61 = fRec173[0];
			output36[i] = (FAUSTFLOAT)fTemp447;
			double fTemp448 = (0.010280801007450555 * fTemp124);
			double fTemp449 = (0.006710422253905646 * fTemp90);
			double fTemp450 = (0.005375323281425214 * fTemp86);
			double fTemp451 = (0.010356563277706062 * fTemp57);
			double fTemp452 = (0.02081853754944071 * fTemp37);
			double fTemp453 = (0.028411270027568696 * fTemp21);
			double fTemp454 = (0.03160554266923823 * fTemp9);
			double fTemp455 = (0.02130845252067654 * fTemp17);
			double fTemp456 = (0.02266435297204512 * fTemp33);
			double fTemp457 = (0.018831075746023575 * fTemp53);
			double fTemp458 = (0.007102817506892205 * fTemp114);
			double fTemp459 = (0.016998264729033916 * fTemp118);
			double fTemp460 = (0.027200442744256415 * fTemp135);
			double fTemp461 = (0.03550821695213564 * fTemp79);
			double fTemp462 = (0.030071220904747842 * fTemp82);
			double fTemp463 = (0.021308452520676615 * fTemp71);
			double fTemp464 = (0.010535180889746113 * fTemp74);
			double fTemp465 = (0.008947229671874156 * fTemp49);
			double fTemp466 = (0.016125969844275587 * fTemp29);
			double fTemp467 = (0.030842403022351552 * fTemp45);
			double fTemp468 = (((fTemp448 + (fTemp449 + (fTemp450 + (fTemp451 + (((fTemp452 + ((fTemp453 + ((fTemp454 + fTemp326) + fTemp455)) + fTemp456)) + fTemp330) + fTemp457))))) - (fTemp458 + (fTemp459 + (fTemp460 + (fTemp461 + (fTemp462 + (fTemp463 + (fTemp464 + (fTemp465 + ((fTemp466 + fTemp341) + fTemp467)))))))))) * fRec1[0]);
			fRec174[0] = max((fRec174[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp468))))));
			fbargraph62 = fRec174[0];
			output37[i] = (FAUSTFLOAT)fTemp468;
			double fTemp469 = (fTemp325 + fTemp339);
			double fTemp470 = ((((((((((((((fTemp265 + fTemp327) + fTemp324) + fTemp340) + fTemp323) + fTemp330) + fTemp342) + fTemp321) + fTemp346) + fTemp331) + fTemp332) + fTemp333) + fTemp334) - ((((((((((fTemp469 + fTemp328) + fTemp338) + fTemp329) + fTemp343) + fTemp322) + fTemp344) + fTemp345) + fTemp335) + fTemp336) + fTemp337)) * fRec1[0]);
			fRec175[0] = max((fRec175[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp470))))));
			fbargraph63 = fRec175[0];
			output38[i] = (FAUSTFLOAT)fTemp470;
			double fTemp471 = (fRec1[0] * ((fTemp356 + (fTemp350 + (fTemp351 + (fTemp359 + (fTemp352 + (((((((fTemp265 + fTemp354) + fTemp340) + fTemp366) + fTemp330) + fTemp367) + fTemp353) + fTemp362)))))) - ((((((((((fTemp469 + fTemp355) + fTemp364) + fTemp365) + fTemp363) + fTemp361) + fTemp360) + fTemp358) + fTemp357) + fTemp349) + fTemp348)));
			fRec176[0] = max((fRec176[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp471))))));
			fbargraph64 = fRec176[0];
			output39[i] = (FAUSTFLOAT)fTemp471;
			double fTemp472 = (fRec1[0] * ((fTemp376 + (fTemp377 + (fTemp357 + (fTemp351 + (fTemp379 + (fTemp369 + (fTemp370 + (fTemp371 + (fTemp330 + (fTemp373 + (fTemp366 + (fTemp374 + (fTemp340 + (fTemp265 + fTemp381)))))))))))))) - ((0.021308452520676553 * (fTemp71 + fTemp114)) + (fTemp378 + (fTemp380 + (fTemp375 + (fTemp372 + (fTemp383 + (fTemp339 + (fTemp382 + fTemp325))))))))));
			fRec177[0] = max((fRec177[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp472))))));
			fbargraph65 = fRec177[0];
			output40[i] = (FAUSTFLOAT)fTemp472;
			double fTemp473 = (fTemp339 + (fTemp325 + fTemp327));
			double fTemp474 = (fRec1[0] * ((fTemp336 + (fTemp390 + (fTemp389 + (fTemp392 + (fTemp345 + (fTemp386 + (fTemp395 + (fTemp330 + (fTemp338 + (fTemp340 + (fTemp387 + (fTemp265 + fTemp328)))))))))))) - (fTemp391 + (fTemp385 + (fTemp334 + (fTemp332 + (fTemp396 + (fTemp343 + (fTemp342 + (fTemp394 + (fTemp388 + fTemp473)))))))))));
			fRec178[0] = max((fRec178[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp474))))));
			fbargraph66 = fRec178[0];
			output41[i] = (FAUSTFLOAT)fTemp474;
			double fTemp475 = (fRec1[0] * ((fTemp407 + (fTemp335 + (fTemp399 + (fTemp400 + (fTemp402 + (fTemp406 + (fTemp410 + (fTemp330 + (fTemp338 + (fTemp340 + (fTemp405 + (fTemp265 + fTemp411)))))))))))) - (fTemp398 + (fTemp333 + (fTemp408 + (fTemp401 + (fTemp409 + (fTemp413 + (fTemp403 + (fTemp412 + (fTemp404 + fTemp473)))))))))));
			fRec179[0] = max((fRec179[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp475))))));
			fbargraph67 = fRec179[0];
			output42[i] = (FAUSTFLOAT)fTemp475;
			double fTemp476 = (fRec1[0] * ((fTemp348 + (fTemp417 + (fTemp418 + (fTemp361 + (fTemp423 + (fTemp353 + (fTemp330 + (fTemp421 + (fTemp425 + (fTemp422 + (fTemp340 + (fTemp265 + fTemp427)))))))))))) - (fTemp415 + (fTemp416 + (fTemp399 + (fTemp351 + (fTemp359 + (fTemp424 + (fTemp419 + (fTemp420 + (fTemp426 + (fTemp339 + (fTemp428 + fTemp325)))))))))))));
			fRec180[0] = max((fRec180[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp476))))));
			fbargraph68 = fRec180[0];
			output43[i] = (FAUSTFLOAT)fTemp476;
			double fTemp477 = (fRec1[0] * ((fTemp446 + (fTemp438 + (fTemp431 + (fTemp432 + (fTemp433 + (fTemp380 + (fTemp437 + (fTemp440 + (fTemp434 + (fTemp445 + (fTemp330 + (fTemp442 + (fTemp340 + (fTemp265 + fTemp435)))))))))))))) - (fTemp430 + (fTemp439 + (fTemp351 + (fTemp379 + (fTemp441 + (fTemp444 + (fTemp443 + (fTemp436 + fTemp469))))))))));
			fRec181[0] = max((fRec181[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp477))))));
			fbargraph69 = fRec181[0];
			output44[i] = (FAUSTFLOAT)fTemp477;
			double fTemp478 = (fRec1[0] * ((fTemp458 + (fTemp459 + (fTemp460 + (fTemp449 + (fTemp450 + (fTemp451 + (fTemp467 + (fTemp330 + (fTemp452 + (fTemp340 + (fTemp453 + (fTemp265 + fTemp454)))))))))))) - (fTemp461 + (fTemp462 + (fTemp463 + (fTemp464 + (fTemp448 + (fTemp457 + (fTemp465 + (fTemp456 + (fTemp466 + (fTemp455 + fTemp469))))))))))));
			fRec182[0] = max((fRec182[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp478))))));
			fbargraph70 = fRec182[0];
			output45[i] = (FAUSTFLOAT)fTemp478;
			double fTemp479 = (fRec1[0] * ((fTemp256 + (fTemp257 + (fTemp259 + (fTemp260 + (fTemp272 + (fTemp261 + (fTemp275 + (fTemp262 + ((fTemp265 + fTemp266) + fTemp263))))))))) - (fTemp254 + (fTemp255 + (fTemp270 + (fTemp258 + (fTemp271 + (fTemp276 + (fTemp269 + (fTemp273 + (fTemp268 + (fTemp274 + (fTemp264 + fTemp267)))))))))))));
			fRec183[0] = max((fRec183[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp479))))));
			fbargraph71 = fRec183[0];
			output46[i] = (FAUSTFLOAT)fTemp479;
			double fTemp480 = (fRec1[0] * ((fTemp254 + (fTemp279 + (fTemp256 + (fTemp260 + (fTemp269 + (fTemp261 + (fTemp283 + (fTemp285 + (fTemp263 + (fTemp265 + fTemp288)))))))))) - (fTemp278 + (fTemp257 + (fTemp258 + (fTemp259 + (fTemp271 + (fTemp280 + (fTemp284 + (fTemp262 + (fTemp268 + (fTemp286 + (fTemp264 + fTemp287)))))))))))));
			fRec184[0] = max((fRec184[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp480))))));
			fbargraph72 = fRec184[0];
			output47[i] = (FAUSTFLOAT)fTemp480;
			double fTemp481 = (fRec1[0] * ((fTemp296 + (fTemp297 + (fTemp290 + (fTemp257 + (fTemp300 + (fTemp259 + (fTemp291 + (fTemp305 + (fTemp261 + (fTemp292 + (fTemp294 + (fTemp303 + (fTemp265 + fTemp263))))))))))))) - (fTemp254 + (fTemp298 + (fTemp299 + (fTemp301 + (fTemp271 + (fTemp295 + (fTemp293 + (fTemp304 + (fTemp268 + (fTemp264 + fTemp302))))))))))));
			fRec185[0] = max((fRec185[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp481))))));
			fbargraph73 = fRec185[0];
			output48[i] = (FAUSTFLOAT)fTemp481;
			double fTemp482 = (fRec1[0] * ((fTemp310 + (fTemp254 + (fTemp311 + (fTemp307 + (fTemp258 + (fTemp315 + (fTemp261 + (fTemp318 + (fTemp262 + (fTemp263 + (fTemp265 + fTemp309))))))))))) - (fTemp312 + (fTemp257 + (fTemp313 + (fTemp259 + (fTemp314 + (fTemp271 + (fTemp319 + (fTemp269 + (fTemp316 + (fTemp268 + (fTemp317 + (fTemp264 + fTemp308))))))))))))));
			fRec186[0] = max((fRec186[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp482))))));
			fbargraph74 = fRec186[0];
			output49[i] = (FAUSTFLOAT)fTemp482;
			// post processing
			fRec186[1] = fRec186[0];
			fRec185[1] = fRec185[0];
			fRec184[1] = fRec184[0];
			fRec183[1] = fRec183[0];
			fRec182[1] = fRec182[0];
			fRec181[1] = fRec181[0];
			fRec180[1] = fRec180[0];
			fRec179[1] = fRec179[0];
			fRec178[1] = fRec178[0];
			fRec177[1] = fRec177[0];
			fRec176[1] = fRec176[0];
			fRec175[1] = fRec175[0];
			fRec174[1] = fRec174[0];
			fRec173[1] = fRec173[0];
			fRec172[1] = fRec172[0];
			fRec171[1] = fRec171[0];
			fRec170[1] = fRec170[0];
			fRec169[1] = fRec169[0];
			fRec168[1] = fRec168[0];
			fRec167[1] = fRec167[0];
			fRec166[1] = fRec166[0];
			fRec165[1] = fRec165[0];
			fRec164[1] = fRec164[0];
			fRec163[1] = fRec163[0];
			fRec162[1] = fRec162[0];
			fRec161[1] = fRec161[0];
			fRec160[1] = fRec160[0];
			fRec159[1] = fRec159[0];
			fRec158[1] = fRec158[0];
			fRec157[1] = fRec157[0];
			fRec156[1] = fRec156[0];
			fRec155[1] = fRec155[0];
			fRec154[1] = fRec154[0];
			fRec153[1] = fRec153[0];
			fRec152[1] = fRec152[0];
			fRec151[1] = fRec151[0];
			fRec150[1] = fRec150[0];
			fRec149[1] = fRec149[0];
			fRec148[1] = fRec148[0];
			fRec147[1] = fRec147[0];
			fRec146[1] = fRec146[0];
			fRec145[1] = fRec145[0];
			fRec120[1] = fRec120[0];
			fRec138[1] = fRec138[0];
			fRec139[1] = fRec139[0];
			fRec143[1] = fRec143[0];
			fRec144[1] = fRec144[0];
			fRec140[1] = fRec140[0];
			fRec141[1] = fRec141[0];
			fRec142[1] = fRec142[0];
			fRec131[1] = fRec131[0];
			fRec132[1] = fRec132[0];
			fRec136[1] = fRec136[0];
			fRec137[1] = fRec137[0];
			fRec133[1] = fRec133[0];
			fRec134[1] = fRec134[0];
			fRec135[1] = fRec135[0];
			fRec125[1] = fRec125[0];
			fRec126[1] = fRec126[0];
			fRec130[1] = fRec130[0];
			fRec127[1] = fRec127[0];
			fRec128[1] = fRec128[0];
			fRec129[1] = fRec129[0];
			fRec121[1] = fRec121[0];
			fRec122[1] = fRec122[0];
			fRec123[1] = fRec123[0];
			fRec124[1] = fRec124[0];
			fRec119[1] = fRec119[0];
			fRec118[1] = fRec118[0];
			fRec117[1] = fRec117[0];
			fRec116[1] = fRec116[0];
			fRec82[1] = fRec82[0];
			fRec109[1] = fRec109[0];
			fRec110[1] = fRec110[0];
			fRec114[1] = fRec114[0];
			fRec115[1] = fRec115[0];
			fRec111[1] = fRec111[0];
			fRec112[1] = fRec112[0];
			fRec113[1] = fRec113[0];
			fRec103[1] = fRec103[0];
			fRec104[1] = fRec104[0];
			fRec108[1] = fRec108[0];
			fRec105[1] = fRec105[0];
			fRec106[1] = fRec106[0];
			fRec107[1] = fRec107[0];
			fRec97[1] = fRec97[0];
			fRec98[1] = fRec98[0];
			fRec102[1] = fRec102[0];
			fRec99[1] = fRec99[0];
			fRec100[1] = fRec100[0];
			fRec101[1] = fRec101[0];
			fRec90[1] = fRec90[0];
			fRec91[1] = fRec91[0];
			fRec95[1] = fRec95[0];
			fRec96[1] = fRec96[0];
			fRec92[1] = fRec92[0];
			fRec93[1] = fRec93[0];
			fRec94[1] = fRec94[0];
			fRec87[1] = fRec87[0];
			fRec88[1] = fRec88[0];
			fRec89[1] = fRec89[0];
			fRec83[1] = fRec83[0];
			fRec84[1] = fRec84[0];
			fRec85[1] = fRec85[0];
			fRec86[1] = fRec86[0];
			fRec81[1] = fRec81[0];
			fRec0[1] = fRec0[0];
			fRec74[1] = fRec74[0];
			fRec75[1] = fRec75[0];
			fRec79[1] = fRec79[0];
			fRec80[1] = fRec80[0];
			fRec76[1] = fRec76[0];
			fRec77[1] = fRec77[0];
			fRec78[1] = fRec78[0];
			fRec67[1] = fRec67[0];
			fRec68[1] = fRec68[0];
			fRec72[1] = fRec72[0];
			fRec73[1] = fRec73[0];
			fRec69[1] = fRec69[0];
			fRec70[1] = fRec70[0];
			fRec71[1] = fRec71[0];
			fRec60[1] = fRec60[0];
			fRec61[1] = fRec61[0];
			fRec65[1] = fRec65[0];
			fRec66[1] = fRec66[0];
			fRec62[1] = fRec62[0];
			fRec63[1] = fRec63[0];
			fRec64[1] = fRec64[0];
			fRec53[1] = fRec53[0];
			fRec54[1] = fRec54[0];
			fRec58[1] = fRec58[0];
			fRec59[1] = fRec59[0];
			fRec55[1] = fRec55[0];
			fRec56[1] = fRec56[0];
			fRec57[1] = fRec57[0];
			fRec46[1] = fRec46[0];
			fRec47[1] = fRec47[0];
			fRec51[1] = fRec51[0];
			fRec52[1] = fRec52[0];
			fRec48[1] = fRec48[0];
			fRec49[1] = fRec49[0];
			fRec50[1] = fRec50[0];
			fRec40[1] = fRec40[0];
			fRec41[1] = fRec41[0];
			fRec45[1] = fRec45[0];
			fRec42[1] = fRec42[0];
			fRec43[1] = fRec43[0];
			fRec44[1] = fRec44[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			fRec39[1] = fRec39[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec38[1] = fRec38[0];
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec33[1] = fRec33[0];
			fRec30[1] = fRec30[0];
			fRec31[1] = fRec31[0];
			fRec32[1] = fRec32[0];
			fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			fRec27[1] = fRec27[0];
			fRec24[1] = fRec24[0];
			fRec25[1] = fRec25[0];
			fRec26[1] = fRec26[0];
			fRec18[1] = fRec18[0];
			fRec19[1] = fRec19[0];
			fRec20[1] = fRec20[0];
			fRec21[1] = fRec21[0];
			fRec14[1] = fRec14[0];
			fRec15[1] = fRec15[0];
			fRec16[1] = fRec16[0];
			fRec17[1] = fRec17[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			fRec12[1] = fRec12[0];
			fRec13[1] = fRec13[0];
			fRec7[1] = fRec7[0];
			fRec8[1] = fRec8[0];
			fRec9[1] = fRec9[0];
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec6[1] = fRec6[0];
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

// EOF
