//----------------------------------------------------------
// name: "HOADecLebedev505"
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
	double 	fConst14;
	double 	fConst15;
	double 	fConst16;
	double 	fRec86[2];
	double 	fRec85[2];
	double 	fConst17;
	double 	fRec84[2];
	double 	fRec88[2];
	double 	fRec87[2];
	double 	fConst18;
	double 	fRec83[2];
	double 	fConst19;
	double 	fRec89[2];
	double 	fRec82[2];
	double 	fRec81[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec95[2];
	double 	fRec94[2];
	double 	fRec93[2];
	double 	fRec97[2];
	double 	fRec96[2];
	double 	fRec92[2];
	double 	fRec98[2];
	double 	fRec91[2];
	double 	fRec90[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec104[2];
	double 	fRec103[2];
	double 	fRec102[2];
	double 	fRec106[2];
	double 	fRec105[2];
	double 	fRec101[2];
	double 	fRec107[2];
	double 	fRec100[2];
	double 	fRec99[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec113[2];
	double 	fRec112[2];
	double 	fRec111[2];
	double 	fRec115[2];
	double 	fRec114[2];
	double 	fRec110[2];
	double 	fRec116[2];
	double 	fRec109[2];
	double 	fRec108[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec122[2];
	double 	fRec121[2];
	double 	fRec120[2];
	double 	fRec124[2];
	double 	fRec123[2];
	double 	fRec119[2];
	double 	fRec125[2];
	double 	fRec118[2];
	double 	fRec117[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec131[2];
	double 	fRec130[2];
	double 	fRec129[2];
	double 	fRec133[2];
	double 	fRec132[2];
	double 	fRec128[2];
	double 	fRec134[2];
	double 	fRec127[2];
	double 	fRec126[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec135[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec142[2];
	double 	fRec141[2];
	double 	fRec140[2];
	double 	fRec144[2];
	double 	fRec143[2];
	double 	fRec139[2];
	double 	fRec145[2];
	double 	fRec138[2];
	double 	fRec137[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec151[2];
	double 	fRec150[2];
	double 	fRec149[2];
	double 	fRec153[2];
	double 	fRec152[2];
	double 	fRec148[2];
	double 	fRec154[2];
	double 	fRec147[2];
	double 	fRec146[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec158[2];
	double 	fRec157[2];
	double 	fRec156[2];
	double 	fRec155[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec161[2];
	double 	fRec160[2];
	double 	fRec159[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec167[2];
	double 	fRec166[2];
	double 	fRec165[2];
	double 	fRec169[2];
	double 	fRec168[2];
	double 	fRec164[2];
	double 	fRec170[2];
	double 	fRec163[2];
	double 	fRec162[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec175[2];
	double 	fRec174[2];
	double 	fRec173[2];
	double 	fRec177[2];
	double 	fRec176[2];
	double 	fRec172[2];
	double 	fRec171[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec182[2];
	double 	fRec181[2];
	double 	fRec180[2];
	double 	fRec183[2];
	double 	fRec179[2];
	double 	fRec178[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec188[2];
	double 	fRec187[2];
	double 	fRec186[2];
	double 	fRec189[2];
	double 	fRec185[2];
	double 	fRec184[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec194[2];
	double 	fRec193[2];
	double 	fRec192[2];
	double 	fRec196[2];
	double 	fRec195[2];
	double 	fRec191[2];
	double 	fRec190[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec136[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec197[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec198[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec199[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec200[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec205[2];
	double 	fRec204[2];
	double 	fRec203[2];
	double 	fRec202[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec210[2];
	double 	fRec209[2];
	double 	fRec208[2];
	double 	fRec211[2];
	double 	fRec207[2];
	double 	fRec206[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec217[2];
	double 	fRec216[2];
	double 	fRec215[2];
	double 	fRec219[2];
	double 	fRec218[2];
	double 	fRec214[2];
	double 	fRec220[2];
	double 	fRec213[2];
	double 	fRec212[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec225[2];
	double 	fRec224[2];
	double 	fRec223[2];
	double 	fRec227[2];
	double 	fRec226[2];
	double 	fRec222[2];
	double 	fRec221[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec233[2];
	double 	fRec232[2];
	double 	fRec231[2];
	double 	fRec235[2];
	double 	fRec234[2];
	double 	fRec230[2];
	double 	fRec236[2];
	double 	fRec229[2];
	double 	fRec228[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec241[2];
	double 	fRec240[2];
	double 	fRec239[2];
	double 	fRec243[2];
	double 	fRec242[2];
	double 	fRec238[2];
	double 	fRec237[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec201[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec244[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec245[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec246[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec247[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec248[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec249[2];
	FAUSTFLOAT 	fbargraph49;
	double 	fRec250[2];
	FAUSTFLOAT 	fbargraph50;
	double 	fRec251[2];
	FAUSTFLOAT 	fbargraph51;
	double 	fRec252[2];
	FAUSTFLOAT 	fbargraph52;
	double 	fRec253[2];
	FAUSTFLOAT 	fbargraph53;
	double 	fRec254[2];
	FAUSTFLOAT 	fbargraph54;
	double 	fRec255[2];
	FAUSTFLOAT 	fbargraph55;
	double 	fRec256[2];
	FAUSTFLOAT 	fbargraph56;
	double 	fRec257[2];
	FAUSTFLOAT 	fbargraph57;
	double 	fRec258[2];
	FAUSTFLOAT 	fbargraph58;
	double 	fRec259[2];
	FAUSTFLOAT 	fbargraph59;
	double 	fRec260[2];
	FAUSTFLOAT 	fbargraph60;
	double 	fRec261[2];
	FAUSTFLOAT 	fbargraph61;
	double 	fRec262[2];
	FAUSTFLOAT 	fbargraph62;
	double 	fRec263[2];
	FAUSTFLOAT 	fbargraph63;
	double 	fRec264[2];
	FAUSTFLOAT 	fbargraph64;
	double 	fRec265[2];
	FAUSTFLOAT 	fbargraph65;
	double 	fRec266[2];
	FAUSTFLOAT 	fbargraph66;
	double 	fRec267[2];
	FAUSTFLOAT 	fbargraph67;
	double 	fRec268[2];
	FAUSTFLOAT 	fbargraph68;
	double 	fRec269[2];
	FAUSTFLOAT 	fbargraph69;
	double 	fRec270[2];
	FAUSTFLOAT 	fbargraph70;
	double 	fRec271[2];
	FAUSTFLOAT 	fbargraph71;
	double 	fRec272[2];
	FAUSTFLOAT 	fbargraph72;
	double 	fRec273[2];
	FAUSTFLOAT 	fbargraph73;
	double 	fRec274[2];
	FAUSTFLOAT 	fbargraph74;
	double 	fRec275[2];
	FAUSTFLOAT 	fbargraph75;
	double 	fRec276[2];
	FAUSTFLOAT 	fbargraph76;
	double 	fRec277[2];
	FAUSTFLOAT 	fbargraph77;
	double 	fRec278[2];
	FAUSTFLOAT 	fbargraph78;
	double 	fRec279[2];
	FAUSTFLOAT 	fbargraph79;
	double 	fRec280[2];
	FAUSTFLOAT 	fbargraph80;
	double 	fRec281[2];
	FAUSTFLOAT 	fbargraph81;
	double 	fRec282[2];
	FAUSTFLOAT 	fbargraph82;
	double 	fRec283[2];
	FAUSTFLOAT 	fbargraph83;
	double 	fRec284[2];
	FAUSTFLOAT 	fbargraph84;
	double 	fRec285[2];
	FAUSTFLOAT 	fbargraph85;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOADecLebedev505");
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

	virtual int getNumInputs() { return 36; }
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
		fConst14 = (619.9455612060303 / fConst0);
		fConst15 = (1139.6651757122002 / fConst0);
		fConst16 = (790.3892630817644 / fConst0);
		fConst17 = (1580.7785261635288 / fConst0);
		fConst18 = (2279.3303514244003 / fConst0);
		fConst19 = (1239.8911224120607 / fConst0);
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
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec88[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fRec89[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec95[i] = 0;
		for (int i=0; i<2; i++) fRec94[i] = 0;
		for (int i=0; i<2; i++) fRec93[i] = 0;
		for (int i=0; i<2; i++) fRec97[i] = 0;
		for (int i=0; i<2; i++) fRec96[i] = 0;
		for (int i=0; i<2; i++) fRec92[i] = 0;
		for (int i=0; i<2; i++) fRec98[i] = 0;
		for (int i=0; i<2; i++) fRec91[i] = 0;
		for (int i=0; i<2; i++) fRec90[i] = 0;
		for (int i=0; i<2; i++) fRec104[i] = 0;
		for (int i=0; i<2; i++) fRec103[i] = 0;
		for (int i=0; i<2; i++) fRec102[i] = 0;
		for (int i=0; i<2; i++) fRec106[i] = 0;
		for (int i=0; i<2; i++) fRec105[i] = 0;
		for (int i=0; i<2; i++) fRec101[i] = 0;
		for (int i=0; i<2; i++) fRec107[i] = 0;
		for (int i=0; i<2; i++) fRec100[i] = 0;
		for (int i=0; i<2; i++) fRec99[i] = 0;
		for (int i=0; i<2; i++) fRec113[i] = 0;
		for (int i=0; i<2; i++) fRec112[i] = 0;
		for (int i=0; i<2; i++) fRec111[i] = 0;
		for (int i=0; i<2; i++) fRec115[i] = 0;
		for (int i=0; i<2; i++) fRec114[i] = 0;
		for (int i=0; i<2; i++) fRec110[i] = 0;
		for (int i=0; i<2; i++) fRec116[i] = 0;
		for (int i=0; i<2; i++) fRec109[i] = 0;
		for (int i=0; i<2; i++) fRec108[i] = 0;
		for (int i=0; i<2; i++) fRec122[i] = 0;
		for (int i=0; i<2; i++) fRec121[i] = 0;
		for (int i=0; i<2; i++) fRec120[i] = 0;
		for (int i=0; i<2; i++) fRec124[i] = 0;
		for (int i=0; i<2; i++) fRec123[i] = 0;
		for (int i=0; i<2; i++) fRec119[i] = 0;
		for (int i=0; i<2; i++) fRec125[i] = 0;
		for (int i=0; i<2; i++) fRec118[i] = 0;
		for (int i=0; i<2; i++) fRec117[i] = 0;
		for (int i=0; i<2; i++) fRec131[i] = 0;
		for (int i=0; i<2; i++) fRec130[i] = 0;
		for (int i=0; i<2; i++) fRec129[i] = 0;
		for (int i=0; i<2; i++) fRec133[i] = 0;
		for (int i=0; i<2; i++) fRec132[i] = 0;
		for (int i=0; i<2; i++) fRec128[i] = 0;
		for (int i=0; i<2; i++) fRec134[i] = 0;
		for (int i=0; i<2; i++) fRec127[i] = 0;
		for (int i=0; i<2; i++) fRec126[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec135[i] = 0;
		for (int i=0; i<2; i++) fRec142[i] = 0;
		for (int i=0; i<2; i++) fRec141[i] = 0;
		for (int i=0; i<2; i++) fRec140[i] = 0;
		for (int i=0; i<2; i++) fRec144[i] = 0;
		for (int i=0; i<2; i++) fRec143[i] = 0;
		for (int i=0; i<2; i++) fRec139[i] = 0;
		for (int i=0; i<2; i++) fRec145[i] = 0;
		for (int i=0; i<2; i++) fRec138[i] = 0;
		for (int i=0; i<2; i++) fRec137[i] = 0;
		for (int i=0; i<2; i++) fRec151[i] = 0;
		for (int i=0; i<2; i++) fRec150[i] = 0;
		for (int i=0; i<2; i++) fRec149[i] = 0;
		for (int i=0; i<2; i++) fRec153[i] = 0;
		for (int i=0; i<2; i++) fRec152[i] = 0;
		for (int i=0; i<2; i++) fRec148[i] = 0;
		for (int i=0; i<2; i++) fRec154[i] = 0;
		for (int i=0; i<2; i++) fRec147[i] = 0;
		for (int i=0; i<2; i++) fRec146[i] = 0;
		for (int i=0; i<2; i++) fRec158[i] = 0;
		for (int i=0; i<2; i++) fRec157[i] = 0;
		for (int i=0; i<2; i++) fRec156[i] = 0;
		for (int i=0; i<2; i++) fRec155[i] = 0;
		for (int i=0; i<2; i++) fRec161[i] = 0;
		for (int i=0; i<2; i++) fRec160[i] = 0;
		for (int i=0; i<2; i++) fRec159[i] = 0;
		for (int i=0; i<2; i++) fRec167[i] = 0;
		for (int i=0; i<2; i++) fRec166[i] = 0;
		for (int i=0; i<2; i++) fRec165[i] = 0;
		for (int i=0; i<2; i++) fRec169[i] = 0;
		for (int i=0; i<2; i++) fRec168[i] = 0;
		for (int i=0; i<2; i++) fRec164[i] = 0;
		for (int i=0; i<2; i++) fRec170[i] = 0;
		for (int i=0; i<2; i++) fRec163[i] = 0;
		for (int i=0; i<2; i++) fRec162[i] = 0;
		for (int i=0; i<2; i++) fRec175[i] = 0;
		for (int i=0; i<2; i++) fRec174[i] = 0;
		for (int i=0; i<2; i++) fRec173[i] = 0;
		for (int i=0; i<2; i++) fRec177[i] = 0;
		for (int i=0; i<2; i++) fRec176[i] = 0;
		for (int i=0; i<2; i++) fRec172[i] = 0;
		for (int i=0; i<2; i++) fRec171[i] = 0;
		for (int i=0; i<2; i++) fRec182[i] = 0;
		for (int i=0; i<2; i++) fRec181[i] = 0;
		for (int i=0; i<2; i++) fRec180[i] = 0;
		for (int i=0; i<2; i++) fRec183[i] = 0;
		for (int i=0; i<2; i++) fRec179[i] = 0;
		for (int i=0; i<2; i++) fRec178[i] = 0;
		for (int i=0; i<2; i++) fRec188[i] = 0;
		for (int i=0; i<2; i++) fRec187[i] = 0;
		for (int i=0; i<2; i++) fRec186[i] = 0;
		for (int i=0; i<2; i++) fRec189[i] = 0;
		for (int i=0; i<2; i++) fRec185[i] = 0;
		for (int i=0; i<2; i++) fRec184[i] = 0;
		for (int i=0; i<2; i++) fRec194[i] = 0;
		for (int i=0; i<2; i++) fRec193[i] = 0;
		for (int i=0; i<2; i++) fRec192[i] = 0;
		for (int i=0; i<2; i++) fRec196[i] = 0;
		for (int i=0; i<2; i++) fRec195[i] = 0;
		for (int i=0; i<2; i++) fRec191[i] = 0;
		for (int i=0; i<2; i++) fRec190[i] = 0;
		for (int i=0; i<2; i++) fRec136[i] = 0;
		for (int i=0; i<2; i++) fRec197[i] = 0;
		for (int i=0; i<2; i++) fRec198[i] = 0;
		for (int i=0; i<2; i++) fRec199[i] = 0;
		for (int i=0; i<2; i++) fRec200[i] = 0;
		for (int i=0; i<2; i++) fRec205[i] = 0;
		for (int i=0; i<2; i++) fRec204[i] = 0;
		for (int i=0; i<2; i++) fRec203[i] = 0;
		for (int i=0; i<2; i++) fRec202[i] = 0;
		for (int i=0; i<2; i++) fRec210[i] = 0;
		for (int i=0; i<2; i++) fRec209[i] = 0;
		for (int i=0; i<2; i++) fRec208[i] = 0;
		for (int i=0; i<2; i++) fRec211[i] = 0;
		for (int i=0; i<2; i++) fRec207[i] = 0;
		for (int i=0; i<2; i++) fRec206[i] = 0;
		for (int i=0; i<2; i++) fRec217[i] = 0;
		for (int i=0; i<2; i++) fRec216[i] = 0;
		for (int i=0; i<2; i++) fRec215[i] = 0;
		for (int i=0; i<2; i++) fRec219[i] = 0;
		for (int i=0; i<2; i++) fRec218[i] = 0;
		for (int i=0; i<2; i++) fRec214[i] = 0;
		for (int i=0; i<2; i++) fRec220[i] = 0;
		for (int i=0; i<2; i++) fRec213[i] = 0;
		for (int i=0; i<2; i++) fRec212[i] = 0;
		for (int i=0; i<2; i++) fRec225[i] = 0;
		for (int i=0; i<2; i++) fRec224[i] = 0;
		for (int i=0; i<2; i++) fRec223[i] = 0;
		for (int i=0; i<2; i++) fRec227[i] = 0;
		for (int i=0; i<2; i++) fRec226[i] = 0;
		for (int i=0; i<2; i++) fRec222[i] = 0;
		for (int i=0; i<2; i++) fRec221[i] = 0;
		for (int i=0; i<2; i++) fRec233[i] = 0;
		for (int i=0; i<2; i++) fRec232[i] = 0;
		for (int i=0; i<2; i++) fRec231[i] = 0;
		for (int i=0; i<2; i++) fRec235[i] = 0;
		for (int i=0; i<2; i++) fRec234[i] = 0;
		for (int i=0; i<2; i++) fRec230[i] = 0;
		for (int i=0; i<2; i++) fRec236[i] = 0;
		for (int i=0; i<2; i++) fRec229[i] = 0;
		for (int i=0; i<2; i++) fRec228[i] = 0;
		for (int i=0; i<2; i++) fRec241[i] = 0;
		for (int i=0; i<2; i++) fRec240[i] = 0;
		for (int i=0; i<2; i++) fRec239[i] = 0;
		for (int i=0; i<2; i++) fRec243[i] = 0;
		for (int i=0; i<2; i++) fRec242[i] = 0;
		for (int i=0; i<2; i++) fRec238[i] = 0;
		for (int i=0; i<2; i++) fRec237[i] = 0;
		for (int i=0; i<2; i++) fRec201[i] = 0;
		for (int i=0; i<2; i++) fRec244[i] = 0;
		for (int i=0; i<2; i++) fRec245[i] = 0;
		for (int i=0; i<2; i++) fRec246[i] = 0;
		for (int i=0; i<2; i++) fRec247[i] = 0;
		for (int i=0; i<2; i++) fRec248[i] = 0;
		for (int i=0; i<2; i++) fRec249[i] = 0;
		for (int i=0; i<2; i++) fRec250[i] = 0;
		for (int i=0; i<2; i++) fRec251[i] = 0;
		for (int i=0; i<2; i++) fRec252[i] = 0;
		for (int i=0; i<2; i++) fRec253[i] = 0;
		for (int i=0; i<2; i++) fRec254[i] = 0;
		for (int i=0; i<2; i++) fRec255[i] = 0;
		for (int i=0; i<2; i++) fRec256[i] = 0;
		for (int i=0; i<2; i++) fRec257[i] = 0;
		for (int i=0; i<2; i++) fRec258[i] = 0;
		for (int i=0; i<2; i++) fRec259[i] = 0;
		for (int i=0; i<2; i++) fRec260[i] = 0;
		for (int i=0; i<2; i++) fRec261[i] = 0;
		for (int i=0; i<2; i++) fRec262[i] = 0;
		for (int i=0; i<2; i++) fRec263[i] = 0;
		for (int i=0; i<2; i++) fRec264[i] = 0;
		for (int i=0; i<2; i++) fRec265[i] = 0;
		for (int i=0; i<2; i++) fRec266[i] = 0;
		for (int i=0; i<2; i++) fRec267[i] = 0;
		for (int i=0; i<2; i++) fRec268[i] = 0;
		for (int i=0; i<2; i++) fRec269[i] = 0;
		for (int i=0; i<2; i++) fRec270[i] = 0;
		for (int i=0; i<2; i++) fRec271[i] = 0;
		for (int i=0; i<2; i++) fRec272[i] = 0;
		for (int i=0; i<2; i++) fRec273[i] = 0;
		for (int i=0; i<2; i++) fRec274[i] = 0;
		for (int i=0; i<2; i++) fRec275[i] = 0;
		for (int i=0; i<2; i++) fRec276[i] = 0;
		for (int i=0; i<2; i++) fRec277[i] = 0;
		for (int i=0; i<2; i++) fRec278[i] = 0;
		for (int i=0; i<2; i++) fRec279[i] = 0;
		for (int i=0; i<2; i++) fRec280[i] = 0;
		for (int i=0; i<2; i++) fRec281[i] = 0;
		for (int i=0; i<2; i++) fRec282[i] = 0;
		for (int i=0; i<2; i++) fRec283[i] = 0;
		for (int i=0; i<2; i++) fRec284[i] = 0;
		for (int i=0; i<2; i++) fRec285[i] = 0;
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
		ui_interface->openVerticalBox("HOADecLebedev505");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("B-Format");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128063a0", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12871bc0", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12809540", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1280bdf0", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1286f3e0", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1289c920", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1280fd40", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12812d90", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12815e50", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1287fb80", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128a0510", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12883690", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1281b0b0", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1281ec40", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128228a0", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128265c0", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1287c050", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128b5250", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128880e0", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128a9af0", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1282c530", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12830790", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12834b40", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12838f50", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1283d500", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5");
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1286c3b0", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128b0f90", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12876810", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128a57b0", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128675a0", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128448d0", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12849700", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1284e7d0", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128538f0", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12858a90", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1285dcc0", &fbargraph20, -7e+01, 6.0);
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
		ui_interface->declare(&fbargraph21, "osc", "/output1");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1285f6b0", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph22, "osc", "/output2");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12862820", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph32, "osc", "/output3");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12889c50", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph33, "osc", "/output4");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1288d9b0", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph34, "osc", "/output5");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128919c0", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph35, "osc", "/output6");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128943f0", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph36, "osc", "/output7");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12899820", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph43, "osc", "/output8");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128b8310", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph44, "osc", "/output9");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128bdd20", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph45, "osc", "/output10");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128c3410", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph46, "osc", "/output11");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128c8870", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph47, "osc", "/output12");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128cce50", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph48, "osc", "/output13");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128d1040", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph49, "osc", "/output14");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128d4f10", &fbargraph49, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph50, "osc", "/output15");
		ui_interface->declare(&fbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128d7c90", &fbargraph50, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph51, "osc", "/output16");
		ui_interface->declare(&fbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128dc1e0", &fbargraph51, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph52, "osc", "/output17");
		ui_interface->declare(&fbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128e07f0", &fbargraph52, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph53, "osc", "/output18");
		ui_interface->declare(&fbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128e4650", &fbargraph53, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph54, "osc", "/output19");
		ui_interface->declare(&fbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128ed2e0", &fbargraph54, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph55, "osc", "/output20");
		ui_interface->declare(&fbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128f3c10", &fbargraph55, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph56, "osc", "/output21");
		ui_interface->declare(&fbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc128fa340", &fbargraph56, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph57, "osc", "/output22");
		ui_interface->declare(&fbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc129043e0", &fbargraph57, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph58, "osc", "/output23");
		ui_interface->declare(&fbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12908910", &fbargraph58, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph59, "osc", "/output24");
		ui_interface->declare(&fbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1290d240", &fbargraph59, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph60, "osc", "/output25");
		ui_interface->declare(&fbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12911c60", &fbargraph60, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph61, "osc", "/output26");
		ui_interface->declare(&fbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12916310", &fbargraph61, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph62, "osc", "/output27");
		ui_interface->declare(&fbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1291e010", &fbargraph62, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph63, "osc", "/output28");
		ui_interface->declare(&fbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12923460", &fbargraph63, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph64, "osc", "/output29");
		ui_interface->declare(&fbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12929a30", &fbargraph64, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph65, "osc", "/output30");
		ui_interface->declare(&fbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1292fa10", &fbargraph65, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph66, "osc", "/output31");
		ui_interface->declare(&fbargraph66, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12937bc0", &fbargraph66, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph67, "osc", "/output32");
		ui_interface->declare(&fbargraph67, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1293ea10", &fbargraph67, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph68, "osc", "/output33");
		ui_interface->declare(&fbargraph68, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12946040", &fbargraph68, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph69, "osc", "/output34");
		ui_interface->declare(&fbargraph69, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1294c170", &fbargraph69, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph70, "osc", "/output35");
		ui_interface->declare(&fbargraph70, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12952b20", &fbargraph70, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fbargraph71, "osc", "/output36");
		ui_interface->declare(&fbargraph71, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12959660", &fbargraph71, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fbargraph72, "osc", "/output37");
		ui_interface->declare(&fbargraph72, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc129606f0", &fbargraph72, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fbargraph73, "osc", "/output38");
		ui_interface->declare(&fbargraph73, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc129681b0", &fbargraph73, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fbargraph74, "osc", "/output39");
		ui_interface->declare(&fbargraph74, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1296c730", &fbargraph74, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fbargraph75, "osc", "/output40");
		ui_interface->declare(&fbargraph75, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12970db0", &fbargraph75, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fbargraph76, "osc", "/output41");
		ui_interface->declare(&fbargraph76, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc129759e0", &fbargraph76, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fbargraph77, "osc", "/output42");
		ui_interface->declare(&fbargraph77, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1297a170", &fbargraph77, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fbargraph78, "osc", "/output43");
		ui_interface->declare(&fbargraph78, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1297e7d0", &fbargraph78, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fbargraph79, "osc", "/output44");
		ui_interface->declare(&fbargraph79, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12983260", &fbargraph79, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fbargraph80, "osc", "/output45");
		ui_interface->declare(&fbargraph80, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12987c80", &fbargraph80, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fbargraph81, "osc", "/output46");
		ui_interface->declare(&fbargraph81, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1298c380", &fbargraph81, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fbargraph82, "osc", "/output47");
		ui_interface->declare(&fbargraph82, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12990c40", &fbargraph82, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fbargraph83, "osc", "/output48");
		ui_interface->declare(&fbargraph83, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12994a90", &fbargraph83, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fbargraph84, "osc", "/output49");
		ui_interface->declare(&fbargraph84, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc12999a50", &fbargraph84, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fbargraph85, "osc", "/output50");
		ui_interface->declare(&fbargraph85, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcc1299e860", &fbargraph85, -7e+01, 6.0);
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
		double 	fSlow25 = (1.0 / ((fConst14 / fSlow3) + 1));
		double 	fSlow26 = (1.0 / (((fConst15 / fSlow3) + (14.2724805132801 * fSlow10)) + 1));
		double 	fSlow27 = (1.0 / (((fConst16 / fSlow3) + (18.1563153134523 * fSlow10)) + 1));
		double 	fSlow28 = (72.6252612538092 * fSlow10);
		double 	fSlow29 = (fSlow28 + (fConst17 / fSlow3));
		double 	fSlow30 = (57.0899220531204 * fSlow10);
		double 	fSlow31 = (fSlow30 + (fConst18 / fSlow3));
		double 	fSlow32 = (fConst19 / fSlow3);
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
		FAUSTFLOAT* input25 = input[25];
		FAUSTFLOAT* input26 = input[26];
		FAUSTFLOAT* input27 = input[27];
		FAUSTFLOAT* input28 = input[28];
		FAUSTFLOAT* input29 = input[29];
		FAUSTFLOAT* input30 = input[30];
		FAUSTFLOAT* input31 = input[31];
		FAUSTFLOAT* input32 = input[32];
		FAUSTFLOAT* input33 = input[33];
		FAUSTFLOAT* input34 = input[34];
		FAUSTFLOAT* input35 = input[35];
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
			double fTemp59 = ((double)input30[i] * fRec3[0]);
			fRec86[0] = (fRec86[1] + fRec84[1]);
			fRec85[0] = (fRec85[1] + fRec86[1]);
			fRec84[0] = (fSlow27 * ((fSlow4 * fTemp59) - ((fSlow28 * fRec85[0]) + (fSlow29 * fRec86[0]))));
			fRec88[0] = (fRec88[1] + fRec83[1]);
			fRec87[0] = (fRec87[1] + fRec88[1]);
			fRec83[0] = (fSlow26 * (fRec84[0] - ((fSlow30 * fRec87[0]) + (fSlow31 * fRec88[0]))));
			fRec89[0] = (fRec89[1] + fRec82[1]);
			fRec82[0] = (fSlow25 * (fRec83[0] - (fSlow32 * fRec89[0])));
			double fTemp60 = (fRec82[0] + (fSlow2 * fTemp59));
			fRec81[0] = max((fRec81[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fbargraph15 = fRec81[0];
			double fTemp61 = fTemp60;
			double fTemp62 = (0.042115870353719366 * fTemp61);
			double fTemp63 = ((double)input31[i] * fRec3[0]);
			fRec95[0] = (fRec95[1] + fRec93[1]);
			fRec94[0] = (fRec94[1] + fRec95[1]);
			fRec93[0] = (fSlow27 * ((fSlow4 * fTemp63) - ((fSlow28 * fRec94[0]) + (fSlow29 * fRec95[0]))));
			fRec97[0] = (fRec97[1] + fRec92[1]);
			fRec96[0] = (fRec96[1] + fRec97[1]);
			fRec92[0] = (fSlow26 * (fRec93[0] - ((fSlow30 * fRec96[0]) + (fSlow31 * fRec97[0]))));
			fRec98[0] = (fRec98[1] + fRec91[1]);
			fRec91[0] = (fSlow25 * (fRec92[0] - (fSlow32 * fRec98[0])));
			double fTemp64 = (fRec91[0] + (fSlow2 * fTemp63));
			fRec90[0] = max((fRec90[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fbargraph16 = fRec90[0];
			double fTemp65 = fTemp64;
			double fTemp66 = (9.987855848740033e-18 * fTemp65);
			double fTemp67 = ((double)input32[i] * fRec3[0]);
			fRec104[0] = (fRec104[1] + fRec102[1]);
			fRec103[0] = (fRec103[1] + fRec104[1]);
			fRec102[0] = (fSlow27 * ((fSlow4 * fTemp67) - ((fSlow28 * fRec103[0]) + (fSlow29 * fRec104[0]))));
			fRec106[0] = (fRec106[1] + fRec101[1]);
			fRec105[0] = (fRec105[1] + fRec106[1]);
			fRec101[0] = (fSlow26 * (fRec102[0] - ((fSlow30 * fRec105[0]) + (fSlow31 * fRec106[0]))));
			fRec107[0] = (fRec107[1] + fRec100[1]);
			fRec100[0] = (fSlow25 * (fRec101[0] - (fSlow32 * fRec107[0])));
			double fTemp68 = (fRec100[0] + (fSlow2 * fTemp67));
			fRec99[0] = max((fRec99[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fbargraph17 = fRec99[0];
			double fTemp69 = fTemp68;
			double fTemp70 = (8.09044008694836e-34 * fTemp69);
			double fTemp71 = ((double)input33[i] * fRec3[0]);
			fRec113[0] = (fRec113[1] + fRec111[1]);
			fRec112[0] = (fRec112[1] + fRec113[1]);
			fRec111[0] = (fSlow27 * ((fSlow4 * fTemp71) - ((fSlow28 * fRec112[0]) + (fSlow29 * fRec113[0]))));
			fRec115[0] = (fRec115[1] + fRec110[1]);
			fRec114[0] = (fRec114[1] + fRec115[1]);
			fRec110[0] = (fSlow26 * (fRec111[0] - ((fSlow30 * fRec114[0]) + (fSlow31 * fRec115[0]))));
			fRec116[0] = (fRec116[1] + fRec109[1]);
			fRec109[0] = (fSlow25 * (fRec110[0] - (fSlow32 * fRec116[0])));
			double fTemp72 = (fRec109[0] + (fSlow2 * fTemp71));
			fRec108[0] = max((fRec108[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp72))))));
			fbargraph18 = fRec108[0];
			double fTemp73 = fTemp72;
			double fTemp74 = (4.0448961198413007e-50 * fTemp73);
			double fTemp75 = ((double)input34[i] * fRec3[0]);
			fRec122[0] = (fRec122[1] + fRec120[1]);
			fRec121[0] = (fRec121[1] + fRec122[1]);
			fRec120[0] = (fSlow27 * ((fSlow4 * fTemp75) - ((fSlow28 * fRec121[0]) + (fSlow29 * fRec122[0]))));
			fRec124[0] = (fRec124[1] + fRec119[1]);
			fRec123[0] = (fRec123[1] + fRec124[1]);
			fRec119[0] = (fSlow26 * (fRec120[0] - ((fSlow30 * fRec123[0]) + (fSlow31 * fRec124[0]))));
			fRec125[0] = (fRec125[1] + fRec118[1]);
			fRec118[0] = (fSlow25 * (fRec119[0] - (fSlow32 * fRec125[0])));
			double fTemp76 = (fRec118[0] + (fSlow2 * fTemp75));
			fRec117[0] = max((fRec117[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp76))))));
			fbargraph19 = fRec117[0];
			double fTemp77 = fTemp76;
			double fTemp78 = (1.3135133594325083e-66 * fTemp77);
			double fTemp79 = ((double)input35[i] * fRec3[0]);
			fRec131[0] = (fRec131[1] + fRec129[1]);
			fRec130[0] = (fRec130[1] + fRec131[1]);
			fRec129[0] = (fSlow27 * ((fSlow4 * fTemp79) - ((fSlow28 * fRec130[0]) + (fSlow29 * fRec131[0]))));
			fRec133[0] = (fRec133[1] + fRec128[1]);
			fRec132[0] = (fRec132[1] + fRec133[1]);
			fRec128[0] = (fSlow26 * (fRec129[0] - ((fSlow30 * fRec132[0]) + (fSlow31 * fRec133[0]))));
			fRec134[0] = (fRec134[1] + fRec127[1]);
			fRec127[0] = (fSlow25 * (fRec128[0] - (fSlow32 * fRec134[0])));
			double fTemp80 = (fRec127[0] + (fSlow2 * fTemp79));
			fRec126[0] = max((fRec126[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp80))))));
			fbargraph20 = fRec126[0];
			double fTemp81 = fTemp80;
			double fTemp82 = (2.5434040020076175e-83 * fTemp81);
			double fTemp83 = (fRec1[0] * ((((((((((((((((((((fTemp2 + fTemp6) + fTemp10) + fTemp14) + fTemp18) + fTemp22) + fTemp26) + fTemp30) + fTemp34) + fTemp38) + fTemp42) + fTemp46) + fTemp50) + fTemp54) + fTemp58) + fTemp62) + fTemp66) + fTemp70) + fTemp74) + fTemp78) + fTemp82));
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp83))))));
			fbargraph21 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp83;
			double fTemp84 = (0.0219942959691286 * fTemp9);
			double fTemp85 = (0.024590370452110585 * fTemp21);
			double fTemp86 = (0.026560635762986527 * fTemp37);
			double fTemp87 = (0.014285714285714285 * fTemp41);
			double fTemp88 = (0.028171808490950554 * fTemp57);
			double fTemp89 = (0.020389258061373223 * fTemp65);
			double fTemp90 = (0.029546842014263944 * fTemp81);
			double fTemp91 = (0.014197256999998666 * fTemp13);
			double fTemp92 = (0.020573779994945588 * fTemp29);
			double fTemp93 = (0.021295885499998 * fTemp49);
			double fTemp94 = (0.02202291575478033 * fTemp73);
			double fTemp95 = (fRec1[0] * ((((((((fTemp2 + fTemp84) + fTemp85) + fTemp86) + fTemp87) + fTemp88) + fTemp89) + fTemp90) - (((fTemp91 + fTemp92) + fTemp93) + fTemp94)));
			fRec135[0] = max((fRec135[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp95))))));
			fbargraph22 = fRec135[0];
			output1[i] = (FAUSTFLOAT)fTemp95;
			double fTemp96 = ((double)input29[i] * fRec3[0]);
			fRec142[0] = (fRec142[1] + fRec140[1]);
			fRec141[0] = (fRec141[1] + fRec142[1]);
			fRec140[0] = (fSlow27 * ((fSlow4 * fTemp96) - ((fSlow28 * fRec141[0]) + (fSlow29 * fRec142[0]))));
			fRec144[0] = (fRec144[1] + fRec139[1]);
			fRec143[0] = (fRec143[1] + fRec144[1]);
			fRec139[0] = (fSlow26 * (fRec140[0] - ((fSlow30 * fRec143[0]) + (fSlow31 * fRec144[0]))));
			fRec145[0] = (fRec145[1] + fRec138[1]);
			fRec138[0] = (fSlow25 * (fRec139[0] - (fSlow32 * fRec145[0])));
			double fTemp97 = (fRec138[0] + (fSlow2 * fTemp96));
			fRec137[0] = max((fRec137[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp97))))));
			fbargraph23 = fRec137[0];
			double fTemp98 = fTemp97;
			double fTemp99 = (0.020389258061373223 * fTemp98);
			double fTemp100 = ((double)input25[i] * fRec3[0]);
			fRec151[0] = (fRec151[1] + fRec149[1]);
			fRec150[0] = (fRec150[1] + fRec151[1]);
			fRec149[0] = (fSlow27 * ((fSlow4 * fTemp100) - ((fSlow28 * fRec150[0]) + (fSlow29 * fRec151[0]))));
			fRec153[0] = (fRec153[1] + fRec148[1]);
			fRec152[0] = (fRec152[1] + fRec153[1]);
			fRec148[0] = (fSlow26 * (fRec149[0] - ((fSlow30 * fRec152[0]) + (fSlow31 * fRec153[0]))));
			fRec154[0] = (fRec154[1] + fRec147[1]);
			fRec147[0] = (fSlow25 * (fRec148[0] - (fSlow32 * fRec154[0])));
			double fTemp101 = (fRec147[0] + (fSlow2 * fTemp100));
			fRec146[0] = max((fRec146[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp101))))));
			fbargraph24 = fRec146[0];
			double fTemp102 = fTemp101;
			double fTemp103 = (0.029546842014263944 * fTemp102);
			double fTemp104 = ((double)input4[i] * fRec3[0]);
			fRec158[0] = (fRec158[1] + fRec156[1]);
			fRec157[0] = (fRec157[1] + fRec158[1]);
			fRec156[0] = (fSlow11 * ((fSlow4 * fTemp104) - ((fSlow12 * fRec157[0]) + (fSlow13 * fRec158[0]))));
			double fTemp105 = (fRec156[0] + (fSlow2 * fTemp104));
			fRec155[0] = max((fRec155[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp105))))));
			fbargraph25 = fRec155[0];
			double fTemp106 = fTemp105;
			double fTemp107 = ((double)input1[i] * fRec3[0]);
			fRec161[0] = (fRec161[1] + fRec160[1]);
			fRec160[0] = (fSlow8 * ((fSlow4 * fTemp107) - (fSlow9 * fRec161[0])));
			double fTemp108 = (fRec160[0] + (fSlow2 * fTemp107));
			fRec159[0] = max((fRec159[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp108))))));
			fbargraph26 = fRec159[0];
			double fTemp109 = fTemp108;
			double fTemp110 = (0.0219942959691286 * fTemp109);
			double fTemp111 = ((double)input27[i] * fRec3[0]);
			fRec167[0] = (fRec167[1] + fRec165[1]);
			fRec166[0] = (fRec166[1] + fRec167[1]);
			fRec165[0] = (fSlow27 * ((fSlow4 * fTemp111) - ((fSlow28 * fRec166[0]) + (fSlow29 * fRec167[0]))));
			fRec169[0] = (fRec169[1] + fRec164[1]);
			fRec168[0] = (fRec168[1] + fRec169[1]);
			fRec164[0] = (fSlow26 * (fRec165[0] - ((fSlow30 * fRec168[0]) + (fSlow31 * fRec169[0]))));
			fRec170[0] = (fRec170[1] + fRec163[1]);
			fRec163[0] = (fSlow25 * (fRec164[0] - (fSlow32 * fRec170[0])));
			double fTemp112 = (fRec163[0] + (fSlow2 * fTemp111));
			fRec162[0] = max((fRec162[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp112))))));
			fbargraph27 = fRec162[0];
			double fTemp113 = fTemp112;
			double fTemp114 = (0.02202291575478033 * fTemp113);
			double fTemp115 = (fTemp2 + fTemp10);
			double fTemp116 = ((double)input16[i] * fRec3[0]);
			fRec175[0] = (fRec175[1] + fRec173[1]);
			fRec174[0] = (fRec174[1] + fRec175[1]);
			fRec173[0] = (fSlow20 * ((fSlow4 * fTemp116) - ((fSlow21 * fRec174[0]) + (fSlow22 * fRec175[0]))));
			fRec177[0] = (fRec177[1] + fRec172[1]);
			fRec176[0] = (fRec176[1] + fRec177[1]);
			fRec172[0] = (fSlow19 * (fRec173[0] - ((fSlow23 * fRec176[0]) + (fSlow24 * fRec177[0]))));
			double fTemp117 = (fRec172[0] + (fSlow2 * fTemp116));
			fRec171[0] = max((fRec171[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp117))))));
			fbargraph28 = fRec171[0];
			double fTemp118 = fTemp117;
			double fTemp119 = ((double)input9[i] * fRec3[0]);
			fRec182[0] = (fRec182[1] + fRec180[1]);
			fRec181[0] = (fRec181[1] + fRec182[1]);
			fRec180[0] = (fSlow15 * ((fSlow4 * fTemp119) - ((fSlow16 * fRec181[0]) + (fSlow17 * fRec182[0]))));
			fRec183[0] = (fRec183[1] + fRec179[1]);
			fRec179[0] = (fSlow14 * (fRec180[0] - (fSlow18 * fRec183[0])));
			double fTemp120 = (fRec179[0] + (fSlow2 * fTemp119));
			fRec178[0] = max((fRec178[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp120))))));
			fbargraph29 = fRec178[0];
			double fTemp121 = fTemp120;
			double fTemp122 = (0.026560635762986527 * fTemp121);
			double fTemp123 = ((double)input11[i] * fRec3[0]);
			fRec188[0] = (fRec188[1] + fRec186[1]);
			fRec187[0] = (fRec187[1] + fRec188[1]);
			fRec186[0] = (fSlow15 * ((fSlow4 * fTemp123) - ((fSlow16 * fRec187[0]) + (fSlow17 * fRec188[0]))));
			fRec189[0] = (fRec189[1] + fRec185[1]);
			fRec185[0] = (fSlow14 * (fRec186[0] - (fSlow18 * fRec189[0])));
			double fTemp124 = (fRec185[0] + (fSlow2 * fTemp123));
			fRec184[0] = max((fRec184[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp124))))));
			fbargraph30 = fRec184[0];
			double fTemp125 = fTemp124;
			double fTemp126 = (0.020573779994945588 * fTemp125);
			double fTemp127 = ((double)input18[i] * fRec3[0]);
			fRec194[0] = (fRec194[1] + fRec192[1]);
			fRec193[0] = (fRec193[1] + fRec194[1]);
			fRec192[0] = (fSlow20 * ((fSlow4 * fTemp127) - ((fSlow21 * fRec193[0]) + (fSlow22 * fRec194[0]))));
			fRec196[0] = (fRec196[1] + fRec191[1]);
			fRec195[0] = (fRec195[1] + fRec196[1]);
			fRec191[0] = (fSlow19 * (fRec192[0] - ((fSlow23 * fRec195[0]) + (fSlow24 * fRec196[0]))));
			double fTemp128 = (fRec191[0] + (fSlow2 * fTemp127));
			fRec190[0] = max((fRec190[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp128))))));
			fbargraph31 = fRec190[0];
			double fTemp129 = fTemp128;
			double fTemp130 = (fRec1[0] * ((fTemp99 + (fTemp103 + ((3.0114518464024883e-18 * fTemp106) + (fTemp110 + (fTemp114 + ((((fTemp88 + (fTemp93 + (fTemp87 + fTemp115))) + (1.248481981092504e-18 * fTemp65)) + (4.045543993047532e-18 * fTemp73)) + (9.046111374420218e-18 * fTemp81))))))) - ((6.900103018926964e-18 * fTemp118) + (fTemp122 + ((fTemp126 + ((4.879109635569025e-18 * fTemp37) + ((fTemp91 + fTemp85) + (1.2597806908585981e-18 * fTemp29)))) + (2.607993801258108e-18 * fTemp129))))));
			fRec136[0] = max((fRec136[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp130))))));
			fbargraph32 = fRec136[0];
			output2[i] = (FAUSTFLOAT)fTemp130;
			double fTemp131 = (fRec1[0] * (((2.496963962185008e-18 * fTemp98) + ((1.8092222748840437e-17 * fTemp102) + ((9.75821927113805e-18 * fTemp121) + ((2.693524415809287e-18 * fTemp109) + ((5.215987602516216e-18 * fTemp129) + (fTemp94 + (fTemp88 + (fTemp87 + (fTemp92 + (fTemp2 + fTemp85)))))))))) - ((1.3800206037853928e-17 * fTemp118) + ((6.0229036928049765e-18 * fTemp106) + ((8.091087986095064e-18 * fTemp113) + ((2.5195613817171963e-18 * fTemp125) + (fTemp90 + (fTemp89 + (fTemp93 + (fTemp86 + (fTemp84 + fTemp91)))))))))));
			fRec197[0] = max((fRec197[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp131))))));
			fbargraph33 = fRec197[0];
			output3[i] = (FAUSTFLOAT)fTemp131;
			double fTemp132 = (fRec1[0] * ((fTemp122 + ((9.034355539207464e-18 * fTemp106) + (fTemp126 + (fTemp88 + (fTemp93 + (fTemp87 + ((1.4637328906707076e-17 * fTemp37) + (fTemp2 + (3.779342072575794e-18 * fTemp29))))))))) - (fTemp99 + (fTemp103 + ((2.0700309056780893e-17 * fTemp118) + (fTemp110 + (fTemp114 + ((7.823981403774323e-18 * fTemp129) + ((7.962406901797708e-17 * fTemp81) + (((fTemp85 + (fTemp91 + (4.040286623713931e-18 * fTemp9))) + (3.745445943277512e-18 * fTemp65)) + (1.2136631979142595e-17 * fTemp73)))))))))));
			fRec198[0] = max((fRec198[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp132))))));
			fbargraph34 = fRec198[0];
			output4[i] = (FAUSTFLOAT)fTemp132;
			double fTemp133 = (fRec1[0] * ((fTemp82 + (fTemp74 + (fTemp66 + (fTemp58 + (fTemp50 + (fTemp42 + (fTemp38 + (fTemp30 + (fTemp22 + (fTemp14 + fTemp115)))))))))) - (fTemp78 + (fTemp70 + (fTemp62 + (fTemp54 + (fTemp46 + (fTemp34 + (fTemp26 + (fTemp6 + fTemp18))))))))));
			fRec199[0] = max((fRec199[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp133))))));
			fbargraph35 = fRec199[0];
			output5[i] = (FAUSTFLOAT)fTemp133;
			double fTemp134 = (0.02936388767304044 * fTemp77);
			double fTemp135 = (0.04844794288974959 * fTemp73);
			double fTemp136 = (0.03390649690496764 * fTemp69);
			double fTemp137 = (0.012520803773755805 * fTemp57);
			double fTemp138 = (0.04732418999999555 * fTemp49);
			double fTemp139 = (0.022574955908289243 * fTemp1);
			double fTemp140 = (0.0276485614705685 * fTemp5);
			double fTemp141 = (fTemp139 + fTemp140);
			double fTemp142 = (0.027648561470568506 * fTemp9);
			double fTemp143 = (0.012619783999998805 * fTemp13);
			double fTemp144 = (0.043716214137085485 * fTemp17);
			double fTemp145 = (0.02185810706854275 * fTemp21);
			double fTemp146 = (0.03879429159750975 * fTemp29);
			double fTemp147 = (0.04089277388695433 * fTemp33);
			double fTemp148 = (0.01669440503167441 * fTemp37);
			double fTemp149 = (0.013385302265262959 * fTemp45);
			double fTemp150 = (0.035414181017315374 * fTemp53);
			double fTemp151 = (0.009285676600414948 * fTemp81);
			double fTemp152 = (0.010558468816293319 * fTemp25);
			double fTemp153 = (0.027513227513227514 * fTemp41);
			double fTemp154 = (0.02812594988253908 * fTemp61);
			double fTemp155 = (0.01922317685141265 * fTemp65);
			double fTemp156 = (fRec1[0] * (((fTemp134 + (fTemp135 + (fTemp136 + (fTemp137 + ((fTemp138 + ((((((((fTemp141 + fTemp142) + fTemp143) + fTemp144) + fTemp145) + fTemp146) + fTemp147) + fTemp148) + fTemp149)) + fTemp150))))) + fTemp151) - (((fTemp152 + fTemp153) + fTemp154) + fTemp155)));
			fRec200[0] = max((fRec200[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp156))))));
			fbargraph36 = fRec200[0];
			output6[i] = (FAUSTFLOAT)fTemp156;
			double fTemp157 = ((double)input5[i] * fRec3[0]);
			fRec205[0] = (fRec205[1] + fRec203[1]);
			fRec204[0] = (fRec204[1] + fRec205[1]);
			fRec203[0] = (fSlow11 * ((fSlow4 * fTemp157) - ((fSlow12 * fRec204[0]) + (fSlow13 * fRec205[0]))));
			double fTemp158 = (fRec203[0] + (fSlow2 * fTemp157));
			fRec202[0] = max((fRec202[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp158))))));
			fbargraph37 = fRec202[0];
			double fTemp159 = fTemp158;
			double fTemp160 = (0.043716214137085485 * fTemp159);
			double fTemp161 = ((double)input10[i] * fRec3[0]);
			fRec210[0] = (fRec210[1] + fRec208[1]);
			fRec209[0] = (fRec209[1] + fRec210[1]);
			fRec208[0] = (fSlow15 * ((fSlow4 * fTemp161) - ((fSlow16 * fRec209[0]) + (fSlow17 * fRec210[0]))));
			fRec211[0] = (fRec211[1] + fRec207[1]);
			fRec207[0] = (fSlow14 * (fRec208[0] - (fSlow18 * fRec211[0])));
			double fTemp162 = (fRec207[0] + (fSlow2 * fTemp161));
			fRec206[0] = max((fRec206[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp162))))));
			fbargraph38 = fRec206[0];
			double fTemp163 = fTemp162;
			double fTemp164 = (5.007920464891509e-18 * fTemp163);
			double fTemp165 = (0.009285676600414948 * fTemp102);
			double fTemp166 = (2.6768460856911014e-18 * fTemp106);
			double fTemp167 = (0.027648561470568506 * fTemp109);
			double fTemp168 = ((double)input28[i] * fRec3[0]);
			fRec217[0] = (fRec217[1] + fRec215[1]);
			fRec216[0] = (fRec216[1] + fRec217[1]);
			fRec215[0] = (fSlow27 * ((fSlow4 * fTemp168) - ((fSlow28 * fRec216[0]) + (fSlow29 * fRec217[0]))));
			fRec219[0] = (fRec219[1] + fRec214[1]);
			fRec218[0] = (fRec218[1] + fRec219[1]);
			fRec214[0] = (fSlow26 * (fRec215[0] - ((fSlow30 * fRec218[0]) + (fSlow31 * fRec219[0]))));
			fRec220[0] = (fRec220[1] + fRec213[1]);
			fRec213[0] = (fSlow25 * (fRec214[0] - (fSlow32 * fRec220[0])));
			double fTemp169 = (fRec213[0] + (fSlow2 * fTemp168));
			fRec212[0] = max((fRec212[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp169))))));
			fbargraph39 = fRec212[0];
			double fTemp170 = fTemp169;
			double fTemp171 = (4.152348290496825e-18 * fTemp170);
			double fTemp172 = ((double)input19[i] * fRec3[0]);
			fRec225[0] = (fRec225[1] + fRec223[1]);
			fRec224[0] = (fRec224[1] + fRec225[1]);
			fRec223[0] = (fSlow20 * ((fSlow4 * fTemp172) - ((fSlow21 * fRec224[0]) + (fSlow22 * fRec225[0]))));
			fRec227[0] = (fRec227[1] + fRec222[1]);
			fRec226[0] = (fRec226[1] + fRec227[1]);
			fRec222[0] = (fSlow19 * (fRec223[0] - ((fSlow23 * fRec226[0]) + (fSlow24 * fRec227[0]))));
			double fTemp173 = (fRec222[0] + (fSlow2 * fTemp172));
			fRec221[0] = max((fRec221[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp173))))));
			fbargraph40 = fRec221[0];
			double fTemp174 = fTemp173;
			double fTemp175 = (0.013385302265262959 * fTemp174);
			double fTemp176 = (5.795541780573573e-18 * fTemp129);
			double fTemp177 = (0.03879429159750975 * fTemp125);
			double fTemp178 = (2.8429185316539108e-18 * fTemp81);
			double fTemp179 = (2.6768460856911007e-18 * fTemp17);
			double fTemp180 = (1.6929861152980278e-18 * fTemp9);
			double fTemp181 = (2.3754652515039685e-18 * fTemp29);
			double fTemp182 = (8.196133787387049e-19 * fTemp45);
			double fTemp183 = (3.0667124528564294e-18 * (fTemp37 + fTemp118));
			double fTemp184 = (0.01922317685141265 * fTemp98);
			double fTemp185 = ((double)input26[i] * fRec3[0]);
			fRec233[0] = (fRec233[1] + fRec231[1]);
			fRec232[0] = (fRec232[1] + fRec233[1]);
			fRec231[0] = (fSlow27 * ((fSlow4 * fTemp185) - ((fSlow28 * fRec232[0]) + (fSlow29 * fRec233[0]))));
			fRec235[0] = (fRec235[1] + fRec230[1]);
			fRec234[0] = (fRec234[1] + fRec235[1]);
			fRec230[0] = (fSlow26 * (fRec231[0] - ((fSlow30 * fRec234[0]) + (fSlow31 * fRec235[0]))));
			fRec236[0] = (fRec236[1] + fRec229[1]);
			fRec229[0] = (fSlow25 * (fRec230[0] - (fSlow32 * fRec236[0])));
			double fTemp186 = (fRec229[0] + (fSlow2 * fTemp185));
			fRec228[0] = max((fRec228[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp186))))));
			fbargraph41 = fRec228[0];
			double fTemp187 = fTemp186;
			double fTemp188 = (7.19207820986228e-18 * fTemp187);
			double fTemp189 = ((double)input17[i] * fRec3[0]);
			fRec241[0] = (fRec241[1] + fRec239[1]);
			fRec240[0] = (fRec240[1] + fRec241[1]);
			fRec239[0] = (fSlow20 * ((fSlow4 * fTemp189) - ((fSlow21 * fRec240[0]) + (fSlow22 * fRec241[0]))));
			fRec243[0] = (fRec243[1] + fRec238[1]);
			fRec242[0] = (fRec242[1] + fRec243[1]);
			fRec238[0] = (fSlow19 * (fRec239[0] - ((fSlow23 * fRec242[0]) + (fSlow24 * fRec243[0]))));
			double fTemp190 = (fRec238[0] + (fSlow2 * fTemp189));
			fRec237[0] = max((fRec237[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp190))))));
			fbargraph42 = fRec237[0];
			double fTemp191 = fTemp190;
			double fTemp192 = (0.035414181017315374 * fTemp191);
			double fTemp193 = (0.01669440503167441 * fTemp121);
			double fTemp194 = (0.04844794288974959 * fTemp113);
			double fTemp195 = (8.89974272778084e-18 * fTemp73);
			double fTemp196 = (6.5054795140920346e-18 * fTemp53);
			double fTemp197 = (1.1770801000262998e-18 * fTemp65);
			double fTemp198 = (fRec1[0] * ((fTemp160 + (fTemp164 + (fTemp165 + (fTemp166 + (fTemp167 + (fTemp171 + (fTemp175 + (fTemp176 + (fTemp177 + (fTemp178 + (fTemp134 + (fTemp137 + (((fTemp179 + (fTemp143 + (fTemp141 + fTemp180))) + fTemp181) + fTemp182))))))))))))) - (fTemp183 + (fTemp184 + (fTemp188 + (fTemp192 + (fTemp193 + (fTemp194 + (fTemp195 + (fTemp136 + ((fTemp154 + (fTemp196 + (fTemp138 + (fTemp153 + (fTemp147 + (fTemp145 + fTemp152)))))) + fTemp197)))))))))));
			fRec201[0] = max((fRec201[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp198))))));
			fbargraph43 = fRec201[0];
			output7[i] = (FAUSTFLOAT)fTemp198;
			double fTemp199 = (1.779948545556168e-17 * fTemp113);
			double fTemp200 = (fTemp141 + fTemp143);
			double fTemp201 = (4.750930503007937e-18 * fTemp125);
			double fTemp202 = (1.6392267574774099e-18 * fTemp174);
			double fTemp203 = (3.3859722305960555e-18 * fTemp109);
			double fTemp204 = (1.3010959028184069e-17 * fTemp191);
			double fTemp205 = (5.6858370633078216e-18 * fTemp102);
			double fTemp206 = (5.353692171382201e-18 * fTemp159);
			double fTemp207 = (6.133424905712859e-18 * (fTemp121 - fTemp118));
			double fTemp208 = (1.1591083561147146e-17 * fTemp129);
			double fTemp209 = (8.30469658099365e-18 * fTemp170);
			double fTemp210 = (5.353692171382203e-18 * fTemp106);
			double fTemp211 = (1.438415641972456e-17 * fTemp187);
			double fTemp212 = (2.3541602000525996e-18 * fTemp98);
			double fTemp213 = (1.0015840929783018e-17 * fTemp163);
			double fTemp214 = ((((((((fTemp199 + (((((fTemp155 + (fTemp137 + (fTemp138 + (fTemp147 + (fTemp145 + fTemp200))))) + fTemp136) + fTemp134) + fTemp201) + fTemp202)) + fTemp203) + fTemp204) + fTemp205) + fTemp206) + fTemp207) - (((((((((fTemp154 + (fTemp150 + (fTemp149 + (fTemp153 + (fTemp148 + (fTemp146 + (fTemp152 + (fTemp142 + fTemp144)))))))) + fTemp135) + fTemp151) + fTemp208) + fTemp209) + fTemp210) + fTemp211) + fTemp212) + fTemp213)) * fRec1[0]);
			fRec244[0] = max((fRec244[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp214))))));
			fbargraph44 = fRec244[0];
			output8[i] = (FAUSTFLOAT)fTemp214;
			double fTemp215 = (1.5023761394674526e-17 * fTemp163);
			double fTemp216 = (8.030538257073304e-18 * fTemp106);
			double fTemp217 = (1.2457044871490476e-17 * fTemp170);
			double fTemp218 = (1.738662534172072e-17 * fTemp129);
			double fTemp219 = (9.20013735856929e-18 * fTemp37);
			double fTemp220 = (1.9516438542276106e-17 * fTemp53);
			double fTemp221 = (3.5312403000788995e-18 * fTemp65);
			double fTemp222 = (2.6699228183342524e-17 * fTemp73);
			double fTemp223 = (2.1576234629586837e-17 * fTemp187);
			double fTemp224 = (9.200137358569287e-18 * fTemp118);
			double fTemp225 = (7.126395754511905e-18 * fTemp29);
			double fTemp226 = (5.078958345894083e-18 * fTemp9);
			double fTemp227 = (8.0305382570733e-18 * fTemp17);
			double fTemp228 = (2.4588401362161147e-18 * fTemp45);
			double fTemp229 = (2.5023430732567692e-17 * fTemp81);
			double fTemp230 = (fRec1[0] * ((fTemp215 + (fTemp184 + (fTemp192 + (fTemp193 + (fTemp216 + (fTemp217 + (fTemp194 + (fTemp218 + (fTemp134 + (((((fTemp219 + fTemp200) + fTemp220) + fTemp137) + fTemp221) + fTemp222)))))))))) - (fTemp160 + (fTemp223 + (fTemp165 + (fTemp224 + (fTemp167 + (fTemp175 + (fTemp177 + ((((((((fTemp225 + (((fTemp226 + fTemp227) + fTemp145) + fTemp152)) + fTemp147) + fTemp153) + fTemp228) + fTemp138) + fTemp154) + fTemp136) + fTemp229))))))))));
			fRec245[0] = max((fRec245[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp230))))));
			fbargraph45 = fRec245[0];
			output9[i] = (FAUSTFLOAT)fTemp230;
			double fTemp231 = (0.025630902468550196 * fTemp98);
			double fTemp232 = (0.043716214137085485 * fTemp106);
			double fTemp233 = (0.0276485614705685 * fTemp109);
			double fTemp234 = (fTemp139 + fTemp142);
			double fTemp235 = (0.025396825396825397 * fTemp41);
			double fTemp236 = (0.0256309024685502 * fTemp65);
			double fTemp237 = (0.02768453879414262 * fTemp73);
			double fTemp238 = (0.037859351999996446 * fTemp129);
			double fTemp239 = (0.025862861065006505 * fTemp125);
			double fTemp240 = (0.05008321509502321 * fTemp57);
			double fTemp241 = (0.02523956799999763 * fTemp13);
			double fTemp242 = (0.02586286106500651 * fTemp29);
			double fTemp243 = (fRec1[0] * ((fTemp231 + ((6.133424905712858e-18 * fTemp118) + (fTemp232 + (fTemp233 + ((((fTemp234 + (2.6768460856911007e-18 * fTemp21)) + fTemp235) + fTemp236) + fTemp237))))) - ((0.0333888100633488 * (fTemp37 - fTemp121)) + ((0.03714270640165978 * fTemp102) + ((0.027684538794142628 * fTemp113) + (fTemp238 + (fTemp239 + ((fTemp240 + ((fTemp241 + fTemp242) + (2.3182167122294292e-18 * fTemp49))) + (0.037142706401659786 * fTemp81)))))))));
			fRec246[0] = max((fRec246[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp243))))));
			fbargraph46 = fRec246[0];
			output10[i] = (FAUSTFLOAT)fTemp243;
			double fTemp244 = (0.0256309024685502 * fTemp98);
			double fTemp245 = (0.025862861065006505 * fTemp29);
			double fTemp246 = (0.02586286106500651 * fTemp125);
			double fTemp247 = (0.0276485614705685 * fTemp9);
			double fTemp248 = (0.025630902468550196 * fTemp65);
			double fTemp249 = (fRec1[0] * ((fTemp244 + ((1.8400274717138574e-17 * fTemp118) + ((0.033388810063348796 * fTemp121) + (fTemp167 + (fTemp238 + (((6.954650136688288e-18 * fTemp49) + (fTemp235 + ((0.03338881006334882 * fTemp37) + (fTemp139 + fTemp245)))) + (0.03714270640165973 * fTemp81))))))) - ((0.03714270640165983 * fTemp102) + (fTemp232 + ((0.027684538794142618 * fTemp113) + (fTemp246 + ((0.02768453879414263 * fTemp73) + ((fTemp240 + ((8.0305382570733e-18 * fTemp21) + (fTemp241 + fTemp247))) + fTemp248))))))));
			fRec247[0] = max((fRec247[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp249))))));
			fbargraph47 = fRec247[0];
			output11[i] = (FAUSTFLOAT)fTemp249;
			double fTemp250 = (fRec1[0] * (((0.037142706401659786 * fTemp102) + ((3.0667124528564293e-17 * fTemp118) + (fTemp232 + ((0.027684538794142663 * fTemp113) + (fTemp239 + ((fTemp235 + ((fTemp242 + (fTemp139 + (1.3384230428455503e-17 * fTemp21))) + (0.03338881006334876 * fTemp37))) + (0.03714270640165978 * fTemp81))))))) - (fTemp231 + ((0.03338881006334885 * fTemp121) + (fTemp233 + (fTemp238 + ((fTemp236 + (fTemp240 + ((fTemp142 + fTemp241) + (1.1591083561147147e-17 * fTemp49)))) + (0.027684538794142587 * fTemp73))))))));
			fRec248[0] = max((fRec248[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp250))))));
			fbargraph48 = fRec248[0];
			output12[i] = (FAUSTFLOAT)fTemp250;
			double fTemp251 = (fRec1[0] * (((0.037142706401659856 * fTemp102) + ((4.293397433999001e-17 * fTemp118) + ((0.02768453879414263 * fTemp113) + (fTemp238 + (fTemp246 + ((((fTemp235 + (fTemp139 + fTemp247)) + (1.6227516985606004e-17 * fTemp49)) + fTemp248) + fTemp237)))))) - (fTemp244 + ((0.03338881006334882 * fTemp121) + (fTemp232 + (((fTemp240 + ((0.0333888100633488 * fTemp37) + (fTemp245 + (fTemp241 + (1.8737922599837705e-17 * fTemp21))))) + (0.03714270640165971 * fTemp81)) + fTemp167))))));
			fRec249[0] = max((fRec249[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp251))))));
			fbargraph49 = fRec249[0];
			output13[i] = (FAUSTFLOAT)fTemp251;
			double fTemp252 = (fRec1[0] * ((fTemp151 + (fTemp135 + (fTemp154 + (fTemp137 + (fTemp138 + (fTemp148 + (fTemp146 + (fTemp152 + (fTemp145 + (fTemp143 + fTemp234)))))))))) - (fTemp134 + (fTemp136 + (fTemp155 + (fTemp150 + (fTemp149 + (fTemp153 + (fTemp147 + (fTemp140 + fTemp144))))))))));
			fRec250[0] = max((fRec250[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp252))))));
			fbargraph50 = fRec250[0];
			output14[i] = (FAUSTFLOAT)fTemp252;
			double fTemp253 = (fRec1[0] * ((fTemp188 + (fTemp165 + (fTemp192 + (fTemp166 + (fTemp167 + (fTemp176 + (fTemp177 + (fTemp178 + (fTemp136 + (fTemp154 + (fTemp137 + (fTemp196 + (fTemp147 + (fTemp181 + (fTemp152 + (fTemp143 + (fTemp139 + fTemp180))))))))))))))))) - (fTemp183 + (fTemp160 + (fTemp164 + (fTemp184 + (fTemp193 + (fTemp171 + (fTemp194 + (fTemp175 + (fTemp134 + (fTemp195 + (fTemp197 + (fTemp138 + (fTemp182 + (fTemp153 + (fTemp145 + (fTemp140 + fTemp179))))))))))))))))));
			fRec251[0] = max((fRec251[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp253))))));
			fbargraph51 = fRec251[0];
			output15[i] = (FAUSTFLOAT)fTemp253;
			double fTemp254 = (fTemp139 + fTemp143);
			double fTemp255 = (fRec1[0] * ((fTemp207 + (fTemp213 + (fTemp211 + (fTemp205 + (fTemp203 + (fTemp209 + (fTemp199 + (fTemp201 + (fTemp155 + (fTemp154 + (fTemp137 + (fTemp150 + (fTemp138 + (fTemp149 + (fTemp152 + (fTemp145 + (fTemp144 + fTemp254))))))))))))))))) - (fTemp206 + (fTemp212 + (fTemp204 + (fTemp210 + (fTemp202 + (fTemp208 + (fTemp151 + (fTemp134 + (fTemp135 + (fTemp136 + (fTemp153 + (fTemp148 + (fTemp147 + (fTemp146 + (fTemp140 + fTemp142)))))))))))))))));
			fRec252[0] = max((fRec252[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp255))))));
			fbargraph52 = fRec252[0];
			output16[i] = (FAUSTFLOAT)fTemp255;
			double fTemp256 = (fRec1[0] * ((fTemp160 + (fTemp184 + (fTemp223 + (fTemp193 + (fTemp216 + (fTemp194 + (fTemp175 + (fTemp218 + (fTemp222 + (fTemp136 + (fTemp221 + (fTemp154 + (fTemp137 + (fTemp228 + (fTemp219 + (fTemp147 + (fTemp152 + (fTemp227 + fTemp254)))))))))))))))))) - (fTemp215 + (fTemp165 + (fTemp192 + (fTemp224 + (fTemp167 + (fTemp217 + (fTemp177 + (fTemp229 + (fTemp134 + (fTemp220 + (fTemp138 + (fTemp153 + (fTemp225 + (fTemp145 + (fTemp226 + fTemp140)))))))))))))))));
			fRec253[0] = max((fRec253[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp256))))));
			fbargraph53 = fRec253[0];
			output17[i] = (FAUSTFLOAT)fTemp256;
			double fTemp257 = (fTemp1 + fTemp5);
			double fTemp258 = (0.02109375 * fTemp257);
			double fTemp259 = (0.027231914153020904 * fTemp159);
			double fTemp260 = (0.041597435974919175 * fTemp163);
			double fTemp261 = (4.877341144075983e-18 * fTemp187);
			double fTemp262 = (0.02941382905783859 * fTemp191);
			double fTemp263 = (2.5471083409710855e-18 * fTemp118);
			double fTemp264 = (0.016982082124440745 * fTemp121);
			double fTemp265 = (0.027231914153020897 * fTemp106);
			double fTemp266 = (0.021093749999999994 * fTemp109);
			double fTemp267 = (1.531697961461448e-17 * fTemp170);
			double fTemp268 = (0.028161597282885743 * fTemp113);
			double fTemp269 = (0.031444705933590805 * fTemp129);
			double fTemp270 = (0.013154264250377145 * fTemp125);
			double fTemp271 = (2.5471083409710867e-18 * fTemp33);
			double fTemp272 = (1.6674738251076273e-18 * fTemp21);
			double fTemp273 = (0.02723191415302091 * fTemp17);
			double fTemp274 = (5.236595479505967e-18 * fTemp13);
			double fTemp275 = ((0.021093749999999998 * fTemp9) + fTemp274);
			double fTemp276 = (0.013154264250377147 * fTemp29);
			double fTemp277 = (1.925432923585088e-18 * fTemp49);
			double fTemp278 = (9.37894502882144e-34 * fTemp69);
			double fTemp279 = (0.01111738239902945 * (fTemp45 + fTemp174));
			double fTemp280 = (0.02607257280619568 * fTemp98);
			double fTemp281 = (0.012594249175901169 * fTemp102);
			double fTemp282 = (0.012594249175901174 * fTemp81);
			double fTemp283 = (0.039826512815546306 * fTemp77);
			double fTemp284 = (0.02816159728288574 * fTemp73);
			double fTemp285 = (0.026072572806195685 * fTemp65);
			double fTemp286 = (0.021480823570105497 * fTemp25);
			double fTemp287 = (0.01698208212444074 * fTemp37);
			double fTemp288 = (0.024609375000000013 * fTemp41);
			double fTemp289 = (0.029413829057838584 * fTemp53);
			double fTemp290 = (0.02079871798745958 * fTemp57);
			double fTemp291 = (0.0067319093514117785 * fTemp61);
			double fTemp292 = (fRec1[0] * ((fTemp258 + (fTemp259 + (fTemp260 + (fTemp261 + (fTemp262 + (fTemp263 + (fTemp264 + (fTemp265 + (fTemp266 + (fTemp267 + (fTemp268 + (fTemp269 + (fTemp270 + (((fTemp271 + ((fTemp272 + (fTemp273 + fTemp275)) + fTemp276)) + fTemp277) + fTemp278)))))))))))))) - (fTemp279 + (fTemp280 + (fTemp281 + (fTemp282 + (fTemp283 + (fTemp284 + (fTemp285 + (((((fTemp286 + fTemp287) + fTemp288) + fTemp289) + fTemp290) + fTemp291))))))))));
			fRec254[0] = max((fRec254[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp292))))));
			fbargraph54 = fRec254[0];
			output18[i] = (FAUSTFLOAT)fTemp292;
			double fTemp293 = (0.01111738239902945 * (fTemp45 - fTemp174));
			double fTemp294 = (0.02723191415302091 * fTemp159);
			double fTemp295 = (1.4632023432227946e-17 * fTemp187);
			double fTemp296 = (0.02941382905783858 * fTemp191);
			double fTemp297 = (7.641325022913257e-18 * fTemp118);
			double fTemp298 = (0.01698208212444074 * fTemp121);
			double fTemp299 = (0.021093749999999998 * fTemp109);
			double fTemp300 = (0.028161597282885732 * fTemp113);
			double fTemp301 = (0.013154264250377147 * fTemp125);
			double fTemp302 = (0.012594249175901155 * fTemp81);
			double fTemp303 = (0.028161597282885746 * fTemp73);
			double fTemp304 = (0.029413829057838594 * fTemp53);
			double fTemp305 = (0.01698208212444075 * fTemp37);
			double fTemp306 = (0.02607257280619568 * fTemp65);
			double fTemp307 = (0.026072572806195685 * fTemp98);
			double fTemp308 = (0.012594249175901188 * fTemp102);
			double fTemp309 = (2.813683508646432e-33 * fTemp69);
			double fTemp310 = (5.776298770755264e-18 * fTemp49);
			double fTemp311 = (7.641325022913258e-18 * fTemp33);
			double fTemp312 = (0.013154264250377145 * fTemp29);
			double fTemp313 = (5.002421475322882e-18 * fTemp21);
			double fTemp314 = (0.021093749999999994 * fTemp9);
			double fTemp315 = (0.027231914153020904 * fTemp17);
			double fTemp316 = (fRec1[0] * ((fTemp293 + (fTemp258 + (fTemp294 + (fTemp295 + (fTemp296 + (fTemp297 + (fTemp298 + (fTemp299 + (fTemp300 + (fTemp301 + (fTemp302 + (fTemp303 + ((fTemp304 + (fTemp274 + fTemp305)) + fTemp306))))))))))))) - (fTemp260 + (fTemp307 + (fTemp308 + (fTemp265 + (fTemp267 + (fTemp269 + (fTemp283 + (fTemp309 + (fTemp291 + (fTemp290 + (fTemp310 + (fTemp288 + (fTemp311 + (fTemp312 + (fTemp286 + (fTemp313 + (fTemp314 + fTemp315)))))))))))))))))));
			fRec255[0] = max((fRec255[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp316))))));
			fbargraph55 = fRec255[0];
			output19[i] = (FAUSTFLOAT)fTemp316;
			double fTemp317 = (2.4386705720379917e-17 * fTemp187);
			double fTemp318 = (0.012594249175901172 * fTemp102);
			double fTemp319 = (1.2735541704855428e-17 * fTemp118);
			double fTemp320 = (0.01111738239902945 * fTemp174);
			double fTemp321 = (0.012594249175901169 * fTemp81);
			double fTemp322 = (0.028161597282885704 * fTemp73);
			double fTemp323 = (0.02607257280619569 * fTemp65);
			double fTemp324 = (0.02941382905783855 * fTemp53);
			double fTemp325 = (9.62716461792544e-18 * fTemp49);
			double fTemp326 = (0.011117382399029452 * fTemp45);
			double fTemp327 = (0.01698208212444072 * fTemp37);
			double fTemp328 = (8.337369125538138e-18 * fTemp21);
			double fTemp329 = (1.273554170485543e-17 * fTemp33);
			double fTemp330 = (4.6894725144107197e-33 * fTemp69);
			double fTemp331 = (0.02941382905783863 * fTemp191);
			double fTemp332 = (0.016982082124440766 * fTemp121);
			double fTemp333 = (0.02816159728288578 * fTemp113);
			double fTemp334 = (0.013154264250377148 * fTemp29);
			double fTemp335 = (fRec1[0] * ((fTemp260 + (fTemp280 + (fTemp317 + (fTemp318 + (fTemp319 + (fTemp265 + (fTemp267 + (fTemp320 + (fTemp269 + (fTemp321 + (fTemp322 + ((fTemp323 + (fTemp324 + (fTemp325 + (fTemp326 + (fTemp327 + ((fTemp274 + fTemp328) + fTemp329)))))) + fTemp330)))))))))))) - ((0.02109375 * (fTemp9 - fTemp257)) + (fTemp259 + (fTemp331 + (fTemp332 + (fTemp266 + (fTemp333 + (fTemp270 + (fTemp283 + (fTemp291 + (fTemp290 + (fTemp288 + (fTemp334 + (fTemp273 + fTemp286)))))))))))))));
			fRec256[0] = max((fRec256[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp335))))));
			fbargraph56 = fRec256[0];
			output20[i] = (FAUSTFLOAT)fTemp335;
			double fTemp336 = (fTemp1 - fTemp109);
			double fTemp337 = (0.02607257280619569 * fTemp98);
			double fTemp338 = (1.7829758386797597e-17 * fTemp118);
			double fTemp339 = (fTemp274 + fTemp314);
			double fTemp340 = (0.011117382399029452 * fTemp174);
			double fTemp341 = (0.012594249175901195 * fTemp102);
			double fTemp342 = (3.4141388008531875e-17 * fTemp187);
			double fTemp343 = (0.029413829057838594 * fTemp191);
			double fTemp344 = (0.01698208212444075 * fTemp121);
			double fTemp345 = (0.028161597282885746 * fTemp113);
			double fTemp346 = (0.013154264250377148 * fTemp125);
			double fTemp347 = (0.012594249175901146 * fTemp81);
			double fTemp348 = (6.565261520175007e-33 * fTemp69);
			double fTemp349 = (0.01111738239902945 * fTemp45);
			double fTemp350 = (1.7829758386797603e-17 * fTemp33);
			double fTemp351 = (1.167231677575339e-17 * fTemp21);
			double fTemp352 = (1.3478030465095617e-17 * fTemp49);
			double fTemp353 = (fRec1[0] * (((0.02109375 * (fTemp5 + fTemp336)) + (fTemp337 + (((fTemp338 + ((fTemp312 + (fTemp315 + fTemp339)) + fTemp340)) + fTemp341) + fTemp342))) - (fTemp294 + (fTemp260 + (fTemp343 + (fTemp344 + (fTemp265 + (fTemp267 + (fTemp345 + (fTemp269 + (fTemp346 + (fTemp347 + (fTemp283 + (fTemp284 + (fTemp348 + (fTemp306 + (fTemp291 + (fTemp290 + (fTemp289 + ((fTemp349 + (fTemp288 + (fTemp287 + (fTemp350 + (fTemp286 + fTemp351))))) + fTemp352))))))))))))))))))));
			fRec257[0] = max((fRec257[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp353))))));
			fbargraph57 = fRec257[0];
			output21[i] = (FAUSTFLOAT)fTemp353;
			double fTemp354 = (fTemp1 - fTemp5);
			double fTemp355 = (0.02109375 * fTemp354);
			double fTemp356 = (fRec1[0] * ((fTemp355 + (fTemp279 + (fTemp263 + (fTemp264 + (fTemp265 + (fTemp266 + (fTemp268 + (fTemp269 + (fTemp270 + (fTemp283 + (fTemp291 + (fTemp289 + (fTemp277 + (fTemp276 + (fTemp286 + (fTemp275 + fTemp272)))))))))))))))) - (fTemp259 + (fTemp260 + (fTemp280 + (fTemp261 + (fTemp281 + (fTemp262 + (fTemp267 + (fTemp282 + (fTemp284 + (fTemp278 + (fTemp285 + (fTemp290 + (fTemp288 + (fTemp287 + (fTemp273 + fTemp271)))))))))))))))));
			fRec258[0] = max((fRec258[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp356))))));
			fbargraph58 = fRec258[0];
			output22[i] = (FAUSTFLOAT)fTemp356;
			double fTemp357 = (fRec1[0] * ((fTemp355 + (fTemp260 + (fTemp297 + (fTemp298 + (fTemp299 + (fTemp267 + (fTemp300 + (fTemp301 + (fTemp302 + (fTemp283 + (fTemp303 + (fTemp309 + (fTemp306 + (fTemp291 + (fTemp305 + (fTemp311 + (fTemp286 + (fTemp274 + fTemp315)))))))))))))))))) - (fTemp293 + (fTemp294 + (fTemp307 + (fTemp295 + (fTemp308 + (fTemp296 + (fTemp265 + (fTemp269 + (fTemp290 + (fTemp304 + (fTemp310 + (fTemp288 + (fTemp312 + (fTemp314 + fTemp313))))))))))))))));
			fRec259[0] = max((fRec259[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp357))))));
			fbargraph59 = fRec259[0];
			output23[i] = (FAUSTFLOAT)fTemp357;
			double fTemp358 = (fRec1[0] * ((fTemp259 + (fTemp280 + (fTemp318 + (fTemp331 + (fTemp319 + (fTemp265 + (fTemp269 + (fTemp321 + (fTemp283 + (fTemp322 + (fTemp323 + (fTemp291 + (fTemp325 + (fTemp327 + (fTemp286 + (fTemp328 + (fTemp274 + fTemp273))))))))))))))))) - ((0.02109375 * (fTemp9 - fTemp354)) + (fTemp260 + (fTemp317 + (fTemp332 + (fTemp266 + (fTemp267 + (fTemp333 + (fTemp320 + (fTemp270 + (fTemp330 + (fTemp290 + (fTemp324 + (fTemp326 + (fTemp288 + (fTemp334 + fTemp329)))))))))))))))));
			fRec260[0] = max((fRec260[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp358))))));
			fbargraph60 = fRec260[0];
			output24[i] = (FAUSTFLOAT)fTemp358;
			double fTemp359 = (fRec1[0] * ((fTemp294 + (fTemp260 + (fTemp337 + (fTemp341 + (fTemp343 + (fTemp338 + (fTemp267 + (fTemp283 + (fTemp348 + (fTemp291 + (fTemp289 + (fTemp349 + (fTemp350 + (fTemp312 + (fTemp286 + fTemp339))))))))))))))) - ((0.02109375 * (fTemp5 - fTemp336)) + (fTemp342 + (fTemp344 + (fTemp265 + (fTemp345 + (fTemp340 + (fTemp269 + (fTemp346 + (fTemp347 + (fTemp284 + (fTemp306 + (fTemp290 + (fTemp352 + (fTemp288 + (fTemp287 + (fTemp315 + fTemp351))))))))))))))))));
			fRec261[0] = max((fRec261[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp359))))));
			fbargraph61 = fRec261[0];
			output25[i] = (FAUSTFLOAT)fTemp359;
			double fTemp360 = (0.021308452520676528 * (fTemp17 + fTemp159));
			double fTemp361 = (0.010535180889746065 * (fTemp9 + fTemp109));
			double fTemp362 = (0.016998264729033836 * fTemp163);
			double fTemp363 = (0.03519123219323861 * fTemp98);
			double fTemp364 = (0.006277025248674522 * fTemp191);
			double fTemp365 = (0.002313170838826743 * fTemp121);
			double fTemp366 = (0.007102817506892165 * fTemp106);
			double fTemp367 = (0.04100135723151817 * fTemp170);
			double fTemp368 = (0.012205326872422687 * fTemp113);
			double fTemp369 = (0.03558738810271335 * fTemp174);
			double fTemp370 = (0.02907849643359095 * fTemp129);
			double fTemp371 = (0.030460165261409405 * fTemp125);
			double fTemp372 = (0.014254712301587407 * fTemp41);
			double fTemp373 = (0.030460165261409402 * fTemp29);
			double fTemp374 = (0.02017333553791887 * fTemp1);
			double fTemp375 = (0.03160554266923825 * fTemp5);
			double fTemp376 = (fTemp374 + fTemp375);
			double fTemp377 = (0.03280650879687195 * fTemp13);
			double fTemp378 = (fTemp376 + fTemp377);
			double fTemp379 = (0.0263335984839893 * fTemp25);
			double fTemp380 = (fTemp378 + fTemp379);
			double fTemp381 = (0.03558738810271334 * fTemp45);
			double fTemp382 = (0.0351912321932386 * fTemp65);
			double fTemp383 = (1.427539198259021e-18 * fTemp187);
			double fTemp384 = (0.00046786183841729075 * fTemp102);
			double fTemp385 = (4.758463994196736e-19 * fTemp118);
			double fTemp386 = (0.0004678618384172905 * fTemp81);
			double fTemp387 = (0.012205326872422696 * fTemp73);
			double fTemp388 = (6.593521123148732e-18 * fTemp69);
			double fTemp389 = (0.001479509039672308 * fTemp57);
			double fTemp390 = (0.0062770252486745275 * fTemp53);
			double fTemp391 = (0.002313170838826745 * fTemp37);
			double fTemp392 = (1.1422201709353029e-18 * fTemp21);
			double fTemp393 = (2.7335294515958808e-18 * fTemp33);
			double fTemp394 = (4.676178873339891e-18 * fTemp49);
			double fTemp395 = (0.0007502480158729096 * fTemp61);
			double fTemp396 = (0.004438527119016925 * fTemp77);
			double fTemp397 = (fRec1[0] * ((fTemp360 + (fTemp361 + (fTemp362 + (fTemp363 + (fTemp364 + (fTemp365 + (fTemp366 + (fTemp367 + (fTemp368 + (fTemp369 + (fTemp370 + (fTemp371 + (((fTemp372 + (fTemp373 + fTemp380)) + fTemp381) + fTemp382))))))))))))) - (fTemp383 + (fTemp384 + (fTemp385 + (fTemp386 + ((fTemp387 + (fTemp388 + ((fTemp389 + (fTemp390 + ((fTemp391 + (fTemp392 + fTemp393)) + fTemp394))) + fTemp395))) + fTemp396)))))));
			fRec262[0] = max((fRec262[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp397))))));
			fbargraph62 = fRec262[0];
			output26[i] = (FAUSTFLOAT)fTemp397;
			double fTemp398 = (1.63068840876984e-18 * fTemp187);
			double fTemp399 = (0.006277025248674523 * fTemp191);
			double fTemp400 = (5.435628029232799e-19 * fTemp118);
			double fTemp401 = (0.012205326872422689 * fTemp113);
			double fTemp402 = (0.00046786183841728994 * fTemp81);
			double fTemp403 = (0.0023131708388267444 * fTemp37);
			double fTemp404 = (0.006277025248674527 * fTemp53);
			double fTemp405 = (0.021308452520676528 * (fTemp17 - fTemp159));
			double fTemp406 = (0.010535180889746065 * (fTemp9 - fTemp109));
			double fTemp407 = (0.00046786183841729124 * fTemp102);
			double fTemp408 = (7.531827134141385e-18 * fTemp69);
			double fTemp409 = (5.341633137212233e-18 * fTemp49);
			double fTemp410 = (3.1225305737205953e-18 * fTemp33);
			double fTemp411 = (1.3047664087114909e-18 * fTemp21);
			double fTemp412 = (fRec1[0] * ((fTemp363 + (fTemp398 + (fTemp399 + (fTemp400 + (fTemp365 + (fTemp401 + (fTemp369 + (fTemp371 + (fTemp402 + (fTemp387 + ((fTemp372 + (fTemp380 + fTemp403)) + fTemp404))))))))))) - (fTemp405 + (fTemp406 + (fTemp362 + (fTemp407 + (fTemp366 + (fTemp367 + (fTemp370 + (fTemp396 + (fTemp408 + (fTemp382 + (fTemp395 + (fTemp389 + (fTemp409 + (fTemp381 + (fTemp410 + (fTemp373 + fTemp411))))))))))))))))));
			fRec263[0] = max((fRec263[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp412))))));
			fbargraph63 = fRec263[0];
			output27[i] = (FAUSTFLOAT)fTemp412;
			double fTemp413 = (2.7178140146164e-18 * fTemp187);
			double fTemp414 = (0.00046786183841729064 * fTemp102);
			double fTemp415 = (9.059380048721333e-19 * fTemp118);
			double fTemp416 = (0.00046786183841729053 * fTemp81);
			double fTemp417 = (0.012205326872422675 * fTemp73);
			double fTemp418 = (1.2553045223568977e-17 * fTemp69);
			double fTemp419 = (0.006277025248674517 * fTemp53);
			double fTemp420 = (8.902721895353721e-18 * fTemp49);
			double fTemp421 = (0.002313170838826741 * fTemp37);
			double fTemp422 = (5.204217622867659e-18 * fTemp33);
			double fTemp423 = (2.1746106811858183e-18 * fTemp21);
			double fTemp424 = (0.021308452520676528 * fTemp159);
			double fTemp425 = (0.0351912321932386 * fTemp98);
			double fTemp426 = (0.0062770252486745335 * fTemp191);
			double fTemp427 = (0.002313170838826747 * fTemp121);
			double fTemp428 = (0.010535180889746065 * fTemp109);
			double fTemp429 = (0.01220532687242271 * fTemp113);
			double fTemp430 = (0.03558738810271334 * fTemp174);
			double fTemp431 = (0.030460165261409402 * fTemp125);
			double fTemp432 = (0.03519123219323861 * fTemp65);
			double fTemp433 = (0.010535180889746067 * fTemp9);
			double fTemp434 = (0.02130845252067653 * fTemp17);
			double fTemp435 = (0.030460165261409412 * fTemp29);
			double fTemp436 = (0.03558738810271335 * fTemp45);
			double fTemp437 = (fRec1[0] * ((fTemp362 + (fTemp413 + (fTemp414 + (fTemp415 + (fTemp366 + (fTemp367 + (fTemp370 + (fTemp416 + (fTemp417 + (fTemp418 + (fTemp419 + (fTemp420 + (fTemp372 + (fTemp421 + (fTemp422 + (fTemp379 + (fTemp378 + fTemp423))))))))))))))))) - (fTemp424 + (fTemp425 + (fTemp426 + (fTemp427 + (fTemp428 + (fTemp429 + (fTemp430 + (fTemp431 + (fTemp396 + (fTemp432 + (fTemp395 + (fTemp389 + (((fTemp433 + fTemp434) + fTemp435) + fTemp436)))))))))))))));
			fRec264[0] = max((fRec264[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp437))))));
			fbargraph64 = fRec264[0];
			output28[i] = (FAUSTFLOAT)fTemp437;
			double fTemp438 = (3.80493962046296e-18 * fTemp187);
			double fTemp439 = (0.0004678618384172915 * fTemp102);
			double fTemp440 = (1.2683132068209864e-18 * fTemp118);
			double fTemp441 = (0.021308452520676528 * fTemp17);
			double fTemp442 = (0.010535180889746065 * fTemp9);
			double fTemp443 = (0.02130845252067653 * fTemp159);
			double fTemp444 = (0.006277025248674527 * fTemp191);
			double fTemp445 = (0.0023131708388267444 * fTemp121);
			double fTemp446 = (0.010535180889746067 * fTemp109);
			double fTemp447 = (0.012205326872422696 * fTemp113);
			double fTemp448 = (0.030460165261409412 * fTemp125);
			double fTemp449 = (0.00046786183841728967 * fTemp81);
			double fTemp450 = (1.7574263312996565e-17 * fTemp69);
			double fTemp451 = (0.006277025248674525 * fTemp53);
			double fTemp452 = (0.0023131708388267436 * fTemp37);
			double fTemp453 = (3.044454953660145e-18 * fTemp21);
			double fTemp454 = (7.285904672014723e-18 * fTemp33);
			double fTemp455 = (1.246381065349521e-17 * fTemp49);
			double fTemp456 = (0.01220532687242269 * fTemp73);
			double fTemp457 = (fRec1[0] * ((fTemp438 + (fTemp439 + (fTemp440 + (fTemp382 + (fTemp381 + (fTemp372 + (fTemp373 + (fTemp379 + (fTemp441 + (fTemp377 + (fTemp376 + fTemp442))))))))))) - (fTemp443 + (fTemp362 + (fTemp363 + (fTemp444 + (fTemp445 + (fTemp366 + (fTemp446 + (fTemp367 + (fTemp447 + (fTemp369 + (fTemp370 + (fTemp448 + (fTemp449 + (fTemp396 + ((fTemp450 + (fTemp395 + (fTemp389 + (fTemp451 + ((fTemp452 + (fTemp453 + fTemp454)) + fTemp455))))) + fTemp456)))))))))))))))));
			fRec265[0] = max((fRec265[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp457))))));
			fbargraph65 = fRec265[0];
			output29[i] = (FAUSTFLOAT)fTemp457;
			double fTemp458 = (0.007102817506892191 * fTemp159);
			double fTemp459 = (0.016998264729033888 * fTemp163);
			double fTemp460 = (0.035508216952135595 * fTemp187);
			double fTemp461 = (0.03696108523496622 * fTemp102);
			double fTemp462 = (0.027200442744256398 * fTemp191);
			double fTemp463 = (0.035508216952135616 * fTemp118);
			double fTemp464 = (0.03007122090474782 * fTemp121);
			double fTemp465 = (0.021308452520676584 * fTemp106);
			double fTemp466 = (0.010535180889746094 * fTemp109);
			double fTemp467 = (0.010356563277706147 * fTemp77);
			double fTemp468 = (0.010356563277706152 * fTemp57);
			double fTemp469 = (0.018831075746023607 * fTemp53);
			double fTemp470 = (0.02081853754944074 * fTemp37);
			double fTemp471 = (0.028411270027568724 * fTemp21);
			double fTemp472 = (0.010535180889746075 * fTemp5);
			double fTemp473 = (fTemp374 + fTemp472);
			double fTemp474 = (0.03160554266923824 * fTemp9);
			double fTemp475 = (0.021308452520676542 * fTemp17);
			double fTemp476 = (0.022664352972045144 * fTemp33);
			double fTemp477 = (0.004251405423280436 * fTemp41);
			double fTemp478 = (0.023090966710758377 * fTemp61);
			double fTemp479 = (0.0009685660236671096 * fTemp98);
			double fTemp480 = (0.020500678615759146 * fTemp170);
			double fTemp481 = (0.00453340712404275 * fTemp113);
			double fTemp482 = (0.010280801007450534 * fTemp174);
			double fTemp483 = (0.006710422253905637 * fTemp129);
			double fTemp484 = (0.005375323281425205 * fTemp125);
			double fTemp485 = (0.027334238154345478 * fTemp69);
			double fTemp486 = (0.008947229671874163 * fTemp49);
			double fTemp487 = (0.01612596984427559 * fTemp29);
			double fTemp488 = (0.01640325439843596 * fTemp13);
			double fTemp489 = (0.020481687709769415 * fTemp25);
			double fTemp490 = (fTemp488 + fTemp489);
			double fTemp491 = (0.03084240302235156 * fTemp45);
			double fTemp492 = (0.0029056980710013246 * fTemp65);
			double fTemp493 = (0.0031385126243372798 * fTemp73);
			double fTemp494 = (0.0014035855152519602 * fTemp81);
			double fTemp495 = (fRec1[0] * ((fTemp458 + (fTemp459 + (fTemp460 + (fTemp461 + (fTemp462 + (fTemp463 + (fTemp464 + (fTemp465 + (fTemp466 + (fTemp467 + ((fTemp468 + (fTemp469 + ((fTemp470 + ((fTemp471 + ((fTemp473 + fTemp474) + fTemp475)) + fTemp476)) + fTemp477))) + fTemp478))))))))))) - (fTemp479 + (fTemp480 + (fTemp481 + (fTemp482 + (fTemp483 + (fTemp484 + (((fTemp485 + ((fTemp486 + ((fTemp487 + fTemp490) + fTemp491)) + fTemp492)) + fTemp493) + fTemp494)))))))));
			fRec266[0] = max((fRec266[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp495))))));
			fbargraph66 = fRec266[0];
			output30[i] = (FAUSTFLOAT)fTemp495;
			double fTemp496 = (0.021308452520676546 * fTemp159);
			double fTemp497 = (0.016998264729033867 * fTemp163);
			double fTemp498 = (0.021308452520676556 * fTemp106);
			double fTemp499 = (0.03160554266923824 * fTemp109);
			double fTemp500 = (0.003138512624337286 * fTemp113);
			double fTemp501 = (0.010356563277706213 * fTemp77);
			double fTemp502 = (0.004533407124042746 * fTemp73);
			double fTemp503 = (0.027334238154345495 * fTemp69);
			double fTemp504 = (0.010356563277706217 * fTemp57);
			double fTemp505 = (0.00894722967187417 * fTemp49);
			double fTemp506 = (0.01053518088974608 * fTemp9);
			double fTemp507 = (0.007102817506892183 * fTemp17);
			double fTemp508 = (0.03696108523496622 * fTemp81);
			double fTemp509 = (0.0029056980710013254 * fTemp98);
			double fTemp510 = (0.03550821695213558 * fTemp187);
			double fTemp511 = (0.0014035855152518813 * fTemp102);
			double fTemp512 = (0.01883107574602364 * fTemp191);
			double fTemp513 = (0.035508216952135595 * fTemp118);
			double fTemp514 = (0.020818537549440783 * fTemp121);
			double fTemp515 = (0.02050067861575912 * fTemp170);
			double fTemp516 = (0.030842403022351562 * fTemp174);
			double fTemp517 = (0.006710422253905627 * fTemp129);
			double fTemp518 = (0.01612596984427559 * fTemp125);
			double fTemp519 = (0.027200442744256374 * fTemp53);
			double fTemp520 = (0.030071220904747797 * fTemp37);
			double fTemp521 = (0.005375323281425198 * fTemp29);
			double fTemp522 = (0.02841127002756874 * fTemp21);
			double fTemp523 = (0.022664352972045162 * fTemp33);
			double fTemp524 = (0.010280801007450522 * fTemp45);
			double fTemp525 = (0.0009685660236671086 * fTemp65);
			double fTemp526 = (fRec1[0] * ((fTemp496 + (fTemp497 + (fTemp498 + (fTemp499 + (fTemp500 + ((fTemp501 + (fTemp502 + (fTemp503 + (fTemp478 + (fTemp504 + (fTemp505 + (fTemp477 + ((fTemp473 + fTemp506) + fTemp507)))))))) + fTemp508)))))) - (fTemp509 + (fTemp510 + (fTemp511 + (fTemp512 + (fTemp513 + (fTemp514 + (fTemp515 + (fTemp516 + (fTemp517 + (fTemp518 + ((fTemp519 + ((fTemp520 + ((fTemp521 + (fTemp489 + (fTemp488 + fTemp522))) + fTemp523)) + fTemp524)) + fTemp525)))))))))))));
			fRec267[0] = max((fRec267[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp526))))));
			fbargraph67 = fRec267[0];
			output31[i] = (FAUSTFLOAT)fTemp526;
			double fTemp527 = (0.035508216952135574 * fTemp187);
			double fTemp528 = (0.020500678615759115 * fTemp170);
			double fTemp529 = (0.0031385126243372867 * fTemp113);
			double fTemp530 = (0.006710422253905626 * fTemp129);
			double fTemp531 = (0.01035656327770623 * fTemp77);
			double fTemp532 = (0.0009685660236671081 * fTemp65);
			double fTemp533 = (0.010356563277706236 * fTemp57);
			double fTemp534 = (0.027200442744256363 * fTemp53);
			double fTemp535 = (0.008947229671874172 * fTemp49);
			double fTemp536 = (0.005375323281425197 * fTemp29);
			double fTemp537 = (0.03007122090474779 * fTemp37);
			double fTemp538 = (0.010280801007450517 * fTemp45);
			double fTemp539 = (0.0273342381543455 * fTemp69);
			double fTemp540 = (0.016998264729033864 * fTemp163);
			double fTemp541 = (0.002905698071001326 * fTemp98);
			double fTemp542 = (0.0014035855152518585 * fTemp102);
			double fTemp543 = (0.01883107574602365 * fTemp191);
			double fTemp544 = (0.02081853754944079 * fTemp121);
			double fTemp545 = (0.03084240302235157 * fTemp174);
			double fTemp546 = (0.016125969844275594 * fTemp125);
			double fTemp547 = (0.007102817506892181 * fTemp17);
			double fTemp548 = (0.010535180889746079 * fTemp9);
			double fTemp549 = (0.028411270027568745 * fTemp21);
			double fTemp550 = (0.0045334071240427445 * fTemp73);
			double fTemp551 = (fRec1[0] * ((fTemp527 + (fTemp513 + (fTemp499 + (fTemp528 + (fTemp529 + (fTemp530 + (fTemp531 + ((fTemp532 + (fTemp478 + (fTemp533 + (fTemp534 + (fTemp535 + ((fTemp477 + ((fTemp473 + fTemp536) + fTemp537)) + fTemp538)))))) + fTemp539)))))))) - ((0.021308452520676553 * (fTemp106 - fTemp159)) + (fTemp540 + (fTemp541 + (fTemp542 + (fTemp543 + (fTemp544 + (fTemp545 + (fTemp546 + (fTemp508 + ((fTemp523 + (fTemp489 + ((fTemp547 + (fTemp488 + fTemp548)) + fTemp549))) + fTemp550))))))))))));
			fRec268[0] = max((fRec268[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp551))))));
			fbargraph68 = fRec268[0];
			output32[i] = (FAUSTFLOAT)fTemp551;
			double fTemp552 = (0.007102817506892193 * fTemp159);
			double fTemp553 = (0.027200442744256408 * fTemp191);
			double fTemp554 = (0.03007122090474784 * fTemp121);
			double fTemp555 = (0.010535180889746096 * fTemp109);
			double fTemp556 = (0.0014035855152519582 * fTemp81);
			double fTemp557 = (0.022664352972045138 * fTemp33);
			double fTemp558 = (0.028411270027568717 * fTemp21);
			double fTemp559 = (0.010356563277706144 * fTemp57);
			double fTemp560 = (0.0031385126243372767 * fTemp73);
			double fTemp561 = (0.010356563277706139 * fTemp77);
			double fTemp562 = (0.00096856602366711 * fTemp98);
			double fTemp563 = (0.0045334071240427515 * fTemp113);
			double fTemp564 = (0.010280801007450538 * fTemp174);
			double fTemp565 = (0.005375323281425206 * fTemp125);
			double fTemp566 = (0.018831075746023586 * fTemp53);
			double fTemp567 = (fTemp474 + fTemp488);
			double fTemp568 = (0.020818537549440724 * fTemp37);
			double fTemp569 = (0.027334238154345474 * fTemp69);
			double fTemp570 = (fRec1[0] * ((fTemp552 + (fTemp461 + (fTemp553 + (fTemp554 + (fTemp555 + (fTemp480 + (fTemp483 + (fTemp556 + (((fTemp492 + (fTemp478 + ((fTemp491 + (fTemp477 + (fTemp557 + (fTemp487 + (fTemp473 + fTemp558))))) + fTemp559))) + fTemp560) + fTemp561))))))))) - (fTemp459 + (fTemp562 + (fTemp460 + (fTemp463 + (fTemp465 + (fTemp563 + (fTemp564 + (fTemp565 + ((fTemp566 + (fTemp486 + ((fTemp489 + (fTemp475 + fTemp567)) + fTemp568))) + fTemp569)))))))))));
			fRec269[0] = max((fRec269[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp570))))));
			fbargraph69 = fRec269[0];
			output33[i] = (FAUSTFLOAT)fTemp570;
			double fTemp571 = (0.01699826472903388 * fTemp163);
			double fTemp572 = (0.0009685660236671092 * fTemp98);
			double fTemp573 = (0.03550821695213561 * fTemp118);
			double fTemp574 = (0.02130845252067657 * fTemp106);
			double fTemp575 = (0.010280801007450529 * fTemp174);
			double fTemp576 = (0.005375323281425202 * fTemp125);
			double fTemp577 = (0.0014035855152519131 * fTemp81);
			double fTemp578 = (0.010356563277706175 * fTemp77);
			double fTemp579 = (0.00313851262433728 * fTemp73);
			double fTemp580 = (0.010356563277706178 * fTemp57);
			double fTemp581 = (0.030842403022351562 * fTemp45);
			double fTemp582 = (0.02266435297204515 * fTemp33);
			double fTemp583 = (0.02841127002756873 * fTemp21);
			double fTemp584 = (0.0029056980710013254 * fTemp65);
			double fTemp585 = (0.007102817506892187 * fTemp159);
			double fTemp586 = (0.01053518088974609 * fTemp109);
			double fTemp587 = (0.020500678615759133 * fTemp170);
			double fTemp588 = (0.0067104222539056315 * fTemp129);
			double fTemp589 = (0.01883107574602361 * fTemp53);
			double fTemp590 = (0.020818537549440744 * fTemp37);
			double fTemp591 = (0.021308452520676546 * fTemp17);
			double fTemp592 = (0.008947229671874167 * fTemp49);
			double fTemp593 = (0.02733423815434548 * fTemp69);
			double fTemp594 = (fRec1[0] * ((fTemp571 + (fTemp572 + (fTemp460 + (fTemp573 + (fTemp574 + (fTemp481 + (fTemp575 + (fTemp576 + (fTemp577 + (fTemp578 + (fTemp579 + ((fTemp478 + (fTemp580 + (fTemp581 + (fTemp477 + (fTemp582 + (fTemp487 + (fTemp473 + fTemp583))))))) + fTemp584)))))))))))) - (fTemp585 + (fTemp461 + (fTemp462 + (fTemp464 + (fTemp586 + (fTemp587 + (fTemp588 + ((fTemp589 + ((fTemp590 + (fTemp489 + (fTemp567 + fTemp591))) + fTemp592)) + fTemp593))))))))));
			fRec270[0] = max((fRec270[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp594))))));
			fbargraph70 = fRec270[0];
			output34[i] = (FAUSTFLOAT)fTemp594;
			double fTemp595 = (0.016998264729033874 * fTemp163);
			double fTemp596 = (0.001403585515251904 * fTemp102);
			double fTemp597 = (0.01883107574602363 * fTemp191);
			double fTemp598 = (0.020818537549440772 * fTemp121);
			double fTemp599 = (0.021308452520676563 * fTemp106);
			double fTemp600 = (0.010356563277706192 * fTemp77);
			double fTemp601 = (0.02733423815434549 * fTemp69);
			double fTemp602 = (0.027200442744256377 * fTemp53);
			double fTemp603 = (0.010280801007450525 * fTemp45);
			double fTemp604 = (0.0300712209047478 * fTemp37);
			double fTemp605 = (0.0053753232814252 * fTemp29);
			double fTemp606 = (0.0103565632777062 * fTemp57);
			double fTemp607 = (0.0009685660236671088 * fTemp65);
			double fTemp608 = (0.03550821695213559 * fTemp187);
			double fTemp609 = (0.02050067861575913 * fTemp170);
			double fTemp610 = (0.003138512624337284 * fTemp113);
			double fTemp611 = (0.00671042225390563 * fTemp129);
			double fTemp612 = (0.022664352972045155 * fTemp33);
			double fTemp613 = (0.028411270027568734 * fTemp21);
			double fTemp614 = (0.007102817506892185 * fTemp17);
			double fTemp615 = (0.010535180889746086 * fTemp9);
			double fTemp616 = (0.004533407124042747 * fTemp73);
			double fTemp617 = (fRec1[0] * ((fTemp595 + (fTemp509 + (fTemp596 + (fTemp597 + (fTemp598 + (fTemp599 + (fTemp516 + (fTemp518 + (fTemp600 + (fTemp601 + ((fTemp478 + ((fTemp602 + (fTemp505 + (fTemp603 + (fTemp477 + (fTemp604 + (fTemp473 + fTemp605)))))) + fTemp606)) + fTemp607))))))))))) - (fTemp496 + (fTemp608 + (fTemp573 + (fTemp499 + (fTemp609 + (fTemp610 + (fTemp611 + (fTemp508 + ((fTemp612 + (fTemp489 + (fTemp613 + (fTemp614 + (fTemp488 + fTemp615))))) + fTemp616)))))))))));
			fRec271[0] = max((fRec271[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp617))))));
			fbargraph71 = fRec271[0];
			output35[i] = (FAUSTFLOAT)fTemp617;
			double fTemp618 = (0.004533407124042744 * fTemp73);
			double fTemp619 = (0.027334238154345505 * fTemp69);
			double fTemp620 = (0.008947229671874174 * fTemp49);
			double fTemp621 = (0.010535180889746074 * fTemp9);
			double fTemp622 = (0.0071028175068921775 * fTemp17);
			double fTemp623 = (0.010356563277706251 * fTemp57);
			double fTemp624 = (0.010356563277706244 * fTemp77);
			double fTemp625 = (0.006710422253905624 * fTemp129);
			double fTemp626 = (0.020500678615759105 * fTemp170);
			double fTemp627 = (0.020818537549440803 * fTemp121);
			double fTemp628 = (0.03550821695213559 * fTemp118);
			double fTemp629 = (0.018831075746023662 * fTemp191);
			double fTemp630 = (0.0014035855152518362 * fTemp102);
			double fTemp631 = (0.03550821695213557 * fTemp187);
			double fTemp632 = (0.0009685660236671078 * fTemp65);
			double fTemp633 = (0.03007122090474778 * fTemp37);
			double fTemp634 = (0.022664352972045165 * fTemp33);
			double fTemp635 = (0.028411270027568748 * fTemp21);
			double fTemp636 = (0.005375323281425194 * fTemp29);
			double fTemp637 = (0.010280801007450515 * fTemp45);
			double fTemp638 = (0.027200442744256356 * fTemp53);
			double fTemp639 = (0.0031385126243372885 * fTemp113);
			double fTemp640 = (0.021308452520676542 * fTemp106);
			double fTemp641 = (0.016998264729033857 * fTemp163);
			double fTemp642 = (0.021308452520676553 * fTemp159);
			double fTemp643 = (((((((((((((((fTemp618 + (fTemp619 + (fTemp478 + ((fTemp620 + (fTemp477 + ((fTemp473 + fTemp621) + fTemp622))) + fTemp623)))) + fTemp624) + fTemp508) + fTemp546) + fTemp625) + fTemp545) + fTemp626) + fTemp627) + fTemp628) + fTemp629) + fTemp630) + fTemp631) + fTemp541) - ((((((fTemp632 + (((fTemp633 + (fTemp634 + ((fTemp489 + (fTemp488 + fTemp635)) + fTemp636))) + fTemp637) + fTemp638)) + fTemp639) + fTemp499) + fTemp640) + fTemp641) + fTemp642)) * fRec1[0]);
			fRec272[0] = max((fRec272[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp643))))));
			fbargraph72 = fRec272[0];
			output36[i] = (FAUSTFLOAT)fTemp643;
			double fTemp644 = (0.0009685660236671116 * fTemp98);
			double fTemp645 = (0.020500678615759178 * fTemp170);
			double fTemp646 = (0.004533407124042753 * fTemp113);
			double fTemp647 = (0.010280801007450555 * fTemp174);
			double fTemp648 = (0.006710422253905646 * fTemp129);
			double fTemp649 = (0.005375323281425214 * fTemp125);
			double fTemp650 = (0.010356563277706059 * fTemp77);
			double fTemp651 = (0.010356563277706062 * fTemp57);
			double fTemp652 = (0.02081853754944071 * fTemp37);
			double fTemp653 = (0.028411270027568696 * fTemp21);
			double fTemp654 = (0.03160554266923823 * fTemp9);
			double fTemp655 = (0.02130845252067654 * fTemp17);
			double fTemp656 = (0.02266435297204512 * fTemp33);
			double fTemp657 = (0.018831075746023575 * fTemp53);
			double fTemp658 = (0.007102817506892205 * fTemp159);
			double fTemp659 = (0.016998264729033916 * fTemp163);
			double fTemp660 = (0.03550821695213562 * fTemp187);
			double fTemp661 = (0.03696108523496621 * fTemp102);
			double fTemp662 = (0.027200442744256415 * fTemp191);
			double fTemp663 = (0.03550821695213564 * fTemp118);
			double fTemp664 = (0.030071220904747842 * fTemp121);
			double fTemp665 = (0.021308452520676615 * fTemp106);
			double fTemp666 = (0.010535180889746113 * fTemp109);
			double fTemp667 = (0.0014035855152521124 * fTemp81);
			double fTemp668 = (0.0031385126243372746 * fTemp73);
			double fTemp669 = (0.02733423815434545 * fTemp69);
			double fTemp670 = (0.008947229671874156 * fTemp49);
			double fTemp671 = (0.016125969844275587 * fTemp29);
			double fTemp672 = (0.030842403022351552 * fTemp45);
			double fTemp673 = (0.002905698071001324 * fTemp65);
			double fTemp674 = (((fTemp644 + (fTemp645 + (fTemp646 + (fTemp647 + (fTemp648 + (fTemp649 + (fTemp650 + ((fTemp651 + (((fTemp652 + ((fTemp653 + ((fTemp654 + fTemp473) + fTemp655)) + fTemp656)) + fTemp477) + fTemp657)) + fTemp478)))))))) - (fTemp658 + (fTemp659 + (fTemp660 + (fTemp661 + (fTemp662 + (fTemp663 + (fTemp664 + (fTemp665 + (fTemp666 + (fTemp667 + (fTemp668 + (fTemp669 + ((fTemp670 + ((fTemp671 + fTemp490) + fTemp672)) + fTemp673)))))))))))))) * fRec1[0]);
			fRec273[0] = max((fRec273[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp674))))));
			fbargraph73 = fRec273[0];
			output37[i] = (FAUSTFLOAT)fTemp674;
			double fTemp675 = (fTemp472 + fTemp488);
			double fTemp676 = (((((((((((((((((fTemp374 + fTemp474) + fTemp471) + fTemp489) + fTemp470) + fTemp477) + fTemp491) + fTemp468) + fTemp485) + fTemp482) + fTemp480) + fTemp466) + fTemp465) + fTemp464) + fTemp463) + fTemp461) - ((((((((((((((((((fTemp675 + fTemp475) + fTemp487) + fTemp476) + fTemp486) + fTemp469) + fTemp478) + fTemp492) + fTemp493) + fTemp467) + fTemp494) + fTemp484) + fTemp483) + fTemp481) + fTemp462) + fTemp460) + fTemp479) + fTemp459) + fTemp458)) * fRec1[0]);
			fRec274[0] = max((fRec274[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp676))))));
			fbargraph74 = fRec274[0];
			output38[i] = (FAUSTFLOAT)fTemp676;
			double fTemp677 = (((((((((((((((((((fTemp374 + fTemp506) + fTemp489) + fTemp523) + fTemp477) + fTemp524) + fTemp505) + fTemp519) + fTemp504) + fTemp502) + fTemp508) + fTemp516) + fTemp500) + fTemp515) + fTemp499) + fTemp498) + fTemp512) + fTemp510) - ((((((((((((((((fTemp675 + fTemp507) + fTemp522) + fTemp521) + fTemp520) + fTemp478) + fTemp525) + fTemp503) + fTemp501) + fTemp518) + fTemp517) + fTemp514) + fTemp513) + fTemp511) + fTemp509) + fTemp497) + fTemp496)) * fRec1[0]);
			fRec275[0] = max((fRec275[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp677))))));
			fbargraph75 = fRec275[0];
			output39[i] = (FAUSTFLOAT)fTemp677;
			double fTemp678 = ((((((((((((((((((fTemp374 + fTemp547) + fTemp489) + fTemp536) + fTemp523) + fTemp537) + fTemp477) + fTemp535) + fTemp533) + fTemp532) + fTemp530) + fTemp545) + fTemp529) + fTemp499) + fTemp513) + fTemp543) + fTemp540) - ((((((((((((((((((0.021308452520676553 * (fTemp106 + fTemp159)) + fTemp472) + fTemp548) + fTemp488) + fTemp549) + fTemp538) + fTemp534) + fTemp478) + fTemp539) + fTemp550) + fTemp531) + fTemp508) + fTemp546) + fTemp528) + fTemp544) + fTemp542) + fTemp527) + fTemp541)) * fRec1[0]);
			fRec276[0] = max((fRec276[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp678))))));
			fbargraph76 = fRec276[0];
			output40[i] = (FAUSTFLOAT)fTemp678;
			double fTemp679 = ((fTemp472 + fTemp474) + fTemp488);
			double fTemp680 = ((((((((((((((((((((fTemp374 + fTemp475) + fTemp558) + fTemp489) + fTemp487) + fTemp477) + fTemp566) + fTemp559) + fTemp492) + fTemp569) + fTemp560) + fTemp556) + fTemp483) + fTemp564) + fTemp555) + fTemp554) + fTemp461) + fTemp460) + fTemp459) - ((((((((((((((fTemp679 + fTemp557) + fTemp568) + fTemp491) + fTemp486) + fTemp478) + fTemp561) + fTemp565) + fTemp563) + fTemp480) + fTemp465) + fTemp463) + fTemp553) + fTemp562) + fTemp552)) * fRec1[0]);
			fRec277[0] = max((fRec277[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp680))))));
			fbargraph77 = fRec277[0];
			output41[i] = (FAUSTFLOAT)fTemp680;
			double fTemp681 = (((((((((((((((((((((fTemp374 + fTemp591) + fTemp583) + fTemp489) + fTemp487) + fTemp477) + fTemp589) + fTemp580) + fTemp584) + fTemp593) + fTemp579) + fTemp577) + fTemp576) + fTemp481) + fTemp587) + fTemp574) + fTemp573) + fTemp462) + fTemp572) + fTemp585) - (((((((((((((fTemp679 + fTemp582) + fTemp590) + fTemp581) + fTemp592) + fTemp478) + fTemp578) + fTemp588) + fTemp575) + fTemp586) + fTemp464) + fTemp461) + fTemp460) + fTemp571)) * fRec1[0]);
			fRec278[0] = max((fRec278[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp681))))));
			fbargraph78 = fRec278[0];
			output42[i] = (FAUSTFLOAT)fTemp681;
			double fTemp682 = (((((((((((((((((((fTemp374 + fTemp614) + fTemp489) + fTemp605) + fTemp612) + fTemp604) + fTemp477) + fTemp505) + fTemp606) + fTemp607) + fTemp518) + fTemp609) + fTemp599) + fTemp598) + fTemp596) + fTemp608) + fTemp509) + fTemp496) - (((((((((((((((((fTemp472 + fTemp615) + fTemp488) + fTemp613) + fTemp603) + fTemp602) + fTemp478) + fTemp601) + fTemp616) + fTemp600) + fTemp508) + fTemp611) + fTemp516) + fTemp610) + fTemp499) + fTemp573) + fTemp597) + fTemp595)) * fRec1[0]);
			fRec279[0] = max((fRec279[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp682))))));
			fbargraph79 = fRec279[0];
			output43[i] = (FAUSTFLOAT)fTemp682;
			double fTemp683 = ((((((((((((fTemp618 + ((((((((fTemp374 + fTemp621) + fTemp489) + fTemp634) + fTemp477) + fTemp637) + fTemp620) + fTemp638) + fTemp623)) + fTemp508) + fTemp546) + fTemp625) + fTemp627) + fTemp628) + fTemp630) + fTemp541) + fTemp641) + fTemp642) - ((((((((fTemp624 + (fTemp619 + (fTemp632 + (((((fTemp675 + fTemp622) + fTemp635) + fTemp636) + fTemp633) + fTemp478)))) + fTemp545) + fTemp639) + fTemp626) + fTemp499) + fTemp640) + fTemp629) + fTemp631)) * fRec1[0]);
			fRec280[0] = max((fRec280[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp683))))));
			fbargraph80 = fRec280[0];
			output44[i] = (FAUSTFLOAT)fTemp683;
			double fTemp684 = (((fTemp658 + (fTemp659 + (fTemp644 + (fTemp660 + (fTemp662 + (fTemp646 + (fTemp648 + (fTemp649 + (fTemp669 + (fTemp651 + (fTemp672 + ((fTemp652 + ((fTemp653 + (fTemp654 + fTemp374)) + fTemp489)) + fTemp477)))))))))))) - (fTemp661 + (fTemp663 + (fTemp664 + (fTemp665 + (fTemp666 + (fTemp645 + (fTemp647 + (fTemp667 + (fTemp650 + (fTemp668 + (fTemp673 + ((fTemp657 + (fTemp670 + (fTemp656 + (fTemp671 + (fTemp655 + fTemp675))))) + fTemp478))))))))))))) * fRec1[0]);
			fRec281[0] = max((fRec281[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp684))))));
			fbargraph81 = fRec281[0];
			output45[i] = (FAUSTFLOAT)fTemp684;
			double fTemp685 = (fTemp374 + fTemp377);
			double fTemp686 = ((((((((((((((((((fTemp685 + fTemp373) + fTemp393) + fTemp372) + fTemp390) + fTemp395) + fTemp382) + fTemp388) + fTemp396) + fTemp371) + fTemp370) + fTemp368) + fTemp366) + fTemp365) + fTemp383) + fTemp363) + fTemp361) - (((((((((((((((fTemp375 + fTemp392) + fTemp379) + fTemp391) + fTemp381) + fTemp394) + fTemp389) + fTemp387) + fTemp386) + fTemp369) + fTemp367) + fTemp385) + fTemp364) + fTemp384) + fTemp362) + fTemp360)) * fRec1[0]);
			fRec282[0] = max((fRec282[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp686))))));
			fbargraph82 = fRec282[0];
			output46[i] = (FAUSTFLOAT)fTemp686;
			double fTemp687 = (((((((((((((((((((fTemp685 + fTemp410) + fTemp403) + fTemp372) + fTemp381) + fTemp395) + fTemp408) + fTemp387) + fTemp396) + fTemp402) + fTemp371) + fTemp401) + fTemp367) + fTemp365) + fTemp400) + fTemp363) + fTemp362) + fTemp405) - ((((((((((((((fTemp375 + fTemp411) + fTemp379) + fTemp373) + fTemp409) + fTemp404) + fTemp389) + fTemp382) + fTemp370) + fTemp369) + fTemp366) + fTemp399) + fTemp407) + fTemp398) + fTemp406)) * fRec1[0]);
			fRec283[0] = max((fRec283[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp687))))));
			fbargraph83 = fRec283[0];
			output47[i] = (FAUSTFLOAT)fTemp687;
			double fTemp688 = (((((((((((((((((((fTemp685 + fTemp434) + fTemp423) + fTemp421) + fTemp372) + fTemp436) + fTemp420) + fTemp395) + fTemp417) + fTemp396) + fTemp416) + fTemp370) + fTemp430) + fTemp366) + fTemp415) + fTemp426) + fTemp414) + fTemp424) - ((((((((((((((((fTemp375 + fTemp433) + fTemp379) + fTemp435) + fTemp422) + fTemp419) + fTemp389) + fTemp432) + fTemp418) + fTemp431) + fTemp429) + fTemp367) + fTemp428) + fTemp427) + fTemp413) + fTemp425) + fTemp362)) * fRec1[0]);
			fRec284[0] = max((fRec284[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp688))))));
			fbargraph84 = fRec284[0];
			output48[i] = (FAUSTFLOAT)fTemp688;
			double fTemp689 = (((((((((((((((((((fTemp374 + fTemp442) + fTemp377) + fTemp373) + fTemp454) + fTemp372) + fTemp451) + fTemp395) + fTemp382) + fTemp450) + fTemp396) + fTemp369) + fTemp367) + fTemp440) + fTemp444) + fTemp439) + fTemp362) + fTemp443) - (((((((((((((((((fTemp375 + fTemp441) + fTemp453) + fTemp379) + fTemp452) + fTemp381) + fTemp455) + fTemp389) + fTemp456) + fTemp449) + fTemp448) + fTemp370) + fTemp447) + fTemp446) + fTemp366) + fTemp445) + fTemp438) + fTemp363)) * fRec1[0]);
			fRec285[0] = max((fRec285[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp689))))));
			fbargraph85 = fRec285[0];
			output49[i] = (FAUSTFLOAT)fTemp689;
			// post processing
			fRec285[1] = fRec285[0];
			fRec284[1] = fRec284[0];
			fRec283[1] = fRec283[0];
			fRec282[1] = fRec282[0];
			fRec281[1] = fRec281[0];
			fRec280[1] = fRec280[0];
			fRec279[1] = fRec279[0];
			fRec278[1] = fRec278[0];
			fRec277[1] = fRec277[0];
			fRec276[1] = fRec276[0];
			fRec275[1] = fRec275[0];
			fRec274[1] = fRec274[0];
			fRec273[1] = fRec273[0];
			fRec272[1] = fRec272[0];
			fRec271[1] = fRec271[0];
			fRec270[1] = fRec270[0];
			fRec269[1] = fRec269[0];
			fRec268[1] = fRec268[0];
			fRec267[1] = fRec267[0];
			fRec266[1] = fRec266[0];
			fRec265[1] = fRec265[0];
			fRec264[1] = fRec264[0];
			fRec263[1] = fRec263[0];
			fRec262[1] = fRec262[0];
			fRec261[1] = fRec261[0];
			fRec260[1] = fRec260[0];
			fRec259[1] = fRec259[0];
			fRec258[1] = fRec258[0];
			fRec257[1] = fRec257[0];
			fRec256[1] = fRec256[0];
			fRec255[1] = fRec255[0];
			fRec254[1] = fRec254[0];
			fRec253[1] = fRec253[0];
			fRec252[1] = fRec252[0];
			fRec251[1] = fRec251[0];
			fRec250[1] = fRec250[0];
			fRec249[1] = fRec249[0];
			fRec248[1] = fRec248[0];
			fRec247[1] = fRec247[0];
			fRec246[1] = fRec246[0];
			fRec245[1] = fRec245[0];
			fRec244[1] = fRec244[0];
			fRec201[1] = fRec201[0];
			fRec237[1] = fRec237[0];
			fRec238[1] = fRec238[0];
			fRec242[1] = fRec242[0];
			fRec243[1] = fRec243[0];
			fRec239[1] = fRec239[0];
			fRec240[1] = fRec240[0];
			fRec241[1] = fRec241[0];
			fRec228[1] = fRec228[0];
			fRec229[1] = fRec229[0];
			fRec236[1] = fRec236[0];
			fRec230[1] = fRec230[0];
			fRec234[1] = fRec234[0];
			fRec235[1] = fRec235[0];
			fRec231[1] = fRec231[0];
			fRec232[1] = fRec232[0];
			fRec233[1] = fRec233[0];
			fRec221[1] = fRec221[0];
			fRec222[1] = fRec222[0];
			fRec226[1] = fRec226[0];
			fRec227[1] = fRec227[0];
			fRec223[1] = fRec223[0];
			fRec224[1] = fRec224[0];
			fRec225[1] = fRec225[0];
			fRec212[1] = fRec212[0];
			fRec213[1] = fRec213[0];
			fRec220[1] = fRec220[0];
			fRec214[1] = fRec214[0];
			fRec218[1] = fRec218[0];
			fRec219[1] = fRec219[0];
			fRec215[1] = fRec215[0];
			fRec216[1] = fRec216[0];
			fRec217[1] = fRec217[0];
			fRec206[1] = fRec206[0];
			fRec207[1] = fRec207[0];
			fRec211[1] = fRec211[0];
			fRec208[1] = fRec208[0];
			fRec209[1] = fRec209[0];
			fRec210[1] = fRec210[0];
			fRec202[1] = fRec202[0];
			fRec203[1] = fRec203[0];
			fRec204[1] = fRec204[0];
			fRec205[1] = fRec205[0];
			fRec200[1] = fRec200[0];
			fRec199[1] = fRec199[0];
			fRec198[1] = fRec198[0];
			fRec197[1] = fRec197[0];
			fRec136[1] = fRec136[0];
			fRec190[1] = fRec190[0];
			fRec191[1] = fRec191[0];
			fRec195[1] = fRec195[0];
			fRec196[1] = fRec196[0];
			fRec192[1] = fRec192[0];
			fRec193[1] = fRec193[0];
			fRec194[1] = fRec194[0];
			fRec184[1] = fRec184[0];
			fRec185[1] = fRec185[0];
			fRec189[1] = fRec189[0];
			fRec186[1] = fRec186[0];
			fRec187[1] = fRec187[0];
			fRec188[1] = fRec188[0];
			fRec178[1] = fRec178[0];
			fRec179[1] = fRec179[0];
			fRec183[1] = fRec183[0];
			fRec180[1] = fRec180[0];
			fRec181[1] = fRec181[0];
			fRec182[1] = fRec182[0];
			fRec171[1] = fRec171[0];
			fRec172[1] = fRec172[0];
			fRec176[1] = fRec176[0];
			fRec177[1] = fRec177[0];
			fRec173[1] = fRec173[0];
			fRec174[1] = fRec174[0];
			fRec175[1] = fRec175[0];
			fRec162[1] = fRec162[0];
			fRec163[1] = fRec163[0];
			fRec170[1] = fRec170[0];
			fRec164[1] = fRec164[0];
			fRec168[1] = fRec168[0];
			fRec169[1] = fRec169[0];
			fRec165[1] = fRec165[0];
			fRec166[1] = fRec166[0];
			fRec167[1] = fRec167[0];
			fRec159[1] = fRec159[0];
			fRec160[1] = fRec160[0];
			fRec161[1] = fRec161[0];
			fRec155[1] = fRec155[0];
			fRec156[1] = fRec156[0];
			fRec157[1] = fRec157[0];
			fRec158[1] = fRec158[0];
			fRec146[1] = fRec146[0];
			fRec147[1] = fRec147[0];
			fRec154[1] = fRec154[0];
			fRec148[1] = fRec148[0];
			fRec152[1] = fRec152[0];
			fRec153[1] = fRec153[0];
			fRec149[1] = fRec149[0];
			fRec150[1] = fRec150[0];
			fRec151[1] = fRec151[0];
			fRec137[1] = fRec137[0];
			fRec138[1] = fRec138[0];
			fRec145[1] = fRec145[0];
			fRec139[1] = fRec139[0];
			fRec143[1] = fRec143[0];
			fRec144[1] = fRec144[0];
			fRec140[1] = fRec140[0];
			fRec141[1] = fRec141[0];
			fRec142[1] = fRec142[0];
			fRec135[1] = fRec135[0];
			fRec0[1] = fRec0[0];
			fRec126[1] = fRec126[0];
			fRec127[1] = fRec127[0];
			fRec134[1] = fRec134[0];
			fRec128[1] = fRec128[0];
			fRec132[1] = fRec132[0];
			fRec133[1] = fRec133[0];
			fRec129[1] = fRec129[0];
			fRec130[1] = fRec130[0];
			fRec131[1] = fRec131[0];
			fRec117[1] = fRec117[0];
			fRec118[1] = fRec118[0];
			fRec125[1] = fRec125[0];
			fRec119[1] = fRec119[0];
			fRec123[1] = fRec123[0];
			fRec124[1] = fRec124[0];
			fRec120[1] = fRec120[0];
			fRec121[1] = fRec121[0];
			fRec122[1] = fRec122[0];
			fRec108[1] = fRec108[0];
			fRec109[1] = fRec109[0];
			fRec116[1] = fRec116[0];
			fRec110[1] = fRec110[0];
			fRec114[1] = fRec114[0];
			fRec115[1] = fRec115[0];
			fRec111[1] = fRec111[0];
			fRec112[1] = fRec112[0];
			fRec113[1] = fRec113[0];
			fRec99[1] = fRec99[0];
			fRec100[1] = fRec100[0];
			fRec107[1] = fRec107[0];
			fRec101[1] = fRec101[0];
			fRec105[1] = fRec105[0];
			fRec106[1] = fRec106[0];
			fRec102[1] = fRec102[0];
			fRec103[1] = fRec103[0];
			fRec104[1] = fRec104[0];
			fRec90[1] = fRec90[0];
			fRec91[1] = fRec91[0];
			fRec98[1] = fRec98[0];
			fRec92[1] = fRec92[0];
			fRec96[1] = fRec96[0];
			fRec97[1] = fRec97[0];
			fRec93[1] = fRec93[0];
			fRec94[1] = fRec94[0];
			fRec95[1] = fRec95[0];
			fRec81[1] = fRec81[0];
			fRec82[1] = fRec82[0];
			fRec89[1] = fRec89[0];
			fRec83[1] = fRec83[0];
			fRec87[1] = fRec87[0];
			fRec88[1] = fRec88[0];
			fRec84[1] = fRec84[0];
			fRec85[1] = fRec85[0];
			fRec86[1] = fRec86[0];
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
