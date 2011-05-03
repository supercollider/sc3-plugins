#include <SC_PlugIn.h>
#include <SC_InterfaceTable.h>
#include <math.h>
#include <stdio.h>

#include "sinctab.h"
#include "Oversamp.h"

// number of poles and zeros for the variable slope filter
#define npoles 4
#define nzeros 4

static inline float bilinear(float p)
{
   return ((2.f+p)/(2.f-p));
}

static inline float tanapprox (float x)
{
  return x*(x*(x*(0.96369f*x-0.865157f)+0.53576f)+0.93f);
}


static InterfaceTable *ft;

struct PeakEQ4 : public Unit
{
	double m_a[8];
	double m_b[10];
	double m_mem[10];
	float freq, width, gain;
};

struct PeakEQ2 : public Unit
{
	double m_mem[4];
	double m_a[4];
	double m_b[5];
	float freq, width, gain;
};

/* ************************************************************************ */
/* ***** Josep Comajuncosas' 18dB/oct resonant 3-pole LPF with tanh dist ** */
/* ***** Coded in C by John ffitch, 2000 Dec 17 *************************** */
/* ************************************************************************ */
/* ported to SuperCollider by Bjoern Erlach. 2008 */

struct LPF18 : public Unit
{
	float ay1, ay2, lastin, aout;
	float kp, kres, last_fco, m_value;
};

/* LPFVS.cpp
 Copyright (C) 2007 by Bjoern Erlach. */

/* Contains a variable slope lowpass filter with slope adjustable from 0-6db and
 a simple one pole lowpass (LPF1). */

struct LPF1 : public Unit
{
	float m_y1;
	float p;
	float last_freq;
};

struct LPFVS6 : public Unit
{
	float mem[npoles];     // memory for the delay lines
	float a[npoles];       // coefficients for the feedforward part
	float b[npoles+1];     // coefficients for the feedforward part
	float norm;            // gain normalization
	float last_slope;      // slope
	float last_freq;       // cutoff frequency
};

/* BLBufRd.cpp
 Copyright (C) 2007 by Bjoern Erlach. */

struct BLBufRd : public Unit
{
	SndBuf *m_buf;
	float m_fbufnum;
};

/* OSNonlinear.cpp
 Copyright (C) 2006 by Bjoern Erlach. */

struct OSWrap4 : public Oversamp4 {};
struct OSWrap8 : public Oversamp8 {};
struct OSFold4 : public Oversamp4 {};
struct OSFold8 : public Oversamp8 {};
struct OSTrunc4 : public Oversamp4 {};
struct OSTrunc8 : public Oversamp8 {};
struct Clipper8 : public Oversamp8 {};
struct Clipper4 : public Oversamp4 {};
struct SoftClipper8 : public Oversamp8 {};
struct SoftClipper4 : public Oversamp4 {};
struct SoftClipAmp8 : public Oversamp8 {};
struct SoftClipAmp4 : public Oversamp4 {};
struct DriveNoise : public Unit {};



extern "C"  {

void load(InterfaceTable *inTable);

void PeakEQ4_Ctor(PeakEQ4 *unit);
void PeakEQ4_next(PeakEQ4 *unit, int inNumSamples);

void PeakEQ2_Ctor(PeakEQ2 *unit);
void PeakEQ2_next(PeakEQ2 *unit, int inNumSamples);

void LPF18_Ctor(LPF18 *unit);
void LPF18_next(LPF18 *unit, int inNumSamples);

void LPF1_Ctor(LPF1 *unit);
void LPF1_next(LPF1 *unit, int inNumSamples);
void LPF1_next_a(LPF1 *unit, int inNumSamples);

void LPFVS6_Ctor(LPFVS6 *unit);
void LPFVS6_next(LPFVS6 *unit, int inNumSamples);

void LPF1_Ctor(LPF1 *unit);
void LPF1_next(LPF1 *unit, int inNumSamples);
void LPF1_next_a(LPF1 *unit, int inNumSamples);

void BLBufRd_Ctor(BLBufRd *unit);
void BLBufRd_next(BLBufRd *unit, int inNumSamples);


void OSWrap8_Ctor(OSWrap8 *unit);
void OSWrap8_next(OSWrap8 *unit, int inNumSamples);

void OSWrap4_Ctor(OSWrap4 *unit);
void OSWrap4_next(OSWrap4 *unit, int inNumSamples);

void OSFold4_Ctor(OSFold4 *unit);
void OSFold4_next(OSFold4 *unit, int inNumSamples);

void OSFold8_Ctor(OSFold8 *unit);
void OSFold8_next(OSFold8 *unit, int inNumSamples);

void OSTrunc4_Ctor(OSTrunc4 *unit);
void OSTrunc4_next(OSTrunc4 *unit, int inNumSamples);

void OSTrunc8_Ctor(OSTrunc8 *unit);
void OSTrunc8_next(OSTrunc8 *unit, int inNumSamples);

void Clipper8_Ctor(Clipper8 *unit);
void Clipper8_next(Clipper8 *unit, int inNumSamples);

void Clipper4_Ctor(Clipper4 *unit);
void Clipper4_next(Clipper4 *unit, int inNumSamples);

void SoftClipper8_Ctor(SoftClipper8 *unit);
void SoftClipper8_next(SoftClipper8 *unit, int inNumSamples);

void SoftClipper4_Ctor(SoftClipper4 *unit);
void SoftClipper4_next(SoftClipper4 *unit, int inNumSamples);

void SoftClipAmp8_Ctor(SoftClipAmp8 *unit);
void SoftClipAmp8_next(SoftClipAmp8 *unit, int inNumSamples);

void SoftClipAmp4_Ctor(SoftClipAmp4 *unit);
void SoftClipAmp4_next(SoftClipAmp4 *unit, int inNumSamples);

void DriveNoise_Ctor(DriveNoise *unit);
void DriveNoise_next(DriveNoise *unit, int inNumSamples);

}



