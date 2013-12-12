#include "hammer.h"
#include <math.h>
#include <stdio.h>

Hammer :: Hammer(float f, float Fs, float m, float K, float p, float Z, float alpha, float v0) 
{
  this->S = 3;
  this->Fs = Fs;
  this->p = p;
  this->K = K;
  this->mi = 1.0/m;
  this->alpha = alpha;
  this->Z2i = 1.0/(2.0*Z);
  this->v0 = v0;
  this->dt = 1.0/(Fs*S);
  this->dti = 1.0/dt;
  this->x = 0;
  this->v = v0;
  this->a = 0.0;
  this->F = 0.0;
  this->upprev = 0;
}

Hammer :: ~Hammer() {
}

float Hammer :: load(float vin) {
  for(int k=0;k<S;k++) {
    float up;
    up = (x>0)?pow(x,p):0;
    float dupdt = (up-upprev)*dti;
    float v1;
    float x1;
    for(int k=0;k<3;k++) {
      F = K*(up+alpha*dupdt);
      if(F<0) F = 0;
      a = -F*mi;
      v1 = v + a * dt;
      x1 = x + (v1-(vin+F*Z2i)) * dt;
      float upnew = (x1>0)?pow(x1,p):0;
      float dupdtnew = (upnew - upprev)/(2*dt);
      float change = dupdtnew - dupdt;
      dupdt = dupdt + (float)0.5*change;
    }
    upprev = up;
    v = v1;
    x = x1;
  }

  return F;
}
