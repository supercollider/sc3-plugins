#ifndef PIANO_H
#define PIANO_H

float TUNE[3] = { 1, 1.0003, 0.9996 };
#define NUM_NOTES 128

#include "filter.h"
#include "dwgs.h"
#include "reverb.h"
#include "hammer.h"
#include "types.h"

#define nlongres 8
#include "SC_PlugIn.h"
InterfaceTable *ft;
World * gWorld;

class Piano {
 public:
  Piano(float f, float Fs, float v0,World * _world=0);
  ~Piano();
  long go(float *out, int samples);
  float getResampleRatio();
 // static void fillFrequencyTable();
  //static double freqTable[NUM_NOTES];    
	//must pass in world since not stored yet
	struct World * world;
	/*
	void* operator new(size_t sz, World* wd){//, InterfaceTable * ft) {
		return RTAlloc(wd, sizeof(Piano));
	}

	//use stored world to sort this
	void operator delete(void* pObject) {

		Piano * lpc = (Piano*)pObject;
		//InterfaceTable * ft= lpc->ft;

		RTFree(lpc->world, (Piano*)pObject);
	}*/
	void* operator new(size_t sz){
		return RTAlloc(gWorld, sizeof(Piano));
	}
	void operator delete(void* pObject) {
		RTFree(gWorld, (Piano*)pObject);
	}
  float v0;
  //long samples;
  long sample;
  float Fs;
  float t;
  float dt;
  float Z;
  float Zb;
  float Zh;
  float f;
  
  int nstrings;
  dwgs *string[3];
  Hammer *hammer;
  Reverb *soundboard;
  Filter shaping1;
  Filter shaping2;
  Filter shaping3;
};

#endif
