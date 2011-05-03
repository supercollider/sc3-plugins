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


struct Decimator : public Unit
{
  float mCount;
  float mLastOut;
};

struct SineShaper : public Unit
{
  // anything needed ?
};

struct Disintegrator : public Unit
{
  float mLastInput;
  bool mActive;
};

//----------------------------------------------------------

extern "C"

{
  void load(InterfaceTable *inTable);


  // from ladspa-util.h - should me make an extra header file for this?
  static inline float f_clamp(float x, float a, float b);
  static inline float cube_interp(const float fr, const float inm1,
				  const float in, const float inp1, const float inp2);

  // Crossover Distortion
  void CrossoverDistortion_next(CrossoverDistortion *unit, int inNumSamples);
  void CrossoverDistortion_Ctor(CrossoverDistortion* unit);

  // Smooth Decimator
  void SmoothDecimator_next(SmoothDecimator *unit, int inNumSamples);
  void SmoothDecimator_Ctor(SmoothDecimator* unit);
  void SmoothDecimator_Dtor(SmoothDecimator* unit);

  // Decimator
  void Decimator_next(Decimator *unit, int inNumSamples);
  void Decimator_Ctor(Decimator* unit);

  // SineShaper
  void SineShaper_next(SineShaper *unit, int inNumSamples);
  void SineShaper_Ctor(SineShaper* unit);

  // Disintegrator
  void Disintegrator_next(Disintegrator *unit, int inNumSamples);
  void Disintegrator_Ctor(Disintegrator* unit);
};

//----------------------------------------------------------
// utilities
//----------------------------------------------------------

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
static inline float cube_interp(const float fr, const float inm1,
				const float in, const float inp1, const float inp2)
{
  return in + 0.5f * fr * (inp1 - inm1 +
			   fr * (4.0f * inp1 + 2.0f * inm1 - 5.0f * in - inp2 +
				 fr * (3.0f * (in - inp1) - inm1 + inp2)));
}

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

// Decimator
void Decimator_Ctor(Decimator* unit)
{
  unit->mCount = 0.0f;
  unit->mLastOut = 0.0f;

  SETCALC(Decimator_next);
  Decimator_next(unit,1);
}

// SineShaper
void SineShaper_Ctor(SineShaper* unit)
{
  SETCALC(SineShaper_next);
  SineShaper_next(unit,1);
}

// Disintegrator
void Disintegrator_Ctor(Disintegrator* unit)
{
  unit->mLastInput = 0.0f;
  unit->mActive = false;

  SETCALC(Disintegrator_next);
  Disintegrator_next(unit,1);
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

void Decimator_next(Decimator *unit, int inNumSamples)
{
  float *in = IN(0);
  float *out = OUT(0);
  float rate = IN0(1);
  float bits = IN0(2);

  float count = unit->mCount;
  float last_out = unit->mLastOut;
  long sample_rate = fSampleRate;
  float step, stepr, delta, ratio;
  double dummy;

  if (bits >= 31.0f || bits < 1.0f) {
    step = 0.0f;
    stepr = 1.0f;
  } else {
    step = pow(0.5f, bits - 0.999f);
    stepr = 1/step;
  }

  if (rate >= sample_rate) {
    ratio = 1.0f;
  } else {
    ratio = rate/sample_rate;
  }

  for (int pos = 0; pos < inNumSamples; pos++) {
    count += ratio;

    if (count >= 1.0f) {
      count -= 1.0f;
      delta = modf((in[pos] + (in[pos]<0?-1.0:1.0)*step*0.5) * stepr, &dummy) * step;
      last_out = in[pos] - delta;
      out[pos] = last_out;
    } else {
      out[pos] = last_out;
    }
  }

  unit->mLastOut = last_out;
  unit->mCount = count;
}

void SineShaper_next(SineShaper *unit, int inNumSamples)
{
  float *in = IN(0);
  float *out = OUT(0);
  float limit = IN0(1);

  float oneOverLimit = 1.f / limit;
  for(int i = 0; i < inNumSamples; i++)
    {
      out[i] = limit * sin(in[i] * oneOverLimit);
    }
}

void Disintegrator_next(Disintegrator *unit, int inNumSamples)
{
  float *in = IN(0);
  float *out = OUT(0);
  float prob = IN0(1);
  float mult = IN0(2);

  for(int i = 0; i < inNumSamples; i++)
    {
      if( (unit->mLastInput>0 && in[i]<0) || (unit->mLastInput<0 && in[i]>0))
	unit->mActive = rand() < prob*RAND_MAX;

      unit->mLastInput = in[i];

      if(unit->mActive)
	out[i] = in[i] * mult;
      else
	out[i] = in[i];
    }
}


//----------------------------------------------------------

PluginLoad(Ladspa)
{
  ft = inTable;

  DefineSimpleUnit(CrossoverDistortion);
  DefineSimpleUnit(SmoothDecimator);
  DefineSimpleUnit(Decimator);
  DefineSimpleUnit(SineShaper);
  DefineSimpleUnit(Disintegrator);
}