// macros to put rgen state in registers
#define RGET \
	RGen& rgen = *unit->mParent->mRGen; \
	uint32 s1 = rgen.s1; \
	uint32 s2 = rgen.s2; \
	uint32 s3 = rgen.s3;

#define RPUT \
	rgen.s1 = s1; \
	rgen.s2 = s2; \
	rgen.s3 = s3;


#define G0 1.f
#define exp10f(x) expf((x)*2.30258509299404590109361f)
#define exp10(x) exp((x)*2.30258509299404590109361)

static inline void calc_coeffs4 (double *bcoeff, double *acoeff, double w0, double Dw, double gain)
{
  double G = exp10f(gain/20.f);
  // bandwidth gain is set to half gain
  double GB = exp10f(gain/40.f);
  double c0 = cosf(w0);
  double WB = tanf(Dw/2);
  double e = sqrtf((G*G-GB*GB)/(GB*GB-1.f));
  double g = powf(G, 0.25f);
  double a = powf(e, 0.25f);
#define  si1  0.382683432365089781779f;
#define  si2  0.923879532511286738483f;
  //Ba(1+i,:) = [g^2*WB^2*v, 2*g*b*si*WB, b^2*v]
  //Aa(1+i,:) = [WB^2*v, 2*a*si*WB, a^2*v]
  double Aa0 = WB*WB;
  double Aatmp = 2*a*WB;
  double Aa10 = Aatmp * si1;
  double Aa11 = Aatmp * si2;
  double as = a*a;
  double Ba0 = Aa0 * g*g;
  double Batmp = 2*g * a * WB;
  double Ba10 = Batmp * si1;
  double Ba11 = Batmp * si2;
  //  fprintf(stdout, "%f %f %f %f %f %f %f\n", Aa0, Aa10, Aa11, Ba0, Ba10, Ba11, as);
  //  double Bhat0 = Ba0/Aa0;
  //  D = A0(i)+A1(i)+A2(i)
  double D1 = Aa0+Aa11+as;
  double D0 = Aa0+Aa10+as;
  //Bhat(i,1) = (B0(i)+B1(i)+B2(i))./D
  //(B0(i)+B1(i)+B2(i))./D
  double tmp = Ba0+as;
  double x;
  double Bhat00 = (Ba10+tmp)/D0;
  double Bhat01 = (Ba11+tmp)/D1;
  x=2.f*(Ba0-as);
  double Bhat10 = x/D0;
  double Bhat11 = x/D1;
  double Bhat20 = (tmp-Ba10)/D0;
  double Bhat21 = (tmp-Ba11)/D1;
  //Ahat(i,1) = 1
  //Ahat(i,2) = 2*(A0(i)-A2(i))./D
  //Ahat(i,3) = (A0(i)-A1(i)+A2(i))./D
  x = 2.f*(Aa0-as);
  double Ahat10 = x/D0;
  double Ahat11 = x/D1;
  double Ahat20 = (Aa0-Aa10+as)/D0;
  double Ahat21 = (Aa0-Aa11+as)/D1;

  //  fprintf(stdout, "%f %f %f %f %f\n %f %f %f %f\n\n",Bhat00, Bhat01, Bhat10, Bhat11, Bhat21, Ahat11, Ahat10, Ahat20, Ahat21);


  // B(i,2) = c0*(Bhat(i,2)-2*Bhat(i,1))
  bcoeff[0] = Bhat00;
  bcoeff[5] = Bhat01;
  bcoeff[1] = c0*(Bhat10-2.f*Bhat00);
  bcoeff[6] = c0*(Bhat11-2.f*Bhat01);
  bcoeff[2] = (Bhat00-Bhat10+Bhat20)*c0*c0- Bhat10;
  bcoeff[7] = (Bhat01-Bhat11+Bhat21)*c0*c0 - Bhat11;
  bcoeff[3] = c0*(Bhat10-2.f*Bhat20);
  bcoeff[8] = c0*(Bhat11-2.f*Bhat21);
  bcoeff[4] = Bhat20;
  bcoeff[9] = Bhat21;

  //  fprintf(stdout, "%f %f %f %f %f\n %f %f %f %f %f\n", Bres00, Bres10, Bres20, Bres30, Bres40, Bres01, Bres11, Bres21, Bres31, Bres41);

  acoeff[0]= c0*(Ahat10-2.f);
  acoeff[4]= c0*(Ahat11-2.f);
  acoeff[1]= (1.f-Ahat10+Ahat20)*c0*c0 - Ahat10;
  acoeff[5]= (1.f-Ahat11+Ahat21)*c0*c0- Ahat11;
  acoeff[2]= c0*(Ahat10-2.f*Ahat20);
  acoeff[6]= c0*(Ahat11-2.f*Ahat21);
  acoeff[3]= Ahat20;
  acoeff[7]= Ahat21;

  //  fprintf(stdout, "%f %f %f %f %f\n %f %f %f %f %f\n", Ares00, Ares10, Ares20, Ares30, Ares40, Ares01, Ares11, Ares21, Ares31, Ares41);
}


