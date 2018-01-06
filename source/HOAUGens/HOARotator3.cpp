//----------------------------------------------------------
// name: "HOARotator3"
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
	int fSamplingFreq;

  public:
	virtual void metadata(Meta* m) { 
		m->declare("name", "HOARotator3");
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

	virtual int getNumInputs() { return 16; }
	virtual int getNumOutputs() { return 16; }
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
		ui_interface->openVerticalBox("HOARotator3");
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
		double 	fSlow27 = ((fSlow21 * fSlow26) + (fSlow6 * fSlow10));
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
		double 	fSlow41 = ((1.1547005383792515 * ((fSlow1 * fSlow19) * fSlow11)) - (0.5773502691896257 * (fSlow12 * (fSlow39 + fSlow40))));
		double 	fSlow42 = faustpower<2>(fSlow11);
		double 	fSlow43 = (faustpower<2>(fSlow1) * fSlow42);
		double 	fSlow44 = faustpower<2>(fSlow6);
		double 	fSlow45 = (faustpower<2>(fSlow7) * fSlow26);
		double 	fSlow46 = (fSlow44 + fSlow45);
		double 	fSlow47 = (fSlow43 - (0.5 * fSlow46));
		double 	fSlow48 = (fSlow14 * fSlow11);
		double 	fSlow49 = ((fSlow7 * fSlow16) * fSlow12);
		double 	fSlow50 = ((1.1547005383792515 * (fSlow20 * fSlow22)) - (0.5773502691896257 * (fSlow48 + fSlow49)));
		double 	fSlow51 = (faustpower<2>(fSlow3) * fSlow42);
		double 	fSlow52 = faustpower<2>(fSlow16);
		double 	fSlow53 = (faustpower<2>(fSlow9) * fSlow26);
		double 	fSlow54 = faustpower<2>(fSlow10);
		double 	fSlow55 = ((0.5773502691896257 * (faustpower<2>(fSlow22) - faustpower<2>(fSlow19))) - (0.28867513459481287 * ((fSlow51 + fSlow52) - (fSlow53 + fSlow54))));
		double 	fSlow56 = (((fSlow9 * fSlow22) * fSlow12) + ((fSlow3 * fSlow19) * fSlow11));
		double 	fSlow57 = (((fSlow15 * fSlow11) * fSlow12) + (1.0000000000000002 * (fSlow6 * fSlow19)));
		double 	fSlow58 = (1.7320508075688772 * (fSlow18 * fSlow11));
		double 	fSlow59 = ((fSlow4 * fSlow42) + (1.0000000000000002 * (fSlow6 * fSlow22)));
		double 	fSlow60 = ((fSlow24 * fSlow22) - ((fSlow9 * fSlow19) * fSlow12));
		double 	fSlow61 = (fSlow48 - fSlow49);
		double 	fSlow62 = (fSlow39 - fSlow40);
		double 	fSlow63 = (fSlow62 * fSlow12);
		double 	fSlow64 = (fSlow44 - fSlow45);
		double 	fSlow65 = (0.8660254037844386 * fSlow64);
		double 	fSlow66 = (0 - (2 * (fSlow36 - fSlow37)));
		double 	fSlow67 = ((fSlow51 + fSlow54) - (fSlow53 + fSlow52));
		double 	fSlow68 = (fSlow6 * fSlow25);
		double 	fSlow69 = (0.5 * (fSlow13 * fSlow66));
		double 	fSlow70 = (fSlow68 + fSlow69);
		double 	fSlow71 = (fSlow6 * fSlow30);
		double 	fSlow72 = (0.5 * (fSlow13 * fSlow67));
		double 	fSlow73 = (fSlow71 + fSlow72);
		double 	fSlow74 = (1.7320508075688772 * fSlow44);
		double 	fSlow75 = (0.9128709291752769 * (fSlow13 * (fSlow65 + fSlow74)));
		double 	fSlow76 = (fSlow27 * fSlow6);
		double 	fSlow77 = ((fSlow62 * fSlow7) * fSlow26);
		double 	fSlow78 = (fSlow76 + fSlow77);
		double 	fSlow79 = (fSlow6 * fSlow29);
		double 	fSlow80 = (fSlow13 * fSlow61);
		double 	fSlow81 = (fSlow79 + fSlow80);
		double 	fSlow82 = ((fSlow24 * fSlow25) + (fSlow23 * fSlow30));
		double 	fSlow83 = (0.5 * ((fSlow16 * fSlow66) + (fSlow10 * fSlow67)));
		double 	fSlow84 = (fSlow82 + fSlow83);
		double 	fSlow85 = (fSlow24 * fSlow30);
		double 	fSlow86 = (0.5 * ((fSlow16 * fSlow67) - (fSlow10 * fSlow66)));
		double 	fSlow87 = (fSlow23 * fSlow25);
		double 	fSlow88 = ((fSlow85 + fSlow86) - fSlow87);
		double 	fSlow89 = ((fSlow13 * fSlow60) + ((fSlow20 * fSlow30) + (fSlow6 * fSlow35)));
		double 	fSlow90 = (((fSlow27 * fSlow1) * fSlow11) + ((fSlow32 * fSlow6) + ((fSlow57 * fSlow7) * fSlow12)));
		double 	fSlow91 = ((fSlow20 * fSlow29) + ((fSlow6 * fSlow34) + (fSlow13 * fSlow59)));
		double 	fSlow92 = ((fSlow13 * fSlow56) + ((fSlow20 * fSlow25) + (fSlow6 * fSlow31)));
		double 	fSlow93 = (3.872983346207417 * (((fSlow18 * fSlow7) * fSlow11) * fSlow12));
		double 	fSlow94 = (((fSlow22 * fSlow25) + (fSlow19 * fSlow30)) + ((fSlow10 * fSlow60) + ((fSlow23 * fSlow35) + ((fSlow24 * fSlow31) + (fSlow16 * fSlow56)))));
		double 	fSlow95 = (((fSlow22 * fSlow30) - (fSlow19 * fSlow25)) + (((fSlow24 * fSlow35) + (fSlow16 * fSlow60)) - ((fSlow23 * fSlow31) + (fSlow10 * fSlow56))));
		double 	fSlow96 = (((0.5163977794943222 * ((fSlow22 * fSlow31) + (fSlow19 * fSlow35))) + (0.447213595499958 * ((fSlow16 * fSlow38) + (fSlow10 * fSlow55)))) - (0.12909944487358055 * (fSlow82 - fSlow83)));
		double 	fSlow97 = (((1.2649110640673518 * (fSlow20 * fSlow31)) + (1.0954451150103324 * (fSlow13 * fSlow38))) - (0.31622776601683794 * (fSlow68 - fSlow69)));
		double 	fSlow98 = ((((fSlow32 * fSlow1) * fSlow11) + (0.8660254037844386 * (fSlow13 * fSlow41))) - (0.25 * (fSlow76 - fSlow77)));
		double 	fSlow99 = ((1.6329931618554518 * fSlow43) + (0.816496580927726 * fSlow47));
		double 	fSlow100 = (fSlow13 * (fSlow99 - (0.23570226039551584 * (fSlow74 - fSlow65))));
		double 	fSlow101 = (((fSlow20 * fSlow34) + (0.8660254037844386 * (fSlow13 * fSlow50))) - (0.25 * (fSlow79 - fSlow80)));
		double 	fSlow102 = (((1.2649110640673518 * (fSlow20 * fSlow35)) + (1.0954451150103324 * (fSlow13 * fSlow55))) - (0.31622776601683794 * (fSlow71 - fSlow72)));
		double 	fSlow103 = (((0.5163977794943222 * ((fSlow22 * fSlow35) - (fSlow19 * fSlow31))) + (0.447213595499958 * ((fSlow16 * fSlow55) - (fSlow10 * fSlow38)))) - (0.12909944487358055 * (fSlow85 - (fSlow87 + fSlow86))));
		double 	fSlow104 = (fSlow6 * fSlow60);
		double 	fSlow105 = (fSlow13 * fSlow35);
		double 	fSlow106 = ((1.3416407864998738 * (fSlow20 * fSlow55)) - (0.7745966692414834 * (fSlow104 + fSlow105)));
		double 	fSlow107 = (fSlow57 * fSlow6);
		double 	fSlow108 = ((fSlow32 * fSlow7) * fSlow12);
		double 	fSlow109 = ((1.0606601717798212 * (fSlow20 * fSlow41)) - (0.6123724356957945 * (fSlow107 + fSlow108)));
		double 	fSlow110 = (fSlow20 * (fSlow47 - (0.9999999999999999 * fSlow46)));
		double 	fSlow111 = (fSlow24 * fSlow60);
		double 	fSlow112 = (fSlow16 * fSlow35);
		double 	fSlow113 = (fSlow23 * fSlow56);
		double 	fSlow114 = (fSlow10 * fSlow31);
		double 	fSlow115 = ((0.5477225575051661 * ((fSlow22 * fSlow55) - (fSlow19 * fSlow38))) - (0.31622776601683794 * ((fSlow111 + fSlow112) - (fSlow113 + fSlow114))));
		double 	fSlow116 = (fSlow10 * fSlow35);
		double 	fSlow117 = ((fSlow24 * fSlow56) + (fSlow23 * fSlow60));
		double 	fSlow118 = (fSlow16 * fSlow31);
		double 	fSlow119 = ((0.5477225575051661 * ((fSlow22 * fSlow38) + (fSlow19 * fSlow55))) - (0.31622776601683794 * (fSlow116 + (fSlow117 + fSlow118))));
		double 	fSlow120 = (fSlow6 * fSlow56);
		double 	fSlow121 = (fSlow13 * fSlow31);
		double 	fSlow122 = ((1.3416407864998738 * (fSlow20 * fSlow38)) - (0.7745966692414834 * (fSlow120 + fSlow121)));
		double 	fSlow123 = (fSlow6 * fSlow59);
		double 	fSlow124 = (fSlow13 * fSlow34);
		double 	fSlow125 = ((1.0606601717798212 * (fSlow20 * fSlow50)) - (0.6123724356957945 * (fSlow123 + fSlow124)));
		double 	fSlow126 = (fSlow6 * fSlow61);
		double 	fSlow127 = (fSlow13 * fSlow29);
		double 	fSlow128 = (((fSlow20 * fSlow59) + (0.8660254037844386 * (fSlow6 * fSlow50))) - (0.25 * (fSlow126 + fSlow127)));
		double 	fSlow129 = (fSlow10 * fSlow30);
		double 	fSlow130 = (0.5 * ((fSlow24 * fSlow66) + (fSlow23 * fSlow67)));
		double 	fSlow131 = (fSlow16 * fSlow25);
		double 	fSlow132 = (((0.5163977794943222 * ((fSlow22 * fSlow56) + (fSlow19 * fSlow60))) + (0.447213595499958 * ((fSlow24 * fSlow38) + (fSlow23 * fSlow55)))) - (0.12909944487358055 * (fSlow129 + (fSlow130 + fSlow131))));
		double 	fSlow133 = (1.7320508075688772 * fSlow45);
		double 	fSlow134 = (fSlow6 * (fSlow99 - (0.23570226039551584 * (fSlow65 + fSlow133))));
		double 	fSlow135 = (0.5 * (fSlow6 * fSlow67));
		double 	fSlow136 = (fSlow13 * fSlow30);
		double 	fSlow137 = (((1.2649110640673518 * (fSlow20 * fSlow60)) + (1.0954451150103324 * (fSlow6 * fSlow55))) - (0.31622776601683794 * (fSlow135 + fSlow136)));
		double 	fSlow138 = (0.5 * (fSlow6 * fSlow66));
		double 	fSlow139 = (fSlow13 * fSlow25);
		double 	fSlow140 = (((1.2649110640673518 * (fSlow20 * fSlow56)) + (1.0954451150103324 * (fSlow6 * fSlow38))) - (0.31622776601683794 * (fSlow138 + fSlow139)));
		double 	fSlow141 = (fSlow62 * fSlow6);
		double 	fSlow142 = (fSlow27 * fSlow7);
		double 	fSlow143 = ((((fSlow57 * fSlow1) * fSlow11) + (0.8660254037844386 * (fSlow6 * fSlow41))) - (0.25 * (fSlow12 * (fSlow141 + fSlow142))));
		double 	fSlow144 = (fSlow16 * fSlow30);
		double 	fSlow145 = (0.5 * ((fSlow24 * fSlow67) - (fSlow23 * fSlow66)));
		double 	fSlow146 = (fSlow10 * fSlow25);
		double 	fSlow147 = (((0.5163977794943222 * ((fSlow22 * fSlow60) - (fSlow19 * fSlow56))) + (0.447213595499958 * ((fSlow24 * fSlow55) - (fSlow23 * fSlow38)))) - (0.12909944487358055 * ((fSlow144 + fSlow145) - fSlow146)));
		double 	fSlow148 = ((0.2041241452319315 * ((fSlow22 * fSlow67) - (fSlow19 * fSlow66))) + (0.408248290463863 * ((fSlow111 + fSlow114) - (fSlow112 + fSlow113))));
		double 	fSlow149 = ((0.2041241452319315 * ((fSlow22 * fSlow66) + (fSlow19 * fSlow67))) + (0.408248290463863 * (fSlow117 - (fSlow118 + fSlow116))));
		double 	fSlow150 = (((0.5 * (fSlow20 * fSlow66)) + fSlow120) - fSlow121);
		double 	fSlow151 = (1.9364916731037085 * (fSlow20 * fSlow64));
		double 	fSlow152 = (((0.5 * (fSlow20 * fSlow67)) + fSlow104) - fSlow105);
		double 	fSlow153 = ((((fSlow62 * fSlow1) * fSlow11) * fSlow12) + (fSlow107 - fSlow108));
		double 	fSlow154 = ((fSlow20 * fSlow61) + (fSlow123 - fSlow124));
		double 	fSlow155 = (fSlow138 - fSlow139);
		double 	fSlow156 = (fSlow135 - fSlow136);
		double 	fSlow157 = (0.9128709291752769 * (fSlow6 * (fSlow65 - fSlow133)));
		double 	fSlow158 = ((fSlow141 - fSlow142) * fSlow12);
		double 	fSlow159 = (fSlow126 - fSlow127);
		double 	fSlow160 = (fSlow130 - (fSlow131 + fSlow129));
		double 	fSlow161 = ((fSlow145 + fSlow146) - fSlow144);
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
			output6[i] = (FAUSTFLOAT)(((((fSlow38 * fTempPerm3) + (fSlow41 * fTempPerm4)) + (fSlow47 * fTempPerm5)) + (fSlow50 * fTempPerm6)) + (fSlow55 * fTempPerm7));
			output7[i] = (FAUSTFLOAT)(((((fSlow56 * fTempPerm3) + (fSlow57 * fTempPerm4)) + (fSlow58 * fTempPerm5)) + (fSlow59 * fTempPerm6)) + (fSlow60 * fTempPerm7));
			output8[i] = (FAUSTFLOAT)(((fSlow61 * fTempPerm6) + ((fSlow63 * fTempPerm4) + (fSlow65 * fTempPerm5))) + (0.5 * ((fSlow66 * fTempPerm3) + (fSlow67 * fTempPerm7))));
			fTempPerm8 = (double)input10[i];
			fTempPerm9 = (double)input14[i];
			fTempPerm10 = (double)input12[i];
			fTempPerm11 = (double)input11[i];
			fTempPerm12 = (double)input13[i];
			fTempPerm13 = (double)input9[i];
			fTempPerm14 = (double)input15[i];
			output9[i] = (FAUSTFLOAT)(((1.224744871391589 * ((fSlow70 * fTempPerm8) + (fSlow73 * fTempPerm9))) + ((fSlow75 * fTempPerm10) + (0.9682458365518543 * ((fSlow78 * fTempPerm11) + (fSlow81 * fTempPerm12))))) + (0.5 * ((fSlow84 * fTempPerm13) + (fSlow88 * fTempPerm14))));
			output10[i] = (FAUSTFLOAT)((fSlow89 * fTempPerm9) + ((0.7905694150420949 * ((fSlow90 * fTempPerm11) + (fSlow91 * fTempPerm12))) + (((fSlow92 * fTempPerm8) + (fSlow93 * fTempPerm10)) + (0.408248290463863 * ((fSlow94 * fTempPerm13) + (fSlow95 * fTempPerm14))))));
			output11[i] = (FAUSTFLOAT)(((((((fSlow96 * fTempPerm13) + (fSlow97 * fTempPerm8)) + (fSlow98 * fTempPerm11)) + (fSlow100 * fTempPerm10)) + (fSlow101 * fTempPerm12)) + (fSlow102 * fTempPerm9)) + (fSlow103 * fTempPerm14));
			output12[i] = (FAUSTFLOAT)((((((fSlow106 * fTempPerm9) + ((fSlow109 * fTempPerm11) + (fSlow110 * fTempPerm10))) + (fSlow115 * fTempPerm14)) + (fSlow119 * fTempPerm13)) + (fSlow122 * fTempPerm8)) + (fSlow125 * fTempPerm12));
			output13[i] = (FAUSTFLOAT)((((((fSlow128 * fTempPerm12) + ((fSlow132 * fTempPerm13) + (fSlow134 * fTempPerm10))) + (fSlow137 * fTempPerm9)) + (fSlow140 * fTempPerm8)) + (fSlow143 * fTempPerm11)) + (fSlow147 * fTempPerm14));
			output14[i] = (FAUSTFLOAT)((((fSlow148 * fTempPerm14) + (((fSlow149 * fTempPerm13) + (fSlow150 * fTempPerm8)) + (fSlow151 * fTempPerm10))) + (fSlow152 * fTempPerm9)) + (0.7905694150420949 * ((fSlow153 * fTempPerm11) + (fSlow154 * fTempPerm12))));
			output15[i] = (FAUSTFLOAT)(((1.224744871391589 * ((fSlow155 * fTempPerm8) + (fSlow156 * fTempPerm9))) + ((fSlow157 * fTempPerm10) + (0.9682458365518543 * ((fSlow158 * fTempPerm11) + (fSlow159 * fTempPerm12))))) + (0.5 * ((fSlow160 * fTempPerm13) + (fSlow161 * fTempPerm14))));
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
