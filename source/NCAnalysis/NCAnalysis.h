/*
 *  NCAnalysis.h
 *  xSC3ExtPlugins-Universal
 *
 *  Created by Nick Collins on 24/03/2009.
 *  Copyright 2009 Nick Collins. All rights reserved.
 *
 */

//#include <sndfile.h>
#include "SC_fftlib.h"
#include "SC_PlugIn.h"
#undef scfft_create
#undef scfft_dofft
#undef scfft_doifft
#undef scfft_destroy
#include "SC_Constants.h"

//#include <vecLib/vecLib.h>
#include <string.h>
#include <math.h>
//#include <stdlib.h>
//#include <stdio.h>


#include "FFT_UGens.h"

//
//struct SCComplexBuf 
//{
//	float dc, nyq;
//	SCComplex bin[1];
//};
//
//struct SCPolarBuf 
//{
//	float dc, nyq;
//	SCPolar bin[1];
//};
//
//SCPolarBuf* ToPolarApx(SndBuf *buf);
//SCComplexBuf* ToComplexApx(SndBuf *buf);

extern InterfaceTable *ft;
