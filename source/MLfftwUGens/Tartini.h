//This file is part of MachineListening. Copyright (C) 2006  Nicholas M.Collins distributed under the terms of the GNU General Public License full notice in file MachineListening.license


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
	
	fftwf_plan planAutocorrTime2FFT, planAutocorrFFT2Time;
	
	float m_currfreq, m_hasfreq; //storing most recent result
	
	int m_amortisationstate;
};

extern "C" {  
	
	void Tartini_next(Tartini *unit, int inNumSamples);
	void Tartini_Ctor(Tartini* unit);
	void Tartini_Dtor(Tartini* unit);
}


