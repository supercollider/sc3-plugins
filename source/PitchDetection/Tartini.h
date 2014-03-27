
//from SC_fftlib.cpp
// We include vDSP even if not using for FFT, since we want to use some vectorised add/mul tricks
#if defined(__APPLE__) && !defined(SC_IPHONE)
//#include "vecLib/vDSP.h"
#include <Accelerate/Accelerate.h>
#elif defined(SC_IPHONE)
#include <Accelerate/Accelerate.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants and structs

// Decisions here about which FFT library to use - vDSP only exists on Mac BTW.
// We include the relevant libs but also ensure that one, and only one, of them is active...
#if SC_FFT_NONE
// "SC_FFT_NONE" allows compilation without FFT support; only expected to be used on very limited platforms
#define SC_FFT_FFTW 0
#define SC_FFT_VDSP 0
#define SC_FFT_GREEN 0
#elif SC_FFT_GREEN
#define SC_FFT_FFTW 0
#define SC_FFT_VDSP 0
#define SC_FFT_GREEN 1
#elif !SC_FFT_FFTW && defined(__APPLE__)
#define SC_FFT_FFTW 0
#define SC_FFT_VDSP 1
#define SC_FFT_GREEN 0
#else
//#elif SC_FFT_FFTW
#define SC_FFT_FFTW 1
#define SC_FFT_VDSP 0
#define SC_FFT_GREEN 0
#include <fftw3.h>
#endif


struct Tartini : public Unit  {
	int n,k, size;  //n=size of FFT buffer window k = autocorrelation window overlap size= n+k for autocorrelation calculation via FFT
					//double freqPerBin;
	int blocklength;
	double rate;
	
	int nover2;
	int overlap;
    int overlapindex;
	
	int m_bufWritePos;
	
	float* dataTemp;
	float * input; //for temporary storage of time window input during amortisation
	float * output; //for k lag results
	float* autocorrTime;
	float* autocorrFFT;
	
#if SC_FFT_FFTW
	fftwf_plan planAutocorrTime2FFT, planAutocorrFFT2Time;
#elif SC_FFT_VDSP
	int m_whichfftindex;
	int log2n; 
	float m_nyquist; 
	
#endif
	
	float m_currfreq, m_hasfreq; //storing most recent result
	
	int m_amortisationstate;
};

extern "C" {  
	
	void Tartini_next(Tartini *unit, int inNumSamples);
	void Tartini_Ctor(Tartini* unit);
	void Tartini_Dtor(Tartini* unit);
}