void PeakEQ4_Ctor(PeakEQ4* unit)
{
  // force coeff calculation
  unit->freq = -1.1;
  memset(unit->m_mem, 0, sizeof(double)*10);
  memset(unit->m_b, 0, sizeof(double)*10);
  memset(unit->m_a, 0, sizeof(double)*8);

  SETCALC(PeakEQ4_next);
}


void PeakEQ4_next(PeakEQ4* unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *input = ZIN(0);
  float freq = ZIN0(1);
  float width = ZIN0(2);
  float gain = ZIN0(3);
  double *bc = unit->m_b;
  double *ac = unit->m_a;
  double *buf = unit->m_mem;


  if ((unit->freq!=freq)||(unit->gain!=gain)||(unit->width!=width)) {
	  double w0 = freq * 2*M_PI / SAMPLERATE;
	  double Dw = w0 * width;
	  calc_coeffs4(bc,ac,w0, Dw, gain);
  }

  LOOP(inNumSamples,
       double in = (double) ZXP(input);
       double iir;
       double iir2;
       double fir;
       iir= in-
         ac[0]*buf[3]-
         ac[1]*buf[2]-
         ac[2]*buf[1]-
         ac[3]*buf[0];
       fir= bc[0]*iir+
         bc[1]*buf[3]+
         bc[2]*buf[2]+
         bc[3]*buf[1]+
         bc[4]*buf[0];
       iir2= fir-
         ac[4]*buf[7]-
         ac[5]*buf[6]-
         ac[6]*buf[5]-
         ac[7]*buf[4];
       fir= bc[5]*iir2+
         bc[6]*buf[7]+
         bc[7]*buf[6]+
         bc[8]*buf[5]+
         bc[9]*buf[4];
       memmove(buf, buf+1, 7*sizeof(double));
       buf[3]= iir;
       buf[7]= iir2;
       ZXP(out) = (float)fir;
       )
}




////////////////////////////////////////////////////////////////////////////


static inline void calc_coeffs2 (double *bcoeff, double *acoeff, double w0, double Dw, double gain)
{
  double G = exp10(((double)gain)/20);
  // bandwidth gain is set to half gain
  double GB = exp10(((double)gain)/40);
  double c0 = cos(w0);
  double WB = tan(((double)Dw)/2);
  double e = sqrt((G*G-GB*GB)/(GB*GB-1.0));
  double g = pow(G, 0.5);
  double a = pow(e, 0.5);
#define  si  0.70710678118654748
  //Ba(1+i,:) = [g^2*WB^2*v, 2*g*b*si*WB, b^2*v]
  //Aa(1+i,:) = [WB^2*v, 2*a*si*WB, a^2*v]
  double Aa0 = WB*WB;
  double Aatmp = 2*a*WB;
  double Aa10 = Aatmp * si;
  double as = a*a;
  double Ba0 = Aa0 * g*g;
  double Batmp = 2*g * a * WB;
  double Ba10 = Batmp * si;
  //  fprintf(stdout, "%f %f %f %f %f %f %f\n", Aa0, Aa10, Aa11, Ba0, Ba10, Ba11, as);
  //  double Bhat0 = Ba0/Aa0;
  //  D = A0(i)+A1(i)+A2(i)
  double D0 = Aa0+Aa10+as;
  //Bhat(i,1) = (B0(i)+B1(i)+B2(i))./D
  //(B0(i)+B1(i)+B2(i))./D
  double tmp = Ba0+as;
  double x;
  double Bhat00 = (Ba10+tmp)/D0;
  x=2.f*(Ba0-as);
  double Bhat10 = x/D0;
  double Bhat20 = (tmp-Ba10)/D0;
  //Ahat(i,1) = 1
  //Ahat(i,2) = 2*(A0(i)-A2(i))./D
  //Ahat(i,3) = (A0(i)-A1(i)+A2(i))./D
  x = 2.f*(Aa0-as);
  double Ahat10 = x/D0;
  double Ahat20 = (Aa0-Aa10+as)/D0;

  //  fprintf(stdout, "%f %f %f %f %f\n %f %f %f %f\n\n",Bhat00, Bhat01, Bhat10, Bhat11, Bhat21, Ahat11, Ahat10, Ahat20, Ahat21);


  // B(i,2) = c0*(Bhat(i,2)-2*Bhat(i,1))
  bcoeff[0] = (double) Bhat00;
  bcoeff[1] = (double) (c0*(Bhat10-2.f*Bhat00));
  bcoeff[2] = (double) ((Bhat00-Bhat10+Bhat20)*c0*c0- Bhat10);
  bcoeff[3] = (double) (c0*(Bhat10-2.f*Bhat20));
  bcoeff[4] = (double) Bhat20;

  //  fprintf(stdout, "%f %f %f %f %f\n %f %f %f %f %f\n", Bres00, Bres10, Bres20, Bres30, Bres40, Bres01, Bres11, Bres21, Bres31, Bres41);

  acoeff[0]= (double) (c0*(Ahat10-2.f));
  acoeff[1]= (double) ((1.f-Ahat10+Ahat20)*c0*c0 - Ahat10);
  acoeff[2]= (double) (c0*(Ahat10-2.f*Ahat20));
  acoeff[3]= (double) Ahat20;

  //  fprintf(stdout, "%f %f %f %f %f\n %f %f %f %f %f\n", Ares00, Ares10, Ares20, Ares30, Ares40, Ares01, Ares11, Ares21, Ares31, Ares41);
}



