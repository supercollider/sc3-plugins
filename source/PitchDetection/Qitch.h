
struct Qitch : Unit {
	
	//FFT data
	int m_bufWritePos;
	float * m_prepareFFTBuf;
	float * m_FFTBuf;
	
	scfft *m_scfft; 

	//Q data
	
	//FFT constants
	//int m_SR, m_N; //not used, just have globals
	int m_SR;
	int m_Nyquist;
	int m_N; 
	int m_log2N;
	int m_Nover2;
	int m_overlap;
	int m_overlapindex;
	float m_framespersec;
	float m_fftscale;
	float m_freqperbin;
	
	//constants for efficiency
	float m_twopioverN;
	float realb,imagb;
	
	int m_qbands;
	float * m_qfreqs;
	int * m_startindex;
	int * m_numindices;
	//int * m_cumulindices;
	float ** m_speckernelvals; //pointers into buffer data
	
	float * m_qmags;
	
	float m_amps[11];
	
	//instantaneous frequency tracking 
	int m_topqcandidate;
	int m_ifbins;
	
	float m_currfreq, m_hasfreq;
	
	
	float m_minfreq, m_maxfreq;
	int m_minqband, m_maxqband;
	
	
};

extern "C"
{
	//required interface functions
	void Qitch_next(Qitch *unit, int wrongNumSamples);
	void Qitch_Ctor(Qitch *unit);
	void Qitch_Dtor(Qitch *unit);
}
