//This file is part of MachineListening. Copyright (C) 2006  Nicholas M.Collins distributed under the terms of the GNU General Public License full notice in file MachineListening.license


struct Concat : Unit {
	
	//essential constants
	int m_sr;
	int m_blocksize;
	int m_fftsize;
	int m_nover2;
	int m_blocksperframe;
	int m_samplesforzcr;
	
	//FFT data
	int m_bufWritePos;
	float * m_FFTBufsource;
	float * m_FFTBufcontrol;
	//float * m_FFTBufsourceafter;
	//float * m_FFTBufcontrolafter;
	
	//vDSP
	//unsigned long m_vlog2n;
	//COMPLEX_SPLIT m_vA;
	//FFTSetup m_vsetup;
	
	//float* afterFFTSource, *afterFFTControl;
	fftwf_plan planTime2FFT; //Source, planTime2FFTControl;
	
	
	//To fix a database once captured enough
	int m_freezestore;
	
	//stored samples
	
	float * m_controlstore;
	float * m_sourcestore;
	int m_controlcounter;
	int m_sourcecounter;
	int m_sourcesize;
	int m_controlsize;
	
	//stored features (for source stream only)
	int m_sourceframes;
	int m_featurecounter;
	
	//time domain
	float * m_rms;
	float * m_zcr;
	
	//frequency domain
	float * m_speccentroid;
	float * m_spectilt;
	
	//control playback
	int m_matchlocation;
	int m_matchcounter;
	int m_matchframes;
	int m_fadeoutlocation;
	
};


extern "C"
{
	//required interface functions
	void Concat_next(Concat *unit, int wrongNumSamples);
	void Concat_Ctor(Concat *unit);
	void Concat_Dtor(Concat *unit);

}

//other functions
void Concat_dofft(Concat *unit, float *);
void sourcefeatures(Concat *unit, float * fftbuf);
void matchfeatures(Concat *unit, float * fftbuf);
float calcst(float * fftbuf);
float calcsc(float * fftbuf, int nover2);