void PeakEQ2_Ctor(PeakEQ2* unit)
{
  // force coeff calculation
  unit->freq = -1.1;
  memset(unit->m_mem, 0, sizeof(double)*4);
  memset(unit->m_b, 0, sizeof(double)*5);
  memset(unit->m_a, 0, sizeof(double)*4);

  SETCALC(PeakEQ2_next);
}


void PeakEQ2_next(PeakEQ2* unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *input = ZIN(0);
  float freq = ZIN0(1);
  float width = ZIN0(2);
  float gain = ZIN0(3);
  double *bc = unit->m_b;
  double *ac = unit->m_a;
  double *buf = unit->m_mem;


  if ((unit->freq!=freq)||(unit->gain!=gain)||(unit->width!=width)) {
	  double w0 = freq * 2*M_PI / SAMPLERATE;
	  double Dw = w0 * width;
	  calc_coeffs2(bc,ac,w0, Dw, gain);
  }

  LOOP(inNumSamples,
       double in = (double) ZXP(input);
       double iir;
       double fir;
       iir= in-
         ac[0]*buf[3]-
         ac[1]*buf[2]-
         ac[2]*buf[1]-
         ac[3]*buf[0];
       fir= bc[0]*iir+
         bc[1]*buf[3]+
         bc[2]*buf[2]+
         bc[3]*buf[1]+
         bc[4]*buf[0];
       memmove(buf, buf+1, 3*sizeof(double));
       buf[3]= iir;
       ZXP(out) = (float)fir;
       )
}


#define  SIGN(x) (x>=0.f?1.f:-1.f)

static inline float tanhapprox (float x) {
  if (fabsf(x) > 1.691976516634051019849f) {
	  return SIGN(x) * 0.811728001858655012768f + x * 0.068523721733649733956f;
  } else {
	  return (x * (x * (x * (x * (0.942810948279731997346f * x)) + -0.191839831351772377444f))
	   + 0.942810948279731997346f);
  }
}

//#define TANH tanhapprox
#define TANH tanhf

/* This code is transcribed from a Csound macro, so no real comments */

void LPF18_Ctor(LPF18 *unit)
{
  float fco = ZIN0(1);
  float res = ZIN0(2);
  float dist = ZIN0(3);
  float kfcn = 2.0f * fco * SAMPLEDUR;
  float kp = ((-2.7528f*kfcn + 3.0429f)*kfcn +
			   1.718f)*kfcn - 0.9984f;
  float kp1 = kp+1.0f;
  float kres = res * (((-2.7079f*kp1 + 10.963f)*kp1
		       - 14.934f)*kp1 + 8.4974f);
  float value = 1.0f+(dist*(1.5f+2.0f*kres*(1.0f-kfcn)));
  unit->kp = kp;
  unit->m_value = value;
  unit->kres = kres;
  unit->last_fco = fco;

  /* Initialise delay lines */
  unit->ay1 = 0.0f;
  unit->ay2 = 0.0f;
  unit->aout = 0.0f;
  unit->lastin = 0.0f;
  SETCALC(LPF18_next);
}

