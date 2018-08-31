#ifndef REVERB_H
#define REVERB_H
#include "SC_PlugIn.h"
extern InterfaceTable *ft;
extern World * gWorld;
class Reverb {
public:
  Reverb(float c1, float c3, float a, float mix, float Fs);
  ~Reverb();
  float reverb(float in); 
  void* operator new(size_t sz){
		return RTAlloc(gWorld, sizeof(Reverb));
	}
	void operator delete(void* pObject) {
		RTFree(gWorld, (Reverb*)pObject);
	}
  float mix;
  Delay d[8];
  float A[8][8];
  float o[8];
  float b[8];
  float c[8];
  Filter decay[8];
};


#endif
