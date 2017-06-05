//----------------------------------------------------------
// name: "HOAEncEigenMike3"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2015"
//
// Code generated with Faust 0.9.100 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with  */
#include <math.h>
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
	FAUSTFLOAT 	fslider0;
	double 	fRec1[2];
	double 	fRec2[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fRec3[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec4[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec5[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fRec6[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec7[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec8[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec9[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec10[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec11[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec12[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec13[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec14[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec15[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec16[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec17[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec18[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec19[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec20[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec21[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec22[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec23[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec24[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec25[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec26[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec27[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec28[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec29[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec30[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec31[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec32[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec33[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec34[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec35[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec36[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec37[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec38[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec39[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec40[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec41[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec42[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec43[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec44[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec45[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec47[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec48[2];
	FAUSTFLOAT 	fbargraph47;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOAEncEigenMike3");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2015");
		m->declare("lib/eigenmike32.lib/name", "Pentakis dodecahedron grid");
		m->declare("lib/eigenmike32.lib/version", "1.0");
		m->declare("lib/eigenmike32.lib/author", "Pierre Lecomte");
		m->declare("lib/eigenmike32.lib/license", "GPL");
		m->declare("lib/eigenmike32.lib/copyright", "(c) Pierre Lecomte 2015");
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

	virtual int getNumInputs() { return 32; }
	virtual int getNumOutputs() { return 16; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = (8e+01 / min(1.92e+05, max(1e+03, (double)fSamplingFreq)));
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
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
		ui_interface->openVerticalBox("HOAEncEigenMike3");
		ui_interface->declare(0, "0", "");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150028ea0", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150027a80", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150026660", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150025240", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150023e20", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150022a00", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff1500215e0", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff1500201c0", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15001eda0", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15001d980", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15001c560", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15001b140", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150019d20", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150018900", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff1500174e0", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff1500160c0", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150014ca0", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150013880", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150012460", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150011040", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15000fc20", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15000e800", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15000d3e0", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15000bfc0", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15000abc0", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff1500097d0", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff1500083e0", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150006fd0", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150005be0", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff1500047f0", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150003430", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150002020", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("Outputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15002d1b0", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff1500324c0", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150035a50", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15003b080", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15003fc00", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150043130", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150046d50", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15004a830", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15004f050", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff1500538a0", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150056570", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15005b670", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff15005d880", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150062c90", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff150065b50", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff14fad5280", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(&fslider0, "1", "");
		ui_interface->declare(&fslider0, "style", "knob");
		ui_interface->declare(&fslider0, "unit", "dB");
		ui_interface->addHorizontalSlider("Gain", &fslider0, 0.0, -1e+01, 5e+01, 0.1);
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider0))));
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
		for (int i=0; i<count; i++) {
			fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
			double fTemp0 = (double)input31[i];
			fRec2[0] = max((fRec2[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fbargraph0 = fRec2[0];
			double fTemp1 = fTemp0;
			double fTemp2 = (double)input30[i];
			fRec3[0] = max((fRec3[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp2))))));
			fbargraph1 = fRec3[0];
			double fTemp3 = fTemp2;
			double fTemp4 = (double)input29[i];
			fRec4[0] = max((fRec4[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fbargraph2 = fRec4[0];
			double fTemp5 = fTemp4;
			double fTemp6 = (double)input28[i];
			fRec5[0] = max((fRec5[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp6))))));
			fbargraph3 = fRec5[0];
			double fTemp7 = fTemp6;
			double fTemp8 = (double)input27[i];
			fRec6[0] = max((fRec6[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp8))))));
			fbargraph4 = fRec6[0];
			double fTemp9 = fTemp8;
			double fTemp10 = (double)input26[i];
			fRec7[0] = max((fRec7[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fbargraph5 = fRec7[0];
			double fTemp11 = fTemp10;
			double fTemp12 = (double)input25[i];
			fRec8[0] = max((fRec8[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp12))))));
			fbargraph6 = fRec8[0];
			double fTemp13 = fTemp12;
			double fTemp14 = (double)input24[i];
			fRec9[0] = max((fRec9[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp14))))));
			fbargraph7 = fRec9[0];
			double fTemp15 = fTemp14;
			double fTemp16 = (double)input23[i];
			fRec10[0] = max((fRec10[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp16))))));
			fbargraph8 = fRec10[0];
			double fTemp17 = fTemp16;
			double fTemp18 = (double)input22[i];
			fRec11[0] = max((fRec11[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fbargraph9 = fRec11[0];
			double fTemp19 = fTemp18;
			double fTemp20 = (double)input21[i];
			fRec12[0] = max((fRec12[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph10 = fRec12[0];
			double fTemp21 = fTemp20;
			double fTemp22 = (double)input20[i];
			fRec13[0] = max((fRec13[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fbargraph11 = fRec13[0];
			double fTemp23 = fTemp22;
			double fTemp24 = (double)input19[i];
			fRec14[0] = max((fRec14[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fbargraph12 = fRec14[0];
			double fTemp25 = fTemp24;
			double fTemp26 = (double)input18[i];
			fRec15[0] = max((fRec15[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp26))))));
			fbargraph13 = fRec15[0];
			double fTemp27 = fTemp26;
			double fTemp28 = (double)input17[i];
			fRec16[0] = max((fRec16[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph14 = fRec16[0];
			double fTemp29 = fTemp28;
			double fTemp30 = (double)input16[i];
			fRec17[0] = max((fRec17[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp30))))));
			fbargraph15 = fRec17[0];
			double fTemp31 = fTemp30;
			double fTemp32 = (double)input15[i];
			fRec18[0] = max((fRec18[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp32))))));
			fbargraph16 = fRec18[0];
			double fTemp33 = fTemp32;
			double fTemp34 = (double)input14[i];
			fRec19[0] = max((fRec19[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fbargraph17 = fRec19[0];
			double fTemp35 = fTemp34;
			double fTemp36 = (double)input13[i];
			fRec20[0] = max((fRec20[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph18 = fRec20[0];
			double fTemp37 = fTemp36;
			double fTemp38 = (double)input12[i];
			fRec21[0] = max((fRec21[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp38))))));
			fbargraph19 = fRec21[0];
			double fTemp39 = fTemp38;
			double fTemp40 = (double)input11[i];
			fRec22[0] = max((fRec22[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp40))))));
			fbargraph20 = fRec22[0];
			double fTemp41 = fTemp40;
			double fTemp42 = (double)input10[i];
			fRec23[0] = max((fRec23[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp42))))));
			fbargraph21 = fRec23[0];
			double fTemp43 = fTemp42;
			double fTemp44 = (double)input9[i];
			fRec24[0] = max((fRec24[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp44))))));
			fbargraph22 = fRec24[0];
			double fTemp45 = fTemp44;
			double fTemp46 = (double)input8[i];
			fRec25[0] = max((fRec25[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fbargraph23 = fRec25[0];
			double fTemp47 = fTemp46;
			double fTemp48 = (double)input7[i];
			fRec26[0] = max((fRec26[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fbargraph24 = fRec26[0];
			double fTemp49 = fTemp48;
			double fTemp50 = (double)input6[i];
			fRec27[0] = max((fRec27[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fbargraph25 = fRec27[0];
			double fTemp51 = fTemp50;
			double fTemp52 = (double)input5[i];
			fRec28[0] = max((fRec28[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp52))))));
			fbargraph26 = fRec28[0];
			double fTemp53 = fTemp52;
			double fTemp54 = (double)input4[i];
			fRec29[0] = max((fRec29[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fbargraph27 = fRec29[0];
			double fTemp55 = fTemp54;
			double fTemp56 = (double)input3[i];
			fRec30[0] = max((fRec30[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fbargraph28 = fRec30[0];
			double fTemp57 = fTemp56;
			double fTemp58 = (double)input2[i];
			fRec31[0] = max((fRec31[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fbargraph29 = fRec31[0];
			double fTemp59 = fTemp58;
			double fTemp60 = (double)input1[i];
			fRec32[0] = max((fRec32[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fbargraph30 = fRec32[0];
			double fTemp61 = fTemp60;
			double fTemp62 = (double)input0[i];
			fRec33[0] = max((fRec33[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fbargraph31 = fRec33[0];
			double fTemp63 = fTemp62;
			double fTemp64 = (0.03125 * (fRec1[0] * (fTemp1 + (fTemp3 + (fTemp5 + (fTemp7 + (fTemp9 + (fTemp11 + (fTemp13 + (fTemp15 + (fTemp17 + (fTemp19 + (fTemp21 + (fTemp23 + (fTemp25 + (fTemp27 + (fTemp29 + (fTemp31 + (fTemp33 + (fTemp35 + (fTemp37 + (fTemp39 + (fTemp41 + (fTemp43 + (fTemp45 + (fTemp47 + (fTemp49 + (fTemp51 + (fTemp53 + (fTemp55 + (fTemp57 + (fTemp59 + (fTemp61 + fTemp63)))))))))))))))))))))))))))))))));
			fRec0[0] = max((fRec0[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp64))))));
			fbargraph32 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp64;
			double fTemp65 = (fTemp53 + fTemp49);
			double fTemp66 = (fTemp13 + fTemp9);
			double fTemp67 = (fTemp21 + fTemp17);
			double fTemp68 = (fTemp31 + fTemp27);
			double fTemp69 = (fTemp45 + fTemp41);
			double fTemp70 = (fTemp23 + fTemp15);
			double fTemp71 = (fTemp37 + fTemp35);
			double fTemp72 = (fTemp3 + (fTemp5 - fTemp71));
			double fTemp73 = (fTemp39 + fTemp33);
			double fTemp74 = (fTemp1 + (fTemp7 - fTemp73));
			double fTemp75 = (fRec1[0] * ((((0.031249999999425383 * fTemp65) + ((((0.02845603116607446 * fTemp61) + (0.05056356215458725 * fTemp51)) + (0.02845603115179167 * fTemp25)) + (0.050563562146664694 * fTemp11))) + (0.03125000000535643 * fTemp66)) - (((((0.03124999999974435 * fTemp67) + ((2.0741519909290582e-11 * fTemp68) + ((0.03124999999253746 * fTemp69) + ((((0.028456031178947395 * fTemp57) + (0.050563562158055714 * fTemp43)) + (0.028456031143523002 * fTemp29)) + (0.05056356214319625 * fTemp19))))) + (1.167285910621688e-11 * fTemp70)) + (0.04604282561133058 * fTemp72)) + (0.01931356213232534 * fTemp74))));
			fRec34[0] = max((fRec34[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp75))))));
			fbargraph33 = fRec34[0];
			output1[i] = (FAUSTFLOAT)fTemp75;
			double fTemp76 = (fTemp53 - fTemp49);
			double fTemp77 = (fTemp9 - (fTemp13 + (fTemp17 - (fTemp21 + (fTemp41 - (fTemp45 - fTemp76))))));
			double fTemp78 = (fTemp59 - fTemp63);
			double fTemp79 = (fTemp27 - (fTemp31 - fTemp78));
			double fTemp80 = (fTemp55 - fTemp47);
			double fTemp81 = (fTemp15 - (fTemp23 + fTemp80));
			double fTemp82 = (fTemp37 - fTemp35);
			double fTemp83 = (fTemp3 - (fTemp5 + fTemp82));
			double fTemp84 = (fTemp39 - fTemp33);
			double fTemp85 = (fTemp1 - (fTemp7 + fTemp84));
			double fTemp86 = (fRec1[0] * ((0.0312500000013087 * fTemp77) - ((((0.019313562147752456 * fTemp79) + (0.0460428256143214 * fTemp81)) + (0.02845603116607446 * fTemp83)) + (0.05056356215458725 * fTemp85))));
			fRec35[0] = max((fRec35[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp86))))));
			fbargraph34 = fRec35[0];
			output2[i] = (FAUSTFLOAT)fTemp86;
			double fTemp87 = (fTemp55 + fTemp47);
			double fTemp88 = (fTemp15 + (fTemp23 - fTemp87));
			double fTemp89 = (fTemp59 + fTemp63);
			double fTemp90 = (fTemp27 + (fTemp31 - fTemp89));
			double fTemp91 = (fTemp5 + fTemp3);
			double fTemp92 = (fTemp7 + fTemp1);
			double fTemp93 = (fRec1[0] * ((((0.031249999999265907 * fTemp65) + ((0.019313562123244806 * fTemp43) + (((0.04604282561133058 * fTemp61) + (0.046042825603374665 * fTemp57)) + (0.01931356213232534 * fTemp51)))) + (0.031250000006153834 * fTemp69)) - ((((0.028456031161235202 * fTemp88) + ((0.05056356214869462 * fTemp90) + (((0.031249999998946943 * fTemp67) + ((3.961277810183797e-12 * fTemp73) + ((9.4435413914174e-12 * fTemp71) + (((0.019313562162147378 * fTemp19) + ((0.04604282562526814 * fTemp29) + (0.04604282562015782 * fTemp25))) + (0.01931356215306686 * fTemp11))))) + (0.03124999999333487 * fTemp66)))) + (1.771220524667885e-11 * fTemp91)) + (7.429730299785245e-12 * fTemp92))));
			fRec36[0] = max((fRec36[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fbargraph35 = fRec36[0];
			output3[i] = (FAUSTFLOAT)fTemp93;
			double fTemp94 = (fRec1[0] * ((((((4.3326398788419054e-11 * fTemp68) + ((((0.05412658773668735 * fTemp61) + (0.04034357649425392 * fTemp51)) + (0.054126587710176526 * fTemp29)) + (0.04034357654745972 * fTemp19))) + (1.3722268867176347e-11 * fTemp70)) + (3.3690616435560736e-11 * fTemp91)) + (5.928021680210456e-12 * fTemp92)) - (((3.160618191051283e-12 * fTemp73) + ((1.796268315438824e-11 * fTemp71) + ((((0.054126587751820394 * fTemp57) + (0.04034357647805324 * fTemp43)) + (0.05412658771989692 * fTemp25)) + (0.040343576531259084 * fTemp11)))) + (0.040343576521304395 * (fTemp9 + (fTemp13 - (fTemp17 + (fTemp21 - (fTemp41 + (fTemp45 - fTemp65))))))))));
			fRec37[0] = max((fRec37[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp94))))));
			fbargraph36 = fRec37[0];
			output4[i] = (FAUSTFLOAT)fTemp94;
			double fTemp95 = (fTemp45 - fTemp41);
			double fTemp96 = (fTemp31 - fTemp27);
			double fTemp97 = (fTemp21 - fTemp17);
			double fTemp98 = (fTemp23 - fTemp15);
			double fTemp99 = (fTemp13 - fTemp9);
			double fTemp100 = (fRec1[0] * (((((0.04034357652394163 * fTemp76) + (0.040343576515049356 * fTemp95)) + (0.05412658773668735 * (fTemp3 - (fTemp5 - fTemp82)))) + (0.04034357649425392 * (fTemp1 - (fTemp7 - fTemp84)))) - ((((1.654921172637441e-11 * fTemp96) + (0.040343576524353414 * fTemp97)) + (2.2203097434927284e-11 * fTemp98)) + (0.04034357653159858 * fTemp99))));
			fRec38[0] = max((fRec38[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp100))))));
			fbargraph37 = fRec38[0];
			output5[i] = (FAUSTFLOAT)fTemp100;
			double fTemp101 = (fTemp15 + (fTemp23 + fTemp87));
			double fTemp102 = (fTemp1 + (fTemp7 + fTemp73));
			double fTemp103 = (fTemp27 + (fTemp31 + fTemp89));
			double fTemp104 = (fTemp3 + (fTemp5 + fTemp71));
			double fTemp105 = (fRec1[0] * ((((0.04090678108379355 * fTemp101) + (2.926356954413283e-12 * (fTemp9 + (fTemp13 + (fTemp17 + (fTemp21 + (fTemp41 + (fTemp45 + fTemp65)))))))) + (0.05653178109647902 * fTemp102)) - (((0.02159321892672506 * fTemp103) + (0.034938562148434216 * (fTemp11 + (fTemp19 + (fTemp25 + (fTemp29 + (fTemp43 + (fTemp51 + (fTemp61 + fTemp57))))))))) + (0.005968218925505893 * fTemp104))));
			fRec39[0] = max((fRec39[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp105))))));
			fbargraph38 = fRec39[0];
			output6[i] = (FAUSTFLOAT)fTemp105;
			double fTemp106 = (fTemp5 - fTemp3);
			double fTemp107 = (fTemp7 - fTemp1);
			double fTemp108 = (fRec1[0] * (((0.05412658773099845 * (fTemp15 - (fTemp23 - fTemp80))) + ((0.0403435765217776 * (fTemp27 - (fTemp31 + fTemp78))) + ((0.04034357652373575 * fTemp76) + (0.04034357651607881 * fTemp99)))) - (((((0.040343576532628024 * fTemp95) + ((1.1101548718630453e-11 * fTemp82) + (8.274605857542004e-12 * fTemp84))) + (0.04034357652332397 * fTemp97)) + (2.0821946059249937e-11 * fTemp106)) + (1.5519762259670396e-11 * fTemp107))));
			fRec40[0] = max((fRec40[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp108))))));
			fbargraph39 = fRec40[0];
			output7[i] = (FAUSTFLOAT)fTemp108;
			double fTemp109 = (fRec1[0] * ((((0.052810427283009165 * fTemp103) + ((1.7578661182284158e-11 * fTemp69) + ((((0.027063293867943854 * fTemp61) + (0.027063293837677766 * fTemp57)) + (0.027063293920965504 * fTemp29)) + (0.027063293901524708 * fTemp25)))) + (0.016726035452584633 * fTemp101)) - (((0.043789329326217374 * fTemp104) + ((1.5519762270076438e-11 * fTemp66) + ((1.0294494561038593e-12 * fTemp67) + ((2.0588989122077187e-13 * fTemp65) + ((0.04510548977304693 * fTemp11) + (((0.04510548980614536 * fTemp51) + (0.04510548982063568 * fTemp43)) + (0.04510548975855663 * fTemp19))))))) + (0.007704937489172767 * fTemp102))));
			fRec41[0] = max((fRec41[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp109))))));
			fbargraph40 = fRec41[0];
			output8[i] = (FAUSTFLOAT)fTemp109;
			double fTemp110 = (fRec1[0] * (((0.002969574645052503 * fTemp74) + ((((0.02515864018258366 * fTemp65) + ((((0.06509994890923472 * fTemp61) + (0.029963512991023594 * fTemp43)) + (0.06509994891469752 * fTemp25)) + (0.029963512856939822 * fTemp19))) + (0.025158640168258844 * fTemp66)) + (0.04023398109160045 * fTemp72))) - ((1.1688378238253342e-11 * fTemp70) + ((0.025158640181813286 * fTemp67) + ((6.557589995889373e-11 * fTemp68) + ((0.025158640199219578 * fTemp69) + ((((0.06509994890431116 * fTemp57) + (0.029963512959726046 * fTemp51)) + (0.06509994891786007 * fTemp29)) + (0.02996351288823717 * fTemp11))))))));
			fRec42[0] = max((fRec42[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp110))))));
			fbargraph41 = fRec42[0];
			output9[i] = (FAUSTFLOAT)fTemp110;
			double fTemp111 = (fRec1[0] * (((1.4651625729709834e-11 * fTemp107) + (((4.090282886747088e-11 * fTemp96) + (3.088348228099085e-11 * fTemp98)) + (4.6862090253384154e-11 * fTemp106))) - (((2.4985262017509136e-11 * fTemp82) + (7.811745183791552e-12 * fTemp84)) + (0.061625831073954335 * (fTemp9 - (fTemp13 - (fTemp17 - (fTemp21 - (fTemp41 - (fTemp45 + fTemp76))))))))));
			fRec43[0] = max((fRec43[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp111))))));
			fbargraph42 = fRec43[0];
			output10[i] = (FAUSTFLOAT)fTemp111;
			double fTemp112 = (fRec1[0] * (((0.019487798896868545 * fTemp66) + ((7.050461636270449e-12 * fTemp68) + (((((0.02661817981474453 * fTemp57) + (0.047297881462573126 * fTemp43)) + (0.026618179781608044 * fTemp29)) + (0.047297881448673376 * fTemp19)) + (0.019487798893169893 * fTemp65)))) - ((0.06076361927172445 * fTemp74) + (((2.8586208079238403e-11 * fTemp70) + ((0.0194877988933688 * fTemp67) + ((((0.026618179789342673 * fTemp25) + ((0.026618179802703 * fTemp61) + (0.04729788145932868 * fTemp51))) + (0.0472978814519178 * fTemp11)) + (0.019487798888874513 * fTemp69)))) + (0.016450939837186842 * fTemp72)))));
			fRec44[0] = max((fRec44[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp112))))));
			fbargraph43 = fRec44[0];
			output11[i] = (FAUSTFLOAT)fTemp112;
			double fTemp113 = (fRec1[0] * (((0.03516578892887412 * fTemp83) + (0.03486231065789992 * fTemp79)) - (((0.03182344232408314 * fTemp77) + (0.021733652816759153 * fTemp81)) + (0.052652155785676925 * fTemp85))));
			fRec45[0] = max((fRec45[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp113))))));
			fbargraph44 = fRec45[0];
			output12[i] = (FAUSTFLOAT)fTemp113;
			double fTemp114 = (fRec1[0] * (((((((0.01806618313222911 * fTemp19) + ((0.04306911965218226 * fTemp29) + (0.043069119647402 * fTemp25))) + (0.018066183123735063 * fTemp11)) + (0.01948779889307044 * fTemp65)) + (0.019487798897365818 * fTemp69)) + (0.017187576256784607 * fTemp90)) - ((((0.06968729944244113 * fTemp88) + ((((1.2462826641630935e-11 * fTemp73) + (((((0.04306911963914487 * fTemp61) + (0.0430691196317028 * fTemp57)) + (0.01806618310433315 * fTemp51)) + (0.018066183095839083 * fTemp43)) + (3.374144162906465e-12 * fTemp71))) + (0.01948779889287153 * fTemp67)) + (0.01948779888937179 * fTemp66))) + (6.328508709624312e-12 * fTemp91)) + (2.337514437443605e-11 * fTemp92))));
			fRec46[0] = max((fRec46[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp114))))));
			fbargraph45 = fRec46[0];
			output13[i] = (FAUSTFLOAT)fTemp114;
			double fTemp115 = (fRec1[0] * (((2.370682845777446e-11 * fTemp99) + ((0.01904342906488186 * fTemp85) + (0.06090893311332806 * fTemp83))) - ((1.5725100189748727e-12 * fTemp97) + ((2.6851848495724207e-11 * fTemp95) + ((0.03764379087242118 * fTemp81) + ((3.1450200379497456e-13 * fTemp76) + (0.049856344630062416 * fTemp79)))))));
			fRec47[0] = max((fRec47[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp115))))));
			fbargraph46 = fRec47[0];
			output14[i] = (FAUSTFLOAT)fTemp115;
			double fTemp116 = (fRec1[0] * (((3.4270952038764392e-12 * fTemp92) + (((((1.8272098227973983e-12 * fTemp73) + ((2.4756382394124778e-11 * fTemp71) + ((0.0580917277492798 * fTemp11) + (((0.00587005863616456 * fTemp29) + (0.00587005867123775 * fTemp25)) + (0.05809172776542287 * fTemp19))))) + (0.025158640183739214 * fTemp67)) + (0.025158640197293657 * fTemp66)) + (4.6432806079350595e-11 * fTemp91))) - ((((0.025158640166332916 * fTemp69) + ((0.02515864018296884 * fTemp65) + ((0.05809172769626292 * fTemp43) + (((0.005870058731821087 * fTemp61) + (0.005870058786424278 * fTemp57)) + (0.05809172771240609 * fTemp51))))) + (0.05328685502201964 * fTemp90)) + (0.009497954538401465 * fTemp88))));
			fRec48[0] = max((fRec48[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp116))))));
			fbargraph47 = fRec48[0];
			output15[i] = (FAUSTFLOAT)fTemp116;
			// post processing
			fRec48[1] = fRec48[0];
			fRec47[1] = fRec47[0];
			fRec46[1] = fRec46[0];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec43[1] = fRec43[0];
			fRec42[1] = fRec42[0];
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fRec39[1] = fRec39[0];
			fRec38[1] = fRec38[0];
			fRec37[1] = fRec37[0];
			fRec36[1] = fRec36[0];
			fRec35[1] = fRec35[0];
			fRec34[1] = fRec34[0];
			fRec0[1] = fRec0[0];
			fRec33[1] = fRec33[0];
			fRec32[1] = fRec32[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
			fRec27[1] = fRec27[0];
			fRec26[1] = fRec26[0];
			fRec25[1] = fRec25[0];
			fRec24[1] = fRec24[0];
			fRec23[1] = fRec23[0];
			fRec22[1] = fRec22[0];
			fRec21[1] = fRec21[0];
			fRec20[1] = fRec20[0];
			fRec19[1] = fRec19[0];
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
			fRec3[1] = fRec3[0];
			fRec2[1] = fRec2[0];
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
