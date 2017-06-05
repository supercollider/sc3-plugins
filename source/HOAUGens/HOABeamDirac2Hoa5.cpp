//----------------------------------------------------------
// name: "HOABeamDirac2HOA5"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2016"
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
	FAUSTFLOAT 	fcheckbox0;
	double 	fConst0;
	double 	fConst1;
	FAUSTFLOAT 	fentry0;
	double 	fRec0[2];
	double 	fRec1[2];
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fslider1;
	FAUSTFLOAT 	fslider2;
	double 	fRec2[2];
	double 	fRec3[2];
	double 	fRec4[2];
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("lib/nfc.lib/name", "NF And NFC Filters Library");
		m->declare("lib/nfc.lib/version", "1.0");
		m->declare("lib/nfc.lib/author", "Pierre Lecomte");
		m->declare("lib/nfc.lib/license", "GPL");
		m->declare("lib/nfc.lib/copyright", "(c) Pierre Lecomte 2014");
		m->declare("name", "HOABeamDirac2HOA5");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
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
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
	}

	virtual int getNumInputs() { return 36; }
	virtual int getNumOutputs() { return 36; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(1.92e+05, max(1e+03, (double)fSamplingFreq));
		fConst1 = (1.0 / fConst0);
	}
	virtual void instanceResetUserInterface() {
		fcheckbox0 = 0.0;
		fentry0 = 1.0;
		fslider0 = 0.0;
		fslider1 = 0.0;
		fslider2 = 0.0;
	}
	virtual void instanceClear() {
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
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
		ui_interface->openHorizontalBox("Parameters");
		ui_interface->declare(0, "2", "");
		ui_interface->openVerticalBox("On/Off");
		ui_interface->addCheckButton("On", &fcheckbox0);
		ui_interface->closeBox();
		ui_interface->declare(&fentry0, "3", "");
		ui_interface->declare(&fentry0, "unit", "s");
		ui_interface->addNumEntry("Crossfade", &fentry0, 1.0, 0.1, 1e+01, 0.1);
		ui_interface->declare(&fslider2, "4", "");
		ui_interface->declare(&fslider2, "unit", "dB");
		ui_interface->addVerticalSlider("Gain", &fslider2, 0.0, -2e+01, 2e+01, 0.1);
		ui_interface->declare(&fslider1, "5", "");
		ui_interface->addVerticalSlider("Azimuth", &fslider1, 0.0, -3.141592653589793, 3.141592653589793, 0.1);
		ui_interface->declare(&fslider0, "6", "");
		ui_interface->addVerticalSlider("Elevation", &fslider0, 0.0, -1.5707963267948966, 1.5707963267948966, 0.1);
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = double(fcheckbox0);
		int 	iSlow1 = int(fSlow0);
		double 	fSlow2 = double(fentry0);
		double 	fSlow3 = (fConst1 / fSlow2);
		int 	iSlow4 = int(((1 - fSlow0) > 0));
		double 	fSlow5 = (fConst0 * fSlow2);
		int 	iSlow6 = int((fSlow0 > 0));
		double 	fSlow7 = double(fslider0);
		double 	fSlow8 = cos(fSlow7);
		double 	fSlow9 = double(fslider1);
		double 	fSlow10 = (3 * fSlow9);
		double 	fSlow11 = sin(fSlow10);
		double 	fSlow12 = (2.091650066335189 * (fSlow8 * fSlow11));
		double 	fSlow13 = (0.0010000000000000009 * pow(10,(0.05 * double(fslider2))));
		double 	fSlow14 = sin(fSlow7);
		double 	fSlow15 = (2 * fSlow9);
		double 	fSlow16 = sin(fSlow15);
		double 	fSlow17 = (5.123475382979799 * (fSlow14 * fSlow16));
		double 	fSlow18 = sin(fSlow9);
		double 	fSlow19 = faustpower<2>(fSlow14);
		double 	fSlow20 = (5 * fSlow19);
		double 	fSlow21 = (fSlow20 + -1);
		double 	fSlow22 = (1.620185174601965 * (fSlow18 * fSlow21));
		double 	fSlow23 = cos(fSlow9);
		double 	fSlow24 = (1.620185174601965 * (fSlow23 * fSlow21));
		double 	fSlow25 = faustpower<2>(fSlow8);
		double 	fSlow26 = cos(fSlow10);
		double 	fSlow27 = (fSlow25 * fSlow26);
		double 	fSlow28 = (2.091650066335189 * fSlow27);
		double 	fSlow29 = faustpower<3>(fSlow8);
		double 	fSlow30 = (4 * fSlow9);
		double 	fSlow31 = sin(fSlow30);
		double 	fSlow32 = (2.218529918662356 * (fSlow29 * fSlow31));
		double 	fSlow33 = (7 * fSlow19);
		double 	fSlow34 = (fSlow33 + -1);
		double 	fSlow35 = (1.6770509831248424 * ((fSlow8 * fSlow16) * fSlow34));
		double 	fSlow36 = cos(fSlow15);
		double 	fSlow37 = (1.6770509831248424 * ((fSlow8 * fSlow36) * fSlow34));
		double 	fSlow38 = cos(fSlow30);
		double 	fSlow39 = (2.218529918662356 * (fSlow29 * fSlow38));
		double 	fSlow40 = faustpower<4>(fSlow8);
		double 	fSlow41 = (5 * fSlow9);
		double 	fSlow42 = sin(fSlow41);
		double 	fSlow43 = (2.3268138086232857 * (fSlow40 * fSlow42));
		double 	fSlow44 = ((9 * fSlow19) + -1);
		double 	fSlow45 = (1.734304615688951 * ((fSlow25 * fSlow11) * fSlow44));
		double 	fSlow46 = ((fSlow19 * ((21 * fSlow19) + -14)) + 1);
		double 	fSlow47 = (1.6056540723331412 * (fSlow18 * fSlow46));
		double 	fSlow48 = (1.6056540723331412 * (fSlow23 * fSlow46));
		double 	fSlow49 = (1.734304615688951 * (fSlow27 * fSlow44));
		double 	fSlow50 = cos(fSlow41);
		double 	fSlow51 = (2.3268138086232857 * (fSlow40 * fSlow50));
		double 	fSlow52 = sin((2 * fSlow7));
		double 	fSlow53 = (fSlow33 + -3);
		double 	fSlow54 = (1.1858541225631423 * (fSlow52 * fSlow53));
		double 	fSlow55 = ((fSlow19 * ((35 * fSlow19) + -30)) + 3);
		double 	fSlow56 = (0.375 * fSlow55);
		double 	fSlow57 = (fSlow8 * fSlow18);
		double 	fSlow58 = (fSlow8 * fSlow23);
		double 	fSlow59 = (fSlow25 * fSlow16);
		double 	fSlow60 = (fSlow52 * fSlow18);
		double 	fSlow61 = (fSlow52 * fSlow23);
		double 	fSlow62 = (fSlow25 * fSlow36);
		double 	fSlow63 = ((3 * fSlow19) + -1);
		double 	fSlow64 = (fSlow25 * fSlow14);
		double 	fSlow65 = (fSlow64 * fSlow16);
		double 	fSlow66 = (8.496322733983215 * fSlow65);
		double 	fSlow67 = (fSlow64 * fSlow36);
		double 	fSlow68 = (8.496322733983215 * fSlow67);
		double 	fSlow69 = (fSlow20 + -3);
		double 	fSlow70 = (1.3228756555322954 * fSlow69);
		double 	fSlow71 = (5.123475382979799 * fSlow62);
		double 	fSlow72 = (fSlow29 * fSlow11);
		double 	fSlow73 = (6.274950199005566 * fSlow72);
		double 	fSlow74 = (fSlow29 * fSlow26);
		double 	fSlow75 = (6.274950199005566 * fSlow74);
		double 	fSlow76 = (fSlow40 * fSlow31);
		double 	fSlow77 = (7.358031326380718 * fSlow76);
		double 	fSlow78 = ((fSlow19 * ((63 * fSlow19) + -70)) + 15);
		double 	fSlow79 = (0.414578098794425 * fSlow78);
		double 	fSlow80 = (fSlow40 * fSlow38);
		double 	fSlow81 = (7.358031326380718 * fSlow80);
		double 	fSlow82 = (0.13783222385544802 * fSlow57);
		double 	fSlow83 = (0.13783222385544802 * fSlow14);
		double 	fSlow84 = (0.13783222385544802 * fSlow58);
		double 	fSlow85 = (0.15410111101537496 * fSlow59);
		double 	fSlow86 = (0.15410111101537496 * fSlow60);
		double 	fSlow87 = (0.08897031792714714 * fSlow63);
		double 	fSlow88 = (0.15410111101537496 * fSlow61);
		double 	fSlow89 = (0.15410111101537496 * fSlow62);
		double 	fSlow90 = (0.16644822363786807 * fSlow72);
		double 	fSlow91 = (0.4077132165054383 * fSlow65);
		double 	fSlow92 = (0.12893023963105413 * (fSlow57 * fSlow21));
		double 	fSlow93 = (0.10527109983694811 * (fSlow14 * fSlow69));
		double 	fSlow94 = (0.12893023963105413 * (fSlow58 * fSlow21));
		double 	fSlow95 = (0.4077132165054383 * fSlow67);
		double 	fSlow96 = (0.16644822363786807 * fSlow74);
		double 	fSlow97 = (0.17654500147618726 * fSlow76);
		double 	fSlow98 = (fSlow29 * fSlow14);
		double 	fSlow99 = (0.4993446709136041 * (fSlow98 * fSlow11));
		double 	fSlow100 = (0.1334554768907207 * (fSlow59 * fSlow34));
		double 	fSlow101 = (0.0943672726959132 * (fSlow60 * fSlow53));
		double 	fSlow102 = (0.029841551829730376 * fSlow55);
		double 	fSlow103 = (0.0943672726959132 * (fSlow61 * fSlow53));
		double 	fSlow104 = (0.1334554768907207 * (fSlow62 * fSlow34));
		double 	fSlow105 = (0.4993446709136041 * (fSlow98 * fSlow26));
		double 	fSlow106 = (0.17654500147618726 * fSlow80);
		double 	fSlow107 = faustpower<5>(fSlow8);
		double 	fSlow108 = (0.18516195964843765 * (fSlow107 * fSlow42));
		double 	fSlow109 = (fSlow40 * fSlow14);
		double 	fSlow110 = (0.5855335285092532 * (fSlow109 * fSlow31));
		double 	fSlow111 = (0.13801157620699323 * (fSlow72 * fSlow44));
		double 	fSlow112 = (0.6761158806087376 * (fSlow65 * fSlow63));
		double 	fSlow113 = (0.12777389125372554 * (fSlow57 * fSlow46));
		double 	fSlow114 = (0.03299107686038644 * (fSlow14 * fSlow78));
		double 	fSlow115 = (0.12777389125372554 * (fSlow58 * fSlow46));
		double 	fSlow116 = (0.6761158806087376 * (fSlow67 * fSlow63));
		double 	fSlow117 = (0.13801157620699323 * (fSlow74 * fSlow44));
		double 	fSlow118 = (0.5855335285092532 * (fSlow109 * fSlow38));
		double 	fSlow119 = (0.18516195964843765 * (fSlow107 * fSlow50));
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
			fRec0[0] = ((iSlow4)?fSlow5:max((double)0, (fRec0[1] + -1)));
			fRec1[0] = ((iSlow6)?0:min(fSlow5, (fRec1[1] + 1)));
			double fTemp1 = (double)input9[i];
			fRec2[0] = (fSlow13 + (0.999 * fRec2[1]));
			double fTemp2 = (fTemp1 * fRec2[0]);
			fRec3[0] = ((iSlow4)?0:min(fSlow5, (fRec3[1] + 1)));
			fRec4[0] = ((iSlow6)?fSlow5:max((double)0, (fRec4[1] + -1)));
			double fTemp3 = (double)input10[i];
			double fTemp4 = (fTemp3 * fRec2[0]);
			double fTemp5 = (double)input11[i];
			double fTemp6 = (fTemp5 * fRec2[0]);
			double fTemp7 = (double)input13[i];
			double fTemp8 = (fTemp7 * fRec2[0]);
			double fTemp9 = (double)input15[i];
			double fTemp10 = (fTemp9 * fRec2[0]);
			double fTemp11 = (double)input16[i];
			double fTemp12 = (fTemp11 * fRec2[0]);
			double fTemp13 = (double)input18[i];
			double fTemp14 = (fTemp13 * fRec2[0]);
			double fTemp15 = (double)input22[i];
			double fTemp16 = (fTemp15 * fRec2[0]);
			double fTemp17 = (double)input24[i];
			double fTemp18 = (fTemp17 * fRec2[0]);
			double fTemp19 = (double)input25[i];
			double fTemp20 = (fTemp19 * fRec2[0]);
			double fTemp21 = (double)input27[i];
			double fTemp22 = (fTemp21 * fRec2[0]);
			double fTemp23 = (double)input29[i];
			double fTemp24 = (fTemp23 * fRec2[0]);
			double fTemp25 = (double)input31[i];
			double fTemp26 = (fTemp25 * fRec2[0]);
			double fTemp27 = (double)input33[i];
			double fTemp28 = (fTemp27 * fRec2[0]);
			double fTemp29 = (double)input35[i];
			double fTemp30 = (fTemp29 * fRec2[0]);
			double fTemp31 = (double)input19[i];
			double fTemp32 = (fTemp31 * fRec2[0]);
			double fTemp33 = (double)input21[i];
			double fTemp34 = (fTemp33 * fRec2[0]);
			double fTemp35 = (double)input20[i];
			double fTemp36 = (fTemp35 * fRec2[0]);
			double fTemp37 = (fTemp0 * fRec2[0]);
			double fTemp38 = (double)input1[i];
			double fTemp39 = (fTemp38 * fRec2[0]);
			double fTemp40 = (double)input2[i];
			double fTemp41 = (fTemp40 * fRec2[0]);
			double fTemp42 = (double)input3[i];
			double fTemp43 = (fTemp42 * fRec2[0]);
			double fTemp44 = (double)input4[i];
			double fTemp45 = (fTemp44 * fRec2[0]);
			double fTemp46 = (double)input5[i];
			double fTemp47 = (fTemp46 * fRec2[0]);
			double fTemp48 = (double)input7[i];
			double fTemp49 = (fTemp48 * fRec2[0]);
			double fTemp50 = (double)input8[i];
			double fTemp51 = (fTemp50 * fRec2[0]);
			double fTemp52 = (double)input6[i];
			double fTemp53 = (fTemp52 * fRec2[0]);
			double fTemp54 = (double)input28[i];
			double fTemp55 = (fTemp54 * fRec2[0]);
			double fTemp56 = (double)input32[i];
			double fTemp57 = (fTemp56 * fRec2[0]);
			double fTemp58 = (double)input12[i];
			double fTemp59 = (fTemp58 * fRec2[0]);
			double fTemp60 = (double)input14[i];
			double fTemp61 = (fTemp60 * fRec2[0]);
			double fTemp62 = (double)input17[i];
			double fTemp63 = (fTemp62 * fRec2[0]);
			double fTemp64 = (double)input23[i];
			double fTemp65 = (fTemp64 * fRec2[0]);
			double fTemp66 = (double)input26[i];
			double fTemp67 = (fTemp66 * fRec2[0]);
			double fTemp68 = (double)input30[i];
			double fTemp69 = (fTemp68 * fRec2[0]);
			double fTemp70 = (double)input34[i];
			double fTemp71 = (fTemp70 * fRec2[0]);
			double fTemp72 = ((fSlow8 * ((((((((((((((fSlow8 * ((fSlow12 * ((iSlow1)?(fSlow3 * (fTemp2 * fRec3[0])):(fSlow3 * (fTemp2 * fRec4[0])))) + (fSlow17 * ((iSlow1)?(fSlow3 * (fTemp4 * fRec3[0])):(fSlow3 * (fTemp4 * fRec4[0])))))) + (fSlow22 * ((iSlow1)?(fSlow3 * (fTemp6 * fRec3[0])):(fSlow3 * (fTemp6 * fRec4[0]))))) + (fSlow24 * ((iSlow1)?(fSlow3 * (fTemp8 * fRec3[0])):(fSlow3 * (fTemp8 * fRec4[0]))))) + (fSlow28 * ((iSlow1)?(fSlow3 * (fTemp10 * fRec3[0])):(fSlow3 * (fTemp10 * fRec4[0]))))) + (fSlow32 * ((iSlow1)?(fSlow3 * (fTemp12 * fRec3[0])):(fSlow3 * (fTemp12 * fRec4[0]))))) + (fSlow35 * ((iSlow1)?(fSlow3 * (fTemp14 * fRec3[0])):(fSlow3 * (fTemp14 * fRec4[0]))))) + (fSlow37 * ((iSlow1)?(fSlow3 * (fTemp16 * fRec3[0])):(fSlow3 * (fTemp16 * fRec4[0]))))) + (fSlow39 * ((iSlow1)?(fSlow3 * (fTemp18 * fRec3[0])):(fSlow3 * (fTemp18 * fRec4[0]))))) + (fSlow43 * ((iSlow1)?(fSlow3 * (fTemp20 * fRec3[0])):(fSlow3 * (fTemp20 * fRec4[0]))))) + (fSlow45 * ((iSlow1)?(fSlow3 * (fTemp22 * fRec3[0])):(fSlow3 * (fTemp22 * fRec4[0]))))) + (fSlow47 * ((iSlow1)?(fSlow3 * (fTemp24 * fRec3[0])):(fSlow3 * (fTemp24 * fRec4[0]))))) + (fSlow48 * ((iSlow1)?(fSlow3 * (fTemp26 * fRec3[0])):(fSlow3 * (fTemp26 * fRec4[0]))))) + (fSlow49 * ((iSlow1)?(fSlow3 * (fTemp28 * fRec3[0])):(fSlow3 * (fTemp28 * fRec4[0]))))) + (fSlow51 * ((iSlow1)?(fSlow3 * (fTemp30 * fRec3[0])):(fSlow3 * (fTemp30 * fRec4[0])))))) + ((((fSlow54 * ((fSlow18 * ((iSlow1)?(fSlow3 * (fTemp32 * fRec3[0])):(fSlow3 * (fTemp32 * fRec4[0])))) + (fSlow23 * ((iSlow1)?(fSlow3 * (fTemp34 * fRec3[0])):(fSlow3 * (fTemp34 * fRec4[0])))))) + ((fSlow56 * ((iSlow1)?(fSlow3 * (fTemp36 * fRec3[0])):(fSlow3 * (fTemp36 * fRec4[0])))) + ((((iSlow1)?(fSlow3 * (fTemp37 * fRec3[0])):(fSlow3 * (fTemp37 * fRec4[0]))) + (1.7320508075688772 * (((fSlow57 * ((iSlow1)?(fSlow3 * (fTemp39 * fRec3[0])):(fSlow3 * (fTemp39 * fRec4[0])))) + (fSlow14 * ((iSlow1)?(fSlow3 * (fTemp41 * fRec3[0])):(fSlow3 * (fTemp41 * fRec4[0]))))) + (fSlow58 * ((iSlow1)?(fSlow3 * (fTemp43 * fRec3[0])):(fSlow3 * (fTemp43 * fRec4[0]))))))) + (1.9364916731037085 * ((((fSlow59 * ((iSlow1)?(fSlow3 * (fTemp45 * fRec3[0])):(fSlow3 * (fTemp45 * fRec4[0])))) + (fSlow60 * ((iSlow1)?(fSlow3 * (fTemp47 * fRec3[0])):(fSlow3 * (fTemp47 * fRec4[0]))))) + (fSlow61 * ((iSlow1)?(fSlow3 * (fTemp49 * fRec3[0])):(fSlow3 * (fTemp49 * fRec4[0]))))) + (fSlow62 * ((iSlow1)?(fSlow3 * (fTemp51 * fRec3[0])):(fSlow3 * (fTemp51 * fRec4[0]))))))))) + (fSlow63 * (((1.118033988749895 * ((iSlow1)?(fSlow3 * (fTemp53 * fRec3[0])):(fSlow3 * (fTemp53 * fRec4[0])))) + (fSlow66 * ((iSlow1)?(fSlow3 * (fTemp55 * fRec3[0])):(fSlow3 * (fTemp55 * fRec4[0]))))) + (fSlow68 * ((iSlow1)?(fSlow3 * (fTemp57 * fRec3[0])):(fSlow3 * (fTemp57 * fRec4[0]))))))) + (fSlow14 * (((((((fSlow70 * ((iSlow1)?(fSlow3 * (fTemp59 * fRec3[0])):(fSlow3 * (fTemp59 * fRec4[0])))) + (fSlow71 * ((iSlow1)?(fSlow3 * (fTemp61 * fRec3[0])):(fSlow3 * (fTemp61 * fRec4[0]))))) + (fSlow73 * ((iSlow1)?(fSlow3 * (fTemp63 * fRec3[0])):(fSlow3 * (fTemp63 * fRec4[0]))))) + (fSlow75 * ((iSlow1)?(fSlow3 * (fTemp65 * fRec3[0])):(fSlow3 * (fTemp65 * fRec4[0]))))) + (fSlow77 * ((iSlow1)?(fSlow3 * (fTemp67 * fRec3[0])):(fSlow3 * (fTemp67 * fRec4[0]))))) + (fSlow79 * ((iSlow1)?(fSlow3 * (fTemp69 * fRec3[0])):(fSlow3 * (fTemp69 * fRec4[0]))))) + (fSlow81 * ((iSlow1)?(fSlow3 * (fTemp71 * fRec3[0])):(fSlow3 * (fTemp71 * fRec4[0]))))))));
			output0[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp0 * fRec0[0])):(fSlow3 * (fTemp0 * fRec1[0]))) + (0.07957747154594767 * fTemp72));
			output1[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp38 * fRec0[0])):(fSlow3 * (fTemp38 * fRec1[0]))) + (fSlow82 * fTemp72));
			output2[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp40 * fRec0[0])):(fSlow3 * (fTemp40 * fRec1[0]))) + (fSlow83 * fTemp72));
			output3[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp42 * fRec0[0])):(fSlow3 * (fTemp42 * fRec1[0]))) + (fSlow84 * fTemp72));
			output4[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp44 * fRec0[0])):(fSlow3 * (fTemp44 * fRec1[0]))) + (fSlow85 * fTemp72));
			output5[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp46 * fRec0[0])):(fSlow3 * (fTemp46 * fRec1[0]))) + (fSlow86 * fTemp72));
			output6[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp52 * fRec0[0])):(fSlow3 * (fTemp52 * fRec1[0]))) + (fSlow87 * fTemp72));
			output7[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp48 * fRec0[0])):(fSlow3 * (fTemp48 * fRec1[0]))) + (fSlow88 * fTemp72));
			output8[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp50 * fRec0[0])):(fSlow3 * (fTemp50 * fRec1[0]))) + (fSlow89 * fTemp72));
			output9[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp1 * fRec0[0])):(fSlow3 * (fTemp1 * fRec1[0]))) + (fSlow90 * fTemp72));
			output10[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp3 * fRec0[0])):(fSlow3 * (fTemp3 * fRec1[0]))) + (fSlow91 * fTemp72));
			output11[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp5 * fRec0[0])):(fSlow3 * (fTemp5 * fRec1[0]))) + (fSlow92 * fTemp72));
			output12[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp58 * fRec0[0])):(fSlow3 * (fTemp58 * fRec1[0]))) + (fSlow93 * fTemp72));
			output13[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp7 * fRec0[0])):(fSlow3 * (fTemp7 * fRec1[0]))) + (fSlow94 * fTemp72));
			output14[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp60 * fRec0[0])):(fSlow3 * (fTemp60 * fRec1[0]))) + (fSlow95 * fTemp72));
			output15[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp9 * fRec0[0])):(fSlow3 * (fTemp9 * fRec1[0]))) + (fSlow96 * fTemp72));
			output16[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp11 * fRec0[0])):(fSlow3 * (fTemp11 * fRec1[0]))) + (fSlow97 * fTemp72));
			output17[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp62 * fRec0[0])):(fSlow3 * (fTemp62 * fRec1[0]))) + (fSlow99 * fTemp72));
			output18[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp13 * fRec0[0])):(fSlow3 * (fTemp13 * fRec1[0]))) + (fSlow100 * fTemp72));
			output19[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp31 * fRec0[0])):(fSlow3 * (fTemp31 * fRec1[0]))) + (fSlow101 * fTemp72));
			output20[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp35 * fRec0[0])):(fSlow3 * (fTemp35 * fRec1[0]))) + (fSlow102 * fTemp72));
			output21[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp33 * fRec0[0])):(fSlow3 * (fTemp33 * fRec1[0]))) + (fSlow103 * fTemp72));
			output22[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp15 * fRec0[0])):(fSlow3 * (fTemp15 * fRec1[0]))) + (fSlow104 * fTemp72));
			output23[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp64 * fRec0[0])):(fSlow3 * (fTemp64 * fRec1[0]))) + (fSlow105 * fTemp72));
			output24[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp17 * fRec0[0])):(fSlow3 * (fTemp17 * fRec1[0]))) + (fSlow106 * fTemp72));
			output25[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp19 * fRec0[0])):(fSlow3 * (fTemp19 * fRec1[0]))) + (fSlow108 * fTemp72));
			output26[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp66 * fRec0[0])):(fSlow3 * (fTemp66 * fRec1[0]))) + (fSlow110 * fTemp72));
			output27[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp21 * fRec0[0])):(fSlow3 * (fTemp21 * fRec1[0]))) + (fSlow111 * fTemp72));
			output28[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp54 * fRec0[0])):(fSlow3 * (fTemp54 * fRec1[0]))) + (fSlow112 * fTemp72));
			output29[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp23 * fRec0[0])):(fSlow3 * (fTemp23 * fRec1[0]))) + (fSlow113 * fTemp72));
			output30[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp68 * fRec0[0])):(fSlow3 * (fTemp68 * fRec1[0]))) + (fSlow114 * fTemp72));
			output31[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp25 * fRec0[0])):(fSlow3 * (fTemp25 * fRec1[0]))) + (fSlow115 * fTemp72));
			output32[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp56 * fRec0[0])):(fSlow3 * (fTemp56 * fRec1[0]))) + (fSlow116 * fTemp72));
			output33[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp27 * fRec0[0])):(fSlow3 * (fTemp27 * fRec1[0]))) + (fSlow117 * fTemp72));
			output34[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp70 * fRec0[0])):(fSlow3 * (fTemp70 * fRec1[0]))) + (fSlow118 * fTemp72));
			output35[i] = (FAUSTFLOAT)(((iSlow1)?(fSlow3 * (fTemp29 * fRec0[0])):(fSlow3 * (fTemp29 * fRec1[0]))) + (fSlow119 * fTemp72));
			// post processing
			fRec4[1] = fRec4[0];
			fRec3[1] = fRec3[0];
			fRec2[1] = fRec2[0];
			fRec1[1] = fRec1[0];
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
