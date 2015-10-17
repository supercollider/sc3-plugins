#ifndef PIANO_H
#define PIANO_H

float TUNE[3] = { 0, 1, -1 };
#define NUM_NOTES 128

#include "filter.h"
#include "dwgs.h"
#include "reverb.h"
#include "hammer.h"



#define nlongres 8
#include "SC_PlugIn.h"
InterfaceTable *ft;
World * gWorld;

class Piano {
 public:

  Piano(Unit * unit=0);
  ~Piano();

 void init(float f, float Fs, float velocity, float minr,float maxr,float ampr,float centerr, float mult_radius_core_string, float minL,float maxL,float ampL,float centerL, float mult_density_string, float mult_modulus_string, float mult_impedance_bridge, float mult_impedance_hammer, float mult_mass_hammer, float mult_force_hammer, float mult_hysteresis_hammer, float mult_stiffness_exponent_hammer, float position_hammer, float mult_loss_filter,float detune,int hammer_type=1);

  long go(float *out, int samples);
  float getResampleRatio();
  void trigger(float v);

	Unit * unit;
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

  //float amp;
  float v0;
  //long sample;
  float Fs;
  //float t;
  //float dt;
  float Z;
  float Zb;
  float Zh;
  float f;
  
  int nstrings;
  dwgs *string[3];
  Hammer *hammer;
  //Reverb *soundboard;
  //Filter shaping1;
  //Filter shaping2;
  //Filter shaping3;
};

#endif
