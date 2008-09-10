//This file is part of MachineListening. Copyright (C) 2006  Nicholas M.Collins distributed under the terms of the GNU General Public License full notice in file MachineListening.license

//all constants like N, OVELAP etc already in AutoTrackfftw.h
/*
//FFT data 
#define N 1024  //FFT size
#define NOVER2 512  //FFT size
#define NOVER4 256  //FFT size
#define OVERLAP 512
//768  //512
#define OVERLAPINDEX 512 
//256 //512
#define HOPSIZE 512 
//256 // 512  
#define FS 44100 //assumes fixed sampling rate
#define FRAMESR  86.1328   
//172.2656    //86.1328
#define FRAMEPERIOD 0.01161
//0.00581 //0.01161 //seconds
*/



#define NUMERBBANDS 40
#define PASTERBBANDS 3
//3 usually, but time resolution improved if made 1?

//in FFT frames
#define MAXEVENTDUR 80
#define MINEVENTDUR 3
//4 or maybe 2

#define CIRCBUFSIZE 15

//7 frames is about 40 mS
//peak picker will use 3 back, 3 forward
#define DFFRAMESSTORED 7

#define MAXBLOCKSIZE 64
#define MAXBLOCKS 700

//MAXEVENTDUR+20 for safety
#define LOUDNESSSTORED 100





struct AnalyseEvents2 : Unit {
	
	//FFT data
	int m_bufWritePos;
	float * m_prepareFFTBuf;
	float * m_FFTBuf;
	
	
	fftwf_plan planTime2FFT; 
	
	//vDSP
	//unsigned long m_vlog2n;
	//COMPLEX_SPLIT m_vA;
	//FFTSetup m_vsetup;
	
	//time positions
	long m_frame;
	long m_lastdetect;
	
	//loudness measure
	float m_loudbands[NUMERBBANDS][PASTERBBANDS]; //stores previous loudness bands
	int m_pasterbbandcounter;
	float m_df[DFFRAMESSTORED]; //detection function buffer (last 5)
	int m_dfcounter; 
	
	float m_loudness[LOUDNESSSTORED];   //store loudness, sent back with triggers for determining accents (probably not that useful since not measured at peak)
	int m_loudnesscounter; 
	
	//recording state
	int m_onsetdetected;
	int m_recording;
	int m_start;
	float m_testintensity; //end can trigger if drop below this
	
	//into global frame buffer
	int m_numframes;
	int m_startframe;
	int m_endframe;
	
	int m_startblock;
	int m_endblock;
	
	//into loudness frame buffer
	int m_lstartframe;
	int m_lendframe;
	
	//into pitch frame buffer
	
	//triggers- greater than 0, send trigger message with that ID
	int m_triggerid;
	
	//target buffer for event data
	uint32 m_bufNum, m_bufSize;
	int m_maxEvents; //, m_lastEvent; //maximum that can be stored
	float * m_eventData; //[0] position stores num of events stored
	int m_numstored;
	
	//different structure to buffer saving for m_circular = true
	//support for circular buffers and on-the-fly event capture
	int m_circular; //act as if a circular buffer, uses m_numstored for which to write/pick up next
					//SendTrigger on a detected event giving place to pick up
	
	//pause just by pausing detection Synth (RecordBuf paused at the same time)
	//int m_pause;	//pause recording, do not store any detected events, zero counts as go round, but Lang side 
	
	
	uint32 m_now;
	
	//int m_maxblocksstored; //since FS fixed at 44100, fix this too
	float * m_maxintensity;
	int m_maxcount;
	float * m_store;
	int m_storecounter;
	
	//uses maxcount
	float * m_pitch;
	float * m_sortbuf;
	float m_patband1[LOUDNESSSTORED]; 
	float m_patband2[LOUDNESSSTORED]; 
	
	int m_featurecounter;
	double m_featurevector[20];
	
	double m_timbreFV[12];
	float m_zcr[LOUDNESSSTORED];
	float m_speccentroid[LOUDNESSSTORED]; 
	
		
	//evaluation- write beat locations to a buffer
//	uint32 m_bufNum2, m_bufSize2;
//	//int m_maxbeats, 
//	int m_dfsaved;
//	float * m_dfData; 
//	//uint32 m_now, m_then;
//	//uint32 m_lastbeattime;
//
//	
};



extern "C"
{
	//required interface functions
	void AnalyseEvents2_next(AnalyseEvents2 *unit, int wrongNumSamples);
	void AnalyseEvents2_Ctor(AnalyseEvents2 *unit);
	void AnalyseEvents2_Dtor(AnalyseEvents2 *unit);
}

