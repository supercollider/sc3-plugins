#ifndef DWGS_H
#define DWGS_H
#include "filter.h"
#include "SC_PlugIn.h"
extern InterfaceTable *ft;
extern World * gWorld;
#include "DWG.hpp"


class dwg_node {
 public:
  dwg_node(float z);
  float z;
  float load;
  float a[2];
   void* operator new(size_t sz){
		return RTAlloc(gWorld, sizeof(dwg_node));
	}
	void operator delete(void* pObject) {
		RTFree(gWorld, (dwg_node*)pObject);
	}
};

class dwgs;

class dwg {
 public:
  dwg(float z, int del1, int del2, int commute, dwgs *parent);
  ~dwg();
   void* operator new(size_t sz){
		return RTAlloc(gWorld, sizeof(dwg));
	}
	void operator delete(void* pObject) {
		RTFree(gWorld, (dwg*)pObject);
	}
  void init();
  void update();
  void dodelay();
  void doload();
  
  void connectLeft(dwg_node *n);
  void connectRight(dwg_node *n);
  void connectLeft(dwg_node *n, int polarity);
  void connectRight(dwg_node *n, int polarity);

  int del1;
  int del2;
  int nl;
  int nr;
  int pl[2];
  int pr[2];
  dwg_node *cl[2];
  dwg_node *cr[2];
  dwg_node *l, *r;
  float loadl, loadr;
  float al[2];
  float ar[2];
  float alphalthis;
  float alpharthis;
  float alphal[2];
  float alphar[2];

  Delay d[2];
  dwgs *parent;
  int commute;
};

class dwgs {
 public:
  dwgs(float f, float Fs, float inpos, float c1, float c3, float B, float Z, float Zb, float Zh,Unit *unit); 
  ~dwgs();
   void* operator new(size_t sz){
		return RTAlloc(gWorld, sizeof(dwgs));
	}
	void operator delete(void* pObject) {
		RTFree(gWorld, (dwgs*)pObject);
	}
   float input_velocity();
   float go_hammer(float load);
   float go_soundboard(float load);

  ThirianT<2> dispersion[4];
  FilterC1C3 lowpass;
  //Filter fracdelay;
  //Thirian *fracdelay2;
  LagrangeT<1024> fracdelay3;
  int M;
  dwg *d[4];
};

#endif
