//----------------------------------------------------------
// name: "HOAConverter5"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2016"
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
	FAUSTFLOAT 	fslider1;
	double 	fRec1[2];
	FAUSTFLOAT 	fbargraph0;
	double 	fRec0[2];
	FAUSTFLOAT 	fbargraph1;
	double 	fRec3[2];
	FAUSTFLOAT 	fbargraph2;
	double 	fRec4[2];
	FAUSTFLOAT 	fbargraph3;
	double 	fRec5[2];
	FAUSTFLOAT 	fbargraph4;
	double 	fRec2[2];
	FAUSTFLOAT 	fbargraph5;
	double 	fRec6[2];
	FAUSTFLOAT 	fbargraph6;
	double 	fRec7[2];
	FAUSTFLOAT 	fbargraph7;
	double 	fRec9[2];
	FAUSTFLOAT 	fbargraph8;
	double 	fRec10[2];
	FAUSTFLOAT 	fbargraph9;
	double 	fRec11[2];
	FAUSTFLOAT 	fbargraph10;
	double 	fRec8[2];
	FAUSTFLOAT 	fbargraph11;
	double 	fRec13[2];
	FAUSTFLOAT 	fbargraph12;
	double 	fRec14[2];
	FAUSTFLOAT 	fbargraph13;
	double 	fRec12[2];
	FAUSTFLOAT 	fbargraph14;
	double 	fRec15[2];
	FAUSTFLOAT 	fbargraph15;
	double 	fRec16[2];
	FAUSTFLOAT 	fbargraph16;
	double 	fRec17[2];
	FAUSTFLOAT 	fbargraph17;
	double 	fRec19[2];
	FAUSTFLOAT 	fbargraph18;
	double 	fRec20[2];
	FAUSTFLOAT 	fbargraph19;
	double 	fRec21[2];
	FAUSTFLOAT 	fbargraph20;
	double 	fRec18[2];
	FAUSTFLOAT 	fbargraph21;
	double 	fRec23[2];
	FAUSTFLOAT 	fbargraph22;
	double 	fRec24[2];
	FAUSTFLOAT 	fbargraph23;
	double 	fRec22[2];
	FAUSTFLOAT 	fbargraph24;
	double 	fRec26[2];
	FAUSTFLOAT 	fbargraph25;
	double 	fRec25[2];
	FAUSTFLOAT 	fbargraph26;
	double 	fRec28[2];
	FAUSTFLOAT 	fbargraph27;
	double 	fRec27[2];
	FAUSTFLOAT 	fbargraph28;
	double 	fRec29[2];
	FAUSTFLOAT 	fbargraph29;
	double 	fRec30[2];
	FAUSTFLOAT 	fbargraph30;
	double 	fRec31[2];
	FAUSTFLOAT 	fbargraph31;
	double 	fRec33[2];
	FAUSTFLOAT 	fbargraph32;
	double 	fRec32[2];
	FAUSTFLOAT 	fbargraph33;
	double 	fRec35[2];
	FAUSTFLOAT 	fbargraph34;
	double 	fRec34[2];
	FAUSTFLOAT 	fbargraph35;
	double 	fRec37[2];
	FAUSTFLOAT 	fbargraph36;
	double 	fRec36[2];
	FAUSTFLOAT 	fbargraph37;
	double 	fRec39[2];
	FAUSTFLOAT 	fbargraph38;
	double 	fRec38[2];
	FAUSTFLOAT 	fbargraph39;
	double 	fRec41[2];
	FAUSTFLOAT 	fbargraph40;
	double 	fRec40[2];
	FAUSTFLOAT 	fbargraph41;
	double 	fRec43[2];
	FAUSTFLOAT 	fbargraph42;
	double 	fRec42[2];
	FAUSTFLOAT 	fbargraph43;
	double 	fRec45[2];
	FAUSTFLOAT 	fbargraph44;
	double 	fRec44[2];
	FAUSTFLOAT 	fbargraph45;
	double 	fRec47[2];
	FAUSTFLOAT 	fbargraph46;
	double 	fRec46[2];
	FAUSTFLOAT 	fbargraph47;
	double 	fRec49[2];
	FAUSTFLOAT 	fbargraph48;
	double 	fRec48[2];
	FAUSTFLOAT 	fbargraph49;
	double 	fRec51[2];
	FAUSTFLOAT 	fbargraph50;
	double 	fRec50[2];
	FAUSTFLOAT 	fbargraph51;
	double 	fRec53[2];
	FAUSTFLOAT 	fbargraph52;
	double 	fRec52[2];
	FAUSTFLOAT 	fbargraph53;
	double 	fRec55[2];
	FAUSTFLOAT 	fbargraph54;
	double 	fRec54[2];
	FAUSTFLOAT 	fbargraph55;
	double 	fRec57[2];
	FAUSTFLOAT 	fbargraph56;
	double 	fRec56[2];
	FAUSTFLOAT 	fbargraph57;
	double 	fRec59[2];
	FAUSTFLOAT 	fbargraph58;
	double 	fRec58[2];
	FAUSTFLOAT 	fbargraph59;
	double 	fRec61[2];
	FAUSTFLOAT 	fbargraph60;
	double 	fRec60[2];
	FAUSTFLOAT 	fbargraph61;
	double 	fRec63[2];
	FAUSTFLOAT 	fbargraph62;
	double 	fRec62[2];
	FAUSTFLOAT 	fbargraph63;
	double 	fRec65[2];
	FAUSTFLOAT 	fbargraph64;
	double 	fRec64[2];
	FAUSTFLOAT 	fbargraph65;
	double 	fRec67[2];
	FAUSTFLOAT 	fbargraph66;
	double 	fRec66[2];
	FAUSTFLOAT 	fbargraph67;
	double 	fRec69[2];
	FAUSTFLOAT 	fbargraph68;
	double 	fRec68[2];
	FAUSTFLOAT 	fbargraph69;
	double 	fRec71[2];
	FAUSTFLOAT 	fbargraph70;
	double 	fRec70[2];
	FAUSTFLOAT 	fbargraph71;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("name", "HOAConverter5");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
		m->declare("lib/gui.lib/name", "GUI Library");
		m->declare("lib/gui.lib/version", "1.0");
		m->declare("lib/gui.lib/author", "Pierre Lecomte");
		m->declare("lib/gui.lib/license", "GPL");
		m->declare("lib/gui.lib/copyright", "(c) Pierre Lecomte 2016");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "0.0");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
	}

	virtual int getNumInputs() { return 36; }
	virtual int getNumOutputs() { return 36; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = (8e+01 / min(1.92e+05, max(1e+03, (double)fSamplingFreq)));
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 1.0;
		fslider1 = 1.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec64[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
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
		ui_interface->openVerticalBox("HOAConverter5");
		ui_interface->declare(&fslider0, "0", "");
		ui_interface->declare(&fslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Input (1-ACN_N3D,2-ACN_SN3D, 3-FuMa)", &fslider0, 1.0, 1.0, 3.0, 1.0);
		ui_interface->declare(&fslider1, "0", "");
		ui_interface->declare(&fslider1, "style", "knob");
		ui_interface->addHorizontalSlider("Output (1-ACN_N3D,2-ACN_SN3D, 3-FuMa)", &fslider1, 1.0, 1.0, 3.0, 1.0);
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("Inputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph0, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa513690", &fbargraph0, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph2, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa517250", &fbargraph2, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph3, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa518a20", &fbargraph3, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph4, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa51a460", &fbargraph4, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph8, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa521d60", &fbargraph8, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph12, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa528b40", &fbargraph12, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph10, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa525210", &fbargraph10, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph13, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa52a750", &fbargraph13, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph9, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa523660", &fbargraph9, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph18, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa534f20", &fbargraph18, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph22, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa53bf00", &fbargraph22, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph25, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa5418b0", &fbargraph25, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph20, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa5384d0", &fbargraph20, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph23, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa53d890", &fbargraph23, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph27, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa545a20", &fbargraph27, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph19, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa5368b0", &fbargraph19, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph32, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa550c80", &fbargraph32, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph34, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa5546a0", &fbargraph34, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph36, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa557f20", &fbargraph36, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph38, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa55b880", &fbargraph38, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph40, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa55f2c0", &fbargraph40, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph42, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa562de0", &fbargraph42, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph44, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa5669e0", &fbargraph44, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph46, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa56a6c0", &fbargraph46, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph48, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa56e480", &fbargraph48, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5");
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph50, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa207fc0", &fbargraph50, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph52, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa20b5d0", &fbargraph52, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph54, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa20ed70", &fbargraph54, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph56, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa2125d0", &fbargraph56, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph58, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2bee34ce0", &fbargraph58, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph60, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1ae260", &fbargraph60, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph62, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1a9490", &fbargraph62, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph64, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1b3050", &fbargraph64, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph66, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1b6cf0", &fbargraph66, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph68, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1baa50", &fbargraph68, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph70, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1be870", &fbargraph70, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openHorizontalBox("Outputs");
		ui_interface->openHorizontalBox("0");
		ui_interface->openVerticalBox("0");
		ui_interface->declare(&fbargraph1, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa515700", &fbargraph1, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("1");
		ui_interface->openVerticalBox("1");
		ui_interface->declare(&fbargraph5, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa51c500", &fbargraph5, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("2");
		ui_interface->declare(&fbargraph6, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa51e3a0", &fbargraph6, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("3");
		ui_interface->declare(&fbargraph7, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa520100", &fbargraph7, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("2");
		ui_interface->openVerticalBox("4");
		ui_interface->declare(&fbargraph11, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa526f80", &fbargraph11, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("5");
		ui_interface->declare(&fbargraph14, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa52c5c0", &fbargraph14, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("6");
		ui_interface->declare(&fbargraph15, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa52e6b0", &fbargraph15, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("7");
		ui_interface->declare(&fbargraph16, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa530bd0", &fbargraph16, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("8");
		ui_interface->declare(&fbargraph17, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa533140", &fbargraph17, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("3");
		ui_interface->openVerticalBox("9");
		ui_interface->declare(&fbargraph21, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa53a2f0", &fbargraph21, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("10");
		ui_interface->declare(&fbargraph24, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa53fb20", &fbargraph24, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("11");
		ui_interface->declare(&fbargraph26, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa543a70", &fbargraph26, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("12");
		ui_interface->declare(&fbargraph28, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa5478f0", &fbargraph28, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("13");
		ui_interface->declare(&fbargraph29, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa549ea0", &fbargraph29, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("14");
		ui_interface->declare(&fbargraph30, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa54c5d0", &fbargraph30, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("15");
		ui_interface->declare(&fbargraph31, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa54ed70", &fbargraph31, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("4");
		ui_interface->openVerticalBox("16");
		ui_interface->declare(&fbargraph33, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa5529f0", &fbargraph33, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("17");
		ui_interface->declare(&fbargraph35, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa556200", &fbargraph35, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("18");
		ui_interface->declare(&fbargraph37, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa559af0", &fbargraph37, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("19");
		ui_interface->declare(&fbargraph39, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa55d4c0", &fbargraph39, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("20");
		ui_interface->declare(&fbargraph41, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa560f70", &fbargraph41, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("21");
		ui_interface->declare(&fbargraph43, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa564b00", &fbargraph43, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("22");
		ui_interface->declare(&fbargraph45, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa568770", &fbargraph45, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("23");
		ui_interface->declare(&fbargraph47, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa56c4c0", &fbargraph47, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("24");
		ui_interface->declare(&fbargraph49, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa5702f0", &fbargraph49, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5");
		ui_interface->openVerticalBox("25");
		ui_interface->declare(&fbargraph51, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa2098f0", &fbargraph51, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("26");
		ui_interface->declare(&fbargraph53, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa20d090", &fbargraph53, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("27");
		ui_interface->declare(&fbargraph55, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa210890", &fbargraph55, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("28");
		ui_interface->declare(&fbargraph57, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa214150", &fbargraph57, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("29");
		ui_interface->declare(&fbargraph59, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1afec0", &fbargraph59, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("30");
		ui_interface->declare(&fbargraph61, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1ab620", &fbargraph61, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("31");
		ui_interface->declare(&fbargraph63, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1b11e0", &fbargraph63, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("32");
		ui_interface->declare(&fbargraph65, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1b4d90", &fbargraph65, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("33");
		ui_interface->declare(&fbargraph67, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1b8a90", &fbargraph67, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("34");
		ui_interface->declare(&fbargraph69, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1bc850", &fbargraph69, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->openVerticalBox("35");
		ui_interface->declare(&fbargraph71, "unit", "dB");
		ui_interface->addVerticalBargraph("0x7ff2aa1c06d0", &fbargraph71, -7e+01, 6.0);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		int 	iSlow0 = int((((3 * rint(double(fslider0))) + rint(double(fslider1))) + -4));
		int 	iSlow1 = int((iSlow0 >= 5));
		int 	iSlow2 = int((iSlow0 >= 7));
		int 	iSlow3 = int((iSlow0 >= 8));
		int 	iSlow4 = int((iSlow0 >= 6));
		int 	iSlow5 = int((iSlow0 >= 3));
		int 	iSlow6 = int((iSlow0 >= 2));
		int 	iSlow7 = int((iSlow0 >= 4));
		int 	iSlow8 = int((iSlow0 >= 1));
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
		for (int i=0; i<count; i++) {
			double fTemp0 = (double)input0[i];
			fRec1[0] = max((fRec1[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp0))))));
			fbargraph0 = fRec1[0];
			double fTemp1 = fTemp0;
			double fTemp2 = (1.4142135623730951 * fTemp1);
			double fTemp3 = (0.7071067811865475 * fTemp1);
			double fTemp4 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp1:fTemp2):((iSlow4)?fTemp2:fTemp3)):((iSlow5)?fTemp1:((iSlow6)?fTemp3:fTemp1)));
			fRec0[0] = max((fRec0[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp4))))));
			fbargraph1 = fRec0[0];
			output0[i] = (FAUSTFLOAT)fTemp4;
			double fTemp5 = (double)input1[i];
			fRec3[0] = max((fRec3[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp5))))));
			fbargraph2 = fRec3[0];
			double fTemp6 = fTemp5;
			double fTemp7 = (double)input2[i];
			fRec4[0] = max((fRec4[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp7))))));
			fbargraph3 = fRec4[0];
			double fTemp8 = fTemp7;
			double fTemp9 = (1.7320508075688772 * fTemp8);
			double fTemp10 = (double)input3[i];
			fRec5[0] = max((fRec5[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp10))))));
			fbargraph4 = fRec5[0];
			double fTemp11 = fTemp10;
			double fTemp12 = (1.7320508075688772 * fTemp6);
			double fTemp13 = (0.5773502691896258 * fTemp11);
			double fTemp14 = (0.5773502691896258 * fTemp6);
			double fTemp15 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp6:fTemp8):((iSlow4)?fTemp9:fTemp11)):((iSlow5)?((iSlow7)?fTemp6:fTemp12):((iSlow6)?fTemp13:((iSlow8)?fTemp14:fTemp6))));
			fRec2[0] = max((fRec2[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp15))))));
			fbargraph5 = fRec2[0];
			output1[i] = (FAUSTFLOAT)fTemp15;
			double fTemp16 = (1.7320508075688772 * fTemp11);
			double fTemp17 = (0.5773502691896258 * fTemp8);
			double fTemp18 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp8:fTemp11):((iSlow4)?fTemp16:fTemp6)):((iSlow5)?((iSlow7)?fTemp8:fTemp9):((iSlow6)?fTemp14:((iSlow8)?fTemp17:fTemp8))));
			fRec6[0] = max((fRec6[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp18))))));
			fbargraph6 = fRec6[0];
			output2[i] = (FAUSTFLOAT)fTemp18;
			double fTemp19 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp11:fTemp6):((iSlow4)?fTemp12:fTemp8)):((iSlow5)?((iSlow7)?fTemp11:fTemp16):((iSlow6)?fTemp17:((iSlow8)?fTemp13:fTemp11))));
			fRec7[0] = max((fRec7[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp19))))));
			fbargraph7 = fRec7[0];
			output3[i] = (FAUSTFLOAT)fTemp19;
			double fTemp20 = (double)input4[i];
			fRec9[0] = max((fRec9[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp20))))));
			fbargraph8 = fRec9[0];
			double fTemp21 = fTemp20;
			double fTemp22 = (double)input8[i];
			fRec10[0] = max((fRec10[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp22))))));
			fbargraph9 = fRec10[0];
			double fTemp23 = fTemp22;
			double fTemp24 = (double)input6[i];
			fRec11[0] = max((fRec11[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp24))))));
			fbargraph10 = fRec11[0];
			double fTemp25 = fTemp24;
			double fTemp26 = (2.23606797749979 * fTemp21);
			double fTemp27 = (0.4472135954999579 * fTemp25);
			double fTemp28 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp21:(0.8660254037844387 * fTemp23)):((iSlow4)?(1.9364916731037085 * fTemp23):fTemp25)):((iSlow5)?((iSlow7)?fTemp21:fTemp26):((iSlow6)?fTemp27:((iSlow8)?(0.4472135954999579 * fTemp21):fTemp21))));
			fRec8[0] = max((fRec8[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp28))))));
			fbargraph11 = fRec8[0];
			output4[i] = (FAUSTFLOAT)fTemp28;
			double fTemp29 = (double)input5[i];
			fRec13[0] = max((fRec13[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp29))))));
			fbargraph12 = fRec13[0];
			double fTemp30 = fTemp29;
			double fTemp31 = (double)input7[i];
			fRec14[0] = max((fRec14[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp31))))));
			fbargraph13 = fRec14[0];
			double fTemp32 = fTemp31;
			double fTemp33 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp30:(0.8660254037844387 * fTemp25)):((iSlow4)?(1.9364916731037085 * fTemp25):(1.1547005383792515 * fTemp32))):((iSlow5)?((iSlow7)?fTemp30:(2.23606797749979 * fTemp30)):((iSlow6)?(0.5163977794943222 * fTemp32):((iSlow8)?(0.4472135954999579 * fTemp30):fTemp30))));
			fRec12[0] = max((fRec12[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp33))))));
			fbargraph14 = fRec12[0];
			output5[i] = (FAUSTFLOAT)fTemp33;
			double fTemp34 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp25:fTemp21):((iSlow4)?fTemp26:(1.1547005383792515 * fTemp30))):((iSlow5)?((iSlow7)?fTemp25:(2.23606797749979 * fTemp25)):((iSlow6)?(0.5163977794943222 * fTemp30):((iSlow8)?fTemp27:fTemp25))));
			fRec15[0] = max((fRec15[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp34))))));
			fbargraph15 = fRec15[0];
			output6[i] = (FAUSTFLOAT)fTemp34;
			double fTemp35 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp32:(0.8660254037844387 * fTemp30)):((iSlow4)?(1.9364916731037085 * fTemp30):(1.1547005383792515 * fTemp23))):((iSlow5)?((iSlow7)?fTemp32:(2.23606797749979 * fTemp32)):((iSlow6)?(0.5163977794943222 * fTemp23):((iSlow8)?(0.4472135954999579 * fTemp32):fTemp32))));
			fRec16[0] = max((fRec16[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp35))))));
			fbargraph16 = fRec16[0];
			output7[i] = (FAUSTFLOAT)fTemp35;
			double fTemp36 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp23:(0.8660254037844387 * fTemp32)):((iSlow4)?(1.9364916731037085 * fTemp32):(1.1547005383792515 * fTemp21))):((iSlow5)?((iSlow7)?fTemp23:(2.23606797749979 * fTemp23)):((iSlow6)?(0.5163977794943222 * fTemp21):((iSlow8)?(0.4472135954999579 * fTemp23):fTemp23))));
			fRec17[0] = max((fRec17[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp36))))));
			fbargraph17 = fRec17[0];
			output8[i] = (FAUSTFLOAT)fTemp36;
			double fTemp37 = (double)input9[i];
			fRec19[0] = max((fRec19[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp37))))));
			fbargraph18 = fRec19[0];
			double fTemp38 = fTemp37;
			double fTemp39 = (double)input15[i];
			fRec20[0] = max((fRec20[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp39))))));
			fbargraph19 = fRec20[0];
			double fTemp40 = fTemp39;
			double fTemp41 = (double)input12[i];
			fRec21[0] = max((fRec21[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp41))))));
			fbargraph20 = fRec21[0];
			double fTemp42 = fTemp41;
			double fTemp43 = (2.6457513110645907 * fTemp38);
			double fTemp44 = (0.3779644730092272 * fTemp42);
			double fTemp45 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp38:(0.7905694150420948 * fTemp40)):((iSlow4)?(2.091650066335189 * fTemp40):fTemp42)):((iSlow5)?((iSlow7)?fTemp38:fTemp43):((iSlow6)?fTemp44:((iSlow8)?(0.3779644730092272 * fTemp38):fTemp38))));
			fRec18[0] = max((fRec18[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp45))))));
			fbargraph21 = fRec18[0];
			output9[i] = (FAUSTFLOAT)fTemp45;
			double fTemp46 = (double)input10[i];
			fRec23[0] = max((fRec23[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp46))))));
			fbargraph22 = fRec23[0];
			double fTemp47 = fTemp46;
			double fTemp48 = (double)input13[i];
			fRec24[0] = max((fRec24[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp48))))));
			fbargraph23 = fRec24[0];
			double fTemp49 = fTemp48;
			double fTemp50 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp47:(0.7453559924999299 * fTemp49)):((iSlow4)?(1.9720265943665387 * fTemp49):(1.1858541225631423 * fTemp49))):((iSlow5)?((iSlow7)?fTemp47:(2.6457513110645907 * fTemp47)):((iSlow6)?(0.4482107285003976 * fTemp49):((iSlow8)?(0.3779644730092272 * fTemp47):fTemp47))));
			fRec22[0] = max((fRec22[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp50))))));
			fbargraph24 = fRec22[0];
			output10[i] = (FAUSTFLOAT)fTemp50;
			double fTemp51 = (double)input11[i];
			fRec26[0] = max((fRec26[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp51))))));
			fbargraph25 = fRec26[0];
			double fTemp52 = fTemp51;
			double fTemp53 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp52:(0.8432740427115678 * fTemp52)):((iSlow4)?(2.2310934040908683 * fTemp52):(1.1858541225631423 * fTemp52))):((iSlow5)?((iSlow7)?fTemp52:(2.6457513110645907 * fTemp52)):((iSlow6)?(0.4482107285003976 * fTemp52):((iSlow8)?(0.3779644730092272 * fTemp52):fTemp52))));
			fRec25[0] = max((fRec25[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp53))))));
			fbargraph26 = fRec25[0];
			output11[i] = (FAUSTFLOAT)fTemp53;
			double fTemp54 = (double)input14[i];
			fRec28[0] = max((fRec28[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp54))))));
			fbargraph27 = fRec28[0];
			double fTemp55 = fTemp54;
			double fTemp56 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp42:fTemp38):((iSlow4)?fTemp43:(1.3416407864998738 * fTemp55))):((iSlow5)?((iSlow7)?fTemp42:(2.6457513110645907 * fTemp42)):((iSlow6)?(0.50709255283711 * fTemp55):((iSlow8)?fTemp44:fTemp42))));
			fRec27[0] = max((fRec27[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp56))))));
			fbargraph28 = fRec27[0];
			output12[i] = (FAUSTFLOAT)fTemp56;
			double fTemp57 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp49:(0.8432740427115678 * fTemp47)):((iSlow4)?(2.2310934040908683 * fTemp47):(1.3416407864998738 * fTemp47))):((iSlow5)?((iSlow7)?fTemp49:(2.6457513110645907 * fTemp49)):((iSlow6)?(0.50709255283711 * fTemp47):((iSlow8)?(0.3779644730092272 * fTemp49):fTemp49))));
			fRec29[0] = max((fRec29[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp57))))));
			fbargraph29 = fRec29[0];
			output13[i] = (FAUSTFLOAT)fTemp57;
			double fTemp58 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp55:(0.7453559924999299 * fTemp42)):((iSlow4)?(1.9720265943665387 * fTemp42):(1.2649110640673518 * fTemp40))):((iSlow5)?((iSlow7)?fTemp55:(2.6457513110645907 * fTemp55)):((iSlow6)?(0.47809144373375745 * fTemp40):((iSlow8)?(0.3779644730092272 * fTemp55):fTemp55))));
			fRec30[0] = max((fRec30[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp58))))));
			fbargraph30 = fRec30[0];
			output14[i] = (FAUSTFLOAT)fTemp58;
			double fTemp59 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp40:(0.7905694150420948 * fTemp55)):((iSlow4)?(2.091650066335189 * fTemp55):(1.2649110640673518 * fTemp38))):((iSlow5)?((iSlow7)?fTemp40:(2.6457513110645907 * fTemp40)):((iSlow6)?(0.47809144373375745 * fTemp38):((iSlow8)?(0.3779644730092272 * fTemp40):fTemp40))));
			fRec31[0] = max((fRec31[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp59))))));
			fbargraph31 = fRec31[0];
			output15[i] = (FAUSTFLOAT)fTemp59;
			double fTemp60 = (double)input16[i];
			fRec33[0] = max((fRec33[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp60))))));
			fbargraph32 = fRec33[0];
			double fTemp61 = fTemp60;
			double fTemp62 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp61:0.0):0):((iSlow5)?((iSlow7)?fTemp61:(3.0 * fTemp61)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp61):fTemp61))));
			fRec32[0] = max((fRec32[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp62))))));
			fbargraph33 = fRec32[0];
			output16[i] = (FAUSTFLOAT)fTemp62;
			double fTemp63 = (double)input17[i];
			fRec35[0] = max((fRec35[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp63))))));
			fbargraph34 = fRec35[0];
			double fTemp64 = fTemp63;
			double fTemp65 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp64:0.0):0):((iSlow5)?((iSlow7)?fTemp64:(3.0 * fTemp64)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp64):fTemp64))));
			fRec34[0] = max((fRec34[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp65))))));
			fbargraph35 = fRec34[0];
			output17[i] = (FAUSTFLOAT)fTemp65;
			double fTemp66 = (double)input18[i];
			fRec37[0] = max((fRec37[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp66))))));
			fbargraph36 = fRec37[0];
			double fTemp67 = fTemp66;
			double fTemp68 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp67:0.0):0):((iSlow5)?((iSlow7)?fTemp67:(3.0 * fTemp67)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp67):fTemp67))));
			fRec36[0] = max((fRec36[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp68))))));
			fbargraph37 = fRec36[0];
			output18[i] = (FAUSTFLOAT)fTemp68;
			double fTemp69 = (double)input19[i];
			fRec39[0] = max((fRec39[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp69))))));
			fbargraph38 = fRec39[0];
			double fTemp70 = fTemp69;
			double fTemp71 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp70:0.0):0):((iSlow5)?((iSlow7)?fTemp70:(3.0 * fTemp70)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp70):fTemp70))));
			fRec38[0] = max((fRec38[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp71))))));
			fbargraph39 = fRec38[0];
			output19[i] = (FAUSTFLOAT)fTemp71;
			double fTemp72 = (double)input20[i];
			fRec41[0] = max((fRec41[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp72))))));
			fbargraph40 = fRec41[0];
			double fTemp73 = fTemp72;
			double fTemp74 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp73:0.0):0):((iSlow5)?((iSlow7)?fTemp73:(3.0 * fTemp73)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp73):fTemp73))));
			fRec40[0] = max((fRec40[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp74))))));
			fbargraph41 = fRec40[0];
			output20[i] = (FAUSTFLOAT)fTemp74;
			double fTemp75 = (double)input21[i];
			fRec43[0] = max((fRec43[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp75))))));
			fbargraph42 = fRec43[0];
			double fTemp76 = fTemp75;
			double fTemp77 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp76:0.0):0):((iSlow5)?((iSlow7)?fTemp76:(3.0 * fTemp76)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp76):fTemp76))));
			fRec42[0] = max((fRec42[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp77))))));
			fbargraph43 = fRec42[0];
			output21[i] = (FAUSTFLOAT)fTemp77;
			double fTemp78 = (double)input22[i];
			fRec45[0] = max((fRec45[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp78))))));
			fbargraph44 = fRec45[0];
			double fTemp79 = fTemp78;
			double fTemp80 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp79:0.0):0):((iSlow5)?((iSlow7)?fTemp79:(3.0 * fTemp79)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp79):fTemp79))));
			fRec44[0] = max((fRec44[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp80))))));
			fbargraph45 = fRec44[0];
			output22[i] = (FAUSTFLOAT)fTemp80;
			double fTemp81 = (double)input23[i];
			fRec47[0] = max((fRec47[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp81))))));
			fbargraph46 = fRec47[0];
			double fTemp82 = fTemp81;
			double fTemp83 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp82:0.0):0):((iSlow5)?((iSlow7)?fTemp82:(3.0 * fTemp82)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp82):fTemp82))));
			fRec46[0] = max((fRec46[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp83))))));
			fbargraph47 = fRec46[0];
			output23[i] = (FAUSTFLOAT)fTemp83;
			double fTemp84 = (double)input24[i];
			fRec49[0] = max((fRec49[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp84))))));
			fbargraph48 = fRec49[0];
			double fTemp85 = fTemp84;
			double fTemp86 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp85:0.0):0):((iSlow5)?((iSlow7)?fTemp85:(3.0 * fTemp85)):((iSlow6)?0:((iSlow8)?(0.3333333333333333 * fTemp85):fTemp85))));
			fRec48[0] = max((fRec48[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp86))))));
			fbargraph49 = fRec48[0];
			output24[i] = (FAUSTFLOAT)fTemp86;
			double fTemp87 = (double)input25[i];
			fRec51[0] = max((fRec51[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp87))))));
			fbargraph50 = fRec51[0];
			double fTemp88 = fTemp87;
			double fTemp89 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp88:0.0):0):((iSlow5)?((iSlow7)?fTemp88:(3.3166247903554 * fTemp88)):((iSlow6)?0:((iSlow8)?(0.30151134457776363 * fTemp88):fTemp88))));
			fRec50[0] = max((fRec50[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp89))))));
			fbargraph51 = fRec50[0];
			output25[i] = (FAUSTFLOAT)fTemp89;
			double fTemp90 = (double)input26[i];
			fRec53[0] = max((fRec53[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp90))))));
			fbargraph52 = fRec53[0];
			double fTemp91 = fTemp90;
			double fTemp92 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp91:0.0):0):((iSlow5)?((iSlow7)?fTemp91:(3.3166247903554 * fTemp91)):((iSlow6)?0:((iSlow8)?(0.30151134457776363 * fTemp91):fTemp91))));
			fRec52[0] = max((fRec52[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp92))))));
			fbargraph53 = fRec52[0];
			output26[i] = (FAUSTFLOAT)fTemp92;
			double fTemp93 = (double)input27[i];
			fRec55[0] = max((fRec55[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp93))))));
			fbargraph54 = fRec55[0];
			double fTemp94 = fTemp93;
			double fTemp95 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp94:0.0):0):((iSlow5)?((iSlow7)?fTemp94:(3.3166247903554 * fTemp94)):((iSlow6)?0:((iSlow8)?(0.30151134457776363 * fTemp94):fTemp94))));
			fRec54[0] = max((fRec54[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp95))))));
			fbargraph55 = fRec54[0];
			output27[i] = (FAUSTFLOAT)fTemp95;
			double fTemp96 = (double)input28[i];
			fRec57[0] = max((fRec57[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp96))))));
			fbargraph56 = fRec57[0];
			double fTemp97 = fTemp96;
			double fTemp98 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp97:0.0):0):((iSlow5)?((iSlow7)?fTemp97:(3.3166247903554 * fTemp97)):((iSlow6)?0:((iSlow8)?(0.30151134457776363 * fTemp97):fTemp97))));
			fRec56[0] = max((fRec56[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp98))))));
			fbargraph57 = fRec56[0];
			output28[i] = (FAUSTFLOAT)fTemp98;
			double fTemp99 = (double)input29[i];
			fRec59[0] = max((fRec59[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp99))))));
			fbargraph58 = fRec59[0];
			double fTemp100 = fTemp99;
			double fTemp101 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp100:0.0):0):((iSlow5)?((iSlow7)?fTemp100:(3.3166247903554 * fTemp100)):((iSlow6)?0:((iSlow8)?(0.30151134457776363 * fTemp100):fTemp100))));
			fRec58[0] = max((fRec58[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp101))))));
			fbargraph59 = fRec58[0];
			output29[i] = (FAUSTFLOAT)fTemp101;
			double fTemp102 = (double)input30[i];
			fRec61[0] = max((fRec61[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp102))))));
			fbargraph60 = fRec61[0];
			double fTemp103 = fTemp102;
			double fTemp104 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp103:0.0):0):((iSlow5)?((iSlow7)?fTemp103:(3.3166247903554 * fTemp103)):((iSlow6)?0:((iSlow8)?(0.30151134457776363 * fTemp103):fTemp103))));
			fRec60[0] = max((fRec60[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp104))))));
			fbargraph61 = fRec60[0];
			output30[i] = (FAUSTFLOAT)fTemp104;
			double fTemp105 = (double)input31[i];
			fRec63[0] = max((fRec63[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp105))))));
			fbargraph62 = fRec63[0];
			double fTemp106 = fTemp105;
			double fTemp107 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp106:0.0):0):((iSlow5)?((iSlow7)?fTemp106:(3.3166247903554 * fTemp106)):((iSlow6)?0:((iSlow8)?(0.30151134457776363 * fTemp106):fTemp106))));
			fRec62[0] = max((fRec62[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp107))))));
			fbargraph63 = fRec62[0];
			output31[i] = (FAUSTFLOAT)fTemp107;
			double fTemp108 = (double)input32[i];
			fRec65[0] = max((fRec65[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp108))))));
			fbargraph64 = fRec65[0];
			double fTemp109 = fTemp108;
			double fTemp110 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp109:0.0):0):((iSlow5)?((iSlow7)?fTemp109:(3.3166247903554 * fTemp109)):((iSlow6)?0:((iSlow8)?(0.30151134457776363 * fTemp109):fTemp109))));
			fRec64[0] = max((fRec64[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp110))))));
			fbargraph65 = fRec64[0];
			output32[i] = (FAUSTFLOAT)fTemp110;
			double fTemp111 = (double)input33[i];
			fRec67[0] = max((fRec67[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp111))))));
			fbargraph66 = fRec67[0];
			double fTemp112 = fTemp111;
			double fTemp113 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp112:0.0):0):((iSlow5)?((iSlow7)?fTemp112:(3.3166247903554 * fTemp112)):((iSlow6)?0:((iSlow8)?(0.30151134457776363 * fTemp112):fTemp112))));
			fRec66[0] = max((fRec66[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp113))))));
			fbargraph67 = fRec66[0];
			output33[i] = (FAUSTFLOAT)fTemp113;
			double fTemp114 = (double)input34[i];
			fRec69[0] = max((fRec69[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp114))))));
			fbargraph68 = fRec69[0];
			double fTemp115 = fTemp114;
			double fTemp116 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp115:0.0):0):((iSlow5)?((iSlow7)?fTemp115:(3.3166247903554 * fTemp115)):((iSlow6)?0:((iSlow8)?(0.30151134457776363 * fTemp115):fTemp115))));
			fRec68[0] = max((fRec68[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp116))))));
			fbargraph69 = fRec68[0];
			output34[i] = (FAUSTFLOAT)fTemp116;
			double fTemp117 = (double)input35[i];
			fRec71[0] = max((fRec71[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp117))))));
			fbargraph70 = fRec71[0];
			double fTemp118 = fTemp117;
			double fTemp119 = ((iSlow1)?((iSlow2)?((iSlow3)?fTemp118:0.0):0):((iSlow5)?((iSlow7)?fTemp118:(3.3166247903554 * fTemp118)):((iSlow6)?0:((iSlow8)?(0.30151134457776363 * fTemp118):fTemp118))));
			fRec70[0] = max((fRec70[1] - fConst0), min((double)6, (20 * log10(max(0.00031622776601683794, fabs(fTemp119))))));
			fbargraph71 = fRec70[0];
			output35[i] = (FAUSTFLOAT)fTemp119;
			// post processing
			fRec70[1] = fRec70[0];
			fRec71[1] = fRec71[0];
			fRec68[1] = fRec68[0];
			fRec69[1] = fRec69[0];
			fRec66[1] = fRec66[0];
			fRec67[1] = fRec67[0];
			fRec64[1] = fRec64[0];
			fRec65[1] = fRec65[0];
			fRec62[1] = fRec62[0];
			fRec63[1] = fRec63[0];
			fRec60[1] = fRec60[0];
			fRec61[1] = fRec61[0];
			fRec58[1] = fRec58[0];
			fRec59[1] = fRec59[0];
			fRec56[1] = fRec56[0];
			fRec57[1] = fRec57[0];
			fRec54[1] = fRec54[0];
			fRec55[1] = fRec55[0];
			fRec52[1] = fRec52[0];
			fRec53[1] = fRec53[0];
			fRec50[1] = fRec50[0];
			fRec51[1] = fRec51[0];
			fRec48[1] = fRec48[0];
			fRec49[1] = fRec49[0];
			fRec46[1] = fRec46[0];
			fRec47[1] = fRec47[0];
			fRec44[1] = fRec44[0];
			fRec45[1] = fRec45[0];
			fRec42[1] = fRec42[0];
			fRec43[1] = fRec43[0];
			fRec40[1] = fRec40[0];
			fRec41[1] = fRec41[0];
			fRec38[1] = fRec38[0];
			fRec39[1] = fRec39[0];
			fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			fRec32[1] = fRec32[0];
			fRec33[1] = fRec33[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec29[1] = fRec29[0];
			fRec27[1] = fRec27[0];
			fRec28[1] = fRec28[0];
			fRec25[1] = fRec25[0];
			fRec26[1] = fRec26[0];
			fRec22[1] = fRec22[0];
			fRec24[1] = fRec24[0];
			fRec23[1] = fRec23[0];
			fRec18[1] = fRec18[0];
			fRec21[1] = fRec21[0];
			fRec20[1] = fRec20[0];
			fRec19[1] = fRec19[0];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			fRec15[1] = fRec15[0];
			fRec12[1] = fRec12[0];
			fRec14[1] = fRec14[0];
			fRec13[1] = fRec13[0];
			fRec8[1] = fRec8[0];
			fRec11[1] = fRec11[0];
			fRec10[1] = fRec10[0];
			fRec9[1] = fRec9[0];
			fRec7[1] = fRec7[0];
			fRec6[1] = fRec6[0];
			fRec2[1] = fRec2[0];
			fRec5[1] = fRec5[0];
			fRec4[1] = fRec4[0];
			fRec3[1] = fRec3[0];
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
