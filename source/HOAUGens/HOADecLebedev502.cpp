//----------------------------------------------------------
// name: "HOADecLebedev502"
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
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec22[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec26[2];
	double 	fRec25[2];
	double 	fRec24[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec30[2];
	double 	fRec29[2];
	double 	fRec28[2];
	double 	fRec27[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec23[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec31[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec32[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec33[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec34[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec39[2];
	double 	fRec38[2];
	double 	fRec37[2];
	double 	fRec36[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec35[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec40[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec41[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec42[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec43[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec44[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec45[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec47[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec48[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec49[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec50[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec51[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec52[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec53[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec54[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec55[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec56[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec57[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec58[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec59[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec60[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec61[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec62[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec63[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec64[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec65[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec66[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec67[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec68[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec69[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec70[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec71[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec72[2];
	FAUSTFLOAT 	fbargraph49;
	double 	fRec73[2];
	FAUSTFLOAT 	fbargraph50;
	double 	fRec74[2];
	FAUSTFLOAT 	fbargraph51;
	double 	fRec75[2];
	FAUSTFLOAT 	fbargraph52;
	double 	fRec76[2];
	FAUSTFLOAT 	fbargraph53;
	double 	fRec77[2];
	FAUSTFLOAT 	fbargraph54;
	double 	fRec78[2];
	FAUSTFLOAT 	fbargraph55;
	double 	fRec79[2];
	FAUSTFLOAT 	fbargraph56;
	double 	fRec80[2];
	FAUSTFLOAT 	fbargraph57;
	double 	fRec81[2];
	FAUSTFLOAT 	fbargraph58;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOADecLebedev502");
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

	virtual int getNumInputs() { return 9; }
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
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<2; i++) fRec78[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
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
		ui_interface->openVerticalBox("HOADecLebedev502");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("B-Format");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61690b0", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae617e460", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae616c200", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae616eab0", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae6181290", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae618dc00", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae6172a00", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae6175a50", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae6178b10", &fbargraph5, -7e+01, 6.0);
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
		ui_interface->declare(&fbargraph6, "osc", "/output1");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae617a240", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph7, "osc", "/output2");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae617bc60", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph10, "osc", "/output3");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61829a0", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph11, "osc", "/output4");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61847c0", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph12, "osc", "/output5");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61864f0", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph13, "osc", "/output6");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae6187fa0", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph14, "osc", "/output7");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae618a4e0", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph16, "osc", "/output8");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae618f7e0", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph17, "osc", "/output9");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae6191cf0", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph18, "osc", "/output10");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61941c0", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph19, "osc", "/output11");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61963e0", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph20, "osc", "/output12");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae6198380", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph21, "osc", "/output13");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae619a2f0", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph22, "osc", "/output14");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae619c310", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph23, "osc", "/output15");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae619df90", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph24, "osc", "/output16");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61a0230", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph25, "osc", "/output17");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61a2320", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph26, "osc", "/output18");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61a45f0", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph27, "osc", "/output19");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61a79a0", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph28, "osc", "/output20");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61aa2f0", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph29, "osc", "/output21");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61ac860", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph30, "osc", "/output22");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61af020", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph31, "osc", "/output23");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61b1540", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph32, "osc", "/output24");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61b36e0", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph33, "osc", "/output25");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61b5ce0", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Outputs 26-50");
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph34, "osc", "/output26");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61b8480", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph35, "osc", "/output27");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61bac00", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph36, "osc", "/output28");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61bce00", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph37, "osc", "/output29");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61befd0", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph38, "osc", "/output30");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61c1320", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph39, "osc", "/output31");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61c40c0", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph40, "osc", "/output32");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61c6970", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph41, "osc", "/output33");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61c8f70", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph42, "osc", "/output34");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61cb300", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph43, "osc", "/output35");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61cdd10", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("36");
		ui_interface->declare(&fbargraph44, "osc", "/output36");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61d0490", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("37");
		ui_interface->declare(&fbargraph45, "osc", "/output37");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61d2d20", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("38");
		ui_interface->declare(&fbargraph46, "osc", "/output38");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61d5850", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("39");
		ui_interface->declare(&fbargraph47, "osc", "/output39");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae61d7900", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("40");
		ui_interface->declare(&fbargraph48, "osc", "/output40");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae7d47fe0", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("41");
		ui_interface->declare(&fbargraph49, "osc", "/output41");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae7d4a2e0", &fbargraph49, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("42");
		ui_interface->declare(&fbargraph50, "osc", "/output42");
		ui_interface->declare(&fbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae7d4c460", &fbargraph50, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("43");
		ui_interface->declare(&fbargraph51, "osc", "/output43");
		ui_interface->declare(&fbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae7d4e7f0", &fbargraph51, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("44");
		ui_interface->declare(&fbargraph52, "osc", "/output44");
		ui_interface->declare(&fbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae7d50bd0", &fbargraph52, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("45");
		ui_interface->declare(&fbargraph53, "osc", "/output45");
		ui_interface->declare(&fbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae7d530e0", &fbargraph53, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("46");
		ui_interface->declare(&fbargraph54, "osc", "/output46");
		ui_interface->declare(&fbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae779d430", &fbargraph54, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("47");
		ui_interface->declare(&fbargraph55, "osc", "/output47");
		ui_interface->declare(&fbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae779f590", &fbargraph55, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("48");
		ui_interface->declare(&fbargraph56, "osc", "/output48");
		ui_interface->declare(&fbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae77a1ba0", &fbargraph56, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("49");
		ui_interface->declare(&fbargraph57, "osc", "/output49");
		ui_interface->declare(&fbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae77a41b0", &fbargraph57, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("50");
		ui_interface->declare(&fbargraph58, "osc", "/output50");
		ui_interface->declare(&fbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fcae77a6780", &fbargraph58, -7e+01, 6.0);
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
			double fTemp0 = (fSlow5 * (fRec3[0] * (double)input0[i]));
			fRec2[0] = max((fRec2[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fbargraph0 = fRec2[0];
			double fTemp1 = fTemp0;
			double fTemp2 = (0.012698412698412698 * fTemp1);
			double fTemp3 = (fRec3[0] * (double)input2[i]);
			fRec6[0] = (fRec6[1] + fRec5[1]);
			fRec5[0] = (fSlow8 * ((fSlow4 * fTemp3) - (fSlow9 * fRec6[0])));
			double fTemp4 = (fRec5[0] + (fSlow2 * fTemp3));
			fRec4[0] = max((fRec4[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fbargraph1 = fRec4[0];
			double fTemp5 = fTemp4;
			double fTemp6 = (0.0219942959691286 * fTemp5);
			double fTemp7 = (fRec3[0] * (double)input3[i]);
			fRec9[0] = (fRec9[1] + fRec8[1]);
			fRec8[0] = (fSlow8 * ((fSlow4 * fTemp7) - (fSlow9 * fRec9[0])));
			double fTemp8 = (fRec8[0] + (fSlow2 * fTemp7));
			fRec7[0] = max((fRec7[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fbargraph2 = fRec7[0];
			double fTemp9 = fTemp8;
			double fTemp10 = (1.3467622079046435e-18 * fTemp9);
			double fTemp11 = (fRec3[0] * (double)input6[i]);
			fRec13[0] = (fRec13[1] + fRec11[1]);
			fRec12[0] = (fRec12[1] + fRec13[1]);
			fRec11[0] = (fSlow11 * ((fSlow4 * fTemp11) - ((fSlow12 * fRec12[0]) + (fSlow13 * fRec13[0]))));
			double fTemp12 = (fRec11[0] + (fSlow2 * fTemp11));
			fRec10[0] = max((fRec10[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fbargraph3 = fRec10[0];
			double fTemp13 = fTemp12;
			double fTemp14 = (0.02839451399999733 * fTemp13);
			double fTemp15 = (fRec3[0] * (double)input7[i]);
			fRec17[0] = (fRec17[1] + fRec15[1]);
			fRec16[0] = (fRec16[1] + fRec17[1]);
			fRec15[0] = (fSlow11 * ((fSlow4 * fTemp15) - ((fSlow12 * fRec16[0]) + (fSlow13 * fRec17[0]))));
			double fTemp16 = (fRec15[0] + (fSlow2 * fTemp15));
			fRec14[0] = max((fRec14[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fbargraph4 = fRec14[0];
			double fTemp17 = fTemp16;
			double fTemp18 = (3.0114518464024883e-18 * fTemp17);
			double fTemp19 = (fRec3[0] * (double)input8[i]);
			fRec21[0] = (fRec21[1] + fRec19[1]);
			fRec20[0] = (fRec20[1] + fRec21[1]);
			fRec19[0] = (fSlow11 * ((fSlow4 * fTemp19) - ((fSlow12 * fRec20[0]) + (fSlow13 * fRec21[0]))));
			double fTemp20 = (fRec19[0] + (fSlow2 * fTemp19));
			fRec18[0] = max((fRec18[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph5 = fRec18[0];
			double fTemp21 = fTemp20;
			double fTemp22 = (9.219912161207984e-35 * fTemp21);
			double fTemp23 = (fRec1[0] * (((((fTemp2 + fTemp6) + fTemp10) + fTemp14) + fTemp18) + fTemp22));
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp23))))));
			fbargraph6 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp23;
			double fTemp24 = (0.0219942959691286 * fTemp9);
			double fTemp25 = (0.024590370452110585 * fTemp21);
			double fTemp26 = (0.014197256999998666 * fTemp13);
			double fTemp27 = (fRec1[0] * (((fTemp24 + fTemp2) + fTemp25) - fTemp26));
			fRec22[0] = max((fRec22[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fbargraph7 = fRec22[0];
			output1[i] = (FAUSTFLOAT)fTemp27;
			double fTemp28 = (fRec3[0] * (double)input1[i]);
			fRec26[0] = (fRec26[1] + fRec25[1]);
			fRec25[0] = (fSlow8 * ((fSlow4 * fTemp28) - (fSlow9 * fRec26[0])));
			double fTemp29 = (fRec25[0] + (fSlow2 * fTemp28));
			fRec24[0] = max((fRec24[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph8 = fRec24[0];
			double fTemp30 = fTemp29;
			double fTemp31 = (0.0219942959691286 * fTemp30);
			double fTemp32 = (fRec3[0] * (double)input4[i]);
			fRec30[0] = (fRec30[1] + fRec28[1]);
			fRec29[0] = (fRec29[1] + fRec30[1]);
			fRec28[0] = (fSlow11 * ((fSlow4 * fTemp32) - ((fSlow12 * fRec29[0]) + (fSlow13 * fRec30[0]))));
			double fTemp33 = (fRec28[0] + (fSlow2 * fTemp32));
			fRec27[0] = max((fRec27[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph9 = fRec27[0];
			double fTemp34 = fTemp33;
			double fTemp35 = (fRec1[0] * ((((fTemp31 + (3.0114518464024883e-18 * fTemp34)) + fTemp2) + fTemp10) - (fTemp26 + fTemp25)));
			fRec23[0] = max((fRec23[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fbargraph10 = fRec23[0];
			output2[i] = (FAUSTFLOAT)fTemp35;
			double fTemp36 = (fRec1[0] * ((fTemp25 + ((2.693524415809287e-18 * fTemp30) + fTemp2)) - (fTemp26 + (fTemp24 + (6.0229036928049765e-18 * fTemp34)))));
			fRec31[0] = max((fRec31[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph11 = fRec31[0];
			output3[i] = (FAUSTFLOAT)fTemp36;
			double fTemp37 = (fRec1[0] * (((9.034355539207464e-18 * fTemp34) + fTemp2) - (fTemp25 + (fTemp26 + (fTemp31 + (4.040286623713931e-18 * fTemp9))))));
			fRec32[0] = max((fRec32[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fbargraph12 = fRec32[0];
			output4[i] = (FAUSTFLOAT)fTemp37;
			double fTemp38 = (fRec1[0] * ((((fTemp2 + fTemp10) + fTemp14) + fTemp22) - (fTemp6 + fTemp18)));
			fRec33[0] = max((fRec33[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fbargraph13 = fRec33[0];
			output5[i] = (FAUSTFLOAT)fTemp38;
			double fTemp39 = (0.022574955908289243 * fTemp1);
			double fTemp40 = (0.0276485614705685 * fTemp5);
			double fTemp41 = (0.027648561470568506 * fTemp9);
			double fTemp42 = (0.012619783999998805 * fTemp13);
			double fTemp43 = (0.043716214137085485 * fTemp17);
			double fTemp44 = (0.02185810706854275 * fTemp21);
			double fTemp45 = (fRec1[0] * (((((fTemp39 + fTemp40) + fTemp41) + fTemp42) + fTemp43) + fTemp44));
			fRec34[0] = max((fRec34[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fbargraph14 = fRec34[0];
			output6[i] = (FAUSTFLOAT)fTemp45;
			double fTemp46 = (1.6929861152980278e-18 * fTemp9);
			double fTemp47 = (0.027648561470568506 * fTemp30);
			double fTemp48 = (fTemp47 + (2.6768460856911014e-18 * fTemp34));
			double fTemp49 = (fRec3[0] * (double)input5[i]);
			fRec39[0] = (fRec39[1] + fRec37[1]);
			fRec38[0] = (fRec38[1] + fRec39[1]);
			fRec37[0] = (fSlow11 * ((fSlow4 * fTemp49) - ((fSlow12 * fRec38[0]) + (fSlow13 * fRec39[0]))));
			double fTemp50 = (fRec37[0] + (fSlow2 * fTemp49));
			fRec36[0] = max((fRec36[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fbargraph15 = fRec36[0];
			double fTemp51 = fTemp50;
			double fTemp52 = (0.043716214137085485 * fTemp51);
			double fTemp53 = (2.6768460856911007e-18 * fTemp17);
			double fTemp54 = (fRec1[0] * (((fTemp42 + (fTemp46 + (fTemp40 + (fTemp39 + (fTemp48 + fTemp52))))) + fTemp53) - fTemp44));
			fRec35[0] = max((fRec35[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fbargraph16 = fRec35[0];
			output7[i] = (FAUSTFLOAT)fTemp54;
			double fTemp55 = (3.3859722305960555e-18 * fTemp30);
			double fTemp56 = (5.353692171382201e-18 * fTemp51);
			double fTemp57 = (5.353692171382203e-18 * fTemp34);
			double fTemp58 = (fRec1[0] * ((fTemp44 + (fTemp42 + (fTemp40 + (fTemp39 + (fTemp55 + fTemp56))))) - (fTemp43 + (fTemp41 + fTemp57))));
			fRec40[0] = max((fRec40[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fbargraph17 = fRec40[0];
			output8[i] = (FAUSTFLOAT)fTemp58;
			double fTemp59 = (8.030538257073304e-18 * fTemp34);
			double fTemp60 = (8.0305382570733e-18 * fTemp17);
			double fTemp61 = (5.078958345894083e-18 * fTemp9);
			double fTemp62 = (fRec1[0] * ((fTemp42 + (fTemp40 + (fTemp39 + fTemp59))) - (fTemp44 + (fTemp60 + (fTemp61 + (fTemp47 + fTemp52))))));
			fRec41[0] = max((fRec41[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fbargraph18 = fRec41[0];
			output9[i] = (FAUSTFLOAT)fTemp62;
			double fTemp63 = (0.0276485614705685 * fTemp30);
			double fTemp64 = (0.043716214137085485 * fTemp34);
			double fTemp65 = (0.02523956799999763 * fTemp13);
			double fTemp66 = (fRec1[0] * (((fTemp41 + (fTemp39 + (fTemp63 + fTemp64))) + (2.6768460856911007e-18 * fTemp21)) - fTemp65));
			fRec42[0] = max((fRec42[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fbargraph19 = fRec42[0];
			output10[i] = (FAUSTFLOAT)fTemp66;
			double fTemp67 = (0.0276485614705685 * fTemp9);
			double fTemp68 = (fRec1[0] * ((fTemp39 + fTemp47) - ((fTemp65 + (fTemp64 + fTemp67)) + (8.0305382570733e-18 * fTemp21))));
			fRec43[0] = max((fRec43[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fbargraph20 = fRec43[0];
			output11[i] = (FAUSTFLOAT)fTemp68;
			double fTemp69 = (fRec1[0] * (((fTemp39 + fTemp64) + (1.3384230428455503e-17 * fTemp21)) - (fTemp65 + (fTemp41 + fTemp63))));
			fRec44[0] = max((fRec44[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp69))))));
			fbargraph21 = fRec44[0];
			output12[i] = (FAUSTFLOAT)fTemp69;
			double fTemp70 = (fRec1[0] * ((fTemp39 + fTemp67) - ((fTemp65 + (fTemp47 + fTemp64)) + (1.8737922599837705e-17 * fTemp21))));
			fRec45[0] = max((fRec45[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp70))))));
			fbargraph22 = fRec45[0];
			output13[i] = (FAUSTFLOAT)fTemp70;
			double fTemp71 = (fRec1[0] * ((fTemp44 + (fTemp42 + (fTemp39 + fTemp41))) - (fTemp40 + fTemp43)));
			fRec46[0] = max((fRec46[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fbargraph23 = fRec46[0];
			output14[i] = (FAUSTFLOAT)fTemp71;
			double fTemp72 = (fRec1[0] * ((fTemp42 + (fTemp46 + (fTemp39 + fTemp48))) - (fTemp44 + (fTemp53 + (fTemp40 + fTemp52)))));
			fRec47[0] = max((fRec47[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp72))))));
			fbargraph24 = fRec47[0];
			output15[i] = (FAUSTFLOAT)fTemp72;
			double fTemp73 = (fRec1[0] * ((fTemp44 + (fTemp43 + (fTemp42 + (fTemp39 + fTemp55)))) - (fTemp41 + (fTemp40 + (fTemp57 + fTemp56)))));
			fRec48[0] = max((fRec48[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp73))))));
			fbargraph25 = fRec48[0];
			output16[i] = (FAUSTFLOAT)fTemp73;
			double fTemp74 = (fRec1[0] * ((fTemp60 + (fTemp42 + (fTemp39 + (fTemp52 + fTemp59)))) - (fTemp44 + (fTemp61 + (fTemp40 + fTemp47)))));
			fRec49[0] = max((fRec49[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp74))))));
			fbargraph26 = fRec49[0];
			output17[i] = (FAUSTFLOAT)fTemp74;
			double fTemp75 = (fTemp1 + fTemp5);
			double fTemp76 = (0.02109375 * fTemp75);
			double fTemp77 = (1.6674738251076273e-18 * fTemp21);
			double fTemp78 = (0.02723191415302091 * fTemp17);
			double fTemp79 = (5.236595479505967e-18 * fTemp13);
			double fTemp80 = (0.021093749999999998 * fTemp9);
			double fTemp81 = (0.027231914153020904 * fTemp51);
			double fTemp82 = (0.021093749999999994 * fTemp30);
			double fTemp83 = (0.027231914153020897 * fTemp34);
			double fTemp84 = (fTemp82 + fTemp83);
			double fTemp85 = (fRec1[0] * (fTemp76 + (fTemp77 + (fTemp78 + (fTemp79 + (fTemp80 + (fTemp81 + fTemp84)))))));
			fRec50[0] = max((fRec50[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp85))))));
			fbargraph27 = fRec50[0];
			output18[i] = (FAUSTFLOAT)fTemp85;
			double fTemp86 = (0.021093749999999998 * fTemp30);
			double fTemp87 = (0.02723191415302091 * fTemp51);
			double fTemp88 = (0.021093749999999994 * fTemp9);
			double fTemp89 = (0.027231914153020904 * fTemp17);
			double fTemp90 = (5.002421475322882e-18 * fTemp21);
			double fTemp91 = (fRec1[0] * ((fTemp76 + (fTemp79 + (fTemp86 + fTemp87))) - (((fTemp83 + fTemp88) + fTemp89) + fTemp90)));
			fRec51[0] = max((fRec51[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp91))))));
			fbargraph28 = fRec51[0];
			output19[i] = (FAUSTFLOAT)fTemp91;
			double fTemp92 = (8.337369125538138e-18 * fTemp21);
			double fTemp93 = (fRec1[0] * (((fTemp83 + fTemp79) + fTemp92) - ((0.02109375 * (fTemp9 - fTemp75)) + (fTemp78 + (fTemp82 + fTemp81)))));
			fRec52[0] = max((fRec52[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fbargraph29 = fRec52[0];
			output20[i] = (FAUSTFLOAT)fTemp93;
			double fTemp94 = (fTemp30 - fTemp1);
			double fTemp95 = (fTemp83 + fTemp87);
			double fTemp96 = (1.167231677575339e-17 * fTemp21);
			double fTemp97 = (fRec1[0] * (((0.02109375 * (fTemp5 - fTemp94)) + (fTemp89 + (fTemp79 + fTemp88))) - (fTemp95 + fTemp96)));
			fRec53[0] = max((fRec53[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp97))))));
			fbargraph30 = fRec53[0];
			output21[i] = (FAUSTFLOAT)fTemp97;
			double fTemp98 = (fTemp1 - fTemp5);
			double fTemp99 = (0.02109375 * fTemp98);
			double fTemp100 = (fRec1[0] * ((fTemp99 + (fTemp77 + ((fTemp80 + fTemp84) + fTemp79))) - (fTemp81 + fTemp78)));
			fRec54[0] = max((fRec54[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp100))))));
			fbargraph31 = fRec54[0];
			output22[i] = (FAUSTFLOAT)fTemp100;
			double fTemp101 = (fRec1[0] * ((fTemp99 + (fTemp89 + (fTemp79 + fTemp86))) - (fTemp90 + (fTemp88 + fTemp95))));
			fRec55[0] = max((fRec55[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp101))))));
			fbargraph32 = fRec55[0];
			output23[i] = (FAUSTFLOAT)fTemp101;
			double fTemp102 = (fRec1[0] * ((fTemp92 + (fTemp78 + (fTemp79 + (fTemp83 + fTemp81)))) - (fTemp82 + (0.02109375 * (fTemp9 - fTemp98)))));
			fRec56[0] = max((fRec56[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp102))))));
			fbargraph33 = fRec56[0];
			output24[i] = (FAUSTFLOAT)fTemp102;
			double fTemp103 = (fRec1[0] * ((fTemp79 + (fTemp88 + fTemp87)) - ((0.02109375 * (fTemp5 + fTemp94)) + (fTemp96 + (fTemp83 + fTemp89)))));
			fRec57[0] = max((fRec57[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp103))))));
			fbargraph34 = fRec57[0];
			output25[i] = (FAUSTFLOAT)fTemp103;
			double fTemp104 = (0.010535180889746065 * (fTemp30 + fTemp9));
			double fTemp105 = (0.03160554266923825 * fTemp5);
			double fTemp106 = (0.02017333553791887 * fTemp1);
			double fTemp107 = (0.007102817506892165 * fTemp34);
			double fTemp108 = (fTemp106 + fTemp107);
			double fTemp109 = (0.03280650879687195 * fTemp13);
			double fTemp110 = ((fTemp105 + fTemp108) + fTemp109);
			double fTemp111 = (0.021308452520676528 * (fTemp51 + fTemp17));
			double fTemp112 = (1.1422201709353029e-18 * fTemp21);
			double fTemp113 = (fRec1[0] * (((fTemp104 + fTemp110) + fTemp111) - fTemp112));
			fRec58[0] = max((fRec58[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp113))))));
			fbargraph35 = fRec58[0];
			output26[i] = (FAUSTFLOAT)fTemp113;
			double fTemp114 = (0.010535180889746065 * (fTemp30 - fTemp9));
			double fTemp115 = (fTemp106 + fTemp105);
			double fTemp116 = (0.021308452520676528 * (fTemp51 - fTemp17));
			double fTemp117 = (1.3047664087114909e-18 * fTemp21);
			double fTemp118 = (fRec1[0] * (((fTemp114 + (fTemp115 + fTemp109)) + fTemp116) - (fTemp107 + fTemp117)));
			fRec59[0] = max((fRec59[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp118))))));
			fbargraph36 = fRec59[0];
			output27[i] = (FAUSTFLOAT)fTemp118;
			double fTemp119 = (2.1746106811858183e-18 * fTemp21);
			double fTemp120 = (0.02130845252067653 * fTemp17);
			double fTemp121 = (0.010535180889746067 * fTemp9);
			double fTemp122 = (0.010535180889746065 * fTemp30);
			double fTemp123 = (0.021308452520676528 * fTemp51);
			double fTemp124 = (fRec1[0] * ((fTemp110 + fTemp119) - (fTemp120 + (fTemp121 + (fTemp122 + fTemp123)))));
			fRec60[0] = max((fRec60[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp124))))));
			fbargraph37 = fRec60[0];
			output28[i] = (FAUSTFLOAT)fTemp124;
			double fTemp125 = (0.021308452520676528 * fTemp17);
			double fTemp126 = (0.010535180889746065 * fTemp9);
			double fTemp127 = (3.044454953660145e-18 * fTemp21);
			double fTemp128 = (0.02130845252067653 * fTemp51);
			double fTemp129 = (fTemp107 + (0.010535180889746067 * fTemp30));
			double fTemp130 = (fRec1[0] * ((fTemp125 + (fTemp109 + (fTemp126 + fTemp115))) - (fTemp127 + (fTemp128 + fTemp129))));
			fRec61[0] = max((fRec61[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp130))))));
			fbargraph38 = fRec61[0];
			output29[i] = (FAUSTFLOAT)fTemp130;
			double fTemp131 = (0.028411270027568724 * fTemp21);
			double fTemp132 = (0.03160554266923824 * fTemp9);
			double fTemp133 = (0.010535180889746075 * fTemp5);
			double fTemp134 = (0.021308452520676584 * fTemp34);
			double fTemp135 = ((0.010535180889746094 * fTemp30) + fTemp134);
			double fTemp136 = (0.007102817506892191 * fTemp51);
			double fTemp137 = (0.021308452520676542 * fTemp17);
			double fTemp138 = (0.01640325439843596 * fTemp13);
			double fTemp139 = (fRec1[0] * ((fTemp131 + ((fTemp132 + (fTemp133 + (fTemp106 + (fTemp135 + fTemp136)))) + fTemp137)) - fTemp138));
			fRec62[0] = max((fRec62[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp139))))));
			fbargraph39 = fRec62[0];
			output30[i] = (FAUSTFLOAT)fTemp139;
			double fTemp140 = (0.01053518088974608 * fTemp9);
			double fTemp141 = (0.03160554266923824 * fTemp30);
			double fTemp142 = (fTemp141 + (0.021308452520676556 * fTemp34));
			double fTemp143 = (0.021308452520676546 * fTemp51);
			double fTemp144 = (0.007102817506892183 * fTemp17);
			double fTemp145 = (0.02841127002756874 * fTemp21);
			double fTemp146 = (fRec1[0] * (((fTemp140 + (fTemp133 + (fTemp106 + (fTemp142 + fTemp143)))) + fTemp144) - (fTemp138 + fTemp145)));
			fRec63[0] = max((fRec63[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp146))))));
			fbargraph40 = fRec63[0];
			output31[i] = (FAUSTFLOAT)fTemp146;
			double fTemp147 = (fTemp106 + fTemp141);
			double fTemp148 = (0.007102817506892181 * fTemp17);
			double fTemp149 = (0.010535180889746079 * fTemp9);
			double fTemp150 = (0.028411270027568745 * fTemp21);
			double fTemp151 = (fRec1[0] * ((fTemp133 + fTemp147) - ((0.021308452520676553 * (fTemp34 - fTemp51)) + ((fTemp148 + (fTemp138 + fTemp149)) + fTemp150))));
			fRec64[0] = max((fRec64[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp151))))));
			fbargraph41 = fRec64[0];
			output32[i] = (FAUSTFLOAT)fTemp151;
			double fTemp152 = (0.028411270027568717 * fTemp21);
			double fTemp153 = (0.010535180889746096 * fTemp30);
			double fTemp154 = (0.007102817506892193 * fTemp51);
			double fTemp155 = (fRec1[0] * ((fTemp152 + (fTemp133 + (fTemp106 + (fTemp153 + fTemp154)))) - (fTemp137 + (fTemp138 + (fTemp132 + fTemp134)))));
			fRec65[0] = max((fRec65[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp155))))));
			fbargraph42 = fRec65[0];
			output33[i] = (FAUSTFLOAT)fTemp155;
			double fTemp156 = (0.02130845252067657 * fTemp34);
			double fTemp157 = (0.02841127002756873 * fTemp21);
			double fTemp158 = (0.01053518088974609 * fTemp30);
			double fTemp159 = (0.007102817506892187 * fTemp51);
			double fTemp160 = (0.021308452520676546 * fTemp17);
			double fTemp161 = (fRec1[0] * (((fTemp133 + (fTemp106 + fTemp156)) + fTemp157) - ((fTemp138 + (fTemp132 + (fTemp158 + fTemp159))) + fTemp160)));
			fRec66[0] = max((fRec66[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp161))))));
			fbargraph43 = fRec66[0];
			output34[i] = (FAUSTFLOAT)fTemp161;
			double fTemp162 = (0.021308452520676563 * fTemp34);
			double fTemp163 = (0.007102817506892185 * fTemp17);
			double fTemp164 = (0.010535180889746086 * fTemp9);
			double fTemp165 = (0.028411270027568734 * fTemp21);
			double fTemp166 = (fRec1[0] * ((fTemp133 + (fTemp106 + fTemp162)) - ((fTemp163 + (fTemp138 + (fTemp164 + (fTemp141 + fTemp143)))) + fTemp165)));
			fRec67[0] = max((fRec67[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp166))))));
			fbargraph44 = fRec67[0];
			output35[i] = (FAUSTFLOAT)fTemp166;
			double fTemp167 = (fTemp106 + fTemp133);
			double fTemp168 = (0.010535180889746074 * fTemp9);
			double fTemp169 = (0.0071028175068921775 * fTemp17);
			double fTemp170 = (0.021308452520676553 * fTemp51);
			double fTemp171 = (fTemp141 + (0.021308452520676542 * fTemp34));
			double fTemp172 = (0.028411270027568748 * fTemp21);
			double fTemp173 = (fRec1[0] * (((fTemp167 + fTemp168) + fTemp169) - ((fTemp138 + (fTemp170 + fTemp171)) + fTemp172)));
			fRec68[0] = max((fRec68[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp173))))));
			fbargraph45 = fRec68[0];
			output36[i] = (FAUSTFLOAT)fTemp173;
			double fTemp174 = (0.028411270027568696 * fTemp21);
			double fTemp175 = (0.03160554266923823 * fTemp9);
			double fTemp176 = (0.02130845252067654 * fTemp17);
			double fTemp177 = ((0.010535180889746113 * fTemp30) + (0.021308452520676615 * fTemp34));
			double fTemp178 = (0.007102817506892205 * fTemp51);
			double fTemp179 = (fRec1[0] * ((fTemp174 + ((fTemp167 + fTemp175) + fTemp176)) - (fTemp138 + (fTemp177 + fTemp178))));
			fRec69[0] = max((fRec69[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp179))))));
			fbargraph46 = fRec69[0];
			output37[i] = (FAUSTFLOAT)fTemp179;
			double fTemp180 = (fRec1[0] * ((fTemp131 + (fTemp132 + (fTemp106 + fTemp135))) - (fTemp137 + (fTemp138 + (fTemp133 + fTemp136)))));
			fRec70[0] = max((fRec70[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp180))))));
			fbargraph47 = fRec70[0];
			output38[i] = (FAUSTFLOAT)fTemp180;
			double fTemp181 = (fRec1[0] * ((fTemp140 + (fTemp106 + fTemp142)) - (fTemp145 + (fTemp144 + (fTemp138 + (fTemp133 + fTemp143))))));
			fRec71[0] = max((fRec71[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp181))))));
			fbargraph48 = fRec71[0];
			output39[i] = (FAUSTFLOAT)fTemp181;
			double fTemp182 = (fRec1[0] * ((fTemp147 + fTemp148) - ((0.021308452520676553 * (fTemp34 + fTemp51)) + (fTemp150 + (fTemp138 + (fTemp133 + fTemp149))))));
			fRec72[0] = max((fRec72[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp182))))));
			fbargraph49 = fRec72[0];
			output40[i] = (FAUSTFLOAT)fTemp182;
			double fTemp183 = (fRec1[0] * ((fTemp152 + (fTemp137 + (fTemp106 + fTemp153))) - (fTemp138 + (fTemp132 + (fTemp133 + (fTemp134 + fTemp154))))));
			fRec73[0] = max((fRec73[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp183))))));
			fbargraph50 = fRec73[0];
			output41[i] = (FAUSTFLOAT)fTemp183;
			double fTemp184 = (fRec1[0] * ((fTemp157 + (fTemp160 + (fTemp106 + (fTemp156 + fTemp159)))) - (fTemp138 + (fTemp132 + (fTemp133 + fTemp158)))));
			fRec74[0] = max((fRec74[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp184))))));
			fbargraph51 = fRec74[0];
			output42[i] = (FAUSTFLOAT)fTemp184;
			double fTemp185 = (fRec1[0] * ((fTemp163 + (fTemp106 + (fTemp143 + fTemp162))) - (fTemp165 + (fTemp138 + (fTemp164 + (fTemp133 + fTemp141))))));
			fRec75[0] = max((fRec75[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp185))))));
			fbargraph52 = fRec75[0];
			output43[i] = (FAUSTFLOAT)fTemp185;
			double fTemp186 = (fRec1[0] * ((fTemp168 + (fTemp106 + fTemp170)) - (fTemp172 + (fTemp169 + (fTemp138 + (fTemp133 + fTemp171))))));
			fRec76[0] = max((fRec76[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp186))))));
			fbargraph53 = fRec76[0];
			output44[i] = (FAUSTFLOAT)fTemp186;
			double fTemp187 = (fRec1[0] * ((fTemp174 + (fTemp175 + (fTemp106 + fTemp178))) - (fTemp176 + (fTemp138 + (fTemp133 + fTemp177)))));
			fRec77[0] = max((fRec77[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp187))))));
			fbargraph54 = fRec77[0];
			output45[i] = (FAUSTFLOAT)fTemp187;
			double fTemp188 = (fRec1[0] * ((fTemp104 + (fTemp108 + fTemp109)) - (fTemp111 + (fTemp105 + fTemp112))));
			fRec78[0] = max((fRec78[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp188))))));
			fbargraph55 = fRec78[0];
			output46[i] = (FAUSTFLOAT)fTemp188;
			double fTemp189 = (fRec1[0] * ((fTemp114 + (fTemp106 + fTemp109)) - (fTemp116 + ((fTemp105 + fTemp107) + fTemp117))));
			fRec79[0] = max((fRec79[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp189))))));
			fbargraph56 = fRec79[0];
			output47[i] = (FAUSTFLOAT)fTemp189;
			double fTemp190 = (fRec1[0] * ((fTemp119 + (fTemp120 + (fTemp109 + (fTemp106 + (fTemp107 + fTemp123))))) - (fTemp121 + (fTemp122 + fTemp105))));
			fRec80[0] = max((fRec80[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp190))))));
			fbargraph57 = fRec80[0];
			output48[i] = (FAUSTFLOAT)fTemp190;
			double fTemp191 = (fRec1[0] * ((fTemp109 + (fTemp126 + (fTemp106 + fTemp128))) - (fTemp127 + (fTemp125 + (fTemp105 + fTemp129)))));
			fRec81[0] = max((fRec81[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp191))))));
			fbargraph58 = fRec81[0];
			output49[i] = (FAUSTFLOAT)fTemp191;
			// post processing
			fRec81[1] = fRec81[0];
			fRec80[1] = fRec80[0];
			fRec79[1] = fRec79[0];
			fRec78[1] = fRec78[0];
			fRec77[1] = fRec77[0];
			fRec76[1] = fRec76[0];
			fRec75[1] = fRec75[0];
			fRec74[1] = fRec74[0];
			fRec73[1] = fRec73[0];
			fRec72[1] = fRec72[0];
			fRec71[1] = fRec71[0];
			fRec70[1] = fRec70[0];
			fRec69[1] = fRec69[0];
			fRec68[1] = fRec68[0];
			fRec67[1] = fRec67[0];
			fRec66[1] = fRec66[0];
			fRec65[1] = fRec65[0];
			fRec64[1] = fRec64[0];
			fRec63[1] = fRec63[0];
			fRec62[1] = fRec62[0];
			fRec61[1] = fRec61[0];
			fRec60[1] = fRec60[0];
			fRec59[1] = fRec59[0];
			fRec58[1] = fRec58[0];
			fRec57[1] = fRec57[0];
			fRec56[1] = fRec56[0];
			fRec55[1] = fRec55[0];
			fRec54[1] = fRec54[0];
			fRec53[1] = fRec53[0];
			fRec52[1] = fRec52[0];
			fRec51[1] = fRec51[0];
			fRec50[1] = fRec50[0];
			fRec49[1] = fRec49[0];
			fRec48[1] = fRec48[0];
			fRec47[1] = fRec47[0];
			fRec46[1] = fRec46[0];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec43[1] = fRec43[0];
			fRec42[1] = fRec42[0];
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fRec35[1] = fRec35[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec38[1] = fRec38[0];
			fRec39[1] = fRec39[0];
			fRec34[1] = fRec34[0];
			fRec33[1] = fRec33[0];
			fRec32[1] = fRec32[0];
			fRec31[1] = fRec31[0];
			fRec23[1] = fRec23[0];
			fRec27[1] = fRec27[0];
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec30[1] = fRec30[0];
			fRec24[1] = fRec24[0];
			fRec25[1] = fRec25[0];
			fRec26[1] = fRec26[0];
			fRec22[1] = fRec22[0];
			fRec0[1] = fRec0[0];
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
