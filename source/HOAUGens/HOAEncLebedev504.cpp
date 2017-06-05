//----------------------------------------------------------
// name: "HOAEncLebedev504"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2014"
//
// Code generated with Faust 0.9.100 (http://faust.grame.fr)
//----------------------------------------------------------

/* link with  */
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
	FAUSTFLOAT 	fslider0;
	double 	fRec0[2];
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOAEncLebedev504");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2014");
		m->declare("lib/lebedev.lib/name", "Lebdev grids and weights");
		m->declare("lib/lebedev.lib/version", "1.0");
		m->declare("lib/lebedev.lib/author", "Pierre Lecomte");
		m->declare("lib/lebedev.lib/license", "GPL");
		m->declare("lib/lebedev.lib/copyright", "(c) Pierre Lecomte 2014");
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
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
	}

	virtual int getNumInputs() { return 50; }
	virtual int getNumOutputs() { return 25; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec0[i] = 0;
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
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("Outputs");
		ui_interface->declare(&fslider0, "1", "");
		ui_interface->declare(&fslider0, "style", "knob");
		ui_interface->declare(&fslider0, "unit", "dB");
		ui_interface->addHorizontalSlider("Gain", &fslider0, 0.0, -1e+01, 5e+01, 0.1);
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
		FAUSTFLOAT* input32 = input[32];
		FAUSTFLOAT* input33 = input[33];
		FAUSTFLOAT* input34 = input[34];
		FAUSTFLOAT* input35 = input[35];
		FAUSTFLOAT* input36 = input[36];
		FAUSTFLOAT* input37 = input[37];
		FAUSTFLOAT* input38 = input[38];
		FAUSTFLOAT* input39 = input[39];
		FAUSTFLOAT* input40 = input[40];
		FAUSTFLOAT* input41 = input[41];
		FAUSTFLOAT* input42 = input[42];
		FAUSTFLOAT* input43 = input[43];
		FAUSTFLOAT* input44 = input[44];
		FAUSTFLOAT* input45 = input[45];
		FAUSTFLOAT* input46 = input[46];
		FAUSTFLOAT* input47 = input[47];
		FAUSTFLOAT* input48 = input[48];
		FAUSTFLOAT* input49 = input[49];
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
		for (int i=0; i<count; i++) {
			fRec0[0] = (fSlow0 + (0.999 * fRec0[1]));
			double fTemp0 = (double)input5[i];
			double fTemp1 = (double)input4[i];
			double fTemp2 = (double)input3[i];
			double fTemp3 = (double)input2[i];
			double fTemp4 = (double)input0[i];
			double fTemp5 = (double)input1[i];
			double fTemp6 = (double)input17[i];
			double fTemp7 = (double)input16[i];
			double fTemp8 = (double)input15[i];
			double fTemp9 = (double)input14[i];
			double fTemp10 = (double)input13[i];
			double fTemp11 = (double)input12[i];
			double fTemp12 = (double)input11[i];
			double fTemp13 = (double)input10[i];
			double fTemp14 = (double)input9[i];
			double fTemp15 = (double)input8[i];
			double fTemp16 = (double)input6[i];
			double fTemp17 = (double)input7[i];
			double fTemp18 = (fTemp14 + (fTemp15 + (fTemp16 + fTemp17)));
			double fTemp19 = (double)input25[i];
			double fTemp20 = (double)input24[i];
			double fTemp21 = (double)input23[i];
			double fTemp22 = (double)input22[i];
			double fTemp23 = (double)input21[i];
			double fTemp24 = (double)input20[i];
			double fTemp25 = (double)input18[i];
			double fTemp26 = (double)input19[i];
			double fTemp27 = (fTemp25 + fTemp26);
			double fTemp28 = (fTemp23 + (fTemp24 + fTemp27));
			double fTemp29 = (fTemp19 + (fTemp20 + (fTemp21 + (fTemp22 + fTemp28))));
			double fTemp30 = (double)input49[i];
			double fTemp31 = (double)input30[i];
			double fTemp32 = (double)input29[i];
			double fTemp33 = (double)input28[i];
			double fTemp34 = (double)input26[i];
			double fTemp35 = (double)input27[i];
			double fTemp36 = (fTemp34 + fTemp35);
			double fTemp37 = (fTemp32 + (fTemp33 + fTemp36));
			double fTemp38 = (double)input31[i];
			double fTemp39 = (double)input32[i];
			double fTemp40 = (double)input33[i];
			double fTemp41 = (double)input34[i];
			double fTemp42 = (double)input35[i];
			double fTemp43 = (double)input36[i];
			double fTemp44 = (double)input37[i];
			double fTemp45 = (double)input38[i];
			double fTemp46 = (double)input39[i];
			double fTemp47 = (double)input40[i];
			double fTemp48 = (double)input41[i];
			double fTemp49 = (double)input42[i];
			double fTemp50 = (double)input43[i];
			double fTemp51 = (double)input44[i];
			double fTemp52 = (double)input45[i];
			double fTemp53 = (double)input46[i];
			double fTemp54 = (double)input47[i];
			double fTemp55 = (double)input48[i];
			output0[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.012698412698412698 * (fTemp0 + (fTemp1 + (fTemp2 + (fTemp3 + (fTemp4 + fTemp5)))))) + (0.022574955908289243 * (fTemp6 + (fTemp7 + (fTemp8 + (fTemp9 + (fTemp10 + (fTemp11 + (fTemp12 + (fTemp13 + fTemp18)))))))))) + (0.02109375 * fTemp29)) + (0.02017333553791887 * (fTemp30 + (((((((((((((((((((fTemp31 + fTemp37) + fTemp38) + fTemp39) + fTemp40) + fTemp41) + fTemp42) + fTemp43) + fTemp44) + fTemp45) + fTemp46) + fTemp47) + fTemp48) + fTemp49) + fTemp50) + fTemp51) + fTemp52) + fTemp53) + fTemp54) + fTemp55)))));
			double fTemp56 = (fTemp3 - fTemp1);
			double fTemp57 = (fTemp13 - fTemp11);
			double fTemp58 = (fTemp15 + fTemp7);
			double fTemp59 = (fTemp26 + fTemp21);
			double fTemp60 = (fTemp31 + fTemp45);
			double fTemp61 = (fTemp40 + fTemp48);
			double fTemp62 = (fTemp32 + fTemp30);
			double fTemp63 = (fTemp33 - fTemp36);
			double fTemp64 = (fTemp44 + fTemp52);
			double fTemp65 = (fTemp17 - fTemp14);
			double fTemp66 = (fTemp25 - fTemp24);
			double fTemp67 = (fTemp20 - (fTemp22 + fTemp66));
			double fTemp68 = (fTemp23 + fTemp19);
			double fTemp69 = (fTemp41 + fTemp49);
			double fTemp70 = (fTemp38 + fTemp39);
			output1[i] = (FAUSTFLOAT)(fRec0[0] * ((((((((2.693524415809287e-18 * fTemp2) + (0.0219942959691286 * fTemp56)) + (0.0276485614705685 * fTemp57)) + (3.3859722305960555e-18 * fTemp58)) + (0.021093749999999998 * fTemp59)) + (0.010535180889746094 * fTemp60)) + (0.010535180889746096 * fTemp61)) - ((0.010535180889746067 * fTemp62) + ((0.010535180889746065 * (fTemp55 - (fTemp54 + (fTemp53 - fTemp63)))) + ((0.010535180889746113 * fTemp64) + (((((0.027648561470568506 * (fTemp6 - (fTemp8 - (fTemp10 - (fTemp12 + fTemp65))))) + (0.021093749999999994 * fTemp67)) + (0.02109375 * fTemp68)) + (0.01053518088974609 * fTemp69)) + (0.03160554266923824 * (fTemp51 + (fTemp50 - (fTemp47 + (fTemp46 - (fTemp43 + (fTemp42 - fTemp70)))))))))))));
			double fTemp71 = (fTemp4 - fTemp0);
			double fTemp72 = (fTemp6 + (fTemp7 + (fTemp8 + (fTemp9 - fTemp18))));
			double fTemp73 = (fTemp19 + (fTemp20 + (fTemp21 + (fTemp22 - fTemp28))));
			double fTemp74 = (fTemp44 + (fTemp43 + (fTemp42 + (fTemp41 + (fTemp40 + (fTemp39 + (fTemp31 + fTemp38)))))));
			double fTemp75 = (fTemp52 + (fTemp51 + (fTemp50 + (fTemp49 + (fTemp48 + (fTemp47 + (fTemp46 + (fTemp45 - fTemp74))))))));
			double fTemp76 = (fTemp30 + (fTemp55 + (fTemp54 + (fTemp53 - fTemp37))));
			output2[i] = (FAUSTFLOAT)(fRec0[0] * ((0.0219942959691286 * fTemp71) - ((((0.0276485614705685 * fTemp72) + (0.02109375 * fTemp73)) + (0.010535180889746075 * fTemp75)) + (0.03160554266923825 * fTemp76))));
			double fTemp77 = (fTemp43 + fTemp51);
			double fTemp78 = (fTemp38 + fTemp46);
			double fTemp79 = (fTemp26 - fTemp23);
			double fTemp80 = (fTemp19 - (fTemp21 + fTemp79));
			double fTemp81 = (fTemp25 + fTemp22);
			double fTemp82 = (fTemp17 + fTemp8);
			double fTemp83 = (fTemp5 - fTemp2);
			double fTemp84 = (fTemp34 - fTemp35);
			double fTemp85 = (fTemp32 + fTemp84);
			double fTemp86 = (fTemp30 - (fTemp54 - (fTemp53 + fTemp85)));
			double fTemp87 = (fTemp42 + fTemp50);
			double fTemp88 = (fTemp31 - fTemp40);
			double fTemp89 = (fTemp49 + (fTemp48 - (fTemp45 - (fTemp41 - fTemp88))));
			double fTemp90 = (fTemp12 - fTemp10);
			double fTemp91 = (fTemp16 - fTemp15);
			double fTemp92 = (fTemp14 + fTemp6);
			double fTemp93 = (fTemp24 + fTemp20);
			double fTemp94 = (fTemp39 + fTemp47);
			double fTemp95 = (fTemp33 + fTemp55);
			output3[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.010535180889746074 * fTemp77) + ((0.01053518088974608 * fTemp78) + ((0.021093749999999994 * fTemp80) + ((0.021093749999999998 * fTemp81) + ((1.6929861152980278e-18 * fTemp82) + ((1.3467622079046435e-18 * (fTemp0 + (fTemp4 + fTemp3))) + ((0.0219942959691286 * fTemp83) + (0.03160554266923823 * fTemp64)))))))) + (0.010535180889746065 * fTemp86)) - (((0.010535180889746086 * fTemp87) + ((0.03160554266923824 * fTemp89) + ((((((4.040286623713931e-18 * fTemp1) + (0.0276485614705685 * fTemp90)) + (0.027648561470568506 * (fTemp7 - (fTemp9 - (fTemp11 - (fTemp13 + fTemp91)))))) + (5.078958345894083e-18 * fTemp92)) + (0.02109375 * fTemp93)) + (0.010535180889746079 * fTemp94)))) + (0.010535180889746067 * fTemp95))));
			double fTemp96 = (fTemp48 - (fTemp45 + fTemp88));
			double fTemp97 = (fTemp25 - fTemp26);
			double fTemp98 = (fTemp23 - (fTemp24 + fTemp97));
			double fTemp99 = (fTemp19 - (fTemp20 - (fTemp21 - (fTemp22 - fTemp98))));
			double fTemp100 = (fTemp10 - (fTemp11 + (fTemp13 - fTemp12)));
			double fTemp101 = (fTemp32 - (fTemp33 + fTemp84));
			double fTemp102 = (fTemp30 - (fTemp55 - (fTemp54 - (fTemp53 - fTemp101))));
			output4[i] = (FAUSTFLOAT)(fRec0[0] * (((0.021308452520676563 * fTemp87) + (((8.030538257073304e-18 * fTemp92) + ((2.6768460856911014e-18 * fTemp82) + (((9.034355539207464e-18 * fTemp1) + (3.0114518464024883e-18 * fTemp3)) + (0.02130845252067657 * fTemp69)))) + (0.021308452520676556 * fTemp78))) - (((0.021308452520676584 * fTemp96) + ((0.027231914153020897 * fTemp99) + ((0.043716214137085485 * fTemp100) + ((0.021308452520676542 * fTemp77) + ((0.021308452520676553 * fTemp94) + (((6.0229036928049765e-18 * fTemp2) + (5.353692171382203e-18 * fTemp58)) + (0.021308452520676615 * fTemp64))))))) + (0.007102817506892165 * fTemp102))));
			double fTemp103 = (fTemp39 - fTemp43);
			double fTemp104 = (fTemp51 - (fTemp47 - fTemp103));
			double fTemp105 = (fTemp15 - fTemp7);
			double fTemp106 = (fTemp6 - (fTemp8 - fTemp65));
			double fTemp107 = (fTemp20 - (fTemp22 - fTemp66));
			double fTemp108 = (fTemp19 - (fTemp21 - fTemp79));
			double fTemp109 = (fTemp31 - fTemp45);
			double fTemp110 = (fTemp40 - fTemp48);
			double fTemp111 = (fTemp38 - fTemp42);
			double fTemp112 = (fTemp50 - (fTemp46 - fTemp111));
			double fTemp113 = (fTemp41 - fTemp49);
			double fTemp114 = (fTemp44 - fTemp52);
			double fTemp115 = (fTemp32 - fTemp30);
			output5[i] = (FAUSTFLOAT)(fRec0[0] * (((0.021308452520676528 * (fTemp55 - (fTemp54 + (fTemp53 + fTemp63)))) + ((0.021308452520676553 * fTemp104) + (((((((5.353692171382201e-18 * fTemp105) + (0.043716214137085485 * fTemp106)) + (0.027231914153020904 * fTemp107)) + (0.02723191415302091 * fTemp108)) + (0.007102817506892191 * fTemp109)) + (0.007102817506892193 * fTemp110)) + (0.021308452520676546 * fTemp112)))) - (((0.007102817506892187 * fTemp113) + (0.007102817506892205 * fTemp114)) + (0.02130845252067653 * fTemp115))));
			double fTemp116 = (fTemp6 + (fTemp7 + (fTemp8 + (fTemp9 + fTemp18))));
			double fTemp117 = (fTemp4 + fTemp0);
			double fTemp118 = (fTemp30 + (fTemp55 + (fTemp54 + (fTemp53 + fTemp37))));
			double fTemp119 = (fTemp1 + (fTemp2 + (fTemp5 + fTemp3)));
			double fTemp120 = (fTemp10 + (fTemp11 + (fTemp13 + fTemp12)));
			double fTemp121 = (fTemp52 + (fTemp51 + (fTemp50 + (fTemp49 + (fTemp48 + (fTemp47 + (fTemp46 + (fTemp45 + fTemp74))))))));
			output6[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.012619783999998805 * fTemp116) + ((0.02839451399999733 * fTemp117) + (5.236595479505967e-18 * fTemp29))) + (0.03280650879687195 * fTemp118)) - (((0.014197256999998666 * fTemp119) + (0.02523956799999763 * fTemp120)) + (0.01640325439843596 * fTemp121))));
			double fTemp122 = (fTemp48 - (fTemp45 - fTemp88));
			double fTemp123 = (fTemp38 - fTemp46);
			double fTemp124 = (fTemp7 - (fTemp9 - fTemp91));
			double fTemp125 = (fTemp17 - fTemp8);
			double fTemp126 = (fTemp43 - fTemp51);
			double fTemp127 = (fTemp42 - fTemp50);
			double fTemp128 = (fTemp39 - fTemp47);
			double fTemp129 = (fTemp14 - fTemp6);
			double fTemp130 = (fTemp33 - fTemp55);
			double fTemp131 = (fTemp30 - (fTemp54 - (fTemp53 - fTemp85)));
			output7[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.021308452520676542 * fTemp122) + ((0.007102817506892183 * fTemp123) + ((0.043716214137085485 * fTemp124) + ((2.6768460856911007e-18 * fTemp125) + (((3.0114518464024883e-18 * fTemp71) + (0.02723191415302091 * fTemp107)) + (0.02130845252067654 * fTemp114)))))) + (0.0071028175068921775 * fTemp126)) - ((((0.007102817506892185 * fTemp127) + ((0.007102817506892181 * fTemp128) + ((8.0305382570733e-18 * fTemp129) + ((0.027231914153020904 * fTemp108) + (0.021308452520676546 * fTemp113))))) + (0.02130845252067653 * fTemp130)) + (0.021308452520676528 * fTemp131))));
			double fTemp132 = (fTemp35 + fTemp54);
			double fTemp133 = (fTemp34 + fTemp53);
			double fTemp134 = (fTemp14 - (fTemp15 + (fTemp16 - fTemp17)));
			double fTemp135 = (fTemp6 - (fTemp7 - (fTemp8 - (fTemp9 - fTemp134))));
			double fTemp136 = (fTemp1 - (fTemp2 + (fTemp5 - fTemp3)));
			output8[i] = (FAUSTFLOAT)(fRec0[0] * (((9.219912161207984e-35 * fTemp117) + ((2.1746106811858183e-18 * fTemp95) + ((8.337369125538138e-18 * fTemp93) + ((1.6674738251076273e-18 * fTemp81) + ((0.028411270027568696 * fTemp64) + ((0.02841127002756873 * fTemp69) + ((((2.6768460856911007e-18 * fTemp13) + (1.3384230428455503e-17 * fTemp11)) + (0.028411270027568724 * fTemp60)) + (0.028411270027568717 * fTemp61)))))))) - ((1.3047664087114909e-18 * fTemp132) + ((1.1422201709353029e-18 * fTemp133) + ((0.02185810706854275 * fTemp135) + ((0.024590370452110585 * fTemp136) + ((0.028411270027568748 * fTemp77) + ((0.028411270027568734 * fTemp87) + ((0.028411270027568745 * fTemp94) + ((0.02841127002756874 * fTemp78) + (((1.167231677575339e-17 * fTemp68) + (((8.0305382570733e-18 * fTemp12) + (1.8737922599837705e-17 * fTemp10)) + (5.002421475322882e-18 * fTemp59))) + (3.044454953660145e-18 * fTemp62))))))))))));
			double fTemp137 = (fTemp6 - (fTemp8 + fTemp65));
			double fTemp138 = (6.133424905712859e-18 * fTemp58);
			double fTemp139 = (fTemp54 + (fTemp53 + fTemp36));
			double fTemp140 = (fTemp31 - fTemp41);
			output9[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.01669440503167441 * fTemp137) + (((0.020818537549440772 * fTemp87) + ((0.016982082124440745 * fTemp81) + (((fTemp138 + ((0.033388810063348796 * fTemp12) + ((9.75821927113805e-18 * fTemp2) + (0.0333888100633488 * fTemp13)))) + (0.01698208212444074 * fTemp59)) + (0.03007122090474784 * fTemp61)))) + (0.020818537549440803 * fTemp77))) + (0.002313170838826743 * fTemp139)) - ((0.03007122090474782 * (fTemp49 - (fTemp45 + fTemp140))) + ((0.002313170838826747 * fTemp95) + ((0.02081853754944079 * fTemp94) + ((0.020818537549440783 * fTemp78) + ((0.016982082124440766 * fTemp93) + ((0.0023131708388267444 * fTemp62) + ((((0.026560635762986527 * fTemp56) + ((0.03338881006334885 * fTemp11) + (0.03338881006334882 * fTemp10))) + (0.01698208212444075 * fTemp68)) + (0.030071220904747842 * fTemp64))))))))));
			output10[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.041597435974919175 * (fTemp19 - (fTemp20 - (fTemp21 - (fTemp22 + fTemp98))))) + (((0.016998264729033888 * fTemp122) + ((0.016998264729033867 * fTemp123) + ((1.5023761394674526e-17 * fTemp129) + ((5.007920464891509e-18 * fTemp125) + (0.01699826472903388 * fTemp113))))) + (0.016998264729033874 * fTemp127))) + (0.016998264729033836 * (fTemp30 - (fTemp55 - (fTemp54 - (fTemp53 + fTemp101)))))) - ((0.016998264729033857 * fTemp126) + ((0.016998264729033864 * fTemp128) + ((1.0015840929783018e-17 * fTemp105) + (0.016998264729033916 * fTemp114))))));
			output11[i] = (FAUSTFLOAT)(fRec0[0] * (((0.016125969844275594 * (fTemp51 - (fTemp47 + fTemp103))) + ((0.01612596984427559 * (fTemp50 - (fTemp46 + fTemp111))) + (((((4.750930503007937e-18 * fTemp58) + (0.013154264250377147 * fTemp59)) + (0.005375323281425202 * fTemp69)) + (0.005375323281425214 * fTemp64)) + (0.030460165261409405 * fTemp139)))) - (((((((((((2.5195613817171963e-18 * fTemp2) + (0.020573779994945588 * fTemp56)) + (0.025862861065006505 * fTemp57)) + (0.013154264250377145 * fTemp67)) + (0.013154264250377148 * fTemp68)) + (0.005375323281425205 * fTemp60)) + (0.005375323281425206 * fTemp61)) + (0.030460165261409412 * fTemp62)) + (0.02586286106500651 * fTemp90)) + (0.030460165261409402 * fTemp95)) + (0.03879429159750975 * fTemp137))));
			output12[i] = (FAUSTFLOAT)(fRec0[0] * (((((0.03359684204526464 * fTemp71) + (0.010558468816293319 * fTemp72)) + (0.021480823570105497 * fTemp73)) + (0.020481687709769415 * fTemp75)) - (0.0263335984839893 * fTemp76)));
			double fTemp141 = (fTemp7 - (fTemp9 + fTemp91));
			output13[i] = (FAUSTFLOAT)((((((0.0053753232814252 * fTemp87) + ((0.01612596984427559 * fTemp89) + ((0.005375323281425197 * fTemp94) + ((0.013154264250377145 * fTemp80) + ((((3.779342072575794e-18 * fTemp1) + (0.025862861065006505 * fTemp90)) + (2.3754652515039685e-18 * fTemp82)) + (0.013154264250377147 * fTemp81)))))) + (0.030460165261409402 * fTemp86)) + (5.0391227634343926e-18 * fTemp117)) - ((0.030460165261409412 * fTemp95) + ((0.005375323281425194 * fTemp77) + ((0.005375323281425198 * fTemp78) + (((((0.016125969844275587 * fTemp64) + ((0.02586286106500651 * fTemp57) + ((1.2597806908585981e-18 * fTemp3) + (0.03879429159750975 * fTemp141)))) + (0.020573779994945588 * fTemp83)) + (7.126395754511905e-18 * fTemp92)) + (0.013154264250377148 * fTemp93)))))) * fRec0[0]);
			double fTemp142 = (fTemp25 - fTemp22);
			double fTemp143 = (fTemp24 - fTemp20);
			double fTemp144 = (fTemp35 - fTemp54);
			double fTemp145 = (fTemp34 - fTemp53);
			double fTemp146 = (fTemp26 - fTemp21);
			double fTemp147 = (fTemp23 - fTemp19);
			output14[i] = (FAUSTFLOAT)(fRec0[0] * (((0.022664352972045162 * (fTemp47 + (fTemp46 - fTemp70))) + (((0.02266435297204512 * fTemp114) + ((0.02266435297204515 * fTemp113) + ((0.022664352972045138 * fTemp110) + (((2.4393594688416384e-34 * fTemp71) + ((2.5471083409710867e-18 * fTemp142) + (1.273554170485543e-17 * fTemp143))) + (0.022664352972045144 * fTemp109))))) + (5.204217622867659e-18 * fTemp130))) - ((3.1225305737205953e-18 * fTemp144) + ((2.7335294515958808e-18 * fTemp145) + ((0.022664352972045165 * fTemp126) + ((0.022664352972045155 * fTemp127) + ((((0.04089277388695433 * ((((fTemp9 + fTemp134) - fTemp8) + fTemp7) - fTemp6)) + (7.641325022913258e-18 * fTemp146)) + (1.7829758386797603e-17 * fTemp147)) + (7.285904672014723e-18 * fTemp115))))))));
			double fTemp148 = (3.0667124528564294e-18 * fTemp82);
			double fTemp149 = (fTemp25 + fTemp23);
			output15[i] = (FAUSTFLOAT)(fRec0[0] * ((((6.097910338853647e-51 * fTemp117) + (((0.0300712209047478 * fTemp87) + ((0.03007122090474779 * fTemp94) + ((0.01698208212444072 * fTemp93) + ((9.20013735856929e-18 * fTemp92) + ((0.026560635762986527 * fTemp83) + ((0.02081853754944071 * fTemp64) + ((((0.03338881006334876 * fTemp11) + ((1.4637328906707076e-17 * fTemp1) + (0.03338881006334882 * fTemp12))) + (0.01698208212444075 * fTemp59)) + (0.02081853754944074 * fTemp60)))))))) + (0.002313170838826741 * fTemp95))) + (0.0023131708388267444 * fTemp132)) - (((((((((((4.879109635569025e-18 * fTemp3) + (0.01669440503167441 * fTemp141)) + (0.020818537549440724 * fTemp61)) + (0.020818537549440744 * fTemp69)) + (0.0023131708388267436 * fTemp62)) + fTemp148) + (0.030071220904747797 * fTemp78)) + (0.03007122090474778 * fTemp77)) + (0.002313170838826745 * fTemp133)) + (0.0333888100633488 * (fTemp13 + fTemp10))) + (0.01698208212444074 * (fTemp19 + (fTemp22 + fTemp149))))));
			output16[i] = (FAUSTFLOAT)(fRec0[0] * ((((((1.2735541704855428e-17 * fTemp93) + ((2.5471083409710855e-18 * fTemp81) + (((1.7829758386797597e-17 * fTemp68) + ((7.641325022913257e-18 * fTemp59) + (((4.293397433999001e-17 * fTemp10) + ((3.0667124528564293e-17 * fTemp11) + ((6.133424905712858e-18 * fTemp13) + (1.8400274717138574e-17 * fTemp12)))) + (0.035508216952135595 * (fTemp47 - (fTemp46 + (fTemp38 - fTemp39))))))) + (1.2683132068209864e-18 * fTemp62)))) + (0.03550821695213559 * fTemp77)) + (9.059380048721333e-19 * fTemp95)) + (5.435628029232799e-19 * fTemp132)) - ((((9.200137358569287e-18 * fTemp92) + (((0.03550821695213564 * fTemp64) + ((((2.0700309056780893e-17 * fTemp1) + ((1.3800206037853928e-17 * fTemp2) + (6.900103018926964e-18 * fTemp3))) + (0.03550821695213561 * (fTemp50 - (fTemp49 + (fTemp41 - fTemp42))))) + fTemp138)) + fTemp148)) + (0.035508216952135616 * fTemp96)) + (4.758463994196736e-19 * fTemp133))));
			output17[i] = (FAUSTFLOAT)(fRec0[0] * (((((0.018831075746023662 * fTemp126) + ((((1.3010959028184069e-17 * fTemp105) + (((0.02941382905783859 * fTemp142) + (0.02941382905783858 * fTemp146)) + (0.027200442744256398 * (fTemp49 - (fTemp45 - fTemp140))))) + (0.027200442744256408 * fTemp110)) + (0.01883107574602363 * fTemp127))) + (0.006277025248674522 * fTemp145)) + (0.006277025248674523 * fTemp144)) - ((0.0062770252486745335 * fTemp130) + ((0.01883107574602365 * fTemp128) + ((0.01883107574602364 * fTemp123) + ((0.006277025248674527 * fTemp115) + (((0.035414181017315374 * fTemp106) + ((0.02941382905783863 * fTemp143) + (0.029413829057838594 * fTemp147))) + (0.027200442744256415 * fTemp114))))))));
			output18[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.037859351999996446 * fTemp100) + (((0.006710422253905626 * fTemp94) + ((1.738662534172072e-17 * fTemp92) + ((5.795541780573573e-18 * fTemp82) + ((5.215987602516216e-18 * fTemp2) + (0.006710422253905646 * fTemp64))))) + (0.006710422253905624 * fTemp77))) + (0.006710422253905637 * fTemp96)) - (((0.031444705933590805 * fTemp99) + (((0.006710422253905627 * fTemp78) + ((((7.823981403774323e-18 * fTemp1) + (2.607993801258108e-18 * fTemp3)) + (1.1591083561147146e-17 * fTemp58)) + (0.0067104222539056315 * fTemp69))) + (0.00671042225390563 * fTemp87))) + (0.02907849643359095 * fTemp102))));
			output19[i] = (FAUSTFLOAT)(fRec0[0] * (((0.010280801007450555 * fTemp114) + ((0.010280801007450529 * fTemp113) + ((0.013385302265262959 * fTemp106) + ((1.6392267574774099e-18 * fTemp105) + ((0.011117382399029452 * fTemp147) + (0.03558738810271335 * (fTemp30 - (fTemp54 + (fTemp53 + (fTemp32 - fTemp36)))))))))) - ((0.03558738810271334 * fTemp130) + ((0.03084240302235157 * fTemp104) + ((0.030842403022351562 * fTemp112) + ((0.010280801007450538 * fTemp110) + ((0.01111738239902945 * (fTemp20 - (fTemp21 + (fTemp22 + (fTemp24 - fTemp27))))) + (0.010280801007450534 * fTemp109))))))));
			output20[i] = (FAUSTFLOAT)(fRec0[0] * ((((((0.014285714285714285 * fTemp119) + (0.0380952380952381 * fTemp117)) + (0.025396825396825397 * fTemp120)) + (0.004251405423280436 * fTemp121)) + (0.014254712301587407 * fTemp118)) - ((0.027513227513227514 * fTemp116) + (0.024609375000000013 * fTemp29))));
			output21[i] = (FAUSTFLOAT)(fRec0[0] * ((((0.010280801007450517 * fTemp128) + ((0.013385302265262959 * fTemp124) + ((8.196133787387049e-19 * fTemp125) + (((7.376520408648356e-18 * fTemp71) + ((0.011117382399029452 * fTemp143) + (0.01111738239902945 * (fTemp19 - (fTemp21 - (fTemp22 - (fTemp23 + fTemp97))))))) + (0.030842403022351562 * fTemp113))))) + (0.010280801007450525 * fTemp127)) - ((((0.010280801007450515 * fTemp126) + ((0.03084240302235156 * fTemp122) + ((0.010280801007450522 * fTemp123) + ((2.4588401362161147e-18 * fTemp129) + (0.030842403022351552 * fTemp114))))) + (0.03558738810271335 * fTemp130)) + (0.03558738810271334 * fTemp131))));
			output22[i] = (FAUSTFLOAT)(fRec0[0] * (((0.021295885499998 * fTemp136) + ((4.79080689136032e-34 * fTemp117) + (((0.008947229671874174 * fTemp77) + ((0.008947229671874172 * fTemp94) + ((9.62716461792544e-18 * fTemp93) + ((1.925432923585088e-18 * fTemp81) + (((6.954650136688288e-18 * fTemp12) + (1.6227516985606004e-17 * fTemp10)) + (0.00894722967187417 * (fTemp50 + (fTemp46 + (fTemp38 + fTemp42))))))))) + (8.902721895353721e-18 * fTemp95)))) - ((5.341633137212233e-18 * fTemp132) + ((4.676178873339891e-18 * fTemp133) + ((0.04732418999999555 * fTemp135) + (((0.008947229671874156 * fTemp64) + ((0.008947229671874167 * fTemp69) + ((1.3478030465095617e-17 * fTemp68) + ((5.776298770755264e-18 * fTemp59) + (((2.3182167122294292e-18 * fTemp13) + (1.1591083561147147e-17 * fTemp11)) + (0.008947229671874163 * (fTemp48 + (fTemp45 + (fTemp31 + fTemp40))))))))) + (1.246381065349521e-17 * fTemp62)))))));
			output23[i] = (FAUSTFLOAT)(fRec0[0] * (((((0.027200442744256377 * fTemp127) + ((0.027200442744256363 * fTemp128) + ((1.9516438542276106e-17 * fTemp129) + ((0.035414181017315374 * fTemp124) + ((0.018831075746023575 * fTemp114) + (((1.829373101656094e-50 * fTemp71) + (((0.029413829057838594 * fTemp146) + (0.02941382905783855 * fTemp143)) + (0.029413829057838584 * (fTemp19 + (fTemp22 - fTemp149))))) + (0.018831075746023607 * fTemp109))))))) + (0.006277025248674517 * fTemp130)) + (0.006277025248674527 * fTemp144)) - (((0.027200442744256356 * fTemp126) + ((0.027200442744256374 * fTemp123) + ((6.5054795140920346e-18 * fTemp125) + (((0.01883107574602361 * fTemp113) + (0.018831075746023586 * fTemp110)) + (0.006277025248674525 * fTemp115))))) + (0.0062770252486745275 * fTemp145))));
			output24[i] = (FAUSTFLOAT)(fRec0[0] * (((0.012520803773755805 * fTemp116) + ((3.9603917912335094e-67 * fTemp117) + ((0.028171808490950554 * fTemp119) + (((0.0103565632777062 * fTemp87) + ((0.010356563277706236 * fTemp94) + ((0.010356563277706217 * fTemp78) + (((0.010356563277706178 * fTemp69) + ((0.010356563277706152 * fTemp60) + (0.010356563277706144 * fTemp61))) + (0.010356563277706062 * fTemp64))))) + (0.010356563277706251 * fTemp77))))) - (((0.05008321509502321 * fTemp120) + (0.02079871798745958 * fTemp29)) + (0.001479509039672308 * fTemp118))));
			// post processing
			fRec0[1] = fRec0[0];
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
