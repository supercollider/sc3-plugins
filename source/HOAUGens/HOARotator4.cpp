//----------------------------------------------------------
// name: "HOARotator4"
// version: "1.0"
// author: "Pierre Lecomte"
// license: "GPL"
// copyright: "(c) Pierre Lecomte 2016"
//
// Code generated with Faust 0.12.0 (http://faust.grame.fr)
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
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fslider1;
	FAUSTFLOAT 	fslider2;
	double 	fTempPerm0;
	double 	fTempPerm1;
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
	double 	fTempPerm15;
	double 	fTempPerm16;
	double 	fTempPerm17;
	double 	fTempPerm18;
	double 	fTempPerm19;
	double 	fTempPerm20;
	double 	fTempPerm21;
	double 	fTempPerm22;
	double 	fTempPerm23;
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOARotator4");
		m->declare("version", "1.0");
		m->declare("author", "Pierre Lecomte");
		m->declare("license", "GPL");
		m->declare("copyright", "(c) Pierre Lecomte 2016");
		m->declare("ymn.lib/name", "Spherical Harmonics library");
		m->declare("ymn.lib/version", "2.0");
		m->declare("ymn.lib/author", "Pierre Lecomte");
		m->declare("ymn.lib/license", "GPL");
		m->declare("ymn.lib/copyright", "(c) Pierre Lecomte 2017");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
	}

	virtual int getNumInputs() { return 25; }
	virtual int getNumOutputs() { return 25; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
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
	}
	virtual void instanceResetUserInterface() {
		fslider0 = 0.0;
		fslider1 = 0.0;
		fslider2 = 0.0;
	}
	virtual void instanceClear() {
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
		ui_interface->openVerticalBox("HOARotator4");
		ui_interface->declare(&fslider2, "osc", "/picth 0 360");
		ui_interface->addHorizontalSlider("Pitch", &fslider2, 0.0, -3.141592653589793, 3.141592653589793, 0.01);
		ui_interface->declare(&fslider0, "osc", "/roll 0 360");
		ui_interface->addHorizontalSlider("Roll", &fslider0, 0.0, -3.141592653589793, 3.141592653589793, 0.01);
		ui_interface->declare(&fslider1, "osc", "/yaw 0 360");
		ui_interface->addHorizontalSlider("Yaw", &fslider1, 0.0, -3.141592653589793, 3.141592653589793, 0.01);
		ui_interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		//zone1
		//zone2
		double 	fSlow0 = double(fslider0);
		double 	fSlow1 = cos(fSlow0);
		double 	fSlow2 = double(fslider1);
		double 	fSlow3 = cos(fSlow2);
		double 	fSlow4 = (fSlow1 * fSlow3);
		double 	fSlow5 = double(fslider2);
		double 	fSlow6 = sin(fSlow5);
		double 	fSlow7 = sin(fSlow0);
		double 	fSlow8 = (fSlow6 * fSlow7);
		double 	fSlow9 = sin(fSlow2);
		double 	fSlow10 = (fSlow4 - (fSlow8 * fSlow9));
		double 	fSlow11 = cos(fSlow5);
		double 	fSlow12 = (0 - fSlow11);
		double 	fSlow13 = (fSlow7 * fSlow12);
		double 	fSlow14 = (fSlow3 * fSlow6);
		double 	fSlow15 = (fSlow1 * fSlow9);
		double 	fSlow16 = ((fSlow14 * fSlow7) + fSlow15);
		double 	fSlow17 = (fSlow3 * fSlow7);
		double 	fSlow18 = (fSlow1 * fSlow6);
		double 	fSlow19 = (fSlow17 + (fSlow18 * fSlow9));
		double 	fSlow20 = (fSlow1 * fSlow11);
		double 	fSlow21 = (fSlow7 * fSlow9);
		double 	fSlow22 = (fSlow21 + (fSlow14 * (0 - fSlow1)));
		double 	fSlow23 = (fSlow9 * fSlow12);
		double 	fSlow24 = (fSlow3 * fSlow11);
		double 	fSlow25 = ((fSlow24 * fSlow10) + ((fSlow9 * fSlow16) * fSlow12));
		double 	fSlow26 = faustpower<2>(fSlow12);
		double 	fSlow27 = ((fSlow6 * fSlow10) + (fSlow21 * fSlow26));
		double 	fSlow28 = (1.7320508075688772 * (fSlow8 * fSlow12));
		double 	fSlow29 = ((fSlow6 * fSlow16) + ((fSlow17 * fSlow11) * fSlow12));
		double 	fSlow30 = ((fSlow24 * fSlow16) - ((fSlow9 * fSlow10) * fSlow12));
		double 	fSlow31 = ((fSlow19 * fSlow16) + (fSlow10 * fSlow22));
		double 	fSlow32 = ((fSlow20 * fSlow10) + (1.0000000000000002 * ((fSlow7 * fSlow19) * fSlow12)));
		double 	fSlow33 = (1.7320508075688772 * (((fSlow1 * fSlow7) * fSlow11) * fSlow12));
		double 	fSlow34 = ((fSlow20 * fSlow16) + (1.0000000000000002 * ((fSlow7 * fSlow22) * fSlow12)));
		double 	fSlow35 = ((fSlow16 * fSlow22) - (fSlow19 * fSlow10));
		double 	fSlow36 = (fSlow16 * fSlow10);
		double 	fSlow37 = (((fSlow3 * fSlow9) * fSlow11) * fSlow12);
		double 	fSlow38 = ((1.1547005383792515 * (fSlow19 * fSlow22)) - (0.5773502691896257 * (fSlow36 + fSlow37)));
		double 	fSlow39 = (fSlow6 * fSlow9);
		double 	fSlow40 = (fSlow7 * fSlow10);
		double 	fSlow41 = ((1.1547005383792515 * ((fSlow1 * fSlow19) * fSlow11)) - (0.5773502691896257 * ((fSlow39 + fSlow40) * fSlow12)));
		double 	fSlow42 = faustpower<2>(fSlow1);
		double 	fSlow43 = faustpower<2>(fSlow11);
		double 	fSlow44 = (fSlow42 * fSlow43);
		double 	fSlow45 = faustpower<2>(fSlow6);
		double 	fSlow46 = faustpower<2>(fSlow7);
		double 	fSlow47 = (fSlow46 * fSlow26);
		double 	fSlow48 = (fSlow45 + fSlow47);
		double 	fSlow49 = (fSlow44 - (0.5 * fSlow48));
		double 	fSlow50 = (fSlow14 * fSlow11);
		double 	fSlow51 = ((fSlow7 * fSlow16) * fSlow12);
		double 	fSlow52 = ((1.1547005383792515 * (fSlow20 * fSlow22)) - (0.5773502691896257 * (fSlow50 + fSlow51)));
		double 	fSlow53 = (faustpower<2>(fSlow3) * fSlow43);
		double 	fSlow54 = faustpower<2>(fSlow16);
		double 	fSlow55 = (faustpower<2>(fSlow9) * fSlow26);
		double 	fSlow56 = faustpower<2>(fSlow10);
		double 	fSlow57 = ((0.5773502691896257 * (faustpower<2>(fSlow22) - faustpower<2>(fSlow19))) - (0.28867513459481287 * ((fSlow53 + fSlow54) - (fSlow55 + fSlow56))));
		double 	fSlow58 = (((fSlow9 * fSlow22) * fSlow12) + ((fSlow3 * fSlow19) * fSlow11));
		double 	fSlow59 = (((fSlow15 * fSlow11) * fSlow12) + (1.0000000000000002 * (fSlow6 * fSlow19)));
		double 	fSlow60 = (fSlow18 * fSlow11);
		double 	fSlow61 = (1.7320508075688772 * fSlow60);
		double 	fSlow62 = ((fSlow4 * fSlow43) + (1.0000000000000002 * (fSlow6 * fSlow22)));
		double 	fSlow63 = ((fSlow24 * fSlow22) - ((fSlow9 * fSlow19) * fSlow12));
		double 	fSlow64 = (fSlow50 - fSlow51);
		double 	fSlow65 = (fSlow39 - fSlow40);
		double 	fSlow66 = (fSlow65 * fSlow12);
		double 	fSlow67 = (fSlow45 - fSlow47);
		double 	fSlow68 = (0.8660254037844386 * fSlow67);
		double 	fSlow69 = (0 - (2 * (fSlow36 - fSlow37)));
		double 	fSlow70 = ((fSlow53 + fSlow56) - (fSlow55 + fSlow54));
		double 	fSlow71 = (fSlow6 * fSlow25);
		double 	fSlow72 = (0.5 * (fSlow13 * fSlow69));
		double 	fSlow73 = (fSlow71 + fSlow72);
		double 	fSlow74 = (fSlow6 * fSlow30);
		double 	fSlow75 = (0.5 * (fSlow13 * fSlow70));
		double 	fSlow76 = (fSlow74 + fSlow75);
		double 	fSlow77 = (1.7320508075688772 * fSlow45);
		double 	fSlow78 = (fSlow68 + fSlow77);
		double 	fSlow79 = (0.9128709291752769 * ((fSlow78 * fSlow7) * fSlow12));
		double 	fSlow80 = (fSlow27 * fSlow6);
		double 	fSlow81 = ((fSlow65 * fSlow7) * fSlow26);
		double 	fSlow82 = (fSlow80 + fSlow81);
		double 	fSlow83 = (fSlow6 * fSlow29);
		double 	fSlow84 = (fSlow13 * fSlow64);
		double 	fSlow85 = (fSlow83 + fSlow84);
		double 	fSlow86 = ((fSlow24 * fSlow25) + (fSlow23 * fSlow30));
		double 	fSlow87 = (0.5 * ((fSlow16 * fSlow69) + (fSlow10 * fSlow70)));
		double 	fSlow88 = (fSlow86 + fSlow87);
		double 	fSlow89 = (fSlow24 * fSlow30);
		double 	fSlow90 = (0.5 * ((fSlow16 * fSlow70) - (fSlow10 * fSlow69)));
		double 	fSlow91 = (fSlow23 * fSlow25);
		double 	fSlow92 = ((fSlow89 + fSlow90) - fSlow91);
		double 	fSlow93 = (((fSlow27 * fSlow1) * fSlow11) + ((fSlow6 * fSlow32) + (fSlow13 * fSlow59)));
		double 	fSlow94 = ((fSlow20 * fSlow29) + ((fSlow6 * fSlow34) + (fSlow13 * fSlow62)));
		double 	fSlow95 = ((fSlow13 * fSlow58) + ((fSlow20 * fSlow25) + (fSlow6 * fSlow31)));
		double 	fSlow96 = (3.872983346207417 * (((fSlow18 * fSlow7) * fSlow11) * fSlow12));
		double 	fSlow97 = ((fSlow13 * fSlow63) + ((fSlow20 * fSlow30) + (fSlow6 * fSlow35)));
		double 	fSlow98 = (((fSlow22 * fSlow25) + (fSlow19 * fSlow30)) + ((fSlow10 * fSlow63) + ((fSlow23 * fSlow35) + ((fSlow24 * fSlow31) + (fSlow16 * fSlow58)))));
		double 	fSlow99 = (((fSlow22 * fSlow30) - (fSlow19 * fSlow25)) + (((fSlow24 * fSlow35) + (fSlow16 * fSlow63)) - ((fSlow23 * fSlow31) + (fSlow10 * fSlow58))));
		double 	fSlow100 = (((1.2649110640673518 * (fSlow20 * fSlow31)) + (1.0954451150103324 * ((fSlow38 * fSlow7) * fSlow12))) - (0.31622776601683794 * (fSlow71 - fSlow72)));
		double 	fSlow101 = (((0.5163977794943222 * ((fSlow22 * fSlow31) + (fSlow19 * fSlow35))) + (0.447213595499958 * ((fSlow38 * fSlow16) + (fSlow57 * fSlow10)))) - (0.12909944487358055 * (fSlow86 - fSlow87)));
		double 	fSlow102 = (((fSlow20 * fSlow32) + (0.8660254037844386 * ((fSlow41 * fSlow7) * fSlow12))) - (0.25 * (fSlow80 - fSlow81)));
		double 	fSlow103 = ((1.6329931618554518 * fSlow44) + (0.816496580927726 * fSlow49));
		double 	fSlow104 = (fSlow103 - (0.23570226039551584 * (fSlow77 - fSlow68)));
		double 	fSlow105 = ((fSlow104 * fSlow7) * fSlow12);
		double 	fSlow106 = (((fSlow20 * fSlow34) + (0.8660254037844386 * ((fSlow52 * fSlow7) * fSlow12))) - (0.25 * (fSlow83 - fSlow84)));
		double 	fSlow107 = (((1.2649110640673518 * (fSlow20 * fSlow35)) + (1.0954451150103324 * ((fSlow57 * fSlow7) * fSlow12))) - (0.31622776601683794 * (fSlow74 - fSlow75)));
		double 	fSlow108 = (((0.5163977794943222 * ((fSlow22 * fSlow35) - (fSlow19 * fSlow31))) + (0.447213595499958 * ((fSlow57 * fSlow16) - (fSlow38 * fSlow10)))) - (0.12909944487358055 * (fSlow89 - (fSlow91 + fSlow90))));
		double 	fSlow109 = ((fSlow24 * fSlow58) + (fSlow23 * fSlow63));
		double 	fSlow110 = (fSlow16 * fSlow31);
		double 	fSlow111 = (fSlow10 * fSlow35);
		double 	fSlow112 = ((0.5477225575051661 * ((fSlow38 * fSlow22) + (fSlow57 * fSlow19))) - (0.31622776601683794 * ((fSlow109 + fSlow110) + fSlow111)));
		double 	fSlow113 = (fSlow6 * fSlow58);
		double 	fSlow114 = (fSlow13 * fSlow31);
		double 	fSlow115 = ((1.3416407864998738 * ((fSlow38 * fSlow1) * fSlow11)) - (0.7745966692414834 * (fSlow113 + fSlow114)));
		double 	fSlow116 = (fSlow6 * fSlow59);
		double 	fSlow117 = (fSlow13 * fSlow32);
		double 	fSlow118 = ((1.0606601717798212 * ((fSlow41 * fSlow1) * fSlow11)) - (0.6123724356957945 * (fSlow116 + fSlow117)));
		double 	fSlow119 = (fSlow49 - (0.9999999999999999 * fSlow48));
		double 	fSlow120 = ((fSlow119 * fSlow1) * fSlow11);
		double 	fSlow121 = (fSlow6 * fSlow62);
		double 	fSlow122 = (fSlow13 * fSlow34);
		double 	fSlow123 = ((1.0606601717798212 * ((fSlow52 * fSlow1) * fSlow11)) - (0.6123724356957945 * (fSlow121 + fSlow122)));
		double 	fSlow124 = (fSlow6 * fSlow63);
		double 	fSlow125 = (fSlow13 * fSlow35);
		double 	fSlow126 = ((1.3416407864998738 * ((fSlow57 * fSlow1) * fSlow11)) - (0.7745966692414834 * (fSlow124 + fSlow125)));
		double 	fSlow127 = (fSlow24 * fSlow63);
		double 	fSlow128 = (fSlow16 * fSlow35);
		double 	fSlow129 = (fSlow23 * fSlow58);
		double 	fSlow130 = (fSlow10 * fSlow31);
		double 	fSlow131 = ((0.5477225575051661 * ((fSlow57 * fSlow22) - (fSlow38 * fSlow19))) - (0.31622776601683794 * ((fSlow127 + fSlow128) - (fSlow129 + fSlow130))));
		double 	fSlow132 = (0.5 * ((fSlow24 * fSlow69) + (fSlow23 * fSlow70)));
		double 	fSlow133 = (fSlow16 * fSlow25);
		double 	fSlow134 = (fSlow10 * fSlow30);
		double 	fSlow135 = (((0.447213595499958 * (((fSlow38 * fSlow3) * fSlow11) + ((fSlow57 * fSlow9) * fSlow12))) + (0.5163977794943222 * ((fSlow22 * fSlow58) + (fSlow19 * fSlow63)))) - (0.12909944487358055 * ((fSlow132 + fSlow133) + fSlow134)));
		double 	fSlow136 = (0.5 * (fSlow6 * fSlow69));
		double 	fSlow137 = (fSlow13 * fSlow25);
		double 	fSlow138 = (((1.2649110640673518 * (fSlow20 * fSlow58)) + (1.0954451150103324 * (fSlow38 * fSlow6))) - (0.31622776601683794 * (fSlow136 + fSlow137)));
		double 	fSlow139 = (fSlow65 * fSlow6);
		double 	fSlow140 = (fSlow27 * fSlow7);
		double 	fSlow141 = (((fSlow20 * fSlow59) + (0.8660254037844386 * (fSlow41 * fSlow6))) - (0.25 * ((fSlow139 + fSlow140) * fSlow12)));
		double 	fSlow142 = (1.7320508075688772 * fSlow47);
		double 	fSlow143 = (fSlow103 - (0.23570226039551584 * (fSlow68 + fSlow142)));
		double 	fSlow144 = (fSlow143 * fSlow6);
		double 	fSlow145 = (fSlow6 * fSlow64);
		double 	fSlow146 = (fSlow13 * fSlow29);
		double 	fSlow147 = (((fSlow20 * fSlow62) + (0.8660254037844386 * (fSlow52 * fSlow6))) - (0.25 * (fSlow145 + fSlow146)));
		double 	fSlow148 = (0.5 * (fSlow6 * fSlow70));
		double 	fSlow149 = (fSlow13 * fSlow30);
		double 	fSlow150 = (((1.2649110640673518 * (fSlow20 * fSlow63)) + (1.0954451150103324 * (fSlow57 * fSlow6))) - (0.31622776601683794 * (fSlow148 + fSlow149)));
		double 	fSlow151 = (fSlow16 * fSlow30);
		double 	fSlow152 = (0.5 * ((fSlow24 * fSlow70) - (fSlow23 * fSlow69)));
		double 	fSlow153 = (fSlow10 * fSlow25);
		double 	fSlow154 = (((0.5163977794943222 * ((fSlow22 * fSlow63) - (fSlow19 * fSlow58))) + (0.447213595499958 * (((fSlow57 * fSlow3) * fSlow11) - ((fSlow38 * fSlow9) * fSlow12)))) - (0.12909944487358055 * ((fSlow151 + fSlow152) - fSlow153)));
		double 	fSlow155 = (((0.5 * (fSlow20 * fSlow70)) + fSlow124) - fSlow125);
		double 	fSlow156 = ((0.2041241452319315 * ((fSlow22 * fSlow70) - (fSlow19 * fSlow69))) + (0.408248290463863 * ((fSlow127 + fSlow130) - (fSlow128 + fSlow129))));
		double 	fSlow157 = ((0.2041241452319315 * ((fSlow22 * fSlow69) + (fSlow19 * fSlow70))) + (0.408248290463863 * (fSlow109 - (fSlow110 + fSlow111))));
		double 	fSlow158 = (((0.5 * (fSlow20 * fSlow69)) + fSlow113) - fSlow114);
		double 	fSlow159 = (1.9364916731037085 * ((fSlow67 * fSlow1) * fSlow11));
		double 	fSlow160 = ((((fSlow65 * fSlow1) * fSlow11) * fSlow12) + (fSlow116 - fSlow117));
		double 	fSlow161 = ((fSlow20 * fSlow64) + (fSlow121 - fSlow122));
		double 	fSlow162 = (fSlow139 - fSlow140);
		double 	fSlow163 = (fSlow162 * fSlow12);
		double 	fSlow164 = (fSlow145 - fSlow146);
		double 	fSlow165 = (fSlow148 - fSlow149);
		double 	fSlow166 = (fSlow136 - fSlow137);
		double 	fSlow167 = ((fSlow152 + fSlow153) - fSlow151);
		double 	fSlow168 = (fSlow132 - (fSlow133 + fSlow134));
		double 	fSlow169 = (fSlow68 - fSlow142);
		double 	fSlow170 = (0.9128709291752769 * (fSlow169 * fSlow6));
		double 	fSlow171 = (fSlow6 * fSlow88);
		double 	fSlow172 = (fSlow13 * fSlow168);
		double 	fSlow173 = (fSlow171 + fSlow172);
		double 	fSlow174 = (fSlow6 * fSlow92);
		double 	fSlow175 = (fSlow13 * fSlow167);
		double 	fSlow176 = (fSlow174 + fSlow175);
		double 	fSlow177 = (0.8539125638299666 * ((((fSlow169 + fSlow78) * fSlow6) * fSlow7) * fSlow12));
		double 	fSlow178 = (fSlow82 * fSlow6);
		double 	fSlow179 = ((fSlow162 * fSlow7) * fSlow26);
		double 	fSlow180 = (fSlow178 + fSlow179);
		double 	fSlow181 = (fSlow6 * fSlow85);
		double 	fSlow182 = (fSlow13 * fSlow164);
		double 	fSlow183 = (fSlow181 + fSlow182);
		double 	fSlow184 = (fSlow73 * fSlow6);
		double 	fSlow185 = ((fSlow166 * fSlow7) * fSlow12);
		double 	fSlow186 = (fSlow184 + fSlow185);
		double 	fSlow187 = (fSlow6 * fSlow76);
		double 	fSlow188 = (fSlow13 * fSlow165);
		double 	fSlow189 = (fSlow187 + fSlow188);
		double 	fSlow190 = ((fSlow24 * fSlow88) + (fSlow23 * fSlow92));
		double 	fSlow191 = ((fSlow10 * fSlow167) + (fSlow16 * fSlow168));
		double 	fSlow192 = (fSlow190 + fSlow191);
		double 	fSlow193 = ((fSlow24 * fSlow92) - (fSlow23 * fSlow88));
		double 	fSlow194 = ((fSlow16 * fSlow167) - (fSlow10 * fSlow168));
		double 	fSlow195 = (fSlow193 + fSlow194);
		double 	fSlow196 = (fSlow6 * fSlow94);
		double 	fSlow197 = (fSlow13 * fSlow161);
		double 	fSlow198 = ((0.6614378277661476 * (fSlow20 * fSlow85)) + (0.6614378277661477 * (fSlow196 + fSlow197)));
		double 	fSlow199 = (3.872983346207417 * fSlow45);
		double 	fSlow200 = (1.9364916731037085 * fSlow67);
		double 	fSlow201 = ((((((0.8100925873009825 * (fSlow199 + fSlow200)) + (0.60380736442456 * fSlow78)) * fSlow1) * fSlow7) * fSlow11) * fSlow12);
		double 	fSlow202 = (0.408248290463863 * (fSlow6 * fSlow98));
		double 	fSlow203 = (fSlow13 * fSlow157);
		double 	fSlow204 = ((0.5 * (fSlow20 * fSlow88)) + (1.224744871391589 * (fSlow202 + fSlow203)));
		double 	fSlow205 = (0.408248290463863 * ((fSlow24 * fSlow98) + (fSlow23 * fSlow99)));
		double 	fSlow206 = (fSlow16 * fSlow157);
		double 	fSlow207 = (fSlow10 * fSlow156);
		double 	fSlow208 = ((0.1767766952966369 * ((fSlow22 * fSlow88) + (fSlow19 * fSlow92))) + (0.4330127018922193 * ((fSlow205 + fSlow206) + fSlow207)));
		double 	fSlow209 = (fSlow6 * fSlow93);
		double 	fSlow210 = (fSlow13 * fSlow160);
		double 	fSlow211 = ((0.6614378277661476 * ((fSlow82 * fSlow1) * fSlow11)) + (0.6614378277661477 * (fSlow209 + fSlow210)));
		double 	fSlow212 = (fSlow6 * fSlow95);
		double 	fSlow213 = (fSlow13 * fSlow158);
		double 	fSlow214 = (((fSlow73 * fSlow1) * fSlow11) + (fSlow212 + fSlow213));
		double 	fSlow215 = (fSlow6 * fSlow97);
		double 	fSlow216 = (fSlow13 * fSlow155);
		double 	fSlow217 = ((fSlow20 * fSlow76) + (fSlow215 + fSlow216));
		double 	fSlow218 = (0.408248290463863 * ((fSlow24 * fSlow99) - (fSlow23 * fSlow98)));
		double 	fSlow219 = (fSlow16 * fSlow156);
		double 	fSlow220 = (fSlow10 * fSlow157);
		double 	fSlow221 = ((0.1767766952966369 * ((fSlow22 * fSlow92) - (fSlow19 * fSlow88))) + (0.4330127018922193 * ((fSlow218 + fSlow219) - fSlow220)));
		double 	fSlow222 = (0.408248290463863 * (fSlow6 * fSlow99));
		double 	fSlow223 = (fSlow13 * fSlow156);
		double 	fSlow224 = ((0.5 * (fSlow20 * fSlow92)) + (1.224744871391589 * (fSlow222 + fSlow223)));
		double 	fSlow225 = (((fSlow20 * fSlow97) + (0.7905694150420949 * ((fSlow107 * fSlow6) + ((fSlow150 * fSlow7) * fSlow12)))) - (0.24999999999999997 * (fSlow187 - fSlow188)));
		double 	fSlow226 = (((0.5345224838248487 * (fSlow20 * fSlow99)) + (1.0350983390135313 * ((fSlow108 * fSlow6) + ((fSlow154 * fSlow7) * fSlow12)))) - (0.1336306209562122 * (fSlow174 - fSlow175)));
		double 	fSlow227 = (((0.1889822365046136 * ((fSlow22 * fSlow99) - (fSlow19 * fSlow98))) + (0.36596252735569995 * ((((fSlow108 * fSlow3) * fSlow11) + (fSlow154 * fSlow16)) - (((fSlow101 * fSlow9) * fSlow12) + (fSlow135 * fSlow10))))) - (0.0472455591261534 * (fSlow193 - fSlow194)));
		double 	fSlow228 = (((0.1889822365046136 * ((fSlow22 * fSlow98) + (fSlow19 * fSlow99))) + (0.36596252735569995 * ((fSlow154 * fSlow10) + ((((fSlow101 * fSlow3) * fSlow11) + ((fSlow108 * fSlow9) * fSlow12)) + (fSlow135 * fSlow16))))) - (0.0472455591261534 * (fSlow190 - fSlow191)));
		double 	fSlow229 = (((0.5345224838248487 * (fSlow20 * fSlow98)) + (1.0350983390135313 * ((fSlow101 * fSlow6) + ((fSlow135 * fSlow7) * fSlow12)))) - (0.1336306209562122 * (fSlow171 - fSlow172)));
		double 	fSlow230 = (((fSlow20 * fSlow95) + (0.7905694150420949 * ((fSlow100 * fSlow6) + ((fSlow138 * fSlow7) * fSlow12)))) - (0.24999999999999997 * (fSlow184 - fSlow185)));
		double 	fSlow231 = ((0.7071067811865476 * ((fSlow20 * fSlow93) + ((fSlow102 * fSlow6) + ((fSlow141 * fSlow7) * fSlow12)))) - (0.17677669529663687 * (fSlow178 - fSlow179)));
		double 	fSlow232 = ((((((0.6846531968814576 * (fSlow143 + fSlow104)) + (3.3541019662496843 * fSlow44)) * fSlow6) * fSlow7) * fSlow12) - (0.1767766952966369 * (0 - (0.9128709291752769 * ((((fSlow169 - fSlow78) * fSlow6) * fSlow7) * fSlow12)))));
		double 	fSlow233 = ((0.7071067811865476 * ((fSlow20 * fSlow94) + ((fSlow106 * fSlow6) + ((fSlow147 * fSlow7) * fSlow12)))) - (0.17677669529663687 * (fSlow181 - fSlow182)));
		double 	fSlow234 = (((0.5175491695067657 * ((fSlow101 * fSlow22) + (fSlow108 * fSlow19))) + (0.4225771273642583 * ((fSlow112 * fSlow16) + (fSlow131 * fSlow10)))) - (0.16366341767699427 * (fSlow205 - (fSlow206 + fSlow207))));
		double 	fSlow235 = (((1.4638501094227998 * ((fSlow101 * fSlow1) * fSlow11)) + (1.1952286093343938 * ((fSlow112 * fSlow7) * fSlow12))) - (0.4629100498862757 * (fSlow202 - fSlow203)));
		double 	fSlow236 = (((1.118033988749895 * ((fSlow100 * fSlow1) * fSlow11)) + (0.9128709291752769 * ((fSlow115 * fSlow7) * fSlow12))) - (0.3535533905932738 * (fSlow212 - fSlow213)));
		double 	fSlow237 = ((((fSlow102 * fSlow1) * fSlow11) + (0.816496580927726 * ((fSlow118 * fSlow7) * fSlow12))) - (0.25 * (fSlow209 - fSlow210)));
		double 	fSlow238 = (0.7905694150420949 * fSlow119);
		double 	fSlow239 = (((((((0.9682458365518543 * fSlow104) + fSlow238) - (0.30618621784789724 * (fSlow199 - fSlow200))) * fSlow1) * fSlow7) * fSlow11) * fSlow12);
		double 	fSlow240 = ((((fSlow106 * fSlow1) * fSlow11) + (0.816496580927726 * ((fSlow123 * fSlow7) * fSlow12))) - (0.25 * (fSlow196 - fSlow197)));
		double 	fSlow241 = (((1.118033988749895 * ((fSlow107 * fSlow1) * fSlow11)) + (0.9128709291752769 * ((fSlow126 * fSlow7) * fSlow12))) - (0.3535533905932738 * (fSlow215 - fSlow216)));
		double 	fSlow242 = (((1.4638501094227998 * ((fSlow108 * fSlow1) * fSlow11)) + (1.1952286093343938 * ((fSlow131 * fSlow7) * fSlow12))) - (0.4629100498862757 * (fSlow222 - fSlow223)));
		double 	fSlow243 = (((0.5175491695067657 * ((fSlow108 * fSlow22) - (fSlow101 * fSlow19))) + (0.4225771273642583 * ((fSlow131 * fSlow16) - (fSlow112 * fSlow10)))) - (0.16366341767699427 * ((fSlow218 + fSlow220) - fSlow219)));
		double 	fSlow244 = (fSlow108 * fSlow10);
		double 	fSlow245 = (((fSlow135 * fSlow3) * fSlow11) + ((fSlow154 * fSlow9) * fSlow12));
		double 	fSlow246 = (fSlow101 * fSlow16);
		double 	fSlow247 = ((0.5345224838248488 * ((fSlow112 * fSlow22) + (fSlow131 * fSlow19))) - (0.32732683535398854 * (fSlow244 + (fSlow245 + fSlow246))));
		double 	fSlow248 = (fSlow135 * fSlow6);
		double 	fSlow249 = ((fSlow101 * fSlow7) * fSlow12);
		double 	fSlow250 = ((1.5118578920369088 * ((fSlow112 * fSlow1) * fSlow11)) - (0.9258200997725514 * (fSlow248 + fSlow249)));
		double 	fSlow251 = (fSlow138 * fSlow6);
		double 	fSlow252 = ((fSlow100 * fSlow7) * fSlow12);
		double 	fSlow253 = ((1.1547005383792515 * ((fSlow115 * fSlow1) * fSlow11)) - (0.7071067811865476 * (fSlow251 + fSlow252)));
		double 	fSlow254 = (fSlow141 * fSlow6);
		double 	fSlow255 = ((fSlow102 * fSlow7) * fSlow12);
		double 	fSlow256 = ((1.0327955589886444 * ((fSlow118 * fSlow1) * fSlow11)) - (0.6324555320336759 * (fSlow254 + fSlow255)));
		double 	fSlow257 = (fSlow143 * fSlow45);
		double 	fSlow258 = ((fSlow104 * fSlow46) * fSlow26);
		double 	fSlow259 = (((fSlow119 * fSlow42) * fSlow43) - (0.6123724356957945 * (fSlow257 + fSlow258)));
		double 	fSlow260 = (fSlow147 * fSlow6);
		double 	fSlow261 = ((fSlow106 * fSlow7) * fSlow12);
		double 	fSlow262 = ((1.0327955589886444 * ((fSlow123 * fSlow1) * fSlow11)) - (0.6324555320336759 * (fSlow260 + fSlow261)));
		double 	fSlow263 = (fSlow150 * fSlow6);
		double 	fSlow264 = ((fSlow107 * fSlow7) * fSlow12);
		double 	fSlow265 = ((1.1547005383792515 * ((fSlow126 * fSlow1) * fSlow11)) - (0.7071067811865476 * (fSlow263 + fSlow264)));
		double 	fSlow266 = (fSlow154 * fSlow6);
		double 	fSlow267 = ((fSlow108 * fSlow7) * fSlow12);
		double 	fSlow268 = ((1.5118578920369088 * ((fSlow131 * fSlow1) * fSlow11)) - (0.9258200997725514 * (fSlow266 + fSlow267)));
		double 	fSlow269 = (fSlow108 * fSlow16);
		double 	fSlow270 = ((fSlow154 * fSlow3) * fSlow11);
		double 	fSlow271 = ((fSlow135 * fSlow9) * fSlow12);
		double 	fSlow272 = (fSlow101 * fSlow10);
		double 	fSlow273 = ((0.5345224838248488 * ((fSlow131 * fSlow22) - (fSlow112 * fSlow19))) - (0.32732683535398854 * ((fSlow269 + fSlow270) - (fSlow271 + fSlow272))));
		double 	fSlow274 = ((fSlow24 * fSlow157) + (fSlow23 * fSlow156));
		double 	fSlow275 = (0.408248290463863 * ((fSlow16 * fSlow98) + (fSlow10 * fSlow99)));
		double 	fSlow276 = (((0.5175491695067657 * ((fSlow135 * fSlow22) + (fSlow154 * fSlow19))) + (0.4225771273642583 * (((fSlow112 * fSlow3) * fSlow11) + ((fSlow131 * fSlow9) * fSlow12)))) - (0.16366341767699427 * (fSlow274 + fSlow275)));
		double 	fSlow277 = (fSlow6 * fSlow157);
		double 	fSlow278 = (0.408248290463863 * (fSlow13 * fSlow98));
		double 	fSlow279 = (((1.4638501094227998 * ((fSlow135 * fSlow1) * fSlow11)) + (1.1952286093343938 * (fSlow112 * fSlow6))) - (0.4629100498862757 * (fSlow277 + fSlow278)));
		double 	fSlow280 = (fSlow6 * fSlow158);
		double 	fSlow281 = (fSlow13 * fSlow95);
		double 	fSlow282 = (((1.118033988749895 * ((fSlow138 * fSlow1) * fSlow11)) + (0.9128709291752769 * (fSlow115 * fSlow6))) - (0.3535533905932738 * (fSlow280 + fSlow281)));
		double 	fSlow283 = (fSlow6 * fSlow160);
		double 	fSlow284 = (fSlow13 * fSlow93);
		double 	fSlow285 = ((((fSlow141 * fSlow1) * fSlow11) + (0.816496580927726 * (fSlow118 * fSlow6))) - (0.25 * (fSlow283 + fSlow284)));
		double 	fSlow286 = (3.872983346207417 * fSlow47);
		double 	fSlow287 = ((((((0.9682458365518543 * fSlow143) + fSlow238) - (0.30618621784789724 * (fSlow200 + fSlow286))) * fSlow1) * fSlow6) * fSlow11);
		double 	fSlow288 = (fSlow6 * fSlow161);
		double 	fSlow289 = (fSlow13 * fSlow94);
		double 	fSlow290 = ((((fSlow147 * fSlow1) * fSlow11) + (0.816496580927726 * (fSlow123 * fSlow6))) - (0.25 * (fSlow288 + fSlow289)));
		double 	fSlow291 = (fSlow6 * fSlow155);
		double 	fSlow292 = (fSlow13 * fSlow97);
		double 	fSlow293 = (((1.118033988749895 * ((fSlow150 * fSlow1) * fSlow11)) + (0.9128709291752769 * (fSlow126 * fSlow6))) - (0.3535533905932738 * (fSlow291 + fSlow292)));
		double 	fSlow294 = (fSlow6 * fSlow156);
		double 	fSlow295 = (0.408248290463863 * (fSlow13 * fSlow99));
		double 	fSlow296 = (((1.4638501094227998 * ((fSlow154 * fSlow1) * fSlow11)) + (1.1952286093343938 * (fSlow131 * fSlow6))) - (0.4629100498862757 * (fSlow294 + fSlow295)));
		double 	fSlow297 = (fSlow24 * fSlow156);
		double 	fSlow298 = (0.408248290463863 * ((fSlow16 * fSlow99) - (fSlow10 * fSlow98)));
		double 	fSlow299 = (fSlow23 * fSlow157);
		double 	fSlow300 = (((0.5175491695067657 * ((fSlow154 * fSlow22) - (fSlow135 * fSlow19))) + (0.4225771273642583 * (((fSlow131 * fSlow3) * fSlow11) - ((fSlow112 * fSlow9) * fSlow12)))) - (0.16366341767699427 * ((fSlow297 + fSlow298) - fSlow299)));
		double 	fSlow301 = ((fSlow24 * fSlow168) + (fSlow23 * fSlow167));
		double 	fSlow302 = ((fSlow16 * fSlow88) + (fSlow10 * fSlow92));
		double 	fSlow303 = (((0.36596252735569995 * (fSlow245 - (fSlow246 + fSlow244))) + (0.4629100498862757 * ((fSlow22 * fSlow157) + (fSlow19 * fSlow156)))) - (0.0472455591261534 * (fSlow301 + fSlow302)));
		double 	fSlow304 = (fSlow6 * fSlow168);
		double 	fSlow305 = (fSlow13 * fSlow88);
		double 	fSlow306 = (((1.3093073414159542 * (fSlow20 * fSlow157)) + (1.0350983390135313 * (fSlow248 - fSlow249))) - (0.1336306209562122 * (fSlow304 + fSlow305)));
		double 	fSlow307 = (fSlow166 * fSlow6);
		double 	fSlow308 = ((fSlow73 * fSlow7) * fSlow12);
		double 	fSlow309 = (((fSlow20 * fSlow158) + (0.7905694150420949 * (fSlow251 - fSlow252))) - (0.24999999999999997 * (fSlow307 + fSlow308)));
		double 	fSlow310 = (fSlow162 * fSlow6);
		double 	fSlow311 = (fSlow82 * fSlow7);
		double 	fSlow312 = ((0.7071067811865476 * ((fSlow20 * fSlow160) + (fSlow254 - fSlow255))) - (0.17677669529663687 * ((fSlow310 + fSlow311) * fSlow12)));
		double 	fSlow313 = (fSlow169 * fSlow45);
		double 	fSlow314 = ((fSlow78 * fSlow46) * fSlow26);
		double 	fSlow315 = (((1.6770509831248421 * ((fSlow67 * fSlow42) * fSlow43)) + (0.6846531968814576 * (fSlow257 - fSlow258))) - (0.16137430609197573 * (fSlow313 + fSlow314)));
		double 	fSlow316 = (fSlow6 * fSlow164);
		double 	fSlow317 = (fSlow13 * fSlow85);
		double 	fSlow318 = ((0.7071067811865476 * ((fSlow20 * fSlow161) + (fSlow260 - fSlow261))) - (0.17677669529663687 * (fSlow316 + fSlow317)));
		double 	fSlow319 = (fSlow6 * fSlow165);
		double 	fSlow320 = (fSlow13 * fSlow76);
		double 	fSlow321 = (((fSlow20 * fSlow155) + (0.7905694150420949 * (fSlow263 - fSlow264))) - (0.24999999999999997 * (fSlow319 + fSlow320)));
		double 	fSlow322 = (fSlow6 * fSlow167);
		double 	fSlow323 = (fSlow13 * fSlow92);
		double 	fSlow324 = (((1.3093073414159542 * (fSlow20 * fSlow156)) + (1.0350983390135313 * (fSlow266 - fSlow267))) - (0.1336306209562122 * (fSlow322 + fSlow323)));
		double 	fSlow325 = ((fSlow24 * fSlow167) - (fSlow23 * fSlow168));
		double 	fSlow326 = ((fSlow16 * fSlow92) - (fSlow10 * fSlow88));
		double 	fSlow327 = (((0.36596252735569995 * ((fSlow272 + fSlow270) - (fSlow271 + fSlow269))) + (0.4629100498862757 * ((fSlow22 * fSlow156) - (fSlow19 * fSlow157)))) - (0.0472455591261534 * (fSlow325 + fSlow326)));
		double 	fSlow328 = ((0.6614378277661476 * (fSlow20 * fSlow164)) + (0.6614378277661477 * (fSlow288 - fSlow289)));
		double 	fSlow329 = (fSlow60 * ((0.60380736442456 * fSlow169) + (0.8100925873009825 * (fSlow200 - fSlow286))));
		double 	fSlow330 = ((0.1767766952966369 * ((fSlow22 * fSlow168) + (fSlow19 * fSlow167))) + (0.4330127018922193 * (fSlow274 - fSlow275)));
		double 	fSlow331 = ((0.5 * (fSlow20 * fSlow168)) + (1.224744871391589 * (fSlow277 - fSlow278)));
		double 	fSlow332 = ((0.6614378277661476 * (((fSlow162 * fSlow1) * fSlow11) * fSlow12)) + (0.6614378277661477 * (fSlow283 - fSlow284)));
		double 	fSlow333 = (((fSlow166 * fSlow1) * fSlow11) + (fSlow280 - fSlow281));
		double 	fSlow334 = ((fSlow20 * fSlow165) + (fSlow291 - fSlow292));
		double 	fSlow335 = ((0.5 * (fSlow20 * fSlow167)) + (1.224744871391589 * (fSlow294 - fSlow295)));
		double 	fSlow336 = ((0.1767766952966369 * ((fSlow22 * fSlow167) - (fSlow19 * fSlow168))) + (0.4330127018922193 * (fSlow297 - (fSlow299 + fSlow298))));
		double 	fSlow337 = (fSlow304 - fSlow305);
		double 	fSlow338 = (fSlow322 - fSlow323);
		double 	fSlow339 = (fSlow307 - fSlow308);
		double 	fSlow340 = (fSlow319 - fSlow320);
		double 	fSlow341 = (0.8539125638299666 * (fSlow313 - fSlow314));
		double 	fSlow342 = ((fSlow310 - fSlow311) * fSlow12);
		double 	fSlow343 = (fSlow316 - fSlow317);
		double 	fSlow344 = (fSlow301 - fSlow302);
		double 	fSlow345 = (fSlow325 - fSlow326);
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
		//LoopGraphScalar
		for (int i=0; i<count; i++) {
			output0[i] = (FAUSTFLOAT)(double)input0[i];
			fTempPerm0 = (double)input1[i];
			fTempPerm1 = (double)input2[i];
			fTempPerm2 = (double)input3[i];
			output1[i] = (FAUSTFLOAT)(((fSlow10 * fTempPerm0) + (fSlow13 * fTempPerm1)) + (fSlow16 * fTempPerm2));
			output2[i] = (FAUSTFLOAT)(((fSlow19 * fTempPerm0) + (fSlow20 * fTempPerm1)) + (fSlow22 * fTempPerm2));
			output3[i] = (FAUSTFLOAT)(((fSlow23 * fTempPerm0) + (fSlow6 * fTempPerm1)) + (fSlow24 * fTempPerm2));
			fTempPerm3 = (double)input4[i];
			fTempPerm4 = (double)input5[i];
			fTempPerm5 = (double)input6[i];
			fTempPerm6 = (double)input7[i];
			fTempPerm7 = (double)input8[i];
			output4[i] = (FAUSTFLOAT)(((((fSlow25 * fTempPerm3) + (fSlow27 * fTempPerm4)) + (fSlow28 * fTempPerm5)) + (fSlow29 * fTempPerm6)) + (fSlow30 * fTempPerm7));
			output5[i] = (FAUSTFLOAT)(((((fSlow31 * fTempPerm3) + (fSlow32 * fTempPerm4)) + (fSlow33 * fTempPerm5)) + (fSlow34 * fTempPerm6)) + (fSlow35 * fTempPerm7));
			output6[i] = (FAUSTFLOAT)(((((fSlow38 * fTempPerm3) + (fSlow41 * fTempPerm4)) + (fSlow49 * fTempPerm5)) + (fSlow52 * fTempPerm6)) + (fSlow57 * fTempPerm7));
			output7[i] = (FAUSTFLOAT)(((((fSlow58 * fTempPerm3) + (fSlow59 * fTempPerm4)) + (fSlow61 * fTempPerm5)) + (fSlow62 * fTempPerm6)) + (fSlow63 * fTempPerm7));
			output8[i] = (FAUSTFLOAT)(((fSlow64 * fTempPerm6) + ((fSlow66 * fTempPerm4) + (fSlow68 * fTempPerm5))) + (0.5 * ((fSlow69 * fTempPerm3) + (fSlow70 * fTempPerm7))));
			fTempPerm8 = (double)input10[i];
			fTempPerm9 = (double)input14[i];
			fTempPerm10 = (double)input12[i];
			fTempPerm11 = (double)input11[i];
			fTempPerm12 = (double)input13[i];
			fTempPerm13 = (double)input9[i];
			fTempPerm14 = (double)input15[i];
			output9[i] = (FAUSTFLOAT)(((1.224744871391589 * ((fSlow73 * fTempPerm8) + (fSlow76 * fTempPerm9))) + ((fSlow79 * fTempPerm10) + (0.9682458365518543 * ((fSlow82 * fTempPerm11) + (fSlow85 * fTempPerm12))))) + (0.5 * ((fSlow88 * fTempPerm13) + (fSlow92 * fTempPerm14))));
			output10[i] = (FAUSTFLOAT)(((0.7905694150420949 * ((fSlow93 * fTempPerm11) + (fSlow94 * fTempPerm12))) + (((fSlow95 * fTempPerm8) + (fSlow96 * fTempPerm10)) + (fSlow97 * fTempPerm9))) + (0.408248290463863 * ((fSlow98 * fTempPerm13) + (fSlow99 * fTempPerm14))));
			output11[i] = (FAUSTFLOAT)(((((((fSlow100 * fTempPerm8) + (fSlow101 * fTempPerm13)) + (fSlow102 * fTempPerm11)) + (fSlow105 * fTempPerm10)) + (fSlow106 * fTempPerm12)) + (fSlow107 * fTempPerm9)) + (fSlow108 * fTempPerm14));
			output12[i] = (FAUSTFLOAT)(((((((fSlow112 * fTempPerm13) + (fSlow115 * fTempPerm8)) + (fSlow118 * fTempPerm11)) + (fSlow120 * fTempPerm10)) + (fSlow123 * fTempPerm12)) + (fSlow126 * fTempPerm9)) + (fSlow131 * fTempPerm14));
			output13[i] = (FAUSTFLOAT)(((((((fSlow135 * fTempPerm13) + (fSlow138 * fTempPerm8)) + (fSlow141 * fTempPerm11)) + (fSlow144 * fTempPerm10)) + (fSlow147 * fTempPerm12)) + (fSlow150 * fTempPerm9)) + (fSlow154 * fTempPerm14));
			output14[i] = (FAUSTFLOAT)((fSlow155 * fTempPerm9) + (((fSlow156 * fTempPerm14) + (((fSlow157 * fTempPerm13) + (fSlow158 * fTempPerm8)) + (fSlow159 * fTempPerm10))) + (0.7905694150420949 * ((fSlow160 * fTempPerm11) + (fSlow161 * fTempPerm12)))));
			output15[i] = (FAUSTFLOAT)((((0.9682458365518543 * ((fSlow163 * fTempPerm11) + (fSlow164 * fTempPerm12))) + (1.224744871391589 * ((fSlow165 * fTempPerm9) + (fSlow166 * fTempPerm8)))) + (0.5 * ((fSlow167 * fTempPerm14) + (fSlow168 * fTempPerm13)))) + (fSlow170 * fTempPerm10));
			fTempPerm15 = (double)input17[i];
			fTempPerm16 = (double)input23[i];
			fTempPerm17 = (double)input20[i];
			fTempPerm18 = (double)input19[i];
			fTempPerm19 = (double)input21[i];
			fTempPerm20 = (double)input18[i];
			fTempPerm21 = (double)input22[i];
			fTempPerm22 = (double)input16[i];
			fTempPerm23 = (double)input24[i];
			output16[i] = (FAUSTFLOAT)(((0.7071067811865476 * ((fSlow173 * fTempPerm15) + (fSlow176 * fTempPerm16))) + (((fSlow177 * fTempPerm17) + (0.9354143466934853 * ((fSlow180 * fTempPerm18) + (fSlow183 * fTempPerm19)))) + (1.3228756555322954 * ((fSlow186 * fTempPerm20) + (fSlow189 * fTempPerm21))))) + (0.25 * ((fSlow192 * fTempPerm22) + (fSlow195 * fTempPerm23))));
			output17[i] = (FAUSTFLOAT)(((((fSlow198 * fTempPerm19) + ((fSlow201 * fTempPerm17) + (((fSlow204 * fTempPerm15) + (fSlow208 * fTempPerm22)) + (fSlow211 * fTempPerm18)))) + (0.9354143466934853 * ((fSlow214 * fTempPerm20) + (fSlow217 * fTempPerm21)))) + (fSlow221 * fTempPerm23)) + (fSlow224 * fTempPerm16));
			output18[i] = (FAUSTFLOAT)(((((((((fSlow225 * fTempPerm21) + (fSlow226 * fTempPerm16)) + (fSlow227 * fTempPerm23)) + (fSlow228 * fTempPerm22)) + (fSlow229 * fTempPerm15)) + (fSlow230 * fTempPerm20)) + (fSlow231 * fTempPerm18)) + (fSlow232 * fTempPerm17)) + (fSlow233 * fTempPerm19));
			output19[i] = (FAUSTFLOAT)(((((((((fSlow234 * fTempPerm22) + (fSlow235 * fTempPerm15)) + (fSlow236 * fTempPerm20)) + (fSlow237 * fTempPerm18)) + (fSlow239 * fTempPerm17)) + (fSlow240 * fTempPerm19)) + (fSlow241 * fTempPerm21)) + (fSlow242 * fTempPerm16)) + (fSlow243 * fTempPerm23));
			output20[i] = (FAUSTFLOAT)(((((((((fSlow247 * fTempPerm22) + (fSlow250 * fTempPerm15)) + (fSlow253 * fTempPerm20)) + (fSlow256 * fTempPerm18)) + (fSlow259 * fTempPerm17)) + (fSlow262 * fTempPerm19)) + (fSlow265 * fTempPerm21)) + (fSlow268 * fTempPerm16)) + (fSlow273 * fTempPerm23));
			output21[i] = (FAUSTFLOAT)(((((((((fSlow276 * fTempPerm22) + (fSlow279 * fTempPerm15)) + (fSlow282 * fTempPerm20)) + (fSlow285 * fTempPerm18)) + (fSlow287 * fTempPerm17)) + (fSlow290 * fTempPerm19)) + (fSlow293 * fTempPerm21)) + (fSlow296 * fTempPerm16)) + (fSlow300 * fTempPerm23));
			output22[i] = (FAUSTFLOAT)(((((((((fSlow303 * fTempPerm22) + (fSlow306 * fTempPerm15)) + (fSlow309 * fTempPerm20)) + (fSlow312 * fTempPerm18)) + (fSlow315 * fTempPerm17)) + (fSlow318 * fTempPerm19)) + (fSlow321 * fTempPerm21)) + (fSlow324 * fTempPerm16)) + (fSlow327 * fTempPerm23));
			output23[i] = (FAUSTFLOAT)(((((fSlow328 * fTempPerm19) + ((fSlow329 * fTempPerm17) + (((fSlow330 * fTempPerm22) + (fSlow331 * fTempPerm15)) + (fSlow332 * fTempPerm18)))) + (0.9354143466934853 * ((fSlow333 * fTempPerm20) + (fSlow334 * fTempPerm21)))) + (fSlow335 * fTempPerm16)) + (fSlow336 * fTempPerm23));
			output24[i] = (FAUSTFLOAT)(((0.7071067811865476 * ((fSlow337 * fTempPerm15) + (fSlow338 * fTempPerm16))) + ((1.3228756555322954 * ((fSlow339 * fTempPerm20) + (fSlow340 * fTempPerm21))) + ((fSlow341 * fTempPerm17) + (0.9354143466934853 * ((fSlow342 * fTempPerm18) + (fSlow343 * fTempPerm19)))))) + (0.25 * ((fSlow344 * fTempPerm22) + (fSlow345 * fTempPerm23))));
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
