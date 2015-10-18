#include "hammer.h"
#include <math.h>
#include <stdio.h>

Hammer :: Hammer(float f, float Fs, float m, float K, float p, float Z, float alpha, float v0) 
{

  this->Fs = Fs;
  this->p = p;
  this->K = K;
  this->mi = 1.0/m;

  this->Z2i = 1.0/(2.0*Z);

  this->a = 0.0;
  this->F = 0.0;

}
BanksHammer :: BanksHammer(float f, float Fs, float m, float K, float p, float Z, float alpha, float v0) :Hammer(f,Fs,m,K, p,Z,alpha,v0)
{

  intv.Init(Fs*2);
  intvh.Init(Fs*2);
  undel.Init();
  vh = 0.0;
  oldvin = 0.0;
  intvh.go(v0*Fs*2);
}
StulovHammer :: StulovHammer(float f, float Fs, float m, float K, float p, float Z, float alpha, float v0):Hammer(f,Fs,m,K, p,Z,alpha,v0)
{
  this->S = 3;
  this->alpha = alpha;
  this->dt = 1.0/(Fs*S);
  this->dti = 1.0/dt;
  this->x = 0;
  this->v = v0;
  this->upprev = 0;

}

Hammer :: ~Hammer() {
}
BanksHammer :: ~BanksHammer() {
}
void StulovHammer::trigger(float v){
  // this->v0 = v;
   this->v = v;
   this->x = 0.0;
}

float StulovHammer :: load(float vin) {
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
void BanksHammer::trigger(float v){
    this->F = 0.0;
   this->vh = 0.0;
   //intvh.lastout = 0.0;
  intv.Init(Fs*2);
  intvh.Init(Fs*2);
  undel.Init();
   intvh.go(v*Fs*2);
}

float BanksHammer::load(float vin){
    float rvin = (vin + oldvin)*0.5;
    float F1,F2;
    
    float vs = (rvin +F*Z2i);
    float deltaV = vh - vs;
    float deltaY = intv.go(deltaV);
    float up = (deltaY>0.0)?pow(deltaY,p):0.0;
    up = up*K;
    a = -F*mi;
    vh = intvh.go(a);
    F1 = F = undel.go(up);
    
    vs = (vin +F*Z2i);
    deltaV = vh - vs;
    deltaY = intv.go(deltaV);
    up = (deltaY>0.0)?pow(deltaY,p):0.0;
    up = up*K;
    a = -F*mi;
    vh = intvh.go(a);
    F2 = F = undel.go(up);
    
    
    return (F1 + F2)*0.5; 
}
