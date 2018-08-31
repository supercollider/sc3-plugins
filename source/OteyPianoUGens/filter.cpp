#include "filter.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "SC_PlugIn.h"
extern InterfaceTable *ft;
extern World * gWorld;

long choose(long n, long k) {
  long divisor = 1;
  long multiplier = n;
  long answer = 1;
  k = std::min(k,n-k);
  while(divisor <= k)
    {
      answer = (answer * multiplier) / divisor;
      multiplier--;
      divisor++;
    }
  return answer;
}

float Db(float B, float f, int M) {
  float C1,C2,k1,k2,k3;
  if(M==4) {
    C1 = .069618;
    C2 = 2.0427;
    k1 = -.00050469;
    k2 = -.0064264;
    k3 = -2.8743;
  } else {
    C1 = .071089;
    C2 = 2.1074;
    k1 = -.0026580;
    k2 = -.014811;
    k3 = -2.9018;
  }

  float logB = log(B);
  float kd = exp(k1*logB*logB + k2*logB + k3);
  float Cd = exp(C1*logB+C2);
  float halfstep = pow(2.0,1.0/12.0);
  float Ikey = log(f*halfstep/27.5) / log(halfstep);
  float D = exp(Cd - Ikey*kd);
  return D;
}
void complex_divide(float Hn[2], float Hd[2], float H[2]) ;
/*
void complex_divide(float Hn[2], float Hd[2], float H[2]) 
{
  float magn = sqrt(Hn[0]*Hn[0] + Hn[1]*Hn[1]);
  float argn = atan2(Hn[1],Hn[0]);
  float magd = sqrt(Hd[0]*Hd[0] + Hd[1]*Hd[1]);
  float argd = atan2(Hd[1],Hd[0]);
  float mag = magn/magd;
  float arg = argn - argd;
  H[0] = mag*cos(arg);
  H[1] = mag*sin(arg);
}
*/
float groupdelay(Filter *c, float f, float Fs) 
{
  float df = 5;
  float f2 = f + df;
  float f1 = f - df;
  float omega2 = 2*PI*f2/Fs;
  float omega1 = 2*PI*f1/Fs;
  return (omega2*phasedelay(c,f2,Fs) - omega1*phasedelay(c,f1,Fs))/(omega2-omega1);
}

float phasedelay(Filter *c, float f, float Fs) 
{
  float Hn[2];
  float Hd[2];
  float H[2];

  Hn[0] = 0.0; Hn[1] = 0.0;
  Hd[0] = 0.0; Hd[1] = 0.0;

  float omega = 2*PI*f/Fs;
  int N = c->n;
  for(int k=0;k<=N;k++) {
    Hn[0] += cos(k*omega)*c->b[k];
    Hn[1] += sin(k*omega)*c->b[k];
  }
  for(int k=0;k<=N;k++) {
    Hd[0] += cos(k*omega)*c->a[k];
    Hd[1] += sin(k*omega)*c->a[k];
  }
  complex_divide(Hn,Hd,H);
  float arg = atan2(H[1],H[0]);
  if(arg<0) arg = arg + 2*PI;
  
  return arg/omega;
}

void differentiator(Filter *c) 
{
  c->x = (float *)RTAlloc(gWorld,sizeof(float[2]));//new float[2];//(float *)RTAlloc(gWorld,sizeof(float[2]));//
  c->y = (float *)RTAlloc(gWorld,sizeof(float[2]));//new float[2];
  c->a = (float *)RTAlloc(gWorld,sizeof(float[2]));//new float[2];
  c->b = (float *)RTAlloc(gWorld,sizeof(float[2]));//new float[2];
  memset(c->x,0,2*sizeof(float));
  memset(c->y,0,2*sizeof(float));
  
  c->a[0] = 1;
  c->a[1] = 0;
  c->b[0] = 1;
  c->b[1] = -1;
  
  c->n = 1;
}