void LPF18_next(LPF18 *unit, int nsmps)
{
  float *in = ZIN(0);
  float *out = ZOUT(0);
  float fco = ZIN0(1);
  float res = ZIN0(2);
  float dist = ZIN0(3);

  float aout = unit->aout;
  float ay1 = unit->ay1;
  float ay2 = unit->ay2;
  float lastin = unit->lastin;
  float kfcn, kp = unit->kp;
  float kp1, kp1h;


  if (fco != unit->last_fco) {
	  kfcn = 2.0f * unit->last_fco * SAMPLEDUR;
	  kp1 = kp+1.0f;
	  kp1h = 0.5f*kp1;

	  float newkfcn = 2.0f * fco * SAMPLEDUR;
	  float newkp = ((-2.7528f*newkfcn + 3.0429f)*newkfcn +
			   1.718f)*newkfcn - 0.9984f;
	  float newkp1 = kp+1.0f;
	  float newkp1h = 0.5f*kp1;
	  float kp1hinc = (newkp1h-kp1h)/nsmps;
	  float kpinc = (newkp-kp)/nsmps;
	  float kres = unit->kres;
	  float newkres = res * (((-2.7079f*newkp1 + 10.963f)*newkp1
				  - 14.934f)*newkp1 + 8.4974f);
	  float kresinc = (newkres-kres)/nsmps;
	  float value = unit->m_value;
	  float newvalue = 1.0f+(dist*(1.5f+2.0f*newkres*(1.0f-newkfcn)));
	  float valueinc = (newvalue-value)/nsmps;
	  unit->kp = newkp;
	  unit->m_value = newvalue;
	  unit->kres = newkres;
	  unit->last_fco = fco;
	  LOOP(nsmps,
	       float ax1   = lastin;
	       float ay11  = ay1;
	       float ay31  = ay2;
	       lastin  =  ZXP(in) - TANH(kres*aout);
	       ay1      = kp1h * (lastin + ax1) - kp*ay1;
	       ay2      = kp1h * (ay1 + ay11) - kp*ay2;
	       aout     = kp1h * (ay2 + ay31) - kp*aout;
	       ZXP(out) = TANH(aout*value);
	       kp1h += kp1hinc;
	       kp += kpinc;
	       kres += kresinc;
	       value += valueinc;
	       );

  } else {
	  kfcn = 2.0f * fco * SAMPLEDUR;
	  kp1 = kp+1.0f;
	  kp1h = 0.5f*kp1;
	  /* Version using log */
	  /* float kres = *unit->res * (2.2173f - 1.6519f*log(kp+1.0f)); */
	  float kres = res * (((-2.7079f*kp1 + 10.963f)*kp1
			       - 14.934f)*kp1 + 8.4974f);
	  float value = 1.0f+(dist*(1.5f+2.0f*kres*(1.0f-kfcn)));
	  unit->m_value = value;
	  unit->kres = kres;
	  LOOP(nsmps,
	       float ax1   = lastin;
	       float ay11  = ay1;
	       float ay31  = ay2;
	       lastin  =  ZXP(in) - TANH(kres*aout);
	       ay1      = kp1h * (lastin + ax1) - kp*ay1;
	       ay2      = kp1h * (ay1 + ay11) - kp*ay2;
	       aout     = kp1h * (ay2 + ay31) - kp*aout;
	       ZXP(out) = TANH(aout*value);
	       );
  }



  unit->ay1 = ay1;
  unit->ay2 = ay2;
  unit->aout = aout;
  unit->lastin = lastin;
}



void
LPF1_Ctor (LPF1 *unit)
{
  unit->m_y1 = 0.f;
  if (INRATE(1) == calc_FullRate) {
	  SETCALC(LPF1_next_a);
  } else {
	  unit->last_freq = ZIN0(1);
	  unit->p = (1.f-2.f*tanf((unit->last_freq/SAMPLERATE)));
	  SETCALC(LPF1_next);
  }
}

void
LPF1_next (LPF1 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float freq = ZIN0(1);
  float p = unit->p;
  float y1 = unit->m_y1;

  if (unit->last_freq != freq) {
	  float newp = (1.f-2.f*tanf((freq/SAMPLERATE)));
	  float pinc = (newp-p)/inNumSamples;
	  unit->p=newp;
	  unit->last_freq=freq;
	  LOOP(inNumSamples,
	       ZXP(out) = y1 = (1.f-p)*ZXP(in)+ p*y1;
	       p += pinc;
	       );
  } else {
	  LOOP(inNumSamples,
	       ZXP(out) = y1 = (1.f-p)*ZXP(in)+ p*y1;
	       );
  }

   unit->m_y1 = y1;
}


void
LPF1_next_a (LPF1 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float *freq = ZIN(1);
  float y1 = unit->m_y1;
  float srate = SAMPLERATE;

  LOOP(inNumSamples,
       float p = (1.f-2.f*tanapprox((ZXP(freq)/srate)));
       ZXP(out) = y1 = (1.f-p)*ZXP(in)+ p*y1;
       );

   unit->m_y1 = y1;
}


#define PI34 ((3.f/4.f)*((float)M_PI))

#define COSPI34  (-0.707106781186547461715)


static float proto[] = { -1.0, -6.3246, -40, -252.9824, -1600, -10119.2896, -64000 };



