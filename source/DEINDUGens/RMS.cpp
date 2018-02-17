//-----------------------------------------------------
// name: "RMS"
// version: "1.1"
// author: "Till Bovermann"
// license: "GPL2+"
// copyright: "(c) Till Bovermann 2016, based on LPF1 as found in BerlachUGens"
//-----------------------------------------------------

#include "SC_PlugIn.h"
// #include <stdio.h>

// #ifndef MAXFLOAT
// # include <float.h>
// # define MAXFLOAT FLT_MAX
// #endif

static inline float tanapprox (float x)
{
  return x*(x*(x*(0.96369f*x-0.865157f)+0.53576f)+0.93f);
}


// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;
typedef unsigned int uint;
// declare struct to hold unit generator state
struct RMS : public Unit
{
	float m_y1;
	float p;
	float last_lpf;
	float last_freq;
};

void RMS_Ctor(RMS *unit);
void RMS_next(RMS *unit, int inNumSamples);
void RMS_next_a(RMS *unit, int inNumSamples);


void
RMS_Ctor (RMS *unit)
{
  unit->m_y1 = 0.f;
  unit->last_lpf = 0;
  unit->last_freq = ZIN0(1);
  unit->p = (1.f-2.f*tanf((unit->last_freq/SAMPLERATE)));
  if (INRATE(1) == calc_FullRate) {
	  SETCALC(RMS_next_a);
  } else {
    unit->last_freq = ZIN0(1);
	  unit->p = (1.f-2.f*tanf((unit->last_freq/SAMPLERATE)));
	  SETCALC(RMS_next);
  }

  RMS_next(unit, 1);
}

void
RMS_next (RMS *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float freq = ZIN0(1);
  float p = unit->p;
  float y1 = unit->m_y1;
  float lastLPF = unit->last_lpf;
  float inVal;

  if (unit->last_freq != freq) {
	  float newp = (1.f-2.f*tanf((freq/SAMPLERATE)));
	  float pinc = (newp-p)/inNumSamples;
	  unit->p=newp;
	  unit->last_freq=freq;
	  LOOP(inNumSamples,
	  	   inVal = ZXP(in);
	  	   lastLPF = (1.f-p)*(inVal*inVal)+ p*lastLPF;
	       ZXP(out) = y1 = zapgremlins(sqrt(lastLPF));
	       p += pinc;
	  );
  } else {
	  LOOP(inNumSamples,
	  		 inVal = ZXP(in);
	  		 lastLPF = (1.f-p)*(inVal*inVal)+ p*lastLPF;
	  	     ZXP(out) = y1 = zapgremlins(sqrt(lastLPF));
	  );
  }

  unit->m_y1 = y1;
  unit->last_lpf = lastLPF;
}


void
RMS_next_a (RMS *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float *freq = ZIN(1);
  float y1 = unit->m_y1;
  float srate = SAMPLERATE;
  float lastLPF = unit->last_lpf;

  LOOP(inNumSamples,
       float p = (1.f-2.f*tanapprox((ZXP(freq)/srate)));
       float inVal = ZXP(in);
       lastLPF = (1.f-p)*(inVal*inVal)+ p*lastLPF;
       ZXP(out) = y1 = zapgremlins(sqrt(lastLPF));
  );

   unit->m_y1 = y1;
   unit->last_lpf = lastLPF;
}


//////////////////////////////
PluginLoad(RMS)
{
	// InterfaceTable *inTable implicitly given as argument to load function
	ft = inTable;
	DefineSimpleUnit(RMS);
}