void resonator(float f, float Fs, float tau, Filter *c) {
  c->x = (float *)RTAlloc(gWorld,sizeof(float[3]));//new float[3];
  c->y = (float *)RTAlloc(gWorld,sizeof(float[3]));//new float[3];
  c->a = (float *)RTAlloc(gWorld,sizeof(float[3]));//new float[3];
  c->b = (float *)RTAlloc(gWorld,sizeof(float[3]));//new float[3];
  memset(c->x,0,3*sizeof(float));
  memset(c->y,0,3*sizeof(float));

  float rp = exp(-1/(tau*Fs));
  float omega = 2*PI*f/Fs;
  c->a[0] = 1;
  c->a[1] = -2*rp*cos(omega);
  c->a[2] = rp*rp;
  c->b[0] = 0;
  c->b[1] = sin(omega);
  c->b[2] = 0;
  c->n = 2;
}

void loss(float f0, float fs, float c1, float c3, Filter *c)
{
  c->x = (float *)RTAlloc(gWorld,sizeof(float[2]));//new float[2];
  c->y = (float *)RTAlloc(gWorld,sizeof(float[2]));//new float[2];
  c->a = (float *)RTAlloc(gWorld,sizeof(float[2]));//new float[2];
  c->b = (float *)RTAlloc(gWorld,sizeof(float[2]));//new float[2];
  memset(c->x,0,2*sizeof(float));
  memset(c->y,0,2*sizeof(float));
  
  float g = 1.0 - c1/f0; 
  float b = 4.0*c3+f0;
  float a1 = (-b+sqrt(b*b-16.0*c3*c3))/(4.0*c3);
  c->b[0] = g*(1+a1);
  c->b[1] = 0;
  c->a[0] = 1;
  c->a[1] = a1;

  c->n = 1;
}
/*/
void biquad(float f0, float fs, float Q, int type, Filter *c)
{
  c->x = (float *)RTAlloc(gWorld,sizeof(float[3]));//new float[3];
  c->y = (float *)RTAlloc(gWorld,sizeof(float[3]));//new float[3];
  c->a = (float *)RTAlloc(gWorld,sizeof(float[3]));//new float[3];
  c->b = (float *)RTAlloc(gWorld,sizeof(float[3]));//new float[3];
  memset(c->x,0,3*sizeof(float));
  memset(c->y,0,3*sizeof(float));

  float a = 1/(2*tan(PI*f0/fs));
  float a2 = a*a;
  float aoQ = a/Q;
  float d = (4*a2+2*aoQ+1);

  c->a[0] = 1;
  c->a[1] = -(8*a2-2) / d;
  c->a[2] = (4*a2 - 2*aoQ + 1) / d;
  
  switch(type) {
  case pass:
    c->b[0] = 2*aoQ/d;
    c->b[1] = 0;
    c->b[2] = -2*aoQ/d;
    break;
  case low:
    c->b[0] = 1/d;
    c->b[1] = 2/d;
    c->b[2] = 1/d;
    break;
  case high: 
    c->b[0] = 4*a2/d;
    c->b[1] = -8*a2/d;
    c->b[2] = 4*a2/d;
    break;
  case notch:
    c->b[0] = (1+4*a2)/d;
    c->b[1] = (2-8*a2)/d;
    c->b[2] = (1+4*a2)/d;
    break;
  }

  c->n = 2;
}
*/
void thirian(float D, int N, Filter *c) 
{
  c->x =(float *)RTAlloc(gWorld, sizeof(float) * (N + 1));// new float[N+1];
  c->y = (float *)RTAlloc(gWorld, sizeof(float) * (N + 1));//new float[N+1];
  c->a = (float *)RTAlloc(gWorld, sizeof(float) * (N + 1));//new float[N+1];
  c->b = (float *)RTAlloc(gWorld, sizeof(float) * (N + 1));//new float[N+1];
  memset(c->x,0,(N+1)*sizeof(float));
  memset(c->y,0,(N+1)*sizeof(float));

  for(int k=0;k<=N;k++) {
    double ak = (float)choose((long)N,(long)k);
    if(k%2==1)
      ak = -ak;
    for(int n=0;n<=N;n++) {
      ak *= ((double)D-(double)(N-n));
      ak /= ((double)D-(double)(N-k-n));
    }
    c->a[k] = (float)ak;
    c->b[N-k] = (float)ak;
  }
  c->n = N;
}