void
LPFVS6_Ctor (LPFVS6 *unit)
{
  // force calculation of the poles and zeros
  unit->last_freq = ZIN0(1);
  unit->last_slope = ZIN0(2);
  float *b = unit->b;
  float *a = unit->a;
  float g = 1.f;

  unit->mem[0] = 0.f;  unit->mem[1] = 0.f;
  unit->mem[2] = 0.f;  unit->mem[3] = 0.f;

  float w = tanf((unit->last_freq*M_PI/(2.f*SAMPLERATE)));

  //  fprintf(stderr, "w: %f\n", w);

  b[0] = bilinear(proto[0] * w);
  //fprintf(stderr, "pole: %f, ", b[0]);

  for (int i=1; i<5; i++) {
	  b[i] = bilinear(proto[i] * w);
	  //  fprintf(stderr, "pole: %f, ", b[i]);
	  if (b[i] < 0.f) b[i] = 0.f;
	  a[i-1] = (b[i-1] + (b[i] - b[i-1]) * unit->last_slope);
	  if (a[i-1] < 0.f) a[i-1] = 0.f;
	  // normalization
	  g *= (1.f-b[i-1])/(1.f-a[i-1]);
	  //fprintf(stderr, "zero: %f, ", a[i-1]);
  }
  //fprintf(stderr, "g: %f\n", g);
  unit->norm = g;
  SETCALC(LPFVS6_next);
}



void
LPFVS6_next (LPFVS6 *unit, int inNumSamples)
{
  float *out   = ZOUT(0);
  float *in    = ZIN(0);
  float freq   = ZIN0(1);
  float slope  = ZIN0(2);
  float g = unit->norm;
  float *b = unit->b;
  float *a = unit->a;
  float *mem = unit->mem;
  float binc[5];
  float ainc[5];
  float ginc;

#if 0
  for (int i=0; i<4; i++) {
	  fprintf(stderr, "%f ", b[i]);
  }
  fprintf(stderr, "\n");
  for (int i=0; i<4; i++) {
	  fprintf(stderr, "%f ", a[i]);
  }
  fprintf(stderr, "\n");
  fprintf(stderr, "%f\n", g);
#endif

  // calculate poles for new cutoff frequency
  if (freq != unit->last_freq) {
	  float w = tanf((freq*M_PI/(2.f*SAMPLERATE)));
	  float nb[5];
	  float ng = 1.f;
	  nb[0] = bilinear(proto[0] * w);
	  binc[0] = (nb[0]-b[0])/inNumSamples;
	  for (int i=1; i<5; i++) {
		  float na;
		  nb[i] = bilinear(proto[i] * w);
		  //fprintf(stdout, "pole: %f\n", p);
		  if (nb[i] < 0.f) nb[i] = 0.f;
		  binc[i] = (nb[i]-b[i])/inNumSamples;
		  na = (nb[i-1] + (nb[i] - nb[i-1]) * slope);
		  if (na < 0.f) na = 0.f;
		  ainc[i-1] = (na - a[i-1])/inNumSamples;
		  // normalization
		  ng *= (1.f-nb[i-1])/(1.f-na);
	  }
	  ginc = (ng-g)/inNumSamples;
	  unit->last_freq = freq;
	  unit->last_slope = slope;
  } else if (slope != unit->last_slope) {
	  // caculate for new slope
	  float ng = 1.f;
	  for (int i=0; i<4; i++) {
		  float na;
		  na = b[i] + (b[i+1] - b[i]) * slope;
		  if (na < 0.f) na = 0.f;
		  ng *= (1.f-b[i])/(1.f-na);
		  ainc[i] = (na - a[i])/inNumSamples;
	  }
	  ginc = (ng-g)/inNumSamples;
	  binc[0] = 0.f; binc[1] = 0.f;binc[2] = 0.f;binc[3] = 0.f;
	  // calculate normalization
	  unit->last_slope = slope;
  } else {
	  ginc = 0.f;
	  binc[0] = 0.f; binc[1] = 0.f;binc[2] = 0.f;binc[3] = 0.f;
	  ainc[0] = 0.f; ainc[1] = 0.f;ainc[2] = 0.f;ainc[3] = 0.f;
  }



  LOOP(inNumSamples,
       float x;
       float y;
       x = ZXP(in);
       //for (k=0; k<4; k++) {
       //	       y = x + b[k]*mem[k]; // first order section (direct form II)
       //       x = y + a[k]*mem[k];
       //	       mem[k] = y;
       //}
       y = x + b[0]*mem[0]; // first order section (direct form II)
       x = y - a[0]*mem[0];
       mem[0] = y;
       b[0] += binc[0];
       a[0] += ainc[0];
       y = x + b[1]*mem[1]; // first order section (direct form II)
       x = y - a[1]*mem[1];
       mem[1] = y;
       b[1] += binc[1];
       a[1] += ainc[1];
       y = x + b[2]*mem[2]; // first order section (direct form II)
       x = y - a[2]*mem[2];
       mem[2] = y;
       b[2] += binc[2];
       a[2] += ainc[2];
       y = x + b[3]*mem[3]; // first order section (direct form II)
       x = y - a[3]*mem[3];
       mem[3] = y;
       b[3] += binc[3];
       a[3] += ainc[3];
       ZXP(out) = x*g;
       g += ginc;
       );

  unit->norm = g;
  mem[0] = zapgremlins(mem[0]);
  mem[1] = zapgremlins(mem[1]);
  mem[2] = zapgremlins(mem[2]);
  mem[3] = zapgremlins(mem[3]);
}




