#include "SC_PlugIn.h"
#include "math.h"

#define fSampleRate (float)SAMPLERATE // simple cast for samplerate to be float
#define LIN_INTERP(f,a,b) ((a) + (f) * ((b) - (a))) // from ladspa-util.h


static InterfaceTable *ft;

//---------------------------------------------------------- 

struct CrossoverDistortion : public Unit
{
  // needed ? - yes... especially since SCs control period tends to be large
  float mAmp;
  float mSmooth;
    float mRfade;
};

struct SmoothDecimator  : public Unit
{
  float mAccum;
  float *mBuffer;
  int mBufferPos;
};

//---------------------------------------------------------- 

extern "C"

{
  void load(InterfaceTable *inTable);
  
  // Crossover Distortion
  void CrossoverDistortion_next(CrossoverDistortion *unit, int inNumSamples);
  void CrossoverDistortion_Ctor(CrossoverDistortion* unit);

  // Smooth Decimator
  void SmoothDecimator_next(SmoothDecimator *unit, int inNumSamples);
  static inline float f_clamp(float x, float a, float b);
  static inline float cube_interp(const float fr, const float inm1, 
				  const float in, const float inp1, const float inp2);
  void SmoothDecimator_Ctor(SmoothDecimator* unit);
  void SmoothDecimator_Dtor(SmoothDecimator* unit);
};

//---------------------------------------------------------- 

void CrossoverDistortion_Ctor(CrossoverDistortion* unit)

{
  SETCALC(CrossoverDistortion_next);
    unit->mAmp = IN0(1);
    unit->mSmooth = IN0(2);
    unit->mRfade = 1 / (fabs(unit->mAmp * unit->mSmooth));
  CrossoverDistortion_next(unit, 1); 
}

void SmoothDecimator_Ctor(SmoothDecimator* unit)
{
  unit->mAccum = 0.0f;
  unit->mBuffer = (float*)RTAlloc(unit->mWorld, 8 * sizeof(float));
  unit->mBufferPos = 0;

  SETCALC(SmoothDecimator_next);
  SmoothDecimator_next(unit, 1); 
}

void SmoothDecimator_Dtor(SmoothDecimator* unit)
{
  RTFree(unit->mWorld,unit->mBuffer);
}

//---------------------------------------------------------- 

void CrossoverDistortion_next(CrossoverDistortion *unit, int inNumSamples)
{

  float *out = OUT(0);
  float *in = IN(0);
  float nextAmp = IN0(1);
  float nextSmooth = IN0(2);
  float sig;
    float amp, smooth, rfadeSlope, ampSlope, smoothSlope;
    ampSlope = smoothSlope = rfadeSlope = 0.0f;
    amp = unit->mAmp;
    smooth = unit->mSmooth;
    float rfade = unit->mRfade;
    
//    float fade = fabs(amp * smooth);
//    float rfade = 1./fade;
    // see if we need to interpolate over the control period, and avoid the constant 1/fade divide!
    if((nextAmp != amp) || (nextSmooth != smooth)){
	float fade = fabs(amp * smooth);
	unit->mRfade = 1./fade;
	rfadeSlope = CALCSLOPE(unit->mRfade, rfade);
	ampSlope = CALCSLOPE(nextAmp, amp);
	smoothSlope = CALCSLOPE(nextSmooth, smooth);
	unit->mAmp = nextAmp;
	unit->mSmooth = nextSmooth;
	}

  for (int i=0; i < inNumSamples; ++i)
    {
	if((nextAmp != amp) || (nextSmooth != smooth)){
	    
	    
	    sig = fabs(in[i]) - amp;
	    
	    if (sig < 0.0f) {                                                                            
		sig *= (1.0f + (sig*rfade)) * smooth;                                                         
	    }                                                                                            
	    
	    if (in[i] < 0.0f) {                                                                     
		out[i] = -sig;                                                           
	    } else {                                                                                     
		out[i] = sig;
	    }
	    rfade += rfadeSlope;
	    amp += ampSlope;
	    smooth += smoothSlope;
	} else {
	    

	sig = fabs(in[i]) - amp;
                                                                                                       
      if (sig < 0.0f) {                                                                            
	sig *= (1.0f + (sig*rfade)) * smooth;                                                         
      }                                                                                            
      
      if (in[i] < 0.0f) {                                                                     
	out[i] = -sig;                                                           
      } else {                                                                                     
	out[i] = sig;
      }
	}
    }
}

// from ladspa-util.h
static inline float f_clamp(float x, float a, float b)
{
	const float x1 = fabs(x - a);
	const float x2 = fabs(x - b);

	x = x1 + a + b;
	x -= x2;
	x *= 0.5;

	return x;
}

// from ladspa-util.h
static inline float cube_interp(const float fr, const float inm1, const float in, const float inp1, const float inp2)
{
  return in + 0.5f * fr * (inp1 - inm1 +
			   fr * (4.0f * inp1 + 2.0f * inm1 - 5.0f * in - inp2 +
				 fr * (3.0f * (in - inp1) - inm1 + inp2)));
}

void SmoothDecimator_next(SmoothDecimator *unit, int inNumSamples)
{
  float *in = IN(0); //our signal input
  float *out = OUT(0); //out signal output
  float rate = IN0(1);
  float smooth = IN0(2);
  
  float smoothed;
  float accum = unit->mAccum;
  float * buffer = unit->mBuffer;
  int buffer_pos = unit->mBufferPos;

  float inc = (rate / fSampleRate);
  inc = f_clamp(inc, 0.0f, 1.0f);
  
  for (int pos = 0; pos < inNumSamples; pos++) {
    accum += inc;
    if (accum >= 1.0f) {
      accum -= 1.0f;
      buffer_pos = (buffer_pos + 1) & 7;
      buffer[buffer_pos] = in[pos];
    }
    smoothed = cube_interp(accum, buffer[(buffer_pos - 3) & 7],
			   buffer[(buffer_pos - 2) & 7],
			   buffer[(buffer_pos - 1) & 7],
			   buffer[buffer_pos]);
    out[pos] = LIN_INTERP(smooth, buffer[(buffer_pos - 3) & 7], smoothed);
  }

  unit->mAccum = accum;
  unit->mBufferPos = buffer_pos;
}


//---------------------------------------------------------- 

void load(InterfaceTable *inTable)
{
  ft = inTable;

  DefineSimpleUnit(CrossoverDistortion);
  DefineSimpleUnit(SmoothDecimator);
}
