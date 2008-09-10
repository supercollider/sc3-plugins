//This file is part of MachineListening. Copyright (C) 2006  Nicholas M.Collins distributed under the terms of the GNU General Public License full notice in file MachineListening.license


//WARNING- if N was changed here, the Hanning window is pre created for 1024 samples, hanning1024, you'd need to change that too, and also as refed in the files
//FFT data 
#define N 1024  //FFT size
#define NOVER2 512  //FFT size
#define NOVER4 256  //FFT size
#define OVERLAP 512
#define OVERLAPINDEX 512
#define HOPSIZE 512  
#define FS 44100 //assumes fixed sampling rate
#define FRAMESR 86.1328
#define FRAMEPERIOD 0.01161 //seconds

#define DFFRAMELENGTH 512
#define DFSTORE 700
//store last 700 to avoid problems during amortisation

#define SKIP 128
#define TIMEELAPSED 1.48608

#define LAGS 128


struct AutoTrack : Unit {
	
	//FFT data
	int m_bufWritePos;
	float * m_prepareFFTBuf;
	float * m_FFTBuf;
	
	fftwf_plan planTime2FFT; 
	
	float * m_prevmag;
	float * m_prevphase;
	float * m_predict;
	
	//vDSP
	//unsigned long m_vlog2n;
	//COMPLEX_SPLIT m_vA;
	//FFTSetup m_vsetup;
	
	
	//time positions
	long m_frame;
	
	//df
	float m_df[DFSTORE];
	int m_dfcounter;
	
	//for peak pick scorer
	int m_dfmemorycounter;
	float m_dfmemory[15];
	
	
	//autocorrelation results on df
	float m_acf[DFFRAMELENGTH];
	
	//float* m_M;
	float m_mg[LAGS];
	float m_besttorsum;
	int m_bestcolumn;
	
	float m_phaseweights[LAGS];
	
	float m_tor;
	int m_torround;
	
	float m_periodp;
	float m_periodg;
	
	int m_flagstep;
	float m_prevperiodp[3];
	
	//amortisation search for best phase from 0 to m_torround-1
	float m_bestphasescore;
	int m_bestphase;
	
	//tempo
	float m_currtempo;
	
	//phase
	float m_currphase;
	
	//phasor, trigger beat and modulo when >1.0
	float m_phase, m_phaseperblock;
	
	//phasor output separate so can have it lock and keep going when don't want to track
	float m_outputphase, m_outputtempo, m_outputphaseperblock;
	
	int halftrig;
    int q1trig;
	int q2trig;
	
	//amortization - more complex structure to support different rates of work
	int m_amortisationstate;
	int m_amortcount;
	int m_amortlength;
	int m_amortisationsteps;
	
	//model states
	int m_stateflag;
	int m_timesig;
	
	int m_storedfcounter;
	int m_storedfcounterend;	
	
	
};



extern "C"
{
	//required interface functions
	void AutoTrack_next(AutoTrack *unit, int wrongNumSamples);
	void AutoTrack_Ctor(AutoTrack *unit);
	void AutoTrack_Dtor(AutoTrack *unit);
}