////////////////////////////////////////////////////////////////////////////


#define BL_GET_BUF \
	float fbufnum  = ZIN0(0); \
	if (fbufnum != unit->m_fbufnum) { \
		uint32 bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
		if (bufnum >= world->mNumSndBufs) bufnum = 0; \
		unit->m_fbufnum = fbufnum; \
		unit->m_buf = world->mSndBufs + bufnum; \
	} \
	SndBuf *buf = unit->m_buf; \

#define BL_CHECK_BUF \
	if (!bufData) { \
                unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	}

#define SINC(iphs, fphs) ((1.f-fphs) * sinctab[iphs] + fphs * sinctab[iphs+1])
#define SINCN(iphs) (sinctab[iphs])

void
BLBufRd_Ctor (BLBufRd *unit)
{
    unit->m_fbufnum = -1.f;
    SETCALC(BLBufRd_next);
}

void
BLBufRd_next (BLBufRd *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *phsin = ZIN(1);
  float ratio = ZIN0(2);


  BL_GET_BUF;


  float *bufData = buf->data;
  int bufFrames = buf->frames;

  BL_CHECK_BUF;

  LOOP(inNumSamples,
       float sum = 0.f;
       float phs = ZXP(phsin);
       float ratio256 = 256.f*(1.f/ratio);
       int pos = (int) phs;
       int nsamps = (int) (ratio * 2.f);
       float frac = phs - (float) pos;
       int n;

       n = sc_min(nsamps, pos);
       for (int k=0; k<n; k++) {
	       float distance = frac+k;
	       float fphs = ratio256*distance;
	       //int iphs = (int) fphs;
	       //fphs = fphs - (float) iphs;
	       //sum += bufData[pos - k] * SINCN(iphs, fphs);
	       int iphs = lrintf(fphs);
	       sum += bufData[pos - k] * SINCN(iphs);
       }

       frac = 1.f - frac;
       pos ++;

       n = sc_min(nsamps, bufFrames-pos);
       for (int k=0; k<n; k++) {
	       float distance = frac+k;
	       float fphs = ratio256*distance;
	       //int iphs = (int) fphs;
	       //fphs = fphs - (float) iphs;
	       //sum += bufData[pos - k] * SINCN(iphs, fphs);
	       int iphs = lrintf(fphs);
	       sum += bufData[pos + k] * SINCN(iphs);
       }

       ZXP(out) = sum/ratio;
       );

}


void
Clipper8_Ctor (Clipper8 *unit)
{
  OVERSAMPLE8_INIT;
  SETCALC(Clipper8_next);
}

void
Clipper8_next (Clipper8 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float min = ZIN0(1);
  float max = ZIN0(2);

  UPSAMPLE8;

  for (int j=0; j<(inNumSamples*8); j++) {
	  domemoff[j] = sc_clip(domemoff[j], min, max);
  }

  DOWNSAMPLE8;
}

////////////////////////////////////////////////////////////////


void
Clipper4_Ctor (Clipper4 *unit)
{
  OVERSAMPLE4_INIT;
  SETCALC(Clipper4_next);
}

void
Clipper4_next (Clipper4 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float min = ZIN0(1);
  float max = ZIN0(2);

  UPSAMPLE4;

  for (int j=0; j<(inNumSamples*4); j++) {
	  domemoff[j] = sc_clip(domemoff[j], min, max);
	  //	  domemoff[j] = domemoff[j] * 0.25;

  }

  //DOWNSAMPLE4;
  DOWNSAMPLE4;
}

////////////////////////////////////////////////////////////////////////////




void
SoftClipper8_Ctor (SoftClipper8 *unit)
{
  OVERSAMPLE8_INIT;
  SETCALC(SoftClipper8_next);
}

void
SoftClipper8_next (SoftClipper8 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);

  UPSAMPLE8;

  for (int j=0; j<(inNumSamples*8); j++) {
	  domemoff[j] = sc_softclip(domemoff[j]);

  }

  DOWNSAMPLE8;
}

////////////////////////////////////////////////////////////////


void
SoftClipper4_Ctor (SoftClipper4 *unit)
{
  OVERSAMPLE4_INIT;
  SETCALC(SoftClipper4_next);
}

void
SoftClipper4_next (SoftClipper4 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);

  UPSAMPLE4;

  for (int j=0; j<(inNumSamples*4); j++) {
	  domemoff[j] = sc_softclip(domemoff[j]);
  }

  DOWNSAMPLE4;
}

//////////////////////////////////////////////////////////////////////////

void
DriveNoise_Ctor (DriveNoise *unit)
{
  SETCALC(DriveNoise_next);
}

void
DriveNoise_next (DriveNoise *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float amount = ZIN0(1);
  int multi = (int) ZIN0(2);

  RGET

  LOOP(inNumSamples,
       float noise;
       float x;
       float m;
       m = x = ZXP(in);
       noise = frand2(s1, s2, s3);
       for (int i=0; i<multi; i++) m *= m;
       ZXP(out) = x + noise * m * amount;
  );

  RPUT
}

