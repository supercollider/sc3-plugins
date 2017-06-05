//----------------------------------------------------------
// name: "HOAEncoder5"
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
	FAUSTFLOAT 	fcheckbox0;
	FAUSTFLOAT 	fentry0;
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fslider1;
	double 	fRec1[2];
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph0;
	FAUSTFLOAT 	fslider2;
	FAUSTFLOAT 	fslider3;
	double 	fConst2;
	double 	fConst3;
	double 	fRec5[2];
	double 	fRec4[2];
	double 	fRec3[2];
	double 	fRec2[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec6[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec7[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fConst4;
	double 	fRec11[2];
	double 	fRec10[2];
	double 	fConst5;
	double 	fRec9[2];
	double 	fRec13[2];
	double 	fVec0[2];
	double 	fRec12[2];
	double 	fRec8[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec14[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec15[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec16[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec17[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fConst6;
	double 	fConst7;
	double 	fConst8;
	double 	fRec23[2];
	double 	fRec22[2];
	double 	fConst9;
	double 	fRec21[2];
	double 	fRec25[2];
	double 	fVec1[2];
	double 	fRec24[2];
	double 	fRec26[2];
	double 	fRec20[2];
	double 	fRec19[2];
	double 	fRec18[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec27[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec28[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec29[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec30[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec31[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec32[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fConst10;
	double 	fConst11;
	double 	fRec37[2];
	double 	fRec36[2];
	double 	fConst12;
	double 	fRec35[2];
	double 	fRec39[2];
	double 	fVec2[2];
	double 	fRec38[2];
	double 	fRec41[2];
	double 	fRec40[2];
	double 	fConst13;
	double 	fRec34[2];
	double 	fRec43[2];
	double 	fVec3[2];
	double 	fRec42[2];
	double 	fRec33[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec44[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec45[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec47[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec48[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec49[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec50[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec51[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fConst14;
	double 	fConst15;
	double 	fConst16;
	double 	fConst17;
	double 	fRec58[2];
	double 	fRec57[2];
	double 	fConst18;
	double 	fRec56[2];
	double 	fRec60[2];
	double 	fVec4[2];
	double 	fRec59[2];
	double 	fRec62[2];
	double 	fRec61[2];
	double 	fConst19;
	double 	fRec55[2];
	double 	fRec64[2];
	double 	fVec5[2];
	double 	fRec63[2];
	double 	fRec65[2];
	double 	fRec54[2];
	double 	fRec53[2];
	double 	fRec52[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec66[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec67[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec68[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec69[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec70[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec71[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec72[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec73[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec74[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec75[2];
	FAUSTFLOAT 	fbargraph35;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOAEncoder5");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/nfc.lib/name", "NF And NFC Filters Library");
		m->declare("lib/nfc.lib/version", "1.0");
		m->declare("lib/nfc.lib/author", "Pierre Lecomte");
		m->declare("lib/nfc.lib/license", "GPL");
		m->declare("lib/nfc.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/ymn.lib/name", "Spherical Harmonics library");
		m->declare("lib/ymn.lib/version", "1.0");
		m->declare("lib/ymn.lib/author", "Pierre Lecomte");
		m->declare("lib/ymn.lib/license", "GPL");
		m->declare("lib/ymn.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
	}

	virtual int getNumInputs() { return 1; }
	virtual int getNumOutputs() { return 36; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(1.92e+05, max(1e+03, (double)fSamplingFreq));
		fConst1 = (8e+01 / fConst0);
		fConst2 = (3.4e+02 / fConst0);
		fConst3 = (1.7e+02 / fConst0);
		fConst4 = (5.1e+02 / fConst0);
		fConst5 = (1.02e+03 / fConst0);
		fConst6 = (789.5430205728705 / fConst0);
		fConst7 = (394.7715102864353 / fConst0);
		fConst8 = (625.2284897135664 / fConst0);
		fConst9 = (1250.4569794271329 / fConst0);
		fConst10 = (984.7116049589241 / fConst0);
		fConst11 = (715.2883950410759 / fConst0);
		fConst12 = (1430.5767900821518 / fConst0);
		fConst13 = (1969.4232099178482 / fConst0);
		fConst14 = (1239.8911224120607 / fConst0);
		fConst15 = (619.9455612060303 / fConst0);
		fConst16 = (1139.6651757122002 / fConst0);
		fConst17 = (790.3892630817644 / fConst0);
		fConst18 = (1580.7785261635288 / fConst0);
		fConst19 = (2279.3303514244003 / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fcheckbox0 = 0.0;
		fentry0 = 1.07;
		fslider0 = 2.0;
		fslider1 = 0.0;
		fslider2 = 0.0;
		fslider3 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fVec0[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
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
		ui_interface->openHorizontalBox("HOAEncoder5");
		ui_interface->openVerticalBox("Parameters");
		ui_interface->openHorizontalBox("Source  0");
		ui_interface->declare(&fslider1, "0+1", "");
		ui_interface->declare(&fslider1, "osc", "/gain_0 -20 20");
		ui_interface->declare(&fslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Gain  0", &fslider1, 0.0, -2e+01, 2e+01, 0.1);
		ui_interface->declare(&fslider0, "0+2", "");
		ui_interface->declare(&fslider0, "osc", "/radius_0 0.5 50");
		ui_interface->declare(&fslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Radius  0", &fslider0, 2.0, 0.5, 5e+01, 0.01);
		ui_interface->declare(&fslider3, "0+3", "");
		ui_interface->declare(&fslider3, "osc", "/azimuth_0 0 360");
		ui_interface->declare(&fslider3, "style", "knob");
		ui_interface->addHorizontalSlider("Azimuth  0", &fslider3, 0.0, -3.141592653589793, 3.141592653589793, 0.1);
		ui_interface->declare(&fslider2, "0+4", "");
		ui_interface->declare(&fslider2, "osc", "/elevation_0 -90 90");
		ui_interface->declare(&fslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Elevation  0", &fslider2, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->declare(0, "0+5", "");
		ui_interface->openHorizontalBox("Spherical Wave");
		ui_interface->addCheckButton("Yes", &fcheckbox0);
		ui_interface->closeBox();
		ui_interface->declare(&fentry0, "~", "");
		ui_interface->addNumEntry("Speaker Radius  0", &fentry0, 1.07, 0.5, 1e+01, 0.01);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "~", "");
		ui_interface->openVerticalBox("Outputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d906a360", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d906f810", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d9071280", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d9072f20", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d9078e60", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d907aba0", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d907cb10", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d907e720", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d9080570", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d9088470", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d908a1f0", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d908c160", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d908dfa0", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d908fc70", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d9091a80", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90939f0", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d909d4b0", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d909f160", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90a1140", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90a2fe0", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90a51b0", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90a6f10", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90a8d20", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90aaba0", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90acc00", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5");
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90b8890", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90ba5f0", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90bc4f0", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90be2a0", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90c05d0", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90c2a90", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90c48b0", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90c67a0", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90c8700", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90ca6d0", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7fe6d90cc840", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = double(fcheckbox0);
		double 	fSlow1 = (1 - fSlow0);
		double 	fSlow2 = double(fentry0);
		double 	fSlow3 = (fSlow0 * fSlow2);
		double 	fSlow4 = double(fslider0);
		double 	fSlow5 = (fSlow1 + (fSlow3 / fSlow4));
		double 	fSlow6 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider1))));
		double 	fSlow7 = double(fslider2);
		double 	fSlow8 = cos(fSlow7);
		double 	fSlow9 = double(fslider3);
		double 	fSlow10 = sin(fSlow9);
		double 	fSlow11 = (fSlow8 * fSlow10);
		double 	fSlow12 = (1.7320508075688772 * fSlow11);
		double 	fSlow13 = (fConst3 / fSlow4);
		double 	fSlow14 = (fSlow13 + 1);
		double 	fSlow15 = (fConst2 / (fSlow4 * fSlow14));
		double 	fSlow16 = (fConst3 / fSlow2);
		double 	fSlow17 = (1.0 / (fSlow16 + 1));
		double 	fSlow18 = ((fSlow3 * fSlow14) / fSlow4);
		double 	fSlow19 = (fConst2 / fSlow2);
		double 	fSlow20 = sin(fSlow7);
		double 	fSlow21 = (1.7320508075688772 * fSlow20);
		double 	fSlow22 = cos(fSlow9);
		double 	fSlow23 = (fSlow8 * fSlow22);
		double 	fSlow24 = (1.7320508075688772 * fSlow23);
		double 	fSlow25 = faustpower<2>(fSlow8);
		double 	fSlow26 = (2 * fSlow9);
		double 	fSlow27 = sin(fSlow26);
		double 	fSlow28 = (fSlow25 * fSlow27);
		double 	fSlow29 = (1.9364916731037085 * fSlow28);
		double 	fSlow30 = faustpower<2>(fSlow16);
		double 	fSlow31 = (1.0 / (((fConst4 / fSlow2) + (3 * fSlow30)) + 1));
		double 	fSlow32 = faustpower<2>(fSlow13);
		double 	fSlow33 = (((fConst4 / fSlow4) + (3 * fSlow32)) + 1);
		double 	fSlow34 = ((fSlow3 * fSlow33) / fSlow4);
		double 	fSlow35 = (12 * fSlow30);
		double 	fSlow36 = (fSlow35 + (fConst5 / fSlow2));
		double 	fSlow37 = (1.0 / fSlow33);
		double 	fSlow38 = (12 * fSlow32);
		double 	fSlow39 = ((fConst5 / fSlow4) + fSlow38);
		double 	fSlow40 = sin((2 * fSlow7));
		double 	fSlow41 = (fSlow40 * fSlow10);
		double 	fSlow42 = (1.9364916731037085 * fSlow41);
		double 	fSlow43 = faustpower<2>(fSlow20);
		double 	fSlow44 = ((3 * fSlow43) + -1);
		double 	fSlow45 = (1.118033988749895 * fSlow44);
		double 	fSlow46 = (fSlow40 * fSlow22);
		double 	fSlow47 = (1.9364916731037085 * fSlow46);
		double 	fSlow48 = cos(fSlow26);
		double 	fSlow49 = (fSlow25 * fSlow48);
		double 	fSlow50 = (1.9364916731037085 * fSlow49);
		double 	fSlow51 = faustpower<3>(fSlow8);
		double 	fSlow52 = (3 * fSlow9);
		double 	fSlow53 = sin(fSlow52);
		double 	fSlow54 = (fSlow51 * fSlow53);
		double 	fSlow55 = (2.091650066335189 * fSlow54);
		double 	fSlow56 = ((fConst7 / fSlow4) + 1);
		double 	fSlow57 = (fConst6 / (fSlow4 * fSlow56));
		double 	fSlow58 = (1.0 / ((fConst7 / fSlow2) + 1));
		double 	fSlow59 = (1.0 / (((fConst8 / fSlow2) + (6.45943269348338 * fSlow30)) + 1));
		double 	fSlow60 = (((fConst8 / fSlow4) + (6.45943269348338 * fSlow32)) + 1);
		double 	fSlow61 = ((fSlow3 * fSlow60) / fSlow4);
		double 	fSlow62 = (25.83773077393352 * fSlow30);
		double 	fSlow63 = (fSlow62 + (fConst9 / fSlow2));
		double 	fSlow64 = (1.0 / fSlow60);
		double 	fSlow65 = (25.83773077393352 * fSlow32);
		double 	fSlow66 = ((fConst9 / fSlow4) + fSlow65);
		double 	fSlow67 = (fConst6 / fSlow2);
		double 	fSlow68 = (fSlow25 * fSlow20);
		double 	fSlow69 = (fSlow68 * fSlow27);
		double 	fSlow70 = (5.123475382979799 * fSlow69);
		double 	fSlow71 = (5 * fSlow43);
		double 	fSlow72 = (fSlow71 + -1);
		double 	fSlow73 = (1.620185174601965 * (fSlow11 * fSlow72));
		double 	fSlow74 = (1.3228756555322954 * (fSlow20 * (fSlow71 + -3)));
		double 	fSlow75 = (1.620185174601965 * (fSlow23 * fSlow72));
		double 	fSlow76 = (fSlow68 * fSlow48);
		double 	fSlow77 = (5.123475382979799 * fSlow76);
		double 	fSlow78 = cos(fSlow52);
		double 	fSlow79 = (fSlow51 * fSlow78);
		double 	fSlow80 = (2.091650066335189 * fSlow79);
		double 	fSlow81 = faustpower<4>(fSlow8);
		double 	fSlow82 = (4 * fSlow9);
		double 	fSlow83 = sin(fSlow82);
		double 	fSlow84 = (2.218529918662356 * (fSlow81 * fSlow83));
		double 	fSlow85 = (1.0 / (((fConst10 / fSlow2) + (9.1401308902779 * fSlow30)) + 1));
		double 	fSlow86 = (((9.1401308902779 * fSlow32) + (fConst10 / fSlow4)) + 1);
		double 	fSlow87 = (1.0 / (((fConst11 / fSlow2) + (11.4878004768713 * fSlow30)) + 1));
		double 	fSlow88 = (((fConst11 / fSlow4) + (11.4878004768713 * fSlow32)) + 1);
		double 	fSlow89 = ((fSlow3 * fSlow88) / fSlow4);
		double 	fSlow90 = (45.9512019074852 * fSlow30);
		double 	fSlow91 = (fSlow90 + (fConst12 / fSlow2));
		double 	fSlow92 = (1.0 / fSlow88);
		double 	fSlow93 = (45.9512019074852 * fSlow32);
		double 	fSlow94 = ((fConst12 / fSlow4) + fSlow93);
		double 	fSlow95 = (36.5605235611116 * fSlow30);
		double 	fSlow96 = (fSlow95 + (fConst13 / fSlow2));
		double 	fSlow97 = (1.0 / fSlow86);
		double 	fSlow98 = (36.5605235611116 * fSlow32);
		double 	fSlow99 = ((fConst13 / fSlow4) + fSlow98);
		double 	fSlow100 = (fSlow51 * fSlow20);
		double 	fSlow101 = (6.274950199005566 * (fSlow100 * fSlow53));
		double 	fSlow102 = (7 * fSlow43);
		double 	fSlow103 = (fSlow102 + -1);
		double 	fSlow104 = (1.6770509831248424 * (fSlow28 * fSlow103));
		double 	fSlow105 = (fSlow102 + -3);
		double 	fSlow106 = (1.1858541225631423 * (fSlow41 * fSlow105));
		double 	fSlow107 = (0.375 * ((fSlow43 * ((35 * fSlow43) + -30)) + 3));
		double 	fSlow108 = (1.1858541225631423 * (fSlow46 * fSlow105));
		double 	fSlow109 = (1.6770509831248424 * (fSlow49 * fSlow103));
		double 	fSlow110 = (6.274950199005566 * (fSlow100 * fSlow78));
		double 	fSlow111 = cos(fSlow82);
		double 	fSlow112 = (2.218529918662356 * (fSlow81 * fSlow111));
		double 	fSlow113 = faustpower<5>(fSlow8);
		double 	fSlow114 = (5 * fSlow9);
		double 	fSlow115 = (2.3268138086232857 * (fSlow113 * sin(fSlow114)));
		double 	fSlow116 = ((fConst15 / fSlow4) + 1);
		double 	fSlow117 = (fConst14 / (fSlow4 * fSlow116));
		double 	fSlow118 = (1.0 / ((fConst15 / fSlow2) + 1));
		double 	fSlow119 = (1.0 / (((fConst16 / fSlow2) + (14.2724805132801 * fSlow30)) + 1));
		double 	fSlow120 = (((14.2724805132801 * fSlow32) + (fConst16 / fSlow4)) + 1);
		double 	fSlow121 = (1.0 / (((fConst17 / fSlow2) + (18.1563153134523 * fSlow30)) + 1));
		double 	fSlow122 = (((fConst17 / fSlow4) + (18.1563153134523 * fSlow32)) + 1);
		double 	fSlow123 = ((fSlow3 * fSlow122) / fSlow4);
		double 	fSlow124 = (72.6252612538092 * fSlow30);
		double 	fSlow125 = (fSlow124 + (fConst18 / fSlow2));
		double 	fSlow126 = (1.0 / fSlow122);
		double 	fSlow127 = (72.6252612538092 * fSlow32);
		double 	fSlow128 = ((fConst18 / fSlow4) + fSlow127);
		double 	fSlow129 = (57.0899220531204 * fSlow30);
		double 	fSlow130 = (fSlow129 + (fConst19 / fSlow2));
		double 	fSlow131 = (1.0 / fSlow120);
		double 	fSlow132 = (57.0899220531204 * fSlow32);
		double 	fSlow133 = ((fConst19 / fSlow4) + fSlow132);
		double 	fSlow134 = (fConst14 / fSlow2);
		double 	fSlow135 = (fSlow81 * fSlow20);
		double 	fSlow136 = (7.358031326380718 * (fSlow135 * fSlow83));
		double 	fSlow137 = ((9 * fSlow43) + -1);
		double 	fSlow138 = (1.734304615688951 * (fSlow54 * fSlow137));
		double 	fSlow139 = (8.496322733983215 * (fSlow69 * fSlow44));
		double 	fSlow140 = ((fSlow43 * ((21 * fSlow43) + -14)) + 1);
		double 	fSlow141 = (1.6056540723331412 * (fSlow11 * fSlow140));
		double 	fSlow142 = (0.414578098794425 * (fSlow20 * ((fSlow43 * ((63 * fSlow43) + -70)) + 15)));
		double 	fSlow143 = (1.6056540723331412 * (fSlow23 * fSlow140));
		double 	fSlow144 = (8.496322733983215 * (fSlow76 * fSlow44));
		double 	fSlow145 = (1.734304615688951 * (fSlow79 * fSlow137));
		double 	fSlow146 = (7.358031326380718 * (fSlow135 * fSlow111));
		double 	fSlow147 = (2.3268138086232857 * (fSlow113 * cos(fSlow114)));
		FAUSTFLOAT* input0 = input[0];
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
		for (int i=0; i<count; i++) {
			fRec1[0] = (fSlow6 + (0.999 * fRec1[1]));
			double fTemp0 = ((double)input0[i] * fRec1[0]);
			double fTemp1 = (fSlow5 * fTemp0);
			fRec0[0] = max((fRec0[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp1))))));
			fbargraph0 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp1;
			fRec5[0] = (fRec5[1] + fRec4[1]);
			fRec4[0] = (fSlow17 * ((fSlow18 * fTemp0) - (fSlow19 * fRec5[0])));
			fRec3[0] = (fRec4[0] + fRec3[1]);
			double fTemp2 = (fSlow1 * fTemp0);
			double fTemp3 = ((fSlow15 * (fRec3[0] - fRec4[0])) + (fRec4[0] + fTemp2));
			double fTemp4 = (fSlow12 * fTemp3);
			fRec2[0] = max((fRec2[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fbargraph1 = fRec2[0];
			output1[i] = (FAUSTFLOAT)fTemp4;
			double fTemp5 = (fSlow21 * fTemp3);
			fRec6[0] = max((fRec6[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fbargraph2 = fRec6[0];
			output2[i] = (FAUSTFLOAT)fTemp5;
			double fTemp6 = (fSlow24 * fTemp3);
			fRec7[0] = max((fRec7[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fbargraph3 = fRec7[0];
			output3[i] = (FAUSTFLOAT)fTemp6;
			fRec11[0] = (fRec11[1] + fRec9[1]);
			fRec10[0] = (fRec10[1] + fRec11[1]);
			fRec9[0] = (fSlow31 * ((fSlow34 * fTemp0) - ((fSlow35 * fRec10[0]) + (fSlow36 * fRec11[0]))));
			fRec13[0] = (fRec9[0] + fRec13[1]);
			double fTemp7 = (fRec13[0] - fRec9[0]);
			fVec0[0] = fTemp7;
			fRec12[0] = (fRec12[1] + fVec0[1]);
			double fTemp8 = ((fRec9[0] + fTemp2) + (fSlow37 * ((fSlow38 * fRec12[0]) + (fSlow39 * fVec0[0]))));
			double fTemp9 = (fSlow29 * fTemp8);
			fRec8[0] = max((fRec8[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp9))))));
			fbargraph4 = fRec8[0];
			output4[i] = (FAUSTFLOAT)fTemp9;
			double fTemp10 = (fSlow42 * fTemp8);
			fRec14[0] = max((fRec14[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fbargraph5 = fRec14[0];
			output5[i] = (FAUSTFLOAT)fTemp10;
			double fTemp11 = (fSlow45 * fTemp8);
			fRec15[0] = max((fRec15[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp11))))));
			fbargraph6 = fRec15[0];
			output6[i] = (FAUSTFLOAT)fTemp11;
			double fTemp12 = (fSlow47 * fTemp8);
			fRec16[0] = max((fRec16[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fbargraph7 = fRec16[0];
			output7[i] = (FAUSTFLOAT)fTemp12;
			double fTemp13 = (fSlow50 * fTemp8);
			fRec17[0] = max((fRec17[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp13))))));
			fbargraph8 = fRec17[0];
			output8[i] = (FAUSTFLOAT)fTemp13;
			fRec23[0] = (fRec23[1] + fRec21[1]);
			fRec22[0] = (fRec22[1] + fRec23[1]);
			fRec21[0] = (fSlow59 * ((fSlow61 * fTemp0) - ((fSlow62 * fRec22[0]) + (fSlow63 * fRec23[0]))));
			fRec25[0] = (fRec21[0] + fRec25[1]);
			double fTemp14 = (fRec25[0] - fRec21[0]);
			fVec1[0] = fTemp14;
			fRec24[0] = (fRec24[1] + fVec1[1]);
			fRec26[0] = (fRec26[1] + fRec20[1]);
			fRec20[0] = (fSlow58 * ((fSlow56 * (fRec21[0] + (fSlow64 * ((fSlow65 * fRec24[0]) + (fSlow66 * fVec1[0]))))) - (fSlow67 * fRec26[0])));
			fRec19[0] = (fRec20[0] + fRec19[1]);
			double fTemp15 = ((fSlow57 * (fRec19[0] - fRec20[0])) + (fRec20[0] + fTemp2));
			double fTemp16 = (fSlow55 * fTemp15);
			fRec18[0] = max((fRec18[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fbargraph9 = fRec18[0];
			output9[i] = (FAUSTFLOAT)fTemp16;
			double fTemp17 = (fSlow70 * fTemp15);
			fRec27[0] = max((fRec27[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp17))))));
			fbargraph10 = fRec27[0];
			output10[i] = (FAUSTFLOAT)fTemp17;
			double fTemp18 = (fSlow73 * fTemp15);
			fRec28[0] = max((fRec28[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fbargraph11 = fRec28[0];
			output11[i] = (FAUSTFLOAT)fTemp18;
			double fTemp19 = (fSlow74 * fTemp15);
			fRec29[0] = max((fRec29[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fbargraph12 = fRec29[0];
			output12[i] = (FAUSTFLOAT)fTemp19;
			double fTemp20 = (fSlow75 * fTemp15);
			fRec30[0] = max((fRec30[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph13 = fRec30[0];
			output13[i] = (FAUSTFLOAT)fTemp20;
			double fTemp21 = (fSlow77 * fTemp15);
			fRec31[0] = max((fRec31[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp21))))));
			fbargraph14 = fRec31[0];
			output14[i] = (FAUSTFLOAT)fTemp21;
			double fTemp22 = (fSlow80 * fTemp15);
			fRec32[0] = max((fRec32[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fbargraph15 = fRec32[0];
			output15[i] = (FAUSTFLOAT)fTemp22;
			fRec37[0] = (fRec37[1] + fRec35[1]);
			fRec36[0] = (fRec36[1] + fRec37[1]);
			fRec35[0] = (fSlow87 * ((fSlow89 * fTemp0) - ((fSlow90 * fRec36[0]) + (fSlow91 * fRec37[0]))));
			fRec39[0] = (fRec35[0] + fRec39[1]);
			double fTemp23 = (fRec39[0] - fRec35[0]);
			fVec2[0] = fTemp23;
			fRec38[0] = (fRec38[1] + fVec2[1]);
			fRec41[0] = (fRec41[1] + fRec34[1]);
			fRec40[0] = (fRec40[1] + fRec41[1]);
			fRec34[0] = (fSlow85 * ((fSlow86 * (fRec35[0] + (fSlow92 * ((fSlow93 * fRec38[0]) + (fSlow94 * fVec2[0]))))) - ((fSlow95 * fRec40[0]) + (fSlow96 * fRec41[0]))));
			fRec43[0] = (fRec34[0] + fRec43[1]);
			double fTemp24 = (fRec43[0] - fRec34[0]);
			fVec3[0] = fTemp24;
			fRec42[0] = (fRec42[1] + fVec3[1]);
			double fTemp25 = ((fRec34[0] + fTemp2) + (fSlow97 * ((fSlow98 * fRec42[0]) + (fSlow99 * fVec3[0]))));
			double fTemp26 = (fSlow84 * fTemp25);
			fRec33[0] = max((fRec33[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fbargraph16 = fRec33[0];
			output16[i] = (FAUSTFLOAT)fTemp26;
			double fTemp27 = (fSlow101 * fTemp25);
			fRec44[0] = max((fRec44[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp27))))));
			fbargraph17 = fRec44[0];
			output17[i] = (FAUSTFLOAT)fTemp27;
			double fTemp28 = (fSlow104 * fTemp25);
			fRec45[0] = max((fRec45[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph18 = fRec45[0];
			output18[i] = (FAUSTFLOAT)fTemp28;
			double fTemp29 = (fSlow106 * fTemp25);
			fRec46[0] = max((fRec46[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph19 = fRec46[0];
			output19[i] = (FAUSTFLOAT)fTemp29;
			double fTemp30 = (fSlow107 * fTemp25);
			fRec47[0] = max((fRec47[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fbargraph20 = fRec47[0];
			output20[i] = (FAUSTFLOAT)fTemp30;
			double fTemp31 = (fSlow108 * fTemp25);
			fRec48[0] = max((fRec48[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fbargraph21 = fRec48[0];
			output21[i] = (FAUSTFLOAT)fTemp31;
			double fTemp32 = (fSlow109 * fTemp25);
			fRec49[0] = max((fRec49[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph22 = fRec49[0];
			output22[i] = (FAUSTFLOAT)fTemp32;
			double fTemp33 = (fSlow110 * fTemp25);
			fRec50[0] = max((fRec50[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph23 = fRec50[0];
			output23[i] = (FAUSTFLOAT)fTemp33;
			double fTemp34 = (fSlow112 * fTemp25);
			fRec51[0] = max((fRec51[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fbargraph24 = fRec51[0];
			output24[i] = (FAUSTFLOAT)fTemp34;
			fRec58[0] = (fRec58[1] + fRec56[1]);
			fRec57[0] = (fRec57[1] + fRec58[1]);
			fRec56[0] = (fSlow121 * ((fSlow123 * fTemp0) - ((fSlow124 * fRec57[0]) + (fSlow125 * fRec58[0]))));
			fRec60[0] = (fRec56[0] + fRec60[1]);
			double fTemp35 = (fRec60[0] - fRec56[0]);
			fVec4[0] = fTemp35;
			fRec59[0] = (fRec59[1] + fVec4[1]);
			fRec62[0] = (fRec62[1] + fRec55[1]);
			fRec61[0] = (fRec61[1] + fRec62[1]);
			fRec55[0] = (fSlow119 * ((fSlow120 * (fRec56[0] + (fSlow126 * ((fSlow127 * fRec59[0]) + (fSlow128 * fVec4[0]))))) - ((fSlow129 * fRec61[0]) + (fSlow130 * fRec62[0]))));
			fRec64[0] = (fRec55[0] + fRec64[1]);
			double fTemp36 = (fRec64[0] - fRec55[0]);
			fVec5[0] = fTemp36;
			fRec63[0] = (fRec63[1] + fVec5[1]);
			fRec65[0] = (fRec65[1] + fRec54[1]);
			fRec54[0] = (fSlow118 * ((fSlow116 * (fRec55[0] + (fSlow131 * ((fSlow132 * fRec63[0]) + (fSlow133 * fVec5[0]))))) - (fSlow134 * fRec65[0])));
			fRec53[0] = (fRec54[0] + fRec53[1]);
			double fTemp37 = ((fSlow117 * (fRec53[0] - fRec54[0])) + (fRec54[0] + fTemp2));
			double fTemp38 = (fSlow115 * fTemp37);
			fRec52[0] = max((fRec52[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fbargraph25 = fRec52[0];
			output25[i] = (FAUSTFLOAT)fTemp38;
			double fTemp39 = (fSlow136 * fTemp37);
			fRec66[0] = max((fRec66[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fbargraph26 = fRec66[0];
			output26[i] = (FAUSTFLOAT)fTemp39;
			double fTemp40 = (fSlow138 * fTemp37);
			fRec67[0] = max((fRec67[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fbargraph27 = fRec67[0];
			output27[i] = (FAUSTFLOAT)fTemp40;
			double fTemp41 = (fSlow139 * fTemp37);
			fRec68[0] = max((fRec68[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fbargraph28 = fRec68[0];
			output28[i] = (FAUSTFLOAT)fTemp41;
			double fTemp42 = (fSlow141 * fTemp37);
			fRec69[0] = max((fRec69[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fbargraph29 = fRec69[0];
			output29[i] = (FAUSTFLOAT)fTemp42;
			double fTemp43 = (fSlow142 * fTemp37);
			fRec70[0] = max((fRec70[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp43))))));
			fbargraph30 = fRec70[0];
			output30[i] = (FAUSTFLOAT)fTemp43;
			double fTemp44 = (fSlow143 * fTemp37);
			fRec71[0] = max((fRec71[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fbargraph31 = fRec71[0];
			output31[i] = (FAUSTFLOAT)fTemp44;
			double fTemp45 = (fSlow144 * fTemp37);
			fRec72[0] = max((fRec72[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fbargraph32 = fRec72[0];
			output32[i] = (FAUSTFLOAT)fTemp45;
			double fTemp46 = (fSlow145 * fTemp37);
			fRec73[0] = max((fRec73[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fbargraph33 = fRec73[0];
			output33[i] = (FAUSTFLOAT)fTemp46;
			double fTemp47 = (fSlow146 * fTemp37);
			fRec74[0] = max((fRec74[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp47))))));
			fbargraph34 = fRec74[0];
			output34[i] = (FAUSTFLOAT)fTemp47;
			double fTemp48 = (fSlow147 * fTemp37);
			fRec75[0] = max((fRec75[1] - fConst1), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fbargraph35 = fRec75[0];
			output35[i] = (FAUSTFLOAT)fTemp48;
			// post processing
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
			fRec52[1] = fRec52[0];
			fRec53[1] = fRec53[0];
			fRec54[1] = fRec54[0];
			fRec65[1] = fRec65[0];
			fRec63[1] = fRec63[0];
			fVec5[1] = fVec5[0];
			fRec64[1] = fRec64[0];
			fRec55[1] = fRec55[0];
			fRec61[1] = fRec61[0];
			fRec62[1] = fRec62[0];
			fRec59[1] = fRec59[0];
			fVec4[1] = fVec4[0];
			fRec60[1] = fRec60[0];
			fRec56[1] = fRec56[0];
			fRec57[1] = fRec57[0];
			fRec58[1] = fRec58[0];
			fRec51[1] = fRec51[0];
			fRec50[1] = fRec50[0];
			fRec49[1] = fRec49[0];
			fRec48[1] = fRec48[0];
			fRec47[1] = fRec47[0];
			fRec46[1] = fRec46[0];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec33[1] = fRec33[0];
			fRec42[1] = fRec42[0];
			fVec3[1] = fVec3[0];
			fRec43[1] = fRec43[0];
			fRec34[1] = fRec34[0];
			fRec40[1] = fRec40[0];
			fRec41[1] = fRec41[0];
			fRec38[1] = fRec38[0];
			fVec2[1] = fVec2[0];
			fRec39[1] = fRec39[0];
			fRec35[1] = fRec35[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec32[1] = fRec32[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
			fRec27[1] = fRec27[0];
			fRec18[1] = fRec18[0];
			fRec19[1] = fRec19[0];
			fRec20[1] = fRec20[0];
			fRec26[1] = fRec26[0];
			fRec24[1] = fRec24[0];
			fVec1[1] = fVec1[0];
			fRec25[1] = fRec25[0];
			fRec21[1] = fRec21[0];
			fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			fRec15[1] = fRec15[0];
			fRec14[1] = fRec14[0];
			fRec8[1] = fRec8[0];
			fRec12[1] = fRec12[0];
			fVec0[1] = fVec0[0];
			fRec13[1] = fRec13[0];
			fRec9[1] = fRec9[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			fRec7[1] = fRec7[0];
			fRec6[1] = fRec6[0];
			fRec2[1] = fRec2[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec0[1] = fRec0[0];
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