void thiriandispersion(float B, float f, int M, Filter *c)
{
  int N = 2;
  float D;
  D = Db(B,f,M);

  if(D<=1.0) {
	c->x = (float *)RTAlloc(gWorld, sizeof(float) * (N + 1));//new float[N+1];
	c->y = (float *)RTAlloc(gWorld, sizeof(float) * (N + 1));//new float[N+1];
	c->a = (float *)RTAlloc(gWorld, sizeof(float) * (N + 1));//new float[N+1];
	c->b = (float *)RTAlloc(gWorld, sizeof(float) * (N + 1));//new float[N+1];
    memset(c->x,0,(N+1)*sizeof(float));
    memset(c->y,0,(N+1)*sizeof(float));
    c->a[0] = 1;
    c->a[1] = 0;
    c->a[2] = 0;
    c->b[0] = 1;
    c->b[1] = 0;
    c->b[2] = 0;
  } else {
    thirian(D,N,c);
  }
}

void destroy_filter(Filter *c) {
  //delete c->a;
  //delete c->b;
  //delete c->x;
  //delete c->y;
  RTFree(gWorld,c->a);
  RTFree(gWorld,c->b);
  RTFree(gWorld,c->x);
  RTFree(gWorld,c->y);
}

float filter(float in, Filter *c) 
{
  float *a = c->a;
  float *b = c->b;
  int n = c->n;
  float *x = c->x + n;
  float *y = c->y + n;
  float *xend = c->x;

  while(x != xend) {
    *(x) = *(x-1);
    *(y) = *(y-1);
    x--;
    y--;
  }
  *x = in;
  float out = *(b) * *(x);  
  b++;
  a++;
  x++;
  y++;
  xend = x+c->n; 

  while(x != xend) {
    out += *(b) * *(x);
    out -= *(a) * *(y);
    b++;
    a++;
    x++;
    y++;
  }
  *(c->y) = out;

  return out;
}


void init_delay(Delay *c, int di)
{
  // turn size into a mask for quick modding
  //c->size = 2*di;
  /*
  int p = 0;
  while(c->size) {
    c->size /= 2;
    p++;
  }
  c->size = 1;
  while(p) {
    c->size *= 2;
    p--;
  }
  c->mask = c->size - 1;
	*/
	c->size = NEXTPOWEROFTWO(2*di);
	c->mask = c->size - 1;
  c->x = (float *)RTAlloc(gWorld,sizeof(float) * c->size);//new float[c->size];
  //c->y = new float[c->size];
  memset(c->x,0,c->size*sizeof(float));
 // memset(c->y,0,c->size*sizeof(float));

  c->cursor = 0;
  c->di = di;
  c->d1 = (c->size-di)%c->size;
}

//float probe_delay(Delay *c, int pos) {
//  return c->y[(c->cursor-pos+c->size)%(c->size)];
//}

void destroy_delay(Delay *c)
{
  RTFree(gWorld,c->x);//delete c->x;
  
  //delete c->y;
}

float delay(float in, Delay *c)
{
  int cursor = c->cursor;
  int d1 = c->d1;
  float y0 = c->x[d1];
  //c->y[cursor] = y0;
  c->x[cursor] = in;
  c->d1++;
  c->d1 &= c->mask;
  c->cursor++;
  c->cursor &= c->mask;
  return y0;
}