////////////////////////////////////////////////////////////////////////////


void
SoftClipAmp8_Ctor (SoftClipAmp8 *unit)
{
  OVERSAMPLE8_INIT;
  SETCALC(SoftClipAmp8_next);
}

void
SoftClipAmp8_next (SoftClipAmp8 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float pregain = ZIN0(1);

  UPSAMPLE8;

  for (int j=0; j<(inNumSamples*8); j++) {
	  domemoff[j] = sc_softclip(domemoff[j]*pregain);
  }

  DOWNSAMPLE8;
}

////////////////////////////////////////////////////////////////


void
SoftClipAmp4_Ctor (SoftClipAmp4 *unit)
{
  OVERSAMPLE4_INIT;
  SETCALC(SoftClipAmp4_next);
}

void
SoftClipAmp4_next (SoftClipAmp4 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float pregain = ZIN0(1);

  UPSAMPLE4;

  for (int j=0; j<(inNumSamples*4); j++) {
	  domemoff[j] = sc_softclip(domemoff[j]*pregain);
  }

  DOWNSAMPLE4;
}


void
OSWrap4_Ctor (OSWrap4 *unit)
{
  OVERSAMPLE4_INIT;
  SETCALC(OSWrap4_next);
}

void
OSWrap4_next (OSWrap4 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float min = ZIN0(1);
  float max = ZIN0(2);

  UPSAMPLE4;

  for (int j=0; j<(inNumSamples*4); j++) {
	  domemoff[j] = sc_wrap(domemoff[j], min, max);
  }

  DOWNSAMPLE4;
}


void
OSWrap8_Ctor (OSWrap8 *unit)
{
  OVERSAMPLE8_INIT;
  SETCALC(OSWrap8_next);
}

void
OSWrap8_next (OSWrap8 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float min = ZIN0(1);
  float max = ZIN0(2);

  UPSAMPLE8;

  for (int j=0; j<(inNumSamples*8); j++) {
	  domemoff[j] = sc_wrap(domemoff[j], min, max);
  }

  DOWNSAMPLE8;
}


void
OSFold4_Ctor (OSFold4 *unit)
{
  OVERSAMPLE4_INIT;
  SETCALC(OSFold4_next);
}

void
OSFold4_next (OSFold4 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float min = ZIN0(1);
  float max = ZIN0(2);

  UPSAMPLE4;

  for (int j=0; j<(inNumSamples*4); j++) {
	  domemoff[j] = sc_fold(domemoff[j], min, max);
  }

  DOWNSAMPLE4;
}


void
OSFold8_Ctor (OSFold8 *unit)
{
  OVERSAMPLE8_INIT;
  SETCALC(OSFold8_next);
}

void
OSFold8_next (OSFold8 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float min = ZIN0(1);
  float max = ZIN0(2);

  UPSAMPLE8;

  for (int j=0; j<(inNumSamples*8); j++) {
	  domemoff[j] = sc_fold(domemoff[j], min, max);
  }

  DOWNSAMPLE8;
}



void
OSTrunc4_Ctor (OSTrunc4 *unit)
{
  OVERSAMPLE4_INIT;
  SETCALC(OSTrunc4_next);
}

void
OSTrunc4_next (OSTrunc4 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float quant = ZIN0(1);

  UPSAMPLE4;

  for (int j=0; j<(inNumSamples*4); j++) {
	  domemoff[j] = sc_trunc(domemoff[j], quant);
  }

  DOWNSAMPLE4;
}


void
OSTrunc8_Ctor (OSTrunc8 *unit)
{
  OVERSAMPLE8_INIT;
  SETCALC(OSTrunc8_next);
}

void
OSTrunc8_next (OSTrunc8 *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  float *in = ZIN(0);
  float quant = ZIN0(1);


  UPSAMPLE8;

  for (int j=0; j<(inNumSamples*8); j++) {
	  domemoff[j] = sc_trunc(domemoff[j], quant);
  }

  DOWNSAMPLE8;
}


////////////////////////////////////////////////////////////////////////////////////

PluginLoad(Berlach)
{
  ft = inTable;
  DefineSimpleUnit(PeakEQ4);
  DefineSimpleUnit(PeakEQ2);
  DefineSimpleUnit(LPF18);
  DefineSimpleUnit(LPF1);
  DefineSimpleUnit(LPFVS6);
  DefineSimpleUnit(LPF1);
  DefineSimpleUnit(BLBufRd);
  DefineSimpleUnit(OSWrap4);
  DefineSimpleUnit(OSWrap8);
  DefineSimpleUnit(OSTrunc4);
  DefineSimpleUnit(OSTrunc8);
  DefineSimpleUnit(OSFold4);
  DefineSimpleUnit(OSFold8);
  DefineSimpleUnit(Clipper8);
  DefineSimpleUnit(Clipper4);
  DefineSimpleUnit(SoftClipper8);
  DefineSimpleUnit(SoftClipper4);
  DefineSimpleUnit(SoftClipAmp8);
  DefineSimpleUnit(SoftClipAmp4);
  DefineSimpleUnit(DriveNoise);


}
