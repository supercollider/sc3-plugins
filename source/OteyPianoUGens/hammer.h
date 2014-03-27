#ifndef HAMMER_H
#define HAMMER_H
#include <stdio.h>
#include "SC_PlugIn.h"
extern InterfaceTable *ft;
extern World * gWorld;
class Hammer {
public:
  Hammer(float f, float Fs, float m, float K, float p, float Z, float alpha, float v0);
  ~Hammer();
   void* operator new(size_t sz){
		return RTAlloc(gWorld, sizeof(Hammer));
	}
	void operator delete(void* pObject) {
		RTFree(gWorld, (Hammer*)pObject);
	}
  float load( float vin);

  float dt;
  float dti;
  float x;
  float v;
  float a;

  int S;
  float v0;
  float mi;
  float K;
  float p;
  float Fs;
  float F;
  float upprev;
  float alpha;
  float Z2i;
};

#endif
